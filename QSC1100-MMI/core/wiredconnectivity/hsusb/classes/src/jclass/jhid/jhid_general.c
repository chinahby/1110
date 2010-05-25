/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include <jos.h>
#include <jusb.h>
#include <usbdi.h>
#include <usbdi_util.h>
#include "jhid_parser.h"
#include "jhid_util.h"
#include <uw_host_hid_common.h>
#include <uw_host_hid.h>
#include "jhid_general.h"
#ifdef CONFIG_JHOST_HID_XBOX360_GAMEPAD
#include "jxbox360.h"
#endif
#include "jboot_report_desc.h"

#ifdef JDEBUG
#define find_report(sc, report_id) find_report_d(sc, report_id, __LINE__);
#endif

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

static jresult_t jhid_general_app_enable(void *context);
static void jhid_general_app_disable(void *context);
static void jhid_general_intr(usbd_xfer_handle xfer, usbd_private_handle addr, 
    usbd_status status);

TAILQ_HEAD(_hid_devices, jhid_general_softc) hid_devices = 
    TAILQ_HEAD_INITIALIZER(hid_devices);
TAILQ_HEAD(_hid_handlers, jhid_gen_dev) hid_handlers = 
    TAILQ_HEAD_INITIALIZER(hid_handlers);

static jresult_t jhid_general_probe(jdevice_t self);
static jresult_t jhid_general_attach(jdevice_t self);
static jresult_t jhid_general_detach(jdevice_t self);
static jresult_t jhid_general_suspend(jdevice_t self);
static jresult_t jhid_general_resume(jdevice_t self);

char jhid_general_name []={'H','I','D', ' ', 'G', 'e', 'n', 'e', 'r', 'a', 'l',
    '\0'};

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#ifdef CONFIG_TEST_SELECTIVE_SUSPEND
static void jhid_test_selective_suspend_task(void *arg)
{
    jresult_t rc;
    static jbool_t is_suspend = 1;
    jhid_general_softc_t *sc = (jhid_general_softc_t *)arg;
    
    DBG_E(DHOST_HID_POWER, ("Test selective suspend: going to %s %p\n",
        is_suspend ? "SUSPEND" : "RESUME", sc->sc_dev));
    
    if (is_suspend)
        rc = hid_general_suspend_device(sc);
    else
        rc = hid_general_resume_device(sc);

    if (rc)
    {
        DBG_E(DHOST_HID_POWER, ("Test selective suspend: %s failed (0x%x)\n",
            is_suspend ? "SUSPEND" : "RESUME", rc));
    }
    else
    {
        is_suspend = !is_suspend;
        jtask_schedule(sc->power_task, 15 * 1000, 
            jhid_test_selective_suspend_task, sc);
    }
}
#endif

#ifdef CONFIG_JHOST_HID_XBOX360_GAMEPAD
static __INLINE__ jbool_t is_xbox_gamepad(struct usb_attach_arg *uaa)
{
    usb_interface_descriptor_t *id = usbd_get_interface_descriptor(uaa->iface);
    usb_device_descriptor_t *dd = usbd_get_device_descriptor(uaa->device);

    DBG_V (DHOST_HID_GENERAL, ("Vendof=0x%x, InterfClass = %d "
        "subClass = %d Protocol = %d\n", UGETW(dd->idVendor),
        id->bInterfaceClass, id->bInterfaceSubClass, 
        id->bInterfaceProtocol));

    if (dd && (XBOX360_GAMEPAD_VID == UGETW(dd->idVendor)) &&
        id &&
        (id->bInterfaceClass == XBOX360_GAMEPAD_IF_CLASS) &&
        (id->bInterfaceSubClass == XBOX360_GAMEPAD_IF_SUB_CLASS) &&
        (id->bInterfaceProtocol == XBOX360_GAMEPAD_IF_PROT))
    {
        return 1;
    }
    return 0;
}
#endif

static __INLINE__ juint32_t get_boot_quirk_type(struct usb_attach_arg *uaa)
{
    juint32_t quirk_type = JHID_NO_QUIRK;
    usb_interface_descriptor_t *id = usbd_get_interface_descriptor(uaa->iface);

    DBG_I(DHOST_HID_GENERAL, ("InterfClass = %d "
        "subClass = %d Protocol = %d\n", id->bInterfaceClass, 
        id->bInterfaceSubClass, id->bInterfaceProtocol));

    if (id && (id->bInterfaceClass == UICLASS_HID) &&
        (id->bInterfaceSubClass == UISUBCLASS_BOOT))
    {
        if (id->bInterfaceProtocol == UIPROTO_BOOT_KEYBOARD)
            quirk_type = JHID_FORCE_BOOT_KBD;
        else if (id->bInterfaceProtocol == UIPROTO_BOOT_MOUSE)
            quirk_type = JHID_FORCE_BOOT_MOUSE;
    }

    DBG_I(DHOST_HID_GENERAL, ("The quirk set to be %d\n", quirk_type));

    return quirk_type;
}

static jresult_t jhid_general_probe(jdevice_t self)
{
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    usb_interface_descriptor_t *id;
    jresult_t rt = UMATCH_NONE;
    DBG_I(DHOST_HID_GENERAL, ("HID_GEN PROBE: Probe Started\n"));
    
    if (!uaa->iface)    /* we attach to ifaces only */
        goto Exit;

    /* Check that this is a HID device. */
    id = usbd_get_interface_descriptor(uaa->iface);
    if (id && id->bInterfaceClass == UICLASS_HID)
        rt = UMATCH_IFACECLASS; /* found it */

#ifdef CONFIG_JHOST_HID_XBOX360_GAMEPAD
    if (is_xbox_gamepad(uaa))
        rt = UMATCH_VENDOR_DEVCLASS_DEVPROTO;
#endif

    DBG_I(DHOST_HID_GENERAL, ("HID_GEN PROBE: done %d\n", rt));
Exit:
    return rt;
}

static jresult_t jhid_general_attach(jdevice_t self)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)j_device_get_softc(self);
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    jhid_gen_dev_t * tmp_handle = NULL;

    juint8_t num_endpoints = 0;
    juint8_t index = 0;
    usb_endpoint_descriptor_t *ed;
    usb_hid_descriptor_t *hid_desc = NULL;
    void *report_descriptor = NULL;
    juint16_t report_descriptor_size = 0;
    jhid_report_t *report = NULL;
    juint8_t rep_id = 0;
    juint8_t i;
    jint_t num_reports;
    jresult_t rc = JENXIO;

    sc->sc_dev = self;
    sc->iface = uaa->iface;
    sc->dev = uaa->device;
    sc->state = HID_NOT_CONNECTED;
    sc->hid_quirks = JHID_NO_QUIRK;
    TAILQ_INIT(&sc->report_list);

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    DBG_I(DHOST_HID_GENERAL, ("HID_GEN ATTACH: Attach Started\n"));

    /* set boot quirk for kbd and mouse */
    sc->hid_quirks = get_boot_quirk_type(uaa);

#ifdef CONFIG_JHOST_HID_XBOX360_GAMEPAD
    /* If the attached HID device is XBOX360 Gamepad all other quirks are
     * irrelevant */
    if (is_xbox_gamepad(uaa))
        sc->hid_quirks = JHID_XBOX360_NO_REPORT_D;
#endif

    if (usbd_endpoint_count(sc->iface, &num_endpoints))
    {
        DBG_E(DHOST_HID_GENERAL,("HID_GEN ATTACH: Invalid Interface "
            "Descriptor\n"));
        goto Error;
    }

    for (index = 0; index < num_endpoints; ++index)
    {
        ed = usbd_interface2endpoint_descriptor(sc->iface, index);
        if (!ed)
        {
            DBG_E(DHOST_HID_GENERAL, ("HID_GEN ATTACH: could not read endpoint "
                "descriptor num %d / %d\n", index, num_endpoints));
            goto Error;
        }
        
        if (UE_GET_XFERTYPE(ed->bmAttributes) == UE_INTERRUPT)
        {
            if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_IN)
                sc->ep_addr_in = ed->bEndpointAddress;
            else 
                sc->ep_addr_out = ed->bEndpointAddress;
        }
    }
    
    if (sc->ep_addr_in == 0)
    {
        DBG_E(DHOST_HID_GENERAL, ("HID_GEN ATTACH: The HID device without "
            "INTERRUPT IN pipe not allowed\n"));
        goto Error;
    }
    
    DBG_V(DHOST_HID_GENERAL, ("HID_GEN ATTACH: found EP_IN (%d) %s (%d)\n",
        sc->ep_addr_in, (sc->ep_addr_out ? "and EP_OUT":"/"), sc->ep_addr_out));

    hid_desc = usbd_get_hid_descriptor(sc->iface);
    if (hid_desc == NULL)
    {
        DBG_E(DHOST_HID_GENERAL,
            ("HID_GEN ATTACH: Unable to get HID descriptor\n"));
        goto Error;
    }
    
    switch (sc->hid_quirks)
    {
#ifdef CONFIG_JHOST_HID_XBOX360_GAMEPAD
    case JHID_XBOX360_NO_REPORT_D:
        hid_desc->bNumDescriptors = 1;
        break;
#endif
    case JHID_FORCE_BOOT_KBD:
        sc->boot_protocol_report.report_descriptor = 
            (void *)&boot_kbd_report_d;
        sc->boot_protocol_report.report_descriptor_size = 
            sizeof(boot_kbd_report_d);

        report = &sc->boot_protocol_report;

        DBG_I(DHOST_HID_GENERAL, ("HID_GEN ATTACH: Setting predefined boot "
            "protocol for keyboard\n"));

        break;
    case JHID_FORCE_BOOT_MOUSE:
        sc->boot_protocol_report.report_descriptor = 
            (void *)&boot_mouse_report_d;
        sc->boot_protocol_report.report_descriptor_size = 
            sizeof(boot_mouse_report_d);
        
        report = &sc->boot_protocol_report;
        
        DBG_I(DHOST_HID_GENERAL, ("HID_GEN ATTACH: Setting predefined boot "
            "protocol for mouse\n"));

        break;
    default:
        break;
    }

    if (report)
    {
        sc->hid_quirks = JHID_NO_QUIRK;
        report->report_id = 0;

        report->in_report_size = hid_report_size(report->report_descriptor, 
            report->report_descriptor_size, hid_input, 0);

        report->out_report_size = hid_report_size(report->report_descriptor,
            report->report_descriptor_size, hid_output, 0);

        report->feature_report_size = 0; 
        report->filled_index = 0;
        report->readed_index = 0;

        sc->max_report_size = report->in_report_size;

        report->in_report = jmalloc(
            report->in_report_size * REPORT_POOL_SIZE, M_ZERO);
        if (!report->in_report)
        {
            DBG_E(DHOST_HID_GENERAL,("HID_GEN ATTACH: Out of memory "
                "%d\n", index));
            rc = JENOMEM;
            goto Error;
        }
    }

    DBG_V(DHOST_HID_GENERAL, ("HID_GEN ATTACH: Going to fetch and parse %d "
        "reports descriptors \n", hid_desc->bNumDescriptors));

    for (index = 0; index < hid_desc->bNumDescriptors; ++index)
    {
        DBG_V(DHOST_HID_GENERAL, ("HID_GEN ATTACH: Fetchng report descriptor: "
            "%d\n", index));

        switch (sc->hid_quirks)
        {
#ifdef CONFIG_JHOST_HID_XBOX360_GAMEPAD
        case JHID_XBOX360_NO_REPORT_D:
           report_descriptor = (void *)&xbox360report_d;
           report_descriptor_size = sizeof(xbox360report_d);
           break;
#endif
        default:
        if (usbd_read_report_desc(sc->iface, &report_descriptor, 
            &report_descriptor_size, index))
        {
            DBG_E(DHOST_HID_GENERAL,("HID_GEN ATTACH: Unable to get report "
                "descriptor %d\n", index));
            goto Error;
        }
            break;
        }

        num_reports = hid_report_count((void *)report_descriptor, 
            report_descriptor_size);
        
        DBG_V(DHOST_HID_GENERAL, ("HID_GEN ATTACH: %d reports found in the "
            "report descriptor\n", num_reports));

        for (i = 0; i < num_reports; ++i)
        {
            report = (jhid_report_t *)jmalloc(sizeof(jhid_report_t), M_ZERO);
            
            if (report == NULL)
            {
                DBG_E(DHOST_HID_GENERAL,("HID_GEN ATTACH: Out of memory %d\n", 
                    index));
                if (sc->hid_quirks == JHID_NO_QUIRK)
                    jfree(report_descriptor);

                rc = JENOMEM;        
                goto Error;
            }

            report->report_descriptor = jmalloc(report_descriptor_size, M_ZERO);
            if (!report->report_descriptor)
            {
                DBG_E(DHOST_HID_GENERAL,("HID_GEN ATTACH: Out of memory %d\n", 
                    index));
                jfree(report);
                if (sc->hid_quirks == JHID_NO_QUIRK)
                    jfree(report_descriptor);
                
                rc = JENOMEM;                     
                goto Error;
            }

            j_memcpy(report->report_descriptor, report_descriptor, 
                report_descriptor_size);
            report->report_descriptor_size = report_descriptor_size;

            report->report_id = rep_id = hid_report_id_by_index(
                report->report_descriptor, report->report_descriptor_size, i);

            report->in_report_size = hid_report_size(report->report_descriptor, 
                report->report_descriptor_size, hid_input, rep_id);
                
            report->out_report_size = hid_report_size(report->report_descriptor,
                report->report_descriptor_size, hid_output, rep_id);

            report->feature_report_size = hid_report_size(
                report->report_descriptor, report->report_descriptor_size,
                hid_feature, rep_id);  

            DBG_V(DHOST_HID_GENERAL, ("HID_GEN ATTACH: Report descriptor: %d \n"
                "report id: %d, in report size: %d, out report size: %d, "
                "feature report size: %d \n", index, report->report_id, 
                report->in_report_size, report->out_report_size, 
                report->feature_report_size));

            report->filled_index = 0;
            report->readed_index = 0;

            if (report->in_report_size)
            {
            report->in_report = jmalloc(
                report->in_report_size * REPORT_POOL_SIZE, M_ZERO);
            if (!report->in_report)
            {
                    DBG_E(DHOST_HID_GENERAL,("HID_GEN ATTACH: Out of memory "
                        "%d\n", index));
                    jfree(report->report_descriptor);
                    jfree(report);
                    rc = JENOMEM;                     
                    goto Error;
                }
            }

            TAILQ_INSERT_TAIL(&sc->report_list, report, next_report);
            sc->max_report_size = MAX(sc->max_report_size, 
                report->in_report_size);
            sc->num_of_reports++;
        }
        if (sc->hid_quirks == JHID_NO_QUIRK)
        jfree(report_descriptor);
        report_descriptor = NULL;
        report_descriptor_size = 0;
    }

    DBG_V(DHOST_HID_GENERAL, ("HID_GEN_ATTACH: Max IN report Size %d\n",
        sc->max_report_size));
    if (sc->max_report_size)
    {
    sc->in_data = (juint8_t*)jmalloc(sc->max_report_size, M_ZERO);
    if (!sc->in_data)
    {
            DBG_E(DHOST_HID_GENERAL,("HID_GEN ATTACH: Out of memory "
                "(in_data)\n"));
            rc = JENOMEM;                     
            goto Error;
        }
    }

    sc->state = HID_CONNECTED;
    sc->hid_handle = NULL;
    TAILQ_INSERT_TAIL(&hid_devices, sc, next_device);

    TAILQ_FOREACH(tmp_handle, &hid_handlers, next_handle)
    {
        sc->state = HID_ATTACHED;
        sc->hid_handle = tmp_handle;
        if (tmp_handle->notify_callback->notify_attach(tmp_handle->context, sc))
        {
            /* We found hid class driver handler */
            break;
        }
        sc->state = HID_CONNECTED;
        sc->hid_handle = NULL;
    }
    
#ifdef CONFIG_TEST_SELECTIVE_SUSPEND
    if (jtask_init(&sc->power_task, TASK_CORE))
    {
        DBG_E(DHOST_HID_GENERAL, ("HID_GEN_ATTACH: Failed to initialize test "
            "power task\n"));
        goto Error;
    }

    jtask_schedule(sc->power_task, 15 * 1000, jhid_test_selective_suspend_task,
        sc);
#endif
    return 0;

Error:
    jhid_general_detach(self);

    return rc;
}

static jresult_t jhid_general_detach(jdevice_t self)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)j_device_get_softc(self);
    juint8_t removed = 0;
    jhid_gen_dev_t *hid_handle = sc->hid_handle;
    jhid_report_t *tmp_rep;

    /* Notify detach event to upper level */
    if (hid_handle && hid_handle->notify_callback->notify_detach)
        hid_handle->notify_callback->notify_detach(hid_handle->context, sc);

    sc->state = HID_NOT_CONNECTED;

    TAILQ_FIND_AND_REMOVE(&hid_devices, sc, jhid_general_softc_t*, next_device,
        removed);
        
#ifdef CONFIG_TEST_SELECTIVE_SUSPEND
    if (sc->power_task)
        jtask_uninit(sc->power_task);
#endif

    tmp_rep = TAILQ_FIRST(&sc->report_list); 
    while (tmp_rep)
    {
        TAILQ_REMOVE(&sc->report_list, tmp_rep, next_report);
        jfree(tmp_rep->report_descriptor);
        if (tmp_rep->in_report)
        jfree(tmp_rep->in_report);
        jfree(tmp_rep);
        tmp_rep = TAILQ_FIRST(&sc->report_list); 
    }

    if (sc->in_data)
        jfree(sc->in_data);

    tmp_rep = &sc->boot_protocol_report;
    if (tmp_rep && tmp_rep->in_report)
            jfree(tmp_rep->in_report);

    TAILQ_INIT(&sc->report_list);

    return 0;
}

static jresult_t jhid_general_app_enable(void *context)
{
    jresult_t rc = 0;
    jhid_general_softc_t *sc = (jhid_general_softc_t *)context;
    
    if (sc->state != HID_ATTACHED)
    {
        DBG_E(DHOST_HID_GENERAL,("HID_GEN: ENABLE in wrong state %s\n", 
            sc->state == HID_NOT_CONNECTED ? "NOT_ATTACHED":
            (sc->state == HID_CONNECTED ? "ATTACHED":"ENABLED")));
        rc = JEINVAL;
        goto Error;
    }

    /* Open INTR IN pipe only if needed */
    if (sc->in_data && sc->max_report_size)
    {
    if (usbd_open_pipe_intr(sc->iface, sc->ep_addr_in, USBD_SHORT_XFER_OK, 
        &sc->in_intr_pipe, sc, sc->in_data, 
        sc->max_report_size, jhid_general_intr, USBD_DEFAULT_INTERVAL))
    {
            DBG_E(DHOST_HID_GENERAL,("HID_GEN ENABLE: Unable to open interrupt "
                "IN pipe\n"));
        rc = JEINVAL;
        goto Error;
    }
    }

    /* Open INTR OUT pipe only if exist*/
    if (sc->ep_addr_out)
    {
        DBG_V(DHOST_HID_GENERAL,("HID_GEN ENABLE: Opening interrupt OUT pipe"
            "\n"));
        if (usbd_open_pipe(sc->iface, sc->ep_addr_out, USBD_EXCLUSIVE_USE, 
            &sc->out_intr_pipe))
        {
            DBG_E(DHOST_HID_GENERAL,("HID_GEN ENABLE: Unable to open interrupt "
                "OUT pipe\n"));
            rc = JEINVAL;
            goto Error;
        }
    }

    sc->state = HID_ENABLED;
    DBG_I(DHOST_HID_GENERAL,("HID_GEN ENABLE: HID GEN enabled\n"));
    goto Exit;

Error:
    jhid_general_app_disable(context);

Exit:
    return rc;
}

static void jhid_general_app_disable(void *context)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)context;

    if (sc->in_intr_pipe)
    {
        usbd_abort_pipe(sc->in_intr_pipe);
        usbd_close_pipe(sc->in_intr_pipe);
        sc->in_intr_pipe = NULL;
    }

    if (sc->out_intr_pipe)
    {
        usbd_abort_pipe(sc->out_intr_pipe);
        usbd_close_pipe(sc->out_intr_pipe);
        sc->out_intr_pipe = NULL;
    }

    if (sc->state == HID_ENABLED)
        sc->state = HID_ATTACHED;
    DBG_I(DHOST_HID_GENERAL,("HID_GEN DISABLE: HID GEN disabled\n"));
}
#ifdef JDEBUG
static __INLINE__ jhid_report_t * find_report_d(jhid_general_softc_t *sc, 
    juint8_t report_id, juint32_t line)
#else
static __INLINE__ jhid_report_t * find_report(jhid_general_softc_t *sc, 
    juint8_t report_id)
#endif
{
    jhid_report_t *report;
    
    if (sc->is_boot_protocol)
    {
        report = &sc->boot_protocol_report;
        DBG_I(DHOST_HID_GENERAL,("find report: boot protocol report %d\n", 
            line));
    }
    else
    {
        report = TAILQ_FIRST(&sc->report_list); 
        while (report && report->report_id != report_id)
            report = TAILQ_NEXT(report, next_report);
        
        DBG_I(DHOST_HID_GENERAL,("find report: report protocol %d\n", line));
    }

    return report;
}

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

static void jhid_general_intr(usbd_xfer_handle xfer, usbd_private_handle addr, 
    usbd_status status)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *) addr;
    juint8_t report_id = 0;
    jhid_report_t *report;
    juint8_t notify = 0;
    juint8_t *tmp_ptr = NULL;
    
    DBG_V(DHOST_HID_GENERAL,("HID_GEN INTR: status = %d\n",status));
    
    if (status == USBD_CANCELLED)
    {
        DBG_I(DHOST_HID_GENERAL,("HID_GEN INTR: Interrupt pipe cannceled\n"));
        goto Exit;
    }
    
    if (status != USBD_NORMAL_COMPLETION) 
    {
        DBG_E(DHOST_HID_GENERAL,("HID_GEN INTR: status=%d\n", status));
        if (status == USBD_STALLED)
            usbd_clear_endpoint_stall_async(sc->in_intr_pipe);
        goto Exit;
    }

    if (sc->num_of_reports > 1)
        report_id  = sc->in_data[0];

    DBG_V(DHOST_HID_GENERAL,("HID_GEN INTR: num_reports = %d report_id %d\n",
        sc->num_of_reports, report_id));

    report = find_report(sc, report_id);
    DBG_V(DHOST_HID_GENERAL, ("HID_GEN INTR: report = %p\n", report));

    if (!report)
        goto Exit;
    
    if (!report->in_report_size)
    {
        /* This device does not declare IN report with this report ID, 
         * - Ignore it */
        DBG_W(DHOST_HID_GENERAL, ("HID_GEN INTR: The IN report with report ID "
            "%d not defined - Ignore it\n", report_id));
        goto Exit;
    }

    if (report->filled_index == report->readed_index)
        notify = 1;
    DBG_V(DHOST_HID_GENERAL,("HID_GEN INTR: notify = %d\n", notify));

    tmp_ptr = (juint8_t *)report->in_report;
    tmp_ptr += report->filled_index * report->in_report_size;

    DBG_V(DHOST_HID_GENERAL,("HID_GEN INTR:  tmp_ptr = %p in_data = %p\n", 
        tmp_ptr, sc->in_data));

    j_memcpy((void *)tmp_ptr ,sc->in_data, report->in_report_size);
    
    report->filled_index++;
    report->filled_index %= REPORT_POOL_SIZE;
    DBG_V(DHOST_HID_GENERAL,("HID_GEN INTR: filled_index  = %d\n", 
        report->filled_index));

    if (notify && sc->hid_handle &&
        sc->hid_handle->notify_callback->notify_read_complete)
    {
        jhid_gen_dev_t *hid_handle = sc->hid_handle;
        hid_handle->notify_callback->notify_read_complete(hid_handle->context,
            sc, report_id, report->in_report_size);
    }

Exit:
    return;
}

juint8_t hid_general_get_report_size(hid_gen_dev_ctx_t hid_dev, 
    juint8_t report_id, juint8_t type)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;
    jhid_report_t *report;
    juint8_t size = 0;

    report = find_report(sc, report_id);

    if (!report)
        goto Exit;
    
    switch (type)
    {
    case UHID_INPUT_REPORT:
        size = report->in_report_size;
        break;
    case UHID_OUTPUT_REPORT:
        size = report->out_report_size;
        break;
    case UHID_FEATURE_REPORT:
        size = report->feature_report_size;
        break;
    default:
        goto Exit;
    }

Exit:
    return size;
}

jresult_t hid_general_write(hid_gen_dev_ctx_t hid_dev, void *data,
    juint8_t report_id, juint8_t type)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;
    juint8_t size = 0;
    jresult_t rc = JEINVAL;

    if (sc->state != HID_ENABLED)
        goto Exit;
   
    size = hid_general_get_report_size(hid_dev, report_id, type);
    if (!size)
        goto Exit;
    
    if (report_id)
       *((juint8_t *)data) = report_id;
    
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    if (!sc->out_intr_pipe)
        rc = usbd_set_report_async(sc->iface, type, report_id, data, size);
    else 
        rc = usbd_set_report_pipe(sc->iface, sc->out_intr_pipe, data, size); 

Exit:
    return rc;
}

jresult_t hid_general_read(hid_gen_dev_ctx_t hid_dev, void *read_buf, 
    juint8_t report_id)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;
    jhid_report_t *report;
    jresult_t rc = JEINVAL;
#ifdef JDEBUG
    if (!read_buf)
        j_panic("HID_GEN_READ no data buffer\n");
#endif
    if (sc->state != HID_ENABLED)
        goto Exit;
    
    report = find_report(sc, report_id);

    if (!report)
        goto Exit;

    if (report->readed_index == report->filled_index)
    {
        if (sc->open_flag & NON_BLOCKING_READ)
        {
            rc = JEAGAIN;
            goto Exit;
        }
        while (report->readed_index == report->filled_index)
            jdelay_ms(10);
    }
   
    j_memcpy(read_buf,(void*)((juint8_t*)(report->in_report) + 
        (report->readed_index * report->in_report_size)),
        report->in_report_size);
    
    report->readed_index++;
    report->readed_index %= REPORT_POOL_SIZE;

    rc = 0;

Exit:
    return rc;
}

jresult_t hid_general_register(notify_t *notify_callback, 
    hid_gen_app_ctx_t app_ctx)
{
    jresult_t rc = 0;
    jhid_general_softc_t *hid_dev = NULL;
    jhid_gen_dev_t *hid_handle = NULL; 
    
    if (!notify_callback)
        j_panic("HID_GEN: No notify callback specified!\n");

    hid_handle = (jhid_gen_dev_t *)jmalloc(sizeof(jhid_gen_dev_t), M_ZERO);

    if (!hid_handle)
    {
        rc = JENOMEM;
        goto Exit;
    }
        
    hid_handle->notify_callback = notify_callback;
    hid_handle->context = app_ctx;
    TAILQ_INSERT_TAIL(&hid_handlers, hid_handle, next_handle);

    TAILQ_FOREACH(hid_dev, &hid_devices, next_device)
    {
        if (hid_dev->state == HID_CONNECTED)
        {
            hid_dev->state = HID_ATTACHED;
            hid_dev->hid_handle = hid_handle;
            if (!notify_callback->notify_attach(app_ctx, hid_dev))
            {
                hid_dev->state = HID_CONNECTED;
                hid_dev->hid_handle = NULL;
            }
        }
    }
    
Exit:
    return rc;
}

hid_gen_dev_ctx_t hid_general_open(hid_gen_dev_ctx_t hid_dev, flag_t flags)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;
    jresult_t rc = 0;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
    rc = jhid_general_app_enable(sc);
    if (rc)
        return NULL;

    sc->open_flag = flags;

    return hid_dev;
}

jresult_t hid_general_close(hid_gen_dev_ctx_t hid_dev)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
    jhid_general_app_disable(sc);
    sc->open_flag = 0;
    return 0;
}

jresult_t hid_general_set_idle(hid_gen_dev_ctx_t hid_dev, juint8_t report_id, 
    juint8_t idle_value)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    return usbd_set_idle(sc->iface, idle_value, report_id);
}

jresult_t hid_general_get_idle(hid_gen_dev_ctx_t hid_dev, juint8_t report_id, 
    juint8_t *idle_value)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    return usbd_get_idle(sc->iface, report_id, idle_value);
}

jresult_t hid_general_set_protocol(hid_gen_dev_ctx_t hid_dev, 
    juint8_t protocol_value)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    sc->is_boot_protocol = protocol_value == 0 ? 1 : 0;

    return usbd_set_protocol(sc->iface, protocol_value);
}

jresult_t hid_general_get_protocol(hid_gen_dev_ctx_t hid_dev, 
    juint8_t *protocol_value)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    return usbd_get_protocol(sc->iface, protocol_value);
}

struct hid_gen_location
{
    struct hid_location location;
    juint32_t flags;
    struct hid_limits limits;
};

void hid_general_free_location(void **location)
{
    if (*location)
        jfree(*location);
    *location = NULL;
}

jresult_t hid_general_get_field_location(hid_gen_dev_ctx_t hid_dev, 
    juint32_t usage, juint8_t type, juint8_t report_id, void **location)
{
    jresult_t rc = 0;
    struct hid_gen_location *loc;
    jhid_report_t *report;
    enum hid_type h_type; 
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;

    *location = NULL;
    report = find_report(sc, report_id);

    if (!report)
    {
        DBG_E(DHOST_HID_GENERAL, ("Report id %d not found\n", report_id));
        rc = JENOENT;
        goto Exit;
    }

    switch (type)
    {
    case UHID_INPUT_REPORT:
        h_type = hid_input;
        break;
    case UHID_OUTPUT_REPORT:
        h_type = hid_output;
        break;
    case UHID_FEATURE_REPORT:
        h_type = hid_feature;
        break;
    default:
        DBG_E(DHOST_HID_GENERAL,("Not supported report type %d\n", type));
        rc = JEINVAL;
        goto Exit;
    }

    loc = (struct hid_gen_location *)jmalloc(sizeof(struct hid_gen_location),
        0);
    if (!loc)
    {
        rc = JENOMEM;
        goto Exit;
    }
    
    rc = hid_get_field_location(report->report_descriptor, 
        report->report_descriptor_size, usage, h_type, report_id, 
        &loc->location, &loc->flags, &loc->limits);
    if (rc)
    {
        jfree (loc);
        goto Exit;
    }

    *location = loc;
    
Exit:
    return rc;
}


juint32_t hid_general_get_field_value(void *location, void* report_data)
{
    struct hid_gen_location *loc = (struct hid_gen_location *)location;
    if (!loc || !report_data)
    {
        DBG_E(DHOST_HID_GENERAL,("NULL pointer passed location = %p "
            "report_data = %p\n", location, report_data));
        return 0;
    }
    return hid_get_data((juint8_t *)report_data, &loc->location, &loc->limits);
}

jresult_t hid_general_set_field_value(void *location, void* report_data,
    juint32_t value)
{
    struct hid_gen_location *loc = (struct hid_gen_location *)location;
    if (!loc || !report_data)
    {
        DBG_E(DHOST_HID_GENERAL,("NULL pointer passed location ="
            " %p report_data = %p\n", location, report_data));
        return JEINVAL;
    }
    return hid_set_data((juint8_t *)report_data, &loc->location, &loc->limits,
        value);
}

jbool_t hid_general_is_collection(hid_gen_dev_ctx_t hid_dev, juint32_t usage)
{
    jbool_t rc = 0;
    jhid_report_t *report;
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_dev;

    report = TAILQ_FIRST(&sc->report_list); 
    while (report && !rc)
    {
        rc = hid_is_collection(report->report_descriptor, 
            report->report_descriptor_size, usage);
        report = TAILQ_NEXT(report, next_report);
    }

    return rc;
}

jbool_t hid_general_is_location_of_usage(juint32_t usage, void *location)
{
    struct hid_gen_location *loc = (struct hid_gen_location *)location;

    return (loc->limits.usage == usage ? 1 : 0);
}

void hid_general_get_att_from_location(void *location, usage_att_t *att)
{
    struct hid_gen_location *loc = (struct hid_gen_location *)location;
    struct hid_limits *lim = (struct hid_limits *)&loc->limits;
    
    att->usage = lim->usage;
    att->logical_min = lim->logical_min;
    att->logical_max = lim->logical_max;
    att->physical_min = lim->physical_min;
    att->physical_max = lim->physical_max;
    att->usage_min = lim->usage_min;
    att->usage_max = lim->usage_max;
    att->unit = lim->unit;
    att->flags = loc->flags;
}

void *hid_general_get_usbd_handle(hid_gen_dev_ctx_t hid_gen_dev_ctx)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_gen_dev_ctx;

    if (!sc)
    {
        DBG_E(DHOST_HID_GENERAL, ("hid_general_get_usbd_handle: received "
            "invalid device handle\n"));
        return NULL;
    } 

    return sc->dev;
}

jresult_t hid_general_suspend_device(hid_gen_dev_ctx_t hid_gen_dev_ctx)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_gen_dev_ctx;
    jresult_t rc;

    DBG_V(DHOST_HID_POWER, ("hid_general_suspend_device: Entered. dev 0x%x\n",
        sc));

    if (!sc)
    {
        DBG_E(DHOST_HID_POWER, ("hid_general_suspend_device: Invalid device "
            "passed to suspend\n"));
        return JEINVAL;
    }

    CORE_SAFE_ENTER;

    /* Suspend the device */
    jhid_general_suspend(sc->sc_dev);

    /* Notify the device parent that the device is suspended */
    rc = j_device_ioctl(j_device_get_parent(sc->sc_dev),
        DRV_IOCTL_PWR_CHILD_SUSPEND, (void *)sc->sc_dev);

    CORE_SAFE_LEAVE;

    return rc;
}

jresult_t hid_general_resume_device(hid_gen_dev_ctx_t hid_gen_dev_ctx)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)hid_gen_dev_ctx;
    jresult_t rc;

    DBG_V(DHOST_HID_POWER, ("hid_general_resume_device: Entered. dev 0x%x\n",
        sc));

    if (!sc)
    {
        DBG_E(DHOST_HID_POWER, ("hid_general_resume_device: Invalid device "
            "passed to resume\n"));
        return JEINVAL;
    }

    /* Notify the device parent that the device wish to resume */
    CORE_SAFE_ENTER;
    rc = j_device_ioctl(j_device_get_parent(sc->sc_dev),
        DRV_IOCTL_PWR_CHILD_RESUME, (void *)sc->sc_dev);

    if (rc)
    {
        DBG_E(DHOST_HID_POWER, ("hid_general_resume_device: Failed to resume "
            "parent\n"));
        goto Exit;
    }

    rc = jhid_general_resume(sc->sc_dev);
#ifdef JDEBUG
    if (rc)
    {
        DBG_E(DHOST_HID_POWER, ("hid_general_resume_device: Failed to resume "
            "device\n"));
    }
#endif

Exit:
    CORE_SAFE_LEAVE;
    return rc;
}

static jresult_t jhid_general_suspend(jdevice_t self)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)j_device_get_softc(self);
    usbd_status err;
    
    DBG_I(DHOST_HID_POWER, ("HID: Suspend %s\n", j_device_get_nameunit(self)));
    
    if (sc->is_suspended)
    {
        DBG_E(DHOST_HID_POWER, ("Device is already suspended\n"));
        return JEINVAL;
    }

    sc->state_before_suspend = sc->state;

    if (sc->state == HID_ENABLED)
        jhid_general_app_disable(sc);

    err = usbd_set_remote_wakeup(sc->dev, TRUE);
    if (err)
    {
        DBG_E(DHOST_HID_POWER, ("HID: Suspend setting remote wakeup %s\n",
            usbd_errstr(err)));
    }

    sc->is_suspended = 1;

    /* Always success - there is no reason to failed the host suspend of the
     * host when remote wakeup to the device failed */
    return 0;
}

static jresult_t jhid_general_resume(jdevice_t self)
{
    jhid_general_softc_t *sc = (jhid_general_softc_t *)j_device_get_softc(self);
    jresult_t rc = 0;
    usbd_status err;
    
    DBG_I(DHOST_HID_POWER, ("HID: Resume %s\n", j_device_get_nameunit(self)));
   
    if (!sc->is_suspended)
    {
        DBG_E(DHOST_HID_POWER, ("Try to resume non-suspended device\n"));
        return JEINVAL;
    }

    err = usbd_set_remote_wakeup(sc->dev, FALSE);
    if (err)
    {
        DBG_E(DHOST_HID_POWER, ("HID: Clearing remote wakeup %s\n",
            usbd_errstr(err)));
    }

    sc->is_suspended = 0;

    if (sc->state_before_suspend == HID_ENABLED)
        rc = jhid_general_app_enable(sc);
    
    return rc;
}

#ifdef CONFIG_JHOST_HID_MOUSE
jresult_t jmouse_init(void);
#endif
#ifdef CONFIG_JHOST_HID_KEYBOARD
jresult_t jkeyboard_init(void);
#endif
#ifdef CONFIG_JHOST_HID_JOYSTICK
jresult_t jjoystick_init(void);
#endif
#ifdef CONFIG_JHOST_HID_XBOX360_GAMEPAD
jresult_t jxbox360_init(void);
#endif
#ifdef CONFIG_HID_TEST
jresult_t hid_test_init(void);
#endif

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

static jresult_t jhid_general_child_init(void)
{
#ifdef CONFIG_JHOST_HID_MOUSE
    {
        jresult_t rc;
        rc = jmouse_init();
        if (rc)
            DBG_E(DHOST_HID_GENERAL, ("Failed to init mouse app [%d]\n", rc));
    }
#endif

#ifdef CONFIG_JHOST_HID_KEYBOARD
    {
        jresult_t rc;
        rc = jkeyboard_init();
        if (rc)
            DBG_E(DHOST_HID_GENERAL, ("Failed to init keyboard app %d\n", rc));
    }
#endif
    
#ifdef CONFIG_JHOST_HID_JOYSTICK
    {
        jresult_t rc;
        rc = jjoystick_init();
        if (rc)
            DBG_E(DHOST_HID_GENERAL, ("Failed  to init joystick app %d\n", rc));
    }
#endif
 
#ifdef CONFIG_JHOST_HID_XBOX360_GAMEPAD
    {
        jresult_t rc;
        rc = jxbox360_init();
        if (rc)
            DBG_E(DHOST_HID_GENERAL, ("Failed  to init xbox360 app %d\n", rc));
    }
#endif
 
#ifdef CONFIG_HID_TEST
    {
        jresult_t rc;
        rc = hid_test_init();
        if (rc)
            DBG_E(DHOST_HID_GENERAL, ("Failed  to init hid tests %d\n", rc));
    }
#endif
 
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
 
    return 0;
}

static jresult_t jhid_general_uninit(void)
{
    jhid_gen_dev_t *tmp = NULL;
    jhid_gen_dev_t *next_han = NULL;
    
    for (tmp = (jhid_gen_dev_t *)TAILQ_FIRST(&hid_handlers); tmp; 
        tmp = next_han)
    {
        next_han = TAILQ_NEXT(tmp, next_handle);
        TAILQ_REMOVE(&hid_handlers, tmp, next_handle);
        jfree(tmp);
    }
   
    return 0;
}

static jresult_t jhid_general_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
    DBG_E(DHOST_HID_GENERAL, ("HID: Unsupported IOCTL %x\n", ioctl));
    return JEINVAL;
}

DRIVER_USB_DECLARE_CUSTOM_INIT(jhid_general, jhid_general_child_init, 
    jhid_general_uninit)


#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

/* Begin – Qualcomm HSUSB addition  */ 
jresult_t hid_general_get_bootprotocol_location(
  void * bp_desc,
  juint16_t desc_size,
  juint32_t usage, 
  juint8_t type, 
  void **location )
{
  jresult_t rc;
  struct hid_gen_location *loc=NULL;
  enum hid_type h_type; 

  switch (type)
  {
  case UHID_INPUT_REPORT:
      h_type = hid_input;
      break;
  case UHID_OUTPUT_REPORT:
      h_type = hid_output;
      break;
  case UHID_FEATURE_REPORT:
      h_type = hid_feature;
      break;
  default:
      return JEINVAL;
  }

  /* allocate the storage for location. 
     * hid_general_free_location wil free  the memory chunk in jkeyboard_detach. */
        
  loc = (struct hid_gen_location *)jmalloc(sizeof(struct hid_gen_location), 0);
  if (!loc)
  {
      rc = JENOMEM;
      goto Exit;
  }

  /* get the location from boot protocol report descriptor */  
  rc = hid_get_field_location(bp_desc,
                              desc_size,
                              usage,
                              h_type,
                              0,
                              &loc->location,
                              &loc->flags,
                              &loc->limits);
  if (rc)
  {
      jfree (loc);
      goto Exit;
  }

  *location = loc;
                              
Exit:
  return rc;
}
/* End – Qualcomm HSUSB addition  */ 


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

#include <jos.h>
#include "host_hid_app.h"
#include <uw_host_hid.h>

enum mouse_values
{
    X_ORDINATE = 0,
    Y_ORDINATE,
    WHEEL,
    BUTTON1,
    BUTTON2,
    BUTTON3,
    MAX_MOUSE_LOCATIONS /* Always the last */ 
};

typedef struct app_mouse_softc 
{
    hid_gen_dev_ctx_t mouse_device;
    input_device_reg_t input_device;
    os_input_ctx_t os_input_dev;
    os_input_ops_t os_in_ops;
    void *location[MAX_MOUSE_LOCATIONS];
    jbool_t is_enabled;
    TAILQ_ENTRY(app_mouse_softc) next_dev;
} app_mouse_softc_t;

static TAILQ_HEAD(_app_mouse_head, app_mouse_softc) app_mouse_dev_queue;

char mousename []={'H','I','D', ' ', 'M', 'o', 'u', 's', 'e', '\0'};

static app_mouse_softc_t *find_mouse_dev(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t device)
{
    app_mouse_softc_t *dev;
    struct _app_mouse_head *list_head = (struct _app_mouse_head*)app_ctx;

    TAILQ_FOREACH(dev, list_head, next_dev)
    {
        if (dev->mouse_device == device)
            return dev;
    }

    return NULL;
}

static jresult_t jmouse_enable(hid_app_dev_ctx_t app_dev_ctx)
{
    app_mouse_softc_t *sc = (app_mouse_softc_t *)app_dev_ctx;
    jresult_t rc = 0;

    if (sc->is_enabled)
    {
        DBG_I(DHOST_HID_GENERAL, ("jmouse_enable: Trying to enable an already "
            "enabled device\n"));
    }
    else
    {
        sc->mouse_device = hid_general_open(sc->mouse_device, 0);
        if (!sc->mouse_device)
            rc = JEIO;
        else
            sc->is_enabled = TRUE;
    }
    
    return rc;
}

static void jmouse_disable(hid_app_dev_ctx_t app_dev_ctx)
{
    app_mouse_softc_t *sc = (app_mouse_softc_t *)app_dev_ctx;

    if (sc->is_enabled)
    {
        hid_general_close(sc->mouse_device);
        sc->is_enabled = FALSE;
    }
    else
    {
        DBG_I(DHOST_HID_GENERAL, ("jmouse_disable: Trying to disable an "
            "already disabled device\n"));
    }
}

static jresult_t jmouse_set_idle(hid_app_dev_ctx_t app_dev_ctx, juint8_t value)
{
    app_mouse_softc_t *sc = (app_mouse_softc_t *)app_dev_ctx;

    return hid_general_set_idle(sc->mouse_device, 0, value);
}

static jresult_t jmouse_get_idle(hid_app_dev_ctx_t app_dev_ctx, juint8_t *value)
{
    app_mouse_softc_t *sc = (app_mouse_softc_t *)app_dev_ctx;
    jresult_t rc = 0;

    rc = hid_general_get_idle(sc->mouse_device, 0, value);
    if (rc)
        *value = 0;
    return rc;
}

static jresult_t jmouse_get_usage_attrib(hid_app_dev_ctx_t app_dev_ctx, 
    juint32_t usage, usage_att_t *att)
{
    app_mouse_softc_t *sc = (app_mouse_softc_t *)app_dev_ctx;
    jresult_t rc = JENOENT;
    jint_t i;

    j_memset(att, 0, sizeof(usage_att_t));
    for (i = 0; i < MAX_MOUSE_LOCATIONS && sc->location[i]; ++i)
    {
        if (hid_general_is_location_of_usage(usage, sc->location[i]))
        {
            hid_general_get_att_from_location(sc->location[i], att);
            rc = 0;
            break;
        }
    }
    
    return rc;
}

static juint32_t jmouse_get_max_buttons(hid_app_dev_ctx_t app_dev_ctx)
{
    juint32_t max_but = 0;
    usage_att_t att;
    app_mouse_softc_t *sc = (app_mouse_softc_t *)app_dev_ctx;

    if (!jmouse_get_usage_attrib(sc, HID_USAGE2(HUP_BUTTON, 1), &att))
        max_but = att.usage_max - att.usage_min + 1;

    return max_but;
}

static void *jmouse_get_usbd_handle(hid_app_dev_ctx_t app_dev_ctx)
{
    app_mouse_softc_t *sc = (app_mouse_softc_t *)app_dev_ctx;

    if (!sc)
    {
        DBG_E(DHOST_HID_GENERAL, ("jmouse_get_usbd_handle: received invalid "
            "device handle\n"));
        return NULL;
    }
    return hid_general_get_usbd_handle(sc->mouse_device);
}

/* Begin – Qualcomm HSUSB addition  */ 
/* boot protocol mouse report descriptor */
static const byte bp_mouse_rd[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};
/* End – Qualcomm HSUSB addition  */ 

static jbool_t jmouse_attach (hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t device)
{
    enum mouse_values i;
    jresult_t rc;
    juint16_t t;
    struct _app_mouse_head *list_head = (struct _app_mouse_head*)app_ctx;
    app_mouse_softc_t *sc = NULL;

#if (defined(FEATURE_BT_EXTPF_HID_HOST) || defined(FEATURE_CANDLEBOX))
    juint8_t report_id = 0; 
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    if (!hid_general_is_collection(device, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_MOUSE)))
    {
        return 0;
    }

    sc = (app_mouse_softc_t*)jmalloc(sizeof(app_mouse_softc_t), M_ZERO);
    if (!sc)
    {
        DBG_E(DHOST_HID_GENERAL,("Attach mouse: Out of memory!\n"));
        goto Exit;
    }

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#ifndef FEATURE_CANDLEBOX
    /* Begin – Qualcomm HSUSB addition  */ 
    /* Accoding to the boot protocol or keyboard and mouse, the report descriptor is predefined
        * and the report descritpor from the device shouldn't be parsed to get the location of the
        * items of usage. The location of the report items will be drived from the hard coded boot
        * protocol report descriptor. For more detail, refer to appendix B and F in Device Class Definition 
        * for Human Interface Devices (HID) Firmware Specification—6/27/01 Version 1.11  
        */

    rc = hid_general_get_bootprotocol_location((void *)bp_mouse_rd, sizeof(bp_mouse_rd),
           HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_X),UHID_INPUT_REPORT, 
           &sc->location[X_ORDINATE]);
    /* End – Qualcomm HSUSB addition  */ 
#else
    rc = hid_general_get_field_location(device, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_X),
        UHID_INPUT_REPORT, report_id, &sc->location[X_ORDINATE]);
#endif
    if (rc)
    {
        DBG_E(DHOST_HID_GENERAL,
            ("Error during parsing of the report descriptor for X :%d\n", rc));
        goto Exit;
    }

#ifndef FEATURE_CANDLEBOX
    /* Begin – Qualcomm HSUSB addition  */ 
    rc = hid_general_get_bootprotocol_location((void *)bp_mouse_rd, sizeof(bp_mouse_rd),  
           HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Y), UHID_INPUT_REPORT, 
           &sc->location[Y_ORDINATE]);
    /* End – Qualcomm HSUSB addition  */ 
#else
    rc = hid_general_get_field_location(device, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Y),
        UHID_INPUT_REPORT, report_id, &sc->location[Y_ORDINATE]);
#endif

    if (rc)
    {
        DBG_E(DHOST_HID_GENERAL,
            ("Error during parsing of the report descriptor for Y: %d\n", rc));
        goto Exit;
    }

#ifndef FEATURE_CANDLEBOX
    /* Begin – Qualcomm HSUSB addition  */ 
    rc = hid_general_get_bootprotocol_location((void *)bp_mouse_rd, sizeof(bp_mouse_rd), 
           HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_WHEEL), UHID_INPUT_REPORT, 
           &sc->location[WHEEL]);
    /* End – Qualcomm HSUSB addition  */ 
#else
    rc = hid_general_get_field_location(device, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_WHEEL),
        UHID_INPUT_REPORT, report_id, &sc->location[WHEEL]);
#endif

    if (rc)
        sc->location[WHEEL] = NULL; /*no wheel found */
    for (i = BUTTON1, t = 1; i <= BUTTON3; ++i, t++)
    {
#ifndef FEATURE_CANDLEBOX
        /* Begin – Qualcomm HSUSB addition  */ 
        rc = hid_general_get_bootprotocol_location((void *)bp_mouse_rd, sizeof(bp_mouse_rd), 
               HID_USAGE2(HUP_BUTTON, t), UHID_INPUT_REPORT, 
               &sc->location[i]);
        /* End – Qualcomm HSUSB addition */   
#else
        rc = hid_general_get_field_location(device, 
            HID_USAGE2(HUP_BUTTON, t),
            UHID_INPUT_REPORT, report_id, &sc->location[i]);
#endif
        if (rc) 
        {
            if (i == BUTTON1)
            {
                DBG_E(DHOST_HID_GENERAL,
                    ("Error during parsing of the report descriptor button "
                    "no %d\n", t));
                goto Exit;
            }
            else 
                sc->location[i] = NULL;
        }
    }
    
    sc->mouse_device = device;
    sc->input_device.enable_cb = jmouse_enable;
    sc->input_device.disable_cb = jmouse_disable;
    sc->input_device.get_out_report_size_cb = NULL;
    sc->input_device.set_report_field_cb = NULL;
    sc->input_device.send_out_cb = NULL;
    sc->input_device.set_idle_cb = jmouse_set_idle;
    sc->input_device.get_idle_cb = jmouse_get_idle;
    sc->input_device.set_protocol_cb = NULL;
    sc->input_device.get_protocol_cb = NULL;
    sc->input_device.query_usage_att_cb = jmouse_get_usage_attrib;
    sc->input_device.get_max_buttons_cb = jmouse_get_max_buttons;
    sc->input_device.get_usbd_handle_cb = jmouse_get_usbd_handle;
    sc->input_device.name = mousename;
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#else
    sc->input_device.device_type = HID_MOUSE;
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    sc->input_device.idvendor = 0;
    sc->input_device.idproduct = 0;
    sc->input_device.idversion = 0; 
    sc->input_device.app_dev_ctx = sc;

    TAILQ_INSERT_TAIL(list_head, sc, next_dev);
    
    rc = register_input_device(&sc->input_device, &sc->os_input_dev, 
        &sc->os_in_ops);
    if (rc)
    {
        jprintf("Mouse device Failed to register %d\n", rc);
        TAILQ_REMOVE(list_head, sc, next_dev);
        goto Exit;
    }

    /* Check existance of needed os callbacks */
    if ((!sc->os_in_ops.unregister_input_device) ||
        (!sc->os_in_ops.report_event) ||
        (!sc->os_in_ops.report_relative_data))
    {
        jprintf("Mouse device os_callbacks absent \n");
        TAILQ_REMOVE(list_head, sc, next_dev);
        goto Exit;
    }

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
    {
        jmouse_set_idle((hid_app_dev_ctx_t) sc, 0);

#ifndef FEATURE_CANDLEBOX
        /* Begin – Qualcomm HSUSB addition  */ 
        /* set the device to boot protocol mode */
        hid_general_set_protocol(sc->mouse_device, 0);
        /* End – Qualcomm HSUSB addition  */ 
#endif /* FEATURE_CANDLEBOX */
    }
    jprintf("Mouse device attached\n");
    return 1;
    
Exit:
    
    if (sc)
    {
        for (i = X_ORDINATE; i <= BUTTON3; ++i)
            hid_general_free_location(&sc->location[i]);
        jfree(sc);
    }
    return 0;
}

void jmouse_detach (hid_gen_app_ctx_t app_ctx, hid_gen_dev_ctx_t device)
{
    jint_t i;
    app_mouse_softc_t *sc = find_mouse_dev(app_ctx, device);
    struct _app_mouse_head *list_head = (struct _app_mouse_head*)app_ctx;
    
    if (!sc)
        return;

    /* Disable mouse before unregistering*/
    jmouse_disable(sc);
    sc->os_in_ops.unregister_input_device(sc->os_input_dev);
    
    for (i = 0; i < MAX_MOUSE_LOCATIONS; ++i)
        hid_general_free_location(&sc->location[i]);
    
    TAILQ_REMOVE(list_head, sc, next_dev);
    jfree(sc);
}

#ifdef CANDLEBOX_FEATURE_USBHID
#define MAX_CB_LOCAL_BUFF_SIZE 16
static juint8_t cb_local_buf[MAX_CB_LOCAL_BUFF_SIZE];
#endif
void jmouse_complete(hid_gen_app_ctx_t app_ctx, hid_gen_dev_ctx_t device, 
    juint8_t rep_id, juint32_t report_size)
{
    jint_t i, t;
    app_mouse_softc_t *sc = find_mouse_dev(app_ctx, device);
    juint8_t *buf;
#ifdef CANDLEBOX_FEATURE_USBHID
    if (report_size <= MAX_CB_LOCAL_BUFF_SIZE)
    {
      buf = cb_local_buf;
    }
    else
#endif /* CANDLEBOX_FEATURE_USBHID */
    {
      buf = (juint8_t *)jmalloc(report_size, M_ZERO);
    }

    if (!buf)
        goto Exit;

    if (hid_general_read(sc->mouse_device, (void *)buf, rep_id))
        goto Exit;

    for (i = BUTTON1, t = 0; i <= BUTTON3; ++i, ++t)
    {
        if (sc->location[i])
        {
            usage_att_t att;
            DBG_I(DHOST_HID_GENERAL, ("Reporting button %d\n", t));
            hid_general_get_att_from_location(sc->location[i], &att);
            sc->os_in_ops.report_event(sc->os_input_dev, t,
                hid_general_get_field_value(sc->location[i],(void *)buf), &att);
        } 
    }

    for (i = X_ORDINATE; i <= WHEEL; ++i)
    {
        if (sc->location[i])
        {
            usage_att_t att;
            hid_general_get_att_from_location(sc->location[i], &att);
            sc->os_in_ops.report_relative_data(sc->os_input_dev,
                (i - X_ORDINATE),
                hid_general_get_field_value(sc->location[i],(void *)buf), &att);
        } 
    }

Exit:
#ifdef CANDLEBOX_FEATURE_USBHID
    if (report_size > MAX_CB_LOCAL_BUFF_SIZE)
#endif
    {
      if(buf)
          jfree(buf);
    }
    return;
}

static notify_t mouse_notify_callbacks = {
    jmouse_attach,
    jmouse_detach,
    jmouse_complete
};

jresult_t jmouse_init(void)
{
    TAILQ_INIT(&app_mouse_dev_queue);
    if (hid_general_register(&mouse_notify_callbacks, &app_mouse_dev_queue))
    {
        DBG_E(DHOST_HID_GENERAL, ("fail to initialize\n"));
        return JEINVAL; 
    }
    DBG_I(DHOST_HID_GENERAL, ("\nMouse driver registered \n"));
    return 0;
}

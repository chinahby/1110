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

#include "jiccd.h"

extern uwh_iccd_callbacks_t *jiccd_os_cbs;

static usb_descriptor_t *get_iccd_desc(usb_config_descriptor_t *conf_d);
static void fill_iccd_info(jiccd_softc_t *sc);

#define SMARTCARD_DESCRIPTOR 0x21

#define NOT_IMPLEMENTED(fname) jprintf(fname ": Not implemented yet\n");

#define ICCD_CLOSE_PIPE(pipe) \
    do { \
        if ((pipe)) \
        { \
          usbd_abort_pipe((pipe)); \
          usbd_close_pipe(pipe); \
        } \
    } while (0)

/********************* USB Core <=> Class driver functions ********************/

/**
 * Function name: jiccd_probe
 * Description: This callback called by USB Core to match new attached device
 *              with this class driver. 
 * Parameters: 
 *     @self: (IN) - device private data 
 *
 * Return value: Match level UMATCH_NONE for none and etc.
 * Scope: local,  callback function
 **/
static jresult_t jiccd_probe(jdevice_t self)
{
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    usb_interface_descriptor_t *id;
    DECLARE_FNAME("jiccd_probe");

    DBG_I(DHOST_ICCD_GENERAL, ("%s: Entered\n", fname));

    if (!uaa->iface)
        return UMATCH_NONE;

    /* Check that this is an ICCD device. */
    id = usbd_get_interface_descriptor(uaa->iface);

    if (id && id->bInterfaceClass == UICLASS_ICCD)
    {
        /* For now we support only protocol version b */
        if (uaa->iface->idesc->bInterfaceProtocol != UIPROTO_ICCD_B)
        {
            DBG_W(DHOST_ICCD_GENERAL, ("%s: Protocol not supported\n", fname));
            return UMATCH_NONE;
        }

        DBG_I(DHOST_ICCD_GENERAL, ("%s: Found a match\n", fname));
        return UMATCH_IFACECLASS;
    }

    return UMATCH_NONE;
}

/**
 * Function name:  jiccd_detach
 * Description: This callback called by USB Core when detach event recognized
 *              This calback have to clear all allocated resources.
 * Parameters: 
 *     @self: (IN) - device private data
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local, callback function
 **/

static jresult_t jiccd_detach(jdevice_t self)
{
    jiccd_softc_t *sc = (jiccd_softc_t *)(j_device_get_softc(self));
    uwh_iccd_app_h os_ctx = (uwh_iccd_app_h)sc->os_ctx;

    DBG_V(DHOST_ICCD_GENERAL, ("jiccd_detach: Entered\n"));
  
    if (sc->state == ICCD_STATE_ATTACHED)
    {
        if ((jiccd_os_cbs) && (jiccd_os_cbs->detach))
            jiccd_os_cbs->detach(os_ctx);
    }

    /* Abort & Close all the pipes */
    ICCD_CLOSE_PIPE(sc->bulkout_pipe);
    ICCD_CLOSE_PIPE(sc->bulkin_pipe);
    ICCD_CLOSE_PIPE(sc->intrin_pipe);

    /* Free the buffer and the xfer */
    if (sc->ctrl_xfer)
    {
        if (sc->ctrl_xfer->xfer)
            usbd_free_xfer(sc->ctrl_xfer->xfer);

        jfree(sc->ctrl_xfer);
    }

    sc->state = ICCD_STATE_DETACHED;

    return 0;
}

/**
 * Function name:  jiccd_attach
 * Description: This callback called by USB Core after successfull match of the
 *              new attached device with current class driver.
 *              This callback will create new iccd instace 
 * Parameters: 
 *     @self: (IN) - device private data
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local, callback function
 **/

static jresult_t jiccd_attach(jdevice_t self)
{
    jiccd_softc_t *sc = (jiccd_softc_t *)(j_device_get_softc(self));
    jresult_t rc = JENXIO;
    usbd_status err;
    jint_t i;
    usb_endpoint_descriptor_t *ed;
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    juint8_t num_endpoints = 0;
    DECLARE_FNAME("jiccd_attach");

    DBG_V(DHOST_ICCD_GENERAL, ("%s: Entered\n", fname));

    if ((!jiccd_os_cbs) || (!jiccd_os_cbs->attach))
    {
        DBG_E(DHOST_ICCD_GENERAL, ("%s: No application registered\n", fname));
        return JEINVAL;
    }

    sc->dev = self;

    sc->ctrl_xfer = (iccd_xfer_t *)jmalloc(sizeof(iccd_xfer_t), M_ZERO);
    if (!sc->ctrl_xfer)
    {
        DBG_E(DHOST_ICCD_GENERAL,("%s: Out of memory\n", fname));
        return JENOMEM;
    }

    /* Getting the interface descriptor and saving the protocol type */
    sc->iface = uaa->iface;
    sc->sc_udev = uaa->device;
    sc->protocol = sc->iface->idesc->bInterfaceProtocol;

    sc->ctrl_xfer->xfer = usbd_alloc_xfer(sc->sc_udev);
    if (!sc->ctrl_xfer->xfer)
    {
        DBG_E(DHOST_ICCD_GENERAL,("%s: Out of memory\n", fname));
        rc = JENOMEM;
        goto Error;
    }

    if (usbd_endpoint_count(sc->iface, &num_endpoints))
    {
        DBG_E(DHOST_ICCD_GENERAL,("%s: Invalid Interface Descriptor\n", fname));
        return rc;
    }

    /* Going over all the endpoints and finding the endpoints we need */
    for (i = 0; i < num_endpoints; i++) 
    {
        ed = usbd_interface2endpoint_descriptor(uaa->iface, (juint8_t)i);
        if (!ed) 
        {    
            DBG_E(DHOST_ICCD_GENERAL, ("%s: Couldn't read endpoint "
                "descriptor\n", fname));
            goto Error;
        }

        if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_IN
            && (UE_GET_XFERTYPE(ed->bmAttributes) == UE_BULK)) 
        {
            sc->bulkin = ed->bEndpointAddress;
        } 
        else if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_OUT
            && (UE_GET_XFERTYPE(ed->bmAttributes) == UE_BULK)) 
        {
            sc->bulkout = ed->bEndpointAddress;
        }
        else if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_IN
            && (UE_GET_XFERTYPE(ed->bmAttributes) == UE_INTERRUPT))
        {
            sc->intrin = ed->bEndpointAddress;
        }
    }

    /* If the protocol is bulk, we have two bulk endpoints and optional
     * interrupt end point */
    if (sc->protocol == UIPROTO_ICCD_BULK)
    {
        /* check whether we found all the endpoints we need */
        if (!sc->bulkin || !sc->bulkout) 
        {
            DBG_E(DHOST_ICCD_GENERAL, ("%s: Endpoint not found %d/%d/%d\n",
                fname, sc->bulkin, sc->bulkout, sc->intrin));
            goto Error;
        }
        /* Open the bulk-in and bulk-out pipe */
        err = usbd_open_pipe(uaa->iface, sc->bulkout,
            USBD_EXCLUSIVE_USE, &sc->bulkout_pipe);
        if (err) 
        {
            DBG_E(DHOST_ICCD_GENERAL, ("%s: Cannot open %d-out pipe (bulk)\n",
                fname, sc->bulkout));
            goto Error;
        }
        err = usbd_open_pipe(uaa->iface, sc->bulkin, USBD_EXCLUSIVE_USE,
            &sc->bulkin_pipe);
        if (err) 
        {
            DBG_E(DHOST_ICCD_GENERAL, ("%s: Cannot open %d-in pipe (bulk)\n",
                fname, sc->bulkin));
            goto Error;
        }
    }

    if (sc->protocol != UIPROTO_ICCD_A && sc->intrin) 
    {
        err = usbd_open_pipe(uaa->iface, sc->intrin, USBD_EXCLUSIVE_USE, 
            &sc->intrin_pipe);
        if (err) 
        {
            DBG_E(DHOST_ICCD_GENERAL, ("%s: Cannot open intr %d-in pipe\n",
                fname ,sc->intrin));
            goto Error;
        }
    }

    fill_iccd_info(sc);

    sc->state = ICCD_STATE_ATTACHED;
    rc = jiccd_os_cbs->attach(sc, &sc->os_ctx);
    if (rc)
    {
        DBG_E(DHOST_ICCD_GENERAL, ("%s: Finished with error %d\n", fname,
            rc));
        goto Error;
    }

    DBG_V(DHOST_ICCD_GENERAL, ("%s: Attached successfully\n", fname));

    return 0;

Error:
    jiccd_detach(self);
    return rc;
}    

static usb_descriptor_t *get_iccd_desc(usb_config_descriptor_t *conf_d)
{
    juint8_t length;
    juint8_t *desc_ptr, *end;
    usb_descriptor_t *desc = NULL;
    DECLARE_FNAME("get_iccd_desc");

    desc_ptr = (juint8_t *)conf_d;
    desc = (usb_descriptor_t *)desc_ptr;
    end = desc_ptr + ua_le16toh(conf_d->wTotalLength);
    length = conf_d->bLength;

    DBG_X(DHOST_ICCD_GENERAL, ("%s: Entered - desc_ptr %p, end %p\n", fname, 
        desc_ptr, end));

    while ((desc_ptr + length <= end) && length)
    {
        if (desc->bDescriptorType == SMARTCARD_DESCRIPTOR)
            goto Exit;

        /* Get the next interface descriptor */
        desc_ptr += length;
        desc = (usb_descriptor_t *)desc_ptr;
        length = desc->bLength;
    }

    desc = NULL;
    DBG_V(DHOST_ICCD_GENERAL, ("%s: Exiting. No descriptor match\n", fname));
    
Exit:
    return desc;
}

static void fill_iccd_info(jiccd_softc_t *sc)
{
    juint_t pos = 0;
    juint8_t *iccd_desc;
    
    /* Getting the iccd descriptor */
    iccd_desc = (juint8_t *)get_iccd_desc(sc->sc_udev->cdesc);
    /* Unpacking all data */
    UNPACK_BYTE(iccd_desc, pos, sc->iccd_info.length);
    UNPACK_BYTE(iccd_desc, pos, sc->iccd_info.descriptor_type);
    UNPACK_WORD(iccd_desc, pos, sc->iccd_info.release_number);
    UNPACK_BYTE(iccd_desc, pos, sc->iccd_info.max_slot_index);
    UNPACK_BYTE(iccd_desc, pos, sc->iccd_info.voltage_support);
    UNPACK_DWORD(iccd_desc, pos, sc->iccd_info.protocols);
    UNPACK_DWORD(iccd_desc, pos, sc->iccd_info.default_clock);
    UNPACK_DWORD(iccd_desc, pos, sc->iccd_info.maximum_clock);
    UNPACK_BYTE(iccd_desc, pos, sc->iccd_info.num_clock_supported);
    UNPACK_DWORD(iccd_desc, pos, sc->iccd_info.data_rate);
    UNPACK_DWORD(iccd_desc, pos, sc->iccd_info.max_data_rate);
    UNPACK_BYTE(iccd_desc, pos, sc->iccd_info.num_data_rate_supported);
    UNPACK_DWORD(iccd_desc, pos, sc->iccd_info.max_ifsd);
    UNPACK_DWORD(iccd_desc, pos, sc->iccd_info.synch_protocols);
    UNPACK_DWORD(iccd_desc, pos, sc->iccd_info.mechanical);
    UNPACK_DWORD(iccd_desc, pos, sc->iccd_info.features);
    UNPACK_DWORD(iccd_desc, pos, sc->iccd_info.max_ccid_message_length);
    UNPACK_BYTE(iccd_desc, pos, sc->iccd_info.class_get_response);
    UNPACK_BYTE(iccd_desc, pos, sc->iccd_info.class_envelope);
    UNPACK_WORD(iccd_desc, pos, sc->iccd_info.lcd_layout);
    UNPACK_BYTE(iccd_desc, pos, sc->iccd_info.pin_support);
    UNPACK_BYTE(iccd_desc, pos, sc->iccd_info.max_ccid_busy_slots);
}

/**
 * Function name:  jiccd_suspend
 * Description: This callback called by USB Core during enter in suspend mode.
 * Parameters: 
 *     @self: (IN) - device private data
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local, callback function
 **/
jresult_t jiccd_suspend(jdevice_t self)
{
    jiccd_softc_t *sc = (jiccd_softc_t *)(j_device_get_softc(self));
    
    /* Abort & Close all the pipes */
    ICCD_CLOSE_PIPE(sc->bulkout_pipe);
    ICCD_CLOSE_PIPE(sc->bulkin_pipe);
    ICCD_CLOSE_PIPE(sc->intrin_pipe);

    return 0;
}

/**
 * Function name:  jiccd_resume
 * Description: This callback called by USB Core during resume.
 * Parameters: 
 *     @self: (IN) - device private data
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local, callback function
 **/
jresult_t jiccd_resume(jdevice_t self)
{
    jiccd_softc_t *sc = (jiccd_softc_t *)(j_device_get_softc(self));
    jresult_t rc;
    usbd_status err;
    struct usb_attach_arg *uaa = j_device_get_ivars(self);

    DECLARE_FNAME("jiccd_resume");

    if (sc->bulkout)
    {
        err = usbd_open_pipe(uaa->iface, sc->bulkout,
            USBD_EXCLUSIVE_USE, &sc->bulkout_pipe);
        if (err) 
        {
            DBG_E(DHOST_ICCD_GENERAL, ("%s: Cannot open %d-out pipe (bulk)\n",
                fname, sc->bulkout));
            rc = JEINVAL;
            goto Error;
        }
    }

    if (sc->bulkin)
    {
        err = usbd_open_pipe(uaa->iface, sc->bulkin, USBD_EXCLUSIVE_USE,
            &sc->bulkin_pipe);
        if (err) 
        {
            DBG_E(DHOST_ICCD_GENERAL, ("%s: Cannot open %d-in pipe (bulk)\n",
                fname, sc->bulkin));
            rc = JEINVAL;
            goto Error;
        }
    }
    
    if (sc->protocol != UIPROTO_ICCD_A && sc->intrin) 
    {
        err = usbd_open_pipe(uaa->iface, sc->intrin, USBD_EXCLUSIVE_USE, 
            &sc->intrin_pipe);
        if (err) 
        {
            DBG_E(DHOST_ICCD_GENERAL, ("%s: Cannot open intr %d-in pipe\n",
                fname ,sc->intrin));
            rc = JEINVAL;
            goto Error;
        }
    }
    
    return 0;
Error:
    return rc;
}

/**
 * Function name:  jiccd_ioctl
 * Description: IOCTL Callback 
 * Parameters: 
 *     @self: (IN) - device private data 
 *     @ioctl: (IN) - IOCTL code
 *     @arg: (IN) - IOCTL parameter
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/

static jresult_t jiccd_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
    return JEINVAL;
}

/* Register the class driver in Core */

DRIVER_USB_DECLARE(jiccd)

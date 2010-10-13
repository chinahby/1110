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

#define ICCD_STATUS_FAILED_MASK 0x40

uwh_iccd_callbacks_t *jiccd_os_cbs = NULL;

static usbd_status ctrl_transfer(struct jiccd_softc *sc, 
    usb_device_request_t *req, void *buffer, juint32_t buflen, 
    iccd_xfer_t *iccd_xfer, usbd_callback callback);
static void power_off_done(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status);
static void get_status_done(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status);
static void data_block_done(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status);
static void xfer_block_done(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status);

#define CHECK_BUSY() \
    do { \
        if (sc->pending_ctrl_req) \
        { \
            DBG_E(DHOST_ICCD_API, ("%s: ctrl request is busy\n", fname)); \
            return JEBUSY; \
        } \
        sc->pending_ctrl_req = 1; \
    } while (0)

/**************************************************************************/
/*                             API Implementation                         */
/**************************************************************************/
jresult_t uwh_iccd_register(uwh_iccd_callbacks_t *callbacks)
{
    DECLARE_FNAME("uwh_iccd_register");

    if (jiccd_os_cbs)
    {
        DBG_E(DHOST_ICCD_API, ("%s: Can't register more then one application\n",
            fname));
        return JEINVAL;
    }

    jiccd_os_cbs = callbacks;
    return 0;
}

void uwh_iccd_unregister()
{
    jiccd_os_cbs = NULL;
}

jresult_t uwh_iccd_power(uwh_iccd_drv_h drv_ctx, jbool_t power_on, 
    uwh_iccd_cb cb)
{
    jiccd_softc_t *sc = (jiccd_softc_t *)drv_ctx;
    iccd_xfer_t *xfer = sc->ctrl_xfer;
    DECLARE_FNAME("uwh_iccd_power");
    
    CHECK_BUSY();

    xfer->cb = cb;
    xfer->sc = sc;

    if (sc->protocol == UIPROTO_ICCD_BULK)
    {
    }
    else
    {
        USETW(sc->request.wIndex, sc->iface->idesc->bInterfaceNumber);
        sc->request.bmRequestType = UT_WRITE_CLASS_INTERFACE;
        USETW(sc->request.wLength, 0);

        DBG_V(DHOST_ICCD_API,("%s: Power %s\n", fname, 
            power_on ? "on" : "off"));
        if (power_on)
        {
            sc->request.bRequest = UWH_ICCD_HOST_TO_RDR_BREQ_ICC_POWER_ON;
            USETW(sc->request.wValue, 1);

            return ctrl_transfer(sc, &sc->request, NULL, 0, xfer, 
                xfer_block_done);
        }
        else
        {
            sc->request.bRequest = UWH_ICCD_HOST_TO_RDR_BREQ_ICC_POWER_OFF;
            USETW(sc->request.wValue, 0);

            return ctrl_transfer(sc, &sc->request, NULL, 0, xfer, 
                power_off_done);
        }
    }

    return 0;
}

jresult_t uwh_iccd_xfr_block(uwh_iccd_drv_h drv_ctx, void *data, 
    juint16_t size, uwh_iccd_cb cb, void *arg)
{
    jiccd_softc_t *sc = (jiccd_softc_t *)drv_ctx;
    iccd_xfer_t *xfer = sc->ctrl_xfer;
    DECLARE_FNAME("uwh_iccd_xfr_block");
    
    CHECK_BUSY();

    xfer->cb = cb;
    xfer->sc = sc;
    xfer->arg = arg;

    sc->request.bRequest = UWH_ICCD_HOST_TO_RDR_BREQ_XFER_BLOCK;
    sc->request.bmRequestType = UT_WRITE_CLASS_INTERFACE;
    USETW(sc->request.wValue, 0);
    USETW(sc->request.wIndex, sc->iface->idesc->bInterfaceNumber);
    USETW(sc->request.wLength, size);

    return ctrl_transfer(sc, &sc->request, data, size, xfer, xfer_block_done);
}

jresult_t uwh_iccd_data_block(uwh_iccd_drv_h drv_ctx, void *data, 
    juint16_t size, uwh_iccd_cb cb, void *arg)
{
    jiccd_softc_t *sc = (jiccd_softc_t *)drv_ctx;
    iccd_xfer_t *xfer = sc->ctrl_xfer;
    DECLARE_FNAME("uwh_iccd_data_block");

    CHECK_BUSY();

    xfer->cb = cb;
    xfer->sc = sc;
    xfer->arg = arg;

    sc->request.bmRequestType = UT_READ_CLASS_INTERFACE;
    sc->request.bRequest = UWH_ICCD_RDD_TO_HOST_BREQ_DATA_BLOCK;
    USETW(sc->request.wValue, 0);
    USETW(sc->request.wIndex, sc->iface->idesc->bInterfaceNumber);
    USETW(sc->request.wLength, size);
 
    return ctrl_transfer(sc, &sc->request, data, size, xfer, data_block_done);
}

uwh_iccd_descriptor_t *uwh_get_info(uwh_iccd_drv_h drv_ctx)
{
    jiccd_softc_t *sc = (jiccd_softc_t *)drv_ctx;

    return &sc->iccd_info;
}

jresult_t uwh_iccd_suspend(uwh_iccd_drv_h drv_ctx)
{
    jiccd_softc_t *sc = (jiccd_softc_t *)drv_ctx;
    jresult_t rc = 0;

    DECLARE_FNAME("uwh_iccd_suspend");

    if (sc->state != ICCD_STATE_ATTACHED)
    {
        DBG_E(DHOST_ICCD_GENERAL, ("%s: Device can't suspend - invalid state\n"
            , fname));
        return JEINVAL;
    }

    CORE_SAFE_ENTER;
    rc = jiccd_suspend(sc->dev);
    if (rc)
    {
        DBG_E(DHOST_ICCD_GENERAL, ("%s: Error suspending the deivce rc = %d\n",
            fname, rc));
        goto Exit;
    }

    /* Abort transfer on ep0 */
    /* HSU temp fix */
    usbd_abort_request((usbd_xfer_handle)sc->ctrl_xfer->xfer);
    /* HSU temp fix */

    rc = j_device_ioctl(j_device_get_parent(sc->dev), 
        DRV_IOCTL_PWR_CHILD_SUSPEND, sc->dev);
    if (rc)
    {
        DBG_E(DHOST_ICCD_GENERAL, ("%s: Cannot send suspend ioctal %d\n",
            fname, rc));
        goto Exit;
    }
 
    sc->state = ICCD_STATE_SUSPEND;

Exit:
    CORE_SAFE_LEAVE;
    return rc;
}

jresult_t uwh_iccd_resume(uwh_iccd_drv_h drv_ctx)
{
    jiccd_softc_t *sc = (jiccd_softc_t *)drv_ctx;
    jresult_t rc = 0;

    DECLARE_FNAME("uwh_iccd_resume");
    
    if (sc->state != ICCD_STATE_SUSPEND)
    {
        DBG_E(DHOST_ICCD_GENERAL, ("%s: Device can't resume invalid state\n", 
            fname));
        return JEINVAL;
    }

    CORE_SAFE_ENTER;
    rc = j_device_ioctl(j_device_get_parent(sc->dev), 
        DRV_IOCTL_PWR_CHILD_RESUME, sc->dev);
    if (rc)
    {
        DBG_E(DHOST_ICCD_GENERAL, ("%s: Cannot send resume ioctal\n",
            fname));
        goto Exit;
    }

    rc = jiccd_resume(sc->dev);
    if (rc)
    {
        DBG_E(DHOST_ICCD_GENERAL, ("%s: Cannot resume device rc = %d\n",
            fname, rc));
        
        j_device_ioctl(j_device_get_parent(sc->dev), 
            DRV_IOCTL_PWR_CHILD_SUSPEND, sc->dev);

        goto Exit;
    }

    sc->state = ICCD_STATE_ATTACHED;

Exit:
    CORE_SAFE_LEAVE;
    return rc;
}


/**************************************************************************/
/*                             Help functions                             */
/**************************************************************************/
static usbd_status ctrl_transfer(struct jiccd_softc *sc, 
    usb_device_request_t *req, void *buffer, juint32_t buflen, 
    iccd_xfer_t *iccd_xfer, usbd_callback callback)
{
    usbd_status rc;
    DECLARE_FNAME("ctrl_transfer");

    usbd_setup_default_xfer(iccd_xfer->xfer, sc->sc_udev, (void *)iccd_xfer, 
        sc->timeout, req, buffer, buflen, USBD_SHORT_XFER_OK, callback);

    rc = usbd_transfer(iccd_xfer->xfer);
    if (rc && rc != USBD_IN_PROGRESS) 
    {
        DBG_E(DHOST_ICCD_API, ("%s: failed to setup ctrl transfer, %s\n",
            fname, usbd_errstr(rc)));
        return rc;
    }

    return USBD_NORMAL_COMPLETION;
}

/* When the power off request is done, we should send a slot_status request */
static void power_off_done(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status)
{
    iccd_xfer_t *iccd_xfer = (iccd_xfer_t *)priv;
    jiccd_softc_t *sc = iccd_xfer->sc;
    juint8_t bReq = (sc->protocol == UIPROTO_ICCD_A ? 
        UWH_ICCD_RDR_TO_HOST_BREQ_GET_STATUS : 
        UWH_ICCD_RDR_TO_HOST_BTYPE_SLOT_STATUS);
    int size = (sc->protocol == UIPROTO_ICCD_A ? 1 : 3);
    void *trans_buffer = jmalloc(size, M_ZERO);
    usbd_status rc;
    
    DECLARE_FNAME("power_off_done");

    if (!trans_buffer)
    {
        DBG_E(DHOST_ICCD_API,("%s: Out of memory\n", fname));
        return;
    }
    
    sc->request.bmRequestType = UT_READ_CLASS_INTERFACE;
    sc->request.bRequest = bReq;
    USETW(sc->request.wValue, 0);
    USETW(sc->request.wIndex, 0);
    USETW(sc->request.wLength, size);
 
    rc = ctrl_transfer(sc, &sc->request, trans_buffer, size, iccd_xfer, 
        get_status_done);
    if (rc)
    {
        DBG_E(DHOST_ICCD_API,("%s: Couldn't perform ctrl_transfer\n", fname));
        jfree(trans_buffer);
    }
}

/* When get status is done, we should parse the data received and call
 * the callback of the application */
static void get_status_done(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status)
{
    ab_data_t ab_data;
    iccd_error_t error = 0;
    iccd_xfer_t *iccd_xfer = (iccd_xfer_t *)priv;
    jiccd_softc_t *sc = iccd_xfer->sc;
    int pos = 0;
    jresult_t usb_status = 0;
    DECLARE_FNAME("get_status_done");

    /* Getting the status received */
    UNPACK_BYTE((juint8_t *)xfer->buffer, pos, ab_data.status);

    if (ab_data.status & ICCD_STATUS_FAILED_MASK)
    {
        UNPACK_BYTE((juint8_t *)xfer->buffer, pos, error);
        ab_data.status &= ~ICCD_STATUS_FAILED_MASK;
    }

    jfree(xfer->buffer);

#ifdef JDEBUG
    if (status)
    {
        DBG_E(DHOST_ICCD_API, ("%s: Error occurred in transfer error is %s\n", 
            fname, usbd_errstr(status)));
    }
#endif

    sc->pending_ctrl_req = 0; 
    
    /* Calling the application callback */
    iccd_xfer->cb(sc->os_ctx, NULL, 0, 0, ab_data, error, iccd_xfer->arg, 
        usb_status);
}

static void data_block_done(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status)
{
    iccd_response_type_t response_type;
    ab_data_t ab_data;
    iccd_error_t error = 0;
    iccd_xfer_t *iccd_xfer = (iccd_xfer_t *)priv;
    jiccd_softc_t *sc = iccd_xfer->sc;
    int pos = 0;
    jresult_t usb_status = 0;

#ifdef JDEBUG
    if (status)
    {
        DBG_E(DHOST_ICCD_API, ("data_block_done: Error occurred in transfer "
            "error is %s\n", usbd_errstr(status)));
    }
#endif

    /* Getting the response type */
    UNPACK_BYTE((juint8_t *)xfer->buffer, pos, response_type);

    /* If the data contains status information unpacking it */
    if (response_type == UWH_ICCD_BRESPONSE_BSTATUS_ERROR)
    {
        UNPACK_BYTE((juint8_t *)xfer->buffer, pos, ab_data.status);
        
        /* If there was an error parsing bError field*/
        if (ab_data.status & ICCD_STATUS_FAILED_MASK)
        {
            UNPACK_BYTE((juint8_t *)xfer->buffer, pos, error);
            ab_data.status &= ~ICCD_STATUS_FAILED_MASK;
        }
    }
    else if (response_type == UWH_ICCD_BRESPONSE_POLLING)
    {
        UNPACK_WORD((juint8_t *)xfer->buffer, pos, ab_data.delay);
    }

    sc->pending_ctrl_req = 0; 
    
    /* Calling the application callback */
    iccd_xfer->cb(sc->os_ctx, xfer->buffer, xfer->actlen, response_type,
        ab_data, error, iccd_xfer->arg, usb_status);
}

static void xfer_block_done(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status)
{
    iccd_xfer_t *iccd_xfer = (iccd_xfer_t *)priv;
    jiccd_softc_t *sc = iccd_xfer->sc;
    jresult_t usb_status = 0;
    ab_data_t ab_data = {0};

#ifdef JDEBUG
    if (status)
    {
        DBG_E(DHOST_ICCD_API, ("xfer_block_done: Error occurred in transfer "
            "error is %s\n", usbd_errstr(status)));
    }
#endif

    sc->pending_ctrl_req = 0; 
    
    /* Calling the application callback */
    iccd_xfer->cb(sc->os_ctx, xfer->buffer, xfer->actlen, 0, ab_data, 0, 
        iccd_xfer->arg, usb_status);
}


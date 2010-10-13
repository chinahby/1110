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

/***************************************************
 * CDC General Class Driver Library Implementation *
 ***************************************************/

#include "jcdc_general.h"

static jresult_t cdc_async_transfer(cdc_cmd_priv_t *priv, 
    usbd_pipe_handle pipe,  void *membuf, juint32_t size, 
    usbd_xfer_handle xfer);
static void cdc_data_xfer_complete(usbd_xfer_handle xfer, 
    usbd_private_handle priv, usbd_status rc);
static jresult_t cdc_xfer_data_pipe(cdc_cmd_priv_t *priv, 
    usbd_pipe_handle pipe, void *membuf, juint32_t size);
static void jcdc_send_encap_cb(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status);

jresult_t status_to_result(usbd_status status)
{
    switch (status)
    {
    case USBD_NORMAL_COMPLETION:
        return 0;
    case USBD_IN_PROGRESS:
        return JEBUSY;
    case USBD_NOMEM:
        return JENOMEM;
    case USBD_IOERROR:
        return JEIO;
    case USBD_TIMEOUT:
        return JETIMEDOUT;
    case USBD_INTERRUPTED:
        return JEINTR;
    case USBD_TEST_FAILURE:
        return JETEST_FAILED;
    case USBD_INVALID_STATE:
    case USBD_INVAL:
        return JEINVAL;
    case USBD_CANCELLED:
        return ECANCEL;
    case USBD_PENDING_REQUESTS:
    case USBD_NOT_STARTED:
    case USBD_BAD_ADDRESS:
    case USBD_IN_USE:
    case USBD_NO_ADDR:
    case USBD_SET_ADDR_FAILED:
    case USBD_NO_POWER:
    case USBD_TOO_DEEP:
    case USBD_NOT_CONFIGURED:
    case USBD_SHORT_XFER:
    case USBD_STALLED:
    default:
        break;
    }

    return EUNKNOWN;
}

/* Build encapsulated cmd / response and transfer it */
static jresult_t encapsulated_transfer(cdc_cmd_priv_t *priv, void *membuf, 
    juint16_t size)
{
    usbd_status status = USBD_NORMAL_COMPLETION;
    usbd_xfer_handle xfer = NULL;
    jcdc_dev_softc_t *sc = priv->sc;
    usb_device_request_t req_t;
    jbool_t in_xfer = (priv->req_type == SEND_ENCAP_CMD) ? 0 : 1;
    DECLARE_FNAME("encapsulated_transfer");

    DBG_X(DHOST_CDC_GENERAL, ("%s: Entered\n", fname));
    xfer = usbd_alloc_xfer(sc->device);
    if (!xfer)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Failed to allocate xfer\n", fname));
        return JENOMEM;
    }

    /* Build request according to xfer direction */
    req_t.bmRequestType = (in_xfer) ? UT_READ_CLASS_INTERFACE : 
        UT_WRITE_CLASS_INTERFACE;
    req_t.bRequest = (in_xfer) ? CDC_GET_ENC_RESPONSE : 
        CDC_SEND_ENC_COMMAND;
    USETW(req_t.wValue, 0);
    USETW(req_t.wIndex, (juint16_t)sc->c_iface->index);
    USETW(req_t.wLength, size);

    /* setup xfer */
    usbd_setup_default_xfer(xfer, priv->sc->device, priv,
        CDC_CONTROL_XFER_TIMEOUT, &req_t, membuf, size, 0, jcdc_send_encap_cb);

    /* Send the request over default pipe (EP0) */
    status = usbd_transfer(xfer);
    if (status && status != USBD_IN_PROGRESS)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Failed transferring control request, "
            "error %d\n", j_device_get_nameunit(priv->sc->dev), status));
        usbd_free_xfer(xfer);
        return status_to_result(status);
    }

    return 0;
}

static void cdc_uninit_request_priv(cdc_cmd_priv_t *priv)
{
    jfree(priv);
}

static cdc_cmd_priv_t *cdc_init_request_priv(jcdc_dev_softc_t *sc,
    cdc_req_type type, uwh_cdc_app_h app_ctx, void *app_priv)
{
    cdc_cmd_priv_t *priv = (cdc_cmd_priv_t *)jmalloc(sizeof(cdc_cmd_priv_t), 
        M_ZERO);
    if (priv)
    {
        priv->sc = sc;
        priv->req_type = type;
        priv->app_ctx = app_ctx;
        priv->app_priv = app_priv;
    }

    return priv;
}

/* Class specific requests */
static jresult_t cdc_start_encapsulated_xfer(uwh_cdc_dev_h dev_ctx,
    void *membuf, juint16_t size, cdc_general_command_cb_t c_cb,
    cdc_general_data_cb_t r_cb, uwh_cdc_app_h app_ctx, void *app_priv,
    cdc_req_type type)
{
    jresult_t rc = JENOMEM;
    cdc_cmd_priv_t *priv = NULL;
    DECLARE_FNAME("cdc_start_encapsulated_xfer");

    DBG_X(DHOST_CDC_GENERAL, ("%s: Entered\n", fname));

    if (!IS_CDC_SC_FULL(*(GENERAL_CDC_SC(dev_ctx))))
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Operation not supported\n",
            fname));
        rc = JEINVAL;
        goto Error; 
    }
    
    priv = cdc_init_request_priv(GENERAL_CDC_SC(dev_ctx), type, app_ctx, 
        app_priv);
    if (!priv)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Failed to allocate private handle\n",
            fname));
        goto Error;
    }
    if (type == SEND_ENCAP_CMD)
        priv->cdc_callback.command_cb = c_cb;
    else
        priv->cdc_callback.data_cb = r_cb;

    rc = encapsulated_transfer(priv, membuf, size);

Error:
    if (rc)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Failed [%d]\n", fname, rc));
        if (priv)
            cdc_uninit_request_priv(priv);
    }
    return rc;
}

/**
 * Function name:  cdc_encapsulated_command_send
 * Description:    Handles a Send Encapsulated Command request from the
 *                 application.
 * Parameters:
 *     @dev_ctx: (IN) CDC class driver context.
 *     @cmd:     (IN) Handle to a buffer containing the command to send.
 *     @size:    (IN) The size of the command's data.
 *     @cb:      (IN) Application completion callback.
 *     @app_ctx: (IN) Application context.
 *
 * Return value:   0 on success, otherwise an error code.   
 * Scope:          Global
 **/
jresult_t cdc_encapsulated_command_send(uwh_cdc_dev_h dev_ctx, void *cmd, 
    juint16_t size, cdc_general_command_cb_t cb, uwh_cdc_app_h app_ctx,
    void *app_priv)
{
    DECLARE_FNAME("cdc_encapsulated_command_send");
    
    DBG_X(DHOST_CDC_GENERAL, ("%s:: Entered\n", fname));
    
    if ((GENERAL_CDC_SC(dev_ctx))->state < CDC_DEV_ATTACHED)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: device is not enabled\n", fname));
        return USBD_INVALID_STATE;
    }

    return cdc_start_encapsulated_xfer(dev_ctx, cmd, size, cb, NULL, app_ctx,
        app_priv, SEND_ENCAP_CMD);
}

/**
 * Function name:  cdc_encapsulated_response_get
 * Description:    Handles a Get Encapsulated Response request from the
 *                 application.
 * Parameters:
 *     @dev_ctx: (IN) CDC class driver context.
 *     @membuf:  (IN) Handle to a buffer for storing the returned response.
 *     @size:    (IN) The size of the command's data.
 *     @cb:      (IN) Application completion callback.
 *     @app_ctx: (IN) Application context.
 *
 * Return value:   0 on success, otherwise an error code.   
 * Scope:          Global
 **/
jresult_t cdc_encapsulated_response_get(uwh_cdc_dev_h dev_ctx,
    void *membuf, juint16_t size, cdc_general_data_cb_t cb,
    uwh_cdc_app_h app_ctx, void *app_priv)
{
    DECLARE_FNAME("cdc_encapsulated_response_get");
    
    DBG_X(DHOST_CDC_GENERAL, ("cdc_encapsulated_response_get: Entered\n"));
    
    if ((GENERAL_CDC_SC(dev_ctx))->state < CDC_DEV_ATTACHED)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: device is not enabled\n", fname));
        return USBD_INVALID_STATE;
    }

    return cdc_start_encapsulated_xfer(dev_ctx, membuf, size, NULL, cb,
        app_ctx, app_priv, GET_ENCAP_RESPONSE);
}

/* Application/user communication */

static jresult_t cdc_start_data_xfer(uwh_cdc_dev_h dev_ctx, void *membuf, 
    juint32_t size, cdc_general_data_cb_t cb, 
    uwh_cdc_app_h app_ctx, void *app_priv, cdc_req_type type)
{
    jresult_t rc = JENOMEM;
    usbd_pipe_handle pipe;
    cdc_cmd_priv_t *priv;
    DECLARE_FNAME("cdc_start_data_xfer");

    DBG_X(DHOST_CDC_GENERAL, ("%s: Entered\n", fname));

    priv = cdc_init_request_priv(GENERAL_CDC_SC(dev_ctx), type, app_ctx,
        app_priv);
    if (!priv)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Failed to allocate private handle\n",
            fname));
        goto Error;
    }
    
    pipe = (type == SEND_DATA) ? priv->sc->bulk_pipe_out : 
        priv->sc->bulk_pipe_in;
    priv->cdc_callback.data_cb = cb;    

    rc = cdc_xfer_data_pipe(priv, pipe, membuf, size);

Error:
    if (rc)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Failed\n", fname));
        if (priv)
            cdc_uninit_request_priv(priv);
    }
    return rc;
}

/**
 * Function name:  cdc_data_get
 * Description:    Performs an asynchronous data transfer from the device to the
 *                 host.
 * Parameters:
 *     @dev_ctx: (IN) CDC class driver context.
 *     @membuf:  (IN) Handle to a buffer for storing the read data.
 *     @size:    (IN) The amount of bytes to transfer.
 *     @cb:      (IN) Application completion callback.
 *     @app_ctx: (IN) Application context.
 *
 * Return value:   0 on success, otherwise an error code.   
 * Scope:          Global
 **/
jresult_t cdc_data_get(uwh_cdc_dev_h dev_ctx, void *membuf, juint32_t size,
    cdc_general_data_cb_t cb, uwh_cdc_app_h app_ctx, void *app_priv)
{
    DECLARE_FNAME("cdc_data_get");

    DBG_X(DHOST_CDC_GENERAL, ("%s: Entered\n", fname));
    
    if ((GENERAL_CDC_SC(dev_ctx))->state < CDC_DEV_ENABLED)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: device is not enabled\n", fname));
        return USBD_INVALID_STATE;
    }

    return cdc_start_data_xfer(dev_ctx, membuf, size, cb, app_ctx, app_priv,
        GET_DATA);
}

/**
 * Function name:  cdc_data_send
 * Description:    Performs an asynchronous data transfer from the host to the
 *                 device.
 * Parameters:
 *     @dev_ctx: (IN) CDC class driver context.
 *     @membuf:  (IN) Handle to a buffer containing the data to send.
 *     @size:    (IN) The amount of bytes to transfer.
 *     @cb:      (IN) Application completion callback.
 *     @app_ctx: (IN) Application context.
 *
 * Return value:   0 on success, otherwise an error code.   
 * Scope:          Global
 **/
jresult_t cdc_data_send(uwh_cdc_dev_h dev_ctx, void *membuf, 
    juint32_t size, cdc_general_data_cb_t cb, uwh_cdc_app_h app_ctx, 
    void *app_priv)
{
    DECLARE_FNAME("cdc_data_send");

    DBG_X(DHOST_CDC_GENERAL, ("cdc_data_send: Entered\n"));
    
    if ((GENERAL_CDC_SC(dev_ctx))->state < CDC_DEV_ENABLED)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: device is not enabled\n", fname));
        return USBD_INVALID_STATE;
    }

    return cdc_start_data_xfer(dev_ctx, membuf, size, cb, app_ctx,
          app_priv, SEND_DATA);
}

static jresult_t cdc_xfer_data_pipe(cdc_cmd_priv_t *priv, 
    usbd_pipe_handle pipe, void *membuf, juint32_t size)
{
    jresult_t rc;
    usbd_xfer_handle xfer = NULL;
    DECLARE_FNAME("cdc_xfer_data_pipe");
    
    DBG_X(DHOST_CDC_GENERAL, ("%s: Entered\n", fname));

    rc = cdc_async_transfer(priv, pipe, membuf, size, xfer);

    DBG_RUN(DHOST_CDC_GENERAL, DL_EX_VERBOSE, if (rc)
        DBG_E(DHOST_CDC_GENERAL, ("%s: Error in xfer %p [%d]\n", fname, xfer, 
        rc)));

    return rc;
}

/* General CDC asynchronous transfer function */
static jresult_t cdc_async_transfer(cdc_cmd_priv_t *priv, 
    usbd_pipe_handle pipe, void *membuf, juint32_t size, 
    usbd_xfer_handle xfer)
{
    jresult_t rc;
    usbd_status status;
    juint16_t flags = USBD_SHORT_XFER_OK | USBD_FORCE_SHORT_XFER;
    DECLARE_FNAME("cdc_async_transfer");
    
    DBG_X(DHOST_CDC_GENERAL, ("%s: Entered\n", fname));

    /* Allocate a transfer request object and set up a xfer */
    xfer = usbd_alloc_xfer(priv->sc->device);
    if (!xfer)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Failed to allocate xfer\n", fname));
        rc = JENOMEM;
        goto Error;
    }
    usbd_setup_xfer(xfer, pipe, (void *)priv, membuf, size, flags, 
        CDC_DATA_XFER_TIMEOUT, cdc_data_xfer_complete);

    /* Execute xfer */
    status = usbd_transfer(xfer);
    if (status && status != USBD_IN_PROGRESS) 
    {
        DBG_W(DHOST_CDC_GENERAL, ("%s: failed to setup transfer, %s\n", fname, 
            usbd_errstr(status)));
        rc = status_to_result(status);
        goto Error;
    }
    return 0;

Error:
    if (xfer)
        usbd_free_xfer(xfer);

    return rc;
}

/* Command completion callback - passed to the Core for async transfers */
static void jcdc_send_encap_cb(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status rc)
{
    /* Activate application's callback according to priv */
    cdc_cmd_priv_t *cmd_priv = (cdc_cmd_priv_t *)priv;
    DECLARE_FNAME("jcdc_send_encap_cb");

    DBG_X(DHOST_CDC_GENERAL, ("%s: Entered\n", fname));
    
    if (cmd_priv->req_type == SEND_ENCAP_CMD)
    {
        cmd_priv->cdc_callback.command_cb(cmd_priv->app_ctx, 
            cmd_priv->app_priv, status_to_result(rc));
    }
    else
    {
        cmd_priv->cdc_callback.data_cb(cmd_priv->app_ctx, cmd_priv->app_priv, 
            xfer->buffer, xfer->actlen, status_to_result(rc));
    }

    usbd_free_xfer(xfer);
    cdc_uninit_request_priv(cmd_priv);
}

/* Transfer completion callback - passed to the Core for async transfers */
static void cdc_data_xfer_complete(usbd_xfer_handle xfer, 
    usbd_private_handle priv, usbd_status rc)
{
    cdc_cmd_priv_t *xfer_priv = (cdc_cmd_priv_t *)priv;

    DECLARE_FNAME("cdc_data_xfer_complete");

    KASSERT(xfer, ("%s: Null xfer handle\n", fname));

    DBG_V(DHOST_CDC_GENERAL, ("%s: Data xfer %p complete, status %d\n", fname,
        xfer, rc));

    switch (rc)
    {
    case 0:
        break;
    case USBD_CANCELLED:
        /* xfer has been canceled (abort_pipe was called) */
        DBG_W(DHOST_CDC_GENERAL, ("%s: xfer %p was canceled\n", fname, xfer));
        break;

    default:
        DBG_E(DHOST_CDC_GENERAL, ("%s: xfer completed with %d\n", fname, rc));
    }
    
    /* Activate callback */
    if (xfer_priv->req_type == GET_DATA || xfer_priv->req_type == SEND_DATA)
    {
        xfer_priv->cdc_callback.data_cb(xfer_priv->app_ctx, 
            xfer_priv->app_priv, xfer->buffer, xfer->actlen, 
            status_to_result(rc));
    }
#ifdef JDEBUG
    else
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Unknown request type %d\n", fname, 
            xfer_priv->req_type));
    }
#endif

    usbd_free_xfer(xfer);
    cdc_uninit_request_priv(xfer_priv);
}

/* Service functions for CDC classes */

jresult_t cdc_open_data_pipes(uwh_cdc_dev_h dev_ctx)
{
    jresult_t rc = 0;
    jcdc_dev_softc_t *sc = (jcdc_dev_softc_t *)dev_ctx;
    DECLARE_FNAME("cdc_open_data_pipes");

    DBG_V(DHOST_CDC_GENERAL, ("%s: Entered\n", fname));

    /* Open bulk pipes */
    rc = usbd_open_pipe(sc->d_iface, sc->bulk_ep_in, 
        USBD_EXCLUSIVE_USE, &sc->bulk_pipe_in);
    if (rc)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Error opening bulk in pipe.\n",
            fname));
        return rc;
    }
    
    rc = usbd_open_pipe(sc->d_iface, sc->bulk_ep_out, 
        USBD_EXCLUSIVE_USE, &sc->bulk_pipe_out);
    if (rc)
    {
        DBG_E(DHOST_CDC_GENERAL, ("%s: Error opening bulk out pipe.\n",
            fname));
        usbd_close_pipe(sc->bulk_pipe_in);
    }

    return rc;
}

void cdc_close_data_pipes(uwh_cdc_dev_h dev_ctx)
{
    jcdc_dev_softc_t *sc = (jcdc_dev_softc_t *)dev_ctx;

    if (sc->bulk_pipe_in)
    {
        usbd_abort_pipe(sc->bulk_pipe_in);
        usbd_close_pipe(sc->bulk_pipe_in);
        sc->bulk_pipe_in = NULL;
    }
    if (sc->bulk_pipe_out)
    {
        usbd_abort_pipe(sc->bulk_pipe_out);
        usbd_close_pipe(sc->bulk_pipe_out);
        sc->bulk_pipe_out = NULL;
    }
}

void cdc_abort_data_pipes(uwh_cdc_dev_h dev_ctx)
{
    jcdc_dev_softc_t *sc = (jcdc_dev_softc_t *)dev_ctx;

    if (sc->bulk_pipe_in)
        usbd_abort_pipe(sc->bulk_pipe_in);

    if (sc->bulk_pipe_out)
        usbd_abort_pipe(sc->bulk_pipe_out);
}

usb_descriptor_t *cdc_find_desc(usb_config_descriptor_t *conf_d, 
    check_desc_func func, void *arg)
{
    juint8_t length;
    juint8_t *desc_ptr, *end;
    usb_descriptor_t *desc = NULL;
    DECLARE_FNAME("cdc_find_desc");

    desc_ptr = (juint8_t *)conf_d;
    desc = (usb_descriptor_t *)desc_ptr;
    end = desc_ptr + ua_le16toh(conf_d->wTotalLength);
    length = conf_d->bLength;

    DBG_X(DHOST_CDC_GENERAL, ("%s: Entered - desc_ptr %p, end %p\n", fname, 
        desc_ptr, end));

    while ((desc_ptr + length <= end) && length)
    {
        if (func(desc, arg))
            goto Exit;

        /* Get the next interface descriptor */
        desc_ptr += length;
        desc = (usb_descriptor_t *)desc_ptr;
        length = desc->bLength;
    }

    desc = NULL;
    DBG_V(DHOST_CDC_GENERAL, ("%s: Exiting. No descriptor match\n", fname));
    
Exit:
    return desc;
}


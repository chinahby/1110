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
#include <jusb_common.h>
#include <jusb_core_fd.h>
#include <jusb_chp9.h>
#include "cdc_fd.h"

/*
 * Host to Device - Requests
 */

/**
 * Function name:  set_comm_feature_complete
 * Description:  completion handler for SET_COMM_FEATURE request
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * Scope: local
 **/
static void set_comm_feature_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    acm_callbacks_t *cbs = &ctx->cbs.acm;
    acm_comm_fs_t selector;
    union acm_comm_feature_setting_t status;
    jresult_t rc;
    DECLARE_FNAME("set_comm_feature_complete");

    DBG_X(DSLAVE_CDC_ACM, ("%s: entered\n", fname));

    if (cdc_get_iostatus(req, 1) != CDC_TRANSFER_OK)
        goto Error;
    
    /* Pass data to application */
    selector = (acm_comm_fs_t)(le16toh(ctx->setup_packet.wValue));
    status.abstract_state = le16toh(*(juint16_t*)(req->buffer.vaddr));

    if (cbs->set_comm_feature_h(selector, &status, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: application failed to "
            "handle set_comm_feature request\n", fname));
        goto Error;
    }
    
    rc = cdc_send_status(ctx);
    if (rc)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: failed to send status packet (%d)\n", rc)); 
        goto Error;
    }

    return;

Error:
    core_stall_pipe(ctx->core_ctx, 0, 1);
}

/**
 * Function name:  handle_set_comm_feature
 * Description:  handler for SET_COMM_FEATURE request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_set_comm_feature(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    acm_init_info_t *info = &ctx->info.acm;
    acm_callbacks_t *cbs = &ctx->cbs.acm;
    request_t *reply = NULL;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_set_comm_feature");

    DBG_X(DSLAVE_CDC_ACM, ("%s: entered\n", fname));

    /* Check against device capabilities */
    if (!(info->capabilities & ACM_CAP_COMM_FEATURE))
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unsuitable request for "
            "device capabilities\n", fname));
        goto Error;
    }

    if (le16toh(req->wLength) != 2)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unrecognized COMM_FEATURE format"
            " (wLength != %u)\n", fname, le16toh(req->wLength)) );
        goto Error;
    }

    if (!cbs->set_comm_feature_h)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: no handler defined for "
            "set_comm_feature request\n", fname));
        goto Error;
    }

    /* Save setup packet */
    j_memcpy(&ctx->setup_packet, req, sizeof(ctx->setup_packet));

    /* Setup reply */
    reply = cdc_create_ep0_read(ctx, 2);
    if (!reply)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: failed to setup reply\n", fname));
        goto Error;
    }

    reply->complete = set_comm_feature_complete;
    reply->context = ctx;

    rc = core_read_data(ctx->core_ctx, 0, reply);
    
Error:
    return rc;
}

/**
 * Function name:  get_comm_feature_complete
 * Description:  completion handler for GET_COMM_FEATURE request
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * Scope: local
 **/
static void get_comm_feature_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    acm_callbacks_t *cbs = &ctx->cbs.acm;

    DBG_X(DSLAVE_CDC_ACM, ("get_comm_feature_complete: entered\n"));

    if (cbs->get_comm_feature_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->get_comm_feature_complete_h(status, ctx->app_ctx);
    }
}

/**
 * Function name:  handle_get_comm_feature
 * Description:  handler for GET_COMM_FEATURE request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_get_comm_feature(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    acm_init_info_t *info = &ctx->info.acm;
    acm_callbacks_t *cbs = &ctx->cbs.acm;
    request_t *reply = NULL;
    acm_comm_fs_t selector;
    union acm_comm_feature_setting_t status;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_get_comm_feature");

    DBG_X(DSLAVE_CDC_ACM, ("%s: entered\n", fname));

    /* Check against device capabilities */
    if (!(info->capabilities & ACM_CAP_COMM_FEATURE))
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unsuitable request for "
            "device capabilities\n", fname));
        goto Error;
    }
            
    if (le16toh(req->wLength) != 2)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unrecognized COMM_FEATURE format"
            " (wLength != %u)\n", fname, le16toh(req->wLength)) );
        goto Error;
    }

    if (!cbs->get_comm_feature_h)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: no handler defined for "
            "set_comm_feature request\n", fname));
        goto Error;
    }

    /* Get data from application */
    selector = (acm_comm_fs_t)(le16toh(req->wValue));

    if (cbs->get_comm_feature_h(selector, &status, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: application failed to "
            "handle request\n", fname));
        goto Error;
    }

    /* Setup reply */
    status.abstract_state = htole16(status.abstract_state);
    reply = cdc_create_ep0_write(ctx, &status, 2, le16toh(req->wLength));

    if (!reply)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: failed to setup reply\n", fname));
        goto Error;
    }

    reply->complete = get_comm_feature_complete;
    reply->context = ctx;
    
    rc =  core_send_data(ctx->core_ctx, 0, reply);
    
Error:
    return rc;
}

/**
 * Function name:  handle_clear_comm_feature
 * Description:  handler for CLEAR_COMM_FEATURE request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_clear_comm_feature(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    acm_init_info_t *info = &ctx->info.acm;
    acm_callbacks_t *cbs = &ctx->cbs.acm;
    acm_comm_fs_t selector;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_clear_comm_feature");

    DBG_X(DSLAVE_CDC_ACM, ("%s: entered\n", fname));

    /* Check against device capabilities */
    if (!(info->capabilities & ACM_CAP_COMM_FEATURE))
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unsuitable request for "
            "device capabilities\n", fname));
        goto Error;
    }

    if (le16toh(req->wLength) != 0)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unrecognized COMM_FEATURE format"
            " (wLength != %u)\n", fname, le16toh(req->wLength)) );
        goto Error;
    }

    if (!cbs->clear_comm_feature_h)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: no handler defined for request\n", fname));
        goto Error;
    }

    /* Pass data to application */
    selector = (acm_comm_fs_t)(le16toh(req->wValue));
    if (cbs->clear_comm_feature_h(selector, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: application failed to handle request\n",
            fname));
        goto Error;
    }

    rc = cdc_send_status(ctx);
    
Error:
    return rc;
}

/**
 * Function name:  set_line_coding_complete
 * Description: completion handler for SET_LINE_CODING request
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void set_line_coding_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    acm_callbacks_t *cbs = &ctx->cbs.acm;
    acm_line_coding_t *le_coding;
    acm_line_coding_t coding;
    jresult_t rc;
    DECLARE_FNAME("set_line_coding_complete");

    DBG_X(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
 
    if (cdc_get_iostatus(req, 1) != CDC_TRANSFER_OK)
        goto Error;
    
    /* Pass data to application */
    le_coding = (acm_line_coding_t*)(req->buffer.vaddr);
    coding.dte_rate = le32toh(le_coding->dte_rate);
    coding.char_format = le_coding->char_format;
    coding.parity_type = le_coding->parity_type;
    coding.data_bits = le_coding->data_bits;
    
    if (cbs->set_line_coding_h(&coding, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: application failed to handle request\n", 
            fname));
        goto Error;
    }

    rc = cdc_send_status(ctx);
    if (rc)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: failed to send status packet (%d)\n", rc)); 
        goto Error;
    }

    return;

Error:
    core_stall_pipe(ctx->core_ctx, 0, 1);
}

/**
 * Function name:  handle_set_line_coding
 * Description:  handler for SET_LINE_CODING request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_set_line_coding(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    acm_init_info_t *info = &ctx->info.acm;
    acm_callbacks_t *cbs = &ctx->cbs.acm;
    request_t *reply = NULL;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_set_line_coding");

    DBG_X(DSLAVE_CDC_ACM, ("%s: entered\n", fname));

    /* Check against device capabilities */
    if (!(info->capabilities & ACM_CAP_LINE_CODING))
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unsuitable request for "
            "device capabilities\n", fname));
        goto Error;
    }

    if (le16toh(req->wLength) != sizeof(acm_line_coding_t))
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unrecognized LINE_CODING format"
            " (wLength != %u)\n", fname, le16toh(req->wLength)) );
        goto Error;
    }

    if (!cbs->set_line_coding_h)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: no handler defined for request\n"));
        goto Error;
    }

    /* Setup reply */
    reply = cdc_create_ep0_read(ctx, sizeof(acm_line_coding_t));
    if (!reply)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: failed to setup reply", fname));
        goto Error;
    }

    reply->complete = set_line_coding_complete;
    reply->context = ctx;
    
    rc = core_read_data(ctx->core_ctx, 0, reply);
    
Error:
    return rc;
}

/**
 * Function name:  get_line_coding_complete
 * Description:  completion handler for GET_LINE_CODING request
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void get_line_coding_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    acm_callbacks_t *cbs = &ctx->cbs.acm;

    DBG_X(DSLAVE_CDC_ACM, ("get_line_coding_complete: entered\n"));
    
    if (cbs->get_line_coding_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->get_line_coding_complete_h(status, ctx->app_ctx);
    }
}

/**
 * Function name:  handle_get_line_coding
 * Description:  handler for GET_LINE_CODING request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_get_line_coding(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    acm_init_info_t *info = &ctx->info.acm;
    acm_callbacks_t *cbs = &ctx->cbs.acm;
    request_t *reply = NULL;
    acm_line_coding_t coding;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_get_line_coding");

    DBG_X(DSLAVE_CDC_ACM, ("%s: entered\n", fname));

    /* Check against device capabilities */
    if (!(info->capabilities & ACM_CAP_LINE_CODING))
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unsuitable request for "
            "device capabilities\n", fname));
        goto Error;
    }

    if (le16toh(req->wLength) != sizeof(acm_line_coding_t))
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unrecognized LINE_CODING format"
            " (wLength != %u)\n", fname, le16toh(req->wLength)) );
        goto Error;
    }

    if (!cbs->get_line_coding_h)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: no handler defined for request\n"));
        goto Error;
    }

    /* Get data from application */
    if (cbs->get_line_coding_h(&coding, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: application failed to handle request\n"));
        goto Error;
    }

    coding.dte_rate = htole32(coding.dte_rate);
    
    /* Setup reply */
    reply = cdc_create_ep0_write(ctx, (void *)&coding, /* ADS 1.2 fix */
        sizeof(struct _acm_line_coding_t), le16toh(req->wLength));

    if (!reply)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: failed to setup reply", fname));
        goto Error;
    }

    reply->complete = get_line_coding_complete;
    reply->context = ctx;
    
    rc = core_send_data(ctx->core_ctx, 0, reply);
    
Error:
    return rc;
}

/**
 * Function name:  handle_set_line_state
 * Description:  handler for SET_LINE_STATE request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_set_line_state(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    acm_init_info_t *info = &ctx->info.acm;
    acm_callbacks_t *cbs = &ctx->cbs.acm;
    juint16_t ctrl_sig_bitmap = 0;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_set_line_state");

    DBG_X(DSLAVE_CDC_ACM, ("%s: entered\n", fname));

    /* Check against device capabilities */
    if (!(info->capabilities & ACM_CAP_LINE_CODING))
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unsuitable request for "
            "device capabilities\n", fname));
        goto Error;
    }

    if (!cbs->set_control_line_state_h)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: no handler defined for request\n"));
        goto Error;
    }

    /* Pass data to application */
    ctrl_sig_bitmap = le16toh(req->wValue);
    if (cbs->set_control_line_state_h(ctrl_sig_bitmap, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: application failed to handle request\n"));
        goto Error;
    }

    rc = cdc_send_status(ctx);

Error:
    return rc;
}

/**
 * Function name:  handle_send_break
 * Description:  handler for SEND_BREAK request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_send_break(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    acm_init_info_t *info = &ctx->info.acm;
    acm_callbacks_t *cbs = &ctx->cbs.acm;
    juint16_t break_duration = 0;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_send_break");

    DBG_X(DSLAVE_CDC_ACM, ("%s: entered\n", fname));

    /* Check against device capabilities */
    if (!(info->capabilities & ACM_CAP_SEND_BREAK))
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: unsuitable request for "
            "device capabilities\n", fname));
        goto Error;
    }

    if (!cbs->send_break_h)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: no handler defined for request\n"));
        goto Error;
    }

    /* Pass data to application */
    break_duration = le16toh(req->wValue);
    if (cbs->send_break_h(break_duration, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: application failed to handle request\n"));
        goto Error;
    }

    rc = cdc_send_status(ctx);
    
Error:
    return rc;
}

/**
 * Function name:  acm_handle_request
 * Description:  ACM model control interface request handler
 * Parameters: 
 *     @context: (IN) CDC context
 *     @buffer: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t acm_handle_request(void *context, void *buffer)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    struct usb_ctrlrequest *req = (struct usb_ctrlrequest*)buffer;
    jresult_t rc = JENOTSUP;

    DBG_X(DSLAVE_CDC_ACM, ("acm_handle_request: called with request (%u)\n",
        req->bRequest));
    
    switch(req->bRequest)
    {
    case CDC_REQ_SEND_ENCAPSULATED_COMMAND:
        rc = cdc_handle_encapsulated_command(ctx, req);
        break;
        
    case CDC_REQ_GET_ENCAPSULATED_RESPONSE:
        rc = cdc_handle_encapsulated_response(ctx, req);
        break;
        
    case CDC_REQ_SET_COMM_FEATURE:
        rc = handle_set_comm_feature(ctx, req);
        break;
        
    case CDC_REQ_GET_COMM_FEATURE:
        rc = handle_get_comm_feature(ctx, req);
        break;

    case CDC_REQ_CLEAR_COMM_FEATURE:
        rc = handle_clear_comm_feature(ctx, req);
        break;
        
    case CDC_REQ_SET_LINE_CODING:
        rc = handle_set_line_coding(ctx, req);
        break;
        
    case CDC_REQ_GET_LINE_CODING:
        rc = handle_get_line_coding(ctx, req);
        break;
        
    case CDC_REQ_SET_CONTROL_LINE_STATE:
        rc = handle_set_line_state(ctx, req);
        break;
        
    case CDC_REQ_SEND_BREAK:
        rc = handle_send_break(ctx, req);
        break;
        
    default:
        DBG_E(DSLAVE_CDC_ACM, ("acm_handle_request: unsupported ACM request "
            "(%u)\n", req->bRequest));
        break;
    }

    return rc;
}

/*
 * Device to Host - Notifications
 */

/**
 * Function name:  notify_serial_state_complete
 * Description:  completion handler for SERIAL_STATE notification
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void notify_serial_state_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    acm_callbacks_t *cbs = &(ctx->cbs.acm);

    DBG_X(DSLAVE_CDC_ACM, ("notify_serial_state_complete: entered\n"));

    if (cbs->notify_serial_state_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->notify_serial_state_complete_h(status, ctx->app_ctx);
    }

    core_request_free(req);
}

/**
 * Function name:  acm_notify_serial_state
 * Description:  Control Interface API - sends a SERIAL_STATE notification
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @serial_state: (IN) serial state (see 'enum acm_serial_state_t')
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t acm_notify_serial_state(cdc_handle_t handle, juint16_t serial_state)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    request_t *req = NULL;
    acm_init_info_t *info = &(ctx->info.acm);
    jresult_t rc = 0;

    if (ctx->intr_pipe.desc == NULL)
    {
        rc = JENXIO;
        goto Error;
    }

    /* Do we support this notification? */
    if (!(info->capabilities & ACM_CAP_LINE_CODING))
    {
        rc = JEPERM;
        goto Error;
    }

    serial_state = htole16(serial_state);
    req = cdc_create_notification(ctx, CDC_NTF_SERIAL_STATE, 0,
        ctx->intr_pipe.if_num, 2, (juint8_t*)&serial_state);

    if (!req)
    {
        rc = JENOMEM;
        goto Error;
    }
    
    req->complete = notify_serial_state_complete;
    req->context = ctx;

    rc = core_send_data(ctx->core_ctx, ctx->intr_pipe.desc, req);

Error:
    return rc;
}

/**
 * Function name:  notify_network_connection_complete
 * Description:  completion handler for a NETWORK_CONNECTION notification
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void notify_network_connection_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    acm_callbacks_t *cbs = &(ctx->cbs.acm);

    DBG_X(DSLAVE_CDC_ACM, ("notify_network_connection_complete: entered\n"));

    if (cbs->notify_net_conn_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->notify_net_conn_complete_h(status, ctx->app_ctx);
    }

    core_request_free(req);
}

/**
 * Function name:  acm_notify_network_connection
 * Description:  Control Interface API - sends a NETWORK_CONNECTION notification
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @link_up: (IN) 0 = link down, 1 = link up
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t acm_notify_network_connection(cdc_handle_t handle, jbool_t link_up)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    request_t *req = NULL;
    acm_init_info_t *info = &(ctx->info.acm);
    jresult_t rc = 0;

    if (ctx->intr_pipe.desc == NULL)
    {
        rc = JENXIO;
        goto Error;
    }

    /* Do we support this notification? */
    if (!(info->capabilities & ACM_CAP_NETWORK_CONNECTION))
    {
        rc = JEPERM;
        goto Error;
    }
    
    req = cdc_create_notification(ctx, CDC_NTF_NETWORK_CONNECTION,
        link_up?1:0, ctx->intr_pipe.if_num, 0, NULL);

    if (!req)
    {
        rc = JENOMEM;
        goto Error;
    }
    
    req->complete = notify_network_connection_complete;
    req->context = ctx;

    rc = core_send_data(ctx->core_ctx, ctx->intr_pipe.desc, req);

Error:
    return rc;
}

/*====================
 *  Initialization
 *===================*/

/**
 * Function name:  acm_get_functional_headers
 * Description:  this function is called by cdc_alloc_descriptors() to create
 *               the functional descriptors for ACM. It should:
 *               1. Return the total size required for the descriptors when
 *                  buffer is NULL
 *               2. Fill the buffer with descriptors when buffer is valid
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @buffer: (OUT) buffer into which functional headers should be written
 *     @is_single_iface: (IN) indicates a single interface 
 *     @interface_number: (IN) the max interface number allocated
 * Return value: total size, or 0 for failure
 * Scope: local
 **/
static juint32_t acm_get_functional_headers(cdc_ctx_t *ctx, void *buffer, 
    jbool_t is_single_iface, juint8_t interface_number)
{
    juint32_t size = 0;
    struct cdc_header_fdesc *hdr;
    struct cdc_call_mgmt_fdesc *cmgmt;
    struct cdc_acm_fdesc *acm;
    struct cdc_union_fdesc *un;

#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND

    struct cdc_mdlm_fdesc *mdlm;
    struct cdc_mdlm_detail_fdesc *mdlm_detail;
    const juint8_t guid[16] = QCOM_16_BYTES_GUID;

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */

    juint32_t capabilities = ctx->info.acm.capabilities;
    juint32_t cap_mask_acm = (ACM_CAP_COMM_FEATURE | ACM_CAP_LINE_CODING |
            ACM_CAP_SEND_BREAK | ACM_CAP_NETWORK_CONNECTION);
    juint32_t cap_mask_cmgmt = (ACM_CAP_CALL_MGMT | ACM_CAP_CALL_MGMT_OVER_DC);
    juint8_t avail_iface;
    juint8_t data_iface;

    DBG_X(DSLAVE_CDC_ACM, ("acm_get_functional_headers entered\n"));

    size = sizeof(struct cdc_header_fdesc) +
        sizeof(struct cdc_call_mgmt_fdesc) +
        sizeof(struct cdc_acm_fdesc) +
        sizeof(struct cdc_union_fdesc);

#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND

    size += (sizeof(struct cdc_mdlm_fdesc) + 
      sizeof(struct cdc_mdlm_detail_fdesc));

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */

    if (buffer)
    {
        avail_iface = interface_number;
        
        if (is_single_iface)
            data_iface = avail_iface;
        else 
            data_iface = avail_iface + 1;
        
        hdr = (struct cdc_header_fdesc*)buffer;
        hdr->bLength    = sizeof(struct cdc_header_fdesc);
        hdr->bType      = USB_DT_CS_INTERFACE;
        hdr->bSubtype   = USB_CDC_SUBTYPE_HEADER;
        ua_htole16(&hdr->bcdCDC, 0x0110);

        acm = (struct cdc_acm_fdesc*)(hdr+1);
        acm->bLength            = sizeof(struct cdc_acm_fdesc);
        acm->bType              = USB_DT_CS_INTERFACE;
        acm->bSubtype           = USB_CDC_SUBTYPE_ACM;
        acm->bmCapabilities     = (juint8_t)(capabilities & cap_mask_acm);

        cmgmt = (struct cdc_call_mgmt_fdesc*)(acm+1);
        cmgmt->bLength          = sizeof(struct cdc_call_mgmt_fdesc);
        cmgmt->bType            = USB_DT_CS_INTERFACE;
        cmgmt->bSubtype         = USB_CDC_SUBTYPE_CALL_MGMT;
        cmgmt->bmCapabilities   = (juint8_t)((capabilities & cap_mask_cmgmt)>>4);
        cmgmt->bDataIface       = data_iface;

        un = (struct cdc_union_fdesc*)(cmgmt+1);
        un->bLength             = sizeof(struct cdc_union_fdesc);
        un->bType               = USB_DT_CS_INTERFACE;
        un->bSubtype            = USB_CDC_SUBTYPE_UNION;
        un->bMasterIface        = avail_iface;
        un->bSlaveIface[0]      = data_iface;

#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND

        mdlm = (struct cdc_mdlm_fdesc*)(un+1);
        mdlm->bLength = sizeof(struct cdc_mdlm_fdesc);
        mdlm->bType = USB_DT_CS_INTERFACE;
        mdlm->bSubtype = USB_CDC_SUBTYPE_MDLM;
        ua_htole16(&mdlm->bcdVersion, 0x0120);
        memcpy((juint8_t*)(&(mdlm->bGUID)), guid, sizeof(mdlm->bGUID));

        mdlm_detail = (struct cdc_mdlm_detail_fdesc*)(mdlm+1);
        mdlm_detail->bLength = sizeof(struct cdc_mdlm_detail_fdesc);
        mdlm_detail->bType = USB_DT_CS_INTERFACE;
        mdlm_detail->bSubtype = USB_CDC_SUBTYPE_MDLM_DETAIL;
        mdlm_detail->bGUIDType = USB_CDC_MDLM_DETAIL_GUID_DISCRIMINATOR;
        mdlm_detail->bmCtrlCapabilities = 
          USB_CDC_MDLM_DETAIL_CONTROL_CAPS_VENDOR_SPECIFIC;
        mdlm_detail->bmDataCapabilities = 
          USB_CDC_MDLM_DETAIL_DATA_CAPS_BYTE_PADDING;

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */

    }

    return size;
}

/**
 * Function name:  cdc_init_acm
 * Description:  ACM FD initialization
 * Parameters: 
 *     @core_ctx: (IN) USB CORE context
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_init_acm(void *core_ctx)
{
    cdc_ctx_t *ctx = NULL;
    cdc_desc_init_t desc_info;
    jresult_t rc = 0;
    cdc_callbacks_t cbs;
    DECLARE_FNAME("cdc_init_acm");

    /* Create our context */
    ctx = jmalloc(sizeof(cdc_ctx_t), M_ZERO);
    if (!ctx)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: failed to allocate ctx\n", fname));
        rc = JENOMEM;
        goto Error;
    }
    
    ctx->core_ctx = core_ctx;

    /* Call application init function */
    ctx->info.basic.interface_number = UWD_UNDEFINED_INTERFACE;
    rc = acm_init(ctx, &ctx->cbs.acm, &ctx->info.acm,
        &ctx->app_ctx);
    if (rc)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: application failed to initialize\n",
            fname));
        goto Error;
    }

    cbs = ctx->cbs.basic;
    KASSERT((cbs.write_multi_complete_h && !cbs.write_complete_h) || 
        (!cbs.write_multi_complete_h && cbs.write_complete_h),
        ("using both single and multiple requests is not supported\n"));
    
    j_memset(&desc_info, 0, sizeof(cdc_desc_init_t));
    desc_info.cc_handler = acm_handle_request;
    desc_info.cc_ntf_supported = TRUE;
    desc_info.dc_supported = TRUE;
    desc_info.dc_empty_alt_0 = FALSE;

    if (ctx->info.acm.basic.flags & CDC_INIT_VENDOR_SPECIFIC_CLASS)
    {
        desc_info.cci_subclass = ctx->info.basic.cci_subclass;
        desc_info.cci_protocol = ctx->info.basic.cci_protocol;
    }
    else
    {
        desc_info.cci_subclass = USB_CDC_SUBCLASS_ACM;
        desc_info.cci_protocol = USB_CDC_CTRL_PROTO_AT;
    }

    if (!(ctx->info.acm.basic.flags & CDC_INIT_NO_CLASS_DESC))
        desc_info.get_func_headers = acm_get_functional_headers;

    if (ctx->info.acm.basic.flags & CDC_INIT_SINGLE_INTERFACE)
        desc_info.is_single_iface = TRUE;
    else
        desc_info.is_single_iface = FALSE;
        
    desc_info.interface_number = ctx->info.basic.interface_number;

    /* Create descriptors */
    rc = cdc_alloc_descriptors(ctx, &desc_info); 
    if (rc)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: failed to allocate descriptors.\n", fname));
        goto Error;
    }

    /* Initialize CDC functionality */
    rc = cdc_init(ctx);
    if (rc)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: failed on cdc_init().\n", fname));
        goto Error;
    }

    return rc;
    
Error:
    if (ctx)
    {
        cdc_free_descriptors(ctx);
        jfree(ctx);
    }
 
    return rc;
}


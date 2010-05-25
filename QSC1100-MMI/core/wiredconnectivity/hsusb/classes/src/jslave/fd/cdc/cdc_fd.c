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

static jresult_t notify_response_available(cdc_ctx_t *ctx);
static void reset_encap_state(cdc_ctx_t *ctx, jbool_t notify_app,
    cdc_iostatus_t status);

/**
 * Function name:  cdc_get_iostatus
 * Description:  translate CORE request status to CDC defined status 
 * Parameters: 
 *     @req: (IN) CORE request 
 *     @check_size: (IN) Should size be checked for a mismatch 
 *
 * Return value: CDC status 
 * Scope: global
 **/
cdc_iostatus_t cdc_get_iostatus(request_t *req, jbool_t check_size)
{
    switch(req->status)
    {
    case REQUEST_COMPLETED:
        if (check_size && (req->bytes_transferred != req->transfer_size))
        {
            DBG_E(DSLAVE_CDC, ("cdc_get_iostatus: transfer size error "
                "(%u/%u)\n", req->bytes_transferred, req->transfer_size));
            return CDC_TRANSFER_ERROR;
        }
        return CDC_TRANSFER_OK;

    case REQUEST_CANCELLED:
        DBG_I(DSLAVE_CDC, ("cdc_get_iostatus: transfer cancelled (%u/%u)\n",
            req->bytes_transferred, req->transfer_size));
        return CDC_TRANSFER_CANCELLED;

    case REQUEST_ERROR:
    default:
        DBG_E(DSLAVE_CDC, ("cdc_get_iostatus: transfer error (%u/%u)\n",
            req->bytes_transferred, req->transfer_size));
        return CDC_TRANSFER_ERROR;
    }
}

/**
 * Function name:  get_max_packet_size
 * Description:  returns the maximum packet size used for a given speed
 * Parameters: 
 *     @speed: (IN) USB connection speed (LS,FS,HS)
 *
 * Return value:  maximum packet size in bytes
 * Scope: local
 **/
static juint16_t get_max_packet_size(device_speed_t speed)
{
    if (speed == SPEED_HIGH)
        return 512;
    else
        return 64;
}

/*========================
 *     Data plane
 *=======================*/

/**
 * Function name:  read_complete
 * Description:  cdc_read() completion callback
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * Scope: local
 **/
static void read_complete(request_t *req)
{
    cdc_ctx_t* ctx = (cdc_ctx_t *)req->context;
    juint32_t completed_req = req->completed_req;
    request_t *curr_req;
    jint_t i;
    cdc_iostatus_t *status;
    juint32_t *size;
    void **buffer;
    jbool_t is_short_packet = 0;

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DSLAVE_CDC, ("read_complete: entered\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */

    status = jmalloc(sizeof(cdc_iostatus_t) * completed_req, M_ZERO);
    KASSERT(status, ("Not enough memory to allocate status buffer\n"));

    size = jmalloc(sizeof(juint32_t) * completed_req, M_ZERO);
    KASSERT(size, ("Not enough memory to allocate size buffer\n"));

    buffer = jmalloc(sizeof(void*) * completed_req, M_ZERO);
    KASSERT(buffer, ("Not enough memory to allocate buffer\n"));

    if ((ctx->cbs.basic.read_multi_complete_h && !req->single_request) || 
        (ctx->cbs.basic.read_complete_h && req->single_request))
    {
        for (i = 0, curr_req = req; i < completed_req && curr_req; 
            i++, curr_req = LIST_NEXT(curr_req, next_request_in_list))
        {
            status[i] = cdc_get_iostatus(curr_req, 0);

            /* We signal the application that the transfer is complete when a short
             * packet is recieved from the host. That is when either:
             * 1. The bytes transferred are less than requested
             * 2. The bytes_transferred == transfer_size, and the transfer size is
             *    NOT a multiple of (USB) max packet size
             * (When it is a multiple of max packet size, we expect the application
             * to call cdc_read again for which we will call it with 0 bytes
             * transferred and a TRUE transfer complete flag) */
            is_short_packet = (curr_req->bytes_transferred < 
                curr_req->transfer_size) || 
                (curr_req->transfer_size % ctx->usb_max_packet_size);
    
            if (is_short_packet)
            {
#ifdef CONFIG_DATA_PATH_DEBUG
                DBG_X(DSLAVE_CDC, ("read_complete: short packet.\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */
            }

            /* Copy received data to application's buffer */
            if (!(ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE))
            {
                j_memcpy(curr_req->caller_private, curr_req->buffer.vaddr,
                    curr_req->bytes_transferred);
            }
            size[i] = curr_req->bytes_transferred;
            buffer[i] = curr_req->caller_private;
        }

        if (req->single_request)
        {
            ctx->cbs.basic.read_complete_h(*status, req->caller_private, 
                req->bytes_transferred, is_short_packet, ctx->app_ctx);
        }
        else 
        {
            ctx->cbs.basic.read_multi_complete_h(status, buffer, size,
                completed_req, ctx->app_ctx);
        }
    }
    else
    {
        DBG_E(DSLAVE_CDC, ("read_complete: no callback provided "
            "for completion\n"));
    }

    core_request_free(req);

    if (status)
        jfree(status);

    if (size)
        jfree(size);

    if (buffer)
        jfree(buffer);
}

/**
 * Function name:  write_complete
 * Description:  cdc_write() completion callback
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * scope: local
 **/
static void write_complete(request_t *req)
{
    cdc_ctx_t* ctx = (cdc_ctx_t*)req->context;
    juint32_t completed_req = req->completed_req;
    cdc_iostatus_t *status = NULL;
    juint32_t *buf_sizes = NULL;
    void **buffers = NULL;
    jint_t i;
    request_t *curr_req;

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_X(DSLAVE_CDC, ("write_complete: entered\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */

    status = jmalloc(sizeof(cdc_iostatus_t) * completed_req, M_ZERO);
    KASSERT(status, ("Memory allocation for status buffer failed\n"));

    buf_sizes = jmalloc(sizeof(juint32_t) * completed_req, M_ZERO);
    KASSERT(buf_sizes, ("Memory allocation for buf_sizes failed\n"));

    buffers = jmalloc (sizeof(void*) * completed_req, M_ZERO);
    KASSERT(buffers, ("Memory allocation for buffers failed\n"));

    if ((ctx->cbs.basic.write_complete_h && req->single_request) ||
        (ctx->cbs.basic.write_multi_complete_h && !req->single_request))
    {
        for (i = 0, curr_req = req; i < completed_req && curr_req; 
            i++, curr_req = LIST_NEXT(curr_req, next_request_in_list))
        {
            status[i] = cdc_get_iostatus(curr_req, 1);
            buf_sizes[i] = curr_req->bytes_transferred;
            buffers[i] = curr_req->caller_private;
        }

        if (req->single_request)
        {
            ctx->cbs.basic.write_complete_h(*status, req->caller_private,
                ctx->app_ctx);
        }
        else 
        {
            ctx->cbs.basic.write_multi_complete_h(status, buffers, buf_sizes,
                completed_req, ctx->app_ctx);
        }
    }

    core_request_free(req);

    if (status)
        jfree(status);

    if (buf_sizes)
        jfree(buf_sizes);

    if (buffers)
        jfree(buffers);
}

/**
 * Function name:  cdc_handle_tx_completion
 * Description:  handle tx completion in the DCD
 * Parameters: 
 *     @handle: (IN) CDC handle
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_handle_tx_completion(cdc_handle_t handle)
{
    cdc_ctx_t *ctx = (cdc_ctx_t *)handle;

    if (!ctx->in_pipe.desc)
    {
        DBG_E(DSLAVE_CDC, ("cdc_handle_tx_completion: in pipe is not enabled"));
        return JEINVAL;
    }

    core_handle_tx_completion(ctx->core_ctx, ctx->in_pipe.desc);

    return 0;
}

/**
 * Function name:  cdc_write_multiple
 * Description:  send data to the USB host
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @buffers: (IN) data to send
 *     @buf_sizes: (IN) size of data to send in bytes
 *     @req_num: (IN) number of requests to send
 *     @timeout: (IN) timeout to complete the request
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_write_multiple(cdc_handle_t handle, void **buffers,
    juint32_t *buf_sizes, juint32_t req_num, juint32_t timeout)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    cdc_init_info_t *info = &ctx->info.basic;
    request_t *req = NULL;
    request_t *curr_req = NULL;
    request_t *start_req = NULL;
    jresult_t rc = 0;
    jint_t i;
    DECLARE_FNAME("cdc_write_multiple");

    /* Make sure we are within segment limits */
    for (i = 0; i < req_num; i++)
    {
        if (info->max_segment_size && (buf_sizes[i] > info->max_segment_size))
        {
            DBG_E(DSLAVE_CDC, ("%s: illegal size: %u (maxseg=%u)\n", fname, 
                buf_sizes[i], info->max_segment_size));
            rc = JEINVAL;
            goto Error;
        }
    }

    if (!ctx->in_pipe.desc)
    {
        DBG_E(DSLAVE_CDC, ("%s: in pipe is not enabled for this "
            "interface & setting\n", fname));
        rc = JEPERM;
        goto Error;
    }

    for (i = 0; i < req_num; i++)
    {
        if (ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE)
            req = core_request_alloc(buf_sizes[i], buffers[i]);
        else 
            req = core_request_alloc(buf_sizes[i], NULL);

        if (!req)
        {
            DBG_E(DSLAVE_CDC, ("%s: failed to allocate request\n", fname));
            rc = JENOMEM;
            goto Error;
        }

        if (!start_req)
            start_req = req;
        else 
            LIST_NEXT(curr_req, next_request_in_list) = req;

        curr_req = req;

        /* Save application pointer, and copy its buffer if necessary */
        req->caller_private = buffers[i];
        if (!(ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE))
            j_memcpy(req->buffer.vaddr, buffers[i], buf_sizes[i]);

        /* Use ZLP's to delineate data:
         * 1. On segment based mediums (such as Ethernet) and the data size is
         *    less than max_segment_size.
         * 2. The application explicitly requests to send ZLP
         */
        if (ctx->info.basic.zlp_terminator || 
            (ctx->info.basic.max_segment_size &&
            (buf_sizes[i] < ctx->info.basic.max_segment_size)))
        {
            req->zero = 1;
        }
        else
        {
            req->zero = 0;
        }
    
        req->complete = write_complete;
        req->context = ctx;
        req->transfer_size = buf_sizes[i];
        req->single_request = 0;
        req->timeout = timeout;
    }

    rc = core_send_data(ctx->core_ctx, ctx->in_pipe.desc, start_req);
    if (rc)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed on send_data\n", fname));
        goto Error;
    }
    
    return rc;

Error:
    if (start_req)
        core_request_free(start_req);
    
    return rc;
}

/**
 * Function name:  cdc_write
 * Description:  send data to the USB host
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @buffer: (IN) data to send
 *     @size: (IN) size of data to send in bytes
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_write(cdc_handle_t handle, void *buffer, juint32_t size)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    cdc_init_info_t *info = &ctx->info.basic;
    request_t *req = NULL;
    jresult_t rc = 0;
    DECLARE_FNAME("cdc_write");

    /* Make sure we are within segment limits */
    if (info->max_segment_size && (size > info->max_segment_size))
    {
        DBG_E(DSLAVE_CDC, ("%s: illegal size: %u (maxseg=%u)\n", fname,
            size, info->max_segment_size));
        rc = JEINVAL;
        goto Error;
    }

    if (!ctx->in_pipe.desc)
    {
        DBG_E(DSLAVE_CDC, ("%s: in pipe is not enabled for this "
            "interface & setting\n", fname));
        rc = JEPERM;
        goto Error;
    }
        
    if (ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE)
        req = core_request_alloc(size, buffer);
    else
        req = core_request_alloc(size, NULL);

    if (!req)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed to allocate request\n", fname));
        rc = JENOMEM;
        goto Error;
    }

    /* Save application pointer, and copy its buffer if necessary */
    req->caller_private = buffer;
    if (!(ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE))
        j_memcpy(req->buffer.vaddr, buffer, size);

    /* Use ZLP's to delineate data:
     * 1. On segment based mediums (such as Ethernet) and the data size is
     *    less than max_segment_size.
     * 2. The application explicitly requests to send ZLP
     */
    if (ctx->info.basic.zlp_terminator || 
        (ctx->info.basic.max_segment_size &&
        (size < ctx->info.basic.max_segment_size)))
    {
        req->zero = 1;
    }
    else
    {
        req->zero = 0;
    }
    
    req->complete = write_complete;
    req->context = ctx;
    req->transfer_size = size;

    /* Temp  fix CR51176: ZLP in 512 bytes transfer (OBEX)*/
    req->zero = 1;
    /* End temp fix*/

    rc = core_send_data(ctx->core_ctx, ctx->in_pipe.desc, req);
    if (rc < 0)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed on send_data\n", fname));
        goto Error;
    }
    
    return rc;

Error:
    if (req)
        core_request_free(req);
    
    return rc;
}

/**
 * Function name:  cdc_read
 * Description:  read data from the USB host 
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @buffer: (OUT) buffer where data will be placed
 *     @size: (IN) size of buffer
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_read_multiple(cdc_handle_t handle, void **buffers,
    juint32_t *buf_sizes, juint32_t req_num, juint32_t timeout)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    cdc_init_info_t *info = &ctx->info.basic;
    request_t *req = NULL;
    request_t *curr_req = NULL;
    request_t *start_req = NULL;
    jint_t i;
    jresult_t rc = 0;
    DECLARE_FNAME("cdc_read_multiple");

    for (i = 0; i < req_num; i++)
    {
        if (info->max_segment_size && (buf_sizes[i] > info->max_segment_size))
        {
            DBG_E(DSLAVE_CDC, ("%s: illegal size: %u (maxseg=%u)\n", fname, 
                buf_sizes[i], info->max_segment_size));
            rc = JEINVAL;
            goto Error;
        }
    }

    if (!ctx->out_pipe.desc)
    {
        DBG_E(DSLAVE_CDC, ("%s: out pipe is not enabled for this "
            "interface & setting\n", fname));
        rc = JEPERM;
        goto Error;
    }

    for (i = 0; i < req_num; i++)
    {
        if (ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE)
            req = core_request_alloc(buf_sizes[i], buffers[i]);
        else 
            req = core_request_alloc(buf_sizes[i], NULL);

        if (!req)
        {
            DBG_E(DSLAVE_CDC, ("%s: failed to allocate request\n", fname));
            return JENOMEM;
        }

        if (!start_req)
            start_req = req;
        else
            LIST_NEXT(curr_req, next_request_in_list) = req;

        curr_req = req;
        req->caller_private = buffers[i];
        req->complete = read_complete;
        req->context = ctx;
        req->transfer_size = buf_sizes[i];
        req->single_request = 0;
        req->timeout = timeout;
    }

    rc = core_read_data(ctx->core_ctx, ctx->out_pipe.desc, start_req);
    if (rc)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed on read_data\n", fname));
        goto Error;
    }
    
    return rc;

Error:
    if (start_req)
        core_request_free(start_req);
    
    DBG_E(DSLAVE_CDC, ("%s: rc =%d\n", fname, rc));
    return rc;
}

jresult_t cdc_read(cdc_handle_t handle, void *buffer, juint32_t size)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    cdc_init_info_t *info = &ctx->info.basic;
    request_t *req = NULL;
    jresult_t rc = 0;
    DECLARE_FNAME("cdc_read");

    /* Make sure we are within segment limits */
    if (info->max_segment_size && (size > info->max_segment_size))
    {
        DBG_E(DSLAVE_CDC, ("%s: illegal size: %u (maxseg=%u)\n", fname,
            size, info->max_segment_size));
        rc = JEINVAL;
        goto Error;
    }

    if (!ctx->out_pipe.desc)
    {
        DBG_E(DSLAVE_CDC, ("%s: out pipe is not enabled for this "
            "interface & setting\n", fname));
        rc = JEPERM;
        goto Error;
    }
    
    if (ctx->info.basic.flags & CDC_INIT_BUFFER_DMABLE)
        req = core_request_alloc(size, buffer);
    else
        req = core_request_alloc(size, NULL);

    if (!req)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed to allocate request\n", fname));
        rc = JENOMEM;
        goto Error;
    }

    /* Save application pointer */
    req->caller_private = buffer;

    req->complete = read_complete;
    req->context = ctx;
    req->transfer_size = size;
    req->single_request = 1;

    rc = core_read_data(ctx->core_ctx, ctx->out_pipe.desc, req);
    if (rc)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed on read_data\n", fname));
        goto Error;
    }
    
    return rc;

Error:
    if (req)
        core_request_free(req);
    
    return rc;
}

/**
 * Function name:  cdc_abort_write
 * Description:  cancel all write requests on the IN pipe
 * Parameters:
 *     @handle: (IN) CDC handle
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_abort_write(cdc_handle_t handle)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;

    if (!ctx->in_pipe.desc)
        return JENXIO;

    core_abort_pipe(ctx->core_ctx, ctx->in_pipe.desc);
    return 0;
}

/**
 * Function name:  cdc_abort_read
 * Description:  cancel all read requests on the OUT pipe
 * Parameters: 
 *     @handle: (IN) CDC handle 
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_abort_read(cdc_handle_t handle)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;

    if (!ctx->out_pipe.desc)
        return JENXIO;

    core_abort_pipe(ctx->core_ctx, ctx->out_pipe.desc);
    return 0;
}

/*===============================================
 *   Control class - Host to Device (Requests)
 *==============================================*/

/**
 * Function name:  create_request
 * Description:  create (or reallocate) a CORE request
 * Parameters: 
 *     @p_req: (IN/OUT) CORE request, or pointer to a NULL request pointer
 *     @buffer: (IN) request data (for write requests. NULL for read requests)
 *     @size: (IN) size of data in bytes (size of buffer, in case of read)
 *
 * Return value: CORE request, or NULL on failure 
 * Scope: local
 **/
static request_t* create_request(request_t **p_req, void *buffer, juint32_t size)
{
    request_t *req = NULL;
    DECLARE_FNAME("create_request");

    req = *p_req;

    /* Make sure we do not overlap requests */
    if (req && req->status == REQUEST_INPROGRESS)
    {
        DBG_E(DSLAVE_CDC, ("%s: overlapping replies - trying to allocate a "
            "request while it is still in progress!\n", fname));
        goto Error;
    }

    /* If this request is being reused, and has been flagged with an error or
     * cancel, we should flag it as ready before using it */
    if (req)
        req->status = REQUEST_READY;

    if (!req || (req->buffer.buffer_size < size))
    {
        request_t *new_req;
        
        /* Dont try to allocate 0 bytes... */
        new_req = core_request_alloc(size ? size : 10, NULL);
        if (!new_req)
        {
            DBG_E(DSLAVE_CDC, ("%s: error allocating request\n", fname));
            goto Error;
        }

        if (req)
            core_request_free(req);

        req = *p_req = new_req;
    }

    if (buffer && size)
        j_memcpy(req->buffer.vaddr, buffer, size);
    
    req->transfer_size = size;
    return req;

Error:
    return NULL;
}

/**
 * Function name:  cdc_create_ep0_read
 * Description:  allocate a CORE request for a CONTROL read
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @size: (IN) size of read buffer
 *
 * Return value: CORE request, or NULL on failure 
 * Scope: global
 **/
request_t* cdc_create_ep0_read(cdc_ctx_t *ctx, juint32_t size)
{
    if (!ctx)
    {
        DBG_E(DSLAVE_CDC, ("cdc_create_ep0_read: no context provided\n"));
        return NULL;
    }

    return create_request(&ctx->ctrl_read, NULL, size);
}

/**
 * Function name:  cdc_create_ep0_write
 * Description: allocate a CORE request for a CONTROL write
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @buffer: (IN) data to send (optional)
 *     @size: (IN) size of data to send
 *     @max_size: (IN) wLength value
 *
 * Return value: CORE request, or NULL on failure 
 * Scope: global
 **/
request_t* cdc_create_ep0_write(cdc_ctx_t *ctx, void *buffer, juint32_t size,
    juint32_t max_size)
{
    request_t *req = NULL;

    if (!ctx)
    {
        DBG_E(DSLAVE_CDC, ("cdc_create_ep0_write: no context provided\n"));
        goto Exit;
    }

    req = create_request(&ctx->ctrl_write, buffer, MIN(size, max_size));
    if (!req)
        goto Exit;

    /* If the host is expecting more data then we plan to send
     * then we should send a ZLP to tell him that we have sent
     * all we meant to */
    if (max_size > size)
        req->zero = 1;
    else
        req->zero = 0;

Exit:
    return req;
}

#ifdef JDEBUG
static void status_complete(request_t *req)
{
    DBG_X(DSLAVE_CDC, ("status_complete: entered\n"));
}
#endif

/**
 * Function name:  cdc_send_status
 * Description:  issue a status packet by sending a zero length packet on ep0
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_send_status(cdc_ctx_t *ctx)
{
    jresult_t rc = -1;
    request_t *reply;
    DECLARE_FNAME("cdc_send_status");

    DBG_X(DSLAVE_CDC, ("%s: entered\n", fname));

    reply = cdc_create_ep0_write(ctx, NULL, 0, 0);
    if (!reply)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed to setup status request\n", fname));
        return rc;
    }

#ifdef JDEBUG
    reply->complete = status_complete;
#else
    reply->complete = NULL;
#endif
    reply->context = NULL;

    rc = core_send_data(ctx->core_ctx, 0, reply);
    if (rc < 0)
        DBG_E(DSLAVE_CDC, ("%s: failed to send reply\n", fname));
    
    return rc;
}

/**
 * Function name:  encapsulated_command_complete
 * Description:  completion handler for the SEND_ENCAPSULATED_COMMAND request
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * Scope: local
 **/
static void encapsulated_command_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    cdc_callbacks_t *cbs = &ctx->cbs.basic;
    cdc_iostatus_t status;
    jresult_t rc;
    DECLARE_FNAME("encapsulated_command_complete");
    
    DBG_X(DSLAVE_CDC, ("%s: entered\n", fname));

    status = cdc_get_iostatus(req, 1);
    if (status != CDC_TRANSFER_OK)
        goto Error;

    /* Pass data to application */
    if (cbs->encap_command_h(req->buffer.vaddr, 
        (juint16_t)req->bytes_transferred, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC, ("%s: application failed to handle request\n", 
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
 * Function name:  cdc_handle_encapsulated_command
 * Description:  handler for the SEND_ENCAPSULATED_COMMAND request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) SETUP packet
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_handle_encapsulated_command(cdc_ctx_t *ctx, 
    struct usb_ctrlrequest *req)
{

    cdc_callbacks_t *cbs = &ctx->cbs.basic;
    request_t *reply = NULL;
    jresult_t rc;
    DECLARE_FNAME("cdc_handle_encapsulated_command");

    DBG_X(DSLAVE_CDC, ("%s: entered\n", fname));

    if (!cbs->encap_command_h)
    {
        DBG_E(DSLAVE_CDC, ("%s: no handler defined for request\n", fname));
        goto Error;
    }

    /* We use the host's commands here as a trigger for 'reminding' it
     * about responses that have not been retrieved yet */
    if (ctx->encap_response.state != CDC_ENCAP_READY)
    {
        rc = notify_response_available(ctx);
        if (rc < 0)
        {
            DBG_W(DSLAVE_CDC, ("%s: failed to send RESPONSE_AVAILABLE "
                "notification (rc=%d)\n", fname, rc));
        }
    }

    /* Setup data stage - get command */
    reply = cdc_create_ep0_read(ctx, le16toh(req->wLength));
    if (!reply)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed to setup read request", fname));
        goto Error;
    }

    reply->complete = encapsulated_command_complete;
    reply->context = ctx;

    return core_read_data(ctx->core_ctx, 0, reply);
    
Error:
    return -1;
}

/**
 * Function name:  encapsulated_response_complete
 * Description:  completion handler for GET_ENCAPSULATED_RESPONSE data stage
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void encapsulated_response_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    cdc_iostatus_t status;
    DECLARE_FNAME("encapsulated_response_complete");
    
    DBG_X(DSLAVE_CDC, ("%s: entered\n", fname));

    /* Did we just send a ZLDP? */
    if (ctx->encap_response.sending_zldp)
    {
        DBG_X(DSLAVE_CDC, ("%s: ZLDP sent\n", fname));

        /* Set our state: if there is a real response pending, set our
         * state to SENDING; otherwise get ready for next response */
        if (ctx->encap_response.size)
            ctx->encap_response.state = CDC_ENCAP_RESP_SENDING;
        else
            ctx->encap_response.state = CDC_ENCAP_READY;

        ctx->encap_response.sending_zldp = 0;

        return;
    }

    /* bytes_transferred is valid even when errors occur */
    ctx->encap_response.bytes_sent += req->bytes_transferred;

    status = cdc_get_iostatus(req, 1);
    if (status != CDC_TRANSFER_OK)
        goto Done;

    if (ctx->encap_response.state != CDC_ENCAP_RESP_SENDING)
    {
        DBG_E(DSLAVE_CDC, ("%s: inappropriate state for response "
            "completion (%u)\n", fname, ctx->encap_response.state));
        status = CDC_TRANSFER_ERROR;
        goto Done;
    }

    DBG_X(DSLAVE_CDC, ("%s: transfered %u bytes. total: %u [out of %u] bytes.\n", fname,
        req->bytes_transferred, ctx->encap_response.bytes_sent, ctx->encap_response.size));
        
    /* Are we done? */
    if (ctx->encap_response.bytes_sent >= ctx->encap_response.size)
        goto Done;

    return;
    
Done:
    /* Cleanup, Notify,  and prepare for next response */
    reset_encap_state(ctx, 1, status);
}

/**
 * Function name:  cdc_handle_encapsulated_response
 * Description:  handler for GET_ENCAPSULATED_RESPONSE request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) SETUP packet
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_handle_encapsulated_response(cdc_ctx_t *ctx, 
    struct usb_ctrlrequest *req)
{
    request_t *reply = NULL;
    juint32_t bytes_to_send;
    juint8_t  *buf = (juint8_t*)ctx->encap_response.buffer;
    juint32_t max_transfer_size = le16toh(req->wLength);
    jresult_t rc;
    DECLARE_FNAME("cdc_handle_encapsulated_response");

    DBG_X(DSLAVE_CDC, ("%s: entered\n", fname));

    /* WMCD, section 7: The host is allowed to poll for a response.
       If there is no response pending, send a zero length reply */
    if (ctx->encap_response.state == CDC_ENCAP_READY)
    {
        DBG_E(DSLAVE_CDC, ("%s: no pending response. sending ZLP.\n",
            fname));
        buf = NULL;
    }

    /* Send as much as we can, starting where we left off */
    bytes_to_send = MIN(max_transfer_size,
        ctx->encap_response.size - ctx->encap_response.bytes_sent);
    
    /* Send pending response to host - i.e. setup reply */
    reply = cdc_create_ep0_write(ctx, buf + ctx->encap_response.bytes_sent,
        bytes_to_send, max_transfer_size);
    if (!reply)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed to setup reply", fname));
        goto Error;
    }

    /* If this is an empty data phase (reply), we need to mark it as such */
    if (!buf)
    {
        ctx->encap_response.sending_zldp = 1;
        reply->ep0_zlp_reply = 1;
    }

    reply->complete = encapsulated_response_complete;
    reply->context = ctx;

    rc = core_send_data(ctx->core_ctx, 0, reply);
    if (rc)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed to send reply (%d)", fname, rc));
        goto Error;
    }   

    return 0;

Error:
    /* Cleanup, Notify, and prepare for next response */
    reset_encap_state(ctx, 1, CDC_TRANSFER_ERROR);
    ctx->encap_response.sending_zldp = 0;
    return -1;
}

/*====================================================
 *   Control class - Device to Host (Notifications)
 *===================================================*/

/**
 * Function name:  cdc_create_notification
 * Description:  allocate a CORE request for generating a notification, and fill
 *               out its header (i.e. SETUP packet format)
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @bNotification: (IN) 8-bit notification code
 *     @wValue: (IN) 16-bit wValue (as defined by CDC)
 *     @wIndex: (IN) 16-bit wIndex (as defined by CDC)
 *     @wLength: (IN) 16-bit wLength (as defined by CDC)
 *     @Data: (IN) notification data
 *
 * Return value: CORE request, or NULL on failure 
 * Scope: global
 **/
request_t* cdc_create_notification(cdc_ctx_t *ctx, juint8_t bNotification,
    juint16_t wValue, juint16_t wIndex, juint16_t wLength, juint8_t *Data)
{
    struct usb_ctrlrequest *ntf_hdr = NULL;
    request_t *req = NULL;
    juint8_t *ntf_data;
    juint32_t transfer_size = sizeof(struct usb_ctrlrequest)+wLength;
    DECLARE_FNAME("cdc_create_notification");

    if (!ctx)
    {
        DBG_E(DSLAVE_CDC, ("%s: no context provided\n", fname));
        return NULL;
    }

    req = core_request_alloc(transfer_size, NULL);
    if (!req)
    {
        DBG_E(DSLAVE_CDC, ("%s: error allocating interrupt request\n", fname));
        return NULL;
    }

    req->zero = 0;
    req->transfer_size = transfer_size;

    ntf_hdr = (struct usb_ctrlrequest*)(req->buffer.vaddr);
    ntf_hdr->bRequestType = USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE;
    ntf_hdr->bRequest = bNotification;
    ntf_hdr->wValue = htole16(wValue);
    ntf_hdr->wIndex = htole16(wIndex);
    ntf_hdr->wLength = htole16(wLength);

    if (wLength && Data)
    {
        ntf_data = (juint8_t*)(ntf_hdr+1);
        j_memcpy(ntf_data, Data, wLength);
    }
    
    return req;
}

/**
 * Function name:  response_available_complete
 * Description:  completion handler for the RESPONSE_AVAILABLE notification
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * Scope: local
 **/
static void response_available_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t *)req->context;
    DECLARE_FNAME("response_available_complete");

    DBG_X(DSLAVE_CDC, ("%s: entered\n", fname));
    ctx->encap_response.notification_pending = 0;

#ifdef JDEBUG
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        if (status != CDC_TRANSFER_OK && status != CDC_TRANSFER_CANCELLED)
        {
            DBG_E(DSLAVE_CDC, ("%s: error sending RESPONSE_AVAILABLE "
                "notification (%u).\n", fname, status));
        }
    }
#endif

    core_request_free(req);
    return;
}

/**
 * Function name:  notify_response_available
 * Description:  send the RESPONSE_AVAILABLE notification.
 *               this notification is not exported as part of the API.
 *               It is called implicitly by cdc_send_encap_response().
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: local
 **/
static jresult_t notify_response_available(cdc_ctx_t *ctx)
{
    request_t *req = NULL;
    jresult_t rc = 0;

    if (ctx->intr_pipe.desc == NULL)
    {
        rc = JENXIO;
        goto Exit;
    }

    if (ctx->encap_response.notification_pending)
        goto Exit;

    req = cdc_create_notification(ctx, CDC_NTF_RESPONSE_AVAILABLE, 0,
        ctx->intr_pipe.if_num, 0, NULL);

    if (!req)
    {
        rc = JENOMEM;
        goto Exit;
    }
    
    req->complete = response_available_complete;
    req->context = ctx;

    rc = core_send_data(ctx->core_ctx, ctx->intr_pipe.desc, req);
    if (rc)
        core_request_free(req);
    else                  
        ctx->encap_response.notification_pending = 1;

Exit:
    return rc;
}

/**
 * Function name:  cdc_send_encap_response
 * Description:  Control Class API function.
 *               This function sends a response for an encapsulated command
 *               received from the host.
 *               Note: queueing is not supported - a second call will return
 *               JEBUSY.
 * Parameters: 
 *     @handle: (IN) CDC handle 
 *     @response: (IN) buffer containing the response
 *     @size: (IN) size of the response
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_send_encap_response(cdc_handle_t handle, void *response,
    juint32_t size)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    jresult_t rc = 0;
    DECLARE_FNAME("cdc_send_encap_response");
    
    DBG_X(DSLAVE_CDC, ("%s: entered\n", fname));

    if (!size)
    {
        DBG_E(DSLAVE_CDC, ("%s: error! size can't be equal to zero\n", fname));
        return JEINVAL;
    }

    /* Check state for readiness */
    if (ctx->encap_response.state != CDC_ENCAP_READY)
    {
        DBG_E(DSLAVE_CDC, ("%s: busy! response already in progress (%u)\n",
            fname, ctx->encap_response.state));

        /* If the application gets impatient, we 'urge' the host to
         * retrieve the response */
        rc = notify_response_available(ctx);
        if (rc < 0)
        {
            DBG_W(DSLAVE_CDC, ("%s: failed to send RESPONSE_AVAILABLE "
                "notification (rc=%d)\n", fname, rc));
        }

        return JEBUSY;
    }

    KASSERT(!ctx->encap_response.buffer, ("Buffer already allocated!"));

    DBG_I(DSLAVE_CDC, ("%s: Allocating buffer - [%d] bytes\n",
        fname, size));

    /* allocating response buffer to the size of the response */
    ctx->encap_response.buffer = jmalloc(size, 0);

    if (!ctx->encap_response.buffer)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed to allocate response buffer.\n",
            fname));
        rc = JENOMEM;
        goto Error;
    }
    
    /* Hang response on our ctx, so that GET_ENCAPSULATED_RESPONSE can use it */
    j_memcpy(ctx->encap_response.buffer, response, size);
    ctx->encap_response.size = size;
    ctx->encap_response.bytes_sent = 0;
    ctx->encap_response.state = CDC_ENCAP_RESP_SENDING;
    
    /* Send RESPONSE_AVAILABLE notification */
    rc = notify_response_available(ctx);
    if (rc < 0)
    {
        DBG_E(DSLAVE_CDC, ("%s: failed to send RESPONSE_AVAILABLE "
            "notification (rc=%d)\n", fname, rc));
        goto Error;
    }

    return 0;

Error:
    /* In the event of an error (busy != error), we need to clean our state
     * to prepare for the next attempt */
    reset_encap_state(ctx, 0, CDC_TRANSFER_OK);
    return rc;
}

/*==========================================
 *    Initialization - internal functions
 *=========================================*/

/**
 * Function name:  cdc_free_descriptors
 * Description:  release memory allocated by cdc_alloc_descriptors()
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 *  Scope: global
 **/
void cdc_free_descriptors(cdc_ctx_t *ctx)
{
    if (ctx && ctx->desc)
    {
        juint8_t i;

        for (i = 0; i < ctx->desc->interface_count; i++)
        {
            if (ctx->desc->interfaces[i].if_string)
            {
                core_release_string_descriptor(ctx->core_ctx,
                    ctx->desc->interfaces[i].if_string);
            }
        }

        jfree(ctx->desc);
        ctx->desc = NULL;
    }
}

static jresult_t cdc_alloc_descriptors_mult(cdc_ctx_t *ctx, 
    cdc_desc_init_t *init_info);
static jresult_t cdc_alloc_descriptors_single(cdc_ctx_t *ctx, 
    cdc_desc_init_t *init_info);
/**
 * Function name:  cdc_alloc_descriptors
 * Description:  allocate memory for CDC descriptors, and fill them out.
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @init_info: (IN) information required to fill out CDC descriptors
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_alloc_descriptors(cdc_ctx_t *ctx, cdc_desc_init_t *init_info)
{
    jresult_t rc;

    if (!ctx || ctx->desc)
        return  JEINVAL;

    if (init_info->is_single_iface)
        rc = cdc_alloc_descriptors_single(ctx, init_info);
    else 
        rc = cdc_alloc_descriptors_mult(ctx, init_info);

    return rc;
}

/**
 * Function name:  cdc_alloc_descriptors_single
 * Description:  allocate memory for CDC descriptors, and fill them out.
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @init_info: (IN) information required to fill out CDC descriptors
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t cdc_alloc_descriptors_single(cdc_ctx_t *ctx, 
    cdc_desc_init_t *init_info)
{
    pipe_desc_t         *pipes = NULL;
    pipe_desc_t         *intr_pipe = NULL;
    pipe_desc_t         *data_pipes = NULL;
    alt_interface_desc_t *alts = NULL;
    interface_desc_t    *iface = NULL;
    fd_desc_t           *desc = NULL;
    void                *ex_desc = NULL;
    void                *curr = NULL;
    juint32_t            size;
    juint32_t            ex_desc_size = 0;
    juint8_t             num_alts = init_info->dc_empty_alt_0 ? 2 : 1;
    juint8_t             num_pipes = 0;
    jresult_t            rc = JENOMEM;
    juint8_t interface_number;

    if (init_info->interface_number == UWD_UNDEFINED_INTERFACE)
        interface_number = core_get_available_iface(ctx->core_ctx);
    else
        interface_number = init_info->interface_number;
    
    size = sizeof(fd_desc_t) +          /* desc */
        sizeof(alt_interface_desc_t)*num_alts +  /* alt */
        sizeof(interface_desc_t);       /* iface (iface) */
    
    if (init_info->get_func_headers)
    {
        ex_desc_size = init_info->get_func_headers(ctx, NULL, TRUE, 
            interface_number);
        size += ex_desc_size;
    }

    if (init_info->cc_ntf_supported)
        size += sizeof(pipe_desc_t);    /* intr pipe */
    
    if (init_info->dc_supported)
        size += sizeof(pipe_desc_t)*2; /* data pipes*/
    
    curr = jmalloc(size, M_ZERO);
    if (!curr)
        goto Exit;

    /* Make room for FD descriptor */
    desc = (fd_desc_t*)curr;
    curr = desc+1;

    pipes = (pipe_desc_t*)curr;
    if (init_info->cc_ntf_supported)
    {
        /* Setup interrupt pipe for notifications */
        intr_pipe = &pipes[0];
        intr_pipe->max_pkt_size_high            = 64;
        intr_pipe->max_pkt_size_full            = 64;
        intr_pipe->type                         = PIPE_INTR;
        intr_pipe->direction                    = DIRECTION_IN;
        intr_pipe->sync_type                    = 0;
        intr_pipe->usage_type                   = 0;
        intr_pipe->address                      = 0; /* Filled by Core */
        intr_pipe->transaction_per_microframe   = 0;
        intr_pipe->rate_u.poll_interval         = 5; /* 5ms in FS, 2ms in HS */
        num_pipes++; 
    }

    if (init_info->dc_supported)
    {
        data_pipes = &pipes[num_pipes];
        curr = data_pipes+2;
        data_pipes[0].max_pkt_size_high = get_max_packet_size(SPEED_HIGH);
        data_pipes[0].max_pkt_size_full = get_max_packet_size(SPEED_FULL);
        data_pipes[0].type              = PIPE_BULK;
        data_pipes[0].direction         = DIRECTION_IN;
        data_pipes[0].sync_type         = 0;
        data_pipes[0].usage_type        = 0;
        data_pipes[0].address           = 0; /* Filled by Core */
        data_pipes[0].transaction_per_microframe = 0;
        data_pipes[0].rate_u.nak_rate   = 32; /* HS: up to 1 NAK per 4 ms */

        data_pipes[1].max_pkt_size_high = get_max_packet_size(SPEED_HIGH);
        data_pipes[1].max_pkt_size_full = get_max_packet_size(SPEED_FULL);
        data_pipes[1].type              = PIPE_BULK;
        data_pipes[1].direction         = DIRECTION_OUT;
        data_pipes[1].sync_type         = 0;
        data_pipes[1].usage_type        = 0;
        data_pipes[1].address           = 0; /* Filled by Core */
        data_pipes[1].transaction_per_microframe = 0;
        data_pipes[1].rate_u.nak_rate   = 32; /* HS: up to 1 NAK per 4 ms */
        num_pipes += 2;
    }

    alts = (alt_interface_desc_t*)curr;

    curr = alts + num_alts;
    if (init_info->dc_empty_alt_0)
    {
        if (init_info->cc_ntf_supported)
        {
            alts[0].pipe_count = 1;  /* Alt setting without data */
            alts[0].pipes = pipes;
        }
        else 
        {
            alts[0].pipe_count = 0;  /* Empty alt setting */
            alts[0].pipes = NULL;
        }
        alts[1].pipe_count = num_pipes;  /* Data capabale alt setting */
        alts[1].pipes = pipes;
    }
    else
    {
        alts[0].pipe_count = num_pipes;  /* alt setting */
        alts[0].pipes = pipes;
    }

    /* Setup interface descriptors */
    iface = (interface_desc_t*)curr;
    curr = iface+1;

    iface->control_msg               = init_info->cc_handler;
    iface->alt_ifs                   = alts;
    iface->alt_if_num                = num_alts;
    iface->alt_if_curr               = 0;
    iface->extra_descriptor          = NULL;
    iface->extra_descriptor_size     = 0;
    iface->number                    = init_info->interface_number;
    /* HSU addition: HSU_STR_IDX_NOT_SET */
    iface->if_string = core_add_string_descriptor(ctx->core_ctx,
      ctx->info.basic.data_string, HSU_STR_IDX_NOT_SET);
    /* HSU addition end */
   
    if (ctx->info.basic.flags & CDC_INIT_VENDOR_SPECIFIC_CLASS)
        iface->if_class               = USB_CLASS_VENDOR_SPEC;
    else
        iface->if_class               = USB_CLASS_COMM;

    if (init_info->cci_subclass)
    {
        iface->if_subclass            = init_info->cci_subclass;
        iface->if_protocol            = init_info->cci_protocol;
    }
    else
    {
        iface->if_subclass            = USB_CLASS_VENDOR_SPEC;
        iface->if_protocol            = USB_CLASS_VENDOR_SPEC;        
    }
    
    /* Setup extra (class specific) descriptor */
    if (init_info->get_func_headers)
    {
        ex_desc = curr;
        curr = (char*)ex_desc + ex_desc_size;
        init_info->get_func_headers(ctx, ex_desc, TRUE, interface_number);
        iface->extra_descriptor = ex_desc;
        iface->extra_descriptor_size = ex_desc_size;
    }

    /* Setup FD descriptor */
    desc->speed = SPEED_HIGH;
    desc->interface_count = 1;
    desc->interfaces = iface;
    desc->iad = NULL;
    ctx->desc = desc;

    rc = 0;
Exit:
    
    return rc;
}

/**
 * Function name:  cdc_alloc_descriptors_mult
 * Description:  allocate memory for CDC descriptors, and fill them out.
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @init_info: (IN) information required to fill out CDC descriptors
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t cdc_alloc_descriptors_mult(cdc_ctx_t *ctx, 
    cdc_desc_init_t *init_info)
{
    pipe_desc_t         *intr_pipe = NULL;
    pipe_desc_t         *data_pipes = NULL;
    alt_interface_desc_t *cc_alt = NULL;
    alt_interface_desc_t *dc_alts = NULL;
    interface_desc_t    *ifaces = NULL;
    struct usb_interface_assoc_descriptor *iad = NULL;
    fd_desc_t           *desc = NULL;
    void                *ex_desc = NULL;
    void                *curr = NULL;
    juint32_t            size;
    juint32_t            ex_desc_size = 0;
    juint8_t             dc_num_alts = init_info->dc_empty_alt_0 ? 2 : 1;
    juint8_t interface_number, next_interface;

    if (init_info->interface_number == UWD_UNDEFINED_INTERFACE)
    {
        interface_number = core_get_available_iface(ctx->core_ctx);
        next_interface = UWD_UNDEFINED_INTERFACE;
    }
    else
    {
        interface_number = init_info->interface_number;
        next_interface = init_info->interface_number + 1;
    }

    size = sizeof(fd_desc_t) +          /* desc */
        sizeof(alt_interface_desc_t) +  /* cc_alt */
        sizeof(interface_desc_t);       /* cc_iface (ifaces[0]) */

    if (init_info->get_func_headers)
    {
        ex_desc_size = init_info->get_func_headers(ctx, NULL, FALSE, 
            interface_number);
        size += ex_desc_size;
    }

    if (init_info->cc_ntf_supported)
        size += sizeof(pipe_desc_t);    /* intr_pipe */
    
    if (init_info->dc_supported)
    {
        size += sizeof(pipe_desc_t)*2 +                 /* data_pipes */
            sizeof(alt_interface_desc_t)*dc_num_alts +  /* dc_alts */
            sizeof(interface_desc_t) +          /* dc_iface (ifaces[1]) */
            sizeof(struct usb_interface_assoc_descriptor);
    }

    curr = jmalloc(size, M_ZERO);
    if (!curr)
        goto Error;

    /* Make room for FD descriptor */
    desc = (fd_desc_t*)curr;
    curr = desc+1;

    /* Setup IAD */
    if ((init_info->dc_supported) &&
      /* HSU addition */
        !(ctx->info.basic.flags & CDC_INIT_NO_IAD))
    {/* End of HSU addition*/
        iad = (struct usb_interface_assoc_descriptor*)curr;
        curr = iad+1;
        
        iad->bLength = sizeof(struct usb_interface_assoc_descriptor);
        iad->bDescriptorType = USB_DT_INTERFACE_ASSOCIATION;
        iad->bFirstInterface = interface_number;
        iad->bInterfaceCount = 2;
        iad->bFunctionClass = USB_CLASS_COMM;
        iad->bFunctionSubClass = 0;
        iad->bFunctionProtocol = 0;
        iad->iFunction = 0;
    }
    
    /* Setup control class alt setting and pipes */
    if (init_info->cc_ntf_supported)
    {
        /* Setup interrupt pipe for notifications */
        intr_pipe = (pipe_desc_t*)curr;
        curr = intr_pipe+1;
        intr_pipe->max_pkt_size_high            = 64;
        intr_pipe->max_pkt_size_full            = 64;
        intr_pipe->type                         = PIPE_INTR;
        intr_pipe->direction                    = DIRECTION_IN;
        intr_pipe->sync_type                    = 0;
        intr_pipe->usage_type                   = 0;
        intr_pipe->address                      = 0; /* Filled by Core */
        intr_pipe->transaction_per_microframe   = 0;
        intr_pipe->rate_u.poll_interval         = 5; /* 5ms in FS, 2ms in HS */

        cc_alt = (alt_interface_desc_t*)curr;
        curr = cc_alt+1;
        cc_alt->pipe_count = 1;
        cc_alt->pipes = intr_pipe;
    }
    else
    {
        /* Control endpoint only */
        cc_alt = (alt_interface_desc_t*)curr;
        curr = cc_alt+1;
        cc_alt->pipe_count = 0;
        cc_alt->pipes = NULL;
    }

    /* Setup data class alt setting and pipes */
    if (init_info->dc_supported)
    {
        data_pipes = (pipe_desc_t*)curr;
        curr = data_pipes+2;
        data_pipes[0].max_pkt_size_high = get_max_packet_size(SPEED_HIGH);
        data_pipes[0].max_pkt_size_full = get_max_packet_size(SPEED_FULL);
        data_pipes[0].type              = PIPE_BULK;
        data_pipes[0].direction         = DIRECTION_IN;
        data_pipes[0].sync_type         = 0;
        data_pipes[0].usage_type        = 0;
        data_pipes[0].address           = 0; /* Filled by Core */
        data_pipes[0].transaction_per_microframe = 0;
        data_pipes[0].rate_u.nak_rate   = 32; /* HS: up to 1 NAK per 4 ms */

        data_pipes[1].max_pkt_size_high = get_max_packet_size(SPEED_HIGH);
        data_pipes[1].max_pkt_size_full = get_max_packet_size(SPEED_FULL);
        data_pipes[1].type              = PIPE_BULK;
        data_pipes[1].direction         = DIRECTION_OUT;
        data_pipes[1].sync_type         = 0;
        data_pipes[1].usage_type        = 0;
        data_pipes[1].address           = 0; /* Filled by Core */
        data_pipes[1].transaction_per_microframe = 0;
        data_pipes[1].rate_u.nak_rate   = 32; /* HS: up to 1 NAK per 4 ms */

        dc_alts = (alt_interface_desc_t*)curr;

        curr = dc_alts + dc_num_alts;
        if (init_info->dc_empty_alt_0)
        {
            dc_alts[0].pipe_count = 0;  /* Empty alt setting */
            dc_alts[0].pipes = NULL;
            dc_alts[1].pipe_count = 2;  /* Data capabale alt setting */
            dc_alts[1].pipes = data_pipes;
        }
        else
        {
            dc_alts[0].pipe_count = 2;  /* Data capabale alt setting */
            dc_alts[0].pipes = data_pipes;
        }
    }

    /* Setup interface descriptors */
    ifaces = (interface_desc_t*)curr;
    curr = init_info->dc_supported ? ifaces+2 : ifaces+1;

    ifaces[0].control_msg               = init_info->cc_handler;
    ifaces[0].alt_ifs                   = cc_alt;
    ifaces[0].alt_if_num                = 1;
    ifaces[0].alt_if_curr               = 0;
    ifaces[0].extra_descriptor          = NULL;
    ifaces[0].extra_descriptor_size     = 0;
    ifaces[0].number                    = init_info->interface_number; 
    /* HSU addition: HSU_STR_IDX_NOT_SET */
    ifaces[0].if_string = core_add_string_descriptor(ctx->core_ctx,
      ctx->info.basic.comm_string, HSU_STR_IDX_NOT_SET);
    /* HSU addition end */

    if (ctx->info.basic.flags & CDC_INIT_VENDOR_SPECIFIC_CLASS)
        ifaces[0].if_class              = USB_CLASS_VENDOR_SPEC;
    else
        ifaces[0].if_class              = USB_CLASS_COMM;

    if (init_info->cci_subclass)
    {
        ifaces[0].if_subclass           = init_info->cci_subclass;
        ifaces[0].if_protocol           = init_info->cci_protocol;
    }
    else
    {
        ifaces[0].if_subclass           = USB_CLASS_VENDOR_SPEC;
        ifaces[0].if_protocol           = USB_CLASS_VENDOR_SPEC;        
    }

    if (init_info->dc_supported)
    {
        ifaces[1].control_msg           = init_info->cc_handler;
        ifaces[1].alt_ifs               = dc_alts;
        ifaces[1].alt_if_num            = dc_num_alts;
        ifaces[1].alt_if_curr           = 0;
        ifaces[1].extra_descriptor      = NULL;
        ifaces[1].extra_descriptor_size = 0;
        ifaces[1].number                = next_interface; 
        ifaces[1].if_class              = USB_CLASS_CDC_DATA;
        ifaces[1].if_subclass           = 0;
        ifaces[1].if_protocol           = 0;
        /* HSU addition: HSU_STR_IDX_NOT_SET */
        ifaces[1].if_string = core_add_string_descriptor(ctx->core_ctx,
          ctx->info.basic.data_string, HSU_STR_IDX_NOT_SET);
        /* HSU addition end */
    }

    /* Setup extra (class specific) descriptor */
    if (init_info->get_func_headers)
    {
        ex_desc = curr;
        curr = (char*)ex_desc + ex_desc_size;
        init_info->get_func_headers(ctx, ex_desc, FALSE, interface_number);
        ifaces[0].extra_descriptor = ex_desc;
        ifaces[0].extra_descriptor_size = ex_desc_size;
    }

    /* Setup FD descriptor */
    desc->speed = SPEED_HIGH;
    desc->interface_count = init_info->dc_supported ? 2 : 1;
    desc->interfaces = ifaces;
    desc->iad = init_info->dc_supported ? iad : NULL;
    ctx->desc = desc;

    return 0;

Error:
    if (desc)
        jfree(desc);
    
    return -1;
}

/**
 * Function name:  reset_encap_state
 * Description: Reset encapsulted command state machine
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @notify_app: (IN) calls response complete callback when true
 *     @status: (IN) io status for response complete callback
 *     
 * Scope: local
 **/
static void reset_encap_state(cdc_ctx_t *ctx, jbool_t notify_app,
    cdc_iostatus_t status)
{
    juint32_t bytes_sent;
    
    if (ctx->encap_response.state == CDC_ENCAP_READY)
        notify_app = 0;

    /* Cleanup and prepare for next response */
    ctx->encap_response.state = CDC_ENCAP_READY;
    ctx->encap_response.size = 0;
    bytes_sent = ctx->encap_response.bytes_sent;
    ctx->encap_response.bytes_sent = 0;

    if (ctx->encap_response.buffer)
    {
        jfree(ctx->encap_response.buffer);
        ctx->encap_response.buffer = NULL;
    }

    /* Notify application of operation status */
    if (notify_app && ctx->cbs.basic.encap_response_complete_h)
    {
        ctx->cbs.basic.encap_response_complete_h(status,
            bytes_sent, ctx->app_ctx);
    }
}

/**
 * Function name:  cdc_abort_all_pipes
 * Description: Abort all active CDC transfers on all pipes
 * Parameters: 
 *     @ctx: (IN) CDC context
 * 
 * Scope: local
 **/
static void cdc_abort_all_pipes(cdc_ctx_t *ctx)
{
    if (ctx->intr_pipe.desc)
        core_abort_pipe(ctx->core_ctx, ctx->intr_pipe.desc);

    if (ctx->in_pipe.desc)
        core_abort_pipe(ctx->core_ctx, ctx->in_pipe.desc);

    if (ctx->out_pipe.desc)
        core_abort_pipe(ctx->core_ctx, ctx->out_pipe.desc);
}

/**
 * Function name:  default_cdc_get_pipe_config
 * Description:  USB CORE callback providing CORE with CDC descriptors
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: USB descriptors structure
 * Scope: global
 **/
static fd_desc_t* default_cdc_get_pipe_config(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    DBG_X(DSLAVE_CDC, ("cdc_get_pipe_config: called (%p)\n", ctx->desc));
    
    return ctx->desc;
}

/**
 * Function name:  cdc_enable
 * Description:  performs required initializations when device is 'enabled' by
 *               the host
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_enable(cdc_ctx_t *ctx)
{
    jbool_t is_single_iface = FALSE;
    juint8_t data_iface_num = 1;
    juint8_t data_pipe_base = 0;
    interface_desc_t *iface = NULL;
    DBG_X(DSLAVE_CDC, ("cdc_enable: entered\n"));
   
    if (!ctx || !ctx->desc)
        return JEINVAL;

    if (ctx->info.basic.flags & CDC_INIT_SINGLE_INTERFACE)
    {
        is_single_iface = TRUE;
        data_iface_num = 0;
        data_pipe_base = 1;
    }

    if (ctx->desc->interface_count >= 1)
    {
        juint8_t curr_alt = ctx->desc->interfaces[0].alt_if_curr;
        iface = &ctx->desc->interfaces[0];
        DBG_X(DSLAVE_CDC, ("cdc_enable: iface: %d, alt_setting: %u\n", 
            iface->number, curr_alt));
        ctx->intr_pipe.if_num = iface->number;
        if (is_single_iface && iface->alt_ifs[curr_alt].pipes && 
            iface->alt_ifs[curr_alt].pipes[0].type != PIPE_INTR)
        {
            ctx->intr_pipe.desc = NULL;
            data_pipe_base = 0; /* No interrupt in single interface */
        }
        else 
        {
            ctx->intr_pipe.desc = &iface->alt_ifs[curr_alt].pipes[0];
        }
    }

    if (ctx->desc->interface_count >= 2 || is_single_iface)
    {
        juint8_t curr_alt = ctx->desc->interfaces[data_iface_num].alt_if_curr;
        iface  = &ctx->desc->interfaces[data_iface_num];
        DBG_X(DSLAVE_CDC, ("cdc_enable: iface: %d, alt_setting: %u\n", 
            iface->number, curr_alt));

        if (iface->alt_ifs[curr_alt].pipe_count >= 1)
        {
            ctx->in_pipe.if_num = iface->number;
            ctx->in_pipe.desc = &iface->alt_ifs[curr_alt].pipes[data_pipe_base];
            DBG_X(DSLAVE_CDC, ("cdc_enable: iface: %d, alt_setting: %u, "
                "in_pipe: %x (0x%p)\n", iface->number, curr_alt, 
                ctx->in_pipe.desc->address, ctx->in_pipe.desc));
        }

        if (iface->alt_ifs[curr_alt].pipe_count >= 2)
        {
            ctx->out_pipe.if_num = iface->number;
            ctx->out_pipe.desc =
                &iface->alt_ifs[curr_alt].pipes[data_pipe_base + 1];
            DBG_X(DSLAVE_CDC, ("cdc_enable: iface: %d, alt_setting: %u, "
                "out_pipe: %x (0x%p)\n", iface->number, curr_alt, 
                ctx->out_pipe.desc->address, ctx->out_pipe.desc));
        }
    }

    ctx->usb_max_packet_size = get_max_packet_size(ctx->desc->current_speed);
    return 0;
}

/**
 * Function name:  default_cdc_enable
 * Description:  default USB CORE callback for handling SetConfig & SetInterface
 *               'events' - i.e. when the device is enabled by the host
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: local
 **/
static jresult_t default_cdc_enable(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    jresult_t rc = 0;

    DBG_X(DSLAVE_CDC, ("default_cdc_enable: entered\n"));

    if (!ctx)
        return JEINVAL;

    rc = cdc_enable(ctx);
    if (rc)
        return rc;
    
    if (ctx->cbs.basic.enabled_h)
        rc = ctx->cbs.basic.enabled_h(ctx->app_ctx);

    return rc;
}

/**
 * Function name:  cdc_disable
 * Description:  performs required cleanup when device is 'disabled' by
 *               the host
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: local
 **/
void cdc_disable(cdc_ctx_t *ctx)
{
    DBG_X(DSLAVE_CDC, ("cdc_disable: entered\n"));

    reset_encap_state(ctx, 1, CDC_TRANSFER_CANCELLED);

    cdc_abort_all_pipes(ctx);

    ctx->intr_pipe.desc = NULL;    
    ctx->intr_pipe.if_num = 0;
    ctx->in_pipe.desc = NULL;
    ctx->in_pipe.if_num = 0;
    ctx->out_pipe.desc = NULL;
    ctx->out_pipe.if_num = 0;
}

/**
 * Function name:  default_cdc_disable
 * Description:  default USB CORE callback for handling SetConfig & SetInterface
 *               'events' - i.e. when the device is disabled by the host
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: None
 * Scope: local
 **/
static void default_cdc_disable(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    
    DBG_X(DSLAVE_CDC, ("default_cdc_disable: entered\n"));

    if (!ctx)
        return;

    if (ctx->cbs.basic.disabled_h)
        ctx->cbs.basic.disabled_h(ctx->app_ctx);

    cdc_disable(ctx);
}

/**
 * Function name:  default_cdc_suspend
 * Description: Handle SUSPEND signal from the host. Abort all active transfers,
 *              and reset encapsulated command state.
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: local
 **/
static jresult_t default_cdc_suspend(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;

    DBG_I(DSLAVE_CDC, ("default_cdc_suspend: Received SUSPEND from host\n"));

    reset_encap_state(ctx, 1, CDC_TRANSFER_CANCELLED);
    cdc_abort_all_pipes(ctx);

    return 0;
}

/* Temp fix for B46104: added resume */

/**
* Function name:  default_cdc_resume
* Description: Handle RESUME signal from the host. Invoke app callback
* Parameters: 
*     @context: (IN) CDC context
*
* Return value: 0 for success, or an error code 
* Scope: local
**/
static jresult_t default_cdc_resume(context_t context)
{
  cdc_ctx_t *ctx = (cdc_ctx_t*)context;
  jresult_t rc = 0;

  DBG_I(DSLAVE_CDC, ("default_cdc_resume: Received RESUME from host\n"));

  if (ctx->cbs.basic.resumed_h)
    rc = ctx->cbs.basic.resumed_h(ctx->app_ctx);

  return rc;
}

/* end temp fix */


/**
 * Function name:  cdc_uninit
 * Description:  USB CORE callback for notifying CDC of stack removal. This
 *               function performs required cleanup before removal.
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **/
jresult_t cdc_uninit(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    
    DBG_X(DSLAVE_CDC, ("cdc_uninit: entered\n"));

    if (!ctx)
        return 0;

    /* Call application's uninit callback */
    if (ctx && ctx->cbs.basic.uninit_h)
        ctx->cbs.basic.uninit_h(ctx->app_ctx);

    /* Free preallocated requests */
    if (ctx->ctrl_read)
        core_request_free(ctx->ctrl_read);
    
    if (ctx->ctrl_write)
        core_request_free(ctx->ctrl_write);
    
    /* Free encapsulated reponse preallocated buffer */
    if (ctx->encap_response.buffer)
        jfree(ctx->encap_response.buffer);

    /* Free descriptors and context */
    cdc_free_descriptors(ctx);
    jfree(ctx);

    return 0;
}

/**
 * Function name:  cdc_init
 * Description:  this function is called by the CDC model's init function to
 *               register the CDC FD with the CORE.
 *               this function assigns the default handlers above, unless other
 *               handlers have been filled in the the model's code
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code  
 * Scope: global
 **/
jresult_t cdc_init(cdc_ctx_t *ctx)
{
    jresult_t rc = -1;

    DBG_X(DSLAVE_CDC, ("cdc_init: entered\n"));

    if (!ctx->ops.get_pipe_config)
        ctx->ops.get_pipe_config = default_cdc_get_pipe_config;
    if (!ctx->ops.enable)
        ctx->ops.enable = default_cdc_enable;
    if (!ctx->ops.disable)
        ctx->ops.disable = default_cdc_disable;
    if (!ctx->ops.suspend)
        ctx->ops.suspend = default_cdc_suspend;
/* Temp fix for B46104: added resume */
    if (!ctx->ops.resume)
      ctx->ops.resume = default_cdc_resume;
/* end temp fix */
    if (!ctx->ops.uninit)
        ctx->ops.uninit = cdc_uninit;

    DBG_X(DSLAVE_CDC, ("cdc_init: calling register_fd.\n"));
    rc = core_register_fd(ctx->core_ctx, &ctx->ops, ctx);
    
    return rc;
}


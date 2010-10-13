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

#include "sicd_fd.h"
#include "uw_device_sicd.h"

#define MAX_EP0_BUFFER_SIZE     512
#define MAX_INTR_BUFFER_SIZE    (sizeof(sicd_container_t))
#define MAX_OP_BUFFER_SIZE      (sizeof(sicd_container_t))
#define MAX_RESP_BUFFER_SIZE    (sizeof(sicd_container_t))
    
#define DEV_CLASS       6 
#define DEV_SUBCLASS    1
#define DEV_PROTOCOL    1 

#define SICD_CTRL_CANCEL_TRANSACTION    0x64
#define SICD_CTRL_GET_EX_EVENT_DATA     0x65
#define SICD_CTRL_DEVICE_RESET          0x66
#define SICD_CTRL_GET_DEV_STATUS        0x67
        
typedef struct
{
    void                *core_ctx;
    fd_desc_t           *fd_desc;
    pipe_desc_t         *in_pipe,
                        *out_pipe,
                        *intr_pipe;
    request_t           *ep0_req;
    
    ptp_instanceh_t     ptpinst;
    ptp_ctx_t           ptp_ctx;
    juint32_t            session_id;

    ptp_io_block_t      ptp_response;
    ptp_io_block_t      ptp_operation;

    juint32_t            usb_max_packet_size;

    jbool_t              is_started;
    jbool_t              is_enabled;
    
    enum
    {
        STATE_READY,
        STATE_OP,
        STATE_DATA_OUT,
        STATE_DATA_IN,
        STATE_DATA_IN_FIRST,
        STATE_RESP_BULK,
        STATE_RESP_EP0
    } state;

} sicd_ctx_t;

#define USB_PACKET_SIZE_HIGH    512
#define USB_PACKET_SIZE_FULL    64

/* Function prototypes */
static jresult_t control_msg(void *context, void *buffer);
static jresult_t handle_read_operation(ptp_transport_ctx_t tr_ctx,
    ptp_ctx_t ptpctx, juint32_t session_id);

/*==============================*/
/*     Utility Functions        */
/*==============================*/

static request_t* alloc_request(juint32_t size)
{
    request_t *req = NULL;
    DECLARE_FNAME("alloc_request");

    DBG_X(DSLAVE_SICD, ("%s: entered (%u)\n", fname, size));
    
    req = core_request_alloc(size, NULL);
    if (!req)
        DBG_E(DSLAVE_SICD, ("%s: failed to allocate request\n", fname));
    
    return req;
}

/*==============================*/
/*     Descriptors              */
/*==============================*/

/* Structs used to pass desired pipe configuration to core */
static pipe_desc_t pipe_desc_array[] = {
    {
        /* max_pkt_size_high            */ USB_PACKET_SIZE_HIGH,
        /* max_pkt_size_full            */ USB_PACKET_SIZE_FULL,
        /* type                         */ PIPE_BULK,
        /* direction                    */ DIRECTION_OUT,
        /* sync_type                    */ 0,
        /* usage_type                   */ 0,
        /* address                      */ 0, /* Filled by Core */
        /* transaction_per_microframe   */ 0,
        /* nak_rate                     */ { 8 } /* HS: at most 1 NAK per ms */
    }, 
    {
        /* max_pkt_size_high            */ USB_PACKET_SIZE_HIGH,
        /* max_pkt_size_full            */ USB_PACKET_SIZE_FULL,
        /* type                         */ PIPE_BULK,
        /* direction                    */ DIRECTION_IN,
        /* sync_type                    */ 0,
        /* usage_type                   */ 0,
        /* address                      */ 0, /* Filled by Core */
        /* transaction_per_microframe   */ 0,
        /* nak_rate                     */ { 8 } /* HS: at most 1 NAK per ms */
    },
    {
        /* max_pkt_size_high            */ 64,
        /* max_pkt_size_full            */ 64,
        /* type                         */ PIPE_INTR,
        /* direction                    */ DIRECTION_IN,
        /* sync_type                    */ 0,
        /* usage_type                   */ 0,
        /* address                      */ 0, /* Filled by Core */
        /* transaction_per_microframe   */ 0,
        /* interval                     */ { 5 } /* 5ms in FS, 2ms in HS */ 
    }
};

static alt_interface_desc_t alt_desc_array[] = {
    {
        /* pipe_count                   */ 3,
        /* pipes                        */ pipe_desc_array,
    }
};

static interface_desc_t if_desc_array[] = {
    {
        /* control_msg                  */ control_msg, 
        /* alt_ifs                      */ alt_desc_array,
        /* alt_if_num                   */ 1,
        /* alt_if_curr                  */ 0,
        /* extra_descriptor_size        */ 0,
        /* extra_descriptor             */ 0,
        /* number                       */ 0, /* Filled by Core */
        /* if_class                     */ DEV_CLASS, 
        /* if_subclass                  */ DEV_SUBCLASS,
        /* if_protocol                  */ DEV_PROTOCOL,
        /* if_string                    */ 0
    }
};

static fd_desc_t fd_desc = {
    /* speed            */ SPEED_HIGH, 
    /* current_speed    */ SPEED_UNKNOWN,
    /* interface_count  */ 1,
    /* interfaces       */ if_desc_array,
    NULL
};

/*=========================================*/
/*     Control Request handling            */
/*=========================================*/

static void ep0_complete(request_t *req)
{
    DECLARE_FNAME("ep0_complete");
    DBG_X(DSLAVE_SICD, ("%s: entered (%d)\n", fname, req->status));
    req->status = REQUEST_READY;
}

static void cancel_request_complete(request_t *req)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)req->context;
    ptp_io_block_t event;
    struct ep0_canceltrans_data *cancel_data;
    DECLARE_FNAME("cancel_request_complete");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    /* Check Request status */
    if (req->status != REQUEST_COMPLETED ||
        req->bytes_transferred < sizeof(struct ep0_canceltrans_data))
    {
        DBG_E(DSLAVE_SICD, ("%s: request failed (%d,%d)\n", fname,
            req->status, req->bytes_transferred));
        req->status = REQUEST_READY;
        return;
    }

    req->status = REQUEST_READY;

    cancel_data = (struct ep0_canceltrans_data*)req->buffer.vaddr;
    if (ua_le16toh(&cancel_data->code) != PTP_EVENT_CANCEL_TRANSACTION)
    {
        DBG_E(DSLAVE_SICD, ("%s: invalid event code (%d)\n", fname,
            ua_le16toh(&cancel_data->code)));
        return;
    }

    /* Simulate an incoming PTP "cancel transaction" operation/event */
    
    ua_htole16(&event.code, PTP_EVENT_CANCEL_TRANSACTION);
    ua_htole32(&event.session_id, ctx->session_id);
    ua_set32(&event.transaction_id, ua_get32(&cancel_data->transaction_id));

    ptp_io_initiator_event(ctx->ptpinst, &event);
}

static jresult_t handle_cancel_request(sicd_ctx_t *ctx,
    struct usb_ctrlrequest *setup)
{
    jresult_t rc = 0;
    request_t *req = ctx->ep0_req;
    DECLARE_FNAME("handle_cancel_request");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    if (ua_le16toh(&setup->wLength) != sizeof(struct ep0_canceltrans_data))
    {
        DBG_E(DSLAVE_LOOP, ("%s: invalid wLength (%d)\n", fname,
            ua_le16toh(&setup->wLength)));
        return JEIO;
    }
    
    req->complete       = cancel_request_complete;
    req->transfer_size  = sizeof(struct ep0_canceltrans_data);
    req->context        = ctx;

    rc = core_read_data(ctx->core_ctx, NULL, req);
    if (rc)
        DBG_E(DSLAVE_LOOP, ("%s: Error reading ep0.\n", fname));

    return rc;
}

static jresult_t handle_ex_event_request(sicd_ctx_t *ctx,
    struct usb_ctrlrequest *setup)
{
    jresult_t rc = 0;
    request_t *req = ctx->ep0_req;
    DECLARE_FNAME("handle_ex_event_request");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    /* TODO: send whatever it is that this abomination needs */

    req->complete       = ep0_complete;
    req->transfer_size  = 0;

    rc  = core_send_data(ctx->core_ctx, NULL, req);
    if (rc)
        DBG_E(DSLAVE_LOOP, ("FD: Error writing to ep0.\n"));
    
    return rc;
}

static jresult_t handle_dev_reset_request(sicd_ctx_t *ctx,
    struct usb_ctrlrequest *setup)
{
    jresult_t rc;
    request_t *req = ctx->ep0_req;
    ptp_io_block_t op;
    DECLARE_FNAME("handle_dev_reset_request");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    ua_htole16(&op.code, PTP_OP_RESET_DEVICE);
    ua_htole32(&op.session_id, ctx->session_id);
    ua_htole32(&op.transaction_id, 0xFFFFFFFF);

    rc = ptp_io_read_operation_complete(ctx->ptpinst, &op);

    /* Send ep0 status (zlp) */
    req->complete       = ep0_complete;
    req->transfer_size  = 0;

    rc  = core_send_data(ctx->core_ctx, NULL, req);
    if (rc)
        DBG_E(DSLAVE_LOOP, ("FD: Error writing to ep0.\n"));
    
    return rc;
}

static jresult_t handle_dev_status_request(sicd_ctx_t *ctx,
    struct usb_ctrlrequest *setup)
{
    jresult_t rc;
    request_t *req = ctx->ep0_req;
    struct ep0_devstat_data devstat;
    juint32_t length = 4;
    DECLARE_FNAME("handle_dev_status_request");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));
        
    /* This means that we are being polled after our bulk endpoints have been
     * stalled. Add the endpoint numbers on our reply */
    if (ctx->state == STATE_RESP_EP0 && ctx->in_pipe && ctx->out_pipe)
    {
        length += 2;
        devstat.ep_num[0] = ctx->in_pipe->address;
        devstat.ep_num[1] = ctx->out_pipe->address;
    }

    if (ua_le16toh(&setup->wLength) < length)
    {
        DBG_E(DSLAVE_SICD, ("%s: packet too short (%d<%d)\n", fname,
            ua_le16toh(&setup->wLength), length));
        return JEIO;
    }

    ua_htole16(&devstat.length, length);
    ua_set16(&devstat.code, ctx->ptp_response.code);

    /* Send device status */
    req->complete       = ep0_complete;
    req->transfer_size  = length;
    j_memcpy(req->buffer.vaddr, (void *)&devstat, length);

    rc  = core_send_data(ctx->core_ctx, NULL, req);
    if (rc)
        DBG_E(DSLAVE_LOOP, ("FD: Error writing to ep0.\n"));

    ctx->state = STATE_READY;
    return rc;
}

/* verify if handling of the MS OS feature here is the correct place */
#ifdef CONFIG_PROTO_PTP_MTP

#define MSOS_EXT_CONFIG_DESC_BCD_VERSION  0x0100
#define MSOS_FEATURE_IDX_EXT_CONFIG_DESC  0x0004
#define MSOS_FEATURE_IDX_EXT_PROP_DESC    0x0005

typedef __PACKED_PRE__ struct 
{
    juint32_t  dwLength;
    juint16_t  bcdVersion;
    juint16_t  wIndex;
    juint8_t   bCount;
    juint8_t   bReserved[7];
} __PACKED_POST__ msos_ext_cfg_desc_hdr_t;

typedef __PACKED_PRE__ struct 
{
    juint8_t bFirstInterfaceNumber;
    juint8_t bInterfaceCount;
    juint8_t compatibleID[8];
    juint8_t subCompatibleID[8];
    juint8_t bReserved[6];
} __PACKED_POST__ msos_ext_cfg_desc_func_t;

static msos_ext_cfg_desc_func_t msos_func =
{
    0x00,       /* starting at inteface  TODO: need to verify */
    0x01,       /* number of interfaces  TODO: need to verify */
    {"MTP"},    /* compatible ID */
    {0},        /* sub compatible ID */
    {0}
};

typedef __PACKED_PRE__ struct 
{
    msos_ext_cfg_desc_hdr_t      header;
    msos_ext_cfg_desc_func_t     function;
} __PACKED_POST__ msos_ext_cfg_desc_t;

typedef __PACKED_PRE__ struct 
{
    juint32_t  dwLength;
    juint16_t  bcdVersion;
    juint16_t  wIndex;
    juint16_t  wCount;
} __PACKED_POST__ msos_ext_prop_desc_hdr_t;

#endif

/* EP0 dispatcher */
static jresult_t control_msg(void *context, void *buffer)
{
    struct usb_ctrlrequest *setup = (struct usb_ctrlrequest*)buffer;
    sicd_ctx_t *ctx = (sicd_ctx_t*)context;
    jresult_t rc = 0;
    DECLARE_FNAME("control_msg");

    DBG_V(DSLAVE_SICD, ("%s: called with request %u\n", fname,
        setup->bRequest));

    /* Make sure our ep0 request can be used */
    if (ctx->ep0_req->status == REQUEST_INPROGRESS)
    {
        DBG_E(DSLAVE_SICD, ("%s: ep0_req is busy! can not handle "
            "request %d\n", fname, setup->bRequest));
        return JEBUSY;
    }

    ctx->ep0_req->status = REQUEST_READY;

#ifdef CONFIG_PROTO_PTP_MTP
    if ((setup->bRequestType & USB_TYPE_MASK) == USB_TYPE_VENDOR)
    {
        msos_ext_cfg_desc_t msos_cfg_desc;
        juint32_t msos_cfg_desc_length = 
            sizeof(msos_ext_cfg_desc_hdr_t)+sizeof(msos_ext_cfg_desc_func_t);

        DBG_I(DSLAVE_LOOP, ("%s: hadling the MS OS Feature Descriptor\n"));

        j_memset((void *)&msos_cfg_desc.header, 0, 
            sizeof(msos_ext_cfg_desc_hdr_t));
        ua_htole32(&msos_cfg_desc.header.dwLength, msos_cfg_desc_length);
        ua_htole16(&msos_cfg_desc.header.bcdVersion,
            MSOS_EXT_CONFIG_DESC_BCD_VERSION);
        ua_htole16(&msos_cfg_desc.header.wIndex,
            MSOS_FEATURE_IDX_EXT_CONFIG_DESC);
        msos_cfg_desc.header.bCount = 1;

        j_memcpy((void *)&msos_cfg_desc.function, (void *)&msos_func,
            sizeof(msos_ext_cfg_desc_func_t));
        
        j_memcpy(ctx->ep0_req->buffer.vaddr, (void *)&msos_cfg_desc,
            msos_cfg_desc_length);
        ctx->ep0_req->transfer_size = ua_le16toh(&setup->wLength);

        ctx->ep0_req->complete = ep0_complete;
        rc = core_send_data(ctx->core_ctx, NULL, ctx->ep0_req);

        return rc;
    }
#endif

    switch(setup->bRequest)
    {
    case SICD_CTRL_CANCEL_TRANSACTION:
        rc = handle_cancel_request(ctx, setup);
        break;
    
    case SICD_CTRL_GET_EX_EVENT_DATA:
        rc = handle_ex_event_request(ctx, setup);
        break;

    case SICD_CTRL_DEVICE_RESET:
        rc = handle_dev_reset_request(ctx, setup);
        break;

    case SICD_CTRL_GET_DEV_STATUS:
        rc = handle_dev_status_request(ctx, setup);
        break;
        
    default:
        rc = JENOTSUP;
        break;
    }
    
    return rc;
}

/*========================
 *     Bulk operations
 *=======================*/

#if 0
static jresult_t sicd_abort_write(sicd_handle_t handle)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)handle;

    if (!ctx->in_pipe)
        return JENXIO;

    core_abort_pipe(ctx->core_ctx, ctx->in_pipe);
    return 0;
}

static jresult_t sicd_abort_read(sicd_handle_t handle)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)handle;

    if (!ctx->out_pipe.desc)
        return JENXIO;

    core_abort_pipe(ctx->core_ctx, ctx->out_pipe.desc);
    return 0;
}
#endif
    
static void read_operation_complete(request_t *req)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)req->context;
    sicd_container_t *cont = (sicd_container_t*)req->buffer.vaddr;
    juint32_t length;
    DECLARE_FNAME("read_operation_complete");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    /* Exit if request was cancelled */
    if (req->status == REQUEST_CANCELLED)
    {
        DBG_E(DSLAVE_SICD, ("%s: request cancelled\n", fname));
        core_request_free(req);
        return;
    }

    /* Check Request status */
    if (req->status != REQUEST_COMPLETED || 
        req->bytes_transferred < sizeof(sicd_container_common_t))
    {
        DBG_E(DSLAVE_SICD, ("%s: request failed (%d) (%d,%d)\n", fname,
            req->status, req->bytes_transferred,
            sizeof(sicd_container_common_t)));
        goto Exit;
    }

    if (ctx->state != STATE_OP)
    {
        DBG_E(DSLAVE_SICD, ("%s: invalid state (%d)\n", fname, ctx->state));
        goto Exit;
    }
    
    length = ua_le32toh(&cont->common.length);
    if (length != req->bytes_transferred)
    {
        DBG_E(DSLAVE_SICD, ("%s: invalid container length (%d, %d)\n", fname,
            length, req->bytes_transferred));

        /* TODO: add status reporting to ptp callbacks */

        goto Exit;
    }
    
    j_memset((void *)&ctx->ptp_operation, 0, sizeof(ctx->ptp_operation));
    ua_set16(&ctx->ptp_operation.code, ua_get16(&cont->common.code));
    ua_set32(&ctx->ptp_operation.transaction_id,
        ua_get32(&cont->common.transaction_id));

    if (ua_le16toh(&ctx->ptp_operation.code) == PTP_OP_GET_DEVICE_INFO)
            ctx->ptp_operation.session_id = 0;
    else if (ua_le16toh(&ctx->ptp_operation.code) == PTP_OP_OPEN_SESSION)
        ctx->session_id = ua_le32toh(&cont->parameter[0]);
    else
        ua_htole32(&ctx->ptp_operation.session_id, ctx->session_id);

    /* Copy parameters if any */
    length -= sizeof(cont->common);
    j_memcpy((void *)ctx->ptp_operation.parameter, (void *)cont->parameter,
        MIN(length, sizeof(cont->parameter)));

Exit:
    ptp_io_read_operation_complete(ctx->ptpinst, &ctx->ptp_operation);
    core_request_free(req);
}

static jresult_t handle_read_operation(ptp_transport_ctx_t tr_ctx, 
    ptp_ctx_t ptpctx, juint32_t session_id)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)tr_ctx;
    request_t *op_req = NULL;
    jresult_t rc = 0;
    DECLARE_FNAME("handle_read_operation");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));
    
    if (ctx->state != STATE_READY)
    {
        DBG_E(DSLAVE_SICD, ("%s: device busy (state=%d)\n", fname,
            ctx->state));
        rc = JEBUSY;
        goto Exit;
    }

    op_req = alloc_request(MAX_OP_BUFFER_SIZE);
    if (!op_req)
    {
        DBG_E(DSLAVE_SICD, ("%s: failed to allocate request\n", fname));
        rc = JENOMEM;
        goto Exit;
    }

    op_req->complete = read_operation_complete;
    op_req->context = ctx;
    op_req->transfer_size = MAX_OP_BUFFER_SIZE;

    rc = core_read_data(ctx->core_ctx, ctx->out_pipe, op_req);
    if (rc)
        goto Exit;

    ctx->state = STATE_OP;

Exit:
    if (rc && op_req)
        core_request_free(op_req);

    return rc;    
}

static void read_data_complete(request_t *req)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)req->context;
    jbool_t is_short_packet = 0;
    juint32_t total = 0;
    juint32_t data_offset = 0;
    DECLARE_FNAME("read_data_complete");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));
    
    /* Exit if request was cancelled */
    if (req->status == REQUEST_CANCELLED)
    {
        DBG_E(DSLAVE_SICD, ("%s: request cancelled\n", fname));
        core_request_free(req);
        return;
    }

    /* Check Request status */
    if (req->status != REQUEST_COMPLETED)
    {
        DBG_E(DSLAVE_SICD, ("%s: request failed (%d)\n", fname, req->status));
        goto Exit;
    }

    is_short_packet = (req->bytes_transferred < req->transfer_size) ||
        (req->transfer_size % ctx->usb_max_packet_size);

    if (ctx->state == STATE_OP)
    {
        sicd_container_common_t *bulk_cont =
            (sicd_container_common_t*)req->buffer.vaddr;

        /* Make sure we have enough data */
        if (req->bytes_transferred <= sizeof(sicd_container_common_t))
        {
            DBG_E(DSLAVE_SICD, ("%s: request failed (%d<=%d)\n", fname,
                req->bytes_transferred, sizeof(sicd_container_common_t)));
            goto Exit;
        }

        ctx->state = STATE_DATA_OUT;
        total = ua_le32toh(&bulk_cont->length) - 
            sizeof(sicd_container_common_t);

        if (ua_le16toh(&bulk_cont->type) != SICD_CONT_DATA)
        {
            DBG_E(DSLAVE_SICD, ("%s: invalid container type: %d\n", fname,
                ua_le16toh(&bulk_cont->type)));
            goto Exit;
        }
        
        if (ua_get16(&bulk_cont->code) != ua_get16(&ctx->ptp_operation.code))
        {
            DBG_E(DSLAVE_SICD, ("%s: invalid operation code: %X (%X)\n",
                fname, ua_le16toh(&bulk_cont->code),
                ua_le16toh(&ctx->ptp_operation.code)));
            goto Exit;
        }
       
        if (ua_get32(&bulk_cont->transaction_id) !=
            ua_get32(&ctx->ptp_operation.transaction_id))
        {
            DBG_E(DSLAVE_SICD, ("%s: invalid transaction id: %X (%X)\n", fname,
                ua_le32toh(&bulk_cont->transaction_id),
                ua_le32toh(&ctx->ptp_operation.transaction_id)));
            goto Exit;
        }

        /* Our data resides past the bulk container */
        data_offset = sizeof(sicd_container_common_t);
    }

    /* Copy received data to target buffer (this copy should be removed when
     * using PTP dmable buffers) */
    j_memcpy(req->caller_private, req->buffer.vaddr, req->bytes_transferred);

Exit:
    ptp_io_read_data_complete(ctx->ptp_ctx, req->bytes_transferred-data_offset,
        total, data_offset, is_short_packet);
    core_request_free(req);
}

static jresult_t handle_read_data(ptp_transport_ctx_t tr_ctx, ptp_ctx_t ptpctx,
    void *data, juint32_t size)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)tr_ctx;
    request_t *dataout_req = NULL;
    jresult_t rc = 0;
    DECLARE_FNAME("handle_read_data");

    DBG_X(DSLAVE_SICD, ("%s: entered, size=%u\n", fname, size));
    
    if (ctx->state != STATE_OP && ctx->state != STATE_DATA_OUT)
    {
        DBG_E(DSLAVE_SICD, ("%s: invalid state (%d)\n", fname, ctx->state));
        rc = -1;
        goto Exit;
    }

    dataout_req = alloc_request(PTP_MAX_BLOCK_SIZE);
    if (!dataout_req)
    {
        DBG_E(DSLAVE_SICD, ("%s: failed to allocate request\n", fname));
        rc = JENOMEM;
        goto Exit;
    }

    dataout_req->complete = read_data_complete;
    dataout_req->context = ctx;
    ctx->ptp_ctx = ptpctx;
    dataout_req->caller_private = data;
    dataout_req->transfer_size = size;

    rc = core_read_data(ctx->core_ctx, ctx->out_pipe, dataout_req);
    if (rc)
        goto Exit;

Exit:
    if (rc && dataout_req)
        core_request_free(dataout_req);

    return rc;    
}

static void send_data_complete(request_t *req)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)req->context;
    juint32_t size;
    DECLARE_FNAME("send_data_complete");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    /* Exit if request was cancelled */
    if (req->status == REQUEST_CANCELLED)
    {
        DBG_E(DSLAVE_SICD, ("%s: request cancelled\n", fname));
        core_request_free(req);
        return;
    }

    /* Check Request status */
    if (req->status  != REQUEST_COMPLETED ||
        req->bytes_transferred < req->transfer_size)
    {
        DBG_E(DSLAVE_SICD, ("%s: request failed (%d) (%d/%d)\n", fname,
            req->status, req->bytes_transferred, req->transfer_size));
    }

    if (ctx->state == STATE_DATA_IN_FIRST)
    {
        size = req->bytes_transferred - sizeof(sicd_container_common_t);
        ctx->state = STATE_DATA_IN;
    }
    else
        size = req->bytes_transferred;
    
    ptp_io_send_data_complete(ctx->ptp_ctx, size);
    core_request_free(req);
}

static jresult_t handle_send_data(ptp_transport_ctx_t tr_ctx, ptp_ctx_t ptpctx,
    void *data, juint32_t size, juint32_t total, jbool_t last)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)tr_ctx;
    request_t *datain_req = NULL;
    jresult_t rc = 0;
    DECLARE_FNAME("handle_send_data");

    DBG_X(DSLAVE_SICD, ("%s: entered. (%p, %d, %d, %d)\n", fname, data, size,
        total, last));

    if (ctx->state != STATE_OP && ctx->state != STATE_DATA_IN)
    {
        DBG_E(DSLAVE_SICD, ("%s: invalid state (%d)\n", fname, ctx->state));
        rc = -1;
        goto Exit;
    }

    datain_req = alloc_request(PTP_MAX_BLOCK_SIZE);
    if (!datain_req)
    {
        DBG_E(DSLAVE_SICD, ("%s: failed to allocate request\n", fname));
        rc = JENOMEM;
        goto Exit;
    }

    if (ctx->state == STATE_OP)         /* First Data sent */
    {
        sicd_container_common_t *bulk_cont = (sicd_container_common_t*)data;
        ctx->state = STATE_DATA_IN_FIRST;

        /* Add the container size to the PTP payload size */
        size += sizeof(sicd_container_common_t);
        total += sizeof(sicd_container_common_t);
    
        ua_htole32(&bulk_cont->length, total);
        ua_htole16(&bulk_cont->type, SICD_CONT_DATA);
        ua_set16(&bulk_cont->code, ctx->ptp_operation.code);
        ua_set32(&bulk_cont->transaction_id, ctx->ptp_operation.transaction_id);
    }

    /* This copy should be removed when using PTP dmable buffers) */
    j_memcpy(datain_req->buffer.vaddr, data, size);
    datain_req->complete = send_data_complete;
    datain_req->context = ctx;
    ctx->ptp_ctx = ptpctx;
    datain_req->transfer_size = size;

    if (last)
        datain_req->zero = 1;
        
    rc = core_send_data(ctx->core_ctx, ctx->in_pipe, datain_req);
    if (rc)
        goto Exit;

Exit:
    if (rc && datain_req)
        core_request_free(datain_req);

    return rc;
}

static void send_response_complete(request_t *req)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)req->context;
    DECLARE_FNAME("send_response_complete");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    /* Exit if request was cancelled */
    if (req->status == REQUEST_CANCELLED)
    {
        DBG_E(DSLAVE_SICD, ("%s: request cancelled\n", fname));
        core_request_free(req);
        return;
    }
    
    /* Check Request status */
    if (req->status != REQUEST_COMPLETED ||
        req->bytes_transferred < req->transfer_size)
    {
        DBG_E(DSLAVE_SICD, ("%s: request failed (%d) (%d/%d)\n", fname,
            req->status, req->bytes_transferred, req->transfer_size));
    }

    ctx->state = STATE_READY;
    ptp_io_send_response_complete(ctx->ptp_ctx);
    core_request_free(req);
}

static jresult_t handle_send_response(ptp_transport_ctx_t tr_ctx,
    ptp_ctx_t ptpctx, ptp_io_block_t *response, juint32_t num_params,
    jbool_t data_phase_present)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)tr_ctx;
    sicd_container_t *bulk_cont;
    request_t *resp_req = NULL;
    jresult_t rc = 0;
    DECLARE_FNAME("handle_send_response");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    if (ctx->state == STATE_RESP_BULK || ctx->state == STATE_RESP_EP0)
    {
        DBG_E(DSLAVE_SICD, ("%s: invalid state (%d)\n", fname, ctx->state));
        rc = -1;
        goto Exit;
    }

    if (data_phase_present)
    {
        DBG_V(DSLAVE_SICD, ("%s: stalling bulk in & out pipes.\n", fname));

        /* If a data phase is expected by the host, we cannot send the response
         * as is. We need to stall the bulk endpoints and send the response
         * over the control pipe, in a Device Status reply */
        j_memcpy((void *)&ctx->ptp_response, (void *)response, 
            sizeof(ctx->ptp_response));
        core_abort_pipe(ctx->core_ctx, ctx->out_pipe);
        core_abort_pipe(ctx->core_ctx, ctx->in_pipe);
        core_stall_pipe(ctx->core_ctx, ctx->out_pipe, 1);
        core_stall_pipe(ctx->core_ctx, ctx->in_pipe, 1);
        ctx->state = STATE_RESP_EP0;
        goto Exit;        
    }

    resp_req = alloc_request(MAX_RESP_BUFFER_SIZE);
    if (!resp_req)
    {
        DBG_E(DSLAVE_SICD, ("%s: failed to allocate request\n", fname));
        rc = JENOMEM;
        goto Exit;
    }

    resp_req->complete = send_response_complete;
    resp_req->context = ctx;
    ctx->ptp_ctx = ptpctx;
    resp_req->transfer_size =
        sizeof(sicd_container_common_t)+(sizeof(juint32_t)*num_params);
    resp_req->zero = 1;
    
    bulk_cont = (sicd_container_t*)resp_req->buffer.vaddr;
    ua_htole32(&bulk_cont->common.length, resp_req->transfer_size);
    ua_htole16(&bulk_cont->common.type, SICD_CONT_RESP);
    ua_set16(&bulk_cont->common.code, ua_get16(&response->code));
    j_memcpy((void *)(&bulk_cont->common.transaction_id), 
        (void *)&response->transaction_id, sizeof(juint32_t)*(num_params+1));

    rc = core_send_data(ctx->core_ctx, ctx->in_pipe, resp_req);
    if (rc)
        goto Exit;
    
    ctx->state = STATE_RESP_BULK;

Exit:
    if (rc && resp_req)
        core_request_free(resp_req);
    
    return rc;
}

static void send_event_complete(request_t *req)
{
    DECLARE_FNAME("send_event_complete");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    /* Exit if request was cancelled */
    if (req->status == REQUEST_CANCELLED)
    {
        DBG_E(DSLAVE_SICD, ("%s: request cancelled\n", fname));
        core_request_free(req);
        return;
    }
    
    /* Check Request status */
    if (req->status != REQUEST_COMPLETED ||
        req->bytes_transferred < req->transfer_size)
    {
        DBG_E(DSLAVE_SICD, ("%s: request failed (%d) (%d/%d)\n", fname,
            req->status, req->bytes_transferred, req->transfer_size));
    }
    
    core_request_free(req);
}

static jresult_t handle_send_event(ptp_transport_ctx_t tr_ctx, ptp_ctx_t ptpctx,
    ptp_io_block_t *event, juint32_t num_params)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)tr_ctx;
    sicd_container_t *bulk_cont;
    request_t *event_req = NULL;
    jresult_t rc = 0;
    DECLARE_FNAME("handle_send_event");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    event_req = alloc_request(MAX_INTR_BUFFER_SIZE);
    if (!event_req)
    {
        DBG_E(DSLAVE_SICD, ("%s: failed to allocate request\n", fname));
        rc = JENOMEM;
        goto Exit;
    }

    event_req->complete = send_event_complete;
    event_req->context = ctx;
    ctx->ptp_ctx = ptpctx;
    event_req->transfer_size =
        sizeof(sicd_container_common_t)+(sizeof(juint32_t)*num_params);
    event_req->zero = 1;

    bulk_cont = (sicd_container_t*)event_req->buffer.vaddr;
    ua_htole32(&bulk_cont->common.length, event_req->transfer_size);
    ua_htole16(&bulk_cont->common.type, SICD_CONT_EVT);
    ua_set16(&bulk_cont->common.code, ua_get16(&event->code));
    j_memcpy((void *)(&bulk_cont->common.transaction_id),
        (void *)&event->transaction_id, sizeof(juint32_t)*(num_params+1));

    rc = core_send_data(ctx->core_ctx, ctx->intr_pipe, event_req);

Exit:    
    if (rc && event_req)
        core_request_free(event_req);
    
    return rc;
}

/*=========================================*/
/*      CORE registration                  */
/*=========================================*/

static fd_desc_t *get_pipe_config(context_t context)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)context;
    DECLARE_FNAME("get_pipe_config");
    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));
    return ctx->fd_desc;
}

static void stop(sicd_ctx_t *ctx)
{
    DECLARE_FNAME("stop");
    
    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    if (!ctx->is_started)
        return;

    ptp_stop_responder(ctx->ptpinst, 1);

    /* Abort active transfers */
    if (ctx->out_pipe)
        core_abort_pipe(ctx->core_ctx, ctx->out_pipe);

    if (ctx->in_pipe)
        core_abort_pipe(ctx->core_ctx, ctx->in_pipe);

    if (ctx->intr_pipe)
        core_abort_pipe(ctx->core_ctx, ctx->intr_pipe);

    ctx->is_started = 0;
}

static jresult_t start(sicd_ctx_t *ctx)
{
    jresult_t rc = 0;
    DECLARE_FNAME("start");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    if (!ctx->is_enabled || ctx->is_started)
        return 0;
    
    ctx->state = STATE_READY;

    /* Start the PTP protocol engine */
    rc = ptp_start_responder(ctx->ptpinst);
    if (!rc)
        ctx->is_started = 1;
#ifdef JDEBUG
    else
        DBG_E(DSLAVE_SICD, ("%s: failed to start responder (%d)\n",
            fname, rc));
#endif

    return rc;
}

static jresult_t suspend(context_t context)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)context;
    DECLARE_FNAME("suspend");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));
    stop(ctx);
    return 0;
}

static void disable(context_t context)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)context;
    DECLARE_FNAME("disable");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    if (!ctx->is_enabled)
        return;

    stop(ctx);
    ctx->out_pipe = NULL;
    ctx->in_pipe = NULL;
    ctx->intr_pipe = NULL;
    ctx->is_enabled = 0;
}

static jresult_t resume(context_t context)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)context;
    DECLARE_FNAME("resume");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));
    return start(ctx);
}

static jresult_t enable(context_t context)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)context;
    jresult_t rc = 0;
    DECLARE_FNAME("enable");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    if (!ctx->is_enabled)
    {
        /* Get the pipe numbers that core assigned to our descriptors */
        ctx->out_pipe = &ctx->fd_desc->interfaces[0].alt_ifs[0].pipes[0];
        ctx->in_pipe = &ctx->fd_desc->interfaces[0].alt_ifs[0].pipes[1];
        ctx->intr_pipe = &ctx->fd_desc->interfaces[0].alt_ifs[0].pipes[2];

        ctx->usb_max_packet_size = (ctx->fd_desc->current_speed == SPEED_HIGH)
            ? USB_PACKET_SIZE_HIGH : USB_PACKET_SIZE_FULL;

        ctx->is_enabled = 1;
    }
    
    rc = start(ctx);
    if (rc)
        disable(context);

    return rc;
}

static jresult_t uninit(context_t context)
{
    sicd_ctx_t *ctx = (sicd_ctx_t*)context;
    DECLARE_FNAME("uninit");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    ptp_delete_instance(ctx->ptpinst, 0);
    
    /* Free requests */
    if (ctx->ep0_req)
    {
        core_request_free(ctx->ep0_req);
        ctx->ep0_req = NULL;
    }

    jfree(ctx);
    return 0;
}

static fd_ops_t fd_ops = {
/*  get_pipe_config     */ get_pipe_config,
/*  enable              */ enable,
/*  disable             */ disable,      
/*  suspend             */ suspend,
/*  resume              */ resume,
/*  uninit              */ uninit
};

jresult_t sicd_init(void *core_ctx)
{
    sicd_ctx_t *ctx;
    jresult_t rc = 0;
    ptp_transport_ops ops;
    ptp_init_args args;
    sicd_init_info_t info;
    DECLARE_FNAME("sicd_init");

    DBG_X(DSLAVE_SICD, ("%s: entered\n", fname));

    ctx = (sicd_ctx_t*)jmalloc(sizeof(sicd_ctx_t), M_ZERO);
    if (!ctx)
    {
        rc = JENOMEM;
        DBG_E(DSLAVE_SICD, ("%s: failed to allocate SICD context\n", fname));
        goto Error;
    }
    
    /* Initialize ops */
    ops.read_operation_h = handle_read_operation;
    ops.read_data_h = handle_read_data;
    ops.send_data_h = handle_send_data;
    ops.send_response_h = handle_send_response;
    ops.send_event_h = handle_send_event;

    /* Initialize args */
    args.transport_datahdr_size = sizeof(sicd_container_common_t);
    args.transport_block_size = PTP_MAX_BLOCK_SIZE;
    args.max_sessions = 1;
    
    /* Initialize sicd context */
    ctx->core_ctx = core_ctx;
    ctx->fd_desc = &fd_desc;
    ctx->session_id = 0;

    /* Allocate requests */
    if (!ctx->ep0_req)
        ctx->ep0_req = core_request_alloc(MAX_EP0_BUFFER_SIZE, NULL);
    if (!ctx->ep0_req)
    {
        rc = JENOMEM;
        DBG_E(DSLAVE_SICD, ("%s: failed to allocate ep0 request\n", fname));
        goto Error;
    }

    /* Create an instance of a PTP protocol parser */
    rc = ptp_create_instance(&ctx->ptpinst, &ops, ctx, &args);
    if (rc)
    {
        DBG_E(DSLAVE_SICD, ("%s: failed to create PTP instance (%d)\n",
            fname, rc));
        goto Error;
    }

    /* Init SICD application - Allow application to initialize
     * PTP/MTP/SICD etc */
    info.interface_number = UWD_UNDEFINED_INTERFACE;
    rc = sicd_app_init(ctx->ptpinst, &info);
    if (rc)
    {
        DBG_E(DSLAVE_SICD, ("%s: application failed to initialize (%d)\n",
            fname, rc));
        goto Error;
    }

    ctx->fd_desc->interfaces[0].number = info.interface_number;

#ifdef CONFIG_PROTO_PTP_MTP
#define MSOS_VENDOR_CODE                  0x01
    core_add_msos_string_descriptor(core_ctx, MSOS_VENDOR_CODE);
#endif

    /* Resigter FD */
    return core_register_fd(core_ctx, &fd_ops, (context_t)ctx);

Error:
    return rc;
}


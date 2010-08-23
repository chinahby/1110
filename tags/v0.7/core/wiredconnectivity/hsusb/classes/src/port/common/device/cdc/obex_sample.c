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

#include <uw_device_cdc_obex.h>
#include <jos.h>

typedef struct
{
    cdc_handle_t hcdc;

    /* My variables*/
#define RX_BUFF_SIZE 512
    juint8_t rx_buf[RX_BUFF_SIZE];
#define COMM_STR "JOBEX_COMMUNICATION_INTERFACE"
#define COMM_STR_SIZE sizeof(COMM_STR)
#define DATA_STR "JOBEX_DATA_INTERFACE"
#define DATA_STR_SIZE sizeof(DATA_STR)
    char comm_str[COMM_STR_SIZE];
    char data_str[DATA_STR_SIZE];
} samp_ctx_t;

/* OBEX request opcodes */
#define OBEX_REQ_CONNECT 0x80
/* ...Add additional opcodes here... */

/* OBEX response opcodes */
#define OBEX_RESP_OK 0xA0
/* ...Add additional opcodes here... */

__PACKED_PRE__
struct obex_hdr
{
    juint8_t code;
    juint16_t length;
} __PACKED_POST__;

__PACKED_PRE__
struct obex_msg_connect
{
    struct obex_hdr hdr;
    juint8_t version;
    juint8_t flags;
    juint16_t max_len;
} __PACKED_POST__;

/* ... Define additional OBEX messages here... */

__PACKED_PRE__
union obex_msg
{
    struct obex_hdr hdr;
    struct obex_msg_connect connect;
    /* ... Add additional OBEX messages defined above... */
    juint8_t raw[1];
} __PACKED_POST__;

static jresult_t handle_obex_request(samp_ctx_t *state, void *req_buf, juint32_t size)
{
    union obex_msg *msg = (union obex_msg*)req_buf;
    jresult_t rc = 0;

    if (size < sizeof(struct obex_msg_connect))
        return -1;
    
    switch(msg->hdr.code)
    {
    case OBEX_REQ_CONNECT:
        {
            union obex_msg resp;
            resp.hdr.code = OBEX_RESP_OK;
            ua_htobe16(&resp.hdr.length, sizeof(struct obex_msg_connect));
            resp.connect.version = 0x10;
            resp.connect.flags = 0;
            ua_htobe16(&resp.connect.max_len, RX_BUFF_SIZE);
            cdc_write(state->hcdc, (void *)&resp, 
                sizeof(struct obex_msg_connect));
            break;
        }
    default:
        rc = -1;
        break;
    }

    return rc;
}

static jresult_t samp_uninit(cdc_appctx_t ctx)
{
    samp_ctx_t *state = (samp_ctx_t*)ctx;

    /* Perform application specific cleanup */
    
    /* Free our context */
    if (state)
        jfree(state);

    return 0;
}

static void samp_read_done(cdc_iostatus_t status, void *buffer,
    juint32_t size, jbool_t transfer_complete, cdc_appctx_t ctx)
{
    samp_ctx_t *state = (samp_ctx_t*)ctx;
    DECLARE_FNAME("samp_read_done");

    DBG_I(DSLAVE_CDC_OBEX, ("%s: entered", fname));
    if ((status == CDC_TRANSFER_OK) && size)
        handle_obex_request(state, buffer, size);

    cdc_read(state->hcdc, state->rx_buf, sizeof(state->rx_buf)-1);
}

static void samp_write_done(cdc_iostatus_t status, void *buffer,
    cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_write_done");

    DBG_I(DSLAVE_CDC_OBEX, ("%s: entered\n", fname));
}

static jresult_t samp_enabled(cdc_appctx_t ctx)
{
    samp_ctx_t *state = (samp_ctx_t*)ctx;
    DECLARE_FNAME("samp_enabled");

    DBG_I(DSLAVE_CDC_OBEX, ("%s: entered\n", fname));
    cdc_read(state->hcdc, state->rx_buf, sizeof(state->rx_buf));
    return 0;
}

static jresult_t samp_disabled(cdc_appctx_t ctx)
{
    samp_ctx_t *state = (samp_ctx_t*)ctx;
    DECLARE_FNAME("samp_disabled");

    DBG_I(DSLAVE_CDC_OBEX, ("%s: entered\n", fname));
    cdc_abort_read(state->hcdc);
    cdc_abort_write(state->hcdc);
    return 0;
}

jresult_t obex_init(cdc_handle_t handle, obex_callbacks_t *cbs,
    obex_init_info_t *info, cdc_appctx_t *ctx)
{
    samp_ctx_t *state;
    
    /* Allocte our state (context) */
    state = jmalloc(sizeof(samp_ctx_t), 0);
    if (!state)
        goto Error;

    *ctx = (cdc_appctx_t*)state;

    /* Save CDC handle (used for calling CDC API's) */
    state->hcdc = handle;

    info->basic.flags = 0;

    /* Set up iterface string descriptors */
    j_strlcpy(state->comm_str, COMM_STR, COMM_STR_SIZE);
    j_strlcpy(state->data_str, DATA_STR, DATA_STR_SIZE);
    info->basic.comm_string = state->comm_str;
    info->basic.data_string = state->data_str;

    /* Set up relevant callbacks */
    cbs->basic.enabled_h = samp_enabled;
    cbs->basic.disabled_h = samp_disabled;
    cbs->basic.uninit_h = samp_uninit;
    
    cbs->basic.encap_command_h = NULL;
    cbs->basic.encap_response_complete_h = NULL;

    cbs->basic.read_complete_h = samp_read_done;
    cbs->basic.write_complete_h = samp_write_done;

#ifdef CONFIG_TEST_STATIC_INTERFACE  
    /* TODO: Initialize interface number here */
    info->basic.interface_number = 60;
#endif

    return 0;

Error:
    samp_uninit(ctx);
    return -1;
}


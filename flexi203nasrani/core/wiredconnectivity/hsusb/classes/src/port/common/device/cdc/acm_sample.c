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

#include <uw_device_cdc_acm.h>
#include <jos.h>

#define MAX_RESPONSE_LEN 128
#define RESPONSE_STR_PREFIX "Got Response: "

typedef struct
{
    cdc_handle_t hcdc;

    /* My variables*/
#define LOOP_SIZE 64
#define LOOP_BUFFERS 5
    juint8_t loop[LOOP_BUFFERS][LOOP_SIZE];
    jint_t next_buf;
#define COMM_STR "JACM_COMMUNICATION_INTERFACE"
#define COMM_STR_SIZE sizeof(COMM_STR)
#define DATA_STR "JACM_DATA_INTERFACE"
#define DATA_STR_SIZE sizeof(DATA_STR)
    char comm_str[COMM_STR_SIZE];
    char data_str[DATA_STR_SIZE];
} samp_ctx_t;

static jresult_t samp_uninit(cdc_appctx_t ctx)
{
    samp_ctx_t *state = (samp_ctx_t*)ctx;

    /* Perform application specific cleanup */
    
    /* Free our context */
    if (state)
        jfree(state);

    return 0;
}

static juint32_t _strlcat(char *str, const char *app, juint32_t len)
{
    while(*str != 0)
        str++;

    return j_strlcpy(str, app, len);
}

static void hex_dump(void *buffer, juint32_t size)
{
    juint32_t i, n;
    juint8_t *buff = (juint8_t*)buffer;
    char line[256];
    juint32_t line_size = 256;
    char tmp[12];

    while(size)
    {
        *line = 0;
        
        for(i=0; i<16 && size; i++,size--,buff++)
        {
            n = j_snprintf(tmp, sizeof(tmp)-1, "%02X ", (juint32_t)*buff);
            tmp[sizeof(tmp)-1] = 0;
            _strlcat(line, tmp, line_size);
            line_size -= n;
        }

        _strlcat(line, "    ", line_size);
        size += i;
        buff -= i;
        line_size-= j_strlen("    ");

        for (; i < 16; i++)
        {
            _strlcat(line, "   ", line_size);
            line_size-= j_strlen("   ");
        }
        
        for (i = 0; i < 16 && size; i++, size--, buff++)
        {
            if (*buff > 31)
            {
                n = j_snprintf(tmp, sizeof(tmp)-1, "%c", *buff);
                tmp[sizeof(tmp)-1] = 0;         
                _strlcat(line, tmp, line_size);
                line_size -= n;
            }
            else
            {
                _strlcat(line, ".", line_size);
                line_size -= j_strlen(".");
            }
        }

        DBG_I(DSLAVE_CDC_ACM, ("\n\t%s", line));
    }

    DBG_I(DSLAVE_CDC_ACM, ("\n"));
}

static jresult_t samp_cmd(void *command, juint16_t size, cdc_appctx_t ctx)
{
    char response_str[MAX_RESPONSE_LEN];
    samp_ctx_t *status = (samp_ctx_t*)ctx;
    DECLARE_FNAME("samp_cmd");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered. size: %d\n", fname, size));

    if (size + j_strlen(RESPONSE_STR_PREFIX) > MAX_RESPONSE_LEN)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: max size exceeded\n", fname));
        return JEINVAL;
    }

    hex_dump(command, size);

    /* Return the command back to the host in a response (loopback) */
    j_snprintf(response_str, sizeof(response_str)-1, "%s%s", 
        RESPONSE_STR_PREFIX, command);
    response_str[sizeof(response_str)-1] = 0;
    cdc_send_encap_response(status->hcdc, response_str, 
        j_strlen(response_str+1));
    return 0;
}

static void samp_resp_done(cdc_iostatus_t result, juint32_t bytes_sent,
    cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_resp_done");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
}

static void samp_read_done(cdc_iostatus_t result, void *buffer,
    juint32_t size, jbool_t transfer_complete, cdc_appctx_t ctx)
{
    samp_ctx_t *state = (samp_ctx_t*)ctx;
    char *p = (char*)buffer;
    static char LF[1] = { 0x0a };
    DECLARE_FNAME("samp_read_done");
        
    DBG_I(DSLAVE_CDC_ACM, ("%s: entered", fname));

    if (result == CDC_TRANSFER_CANCELLED)
    {
        DBG_I(DSLAVE_CDC_ACM, ("%s: request was cancelled\n", fname));
        if (size)
            hex_dump(buffer, size);
        return;
    }

    if (buffer != state->loop[state->next_buf])
        DBG_W(DSLAVE_CDC_ACM, ("%s: mismatch in buffers\n", fname));

    hex_dump(buffer, size);

    if (result != CDC_TRANSFER_ERROR)
    {
        cdc_write(state->hcdc, buffer, size);
        if (*p == 0x0d)
            cdc_write(state->hcdc, LF, 1);
    }
            
    cdc_read(state->hcdc, state->loop[state->next_buf], LOOP_SIZE);
    state->next_buf = (state->next_buf + 1) % LOOP_BUFFERS;
}

static void samp_write_done(cdc_iostatus_t result, void *buffer,
    cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_write_done");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
}

static jresult_t samp_set_comm(acm_comm_fs_t selector,
    union acm_comm_feature_setting_t *status, cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_set_comm");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
    DBG_I(DSLAVE_CDC_ACM, ("%s: selector %u, status: %u\n", fname, selector,
        status->abstract_state));
    return 0;
}

static jresult_t samp_get_comm(acm_comm_fs_t selector,
    union acm_comm_feature_setting_t *status, cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_get_comm");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
    DBG_I(DSLAVE_CDC_ACM, ("%s: selector %u\n", fname, selector));
    status->abstract_state = 0;
    return 0;
}

static jresult_t samp_clear_comm(acm_comm_fs_t selector, cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_clear_comm");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
    DBG_I(DSLAVE_CDC_ACM, ("%s: selector %u\n", fname, selector));
    return 0;
}

static jresult_t samp_set_coding(acm_line_coding_t *coding, cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_set_coding");
    
    DBG_I(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
    DBG_I(DSLAVE_CDC_ACM, ("%s: DTE rate: %u, stop %u, parity %u, data %u\n",
        fname, coding->dte_rate, coding->char_format,
        coding->parity_type, coding->data_bits));
    return 0;
}

static jresult_t samp_get_coding(acm_line_coding_t *coding, cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_get_coding");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
    coding->dte_rate = 9600;
    coding->char_format = ACM_LC_1_STOP_BITS;
    coding->parity_type = ACM_LC_PARITY_NONE;
    coding->data_bits = ACM_LC_DATA_8_BITS;
    return 0;
}

static jresult_t samp_set_state(juint16_t ctrl_sig_bitmap, cdc_appctx_t ctx)
{
    samp_ctx_t *state = (samp_ctx_t*)ctx;
    jresult_t rc;
    DECLARE_FNAME("samp_set_state");
    
    DBG_I(DSLAVE_CDC_ACM, ("%s: entered %u\n", fname, ctrl_sig_bitmap));

    if (ctrl_sig_bitmap & ACM_CONTROL_SIG_DTR)
    {
        jint_t i;

        DBG_I(DSLAVE_CDC_ACM, ("%s: SIG_DTR\n", fname));

        state->next_buf = 0;
        for (i = 0; i < LOOP_BUFFERS; i++)
        {
            rc = cdc_read(state->hcdc, state->loop[i], LOOP_SIZE);
            if (rc < 0)
            {
                DBG_E(DSLAVE_CDC_ACM, ("%s: failed to post cdc_read (%d): "
                    "%u\n", fname, i, rc));
                break;
            }
            else
            {
                DBG_I(DSLAVE_CDC_ACM, ("%s: posted read request %d\n", fname,
                    i));
            }
        }
    }
    else
    {
        cdc_abort_write(state->hcdc);
        cdc_abort_read(state->hcdc);
    }
    
    if (ctrl_sig_bitmap & ACM_CONTROL_SIG_RTS)
        DBG_I(DSLAVE_CDC_ACM, ("%s: SIG_RTS\n", fname));
    
    return 0;
}

static jresult_t samp_break(juint16_t break_duration, cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_break");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
    DBG_I(DSLAVE_CDC_ACM, ("%s: duration: %u\n", fname, break_duration));
    return 0;
}

static jresult_t samp_enabled(cdc_appctx_t ctx)
{
    jresult_t rc;
    samp_ctx_t *state = (samp_ctx_t*)ctx;
    DECLARE_FNAME("samp_enabled");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
    
    rc = acm_notify_serial_state(state->hcdc, ACM_SERIAL_DSR|ACM_SERIAL_DCD);
    if (rc < 0)
    {
        DBG_E(DSLAVE_CDC_ACM, ("%s: failed to send ser_state notification: "
            "%u\n", fname, rc));
    }

    return 0;
}

static void samp_serstate_complete(cdc_iostatus_t result, cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_serstate_complete");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered %d\n", fname, result));
}

static jresult_t samp_disabled(cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_disabled");

    DBG_I(DSLAVE_CDC_ACM, ("%s: entered\n", fname));
    return 0;
}

jresult_t acm_init(cdc_handle_t handle, acm_callbacks_t *cbs,
    acm_init_info_t *info, cdc_appctx_t *ctx)
{
    samp_ctx_t *state;
    
    /* Allocte our state (context) */
    state = jmalloc(sizeof(samp_ctx_t), M_ZERO);
    if (!state)
        goto Error;

    /* Store our state with CDC */
    *ctx = (cdc_appctx_t*)state;

    /* Save CDC handle (used for calling CDC API's) */
    state->hcdc = handle;

    /* Show what we are capable of :) */
    info->capabilities = ACM_CAP_COMM_FEATURE |
        ACM_CAP_LINE_CODING | ACM_CAP_SEND_BREAK |
        ACM_CAP_NETWORK_CONNECTION | ACM_CAP_CALL_MGMT |
        ACM_CAP_CALL_MGMT_OVER_DC;

    info->basic.flags = 0;

#ifdef CONFIG_TEST_STATIC_INTERFACE  
    /* TODO: Initialize interface number here */
    info->basic.interface_number = 53;
#endif

    /* Set up interface string descriptors */
    j_strlcpy(state->comm_str, COMM_STR, COMM_STR_SIZE);
    j_strlcpy(state->data_str, DATA_STR, DATA_STR_SIZE);
    info->basic.comm_string = state->comm_str;
    info->basic.data_string = state->data_str;

    /* Set up relevant callbacks */
    cbs->basic.enabled_h = samp_enabled;
    cbs->basic.disabled_h = samp_disabled;
    cbs->basic.uninit_h = samp_uninit;
    
    cbs->basic.encap_command_h = samp_cmd;
    cbs->basic.encap_response_complete_h = samp_resp_done;

    cbs->basic.read_complete_h = samp_read_done;
    cbs->basic.write_complete_h = samp_write_done;

    /* ACM Host to device request handlers */
    cbs->set_comm_feature_h = samp_set_comm;
    cbs->get_comm_feature_h = samp_get_comm;
    cbs->clear_comm_feature_h = samp_clear_comm;
    cbs->set_line_coding_h = samp_set_coding;
    cbs->get_line_coding_h = samp_get_coding;
    cbs->set_control_line_state_h = samp_set_state;
    cbs->send_break_h = samp_break;

    /* ACM Request completion handlers */
    cbs->get_comm_feature_complete_h = NULL;
    cbs->get_line_coding_complete_h = NULL;

    /* ACM Notification completion handlers */
    cbs->notify_net_conn_complete_h = NULL;
    cbs->notify_serial_state_complete_h = samp_serstate_complete;

    return 0;

Error:
    samp_uninit(ctx);
    return -1;
}


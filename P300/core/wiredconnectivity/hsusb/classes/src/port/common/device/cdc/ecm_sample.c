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

#include <uw_device_cdc_ecm.h>
#include <jos.h>
#include "ecm_sample.h"

#define MAX_SEG_SIZE 1514
#define MAX_RESPONSE_LEN 128
#define RESPONSE_STR_PREFIX "Got Response: "

typedef struct
{
    cdc_handle_t hcdc;
    netdev_h dev;
#define COMM_STR "JECM_COMMUNICATION_INTERFACE"
#define COMM_STR_SIZE sizeof(COMM_STR)
#define DATA_STR "JECM_DATA_INTERFACE"
#define DATA_STR_SIZE sizeof(DATA_STR)
    char comm_str[COMM_STR_SIZE];
    char data_str[DATA_STR_SIZE];
} samp_ctx_t;

static jresult_t samp_uninit(cdc_appctx_t ctx)
{
    samp_ctx_t *state = (samp_ctx_t*)ctx;

    /* Perform application specific cleanup */
    if (state)
    {
        if (state->dev)
            net_unbind(state->dev);

        /* Free our context */
        jfree(state);
    }

    return 0;
}

static juint32_t  _strlcat(char *str, const char *app, juint32_t len)
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

    DBG_I(DSLAVE_CDC_ECM, ("\n"));
    while (size)
    {
        *line = 0;
        
        for (i=0; i<16 && size; i++,size--,buff++)
        {
            n = j_snprintf(tmp, sizeof(tmp), "%02X ", (juint32_t)*buff);
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
            if (*buff > 31 && *buff < 127)
            {
                n = j_snprintf(tmp, sizeof(tmp), "%c", *buff);            
                _strlcat(line, tmp, line_size);
                line_size -= n;
            }
            else
            {
                _strlcat(line, ".", line_size);
                line_size -= j_strlen(".");
            }
        }

        DBG_I(DSLAVE_CDC_ECM, ("\t%s\n", line));
    }
}

static jresult_t samp_cmd(void *command, juint16_t size, cdc_appctx_t ctx)
{
    char response_str[MAX_RESPONSE_LEN];
    samp_ctx_t *status = (samp_ctx_t*)ctx;
    DECLARE_FNAME("samp_cmd");

    DBG_I(DSLAVE_CDC_ECM, ("%s: entered\n", fname));

    if (size + j_strlen(RESPONSE_STR_PREFIX) > MAX_RESPONSE_LEN)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: entered\n", fname));
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
    DBG_I(DSLAVE_CDC_ECM, ("%s: entered\n", fname));
}

static void samp_read_done(cdc_iostatus_t result, void *buffer,
    juint32_t size, jbool_t transfer_complete, cdc_appctx_t ctx)
{
    samp_ctx_t *status = (samp_ctx_t*)ctx;
    DECLARE_FNAME("samp_read_done");

    DBG_I(DSLAVE_CDC_ECM, ("%s: entered\n", fname));
    hex_dump(buffer, size);
    net_rx_complete(status->dev, size, (result==CDC_TRANSFER_ERROR)?-1:0);
}

static void samp_write_done(cdc_iostatus_t result, void *buffer,
    cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_write_done");

    DBG_I(DSLAVE_CDC_ECM, ("%s: entered\n", fname));
    net_tx_complete( ((samp_ctx_t*)ctx)->dev, (result==CDC_TRANSFER_ERROR)?-1:0 );
}

static jresult_t samp_enabled(cdc_appctx_t ctx)
{
    DECLARE_FNAME("samp_enabled");
    
    DBG_I(DSLAVE_CDC_ECM, ("%s: entered\n", fname));
    net_enable(((samp_ctx_t*)ctx)->dev);
    return 0;
}

static jresult_t samp_disabled(cdc_appctx_t ctx)
{
    samp_ctx_t *state = (samp_ctx_t*)ctx;
    DECLARE_FNAME("samp_disabled");

    DBG_I(DSLAVE_CDC_ECM, ("%s: entered\n", fname));
    cdc_abort_read(state->hcdc);
    cdc_abort_write(state->hcdc);
    net_disable(state->dev);
    return 0;
}

static jresult_t samp_set_mc_filter(mac_addr_t *mc_filters,
    juint16_t nfilters, cdc_appctx_t ctx)
{
    return 0;
}

static jresult_t samp_set_pm_filter(juint16_t filter_id,
    juint16_t filter_size, ecm_pm_filter_t *pm_filter, cdc_appctx_t ctx)
{
    return 0;
}

static jresult_t samp_get_pm_filter(juint16_t filter_id,
    jbool_t *is_active, cdc_appctx_t ctx)
{
    return 0;
}

static jresult_t samp_set_pkt_filter(juint16_t pf_bitmask,
    cdc_appctx_t ctx)
{
    DBG_I(DSLAVE_CDC_ECM, ("samp_get_pkt_filter: called with %x\n",
        pf_bitmask));
    return 0;
}

static jresult_t samp_get_stats(ecm_enum_stats_t feature, juint32_t *count,
    cdc_appctx_t ctx)
{
    samp_ctx_t *state = (samp_ctx_t*)ctx;
    jresult_t rc = 0;

    switch(feature)
    {
    case ECM_FS_XMIT_OK:
        *count = net_get_stats(state->dev, NET_STATS_TX_PACKETS);
        DBG_I(DSLAVE_CDC_ECM, ("samp_get_stats: XMIT_OK: %d\n", *count));
        break;

    case ECM_FS_XMIT_ERR:
        *count = net_get_stats(state->dev, NET_STATS_TX_ERRORS);
        DBG_I(DSLAVE_CDC_ECM, ("samp_get_stats: XMIT_ERR: %d\n", *count));
        break;

    default:
        DBG_I(DSLAVE_CDC_ECM, ("samp_get_stats: unknown feature!\n", *count));
        rc = -1;
        break;
    }

    return rc;
}

/* Operation callbacks for network device */
jint_t upper_rx(lower_ctx_h ctx, void *buf, juint32_t size)
{
    jint_t rc;

    rc = cdc_read(((samp_ctx_t*)ctx)->hcdc, buf, size);

    DBG_RUN(DSLAVE_CDC_ECM, DL_ERROR,
        if (rc)
            DBG_E(DSLAVE_CDC_ECM, ("upper_rx: cdc_read failed\n")));

    return rc;
}

jint_t upper_tx(lower_ctx_h ctx, void *buf, juint32_t size)
{
    jint_t rc;

    rc = cdc_write(((samp_ctx_t*)ctx)->hcdc, buf, size);

    DBG_RUN(DSLAVE_CDC_ECM, DL_ERROR,
        if (rc)
            DBG_E(DSLAVE_CDC_ECM, ("upper_tx: cdc_write failed\n")));

    return rc;
}

jint_t upper_abort_rx(lower_ctx_h ctx)
{
    return cdc_abort_read(((samp_ctx_t*)ctx)->hcdc);
}

jint_t upper_abort_tx(lower_ctx_h ctx)
{
    return cdc_abort_write(((samp_ctx_t*)ctx)->hcdc);
}

jresult_t ecm_init(cdc_handle_t handle, ecm_callbacks_t *cbs,
    ecm_init_info_t *info, cdc_appctx_t *ctx)
{
    samp_ctx_t *state;
    net_lower_ops_t ops;
    struct net_macaddr NIC_ADDR = {{ 0x00, 0x0D, 0x87, 0x8E, 0x4B, 0xAC }};

    /* Allocte our state (context) */
    state = jmalloc(sizeof(samp_ctx_t), M_ZERO);
    if (!state)
        goto Error;

    ops.rx = upper_rx;
    ops.tx = upper_tx;
    ops.abort_rx = upper_abort_rx;
    ops.abort_tx = upper_abort_tx;

#ifdef CONFIG_TEST_STATIC_INTERFACE  
    /* TODO: Initialize interface number here */
    info->basic.interface_number = 3;
#endif

    /* Initialize networking upper layer */
    if (net_bind(&state->dev, &ops, &NIC_ADDR, state))
    {
        DBG_E(DSLAVE_CDC_ECM, ("ecm_init: net_bind() failed\n"));
        goto Error;
    }

    /* Store our state with CDC */
    *ctx = (cdc_appctx_t*)state;

    /* Save CDC handle (used for calling CDC API's) */
    state->hcdc = handle;

    /* Show who we are and what we are capable of :) */
    info->eth_statistics = ECM_FS_XMIT_OK | ECM_FS_XMIT_ERR;
    j_strlcpy(info->mac_addr, "000D878E4BAC", MAC_ADDR_STR_SIZE);
    info->num_power_filters = 1;
    info->mc_filters.perfect_filtering = 1;
    info->mc_filters.num_filters = 1;
    
    info->basic.flags = 0;
    info->basic.max_segment_size = MAX_SEG_SIZE;

    /* Set up intetrface string descriptors */
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

    /* ECM Host to device request handlers */
    cbs->set_multicast_filter_h = samp_set_mc_filter;
    cbs->set_power_filter_h = samp_set_pm_filter;
    cbs->get_power_filter_h = samp_get_pm_filter;
    cbs->set_packet_filter_h = samp_set_pkt_filter;
    cbs->get_stats_h = samp_get_stats;

    /* ECM Request completion handlers */
    cbs->get_power_filter_complete_h = NULL;
    cbs->get_stats_complete_h = NULL;

    /* ECM Notification completion handlers */
    cbs->notify_net_conn_complete_h = NULL;
    cbs->notify_speed_change_complete_h = NULL;

    return 0;

Error:
    samp_uninit(ctx);
    return -1;
}


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
 * static structure for saving ecm specifics like string decriptors
 **/
typedef struct _ecm_ctx_t 
{
    cdc_ctx_t cdc_ctx;          /* the CDC context */
    juint8_t iMACAddress;        /* iMACAddress descriptor index */
} ecm_ctx_t;  
     
/*
 * Host to Device - Requests
 */

/**
 * Function name:  set_multicast_filters_complete
 * Description: completion handler for the SET_MULTICAST_FILTERS request
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void set_multicast_filters_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    juint16_t nfilters;
    jresult_t rc;
    DECLARE_FNAME("set_multicast_filters_complete");

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", fname));

    if (cdc_get_iostatus(req, 1) != CDC_TRANSFER_OK)
        goto Error;
    
    /* Pass data to application */
    nfilters = le16toh(ctx->setup_packet.wValue);

    if (cbs->set_multicast_filter_h((mac_addr_t*)req->buffer.vaddr,
        nfilters, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: application failed to "
            "handle request\n", fname));
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
 * Function name:  handle_set_multicast_filters
 * Description: handler for the SET_MULTICAST_FILTERS request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_set_multicast_filters(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    request_t *reply = NULL;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_set_multicast_filters");

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", fname));

    if (!cbs->set_multicast_filter_h)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: no handler defined for request\n", fname));
        goto Error;
    }

    /* Save setup packet */
    j_memcpy(&ctx->setup_packet, req, sizeof(ctx->setup_packet));

    /* Setup reply */
    reply = cdc_create_ep0_read(ctx, le16toh(req->wLength));
    if (!reply)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: failed to setup reply\n", fname));
        goto Error;
    }

    reply->complete = set_multicast_filters_complete;
    reply->context = ctx;

    rc = core_read_data(ctx->core_ctx, 0, reply);
    
Error:
    return rc;
}

/*HSU FIX: Adding support for SET_CONTROL_LINE_STATE on ECM */
/**
* Function name:  handle_set_control_line_state
* Description:  handler for SET_CONTROL_LINE_STATE request
* Parameters: 
*     @ctx: (IN) CDC context
*     @req: (IN) USB SETUP packet
*
* Return value: 0 for success, or an error code
* Scope: local
**/
static jresult_t handle_set_control_line_state(cdc_ctx_t *ctx,
struct usb_ctrlrequest *req)
{
  ecm_callbacks_t *cbs = NULL;
  juint16_t ctrl_sig_bitmap = 0;
  jresult_t rc = -1;
  DECLARE_FNAME("handle_set_control_line_state");

  DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", fname));

  HSU_ASSERT(ctx != NULL);
  HSU_ASSERT(req != NULL);

  cbs = &ctx->cbs.ecm;
  if (!cbs->set_control_line_state_h)
  {
    DBG_E(DSLAVE_CDC_ECM, ("%s: no handler defined for request\n"));
    goto Error;
  }

  /* Pass data to application */
  ctrl_sig_bitmap = le16toh(req->wValue);
  if (cbs->set_control_line_state_h(ctrl_sig_bitmap, ctx->app_ctx) < 0)
  {
    DBG_E(DSLAVE_CDC_ECM, ("%s: application failed to handle request\n"));
    goto Error;
  }

  rc = cdc_send_status(ctx);

Error:
  return rc;
}
/*End of HSU FIX*/

/**
 * Function name:  set_power_filter_complete
 * Description: completion handler for the SET_POWER_FILTER request
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void set_power_filter_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    juint16_t filter_id;
    juint16_t filter_size;
    ecm_pm_filter_t pm_filter;
    juint8_t *bytep;
    jresult_t rc;
    DECLARE_FNAME("set_power_filter_complete");

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", fname));

    if (cdc_get_iostatus(req, 1) != CDC_TRANSFER_OK)
        goto Error;
    
    /* Pass data to application */
    filter_id = le16toh(ctx->setup_packet.wValue);
    filter_size = le16toh(ctx->setup_packet.wLength);
    bytep = (juint8_t*)(req->buffer.vaddr);
    pm_filter.mask_size = le16toh(*(juint16_t*)req->buffer.vaddr);
    pm_filter.mask = bytep+2;
    pm_filter.pattern = bytep+2+pm_filter.mask_size;
    if (cbs->set_power_filter_h(filter_id, filter_size, &pm_filter,
        ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: application failed to handle request\n",
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
 * Function name:  handle_set_power_filter
 * Description: handler for the SET_POWER_FILTER request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_set_power_filter(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    request_t *reply = NULL;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_set_power_filter");

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", fname));

    if (!cbs->set_power_filter_h)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: no handler defined for request\n", fname));
        goto Error;
    }

    /* Save setup packet */
    j_memcpy(&ctx->setup_packet, req, sizeof(ctx->setup_packet));

    /* Setup reply */
    reply = cdc_create_ep0_read(ctx, le16toh(req->wLength));
    if (!reply)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: failed to setup reply\n", fname));
        goto Error;
    }

    reply->complete = set_power_filter_complete;
    reply->context = ctx;

    rc = core_read_data(ctx->core_ctx, 0, reply);
    
Error:
    return rc;
}

/**
 * Function name:  get_power_filter_complete
 * Description: completion handler for the GET_POWER_FILTER request
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 *Scope: local
 **/
static void get_power_filter_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;

    DBG_X(DSLAVE_CDC_ECM, ("get_power_filter_complete: entered\n"));
    
    if (cbs->get_power_filter_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->get_power_filter_complete_h(status, ctx->app_ctx);
    }
}

/**
 * Function name:  handle_get_power_filter
 * Description: handler for the GET_POWER_FILTER request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) CORE request
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_get_power_filter(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    request_t *reply = NULL;
    juint16_t filter_id;
    juint16_t pattern_active;
    jbool_t is_active;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_get_power_filter");

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", fname));

    if (!cbs->get_power_filter_h)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: no handler defined for request\n", fname));
        goto Error;
    }

    /* Get data from application */
    filter_id = le16toh(req->wValue);
    if (cbs->get_power_filter_h(filter_id, &is_active, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: application failed to "
            "handle request\n", fname));
        goto Error;
    }

    /* Setup reply */
    pattern_active = htole16(is_active?1:0);
    reply = cdc_create_ep0_write(ctx, &pattern_active, sizeof(pattern_active),
        le16toh(req->wLength));
    if (!reply)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: failed to setup reply\n", fname));
        goto Error;
    }

    reply->complete = get_power_filter_complete;
    reply->context = ctx;
    
    rc = core_send_data(ctx->core_ctx, 0, reply);

Error:
    return rc;
}

/**
 * Function name:  handle_set_packet_filter
 * Description: handler for the SET_PACKET_FILTER request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_set_packet_filter(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    juint16_t pf_bitmask = 0;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_set_packet_filter");

    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", fname));

    if (!cbs->set_packet_filter_h)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: no handler defined for request\n", fname));
        goto Error;
    }

    /* Pass data to application */
    pf_bitmask = le16toh(req->wValue);
    if (cbs->set_packet_filter_h(pf_bitmask, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: application failed to handle request\n",
            fname));
        goto Error;
    }

    rc = cdc_send_status(ctx);
    
Error:
    return rc;
}

/**
 * Function name:  get_statistics_complete
 * Description: completion handler for the GET_ETHERNET_STATISTICS request
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void get_statistics_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;

    DBG_X(DSLAVE_CDC_ECM, ("get_statistics_complete: entered\n"));
    
    if (cbs->get_stats_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->get_stats_complete_h(status, ctx->app_ctx);
    }
}

/**
 * Function name:  handle_get_statistics
 * Description: handler for the GET_ETHERNET_STATISTICS request
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @req: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t handle_get_statistics(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req)
{
    ecm_callbacks_t *cbs = &ctx->cbs.ecm;
    ecm_init_info_t *info = &ctx->info.ecm;
    request_t *reply = NULL;
    ecm_enum_stats_t selector = ECM_FS_NONE;
    juint32_t count;
    jresult_t rc = -1;
    DECLARE_FNAME("handle_get_statistics");
    
    DBG_X(DSLAVE_CDC_ECM, ("%s: entered\n", fname));

    if (!cbs->get_stats_h)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: no handler defined for request\n", fname));
        goto Error;
    }

    if (le16toh(req->wValue))
	selector = (ecm_enum_stats_t)(1 << (le16toh(req->wValue)-1));

    /* Check against device capabilities */
    if ( !(selector & info->eth_statistics) )
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: request for unsupported statistic (%u)\n",
            fname, selector));
        goto Error;
    }
    
    /* Get data from application */
    if (cbs->get_stats_h(selector, &count, ctx->app_ctx) < 0)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: application failed to handle request\n",
            fname));
        goto Error;
    }

    /* Setup reply */
    count = htole32(count);
    reply = cdc_create_ep0_write(ctx, &count, sizeof(count), 
        le16toh(req->wLength));
    if (!reply)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: failed to setup reply\n", fname));
        goto Error;
    }

    reply->complete = get_statistics_complete;
    reply->context = ctx;
    
    rc = core_send_data(ctx->core_ctx, 0, reply);

Error:
    return rc;
}


/**
 * Function name:  ecm_handle_request
 * Description:  ECM model control interface request handler
 * Parameters: 
 *     @context: (IN) CDC context
 *     @buffer: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t ecm_handle_request(void *context, void *buffer)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    struct usb_ctrlrequest *req = (struct usb_ctrlrequest*)buffer;
    jresult_t rc = JENOTSUP;

    DBG_X(DSLAVE_CDC_ECM, ("ecm_handle_request: called with request (%u)\n",
        req->bRequest));
    
    switch(req->bRequest)
    {
    case CDC_REQ_SEND_ENCAPSULATED_COMMAND:
        rc = cdc_handle_encapsulated_command(ctx, req);
        break;
        
    case CDC_REQ_GET_ENCAPSULATED_RESPONSE:
        rc = cdc_handle_encapsulated_response(ctx, req);
        break;
        
    case CDC_REQ_SET_ETHERNET_MULTICAST_FILTERS:
        rc = handle_set_multicast_filters(ctx, req);
        break;
        
    case CDC_REQ_SET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER:
        rc = handle_set_power_filter(ctx, req);
        break;
        
    case CDC_REQ_GET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER:
        rc = handle_get_power_filter(ctx, req);
        break;
        
    case CDC_REQ_SET_ETHERNET_PACKET_FILTER:
        rc = handle_set_packet_filter(ctx, req);
        break;
        
    case CDC_REQ_GET_ETHERNET_STATISTIC:
        rc = handle_get_statistics(ctx, req);
        break;
/*HSU FIX: Adding support for SET_CONTROL_LINE_STATE on ECM*/
    case CDC_REQ_SET_CONTROL_LINE_STATE:
      rc = handle_set_control_line_state(ctx, req);
      break;  
/* End of HSU fix */

    default:
        DBG_E(DSLAVE_CDC_ECM, ("ecm_handle_request: unsupported request (%u)\n",
            req->bRequest));
        break;
    }

    return rc;
}

/*
 * Device to Host - Notifications
 */

/**
 * Function name:  notify_conn_speed_change_complete
 * Description: completion handler for the CONNECTION_SPEED_CHANGE notification
 * Parameters: 
 *     @req: (IN) CORE request
 *     
 * Scope: local
 **/
static void notify_conn_speed_change_complete(request_t *req)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)req->context;
    ecm_callbacks_t *cbs = &(ctx->cbs.ecm);

    DBG_X(DSLAVE_CDC_ECM, ("notify_conn_speed_change_complete: entered\n"));
    
    if (cbs->notify_speed_change_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->notify_speed_change_complete_h(status, ctx->app_ctx);
    }

    core_request_free(req);
}

/**
 * Function name:  ecm_notify_conn_speed_change
 * Description:  Control Interface API - sends a CONNECTION_SPEED_CHANGE
 *               notification
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @conn_speed: (IN) connection speed
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t ecm_notify_conn_speed_change(cdc_handle_t handle,
    ecm_conn_speed_t *conn_speed)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    request_t *req = NULL;
    jresult_t rc = 0;
    ecm_conn_speed_t c_speed;

    if (ctx->intr_pipe.desc == NULL)
    {
        rc = JENXIO;
        goto Error;
    }

    c_speed.upstream_bitrate = htole32(conn_speed->upstream_bitrate);
    c_speed.downstream_bitrate = htole32(conn_speed->downstream_bitrate);
    req = cdc_create_notification(ctx, CDC_NTF_CONNECTION_SPEED_CHANGE, 0,
        ctx->intr_pipe.if_num, sizeof(c_speed), (juint8_t*)&c_speed);

    if (!req)
    {
        rc = JENOMEM;
        goto Error;
    }
    
    req->complete = notify_conn_speed_change_complete;
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
    ecm_callbacks_t *cbs = &(ctx->cbs.ecm);

    DBG_X(DSLAVE_CDC_ECM, ("notify_network_connection_complete: entered\n"));

    if (cbs->notify_net_conn_complete_h)
    {
        cdc_iostatus_t status = cdc_get_iostatus(req, 1);
        cbs->notify_net_conn_complete_h(status, ctx->app_ctx);
    }

    core_request_free(req);
}

/**
 * Function name:  ecm_notify_network_connection
 * Description:  Control Interface API - sends a NETWORK_CONNECTION notification
 * Parameters: 
 *     @handle: (IN) CDC handle
 *     @link_up: (IN) 0 = link down, 1 = link up
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t ecm_notify_network_connection(cdc_handle_t handle, jbool_t link_up)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)handle;
    request_t *req = NULL;
    jresult_t rc = 0;

    if (ctx->intr_pipe.desc == NULL)
    {
        rc = JENXIO;
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
 * Function name:  ecm_get_functional_headers
 * Description:  this function is called by cdc_alloc_descriptors() to create
 *               the functional descriptors for ECM. It should:
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
static juint32_t ecm_get_functional_headers(cdc_ctx_t *ctx, void *buffer,
    jbool_t is_single_iface, juint8_t interface_number)
{
    ecm_ctx_t *ecm_ctx = (ecm_ctx_t *)ctx;
    juint32_t size = 0;
    struct cdc_header_fdesc *hdr;
    struct cdc_ecm_fdesc *ecm;
    struct cdc_union_fdesc *un;
    juint8_t avail_iface;
    juint8_t data_iface;

#ifdef FEATURE_HS_USB_TLP
    struct cdc_mdlm_fdesc *mdlm;
    struct cdc_mdlm_detail_fdesc *mdlm_detail;
    const juint8_t guid[16] = QCOM_16_BYTES_GUID;
#endif /* FEATURE_HS_USB_TLP */

    DBG_X(DSLAVE_CDC_ECM, ("ecm_get_functional_headers entered\n"));

    size = sizeof(struct cdc_header_fdesc) +
        sizeof(struct cdc_ecm_fdesc) +
        sizeof(struct cdc_union_fdesc);

#ifdef FEATURE_HS_USB_TLP

    size += (sizeof(struct cdc_mdlm_fdesc) + 
      sizeof(struct cdc_mdlm_detail_fdesc));

#endif /* FEATURE_HS_USB_TLP */

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

        ecm = (struct cdc_ecm_fdesc*)(hdr+1);
        ecm->bLength = sizeof(struct cdc_ecm_fdesc);
        ecm->bType = USB_DT_CS_INTERFACE;
        ecm->bSubtype = USB_CDC_SUBTYPE_ECM;
        ecm->iMACAddress = core_add_string_descriptor(ctx->core_ctx,
            ctx->info.ecm.mac_addr, HSU_STR_IDX_NOT_SET);
        
        /* save string descriptor Index is ecm struct  */
        ecm_ctx->iMACAddress = ecm->iMACAddress;
        
        ua_htole32(&ecm->bmEtherStats, ctx->info.ecm.eth_statistics);
        ua_htole16(&ecm->wMaxSegSize, ctx->info.basic.max_segment_size);
        ua_htole16(&ecm->wNumMCFilters, 
	    (*(juint16_t*)(&ctx->info.ecm.mc_filters)));
        ecm->bNumPowerFilters = ctx->info.ecm.num_power_filters;

        un = (struct cdc_union_fdesc*)(ecm+1);
        un->bLength             = sizeof(struct cdc_union_fdesc);
        un->bType               = USB_DT_CS_INTERFACE;
        un->bSubtype            = USB_CDC_SUBTYPE_UNION;
        un->bMasterIface        = avail_iface;
        un->bSlaveIface[0]      = data_iface;

#ifdef FEATURE_HS_USB_TLP

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
          (juint8_t)USB_CDC_MDLM_DETAIL_DATA_CAPS_DL_TLP; 
   /* Uncomment the folowing line when UP TLP is supported by the host driver*/
         /* | (juint8_t)USB_CDC_MDLM_DETAIL_DATA_CAPS_UL_TLP; */

#endif /* FEATURE_HS_USB_TLP */
    }

    return size;
}

/**
 * Function name:  ecm_disable
 * Description:  USB CORE 'disable' callback (overrides default in cdc_fd.c)
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: None
 * Scope: local
 **/
static void ecm_disable(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    
    DBG_X(DSLAVE_CDC_ECM, ("ecm_disable: entered\n"));

    if (!ctx)
        return;

    if (ctx->cbs.basic.disabled_h && ctx->app_enabled)
    {
        ctx->app_enabled = 0;
        ctx->cbs.basic.disabled_h(ctx->app_ctx);
    }

    cdc_disable(ctx);
}

/**
 * Function name:  ecm_enable
 * Description:  USB CORE 'enable' callback (overrides default in cdc_fd.c)
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t ecm_enable(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    jresult_t rc = 0;
    jint_t data_if_num = 1;

    DBG_X(DSLAVE_CDC_ECM, ("ecm_enable: entered\n"));

    if (!ctx)
        return JEINVAL;

    rc = cdc_enable(ctx);
    if (rc)
        return rc;

    if (ctx->info.basic.flags & CDC_INIT_SINGLE_INTERFACE)
        data_if_num = 0;

    /* first factor in the bellow expression is HSU addition:
       API to disable alt. settings */
    if (((ctx->info.basic.flags & CDC_INIT_NO_ALT_SETTINGS) ||
        (ctx->desc->interfaces[data_if_num].alt_if_curr != 0)) &&
        ctx->cbs.basic.enabled_h)
    {
        rc = ctx->cbs.basic.enabled_h(ctx->app_ctx);
        if (!rc)
            ctx->app_enabled = 1;
    }

    return rc;
}

/**
 * Function name:  cdc_uninit_ecm
 * Description: ECM FD uninitialization - release string descriptor
 * Parameters:
 *     @context: (IN) ecm context  
 * Return value: 0 for success, or an error code
 * Scope: local
 * **/
static jresult_t cdc_uninit_ecm(context_t context)
{
    ecm_ctx_t *ecm_ctx = (ecm_ctx_t *)context;
    cdc_ctx_t *cdc_ctx = (cdc_ctx_t *)ecm_ctx;
    void *core_ctx = cdc_ctx->core_ctx; 
    
    core_release_string_descriptor(core_ctx,ecm_ctx->iMACAddress); 
    ecm_ctx->iMACAddress = 0;
    
    return cdc_uninit(context);
}

/**
 * Function name:  cdc_init_ecm
 * Description: ECM FD initialization
 * Parameters: 
 *     @core_ctx: (IN) USB CORE context
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_init_ecm(void *core_ctx)
{
    ecm_ctx_t *ecm_ctx = NULL;
    cdc_ctx_t *ctx = NULL;
    cdc_desc_init_t desc_info;
    jresult_t rc = 0;
    cdc_callbacks_t cbs;
    DECLARE_FNAME("cdc_init_ecm");

    /* Create our context */
    ecm_ctx = (ecm_ctx_t *)jmalloc(sizeof(ecm_ctx_t), M_ZERO);
    ctx = (cdc_ctx_t *)ecm_ctx;
    if (!ctx)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: failed to allocate ctx\n", fname));
        rc = JENOMEM;
        goto Error;
    }
    
    ctx->core_ctx = core_ctx;
    
    /* Call application init function */
    ctx->info.basic.interface_number = UWD_UNDEFINED_INTERFACE;
    rc = ecm_init(ctx, &ctx->cbs.ecm, &ctx->info.ecm,
        &ctx->app_ctx);
    if (rc)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: application failed to initialize\n",
            fname));
        goto Error;
    }

    cbs = ctx->cbs.basic;
    KASSERT((cbs.write_multi_complete_h && !cbs.write_complete_h) || 
        (!cbs.write_multi_complete_h && cbs.write_complete_h),
        ("using both single and multiple requests is not supported\n"));

    j_memset(&desc_info, 0, sizeof(cdc_desc_init_t));
    desc_info.cc_handler = ecm_handle_request;
    desc_info.cc_ntf_supported = TRUE;
    desc_info.dc_supported = TRUE;
    /* HSU addition: API to disable alt. settings */
    desc_info.dc_empty_alt_0 = 
      ((ctx->info.ecm.basic.flags & CDC_INIT_NO_ALT_SETTINGS) ? FALSE : TRUE);

    if (ctx->info.ecm.basic.flags & CDC_INIT_VENDOR_SPECIFIC_CLASS)
    {
        desc_info.cci_subclass = ctx->info.basic.cci_subclass;
        desc_info.cci_protocol = ctx->info.basic.cci_protocol;
    }
    else
    {
        desc_info.cci_subclass = USB_CDC_SUBCLASS_ECM;
        desc_info.cci_protocol = USB_CDC_CTRL_PROTO_NONE;
    }

    if (!(ctx->info.ecm.basic.flags & CDC_INIT_NO_CLASS_DESC))
        desc_info.get_func_headers = ecm_get_functional_headers;

    if (ctx->info.ecm.basic.flags & CDC_INIT_SINGLE_INTERFACE)
        desc_info.is_single_iface = TRUE;
    else 
        desc_info.is_single_iface = FALSE;

    desc_info.interface_number = ctx->info.basic.interface_number;
  
    /* Create descriptors */
    rc = cdc_alloc_descriptors(ctx, &desc_info);
    if (rc)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: failed to allocate descriptors.\n", fname));
        goto Error;
    }

    /* Override enable/disable/uninit default handlers */
    ctx->ops.enable = ecm_enable;
    ctx->ops.disable = ecm_disable;
    ctx->ops.uninit = cdc_uninit_ecm;
    
    /* Initialize CDC functionality */
    rc = cdc_init(ctx);
    if (rc)
    {
        DBG_E(DSLAVE_CDC_ECM, ("%s: failed on cdc_init().\n", fname));
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

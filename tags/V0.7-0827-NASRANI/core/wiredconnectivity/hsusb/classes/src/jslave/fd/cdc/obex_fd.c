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

/**
 * Function name:  obex_handle_request
 * Description:  OBEX model control interface request handler
 * Parameters: 
 *     @context: (IN) CDC context
 *     @buffer: (IN) USB SETUP packet
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t obex_handle_request(void *context, void *buffer)
{
    /* HSU addition */
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    struct usb_ctrlrequest* req = (struct usb_ctrlrequest*)buffer;
    /* End of HSU addition */
    DECLARE_FNAME("obex_handle_request");
    
    DBG_W(DSLAVE_CDC_OBEX, ("%s: called with request %u\n", fname, 
        ((struct usb_ctrlrequest *)buffer)->bRequest));

    /* HSU addition */
    if (ctx->cbs.obex.request_handler_cb)
    {
      return ctx->cbs.obex.request_handler_cb(ctx->app_ctx, req->bRequest, req->wValue, 
      req->wIndex, req->wLength);
    }
    else
    {
      DBG_W(DSLAVE_CDC_OBEX, ("%s: warning: no class specific requests are "
        "defined for obex!\n", fname));
    }
    /* End of HSU addition*/
    return JENOTSUP;
}

/*====================
 *  Initialization
 *===================*/

/**
 * Function name:  obex_get_functional_headers
 * Description:  this function is called by cdc_alloc_descriptors() to create
 *               the functional descriptors for OBEX. It should:
 *               1. Return the total size required for the descriptors when
 *                  buffer is NULL
 *               2. Fill the buffer with descriptors when buffer is valid
 * Parameters: 
 *     @ctx: (IN) CDC context
 *     @buffer: (OUT) buffer into which functional headers should be written
 *     @is_single_iface: (IN) indicates a single interface
 *     @interface_number: (IN) the max interface number allocated
 *
 * Return value: total size, or 0 for failure
 * Scope: local
 **/
static juint32_t obex_get_functional_headers(cdc_ctx_t *ctx, void *buffer, 
    jbool_t is_single_iface, juint8_t interface_number)
{
    juint32_t size = 0;
    struct cdc_header_fdesc *hdr;
    struct wmcdc_obex_fdesc *obex;
    struct cdc_union_fdesc *un;

#if defined (FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND) || defined (T_UNDP)
    struct cdc_mdlm_fdesc *mdlm;
    struct cdc_mdlm_detail_fdesc *mdlm_detail;
    const juint8_t guid[16] = QCOM_16_BYTES_GUID;
    boolean is_mdlm_needed = FALSE;
#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND || T_UNDP */


    juint8_t avail_iface;
    juint8_t data_iface;

    DBG_X(DSLAVE_CDC_OBEX, ("obex_get_functional_headers: entered\n"));

    size = sizeof(struct cdc_header_fdesc) +
        sizeof(struct wmcdc_obex_fdesc) +
        sizeof(struct cdc_union_fdesc);

#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND

    size += (sizeof(struct cdc_mdlm_fdesc) + 
      sizeof(struct cdc_mdlm_detail_fdesc));
    is_mdlm_needed = TRUE;

#elif defined (T_UNDP)
  if (ctx->info.obex.basic.flags & CDC_INIT_BI_DIR_NMEA)
  {
    size += (sizeof(struct cdc_mdlm_fdesc) + 
      sizeof(struct cdc_mdlm_detail_fdesc));
    is_mdlm_needed = TRUE;
  }
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

        obex = (struct wmcdc_obex_fdesc*)(hdr+1);
        obex->bLength           = sizeof(struct wmcdc_obex_fdesc);
        obex->bType             = USB_DT_CS_INTERFACE;
        obex->bSubtype          = USB_CDC_SUBTYPE_OBEX;
        ua_htole16(&obex->bcdVersion, 0x0100);
        
        un = (struct cdc_union_fdesc*)(obex+1);
        un->bLength             = sizeof(struct cdc_union_fdesc);
        un->bType               = USB_DT_CS_INTERFACE;
        un->bSubtype            = USB_CDC_SUBTYPE_UNION;
        un->bMasterIface        = avail_iface;
        un->bSlaveIface[0]      = data_iface;
#if defined (FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND) || defined (T_UNDP)
        if (is_mdlm_needed)
        {
          mdlm = (struct cdc_mdlm_fdesc*)(un+1);
          mdlm->bLength = sizeof(struct cdc_mdlm_fdesc);
          mdlm->bType = USB_DT_CS_INTERFACE;
          mdlm->bSubtype = USB_CDC_SUBTYPE_MDLM;
          ua_htole16(&mdlm->bcdVersion, 0x0120);
          memcpy((juint8_t*)(&(mdlm->bGUID)), guid, sizeof(mdlm->bGUID));

          mdlm_detail = (struct cdc_mdlm_detail_fdesc*)(mdlm+1);
          mdlm_detail->bmDataCapabilities = 0;
          mdlm_detail->bLength = sizeof(struct cdc_mdlm_detail_fdesc);
          mdlm_detail->bType = USB_DT_CS_INTERFACE;
          mdlm_detail->bSubtype = USB_CDC_SUBTYPE_MDLM_DETAIL;
          mdlm_detail->bGUIDType = USB_CDC_MDLM_DETAIL_GUID_DISCRIMINATOR;
          mdlm_detail->bmCtrlCapabilities = 
            USB_CDC_MDLM_DETAIL_CONTROL_CAPS_VENDOR_SPECIFIC;

#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND
          mdlm_detail->bmDataCapabilities |= 
            USB_CDC_MDLM_DETAIL_DATA_CAPS_BYTE_PADDING;

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */
#ifdef T_UNDP
          if (ctx->info.obex.basic.flags & CDC_INIT_BI_DIR_NMEA)
          {
            mdlm_detail->bmDataCapabilities |= 
              USB_CDC_MDLM_DETAIL_BI_DIR_NMEA;
          }
#endif /* T_UNDP */
        } /* is_mdlm_needed */
#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND || T_UNDP */
    }

    return size;
}

/**
 * Function name:  obex_disable
 * Description:  USB CORE 'disable' callback (overrides default in cdc_fd.c)
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: None
 * Scope: global
 **/
static void obex_disable(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    
    DBG_X(DSLAVE_CDC_OBEX, ("obex_disable: entered\n"));

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
 * Function name:  obex_enable
 * Description:  USB CORE 'enable' callback (overrides default in cdc_fd.c)
 * Parameters: 
 *     @context: (IN) CDC context
 *
 * Return value: 0 for success, or an error code
 * Scope: local
 **/
static jresult_t obex_enable(context_t context)
{
    cdc_ctx_t *ctx = (cdc_ctx_t*)context;
    jresult_t rc = 0;
    jint_t data_if_index = 1;

    DBG_X(DSLAVE_CDC_OBEX, ("obex_enable: entered\n"));

    if (!ctx)
        return JEINVAL;

    rc = cdc_enable(ctx);
    if (rc)
        return rc;

    if (ctx->info.basic.flags & CDC_INIT_SINGLE_INTERFACE)
        data_if_index = 0;

    /* first factor in the below expression is HSU addition:
       API to disable alt. settings */
    if (((ctx->info.basic.flags & CDC_INIT_NO_ALT_SETTINGS) ||
        (ctx->desc->interfaces[data_if_index].alt_if_curr != 0)) &&
        ctx->cbs.basic.enabled_h)
    {
        rc = ctx->cbs.basic.enabled_h(ctx->app_ctx);
        if (!rc)
            ctx->app_enabled = 1;
    }

    return rc;
}

/**
 * Function name:  cdc_init_obex
 * Description:  OBEX FD initialization
 * Parameters: 
 *     @core_ctx: (IN) USB CORE context
 *
 * Return value: 0 for success, or an error code
 * Scope: global
 **/
jresult_t cdc_init_obex(void *core_ctx)
{
    cdc_ctx_t *ctx = NULL;
    cdc_desc_init_t desc_info;
    jresult_t rc = 0;
    cdc_callbacks_t cbs;
    DECLARE_FNAME("cdc_init_obex");

    /* Create our context */
    ctx = jmalloc(sizeof(cdc_ctx_t), M_ZERO);
    if (!ctx)
    {
        DBG_E(DSLAVE_CDC_OBEX, ("%s: failed to allocate ctx\n", fname));
        rc = JENOMEM;
        goto Error;
    }
    
    ctx->core_ctx = core_ctx;

    /* Call application init function */
    ctx->info.basic.interface_number = UWD_UNDEFINED_INTERFACE;
    rc = obex_init(ctx, &ctx->cbs.obex, &ctx->info.obex,
        &ctx->app_ctx);
    if (rc)
    {
        DBG_E(DSLAVE_CDC_OBEX, ("%s: application failed to initialize\n",
            fname));
        goto Error;
    }
    
    cbs = ctx->cbs.basic;

    /* HSU Fix: Resolve compilation warnings about 'cbs' not being used after 
    ** being assigned in nandprg\hostdl builds, where KASSERT is mapped 
    ** to nothing.
    */
    HSU_USE_PARAM(cbs);
    /* HSU Fix end */

    KASSERT((cbs.write_multi_complete_h && !cbs.write_complete_h) || 
        (!cbs.write_multi_complete_h && cbs.write_complete_h),
        ("using both single and multiple requests is not supported\n"));

    j_memset(&desc_info, 0, sizeof(cdc_desc_init_t));
    desc_info.cc_handler = obex_handle_request;
    desc_info.cc_ntf_supported = FALSE;
    desc_info.dc_supported = TRUE;
    /* HSU addition: API to disable alt. settings */
    desc_info.dc_empty_alt_0 = 
      ((ctx->info.obex.basic.flags & CDC_INIT_NO_ALT_SETTINGS) ? FALSE : TRUE);
    
    if (ctx->info.obex.basic.flags & CDC_INIT_VENDOR_SPECIFIC_CLASS)        
    {
        desc_info.cci_subclass = ctx->info.basic.cci_subclass;
        desc_info.cci_protocol = ctx->info.basic.cci_protocol;
    }
    else
    {
        desc_info.cci_subclass = USB_CDC_SUBCLASS_OBEX;
        desc_info.cci_protocol = USB_CDC_CTRL_PROTO_NONE;
    }

    if (!(ctx->info.obex.basic.flags & CDC_INIT_NO_CLASS_DESC))
        desc_info.get_func_headers = obex_get_functional_headers;

    if (ctx->info.obex.basic.flags & CDC_INIT_SINGLE_INTERFACE)
        desc_info.is_single_iface = TRUE;
    else 
        desc_info.is_single_iface = FALSE;

    desc_info.interface_number = ctx->info.basic.interface_number;
    
   /* Create descriptors */
    rc = cdc_alloc_descriptors(ctx, &desc_info);
    if (rc)
    {
        DBG_E(DSLAVE_CDC_OBEX, ("%s: failed to allocate descriptors.\n",
            fname));
        goto Error;
    }

    /* Override enable/disable default handlers */
    ctx->ops.enable = obex_enable;
    ctx->ops.disable = obex_disable;
    
    /* Initialize CDC functionality */
    rc = cdc_init(ctx);
    if (rc)
    {
        DBG_E(DSLAVE_CDC_OBEX, ("%s: failed on cdc_init().\n", fname));
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


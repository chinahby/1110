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
#include <jusb_chp9.h>
#include "jusb_core.h"

/*HSU Addition*/
#include "hsu_vendor_specific.h"
/*End of HSU addition*/


/* String Descriptor indexes */
#define STRING_SUPPORTED_LANG  0
#define UNICODE_LANG_ID_ENG_US 0x0409

/* MS OS String descriptor */
#define MSOS_DESC_SIZE         18
#define MSOS_STRING_INDEX      0xEE
#define MSOS_VENDOR_CODE       0x01
#define MSOS_SIGNATURE         "M\0S\0F\0T\0""1\0""0\0""0\0"

#define REALLOC_CORE_REQUEST(core, new_size) \
{ \
    request_t *old_req = core->request; \
    \
    core->request = core_request_alloc(new_size, NULL); \
    if (!core->request) \
    { \
        DBG_E(DSLAVE_CORE, ("Failed allocate long request (%d bytes)\n", \
            new_size)); \
        core->request = old_req; \
        return JENOMEM; \
    } \
    \
    core->request->caller_private = old_req; \
}

#define CORE_REQUEST_RESTORE(core) \
do { \
    if (core->request->caller_private) \
    { \
        request_t *old_req; \
        old_req = (request_t *)core->request->caller_private; \
        core_request_free(core->request); \
        core->request = old_req; \
        core->request->caller_private = NULL; \
    } \
} while(0)
 
 
static __INLINE__ jint_t interface_number_to_index(core_t *core, 
    juint8_t number);
static jresult_t post_command_reply(core_t *core, juint32_t size, 
    juint32_t max_send_size, juint8_t ep0_req_tag);
#define send_zero_packet(core, ep0_tag) post_command_reply(core, 0, 1, ep0_tag)

/* Static USB chap9 descriptors */
static struct usb_device_descriptor
device_desc = {
/*  bLength             */ USB_DT_DEVICE_SIZE,
/*  bDescriptorType     */ USB_DT_DEVICE,
/*  bcdUSB              */ htole16(0x0200),
/*  bDeviceClass        */ 0,
/*  bDeviceSubClass     */ 0,
/*  bDeviceProtocol     */ 0,
/*  bMaxPacketSize0     */ 8,
/*  idVendor            */ 0,
/*  idProduct           */ 0,
/*  bcdDevice           */ 0,
/*  iManufacturer       */ 0,
/*  iProduct            */ 0,
/*  iSerialNumber       */ 0,
/*  bNumConfigurations  */ 0
};

#ifndef CONFIG_FULL_SPEED_ONLY
static struct usb_qualifier_descriptor
dev_qualifier = {
/*  bLength             */ USB_DT_QUALIFIER_SIZE,
/*  bDescriptorType     */ USB_DT_DEVICE_QUALIFIER,
/*  bcdUSB              */ htole16(0x0200),
/*  bDeviceClass        */ 0,
/*  bDeviceSubClass     */ 0,
/*  bDeviceProtocol     */ 0,
/*  bMaxPacketSize0     */ 8,
/*  bNumConfigurations  */ 0,
/*  bRESERVED           */ 0
};
#endif

static struct usb_config_descriptor
config_descriptor = {
/*  bLength             */ USB_DT_CONFIG_SIZE,
/*  bDescriptorType     */ USB_DT_CONFIG,
/*  wTotalLength        */ 0, /* Dynamic */
/*  bNumInterfaces      */ 0, /* Dynamic */
/*  bConfigurationValue */ 0, /* Dynamic */
/*  iConfiguration      */ 0, /* No string descriptors */
/*  bmAttributes        */ USB_CONFIG_ATT_ONE,
/*  bMaxPower           */ 50  
};

static struct usb_interface_descriptor if_descriptor = {
/*  bLength             */ USB_DT_INTERFACE_SIZE,
/*  bDescriptorType     */ USB_DT_INTERFACE,
/*  bInterfaceNumber    */ 0, /* Dynamic */
/*  bAlternateSetting   */ 0, /* Dynamic */
/*  bNumEndpoints       */ 0, /* Dynamic */
/*  bInterfaceClass     */ 0, /* Dynamic */
/*  bInterfaceSubClass  */ 0, /* Dynamic */
/*  bInterfaceProtocol  */ 0, /* Dynamic */
/*  iInterface          */ 0  /* Dynamic */ 
};

static struct usb_endpoint_descriptor ep_descriptor = {
/*  bLength             */ 0, /* Dynamic */
/*  bDescriptorType     */ USB_DT_ENDPOINT,
/*  bEndpointAddress    */ 0, /* Dynamic */
/*  bmAttributes        */ 0, /* Dynamic */
/*  wMaxPacketSize      */ 0, /* Dynamic */
/*  bInterval           */ 0  /* Dynamic */
#ifdef CONFIG_FD_AUDIO
/*  bRefresh            */ ,0 /* Dynamic */
/*  bSynchAddress       */ ,0 /* Dynamic */
#endif
};

#ifdef CONFIG_JOTG
static struct usb_otg_descriptor otg_descriptor = {
/* bLength              */ USB_DT_OTG_SIZE,
/* bDescriptorType      */ USB_DT_OTG,
/* bmAttributes         */ 0
};

#define IS_DEVICE_OTG() \
    (core->dcd_ops->dcd_is_otg && core->dcd_ops->dcd_is_otg(core->dcd_dev))

#endif

#if defined(CONFIG_JSLAVE_TEST_MODE)
static void ep0_test_mode_complete_callback(request_t *request)
{
    core_t *core = (core_t *)request->context;

    core->dcd_ops->dcd_set_test_mode(core->dcd_dev, request->test_mode);
}
#endif

static void ep0_vendor_out_data_complete_callback(request_t *request)
{
    core_t *core = (core_t *)request->context;
    juint8_t ep0_req_tag = request->ep0_req_tag;
    jresult_t rc;

    DBG_V(DSLAVE_CORE, ("CH9: Handle completion of vendor specific out "
        "request\n"));

    if (request->status !=  REQUEST_COMPLETED)
    {
        DBG_E(DSLAVE_CORE, ("CH9: Failed to receive data stage for EP0\n"));
        goto Error;
    }

    rc = core->device_param.vendor_cmd_out_data_stage_cb(request->buffer.vaddr,
        request->bytes_transferred);
    CORE_REQUEST_RESTORE(core);
    if (rc)
        goto Error;

    core->request->status = REQUEST_READY;
    rc = send_zero_packet(core, ep0_req_tag);
    if (rc)
        goto Error;

    return;

Error:
    core_stall_pipe(core, NULL, 1);
}

/*HSU addition*/
static void ep0_reserved_out_data_complete_callback(request_t *request)
{
    core_t *core = (core_t *)request->context;
    juint8_t ep0_req_tag = request->ep0_req_tag;
    jresult_t rc;

    DBG_V(DSLAVE_CORE, ("CH9: Handle completion of reserved specific out "
        "request\n"));

    if (request->status !=  REQUEST_COMPLETED)
    {
        DBG_E(DSLAVE_CORE, ("CH9: Failed to receive data stage for EP0\n"));
        goto Error;
    }

    rc = core->device_param.reserved_cmd_out_data_stage_cb(request->buffer.vaddr,
        request->bytes_transferred);
    CORE_REQUEST_RESTORE(core);
    if (rc)
        goto Error;

    core->request->status = REQUEST_READY;
    rc = send_zero_packet(core, ep0_req_tag);
    if (rc)
        goto Error;

    return;

Error:
    core_stall_pipe(core, NULL, 1);
}
/*End of HSU addition*/
/**
 * Function name:  ep0_reply_complete_callback
 * Description: completion callback when response to endpoint 0 command is
 *              completed
 * Parameters: 
 *     @request: (IN) CORE request
 *
 * Return value: None
 * Scope: local
 **/
static void ep0_reply_complete_callback(request_t *request)
{
    core_t *core = (core_t *)request->context;

    if (core->request->transfer_size != core->request->bytes_transferred)
    {
        DBG_E(DSLAVE_CORE, ("EP0 command reply was not sent correctly %d "
            "[%d:%d]\n",
            request->status, request->bytes_transferred,
            request->transfer_size));
    }

    DBG_V(DSLAVE_CORE, ("Callback: EP0 Reply finished. %d of %d sent\n", 
        core->request->bytes_transferred, core->request->transfer_size));

    if (core->request->caller_private)
    {
        DBG_I(DSLAVE_CORE, ("ep0_reply_complete_callback: Restoring old "
            "request\n"));
        CORE_REQUEST_RESTORE(core);
    }

    core->request->status = REQUEST_READY;
}

/**
 * Function name:  core_add_msos_string_descriptor
 * Description:    Add a string descriptor to be included in the response to a 
 *                 GET_MS_DESCRIPTOR request
 * Parameters:
 *     @ctx:         (IN) CORE context
 *     @vendor_code: (IN) Vendor code to send in the response
 *
 * Return value:   None
 * Scope:          Global
 **/
void core_add_msos_string_descriptor(void *ctx, juint8_t vendor_code)
{
    core_t *core = (core_t *)ctx;

    core->msos_vendor_code = vendor_code;
    core->msos_vendor_code_set = 1;
}

/**
 * Function name:  core_add_string_descriptor
 * Description:    Add a string descriptor to be included in the response to a 
 *                 GET_DESCRIPTOR request
 * Parameters: 
 *     @ctx:    (IN) CORE context
 *     @string: (IN) The string to add
 *     @str_desc: (IN) The string description
 *
 * Return value:   The string index
 * Scope:          Global
 **/
juint8_t core_add_string_descriptor(void *ctx, char *string, hsu_str_idx_desc_type str_desc)
{
    core_t *core = (core_t *)ctx;
    juint8_t i;
    juint8_t  idx = 0;
    
    if (!string)
        return 0; /* This is a valid value specifying "no descriptor" */ 
    
    /*HSU Addition */
    /*Check if this string has a pre-defined index for it*/
    idx = hsu_vendor_specific_get_str_idx(str_desc);
    if (idx < MAX_STRING_DESCRIPTORS)
    {
      /* verify that the index is free*/
      if (core->string_descriptors[idx-1])
      {
        HSU_MSG_ERROR("core_add_string_descriptor: the given index is already occupied!"
                "(idx = %d)",idx,0,0);
      }
      else
      {
        core->string_descriptors[idx-1] = string;
        return idx;
      }
    }
    
    for (i = 0; i < MAX_STRING_DESCRIPTORS ; i++)
    {
      /* If the index is free and not saved for another string */
      if ((!core->string_descriptors[i]) && (!hsu_vendor_specific_is_idx_occupied(i+1)))
      {
        break;
      }
    }
        
    if (i == MAX_STRING_DESCRIPTORS)
    {
      HSU_MSG_HIGH("core_add_string_descriptor: descriptors array is full",0,0,0);
        return 0; /* descriptors array is full */
    }
    /*End of HSU addition*/
    core->string_descriptors[i] = string;
    return i+1;
}

/**
 * Function name:  core_release_string_descriptor 
 * Description:    Remove a string descriptor
 * Parameters: 
 *     @ctx:    (IN) CORE context
 *     @index_h (IN) Index handle
 * Return value:   None 
 * Scope:          Global
 **/
void core_release_string_descriptor(void *ctx, juint8_t index)
{
    core_t *core = (core_t *)ctx;
    
    if (!index || index > MAX_STRING_DESCRIPTORS)
        return;
    core->string_descriptors[index - 1] = NULL;
}

/**
 * Function name:  core_change_string_descriptor
 * Description:    Change a string descriptor
 * Parameters: 
 *     @ctx:    (IN) CORE context
 *     @id:     (IN) Index of the string to update
 *     @string: (IN) The string to update
 *
 * Return value: None
 * Scope:        Global
 **/
void core_change_string_descriptor(void *ctx, juint8_t id, char *string)
{
    core_t *core = (core_t *)ctx;
    core->string_descriptors[id - 1] = string;
}

/**
 * Function name:  get_string_descriptor
 * Description:    Return the string at index <id>
 * Parameters: 
 *     @core: (IN) CORE context
 *     @id:   (IN) index of the requested string
 *
 * Return value:   String descriptor
 * Scope:          Local
 **/
static char *get_string_descriptor(core_t *core, juint8_t id)
{
    if (!id  || id > MAX_STRING_DESCRIPTORS)   
        return NULL;

    return core->string_descriptors[id - 1];
}

/**
 * Function name:  post_command_reply
 * Description: Send a reply to an endpoint 0 command
 * Parameters: 
 *     @core: (IN) CORE context
 *     @size: (IN) reply size (data is already ready in core->request->buffer)
 *     @max_send_size: (IN) the wLength parameter in ctrl_request
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control 
 *                        request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t post_command_reply(core_t *core, juint32_t size, 
    juint32_t max_send_size, juint8_t ep0_req_tag)
{
    if (!core->request)
    {
        DBG_E(DSLAVE_CORE, ("Core request struct not ready\n"));
        return JEINVAL;
    }

    if (core->request->status != REQUEST_READY)
    {
        DBG_E(DSLAVE_CORE, ("Attempt to use a busy request\n"));
        return JEAGAIN;
    }

    if (core->ep0_req_tag != ep0_req_tag)
    {
        DBG_E(DSLAVE_CORE, ("Attempt to answer an expired EP0 request.\n"));
        return 0;
    }

    DBG_V(DSLAVE_CORE, ("CH9: EP0 post reply size %d (max %d)\n",
        size, max_send_size));

    /* If the host is expecting more data then we plan to send
     * then we should send a ZLP to tell him that we have sent
     * all we meant to */
    if (max_send_size > size)
        core->request->zero = 1;
    else
        core->request->zero = 0;

    core->request->context      = core;
    core->request->complete     = ep0_reply_complete_callback;
    core->request->transfer_size = MIN(size, max_send_size);

    /*  When the request is from Device to Host but with no data stage 
        (wLength = 0) we need to send the status stage in OUT direction */
    if (max_send_size)
        return core_send_data(core, NULL, core->request);
    else
        return core_read_data(core, NULL, core->request);
}

/**
 * Function name:  do_iface_ep_disable
 * Description: Disable all endpoint in a specific interface
 * Parameters: 
 *     @core: (IN) CORE context
 *     @if_desc: (IN) Interface context
 *
 * Return value: None
 * Scope: local
 **/
static void do_iface_ep_disable(core_t *core, alt_interface_desc_t *if_desc)
{
    jint_t i;
    for (i = 0; i < if_desc->pipe_count; i++)
    {
        DBG_V(DSLAVE_CORE, ("CORE: Disable pipe %x status %x\n",
            if_desc->pipes[i].address, if_desc->pipes[i].status));
        
        if (if_desc->pipes[i].status == PIPE_ENABLED ||
            if_desc->pipes[i].status == PIPE_STALLLED)
        {
            core->dcd_ops->dcd_disable_ep(core->dcd_dev, &if_desc->pipes[i]);
        }
    }
}

/**
 * Function name:  do_iface_ep_enable
 * Description: Enable all endpoints in a specific interface
 * Parameters: 
 *     @core: (IN) CORE context
 *     @if_desc: (IN) Interface context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t do_iface_ep_enable(core_t *core, alt_interface_desc_t *if_desc)
{
    jint_t i;
    jresult_t rc;
    
    DBG_V(DSLAVE_CORE, ("CH9: Enabling iface %p\n", if_desc));

    for (i = 0; i < if_desc->pipe_count; i++)
    {
        DBG_X(DSLAVE_CORE, ("CH9: Enabling pipe 0x%x\n", 
            if_desc->pipes[i].address));

        /* Set the max packet size based on speed */
        if_desc->pipes[i].max_pkt_size =
            (core->curr_speed == SPEED_HIGH ? 
                if_desc->pipes[i].max_pkt_size_high :
                if_desc->pipes[i].max_pkt_size_full);

        /* Ask DCD to start things up */
        rc = core->dcd_ops->dcd_enable_ep(core->dcd_dev, &if_desc->pipes[i]);
        if (rc)
            return rc;
    }

    DBG_V(DSLAVE_CORE, ("CH9: Enable ep done\n"));
    return 0;
}

/**
 * Function name:  do_ep_disable
 * Description: Disable all device endpoints
 * Parameters: 
 *     @core: (IN) CORE context
 *     @idx:  (IN) configuration value
 *
 * Return value: None
 * Scope: local
 **/
static void do_ep_disable(core_t *core, juint8_t idx)
{
    jint_t i;
    config_t *config = &core->configs[idx - 1];

    /* Disable endpoints of each interface */
    for (i = 0; i < config->interface_count; i++)
        do_iface_ep_disable(core, config->interfaces[i].if_curr);
}

/**
 * Function name:  do_ep_enable
 * Description: Enable all device endpoints
 * Parameters: 
 *     @core: (IN) CORE context
 *     @idx:  (IN) configuration value
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t do_ep_enable(core_t *core, juint8_t idx)
{
    jint_t i;
    jresult_t rc;
    config_t *config = &core->configs[idx - 1];

    DBG_V(DSLAVE_CORE, ("CH9: Starting ep enable\n"));

    /* Attempt to enable endpoints of each interface */
    for (i = 0; i < config->interface_count; i++)
    {
        DBG_X(DSLAVE_CORE, ("CH9: Enabling iface %d [%p]\n",
            i, config->interfaces[i].if_curr));
            
        /* For each interface enable all endpoints */
        rc = do_iface_ep_enable(core, config->interfaces[i].if_curr);
        if (rc)
            return rc;
    }
    return 0;
}

/**
 * Function name:  handle_set_interface
 * Description: Handle set interface request. Disable the interface including
 *              notifying the function driver, then enable with the new 
 *              alternate setting and notify the function driver
 * Parameters: 
 *     @core: (IN) CORE context
 *     @iface: (IN) interface number
 *     @altif: (IN) alternate setting number
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t handle_set_interface(core_t *core, juint8_t iface, juint8_t altif)
{
    fd_t *fd;
    jresult_t rc = 0;
    jint_t interface_index;
    config_t *config;

    DBG_V(DSLAVE_CORE, ("SET INTERFACE %d ALT %d\n", iface, altif));

    /* Check that core is configured */
    if (!core->curr_config)
    {
        DBG_W(DSLAVE_CORE, ("Attempt to set an iface when core "
            "not configured !\n"));
        return JEINVAL;
    }

    interface_index = interface_number_to_index(core, iface);
    config = &core->configs[core->curr_config - 1];

    /* Check if we support the interface */
    if (interface_index >= config->interface_count)
    {
        DBG_W(DSLAVE_CORE, ("Attempt to set an invalid interface %d\n",
            iface));
        return JEINVAL;
    }

    /* Check if we support the alt-interface */
    if (altif >= config->interfaces[interface_index].if_arr->alt_if_num)
    {
        DBG_W(DSLAVE_CORE, ("Attempt to set an invalid alt_iface %d:%d\n",
            iface, altif));
        return JEINVAL;
    }

    DBG_V(DSLAVE_CORE, ("Setting interface %d altestting %d\n",
        iface, altif));

    fd = config->interfaces[interface_index].fd;

    /* Disable FD */
    if (fd->state == FD_ENABLED)
        fd->fd_ops->disable(fd->context);
    fd->state = FD_DISABLED; 

    /* Disable EPs */
    do_iface_ep_disable(core, config->interfaces[interface_index].if_curr);
    
    /* Change core pointers */
    config->interfaces[interface_index].if_curr = 
        &config->interfaces[interface_index].
        if_arr->alt_ifs[altif];

    /* Set number of select alt iface */
    config->interfaces[interface_index].if_arr->alt_if_curr = altif;

    /* Enable EPs */
    rc = do_iface_ep_enable(core, config->interfaces[interface_index].if_curr);
    if (rc)
        goto error;

    /* Update current speed in FD's desc */
    fd->fd_desc->current_speed = core->curr_speed;
        
    /* Enable FD */
    rc = fd->fd_ops->enable(fd->context);
    if (rc)
        goto error;

    fd->state = FD_ENABLED;

    DBG_V(DSLAVE_CORE, ("Set interface %d altsetting %d complete (%d)\n",
        iface, altif, rc));

    return 0;

error:
    do_iface_ep_disable(core, config->interfaces[interface_index].if_curr);

    DBG_W(DSLAVE_CORE, ("Set interface %d alt %d encountered an error (%d)\n",
        iface, altif, rc));
    return rc;
}

/**
 * Function name:  chp9_handle_set_configuration
 * Description: Handle set configuration request. First un-configure the device
 *              (in case it is already configured) by disable all endpoints and
 *              function drivers. Then enable all the endpoints and function 
 *              drivers
 * Parameters: 
 *     @core: (IN) CORE context
 *     @config_idx: (IN) configuration value
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: global
 **/
jresult_t chp9_handle_set_configuration(core_t *core, juint8_t config_idx)
{
    fd_t *fd;
    jint_t i;
    jresult_t rc = 0;
    config_t *config;

    DBG_V(DSLAVE_CORE, ("SET CONFIG %d (%d)\n", config_idx, core->curr_config));

    rc = core->dcd_ops->dcd_set_config_state(core->dcd_dev, config_idx);
    if (rc)
        return rc;

    /* We can assume that FD is valid, else the command would not have
     * been processed in the first place */

    /* Check if we support the configuration */
    if (config_idx > core->config_count)
    {
        DBG_W(DSLAVE_CORE, ("Attempt to set an invalid configuration\n"));
        return JEINVAL;
    }

    if (core->curr_config)
    {
        /* change usb_state to address */
        core->usb_state = USB_ADDRESS;

        /* Disable all FDs */
        for (fd = core->configs[core->curr_config - 1].fd_list; fd;
            fd = fd->next)
        {
            if (fd->state == FD_ENABLED)
                fd->fd_ops->disable(fd->context);
            fd->state = FD_DISABLED;
        }

        /* Disable all EPs */
        do_ep_disable(core, core->curr_config);
    }


    /* if config_idx = 0, no configuration is set.
     * The USB devices should return to "Address" state */
    if (!config_idx)
    {
        /* Notify only in case the device was already configured */
        if (core->curr_config)
        {
            core->curr_config = 0;
            jnotify_os(NOTIFY_DEVICE_UNCONFIGURED, 
                j_device_get_bus(core->dcd_dev));
            
            /* Temp fix for B47218 */
            
            core->app_cbs.notify_device_configured(core->app_ctx, FALSE);
            
            /* Temp fix END */
        }
        return 0;
    }


    DBG_V(DSLAVE_CORE, ("Setting configuration %d speed %d\n",
        config_idx, core->curr_speed));

    config = &core->configs[config_idx - 1];

    /* Reset all alt_ifaces */
    for (i = 0; i < config->interface_count; i++)
    {
        /* Get pointer of active alt_iface and put it somewhere handly */
        config->interfaces[i].if_curr = &(config->interfaces[i].
            if_arr->alt_ifs[0]);

        DBG_V(DSLAVE_CORE, ("CH9: Setting iface %d [core %d] curr to %p\n",
            i, config->interfaces[i].fd_if_num, config->interfaces[i].if_curr));

        /* Set first interface to be alt-set 0 */
        config->interfaces[i].if_arr->alt_if_curr = 0;
    }
    
    rc = do_ep_enable(core, config_idx);
    if (rc)
        goto error;
    
    core->curr_config   = config_idx;

    /* Attempt to enable function drivers */
    for (fd = config->fd_list; fd; fd = fd->next)
    {
        /* Update current speed in FD's desc */
        fd->fd_desc->current_speed = core->curr_speed;
        
        rc = fd->fd_ops->enable(fd->context);
        if (rc)
            goto error;

        fd->state = FD_ENABLED;
    }

    core->usb_state     = USB_CONFIGURED;

    jnotify_os(NOTIFY_DEVICE_CONFIGURED, j_device_get_bus(core->dcd_dev));
/* Temp fix for B47218 */

    core->app_cbs.notify_device_configured(core->app_ctx, TRUE);

/* Temp fix END */

    DBG_V(DSLAVE_CORE, ("Set configuration complete (%d)\n", rc));
    return 0;

error:
    do_ep_disable(core, config_idx);

    for (fd = config->fd_list; fd; fd = fd->next)
    {
        if (fd->state == FD_ENABLED)
            fd->fd_ops->disable(fd->context);
        fd->state = FD_DISABLED;
    }

    core->curr_config = 0;

    DBG_W(DSLAVE_CORE, ("Set configuration encountered an error (%d)\n", rc));
    return rc;
}

/**
 * Function name:  send_descriptor_device
 * Description: Send device descriptor
 * Parameters: 
 *     @core: (IN) CORE context
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control 
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: local
 **/
static jresult_t send_descriptor_device(core_t *core, juint16_t wLength, 
    juint8_t ep0_req_tag)
{
    dev_info_t *dev_info = core->device_param.dev_info; 

    DBG_V(DSLAVE_CORE, ("CH9: Send DEVICE DESCRIPTOR:\n"));

    device_desc.iProduct = core->i_product;
    device_desc.iManufacturer = core->i_manufacturer;
    device_desc.iSerialNumber = core->i_serial_number;
    device_desc.idVendor        = htole16(dev_info->vendor);
    device_desc.idProduct       = htole16(dev_info->product);
    device_desc.bcdDevice       = htole16(dev_info->release);
    device_desc.bDeviceClass    = dev_info->dev_class;
    device_desc.bDeviceSubClass = dev_info->dev_sub_class;
    device_desc.bDeviceProtocol = dev_info->dev_protocol;
    device_desc.bMaxPacketSize0 = 
        core->dcd_ops->dcd_get_max_packet0(core->dcd_dev);
    device_desc.bNumConfigurations = core->config_count;
    /* HSU temp fix */
    device_desc.bcdUSB          = htole16(dev_info->bcdUSB);
    /* HSU temp fix */

    j_memcpy(core->request->buffer.vaddr, &device_desc, USB_DT_DEVICE_SIZE);
    return post_command_reply(core, USB_DT_DEVICE_SIZE, wLength, ep0_req_tag);
}

static juint16_t calc_config_desc_size(core_t *core, juint8_t config_index)
{
    interface_desc_t    *if_desc;
    alt_interface_desc_t *alt_desc;
    pipe_desc_t         *ep_desc;
    juint8_t curr_if = 0;
    juint8_t curr_alt = 0;
    jint_t curr_ep = 0;
    jint_t curr_fd_if = 0;
    fd_t *curr_fd = NULL;
    juint16_t size;
    config_t *config = &core->configs[config_index];

    size = USB_DT_CONFIG_SIZE;

#ifdef CONFIG_JOTG
    if (IS_DEVICE_OTG())
        size += USB_DT_OTG_SIZE;
#endif
    for (curr_if = 0, curr_fd_if = 0, curr_fd = config->fd_list;
        curr_if < config->interface_count; curr_if++, curr_fd_if++)
    {
        if (curr_fd_if > curr_fd->fd_desc->interface_count - 1)
        {
            curr_fd = curr_fd->next;
            curr_fd_if = 0;
        }
        if_desc = config->interfaces[curr_if].if_arr;
        
        if (!curr_fd_if && curr_fd->fd_desc->iad)
            size += USB_DT_INTERFACE_ASSOC_SIZE;

        for (curr_alt = 0; curr_alt < if_desc->alt_if_num; curr_alt++)
        {
            alt_desc = &if_desc->alt_ifs[curr_alt];

            size += USB_DT_INTERFACE_SIZE;
            
            /* Check if there is vendor/class specific descriptor data to add */
            if (!curr_alt && if_desc->extra_descriptor_size > 0)
                size += if_desc->extra_descriptor_size;

            /* Check if there is vendor/class specific descriptor data to add */
            if (alt_desc->extra_descriptor_size > 0)
                size += alt_desc->extra_descriptor_size;

            /* Add all EPs that belong to this interface */
            for (curr_ep = 0; curr_ep < alt_desc->pipe_count; curr_ep++)
            {
                ep_desc = &alt_desc->pipes[curr_ep];

#ifdef CONFIG_FD_AUDIO
                size += (if_desc->if_class == USB_CLASS_AUDIO) ?  
                    USB_DT_AUDIO_ENDPOINT_SIZE : USB_DT_ENDPOINT_SIZE;
#else
                size += USB_DT_ENDPOINT_SIZE;
#endif
                if (ep_desc->extra_descriptor_size)
                    size += ep_desc->extra_descriptor_size;
            }
        }
    }

    return size;
}

#define ADD_TO_BUF(from, size) \
    do { \
        if (curr_pos + size > desc_size) \
            return JE2BIG; \
        j_memcpy(buffer + curr_pos, from, size); \
        curr_pos += size; \
    } while (0)

/**
 * Function name:  send_descriptor_config
 * Description: Send configuration descriptor, including all extra descriptors
 * Parameters: 
 *     @core: (IN) CORE context
 *     @index: (IN) configuration value
 *     @speed: (IN) device speed
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: local
 **/
static jresult_t send_descriptor_config(core_t *core, juint8_t index, 
    juint8_t speed, juint16_t wLength, juint8_t ep0_req_tag)
{
    jint_t curr_pos = 0;
    juint8_t curr_if = 0;
    juint8_t curr_alt = 0;
    jint_t curr_ep = 0;
    jint_t curr_fd_if = 0;
    fd_t *curr_fd = NULL;

    /* We rely on descriptor information passed from FD */
    interface_desc_t    *if_desc;
    alt_interface_desc_t *alt_desc;
    pipe_desc_t         *ep_desc;
    juint8_t *buffer;
    /* HSU temp fix - remove #ifdef CONFIG_JOTG, since dev_info is used in nandprg as well */
/*#ifdef CONFIG_JOTG*/
    dev_info_t *dev_info = core->device_param.dev_info; 
/*#endif*/
    /* end of HSU temp fix */
    juint16_t desc_size;
    config_param_t *config_param;

    DBG_V(DSLAVE_CORE, ("CH9: Send CONFIG DESCRIPTOR: %d\n", index));

    /* Verify that the requested configuration is valid */
    if (index >= core->config_count)
    {
        DBG_E(DSLAVE_CORE, ("Attempt to get an invalid configuration (%d, %d)\n",
            index, core->config_count));
        return JEINVAL;
    }

    desc_size = calc_config_desc_size(core, index);
    config_param = &core->device_param.config_param[index];

    if (desc_size > MAX_DESCRIPTOR_SIZE)
    {
        DBG_I(DSLAVE_CORE, ("send_descriptor_config: Config descriptor too "
            "long - allocating new request\n"));

        REALLOC_CORE_REQUEST(core, desc_size);
    }

    buffer = (juint8_t *)core->request->buffer.vaddr;

    /* Set the config descriptor*/
    config_descriptor.bNumInterfaces = core->configs[index].interface_count;
    config_descriptor.bConfigurationValue = index + 1;

    /* iConfiguration */
    config_descriptor.iConfiguration = core->configs[index].i_config;

    /* HSU Addition*/
    /* Set the configuration descriptor */
    if (config_descriptor.iConfiguration)
    {
        core_change_string_descriptor(core, config_descriptor.iConfiguration, 
         dev_info->configuration);
    }
    else
    {
    config_descriptor.iConfiguration = core_add_string_descriptor(core,
         dev_info->configuration, HSU_STR_IDX_CONFIGURATION);
    }
    /* End of HSU addition*/


    /* Self Powered ? */
    if (config_param->self_powered)
        config_descriptor.bmAttributes |= USB_CONFIG_ATT_SELFPOWER;

    /* Supports remote wake-up ? */
    if (config_param->remote_wakeup)
        config_descriptor.bmAttributes |= USB_CONFIG_ATT_WAKEUP;
    
    /* Power consumption */
    config_descriptor.bMaxPower = config_param->max_power;

    curr_pos += USB_DT_CONFIG_SIZE;

#ifdef CONFIG_JOTG
    if (IS_DEVICE_OTG() && (dev_info->srp_support || dev_info->hnp_support))
    {
        if (dev_info->srp_support)
            otg_descriptor.bmAttributes |= USB_OTG_SRP;
        if (dev_info->hnp_support)
            otg_descriptor.bmAttributes |= USB_OTG_HNP;
        ADD_TO_BUF(&otg_descriptor, USB_DT_OTG_SIZE);
    }
#endif

    /* Add all interfaces */
    for (curr_if = 0, curr_fd_if = 0, curr_fd = core->configs[index].fd_list;
        curr_if < core->configs[index].interface_count;
        curr_if++, curr_fd_if++)
    {
        if (curr_fd_if > curr_fd->fd_desc->interface_count - 1)
        {
            curr_fd = curr_fd->next;
            curr_fd_if = 0;
        }
        if_desc = core->configs[index].interfaces[curr_if].if_arr;
        
        if (!curr_fd_if && curr_fd->fd_desc->iad)
        {
            DBG_X(DSLAVE_CORE, ("Adding IAD for if %d\n", curr_if));
            ADD_TO_BUF(curr_fd->fd_desc->iad, USB_DT_INTERFACE_ASSOC_SIZE);
        }

        for (curr_alt = 0; curr_alt < if_desc->alt_if_num; curr_alt++)
        {
            alt_desc = &if_desc->alt_ifs[curr_alt];

            /* USB interface numbers will correspond with Core's
             * internal interfaces[] */
            if_descriptor.bInterfaceNumber      = 
                curr_fd->fd_desc->interfaces[curr_fd_if].number;
            if_descriptor.bAlternateSetting     = curr_alt;
            if_descriptor.bNumEndpoints         = alt_desc->pipe_count;
            if_descriptor.bInterfaceClass       = if_desc->if_class;
            if_descriptor.bInterfaceSubClass    = if_desc->if_subclass;
            if_descriptor.bInterfaceProtocol    = if_desc->if_protocol;
            if_descriptor.iInterface            = if_desc->if_string;

            ADD_TO_BUF(&if_descriptor, USB_DT_INTERFACE_SIZE);
            
            /* Check if there is vendor/class specific descriptor data to add */
            if (!curr_alt && if_desc->extra_descriptor_size > 0)
            {
                ADD_TO_BUF(if_desc->extra_descriptor,
                    if_desc->extra_descriptor_size);
            }

            /* Check if there is vendor/class specific descriptor data to add */
            if (alt_desc->extra_descriptor_size > 0)
            {
                ADD_TO_BUF(alt_desc->extra_descriptor,
                    alt_desc->extra_descriptor_size);
            }

            /* Add all EPs that belong to this interface */
            for (curr_ep = 0; curr_ep < alt_desc->pipe_count; curr_ep++)
            {
                ep_desc = &alt_desc->pipes[curr_ep];

                if (ep_desc->direction == DIRECTION_IN)
                    ep_descriptor.bEndpointAddress = USB_DIR_IN;
                else
                    ep_descriptor.bEndpointAddress = USB_DIR_OUT;

                /* EP address is known only to core */
                ep_descriptor.bEndpointAddress |= ep_desc->address;

                ep_descriptor.bmAttributes = (juint8_t)ep_desc->type;
                ep_descriptor.bInterval = ep_desc->rate_u.nak_rate;

                if (ep_desc->type == PIPE_ISOC)
                {
                    ep_descriptor.bmAttributes |= ep_desc->sync_type  << 2;
                    ep_descriptor.bmAttributes |= ep_desc->usage_type << 4;
                }

                ep_descriptor.wMaxPacketSize = htole16(
                    MIN(MAX_PACKET_SIZES[ep_desc->type][speed - 1], 
                    (speed == SPEED_HIGH ? 
                    ep_desc->max_pkt_size_high : ep_desc->max_pkt_size_full)));

                if (speed == SPEED_HIGH && 
                    (ep_desc->type == PIPE_ISOC || ep_desc->type == PIPE_INTR))
                {
                    ep_descriptor.wMaxPacketSize |= htole16( 
                        ep_desc->transaction_per_microframe << 11);

                    if (ep_desc->poll_interval_high)
                        ep_descriptor.bInterval = ep_desc->poll_interval_high;
                }

#ifdef CONFIG_FD_AUDIO
                ep_descriptor.bLength = (if_desc->if_class == USB_CLASS_AUDIO) ?
                    USB_DT_AUDIO_ENDPOINT_SIZE : USB_DT_ENDPOINT_SIZE;
#else
                ep_descriptor.bLength = USB_DT_ENDPOINT_SIZE;
#endif
                ADD_TO_BUF(&ep_descriptor, ep_descriptor.bLength);

                if (ep_desc->extra_descriptor_size)
                {
                    DBG_X(DSLAVE_CORE, ("CORE: Going to add EP extra "
                        "descriptor. Size = %d Addr = 0x%p\n", 
                        ep_desc->extra_descriptor_size, 
                        ep_desc->extra_descriptor));
                    ADD_TO_BUF(ep_desc->extra_descriptor, 
                        ep_desc->extra_descriptor_size);
                }
            }

        }
    }

    /* Now copy the config descriptor */
    config_descriptor.wTotalLength = htole16(curr_pos);
    j_memcpy(buffer, &config_descriptor, USB_DT_CONFIG_SIZE);

    return post_command_reply(core, curr_pos, wLength, ep0_req_tag);
}
#undef ADD_TO_BUF

/**
 * Function name:  send_string
 * Description: Send string to the host
 * Parameters: 
 *     @core: (IN) CORE context
 *     @str: (IN) the string to send
 *     @wLength: (IN) maximum length of the string
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: local
 **/
#define MAX_STRING_LEN 126
static jresult_t send_string(core_t *core, char *str, juint16_t wLength, 
    juint8_t ep0_req_tag)
{
    juint8_t *buffer;
    juint8_t str_len, act_len = 0;

    if (!str)
        return JEINVAL;

    buffer = (juint8_t *)core->request->buffer.vaddr;

    str_len     = MIN(j_strlen(str), MAX_STRING_LEN);
    buffer[0]   = (str_len + 1) * 2;
    buffer[1]   = USB_DT_STRING;
    j_memset(buffer + 2, 0, 2 * str_len);        
    
    while (act_len < str_len) 
    {
        buffer[2 + act_len * 2] = str[act_len];
        act_len++;
    }

    return post_command_reply(core, buffer[0], wLength, ep0_req_tag);
}

/**
 * Function name:  send_descriptor_string
 * Description: Send string descriptor
 * Parameters: 
 *     @core: (IN) CORE context
 *     @index: (IN) string index
 *     @lang_index: (IN) language id
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: local
 **/
static jresult_t send_descriptor_string(core_t *core, juint8_t index, 
    juint16_t lang_index, juint16_t wLength, juint8_t ep0_req_tag)
{
    juint8_t     *buffer;
    char        *string;

    DBG_V(DSLAVE_CORE, ("CH9: Send STRING DESCRIPTOR: %d\n", index));
    
    buffer = (juint8_t *)core->request->buffer.vaddr;

    /* Send the supportted language id */
    if (index == STRING_SUPPORTED_LANG)
    {
        buffer[0] = 4;
        buffer[1] = USB_DT_STRING;
        buffer[2] = UNICODE_LANG_ID_ENG_US & 0x00ff;
        buffer[3] = UNICODE_LANG_ID_ENG_US >> 8;
        return post_command_reply(core, 4, wLength, ep0_req_tag);
    }

    /* handling Microsoft OS String Descriptor */
    if (index == MSOS_STRING_INDEX && core->msos_vendor_code_set) 
    {
        DBG_I(DSLAVE_CORE, ("CH9: Send MS OS DESCRIPTOR: %d \n", index));

        buffer[0] = MSOS_DESC_SIZE;
        buffer[1] = USB_DT_STRING;
        j_memcpy(&buffer[2], MSOS_SIGNATURE, 14);
        buffer[16] = core->msos_vendor_code;
        buffer[17] = 0x00;  /* Padding */
        return post_command_reply(core, MSOS_DESC_SIZE, wLength, ep0_req_tag);
    }

    string = get_string_descriptor(core, index);
    if (!string && core->device_param.vendor_get_string_cb)
        string = core->device_param.vendor_get_string_cb(index);

    if (string)
        return send_string(core, string, wLength, ep0_req_tag);
    
    DBG_W(DSLAVE_CORE, ("Core: Uknown string descriptor %d\n", index));
    return JEINVAL;
}

#ifndef CONFIG_FULL_SPEED_ONLY
/**
 * Function name:  send_descriptor_dev_qual
 * Description: Send device qualifier descriptor
 * Parameters: 
 *     @core: (IN) CORE context
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: local
 **/
static jresult_t send_descriptor_dev_qual(core_t *core, juint16_t wLength,
    juint8_t ep0_req_tag)
{
    dev_info_t *dev_info = core->device_param.dev_info;

    DBG_V(DSLAVE_CORE, ("CH9: Send DEV QUALIFIER DESCRIPTOR:\n"));

    dev_qualifier.bDeviceClass    = dev_info->dev_class;
    dev_qualifier.bDeviceSubClass = dev_info->dev_sub_class;
    dev_qualifier.bDeviceProtocol = dev_info->dev_protocol;
    dev_qualifier.bMaxPacketSize0 = 
        core->dcd_ops->dcd_get_max_packet0(core->dcd_dev);
    dev_qualifier.bNumConfigurations = core->config_count;
    /* HSU temp fix */
    dev_qualifier.bcdUSB          = htole16(dev_info->bcdUSB);
    /* HSU temp fix */

    j_memcpy(core->request->buffer.vaddr, &dev_qualifier, 
        USB_DT_QUALIFIER_SIZE);

    return post_command_reply(core, USB_DT_QUALIFIER_SIZE, wLength, 
        ep0_req_tag);
}

/**
 * Function name:  send_descriptor_other_speed
 * Description: Send other speed descriptor
 * Parameters: 
 *     @core: (IN) CORE context
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: local
 **/
static jresult_t send_descriptor_other_speed(core_t *core, juint16_t wLength, 
    juint8_t ep0_req_tag)
{
    jresult_t rc;

    /* We support only two configs now, Full speed and High speed */
    DBG_V(DSLAVE_CORE, ("CH9: Send OTHER SPEED CONFIG DESCRIPTOR:\n"));

    /* Check if we support High speed */
    if (core->max_speed != SPEED_HIGH)
    {
        DBG_E(DSLAVE_CORE, ("We have no other speed config "
            "(Full speed device)\n"));
        return JEINVAL;
    }

    /* The "other speed" configuration descriptor looks almost like a
     * regular config descriptor. */
    config_descriptor.bDescriptorType = USB_DT_OTHER_SPEED_CONFIG;

    rc = send_descriptor_config(core, 0, (core->curr_speed == SPEED_HIGH ? 
        SPEED_FULL : SPEED_HIGH), wLength, ep0_req_tag);
    
    config_descriptor.bDescriptorType = USB_DT_CONFIG;
    return rc;
}
#endif

#ifdef CONFIG_JOTG
/**
 * Function name:  send_descriptor_otg
 * Description: Send OTG descriptor
 * Parameters: 
 *     @core: (IN) CORE context
 *     @wLength: (IN) maximum length of the descriptor
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: local
 **/
static jresult_t send_descriptor_otg(core_t *core, juint16_t wLength, 
    juint8_t ep0_req_tag)
{
    DBG_V(DSLAVE_CORE, ("CH9: Send OTG DESCRIPTOR:\n"));

    j_memcpy(core->request->buffer.vaddr, &otg_descriptor, USB_DT_OTG_SIZE);
    return post_command_reply(core, USB_DT_OTG_SIZE, wLength, ep0_req_tag);
}
#endif

/* Device status flags */
#define UDS_SELF_POWERED                0x0001
#define UDS_REMOTE_WAKEUP               0x0002

#ifdef CONFIG_REMOTE_WAKEUP_TEST
static void test_remote_wakeup(void *ctx)
{
    core_t *core = (core_t *)ctx;

    DBG_E(DSLAVE_CORE, ("test_remote_wakeup: wake the host\n"));

    jslave_wakeup_host(core->core_number);
}
#endif

/**
 * Function name:  handle_get_status
 * Description: Handle get status request
 * Parameters: 
 *     @core: (IN) CORE context
 *     @bmRequestType: (IN) request type
 *     @wIndex: (IN) request index (depends on bmRequestType)
 *     @wLength: (IN) maximum length of the reply
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: local
 **/
static jresult_t handle_get_status(core_t *core, juint8_t bmRequestType,
    juint16_t wIndex, juint16_t wLength, juint8_t ep0_req_tag)
{
    juint8_t buffer[USB_GET_STATUS_SIZE] = {0};
    dev_info_t *dev_info = core->device_param.dev_info; 
    jresult_t rc = 0;
    
    DBG_V(DSLAVE_CORE, ("CH9: Send GET STATUS\n"));

    switch (bmRequestType & USB_RECIP_MASK)
    {
    case USB_RECIP_DEVICE:

        /* Self Powered ? */
        if (dev_info->self_powered_func)
            dev_info->self_powered = dev_info->self_powered_func() ? 1 : 0;

        if (dev_info->self_powered)
            buffer[0] |= UDS_SELF_POWERED;

        /* Is remote wakeup currently enabled ? */
        if (core->remote_wakeup_enabled)        
            buffer[0] |= UDS_REMOTE_WAKEUP;        

        break;  
        
    case USB_RECIP_INTERFACE:
        /* We must be configured */
        if (core->usb_state != USB_CONFIGURED)
            return JEINVAL;

        /* returns two bytes of zero go figure... */
        break;
        
    case USB_RECIP_ENDPOINT:
        /* We must be configured */
        if (core->usb_state != USB_CONFIGURED)
            return JEINVAL;

        rc = core->dcd_ops->dcd_get_ep_status(core->dcd_dev, buffer, 
            (juint8_t)wIndex);
        
        if (rc)
            return rc;

        break;

    default:
        return JEINVAL;  
    }

    j_memcpy(core->request->buffer.vaddr, buffer, USB_GET_STATUS_SIZE);

    return post_command_reply(core, USB_DT_DEVICE_SIZE, wLength, ep0_req_tag);
}

static jresult_t handle_feature(core_t *core, juint8_t bmRequestType,
    juint16_t feature, juint16_t index, juint8_t set, juint8_t ep0_req_tag)
{
    jresult_t rc = 0;
    juint8_t recipient = bmRequestType & USB_RECIP_MASK;

    switch (feature)
    {
    case USB_DEVICE_REMOTE_WAKEUP:
        if (recipient != USB_RECIP_DEVICE || !core->curr_config ||
            !core->device_param.config_param[core->curr_config - 1].
            remote_wakeup)
        {
            rc = JEINVAL;
            goto Exit;
        }

        if (set)
            core->remote_wakeup_enabled = 1;
        else
            core->remote_wakeup_enabled = 0;

        #ifdef CONFIG_REMOTE_WAKEUP_TEST
            /* Schedule a remote wakeup request for testing 
             * We schedule the task even for clear feature, to verify that the
             * core doesn't allow to remote wakeup when the host disable it */
            jtask_schedule(core->remote_wakeup_test_task, 10000, 
                test_remote_wakeup, core);
        #endif
    
        break;
#if defined(CONFIG_JSLAVE_TEST_MODE)
    case USB_DEVICE_TEST_MODE:
        if (recipient != USB_RECIP_DEVICE)
        {
            rc = JEINVAL;
            goto Exit;
        }

        if (!set || !core->dcd_ops->dcd_set_test_mode)
        {
            rc = JEINVAL;
            goto Exit;
        }
        /* We need to change to test mode only after we send a status packet
         * to the host */
        core->request->zero = 0;
        core->request->context      = core;
        core->request->complete     = ep0_test_mode_complete_callback;
        core->request->transfer_size = 0;
        /* Save Test Selector to be used in the completion of the status
         * packet */
        core->request->test_mode = ((index & 0x0f00) >> 8);
        core->request->status = REQUEST_READY;

        rc = core_send_data(core, NULL, core->request);

        goto Exit;
#endif
#ifdef CONFIG_JOTG
    case USB_DEVICE_B_HNP_ENABLE:
        if (set && recipient != USB_RECIP_DEVICE)
        {
            rc = JEINVAL;
            goto Exit;
        }

        if (IS_DEVICE_OTG())
        {
            rc = core->dcd_ops->dcd_invoke_otg_ioctl(core->dcd_dev, 
                DRV_IOCTL_DEV_HNP_ENBL, NULL);
        }
        else
            rc = JENOTSUP;
        break;
    case USB_DEVICE_A_HNP_SUPPORT:
        if (set && recipient != USB_RECIP_DEVICE)
        {
            rc = JEINVAL;
            goto Exit;
        }

        if (!IS_DEVICE_OTG())
            rc = JENOTSUP;
        break;
    case USB_DEVICE_A_ALT_HNP_SUPPORT:
        if (set && recipient != USB_RECIP_DEVICE)
        {
            rc = JEINVAL;
            goto Exit;
        }

        if (IS_DEVICE_OTG())
        {
            rc = core->dcd_ops->dcd_invoke_otg_ioctl(core->dcd_dev, 
                DRV_IOCTL_DEV_HNP_ALT_SUPPORTED, NULL);
        }
        else
            rc = JENOTSUP;
        break;
#endif
    case USB_ENDPOINT_HALT:
        if (recipient != USB_RECIP_ENDPOINT ||
            (core->usb_state != USB_CONFIGURED && index))
        {
            rc = JEINVAL;
            goto Exit;
        }
    default:
        rc = core->dcd_ops->dcd_handle_feature(core->dcd_dev, feature, index,
            set);
        break;
    }
    
    if (!rc)
        rc = send_zero_packet(core, ep0_req_tag);

Exit:
    return rc;
}
/**
 * Function name:  handle_get_descriptor
 * Description: Handle get descriptor request
 * Parameters: 
 *     @core: (IN) CORE context
 *     @wValue: (IN) descriptor type (higher byte) and index (lower byte)
 *     @wIndex: (IN) Descriptor index
 *     @wLength: (IN) maximum length of the reply
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: local
 **/
static jresult_t handle_get_descriptor(core_t *core, juint16_t wValue, 
    juint16_t wIndex, juint16_t wLength, juint8_t ep0_req_tag)
{
    jresult_t    rc = 0;
    juint8_t     descriptor_type  = (juint8_t)(wValue >> 8);
    juint8_t     descriptor_index = (juint8_t)(wValue & 0x00ff);

    DBG_V(DSLAVE_CORE, ("CH9: Get descriptor %d\n", descriptor_type));

    switch (descriptor_type)
    {
    case USB_DT_DEVICE:
        rc = send_descriptor_device(core, wLength, ep0_req_tag);
        break;
    case USB_DT_CONFIG:
        rc = send_descriptor_config(core, descriptor_index, core->curr_speed, 
            wLength, ep0_req_tag);
        break;
    case USB_DT_STRING:
        rc = send_descriptor_string(core, descriptor_index, wIndex, wLength, 
            ep0_req_tag);
        break;
    case USB_DT_DEVICE_QUALIFIER:
#ifdef CONFIG_FULL_SPEED_ONLY
            rc = JEINVAL;
#else
            if (core->device_param.full_speed_only)
                rc = JEINVAL;
            else
                rc = send_descriptor_dev_qual(core, wLength, ep0_req_tag);
#endif
        break;
    case USB_DT_OTHER_SPEED_CONFIG:
#ifdef CONFIG_FULL_SPEED_ONLY
            rc = JEINVAL;
#else
            if (core->device_param.full_speed_only)
                rc = JEINVAL;
            else
                rc = send_descriptor_other_speed(core, wLength, ep0_req_tag);
#endif
        break;
#ifdef CONFIG_JOTG
    case USB_DT_OTG:
        if (IS_DEVICE_OTG())
            rc = send_descriptor_otg(core, wLength, ep0_req_tag);
        else
        {
            DBG_W(DSLAVE_CORE, ("Got \"Get OTG Descriptor \" on a non-OTG "
                "device\n"));
            rc = JEINVAL;
        }
        break;
#endif
    default:
        DBG_W(DSLAVE_CORE, ("Unknown descriptor type requested: %02x\n",
            descriptor_type));
        DBG_W(DSLAVE_CORE, ("Forwarding request to FD\n"));
        rc = JENOTSUP;
    }

    return rc;
}

/* Handle usb command directed for Core */

/**
 * Function name:  core_usb_command
 * Description: Handle standard usb requests (ch 9)
 * Parameters: 
 *     @core: (IN) CORE context
 *     @ctrl_req: (IN) control request
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t core_usb_command(core_t *core, struct usb_ctrlrequest *ctrl_req,
    juint8_t ep0_req_tag)
{
    jresult_t    rc            = 0;
    juint8_t     bmRequestType = ctrl_req->bRequestType;
    juint8_t     bRequest      = ctrl_req->bRequest;
    juint16_t    wValue        = le16toh(ctrl_req->wValue);
    juint16_t    wIndex        = le16toh(ctrl_req->wIndex);
    juint16_t    wLength       = le16toh(ctrl_req->wLength);
    jint_t       interface_index;

    juint8_t     *buffer = (juint8_t *)core->request->buffer.vaddr;

    switch (bRequest)
    {
    case USB_REQ_GET_STATUS:
        DBG_V(DSLAVE_CORE, ("CH9: Get status request %d:%d\n", 
            wIndex, wLength));
        rc = handle_get_status(core, bmRequestType, wIndex, wLength,
            ep0_req_tag);
        break;
    case USB_REQ_CLEAR_FEATURE:
    case USB_REQ_SET_FEATURE:
        DBG_V(DSLAVE_CORE, ("CH9: %s feature request %d:%d\n", 
            bRequest == USB_REQ_SET_FEATURE ? "Set" : "Clear", wValue, wIndex));

        rc = handle_feature(core, bmRequestType, wValue, wIndex, 
            bRequest == USB_REQ_SET_FEATURE ? 1 : 0, ep0_req_tag);
        break;
    case USB_REQ_SET_ADDRESS:
        DBG_V(DSLAVE_CORE, ("CH9: Set Address %d request\n", wValue));
        rc = core->dcd_ops->dcd_set_address_state(core->dcd_dev, wValue);
        if (!rc)
            rc = send_zero_packet(core, ep0_req_tag);
        break;
    case USB_REQ_GET_DESCRIPTOR:
        DBG_V(DSLAVE_CORE, ("CH9: Get descriptor request reached core\n"));
        rc = handle_get_descriptor(core, wValue, wIndex, wLength, ep0_req_tag);
        break;
    case USB_REQ_SET_DESCRIPTOR:
        /* We currently do not support descriptor changes */
        DBG_E(DSLAVE_CORE, ("CH9: Set descriptor request reached core\n"));
        rc = JENOTSUP;
        break;
    case USB_REQ_GET_CONFIGURATION:
        if (core->usb_state != USB_CONFIGURED && core->curr_config != 0)
        {
            DBG_E(DSLAVE_CORE, ("Uncofigured core appears to "
                "say its configured\n"));
        }
        buffer[0] = core->curr_config;
        rc = post_command_reply(core, 1, wLength, ep0_req_tag);
        break;
    case USB_REQ_SET_CONFIGURATION:
        rc = chp9_handle_set_configuration(core, (juint8_t)wValue);
        DBG_I(DSLAVE_CORE, ("CH9: Set Confiuration STATUS\n"));
        if (!rc)
            rc = send_zero_packet(core, ep0_req_tag);
        break;
    case USB_REQ_GET_INTERFACE:
        interface_index = interface_number_to_index(core, wIndex);
        if (core->usb_state != USB_CONFIGURED || wLength != 1 ||
            interface_index >=
            core->configs[core->curr_config - 1].interface_count)
        {
            rc = JEINVAL;
        }
        else
        {
            buffer[0] = core->configs[core->curr_config - 1].interfaces[
                interface_index].if_arr->alt_if_curr;
            rc = post_command_reply(core, 1, wLength, ep0_req_tag);
        }
        break;
    case USB_REQ_SET_INTERFACE:
        rc = handle_set_interface(core, (juint8_t)wIndex, (juint8_t)wValue);
        if (!rc)
            rc = send_zero_packet(core, ep0_req_tag);
        break;
    case USB_REQ_SYNCH_FRAME:
        DBG_E(DSLAVE_CORE, ("CH9: Sync Frame request reached core: %02x\n",
            bRequest));
        rc = JENOTSUP;
        break;
    default:
        /* We can't handle it, the core might want to try FD handling */
        DBG_W(DSLAVE_CORE, ("CH9: Unsupported request recieved\n"));
        rc = JENOTSUP;
    }

    /* HSU addition - standard_cmd_cb_t */
    if (core->device_param.standard_cmd_cb)
    {
      core->device_param.standard_cmd_cb(bmRequestType, bRequest, wValue, wIndex, wLength);
    }

    return rc;
}

static jresult_t chp9_handle_vendor_request(core_t *core, 
    struct usb_ctrlrequest *setup, juint8_t ep0_req_tag)
{
    jresult_t rc = JEINVAL;
    juint8_t req_recip;
    jint_t i;
    /*HSU addition*/
    juint8_t     req_type;

    /* Get the request type */
    req_type = setup->bRequestType & USB_TYPE_MASK;
    /*End of HSU addition*/
    req_recip = setup->bRequestType & USB_RECIP_MASK;

    switch (req_recip)
    {
    case USB_RECIP_DEVICE:
        {
            jbool_t is_in = setup->bRequestType & USB_DIR_IN;
            juint16_t act_size = 0;
            juint16_t wLength = le16toh(setup->wLength);

            if ((req_type == USB_TYPE_VENDOR) &&
                !((is_in && core->device_param.vendor_cmd_in_cb) ||
                (!is_in && core->device_param.vendor_cmd_out_cb && 
                core->device_param.vendor_cmd_out_data_stage_cb)))
            {
                DBG_E(DSLAVE_CORE, ("CH9: Received vendor specific command for "
                    "device, but no handlers registered\n"));
                goto Exit;
            }

            /*HSU addition*/
            if ((req_type == USB_TYPE_RESERVED) &&
                !((is_in && core->device_param.reserved_cmd_in_cb) ||
                (!is_in && core->device_param.reserved_cmd_out_cb && 
                core->device_param.reserved_cmd_out_data_stage_cb)))
            {
                DBG_E(DSLAVE_CORE, ("CH9: Received reserved command for "
                    "device, but no handlers registered\n"));
                goto Exit;
            }
            /*End of HSU addition*/

            DBG_V(DSLAVE_CORE, ("CHP9: Handling device recipient vendor "
                "specific request\n"));

            if (wLength > MAX_DESCRIPTOR_SIZE)
                REALLOC_CORE_REQUEST(core, wLength);

            if (is_in)
            {
                /*HSU addition*/
                if (req_type == USB_TYPE_RESERVED)
                {
                	rc = core->device_param.reserved_cmd_in_cb(setup->bRequest, 
                  	  setup->wValue, setup->wIndex, core->request->buffer.vaddr, 
                    	wLength, &act_size);
                } /*End of HSU addition*/
                else
                {
                rc = core->device_param.vendor_cmd_in_cb(setup->bRequest, 
                    setup->wValue, setup->wIndex, core->request->buffer.vaddr, 
                    wLength, &act_size);
                }
                if (rc)
                {
                    DBG_E(DSLAVE_CORE, ("CHP9: Failed to handle device "
                        "recipient IN vendor specific request\n"));
                    CORE_REQUEST_RESTORE(core);
                    goto Exit;
                }
                if (wLength && !act_size)
                    core->request->ep0_zlp_reply = 1;

                rc = post_command_reply(core, act_size, wLength, ep0_req_tag);
                if (rc)
                {
                    DBG_E(DSLAVE_CORE, ("CH9: Failed to post reply for "
                        "device recipient vendor specific request (%d)\n", rc));
                }
                goto Exit;
            }
            else
            {
                /*HSU addition*/
                if (req_type == USB_TYPE_RESERVED)
                {
                	rc = core->device_param.reserved_cmd_out_cb(setup->bRequest,
                  	  setup->wValue, setup->wIndex, setup->wLength);
                }/*End of HSU addition*/
                else
                {
                rc = core->device_param.vendor_cmd_out_cb(setup->bRequest,
                    setup->wValue, setup->wIndex, setup->wLength);
                }
                if (rc)
                {
                    DBG_E(DSLAVE_CORE, ("CHP9: Failed to handle device "
                        "recipient OUT vendor specific request\n"));
                    goto Exit;
                }

                if (wLength)
                {
                    core->request->zero = 0;
                    core->request->ep0_req_tag = ep0_req_tag;
                    core->request->context = core;
                    /*HSU addition*/
                    if (req_type == USB_TYPE_RESERVED)
                    {
                    	core->request->complete = 
                      	  ep0_reserved_out_data_complete_callback;
                    }/*End of HSU addition*/
                    else
                    {
                    core->request->complete = 
                        ep0_vendor_out_data_complete_callback;
                    }
                    core->request->transfer_size = wLength;
                    core->request->status = REQUEST_READY;

                    rc = core_read_data(core, NULL, core->request);
                    if (rc)
                    {
                        DBG_E(DSLAVE_CORE, ("CH9: Failed to read data stage "
                            "(%d)\n", rc));
                    }
                    goto Exit;
                }
                else
                {
                    rc = send_zero_packet(core, ep0_req_tag);
                    goto Exit;
                }
            }
        }
    case USB_RECIP_INTERFACE:
        /* For vendor command with no specific interface we should try all
         * interfaces */
        if (core->usb_state != USB_CONFIGURED)
            goto Exit;

        for (i = 0; i < core->configs[core->curr_config - 1].interface_count;
            i++)
        {
            rc = core->configs[core->curr_config - 1].interfaces[i].
                handle_usb_cmd(core->configs[core->curr_config - 1].
                interfaces[i].fd->context, (void *)setup);
            if (rc != JENOTSUP)
            {
                DBG_I(DSLAVE_CORE, ("CHP9: Vendor specific request was handled "
                    "by interface %d\n", i));
                goto Exit;
            }
        }
        break;
    }
Exit:
    return rc;
}

static __INLINE__ jint_t interface_number_to_index(core_t *core, 
    juint8_t number)
{
    jint_t i;
    jint_t idx = core->curr_config ? core->curr_config - 1 : 0;

    for (i = 0; i < core->configs[idx].interface_count; i++)
    {
        if (core->configs[idx].interfaces[i].number == number)
            break;
    }

    return i;
}

/**
 * Function name:  chp9_handle_usb_command
 * Description: Handle an usb chap9 command (decide who's responsibility it is)
 * Parameters: 
 *     @core: (IN) CORE context
 *     @buffer: (IN) the control request
 *     @ep0_req_tag: (IN) tag to verify the reply match the right control
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t chp9_handle_usb_command(core_t *core, void *buffer, 
    juint8_t ep0_req_tag)
{
    jresult_t    rc = 0;
    jint_t       i, j;
    jbool_t      found = 0;
    juint8_t     req_type;
    juint8_t     req_recip;
    jint_t       config_index = core->curr_config ? core->curr_config - 1 : 0;
    jint_t       interface_index = core->configs[config_index].interface_count;
    juint8_t     interface_num = 0;
    juint8_t     endpoint_num;
    struct usb_ctrlrequest *ctrl_req = (struct usb_ctrlrequest*)buffer;
    config_t *config = &core->configs[config_index];

    /* Get the request type */
    req_type = ctrl_req->bRequestType & USB_TYPE_MASK;
    
    /* Get the request pecipient */
    req_recip = ctrl_req->bRequestType & USB_RECIP_MASK;

    DBG_V(DSLAVE_CORE, ("CH9: bRequestType = 0x%x, bRequest = 0x%x,"
        "wValue = 0x%x, wIndex = 0x%x, wLength = 0x%x\n", 
        ctrl_req->bRequestType, ctrl_req->bRequest, ctrl_req->wValue,
        ctrl_req->wIndex, ctrl_req->wLength));

    /* Try to pass the request to Core and see if it can handle it */
    if (req_type == USB_TYPE_STANDARD)
    {
        rc = core_usb_command(core, ctrl_req, ep0_req_tag);
        if (rc != JENOTSUP)
            goto Exit;
    }
    else if (req_type == USB_TYPE_VENDOR && req_recip == USB_RECIP_DEVICE &&
        ((ctrl_req->bRequest == core->msos_vendor_code) && 
        core->msos_vendor_code_set))
    {
        DBG_I(DSLAVE_CORE, ("CH9: request recipient :device, type: vendor \n"));
        interface_num = (juint8_t)(le16toh(USB_REQ_INTERFACE_NUMBER(
            ctrl_req->wValue)));
        /* If we're not configured yet, we'll check the first configuration
         * for the requested interface */
        interface_index = interface_number_to_index(core, interface_num);
    }

    endpoint_num = (juint8_t)USB_REQ_ENDPOINT_NUMBER(ctrl_req->wIndex);

    /* If not interface was specified, try the first FD
     * else find the exact recipient by iface or endpoint number */
    if (req_recip == USB_RECIP_INTERFACE)
    {
        if (core->usb_state != USB_CONFIGURED)
        {
            rc = JEINVAL;
            goto Exit;
        }

        interface_num = (juint8_t)USB_REQ_INTERFACE_NUMBER(ctrl_req->wIndex);
        interface_index = interface_number_to_index(core, interface_num);
    }
    else if (req_recip == USB_RECIP_ENDPOINT && endpoint_num)
    {
        if (core->usb_state != USB_CONFIGURED)
        {
            rc = JEINVAL;
            goto Exit;
        }

        endpoint_num |= (juint8_t)USB_REQ_ENDPOINT_DIRECTION(ctrl_req->wIndex);

        /* Check out all interfaces */
        for (i = 0; i < config->interface_count && !found; i++)
        {
            /* Check out all active endpoints in that interface */
            for (j = 0; config->interfaces[i].if_curr && 
                j < config->interfaces[i].if_curr->pipe_count && !found; j++)
            {
                /* Did we find it ? */
                if (config->interfaces[i].if_curr->pipes[j].address == 
                    endpoint_num)
                {
                    interface_num = (juint8_t)config->interfaces[i].number;
                    interface_index = i;
                    found = 1;
                }
            }
        }
    }

    if (interface_index >= config->interface_count)
    {
        /*HSU addition*/
        if ((req_type != USB_TYPE_VENDOR) && (req_type != USB_TYPE_RESERVED))
        {
            DBG_E(DSLAVE_CORE, ("CH9: Request referencing a non existant "
                "interface: %d\n", interface_num));
            rc = JEINVAL;
            goto Exit;
        }

        rc = chp9_handle_vendor_request(core, ctrl_req, ep0_req_tag);
        goto Exit;
    }

    DBG_V(DSLAVE_CORE, ("CH9: Class specific at interface %d\n", 
        interface_num));

    /* Call the FD's EP0 command handler */
    rc = config->interfaces[interface_index].handle_usb_cmd(
        config->interfaces[interface_index].fd->context, buffer);

Exit:
    return rc;
}

/* HSU addition */
/**
 * Function name: chp9_clear_string_descriptors 
 * Description: Clear string descriptors array & indexes
 * Parameters: 
 *     @core: (IN) CORE context
 *
 * Return value: None
 * Scope: global
 **/
void chp9_clear_string_descriptors(core_t *core)
{
  juint8_t i = 0;

  /* Zero string descriptors array */
  for (i = 0; i < MAX_STRING_DESCRIPTORS; i++)
  {
    core->string_descriptors[i] = NULL;
  }

  core->i_product = 0;
  core->i_manufacturer = 0;
  core->i_serial_number = 0;
  config_descriptor.iConfiguration = 0;
}
/* End of HSU addition */

/**
 * Function name: chp9_set_string_descriptors 
 * Description: Set device level string descriptors
 * Parameters: 
 *     @core: (IN) CORE context
 *
 * Return value: None
 * Scope: global
 **/
void chp9_set_string_descriptors(core_t *core)
{
    dev_info_t *dev_info = core->device_param.dev_info;

    if (core->i_product)
    {
        core_change_string_descriptor(core, core->i_product, 
         dev_info->product_name);
    }
    else
    {
        core->i_product = core_add_string_descriptor(core,
         dev_info->product_name, HSU_STR_IDX_PRODUCT);
    }
       
    if (core->i_manufacturer)
    {
        core_change_string_descriptor(core, core->i_manufacturer, 
         dev_info->product_manufacturer);
    }
    else
    {
        core->i_manufacturer = core_add_string_descriptor(core,
         dev_info->product_manufacturer,HSU_STR_IDX_MANUFACTURER);
    }

    if (dev_info->product_serial)
    {
        if (core->i_serial_number)
        {
            core_change_string_descriptor(core, core->i_serial_number, 
             dev_info->product_serial);
        }
        else
        {
            core->i_serial_number = core_add_string_descriptor(core, 
             dev_info->product_serial, HSU_STR_IDX_SERIAL_NUMBER);
        }
    }
    else        /* there's no serial now */
    {
        if (core->i_serial_number)
        {
            /* special case : previously there was a serial descriptor and now
             * there isn't any (probably after device reconfiguration) so we
             * should release the space in the descriptors array 
             * */
            core_release_string_descriptor(core, core->i_serial_number);
            core->i_serial_number = 0;
        }
    }
}

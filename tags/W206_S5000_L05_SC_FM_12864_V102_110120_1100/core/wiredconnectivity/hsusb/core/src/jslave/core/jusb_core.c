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
#include "jusb_core.h"
#include <uw_args.h>

#define CORE_RECONFIG_DELAY_MS 100

/* TODO - consider move this to header file */
#ifdef CONFIG_FD_MASS
jresult_t mass_storage_init(void *ctx);
#endif

#ifdef CONFIG_FD_LOOPBACK
jresult_t loopback_init(void *ctx);
#endif

#ifdef CONFIG_FD_TEST
jresult_t fd_test_init(void *ctx);
#endif

#ifdef CONFIG_FD_SERIAL
jresult_t serial_init(void *ctx);
#endif

#ifdef CONFIG_FD_HID_KEYBOARD
jresult_t hid_kbd_init(void *ctx);
#endif

#ifdef CONFIG_FD_CDC_ACM
jresult_t cdc_init_acm(void *ctx);
#endif
    
#ifdef CONFIG_FD_CDC_ECM
jresult_t cdc_init_ecm(void *ctx);
#endif
    
#ifdef CONFIG_FD_CDC_OBEX
jresult_t cdc_init_obex(void *ctx);
#endif

#ifdef CONFIG_FD_VIDEO
jresult_t video_init(void *ctx);
#endif

#ifdef CONFIG_FD_DFU
jresult_t dfu_init(void *ctx);
#endif

#ifdef CONFIG_FD_SICD
jresult_t sicd_init(void *ctx);
#endif

#ifdef CONFIG_FD_AUDIO
jresult_t fd_audio_init(void *ctx);
#endif

static jresult_t register_finish(core_t *core);

/* Cores maintenance */
static core_states_t global_core_state = CORE_NOT_INITIALIZED;
static core_t *cores;
static juint8_t ncores;

juint16_t MAX_PACKET_SIZES[4][3] = { 
/* Control      : */ { 8,       64,     64 },
/* Isync        : */ { 0,       1023,   1024 },
/* Bulk         : */ { 0,       64,     512 },
/* Interupt     : */ { 8,       64,     1024 }};

/**
 * Function name:  find_core_by_idx
 * Description: Locate the core context the match <dcd_idx>
 * Parameters: 
 *     @dcd_idx: (IN) index (zero based) of the controller 
 *
 * Return value: Core context or NULL of not found
 * Scope: local
 **/
static __INLINE__ core_t *find_core_by_idx(juint8_t dcd_idx)
{
    if (dcd_idx >= ncores)
        return NULL;

    return &cores[dcd_idx];
}

#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
/**
 * Function name:  core_remote_wakeup_task
 * Description: Called if remote wakeup request was sent, but there was no
 *              resume from the host - cancel all queued requests.
 * Parameters: 
 *     @ctx: (IN) CORE context
 *     
 * Scope: local
 **/
static void core_remote_wakeup_task(void *ctx)
{
    jint_t i, j;
    core_t *core = (core_t *)ctx;

    DBG_W(DSLAVE_DCD, ("core_remote_wakeup_task: Didn't get RESUME from the "
        "host, abort all pending transfers\n"));

    /* Abort control pipe transfer */
    core_abort_pipe(core, NULL);

    if (!core->curr_config)
        goto Exit;

    /* Abort all data pipes transfers */
    for (i = 0; i < core->configs[core->curr_config - 1].interface_count; i++)
    {
        alt_interface_desc_t *ifc =
            core->configs[core->curr_config - 1].interfaces[i].if_curr;

        for (j = 0; j < ifc->pipe_count; j++)
        {
            pipe_desc_t *pipe = &ifc->pipes[j];
            if (!STAILQ_EMPTY(&pipe->req_queue))
            {
                DBG_V(DSLAVE_DCD, ("core_remote_wakeup_task: cancel transfers "
                    "on pipe 0x%x\n", pipe->address));
                core_abort_pipe(core, pipe);
            }
        }
    }

Exit:
    core->remote_wakeup_scheduled = 0;
}
#endif

/**
 * Function name:  core_handle_tx_completion
 * Description: Called by the function driver to handle "IN" timeout in the DCD
 * Parameters: 
 *     @ctx: (IN) CORE context
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: None
 * Scope: Global
 **/
void core_handle_tx_completion(void *ctx, pipe_desc_t *pipe)
{
    core_t *core = (core_t *)ctx;

    if (core->dcd_ops->dcd_handle_tx_completion)
        core->dcd_ops->dcd_handle_tx_completion(core->dcd_dev, pipe);
}

/**
 * Function name:  send_io_request
 * Description: Send new request. In case the pipe request queue is empty, send
 *              the transfer to the DCD, otherwise only queue it.
 * Parameters: 
 *     @core: (IN) CORE context
 *     @pipe: (IN) CORE pipe context
 *     @request: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static __INLINE__ jresult_t send_io_request(core_t *core, pipe_desc_t *pipe, 
    request_t *request)
{
    jbool_t queue_empty = 0;
    jresult_t rc = 0;
    request_t *curr_request;
    LIST_HEAD(req_list_head, request_t) req_list;

    LIST_FIRST(&req_list) = request;

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_V(DSLAVE_CORE, ("Send IO\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */

    /* Make sure the device is in proper state for sending data */
    if (core->core_state != CORE_ENABLED && 
        core->core_state != CORE_SUSPENDED &&
        core->usb_state != USB_CONFIGURED)
    {
        DBG_E(DSLAVE_CORE, ("Attempt to %s data when Core/USB "
            "not in proper state %02x\n", 
            request->direction == DIRECTION_IN ? "send" : "read", 
            core->usb_state));
        rc = JEPERM;
        goto Exit;
    }

    if (pipe && pipe->status == PIPE_ABORTING)
    {
        DBG_E(DSLAVE_CORE, ("Attempt to %s data while pipe aborting\n",
            request->direction == DIRECTION_IN ? "send" : "read"));
        rc = JEPERM;
        goto Exit;
    }

#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    if (core->core_state == CORE_SUSPENDED)
    {
        if (!core->remote_wakeup_enabled)
        {
            DBG_E(DSLAVE_CORE, ("Attempt to %s data when DCD in suspend mode, "
                "while remote wakeup wasn't enabled by the host\n",
                request->direction == DIRECTION_IN ? "send" : "read"));
            rc = ESUSPENDED;
            goto Exit;
        }                     

        /* Only IN request can wake the host */
        if (request->direction == DIRECTION_OUT)
        {
            DBG_E(DSLAVE_CORE, ("Attemp to read data when the DCD is in "
                "suspend mode\n"));
            rc = ESUSPENDED;
            goto Exit;
        }
    }
#else
    if (core->core_state == CORE_SUSPENDED)
    {
        DBG_E(DSLAVE_CORE, ("Attempt to %s data when DCD in suspend mode\n",
            request->direction == DIRECTION_IN ? "send" : "read"));
        rc = ESUSPENDED;
        goto Exit;
    }
#endif

    LIST_FOREACH(curr_request, &req_list, next_request_in_list)
    {
        /* check request is not in use */
        if (curr_request->status != REQUEST_READY)
        {
            DBG_E(DSLAVE_CORE, ("Attemp to use not ready request %x [size %d]\n",
                curr_request, curr_request->transfer_size));
            rc = JEBUSY;
            goto Exit;
        }

        /* check buffer validity */
        if (curr_request->transfer_size && !curr_request->buffer.vaddr)
        {
            DBG_E(DSLAVE_CORE, ("Bad buffer passed to %s data\n",
                curr_request->direction == DIRECTION_IN ? "send" : "read")); 
            rc = JEINVAL;
            goto Exit;
        }
    }

#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    /* Try to wakeup the host - wait for resume CONFIG_REMOTE_WAKEUP_TIMEOUT 
     * ms */
    if (core->core_state == CORE_SUSPENDED)
    {
        DBG_I(DSLAVE_CORE, ("CORE in SUSPEND mode!\n"));
        if (!core->remote_wakeup_scheduled)
        {
            DBG_V(DSLAVE_CORE, ("send_io_request: schudule remote wakeup "
                "task\n"));
            rc = jslave_wakeup_host(core->core_number);
            if (rc)
            {
                rc = ESUSPENDED;
                goto Exit;
            }
            rc = jtask_schedule(core->remote_wakeup_task,
                CONFIG_REMOTE_WAKEUP_TIMEOUT, core_remote_wakeup_task, core);
            if (rc)
            {
                DBG_E(DSLAVE_CORE, ("jtask_schedule for remote_wakeup_task "
                    "failed - rc %d\n", rc));
                goto Exit;
            }
            core->remote_wakeup_scheduled = 1;

        }
    }
#endif

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DSLAVE_CORE, ("Sending a new io %s pipe (0x%x) "
        "%d\n", request->single_request ?
        "single request" : "request list", pipe ? pipe->address : 0));
#endif /* CONFIG_DATA_PATH_DEBUG */

    LIST_FOREACH(curr_request, &req_list, next_request_in_list)
    {
        curr_request->status = REQUEST_INPROGRESS;
        curr_request->bytes_transferred = 0;
    }

    if (pipe && (request->direction == DIRECTION_OUT || 
        request->single_request || core->remote_wakeup_scheduled))
    {
        queue_empty = STAILQ_EMPTY(&pipe->req_queue);
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DSLAVE_CORE, ("send_io_request: queueing request on ep 0x%x. "
            "queue is %sempty\n", pipe->address, queue_empty ? "" : "NOT "));
#endif /* CONFIG_DATA_PATH_DEBUG */
        STAILQ_INSERT_TAIL(&pipe->req_queue, request, next_request);
    }    
        
    if (!core->remote_wakeup_scheduled && (!pipe || queue_empty ||
        (request->direction == DIRECTION_IN && !request->single_request)))
    {
        rc = core->dcd_ops->dcd_send_io_request(core->dcd_dev, pipe, request);
    }

Exit:
    return rc;
}

/**
 * Function name:  core_send_data
 * Description: Send data to the host
 * Parameters: 
 *     @ctx: (IN) DCD context 
 *     @pipe: (IN) CORE pipe context
 *     @request: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_send_data(void *ctx, pipe_desc_t *pipe, request_t *request)
{
    request_t *curr_request;
    LIST_HEAD(req_list_head, request_t) req_list;

    LIST_FIRST(&req_list) = request;

    LIST_FOREACH(curr_request, &req_list, next_request_in_list)
        curr_request->direction = DIRECTION_IN;

    return send_io_request((core_t *)ctx, pipe, request);
}

/**
 * Function name:  core_read_data
 * Description: Read data from the host
 * Parameters: 
 *     @ctx: (IN) DCD context  
 *     @pipe: (IN) CORE pipe context
 *     @request: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_read_data(void *ctx, pipe_desc_t *pipe, request_t *request)
{
    request_t *curr_request;
    LIST_HEAD(req_list_head, request_t) req_list;

    LIST_FIRST(&req_list) = request;

    LIST_FOREACH(curr_request, &req_list, next_request_in_list)
        curr_request->direction = DIRECTION_OUT;

    return send_io_request((core_t *)ctx, pipe, request);
}

/**
 * Function name:  core_stall_pipe
 * Description: Set/Clear STALL on the pipe
 * Parameters: 
 *     @ctx: (IN) DCD context  
 *     @pipe: (IN) CORE pipe context
 *     @flag: (IN) 1 for stall the endpoint, 0 for clear stalled endpoint
 *
 * Return value: None
 * Scope: global
 **/
void core_stall_pipe(void *ctx, pipe_desc_t *pipe, juint8_t flag)
{
    core_t *core = (core_t *)ctx;
    jresult_t rc;

    rc = core->dcd_ops->dcd_stall_ep(core->dcd_dev, pipe, flag);
    if (rc)
    {
        DBG_E(DSLAVE_CORE, ("core_stall_pipe: DCD failed to stall the pipe "
            "(%d)\n", rc));
    }
}

/**
 * Function name:  core_abort_pipe
 * Description: Cancel all pending request on the pipe. All completion callback
 *              will be called with request status set to REQUEST_CANCELLED
 * Parameters: 
 *     @ctx: (IN) DCD context
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: None
 * Scope: global
 **/
void core_abort_pipe(void *ctx, pipe_desc_t *pipe)
{
    core_t *core = (core_t *)ctx;
    jresult_t rc;

    rc = core->dcd_ops->dcd_abort_ep(core->dcd_dev, pipe);
    if (rc)
    {
        DBG_E(DSLAVE_CORE, ("core_abort_pipe: Failed to abort ep on DCD (%d)\n",
            rc));
    }
}

/**
 * Function name:  init_fd
 * Description: Call to the specific function driver initialization function
 * Parameters: 
 *     @core: (IN) CORE context
 *     @type: (IN) function driver type 
 *     @count: (IN) how many instances of the function driver
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t init_fd(core_t *core, fd_instance_t *fd_instance)
{
    jresult_t rc = 0;
    jint_t i;

    for (i = 0; i < fd_instance->count; i++)
    {
        switch (fd_instance->type)
        {
        #ifdef CONFIG_FD_MASS
        case DEVICE_TYPE_MSD:
            {
                rc = mass_storage_init(core);
                if (rc)
                    goto Error;
                break;
            }
        #endif

        #ifdef CONFIG_FD_LOOPBACK
        case DEVICE_TYPE_LOOPBACK:
            rc = loopback_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_SERIAL
        case DEVICE_TYPE_SERIAL:
            rc = serial_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_HID_KEYBOARD
        case DEVICE_TYPE_HID:
                rc = hid_kbd_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_TEST
        case DEVICE_TYPE_TEST:
            rc = fd_test_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_CDC_ACM
        case DEVICE_TYPE_CDC_ACM:
            rc = cdc_init_acm(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_CDC_ECM
        case DEVICE_TYPE_CDC_ECM:
            rc = cdc_init_ecm(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_CDC_OBEX
        case DEVICE_TYPE_CDC_OBEX:
            rc = cdc_init_obex(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_VIDEO
        case DEVICE_TYPE_VIDEO:
            rc = video_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_DFU
        case DEVICE_TYPE_DFU:
            rc = dfu_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_SICD
        case DEVICE_TYPE_SICD:
            rc = sicd_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        #ifdef CONFIG_FD_AUDIO
        case DEVICE_TYPE_AUDIO:
            rc = fd_audio_init(core);
            if (rc)
                goto Error;
            break;
        #endif

        case DEVICE_TYPE_VENDOR:
            if (fd_instance->init_func)
            {
                rc = fd_instance->init_func(core);
            }
            else 
            {
                DBG_E(DSLAVE_CORE, ("Got DEVICE_TYPE_VENDOR with no init "
                    "callback\n"));
                rc = JEINVAL;
            }
            if (rc)
                goto Error;
            break;
        default:
            DBG_E(DSLAVE_CORE, ("Unknown FD type : %d\n",fd_instance->type));
            rc = JEINVAL;
            break;
        }
    }

Error:
    
    return rc;
}

/**
 * Function name:  init_fds
 * Description: Initialize all relevant FD's for the specific core
 * Parameters: 
 *     @core: (IN) CORE context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t init_fds(core_t *core)
{
    juint_t i, j;
    jresult_t rc = 0;

    core->configs = jmalloc(sizeof(config_t) *
        core->device_param.num_of_configs, M_ZERO);
    if (!core->configs)
    {
        DBG_E(DSLAVE_CORE, ("init_fds: failed allocating configuration "
            "list\n"));
        return JENOMEM;
    }

    core->config_count = core->device_param.num_of_configs;

    DBG_V(DSLAVE_CORE ,("init_fds: config count %d\n", core->config_count));
    for (i = 0; i < core->config_count; i++)
    {
        DBG_V(DSLAVE_CORE ,("init_fds: config #%d\n", i)); 
        core->curr_config = i + 1;
        for (j = 0; j < core->device_param.config_param[i].num_of_fd_instances;
            j++)
        {
            fd_instance_t *fd_instance =
                &core->device_param.config_param[i].fd_instances[j];

            rc = init_fd(core, fd_instance);
            if (rc)
            {
                DBG_E(DSLAVE_CORE, ("failed initialize FD (type %d)\n", 
                    fd_instance->type));
                goto Error;
            }
        }

        if (core->device_param.config_param[i].config_name)
        {
            core->configs[i].i_config = core_add_string_descriptor(core,
                core->device_param.config_param[i].config_name, HSU_STR_IDX_NOT_SET);
        }
    }

    core->curr_config = 0;
    core->core_state = CORE_FD_CONFIGURED;

    rc = register_finish(core);
    if (rc)
    {
        DBG_E(DSLAVE_CORE ,("init_fds: register_finish failed\n")); 
        goto Error;
    }

    return 0;

Error:
    for (i = 0; i < core->config_count; i++)
    {
        if (core->configs[i].i_config)
            core_release_string_descriptor(core, core->configs[i].i_config);

        jfree(core->configs);
        core->configs = NULL;
        core->config_count = 0;
    }

    return rc;
}

/**
 * Function name:  register_finish
 * Description: Called after all function driver have been initialized and
 *              registered with core. Allocate and build the device descriptors
 *              according to the various function drivers.
 * Parameters: 
 *     @core: (IN) CORE context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t register_finish(core_t *core)
{
    juint8_t idx;

    DBG_V(DSLAVE_CORE, ("Finishing registration process\n"));

    if (core->core_state != CORE_FD_CONFIGURED)
    {
        DBG_E(DSLAVE_CORE, ("register_finish: called before CORE FD "
            "CONFIGURED (state %d)\n", core->core_state));
        return JEPERM;
    }

    for (idx = 0; idx < core->config_count; idx++)
    {
        juint8_t i, j, fd_count;
        fd_t *fd;
        config_t *config = &core->configs[idx];

        DBG_V(DSLAVE_CORE, ("registering config %d\n", idx));
        /* Initialize the interface list */
        config->interfaces = jmalloc(sizeof(interface_t) *
            config->interface_count, M_ZERO);
        if (!config->interfaces)
            goto Error;

        /* Fill up Interface data */
        for (fd_count = 0, i = 0, j = 0, fd = config->fd_list;
            fd && i < config->interface_count;
            i++, j++)
        {
            /* Each FD might handle a few interfaces */
            if (j >= fd->fd_desc->interface_count)
            {
                fd = fd->next;
                fd_count++;
                j = 0;
            }

            DBG_V(DSLAVE_CORE, ("Adding interface %d FD %d\n", i, fd_count));
            config->interfaces[i].fd_if_num = j;
            config->interfaces[i].fd      = fd;

            /* Checking if we received an interface number from the application.
             * If we did setting it as the interface number, if we didn't
             * allocation interface number */
            if (fd->fd_desc->interfaces[j].number != UWD_UNDEFINED_INTERFACE)
            {
                KASSERT(core->device_param.static_interface_number, ("Interface"
                    " number should be allocated by the core %d\n", 
                    core->device_param.static_interface_number));
                config->interfaces[i].number =
                    fd->fd_desc->interfaces[j].number;
            }
            else
            {
                KASSERT(!core->device_param.static_interface_number,
                    ("Interface number should be allocated by the "
                    "applications\n"));
                config->interfaces[i].number  = i;
                fd->fd_desc->interfaces[j].number = i;
            }

            config->interfaces[i].if_curr =
                &fd->fd_desc->interfaces[j].alt_ifs[0];
            config->interfaces[i].if_arr  = &fd->fd_desc->interfaces[j];
            config->interfaces[i].handle_usb_cmd = 
                fd->fd_desc->interfaces[j].control_msg;

            /* Set first interface to be alt-set 0 */
            fd->fd_desc->interfaces[j].alt_if_curr = 0;
        }
    }

    core->core_state = CORE_CONFIGURED;

    return 0;

Error:
    for (; idx; idx--)
        jfree(core->configs[idx - 1].interfaces);

    return JENOMEM;
}

/**
 * Function name:  core_register_fd
 * Description: Called by the function driver to register with the CORE
 * Parameters: 
 *     @ctx: (IN) CORE context 
 *     @ops: (IN) function driver callbacks
 *     @context: (IN) function driver context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_register_fd(void *ctx, fd_ops_t *ops, context_t context)
{
    core_t *core = (core_t *)ctx;
    fd_t *fd, **tmp;
    jint_t i, j, k;
    jresult_t rc;
    juint8_t interface_number;

    DBG_V(DSLAVE_CORE, ("Register FD\n"));

    /* Is this the right time ? */
    if (core->core_state != CORE_DCD_CONFIGURED)
    {
        DBG_E(DSLAVE_CORE, ("Attempt to register FD at wrong Core state %02x\n",
            core->core_state));
        return JEPERM;
    }

    /* Allocate new FD struct */
    fd = jmalloc(sizeof(fd_t), M_ZERO);
    if (!fd)
    {
        DBG_E(DSLAVE_CORE, ("Cannot alloc memory for FD structure\n"));
        return JENOMEM;
    }

    fd->fd_ops  = ops;
    fd->context = context;
    fd->next    = NULL;
    fd->state   = FD_DISABLED;
   
    /* Retrieve FD's requested configuration */
    fd->fd_desc = fd->fd_ops->get_pipe_config(fd->context);
    if (!fd->fd_desc)
    {
        DBG_E(DSLAVE_CORE, ("Got no pipe configuration from FD"));
        jfree(fd);
        return JEINVAL;
    }

    DBG_V(DSLAVE_CORE, ("Got Pipe config. Interface count = %d\n",
        fd->fd_desc->interface_count));

    /* Alloc pipes for each new interface */
    for (i = 0; i < fd->fd_desc->interface_count; i++)
    {
        if (fd->fd_desc->interfaces[i].number == 
            UWD_UNDEFINED_INTERFACE)
        {
            interface_number =
                core->configs[core->curr_config - 1].interface_count + i;
        }
        else
        {
            interface_number = fd->fd_desc->interfaces[i].number;
        }

       /* For each alternate setting */
        for (j = 0; j < fd->fd_desc->interfaces[i].alt_if_num; j++)
        {
            /* For each endpoint */
            for (k = 0; 
                k < fd->fd_desc->interfaces[i].alt_ifs[j].pipe_count; 
                k++)
            {
                pipe_desc_t *pipe = 
                    &fd->fd_desc->interfaces[i].alt_ifs[j].pipes[k];

                rc = core->dcd_ops->dcd_alloc_pipe(core->dcd_dev,
                    pipe, core->curr_config, interface_number, (juint8_t)j);
                if (rc)
                {
                    DBG_E(DSLAVE_CORE, ("CORE: Error allocating pipes "
                        "for FD %d\n", rc));
                    return rc;
                }

                STAILQ_INIT(&pipe->req_queue);
            }
        }
    }

    /* Calculate total interface count */
    core->configs[core->curr_config - 1].interface_count +=
        fd->fd_desc->interface_count;

    /* Update max suported speed */
    if (core->max_speed > fd->fd_desc->speed)
        core->max_speed = fd->fd_desc->speed;

    /* If an IAD is defined, we need to know it later */
    if (fd->fd_desc->iad)
        core->iad_defined = 1;
        
    /* Add FD to the end of the list of registered FDs 
     * This affects interface number assignment */
    for (tmp = &core->configs[core->curr_config - 1].fd_list; *tmp;
        tmp = &((*tmp)->next))
        ;

    *tmp = fd;

    DBG_V(DSLAVE_CORE, ("FD registered! Core state = %02x\n", 
        core->core_state));

    return 0;
}

/**
 * Function name:  core_get_available_iface
 * Description: Return the next free interface number that can be used by the
 *              function driver
 * Parameters: 
 *     @ctx: (IN) CORE context
 *
 * Return value: The index of the next free interface
 * Scope: global
 **/
juint8_t core_get_available_iface(void *ctx)
{
    core_t *core = (core_t *)ctx;

    KASSERT(core->curr_config, ("CORE: calling core_get_available_iface "
        "outside Init sequence\n"));
    return core->configs[core->curr_config - 1].interface_count;
}

/**
 * Function name:  core_request_alloc
 * Description: Allocate new CORE request, including a data buffer (in case
 *              <address> is NULL 
 * Parameters: 
 *     @buffer_size: (IN) size of data buffer
 *     @address: (IN) data buffer address
 *
 * Return value: pointer to the new allocated request, or NULL in case of an
 *               error
 * Scope: global
 **/
/* HSU Temp Fix for Jungo CR B49754: Start */
/* (Added core_request_alloc_ex() and changed implementation of core_request_alloc() */
request_t *core_request_alloc(juint32_t buffer_size, void *address)
{
  return core_request_alloc_ex(buffer_size, address, 0);
}

request_t *core_request_alloc_ex(juint32_t buffer_size, void *address, juint16_t align)
{
    request_t *request;

    request = jmalloc(sizeof(request_t), M_ZERO);
    if (!request)
    {
        DBG_E(DSLAVE_CORE, ("core_request_alloc: Failed to allocate "
            "request\n"));
        return NULL;
    }

    if (address)
    {
        request->buffer.vaddr = request->buffer.dma_addr = 
            request->buffer.cookie = address;
    }
    else
    {
        jresult_t rc = jdma_alloc(buffer_size, align, &request->buffer.vaddr, 
            &request->buffer.dma_addr, M_CACHABLE, &request->buffer.cookie);

        if (rc)
        {
            DBG_E(DSLAVE_CORE, ("core_request_alloc: Failed to allocate buffer "
                "for the request (size %ld bytes)\n\n", buffer_size));
            jfree(request);
            return NULL;
        }
        request->is_buffer_allocated = 1;
    }
    request->buffer.buffer_size = buffer_size;
    request->completed_req = 0;
    request->timeout = 0;
    request->single_request = 1;
    LIST_NEXT(request, next_request_in_list) = NULL;

    return request;
}
/* HSU Temp Fix for Jungo CR B49754: End */
/**
 * Function name:  core_request_free
 * Description: Free previously allocate request
 * Parameters: 
 *     @request: (IN) CORE request
 *
 * Return value: None
 * Scope: global
 **/
void core_request_free(request_t *request)
{
    request_t *curr_request, *prev_req;
    if (!request)
        return;

    curr_request = request;
    while (curr_request)
    {
        if (curr_request->is_buffer_allocated && curr_request->buffer.cookie)
            jdma_free(curr_request->buffer.cookie);
        prev_req = curr_request;
        curr_request = LIST_NEXT(curr_request, next_request_in_list);
        jfree(prev_req);
    }
   }

/**
 * Function name:  core_handle_ep0
 * Description: Called by DCD upon endpoint 0 transfer completion
 * Parameters: 
 *     @core_ctx: (IN) CORE context
 *     @buffer: (IN) data buffer
 *     @speed: (IN) device speed
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_handle_ep0(void *core_ctx, juint8_t *buffer, device_speed_t speed)
{
    core_t *core = (core_t *)core_ctx;

    DBG_V(DSLAVE_CORE, ("Handle EP\n"));

    /* Increment the EP0 request tag (so we can only answer the latest EP0) */
    core->ep0_req_tag++;
    
    if (core->core_state != CORE_ENABLED)
    {
        DBG_E(DSLAVE_CORE, ("EP0 Message at wrong Core state %02x\n",
            core->core_state));
        return JEPERM;
    }

    if (core->usb_state != USB_CONFIGURED && core->usb_state != USB_ADDRESS)
    {
        DBG_E(DSLAVE_CORE, ("EP0 Message at wrong usb device state %02x\n",
            core->usb_state));
        return JEPERM;
    }

    if (!buffer)
    {
        DBG_E(DSLAVE_CORE, ("Bad buffer(0x%x) passed to handle EP0\n", buffer));
        return JEINVAL;
    }

    core->curr_speed = speed;

    /* HSU Addition */
    if(NULL != core->ep0_handler)
    {
      return core->ep0_handler(core, buffer, core->ep0_req_tag);
    }
    else
    {
    return chp9_handle_usb_command(core, buffer, core->ep0_req_tag);
}
    /* HSU Addition */
}

/**
 * Function name:  jslave_wakeup_host
 * Description: Called to issue a remote wakeup to the host
 * Parameters: 
 *     @core_number: (IN) Number of the core to enable
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t jslave_wakeup_host(juint8_t core_number)
{
    core_t *core = find_core_by_idx(core_get_controller_idx(core_number));
                        
    if (!core)
    {
        DBG_E(DSLAVE_CORE, ("Trying to wakeup a DCD to a non-existing Core\n"));
        return JEINVAL;
    } 

    if (!core->dcd_ops->wake_the_host)
        return JENOTSUP;

    if (!core->remote_wakeup_enabled)
    {
        DBG_E(DSLAVE_DCD, ("wake_the_host: Remote wakeup wasn't enabled by "
            "the host\n"));
        return JEINVAL;
    }

    return core->dcd_ops->wake_the_host(core->dcd_dev);
}

/**
 * Function name:  core_suspend
 * Description: Handle suspend from the bus
 * Parameters: 
 *     @core_ctx: (IN) CORE context
 *     @bus: (IN) DCD bus handle
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_suspend(void *core_ctx)
{
    core_t *core = (core_t *)core_ctx;

    core->core_state = CORE_SUSPENDED;

    /* Abort current control transfer */
    core->dcd_ops->dcd_abort_ep(core->dcd_dev, NULL);
    core->request->status = REQUEST_READY;

    /* Notify function drivers on suspend */
    if (core->curr_config)
    {
        fd_t *fd;

        for (fd = core->configs[core->curr_config - 1].fd_list; fd;
            fd = fd->next)
        {
            if (fd->fd_ops && fd->fd_ops->suspend)
                fd->fd_ops->suspend(fd->context);
        }
    }

    /* Notify application on bus suspend */
    jnotify_os(NOTIFY_DEVICE_SUSPEND, j_device_get_bus(core->dcd_dev));

    return 0; 
}

/**
 * Function name:  core_resume
 * Description: Handle resume from the bus
 * Parameters: 
 *     @core_ctx: (IN) CORE context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_resume(void *core_ctx)
{
    core_t *core = (core_t *)core_ctx;
    jbus_h dcd_bus = j_device_get_bus(core->dcd_dev);

    /* Notify application on resume signal */
    jnotify_os(NOTIFY_DEVICE_RESUME, dcd_bus);

    core->core_state = CORE_ENABLED;

#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    if (core->remote_wakeup_scheduled)
    {
        jtask_stop(core->remote_wakeup_task);
        core->remote_wakeup_scheduled = 0;

        if (core->curr_config)
        {
            jint_t i, j;
            config_t *config = &core->configs[core->curr_config - 1];

            for (i = 0; i < config->interface_count; i++)
            {
                alt_interface_desc_t *ifc = config->interfaces[i].if_curr;
                for (j = 0; j < ifc->pipe_count; j++)
                {
                    pipe_desc_t *pipe = &ifc->pipes[j];
                    request_t *req;
                    jresult_t rc;
                    jbool_t in_multiple_req = 0;

                    if (STAILQ_EMPTY(&pipe->req_queue))
                        continue;

                    req = STAILQ_FIRST(&pipe->req_queue);
                    if (req->direction == DIRECTION_IN && !req->single_request)
                        in_multiple_req = 1;

                        DBG_V(DSLAVE_DCD, ("resume: resuming transfers on pipe "
                            "0x%x\n", pipe->address));

                    do
                    {
                        rc = core->dcd_ops->dcd_send_io_request(core->dcd_dev,
                            pipe, req);
                        if (rc)
                        {
                            DBG_E(DSLAVE_CORE, ("core_resume: Failed sending io"
                                " request, rc %d", rc));
                            STAILQ_REMOVE_HEAD(&pipe->req_queue, next_request);
                            if (req->complete)
                                req->complete(req);
                            return rc;
                    }

                        if (in_multiple_req)
                            STAILQ_REMOVE_HEAD(&pipe->req_queue, next_request);

                        req = STAILQ_FIRST(&pipe->req_queue);
                    }while (in_multiple_req && !STAILQ_EMPTY(&pipe->req_queue));
                }
            }
        }
    }
#endif

    /* Notify function drivers on resume */
    if (core->curr_config)
    {
        fd_t *fd;

        for (fd = core->configs[core->curr_config - 1].fd_list; fd; 
            fd = fd->next)
        {
            if (fd->state == FD_ENABLED && fd->fd_ops && fd->fd_ops->resume)
                fd->fd_ops->resume(fd->context);
        }
    }

    /* Notify application that the core is now resumed */
    jnotify_os(NOTIFY_DEVICE_RESUME_COMPLETED, dcd_bus);

    return 0; 
}

/**
 * Function name:  core_disconnect
 * Description: Handle disconnecting from the bus
 * Parameters: 
 *     @core_ctx: (IN) CORE context
 *
 * Return value: None
 * Scope: global
 **/
void core_disconnect(void *core_ctx)
{
    core_t *core = (core_t *)core_ctx;
    jresult_t rc;

    if ((core->core_state != CORE_ENABLED) &&
        (core->core_state != CORE_CONFIGURED && 
        (core->core_state != CORE_SUSPENDED)))
    {
        DBG_E(DSLAVE_CORE, ("USB Disconnect at wrong Core state %02x\n",
            core->core_state));
    }

    /* Disable REMOTE_WAKEUP capability */
    core->remote_wakeup_enabled = 0;

    rc = chp9_handle_set_configuration(core, 0);
    if (rc)
    {
        DBG_W(DSLAVE_CORE, ("core_unregister_dcd: Failed to set "
            "configuration 0 (%d), continue anyway\n", rc));
    }

    /* Abort control pipe transfer */
    core_abort_pipe(core, NULL);

    core->core_state = CORE_ENABLED;

    DBG_V(DSLAVE_CORE, ("Core: Disconnect %d\n", rc));
}

/**
 * Function name:  core_register_dcd
 * Description: called by DCD to register with the CORE
 * Parameters: 
 *     @dev: (IN) DCD context
 *     @dcd_ops: (IN) DCD callbacks
 *     @core_ctx: (OUT) CORE context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_register_dcd(jdevice_t dev, dcd_ops_t *dcd_ops, void **core_ctx)
{
    core_t *core = NULL;
    jresult_t rc;
    jint_t i;

    DBG_V(DSLAVE_CORE, ("CORE: Registering DCD\n"));
    
    if (global_core_state != CORE_INITIALIZED)
    {
        DBG_E(DSLAVE_CORE, ("Attempt to register DCD at wrong "
            "Core state %02x\n", global_core_state));
        return JEPERM;
    }

    /* Find the first core that has yet to assign dcd_ops */
    for (i = 0; i < ncores; i++)
    {
        if (!cores[i].dcd_ops)
        {
            core = &cores[i];
            break;
        }
    }
    if (!core)
    {
        DBG_E(DSLAVE_CORE, ("Trying to register a DCD to a non-existing "
            "Core\n"));
        rc = JEINVAL;
        goto Error;
    }

    DBG_V(DSLAVE_CORE, ("core_register_dcd: adding core 0x%x\n", core));

    if (core_ctx)
        *core_ctx = core;

/* Temp fix for B47218 */

rc = dev_app_init(core->app_ctx, &(core->app_cbs));
if (rc)
{
  DBG_E(DSLAVE_CORE, ("Cannot init dev app %d\n", rc));
  goto Error;
}

/* Temp fix END */


    core->request = core_request_alloc(MAX_DESCRIPTOR_SIZE, NULL);
    if (!core->request)
    {
        DBG_E(DSLAVE_CORE, ("Cannot allocate memory for Core's request\n"));
        rc = JENOMEM;
        goto Error;
    }
#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    rc = jtask_init(&core->remote_wakeup_task, TASK_CONTROLLER);
    if (rc)
    {
        DBG_E(DSLAVE_CORE, ("Cannot initialize remote wakeup task %d\n", rc));
        goto Error;
    }
#endif
#ifdef CONFIG_REMOTE_WAKEUP_TEST
    rc = jtask_init(&core->remote_wakeup_test_task, TASK_CONTROLLER);
    if (rc)
    {
        DBG_E(DSLAVE_CORE, ("Cannot initialize remote wakeup test task %d\n", 
            rc));
        goto Error;
    }
#endif
    rc = jtask_init(&core->reconfig_task, TASK_CONTROLLER);
    if (rc)
    {
        DBG_E(DSLAVE_CORE, ("Cannot initialize reconfig task %d\n", rc));
        goto Error;
    }

    core->core_state = CORE_DCD_CONFIGURED;
    core->usb_state = USB_ADDRESS;
    core->max_speed = SPEED_HIGH;
    core->dcd_ops = dcd_ops;
    core->dcd_dev = dev;
    core->iad_defined = 0;

    rc = init_fds(core);
    if (rc)
        goto Error;
        
    DBG_V(DSLAVE_CORE, ("CORE: DCD registered ! Core state = %02x\n",
        core->core_state));

    return 0;

Error:
    DBG_E(DSLAVE_CORE, ("CORE: DCD registration with core failed\n"));

    return rc;
}

static void uninit_fds(core_t *core)
{
    fd_t *fd, *temp;
    jint_t i, j, k, l;
    
    for (l = 0; l < core->config_count; l++)
    {
        for (fd = core->configs[l].fd_list; fd; fd = temp)
        {
            /* Free all allocated pipes */
            for (i = 0; i < fd->fd_desc->interface_count; i++)
            {
                /* For each alternate setting */
                for (j = 0; j < fd->fd_desc->interfaces[i].alt_if_num; j++)
                {
                    /* For each endpoint */
                    for (k = 0; 
                        k < fd->fd_desc->interfaces[i].alt_ifs[j].pipe_count; 
                        k++)
                    {
                        pipe_desc_t *pipe = 
                            &fd->fd_desc->interfaces[i].alt_ifs[j].pipes[k];

                        core->dcd_ops->dcd_free_pipe(core->dcd_dev, pipe);
                    }
                }
            }

            if (fd->fd_ops)
                fd->fd_ops->uninit(fd->context);

            temp = fd->next;
            jfree(fd);
        }

        /* Free interfaces */
        if (core->configs[l].interfaces)
            jfree(core->configs[l].interfaces);

        core->configs[l].interface_count = 0;
        core->configs[l].fd_list = NULL;

        if (core->configs[l].i_config)
            core_release_string_descriptor(core, core->configs[l].i_config);
    }

    jfree(core->configs);
    core->configs = NULL;
    core->curr_config = 0;
    core->core_state = CORE_DCD_CONFIGURED;
}

/**
 * Function name:  core_unregister_dcd
 * Description: called by DCD to un-register with the CORE
 * Parameters: 
 *     @core_ctx: (IN) CORE context
 *
 * Return value: None
 * Scope: global
 **/
void core_unregister_dcd(void *core_ctx)
{
    core_t *core = (core_t *)core_ctx;
    jresult_t rc;

    if (!core)
    {
        DBG_E(DSLAVE_CORE, ("Attempt to unregister dcd failed: invalid core "
            "context\n"));
        return;
    }

    rc = chp9_handle_set_configuration(core, 0);
    if (rc)
    {
        DBG_W(DSLAVE_CORE, ("core_unregister_dcd: Failed to set "
            "configuration 0 (%d), continue anyway\n", rc));
    }

    uninit_fds(core);

#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    if (core->remote_wakeup_task)
        jtask_uninit(core->remote_wakeup_task);
#endif
#ifdef CONFIG_REMOTE_WAKEUP_TEST
    if (core->remote_wakeup_test_task)
        jtask_uninit(core->remote_wakeup_test_task);
#endif
    if (core->reconfig_task)
        jtask_uninit(core->reconfig_task);

    if (core->request)
        core_request_free(core->request);

    core->dcd_ops = NULL;
}

static jbool_t check_idx_is_duplicated(uw_hw_core_param_t *hw_core_params, 
    juint8_t num_of_cores)
{
    jint_t i, j;

    for (i = 0; i < num_of_cores; i++)
    {
        for (j = i + 1; j < num_of_cores; j++)
        {
            if (hw_core_params[j].core_number == hw_core_params[i].core_number)
            {
                return 1;
            }
        }
    }

    return 0;
}

/**
 * Function name:  core_init
 * Description: CORE initialization function. Initialize CORE contexts list, and
 *              save argument for later use
 * Parameters: 
 *     @devices: (IN) array of device_params_t
 *     @num_of_devices: (IN) number of elements in <devices>
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t core_init(uw_hw_core_param_t *hw_core_params, juint8_t num_of_cores)
{
    jint_t i;
    core_t *core;
    device_params_t *device_param, *src_device_param; 

    DBG_I(DSLAVE_CORE, ("Starting core init...\n"));

    if (check_idx_is_duplicated(hw_core_params, num_of_cores))
    {
        DBG_E(DSLAVE_CORE, ("core_init: received duplicated controller "
            "indices\n"));
        return JEINVAL;
    }

    ncores = num_of_cores;
    cores = (core_t *)jmalloc(sizeof(core_t) * ncores, M_ZERO);
    if (!cores)
    {
        DBG_E(DSLAVE_CORE, ("core_init: Failed to allocate cores list\n"));
        goto Error;
    }

    for (i = 0; i < ncores; i++)
    {
        jint_t j;

        /* Added by mshaked - host controller doesn't need device parameters */      
        if (hw_core_params[i].init_opt == UW_INIT_HOST)
        {
          continue;
        }
        core = &cores[i];
        /* HSU Addition */
        core->ep0_handler = NULL;
        core->core_index = i;

        core->core_number = hw_core_params[i].core_number;

        src_device_param = hw_core_params[i].device_param;
        
        device_param = &core->device_param;
        device_param->dev_info = (dev_info_t *)jmalloc
            (sizeof(dev_info_t), M_ZERO);
        if (!device_param->dev_info)
            goto Error;
        j_memcpy(device_param->dev_info, src_device_param->dev_info, 
            sizeof(dev_info_t));
        
        device_param->num_of_configs = src_device_param->num_of_configs;
        device_param->config_param = jmalloc(sizeof(config_param_t) *
            device_param->num_of_configs, M_ZERO);
        if (!device_param->config_param)
            goto Error;

        for (j = 0; j < device_param->num_of_configs; j++)
        {
            config_param_t *config_param = &device_param->config_param[j];
            config_param_t *src_config_param =
                &src_device_param->config_param[j];

            DBG_I(DSLAVE_CORE, ("core_init: config #%d\n", j));
            config_param->fd_instances = jmalloc(sizeof(fd_instance_t) * 
                src_config_param->num_of_fd_instances, M_ZERO);
            if (!config_param->fd_instances)
                goto Error;
            j_memcpy(config_param->fd_instances,
                src_config_param->fd_instances, sizeof(fd_instance_t) *
                src_config_param->num_of_fd_instances);

            config_param->num_of_fd_instances = 
                src_config_param->num_of_fd_instances;

            config_param->config_name = src_config_param->config_name;
            config_param->self_powered = src_config_param->self_powered;
            config_param->remote_wakeup = src_config_param->remote_wakeup;
            config_param->max_power = src_config_param->max_power;
        }

#ifndef CONFIG_FULL_SPEED_ONLY
        device_param->full_speed_only = src_device_param->full_speed_only;
#endif
        device_param->vendor_get_string_cb = 
            src_device_param->vendor_get_string_cb;
        device_param->vendor_cmd_in_cb = 
            src_device_param->vendor_cmd_in_cb;
        device_param->vendor_cmd_out_cb = 
            src_device_param->vendor_cmd_out_cb;
        device_param->vendor_cmd_out_data_stage_cb = 
            src_device_param->vendor_cmd_out_data_stage_cb;
        /*HSU addition*/
        device_param->reserved_cmd_in_cb = 
            src_device_param->reserved_cmd_in_cb;
        device_param->reserved_cmd_out_cb = 
            src_device_param->reserved_cmd_out_cb;
        device_param->reserved_cmd_out_data_stage_cb = 
            src_device_param->reserved_cmd_out_data_stage_cb;
        /*End of HSU addition*/

        /* HSU addition - standard_cmd_cb_t */
        device_param->standard_cmd_cb = 
            src_device_param->standard_cmd_cb;

        device_param->static_interface_number = 
            src_device_param->static_interface_number;
    }

    /* Initialization finished */
    global_core_state = CORE_INITIALIZED;

    DBG_V(DSLAVE_CORE, ("Core initialized (%d cores initialized)\n", 
        num_of_cores));

    return 0;

Error:
    core_uninit();
    return JENOMEM;
}

/**
 * Function name:  core_uninit
 * Description: Uninitialized the CORE
 * Parameters: 
 *
 * Return value: None
 * Scope: global
 **/
void core_uninit(void)
{
    jint_t i;
    device_params_t *device_param;

    DBG_I(DSLAVE_CORE, ("Starting core un-init...\n"));
  
    /* Run as long as there are still elements in the cores list */
    for (i = 0; i < ncores; i++)
    {
        jint_t j;

        device_param = &cores[i].device_param;
        for (j = 0; j < device_param->num_of_configs; j++)
        {
            if (device_param->config_param[j].fd_instances)
                jfree(device_param->config_param[j].fd_instances);
        }
        jfree(device_param->config_param);
        if (device_param->dev_info)
            jfree(device_param->dev_info);
    }

    if (cores)
    {
        jfree(cores);
        cores = NULL;
        ncores = 0;
    }

    global_core_state = CORE_NOT_INITIALIZED;
}

static jresult_t core_enable(core_t *core)
{
    jresult_t rc;
    jbool_t full_speed_dcd;
  
    if (!core)
    {
        DBG_E(DSLAVE_CORE, ("CORE: Cannot enable core, invalid controller "
            "index\n"));
        return JEINVAL;
    }
    
    DBG_I(DSLAVE_CORE, ("core_enable: core = 0x%x (index %d, number %d), "
        "state %d\n", core, core->core_index, core->core_number, 
        core->core_state));
        
    if (core->core_state != CORE_CONFIGURED)
    {
        switch (core->core_state)
        {
        case CORE_ENABLED:
            return 0;
            
        case CORE_DCD_CONFIGURED:
            DBG_E(DSLAVE_CORE, ("CORE: Attempt to enable core before FD(s) "
                "was registered\n"));
            break;
            
        case CORE_FD_CONFIGURED:
            DBG_E(DSLAVE_CORE, ("CORE: Attempt to enable core before "
                "register_finish() was called\n"));
            break;

        default:
            DBG_E(DSLAVE_CORE, ("CORE: Attempt to enable core at unknown "
                "state !\n"));
            break;
        }
        
        return JEPERM;
    }

    /* If an FD is using an IAD, the device descriptor should be adjusted
     * accordingly to allow it to work */
    if (core->iad_defined)
    {
        dev_info_t *dev_info = core->device_param.dev_info;

        DBG_I(DSLAVE_CORE, ("CORE: using IAD compliant DEVICE DESCRIPTOR\n"));

        dev_info->dev_class     = USB_CLASS_MISC;
        dev_info->dev_sub_class = USB_MISC_SUBCLASS_COMMON;
        dev_info->dev_protocol  = USB_MISC_SUBCLASS_COMMON_PRTCL_IAD;
    }

    chp9_set_string_descriptors(core);

    /* Change core state */
    core->core_state = CORE_ENABLED;
    
    /* Enable the DCD */
#ifdef CONFIG_FULL_SPEED_ONLY
    full_speed_dcd = 1;
#else
    full_speed_dcd = core->device_param.full_speed_only;
#endif
    rc = core->dcd_ops->dcd_enable(core->dcd_dev, full_speed_dcd);
    if (rc)
        return rc;

    /* NOTE: FDs are enabled when SET_CONFIG(1) arrives */
    DBG_V(DSLAVE_CORE, ("Core enabled !\n"));

    return 0;
}

juint8_t core_get_curr_configuration(juint8_t core_number)
{
    core_t *core;

    core = find_core_by_idx(core_get_controller_idx(core_number));
    if (!core)
    {
        DBG_E(DSLAVE_CORE, ("CORE: Cannot get configuartion, invalid "
            "controller index\n"));
        return 0;
    }

    return core->curr_config;
}

juint8_t core_get_controller_idx(juint8_t core_number)
{
    juint8_t i;

    for (i = 0; i < ncores; i++)
    {
        if (cores[i].core_number == core_number)
            break;
    }

    return i;
}


/**
 * Function name:  core_enable_by_idx
 * Description: Called by application to enable a specific controller
 * Parameters: 
 *     @core_index: (IN) index (zero based) of the controller to enable
 *
 * Return value: 0 on success, otherwise an error code  
 * Scope: global
 **/
jresult_t core_enable_by_idx(juint8_t core_index)
{
    return core_enable(find_core_by_idx(core_index));
}

/**
 * Function name:  core_enable_by_ctx
 * Description: Called by DCD to enable a specific controller
 * Parameters:
 *     @core_ctx: (IN) CORE context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t core_enable_by_ctx(void *core_ctx)
{
    return core_enable((core_t *)core_ctx);
}

static void core_disable(core_t *core)
{
    jresult_t rc = 0;
    
    if (!core)
    {
        DBG_E(DSLAVE_CORE, ("CORE: Cannot disable core, invalid controller "
            "index\n"));
        return;
    }

    DBG_V(DSLAVE_CORE, ("CORE: Disabling core, core index = %d core number = "
        "%d\n", core->core_index, core->core_number));
    if (core->core_state != CORE_CONFIGURED && core->core_state != CORE_ENABLED 
        && core->core_state != CORE_SUSPENDED)
    {
        DBG_W(DSLAVE_CORE, ("Attempt to disable the core in wrong state." 
            " %02x\n", core->core_state));
        return;
    }

    /* Disable all FDs */
    rc = chp9_handle_set_configuration(core, 0);
    if (rc)
    {
        DBG_W(DSLAVE_CORE, ("core_disable: Failed to set configuration 0 (%d), "
            "continue anyway\n", rc));
    }
  
    /* Disable the DCD */
    core->dcd_ops->dcd_disable(core->dcd_dev);
 
    /* Change core state */
    core->core_state = CORE_CONFIGURED;
     
    DBG_V(DSLAVE_CORE, ("Core (index = %d  number = %d) disabled !\n", 
        core->core_index, core->core_number));
}

/**
 * Function name:  core_disable_by_idx
 * Description: Called by application to disable a specific controller
 * Parameters: 
 *     @core_index: (IN) index (zero based) of the controller to enable
 *
 * Return value: None
 * Scope: global
 **/
void core_disable_by_idx(juint8_t core_index)
{
    core_disable(find_core_by_idx(core_index));
}

/**
 * Function name:  core_disable_by_ctx
 * Description: Called by DCD to disable a specific controller
 * Parameters:
 *     @core_ctx: (IN) CORE context
 *
 * Return value: None
 * Scope:          Global
 **/
void core_disable_by_ctx(void *core_ctx)
{
    core_disable((core_t *)core_ctx);
}

static void core_reconfig_task(void *arg)
{
    jint_t i;
    jresult_t rc = 0;
    core_t *core = (core_t *)arg;
    device_params_t *args = &core->reconfig_args.device_param;
    device_params_t *device_param = &core->device_param;

    /* Update list of fd_instances for current Core */
    for (i = 0; i < device_param->num_of_configs; i++)
        jfree(device_param->config_param[i].fd_instances);

    jfree(device_param->config_param);
    device_param->config_param = args->config_param;
    device_param->num_of_configs = args->num_of_configs;

    /* Unconfigure the device */
    rc = chp9_handle_set_configuration(core, 0);
    if (rc)
    {
        DBG_W(DSLAVE_CORE, ("core_reconfig_task: Failed to set configuration "
            "0 (%d), continue anyway\n", rc));
    }

    /* HSU addition */
    /* Zero string descriptors array */
    chp9_clear_string_descriptors(core);
    /* End of HSU addition*/

    /* Disable and uninitialize current FD's */
    uninit_fds(core);

    /* Re-init the FD's according to new configuration */
    rc = init_fds(core);
    if (rc)
        goto Exit;

    /* Save dev_info */
    jfree(device_param->dev_info);
    device_param->dev_info = args->dev_info;

    core->core_state = core->reconfig_args.state_before_reconfig;
    if (core->core_state == CORE_ENABLED)
        chp9_set_string_descriptors(core);

    /* Reconnect to the host */
    if (core->dcd_ops->vbus_connect)
        core->dcd_ops->vbus_connect(core->dcd_dev);

Exit:                                                       

    DBG_I(DSLAVE_CORE, ("core_reconfig_task: Finish reconfigure the stack - "
        "status 0x%x\n", rc));

    core->reconfig_inprogress = 0;
    if (core->reconfig_args.cb)
        core->reconfig_args.cb(core->reconfig_args.arg, rc);
}

jresult_t core_reconfig(juint8_t core_index, device_params_t *new_config,
    reconfig_complete_cb_t cb, void *arg)
{
    jint_t i;
    core_t *core = find_core_by_idx(core_index);
    reconfig_arg_t *reconfig_args = NULL;
    jresult_t rc = 0;

    if (!core)
    {
        DBG_E(DSLAVE_CORE, ("CORE: Trying to reconfigure a non-existing "
            "controller index - %d", core_index));
        rc = JEINVAL;
        goto Exit;
    }

    if (core->reconfig_inprogress)
    {
        DBG_E(DSLAVE_CORE, ("CORE: Reconfigure is already in progress\n"));
        rc = JEINVAL;
        goto Exit;
    }

    reconfig_args = &core->reconfig_args;
    reconfig_args->device_param.num_of_configs = new_config->num_of_configs;
    reconfig_args->device_param.config_param = (config_param_t *)jmalloc
        (sizeof(config_param_t) * new_config->num_of_configs, M_ZERO);
    if (!reconfig_args->device_param.config_param)
    {
        DBG_E(DSLAVE_CORE, ("CORE: Failed allocate new config_param array\n"));
        rc = JENOMEM;
        goto Exit;
    }

    for (i = 0; i < new_config->num_of_configs; i++)
    {
        reconfig_args->device_param.config_param[i].fd_instances =
            (fd_instance_t *)jmalloc(sizeof(fd_instance_t) *
            new_config->config_param[i].num_of_fd_instances, M_ZERO);
        if (!reconfig_args->device_param.config_param[i].fd_instances)
        {
            DBG_E(DSLAVE_CORE, ("CORE: Failed allocate new fd_instances array\n"));
            rc = JENOMEM;
            goto Exit;
        }
    }

    reconfig_args->device_param.dev_info = (dev_info_t *)jmalloc
        (sizeof(dev_info_t), M_ZERO);
    if (!reconfig_args->device_param.dev_info)
    {
        DBG_E(DSLAVE_CORE, ("CORE: Failed allocate new device info\n"));
        rc = JENOMEM;
        goto Exit;
    }
    reconfig_args->state_before_reconfig = core->core_state;

    core->reconfig_inprogress = 1;

    core->ep0_handler = NULL;

    /* Disconnect from the host */
    /* HSU Addition */
    if (core->dcd_ops)
    if (core->dcd_ops->vbus_disconnect)
        core->dcd_ops->vbus_disconnect(core->dcd_dev);

    /* Run the reconfigure process in a generic task to avoid infinite recursive
     * FD's disable */
    /* Copy the new configuration parameters to the core's context */
    for (i = 0; i < new_config->num_of_configs; i++)
    {
        j_memcpy(reconfig_args->device_param.config_param[i].fd_instances,
            new_config->config_param[i].fd_instances, sizeof(fd_instance_t) *
            new_config->config_param[i].num_of_fd_instances);
        reconfig_args->device_param.config_param[i].num_of_fd_instances = 
            new_config->config_param[i].num_of_fd_instances;
        reconfig_args->device_param.config_param[i].config_name =
            new_config->config_param[i].config_name;
        reconfig_args->device_param.config_param[i].self_powered =
            new_config->config_param[i].self_powered;
        reconfig_args->device_param.config_param[i].remote_wakeup =
            new_config->config_param[i].remote_wakeup;
        reconfig_args->device_param.config_param[i].max_power =
            new_config->config_param[i].max_power;
    }

    j_memcpy(reconfig_args->device_param.dev_info, new_config->dev_info, 
        sizeof(dev_info_t));

    core->reconfig_args.cb = cb;
    core->reconfig_args.arg = arg;

    jtask_schedule(core->reconfig_task, CORE_RECONFIG_DELAY_MS, core_reconfig_task, core);

Exit:
    if (rc && reconfig_args && reconfig_args->device_param.config_param)
    {
        for (i = 0; i < new_config->num_of_configs &&
            reconfig_args->device_param.config_param[i].fd_instances; i++)
        {
            jfree(reconfig_args->device_param.config_param[i].fd_instances);
        }

        jfree(reconfig_args->device_param.config_param);
    }

    return rc;
}

/**
 * Function name:  get_controller_index
 * Description:   Return the Controller index of the specific core instance
 * Parameters:
 *     @core_ctx: (IN) CORE context
 *
 * Return value:  The specific core's controller index 
 * Scope:          Global
 **/
juint8_t core_get_controller_index(void *core_ctx)
{
    return ((core_t *)core_ctx)->core_index;
}

/* HSU Addition */
/**
* Function name:  core_set_ep0_handler
* Description:   Registers a handler for an EP0 handler.
*                It allows external entity (i.e., BREW layer) to register 
*                a handler for EP0.
* Parameters:
*     @core_index: (IN) CORE index
*     @ep0_handler: (IN) handler for EP0.
* Return value:   0 on success, otherwise an error code  
* Scope:          Global
**/
jresult_t core_set_ep0_handler(juint8_t core_index, 
                               jresult_t (*ep0_handler)(core_t *core, 
                               void *buffer,juint8_t ep0_req_tag))
{
  core_t *core = find_core_by_idx(core_index);
  core->ep0_handler = ep0_handler;
  return 0;
}

#ifdef FEATURE_HS_USB_POLLING
/**
* Function name:  core_set_fd_ops
* Description:   
* Parameters:
* Return value:   
* Scope:          Global
**/
void core_set_fd_ops(void *core_ctx, fd_ops_t *fd_ops)
{
  core_t *core = (core_t *)core_ctx;
  juint8_t i;

  for (i=0; i < core->config_count; i++)
  {
    fd_t *fd;

    for (fd = core->configs[i].fd_list; fd; fd = fd->next)
    {
      fd->fd_ops = fd_ops;
    }
  }
}

/**
* Function name:  core_get_core_context
* Description:   
* Parameters:
* Return value:   
* Scope:          Global
**/

void *core_get_core_context(uint8 core_idx)
{
   void * ret;

   ret = find_core_by_idx(core_idx);

   return ret;
}

/**
* Function name:  core_get_pipe_desc
* Description:   
* Parameters:
* Return value:   
* Scope:          Global
**/
pipe_desc_t *core_get_pipe_desc(void *core_ctx, juint8_t direction, juint8_t ep_num)
{

  core_t *core = (core_t *)core_ctx;
  interface_desc_t * infs;
  juint8_t inf_cnt;
  juint8_t i;
  alt_interface_desc_t *alt_infs;
  juint8_t alt_inf_cnt;
  juint8_t j;
  pipe_desc_t *pipes;
  juint8_t pipe_cnt;
  juint8_t k;

  if (core->curr_config)
  {
    fd_t *fd;
    for (fd = core->configs[core->curr_config - 1].fd_list; fd; fd = fd->next)
    {
      infs = fd->fd_desc->interfaces;
      inf_cnt = fd->fd_desc->interface_count;
      for(i=0; i < inf_cnt; i++)
      {
        alt_infs = infs[i].alt_ifs;
        alt_inf_cnt = infs[i].alt_if_num;
        for(j=0; j < alt_inf_cnt; j++)
        {
          pipes = alt_infs[j].pipes;
          pipe_cnt = alt_infs[j].pipe_count;
          for(k=0; k < pipe_cnt; k++)
          {
            if(( pipes[k].direction == (unsigned)direction) &&
               ((pipes[k].address & 0x0F) == ep_num))
            {
              return &pipes[k];
            }
          }
        }
      }
    }
  }


  return NULL;
}

/**
* Function name:  core_is_configured
* Description:   
* Parameters:
* Return value:   
* Scope:          Global
**/
boolean core_is_configured(void *core_ctx)
{
  core_t *core = (core_t *)core_ctx;
  boolean ret = FALSE;

  if((core->core_state == CORE_ENABLED) && 
     (core->usb_state == USB_CONFIGURED))
  {
    ret = TRUE;
  }
  
  return ret;
}
#endif /* FEATURE_HS_USB_POLLING */

void core_clear_reconfig_flag(juint8_t core_index)
{
    core_t *core = find_core_by_idx(core_index);

    core->reconfig_inprogress = 0;
}

/* HSU Addition */

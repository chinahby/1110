/*==============================================================================

                      High Speed USB Sample Loopback Function Driver
  
GENERAL DESCRIPTION
Implementation of loopback function Driver.
This includes implementation of the API functions defined in hsu_fd_sample_loop.h.
      
Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
$Header: 
$DateTime: 


when      who     what, where, why
--------  ---     ----------------------------------------------------------
06/01/08  tk      created file

==============================================================================*/
#include <jusb_common.h>
#include <jusb_core_fd.h>
#include <jusb_chp9.h>

#define MAX_BUFFER_SIZE 512

#define DEV_CLASS       USB_CLASS_VENDOR_SPEC 
#define DEV_SUBCLASS    USB_CLASS_VENDOR_SPEC
#define DEV_PROTOCOL    0 

typedef enum {
    STATE_DISABLED = 0,
    STATE_ENABLED
} loopback_state_t;

static void *core_ctx;
static pipe_desc_t *in_pipe, *out_pipe;
static loopback_state_t state = STATE_DISABLED;
static loopback_state_t p_state = STATE_DISABLED;
static request_t *request=NULL;

/* Function prototypes */
static jresult_t control_msg(void *context, void *buffer);
static void send_complete(request_t *request);
static void read_complete(request_t *request);

/* Structs used to pass desired pipe configuration to core */
static pipe_desc_t pipe_desc_array[] = {
    {
        /* max_pkt_size_high            */ 512,
        /* max_pkt_size_full            */ 64,
        /* type                         */ PIPE_BULK,
        /* direction                    */ DIRECTION_OUT,
        /* sync_type                    */ 0,
        /* usage_type                   */ 0,
        /* transaction_per_microframe   */ 0,
        /* pipe                         */ 0, /* Filled by Core */
        /* nak_rate                     */ { 0 }
    }, 
    {
        /* max_pkt_size_high            */ 512,
        /* max_pkt_size_full            */ 64,
        /* type                         */ PIPE_BULK,
        /* direction                    */ DIRECTION_IN,
        /* sync_type                    */ 0,
        /* usage_type                   */ 0,
        /* transaction_per_microframe   */ 0,
        /* pipe                         */ 0, /* Filled by Core */
        /* nak_rate                     */ { 0 } 
    }
};

static alt_interface_desc_t alt_desc_array[] = {
    {
        /* pipe_count                   */ 2,
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
        /* if_protocol                  */ DEV_PROTOCOL
    }
};

static fd_desc_t fd_desc = {
    /* speed            */ SPEED_HIGH, 
    /* current speed    */ SPEED_UNKNOWN, 
    /* interface_count  */ 1,
    /* interfaces       */ if_desc_array
};

static jresult_t post_read(void)
{
    jresult_t rc = 0;

    request->complete           = read_complete;
    request->transfer_size      = MAX_BUFFER_SIZE;

    rc = core_read_data(core_ctx, out_pipe, request);
    if (rc)
    {
        DBG_E(DSLAVE_LOOP, ("FD: Error posting a read request. FD is now "
            "useless !\n"));
    }

    return rc;
}

static jresult_t post_send(uint32_t size)
{
    jresult_t rc = 0;

    request->complete           = send_complete;
    request->transfer_size      = size;

    rc  = core_send_data(core_ctx, in_pipe, request);
    if (rc)
    {
        DBG_E(DSLAVE_LOOP, ("FD: Error writing data back. FD is now "
            "useless !\n"));
    }
    
    return rc;
}

void send_complete(request_t *request)
{
    request_status_t status = request->status;

    request->status = REQUEST_READY;

    if (status != REQUEST_COMPLETED || 
        request->transfer_size != request->bytes_transferred)
    {
        DBG_W(DSLAVE_LOOP, ("FD: Error sending the packet. Sent %d out "
            "of %d!\n", request->bytes_transferred, request->transfer_size));
        
        /* Try sending again */
        post_send(request->transfer_size);
    }
    else
    {
        DBG_I(DSLAVE_LOOP, ("FD: Out packet got sent ! Give us more !\n"));     
        post_read();
    }   
}

void read_complete(request_t *request)
{
    request_status_t status = request->status;

    request->status = REQUEST_READY;

    if (status != REQUEST_COMPLETED)
    {
        DBG_W(DSLAVE_LOOP, ("FD: We did not get a packet like we wanted !\n"));
    }
    else
    {
        DBG_I(DSLAVE_LOOP, ("FD: Got packet sized %d and now sending it back\n",
            request->bytes_transferred));       
        post_send(request->bytes_transferred);
    }
}

static jresult_t control_msg(void *context, void *buffer)
{
    /* We don't know how to handle EP0 messages */
    return -1;
}

static fd_desc_t *get_pipe_config(context_t context)
{
    return &fd_desc;
}

static jresult_t enable(context_t context)
{
    jresult_t rc = 0;
    fd_desc_t *fd = (fd_desc_t *)context;

    /* Get the pipe numbers that core assigned to our descriptors */
    out_pipe = &fd->interfaces[0].alt_ifs[0].pipes[0];
    in_pipe  = &fd->interfaces[0].alt_ifs[0].pipes[1];

    /* Allocate a request */
    request = core_request_alloc(MAX_BUFFER_SIZE, NULL); 
    if (!request)
        return JENOMEM;
    
    /* Post a read request */
    rc = post_read();
    if (rc)
        return rc;

    state = STATE_ENABLED;
    return 0;
}

static void disable(context_t context)
{
     if (request)
     {
       core_request_free(request);
       request = NULL;
     }

    if(state == STATE_ENABLED)
    {
      core_abort_pipe(core_ctx, out_pipe);
      core_abort_pipe(core_ctx, in_pipe);
    }

    state = STATE_DISABLED;
}

static jresult_t uninit(context_t context)
{
   if (request)
   {
     core_request_free(request);
     request = NULL;
   }
  
    return 0;
}

static jresult_t suspend(context_t context)
{
    p_state = state;
    if(state == STATE_ENABLED)
    {
      disable(context);
    }
    return 0;
}

static jresult_t resume(void *context)
{
    jresult_t rc = 0;

     if(p_state == STATE_ENABLED)
     {
        rc = enable(context);
     }
     
    return rc;
}


static fd_ops_t fd_ops = {
/*  get_pipe_config     */ get_pipe_config,
/*  enable              */ enable,
/*  disable             */ disable,      
/*  suspend    	        */ suspend,
/*  resume              */ resume,
/*  uninit              */ uninit
};

jresult_t hsu_fd_sample_loopback_init(void *ctx)
{
    core_ctx = ctx;
    return core_register_fd(core_ctx, &fd_ops, (context_t)&fd_desc);
}


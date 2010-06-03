/*==============================================================================

                      High Speed USB MTP Function Driver
  
GENERAL DESCRIPTION
Implementation of MTP Function Driver.
This includes implementation of the API functions defined in mtp_fd.h and uw_device_mtp.h.
      
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/hsu_fd/mtp/hsu_mtp_fd.c#6 $
$DateTime: 2009/04/02 10:21:06 $ $Author: tkwon $    




when      who     what, where, why
--------  ---     ----------------------------------------------------------
08/31/07   dk      Removed featurization with CONFIG_PROTO_MTP flag
                   for registering MS OS string descriptor
06/29/07   bt      Integrate MTP (IUSBDriver) feature into Main Line
    
==============================================================================*/
#include "hsu_common.h"
#include <jusb_common.h>
//#include <jusb_core_fd.h>
//#include <jusb_chp9.h>
#include "mtp_fd.h"
#include "mtp_fd_api.h"
#include "hsu_conf_sel_types.h"
#include "hsu_conf_sel_i.h"

#define MTP_DEV_CLASS       0xFF
#define MTP_DEV_SUBCLASS    0xFF
#define MTP_DEV_PROTOCOL    0xFF 

#define MTP_USB_PACKET_SIZE_HIGH    0x0200
#define MTP_USB_PACKET_SIZE_FULL    0x0040

#define MSOS_MTP_VENDOR_CODE          0xFE

#define MTP_CTRL_CANCEL_TRANSACTION    0x64
#define MTP_CTRL_GET_EX_EVENT_DATA     0x65
#define MTP_CTRL_DEVICE_RESET          0x66
#define MTP_CTRL_GET_DEV_STATUS        0x67

#define MTP_MAX_CONTROL_READ_SIZE 512

#define MTP_EXT_COMPT_ID_OS_DESC_TYPE 0x0004
#define MTP_EXT_PROP_OS_DESC_TYPE     0x0005

static mtpfd_callbacks_t *mtpfd_app_cbs;      /* Pointer to the OEMlayer call back functions  */

/* Function prototypes */
static jresult_t mtpfd_control_msg(void *context, void *buffer);
/* callback for IN transfer completed */
static void mtpfd_write_bulk_complete_cb(request_t *request);
/* callback for INTR transfer completed */
static void mtpfd_write_intr_complete_cb(request_t *request);
/* callback for OUT transfer completed */
static void mtpfd_read_bulk_complete_cb(request_t *request);

static void mtpfd_control_read_complete(request_t *req);

static void mtpfd_control_write_complete(request_t *req);


/* Structs used to pass desired pipe configuration to core */  
static pipe_desc_t mtp_pipe_desc_array[] = {
    {
        MTP_USB_PACKET_SIZE_HIGH,   /* max_pkt_size_high                    */
        MTP_USB_PACKET_SIZE_FULL,   /* max_pkt_size_full                    */
        PIPE_BULK,                  /* type                                 */
        DIRECTION_OUT,              /* direction                            */
        0,                          /* sync_type                            */
        0,                          /* usage_type                           */
        0,                          /* transaction_per_microframe           */
        0,                          /* pipe: Filled by the Core             */
        { 0 },                      /* nak_rate                             */
        0,                          /* extra_descriptor_size                */
        NULL,                       /* extra_descriptor                     */
        0,                          /* poll_interval_high                   */
        NULL,                       /* fd_handle                            */
        NULL,                       /* dcd_handle                           */
        0,                          /* Packet size for current speed        */
        PIPE_UNKNOWN,               /* pipe status                          */
        {NULL, NULL}                /* requests queue                       */
    },
    {
        MTP_USB_PACKET_SIZE_HIGH,   /* max_pkt_size_high                    */
        MTP_USB_PACKET_SIZE_FULL,   /* max_pkt_size_full                    */
        PIPE_BULK,                  /* type                                 */
        DIRECTION_IN,               /* direction                            */
        0,                          /* sync_type                            */
        0,                          /* usage_type                           */
        0,                          /* transaction_per_microframe           */
        0,                          /* pipe: Filled by the Core             */
        { 0 },                      /* nak_rate                             */
        0,                          /* extra_descriptor_size                */
        NULL,                       /* extra_descriptor                     */
        0,                          /* poll_interval_high                   */
        NULL,                       /* fd_handle                            */
        NULL,                       /* dcd_handle                           */
        0,                          /* Packet size for current speed        */
        PIPE_UNKNOWN,               /* pipe status                          */
        {NULL, NULL}                /* requests queue                       */
    },
    {
        MTP_USB_PACKET_SIZE_HIGH,   /* max_pkt_size_high                    */
        MTP_USB_PACKET_SIZE_FULL,   /* max_pkt_size_full                    */
        PIPE_INTR,                  /* type                                 */
        DIRECTION_IN,               /* direction                            */
        0,                          /* sync_type                            */
        0,                          /* usage_type                           */
        0,                          /* transaction_per_microframe           */
        0,                          /* pipe: Filled by the Core             */
        { 5 },                      /* nak_rate: 5ms in FS, 2ms in HS       */
         0,                          /* extra_descriptor_size                */
        NULL,                       /* extra_descriptor                     */
        0,                          /* poll_interval_high                   */
        NULL,                       /* fd_handle                            */
        NULL,                       /* dcd_handle                           */
        0,                          /* Packet size for current speed        */
        PIPE_UNKNOWN,               /* pipe status                          */
        {NULL, NULL}                /* requests queue                       */
   }
};

static alt_interface_desc_t mtp_alt_desc_array[] = {
    {
        3,                          /* pipe_count                               */
        mtp_pipe_desc_array,        /* pipes                                    */
		NULL,						/* extra descriptor 		*/
		0							/* extra descriptor size	*/
    }
};

static interface_desc_t mtp_if_desc_array[] = {
    {
        mtpfd_control_msg,          /* control_msg                             */
        mtp_alt_desc_array,         /* alt_ifs                                 */
        1,                          /* alt_if_num                              */
        0,                          /* alt_if_curr                             */
        0,                          /* extra_descriptor_size                   */
        0,                          /* extra_descriptor                        */
        UWD_UNDEFINED_INTERFACE,    /* number: to be filled by the core        */
        MTP_DEV_CLASS,              /* if_class                                */
        MTP_DEV_SUBCLASS,           /* if_subclass                             */
        MTP_DEV_PROTOCOL,           /* if_protocol                             */
        0                           /* if_string                               */
    }
};

static fd_desc_t fd_mtp_desc = {
    SPEED_HIGH,                     /* speed                                   */
    SPEED_UNKNOWN,                  /* current speed                           */
    1,                              /* interface_count                         */
    mtp_if_desc_array,              /* interfaces                              */
    NULL                            /* struct usb_interface_assoc_descriptor*  */
};


/* Main MTP FD structure            */
typedef struct _mtpfd_ctx_t
{
    
    void *core_ctx;               /* Core context                                       */
    fd_ops_t   fd_ops;            /* CORE callbacks                                    */
    fd_desc_t *fd_desc;           /* Descriptor for this FD                            */

    pipe_desc_t *in_pipe,         /* BULK IN pipe                                        */
                *out_pipe,        /* BULK OUT pipe                                     */
                *intr_pipe;       /* INTR IN pipe                                        */

    request_t *ctrl_read,          /* Preallocated request for control reads   */
              *ctrl_write,         /* Preallocated request for control writes  */
              *in_request,         /* Used to check if a request is in progress*/
              *intr_request;       /* Used to check if a request is in progress*/

    juint32_t usb_max_packet_size; /* MAX packet Size of Bulk pipe                */

    mtpfd_states_t  state;         /* Current state                                       */
    mtpfd_states_t  prev_state;    /* State before suspend                            */
    
    jbool_t         is_started;    /* Set true when enabled & not suspended */
    jbool_t         is_enabled;    /* Set true when USB enum is complete     */

    mtpfd_callbacks_t* app_cbs;    /* Holds call back functions from OEM layer */

    jbool_t mtpfd_device_connected;/* Saves the current dis/connected state    */

    struct usb_ctrlrequest curr_setup; /* data received in setup stage */
    
}mtpfd_ctx_t;


static void *mtp_ep0_data_ptr = NULL; /* data received in data stage */

static uint32 mtp_ep0_data_max_size = 0; /* maximum size of the data in ep0 data stage */

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

/*=========================================================*/
/*     Utility Functions                                                                                                     */
/*=========================================================*/

/*===========================USB GENERAL FUNCTIONS=============*/
/*             mtpfd_abort_read                                                                                          */
/*             mtpfd_abort_write                                                                                          */
/*             mtpfd_stall_in_pipe                                                                                        */
/*             mtpfd_stall_out_pipe                                                                                      */
/*             mtpfd_stop_io                                                                                                */
/*             mtpfd_ioctl                                                                                                    */
/*=========================================================*/


/**************************************************************************
 * Function name:  mtpfd_abort_read 
 * Description:    Aborts pending read requests (BULK OUT EP).
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
jresult_t mtpfd_abort_read(mtpfd_handle_t handle)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*) handle;

    if (!ctx->out_pipe)
        return JENXIO;

    core_abort_pipe(ctx->core_ctx, ctx->out_pipe);
    return 0;
}

/**************************************************************************
 * Function name:  mtpfd_abort_write
 * Description:    Aborts pending read requests.
 * Parameters: 
 *     @fd: (IN) FD context, pipe type BULK/INTR
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
jresult_t mtpfd_abort_write(mtpfd_handle_t handle, pipe_type_t type)
{
  mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)handle;

     if(type == PIPE_BULK)
     {
        if (!ctx->in_pipe)
          return JENXIO;
        core_abort_pipe(ctx->core_ctx, ctx->in_pipe);
     }
     else
     {
         if (type == PIPE_INTR)
         {
            if (!ctx->intr_pipe)
              return JENXIO;
            core_abort_pipe(ctx->core_ctx, ctx->intr_pipe);
         }   
     }     
    return 0;
}

/**************************************************************************
 * Function name:  mtpfd_stall_in_pipe
 * Description:    Stalls the IN data pipe.
 * Parameters: 
 *     @fd: (IN) FD context
 *     @type: (IN) pipe type
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
void mtpfd_stall_in_pipe(mtpfd_handle_t handle, pipe_type_t type)
{
     mtpfd_ctx_t *ctx = (mtpfd_ctx_t *) handle;
     
     if(type == PIPE_BULK)
     {
        core_stall_pipe(ctx->core_ctx, ctx->in_pipe, 1);
     }
     else if (type == PIPE_INTR)
     {
        core_stall_pipe(ctx->core_ctx, ctx->intr_pipe, 1);
     }
}

/**************************************************************************
 * Function name:  mtpfd_stall_out_pipe
 * Description:    Stalls the OUT data pipe.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
void mtpfd_stall_out_pipe(mtpfd_handle_t handle)
{
    mtpfd_ctx_t * ctx = (mtpfd_ctx_t *)handle;
    core_stall_pipe(ctx->core_ctx, ctx->out_pipe, 1);
}

/**************************************************************************
 * Function name:  mtpfd_stop_io
 * Description:    Cancels the current USB I/O request.
 * Parameters: 
 *     @fd:       (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
void mtpfd_stop_io(mtpfd_handle_t handle)
{
    mtpfd_ctx_t * ctx = (mtpfd_ctx_t *)handle;

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    if (!ctx->is_started)
        return;

    /* Abort active transfers                                        */
    if (ctx->out_pipe)
        core_abort_pipe(ctx->core_ctx, ctx->out_pipe);

    if (ctx->in_pipe)
        core_abort_pipe(ctx->core_ctx, ctx->in_pipe);

    if (ctx->intr_pipe)
        core_abort_pipe(ctx->core_ctx, ctx->intr_pipe);

    ctx->is_started = 0;
}

/**************************************************************************
 * Function name:  mtpfd_flush_in_pipe
 * Description:    Cancels the current IN request for BULK/INTR
 * Parameters: 
 *     @fd:       (IN) FD context, Pipe type, flush call back from the applayer
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
void mtpfd_flush_in_pipe(mtpfd_handle_t handle, mtpfd_pipe_type_t type, 
                            mtpfd_flush_in_pipe_cb FlushTxCB)
{
   mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)handle;

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif 

   if (type == MTPFD_PIPE_BULK)
   {
       if(ctx->in_request)
        {
          if (ctx->in_request->status == REQUEST_INPROGRESS)
             mtpfd_app_cbs->tx_flush_h = FlushTxCB;
        }  
       else
        {
          if (FlushTxCB)
          {
               if (ctx->in_pipe)
                    core_abort_pipe(ctx->core_ctx, ctx->in_pipe);
               FlushTxCB();
          }
        }  
   }
   if (type == MTPFD_PIPE_INTR)
   {
        if (ctx->intr_request)
        {
            if (ctx->intr_request->status == REQUEST_INPROGRESS)
              mtpfd_app_cbs->txInt_flush_h = FlushTxCB;
        }   
        else
        {
          if (FlushTxCB)
          {
               if (ctx->intr_pipe)
                    core_abort_pipe(ctx->core_ctx, ctx->intr_pipe);
               FlushTxCB();
           }
        }  
    }
}

/**************************************************************************
 * Function name:  mtpfd_get_iostatus
 * Description:  translate CORE request status to MTPFD defined status 
 * Parameters: 
 *     @req: (IN) CORE request 
 *     @check_size: (IN) Should size be checked for a mismatch 
 *
 * Return value: CDC status 
 * Scope: global
 **************************************************************************/
mtpfd_iostatus_t mtpfd_get_iostatus(const request_t *req, jbool_t check_size)
{
   switch(req->status)
    {
        case REQUEST_COMPLETED:
           if (check_size && (req->bytes_transferred != req->transfer_size))
           {
               HSU_MSG_ERROR("mtpfd_get_iostatus: size error: bytes xfered:%d: xfer_size: %d", 
                             req->bytes_transferred, req->transfer_size, 0);
               return MTP_TRANSFER_ERROR;
           }
           return MTP_TRANSFER_OK;

        case REQUEST_CANCELLED:
           HSU_MSG_ERROR("mtpfd_get_iostatus: cancelled: bytes xfered:%d: xfer_size: %d", 
                         req->bytes_transferred, req->transfer_size, 0);

           return MTP_TRANSFER_CANCELLED;

        case REQUEST_ERROR:
         
        default:
           HSU_MSG_ERROR("mtpfd_get_iostatus: error: bytes xfered:%d: xfer_size: %d", 
                         req->bytes_transferred, req->transfer_size, 0);
           return MTP_TRANSFER_ERROR;
    }
}

/**************************************************************************
 * Function name:  mtpfd_ioclt
 * Description:   IOCTL 
 * Parameters:
 *     @request: (IN) Core request
 *
 * Return value:   None
 * Scope:          Local
 **************************************************************************/
void mtpfd_ioctl(mtpfd_handle_t handle, 
                  mtpfd_pipe_type_t pipe, 
                  mtpfd_ioctl_command_type command,
                  void *command_param)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)handle;

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif 

    switch (command) 
     {
      case MTPFD_ABORT_READ:              /* Abort an uncompleted read     */
        {
          (void) mtpfd_abort_read(handle); 
        }
        break;
      case MTPFD_ABORT_WRITE:             /* Abort an uncompleted write     */
        {
          (void) mtpfd_abort_write(handle, (pipe_type_t)pipe);
        }
        break;   

      case MTPFD_GET_CONNECT_STATUS:       /* Retrieve the connect status   */
       {
          *((uint32*)command_param) = (uint32)ctx->mtpfd_device_connected;
       }    
       break;     

      case MTPFD_SET_CONNECT_STATUS_CB:    /* Register callback to notify     */
       {                                   /* connect/disconnect  event     */
         if (command_param)
           mtpfd_app_cbs->cnxn_state_chng_h = (mtpfd_cnxn_status_cb)command_param;
       }
       break;
          
      case MTPFD_CLEAR_CONNECT_STATUS_CB: 
        {
           mtpfd_app_cbs->cnxn_state_chng_h = NULL;
        }
        break;
     
      default:
            break;
     }
}

/*========================================================*/
/*     Callacks to the core                                                                                            */
/*========================================================*/


/*==========================================================*/
/*    Transfer data over  USB IN/OUT  data  endpoints/pipes                                              */
/*==========================================================*/

/**************************************************************************
 * Function name:  mtpfd_read_complete_cb
 * Description:  mtpfd_post_read() completion callback
 * Parameters: 
 *     @req: (IN) CORE request
 *
 * Scope: local
  **************************************************************************/
void mtpfd_read_bulk_complete_cb(request_t *req)
{
    mtpfd_ctx_t* ctx = (mtpfd_ctx_t*)req->context;
    
    jbool_t is_short_packet;
    mtpfd_iostatus_t status = mtpfd_get_iostatus(req, 0);
   
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif 
    *((juint16_t*)req->caller_private) = (juint16_t)req->bytes_transferred;

    /* check if the host sent a ZLP and the reason for the end of transfer is a short ZLP */
    if ((req->bytes_transferred ==0)&& (req->transfer_size >0))
    {
       (void) mtpfd_post_read((void*) ctx,
                         req->buffer.vaddr,
                         (juint16_t)req->transfer_size,
                         req->caller_private);
       core_request_free(req);
    }
    else
    {
       if (status != MTP_TRANSFER_ERROR)
       {
           /* We signal the application that the transfer is complete when a short
            * packet is recieved from the host. That is when either:
            *      1. The bytes transferred are less than requested
            *      2. The bytes_transferred == transfer_size, and the transfer size is
            *          NOT a multiple of (USB) max packet size
            *         (When it is a multiple of max packet size, we expect the application
            *           to call mtpfd_read again for which we will call it with 0 bytes
            *          transferred and a TRUE transfer complete flag)                          */
            
         is_short_packet = (req->bytes_transferred < req->transfer_size) ||
                          (req->transfer_size % ctx->usb_max_packet_size);
    
         if (is_short_packet)
         {
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif             
         }
         

         if (mtpfd_app_cbs->rx_complete_h)
         {
            /* Invoke call back from the App layer USBDRIVER_USBDCRxCB      */
            mtpfd_app_cbs->rx_complete_h(MTPFD_PIPE_BULK);
         }
       }
       else
       {
    
           HSU_MSG_ERROR("mtpfd_read_bulk_complete_cb: rx error", 0,0,0);
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif
     
           if (mtpfd_app_cbs->rx_error_h)
           {
              /* Invoke call back from the App layer USBDRIVER_USBDCRxErrCB       */
              mtpfd_app_cbs->rx_error_h();
           }
       }
      core_request_free(req);
    }  

}

/**************************************************************************
 * Function name:  mtp_write_bulk_complete_cb
 * Description:  mtp_post_write() completion callback
 * Parameters: 
 *     @req: (IN) CORE request
 *
  **************************************************************************/
void mtpfd_write_bulk_complete_cb(request_t *req)
{
    mtpfd_ctx_t* ctx = (mtpfd_ctx_t*)req->context;
    mtpfd_iostatus_t status;
       
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    status =  mtpfd_get_iostatus(req, 1);
    if (status != MTP_TRANSFER_ERROR)
    {
       if (mtpfd_app_cbs->tx_complete_h)
       {
          /* Invoke call back from the OEM layer USBDRIVER_USBDCTxCB   */
          mtpfd_app_cbs->tx_complete_h();
       }
       if ((ctx->in_request)&& (mtpfd_app_cbs->tx_flush_h))
       {
          /* Invoke call back from the OEM layer USBDriver_FlushTxCB          */
          mtpfd_app_cbs->tx_flush_h();
          mtpfd_app_cbs->tx_flush_h = NULL;
          ctx->in_request = NULL;
       }
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif               
    }
    else
    {
       if (mtpfd_app_cbs->tx_error_h)
       {
          /* Invoke call back from the OEM layer USBDRIVER_USBDCTxErrCB*/
           mtpfd_app_cbs->tx_error_h();
           HSU_MSG_ERROR("mtpfd_write_bulk_complete_cb: tx_error", 0,0,0);
       }
    }
    core_request_free(req);
}

/**************************************************************************
 * Function name:  mtp_write_intr_complete_cb
 * Description:  mtp_post_write() completion callback
 * Parameters: 
 *     @req: (IN) CORE request
 *
  **************************************************************************/
void mtpfd_write_intr_complete_cb(request_t *req)
{
    mtpfd_ctx_t* ctx = (mtpfd_ctx_t*)req->context;
    mtpfd_iostatus_t status;


#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    status =  mtpfd_get_iostatus(req, 1);
     if (status != MTP_TRANSFER_ERROR)
     {
       if (mtpfd_app_cbs->txInt_complete_h)
       {
          /* Invoke call back from the OEM layer USBDRIVER_USBDCTxCB       */
          mtpfd_app_cbs->txInt_complete_h();
       }
       
       if ((ctx->intr_request)&& (mtpfd_app_cbs->txInt_flush_h))
       {
          /* Invoke call back from the OEM layer USBDriver_FlushTxIntCB         */
          mtpfd_app_cbs->txInt_flush_h();
          mtpfd_app_cbs->txInt_flush_h = NULL;
          ctx->intr_request = NULL;
       }
     }
     else
     {
       if (mtpfd_app_cbs->txInt_error_h)
       {
          /* Invoke call back from the App layer USBDRIVER_USBDCTxIntErrCB */
           mtpfd_app_cbs->txInt_error_h();
       }
     }
     core_request_free(req);
}

/**************************************************************************
 * Function name:  mtpfd_post_read
 * Description:    Sends a read request to the host.
 * Parameters: 
 *     @ctx:       (IN) FD context
 *     @size:     (IN) Buffer size, in bytes
 *     @buf_id:   (IN) Buffer index (zero based) for handling double buffering
 *     @callback: (IN) Completion callback
 *     @context:  (IN) Context for the completion callback
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Global
 **************************************************************************/
int mtpfd_post_read(mtpfd_handle_t handle, 
                              void *buffer,
                              uint16 buffer_size,
                              uint16 *size)
{

    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)handle;
    request_t *dataout_req = NULL;
    jresult_t rc = 0;

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    do {
        if (!ctx->out_pipe)
        {
           HSU_MSG_ERROR("mtpfd_post_read: pipe not enabled for this intfc & setting", 0, 0,0);
           rc = JEPERM;
           break;
        }

        dataout_req = core_request_alloc(buffer_size, buffer);
        if (!dataout_req)
        {
          HSU_MSG_ERROR("mtpfd_post_read:failed to allocate request", 0, 0,0);
          rc = JENOMEM;
          break;
        }

        *size = (juint16_t) 0;
        dataout_req->transfer_size = (((juint32_t)buffer_size) <<16) >> 16;
        dataout_req->complete = mtpfd_read_bulk_complete_cb; 
        dataout_req->context = handle;
        
        dataout_req->caller_private = (void*)size;
        dataout_req->status = REQUEST_READY;
        
        rc = core_read_data(ctx->core_ctx, ctx->out_pipe, dataout_req);
        
        if (rc)
        {
           HSU_MSG_ERROR("mtpfd_post_read: Error posting request. FD now useless", 0,0,0);     
           break;
        }

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

        return rc;
    }while (0);
    
    /* Error condition (rc != 0) */
    
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    if (dataout_req)
        core_request_free(dataout_req);

    return rc;  
}

/**************************************************************************
 * Function name:  mtpfd_post_write
 * Description:    Sends data to the host.
 * Parameters: 
 *     @ctx:       (IN) FD context 
 *     @size:     (IN) Buffer size, in bytes
 *     @buf_id:   (IN) Buffer index (zero based) for handling double buffering
 *     @callback: (IN) Completion callback
 *     @context:  (IN) Context for the completion callback
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Global
 *
 **************************************************************************/
int mtpfd_post_write(mtpfd_handle_t handle, void *buffer,
                                  uint16 size, mtpfd_pipe_type_t type)
{
    jresult_t rc = 0;
    request_t *datain_req = NULL;
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)handle;

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    do {
        if (((type == MTPFD_PIPE_BULK)&& (!ctx->in_pipe)) ||  /* Check if pipe exists */
           ((type == MTPFD_PIPE_INTR) && (!ctx->intr_pipe)))
        {
            HSU_MSG_ERROR("mtpfd_post_write:in pipe is not enabled for this interface & setting", 0,0,0);
            rc = JEPERM;
            break;
        }

        datain_req = core_request_alloc(size, buffer);
        if (!datain_req)
        {
           HSU_MSG_ERROR("mtpfd_post_write:failed to allocate request", 0,0,0);
           rc = JENOMEM;
           break;
        }
       
        datain_req->transfer_size = (((juint32_t)size)<<16)>>16;
        datain_req->context = handle;
        datain_req->status = REQUEST_READY;
        
        if (type == MTPFD_PIPE_BULK)
        {
          datain_req->complete = mtpfd_write_bulk_complete_cb; 
          ctx->in_request = datain_req;   /* Used later to check if requests in progress */
          rc  = core_send_data(ctx->core_ctx, ctx->in_pipe, datain_req);
        }
        else
        {
          datain_req->complete = mtpfd_write_intr_complete_cb; 
          ctx->intr_request = datain_req; /* Used later to check if requests in progress */
          rc  = core_send_data(ctx->core_ctx, ctx->intr_pipe, datain_req);
        }
        
        if (rc < 0)
        {
             HSU_MSG_ERROR("mtpfd_post_write:Error writing data back. FD is now useless", 0,0,0);
             break;
        }
        return rc;
        
    }while (0);
    
    /* Error condition */
    return rc;  
}


/*========================================================================*/
/*     Control Request handling                                                                                                                         */
/*========================================================================*/

/**************************************************************************
 * Function name:  mtpfd_ep0_read_complete
 * Description: callback funciton for read operation on EP0
 * Parameters: the request given to the read operation.
 * Return value: none 
 * Scope: local
 **************************************************************************/
static void mtpfd_ep0_read_complete(request_t *req)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)req->context;

    /* Check Request status */

    if (req->status != REQUEST_COMPLETED ||
        req->bytes_transferred != req->transfer_size)
    {
        req->status = REQUEST_READY;
        return;
    }

    req->status = REQUEST_READY;

    if((mtp_ep0_data_ptr) && (req->bytes_transferred <= mtp_ep0_data_max_size))
    {
      memcpy(mtp_ep0_data_ptr, 
             req->buffer.vaddr, 
             req->bytes_transferred);
    }
    else
    {
       HSU_MSG_ERROR("ep0 data buffer was not configured properly.", 
                      0,0,0); 
       HSU_ASSERT(0);
    }

    if(mtpfd_app_cbs->setup_msg_h)
    {
      mtpfd_app_cbs->setup_msg_h(&ctx->curr_setup);
    }
    
}

/**************************************************************************
 * Function name:  mtpfd_ep0_read
 * Description:  start to read on EP0 for data stage
 * Parameters: 
     ctx: (IN) MTP FD context.
     setup: The data from setup stage.

 * Return value: none 
 * Scope: local
 **************************************************************************/
static jresult_t mtpfd_ep0_read(mtpfd_ctx_t *ctx,
    struct usb_ctrlrequest *setup)
{
    jresult_t rc = 0;
    request_t *req = ctx->ctrl_read;
   
    req->complete       = mtpfd_ep0_read_complete;
    req->transfer_size  = setup->wLength;
    req->context        = ctx;
    memcpy(&ctx->curr_setup, setup, sizeof(struct usb_ctrlrequest));

    rc = core_read_data(ctx->core_ctx, NULL, req);

    return rc;
}

/**************************************************************************
 * Function name:  mtpfd_control_msg
 * Description:  callback function for EP0 setup stage.
 * Parameters: 
     context: (IN) MTP FD context.
     buffer: The data from setup stage.
 * Return value: indicate error. 
 * Scope: local
 **************************************************************************/
static jresult_t mtpfd_control_msg(void *context, void *buffer)
{
    struct usb_ctrlrequest *setup = (struct usb_ctrlrequest*)buffer;
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)context;
    jresult_t rc = -1;
    void *os_desc;
    const hsu_conf_sel_comp_info_type *comp;

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif
    
    if (mtpfd_app_cbs->setup_msg_h)
    {
      switch(setup->bRequest)
      {
      case MTP_CTRL_CANCEL_TRANSACTION:
          rc = mtpfd_ep0_read(ctx, setup);
          break;

      case MTP_CTRL_DEVICE_RESET:
      case MTP_CTRL_GET_DEV_STATUS:
      case MTP_CTRL_GET_EX_EVENT_DATA:
      case MSOS_MTP_VENDOR_CODE:
      default:
          mtpfd_app_cbs->setup_msg_h(setup);
          rc = 0;
          break;
      }
    }
    else
    {
      switch(setup->bRequest)
      {
      case MSOS_MTP_VENDOR_CODE:
        if( setup->wIndex == MTP_EXT_COMPT_ID_OS_DESC_TYPE )
        {
          /* get current compostion information to find corresponding MS OS
           * descriptor. 
           */
          comp = hsu_conf_sel_get_cur_comp_details();
          
          os_desc = comp->ms_os_descriptor;
          
          /* send response with the MS OS descriptor. */
          if( os_desc )
             rc = mtpfd_control_write(context, (void*)os_desc, setup->wLength);
        }
        else
        {
          core_stall_pipe(ctx->core_ctx, 0, 1);
        }
        break;

       default:
          break;
      }
    }

    if(!rc) 
    {
      return rc;
    }

   return rc;
}


/**************************************************************************
 * Function name:  mtpfd_create_request
 * Description:  create (or reallocate) a CORE request
 * Parameters: 
 *	   @p_req: (IN/OUT) CORE request, or pointer to a NULL request pointer
 *	   @buffer: (IN) request data (for write requests. NULL for read requests)
 *	   @size: (IN) size of data in bytes (size of buffer, in case of read)
 *
 * Return value: CORE request, or NULL on failure 
 * Scope: local
 **************************************************************************/
static request_t* mtpfd_create_request(request_t **p_req,
                                           void *buffer, juint32_t size)
{
    request_t *req = NULL;

    HSU_PARAM_COULD_BE_CONST(buffer);

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    req = *p_req;

    do {
        /* Make sure we do not overlap requests */
        if (req && req->status == REQUEST_INPROGRESS)
        {
           HSU_MSG_ERROR("overlapping-trying to allocate req while still in progress", 0,0,0);
           break;
        }

        /*   If this request is being reused, and has been flagged with an error or  cancel, */
        /*   we should flag it as ready before using it                                                     */
        
        if (req)
            req->status = REQUEST_READY;

        if (!req || (req->buffer.buffer_size < size))
        {
            request_t *new_req;
        
           /* Don't try to allocate 0 bytes...   set it atleast 10 bytes */
           new_req = core_request_alloc(size ? size : 10, NULL);
           if (!new_req)
           {
              HSU_MSG_ERROR("mtpfd_create_request: error allocating ep0 request", 0,0,0);
              break;
           }
           
           if (req)
             core_request_free(req);

           req = *p_req = new_req;
        }

        if (buffer && size)
           (void) j_memcpy(req->buffer.vaddr, buffer, size);
        
        req->zero = 0; /* Don't let core send ZLP automatically */
        req->transfer_size = size;
        
        return req;
    } while (0);

    /* Error condition */
    return NULL;
}

/**************************************************************************
 * Function name:  mtpfd_create_ep0_read
 * Description:  allocate a CORE request for a CONTROL read
 * Parameters: 
 *     @ctx: (IN) MTPFD context
 *     @size: (IN) size of buffer
 *
 * Return value: CORE request, or NULL on failure 
 * Scope: global
 **************************************************************************/
request_t* mtpfd_create_ep0_read(mtpfd_ctx_t *ctx,  void *buffer,
                                                         juint32_t size)
{
    if (!ctx)
    {
        HSU_MSG_ERROR("mtpfd_create_ep0_read: no context provided", 0,0,0);
        return NULL;
    }

    return mtpfd_create_request(&ctx->ctrl_read, buffer, size);
}

/**************************************************************************
 * Function name:  mtpfd_create_ep0_write
 * Description:  allocate a CORE request for a CONTROL write
 * Parameters: 
 *     @ctx: (IN) MTPFD context
 *     @size: (IN) size of buffer
 *
 * Return value: CORE request, or NULL on failure 
 * Scope: global
 **************************************************************************/
request_t* mtpfd_create_ep0_write(mtpfd_ctx_t *ctx, void *buffer, 
                                                       juint32_t size)
{
    if (!ctx)
    {
        HSU_MSG_ERROR("mtpfd_create_ep0_write: no context provided", 0,0,0);
        return NULL;
    }

    return mtpfd_create_request(&ctx->ctrl_write, buffer, size);
}

/**************************************************************************
 * Function name:  mtpfd_send_setup_status
 * Description:  issue a status packet by sending a zero length packet on ep0
 * Parameters: 
 *     @ctx: (IN) MTPFD context
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **************************************************************************/
jresult_t mtpfd_send_setup_status(mtpfd_ctx_t *ctx)
{
    jresult_t rc = -1;
    request_t *response;

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    response = mtpfd_create_ep0_write(ctx, NULL, 0);
    if (!response)
    {
       HSU_MSG_ERROR("mtpfd_send_setup_status:failed to setup status request", 0,0,0);
       return rc;
    }

    response->complete = mtpfd_control_write_complete;
    response->context  = ctx; /* Save MTPFD ctx so that we could get it back */

    rc = core_send_data(ctx->core_ctx, 0, response);
    if (rc < 0)
        HSU_MSG_ERROR("mtpfd_send_setup_status:failed to send response, status", 0,0,0);
            
    return rc;
}

/*===========================================================================

FUNCTION mtpfd_control_read_complete

DESCRIPTION
completion handler for the SETUP request with DATA (OUT) phase.

DEPENDENCIES
mtpfd_init() must have been called before this function is used.

PARAMETERS
request_t * req [IN and OUT]: request structure used to send control data.

RETURN VALUE
None.

SIDE EFFECTS
req->status gets updated for next use.

===========================================================================*/
static void mtpfd_control_read_complete(request_t *req)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)req->context;
    mtpfd_iostatus_t status;
    jresult_t rc;

    HSU_PARAM_COULD_BE_CONST(req);

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif    

    do {
        status = mtpfd_get_iostatus(req, 1);
        if (status != MTP_TRANSFER_OK)
            break;

        if (req->status != REQUEST_COMPLETED)
        {
            HSU_MSG_ERROR("mtpfd_control_read_complete: failed receiving ctl "
                          "data through ep0: status=%d", req->status,0,0);
            break;
        }
        
        req->status = REQUEST_READY;

        rc = mtpfd_send_setup_status(ctx);
        if (rc)
        {
            HSU_MSG_ERROR("mtpfd_control_read_complete:failed to send status packet", 0,0,0);
            break;
        }

        return;
    } while (0);
    
    /* Error condition */
    core_stall_pipe(ctx->core_ctx, 0, 1);
    
}
/**************************************************************************
 * Function name:  mtpfd_control_read
 * Description:  send response on ep0
 * Parameters: 
 *     @ctx: (IN) CDC context
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **************************************************************************/
int mtpfd_control_read(mtpfd_handle_t handle, void *buffer, 
                                                       uint32 size)
{
    jresult_t rc = -1;
    request_t *response =NULL;
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)handle; 
  
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    do {
        /* Setup reply                                              */
        response = mtpfd_create_ep0_read(ctx, buffer, size);
        if (!response)
        {
            HSU_MSG_ERROR("mtpfd_control_read:failed to setup reply", 0,0,0);
            break;
        }

        response->complete = mtpfd_control_read_complete;
        response->context = handle;
        
        rc = core_read_data(ctx->core_ctx, 0, response);

        return rc;
    } while (0);

    /* Error condition */
    return rc;
}

/*===========================================================================

FUNCTION mtpfd_control_write_complete

DESCRIPTION
completion handler for control data write (IN).

DEPENDENCIES
mtpfd_init() must have been called before this function is used.

PARAMETERS
request_t * req [IN and OUT]: request structure used to send control data.

RETURN VALUE
None.

SIDE EFFECTS
req->status gets updated for next use.

===========================================================================*/
static void mtpfd_control_write_complete(request_t *req)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)req->context;

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif    

    core_request_free(req);
    
    ctx->ctrl_write = NULL;
    
}

/**************************************************************************
 * Function name:  mtpfd_control_write
 * Description:  send response on ep0
 * Parameters: 
 *     @ctx: (IN) MTPFD context
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **************************************************************************/
int mtpfd_control_write(mtpfd_handle_t handle, void *buffer, 
                                                       uint16 size)
{
    jresult_t rc = -1;
    request_t *response;
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)handle; 
  
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    response = mtpfd_create_ep0_write(ctx, buffer, size);
    if (!response)
    {
        HSU_MSG_ERROR("mtpfd_control_write:failed to setup status req", 0,0,0);
        return rc;
    }

    response->complete = mtpfd_control_write_complete;
    response->context = ctx;

    rc = core_send_data(ctx->core_ctx, 0, response);
    if (rc < 0)
       HSU_MSG_ERROR("mtpfd_control_write:failed to send response", 0,0,0);
    
    return rc;
}

/**************************************************************************
 * Function name:  mtpfd_update_connect_status
 * Description:  Updates mtpfd_device_connected status and notifies the OEM layer of 
 *                    any change int eh connect status
 * Parameters: 
 *     @ctx: (IN) MTPFD context,  updated boolean value of mtpfd_device_connected
 *
 * Return value: 0 for success, or an error code 
 * Scope: global
 **************************************************************************/
static void mtpfd_update_connect_status(context_t context,
                                               boolean connected)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t *)context;
    
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    /* Update the connection status only if it changed. */
    if ( ctx->mtpfd_device_connected != connected )
    {
      /* Update and store current connect status before notifying client */
      ctx->mtpfd_device_connected = connected;
  
      /* Notify the client that connection status changed */
      if (mtpfd_app_cbs->cnxn_state_chng_h)
      {
         /* Invoke call back from the OEM layer                     */
         mtpfd_app_cbs->cnxn_state_chng_h();
      }
    }
}


/*=====================REGISTER FUNCTIONS TO CORE=============*/
/*             mtpfd_get_pipe_config                                                                              */
/*             mtpfd_enable                                                                                           */
/*             mtpfd_disable                                                                                           */
/*             mtpfd_suspend                                                                                         */
/*             mtpfd_resume                                                                                           */
/*             mtpfd_unint                                                                                              */
/*=======================================================*/


/**************************************************************************
 * Function name:  mtpfd_get_pipe_config
 * Description:    Returns brew extended device driver's USB
 * descriptors.
 * Parameters:
 *     @context: (IN) FD context
 *
 * Return value: FD descriptor
 * Scope:        Local
 **************************************************************************/
static fd_desc_t *mtpfd_get_pipe_config(context_t context)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t *)context;
    
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    return ctx->fd_desc;
}

static jresult_t mtpfd_enable(context_t context)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t *)context;
    
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

    /* Get the pipe numbers that core assigned to our descriptors */
    if (!ctx->is_enabled)
    {
      /* Get the pipe numbers that core assigned to our descriptors */
      ctx->out_pipe = &ctx->fd_desc->interfaces[0].alt_ifs[0].pipes[0];
      ctx->in_pipe = &ctx->fd_desc->interfaces[0].alt_ifs[0].pipes[1];
      ctx->intr_pipe = &ctx->fd_desc->interfaces[0].alt_ifs[0].pipes[2];

      ctx->usb_max_packet_size = (ctx->fd_desc->current_speed == SPEED_HIGH)
         ? MTP_USB_PACKET_SIZE_HIGH : MTP_USB_PACKET_SIZE_FULL;

      ctx->is_enabled = 1;
    }
    
    ctx->state = MTP_STATE_ENABLED;
    ctx->prev_state = ctx->state;

    if (mtpfd_app_cbs->enabled_h)
    {
       /* Call the call back from the App layer */
       mtpfd_app_cbs->enabled_h();
    }

    /* To start up Teleca MTP application here */
    if (mtpfd_app_cbs->mtp_app_launch_h)
    {
       mtpfd_app_cbs->mtp_app_launch_h(TRUE);  /* Launch(TRUE) MTP background application */
    }

    /* Update and store current connect status to connected   ......*/
    mtpfd_update_connect_status(context, TRUE); 
  
    ctx->is_started = 1;    
    return 0;
}


static void mtpfd_disable(context_t context)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)context;

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif    

    if (!ctx->is_enabled)
        return;

    mtpfd_stop_io(ctx);
    ctx->out_pipe = NULL;
    ctx->in_pipe = NULL;
    ctx->intr_pipe = NULL;
    ctx->is_enabled = 0;

    if (mtpfd_app_cbs->disabled_h)
    {
       /* Call the call back from the App layer */
       mtpfd_app_cbs->disabled_h();
    }
    
    /* When cable is unplugged, we need to close MTP app */
    if (mtpfd_app_cbs->mtp_app_launch_h)
    {
       mtpfd_app_cbs->mtp_app_launch_h(FALSE);  /* Close (FALSE) MTP background application */
    }
    
    /* Update and store current connect status to not connected      */
    mtpfd_update_connect_status(context, FALSE);
    ctx->state = MTP_STATE_DISABLED;
}

static jresult_t mtpfd_suspend(context_t context)
{

    mtpfd_ctx_t *ctx = (mtpfd_ctx_t *)context;
    
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif    

    ctx->prev_state = ctx->state;     /* Save state before suspending*/
    if (ctx->state == MTP_STATE_ENABLED)
        mtpfd_disable(ctx);

    if (mtpfd_app_cbs->suspend_h)
    {
        /* Call the call back from the App layer */
        mtpfd_app_cbs->suspend_h();
    }

    if (mtpfd_app_cbs->mtp_app_launch_h)
    {
       mtpfd_app_cbs->mtp_app_launch_h(FALSE);  /* Close (FALSE) MTP background application */
    }
           
    return 0;
}


static jresult_t mtpfd_resume(context_t context)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)context;
    jresult_t rc = 0;
    
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif    

    if (ctx->prev_state == MTP_STATE_ENABLED)
    {
        rc = mtpfd_enable(context);
        if (rc)
            HSU_MSG_HIGH("mtpfd_resume:failed to resume", 0,0,0);
    }
    
    if (mtpfd_app_cbs->resume_h)
    {
        /* Invoke call back from the OEM layer                        */
        mtpfd_app_cbs->resume_h();
    }
    
    /* Update and store current connect status to connected  .....*/
    mtpfd_update_connect_status(context, TRUE);

    ctx->is_started = 1;

    return rc;
}

/**************************************************************************
 * Function name:  mtpfd_free_fd_desc
 * Description: Free MTP FD descriptor and dynamically allocated substructures.
 * Parameters: fd_desc_t -- function driver descriptor pointer
 *
 * Return value: None
 * Scope:        Local
 **************************************************************************/
static void mtpfd_free_fd_desc(fd_desc_t * fd_desc)
{

  if (fd_desc)
  {
    if (fd_desc->interfaces)
    {    
      if (fd_desc->interfaces->alt_ifs)
      {  
        if (fd_desc->interfaces->alt_ifs->pipes)
        {
          jfree(fd_desc->interfaces->alt_ifs->pipes);
        }

        jfree(fd_desc->interfaces->alt_ifs);
      }

      jfree(fd_desc->interfaces);
    }    

    jfree(fd_desc);
  }
  
}


static jresult_t mtpfd_uninit(context_t context)
{
    mtpfd_ctx_t *ctx = (mtpfd_ctx_t*)context;
   
#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif    

    /* Free requests */
    if (ctx->ctrl_read)
    {
        core_request_free(ctx->ctrl_read);
        ctx->ctrl_read = NULL;
    }

    if (ctx->ctrl_write)
    {
        core_request_free(ctx->ctrl_write);
        ctx->ctrl_write = NULL;
    }

    /* Free MTP FD descriptor */
    mtpfd_free_fd_desc(ctx->fd_desc);
    ctx->fd_desc = NULL;

    jfree(ctx);
    return 0;
}

/*========================================================================*/
/*                    FUNCTION DRIVER INITIALIZATION                      */
/*========================================================================*/

/**************************************************************************
 * Function name:  mtpfd_copy_fd_desc
 * Description:    Allocate all FD descriptors and copies them into
 *                 one structure.
 * Parameters: 
 *
 * Return value: FD descriptor
 * Scope:        Local
 **************************************************************************/
static fd_desc_t *mtpfd_copy_fd_desc(void)
{
    pipe_desc_t         *pipe = NULL;
    alt_interface_desc_t *alt = NULL;
    interface_desc_t    *iface = NULL;
    fd_desc_t           *desc = NULL;

    do {
        pipe = (pipe_desc_t*)jmalloc(sizeof(mtp_pipe_desc_array), 0);
        if (!pipe)
            break;

        (void) j_memcpy(pipe, mtp_pipe_desc_array, sizeof(mtp_pipe_desc_array));

        alt = (alt_interface_desc_t*)jmalloc(sizeof(mtp_alt_desc_array), 0);
        if (!alt)
            break;

        (void) j_memcpy(alt, mtp_alt_desc_array, sizeof(mtp_alt_desc_array));

        iface = (interface_desc_t*)jmalloc(sizeof(mtp_if_desc_array), 0);
        if (!iface)
            break;

        (void) j_memcpy(iface, mtp_if_desc_array, sizeof(mtp_if_desc_array));
        
        desc = (fd_desc_t*)jmalloc(sizeof(fd_mtp_desc), 0);
        if (!desc)
            break;

        (void) j_memcpy(desc, &fd_mtp_desc, sizeof(fd_mtp_desc));
        
        alt->pipes = pipe;
        iface->alt_ifs = alt;
        desc->interfaces = iface;

        return desc;
    } while (0);

    /* Error condition */
    if (pipe)
        jfree(pipe);

    if (alt)
        jfree(alt);

    if (iface)
        jfree(iface);

    if (desc)
        jfree(desc);

    return NULL;
}

/**************************************************************************
 * Function name:  mtpfd_init
 * Description:    Initializes the USB function driver.
 * Parameters: 
 *     @ctx: (IN) Core context
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Global
 **************************************************************************/
jresult_t mtpfd_init(void *core_ctx)
{
     jresult_t rc;
     mtpfd_ctx_t *mtpfd_ctx;
     mtpfd_handle_t mtpfd_client_handle;  /* Handle returned to the mtpfd client:
                                                                           * hsu_al layer                                 */

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif      

     mtpfd_ctx = (mtpfd_ctx_t*)jmalloc(sizeof(mtpfd_ctx_t), M_ZERO);

     do {
          if (!mtpfd_ctx)
          {
            rc = JENOMEM;
            HSU_MSG_ERROR("mtpfd_init:failed to allocate mtpfd context", 0,0,0);
            return JENOMEM;
          }
          
          mtpfd_ctx->core_ctx = core_ctx;               /* Point to core                   */
          mtpfd_ctx->state = MTP_STATE_DISABLED;        /* Device not yet enabled    */
          mtpfd_ctx->prev_state = MTP_STATE_DISABLED;
          mtpfd_ctx->mtpfd_device_connected = FALSE;    /* Device not yet connected */

          mtpfd_ctx->fd_desc = mtpfd_copy_fd_desc();    /* Get descriptor information*/
          if (!mtpfd_ctx->fd_desc)
          {
             rc = JENOMEM;
             HSU_MSG_ERROR("mtpfd_init:failed to allocate mtpfd context", 0,0,0);
             break;
          }

          /* Fill in the standard fd_ops functions that core needs */
          if (!mtpfd_ctx->fd_ops.get_pipe_config)
              mtpfd_ctx->fd_ops.get_pipe_config = mtpfd_get_pipe_config;
          if (!mtpfd_ctx->fd_ops.enable)
              mtpfd_ctx->fd_ops.enable = mtpfd_enable;
          if (!mtpfd_ctx->fd_ops.disable)
              mtpfd_ctx->fd_ops.disable = mtpfd_disable;
          if (!mtpfd_ctx->fd_ops.suspend)
              mtpfd_ctx->fd_ops.suspend = mtpfd_suspend;
          if (!mtpfd_ctx->fd_ops.resume)
              mtpfd_ctx->fd_ops.resume = mtpfd_resume;
          if (!mtpfd_ctx->fd_ops.uninit)
              mtpfd_ctx->fd_ops.uninit = mtpfd_uninit;

          mtpfd_app_cbs = mtpfd_ctx->app_cbs;
      
          core_add_msos_string_descriptor(core_ctx, MSOS_MTP_VENDOR_CODE);

          mtpfd_client_handle = (mtpfd_handle_t)mtpfd_ctx;

          rc = brewext_app_init(mtpfd_client_handle, &(mtpfd_ctx->app_cbs));

          if (rc)
          {
             HSU_MSG_ERROR("application failed to initialize ", 0,0,0);
             break;
          }
          
          mtpfd_app_cbs = mtpfd_ctx->app_cbs;

          mtpfd_ctx->ctrl_read = core_request_alloc(MTP_MAX_CONTROL_READ_SIZE, NULL);
          
          /* Resigter FD to  core     */
          return core_register_fd(mtpfd_ctx->core_ctx, &mtpfd_ctx->fd_ops, (context_t)mtpfd_ctx);
     } while (0);

     /* Error condition */
     jfree(mtpfd_ctx);
     return JENOMEM;    
}

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
 #endif


/**************************************************************************
* Function name:  mtpfd_set_setup_msg_data_buffer
* Description: configure the memory buffer for data in data stage on EP0    
* Parameters: 
*     buffer: (IN) The pointer of buffer
*     size: (IN) The size of the buffer
* Return value: None
* Scope:        Global
**************************************************************************/
void mtpfd_set_setup_msg_data_buffer(
  void *buffer, 
  uint32 size)
{
   mtp_ep0_data_ptr = buffer;
   mtp_ep0_data_max_size = size;
}


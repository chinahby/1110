/*==============================================================================

                      High Speed USB Brew Extended Device Adaptation Layer
  
GENERAL DESCRIPTION
Implementation of Brew Extended Adaptation Layer.
This includes implementation of the API functions defined in hsu_al_brewext.h.
      
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
    $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_brewext.c#12 $
    $DateTime: 2009/04/02 10:21:06 $ $Author: tkwon $ 
when       who     what, where, why
--------  ---     ----------------------------------------------------------
06/29/07   bt      Integrate MTP (IUSBDriver) feature into Main Line

==============================================================================*/


/*==============================================================================

  INCLUDE FILES FOR MODULE
  
==============================================================================*/

#include "customer.h" /* For FEATURE_HSU_TEST to work */
#include "hsu_common.h"
#include "err.h"

#include "jusb_common.h"

#include "hsu_conf_sel_comp_utils.h"  
#include "hsu_conf_sel_i.h"  /* Dynamic composition switching */
#include "hsu_conf_sel_ui.h" /* Dynamic composition switching */

#include "OEMUSBDriver.h"
#include "hsu_al_brewext.h"

#include "rdevmap.h"

#include "rex.h"
#include "ui.h"
#include "task.h"
#include "AEE_OEM.h"
#include "AEEShell.h"
#include "mtp_fd.h"

/*==============================================================================
                   Brewext  private data
==============================================================================*/
typedef struct
{
    
    void *client_h; /* BREW Extended Device client handle. Used for callback */
   
    /* save the original composition to restore later */
    hsu_conf_sel_composition_id_enum old_hsu_comp_id; 

    brewext_notify_cb_t brewext_cb;
    /* indicates dynamic comp switching is finished. */          
    boolean comp_change_done; 

    mtpfd_callbacks_t app_cbs;

    void *mtpfd_client_h;
 
} brewext_app_ctx_t;

/* Declare it to be static will also ensure everything is initialized to be NULL. */
static brewext_app_ctx_t brewext_app_ctx;

static enum {
  CONNECT_IDLE, /* ready to connect or disconnect */
  CONNECT_START, /* connect or disconnect is in progress */
  CONNECT_WAIT_DISCONNECT, /* disconnet after connect is finished */
  CONNECT_WAIT_CONNECT /* connect after disconnect is finished */
} connect_state = CONNECT_IDLE;

/*==============================================================================
                   Brewext  private functions
==============================================================================*/

/*==============================================================================
                     Brew Extended Device  API implementations
==============================================================================*/

boolean hsu_al_brewext_handle_mode_change(brewext_rdm_change_status_t status, boolean appClosing)
{
    boolean ret = FALSE;
    
    switch(status)
     {
       case BREW_RDM_DONE:
        /* Ignore subsequent RDM_DONE_S notifications after the initial one.
                  Calling hsu_conf_sel_ui_rdm_done() multiple times will result in
                   ERROR FATAL */
      
         if (TRUE == brewext_app_ctx.comp_change_done)
         {
            ret = TRUE;
            break;
         }
          
         if(hsu_conf_sel_ui_rdm_done() == FALSE)
         {
            HSU_MSG_ERROR("hsu_conf_sel_ui_rdm_done failed", 0, 0, 0);
         }
         else
         {
            if(hsu_conf_sel_ui_comp_change_is_done() == TRUE)
            {
                /* Successful composition change */
                HSU_MSG_HIGH("composition change successful!!!", 0,0,0);
                ret = TRUE;
                brewext_app_ctx.comp_change_done = TRUE;

                if (appClosing)
                {
                  /* Now it is safe to unregister  callbacks  */
                  (void) hsu_al_brewext_unregister_rdm_callback();
                }

                switch (connect_state)
                {
                  case CONNECT_WAIT_DISCONNECT:
                    connect_state = CONNECT_IDLE;
                    hsu_al_brewext_disconnect();
                    break;

                  case CONNECT_WAIT_CONNECT:
                    connect_state = CONNECT_IDLE;
                    hsu_al_brewext_connect();
                    break;
                    
                  default:
                    connect_state = CONNECT_IDLE;
                    break;;
                }
            }
         }              
         break;
        
       case BREW_RDM_NOT_ALLOWED:       
       case BREW_RDM_DEVMAP_BUSY:       
       case BREW_RDM_APP_BUSY:
          /* USB mode change failed. */
          hsu_conf_sel_ui_rdm_event_received((uint16)status);        
          break;
     }
    return ret;
}

/* hsu_al_brewext_connect_cb is called when composition switch is done. 
 * hsu_al_brewext_disconnect starts when currect state is CONNECT_WAIT_DISCONNECT
 * The current state is set as CONNECT_WAIT_DISCONNECT when 
 * hsu_al_brewext_disconnect is called in the middle of processing 
 * hsu_al_brewext_connect. */
void hsu_al_brewext_connect_cb(void *arg, jresult_t result)
{
  switch (connect_state)
  {
    case CONNECT_WAIT_DISCONNECT:
      connect_state = CONNECT_IDLE;
      hsu_al_brewext_disconnect();
      break;
      
    default:
      connect_state = CONNECT_IDLE;
      break;
  }
}

/* Initiate spoof disconnect and re-enumrate the device as a MTP Brew extended Device */
juint32_t hsu_al_brewext_connect()
{
    const hsu_conf_sel_comp_info_type * comp_info = hsu_conf_sel_get_cur_comp_details();
    boolean is_connected = FALSE;
   
    if (NULL == comp_info)
    {
        return HSU_CONNECT_FAIL;
    }

    switch (connect_state)
    {
      case CONNECT_IDLE:
        connect_state = CONNECT_START;
        break;
        
      default:
        connect_state = CONNECT_WAIT_CONNECT;
        return 1;
    }
    
    brewext_app_ctx.old_hsu_comp_id = hsu_conf_sel_get_cur_comp_id();

    if (HSU_CONF_SEL_MTP_COMP == comp_info->hsu_comp_id || 
        HSU_CONF_SEL_BREW_EXT_COMP == comp_info->hsu_comp_id ||
        HSU_CONF_SEL_MTP_DIAG_COMP == comp_info->hsu_comp_id)
    {

        hsu_al_brewext_ioctl(MTPFD_PIPE_BULK, MTPFD_GET_CONNECT_STATUS, (void *)&is_connected);
        if (brewext_app_ctx.app_cbs.enabled_h)
         {
           if ((is_connected)== TRUE) 
             brewext_app_ctx.app_cbs.enabled_h ();
          } 
        return HSU_CONNECT_SUCCESS; 
    }
    
    /* Check if we can switch the composition dynamically right now */
    if (!hsu_conf_sel_ui_dynamic_switch_is_allowed())
    {
        return HSU_CONNECT_FAIL; /* We cannot change the composition right now */
    }

    brewext_app_ctx.comp_change_done = FALSE;

    /* Dynamically swtich over to Brew Ext (not MTP).
       We can only go to MTP composition when NV value is set to 4.
    */
    hsu_conf_sel_switch_dev_stack(
              HSU_CONF_SEL_BREW_EXT_COMP, 
              hsu_al_brewext_connect_cb,
              NULL);

    return HSU_CONNECT_SUCCESS;
}

/* hsu_al_brewext_disconnect_cb is called when composition switch is done. 
 * If connect_state is CONNECT_WAIT_CONNECT, start hsu_al_brewext_connect.
 */
void hsu_al_brewext_disconnect_cb(void *arg, jresult_t result)
{
  switch (connect_state)
  {
    case CONNECT_WAIT_CONNECT:
      connect_state = CONNECT_IDLE;
      hsu_al_brewext_connect();
      break;
      
    default:
      connect_state = CONNECT_IDLE;
      break;
  }
}

juint32_t hsu_al_brewext_disconnect()
{
    /* No need to check if we can switch the composition dynamically right now */

    switch (connect_state)
    {
      case CONNECT_IDLE:
        break;
        
      default:
        connect_state = CONNECT_WAIT_DISCONNECT;
        return 1;
    }

    /*  if we are aleady in the same comp, do nothing */
    if ( brewext_app_ctx.old_hsu_comp_id == hsu_conf_sel_get_cur_comp_id())
    {
      return 1;
    }

    brewext_app_ctx.comp_change_done = FALSE;

    /* roll back the composition */
    hsu_conf_sel_switch_dev_stack(
              brewext_app_ctx.old_hsu_comp_id, 
              hsu_al_brewext_disconnect_cb,
              NULL);

    return 1;
}

boolean hsu_al_brewext_register_rdm_callback(void * pClientData,
                                               brewext_notify_cb_t cb)
{
    brewext_app_ctx.client_h = pClientData;
    brewext_app_ctx.brewext_cb = cb;
    
    return TRUE;
}

/* This function is called from OEMUSBDriver_Init() at power up. The purpose is to 
   register callback function to handle launching/closing MTP background application. 
 */
boolean hsu_al_brewext_register_mtp_app_cb(mtpfd_mtp_app_cb mtpAppCBFunc)
{
    const hsu_conf_sel_comp_info_type * comp_info = hsu_conf_sel_get_cur_comp_details();

    if (NULL == comp_info)
    {
        brewext_app_ctx.app_cbs.mtp_app_launch_h         = NULL;
        return FALSE;
    }
    
    /* Only launch/close MTP background app when USB composition is set to 
       HSU_CONF_SEL_MTP_COMP (value '4'). This is to fix a bug caused by starting 
       of MTP background app after USBDriverDiag test app is entered.
    */
    if ((HSU_CONF_SEL_MTP_COMP == comp_info->hsu_comp_id) ||
        (HSU_CONF_SEL_MTP_DIAG_COMP == comp_info->hsu_comp_id))    
    {
        brewext_app_ctx.app_cbs.mtp_app_launch_h         = mtpAppCBFunc;
    }
    else
    {
        brewext_app_ctx.app_cbs.mtp_app_launch_h         = NULL;
    }
    
    return TRUE;

}

int hsu_al_brewext_register_app_cbs (mtpfd_enabled_cb EnableCB_ISR, 
                                                 mtpfd_write_complete_cb TxCB,
                                                 mtpfd_read_complete_cb RxCB,
                                                 mtpfd_notfication_cb IntTxCB,
                                                 mtpfd_write_error_cb TxErrCB,  
                                                 mtpfd_read_error_cb RxErrCB,
                                                 mtpfd_notification_error_cb IntTxErrCB,
                                                 mtpfd_setupmsg_cb SetupMsgCB_ISR)
{
    jresult_t rc=0;
  
    /* Fill General Handler   */
    brewext_app_ctx.app_cbs.enabled_h         = EnableCB_ISR;
    brewext_app_ctx.app_cbs.disabled_h        = NULL;

    brewext_app_ctx.app_cbs.suspend_h         = NULL;
    brewext_app_ctx.app_cbs.resume_h          = NULL;
           
    brewext_app_ctx.app_cbs.cnxn_state_chng_h = NULL;  
    brewext_app_ctx.app_cbs.tx_complete_h     = TxCB;
    brewext_app_ctx.app_cbs.rx_complete_h     = RxCB;
    brewext_app_ctx.app_cbs.txInt_complete_h  = IntTxCB;

    brewext_app_ctx.app_cbs.rx_error_h        = RxErrCB;
    brewext_app_ctx.app_cbs.tx_error_h        = TxErrCB;
    brewext_app_ctx.app_cbs.txInt_error_h     = IntTxErrCB; 

    brewext_app_ctx.app_cbs.tx_flush_h        = NULL;
    brewext_app_ctx.app_cbs.txInt_flush_h     = NULL; 

    brewext_app_ctx.app_cbs.setup_msg_h       = SetupMsgCB_ISR;
   
   return rc;

}


int hsu_al_brewext_unregister_app_cbs(void)
{
    jresult_t rc=0;

    brewext_app_ctx.app_cbs.enabled_h         = NULL;
    brewext_app_ctx.app_cbs.disabled_h        = NULL;

    brewext_app_ctx.app_cbs.suspend_h         = NULL;
    brewext_app_ctx.app_cbs.resume_h          = NULL;
           
    brewext_app_ctx.app_cbs.cnxn_state_chng_h = NULL;  
    brewext_app_ctx.app_cbs.tx_complete_h     = NULL;
    brewext_app_ctx.app_cbs.rx_complete_h     = NULL;
    brewext_app_ctx.app_cbs.txInt_complete_h  = NULL;

    brewext_app_ctx.app_cbs.rx_error_h        = NULL;
    brewext_app_ctx.app_cbs.tx_error_h        = NULL;
    brewext_app_ctx.app_cbs.txInt_error_h     = NULL; 

    brewext_app_ctx.app_cbs.tx_flush_h        = NULL;
    brewext_app_ctx.app_cbs.txInt_flush_h     = NULL; 

    brewext_app_ctx.app_cbs.setup_msg_h       = NULL;
       
   return rc;

}

int hsu_al_brewext_unregister_rdm_callback(void)
{
    jresult_t rc=0;

    brewext_app_ctx.brewext_cb = NULL;
    return rc;
}


int brewext_app_init(void* handle,  mtpfd_callbacks_t** cbs)
{ 
    jresult_t rc=0;

    brewext_app_ctx.mtpfd_client_h = handle;      /*  Save a handle to the  mtpfd context    */

    *cbs=&brewext_app_ctx.app_cbs;        /* Register the callbacks from OEMUSBDriver layer   */ 

    return rc;
}

/*==============================================================================
                USB functions -- Jungo calls to MTPFD
==============================================================================*/
void hsu_al_brewext_send_setup_msg_response(int size, void *response)
{
   /* Lock the giant Mutex before calling into MTP FD 
      to make sure USB stack is run by only one task at a time. */
   jsafe_enter(); 
   (void) mtpfd_control_write(brewext_app_ctx.mtpfd_client_h, response, (juint16_t)size);
   jsafe_leave();
}

void hsu_al_brewext_read_request(void *rx_buffer,
                                       uint16 rx_buffer_size,
                                       uint16 *rx_data_size)
{
  HSU_ASSERT (rx_buffer !=NULL);
   /* Lock the giant Mutex before calling into MTP FD 
      to make sure USB stack is run by only one task at a time. */
  jsafe_enter();
  (void) mtpfd_post_read(brewext_app_ctx.mtpfd_client_h,
                         rx_buffer, rx_buffer_size, rx_data_size);
  jsafe_leave();            
}
void hsu_al_brewext_write_request(mtpfd_pipe_type_t pipe_type, void* response, int size)
{
   /* Lock the giant Mutex before calling into MTP FD 
      to make sure USB stack is run by only one task at a time. */
   jsafe_enter();
   (void) mtpfd_post_write(brewext_app_ctx.mtpfd_client_h, response, (juint16_t)size, pipe_type);
   jsafe_leave();
}

void hsu_al_brewext_flush_tx(mtpfd_pipe_type_t type, mtpfd_flush_in_pipe_cb FlushCB)
{
   HSU_ASSERT (FlushCB !=NULL);
   /* Lock the giant Mutex before calling into MTP FD 
      to make sure USB stack is run by only one task at a time. */
   jsafe_enter();
   mtpfd_flush_in_pipe(brewext_app_ctx.mtpfd_client_h, type, FlushCB);
   jsafe_leave();
}

void hsu_al_brewext_ioctl(mtpfd_pipe_type_t pipe, 
                             mtpfd_ioctl_command_type command,
                             void *command_param)
{
   /* Lock the giant Mutex before calling into MTP FD 
      to make sure USB stack is run by only one task at a time. */
   jsafe_enter();
   mtpfd_ioctl(brewext_app_ctx.mtpfd_client_h, pipe, command, command_param);
   jsafe_leave();

}

/* Invoke the FD layer to stall the IN data pipe (BULK IN or INTR) */
void hsu_al_brewext_stall_in_pipe(mtpfd_pipe_type_t pipe)
{
   /* Lock the giant Mutex before calling into MTP FD 
      to make sure USB stack is run by only one task at a time. */
   jsafe_enter();
   mtpfd_stall_in_pipe(brewext_app_ctx.mtpfd_client_h, (pipe_type_t)pipe);
   jsafe_leave();
}

/* Invoke the FD layer to stall the OUT data pipe (BULK OUT) */
void hsu_al_brewext_stall_out_pipe(void)
{
   /* Lock the giant Mutex before calling into MTP FD 
      to make sure USB stack is run by only one task at a time. */
   jsafe_enter();
   mtpfd_stall_out_pipe(brewext_app_ctx.mtpfd_client_h);
   jsafe_leave();
}

/* Configure the memory buffer for data in data stage on EP0 */
void hsu_al_brewext_set_setup_msg_data_buffer(void *data, uint32 size)
{
  mtpfd_set_setup_msg_data_buffer(data, size);
}

/*==============================================================================

                      High Speed USB Brew Extended Device Adaptation Layer
  
GENERAL DESCRIPTION
Implementation of Brew Extended Adaptation Layer.
This si the header file for hsu_al_brewext.c.
      
Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_al_brewext.h#4 $
  $DateTime: 2009/02/02 11:45:12 $ $Author: tkwon $  
     
when       who     what, where, why
--------  ---     ---------------------------------------------------------
06/29/07   bt      Integrate MTP (IUSBDriver) feature into Main Line
   
==============================================================================*/
#include "hsu_conf_sel_types.h"
#include "mtp_fd_api.h"

/*==============================================================================

                     Jungo specific private data

==============================================================================*/
typedef void (*brewext_notify_cb_t)(void *po, uint32 rdm_event);
typedef void (*brewext_enable_cb_t)(void);

/* connect status */
#define HSU_CONNECT_SUCCESS     0
#define HSU_CONNECT_FAIL        1
#define HSU_CONNECT_WAIT        2

/* BREW extended device dynamic composition switching operation status */
typedef enum
{
    BREW_RDM_DONE,
    BREW_RDM_NOT_ALLOWED,    
    BREW_RDM_DEVMAP_BUSY,
    BREW_RDM_APP_BUSY
} brewext_rdm_change_status_t;


/*==============================================================================
                     Brew Extended Device  API implementations
==============================================================================*/

boolean hsu_al_brewext_handle_mode_change(brewext_rdm_change_status_t status, boolean appClosing);


/* Initiate spoof disconnect and re-enumrate the device as a MTP Brew extended Device */
uint32 hsu_al_brewext_connect(void);


uint32 hsu_al_brewext_disconnect(void);
/*==============================================================================
              USB functions
==============================================================================*/
void hsu_al_brewext_read_request(void *rx_buffer,
                                       uint16 rx_buffer_size,
                                       uint16 *rx_data_size);


void hsu_al_brewext_write_request(mtpfd_pipe_type_t pipe_type, 
                                       void* response, 
                                       int size);

void hsu_al_brewext_send_setup_msg_response(int nSize,
                                            void *response);

void hsu_al_brewext_flush_tx(mtpfd_pipe_type_t type, 
                                 mtpfd_flush_in_pipe_cb FlushCB);

void hsu_al_brewext_ioctl(mtpfd_pipe_type_t pipe, 
                             mtpfd_ioctl_command_type command,
                             void *command_param);

void hsu_al_brewext_stall_in_pipe(mtpfd_pipe_type_t pipe);

void hsu_al_brewext_stall_out_pipe(void);

/*==============================================================================
                Internal functions -- JUNGO call-back handlers
==============================================================================*/
boolean hsu_al_brewext_register_rdm_callback(void * pClientData,  
                                                         brewext_notify_cb_t cb);
boolean hsu_al_brewext_register_mtp_app_cb(mtpfd_mtp_app_cb mtpAppCBFunc);
int hsu_al_brewext_register_app_cbs (mtpfd_enabled_cb EnableCB_ISR, 
                                                 mtpfd_write_complete_cb TxCB,
                                                 mtpfd_read_complete_cb RxCB,
                                                 mtpfd_notfication_cb IntTxCB,
                                                 mtpfd_write_error_cb TxErrCB,  
                                                 mtpfd_read_error_cb RxErrCB,
                                                 mtpfd_notification_error_cb IntTxErrCB,
                                                 mtpfd_setupmsg_cb SetupMsgCB_ISR);


int hsu_al_brewext_unregister_app_cbs(void);
int hsu_al_brewext_unregister_rdm_callback(void);

void hsu_al_brewext_set_setup_msg_data_buffer(void *data, uint32 size);


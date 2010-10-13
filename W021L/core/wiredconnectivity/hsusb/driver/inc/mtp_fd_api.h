/*==============================================================================

                      High Speed USB MTP Function Driver Header File
                      
  
GENERAL DESCRIPTION
Header file for Function driver (mtp_fd.c).  Exposes  APIs available to the client
      
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE
  
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
  
$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/mtp_fd_api.h#2 $
$DateTime: 2008/05/25 08:09:29 $ $Author: lmanor $
    
when      who     what, where, why
--------  ---     ----------------------------------------------------------
06/29/07   bt      Integrate MTP (IUSBDriver) feature into Main Line
    
==============================================================================*/

#ifndef _MTP_FD_API_H_
#define _MTP_FD_API_H_


typedef void * mtpfd_handle_t;

typedef enum
{
    MTPFD_ABORT_READ,             /* Abort an uncompleted read                          */
    MTPFD_ABORT_WRITE,            /* Abort an uncompleted write                          */
    MTPFD_GET_CONNECT_STATUS,     /* Retrieve the state of the DTR signal              */
    MTPFD_SET_CONNECT_STATUS_CB,  /* Register DTR-Toggle callback function          */
    MTPFD_CLEAR_CONNECT_STATUS_CB /* Deregister the DTR-Toggle callback function */
} mtpfd_ioctl_command_type;


/* Types of supported Pipes (same as USB Chp 9) */
typedef enum
{
  MTPFD_PIPE_CONTROL = 0,
  MTPFD_PIPE_ISOC    = 1,
  MTPFD_PIPE_BULK    = 2,
  MTPFD_PIPE_INTR    = 3
} mtpfd_pipe_type_t;


/* USB connection API *************/
typedef void (*mtpfd_mtp_app_cb)(boolean);
typedef void (*mtpfd_enabled_cb)(void);
typedef void (*mtpfd_disabled_cb)(void);
typedef int (*mtpfd_uninit_cb)(mtpfd_handle_t );

typedef void (*mtpfd_read_complete_cb)(mtpfd_pipe_type_t);
typedef void (*mtpfd_write_complete_cb)(void);
typedef void (*mtpfd_notfication_cb)(void);

typedef void (*mtpfd_read_error_cb)(void);
typedef void (*mtpfd_write_error_cb)(void);
typedef void (*mtpfd_notification_error_cb)(void);

typedef void (*mtpfd_suspend_cb)(void);
typedef void (*mtpfd_resume_cb)(void);


typedef void (*mtpfd_setupmsg_cb)(void *);
typedef void (*mtpfd_cnxn_status_cb)(void);

typedef void (*mtpfd_flush_in_pipe_cb)(void);

/*==========================================================
 *   Control class API
 *==========================================================*/

/* Following is initialized  from the OEM driver layer  */
typedef struct
{
   /* This is the callback function to start/close Teleca MTPH background app. */
   mtpfd_mtp_app_cb              mtp_app_launch_h;

   /* General Handlers */
   mtpfd_enabled_cb             enabled_h;
   mtpfd_disabled_cb            disabled_h;
   mtpfd_uninit_cb              uninit_h;

   /*  suspend/resume  notofication cb  */
   mtpfd_suspend_cb             suspend_h;
   mtpfd_resume_cb              resume_h;
   mtpfd_cnxn_status_cb         cnxn_state_chng_h;

   /* Transfer Completion Handlers */
   mtpfd_read_complete_cb       rx_complete_h;   /* RX cb:xfer from OUT complete */
   mtpfd_write_complete_cb      tx_complete_h;   /* TX cb:xfer on IN complete       */
   mtpfd_notfication_cb         txInt_complete_h;/* TX cb:xfer on INTR complete   */

   /* Transfer error occurence Handlers */
   mtpfd_read_error_cb          rx_error_h;       /* RX error call back                   */
   mtpfd_write_error_cb         tx_error_h;       /* TX error call back                   */
   mtpfd_notification_error_cb  txInt_error_h;    /* TX Int error call back              */

   /* Flsuh transmit  pipe (IN endpoints) handlers */
   mtpfd_flush_in_pipe_cb       tx_flush_h ;      /* TX flush call back for Bulk IN  */
   mtpfd_flush_in_pipe_cb       txInt_flush_h ;   /* TX flush call back for iINTR IN*/

   /* Process Non-standard SETUP request handlers */
   mtpfd_setupmsg_cb            setup_msg_h;
} mtpfd_callbacks_t;

#ifdef HSU_MTP_DEBUG_MSG_ON
#error code not present
#endif

/* Defined by teh MTPFD client: hsu_al_brewext.c         */
int brewext_app_init(void* handle, mtpfd_callbacks_t** cbs_h);

/*==========================================================
 *     Data plane API available to MTPFD client
 *==========================================================*/
 
void mtpfd_ioctl(mtpfd_handle_t handle, mtpfd_pipe_type_t type, 
                  mtpfd_ioctl_command_type command,
                  void *command_param);

int mtpfd_post_write(mtpfd_handle_t handle, void *buffer,
                       uint16 size, mtpfd_pipe_type_t type);

int mtpfd_post_read(mtpfd_handle_t handle, void *buffer, 
                             uint16 buffer_size,  uint16 *size);

int mtpfd_control_write(mtpfd_handle_t handle, void *buffer, 
                                                      uint16 size);

int mtpfd_control_read(mtpfd_handle_t handle, void *buffer, 
                                                      uint32 size);

void mtpfd_flush_in_pipe(mtpfd_handle_t handle, mtpfd_pipe_type_t type, 
                            mtpfd_flush_in_pipe_cb FlushCB);

#endif


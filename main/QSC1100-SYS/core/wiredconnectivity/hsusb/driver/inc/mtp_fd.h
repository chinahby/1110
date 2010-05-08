/*==============================================================================

                      High Speed USB MTP Function Driver Header File
                      
  
GENERAL DESCRIPTION
Header file for mtp_fd.c.
      
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE
  
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
  
$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/mtp_fd.h#4 $
$DateTime: 2009/02/02 11:45:12 $ $Author: tkwon $
    


when      who     what, where, why
--------  ---     ----------------------------------------------------------
06/29/07   bt      Integrate MTP (IUSBDriver) feature into Main Line

==============================================================================*/
#ifndef _FD_MTPFD_H_
#define _FD_MTPFD_H_
#include <mtp_fd_api.h>

#include <jusb_common.h>
#include <jusb_core_fd.h>

#define USB_MAX_BLOCK_SIZE        0x8000   /* 32 K buffer*/

/* MTP function driver (FD) states      */
typedef enum {
    MTP_STATE_DISABLED = 0,
    MTP_STATE_ENABLED  = 1
} mtpfd_states_t;

typedef enum {
    MTP_TRANSFER_ERROR = -1,
    MTP_TRANSFER_OK = 0,
    MTP_TRANSFER_CANCELLED = 1
} mtpfd_iostatus_t;


/* MTP Extended Configuration Descriptor   */
typedef PACKED struct _mtp_descr_header
{
    uint32      dwLength;           /* length in byte   */
    uint16      bcdVersion;         /* release number in BCD format */
    uint16      wIndex;             /* Fixed = 0x 0004 */
    uint8       bCount;             /* Total number of function sections = 0x01 */
    uint8       reserved_1[7];      /* Reserved */
    
} mtp_feature_descr_header_t;

typedef PACKED struct _mtp_descr_function
{
    uint8       bFirstInterfaceNumber;  /* starting interface number = 0x00 */
    uint8       bInterfaceCount;        /* total number of interfaces  */ 
    uint8       compatibleID[8];        /* compatible ID = MTP */
    uint8       subCompatibleID[8];     /* Sub compatible ID */ 
    uint8       reserved_2[6];          /* Reserved */
    
} mtp_descr_function_t;

typedef PACKED struct _mtp_descr
{
    mtp_feature_descr_header_t  mtp_header;
    mtp_descr_function_t        mtp_function;
    
} mtp_descr_t;

/**************************************************************************
 * USB Functions                                                                                                          *
 *                                                                                                                               *
 **************************************************************************/


/**************************************************************************
 * Function name:  mtp_stop_io
 * Description:    Cancels the current USB I/O request.
 * Parameters: 
 *     @ctx:       (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
void mtpfd_stop_io(mtpfd_handle_t handle);

/**************************************************************************
 * Function name:  mtp_abort_write
 * Description:    Aborts pending read requests.
 * Parameters: 
 *     @ctx: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
jresult_t mtpfd_abort_write(mtpfd_handle_t handle, pipe_type_t type);

/**************************************************************************
 * Function name:  mtp_abort_read
 * Description:    Aborts pending read requests.
 * Parameters: 
 *     @ctx: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
jresult_t mtpfd_abort_read(mtpfd_handle_t handle);

/**************************************************************************
 * Function name:  mtp_stall_in_pipe
 * Description:    Stalls the IN data/intr pipe.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
void mtpfd_stall_in_pipe(mtpfd_handle_t handle, pipe_type_t type);

/**************************************************************************
 * Function name:  mtp_stall_out_pipe
 * Description:    Stalls the OUT data pipe.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **************************************************************************/
void mtpfd_stall_out_pipe(mtpfd_handle_t handle);


/**************************************************************************
 * Function name:  mtpfd_init
 * Description:    Initializes the USB function driver.
 * Parameters: 
 *     @ctx: (IN) Core context
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Global
 **************************************************************************/
jresult_t mtpfd_init(void *core_ctx);

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
   uint32 size);

#endif


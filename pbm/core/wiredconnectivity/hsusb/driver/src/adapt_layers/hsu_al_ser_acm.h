#ifndef _HSU_AL_SER_ACM_H_
#define _HSU_AL_SER_ACM_H_

/*==============================================================================

Serial Functions over ACM Adaptation Layer for High Speed USB
H E A D E R   F I L E

GENERAL DESCRIPTION
Contains the API for the serial functions over ACM adapt. layer

EXTERNALIZED FUNCTIONS
hsu_al_ser_acm_populate_acm_specific_data
hsu_al_ser_acm_ioctl
hsu_al_ser_acm_open
hsu_al_ser_acm_close

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_base_init must be called for function instance, 
prior to performing any other operations on the instance.

Copyright (c) 2006 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_ser_acm.h#8 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/30/06 esh  First revision of file    
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" /* For FEATURE_HSU_TEST to work */

#include "hsu_al_ser_base.h"

#include "sio.h"

#include "uw_device_cdc.h"
#include "uw_device_cdc_acm.h"
#include "jtypes.h"

#include "dsm.h"

#include "rex.h" /*  For rex_timer_type and rex_timer_cnt_type */
/*==============================================================================
Typedefs
==============================================================================*/
typedef struct
{
  juint16_t             ctrl_line_state;
    /* Ctrl Line State: See CDC Spec. Section 6.2.14 */

  sio_vv_func_ptr_type  app_dtr_changed_cb_func;
    /* App callback to be invoked whenever DTR state is changed */

  acm_line_coding_t     line_coding;
    /* Holds the current line coding, as was determined by the host */
    /* See CDC Spec. Section 6.2.13 */

  juint16_t             last_serial_state_bitmap_rxed_by_host;
    /* Holds the last serial_state bitmap ever received by the host. */
    /* See CDC Spec. Section 6.2.5 */

  juint16_t             last_serial_state_bitmap_sent_to_host;
    /* Holds the last serial_state bitmap ever sent to the host
    by the FD, not necessarily received by the host. */
    /* See CDC Spec. Section 6.2.5 */

  juint16_t             serial_state_bitmap_to_send_to_host;
    /* Holds the most updated serial_state bitmap which needs to be sent to host.
    This is to prevent consecutive calls to acm_notify without waiting for the 
    completion callback. */
    /* See CDC Spec. Section 6.2.5 */

  void*                 stream_mode_specific_data_ptr;
    /* Specific data, depending on strea_mode */

  boolean               inbound_flow_enabled;
    /* Indication whether or not inbound flow is enabled, which means more 
    read operations can be done. If disabled, no further reads should be made. */
  void*                 saved_buffer_when_inbound_flow_disabled;
    /* When inbound_flow_enabled is set to FALSE, and there was a buffer to 
    send to the app, then this variable will store the buffer, and send it once 
    inbound_flow is enabled again */
  juint32_t             saved_buffer_data_size;
    /* When inbound_flow_enabled is set to FALSE, then this variable will 
    store the size of the buffer's data to send to the app, once it is enabled again */
  boolean               enqueued_simulated_rx_done_when_inbound_enabled;
    /* Indicates that when inbound flow was enabled (and 
    saved_buffer_when_inbound_flow_disabled isn't NULL) a request to enqueue 
    a simulated Rx done was made to hsu_al_task. This is to prevent multiple
    calls to simulating Rx done when receiving a disable/enable/disable/enable 
    sequence before the hsu_al_task had the chance to run. */

  uint32                sio_ioctl_set_buffer_size_mirror;
    /* "Mirror image" of the given I/O ctl 
    SIO_IOCTL_SET_BUFFER_SIZE/SIO_IOCTL_GET_BUFFER_SIZE, so that the app
    will be under the impression its set buffer size got through */

  boolean                           flush_tx_ended;
  /* flag to indicate that the flush has ended.
  Currently used for a special case of flush - 
  that needs to be performed synchronously (during acm_ioctl that handles the cmd:CHANGE_MODE*/

} modem_specific_data_struct;
  /* Data unique for the modem function */

/* modem's stream mode specific data: */
/* stream mode specific data when modem is in raw data mode */
typedef struct
{
  rex_timer_type          rawdata_mode_esc_seq_timer_cb_item;
  sio_vv_func_ptr_type    rawdata_mode_esc_sequence_cb_func;
  uint8                   num_of_esc_seq_characters_seen;
  boolean                 should_wait_for_clk;
} rawdata_mode_data_struct;
  /* For Raw Data, the field stream_mode_specific_data_ptr shall 
  assume the type of the above struct */

typedef struct  
{
  hsu_al_ser_base_data_for_ser_if     base_data;
  modem_specific_data_struct          modem_if_data;
} hsu_al_ser_acm_data; 
/* Information needed from an ACM I/F */

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_ser_acm_populate_acm_specific_data

DESCRIPTION
This function, called following acm_init, populates the various fields 
needed by the ACM FD.  

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_acm_populate_acm_specific_data
(
  hsu_al_ser_acm_data* acm_data_ptr,        /* Pointer to ACM Data struct */
  cdc_handle_t handle,                      /* Given ACM FD handle        */
  acm_callbacks_t* cbs,                     /* output param: ACM callback 
                                               struct to populate          */
  acm_init_info_t* info,                    /* output param: ACM info struct 
                                               to populate                 */
  cdc_appctx_t* ctx                         /* output param: Context to be 
                                               passed by ACM FD in various 
                                               callbacks                   */
);

/*===========================================================================

FUNCTION hsu_al_ser_acm_init

DESCRIPTION
Initializes CDC/ACM AL specific fields.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_acm_init
(
  hsu_al_ser_acm_data* acm_data_ptr          /* Pointer to ACM Data struct */
);

/*===========================================================================

FUNCTION hsu_al_ser_acm_ioctl

DESCRIPTION
Allows passing an I/O control command to the serial I/F using CDC/ACM.

DEPENDENCIES
None

RETURN VALUE
TRUE - I/O control command handled. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_acm_ioctl
(
  hsu_al_ser_acm_data* acm_data_ptr,         /* Pointer to ACM Data struct */
  sio_ioctl_cmd_type ioctl_cmd,              /* SIO I/O control command    */
  sio_ioctl_param_type* ioctl_param_ptr      /* SIO I/O control command's 
                                                parameter                  */
);

/*===========================================================================

FUNCTION hsu_al_ser_acm_open

DESCRIPTION
This function opens a stream across a serial interface on the CDC/ACM FD. 

DEPENDENCIES
None

RETURN VALUE
TRUE - Operation Successful. FALSE - Otherwise

SIDE EFFECTS
TODO

===========================================================================*/
boolean hsu_al_ser_acm_open
(
  hsu_al_ser_acm_data* acm_data_ptr,         /* Pointer to ACM Data struct */
  const sio_open_type* open_ptr,             /* Configurations of stream   */
  dsm_mempool_id_enum_type pool_id,          /* Pool ID from where to take 
                                               DSM items                   */
  byte* memory_pool_for_rx_ptr,              /* Mem pool for Rx            */
  byte* memory_pool_for_tx_ptr,              /* Mem pool for Tx            */
  juint32_t buff_size_for_read,              /* Typical amount of bytes to 
                                               alloc for Rx operations     */
  juint32_t max_buff_size_for_write,         /* Maximal amount of bytes to 
                                               alloc for Tx operations     */
  rex_timer_cnt_type tx_aggr_delay_in_msecs  /* Amount of msecs to wait, in 
                                             order to aggregate more packets
                                                                           */
);

/*===========================================================================

FUNCTION hsu_al_ser_acm_close

DESCRIPTION
This function closes an open stream of a serial interface above the CDC/ACM FD.
It allows an optional callback to notify caller when all the bookkeeping 
needed for closing the stream (flushing Tx, aborting Rx, etc.) 
has been completed.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_acm_close
(
  hsu_al_ser_acm_data* acm_data_ptr,         /* Pointer to ACM Data struct */
  hsu_al_ser_base_op_complete_cb close_cb,   /* Callback to be invoked 
                                             when close has been completed */
  boolean abort_io_operations_immediately    /* Whether or not to abort all 
                                             read/write operations 
                                             immediately or wait until all 
                                             Tx data is flushed, followed 
                                             by an abort read              */
);

#endif /* _HSU_AL_SER_ACM_H_ */

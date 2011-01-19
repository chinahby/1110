#ifndef _HSU_AL_SER_BASE_H_
#define _HSU_AL_SER_BASE_H_

/*==============================================================================

Serial Functions (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB
"Base Class" Header File

GENERAL DESCRIPTION
Contains data structures and API for the "base class"
of the various serial I/Fs which are used in the serial functions' 
adaptation layer.

EXTERNALIZED FUNCTIONS

hsu_al_ser_base_init
hsu_al_ser_base_open
hsu_al_ser_base_close
hsu_al_ser_base_transmit
hsu_al_ser_base_flush_tx
hsu_al_ser_base_ioctl

Copyright (c) 2006 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/30/06 esh  First revision of file  
==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" /* For FEATURE_HSU_TEST to work */

#include "sio.h"
#include "dsm.h"
#include "rex.h"

#include "uw_device_cdc.h"
#include "jtypes.h"
/*==============================================================================
Constants and Macros
==============================================================================*/
#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

/*==============================================================================
Forward Declarations
==============================================================================*/
struct hsu_al_ser_base_data_for_ser_if_st;

/*==============================================================================
Typedefs
==============================================================================*/
typedef void (*hsu_al_ser_base_op_complete_cb)
  (struct hsu_al_ser_base_data_for_ser_if_st*);
    /* Used for completion of close / flush tx operations */

typedef struct hsu_al_ser_base_data_for_ser_if_st
{
#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

  sio_stream_id_type                stream_id;
    /* stream id used by this I/F */

  sio_stream_mode_type              stream_mode;
  /* The stream mode given by the app (autodetect or raw data)*/

  dsm_watermark_type*               tx_wmq_ptr;
    /* Tx WMQ used by this I/F */

  dsm_watermark_type*               rx_wmq_ptr;
    /* Rx WMQ used by this I/F */

  sio_rx_func_ptr_type              app_rx_cb_func;
    /* Rx callback used by this I/F */

  hsu_al_ser_base_op_complete_cb     flush_tx_complete_cb;
    /* Flush Tx callback used by this I/F */

  hsu_al_ser_base_op_complete_cb     close_complete_cb;
    /* Close callback used by this I/F */

  boolean                           dmov_gather_or_fd_write_or_timer_busy;
    /* Indicator whether or not to wait until DMOV (currently busy with gathering) 
    or CDC FD (currently busy with cdc_write)
    are free (thus allowing more packets to be queued for the next DMOV API gather) 
    In case of aggregation via timer, this indicates also whether or not
    a timer has been set. */

  cdc_handle_t                      instance_of_fd_used;
    /* Handle of the FD used by this I/F */


  /* TODO: Obsolete type - change to dsm_mempool_id_type 
  when moving to targets >= 7500 */
  dsm_mempool_id_enum_type          pool_id_of_new_dsm_items_to_alloc;
    /* Pool ID from which DSM items would be allocated */

  /* Rx and Tx mem pools to provide buffers for serial CDC FD */
  byte*                             memory_pool_for_rx;
  byte*                             memory_pool_for_tx;

  /* Size of buffer to allocate from Rx pools */
  juint32_t                         buff_size_for_read;

  /* "Reference Count" of the number of aborts */
  uint8                             num_of_aborts_issued_prior_to_close;

  /* Crit section guard for abort write as well as for accessing 
  instance_enabled_by_cdc_fd */
  rex_crit_sect_type                al_ser_base_access_guard;

  sio_ioctl_flow_ctl_type           ioctl_flow_ctl_mirror;
    /* "Mirror image" of the given I/O ctl flow type, so that the app
    will be under the impression its set flow ioctl got through */

  /* Length of current buffer to submit to FD for sending */
  juint32_t                         size_of_curr_buff_for_write;

  /* Indication whether this instance is enabled or not by the FD */
  boolean                           instance_enabled_by_cdc_fd;

  /* Indication whether there is a "read" operation in progress.
  This means either a call for the FD's read was made, or that
  DMOV is in the middle of a scatter operation which will forward the 
  results of the read operation to the app */
  boolean                           read_oper_is_in_progress;

  /* Max Size of buffer to allocate from Tx pools */
  juint32_t                         max_buff_size_for_write;

  /* Max Size of buffer to allocate from Rx pools */
  juint32_t                         max_buff_size_for_read;

  /* Partial DSM chain to Tx, in case in a previous transfer only part of a DSM 
  chain was taken (the remaining part is in this field) due to it being larger 
  than max_buff_size_for_write. */
  dsm_item_type*                    remaining_chain_to_tx_ptr;


  /* Members relevant for Tx aggregation via timer */
  /* Timer value in msecs */
  rex_timer_cnt_type                tx_aggr_delay_in_msecs;
  
  /* Timer struct for Tx aggregation timer */
  rex_timer_type                    tx_aggr_timer;

  /* Indication whether or not there is a need to flush accumulated data, once
  received Tx done callback from HS-USB stack */
  boolean                           tx_aggr_flush_next_aggr_packets;

  /* Indication whether or not a previously flush command has already been 
  enqueued. When set to TRUE and in case there is an active timer exists, this will prevent 
  the timer cb to also invoke a request to send all enqueued data. */
  boolean                           tx_aggr_flush_command_already_enqueued;


#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND

  boolean                           overcome_1_2_byte_hw_crc_err;

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */



#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* defined(FEATURE_HSU_TEST) && !defined(T_REXNT) */

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

  boolean                           cdc_abort_write_issued ;
    /* This flag is set to TRUE when cdc_abort_write was issued following a 
       call to flush_tx
     */

  rex_timer_type                    flush_tx_timer;
    /* This timer is set when flush_tx is issued. When this timer expires, if 
       flush_tx_cb was not issued yet, TX WMQ will be emptied and 
       cdc_abort_write will be issued 
     */

  boolean flush_tx_in_progress;
  /* flag to state that the flush_tx was called & hasn't ended yet */

} hsu_al_ser_base_data_for_ser_if; 
    /* Data common to all Serial I/Fs */

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
/*===========================================================================

FUNCTION hsu_al_ser_base_init

DESCRIPTION
Initializes the given serial I/F data structure for later usage.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_al_ser_base_init
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                                              /* Pointer to serial I/F data 
                                               struct                      */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_open

DESCRIPTION
This function opens a stream across a serial interface on the CDC FD.  

DEPENDENCIES

RETURN VALUE
TRUE - Operation Successful. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_base_open
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,    
                                            /* Pointer to serial I/F data 
                                               struct                      */
  const sio_open_type* open_ptr,            /* Configurations of stream    */
  dsm_mempool_id_enum_type pool_id,         /* Pool ID from where to take 
                                               DSM items                   */
  byte* memory_pool_for_rx_ptr,             /* Mem pool for Rx             */
  byte* memory_pool_for_tx_ptr,             /* Mem pool for Tx             */
  juint32_t buff_size_for_read,             /* Typical amount of bytes to 
                                               alloc for Rx operations     */
  juint32_t max_buff_size_for_write,        /* Maxmimal amount of bytes to 
                                               alloc for Tx operations     */
  juint32_t max_buff_size_for_read,         /* Maxmimal amount of bytes to 
                                               alloc for Rx operations     */
  rex_timer_cnt_type tx_aggr_delay_in_msecs /* Amount of msecs to wait, in 
                                               order to aggregate more 
                                               packets                     */

);

/*===========================================================================

FUNCTION hsu_al_ser_base_close

DESCRIPTION
This function closes an open stream of a serial interface above the CDC FD.
It allows an optional callback to notify caller when all the bookkeeping 
needed for closing the stream (flushing Tx, aborting Rx, etc.) 
has been completed.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,    
                                            /* Pointer to serial I/F data 
                                               struct                      */
  hsu_al_ser_base_op_complete_cb close_cb,  /* Callback to be invoked 
                                             when close has been completed */
  boolean abort_io_operations_immediately   /* Whether or not to abort all 
                                             read/write operations 
                                             immediately or wait until all 
                                             Tx data is flushed, followed 
                                             by an abort read              */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_transmit

DESCRIPTION
This function will transmit a packet or if transmitter is busy, schedule
the transmission of a packet. Note that following this invocation, the caller
is no longer the owner of the packet and must not use it anymore.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_transmit
( 
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
                                   /* Pointer to serial I/F data struct    */
  dsm_item_type *tx_ptr            /* Packet which needs to be transmitted */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_flush_tx

DESCRIPTION
This function will keep transmitting until no more data is pending transmission.
It requires provding a callback to notify caller when reaching this situation.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_flush_tx
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
                                   /* Pointer to serial I/F data struct    */
  hsu_al_ser_base_op_complete_cb flush_tx_cb 
                                   /* App callback to be invoked when all 
                                   pending data has been transmitted       */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_ioctl

DESCRIPTION
Allows passing an I/O control to the serial I/F using CDC.

DEPENDENCIES

RETURN VALUE
TRUE - IOCTL handled. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_base_ioctl
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
                                             /* Pointer to serial I/F data 
                                                struct                     */
  sio_ioctl_cmd_type ioctl_cmd,              /* SIO I/O control command    */
  sio_ioctl_param_type* ioctl_param_ptr      /* SIO I/O control command's 
                                                parameter                  */
);

#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* FEATURE_HSU_TEST && !T_REXNT */


#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

#endif /* _HSU_AL_SER_BASE_H_ */


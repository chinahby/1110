/*==============================================================================

Serial Functions (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB
"Base Class" Implementation

GENERAL DESCRIPTION
Contains the implementation of the API for the serial functions'
adapt. layer "Base Class"

EXTERNALIZED FUNCTIONS

hsu_al_ser_base_init
hsu_al_ser_base_open
hsu_al_ser_base_close
hsu_al_ser_base_transmit
hsu_al_ser_base_flush_tx
hsu_al_ser_base_ioctl

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_base_init must be called for each function instance, 
prior to performing any other operations on the instance.

Copyright (c) 2006 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_ser_base.c#25 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
07/30/06  esh     First revision of file
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_ser_base.h"
#include "hsu_al_ser_base_i.h"

#include "customer.h" /* For FEATURE_HSU_TEST to work */

#include "hsu_al_ser_i.h"
#include "hsu_common.h"
#include "hsu_al_dmov_api.h"
#include "hsu_al_dsm_utils.h"

#include "err.h"

#include "port.h"

#include "hsu_al_task.h"

#include "hsu_cdc_fd_api.h"

#include "sio_wrapper.h" /* for SIO_PAYLOAD */
/* NOTE: defined(FEATURE_HSU_TEST) && !defined(T_REXNT) 
means the code is used for testing the adaptation layer above UART */
#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

/* For ESUSPENDED */
#include "jerrno.h"

#include "dsm_pool.h"
/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

#define AL_SER_BASE_MAX_IO_ABORTS_BEFORE_CLOSE (2)
  /* Can issue no more than one abort read and one abort write */

 #define AL_SER_BASE_FLUSH_TX_TIMEOUT_MS         4000 
  /* Timeout to wait for flush_tx_cb to be invoked in msec 
     This value is an estimation of the time that will take the AL to
     send all the data from a full tx_wmq:
     The DNE value of DIAG tx_wmq is 400K
     Out transfer rate is 100KBps
     => it will take us ~4sec to send all data from a full tx_wmq
   */
/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/

/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
static void al_ser_base_dmov_scatter_callback
(
  boolean operation_result,
  dsm_item_type* dsm_chain_ptr,
  void* buffer_ptr,
  void* user_ptr
);

static void al_ser_base_dmov_gather_callback
(
  boolean operation_result,
  dsm_item_type* dsm_chain_ptr,
  void* buffer_ptr,
  void* user_ptr
);

static void al_ser_base_set_params_in_open_close
(
 hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr, 
 sio_stream_id_type stream_id,
 dsm_watermark_type* tx_wmq_ptr,
 dsm_watermark_type* rx_wmq_ptr,
 sio_rx_func_ptr_type app_rx_cb_func,
 dsm_mempool_id_enum_type pool_id_of_new_dsm_items_to_alloc,
 byte* memory_pool_for_rx,
 byte* memory_pool_for_tx,
 juint32_t buff_size_for_read,
 sio_flow_ctl_type rx_flow_ctl,
 sio_flow_ctl_type tx_flow_ctl,
 juint32_t max_buff_size_for_write,
 juint32_t max_buff_size_for_read,
 rex_timer_cnt_type tx_aggr_delay_in_msecs
 );

static void al_ser_base_internal_close
(
 hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
 );

static void al_ser_base_internal_abort_read
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
);

static void al_ser_base_internal_abort_write
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
);

static void al_ser_base_internal_flush_tx_done_issued_by_close
(
 hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
 );

static boolean al_ser_base_has_already_been_closed
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
);


/* The following two are invoked from AL Task */
static void al_ser_base_command_read
(
  void* ctx_for_al_task
);

static void al_ser_base_command_write
(
  void* ctx_for_al_task
);


static boolean al_ser_base_alloc_buffer_and_setup_read
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  jresult_t* cdc_fd_error
);

static void al_ser_base_discard_all_tx_data
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  void* curr_buffer_to_tx_ptr
);

static void al_ser_base_send_all_tx_data
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
);


/* Tx Aggregation functions: */
static boolean al_ser_base_tx_aggr_is_used
(
  const hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
);

static void al_ser_base_tx_aggr_timer_cb
(
  unsigned long ser_if_data_as_cb_arg
);

static void al_ser_base_start_aggregation_via_timer
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  dsm_item_type* tx_ptr
);

static void al_ser_base_cancel_aggregation_and_flush
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
);

static void al_ser_base_send_more_data_or_aggregate
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
);

/* The following is invoked from AL Task */
static void al_ser_base_command_send_all_data
(
  void* ctx_for_al_task
);

static void al_ser_base_flush_tx_timer_cb
(
 unsigned long ser_if_data_as_cb_arg
);

#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* defined(FEATURE_HSU_TEST) && !defined(T_REXNT) */
/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================
FUNCTION hsu_al_ser_base_init()
===========================================================================*/
void hsu_al_ser_base_init
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
)
{
#ifdef FEATURE_HSU_TEST  
#error code not present
#endif /* FEATURE_HSU_TEST */

#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* defined(FEATURE_HSU_TEST) && !defined(T_REXNT) */

  (void)hsu_al_ser_base_i_uninit(ser_if_data_ptr);

  rex_init_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));
  rex_def_timer_ex(&(ser_if_data_ptr->tx_aggr_timer), 
    al_ser_base_tx_aggr_timer_cb,
    (unsigned long)ser_if_data_ptr);

  ser_if_data_ptr->cdc_abort_write_issued = FALSE;
  rex_def_timer_ex(&(ser_if_data_ptr->flush_tx_timer), 
    al_ser_base_flush_tx_timer_cb,
    (unsigned long)ser_if_data_ptr);

  ser_if_data_ptr->stream_id = SIO_NO_STREAM_ID;

  ser_if_data_ptr->flush_tx_in_progress = FALSE;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_open()
===========================================================================*/
boolean hsu_al_ser_base_open
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  const sio_open_type* open_ptr,
  dsm_mempool_id_enum_type pool_id,
  byte* memory_pool_for_rx_ptr,
  byte* memory_pool_for_tx_ptr,
  juint32_t buff_size_for_read,
  juint32_t max_buff_size_for_write,
 juint32_t max_buff_size_for_read,
  rex_timer_cnt_type tx_aggr_delay_in_msecs
)
{
  boolean read_operation_successful;
  jresult_t cdc_read_res;

  /* TODO: Validation function --> move to hsu_al_ser */
  /* i.e.: validate stream_id != SIO_NO_STREAM_ID */

  HSU_MSG_LOW("hsu_al_ser_base_open: Checking if ser_if_data_ptr (0x%x) valid...", 
    ser_if_data_ptr, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  HSU_ASSERT(open_ptr != NULL);

  /* When dynamic configurations are supported, it is possible that an app 
  requested to open a HSU port, but without the HSU stack initializing it.
  To verify this, instance_of_fd_used must be checked to be non NULL.
  It is assumed that hsu_al_ser_base_init was called previously for this instance. */
  if (ser_if_data_ptr->instance_of_fd_used == NULL)
  {
    HSU_MSG_ERROR("hsu_al_ser_base_open: instance for port_id=%u was not \
                  initialized by stack!", open_ptr->port_id, 0, 0);
    return FALSE;
  }

  HSU_ASSERT(memory_pool_for_tx_ptr != NULL);
  HSU_ASSERT(max_buff_size_for_write > 0);


  if (memory_pool_for_rx_ptr != NULL)
  {
    HSU_ASSERT(buff_size_for_read > 0);
  }

  /* Save the stream mode */
  ser_if_data_ptr->stream_mode = open_ptr->stream_mode;

  /* If instance is not using an Rx buffer, simply fill the fields and return */
  if (memory_pool_for_rx_ptr == NULL)
  {
    al_ser_base_set_params_in_open_close(ser_if_data_ptr, open_ptr->stream_id, 
      open_ptr->tx_queue, open_ptr->rx_queue, open_ptr->rx_func_ptr, pool_id, 
      memory_pool_for_rx_ptr, memory_pool_for_tx_ptr, buff_size_for_read, 
      open_ptr->rx_flow, open_ptr->tx_flow, max_buff_size_for_write, 0,
      tx_aggr_delay_in_msecs);    

    HSU_MSG_LOW("hsu_al_ser_base_open: Operation Successful (no Rx buff).", 
      0, 0, 0);
    return TRUE;
  }

  HSU_ASSERT(max_buff_size_for_read > 0);
  /* The following scenario is possible: 
    0. enable callback not yet invoked
  1. Inside al_ser_base_set_params_in_open_close, memory_pool_for_rx was set
    2. CDC FD invokes enable -> causes cdc_read operation
    3. (context returned here): because enable was invoked
    ser_if_data_ptr->instance_enabled_by_cdc_fd is TRUE , so yet another read is 
    set...
  */

  rex_enter_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

  al_ser_base_set_params_in_open_close(ser_if_data_ptr, open_ptr->stream_id, 
    open_ptr->tx_queue, open_ptr->rx_queue, open_ptr->rx_func_ptr, pool_id, 
    memory_pool_for_rx_ptr, memory_pool_for_tx_ptr, buff_size_for_read, 
    open_ptr->rx_flow, open_ptr->tx_flow, max_buff_size_for_write,
    max_buff_size_for_read, tx_aggr_delay_in_msecs);

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* FEATURE_HSU_TEST && !T_REXNT */

  if (ser_if_data_ptr->instance_enabled_by_cdc_fd == TRUE)
  {
    /* Reaching this point means enable callback was already called */
    /* Must be released prior to calling cdc_read */
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));
    
    /* Now, even if a disable cb is invoked, al_ser_base_alloc_buffer_and_setup_read 
    checks instance_enabled_by_cdc_fd (without interference by FD) prior to calling 
    cdc_read */

    /* Assuming there can't be a call to disable cb and then enable cb before 
    the call to hsu_cdc_fd_api_cdc_read inside al_ser_base_alloc_buffer_and_setup_read */

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

    /* Setup a new read: */    
    read_operation_successful = 
      al_ser_base_alloc_buffer_and_setup_read(ser_if_data_ptr, &cdc_read_res);
    if (read_operation_successful == FALSE)
    {
      HSU_MSG_ERROR("hsu_al_ser_base_open: al_ser_base_alloc_buffer_and_setup_read \
          returned FALSE, FD error=%d", cdc_read_res, 0, 0);
      HSU_ASSERT((uint32)"hsu_al_ser_base_open: failed" == NULL);
    }
  }
  else
  {
    /* Reaching this point means enable callback was not yet called, 
    but there is no problem it would be called from now on */
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

  }

  HSU_MSG_LOW("hsu_al_ser_base_open: Operation Successful.", 0, 0, 0);
  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_close()
===========================================================================*/
void hsu_al_ser_base_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  hsu_al_ser_base_op_complete_cb close_cb,
 boolean abort_io_operations_immediately
)
{
  HSU_MSG_LOW("hsu_al_ser_base_close: Checking if ser_if_data_ptr (0x%x) valid...", 
    ser_if_data_ptr, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* Save close callback (invoked when either all I/O operations were aborted, 
  or when Tx was flushed and after which, Rx was aborted */
  ser_if_data_ptr->close_complete_cb = close_cb;

  if (abort_io_operations_immediately == TRUE)
  {
  HSU_ASSERT(ser_if_data_ptr->num_of_aborts_issued_prior_to_close == 0);

  #ifdef FEATURE_HSU_TEST
#error code not present
  #endif /* FEATURE_HSU_TEST */


  /* Critical Section Start */
  rex_enter_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

  /* First check if interface isn't in a middle of a read operation (can only 
  be changed to TRUE by an enabled operation) AND is currently not Tx'ing 
  (note that hsu_al_ser_transmit is app context) */

  if ((ser_if_data_ptr->read_oper_is_in_progress == FALSE) &&
    (ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy == FALSE))
  {
    /* No more aborts - call close immediately */
      al_ser_base_internal_close(ser_if_data_ptr);

    /* Critical Section End (released AFTER internal close, so that if suddenly 
    an enabled callback is received no further read operations would be done) 
      Note that al_ser_base_internal_close does NOT grab giant mutex */
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

    return;
  }
  /* Else: abort read AND/OR abort write
  (when it is done - complete close) */

  /* Note: The update of ser_if_data_ptr->num_of_aborts_issued_prior_to_close 
  is done here, prior to calling al_ser_base_internal_abort_*, 
  instead of INSIDE the implementation of al_ser_base_internal_abort_*. 
  This is due to the following scenario:

  1. Issued al_ser_base_internal_abort_read, (and no scatter DMOV in progress) 
  2. Inside there, num_of_aborts_issued_prior_to_close is set to 1
  3. cdc_abort_read called, but it is synchronous, so the read completion callback
  (with CANCELLED) is invoked, setting num_of_aborts_issued_prior_to_close to 0
    4. This causes a call to al_ser_base_internal_close prior to abort write.
  */

  #ifdef FEATURE_HSU_TEST
#error code not present
  #endif /* FEATURE_HSU_TEST */

  /* The critical section is still not released in order to prevent the scenario:
  1. close issued (app context), and there is a cdc_write in progress
  2. It checks: dmov_gather_or_fd_write_or_timer_busy == TRUE and the if statement is taken.
  3. Tx Done is called immediately, and there is no more data in Tx WMQ
  4. (back to app context) num_of_aborts_issued_prior_to_close is updated, 
  but will never be decreased, so final close will never be called.

  In Tx Done there is the corresponding Crit Section to prevent the scenario.
  */

  /* Check if client isn't in a middle of a read operation (can only 
  be changed to TRUE by an enabled operation, by invoking 
  al_ser_base_alloc_buffer_and_setup_read) */
  if (ser_if_data_ptr->read_oper_is_in_progress == TRUE)
  {
      /* Before updating ser_if_data_ptr->num_of_aborts_issued_prior_to_close 
      check if abort write will be issued as well: */
      if (ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy == TRUE)  
    {
      /* num_of_aborts_issued_prior_to_close is immediately set to max value, 
      so it is an atomic operation. */
      ser_if_data_ptr->num_of_aborts_issued_prior_to_close = 
        AL_SER_BASE_MAX_IO_ABORTS_BEFORE_CLOSE;

      /* Critical Section End */
      rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

      al_ser_base_internal_abort_write(ser_if_data_ptr);
    }
    else /* only abort read is needed: */
    {
      ++ser_if_data_ptr->num_of_aborts_issued_prior_to_close;

      /* Critical Section End */
      rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */
    }

    al_ser_base_internal_abort_read(ser_if_data_ptr);
  }
    else if (ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy == TRUE)
  {
    /* Only abort write is needed: */
    ++ser_if_data_ptr->num_of_aborts_issued_prior_to_close;

    /* Critical Section End */
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

    al_ser_base_internal_abort_write(ser_if_data_ptr);
  }
  }
  else /* (abort_io_operations_immediately != TRUE) */
  {
    /* if abort_io_operations_immediately is FALSE then 
    Flush Tx should be issued with an internal callback */
    hsu_al_ser_base_flush_tx(ser_if_data_ptr, 
      al_ser_base_internal_flush_tx_done_issued_by_close);
  }  
}

/*===========================================================================
FUNCTION hsu_al_base_transmit()
===========================================================================*/
void hsu_al_ser_base_transmit
( 
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  dsm_item_type *tx_ptr
)
{
  uint32 tx_ptr_chain_length = 0;
  void* buffer_to_send = NULL;
  uint16 size_of_dsm_item = 0;
  boolean ret_val = FALSE;
  boolean tx_ptr_enqueued = FALSE;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);
  HSU_ASSERT(tx_ptr != NULL);

  /* Verify that the max buff size is big enough to handle at least one 
  typical DSM item used in Tx. This should have been done in hsu_al_ser_base_open,
  however, at that time it is unknown from which pool are the items taken from, only
  at the very first time hsu_al_ser_transmit is called.
  Use the DSM chain's first item's size field, instead of the macro:
  DSM_POOL_ITEM_SIZE(pool_id); 
  Since for the pool ID: DSM_DUP_ITEM_POOL that value is zero */
  size_of_dsm_item = tx_ptr->size;
  HSU_ASSERT(ser_if_data_ptr->max_buff_size_for_write >= size_of_dsm_item);

  /* No need to guard this check from enable/disable cb:
  1. If ser_if_data_ptr->instance_enabled_by_cdc_fd == FALSE is taken and enable cb invoked,
  then the packet is simply discarded (which would have happened if enable cb was invoked a 
  little bit later, after exiting this function).

  2. Otherwise, even if disable cb is invoked, all paths ultimately lead into the AL task
  calling al_ser_base_command_write, which cannot be interrupted by enabled/disabled cb.
  So it is enough that the check for ser_if_data_ptr->instance_enabled_by_cdc_fd is done
  there.
  */
  if (ser_if_data_ptr->instance_enabled_by_cdc_fd == FALSE)
  {
    /* Instance not enabled, discard packet: */
    dsm_free_packet(&tx_ptr);
    return;
  }

  tx_ptr_chain_length = dsm_length_packet(tx_ptr);

  if (tx_ptr_chain_length == 0)
  {
    /* Zero length DSM chain, discard packet: */
    HSU_MSG_ERROR("hsu_al_ser_base_transmit: length of tx_ptr is zero!", 
      0, 0, 0);
    dsm_free_packet(&tx_ptr);
    return;
  }

  /* Check if there's a need to invoke DMOV or not: */
  if (ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy == TRUE)
  {
    #ifdef FEATURE_HSU_TEST
#error code not present
    #endif /* FEATURE_HSU_TEST */

    /* Enter Crit Section */
    /* This is to prevent the following scenario:
    - The app has a higher priority than HS-USB task
    - If this is in the middle of Tx Done, no more data in WMQ and just before
    marking dmov_gather_or_fd_write_or_timer_busy as FALSE a context switch occurs and
    the app calls hsu_al_ser_base_transmit
    - The app sees dmov_gather_or_fd_write_or_timer_busy is TRUE, so it enqueues data
    - Tx done does not know new data has been enqueued, so it does nothing (should have
    sent the new data).
    - Not only that but if a new call to hsu_al_ser_base_transmit occurs, 
    the new data will be transmitted BEFORE the one residing in the queue.
    */
    rex_enter_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

    /* Enqueue chain in WMQ: */
    dsm_enqueue(ser_if_data_ptr->tx_wmq_ptr, &tx_ptr);
    tx_ptr_enqueued = TRUE;

    if (al_ser_base_tx_aggr_is_used(ser_if_data_ptr))
    {
      /* Check if a request to flush has already been issued. */
      if (!(ser_if_data_ptr->tx_aggr_flush_next_aggr_packets))
      {    
        if (dsm_queue_cnt(ser_if_data_ptr->tx_wmq_ptr) >= 
          ser_if_data_ptr->max_buff_size_for_write)
        {
            al_ser_base_cancel_aggregation_and_flush(ser_if_data_ptr);        
        }     
      }
    }

    /* Leave Crit Section */
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

    /* The following code is to prevent the following scenario:
    The Tx WMQ is empty now, there was a pending 
    cdc_write but no hsu_al_ser_base_i_tx_done called yet. 
    Now:
    1. The app calls hsu_al_ser_base_transmit.
    2. It reads ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy == TRUE
    3. hsu_al_ser_base_i_tx_done invoked.
    4. It sees that the WMQ is empty, so ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy is set 
    to FALSE.
    5. (back to hsu_al_ser_base_transmit): it enqueues the chain, but was free to Tx it!
    */

    #ifdef FEATURE_HSU_TEST
#error code not present
    #endif /* FEATURE_HSU_TEST */

    if (ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy == TRUE)
    {
      /* hsu_al_ser_base_i_tx_done was not invoked, so since the chain was
      already queued nothing more to do here...*/
      return;
    }

    /* The following code handles the four possible scenarios: */

    /* 1. The scenario mentioned above - in this case WMQ contains only the 
    tx_ptr which was given in calling hsu_al_ser_base_transmit,
    so there is a need to "dequeue" it and send it to DMOV right away */

    /* 2. 
    - WMQ isn't empty in the first place
    - The FD already acked for what was previously sent +
    it was quick enough to not only send everything that is in the WMQ 
    (including this tx_ptr!) to DMOV +
    DMOV is quick enough to have finished gathering and calls cdc_write +
    FD is quick enough to also ack for that cdc_write 
    (so now WMQ is empty as well as dmov_gather_or_fd_write_or_timer_busy == FALSE!).
    In this case, the tx_ptr must NOT be sent 
    (it points to an already freed DSM chain)! Therefore, it is overwritten to NULL. */

    /* 3.
    WMQ is empty and we handle the tx_ptr which was given in calling hsu_al_ser_base_transmit() */

    /* 4.
    WMQ isn't empty, and we didn't dequeue the tx_ptr which was given in calling 
    hsu_al_ser_base_transmit(). In this case: enqueue tx_ptr and dequeue the DSM chain
    that was stored in the queue. */

  }

  /* (In light of the above scenario) 
  Re-Check if there's a need to invoke DMOV or not: */
  if (ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy == FALSE)
  {
    /* DMOV is free: mark it now as busy */
    ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy = TRUE;
    if (!dsm_is_wm_empty(ser_if_data_ptr->tx_wmq_ptr))
    {
      if (!tx_ptr_enqueued)
      {
        /* In case WMQ is not empty (as it should be), enqueue the new DSM chain and handle the 
        DSM chain that is already stored in the queue */
        HSU_MSG_ERROR( "hsu_al_ser_base_transmit: new data arrived though wmq is not empty.", 
        0, 0, 0); 
        dsm_enqueue(ser_if_data_ptr->tx_wmq_ptr, &tx_ptr);
      }
      /* 2 possible scenarios to reach here: 
      1. dequeue the DSM chain that was enqueued when dmov_gather_or_fd_write_or_timer_busy = TRUE
      2. dequeue previous data that was stored in the queue. */
      tx_ptr = dsm_dequeue(ser_if_data_ptr->tx_wmq_ptr);
    }
    else /* dsm_is_wm_empty = TRUE */ 
    {
      /* If we are here and tx_ptr_enqueued = TRUE it means that the tx_ptr of which
      this function was issued for was already transmitted.
      Since dsm_is_wm_empty = TRUE we mark 
      dmov_gather_or_fd_write_or_timer_busy = FALSE and return. */
      if (tx_ptr_enqueued)
      {
        HSU_MSG_HIGH( "hsu_al_ser_base_transmit: enqueue dsm chain was already transmitted .", 
        0, 0, 0); 
        ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy = FALSE;
        return;
      }
      /*else: handle the tx_ptr of which this function was issued for */
    }

    if (al_ser_base_tx_aggr_is_used(ser_if_data_ptr))
    {
      al_ser_base_start_aggregation_via_timer(ser_if_data_ptr, tx_ptr);
      return;
    }

    /* If chain is bigger than max_buff_size_for_write, split it: */
    if (tx_ptr_chain_length > ser_if_data_ptr->max_buff_size_for_write)
    {
      ret_val = hsu_al_dsm_utils_split_dsm_chain(tx_ptr->pool_id, tx_ptr, 
        &(ser_if_data_ptr->remaining_chain_to_tx_ptr), 
        ser_if_data_ptr->max_buff_size_for_write);
      HSU_ASSERT(ret_val);
      /* Update new length of chain to Tx */
      tx_ptr_chain_length = dsm_length_packet(tx_ptr);
    }

    /* Alloc new buffer: */
    buffer_to_send = hsu_al_ser_i_alloc_buffer_for_read_write(
      ser_if_data_ptr->memory_pool_for_tx, tx_ptr_chain_length);
    if (buffer_to_send == NULL)
    {
      HSU_MSG_ERROR("hsu_al_ser_base_transmit: Couldn't alloc buffer! mem_pool=0x%x size=%u", 
        ser_if_data_ptr->memory_pool_for_tx, tx_ptr_chain_length, 0);
      HSU_ASSERT((uint32)"hsu_al_ser_base_transmit: alloc buffer failed!" == NULL);
    }

    /* Request DMOV to gather DSM chain into the contiguous buffer: */

    ser_if_data_ptr->size_of_curr_buff_for_write = tx_ptr_chain_length;

    ret_val = hsu_al_dmov_api_gather_dsm_chain(tx_ptr, buffer_to_send, 
      tx_ptr_chain_length, al_ser_base_dmov_gather_callback, ser_if_data_ptr);
    if (ret_val == FALSE)
    {
      ser_if_data_ptr->size_of_curr_buff_for_write = 0;

      HSU_MSG_ERROR("hsu_al_ser_base_transmit: *_gather_dsm_chain returned FALSE! tx_ptr=0x%x\
          buffer_to_send=0x%x chain_length=%u", 
          tx_ptr, buffer_to_send, tx_ptr_chain_length);
      HSU_MSG_ERROR("hsu_al_ser_base_transmit: *_gather_dsm_chain returned FALSE! callback=0x%x\
          user_data=0x%x", al_ser_base_dmov_gather_callback, ser_if_data_ptr, 0);

      HSU_ASSERT((uint32)"hsu_al_ser_base_transmit: DMOV gather failed!" == NULL);
    }
  }
}

/*===========================================================================
FUNCTION hsu_al_ser_base_flush_tx()
===========================================================================*/
void hsu_al_ser_base_flush_tx
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  hsu_al_ser_base_op_complete_cb flush_tx_cb
)
{
  HSU_MSG_LOW("*_flush_tx: Checking if ser_if_data_ptr (0x%x) valid...", 
    ser_if_data_ptr, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);  

  /* If  flush_tx_in_progress it means that the application already called
  _flush_tx but the flush_tx_cb() wasn't called  yet.  */
  if (ser_if_data_ptr->flush_tx_in_progress == TRUE)
  {
    HSU_MSG_HIGH("*_flush_tx: Already called but didn't return yet.",0,0,0);
    return;
  }
  ser_if_data_ptr->flush_tx_complete_cb = flush_tx_cb;
  ser_if_data_ptr->flush_tx_in_progress = TRUE;

  if (ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy == FALSE)
  {
    /* No more data to send, call Flush Tx callback: */
    HSU_MSG_LOW("*_flush_tx: Operation Successful (no more data).",
      0, 0, 0);

    if (ser_if_data_ptr->flush_tx_complete_cb != NULL)
    {
    ser_if_data_ptr->flush_tx_complete_cb(ser_if_data_ptr);
    ser_if_data_ptr->flush_tx_complete_cb = NULL;
    }
    ser_if_data_ptr->flush_tx_in_progress = FALSE;
    return;
  }

  /* Arm the flush_tx timer. Need to do so before calling
     al_ser_base_cancel_aggregation_and_flush, to avoid arming the timer after
     all TX had already finished */
  rex_set_timer(&(ser_if_data_ptr->flush_tx_timer),
    (rex_timer_cnt_type)AL_SER_BASE_FLUSH_TX_TIMEOUT_MS);

  /* else: ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy == TRUE */
  if (al_ser_base_tx_aggr_is_used(ser_if_data_ptr))
  {
    al_ser_base_cancel_aggregation_and_flush(ser_if_data_ptr);
  }
  /* else: (aggregation isn't being used) do nothing - 
  when the last Tx done is called, it will cause the same invocation as above... */
}

/*===========================================================================
FUNCTION hsu_al_ser_base_ioctl()
===========================================================================*/
boolean hsu_al_ser_base_ioctl
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  sio_ioctl_cmd_type ioctl_cmd,
  sio_ioctl_param_type* ioctl_param_ptr
)
{
  HSU_MSG_LOW("*_ioctl: Checking if ser_if_data_ptr (0x%x) valid...", 
    ser_if_data_ptr, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE); 

  /* It doesn't make sense to call ioctl if the stream is not open */
  HSU_ASSERT(ser_if_data_ptr->stream_id != SIO_NO_STREAM_ID);

  /* Handles only the following IOCTLs:
  SIO_IOCTL_SET_FLOW_CTL
  SIO_IOCTL_GET_FLOW_CTL

  SIO_IOCTL_CHANGE_BAUD_RATE
  SIO_IOCTL_CHANGE_BAUD_NOW
  */
  switch (ioctl_cmd)
  {
    case SIO_IOCTL_SET_FLOW_CTL:
    {
      ser_if_data_ptr->ioctl_flow_ctl_mirror = ioctl_param_ptr->flow_ctl;
      break;
    }
    case SIO_IOCTL_GET_FLOW_CTL:
    {
      ioctl_param_ptr->flow_ctl = ser_if_data_ptr->ioctl_flow_ctl_mirror;
      break;
    }
    case SIO_IOCTL_CHANGE_BAUD_RATE:
    case SIO_IOCTL_CHANGE_BAUD_NOW:
    {
      /* Do nothing, baud rate is not supported in USB...*/
      break;
    }
    default:
    {
      HSU_MSG_HIGH("*_ioctl: cmd %u not handled", ioctl_cmd, 0, 0);
      return FALSE;
    }
  }
  return TRUE;
}

#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* FEATURE_HSU_TEST && !T_REXNT */

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR INTERNAL MODULE (hsu_al_ser_base_i.h)

==============================================================================*/
/*===========================================================================
FUNCTION hsu_al_ser_base_i_ctx_is_valid()
===========================================================================*/
boolean hsu_al_ser_base_i_ctx_is_valid
(
  const hsu_al_ser_base_data_for_ser_if* ctx
)
{
  if (ctx == NULL)
  {
    HSU_MSG_ERROR("hsu_al_ser_base_i_ctx_is_valid: ctx is NULL!", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_HSU_TEST 
#error code not present
#endif /* FEATURE_HSU_TEST  */

  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_uninit()
===========================================================================*/
jresult_t hsu_al_ser_base_i_uninit
(
  cdc_appctx_t ctx
)
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;

  HSU_MSG_HIGH("*_i_uninit: entered", 0, 0, 0);

  HSU_MSG_LOW("*_i_uninit: Checking if ctx (0x%x) valid...", ctx, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /*
  When acm_init/obex_init is issued, this field must point to a valid FD !
  */
  ser_if_data_ptr->instance_of_fd_used = NULL;

  HSU_MSG_HIGH("*_i_uninit: exited", 0, 0, 0);

  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_rx_done()
===========================================================================*/
void hsu_al_ser_base_i_rx_done
(
  cdc_iostatus_t status,
  void* buffer_ptr,
  juint32_t size,
  jbool_t transfer_complete,
  cdc_appctx_t ctx
)
{
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = NULL;
  boolean dmov_api_oper_successful = FALSE;
  dsm_item_type* dsm_chain_head_ptr = NULL;

  /* It doesn't make any sense that the FD is forwarding NULL ptr as buffer 
  to the app...*/
  HSU_ASSERT(buffer_ptr != NULL);

  app_data_ptr = (hsu_al_ser_base_data_for_ser_if*)ctx;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(app_data_ptr) == TRUE);

  if (status != CDC_TRANSFER_OK)
  {
    /* Do not move buffer to upper layer ! */
    if (status == CDC_TRANSFER_CANCELLED)
    {
      /* No further calls to cdc_read */
      app_data_ptr->read_oper_is_in_progress = FALSE;

      HSU_MSG_HIGH("*_i_rx_done: Rx cancelled", 0, 0, 0);
      hsu_al_ser_i_free_buffer_from_read_write(app_data_ptr->memory_pool_for_rx, 
        buffer_ptr);

      /* In case abort read was issued (e.g. close), 
      and the FD notified us it aborted the read, 
      then if no more aborts are needed, finish the call to close */
      if (app_data_ptr->num_of_aborts_issued_prior_to_close > 0)
      {
        --app_data_ptr->num_of_aborts_issued_prior_to_close;

        if (app_data_ptr->num_of_aborts_issued_prior_to_close == 0)
        {
          /* Check if close has already been called: */
          /* This is in order to prevent the following scenario:
          1. Close issued (with no callback) - abort read & write issued
          2. hsu_al_ser_base_i_rx_done called, sees num_of_aborts > 0 
          3. it decreases it by one, but then the gather DMOV callback interrupts.
          4. gather callback decreases num_of_aborts to zero and calls 
          al_ser_base_internal_close 
          5. Now we return here, check 
          app_data_ptr->num_of_aborts_issued_prior_to_close == 0 
          6. call close again*/

          /* Note: If al_ser_base_has_already_been_closed returns FALSE, this also 
          means DMOV callback cannot be called now
          (because num_of_aborts_issued_prior_to_close == 0) */
          if (al_ser_base_has_already_been_closed(app_data_ptr) == FALSE)
          {
            al_ser_base_internal_close(app_data_ptr);
          }
        }
      }

      return;    
    }
    else /* CDC_TRANSFER_ERROR */
    {
      jresult_t cdc_read_res;
      HSU_MSG_ERROR("hsu_al_ser_base_i_rx_done: Rx unsuccessful; status=%d,\
          buffer_ptr=0x%x, size=%u", status, buffer_ptr, size);
      HSU_MSG_ERROR("hsu_al_ser_base_i_rx_done: Rx unsuccessful; transfer_complete=%d,\
          ctx=0x%x", transfer_complete, ctx, 0);
      HSU_MSG_ERROR("hsu_al_ser_base_i_rx_done: Rx unsuccessful; app_stream_id=%d,\
          app instance of FD used=0x%x", app_data_ptr->stream_id, 
          app_data_ptr->instance_of_fd_used, 0);

      /* No need to free buffer - just re-use it... */
      cdc_read_res = hsu_cdc_fd_api_cdc_read(app_data_ptr->instance_of_fd_used, buffer_ptr, 
        app_data_ptr->buff_size_for_read);
      if (cdc_read_res != HSU_JSUCCESS)
      {
        HSU_MSG_ERROR("*_i_rx_done: zero length packet AND couldn't perform cdc_read", 
                     0, 0, 0);

        hsu_al_ser_i_free_buffer_from_read_write(app_data_ptr->memory_pool_for_rx, 
          buffer_ptr);
      }
      return;
    }
  }
  /* else if (status == CDC_TRANSFER_OK) */

  if (size == 0)
  {
    /* In case received a zero length packet due to transfer being exactly a 
    multiple of max_packet_size AND it filled the entire buffer, try another read 
    with the same buffer */
    jresult_t cdc_read_res;

    HSU_MSG_HIGH("*_i_rx_done: received zero length packet. Issuing a new read.", 0, 0, 0);

    /* No need to free buffer - just re-use it... */
    cdc_read_res = hsu_cdc_fd_api_cdc_read(app_data_ptr->instance_of_fd_used, buffer_ptr, 
      app_data_ptr->buff_size_for_read);
    if (cdc_read_res != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR("*_i_rx_done: zero length packet AND couldn't perform cdc_read",
                   0, 0, 0);

      hsu_al_ser_i_free_buffer_from_read_write(app_data_ptr->memory_pool_for_rx, 
        buffer_ptr);
    }
    return;
  }

  /* Create a DSM item chain: */
  dsm_chain_head_ptr = 
    hsu_al_dsm_utils_generate_dsm_chain(app_data_ptr->pool_id_of_new_dsm_items_to_alloc, 
    (uint32)size);
  if (dsm_chain_head_ptr == NULL)
  {
    /* If no more DSM items, try another read with the same buffer */
    /* TODO: Is this a common situation??? */
    jresult_t cdc_read_res;

    /* No need to free buffer - just re-use it... */
    cdc_read_res = hsu_cdc_fd_api_cdc_read(app_data_ptr->instance_of_fd_used, buffer_ptr, 
      app_data_ptr->buff_size_for_read);

    if (cdc_read_res != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR("hsu_al_ser_base_i_rx_done: hsu_al_dsm_utils_generate_dsm_chain \
                    failured AND couldn't perform cdc_read", 0, 0, 0);

      hsu_al_ser_i_free_buffer_from_read_write(app_data_ptr->memory_pool_for_rx, 
        buffer_ptr);     
    }
    return;
  }

  /* Break the buffer into DSM item chain, etc. 
  callback will be invoked when DMOV calls the dmov callback... */
  dmov_api_oper_successful = hsu_al_dmov_api_scatter_dsm_chain(buffer_ptr, 
    size, dsm_chain_head_ptr, al_ser_base_dmov_scatter_callback, app_data_ptr);
  if (dmov_api_oper_successful == FALSE)
  {
    /* Failure can only be due to dmov_transfer returning FALSE:
    buffer != NULL, size != 0 are checked here.
    dsm_chain_head_ptr != NULL because hsu_al_dsm_utils_generate_dsm_chain succeeded.
    al_ser_base_dmov_scatter_callback != NULL since it's a real function (has an addr).
    size <= length of dsm_chain_head_ptr, since hsu_al_dsm_utils_generate_dsm_chain succeeded
    (There is an ASSERT in case any of the DSM items has a capacity smaller 
    than expected).
    */
    HSU_MSG_ERROR("hsu_al_ser_base_i_rx_done: hsu_al_dmov_api_scatter_dsm_chain returned\
                  failure; buffer_ptr=0x%x, size=%u, dsm_chain_head_ptr=0x%x",
                  buffer_ptr, size, dsm_chain_head_ptr);
    HSU_MSG_ERROR("hsu_al_ser_base_i_rx_done: hsu_al_dmov_api_scatter_dsm_chain returned\
                  failure; al_ser_base_dmov_scatter_callback=0x%x, app_data_ptr=0x%x", 
                  al_ser_base_dmov_scatter_callback, app_data_ptr, 0);

    HSU_ASSERT((uint32)"Rx callback: unexpected failure!" == NULL);

  }
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_tx_done()
===========================================================================*/
void hsu_al_ser_base_i_tx_done
(
  cdc_iostatus_t status,
  void *buffer_ptr,
  cdc_appctx_t ctx
)
{
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = NULL;

  app_data_ptr = (hsu_al_ser_base_data_for_ser_if*)ctx;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(app_data_ptr) == TRUE);

  /* It doesn't make any sense that the FD is forwarding NULL ptr as buffer 
  to the AL...*/
  HSU_ASSERT(buffer_ptr != NULL);

  /* Enter Crit Section (to protect abort write operation)
  (see corresponding comment in hsu_al_ser_base_close) */
  rex_enter_crit_sect(&(app_data_ptr->al_ser_base_access_guard));

  /* See corresponding comment in hsu_al_ser_i_rx_done. */
  if (app_data_ptr->num_of_aborts_issued_prior_to_close > 0)
  {
    HSU_MSG_HIGH("*_i_tx_done: Abort write issued", 0, 0, 0);

    /* Release the no longer needed buffer & free all DSM items in WMQ */
    al_ser_base_discard_all_tx_data(app_data_ptr, buffer_ptr);

    --app_data_ptr->num_of_aborts_issued_prior_to_close;
    if (app_data_ptr->num_of_aborts_issued_prior_to_close == 0)
    {
      /* Check if close has already been called: */
      /* This is in order to prevent the following scenario:
      1. Close issued (with no callback) - abort read & write issued
      2. hsu_al_ser_base_i_tx_done called, sees num_of_aborts > 0 
      3. it decreases it by one, but then the scatter DMOV callback interrupts.
      4. scatter callback decreases num_of_aborts to zero and calls 
      al_ser_base_internal_close 
      5. Now we return here, check 
      app_data_ptr->num_of_aborts_issued_prior_to_close == 0 
      6. call close again*/
      if (al_ser_base_has_already_been_closed(app_data_ptr) == FALSE)
      {
        al_ser_base_internal_close(app_data_ptr);
      }
    }

    /* Exit crit section */
    rex_leave_crit_sect(&(app_data_ptr->al_ser_base_access_guard));
    return ;
  }

  if (status == CDC_TRANSFER_CANCELLED)
  {
    /* If app_data_ptr->cdc_abort_write_issued == TRUE (and 
       status == CDC_TRANSFER_CANCELLED) it means that flush_tx timer expired
       and cdc_abort_write was issued 
     */
    if (app_data_ptr->cdc_abort_write_issued)
    {
      app_data_ptr->cdc_abort_write_issued = FALSE ;
      dsm_free_packet(&(app_data_ptr->remaining_chain_to_tx_ptr));
      dsm_empty_queue(app_data_ptr->tx_wmq_ptr);
    }
  }
 

  /* Exit crit section */
  rex_leave_crit_sect(&(app_data_ptr->al_ser_base_access_guard));

  if (status == CDC_TRANSFER_ERROR)
  {
    HSU_MSG_ERROR("hsu_al_ser_base_i_tx_done: Tx unsuccessful; status=%d,\
        buffer_ptr=0x%x ctx=0x%x", status, buffer_ptr, ctx);
    HSU_MSG_ERROR("hsu_al_ser_base_i_tx_done: Tx unsuccessful; app_stream_id=%d,\
        app instance of FD used=0x%x", app_data_ptr->stream_id, 
        app_data_ptr->instance_of_fd_used, 0);
    /*If a transfer error occurred we ignore it and try to retransmit again*/
  }

  /* If status == CDC_TRANSFER_CANCELLED then treat this as if CDC_TRANSFER_OK
  because having a transfer canceled, which isn't by a close operation, 
  doesn't mean the entire queue needs to be emptied. */

  /* Reclaim the used buffer: */
  hsu_al_ser_i_free_buffer_from_read_write(app_data_ptr->memory_pool_for_tx,
    buffer_ptr);

  if (al_ser_base_tx_aggr_is_used(app_data_ptr))
  {
    al_ser_base_send_more_data_or_aggregate(app_data_ptr);
  }
  else
  {
    /* tx aggr delay not being used, proceed to send remaining data*/
    al_ser_base_send_all_tx_data(app_data_ptr);
  }
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_enabled()
===========================================================================*/
jresult_t hsu_al_ser_base_i_enabled
(
  cdc_appctx_t ctx,
  boolean issue_new_read
)
{
  boolean operation_successful;
  jresult_t cdc_fd_error;
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = NULL;
  app_data_ptr = (hsu_al_ser_base_data_for_ser_if*)ctx;

  HSU_MSG_HIGH("*_i_enabled: Entered", 0, 0, 0);

  HSU_MSG_LOW("*_i_enabled: Checking if ctx (0x%x) valid...", ctx, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(app_data_ptr) == TRUE);

  /* Enter Crit Section */
  /* See corresponding comment in hsu_al_ser_base_open */
  rex_enter_crit_sect(&(app_data_ptr->al_ser_base_access_guard));

  /* Make sure no two enabled cbs are received without a disabled in between*/
  HSU_ASSERT(app_data_ptr->instance_enabled_by_cdc_fd == FALSE);

  app_data_ptr->instance_enabled_by_cdc_fd = TRUE;

  /* Leave Crit Section */
  rex_leave_crit_sect(&(app_data_ptr->al_ser_base_access_guard));

  /* Check if the app is in closing process (or already closed): */
  /* If so, don't do any reads...*/
  HSU_MSG_LOW("*_i_enabled: Checking if app is closed/closing process", 
    0, 0, 0);

  if ((app_data_ptr->num_of_aborts_issued_prior_to_close > 0)
    || (al_ser_base_has_already_been_closed(app_data_ptr)) == TRUE)
  {
    HSU_MSG_HIGH("*_i_enabled: Exit (app closed/closing)", 0, 0, 0);
    return HSU_JSUCCESS;
  }

  if (issue_new_read)
  {
    /* Setup a new read, but only if open already issued (or if interface is using read):*/
    if (app_data_ptr->memory_pool_for_rx == NULL)
    {
      /* Not using read, or open not yet issued: */
      HSU_MSG_HIGH("*_i_enabled: Exit (no Rx mem. pool/open not issued yet)", 0, 0, 0);
      return HSU_JSUCCESS;
    }

    operation_successful = al_ser_base_alloc_buffer_and_setup_read(app_data_ptr, 
      &cdc_fd_error);

    if (operation_successful == FALSE)
    {
      HSU_MSG_ERROR("hsu_al_ser_base_i_enabled: al_ser_base_alloc_buffer_and_setup_read \
          returned FALSE, FD error=%d", cdc_fd_error, 0, 0);
      HSU_ASSERT((uint32)"hsu_al_ser_base_i_enabled: failed" == NULL);
    }
    HSU_MSG_HIGH("*_i_enabled: Exit (after requesting a read operation)", 0, 0, 0);
  }
  else
  {
    HSU_MSG_HIGH("*_i_enabled: Exit (didn't issue a read operation!)", 0, 0, 0);
  }

  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_disabled()
===========================================================================*/
jresult_t hsu_al_ser_base_i_disabled
(
  cdc_appctx_t ctx
)
{
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = NULL;
  app_data_ptr = (hsu_al_ser_base_data_for_ser_if*)ctx;

  HSU_MSG_HIGH("*_i_disabled: Entered", 0, 0, 0);

  HSU_MSG_LOW("*_i_disabled: Checking if ctx (0x%x) valid...", ctx, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(app_data_ptr) == TRUE);

  /* Make sure no two disabled cbs are received without an enabled in between*/
  HSU_ASSERT(app_data_ptr->instance_enabled_by_cdc_fd == TRUE);


#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND

  /* Reset this since the host will renegotiate upon next enumeration. */
  app_data_ptr->overcome_1_2_byte_hw_crc_err = FALSE;

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */


  /* No need for critical section here. In all open/close/read/write scenarios
  Eventually there is a check if instance_enabled_by_cdc_fd is FALSE before
  invoking cdc_read / cdc_write.
  */
  app_data_ptr->instance_enabled_by_cdc_fd = FALSE;

  /* It is assumed that any abort read/writes will receive callbacks for */
  HSU_MSG_HIGH("*_i_disabled: Exit", 0, 0, 0);
  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION hsu_al_ser_base_i_resumed()
===========================================================================*/
jresult_t hsu_al_ser_base_i_resumed
(
  cdc_appctx_t ctx
)
{
  boolean operation_successful;
  jresult_t cdc_read_res;
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = NULL;
  app_data_ptr = (hsu_al_ser_base_data_for_ser_if*)ctx;

  HSU_MSG_HIGH("*_i_resumed: Entered", 0, 0, 0);

  HSU_MSG_LOW("*_i_resumed: Checking if ctx (0x%x) valid...", ctx, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(app_data_ptr) == TRUE);

  if (app_data_ptr->memory_pool_for_rx == NULL)
  {
    HSU_MSG_LOW("*_i_resumed: instance not using Rx buffer, nothing to do...", 
      0, 0, 0);
    return HSU_JSUCCESS;
  }

  if (app_data_ptr->instance_enabled_by_cdc_fd == FALSE)
  {
    HSU_MSG_LOW("*_i_resumed: instance not enabled, nothing to do...", 
      0, 0, 0);
    return HSU_JSUCCESS;
  }

  if (al_ser_base_has_already_been_closed(app_data_ptr) == TRUE)
  {
    HSU_MSG_LOW("*_i_resumed: app not using instance, nothing to do...", 
      0, 0, 0);
    return HSU_JSUCCESS;
  }

  operation_successful = al_ser_base_alloc_buffer_and_setup_read(app_data_ptr, 
    &cdc_read_res);
  if (operation_successful == FALSE)
  {
    HSU_MSG_ERROR("*_i_resumed: al_ser_base_alloc_buffer_and_setup_read \
        returned FALSE, FD error=%d", cdc_read_res, 0, 0);
    HSU_ASSERT((uint32)"*_i_resumed: failed" == NULL);
  }

  HSU_MSG_LOW("*_i_resumed: Exited", 0, 0, 0);

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION al_ser_base_dmov_scatter_callback

DESCRIPTION
DMOV callback to be invoked when DMOV scatter is done. Common to all serial
functions.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void al_ser_base_dmov_scatter_callback
(
  boolean operation_result,            /* DMOV operation successful or not */
  dsm_item_type* dsm_chain_ptr,   /* Pointer to DSM chain to return to app */
  void* buffer_ptr,               /* Buffer used in the DMOV operation     */
  void* user_ptr                  /* User data which was provided when 
                                  scatter operation was invoked         */
)
{
  hsu_al_ser_base_data_for_ser_if* app_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)user_ptr;

  HSU_ASSERT(buffer_ptr != NULL);

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(app_data_ptr) == TRUE);

  /* The following code handles the following scenario:
  1. hsu_al_dmov_api_scatter_dsm_chain was called  
  2. abort read was issued (e.g. call to close) 
  3. This callback was called - we must handle the abort read here, 
  because there is no guarantee there will ever be 
  another read done callback from FD (thus not handling the abort read) */
  if (app_data_ptr->num_of_aborts_issued_prior_to_close > 0)
  {
    /* Release the no longer needed buffer */
    hsu_al_ser_i_free_buffer_from_read_write(app_data_ptr->memory_pool_for_tx, 
      buffer_ptr);

    /* Don't forward the chain to app */
    dsm_free_packet(&dsm_chain_ptr);

    --app_data_ptr->num_of_aborts_issued_prior_to_close;
    if (app_data_ptr->num_of_aborts_issued_prior_to_close == 0)
    {
      al_ser_base_internal_close(app_data_ptr);
    }

    return;
  }

  if (operation_result == FALSE)
  {
    HSU_MSG_ERROR("al_ser_base_dmov_scatter_callback: DMOV returned failure;\
                  dsm_chain_ptr=0x%x, buffer_ptr=0x%x, user_ptr=0x%x", 
                  dsm_chain_ptr, buffer_ptr, user_ptr);
    HSU_MSG_ERROR("al_ser_base_dmov_scatter_callback: DMOV returned failure;\
                  app_data_ptr=0x%x, app stream id=%d", 
                  app_data_ptr, app_data_ptr->stream_id, 0);
    HSU_ASSERT((uint32)"DMOV scatter callback: unexpected failure!" == NULL);
    return ;
  }

  /* Either call the Rx callback or enqueue in WMQ */

  if (app_data_ptr->stream_mode == SIO_DS_RAWDATA_MODE)
  {
    dsm_item_type* cur_item;
    for (cur_item = dsm_chain_ptr; cur_item != NULL; cur_item = cur_item->pkt_ptr)
    {
      cur_item->kind = SIO_PAYLOAD;
    }
  }

  if (app_data_ptr->app_rx_cb_func != NULL)
  {
    app_data_ptr->app_rx_cb_func(&dsm_chain_ptr);
  }
  /* else: enqueue dsm chain into WMQ */
  else if (app_data_ptr->rx_wmq_ptr != NULL)
  {
    dsm_enqueue(app_data_ptr->rx_wmq_ptr, &dsm_chain_ptr);
  }
  else /* No Rx callback AND no WMQ - Not supposed to happen... */
  {
    HSU_MSG_ERROR("DMOV scatter callback: No Rx callback AND no Rx WMQ. \
                  Shouldn't happen!",0,0,0);
    dsm_free_packet(&dsm_chain_ptr);
  }

  /* Cleanup: Reclaim the buffer and set up a new cdc_read: */
  hsu_al_ser_i_free_buffer_from_read_write(app_data_ptr->memory_pool_for_rx, 
    buffer_ptr);

  /* The AL task will take it from here (in order to avoid INT context) 
  This will also signal it that there's a new command*/
  hsu_al_task_enqueue_cmd(al_ser_base_command_read, app_data_ptr);
}

/*===========================================================================

FUNCTION al_ser_base_dmov_gather_callback

DESCRIPTION
DMOV callback to be invoked when DMOV gather is done. Common to all serial
functions.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void al_ser_base_dmov_gather_callback
(
  boolean operation_result,       /* DMOV operation successful or not      */
  dsm_item_type* dsm_chain_ptr,   /* Pointer to DSM chain used in operation*/
  void* buffer_ptr,               /* Buffer to return to application       */
  void* user_ptr                  /* User data which was provided when 
                                  gather operation was invoked          */
)
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)user_ptr;

  HSU_ASSERT(buffer_ptr != NULL);

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* The following code handles the following scenario:
  1. hsu_al_dmov_api_gather_dsm_chain was called  
  2. abort write was issued (e.g. call to close) 
  3. FD invokes disable callback.
  4. This callback was called - if num_of_aborts_issued_prior_to_close is not 
  checked, then this would invoke the AL task which will call hsu_cdc_fd_api_cdc_write
  5. hsu_cdc_fd_api_cdc_write discovers instance is disabled, aborts all Tx data -> 
  but num_of_aborts_issued_prior_to_close is NOT updated!!!
  */
  if (ser_if_data_ptr->num_of_aborts_issued_prior_to_close > 0)
  {
    HSU_MSG_HIGH("al_ser_base_dmov_gather_callback: Abort write issued", 0, 0, 0);

    /* Release the no longer needed buffer & free all DSM items in WMQ */
    al_ser_base_discard_all_tx_data(ser_if_data_ptr, buffer_ptr);
    /* We also need to release dsm_chain_ptr since it is no longer in the WMQ */ 
    dsm_free_packet(&dsm_chain_ptr);

    --ser_if_data_ptr->num_of_aborts_issued_prior_to_close;
    if (ser_if_data_ptr->num_of_aborts_issued_prior_to_close == 0)
    {
      al_ser_base_internal_close(ser_if_data_ptr);
    }

    return ;
  }

  if (operation_result == FALSE)
  {
    HSU_MSG_ERROR("al_ser_base_dmov_gather_callback: DMOV returned failure;\
                  dsm_chain_ptr=0x%x, buffer_ptr=0x%x, user_ptr=0x%x", 
                  dsm_chain_ptr, buffer_ptr, user_ptr);
    HSU_MSG_ERROR("al_ser_base_dmov_gather_callback: DMOV returned failure;\
                  ser_if_data_ptr=0x%x, app stream id=%d", 
                  ser_if_data_ptr, ser_if_data_ptr->stream_id, 0);
    HSU_ASSERT((uint32)"DMOV gather callback: unexpected failure!" == NULL);
    return ;
  }

  /* The following scenario is possible: 

  1. The DMOV has been given a request to gather (could be for already queued data).
  2. The CDC FD invokes disable
  3. DMOV gather done called (INT context)
  4. We could have checked here if I/F is disabled (without CRIT section) but:

  Suppose the following different scenario happens:

  1. The DMOV has been given a request to gather (could be for already queued data).
  2. The CDC FD invokes disable - but before it wishes to update 
  ser_if_data_ptr->instance_enabled_by_cdc_fd, DMOV gather done called (INT context).
  4. DMOV reads the value (enabled) and continues.
  5. CDC FD updates the field to false
  6. AL task invokes cdc_write, but the I/F is already disabled! 

  Therefore, the check of enabled/disabled is moved to al_ser_base_command_write
  */

  /* Assuming buffer size saved is valid... */
  HSU_ASSERT(ser_if_data_ptr->size_of_curr_buff_for_write != 0);

  /* At this point the contents of the DSM chain have been copied, so it may be freed: */
  dsm_free_packet(&dsm_chain_ptr);

  /* The AL task will take it from here (in order to avoid INT context) */
  /* The buffer size was already saved in ser_if_data_ptr->size_of_curr_buff_for_write, 
  so it is known when the AL task invokes al_ser_base_command_write */

  /* The AL task will take it from here (in order to avoid INT context) 
  This will also signal it that there's a new command*/
  hsu_al_task_enqueue_cmd(al_ser_base_command_write, ser_if_data_ptr);
}

/*===========================================================================

FUNCTION al_ser_base_set_params_in_open_close

DESCRIPTION
Sets various fields in the given ser_if_data_ptr common in both 
open and close operations. In close operation, the specified params shall have
the default values.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_set_params_in_open_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr, 
                                      /* Pointer to Serial I/F Data struct */
  sio_stream_id_type stream_id,                           /* sio stream id */
  dsm_watermark_type* tx_wmq_ptr,                     /* Pointer to Tx WMQ */
  dsm_watermark_type* rx_wmq_ptr,                     /* Pointer to Rx WMQ */
  sio_rx_func_ptr_type app_rx_cb_func,                      /* Rx callback */
  dsm_mempool_id_enum_type pool_id_of_new_dsm_items_to_alloc,
                              /* Pool ID from where to alloc new DSM items */
  byte* memory_pool_for_rx,                             /* Mem pool for Rx */
  byte* memory_pool_for_tx,                             /* Mem pool for Tx */
  juint32_t buff_size_for_read, 
                          /* Typical size of contiguous buffer to allocate */
  sio_flow_ctl_type rx_flow_ctl,
                          /* Rx flow ctl type                              */
  sio_flow_ctl_type tx_flow_ctl,
                          /* Tx flow ctl type                              */
  juint32_t max_buff_size_for_write,
                          /* Max size of cont. buffer to allocate for Tx   */
 juint32_t max_buff_size_for_read,
 /* Max size of cont. buffer to allocate for Rx   */
  rex_timer_cnt_type tx_aggr_delay_in_msecs
                          /* Delay in msecs for allowing packet aggr.      */
)
{
  HSU_ASSERT(ser_if_data_ptr != NULL);

  ser_if_data_ptr->stream_id = stream_id;
  ser_if_data_ptr->tx_wmq_ptr = tx_wmq_ptr;
  ser_if_data_ptr->rx_wmq_ptr = rx_wmq_ptr;
  ser_if_data_ptr->app_rx_cb_func = app_rx_cb_func;
  ser_if_data_ptr->pool_id_of_new_dsm_items_to_alloc = pool_id_of_new_dsm_items_to_alloc;
  
  ser_if_data_ptr->buff_size_for_read = buff_size_for_read;
  ser_if_data_ptr->memory_pool_for_rx = memory_pool_for_rx;
  ser_if_data_ptr->memory_pool_for_tx = memory_pool_for_tx;

  ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy = FALSE;
  ser_if_data_ptr->num_of_aborts_issued_prior_to_close = 0;
  ser_if_data_ptr->close_complete_cb = NULL;
  ser_if_data_ptr->flush_tx_complete_cb = NULL;

  ser_if_data_ptr->ioctl_flow_ctl_mirror.rx_flow = rx_flow_ctl;
  ser_if_data_ptr->ioctl_flow_ctl_mirror.tx_flow = tx_flow_ctl;

  ser_if_data_ptr->size_of_curr_buff_for_write = 0;

  ser_if_data_ptr->read_oper_is_in_progress = FALSE;

  ser_if_data_ptr->max_buff_size_for_write = max_buff_size_for_write;

  ser_if_data_ptr->max_buff_size_for_read = max_buff_size_for_read;

  ser_if_data_ptr->remaining_chain_to_tx_ptr = NULL;

  ser_if_data_ptr->tx_aggr_delay_in_msecs = tx_aggr_delay_in_msecs;

  ser_if_data_ptr->tx_aggr_flush_next_aggr_packets = FALSE;

  ser_if_data_ptr->tx_aggr_flush_command_already_enqueued = FALSE;
}

/*===========================================================================

FUNCTION al_ser_base_internal_close

DESCRIPTION
Reverts various fields of the given data for serial I/F struct to default values,
and calls the app's close callback.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_internal_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                               /* Serial I/F data struct used in operation */
)
{
  hsu_al_ser_base_op_complete_cb close_cb = NULL;

  HSU_MSG_HIGH("al_ser_base_internal_close: Entered", 0, 0, 0);

  HSU_MSG_LOW("al_ser_base_internal_close: Checking if ser_if_data_ptr (0x%x) valid...",
    ser_if_data_ptr, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  HSU_ASSERT(ser_if_data_ptr->num_of_aborts_issued_prior_to_close == 0);

  close_cb = ser_if_data_ptr->close_complete_cb;

  /* Clear the flush timer */
  rex_clr_timer(&(ser_if_data_ptr->flush_tx_timer));

  /* Clean all fields set in open: */
  al_ser_base_set_params_in_open_close(ser_if_data_ptr, SIO_NO_STREAM_ID, NULL, NULL, 
    NULL, NULL, NULL, NULL, 0, SIO_MAX_FLOW, SIO_MAX_FLOW, 0, 0, 0);

#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* FEATURE_HSU_TEST && !T_REXNT */

  /* Finally, invoke the app close cb (if given): */
  if (close_cb != NULL)
  {
    close_cb(ser_if_data_ptr);
  }

  HSU_MSG_HIGH("al_ser_base_internal_close: Exit", 0, 0, 0);
}

/*===========================================================================

FUNCTION al_ser_base_internal_abort_read

DESCRIPTION
Requests the CDC FD to abort all read operations for the given ser_if_data_ptr

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_internal_abort_read
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                               /* Serial I/F data struct used in operation */
)
{
  HSU_MSG_HIGH("al_ser_base_internal_abort_read: Entered", 0, 0, 0);

  HSU_MSG_LOW("al_ser_base_internal_abort_read: Checking if ser_if_data_ptr (0x%x) valid...",
    ser_if_data_ptr, 0, 0);

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* Abort read: */
  /* Note that ser_if_data_ptr->memory_pool_for_rx must still exist - 
  since the read callback (with status CANCELLED) must free the buffer to the pool.*/
  /* Therefore, in the case of close, cleaning of ser_if_data_ptr would be invoked there */

  (void)hsu_cdc_fd_api_cdc_abort_read(ser_if_data_ptr->instance_of_fd_used);

  HSU_MSG_HIGH("al_ser_base_internal_abort_read: Exit", 0, 0, 0);
}

/*===========================================================================

FUNCTION al_ser_base_internal_abort_write

DESCRIPTION
Requests the CDC FD to abort all write operations for the given ser_if_data_ptr

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_internal_abort_write
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                               /* Serial I/F data struct used in operation */
)
{
  HSU_MSG_HIGH("al_ser_base_internal_abort_write: Entered", 0, 0, 0);

  HSU_MSG_LOW("al_ser_base_internal_abort_write: Checking if ser_if_data_ptr (0x%x) valid...",
    ser_if_data_ptr, 0, 0);

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* Abort write: */
  /* Note that ser_if_data_ptr->memory_pool_for_tx must still exist - since the write callback (with status CANCELLED) must free the buffer to the pool.*/
  /* Therefore, in the case of close, cleaning of ser_if_data_ptr would be invoked there */

  (void)hsu_cdc_fd_api_cdc_abort_write(ser_if_data_ptr->instance_of_fd_used);

  HSU_MSG_HIGH("al_ser_base_internal_abort_write: Exit", 0, 0, 0);
}

/*===========================================================================

FUNCTION al_ser_base_internal_flush_tx_done_issued_by_close

DESCRIPTION
Serves as a "client" callback for Flush Tx, whenever the client invoked 
hsu_al_ser_base_close, with the flag: abort_io_opertions_immediately being 
set to FALSE. Will simply abort any read operation (and eventually call 
al_ser_base_internal_close)

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_internal_flush_tx_done_issued_by_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                               /* Serial I/F data struct used in operation */
)
{
  HSU_MSG_HIGH("*_internal_flush_tx_done_issued_by_close: Entered", 0, 0, 0);

  HSU_MSG_LOW("*_internal_flush_tx_done_issued_by_close: Checking if \
              ser_if_data_ptr (0x%x) valid...", ser_if_data_ptr, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);


  /* Abort read (and eventually: clean the data struct and 
  finally call client close callback) */

  /* No need for abort write as flush tx callback is called when no more Tx */

  /* Check if client is in the middle of a read operation (note that this is 
  invoked from either DPC Tx done or from app context (close), the latter needs 
  to grab the crit section to protect it from enabled cb.
  Note: It is ok for a DPC to grab al_ser_base_access_guard, but forbidden for 
  app to grab giant mutex while holding al_ser_base_access_guard 
  (a deadlock may occur) */

  /* Enter Crit Section */
  rex_enter_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

  if (ser_if_data_ptr->read_oper_is_in_progress == TRUE)
  {
    ++ser_if_data_ptr->num_of_aborts_issued_prior_to_close;
    HSU_ASSERT(ser_if_data_ptr->num_of_aborts_issued_prior_to_close <= 
      AL_SER_BASE_MAX_IO_ABORTS_BEFORE_CLOSE);

    /* Leave Crit Section */
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

    al_ser_base_internal_abort_read(ser_if_data_ptr);
  }
  else
  {
    /* No more aborts - call close immediately */
    al_ser_base_internal_close(ser_if_data_ptr);

    /* Leave Crit Section */
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));
  }

  HSU_MSG_HIGH("*_internal_flush_tx_done_issued_by_close: Exit", 0, 0, 0);
}

/*===========================================================================

FUNCTION al_ser_base_has_already_been_closed

DESCRIPTION
Checks if this serial I/F data struct has already been closed by this layer. 
This means that the client's close cb had been called before, and therefore
had set the field stream_id to SIO_NO_STREAM_ID.

DEPENDENCIES
None

RETURN VALUE
TRUE - Close has been issued. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
static boolean al_ser_base_has_already_been_closed
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
                               /* Serial I/F data struct used in operation */
)
{
  HSU_MSG_LOW(
    "al_ser_base_has_already_been_closed: Checking if ser_if_data_ptr (0x%x) valid...",
    ser_if_data_ptr, 0, 0);

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr));

  return (boolean)(ser_if_data_ptr->stream_id == SIO_NO_STREAM_ID);
}


/*===========================================================================

FUNCTION al_ser_base_command_read

DESCRIPTION
Invoked by the AL task, this function attempts to initiate a read 
operation (but only if the instance which this read is related to is enabled).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_command_read
(
  void* ctx_for_al_task
)
{
  boolean read_operation_result;
  jresult_t cdc_read_res, cdc_abort_read_res;
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx_for_al_task;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* Only setup a new read if instance is enabled: */
  /* Assuming the AL task does NOT grab the crit section:
  There is no need to guard instance_enabled_by_cdc_fd:
  1. If ser_if_data_ptr->instance_enabled_by_cdc_fd == TRUE is NOT taken, and 
  then enable cb is invoked, then it will call al_ser_base_alloc_buffer_and_setup_read

  2. Otherwise, after the "if" a disabled cb is invoked, however, 
  al_ser_base_alloc_buffer_and_setup_read grabs the giant and checks by itself
  instance_enabled_by_cdc_fd and then it will discard the read operation 
  (with the allocated buffer).
  */
  if (ser_if_data_ptr->instance_enabled_by_cdc_fd == TRUE)
  {
    /* 
    To prevent the following scenario:
    1. Data mover got an assignment to scatter
    2. Disable callback invoked, followed by enable, causing a cdc_read
    3. Now data mover callback returns, issuing the AL task to also call cdc_read.
    */
    jsafe_enter();

    cdc_abort_read_res = 
      hsu_cdc_fd_api_cdc_abort_read(ser_if_data_ptr->instance_of_fd_used);
    if (cdc_abort_read_res != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR("al_ser_base_command_read: *_cdc_abort_read prior to\
                    read oper returned non zero value, FD error=%d", 
                    cdc_abort_read_res, 0, 0);
      HSU_ASSERT((uint32)"al_ser_base_command_read: failed" == NULL);
    }

    read_operation_result = al_ser_base_alloc_buffer_and_setup_read(ser_if_data_ptr, 
      &cdc_read_res);
    if (read_operation_result == FALSE)
    {
      HSU_MSG_ERROR("al_ser_base_command_read: \
        al_ser_base_alloc_buffer_and_setup_read returned FALSE, FD error=%d", 
        cdc_read_res, 0, 0);
      HSU_ASSERT((uint32)"al_ser_base_command_read: failed" == NULL);
    }

    jsafe_leave();
  }

  return ;
}

/*===========================================================================

FUNCTION al_ser_base_command_write

DESCRIPTION
Invoked by the AL task, this function attempts to initiate a write 
operation (but only if the instance which this read is related to is enabled).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_command_write
(
  void* ctx_for_al_task
)
{
  jresult_t cdc_write_res;
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx_for_al_task;

  void* buffer_ptr = NULL;
  

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  HSU_ASSERT(ser_if_data_ptr->size_of_curr_buff_for_write != 0);

/* TODO: For now it is assumed the buffer ptr is the entire Tx mem. pool !!! */
/* If this is incorrect, need to save the buffer ptr in a separate field !!! */
  buffer_ptr = ser_if_data_ptr->memory_pool_for_tx;

  /* The following is to prevent the scenario:
  1. The DMOV has been given a request to gather (could be for already queued data).
  2. DMOV gather done called (INT context) -> wakes AL task.
  3. (In case the AL task is not responsible for grabbing the mutex) AL task 
     invokes this function, wants to issue cdc_write.
  4. The CDC FD invokes disable.
  5. cdc_write is issued, but the I/F is already disabled!
  */
  jsafe_enter();
  /* Here is safe to check if enabled/disabled - as only the FD 
  can modify these values - and right now it does not have the giant mutex*/
  if (ser_if_data_ptr->instance_enabled_by_cdc_fd == TRUE)
  {
#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND
    /* TODO: Where is this initialized? */
    if (ser_if_data_ptr->overcome_1_2_byte_hw_crc_err == TRUE)
    {
      if (((ser_if_data_ptr->size_of_curr_buff_for_write % \
        HSU_MAX_FULL_SPEED_PACKET_SIZE) == 1) ||
        ((ser_if_data_ptr->size_of_curr_buff_for_write % \
        HSU_MAX_FULL_SPEED_PACKET_SIZE) == 2))
      {
        /* Add 4 byte padding */
        static const byte hw_crc_error_padding[4] = HSU_CRC_ERR_PADDING_ARRAY;
        
        /* Assuming max buffer size is enough to add 4 more bytes 
        when transfer has 1 or 2 byte remainder. */
        HSU_ASSERT((ser_if_data_ptr->size_of_curr_buff_for_write+4) <= 
          ser_if_data_ptr->max_buff_size_for_write);

        memcpy(((byte*)buffer_ptr+(ser_if_data_ptr->size_of_curr_buff_for_write)), 
          hw_crc_error_padding, 4);
        ser_if_data_ptr->size_of_curr_buff_for_write += 4;
      }
    }
#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */


    /* Send buffer to FD: */
    cdc_write_res = hsu_cdc_fd_api_cdc_write(ser_if_data_ptr->instance_of_fd_used, 
      buffer_ptr, ser_if_data_ptr->size_of_curr_buff_for_write);
    if (cdc_write_res != HSU_JSUCCESS)
    {
      /* Check if the write operation failed due to the fact the device was suspended. 
      If so, treat as if the hsu_al_ser_base_i_tx callback was invoked with status
      cancelled (since the callback is invoked from DSR context, just like this
      function no need to release mutex) */
      if (cdc_write_res == ESUSPENDED)
      {
        /* No need to free buffer, it will be done in tx_done: */
        hsu_al_ser_base_i_tx_done(CDC_TRANSFER_CANCELLED, buffer_ptr, ser_if_data_ptr);

        /* Release giant mutex */
        jsafe_leave();

        return;
      }

      /* Release giant mutex */
      jsafe_leave();

      HSU_MSG_ERROR("al_ser_base_command_write: hsu_cdc_fd_api_cdc_write failed;\
                    result=%d, FD instance=0x%x, buffer_ptr=0x%x", cdc_write_res, 
                    ser_if_data_ptr->instance_of_fd_used, buffer_ptr);
      HSU_MSG_ERROR(
        "al_ser_base_command_write: hsu_cdc_fd_api_cdc_write failed; buffer size=%u", 
        ser_if_data_ptr->size_of_curr_buff_for_write, 0, 0);
      HSU_ASSERT((uint32)"al_ser_base_command_write: unexpected failure!" == NULL);

      return;        
    }
  }
  else /* ser_if_data_ptr->instance_enabled_by_cdc_fd == FALSE */
  {
    /* Release giant mutext */
    jsafe_leave();

    /* instance is disabled - discard current transmission, as well as any other 
    pending data.
    Note that the app cannot interrupt, since this code is running in the AL task 
    context. */
    al_ser_base_discard_all_tx_data(ser_if_data_ptr, buffer_ptr);

    return;
  }
  /* Release giant mutext */
  jsafe_leave();

  return;
}

/*===========================================================================

FUNCTION al_ser_base_alloc_buffer_and_setup_read

DESCRIPTION
Allocates a buffer from the corresponding Rx mem. pool and sets up a new read 
operation, but only if the instance, which this read operation is related to,
is enabled.

DEPENDENCIES
None

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise (internal error or CDC FD error).
In case of CDC FD error, it will set the supplied cdc_fd_error accordingly.

SIDE EFFECTS
None

===========================================================================*/
static boolean al_ser_base_alloc_buffer_and_setup_read
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
                               /* Serial I/F data struct used in operation */
  jresult_t* cdc_fd_error      /* In case the FD returned an error, 
                               its value (see jerrno.h for more details)   */
)
{
  void* buffer_for_read_ptr = NULL;
  jresult_t cdc_read_res;

  HSU_ASSERT(cdc_fd_error != NULL);

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  if ((ser_if_data_ptr->app_rx_cb_func == NULL) &&
    (ser_if_data_ptr->rx_wmq_ptr == NULL))
  {
    HSU_MSG_HIGH("*__alloc_buffer_and_setup_read: Both the rx cb and rx wmq \
                 are NULL. Aborting.",0,0,0);
    return TRUE;
  }


  buffer_for_read_ptr = hsu_al_ser_i_alloc_buffer_for_read_write(
    ser_if_data_ptr->memory_pool_for_rx, ser_if_data_ptr->buff_size_for_read);
  if (buffer_for_read_ptr == NULL)
  {
    HSU_MSG_ERROR("al_ser_base_alloc_buffer_and_setup_read:\
                  Allocate read buffer failed!", 0, 0, 0);

    HSU_ASSERT(
      (uint32)"al_ser_base_alloc_buffer_and_setup_read: Allocate read buffer failed!"
        == NULL);
  }

  /* NOTE: instance_of_fd_used is already known at this stage 
  (obex_init/acm_init was already called) */

  /* Grab giant mutex prior to checking enable/disable: */
  jsafe_enter();
  /* Here is safe to check if enabled/disabled - as only the FD 
  can modify these values - and right now it does not have the giant mutex*/
  if (ser_if_data_ptr->instance_enabled_by_cdc_fd == TRUE)
  {
    cdc_read_res = hsu_cdc_fd_api_cdc_read(ser_if_data_ptr->instance_of_fd_used, 
      buffer_for_read_ptr, ser_if_data_ptr->buff_size_for_read);

    if (cdc_read_res != HSU_JSUCCESS)
    {
      /* Release giant mutex */
      jsafe_leave();

      if (cdc_read_res == ESUSPENDED)
      {
        /* Treat the case in which this interface was on USB, cable was unplugged.
        While cable was unplugged, the user moved this function to a non USB port, 
        and then moved back to USB. In this case a disabled callback has yet to 
        be received, but the cdc fd would return ESUSPENDED */

        /* Free allocated buffer, as it won't be used now */
        hsu_al_ser_i_free_buffer_from_read_write(ser_if_data_ptr->memory_pool_for_rx, 
          buffer_for_read_ptr);

        return TRUE;
      }
      /* else: cdc_read_res != ESUSPENDED but is an error */

      HSU_MSG_ERROR("al_ser_base_alloc_buffer_and_setup_read: \
                    hsu_cdc_fd_api_cdc_read failed (%d) for stream_id %d!", 
                    cdc_read_res, ser_if_data_ptr->stream_id, 0);

      hsu_al_ser_i_free_buffer_from_read_write(ser_if_data_ptr->memory_pool_for_rx, 
        buffer_for_read_ptr);

      /* Set error */
      *cdc_fd_error = cdc_read_res;

      return FALSE;        
    } 
    /* else: (cdc_read_res == HSU_JSUCCESS)*/

    ser_if_data_ptr->read_oper_is_in_progress = TRUE;

    /* Release giant mutex */
    jsafe_leave();
  }
  else /* (ser_if_data_ptr->instance_enabled_by_cdc_fd == FALSE) */
  {
    /* Release giant mutex */
    jsafe_leave();

    /* Release allocated buffer */
    hsu_al_ser_i_free_buffer_from_read_write(ser_if_data_ptr->memory_pool_for_rx, 
      buffer_for_read_ptr);
  }

  return TRUE;
}

/*===========================================================================

FUNCTION al_ser_base_discard_all_tx_data

DESCRIPTION
Discards the current buffer which was meant to be Tx'ed, as well as any
other data queued in this instance's Tx WMQ. Finally, marks for this instance
that it is neither in a DMOV gather operation nor during the FD write operation.

DEPENDENCIES
None

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise (internal error or CDC FD error).
In case of CDC FD error, it will set the supplied cdc_fd_error accordingly.

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_discard_all_tx_data
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  void* curr_buffer_to_tx_ptr
)
{
  HSU_MSG_LOW("*_discard_all_tx_data: Checking if ctx (0x%x) valid...", 
    ser_if_data_ptr, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);
  HSU_ASSERT(curr_buffer_to_tx_ptr != NULL);

  /* Free current Tx buffer which was supposed to be tx'ed: */
  hsu_al_ser_i_free_buffer_from_read_write(ser_if_data_ptr->memory_pool_for_tx, 
    curr_buffer_to_tx_ptr);

  /* Free all DSM items in this instance's WMQ, as there is no longer a point 
  in having them: */
    dsm_empty_queue(ser_if_data_ptr->tx_wmq_ptr);

  /* Mark for this data ptr that it is no longer busy (as if aborted) */
  /* Notet that this function is invoked either from INT context (DMOV gather)
  or from AL task/FD*/
  ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy = FALSE;
}

/*===========================================================================

FUNCTION al_ser_base_send_all_tx_data

DESCRIPTION
Sends all enqueued data, up to the size of the instance's 
max_buff_size_for_write.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
- In case flush_tx callback is set and no more data to be sent - the callback
is invoked.

===========================================================================*/
static void al_ser_base_send_all_tx_data
(
 hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
 )
{
  dsm_item_type *chain_to_gather_ptr = NULL, *curr_chain_to_add_ptr = NULL;
  /* dsm_append nullifies its second argument, therefore need to save it prior 
  to dsm_append */
  juint32_t chain_to_gather_total_length = 0;
  void* new_buffer_to_send_ptr = NULL;
  boolean ret_val = FALSE;
  uint32 curr_chain_to_add_length = 0;
  uint32 size_of_first_dsm_item = 0;  

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);


  if (ser_if_data_ptr->remaining_chain_to_tx_ptr != NULL)
  {
    /* If, in a previous transfer, a fragment of a DSM chain was left, then
    for this transfer it must be the first part of the chain_to_gather_ptr */
    chain_to_gather_ptr = ser_if_data_ptr->remaining_chain_to_tx_ptr;
    chain_to_gather_total_length = dsm_length_packet(chain_to_gather_ptr);

    if (chain_to_gather_total_length > ser_if_data_ptr->max_buff_size_for_write)
    {
      /* The remaining DSM chain still exceeds max buff size for Tx.
      Need to trim it. */
      ser_if_data_ptr->remaining_chain_to_tx_ptr = NULL;
      ret_val = hsu_al_dsm_utils_split_dsm_chain(chain_to_gather_ptr->pool_id, 
        chain_to_gather_ptr, &(ser_if_data_ptr->remaining_chain_to_tx_ptr), 
        ser_if_data_ptr->max_buff_size_for_write);
      HSU_ASSERT(ret_val);

      /* Update chain_to_gather_ptr's new length as result of the trimming */
      chain_to_gather_total_length = dsm_length_packet(chain_to_gather_ptr);
    }
    else
    {
      ser_if_data_ptr->remaining_chain_to_tx_ptr = NULL;
    } 
  }
  else /* app_data_ptr->remaining_chain_to_tx_ptr == NULL */
  {
    /* Enter Crit Section */
    /* This is to prevent the following scenario:
    - The app has a higher priority than HS-USB task
    - If this is in the middle of Tx Done/al_ser_base_tx_aggr_timer_cb, no more 
    data in WMQ and just before marking dmov_gather_or_fd_write_or_timer_busy as FALSE 
    a context switch occurs and the app calls hsu_al_ser_base_transmit
    - The app sees dmov_gather_or_fd_write_or_timer_busy is TRUE, so it enqueues data 
    (as well as not setting timer, if used)
    - Tx done / al_ser_base_tx_aggr_timer_cb does not know new data has been 
    enqueued, so it does nothing (should have sent the new data).
    - (In Tx done only:)Not only that but if a new call to hsu_al_ser_base_transmit occurs, 
    the new data will be transmitted BEFORE the one residing in the queue.
    - If no further calls are made, the enqueued chain is forever there.
    */
    rex_enter_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

    /* If Tx WMQ non empty: proceed to gather all of its contents (or until mem. 
    pool capacity is met) into one chain: */
    chain_to_gather_ptr = dsm_dequeue(ser_if_data_ptr->tx_wmq_ptr);
    if (chain_to_gather_ptr != NULL)
    {
      chain_to_gather_total_length = dsm_length_packet(chain_to_gather_ptr);

      if (chain_to_gather_total_length > ser_if_data_ptr->max_buff_size_for_write)
      {
        /* The first DSM chain from the WMQ exceeds max buff size for Tx.
        Need to trim it, and leave */
        ret_val = hsu_al_dsm_utils_split_dsm_chain(chain_to_gather_ptr->pool_id, 
          chain_to_gather_ptr, &(ser_if_data_ptr->remaining_chain_to_tx_ptr), 
          ser_if_data_ptr->max_buff_size_for_write);
        HSU_ASSERT(ret_val);

        /* Update chain_to_gather_ptr's new length as result of the trimming */
        chain_to_gather_total_length = dsm_length_packet(chain_to_gather_ptr);
      }
    }
    else /* (chain_to_gather_ptr == NULL) */
    {
      /* Mark for this data ptr that it is no longer busy as well 
      as invoking the app's flush callback (if supplied) */
      ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy = FALSE;

      if (ser_if_data_ptr->flush_tx_in_progress)
      {
      if (ser_if_data_ptr->flush_tx_complete_cb != NULL)
      {
        ser_if_data_ptr->flush_tx_complete_cb(ser_if_data_ptr);
        ser_if_data_ptr->flush_tx_complete_cb = NULL;
        }
        rex_clr_timer(&(ser_if_data_ptr->flush_tx_timer));
        ser_if_data_ptr->flush_tx_in_progress = FALSE;
      }

      /* Handle the case where the application closed the port when there's 
         pending TX data */
      if (ser_if_data_ptr->num_of_aborts_issued_prior_to_close > 0)
      {
        --ser_if_data_ptr->num_of_aborts_issued_prior_to_close;
        if (ser_if_data_ptr->num_of_aborts_issued_prior_to_close == 0)
        {
          al_ser_base_internal_close(ser_if_data_ptr);
        }
      }

      /* Leave Crit Section */
      rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

      /* No more data to send */
      return;
    }

    /* Leave Crit Section */
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));
  }

  /* At this point chain_to_gather_ptr is non NULL */
  HSU_ASSERT(chain_to_gather_ptr != NULL);

  /* Continue adding more DSM chains, until:
  1. There are no more chains in WMQ 
  2. The length of chain(s) gathered so far is almost the max size of Tx buffer
  (almost, as there is no more room even for the first DSM item in the next 
  DSM chain)*/
  while (!dsm_is_wm_empty(ser_if_data_ptr->tx_wmq_ptr))
    {
    curr_chain_to_add_ptr = hsu_al_dsm_utils_peek_wmq(ser_if_data_ptr->tx_wmq_ptr);
    size_of_first_dsm_item = curr_chain_to_add_ptr->size; 
      /* We assume that dsm_chains that are queued in tx_wmq could be 
         allocated from different dsm pools so we need to check the
    size of the first dsm item for each dequeued chain. 
    Use the DSM chain's first item's size field, instead of the macro:
    DSM_POOL_ITEM_SIZE(chain_to_gather_ptr->pool_id); 
    Since for the pool ID: DSM_DUP_ITEM_POOL that value is zero 
    */
    if ((chain_to_gather_total_length + size_of_first_dsm_item) > 
      ser_if_data_ptr->max_buff_size_for_write)
    {
      break;
    }

    curr_chain_to_add_ptr = dsm_dequeue(ser_if_data_ptr->tx_wmq_ptr);
      curr_chain_to_add_length = dsm_length_packet(curr_chain_to_add_ptr);
      if (curr_chain_to_add_length > 
        (ser_if_data_ptr->max_buff_size_for_write - chain_to_gather_total_length))
      {
        HSU_ASSERT(ser_if_data_ptr->remaining_chain_to_tx_ptr == NULL);
        ret_val = hsu_al_dsm_utils_split_dsm_chain(curr_chain_to_add_ptr->pool_id, 
        curr_chain_to_add_ptr, &(ser_if_data_ptr->remaining_chain_to_tx_ptr), 
          (ser_if_data_ptr->max_buff_size_for_write - chain_to_gather_total_length));
        HSU_ASSERT(ret_val);

      chain_to_gather_total_length += dsm_length_packet(curr_chain_to_add_ptr);
      dsm_append(&chain_to_gather_ptr, &curr_chain_to_add_ptr);
      break;
      }
    chain_to_gather_total_length += curr_chain_to_add_length;
      dsm_append(&chain_to_gather_ptr, &curr_chain_to_add_ptr);
  }

  /* Check if in-fact there is still something in the Tx WMQ */
  if (chain_to_gather_ptr == NULL)
  {
    /* Mark for this data ptr that it is no longer busy as well 
    as invoking the app's flush callback (if supplied) */
    ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy = FALSE;

    if (ser_if_data_ptr->flush_tx_in_progress)
    {
    if (ser_if_data_ptr->flush_tx_complete_cb != NULL)
    {
      ser_if_data_ptr->flush_tx_complete_cb(ser_if_data_ptr);
      ser_if_data_ptr->flush_tx_complete_cb = NULL;
      }
      rex_clr_timer(&(ser_if_data_ptr->flush_tx_timer));
      ser_if_data_ptr->flush_tx_in_progress = FALSE;
    }

    return;
  }
  /* else: proceed to allocating buffer and performing gather operation */

  HSU_ASSERT(chain_to_gather_total_length != 0);
  HSU_ASSERT(chain_to_gather_total_length <= ser_if_data_ptr->max_buff_size_for_write);

  /* Alloc new buffer: */
  new_buffer_to_send_ptr = hsu_al_ser_i_alloc_buffer_for_read_write(
    ser_if_data_ptr->memory_pool_for_tx, chain_to_gather_total_length);
  if (new_buffer_to_send_ptr == NULL)
  {
    HSU_MSG_ERROR("*_send_all_tx_data: Couldn't alloc buffer! mem_pool=0x%x size=%u", 
      ser_if_data_ptr->memory_pool_for_tx, chain_to_gather_total_length, 0);
    HSU_ASSERT((uint32)"al_ser_base_send_all_tx_data: alloc buffer failed!" == NULL);
  }

  /* Request DMOV to gather DSM chain into the contiguous buffer: */

  ser_if_data_ptr->size_of_curr_buff_for_write = chain_to_gather_total_length;

  ret_val = hsu_al_dmov_api_gather_dsm_chain(chain_to_gather_ptr, 
    new_buffer_to_send_ptr, chain_to_gather_total_length, 
    al_ser_base_dmov_gather_callback, ser_if_data_ptr);
  if (ret_val == FALSE)
  {
    ser_if_data_ptr->size_of_curr_buff_for_write = 0;

    HSU_MSG_ERROR("*_send_all_tx_data: *_gather_dsm_chain returned FALSE! tx_ptr=0x%x\
        buffer_to_send=0x%x chain_length=%u", 
        chain_to_gather_ptr, new_buffer_to_send_ptr, chain_to_gather_total_length);
    HSU_MSG_ERROR("*_send_all_tx_data: *_gather_dsm_chain returned FALSE! callback=0x%x\
        user_data=0x%x", al_ser_base_dmov_gather_callback, ser_if_data_ptr, 0);

    dsm_free_packet(&chain_to_gather_ptr);

    HSU_ASSERT((uint32)"al_ser_base_send_all_tx_data: DMOV gather failed!" == NULL);
  }
}

/* Aggregation via timer related functions: */
/*===========================================================================

FUNCTION al_ser_base_tx_aggr_is_used

DESCRIPTION
Determines whether or not Tx packet agregation via timer is used.

DEPENDENCIES
None

RETURN VALUE
TRUE - Above aggregation is being used. FALSE - Otherwise.

SIDE EFFECTS
None

===========================================================================*/
static boolean al_ser_base_tx_aggr_is_used
(
  const hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
)
{
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);
  return (boolean)(ser_if_data_ptr->tx_aggr_delay_in_msecs != 0);
}


/*===========================================================================

FUNCTION al_ser_base_flush_tx_timer_cb

DESCRIPTION
Tx_flush timer callback, invoked upon timer expiration.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Issues cdc_abort_write.

===========================================================================*/
static void al_ser_base_flush_tx_timer_cb
(
 unsigned long ser_if_data_as_cb_arg
 )
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ser_if_data_as_cb_arg;

  HSU_MSG_LOW("*_flush_tx_timer_cb: Checking if ctx (0x%x) valid...", 
    ser_if_data_as_cb_arg, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

    rex_enter_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));
    ser_if_data_ptr->cdc_abort_write_issued = TRUE;
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

    (void)hsu_cdc_fd_api_cdc_abort_write(ser_if_data_ptr->instance_of_fd_used);
}

/*===========================================================================

FUNCTION al_ser_base_tx_aggr_timer_cb

DESCRIPTION
Aggregation timer callback, invoked upon timer expiration.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_tx_aggr_timer_cb
(
 unsigned long ser_if_data_as_cb_arg
 )
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ser_if_data_as_cb_arg;

  HSU_MSG_LOW("*_tx_aggr_timer_cb: Checking if ctx (0x%x) valid...", 
    ser_if_data_as_cb_arg, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  rex_enter_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));
  if (ser_if_data_ptr->tx_aggr_flush_command_already_enqueued)
  {
    ser_if_data_ptr->tx_aggr_flush_next_aggr_packets = FALSE;  
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));
    HSU_MSG_HIGH(
      "*_tx_aggr_timer_cb: Timer expired while flush already enqueued. Do nothing.", 
      0, 0, 0);     
    return;
  }
  /* ser_if_data_ptr->tx_aggr_flush_command_already_enqueued == FALSE */

  rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));

  /* The AL task will take it from here (in order to avoid timer context) 
  This will also signal it that there's a new command*/
  hsu_al_task_enqueue_cmd(al_ser_base_command_send_all_data, 
    ser_if_data_ptr);
}

/*===========================================================================

FUNCTION al_ser_base_start_aggregation_via_timer

DESCRIPTION
Enqueues given packet and starts the aggregation timer.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_start_aggregation_via_timer
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr,
  dsm_item_type* tx_ptr
)
{
  rex_timer_cnt_type set_timer_ret_val = 0;
  uint32 tx_wmq_curr_cnt = 0;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  if (ser_if_data_ptr->tx_aggr_delay_in_msecs == 0)
  {
    HSU_MSG_ERROR(
      "*_start_aggregation_via_timer: Error! Called with timer == 0...", 
      0, 0, 0);
    return;    
  }

  /* No need to guard this, since if dmov_gather_or_fd_write_or_timer_busy was FALSE,
  It means no timer is running and that no Tx done callback will be invoked */
  /* Enqueue chain in WMQ: */
  dsm_enqueue(ser_if_data_ptr->tx_wmq_ptr, &tx_ptr);

  /* If enqueuing at once ser_if_data_ptr->max_buff_size_for_write or more,
  don't set a timer, but enqueue a command to AL task. Otherwise, set timer. */
  tx_wmq_curr_cnt = dsm_queue_cnt(ser_if_data_ptr->tx_wmq_ptr);
  if (tx_wmq_curr_cnt >= ser_if_data_ptr->max_buff_size_for_write)
  {
     HSU_MSG_LOW(
      "*_start_aggregation_via_timer: No timer since Tx WMQ has %u > buff size %u", 
      tx_wmq_curr_cnt, ser_if_data_ptr->max_buff_size_for_write, 0);   
    al_ser_base_cancel_aggregation_and_flush(ser_if_data_ptr);

    /* The AL task will take it from here (in order to avoid timer context) 
             This will also signal it that there's a new command */
     hsu_al_task_enqueue_cmd(al_ser_base_command_send_all_data, 
       ser_if_data_ptr);  
  }
  else
  {
    set_timer_ret_val = rex_set_timer(&(ser_if_data_ptr->tx_aggr_timer),
      ser_if_data_ptr->tx_aggr_delay_in_msecs);
    /* This function is only called when no timer is active. */
    HSU_ASSERT(set_timer_ret_val == 0);
    
    HSU_MSG_LOW(
      "*_start_aggregation_via_timer: Aggregation of %u msecs started...", 
      ser_if_data_ptr->tx_aggr_delay_in_msecs, 0, 0);
  }

  return;
}

/*===========================================================================

FUNCTION al_ser_base_cancel_aggregation_and_flush

DESCRIPTION
Cancels aggregation timer and schedules a flush operation for whatever data
has been accumulated so far.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_cancel_aggregation_and_flush
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
)
{
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  if (ser_if_data_ptr->tx_aggr_delay_in_msecs != 0)
  {
    HSU_MSG_LOW(
      "*_cancel_aggregation_and_flush: Aggr. about to be stopped. Aggregated: %u bytes", 
      dsm_queue_cnt(ser_if_data_ptr->tx_wmq_ptr), 0, 0);
    /* Cancel timer and transmit all remaining data*/
    rex_enter_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));  
    ser_if_data_ptr->tx_aggr_flush_next_aggr_packets = TRUE;

    if (rex_get_timer(&(ser_if_data_ptr->tx_aggr_timer)) == 0)
    {
      /* No timer running or timer is about to expire. */
      /* If no timer running, then either dmov or FD write are already busy. 
          When Tx done will be invoked, tx_aggr_flush_next_aggr_packets being true will cause 
          another transmission. */
      /* If timer is about to expire, do nothing as soon enough the timer cb will be invoked. */
      rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));  
      return;
    }
    /* else: a timer is running, but not yet expired */

    ser_if_data_ptr->tx_aggr_flush_command_already_enqueued = TRUE;

    (void)rex_clr_timer(&(ser_if_data_ptr->tx_aggr_timer));
    /* If rex_clr_timer returns 0 then ser_if_data_ptr->tx_aggr_flush_next_aggr_packets == TRUE,
       since in this case the timer expired while this context holds the crit section. */
    /* This happens in the following scenario:
        dmov or FD are NOT busy.
        This function grabbed the critical section, so even if we were unable to explicitly cancel the timer 
        and the timer cb was invoked, it is now stuck on the crit. section, so it cannot set 
        ser_if_data_ptr->tx_aggr_flush_next_aggr_packets to FALSE. 
        However, we already enqueued a send_all command and need to prevent the timer cb to
        also enqueue such a command, that is why tx_aggr_flush_command_already_enqueued is used.
        The above can occur in case in the middle of this function a higher priority task stepped in and caused 
        the timer to expire. 
        */   

    HSU_MSG_LOW(
      "*_cancel_aggregation_and_flush: Aggr. timer cancelled...", 
      0, 0, 0);    
    /* A pending timer was running before this flush call, send data immediately */
    /* The AL task will take it from here (in order to avoid timer context) 
            This will also signal it that there's a new command */

    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));                  

    hsu_al_task_enqueue_cmd(al_ser_base_command_send_all_data, 
      ser_if_data_ptr);   
  }
}

/*===========================================================================

FUNCTION al_ser_base_send_more_data_or_aggregate

DESCRIPTION
Determines whether there is more data to be sent and if so, whether to do
so immediately or start aggregating.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_send_more_data_or_aggregate
(
 hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
)
{
  rex_timer_cnt_type ret_val_from_set_timer;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* First, check if there is still a remainder needed to be Tx'ed or if Tx WMQ 
  is non empty. */

  /* Enter Crit Section */
  /* This is to prevent the following scenario:
  - The app has a higher priority than HS-USB task
  - If this is in the middle of Tx Done, no more data in WMQ and just before
  marking dmov_gather_or_fd_write_or_timer_busy as FALSE a context switch occurs and
  the app calls hsu_al_ser_base_transmit
  - The app sees dmov_gather_or_fd_write_or_timer_busy is TRUE, so it enqueues data, without
  setting the timer.
  - Tx done does not know new data has been enqueued, so it does nothing (should have
  sent the new data).
  */
  rex_enter_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));  

  ser_if_data_ptr->tx_aggr_flush_command_already_enqueued = FALSE;

  if (ser_if_data_ptr->remaining_chain_to_tx_ptr == NULL)
  {
    /* If Tx WMQ empty: */
    if (dsm_is_wm_empty(ser_if_data_ptr->tx_wmq_ptr))
    {
      ser_if_data_ptr->tx_aggr_flush_next_aggr_packets = FALSE; 
      ser_if_data_ptr->dmov_gather_or_fd_write_or_timer_busy = FALSE;
      
        if (ser_if_data_ptr->flush_tx_complete_cb != NULL)
        {
          ser_if_data_ptr->flush_tx_complete_cb(ser_if_data_ptr);
          ser_if_data_ptr->flush_tx_complete_cb = NULL;
        }
      if (ser_if_data_ptr->flush_tx_in_progress)
      {
        rex_clr_timer(&(ser_if_data_ptr->flush_tx_timer));
        ser_if_data_ptr->flush_tx_in_progress = FALSE;
      }  

      /* Leave Crit Section */
      rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));   
      return;
    }
  }
  /* ser_if_data_ptr->remaining_chain_to_tx_ptr != NULL || 
     !dsm_is_wm_empty(ser_if_data_ptr->tx_wmq_ptr) */

  if (ser_if_data_ptr->tx_aggr_flush_next_aggr_packets)
  {
    if (dsm_length_packet(ser_if_data_ptr->remaining_chain_to_tx_ptr) + \
      dsm_queue_cnt(ser_if_data_ptr->tx_wmq_ptr) < ser_if_data_ptr->max_buff_size_for_write)
    {
      /* The data accumulated so far is less than the buffer for FD, so upon the next completion
          if it doesn't grow - mark that a timer should be used. */
      ser_if_data_ptr->tx_aggr_flush_next_aggr_packets = FALSE;        
    }
    /* else: there is more than ser_if_data_ptr->max_buff_size_for_write to send, 
       so upon next completion - flush rather than set a timer. */

    /* Leave Crit Section */
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard));            

    al_ser_base_send_all_tx_data(ser_if_data_ptr);       
  }
  else
  {
    ret_val_from_set_timer = rex_set_timer(&(ser_if_data_ptr->tx_aggr_timer),
      ser_if_data_ptr->tx_aggr_delay_in_msecs);
    HSU_ASSERT(ret_val_from_set_timer == 0);            

    HSU_MSG_LOW("*_send_more_data_or_aggregate: set timer to %u msecs.",
      ser_if_data_ptr->tx_aggr_delay_in_msecs, 0, 0);

    /* Leaving the crit section after setting timer, to avoid the following:
    1. A context switch occurs right before rex_set_timer.
    2. App calls transmit + flush functions, which will see:
        - dmov_gather_or_fd_write_or_timer_busy is still TRUE    
        - timer is not yet set
       which means it will do nothing (just enqueue the data). 
    3. context returns here, timer is set (although it should not have done so).
    */
    /* Leave Crit Section. */
    rex_leave_crit_sect(&(ser_if_data_ptr->al_ser_base_access_guard)); 
  }
}

/*===========================================================================

FUNCTION al_ser_base_command_send_all_data

DESCRIPTION
Invoked by the AL task, this function attempts to send all data enqueued so far.
This function is typically used when aggregation is supported.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_base_command_send_all_data
(
  void* ctx_for_al_task
)

{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx_for_al_task;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  al_ser_base_send_all_tx_data(ser_if_data_ptr);
}


#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* defined(FEATURE_HSU_TEST) && !defined(T_REXNT) */


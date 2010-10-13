/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             S I O R S 2 3 2  M D D
                             
            SIO Media Dependent Driver layer for RS-232 communication

GENERAL DESCRIPTION

  This module contain the code shared by different serial driver.  It calls
  seperate hardware dependent driver code to access the actual UART hardware.
  

EXTERNALIZED FUNCTIONS
    
  siors_mdd_api_chng_bitrate()
    Change bit rate of serial link.

  siors_mdd_api_close_stream()
    Deallocate SIO resources and do necessary book-keeping.
    
  siors_mdd_api_dev_init()
    Initialize device.
                        
  siors_mdd_api_dev_transmit()
    Transmit given data over MSM's RS-232 UART.
    
  siors_mdd_api_disable_device()
    Disables given device.

  siors_mdd_api_flush_tx()
    Flushes the TX.

  siors_mdd_api_ioctl()
    Control open stream.

  siors_mdd_api_open_stream()
    Allocate SIO resources and do necessary book-keeping.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  sio_rs232_dev_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 1999-2000, 2007-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_mdd.c#11 $ $DateTime: 2009/06/15 11:44:09 $ $Author: cmihalik $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/12/08   cpm     Add data support back in
04/03/08   rh      Use a common HW type definition
03/24/08   rh      Enable the use of LARGE_DSM_ITEM pool
03/18/08   rh      Add missing featurization for FEATURE_FIRST_UART
02/19/08   rh      Use a busy while loop to wait for TX empty
01/22/08   rh      Featurize the call to sleep_set_mclk_restriction
01/10/08   rh      Disable TX flush if TX transmit is called
01/09/08   rh      Disable cable check timer if all ports are in sleep state
12/17/07   rh      Ensure port is fully closed when sio_close is called
12/04/07   rh      Add ARM HALT workaround support
11/21/07   rh      Add IOCTL for RX line state detection
10/18/07   rh      Add debug trace for the received characters
09/24/07   rh      Add new IOCTL function to control TX BREAK event
08/10/07   rh      Added per UART configuration 
08/09/07   rh      Integrate changes required by the UARTDM-PD driver
07/27/07   rh      Remove some unused code
                   Added ability to handle receive function that returns
                   no valid data
04/13/07   rh      Created file from siors232.c
===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "siors232_mdd_defs.h"
#include "siors232_mdd_data.h"
#include "uart_compat.h"

//  If there is is at least one UART.
#if( defined(FEATURE_FIRST_UART)  || \
     defined(FEATURE_SECOND_UART) || \
     defined(FEATURE_THIRD_UART))              //{

#include "bio.h" 
#if defined(FEATURE_SIO_DTR_HIGH_GPIO) || defined (MSMHW_GROUP_INT_FOR_ALL_GPIO)
/*
 * If DTR is on one of the high GPIO_INT lines, then we need to use
 * the gpio_int API to connect to it.
 */
#include "gpio_int.h"           /* Group GPIO interrupts */
#endif

/*---------------------------------------------------------------------------
 The following array contains the port IDs of open streams.  If a stream is
 not open, the value in location is undefined.
---------------------------------------------------------------------------*/
static sio_port_id_type siors_mdd_stream_port[SIO_MAX_STREAM];

/*---------------------------------------------------------------------------
 The following array contains the port structure.  All the required 
 information/variable is stored in this structure. 
---------------------------------------------------------------------------*/

sio_mdd_port_type siors_mdd_ports[UART_NUM_DEVICES];

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*---------------------------------------------------------------------------
                      Function prototypes 
---------------------------------------------------------------------------*/
void siors_mdd_dsm_new_buffer_null_cleanup( sio_mdd_port_type * );
sio_status_type siors_mdd_chng_bitrate ( sio_mdd_port_type *, const sio_open_type *);
static void siors_mdd_callback_entry ( void *, siors_mdd_callback_type);
static void siors_mdd_update_flow_control( sio_mdd_port_type * );
static void siors_mdd_set_packet_mode ( sio_mdd_port_type   *mdd_port );

static void        siors_mdd_set_dcd_output( sio_mdd_port_type* mdd_port, boolean asserted );
static void        siors_mdd_enable_dtr_event( sio_mdd_port_type* mdd_port, sio_ioctl_param_type* param );
static void        siors_mdd_disable_dtr_event( sio_mdd_port_type* mdd_port, sio_ioctl_param_type* param );
static void        siors_mdd_check_dte_ready_asserted( sio_mdd_port_type *mdd_port, sio_ioctl_param_type *param );

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*lint -save -e714*/

/*===========================================================================

                       FUNCTIONs, ISRs and MACROs

===========================================================================*/

#ifdef FEATURE_UART_POWERDOWN //{
/* Number of times we'll sample Rx line as break before power down         */
#define SIO_CHECK_NO_CABLE_TIMES         3 
/* When doing UART Powerdown, check for BREAK state every 250ms.           */
#define SIO_CHECK_NO_CABLE_INTERVAL    250 
                                           
clk_cb_type         siors_mdd_check_no_cable_cb_item;
boolean             siors_mdd_check_no_cable_enabled;
#endif

/* Macro used to register cable detect timer */
#ifndef FEATURE_UART_POWERDOWN
  #define SIORS_MDD_REGISTER_CABLE_CHECK_TIMER()
#else
  #define SIORS_MDD_REGISTER_CABLE_CHECK_TIMER()  \
        do {                                                     \
           if(!siors_mdd_check_no_cable_enabled)                 \
           {                                                     \
              clk_reg( &siors_mdd_check_no_cable_cb_item,        \
                        siors_mdd_check_no_cable_cb_isr,         \
                        (int4)SIO_CHECK_NO_CABLE_INTERVAL,       \
                        (int4)SIO_CHECK_NO_CABLE_INTERVAL,       \
                        TRUE);                                   \
              siors_mdd_check_no_cable_enabled = TRUE;           \
           }                                                     \
        } while(0);
#endif //}

/*===========================================================================

FUNCTION SIORS_MDD_SCHEDULE_EMPTY_TX_FIFO_CHK

DESCRIPTION
  This procedure will set Tx Fifo watermark so that we get a TXLEV interrupt
  when the fifo is empty.  Also, do some record keeping so that when the
  TXLEV interrupt occurs, it will be clear that the condition that is being
  checked for is the empty condition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_schedule_empty_tx_fifo_chk
(
  sio_mdd_port_type   *mdd_port
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!mdd_port->is_clock_enabled)  return;

  UART_LOCK();

  mdd_port->gv.sio_chk_empty_tx_fifo_condition = TRUE;
  mdd_port->fp.enable_tx_interrupt(mdd_port->pPortHead, TRUE);

  UART_UNLOCK();
} 


/*===========================================================================

FUNCTION SIORS_MDD_CALL_STREAM_CLOSE_FUNC_ISR

DESCRIPTION
  This function simply calls the stream close function pointer.  clk_reg
  cannot be given function pointer directly because parameter requirements
  are different.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void siors_mdd_call_stream_close_func_isr
(
  int32 time_ms, 
  timer_cb_data_type data
)
{
  sio_mdd_port_type   *mdd_port;
  mdd_port = (sio_mdd_port_type *)data;  
  (mdd_port->gv.sio_stream_close_func_ptr)();
} 


/*===========================================================================

FUNCTION SIORS_MDD_SCHDL_STREAM_CHNG                              INTERNAL FUNCTION

DESCRIPTION
  This procedure is called by sio_close to delay calling a call-back routine
  until the last bit of the last byte of a pending transmission leaves the 
  TX shift register of the UART.  

  It is important to completely empty TX before switching because stream
  change may involve change in bit rates.  

DEPENDENCIES
  Called by sio_close.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

/*lint -save -e715 -e550*/ 

static void siors_mdd_schdl_stream_chng
(
  sio_mdd_port_type   *mdd_port,

  void             (*close_func_ptr)(void)  /* Function to call when 
                                               transmission is complete.   */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  UART_LOCK();
  ASSERT( close_func_ptr != NULL);
  
  mdd_port->gv.sio_stream_close_func_ptr = close_func_ptr;
  
  if (mdd_port->gv.sio_closing_stream == TRUE)
  {
      mdd_port->fp.reset_rx ( mdd_port->pPortHead );
  } 

  if (!mdd_port->is_clock_enabled)
  {
    /*----------------------------------------------------------------------- 
     UART clock is off, cannot transmit anything, and no need to check FIFO 
      empty either.  just call close func
     ----------------------------------------------------------------------*/ 
    close_func_ptr();
    /*-------------------------------------------------------------------------
     Remove ISR when the port is closed
    -------------------------------------------------------------------------*/
    mdd_port->fp.close ( mdd_port->pPortHead );
    mdd_port->is_clock_enabled = FALSE;
    sleep_assert_okts( mdd_port->gv.sleep_handle );
  }
  else
    /*-------------------------------------------------------------------------
     Check whether or not any data needs to be transmitted.  If not, call
     the call-back function.
    -------------------------------------------------------------------------*/
  if ( mdd_port->gv.sio_tx_current_wm_item_ptr == NULL)
  {
    if ( ( mdd_port->gv.sio_tx_current_wm_item_ptr = 
                   dsm_dequeue(mdd_port->gv.sio_tx_watermark_queue_ptr)) != NULL)
    {
      /*---------------------------------------------------------------------
       Set boolean indicating that we're in the middle of switching streams
       so that the TX code can correctly check for the completely empty
       condition and set the signal.
      ---------------------------------------------------------------------*/

      mdd_port->gv.sio_switch_streams = TRUE;
    } /* if sio_tx_current_wm_item_ptr = dsm_dequeue( sio_tx_watermark_ .. */
    else if (( mdd_port->fp.is_tx_fifo_empty ( mdd_port->pPortHead )) != 0)
    {
      /*---------------------------------------------------------------------
       If queue, FIFO and TX shift register are empty, all that is needed is
       to set the signal indicating that all pending data has been
       transmitted.  MSM documentation indicates that TXEMT condition is
       asserted when TX shift register becomes empty and there are no more
       bytes in TX FIFO.
      ---------------------------------------------------------------------*/

      if (mdd_port->gv.sio_closing_stream == TRUE)
      {
         mdd_port->gv.sio_tx_watermark_queue_ptr = NULL;
      }

      /*---------------------------------------------------------------------
       In this case, we are able to call sio_stream_close_func_ptr 
       immediately, but we should not.  For instance 
       sio_stream_close_func_ptr can point to a function containing a 
       rex_set_sigs command.  This is so when DS gets AT$QCDMG command.  
       rex_set_sigs can immediately wake up task being signalled, even if 
       this procedure was called from within an INTLOCK_SAV/INTFREE_SAV block
       (which it is).  The woken up task may call sio_open immediately and 
       not be able to allocate stream because some other caller (task or ISR)
       may be still trying to close the stream.
       
       Therefore, register a clock call-back to call the function pointed to
       by the function pointer.  By the time the function is called, stream
       will have been closed.
      ---------------------------------------------------------------------
        Lets try again.  We know that the callback may result in sigs 
        being set and that may induce some thread switch.  If we are closing
        the stream, that is true.  But if we are only flushing, then we can
        feel assured the user task won't set someone else's signal...that's
        the agreement...
      ----------------------------------------------------------------------*/

      if (mdd_port->gv.sio_closing_stream == TRUE)
      {
         timer_reg(&(mdd_port->gv.sio_stream_close_cb_item),
                  (timer_t2_cb_type)siors_mdd_call_stream_close_func_isr,
                  (timer_cb_data_type)mdd_port,
                  (int4)CLK_MS_PER_TICK,
                  (int4)0);
      }
      else
      {
        /* Just flushing the queue.  call callback immediately. 
			* If there is any pending TX FLUSH, make sure they are not active */
        close_func_ptr();
        mdd_port->gv.sio_switch_streams = FALSE;
        mdd_port->gv.sio_chk_empty_tx_fifo_condition = FALSE;
        mdd_port->fp.disable_tx_interrupt ( mdd_port->pPortHead );
      }
      /*-------------------------------------------------------------------------
       Remove ISR when the port is closed
      -------------------------------------------------------------------------*/
      if ( mdd_port->gv.sio_closing_stream == TRUE)
      {
        mdd_port->fp.close ( mdd_port->pPortHead );
        mdd_port->is_clock_enabled = FALSE;
        sleep_assert_okts( mdd_port->gv.sleep_handle );
      }
    } /* else if HWIO_IN( UART_SR) & MSMU_SR_TXEMT != 0 */
    else
    {
      /*---------------------------------------------------------------------
       Allow the TX FIFO and TX shift register to drain.
      ---------------------------------------------------------------------*/

      if (mdd_port->gv.sio_closing_stream == TRUE)
      {
         mdd_port->gv.sio_tx_watermark_queue_ptr = NULL;
      }
      mdd_port->gv.sio_switch_streams = TRUE;

      /*---------------------------------------------------------------------
       Queue is empty.  So, now check the condition where the TX FIFO is
       empty.  Cannot interrupt on the completely empty condition because
       there is no such interrupt.  The completely empty condition is
       indicated by a bit in a status register only.
      ---------------------------------------------------------------------*/

      siors_mdd_schedule_empty_tx_fifo_chk( mdd_port );
    } /* else if no item in queue and TX FIFO not empty */
  } /* if sio_tx_current_wm_item_ptr == NULL */
  else
  {
    mdd_port->gv.sio_switch_streams = TRUE;
  } /* else if sio_tx_current_wm_item_ptr != NULL */
  UART_UNLOCK();

} /* siors_schdl_stream_chng */
/*lint restore*/


/*===========================================================================

FUNCTION SIORS_MDD_TIME_STREAM_CHANGE_ISR

DESCRIPTION
  This function is executed after allowing enough time for any remaining
  data to be drained out of the UART's TX Fifo and TX shift register.

  This routine is used (in conjunction with others) to delay the setting
  of signal indicating that it is safe for some other task to open an SIO
  stream.  This signal needs to be set when TX is completely empty because
  the stream change may involve change in bit-rate.

DEPENDENCIES
  Should be called after TXLEV interrupt happens with a Tx Watermark value
  of 0 and after enough time has been allowed for the shift register to
  empty.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void siors_mdd_time_stream_change_isr(int32 time_ms, timer_cb_data_type data)
{
  sio_mdd_port_type   *mdd_port;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mdd_port = (sio_mdd_port_type *)data;

  if( (mdd_port->gv.sio_chk_empty_tx_fifo_condition ||
       mdd_port->gv.sio_switch_streams ||
       mdd_port->gv.sio_closing_stream) == FALSE )
  {
	  /* No longer checking for stream change condition, exit right away */
	  return;
  }

  /* Check to see if FIFO is truely empty.  If not, wait some more time */
  if (mdd_port->fp.is_tx_fifo_empty( mdd_port->pPortHead ) == 0) 
  {
    timer_reg( &(mdd_port->gv.sio_stream_close_cb_item),
             (timer_t2_cb_type)siors_mdd_time_stream_change_isr,
             (timer_cb_data_type)mdd_port,
             (int4)SIO_DEFAULT_SINGLE_CHAR_TIME,
             (int4)0);
    return;
  }
  /*-------------------------------------------------------
   If flow control method was hardware flow control, flow
   control was disabled.  Re-enable it.
  -------------------------------------------------------*/    
  if ( mdd_port->fv.sio_tx_flow_control_method == SIO_CTSRFR_FCTL)
  {
    mdd_port->fp.enable_tx_hw_fctl ( mdd_port->pPortHead );
  }

  mdd_port->gv.sio_chk_empty_tx_fifo_condition = FALSE;
  mdd_port->gv.sio_switch_streams              = FALSE;

  mdd_port->fp.disable_tx_interrupt ( mdd_port->pPortHead );
  if  (mdd_port->gv.sio_closing_stream == TRUE)
  {
    mdd_port->gv.sio_tx_watermark_queue_ptr = NULL;
  }

  /*-------------------------------------------------------------------------
   This means that we're switching streams, we're checking for the empty 
   condition by setting TX watermark at 0 and we've gotten a TXLEV interrupt.  
   Also, enough time has passed to allow byte in UART's TX FIFO to have been
   transmitted. All these conditions mean that what needed to be TX'ed has 
   been TX'ed (mostly likely an acknowledgement).  So, we're finally ready 
   to switch streams.  This statement needs to be the last statement
   in this ISR, since caller may use the call-back to re-open SIO stream 
   with different parameters, which is perfectly acceptable use of SIO 
   and should not cause any problems.
  --------------------------------------------------------------------------*/
  ( mdd_port->gv.sio_stream_close_func_ptr)();
  /* Remove ISR when the port is closed */
  if(mdd_port->gv.sio_closing_stream == TRUE)
  {
    /* Clean up the hardware and make sure the clock is off */
    if(mdd_port->is_clock_enabled)
    {
      mdd_port->fp.close ( mdd_port->pPortHead );
      mdd_port->is_clock_enabled = FALSE;
      sleep_assert_okts( mdd_port->gv.sleep_handle );
    }
  }
} 


#ifdef FEATURE_UART_POWERDOWN
/*===========================================================================

FUNCTION SIORS_MDD_CHECK_NO_CABLE

DESCRIPTION
  This function is called periodically to check for a BREAK state for the
  purpose of determining when a cable connection no longer exists.
  When no cable is connected we can power down the UART.

  The idea is to keep the UART running if there is a cable connected.  
  If the RX state is kept in the marked state of a long time, it is 
  we can shutdown the UART.

DEPENDENCIES
  - Should be called only by siors_mdd_check_no_cable_cb_isr

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

#if( defined( SIORS_UART1_PD_CHECK_CABLE_STATE )  ||\
     defined( SIORS_UART2_PD_CHECK_CABLE_STATE )  ||\
     defined( SIORS_UART3_PD_CHECK_CABLE_STATE )) //{

static void siors_mdd_check_no_cable( sio_mdd_port_type *mdd_port )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(mdd_port->is_clock_enabled)
  {
    /* Check to see if we have a cable connected */
    if(mdd_port->fp.check_rx_cable_status(mdd_port->pPortHead))
    {
      mdd_port->gv.check_no_cable_cnt = 0;
    }
    else
    {
      /* Detected a cable disconnect, wait to see if it is long enough */
      if ( ++mdd_port->gv.check_no_cable_cnt >= SIO_CHECK_NO_CABLE_TIMES )
      {
        /* Timeout, shutdown the UART */
        UART_LOCK();
        SIORS_TRACE_CODE(125, mdd_port->device_id);
        mdd_port->gv.check_no_cable_cnt = 0;
        mdd_port->fp.close ( mdd_port->pPortHead );
        mdd_port->is_clock_enabled = FALSE;
        sleep_assert_okts( mdd_port->gv.sleep_handle );
        /* If the port is capable to wake from RX event, enable it */
        if( mdd_port->gv.can_wake_on_rx_event )
        {
           mdd_port->fp.detect_rx_activity( mdd_port->pPortHead );
        }
        UART_UNLOCK();
      }
    }
  } /* Is the clock ON */
  else
  {
    if(mdd_port->fp.check_rx_cable_status(mdd_port->pPortHead))
    {
      /* Cable is connected, enable the UART */
      UART_LOCK();
      SIORS_TRACE_CODE(126, mdd_port->device_id);
      mdd_port->fp.open ( mdd_port->pPortHead, QUICK);
      siors_mdd_update_flow_control ( mdd_port );
      mdd_port->is_clock_enabled = TRUE;
      sleep_negate_okts(mdd_port->gv.sleep_handle);
      UART_UNLOCK();
    }
  }
} 

#endif //}

/*===========================================================================

FUNCTION SIORS_MDD_CHECK_NO_CABLE_CB_ISR

DESCRIPTION
  - This function calls siors_mdd_check_no_cable

DEPENDENCIES
  - Should be scheduled by calling clk_reg.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void siors_mdd_check_no_cable_cb_isr(int4 dummy)
{
  uint32    wake_count = 0;            /* Track number of ports that is wake */

  /* For each opened port, check the power condition */
#ifdef SIORS_UART1_PD_CHECK_CABLE_STATE
  {
     sio_mdd_port_type *mdd_port;
     mdd_port = &(siors_mdd_ports[UART_DEVICE_1]); 
     if(mdd_port->gv.sio_stream_open_rx)
     {
       siors_mdd_check_no_cable( mdd_port );
       wake_count += mdd_port->is_clock_enabled ? 1 : 0;
     }
  }
#endif 

#ifdef SIORS_UART2_PD_CHECK_CABLE_STATE
  {
     sio_mdd_port_type *mdd_port;
     mdd_port = &(siors_mdd_ports[UART_DEVICE_2]); 
     if(mdd_port->gv.sio_stream_open_rx)
     {
       siors_mdd_check_no_cable( mdd_port );
       wake_count += mdd_port->is_clock_enabled ? 1 : 0;
     }
  }
#endif 

#ifdef SIORS_UART3_PD_CHECK_CABLE_STATE
  {
     sio_mdd_port_type *mdd_port;
     mdd_port = &(siors_mdd_ports[UART_DEVICE_3]); 
     if(mdd_port->gv.sio_stream_open_rx)
     {
       siors_mdd_check_no_cable( mdd_port );
       wake_count += mdd_port->is_clock_enabled ? 1 : 0;
     }
  }
#endif 

  if( wake_count < 1 )
  {
     /* Nothing is awake, let's kill the cable check timer */
     clk_dereg( &siors_mdd_check_no_cable_cb_item );
     siors_mdd_check_no_cable_enabled = FALSE;
  }
}
#endif  /* FEATURE_UART_POWERDOWN */


#ifdef FEATURE_UART_POWERDOWN
/*===========================================================================

FUNCTION SIORS_MDD_RX_WAKEUP_HELPER_F

DESCRIPTION
  This is a helper function run from the RPC PROXY thread, which allows
   the function to make RPC calls.  The function is triggered from the 
   callback function in ISR space
      
DEPENDENCIES
  Should only be called from ISR space

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef IMAGE_APPS_PROC
void siors_mdd_rx_wakeup_helper_f
(
  oncrpc_proxy_cmd_client_call_type *msg   /* ONCRPC Proxy command message */
)
{
  sio_mdd_port_type       *mdd_port;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Retrive the port structure */
  mdd_port = (sio_mdd_port_type *)msg->data[0];
  ASSERT(mdd_port);
  /* Enable UART */
  SIORS_TRACE_CODE(127, mdd_port->device_id);
  UART_LOCK();
  if( !mdd_port->is_clock_enabled )
  {
     SIORS_TRACE_CODE(128, mdd_port->device_id);
     mdd_port->fp.open ( mdd_port->pPortHead, QUICK );
     siors_mdd_update_flow_control ( mdd_port );
     mdd_port->is_clock_enabled = TRUE;
     sleep_negate_okts(mdd_port->gv.sleep_handle);
  }
  /* If cable check timer is disabled, re-enable it */
  SIORS_MDD_REGISTER_CABLE_CHECK_TIMER();

  UART_UNLOCK();
}
#endif



/*===========================================================================

FUNCTION SIORS_MDD_RX_WAKEUP

DESCRIPTION
  This ISR is called when RXD line transitions while serial port is asleep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef IMAGE_APPS_PROC
static void siors_mdd_rx_wakeup
(
  sio_mdd_port_type   *mdd_port
)
{
  oncrpc_proxy_cmd_client_call_type *WakeFunc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Only re-start the UART if the port is actually powered down */
  if( mdd_port->is_clock_enabled )
  {
    return;
  }

  /* Get a message pointer */
  WakeFunc = remote_apis_client_call_get();

  /* Check that the message pointer is valid */
  if( WakeFunc != NULL )
  {
    /* Fill up the pointer data */
    WakeFunc->client_call =
      (void(*)( struct oncrpc_proxy_cmd_client_call_type *ptr ))
        siors_mdd_rx_wakeup_helper_f;

    /* Set message data to be the callback data */
    WakeFunc->data[0] = (int32) mdd_port;

    /* Queue up the message */
    oncprc_proxy_client_call_send( WakeFunc );
    /* Explicitly set to NULL */
    WakeFunc = NULL;
  }
} 

#else   /* IMAGE_APPS_PROC */
static void siors_mdd_rx_wakeup
(
  sio_mdd_port_type   *mdd_port
)
{
   /* On modem processor, there is no need to use the proxy thread
    * enable the UART directly                                         */
   /* Only re-start the UART if the port is actually powered down */

  SIORS_TRACE_CODE(142, mdd_port->device_id);
  if( !mdd_port->is_clock_enabled )
  {
    mdd_port->fp.open ( mdd_port->pPortHead, QUICK );
    siors_mdd_update_flow_control ( mdd_port );
    mdd_port->is_clock_enabled = TRUE;
    sleep_negate_okts(mdd_port->gv.sleep_handle);
  }
  /* If cable check timer is disabled, re-enable it */
  SIORS_MDD_REGISTER_CABLE_CHECK_TIMER();
}
#endif  /* IMAGE_APPS_PROC */

#else   /* FEATURE_UART_POWERDOWN */
static void siors_mdd_rx_wakeup
(
  sio_mdd_port_type   *mdd_port
)
{
    /* If powerdown is not used, do not do anything here */
}

#endif  /* FEATURE_UART_POWERDOWN */


/*===========================================================================

FUNCTION SIORS_MDD_GET_MORE_DATA_FOR_TX

DESCRIPTION
  This function will try to determine whether or not there is more data for
  transmission.  If so, interrupt is enabled.  If not, interrupt is disabled.
    
DEPENDENCIES
  Current transmit buffers should be empty when this function is called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_get_more_data_for_tx
(
  sio_mdd_port_type   *mdd_port
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( mdd_port->gv.sio_bytes_to_tx > 0)
  {
    /*-----------------------------------------------------------------------
     If more bytes are contained within the current packet, need to enable
     TX interrupt in order to be able to transmit when TX FIFO is ready to
     be stuffed again.  Since a micro-processor, even a '186, can write to
     registers much more quickly than UART can transmit, interrupt will
     happen when TX FIFO is fairly empty.
    -----------------------------------------------------------------------*/
    mdd_port->fp.enable_tx_interrupt( mdd_port->pPortHead, FALSE );
  } /* if sio_bytes_to_tx > 0 */
  else   
  { 
    /*----------------------------------------------------------------------
      if current item has no data, try to traverse packet chain first. 
     ----------------------------------------------------------------------*/
    mdd_port->gv.sio_tx_current_wm_item_ptr = 
            dsm_free_buffer( mdd_port->gv.sio_tx_current_wm_item_ptr);
    /* traverses the transmit queue if packet chain is empty */
    if (mdd_port->gv.sio_tx_current_wm_item_ptr == NULL) 
    { 
      mdd_port->gv.sio_tx_current_wm_item_ptr =
            dsm_dequeue( mdd_port->gv.sio_tx_watermark_queue_ptr);
    }

    if ( mdd_port->gv.sio_tx_current_wm_item_ptr != NULL ) 
    {
      /*---------------------------------------------------------------------
       If there is another packet in the transmit queue or packet chain, 
       do the necessary book-keeping and enable TX interrupt.
      ---------------------------------------------------------------------*/
      mdd_port->gv.sio_current_tx_ptr = 
               ( mdd_port->gv.sio_tx_current_wm_item_ptr)->data_ptr;

      mdd_port->gv.sio_bytes_to_tx = 
               ( mdd_port->gv.sio_tx_current_wm_item_ptr)->used;

      ASSERT( mdd_port->gv.sio_bytes_to_tx > 0);

      mdd_port->fp.enable_tx_interrupt( mdd_port->pPortHead, FALSE );
    }
    else
    {
      if ( mdd_port->gv.sio_switch_streams)
      {
        /*-------------------------------------------------------------------
         If there is no more data to transmit and we're in the middle of
         switching streams, need to check for empty FIFO condition so that
         the new stream is opened when all of the "old" data is completely
         transmitted.
        -------------------------------------------------------------------*/
        siors_mdd_schedule_empty_tx_fifo_chk(mdd_port);
      } /* if sio_switch_streams */
      else
      {
        /*-------------------------------------------------------------------
         If there is no more data to transmit, transmit interrupt needs to
         be disabled.
        -------------------------------------------------------------------*/
        mdd_port->fp.disable_tx_interrupt( mdd_port->pPortHead );
      } /* else if sio_switch_streams == FALSE */

      mdd_port->gv.sio_tx_current_wm_item_ptr = NULL;
    } /* else if no data in queue or packet chain*/

  } /* else if no data in current packet */

} /* siors_get_more_data_for_tx */


/*===========================================================================

FUNCTION SIORS_MDD_TX_ISR

DESCRIPTION
  This ISR is called when a TX interrupt is generated.  A TX interrupt is
  enabled when there is data that needs to be transmitted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_mdd_tx_isr
(
  sio_mdd_port_type   *mdd_port
)
{
  byte *current_tx_ptr;                  /* Temporary which contains
                                            current TX pointer             */
  int16 tx_byte_cnt = 0;                 /* number of bytes transmitted 
                                             in this rx_isr                */
  int32 byte_count;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!mdd_port->is_clock_enabled)  return;

  /*---------------------------------------------------------------------
   Check whether or not a software flow control character needs to be
   sent.  Only true when software flow control is the flow control
   method.  If we need to transmit a software flow control character and
   we're in the middle of a stream change, transmit the software flow
   control character and then check for the empty condition on the next
   call to siors_tx_isr.
  ---------------------------------------------------------------------*/

  if ( mdd_port->gv.sio_chk_empty_tx_fifo_condition)
  {
    /*-----------------------------------------------------------
       If we are flushing the tx, then we have an implicit
       agreement with the user that they will stop enqueuing
       data for tx.  If they renege, then we try to handle that
       case.
    -----------------------------------------------------------*/
    if ((mdd_port->gv.sio_closing_stream == FALSE) &&
        (mdd_port->gv.sio_changing_baudrate == FALSE))
    {
      if ( mdd_port->gv.sio_tx_current_wm_item_ptr != NULL)
      {
        /* the user has xmitted, despite flushing (bad guy!).  let's
         * compensate */
        ERR( "Flush agreement not met",0,0,0); 
        mdd_port->gv.sio_chk_empty_tx_fifo_condition = FALSE;
        mdd_port->gv.sio_switch_streams              = FALSE;

        /* TX interrupt is already enabled, call this to restore normal
         * TXLEV value */
        mdd_port->fp.enable_tx_interrupt(mdd_port->pPortHead, FALSE);
 
        /* int should re-occur, and then normal op continues*/
        return;  /* get out of the ISR now */
      }
    }
   
    /* ----------------------------------------------------------------
     * if we are here, then either we are 1) actually closing
     * the port, 2) we are flushing, or 3) we are changing
     * baud rate.  We do know that the user did not try to 
     * queue any more data
     * --------------------------------------------------------------- */
    ASSERT( mdd_port->gv.sio_tx_current_wm_item_ptr == NULL);

    mdd_port->fp.disable_tx_interrupt(mdd_port->pPortHead);

    /*-------------------------------------------------------------------
     If flow control method is hardware flow control and we have reached
     this point in the code, then we are trying to empty the last byte
     out of TX.
     
     However, some perverse timing could cause DTE to flow control the
     link right at this point.  The only way to check for the empty
     condition (other than to sit waiting for the empty condition within
     this ISR) is to register a clock call-back for the amount of time
     it takes for a byte to be transmitted.  If the link is flow 
     controlled, software will think that TX is completely empty, when
     in fact it is not.
    -------------------------------------------------------------------*/

    if ( mdd_port->fv.sio_tx_flow_control_method == SIO_CTSRFR_FCTL)
    {
      mdd_port->fp.disable_tx_hw_fctl( mdd_port->pPortHead );
    }

    /*-------------------------------------------------------------------
     Even though this particular TXLEV interrupt was generated using TX
     FIFO watermark of 0, if mode change is asked for immediately, the
     very last byte (which is in the process of being TX'ed will be
     corrupted).  10ms is enough time for a byte to get transmitted for
     bit rates higher than 2000bps.
     ------------------------------------------------------------------
     We will just call the callback if we are only flushing the queue.
     There may be an issue with TX_EMT, but we believe that enough time
     will expire so that byte does leave
     ------------------------------------------------------------------
     we will also use the callback mechanism if we have a slow baudrate,
     since the last char may take a few msecs to leave.  We are not worried
     about the callback setting a signal and causing an immediate context
     switch, since we are in an ISR, and the ISR must complete first.
     ------------------------------------------------------------------*/

    if (  (mdd_port->gv.sio_current_tx_bitrate <= SIO_BITRATE_115200) &&
			 (mdd_port->fp.is_tx_fifo_empty( mdd_port->pPortHead ) == 0) ) 
    {
       timer_reg( &(mdd_port->gv.sio_stream_close_cb_item),
                (timer_t2_cb_type)siors_mdd_time_stream_change_isr,
                (timer_cb_data_type)mdd_port,
                (int4)SIO_DEFAULT_SINGLE_CHAR_TIME,
                (int4)0);
    }
    else 
    {
      /* ---------------------------------------------------------------
       * either all bytes are gone, or we are flushing 
       * and we figure we have max .25 msec/char...(38400
       * baud or better).  Assuming that by the time the
       * relevant function executes, the shift reg should
       * be empty
       * However, should still use timer so close function is not
       * called from ISR context.  Close function can have RPC calls
       * --------------------------------------------------------------- */

      while(mdd_port->fp.is_tx_fifo_empty( mdd_port->pPortHead ) == 0);

      siors_mdd_time_stream_change_isr(0, (timer_cb_data_type)mdd_port);
    }
  } /* if sio_chk_empty_tx_fifo_condition */

  else /* !sio_chk_empty_tx_fifo_condition */
  {
    /*-------------------------------------------------------------------
     If the stream is closed but we're doing pending transmission that 
     needs to go out, we should continue processing.
     Otherwise, if Stream is not open, don't service interrupt.  
    -------------------------------------------------------------------*/

    if ( ( mdd_port->gv.sio_stream_open_tx == FALSE) &&
         ( mdd_port->gv.sio_switch_streams == FALSE))
    {
      mdd_port->fp.disable_tx_interrupt( mdd_port->pPortHead );
    } 
    else if ( mdd_port->fv.sio_outbound_flow_disabled)
    {
      /*-----------------------------------------------------------------
       Out-bound flow is disabled, but this interrupt happened.  So,
       disable TX interrupt so that the system does not get deadlocked.
      -----------------------------------------------------------------*/

      mdd_port->fp.disable_tx_interrupt( mdd_port->pPortHead );
    } /* if sio_outbound_flow_disabled */
    else
    {
      ASSERT( mdd_port->gv.sio_bytes_to_tx > 0);

      current_tx_ptr = mdd_port->gv.sio_current_tx_ptr;

      /* Ensure the transmit is done in a single unit.  If not, 
       * tx_pkt_done might happen and cause the value of bytes_to_tx and
       * current_tx_ptr to change */

      UART_LOCK();
      while(mdd_port->gv.sio_bytes_to_tx)
      {
        /* Loop until either the packet is empty or FIFO is full */
        byte_count = mdd_port->gv.sio_bytes_to_tx;
        mdd_port->fp.transmit(mdd_port->pPortHead, current_tx_ptr, &byte_count);
        mdd_port->gv.sio_bytes_to_tx -= byte_count;
    
        /* If the packet is empty, traverse and get the next packet */
        if(mdd_port->gv.sio_bytes_to_tx == 0)       
        {
          if ( mdd_port->gv.sio_tx_current_wm_item_ptr->pkt_ptr != NULL )
          { 
            /*---------------------------------------------------------------
              if more items are chained after this item, travers the pkt_chain
              and continue transmiting 
            ----------------------------------------------------------------*/
            siors_mdd_get_more_data_for_tx (mdd_port); 
            current_tx_ptr = mdd_port->gv.sio_current_tx_ptr;

            /*----------------------------------------------------------------
              Total bytes txed per sio_transmit() call is limited by a UPPER
              BOUND, so that sio_transmit() function would not last too long.
              Note: sio_transmit() LOCK interrupt.
             ---------------------------------------------------------------*/
            tx_byte_cnt += mdd_port->gv.sio_bytes_to_tx;
            if ( tx_byte_cnt > RS232_TX_UPPER_BOUND) break;
          }
          else  /* if packet is chained */
          { 
            /* If there is no more data in the packet, stop stuffing FIFO. */
            break;
          }
        }
        else /* if FIFO is full */
        {
          /* FIFO is full, record the number of bytes left and return */  
          mdd_port->gv.sio_current_tx_ptr = current_tx_ptr +  byte_count;
          break;
        }
      } 
      /*-----------------------------------------------------------------
       Try to get more data for TX'ing.  If there is more data that needs 
       to be TX'ed, the following routine will automatically enable TXLEV 
       interrupt.
      -----------------------------------------------------------------*/
      siors_mdd_get_more_data_for_tx(mdd_port);
      UART_UNLOCK();
    } /* else if TX is permitted */
  } /* else if sio_chk_empty_tx_fifo_condition == FALSE */
} /* siors_tx_isr */



/*===========================================================================

FUNCTION SIORS_MDD_TX_PKT_DONE

DESCRIPTION
  Mark the current packet that is being transmitted as finished, also
  traverse to the next packet in the chain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_mdd_tx_pkt_done
(
  sio_mdd_port_type   *mdd_port
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  mdd_port->gv.sio_bytes_to_tx = 0;
  siors_mdd_get_more_data_for_tx (mdd_port); 
}

/*===========================================================================

FUNCTION SIORS_MDD_RX_HRD_FCTL_TAIL_CHR_ISR

DESCRIPTION
  This ISR will handle the case where the incoming data is demarcated by
  a tail character.  If a tail character is seen, this ISR will enqueue
  the DSM item immediately.

DEPENDENCIES
  Should only be called by siors_rx_isr.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_rx_hrd_fctl_tail_chr_isr
(
  sio_mdd_port_type *mdd_port                   /* Port                       */
)
{
  dsm_item_type *item_ptr = NULL;            /* DSM item which contains
                                                packet                     */
  byte          *ptr;                        /* char pointer within buffer */
  uint32         pkt_len;                    /* Length of packet           */
  boolean        more_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do 
  {

    if ( item_ptr == NULL)  /* if we don't already have one */
    {
      /*---------------------------------------------------------------
       dsm_new_buffer no longer ERR_FATAL's when there are no more items.
      ---------------------------------------------------------------*/
      if (mdd_port->gv.sio_current_stream_mode == SIO_DM_MODE) 
      {
        item_ptr = dsm_new_buffer(  DSM_DIAG_ITEM_POOL);
      }
      else
      {
        item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL);
      }

      if ( item_ptr == NULL ) /* we couldn't get one. */
      {
        siors_mdd_dsm_new_buffer_null_cleanup( mdd_port );
        return;
      }
    }

    ptr  = item_ptr->data_ptr;
    pkt_len = item_ptr->size;

    mdd_port->fp.receive_tail_char ( mdd_port->pPortHead, ptr, 
                                  &pkt_len, mdd_port->gv.sio_tail_char,
                                  &more_data);
    SIORS_TRACE_RCVD_CHARS(ptr, pkt_len, mdd_port->device_id);
    /*----------------------------------------------------------
     If a tail character is found, enqueue immediately.  If more
     bytes need processing, we'll get another DSM item.
     Cannot exit immediately because the tail character may be
     the very first character which is read.
    ----------------------------------------------------------*/
    /*------------------------------------------------------------
     The following is a sanity check on pkt_len.
     Note:  the item could be a DSM_DS_SMALL_ITEM or a DSM_DIAG_ITEM. 
    ------------------------------------------------------------*/
    ASSERT( pkt_len <= item_ptr->size );
    
    item_ptr->used = pkt_len;

    /* If receive function returns zero byte, there is no more data to read */
    if (pkt_len == 0)
    {
      dsm_free_buffer( item_ptr );
      break;
    }

    if ( mdd_port->gv.sio_rx_func_ptr)
    {
      (mdd_port->gv.sio_rx_func_ptr)( &item_ptr);
    }
    else if(mdd_port->gv.sio_rx_watermark_queue_ptr)
    {
      dsm_simple_enqueue_isr( mdd_port->gv.sio_rx_watermark_queue_ptr, 
                             &item_ptr);
    } 
    else
    {
      dsm_free_buffer( item_ptr); /*lint !e534*/
    }

    item_ptr = NULL;  /* if we still need one, we'll have to get another */

  } while ( more_data );

} /* siors_rx_sft_fctl_tail_chr_isr */

/*===========================================================================

FUNCTION SIORS_MDD_RX_HRD_FCTL_GENERIC_ISR

DESCRIPTION
  This ISR is for generic RX processing without special processing, when
  flow control method is hardware flow control.

DEPENDENCIES
  Should only be called by siors_rx_isr.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_rx_hrd_fctl_generic_isr
(
  sio_mdd_port_type   *mdd_port
)
{
  dsm_item_type *item_ptr;         /* Pointer to DSM item                  */
  byte          *ptr;              /* char pointer within buffer           */
  uint32         pkt_len;          /* Packet length                        */
  uint32         buffer_len;       /* Size of the rx buffer                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*---------------------------------------------------------------
     dsm_new_buffer no longer ERR_FATAL's when there are no more items.
    ---------------------------------------------------------------*/
    if (mdd_port->gv.sio_current_stream_mode == SIO_DM_MODE) 
    {
      item_ptr = dsm_new_buffer(  DSM_DIAG_ITEM_POOL);
    }
    else
    {
      item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL);
    }

    if ( item_ptr == NULL ) /* we couldn't get one. */
    {
      siors_mdd_dsm_new_buffer_null_cleanup( mdd_port );
      return;
    }

    ptr  = item_ptr->data_ptr;
    buffer_len = item_ptr->size;
    pkt_len = buffer_len;

    /* -------------------------------------------------------------------
     *  Until we run out of chars or run out of buffer . . .
     * We keep a copy of the rdy status for consistency.  Otherwise
     * after we enqueue there could be new bytes and we create lots of
     * buffers with only one or two bytes. That hurts performance.
     * ------------------------------------------------------------------*/
    mdd_port->fp.receive ( mdd_port->pPortHead, ptr, &pkt_len );
    SIORS_TRACE_RCVD_CHARS(ptr, pkt_len, mdd_port->device_id);

    /*-----------------------------------------------------------------
     Whether we have a full buffer or we're out of fifo data, we enqueue it.
    ---------------------------------------------------------------
     The following is a sanity check on DSM_DS_SMALL_ITEM_SIZ.
    ---------------------------------------------------------------
      The following is a sanity check on pkt_len.
      Note:  the item could be  a DSM_DS_SMALL_ITEM or DSM_DIAG_ITEM. 
     ------------------------------------------------------------------*/ 
    ASSERT( ( pkt_len <= item_ptr->size ) );

    item_ptr->used = pkt_len;

    /* If receive function returns zero byte, there is no more data to read */
    if (pkt_len == 0)
    {
      dsm_free_buffer( item_ptr );
      break;
    }

    if ( mdd_port->gv.sio_rx_func_ptr)
    {
      ( mdd_port->gv.sio_rx_func_ptr)( &item_ptr);
    }
    else if( mdd_port->gv.sio_rx_watermark_queue_ptr )
    {
      dsm_simple_enqueue_isr( mdd_port->gv.sio_rx_watermark_queue_ptr,
                             &item_ptr);
    }
    else
    { 
      dsm_free_buffer( item_ptr); /*lint !e534*/
    }

  } while ( pkt_len == buffer_len );
} /* siors_rx_hrd_fctl_generic_isr */


#ifdef SIORS_HAS_PACKET_MODE
/*===========================================================================

FUNCTION SIORS_MDD_RX_HRD_FCTL_PACKET_ISR

DESCRIPTION
  This ISR is for packet RX processing without special processing, when
  flow control method is hardware flow control.

DEPENDENCIES
  Should only be called by siors_rx_isr.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_rx_hrd_fctl_packet_isr
(
  sio_mdd_port_type   *mdd_port
)
{
  dsm_item_type *item_ptr;         /* Pointer to DSM item                  */
  byte          *ptr;              /* char pointer within buffer           */
  uint32         pkt_len;          /* Packet length                        */
  uint32         buffer_len;       /* Size of the rx buffer                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*---------------------------------------------------------------
     dsm_new_buffer no longer ERR_FATAL's when there are no more items.
    ---------------------------------------------------------------*/
    item_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );

    if ( item_ptr == NULL ) /* we couldn't get one. */
    {
      siors_mdd_dsm_new_buffer_null_cleanup( mdd_port );
      return;
    }

    ptr  = item_ptr->data_ptr;
    buffer_len = item_ptr->size;
    pkt_len = buffer_len;

    /* -------------------------------------------------------------------
     *  Until we run out of chars or run out of buffer . . .
     * We keep a copy of the rdy status for consistency.  Otherwise
     * after we enqueue there could be new bytes and we create lots of
     * buffers with only one or two bytes. That hurts performance.
     * ------------------------------------------------------------------*/
    mdd_port->fp.receive ( mdd_port->pPortHead, ptr, &pkt_len );
    SIORS_TRACE_RCVD_CHARS(ptr, pkt_len, mdd_port->device_id);

    /*-----------------------------------------------------------------
     Whether we have a full buffer or we're out of fifo data, we enqueue it.
    ---------------------------------------------------------------
     The following is a sanity check on DSM_DS_SMALL_ITEM_SIZ.
    ---------------------------------------------------------------
      The following is a sanity check on pkt_len.
      Note:  the item could be  a DSM_DS_SMALL_ITEM or DSM_DIAG_ITEM. 
     ------------------------------------------------------------------*/ 
    ASSERT( ( pkt_len <= item_ptr->size ) );

    item_ptr->used = pkt_len;

    /* If receive function returns zero byte, there is no more data to read */
    if (0 == pkt_len)
    {
      dsm_free_buffer( item_ptr );
      break;
    }

    if ( mdd_port->gv.sio_rx_func_ptr)
    {
      ( mdd_port->gv.sio_rx_func_ptr)( &item_ptr);
    }
    else if( mdd_port->gv.sio_rx_watermark_queue_ptr )
    {
      dsm_simple_enqueue_isr( mdd_port->gv.sio_rx_watermark_queue_ptr,
                             &item_ptr);
    }
    else
    { 
      dsm_free_buffer( item_ptr); /*lint !e534*/
    }

  } while ( pkt_len == buffer_len );
}  /* siors_rx_hrd_fctl_packet_isr */

#endif   /* SIORS_HAS_PACKET_MODE */



/*===========================================================================

FUNCTION SIORS_MDD_RX_ISR

DESCRIPTION

  This is the ISR that is called when there is an UART RX interrupt.

  Note that this ISR calls many different ISR depending on different
  conditions.  The special casing (especially those which are based
  on stream_id are meant to be strictly transparent to the user.

  Partitioning into many different ISRs at "top level" pre-empts the
  need to do similar comparisons for each byt which is received, which
  is wasteful and causes SIO to miss its real time deadline, which in
  turn causes other ISRs to miss their deadlines.  Also, partitioning
  into different ISRs has the added benefit of making the design modular
  and easier to read and understand.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_rx_isr
( 
  sio_mdd_port_type  *mdd_port    /* Port                                     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------
   If Stream is not open, don't process any further
  ------------------------------------------------------------------------*/
  if ( mdd_port->gv.sio_stream_open_rx == FALSE)
  {
    mdd_port->fp.reset_rx( mdd_port->pPortHead );
    return;
  } 
  /*------------------------------------------------------------------------
    Now use the current mode for the specified port to determine how to 
    process the incoming byte(s).
  ------------------------------------------------------------------------*/
  switch ( mdd_port->gv.sio_current_stream_mode)
  {
    case SIO_DS_RAWDATA_MODE:
      /*--------------------------------------------------------------------
       This mode does not use tail character.  So, don't implement feature 
       and assert that tail character is not used.
      --------------------------------------------------------------------*/
      ASSERT( ( mdd_port->gv.sio_tail_char_used) == FALSE);

      /*--------------------------------------------------------------------
       For RawData stream, '+++' escape code has to be detected and data has
       to be transferred very efficiently.  Need to check for flow control
       method at "top level" so that the comparison is not made for each
       byte that is processed.
      --------------------------------------------------------------------*/

      if ( ( mdd_port->gv.sio_esc_seq_state) == SIO_ESC_SEQ_NULL)
      {
        siors_mdd_rx_rdata_nesc_isr( mdd_port ); 
      }
      else
      {
        siors_mdd_rx_rdata_esc_isr( mdd_port );
      }
      break;

#ifdef SIORS_HAS_AUTODETECT   //{
    case SIO_DS_AUTODETECT_MODE:
      {
        /*------------------------------------------------------------------
         Invoke the non-Autobaud ISR if DTE rate has been set for the command
         line or if Autobaud is not enabled.
        ------------------------------------------------------------------*/
        siors_mdd_rx_autodetect_isr( mdd_port );
      }
      break;
#endif //}

#ifdef SIORS_HAS_PACKET_MODE
    case SIO_DS_PKT_MODE:
      if ( mdd_port->gv.sio_uart_packet_mode )
      { 
        siors_mdd_rx_hrd_fctl_packet_isr( mdd_port );
      }
      else /* unoptimized packet */
      {
        siors_mdd_rx_hrd_fctl_generic_isr( mdd_port );
      }
      break;
#endif  /* PACKET_MODE */

    case SIO_GENERIC_MODE:
    case SIO_DM_MODE:
      /*--------------------------------------------------------------------
        Reduce the SIO mode down to generic mode with hardware flow
        control only
      --------------------------------------------------------------------*/
      
      if ( mdd_port->gv.sio_tail_char_used)
      {
        siors_mdd_rx_hrd_fctl_tail_chr_isr( mdd_port );
      }
      else
      {
        siors_mdd_rx_hrd_fctl_generic_isr( mdd_port );
      }
      break;

    default:
      ERR_FATAL( "Invalid SIO stream mode",0,0,0);
      break;
  } /* switch sio_current_stream_mode */

} /* siors_rx_isr */


/*===========================================================================

FUNCTION SIORS_MDD_INITIALIZE_STATE                         INTERNAL FUNCTION

DESCRIPTION
  Initialize state variables for the given device.

DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_mdd_initialize_state
( 
  sio_mdd_port_type   *mdd_port
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  mdd_port->gv.sio_rx_watermark_queue_ptr      = NULL;
  mdd_port->gv.sio_rx_func_ptr                 = NULL;
  mdd_port->gv.sio_tx_watermark_queue_ptr      = NULL;
  mdd_port->gv.rx_activity_cb_func_ptr         = NULL;

  mdd_port->gv.sio_current_rx_bitrate          = SIO_BITRATE_19200;
  mdd_port->gv.sio_current_tx_bitrate          = SIO_BITRATE_19200;
  mdd_port->gv.sio_current_stream_mode         = SIO_GENERIC_MODE;
  mdd_port->gv.sio_current_stream_id           = SIO_NO_STREAM_ID; 
  mdd_port->gv.sio_type_of_stream              = SIO_STREAM_RXTX;
  mdd_port->gv.sio_stream_open_rx              = FALSE;
  mdd_port->gv.sio_stream_open_tx              = FALSE;

  mdd_port->gv.sio_tx_current_wm_item_ptr      = NULL;
  mdd_port->gv.sio_current_tx_ptr              = NULL;
  mdd_port->gv.sio_bytes_to_tx                 = DSM_DS_SMALL_ITEM_SIZ;
  mdd_port->gv.sio_stream_close_func_ptr       = NULL;
  mdd_port->gv.sio_tail_char_used              = FALSE;

  mdd_port->gv.sio_switch_streams              = FALSE;
  mdd_port->gv.sio_chk_empty_tx_fifo_condition = FALSE;

  mdd_port->gv.sio_escape_code_func_ptr        = NULL;
  mdd_port->gv.sio_packet_func_ptr             = NULL;
  mdd_port->gv.sio_packet_detection_active     = FALSE;
  mdd_port->gv.sio_uart_packet_mode            = FALSE;

  mdd_port->fv.sio_tx_flow_control_method      = SIO_CTSRFR_FCTL;
  mdd_port->fv.sio_rx_flow_control_method      = SIO_CTSRFR_FCTL;
  mdd_port->fv.sio_rx_software_flow_control    = FALSE;
  mdd_port->fv.sio_tx_software_flow_control    = FALSE;
  mdd_port->fv.sio_sw_fc_fail_safe             = FALSE;

  mdd_port->fv.sio_outbound_flow_disabled      = FALSE;
  mdd_port->fv.sio_inbound_flow_enabled        = FALSE;

  mdd_port->adv.ds_incall_device_id            = UART_DEVICE_INVALID;
  mdd_port->adv.autodetect_device_id           = UART_DEVICE_INVALID;
  mdd_port->adv.state                          = SIO_AD_INIT;
  mdd_port->adv.packetpause_occurred           = FALSE;

  /*-------------------------------------------------------------------------
   Define call back items.  In mc.c, clk_init is called after sio_init.
   However, these calls do not require clk initialzation.  It would be
   an error to do clk_reg and clk_dereg calls here, however.
  -------------------------------------------------------------------------*/
  clk_def( &( mdd_port->gv.sio_stream_close_cb_item));
  clk_def( &( mdd_port->gv.sio_esc_seq_cb_item));
} /* siors_initialize_state */



/*===========================================================================

FUNCTION SIO_MDD_API_DISABLE_DEVICE                  EXTERNALIZED FUNCTION

DESCRIPTION
  This routine disables given device, including turning off clock to save
  power.  This is different from UART powerdown feature which attempts to
  save power with minimal loss of data.  Instead, this function is meant 
  for customers who may not want to use both UARTs.
  
  Effects of this function may be reversed by calling sio_rs232_dev_init.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void siors_mdd_api_disable_device
(
  sio_port_id_type   port_id              /* SIO Port ID                   */  
)
{
  sio_mdd_port_type   *mdd_port;          /* Port                          */
  uart_device_id_type  device_id;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  SIO_RS232_CHECK_PORT_EXIST(port_id);
  device_id = uart_sio_port_id_to_device_id( port_id );

  mdd_port = &(siors_mdd_ports[device_id]);
  if (!mdd_port->is_clock_enabled)  return;
  
  (mdd_port->fp.close)( mdd_port->pPortHead );
  mdd_port->is_clock_enabled = FALSE;
  sleep_assert_okts( mdd_port->gv.sleep_handle );
}


/*===========================================================================

FUNCTION SIO_MDD_API_DEV_INIT                        EXTERNALIZED FUNCTION

DESCRIPTION
  Initialize the MSM UART serial interface for operation and initialize 
  data structures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void siors_mdd_api_dev_init (void)
{
  sio_mdd_port_type *mdd_port;               
  uart_device_id_type device_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize sequence, done for all UART port in use:
   * 1. Initialize the PDD layer
   * 2. Initialize all state variables
   * 3. Initialize sleep structure, register with sleep service
   * 4. Vote to sleep, so phone can go to sleep by default
   *                                                                 */

  UART_INIT_LOCK();

#ifdef FEATURE_FIRST_UART
  device_id = UART_DEVICE_1;
  mdd_port = &(siors_mdd_ports[device_id]); 
  mdd_port->device_id = device_id;
  SIORS_UART1_INITIALIZE(mdd_port->device_id,
                         &(mdd_port->fp), 
                         &(mdd_port->pPortHead) ,
                         mdd_port,
                         siors_mdd_callback_entry);
  siors_mdd_initialize_state(mdd_port);
  mdd_port->gv.sleep_handle = sleep_register("UART1", TRUE);
  if(SIORS_FIRST_UART_NEGATE_ARM_HALT) {
    sleep_set_mclk_restriction(mdd_port->gv.sleep_handle, CLKRGM_MCLK_ON);
    sleep_set_hclk_restriction(mdd_port->gv.sleep_handle,CLKRGM_HCLK_FULL);
  }
  sleep_assert_okts( mdd_port->gv.sleep_handle );
  mdd_port->is_clock_enabled = FALSE;
  mdd_port->gv.can_wake_on_rx_event = SIORS_FIRST_UART_PD_WAKE_ON_RX;
#endif 

#ifdef FEATURE_SECOND_UART
  device_id = UART_DEVICE_2;
  mdd_port = &(siors_mdd_ports[device_id]); 
  mdd_port->device_id = device_id;
  SIORS_UART2_INITIALIZE(mdd_port->device_id,
                         &(mdd_port->fp), 
                         &(mdd_port->pPortHead) ,
                         mdd_port,
                         siors_mdd_callback_entry);
  siors_mdd_initialize_state(mdd_port);
  mdd_port->gv.sleep_handle = sleep_register("UART2", TRUE);
  if(SIORS_SECOND_UART_NEGATE_ARM_HALT) {
    sleep_set_mclk_restriction(mdd_port->gv.sleep_handle, CLKRGM_MCLK_ON);
  }
  sleep_assert_okts( mdd_port->gv.sleep_handle );
  mdd_port->is_clock_enabled = FALSE;
  mdd_port->gv.can_wake_on_rx_event = SIORS_SECOND_UART_PD_WAKE_ON_RX;
#endif 

#ifdef FEATURE_THIRD_UART
  device_id = UART_DEVICE_3;
  mdd_port = &(siors_mdd_ports[device_id]); 
  mdd_port->device_id = device_id;
  SIORS_UART3_INITIALIZE(mdd_port->device_id,
                         &(mdd_port->fp), 
                         &(mdd_port->pPortHead) ,
                         mdd_port,
                         siors_mdd_callback_entry);
  siors_mdd_initialize_state(mdd_port);
  mdd_port->gv.sleep_handle = sleep_register("UART3", TRUE);
  if(SIORS_THIRD_UART_NEGATE_ARM_HALT) {
    sleep_set_mclk_restriction(mdd_port->gv.sleep_handle, CLKRGM_MCLK_ON);
  }
  sleep_assert_okts( mdd_port->gv.sleep_handle );
  mdd_port->is_clock_enabled = FALSE;
  mdd_port->gv.can_wake_on_rx_event = SIORS_THIRD_UART_PD_WAKE_ON_RX;
#endif 

#ifdef FEATURE_UART_POWERDOWN
  /* Initialize the clock calback structure */
  clk_def( &siors_mdd_check_no_cable_cb_item);
/* ------------------------------------------------------------------------
 * since tmc.c calls clk_init() before sio_init(), we were able to move 
 * registral of siors_check_no_cable_cb_isr() to this spot. 
 * ------------------------------------------------------------------------*/
  siors_mdd_check_no_cable_enabled = FALSE;
  SIORS_MDD_REGISTER_CABLE_CHECK_TIMER();
  /* Depreicated, always sleep */
  sleep_uart_clock_rgm = FALSE;
#endif /* FEATURE_UART_POWERDOWN */
} 


/*===========================================================================

FUNCTION SIORS_MDD_ALLOC_STREAM_RX                          INTERNAL FUNCTION

DESCRIPTION
  This function allocates RX.  If all available RX streams are allocated
  (can be just one), this routine returns SIO_UNAVAIL_S.
                       
DEPENDENCIES
  Should only be called from sio_open.  open_ptr should be the same pointer
  which was passed to sio_open.

RETURN VALUE
  SIO_DONE_S: RX was successfully allocated.
  SIO_UNAVAIL_S: RX could not be allocated.

SIDE EFFECTS
  None

===========================================================================*/

static sio_status_type siors_mdd_alloc_stream_rx
(
  sio_stream_id_type   stream_id,         /* Stream ID                     */
  sio_mdd_port_type   *mdd_port,          /* Port                          */
  const sio_open_type *open_ptr           /* Configuration Information     */
)
{
  sio_status_type return_val;             /* Return Value                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  UART_LOCK();

  if ( mdd_port->gv.sio_stream_open_rx)
  {
    return_val = SIO_UNAVAIL_S;
  } /* if RX resource already allocated */
  else
  {
    /*----------------------------------------------------
      Assign static variables with the Stream and port IDs.
      Store stream modes and characteristics.
    ----------------------------------------------------*/

    mdd_port->gv.sio_stream_open_rx      = TRUE;
    mdd_port->gv.sio_current_stream_mode = open_ptr->stream_mode;
    mdd_port->gv.sio_current_stream_id   = stream_id;

    /*----------------------------------------------------
     If tail character is defined, SIO will always
     end a packet with the unescaped tail character,
     if it sees a tail character.
    ----------------------------------------------------*/

    mdd_port->gv.sio_tail_char_used = open_ptr->tail_char_used;
    mdd_port->gv.sio_tail_char      = open_ptr->tail_char;
  
    /*----------------------------------------------------
     These are parameters for backwards compatible
     functions, which are handled with extra calls
     that only specific streams are allowed to use.
     For now, reset these values.
    ----------------------------------------------------*/

    mdd_port->gv.sio_escape_code_func_ptr = NULL;
    mdd_port->gv.sio_packet_func_ptr      = NULL;
   
    mdd_port->gv.sio_rx_watermark_queue_ptr = open_ptr->rx_queue; 
    mdd_port->gv.sio_rx_func_ptr            = open_ptr->rx_func_ptr;
  
#ifdef SIORS_HAS_AUTODETECT  //{
    if ( ( mdd_port->gv.sio_current_stream_mode) == SIO_DS_RAWDATA_MODE ||
         ( mdd_port->gv.sio_current_stream_mode) == SIO_DS_PKT_MODE)
    {
      /*--------------------------------------------------
       Check to insure that RAW_DATA mode is not already
       active on either UART. If so kill the phone as
       this is a pathological error that should never
       exit lab testing.
      --------------------------------------------------*/
      if( mdd_port->adv.ds_incall_device_id != UART_DEVICE_INVALID)
      {
        ERR_FATAL( "Multiple UARTS in 'data call' mode",0,0,0);
      }
      else /* OK to go into RAW_DATA or Packet mode */
      {
        /*--------------------------------
         Assign Port and Stream IDs for
         DS-in-call mode operation
        --------------------------------*/
        /* TODO:  Fix the DS incall */
        mdd_port->adv.ds_incall_device_id = mdd_port->device_id;
        mdd_port->adv.ds_incall_stream_id = stream_id;
      }
      if ( ( mdd_port->gv.sio_current_stream_mode) == SIO_DS_RAWDATA_MODE)
      {
      
        /*--------------------------------------------------
         Transition to RawData stream involves connecting
         a call, which takes many seconds.  Therefore,
         for all practical purposes, the very first 
         Guard Time has happened.
        --------------------------------------------------*/
        mdd_port->gv.sio_esc_seq_state = SIO_ESC_SEQ_1ST_GUARDTIME;
      }
    } /* if current mode is RawData mode */
    else if ( mdd_port->gv.sio_current_stream_mode == SIO_DS_AUTODETECT_MODE)
    {
      /*-------------------------------------------------------------
       Perform Run-time Mode check then Setup UART, if all is proper
      -------------------------------------------------------------*/

      /*--------------------------------
       Assign Port and Stream IDs for
       Autodetect mode operation
      --------------------------------*/
      mdd_port->adv.autodetect_stream_id = stream_id;
      mdd_port->adv.autodetect_device_id = mdd_port->device_id;
      siors_mdd_setup_uart_autodetect( mdd_port );
    } 
#endif //}

    return_val = SIO_DONE_S;

  }  /* if RX resource not already allocated */

  UART_UNLOCK();

  return return_val;
} /* siors_alloc_stream_rx */


/*===========================================================================

FUNCTION SIORS_MDD_DEALLOC_STREAM_RX                        INTERNAL FUNCTION

DESCRIPTION
  This function deallocates RX.

DEPENDENCIES
  Should only be called from sio_open or sio_close.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_mdd_dealloc_stream_rx
(
  sio_stream_id_type stream_id,              /* Stream ID                  */
  sio_mdd_port_type   *mdd_port              /* Port                       */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT( mdd_port->gv.sio_stream_open_rx == TRUE);

  mdd_port->gv.sio_stream_open_rx         = FALSE;
  mdd_port->fp.disable_rx( mdd_port->pPortHead );
  mdd_port->gv.sio_rx_watermark_queue_ptr = NULL;
  mdd_port->gv.sio_rx_func_ptr            = NULL;

  /*------------------------------------------------------
   Do book-keeping necessary for just SIO_RAWDATA_STREAM.
  ------------------------------------------------------*/

  if ( mdd_port->gv.sio_current_stream_mode == SIO_DS_RAWDATA_MODE ||
       mdd_port->gv.sio_current_stream_mode == SIO_DS_PKT_MODE)
  {
    /*-----------------------------
      Clear DS-in-call IDs
    -----------------------------*/
    mdd_port->adv.ds_incall_device_id = UART_DEVICE_INVALID;
    mdd_port->adv.ds_incall_stream_id = SIO_NO_STREAM_ID;

    mdd_port->adv.autodetect_device_id = UART_DEVICE_INVALID;
    mdd_port->adv.autodetect_stream_id = SIO_NO_STREAM_ID;

    if ( ( mdd_port->gv.sio_current_stream_mode) == SIO_DS_RAWDATA_MODE)
    {
      /*----------------------------------------------------
       De register the clock call-back function used by 
       SIO_RAWDATA_STREAM. Clear the DS-in-call mode 
       IDs
      ----------------------------------------------------*/
      clk_dereg( &( mdd_port->gv.sio_esc_seq_cb_item));
    }
  } 
  else if ( mdd_port->gv.sio_current_stream_mode == SIO_DS_AUTODETECT_MODE)
  {
    /*-----------------------------------------------------------------
     De-register the discard_recovery_cb_isr, reset discard_active flag
     ----------------------------------------------------------------*/
    clk_dereg(&(mdd_port->adv.pausecheck_cb_item));
    mdd_port->adv.state = SIO_AD_INIT;

    /*----------------------------------------------------
     De-register RX Break callback. Clear Autodetect mode
     IDs.
    ----------------------------------------------------*/
    // mdd_port->adv.autodetect_stream_id = SIO_NO_STREAM_ID;
  } /* if stream_id == SIO_DS_AUTODETECT_MODE */

  return;
} /* siors_dealloc_stream_rx */


/*===========================================================================

FUNCTION SIORS_MDD_ALLOC_STREAM_TX                          INTERNAL FUNCTION

DESCRIPTION
  This function allocates TX.  If all available TX streams are allocated
  (can be just one), this routine returns SIO_UNAVAIL_S.

DEPENDENCIES
  Should only be called from sio_open.  open_ptr should be the same pointer
  which was passed to sio_open.  

RETURN VALUE
  SIO_DONE_S: TX was successfully allocated.
  SIO_UNAVAIL_S: TX could not be allocated.

SIDE EFFECTS
  None

===========================================================================*/

static sio_status_type siors_mdd_alloc_stream_tx
(
  sio_stream_id_type   stream_id,            /* Stream ID                  */
  sio_mdd_port_type   *mdd_port,             /* Port                       */
  const sio_open_type *open_ptr
)
{
  sio_status_type return_val;             /* Return Value                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  UART_LOCK();

  if ( mdd_port->gv.sio_stream_open_tx)
  {
    return_val = SIO_UNAVAIL_S;         /* if TX resource already allocated */
  } 
  else
  {
    mdd_port->gv.sio_stream_open_tx      = TRUE;
    mdd_port->gv.sio_current_stream_mode = open_ptr->stream_mode;
    mdd_port->gv.sio_current_stream_id   = stream_id;

    /*-----------------------------------------------------
     Reset All of the TX variables upon opening of each
     stream.
    -----------------------------------------------------*/

    mdd_port->gv.sio_tx_watermark_queue_ptr = open_ptr->tx_queue;
    mdd_port->gv.sio_tx_current_wm_item_ptr = NULL;
    mdd_port->gv.sio_bytes_to_tx            = 0;
    mdd_port->fp.enable_tx ( mdd_port->pPortHead );

    return_val                             = SIO_DONE_S;

  } /* if TX resource not already allocated */

  UART_UNLOCK();

  return return_val;
} /* siors_alloc_stream_tx */


/*===========================================================================

FUNCTION SIORS_MDD_UNCEREMONIOUS_CLOSE                         INTERNAL FUNCTION

DESCRIPTION
  
  This function is called when caller (task or ISR) needs to close a stream
  in short order.  The only requirement is that the serial device and memory
  pool are not left in bad state such that more streams cannot be opened.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_mdd_unceremonious_close
(
  sio_stream_id_type stream_id,           /* Stream ID                     */
  sio_mdd_port_type   *mdd_port              /* Port                       */
)
{
  dsm_item_type *item_ptr;                /* Temporary for packet          */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------
   If control flow reaches this point, a signal was not defined.  
   Therefore, empty the TX queue, if it is not already empty
   and then reset the TX queue variable.  Record an error if
   TX queue is not empty because the caller should have emptied
   the TX queue before calling this routine.
   Disable interrupts since we're dumping any pending data.
  ---------------------------------------------------------------*/
  mdd_port->fp.disable_tx_interrupt ( mdd_port->pPortHead );
  mdd_port->gv.sio_tx_current_wm_item_ptr = NULL;
  mdd_port->gv.sio_bytes_to_tx            = 0;

  if ( (item_ptr = dsm_dequeue(mdd_port->gv.sio_tx_watermark_queue_ptr)) != NULL)
  {
    ERR( "sio_close called with non-emtpy TX queue",0,0,0);
    (void)dsm_free_buffer( item_ptr);
    dsm_empty_queue( mdd_port->gv.sio_tx_watermark_queue_ptr);
  }
  mdd_port->gv.sio_tx_watermark_queue_ptr = NULL;
  if(mdd_port->is_clock_enabled)
  {
    mdd_port->fp.close ( mdd_port->pPortHead );
    mdd_port->is_clock_enabled = FALSE;
    sleep_assert_okts( mdd_port->gv.sleep_handle );
  }
} 


/*===========================================================================

FUNCTION SIORS_MDD_DEALLOC_STREAM_TX                        INTERNAL FUNCTION

DESCRIPTION
  This function deallocates TX.

DEPENDENCIES
  Should only be called from sio_open or sio_close.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_dealloc_stream_tx
(
  sio_stream_id_type   stream_id,             /* Stream ID                   */
  sio_mdd_port_type   *mdd_port,              /* Port                        */
  void               (*close_func_ptr)(void)  /* Function to call when 
                                                 pending transmission is 
                                                 complete                    */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT( mdd_port->gv.sio_stream_open_tx == TRUE);
  mdd_port->gv.sio_stream_open_tx     = FALSE;

  /*------------------------------------------------------------------------
   If function pointer is defined, wait until all pending TX has finished 
   and then call function pointed to by function pointer.
  -------------------------------------------------------------------------*/
  if ( close_func_ptr != NULL)
  {
    mdd_port->gv.sio_closing_stream = TRUE;
    siors_mdd_schdl_stream_chng( mdd_port, close_func_ptr); 
  }
  else
  {
    siors_mdd_unceremonious_close( stream_id, mdd_port);
  }
  return;
} 


/*===========================================================================

FUNCTION SIORS_MDD_API_FLUSH_TX                       EXTERNALIZED FUNCTION

DESCRIPTION
  This function flushes the TX.

DEPENDENCIES
 User should not queue more data until after the registered callback
 is executed.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void siors_mdd_api_flush_tx
(
  sio_stream_id_type stream_id,             /* Stream ID                   */
  sio_port_id_type   port_id,               /* Port ID                     */
  void             (*flush_func_ptr)(void)  /* Function to call when 
                                               pending transmission is 
                                               complete                    */
)
{
  sio_mdd_port_type   *mdd_port;
  uart_device_id_type  device_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT( flush_func_ptr != NULL);
  SIO_RS232_CHECK_PORT_EXIST(port_id);
  device_id = uart_sio_port_id_to_device_id( port_id );
  mdd_port = &(siors_mdd_ports[device_id]);

  ASSERT(mdd_port->gv.sio_stream_open_tx == TRUE);

  mdd_port->gv.sio_closing_stream = FALSE;
  siors_mdd_schdl_stream_chng( mdd_port, flush_func_ptr); 
  return;
}


/*===========================================================================

FUNCTION SIORS_MDD_API_OPEN_STREAM                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function allocates stream as indicated in open_ptr.
    
DEPENDENCIES
  None
  
RETURN VALUE
  SIO_DONE_S: TX was successfully allocated.
  SIO_UNAVAIL_S: TX could not be allocated.  
  
SIDE EFFECTS
  None
  
===========================================================================*/

sio_status_type siors_mdd_api_open_stream
(
  sio_open_type     *open_ptr         /* Configuration Information         */
)
{
  sio_status_type      return_val = SIO_DONE_S; /* Return Value            */
  sio_stream_id_type   stream_id;               /* Stream ID               */
  sio_port_id_type     port_id;                 /* Port ID                 */
  sio_mdd_port_type   *mdd_port;
  uart_device_id_type  device_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  stream_id                         = open_ptr->stream_id;
  port_id                           = open_ptr->port_id;
  

  SIO_RS232_CHECK_PORT_EXIST(port_id);
  siors_mdd_stream_port[ stream_id] = port_id;

  device_id = uart_sio_port_id_to_device_id( port_id );
  if( UART_DEVICE_INVALID == device_id )
  {
    return( SIO_BADP_S );
  }
  
  mdd_port = &(siors_mdd_ports[device_id]);

  mdd_port->gv.sio_type_of_stream    = open_ptr->stream_type;
  mdd_port->gv.open_stream_id        = stream_id;

  /* Open the serial port and set the clock ON when the port is opened */
  mdd_port->fp.open ( mdd_port->pPortHead, SLOW );
  mdd_port->is_clock_enabled = TRUE;

  /* When the port is in use, do not sleep */
  sleep_negate_okts(mdd_port->gv.sleep_handle);

  SIORS_MDD_REGISTER_CABLE_CHECK_TIMER();

  /* Move setting of the bitrate to here */
  return_val = siors_mdd_chng_bitrate( mdd_port, open_ptr);

  if ( return_val != SIO_DONE_S)
  {
    return (sio_status_type)SIO_NO_STREAM_ID;  
  }
  
  if ( ( ( mdd_port->gv.sio_type_of_stream) == SIO_STREAM_RXTX) || 
       ( ( mdd_port->gv.sio_type_of_stream) == SIO_STREAM_TX_ONLY))
  {
    return_val = siors_mdd_alloc_stream_tx( stream_id, mdd_port, open_ptr);

    if ( return_val != SIO_DONE_S) /* If we could not allocate TX, exit */
    {
      return (sio_status_type)SIO_NO_STREAM_ID;  
    } 
  } /* if sio_type_of_stream needs TX resources */

  if ( ( ( mdd_port->gv.sio_type_of_stream) == SIO_STREAM_RXTX) || 
       ( ( mdd_port->gv.sio_type_of_stream) == SIO_STREAM_RX_ONLY))
  {
    return_val = siors_mdd_alloc_stream_rx( stream_id, mdd_port, open_ptr);
    
    if ( return_val != SIO_DONE_S)
    {
      if ( ( mdd_port->gv.sio_type_of_stream) == SIO_STREAM_RXTX)
      {
        /*------------------------------------------------------
         If control flow reaches this point, we could not open
         RX but we were able to open TX.  So, before exiting,
         need to deallocate RX.
        ------------------------------------------------------*/
        siors_mdd_dealloc_stream_tx( stream_id, mdd_port, NULL);  
      } 

      return (sio_status_type)SIO_NO_STREAM_ID;  

    } /* if return_val != SIO_DONE_S */ 

  } /* if sio_type_of_stream needs RX resources */

  /* Update the hardware flow control setting so the function is used at least
   * once */
  siors_mdd_update_flow_control( mdd_port );
  return return_val;
} /* siors_mdd_api_open_stream */


/*===========================================================================

FUNCTION SIORS_MDD_API_CLOSE_STREAM                       EXTERNALIZED FUNCTION

DESCRIPTION
  This function deallocates given stream.
    
DEPENDENCIES
  None
  
RETURN VALUE
  None
   
SIDE EFFECTS
  None
  
===========================================================================*/

void siors_mdd_api_close_stream
(
  sio_stream_id_type stream_id,             /* Stream ID                   */
  void             (*close_func_ptr)(void)  /* Function to call when 
                                               pending transmission is 
                                               complete                    */
)
{
  sio_port_id_type port_id;                 /* Port ID                     */
  uart_device_id_type  device_id;
  sio_mdd_port_type   *mdd_port;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT( stream_id < SIO_MAX_STREAM);
  port_id = siors_mdd_stream_port[ stream_id];
  SIO_RS232_CHECK_PORT_EXIST(port_id);
  device_id = uart_sio_port_id_to_device_id( port_id );
  mdd_port = &(siors_mdd_ports[device_id]);

  /*------------------------------------------------------------
   If this is the MAIN UART then clear the siors_dtr_func_ptr
  ------------------------------------------------------------*/
  if ( port_id == SIO_PORT_UART_MAIN ) 
  {
    siors_mdd_api_ioctl( stream_id, port_id, SIO_IOCTL_DISABLE_DTR_EVENT, NULL );
  }

  /*-----------------------------------------------------------------------
   The following statements have to be executed as an atomic unit.  
   Therefore, lock out interrupts.
  -------------------------------------------------------------------------*/
  UART_LOCK();

  if ( ( mdd_port->gv.sio_type_of_stream == SIO_STREAM_RXTX) || 
       ( mdd_port->gv.sio_type_of_stream == SIO_STREAM_RX_ONLY))
  {
    siors_mdd_dealloc_stream_rx( stream_id, mdd_port);     
  }

  if ( ( mdd_port->gv.sio_type_of_stream == SIO_STREAM_RXTX) || 
       ( mdd_port->gv.sio_type_of_stream == SIO_STREAM_TX_ONLY))
  {
    siors_mdd_dealloc_stream_tx( stream_id, mdd_port, close_func_ptr);          
  }

  UART_UNLOCK();

} /* siors_mdd_api_close_stream */


/*===========================================================================

FUNCTION SIORS_MDD_API_DEV_TRANSMIT                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function transmits over MSM's RS-232 UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void siors_mdd_api_dev_transmit
(
  sio_stream_id_type stream_id,           /* Stream ID                     */
  sio_port_id_type   port_id,             /* SIO Port ID                   */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
)
{
  sio_mdd_port_type   *mdd_port;
  uart_device_id_type  device_id;
  int32    byte_count;
  int32    tx_byte_cnt = 0;
  uint8   *current_tx_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check to see if the port is valid and assign 
   * interval variables for easy access                                    */
  SIO_RS232_CHECK_PORT_EXIST(port_id);
  device_id = uart_sio_port_id_to_device_id( port_id );
  mdd_port = &(siors_mdd_ports[device_id]);
  UART_LOCK();

  /*------------------------------------------------------------------------
    If we did shut down UART and its clock regime, we should not access 
    UART registers any more.  So throw data away in this case 
   -------------------------------------------------------------------------*/ 
  if (!mdd_port->is_clock_enabled)
  {
    dsm_free_packet( &tx_ptr );
  }
  else
  {

    ASSERT( mdd_port->gv.sio_stream_open_tx == TRUE);

    if ( ( ( ( mdd_port->gv.sio_tx_current_wm_item_ptr) != NULL) && 
           ( ( mdd_port->gv.sio_bytes_to_tx) > 0))               ||
         ( mdd_port->fv.sio_outbound_flow_disabled))
    {
      /*-----------------------------------------------------------------------
       Need to enqueue the given packet.  Some packet is already in the process
       of being transmitted or the link is flow controlled.  It is permissable 
       to call dsm_simple_enqueue_isr here because this code is contained 
       within a critical section. 
      -----------------------------------------------------------------------*/
      SIORS_TRACE_CODE(110, device_id);
      /*----------------------------------------------------------------------
        sio_rs232_dev_transmit() is handling pkt_chain now.  So we call 
        dsm_enqueue() in stand of dsm_simple_enqueue_isr().
       ----------------------------------------------------------------------*/ 
      dsm_enqueue( mdd_port->gv.sio_tx_watermark_queue_ptr, &tx_ptr);
    } /* if Transmitter Full */
    else
    {
      mdd_port->gv.sio_tx_current_wm_item_ptr = tx_ptr;
      mdd_port->gv.sio_bytes_to_tx            = tx_ptr->used;
      current_tx_ptr = tx_ptr->data_ptr;
      mdd_port->gv.sio_chk_empty_tx_fifo_condition = FALSE;

      while(mdd_port->gv.sio_bytes_to_tx)
      {
        /* Loop until either the packet is empty or FIFO is full */
        byte_count = mdd_port->gv.sio_bytes_to_tx;
        mdd_port->fp.transmit(mdd_port->pPortHead, current_tx_ptr, &byte_count);
        mdd_port->gv.sio_bytes_to_tx -= byte_count;
    
        /* If the packet is empty, traverse and get the next packet */
        if(mdd_port->gv.sio_bytes_to_tx == 0)     
        {
          if ( mdd_port->gv.sio_tx_current_wm_item_ptr->pkt_ptr != NULL )
          { 
            /*---------------------------------------------------------------
              if more items are chained after this item, travers the pkt_chain
              and continue transmiting 
            ----------------------------------------------------------------*/
            siors_mdd_get_more_data_for_tx (mdd_port); 
            current_tx_ptr = mdd_port->gv.sio_current_tx_ptr;

            /*----------------------------------------------------------------
              Total bytes txed per sio_transmit() call is limited by a UPPER
              BOUND, so that sio_transmit() function would not last too long.
              Note: sio_transmit() LOCK interrupt.
             ---------------------------------------------------------------*/
            tx_byte_cnt += mdd_port->gv.sio_bytes_to_tx;
            if ( tx_byte_cnt > RS232_TX_UPPER_BOUND) break;
          }
          else  /* if packet is chained */
          { 
            /* If there is no more data in the packet, stop stuffing FIFO. */
            break;
          }
        }
        else /* if FIFO is full */
        {
          /* FIFO is full, record the number of bytes left and return */  
          mdd_port->gv.sio_current_tx_ptr = current_tx_ptr + byte_count;
          break;
        }
      } 
    }
    /* Setup TX interrupt if needed */
    siors_mdd_get_more_data_for_tx(mdd_port);
  }
  UART_UNLOCK();
} 


/*===========================================================================

FUNCTION SIORS_MDD_CHNG_BITRATE                             INTERNAL

DESCRIPTION
  This function changes the Bit Rate and does the necessary record keeping.

DEPENDENCIES
  Should only be called from sio_open.  open_ptr should be the same pointer
  which was passed to sio_open.
  
  Should be called before siors_enable_receiver or siors_enable_transmitter.

RETURN VALUE
  SIO_DONE_S: Bit-rate was set correctly.
  Currently, SIO just dies if requested bit-rate is invalid.  However, for
  devices other than RS-232 serial devices, this interface may return
  something other than SIO_DONE_S.

SIDE EFFECTS
  None

===========================================================================*/

sio_status_type siors_mdd_chng_bitrate
(
 sio_mdd_port_type          *mdd_port,
  const sio_open_type       *open_ptr           /* Stream configuration    */
)
{
  sio_bitrate_type     new_rx_bitrate;          /* Requested RX bit-rate   */
  sio_bitrate_type     new_tx_bitrate;          /* Requested TX bit-rate   */
  sio_status_type      return_val = SIO_DONE_S; /* Return status           */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  new_rx_bitrate = open_ptr->rx_bitrate;
  new_tx_bitrate = open_ptr->tx_bitrate;

  /*-------------------------------------------------------------------
   The requested rates must be equal and both less than SIO_BITRATE_MAX
  -------------------------------------------------------------------*/
  ASSERT( new_rx_bitrate < SIO_BITRATE_MAX);
  ASSERT( new_tx_bitrate == new_rx_bitrate);        

  /*-----------------------------------------------------------------
   Find out whether or not bit-rate needs changing
  -------------------------------------------------------------------
   Since we're requiring that the requested Rx and Tx to be equal, and 
   we're going to set the rate regardless of what the requested value is,
   and the sio_current_*x_bitrate's might differ due to past autobauding 
   [ see function siors_set_bit_rate() ], we'll give the current Tx rate 
   precedence.
   Note: we don't clear/reset sio_current_*x_bitrate's on close.
  -------------------------------------------------------------------*/

  if ( new_rx_bitrate == SIO_BITRATE_BEST )
  {
    /*--------------------------------------------------------------- 
     Set both to the Tx rate (We don't use both the Tx and Rx rates 
     now, but we may in the future.) 
    ---------------------------------------------------------------*/
    new_rx_bitrate = mdd_port->gv.sio_current_tx_bitrate;
    new_tx_bitrate = mdd_port->gv.sio_current_tx_bitrate;
  }

  /*----------------------------------------------------------------
   Actually ask for bit-rate change.  Even if the bit-rate remains 
   the same, mode and flow control methods may have changed which 
   _may_ affect the RX_LEV.  So, explicitly set bit-rate anyway.
  ------------------------------------------------------------------*/
  if ( new_rx_bitrate == new_tx_bitrate)
  {
    mdd_port->gv.sio_current_rx_bitrate          = new_rx_bitrate;
    mdd_port->gv.sio_current_tx_bitrate          = new_tx_bitrate;
    mdd_port->fp.set_bit_rate(mdd_port->pPortHead, new_rx_bitrate);
  }
  else  
  {
    ERR_FATAL( "(RX bitrate) != (TX bitrate)",0,0,0);
  }

  return return_val;
} /* siors_chng_bitrate */


/*===========================================================================

FUNCTION SIORS_MDD_SET_FLOW_CTL                             INTERNAL FUNCTION

DESCRIPTION
  This function will set the flow control method between the DCE
  and the DTE for the given device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_mdd_set_flow_ctl
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  sio_mdd_port_type *mdd_port,
  sio_flow_ctl_type  tx_flow,                 /* TX flow control method    */
  sio_flow_ctl_type  rx_flow                  /* RX flow control method    */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
   For this device, best means no change.
  -------------------------------------------------------------------------*/

  if ( tx_flow == SIO_FCTL_BEST)
  {
    tx_flow = mdd_port->fv.sio_tx_flow_control_method; 
  }

  if ( rx_flow == SIO_FCTL_BEST)
  {
    rx_flow = mdd_port->fv.sio_rx_flow_control_method; 
  }

  /*-------------------------------------------------------------------------
   Every time that flow control is reset, need to reset XON XOFF flow 
   control variables.  If switching to XON/XOFF, need to have the 
   variables in a good state.  And if switching from XON/XOFF, need to 
   have XON/XOFF flow control variables to not interfere with HW or other 
   flow control methods
  -------------------------------------------------------------------------*/
  if ( ( mdd_port->fv.sio_rx_flow_control_method) != rx_flow)
  {
    mdd_port->fv.sio_sw_fc_fail_safe = FALSE;

    /*-----------------------------------------------------------------------
     It is ambiguous as to whether or not the old flow control state should
     be kept when flow control method is changed.  For instance, should
     software consider a previous asserted hardware flow control line as
     tantamount to having received an XOFF ?  For now, default to deasserted
     flow control.
    -----------------------------------------------------------------------*/
    mdd_port->fv.sio_inbound_flow_enabled       = TRUE;
    mdd_port->fv.sio_sw_fc_ctlchr_tx_pending    = FALSE;
    mdd_port->fv.sio_sw_fc_pending_tx_fail_safe = FALSE;
    
  } /* if old_rx_flow != rx_flow */
  
  if ( ( mdd_port->fv.sio_tx_flow_control_method) != tx_flow)
  {
    /*-----------------------------------------------------------------------
     It is ambiguous as to whether or not the old flow control state 
     should be kept when flow control method is changed.  For instance,
     in software flow control TX has to be explicitly stopped by software,
     wheareas in hardware flow control, TX flow control is done automatically
     by UART hardware.
    -----------------------------------------------------------------------*/
    mdd_port->fv.sio_outbound_flow_disabled = FALSE;
  } 
  
  mdd_port->fv.sio_tx_flow_control_method = tx_flow;
  mdd_port->fv.sio_rx_flow_control_method = rx_flow;

  /*-------------------------------------------------------------------------
   Software flow control is not supported anymore
  -------------------------------------------------------------------------*/
  if ( ( tx_flow == SIO_XONXOFF_STRIP_FCTL_FS)  ||
       ( tx_flow == SIO_XONXOFF_STRIP_FCTL_NFS) ||
       ( tx_flow == SIO_XONXOFF_NSTRIP_FCTL_FS) ||
       ( tx_flow == SIO_XONXOFF_NSTRIP_FCTL_NFS))
  {
    ERR_FATAL( "Software flow control not supported!", 0, 0, 0 );
  } /* If tx_flow == software flow control */

  if ( ( rx_flow == SIO_XONXOFF_STRIP_FCTL_FS)  ||
       ( rx_flow == SIO_XONXOFF_STRIP_FCTL_NFS) ||
       ( rx_flow == SIO_XONXOFF_NSTRIP_FCTL_FS) ||
       ( rx_flow == SIO_XONXOFF_NSTRIP_FCTL_NFS))
  {
    ERR_FATAL( "Software flow control not supported!", 0, 0, 0 );
  } /* if rx_flow == software flow control */

  mdd_port->fv.sio_tx_software_flow_control = FALSE;
  mdd_port->fv.sio_rx_software_flow_control = FALSE;

  /*-----------------------------------------------------------------------
   Flow control method is software flow control or no flow control.
   Need to stop listening to hardware flow control signal.
  -----------------------------------------------------------------------*/
  if ( tx_flow == SIO_FCTL_OFF )
  {
    mdd_port->fp.disable_tx_hw_fctl( mdd_port->pPortHead );
  } 
  else 
  {
    mdd_port->fp.enable_tx_hw_fctl( mdd_port->pPortHead );
  } 

  /* If RX flow control is not done through software or no flow control
   * used at all, disable the hardware auto flow control.              */
  if( rx_flow == SIO_FCTL_OFF )
  {
    mdd_port->fp.disable_auto_hw_fctl(mdd_port->pPortHead);
  }
  else if(rx_flow == SIO_CTSRFR_AUTO_FCTL)
  {
    /* If auto hardware flow control is what caller requested,
     * enable auto hardware flow control.
     * Have to set the RFR trigger level before enable the flow control */  
    mdd_port->fp.enable_auto_hw_fctl(mdd_port->pPortHead);
  }

  /* Update packet mode setting */
  siors_mdd_set_packet_mode(mdd_port);
} /* siors_mdd_set_flow_ctl */


/*===========================================================================

FUNCTION SIORS_MDD_GET_FLOW_CTL                             INTERNAL FUNCTION

DESCRIPTION
  This function will get the flow control method between the DCE
  and the DTE for the given device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Fills in the flow control methods into the param union
  
===========================================================================*/

static void siors_mdd_get_flow_ctl
( 
  sio_mdd_port_type   *mdd_port,
  sio_ioctl_param_type *param
)
{
  param->flow_ctl.tx_flow = mdd_port->fv.sio_tx_flow_control_method; 
  param->flow_ctl.rx_flow = mdd_port->fv.sio_rx_flow_control_method;
} 


/*===========================================================================

FUNCTION SIORS_MDD_DO_BITRATE_CHANGE

DESCRIPTION
  This function changes the bitrate on the UART.  This is 
  called when the TX FIFO has completely emptied.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_FIRST_UART
static void siors_mdd_do_bitrate_change1 (void)
{
  sio_mdd_port_type   *mdd_port;
  mdd_port = &(siors_mdd_ports[UART_DEVICE_1]);

  mdd_port->gv.sio_changing_baudrate = FALSE;
  mdd_port->gv.sio_switch_streams    = FALSE;

  mdd_port->fp.set_bit_rate(mdd_port->pPortHead,
                     mdd_port->gv.sio_new_bitrate);
}
#endif

#ifdef FEATURE_SECOND_UART
static void siors_mdd_do_bitrate_change2 (void)
{
  sio_mdd_port_type   *mdd_port;
  mdd_port = &(siors_mdd_ports[UART_DEVICE_2]);

  mdd_port->gv.sio_changing_baudrate = FALSE;
  mdd_port->gv.sio_switch_streams    = FALSE;

  mdd_port->fp.set_bit_rate(mdd_port->pPortHead,
                     mdd_port->gv.sio_new_bitrate);
}  
#endif

#ifdef FEATURE_THIRD_UART
static void siors_mdd_do_bitrate_change3 (void)
{
  sio_mdd_port_type   *mdd_port;
  mdd_port = &(siors_mdd_ports[UART_DEVICE_3]);

  mdd_port->gv.sio_changing_baudrate = FALSE;
  mdd_port->gv.sio_switch_streams    = FALSE;

  mdd_port->fp.set_bit_rate(mdd_port->pPortHead,
                     mdd_port->gv.sio_new_bitrate);
}  
#endif

/*===========================================================================

FUNCTION SIORS_MDD_SCHED_BAUDRATE_CHANGE

DESCRIPTION
  This function schedules a baud-rate change.  Here's what happens - 
  this function will set up some vars that tell the code to change
  the baudrate.  After the NEXT TX ISR, where there is no data
  left to be transmitted (either in the Tx watermark or in the Tx
  FIFO), then the baudrate is changed.
  
  Note: when this function is called, if there is currently no data
  left to be xmitted (s/w watermark and h/w fifo are empty), then the
  baudrate is NOT changed.  It must still wait for the next Tx ISR.
  
  This follows the scheme used in the old SIO.  That scheme assumed
  the baudrate was only changed on AT+IPR from the connected laptop;
  the DS code would call the sched_baudrate command, and then call
  the transmit() function to send the "OK" back to the laptop.  So
  we assume the same scheme.

DEPENDENCIES
  Assume that a Tx ISR will happen in our future.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void siors_mdd_sched_baudrate_change
(
  sio_stream_id_type stream_id,              /* Stream ID                  */
  sio_mdd_port_type   *mdd_port,
  sio_bitrate_type   new_bitrate             /* new bitrate                */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT( mdd_port->gv.sio_stream_open_tx == TRUE);
  ASSERT( new_bitrate < SIO_BITRATE_MAX);

  UART_LOCK();

  /*-------------------------------------------------------------
    First, we disable the receive side, since we are going to
    change bit rate.  Disable RX and flush RX fifo.  
    RX is re-enabled when mode change is successful.
    -------------------------------------------------------------
    Trouble with the DM is, when UARTDM is used, a reset receiver
    will disable RX, which is not the case with old UART.  If RX
    is disabled while TX is empty with nothing to send, the whole
    thing will get stuck as TX isr is not triggering, schdule
    a check to see if TX is empty at the end of this function
  ----------------------------------------------------------------------*/
    mdd_port->fp.reset_rx ( mdd_port->pPortHead );
    if(mdd_port->fv.sio_inbound_flow_enabled)
    {
      mdd_port->fp.enable_rx_hw_fctl ( mdd_port->pPortHead );
    }
  
  /*-------------------------------------------------------------
    The plan here is similar to when we flush the tx.  We 
    will set some vars that indicate that we want to change
    the baudrate, once the tx watermark and the tx fifo become
    empty.  Since we will insert ourselves into the mechanism
    used to determine empty tx fifo conditions (also used for
    closing and flushing a stream), we will set the
    sio_closing_stream var to false.
  --------------------------------------------------------------*/
  mdd_port->gv.sio_changing_baudrate = TRUE;
  mdd_port->gv.sio_closing_stream = FALSE;

  /*-------------------------------------------------------------
    We set the switch_streams var also.  We do this because this
    flag indicates to the code that we are going to make some
    change to the stream; we are not really switching a stream,
    but we will instead make a bitrate change...This flag tells
    the code that we should be looking for an empty tx fifo 
    condition.
  --------------------------------------------------------------*/
  mdd_port->gv.sio_switch_streams = TRUE;

  /*-------------------------------------------------------------
    Save the baudrate info in a global.  This info is later 
    accessed in siors_do_bitrate_change.
  --------------------------------------------------------------*/
  mdd_port->gv.sio_new_bitrate  = new_bitrate;

  /*-------------------------------------------------------------
    We then store the bitrate change func as the close_func_ptr.
    This is because when the tx finally empties, this callback is
    called (if appropriate flags are set).  We are re-using that
    functionality, but using our own callback.
  --------------------------------------------------------------*/
  switch(mdd_port->device_id)
  {
#ifdef FEATURE_FIRST_UART
	 case UART_DEVICE_1:
      mdd_port->gv.sio_stream_close_func_ptr = siors_mdd_do_bitrate_change1;
		break;
#endif

#ifdef FEATURE_SECOND_UART
	 case UART_DEVICE_2:
      mdd_port->gv.sio_stream_close_func_ptr = siors_mdd_do_bitrate_change2;
		break;
#endif

#ifdef FEATURE_THIRD_UART
    case UART_DEVICE_3:
      mdd_port->gv.sio_stream_close_func_ptr = siors_mdd_do_bitrate_change3;
#endif 

	 default:
      ERR_FATAL( "Invalid Port ID=%x", mdd_port->device_id, 0, 0 ); 
  }

  /* Make sure we check to see if TX is empty */
  siors_mdd_schedule_empty_tx_fifo_chk(mdd_port);
  UART_UNLOCK();
} /* siors_sched_baudrate_change */


/*===========================================================================

FUNCTION SIORS_MDD_BAUDRATE_CHANGE_NOW

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void siors_mdd_baudrate_change_now
(
  sio_mdd_port_type   *mdd_port,             /* Port                       */
  sio_bitrate_type     new_bitrate           /* new bitrate                */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT( new_bitrate < SIO_BITRATE_MAX);

  mdd_port->gv.sio_new_bitrate = new_bitrate;
  mdd_port->fp.reset_rx ( mdd_port->pPortHead );
  if(mdd_port->fv.sio_inbound_flow_enabled)
  {
    mdd_port->fp.enable_rx_hw_fctl ( mdd_port->pPortHead );
  }

  UART_LOCK();

  mdd_port->fp.set_bit_rate(mdd_port->pPortHead,
                     mdd_port->gv.sio_new_bitrate);

  UART_UNLOCK();
} 



/*===========================================================================

FUNCTION SIORS_MDD_UPDATE_FLOW_CONTROL                   INTERNAL

DESCRIPTION
  Update the hardware flow control setting using the stored setting
  in the port structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void siors_mdd_update_flow_control
(
  sio_mdd_port_type   *mdd_port              /* Port                       */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( mdd_port->fv.sio_tx_flow_control_method == SIO_FCTL_OFF )
  {
    mdd_port->fp.disable_tx_hw_fctl( mdd_port->pPortHead );
  } 
  else 
  {
    mdd_port->fp.enable_tx_hw_fctl( mdd_port->pPortHead );
  } 

  if( mdd_port->fv.sio_rx_flow_control_method == SIO_FCTL_OFF ||
		mdd_port->fv.sio_rx_flow_control_method == SIO_CTSRFR_FCTL  )
  {
    mdd_port->fp.disable_auto_hw_fctl(mdd_port->pPortHead);
  }
  else if( mdd_port->fv.sio_rx_flow_control_method == SIO_CTSRFR_AUTO_FCTL)
  {
    mdd_port->fp.enable_auto_hw_fctl(mdd_port->pPortHead);
  }

  if(mdd_port->fv.sio_inbound_flow_enabled)
  {
    mdd_port->fp.enable_rx_hw_fctl ( mdd_port->pPortHead );
  }
} 



/*===========================================================================

FUNCTION SIORS_MDD_CHANGE_MODE

DESCRIPTION
  Changes the operating mode characteristics of the SIO stream.
  The Tx and Rx may be enabled during this (if we had been autobauding 
  and we're now changing to RAW or PKT modes).
  
  if the new mode == the old mode, then we exit without doing 
  anything.

DEPENDENCIES
  Assumes the user has already opened the stream, and is merely
  changing modes.
  
  Any watermarks which are obsoleted are not freed; it is up to the 
  application to free information stored on the watermark.

  Also assumes that the user has flushed any pending output.
  
RETURN VALUE
  None

SIDE EFFECTS
  May end up in changing baud rate (if going into/out of autodetect).
===========================================================================*/
static void siors_mdd_change_mode
(
  sio_stream_id_type            stream_id,  /* Stream ID                   */
  sio_mdd_port_type             *mdd_port,  /* Port                        */
  sio_open_type                 *open_ptr   /* contains the new mode's info*/
)
{
  sio_stream_mode_type          old_mode;   /* the previous mode we used   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   ASSERT( mdd_port->gv.sio_stream_open_tx == TRUE);
   ASSERT( mdd_port->gv.open_stream_id == stream_id);
   ASSERT( open_ptr->rx_bitrate == open_ptr->tx_bitrate);
   
   if (open_ptr->stream_mode == mdd_port->gv.sio_current_stream_mode)
   {
     return;   /* do nothing, same mode.... */
   }
   /*-----------------------------------------------------------
    Based on the new mode, we will have to reset some
    vars, and then set some other vars.  We take each
    one within a switch statement.  We intlock, since
    so many state vars will change.
    
    We will first do the the assignments common to all
    modes.  Then we will proceed with any special initialization
    required by a mode.
   ------------------------------------------------------------*/
   UART_LOCK();
  
   /*-----------------------------------------------------
    Reset All of the variables upon mode change.  This
    is common for all types of modes.  We do the tx first,
    then the Rx vars.
   -----------------------------------------------------*/
   old_mode = mdd_port->gv.sio_current_stream_mode;
   mdd_port->gv.sio_current_stream_mode = open_ptr->stream_mode;

    /* ----------------------  TX vars  ------------------------------- */

   if (open_ptr->tx_queue != mdd_port->gv.sio_tx_watermark_queue_ptr)
   {
     if (mdd_port->gv.sio_tx_watermark_queue_ptr != NULL)
     {
       if (mdd_port->gv.sio_tx_watermark_queue_ptr->current_cnt > 0)
       {
         ERR("We are going to erase a non-empty watermark",0,0,0); 
       }
       dsm_empty_queue(mdd_port->gv.sio_tx_watermark_queue_ptr);
     }
     mdd_port->gv.sio_tx_watermark_queue_ptr = open_ptr->tx_queue;

     if (mdd_port->gv.sio_tx_current_wm_item_ptr != NULL)
     {
       dsm_free_packet( &(mdd_port->gv.sio_tx_current_wm_item_ptr));
     }
     
     /* There's no more data to be Tx'd, so disable Tx Intr */
     
     mdd_port->fp.disable_tx_interrupt ( mdd_port->pPortHead );

     mdd_port->gv.sio_bytes_to_tx          = 0;
   }

    /* ----------------------  RX vars  --------------------------------  */

   mdd_port->gv.sio_tail_char_used       = open_ptr->tail_char_used;
   mdd_port->gv.sio_tail_char            = open_ptr->tail_char;
   mdd_port->gv.sio_escape_code_func_ptr = NULL;
   mdd_port->gv.sio_packet_func_ptr      = NULL;
                                       
   if (open_ptr->rx_queue != mdd_port->gv.sio_rx_watermark_queue_ptr)
   {
     if (mdd_port->gv.sio_rx_watermark_queue_ptr != NULL)
     {
       if (mdd_port->gv.sio_rx_watermark_queue_ptr->current_cnt > 0)
       {
         ERR("We are going to erase a non-empty watermark",0,0,0); 
       }
       dsm_empty_queue(mdd_port->gv.sio_rx_watermark_queue_ptr);
     }
     mdd_port->gv.sio_rx_watermark_queue_ptr = open_ptr->rx_queue; 
   }

   mdd_port->gv.sio_rx_func_ptr            = open_ptr->rx_func_ptr;

 
   /*--------------------------------------------------------
      Some of the modes need special cleanup, so we do that
      here
   ---------------------------------------------------------*/
   switch(old_mode)
   {
     case SIO_DS_PKT_MODE:
       mdd_port->adv.ds_incall_device_id = UART_DEVICE_INVALID;
       mdd_port->adv.ds_incall_stream_id = SIO_NO_STREAM_ID;
       /* because the RXLEV may have been a special packet value */ 
       //siors_set_rxlev(mdd_port);
       break;

     case SIO_DS_RAWDATA_MODE:
       mdd_port->adv.ds_incall_device_id = UART_DEVICE_INVALID;
       mdd_port->adv.ds_incall_stream_id = SIO_NO_STREAM_ID;

       clk_dereg( &( mdd_port->gv.sio_esc_seq_cb_item));
       break;

     case SIO_DS_AUTODETECT_MODE:
       mdd_port->adv.autodetect_device_id = UART_DEVICE_INVALID;
       mdd_port->adv.autodetect_stream_id = SIO_NO_STREAM_ID;
       clk_dereg( &(mdd_port->adv.pausecheck_cb_item));
       break;

     default:
       break; 
   }

   /*--------------------------------------------------------
     If the mode we are changing to is packet or rawdata, then
     we want to make sure that this is the only stream doing
     that mode.  Ditto for autodetect.
   ---------------------------------------------------------*/
   if ( ( mdd_port->gv.sio_current_stream_mode == SIO_DS_RAWDATA_MODE ) ||
        ( mdd_port->gv.sio_current_stream_mode == SIO_DS_PKT_MODE))
   {
     /*--------------------------------------------------
       Check to insure that RAW_DATA mode is not already
       active on either UART. If so kill the phone as
       this is a pathological error that should never
       exit lab testing.
     --------------------------------------------------*/
     if( mdd_port->adv.ds_incall_device_id != UART_DEVICE_INVALID)
     {
       ERR_FATAL( "Multiple UARTS in 'data call' mode",0,0,0);
     }
   }
   else if (mdd_port->gv.sio_current_stream_mode == SIO_DS_AUTODETECT_MODE)
   {
     if( mdd_port->adv.autodetect_device_id != UART_DEVICE_INVALID)
     {
       ERR_FATAL( "Multiple UARTS in Autodetect mode",0,0,0);
     }
   }
   /*--------------------------------------------------------
     Now do any special setup required by the new mode. 
   ---------------------------------------------------------*/
   switch(open_ptr->stream_mode)
   {
#ifdef SIORS_HAS_AUTODETECT   //{
     case SIO_DS_RAWDATA_MODE:
       /* since connecting a call takes many seconds, we assume
        * that the first guardtime has already happened */
       mdd_port->gv.sio_esc_seq_state= SIO_ESC_SEQ_1ST_GUARDTIME;

       /* Now fall through - rest is the same as for PKT */

     case SIO_DS_PKT_MODE: /*lint !e825*/
       mdd_port->adv.ds_incall_device_id = mdd_port->device_id;
       mdd_port->adv.ds_incall_stream_id = stream_id;
       /* because the RXLEV may now need to be a special packet value */ 
       //siors_set_rxlev(rs_port);

       break;
   
     case SIO_DS_AUTODETECT_MODE:
       mdd_port->adv.autodetect_stream_id = stream_id;
       mdd_port->adv.autodetect_device_id = mdd_port->device_id;
       siors_mdd_setup_uart_autodetect( mdd_port );
       break;
#endif //}

     case SIO_DM_MODE:
       break;

     case SIO_GENERIC_MODE:
       break;

     default:
       ERR_FATAL("Unknown sio mode %d",open_ptr->stream_mode,0,0);
       break;
   }
   UART_UNLOCK();
} /*siors_mdd_change_mode*/


/*===========================================================================

FUNCTION SIORS_MDD_API_IOCTL                        EXTERNALIZED FUNCTION

DESCRIPTION
  Allows control of open stream, given that the device is an RS232 serial
  device.    
  
DEPENDENCIES
  None
                                                  
RETURN VALUE
  None
    
SIDE EFFECTS

===========================================================================*/
void siors_mdd_api_ioctl
(
  sio_stream_id_type    stream_id,          /* Stream ID                */
  sio_port_id_type      port_id,            /* SIO Port ID              */
  sio_ioctl_cmd_type    cmd,                /* Command                  */
  sio_ioctl_param_type *param               /* Parameter                */
)
{
  sio_mdd_port_type   *mdd_port;            /* Port                       */
  uart_device_id_type  device_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SIO_RS232_CHECK_PORT_EXIST(port_id); 
  device_id = uart_sio_port_id_to_device_id( port_id );
  mdd_port = &(siors_mdd_ports[device_id]);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch ( cmd)
  {
    case SIO_IOCTL_CHANGE_BAUD_RATE:
      siors_mdd_sched_baudrate_change(stream_id, 
              mdd_port, 
              (sio_bitrate_type)(param->bitrate));
      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    case SIO_IOCTL_CHANGE_BAUD_NOW:
      siors_mdd_baudrate_change_now( mdd_port, 
              (sio_bitrate_type)(param->bitrate));
      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_CHANGE_MODE:
      siors_mdd_change_mode(stream_id,
              mdd_port, 
              (sio_open_type *)param->open_ptr);
      break;
    
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    case SIO_IOCTL_CD_ASSERT:

      siors_mdd_set_dcd_output( mdd_port, TRUE );
      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_CD_DEASSERT:

      siors_mdd_set_dcd_output( mdd_port, FALSE );
      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_RI_ASSERT:

      //siors_set_ri_output(rs_port, TRUE);

      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_RI_DEASSERT:

      //siors_set_ri_output(rs_port, FALSE);

      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_ENABLE_DTR_EVENT:

      siors_mdd_enable_dtr_event( mdd_port, param );
      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_DISABLE_DTR_EVENT:

      siors_mdd_disable_dtr_event( mdd_port, param );
      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_INBOUND_FLOW_ENABLE:

      mdd_port->fv.sio_inbound_flow_enabled = TRUE;
      mdd_port->fp.enable_rx_hw_fctl ( mdd_port->pPortHead );

      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_INBOUND_FLOW_DISABLE:

      mdd_port->fv.sio_inbound_flow_enabled = FALSE;
      mdd_port->fp.disable_rx_hw_fctl ( mdd_port->pPortHead );

      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_DSR_ASSERT:

          /* Do Nothing */

      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_DSR_DEASSERT:

          /* Do Nothing */

      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_DTE_READY_ASSERTED:

      siors_mdd_check_dte_ready_asserted( mdd_port, param );
      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_SET_FLOW_CTL:

      siors_mdd_set_flow_ctl( stream_id,
                              mdd_port, 
                              param->flow_ctl.tx_flow, 
                              param->flow_ctl.rx_flow);
      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_GET_FLOW_CTL:

      siors_mdd_get_flow_ctl(mdd_port, param);

      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef SIORS_HAS_AUTODETECT  //{
    case SIO_IOCTL_ENABLE_AUTODETECT:
      /*---------------------------------------------------------------------
        The function below will re-arm Autobaud if the Autobaud Feature has
        been enabled at compile time and the Main port is being used for
        Autodetect mode. Otherwise the Autodetect processing will only
        enable for another AT command line
      ---------------------------------------------------------------------*/
      siors_mdd_setup_uart_autodetect( mdd_port );
      break; 
#endif  //} 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_RECORD_ESC_FUNC_PTR:

      if ( mdd_port->gv.sio_current_stream_mode != SIO_DS_RAWDATA_MODE)
      {
        ERR( "Escape code func. ptr recorded in mode != RawData",0,0,0);  
      }

      mdd_port->gv.sio_escape_code_func_ptr = param->record_escape_func_ptr;

      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_RECORD_PKT_FUNC_PTR:

      if ( mdd_port->gv.sio_current_stream_mode != SIO_DS_AUTODETECT_MODE)
      {
        ERR( "Packet func. ptr recorded in mode != AutoDetect",0,0,0);  
      }

      mdd_port->gv.sio_packet_func_ptr = param->record_pkt_func_ptr;

      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_FLUSH_TX:

      siors_mdd_api_flush_tx( stream_id, port_id, param->record_flush_func_ptr);

      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_BEGIN_FAX_IMAGE:

      ERR( "No FAX image support",0,0,0);  

      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_TX_START_BREAK:

      if(mdd_port->fp.set_tx_break_state != NULL)
      {
        mdd_port->fp.set_tx_break_state ( mdd_port->pPortHead, TRUE );
      }

      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_TX_END_BREAK:

      if(mdd_port->fp.set_tx_break_state != NULL)
      {
        mdd_port->fp.set_tx_break_state ( mdd_port->pPortHead, FALSE );
      }

      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    case SIO_IOCTL_GET_CURRENT_RTS:

      /*-----------------------------------------------------------------
        Read the current value of RTS/RFR from the PC (DTE) which is
        CTS on the MSM (DCE). Return TRUE if RTS is asserted (High).
      -----------------------------------------------------------------*/
      mdd_port->fp.get_current_rts ( mdd_port->pPortHead, param );
      break; 

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   
    case SIO_IOCTL_POWERDOWN:
      SIORS_TRACE_CODE(140, mdd_port->device_id);
#ifdef FEATURE_UART_POWERDOWN
      mdd_port->fp.close ( mdd_port->pPortHead );
      mdd_port->is_clock_enabled = FALSE;
      if( mdd_port->gv.can_wake_on_rx_event )
      {
         mdd_port->fp.detect_rx_activity( mdd_port->pPortHead );
      }
      sleep_assert_okts( mdd_port->gv.sleep_handle );
#endif
      break; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_WAKEUP:
      /* Waking up the UART 
       * 1. Reopen the port 
       * 2. Update flow control state 
       * 3. Vote not to sleep                                           * */
#ifdef FEATURE_UART_POWERDOWN
      sleep_negate_okts(mdd_port->gv.sleep_handle);
      mdd_port->fp.open ( mdd_port->pPortHead, QUICK );
      siors_mdd_update_flow_control( mdd_port );
      mdd_port->is_clock_enabled = TRUE;
#endif
      SIORS_TRACE_CODE(141, mdd_port->device_id);
      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_RX_HIGH_EVENT:
      if(param->rx_high_event_func_ptr != NULL)
      {
        mdd_port->fp.detect_rx_activity( mdd_port->pPortHead );
      }
      mdd_port->gv.rx_activity_cb_func_ptr = param->rx_high_event_func_ptr;
      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_CHECK_RX_STATE:
      *(param->rx_line_state) = 
         mdd_port->fp.check_rx_cable_status(mdd_port->pPortHead);
      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case SIO_IOCTL_GET_PACKET_SIZE:
    case SIO_IOCTL_SET_PACKET_SIZE:

      MSG_HIGH("Ignoring IOCTL to get/set packet size",0,0,0);
      break;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    default:
      ERR_FATAL( "Invalid SIO IOCTL command %d",cmd,0,0);
      break;
  } /* switch cmd */
}


/*===========================================================================

FUNCTION SIORS_MDD_RX_ACTIVITY

DESCRIPTION
  Called when the PDD layer reports a detection of the RX activity
  The function need to decide if wakeup is required, or RX activity
  callback is required

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_mdd_rx_activity
(
  sio_mdd_port_type             *mdd_port 
)
{
  SIORS_TRACE_CODE(144, mdd_port->device_id);
  
  /* If the port is not open, do not process any RX activity event */
  if(mdd_port->gv.sio_stream_open_rx == FALSE){
    return;
  }

  /* Decide if the port need to be re-opened */
  if( mdd_port->gv.can_wake_on_rx_event && !mdd_port->is_clock_enabled)
  {
    siors_mdd_rx_wakeup(mdd_port);
  }

  /* Generate a callback if one is required */
  if(mdd_port->gv.rx_activity_cb_func_ptr != NULL)
  {
    SIORS_TRACE_CODE(143, mdd_port->device_id);
    (mdd_port->gv.rx_activity_cb_func_ptr) ();
    mdd_port->gv.rx_activity_cb_func_ptr = NULL;
  }
}

/*===========================================================================

FUNCTION SIORS_MDD_CALLBACK_ENTRY

DESCRIPTION
  Entry point for all call back function coming from PDD layer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_mdd_callback_entry
(
    void *pHead,
    siors_mdd_callback_type intr_type
)
{
  sio_mdd_port_type *mdd_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch(intr_type)
  {
      case SIORS_TX_INTR:
          siors_mdd_tx_isr(mdd_port);
          break;
      case SIORS_TX_PKT_DONE:
          siors_mdd_tx_pkt_done(mdd_port);
          break;
      case SIORS_RXLEV_INTR:
          siors_mdd_rx_isr(mdd_port);
          break;
      case SIORS_RXSTALE_INTR:
          siors_mdd_rx_isr(mdd_port);
          break;
      case SIORS_RX_ACTIVITY:
          siors_mdd_rx_activity(mdd_port);
          break;
      default:
          ERR_FATAL( "Incorrect MDD callback type", 0, 0, 0);
          break;
  }
}


/*===========================================================================

FUNCTION SIORS_MDD_DSM_NEW_BUFFER_NULL_CLEANUP

DESCRIPTION
  Cleans up when dsm_new_buffer() returns a NULL meaning that no buufer
  is available. Flushes Rx FIFO and sets up for recovery.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void siors_mdd_dsm_new_buffer_null_cleanup
(
    sio_mdd_port_type   *mdd_port
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!mdd_port->is_clock_enabled)  return;

  mdd_port->fp.reset_rx ( mdd_port->pPortHead );
  MSG_HIGH("SIO DSM new buffer unavailable!, device_id: %x", mdd_port->device_id, 0, 0);
  mdd_port->fp.enable_rx ( mdd_port->pPortHead );
  if(mdd_port->fv.sio_inbound_flow_enabled)
  {
    mdd_port->fp.enable_rx_hw_fctl ( mdd_port->pPortHead );
  }
} 


/*===========================================================================

FUNCTION SIORS_MDD_SET_PACKET_MODE

DESCRIPTION
  Check the condition to see if it is possible to set up packet mode
  operation.  Physical device driver may respond to the packt mode
  variable by increase the size of the packet in each transfer.

DEPENDENCIES
  UART is running and enabled.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void siors_mdd_set_packet_mode
(
    sio_mdd_port_type   *mdd_port
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  mdd_port->gv.sio_uart_packet_mode = TRUE;

  if ( mdd_port->device_id != UART_DEVICE_1 ) {
    mdd_port->gv.sio_uart_packet_mode = FALSE;
  }

  if ( mdd_port->gv.sio_current_stream_mode != SIO_DS_PKT_MODE ) {
    mdd_port->gv.sio_uart_packet_mode = FALSE;
  }
  
  if ( (mdd_port->fv.sio_rx_flow_control_method != SIO_CTSRFR_FCTL) &&
       (mdd_port->fv.sio_rx_flow_control_method != SIO_CTSRFR_AUTO_FCTL) ) {
    mdd_port->gv.sio_uart_packet_mode = FALSE;
  }

  if ( (mdd_port->fv.sio_tx_flow_control_method != SIO_CTSRFR_FCTL) &&
       (mdd_port->fv.sio_tx_flow_control_method != SIO_CTSRFR_AUTO_FCTL) ) {
    mdd_port->gv.sio_uart_packet_mode = FALSE;
  }
}

/*===========================================================================

FUNCTION SIORS_MDD_SET_DCD_OUTPUT                           INTERNAL FUNCTION

DESCRIPTION
  This procedure assert/deassert the DCD line

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_set_dcd_output
(
    sio_mdd_port_type  *mdd_port,                  /* Port                       */
    boolean             CDC_value
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
)
{
    switch (mdd_port->device_id)
    {
        case UART_DEVICE_1:
          /*-----------------------------------------------------------------
           Set/reset the GPIO used for DCD.
          -----------------------------------------------------------------*/
#ifndef SIORS_UART1_USES_4WIRE_ONLY //{
          if(CDC_value == TRUE)
          {
              BIO_OUT( BIO_DCD_REG, BIO_DCD_M, BIO_DCD_ON_V);
          }
          else
          {
              BIO_OUT( BIO_DCD_REG, BIO_DCD_M, BIO_DCD_OFF_V);
          }
#endif //} 
          break;

        default:
          break;
    } 
}

/*===========================================================================

FUNCTION SIORS_MDD_DTR_EVENT_ISR                                  INTERNAL FUNCTION

DESCRIPTION
  This function performs Interrupt processing for the DTR interrupt
  processing on the MSM UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_dtr_event_isr( void)
{
  sio_mdd_port_type *mdd_port;                  /* Port                       */
  mdd_port = &(siors_mdd_ports[UART_DEVICE_1]); 

#ifdef FEATURE_REX_PREEMPT_INTS
  /*-------------------------------------------------------------------------
    If pre-emptive interrupts are installed, then need to re-enable the
    unexpected clk interrupt.
  -------------------------------------------------------------------------*/
  clk_unexpected_enable();
#endif

  /*------------------------------------------------------------------------
    When PNP is defined, we want to know about any DTR changes.
    Otherwise, we only care if (siors_dtr_func_ptr != NULL).
  ------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------
    DTR monitoring is enabled.  Read DTR and then set DMOD to interrupt on
    the opposite state/polarity.  Signal the task who set up this interupt.
    It is a mistake to compare with BIO_DTR_ON_V, since DMOD_OUTM uses
    different constants than BIO_INM, and those constants are not
    conditionally compiled for each target.  This will cause problems for
    targets with Active low DTR signals.
  -----------------------------------------------------------------------*/
  if( BIO_INM( BIO_DTR_REG, BIO_DTR_M) == BIO_DTR_M)  
  {
    /* DTR now High, Set to interrupt when it goes Low */
#ifdef FEATURE_SIO_DTR_HIGH_GPIO
    gpio_int_set_handler((gpio_int_type)BIO_DTR_GPIO_INT, ACTIVE_LOW, siors_mdd_dtr_event_isr); /*lint !e534*/
#else
    HWIO_OUTM( INT_POLARITY_2, DMOD_GP_I_POL2_M, DMOD_GP_I_POL2_INV_V);
#endif
  }
  else 
  {
    /* DTR now Low, Set to interrupt when it goes Hi */
#ifdef FEATURE_SIO_DTR_HIGH_GPIO
    gpio_int_set_handler((gpio_int_type)BIO_DTR_GPIO_INT, ACTIVE_HIGH, siors_mdd_dtr_event_isr); /*lint !e534*/
#else
    HWIO_OUTM( INT_POLARITY_2, DMOD_GP_I_POL2_M, DMOD_GP_I_POL2_V);
#endif
  }

  /* Call any task with interest in DTR */
  if (mdd_port->gv.siors_dtr_func_ptr)
  {
    (*mdd_port->gv.siors_dtr_func_ptr)();
  }
} /* siors_mdd_dtr_event_isr */

/*===========================================================================

FUNCTION SIORS_ENABLE_DTR_EVENT                        INTERNAL FUNCTION

DESCRIPTION
  This procedure enable the process of firing a call back when DTR event
  is detected

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_enable_dtr_event
(
    sio_mdd_port_type *mdd_port,               /* Port                     */
    sio_ioctl_param_type *param                /* Parameter                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
)
{
      switch (mdd_port->device_id)
      {
        case UART_DEVICE_1:

#ifndef SIORS_UART1_USES_4WIRE_ONLY //{
          /*-----------------------------------------------------------------
           Store the Signal mask and TCB pointer of calling task.
          -----------------------------------------------------------------*/

          mdd_port->gv.siors_dtr_func_ptr = param->enable_dte_ready_event;
    
          /*-----------------------------------------------------------------
           If PNP is defined, DTR monitoring is continuous 
          -----------------------------------------------------------------*/
          /*-----------------------------------------------------------------
           DTR monitoring is enabled.  Read DTR and then set DMOD to 
           interrupt on the opposite state/polarity.  Signal the task who set
           up this interupt.  It is a mistake to compare with BIO_DTR_ON_V, 
           since DMOD_OUTM uses different constants than BIO_INM, and those 
           constants are not conditionally compiled for each target.  This 
           will cause problems for targets with Active low DTR signals.
          -----------------------------------------------------------------*/
          if( BIO_INM( BIO_DTR_REG, BIO_DTR_M) == BIO_DTR_M)  
          {
            /* DTR now High, Set to interrupt when it goes Low */

 #ifdef FEATURE_SIO_DTR_HIGH_GPIO
            gpio_int_set_handler((gpio_int_type)BIO_DTR_GPIO_INT, ACTIVE_LOW, siors_mdd_dtr_event_isr); /*lint !e534*/
 #else
            HWIO_OUTM( INT_POLARITY_2, DMOD_GP_I_POL2_M, DMOD_GP_I_POL2_INV_V);
 #endif
          }
          else
          {
            /* DTR now Low, Set to interrupt when it goes Hi */

 #ifdef FEATURE_SIO_DTR_HIGH_GPIO
            gpio_int_set_handler((gpio_int_type)BIO_DTR_GPIO_INT, ACTIVE_HIGH, siors_mdd_dtr_event_isr); /*lint !e534*/
 #else
            HWIO_OUTM( INT_POLARITY_2, DMOD_GP_I_POL2_M, DMOD_GP_I_POL2_V);
 #endif
          }
    
 #ifndef FEATURE_SIO_DTR_HIGH_GPIO
          tramp_set_isr( TRAMP_GPIO_INT_2_ISR, siors_mdd_dtr_event_isr);
 #endif  


#endif  //}
          break;

        default:
          break;
      } 
}

/*===========================================================================

FUNCTION SIORS_MDD_DISABLE_DTR_EVENT                       INTERNAL FUNCTION

DESCRIPTION
  This procedure disable the process of firing a call back when DTR event
  is detected

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_disable_dtr_event
(
    sio_mdd_port_type *mdd_port,              /* Port                     */
    sio_ioctl_param_type *param               /* Parameter                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
)
{
    switch (mdd_port->device_id)
    {
        case UART_DEVICE_1:

#ifndef SIORS_UART1_USES_4WIRE_ONLY //{
          /*-----------------------------------------------------------------
           Reset flag to signify no task DTR monitoring underway.
          -----------------------------------------------------------------*/
          mdd_port->gv.siors_dtr_func_ptr = NULL;

#ifdef FEATURE_SIO_DTR_HIGH_GPIO
          gpio_int_set_handler((gpio_int_type)BIO_DTR_GPIO_INT, ACTIVE_HIGH, NULL); /*lint !e534*/
#else
          tramp_set_isr( TRAMP_GPIO_INT_2_ISR, NULL);
#endif  

#endif  //}
          break;

        default:
          break;
    } 
}

/*===========================================================================

FUNCTION SIORS_MDD_CHECK_DTE_READY_ASSERTED                INTERNAL FUNCTION

DESCRIPTION
  This procedure check to see if the DTE signal is asserted by the host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_mdd_check_dte_ready_asserted
(
    sio_mdd_port_type *mdd_port,               /* Port                     */
    sio_ioctl_param_type *param                /* Parameter                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
)
{
    switch (mdd_port->device_id)
    {
        case UART_DEVICE_1:
#ifndef SIORS_UART1_USES_4WIRE_ONLY //{
          /*-----------------------------------------------------------------
           Read DTR (interrupt) pin and return TRUE if Asserted.
           (Note that the port value is the inverse of the pin for ISS2.)
          -----------------------------------------------------------------*/
          if( BIO_INM( BIO_DTR_REG, BIO_DTR_M) == BIO_DTR_ON_V)  
          {
            *param->dte_ready_asserted = TRUE;
          }
          else
          {
            *param->dte_ready_asserted = FALSE;
          }

#ifdef FEATURE_SIO_DTR_ACTIVE_LOW
          *param->dte_ready_asserted = !(*param->dte_ready_asserted);
#endif

#endif  //}
          break;

        default:
          break;
    } 
}

#endif //} 


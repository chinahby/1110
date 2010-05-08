/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             S I O R S 2 3 2  M D D  D A T A
                             
            SIO Media Dependent Driver layer for RS-232 communication

GENERAL DESCRIPTION

  This module contain the code shared by different serial driver.  It calls
  seperate hardware dependent driver code to access the actual UART hardware.
  

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  sio_rs232_dev_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 1999-2000, 2007-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_mdd_data.c#2 $

when         who       what, where, why
-----------  --------  ------------------------------------------------------
21-Jul-2008  cmihalik  Initial revision.

===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "siors232_mdd_defs.h"
#include "siors232_mdd_data.h"
#include "sio_wrapper.h"

//  If there is is at least one UART.
#if( defined(FEATURE_FIRST_UART)  || \
     defined(FEATURE_SECOND_UART) || \
     defined(FEATURE_THIRD_UART))              //{

/*===========================================================================

                       FUNCTIONs, ISRs and MACROs

===========================================================================*/

void             siors_mdd_dsm_new_buffer_null_cleanup( sio_mdd_port_type * );
static void      siors_mdd_ad_setup_discard_recovery( sio_mdd_port_type* );
void             siors_mdd_setup_uart_autodetect ( sio_mdd_port_type* );

/*===========================================================================

FUNCTION SIORS_MDD_ESC_SEQ_GUARDTIME_ISR

DESCRIPTION
  This function will monitor the incoming serial RX stream to determine if
  the Escape Sequence is being received.  At the first guard-time, before
  the Escape characters, the guardtime is long enough that the Modem's fifo
  will be completely drained.  So, the first character that will be seen in
  the fifo will be a Escape character.  When this routine is invoke after
  the last Escape character, the fifo should be completely empty.

  This routine is also invoked after the first and second Escape characters.
  This is because the escape sequence should be presented to the modem in a
  finite amount of time and because the guartime after the first or the
  second escape character may be the beginning of the true escape sequence.

DEPENDENCIES
  This function must only be called while in RAWDATA or PKT modes, and must
  only be invoked from a Clock Services callback function.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void siors_mdd_esc_seq_guardtime_isr(int32 time_ms, timer_cb_data_type data)
{
  sio_mdd_port_type *mdd_port;
  mdd_port = (sio_mdd_port_type *)data;  

  /*-------------------------------------------------------------------------
   Use the Escape Sequence state to begin checking the progress of the
   Escape Sequence.
  -------------------------------------------------------------------------*/
  switch ( mdd_port->gv.sio_esc_seq_state )
  {
    case SIO_ESC_SEQ_NULL:

      mdd_port->gv.sio_esc_seq_state = SIO_ESC_SEQ_1ST_GUARDTIME;
      break;

    case SIO_ESC_SEQ_1ST_GUARDTIME:
    case SIO_ESC_SEQ_1ST_CHAR:
    case SIO_ESC_SEQ_2ND_CHAR:

      //  Don't care.
      break;

    case SIO_ESC_SEQ_3RD_CHAR:

      /*---------------------------------------------------------------------
       Post- escape bytes guardtime has expired. If no bytes received then
       set Escape Sequence signal to task.
      ---------------------------------------------------------------------*/

      /*-------------------------------------------------------------------
       Escape sequence received. Signal Task.
      -------------------------------------------------------------------*/
      ASSERT( mdd_port->gv.sio_escape_code_func_ptr != NULL );
      (mdd_port->gv.sio_escape_code_func_ptr)();

      mdd_port->gv.sio_esc_seq_state = SIO_ESC_SEQ_NULL;

      timer_reg( &(mdd_port->gv.sio_esc_seq_cb_item),
               (timer_t2_cb_type)siors_mdd_esc_seq_guardtime_isr,
               (timer_cb_data_type)mdd_port,
               (int4)SIO_ESC_GUARD_TIME,
               (int4)SIO_ESC_GUARD_TIME);
      break;

    default:
      ERR_FATAL( "Invalid Callback Invocation", 0, 0, 0);
      break;
  } /* switch sio_esc_seq_state */
}

/*===========================================================================

FUNCTION SIORS_MDD_RX_HRD_FCTL_RDATA_NESC_ISR

DESCRIPTION
  This is the RX ISR for RawData mode with hardware flow control and no 
  escape code detection is required.  Escape code detection is not needed if 
  guard-time has not expired, which means that only guard-time needs checking 
  and not the character by character comparision.

DEPENDENCIES
  Should only be called from siors_rx_isr.  Flow control method should be
  hardware flow control.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void siors_mdd_rx_rdata_nesc_isr
(
  sio_mdd_port_type   *mdd_port
)
{
  dsm_item_type *item_ptr;               /* DSM item which contains packet */
  uint32         pkt_len;                /* Length of packet               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED( "Rawdata ISR #4\n",0,0,0);
  do
  {
    /*---------------------------------------------------------------
     dsm_new_buffer no longer ERR_FATAL's when there are no more items.
    ---------------------------------------------------------------*/
    item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL);

    if ( item_ptr == NULL ) /* we couldn't get one. */
    {
      siors_mdd_dsm_new_buffer_null_cleanup( mdd_port );
      return;
    }

    /* ------------------------------------------------------------------
     * Until we run out of chars or run out of buffer . . .
     * We keep a copy of the rdy status for consistency.  Otherwise
     * after we enqueue there could be new bytes and we create lots of
     * buffers with only one or two bytes. That hurts performance.
     *------------------------------------------------------------------- */

    pkt_len = item_ptr->size;
    mdd_port->fp.receive ( mdd_port->pPortHead, item_ptr->data_ptr, &pkt_len );
    SIORS_TRACE_RCVD_CHARS( item_ptr->data_ptr, pkt_len, mdd_port->port_id );

    /*---------------------------------------------------------------
     Schedule a guard time check ISR ( for '+++' escape code
     detection).
    ---------------------------------------------------------------*/

    mdd_port->gv.sio_esc_seq_state = SIO_ESC_SEQ_NULL;

    timer_reg( &(mdd_port->gv.sio_esc_seq_cb_item),
             (timer_t2_cb_type)siors_mdd_esc_seq_guardtime_isr,
             (timer_cb_data_type)mdd_port,
             (int4)SIO_ESC_GUARD_TIME,
             (int4)SIO_ESC_GUARD_TIME);

    /*-----------------------------------------------------------------
     Whether we have a full buffer or we're out of fifo data, we enqueue it.
    -----------------------------------------------------------------
     The following is a sanity check on DSM_DS_SMALL_ITEM_SIZ.
    ---------------------------------------------------------------*/
    ASSERT( pkt_len <= DSM_DS_SMALL_ITEM_SIZ );

    item_ptr->used = pkt_len;
    item_ptr->kind = SIO_PAYLOAD;

    /* If receive function returns zero byte, there is no more data to read */
    if (0 == pkt_len)
    {
      dsm_free_buffer( item_ptr );
      break;
    }

    if (mdd_port->gv.sio_rx_func_ptr)
    {
      (mdd_port->gv.sio_rx_func_ptr)( &item_ptr);
    }
    else if (mdd_port->gv.sio_rx_watermark_queue_ptr)
    {
      dsm_simple_enqueue_isr( mdd_port->gv.sio_rx_watermark_queue_ptr,&item_ptr);
    }
    else
    { 
      dsm_free_buffer( item_ptr); /*lint !e534*/
    }

  } while ( item_ptr->used == item_ptr->size );
} /* siors_mdd_rx_rdata_nesc_isr */

/*===========================================================================

FUNCTION SIORS_MDD_RX_HRD_FCTL_RDATA_ESC_ISR

DESCRIPTION
  This is the RX ISR for RawData mode with hardware flow control and escape 
  code detection is required.  Escape code detection is needed if guard-time 
  has expired, which means that character by character comparision is required.

DEPENDENCIES
  Should only be called from siors_rx_isr.  Flow control method should be
  hardware flow control.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void siors_mdd_rx_rdata_esc_isr
(
  sio_mdd_port_type *mdd_port           /* Port identifier                 */
)
{
  dsm_item_type *item_ptr;              /* DSM item which contains packet  */
  byte           rx_char;               /* Received character              */
  uint32         pkt_len;               /* Number of input bytes to read   */
  byte          *ptr;                   /* ptr into dsm item buffer        */
  byte          *stop;                  /* one past end of buffer          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED( "Rawdata ISR #2\n",0,0,0);
  do
  {
    /*---------------------------------------------------------------
     dsm_new_buffer no longer ERR_FATAL's when there are no more items.
    ---------------------------------------------------------------*/
    item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL);

    if ( item_ptr == NULL ) /* we couldn't get one. */
    {
      siors_mdd_dsm_new_buffer_null_cleanup( mdd_port );
      return;
    }

    /* ------------------------------------------------------------------
     * Until we run out of chars or run out of buffer . . .
     * We keep a copy of the rdy status for consistency.  Otherwise
     * after we enqueue there could be new bytes and we create lots of
     * buffers with only one or two bytes. That hurts performance.
     *------------------------------------------------------------------- */

    pkt_len = item_ptr->size;
    mdd_port->fp.receive ( mdd_port->pPortHead, item_ptr->data_ptr, &pkt_len );
    SIORS_TRACE_RCVD_CHARS( item_ptr->data_ptr, pkt_len, mdd_port->port_id );

    /*---------------------------------------------------------
     * If the state is SIO_ESC_SEQ_NULL at any time during buffer
     * processing, it will be SIO_ESC_SEQ_NULL at the end of 
     * buffer processing.  Stop buffer processing whenever the
     * state becomes SIO_ESC_SEQ_NULL.
    ---------------------------------------------------------*/

    ptr  = item_ptr->data_ptr;
    stop = ptr + pkt_len;
    while(( ptr < stop ) && ( mdd_port->gv.sio_esc_seq_state != SIO_ESC_SEQ_NULL ))
    {
      rx_char = *ptr++;

      switch (mdd_port->gv.sio_esc_seq_state)
      {
        case SIO_ESC_SEQ_NULL:
          //  Nothing to do, but we shouldn't get here anyway.  
          break;

        /*---------------------------------------------------------
         Expecting to get an SIO_ESC_CHAR here.  If we get one,
         keep going.  Otherwise, go to NULL state.
         ---------------------------------------------------------*/

        case SIO_ESC_SEQ_1ST_GUARDTIME:
        case SIO_ESC_SEQ_1ST_CHAR:
        case SIO_ESC_SEQ_2ND_CHAR:

          if (SIO_ESC_CHAR == rx_char)
          {
            mdd_port->gv.sio_esc_seq_state++; /*lint !e641*/ 
          }
          else
          {
            mdd_port->gv.sio_esc_seq_state = SIO_ESC_SEQ_NULL;
          }
          break;

        /*---------------------------------------------------------
         This means that a third escape character was read and then
         another character (escape character or not) was read before
         the guard time.  Otherwise, if the rx fifo is empty, an 
         RXSTALE interrupt would not have happened
         ---------------------------------------------------------*/

        case SIO_ESC_SEQ_3RD_CHAR:

          mdd_port->gv.sio_esc_seq_state = SIO_ESC_SEQ_NULL;
          break;

        /*---------------------------------------------------------
         Not sure if this could happen.
         --------------------------------------------------------*/

        case SIO_ESC_SEQ_DETECTED:

          mdd_port->gv.sio_esc_seq_state = SIO_ESC_SEQ_NULL;
          break;

        default:
          break;
      }
    }

    /*---------------------------------------------------------------
     Schedule a guard time check (if needed).
    ---------------------------------------------------------------*/

    switch (mdd_port->gv.sio_esc_seq_state)
    {
      /*---------------------------------------------------------------
       Schedule a guard time check ISR ( for '+++' escape code
       detection).
      ---------------------------------------------------------------*/

      case SIO_ESC_SEQ_NULL:

        timer_reg( &(mdd_port->gv.sio_esc_seq_cb_item),
                   (timer_t2_cb_type) siors_mdd_esc_seq_guardtime_isr,
                   (timer_cb_data_type) mdd_port,
                   (int4)SIO_ESC_GUARD_TIME,
                   (int4)SIO_ESC_GUARD_TIME);
        break;

      /*---------------------------------------------------------------
       Schedule a guard time check ISR for guard time following escape
       sequence.
      ---------------------------------------------------------------*/

      case SIO_ESC_SEQ_3RD_CHAR:

        timer_reg( &(mdd_port->gv.sio_esc_seq_cb_item),
                   (timer_t2_cb_type) siors_mdd_esc_seq_guardtime_isr,
                   (timer_cb_data_type) mdd_port,
                   (int4)SIO_ESC_GUARD_TIME,
                   (int4)0 );
        break;

      /*---------------------------------------------------------------
       How to handle the case where the time between escape characters
       is greater than the guard time?  e.g. 10 seconds, ESC_CHAR, 
       10 seconds, ESC_CHAR.  Is the second 10 seconds the beginning 
       of a new escape sequence or the continuation of the escape sequence 
       started with the first 10 seconds?  I say do nothing.  There 
       can be any amount of time between the escape characters.  If this
       is not correct, schedule a guard time check here.
      ---------------------------------------------------------------*/

      default:
        break;
    }

    /*-----------------------------------------------------------------
     Whether we have a full buffer or we're out of fifo data, we enqueue it.
    -----------------------------------------------------------------
     The following is a sanity check on DSM_DS_SMALL_ITEM_SIZ.
    ---------------------------------------------------------------*/

    ASSERT( pkt_len < item_ptr->size );
    item_ptr->used = pkt_len;
    item_ptr->kind = SIO_PAYLOAD;

    if (mdd_port->gv.sio_rx_func_ptr)
    {
      (mdd_port->gv.sio_rx_func_ptr)( &item_ptr);
    }
    else if (mdd_port->gv.sio_rx_watermark_queue_ptr)
    {
      dsm_simple_enqueue_isr( mdd_port->gv.sio_rx_watermark_queue_ptr,&item_ptr);
    }
    else
    { 
      dsm_free_buffer( item_ptr); /*lint !e534*/
    }
  } 
  while( pkt_len > 0 );

} /* siors_mdd_rx_rdata_esc_isr */

#ifdef SIORS_HAS_AUTODETECT  //{

/*===========================================================================

FUNCTION SIORS_MDD_SETUP_UART_AUTODETECT

DESCRIPTION
   Setup the UART for Autodetect processing. This function is called for 
   every new AT cmd line. 

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void siors_mdd_setup_uart_autodetect
(
    sio_mdd_port_type     *mdd_port          /* Port                          */
)
{
  INTLOCK();
  /*-------------------------------------------------------------------
    Set UART interrupt mask conditions for Autodetect mode. 
    Leave bit rate as already set. 
  ---------------------------------------------------------------------*/


  /*-----------------------------------------------------------------------
    Set Autodetect mode pause variables to default states then register 
    clock callback to set check for Pre-pause interval (delay before bytes 
    arrive)
  -----------------------------------------------------------------------*/
  mdd_port->adv.packetpause_occurred = FALSE;
  mdd_port->adv.state                = SIO_AD_INIT;
  INTFREE();
} /* siors_mdd_setup_uart_autodetect */

/*===========================================================================

FUNCTION SIORS_MDD_AD_DISCARD_RECOVERY_ISR

DESCRIPTION
  This function is a callback ISR that will determine if the data services 
  serial port has stopped receiving bytes and if so then this function will
  re-initialize the port for data services Autodetect mode operation. If the
  port is still receiving bytes then this callback will register another
  invocation to check again later.

DEPENDENCIES
  - This function must be run in ISR context to insure no context switches
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void siors_mdd_ad_discard_recovery_isr
(
  sio_mdd_port_type     *mdd_port                /* Port                         */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   
  if (0 == mdd_port->adv.rx_cnt)
  {
    /*-------------------------------------------------------------
      The line is now Pausing. Setup for Autodetect mode processing
    -------------------------------------------------------------*/
    MSG_MED( "Ending SIO Discard",0,0,0);
    siors_mdd_setup_uart_autodetect(mdd_port);
  }
  else /* RX bytes still rolling in */
  {
    /*-------------------------------------------------------------
      The line is still active. Store the saved count again and
      re-register this callback to check again after the pause
      interval expires
    -------------------------------------------------------------*/
    siors_mdd_ad_setup_discard_recovery( mdd_port );
  }
} 

/*===========================================================================

FUNCTION SIORS_MDD_AD_SETUP_DISCARD_RECOVERY

DESCRIPTION
  This function will setup for the Autodetect mode RX ISR to recover from 
  Discard mode. A callback will be setup to monitor the RX line. Once the RX 
  line goes stale (no bytes received) for the the pre-pause interval then the
  callback will prepare the serial port for Autodetect processing.
  
  Autodetect mode is restricted to the main UART channel.

DEPENDENCIES
  This function must be run in ISR context to insure no context switches
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void siors_mdd_ad_setup_discard_recovery
(
  sio_mdd_port_type     *mdd_port                /* Port                         */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */   
  /*------------------------------------------------------------------
    If for some reason we make it here, but the stream is not in 
    AUTODETECT throw an error message and return out. 
  ------------------------------------------------------------------*/
  if(!mdd_port->adv.is_in_autodetect_mode)
  {
    MSG_ERROR("AD discard while not in AD mode", 0,0,0);
    return;
  }
  /*------------------------------------------------------------------
    Save away the current RX count for the callback to check later, set
    RXLEV up high to lighten CPU load during Discarding (but low enough 
    so that RX interrupts will occur at least once between each discard
    callback, then register callback that will re-initialize Autodetect 
    mode once the line pauses.
  ------------------------------------------------------------------*/
  if( mdd_port->adv.state != SIO_AD_DISCARD)
  {
    MSG_HIGH( "SIO Discard Active",0,0,0);
  }
  mdd_port->adv.state  = SIO_AD_DISCARD;
  mdd_port->adv.rx_cnt = 0;

  clk_reg2(&(mdd_port->adv.pausecheck_cb_item), 
          siors_mdd_ad_discard_recovery_isr,
          (int4)DISCARD_CHECK_INTERVAL, 
          (int4)0, 
          FALSE,
          mdd_port);        

} /* siors_mdd_ad_setup_discard_recovery */

/*===========================================================================

FUNCTION SIORS_MDD_AD_PACKETPAUSE_CHECK_ISR

DESCRIPTION
  This function is a callback ISR that set a semaphore that indicates that
  no bytes have been received since the callback was registered. This is used
  for Packet detection in Autodetect mode.
  
  Autodetect mode is restricted to the main UART channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void siors_mdd_ad_packetpause_check_isr
(
  sio_mdd_port_type     *mdd_port               /* Port                       */
)
{
  /*------------------------------------------------------------------
    Set Semaphore indicating that the Packet Pause interval has elapsed
  ------------------------------------------------------------------*/
  mdd_port->adv.packetpause_occurred = TRUE;
} 

/*===========================================================================

FUNCTION SIORS_MDD_NEW_VALID_PACKET

DESCRIPTION
  This procedure will signal packet detection to the registered task and
  setup the associated Autodetect mode UART for Packet data flow operation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void siors_mdd_new_valid_packet
(
  sio_mdd_port_type     *mdd_port               /* Port                       */
)
{
  /*-------------------------------------------------------------------------
   Valid packet incoming. Set the UART for Packet mode processing, and signal 
   task, if registered.  
  -------------------------------------------------------------------------*/
  mdd_port->adv.state = SIO_AD_INIT;   /* reset for next time */
  
  if( mdd_port->gv.sio_packet_func_ptr != NULL)
  {
    ( mdd_port->gv.sio_packet_func_ptr)();
  }
} 

/*===========================================================================

FUNCTION SIORS_MDD_RX_AUTODETECT_ISR

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void siors_mdd_rx_autodetect_isr
( 
  sio_mdd_port_type*           mdd_port 
)
{
  dsm_item_type *item_ptr;                   /* DSM item which contains packet */
  byte           rx_char;                    /* Received character             */
  uint32         pkt_len;                    /* Number of input bytes to read  */
  byte          *ptr;
  byte          *stop;
  boolean        done;                       /* Done with current buffer       */
  int            num_chars;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*---------------------------------------------------------------
     dsm_new_buffer no longer ERR_FATAL's when there are no more items.
    ---------------------------------------------------------------*/
    item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL);

    if (item_ptr == NULL) /* we couldn't get one. */
    {
      siors_mdd_dsm_new_buffer_null_cleanup( mdd_port );
      return;
    }

    pkt_len = item_ptr->size;
    mdd_port->fp.receive ( mdd_port->pPortHead, item_ptr->data_ptr, &pkt_len );
    SIORS_TRACE_RCVD_CHARS( item_ptr->data_ptr, pkt_len, mdd_port->port_id );

    ptr  = item_ptr->data_ptr;
    stop = ptr + pkt_len; 
    done = FALSE;

    while(( ptr < stop ) && !done )
    {
      rx_char = *ptr;

      mdd_port->adv.rx_cnt++;

      switch (mdd_port->adv.state)
      {
        /*-----------------------------------------------------------------
         No characters received yet.
        -----------------------------------------------------------------*/

        case SIO_AD_INIT:

          switch (rx_char)
          {
            case 'a':
            case 'A':

              mdd_port->adv.state = SIO_AD_GOT_A;
              item_ptr->used++;
              break;

            case '\r':
            case '\n':

              //  Ignore \r, \n between AT commands.
              break;

            case SIO_ASYNC_FLAG:

              /*-----------------------------------------------------
               Received an SIO_ASYNC_FLAG after the pre-pause interval,
               which means that this could be the beginning of a packet
               -----------------------------------------------------*/

              num_chars = stop - ptr;
              if (num_chars > 0)
              {
                /*---------------------------------------------------
                 There is at least 1 more byte in the RX fifo, 
                 indicating that a valid start of packet has been rx'd.
                 ---------------------------------------------------*/
                siors_mdd_new_valid_packet( mdd_port );
              }
              else
              {
                /*---------------------------------------------------
                 Flag byte received, but no more bytes now available.
                 Register the 2 ms packet detection clock callback 
                 that will set a Pause semaphore if the delay period 
                 is reached
                ---------------------------------------------------*/

                mdd_port->adv.state = SIO_AD_GOT_ASYNC_FLAG;

                mdd_port->adv.packetpause_occurred = FALSE;

                clk_reg2( &(mdd_port->adv.packetpause_cb_item), 
                          siors_mdd_ad_packetpause_check_isr,
                          (int4)SIO_PACKET_DETECTION_TIMEOUT,
                          (int4)0, 
                          FALSE,
                          mdd_port);
              }
              break;

            default:

              item_ptr->used = 0;
              siors_mdd_ad_setup_discard_recovery( mdd_port );
              break;
          }
          break;

        /*-----------------------------------------------------------------
         Already have an 'a' or 'A', the beginning of an AT command.
        -----------------------------------------------------------------*/

        case SIO_AD_GOT_A:

          switch (rx_char)
          {
            case 'a':
            case 'A':

              //  Do nothing.  Multiple a's OK.
              item_ptr->used++;
              break;

            case '/':
            case 't':
            case 'T':

              mdd_port->adv.state = SIO_AD_GOT_AT;
              item_ptr->used++;
              break;

            default:

              item_ptr->used = 0;
              siors_mdd_ad_setup_discard_recovery( mdd_port );
              break;
          }
          break;

        /*-----------------------------------------------------------------
         Got AT command.
        -----------------------------------------------------------------*/

        case SIO_AD_GOT_AT:

          //  Mark this char, and all the rest of the chars, as used.
          num_chars = stop - ptr;
          item_ptr->used += num_chars;
          done = TRUE;
          break;

        /*-----------------------------------------------------------------
         Got SIO_ASYNC_FLAG.  This should only happen if the SIO_ASYNC_FLAG
         was the last character in the buffer.  (If it wasn't, we could have
         processed it already.)
        -----------------------------------------------------------------*/

        case SIO_AD_GOT_ASYNC_FLAG:

          if (FALSE == mdd_port->adv.packetpause_occurred)
          {
            //  packetpause timer didn't expire.
            clk_dereg( &(mdd_port->adv.packetpause_cb_item));
            siors_mdd_new_valid_packet( mdd_port );
          }
          else
          {
            item_ptr->used = 0;
            siors_mdd_ad_setup_discard_recovery( mdd_port );
          }
          break;

        /*-----------------------------------------------------------------
         Discarding.
        -----------------------------------------------------------------*/

        case SIO_AD_DISCARD:

          done = TRUE;
          break;

        /*-----------------------------------------------------------------
         Shouldn't happen.
        -----------------------------------------------------------------*/

        default:
          break;
      }

      ptr++;
    }

    if (SIO_AD_DISCARD == mdd_port->adv.state)
    {
      //  Discard all characters in current buffer.
      item_ptr->used = 0;
    }

    /*-----------------------------------------------------------------
     Whether we have a full buffer or we're out of fifo data, we enqueue it.
    -----------------------------------------------------------------
     The following is a sanity check on DSM_DS_SMALL_ITEM_SIZ.
    ---------------------------------------------------------------*/

    ASSERT( item_ptr->used <= DSM_DS_SMALL_ITEM_SIZ );
    item_ptr->kind = SIO_PAYLOAD;

    /* No valid data, free the buffer. */
    if (0 == item_ptr->used)
    {
      dsm_free_buffer( item_ptr );
    }
    else if (mdd_port->gv.sio_rx_func_ptr)
    {
      (mdd_port->gv.sio_rx_func_ptr)( &item_ptr);
    }
    else if (mdd_port->gv.sio_rx_watermark_queue_ptr)
    {
      dsm_simple_enqueue_isr( mdd_port->gv.sio_rx_watermark_queue_ptr,&item_ptr);
    }
    else
    { 
      dsm_free_buffer( item_ptr); /*lint !e534*/
    }
  }
  while( pkt_len > 0 );

} /* siors_mdd_rx_hrd_autodetect_isr */

#endif //} 

#endif //} 


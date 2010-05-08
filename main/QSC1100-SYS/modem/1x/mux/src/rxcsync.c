/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 C D M A    S Y N C    M O D U L E

GENERAL DESCRIPTION
  Contains items associated with the sync channel

EXTERNALIZED FUNCTIONS
  rxc_sc_isr - interrupt handler for the Decoder interrupt used
  when the Receive task is processing the Sync Channel
  rxc_sc_init - initializes the Sync channel processing state
  rxc_sc - Sync channel processing state

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1990 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxcsync.c_v   1.15   11 Oct 2002 12:53:08   sanjayr  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcsync.c#3 $ $DateTime: 2008/08/18 13:56:52 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/08   jtm     Klockwork fixes and added debug messages.
07/16/08   jtm     Reduced debug string sizes for ULC effort.
06/09/06   trc     Remove T_MSM6700 featurization
09/08/05   ejv     Use new srch_mux interface calls.
07/25/05   rkc     Fixed lint errors.
07/15/05   bn      Make sure to clear the Dec status bit after install the decoder synch isr
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
02/24/05   vlc     Lock interrupt between clearing DEC_DONE_INT_STATUS and
                   installing RXC ISR.
07/29/04   bn,rz   Clear the DEC_DONE_INT_STATUS on power up
06/24/04   sr      Added support for 2nd level dec_done interrupt in ISR
04/20/04   jrp     Fixed lint warnings.
10/22/03   sr      Fixed compilation warning errors when using RVCT compiler
06/16/03   bn      Fixed the "out of buffer dec_free_q" problem.
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
03/05/03   fc      Consolidated f-csh fade timer implementation.
10/11/02   sr      Initialized rxc_isr_q in rxc_sc_init
08/06/02   cr      Added MSM6100 changes.
08/13/02   sr      Set slotted_status to AWAKE before entering BCCH state
08/13/02   sr      Added call to rxcbcch_set_first_frame_ind() in rxc_sc()
                   under REL_A_CC
02/27/02   lad     Merged in IS2000 Release A Common Channels.
11/16/01   vlc     Added call to dec_clk_select () before switching rxc state.
06/15/01   day     Delete FEATURE_SPECIAL_MDR
04/20/01   sr      Merged in from MSM5100 archive
04/19/01   tc      Merged MSM_MUX1X.01.00.01.
           fc      Renamed mcc_report_event_msg_rxed to
                   mclog_report_event_msg_rxed.
03/27/01   vlc     Merged MSM_MUX1X.00.00.17.
           fc      Added support of events report: Message received.
03/01/01   tc      Merged MSM_MUX1X.00.00.15.
02/21/01   sr      Fixes for paging channel loss problem in rxc_sc_init()
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime switch.
01/18/01   ejv     Modified some combiner macros to match new names.
01/12/01   tc      Merged MSM_MUX1X.00.00.06.
11/23/00   tc      Fixed MSM5100 PLT ftm_sc_msg_sent.
12/07/00   bgc     Commented out some unused PLT code.
11/13/00   bgc     Moved FTM sync variables to here from common.
11/10/00   jc      This file was part of rxc.c. See rxccommon.c for full
                   change history

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <memory.h>
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "rex.h"
#include "bit.h"
#include "queue.h"
#include "task.h"
#include "err.h"
#include "log.h"
#include "crc.h"
#include "enc.h"
#include "cmd.h"
#include "rxc.h"
#include "mccdma.h"
#include "rxtx.h"
#include "msg.h"
#include "tramp.h"
#include "mccrx.h"
#include "ts.h"
#include "cai.h"
#include "caix.h"
#include "rx.h"
#include "tx.h"
#include "mccrxtx.h"
#include "srch_mux.h"
#include "bio.h"

#include "deint.h"
#include "msm_drv.h"
#include "atmr.h"

#if  defined (FEATURE_IS95B_MDR)
  #include "mccscm.h"             /* supplemental channel processing routines */
#endif /* FEATURE_IS95B_MDR */


#ifdef TMOBILE
#error code not present
#endif

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

#ifdef FEATURE_FACTORY_TESTMODE
  /* need some txc buffers to do loopback */
  #include "txc.h"

  boolean ftm_sc_msg_sent = FALSE;
  /* Flag indicating if a message has been sent to PLT_MCC. The message
     notifies PLT_MCC that RXC has received a Sync Channel message */

#endif

#include "rxccommon.h"
#include "rxctraffic.h"
#include "rxcsync.h"
#include "rxcrda.h"
#include "rxcdemux.h"
#include "rxcpaging.h"
#include "rxcidle.h"
#ifdef FEATURE_IS2000_REL_A_CC
#include "rxcbcch.h"
#endif

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif
#include "mclog.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Number of bytes in a Sync Channel subframe */
#define SC_SF_BYTES   ((DEC_96_BITS / 8) / 3 )

/* Number of bits in a Sync Channel subframe */
#define SC_SF_BITS    (SC_SF_BYTES * 8)

/* DEBUG CRC statistics */

/* number of Sync Channel messages with good CRCs */
dword rxc_sc_crc_good = 0;

/* number of Sync Channel messages with bad CRCs */
dword rxc_sc_crc_bad;

/*-------------------------------------------------------------------------*/
/* States of Sync Channel parsing state machine                            */
/*-------------------------------------------------------------------------*/
typedef enum
{
  SC_HUNT_STATE,   /* Hunt for start of message */
  SC_MSG_STATE,    /* Collect bits of message */
  SC_FLUSH_STATE   /* Discard remaining bits in subframe */
}rxc_sc_state_type;

/*-------------------------------------------------------------------------*/
/* Sync Channel State variables                                            */
/*-------------------------------------------------------------------------*/

struct
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*         TASK VARIABLES                                                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  byte            sync_blk[ DEC_96_BITS / 8 ];
    /* Sync channel frame read from decoder */
  rxc_sc_state_type  state;
    /* State of parsing state machine */
  word  msg_inx; /* index into message buffer */
  word  msg_len; /* number of bits in message rounded to 93 */
  byte  sav_inx; /* subframe index where start of message bit was
                     found -- passed back to the Main control task */
  rxtx_rx_msg_type *msg_ptr;
    /* pointer to buffer to place received message for Layer 2 task */

} rxc_sc_var;


/* Symbol error rates returned from decoder */

dec_sc_status_type rxc_sc_status = {0, 0, 0}; /* Status(CRC + QUALITY), ser, energy */
rxc_sc_ser_type rxc_sc_ser_total = {0, 0};


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION RXC_SC_ISR

DESCRIPTION
  This function is the interrupt handler for the Decoder interrupt used
  when the Receive task is processing the Sync Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  rxc_sc_isr ( void )
{
  rxc_dec_type *buf_ptr;
    /* pointer to buffer to be placed on rxc_dec_q */
  rxc_isr_cmd_type *cmd_ptr;
    /* pointer to command received from the task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

  uint32 dec_int_status;


  dec_int_status = HWIO_IN(DEC_DONE_INT_STATUS);
  HWIO_OUT(DEC_DONE_INT_STATUS, dec_int_status);
  if(dec_int_status != 0x2)
  {
    MSG_ERROR("DEC Done Int, but not SVD! %d", dec_int_status, 0, 0);
    return;
  }

  /* increment interrupt number and frame number */
  qw_inc( rxc_dec_frame, 1L );

  /* Check to see if there are pending commands for the interrupt handler */
  if( (cmd_ptr = (rxc_isr_cmd_type *)q_get(&rxc_isr_q) ) != NULL )
  {
    switch( cmd_ptr->cmd )
    {

      case RXC_SC_F:
      {
        MSG_MED("Got RXC_SC_F", 0, 0, 0);
        /* initialize sync mode */
        enc_set_mode( ENC_SC_MODE );
        break;
      }

      default:
      {
        MSG_FATAL( "Bad cmd %d", cmd_ptr->cmd, 0, 0  );
      }
    } /* switch */
    /* return the memnory back to the free queue */
    q_put (&rxc_isr_free_q, &cmd_ptr->link);
    (void)rex_set_sigs ( &rx_tcb, RXC_ISR_CMD_SIG );
  }
  else
  {
    /* Process Decoder data */
/*-------------------------------------------------------------------------*/

    /* Read the status information for Sync Channel */
    dec_read_sc_status(&rxc_sc_status);
    rxc_sc_ser_accum();


    /* Peform normal interrupt processing */
    if ( (buf_ptr = (rxc_dec_type *)q_get( &rxc_dec_free_q)) == NULL )
    {
      ERR( "0 bufs on dec_free_q", 0, 0, 0  );
    }

    else
    {

/*********************** READ SYNC CHANNEL DECODED BITS **************/
        dec_read_sc_data( buf_ptr->buf.sc_buf);

          /* get data from decoder */
        buf_ptr->state = RXC_SC_STATE;
          /* indicate data is from Sync Channel */
        q_put( &rxc_dec_q, &buf_ptr->link );
          /* put data on queue for task */

      (void)rex_set_sigs ( &rx_tcb, RXC_INT_SIG );
        /* signal task to indicate data is on queue */
    }

  } /* if */

#ifdef FEATURE_AGC_LOGGING
  /* perform AGC/power control logging */
  srch_mux_log_agc_pctl();
#endif

} /* rxc_sc_isr */



/*===========================================================================

FUNCTION RXC_LOG_SC_MSG

DESCRIPTION
  This function logs messages received on the Sync Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  rxc_log_sc_msg
(
  qword ts,                  /* time stamp when message was received */
  cai_sc_ext_type *msg_ptr   /* pointer to received message */
)
{
#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
  log_sc_msg_type *log_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr && msg_ptr->length != 0)
  {
    log_ptr = (log_sc_msg_type *)
                log_alloc (LOG_SC_MSG_C,
                           msg_ptr->length + sizeof(log_ptr->hdr));
  }
  else
  {
    log_ptr = NULL;
  }

  if (log_ptr == NULL)
  {
    rxc_log_msg_drop++;
  }
  else
  {
    /* Record timestamp from actual SC generation */
    qw_equ( log_ptr->hdr.ts, ts );

    /* copy message into buffer */
    (void)memcpy( (void *)(log_ptr->msg),
                  (void *)&(msg_ptr->length),
                   msg_ptr->length);  //lint !e613 msg_ptr has been checked non-null

    log_commit((log_type*) log_ptr);
  }
#else
  rxc_log_msg_type *log_ptr;   /* pointer to buffer to log msg in */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get a buffer off of the rxc_log_msg_q to log the received message */
  if( (log_ptr = (rxc_log_msg_type *) q_get(&rxc_log_msg_q) ) == NULL )
  {
    rxc_log_msg_drop++;
  }
  else
  {
    /* set up log buffer */
    log_ptr->entry.sc_msg.hdr.len = msg_ptr->length +
                                    sizeof( log_hdr_type );

       /* Mark that message is from Sync Channel */
    log_ptr->entry.sc_msg.hdr.code = LOG_SC_MSG_C;
      /* put timestamp into buffer */
    qw_equ( log_ptr->entry.sc_msg.hdr.ts, ts );

    /* copy message into buffer */
    (void)memcpy( (void *)(log_ptr->entry.sc_msg.msg),
                  (void *)&(msg_ptr->length),
                   msg_ptr->length);

    log_put( &log_ptr->desc );     /* log message */
  }
#endif /* !defined(FEATURE_DIAG_V7_LOG_IFACE) */

  /* Event report: Message received */
  if (msg_ptr)
  {
    mclog_report_event_msg_rxed(CAI_SYNCH, msg_ptr->body);
  }
} /* rxc_log_sc_msg */


/*===========================================================================

FUNCTION RXC_SC_PARSE

DESCRIPTION
  This function collects the bits of a Sync Channel frame by parsing
  Sync Channel frames.  When it determines that an entire Sync Channel
  message has been received, and that the message CRC passes, it will place
  the message on a queue for the Layer 2 task and signal the Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_sc_parse
(
  byte                  sf_inx,     /* Subframe number */
  byte                  sframe[],   /* Address of 1st byte of subframe */
  word                  sframe_len  /* Number of bits in subframe */
)
{

  word  sframe_inx;      /* index into subframe */
  word  num_bits;        /* Number of bits to be copied to the message
                            buffer */
  static qword ts;       /* timestamp to use when logging message */
  mccrxtx_cmd_type *msg_ptr;
    /* pointer to buffer to place translated message in */
  word status;
    /* status returned by caix_ext_to_int */
  word msg_length;
    /* message length to send to caix_ext_to_int */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( sframe[0] & 0x80 )
  {
    /* Start of message bit found -- start parsing new message
       regardless of current state */
    rxc_sc_var.state = SC_HUNT_STATE;
  }

  sframe_inx = 1;  /* Skip over 1st bit (SOM). */

  while ( sframe_inx < sframe_len )  /* While still more bits. */
  {

    switch( rxc_sc_var.state )
    {
      case  SC_HUNT_STATE:
      {
        if( sframe[0] & 0x80 )
        {

          rxc_sc_var.msg_inx = 0;

          if ( ( (rxc_sc_var.msg_len =
                  (word)(b_unpackw ( sframe, sframe_inx, 8 ) * 8)) > 0 ) &&
                 (rxc_sc_var.msg_len <= ( CAI_SC_EXT_SIZE * 8 ) ) )
          {
            /* adjust message length so that an even number of
               80 ms will be processed (80 ms = 93 bits ) */
            rxc_sc_var.msg_len = (word)( ( (rxc_sc_var.msg_len + 92)/93 ) * 93);

            if  ( (rxc_sc_var.msg_ptr == NULL ) &&
                ( (rxc_sc_var.msg_ptr =
                    (rxtx_rx_msg_type *)q_get( &rxtx_rx_free_q) ) == NULL ) )
            {

              ERR( "0 bufs on rxtx_rx_free_q", 0, 0, 0 );
              /* out of buffers -- skip this frame and start looking
                for a new start of message */
              rxc_sc_var.state = SC_FLUSH_STATE;

            }
            else
            {
              /* start filling message buffer with length byte */
              b_copy ( sframe, sframe_inx,
                       (void *)(&rxc_sc_var.msg_ptr->msg.sc.length),
                       rxc_sc_var.msg_inx, 8 );
              /* copy length byte into message buffer */
              rxc_sc_var.msg_inx += 8;
              /* increment index into message buffer */
              sframe_inx += 8;
              /* increment index into subframe */
              rxc_sc_var.sav_inx = sf_inx;
                /* save subframe start of message was found in */

              rxc_sc_var.state = SC_MSG_STATE;

              ts_get( ts );  /* save timestamp for logging message */

            }
          }
          else
          { /* message length = 0 */
            rxc_sc_var.state = SC_FLUSH_STATE;
          }

        }

        else
        {       /* start of message bit not found */
          rxc_sc_var.state = SC_FLUSH_STATE;
        }

        break;
      }

      case SC_MSG_STATE:
      {
        num_bits = MIN ( (rxc_sc_var.msg_len - rxc_sc_var.msg_inx),
                         (sframe_len - sframe_inx) );
        /* calculate number of bits to copy into message buffer */

        b_copy ( sframe, sframe_inx, &rxc_sc_var.msg_ptr->msg.sc.length,
                  rxc_sc_var.msg_inx, num_bits );

        sframe_inx += num_bits;
        /* increment index into subframe */

        rxc_sc_var.msg_inx += num_bits;
        /* increment index into message buffer */

        if ( rxc_sc_var.msg_inx >= rxc_sc_var.msg_len )
        {

          /* entire message collected  -- calculate CRC */
          if ( (rxc_sc_var.msg_ptr != NULL) &&
               ( (crc_30_calc (&rxc_sc_var.msg_ptr->msg.sc.length,
                 (rxc_sc_var.msg_ptr->msg.sc.length * 8 ))) == CRC_30_OK))
          {

            rxc_sc_crc_good++;

            /* log message */
            rxc_log_sc_msg( ts, &rxc_sc_var.msg_ptr->msg.sc );

            /* translate message to put on queue for MC */
            if((msg_ptr = (mccrxtx_cmd_type *)q_get(&mcc_rxtx_free_q)) == NULL)
            {
              ERR( "0 bufs on mc_rxtx_free_q", 0, 0, 0 );
            }
            else
            {
              msg_length = (rxc_sc_var.msg_ptr->msg.sc.length * 8 ) -
                             CAI_SC_CRC_SIZE - CAI_SC_LGTH_SIZE;
              if ( (status = caix_ext_to_int(msg_length, CAIX_SC,
                    &rxc_sc_var.msg_ptr->msg, &msg_ptr->msg.msg ))
                    != CAIX_DONE_S )
              {
                MSG_MED("Bad SC transl: status %d",status,0,0 );
                /* Put item back on to free q */
                q_put( &mcc_rxtx_free_q, &msg_ptr->msg.hdr.cmd_hdr.link );
              }
              else
              {

#ifdef FEATURE_FACTORY_TESTMODE
                if ((ftm_mode == FTM_MODE) && ( !ftm_sc_msg_sent ))
                {
                  MSG_HIGH( "RXC got Sync Ch msg", 0, 0, 0 );
                }

                if ((ftm_mode == DMSS_MODE) || ( !ftm_sc_msg_sent ))
                {
#endif /* FEATURE_FACTORY_TESTMODE */
                  /* put the rest of the information on the queue for MC */
                  msg_ptr->msg.hdr.command = MCCRXTX_MSG_F;
                  msg_ptr->msg.chn = RXC_CDMA_SC;
                    /* message is from sync channel */
                  msg_ptr->msg.frm.sframe = rxc_sc_var.sav_inx;

                  msg_ptr->msg.hdr.cmd_hdr.done_q_ptr = &mcc_rxtx_free_q;
                  msg_ptr->msg.hdr.cmd_hdr.task_ptr = NULL;

                  /* Put message on queue for MC */
                  mccrxtx_cmd( msg_ptr );
#ifdef FEATURE_FACTORY_TESTMODE
                  ftm_sc_msg_sent = TRUE;
                }
                else
                {
                   /* Put item back on to free q */
                   q_put( &mcc_rxtx_free_q, &msg_ptr->msg.hdr.cmd_hdr.link );
                }

                /* For FTM, RXC should continue decoding Sync Channel frames
                   until it is told to go to some other state */

#else /* FEATURE_FACTORY_TESTMODE */
                /* Normal (non-PLT) operation */
                /* If not in sounder mode */
                if (!BIO_GET_SW( BIO_SW7_M ))
                {
                  /* turn off DEC interrupts */
                  rxc_clear_isr();
                }
#endif /* FEATURE_FACTORY_TESTMODE */
              }
            }
          }
          else
          {
            rxc_sc_crc_bad++;   /* bad CRC */

          }
          rxc_sc_var.state = SC_FLUSH_STATE;
          /* the buffer pointed to by rxc_sc_var will be filled up with the
             next message */
        }

        break;
      }

      case SC_FLUSH_STATE:
      {
        sframe_inx = sframe_len; /* Indicate all bits used up. */
        rxc_sc_var.state = SC_HUNT_STATE;

        break;
      }

      default:
      {
        ERR_FATAL("Bad sc parse subst %d",rxc_sc_var.state,0,0 );
      }

    } /* switch */
  } /* while */

} /* rxc_sc_parse */


/*===========================================================================

FUNCTION RXC_SC_DATA

DESCRIPTION
  This procedure processes the bits received from the decoder while the
  Receive task is processing the Sync Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  rxc_sc_data ( void )
{
  byte sub_inx;
    /* index of subframe */
  rxc_dec_type *buf_ptr;
    /* pointer to buffer received from the rxc_dec_q */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void)rex_clr_sigs( rex_self(), RXC_INT_SIG );
  while( (buf_ptr = (rxc_dec_type *)q_get( &rxc_dec_q ) ) != NULL )
  {
    if( buf_ptr->state == RXC_SC_STATE )
    {
      /* buffer is a Sync Channel buffer */
      /* Note that there is a 72 bit delay in the decoder so that we
         need to reassemble the frame by combining the first 72 bits
         of the current frame with the last 24 bits of the previous frame */
      b_copy ( buf_ptr->buf.sc_buf, 0, rxc_sc_var.sync_blk, 24, 72 );

      /* Copy remaining portion of current frame to buffer. */
      for ( sub_inx = 0; sub_inx < 3; sub_inx++ )
      {
        /* Send bits to Sync Channel message processor */
        rxc_sc_parse ( sub_inx,
                      &rxc_sc_var.sync_blk[ sub_inx * ((DEC_96_BITS/8)/3 )],
                      SC_SF_BITS);
      }
      b_copy ( buf_ptr->buf.sc_buf, 72, rxc_sc_var.sync_blk, 0, 24 );
        /* Copy first piece of next frame to buffer. */
    }

    q_put( &rxc_dec_free_q, &buf_ptr->link );
    (void)rex_clr_sigs( rex_self(), RXC_INT_SIG );
  }

} /* rxc_sc_data */



/*===========================================================================

FUNCTION RXC_SC_INIT

DESCRIPTION
  This funtion initializes the Sync channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_sc_init
(
  word curr_state
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSM_LOCK( );
  DEMOD_DISABLE_Q_CHANNEL( );  /* Disable Q channel for Sync Channel */
  COMBINER_SELECT_CH0_256_CHIP_SYMBOL( );  /* Tell the symbol combiner to
                                       accumulate symbols over 256 pn chips */

  DEMOD_SET_CH0_TD_SIGN( 0x0 );
  DEMOD_DISABLE_CH0_RC4( );
  MSM_UNLOCK( );
  switch( curr_state )
  {
#ifdef FEATURE_FACTORY_TESTMODE
    case RXC_SC_STATE:
      if(ftm_mode == DMSS_MODE)
      {
        ERR_FATAL("Bad state %d", curr_state, 0, 0 );
      }
      /* For FTM_MODE, fall through */
#endif /* FEATURE_FACTORY_TESTMODE */
    case RXC_CDMA_STATE:
    case RXC_PC_STATE:
#ifdef FEATURE_IS2000_REL_A_CC
    case RXC_BCCH_STATE:
    case RXC_FCCCH_STATE:
#endif
    case RXC_TC_STATE:
    {
      /* initialize variables for Sync Channel parsing state machine */
      rxc_sc_var.state = SC_HUNT_STATE;

      /* initialize sync channel crc count variables */
      rxc_sc_crc_good = 0;
      rxc_sc_crc_bad = 0;

      /* initialize ser totals */
      rxc_ser_reset();

      /* Must initialize MOD_MISC_CTL to sync channel prior to
      installing ISR.  This will then enable decoder to give a DEC_INT which
      can then be used for Sync Channel Initialization
      */
      enc_set_mode(ENC_SC_MODE);

      /* Decoder clk is CHIPX8 in sync state */
      dec_clk_select (DEC_CLK_CHIPX8);

      /* clear the frame offset */
      rxc_set_frame_offset(0);

      INTLOCK ();
      /* register interrupt for Sync Channel data processing */
      tramp_set_isr( TRAMP_RX_ISR, rxc_sc_isr );
      /* Clear DEC DONE int status to make sure ISR is properly serviced.
      */
      HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
      INTFREE ();

      deint_set_sc_mode();  /* Set deint/dec to sync mode */

      /* Clear any unprocessed commands on the ISR queue.
       * This covers the case when an ISR command, say, RXC_WAKE_F,
       * has been queued up, but before it can be finished, i.e., before
       * SRCH reinstalls the decoder ISR, SRCH fails to reacquire after
       * sleep. In this case the command lingers on the queue and the
       * next ISR command (probably  the one below) is going to try to
       * queue up rxc_isr_cmd_buf again resulting in a messed up rxc_isr_q
       * Need to free all the lingering item on the queue
      */
      rxc_clear_isr_cmd_queue();

      rxc_isr_cmd_buf_ptr = (rxc_isr_cmd_type *) q_get (&rxc_isr_free_q);
      if (rxc_isr_cmd_buf_ptr != NULL)
      {
        rxc_isr_cmd_buf_ptr->cmd = RXC_SC_F;
        /* send command to interrupt handler to initialize SC mode */
        rxc_isr_cmd(rxc_isr_cmd_buf_ptr);
      }
      else
      {
        ERR( "0 bufs on isr_free_q", 0, 0, 0 );
      }
      break;
    }

    default:
    {
      ERR_FATAL("Bad state %d", curr_state, 0, 0 );
    }
  } /* switch */

} /* rxc_sc_init */



/*===========================================================================

FUNCTION RXC_SC_EXIT

DESCRIPTION
  This funtion performs processing necessary to end the Sync channel state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_sc_exit( void )
{
#ifdef FEATURE_FACTORY_TESTMODE
  ftm_sc_msg_sent = FALSE;
    /* Reset the flag */
#endif /* FEATURE_FACTORY_TESTMODE */

  /* Put pending messages back on the free q */
  if (rxc_sc_var.msg_ptr != NULL)
  {
    q_put(&rxtx_rx_free_q, &rxc_sc_var.msg_ptr->link );
    rxc_sc_var.msg_ptr = NULL ;
  }
} /* rxc_sc_exit */



/*===========================================================================

FUNCTION RXC_SC

DESCRIPTION
  Sync channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Next state to be processed.

===========================================================================*/

word rxc_sc ( void )
{

  rxc_state_type next_state;
    /* next state to be processed */
  rxc_cmd_msg_type *cmd_ptr;
    /* pointer to command buffer received on rxc_cmd_q */
  boolean success = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

  next_state = RXC_SC_STATE;
  while (next_state == RXC_SC_STATE)
  {
    /* check for received interrupts */
    rxc_sc_data();

    /* Check for received messages */
    (void)rex_clr_sigs( rex_self(), RXC_CMD_Q_SIG );

    cmd_ptr = (rxc_cmd_msg_type *) q_get (&rxc_cmd_q);

    if (cmd_ptr != NULL)
    {
      switch (cmd_ptr->hdr.command)
      {
        case RXC_EXIT_F:
        {
          MSG_MED("Got RXC_EXIT_F", 0, 0, 0);
          cmd_ptr->hdr.status = RXC_DONE_S;
          next_state = RXC_EXIT_STATE;
          break;
        }

        case RXC_IDLE_F:
        {
          MSG_MED("Got RXC_IDLE_F", 0, 0, 0);
          rxcidle_init();
#ifdef FEATURE_IS2000_REL_A_CC
// TODO: Review: intentionally ignoring cmd_ptr->idle.frame since no
// channel actually needs the info (they all get frame number in their ISRs).
          next_state = rxcidle_chan_config (&cmd_ptr->idle.chan_info);
          /* this should call after inititialized funtion finishing its
          execution, so that the atmr won't mess it up */
          rxcidle_set_slotted_status( RXCIDLE_SLOTTED_AWAKE );
#else
          if (cmd_ptr->idle.chan_info.chan_id == CAI_PCH)
          {
            rxc_pc_init (RXC_SC_STATE,
              cmd_ptr->idle.chan_info.config.pch.rate);
            next_state = RXC_PC_STATE;
          }
#endif

          if (next_state != RXC_SC_STATE)
          {
            cmd_ptr->hdr.status = RXC_DONE_S;
          }
          else
          {
            success = FALSE;
          }

          break;
        }

        case RXC_PC_F:
        {
          MSG_MED("Got RXC_PC_F", 0, 0, 0);
          /* begin Paging Channel processing */
          rxc_pc_init( RXC_SC_STATE, cmd_ptr->pc.rate);
          cmd_ptr->hdr.status = RXC_DONE_S;
          next_state = RXC_PC_STATE;
          break;
        }

        case RXC_CDMA_F:
        {
          MSG_MED("Got RXC_CDMA_F", 0, 0, 0);
          /* --------------------------------------------------
          ** Transition back to CDMA init state without exiting
          ** rxc_subtask()
          ** -------------------------------------------------- */

          /* --------------------------------------
          ** Clean up before switching to CDMA mode
          ** -------------------------------------- */
          rxc_exit();

          cmd_ptr->hdr.status = RXC_DONE_S;
          next_state = RXC_CDMA_STATE;
          break;
        }

#ifdef FEATURE_FACTORY_TESTMODE
        case RXC_SC_F:
        {
          MSG_MED("Got RXC_SC_F", 0, 0, 0);
          if ( ftm_mode == FTM_MODE )
          {
            /* --------------------------------------
            ** Clean up before switching to CDMA mode
            ** -------------------------------------- */

            cmd_ptr->hdr.status = RXC_DONE_S;

            /* Place item on queue if requested */
            cmd_done( &cmd_ptr->hdr.cmd_hdr );

            (void)rex_clr_sigs( rex_self(), RXC_CMD_Q_SIG );

            /* Perform exit processing */
            rxc_sc_exit();

            rxc_sc_init( next_state );
            /* Initialize Sync Channel processing state. We will return
               to this state */

            return( next_state );
          }
          /* Fall through for mode == DMSS */
        }
#endif /* FEATURE_FACTORY_TESTMODE */

        default:
        {
          success = FALSE;
        }
      } /* switch */

      if (!success) {
#ifdef FEATURE_FACTORY_TESTMODE
          if(ftm_mode == FTM_MODE)
          {
          MSG_HIGH( "Bad RXC cmd %d", cmd_ptr->hdr.command ,0, 0 );
          } else {
            ERR_FATAL( "Bad cmd %d", cmd_ptr->hdr.command, 0, 0  );
          }
#else
          ERR_FATAL( "Bad cmd %d", cmd_ptr->hdr.command, 0, 0  );
#endif /* FEATURE_FACTORY_TESTMODE */
          /* illegal command */
      }

      /* Place item on queue if requested */
      cmd_done( &cmd_ptr->hdr.cmd_hdr );

      (void)rex_clr_sigs( rex_self(), RXC_CMD_Q_SIG );
    }
    else
    {

      (void)rxc_wait( RXC_CMD_Q_SIG | RXC_INT_SIG );
        /* wait for a command or for an interupt */
    }
  } /* while */

  /* Perform exit processing */
  rxc_sc_exit();

  return( next_state );

} /* rxc_sc */


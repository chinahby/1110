/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 

              CDMA2000 Common Control Channel Monitoring

GENERAL DESCRIPTION
  Contains implementation of Physical, LAC and MAC layer of IS2000 release
  New Common Control Channel (NCCCH) operation.

  
EXTERNALIZED FUNCTIONS
  rxcnccch_isr   - NCC decoder ISR
  rxcnccch_state - NCC state processing
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath:                                                                  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcnccch.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/15/08   jtm     Added debug messages to cmd processing states.
07/16/08   jtm     Reduced debug strings and removed unnecessary if/else in
                   rxcnccch_state() for ULC effort.
05/29/07   vlc     Fixed compiler warnings for rel 0 only build.
05/03/07   cab     Moved to new sleep voting interface
06/12/06   awj     #include tramp.h; no longer included through clk.h
06/09/06   trc     Remove T_MSM6700 featurization
05/23/06   trc     Add NCCCH failsafe timer support
05/08/06   bn      Added logic to support the case if receiving slot_mask = 0 in rude wakeup
03/22/06   kwo     Added calls into srch to report frame stats
09/08/05   ejv     Use new srch_mux interface calls.
09/07/05   rkc     Mainline FEATURE_ZZ2.
08/29/05   bn      Clear the all the decoder status it after installing the isr
08/11/05   bn      Added logic to relay the page match event to MC
05/04/05   kwo     srch_idle_page_match() -> srch_mux_page_match()
03/14/05   bn      Instead of SRCH, MUX need to install the ISR after WAKE_F
02/24/05   vlc     Lock interrupt between clearing DEC_DONE_INT_STATUS and
                   installing RXC ISR.
02/23/05   vlc     Clear the DEC_DONE_INT_STATUS when setting RXC ISR.
07/29/04   jyw     Added Interrupt cleaning to rxcnccch_isr
04/20/04   jrp     Fixed lint warnings.
04/19/04   jrp     Changed srch_stm.h to stm.h.
12/03/04   bn      Featurize the logic to turn off BCCH in SLEEP. 
                   FEATURE_IS2000_REL_A_CC_BCCH_ON_IN_SLEEP is not defined by default
09/03/04   bn      Do not delete FCCCH deint task when transition to TCI or SC state
03/03/04   bn      Fixed the SMS address matched logic for back-to-back SMS
01/23/04   bn      Reading the decoder when in idle HO
12/11/03   bn      No need to check for FCCCH frame CRC. Let the msg CRC do the job
11/20/03   fc      Fixed featurization of quick page match.
11/17/03   fc      Added quick page match.
11/04/03   bn      Improved the 5ms delay on FCCCH
10/09/03   sfm     Updated for new state machine paradigm
09/23/03   bn      Fixed the corner case in the secondary BCCH skip logic
09/09/03   dna     Merged porting changes for MSM6500
06/27/03   bn      Flush the FCCCH log info when going to sleep
06/16/03   bn      Fixed the "out of buffer dec_free_q" problem. Also fixed the
                   hard-handoff problem when switching from NCCCH to PCH
06/10/03   bn      Improving the skipping logic so that it will be more specific
06/06/03   bn      Used the correct operator to toggled the channel bits mask
05/30/03   bn      Fixed compiler warning inventing: extern int srch_idle_page_match()
05/14/03   bn      Support frame crc statistic for simultanous FCCCH/BCCH in PLT
05/05/03   bn      Optimized the NCCCH logics and F3 debug messages
02/07/03   bn,sr   Created the new file to handle all the NCC channels

===========================================================================*/
#include "customer.h"
#include "target.h"
#include "comdef.h"

#ifdef FEATURE_IS2000_REL_A_CC_SMMC

#include "assert.h"
#include "err.h"
#include "log.h"
#include "log_codes.h"
#include "msg.h"
#include "queue.h"
#include "tramp.h"

#include "cai.h"
#ifdef FEATURE_IS2000_REL_A_CC_QPM
#include "mccdma.h"
#endif /* FEATURE_IS2000_REL_A_CC_QPM */
#include "mux_logtypes.h"
#include "rx.h"
#include "rxc.h"
#include "dec5000.h"
#include "deint.h"
#include "msm_drv.h"

#include "rxccommon.h"
#include "rxcfccch.h"
#include "rxcbcch.h"
#include "rxcidle.h"
#include "rxcmsg.h"
#include "rxcsar.h"
#include "rxcsync.h"
#include "rxctraffic.h"
#include "rxcnccch.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif
#include "ts.h"

#include "srch_mux.h"
#include "stm.h"

#define RXCNCCH_BCCH_DECODER_INT    0x10
#define RXCNCCH_FCCCH_DECODER_INT   0x00

/* extern variable */
extern boolean rxc_read_bcch_data_from_decoder;
extern boolean rxc_read_fccch_data_from_decoder;
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
extern boolean rxc_bcbcch_first_frame;
extern boolean rxc_bcbcch_peek_sms_msadress_done;
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
extern rxcfccch_info_type rxcfccch_info;
extern rxcbcch_info_type rxcbcch_info;
extern rxcsar_msg_desc_type rxcfccch_sar_msg_desc;
extern rxcsar_msg_desc_type rxcbcch_sar_msg_desc;
extern boolean raho_flag;
extern rxc_crc_cnt_type rxcbcch_frame_crc_stats;
extern rxc_crc_cnt_type rxcfccch_frame_crc_stats;
#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */
extern const rxcfccch_frame_info_tbl_type rxcfccch_frame_info_tbl[];

#ifdef FEATURE_IS2000_REL_A_CC_QPM
extern word rxcfccch_last_sec;
extern boolean rxcfccch_empty_gpm_and_done(byte *);
extern boolean rxcfccch_empty_upm_and_done(byte *);
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

/* NCCCH global variables */
byte rxcnccch_chan_enable_bits;
#ifdef FEATURE_IS2000_REL_A_CC_QPM
boolean rxcnccch_page_match_ind_sent = FALSE;
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

extern boolean rxcfccch_increase_page_matched_cnt;

/*--------------------------------------------------------------------------
 * Definitions and Declarations
 *--------------------------------------------------------------------------*/

rxc_state_type rxcnccch_process_cmd (void);
void rxcnccch_exit(rxc_state_type state);

/*===========================================================================

FUNCTION RXCNCCCH_STATE

DESCRIPTION
  NCCCH state processing.

DEPENDENCIES
  
RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
rxc_state_type 
rxcnccch_state (void)
{
  rxc_state_type next_state = RXC_NCCCH_STATE;
  
  rex_sigs_type sigs;             
  rex_sigs_type sa_wait_mask;
  rxc_dec_type *buf_ptr;  /* pointer to decoder data buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

  MSG_HIGH ("Start NCCCH state", 0, 0, 0);

  rxc_vote_not_ok_to_sleep();

  while ((next_state = rxcnccch_process_cmd ()) == RXC_NCCCH_STATE) 
  {
    
    sa_wait_mask = RXC_CMD_Q_SIG | RXC_INT_SIG | rxcidle_wait_mask ();

    sigs = rxc_wait (sa_wait_mask);

    /* Check if we have any signals to process.  Afterward, complete loop
       by checking the RXC command queue. */
    if (rxcidle_process_sigs (sigs,RXC_NCCCH_STATE)) 
    {
      if (sigs & RXC_INT_SIG) 
      {
        (void) rex_clr_sigs (&rx_tcb, RXC_INT_SIG);
         while ((buf_ptr = (rxc_dec_type *) q_get (&rxc_dec_q)) != NULL) 
         {
           /* If not FCCCH state frame, drop the frame */
           if (buf_ptr->state == RXC_FCCCH_STATE) 
           {
             /* do frame counting */
             rxcfccch_update_frame_counters();
             /* process the FCCCH frame */
             rxcfccch_process_frames (buf_ptr);
           }
           else if (buf_ptr->state == RXC_BCCH_STATE)
           {
             rxcbcch_process_frames (buf_ptr);
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
             if (rxc_slot.slot_mask & BC_DATA_SLOT)
             {
               if ((rxc_slot.bcsms_slot_frames == 0) || (rxc_slot.valid_msg))
               {
                 if(rxc_slot.valid_msg)
                 {
                   MSG_HIGH( "Got ok BC msg on BCCH,Total #%x frames", 
                  (MAX_NUM_FRAME_PER_BCBCCH_MSG-rxc_slot.bcsms_slot_frames), 0, 0 );
                 }
                 else
                 {
                   MSG_HIGH( "No ok BC msg on BCCH, time out",0, 0, 0 );
                 }

                 if(rxc_slot.bcsms_slot_frames == 0)
                 {
                   byte rpt_slot_mask = 0; /* the mask to be passed in SLOT_R */
                   mccrx_rpt_type *rpt_ptr;
                   rpt_slot_mask |= BC_DATA_SLOT;

                   if ((rpt_ptr = (mccrx_rpt_type *)q_get(&mcc_rx_free_q) ) != NULL)
                   {
                     rpt_ptr->hdr.rpt = RXC_SLOT_R;
                     ts_get_20ms_frame_time( rpt_ptr->slot.rpt_time );
                     rpt_ptr->slot.slot_mask = rpt_slot_mask;

                     /* we don't stop counting slot frames like before, the decision
                     * is left for CP to wake. rxc_slot.active cleared when
                     * RXC_SLEEP_F is rx'd
                     */
                     rxc_mc_rpt(rpt_ptr );
                     /* clear the bits that triggered the valid msg report, so we
                     * don't send the same report more than once
                     */
                     rxc_slot.valid_msg_mask &= ~rpt_slot_mask;
                     rxc_slot.slot_mask = 0;
                     rxcidle_set_slotted_status( RXCIDLE_SLOTTED_ASLEEP );
                     rxc_slot.valid_msg = FALSE;
                   }
                   else
                   {
                     ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
                   }
                 }
               }
             }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
           }
           else
           {
             MSG_HIGH("In NCCCH state, got none-NCCCH frame",0,0,0);
           }
           /* Free up the buffer for the ISR */
           q_put (&rxc_dec_free_q, &buf_ptr->link);
         }
      }
    }
  }

  /* Perform exit processing */
  rxcnccch_exit(next_state);
  return (next_state);

}

void rxcnccch_isr(void)
{
  rxc_dec_type *buf_ptr = NULL;
  rxc_isr_cmd_type *cmd_ptr = NULL;
  boolean  receive_slot_mask_zero = FALSE;
  dec_frame_status_type dec_status = {0, 0, 0, 0};
  byte task_id = 0xff;
  uint8 dec_int_status;

  dec_int_status = (uint8) HWIO_IN(DEC_DONE_INT_STATUS);
  HWIO_OUT(DEC_DONE_INT_STATUS, dec_int_status); //clear the interrupt
  if(!(dec_int_status & 0x2))
  {
    return;
  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get frame number and subtract 1 since the decoder output buffer will 
   * contain curr_frame_time-1 frame data in packet mode. */
  ts_get_20ms_frame_time (rxc_dec_frame);
  qw_dec (rxc_dec_frame, 1L);

  /* Check to see if there are pending commands for the interrupt handler */
  while( (cmd_ptr = (rxc_isr_cmd_type *)q_get(&rxc_isr_q)) != NULL )
  {
    MSG_HIGH ("rxcnccch_isr got ISR cmd 0x%x", cmd_ptr->cmd, 0, 0);

    switch (cmd_ptr->cmd) {
      case RXC_TC_F:

        MSG_MED("Got RXC_TC_F", 0, 0, 0);

        /* Always returns TRUE.  Ignoring return value */
        (void) rxctraffic_frame_bnd_init ();
        break;

      case RXC_WAKE_F:
        {
          qword tmp_qw;

          MSG_MED("Got RXC_WAKE_F", 0, 0, 0);
          qw_equ(tmp_qw, rxc_dec_frame);
          qw_shift(tmp_qw, -2); /* get the cur slot number */
  
          /* Save the page slot and sci mask for use elsewhere */
          rxc_slot.idle_pgslot = cmd_ptr->idle_pgslot;
          rxc_slot.sci_mask    = cmd_ptr->sci_mask;
  
          rxc_slot.slot_now = (word)(qw_lo(tmp_qw));
  
          if (rxc_slot.slot_now > rxc_slot.slot)
          {
            /* slot has wrapped the word boundary, back roll the number by one
             * cycle*/
            rxc_slot.slot -= (cmd_ptr->sci_mask+1);
            rxc_slot.slot_now -= (cmd_ptr->sci_mask+1);
          }
  
          if ((rxc_slot.slot_mask & PAGE_SLOT) || (rxc_slot.slot_mask == 0))
          {
            /* set 2 slot timer for assigned paging slot */
            rxc_slot.slot_frames =
                4 * (rxc_slot.slot - rxc_slot.slot_now) + 8
                - ( word )(qw_lo( rxc_dec_frame ) & 03); //lint !e734 Loss of precision (assignment) (18 bits to 16 bits)
            if (rxc_slot.slot_mask == 0)
            {
              /* in the case of rule wakeup, MC can send MUX a slot mask = 0, for such case, MUX 
              should treat it as a PAGE_SLOT, but without a checking for page match */
              receive_slot_mask_zero = TRUE;
              rxc_slot.slot_mask = PAGE_SLOT;
              rxc_slot.valid_msg_mask = rxc_slot.valid_msg_mask | rxc_slot.slot_mask;
              /* Page match is not available */
              rxc_slot.page_match = FALSE;
              MSG_HIGH("MUX got slot mask = 0 from MC, treat as PC_SLOT",0,0,0);
            }
          }
  
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
          if (rxc_slot.slot_mask & BC_DATA_SLOT)
          {
            rxc_slot.valid_msg_mask = rxc_slot.slot_mask;
            /* The maximum BC MSG length is 267 bytes which is about 3 frames
            ** the frame duration is is also the RXC BCCH ISR
            ** interval.
            */
            rxc_slot.bcsms_slot_frames = MAX_NUM_FRAME_PER_BCBCCH_MSG;
            rxc_slot.valid_msg = FALSE;
      /* need to peek into BC BCCH frame to see the MS address is matched*/
            rxc_bcbcch_peek_sms_msadress_done = FALSE;
          }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

#ifdef FEATURE_IS2000_REL_A_CC_QPM
          /* May come from Paging Channel as a result of Sync Channel
             workaround. So make sure page match criteria is for FCCCH */
          if (!receive_slot_mask_zero)
          {
          if ((rxc_slot.slot_mask & PAGE_SLOT)
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
              && !(rxc_slot.slot_mask & BC_DATA_SLOT)
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
             )
          {
            /* Page match is available */
            rxc_slot.page_match = TRUE;
          }
          else
          {
            /* Page match is not available */
            rxc_slot.page_match = FALSE;
          }
          }

          /* Mark page match indication has not been sent */
          rxcnccch_page_match_ind_sent = FALSE;
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

          rxc_slot.active = TRUE;
  
          rxc_slot.after_slot_boundary = FALSE;
  
          /* Set countdown to re-initialize frame count after interrupt
           * timing has setlled down as we have just slammed and reprogrammed
           * the decoder mode.
           */
          rxc_time_cnt = 3;
          raho_flag = FALSE;
        }
        break;

      default:
        MSG_ERROR("Bad ISR CMD: %d", cmd_ptr->cmd, 0, 0);
        break;

    } /* switch */

    /* return the memnory back to the free queue */
    q_put (&rxc_isr_free_q, &cmd_ptr->link);

    /* Notify task of completion of ISR command. */
    (void) rex_set_sigs (&rx_tcb, RXC_ISR_CMD_SIG);
  }

/*-------------------------------------------------------------------------*/
  /* perform normal interrupt processing */
  /* get the task id */
  task_id = DEINT_TASK_ID_GET();

  if ((task_id == RXCNCCH_FCCCH_DECODER_INT) && (rxc_slot.skipping == TRUE))
  {
    if( rxcidle_first_frame( RXC_CDMA_FCCCH ) == TRUE )
    {
      rxcfccch_info.first_frame = TRUE;
      /* This is the first frame of the slot, after a WAKEUP. */
      /* This flag is unused at the moment */

      rxc_slot.skipping = FALSE;
      /* Don't skip frames anymore. */
    }
  }
  
  if ((task_id == RXCNCCH_BCCH_DECODER_INT) && (rxc_slot.skipping_bcch))
  {
    if( rxcidle_first_frame( RXC_CDMA_BCCH ) == TRUE )
    {
      rxcbcch_info.first_frame = TRUE;
      /* This is the first frame of the slot, after a WAKEUP. */
      /* This flag is unused at the moment */

      rxc_slot.skipping_bcch = FALSE;
      /* Don't skip frames anymore. */
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      if (rxc_slot.slot_mask & BC_DATA_SLOT)
      {
        /* some time SRCH could wake 40ms early, before the BS send
        * the first BC BCCH frame, those case, SRCH will tell RXC to skip
        * that frame
        */
        MSG_HIGH("Skp the fr of BC BCCH ",0,0,0);
        return;
      }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
    }
  }

  if(((rxc_slot.skipping == TRUE) && (task_id == RXCNCCH_FCCCH_DECODER_INT)) 
       ||((rxc_slot.skipping_bcch == TRUE) && (task_id == RXCNCCH_BCCH_DECODER_INT)))  
  {
       /* This is the interrupt for a frame
        we do not care about. Just exit,
        discarding it outright.  */

    rxc_slot.frame_skip_cnt++;
      /* Count each frame while skipping is TRUE */

    /* Once we have waited for an excessive number of frames to
       go by without recognizing the first frame of the slot, assume
       that the symbol combiner phase/offset is incorrect and start
       decoding.  This will likely cause a lost NCCCH Channel, but
       is better than hanging indefinitely (probably forever) waiting
       for the phase to be correct. */
    if ( rxc_slot.frame_skip_cnt >= MAX_SKIPPED_FRAMES )
    {
      MSG_ERROR("Skpped %d fr - SC Bad",
                 rxc_slot.frame_skip_cnt, 0, 0);

      /* Don't skip frames anymore. */
      rxc_slot.skipping = FALSE;
      /* This is definitely not the first frame of the slot after a WAKEUP. */
      rxcfccch_info.first_frame = FALSE;

      rxc_slot.skipping_bcch = FALSE;
      rxcbcch_info.first_frame = FALSE;
     
#ifdef FEATURE_IS2000_REL_A_CC_QPM 
      rxc_slot.page_match = FALSE;
      rxcnccch_page_match_ind_sent = TRUE;
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

      srch_mux_page_match( FALSE );
        /* Definitely no page matching happening at this point */
    }

    MSG_LOW("  Decint: fr=%lx, slt=%d",  qw_lo(rxc_dec_frame),
       ( (word)(qw_lo( rxc_dec_frame ) >> 2) & rxc_slot.sci_mask ), 0 );
    
    if(task_id == RXCNCCH_FCCCH_DECODER_INT)
    {

#ifdef FEATURE_IS2000_REL_A_CC_QPM
      rxcfccch_last_sec = dec_status.sec;
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

      MSG_HIGH( "Skping FCCCH fr after wakeup", 0, 0, 0 );
    }
    else
    {
      MSG_HIGH( "Skping BCCH fr after wakeup", 0, 0, 0 );
    }
    return;
  }
  
  /* handle BCCH */
  if ((rxc_read_bcch_data_from_decoder) && (task_id == RXCNCCH_BCCH_DECODER_INT))
  {
    rxc_read_bcch_data_from_decoder = FALSE;
    /* Logging needs to keep track of frame quality history, so a buffer 
       should be sent regardless of CRC success.  If CRC fails, nothing
       is stored. */
    buf_ptr = (rxc_dec_type *) q_get (&rxc_dec_free_q);
  
    if (rxcbcch_info.first_frame  
#ifdef FEATURE_IS2000_REL_A_CC_QPM 
        && !rxcnccch_page_match_ind_sent
#endif /* FEATURE_IS2000_REL_A_CC_QPM */
       )
    {
#ifdef FEATURE_IS2000_REL_A_CC_QPM 
      rxc_slot.page_match = FALSE;
      rxcnccch_page_match_ind_sent = TRUE;
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

      srch_mux_page_match( FALSE );

      rxcbcch_info.first_frame = FALSE;
    }

    if (!dec_read_bcch_status (&dec_status))
    {
      MSG_ERROR("Can't read BCCH stat",0,0,0);
    }

    srch_mux_idle_frame_stats(CAI_BCCH, dec_status.sec, 
                              dec_status.energy, dec_status.crc_passed); 

    if (dec_status.crc_passed) 
    {
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      /* check to see if the data is BC BCCH */
      if(rxc_slot.slot_mask & BC_DATA_SLOT)
      {
        if (!rxc_bcbcch_first_frame)
        {
          rxc_bcbcch_first_frame = TRUE;
        }
      }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

      MSG_MED("BCCH CRC ok S:0x%x E:0x%x",dec_status.sec, dec_status.energy,0);
      rxcbcch_frame_crc_stats.good_crcs++;
      if (buf_ptr)
      {
        /* Indicate to the task level frame processor that this was a good
        frame. This distinction is used for frame quality history in
        logging. */
        buf_ptr->dec_rate = CAI_FULL_RATE;

        /* Always returns TRUE.  Ignoring return value */
        (void) dec_read_data (0, buf_ptr->buf.buf, DEC_BCCH_SIZ);
        /* record the frame time */
        ts_get_20ms_frame_time (buf_ptr->frame);

        /* The decoder ISR for BCCH is executed at the end of the frame that
        is being decoded.  Therefore, the frame number for the decoded
        frame is curr_frame_time - 1 */
        qw_dec (buf_ptr->frame, 1L);
      }
    }
    else
    {
      MSG_MED("BCCH CRC bad S:0x%x E:0x%x",dec_status.sec, dec_status.energy,0);
      rxcbcch_frame_crc_stats.bad_crcs++;
      if (buf_ptr)
      {
        /* Indicate to the task level frame processor that this was a bad
        frame. This distinction is used for frame quality history in
        logging. */
        buf_ptr->dec_rate = CAI_NULL_RATE;
      }
    }
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
    /*decrease the BC BCCH maximum message duration by 1 */
    if((rxc_slot.slot_mask & BC_DATA_SLOT)  && (rxc_slot.bcsms_slot_frames > 0) )
    {
      rxc_slot.bcsms_slot_frames--;
    }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
    if (buf_ptr) 
    {
      buf_ptr->state = RXC_BCCH_STATE;
      q_put (&rxc_dec_q, &buf_ptr->link);
    } 
    else 
    {
      ERR( "0 bufs on dec_free_q", 0, 0, 0 );
    }
    /* for now, there maybe 2 interupts per 20ms slot, the first one 
    is BCCH, the second one is FCCCH, during the first one, just service
    BCCH, then return */
    // Set the signal always so the task gets to consume any leftover items
    // on the rxc_dec_q and return them to the rxc_dec_free_q
    (void) rex_set_sigs (&rx_tcb, RXC_INT_SIG);
#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */
    return;
  }
  /* handle FCCCH */
  if ((rxc_read_fccch_data_from_decoder) && (task_id == RXCNCCH_FCCCH_DECODER_INT))
  {
    buf_ptr = (rxc_dec_type *) q_get (&rxc_dec_free_q);    

#ifndef FEATURE_IS2000_REL_A_CC_QPM
    if (rxcfccch_info.first_frame)
    {
      srch_mux_page_match( FALSE );
      rxcfccch_info.first_frame = FALSE;
    }
#endif /* !FEATURE_IS2000_REL_A_CC_QPM */

    if (buf_ptr)
    {
      /* read in status bytes: CRC, Yamamoto, symbol error rates & energy */
      if (!dec_read_ccch_status (&dec_status, rxcfccch_info.data_rate))
      {
        MSG_ERROR("Can't read CCCH stat",0,0,0);
      }

      srch_mux_idle_frame_stats(CAI_F_CCCH, dec_status.sec, 
                                dec_status.energy, dec_status.crc_passed); 

      if (dec_status.crc_passed)
      {
        /* This can be helpful for debugging. */
        MSG_MED ("FCCCH CRC ok S:0x%x E:0x%x", dec_status.sec,dec_status.energy, 0);
        rxcfccch_frame_crc_stats.good_crcs++;
        buf_ptr->dec_rate = CAI_FULL_RATE;
      }
      else
      {
        MSG_MED("FCCCH CRC bad S:0x%x E:0x%x",dec_status.sec,dec_status.energy, 0);
        rxcfccch_frame_crc_stats.bad_crcs++;
        buf_ptr->dec_rate = CAI_NULL_RATE;
      }
      /* Indicate to the task level frame processor that this was a good 
         frame. This distinction is used for frame quality history in 
         logging. */
  
      dec_read_ccch_data (buf_ptr->buf.buf, rxcfccch_info.data_rate);
  
      qw_equ(buf_ptr->frame, rxc_dec_frame);

#ifdef FEATURE_IS2000_REL_A_CC_QPM 
      if (rxcfccch_info.first_frame)
      {
        boolean page_match = FALSE;

        /* Check to see if page match was enabled this wakeup */
        if (rxc_slot.page_match)
        {
          /* OK, we can try to match. Go for it */
            
          /* Disable matcher for this wakeup */
          rxc_slot.page_match = FALSE;
 
          /* Check of the first half frame of the slot is an empty General
             Page Message or unsegmented Universal Page Message.
             Note: This check will work only with a low probability of false
             sleeping 
          */

          /* Quick page match for General Page Message */ 
          if (rxcfccch_empty_gpm_and_done(buf_ptr->buf.buf))
          {
            /* An empty General Page Message and no more page for this mobile
               in the current slot 
            */
            MSG_HIGH("Gen Pg Match: ser%d|%d rssi=%d",
              rxcfccch_last_sec, dec_status.sec, srch_mux_get_fing_rssi());
            page_match = TRUE;
            /* set the page matched flag to indicate to MC later on */
            rxcfccch_increase_page_matched_cnt = TRUE;
          }
          /* Quick page match for Universal Page Message */ 
          else if (rxcfccch_empty_upm_and_done(buf_ptr->buf.buf))
          {
            /* An empty Universal Page Message and no more page for this mobile
               in the current slot */
            MSG_HIGH("Univ Pg Match: ser%d|%d rssi=%d",
              rxcfccch_last_sec, dec_status.sec, srch_mux_get_fing_rssi());
            page_match = TRUE;
            /* set the page matched flag to indicate to MC later on */
            rxcfccch_increase_page_matched_cnt = TRUE;
          }
          else
          {
            MSG_HIGH("No Pg Match: ser%d|%d rssi=%d",
              rxcfccch_last_sec, dec_status.sec, srch_mux_get_fing_rssi());
          }
        } /* end if (rxc_slot.page_match) */

        rxcnccch_page_match_ind_sent = TRUE;

        srch_mux_page_match( page_match );

        rxcfccch_info.first_frame = FALSE;
      } /* end if (rxcfccch_info.first_frame) */
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

    }
#ifdef FEATURE_IS2000_REL_A_CC_QPM 
    else
    {
      if (rxcfccch_info.first_frame)
      {
        rxc_slot.page_match = FALSE;
        rxcnccch_page_match_ind_sent = TRUE;

        srch_mux_page_match( FALSE );

        rxcfccch_info.first_frame = FALSE;
      }
    } /* end if (buf_ptr) */
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

    if (buf_ptr) 
    {
      buf_ptr->state = RXC_FCCCH_STATE;
      q_put (&rxc_dec_q, &buf_ptr->link);
    } 
    else 
    {
      ERR( "0 bufs on dec_free_q", 0, 0, 0 );
    }
#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */
  }
  // Set the signal always so the task gets to consume any leftover items
  // on the rxc_dec_q and return them to the rxc_dec_free_q
  (void) rex_set_sigs (&rx_tcb, RXC_INT_SIG);
}

void rxcnccch_exit(rxc_state_type state)
{
  /* the slot to asleep */
  rxcidle_set_slotted_status( RXCIDLE_SLOTTED_ASLEEP );
 
  /* check to see which channel is on, then call the correspoinding exit
  function */
  if ((rxcnccch_chan_enable_bits & RXCNCCCH_PRIMARY_BCCH_BIT)
    || (rxcnccch_chan_enable_bits & RXCNCCCH_SECONDARY_BCCH_BIT ))
  {
    /* delete the deinterleave task for BCCH, in the second 5 ms slot */
    deint_delete_nccch_task_list(RXC_BCCH_DEINT_TASK_SLOT_NUMBER);
    rxcbcch_exit();
  }
  if (rxcnccch_chan_enable_bits & RXCNCCCH_FCCCH_BIT)
  {
    /* delete the deinterleave task for FCCCH, in the first 5 ms slot */
    /* since the fundicate or synch channel is also in the first 5ms slot,
    shoud not delete the deinterleave task when transit to TCI or SC state */
    if ((state != RXC_TCI_STATE) && (state != RXC_SC_STATE))
    {
      deint_delete_nccch_task_list(RXC_FCCCH_DEINT_TASK_SLOT_NUMBER);
    }
    rxcfccch_exit();
  }
  /* clean the channel bit */
  rxcnccch_chan_enable_bits = 0;

  /* Stop the NCCCH failsafe timer */
  rxcnccch_failsafe_timer(RXCNCCCH_FAILSAFE_STOP);

  MSG_HIGH("Exit NCCCH state ",0,0,0);
}

void rxcnccch_activate (void)
{
  INTLOCK ();
  tramp_set_isr (TRAMP_RX_ISR, rxcnccch_isr);
  /* Clear DEC DONE int status to make sure ISR is properly serviced.
  */
  HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
  INTFREE ();

  rxcfccch_info.first_frame = TRUE;
  rxc_read_fccch_data_from_decoder = TRUE;
  rxcidle_set_slotted_status( RXCIDLE_SLOTTED_AWAKE );
}


rxc_state_type rxcnccch_process_cmd (void)
{
  rxc_cmd_msg_type *cmd_ptr = NULL;
  rxc_state_type next_state = RXC_NCCCH_STATE;
  boolean success = TRUE;

  (void) rex_clr_sigs (&rx_tcb, RXC_CMD_Q_SIG);

  while ((cmd_ptr = (rxc_cmd_msg_type *) q_get (&rxc_cmd_q)) != NULL) {
    switch (cmd_ptr->hdr.command) {
      
      case RXC_TCI_F:
      {
        MSG_MED("Got RXC_TCI_F", 0, 0, 0);

        /* this is ok using RXC_FCCCH_STATE, the RXC_NCCCH_STATE
        is transparent to TCI, for the TCI, it is the same as RXC_FCCCH_STATE*/
        MSG_HIGH("Go to TCI init state ", 0,0,0);
        next_state = rxc_tci_init (RXC_FCCCH_STATE, cmd_ptr);

        break;
      }
      
      case RXC_ACC_FADE_F:
      {
        MSG_MED("Got RXC_ACC_FADE_F", 0, 0, 0);

        //TODO: Review  P_REV check in rxcpaging.c.
        //IS95B is not used for FCCCH.  Need to check P_REV?
        rxcidle_set_fade_timer (RXCIDLE_ACCESS_FADE);
        
        /* Make sure MC's task priority is at the normal level */
        (void) rex_task_pri(&mc_tcb, MC_PRI ); //lint !e506 boolean constant due to MC_PRI macro definition
        
        break;
      }
  
      case RXC_IDL_FADE_F:
      {
        MSG_MED("Got RXC_IDL_FADE_F", 0, 0, 0);
        rxcidle_set_fade_timer (RXCIDLE_IDLE_FADE);

        break;
      }
  
      case RXC_CHAN_CONFIG_F:
      {

        MSG_MED("Got RXC_CHAN_CONFIG_F", 0, 0, 0);

        /* Note: Currently, we ignore ''num_chan' as the driver only 
           supports 1 channel */
        if ( cmd_ptr->hdr.cmd_hdr.sigs == (rex_sigs_type) SRCH_CMD_SIG )
        {
          raho_flag = TRUE;
        }
        next_state = rxcidle_nccchan_config (&cmd_ptr->chan_config);
        break;
      }
  
      case RXC_CDMA_F:
      {
        /* --------------------------------------------------
         * Transition back to CDMA init state without exiting
         * rxc_subtask()
         * -------------------------------------------------- */
        MSG_MED("Got RXC_CDMA_F", 0, 0, 0);
        rxc_exit ();
        
        next_state = RXC_CDMA_STATE;
        break;
      }
      
      case RXC_EXIT_F:
      {
        MSG_MED("Got RXC_EXIT_F", 0, 0, 0);
        next_state = RXC_EXIT_STATE;
        break;
      }
  
      case RXC_SC_F:
      {
        MSG_MED("Got RXC_SC_F", 0, 0, 0);

       /* initialize Sync channel state */
        rxc_sc_init (RXC_FCCCH_STATE);/* this is ok using RXC_FCCCH_STATE */
        next_state = RXC_SC_STATE;
        break;
      }
  
      case RXC_IDLE_HO_F:
      {
        MSG_MED("Got RXC_IDLE_HO_F", 0, 0, 0);

        /* handle FCCCH */
        rxcsar_terminate (&rxcfccch_sar_msg_desc,
        rxcfccch_frame_info_tbl[rxcfccch_info.data_rate].subframe_flush_cnt);
        /* flush the accumalated fccch logging buffer */
        rxcfccch_frame_info_log_send ();

        /* handle BCCH */
        rxcsar_terminate (&rxcbcch_sar_msg_desc, 8);
        /* flush the accumalated bcch logging buffer */
        rxcbcch_frame_info_log_send ();
      
        break;
      }
  
      case RXC_WAKE_F:
      {
        MSG_MED("Got RXC_WAKE_F", 0, 0, 0);

        /* call the function to install the corresponding decoder ISR */
        rxc_idle_activate();
        
        /* Set flush count to 0 in case an Idle HO occured before sleep */
        rxcsar_terminate (&rxcfccch_sar_msg_desc, 0);
        rxcsar_terminate (&rxcbcch_sar_msg_desc, 0);

        INTLOCK ();

        /* Clear OKTS sig - We don't want to sleep now */
        rxc_vote_not_ok_to_sleep();

        /* we don't want to set the skip flag to TRUE in case
        * of BC BCCH, because for BC BCCH, the decision of skipping
        * will send send in the chan config cmd from SRCH, we will
        * set it when we receive the CHAN_CONFIG_CMD 
        */
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
        if ((cmd_ptr->wake.slot_mask & BC_DATA_SLOT) == 0)
#endif  /*  FEATURE_IS2000_REL_A_CC_BCSMS */
        {
          if (rxcnccch_chan_enable_bits & RXCNCCCH_FCCCH_BIT)
          {
            /* Reinitialize slot counter for FCCCH */
            rxc_slot.skipping = TRUE;
          }
          if (rxcnccch_chan_enable_bits & RXCNCCCH_PRIMARY_BCCH_BIT)
          {
            /* Reinitialize slot counter for BCCH */
            rxc_slot.skipping_bcch = TRUE;
          }
        }

        /* Reset the frame count used during skipping */
        rxc_slot.frame_skip_cnt = 0;

        /* Clear out path clear control flag */
        rxc_slot.blank = FALSE;

        rxc_slot.slot_mask = cmd_ptr->wake.slot_mask;
        rxc_slot.valid_msg_mask |= cmd_ptr->wake.slot_mask;
        rxc_slot.slot = cmd_ptr->wake.slot;
        
#ifdef FEATURE_IS2000_REL_A_CC_QPM 
        if ((rxc_slot.slot_mask & PAGE_SLOT)
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
            && !(rxc_slot.slot_mask & BC_DATA_SLOT)
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
           )
        {
          /* Page match is available */
          rxc_slot.page_match = TRUE;
        }
        else
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

        {
          /* Page match is not available */
          rxc_slot.page_match = FALSE;
        }

#ifdef FEATURE_IS2000_REL_A_CC_QPM 
        /* Mark page match indication has not been sent */
        rxcnccch_page_match_ind_sent = FALSE;
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

        //XXX The ISR command is for rxc_slot processing.
        /* Send a command to the interrupt handler to reset frame count */
        rxc_isr_cmd_buf_ptr = (rxc_isr_cmd_type *) q_get (&rxc_isr_free_q);
        if (rxc_isr_cmd_buf_ptr != NULL)
        {
          rxc_isr_cmd_buf_ptr->cmd = RXC_WAKE_F;
          rxc_isr_cmd_buf_ptr->sci_mask = cmd_ptr->wake.sci_mask;
          rxc_isr_cmd_buf_ptr->idle_pgslot = cmd_ptr->wake.idle_pgslot;
          rxc_isr_cmd (rxc_isr_cmd_buf_ptr);
        }
        else
        {
          ERR( "0 bufs on isr_free_q", 0, 0, 0 );
        }

        INTFREE ();

        /* begin timing 2 slots and checking for a valid message in the
           2 slots */
        rxc_slot.valid_msg = FALSE;

        /* start Idle fade timer */
        rxcidle_set_fade_timer (RXCIDLE_IDLE_FADE);

        /* Start the failsafe timer */
        rxcnccch_failsafe_timer(RXCNCCCH_FAILSAFE_START);

        break;
      }
  
      case RXC_SLEEP_F:
      {
        rxcidle_set_slotted_status( RXCIDLE_SLOTTED_ASLEEP );

#ifdef FEATURE_IS2000_REL_A_CC_BCCH_ON_IN_SLEEP
        if (rxcnccch_chan_enable_bits & RXCNCCCH_PRIMARY_BCCH_BIT)
        {
          /* delete the deinterleave task for BCCH, in the second 5 ms slot */
          deint_delete_nccch_task_list(RXC_BCCH_DEINT_TASK_SLOT_NUMBER);
          rxcbcch_exit();
          rxcnccch_chan_enable_bits = 
               rxcnccch_chan_enable_bits & (~RXCNCCCH_PRIMARY_BCCH_BIT);
          MSG_HIGH("Got RXC_SLEEP_F before BCCH is off", 0, 0, 0);
        }
        else
#endif /* FEATURE_IS2000_REL_A_CC_BCCH_ON_IN_SLEEP */
        {
          MSG_MED("Got RXC_SLEEP_F", 0, 0, 0);
        }

        /* stop running slot timer also */
        rxc_slot.active = FALSE;
        
        /* reset all valid message reporting counters */
        rxc_reset_frame_count ();

        rxcidle_clr_fade_timer ();

        /* Stop the failsafe timer */
        rxcnccch_failsafe_timer(RXCNCCCH_FAILSAFE_STOP);

        /* Disable RX ISR as we are done demodulating frames */
        rxc_clear_isr ();

        /* Clear buffers from decoder queue. */
        rxc_clear_decoder_queue ();
        /* Tell the sleep task that we're ready for the CPU to halt
        ** The sleep task is lower priority, so it won't do anything
        ** until the RXC task suspends.
        */

         /* Send accumulated log, if any. */
        rxcfccch_frame_info_log_send ();

        rxc_vote_ok_to_sleep();
        break;
      }

      case RXC_SLOT_F: //XXX
      {
        MSG_MED("Got RXC_SLOT_F", 0, 0, 0);
  
        INTLOCK();

        /* Make sure page matching is completely disabled */
        rxc_slot.page_match = FALSE;

#ifdef FEATURE_IS2000_REL_A_CC_QPM 
        /* Mark page match indication has not been sent */
        rxcnccch_page_match_ind_sent = FALSE;
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

        rxc_slot.blank      = FALSE;
        if (next_state == RXC_NCCCH_STATE)
        {
          /* set interrupts on for FCCCH */
          rxcnccch_activate();
          rxc_slot.skipping   = FALSE;
          rxc_slot.skipping_bcch = FALSE;
        }
     
        /* Clear OKTS sig - We don't want to sleep now */
        rxc_vote_not_ok_to_sleep();
  
        rxc_slot.slot_mask = cmd_ptr->wake.slot_mask;
        rxc_slot.valid_msg_mask |= cmd_ptr->wake.slot_mask;
        rxc_slot.slot = cmd_ptr->wake.slot;
        /* Send a command to the interrupt handler to reset frame count */
        rxc_isr_cmd_buf_ptr = (rxc_isr_cmd_type *) q_get (&rxc_isr_free_q);
        if (rxc_isr_cmd_buf_ptr != NULL)
        {
          rxc_isr_cmd_buf_ptr->cmd = RXC_WAKE_F;
          rxc_isr_cmd_buf_ptr->sci_mask = cmd_ptr->wake.sci_mask;
          rxc_isr_cmd_buf_ptr->idle_pgslot = cmd_ptr->wake.idle_pgslot;
          rxc_isr_cmd(rxc_isr_cmd_buf_ptr);
        }
        else
        {
          ERR( "0 bufs on isr_free_q", 0, 0, 0 );
        }

        INTFREE();

        /* begin timing 2 slots and checking for a valid message in the
           2 slots */
        rxc_slot.valid_msg = FALSE;
  
        /* Start the failsafe timer */
        rxcnccch_failsafe_timer(RXCNCCCH_FAILSAFE_START);

        cmd_ptr->hdr.status = RXC_DONE_S;
        break;
      }
  
      default:
      {
       success = FALSE;
#ifdef FEATURE_FACTORY_TESTMODE
        if(ftm_mode == FTM_MODE) {
          MSG_HIGH("Bad RXC cmd %d", cmd_ptr->hdr.command ,0, 0);
        } else {
          ERR_FATAL("Bad cmd %d", cmd_ptr->hdr.command, 0, 0);
        }
#else
        ERR_FATAL("Bad cmd %d", cmd_ptr->hdr.command, 0, 0);
#endif /* FEATURE_FACTORY_TESTMODE */
      }
    } /* switch */
  
    if (success) {
      cmd_ptr->hdr.status = RXC_DONE_S;
    }

    /* return item to queue if necessary */
    cmd_done (&cmd_ptr->hdr.cmd_hdr);
    
    //Note: Copied from rxc_pc() - not sure why this is called twice.
    (void) rex_clr_sigs (&rx_tcb, RXC_CMD_Q_SIG);
  }

  return next_state;
}


/*===========================================================================
FUNCTION:
  rxcnccch_failsafe_timer

DESCRIPTION:
  This function encapsulates a failsafe timer for use during simultaneous
  BCCH + FCCCH demodulation.  The failsafe timer monitors BOTH the BCCH
  and FCCCH demodulation, and will set a signal (RXCNCCCH_FAILSAFE_TIMER_SIG)
  to the CDMA receive subtask when either (or both) of the BCCH and FCCCH
  have reported bad frames or messages for a duration of
  RXCNCCCH_FAILSAFE_TIMER_MS.

  This timer is needed due to the fact that the standard T30M timer only
  ensures that ONE of either BCCH or FCCCH has good messages.  In the
  unlikely (but possible) scenario that the BCCH is bad, but the FCCH is
  good (or vice-versa), the T30M timer will never fire, but the failsafe
  will.

  This function takes a command of type rxcnccch_failsafe_cmd_type that
  instruct the failsafe timer to start, stop, or report good or bad frames
  for either the BCCH or FCCCH channel.

DEPENDENCIES:
  This function uses the global variable rxcnccch_chan_enable_bits to
  detect which channels (FCCCH and/or BCCH) are currently active.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rxcnccch_failsafe_timer(rxcnccch_failsafe_cmd_type cmd)
{
  static rex_timer_type failsafe_timer;

  /* Internal state variables */
  static boolean failsafe_initialized = FALSE, failsafe_started;

  /* An internal bitmask similar to rxcnccch_chan_enable_bits that
   * keeps track of which channels have bad frames/msgs.
   */
  static byte rxcnccch_channel_failure_mask;

  /* Initialize the timer upon the first call to this function. */
  if (!failsafe_initialized) {
    /* Initialize all internal state variables */
    failsafe_initialized = TRUE;
    failsafe_started = FALSE;
    rxcnccch_channel_failure_mask = 0;
    /* Initialize the actual failsafe timer */
    rex_def_timer (&failsafe_timer,
                   &rx_tcb,
                   RXC_PC_FS_TIMER_SIG);
  }
  /* Since initialization (above) always succeeds, no need to check
   * the failsafe_initialized variable anymore
   */

  /* Handle whatever command we were sent */
  switch(cmd)
  {
    case RXCNCCCH_FAILSAFE_START:
      MSG_MED("Got RXCNCCCH_FAILSAFE_START", 0, 0, 0);

      /* Start the failsafe timer and clear the failure mask.
       * (Do nothing, however, if timer is already started.)
       */
      if (!failsafe_started) {
        rxcnccch_channel_failure_mask = 0;
        failsafe_started = TRUE;
        /* Actual timer will be started below... */
      }
      break;

    case RXCNCCCH_FAILSAFE_STOP:
      MSG_MED("Got RXCNCCCH_FAILSAFE_STOP", 0, 0, 0);

      /* Stop the failsafe timer and clear any pending signal */
      failsafe_started = FALSE;
      (void)rex_clr_timer(&failsafe_timer);
      (void)rex_clr_sigs(&rx_tcb,RXC_PC_FS_TIMER_SIG);
      break;

    case RXCNCCCH_NOTIFY_FCCCH_GOOD:
      MSG_MED("Got RXCNCCCH_NOTIFY_FCCCH_GOOD", 0, 0, 0);

      /* Notify the failsafe of a valid FCCCH channel.
       * Reset the failsafe timer if BCCH is also presently valid.
       */
      rxcnccch_channel_failure_mask &= ~(RXCNCCCH_FCCCH_BIT);
      break;

    case RXCNCCCH_NOTIFY_FCCCH_BAD:
      MSG_MED("Got RXCNCCCH_NOTIFY_FCCCH_BAD", 0, 0, 0);

      /* Notify the failsafe of an invalid FCCCH channel */
      rxcnccch_channel_failure_mask |= (RXCNCCCH_FCCCH_BIT);
      break;

    case RXCNCCCH_NOTIFY_BCCH_GOOD:
      MSG_MED("Got RXCNCCCH_NOTIFY_BCCH_GOOD", 0, 0, 0);

      /* Notify the failsafe of a valid BCCH channel.
       * Reset the failsafe timer if FCCCH is also presently valid.
       */
      rxcnccch_channel_failure_mask &=
        ~(RXCNCCCH_PRIMARY_BCCH_BIT | RXCNCCCH_SECONDARY_BCCH_BIT);
      break;

    case RXCNCCCH_NOTIFY_BCCH_BAD:
      MSG_MED("Got RXCNCCCH_NOTIFY_BCCH_BAD", 0, 0, 0);

      /* Notify the failsafe of an invalid BCCH channel */
      rxcnccch_channel_failure_mask |=
        (RXCNCCCH_PRIMARY_BCCH_BIT | RXCNCCCH_SECONDARY_BCCH_BIT);
      break;

    default:
      /* Should never get here */
      MSG_ERROR("Bad cmd = %d", cmd, 0, 0);
      break;
  }

  /* Start (or reset) the failsafe timer if the following are true:
   * 1) The failsafe timer is currently running, and
   * 2) There are NO failures on any of the currently enabled NCCCH channels.
   */
  if (failsafe_started &&
      !(rxcnccch_chan_enable_bits & rxcnccch_channel_failure_mask)) {
    /* Start/Restart the failsafe timer */
    (void)rex_set_timer(&failsafe_timer,
                        RXCNCCCH_FAILSAFE_TIMER_MS);
  }

} /* rxcnccch_failsafe_timer() */

#endif /* FEATURE_IS2000_REL_A_CC_SMMC */





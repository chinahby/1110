/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              CDMA2000 Receive Task Idle State Processing.

GENERAL DESCRIPTION
  Contains implementation of idle state operations that are not specifically
  tied to a physical layer channel.  Paging Channel, BCCH, and FCCCH all use
  this common functionality.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcidle.c_v   1.15   23 Oct 2002 16:27:16   baon  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcidle.c#2 $ $DateTime: 2008/07/18 10:34:36 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Reduced debug string sizes for ULC effort.
08/02/06   trc     Add NCCCH failsafe timer support
07/20/06   bn      Check in the powercollapse code under FEATURE_FULL_POWER_COLLAPSE
07/25/05   rkc     Fixed lint errors.
06/16/04   sr      Merged in changes from 6500 5004 build
06/04/04   bt      Use srch_mux_get_curr_20ms_frame instead of directly
                   accessing symbol combiner.
04/20/04   jrp     Fixed lint warnings.
03/03/04   bn      Don't delete the FCCCH deint task when the next state is PC
11/04/03   bn      Improved the 5ms delay on FCCCH
10/22/03   sr      Fixed compilation warning errors when using RVCT compiler
07/31/03   bn      Removed the redundancy checking skip flag in the rxcidle_first_frame()
                   This fixes the wrong skipping frame when in simultaneously FCCCH/BCCH
06/16/03   bn      Fixed the hard-handoff problem when switching from NCCCH to PCH
05/22/03   vlc     Corrected typo for FEATURE_FACTORY_TESTMODE.
05/05/03   bn      Optimized the NCCCH logics and F3 debug messages
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
03/04/03   fc      Consolidated f-csh fade timer implementation.
02/25/03   va      Removed changing MC and SRCH priority by MC.
10/21/02   bn      Added support for NCC statistic counters
10/03/02   bn      Added support for Broadcast BCCH
10/14/02   sr      Changed FEATURE_IS2000_REL_A to ...REL_A_CC
                   in rxcidle_first_frame()
08/13/02   sr      Removed debug message
08/13/02   sr      Removed rxc_idle_chan_config(). New SRCH-RXC command
                   interface for fast RAHO replaces this.
06/03/02   sr      Resolved compile errors for non-REL_A builds
05/31/02   sr      Added rxcidle_first_frame() to check if we are in the first
                   frame after wakeup
02/15/02   lad     Added external routine rxc_idle_chan_config() for Fast RAHO.
                   Added rxcidle_chan_config().  Added rxcidle_pending_state()
                   for state transitions triggered in external context.
01/23/02   lad     Updated rxcidle_init() to only set up timer at the first
                   time.
01/17/02   lad     Created file.  Includes fade timer support.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "parm.h"
#include "rxcidle.h"
#include "rxc.h"
#include "rxccommon.h"

#include "cai.h"
#include "comdef.h"
#include "err.h"
#include "mccrx.h"
#include "mclog.h"
#include "msg.h"
#include "rex.h"
#include "rx.h"

#ifdef FEATURE_IS2000_REL_A_CC
#include "rxcbcch.h"
#include "rxcfccch.h"
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
#include "rxcnccch.h"
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
#endif /* FEATURE_IS2000_REL_A_CC */

#include "srch_mux.h"

#include "rxcpaging.h"
#include "txc.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif
#ifdef FEATURE_FULL_POWER_COLLAPSE
#error code not present
#endif

/*--------------------------------------------------------------------------
 * Definitions and Declarations
 *--------------------------------------------------------------------------*/
/* Fade timer information.  This structure is used to apply T30M and T40M.
   T72M is handled explicitly. */
static struct {
  rxcidle_fade_type curr_fade_type; /* Type of timer in use */

  /* Timer for amount of time since mobile last detected a valid SAT while
     communicating with the base station over the voice channel */
  rex_timer_type timer;

  /* Duration of 'timer' */
  word timer_duration;

  /* AHO support */
  /* This timer is used for T72M timer monitoring */
  rex_timer_type t72_timer;

  /* Transmitter status */
  boolean tx_disabled;
} rxcidle_fade_info;

#ifdef FEATURE_IS2000_REL_A_CC
static rxcidle_slotted_status_type
         rxcidle_slotted_status = RXCIDLE_SLOTTED_AWAKE;
#endif /* FEATURE_IS2000_REL_A_CC */

#ifdef FEATURE_IS2000_REL_A_CC_SMMC
static byte rxcidle_ncc_setup_chan_bit = 0;
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */


#ifdef FEATURE_FULL_POWER_COLLAPSE
#error code not present
#endif

/*--------------------------------------------------------------------------
 * Internal Function Prototypes
 *--------------------------------------------------------------------------*/
static void rxcidle_rpt (mccrx_rpt_name_type);
static void rxcidle_config_tx (boolean);

/*--------------------------------------------------------------------------
 * Function Implementations
 *--------------------------------------------------------------------------*/
/*===========================================================================*/
void
rxcidle_def_timer (void)
{
  rex_def_timer (&rxcidle_fade_info.timer, &rx_tcb, RXC_FADE_TIMER_SIG);
  rex_def_timer (&rxcidle_fade_info.t72_timer, &rx_tcb, RXC_AHO_FADE_TIMER_SIG);
}

/*===========================================================================*/
void
rxcidle_init (void)
{
  rxcidle_fade_info.curr_fade_type = RXCIDLE_IDLE_FADE;
  rxcidle_fade_info.tx_disabled = FALSE;
  rxcidle_fade_info.timer_duration = 0;
}

/*===========================================================================*/
rex_sigs_type
rxcidle_wait_mask (void)
{
  rex_sigs_type mask = 0;

  switch (rxcidle_fade_info.curr_fade_type) {
    case RXCIDLE_ACCESS_FADE:
      mask = RXC_AHO_FADE_TIMER_SIG | RXC_FADE_TIMER_SIG;
      break;
    case RXCIDLE_IDLE_FADE:
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
      mask = RXC_FADE_TIMER_SIG | RXC_PC_FS_TIMER_SIG;
#else /* else if !FEATURE_IS2000_REL_A_CC_SMMC */
      mask = RXC_FADE_TIMER_SIG;
#endif /* !FEATURE_IS2000_REL_A_CC_SMMC */
      break;
    default:
      mask = 0;
  }

  return mask;
}


/*===========================================================================*/
void
rxcidle_set_fade_timer (rxcidle_fade_type fade_type)
{

#ifdef FEATURE_FACTORY_TESTMODE
  if (ftm_mode == DMSS_MODE)
#endif /* FEATURE_FACTORY_TESTMODE */
  {
    if (fade_type == RXCIDLE_CURR_FADE)
    {
      fade_type = rxcidle_fade_info.curr_fade_type;
    }
    else if (rxcidle_fade_info.curr_fade_type != fade_type)
    {
      /* Changing timer types, clear existing timer and set new type */
      rxcidle_clr_fade_timer();
      rxcidle_fade_info.curr_fade_type = fade_type;
    }

    switch (fade_type)
    {
      case RXCIDLE_IDLE_FADE:
        rxcidle_fade_info.timer_duration = cai_tmo.t30m;
        break;

      case RXCIDLE_ACCESS_FADE:

        /* If TX was disabled, and we are now setting the timer, TX will
           no longer be disabled.  Therefore, send a message to MC to
           notify that a valid message was received after T72M expired. */
        if (rxcidle_fade_info.tx_disabled)
        {
          rxcidle_rpt (RXC_VALID_PC_MSG_R);
        }

        if (P_REV_IS_4_OR_GREATER)
        {
          /* Access fade uses both T40M (3 secs) and T72M (1 sec) */
          rxcidle_fade_info.timer_duration = cai_tmo.t40m;

          /* Enable the transmitter */
          rxcidle_config_tx (TRUE);

          (void) rex_set_timer(&rxcidle_fade_info.t72_timer, cai_tmo.t72m);
        }
        else
        {
          /* Access fade uses only T40M (1 sec) */
          rxcidle_fade_info.timer_duration = 1000;
        }
        break;

     default:
       rxcidle_fade_info.timer_duration = 0;
       break;
    }

    if (rxcidle_fade_info.timer_duration > 0)
    {
      (void) rex_set_timer(&rxcidle_fade_info.timer,
                           rxcidle_fade_info.timer_duration);
    }
  }
}

/*===========================================================================*/
void
rxcidle_clr_fade_timer (void)
{
  switch (rxcidle_fade_info.curr_fade_type) {
    case RXCIDLE_ACCESS_FADE:
      (void) rex_clr_timer (&rxcidle_fade_info.t72_timer);
      (void) rex_clr_sigs (&rx_tcb, RXC_AHO_FADE_TIMER_SIG);
      /* Fall through for regular fade timer */

    case RXCIDLE_IDLE_FADE:
      (void) rex_clr_timer (&rxcidle_fade_info.timer);
      (void) rex_clr_sigs (&rx_tcb, RXC_FADE_TIMER_SIG);
      break;

    case RXCIDLE_CURR_FADE:
      /* Do nothing */
      break;
  }
}

/*===========================================================================*/
boolean
rxcidle_process_sigs (rex_sigs_type sigs, rxc_state_type state)
{
  boolean OK_to_continue = TRUE;
  cai_timer_id_type timer_id = CAI_TIMER_T30M;
  mccrx_rpt_name_type rpt = RXC_PC_T30M_TMO_R;

  /* wait for command or interrupt or fade timer to expire */
  if (sigs & RXC_FADE_TIMER_SIG) {
    (void) rex_clr_sigs (&rx_tcb, RXC_FADE_TIMER_SIG);

    switch (rxcidle_fade_info.curr_fade_type) {
      case RXCIDLE_IDLE_FADE: /* Values set by default */
        MSG_MED ("T30M exp", 0, 0, 0);
        /* These values are set by default. */
        break;

      case RXCIDLE_ACCESS_FADE:
        MSG_MED ("T40M exp", 0, 0, 0);
        rpt = RXC_PC_T40M_TMO_R;
        timer_id = CAI_TIMER_T40M;
        break;

      default:
        /* Invalid state */
        break;
    }
    if ((rxcidle_fade_info.curr_fade_type == RXCIDLE_IDLE_FADE) ||
        (rxcidle_fade_info.curr_fade_type == RXCIDLE_ACCESS_FADE) )
    {
      if (state == RXC_PC_STATE)
      {
        /* Increment he counter which keeps track the number of time that the
           MS declared a loss of PCH channel */
        (void) parm_inc(CAI_PAG_6_ID, 1);
      }
#ifdef FEATURE_IS2000_REL_A_CC
      else if (state == RXC_BCCH_STATE)
      {
        /* increase the counter keeps track the number of time that the
        MS declared a loss of BCCH channel */
        (void) parm_inc(CAI_BCCH_5_ID, 1);
      }
      else
      {
        /* increase the counter keeps track the number of time that the
        MS declared a loss of FCCCH channel */
        (void) parm_inc(CAI_FCCCH_4_ID, 1);
      }
#endif /* FEATURE_IS2000_REL_A_CC */
    }

//XXX This looks like some hack in rxcpaging.c.  I wonder why...
//    rxcidle_fade_info.tx_disabled = FALSE;
//    -or-
//    rxcidle_config_tx (TRUE);

    /* Report timer expiration to upper layer. */
    rxcidle_rpt (rpt);

    /* Event report: Timer expired. */
    mclog_report_event_timer (timer_id);
  }

  if (sigs & RXC_AHO_FADE_TIMER_SIG) {
    (void) rex_clr_sigs (&rx_tcb, RXC_AHO_FADE_TIMER_SIG);

    /* ------------------------------------------------------------
    ** T72M timer has expired, Disable the transmitter and suspend
    ** the probe transmission.  Send a T72M Fade report to MC task.
    ** -----------------------------------------------------------*/
    mclog_report_event_timer (CAI_TIMER_T72M);

    /* Disable the transmitter */
    rxcidle_config_tx (FALSE);

    rxcidle_rpt (RXC_PC_T72M_TMO_R);

    OK_to_continue = FALSE; /* This timer expiration is a show stopper */
  }

#ifdef FEATURE_IS2000_REL_A_CC_SMMC
  if (sigs & RXC_PC_FS_TIMER_SIG) {
    /* Clear the sig */
    (void) rex_clr_sigs (&rx_tcb, RXC_PC_FS_TIMER_SIG);

    /* Emit a message for the log */
    MSG_HIGH("NCCCH FS timer exp",0,0,0);

    /* Report failsafe timer expiration to upper layer. */
    rxcidle_rpt(RXC_PC_FS_TMO_R);
  }
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

  return OK_to_continue;

}

/*===========================================================================

FUNCTION RXCIDLE_RPT

DESCRIPTION
  Sends a report to the upper layer (MC).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void
rxcidle_rpt (mccrx_rpt_name_type rpt_name)
{
  mccrx_rpt_type *rpt_ptr = NULL;

  if ((rpt_ptr = (mccrx_rpt_type *) q_get (&mcc_rx_free_q)) != NULL) {
    rpt_ptr->hdr.rpt = rpt_name;
    rxc_mc_rpt (rpt_ptr);
  } else {
    ERR ("0 bufs on mcc_rx_free_q", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION RXCIDLE_CONFIG_TX

DESCRIPTION
  This function enables/disables transmitter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void
rxcidle_config_tx (boolean enable)
{
  boolean suspend = (enable) ? FALSE : TRUE;

  txc_suspend_tx (suspend);
  rxcidle_fade_info.tx_disabled = suspend;
}

/*===========================================================================

FUNCTION RXCIDLE_FIRST_FRAME

DESCRIPTION
  This function determines whether it is the first frame after wake
  up

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is the first frame; FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxcidle_first_frame( rxc_chan_type chan )
{
  boolean result = FALSE;
  srch_mux_20ms_frame_type frame_20ms;

  frame_20ms = srch_mux_get_curr_20ms_frame();

  if(frame_20ms == FRAME_NOT_AVAILABLE)
  {
    /* no frame information available */
    result = FALSE;
  }
  else
  {
#ifdef FEATURE_IS2000_REL_A_CC
    if( ((chan == RXC_CDMA_BCCH) &&
         ((frame_20ms == FRAME_40_MS) || (frame_20ms == FRAME_60_MS))) ||
    ( ((chan == RXC_CDMA_FCCCH) || (chan == RXC_CDMA_PC) ) &&
         (frame_20ms == FRAME_20_MS)))
#else
    if((chan == RXC_CDMA_PC) && (frame_20ms == FRAME_20_MS))
#endif /* FEATURE_IS2000_REL_A_CC */
    {
      /* Decoder Interrupt occured between 20.0ms and 39.9ms.(PCH/FCCCH) or
       * between 40ms and 79.9ms (BCCH)
       * This must be for the frame from 0ms to 20ms. (40ms for BCCH) */

      result = TRUE;
      /* Don't skip frames anymore. */
    }
    else
    {
      result = FALSE;
    }
  }

  return( result );
}

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================*/
void
rxc_idle_activate (void)
{
  /* This routine is being called because we need to installer the decoder
     ISR pronto.  No time to send a command to RXC task. */
  switch (rxc_get_state ()) {
    case RXC_PC_STATE:
      rxc_set_pc_isr ();
      break;

    case RXC_BCCH_STATE:
      rxcbcch_activate ();
      break;

    case RXC_FCCCH_STATE:
      rxcfccch_activate ();
      break;
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
    case RXC_NCCCH_STATE:
      rxcnccch_activate ();
      break;
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

    default:
      MSG_LOW("Bad state for idle state act", 0, 0, 0);
      break;
  }
}

/*===========================================================================*/
rxc_state_type
rxcidle_chan_config (rxc_idle_chan_info_type *config)
{
  rxc_state_type curr_state, next_state;

  next_state = curr_state = rxc_get_state ();

  if (config)
  {
    switch (config->chan_id)
    {
      case CAI_BCCH: //Is this valid from PC?
        /* this will initialize either primary BCCH or secondary BCCH */
        MSG_HIGH("RXC Idle CHAN CONFIG,for BCCH ",0,0,0);
        if (rxcbcch_init (curr_state, &config->config.bcch, config->ebcsms))
        {
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
          if (config->ebcsms)
          {
            rxcidle_ncc_setup_chan_bit =
                   rxcidle_ncc_setup_chan_bit | RXCNCCCH_SECONDARY_BCCH_BIT;
            next_state = RXC_NCCCH_STATE;
          }
          else
          {
            rxcidle_ncc_setup_chan_bit =
                   rxcidle_ncc_setup_chan_bit | RXCNCCCH_PRIMARY_BCCH_BIT;
            next_state = RXC_BCCH_STATE;
          }
#else
          next_state = RXC_BCCH_STATE;
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
        }
        else
        {
          MSG_ERROR("config BCCH fail",0,0,0);
        }
        break;

      case CAI_F_CCCH:
        MSG_HIGH("RXC Idle CHAN CONFIG,for FCCCH ",0,0,0);
        if (rxcfccch_init (curr_state, &config->config.fccch))
        {
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
          rxcidle_ncc_setup_chan_bit = rxcidle_ncc_setup_chan_bit | RXCNCCCH_FCCCH_BIT;
          /* if this feature is turned on then we know that, when we bring up
          FCCCH, there is a chance we could monitor both FCCCH and BCCH, so we
          want this in NCCCH_STATE */
          next_state = RXC_NCCCH_STATE;
#else
          next_state = RXC_FCCCH_STATE;
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
        }
#ifdef FEATURE_FULL_POWER_COLLAPSE
#error code not present
#endif
        break;

      case CAI_PCH:
        MSG_HIGH("RXC Idle CHAN CONFIG,for PCH ",0,0,0);
        rxc_pc_init (curr_state, config->config.pch.rate);
        next_state = RXC_PC_STATE;
        /* just store it for power collapse */
#ifdef FEATURE_FULL_POWER_COLLAPSE
#error code not present
#endif
        break;

      case CAI_SYNCH:
      case CAI_F_CACH:
      case CAI_F_FCH:
      case CAI_F_DCCH:
        MSG_HIGH("RXC Idle CHAN CONFIG,for ??? ",0,0,0);
        break;

    }
  }

  return next_state;
}
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
rxc_state_type rxcidle_nccchan_config( rxc_chan_config_cmd_type *chan_config)
{
  rxc_state_type next_state = rxc_get_state();
  byte index;
  rxcidle_ncc_setup_chan_bit = 0;
  for (index = 0; index < chan_config->num_chan; index++)
  {
     next_state = rxcidle_chan_config(&chan_config->chan_info[index]);
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
     if(chan_config->chan_info[index].ebcsms)
     {
       rxc_slot.skipping_bcch = chan_config->skip_frame;
     }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
  }
  /* for now we always return RXC_NCCCH_STATE */
  /* check to see if we need to turn off any channel */
  if (((rxcidle_ncc_setup_chan_bit & RXCNCCCH_SECONDARY_BCCH_BIT) == FALSE) &&
      ((rxcidle_ncc_setup_chan_bit & RXCNCCCH_PRIMARY_BCCH_BIT) == FALSE))
  {
    /* turn off BCCH */
    if ((rxcnccch_chan_enable_bits & RXCNCCCH_PRIMARY_BCCH_BIT) ||
        (rxcnccch_chan_enable_bits & RXCNCCCH_SECONDARY_BCCH_BIT))
    {
        /* delete the deinterleave task for BCCH, in the second 5 ms slot */
        deint_delete_nccch_task_list(RXC_BCCH_DEINT_TASK_SLOT_NUMBER);
      rxcbcch_exit();
    }
  }
  if( (rxcnccch_chan_enable_bits & RXCNCCCH_FCCCH_BIT) &&
      ((rxcidle_ncc_setup_chan_bit & RXCNCCCH_FCCCH_BIT) == FALSE))
  {
     /* check if MS switchs back to paging, then don't delete the
     deinterleave task in the first 5 ms slot since it is programed
     for paging channel */
     if (next_state != RXC_PC_STATE)
     {
       /* delete the deinterleave task for FCCCH, in the first 5 ms slot */
       deint_delete_nccch_task_list(RXC_FCCCH_DEINT_TASK_SLOT_NUMBER);
     }
     /* turn of primary FCCCH */
     rxcfccch_exit();
  }
  /* update the channel bit map */
  rxcnccch_chan_enable_bits = rxcidle_ncc_setup_chan_bit;
  if (next_state != RXC_PC_STATE)
  {
    next_state = RXC_NCCCH_STATE;
  }
  return(next_state);
}
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
/*===========================================================================

FUNCTION RXCIDLE_GET_SLOTTED_STATUS

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
rxcidle_slotted_status_type rxcidle_get_slotted_status( void )
{
  return rxcidle_slotted_status;
}

/*===========================================================================

FUNCTION RXCIDLE_SET_SLOTTED_STATUS

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void rxcidle_set_slotted_status( rxcidle_slotted_status_type status )
{
  rxcidle_slotted_status = status;
}
#endif /* FEATURE_IS2000_REL_A_CC */


#ifdef FEATURE_FULL_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_FULL_POWER_COLLAPSE */



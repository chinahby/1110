/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                C D M A    P A G I N G    M O D U L E

GENERAL DESCRIPTION
  Contains items associated with the paging channel

EXTERNALIZED FUNCTIONS
  rxc_pc_init - initializes the Paging Channel processing state
  rxc_pc - Paging Channel state processing
  rxc_pc_isr - interrupt handler for the Decoder interrupt used
  when the Receive task is processing the Paging Channel

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1990 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxcpaging.c_v   1.34   06 Sep 2002 16:11:38   cryan  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcpaging.c#3 $ $DateTime: 2008/07/28 09:31:07 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/08   jtm     Corrected the call to rxc_configure_ffpc_channel().
07/16/08   jtm     Removed unused variables. Added wrapper function for common
                   debug strings. Reduced debug string sizes. Added debug strings
                   for monitoring commands. Replaced block of code in 
                   rxc_vote_not_ok_to_sleep() with rxc_configure_ffpc_channel() 
                   since it is duplicated elsewhere.
05/03/07   cab     Moved to new sleep voting interface
02/23/07   vlc     Fixed compiler warnings/errors when SCH, Rel A, and P2 
                   features are turned off.
10/30/06   bn      Added code to support DO to 1x handoff
06/09/06   trc     Remove T_MSM6700 featurization
05/08/06   bn      Added logic to support the case if receiving slot_mask = 0 in rude wakeup
01/20/06   fh      Added the support for AKA and MI.
03/31/06   bn      Added call to the function to map the deint RAM in rxc_pc_init()
03/22/06   kwo     Added calls into srch to report frame stats
03/22/06   rkc     Fix debug printing for OOS after slot+1.
01/12/06   rkc     OOS Enhancement: In addition to the normal SLOT_R, send MC
                   a SLOT_R after (slot+1) if a good message CRC after a bad
                   CRC in (slot) and (slot+1).
09/27/05   rkc     In rxc_log_pc_msg(), check if log_ptr != NULL before logging.
09/26/05   bn      Modifed the condition to send out decoder error message
09/08/05   ejv     Use new srch_mux interface calls.
09/07/05   rkc     Mainline FEATURE_ZZ2.
08/29/05   bn      Clear the all the decoder status it after installing the isr
08/09/05   bn      Added logic to indicate the page matched event to MC
07/25/05   rkc     Fixed lint errors.
07/05/05   bn      Fixed the page match logic in MUX
05/04/05   kwo     srch_idle_page_match() -> srch_mux_page_match()
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
04/08/05   fc/vlc  Added support for FEATURE_IS2000_REL_D TCI to IDLE state
                   transition (for DCA - Direct Channel Assignment).
03/14/05   bn      Instead of SRCH, MUX need to install the ISR after WAKE_F
02/24/05   vlc     Lock interrupt between clearing DEC_DONE_INT_STATUS and
                   installing RXC ISR.
02/23/05   vlc     Clear the DEC_DONE_INT_STATUS when setting RXC ISR.
01/26/05   vlc     Removed obsolete ds.h inclusion.
06/24/04   sr      Added support for 2nd level dec_done interrupt in ISR
04/20/04   jrp     Fixed lint warnings.
04/14/04   sr      Removed unused ZIF logging code
03/05/04   sr/bkm  Moved initialization of frame offset from MC to RXC_TCI_F
11/24/03   vlc     Fixed compilation warnings for RVCT compiler.
10/21/03   bn      Added support for paging statistic counters
10/20/03   bn      Reset the DCCH,FCH data valid when entering TCI state
09/09/03   dna     Merged porting changes for MSM6500
07/18/03   vlc     Release A changes:  Changed FCH and DCCH
                   channel set up to better indicate when decoder data is
                   available for the appropriate channel(s) for the RXC layer.
06/16/03   bn      Fixed the "out of buffer dec_free_q" problem. Also fixed the
                   hard-handoff problem when switching from NCCCH to PCH
05/28/03   vlc     Added support for FEATURE_IS2000_REL_B traffic to idle 
                   transition.
05/05/03   bn      Allowed transition from NCCCH to PCH
03/24/03   sr/bn   CHAN_CONFIG_F is now outside the sync channel workaround
                   FEATURE. We ignore it if we get it from SRCH in PC state.
03/05/03   fc      Consolidated f-csh fade timer implementation.
           sr      Added support for Sync Channel workaround.
02/25/03   va      Removed changing MC and SRCH priority by MC. 
02/07/03   lcc/sr  Changed flush count from 8 to 4 in case of idle handoff
01/30/03   bt      Changed FAST_SLEEP_CMD for FEATURE_ZZ2.
08/06/02   cr      Added MSM6100 changes.
08/14/02   sr      Removed #define MAX_SKIPPED_FRAMES
08/13/02   sr      Added Fast RAHO support
06/03/02   sr      Changed CAIX_PC to RXC_CDMA_PC
05/31/02   lcc     Corrected programming of reverse power control delay to be
                   like MSM6050.
05/31/02   sr      Call to function rxcidle_first_frame() instead of inline
                   code that checks symbol combiner position to determine if
                   it is the first frame after wake up
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
                   Implemented comments from VP2 code review.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up 
                   (feature FEATURE_IS2000_REL_A_VP2.)
03/05/02   ems     Merged in changes for MSM6050. Fixed shifting for setting 
                   of MASK_DELAY in AGC_CTL8.  Added zif logging code.
02/27/02   lad     Merged IS2000 Releas A Common Channels:
                   Replaced inline code with rxc_clear_decoder_queue().
                   Moved rxc ATMR allocation to rxctraffic.c.
                   Moved TCI code to rxc_tci_init().
01/14/02   bgc     Added fixes for power control delay.
01/12/02   sst     Modified to use the new srch_* files that replaced srchdrv.c
                     Includes a function name change:
                       srch_get_vsc_time -> srch_scomb_get_vsc_time
11/16/01   vlc     Added call to dec_clk_select () before switching rxc state.
11/13/01   lh      Removed extra increment of CAI_PAG_4_ID.
11/13/01   bn      Added the enhance DCCH DTX detection and channel supervision logics
10/02/01   ejv     Added new SLEEP macros.
09/20/01   lcc     Moved call to rxc_init_action_item to rxctraffic.c inside
                   rxc_tci_init.
08/22/01   bn      Support DCCH in FTM mode
06/15/01   day     Mainlined FEATURE_FFPC, FEATURE_IS95B, FEATURE_TMSI, FEATURE_IS2000
                   Delete FEATURE_SPECIAL_MDR
06/11/01   fc      Changes to support split of fade reports between idle and
                   system access substates.
04/20/01   sr      Merged in from MSM5100 archive
04/19/01   tc      Merged with MSM_MUX1X.01.00.01
           lcc     Corrected problem initializing to the wrong RC in rxc_pc when
                   RXC_TCI_F is received.
           fc      Renamed mcc_report_event_timer to mclog_report_event_timer.
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           bgc     Added FEATURE_IS2000_P2 for DCCH.
03/27/01   vlc     Merged MSM_MUX1X.00.00.17.
           fc      Fixed timer ID for T40m.
           fc      Added support of events report: Message received and Timer
                   expired.
03/07/01   ph      Passing the parser encryption status on to MC when a
                   message is received on f-csch (PCH, BCCH, F-CCCH)
03/07/01   va      Added support for populating p_rev_in_use info during TC init.
03/02/01   tc      Merged MSM_MUX1X.00.00.15.
02/21/01   sr      Fixes for paging channel loss problem in rxc_pc_init()
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime switch.
01/18/01   ejv     Modified some combiner macros to match new names.
01/12/01   tc      Merged MSM_MUX1X.00.00.06.
11/23/00   tc      Fixed MSM5100 PLT ftm_pc_msg_sent, ftm_rxc_pc_cmd.
12/07/00   bgc     Commented out some unused PLT code.
11/29/00   jq      Moved initialization of rxc_slot.slot out of BROADCAST_SMS.
11/13/00   bgc     Moved FTM paging flags to rxcpaging.c
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
#include "qw.h"
#include "rex.h"
#include "bit.h"
#include "queue.h"
#include "task.h"
#include "err.h"
#include "log.h"
#include "voc.h"
#include "crc.h"
#include "enc.h"
#include "cmd.h"
#include "rxc.h"
#include "mccdma.h"
#include "parm.h"
#include "rxtx.h"
#include "msg.h"
#include "dog.h"
#include "tramp.h"
#include "mccrx.h"
#include "ts.h"
#include "cai.h"
#include "caix.h"
#include "rx.h"
#include "tx.h"
#include "mccrxtx.h"

#include "hw.h"
#include "deint.h"
#include "clkregim.h"
#include "msm_drv.h"
#include "atmr.h"
#include "ffpc.h"
#include "muxmsm.h"

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
#endif

#include "rxccommon.h"
#include "rxctraffic.h"
#include "rxcsync.h"
#include "rxcrda.h"
#include "rxcdemux.h"
#include "rxcpaging.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif
#include "mclog.h"

#include "rxcidle.h"

#include "srch_mux.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_IS2000_REL_A_CC
extern boolean raho_flag;
#endif


/* number of Paging Channel messages with good CRCs */
dword rxc_pc_crc_good = 0;

/* number of Paging Channel messages with bad CRCs */
dword rxc_pc_crc_bad = 0;

/* MUX paging statistic counter for DIAG */
word rxc_gpm_page_matched_counter = 0;
word rxc_spm_page_matched_counter = 0;
word rxc_srch_rssi_negative_counter = 0;
boolean rxc_page_increase_page_matched_cnt = FALSE;

#ifdef FEATURE_FACTORY_TESTMODE
  boolean ftm_pc_msg_sent = FALSE;
  /* Flag indicating if a message has been sent to TEST_MCC. The message
     notifies PLT_MCC that RXC has received a Paging Channel message */

#endif /*FEATURE_FACTORY_TESTMODE*/

/*-------------------------------------------------------------------------*/
/* Paging channel State Variables                                          */
/*-------------------------------------------------------------------------*/

/* States of Paging Channel parsing state machine */
typedef enum
{
  PC_HUNT_STATE,
  PC_LEN_STATE,
  PC_MSG_STATE,
  PC_FLUSH_STATE,
  PC_SKIP_STATE
}rxc_pc_state_type;


struct
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*         TASK VARIABLES                                                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qword msg_frame;
    /* frame in which current msg was received */
  word msg_state;
    /* state in which current msg was received */
  byte   page_frm[ DEC_192_BITS / 8 ];
    /* Paging Channel frame read from Decoder */
  byte   page_blk[ DEC_192_BITS / 8 ];
    /* reconstructed over-the-air Paging Channel frame */
  word pc_bits;
    /* Number of bits in a Paging Channel frame */
  word parse_bits;
    /* Number of bits to send to rxc_pc_parse */
  word parse_index;
    /* Bit index of next paging channel sub-frame to parse */
  word parse_start;
    /* parse_index reset value on (waking up after a sleep) */
  word bc1;
  /* Parameter for bcopy procedure used in rxc_pc_data */
  word bc2;
  /* Parameter for bcopy procedure used in rxc_pc_data */
  rxc_pc_state_type  state;
  /* State of Paging Channel state machine */
  word  msg_inx;    /* Index into message buffer */
  word  msg_len;    /* total number of bits in message */
  word  err_cnt;    /* Number of consecutive messages with bad CRCs */
  rxtx_rx_msg_type *msg_ptr;
    /* pointer to buffer to place received message for Layer 2 task */
  word flush_cnt;    /* number of frames remaining to be flushed - used for
                        idle handoff */
  byte half_frames;  /* number of half frames which contained part of the
                        current message */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*         INTERRUPT HANDLER VARIABLES                                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  word pc_rate;
    /* Paging channel data rate */
} rxc_pc_var;


dec_pc_status_type rxc_pc_status = {0, 0, 0};

#define RXC_MAX_INT_CNT    3    /* Max count for timing ints after wakeup */
#define RXC_SLOT_INT_CNT   2    /* Count at which our slot will arrive */

word rxc_last_pc_ser=0;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION RXC_EMPTY_GPM_AND_DONE

DESCRIPTION
  This function performs an empty page match routine on General Page Message.
  It is called from the Paging Channel Decoder interrupt handler during slotted
  mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If it is an empty General Page Message that indicates no more page
        for this mobile in the current slot.
  FALSE: Otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean rxc_empty_gpm_and_done( dec_pc_type bits )
{
  /* Perform empty general page match with 72-bit Decoder delay */
  if ( (bits[9] == 0x84 ) &&
       (bits[10] == 0x88 ) &&
       ( (bits[11] & 0x80) == 0x80 ) &&
           /* The above pattern match ensures that
              1. SOM (SCI) bit is set to 1,
              2. MSG_LEN is 9-bytes for an empty General Page Message, and
              3. MSG_TYPE is CAI_GENERAL_PAGE_MSG, which is 0x11.
            */
       (((bits[11] & 0x7E) >> 1) == mcc_get_cur_msg_seq() ) &&
           /* current CFG_MSG_SEQ */
       ((cdma.assigning_tmsi.info.tmsi_code == INVALID_TMSI_CODE) ||
        (bits[12] & 0x01) == 0x01) &&   /* TMSI not assigned or TMSI_DONE */

       (
          ((cdma.imsi_addr_num != IMSI_CLASS_0_ADDR_NUM) &&
           ((bits[12] & 0x02) == 0x02)) ||
          ((cdma.imsi_addr_num == IMSI_CLASS_0_ADDR_NUM) &&
           ((bits[12] & 0x04) == 0x04))
       )      /* (MS has a class 1 IMSI and CLASS_1_DONE) or
                 (MS has a class 0 IMSI and CLASS_0_DONE)
               */
              /* Removed feature definition for FEATURE_CLASS_1_IMSI to embed the code */
#ifdef FEATURE_BROADCAST_SMS
       && ( mcc_bc_sms_enable ? (bits[13] & 0x40) : 1)
                 /* Check for BROADCAST_DONE */
#endif /* FEATURE_BROADCAST_SMS */
    )
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
} /* rxc_empty_gpm_and_done */



/*===========================================================================

FUNCTION RXC_EMPTY_SPM_AND_DONE

DESCRIPTION
  This function performs an empty page match routine on Slotted Page Message.
  It is called from the Paging Channel Decoder interrupt handler during slotted
  mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If it is an empty Slotted Page Message that indicates no more page
        for this mobile in the current slot.
  FALSE: Otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean rxc_empty_spm_and_done( dec_pc_type bits)
{
  /* Perform empty general page match with 72-bit Decoder delay */
  if ( (bits[9] == 0x84 ) &&
       (bits[10] == 0x02 ) &&
       ((bits[11] & 0x80) == 0x80 ) &&
           /* The above pattern match ensures that
              1. SOM (SCI) bit is set to 1,
              2. MSG_LEN is 8-bytes for an empty Slotted Page Message, and
              3. MSG_TYPE is CAI_SLT_PAGE_MSG, which is 0x05.
            */
        (((bits[11] & 0x7E) >> 1) == mcc_get_cur_msg_seq()) &&
                                        /* current CFG_MSG_SEQ */
        ((bits[12] & 0x04) == 0x00)     /* No More Pages */
      )
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
} /* rxc_empty_spm_and_done */


/*===========================================================================

FUNCTION RXC_PC_ISR

DESCRIPTION
  This function is the interrupt handler for the Decoder interrupt used
  when the Receive task is processing the Paging Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  rxc_pc_isr ( void )
{
  rxc_dec_type *buf_ptr;
    /* pointer to buffer to be placed on rxc_dec_q */
  rxc_isr_cmd_type *cmd_ptr;
    /* pointer to command received from the task */
  boolean receive_slot_mask_zero = FALSE;
  boolean first_frame = FALSE;
    /* Set to true after wakeup from sleep on the first frame of the slot */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

  uint32 dec_int_status;

  dec_int_status = HWIO_IN(DEC_DONE_INT_STATUS);
  HWIO_OUT(DEC_DONE_INT_STATUS, dec_int_status);
  if(dec_int_status != 0x2)
  {
    /* when MUX got an dec int, the status bits will be none-zero. If the status bit 
     is zero, then this must be the very first time MUX configs the decoder. We won't flag this
     as an error. We will flag error when we expect SVB bit, but we got none-SVB bit 
    */
    if (dec_int_status > 0)
    {
      MSG_ERROR("DEC Done Int, but not SVD %d", dec_int_status, 0, 0);
    }
    return;
  }

  /* get frame number if interrupts have just settled down */
  ts_get_20ms_frame_time ( rxc_dec_frame );
  /* During ISR of a paging channel (which operates in continuous mode), 
   * the frame number being decoded is 2 less than the current system
   * time's frame number.  This is due to the fact that the 20ms_sys_time - 1
   * is containing the end of the frame before that. */
  qw_dec( rxc_dec_frame, 2L );

  /* Check to see if there are pending commands for the interrupt handler */
  while( (cmd_ptr = (rxc_isr_cmd_type *)q_get(&rxc_isr_q)) != NULL )
  {
    /* process command */
    switch( cmd_ptr->cmd )
    {
      case RXC_SC_F:
      {
        MSG_MED ("Got RXC_SC_F",0,0,0);

        /* switch to Sync channel */
        deint_set_sc_mode();
        enc_set_mode( ENC_SC_MODE );

        /* Decoder clk is CHIPX8 in sync state */
        dec_clk_select (DEC_CLK_CHIPX8);
        INTLOCK ();
        tramp_set_isr( TRAMP_RX_ISR, rxc_sc_isr);
        /* Clear DEC DONE int status to make sure ISR is properly serviced.
        */
        HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
        INTFREE ();
        break;
      }

      case RXC_TC_F:
      {
        MSG_MED ("Got RXC_TC_F",0,0,0);

#ifdef FEATURE_IS2000_REL_A_CC
        /* Always returns TRUE.  Ignoring return value. */
        (void) rxctraffic_frame_bnd_init ();
#else
        /* switch to Traffic Channel and use initial rate set */
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
        if ( rxc_so.for_dcch.included )
        {
          rxc_set_dcch_mode (rxc_so.for_dcch.rc,
                             (ffpc_channel == CAI_FPC_PRI_CHAN_DCCH)
                            );
        }
        if ( rxc_so.for_fch.included )
        {
          rxc_set_fch_mode  (rxc_so.for_fch.rc,
                             (ffpc_channel == CAI_FPC_PRI_CHAN_FCH)
                            );
        }
#else
        #ifdef FEATURE_IS2000_P2
        if ( rxc_so.for_dcch.included )
        {
          // Note: for MSM5000, FCH and DCCH shares the same hardware
          deint_set_fch_mode( (deint_rc_type)rxc_so.for_dcch.rc );
        }
        if ( rxc_so.for_fch.included )
        #endif // FEATURE_IS2000_P2
        {
          deint_set_fch_mode( (deint_rc_type)rxc_so.for_fch.rc );
        }
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

        /* increment frame count */
        qw_inc( rxc_dec_frame, 1L );

        /* Decoder clk is TCXO in traffic state */
        dec_clk_select (DEC_CLK_TCXO);
        INTLOCK ();
        tramp_set_isr( TRAMP_RX_ISR, rxc_tc_isr);
        /* Clear DEC DONE int status to make sure ISR is properly serviced.
        */
        HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
        INTFREE ();
#endif /* FEATURE_IS2000_REL_A_CC */
        break;
      }

      case RXC_WAKE_F:
      {
        qword tmp_qw;

        MSG_MED ("Got RXC_WAKE_F",0,0,0);

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
              - ( word )(qw_lo( rxc_dec_frame ) & 03);//lint !e734 Loss of precision (assignment) (18 bits to 16 bits)
          if (rxc_slot.slot_mask == 0)
          {
            /* in the case of rule wakeup, MC can send MUX a slot mask = 0, for such case, MUX 
            should treat it as a PAGE_SLOT, but without a checking for page match */
            receive_slot_mask_zero = TRUE;
            rxc_slot.slot_mask = PAGE_SLOT;
            rxc_slot.valid_msg_mask = rxc_slot.valid_msg_mask | rxc_slot.slot_mask;
            /* Page match is not available */
            rxc_slot.page_match = FALSE;
            MSG_HIGH("MUX got slot msk = 0 from MC, treat as PC_SLOT",0,0,0);
          }
        }

#ifdef FEATURE_IS2000_REL_A_CC
        /* We may be completing teh WAKE_F from a previous FCCCH state
        ** (in case of Sync Channel workaround so make sure to turn on 
        ** page_match here, because FCCCH always turn off page_match
        */
        if (!receive_slot_mask_zero)
        {
        if( rxc_pc_var.pc_rate == CAI_PC_1_RATE )
        {
           /* Optional page match feature only available
              for 9600-baud paging channel right now */
           rxc_slot.page_match = TRUE;

        }
        else
        {
           /* Page match feature not available */
           rxc_slot.page_match = FALSE;
        }
        }
#endif /* FEATURE_IS2000_REL_A_CC */

#ifdef FEATURE_BROADCAST_SMS
        if (rxc_slot.slot_mask & BC_PAGE_SLOT)
        {
          /* set 2 slot timer for broadcast paging slot */
          rxc_slot.bcpg_slot_frames =
              4 * (rxc_slot.slot - rxc_slot.slot_now) + 8
              - ( word )(qw_lo( rxc_dec_frame ) & 03);//lint !e734 Loss of precision (assignment) (18 bits to 16 bits)
        }
        if (rxc_slot.slot_mask & BC_DATA_SLOT)
        {
          /* set 3 slot timer for broadcast SMS slot */
          rxc_slot.bcsms_slot_frames =
              4 * (rxc_slot.slot - rxc_slot.slot_now) + 12
              - ( word )(qw_lo( rxc_dec_frame ) & 03);//lint !e734 Loss of precision (assignment) (18 bits to 16 bits)
          /* no idle page match in the schedule broadcast slot */
          MSG_MED("Pg match OFF in BCSMS slot",0,0,0);
          rxc_slot.page_match = FALSE;
          rxc_slot.bc_started = FALSE;
        }
        MSG_MED("SlotFrames: Asgn=%d, BC=%d, SMS=%d",
                 rxc_slot.slot_frames,
                 rxc_slot.bcpg_slot_frames,
                 rxc_slot.bcsms_slot_frames);
#endif /* FEATURE_BROADCAST_SMS */

        rxc_slot.active = TRUE;

        rxc_slot.after_slot_boundary = FALSE;

        /* Set countdown to re-initialize frame count after interrupt
         * timing has setlled down as we have just slammed and reprogrammed
         * the decoder mode.
         */
        rxc_time_cnt = 3;

#ifdef FEATURE_IS2000_REL_A_CC
        raho_flag = FALSE;
#endif

        break;
      }

      default:
      {
        /* illegal command */
        ERR_FATAL("Bad cmd %d", cmd_ptr->cmd, 0, 0 );
      }
    } /* switch */

    /* return the memnory back to the free queue */
    q_put (&rxc_isr_free_q, &cmd_ptr->link);

    (void)rex_set_sigs ( &rx_tcb, RXC_ISR_CMD_SIG );
      /* signal task that command has been processed */
  }/* while */

/*-------------------------------------------------------------------------*/
  /* perform normal interrupt processing */

  /* read in symbol error rates */
  dec_read_pc_status(&rxc_pc_status);

  srch_mux_idle_frame_stats(CAI_PCH, rxc_pc_status.ser, rxc_pc_status.energy, FALSE);

  if ( rxc_slot.skipping == TRUE )
  {
    if( rxcidle_first_frame( RXC_CDMA_PC ) == TRUE )
    {
      first_frame = TRUE;
        /* This is the first frame of the slot, after a WAKEUP. */

      rxc_slot.skipping = FALSE;
        /* Don't skip frames anymore. */
    }
  }

  if( rxc_slot.skipping )
  {
     /* This is the interrupt for a frame
        we do not care about. Just exit,
        discarding it outright.  */

    rxc_slot.frame_skip_cnt++;
      /* Count each frame while skipping is TRUE */

    /* Once we have waited for an excessive number of frames to
       go by without recognizing the first frame of the slot, assume
       that the symbol combiner phase/offset is incorrect and start
       decoding.  This will likely cause a lost Paging Channel, but
       is better than hanging indefinitely (probably forever) waiting
       for the phase to be correct. */
    if ( rxc_slot.frame_skip_cnt >= MAX_SKIPPED_FRAMES )
    {
      MSG_ERROR("Skpped %d fr - SC Bad",
                 rxc_slot.frame_skip_cnt, 0, 0);

      rxc_slot.skipping = FALSE;
        /* Don't skip frames anymore. */

      first_frame = FALSE;
        /* This is definitely not the first frame of the slot, after a WAKEUP. */

      MSM_LOCK();

      DECODER_INITIAL_STATE_NORMAL( );
        /* Ensure decoder is not reseting the Viterbi Decoder
           state metrics at the start of future frames. */

      MSM_UNLOCK();

      srch_mux_page_match( FALSE );
        /* Definitely no page matching happening at this point */
    }

    MSG_LOW("  Decint: fr=%lx, slt=%d",  qw_lo(rxc_dec_frame),
            ( (word)(qw_lo( rxc_dec_frame ) >> 2) & rxc_slot.sci_mask ), 0 );

//     MSG_LOW("  PC: ser=%lx, PC_Energy=%lx",
//      rxc_pc_status.ser, rxc_pc_status.energy);
     rxc_last_pc_ser = rxc_pc_status.ser;
     return;
  }

  if ( (buf_ptr = (rxc_dec_type *)q_get( &rxc_dec_free_q)) == NULL )
  {
    ERR( "0 bufs on dec_free_q", 0, 0, 0 );
  }

  else
  {
    dec_read_pc_data( buf_ptr->buf.pc_buf.bits );
      /* get decoder data */

    if( first_frame )
    {
       int2  srch_rssi_value = srch_mux_get_fing_rssi();
       MSM_LOCK();

       DECODER_INITIAL_STATE_NORMAL( );
         /* Ensure decoder is not reseting the Viterbi Decoder
            state metrics at the start of future frames. */

       MSM_UNLOCK();

       /* This is the interrupt for the
          first frame from our paging slot */

       MSG_LOW("  Decint: fr=%lx, slt=%d",  qw_lo(rxc_dec_frame),
         ( (word)(qw_lo( rxc_dec_frame ) >> 2) & rxc_slot.sci_mask ), 0 );

       /* Check to see if page matching was enabled this wakeup */
       if( rxc_slot.page_match )
       {
         /* OK, we can try to match. Go for it. */

         /* Disable matcher for this wakeup */
         rxc_slot.page_match = FALSE;

          /* Check if the first half frame of the slot is an empty General Page Msg */
          /* Note that this check will work for 9600 Paging Channels only with
             a low probability of false sleeping */

          /* quick page match for a General Page */
          if (rxc_empty_gpm_and_done(buf_ptr->buf.pc_buf.bits))
          {
             /* increase the GPM page matched counter */
             rxc_gpm_page_matched_counter++;
             /* set the page matched flag to indicate to MC later on */
             rxc_page_increase_page_matched_cnt = TRUE;
             /* an empty General Page Message and no more page for this mobile
             * in the current slot.
             */
             MSG_HIGH("Gen Pg match: ser %d|%d rssi=%d",
                       rxc_last_pc_ser, (word) rxc_pc_status.ser, srch_rssi_value);

             srch_mux_page_match(TRUE);

          }
          /* Quick Page Match for Slotted Pages */
          else if (rxc_empty_spm_and_done(buf_ptr->buf.pc_buf.bits))
          {
             /* increase the SPM page matched counter */
             rxc_spm_page_matched_counter++;
             /* set the page matched flag to indicate to MC later on */
             rxc_page_increase_page_matched_cnt = TRUE;
             /* an empty Slotted Page Message and no more page for this mobile
             * in the current slot.
             */
             MSG_HIGH("Slotted Pg match: ser %d|%d rssi=%d",
                       rxc_last_pc_ser, (word) rxc_pc_status.ser, srch_rssi_value);
             srch_mux_page_match(TRUE);
          }
          else
          {
             MSG_HIGH("No pg match: ser %d|%d rssi=%d",
                       rxc_last_pc_ser, (word) rxc_pc_status.ser, srch_rssi_value);
             srch_mux_page_match( FALSE );
          }
       }
       else
       {
          /* Not enabled this time */
          MSG_HIGH("Pg match not on: ser %d|%d, rssi=%d",
                   rxc_last_pc_ser, (word) rxc_pc_status.ser, srch_rssi_value);
          srch_mux_page_match( FALSE );
       } /* page_match */

       /* check to see if SRCH RSSI value if low, SRCH out of lock, then increase the counter */
       if (srch_rssi_value < 0 )
       {
         rxc_srch_rssi_negative_counter++;
       }
    } /* first_frame */
    
    qw_equ (buf_ptr->buf.pc_buf.frame, rxc_dec_frame );
      /* store frame number with data */
    buf_ptr->state = RXC_PC_STATE;
      /* indicate data from Pagig Channel */
#ifdef FEATURE_PLT
#error code not present
#endif
    q_put( &rxc_dec_q, &buf_ptr->link );
      /* put item on queue for task */
    (void)rex_set_sigs ( &rx_tcb, RXC_INT_SIG );
      /* set signal to indicate item is on queue */
  }

#if (!defined FEATURE_IS2000_REL_B) && (!defined FEATURE_IS2000_REL_A_CC_SMMC) && \
    (!defined FEATURE_IS2000_REL_D)

  /* For release B traffic to idle transition scenario, call deint_set_pc_mode()
  ** in rxc_pc_init() and do not call it again here.  Since the deint task list
  ** is deleted when exiting traffic state, it's necessary to set up the task 
  ** list for Paging mode in order to get RXC interrupts.
  **
  ** Prior to release B implementation, the rxc_pc_isr() will run with Sync mode 
  ** deint task list until the following condition is met and the deint task list
  ** is switched to Paging mode parameters.  This synchronization is deemed 
  ** un-necessary since some Demod parameters have been program for Paging mode 
  ** in rxc_pc_init(), so might as well program deint parameters in rxc_pc_init()
  ** also.
  */

  /* Switch decoder back to continuous mode if we
     are in Paging Channel state */

  if ( srch_mux_start_pc_state( FALSE ) )
  {
    deint_set_pc_mode((deint_pc_rate_type)rxc_pc_var.pc_rate);
  }
#endif /* !FEATURE_IS2000_REL_B && !FEATURE_IS2000_REL_A_CC_SMMC && !defined FEATURE_IS2000_REL_D */

#ifdef FEATURE_AGC_LOGGING
  /* perform AGC/power control logging */
  srch_mux_log_agc_pctl();
#endif

} /* rxc_pc_isr */



/*===========================================================================

FUNCTION RXC_LOG_PC_MSG

DESCRIPTION
  This function logs messages received on the Paging Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  rxc_log_pc_msg
(
  cai_pc_ext_type *msg_ptr   /* pointer to received message */
)
{
#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
  log_pc_msg_type  *log_ptr = NULL; /* Pointer to log */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (msg_ptr && msg_ptr->length != 0)
  {
    log_ptr = (log_pc_msg_type *) log_alloc(LOG_PC_MSG_C,
                                            FSIZ(log_pc_msg_type, hdr) +
                                            msg_ptr->length);
  }
  else
    log_ptr = NULL;

  if (log_ptr == NULL)
  {
    rxc_log_msg_drop++;
  }
  else
  {
    /* copy message into buffer */
    (void)memcpy( (void *) log_ptr->msg, (void *) &msg_ptr->length,  //length includes both the byte for intself, as well as body
                  msg_ptr->length );//lint !e613 Possible use of null pointer (msg_ptr). The above has checked msg_ptr non-null

    log_commit((log_type *) log_ptr);
  }
#else
  rxc_log_msg_type *log_ptr = NULL;   /* pointer to buffer to log msg in */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get a buffer off of the rxc_log_msg_q to log the received message */
  if ( (msg_ptr && msg_ptr->length == 0) ||
       (( log_ptr = (rxc_log_msg_type *)q_get(&rxc_log_msg_q) ) == NULL ) )
  {
    rxc_log_msg_drop++;
  }
  else
  {
    /* set up log buffer */
    log_ptr->entry.pc_msg.hdr.len = msg_ptr->length + sizeof(log_hdr_type);
    log_ptr->entry.pc_msg.hdr.code = LOG_PC_MSG_C;
    ts_get( (unsigned long *)(log_ptr->entry.pc_msg.hdr.ts));

    /* copy message into buffer */
    (void)memcpy((void *) log_ptr->entry.pc_msg.msg,
                 (void*) &msg_ptr->length, //length includes both the byte for itself, as well as body
                 msg_ptr->length );

    /* log message */
    log_put( &log_ptr->desc );
  }
#endif /* !defined(FEATURE_DIAG_V7_LOG_IFACE) */

  /* Event report: Message received */
  if ((msg_ptr) && (log_ptr != NULL))
  {
    mclog_report_event_msg_rxed(CAI_PCH, msg_ptr->body);
  }
} /* rxc_log_pc_msg */




/*===========================================================================

FUNCTION RXC_PC_PARSE

DESCRIPTION
  This function collects the bits of a Paging Channel frame by parsing
  Paging Channel frames.  When it determines that an entire Paging Channel
  message has been received, and the message CRC checks, it will place the
  message on a queue for the Layer 2 task and signal the Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/


void rxc_pc_parse
(
  qword  frame_num,   /* frame number */
  byte   sframe[],    /* Address of 1st byte of subframe. */
  word   sframe_len   /* Number of bits in subframe */
)
{
  word  sframe_inx;         /* index into subframe */
  word  num_bits;           /* Number of bits to be copied to the message
                               buffer */
  word parm_stat;           /* status returned by parm_inc */
  boolean frame_cnted = FALSE;
    /* indicator of whether subframe has been counted as one with a part of
        a message with a CRC that checks */
  mccrxtx_cmd_type *msg_ptr;
    /* pointer to buffer to place translated message in */
  word status;
    /* status returned by caix_ext_to_int */
  word msg_length;
    /* message length to pass to caix_ext_to_int */
  static qword  frame_stamp;
    /* timestamp frame number */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* increment number of half frames received */
  if( ( parm_stat = parm_inc ( CAI_PAG_4_ID, 1 )) != PARM_DONE_S )
  {
    MSG_HIGH("Bad param incr: status %d",
              parm_stat,0,0 );
  }

  if ( rxc_slot.active  &&  rxc_slot.after_slot_boundary == FALSE )
  {
#ifdef FEATURE_BROADCAST_SMS
    if ((word)(qw_lo( frame_num ) >> 2) == rxc_slot.slot)
#else
    if ( ((qw_lo( frame_num ) >> 2) & rxc_slot.sci_mask) ==
         (   rxc_slot.idle_pgslot   & rxc_slot.sci_mask)    )
#endif /* FEATURE_BROADCAST_SMS */
    {
      rxc_slot.after_slot_boundary = TRUE;
    }
  }

  if ( ( sframe[0] & 0x80 ) && ( rxc_pc_var.flush_cnt == 0 ) )
  {
    /* start of message bit found - start parsing a new message regardless
       of message parsing state */
    rxc_pc_var.state = PC_HUNT_STATE;
    rxc_pc_var.half_frames = 0;
      /* zero out number of half frames counted for last message */
  }

  sframe_inx = 1;  /* Skip over 1st bit (SOM). */

  while ( sframe_inx < sframe_len )  /* While still more bits. */
  {
    switch ( rxc_pc_var.state )
    {
      case  PC_HUNT_STATE:
      {
        /* Search for start of message bit */
        if( sframe[0] & 0x80 )
        {
          rxc_pc_var.msg_inx = 0;

          rxc_pc_var.state = PC_LEN_STATE;
        }

        else
        {
          rxc_pc_var.state = PC_FLUSH_STATE;
        }

        break;
      }

      case  PC_LEN_STATE:
      {
        if ( ( (sframe_inx + 8) <= sframe_len ) &&
             ( (rxc_pc_var.msg_len =
                 (word)(b_unpackw ( sframe, sframe_inx, 8 ) * 8)) > 0 ) &&
               ( rxc_pc_var.msg_len <= ( CAI_PC_MAX_MSG_BYTES * 8 ) ) )
        {

          if( (rxc_pc_var.msg_ptr == NULL ) &&
              ( (rxc_pc_var.msg_ptr =
                   (rxtx_rx_msg_type *)q_get( &rxtx_rx_free_q) ) == NULL ) )
          {
            ERR( "0 bufs on rxtx_rx_free_q", 0, 0, 0 );
            /* out of buffers -- go to skip state */
            rxc_pc_var.state = PC_SKIP_STATE;
          }
          else
          {
            qw_equ (frame_stamp, frame_num);

            b_copy ( sframe, sframe_inx,
                     &rxc_pc_var.msg_ptr->msg.pc.length,
                     rxc_pc_var.msg_inx, 8 );
                       /* copy length byte to message buffer */

            rxc_pc_var.msg_inx += 8; /* increment index into message buffer */

            sframe_inx += 8;    /* increment index into subframe */

            if( !frame_cnted )
            {
              rxc_pc_var.half_frames++;
                /* count this half frame as part of a message */
              frame_cnted = TRUE;
            }

            rxc_pc_var.state = PC_MSG_STATE; /* go to next state */

            if( ( parm_stat = parm_inc ( CAI_PAG_1_ID, 1 ))
                  != PARM_DONE_S )
            {
              MSG_HIGH("Bad param incr: status %d",
                        parm_stat,0,0 );
            }
          }
        }

        else
        { /* message length = 0 or less than 8 bits left in subframe */
          rxc_pc_var.state = PC_FLUSH_STATE;
        }

        break;
      }

      case PC_MSG_STATE:
      {
        num_bits = MIN ( (rxc_pc_var.msg_len - rxc_pc_var.msg_inx),
                         (sframe_len - sframe_inx) );
          /* calculate number of bits to copy into message buffer */
        b_copy ( sframe, sframe_inx, &rxc_pc_var.msg_ptr->msg.pc.length,
                   rxc_pc_var.msg_inx, num_bits );

        sframe_inx += num_bits;
          /* increment index into subframe */

        rxc_pc_var.msg_inx += num_bits;
          /* increment index into message buffer */

        /* Count this half frame as having a part of a message */
        if( !frame_cnted )
        {
          rxc_pc_var.half_frames++;
            /* count this half frame as part of a message */
          frame_cnted = TRUE;
        }

#ifdef FEATURE_BROADCAST_SMS
        if ((rxc_slot.slot_mask & BC_DATA_SLOT) &&
            (rxc_pc_var.msg_inx > (CAII_BC_HDR_LEN+CAI_PC_LGTH_SIZE)) &&
            (rxc_slot.bcsms_slot_frames > RXC_BC_PEEK_BOUNDRY) &&
            (rxc_slot.bc_started == FALSE))
        {
          /* if the current slot is a BC SMS slot, and we have acquired enough
           * header bits, and the beginning of these bits started in the first
           * slot of the scheduled slots, we can try to determine whether the
           * expected BC SMS has arrived. */
          if (mcc_bc_match(rxc_pc_var.msg_ptr->msg.pc.body))
          {
            /* we have found the expected BC */
            rxc_slot.bc_started = TRUE;
          }
        }
#endif /* FEATURE_BROADCAST_SMS */

        if ( rxc_pc_var.msg_inx >= rxc_pc_var.msg_len )
        {
          /* all bits of message collected */
          if ( (crc_30_calc (&rxc_pc_var.msg_ptr->msg.pc.length,
                                       rxc_pc_var.msg_len )) == CRC_30_OK )
          {

            rxc_pc_crc_good++;
            /* process message. */
            rxc_pc_var.err_cnt = 0;

            if( ( parm_stat = parm_inc ( CAI_PAG_5_ID, rxc_pc_var.half_frames))
                  != PARM_DONE_S )
            {
              MSG_HIGH("Bad param incr: status %d",
                        parm_stat,0,0 );
            }

            /* Reset the fade timer since we got a valid message */
            rxcidle_set_fade_timer(RXCIDLE_CURR_FADE);

            if ( rxc_slot.active  &&  !rxc_slot.valid_msg  &&
                 rxc_slot.after_slot_boundary )
            {
              rxc_slot.valid_msg = TRUE;
            }

            if (rxc_pc_var.msg_len > 40 )
            {
              /* log message */
              rxc_log_pc_msg( &rxc_pc_var.msg_ptr->msg.pc );

              /* translate every message but null message and send to
                  Main Control task */

              if((msg_ptr =
                     (mccrxtx_cmd_type *)q_get(&mcc_rxtx_free_q)) == NULL)
              {
                ERR( "0 bufs on mcc_rxtx_free_q", 0, 0, 0 );
              }
              else
              {
                msg_length = rxc_pc_var.msg_len - CAI_PC_CRC_SIZE -
                             CAI_PC_LGTH_SIZE ;
                status = caix_ext_to_int( msg_length, CAIX_PC,
                          &rxc_pc_var.msg_ptr->msg,
                          &msg_ptr->msg.msg );

                #if defined(FEATURE_IS2000_REL_C_MI) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
                #endif /* FEATURE_IS2000_REL_C_MI || FEATURE_IS2000_REL_A_AES */

                if ( (status != CAIX_DONE_S)
#ifdef FEATURE_IS2000_REL_A
                /* If Ext. Encryption failure occurs, we want to send the message to
                MC anyway so it can take appropriate action by examining the
                Ext. Encryption fields. */

                     &&
                     (status != CAIX_INV_ENC_MODE_NOT_SUP_S)
#endif
                   )
                {
                  MSG_MED("Bad PC transl: status %d",status,0,0 );
                  /* Put item back on to free q */
                  q_put(&mcc_rxtx_free_q, &msg_ptr->msg.hdr.cmd_hdr.link );
                }
                else
                {
#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_PLT
#error code not present
#else
                  if ( (ftm_mode == DMSS_MODE) ||
                       (ftm_wait_for_pc_msg && !ftm_pc_msg_sent ) )
#endif
                  {
                     /* User waits for a PC message */
#endif /* FEATURE_FACTORY_TESTMODE */
#ifdef FEATURE_IS2000_REL_A
                    if (status == CAIX_INV_ENC_MODE_NOT_SUP_S)
                    {
                      /* Ext. Encryption failure at parser, log and pass to MC. */
                      MSG_HIGH("Ext. Encryp fail, pass to MC", 0,0,0);
                      msg_ptr->msg.parser_decryption_failure = TRUE;
                    }
                    else
                    {
                      msg_ptr->msg.parser_decryption_failure = FALSE;
                    }
#endif
                    msg_ptr->msg.hdr.command = MCCRXTX_MSG_F;
                    msg_ptr->msg.chn = RXC_CDMA_PC;
                    /* indicate to MC if there is a page matched */
                    if (rxc_page_increase_page_matched_cnt)
                    {
                      msg_ptr->msg.page_match_counter_incremented = TRUE;
                      rxc_page_increase_page_matched_cnt = FALSE;
                    }
                    else
                    {
                      msg_ptr->msg.page_match_counter_incremented = FALSE;
                    }
                    /* message is from Paging Channel */
                    qw_equ (msg_ptr->msg.frm.frame, frame_stamp);

                    msg_ptr->msg.hdr.cmd_hdr.done_q_ptr = &mcc_rxtx_free_q;
                    msg_ptr->msg.hdr.cmd_hdr.task_ptr = NULL;

                    /* Put message on queue for MC */
                    mccrxtx_cmd( msg_ptr);
#ifdef FEATURE_FACTORY_TESTMODE
                     ftm_pc_msg_sent = TRUE;
                  }
                  else
                  {
                     /* Put item back on to free q */
                     q_put( &mcc_rxtx_free_q, &msg_ptr->msg.hdr.cmd_hdr.link );
                  }
#endif /* FEATURE_FACTORY_TESTMODE */
                }
              }

            }

            rxc_pc_var.state = PC_LEN_STATE;
          }

          else
          {
            /* CRC did not check */
            rxc_pc_crc_bad++;
            if( ( parm_stat = parm_inc ( CAI_PAG_2_ID, 1 )) != PARM_DONE_S )
            {
              MSG_HIGH("Bad param incr: status %d",
                        parm_stat,0,0 );
            }

            /* log message of 0 length to indicate bad CRC*/
            rxc_pc_var.msg_ptr->msg.pc.length = 0;
            rxc_log_pc_msg( &rxc_pc_var.msg_ptr->msg.pc );

            if ( ++rxc_pc_var.err_cnt > 2 )
            {
              /* need to resynchronize to paging channel */
              rxc_pc_var.err_cnt = 0;
              rxc_pc_var.state = PC_FLUSH_STATE;
            }
            else
            {
              rxc_pc_var.state = PC_LEN_STATE;
            }

          }
          rxc_pc_var.msg_inx = 0;  /* reset message index */
          /* regardless of whether CRC checked or not zero out count of
              half-frames which contributed to message */
          rxc_pc_var.half_frames = 0;
#ifdef FEATURE_PLT
#error code not present
#endif
        }

        break;
      }

      case PC_FLUSH_STATE:
      {
        sframe_inx = sframe_len; /* Indicate all bits used up. */

        /* only go back to hunt state if you are not flushing frames from
           idle handoff */
        if( rxc_pc_var.flush_cnt > 0 )
        {
          rxc_pc_var.flush_cnt--;
        }
        if( rxc_pc_var.flush_cnt == 0 )
        {
          rxc_pc_var.state = PC_HUNT_STATE;
        }

        break;
      }

      case PC_SKIP_STATE:
      {
        /* Here we are keeping track of the bits of the message that
           have been skipped with msg_inx even though we are not
           collecting the bits in a buffer */

        num_bits = MIN ( (rxc_pc_var.msg_len - rxc_pc_var.msg_inx),
                         (sframe_len - sframe_inx) );

        sframe_inx += num_bits;
          /* increment index into subframe */
        rxc_pc_var.msg_inx += num_bits;
          /* increment index into message buffer */

        if ( rxc_pc_var.msg_inx >= rxc_pc_var.msg_len )
        {
          rxc_pc_var.state = PC_HUNT_STATE;
        }
        break;
      }

      default:
      {
        ERR_FATAL("Bad PC parse subst %d", rxc_pc_var.state,0,0);
      }
    } /* switch */

  } /* while */

} /* rxc_pc_parse */

/* <EJECT> */
/*===========================================================================

FUNCTION RXC_PC_DATA

DESCRIPTION
  This procedure processes data received from the decoder while the Receive
  task is processing the Paging Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  rxc_pc_data ( void )
{
  rxc_dec_type *buf_ptr;  /* pointer to decoder data buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void)rex_clr_sigs( rex_self(), RXC_INT_SIG );


  while ( (buf_ptr = (rxc_dec_type *)q_get( &rxc_dec_q) ) != NULL )
  {
      /* copy the data we need from the decoder buffer */
    memcpy (rxc_pc_var.page_frm, buf_ptr->buf.pc_buf.bits, DEC_192_BITS / 8);
    rxc_pc_var.msg_state = buf_ptr->state;
    qw_equ (rxc_pc_var.msg_frame, buf_ptr->buf.pc_buf.frame);

      /* Free up the buffer for the ISR */
    q_put( &rxc_dec_free_q, &buf_ptr->link );

    if( rxc_pc_var.msg_state == RXC_PC_STATE )
    {
      /* Note that we must take into account 71 bit decoder delay on
         Paging Channel but since number of bits received per frame
         varies with the data rate we have set up the parameters for
         the b_copy routine in rxc_pc_init */

      b_copy ( rxc_pc_var.page_frm, 0, rxc_pc_var.page_blk,
               rxc_pc_var.bc1, rxc_pc_var.bc2 );
        /* Copy remaining portion of current frame to buffer. */

      while (rxc_pc_var.parse_index < rxc_pc_var.pc_bits)
      {
        /* Send bits to Paging Channel message processor */
        rxc_pc_parse ( rxc_pc_var.msg_frame,
                      &rxc_pc_var.page_blk[rxc_pc_var.parse_index/8],
                      rxc_pc_var.parse_bits );
        rxc_pc_var.parse_index += rxc_pc_var.parse_bits;
      }

      b_copy ( rxc_pc_var.page_frm, rxc_pc_var.bc2, rxc_pc_var.page_blk, 0,
               rxc_pc_var.bc1 );
        /* Copy first piece of next frame to buffer. */

      rxc_pc_var.parse_index = 0;

      /* If first part of new buffer is large enough to contain
         parsable sections, we can parse them. */
      if (rxc_pc_var.parse_bits < rxc_pc_var.bc1)
      {
        /* This message is part of the next frame; increase frame number */
        qw_inc( rxc_pc_var.msg_frame, 1L);

        while (rxc_pc_var.parse_index + rxc_pc_var.parse_bits <
               rxc_pc_var.pc_bits)
        {
          rxc_pc_parse( rxc_pc_var.msg_frame,
                       &rxc_pc_var.page_blk[rxc_pc_var.parse_index/8],
                       rxc_pc_var.parse_bits );
          rxc_pc_var.parse_index += rxc_pc_var.parse_bits;
        }
      }
    }

  }
} /* rxc_pc_data */



/*===========================================================================

FUNCTION RXC_PC_INIT

DESCRIPTION
  This funtion initializes the Paging Channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_pc_init
(
  word curr_state,  /* Current RXC state machine state */
  word pc_rate      /* Rate of Paging Channel */
)
{

  MSM_LOCK( );
  DEMOD_DISABLE_Q_CHANNEL( );  /* Disable Q channel for Sync Channel */
  COMBINER_SELECT_CH0_64_CHIP_SYMBOL( );  /* Tell the Combiner to accumulate
                                        symbols over 64 pn chips */

  DEMOD_SET_CH0_TD_SIGN( 0x0 );
  DEMOD_DISABLE_CH0_RC4( );

  MSM_UNLOCK( );

#ifndef FEATURE_IS2000_REL_A_CC
if((rxc_atmr_handle != ATMR_INVALID_HANDLE))
  {
          /*  Free action timer resource. */
           atmr_set( rxc_atmr_handle,
                     NULL,
                     ATMR_5MS_FRAME(NONE),
                     (atmr_pcg_offset_enum_type)0,
                     (unsigned long)0 ) ;
  }

  rxc_atmr_handle = atmr_get( ATMR_TIME_2 ) ;
  if( rxc_atmr_handle == ATMR_INVALID_HANDLE )
  {
    ERR( "rxc can't get atmr handle",0,0,0 ) ;
  }
#endif /* FEATURE_IS2000_REL_A_CC */

  switch( curr_state )
  {
    case RXC_SC_STATE:
#ifdef FEATURE_VOIP_1X_HANDOFF
    case RXC_CDMA_STATE:
#endif /* FEATURE_VOIP_1X_HANDOFF */
#ifdef FEATURE_IS2000_REL_A_CC
    case RXC_BCCH_STATE:
    case RXC_FCCCH_STATE:
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
    case RXC_NCCCH_STATE:
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
#endif
#ifdef FEATURE_IS2000_REL_B
    case RXC_TC_STATE:
#endif /* FEATURE_IS2000_REL_B */
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

    {
      enc_set_mode( ENC_PC_MODE );

#if (defined FEATURE_IS2000_REL_B) || (defined FEATURE_IS2000_REL_A_CC_SMMC) || \
    (defined FEATURE_IS2000_REL_D)

      /* For release B traffic to idle transition scenario, call deint_set_pc_mode()
      ** here.  Since the deint task list is deleted when exiting traffic state, 
      ** it's necessary to set up the task list for Paging mode in order to get 
      ** RXC paging channel interrupts (see rxc_pc_isr() for further explanation.)
      */
      deint_set_pc_mode ((deint_pc_rate_type)pc_rate);
#endif /* FEATURE_IS2000_REL_B or FEATURE_IS2000_REL_A_CC_SMMC or FEATURE_IS2000_REL_D */
      /* Mapping the DEINT RAM for paging, If we get in here from SYNC, then the DEINT
      is already mapped in the synch init, other than that, we must re-map it again */
      deint_allocate_CH0_RAM();

      /* set interrupt handler for Paging channel */
      rxc_set_pc_isr();

      /* Set up variables for State machine */
      if( pc_rate == CAI_PC_1_RATE )
      {
        rxc_pc_var.pc_bits = 192;
      }
      else if ( pc_rate == CAI_PC_2_RATE )
      {
        rxc_pc_var.pc_bits = 96;
      }
      else
      {
        /* Set error value for 1/2 rate */
        rxc_pc_var.pc_bits = 96;
        MSG_HIGH( "Bad PC rate %d", pc_rate, 0, 0 );
      }
      /* SVD has a 72 bit delay in continous mode */
      rxc_pc_var.bc2 = 72;
      rxc_pc_var.bc1 = rxc_pc_var.pc_bits - rxc_pc_var.bc2;

      /* We parse 2 subframes per frame */
      rxc_pc_var.parse_bits = rxc_pc_var.pc_bits / 2;

      /* Determine where in frame to start decoding sub-frames after wakeup.
         The first 71 bits out of the decoder may or may not be large
         enough to contain subframe(s) */
      rxc_pc_var.parse_start = (word)( rxc_pc_var.pc_bits -
         rxc_pc_var.parse_bits * (rxc_pc_var.bc2 / rxc_pc_var.parse_bits) );
      rxc_pc_var.parse_index = rxc_pc_var.parse_start;

      /* initialize ser totals */
      rxc_ser_reset();

      rxc_pc_crc_good = 0;
      rxc_pc_crc_bad = 0;

      rxc_pc_var.pc_rate = pc_rate;  /* Used in rxc_dec_pc_init */

      rxc_pc_var.err_cnt = 0;  /* initialize parsing errors */
      rxc_pc_var.state = PC_HUNT_STATE;
        /* initialize state of Paging Channel state machine */
      rxc_pc_var.half_frames = 0;
        /* no half frames have been part of a message yet */
      rxc_pc_var.flush_cnt = 0;  /*no frames to flush */

      rxc_slot.active = FALSE;  /* not in slotted mode yet */
      /* Make sure page matching is completely disabled */
      rxc_slot.page_match = FALSE;
      break;
    } /* RXC_SC_STATE */

    default:
    {
      MSG_FATAL( "Bad state %d", curr_state,0, 0 );
    }
  } /* switch */

} /* rxc_pc_init */



/*===========================================================================

FUNCTION RXC_PC_EXIT

DESCRIPTION
  This funtion performs processing necessary to end the Paging channel state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_pc_exit( void )
{
#ifdef FEATURE_FACTORY_TESTMODE
  ftm_pc_msg_sent = FALSE;
    /* Reset the flag */
#endif /* FEATURE_FACTORY_TESTMODE */

  /* make sure all items are placed back on the rxtx_rx_free_q */
  if (rxc_pc_var.msg_ptr != NULL )
  {
    q_put(&rxtx_rx_free_q, &rxc_pc_var.msg_ptr->link );
    rxc_pc_var.msg_ptr = NULL;
  }
}/* rxc_pc_exit */



/*===========================================================================

FUNCTION RXC_PC

DESCRIPTION
  Paging Channel state processing.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  Next state to be processed.

===========================================================================*/

word rxc_pc ( void )
{

  rxc_state_type next_state;
    /* next state to be processed */
  rxc_cmd_msg_type *cmd_ptr;
    /* pointer to command buffer received on rxc_cmd_q */
  mccrx_rpt_type *rpt_ptr;
    /* pointer to report to send to Main Control task */
  word isave;
  rex_sigs_type sigs;       /* signals returned by rx_wait */
  rex_sigs_type sa_wait_mask;
    /* This mask is used to set various signals on which Mobile waits */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

  /* Clear OKTS sig - We don't want to sleep yet */
  rxc_vote_not_ok_to_sleep();

  next_state = RXC_PC_STATE;
  while( next_state == RXC_PC_STATE )
  {
    /* check for received interrupts */
    rxc_pc_data();

    (void)rex_clr_sigs( rex_self(), RXC_CMD_Q_SIG );
    if( (cmd_ptr = (rxc_cmd_msg_type *)q_get(&rxc_cmd_q) ) != NULL )
    {
      switch( cmd_ptr->hdr.command )
      {
        case RXC_CDMA_F:
          MSG_MED ("Got RXC_CDMA_F",0,0,0);

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

        case RXC_EXIT_F:
        {
          MSG_MED ("Got RXC_EXIT_F",0,0,0);

          cmd_ptr->hdr.status = RXC_DONE_S;
          next_state = RXC_EXIT_STATE;
          break;
        }

        case RXC_SC_F:
        {
          MSG_MED ("Got RXC_SC_F",0,0,0);

          /* initialize Sync channel state */
          rxc_sc_init( RXC_PC_STATE);
          cmd_ptr->hdr.status = RXC_DONE_S;
          next_state = RXC_SC_STATE;
          break;
        }/* RXC_SC_F */

        case RXC_TCI_F:
        {

#ifdef FEATURE_IS2000_REL_A_CC
          MSG_MED ("Got RXC_TCI_F",0,0,0);
          next_state = rxc_tci_init (RXC_PC_STATE, cmd_ptr);
#else
          // Current FCH/DCCH RC, initialize to something to silent compiler
          cai_radio_config_type cur_rc = cmd_ptr->tci.for_fch.rc;

          MSG_MED ("Got RXC_TCI_F",0,0,0);
          rxc_set_frame_offset(cmd_ptr->tci.frame_offset);

          INTLOCK_SAV( isave );

          rxc_clear_decoder_queue ();

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */
#ifdef FEATURE_FACTORY_TESTMODE
          rxc_so.enable_skip_tci = cmd_ptr->tci.enable_skip_tci;
          rxc_set_fch_loopback(FALSE);
#ifdef FEATURE_IS2000_P2
          /* Support FEATURE_FTM_DCCH */
          rxc_set_dcch_loopback(FALSE);
          /* End of FEATURE_FTM_DCCH */
#endif
          rxc_set_sch_loopback(FALSE);
#endif /* FEATURE_FACTORY_TESTMODE */

#ifdef FEATURE_IS2000_REL_A
          rxc_tc_var.p_rev_in_use = cmd_ptr->tci.p_rev_in_use;

          // Initialize the P_REV transistion flag to FALSE.
          rxc_tc_var.in_p_rev_transition = FALSE;

#endif

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)

          cur_rc = rxc_configure_ffpc_channel(cmd_ptr);

#else
#ifdef FEATURE_IS2000_P2   /* DCCH/FCH */
          /* DCCH configuration */
          if ( (rxc_so.for_dcch.included = cmd_ptr->tci.for_dcch.included) != FALSE )
          {
            cur_rc = rxc_so.for_dcch.rc = cmd_ptr->tci.for_dcch.rc;
            rxc_so.high_rc = TRUE;
          }

          /* FCH configuration */
          if ( (rxc_so.for_fch.included = cmd_ptr->tci.for_fch.included) != FALSE )
#endif /* FEATURE_IS2000_P2 */
          {
            // cur_rc already holds fch rc
            rxc_so.for_fch.rc = cmd_ptr->tci.for_fch.rc;
            rxc_so.high_rc = rxc_so.for_fch.rc > CAI_RC_2;
          }
          rxc_config_demod( (deint_rc_type) cur_rc );
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */


          /* Initialize SCH0 RC */
          rxc_sch.rc = (dec_rc_type)DEINT_RC_INVALID;

          /* For SRCH logging */
#ifdef FEATURE_IS2000_P2
          srch_mux_update_rc (rxc_so.for_fch.included?
                              (deint_rc_type) rxc_so.for_fch.rc:DEINT_RC_INVALID,
                              (deint_rc_type) rxc_sch.rc,
                              DEINT_RC_INVALID,
                              rxc_so.for_dcch.included?
                              (deint_rc_type)rxc_so.for_dcch.rc:DEINT_RC_INVALID );
#else
          srch_mux_update_rc (rxc_so.for_fch.rc,
                              rxc_sch.rc,
                              DEINT_RC_INVALID,
                              DEINT_RC_INVALID);
#endif /* FEATURE_IS2000_P2 */

          rxc_tci_init( RXC_PC_STATE );

          /* program the delay of TX generated MASK_DATA to RX PCG timing */
          MUXMSM_AGC_CTL_MASK_DELAY(cmd_ptr->tci.rev_pwr_cntl_delay);

          ffpc_tci_cmd (cmd_ptr);
#ifdef FEATURE_DCCH_FRAME_QUALITY_SUPERVISION  
          if (cmd_ptr->tci.for_dcch.included)
          {
            /* call the function to initialize DCCH quality threshole value */
            rxcrda_dcch_init();
          }
#endif /* End of FEATURE_DCCH_FRAME_QUALITY_SUPERVISION   */
          cmd_ptr->hdr.status = RXC_DONE_S;
          next_state = RXC_TCI_STATE;

#if defined (FEATURE_IS2000_P2) && !defined (FEATURE_IS2000_REL_A_SVD) 
          if(hw_version() >= MSM_50_A0)
          {
            if(cmd_ptr->tci.for_dcch.included && cmd_ptr->tci.for_fch.included)
            {
              MSG_ERROR("No FCH and DCCH on MSM5000",0,0,0);
              cmd_ptr->hdr.status = RXC_BADPARM_S;
            }
          }
#endif /* FEATURE_IS2000_P2 && !FEATURE_IS2000_REL_A_SVD */

          INTFREE_SAV( isave );
          rxc_pwr.sch_tot_frames[0] = 0;
          rxc_pwr.sch_bad_frames[0] = 0;
          rxc_pwr.sch_fer_rep[0] = FALSE;
          rxc_pwr.sch_tot_frames[1] = 0;
          rxc_pwr.sch_bad_frames[1] = 0;
          rxc_pwr.sch_fer_rep[1] = FALSE;
#endif /* FEATURE_IS2000_REL_A_CC */
          break;
        } /* RXC_TCI_F */

        case RXC_WAKE_F:
        {
          MSG_MED ("Got RXC_WAKE_F",0,0,0);

          /* call the function to install the corresponding decoder ISR */
#ifdef FEATURE_IS2000_REL_A_CC
          rxc_idle_activate();
#else
          rxc_set_pc_isr ();
#endif /* FEATURE_IS2000_REL_A_CC */

          /* Reset parser to begin proper message parsing. */
          rxc_pc_var.parse_index = rxc_pc_var.parse_start;

          /* Reset flush count to 0 in case an Idle HO occured before
           * sleep
           */
          rxc_pc_var.flush_cnt = 0;

          INTLOCK_SAV( isave );

          /* reset paging channel mode */

          MSM_LOCK( );
          clk_regime_enable(CLK_RGM_DEC_M);
          DEMOD_DISABLE_Q_CHANNEL( );  /* Disable Q channel for Sync Channel */
          COMBINER_SELECT_CH0_64_CHIP_SYMBOL( );  /* Tell the Combiner to accumulate
                                                symbols over 64 pn chips */
          MSM_UNLOCK( );

          /* Reinitialize slot counter */
          rxc_slot.skipping = TRUE;

          /* Reset the frame count used during skipping */
          rxc_slot.frame_skip_cnt = 0;

          /* Clear out path clear control flag */
          rxc_slot.blank = FALSE;
             
          if( rxc_pc_var.pc_rate == CAI_PC_1_RATE )
          {
             /* Optional page match feature only available
                for 9600-baud paging channel right now */
             rxc_slot.page_match = TRUE;

          }
          else
          {
             /* Page match feature not available */
             rxc_slot.page_match = FALSE;
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

          INTFREE_SAV( isave );

          /* begin timing 2 slots and checking for a valid message in the
             2 slots */
          rxc_slot.valid_msg = FALSE;

          /* Start Idle fade timer */
          rxcidle_set_fade_timer(RXCIDLE_IDLE_FADE);

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

        case RXC_SLOT_F:
        {
          MSG_MED ("Got RXC_SLOT_F",0,0,0);

          /* Reset parser to begin proper message parsing. */
          // rxc_pc_var.parse_index = rxc_pc_var.parse_start;
          /* it's commented out because if RXC_SLOT_F is received while a cross
           * slot message is being received, it would be discarded. it has been
           * seen happening with large SMS messages */

          INTLOCK_SAV( isave );

          /* reset paging channel mode */
          MSM_LOCK( );
          clk_regime_enable(CLK_RGM_DEC_M);
          DEMOD_DISABLE_Q_CHANNEL( );  /* Disable Q channel for Sync Channel */
          COMBINER_SELECT_CH0_64_CHIP_SYMBOL( );  /* Tell the Combiner to accumulate
                                                symbols over 64 pn chips */
          MSM_UNLOCK( );

         /* Make sure page matching is completely disabled */
          rxc_slot.skipping   = FALSE;
          rxc_slot.page_match = FALSE;
          rxc_slot.blank      = FALSE;

          /* set interrupts on for Paging channel */
          rxc_set_pc_isr();

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

          INTFREE_SAV( isave );

          /* begin timing 2 slots and checking for a valid message in the
             2 slots */
          rxc_slot.valid_msg = FALSE;

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

        case RXC_SLEEP_F:
        {
          MSG_MED ("Got RXC_SLEEP_F",0,0,0);

          /* Go back to HUNT state */
          rxc_pc_var.state = PC_HUNT_STATE;

          /* stop running slot timer also */
          rxc_slot.active = FALSE;

          /* reset all valid message reporting counters */
          rxc_reset_frame_count();
     
          rxcidle_clr_fade_timer();

          /* Disable RX ISR as we are done demodulating frames
          */
          rxc_clear_isr();

          /* Clear buffers from decoder queue. */
          rxc_clear_decoder_queue ();

          /* Tell the sleep task that we're ready for the CPU to halt
          ** The sleep task is lower priority, so it won't do anything
          ** until the RXC task suspends.
          */
          rxc_vote_ok_to_sleep();

          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }

        case RXC_ACC_FADE_F:
        {
          MSG_MED ("Got RXC_ACC_FADE_F",0,0,0);

          rxcidle_set_fade_timer(RXCIDLE_ACCESS_FADE);

          rxc_slot.active = FALSE;  /* not in slotted mode anymore */

          cmd_ptr->acc_fade.hdr.status = RXC_DONE_S;
          break;
        }

        case RXC_IDL_FADE_F:
        {
          MSG_MED ("Got RXC_IDL_FADE_F",0,0,0);

          /* start using Idle fade timer (CAI_T30M ) */
          rxcidle_set_fade_timer(RXCIDLE_IDLE_FADE);



          cmd_ptr->idl_fade.hdr.status = RXC_DONE_S;
          break;
        }

        case RXC_IDLE_HO_F:
        {
          MSG_MED ("Got RXC_IDLE_HO_F",0,0,0);

          /* perform idle handoff processing */

          /* reset message parsing state machine */
          if( rxc_pc_var.msg_ptr != NULL )
          {
            q_put( &rxtx_rx_free_q, &rxc_pc_var.msg_ptr->link );
            rxc_pc_var.msg_ptr = NULL;
          }

          rxc_pc_var.msg_inx = 0;
          rxc_pc_var.msg_len = 0;
          rxc_pc_var.err_cnt = 0;
          rxc_pc_var.half_frames = 0;
          rxc_pc_var.state = PC_FLUSH_STATE;
          rxc_pc_var.flush_cnt = 4;  /* flush next 4 half-frames */

          cmd_ptr->idle_ho.hdr.status = RXC_DONE_S;
          break;
        }

#ifdef FEATURE_FACTORY_TESTMODE
        case RXC_PC_F:
        {
          MSG_MED ("Got RXC_PC_F",0,0,0);

          if(ftm_mode == FTM_MODE)
          {
            cmd_ptr->hdr.status = RXC_DONE_S;
            break;
          }
          /* Fall through to default: case */
        }
#endif /* FEATURE_FACTORY_TESTMODE */

#ifdef FEATURE_IS2000_REL_A_CC
        case RXC_CHAN_CONFIG_F:
        {
          MSG_MED ("Got RXC_CHAN_CONFIG_F",0,0,0);

           /* recieved chan config for PCH in PCH state
            * disregard message */
#ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
          /* Note: Currently, we ignore ''num_chan' as the driver only 
             supports 1 channel */

          /* Ignore CHAN_CONFIG_F if we are in PC state and chan_id == PCH */
          if( !((rxc_get_state() == RXC_PC_STATE) &&
               (cmd_ptr->chan_config.chan_info[0].chan_id == CAI_PCH)) )
          {
            if( cmd_ptr->hdr.cmd_hdr.sigs == (rex_sigs_type) SRCH_CMD_SIG )
            {
              MSG_HIGH("Got CHAN_CONFIG_F from SRCH", 0, 0, 0);
              raho_flag = TRUE;
            }
    
            /* Start FCCCH state off in AWAKE state */
            rxcidle_set_slotted_status( RXCIDLE_SLOTTED_AWAKE );

            next_state = rxcidle_chan_config 
                            (&cmd_ptr->chan_config.chan_info[0]);
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
            /* check if BC BCCH, copy 40ms skip flag from SRCH to RXC BC BCCH*/
            if (cmd_ptr->chan_config.chan_info[0].ebcsms)
            {
              rxc_slot.skipping = cmd_ptr->chan_config.skip_frame;
            }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
          }
#endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */
          cmd_ptr->hdr.status = RXC_DONE_S;
          break;
        }
#endif /* FEATURE_IS2000_REL_A_CC */

        default:
        {
#ifdef FEATURE_FACTORY_TESTMODE
          if(ftm_mode == FTM_MODE)
          {
          MSG_HIGH( "Bad RXC cmd %d", cmd_ptr->hdr.command ,0, 0 );
          } else {
            ERR_FATAL( "Bad cmd %d", cmd_ptr->hdr.command, 0, 0 );
          }
#else
          ERR_FATAL( "Bad cmd %d", cmd_ptr->hdr.command, 0, 0 );
#endif /* FEATURE_FACTORY_TESTMODE */
        }
      } /* switch */

      /* return item to queue if necessary */
      cmd_done( &cmd_ptr->hdr.cmd_hdr );

      (void)rex_clr_sigs( rex_self(), RXC_CMD_Q_SIG );
    }
    else
    {
      sa_wait_mask = RXC_CMD_Q_SIG | RXC_INT_SIG | rxcidle_wait_mask();

      sigs = rxc_wait(sa_wait_mask);

      /* Check if any fade timer has expired */
      if (rxcidle_process_sigs(sigs, RXC_PC_STATE))
      {
        if ( sigs & RXC_INT_SIG )
        {
          /* do frame counting */
          if( rxc_slot.active)
          {
            /* counting slots */
            if( rxc_slot.slot_frames > 0 )
            {
              rxc_slot.slot_frames--;
            }
#ifdef FEATURE_BROADCAST_SMS
            if( rxc_slot.bcpg_slot_frames > 0 )
            {
              rxc_slot.bcpg_slot_frames--;
            }
            if( rxc_slot.bcsms_slot_frames > 0 )
            {
              rxc_slot.bcsms_slot_frames--;
            }
            MSG_LOW("SlotFr: Asgn=%d, BC=%d, SMS=%d", rxc_slot.slot_frames,
                    rxc_slot.bcpg_slot_frames, rxc_slot.bcsms_slot_frames);

            /* the expected BC didn't start in the slot, we no longer need to
             * monitor for BC data anymore */
            if ((rxc_slot.slot_mask & BC_DATA_SLOT) &&
                (rxc_slot.bcsms_slot_frames <= RXC_BC_PEEK_BOUNDRY) &&
                !rxc_slot.bc_started)
            {
              rxc_slot.bcsms_slot_frames = 0;
            }
#endif /* FEATURE_BROADCAST_SMS */

            MSG_LOW("valid_mask=%d, valid_msg=%d",
                    rxc_slot.valid_msg_mask, rxc_slot.valid_msg, 0);

            /*
             * OOS Enhancement: Send MC a SLOT_R after (slot+1) if a good message CRC
             * after a bad CRC in (slot) and (slot+1).  Still send normal SLOT_R
             * if CRC passes in (slot) and (slot+1)
             */
            {
              byte rpt_slot_mask=0; /* the mask to be passed in SLOT_R */
              static int after_slot_1_cnt=0;   /* > 1 if after (slot+1) && no valid msg yet */

              if( rxc_slot.slot_frames > 0 )
              {   
                after_slot_1_cnt = 0;  /* Countdown is not after (slot+1) */
              }
              if ((rxc_slot.slot_frames==0)&&(rxc_slot.valid_msg_mask&PAGE_SLOT))
              {
                rpt_slot_mask |= PAGE_SLOT;
                after_slot_1_cnt++;   /* Count one slot early since it only takes affect if msg CRC fails */
                if (after_slot_1_cnt == 2)
                {
                    MSG_HIGH("After slot+1, waiting for good CRC to send SLOT_R",0,0,0);
                }
              }
#ifdef FEATURE_BROADCAST_SMS
              if ((rxc_slot.bcpg_slot_frames==0) &&
                  (rxc_slot.valid_msg_mask & BC_PAGE_SLOT))
              {
                rpt_slot_mask |= BC_PAGE_SLOT;
              }
              if ((rxc_slot.bcsms_slot_frames == 0) &&
                  (rxc_slot.valid_msg_mask & BC_DATA_SLOT))
              {
                rpt_slot_mask |= BC_DATA_SLOT;
              }
#endif /* FEATURE_BROADCAST_SMS */

              if ( rxc_slot.valid_msg )
              {
                /* Send SLOT_R if this is our assigned slot (normal case) or
                 * if this is after (slot+1) and no previous valid msg (OOS enhancement).
                 */
                if( (rpt_slot_mask != 0) || (after_slot_1_cnt > 1) )
                {
                  if ((rpt_ptr = (mccrx_rpt_type *)q_get(&mcc_rx_free_q) ) != NULL)
                  {
                    rpt_ptr->hdr.rpt = RXC_SLOT_R;
                    ts_get_20ms_frame_time( rpt_ptr->slot.rpt_time );
                    rpt_ptr->slot.slot_mask = rpt_slot_mask;
                    /* we don't stop counting slot frames like before, the decision
                     * is left for CP to wake. rxc_slot.active cleared when
                     * RXC_SLEEP_F is rx'd */

                    if (after_slot_1_cnt > 1)
                    { 
                      /* We got here after (slot+1) which means (slot) and (slot+1) failed CRC. */
                      MSG_HIGH("Sending SLOT_R after missed slot (OOS)",0,0,0 );
                    }
                    else
                    {
                      MSG_HIGH("Sending SLOT_R (normal case)",0,0,0 );
                    }
                    /* Clear flag so we do not keep sending SLOT_R. */
                    after_slot_1_cnt = 0;

                    rxc_mc_rpt(rpt_ptr );
                    /* clear the bits that triggered the valid msg report, so we
                     * don't send the same report more than once */
                    rxc_slot.valid_msg_mask &= ~rpt_slot_mask;
                  }
                  else
                  {
                    ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
                  }
                }
              }
            }
          }
        } /* end of frame counting */
      } /* if (rxcidle_process_sigs(sigs, RXC_PCH_STATE)) */
    } /* if( (cmd_ptr = (rxc_cmd_msg_type *)q_get(&rxc_cmd_q) ) != NULL ) */


    MSG_LOW("RXC slot_fr=%d",rxc_slot.slot_frames,0,0);
  } /* while */

  /* Just make sure OKTS sig is cleared */
  rxc_vote_not_ok_to_sleep();

  /* Perform exit processing */
  rxc_pc_exit();

  return( next_state );

} /* rxc_pc */



/*===========================================================================

FUNCTION RXC_SET_PC_ISR

DESCRIPTION
  This function sets up the paging channel isr for DEC_INT.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rxc_set_pc_isr ( void )
{
  /* Decoder clk is CHIPX8 in paging state */
  dec_clk_select (DEC_CLK_CHIPX8);
  INTLOCK ();
  tramp_set_isr( TRAMP_RX_ISR, rxc_pc_isr);
  /* Clear DEC DONE int status to make sure ISR is properly serviced.
  */
  HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
  INTFREE ();
}

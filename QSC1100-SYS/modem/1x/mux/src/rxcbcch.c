/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              CDMA2000 Broadcast Control Channel Monitoring

GENERAL DESCRIPTION
  Contains implementation of Physical, LAC and MAC layer of IS2000 release
A Broadcast Control Channel (BCCH) operation.

  
EXTERNALIZED FUNCTIONS
  rxcbcch_init - initializes the BCCH processing state
  rxcbcch_state - BCCH state processing
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcbcch.c_v   1.32   23 Oct 2002 16:26:34   baon  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcbcch.c#7 $ $DateTime: 2008/11/03 13:30:26 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/08   jtm     Modified/Added F3 messages for debugging.
09/26/08   jtm     Klockwork fixes
09/08/08   jtm     Increased rxc_bcch_crc_temp_array size to RXCSAR_MAX_EXPECTED_MSG_LENGTH
                   to prevent overflow when copying the message to the CRC buffer. 
08/15/08   jtm     Added debug messages to cmd processing states.
07/24/08   jtm     Removed FEATURE_BCCH_CH0_WORKAROUND code and make Klockwork fixes.
07/16/08   jtm     Removed FEATURE_RXCCHAIN_DEBUG and FEATURE_BCBCCH_LOG_DEBUG.
                   Reduced debug string sizes for ULC effort.
09/26/07   bn/vlc  Fixed high FER issue on BCCH in fading condition.
05/29/07   vlc     Fixed compiler warnings for rel 0 only build.
05/03/07   cab     Moved to new sleep voting interface
09/19/06   vlc     Fixed lint errors.
06/12/06   awj     #include tramp.h; no longer included through clk.h
06/09/06   trc     Remove T_MSM6700 featurization
05/23/06   trc     Add NCCCH failsafe timer support
03/22/06   kwo     Added calls into srch to report frame stats
09/07/05   rkc     Mainline FEATURE_ZZ2.
08/29/05   bn      Clear the all the decoder status it after installing the isr
08/11/05   bn      Added logic to relay the page match event to MC. Clear up the code
07/25/05   rkc     Fixed lint errors.
07/05/05   bn      Fixed the page match logic in MUX
05/04/05   kwo     srch_idle_page_match() -> srch_mux_page_match()
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
04/08/05   vlc     Added support for FEATURE_IS2000_REL_D TCI to IDLE state
                   transition (for DCA - Direct Channel Assignment).                                           
03/14/05   vlc     Fixed compiler warnings for MSM6700.
03/07/05   bn      Instead of SRCH, MUX need to install the ISR after WAKE_F
03/14/05   vlc     Merged in changes for MSM7500.  (dlb')
02/24/05   vlc     Lock interrupt between clearing DEC_DONE_INT_STATUS and
                   installing RXC ISR.
02/23/05   vlc     Clear the DEC_DONE_INT_STATUS when setting RXC ISR.
12/14/04   ljl     Added event report for BCCH.
09/28/04   sr      Merged in changes from 6550 2009 build
08/09/04   ejv     #include muxmsm.h.
08/09/04   bn/kwo  Added new interfaces to expose chan mod setup parameters
08/09/04   sr      Added line to rxcbcch_config_hw() to clear PDCCH bit in 
                   DEM1X_CHANNEL2 register for MSM6700
07/23/04   jyw     Added the Decoder Interrupt cleaning at the rxcbcch_isr()
07/15/04   bn      Re-Initialize the BCCH info structure when exit
04/20/04   jrp     Fixed lint warnings.
03/03/04   bn      Fixed the SMS address matched logic for back-to-back SMS
11/04/03   bn      Improved the 5ms delay on FCCCH
10/22/03   sr      Fixed compilation warning errors when using RVCT compiler 
09/23/03   bn      Fixed the corner case in the secondary BCCH skip logic
09/16/03   ejv     Updated rxcbcch_config_hw() for 6500.
09/09/03   dna     Merged porting changes for MSM6500
06/30/03   bn      Fixed potential problem in zero-out the BCCH log info buffer
06/16/03   bn      Fixed the "out of buffer dec_free_q" problem. Also fixed the
                   hard-handoff problem when switching from NCCCH to PCH
05/30/03   bn      Replaced MSG_FATAL to MSG_HIGH in rxcbcch_init
05/28/03   vlc     Added support for FEATURE_IS2000_REL_B traffic to idle 
                   transition.
05/14/03   bn      Support frame crc statistic for simultanous FCCCH/BCCH in PLT
05/05/03   bn      Optimized the NCCCH logics and F3 debug messages
04/14/03   bn      Featurize the BCCH channel hard-code value
04/14/03   bn      Used hard-code value of CHAN_1 for BCCH
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
03/05/03   sr      Added support for Sync Channel workaround.
02/25/03   va      Removed changing MC priority.
01/30/03   bt      Changed FAST_SLEEP_CMD for FEATURE_ZZ2.
01/09/03   bt      Added support for FEATURE_ZZ2 and fixed ts warning.
12/12/02   bn      Added logic to handle SLOT_F cmd when transition in 2sd-BCCH
10/21/02   bn      Added support for NCC statistic counters
10/03/02   bn      Added support for Broadcast BCCH
10/22/02   bn      Correct the error in BCCH frame quality logging
10/14/02   sr      Added MSM6100 changes
10/11/02   sr      Reset MC task priority back to MC_PRI in ACC_FADE_F
08/06/02   cr      Added MSM6100 changes.
08/24/02   sr      Modified rxcbcch_sync20_atmr_isr() to properly synchronize
                   programming the sequence repetition in the deinterleaver
                   Added some clean-up to rxcbcch_exit()
08/14/02   sr      Added protection against skipping frames forever after
                   a slot boundary due to bad symbol combiner offset.
08/13/02   sr      Added fast RAHO support
06/25/02   lad     Fixed logic error in frame info logging.
06/03/02   sr      Changed CAIX_BCCH to RXC_CDMA_BCCH
05/31/02   sr      Added RXC_WAKE_F support and no longer blindly skip
                   processing of first frame
04/24/02   lad     Fixed 9600/4800 rates.
04/05/02   lad     Modified SAR interface to use const info in msg_desc.
04/01/02   lad     Added frame info logging, message logging, first frame
                   omission to avoid bad CRC.
03/07/02   lad     Updated/added crc stat messages for PLT.
02/27/02   lad     Removed rxcchain under FEATURE_RXCCHAIN_DEBUG.  Moved 
                   driver usage from rxcchain to this file.
02/15/02   lad     Moved RXC_CHAN_CONFIG_F handling to rxcidle.c.
                   Moved rxcidle_init() call to rxcsync.c.
                   Added deactivation of current BCCH config in rxcbcch_init()
                   Added rxcidle_pending_state() check.
02/05/02   lad     Added PCH and BCCH to RXC_CHAN_CONFIG_F.  
                   Removed flush count from rxcsar_terminate() call.
01/23/02   lad     Update to fix rxtx interface.  Cleanup.
01/17/02   lad     Changed rxcbcch_init() to take rxc_bcch_info_type.
                   Updated rxcsar usage.  Update sleep API usage.
                   Added RXC_CHAN_CONFIG_F and usage of rxcidle, rxcmsg.
11/29/01   lad     Added support for processing frames (use rxcsar), and
                   sending messages to caller.
10/22/01   lad     Added frame statistics (CRC counts).
10/17/01   lad     Added FCCCH command processing.
07/16/01   lad     Created file.

===========================================================================*/

#include "customer.h"
#include "target.h"
#include "comdef.h"

#ifdef FEATURE_IS2000_REL_A_CC

#include "assert.h"
#include "err.h"
#include "log_codes.h"
#include "log.h"
#include "msg.h"
#include "queue.h"
#include "task.h"
#include "tramp.h"

#include "cai.h"
#include "caix.h"
#include "mux_logtypes.h"
#include "rx.h"
#include "rxc.h"
#include "rxcbcch.h"
#include "rxccommon.h"
#include "rxcidle.h"
#include "rxcmsg.h"
#include "rxcsar.h"
#include "rxcsync.h"
#include "rxtx.h"
#include "deint.h"
#include "dec5000.h"
#include "msm_drv.h"
#include "muxmsm.h"


#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif

#include "ts.h"

#include "srch_mux.h"
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
#include "rxcnccch.h"
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
#include "mclog.h"

/*--------------------------------------------------------------------------
 * Definitions and Declarations
 *--------------------------------------------------------------------------*/
extern boolean raho_flag;

extern uint16 parm_inc ( uint16 parm_id, uint8  inc); 

/* Function called by RXCSAR. */
static void * rxcbcch_msg_alloc (void *client_data, unsigned int length);
static void rxcbcch_msg_deliver (void *client_data, unsigned int length, 
                                 void *data, qword *frame_num);
static void rxcbcch_msg_discard (void *client_data, void *data);
static void rxcbcch_msg_event (void *client_data, rxcsar_msg_event_type event);

static const rxcsar_msg_const_desc_type rxcbcch_msg_const_desc = {
  LOG_BCCH_SIGNALING_C,
  TRUE,
  6,
  rxcbcch_msg_alloc,
  rxcbcch_msg_deliver,
  rxcbcch_msg_discard,
  rxcbcch_msg_event
};
/* store buffer of the BCCH message to check for CRC */
static byte rxc_bcch_crc_temp_array[RXCSAR_MAX_EXPECTED_MSG_LENGTH];
static rxcsar_fcsch_frame_desc_type rxcbcch_sar_frame_desc;
rxcsar_msg_desc_type rxcbcch_sar_msg_desc = {
  NULL, &rxcbcch_msg_const_desc, rxc_bcch_crc_temp_array, 0, 0,0
};

#define RXCBCCH_NUM_LOG_FRAME_PER_BYTE   (4)
/* Logging Info */
#define RXCBCCH_FRAME_LOG_HISTORY_SIZE (16*RXCBCCH_NUM_LOG_FRAME_PER_BYTE)

static union {
  mux_bcch_frame_info_type log;
  
  /*lint -e754 -save buf is not referenced but is needed to assure log buffer is large enough */
  /* This member assures the log buffer is large enough */
  byte buf[FPOS (mux_bcch_frame_info_type, frame_quality) + 
    (RXCBCCH_FRAME_LOG_HISTORY_SIZE + (RXCBCCH_NUM_LOG_FRAME_PER_BYTE-1)) /RXCBCCH_NUM_LOG_FRAME_PER_BYTE];//lint !e30 FPOS
  /*lint -restore */
} rxcbcch_log_info;

/* Frame statistics */
/* # of BCCH frames with good CRCs */
rxc_crc_cnt_type rxcbcch_frame_crc_stats; /*lint -e552, it is used */
static rxc_crc_cnt_type rxcbcch_msg_crc_stats;
#ifdef FEATURE_PLT
#error code not present
#endif


rxcbcch_info_type rxcbcch_info = {
  CAI_CODE_RATE_INVALID,
  CAI_BCCH_RATE_INVALID,
  0,
  TRUE,
  ATMR_INVALID_HANDLE
};

/* indexed using cai_code_rate_type */
static const dec_sch_rate_type rxcbcch_mac_sch_map_tbl [] = { 
  DEC_SCH_RATE_2X,
  DEC_SCH_RATE_1X
};

/* Used to indicate the first frame condition to align timing and 
   detect that the frame is expected to be invalid. */

/* Indexed by cai_bcch_rate_type, this table gives the mask to used to 
   determine if frame boundary conditions (160, 80, and 40ms frames). */
static const byte rxcbcch_frame_bnd_mask_tbl [] = {0x07, 0x03, 0x01};


/* <EJECT> */
/*--------------------------------------------------------------------------
 * Forward Declarations
 *--------------------------------------------------------------------------*/

static boolean rxcbcch_config_hw (void);
static void rxcbcch_sync20_atmr_isr (void);
static void rxcbcch_isr (void);
extern void rxc_config_mac_for_ch1 (cai_code_rate_type code_rate);


static rxc_state_type rxcbcch_process_cmd (void);
static void rxcbcch_frame_info_log_init (void);
static void rxcbcch_log_frame_info (boolean frame_quality_ind);
static void rxcbcch_reset_stats (void);

static boolean program_zero_seq_rep_done = TRUE;
static boolean program_seq_rep_done = TRUE;

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
extern uint16 mcc_bc_match(byte * pc_hdr);
boolean rxc_bc_bcch_peek_ms_address(byte *ptr);
static uint32  rxc_bcbcch_frame_counter = 0;
static uint32  rxc_bcbcch_frame_at_init = 0;
boolean rxc_bcbcch_first_frame = FALSE;
boolean rxc_bcbcch_peek_sms_msadress_done = FALSE;
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

boolean rxc_read_bcch_data_from_decoder = FALSE;

#ifdef FEATURE_IS2000_REL_A_CC_SMMC
/* use channel 1 for BCCH in simultaneous FCCCH/BCCH */
static deint_ch_id_type  rxcbcch_chan_id = DEINT_CH1;
#else
static deint_ch_id_type  rxcbcch_chan_id = DEINT_CH2;
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

boolean rxc_nccch_deint_ram_mapping_done = FALSE;

const rxc_chan_demod_config_param_type  
rxc_bcch_demod_config_param_tab[CAI_CODE_RATE_COUNT][CAI_BCCH_RATE_COUNT] =
{
   /*cr 1/4, dr 4800 => sr 32 chips, mac rnd = 2, repeat fac = 1*/ 
  {{SPR_32_CHIP_V,2,DMOD_REP_FACTOR_1},
   /* 1/4:9600  => sr 32 chips, mac rnd = 2, repeat fac = 1*/
  {SPR_32_CHIP_V,2,DMOD_REP_FACTOR_1},
   /* 1/4:19200 => sr 32 chips, mac rnd = 2, repeat fac = 1*/ 
  {SPR_32_CHIP_V,2,DMOD_REP_FACTOR_1}},
   /*1/2:4800  => sr 64 chips, mac rnd = 3, repeat fac = 1*/ 
  {{SPR_64_CHIP_V,3,DMOD_REP_FACTOR_1},
   /* 1/2:9600 => sr 64 chips, mac rnd = 3, repeat fac = 1*/
  {SPR_64_CHIP_V,3,DMOD_REP_FACTOR_1}, 
   /* 1/2:19200 => sr 64 chips, mac rnd = 3, repeat fac = 1*/
  {SPR_64_CHIP_V,3,DMOD_REP_FACTOR_1} } 
};

extern void rxcfccch_exit_clean_up(void);

/* <EJECT> */
/*--------------------------------------------------------------------------
 * Public Function Implementations
 *--------------------------------------------------------------------------*/

/*=========================================================================*/
boolean
rxcbcch_init
(
  rxc_state_type curr_state,      /* Current RXC state machine state */
  rxc_bcch_info_type *config_ptr, /* Configuration pointer */
  boolean  init_secondary_bcch    /* indicated if secondary BCCH is active */
)
{
  boolean success = FALSE;

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  qword frame;
  unsigned int bcbcch_40ms_frame_bd;
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  if (config_ptr != NULL) 
  {
    if (curr_state == RXC_SC_STATE || curr_state == RXC_FCCCH_STATE ||
        curr_state == RXC_BCCH_STATE 
#ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
        || curr_state == RXC_PC_STATE
#endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
        || (curr_state == RXC_NCCCH_STATE)
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
#ifdef FEATURE_IS2000_REL_B
        || (curr_state == RXC_TC_STATE)
#endif /* FEATURE_IS2000_REL_B */
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */
       ) 
    {
      /* Set the slot to SLEEP so that the ATMR won't mess it up 
      when the init function has not finished yet */
      rxcidle_set_slotted_status( RXCIDLE_SLOTTED_ASLEEP );

      /* for some unknown reason, the DEINT/DECODER HW in some unknown state. 
      In such state, no matter how SW config BCCH, BCCH still be failed. The 
      only thing SW can do is to reset the DEINT/DECODER HW to put it
      into a known state so that SW can config BCCH correctly. */
      MSM_LOCK();
      HWIO_OUT(DINT_RESET, 0x1);
      MSM_UNLOCK();

      /* call the FCCCH cleanup function to force a FCCCH reconfig */
      rxcfccch_exit_clean_up();

      /* check to make sure all the code rate and data rate in the
      valid ranges */
      if (config_ptr->crate < CAI_CODE_RATE_COUNT ||
          config_ptr->rate < CAI_BCCH_RATE_COUNT) 
      {

        /* Store channel configuration information. */
        rxcbcch_info.code_rate = config_ptr->crate;
        rxcbcch_info.data_rate = config_ptr->rate;
        rxcbcch_info.code_chan = config_ptr->code_chan;
        rxcbcch_info.first_frame = FALSE;
     
        /* Make sure page matching is completely disabled */
        rxc_slot.page_match = FALSE;
        
        /* check of the deint ram has been mapped for NCCCH, if not
        then go ahead and map it. It will map for both channel 1 and 2 */
        if (!rxc_nccch_deint_ram_mapping_done)
        {
          deint_allocate_ram_nccch();
          rxc_nccch_deint_ram_mapping_done = TRUE;
        }

        /* call the function from SRCH to get the channel ID, but for 
        now, just use the hard code value of channel #1 */
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
        rxcbcch_chan_id = DEINT_CH1; /* use channel 1 for BCCH in simultaneous FCCCH/BCCH */
#else
        rxcbcch_chan_id = DEINT_CH2;
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

        success = rxcbcch_config_hw ();

        program_zero_seq_rep_done =  FALSE;
        program_seq_rep_done =  FALSE;
      }
      /* The action timer ISR will set these at the approrpiate time */
      /* rxcbcch_chan_id is guaranteed to be valid.  Return value "success" is ignored */
      (void) deint_config_20ms_frame_boundary (rxcbcch_chan_id, FALSE);

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      /* in most case RXC we get a WAKE_UP command from MC first before the config cmd
       * from SRCH, and the rxc_slot.slot_mask and other control vars will be set up
       * correstly, but in some case, RXC can get a config cmds before the WAKE_UP cmd.
       * In those, we want to set rxc_slot.slot_mask to BC DATA SLOT
       */
      if ((init_secondary_bcch) && ((rxc_slot.slot_mask & BC_DATA_SLOT) == 0))
      {
        rxc_slot.slot_mask |= BC_DATA_SLOT;
        MSG_HIGH("RXC got 2sd BCCH CFG before WAKE-UP cmd",0,0,0);
      }
      /* For BCBCCH, SRCH configures the secondary BCCH only just 20 ms
      before the BS transmitts the first BCSMS, so we need to program
      the sequences repettion to zero and the frame boundary to TRUE, 
      if we wait for the ATMR to perform the initialization, then it 
      is too late
      */
      if(rxc_slot.slot_mask & BC_DATA_SLOT)
      {
        /* reset the first BC BCCH frame flag */
        rxc_bcbcch_first_frame = FALSE;
        /* need to peek into the first BC BCCH frame to see the MS address*/
        rxc_bcbcch_peek_sms_msadress_done = FALSE;
        /* get the system time */
        ts_get_20ms_frame_time (frame);
        /* check if the next frame is the 40 ms frame boundary */
        rxc_bcbcch_frame_at_init = qw_lo(frame);
        bcbcch_40ms_frame_bd = ((rxc_bcbcch_frame_at_init + 1) % 2);

        if(bcbcch_40ms_frame_bd == 0)
        {
          /* We programmed zero sequence repetition, but need a 40ms boundary
          to make sure it actually loads. */
          program_zero_seq_rep_done= TRUE;

          /* The SYNC20 signal is used to specify a frame boundary in the
          deinterleaver.  BCCH sequence repetition consists of repeated 40ms
          frames (provides redundancy == BCCH can tolerate lower power).
          The deinterleaver performs a summation of symbol energies of each
          repeated frame.  It needs to know when the 40ms frame boundary occurs,
          which is every even frame.  If sequence repetition > 1, the sync20
          boundary  still needs to be set every 40 ms, but the ISR is not
          installed until the final frame of the repetition. */
          /* rxcbcch_chan_id is guaranteed to be valid.  Return value "success" is ignored */
          (void) deint_config_20ms_frame_boundary( rxcbcch_chan_id, TRUE );
        }
        MSG_HIGH("Init 2sd BCCH on CH %d,drate=%d,crate=%d", rxcbcch_chan_id, config_ptr->rate,
        config_ptr->crate);
      }
      else
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
      {
        /* since we know this is the start of the NCC, it is save to clear the decoder queue */
        rxc_clear_decoder_queue (); 
        MSG_HIGH("Init prim BCCH on CH %d,drate=%d,crate=%d", rxcbcch_chan_id, config_ptr->rate,
        config_ptr->crate);
      }

      if (success) 
      {
        /* Initialize the BCCH message parser */
        (void) rxcbcch_frame_proc_init (); /* Return value is always TRUE */
        rxcbcch_reset_stats ();
        rxcbcch_frame_info_log_init ();
        rxc_read_bcch_data_from_decoder = FALSE;
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
        if (rxcnccch_chan_enable_bits == 0)
        {
          INTLOCK ();
          tramp_set_isr (TRAMP_RX_ISR, rxcnccch_isr);
          /* Clear DEC DONE int status to make sure ISR is properly serviced.
          */
          HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
          INTFREE ();
        }
        if(rxc_slot.slot_mask & BC_DATA_SLOT)
        {
          rxcnccch_chan_enable_bits = (rxcnccch_chan_enable_bits | RXCNCCCH_SECONDARY_BCCH_BIT);
        }
        else
        {
          rxcnccch_chan_enable_bits = (rxcnccch_chan_enable_bits | RXCNCCCH_PRIMARY_BCCH_BIT);
        }
#else
        INTLOCK ();
        tramp_set_isr (TRAMP_RX_ISR, rxcbcch_isr);
        /* Clear DEC DONE int status to make sure ISR is properly serviced.
        */
        HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
        INTFREE ();
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
      }
    } 
    else 
    {
      ERR_FATAL ("Bad state %d", curr_state, 0, 0);
    }
  }
   /* Now we can let the cat out */
   rxcidle_set_slotted_status( RXCIDLE_SLOTTED_AWAKE );
  return success;
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_STATE

DESCRIPTION
  BCCH state processing.

DEPENDENCIES
  rxcbcch_init() must have been executed successfully prior to entering this
  state.

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
rxc_state_type
rxcbcch_state (void)
{
  rxc_state_type next_state = RXC_BCCH_STATE;
  rxc_dec_type *buf_ptr; /* pointer to decoder data buffer */
  rex_sigs_type sigs;
  rex_sigs_type sa_wait_mask;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/

//TODO: Make this a runtime check (return FALSE) instead of ASSERT

  ASSERT(rxcbcch_info.code_rate < CAI_CODE_RATE_COUNT &&
         rxcbcch_info.data_rate < CAI_BCCH_RATE_COUNT);



  MSG_LOW("Start BCCH state", 0, 0, 0);

  /* Clear OKTS sig - We don't want to sleep yet */
  rxc_vote_not_ok_to_sleep();


    while ((next_state = rxcbcch_process_cmd ()) == RXC_BCCH_STATE) {

      sa_wait_mask = RXC_CMD_Q_SIG | RXC_INT_SIG | rxcidle_wait_mask ();

      sigs = rxc_wait (sa_wait_mask);
      
      /* Check if we have any signals to process.  Afterward, complete loop
         by checking the RXC command queue. */
      if (rxcidle_process_sigs (sigs,RXC_BCCH_STATE))
      {
        if (sigs & RXC_INT_SIG)
        {
          (void) rex_clr_sigs (&rx_tcb, RXC_INT_SIG);
           while ((buf_ptr = (rxc_dec_type *) q_get (&rxc_dec_q)) != NULL) 
           {

           /* If not BCCH state frame, drop the frame */
             if (buf_ptr->state == RXC_BCCH_STATE) 
             {
               rxcbcch_process_frames (buf_ptr);
             }
             q_put (&rxc_dec_free_q, &buf_ptr->link);
           }

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

                   MSG_HIGH("Send RXC_SLOT_R",0,0,0);

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
      }
    }


#ifdef FEATURE_IS2000_REL_A_CC_SMMC
  /* if the next state is not NCCCH, then we need to clean up, otherwise,
  BCCH still need to be up for continuous monitor*/
  if (next_state != RXC_NCCCH_STATE)
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
  {
    rxcbcch_exit ();
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
    rxcnccch_chan_enable_bits = 0;
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
  }
  return (next_state);
} /* rxcbcch_state */


/* <EJECT> */
/*--------------------------------------------------------------------------
 * Private Function Implementations
 *--------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION RXCBCCH_CONFIG_HW

DESCRIPTION
  This routine configures the hardware to decode the BCCH.

DEPENDENCIES
  None.

RETURN VALUE
  boolean indicating success.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean 
rxcbcch_config_hw (void)
{
  boolean success = TRUE;

  /* Set RX chain hardware in reverse processing order */
  
  /* Place decoder in packet mode */
  dec_tc_init ();

  /* Decoder clk is TCXO in BCCH. */
  dec_clk_select (DEC_CLK_TCXO);
  
  /* Disable the frame boundary trigger until timing
   * is established in the ATMR ISR */

  if (!deint_config_20ms_frame_boundary (rxcbcch_chan_id, FALSE)) /*lint -e571 */
  {
    ERR_FATAL("Bad ch id %d",rxcbcch_chan_id,0,0);
  }
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
  /* Deinterleaver next */
  /* Ignoring return value "success", which is always TRUE. */
  (void) deint_set_bcch_mode (rxcbcch_info.data_rate, rxcbcch_info.code_rate, 
                       rxcbcch_chan_id,rxcnccch_chan_enable_bits);
#else
  /* Deinterleaver next */
  deint_set_bcch_mode (rxcbcch_info.data_rate, rxcbcch_info.code_rate, rxcbcch_chan_id, 0);
#endif

  MSM_LOCK();

  DEMOD_ENABLE_Q_CHANNEL();

  if (rxcbcch_chan_id == DEINT_CH1)
  {
    DEMOD_ENABLE_CH1_HW_COMBINING();
  }
  else
  {
    DEMOD_ENABLE_CH2_HW_COMBINING();
    MSM_CLEAR( DEM1X_CHANNEL2_FRAME, PDCCH_1X_SEL );
  }
  
  /* for BCCH, it can only use channel 1 or channel 2 */
  if (rxcbcch_chan_id == DEINT_CH2)
  {
    rxc_set_mac_rnd(DEINT_RC3, rxcbcch_mac_sch_map_tbl[
                      RXC_UPPER_BOUNDS_CHECK(rxcbcch_info.code_rate, 
                                             CAI_CODE_RATE_COUNT)], 
                      DEC_CONVOLUTIONAL);
  }
  else
  {
    rxc_config_mac_for_ch1 (rxcbcch_info.code_rate);
  }

  MSM_UNLOCK();

  if (rxcbcch_info.atmr_handle != ATMR_INVALID_HANDLE) 
  {
      MSG_MED("rel atmr hndl", 0, 0 ,0);

    /* Free action timer resource. */
    atmr_set (rxcbcch_info.atmr_handle, NULL, ATMR_5MS_FRAME(NONE), 
      (atmr_pcg_offset_enum_type) 0, (unsigned long) 0);
  }

  rxcbcch_info.atmr_handle = atmr_get (ATMR_TIME_1);

  if (rxcbcch_info.atmr_handle == ATMR_INVALID_HANDLE) 
  {
    MSG_HIGH("rxc can't get atmr hndl", 0, 0, 0);
    success = FALSE;
  } 
  else 
  {
  /* since this action timer is used for BCCH, and BCCH is
  set up in the second 5ms slot, so the action timer need to be
  in the third slot */
    atmr_set (rxcbcch_info.atmr_handle, rxcbcch_sync20_atmr_isr,
      ATMR_5MS_FRAME_3, ATMR_PCG_OFFSET(1), 0);
  }

  return success;
}


/*===========================================================================

FUNCTION RXCBCCH_SYNC20_ATMR_ISR

DESCRIPTION
  This routine determines frame timing for BCCH.  It is executed when the
  action timer expires, at which time this routine decides if a frame boundry 
  is pending on the next SYNC20.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void 
rxcbcch_sync20_atmr_isr (void)
{
  qword frame;
  unsigned int bcch_20ms_cnt = 0;
  boolean bcch_40ms_bdy = FALSE;  /* true if next 20 ms boundary is a 40 boundary */
  boolean bcch_frame_bdy = FALSE; /* true if next 20 ms boundary is a BCCH frame boundary */
  cai_bcch_rate_type bcch_rate ;

  /* This function assumes the following enum values, defined in cai.h. */
  ASSERT (CAI_BCCH_4800_RATE == 0);
  ASSERT (CAI_BCCH_9600_RATE == 1);
  ASSERT (CAI_BCCH_19200_RATE == 2);

  bcch_rate = rxcbcch_info.data_rate ;

  /* ATMR needs to establish frame timing */
  ts_get_20ms_frame_time (frame);

  if ( rxcidle_get_slotted_status() != RXCIDLE_SLOTTED_ASLEEP )
  {
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
    if (rxc_slot.slot_mask & BC_DATA_SLOT)
    {
      uint32 current_bcbcch_frame;

      /* get the current system time frame slot */
      current_bcbcch_frame = (qw_lo(frame));

      /* since the current_bcbcch_frame is the low part of the system time, so there is a
      * chance (like winning a lottory) it could roll over,so we need to have the logic to
      * account for that
      */
      if(current_bcbcch_frame >= rxc_bcbcch_frame_at_init)
      {
        rxc_bcbcch_frame_counter = current_bcbcch_frame   - rxc_bcbcch_frame_at_init;
      }
      else
      {
        rxc_bcbcch_frame_counter = rxc_bcbcch_frame_counter + 1;
      }
      /* make sure the bcch_int and the ATRM don't happen on the same 20 ms slot..
      * if they are in the same slot, then for this slot, the ATMR should do nothing
      * becuase the bcch_init() already did all the thing we suppose to do for this slot
      */
      if (rxc_bcbcch_frame_counter > 0)
      {
        if (!program_zero_seq_rep_done)
        {
          /* for secondary BCCH, usually, this is done in the bcch_init() function, in the previous
          frame, Just in case, it is was not done in the bcch_init(), because the previous frame is
          the 40 ms boundary, so we will delay from the bcch_init function and do it here sine
          the next frame is the 40 ms boundary */
          program_zero_seq_rep_done = TRUE;
          rxc_bcbcch_frame_counter = 0;
          bcch_frame_bdy = FALSE; /* No BCCH_ISR for this time */
          bcch_40ms_bdy = TRUE; /* start to indicate the next frame is the first start 40 ms boundary */
          rxc_bcbcch_frame_at_init =  (qw_lo(frame));
        }
        else
        {
          /* since for the secondary BCCH, the frame is only in synch with 40 ms, not like the
          primary BCCH, the frame is in sync with 160 ms. Because of the difference, in secondary
          BCCH, we need to have our own time counter, we can't reply on the system absoluted time.
          as long as we know when the BS start to TX the first BCBCCH frame, then
          we can set our time counter to zero and all the subsequence time event, will derive from
          the counter
          */
          /* Calculate frame time based on the next frame (relative to this ISR) */
          bcch_20ms_cnt = (rxc_bcbcch_frame_counter) & 
            rxcbcch_frame_bnd_mask_tbl[
              RXC_UPPER_BOUNDS_CHECK(bcch_rate, CAI_BCCH_RATE_COUNT)];

          bcch_40ms_bdy = (bcch_20ms_cnt % 2 == 0) ? TRUE : FALSE ;
          bcch_frame_bdy = (bcch_20ms_cnt == 0) ;
        }
      }
      else
      {
        MSG_HIGH("Bcbcch init/atmr runs in same slot,return ", 0, 0, 0);
        return;
      }
    }
    else
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
    {
      /* Calculate frame time based on the next frame (relative to this ISR) */
      bcch_20ms_cnt = (qw_lo(frame)+1) & 
        rxcbcch_frame_bnd_mask_tbl[
          RXC_UPPER_BOUNDS_CHECK(bcch_rate, CAI_BCCH_RATE_COUNT)] ;

      bcch_40ms_bdy = (bcch_20ms_cnt % 2 == 0) ? TRUE : FALSE ;
      bcch_frame_bdy = (bcch_20ms_cnt == 0) ;
    }

    if( (!program_zero_seq_rep_done) && (bcch_20ms_cnt == 0) )
    {
      /* We programmed zero sequence repetition, but need a 40ms boundary
      to make sure it actually loads. */
      program_zero_seq_rep_done= TRUE;
      MSG_HIGH("BCCH ATMR fin prg 0 seq!",0,0,0);
    }
    if( (program_zero_seq_rep_done) && (!program_seq_rep_done) && (bcch_20ms_cnt == 1) )
    {
      /* Sequence repetition was previously programmed with zero
      to achieve synchronization.  Now, re-program sequence
      repetition with actual value to be used on BCCH.  */
      deint_bcch_seq_repetition(bcch_rate,rxcbcch_chan_id);
      program_seq_rep_done = TRUE;
      MSG_HIGH("BCCH ATMR fin prg act seq=%d,ch=%d",bcch_rate,rxcbcch_chan_id,0);
    }

    /* The SYNC20 signal is used to specify a frame boundary in the
    deinterleaver.  BCCH sequence repetition consists of repeated 40ms
    frames (provides redundancy == BCCH can tolerate lower power).
    The deinterleaver performs a summation of symbol energies of each
    repeated frame.  It needs to know when the 40ms frame boundary occurs,
    which is every even frame.  If sequence repetition > 1, the sync20
    boundary  still needs to be set every 40 ms, but the ISR is not
    installed until the final frame of the repetition. */

    if (!deint_config_20ms_frame_boundary( rxcbcch_chan_id, bcch_40ms_bdy )) /*lint -e571 */
    {
      ERR_FATAL("Bad ch id %d",rxcbcch_chan_id,0,0);
    }

    if (bcch_frame_bdy && program_seq_rep_done)
    {
      rxc_read_bcch_data_from_decoder = TRUE;
      MSG_HIGH("BCCH ATMR set rxc_read_bcch_data_from_decoder = TRUE",0,0,0);
    }
    else
    {
      rxc_read_bcch_data_from_decoder = FALSE;
    }
  }
}



/*===========================================================================

FUNCTION RXCBCCH_PROCESS_CMD

DESCRIPTION
  This function is the interrupt handler for BCCH frames.  It is called from
  the RX chain controller when a BCCH frame is received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static rxc_state_type
rxcbcch_process_cmd (void)
{
  rxc_cmd_msg_type *cmd_ptr = NULL;
  rxc_state_type next_state = RXC_BCCH_STATE;
  boolean success = TRUE;

  (void) rex_clr_sigs (&rx_tcb, RXC_CMD_Q_SIG);

  while (next_state == RXC_BCCH_STATE && 
         (cmd_ptr = (rxc_cmd_msg_type *) q_get (&rxc_cmd_q)) != NULL) {

    switch (cmd_ptr->hdr.command) {
      case RXC_CHAN_CONFIG_F:
      {
        if( cmd_ptr->hdr.cmd_hdr.sigs == (rex_sigs_type) SRCH_CMD_SIG )
        {
          MSG_MED( "Got RXC_CHAN_CONFIG_F from SRCH", 0, 0, 0);
          raho_flag = TRUE;
        }
        else
        {
          MSG_MED( "Got RXC_CHAN_CONFIG_F", 0, 0, 0);
        }
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
        next_state = rxcidle_nccchan_config (&cmd_ptr->chan_config);
#else
        next_state = rxcidle_chan_config (&cmd_ptr->chan_config.chan_info[0]);
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
        /* check if BC BCCH, copy the 40ms skip flag from SRCH to RXC BC BCCH*/
        if (cmd_ptr->chan_config.chan_info[0].ebcsms)
        {
          rxc_slot.skipping = cmd_ptr->chan_config.skip_frame;
        }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

        break;
      }
  
      case RXC_EXIT_F:
      {
        MSG_MED( "Got RXC_EXIT_F", 0, 0, 0);
        
        next_state = RXC_EXIT_STATE;   
        break;
      }
  
      case RXC_SC_F:
      {
        MSG_MED( "Got RXC_SC_F", 0, 0, 0);

        /* initialize Sync channel state */
        rxc_sc_init (RXC_PC_STATE);
        next_state = RXC_SC_STATE;
        break;
      }
  
      case RXC_CDMA_F:
      {
        MSG_MED( "Got RXC_CDMA_F", 0, 0, 0);

        /* --------------------------------------------------
        ** Transition back to CDMA init state without exiting
        ** rxc_subtask()
        ** -------------------------------------------------- */

        /* --------------------------------------
          ** Clean up before switching to CDMA mode
          ** -------------------------------------- */
        rxc_exit ();

        next_state = RXC_CDMA_STATE;
        
        break;
      }

      case RXC_ACC_FADE_F:
      {
        MSG_MED( "Got RXC_ACC_FADE_F", 0, 0, 0);
        rxcidle_set_fade_timer (RXCIDLE_ACCESS_FADE);

        break;
      }
  
      case RXC_IDL_FADE_F:
      {
        MSG_MED( "Got RXC_IDL_FADE_F", 0, 0, 0);
        rxcidle_set_fade_timer (RXCIDLE_IDLE_FADE);

        break;
      }
  
      case RXC_IDLE_HO_F:
      {
        MSG_MED( "Got RXC_IDLE_HO_F", 0, 0, 0);
        //TODO: Determine # of subframes needed to flush
        rxcsar_terminate (&rxcbcch_sar_msg_desc, 8);
        /* flush the accumalated bcch logging buffer */
        rxcbcch_frame_info_log_send ();
        break;
      }
      
#ifdef FEATURE_FACTORY_TESTMODE
      case RXC_PC_F: //TODO
        {
          MSG_MED( "Got RXC_PC_F", 0, 0, 0);
          if (ftm_mode == FTM_MODE) {
            break;
          }
          /* Fall through to default: case */
        }
#endif /* FEATURE_FACTORY_TESTMODE */

      case RXC_SLOT_F: //XXX
      {
        MSG_MED( "Got RXC_SLOT_F", 0, 0, 0);
        /* Make sure page matching is completely disabled */
        rxc_slot.page_match = FALSE;
        rxc_slot.blank      = FALSE;

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
        if ((cmd_ptr->wake.slot_mask & BC_DATA_SLOT) == 0)
#endif  /*  FEATURE_IS2000_REL_A_CC_BCSMS */
        {
              rxc_slot.skipping   = FALSE;
        }

        INTLOCK();
        /* Clear OKTS sig - We don't want to sleep now */
        rxc_vote_not_ok_to_sleep();

        rxc_slot.slot_mask = cmd_ptr->wake.slot_mask;
        rxc_slot.valid_msg_mask |= cmd_ptr->wake.slot_mask;
        rxc_slot.slot = cmd_ptr->wake.slot;

        rxc_isr_cmd_buf_ptr = (rxc_isr_cmd_type *) q_get (&rxc_isr_free_q);
        if (rxc_isr_cmd_buf_ptr != NULL)
        {
          /* Send a command to the interrupt handler to reset frame count */
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
        rxc_slot.valid_msg = FALSE;
        cmd_ptr->hdr.status = RXC_DONE_S;
        break;
      }
        
      case RXC_WAKE_F:
      {

        MSG_MED( "Got RXC_WAKE_F", 0, 0, 0);

        /* Set flush count to 0 in case an Idle HO occured before sleep */
        rxcsar_terminate (&rxcbcch_sar_msg_desc, 0);

        /* call the function to install the corresponding decoder ISR */
        rxc_idle_activate();

        INTLOCK ();

        /* Clear OKTS sig - We don't want to sleep now */
        rxc_vote_not_ok_to_sleep();

        INTFREE ();

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
        /* we don't want to set the skip flag to TRUE in case
        * of BC BCCH, because for BC BCCH, the decision of skipping
        * will send send in the chan config cmd from SRCH, we will
        * set it when we receive the CHAN_CONFIG_CMD 
        */
        if ((cmd_ptr->wake.slot_mask & BC_DATA_SLOT) == 0)
#endif /*  FEATURE_IS2000_REL_A_CC_BCSMS */
        {
          /* Reinitialize slot counter */
          rxc_slot.skipping = TRUE;
        }

        /* Reset the frame count used during skipping */
        rxc_slot.frame_skip_cnt = 0;

        /* Clear out path clear control flag */
        rxc_slot.blank = FALSE;

        rxc_slot.slot_mask = cmd_ptr->wake.slot_mask;
        rxc_slot.valid_msg_mask |= cmd_ptr->wake.slot_mask;
        rxc_slot.slot = cmd_ptr->wake.slot;

        /* XXX The ISR command is for rxc_slot processing.
        Send a command to the interrupt handler to reset frame count */
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

        rxc_slot.valid_msg = FALSE;

        /* start Idle fade timer */
        rxcidle_set_fade_timer (RXCIDLE_IDLE_FADE);
        break;

      }
      case RXC_SLEEP_F:
      {
        rxcidle_set_slotted_status( RXCIDLE_SLOTTED_ASLEEP );

        MSG_MED( "Got RXC_SLEEP_F", 0, 0, 0);

        /* Reset frame processor */
        rxcsar_terminate (&rxcbcch_sar_msg_desc, 0);

        /* stop running slot timer also */
        rxc_slot.active = FALSE;

        /* reset all valid message reporting counters */
        rxc_reset_frame_count ();

        rxcidle_clr_fade_timer ();

        /* Disable RX ISR as we are done demodulating frames */
        rxc_clear_isr ();

        /* Clear buffers from decoder queue. */
        rxc_clear_decoder_queue ();

        /* Send accumulated log, if any. */
        rxcbcch_frame_info_log_send ();

        /* Tell the sleep task that we're ready for the CPU to halt
        ** The sleep task is lower priority, so it won't do anything
        ** until the RXC task suspends.
        */
        rxc_vote_ok_to_sleep();
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
    
//XXX Note: Copied from rxc_pc() - not sure why this is called twice.
    (void) rex_clr_sigs (&rx_tcb, RXC_CMD_Q_SIG);
  }

  return next_state;
}

/*===========================================================================

FUNCTION RXCBCCH_ISR

DESCRIPTION
  This function is the interrupt handler for BCCH frames.  It is called from
  the RX chain controller when a BCCH frame is received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void 
rxcbcch_isr (void)
{
  rxc_dec_type *buf_ptr = NULL;
  rxc_isr_cmd_type *cmd_ptr = NULL;
  dec_frame_status_type dec_status = {0, 0, 0, 0};
  uint8 dec_int_status;

  dec_int_status = (uint8) HWIO_IN(DEC_DONE_INT_STATUS);
  HWIO_OUT(DEC_DONE_INT_STATUS, dec_int_status); //clear the interrupt
  if(!(dec_int_status & 0x2))
  {
    return;
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* this flag is set and reset from the BCCH-ATMR, if it is true, then slot is
  the BCCH-frame slot, go ahead and read it from the decoder */
  if (!rxc_read_bcch_data_from_decoder)
  {
    return;
  }
#ifdef FEATURE_DEINT_RAM_LOGGING
#error code not present
#endif

  /* Check to see if there are pending commands for the interrupt handler */
  while( (cmd_ptr = (rxc_isr_cmd_type *)q_get(&rxc_isr_q)) != NULL )
  {
    //TODO: Handle ISR commands, if any.
    switch( cmd_ptr->cmd )
    {
      case RXC_WAKE_F:
        {
          qword tmp_qw;
          MSG_MED( "Got RXC_WAKE_F", 0, 0, 0);

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
  
          if (rxc_slot.slot_mask & PAGE_SLOT)
          {
            /* set 2 slot timer for assigned paging slot */
            rxc_slot.slot_frames =
                4 * (rxc_slot.slot - rxc_slot.slot_now) + 8
                - ( word )(qw_lo( rxc_dec_frame ) & 03);//lint !e734 Loss of precision (assignment) (18 bits to 16 bits)
          }
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
          if ( rxc_slot.slot_mask & BC_DATA_SLOT )
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
        MSG_ERROR("Bad ISR cmd = %d", cmd_ptr->cmd, 0, 0);
        break;
    } /* switch */

    /* return the memnory back to the free queue */
    q_put (&rxc_isr_free_q, &cmd_ptr->link);

    /* Notify task of completion of ISR command. */
    (void) rex_set_sigs (&rx_tcb, RXC_ISR_CMD_SIG);
  }

/*-------------------------------------------------------------------------*/
  /* perform normal interrupt processing */

  if ( rxc_slot.skipping == TRUE )
  {
    
    if( rxcidle_first_frame( RXC_CDMA_BCCH ) == TRUE )
    {
      rxcbcch_info.first_frame = TRUE;
      /* This is the first frame of the slot, after a WAKEUP. */
      /* This flag is unused at the moment */

      rxc_slot.skipping = FALSE;
      /* Don't skip frames anymore. */
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      if (rxc_slot.slot_mask & BC_DATA_SLOT)
      {
        /* some time SRCH could wake 40ms early, before the BS send
         * the first BC BCCH frame, those case, SRCH will tell RXC to skip
         * that frame
         */
        MSG_HIGH(" Skip fr of BC BCCH ",0,0,0);
        return;
      }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
    }
  }
  if( rxc_slot.skipping == TRUE )
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

      rxcbcch_info.first_frame = FALSE;
        /* This is definitely not the first frame of the slot after a WAKEUP. */

      srch_mux_page_match( FALSE );
        /* Definitely no page matching happening at this point */
    }

    MSG_LOW("  Decint: fr=%lx, slt=%d",  qw_lo(rxc_dec_frame),
       ( (word)(qw_lo( rxc_dec_frame ) >> 2) & rxc_slot.sci_mask ), 0 );

    /* add in protection against skipping too many frame due to SC error */
    MSG_HIGH( "Skping fr after wakeup", 0, 0, 0 );
    return;
  }

  /* Logging needs to keep track of frame quality history, so a buffer 
     should be sent regardless of CRC success.  If CRC fails, nothing
     is stored. */
  buf_ptr = (rxc_dec_type *) q_get (&rxc_dec_free_q);

  if ( rxcbcch_info.first_frame == TRUE )
  {
    srch_mux_page_match( FALSE );
    rxcbcch_info.first_frame = FALSE;
  }

  /* Return value "success" ignored since pointer passed in is not NULL */
  (void) dec_read_bcch_status (&dec_status);

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

        /* Return value always TRUE */
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

      if ( buf_ptr )
      {
        /* Indicate to the task level frame processor that this was a bad
        frame. This distinction is used for frame quality history in
        logging. */
        buf_ptr->dec_rate = CAI_NULL_RATE;

#ifdef FEATURE_PLT
#error code not present
#endif
      }
    }
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
    /*decrease the BC BCCH maximum message duration by 1 */
    if((rxc_slot.slot_mask & BC_DATA_SLOT)  && (rxc_slot.bcsms_slot_frames > 0) )
    {
      rxc_slot.bcsms_slot_frames--;
    }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */


    if ( buf_ptr )
    {
      buf_ptr->state = RXC_BCCH_STATE;

      q_put (&rxc_dec_q, &buf_ptr->link);
      (void) rex_set_sigs (&rx_tcb, RXC_INT_SIG);
    }
    else
    {
      ERR( "0 bufs on dec_free_q", 0, 0, 0 );
    }
  
#ifdef FEATURE_PLT
#error code not present
#endif
  }


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_PARSER_INIT

DESCRIPTION
  This routine initializes the BCCH frame processor.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxcbcch_frame_proc_init (void) 
{  
  /* Ref: 3GPP2 C.S0003-A-1 (MAC) section 2.2.1.1.2.1.3 */
  rxcbcch_sar_frame_desc.subframe_size = 93;
  rxcbcch_sar_frame_desc.sci_size = 1;
  rxcbcch_sar_frame_desc.total_subframe = 0;

  /* Remove the 3 byte decoder tail (2 for CRC, 1 for tail) to get 
     frame data size */
  rxcbcch_sar_frame_desc.frame_size = (DEC_BCCH_SIZ-3) * 8;

  rxcsar_init_msg_descriptor (&rxcbcch_sar_msg_desc);
  return TRUE;
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_PROCESS_FRAMES

DESCRIPTION
  This procedure processes decoder frames from rxc_dec_q and converts them
  to BCCH frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxcbcch_process_frames(rxc_dec_type *buf_ptr)
{
  boolean good_frame = FALSE;
  boolean process_frame;

#ifdef FEATURE_PLT
#error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      rxcbcch_sar_frame_desc.frame_num = &buf_ptr->frame; //lint !e545 Suspicious use of &, due to qword *frame_num
      rxcbcch_sar_frame_desc.data = buf_ptr->buf.buf;

      if (buf_ptr->dec_rate == CAI_FULL_RATE) 
      {
        process_frame = TRUE;
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
        if (rxc_slot.slot_mask & BC_DATA_SLOT)
        {
          if((rxc_bcbcch_first_frame) && (!rxc_bcbcch_peek_sms_msadress_done))
          {
            /* this is the logic which peeks into the first frame to see if
            the address of the BCCH SMS is indeed the one we are looking for.
            if not then we could reset the rxc_slot.bcsms_slot_frames to zero, this
            will trigger BCCH send the RXC_SLOT_R to MC right away */
            if (rxc_bc_bcch_peek_ms_address(buf_ptr->buf.buf))
            {
              MSG_HIGH("Got matched MS adr on 1st BC fr",0,0,0);
            }
            else
            {
              /* no matched MS address on the BC frame, then stop
              monitor the BC BCCH, sending SLOT_R to MC */
              MSG_HIGH("BC adr msg dsn't match,Tx SLOT_R to MC",0,0,0);
              rxc_slot.bcsms_slot_frames = 0;
              process_frame = FALSE;
            }
            rxc_bcbcch_peek_sms_msadress_done = TRUE;
          }
        }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
        if (process_frame )
        {
          if (!rxcsar_process_fcsch_frame (&rxcbcch_sar_frame_desc,
            &rxcbcch_sar_msg_desc))
          {
            MSG_ERROR("FCCCH Fr Proc Err",0,0,0);
          }
          good_frame = TRUE;
        }
        /* take care the BCCH statistic counters, the counters only applies for primary BCCH */
        if ((rxc_slot.slot_mask & BC_DATA_SLOT) == FALSE)
        {
           switch (rxcbcch_info.data_rate)
           {
             case CAI_BCCH_19200_RATE:
               /* 40 ms frames */
               (void) parm_inc(CAI_BCCH_6_ID, 1);
               break;
             case CAI_BCCH_9600_RATE:
               /* 80 ms frames */
               (void) parm_inc(CAI_BCCH_7_ID, 1);
               break;
             case CAI_BCCH_4800_RATE:
               /* 160 ms frames */
               (void) parm_inc(CAI_BCCH_8_ID, 1);
               break;

             default:
               MSG_HIGH("Bad Pri BCCH data rate ",0,0,0);
               break;
           }
        }
      }
      else
      {
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
        /* Make note of the bad frame for NCCCH failsafe timer */
        rxcnccch_failsafe_timer(RXCNCCCH_NOTIFY_BCCH_BAD);
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

        if ((rxc_slot.slot_mask & BC_DATA_SLOT) == FALSE)
        {
          /* Primary BCCH ERASURE frame  */
          (void) parm_inc(CAI_BCCH_9_ID, 1);
        }
      }

      /* Log frame info. */
      rxcbcch_log_frame_info (good_frame);

#ifdef FEATURE_PLT
#error code not present
#endif
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_MSG_ALLOC

DESCRIPTION
  This procedure allocates a buffer for a message to be written into.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void * 
rxcbcch_msg_alloc (
  void *client_data,
  unsigned int length
)
{
  void *data = NULL;

  rxtx_rx_msg_type *msg_buffer = NULL;

  /* Unused parameter */
  (void) client_data;

  if (length <= sizeof (msg_buffer->msg.bcch.body)) 
  {
    
    msg_buffer = (rxtx_rx_msg_type *) q_get (&rxtx_rx_free_q);

    if (msg_buffer) {
      data = msg_buffer->msg.bcch.body;
    }
  }

  return data;
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_MSG_DELIVER

DESCRIPTION
  This procedure processes decoder frames from rxc_dec_q and converts them
  to BCCH frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void 
rxcbcch_msg_deliver (
  void *client_data,
  unsigned int length,
  void *data, /* Same pointer as what was returned from rxcsar_msg_alloc() */
  qword *frame_num
)
{
  rxtx_rx_msg_type *msg_buffer;

  /* Unused parameter */
  (void) client_data;

  if (data) 
  {
    /* Upon receiving a valid message, set the fade timer */
    rxcidle_set_fade_timer (RXCIDLE_CURR_FADE);

#ifdef FEATURE_IS2000_REL_A_CC_SMMC
    /* If we get here, let the failsafe timer know BCCH channel is good */
    rxcnccch_failsafe_timer(RXCNCCCH_NOTIFY_BCCH_GOOD);
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

    /* Assume that RXCSAR did not alter the the value of data, 
       as it is documented as such. */
    msg_buffer = (rxtx_rx_msg_type *) (((byte *) data) - 
      FPOS (rxtx_rx_msg_type, msg.bcch.body)); //lint !e413 FPOS

    rxcbcch_msg_crc_stats.good_crcs++;
    MSG_HIGH("BCCH MSG CRC ok, total %d", rxcbcch_msg_crc_stats.good_crcs, 0, 0);

    /* Increase the total number of message */
    (void) parm_inc(CAI_BCCH_1_ID, 1);
    /* Increase the total number of subframes */
    (void) parm_inc(CAI_BCCH_3_ID, (uint8) rxcbcch_sar_frame_desc.total_subframe);
    rxcbcch_sar_frame_desc.total_subframe = 0;

    if (frame_num) {
      qw_equ (msg_buffer->msg.bcch.frame_num, *frame_num);
    } else {
      qw_set (msg_buffer->msg.bcch.frame_num, 0, 0);
    }
  
    /* Fill in the rest of the structure */
    msg_buffer->chn = RXC_CDMA_BCCH;
    msg_buffer->length = length;
    msg_buffer->msg.bcch.length = (word) length;
    /*lint -e545 */
    if (rxcmsg_deliver (CAIX_BCCH, &msg_buffer->msg, length,
                        &msg_buffer->msg.bcch.frame_num, FALSE))
    {
      /*bnsms*/
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      if (rxc_slot.slot_mask & BC_DATA_SLOT)
      {
        rxc_slot.valid_msg = TRUE;
      }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
      MSG_HIGH("MUX sends BCCH msg to MC",0,0,0);
    }

    // Send event
    mclog_report_event_msg_rxed(CAI_BCCH, msg_buffer->msg.bcch.body);
    
    /* We are now done with this message buffer.  Discard. */
    q_put (&rxtx_rx_free_q, &msg_buffer->link);
    msg_buffer = NULL;

#ifdef FEATURE_PLT
#error code not present
#endif

  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_MSG_DISCARD

DESCRIPTION
  This procedure processes decoder frames from rxc_dec_q and converts them
  to BCCH frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void 
rxcbcch_msg_discard (
  void *client_data,
  void *data
)
{

  rxtx_rx_msg_type *msg_buffer;

  /* Unused parameter */
  (void) client_data;

  if (data) {
    /* Assume that RXCSAR did not alter the the value of data, 
       as it is documented as such. */
    msg_buffer = (rxtx_rx_msg_type *) (((byte *) data) - 
      FPOS (rxtx_rx_msg_type, msg.bcch.body)); //lint !e413 FPOS

    /* We are now done with this message buffer.  Discard. */
    q_put (&rxtx_rx_free_q, &msg_buffer->link);
    msg_buffer = NULL;
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_MSG_EVENT

DESCRIPTION
  This procedure is called whenever a noteworthy event happens during the 
  course of frame processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void
rxcbcch_msg_event (
  void *client_data,
  rxcsar_msg_event_type event
)
{
  /* Unused parameter */
  (void) client_data;

  if (event == RXCSAR_CRC_FAIL_S)
  {
    rxcbcch_msg_crc_stats.bad_crcs++;

#ifdef FEATURE_IS2000_REL_A_CC_SMMC
    /* Make note of the bad msg for NCCCH failsafe timer */
    rxcnccch_failsafe_timer(RXCNCCCH_NOTIFY_BCCH_BAD);
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

#ifndef FEATURE_PLT
    MSG_HIGH("BCCH MSG CRC bad, %d", rxcbcch_msg_crc_stats.bad_crcs, 0, 0);
#endif
    /* Increase the total number of message */
    (void) parm_inc(CAI_BCCH_1_ID, 1);
    /* Increase the total number of message that failed CRC */
    (void) parm_inc(CAI_BCCH_2_ID, 1);
    /* Increase the total number of subframes */
    (void) parm_inc(CAI_BCCH_3_ID, (uint8) rxcbcch_sar_frame_desc.total_subframe);
    /* Increase the total number of subframes which have message CRC not matched*/
    (void) parm_inc(CAI_BCCH_4_ID, (uint8) rxcbcch_sar_frame_desc.total_subframe);
    rxcbcch_sar_frame_desc.total_subframe = 0;
  
#ifdef FEATURE_PLT
#error code not present
#endif
  } else 
  {
    /* Unknown event, discard */
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_FRAME_INFO_LOG_INIT

DESCRIPTION
  This procedure initializes the accumulation buffer for frame info logging.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void 
rxcbcch_frame_info_log_init (void)
{
  rxcbcch_log_info.log.num_frames = 0;

  log_set_code (&rxcbcch_log_info.log, LOG_BCCH_FRAME_INFO_C);
  
  rxcbcch_log_info.log.data_rate = rxcbcch_info.data_rate;
  rxcbcch_log_info.log.code_rate = rxcbcch_info.code_rate;
  rxcbcch_log_info.log.code_chan = rxcbcch_info.code_chan;
  rxcbcch_log_info.log.num_frames = 0;
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_LOG_FRAME_INFO

DESCRIPTION
  This procedure accumulates and logs frame info for BCCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static const byte rxc_bcch_good_frame_ind_mask[RXCBCCH_NUM_LOG_FRAME_PER_BYTE] 
                        = {0xc0,0x30,0x0c,0x03};
static const byte rxc_bcch_bad_frame_ind_mask[RXCBCCH_NUM_LOG_FRAME_PER_BYTE] 
                        = {0x80,0x20,0x08,0x02};
static void 
rxcbcch_log_frame_info (boolean frame_quality_ind)
{
  unsigned int idx = 0;
  byte mask_index = 0;

  /* No need to accumulate stats unless it is enabled. */
  if (log_status (LOG_BCCH_FRAME_INFO_C)) {
    if (rxcbcch_log_info.log.num_frames == 0) {
      /* Timestamp indicates the first frame in the history */
      log_set_timestamp (&rxcbcch_log_info.log);
    }

    /* Calculate the array index and bit mask for the log history 
     * field before incrementing num_frames. Since we need 2 bit per frame
     * and 8 bits per byte, so we only increase the index for every 4 frames
     */
    idx = (rxcbcch_log_info.log.num_frames) / RXCBCCH_NUM_LOG_FRAME_PER_BYTE;
    mask_index = rxcbcch_log_info.log.num_frames%RXCBCCH_NUM_LOG_FRAME_PER_BYTE;
    if(mask_index == 0)
    {
      /* start of the byte, reset all the bits to zero */
      rxcbcch_log_info.log.frame_quality[idx] = 0;
    }
    /* Now increment the num_frames count */
    rxcbcch_log_info.log.num_frames++;

    if (rxcbcch_log_info.log.num_frames <= RXCBCCH_FRAME_LOG_HISTORY_SIZE) {

      /* '11' if good frame, '10' if not */
      if (frame_quality_ind) 
      {
        rxcbcch_log_info.log.frame_quality[idx] |= 
               rxc_bcch_good_frame_ind_mask[mask_index];
      }
      else
      {
        rxcbcch_log_info.log.frame_quality[idx] |= rxc_bcch_bad_frame_ind_mask[mask_index];        
      }

      if (rxcbcch_log_info.log.num_frames >= (RXCBCCH_FRAME_LOG_HISTORY_SIZE - RXCBCCH_NUM_LOG_FRAME_PER_BYTE)) 
      {
        rxcbcch_frame_info_log_send ();
      }
    } else {
      /* Oh well - we tried.  Reset the buffer and consider that one dropped */
      rxcbcch_frame_info_log_init ();
    }
  } else if (rxcbcch_log_info.log.num_frames > 0) {
    /* To save CPU, only init if not initialized. */
    rxcbcch_frame_info_log_init ();
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_FRAME_INFO_LOG_SEND

DESCRIPTION
  This procedure send the accumulated log buffer for frame info logging.

DEPENDENCIES
  rxcbcch_frame_info_log_init() must have been called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxcbcch_frame_info_log_send (void)
{
  log_set_length (&rxcbcch_log_info.log, FPOS (mux_bcch_frame_info_type,
    frame_quality) + (rxcbcch_log_info.log.num_frames + 
             RXCBCCH_NUM_LOG_FRAME_PER_BYTE -1) / RXCBCCH_NUM_LOG_FRAME_PER_BYTE);

  if (rxcbcch_log_info.log.num_frames > 0 &&
      log_submit (&rxcbcch_log_info.log)) {
    /* Successfully sent a log, re-initialize */
    rxcbcch_frame_info_log_init ();
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_RESET_STATS

DESCRIPTION
  This funtion resets statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void
rxcbcch_reset_stats (void)
{
  rxcbcch_frame_crc_stats.good_crcs = 0;
  rxcbcch_frame_crc_stats.bad_crcs = 0;

  rxcbcch_msg_crc_stats.good_crcs = 0;
  rxcbcch_msg_crc_stats.bad_crcs = 0;

#ifdef FEATURE_PLT
#error code not present
#endif
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCBCCH_EXIT

DESCRIPTION
  This funtion performs processing necessary to end the Paging channel state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxcbcch_exit (void)
{
  MSG_HIGH("Dis BCCH", 0, 0 ,0);

  /* Free action timer resource. */
  atmr_set (rxcbcch_info.atmr_handle, NULL, ATMR_5MS_FRAME(NONE), 
    (atmr_pcg_offset_enum_type) 0, (unsigned long) 0);

  rxcsar_terminate (&rxcbcch_sar_msg_desc, 0);
  rxcbcch_frame_info_log_send ();

  /* Only BCCH uses sequence repetition factors of 2 and 4 for 9600 & 4800bps
   * Set the sequence repetition factor back to 1 (BCCH data rate 19200) for
   * other channels
   */
  deint_bcch_seq_repetition(CAI_BCCH_19200_RATE,rxcbcch_chan_id); /*lint -e571 */

  if (!deint_config_20ms_frame_boundary (rxcbcch_chan_id, TRUE))
  {
    ERR_FATAL("Bad ch id %d",rxcbcch_chan_id,0,0);
  }

  /* reset the sequence repetition */
  program_zero_seq_rep_done = TRUE;
  program_seq_rep_done = TRUE;

  rxc_read_bcch_data_from_decoder = FALSE;
  rxc_nccch_deint_ram_mapping_done = FALSE;

  /* reset the bcch info structure */
  rxcbcch_info.code_rate = CAI_CODE_RATE_INVALID;
  rxcbcch_info.data_rate = CAI_BCCH_RATE_INVALID;
  rxcbcch_info.code_chan = 0xFF;
  rxcbcch_info.first_frame = FALSE;

} /* rxcbcch_exit */

/*===========================================================================

FUNCTION RXCBCCH_ACTIVATE

DESCRIPTION
  Activates BCCH during slotted mode operation.

DEPENDENCIES
  Must be in RXC_BCCH_STATE.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
rxcbcch_activate (void)
{
  INTLOCK ();
  tramp_set_isr (TRAMP_RX_ISR, rxcbcch_isr);
  /* Clear DEC DONE int status to make sure ISR is properly serviced.
  */
  HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
  INTFREE ();
  rxcbcch_info.first_frame = TRUE;
  rxcidle_set_slotted_status( RXCIDLE_SLOTTED_AWAKE );

}

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
/*===========================================================================

FUNCTION RXC_BC_BCCH_PEEK_MS_ADDRESS

DESCRIPTION

DEPENDENCIES
  After receive the first BC BCCH frame, this function will call to peek
  into the BC frame to see if this frame is indeed belong to the MS
  It also checks if there is an extended BCBCCH message( all the messages that
  require more 3 BCBCCH slots to send, then it will re-initialize the total
  number of slots needs to monitor 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxc_bc_bcch_peek_ms_address(byte *ptr)
{
  uint16 bcmsg_totalbyte;
  uint16 bcbcch_frame;
  /* got the first BC BCCH, extract the address info so that we could peek into the
   * the address to know whethere this Broadcast page is intended for the MS
   */
  bcmsg_totalbyte = mcc_bc_match(ptr);
  if ( bcmsg_totalbyte > 0)
  {
    /* we could look at the total number of bytes to 
    figure out that if we have a extended BC BCCH message, 
    then we will set rxc_slot.bcsms_slot_frames to the correct
    slot number so that the MS will keep monitor the secondary BCCH
    until the whole BC BCCH message receives 
    */
    if ((bcmsg_totalbyte % BC_BCCH_BYTE_PER_FRAME) == 0)
    {
      bcbcch_frame = (bcmsg_totalbyte/BC_BCCH_BYTE_PER_FRAME);
    }
    else
    {
      bcbcch_frame = (bcmsg_totalbyte/BC_BCCH_BYTE_PER_FRAME) + 1;
    }
    if (bcbcch_frame > MAX_NUM_FRAME_PER_BCBCCH_MSG)
    {
      /* got extended bcbcch message, re-initialize the number of slots needs to monitor */
      rxc_slot.bcsms_slot_frames =  (bcbcch_frame - 1); /* we already got the first frame */
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

#endif /* FEATURE_IS2000_REL_A_CC */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           CDMA2000 Forward Common Control Channel Monitoring

GENERAL DESCRIPTION
  Contains implementation of Physical, LAC and MAC layer of IS2000 release
A Forward Common Control Channel (FCCCH) operation.

  
EXTERNALIZED FUNCTIONS
  rxcfccch_init - initializes the FCCCH processing state
  rxcfccch_state - FCCCH state processing
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcfccch.c_v   1.35   23 Oct 2002 16:27:04   baon  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcfccch.c#5 $ $DateTime: 2008/09/09 07:26:45 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/08   jtm     Increased rxc_fccch_crc_temp_array size to RXCSAR_MAX_EXPECTED_MSG_LENGTH
                   to prevent overflow when copying the message to the CRC buffer. 
08/15/08   jtm     Added debug messages to cmd processing states.
07/24/08   jtm     Klockwork correction.
07/16/08   jtm     Removed FEATURE_RXCCHAIN_DEBUG. Reduced debug string sizes
                   for ULC effort.
09/26/07   bn/vlc  Fixed high FER issue on BCCH in fading condition.
05/29/07   vlc     Fixed compiler warnings for rel 0 only build.
05/03/07   cab     Moved to new sleep voting interface
06/12/06   awj     #include tramp.h; no longer included through clk.h
06/09/06   trc     Remove T_MSM6700 featurization
05/23/06   trc     Add NCCCH failsafe timer support
05/08/06   bn      Added logic to support the case if receiving slot_mask = 0 in rude wakeup
03/22/06   kwo     Added calls into srch to report frame stats
01/18/06   bn      Enable the Q channel in rxcfccch_config_demod().
11/22/05   bn      Since the SAR doesn't handle the parsing of the failed CRC well,
                   so for now, only parsing passed CRC frames.
09/08/05   ejv     Use new srch_mux interface calls.
09/07/05   rkc     Mainline FEATURE_ZZ2.
08/29/05   bn      Clear the all the decoder status it after installing the isr
08/11/05   bn      Added logic to relay the page match event to MC. Clear up the code
07/25/05   rkc     Fixed lint errors.
07/05/05   bn      Fixed the page match logic in MUX
05/04/05   kwo     srch_idle_page_match() -> srch_mux_page_match()
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
03/14/05   vlc     Fixed compiler warnings for MSM6700.
03/14/05   bn      Instead of SRCH, MUX need to install the ISR after WAKE_F
03/14/05   vlc/    Merged in changes for MSM7500.  (dlb')
02/24/05   vlc     Lock interrupt between clearing DEC_DONE_INT_STATUS and
                   installing RXC ISR.
02/23/05   vlc     Clear the DEC_DONE_INT_STATUS when setting RXC ISR.
12/14/04   ljl     Added event report for F-CCCH.
09/28/04   sr      Merged in changes from 6550 2009 build
08/09/04   ejv     #include muxmsm.h.
08/09/04   bn/kwo  Added new interfaces to expose chan mod setup parameters
08/09/04   sr      Added line to rxcfccch_config_demod() to clear PDCCH bit in 
                   DEM1X_CHANNEL2 register for MSM6700
07/23/04   jyw     Added the interrupt cleaning to rxcfccch_isr()
07/15/04   bn      Re-Initialize the FCCCH info structure when exit
06/16/04   sr      Merged in changes from 6500 5004 build
05/05/04   bn      Fixed the mismatch in the logging of FCCCH data rate
04/20/04   jrp     Fixed lint warnings.
12/11/03   bn      No need to check for FCCCH frame CRC. Let the msg CRC do the job
11/17/03   fc      Added quick page match.
09/16/03   ejv     Updated rxcfccch_config_demod() for 6500.
09/09/03   dna     Merged porting changes for MSM6500
08/11/03   bn      Fixed the logic to trigger sending SLOT_R to MC
06/30/03   bn      Fixed potential problem in zero-out the FCCCH log info buffer
06/16/03   bn      Fixed the "out of buffer dec_free_q" problem. Also fixed the
                   hard-handoff problem when switching from NCCCH to PCH
05/14/03   bn      Support frame crc statistic for simultanous FCCCH/BCCH in PLT
05/05/03   bn      Optimized the NCCCH logics and F3 debug messages
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
03/05/03   sr      Added support for Sync Channel workaround.
02/25/03   va      Removed changing MC and SRCH priority by MC. 
02/04/03   bn      Corrected the logic to increase the MUX statistic counters
01/30/03   bt      Changed FAST_SLEEP_CMD for FEATURE_ZZ2.
01/09/03   bt      Added support for FEATURE_ZZ2 and fix ts warning.
12/12/02   bn      Added logic to handle SLOT_F cmd when transition in 2sd-BCCH
10/21/02   bn      Added support for NCC statistic counters
11/20/02   gs      Added support for FEATURE_ZZ2
10/03/02   bn      Added support for Broadcast BCCH
10/22/02   bn      Correct the error in FCCCH frame quality logging
10/14/02   sr      Added MSM6100 changes
10/11/02   sr      Reset MC task priority back to MC_PRI in ACC_FADE_F
                   Set RXC_INT_SIG unconditionally in rxcfccch_isr()
08/06/02   cr      Added MSM6100 changes.
08/14/02   sr      Added protection against skipping frames forever after
                   a slot boundary due to bad symbol combiner offset.
08/13/02   sr      Added Fast RAHO support
06/25/02   lad     Fixed logic error in frame info logging.
06/03/02   sr      Changed CAIX_FCCCH to RXC_CDMA_FCCCH
05/31/02   sr      Added RXC_SLOT_R support and no longer blindly skip
                   processing of first frame
04/22/02   lad     Added flush count to const frame info for Idle HO.
04/05/02   lad     Modified SAR interface to use const info in msg_desc.
04/01/02   lad     Added frame info logging, message logging, first frame
                   omission to avoid bad CRC.
03/07/02   lad     Updated/added crc stat messages for PLT.
03/27/02   bn      Corrected the mistypo of rxcfccch_last_msg_crc_stats
03/07/02   lad     Updated/added crc stat messages for PLT.
02/27/02   lad     Updated API to take cai_fccch_data_rate_type and 
                   cai_fccch_frame_size_type instead of cai_fccch_rate_type.
                   Updated decoder driver usage.
02/25/02   lad     Added SRCH call to inform fast sleep.
02/15/02   lad     Moved demod settings into rxcfccch_config_demod().
                   Added rxcidle_pending_state() check.
                   Moved RXC_CHAN_CONFIG_F handling to rxcidle.c.
                   Added initial support for SLEEP and WAKE (no slot support).
02/05/02   lad     Added PCH and BCCH to RXC_CHAN_CONFIG_F.
                   Removed flush count from rxcsar_terminate() call.
                   Removed time math from rxcfccch_init() (copied from rxcpaging)
01/17/02   lad     Changed rxcfccch_init() to take rxc_fccch_info_type.
                   Updated rxcsar usage.  Update sleep API usage.
                   Added RXC_CHAN_CONFIG_F and usage of rxcidle, rxcmsg.
                   Moved rxcfccch_isr() prototype from rxcfccch.h.
11/20/01   sr      Added new rxcfccch_init() and rxcfccch_isr() routines under
                   FEATURE_RXCCHAIN_DEBUG
10/22/01   lad     Moved call to rxcchain_deactivate to ISR cmd handler.
10/17/01   lad     Added dummy state and state transition to TCI state.
10/05/01   lad     Created file.

===========================================================================*/

#include "customer.h"
#include "target.h"
#include "comdef.h"

#ifdef FEATURE_IS2000_REL_A_CC

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
#include "muxmsm.h"


#include "rxccommon.h"
#include "rxcfccch.h"
#include "rxcidle.h"
#include "rxcmsg.h"
#include "rxcsar.h"
#include "rxcsync.h"
#include "rxctraffic.h"

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
boolean raho_flag = FALSE;
extern uint16 parm_inc ( uint16 parm_id, uint8  inc); 


/* Functions called by RXCSAR. */
static void * rxcfccch_msg_alloc (void *client_data, unsigned int length);
static void rxcfccch_msg_deliver (void *client_data, unsigned int length, 
                                  void *data, qword *frame_num);
static void rxcfccch_msg_discard (void *client_data, void *data);
static void rxcfccch_msg_event (void *client_data, rxcsar_msg_event_type event);

static const rxcsar_msg_const_desc_type rxcfccch_msg_const_desc = {
  LOG_FCCCH_SIGNALING_C,
  TRUE,
  6,
  rxcfccch_msg_alloc,
  rxcfccch_msg_deliver,
  rxcfccch_msg_discard,
  rxcfccch_msg_event
};

/* SAR layer variables */
static byte rxc_fccch_crc_temp_array[RXCSAR_MAX_EXPECTED_MSG_LENGTH];
static rxcsar_fcsch_frame_desc_type rxcfccch_sar_frame_desc;
rxcsar_msg_desc_type rxcfccch_sar_msg_desc = {
  NULL, &rxcfccch_msg_const_desc, rxc_fccch_crc_temp_array,0,0,0
};

/* Table of frame and subframe sizes (in bits), indexed by 
 * dec_ccch_data_rate_type.
 * Ref: 3GPP2 C.S0003-A-1 (MAC) section 2.2.1.1.2.1.2.
 * Flush count is used during idle handoff to flush a 80ms worth of data from
 * the software.  This is supposed to flush all frames in the decoder queue
 * and assure the seq numbers of the signalling messages get out of sync.
 * 80ms is used since the paging channel would flush 8 half frames (4 * 20ms
 * frames).  NOTE: Currently, we only support 20ms frames */

const rxcfccch_frame_info_tbl_type rxcfccch_frame_info_tbl[] = 
{
  {172, 86, 4 * 2},
  {360, 90, 4 * 4},
  {744, 93, 4 * 8}
};


/* Logging Info */
#define RXCFCCCH_NUM_LOG_FRAME_PER_BYTE   (4)
#define RXCFCCCH_FRAME_LOG_HISTORY_SIZE (32*RXCFCCCH_NUM_LOG_FRAME_PER_BYTE)
static union {
  mux_fccch_frame_info_type log;
  
  /*lint -e754 -save buf is not referenced but is needed to assure log buffer is large enough */
  /* This member assures the log buffer is large enough */
  byte buf[FPOS (mux_fccch_frame_info_type, frame_quality) + 
    (RXCFCCCH_FRAME_LOG_HISTORY_SIZE + RXCFCCCH_NUM_LOG_FRAME_PER_BYTE - 1) /RXCFCCCH_NUM_LOG_FRAME_PER_BYTE]; //lint !e30 FPOS
  /*lint -restore */
} rxcfccch_log_info;

/* Frame statistics */
/* # of FCCCH frames with good CRCs */
rxc_crc_cnt_type rxcfccch_frame_crc_stats;
static rxc_crc_cnt_type rxcfccch_msg_crc_stats;
#ifdef FEATURE_PLT
#error code not present
#endif



rxcfccch_info_type rxcfccch_info;
static dec_ccch_data_rate_type rxcfccch_data_rate;

extern void rxc_set_mac_rnd_ccch
(
  deint_code_rate_type code_rate,
  dec_ccch_data_rate_type data_rate,
  deint_ch_id_type chan_id
);


/* <EJECT> */
/*--------------------------------------------------------------------------
 * Forward Declarations
 *--------------------------------------------------------------------------*/

static void rxcfccch_config_demod
(
  deint_code_rate_type code_rate,
  dec_ccch_data_rate_type data_rate,
  deint_ch_id_type chan_id
);

static void rxcfccch_isr (void);


static rxc_state_type rxcfccch_process_cmd (void);
static void rxcfccch_frame_info_log_init (void);
static void rxcfccch_log_frame_info (boolean frame_quality_ind);
static void rxcfccch_reset_stats (void);

void rxcfccch_exit_clean_up (void);

static deint_ch_id_type  rxcfccch_chan_id = DEINT_CH2;

#ifdef FEATURE_IS2000_REL_A_CC_QPM
word rxcfccch_last_sec = 0;
#endif /* FEATURE_IS2000_REL_A_CC_QPM */
extern boolean rxc_nccch_deint_ram_mapping_done;

const rxc_chan_demod_config_param_type  
rxc_fccch_demod_config_param_tab[CAI_CODE_RATE_COUNT][CAI_FCCCH_DATA_RATE_INVALID] =
{
  /*cr 1/4, dr 9600 => sr 64 chips, mac rnd = 3, repeat fac = 1*/ 
  {{SPR_64_CHIP_V,3,DMOD_REP_FACTOR_1},
  /*cr 1/4, dr 19200 => sr 32 chips, mac rnd = 2, repeat fac = 1*/ 
  {SPR_32_CHIP_V,2,DMOD_REP_FACTOR_1}, 
  /*cr 1/4, dr 38400 => sr 16 chips, mac rnd = 2, repeat fac = 1*/ 
  {SPR_16_CHIP_V,2,DMOD_REP_FACTOR_1}},
  /*cr 1/2, dr 9600 => sr 128 chips, mac rnd = 3, repeat fac = 1*/ 
  {{SPR_128_CHIP_V,3,DMOD_REP_FACTOR_1},
  /*cr 1/2, dr 19200 => sr 64 chips, mac rnd = 3, repeat fac = 1*/ 
  {SPR_64_CHIP_V,3,DMOD_REP_FACTOR_1}, 
  /*cr 1/2, dr 38400 => sr 32 chips, mac rnd = 2, repeat fac = 1*/ 
  {SPR_32_CHIP_V,2,DMOD_REP_FACTOR_1}}
};

/* <EJECT> */
/*--------------------------------------------------------------------------
 * Public function implementations
 *--------------------------------------------------------------------------*/

#ifdef FEATURE_IS2000_REL_A_CC_SMMC
boolean  rxc_read_fccch_data_from_decoder = FALSE;
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

boolean rxcfccch_increase_page_matched_cnt = FALSE;

boolean
rxcfccch_init
(
  rxc_state_type curr_state,     /* Current RXC state machine state */
  rxc_fccch_info_type *config_ptr /* Configuration pointer */
)
{
  boolean success = FALSE;

  /* Unused Parameter */
  (void) curr_state;

  if (config_ptr) 
  {
    success = TRUE;

    //TODO: This case should be a table lookup.
    switch (config_ptr->rate)
    {
      case CAI_FCCCH_9600_RATE:
        {
          rxcfccch_data_rate = DEC_CCCH_FULL_RATE;
        }
        break;
        
      case CAI_FCCCH_19200_RATE:
        {
          rxcfccch_data_rate = DEC_CCCH_2X_RATE;
        }
        break;
  
      case CAI_FCCCH_38400_RATE:
        {
          rxcfccch_data_rate = DEC_CCCH_4X_RATE;
        }
        break;
  
      default:
        {
          success = FALSE;
        }
        break;
    }
    /* For now, we only support 20ms FCCCH frames */
    if (config_ptr->frame_size != CAI_FCCCH_20MS_FRAME)
    {
      success = FALSE;
    }
    if (success) 
    {
      /* if FCCCH is already up, check to see of the config is changing, then
      we need to re-program it again */
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
      if(rxcnccch_chan_enable_bits & RXCNCCCH_FCCCH_BIT)
      {
        if ((rxcfccch_data_rate == rxcfccch_info.data_rate) &&
            (config_ptr->frame_size == rxcfccch_info.frame_size) &&
            (((deint_code_rate_type) config_ptr->crate) == rxcfccch_info.code_rate) )
        {
          MSG_HIGH("FCCCH alrdy up, same config, rtrn",0,0,0);
          return(TRUE);
        }
        else
        {
          MSG_HIGH("FCCCH is alrdy up, new config, re-init",0,0,0);
        }
      }
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
      /* check of the deint ram has been mapped for NCCCH, if not
      then go ahead and map it. It will map for both channel 1 and 2 */
      if (!rxc_nccch_deint_ram_mapping_done)
      {
        deint_allocate_ram_nccch();
        rxc_nccch_deint_ram_mapping_done = TRUE;
      }

      /* call the SRCH function to get the channel number, but for now
      use the default channel # 2
      */
      rxcfccch_chan_id = DEINT_CH2;

      rxcfccch_info.data_rate = rxcfccch_data_rate;
      rxcfccch_info.code_rate = (deint_code_rate_type) config_ptr->crate;
      MSG_MED("FCCCH drate=%d, crate=%d", rxcfccch_info.data_rate, rxcfccch_info.code_rate, 0);
    
      rxcfccch_info.code_chan = config_ptr->code_chan; /* needed for logging */
      rxcfccch_info.frame_size = config_ptr->frame_size;

      rxcfccch_config_demod (rxcfccch_info.code_rate, rxcfccch_info.data_rate,rxcfccch_chan_id);
    
      /* Decoder clk is TCXO in BCCH, but do it again JIC */
      dec_clk_select (DEC_CLK_TCXO);

      dec_ccch_init ();
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
      deint_set_ccch_mode (rxcfccch_info.code_rate, rxcfccch_info.data_rate,
                           rxcfccch_chan_id,rxcnccch_chan_enable_bits );
#else
      deint_set_ccch_mode (rxcfccch_info.code_rate, rxcfccch_info.data_rate,rxcfccch_chan_id,0);
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */


      rxcfccch_info.first_frame = FALSE;
      /* Make sure page matching is completely disabled */
      rxc_slot.page_match = FALSE;
      
      rxcfccch_reset_stats ();
      rxcfccch_frame_info_log_init ();

      /* Initialize the FCCCH frame processor */
      /* Always returns true. Ignoring return value */
      (void) rxcfccch_frame_proc_init (rxcfccch_info.data_rate);
    
      // Only install the ISR if we are not sleeping. This is to avoid
      // processing decoder interrupts right after search sends us the
      // CHAN_CONFIG_F command as a result of a fast RAHO, in which case
      // they will call rxc_idle_activate() at the slot boundary, which 
      // will install the ISR.
      if( rxcidle_get_slotted_status() != RXCIDLE_SLOTTED_ASLEEP )
      {
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
        rxcnccch_chan_enable_bits = (rxcnccch_chan_enable_bits | RXCNCCCH_FCCCH_BIT);
        rxc_read_fccch_data_from_decoder = TRUE;
#else
        INTLOCK ();
        tramp_set_isr (TRAMP_RX_ISR, rxcfccch_isr);
        /* Clear DEC DONE int status to make sure ISR is properly serviced.
        */
        HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
        INTFREE ();
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
      }
    }
  }
  return success;
}


/*=========================================================================*/
void
rxcfccch_activate (void)
{
  INTLOCK ();
  tramp_set_isr (TRAMP_RX_ISR, rxcfccch_isr);
  /* Clear DEC DONE int status to make sure ISR is properly serviced.
  */
  HWIO_OUT(DEC_DONE_INT_STATUS, 0x7);
  INTFREE ();

  rxcfccch_info.first_frame = TRUE;
  rxcidle_set_slotted_status( RXCIDLE_SLOTTED_AWAKE );
}


/* <EJECT> */
/*=========================================================================*/
rxc_state_type 
rxcfccch_state (void)
{
  rxc_state_type next_state = RXC_FCCCH_STATE;
  
  rex_sigs_type sigs;             
  rex_sigs_type sa_wait_mask;
  rxc_dec_type *buf_ptr;  /* pointer to decoder data buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - -*/


  MSG_LOW ("Start FCCCH state", 0, 0, 0);

  rxc_vote_not_ok_to_sleep();
  
    while ((next_state = rxcfccch_process_cmd ()) == RXC_FCCCH_STATE) {
      
      sa_wait_mask = RXC_CMD_Q_SIG | RXC_INT_SIG | rxcidle_wait_mask ();

      sigs = rxc_wait (sa_wait_mask);

      /* Check if we have any signals to process.  Afterward, complete loop
         by checking the RXC command queue. */
      if (rxcidle_process_sigs (sigs,RXC_FCCCH_STATE)) {
        if (sigs & RXC_INT_SIG) {
          /* do frame counting */
          rxcfccch_update_frame_counters();

          (void) rex_clr_sigs (&rx_tcb, RXC_INT_SIG);
           while ((buf_ptr = (rxc_dec_type *) q_get (&rxc_dec_q)) != NULL) 
           {
             /* If not FCCCH state frame, drop the frame */
             if (buf_ptr->state == RXC_FCCCH_STATE) 
             {
               rxcfccch_process_frames (buf_ptr);
             }
             /* Free up the buffer for the ISR */
             q_put (&rxc_dec_free_q, &buf_ptr->link);
           }
        }
      }
    }
 
  /* Perform exit processing */
  rxcfccch_exit ();

  return (next_state);
} /* rxcfccch_state */


/* <EJECT> */
/*--------------------------------------------------------------------------
 * Private function implementations
 *--------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION RXCFCCCH_CONFIG_DEMOD

DESCRIPTION
  This function configures the demodulator for FCCCH operation.  This is
  used for initilization and wakeup.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void
rxcfccch_config_demod 
(
  deint_code_rate_type code_rate,
  dec_ccch_data_rate_type data_rate,
  deint_ch_id_type chan_id
)
{
  /* FCCCH is always on channel 2. */
  MSM_LOCK();
  /* need to enable the Q channel, since we can enter here from synch/paging states
  and in those states, Q channel is disable */
  DEMOD_ENABLE_Q_CHANNEL();
  if (chan_id == DEINT_CH1)
  {
    /* Lint error due to 0xffffffff in macro interpretted as signed long. */
    DEMOD_ENABLE_CH1_HW_COMBINING(); //lint !e737 Loss of sign in promotion from int to unsigned long.
  }
  else
  {
    /* Lint error due to 0xffffffff in macro interpretted as signed long. */
    DEMOD_ENABLE_CH2_HW_COMBINING(); //lint !e737 Loss of sign in promotion from int to unsigned long.
    MSM_CLEAR( DEM1X_CHANNEL2_FRAME, PDCCH_1X_SEL );
  }
  rxc_set_mac_rnd_ccch (code_rate, data_rate, chan_id);
  MSM_UNLOCK();

}

#ifdef FEATURE_IS2000_REL_A_CC_QPM
/*===========================================================================

FUNCTION RXCFCCCH_EMPTY_GPM_AND_DONE

DESCRIPTION
  This function performs an empty page match routine on a General Page
  Message.  It is called from the FCCCH Decoder interrupt handler during
  slotted mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If it is an empty General Page Message that indicates no more page
        for this mobile in the current slot.
  FALSE: Otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean rxcfccch_empty_gpm_and_done(byte *bits)
{
  /* Perform empty General Page match  */
  if ( (bits[0] == 0x42 ) &&
       ( (bits[1] & 0x44) == 0x44 ) &&
       ( (bits[2] & 0x40) == 0x40 ) &&
           /* The above pattern match ensures that
              1. SCI (2 bits) is set to 1,
              2. EXT_MSG_LENGTH (1 bit) is set to 0.
              3. MSG_LEN (7 bits)_is 9-bytes for an empty General Page Message, and
              3. MSG_TYPE is CAI_GENERAL_PAGE_MSG, which is 0x11.
            */

       ( (bits[2] & 0x3F) == mcc_get_cur_msg_seq() ) &&
           /* current CFG_MSG_SEQ */

       ( (cdma.assigning_tmsi.info.tmsi_code == INVALID_TMSI_CODE) ||
         ( (bits[4] & 0x80) == 0x80 ) ) &&   
           /* TMSI not assigned or TMSI_DONE */

       (
          ((cdma.imsi_addr_num != IMSI_CLASS_0_ADDR_NUM) &&
           ((bits[3] & 0x01) == 0x01)) ||
          ((cdma.imsi_addr_num == IMSI_CLASS_0_ADDR_NUM) &&
           ((bits[3] & 0x02) == 0x02))
       )     
           /* (MS has a class 1 IMSI and CLASS_1_DONE) or
              (MS has a class 0 IMSI and CLASS_0_DONE)
            */
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
       && ( mcc_bc_sms_enable ? (bits[4] & 0x20) : 1 )
           /* Check for BROADCAST_DONE */
#endif /* FEATURE_IS2000_REL_A_CC_BSMS */
    )
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }

} /* rxcfccch_empty_gpm_and_done */

/*===========================================================================

FUNCTION RXCFCCCH_EMPTY_UPM_AND_DONE

DESCRIPTION
  This function performs an empty page match routine on an Universal Page
  Message.  It is called from the FCCCH Decoder interrupt handler during
  slotted mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If it is an empty Universal Page Message that indicates no more page
        for this mobile in the current slot.
  FALSE: Otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean rxcfccch_empty_upm_and_done(byte *bits)
{
  /* Perform empty Universal Page match */
  if ( (bits[0] == 0x42 ) &&
       ( (bits[1] & 0x08) == 0x08 ) &&
       ( (bits[2] & 0xC0) == 0xC0 ) &&
           /* The above pattern match ensures that
              1. SCI (2 bits) is set to 1,
              2. EXT_MSG_LENGTH (1 bit) is set to 0.
              3. MSG_LEN (7 bits) is 8-bytes for an empty Universal Page 
                 Message, and
              3. MSG_TYPE is CAI_UNIVERSAL_PAGE_MSG, which is 0x11.
            */

       ( (bits[2] & 0x3F) == mcc_get_cur_msg_seq() ) &&
            /* current CFG_MSG_SEQ */

       ( (bits[3] & 0x03) == 0x00 ) &&
            /* READ_NEXT_SLOT = 0 and READ_NEXT_SLOT_BCAST = 0 */

       ( (cdma.assigning_tmsi.info.tmsi_code == INVALID_TMSI_CODE) ||
         ( (bits[4] & 0x20) == 0x00 ) ) &&   
            /* TMSI not assigned or TMSI_DONE */

       ( (bits[4] & 0x40) == 0x00 ) 
            /* CLASS_1_DONE and CLASS_0_DONE */

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
       && ( mcc_bc_sms_enable ?  ( (bits[4] & 0x80) == 0x00 ) : 1 )
            /* Check for BROADCAST_DONE */
#endif /* FEATURE_IS2000_REL_A_CC_BSMS */
    )
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }

} /* rxcfccch_empty_upm_and_done */
#endif /* FEATURE_IS2000_REL_A_CC_QPM */


/*===========================================================================

FUNCTION RXCFCCCH_ISR

DESCRIPTION
  This function is the interrupt handler for the Decoder interrupt used
  when the Receive task is processing the FCCCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void rxcfccch_isr (void)
{
  rxc_dec_type *buf_ptr = NULL;
  rxc_isr_cmd_type *cmd_ptr = NULL;
  dec_frame_status_type dec_status = {0, 0, 0, 0};
  boolean  receive_slot_mask_zero = FALSE;
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
    MSG_LOW ("rxcfccch_isr got ISR command 0x%x", cmd_ptr->cmd, 0, 0);

    switch (cmd_ptr->cmd) {
      case RXC_TC_F:

        /* Always returns true.  Ignoring return value. */
        (void) rxctraffic_frame_bnd_init ();
        break;

      case RXC_WAKE_F:
        {
          qword tmp_qw;

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
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

          rxc_slot.active = TRUE;
  
          rxc_slot.after_slot_boundary = FALSE;

          /* Set countdown to re-initialize frame count after interrupt
           * timing has setlled down as we have just slammed and reprogrammed
           * the decoder mode.
           */
          rxc_time_cnt = 3;

          /* take care of any pending commands from the FCCCH state */
          if( raho_flag == TRUE )
          {
            raho_flag = FALSE;
          }
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

  if ( rxc_slot.skipping == TRUE )
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

      rxcfccch_info.first_frame = FALSE;
        /* This is definitely not the first frame of the slot after a WAKEUP. */
     
#ifdef FEATURE_IS2000_REL_A_CC_QPM
      rxc_slot.page_match = FALSE;
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

      srch_mux_page_match( FALSE );
        /* Definitely no page matching happening at this point */
    }

    MSG_LOW("  Decint: fr=%lx, slt=%d",  qw_lo(rxc_dec_frame),
       ( (word)(qw_lo( rxc_dec_frame ) >> 2) & rxc_slot.sci_mask ), 0 );

#ifdef FEATURE_IS2000_REL_A_CC_QPM
    rxcfccch_last_sec = dec_status.sec;
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

    /* add in protection against skipping too many frame due to SC error */
    MSG_HIGH( "Skping fr after wakeup", 0, 0, 0 );
    return;
  }
  
  /* Logging needs to keep track of frame quality history, so a buffer 
     should be sent regardless of CRC success.  If CRC fails, nothing
     is stored. */
  buf_ptr = (rxc_dec_type *) q_get (&rxc_dec_free_q);

#ifndef FEATURE_IS2000_REL_A_CC_QPM
  if( rxcfccch_info.first_frame == TRUE )
  {
    srch_mux_page_match( FALSE );
    rxcfccch_info.first_frame = FALSE;
  }
#endif /* !FEATURE_IS2000_RLE_A_CC_QPM */
  
  if (buf_ptr)
  {
    /* read in status bytes: CRC, Yamamoto, symbol error rates & energy */
    if (!dec_read_ccch_status (&dec_status, rxcfccch_info.data_rate))
    {
      ERR_FATAL("Can't read CCCH status",0,0,0);
    }
  
    srch_mux_idle_frame_stats(CAI_F_CCCH, dec_status.sec, 
                              dec_status.energy, dec_status.crc_passed); 

    if (dec_status.crc_passed)
    {
      /* This can be helpful for debugging. */
      MSG_MED ("FCCCH CRC ok S:0x%x E:0x%x", dec_status.sec,dec_status.energy, 0);
      rxcfccch_frame_crc_stats.good_crcs++;
      /* Indicate to the task level frame processor that this was a good 
       frame. This distinction is used for frame quality history in 
       logging. and mux statistic counter*/
      buf_ptr->dec_rate = CAI_FULL_RATE;
    
    }
    else
    {
      MSG_MED("FCCCH CRC bad S:0x%x E:0x%x",dec_status.sec,dec_status.energy, 0);
      rxcfccch_frame_crc_stats.bad_crcs++;
      /* Indicate to the task level frame processor that this was a bad 
       frame. This distinction is used for frame quality history in 
       logging. and mux statistic counter*/
      buf_ptr->dec_rate = CAI_NULL_RATE;
    }
  
    dec_read_ccch_data (buf_ptr->buf.buf, rxcfccch_info.data_rate);
  
    qw_equ(buf_ptr->frame, rxc_dec_frame);

#ifdef FEATURE_IS2000_REL_A_CC_QPM
    if( rxcfccch_info.first_frame == TRUE )
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
           sleeping */

        /* Quick page match for General Page Message */ 
        if (rxcfccch_empty_gpm_and_done(buf_ptr->buf.buf))
        {
          /* An empty General Page Message and no more page for this mobile
             in the current slot */
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

      srch_mux_page_match( page_match );

      rxcfccch_info.first_frame = FALSE;
    } /* end if( rxcfccch_info.first_frame == TRUE ) */
#endif /* FEATURE_IS2000_REL_A_CC_QPM */

  }
#ifdef FEATURE_IS2000_REL_A_CC_QPM
  else
  {
    if( rxcfccch_info.first_frame == TRUE )
    {
      rxc_slot.page_match = FALSE;

      srch_mux_page_match( FALSE );

      rxcfccch_info.first_frame = FALSE;
    }
  }
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

  // Set the signal always so the task gets to consume any leftover items
  // on the rxc_dec_q and return them to the rxc_dec_free_q
  (void) rex_set_sigs (&rx_tcb, RXC_INT_SIG);

#ifdef FEATURE_PLT
#error code not present
#endif

} /* rxcfccch_isr */


/*===========================================================================

FUNCTION RXCFCCCH_PROCESS_CMD

DESCRIPTION
  This function is the interrupt handler for FCCCH frames.  It is called from
  the RX chain controller when a FCCCH frame is received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static rxc_state_type
rxcfccch_process_cmd (void)
{
  rxc_cmd_msg_type *cmd_ptr = NULL;
  rxc_state_type next_state = RXC_FCCCH_STATE;
  boolean success = TRUE;

  (void) rex_clr_sigs (&rx_tcb, RXC_CMD_Q_SIG);

  while ((cmd_ptr = (rxc_cmd_msg_type *) q_get (&rxc_cmd_q)) != NULL) {
    switch (cmd_ptr->hdr.command) {
      
      case RXC_TCI_F:
      {
        MSG_MED("Got RXC_TCI_F", 0, 0, 0);
        next_state = rxc_tci_init (RXC_FCCCH_STATE, cmd_ptr);

        break;
      }
      
      case RXC_ACC_FADE_F:
      {
        MSG_MED("Got RXC_ACC_FADE_F", 0, 0, 0);
        //TODO: Review  P_REV check in rxcpaging.c.
        //IS95B is not used for FCCCH.  Need to check P_REV?
        rxcidle_set_fade_timer (RXCIDLE_ACCESS_FADE);
        
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
        /* Note: Currently, we ignore ''num_chan' as the driver only 
           supports 1 channel */
        if( cmd_ptr->hdr.cmd_hdr.sigs == (rex_sigs_type) SRCH_CMD_SIG )
        {
          MSG_MED("Got CHAN_CONFIG_F from SRCH", 0, 0, 0);
          raho_flag = TRUE;
        }
        else
        {
          MSG_MED("Got RXC_CHAN_CONFIG_F", 0, 0, 0);
        }
        next_state = rxcidle_chan_config (&cmd_ptr->chan_config.chan_info[0]);
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
        /* check if BC BCCH, copy the 40ms skip flag from SRCH to RXC BC BCCH*/
        if (cmd_ptr->chan_config.chan_info[0].ebcsms)
        {
          rxc_slot.skipping = cmd_ptr->chan_config.skip_frame;
        }
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

        break;
      }
  
      case RXC_CDMA_F:
      {
        MSG_MED("Got RXC_CDMA_F", 0, 0, 0);
        /* --------------------------------------------------
         * Transition back to CDMA init state without exiting
         * rxc_subtask()
         * -------------------------------------------------- */

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
        rxc_sc_init (RXC_FCCCH_STATE);
        next_state = RXC_SC_STATE;
        break;
      }
  
      case RXC_IDLE_HO_F:
      {
        MSG_MED("Got RXC_IDLE_HO_F", 0, 0, 0);
        rxcsar_terminate (&rxcfccch_sar_msg_desc,
          rxcfccch_frame_info_tbl[
                          RXC_UPPER_BOUNDS_CHECK(rxcfccch_info.data_rate, 
                                     DEC_NUM_CCCH_RATES)].subframe_flush_cnt);

        /* flush the accumalated fccch logging buffer */
        rxcfccch_frame_info_log_send ();
        break;
      }
  
      case RXC_WAKE_F:
      {
        /* call the function to install the corresponding decoder ISR */
        rxc_idle_activate();

        MSG_MED("Got RXC_WAKE_F", 0, 0, 0);
        
        /* Set flush count to 0 in case an Idle HO occured before sleep */
        rxcsar_terminate (&rxcfccch_sar_msg_desc, 0);

        INTLOCK ();

        /* Restore demodulator settings */
        rxcfccch_config_demod (rxcfccch_info.code_rate, 
          rxcfccch_info.data_rate,rxcfccch_chan_id);

        /* we want to prevent sleep */
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

        break;
      }
  
      case RXC_SLEEP_F:
      {
        rxcidle_set_slotted_status( RXCIDLE_SLOTTED_ASLEEP );

        MSG_MED("Got RXC_SLEEP_F", 0, 0, 0);

        /* Reset frame processor */
        rxcsar_terminate (&rxcfccch_sar_msg_desc, 0);

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
        rxcfccch_frame_info_log_send ();

        /* Tell the sleep task that we're ready for the CPU to halt
        ** The sleep task is lower priority, so it won't do anything
        ** until the RXC task suspends.
        */
        rxc_vote_ok_to_sleep();
        break;
      }

      case RXC_SLOT_F: //XXX
      {
        MSG_MED("Got RXC_SLOT_F", 0, 0, 0);
  
        INTLOCK();
        /* Make sure page matching is completely disabled */
        rxc_slot.page_match = FALSE;
        rxc_slot.blank      = FALSE;
        if (next_state == RXC_FCCCH_STATE)
        {
          /* set interrupts on for FCCCH */
          rxcfccch_activate();
          rxc_slot.skipping   = FALSE;
        }

        /* we want to prevent sleep */
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


/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_FRAME_PROC_INIT

DESCRIPTION
  This routine initializes the FCCCH message parser.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxcfccch_frame_proc_init (dec_ccch_data_rate_type data_rate) 
{
  rxcfccch_sar_frame_desc.frame_size =
  rxcfccch_frame_info_tbl[data_rate].frame_size;
  rxcfccch_sar_frame_desc.subframe_size =
  rxcfccch_frame_info_tbl[data_rate].subframe_size;
  rxcfccch_sar_frame_desc.sci_size = 2;
  rxcfccch_sar_frame_desc.total_subframe = 0;

  rxcsar_init_msg_descriptor (&rxcfccch_sar_msg_desc);
  
  return TRUE;
}


/* <EJECT> */
#ifdef FEATURE_DECODER_LOGGING
extern void rxcchain_log_frame_output (void *src_ptr, int length);
#endif

/*===========================================================================

FUNCTION RXCFCCCH_PROCESS_FRAMES

DESCRIPTION
  This procedure processes decoder frames from rxc_dec_q and converts them
  to FCCCH frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxcfccch_process_frames (rxc_dec_type *buf_ptr)
{
  boolean good_frame = FALSE;
#ifdef FEATURE_PLT
#error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rxcfccch_sar_frame_desc.frame_num = &buf_ptr->frame; //lint !e545 Suspicious use of & due to qword *frame_num 
  rxcfccch_sar_frame_desc.data = buf_ptr->buf.buf;

  if (!rxc_slot.after_slot_boundary)
  {
    uint32 current_fccch_frame;
    current_fccch_frame = qw_lo(buf_ptr->frame);
     
    /* check if this slot is indeed the intended slot for the MS */
    if ( ((current_fccch_frame >> 2) & rxc_slot.sci_mask) ==
         (rxc_slot.idle_pgslot   & rxc_slot.sci_mask))
    {
    /* indicate the MS is monitoring the intended slot */
      rxc_slot.after_slot_boundary = TRUE;
    }
  }

  if (buf_ptr->dec_rate == CAI_FULL_RATE) 
  {
    good_frame = TRUE;

    /* Note that there is a chance we can get a good message from a failed CRC frame.
    since the SAR does not handle parsing failed CRC frames correctly, so
    For now, only process FCCCH frames with CRC passed. In the future, we should modify
    the SAR, so that it can parser the failed CRC frames without effecting the next good
    CRC frames*/
    if (!rxcsar_process_fcsch_frame (&rxcfccch_sar_frame_desc, 
          &rxcfccch_sar_msg_desc))
    {
      MSG_ERROR("Do FCSCH Fr Fail",0,0,0);
    }

#ifdef FEATURE_PLT
#error code not present
#endif
    /* increase the MAC statistic counters */
    switch (rxcfccch_info.frame_size)
    {
      case CAI_FCCCH_20MS_FRAME :
        if (rxcfccch_info.data_rate == DEC_CCCH_FULL_RATE)
        {
          (void) parm_inc(CAI_FCCCH_8_ID, 1);
        }
        else if (rxcfccch_info.data_rate == DEC_CCCH_2X_RATE)
        {
          (void) parm_inc(CAI_FCCCH_9_ID, 1);
        }
        else if (rxcfccch_info.data_rate == DEC_CCCH_4X_RATE)
        {
          (void) parm_inc(CAI_FCCCH_10_ID, 1);
        }
        else
        {
          MSG_HIGH("Bad FCCCH data rate ",0,0,0);
        }
        break;
      case CAI_FCCCH_10MS_FRAME :
        if (rxcfccch_info.data_rate == DEC_CCCH_2X_RATE)
        {
          (void) parm_inc(CAI_FCCCH_6_ID, 1);
        }
        else if (rxcfccch_info.data_rate == DEC_CCCH_4X_RATE)
        {
          (void) parm_inc(CAI_FCCCH_7_ID, 1);
        }
        else
        {
          MSG_HIGH("Bad FCCCH data rate",0,0,0);
        }
        break;
      case CAI_FCCCH_5MS_FRAME :
        if (rxcfccch_info.data_rate == DEC_CCCH_4X_RATE)
        {
          (void) parm_inc(CAI_FCCCH_5_ID, 1);
        }
        else
        {
          MSG_HIGH("Bad FCCCH data rate",0,0,0);
        }
        break;

      case CAI_FCCCH_FRAME_SIZE_INVALID :

      default:
           MSG_HIGH("Bad FCCCH fr size",0,0,0);
           break;
    }
  }
  else
  {
#ifdef FEATURE_IS2000_REL_A_CC_SMMC
    /* Make note of the bad frame for NCCCH failsafe timer */
    rxcnccch_failsafe_timer(RXCNCCCH_NOTIFY_FCCCH_BAD);
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

    /*  FCCCH ERASURE frame  */
    (void) parm_inc(CAI_FCCCH_11_ID, 1);
  }

  /* Log frame info. */
  rxcfccch_log_frame_info (good_frame);

}
/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_MSG_ALLOC

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
rxcfccch_msg_alloc (
  void *client_data,
  unsigned int length
)
{
  void *data = NULL;

  rxtx_rx_msg_type *msg_ptr = NULL;

  /* Unused Parameter */
  (void) client_data;

  if (length <= sizeof (msg_ptr->msg.fccch.body)) {
    msg_ptr = (rxtx_rx_msg_type *) q_get (&rxtx_rx_free_q);

    if (msg_ptr) {
      data = msg_ptr->msg.fccch.body;
    }
  }
  return data;
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_MSG_DELIVER

DESCRIPTION
  This procedure processes decoder frames from rxc_dec_q and converts them
  to FCCCH frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void 
rxcfccch_msg_deliver (
  void *client_data,
  unsigned int length,
  void *data, /* Same pointer as what was returned from rxcsar_msg_alloc() */
  qword *frame_num
)
{

  rxtx_rx_msg_type *msg_ptr;

  /* Unused Parameter */
  (void) client_data;

  if (data) 
  {
  
    /* Upon receiving a valid message, set the fade timer */
    rxcidle_set_fade_timer (RXCIDLE_CURR_FADE);

#ifdef FEATURE_IS2000_REL_A_CC_SMMC
    /* If we get here, let the failsafe timer know FCCCH channel is good */
    rxcnccch_failsafe_timer(RXCNCCCH_NOTIFY_FCCCH_GOOD);
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

    /* Assume that RXCSAR did not alter the the value of data, 
       as it is documented as such. */
    msg_ptr = (rxtx_rx_msg_type *) (((byte *) data) - 
      FPOS (rxtx_rx_msg_type, msg.fccch.body)); //lint !e413 FPOS

    rxcfccch_msg_crc_stats.good_crcs++;
    MSG_HIGH("FCCCH MSG CRC ok, totl %d", rxcfccch_msg_crc_stats.good_crcs, 0, 0);
    
    /* Set valid_msg flag for SLOT_R report */
    if ( rxc_slot.active  &&  !rxc_slot.valid_msg  &&
                 rxc_slot.after_slot_boundary )
    {
      rxc_slot.valid_msg = TRUE;
    }

    /* Increase the total number of message */
    (void) parm_inc(CAI_FCCCH_1_ID, 1);
  
    /* Fill in the rest of the structure */
    msg_ptr->chn = RXC_CDMA_FCCCH;
    msg_ptr->length = length;
    msg_ptr->msg.fccch.length = (word) length;

    /* indicate to MC if there is a page matched */
    if (rxcmsg_deliver (CAIX_FCCCH, &msg_ptr->msg, length, frame_num,rxcfccch_increase_page_matched_cnt)) 
    {
      MSG_HIGH("MUX sends FCCCH message to MC",0,0,0);
    }
    /* reset the page matched flag */
    rxcfccch_increase_page_matched_cnt = FALSE; 
    
    // Send event
    mclog_report_event_msg_rxed(CAI_F_CCCH, msg_ptr->msg.fccch.body);

    /* We are now done with this message buffer.  Discard. */
    q_put (&rxtx_rx_free_q, &msg_ptr->link);
    msg_ptr = NULL;
  
#ifdef FEATURE_PLT
#error code not present
#endif
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_MSG_DISCARD

DESCRIPTION
  This procedure processes decoder frames from rxc_dec_q and converts them
  to FCCCH frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void 
rxcfccch_msg_discard (
  void *client_data,
  void *data
)
{
  rxtx_rx_msg_type *msg_ptr;

  /* Unused Parameter */
  (void) client_data;

  if (data) {
    /* Assume that RXCSAR did not alter the the value of data, 
       as it is documented as such. */
    msg_ptr = (rxtx_rx_msg_type *) (((byte *) data) - 
      FPOS (rxtx_rx_msg_type, msg.fccch.body)); //lint !e413 FPOS
  
    /* We are now done with this message buffer.  Discard. */
    q_put (&rxtx_rx_free_q, &msg_ptr->link);
    msg_ptr = NULL;
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_MSG_EVENT

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
rxcfccch_msg_event (
  void *client_data,
  rxcsar_msg_event_type event
)
{
  /* Unused Parameter */
  (void) client_data;

  if (event == RXCSAR_CRC_FAIL_S) 
  {
    rxcfccch_msg_crc_stats.bad_crcs++;

#ifdef FEATURE_IS2000_REL_A_CC_SMMC
    /* Make note of the bad msg for NCCCH failsafe timer */
    rxcnccch_failsafe_timer(RXCNCCCH_NOTIFY_FCCCH_BAD);
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

#ifndef FEATURE_PLT
    MSG_HIGH("FCCCH MSG CRC bad, %d", rxcfccch_msg_crc_stats.bad_crcs, 0, 0);
#endif
    /* Increase the total number of message */
    (void) parm_inc(CAI_FCCCH_1_ID, 1);
    /* Increase the total number of message that failed CRC */
    (void) parm_inc(CAI_FCCCH_2_ID, 1);
#ifdef FEATURE_PLT
#error code not present
#endif

  } else {
    /* Unknown event, discard */
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_FRAME_INFO_LOG_INIT

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
rxcfccch_frame_info_log_init (void)
{
  rxcfccch_log_info.log.num_frames = 0;

  log_set_code (&rxcfccch_log_info.log, LOG_FCCCH_FRAME_INFO_C);
  
  if (rxcfccch_info.frame_size == CAI_FCCCH_20MS_FRAME)
  {
    if (rxcfccch_info.data_rate == DEC_CCCH_FULL_RATE)
    {
      rxcfccch_log_info.log.data_rate = CAI_9600_BPS_20MS_FRAME;
    }
    else if (rxcfccch_info.data_rate == DEC_CCCH_2X_RATE)
    {
      rxcfccch_log_info.log.data_rate = CAI_19200_BPS_20MS_FRAME;
    }
    else if (rxcfccch_info.data_rate == DEC_CCCH_4X_RATE)
    {
      rxcfccch_log_info.log.data_rate = CAI_38400_BPS_20MS_FRAME;
    }
    else
    {
      rxcfccch_log_info.log.data_rate = CAI_FCCCH_RATE_INVALID;
    }
  }
  else if (rxcfccch_info.frame_size == CAI_FCCCH_10MS_FRAME)
  {
    if (rxcfccch_info.data_rate == DEC_CCCH_2X_RATE)
    {
      rxcfccch_log_info.log.data_rate = CAI_19200_BPS_10MS_FRAME;
    }
    else if (rxcfccch_info.data_rate == DEC_CCCH_4X_RATE)
    {
      rxcfccch_log_info.log.data_rate = CAI_38400_BPS_10MS_FRAME;
    }
    else
    {
      rxcfccch_log_info.log.data_rate = CAI_FCCCH_RATE_INVALID;
    }
  }
  else if (rxcfccch_info.frame_size == CAI_FCCCH_5MS_FRAME)
  {
    if (rxcfccch_info.data_rate == DEC_CCCH_4X_RATE)
    {
      rxcfccch_log_info.log.data_rate = CAI_38400_BPS_5MS_FRAME;
    }
    else
    {
      rxcfccch_log_info.log.data_rate = CAI_FCCCH_RATE_INVALID;
    }
  }
  else  /* invalid frame size */
  {
    rxcfccch_log_info.log.data_rate = CAI_FCCCH_RATE_INVALID;
  }
  rxcfccch_log_info.log.code_rate = rxcfccch_info.code_rate;
  rxcfccch_log_info.log.code_chan = rxcfccch_info.code_chan;
  rxcfccch_log_info.log.num_frames = 0;
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_LOG_FRAME_INFO

DESCRIPTION
  This procedure accumulates and logs frame info for FCCCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static const byte rxc_fccch_good_frame_ind_mask[RXCFCCCH_NUM_LOG_FRAME_PER_BYTE] 
                        = {0xc0,0x30,0x0c,0x03};
static const byte rxc_fccch_bad_frame_ind_mask[RXCFCCCH_NUM_LOG_FRAME_PER_BYTE] 
                        = {0x80,0x20,0x08,0x02};
static void 
rxcfccch_log_frame_info (boolean frame_quality_ind)
{
  unsigned int idx = 0;
  byte mask_index = 0;

  /* No need to accumulate stats unless it is enabled. */
  if (log_status (LOG_FCCCH_FRAME_INFO_C)) {
    if (rxcfccch_log_info.log.num_frames == 0) {
      /* Timestamp indicates the first frame in the history */
      log_set_timestamp (&rxcfccch_log_info.log);
    }

    /* Calculate the array index and bit mask for the log history 
     * field before incrementing num_frames. Since we need 2 bit per frame
     * and 8 bits per byte, so we only increase the index for every 4 frames
     */
    idx = rxcfccch_log_info.log.num_frames / RXCFCCCH_NUM_LOG_FRAME_PER_BYTE;
    mask_index = rxcfccch_log_info.log.num_frames % RXCFCCCH_NUM_LOG_FRAME_PER_BYTE;
    if(mask_index == 0)
    {
      /* reset the log byte to zero */
      rxcfccch_log_info.log.frame_quality[idx] = 0;
    }

    /* Now increment the num_frames count */
    rxcfccch_log_info.log.num_frames++;

    if (rxcfccch_log_info.log.num_frames <= RXCFCCCH_FRAME_LOG_HISTORY_SIZE) {


      /* '11' if good frame, '10' if not */
      if (frame_quality_ind) 
      {
        rxcfccch_log_info.log.frame_quality[idx] |= rxc_fccch_good_frame_ind_mask[mask_index];
      }
      else
      {
         rxcfccch_log_info.log.frame_quality[idx] |= rxc_fccch_bad_frame_ind_mask[mask_index];
      }
      if (rxcfccch_log_info.log.num_frames >= (RXCFCCCH_FRAME_LOG_HISTORY_SIZE - RXCFCCCH_NUM_LOG_FRAME_PER_BYTE))
      {
        rxcfccch_frame_info_log_send ();
      }
    } else {
      /* Oh well - we tried.  Reset the buffer and consider that one dropped */
      rxcfccch_frame_info_log_init ();
    }
  } else if (rxcfccch_log_info.log.num_frames > 0) {
    /* To save CPU, only init if not initialized. */
    rxcfccch_frame_info_log_init ();
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_FRAME_INFO_LOG_SEND

DESCRIPTION
  This procedure send the accumulated log buffer for frame info logging.

DEPENDENCIES
  rxcfccch_frame_info_log_init() must have been called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxcfccch_frame_info_log_send (void)
{
  log_set_length (&rxcfccch_log_info.log, FPOS (mux_fccch_frame_info_type, 
    frame_quality) + (rxcfccch_log_info.log.num_frames + RXCFCCCH_NUM_LOG_FRAME_PER_BYTE -1) 
                              / RXCFCCCH_NUM_LOG_FRAME_PER_BYTE);

  if (rxcfccch_log_info.log.num_frames > 0 &&
      log_submit (&rxcfccch_log_info.log)) {
    /* Successfully sent a log, re-initialize */
    rxcfccch_frame_info_log_init ();
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_RESET_STATS

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
rxcfccch_reset_stats (void)
{
  rxcfccch_frame_crc_stats.good_crcs = 0;
  rxcfccch_frame_crc_stats.bad_crcs = 0;

  rxcfccch_msg_crc_stats.good_crcs = 0;
  rxcfccch_msg_crc_stats.bad_crcs = 0;

#ifdef FEATURE_PLT
#error code not present
#endif
}

void rxcfccch_exit_clean_up (void)
{

  rxc_nccch_deint_ram_mapping_done = FALSE;
  /* reset the fccch info structure, so that this will force a FCCCH re-config */
  rxcfccch_info.code_rate = DEINT_NUM_CODE_RATES;
  rxcfccch_info.data_rate = DEC_NUM_CCCH_RATES;
  rxcfccch_info.code_chan = 0xFF;
  rxcfccch_info.frame_size = CAI_FCCCH_FRAME_SIZE_INVALID;
}

/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_EXIT

DESCRIPTION
  This function performs processing necessary to end the Paging channel state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxcfccch_exit (void)
{
  MSG_HIGH("Dis FCCCH", 0, 0 ,0);

  #ifdef FEATURE_IS2000_REL_A_CC_SMMC
  rxc_read_fccch_data_from_decoder = FALSE;
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

  rxcsar_terminate (&rxcfccch_sar_msg_desc, 0);
  rxcfccch_frame_info_log_send (); 
  rxc_vote_not_ok_to_sleep();

  rxcfccch_info.first_frame = FALSE;

  /* call the clean up function to reset the fccch info structure */
  rxcfccch_exit_clean_up();

} /* rxcfccch_exit */

/* <EJECT> */
/*===========================================================================

FUNCTION RXCFCCCH_UPDATE_FRAME_COUNTERS

DESCRIPTION
  This function updates the various frame counters every interrupt

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxcfccch_update_frame_counters(void)
{
  mccrx_rpt_type *rpt_ptr;
    /* pointer to report to send to Main Control task */

  if( rxc_slot.active)
  {
    /* counting slots */
    if( rxc_slot.slot_frames > 0 )
    {
      rxc_slot.slot_frames--;
    }

    MSG_LOW("valid_mask=%d, valid_msg=%d",
           rxc_slot.valid_msg_mask,
           rxc_slot.valid_msg,
           0);
 
    if (rxc_slot.valid_msg)
    {
      byte rpt_slot_mask=0; /* the mask to be passed in SLOT_R */
      if ((rxc_slot.slot_frames==0)&&(rxc_slot.valid_msg_mask&PAGE_SLOT))
      {
        rpt_slot_mask |= PAGE_SLOT;
      }

      if (rpt_slot_mask != 0)
      {
        if ((rpt_ptr = (mccrx_rpt_type *)q_get(&mcc_rx_free_q) ) != NULL)
        {
          rpt_ptr->hdr.rpt = RXC_SLOT_R;
          ts_get_20ms_frame_time( rpt_ptr->slot.rpt_time );
          rpt_ptr->slot.slot_mask = rpt_slot_mask;
          /* we don't stop counting slot frames like before, the 
           * decision is left for CP to wake. rxc_slot.active cleared 
           * when RXC_SLEEP_F is rx'd */
          rxc_mc_rpt(rpt_ptr );
          /* clear the bits that triggered the valid msg report, so we
           * don't send the same report more than once */
          rxc_slot.valid_msg_mask &= ~rpt_slot_mask;
          MSG_HIGH("Tx RXC_SLOT_R to MC ",0,0,0);
        }
        else
        {
          ERR( "0 bufs on mcc_rx_free_q", 0, 0, 0 );
        }
      }
    }
  } /* end of frame counting */
} /* rxcfccch_update_frame_counters */

#endif /* FEATURE_IS2000_REL_A_CC */

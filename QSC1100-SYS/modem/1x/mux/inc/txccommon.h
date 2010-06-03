#ifndef TXCCOMMON_H
#define TXCCOMMON_H

/*===========================================================================

          C D M A    T R A N S M I T   C O M M O N   M O D U L E

 DESCRIPTION
  This header file contains definitions for the transmit common module that
  are used by the txc unit.  This header file is internal to the txc unit.

Copyright (c) 1997 - 2008 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/txccommon.h#3 $ $DateTime: 2008/07/28 09:31:07 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/08   jtm     Updated TXC_UPR_BNDS_CHK to TXC_UPPER_BOUNDS_CHECK.
07/16/08   jtm     Removed unused variables for ULC effort. Added the macro 
                   TXC_UPR_BNDS_CHK.
04/02/07   vlc     Added new PCG define for bin0 ENC ISRs specifically for loop
                   back calls.  Moved default PCG to 1 for bin0 ENC ISRs for 
                   all other calls.
01/22/07   trc     Parameterize the PCG for bin0 and bin3 ENC ISRs and bump
                   bin0 from PCG 1 to PCG 3.
10/30/06   bn      Added code to support DO/UMTS to 1x handoff
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
06/09/06   trc     Remove T_MSM6700 featurization
03/22/06   bn      Set the TXC_IS95A_FCH_GAIN to (255) / sqrt(2)
05/23/05   vlc     Changed FEATURE_IS2000_REL_D_JUMP to FEATURE_RL_JUMP_ENHANCEMENT.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
04/06/05   vlc     Changed TXC_JUMP_CMD to TXC_JUMP.
03/07/05   vlc     Added support for Fast Call Set Up - New Jump to
                   Hyperspace command feature.
09/01/04   jrp     Added macros for differences between pre-6700 and post-6700
                   targets.
06/29/04   jrp     Added define for TXC_IS95A_FCH_GAIN
05/07/04   vlc     Modified RLP logging extensively to decouple MUX processing
                   and RLP processing, moved RLP processing to RLP module.
04/26/04   vlc     Implemented new way of handling RXTX confirmation of all
                   signalling messages for voice calls/data calls/svd calls.
04/20/04   jrp     Fixed lint warnings.
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
02/03/04   sab     Featurized confirmation_type under Release B.
08/25/03   vlc     Removed shared rate_set from txc_so (txc_so_type) to avoid
                   confusion between FCH and DCCH rate sets.  Now calculate
                   rate set from RC as it is needed.
08/06/03   sab     Added confirmation_type to txc_tx.
08/23/02   jrp     Changed MAX_REV_MSG_BYTES for Release A to allow space for
                   extended message size.
05/22/02   bn      Added new logics to support VP2 RPC
04/08/02   tc      Supported REACH 20ms frame, 38400bps pad size.
02/25/02   bn      Added support for  MUX SVD signaling
10/30/01   bn      Added support for Release A RDCH logging
08/28/01   lh      Back out SI bits changes for Release A Addendum 2.
08/13/01   bn,hrk  Added prototype for txc_frame_types_send_log().
06/28/01   vlc     Removed rev_sch_rc from txc_so_type (it's a duplication
                   of r_sch_rc in txtc_type.)
06/15/01   day     Mainlined FEATURE_FFPC, FEATURE_DATA_TIMELINE_ADVANCE, FEATURE_IS2000
                   Deleted FEATURE_FINGER_POLL
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
04/26/01   vlc     Added rev_sch_rc (to support SCH RC4) to txc_so_type.
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
                   Moved txc_scr_action() and txc_scr_trans_cmd to another file.
03/07/01   lh      Changes for LAC support for Rel A.
03/01/01   tc      Merged MSM_MUX1X.00.00.15.
02/16/01   hrk     Added prototype for txc_rpctrl_send_log().
01/30/01   tc      Merged GPS support.
01/12/01   tc      Merged MSM_MUX1X.00.00.06.
11/13/00   tc      Defined txc_pch_gain, txc_fch_gain, txc_sch_gain.
11/09/00   tc      Defined feature T_MSM_5100.
11/08/00   tc      Supported MSM5100 modulator driver interface.
11/10/00   jc      Created after repartition of txc.c

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "cai.h"
#include "bio.h"
#include "log.h"
#include "enci.h"
#include "txc.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Define the PCGs (0-15) for which we want to receive an interrupt from the
   encoder during the two phases of the Tx timeline. The first phase of the TX
   timeline (slot 0) builds the upcoming Tx frame/s, while the second phase
   actually gives the frame/s to the encoder.

   The strategy here is to ensure that the slot0 interrupt occurs early enough
   to allow the frame to be built in-time for the slot3 interrupt where the
   actual frame will be sent to the encoder. The earliest PCG to build
   a frame is PCG 1, to allow for the slight difference between forward link
   and reverse link system time due to the BTF.

   The corner-case is a loopback call, where we want to immediately
   Tx the frame that we just Rx'ed.  The slot0 interrupt needs to happen late
   enough into the frame to ensure that the Rx frame data has been extracted
   and made available to the Tx subsystem.  Currently, the loopback call is the
   only call requiring a slight delay in building the frame, it uses PCG 3.  
   All the other service options use PCG 1 for the slot0 interrupt.
*/
#define TXC_SLOT0_ISR_PCG_NUM       1   /* slot0 ISR occurs @ frame + 1.25ms  */
#define TXC_SLOT0_ISR_LB_PCG_NUM    3   /* slot0 ISR occurs @ frame + 3.75ms  */
#define TXC_SLOT3_ISR_PCG_NUM       13  /* slot3 ISR occurs @ frame + 16.25ms */

/* PCG value for bin0 ENC ISRs */
extern byte txc_slot0_isr_pcg;

#define PREAMBLES_TO_GATING_PATN(p) ((word) ( (word)((1 << (p)) - 1) << (16-(p))))

extern word txc_pch_gain;
extern word txc_fch_gain;
extern word txc_sch_gain;
extern word txc_dcch_gain;

#define TXC_SET_PCH_GAIN(gain) muxmdsp_wr_mod_pch_gain( gain )
#define TXC_SET_FCH_GAIN(gain) muxmdsp_wr_mod_fch_gain( gain )
#define TXC_SET_DCCH_GAIN(gain) muxmdsp_wr_mod_dcch_gain( gain )
#define TXC_SET_SCH_GAIN(gain) muxmdsp_wr_mod_sch_gain( gain )
#define TXC_LAST_ENC_INT_BIN txtc.last_encoder_interrupt_bin

/* commands to the interrupt handlers */
typedef enum {
  INT_TC_F,                       /* traffic channel */
  INT_FRAME_F                     /* initialize frame count */
}txc_int_cmd_type;

/* Gain value for IS-95 waveform (RC1 & RC2)
** Only 1 traffic channel on RL
** Allocate all power to traffic channel
** Gain setting for traffic channel is a constant
** OLD (8-bit):     255 / sqrt(2) = 180
** NEW (15-bit):    (255 << 7) / sqrt(2) = 23079
**   Sqrt(2) is for gain adjustment for QPSK
*/
#define TXC_IS95A_FCH_GAIN (23079)

/* define buffers to hold the messages to be sent */
#ifdef FEATURE_IS2000_REL_A
#define MAX_REV_MSG_BYTES   MAX( CAI_REV_TC_MAX_MSG_BYTES, CAI_REV_CSCH_MAX_MSG_BYTES)
  /* This constant will be used for boundary checking for SAR sublayer*/
#else
#define MAX_REV_MSG_BYTES   MAX( CAI_REV_TC_MAX_MSG_BYTES, CAI_AC_MAX_MSG_BYTES)
  /* This constant will be used for boundary checking for SAR sublayer*/
#endif // FEATURE_IS2000_REL_A

typedef struct {
  txc_cmd_type *cmd_ptr;           /* command pointer */
  boolean     blank;              /* blank and burst. */
#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#else /* FEATURE_TC_EXTENDED_MSG_LENGTH */
  word        len;                /* length, in bits of the message */
  word        pos;                /* current position, in bits */
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
  qword       mask;               /* access channel mask */
  byte        msg[MAX_REV_MSG_BYTES]; /* the message */
} txc_msg_buf_type;

/* macro used to set interrupt command and wait for one interrupt to occur */
#define DO_INT_CMD()                    \
  (void)rex_clr_sigs( &tx_tcb, TXC_INT_CMD_SIG );  \
  txc_int_cmd.iscmd = TRUE;              \
  (void)txc_wait( TXC_INT_CMD_SIG );               \
  (void)rex_clr_sigs( &tx_tcb, TXC_INT_CMD_SIG );


/* This moves the conv encoder interrupt to 10ms from the begining of the
** frame. This is done to separate the viterbi decoder interrupt which happens
** at 4.5ms into the frame and the conv encoder interrupt which *used to*
** happen at 5ms previously. Applicable only to voice calls.
*/
#define TXC_INT_BIN_MASK_BIN_0       0x0001      /* 20 ms bin 0    */
#define TXC_INT_BIN_MASK_BIN_1       0x0002      /* 20 ms bin 1    */
#define TXC_INT_BIN_MASK_BIN_2       0x0004      /* 20 ms bin 2    */
#define TXC_INT_BIN_MASK_BIN_3       0x0008      /* 20 ms bin 3    */
#define TXC_INT_OFFSET_PCG_0         0x0000      /* 5 ms  pcg 0    */
#define TXC_INT_OFFSET_PCG_1         0x0001      /* 5 ms  pcg 1    */
#define TXC_INT_OFFSET_PCG_2         0x0002      /* 5 ms  pcg 2    */
#define TXC_INT_OFFSET_PCG_3         0x0003      /* 5 ms  pcg 3    */
#define TXC_INT_OFFSET_WALSH_0       0x0000      /* walsh symbol 0 */
#define TXC_INT_OFFSET_WALSH_1       0x0001      /* walsh symbol 1 */
#define TXC_INT_OFFSET_WALSH_2       0x0002      /* walsh symbol 2 */
#define TXC_INT_OFFSET_WALSH_3       0x0003      /* walsh symbol 3 */
#define TXC_INT_OFFSET_WALSH_4       0x0004      /* walsh symbol 4 */
#define TXC_INT_OFFSET_WALSH_5       0x0005      /* walsh symbol 5 */

#define TXC_INT_OFFSET_PCG      8                /* 1 PCG = 1.25 ms */

#define TXC_INT_OFFSET_PCG_DATA 4                /* 1 PCG = 1.25 ms */

#define TXC_INT_OFFSET_WALSH    0

/* Queue definitions */
extern q_type txc_cmd_q;

/* Timer to wake Transmit task so that it can set its Watchdog task
   Report signal */
extern rex_timer_type  txc_rf_timer;

/* Data rate of current frame */
extern cai_data_rate_type txc_data_rate;

#ifdef FEATURE_IS2000_REL_A_CC

/* REACH 20ms frame, 38400bps data rate, 768 bits per frame. */
/* It needs 96 bytes of zeros for padding the REACH frame.    */

#define TXC_REACH_20MS_FRAME_38400_DATA_RATE_SIZE   96

extern byte zeros[TXC_REACH_20MS_FRAME_38400_DATA_RATE_SIZE];

#else

extern byte zeros[ENC_14400_FULL_RATE_SIZE];

#endif // FEATURE_IS2000_REL_A_CC

/* The possible states the TXC task can be in */
typedef enum {
  TXC_CDMA_S,                      /* Initializing CDMA */
  TXC_ACC_IDLE_S,                  /* Idle on Paging channel */
  TXC_ACC_JUMP_S,                  /* Performing jump to hyperspace */
  TXC_ACC_ATT_S,                   /* Performing an access attempt */
#if defined(FEATURE_VOIP_1X_HANDOFF) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
  TXC_TT_S,                        /* Set up the Time Transfer to be ready on the RL */
#endif /* FEATURE_VOIP_1X_HANDOFF and FEATURE_UMTS_1X_HANDOVER_1XMSM */
#ifdef FEATURE_RL_JUMP_ENHANCEMENT
  TXC_HOLD_ENC_CLK_S,              /* Hold ENC clocks ON */
#endif /* FEATURE_RL_JUMP_ENHANCEMENT */
  TXC_TRAFFIC_S,                   /* Traffic channel */
  TXC_EXIT_S                       /* Stop state */
}txc_state_type;
/* Curent state of TXC state machine */
extern txc_state_type  txc_state;

/* command block for interrupt handler commands */
typedef struct  {
  boolean iscmd;                  /* is there a command? */
  txc_int_cmd_type    cmd;
  union
  {
    struct {                      /* traffic channel */
      qword mask;                 /* long pn mask to use */
    } tc;
  } parm;
} txc_int_cmd_type_main;
extern txc_int_cmd_type_main txc_int_cmd;

/* Values of txc_tx.delay.
*/
typedef enum {
  TXC_NO_CONF,       /* Do not need to send signalling confirmation */
  TXC_DELAY_CONF,    /* Delay sending confirmation by 1 frame */
  TXC_SEND_CONF      /* Send signalling confirmation this frame */

} txc_delay_conf_type;

/* Values of txc_tx.jump_source.
*/
typedef enum {

  TXC_READY_TO_JUMP, /* Has not jump to hyperspace yet */
  TXC_JUMP,          /* Jump to hyperspace */
  TXC_JUMP_ACCESS,   /* Jump to hyperspace prior to access state */
  TXC_JUMP_TRAFFIC   /* Jump to hyperspace prior to traffic state */

} txc_jump_source_type;

/* hold most of the global variables for the transmit task */
typedef struct {
  word             int_cnt;        /* count interrupts */
  qword            frame_cnt;      /* count transmit frames */
  qword            mask;           /* long pn mask to use */
  boolean          active_msg;     /* flag for active message buffer */
  boolean          tx_on;          /* indicator of whether transmitter
                                      is on */
  boolean          tx_blank;       /* expect a blank from vocoder */
  txc_jump_source_type jump_source; /* why TXC performs jump to hyperspace */
  txc_delay_conf_type delay;       /* may need to delay signalling msg
                                      confirmation to MC by 1 frame */
  txc_msg_buf_type buf;            /* buffer to send */
#ifdef FEATURE_IS2000_REL_B
  txc_confirmation_type confirmation_type;   /* indicates when rxtx wants the
                                                confirmation */
#endif /* FEATURE_IS2000_REL_B */
} txc_tx_type;
extern txc_tx_type txc_tx;

typedef struct
{
  boolean pending;     /* Indicates whether a frame offset cmd is pending */
  byte frame_offset;   /* Traffic Channel frame offset */
  qword action_frame;  /* Frame when frame offset takes effect */
} txc_frame_offset_cmd_type_pend;
extern txc_frame_offset_cmd_type_pend txc_frame_offset_cmd;

extern byte txc_frame_offset;
/* Offset of the transmit interrupt from the frame boundary for Traffic
** Channel.*/

#if defined (FEATURE_DIAG_V7_LOG_IFACE)
extern q_type txc_log_msg_q;              /* for logging sent messages */
#ifdef TMOBILE
#error code not present
#endif

/*-------------------------------------------------------------------------*/
/* Buffers for Logging Queues                                              */
/*-------------------------------------------------------------------------*/

typedef struct {
  log_desc_type desc;             /* log descriptor for logging services */
  log_voc_type  voc;              /* vocoder frame log type */
} txc_log_voc_type;

typedef struct
{
  log_desc_type desc;             /* log descriptor for logging services */
  log_r_mux1_type  entry;         /* transmitted frame rates log */
} txc_log_mux1_type;
extern txc_log_mux1_type *txc_mux1_ptr;
    /* pointer to current log buffer */


typedef struct
{
  log_desc_type desc;             /* log descriptor for logging services */
  union
  {
    log_hdr_type       hdr;       /* log header */
    log_ac_msg_type    ac_msg;    /* access channel message buffer */
    log_r_tc_msg_type  tc_msg;    /* traffic channel message buffer */
  }entry;
} txc_log_msg_type;
/* buffers for txc_log_msg_q */
#define                          TXC_NUM_MSG_BUFS 4
extern txc_log_msg_type txc_log_msg_pool[ TXC_NUM_MSG_BUFS ];
#endif /* FEATURE_DIAG_V7_LOG_IFACE */


typedef struct
{
  struct
  {
    #ifdef FEATURE_IS2000_P2
    boolean included;
    #endif
    cai_radio_config_type rc;
  }
  #ifdef FEATURE_IS2000_P2
  rev_fch, rev_dcch;
  #else
  rev_fch;
  #endif  // FEATURE_IS2000_P2
  fpc_mode_type fpc_mode;
#ifndef FEATURE_IS2000_REL_A
  word pri;
  word sec;
  word stored_pri;
#else
  struct
  {
    word    so;                 /* service option */
    word    phy_channels;       /* physical channel mapping */
  } pri, sec;

  word signaling_phy_channel;   /* FCH or/and DCCH carrying signaling data */
  word rev_sch_mux_option;
#endif /* FEATURE_IS2000_REL_A */

} txc_so_type;


#ifdef FEATURE_IS2000_REL_A
#define SR_ID_MAX    8          /* service reference option */

typedef struct
{
   word   traffic_type;         /* 0 for primary, 1 for secondary */
   word   phy_channels;         /* which channel FCH or/and SCH or/and DCCH carries
                                primary data.We are only expecting the corresponding
                                data type(pri or sec) will come from the corresponding
                                phy_channels */

   word   so;                   /* service option */
} txc_srid_ref_type ;

extern txc_srid_ref_type  txc_srid_ref[SR_ID_MAX];

#endif /* FEATURE_IS2000_REL_A */

extern txc_so_type txc_so;

/* gain table taken from Tige2 HLD */
extern txc_gain_type txc_pch_fch_sch_gain_tab[ ENC_NUM_SCH_RATES ];

extern txc_gain_type txc_pch_fch_gain_tab[ ENC_NUM_FRAC_RATES ];

/* Adjusted R-PCH Gain Table when only R-FCH is active
** and R-SCH is not assigned */
extern int16 txc_pch_adj_gain_tab[ ENC_NUM_FRAC_RATES ];

#ifdef FEATURE_IS2000_R_SCH
   extern int16 txc_sch_pch_adj_gain_tab[ ENC_NUM_FRAC_RATES ];
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_R_SCH
  /* Gain Table when R-FCH and R-SCH is assigned and R-SCH is active */
  extern txc_gain_type txc_gain_tab[ ENC_NUM_FRAC_RATES ];

  /* Gain table when R-FCH and R-SCH is assigned but R-SCH is inactive */
  extern txc_gain_type txc_dtx_gain_tab[ ENC_NUM_FRAC_RATES ];

  extern int16 txc_dtx_pch_adj_gain_tab[ ENC_NUM_FRAC_RATES ];
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_P2
  /* gain table for various combinations of DCCH and SCH operating modes */
  extern txc_dcch_sch_gain_type txc_dcch_sch_gain_tab[TXC_DCCH_SCH_CONFIG_MODES];
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_REL_A_SVD
/* VP2 RPC new structure */
  extern txc_fch_dcch_sch_gain_type txc_fch_dcch_sch_gain_tab[ENC_NUM_FRAC_RATES][TXC_DCCH_SCH_CONFIG_MODES];
#endif /* FEATURE_IS2000_REL_A_SVD */

extern word  txc_action_time;
  /* System Action Time as specified in OTA message */

extern boolean txc_action_time_chk_enabled;
  /* When TRUE, txc_traffic() should check if the action time is up
   * while processing "TXC_INT_SIG". If the action time is up,
   * TXC will send "MCC_TXC_FRAME_SIG" to MCC
   */

extern boolean txc_action_time_stop_tx_enabled;
  /* When TRUE, TXC will command the transmitter to be shut down at
   * action time, i.e. while the first MCC_TXC_FRAME_SIG is generated
   */

#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
  extern boolean txc_next_int_occurred;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

#define TXC_UPPER_BOUNDS_CHECK(index, array_size) \
          (((index) < (array_size)) ? (index):((array_size)-1))

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION TXC_CMD_DONE

DESCRIPTION
  Dispose of a command buffer when we're done with it.  Depending of
  the contents of the buffer, this will involve placing it on a free
  queue if a free queue is designated,  and if a tcb is given,  the
  appropriate task will be notified.

DEPENDENCIES
  None.
RETURN VALUE
  Just returns the status passed to it
SIDE EFFECTS
  Could signal another task
===========================================================================*/
extern void txc_cmd_done(
  txc_cmd_type   *cmd_ptr,         /* buffer to dispose of */
  word          status            /* set status of buffer */
);


/*===========================================================================

FUNCTION TXC_CLR_MSGS

DESCRIPTION
  Move any pending messages in the messages buffer over to the free
  queue,  effectively erasing any queued messages.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
extern void txc_clr_msgs( void );

/*===========================================================================

FUNCTION TXC_WAIT

DESCRIPTION
  Wait for the watchdog and for any other signals that might get set.
  Return if one of the other signals is set.  Always kick the watchdog
  whenever we wake up.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the signals for the current task.

SIDE EFFECTS
  None.

===========================================================================*/
extern rex_sigs_type txc_wait
(
  word requested_mask
    /* Mask of signals to wait for */
);


/*===========================================================================

FUNCTION TXC_CDMA_ENC_SETUP

DESCRIPTION
  Set up the encoder / interleaver / deinterleaver for CDMA

DEPENDENCIES
  Assume interleaver already hard reset
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
extern void txc_cdma_enc_setup(void);

/*===========================================================================

FUNCTION TXC_CDMA_ENC_SHUTDOWN

DESCRIPTION
  Shut down the encoder / interleaver / deinterleaver & PA for CDMA

DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
extern void txc_cdma_enc_shutdown(void);

/*===========================================================================

FUNCTION TXC_EXIT

DESCRIPTION
  This function performs processing necessary for exiting CDMA Transmit
  subtask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void txc_exit (void );

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_RPCTRL_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated reverse power control frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A reverse power control log may be sent.

===========================================================================*/
void txc_rpctrl_send_log(void);

/* <EJECT> */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION TXC_RDCH_FRAME_INFO_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated RDCH frame information log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A RDCH frame information log may be sent.

===========================================================================*/
void txc_rdch_frame_info_send_log(void);
#else
/*===========================================================================

FUNCTION TXC_FRAME_TYPES_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated reverse power control frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A reverse power control log may be sent.

===========================================================================*/
void txc_frame_types_send_log(void);

#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION TXC_RPCTRL_LOG_INIT

DESCRIPTION
  This function initializes the Reverse Power Control log subrecord, as well as the
  pointer to it and the frame entry, in the log buffer.  If necessary, the log is
  sent and a new one is generated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A reverse power control log may be sent.

===========================================================================*/
extern log_rev_pctrl_frame_type * txc_rpctrl_log_frame_init(void);

#endif /*TXC_COMMON_H*/


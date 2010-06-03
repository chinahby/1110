#ifndef MUXMDSP_H
#define MUXMDSP_H

/*===========================================================================

                      MUX mDSP interface header File

General Description
  This file contains declarations for MUX mDSP interface data structures and
  function protopes.

Copyright (c) 1993 through 2008 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                             Edit History

$PVCSPath:   $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/muxmdsp.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV and obsolete prototypes.
09/13/07   vlc     Increased MUXMDSP_TX_RATE_ADJ_LATCHED_OFFSET for targets 
                   with HW controlled TXAGC.
02/09/07   vlc     Put inclusion of muxdvlog.h under REL C feature.
06/09/06   trc     Remove T_MSM6700 featurization
03/14/05   vlc     Fixed compiler warnings for MSM6700.
11/19/04   jyw     Removed the include to the "dv_swInteface.h"
11/08/04   sr      Added prototype for muxmdsp_wr_pdcchStartDelay()
10/18/04   vlc     Fixed typo, muxmdsp_rd_ackHistory is now muxmdsp_rd_ackchHistory.
09/10/04   ljl     Added the enum type muxmdsp_dvchannel_rl_gain_pdch_mode_type
                   and the function muxmdsp_wr_rlGainPdchMode().
09/01/04   jrp     Added 0.5x counters.
09/01/04   sr      Added MUX to list of mDSP Msg recipients
09/01/04   jrp     Conditionally compile entire file.
07/30/04   vlc     Added support for rel C TDSO.
07/29/04   jrp     Added interface for mdsp2arm_int_reg_clt_callback_function.
07/28/04   jrp     Renamed Rev D gain enums.  Removed lint warnings.
07/27/04   bn      Added the defined value for tx_rate_adj latched offset
07/26/04   bn      Added new function for tx_rate_adj latched frame offset
07/23/05   jyw     Added the definition of function
                   muxmdsp_wr_pdchInitialDescrambleState
07/21/04   bn      Added function for MDSP frame offset loading command
06/29/04   sr      Added prototypes for pass-through functions
06/24/04   sr      Added mdsp2arm_int_handler
06/18/04   bn      Added support for Rel D channel gains override
05/18/04   sr      Added prototype for startSwitchCmd
05/04/04   bn      Added the MDSP to ARM universal INT handler and registering utilizes
03/17/04   bn      Modified the register callback function to allow deregistering
03/09/04   sr/bkm  Featurized DV code
09/09/03   bn      Created MUX mDSP interface header File
===========================================================================*/

#include "target.h"
#include "comdef.h"
#include "cai.h"
#include "mdspext.h"

/*--------------------------------------------------------------------------
 * Definitions and Declarations
 *--------------------------------------------------------------------------*/


/***************  define bit field size and bit postion **********************/
/* this macro should be moved into the mDSP file  */


#define SHIFT_MASK( PARAM, FIELD, VAL )  (((VAL)&((1<<SIZE_##PARAM##__##FIELD)-1))<<POS_##PARAM##__##FIELD)



/******************  define data structure types for DV **********************/
/* define constants based on the dv_swInterface.h */
#define MASK_loggingCtrl                  0x1ff
#define MASK_overrideCtrl                 0x3f

/* DV channel enable or disable defined values */
#define MUXMDSP_PDCH_CHAN_DISABLE         0x0
#define MUXMDSP_PDCH_CHAN_ENABLE          0x1

/* ACKCH repeat factor defined values */
#define   MUXMDSP_ACKCH_REPEAT_FACTOR_1 0x1
#define   MUXMDSP_ACKCH_REPEAT_FACTOR_2 0x2
#define   MUXMDSP_ACKCH_REPEAT_FACTOR_4 0x4

/* define Override control bits */
#define   MUXMDSP_ACKCH_OVERRIDE_ENABLE   0x20
#define   MUXMDSP_RL_GAIN_OVERRIDE_ENABLE     0x10
#define   MUXMDSP_CQICH_OVERRIDE_ENABLE   0x08
#define   MUXMDSP_PDCH_OVERRIDE_ENABLE    0x04
#define   MUXMDSP_WALSH_MASK_OVERRIDE_ENABLE  0x02
#define   MUXMDSP_PDCCH_OVERRIDE_ENABLE   0x01

/* define MUX MDSP log control bits */
#define   MUXMDSP_SNDA_LOG_ENABLE       0x100
#define   MUXMDSP_RL_GAIN_LOG_ENABLE    0x80
#define   MUXMDSP_CQI_ACK_HI_LOG_ENABLE 0x40
#define   MUXMDSP_CQI_ACK_LO_LOG_ENABLE 0x20
#define   MUXMDSP_PDCH_FE_LOG_ENABLE    0x10
#define   MUXMDSP_PDCH_BE_LOG_ENABLE    0x08
#define   MUXMDSP_WALSH_LOG_ENABLE    0x04
#define   MUXMDSP_PDCCH_LOG_ENABLE_HI   0x02
#define   MUXMDSP_PDCCH_LOG_ENABLE_LO   0x01

/* This is the offset time between frame boundary and when MDSP updates digital
** channel gains and txRateAdj to HW.  Increase offset for targets with HW
** controlled TXAGC.  FW controlled TXAGC remains at 100 chips.
*/
#ifdef  FEATURE_MDSP_SUPPORTS_TX_AGC
/* define the TX_RATE_ADJ latched offset */
#define   MUXMDSP_TX_RATE_ADJ_LATCHED_OFFSET    100 /* 100 chips */
#else
/* define the TX_RATE_ADJ latched offset */
#define   MUXMDSP_TX_RATE_ADJ_LATCHED_OFFSET    640 /* 640 chips */
#endif /* FEATURE_MDSP_SUPPORTS_TX_AGC */

/* define DME maximum length based on 4KB diagnostic buffer limit and
 * maximum log header length. */
#define   MUXMDSP_DME_BUFFER_LENGTH   ((4096 - sizeof(muxdvlog_dv_info_type) - sizeof(muxdvlog_dv_info_gain_ratios_type) )/2)


/* enumeration to indicate serving sector or target sector in DV state */
typedef enum
{
   MUXMDSP_TARGET_SEC_REF,
   MUXMDSP_SERVING_SEC_REF,
   MUXMDSP_TOTAL_SEC_REF
} muxmdsp_sector_ref_type;


/*  data structure to store control parameters for PDCCH */
typedef struct
{
   uint8      num_chan;
   uint16     sector_pn_offset;
   uint8      mac_id;
} muxmdsp_pdcch_ctrl_type;

/* data structure for PDCH descramble operation */
typedef struct
{
   boolean  descramble_bypass;  /* used to bypass PDCH descramble operation */
   uint16 descramble_state;   /* initial descramble state for PDCH, total 17 bits, the
                        MSB is 1 and the others 16 bits are the LS 16 bits of
                        the long code mask*/
} muxmdsp_pdch_descramble_info_type;

/*  data structure to store walsh parameters for PDCH */
typedef struct
{
   uint8    walsh_table_id;
   uint16     walsh_mask;
} muxmdsp_pdchwalsh_info_type;

/*  data structure to store control parameters for PDCH */
typedef struct
{
   muxmdsp_pdchwalsh_info_type        pdch_walsh_info[MUXMDSP_TOTAL_SEC_REF];
   muxmdsp_pdch_descramble_info_type  descramble_info;
} muxmdsp_pdch_ctrl_type;


/* R-CQICH modulation path */
typedef  enum
{
   MUXMDSP_RCQICH_MOD_ON_Q,
   MUXMDSP_RCQICH_MOD_ON_I
} muxmdsp_rcqich_mod_path_type;

/* C/I feedback mode */
typedef  enum
{
   MUXMDSP_RCQICH_REPORT_DIFF_M,
   MUXMDSP_RCQICH_REPORT_FULL_M
} muxmdsp_rcqich_report_mode_type;

/* Reverse Pilot gating rate */
typedef  enum
{
   MUXMDSP_RPCH_GATING_MODE_NONE,
   MUXMDSP_RPCH_GATING_MODE_HALF,
   MUXMDSP_RPCH_GATING_MODE_QUARTER
} muxmdsp_rpch_gating_mode_type;

/* data structure for CQI control parameters */
typedef struct
{
   uint8                    frame_offset;
   muxmdsp_rcqich_mod_path_type     chan_mod_path;
   muxmdsp_rcqich_report_mode_type  report_mode;
   muxmdsp_rpch_gating_mode_type  gating_rate;
   uint8                    num_slot_full_rep_factor;
} muxmdsp_cqich_ctrl_info_type;


/* Switch command type */
typedef  enum
{
   MUXMDSP_START_SWITCH = 1,
   MUXMDSP_CONTINUE_SWITCH = 2,
   MUXMDSP_ABORT_SWITCH = 3
} muxmdsp_switch_cmd_type;

/* Switch command type */
typedef  enum
{
   MUXMDSP_WALSH_COVER_CTRL_TARGET = 0,
   MUXMDSP_WALSH_COVER_CTRL_TARGET_AND_SOURCE = 1
} muxmdsp_switch_ctrl_type;

/* CQICH switch command data structure */
typedef  struct
{
   uint8                      num_switching_frames;
   uint8                      num_switching_slots;
   uint8                      walsh_cover;
   muxmdsp_switch_ctrl_type     walsh_cover_ctrl;
   muxmdsp_switch_cmd_type      switch_cmd;
} muxmdsp_cqich_switch_cmd_type;

/* enum type for different channels gain ratio */
typedef  enum
{
   MUXMDSP_FCH_TO_PICH_GAIN_RATIO,
   MUXMDSP_DCCH_TO_PICH_GAIN_RATIO,
   MUXMDSP_SCH_TO_PICH_GAIN_RATIO,
   MUXMDSP_ACKCH_TO_PICH_GAIN_RATIO,
   MUXMDSP_CQICH_TO_PICH_GAIN_RATIO_L,
   MUXMDSP_CQICH_TO_PICH_GAIN_RATIO_H,
   MUXMDSP_SPICH_TO_PICH_GAIN_RATIO,
   MUXMDSP_REQCH_TO_PICH_GAIN_RATIO,
   MUXMDSP_PDCCH_TO_PICH_GAIN_RATIO,
   MUXMDSP_PDCH_TO_PICH_GAIN_RATIO
}   muxmdsp_dvchannel_rl_gainratio_type;

/* enum type for different linear range selection in TX_RATE_ADJ */
typedef enum
{
   MUXMDSP_TXRADJ_RANGE_1PER12DB = 0,
   MUXMDSP_TXRADJ_RANGE_1PER10DB = 1
} muxmdsp_txradj_range_type;

/* enum type for Override Control */
typedef  enum
{
  MUXMDSP_PDCCH_OVERRIDE,
  MUXMDSP_WALSH_MASK_OVERRIDE,
  MUXMDSP_PDCH_OVERRIDE,
  MUXMDSP_CQICH_OVERRIDE,
  MUXMDSP_RL_GAIN_OVERRIDE,
  MUXMDSP_ACKCH_OVERRIDE,
  MUXMDSP_ALL_FIELD_OVERRIDE
} muxmdsp_override_ctrl_type;

/* enum type for Reverlink Gain Override */
enum
{
  MUXMDSP_RLPICH_GAIN_OVERRIDE,
  MUXMDSP_RLFCH_GAIN_OVERRIDE,
  MUXMDSP_RLDCCH_GAIN_OVERRIDE,
  MUXMDSP_RLSCH_GAIN_OVERRIDE,
  MUXMDSP_RLACKCH_GAIN_OVERRIDE,
  MUXMDSP_RLCQICH_GAIN_OVERRIDE,
  MUXMDSP_RLPDCH_GAIN_OVERRIDE,
  MUXMDSP_RLPDCCH_GAIN_OVERRIDE,
  MUXMDSP_RLREQCH_GAIN_OVERRIDE,
  MUXMDSP_RLSPICH_GAIN_OVERRIDE,
  MUXMDSP_RLTXRATEADJ_GAIN_OVERRIDE,
  MUXMDSP_TOTAL_RLGAIN_OVERRIDE
};

/* Data structure for Override data storage*/
typedef struct
{
   uint16   overridebitsrequest;  /* requested override bits*/
   uint16 overridebitsenable;   /* the current override enable bits */
   uint32 pdcch0OvrBuff[16];      /* PDCCH override data buffer 0 */
   uint32 pdcch1OvrBuff[16];      /* PDCCH override data buffer 1*/
   uint32 pdchReg0Buff[16];     /* PDCH override data buffer */
   uint32 ackchOvrRegBuff[16];  /* ACKCH override data buffer */
   uint32 cqiOvrRegBuff[16];      /* CQICH override data buffer */
   uint32 pichOvrRegBuff[16];     /* PICH override data buffer */
   uint16 cqiOvrGainPattern;      /* CQICH override gain pattern */
   uint32 rlDVchanGain[16][MUXMDSP_TOTAL_RLGAIN_OVERRIDE];/* DV channels override RL gain  */
} muxmdsp_override_data_info_type;

/* MDSP to ARM message type, this should be consistent with what defined in MDSP */
typedef enum
{
   MUXMDSP_CELL_SWITCH_TERMINATION_MSG,
   MUXMDSP_EXIT_CONTROL_HOLD_MSG,
   MUXMDSP_PDCH_TRAFFIC_CHANNEL_MSG,
   MUXMDSP_PILOT_DISABLE_MSG,
   MUXMDSP_IGNORE_MSG,
   MUXMDSP_TOTAL_MSG
} muxmdsp_message_enum_type;

/* switch termination type */
typedef enum
{
   MUXMDSP_SWITCH_TER_NORMAL,
   MUXMDSP_SWITCH_TER_EARLY,
   MUXMDSP_SWITCH_TER_ABORT,
   MUXMDSP_SWITCH_TER_OVERRIDE
} muxmdsp_switch_termination_type;

/* message switch termination data structure */
typedef struct
{
   muxmdsp_switch_termination_type  term_type;
   uint16                           new_sec_pn_offset; /*bit 0 to 8 */
} muxmdsp_switch_termination_msg_type;

/* Exit control hold type */
typedef enum
{
   MUXMDSP_EXITCHD_MAC_MSG,
   MUXMDSP_EXITCHD_PDCH_DECODE
} muxmdsp_exit_control_hold_type;

/* message exit control type */
typedef struct
{
   muxmdsp_exit_control_hold_type   exitchd_type;
   uint16                           word02;
} muxmdsp_exit_control_hold_msg_type;

/* PDCH traffic type */
typedef enum
{
   MUXMDSP_PDCH_TRAFFIC_INIT,
   MUXMDSP_PDCH_TRAFFIC_TER
} muxmdsp_pdch_traffic_type;

/* message PDCH traffic type */
typedef struct
{
   muxmdsp_pdch_traffic_type    traffic_type;
   uint16                       word02;
} muxmdsp_pdch_traffic_msg_type;

/* message PILOT DISABLE  type */
typedef struct
{
   uint16              walsh_cover_id;/*bit0->2, active set pilot to be dropped */
   uint16              word02;
} muxmdsp_pilot_disable_msg_type;

/* IGNORE traffic type */
typedef enum
{
   MUXMDSP_IGNORE_ABORT,
   MUXMDSP_IGNORE_CONTINUE
} muxmdsp_ignore_type;

/* message IGNORE  type */
typedef struct
{
   muxmdsp_ignore_type              ignore_type;
   uint16                           word02;
} muxmdsp_ignore_msg_type;

/* MDSP to ARM message structure */
typedef struct
{
   muxmdsp_message_enum_type              msg_type;
   uint8                                  msg_time; /* in units of slots up to 80ms*/
   union
   {
      muxmdsp_pdch_traffic_msg_type       pdch_traffic;
      muxmdsp_switch_termination_msg_type switch_ter;
      muxmdsp_exit_control_hold_msg_type  exit_ctlr_hold;
      muxmdsp_pilot_disable_msg_type      pilot_disable;
      muxmdsp_ignore_msg_type             ignore;
   } msg_body;
} muxmdsp_message_data_type;

/* define all the client IDs for the mDSP message */
typedef enum
{
  MUXMDSP_SRCH_CLIENT,
  MUXMDSP_MC_CLIENT,
  MUXMDSP_MUX_CLIENT,
  MUXMDSP_MAX_SERVING_CLIENT
} muxmdsp_msg_client_id_enum_type;

/* function prototype for client callback function */
typedef void (*muxmdsp_msg_client_func_type) (muxmdsp_message_data_type *);


/* DME data structure */
typedef struct
{
   uint16 log_enable_bits;  /* the current enable log bits for this slot */
   uint16 data_size;          /* the number of valid data since the last read or get size */
   uint16 dmebufferlength;  /* the maximum length of the DME buffer */
   uint16 *dmebaseaddress;  /* DME base address pointer*/
   uint16 *dmewrtptr;   /* DME write pointer, maintain by the DSP */
   uint16 *dmerdptr;    /* DMS read pointer, maintain by MUX */
} muxmdsp_dme_info_type;


/* enum type for Override Control */
typedef  enum
{
  MUXMDSP_PDCCH_LO_LOG,
  MUXMDSP_PDCCH_HI_LOG,
  MUXMDSP_WALSH_LOG,
  MUXMDSP_PDCH_BE_LOG,
  MUXMDSP_PDCH_FE_LOG,
  MUXMDSP_CQIACK_LO_LOG,
  MUXMDSP_CQIACK_HI_LOG,
  MUXMDSP_RL_GAIN_LOG,
  MUXMDSP_SNDA_LOG,
  MUXMDSP_ALL_FIELD_LOG
} muxmdsp_log_ctrl_type;

/* define enum type for PDCH ACK statistics counters */
typedef enum
{
   MUXMDSP_PDCH0_ACK1X_SC,
   MUXMDSP_PDCH0_ACK2X_SC,
   MUXMDSP_PDCH0_ACK4X_SC,
   MUXMDSP_PDCH0_ACK6X_SC,
   MUXMDSP_PDCH0_ACK8X_SC,
   MUXMDSP_PDCH0_ACK10X_SC,
   MUXMDSP_PDCH0_ACK0_5X_SC,
   MUXMDSP_PDCH1_ACK1X_SC,
   MUXMDSP_PDCH1_ACK2X_SC,
   MUXMDSP_PDCH1_ACK4X_SC,
   MUXMDSP_PDCH1_ACK6X_SC,
   MUXMDSP_PDCH1_ACK8X_SC,
   MUXMDSP_PDCH1_ACK10X_SC,
   MUXMDSP_PDCH1_ACK0_5X_SC
} muxmdsp_pdch_ack_stat_counter_type;

/* define enum type for PDCH NAK statistics counters */
typedef enum
{
   MUXMDSP_PDCH0_NAK1X_SC,
   MUXMDSP_PDCH0_NAK2X_SC,
   MUXMDSP_PDCH0_NAK4X_SC,
   MUXMDSP_PDCH0_NAK6X_SC,
   MUXMDSP_PDCH0_NAK8X_SC,
   MUXMDSP_PDCH0_NAK10X_SC,
   MUXMDSP_PDCH0_NAK0_5X_SC,
   MUXMDSP_PDCH1_NAK1X_SC,
   MUXMDSP_PDCH1_NAK2X_SC,
   MUXMDSP_PDCH1_NAK4X_SC,
   MUXMDSP_PDCH1_NAK6X_SC,
   MUXMDSP_PDCH1_NAK8X_SC,
   MUXMDSP_PDCH1_NAK10X_SC,
   MUXMDSP_PDCH1_NAK0_5X_SC
} muxmdsp_pdch_nak_stat_counter_type;

typedef enum
{
  MUXMDSP_CELL_SWITCH_INIT,
  MUXMDSP_CELL_SWITCH_START,
  MUXMDSP_CELL_SWITCH_CONTINOUS
} muxmdsp_dv_cell_switch_type;

// R-PDCH modulation mode
typedef enum
{
  MUXMDSP_RLGAIN_PDCH_MODE_BPSK = 0,
  MUXMDSP_RLGAIN_PDCH_MODE_QPSK_W4,
  MUXMDSP_RLGAIN_PDCH_MODE_QPSK_W2,
  MUXMDSP_RLGAIN_PDCH_MODE_QPSK_W4_W2,
  MUXMDSP_RLGAIN_PDCH_MODE_8PSK
} muxmdsp_dvchannel_rl_gain_pdch_mode_type;

/* type define structure for all the DV parameters */
typedef struct
{
  boolean                       pdch_on;
  cai_radio_config_type         pdch_rc;
  uint16                        fundframeoffset;
  uint16                        pilotpreambledelay;
  muxmdsp_pdch_ctrl_type        pdch_ctrl_info;
  muxmdsp_pdcch_ctrl_type       pdcch_ctrl_info[MUXMDSP_TOTAL_SEC_REF];
  muxmdsp_cqich_ctrl_info_type  cqich_ctrl_info;
  muxmdsp_cqich_switch_cmd_type cqich_switch_cmd_info;
  uint16                        ackch_reps;
} muxmdsp_dv_info_type;

/* DV global control info data structure */
extern muxmdsp_dv_info_type muxmdsp_dv_info;
/* override data structure */
extern muxmdsp_override_data_info_type  muxmdsp_override_data_info;

typedef enum
{
  MUXMDSP_OB_VALID_PKTS_ONLY=0,
  MUXMDSP_OB_ALL_PKTS=1        /* debug mode */
} muxmdsp_ob_mode_enum_type;

typedef struct
{
  muxmdsp_ob_mode_enum_type ob_mode;
  uint8                     bit_shift;
} muxmdsp_ob_ctrl_info_type;

typedef enum
{
  MDSP2ARM_INT_DECOB_WATERMARK_CLT,
  MDSP2ARM_INT_DECOB_STALETIME_CLT,
  MDSP2ARM_INT_MDSPMGS_CLT,
  MDSP2ARM_INT_SRCHDONE_CLT,
  MDSP2ARM_INT_MAX_CLT
} mdsp2arm_int_clt_enum_type;

/* function prototype for MDSP to ARM INT client callback function */
typedef void (*mdsp2arm_int_client_func_type) (void);

/*===========================================================================

FUNCTION muxmdsp_wr_mod_pch_gain

DESCRIPTION
  Write the MOD_PCH_GAIN to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_mod_pch_gain (
  word pch_gain);      /* 8:0 of MOD_PCH_GAIN register is rev. pilot ch gain */

/*===========================================================================

FUNCTION muxmdsp_wr_mod_fch_gain

DESCRIPTION
  Write the MOD_FCH_GAIN to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_mod_fch_gain (
  word fch_gain);      /* 8:0 of MOD_FCH_GAIN register is rev. fundamental
                          ch gain */


/*===========================================================================

FUNCTION muxmdsp_wr_mod_dcch_gain

DESCRIPTION
  Write the MOD_DCCH_GAIN to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_mod_dcch_gain (
  word dcch_gain);      /* 8:0 of MOD_DCCH_GAIN register is rev. dedicated
                          control ch gain */



/*===========================================================================

FUNCTION muxmdsp_wr_mod_sch_gain

DESCRIPTION
  Write the MOD_SCH_GAIN to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_mod_sch_gain (
  word sch_gain);        /* 8:0 of MOD_SCH_GAIN register is rev. supplemental
                          ch gain */


/*===========================================================================

FUNCTION muxmdsp_wr_mod_prmbl_gain

DESCRIPTION
  Write the MOD_PRMBL_GAIN to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_mod_prmbl_gain (
  word prmbl_gain);      /* 8:0 of MOD_PRMBL_GAIN register is preamble gain */

/*===========================================================================

FUNCTION muxmdsp_wr_tx_rate_adj

DESCRIPTION
  Write the TX_RATE_ADJ to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_tx_rate_adj (
  word tx_rate_adj);

/*===========================================================================

FUNCTION muxmdsp_wr_tx_rate_adj_latch_offset

DESCRIPTION
  The function updates offset parameter which spcecifies the number of chips before
  the fundicated frame boundary that the modulator HW latches TX_RATE_ADJ

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_tx_rate_adj_latch_offset (
  word tx_rate_adj_latch_offset);

/*===========================================================================

FUNCTION MUXMDSP_WR_FUNDFRAMEOFFSET

DESCRIPTION
  This funtion sets the fundicated frame offset to the mDSP

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_fundFrameOffset (uint16  fundoffset_value); /* value range from 0 to 15 PCG*/

/*===========================================================================

FUNCTION MUXMDSP_WR_FUNDFRAMEOFFSETIMMLOADCMD

DESCRIPTION
  This funtion indicates to the mDSP if the frame offset is load immediately

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_fundFrameOffsetImmLoadCmd (boolean load_imm);

/*===========================================================================

FUNCTION MUXMDSP_WR_PASSTHRUENABLE

DESCRIPTION
  This funtion sets (TRUE) or clears (FALSE) the mDSP digital gain pass
  through functionality.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_passThruEnable(boolean enable); /* TRUE turns on pass-through */

/*===========================================================================

FUNCTION muxmdsp_cdma_init

DESCRIPTION
  This function is called by MUX to initialize DV control parameters to mDSP

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_cdma_init(void);

#endif /* MUXMDSP_H */

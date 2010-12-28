#ifndef TXCMC_H
#define TXCMC_H
/*===========================================================================

          T R A N S M I T   T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the transmit task that don't fit
  into one of the other H files such as CAI.H

Copyright (c) 2005 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath:  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/txcmc.h#2 $ $DateTime: 2008/07/18 10:34:36 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV, FEATURE_IS2000_REL_D_DV, 
                   and FEATURE_DELAYED_TX_CLOCK_SHUTDOWN. Removed unused 
                   function prototypes.
12/04/06   trc     MSM7600 support
10/30/06   bn      Added code to support DO/UMTS to 1x handoff
08/02/06   trc     Mainline FEATURE_WINCE support
10/17/05   rkc     Remove prototype for wrapper function txc_enc_set_frame_off().
10/12/05   rkc     Add prototype for wrapper function txc_enc_set_frame_off().
09/15/05   rkc     Add boolean hold_enc_clk_on to txc_jump_cmd_type.
07/08/05   bn      Added new interface function to get RLGC parameters for MC 
05/23/05   vlc     Changed FEATURE_IS2000_REL_D_JUMP to FEATURE_RL_JUMP_ENHANCEMENT.
                   Allow function txc_ok_to_sleep() to be visible for 
                   FEATURE_RL_JUMP_ENHANCEMENT. 
                   Added new parameter required in Jump to hyperspace to 
                   txc_jump_cmd_type under FEATURE_RL_JUMP_ENHANCEMENT.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
04/13/05   bn      First Revision

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "qw.h"
#include "cmd.h"
#include "cai.h"
#include "enc.h"
#include "caii.h"  //IS2000 change
#include "mctrans.h"
#include "ffpc.h"

#ifdef FEATURE_TX_POWER_BACK_OFF
#include "rfnv.h"
#endif /* FEATURE_TX_POWER_BACK_OFF */
/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Possible status value for a command */
typedef enum {
  TXC_DONE_S,                      /* Generic OK return */
  TXC_BUSY_S,                      /* Command is being processed */
  TXC_BADCMD_S,                    /* Invalid command */
  TXC_ERR_S,                       /* Error attempting to do something */
  TXC_MAXIMUM_S                    /* always last, this counts statuses */
} txc_status_type;

typedef enum {
  TXC_CDMA_F,                      /* Switch to CDMA mode */
  TXC_PC_F,                        /* Move to Paging Channel */
  TXC_TC_F,                        /* Move to Traffic Channel */
  TXC_EXIT_F,                      /* Exit CDMA operation */
  TXC_MSG_F,                       /* Send a message */
  TXC_ACCESS_F,                    /* Make an access attempt */
  TXC_ACC_STOP_F,                  /* Stop access attempts immediately */
  TXC_OFF_TX_F,                    /* Stop transmitting */
  TXC_ON_TX_F,                     /* Start transmitting */
  TXC_NULLS_F,                     /* Transmit null data on TC */
  TXC_VOC_F,                       /* Wait for voc initialization */
  TXC_SO_F,                        /* Switch service options */
  TXC_RDR_F,                       /* Reverse data rate limit parameters */
  TXC_FRAME_OFFSET_F,              /* Set frame offset */
#if defined (FEATURE_IS95B_PILOT_RPT)
  TXC_ACC_PROBE_F,                 /* Transmit access probe */
#endif
#ifdef FEATURE_IS95B_ACCESS_PROBE_HO
  TXC_ACC_SUSPEND_F,
#endif /* FEATURE_IS95B_ACCESS_PROBE_HO */
  TXC_PWR_CTRL_F,                 /* Pass along new power ctrl parameters */
  TXC_CONNECT_SO_F,               /* Indicates we are in CONV substate,
                                     Connect stored SO with vocoder if applicable */
  TXC_HO_F,                       /* HO command */
  TXC_HHO_SUCCESS_F,              /* Indicates that a HHO was successful, Can program
                                     necessary parameters */
  TXC_RETURNED_F,                 /* Indicates a return back to SF after a failed HHO,
                                     Reprogram necessary params back to old values */
#ifdef FEATURE_IS2000_REL_A_SVD  
  TXC_RELEASE_SO_F,               /* release the service option */
#endif /* FEATURE_IS2000_REL_A_SVD */
  TXC_RELEASE_F,                  /* Sent after a release, Equivalent to old command of
                                     sending a NULL SO command */
  TXC_MC_TRANS_F,                 /* Transaction from MC */

#ifdef FEATURE_PLT
#error code not present
#endif // FEATURE_PLT

#ifdef FEATURE_FACTORY_TESTMODE
  TXC_FTM_SCH_F,                  /* Command to start SCH */
  TXC_FTM_FCH_LOOPBACK,           /* Start FCH loopback */
#ifdef FEATURE_IS2000_P2
  /* Support FEATURE_FTM_DCCH */
  TXC_FTM_DCCH_LOOPBACK,           /* Start DCCH loopback */
  /* End of FEATURE_FTM_DCCH */
#endif
  TXC_FTM_SCH_LOOPBACK,           /* Start SCH loopback */
#endif

#ifdef FEATURE_IS2000_REL_A_CC
//txc_reach_on
  TXC_REACH_F,                    /* Make an access attempt */
  TXC_REACH_PROBE_F,              /* Change R-EACH probe parameters */
//txc_reach_on
#endif
  TXC_JUMP_F,                     /* Jump to Hyperspace */
#if defined(FEATURE_VOIP_1X_HANDOFF) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
  TXC_TT_F,                       /* go the timing transfer state*/
#endif /* FEATURE_VOIP_1X_HANDOFF and FEATURE_UMTS_1X_HANDOVER_1XMSM */
  TXC_COMMAND_COUNT                /* End of the enum */
} txc_cmd_name_type;

/* The common Header for all the commands */
typedef struct {
  cmd_hdr_type         cmd_hdr;        /* command header */
  txc_cmd_name_type    command;        /* The command */
  txc_status_type       status;        /* Space for message status */
} txc_hdr_type;

typedef struct {                  /* TXC_CDMA_F */
  txc_hdr_type   hdr;
} txc_cdma_cmd_type;

typedef struct {                  /* TXC_PC_F */
  txc_hdr_type   hdr;
} txc_pc_cmd_type;

#if defined(FEATURE_VOIP_1X_HANDOFF) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
typedef struct {                  /* TXC_TT_F */
  txc_hdr_type   hdr;
} txc_tt_cmd_type;
#endif /* FEATURE_VOIP_1X_HANDOFF and FEATURE_UMTS_1X_HANDOVER_1XMSM */
typedef struct {                  /* TXC_TC_F */
  txc_hdr_type   hdr;
  uint16         sigs;       /* Signal to set when TX task has acquired
                              * timing and moved to Traffic state */
  rex_tcb_type  *tcb_ptr;         /* Pointer to task to signal */
  qword          mask;            /* Long mask */

  /* Note that the rev_fch.included or rev_dcch.included flags above may
   * be set to TRUE without the corresponding forward channels in RXC_TCI_F
   * being TRUE. */
  chan_cfg_type  rev_fch;
  chan_cfg_type  rev_dcch;

  uint8 frame_offset;  /* Fundicated channel frame offset. */

  uint8 rl_gain_adj;   /*In 8-bit 2's compliment, units of 1db*/
  uint8 rlgain_traffic_pilot; /* In 8-bit 2's compliment */
  boolean rev_fch_gating_on; /* Indicates if 8th rate gating is on 
                               * or off on R_FCH */
  uint16 signaling_phy_channel; /* physical channel(s) for signaling */
#ifdef FEATURE_FACTORY_TESTMODE
  enc_rate_type     fch_rate;
#ifdef FEATURE_IS2000_P2
  /* Support FEATURE_FTM_DCCH */
  enc_rate_type     dcch_rate;
  /* End of FEATURE_FTM_DCCH */
#endif
  boolean           sch_on;
  cai_radio_config_type rev_sch_rc;
  boolean           sch_turbo;
  uint16             sch_ltu_size;
  enc_sch_rate_type sch_rate;
  uint16             num_preambles;
  uint32             tx_pattern;
#endif // FEATURE_FACTORY_TESTMODE

#ifdef FEATURE_IS2000_REL_A
  uint8 p_rev_in_use; /* Needed for frame formatting */
#endif  // Do we still need this?

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif

#if defined(FEATURE_VOIP_1X_HANDOFF) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
  boolean turn_on_transmitter;
#endif /* FEATURE_VOIP_1X_HANDOFF and FEATURE_UMTS_1X_HANDOVER_1XMSM */
} txc_tc_cmd_type;

#ifdef FEATURE_FACTORY_TESTMODE
typedef struct {
  txc_hdr_type      hdr;
  cai_radio_config_type radio_cfg;
  boolean           sch_on;
  enc_sch_rate_type sch_rate;
  boolean           sch_turbo;
  dword             tx_pattern;
} txc_ftm_sch_cmd_type;

typedef struct {
  txc_hdr_type      hdr;
  boolean           loopback_on;
} txc_ftm_loopback_cmd_type;

#endif // FEATURE_FACTORY_TESTMODE

/* Signaling mode for txc_tc_frame_type */
typedef enum
{
  NORM  = 0,                         /* normal mode */
  DIM   = 1,                         /* dim and burst frame */
  BLANK = 2                          /* blank and burst frame */
} txc_sig_frame_type;


typedef struct {                  /* TXC_EXIT_F */
  txc_hdr_type   hdr;
} txc_exit_cmd_type;

#ifdef FEATURE_IS2000_REL_B
typedef enum {
  CONFIRM_MSG_SENT,     /* rxtx wants the confirmation when the msg is sent */
  CONFIRM_MSG_BUILT     /* rxtx wants the confirmation when the msg is built */
} txc_confirmation_type;
#endif /* FEATURE_IS2000_REL_B */

typedef struct {                  /* TXC_MSG_F */
  txc_hdr_type   hdr;
#ifdef FEATURE_IS2000_REL_B
  txc_confirmation_type confirmation_type;  /* indicates when rxtx is  
                           expecting to receive the 
                           confirmation. */
#endif /* FEATURE_IS2000_REL_B */
  word          len;              /* number of bits in message */
  byte          *msg_ptr;         /* the message to send */
} txc_msg_cmd_type;

typedef struct {                  /* TXC_ACCESS_F */
  txc_hdr_type  hdr;
  word          len;              /* Number of bits in message */
  boolean       req;              /* True if access request */
  byte          seq_num;          /* Max number of access probe sequences */
  byte          pagech;           /* Current Paging Channel */
  word          base_id;          /* Base station identification */
  word          pilot_pn;         /* Pilot PN sequence offset index */
  byte          pam_sz;           /* Access Channel preamble length */
  byte          max_cap_sz;       /* Max Access Channel msg capsule size */
  byte          bkoff;            /* Probe sequence backoff range */
  byte          probe_bkoff;      /* Probe backoff range */
  byte          acc_tmo;          /* Acknowledgement timeout */
  byte          init_pwr;         /* Initial power offset for access */
  byte          nom_pwr;          /* Nominal transmit power offset */
  byte          pwr_step;         /* Power increment */
  byte          num_step;         /* Number of access probes */
  byte          acc_chan;         /* Number of access channels */
  boolean       use_chan;         /* Test feature to use specific acc chan */
  word          rn;               /* Random number of chips backoff - ignored now */
  dword         p;                /* Persistence probability */
  byte          *msg_ptr;         /* Access Channel message to send */
  word          sigs;             /* Signal to set when access attempt is
                                     done */
  rex_tcb_type  *tcb_ptr;         /* Pointer to task to signal when access
                                     attempt is done */
} txc_acc_cmd_type;


#ifdef FEATURE_IS2000_REL_A_CC
//txc_reach_on

typedef enum
{
  TXC_EACH_RATE_9600_20_V = 0,         /* 9600 bps,  20 ms frame */
  TXC_EACH_RATE_19200_20_V,            /* 19200 bps, 20 ms frame */
  TXC_EACH_RATE_19200_10_V,            /* 19200 bps, 10 ms frame */
  TXC_EACH_RATE_38400_20_V,            /* 38400 bps, 20 ms frame */
  TXC_EACH_RATE_38400_10_V,            /* 38400 bps, 10 ms frame */
  TXC_EACH_RATE_38400_5_V              /* 38400 bps,  5 ms frame */
} txc_each_rate_type;

typedef struct {                  /* TXC_ACCESS_F */
  txc_hdr_type  hdr;
  word          len;              /* Number of bits in message */
  boolean       req;              /* True if access request */
  byte          seq_num;          /* Max number of access probe sequences */
  //byte          pagech;           /* Current Paging Channel */
  byte          fccch;            /* Current Paging Channel */
  word          base_id;          /* Base station identification */
  word          pilot_pn;         /* Pilot PN sequence offset index */
  //byte          pam_sz;           /* Access Channel preamble length */
  //byte          max_cap_sz;       /* Max Access Channel msg capsule size */
  byte          bkoff;            /* Probe sequence backoff range */
  byte          probe_bkoff;      /* Probe backoff range */
  byte          acc_tmo;          /* Acknowledgement timeout */
  byte          init_pwr;         /* Initial power offset for access */
  byte          nom_pwr;          /* Nominal transmit power offset */
  byte          pwr_step;         /* Power increment */
  byte          num_step;         /* Number of access probes */
  //byte          acc_chan;         /* Number of access channels */
  //boolean       use_chan;         /* Test feature to use specific acc chan */
  
  byte          num_each_ba;        /* Number of enhanced access channels */

  txc_each_rate_type   rate;

  boolean       preamble_enabled;
  

  //word          rn;               /* Random number of chips backoff - ignored now */
  dword         p;                /* Persistence probability */

  byte          slot;
  byte          slot_offset1;
  byte          slot_offset2;

  //word          preamble_length;

  byte          preamble_num_frac;
  byte          preamble_frac_duration;
  byte          preamble_off_duration;
  byte          preamble_add_duration;

  byte rlgain_common_pilot;   /* Gain adjustment of EACH or RCCCH relative to reverse pilot */
  byte ic_thresh;             /* interference correction threshhold */
  byte ic_max;                /* maximum interference correction that can be applied */
  
  //word          frame_size;
  //word          data_rate;

  byte          *msg_ptr;         /* Access Channel message to send */
  word          sigs;             /* Signal to set when access attempt is
                                     done */
  rex_tcb_type  *tcb_ptr;         /* Pointer to task to signal when access
                                     attempt is done */
} txc_reach_cmd_type;

//txc_reach_on
#endif // FEATURE_IS2000_REL_A_CC


typedef struct {                  /* TXC_ACC_STOP_F  */
  txc_hdr_type  hdr;
  word          sigs;             /* Signal to set when access attempt is
                                     done */
  rex_tcb_type  *tcb_ptr;         /* Pointer to task to signal when access
                                     attempt is done */
} txc_stop_cmd_type;

typedef struct {                  /* TXC_ON_TX_F */
  txc_hdr_type   hdr;
} txc_on_cmd_type;

typedef struct {                  /* TXC_OFF_TX_F */
  txc_hdr_type   hdr;
} txc_off_cmd_type;

typedef struct {                  /* TXC_NULLS_F */
  txc_hdr_type   hdr;
} txc_nulls_cmd_type;


#ifndef FEATURE_IS2000_REL_A
typedef struct {
   boolean change_pri;
   word pri;
   boolean change_sec;
   word sec;
   boolean connect_so;             /* Indicates if this SO can be connected with the vocoder
                                      1 if we are in CONV state already; 0 otherwise */
  struct
  {
    boolean included;               /* Indicates if channel is included */
    cai_radio_config_type rc;       /* RC of channel */
  }
  #ifdef FEATURE_IS2000_P2
  rev_fch, rev_dcch;
  #else
  rev_fch;                          /* When P2 not defined, used to indicate if RC for R-FCH is included */
  #endif  // FEATURE_IS2000_P2

} txc_scr_info_type;

#else

typedef struct {
  uint8 num_of_so_recs;

  struct
  {
    uint16  so;                 // SO for this connection
    boolean connect_so;         // Indicates if this SO can be connected with the vocoder
                                // 1 if we are in CONV state already; 0 otherwise
    byte    sr_id;
    byte    traffic_type;       // CAI_PRI_TRAF or CAI_SEC_TRAF
    word    phy_channel;        // combined Bit mask of physical channels associated with SO
  } so_cfgs[MAX_NO_OF_SO_RECS];

  word signaling_phy_channel;  //Bit mask of signaling physical channel
  struct
  {
    boolean included;               /* Indicates if channel is included */
    cai_radio_config_type rc;       /* RC of channel */
  }
  #ifdef FEATURE_IS2000_P2
  rev_fch, rev_dcch;
  #else
  rev_fch;                          /* When P2 not defined, used to indicate if RC for R-FCH is included */
  #endif  // FEATURE_IS2000_P2

} txc_scr_info_type;
#endif /* FEATURE_IS2000_REL_A */

typedef struct {                  /* TXC_SO_F */
  txc_hdr_type   hdr;
  qword action_time;              /* Action time when the new params take effect */
  txc_scr_info_type scr_info;     /* The new service config info */
  byte transaction_num;           /* transaction number for this MC-TXC transaction*/
} txc_so_cmd_type;

typedef struct {                  /* TXC_RDR_F */
  txc_hdr_type hdr;
  byte rdr_limit;                 /* Reverse data rate limit */
  byte rdr_ena_thresh;            /* Reverse data rate limit enable
                                     threshold */
  byte rdr_dis_thresh;            /* Reverse data rate limit disable
                                     threshold */
} txc_rdr_cmd_type;

typedef struct {                  /* TXC_FRAME_OFFSET_F */
  txc_hdr_type hdr;
  byte frame_offset;              /* Traffic Channel frame offset */
  qword action_frame;             /* Frame when frame offset takes effect */
} txc_frame_offset_cmd_type;


typedef struct {
  txc_hdr_type hdr;
  qword action_time;
  byte transaction_num; /* Transaction num of this command */
  byte pwr_ctrl_step; /* power control step size */

  boolean rpc_incl; /* Indicates if RPC records are present */
  byte num_of_rpc_records; /* Number of RPC records */
  caii_rpc_type rpc_records[CAI_MAX_NUM_RPC_RECS]; /* RPC records */
} txc_pwr_ctrl_cmd_type;

typedef struct {                  /* TXC_CONNECT_SO_F */
  txc_hdr_type   hdr;
#ifdef FEATURE_IS2000_REL_A
  byte   sr_id;                /* so service reference ID */
#endif /* FEATURE_IS2000_REL_A */
} txc_conn_so_cmd_type;

#ifdef FEATURE_IS2000_REL_A_SVD
typedef struct {                  /* TXC_RELEASE_SO_F */
  txc_hdr_type   hdr;
  byte   sr_id;                /* so service reference ID */
} txc_release_so_cmd_type;
#endif /* FEATURE_IS2000_REL_A_SVD */

typedef struct {
  txc_hdr_type hdr;
  byte transaction_num; /* Transaction num to validate the trans */
  qword action_time; /* Action time of this transaction */

  boolean hho_ind; /* HHO Indication */
  boolean rif_ind; /* Return on Failure Indication */

  boolean extra_params; /* Extra params indication */
  byte frame_offset; /* Valid only if extra_params is 1, Change FO only if different */
  byte nom_pwr; /* Valid only if extra_params is 1 */
  byte num_preamble; /* Valid only if extra_params is 1 */
  boolean lcm_change_ind; /* Valid only if extra_params is 1 */
  qword lcm;
  boolean rpc_incl;  /* Valid only if extra_params is 1 */
  int8 rlgain_traffic_pilot; /* Valid only if rpc_incl is 1 */
  boolean default_rlag; /* Valid only if rpc_incl is 1 */

  boolean pwr_ctrl_step_incl;  /* Pwr control step size is included in this cmd */
  byte pwr_ctrl_step;  /* pwr ctrl step size valid only if above flag is 1 */

  boolean scr_info_incl;  /* SCR info incl indicator */
  txc_scr_info_type scr_info;  /* SCR info valid only if above is true */

} txc_ho_cmd_type;

typedef struct {
  txc_hdr_type hdr;
} txc_hho_ok_cmd_type;

typedef struct {
  txc_hdr_type hdr;
} txc_rel_cmd_type;

typedef struct {
  txc_hdr_type hdr;
  boolean frame_offset_incl; /* FO included indicator */
  byte frame_offset;  /* Frame offset , valid only if above is TRUE */
  boolean pwr_ctrl_step_incl;  /* Pwr control step size is included in this cmd */
  byte pwr_ctrl_step;  /* pwr ctrl step size valid only if above flag is 1 */
  boolean scr_incl; /* SCR included indicator */
  txc_scr_info_type scr_info; /* SCR info valid only if above is true*/
  boolean lcm_change_ind; /* LCM change indicator */
  qword lcm;  /* Valid only if above is TRUE */
  boolean rev_fch_gating_on; /* Indicates if 8th rate gating is on 
                                     or off on R_FCH */
#ifdef FEATURE_IS2000_REL_A
  byte p_rev_in_use; /* Needed for frame formatting */
#endif
} txc_returned_cmd_type;

typedef struct {
  txc_hdr_type hdr;
  enc_gating_mode_type gating_mode;
  word gating_pattern;
  byte num_frames_gating_on;
  byte num_frames_gating_off;
} txc_pilot_gating_cmd_type;

typedef struct {
  txc_hdr_type hdr;
  word          rn;                /* Random number of chips backoff */
  boolean       hold_enc_clk_on;   /* Hold encoder clock on, e.g., enhance call setup */

} txc_jump_cmd_type;               /* TXC_JUMP_F - Jump to Hyperspace */


typedef union {
  txc_hdr_type       hdr;          /* Generic header */
  txc_cdma_cmd_type  cdma;         /* CDMA init */
  txc_pc_cmd_type    pc;           /* page channel */
#if defined(FEATURE_VOIP_1X_HANDOFF) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
  txc_tt_cmd_type    tt;           /* Time transfer*/
#endif /* FEATURE_VOIP_1X_HANDOFF and FEATURE_UMTS_1X_HANDOVER_1XMSM */
  txc_tc_cmd_type    tc;           /* traffic channel */
  txc_exit_cmd_type  exit;         /* exit cdma  */
  txc_msg_cmd_type   msg;          /* message to transmit */
  txc_acc_cmd_type   acc;          /* access attempt */
  txc_stop_cmd_type  stop;         /* stop access attempt and TX clock */
  txc_on_cmd_type    on;           /* start transmitting */
  txc_off_cmd_type   off;          /* stop transmitting */
  txc_nulls_cmd_type nulls;        /* transmit null traffic data */
  txc_so_cmd_type    so;           /* switch service options */
  txc_rdr_cmd_type   rdr;          /* reverse data rate limit parameters */
  txc_frame_offset_cmd_type fo;    /* Set frame offset                   */
  txc_pwr_ctrl_cmd_type pwr_ctrl;        /* Change Power Control Parameters */
  txc_conn_so_cmd_type conn_so;    /* Connect SO with vocoder if necessary */
#ifdef FEATURE_IS2000_REL_A_SVD
  txc_release_so_cmd_type  release_so; /* release service option */
#endif /* FEATURE_IS2000_REL_A_SVD */
  txc_ho_cmd_type ho;              /* Ho command */
  txc_hho_ok_cmd_type hho_ok;      /* HHO Okay Command*/
  txc_returned_cmd_type ret;       /* HHO returned command */
  txc_rel_cmd_type      rel;       /* Equivalent to sending a SO cmd with null SO */
#if !defined(FEATURE_TRANS_MANAGER) && !defined(FEATURE_WINCE) && defined(FEATURE_CDMA)
  txc_mc_trans_cmd_type mc_trans;  /* Transaction command from MC */
#endif /* !FEATURE_TRANS_MANAGER && !defined(FEATURE_WINCE) */

  #ifdef FEATURE_PLT
#error code not present
  #endif

  #ifdef FEATURE_FACTORY_TESTMODE
  txc_ftm_sch_cmd_type  ftm_sch;   /* FTM turn on SCH command */
  txc_ftm_loopback_cmd_type loopback; /* FTM loopback command */
  #endif

#ifdef FEATURE_IS2000_REL_A_CC
//txc_reach_on
  txc_reach_cmd_type   reach;          /* access attempt */
#endif // FEATURE_IS2000_REL_A_CC

   txc_jump_cmd_type jump;          /* TXC_JUMP_F - Jump to Hyperspace */

} txc_cmd_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION TXC_SUSPEND_TX

DESCRIPTION
  This functions enables/Disables the Transmitter and enables/disables the
  transmission of Access probes.

DEPENDENCIES
  Depends on the current state of the TX task.

RETURN VALUE
  None.

SIDE EFFECTS
  This function is called when the T72 fade timer expires to temporarily
  enable/disable the transmission of the Access Probes.

===========================================================================*/
void txc_suspend_tx (boolean);    /* Enable/Disable probe transmission */

/*===========================================================================

FUNCTION TXC_POWERUP_INIT

DESCRIPTION
  Initializes timers and queues for CDMA Transmit task processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void txc_powerup_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_CMD

DESCRIPTION
  This is how the outside world interfaces with the tx task.  Call this
  procedure with the address of the txc_cmd_type and the command will be
  placed on the txc_cmd_q

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void txc_cmd
(
  txc_cmd_type *cmd_ptr
    /* The command to queue up */
);



/* <EJECT> */
/*===========================================================================

FUNCTION TXC_SUBTASK

DESCRIPTION
  Transmit subtask to process CDMA.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void txc_subtask( void );

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_SET_FRAME_OFFSET

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void txc_set_frame_offset( byte new_frame_offset );

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_GET_TIME_IN_1_25MS_UNIT

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern dword txc_get_time_in_1_25ms_unit( void );



#if defined (FEATURE_RL_JUMP_ENHANCEMENT)
/*===========================================================================

FUNCTION TXC_OK_TO_SLEEP

DESCRIPTION
  This function returns true if MC can begin sleep.  This mechanism is in
  place to delay shutdown for a period after an access attempt to prevent
  the need to jump to hyperspace for another attempt that occurs soon
  thereafter.

DEPENDENCIES

RETURN VALUE
  True if it is okay to sleep.  False if sleep is to be delayed.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean txc_ok_to_sleep( void );
#endif // FEATURE_RL_JUMP_ENHANCEMENT

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION rpc_reach_rate_word_satisfies_gain_limit

DESCRIPTION

  Determines if the passed rate_word would use more gain than allowed.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if rate_word satisfies the gain limit.
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean rpc_reach_rate_word_satisfies_gain_limit
(
  txc_each_rate_type rate_word,
  int16 rlgain_common_pilot
);


/*===========================================================================

FUNCTION RLGC_GET_NOMINAL_RCCH_ATTRIBUTE_GAIN

DESCRIPTION
  The index must be between 0 and TXC_EACH_RATE_38400_5_V, or else it will return
  the value at index 0.

DEPENDENCIES

RETURN VALUE
 Return EACH Attribute Gains from the rpc_nominal_RCCH_attribute_gain_table.

SIDE EFFECTS
  None.
===========================================================================*/
int16 rlgc_get_nominal_rcch_attribute_gain(int16 index);
#endif

#endif /* TXC_H */


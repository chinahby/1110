#ifndef TXCTRAFFIC_H
#define TXCTRAFFIC_H
/*===========================================================================

           C D M A   T R A N S M I T   T R A F F I C   M O D U L E

DESCRIPTION
  This header file contains definitions for the transmit traffic module  that
  are used by the txc unit.  This header file is internal to the txc unit.

Copyright (c) 1997 through 2008 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/txctraffic.h_v   1.18   15 Oct 2002 18:31:02   donb  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/txctraffic.h#2 $ $DateTime: 2008/07/18 10:34:36 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV.
10/30/06   bn      Added function prototype txc_tt_tc_init() to support DO/UMTS 
                   to 1x handoff
07/24/06   vlc     Removed extern prim_num_bits which is only used in txcmux.c.
06/09/06   trc     Remove T_MSM6700 featurization
09/01/04   jrp     Corrected compiler warning.
09/01/04   jrp     Added back pre-6700 interfaces.
08/16/04   sr      Decalred prototype for txc_mdsp_dv_app_enable()
08/16/04   ljl     Rel D reverse channels PLT.
07/28/04   jrp     Removed lint warning.
06/29/04   jrp     Added argument to txc_traffic_isr() to store last interrupt
                   bin.
05/07/04   vlc     Modified RLP logging extensively to decouple MUX processing
                   and RLP processing, moved RLP processing to RLP module.
04/20/04   jrp     Fixed lint warnings.
03/28/03   vlc     Added struct txc_rlp_sig_priority_type to keep track of
                   priorities in sending RLP control frames as secondary SO
                   in a SVD call.
10/15/02   dlb     Added new power control logging to combine power control
                   and frame type logging.
09/04/02   bn,hrk  Moved prim_dcch_data_ptr, prim_dcch_rate out of
                   FEATURE_IS2000_REL_A_SVD.
06/28/02   bn      Added new interface to support MUX PDU5
06/12/02   hrk     Added prototype for txc_tx_process_fundich_data3g().
06/05/02   hrk     Added extern declarations for globals used for Primary and
                   Secondary traffic types.
03/04/02   bgc     Changed features to IS2000_R_SCH where appropriate.
02/25/02   hjr     Fixed featurization of FEATURE_IS2000_SCH with
                   FEATURE_IS2000_DS_RS2 on.
02/25/02   bn      Added support for  MUX SVD signaling
10/30/01   bn      Added support for Release A RDCH logging
09/27/01   vlc     Added inclusion of datapump.h to resolve datapump externs
                   declared in this file (for PLT mode only.)
08/22/01   bn      Support DCCH in FTM mode
06/15/01   day     Mainlined FEATURE_IS2000
05/11/01   vlc     Merged in datapump changes for PLT (ks.)
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
03/07/01   va      Added p_rev_in_use in the txtc_type structure.
03/01/01   tc      Merged MSM_MUX1X.00.00.15.
02/20/01   hrk     Added tx_rate_adj to txtc_type.
01/11/01   tc      Merged MSM_MUX1X.00.00.06.
11/23/00   tc      Fixed txc_dp_run for MSM5100 PLT.
12/07/00   bgc     Removed initialization of extern for PLT.
11/10/00   jc      Created after repartition of txc.c

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "bio.h"
#include "voc.h"
#include "cai.h"
#include "txc.h"
#include "dsrlp.h"

#ifdef FEATURE_PLT
#error code not present
#endif

#include "txccommon.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

// Definitions for physical channels carrying signalling
typedef enum
{
  TXC_REV_FCH  = 0x1,
  TXC_REV_DCCH = 0x2
} txc_dsch_type;

/* Specific variables for the Traffic Channel */
typedef struct
{
  boolean            preambles;    /* Send Traffic Channel preambles  */
  word               num_preambles;/* # of Traffic Channel preambles  */
  boolean            nulls;        /* Send null data  */
  txc_tc_frame_type  frame;        /* Current frame to send */
  #ifdef FEATURE_IS2000_P2
  txc_tc_dcch_frame_type  dcch_frame;   /* Current DCCH frame to send */
  txc_dsch_type      dsch_map;     /* Channel mask of channels mapped to dedicated signalling channel */
  txc_dsch_type      cur_dsch;     /* DSCH currently used */
  #ifdef FEATURE_IS2000_CHS
  cai_pilot_gating_rate_type gating_rate; /* Indicate if pilot gated and rate */
  #endif // FEATURE_IS2000_CHS
  #endif // FEATURE_IS2000_P2
  byte               dim_delay;    /* Wait for correct rate vocoder frame */
  byte               power;        /* Power level */
  enc_rate_type      rate;         /* current frame encoder rate */
  enc_rate_type      last_rate;    /* Rate of the last tc frame */
  word               sigs;         /* Signal to set when TX task has acquired
                                      timing has moved to traffic state */
  rex_tcb_type       *tcb_ptr;     /* Pointer to task to signal when TX task
                                      has moved to traffic state */
  boolean            ready_to_transmit;
                                   /* Flag to indicate if TC is ready to
                                      transmit (the preamble) during TC
                                      initialization.  We do not transmit
                                      anything before the 2 good frames are
                                      received in TCI. */
#ifdef FEATURE_IS2000_R_SCH
  txc_tc_supp_frame_type  supp_frame; /* current supp. frame to send */
  enc_sch_rate_type       supp_rate;
#endif /*FEATURE_IS2000_R_SCH*/

  int8               rl_gain_adj;
  boolean            fch_8th_rate_gating;
  byte               offset_pwr; /* turn around constant for
                                    Open Loop Power Estimation */

  boolean            sch_on;
  boolean            supp_turbo;    /* supp. channel Turbo?  */
  word               supp_ltu_size; /* supp. channel ltu size   */
#ifdef FEATURE_IS2000_REL_A //32X
  cai_mux_pdu_type_type mux_pdu_type; // 1,2, 3, 5
  uint8             num_ltu;          // Number of LTUs in an SDU
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_R_SCH
  boolean            txc_dtx_off;           /* DTX expired? */
  boolean            dtx_duration_included; /* DTX activate? */
  byte               dtx_duration_org;      /* original/reset value */
  uint16             dtx_duration;          /* active value */

  byte               pot_num_mux_pdus; /* # of MuxPDUs in a R-SCH frame    */
  byte               ok_num_mux_pdus;  /* allowed # mux=-pdus in a frame   */

  boolean            double_sized;  /* TRUE if double sized fr being tx */
  cai_radio_config_type   r_sch_rc; /* RC of R-SCH when sch_on is TRUE */
#endif /* FEATURE_IS2000_R_SCH */

  byte               pwr_ctrl_step_size; /* Step size */
#ifdef FEATURE_FACTORY_TESTMODE
  boolean            fch_loopback;  /* fch loopback mode */
  /* Support FEATURE_DCCH */
  boolean            dcch_loopback;  /* dcch loopback mode */
  /* End of FEATURE_DCCH */
  boolean            sch_loopback;  /* sch loopback mode */
#endif
  int16              tx_rate_adj;
#ifdef FEATURE_IS2000_REL_A
  byte p_rev_in_use;  /* P_REV in for needed for frame building */
#endif
  byte rev_mux;       /* for RDCH logging */

  uint16 last_encoder_interrupt_bin;
} txtc_type;

extern txtc_type txtc;

extern byte *prim_data_ptr;           /* Pointer to primary data */
extern cai_data_rate_type prim_rate;  /* Primary data rate */

#ifdef FEATURE_IS2000_REL_A_SVD
extern byte *sec_fch_data_ptr;        /* Pointer to FCH secondary data */
extern word sec_fch_num_bits;         /* Num of bits in FCH secondary frame */

extern byte *sec_dcch_data_ptr;       /* Pointer to DCCH secondary data */
extern word sec_dcch_num_bits;        /* Num of bits in DCCH secondary frame */
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_P2
extern byte *prim_dcch_data_ptr;      /* Pointer to DCCH primary data */
extern cai_data_rate_type prim_dcch_rate; /* Primary DCCH data rate */
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_FACTORY_TESTMODE
  /* Holding buffer for TX data on the reverse FCH*/
  extern txc_tc_frame_type  txc_ftm_rfch_data;
  extern enc_rate_type txc_ftm_rfch_rate;
#ifdef FEATURE_IS2000_P2
  /* Support FEATURE_FTM_DCCH */
  /* Holding buffer for TX data on the reverse DCCH*/
  extern txc_tc_frame_type  txc_ftm_rdcch_data;
  extern enc_rate_type txc_ftm_rdcch_rate;
  extern boolean txc_ftm_dtx_dcch;
  /* End of FEATURE_FTM_DCCH */
#endif
#ifdef FEATURE_IS2000_R_SCH
  /* Holding buffer for TX data on the reverse SCH*/
  extern txc_tc_supp_frame_type  txc_ftm_rsch_data;
  extern enc_sch_rate_type txc_ftm_rsch_rate;
#endif /*FEATURE_IS2000_R_SCH*/

#endif /*FEATURE_FACTORY_TESTMODE*/

  /* Accumulation buffer */
  extern log_rev_pctrl_type txc_log_rpctrl_buf;
  /* Sub-record pointer */
  extern log_rev_pctrl_subrecord_type *txc_log_rpctrl_sr_ptr;

  #define TXC_RPCTRL_LOG_OFFSET \
  (txc_log_rpctrl_buf.hdr.len - FPOS(log_rev_pctrl_type, var_len_buf))

  /* Reverse frame types log accumulation buffer */
  /* Accumulation buffer */
  extern log_rev_frame_types_type           txc_log_frm_types_buf;

  /* R-SCH0 frame type */
  extern log_sch_rate_enum_type txc_sch0_frame_rate;

  /* Sub-record pointer */
  extern log_rev_frame_types_subrecord_type *txc_log_frm_types_sr_ptr;

  #define TXC_FRM_TYPES_LOG_OFFSET \
  (txc_log_frm_types_buf.hdr.len - FPOS(log_rev_frame_types_type, var_len_buf))

#ifdef FEATURE_PLT
#error code not present
#endif // FEATURE_PLT

/* Handoff Action Time information */
extern boolean txc_use_action_time;
  /* TRUE --- explicit action time;
   * FALSE --- implicit action time.
   */

extern boolean txc_second_frm_sig_enabled;
  /* When "txc_second_frm_sig_enabled" is TRUE, TXC will report
   * "MCC_TXC_FRAME_SIG" to MC at the next "TXC_INT_SIG" after
   * TXC sends the first "MCC_TXC_FRAME_SIG" so that MC can tune
   * to different frequency after the transmitter is turned off
   */

   extern boolean txc_connect_priso;  /* for service option that associated with primary data */
#ifdef FEATURE_IS2000_REL_A_SVD
   extern boolean txc_connect_secso;  /* for service option that associated with secondary data */
#endif /* FEATURE_IS2000_REL_A_SVD */
   extern boolean txc_hho_preamble_pcg_units;

   #ifdef FEATURE_MAX_PWR_PROTECTION
        extern int16  tx_extra_gain;
   #endif /* FEATURE_MAX_PWR_PROTECTION */

#ifdef FEATURE_IS2000_REL_A_SVD
/* txc_rlp_sig_priority is used to keep track of priorities in sending RLP control
** frames when RLP is the secondary SO in a SVD call.  The purpose of the
** implementation is to improve voice quality in a SVD call.
*/
typedef struct
{
  dsrlp_tx_sig_enum_type prev_fr_val;
  dsrlp_tx_sig_enum_type curr_fr_val;
  word delay_fr_cnt;

} txc_rlp_sig_priority_type;

extern txc_rlp_sig_priority_type txc_rlp_sig_priority;
#endif /* FEATURE_IS2000_REL_A_SVD */

/* Number of frames to wait for Vocoder to act on a DIM request. */
#define TXC_VOC_DIM_DELAY 3

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION TXC_TRAFFIC

DESCRIPTION
  Process traffic channel state

DEPENDENCIES
  None
RETURN VALUE
  Next state.
SIDE EFFECTS
  None
===========================================================================*/
extern txc_state_type txc_traffic( txc_state_type state );

/*========================================================================
FUNCTION TXC_RLGC_TRANS_CMD

DESCRIPTION
   The RLGC transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
  extern void txc_rlgc_trans_cmd( mctrans_action_packet_type *pkt_ptr );

/*========================================================================
FUNCTION TXC_HO_TRANS_CMD

DESCRIPTION
   The HO transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
  extern void txc_ho_trans_cmd( mctrans_action_packet_type *pkt_ptr );

/*===========================================================================

FUNCTION TXC_TRAFFIC_INIT

DESCRIPTION
  Initialization section for traffic channel.

DEPENDENCIES
  None
RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/
extern void txc_traffic_init( void );

#ifdef FEATURE_IS2000_R_SCH
/*========================================================================
FUNCTION TXC_REV_SCH_TRANS_CMD

DESCRIPTION
   The reverse SCH transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
extern void txc_rev_sch_trans_cmd( mctrans_action_packet_type *pkt_ptr );
#endif

/*===========================================================================

FUNCTION TXC_TRAFFIC_ISR

DESCRIPTION
  Interrupt handler for the traffic channel.  Merge the vocoder and
  signaling data and give them to the encoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void txc_traffic_isr
(
  uint32 arg
);

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION TXC_TX_PROCESS_FUNDICH_DATA3G

DESCRIPTION
  This function sets up all information needed for RLP to build data frames
  for R-FCH and/or R-DCCH channels.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void txc_tx_process_fundich_data3g
(
 word,
 dsrlp_rate_enum_type,
 dsrlp_srvc_enum_type
);
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_LOG_PC_INFO_2
/*===========================================================================

FUNCTION TXC_RPC_INFO_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated reverse power control info log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A reverse power control info log may be sent.

===========================================================================*/
void txc_rpc_info_send_log (void);
#endif /* FEATURE_LOG_PC_INFO_2 */

#if defined(FEATURE_VOIP_1X_HANDOFF) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
/*===========================================================================

FUNCTION TXC_TT_TC_INIT

DESCRIPTION
  When transition from TXC_TT_S to TXC_TRAFFIC_S, this function will be called
  to initialize the RF to be ready for traffic.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void txc_tt_tc_init (boolean turn_on_tx);
#endif /* FEATURE_VOIP_1X_HANDOFF and FEATURE_UMTS_1X_HANDOVER_1XMSM */

#endif


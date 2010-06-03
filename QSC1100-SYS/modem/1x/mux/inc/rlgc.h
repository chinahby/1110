#ifndef RLGC_H
#define RLGC_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  REVERSE LINK POWER CONTROL PROCESSING

GENERAL DESCRIPTION
  This module is responsible for low level processing of data received
  from the base station in CDMA mode of operation for Reverse Link
  Power Control.

Copyright (c) 1999 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/rlgc.h_v   1.8   20 Aug 2002 16:51:40   baon  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/rlgc.h#2 $ $DateTime: 2008/07/18 10:34:36 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV and FEATURE_IS2000_REL_D_DV.
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
06/09/06   trc     Remove T_MSM6700 featurization
07/25/05   rkc     Fixed lint errors.
07/08/05   bn      Moved the function rpc_reach_rate_word_satisfies_gain_limit to txcmc.h
10/20/04   ljl     Updated for Rev D channels.
09/16/04   jrp     Added MAC support.
09/01/04   jrp     Allows for DTX of R-REQCH when writing gain ratios.
07/28/04   jrp     Added interface for programming linear gains to mDSP.
07/28/04   jrp     Removed interfaces that do not need to be exported.
06/24/03   jrp     Added rpc_set_tx_rate_adj() to conditionally convert from
                   1/12dB units to 1/10dB units before writing to TX_RATE_ADJ.
04/07/03   jrp     Lint changes.
08/20/02   bn      Added funtion prototype that supports new SCRM rate logic
04/03/02   jrp     Added rpc_rate_word_satisifes_gain_limit(),
                   rpc_get_reach_pilot_gain(), and rpc_get_reach_data_gain().
06/15/01   day     Mainlined FEATURE_RLGC
06/12/01   vlc     Added new value for Max_Num_Of_High_Rates.  Added
                   RPC_32X_RATE (inside FEATURE_32X_SCH_DATA_RATE.)
04/26/01   vlc     Changed name of a few constants to accomodate new SCH RC4
                   development on MSM5100.
04/20/01   sr      Merged in from MSM5100 archive
04/10/01   hrk     Added support for FEATURE_IS2000_P2
11/22/00   hrk     Removed inclusion of err.h and msg.h
09/19/00   hrk     Added prototype for rpc_plt_get_rl_gain_adj (),
                   rpc_plt_get_fch_rate (), rpc_fch_gating_cmd().
09/06/00   bgc     Added FEATURE_FACTORY_TESTMODE.
08/24/00   hrk     Mainlined FEATURE_CS_ACTION_TIME_TRANS.
06/09/00   hrk     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "qw.h"
#include "rxc.h"
#include "txc.h"
#include "enci.h"
#include "enc.h"
#include "cai.h"
#include "caii.h"
#include "deint.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm_cmd.h"
#endif

/*===========================================================================
                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

typedef struct
{
  int16  nom_att_gain;
  byte  pilot_ref_lvl;
} rl_att_gain_type;

#ifdef FEATURE_IS2000_REL_A_CC
/* Gain values output to MDSP has 15 bit resolution as result of TX
** gain enhancement on Dora target.
*/
typedef struct
{
  word pch_gain;    
  word each_gain;   
} rpc_reach_gain_type;

extern const int rpc_nominal_RCCH_attribute_gain_table[];
#endif // FEATURE_IS2000_REL_A_CC

/* 2x, 4x, 8x, 16x, 32x (32x has its own feature) */
#ifdef FEATURE_32X_SCH_DATA_RATE
#define Max_Num_Of_High_Rates     5
#else
#define Max_Num_Of_High_Rates     4
#endif /* FEATURE_32X_SCH_DATA_RATE */

/* 0 : Convolutional; 1 : Turbo */
#define Max_Num_Of_Coding_Types   2

/* First entri in rpc_gain_adj_tab corresponds to PCH gain of 30 */
#define TABLE_OFFSET    30

#ifdef FEATURE_32X_SCH_DATA_RATE
#define RPC_32X_RATE  (ENC_SCH_32X_RATE - ENC_SCH_2X_RATE)
#endif /* FEATURE_32X_SCH_DATA_RATE */
#define RPC_16X_RATE  (ENC_SCH_16X_RATE - ENC_SCH_2X_RATE)
#define RPC_8X_RATE   (ENC_SCH_8X_RATE  - ENC_SCH_2X_RATE)
#define RPC_4X_RATE   (ENC_SCH_4X_RATE  - ENC_SCH_2X_RATE)
#define RPC_2X_RATE   (ENC_SCH_2X_RATE  - ENC_SCH_2X_RATE)


/* Total digital baseband power = 255
*/
#define RPC_TOT_DIG_PWR_255  255

/* Reduce pilot channel digital gain in from 15 to 8 bit resolution.  The 8 
** bit resolution digital gain value is used to calculate TX Rate Adjust.  The
** 15 bit resolution digital gain value is used to output to MDSP.
*/
#define PCH_GAIN_TRUNCATE_TO_8BITS(gain_15bits) ((byte) ((gain_15bits) >> 7))


/* <EJECT> */
/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_TC_CMD

DESCRIPTION
  Performs initialization for Reverse Link Gain Control processing.

DEPENDENCIES
  Rate Set should be initialized in txc_so before invoking this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void
rpc_tc_cmd (txc_cmd_type  *cmd_ptr);

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_RELATIVE_GAINS

DESCRIPTION
  Returns the adjusted gain value for PCH.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void
rpc_get_relative_gains
(
  int16 *fch2pch,
  int16 *dcch2pch,
  int16 *sch02pch,
  enc_rate_type rate,
  txc_chan_status_type sch_chan_status,
  txc_chan_status_type dcch_chan_status
);

/* <EJECT> */
/*===========================================================================
 FUNCTION RPC_TRANS_CMD

DESCRIPTION
  This function process the MCTRANS_RLGC_ACTION_CMD from MC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void
rpc_trans_cmd (mctrans_action_packet_type *cmd_ptr);

/* <EJECT>^L */
/*===========================================================================
 FUNCTION RPC_FCH_GATING_MODE

DESCRIPTION
  If the R-FCH 1/8 Rate gating mode changes, this function re-calculates the
  gains.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void
rpc_fch_gating_cmd
(
 boolean rev_fch_gating_on
);

#ifdef FEATURE_IS2000_REL_A_CC

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_REACH_PILOT_GAIN

DESCRIPTION
  Returns the R_PICH digital gain that should be used with the R-EACH
  for the speicified rate_word.

DEPENDENCIES

RETURN VALUE
  Digital R-PICH gain.

SIDE EFFECTS
  None.
===========================================================================*/
word
rpc_get_reach_pilot_gain
(
  txc_each_rate_type rate_word
);


/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_REACH_DATA_GAIN

DESCRIPTION
  Returns the R_EACH digital gain that should be used for the speicified
  rate_word.

DEPENDENCIES

RETURN VALUE
  Digital R-EACH gain.

SIDE EFFECTS
  None.
===========================================================================*/
word
rpc_get_reach_data_gain
(
  txc_each_rate_type rate_word
);

#endif // FEATURE_IS2000_REL_A_CC

int16
rpc_get_dcch2pch_gain_ratio
(
 boolean dcch_only
);

#ifdef FEATURE_PLT
#error code not present
#endif

#ifdef FEATURE_MAX_PWR_PROTECTION
#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
FUNCTION RPC_GET_SCH_SCRM_RATE

DESCRIPTION
  For the given condition, this function returns the suitable R_SCH rate which is
  used in the SCRM.

DEPENDENCIES

RETURN VALUE
  R_SCH rate.

SIDE EFFECTS
  None.
===========================================================================*/
enc_sch_rate_type rpc_get_sch_scrm_rate
(
  enc_sch_rate_type curr_sch_rate
);
#endif /* FEATURE_IS2000_R_SCH */
#endif /* FEATURE_MAX_PWR_PROTECTION */

/*===========================================================================
FUNCTION RPC_SET_TX_RATE_ADJ

DESCRIPTION
  This function converts from 1/12dB to 1/10dB resolution if necessary, and
  calls srch to write the rate adjust value into hardware.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  rpc_set_tx_rate_adj
(
  int16  tx_adj
);

#ifdef FEATURE_IS2000_R_SCH
/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_SCH2PICH_GAIN_RATIO

DESCRIPTION
  This function computes the gain ratio between R-SCH and R-PICH.
  See section 2.1.2.3.3.7 of the rev D physical layer standard.

DEPENDENCIES
  This function should be called after RLGC parameters have been initialized.

RETURN VALUE
  SCH/PICH Gain Ratio.

SIDE EFFECTS
  None.
===========================================================================*/
int2 rpc_get_sch2pich_gain_ratio
(
 enc_sch_rate_type rate,
 boolean code_type       /* FALSE=Convolutional, TRUE=Turbo */
);
#endif /* FEATURE_IS2000_R_SCH */

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_PCH_ADJ_GAIN

DESCRIPTION
  Returns the adjusted gain value for PICH.
  This is programmed as TX_RATE_ADJ to correct for the PICH gain not equal
  to 256.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
int16 rpc_get_pch_adj_gain
(
  byte pch_gain
);

#endif /* RLGC_H */


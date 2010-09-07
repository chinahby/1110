/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      REVERSE LINK GAIN CONTROL

GENERAL DESCRIPTION
  This module is responsible for low level processing of data received
  from the base station in CDMA mode of operation for Reverse Link
  Power Control.


DATA DECLARATIONS
  txc_rpc

  Conversion Table
    rpc_gain_adj_tab

  Nominal Attribute Gain Tables
    FCH/DCCH Attribute Gains
      basic_rs1_rl_nom_att_gains
      gated_rs1_rl_nom_att_gains
      basic_rs2_rl_nom_att_gains
      gated_rs2_rl_nom_att_gains
    SCH Attribute Gains
      high_rs1_rl_nom_att_gains [Max_Num_Of_Coding_Types]
      high_rs2_rl_nom_att_gains [Max_Num_Of_Coding_Types]
    EACH Attribute Gains
      rpc_nominal_RCCH_attribute_gain_table
    ACKCH Attribute Gains
      ackch_rl_nom_att_gains
    CQICH Attribute Gains
      cqich_rl_nom_att_gains
    SPICH Attribute Gains
      spich_rl_nom_att_gains
    PDCCH Attribute Gains
      pdcch_rl_nom_att_gains
    REQCH Attribute Gains
      reqch_rl_nom_att_gains
    PDCH Attribute Gains
      pdch_table_sel_0_rl_nom_att_gains
      pdch_table_sel_0_rl_nom_att_gains

  Attribute Adjustment Gain Tables
    FCH/DCCH Attribute Adjustment Gains
      basic_rs1_rl_att_adj_gains
      basic_rs2_rl_att_adj_gains
    SCH Attribute Adjustment Gains
      high_rs1_rl_att_adj_gains
      high_rs2_rl_att_adj_gains
    PDCCH Attribute Adjustment Gains
      pdcch_rl_att_adj_gains
      pdcch_rl_boost_adj_gains
      pdcch_rl_payload_adj_gains
    PDCH Attribute Adjustment Gains
      pdch_rl_att_adj_gains
      pdch_rl_boost_adj_gains
      pdch_rl_subpacket_adj_gains
      pdch_rl_payload_adj_gains
  Stored Gain Ratios
    rlgc_fch_gain_ratio
    rlgc_rpdch_tpr_table
  Stored Digital Gains
    rpc_reach_gain_table

  MAC Support Tables
    rpc_tpr_table_linearQ10

PUBLIC FUNCTIONS
  rpc_tc_cmd
  rpc_trans_cmd
  rpc_fch_gating_cmd
  rpc_reach_rate_word_satisfies_gain_limit
  rpc_get_reach_pilot_gain
  rpc_get_reach_data_gain
  rpc_program_non_rev_d_gain_ratios
  rpc_program_rev_d_gain_ratios
  rpc_get_relative_gains
  rpc_set_tx_rate_adj
  rpc_get_sch_scrm_rate
  rpc_get_pch_adj_gain
  rpc_update_mac_tables
  rpc_get_maximum_tpr

PRIVATE FUNCTIONS
  Gain Ratio Functions
    rpc_get_fch2pich_gain_ratio
    rpc_get_dcch2pich_gain_ratio
    rpc_get_sch2pich_gain_ratio
    rpc_get_spich2pich_gain_ratio
    rpc_get_pdcch2pich_gain_ratio
    rpc_get_reqch2pich_gain_ratio
    rpc_get_ackch2pich_gain_ratio
    rpc_get_cqich2pich_gain_ratio
    rpc_get_pdch2pich_gain_ratio

  Initialization Functions
    rpc_init_att_adj_gain
    rpc_init_chan_adj_gain

  Digital Gain Computations
    rpc_compute_gains
    rpc_compute_reach_gains

  L3 Table Update Functions
    rpc_copy_adj0_rec_params
    rpc_copy_adj1_rec_params
    rpc_copy_adj2_rec_params
    rpc_copy_adj3_rec_params
    rpc_copy_adj4_rec_params
    rpc_init_pwr_ctrl_params
    rpc_mean_code_chan_output_power
    rpc_get_multi_chan_adj_gain
    rpc_calculate_multi_chan_adjustments

  Math Support Functions
    rpc_dBQ3_to_linearQ10
    rpc_exp10
    rpc_sqrt32
    rpc_chan_to_pch_gain
    rpc_max_gain
    rpc_min_gain

  PLT Support
    rpc_ho_cmd
    rpc_so_cmd
    rpc_pwr_ctrl_cmd
    rpc_plt_relative_gains
    rpc_plt_tc_cmd
    rpc_plt_ho_cmd
    rpc_plt_so_cmd
    rpc_plt_pwr_ctrl_cmd
    rpc_plt_escam_cmd
    rpc_plt_get_rl_gain_adj
    rpc_plt_get_fch_rate

  SCH Support Functions
    rpc_get_sch_pilot_ref_level

  MAC Support Functions
    get_current_power_headroom_linearQ10


Copyright (c) 1999 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rlgc.c#5 $ $DateTime: 2008/09/26 14:46:09 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/08   jtm     Klockwork fixes
07/24/08   jtm     Added missing brace in rpc_mean_code_chan_output_power().
                   Updated RLGC_UPR_BNDS_CHK to RLGC_UPPER_BOUNDS_CHECK.
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV and FEATURE_IS2000_REL_D_DV.
                   Fixed Klockwork issues. Removed unused functions and reduced
                   debug strings for ULC effort.
03/07/08   vlc     In function rpc_get_sch_scrm_rate(), put dcch2pich 
                   definition under P2 feature.
04/23/07   vlc     Changed RL power error margin to 1dB.
02/23/07   vlc     Fixed compiler warnings/errors when SCH, Rel A, and P2 
                   features are turned off.
01/29/07   vlc     Store RL power error margin in a variable rather than using
                   the hard coded value.
01/09/07   vlc     Merged the following change to main line from SC2X branch.
                   Mainly, adding support for FEATURE_MDSP_SUPPORTS_TX_AGC.
           vm      Added the support for Firmware TX 
09/19/06   vlc     Fixed lint errors.
09/11/06   vlc     Implemented TX gain enhancements Phase 2, where the db to
                   linear calculation is changed from using a lookup table to
                   calling function convert_dBq8_to_lin(). 
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
06/09/06   trc     Remove T_MSM6700 featurization
03/20/06   vlc     In function rpc_calculate_multi_chan_adjustments(),
                   corrected look up tables based on RC. Original lookup tables
                   were swapped.
09/08/05   ejv     Use new srch_mux interface calls.
07/25/05   rkc     Fixed lint errors.
07/08/05   bn      Added the new rlgc_get_nominal_rcch_attribute_gain() for MC
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
03/14/05   vlc     Fixed compiler warnings for MSM6700.
12/16/04   ljl     Updated for featurization.
12/06/04   ljl     Enabled ACKCH and CQICH when f_dv_rlgc_op.mode == MCTRANS_RLGC_START.
10/22/04   ljl     Changed pdcch_info to r_pdcch_info.
10/20/04   ljl     Updated for Rev D channels.
09/30/04   ljl     Added featurization for get_current_power_headroom_linearQ10().
09/27/04   ljl     Replaced pdch with rev_pdch.
09/16/04   jrp     Added multi-channel adjustment, linting, code review comments, and rest
                   of MAC support.
09/15/04   jrp     Added MAC support.
09/14/04   jrp     Update calls to DV_INFO header.
09/01/04   jrp     Allows for DTX of R-REQCH when writing gain ratios.  Added
                   rpc_update_mac_tables().
09/01/04   jrp     Corrected featurization.
08/17/04   jrp     Set values for cqich_on and ackch_on.  Removed debug messages.
08/17/04   jyw     Fixed the multi-channel-gain at 1x SCH
08/02/04   jrp     Fixed featurization.
07/28/04   jrp     Added programming of linear gains to mDSP.
07/28/04   jrp     Restructured file.
07/22/04   bn      For DV, call muxmdsp_wr_tx_rate_adj to program the tx_rate_adj to MDSP
06/16/04   sr      Merged in changes from 6500 5004 build
05/21/04   jyw     intorduce new PA backoff interface
                   new SCRM logic based on new PA backoff feature
05/06/04   jrp     Removed reference to rlgain_ackcqich_pilot
10/30/03   jrp     Fixed compiler warnings.
09/09/03   dna     Merged porting changes for MSM6500
06/24/03   jrp     Added rpc_set_tx_rate_adj() to conditionally convert from
                   1/12dB units to 1/10dB units before writing to TX_RATE_ADJ.
04/07/03   jrp     Lint changes.
03/26/03   bn      Added logic to support SCRM rate for R-SCH 32x
01/23/02   sr/wn   P2 featurizations fixes for JCDMA builds
10/10/02   jrp     Use rlgain_common_pilot as a 2's complement 6 bit number.
09/16/02   bn      Introduced the difference between the SCRM proposed set point
                   and the current set point into the total power bugget of SCRM logic
08/20/02   bn      Added new SCRM rate logics
08/20/02   dbc     Remove usage of global rf_tx_pwr_limit; instead use RF unit
                   API function rf_get_cdma_tx_pwr_limit().
08/07/02   jrp     Gave initial values for rpc_reach_gain_table[].
07/16/02   jrp     Added #ifdef in rpc_reach_rate_word_satisfies_gain_limit() in
                   support of multiple targets.
05/22/02   bn      Added new logics to support VP2 RPC
04/03/02   jrp     Added rpc_compute_reach_gains(), rpc_rate_word_satisifes_gain_limit(),
                   rpc_get_reach_pilot_gain(), and rpc_get_reach_data_gain().
01/14/02   vlc     Added rate set 2 support to PLT function
                   rpc_plt_escam_cmd().
01/03/02   bgc     Added SCH featurization to turn off for MSM6000.
10/02/01   hrk     Fixed Multi_Chan_Adj_Gain computation.
08/21/01   sj      Changed rs1_20ms_incl -> rc3_rc5_20ms_incl  &
                           rs2_20ms_incl -> rc4_rc6_20ms_incl
07/12/01   vlc     Changed FEATURE_RC4_SCH to FEATURE_DS_IS2000_RS2 in order
                   to eliminate FEATURE_RC4_SCH.
06/19/01   hrk     Fixed incorrect featurization.
06/15/01   day     Mainlined FEATURE_RLGC
06/14/01   lcc     Added initialization of rlgain_sch0_pilot in rpc_tc_cmd.
06/12/01   vlc     Added 32X support on R-SCH RC3.  Added R-SCH RC4 support.
                   Changes are inside the appropriate feature.
04/26/01   vlc     Changed name of a few constants to accomodate new SCH RC4
                   development on MSM5100.
04/23/01   sr      Changes to PLT to support new interface
04/20/01   sr      Merged in from MSM5100 archive
04/10/01   hrk     Added support for FEATURE_IS2000_P2
02/20/01   hrk     Removed MSGs to print GAIN RATIOs. Computing gain ratios is done
                   in ISR context and printing MSGs was taking significant time.
                   If R-SCH is DTX'd, a special value is used for SCH_TO_PICH gain
                   ratio so logging tools can handle TX_PWR calculations correctly.
11/14/00   hrk     For 1x R-SCH 1/8th rate gain values were used instead of FULL rate
09/19/00   hrk     Supported changes to Nominal Gain values for 1/8 R-FCH gating.
09/11/00   hrk     Linted.
09/06/00   djd/et  Added ifdef MSMHW_TURBO_CODING around turbo coding software.
08/24/00   hrk     Mainlined FEATURE_CS_ACTION_TIME_TRANS.
07/29/00   hrk     Enclosed RLGC functions put in Internal RAM with
                   #ifndef MSM5000_IRAM_REV
07/12/00   hrk     Added PLT support for RLGC on R-SCH.
06/09/00   hrk     Added support new MC-RXC interface and support for R-SCH.
04/17/00   hrk     Using a new, faster routine to compute Square Roots.
04/04/00   hrk     Added support for Reverse Power Control Logging.
                   Fixed data size inconsistency in computing relative gains.
02/24/00   hrk     Updated rpc_gain_adj_tab for new scaling factor.
                   Resetting Channel and Attribute Adj. Gain values if
                   BS sets DEFAULT_RLAG to TRUE in a HO message.
                   In rpc_pwr_ctrl_cmd () iterating based on enc_rate_type.
                   Modified implementation of rpc_get_pch_adj_gain().
                   Included parameter "fch_rate" in rpc_plt_tc_cmd().
                   Added rpc_plt_get_fch_rate() to return FCH rate for PLT.
02/10/00   hrk     Using new functions to access txc structs, MACROS defined
                   in enci.h for fractional rates.
02/09/00   hrk     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <memory.h>
#include "comdef.h"
#include "qw.h"
#include "err.h"
#include "log.h"
#include "rxc.h"
#include "txc.h"
#include "txccommon.h"
#include "enci.h"
#include "enc.h"
#include "msg.h"
#include "caii.h"
#include "deint.h"

#ifdef FEATURE_PLT
#error code not present
#endif
#include "txctraffic.h"
#include "msm_help.h"
#include "msm_drv.h"
#include "rf.h"
#include "rfm.h"
#include "rlgc.h"
#include "muxmdsp.h"
#include "ffpc.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE {

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Error margin for RL power is set to 1dB */
int16 rlgc_pch_adj_gain_margin = 12;

struct
{
  /********************/
  /* Enabled Channels */
  /********************/

  boolean fch_on;
#ifdef FEATURE_IS2000_P2
  boolean dcch_on;
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_R_SCH
  boolean sch_on;
#endif /* FEATURE_IS2000_R_SCH */

  /*********************/
  /* RLGAIN Parameters */
  /*********************/

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */
  int8 rlgain_traffic_pilot; /* unit : 0.125 dB */
#ifdef FEATURE_IS2000_R_SCH
  int8 rlgain_sch0_pilot;
#endif /* FEATURE_IS2000_R_SCH */

  /**********************************/
  /* Channel Adjustments Parameters */
  /**********************************/

  int8 fch_chan_adj_gain;    /* unit : 0.125 dB */
#ifdef FEATURE_IS2000_P2
  int8 dcch_chan_adj_gain;
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_R_SCH
  int8 sch0_chan_adj_gain;
#endif /* FEATURE_IS2000_R_SCH */
  int8 sch1_chan_adj_gain;

  /***************************************/
  /* Multichannel Adjustments Parameters */
  /***************************************/

  int16 fch_multi_chan_adj_gain;   /* valid only if sch_on is TRUE */
#ifdef FEATURE_IS2000_P2
  int16 dcch_multi_chan_adj_gain;  /* valid only if sch_on is TRUE */
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_R_SCH
  int16 sch_multi_chan_adj_gain;
#endif /* FEATURE_IS2000_R_SCH */

  /*********************/
  /*   FCH Parameters  */
  /*********************/

  cai_radio_config_type fch_rc;
  enc_rate_set_type  fch_rate_set;
  boolean rev_fch_gating_on;  /* TRUE indicates R-FCH 1/8 gating is ON/OFF */

  /*********************/
  /*  DCCH Parameters  */
  /*********************/

#ifdef FEATURE_IS2000_P2
  cai_radio_config_type dcch_rc;
  enc_rate_set_type  dcch_rate_set;
#endif /* FEATURE_IS2000_P2 */

  /*********************/
  /*   SCH Parameters  */
  /*********************/

#ifdef FEATURE_IS2000_R_SCH
#ifdef FEATURE_DS_IS2000_RS2
  cai_radio_config_type sch_rc;  /* R-SCH rc.    valid only if sch_on is TRUE */
#endif /* FEATURE_DS_IS2000_RS2 */
  boolean sch_code_type;      /* TURBO or CONVO. valid only if sch_on is TRUE */
  enc_sch_rate_type sch_rate; /* R-SCH rate.     valid only if sch_on is TRUE */
#endif /* FEATURE_IS2000_R_SCH */

  /*********************/
  /*   PLT Parameters  */
  /*********************/

#ifdef FEATURE_PLT
#error code not present
#endif

} txc_rpc;





/****************************************************************
 ****************************************************************
 *                     Conversion Table {                      *
 ****************************************************************
 ****************************************************************/

/* rpc_gain_adj_tab[0] corresponds to PCH gain of 30 */

const byte rpc_gain_adj_tab [226] = {
242, 238, 234, 231, 228, 224, 221, 218, 215, 212,
209, 206, 204, 201, 198, 196, 193, 191, 189, 186,
184, 182, 180, 177, 175, 173, 171, 169, 167, 165,
163, 162, 160, 158, 156, 154, 153, 151, 149, 148,
146, 144, 143, 141, 140, 138, 137, 135, 134, 132,
131, 129, 128, 127, 125, 124, 123, 121, 120, 119,
118, 116, 115, 114, 113, 111, 110, 109, 108, 107,
106, 105, 103, 102, 101, 100, 99,  98,  97,  96,
95,  94,  93,  92,  91,  90,  89,  88,  87,  86,
85,  84,  83,  82,  81,  81,  80,  79,  78,  77,
76,  75,  74,  73,  73,  72,  71,  70,  69,  69,
68,  67,  66,  65,  65,  64,  63,  62,  61,  61,
60,  59,  58,  58,  57,  56,  55,  55,  54,  53,
53,  52,  51,  51,  50,  49,  48,  48,  47,  46,
46,  45,  44,  44,  43,  43,  42,  41,  41,  40,
39,  39,  38,  37,  37,  36,  36,  35,  34,  34,
33,  33,  32,  31,  31,  30,  30,  29,  29,  28,
27,  27,  26,  26,  25,  25,  24,  24,  23,  22,
22,  21,  21,  20,  20,  19,  19,  18,  18,  17,
17,  16,  16,  15,  15,  14,  14,  13,  13,  12,
12,  11,  11,  10,  10,  9,   9,   8,   8,   7,
7,   6,   6,   5,   5,   5,   4,   4,   3,   3,
2,   2,   1,   1,   0,   0
};

/****************** End Conversion Table } ******************/


/****************************************************************
 ****************************************************************
 *                Nominal Attribute Gain Tables {               *
 ****************************************************************
 ****************************************************************/




/****************************************************************
 *                   FCH/DCCH Attribute Gains                   *
 * See Tables 2.1.2.3.3.7-1 and 7-3 of Rev D Physical Layer     *
 * standard.                                                    *
 ****************************************************************/

/* Nominal Attribute Gain Table for Rate Set 1 RC3 for R-FCH
 * and R-DCCH */
const rl_att_gain_type basic_rs1_rl_nom_att_gains [ENC_NUM_FRAC_RATES] =
                 {
                   { 30, 0}, /* FULL 9600 bps */
                   { -2, 0}, /* HALF 4800 bps */
                   {-22, 0}, /* QTR  2700 bps */
                   {-47, 0}  /* 8TH  1500 bps */
                 };

/* Rate Set 1 Nominal Attribute Gain for R-FCH 1/8 Rate Gating */
const rl_att_gain_type gated_rs1_rl_nom_att_gains =  {-10, 0};

/* Nominal Attribute Gain Table for Rate Set 2 RC4 for R-FCH
 * and R-DCCH */
const rl_att_gain_type basic_rs2_rl_nom_att_gains [ENC_NUM_FRAC_RATES] =
                 {
                   { 44, 3},  /* FULL 14400 bps */
                   { 15, 3},  /* HALF 7200  bps */
                   {-13, 3},  /* QTR  3600  bps */
                   {-42, 3}   /* 8TH  1800  bps */
                 };

/* Rate Set 2 Nominal Attribute Gain for R-FCH 1/8 Rate Gating */
const rl_att_gain_type gated_rs2_rl_nom_att_gains =  {-2, 3};






/****************************************************************
 *                      SCH Attribute Gains                     *
 * See Tables 2.1.2.3.3.7-1 and 7-2 of Rev D Physical Layer     *
 * standard.                                                    *
 ****************************************************************/

/* Nominal Attribute Gain Table for Rate Set 1 R-SCH */
const rl_att_gain_type high_rs1_rl_nom_att_gains [Max_Num_Of_Coding_Types]
                                           [Max_Num_Of_High_Rates] =
                 {
                   { /* CONVOLUTIONAL */
                     {50, 1},   /* 2X  19200  bps */
                     {60, 11},  /* 4X  38400  bps */
                     {72, 21},  /* 8X  76800  bps */
                     {84, 36}   /* 16X 153600 bps */
#ifdef FEATURE_32X_SCH_DATA_RATE
                    ,{96, 54}   /* 32X 307200 bps */
#endif /* FEATURE_32X_SCH_DATA_RATE */
                   },
                   { /* TURBO */
                     {44, 2},   /* 2X  19200  bps */
                     {56, 10},  /* 4X  38400  bps */
                     {68, 19},  /* 8X  76800  bps */
                     {76, 33}   /* 16X 153600 bps */
#ifdef FEATURE_32X_SCH_DATA_RATE
                    ,{88, 50}   /* 32X 307200 bps */
#endif /* FEATURE_32X_SCH_DATA_RATE */
                   }
                 };

#ifdef FEATURE_DS_IS2000_RS2
/* Nominal Attribute Gain Table for Rate Set 2 R-SCH */
const rl_att_gain_type high_rs2_rl_nom_att_gains [Max_Num_Of_Coding_Types]
                                           [Max_Num_Of_High_Rates] =
                 {
                   { /* CONVOLUTIONAL */
                     {56, 11},  /* 2X  28800  bps */
                     {72, 18},  /* 4X  57600  bps */
                     {80, 32},  /* 8X  115200 bps */
                     {88, 46}   /* 16X 230400 bps */
#ifdef FEATURE_32X_SCH_DATA_RATE
                    ,{0,  0}    /* 32X (No such rate for RC4) */
#endif /* FEATURE_32X_SCH_DATA_RATE */
                   },
                   { /* TURBO */
                     {52, 9},   /* 2X  28800  bps */
                     {64, 19},  /* 4X  57600  bps */
                     {76, 29},  /* 8X  115200 bps */
                     {88, 39}   /* 16X 230400 bps */
#ifdef FEATURE_32X_SCH_DATA_RATE
                    ,{0,  0}    /* 32X (No such rate for RC4) */
#endif /* FEATURE_32X_SCH_DATA_RATE */
                   }
                 };
#endif /* FEATURE_DS_IS2000_RS2 */



/****************************************************************
 *                     EACH Attribute Gains                     *
 * See Table 2.1.2.3.3.2-1 of Rev D Physical Layer standard.    *
 ****************************************************************/

#ifdef FEATURE_IS2000_REL_A_CC
// Taken from table 2.1.2.3.3.1-1 of physical layer standard
const int rpc_nominal_RCCH_attribute_gain_table[] =  // Indexed by txc_each_rate_type;
{
  30, // TXC_EACH_RATE_9600_20_V
  50, // TXC_EACH_RATE_19200_20_V
  64, // TXC_EACH_RATE_19200_10_V
  72, // TXC_EACH_RATE_38400_20_V
  80, // TXC_EACH_RATE_38400_10_V
  88  // TXC_EACH_RATE_38400_5_V
};
#endif // FEATURE_IS2000_REL_A_CC


/************* End Nominal Attribute Gain Tables } **************/




/****************************************************************
 ****************************************************************
 *              Attribute Adjustment Gain Tables {              *
 ****************************************************************
 ****************************************************************/





/****************************************************************
 *             FCH/DCCH Attribute Adjustment Gains              *
 ****************************************************************/

/* Attribute Adjustment Gain Table for Rate Set 1 for R-FCH */
rl_att_gain_type basic_rs1_rl_att_adj_gains [ENC_NUM_FRAC_RATES] =
                 {
                   {0, 0}, {0, 0}, {0, 0}, {0, 0}
                 };

/* Attribute Adjustment Gain Table for Rate Set 2 for R-FCH */
rl_att_gain_type basic_rs2_rl_att_adj_gains [ENC_NUM_FRAC_RATES] =
                 {
                   {0, 0}, {0, 0}, {0, 0}, {0, 0}
                 };





/****************************************************************
 *                SCH Attribute Adjustment Gains                *
 ****************************************************************/

/* Attribute Adjustment Gain Table for Rate Set 1 R-SCH */
rl_att_gain_type high_rs1_rl_att_adj_gains [Max_Num_Of_Coding_Types]
                                           [Max_Num_Of_High_Rates] =
                 {
                   {
                     /* CONVOLUTIONAL */
                     {0, 0}, {0, 0}, {0, 0}, {0, 0}
#ifdef FEATURE_32X_SCH_DATA_RATE
                    ,{0, 0}
#endif /* FEATURE_32X_SCH_DATA_RATE */
                   },
                   {
                     /* TURBO */
                     {0, 0}, {0, 0}, {0, 0}, {0, 0}
#ifdef FEATURE_32X_SCH_DATA_RATE
                    ,{0, 0}
#endif /* FEATURE_32X_SCH_DATA_RATE */
                   }
                 };

#ifdef FEATURE_DS_IS2000_RS2
/* Attribute Adjustment Gain Table for Rate Set 2 R-SCH */
rl_att_gain_type high_rs2_rl_att_adj_gains [Max_Num_Of_Coding_Types]
                                           [Max_Num_Of_High_Rates] =
                 {
                   {
                     /* CONVOLUTIONAL */
                     {0, 0}, {0, 0}, {0, 0}, {0, 0}
#ifdef FEATURE_32X_SCH_DATA_RATE
                    ,{0, 0}
#endif /* FEATURE_32X_SCH_DATA_RATE */
                   },
                   {
                     /* TURBO */
                     {0, 0}, {0, 0}, {0, 0}, {0, 0}
#ifdef FEATURE_32X_SCH_DATA_RATE
                    ,{0, 0}
#endif /* FEATURE_32X_SCH_DATA_RATE */
                   }
                 };
#endif /* FEATURE_DS_IS2000_RS2 */


/*********** End Attribute Adjustment Gain Tables } *************/





/****************************************************************
 ****************************************************************
 *                       Stored Gain Ratios {                   *
 ****************************************************************
 ****************************************************************/

/*********************/
/*     FCH Ratios    */
/*********************/

int16 rlgc_fch_gain_ratio[ENC_NUM_FRAC_RATES];


/*********************/
/*     DCCH Ratio    */
/*********************/

int16 rlgc_dcch_gain_ratio=0;

/********************/
/*     SCH Ratio    */
/********************/

int16 rlgc_sch_gain_ratio=0;


/******************* End Stored Gain Ratios } *******************/





/****************************************************************
 ****************************************************************
 *                     Stored Digital Gains {                   *
 ****************************************************************
 ****************************************************************/

#ifdef FEATURE_IS2000_REL_A_CC
// Table of gains computed for rate_words.  Index is of type txc_each_rate_type.
// When rpc_reach_rate_word_satisfies_gain_limit() is called by CP, the gains are
// computed for the given rate_word and stored in this table.
// Initial values correspond to rlgain_common_pilot==0
// Resolution was changed from 8 bits to 15 bits by shifting left by 7.  These
// default values will most likely be overwritten by RLGC gain calculation.
rpc_reach_gain_type rpc_reach_gain_table[6] =
{
   /* TXC_EACH_RATE_9600_20_V  */
   {139<<7, 214<<7},
   /* TXC_EACH_RATE_19200_20_V */
   {112<<7, 228<<7},
   /* TXC_EACH_RATE_19200_10_V */
   {94<<7, 235<<7},
   /* TXC_EACH_RATE_38400_20_V */
   {86<<7, 242<<7},
   /* TXC_EACH_RATE_38400_10_V */
   {78<<7, 245<<7},
   /* TXC_EACH_RATE_38400_5_V  */
   {70<<7, 248<<7}
};
#endif // FEATURE_IS2000_REL_A_CC

/******************* End Stored Digital Gains } *****************/


/****************************************************************
 ****************************************************************
 *                       MAC Support Tables {                   *
 ****************************************************************
 ****************************************************************/

uint16 rpc_tpr_table_linearQ10[17];

/******************* End MAC Support Tables } *****************/


#define FCH_GAIN     0x01
#define FCH_GAIN_M   0x01
#define DCCH_GAIN    0x02
#define DCCH_GAIN_M  0x02
#define SCH_GAIN     0x04
#define SCH_GAIN_M   0x04

/* SCH_TO_PICH gain ratio when R-SCH is assigned but DTX'd.
** !! NOTE: This value is also used by QCAT and if for
** any reason is changed, QCAT should be notified
*/
#define RPC_SCH2PCH_DTX_GAIN_RATIO  0x7FFF

/* DCCH_TO_PICH gain ratio when R-DCCH is assigned but DTX'd.
** !! NOTE: This value is also used by QCAT and if for
** any reason is changed, QCAT should be notified
*/
#define RPC_DCCH2PCH_DTX_GAIN_RATIO  0x7FFF

/* Part of the TX gain enhancements is to replace the dB2lin lookup table with 
** calls to convert_dbq8_to_lin() for better gain ratio accuracy.  This
** change results in higher precision with proper Q format.  
** Under normal operations, call this function with bias of -0.0089 (-2 in Q8)
** to remove any positive bias inherent in the dB2lin function due to
** RF non-linear effects.
*/
#define RF_NON_LINEAR_BIAS_CORRECTION -2

#ifdef FEATURE_PLT
#error code not present
#endif


#define RLGC_UPPER_BOUNDS_CHECK(index, array_size) \
          (((index) < (array_size)) ? (index):((array_size)-1))


/*===========================================================================
                      } PRIVATE FUNCTION PROTOTYPES {
===========================================================================*/

#ifdef FEATURE_IS2000_R_SCH
int16 rpc_get_sch_pilot_ref_level(enc_sch_rate_type, boolean);
int16 rpc_get_multi_chan_adj_gain(enc_sch_rate_type, enc_rate_set_type);
#endif /*  FEATURE_IS2000_R_SCH */
void rpc_init_att_adj_gain (void);
void rpc_init_chan_adj_gain (void);
txc_gain_type rpc_compute_gains(int16, int16, int16, byte, byte);
#ifdef FEATURE_IS2000_REL_A_CC
rpc_reach_gain_type rpc_compute_reach_gains(txc_each_rate_type, int16, byte);
#endif /* FEATURE_IS2000_REL_A_CC */
void rpc_copy_adj0_rec_params(caii_rpc_adj0_type*);
void rpc_copy_adj1_rec_params(caii_rpc_adj1_type*);
void rpc_copy_adj2_rec_params(caii_rpc_adj2_type*);
void rpc_init_pwr_ctrl_params(byte, caii_rpc_type*);
void rpc_mean_code_chan_output_power(void);
dword rpc_sqrt32(dword);
word rpc_max_gain(word, word, word, word);
int2 rpc_get_fch2pich_gain_ratio(byte, boolean);
int2 rpc_get_dcch2pich_gain_ratio(boolean);
void rpc_calculate_multi_chan_adjustments ( void );



/* <EJECT> */
/*===========================================================================
                      } PUBLIC FUNCTIONS {
===========================================================================*/


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
void rpc_tc_cmd
(
  txc_cmd_type  *cmd_ptr
)
{
   /* initialize Attribute Adjustment Gain to 0 */
   rpc_init_att_adj_gain ();

   /* initialize Reverse Channel Adjustment Gains for all chans to 0 */
   rpc_init_chan_adj_gain ();

#ifdef FEATURE_PLT
#error code not present
#endif

   /* store RLGAIN_TRAFFIC_PILOT value regardless of RC,
    * although it will be used only for RC3 and above.
    */
   txc_rpc.rlgain_traffic_pilot = (int8) cmd_ptr->tc.rlgain_traffic_pilot;

#ifdef FEATURE_IS2000_R_SCH
   /* Initialize boolean to indicate R-SCH is not assigned */
   txc_rpc.sch_on = FALSE;
   txc_rpc.rlgain_sch0_pilot = 0;
   rlgc_sch_gain_ratio = 0;
#endif /* FEATURE_IS2000_R_SCH */

   /* Initialize R-FCH 1/8 Rate gating mode */
   txc_rpc.rev_fch_gating_on = cmd_ptr->tc.rev_fch_gating_on;

#ifdef FEATURE_IS2000_R_SCH
   /* Initialize MULTI_CHAN_ADJ_GAIN for R-FCH/R-DCCH */
   txc_rpc.fch_multi_chan_adj_gain  = 0;
#ifdef FEATURE_IS2000_P2
   txc_rpc.dcch_multi_chan_adj_gain = 0;
#endif /* FEATURE_IS2000_P2 */
#endif /* FEATURE_IS2000_R_SCH */

   txc_rpc.fch_on = FALSE;
   if (
#ifdef FEATURE_IS2000_P2
       (cmd_ptr->tc.rev_fch.included) &&
#endif /* FEATURE_IS2000_P2 */
       ((enc_fch_radio_config_type) cmd_ptr->tc.rev_fch.rc >= ENC_FCH_RC_3))
   {
      txc_rpc.fch_on = TRUE;

      /* Initialize R-FCH rate set and RC */
      txc_rpc.fch_rc = cmd_ptr->tc.rev_fch.rc;
      txc_rpc.fch_rate_set =
        (enc_rate_set_type) ENC_RC_TO_RATE_SET (cmd_ptr->tc.rev_fch.rc);

      /* compute FCH_TO_PCH gain */
      //rpc_mean_code_chan_output_power ();
   }

#ifdef FEATURE_IS2000_P2
   txc_rpc.dcch_on = FALSE;
   if ((cmd_ptr->tc.rev_dcch.included) &&
       ((enc_fch_radio_config_type) cmd_ptr->tc.rev_dcch.rc >= ENC_FCH_RC_3))
   {
      txc_rpc.dcch_on = TRUE;

      /* Initialize R-DCCH rate set and RC */
      txc_rpc.dcch_rc = cmd_ptr->tc.rev_dcch.rc;
      txc_rpc.dcch_rate_set =
        (enc_rate_set_type) ENC_RC_TO_RATE_SET (cmd_ptr->tc.rev_dcch.rc);

      /* compute DCCH_TO_PCH gain */
      //rpc_mean_code_chan_output_power ();
   }
#endif /* FEATURE_IS2000_P2 */

   // Compute gains
   rpc_mean_code_chan_output_power ();

} /* rpc_tc_cmd */




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
void rpc_trans_cmd
(
 mctrans_action_packet_type *cmd_ptr
)
{
  mctrans_rlgc_action_type  *rlgc_ptr = &cmd_ptr->action.rlgc;

  if (rlgc_ptr->fch_rlgc_op.mode == MCTRANS_RLGC_START)
  {
    txc_rpc.fch_on = TRUE;
    txc_rpc.fch_rc = rlgc_ptr->fch_rlgc_op.rc;
    txc_rpc.fch_rate_set =
        (enc_rate_set_type) ENC_RC_TO_RATE_SET (rlgc_ptr->fch_rlgc_op.rc);
  }
  else if (rlgc_ptr->fch_rlgc_op.mode == MCTRANS_RLGC_STOP)
  {
    txc_rpc.fch_on = FALSE;
    txc_rpc.fch_multi_chan_adj_gain = 0;
  }

#ifdef FEATURE_IS2000_P2
  if (rlgc_ptr->dcch_rlgc_op.mode == MCTRANS_RLGC_START)
  {
    txc_rpc.dcch_on = TRUE;
    txc_rpc.dcch_rc = rlgc_ptr->dcch_rlgc_op.rc;
    txc_rpc.dcch_rate_set =
        (enc_rate_set_type) ENC_RC_TO_RATE_SET (rlgc_ptr->dcch_rlgc_op.rc);
  }
  else if (rlgc_ptr->dcch_rlgc_op.mode == MCTRANS_RLGC_STOP)
  {
    txc_rpc.dcch_on = FALSE;
    txc_rpc.dcch_multi_chan_adj_gain = 0;
  }
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_R_SCH
  if (rlgc_ptr->rlgain_sch_pilot[0].included)
  {
     txc_rpc.rlgain_sch0_pilot = (int8) rlgc_ptr->rlgain_sch_pilot[0].value;
  }

  if (rlgc_ptr->sch_rlgc_op.mode == MCTRANS_RLGC_START)
  {
    txc_rpc.sch_on = TRUE;
    txc_rpc.sch_rate = (enc_sch_rate_type)(rlgc_ptr->sch_rlgc_op.sch_rate + 3);
    txc_rpc.sch_code_type = rlgc_ptr->sch_rlgc_op.sch_code_type;
#ifdef FEATURE_DS_IS2000_RS2
    txc_rpc.sch_rc   = rlgc_ptr->sch_rlgc_op.rc;
#endif /* FEATURE_DS_IS2000_RS2 */
  }
  else if (rlgc_ptr->sch_rlgc_op.mode == MCTRANS_RLGC_STOP)
  {
    txc_rpc.sch_on = FALSE;
  }

  if (rlgc_ptr->use_default_rlag)
  {
    /* reset any Attribute Adj Gain info previously stored. */
    rpc_init_att_adj_gain ();
    rpc_init_chan_adj_gain ();
  }

  if (rlgc_ptr->rlgain_traffic_pilot.included)
  {
    /* cache RLGAIN_TRAFFIC_PILOT */
     txc_rpc.rlgain_traffic_pilot = rlgc_ptr->rlgain_traffic_pilot.value;
  }

  /* process RPC_RECORDS */
  rpc_init_pwr_ctrl_params (rlgc_ptr->num_of_rpc_records,
                            &rlgc_ptr->rpc_records[0]);

  if (txc_rpc.sch_on)
  {
    /* If R-SCH is assigned, calculate fch_multi_chan_adj_gain */
    if ((txc_rpc.sch_rate >= ENC_SCH_FULL_RATE) &&
#ifdef FEATURE_32X_SCH_DATA_RATE
    (txc_rpc.sch_rate <= ENC_SCH_32X_RATE)
#else
    (txc_rpc.sch_rate <= ENC_SCH_16X_RATE)
#endif
       )
    {
      if (txc_rpc.fch_on)
      {
        txc_rpc.fch_multi_chan_adj_gain =
          rpc_get_multi_chan_adj_gain (txc_rpc.sch_rate, txc_rpc.fch_rate_set);
      }

#ifdef FEATURE_IS2000_P2
      if (txc_rpc.dcch_on)
      {
        txc_rpc.dcch_multi_chan_adj_gain =
          rpc_get_multi_chan_adj_gain (txc_rpc.sch_rate, txc_rpc.dcch_rate_set);
      }
#endif /* FEATURE_IS2000_P2 */
    }
    else
    {
      MSG_FATAL("Bad SCH rate: %d", txc_rpc.sch_rate, 0, 0);
    }
  }
#endif /* FEATURE_IS2000_R_SCH */

  rpc_mean_code_chan_output_power ();
} /* rpc_trans_cmd */



/* <EJECT>^L */
/*===========================================================================
FUNCTION RPC_FCH_GATING_MODE

DESCRIPTION
  Processing for MCTRANS_GATING_ACTION.
  If the R-FCH 1/8 Rate gating mode changes, this function re-calculates the
  gains.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_fch_gating_cmd
(
 boolean rev_fch_gating_on
)
{
   if (txc_rpc.rev_fch_gating_on != rev_fch_gating_on)
   {
      /* Gating mode changed. Initialize new gating mode */
      txc_rpc.rev_fch_gating_on = rev_fch_gating_on;

      /* recompute gains.  */
      rpc_mean_code_chan_output_power ();
   }
} /* rpc_fch_gating_cmd */


#ifdef FEATURE_IS2000_REL_A_CC
/* <EJECT> */
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
)
{
  int16 tx_open_loop;
  int16 tx_gain_adj;
  int16 tx_rate_adj;
  int16 tx_pwr_limit;

  /***************************************************************************
   * Sample verified values (in 8 bit resolution)
   *
   * RATE_WORD                  RLGAIN_COMMON_PILOT  Gp     Ge     TX_RATE_ADJ
   * TXC_EACH_RATE_9600_20_V     0                   139    214    39
   * TXC_EACH_RATE_19200_20_V    0                   112    228    17
   * TXC_EACH_RATE_19200_10_V    0                   94     235    -2
   * TXC_EACH_RATE_38400_20_V    0                   86     242    -11
   * TXC_EACH_RATE_38400_10_V    0                   78     245    -21
   * TXC_EACH_RATE_38400_5_V     0                   70     248    -32
   * TXC_EACH_RATE_9600_20_V    20                   112    229    17
   * TXC_EACH_RATE_19200_20_V   20                   88     240    -8
   * TXC_EACH_RATE_19200_10_V   20                   74     247    -27
   * TXC_EACH_RATE_38400_20_V   20                   66     247    -39
   * TXC_EACH_RATE_38400_10_V   20                   58     244    -52
   * TXC_EACH_RATE_38400_5_V    20                   52     245    -64
   *
   ***************************************************************************/

  rpc_reach_gain_table[rate_word] = rpc_compute_reach_gains(rate_word, rlgain_common_pilot, RPC_TOT_DIG_PWR_255);

  tx_rate_adj = rpc_get_pch_adj_gain(PCH_GAIN_TRUNCATE_TO_8BITS(rpc_reach_gain_table[rate_word].pch_gain));
  /*-------------------------------------------------------
  ** Adjust for offset power:
  ** For CELLULAR : -73 - (-81.5),
  ** For PCS      : -76 - (-84.5) = 8.5
  ** See Table 2.1.2.3.1-1
  **------------------------------------------------------*/
  tx_rate_adj += 102;
  /* units of 1/12 dB */

  MSM_LOCK();
  /* Open Loop RSSI in 1/12 dB units */
  tx_open_loop = rfm_get_tx_open_loop(RFCOM_TRANSCEIVER_0);
  /* gain adjust value in 1/12 dB units */
  tx_gain_adj = rfm_get_tx_gain_adjust(RFCOM_TRANSCEIVER_0);
  MSM_UNLOCK();

  /* convert to -1/12 dB units */
  tx_open_loop *= -1;

  /* change TX_GAIN_ADJ value to -1/12 dB units */
  tx_gain_adj *= -3;

  // gain limit represented by -(511 - rf_tx_pwr_limit)
  tx_pwr_limit = (int16) rfm_get_cdma_tx_pwr_limit(RFCOM_TRANSCEIVER_0);

  return ( ( -(511 - tx_pwr_limit ) - tx_open_loop - tx_gain_adj ) > tx_rate_adj);
} /* rpc_reach_rate_word_satisfies_gain_limit */


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
word rpc_get_reach_pilot_gain
(
  txc_each_rate_type rate_word
)
{
  return rpc_reach_gain_table[rate_word].pch_gain;
} /* rpc_get_reach_pilot_gain */


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
word rpc_get_reach_data_gain
(
  txc_each_rate_type rate_word
)
{
  return rpc_reach_gain_table[rate_word].each_gain;
}
/* rpc_get_reach_data_gain */

#endif // FEATURE_IS2000_REL_A_CC

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
void rpc_get_relative_gains
(
  int16 *fch2pich,
  int16 *dcch2pich,
  int16 *sch02pich,
  enc_rate_type rate,
  txc_chan_status_type sch_chan_status,
  txc_chan_status_type dcch_chan_status
)
{
   if (txc_rpc.fch_on)
   {
      *fch2pich  = rlgc_fch_gain_ratio[rate];
   }

#ifdef FEATURE_IS2000_P2
   if (txc_rpc.dcch_on)
   {
      if (dcch_chan_status == TXC_DCCH_ACTIVE)
      {
         *dcch2pich = rlgc_dcch_gain_ratio;
      }
      else
      {
         /* R-DCCH is DTX. */
         *dcch2pich = RPC_DCCH2PCH_DTX_GAIN_RATIO;
      }
   }
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_R_SCH
   if (txc_rpc.sch_on)
   {
      if (sch_chan_status == TXC_SCH_ACTIVE)
      {
         *sch02pich = rlgc_sch_gain_ratio;
      }
      else
      {
         /* R-SCH is DTX. */
         *sch02pich = RPC_SCH2PCH_DTX_GAIN_RATIO;
      }
   }
#endif /* FEATURE_IS2000_R_SCH */
} /* rpc_get_relative_gains */


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
void rpc_set_tx_rate_adj
(
  int16  tx_adj
  /* The 9-bit value to be written to the tx rate adjust register. */
)
{
  if ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
  {
    /* Convert from 1/12dB resolution to 1/10dB resolution */
    tx_adj *= 5;
    tx_adj /= 6;

    /* Since this reduces the absolute value, there is no need to saturate. */
  }

  muxmdsp_wr_tx_rate_adj ((word)tx_adj);
} /* rlgc_set_tx_rate_adj */


#ifdef FEATURE_MAX_PWR_PROTECTION
#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
FUNCTION RPC_GET_SCH_SCRM_RATE

DESCRIPTION
  For the given condition, this function returns the suitable R_SCH rate which is
  used in the SCRM.

DEPENDENCIES

RETURN VALUE
  R_SCH rate. A rate of zero indicates invalid rate

SIDE EFFECTS
  None.
===========================================================================*/
enc_sch_rate_type rpc_get_sch_scrm_rate
(
 enc_sch_rate_type curr_sch_rate
)
{

  int16  sch2pich, fch2pich, pch_adj_gain, scrm_tx_extra_gain, current_tx_extra_gain;
#ifdef FEATURE_IS2000_P2
  int16  dcch2pich;
#endif /* FEATURE_IS2000_P2 */

  int16  cur_sch_rate_pilot_ref_lvl = 0, proposed_sch_rate_pilot_ref_lvl = 0;
  int16  pilot_ref_lvl_adj = 0;
  txc_gain_type  gain;
  enc_sch_rate_type rate_index;
  rf_txc_chan_config_type  scrm_proposed_chan_config;
  rf_txc_pwr_limit_type    scrm_proposed_pwr_limit;

  /* compute current the extra gain for SCRM without the current config power limit */
  current_tx_extra_gain = (int16) (tx_extra_gain - rf_get_cdma_tx_pwr_limit());

  /* initialize the static chan. configs parameter for the proposed config*/
  scrm_proposed_chan_config.fch_on = txc_rpc.fch_on;
  scrm_proposed_chan_config.fch_rc = txc_rpc.fch_rc;

#ifdef FEATURE_IS2000_P2
  scrm_proposed_chan_config.dcch_on = txc_rpc.dcch_on;
  scrm_proposed_chan_config.dcch_rc = txc_rpc.dcch_rc;
#endif /* FEATURE_IS2000_P2 */

  scrm_proposed_chan_config.sch_on = TRUE; /* SCRM assume SCH always on */
  scrm_proposed_chan_config.sch_rc = txc_rpc.sch_rc;
  /* indicate to RF this is a valid config */
  scrm_proposed_chan_config.backoff_config = BACKOFF_CHAN_CONFIG_VALID;

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
  if ((txc_rpc.dcch_on) && (txc_rpc.fch_on))
  {
    for (rate_index = (curr_sch_rate); rate_index >= ENC_SCH_FULL_RATE; rate_index--) //lint !e734 Loss of precision - 31 to 8 bits
    {
      /* update the multi-channel gain for diff. rate */
      txc_rpc.fch_multi_chan_adj_gain =
        rpc_get_multi_chan_adj_gain (rate_index, txc_rpc.fch_rate_set);
      /* to account for the worst case. we need to
      ** get FCH to PCH gain ratio for R-FCH FULL rate
      */
      fch2pich = rpc_get_fch2pich_gain_ratio(ENC_FULL_RATE, FALSE);

      /* compute the gain for DCCH */
      /* update the multi-channel gain for diff. rate */
      txc_rpc.dcch_multi_chan_adj_gain =
          rpc_get_multi_chan_adj_gain (rate_index, txc_rpc.dcch_rate_set);
      dcch2pich = rpc_get_dcch2pich_gain_ratio (FALSE);

      /* compute R_SCH to PCH again ratio for a given R_SCH rate */
      sch2pich = rpc_get_sch2pich_gain_ratio (rate_index, txc_rpc.sch_code_type);

      /* compute DCCH/FCH, PCH gains when R-FCH,R-DCCH and R-SCH is assigned and
      ** R-SCH is ACTIVE
      */
      gain = rpc_compute_gains (fch2pich,sch2pich,dcch2pich,
                                 FCH_GAIN | SCH_GAIN | DCCH_GAIN, RPC_TOT_DIG_PWR_255);

      MSG_HIGH ("fch2pich %d sch2pich %d dcch2pich %d",
                  fch2pich, sch2pich, dcch2pich);

      /* compute adjusted PCH gain */
      pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));

      /* Initialize the SCH proposed rate then call the RF API function
      to get the power limit and adjustment for that rate*/
      scrm_proposed_chan_config.sch_rate = rate_index;
      /* check of the SCH to PCH ratio is bigger than 6 dB*/
      if (sch2pich > 48) /* SCH2PCH in the unit of 1/8 Db, so 6*8 = 48 */
      {
        scrm_proposed_chan_config.sch_to_pch_over_6dB = TRUE;
      }
      else
      {
        scrm_proposed_chan_config.sch_to_pch_over_6dB = FALSE;
      }
      rf_get_tx_pwr_limit_adj(&scrm_proposed_chan_config, &scrm_proposed_pwr_limit);

      /* compute the extra gain for the proposed config. */
      scrm_tx_extra_gain = current_tx_extra_gain + (int16) scrm_proposed_pwr_limit.tx_pwr_limit
                                + scrm_proposed_pwr_limit.tx_pwr_limit_adj;

      if (txc_rpc.sch_on)
      {
        cur_sch_rate_pilot_ref_lvl =
           rpc_get_sch_pilot_ref_level (txc_rpc.sch_rate, txc_rpc.sch_code_type);
      }

      proposed_sch_rate_pilot_ref_lvl =
         rpc_get_sch_pilot_ref_level (rate_index, txc_rpc.sch_code_type);

      pilot_ref_lvl_adj = proposed_sch_rate_pilot_ref_lvl -
                          cur_sch_rate_pilot_ref_lvl;

      MSG_HIGH("SCRM rfpl %d,eg %d,plra %d",scrm_proposed_pwr_limit.tx_pwr_limit,
                         scrm_tx_extra_gain, pilot_ref_lvl_adj);

       /* convert to 1/12dB
      */
      pilot_ref_lvl_adj *= 3;
      pilot_ref_lvl_adj /= 2;
      /* adjust power for the pilot reference level between the propose rate
      ** and the current rate. This is to take into account setpoint changes
      ** at BS due to change in SCH rate.
      ** Allow an error margin.
      */
      if (-(pch_adj_gain - rlgc_pch_adj_gain_margin) < (scrm_tx_extra_gain - pilot_ref_lvl_adj))
      {
        MSG_HIGH("NEP,lower from %d rate, to %d rate ", curr_sch_rate,rate_index,0);
        txc_rpc.fch_multi_chan_adj_gain = 0;
        return(rate_index);
      }
    }
  }
  else if (txc_rpc.fch_on)
#else
  if (txc_rpc.fch_on)
#endif /* FEATURE_IS2000_REL_A_SVD  and FEATURE_IS2000_P2*/
  {
    for (rate_index = (curr_sch_rate); rate_index >= ENC_SCH_FULL_RATE; rate_index--) //lint !e734 Loss of precision - 31 to 8 bits
    {
      /* update the multi-channel gain for diff. rate */
      txc_rpc.fch_multi_chan_adj_gain =
          rpc_get_multi_chan_adj_gain ( (enc_sch_rate_type) rate_index, txc_rpc.fch_rate_set);
      /* to account for the worst case. we need to
      ** get FCH to PCH gain ratio for R-FCH FULL rate
      */
      fch2pich = rpc_get_fch2pich_gain_ratio(ENC_FULL_RATE, FALSE);

      /* compute R_SCH to PCH again ratio for a given R_SCH rate */
      sch2pich = rpc_get_sch2pich_gain_ratio ( (enc_sch_rate_type) rate_index, txc_rpc.sch_code_type);

      /* compute FCH, PCH gains when R-FCH and R-SCH is assigned and
      ** R-SCH is ACTIVE
      */
      gain = rpc_compute_gains (fch2pich, sch2pich, 0,
                                   FCH_GAIN | SCH_GAIN, RPC_TOT_DIG_PWR_255);

      MSG_HIGH ("fch2pich %d sch2pich %d mulchgain %d",
                  fch2pich, sch2pich, txc_rpc.fch_multi_chan_adj_gain);

      /* compute adjusted PCH gain */
      pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));

      /* Initialize the SCH proposed rate then call the RF API function
      to get the power limit and adjustment for that rate*/
      scrm_proposed_chan_config.sch_rate = rate_index;
      /* check of the SCH to PCH ratio is bigger than 6 dB*/
      if (sch2pich > 48) /* SCH2PCH in the unit of 1/8 Db, so 6*8 = 48 */
      {
        scrm_proposed_chan_config.sch_to_pch_over_6dB = TRUE;
      }
      else
      {
        scrm_proposed_chan_config.sch_to_pch_over_6dB = FALSE;
      }
      rf_get_tx_pwr_limit_adj(&scrm_proposed_chan_config, &scrm_proposed_pwr_limit);

      /* compute the extra gain for the proposed config. */
      scrm_tx_extra_gain = current_tx_extra_gain + (int16) scrm_proposed_pwr_limit.tx_pwr_limit
                                + scrm_proposed_pwr_limit.tx_pwr_limit_adj;

      if (txc_rpc.sch_on)
      {
        cur_sch_rate_pilot_ref_lvl =
           rpc_get_sch_pilot_ref_level (txc_rpc.sch_rate, txc_rpc.sch_code_type);
      }

      proposed_sch_rate_pilot_ref_lvl =
         rpc_get_sch_pilot_ref_level ( (enc_sch_rate_type) rate_index, txc_rpc.sch_code_type);

      pilot_ref_lvl_adj = proposed_sch_rate_pilot_ref_lvl -
                          cur_sch_rate_pilot_ref_lvl;

      MSG_HIGH("SCRM rfpl %d,eg %d,plra %d",scrm_proposed_pwr_limit.tx_pwr_limit,
                         scrm_tx_extra_gain, pilot_ref_lvl_adj);

      /* convert to 1/12dB
      */
      pilot_ref_lvl_adj *= 3;
      pilot_ref_lvl_adj /= 2;
      /* adjust power for the pilot reference level between the propose rate
      ** and the current rate. This is to take into account setpoint changes
      ** at BS due to change in SCH rate.
      ** Allow an error margin.
      */
      if (-(pch_adj_gain - rlgc_pch_adj_gain_margin) < (scrm_tx_extra_gain - pilot_ref_lvl_adj))
      {
        MSG_HIGH("NEP,lower from %d rate, to %d rate ", curr_sch_rate,rate_index,0);
        txc_rpc.fch_multi_chan_adj_gain = 0;
        return( (enc_sch_rate_type) rate_index);
      }
    }
  }
#ifdef FEATURE_IS2000_P2
  else if (txc_rpc.dcch_on)
  {
    for (rate_index = (curr_sch_rate); rate_index >= ENC_SCH_FULL_RATE; rate_index--) //lint !e734 Loss of precision - 31 to 8 bits
    {
      /* update the multi-channel gain for diff. rate */
      txc_rpc.dcch_multi_chan_adj_gain =
          rpc_get_multi_chan_adj_gain ( (enc_sch_rate_type) rate_index, txc_rpc.dcch_rate_set);
      dcch2pich = rpc_get_dcch2pich_gain_ratio (FALSE);

      /* compute R_SCH to PCH again ratio for a given R_SCH rate */
      sch2pich = rpc_get_sch2pich_gain_ratio ( (enc_sch_rate_type) rate_index, txc_rpc.sch_code_type);


      /* R-DCCH and R-SCH assigned and both ACTIVE */
      gain = rpc_compute_gains (0, sch2pich, dcch2pich,
                                DCCH_GAIN | SCH_GAIN, RPC_TOT_DIG_PWR_255);

      MSG_HIGH ("dcch2pich %d sch2pich %d mulchgain %d",
                  dcch2pich, sch2pich, txc_rpc.dcch_multi_chan_adj_gain);

      /* compute adjusted PCH gain */
      pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));

      /* Initialize the SCH proposed rate then call the RF API function
      to get the power limit and adjustment for that rate*/
      scrm_proposed_chan_config.sch_rate = rate_index;
      /* check of the SCH to PCH ratio is bigger than 6 dB*/
      if (sch2pich > 48) /* SCH2PCH in the unit of 1/8 Db, so 6*8 = 48 */
      {
        scrm_proposed_chan_config.sch_to_pch_over_6dB = TRUE;
      }
      else
      {
        scrm_proposed_chan_config.sch_to_pch_over_6dB = FALSE;
      }
      rf_get_tx_pwr_limit_adj(&scrm_proposed_chan_config, &scrm_proposed_pwr_limit);

      /* compute the extra gain for the proposed config. */
      scrm_tx_extra_gain = current_tx_extra_gain + (int16) scrm_proposed_pwr_limit.tx_pwr_limit
                                + scrm_proposed_pwr_limit.tx_pwr_limit_adj;

      if (txc_rpc.sch_on)
      {
        cur_sch_rate_pilot_ref_lvl =
           rpc_get_sch_pilot_ref_level (txc_rpc.sch_rate, txc_rpc.sch_code_type);
      }

      proposed_sch_rate_pilot_ref_lvl =
         rpc_get_sch_pilot_ref_level ( (enc_sch_rate_type) rate_index, txc_rpc.sch_code_type);

      pilot_ref_lvl_adj = proposed_sch_rate_pilot_ref_lvl -
                          cur_sch_rate_pilot_ref_lvl;

      MSG_HIGH("SCRM rfpl %d,eg %d,plra %d",scrm_proposed_pwr_limit.tx_pwr_limit,
                         scrm_tx_extra_gain, pilot_ref_lvl_adj);


      /* convert to 1/12dB
      */
      pilot_ref_lvl_adj *= 3;
      pilot_ref_lvl_adj /= 2;

      /* adjust power for the pilot reference level between the propose rate
      ** and the current rate. This is to take into account setpoint changes
      ** at BS due to change in SCH rate.
      ** Allow an error margin.
      */
      if (-(pch_adj_gain - rlgc_pch_adj_gain_margin) < (scrm_tx_extra_gain - pilot_ref_lvl_adj))
      {
         MSG_HIGH("NEP,lower from %d rate, to %d rate ", curr_sch_rate,rate_index,0);
         txc_rpc.dcch_multi_chan_adj_gain = 0;
         return( (enc_sch_rate_type) rate_index);
      }
    }
  }
#endif /* FEATURE_IS2000_P2 */
  else
  {
    MSG_FATAL("Bad call set up ", 0, 0, 0);
    return ( (enc_sch_rate_type) 0); /* return invalid rate */
  }

  /* If excecute this line, then the MS does not have enough power to bring up R_SCH */
  MSG_ERROR("Need more pwr to bring up R_SCH ", 0, 0, 0);
  txc_rpc.fch_multi_chan_adj_gain = 0;
#ifdef FEATURE_IS2000_P2
  txc_rpc.dcch_multi_chan_adj_gain = 0;
#endif
  return ( (enc_sch_rate_type) 0); /* return invalid rate */

}
#endif /* FEATURE_IS2000_R_SCH */
#endif /* FEATURE_MAX_PWR_PROTECTION */


/*===========================================================================
                      } PRIVATE FUNCTIONS {
===========================================================================*/






/****************************************************************
 ****************************************************************
 *                      Gain Ratio Functions {                  *
 ****************************************************************
 ****************************************************************/



/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_FCH2PICH_GAIN_RATIO

DESCRIPTION
  This function computes the gain ratio between R-FCH and R-PICH.
  See section 2.1.2.3.3.7 of the rev D physical layer standard.

DEPENDENCIES
  This function should be called after RLGC parameters have been initialized.

RETURN VALUE
  FCH/PICH Gain Ratio.

SIDE EFFECTS
  None.
===========================================================================*/
int2 rpc_get_fch2pich_gain_ratio
(
  byte rate,
  boolean fch_only
)
{
   int16  nom_att_gain;
   int16  att_adj_gain;
   int16  multi_chan_adj_gain;

   /* Retrieve the nominal attribute gain and the attribute adjustment
    * gain. */

#ifdef FEATURE_PLT
#error code not present
#else
   if (txc_rpc.fch_rate_set == ENC_RATE_9600)
#endif
   {
      /* Rate Set 1 */
      nom_att_gain = basic_rs1_rl_nom_att_gains[rate].nom_att_gain;
      if ((rate == ENC_8TH_RATE) && (txc_rpc.rev_fch_gating_on))
      {
         nom_att_gain = gated_rs1_rl_nom_att_gains.nom_att_gain;
      }
      att_adj_gain = basic_rs1_rl_att_adj_gains[rate].nom_att_gain;
   }
   else
   {
      /* Rate Set 2 */
      nom_att_gain = basic_rs2_rl_nom_att_gains[rate].nom_att_gain;
      if ((rate == ENC_8TH_RATE) && (txc_rpc.rev_fch_gating_on))
      {
         nom_att_gain = gated_rs2_rl_nom_att_gains.nom_att_gain;
      }
      att_adj_gain = basic_rs2_rl_att_adj_gains[rate].nom_att_gain;
   }

   /* Determine multiple channel adjustment */
   multi_chan_adj_gain = (fch_only ? 0 : txc_rpc.fch_multi_chan_adj_gain);

   /* Calculate the FCH/PICH ratio.
    *
    * If default_rlag is TRUE, RL_ATT_ADJ_GAIN, fch_chan_adj_gain,
    * is reset to 0
    */

   return (nom_att_gain
          + att_adj_gain
          + txc_rpc.fch_chan_adj_gain
          - multi_chan_adj_gain
          + txc_rpc.rlgain_traffic_pilot);

} /* rpc_get_fch2pich_gain_ratio */


#ifdef FEATURE_IS2000_P2
/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_DCCH2PICH_GAIN_RATIO

DESCRIPTION
  This function computes the gain ratio between R-DCCH and R-PICH.
  See section 2.1.2.3.3.7 of the rev D physical layer standard.

DEPENDENCIES
  This function should be called after RLGC parameters have been initialized.

RETURN VALUE
  DCCH/PICH Gain Ratio.

SIDE EFFECTS
  None.
===========================================================================*/
int2 rpc_get_dcch2pich_gain_ratio
(
  boolean dcch_only
)
{
   int16  nom_att_gain;
   int16  att_adj_gain;
   int16  multi_chan_adj_gain;

   /* Retrieve the nominal attribute gain and the attribute adjustment
    * gain. */

#ifdef FEATURE_PLT
#error code not present
#else
   if (txc_rpc.dcch_rate_set == ENC_RATE_9600)
#endif
   {
      /* Rate Set 1 */
      nom_att_gain = basic_rs1_rl_nom_att_gains[ENC_FULL_RATE].nom_att_gain;
      att_adj_gain = basic_rs1_rl_att_adj_gains[ENC_FULL_RATE].nom_att_gain;
   }
   else
   {
      /* Rate Set 2 */
      nom_att_gain = basic_rs2_rl_nom_att_gains[ENC_FULL_RATE].nom_att_gain;
      att_adj_gain = basic_rs2_rl_att_adj_gains[ENC_FULL_RATE].nom_att_gain;
   }

   /* Determine multiple channel adjustment */
   multi_chan_adj_gain = (dcch_only ? 0 : txc_rpc.dcch_multi_chan_adj_gain);

   /* Calculate the DCCH/PICH ratio.
    *
    * If default_rlag is TRUE, RL_ATT_ADJ_GAIN, dcch_chan_adj_gain,
    * is reset to 0
    */

   return (nom_att_gain
          + att_adj_gain
          + txc_rpc.dcch_chan_adj_gain
          - multi_chan_adj_gain
          + txc_rpc.rlgain_traffic_pilot);

} /* rpc_get_dcch2pich_gain_ratio */
#endif /* FEATURE_IS2000_P2 */



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
)
{
   int16 nom_att_gain = 0;
   int16 att_adj_gain = 0;
   int16  multi_chan_adj_gain;

   /* Determine multiple channel adjustment */
   multi_chan_adj_gain = txc_rpc.sch_multi_chan_adj_gain;

   /* Retrieve the nominal attribute gain and the attribute adjustment
    * gain. */

   if (rate >= ENC_SCH_2X_RATE)
   {
     uint8 index = (uint8)(RLGC_UPPER_BOUNDS_CHECK(
                            rate - ENC_SCH_2X_RATE, Max_Num_Of_High_Rates));
      /*  reduce the enum rate down to within the bounds of
          the gains arrays */

       /* Rate is 2x or greater.  Use high rate tables */
#ifdef FEATURE_DS_IS2000_RS2
      if (txc_rpc.sch_rc == CAI_RC_4)
      {
         /* Rate Set 2 */
         nom_att_gain = high_rs2_rl_nom_att_gains[code_type][index].nom_att_gain;
         att_adj_gain = high_rs2_rl_att_adj_gains[code_type][index].nom_att_gain;
      }
      else
#endif /* FEATURE_DS_IS2000_RS2 */
      {
         /* Rate Set 1 */
         nom_att_gain = high_rs1_rl_nom_att_gains[code_type]
                  [index].nom_att_gain;
         att_adj_gain = high_rs1_rl_att_adj_gains[code_type]
                  [index].nom_att_gain;
      }
   }
   else if (rate == ENC_SCH_FULL_RATE)
   {

      /* Rate is 1x.
       * For 1x R-SCH only Convolutional code is supported.
       * Indexing into the gain table for RS1 fractional rates. Use the
       * gain values corresponding to FULL (9600) rate.
       */
#ifdef FEATURE_DS_IS2000_RS2
      if (txc_rpc.sch_rc == CAI_RC_4)
      {
         /* Rate Set 2 */
         nom_att_gain = basic_rs2_rl_nom_att_gains[ENC_FULL_RATE].nom_att_gain;
         att_adj_gain = basic_rs2_rl_att_adj_gains[ENC_FULL_RATE].nom_att_gain;
      }
      else
#endif /* FEATURE_DS_IS2000_RS2 */
      {
         /* Rate Set 1 */
         nom_att_gain = basic_rs1_rl_nom_att_gains[ENC_FULL_RATE].nom_att_gain;
         att_adj_gain = basic_rs1_rl_att_adj_gains[ENC_FULL_RATE].nom_att_gain;
      }
   }

   /* Calculate the SCH/PICH ratio.
    *
    * If default_rlag is TRUE, RL_ATT_ADJ_GAIN and SCH0_CHAN_ADJ_GAIN
    * already initialized to 0
    */

   return (nom_att_gain
          + att_adj_gain
          + txc_rpc.sch0_chan_adj_gain
          - multi_chan_adj_gain
          + txc_rpc.rlgain_traffic_pilot
          + txc_rpc.rlgain_sch0_pilot);

} /* rpc_get_sch2pich_gain_ratio */

#endif /* FEATURE_IS2000_R_SCH */


/****************** Gain Ratio Functions } *******************/





/****************************************************************
 ****************************************************************
 *                  Initialization Functions {                  *
 ****************************************************************
 ****************************************************************/


/* <EJECT> */
/*===========================================================================
FUNCTION RPC_INIT_ATT_ADJ_GAIN

DESCRIPTION
  Initializes all Attribute Adjustment Gains arrays to zeros.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_init_att_adj_gain (void)
{
   byte rate;
   rl_att_gain_type att_adj_gain = {0, 0};

   for (rate = (byte) ENC_FULL_RATE; rate < ENC_NUM_FRAC_RATES; rate++)
   {
      basic_rs1_rl_att_adj_gains[rate] = att_adj_gain;
      basic_rs2_rl_att_adj_gains[rate] = att_adj_gain;
   }

   for (rate = 0; rate < Max_Num_Of_High_Rates; rate++)
   {
      high_rs1_rl_att_adj_gains[0][rate] = att_adj_gain;
      high_rs1_rl_att_adj_gains[1][rate] = att_adj_gain;
#ifdef FEATURE_DS_IS2000_RS2
      high_rs2_rl_att_adj_gains[0][rate] = att_adj_gain;
      high_rs2_rl_att_adj_gains[1][rate] = att_adj_gain;
#endif /* FEATURE_DS_IS2000_RS2 */
   }
} /* rpc_init_att_adj_gain */

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_INIT_CHAN_ADJ_GAIN

DESCRIPTION
  Initializes Cahnnel Adjust Gains for FCH, DCCH, SCH0, SCH1

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_init_chan_adj_gain (void)
{
  txc_rpc.fch_chan_adj_gain  = 0;
#ifdef FEATURE_IS2000_P2
  txc_rpc.dcch_chan_adj_gain = 0;
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_R_SCH
  txc_rpc.sch0_chan_adj_gain = 0;
#endif /* FEATURE_IS2000_R_SCH */
  txc_rpc.sch1_chan_adj_gain = 0;
} /* rpc_init_chan_adj_gain */


/**************** End Initialization Functions } ****************/


/****************************************************************
 ****************************************************************
 *                  Digital Gain Computations {                 *
 ****************************************************************
 ****************************************************************/


/* <EJECT> */
/*===========================================================================
FUNCTION RPC_COMPUTE_GAINS

DESCRIPTION
  Computes the relative gains for FCH, PCH, SCH.  Resulting gains have 
  15 bit resolution to output to MDSP.

DEPENDENCIES


RETURN VALUE
  Relative gains for FCH, PCH, SCH.

SIDE EFFECTS
  None.
===========================================================================*/
txc_gain_type rpc_compute_gains
(
 int16 fch,
 int16 sch,
 int16 dcch,
 byte  gain_mask,
 byte  tot_dig_pwr
)
{
   word   gp_temp, gf_temp = 0, gs_temp = 0, gd_temp = 0;
   word   max, final_scale;
   dword  power, prod, remainder, sqroot;
   byte   binary_precision = 0;
   txc_gain_type gain;
   
   /* gp_temp = 1.0 in 16Q11 format */
   gp_temp = 1 << 11;

   if (gain_mask & FCH_GAIN_M)
   {
     /* Convert R-FCH decibel gain to linear gain.
     ** fch is in range of -133 to 144.
     ** gf_temp = convert_dBq8_to_lin ((((fch / 8) * 2^8) / 2), 11, 16)
     **        OR convert_dBq8_to_lin((fch << 4) + bias, 11, 16)
     ** bias (units of 1/256 dB) of -0.0089 or -2 in Q8 is used to offset 
     ** RF non-linear effects inherent in the dB2lin function.
     */
     gf_temp = (word) convert_dBq8_to_lin
       (((fch<<4) + RF_NON_LINEAR_BIAS_CORRECTION), 11, 16);
   }

#ifdef FEATURE_IS2000_P2
   if (gain_mask & DCCH_GAIN_M)
   {
     gd_temp = (word) convert_dBq8_to_lin 
       (((dcch<<4) + RF_NON_LINEAR_BIAS_CORRECTION), 11, 16);
   }
#endif /* FEATURE_IS2000_P2 */

   if (gain_mask & SCH_GAIN_M)
   {
     gs_temp = (word) convert_dBq8_to_lin
       (((sch<<4) + RF_NON_LINEAR_BIAS_CORRECTION), 11, 16);
   }

   max = rpc_max_gain (gp_temp, gf_temp, gs_temp, gd_temp);

   /* power = 1.0 + gf_temp^2 + gs_temp^2  (in 32Q22 format) */
   power = (1 << 22) + (gf_temp * gf_temp) + (gs_temp * gs_temp);

#ifdef FEATURE_IS2000_P2
   /* power += gd_temp^2 (in 32Q22 format) */
   power += (gd_temp * gd_temp);
#endif /* FEATURE_IS2000_P2 */

   while (power < (1 << 30))
   {
      power <<= 2;
      binary_precision++;
   }

   sqroot = rpc_sqrt32 (power);

   final_scale = (word) (((1 << (22 + binary_precision)) + (sqroot >> 1)) / sqroot);

   if ((max * final_scale) > (1 << 22))
   {
      final_scale--;
   }

   prod = gp_temp * final_scale * tot_dig_pwr;
   gain.pch_gain = (word) (prod >> 15);
   remainder = prod % (1 << 15);
   if ((remainder << 1) >= (1 << 15))
   {
      gain.pch_gain++;
   }

   gain.fch_gain = 0;
   if (gain_mask & FCH_GAIN_M)
   {
      prod = gf_temp * final_scale * tot_dig_pwr;
      gain.fch_gain = (word) (prod >> 15);
      remainder = prod % (1 << 15);
      if ((remainder << 1) >= (1 << 15))
      {
         gain.fch_gain++;
      }
   }

   gain.sch_gain = 0;
   if (gain_mask & SCH_GAIN_M)
   {
      prod = gs_temp * final_scale * tot_dig_pwr;
      gain.sch_gain = (word) (prod >> 15);
      remainder = prod % (1 << 15);
      if ((remainder << 1) >= (1 << 15))
      {
         gain.sch_gain++;
      }
   }

#ifdef FEATURE_IS2000_P2
   gain.dcch_gain = 0;
   if (gain_mask & DCCH_GAIN_M)
   {
      prod = gd_temp * final_scale * tot_dig_pwr;
      gain.dcch_gain = (word) (prod >> 15);
      remainder = prod % (1 << 15);
      if ((remainder << 1) >= (1 << 15))
      {
         gain.dcch_gain++;
      }
   }
#endif /* FEATURE_IS2000_P2 */

   return gain;
} /* rpc_compute_gains */


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
)
{
   int16  pch_adj_gain;

   /* First entry in rpc_gain_adj_tab[] corresponds
   ** to a PCH gain of TABLE_OFFSET
   */
   if (pch_gain < TABLE_OFFSET)
   {
      pch_adj_gain = rpc_gain_adj_tab [0];
   }
   else
   {
      pch_adj_gain = rpc_gain_adj_tab [pch_gain - TABLE_OFFSET];
   }

   /* Since LNA mode is 85.3 dB, scaling factor is 12/13. Convert
   ** the adjusted gain to a 9-bit signed 2's compliment number.
   */
   pch_adj_gain = ((pch_adj_gain * 12) / 13) * -1; //lint !e734 Loss of precision 20 to 15 bits

   return pch_adj_gain;
} /* rpc_get_pch_adj_gain */





#ifdef FEATURE_IS2000_REL_A_CC

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_COMPUTE_REACH_GAINS

DESCRIPTION
  Computes the relative gains for EACH, PCH.
  Run-time efficiency could be improved at the cost of space by storing results
  for all rate_words for the last rlgain_common_pilot value.  Similarly, could
  precompute values for rlgain_common_pilot==0 and store in a table.

DEPENDENCIES

RETURN VALUE
  Relative gains for EACH, PCH

SIDE EFFECTS
  None.
===========================================================================*/
rpc_reach_gain_type rpc_compute_reach_gains
(
  txc_each_rate_type rate_word,
  int16 rlgain_common_pilot,
  byte  tot_dig_pwr
)
{
  int16  each;
  word   gp_temp, ge_temp = 0;  // Temporary linear gains
  word   max, final_scale;
  dword  power, prod, remainder, sqroot;
  byte   binary_precision = 0;
  rpc_reach_gain_type gain;

  rlgain_common_pilot = (int16) (
                          (rlgain_common_pilot & 0x0020)?
                          (rlgain_common_pilot | 0xFFC0) :
                          (rlgain_common_pilot & 0x003F)
                        );

  // For calculation of each relative gain, see section 2.1.2.3.3.1
  // Lookup standard specified ratio of power of R-EACH to R-PICH.
  each = (int16) rpc_nominal_RCCH_attribute_gain_table[rate_word];
  // Adjust gain of EACH relative to reverse pilot
  each += rlgain_common_pilot;

  // Pilot gain is constant 1.0 in 16Q11 format
  gp_temp = 1 << 11;

  /* Convert R-EACH decibel gain to linear gain.
  ** each is in range of -133 to 144.
  ** ge_temp = convert_dBq8_to_lin ((((each / 8) * 2^8) / 2), 11, 16)
  **        OR convert_dBq8_to_lin((each << 4) + bias, 11, 16)
  ** bias (units of 1/256 dB) of -0.0089 or -2 in Q8 is used to offset 
  ** RF non-linear effects inherent in the dB2lin function.
  */
  ge_temp = (word) convert_dBq8_to_lin
    (((each<<4) + RF_NON_LINEAR_BIAS_CORRECTION), 11, 16);

  // Determine max(R-EACH, R-PICH)
  max = (gp_temp > ge_temp) ? gp_temp : ge_temp;

  // Calculate the sum of (R-PICH)^2 + (R-EACH)^2
  // Since R-PICH is 1<<11, (R-PICH)^2 is 1<<22 (32Q22 format)
  power = (1 << 22) + (ge_temp * ge_temp);

  // Increase the significant bits in the power in advance of taking
  // the square root.  Each time the power is multiplied by 4 the
  // binary_precision is incremented by one.  This is because power
  // is the sum of squares.  (2A)^2 + (2B)^2 = 4 (A^2 + B^2).  Hence,
  // Shifting power by 2, and incrementing binary_precision by one.
  while (power < (1 << 30))
  {
    power <<= 2;
    binary_precision++;
  }

  // Take the square root of power.
  sqroot = rpc_sqrt32 (power);

  // 2^15 is a scaling factor chosen to minimize the trucation error.  It is
  // offset by binary_precision to correct for the manipulation of power above.
  final_scale = (word) (((1 << (22 + binary_precision)) + (sqroot >> 1)) / sqroot);

  // Test the range of the maximal digital gain and reduce the value of scale
  // by 1 when the maximal digital gain is over 255.
  if ((max * final_scale) > (1 << 22))
  {
    final_scale--;
  }

  // Calculate R-PICH gain
  prod = gp_temp * final_scale * tot_dig_pwr;
  gain.pch_gain = (word) (prod >> 15);
  remainder = prod % (1 << 15);
  if ((remainder << 1) >= (1 << 15))
  {
    gain.pch_gain++;
  }

  // Calculate R-EACH gain
  prod = ge_temp * final_scale * tot_dig_pwr;
  gain.each_gain = (word) (prod >> 15);
  remainder = prod % (1 << 15);
  if ((remainder << 1) >= (1 << 15))
  {
    gain.each_gain++;
  }

  return gain;
} /* rpc_compute_reach_gains */
#endif // FEATURE_IS2000_REL_A_CC

/*************** End Digital Gain Computations } ****************/




/****************************************************************
 ****************************************************************
 *                  L3 Table Update Functions {                 *
 ****************************************************************
 ****************************************************************/



/* <EJECT> */
/*===========================================================================
FUNCTION RPC_COPY_ADJ0_REC_PARAMS

DESCRIPTION
  Copies adj0_rec parameters from L3 message into internal data sturctures.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_copy_adj0_rec_params
(
   caii_rpc_adj0_type *adj0_rec
)
{
   if (adj0_rec->fch_incl)
   {
      txc_rpc.fch_chan_adj_gain  = (int8) adj0_rec->fch_chan_adj_gain;
   }

#ifdef FEATURE_IS2000_P2
   if (adj0_rec->dcch_incl)
   {
      txc_rpc.dcch_chan_adj_gain = (int8) adj0_rec->dcch_chan_adj_gain;
   }
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_R_SCH
   if (adj0_rec->sch0_incl)
   {
      txc_rpc.sch0_chan_adj_gain = (int8) adj0_rec->sch0_chan_adj_gain;
   }
#endif /* FEATURE_IS2000_R_SCH */

   if (adj0_rec->sch1_incl)
   {
      txc_rpc.sch1_chan_adj_gain = (int8) adj0_rec->sch1_chan_adj_gain;
   }

} /* rpc_copy_adj0_rec_params */



/* <EJECT> */
/*===========================================================================
FUNCTION RPC_COPY_ADJ1_REC_PARAMS

DESCRIPTION
  Copies adj1_rec parameters from L3 message into internal data sturctures.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_copy_adj1_rec_params
(
   caii_rpc_adj1_type *adj1_rec
)
{
   if (adj1_rec->rl_att_adj_gain_type)
   {
      /* adj to pilot reference level */

      if (adj1_rec->rc3_rc5_20ms_incl)
      {
         basic_rs1_rl_att_adj_gains[ENC_FULL_RATE].pilot_ref_lvl =
            adj1_rec->rl_att_adj_gain_9600;

         basic_rs1_rl_att_adj_gains[ENC_HALF_RATE].pilot_ref_lvl =
            adj1_rec->rl_att_adj_gain_4800;

         basic_rs1_rl_att_adj_gains[ENC_QTR_RATE].pilot_ref_lvl =
            adj1_rec->rl_att_adj_gain_2700;

         basic_rs1_rl_att_adj_gains[ENC_8TH_RATE].pilot_ref_lvl =
            adj1_rec->rl_att_adj_gain_1500;
      }

      if (adj1_rec->rc4_rc6_20ms_incl)
      {
         basic_rs2_rl_att_adj_gains[ENC_FULL_RATE].pilot_ref_lvl =
            adj1_rec->rl_att_adj_gain_14400;

         basic_rs2_rl_att_adj_gains[ENC_HALF_RATE].pilot_ref_lvl =
            adj1_rec->rl_att_adj_gain_7200;

         basic_rs2_rl_att_adj_gains[ENC_QTR_RATE].pilot_ref_lvl =
            adj1_rec->rl_att_adj_gain_3600;

         basic_rs2_rl_att_adj_gains[ENC_8TH_RATE].pilot_ref_lvl =
            adj1_rec->rl_att_adj_gain_1800;
      }
   }
   else
   {
      /* adj to nominal attribute gain */

      if (adj1_rec->rc3_rc5_20ms_incl)
      {
         basic_rs1_rl_att_adj_gains[ENC_FULL_RATE].nom_att_gain =
            (int8) adj1_rec->rl_att_adj_gain_9600;

         basic_rs1_rl_att_adj_gains[ENC_HALF_RATE].nom_att_gain =
            (int8) adj1_rec->rl_att_adj_gain_4800;

         basic_rs1_rl_att_adj_gains[ENC_QTR_RATE].nom_att_gain =
            (int8) adj1_rec->rl_att_adj_gain_2700;

         basic_rs1_rl_att_adj_gains[ENC_8TH_RATE].nom_att_gain =
            (int8) adj1_rec->rl_att_adj_gain_1500;
      }

      if (adj1_rec->rc4_rc6_20ms_incl)
      {
         basic_rs2_rl_att_adj_gains[ENC_FULL_RATE].nom_att_gain =
            (int8) adj1_rec->rl_att_adj_gain_14400;

         basic_rs2_rl_att_adj_gains[ENC_HALF_RATE].nom_att_gain =
            (int8) adj1_rec->rl_att_adj_gain_7200;

         basic_rs2_rl_att_adj_gains[ENC_QTR_RATE].nom_att_gain =
            (int8) adj1_rec->rl_att_adj_gain_3600;

         basic_rs2_rl_att_adj_gains[ENC_8TH_RATE].nom_att_gain =
            (int8) adj1_rec->rl_att_adj_gain_1800;
      }
   }
} /* rpc_copy_adj1_rec_params */



/* <EJECT> */
/*===========================================================================
FUNCTION RPC_COPY_ADJ2_REC_PARAMS

DESCRIPTION
  Copies adj2_rec parameters from L3 message into internal data sturctures.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_copy_adj2_rec_params
(
   caii_rpc_adj2_type *adj2_rec
)
{
   if (adj2_rec->code_type)
   {
#ifdef MSMHW_TURBO_CODING
      /* TURBO codes */

      if (adj2_rec->rl_att_adj_gain_type)
      {
         /* adj to pilot reference level */

         if (adj2_rec->rc3_rc5_20ms_incl)
         {
#ifdef FEATURE_32X_SCH_DATA_RATE
            high_rs1_rl_att_adj_gains[1][RPC_32X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_307200;
#endif /* FEATURE_32X_SCH_DATA_RATE */

            high_rs1_rl_att_adj_gains[1][RPC_16X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_153600;

            high_rs1_rl_att_adj_gains[1][RPC_8X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_76800;

            high_rs1_rl_att_adj_gains[1][RPC_4X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_38400;

            high_rs1_rl_att_adj_gains[1][RPC_2X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_19200;
         }

         if (adj2_rec->rc4_rc6_20ms_incl)
         {
#ifdef FEATURE_DS_IS2000_RS2
            high_rs2_rl_att_adj_gains[1][RPC_16X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_230400;

            high_rs2_rl_att_adj_gains[1][RPC_8X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_115200;

            high_rs2_rl_att_adj_gains[1][RPC_4X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_57600;

            high_rs2_rl_att_adj_gains[1][RPC_2X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_28800;
#else
            MSG_ERROR("RS2 for rec type 2 in PCNM not supp", 0, 0, 0 );
#endif /* FEATURE_DS_IS2000_RS2 */
         }
      }
      else
      {
         /* adj to nominal attribute gain */

         if (adj2_rec->rc3_rc5_20ms_incl)
         {
#ifdef FEATURE_32X_SCH_DATA_RATE
            high_rs1_rl_att_adj_gains[1][RPC_32X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_307200;
#endif /* FEATURE_32X_SCH_DATA_RATE */

            high_rs1_rl_att_adj_gains[1][RPC_16X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_153600;

            high_rs1_rl_att_adj_gains[1][RPC_8X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_76800;

            high_rs1_rl_att_adj_gains[1][RPC_4X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_38400;

            high_rs1_rl_att_adj_gains[1][RPC_2X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_19200;
         }

         if (adj2_rec->rc4_rc6_20ms_incl)
         {
#ifdef FEATURE_DS_IS2000_RS2
            high_rs2_rl_att_adj_gains[1][RPC_16X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_230400;

            high_rs2_rl_att_adj_gains[1][RPC_8X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_115200;

            high_rs2_rl_att_adj_gains[1][RPC_4X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_57600;

            high_rs2_rl_att_adj_gains[1][RPC_2X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_28800;

#else
            MSG_ERROR("RS2 for rec type 2 in PCNM not supp", 0, 0, 0 );
#endif /* FEATURE_DS_IS2000_RS2 */
         }
      }
#endif /* MSMHW_TURBO_CODING */
   }
   else
   {
      /* CONVOLUTIONAL codes */

      if (adj2_rec->rl_att_adj_gain_type)
      {
         /* adj to pilot reference level */

         if (adj2_rec->rc3_rc5_20ms_incl)
         {
#ifdef FEATURE_32X_SCH_DATA_RATE
            high_rs1_rl_att_adj_gains[0][RPC_32X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_307200;
#endif /* FEATURE_32X_SCH_DATA_RATE */

            high_rs1_rl_att_adj_gains[0][RPC_16X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_153600;

            high_rs1_rl_att_adj_gains[0][RPC_8X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_76800;

            high_rs1_rl_att_adj_gains[0][RPC_4X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_38400;

            high_rs1_rl_att_adj_gains[0][RPC_2X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_19200;
         }

         if (adj2_rec->rc4_rc6_20ms_incl)
         {
#ifdef FEATURE_DS_IS2000_RS2
            high_rs2_rl_att_adj_gains[0][RPC_16X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_230400;

            high_rs2_rl_att_adj_gains[0][RPC_8X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_115200;

            high_rs2_rl_att_adj_gains[0][RPC_4X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_57600;

            high_rs2_rl_att_adj_gains[0][RPC_2X_RATE].pilot_ref_lvl =
               adj2_rec->rl_att_adj_gain_28800;
#else
            MSG_ERROR("RS2 for rec type 2 in PCNM not supp", 0, 0, 0 );
#endif /* FEATURE_DS_IS2000_RS2 */
         }
      }
      else
      {
         /* adj to nominal attribute gain */

         if (adj2_rec->rc3_rc5_20ms_incl)
         {
#ifdef FEATURE_32X_SCH_DATA_RATE
            high_rs1_rl_att_adj_gains[0][RPC_32X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_307200;
#endif /* FEATURE_32X_SCH_DATA_RATE */

            high_rs1_rl_att_adj_gains[0][RPC_16X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_153600;

            high_rs1_rl_att_adj_gains[0][RPC_8X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_76800;

            high_rs1_rl_att_adj_gains[0][RPC_4X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_38400;

            high_rs1_rl_att_adj_gains[0][RPC_2X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_19200;
         }

         if (adj2_rec->rc4_rc6_20ms_incl)
         {
#ifdef FEATURE_DS_IS2000_RS2
            high_rs2_rl_att_adj_gains[0][RPC_16X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_230400;

            high_rs2_rl_att_adj_gains[0][RPC_8X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_115200;

            high_rs2_rl_att_adj_gains[0][RPC_4X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_57600;

            high_rs2_rl_att_adj_gains[0][RPC_2X_RATE].nom_att_gain =
               (int8) adj2_rec->rl_att_adj_gain_28800;
#else
            MSG_ERROR("RS2 for rec type 2 in PCNM not supp", 0, 0, 0 );
#endif /* FEATURE_DS_IS2000_RS2 */
         }
      }
   }
} /* rpc_copy_adj2_rec_params */


/* <EJECT> */
/*===========================================================================
FUNCTION RPC_INIT_PWR_CTRL_PARAMS

DESCRIPTION
  Copies rpc_recs from L3 message into internal data sturctures.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_init_pwr_ctrl_params
(
  byte  num_rpc_recs,
  caii_rpc_type *rpc_rec
)
{
   byte rec_num;

   if (num_rpc_recs > CAI_MAX_NUM_RPC_RECS)
   {
      MSG_ERROR("Num of recs in PCNM > CAI_MAX_NUM_RPC_RECS", 0, 0, 0 );
      num_rpc_recs = CAI_MAX_NUM_RPC_RECS;
   }

   for (rec_num = 0; rec_num < num_rpc_recs; rec_num++)
   {
      switch (rpc_rec->rpc_adj_rec_type)
      {
         case 0x0000:
            rpc_copy_adj0_rec_params( &(rpc_rec->rpc_adj.adj0) );
            break;

         case 0x0001:
            rpc_copy_adj1_rec_params( &(rpc_rec->rpc_adj.adj1) );
            break;

         case 0x0002:
            rpc_copy_adj2_rec_params( &(rpc_rec->rpc_adj.adj2) );
            break;

         default:
            MSG_ERROR("Bad rec type in PCNM", 0, 0, 0 );
      }

      /* next RPC Record */
      rpc_rec++;

   } /* for */
} /* rpc_init_pwr_ctrl_params */

/* <EJECT> */

/*===========================================================================
FUNCTION RPC_MEAN_CODE_CHAN_OUTPUT_POWER

DESCRIPTION
  Computes the Mean Code Channel Output Power.

DEPENDENCIES
  This function should be called after RLGC parameters have been initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_mean_code_chan_output_power (void)
{
   int16          fch2pich, pch_adj_gain;
   byte           rate = 0;
   txc_gain_type  gain;


#ifdef FEATURE_IS2000_R_SCH
   /* Update the multiple channel adjustments */
   rpc_calculate_multi_chan_adjustments();

   if (txc_rpc.sch_on)
   {
      rlgc_sch_gain_ratio = rpc_get_sch2pich_gain_ratio (txc_rpc.sch_rate,
                                                    txc_rpc.sch_code_type);
   }
#endif /* FEATURE_IS2000_R_SCH */

#if defined (FEATURE_IS2000_REL_A_SVD) && defined (FEATURE_IS2000_P2)
   /* for VP2 both FCH and DCCH can be on at the same time */
   if (txc_rpc.fch_on && txc_rpc.dcch_on)
   {
     for (rate = (byte) ENC_FULL_RATE; rate < ENC_NUM_FRAC_RATES; rate++)
     {
       if (txc_rpc.sch_on)
       {
         /* get FCH to PCH gain ratio for R-FCH  */
         fch2pich = rpc_get_fch2pich_gain_ratio (rate, FALSE);
         /* store fch2pich for logging purposes */
         rlgc_fch_gain_ratio[rate] = fch2pich;
         /* get FCH to PCH gain ratio for R-FCH  */
         rlgc_dcch_gain_ratio = rpc_get_dcch2pich_gain_ratio (FALSE);


         /**** FCH, DCCH and SCH active : DCCH_ACTIVE_SCH_ACTIVE ****/

         /* compute FCH, PCH gains when R-FCH, R-DCCH and R-SCH are assigned
         ** and all of them are ACTIVE
         */
         gain = rpc_compute_gains (fch2pich,rlgc_sch_gain_ratio,
                      rlgc_dcch_gain_ratio,FCH_GAIN | SCH_GAIN | DCCH_GAIN, RPC_TOT_DIG_PWR_255);
         /* compute adjusted PCH gain */
         pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));
         /* store the gain in the correct data structure */
         txc_fch_dcch_sch_gain_tab[rate][DCCH_ACTIVE_SCH_ACTIVE].gain = gain;
         txc_fch_dcch_sch_gain_tab[rate][DCCH_ACTIVE_SCH_ACTIVE].pch_adj_gain = pch_adj_gain;

         /**** FCH and DCCH active, SCH is DTX : DCCH_ACTIVE_SCH_DTX ****/

         /* get FCH to PCH gain ratio for R-FCH  */
         /* compute FCH, PCH gains when R-FCH, R-DCCH and R-SCH are assigned
         ** and FCH and DCCH are active and SCH is DTX
         */
         gain = rpc_compute_gains (fch2pich,0,
                      rlgc_dcch_gain_ratio,FCH_GAIN | DCCH_GAIN, RPC_TOT_DIG_PWR_255);
         /* compute adjusted PCH gain */
         pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));
         /* store the gain in the correct data structure */
         txc_fch_dcch_sch_gain_tab[rate][DCCH_ACTIVE_SCH_DTX].gain = gain;
         txc_fch_dcch_sch_gain_tab[rate][DCCH_ACTIVE_SCH_DTX].pch_adj_gain = pch_adj_gain;

         /**** FCH active, DCCH and SCH DTX: DCCH_DTX_SCH_DTX ****/

         /* get FCH to PCH gain ratio for R-FCH  */
         /* compute FCH, PCH gains when R-FCH, R-DCCH and R-SCH are assigned
         ** and FCH is active, DCCH and SCH are DTX
         */
         gain = rpc_compute_gains (fch2pich,0,0,FCH_GAIN, RPC_TOT_DIG_PWR_255);
         /* compute adjusted PCH gain */
         pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));
         /* store the gain in the correct data structure */
         txc_fch_dcch_sch_gain_tab[rate][DCCH_DTX_SCH_DTX].gain = gain;
         txc_fch_dcch_sch_gain_tab[rate][DCCH_DTX_SCH_DTX].pch_adj_gain = pch_adj_gain;


         /**** FCH and SCH active, DCCH DTX : DCCH_DTX_SCH_ACTIVE ****/

         /* get FCH to PCH gain ratio for R-FCH  */
         /* compute FCH, PCH gains when R-FCH, R-DCCH and R-SCH are assigned
         ** and FCH is active, DCCH and SCH are DTX
         */
         gain = rpc_compute_gains (fch2pich,rlgc_sch_gain_ratio,0,FCH_GAIN | SCH_GAIN, RPC_TOT_DIG_PWR_255);
         /* compute adjusted PCH gain */
         pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));
         /* store the gain in the correct data structure */
         txc_fch_dcch_sch_gain_tab[rate][DCCH_DTX_SCH_ACTIVE].gain = gain;
         txc_fch_dcch_sch_gain_tab[rate][DCCH_DTX_SCH_ACTIVE].pch_adj_gain = pch_adj_gain;
       }
       else
       {
         /* get FCH to PCH gain ratio for R-FCH  */
         fch2pich = rpc_get_fch2pich_gain_ratio (rate, TRUE); /* No need for multi-channels adjustment */
         /* store fch2pich for logging purposes */
         rlgc_fch_gain_ratio[rate] = fch2pich;
         /* get FCH to PCH gain ratio for R-FCH  */
         rlgc_dcch_gain_ratio = rpc_get_dcch2pich_gain_ratio (TRUE);/* No need for multi-channels adjustment */

         /**** FCH and DCCH active, but SCH is OFF: DCCH_ACTIVE_SCH_OFF ****/

         /* get FCH to PCH gain ratio for R-FCH  */
         /* compute FCH, PCH gains when R-FCH, R-DCCH are assigned and SCH is not
         ** FCH and DCCH are active,SCH is OFF
         */
         gain = rpc_compute_gains (fch2pich,0,rlgc_dcch_gain_ratio,FCH_GAIN | DCCH_GAIN, RPC_TOT_DIG_PWR_255);
         /* compute adjusted PCH gain */
         pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));
         /* store the gain in the correct data structure */
         txc_fch_dcch_sch_gain_tab[rate][DCCH_ACTIVE_SCH_OFF].gain = gain;
         txc_fch_dcch_sch_gain_tab[rate][DCCH_ACTIVE_SCH_OFF].pch_adj_gain = pch_adj_gain;

         /**** FCH active, DCCH is DTX and SCH is OFF: DCCH_DTX_SCH_OFF ****/

         /* get FCH to PCH gain ratio for R-FCH  */
         /* compute FCH, PCH gains when R-FCH, R-DCCH are assigned and SCH is not
         ** FCH is active, DCCH is DTX and SCH is OFF
         */
         gain = rpc_compute_gains (fch2pich,0,0,FCH_GAIN, RPC_TOT_DIG_PWR_255);
         /* compute adjusted PCH gain */
         pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));
         /* store the gain in the correct data structure */
         txc_fch_dcch_sch_gain_tab[rate][DCCH_DTX_SCH_OFF].gain = gain;
         txc_fch_dcch_sch_gain_tab[rate][DCCH_DTX_SCH_OFF].pch_adj_gain = pch_adj_gain;

       }

     }
   }
   else if (txc_rpc.fch_on )
#else
   if (txc_rpc.fch_on)
#endif /* FEATURE_IS2000_REL_A_SVD and FEATURE_IS2000_P2 */
   {
      for (rate = (byte) ENC_FULL_RATE; rate < ENC_NUM_FRAC_RATES; rate++)
      {
#ifdef FEATURE_IS2000_R_SCH
         if (txc_rpc.sch_on)
         {
            /* get FCH to PCH gain ratio for F-FCH and F-SCH  */
            fch2pich = rpc_get_fch2pich_gain_ratio (rate, FALSE);

            /* store fch2pich for logging purposes */
            rlgc_fch_gain_ratio[rate] = fch2pich;

            /* compute FCH, PCH gains when F-FCH and F-SCH is assigned but
            ** F-SCH is INACTIVE
            */
            gain = rpc_compute_gains (fch2pich, 0, 0, FCH_GAIN, RPC_TOT_DIG_PWR_255);

            /* compute adjusted PCH gain */
            pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));

            txc_set_pch_fch_gain (rate, gain, pch_adj_gain, TXC_SCH_DTX);

            /* compute FCH, PCH gains when F-FCH and F-SCH is assigned and
            ** F-SCH is ACTIVE
            */
            gain = rpc_compute_gains (fch2pich, rlgc_sch_gain_ratio, 0,
                                      FCH_GAIN | SCH_GAIN, RPC_TOT_DIG_PWR_255);

            /* compute adjusted PCH gain */
            pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));

            txc_set_pch_fch_gain (rate, gain, pch_adj_gain, TXC_SCH_ACTIVE);
         }
         else
#endif /* FEATURE_IS2000_R_SCH */
         {
            /* get FCH to PCH gain ratio for F-FCH only case */
            fch2pich = rpc_get_fch2pich_gain_ratio (rate, TRUE);

            /* store fch2pich for logging purposes */
            rlgc_fch_gain_ratio[rate] = fch2pich;

            /* compute FCH, PCH gains for F-FCH only case */
            gain = rpc_compute_gains (fch2pich, 0, 0, FCH_GAIN, RPC_TOT_DIG_PWR_255);

            /* compute adjusted PCH gain */
            pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));

            txc_set_pch_fch_gain (rate, gain, pch_adj_gain, TXC_FCH_ONLY);
         }
      }
   }
#ifdef FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A_SVD
   else if (txc_rpc.dcch_on)
#else
   if (txc_rpc.dcch_on)
#endif /* FEATURE_IS2000_REL_A_SVD */
   {
#ifdef FEATURE_IS2000_R_SCH
      if (txc_rpc.sch_on)
      {

         /* R-DCCH and R-SCH assigned, get DCCH_TO_PICH gain ratio */
         rlgc_dcch_gain_ratio = rpc_get_dcch2pich_gain_ratio (FALSE);

         /* R-DCCH and R-SCH assigned and both ACTIVE */
         gain = rpc_compute_gains (0, rlgc_sch_gain_ratio, rlgc_dcch_gain_ratio,
                                   DCCH_GAIN | SCH_GAIN, RPC_TOT_DIG_PWR_255);
         pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));
         txc_dcch_sch_gain_tab [DCCH_ACTIVE_SCH_ACTIVE].gain = gain;
         txc_dcch_sch_gain_tab [DCCH_ACTIVE_SCH_ACTIVE].pch_adj_gain = pch_adj_gain;

         /* R-DCCH assigned and ACTIVE, R-SCH assigned but DTX */
         gain = rpc_compute_gains (0, 0, rlgc_dcch_gain_ratio, DCCH_GAIN,
                                   RPC_TOT_DIG_PWR_255);
         pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));
         txc_dcch_sch_gain_tab [DCCH_ACTIVE_SCH_DTX].gain = gain;
         txc_dcch_sch_gain_tab [DCCH_ACTIVE_SCH_DTX].pch_adj_gain = pch_adj_gain;

         /* R-DCCH assigned but DTX, R-SCH assigned and ACTIVE */
         gain = rpc_compute_gains (0, rlgc_sch_gain_ratio, 0, SCH_GAIN, RPC_TOT_DIG_PWR_255);
         pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_ACTIVE].gain = gain;
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_ACTIVE].pch_adj_gain = pch_adj_gain;

         /* R-DCCH and R-SCH assigned and both DTX */
//       gain = rpc_compute_gains (0, 0, 0, 0);
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_DTX].gain.fch_gain  = 0;
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_DTX].gain.sch_gain  = 0;
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_DTX].gain.dcch_gain = 0;

         /* Make sure the PCH gain is left shift by 7 as part of TX gain enhancement
         ** to give MDSP 15 bit gain resolution.
         */
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_DTX].gain.pch_gain  =
                                                         RPC_TOT_DIG_PWR_255<<7;
         /* To calculate the rate adjustment gain, use the byte value, not the
         ** value that has been shifted.
         */
         pch_adj_gain = rpc_get_pch_adj_gain (RPC_TOT_DIG_PWR_255);
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_DTX].pch_adj_gain = pch_adj_gain;
      }
      else
#endif /* FEATURE_IS2000_R_SCH */
      {
         /* get DCCH_TO_PICH gain ration for R-DCCH, no R-SCH assigned */
         rlgc_dcch_gain_ratio = rpc_get_dcch2pich_gain_ratio (TRUE);

         /* R-DCCH assigned and ACTIVE, no R-SCH */
         gain = rpc_compute_gains (0, 0, rlgc_dcch_gain_ratio, DCCH_GAIN,
                                   RPC_TOT_DIG_PWR_255);
         pch_adj_gain = rpc_get_pch_adj_gain (PCH_GAIN_TRUNCATE_TO_8BITS(gain.pch_gain));
         txc_dcch_sch_gain_tab [DCCH_ACTIVE_SCH_OFF].gain = gain;
         txc_dcch_sch_gain_tab [DCCH_ACTIVE_SCH_OFF].pch_adj_gain = pch_adj_gain;

         /* R-DCCH assigned but DTX, no R-SCH */
//       gain = rpc_compute_gains (0, 0, 0, 0);
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_OFF].gain.fch_gain  = 0;
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_OFF].gain.sch_gain  = 0;
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_OFF].gain.dcch_gain = 0;

         /* Make sure the PCH gain is left shift by 7 as part of TX gain enhancement
         ** to give MDSP 15 bit gain resolution.
         */
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_OFF].gain.pch_gain  =
                                                       RPC_TOT_DIG_PWR_255<<7;

         /* To calculate the rate adjustment gain, use the byte value, not the
         ** value that has been shifted.
         */
         pch_adj_gain = rpc_get_pch_adj_gain (RPC_TOT_DIG_PWR_255);
         txc_dcch_sch_gain_tab [DCCH_DTX_SCH_OFF].pch_adj_gain   = pch_adj_gain;
      }
   }
#endif /* FEATURE_IS2000_P2 */

} /* rpc_mean_code_chan_output_power */

#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
FUNCTION RPC_GET_MULTI_CHAN_ADJ_GAIN

DESCRIPTION
  This function computes the chan adjustment gain because of the presence of
  multiple channels.

DEPENDENCIES
  This function should be called after RLGC parameters have been initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 rpc_get_multi_chan_adj_gain
(
  enc_sch_rate_type  sch_rate,
  enc_rate_set_type  rate_set
)
{
  int16 multi_chan_adj_gain = 0;

  /* If R-SCH is assigned, calculate fch_multi_chan_adj_gain */
  if (sch_rate >= ENC_SCH_2X_RATE)
  {
    uint8 index = (uint8)(RLGC_UPPER_BOUNDS_CHECK(
                           sch_rate - ENC_SCH_2X_RATE, Max_Num_Of_High_Rates));
     /*  reduce the enum rate down to within the bounds of
         the gains arrays */

#ifdef FEATURE_DS_IS2000_RS2
    if (txc_rpc.sch_rc == CAI_RC_4)
    {
      multi_chan_adj_gain =
      (int16) high_rs2_rl_nom_att_gains[txc_rpc.sch_code_type][index].pilot_ref_lvl +
      (int16) high_rs2_rl_att_adj_gains[txc_rpc.sch_code_type][index].pilot_ref_lvl;
    }
    else
#endif /* FEATURE_DS_IS2000_RS2 */
    {
      multi_chan_adj_gain =
      (int16) high_rs1_rl_nom_att_gains[txc_rpc.sch_code_type][index].pilot_ref_lvl +
      (int16) high_rs1_rl_att_adj_gains[txc_rpc.sch_code_type][index].pilot_ref_lvl;
    }

    if (rate_set == ENC_RATE_14400)
    {
      /* Rate Set 2 */
      multi_chan_adj_gain -= 3;
    }
  }
  return multi_chan_adj_gain;
} /* rpc_get_multi_chan_adj_gain */


/*===========================================================================
FUNCTION RPC_CALCULATE_MULTI_CHAN_ADJUSTMENTS

DESCRIPTION
  This function computes the multiple channel adjustments for all assigned
  channels.

DEPENDENCIES
  This function should be called after the assigned channels have been
  configured.

RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the multiple channel adjustments in txc_rpc.
===========================================================================*/
void rpc_calculate_multi_chan_adjustments ( void )
{
  int16 max_pilot_reference_level=0;

  int16 fch_prl=0;
#ifdef FEATURE_IS2000_P2
  int16 dcch_prl=0;
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_R_SCH
  int16 sch_prl=0;
#endif /* FEATURE_IS2000_R_SCH */

  /**************************************************
   * Calculate all channels' pilot reference levels *
   **************************************************/

  if (txc_rpc.fch_rate_set == ENC_RATE_9600)
  {
    fch_prl = basic_rs1_rl_nom_att_gains [ENC_FULL_RATE].pilot_ref_lvl
              + basic_rs1_rl_att_adj_gains [ENC_FULL_RATE].pilot_ref_lvl;
  }
  else
  {
    fch_prl = basic_rs2_rl_nom_att_gains [ENC_FULL_RATE].pilot_ref_lvl
              + basic_rs2_rl_att_adj_gains [ENC_FULL_RATE].pilot_ref_lvl;
  }

#ifdef FEATURE_IS2000_P2
  if (txc_rpc.dcch_rate_set == ENC_RATE_9600)
  {
    dcch_prl = basic_rs1_rl_nom_att_gains [ENC_FULL_RATE].pilot_ref_lvl
              + basic_rs1_rl_att_adj_gains [ENC_FULL_RATE].pilot_ref_lvl;
  }
  else
  {
    dcch_prl = basic_rs2_rl_nom_att_gains [ENC_FULL_RATE].pilot_ref_lvl
              + basic_rs2_rl_att_adj_gains [ENC_FULL_RATE].pilot_ref_lvl;
  }
#endif /* FEATURE_IS2000_P2 */


#ifdef FEATURE_IS2000_R_SCH
  if (txc_rpc.sch_rate >= ENC_SCH_2X_RATE)
  {
    uint8 index = (uint8)(RLGC_UPPER_BOUNDS_CHECK(
                           txc_rpc.sch_rate - ENC_SCH_2X_RATE, Max_Num_Of_High_Rates));
     /*  reduce the enum rate down to within the bounds of
         the gains arrays */

#ifdef FEATURE_DS_IS2000_RS2
    if (txc_rpc.sch_rc == CAI_RC_4)
    {
      sch_prl =
      (int16) high_rs2_rl_nom_att_gains[txc_rpc.sch_code_type][index].pilot_ref_lvl +
      (int16) high_rs2_rl_att_adj_gains[txc_rpc.sch_code_type][index].pilot_ref_lvl;
    }
    else
#endif /* FEATURE_DS_IS2000_RS2 */
    {
      sch_prl =
      (int16) high_rs1_rl_nom_att_gains[txc_rpc.sch_code_type][index].pilot_ref_lvl +
      (int16) high_rs1_rl_att_adj_gains[txc_rpc.sch_code_type][index].pilot_ref_lvl;
    }
  }
  else
  {
    if (txc_rpc.sch_rc == CAI_RC_4)
    {
      sch_prl = basic_rs2_rl_nom_att_gains [ENC_FULL_RATE].pilot_ref_lvl
                + basic_rs2_rl_att_adj_gains [ENC_FULL_RATE].pilot_ref_lvl;
    }
    else
    {
      sch_prl = basic_rs1_rl_nom_att_gains [ENC_FULL_RATE].pilot_ref_lvl
                + basic_rs1_rl_att_adj_gains [ENC_FULL_RATE].pilot_ref_lvl;
    }
  }
#endif /* FEATURE_IS2000_R_SCH */

  /***********************************************
   * Calculate the maximum pilot reference level *
   ***********************************************/

  max_pilot_reference_level = MAX(max_pilot_reference_level, fch_prl);
#ifdef FEATURE_IS2000_P2
  max_pilot_reference_level = MAX(max_pilot_reference_level, dcch_prl);
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_R_SCH
  max_pilot_reference_level = MAX(max_pilot_reference_level, sch_prl);
#endif /* FEATURE_IS2000_R_SCH */

  /************************************************
   * Calculate the multiple chan adjustment gains *
   ************************************************/

  txc_rpc.fch_multi_chan_adj_gain = max_pilot_reference_level - fch_prl;
#ifdef FEATURE_IS2000_P2
  txc_rpc.dcch_multi_chan_adj_gain = max_pilot_reference_level - dcch_prl;
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_R_SCH
  txc_rpc.sch_multi_chan_adj_gain = max_pilot_reference_level - sch_prl;
#endif /* FEATURE_IS2000_R_SCH */

} /* rpc_calculate_multi_chan_adjustments */

#endif /* FEATURE_IS2000_R_SCH */


/*************** End L3 Table Update Functions } ****************/






/****************************************************************
 ****************************************************************
 *                   Math Support Functions {                   *
 ****************************************************************
 ****************************************************************/

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_SQRT32

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
dword rpc_sqrt32
(
  dword val
)
{
   dword divider, remainder, result;
   word  current_bit;
   byte  i;

   result = 0;
   divider = 0;
   remainder = (val >> 30);
   val = (val << 2);
   current_bit = 0x8000;

   for (i = 0; i < 16; i++)
   {
      if (remainder >= divider+1)
      {
         result |= current_bit;
         remainder -= (++divider);
         divider++;
      }
      remainder = ((remainder << 2) | (val >> 30));
      val <<= 2;
      divider <<= 1;
      current_bit >>= 1;
   }

   if (remainder >= divider+1)
   {
      /* Next bit indicates a round up to nearest integer */
      result++;
   }

   return result;
} /* rpc_sqrt32 */

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_MAX_GAIN

DESCRIPTION
  Finds out MAX { FCH_GAIN, PCH_GAIN, SCH_GAIN, DCCH_GAIN }

DEPENDENCIES

RETURN VALUE
  Returns the max value.

SIDE EFFECTS
  None.
===========================================================================*/
word rpc_max_gain
(
  word  gp,
  word  gf,
  word  gs,
  word  gd
)
{
   word  max = gs;

   if (max < gp)
      max = gp;

   if (max < gf)
      max = gf;

   if (max < gd)
      max = gd;

   return max;
} /* rpc_max_gain */

/**************** End Math Support Functions } ******************/







/****************************************************************
 ****************************************************************
 *                     SCH Support Functions {                  *
 ****************************************************************
 ****************************************************************/



#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
FUNCTION RPC_GET_SCH_PILOT_REF_LEVEL

DESCRIPTION
  This function calculates the pilot reference level for the SCH channel.

DEPENDENCIES

RETURN VALUE
  Pilot reference level for SCH.

SIDE EFFECTS
  None.
===========================================================================*/
int16 rpc_get_sch_pilot_ref_level
(
  enc_sch_rate_type sch_rate,
  boolean           sch_code_type
)
{
  int16 pilot_ref_lvl = 0;

  if (sch_rate >= ENC_SCH_2X_RATE)
  {
    uint8 index = (uint8)(RLGC_UPPER_BOUNDS_CHECK(
                           sch_rate - ENC_SCH_2X_RATE, Max_Num_Of_High_Rates));
     /*  reduce the enum rate down to within the bounds of
         the gains arrays */

#ifdef FEATURE_DS_IS2000_RS2
    if (txc_rpc.sch_rc == CAI_RC_4)
    {
      pilot_ref_lvl = 
        high_rs2_rl_nom_att_gains[sch_code_type][index].pilot_ref_lvl +
        high_rs2_rl_att_adj_gains[sch_code_type][index].pilot_ref_lvl;
    }
    else
#endif /* FEATURE_DS_IS2000_RS2 */
    {
      pilot_ref_lvl = 
        high_rs1_rl_nom_att_gains[sch_code_type][index].pilot_ref_lvl + 
        high_rs1_rl_att_adj_gains[sch_code_type][index].pilot_ref_lvl;
    }
  }

  return pilot_ref_lvl;
} /* rpc_get_sch_pilot_ref_level */
#endif /* FEATURE_IS2000_R_SCH */




/******************** End SCH Support Functions } ***************/

/****************************************************************
 ****************************************************************
 *                          PLT Support   {                     *
 ****************************************************************
 ****************************************************************/

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

/********************** End PLT Support } **********************/

#ifdef FEATURE_IS2000_REL_A_CC
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
int16 rlgc_get_nominal_rcch_attribute_gain(int16 index)
{
  if ((index >0) && (index < TXC_EACH_RATE_38400_5_V))
  {
    return(rpc_nominal_RCCH_attribute_gain_table[index]);
  }
  else
  {
    return(rpc_nominal_RCCH_attribute_gain_table[0]);
  }
}

#endif


/************************* } END PRIVATE FUNCTIONS ***************************/

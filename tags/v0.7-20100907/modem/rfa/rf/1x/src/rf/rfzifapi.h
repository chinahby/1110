#ifndef RFZIFAPI_H
#define RFZIFAPI_H
/*=================================================================================

                          R F  Z I F  A P I

GENERAL DESCRIPTION
  This file contains all the api functions Qualcomm is providing to the customers
  to be able to manipulate the ZIF chipset

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1999 - 2006 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007 - 2009 by QUALCOMM, Inc.  All Rights Reserved.
===================================================================================*/


/*=================================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfzifapi.h#8 $
  $DateTime: 2009/01/27 15:17:40 $
  $Author: vboda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------
01/22/09   vb      Removed RF APIs that are no longer needed
12/10/08   vm      Added the support for new RF APIs
11/07/08   vm      Added support for 3 State PA
09/25/08   vm      Added support for PA Selection for Bypass and Non Bypass PA
07/22/08   vm      Added customer interface APIs for QSC1100
04/28/08   sl      Added LO PWC Cal support.
03/28/08   vm      Added the support for Tx AGC interface with MDSP for QSC1100
11/28/07   ems     Support for QSC1100
11/24/07   ans     Added API to configure external GPS LNA.
11/02/07   adk     Added prototype for ziflib_get_rx_spectral_inversion().
08/13/07   aak     APIs for DA Selection and DA Bias
06/13/07   vm      Added the support for Tx Bypass PA configration
06/06/07   zg      Support GPS External LNA on QSC60X5.
03/20/07   bmg     Added rft_da_select_set API
                   Added include guards
06/14/06   zg      Added API for choosing LNA output topology.
04/06/06   ans     Changed adjust current API for RFR6155/RFR6185.
                   Added APi to enable/disable IM2 Cal for RFR6155/RFR6185.
03/30/06   ans     Changed rfr_vco_coarse_tune_adjust api with synth_vco_coarse_tune_adjust api.
03/17/06   ans     Added APIs for RFR6155 and RFR6185.
03/02/06   bmg     CFT API removed in favor of a baseline RFT6150 change
02/06/06   hkk     Added rft_set_cft_adjust
12/06/05   hkk     API for setting VCO Bias
02/25/04   ans     Added synth_rx_Icp_set api to configure charge pump 
                   current.
11/08/04   ans     Added API rfr_set_pre_lna_on_off for customers to control pre-lna.
10/31/04   dyc     Misc typo fixes.
10/28/04   ans     Changes to rfr_adjust_rfr_current for RFR6135
03/25/04   dbc     added rfr_set_lo_sleep_mode() API
11/05/03    et     updated prototype and body of rfr_adjust_rfr_current
                   to comply with library rules
11/05/03    et     added rfrdev parameter to rfr_adjust_rfr_current 
10/31/03    et     added rfr_adjust_rfr_current() API
09/12/02    et     added functions rft_tx_lock_det_counter_length_set
                   and rft_rx_lock_det_counter_length_set
08/28/02    et     added rft_lock_det_set
07/26/02   dyc     removed more unnecessary functions
                   (merged from et MSM6000_RF.01.00.15)
06/21/02    et     removed unnecessary functions
05/30/02    et     updated RFR and RFT regs per 5/22 meeting
03/23/02   aks     Remove RFR APIs.
03/18/02   aks     Added proper comment closer, to activate include files.
03/15/02    et     initial revision

===================================================================================*/

#include "comdef.h"
#include "rfcom.h"
#include "enc.h"
#include "rficap.h"

extern boolean rf_card_can_bit_bang;
/******************************************************************************/
/* RFT return value - ERROR CODES definitions */
/******************************************************************************/

#define RFT_WRITE_SUCCESS    0x00 /* value was written successfully, 
                                     no errors */ 
#define RFT_ERROR_CODE_1     0x01 /* one or more of the parameters is 
                                     out of range */
#define RFT_ERROR_CODE_2     0x02 /* one or more of the paramters is not a field
                                     of the enum type provided */


/******************************************************************************/
/* RFL return value - ERROR CODES definitions */
/******************************************************************************/

#define RFL_WRITE_SUCCESS    0x00 /* value was written successfully,
                                     no errors */
#define RFL_ERROR_CODE_1     0x01 /* one or more of the parameters is
                                     out of range */
#define RFL_ERROR_CODE_2     0x02 /* one or more of the paramters is not a field
                                     of the enum type provided */


/******************************************************************************/
/* RFR return value - ERROR CODES definitions */
/******************************************************************************/

#define RFR_WRITE_SUCCESS    0x00 /* value was written successfully,
                                     no errors */
#define RFR_ERROR_CODE_1     0x01 /* one or more of the parameters is
                                     out of range */
#define RFR_ERROR_CODE_2     0x02 /* one or more of the paramters is not a field
                                     of the enum type provided */

/* SYNTH return value - ERROR CODES definitions */
/******************************************************************************/

#define SYNTH_WRITE_SUCCESS    0x00 /* value was written successfully,
                                     no errors */
#define SYNTH_ERROR_CODE_1     0x01 /* one or more of the parameters is
                                     out of range */
#define SYNTH_ERROR_CODE_2     0x02 /* one or more of the paramters is not a field
                                     of the enum type provided */

/******************************************************************************/
                /*  RFT ENUMS */
/******************************************************************************/

/* enum to program the Fast Acquisition boost mode control bits */

typedef enum {
  RFT_OFF,
  RFT_4X,
  RFT_8X,
  RFT_16X
}rft_faq_boost_ctl_type;


/* enum to program the DA Puncture control */

typedef enum{
  RFT_DA_ON_W_IDLEB,     /* DA powers on with the IDLEB signal */
  RFT_DA_ON_W_TXON,      /* DA powers on with the TXON signal */
  RFT_DA_ON_W_TX_SIG_ON, /* DA powers on with the TX_SIG_ON signal */
                         /* TX_SIG_ON = TX_ON && LOCK */
  RFT_DA_OFF             /* DA powers off always */
} rft_da_punct_type;

typedef enum{
  RFL_DEV0,
  RFL_DEV1
} rfl_device_enum_type;

typedef enum{
  RFR_DEV0,
  RFR_DEV1
} rfr_device_enum_type;



/* enum to program the UPC Puncture control */

typedef enum{
  RFT_UPC_ON_W_IDLEB,     /* UPC powers on with the IDLEB signal */
  RFT_UPC_ON_W_TXON,      /* UPC powers on with the TXON signal */
  RFT_UPC_ON_W_TX_SIG_ON, /* UPC powers on with the TX_SIG_ON signal */
                          /* TX_SIG_ON = TX_ON && LOCK */
  RFT_UPC_OFF             /* UPC powers off always */
} rft_upc_punct_type;

/* enum to program the LO Generator Puncture control */

typedef enum{
  RFT_LO_GEN_ON_W_IDLEB,     /* LOgen powers on with the IDLEB signal */
  RFT_LO_GEN_ON_W_TXON,      /* LOgen powers on with the TXON signal */
  RFT_LO_GEN_ON_W_TX_SIG_ON, /* LOgen powers on with the TX_SIG_ON signal */
                             /* TX_SIG_ON = TX_ON && LOCK */
  RFT_LO_GEN_OFF             /* LOgen powers off always */
} rft_lo_gen_punct_type;

/* enum to program the Iref/TxDAC Puncture control */

typedef enum{
  RFT_IREF_ON_W_IDLEB,     /* Iref powers on with the IDLEB signal */
  RFT_IREF_ON_W_TXON,      /* Iref control powers on with the TXON signal */
  RFT_IREF_ON_W_TX_SIG_ON, /* Iref control powers on with the TX_SIG_ON signal */
                           /* TX_SIG_ON = TX_ON && LOCK */
  RFT_IREF_OFF             /* Iref control powers off always */
} rft_iref_punct_type;

/* enum to program the SBI Puncture control */

typedef enum{
  RFT_SBI_DIS_W_TXON,       /* SBI is disabled with the TX_ON signal */
  RFT_SBI_DIS_W_TX_SIG_ON,  /* SBI is disabled with the TX_SIG_ON signal */
  RFT_SBI_DIS_NEVER         /* SBI is never disabled */
} rft_sbi_punct_type;

/* enum to program the TX PLL / VCO Puncture control */

typedef enum{
  RFT_TXPLL_VCO_ON_W_IDLEB,     /* TXPLL / VCO powers on with the IDLEB signal */
  RFT_TXPLL_VCO_ON_W_TXON,      /* TXPLL / VCO powers on with the TXON signal */
  RFT_TXPLL_VCO_OFF_ALWAYS      /* TXPLL / VCO powers off always */
} rft_txpll_vco_punct_type;

/* Tx driver amp selection enum */


typedef enum {    
  RFT_DA_SELECT_DA_1_ALWAYS,  /* Ignores PA_R0 for selecting the active DA 0x86 00*/
  RFT_DA_SELECT_DA_2_ALWAYS,  /* Ignores PA_R0 for selecting the active DA 0xA6 01*/
  RFT_DA_SELECT_DA_1_PA_R0,   /* PA_R0 high selects DA 1 0xC6 10*/
  RFT_DA_SELECT_DA_2_PA_R0,   /* PA_R0 high selects DA 2 0xE6 11*/
  RFT_DA_SELECT_DA_1_PA_R1,   /* PA_R1 high selects DA 1 0xA6*/
  RFT_DA_SELECT_DA_2_PA_R1    /* PA_R1 high selects DA 2 0x86*/
} rft_da_select_type;

typedef enum {
  RFT_BYPASS_PA,
  RFT_NON_BYPASS_PA 
} rft_pa_select_type;


typedef enum {
  RFT_SINGLE_STATE_PA,
  RFT_DUAL_STATE_PA,
  RFT_TRI_STATE_PA,
  RFT_QUAD_STATE_PA
} rft_pa_states_type;

typedef enum {
  NORMAL_MODE,
  TEST_MODE
} rf_test_port_mode_type;

typedef enum {
  TX_UNMASK_MODE,
  TX_MASK_MODE
} rft_da_tx_mask_type;


typedef enum {
  RFT_DA_SELECT_LOW_BIAS_ALWAYS,   /* Ignores PA_R0 for selecting DA bias*/
  RFT_DA_SELECT_HIGH_BIAS_ALWAYS,  /* Ignores PA_R0 for selecting DA bias */
  RFT_DA_SELECT_LOW_BIAS_PA_R0,    /* PA_R0 high selects low bias */
  RFT_DA_SELECT_HIGH_BIAS_PA_R0,   /* PA_R0 high selects high bias */
  RFT_DA_SELECT_LOW_BIAS_PA_R1,    /* PA_R1 high selects low bias */
  RFT_DA_SELECT_HIGH_BIAS_PA_R1    /* PA_R1 high selects high bias */
} rft_da_select_bias;

typedef enum {
  NON_BYPASS_PA_SUPPORTED,
  BYPASS_PA_SUPPORTED
} rft_pa_bypass_config_type;

/****************************************************************************/
/*           RFL        ENUMS                                                  */
/****************************************************************************/

/* enum to set LNA gain state */

typedef enum{
  RFL_MAX_GAIN,           
  RFL_MID_GAIN,          
  RFL_LOW_GAIN,         
  RFL_DEFAULT_LOW_GAIN 
} rfl_gain_state_type;


/* enum to set LNA mode */

typedef enum{
  RFL_AMPS,
  RFL_CELL_CDMA,
  RFL_PCS_IMT,
  RFL_DEF_PCS
} rfl_mode_type;

/******************************************************************************/
                /*  FUNCTIONS */
/******************************************************************************/

/*==============================================================================

FUNCTION

  RFT_TX_ICP_SET

PROTOTYPE

  byte rft_tx_Icp_set( byte M_value )

DESCRIPTION

  This function sets the charge pump output current of the transmitter
  The user passes in the M_value, which translates into the Icp via the equation:

  Icp = (M+1) * 62.5 uA


PARAMETERS

  byte M_value - an integer between 0 and 63

RETURN VALUE

  RFT_ERROR_CODE_1  if M_value is out of range
  RFT_WRITE_SUCCESS if M_value was in range and was written successfully

EXAMPLE 

  M = 0 to 63
  Icp = (M+1) * 62.5 uA
  Icp = 1.0625mA  -> M = 16
  user programs 16

  rft_tx_Icp_set(16);

SIDE EFFECTS
  None.

===========================================================================*/


byte rft_tx_Icp_set( byte M_value );

/*==============================================================================

FUNCTION

  RFT_RX_ICP_SET

PROTOTYPE

  byte rft_rx_Icp_set( byte M_value )

DESCRIPTION

  This function sets the charge pump output current of the receiver
  The user passes in the M_value, which translates into the Icp via the equation:

  Icp = (M+1) * 62.5 uA


PARAMETERS

  byte M_value - an integer between 0 and 63

RETURN VALUE

  RFT_ERROR_CODE_1  if M_value is out of range
  RFT_WRITE_SUCCESS if M_value was in range and was written successfully

EXAMPLE 

  M = 0 to 63
  Icp = (M+1) * 62.5 uA
  Icp = 1.0625mA  -> M = 16
  user programs 16

  rft_rx_Icp_set(16);

SIDE EFFECTS
  None.

===========================================================================*/


byte rft_rx_Icp_set( byte M_value );

/*==============================================================================

FUNCTION

  RFT_TX_FAQ_BOOST_MODE

PROTOTYPE

  byte rft_tx_faq_boost_mode( rft_faq_boost_ctl_type boost_mode);

DESCRIPTION
  
  This function programs the Fast Acquisition Boost Mode control bits
  in the transmitter
  Only available on PLL1 & PLL2 


PARAMETERS

  rft_faq_boost_ctl_type boost_mode

  typedef enum {
    RFT_OFF,
    RFT_4X,
    RFT_8X,
    RFT_16X
  }rft_faq_boost_ctl_type

RETURN VALUE

  RFT_ERROR_CODE_2  if boost_mode value is not member of the enum
  RFT_WRITE_SUCCESS if boost_mode was in range and was written successfully

EXAMPLE 

  rft_tx_faq_boost_mode(RFT_4X); 

SIDE EFFECTS
  None.

===========================================================================*/


byte rft_tx_faq_boost_mode( rft_faq_boost_ctl_type boost_mode);

/*==============================================================================

FUNCTION

  RFT_RX_FAQ_BOOST_MODE

PROTOTYPE

  byte rft_rx_faq_boost_mode( rft_faq_boost_ctl_type boost_mode);

DESCRIPTION
  
  This function programs the Fast Acquisition Boost Mode control bits
  in the receiver
  Only available on PLL1 & PLL2 


PARAMETERS

  rft_faq_boost_ctl_type boost_mode

  typedef enum {
    RFT_OFF,
    RFT_4X,
    RFT_8X,
    RFT_16X
  }rft_faq_boost_ctl_type

RETURN VALUE

  RFT_ERROR_CODE_2  if boost_mode value is not member of the enum
  RFT_WRITE_SUCCESS if boost_mode was in range and was written successfully

EXAMPLE 

  rft_rx_faq_boost_mode(RFT_4X); 

SIDE EFFECTS
  None.

===========================================================================*/

byte rft_rx_faq_boost_mode( rft_faq_boost_ctl_type boost_mode);



/*==============================================================================

FUNCTION

  RFT_DA_PUNCTURE_CTL_SET

PROTOTYPE

  byte rft_da_puncture_ctl_set( rft_da_punct_type da_punct )

DESCRIPTION
  
  This function programs the DA Puncture Control

PARAMETERS

  rft_da_punct_type da_punct

  typedef enum{
    RFT_DA_ON_W_IDLEB,     // DA powers on with the IDLEB signal 
    RFT_DA_ON_W_TXON,      // DA powers on with the TXON signal 
    RFT_DA_ON_W_TX_SIG_ON, // DA powers on with the TX_SIG_ON signal
                           // TX_SIG_ON = TX_ON && LOCK
    RFT_DA_OFF             // DA powers off always 
  } rft_da_punct_type
  
RETURN VALUE

  RFT_ERROR_CODE_2  if da_punct is not a member of the enum
  RFT_WRITE_SUCCESS if da_punct was in range and was written successfully

EXAMPLE 

  rft_da_puncture_ctl_set(RFT_DA_ON_W_TXON);   powers up DA with TXON sig

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_da_puncture_ctl_set( rft_da_punct_type da_punct );

/*==============================================================================

FUNCTION

  RFT_UPC_PUNCTURE_CTL_SET

PROTOTYPE

  byte rft_upc_puncture_ctl_set( rft_upc_punct_type upc_punct )

DESCRIPTION
  
  This function programs the UPC Puncture Control

PARAMETERS

  rft_upc_punct_type upc_punct

  typedef enum{
    RFT_UPC_ON_W_IDLEB,     // UPC powers on with the IDLEB signal 
    RFT_UPC_ON_W_TXON,      // UPC powers on with the TXON signal 
    RFT_UPC_ON_W_TX_SIG_ON, // UPC powers on with the TX_SIG_ON signal 
                            // TX_SIG_ON = TX_ON && LOCK 
    RFT_UPC_OFF             // UPC powers off always 
  } rft_upc_punct_type

RETURN VALUE

  RFT_ERROR_CODE_2  if upc_punct is not a member of the enum
  RFT_WRITE_SUCCESS if upc_punct was in range and was written successfully

EXAMPLE 

  rft_upc_puncture_ctl_set(RFT_UPC_ON_W_TXON);   powers up UPC with TXON sig

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_upc_puncture_ctl_set( rft_upc_punct_type upc_punct );

/*==============================================================================

FUNCTION

  RFT_LO_GEN_PUNCTURE_CTL_SET

PROTOTYPE

  byte rft_lo_gen_puncture_ctl_set( rft_lo_gen_punct_type lo_gen_punct )

DESCRIPTION
  
  This function programs the LO Generator Puncture Control (Includes VCO
  divider but not the VCO)

PARAMETERS

  rft_lo_gen_punct_type lo_gen_punct

  typedef enum{
    RFT_LO_GEN_ON_W_IDLEB,     // LOgen powers on with the IDLEB signal
    RFT_LO_GEN_ON_W_TXON,      // LOgen powers on with the TXON signal
    RFT_LO_GEN_ON_W_TX_SIG_ON, // LOgen powers on with the TX_SIG_ON signal
                               // TX_SIG_ON = TX_ON && LOCK
    RFT_LO_GEN_OFF             // LOgen powers off always
  } rft_lo_gen_punct_type

RETURN VALUE

  RFT_ERROR_CODE_2  if lo_gen_punct is not a member of the enum
  RFT_WRITE_SUCCESS if lo_gen_punct was in range and was written successfully

EXAMPLE 

  powers up LOgen with TXON sig
  rft_lo_gen_puncture_ctl_set(RFT_LO_GEN_ON_W_TXON);

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_lo_gen_puncture_ctl_set( rft_lo_gen_punct_type lo_gen_punct );

/*==============================================================================

FUNCTION

  RFT_IREF_PUNCTURE_CTL_SET

PROTOTYPE

  byte rft_iref_puncture_ctl_set( rft_iref_punct_type iref_punct )

DESCRIPTION
  
  This function programs the Iref/ TxDAC Puncture Control

PARAMETERS

  rft_iref_punct_type iref_punct

  typedef enum{
    RFT_IREF_ON_W_IDLEB,     // Iref powers on with the IDLEB signal
    RFT_IREF_ON_W_TXON,      // Iref control powers on with the TXON signal 
    RFT_IREF_ON_W_TX_SIG_ON, // Iref control powers on with the TX_SIG_ON signal
                             // TX_SIG_ON = TX_ON && LOCK
    RFT_IREF_OFF             // Iref control powers off always
  } rft_iref_punct_type

RETURN VALUE

  RFT_ERROR_CODE_2  if iref_punct is not a member of the enum
  RFT_WRITE_SUCCESS if iref_punct was in range and was written successfully

EXAMPLE 

  powers on Iref with TXON sig
  rft_iref_puncture_ctl_set(RFT_IREF_ON_W_TXON);

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_iref_puncture_ctl_set( rft_iref_punct_type iref_punct );

/*==============================================================================

FUNCTION

  RFT_SBI_PUNCTURE_CTL_SET

PROTOTYPE

  byte rft_sbi_puncture_ctl_set( rft_vco_punct_type vco_punct )

DESCRIPTION
  
  This function programs the SBI Puncture Control

PARAMETERS

  rft_sbi_punct_type sbi_punct

  typedef enum{
    RFT_SBI_DIS_W_TXON,       // SBI is disabled with the TX_ON signal 
    RFT_SBI_DIS_W_TX_SIG_ON,  // SBI is disabled with the TX_SIG_ON signal 
    RFT_SBI_DIS_NEVER         // SBI is never disabled
  } rft_sbi_punct_type

RETURN VALUE

  RFT_ERROR_CODE_2  if vco_punct is not a member of the enum
  RFT_WRITE_SUCCESS if vco_punct was in range and was written successfully

EXAMPLE 

  powers on vco with TXON sig
  rft_vco_puncture_ctl_set(RFT_VCO_ON_W_TXON);

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_sbi_puncture_ctl_set( rft_sbi_punct_type sbi_punct );

/*==============================================================================

FUNCTION

  RFT_TXPLL_VCO_PUNCTURE_CTL_SET

PROTOTYPE

  byte rft_txpll_vco_puncture_ctl_set( rft_txpll_vco_punct_type txpll_vco_punct )

DESCRIPTION
  
  This function programs the TX PLL / VCO Puncture Control

PARAMETERS

  rft_txpll_punct_type txpll_punct

  typedef enum{
    RFT_TXPLL_VCO_ON_W_IDLEB,     // TXPLL / VCO powers on with the IDLEB signal
    RFT_TXPLL_VCO_ON_W_TXON,      // TXPLL / VCO powers on with the TXON signal
    RFT_TXPLL_VCO_OFF_ALWAYS      // TXPLL / VCO powers off always
  } rft_txpll_punct_type

RETURN VALUE

  RFT_ERROR_CODE_2  if txpll_punct is not a member of the enum
  RFT_WRITE_SUCCESS if txpll_punct was in range and was written successfully

EXAMPLE 

  powers on vco with TXON sig
  rft_txpll_puncture_ctl_set(RFT_TXPLL_ON_W_TXON);

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_txpll_vco_puncture_ctl_set( rft_txpll_vco_punct_type txpll_vco_punct );

/*==============================================================================

FUNCTION

  RFT_UHF_LO_IN_BUF_PUNCTURE_CTL

PROTOTYPE

  byte rft_uhf_lo_in_buf_puncture_ctl( boolean sig_to_follow )

DESCRIPTION
  
  This function sets the UHF LO Input Buffer Puncture Control to
  follow the Lo Gen or the IDLEB signal (offset mode only)

PARAMETERS

  boolean sig_to_follow

  0 - Follows the Lo Gen
  1 - Follows the IDLEB signal

RETURN VALUE

  RFT_ERROR_CODE_1  if sig_to_follow is not a 0 or 1
  RFT_WRITE_SUCCESS if sig_to_follow was in range and was written successfully

EXAMPLE 

  makes the UHF LO Input Buffer Control follow the Lo Gen Signal 
  rft_uhf_lo_in_buf_puncture_ctl(0); 

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_uhf_lo_in_buf_puncture_ctl( boolean sig_to_follow );

/*==============================================================================

FUNCTION

  RFT_DA_GAIN_CTL

PROTOTYPE

  byte rft_da_gain_ctl( boolean sig_to_follow )

DESCRIPTION
  
  This function sets DA control

PARAMETERS

  boolean sig_to_follow

  0 - Follows the DA 
  1 - Follows the IDLEB signal

RETURN VALUE

  RFT_ERROR_CODE_1  if sig_to_follow is not a 0 or 1
  RFT_WRITE_SUCCESS if sig_to_follow was in range and was written successfully

EXAMPLE 

  rft_da_gain_ctl(0);   sets the DA Gain Control to follow the DA 

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_da_gain_ctl( boolean sig_to_follow );

/*==============================================================================

FUNCTION

  RFT_UPC_GAIN_CTL

PROTOTYPE

  byte rft_upc_gain_ctl( boolean sig_to_follow )

DESCRIPTION
  
  This function sets the upconverter gain control

PARAMETERS

  boolean sig_to_follow

  0 - Follows the UPC 
  1 - Follows the IDLEB signal

RETURN VALUE

  RFT_ERROR_CODE_1  if sig_to_follow is not a 0 or 1
  RFT_WRITE_SUCCESS if sig_to_follow was in range and was written successfully

EXAMPLE 

  rft_upc_gain_ctl(0);   sets the UPC Gain Control to follow the UPC 

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_upc_gain_ctl( boolean sig_to_follow );

/*==============================================================================

FUNCTION

  RFT_LO_GEN_TANK_CTL_CONF

PROTOTYPE

  byte rft_lo_gen_tank_ctl_conf( byte register_val)

DESCRIPTION

  This function configures the LO Generation Tank Control Register

PARAMETERS

  byte register_val
  a number between 0 and 255

RETURN VALUE

  RFT_WRITE_SUCCESS if register_val was in range and was written successfully

EXAMPLE

  rft_lo_gen_tank_ctl_conf(0x3F);   configures 0x3F into the register 

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_lo_gen_tank_ctl_conf( byte register_val);

/*==============================================================================

FUNCTION

  RFT_UPC_TANK_ADJ_CONF

PROTOTYPE

  byte rft_upc_tank_adj_conf( byte register_val)

DESCRIPTION

  This function configures the UPC Tank Adjustment register

PARAMETERS

  byte register_val
  a number between 0 and 255

RETURN VALUE

  RFT_WRITE_SUCCESS if register_val was in range and was written successfully

EXAMPLE

  rft_upc_tank_adj_conf(0x3F);   configures 0x3F into the register 

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_upc_tank_adj_conf( byte register_val);

/*==============================================================================

FUNCTION

  RFT_LOCK_DET_SET

PROTOTYPE

  byte rft_lock_det_set( boolean enable)

DESCRIPTION

  This function configures Lock Detect to be enabled or disabled

PARAMETERS

  boolean enable / disable
  enable: enables the lock detect
  disable: disables the lock detect

RETURN VALUE

  RFT_WRITE_SUCCESS

EXAMPLE

  rft_lock_det_set(TRUE);   //will disable the TX pll LOCK detect 

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_lock_det_set(boolean enable);

/*==============================================================================

FUNCTION

  RFT_TX_LOCK_DET_COUNTER_LENGTH_SET

PROTOTYPE

  byte rft_tx_lock_det_counter_length_set( byte value)

DESCRIPTION

  This function will set the counter length of the TX PLL lock detect.
  A value between 0x0 and 0xF is programmed, for a corrisponding KLD value.

PARAMETERS

  byte value.  MUST BE between 0x0 and 0xF.
  The value-KLD table is:

  value   0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
  KLD    20  24  28  32  40  48  56  64  80  96  112 128 160 192 224 256

RETURN VALUE

  RFT_WRITE_SUCCESS for a valid value between 0x0 and 0xF, and
  RFT_ERROR_CODE_1 if the value is out of range

EXAMPLE

  // this will set the TX pll counter length to 56 
  rft_tx_lock_counter_length_set(0x06);

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_tx_lock_det_counter_length_set(byte value);

/*==============================================================================

FUNCTION

  RFT_RX_LOCK_DET_COUNTER_LENGTH_SET

PROTOTYPE

  byte rft_rx_lock_det_counter_length_set( byte value)

DESCRIPTION

  This function will set the counter length of the RX PLL lock detect.
  A value between 0x0 and 0xF is programmed, for a corrisponding KLD value.

PARAMETERS

  byte value.  MUST BE between 0x0 and 0xF.
  The value-KLD table is:

  value   0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
  KLD    20  24  28  32  40  48  56  64  80  96  112 128 160 192 224 256

RETURN VALUE

  RFT_WRITE_SUCCESS for a valid value between 0x0 and 0xF, and
  RFT_ERROR_CODE_1 if the value is out of range

EXAMPLE

  // this will set the RX pll counter length to 56
  rft_rx_lock_counter_length_set(0x06);

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_rx_lock_det_counter_length_set(byte value);

/****************************************************************************/
/****************************************************************************/
/*         LNA                                                              */
/****************************************************************************/
/****************************************************************************/
/*==============================================================================

FUNCTION

  RFL_GAIN_STATE_SET

PROTOTYPE

  byte rfl_gain_state_set( rfl_gain_state_type gain_state )

DESCRIPTION
  
  This function programs the gain state of the LNA

PARAMETERS

  rfl_gain_state_type gain_state

  typedef enum{
    RFL_MAX_GAIN,           
    RFL_MID_GAIN,          
    RFL_LOW_GAIN,         
    RFL_DEFAULT_LOW_GAIN 
  } rfl_gain_state_type
  
 

RETURN VALUE

  RFL_ERROR_CODE_2  if gain_state is not a member of the enum
  RFL_WRITE_SUCCESS if gain_state was in range and was written successfully

EXAMPLE 

  sets the LNA gain state to MID GAIN 
  rfl_gain_state_set(RFL_MID_GAIN);

SIDE EFFECTS
  None.

==============================================================================*/
byte rfl_gain_state_set(rfl_gain_state_type gain_state, rfl_device_enum_type rfldev);

/*==============================================================================

FUNCTION

  RFL_IDLE_SEL

PROTOTYPE

  byte rfl_idle_sel( boolean idle_sel )

DESCRIPTION
  
  This function sets the idle bit

PARAMETERS

  boolean idle_set

  0 - Rx Only
  1 - Rx/Tx draw more current

RETURN VALUE

  RFL_ERROR_CODE_1  if idle_sel is not a 0 or 1
  RFL_WRITE_SUCCESS if idle_sel was in range and was written successfully

EXAMPLE 

  rfl_idle_sel(0);   puts chip in Rx only mode 

SIDE EFFECTS
  None.

===========================================================================*/
byte rfl_idle_sel( boolean idle_sel, rfl_device_enum_type rfldev );


/*==============================================================================

FUNCTION

  RFL_SLEEP_SEL

PROTOTYPE

  byte rfl_sleep_sel( boolean sleep_sel )

DESCRIPTION
  
  This function sets the sleep bit

PARAMETERS

  boolean sleep_set

  0 - sleep  mode
  1 - active mode

RETURN VALUE

  RFL_ERROR_CODE_1  if sleep_sel is not a 0 or 1
  RFL_WRITE_SUCCESS if sleep_sel was in range and was written successfully

EXAMPLE 

  rfl_sleep_sel(0);   puts chip in sleep mode 

SIDE EFFECTS
  None.

===========================================================================*/
byte rfl_sleep_sel( boolean sleep_sel, rfl_device_enum_type rfldev );

/*==============================================================================

FUNCTION

  RFL_R_SET

PROTOTYPE

  byte rfl_r_set( boolean r_val )

DESCRIPTION
  
  This function sets the r value (for CLNA gain/linearity)

PARAMETERS

  boolean r_val

  0 - default  value of R
  1 - increase value of R

RETURN VALUE

  RFL_ERROR_CODE_1  if r_val is not a 0 or 1
  RFL_WRITE_SUCCESS if r_val was in range and was written successfully

EXAMPLE 

  rfl_r_set(0);   sets R to its default value 

SIDE EFFECTS
  None.

===========================================================================*/
byte rfl_r_set(boolean r_val, rfl_device_enum_type rfldev);


/*========================================================================

FUNCTION rfr_adjust_rfr_current

DESCRIPTION
  This function is used to adjust registers that affect current consumption.

  RFR6125
  -------
  
  Bits [11] of tweaks adjusts LO current
  Bits [10:8] of tweaks adjusts CLNA current
  Bits [7:3] of tweaks adjusts GMBIAS_CTRL
  Bits [2:0] of tweaks adjusts UHF VCO core current

  Note: When bit D[11] is set to "0" the setting in bits D[2:0] (UHF VCO core current
        are ignored

        When bit D[11] is set to "1" the setting in bits D[2:0] (UHF VCO core current
           cannot be '011' or the stand LO current settings will be used instead

  RFR6135
  -------
  tweaks:
  * Bits 2:0  LNA Bias current Rx/Tx mode 
     111 Min Current
     000 MAX Current

  * Bits 5:3  LNA Bias current Rx Only mode
     111 Min Current
     000 MAX Current

  * Bits 11:6 VCO Current control
  Increase in this value increases VCO current 
    a) 8:6 VCO BIAS Reference Current
       QC recommended value = 100b
    b) 11:9 VCO core class AB limiting current
       QC recommended value = 000b

  * Bits 15:12
  Not used

  RFR6155 & RFR6185
  -----------------

    Bits <12:9> map  to adjust CDMA VCO core current

    Bits <8:3> map  to adjust LO driver current per LNA gain state

    Bits <2:0> map  to adjust LO driver current 


  
DEPENDENCIES
  Should be called after every time a mode is programmed.
  Proper mode (IMT/USPCS/KPCS/...) should be programmed before calling this function.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_adjust_rfr_current( word tweaks, rfr_device_enum_type rfr);

/*========================================================================

FUNCTION rfr_adjust_additional_rfr_current

DESCRIPTION
  This function is used to adjust additional registers that affect current consumption. This
  function is in addition to rfr_adjust_rfr_current().

  Only valid for RFR6155 for Now.

  RFR6155 & RFR6185
  -----------------

    Bits <23:20> map  to adjust buffer current

    Bits <19:16> map  to adjust mixer core current

    Bits <15:14> map  to adjust TIA OPAMP current

    Bits <13:10> map  to adjust mixer bias

    Bits <9:8> map  to Set PCS LNA

    Bits <7:7> map  to set PCS LNA current per setting in 0x70<7:6>

    Bits <6:6> map  to enable/disable IM3 cancellation

    Bits <5:2> map  to adjust GPS BB filter current

    Bits <1:0>  not used.

DEPENDENCIES
  Should be called after every time a mode is programmed.
  Proper mode (IMT/USPCS/KPCS/...) should be programmed before calling this function.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  rfr_adjust_additional_rfr_current( uint32 tweaks,  rfr_device_enum_type rfr );

/*========================================================================

FUNCTION rfr_enable_im2_cal

DESCRIPTION
  This function enables or disables IM2 Cal.

  Only valid for RFR6155 & RFR6185 for Now.

PARAMETERS
  enable = TRUE enables IM2 Cal
  disable = FALSE disables IM2 Cal

DEPENDENCIES
  Should be called after every time a mode is programmed.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  rfr_enable_im2_cal( boolean enable,  rfr_device_enum_type rfr );


/*===========================================================================

FUNCTION    SYNTH_VCO_COARSE_TUNE_ADJUST

DESCRIPTION
  This function is used to adjust coarse tune for CDMA and GPS

  RFR6155 & RFR6185
  -----------------
    Bits <4:3> map  to adjust VCO Coarse Tuning timing control

    Bits <2:0> map  to adjust VCO Vtune voltage adjust


    If current mode is CDMA it will affect CDMA settings, If current mode is GPS 
    it will affect GPS setting.

DEPENDENCIES
  This function should be called after synth_rfrx_pll_load. 
  synth_rfrx_pll_load function loads the default setting.

  RFR6155 and RFR6185 only.

RETURN VALUE
  SYNTH_ERROR_CODE_2  if synth device is not supported by this API
  SYNTH_WRITE_SUCCESS if VCO adjust was written successfully

SIDE EFFECTS
  None.

===========================================================================*/
byte synth_vco_coarse_tune_adjust( word tweaks,  synth_device_enum synth );

/*========================================================================

FUNCTION rfr_set_lo_sleep_mode

DESCRIPTION
  This function is used to adjust set the properties of the LO section 
  when the RFR is in sleep mode.  When passed TRUE for on_off the LO 
  section is powered down when the sleep bit is set. When passed FALSE the
  LO section is not powered dwon when the sleep bit is set (assuming the
  power supply connected to the pins supplying power to the LO circuits
  is on)
  
  NOTE:  This API only applies to RF configurations that use the 
         RFR6120/RFR6125

DEPENDENCIES
  Should be called after every time a mode is programmed

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_set_lo_sleep_mode( boolean on_off, rfr_device_enum_type rfr );

/*==============================================================================

FUNCTION RFR_SET_PRE_LNA_ON_OFF

DESCRIPTION
  This function is called to set pre-lna setting, if applicable
  To switch pre-lna on input should be TRUE.
  To switch pre-lna off input should be FALSE.
  
DEPENDENCIES
  This should be called after mode is set. This will ensure these 
  settings are not overwritten.
  
  For RFR6135 only for now.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void rfr_set_pre_lna_on_off( boolean on_off, rfr_device_enum_type rfr );

/*==============================================================================

FUNCTION

  SYNTH_RX_ICP_SET

PROTOTYPE

    byte synth_rx_Icp_set( byte M_value, synth_device_enum synth )

DESCRIPTION

  This function sets the charge pump output current of the receiver
  The user passes in the M_value, which translates into the Icp via the equation:

  Icp = (M+1) * 62.5 uA

  This function is valid for RFR6135 only - for now.

DEPENDENCIES
  This function should be called after synth_rfrx_pll_load. 
  synth_rfrx_pll_load function loads the default setting for
  charge pump current.

PARAMETERS

  byte M_value - an integer between 0 and 63
  synth_device_enum synth - synth device enum eg. RFR6135_SYNTH

RETURN VALUE

  SYNTH_ERROR_CODE_1  if M_value is out of range
  SYNTH_ERROR_CODE_2  if synth device is not supported by this API
  SYNTH_WRITE_SUCCESS if M_value was in range and was written successfully
  

EXAMPLE 

  M = 0 to 63
  Icp = (M+1) * 62.5 uA
  Icp = 1.0625mA  -> M = 16
  user programs 16

  synth_rx_Icp_set(16, RFR6135_SYNTH);

SIDE EFFECTS
  None.

===========================================================================*/
byte synth_rx_Icp_set( byte M_value, synth_device_enum synth );

/*==============================================================================

FUNCTION RFT_SET_VCO_BIAS

DESCRIPTION
  This function is called to change the RFT VCO Bias.
      
DEPENDENCIES
  For RFT6150 only for now.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void rft_set_vco_bias(void);

/*==============================================================================

FUNCTION RFR_SELECTS_LL_MATCHING

DESCRIPTION
  This function is called to select L-L as LNA output matching topology on the 
  RF card. Applies to RFR6185 PCS band only.
      
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rfr_selects_LL_matching(boolean LL_overrides_LC);


/*==============================================================================

FUNCTION

  RFT_DA_SELECT_SET

PROTOTYPE

  byte rft_da_select_set( rf_card_band_type band,
                          rft_da_select_type da_select )

DESCRIPTION
  
  This function programs the DA selection control.  RFT/RTRs that support
  bypass PAs can configure the polarity of the PA Range pin that selects
  DA1/DA2 output, or ignore the PA Range selection and force the selection
  of one DA output only.  Using an RFT/RTR with bypass support on a non-
  bypass PA will most likely require using this API.

  The API allows independent DA configuration per band.  The configuration
  is "sticky," and need only be set once per target configuration.  The
  recommended location to call this API is during RF Card initialization.

PARAMETERS

  rf_card_band_type band

  rft_da_select_type da_select

  typedef enum {
  RFT_DA_SELECT_DA_2_ALWAYS,  
  RFT_DA_SELECT_DA_1_ALWAYS,  
  RFT_DA_SELECT_DA_2_PA_R0,   
  RFT_DA_SELECT_DA_1_PA_R0,   
  RFT_DA_SELECT_DA_2_PA_R1,   
  RFT_DA_SELECT_DA_1_PA_R1    
  } rft_da_select_set;
  
RETURN VALUE

  RFT_ERROR_CODE_1  if this chip does not support DA selection control
  RFT_ERROR_CODE_2  if da_select is invalid for a supported chip
  RFT_WRITE_SUCCESS if da_select was in range and was written successfully

EXAMPLE 

  rft_da_select_set(RF_BC0_BAND, RFT_DA_SELECT_DA_1_ALWAYS);
    Tx will always use DA1 for output in BC0.

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_da_select_set( rf_card_band_type band, rft_da_select_type da_select );

/*==============================================================================

FUNCTION

  RFT_PA_SELECT

PROTOTYPE

  byte rft_pa_select( rft_pa_select_type pa_select )

DESCRIPTION
  
  This function Selects for Non Bypass and Bypass PAs

PARAMETERS

  rft_pa_select_type pa_select

  typedef enum {
     RFT_BYPASS_PA,
     RFT_NON_BYPASS_PA 
  } rft_pa_select_type;
  
RETURN VALUE

  RFT_ERROR_CODE_1  if this chip does not support DA selection control
  RFT_ERROR_CODE_2  if da_select is invalid for a supported chip
  RFT_WRITE_SUCCESS if da_select was in range and was written successfully


SIDE EFFECTS
  None.

===========================================================================*/
byte rft_pa_select( rf_card_band_type band, rft_pa_select_type pa_select );

#ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
/*==============================================================================

FUNCTION

  RFT_CONFIGURE_TX_PDM_TABLES

DESCRIPTION
   This function Correctly assigns the correct Tx AGC Table for a given Band Class
  for different PA Gain states

PARAMETERS
  Band Class for which the tables need to be assigned for different gain states
  
RETURN VALUE

  RFT_ERROR_CODE_1  if this chip does not TX AGC PDM Tables  
  RFT_WRITE_SUCCESS if da_select was in range and was written successfully

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_configure_tx_pdm_tables(rf_card_band_type band);
#endif

/*==============================================================================

FUNCTION

  RFT_DA_SELECT_SET_BIAS

PROTOTYPE

  byte rft_da_select_set_bias( rf_card_band_type band,
                          rft_da_select_bias da_bias )

DESCRIPTION
  
  This function programs the bias selection for the DA.  

  The API allows independent DA bias configuration per band.  The configuration
  is "sticky," and need only be set once per target configuration.  The
  recommended location to call this API is during RF Card initialization.

PARAMETERS

  rf_card_band_type band

  rft_da_select_type da_bias

  typedef enum {
  RFT_DA_SELECT_LOW_BIAS_ALWAYS,   
  RFT_DA_SELECT_HIGH_BIAS_ALWAYS,  
  RFT_DA_SELECT_LOW_BIAS_PA_R0,    
  RFT_DA_SELECT_HIGH_BIAS_PA_R0,   
  RFT_DA_SELECT_LOW_BIAS_PA_R1,    
  RFT_DA_SELECT_HIGH_BIAS_PA_R1    
  } rft_da_select_bias;
  
RETURN VALUE

  RFT_ERROR_CODE_1  if this chip does not support DA selection control
  RFT_ERROR_CODE_2  if da_select is invalid for a supported chip
  RFT_WRITE_SUCCESS if da_select was in range and was written successfully

EXAMPLE 

  rft_da_select_set_bias(RF_BC0_BAND, RFT_DA_SELECT_LOW_BIAS_ALWAYS);
    Tx will always use low bias for DA.

SIDE EFFECTS
  None.

===========================================================================*/
byte rft_da_select_set_bias( rf_card_band_type band, rft_da_select_bias da_bias );

void rft_set_pa_bypass_config(rf_card_band_type band,rft_pa_bypass_config_type bypass_pa);

/*==============================================================================

FUNCTION

  RFT_DA_SELECT_SET

PROTOTYPE

  byte rft_pa_select( rft_pa_select_type pa_select )

DESCRIPTION
  
  This function Selects for Non Bypass and Bypass PAs

PARAMETERS

  RFT_SINGLE_STATE_PA,
  RFT_DUAL_STATE_PA,
  RFT_TRI_STATE_PA,
  RFT_QUAD_STATE_PA

  
RETURN VALUE

  RFT_ERROR_CODE_1  if this chip does not support DA selection control
  RFT_ERROR_CODE_2  if da_select is invalid for a supported chip
  RFT_WRITE_SUCCESS if da_select was in range and was written successfully


SIDE EFFECTS
  None.

 
===========================================================================*/
byte rft_num_pa_states_set( rf_card_band_type band, rft_pa_states_type pa_states );


/*==============================================================================

FUNCTION ziflib_get_rx_spectral_inversion

DESCRIPTION
  This function returns the spectral inversion settings based on path and band used.
  Currently this function is implemented only for RTR6500-GPS band. For RTR6500-GPS 
  band, spectral inversion can change during runtime. For other chipsets  if this 
  functionality is needed, this API should be extended.

DEPENDENCIES
  None

RETURN VALUE
  Return the spectral inversion (enc_rx_polarity_type) settings.

SIDE EFFECTS
  None.

===========================================================================*/
enc_rx_polarity_type ziflib_get_rx_spectral_inversion(  rf_path_enum_type path,
                                                        rf_card_band_type band );

#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */

/*===========================================================================

FUNCTION rfr_set_lo_cal_chan                

DESCRIPTION
    Initializes an array used by LO PWC cal with the cal channels to use for each band.
  
DEPENDENCIES
    None

RETURN VALUE
    None

Parameters Passed
    band = Input. The supported band for RF.
    chan = Input. The channel where to perform LO PWC cal.

SIDE EFFECTS
    See description.

===========================================================================*/
void rfr_set_lo_cal_chan( rf_card_band_type band, int chan );

/*===========================================================================

FUNCTION rfr_adjust_lo_pwc_dac_codes                

DESCRIPTION
    Performs PWC DAC codes updates based on temperature.
  
DEPENDENCIES
    None

RETURN VALUE
    None

Parameters Passed
    rf_path_enum_type path = Input. The current rf path.

SIDE EFFECTS
    See description.

===========================================================================*/
void rfr_adjust_lo_pwc_dac_codes( rf_path_enum_type path );

#endif /* RFZIFAPI_H */


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    R F C _ Q S C 6 0 5 5  D R I V E R

GENERAL DESCRIPTION
  This file contains constant and function definitions to support interaction
  with the Qualcomm QSC11x0 RF Module.  

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c)  2006, 2007, 2008 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c)  2009             by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfc_qsc1100.c#30 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/09   vb      Fix added to avoid warning in certain build flavors
02/05/09   vb      Added support for RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA
                   Fix added to avoid warning in certain build flavors
                   20 ms Hysteresis timer for all types of PA for all bands
01/22/09   vb      Removed RF APIs that are no longer needed
01/09/09   vb      Changed the parameters for rfr_configure_device_params_3  in PCS band
12/10/08   vm      Added the support for new RF APIs
12/08/08   vb      Added support for missing Channels in BC0 and BC1
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
11/18/08   lp      Added support for variable SMPS level
11/18/08   lp      Fixed Klocwork error and compiler errors
11/14/08   vm      Added the support for Variable Switcher Size and Frequency
11/07/08   vm      Program the SMPS Supply voltage S1 to 1.2 V for Cell Band
                   and 1.3 V for all other Bands
10/14/08   vm      Added support for PDET Waveform Compensaion
10/13/08   vb      Added support for more channels in BC5
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
09/25/08   vm      Added support for Driver Amp selection for Bypass and Non Bypass PA
09/14/08   vm      Added the support for TCXO Shutdown
09/14/08   vm      Added the support for BC6 Band
08/29/08   vm      Fix for issue that phone is not making call
                   on Power grid modified FFA
08/06/08   vm      Changed the PA Hysterisis timer settings for QSC1100
07/25/08   ems     Addded corrections for BC5 NV
07/24/08   vm      Changed PA Hysterisis timer for QSC1100
07/23/08   vm      Changed PA Hysterisis Timer to 20 ms for QSC1100
07/22/08   vm      Added customer interface APIs for QSC1100
06/13/08   vm      Changed RFA1 to 1.3 V for QSC1100
05/05/08   vm      Added more support for Bringup
04/28/08   sl      Added LO PWC Cal support.
04/16/08   vm      Added support for QSC1100 Bringup
03/28/08   vm      Added the support for Tx AGC interface with MDSP for QSC1100
03/05/08   vm      Made changes for Napolean and RICO Bringup
                   Disbale the PMIC SMPS frequency and LDO Management for RICO
11/19/07   ems     Ported from SC2X 3044
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "cagc.h"      
#include "rficap.h"     /* RF capabilities */
#include "rfi.h"
#include "err.h"        /* Errors log & messages */
#include "enc.h"        /* Encoder driver APIs */
#include "rxf.h"
#include "msm_help.h"
#include "rfc_qsc1100.h"
#include "rf_cards.h"
#include "pm.h"
#include "rf_intelliceiver.h"
#include "rf_error.h"
#include "rfzifapi.h"
#include "rf_adc_samp_freq.h"
#include "enc.h"
#ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
#include "srch_rx_div.h"
#endif
#include "rfr.h"
#ifdef RF_HAS_RFCONFIG_RX_LNA
#include "rfconfig_rx_lna.h"
#include "rfconfig_qsc11x0.h"
#include "rfnv_cdma1x_bc0.h"
#include "rfnv_cdma1x_bc1.h"
#include "rfnv_cdma1x_bc5.h"
#include "rfnv_rfconfig_migration_tracker.h"
#endif /* RF_HAS_RFCONFIG_RX_LNA */

#define RF_HAS_PMIC_VREG_POWERUP_SEQUENCE  //need to move to rficap
#define RF_PA_RISE_MAX_VALUE  255

/* Enable Tx VREG with Rx */
#define RF_HAS_TX_WITH_RX_VREG_ENABLE

//#define RF_MANAGED_LDO_DEBUG
#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif /* RF_MANAGED_LDO_DEBUG */

#include "ftm.h"
extern ftm_mode_type  ftm_mode;
extern boolean rf_qsc11x0_sleep_enable_flag;

#ifdef RF_HAS_BC0_NV_TABLE
extern rfnv_digital_items_type rfnv_cdma1x_bc0_tbl;
#endif
#ifdef RF_HAS_BC1_NV_TABLE
extern rfnv_digital_items_type rfnv_cdma1x_bc1_tbl;
#endif
#ifdef RF_HAS_BC5_NV_TABLE
extern rfnv_digital_items_type rfnv_cdma1x_bc5_tbl;
#endif
#ifdef RF_HAS_BC6_NV_TABLE
#error code not present
#endif

#ifndef RF_HAS_TCXO_SHUTDOWN_DISABLED
extern boolean clkregime_power_on_flag_is_set(void);
extern void clkregime_power_on_flag_clear(void);
#endif /* RF_HAS_TCXO_SHUTDOWN_DISABLED*/

#ifndef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#define BC5_PA_ON_SIG      GPIO_OUTPUT_38
#define PA_R0_SWITCH_SIG   GPIO_OUTPUT_37
#define BC1_PA_ON_SIG      GPIO_OUTPUT_36
#define BC0_PA_ON_SIG      GPIO_OUTPUT_35
#endif

#ifndef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
enc_rx_polarity_type rf_debug_qsc11x0_rx_spectral_norm_pcs = ENC_RX_SPECTRAL_NORM;
enc_rx_polarity_type rf_debug_qsc11x0_rx_spectral_norm_cell = ENC_RX_SPECTRAL_NORM;

enc_tx_polarity_type rf_debug_qsc11x0_tx_spectral_norm_bc1 = ENC_TX_SPECTRAL_NORM;
enc_tx_polarity_type rf_debug_qsc11x0_tx_spectral_norm_bc0 = ENC_TX_SPECTRAL_NORM;
enc_tx_polarity_type rf_debug_qsc11x0_tx_spectral_norm_bc5 = ENC_TX_SPECTRAL_NORM;
#endif
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/*************************************
   RF CARD SPECIFIC VREG SETTINGS
**************************************/
#ifdef RF_HAS_VREG_VOLTAGE_MANAGEMENT
/* RFA1 Voltage setting  = 1.3v */
#define RFC_RFA1_LEVEL  1300

/* RFA2 Voltage setting  = 1.3v */
#define RFC_RFA2_LEVEL  2200

/* RF VREG/SMPS */
#ifndef RF_HAS_SMPS_RF_VDD
  /* RF SMPS is connected to RF LDOs at 2.25v*/
#define RFC_VREG_RF_LEVEL 2250
#else
  /* RF SMPS is directly connected to RF devices at 2.1v*/
  #define RFC_VREG_RF_LEVEL 2100
#endif /* !RF_HAS_SMPS_RF_VDD */
  
/* RFRX1 Voltage setting = 2.1v */
#define RFC_RFRX1_LEVEL 2100
/* RFRX2 Voltage setting = 2.1v */
#define RFC_RFRX2_LEVEL 2100
/* RFTX Voltage setting  = 2.1v */
#define RFC_RFTX_LEVEL  2100
/* RF VCO Voltage setting = 2.1v */
#define RFC_RF_VCO_LEVEL 2100
/* MSM Analog Voltage setting = 2.1v */
#define RFC_MSMA_LEVEL 2100

#endif /* RF_HAS_VREG_VOLTAGE_MANAGEMENT */

#define RFC_RF_SMPS_S1_LEVEL 1300

/* 
* GPIO Configurations for TRI-BAND 
*
* 4 GPIOs used to select Rx input between BC0, BC1, BC15 and GPS
* The GPIO GPS_SWITCHING (52) is used by both DUAL and TRI-Band HW
*/


/* ------------------------------------------------------------------- */
/*              T I M E   C O N S T A N T S                            */
/* ------------------------------------------------------------------- */

/* time in usec for RF to settle after rf_tune_to_chan() returns */
#define QSC11X0_TUNE_TO_CHAN_TIME_USEC ((word)2100)

/* time in usec for a band change in rf_digital_band_select() */
#define QSC11X0_BAND_CHANGE_TIME_USEC  ((word)4000)

/* Time in usec for the PA thermistor to reach steady state */
#define QSC11X0_THERM_VREG_ENABLE_TIME_USEC 30
                                    

/*===========================================================================
                         LOCAL FUNCTION PROTOTYPES
===========================================================================*/
#ifdef RF_HAS_RFCONFIG_RX_LNA
static void rfc_qsc1100_init_rx_lna_rfconfig_data(void);
#endif /* RF_HAS_RFCONFIG_RX_LNA */

extern void qsc11x0_rx_prx_init(void);
extern void qsc11x0_tx_preset_init(void);
extern void qsc11x0_rx_drx_init(void);


/*===========================================================================

                              DATA DEFINITIONS

===========================================================================*/
extern rf_adc_clk_state_type rf_adc_clk_status;

/* Configuration of RF chains */
extern rf_path_0_config_type rf_path_0;  
extern rf_path_1_config_type rf_path_1;

extern rf_card_type rf_card;

/* Internal state for tracking the last configured band on the
   diversity path */
typedef enum  {
  QSC11X0_EXT_LO_BAND_PCS,
  QSC11X0_EXT_LO_BAND_CELL
} rfc_qsc11x0_ext_lo_band_type;

static struct
{
  boolean external_vco_enabled; /*lint -e754 Not referenced externally */
  rfc_qsc11x0_ext_lo_band_type ext_lo_band;
} rfc_qsc11x0;

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
/* Radio Configuration 3 compensation values for HDET. */
/* All Compensation values are in 100th of a dBm. */
static rfc_hdet_rc3_comp_values_type rfc_rc3_hdet_comp_values_table = 
{ /* R-SCH and R-FCH. */
  {0,    /* 1/8 rate  */
   0,    /* 1/4 rate  */
   0,    /* 1/2 rate  */
   30,   /* Full rate */
   27,   /* 2X rate   */
   26,   /* 4X rate   */
   0,   /* 8X rate   */
   -4    /* 16X rate  */
#ifdef FEATURE_32X_SCH_DATA_RATE
   ,0},  /* 32X rate  */
#else
   },
#endif /* FEATURE_32X_SCH_DATA_RATE */
  /* R-SCH and DCCCH on. */
  {0,    /* 1/8 rate  */
   0,    /* 1/4 rate  */
   0,    /* 1/2 rate  */
   0,   /* Full rate */
   0,    /* 2X rate   */
   0,    /* 4X rate   */
   0,    /* 8X rate   */
   0     /* 16X rate  */
#ifdef FEATURE_32X_SCH_DATA_RATE
   ,0},  /* 32X rate  */
#else
   },
#endif /* FEATURE_32X_SCH_DATA_RATE */
  /* R-FCH only. */
  {-9,
   -9,
   -13,
   -1
  },
  /* DCCCH only  */
  {0}
};
#ifdef DISABLE_GET_HDET_THERM_COMP_VALUE_FUNCTION
/* Temperature Compensation values for HDET. */
static rfc_hdet_therm_comp_values_type rfc_hdet_therm_comp_values_table[] = \
{  /* Raw Temperature -  Compensation Value in 10th of a dBm.
                                BC0             BC1 */
  { /*68C  =*/ 211,            {45,             65}},
  { /*54C  =*/ 203,            {35,             50}},
  { /*40C  =*/ 195,            {25,             35}},
  { /*26C  =*/ 187,            { 0,              0}},
  { /*12C  =*/ 179,            {-10,            -15}},
  { /*-2C  =*/ 171,            {-25,           -35}},
  { /*-16C =*/ 163,            {-35,           -60}},
  { /*-30C =*/ 155,            {-60,           -95}}
};
#endif /* DISABLE_GET_HDET_THERM_COMP_VALUE_FUNCTION */
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */
#ifdef FEATURE_XO
boolean xotherm_adc_pwr_on = FALSE;
#endif

/*===========================================================================

                         LOCAL FUNCTION PROTOTYPES

===========================================================================*/

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)

/* Function is declared and not used. Disabled function */
#ifdef DISABLE_GET_HDET_THERM_COMP_VALUE_FUNCTION
static int16 rfc_qsc11x0_get_hdet_therm_comp_value(int16             temp, \
                                                   rf_card_band_type band);
#endif
static int16 rfc_qsc11x0_get_hdet_rc_comp_value(                           \
                                         cai_radio_config_type rc,         \
                                         enc_sch_rate_type     r_sch_rate, \
                                         enc_rate_type         r_fch_rate, \
                                         boolean               dcch_on,    \
                                         boolean               r_fch_on,   \
                                         boolean               r_sch_on);


#ifdef RF_HAS_HDR
#error code not present
#endif

#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */
/*===========================================================================

                           FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION    QSC11X0_INIT_CARD

DESCRIPTION
  Sets up the internal structures to control QSC11x0/RaMSIS RFA device on
  SC2x.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_init_card(void *hw)
{
    rf_card_type *rfhw = (rf_card_type *) hw;
    int16 i;    

    #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE
    pm_err_flag_type pm_level_error = PM_ERR_FLAG__SUCCESS;
    #endif

    /* Make sure the NULL pointer condition does not exist */
    if (rfhw == NULL)
    {
      RF_ASSERT(rfhw!=NULL);
      return;   
    }

    /*lint -save -e613 */

    /* Setup the card related function pointers */
    if(rf_card_in_system == RF_HW_QSC11X0_TRI_BAND)
    {
      rfhw->rf_hw_id                      = rf_card_in_system;
    }
    else if (rf_card_in_system == RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA)
    {
      rfhw->rf_hw_id                      = rf_card_in_system;
    }
    else 
    { /* This is the default hw configuration */
      rfhw->rf_hw_id                      = RF_HW_QSC11X0_DUAL_BAND;
    }

    rfhw->rfc_default_band                = RF_BC0_BAND;
    rfhw->rfc_tune_to_chan_time_usec      = (word) QSC11X0_TUNE_TO_CHAN_TIME_USEC;
    rfhw->rfc_band_change_time_usec       = (word) QSC11X0_BAND_CHANGE_TIME_USEC;
    rfhw->rfc_init_card                   = rfc_qsc11x0_init_card;
    rfhw->rfc_set_mode_park               = rfc_qsc11x0_set_mode_park;
    rfhw->rfc_set_mode                    = rfc_qsc11x0_set_mode;
    rfhw->rfc_select_primary_vco          = rfc_qsc11x0_select_primary_vco;
    rfhw->rfc_select_secondary_vco        = rfc_qsc11x0_select_secondary_vco;
    rfhw->rfc_rft_band_A_select           = (rfc_rft_band_A_select_type)rfc_empty_func;
    rfhw->rfc_rft_band_B_select           = (rfc_rft_band_B_select_type)rfc_empty_func;
    rfhw->rfc_enable_primary_chain_vco    = (rfc_enable_primary_chain_vco_type)rfc_empty_func;
    rfhw->rfc_enable_secondary_chain_vco  = rfc_qsc11x0_enable_secondary_chain_vco;
    rfhw->rfc_disable_primary_chain_vco   = (rfc_disable_primary_chain_vco_type)rfc_empty_func;
    rfhw->rfc_disable_secondary_chain_vco = rfc_qsc11x0_disable_secondary_chain_vco;
    rfhw->rfc_get_hdet_adc_channel        = rfc_qsc11x0_get_hdet_adc_channel;
    rfhw->rfc_manage_vregs                = rfc_qsc11x0_manage_vregs;
    rfhw->rfc_uses_external_vco           = rfc_qsc11x0_uses_external_vco;
    #ifdef RF_HAS_HDET_COMP_FOR_RC_AND_TEMP
    rfhw->rfc_get_hdet_comp_value         = rfc_qsc11x0_get_hdet_comp_value;
    #ifdef RF_HAS_HDR
#error code not present
    #endif /* RF_HAS_HDR */
    #endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */
    
    #ifdef RF_HAS_INTERNAL_HDET_SUPPORT
    rfhw->rfc_uses_internal_hdet          = qsc11x0_uses_internal_hdet;
    #endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

    #ifdef RF_HAS_INTERNAL_THERM_SUPPORT
    rfhw->rfc_uses_internal_therm          = qsc11x0_uses_internal_therm;
    rfhw->rfc_read_internal_therm          = qsc11x0_read_internal_therm;
    #endif /* RF_HAS_INTERNAL_THERM_SUPPORT */
    
    #ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
    rfhw->rfc_uses_device_info_for_spectral_inv = rfc_qsc11x0_set_spectral_inv;
    #endif
    #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
    rfhw->rfc_set_pa_access_probe_config =   rfc_qsc11x0_set_pa_access_probe_config;
    #endif
    #ifdef RF_HAS_RFCONFIG_RX_LNA
    rfhw->rfc_init_rfconfig_rx_lna       =   rfc_qsc1100_init_rx_lna_rfconfig_data;
    #endif /* RF_HAS_RFCONFIG_RX_LNA */

    #ifdef RF_HAS_WU_OPTIMIZED
#error code not present
    #endif /* RF_HAS_WU_OPTIMIZED */

    #ifdef RF_HAS_SUB_BLOCK_SUPPORT
    rfhw->rfc_get_block = rfc_qsc11x0_get_block;
    rfhw->rfc_block_mask_is_valid = rfc_qsc11x0_block_mask_is_valid;
    rfhw->rfc_set_block = rfc_qsc11x0_set_block;
    #endif
    
    #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
    rfhw->rfc_support_temp_based_device_config = rfc_qsc11x0_support_temp_based_device_config;
    #endif
    /* Configure RFT devices */
    rf_path_0.rftdev = QSC11X0_TX;
    rf_path_0.txpllsynthdev = QSC11X0_TX_SYNTH;
    rf_path_0.rft_serial_device_type = SSBI;
    rf_path_0.rft_sbibus = RFT_SSBI_BUS;
    
    /* Configure RFR devices */
    rf_path_0.rfrdev = QSC11X0_RX;
    rf_path_0.rxpllsynthdev = QSC11X0_RX0_SYNTH;
    rf_path_0.rfr_serial_device_type = SSBI;
    rf_path_0.rfr_sbibus = RFR_SSBI_BUS;


    /* Misc Attributes */
    rfhw->rfc_max_ants = 1;

    /* Set the default to Subclass 0 supported */
    for(i=0;i<(int16)RF_MAX_BAND;i++)
    {
      rfhw->rfc_subclass_mask_tbl[i] = RF_SC_0;
    }

    /* Set subclass masks for unsupported bands that have subclasses to 0 */ 
    //rfhw->rfc_subclass_mask_tbl[RF_BC5_BAND] = 0;

    rfhw->rfc_subclass_mask_tbl[RF_BC0_BAND] = 
        RF_SC_0|
        RF_SC_1;

    rfhw->rfc_subclass_mask_tbl[RF_BC5_BAND] = 
      RF_SC_0|RF_SC_1|RF_SC_2|RF_SC_3|RF_SC_4|RF_SC_5|RF_SC_6|RF_SC_7|RF_SC_8|RF_SC_9|RF_SC_10|RF_SC_11;//Supports all Subclasses 0-11for BC5

    rfhw->rfc_ant_attrib[0].ant_id =  0;
    rfhw->rfc_ant_attrib[0].tunnable[RF_BC0_BAND] = TRUE;
    rfhw->rfc_ant_attrib[0].tunnable[RF_BC1_BAND] = TRUE;
    rfhw->rfc_ant_attrib[0].tunnable[RF_BC5_BAND] = TRUE;


    rfhw->rfc_ant_attrib[0].tunnable[RF_GPS_BAND] = FALSE;

    /* Check for tri-band support (BC0, BC1 and BC15 */
    /*if ( rf_card.rf_hw_id == RF_HW_QSC11X0_TRI_BAND ) 
    {
       rfhw->rfc_ant_attrib[0].tunnable[RF_BC15_BAND] = TRUE;
       rfhw->rfc_ant_attrib[1].tunnable[RF_BC15_BAND] = TRUE;
    }*/

    /*#ifndef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
    gpio_tlmm_config(BC5_PA_ON_SIG);*/  /* PCS  Signal cntrl for PRx */

    /*gpio_tlmm_config(PA_R0_SWITCH_SIG);  */  /* CELL Signal cntrl for PRx */
    
    /*gpio_tlmm_config(BC0_PA_ON_SIG);

    if(rf_card_in_system == RF_HW_QSC11X0_TRI_BAND)
    {
      gpio_tlmm_config(BC1_PA_ON_SIG);
    }
    #endif*/

    #ifdef RF_HAS_VREG_VOLTAGE_MANAGEMENT
    #ifndef QSC11X0_PG2_POWER_GRID

    /* 
    * RF Driver is responsible for configuring PMIC voltages and
    * VREG/LDO behavior
    */  

    /* Set SMPS/VREG Voltage Levels */
    //pm_level_error |= pm_vreg_set_level(PM_VREG_RF_ID, RFC_VREG_RF_LEVEL);

    /* Set LDO Voltage Levels PM_VREG_RFA1_ID
PM_VREG_RFA2_ID
*/
    pm_level_error |= pm_vreg_set_level(PM_VREG_RFA1_ID, RFC_RFA1_LEVEL);
    pm_level_error |= pm_vreg_set_level(PM_VREG_RFA2_ID, RFC_RFA2_LEVEL);

    /*pm_level_error |= pm_vreg_set_level(PM_VREG_RFTX_ID, RFC_RFTX_LEVEL);
    pm_level_error |= pm_vreg_set_level(PM_VREG_RFRX1_ID,RFC_RFRX1_LEVEL);
    pm_level_error |= pm_vreg_set_level(PM_VREG_RFRX2_ID,RFC_RFRX2_LEVEL);
    pm_level_error |= pm_vreg_set_level(PM_VREG_RF_VCO_ID, RFC_RF_VCO_LEVEL);
    pm_level_error |= pm_vreg_set_level(PM_VREG_MSMA_ID,   RFC_MSMA_LEVEL);*/

    /* Assert if not successful VREG setting */
    RF_ASSERT(pm_level_error == PM_ERR_FLAG__SUCCESS);

    /* Configure Pull Downs on the VREG lines to ensure fast voltage decay when turned off */
    (void) pm_vreg_pull_down_switch( PM_ON_CMD, PM_VREG_PDOWN_RFA1_ID );
    (void) pm_vreg_pull_down_switch( PM_ON_CMD, PM_VREG_PDOWN_RFA2_ID );

    /*(void) pm_vreg_pull_down_switch( PM_ON_CMD, PM_VREG_PDOWN_RFRX2_ID );
    (void) pm_vreg_pull_down_switch( PM_ON_CMD, PM_VREG_PDOWN_RFTX_ID );
    (void) pm_vreg_pull_down_switch( PM_ON_CMD, PM_VREG_PDOWN_RF_VCO_ID );*/
    #endif  /* QSC11X0_PG2_POWER_GRID */
    #endif /* RF_HAS_VREG_VOLTAGE_MANAGEMENT */

    /* Configure the QSC11X0 GPIOs */
    //gpio_tlmm_config(GPS_SWITCHING);   /* CDMA DRx / GPS signal select */

    /* Configure additional GPIOs for tri-band hw */
    //if ( rf_card.rf_hw_id == RF_HW_QSC11X0_TRI_BAND ) 
   // {
     //  gpio_tlmm_config(AWS0_PCS0_N);  /* PCS  Signal cntrl for PRx */
     //  gpio_tlmm_config(CELL_HB_N);    /* CELL Signal cntrl for PRx */
     //  gpio_tlmm_config(AWS1_PCS1_N);  /* PCS  Signal cntrl for DRx */
   // }

    rfc_qsc11x0.external_vco_enabled = 0;
    rfc_qsc11x0.ext_lo_band = QSC11X0_EXT_LO_BAND_PCS;   

    /*lint -restore */
    /* 
    *  Set the SMPS frequency configurations.
    */
    #ifndef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
    #ifndef QSC11X0_PG2_POWER_GRID
    //pm_vreg_smps_tcxo_div_sel(PM_VREG_SMPS_TCXO_DIV__12);//1.6 MHz
    pm_vreg_smps_clock_sel( PM_VREG_SMPS_CLK_SEL__TCXO );


    pm_vreg_control( PM_ON_CMD, PM_VREG_RFA2_M);

    /* Enable RF SMPS supply*/
    pm_vreg_control( PM_ON_CMD, PM_VREG_RFA1_M);

    //Do not manage the SMPS CLOCK frequencies for RICO with Voltron
    /*pm_vreg_smps_tcxo_div_sel(PM_VREG_SMPS_TCXO_DIV__8);
    pm_vreg_smps_individual_clock_enable(PM_VREG_SMPS_RF_ID, TRUE); 
    pm_vreg_smps_individual_clock_div_sel(PM_VREG_SMPS_RF_ID, PM_VREG_SMPS_TCXO_DIV__10); 
    pm_vreg_smps_individual_clock_enable(PM_VREG_SMPS_BOOST_ID, TRUE); 
    pm_vreg_smps_individual_clock_div_sel(PM_VREG_SMPS_BOOST_ID, PM_VREG_SMPS_TCXO_DIV__10);*/

    /* Enable ANALOG supply */
    //pm_vreg_control( PM_ON_CMD, PM_VREG_MSMA_M );

    /* Enable RF SMPS supply*/
   // pm_vreg_control( PM_ON_CMD, PM_VREG_RF_M );

    /* Enable RF LDOs */
    /*pm_vote_vreg_switch( PM_ON_CMD, PM_VREG_RF_VCO_ID, PM_VOTE_VREG_RFRX1_APP__RF );
    pm_vote_vreg_switch( PM_ON_CMD, PM_VREG_RFRX1_ID,  PM_VOTE_VREG_RFRX1_APP__RF );
    pm_vote_vreg_switch( PM_ON_CMD, PM_VREG_RFRX2_ID,  PM_VOTE_VREG_RFRX2_APP__RF );
    pm_vote_vreg_switch( PM_ON_CMD, PM_VREG_RFTX_ID,   PM_VOTE_VREG_RFTX_APP__RF );*/
    #endif
    #endif

#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif /* RF_MANAGED_LDO_DEBUG */
   
    #ifndef RF_HAS_SUPPORT_FOR_RICO_BRINGUP   
    /* Sequence the XO RF buffer */
    pm_xo_enable(TRUE);
    pm_xo_rfbuf_enable(TRUE);
    #endif

    #ifdef FEATURE_XO
      xotherm_adc_pwr_on = TRUE;    
    #endif

    /* PA Bypass with 0dB power hysteresis needs 40ms of timer hysteresis */
    /* The hardware resolution for the 1x PA timer is 1.25ms */
   // #ifndef RF_HAS_SUPPORT_FOR_QSC11X0_BRINGUP
                                                                                                
    /* 1xEV-DO PA Bypass with 0dB power hysteresis needs 5ms of timer hysteresis */
    /* The hardware resolution for the 1x PA timer is 1024 chips (833us) */
    rfnv_cdma1x_bc0_tbl.rfnv_1xEV_DO_pa_r_timer = 6; //5/.833
    rfnv_cdma1x_bc1_tbl.rfnv_1xEV_DO_pa_r_timer = 6; //5/.833
    //#else
    //rfnv_cdma1x_bc0_tbl.rfnv_1x_pa_r_timer = 0; //40/1.25
    //rfnv_cdma1x_bc1_tbl.rfnv_1x_pa_r_timer = 0; //40/1.25
                                                   
    /* 1xEV-DO PA Bypass with 0dB power hysteresis needs 5ms of timer hysteresis */
    /* The hardware resolution for the 1x PA timer is 1024 chips (833us) */
    //rfnv_cdma1x_bc0_tbl.rfnv_1xEV_DO_pa_r_timer = 0; //5/.833
    //rfnv_cdma1x_bc1_tbl.rfnv_1xEV_DO_pa_r_timer = 0; //5/.833
    //#endif
    /* 
    * rft_da_select_set()  determines which DA to selsect for a particular Band Configration
    * As per the Dozer and Tank HWSW for Sc2X BCO and BC1 uses Bypass PA
    * We are selecting DA2 to drive the PA in High Power mode for BC0 and BC1
    * DA1 will be driving the Bypass PA in low power mode for BC0 and BC1
    * For BC15 Band no Bypass PAs are being used
    * we are selecting DA2 to always drive 2-state PA on BC15
    */         

     #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE     
     pm_level_error |= pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S1_ID,TRUE);

     pm_level_error |= pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S2_ID,TRUE);
    
     pm_level_error |= pm_vreg_smps_switch_size_set(PM_VREG_SMPS_S1_ID,PM_SMPS_SWITCH_SIZE_1_2);

     pm_level_error |= pm_vreg_smps_switch_size_set(PM_VREG_SMPS_S1_ID,PM_SMPS_SWITCH_SIZE_1_2);

     RF_ASSERT(pm_level_error == PM_ERR_FLAG__SUCCESS);
     #endif

    #ifdef RF_HAS_BC0_NV_TABLE  
    rfnv_cdma1x_bc0_tbl.rfnv_1x_pa_r_timer = 16; //Each timer count is 1.25ms hence for 20ms delay: 20/1.25 = 16
                                                
    if(rft_num_pa_states_set(RF_BC0_BAND, RFT_DUAL_STATE_PA) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("The value for Number of States for PA Selection control for BC0 Band is not written successfully");
    }

    #ifdef RF_SUPPORTS_NON_BYPASS_PA_FOR_BC0
    if(rft_da_select_set(RF_BC0_BAND, RFT_DA_SELECT_DA_1_ALWAYS) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("The value for DA Selection control for BC0 Band is not written successfully");
    }
    if(rft_pa_select(RF_BC0_BAND, RFT_NON_BYPASS_PA) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("PA configration is not set properly for BC0 Band");
    }
    #else    
    if(rft_da_select_set(RF_BC0_BAND, RFT_DA_SELECT_DA_2_PA_R0) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("The value for DA Selection control for BC0 Band is not written successfully");
    }
    if(rft_pa_select(RF_BC0_BAND, RFT_BYPASS_PA) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("PA configration is not set properly for BC0 Band");
    }
    #endif
    #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
    if(rft_configure_tx_pdm_tables(RF_BC0_BAND)!= RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("PDM Tables have not been configured properly for BC0");
    }
    #endif

    #endif

    #ifdef RF_HAS_BC1_NV_TABLE
    rfnv_cdma1x_bc1_tbl.rfnv_1x_pa_r_timer = 16; //Each timer count is 1.25ms hence for 20ms delay: 20/1.25 = 16

    if(rft_num_pa_states_set(RF_BC1_BAND, RFT_DUAL_STATE_PA) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("The value for Number of States for PA Selection control for BC1 Band is not written successfully");
    }

    #ifdef RF_SUPPORTS_NON_BYPASS_PA_FOR_BC1
   
    if(rft_da_select_set(RF_BC1_BAND, RFT_DA_SELECT_DA_1_ALWAYS) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("The value for DA Selection control for BC1 Band is not written successfully");
    }
    if(rft_pa_select(RF_BC1_BAND, RFT_NON_BYPASS_PA) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("PA configration is not set properly for BC1 Band");
    }
    #else
   
    if(rft_da_select_set(RF_BC1_BAND, RFT_DA_SELECT_DA_2_PA_R0) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("The value for DA Selection control for BC1 Band is not written successfully");
    }
    if(rft_pa_select(RF_BC1_BAND, RFT_BYPASS_PA) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("PA configration is not set properly for BC1 Band");
    }
    #endif
    #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
    if(rft_configure_tx_pdm_tables(RF_BC1_BAND)!= RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("PDM Tables have not been configured properly for BC1");
    }
    #endif

    #endif

    #ifdef RF_HAS_BC5_NV_TABLE

    rfnv_cdma1x_bc5_tbl.rfnv_1x_pa_r_timer = 16; //Each timer count is 1.25ms hence for 20ms delay: 20/1.25 = 16

    /* Select the number of PA States for BC5 based on the HW ID */
    switch(rfhw->rf_hw_id)
    {
    case RF_HW_QSC11X0_TRI_BAND:
      if(rft_num_pa_states_set(RF_BC5_BAND, RFT_DUAL_STATE_PA) != RFT_WRITE_SUCCESS)
      {
        RF_FATAL_ERROR("The value for Number of States for PA Selection control for BC5 Band is not written successfully");
      }
      break;

    case RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA:
      if(rft_num_pa_states_set(RF_BC5_BAND, RFT_TRI_STATE_PA) != RFT_WRITE_SUCCESS)
      {
        RF_FATAL_ERROR("The value for Number of States for PA Selection control for BC5 Band is not written successfully");
      }
      break;

    default:
      RF_FATAL_ERROR("Invalid QSC1100 HW Configuration");
      break;
    }

    #ifdef RF_SUPPORTS_NON_BYPASS_PA_FOR_BC5
   
    if(rft_da_select_set(RF_BC5_BAND, RFT_DA_SELECT_DA_2_ALWAYS) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("The value for DA Selection control for BC5 Band is not written successfully");
    }
    if(rft_pa_select(RF_BC5_BAND, RFT_NON_BYPASS_PA) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("PA configration is not set properly for BC5 Band");
    }
    #else
    
    if(rft_da_select_set(RF_BC5_BAND, RFT_DA_SELECT_DA_2_PA_R0) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("The value for DA Selection control for BC5 Band is not written successfully");
    }
    if(rft_pa_select(RF_BC5_BAND, RFT_BYPASS_PA) != RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("PA configration is not set properly for BC5 Band");
    }
    #endif
    #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
    if(rft_configure_tx_pdm_tables(RF_BC5_BAND)!= RFT_WRITE_SUCCESS)
    {
      RF_FATAL_ERROR("PDM Tables have not been configured properly for BC5");
    }
    #endif

    #endif

    #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
    #endif


#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
    rfr_set_lo_cal_chan( RF_BC0_BAND , 358 );
    rfr_set_lo_cal_chan( RF_BC1_BAND , 563 );
    rfr_set_lo_cal_chan( RF_BC5_BAND , 210 );
    rfr_set_lo_cal_chan( RF_BC6_BAND , 563 );
    rfr_set_lo_cal_chan( RF_BC10_BAND , 358 );
    rfr_set_lo_cal_chan( RF_BC11_BAND , 210 );
#endif //RF_HAS_SUPPORT_FOR_LO_PWC_CAL
    
}
/*===========================================================================

FUNCTION    QSC11X0_SET_MODE_PARK

DESCRIPTION
  Sets the specified path to park.
  
PARAMETERS
  RF PATH to be parked.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_mode_park(rf_path_enum_type path)
{
  MSG_1( MSG_SSID_RF, MSG_LEGACY_LOW, "rfc_qsc11x0_set_mode_park %d", path );

  if(path == RF_PATH_1)
  {
    /* Update module state to reflect the current GPIO settings */
    rfc_qsc11x0.external_vco_enabled = FALSE;
    rfc_qsc11x0.ext_lo_band = QSC11X0_EXT_LO_BAND_PCS;
  }
}


#ifdef RF_HAS_SUB_BLOCK_SUPPORT

/*=========================================================================

FUNCTION RFC_QSC11X0_GET_BLOCK

DESCRIPTION
This function is called on every band or channel change
Maps the Sub block for Band/Channel Combination
Returns the Sub class Mask for given Band/Channel combination to be tuned

  
DEPENDENCIES
  None.

RETURN VALUE
  Returns the Block mask Valid for Current Channel as read from NV

SIDE EFFECTS
  None.

===========================================================================*/ 
uint32 rfc_qsc11x0_get_block(rf_card_band_type band, word rf_chan)
{
  
  switch(band)
  {
  case RF_BC0_BAND:

    if ( (1 <= rf_chan) && (rf_chan <= 799))
      {
      /*These channels correspond to band Sub-class 0 */
      return 0x1;
      }

    else if ( (991 <= rf_chan) && (rf_chan <= 1023))
      {
      /*These channels correspond to band Sub-class 0 */
      return 0x1;
      }

    else if ( (1024 <= rf_chan) && (rf_chan <= 1323))
      {
      /*These channels correspond to band Sub-class 3 */
      return 0x8;
      }

    else
      {        
        RF_FATAL_ERROR("Un-supported channel passed in BC0");
        return 0x0;
      }
    break;

  case RF_BC1_BAND:

    if ( rf_chan <= 299 )
      { 
      /*These channels correspond to block A */
      return 0x1;
      }

    else if ( (300 <= rf_chan) && (rf_chan <= 399))
      {
      /*These channels correspond to block D */
      return 0x8;
      }

    else if ( (400 <= rf_chan) && (rf_chan <= 699))
      {
      /*These channels correspond to block B */
      return 0x2;
      }

    else if ( (700 <= rf_chan) && (rf_chan <= 799))
      {
      /*These channels correspond to block E */
      return 0x10;
      }

    else if ( (800 <= rf_chan) && (rf_chan <= 899))
      {
      /*These channels correspond to block F */
      return 0x20;
      }

    else if ( (900 <= rf_chan) && (rf_chan <= 1199))
      {
      /*These channels correspond to block C */
      return 0x4;
      }

    else
      {        
        RF_FATAL_ERROR("Un-supported channel passed in BC1");
        return 0x0;
      }
    break;

  case RF_BC5_BAND:

    if ( (1 <= rf_chan) && (rf_chan <= 80))
      {
      /* Return Mask representing Sub-block-C */
      return 0x00000004;
      }

    else if ( (81 <= rf_chan) && (rf_chan <= 100))
      {
      /* Return Mask representing Sub-block-B */
      return 0x00000002;
      }

    else if ( (101 <= rf_chan) && (rf_chan <= 300))
      {
      /* Return Mask representing Sub-block-A */
      return 0x00000001;
      }

    else if ( (301 <= rf_chan) && (rf_chan <= 400))
      {
      /* Return Mask representing Sub-block-J */
      return 0x00000200;
      }

    else if ( (472 <= rf_chan) && (rf_chan <= 706))
      {
      /* Return Mask representing Sub-block-D */
      return 0x00000008;
      }

    else if ( (707 <= rf_chan) && (rf_chan <= 871))
      {
      /* Return Mask representing Sub-block-E */
      return 0x00000010;
      }

    else if ( (1039 <= rf_chan) && (rf_chan <= 1260))
      {
      /* Return Mask representing Sub-block-H */
      return 0x00000080;
      }

    else if ( (1261 <= rf_chan) && (rf_chan <= 1473))
      {
      /* Return Mask representing Sub-block-G */
      return 0x00000040;
      }

    else if ( (1536 <= rf_chan) && (rf_chan <= 1715))
      {
      /* Return Mask representing Sub-block-K */
      return 0x00000400;
      }

    else if ( (1792 <= rf_chan) && (rf_chan <= 2016))
      {
      /* Return Mask representing Sub-block-F */
      return 0x00000020;
      }
  
    else
      {        
        RF_FATAL_ERROR("Un-supported channel passed in BC5");
        return 0x0;
      }
    
  break;

  default:
    RF_FATAL_ERROR("Band Class not supported");
    return 0x0;
  }/*switch()*/

  return 0xFFFFFFFF;  
}

/*=========================================================================
FUNCTION RFC_QSC11X0_BLOCK_MASK_IS_VALID

DESCRIPTION
This function is called during RF Cal mode to check the validity of block Mask Passed 
Returns if the given Block Mask passed during RF Cal is Valid
If RF Cal's s Block Mask is such that it asks for 2 blocks which are supported on different 
RF Paths (Different duplexer and different Tx SAW) for given Band,this function will return FALSE, 
otherwise the functon returns TRUE
  
DEPENDENCIES
  None.

RETURN VALUE
 Returns TRUE if Block Mask is Valid otherwise returns FALSE

SIDE EFFECTS
  None.

===========================================================================*/ 
boolean rfc_qsc11x0_block_mask_is_valid(rf_card_band_type band,uint32 subclass_mask)
{
  switch(band)
  {
  case RF_BC5_BAND:
  case RF_BC11_BAND:
    return TRUE;
    break;

  case RF_BC0_BAND:
  case RF_BC1_BAND:
  case RF_BC6_BAND:
  case RF_BC10_BAND:
    return TRUE;
    break;

  default:
    RF_FATAL_ERROR("Band Not Supported");
    return FALSE;
    break;
  }
}
/*=========================================================================
FUNCTION RFC_QSC11x0_SET_BLOCK

DESCRIPTION
Configure the GPIOs for the current Band and Block combination 
  
DEPENDENCIES
  None.

RETURN VALUE
 

SIDE EFFECTS
  None.

===========================================================================*/ 
void rfc_qsc11x0_set_block(rfcom_device_enum_type device,rf_card_band_type band,uint32 block_mask)
{
 /*configure the GPIOs for the current Band and Block combination */
}

#endif

/*===========================================================================

FUNCTION    QSC11X0_SET_MODE

DESCRIPTION
  Sets the specified path to appropriate mode of operation.
  
PARAMETERS
  Path to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_mode(rf_path_enum_type path, rf_card_band_type band)
{
   MSG_2( MSG_SSID_RF, MSG_LEGACY_LOW, "rfc_qsc11x0_set_mode %d %d", path, band );

   switch (band) {
   case RF_BC0_BAND:
   case RF_BC10_BAND:
     rfc_qsc11x0_set_mode_cell(path,band);
     break;

   case RF_BC1_BAND:
   case RF_BC6_BAND:
   case RF_BC5_BAND:
   case RF_BC11_BAND:
     rfc_qsc11x0_set_mode_pcs(path,band);
     break;

   default:
     ERR_FATAL("Invalid RF band (%d) in rfc_qsc11x0_set_mode", band, 0, 0);
     break;
   }
}


/*===========================================================================

FUNCTION    QSC11X0_SET_MODE_CELL

DESCRIPTION
  Sets the specified path to CELL ( Lower band) mode of operation.
  
PARAMETERS
  Path to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_mode_cell(rf_path_enum_type path,rf_card_band_type band)
{ 
  pm_err_flag_type pm_level_error = PM_ERR_FLAG__SUCCESS;
  

  MSG_1( MSG_SSID_RF, MSG_LEGACY_LOW, "rfc_qsc11x0_set_mode_cell %d", path );

  if (path == RF_PATH_0)
  {
    MSM_W_MASKED(PA_ON_CTL, BAND_SEL, CAGC_RF_PA_ON_CELL_BAND_SEL_V)
    
    enc_set_tx_polarity(rf_debug_qsc11x0_tx_spectral_norm_bc0);

    #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
    #else
    rxf_set_rx_polarity(path, rf_debug_qsc11x0_rx_spectral_norm_cell);
    #endif

    #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
    if(rfc_qsc11x0_support_temp_based_device_config(band) == TRUE)
    {    
      rfr_set_initial_temp_based_device_config(path, band);
    }
    else
    {       
      pm_level_error |= pm_vreg_set_level(PM_VREG_S1_ID, RFC_RF_SMPS_S1_LEVEL);

      if(pm_level_error != PM_ERR_FLAG__SUCCESS)
      {
        MSG_ERROR("PMIC SMPS S1 Voltage is not set properly for Band: %d",band,0,0);
      }
    }
    #else
    pm_level_error |= pm_vreg_set_level(PM_VREG_S1_ID, RFC_RF_SMPS_S1_LEVEL);

    if(pm_level_error != PM_ERR_FLAG__SUCCESS)
    {
      MSG_ERROR("PMIC SMPS S1 Voltage is not set properly for Band: %d",band,0,0);
    }
    #endif
  }
  else
  {
    ERR_FATAL("Invalid RF path (%d) in rfc_qsc11x0_set_mode_cell", path, 0, 0);
  }
}/* rfc_qsc11x0_set_mode_cell */


/*===========================================================================

FUNCTION    QSC11X0_SET_MODE_PCS

DESCRIPTION
  Sets the specified path to PCS (Upper band) mode of operation.
  
PARAMETERS
  Path to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_mode_pcs(rf_path_enum_type path,rf_card_band_type band)
{  
    pm_err_flag_type pm_level_error = PM_ERR_FLAG__SUCCESS;

    MSG_1( MSG_SSID_RF, MSG_LEGACY_LOW, "rfc_qsc11x0_set_mode_pcs %d", path );

    
  if (path == RF_PATH_0)
  {
    if((band == RF_BC6_BAND) || (band == RF_BC1_BAND))
    {    
      MSM_W_MASKED(PA_ON_CTL, BAND_SEL, CAGC_RF_PA_ON_PCS_BAND_SEL_V); 
      enc_set_tx_polarity(rf_debug_qsc11x0_tx_spectral_norm_bc1);
    }
    else 
    {
      MSM_W_MASKED(PA_ON_CTL, BAND_SEL, 0x2);
      enc_set_tx_polarity(rf_debug_qsc11x0_tx_spectral_norm_bc5);
    }
    //enc_set_tx_polarity( ENC_TX_SPECTRAL_NORM );

    #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
    #else
    rxf_set_rx_polarity(path, rf_debug_qsc11x0_rx_spectral_norm_pcs);
    #endif

    #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
    if(rfc_qsc11x0_support_temp_based_device_config(band) == TRUE)
    {    
      rfr_set_initial_temp_based_device_config(path, band);
    }
    else
    {       
      pm_level_error |= pm_vreg_set_level(PM_VREG_S1_ID, RFC_RF_SMPS_S1_LEVEL);

      if(pm_level_error != PM_ERR_FLAG__SUCCESS)
      {
        MSG_ERROR("PMIC SMPS S1 Voltage is not set properly for Band: %d",band,0,0);
      }
    }
    #else
    pm_level_error |= pm_vreg_set_level(PM_VREG_S1_ID, RFC_RF_SMPS_S1_LEVEL);

    if(pm_level_error != PM_ERR_FLAG__SUCCESS)
    {
      MSG_ERROR("PMIC SMPS S1 Voltage is not set properly for Band: %d",band,0,0);
    }
    #endif

    /* Configure PRx PCS for TRI-MODE HW */
    /*if ( rf_card.rf_hw_id == RF_HW_QSC11X0_TRI_BAND )
    {
       gpio_out(AWS0_PCS0_N, GPIO_LOW_VALUE);
       gpio_out(CELL_HB_N,   GPIO_LOW_VALUE);
    }*/
  }
  //else if (path == RF_PATH_1)
 // {
 //   rfc_qsc11x0.ext_lo_band = QSC11X0_EXT_LO_BAND_PCS;

    /* Select secondary antenna for 1x */
  //  gpio_out(GPS_SWITCHING, GPIO_LOW_VALUE);

    /* Configure DRx PCS for TRI-MODE HW */
  //  if ( rf_card.rf_hw_id == RF_HW_QSC11X0_TRI_BAND )
   // {
    //   gpio_out(AWS1_PCS1_N, GPIO_LOW_VALUE);
   // }
  //}
  else
  {
    ERR_FATAL("Invalid RF path (%d) in rfc_qsc11x0_set_mode_pcs", path, 0, 0);
  }
} /* rfc_qsc11x0_set_mode_pcs */


#ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
/*===========================================================================

FUNCTION  RFC_QSC11X0_SET_SPECTRAL_INV

DESCRIPTION
  Sets the correct spectral inversion in MSM dependent on band
  and Device being used
  
PARAMETERS
  Device currently used and Band
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_spectral_inv(rfcom_device_enum_type device,rf_card_band_type band)
{
  rf_path_enum_type path = rfcom_to_path[ device ];

  switch (band) 
  {
  case RF_BC0_BAND:
  case RF_BC10_BAND:
    switch (device)
    {
    case RFCOM_TRANSCEIVER_0:
      if ( srchrdiv_get_ftm_second_chain_test() )
      {
        /* Second chain test call uses Antenna 1 and 
           spectral inversion is inverted for second chain for BC0. */
        rxf_set_rx_polarity(path,ENC_RX_SPECTRAL_INV);
      }
      else
      {
        rxf_set_rx_polarity(path,ENC_RX_SPECTRAL_NORM);
      }
      break;

    case RFCOM_RECEIVER_DIV:
    case RFCOM_RECEIVER_1:
      rxf_set_rx_polarity(path,ENC_RX_SPECTRAL_INV);
      break;

    case RFCOM_RECEIVER_DUAL:
      MSG_ERROR( "Device %d not supported!!!", device, 0, 0 );
      break;

    default:
      RF_FATAL_ERROR("Unknown Device!!!");
      break;
    }
    break;

  case RF_BC1_BAND:  
  case RF_BC6_BAND:   
  case RF_BC5_BAND:
  case RF_BC11_BAND:
    switch (device)
    {
    case RFCOM_TRANSCEIVER_0:
      rxf_set_rx_polarity(path,ENC_RX_SPECTRAL_NORM);      
      break;

    case RFCOM_RECEIVER_DIV:
      rxf_set_rx_polarity(path,ENC_RX_SPECTRAL_NORM);
      break;

    case RFCOM_RECEIVER_1:
      rxf_set_rx_polarity(path,ENC_RX_SPECTRAL_INV);
      break;

    case RFCOM_RECEIVER_DUAL:
      MSG_ERROR( "Device %d not supported!!!", device, 0, 0 );
      break;

    default:
      RF_FATAL_ERROR("Unknown Device!!!");
      break;
    }
    break;

  default:
    RF_FATAL_ERROR("Band not supported");
    break;
  }/*switch (band) */
}
#endif /*RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE*/

/*===========================================================================

FUNCTION    QSC11X0_SELECT_PRIMARY_VCO

DESCRIPTION
  Selects the primary vco.
  
PARAMETERS
  Path for the vco.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_select_primary_vco(void)
{
  /* No GPIO configuration needed */
} /* rfc_qsc11x0_select_primary_vco */


/*===========================================================================

FUNCTION    QSC11X0_SELECT_SECONDARY_VCO

DESCRIPTION
  Selects the secondary chain vco.
  
PARAMETERS
  Path for the vco.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_select_secondary_vco(void)
{
  /* No GPIO configuration needed */
} /* rfc_qsc11x0_select_secondary_vco */


/*===========================================================================

FUNCTION    QSC11X0_ENABLE_SECONDARY_CHAIN_VCO

DESCRIPTION
  Enables the secondary chain's vco.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_enable_secondary_chain_vco(void)
{
 return;  
} /* rfc_qsc11x0_enable_secondary_chain_vco */
  

/*===========================================================================

FUNCTION    QSC11X0_DISABLE_SECONDARY_CHAIN_VCO

DESCRIPTION
  Disables the secondary chain's vco.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_disable_secondary_chain_vco(void)
{
 return;
} /* rfc_qsc11x0_disable_secondary_chain_vco */


/*===========================================================================

FUNCTION    QSC11X0_GET_HDET_ADC_CHANNEL

DESCRIPTION
  Returns the ADC logical channel for the HDET based upon band class
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
adc_logical_channel_type rfc_qsc11x0_get_hdet_adc_channel( rf_card_band_type band) /*lint -esym(715, band) unused */
{
    return ADC_HDET_CELL;
} /* rfc_qsc11x0_get_hdet_adc_channel */

#if defined(FEATURE_PMIC_MANAGED_LDO) && !defined(RF_HAS_RFA_POWER_COLLAPSE)
/*=========================================================================

FUNCTION qsc11x0_check_rx_chain_presets                      INTERNAL FUNCTION

DESCRIPTION
  Checks to see if RF device needs to be reset and re-intialized with
  internal default settings.  Device/driver re-initialization is expected
  after device core (i.e. RX0 and RX1) is powered-up.

  Check is against the PMIC VREG status which needs to be snapshot prior
  to any VREG enabling.

  This check is to be used after VREGs have been turned on.

RETURN VALUE
  None.

PARAMETERS
  Bit mask of the status of PMIC VREGs

SIDE EFFECTS
  RF device driver initialized with device default values.

===========================================================================*/
void qsc11x0_check_rx_chain_presets( pm_vreg_mask_type vreg_status_before_on )
{
    /* 
    * Check against the state of PMIC VREG.  If the state if off, then
    * expected that preset would be required since this function is called
    * after turning the VREGs on. 
    */
    if ((vreg_status_before_on & PM_VREG_RFRX1_M) == 0 )
    {
       /* Rx1 vreg was previously off, requires preset */
       qsc11x0_rx_prx_init();

#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif
    }

    if ((vreg_status_before_on & PM_VREG_RFRX2_M) == 0 )
    {
       /* Rx2 vreg was previously off, requires preset */
       qsc11x0_rx_drx_init();

#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif
    }
}
#endif /* FEATURE_PMIC_MANAGED_LDO && !RF_HAS_RFA_POWER_COLLAPSE */

#if defined(FEATURE_PMIC_MANAGED_LDO) && !defined(RF_HAS_RFA_POWER_COLLAPSE)
/*=========================================================================

FUNCTION QSC11X0_MANAGE_LDO                               INTERNAL FUNCTION

DESCRIPTION
  Manages the RFRX1, RFRX2, RFTX and PA voltage regulators for the RF Card
  to conform to the RF Chain State.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rfc_qsc11x0_manage_ldo(rfc_vregs_id_type vregs,     \
                            pm_switch_cmd_type cmd_arg,  \
                            rf_path_enum_type path )
{
    pm_switch_cmd_type cmd = cmd_arg;         /* internal cmd which can be bypass when debug enabled */
    pm_vreg_mask_type vreg_status_before_on;  /* Used to determine status of VREG prior to on vote */
 
#ifndef RF_HAS_PMIC_VREG_POWERUP_SEQUENCE
    uint16 vreg_wait = RF_VREG_RX_PWR_WAIT;
#endif 

#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif

    /* Get current on/off status of PMIC VREGs */
    vreg_status_before_on = pm_vreg_status_get();

    switch ( vregs )
    {
    case RFC_RX_VREGS:
#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif /* RF_MANAGED_LDO_DEBUG */
        switch ( path ) 
        {
        case RF_PATH_0:
            /* Manage power for PRx - RFVCO and RFRX1 */
            pm_vote_vreg_switch( cmd, PM_VREG_RF_VCO_ID, PM_VOTE_VREG_RFRX1_APP__RF_CHAIN0 );
            pm_vote_vreg_switch( cmd, PM_VREG_RFRX1_ID,  PM_VOTE_VREG_RFRX1_APP__RF_CHAIN0 );

            /* Intelliceiver is connected to VREG2, thus needs to be enable */
            pm_vote_vreg_switch( cmd, PM_VREG_RFRX2_ID, PM_VOTE_VREG_RFRX1_APP__RF_CHAIN0 );

            /* Need to determine if RF cores needs pre-setting */
            if ( cmd == PM_ON_CMD )
            {
               qsc11x0_check_rx_chain_presets( vreg_status_before_on );
            }

#ifdef RF_HAS_TX_WITH_RX_VREG_ENABLE
            /* Currently FTM requires Rx/Tx PLLs at same time */
            pm_vote_vreg_switch( cmd, PM_VREG_RFTX_ID, PM_VOTE_VREG_RFTX_APP__RF );

            if ((vreg_status_before_on & PM_VREG_RFTX_M) == 0 )
            {
               /* Tx vreg was previously off, requires preset */
               qsc11x0_tx_preset_init();

#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif
            }
#endif /* BYPASS_TX_VREG */

#ifndef RF_HAS_PMIC_VREG_POWERUP_SEQUENCE
            /* wait for both SYNTH and RFRX1 LDOs warm up*/ 
            vreg_wait = RF_VREG_SYNTH_PWR_WAIT;
#endif
            break;

        case RF_PATH_1:
            /* Manage power for DRx (requires all PRx VREGS + RFRX2 */
            pm_vote_vreg_switch( cmd, PM_VREG_RF_VCO_ID, PM_VOTE_VREG_RFRX1_APP__RF_CHAIN1 );
            pm_vote_vreg_switch( cmd, PM_VREG_RFRX1_ID,  PM_VOTE_VREG_RFRX1_APP__RF_CHAIN1);
            pm_vote_vreg_switch( cmd, PM_VREG_RFRX2_ID,  PM_VOTE_VREG_RFRX2_APP__RF );

            /* Need to determine if RF cores needs pre-setting */
            if ( cmd == PM_ON_CMD )
            {
               qsc11x0_check_rx_chain_presets( vreg_status_before_on );
            }
            break;

        default:
            ERR_FATAL("Invalid RF path in Rx manage_vregs: %d", path, 0, 0);
            return; /*lint !e527 */
        }

#if !defined (RF_HAS_WU_OPTIMIZED) && \
    !defined (RF_HAS_PMIC_VREG_POWERUP_SEQUENCE )
        /* Allow wait to occur if RFRX VREG(s) are powering up */
        if(cmd == PM_ON_CMD)
        {
          clk_busy_wait( vreg_wait );/*lint !e834 Operator '-' followed by operator '+' is confusing*/
        }
#endif
        break;
    
    case RFC_RX_DIV_VREGS:
#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif /* RF_MANAGED_LDO_DEBUG */

        /* Manage power for DRx diversity (requires all PRx VREGS + RFRX2 */
        pm_vote_vreg_switch( cmd, PM_VREG_RF_VCO_ID, PM_VOTE_VREG_RFRX1_APP__RF_CHAIN1 );
        pm_vote_vreg_switch( cmd, PM_VREG_RFRX1_ID,  PM_VOTE_VREG_RFRX1_APP__RF_CHAIN1);
        pm_vote_vreg_switch( cmd, PM_VREG_RFRX2_ID, PM_VOTE_VREG_RFRX2_APP__RF );

        /* Need to determine if RF cores needs pre-setting */
        if ( cmd == PM_ON_CMD )
        {
           qsc11x0_check_rx_chain_presets( vreg_status_before_on );
        }

        break;
    
    case RFC_TX_VREGS:
#ifndef RF_HAS_TX_WITH_RX_VREG_ENABLE

#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif /* RF_MANAGED_LDO_DEBUG */

       pm_vote_vreg_switch( cmd, PM_VREG_RFTX_ID, PM_VOTE_VREG_RFTX_APP__RF );

       if ((vreg_status_before_on & PM_VREG_RFTX_M) == 0 )
       {
          /* Tx vreg was previously off, requires preset */
          qsc11x0_tx_preset_init();

#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif
       }
#endif /* RF_HAS_TX_WITH_RX_VREG_ENABLE */

        break;

    case RFC_PA_THERM_VREGS:
        /* no VREG_MSMA management at this time */

        break;

    case RFC_GPS_VREGS:
#ifdef RF_MANAGED_LDO_DEBUG
#error code not present
#endif /* RF_MANAGED_LDO_DEBUG */
         
        /* Manage power similar as DRx (requires all PRx VREGS + RFRX2 */
        pm_vote_vreg_switch( cmd, PM_VREG_RF_VCO_ID, PM_VOTE_VREG_RFRX1_APP__GPS );
        pm_vote_vreg_switch( cmd, PM_VREG_RFRX1_ID,  PM_VOTE_VREG_RFRX1_APP__GPS);
        pm_vote_vreg_switch( cmd, PM_VREG_RFRX2_ID,  PM_VOTE_VREG_RFRX2_APP__GPS );

        /* Need to determine if RF cores needs pre-setting */
        if ( cmd == PM_ON_CMD )
        {
           qsc11x0_check_rx_chain_presets( vreg_status_before_on );
        }

#ifndef RF_HAS_PMIC_VREG_POWERUP_SEQUENCE
        /* Allow wait to occur if RFRX VREG(s) are powering up */
        if(cmd == PM_ON_CMD)
        {
            clk_busy_wait( vreg_wait );/*lint !e834  Operator '-' followed by operator '+' is confusing*/
        }
#endif  /* RF_HAS_PMIC_VREG_POWERUP_SEQUENCE */

        break;

    default:
        MSG_FATAL("Unknown Rx manage_vregs request %d path=%d", vregs, path, 0);
        break;
    }

} /* rfc_qsc11x0_manage_vregs */
#endif /* FEATURE_PMIC_MANAGED_LDO && !RF_HAS_RFA_POWER_COLLAPSE */

#ifndef RF_HAS_TCXO_SHUTDOWN_DISABLED
/*=========================================================================

FUNCTION QSC11X0_MANAGE_RFA_POWER_COLLAPSE                INTERNAL FUNCTION

DESCRIPTION
  RF function to support full power collapse of RF / Analog portion
  of SC2x.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rfc_qsc11x0_manage_rfa_power_collapse(rfc_vregs_id_type vregs, \
                                           pm_switch_cmd_type cmd,  \
                                           rf_path_enum_type path )
{
   /*
   * RF/Analog VREG managment is done by clkregime during TCXO shutdown 
   * and wakeup.  This ensures that QSC RF/Analog circuits can be shutdown completely
   * and restarted together.
   *
   * During power-up the RF cores must be re-initialized and device remains on
   * for all transactions until the next TCXO shutdown is voted for as determined
   * by clkregime module.
   */
   if ( cmd == PM_ON_CMD ) 
   {
      switch ( vregs )
      {
         case RFC_RX_VREGS:         
         //case RFC_TX_VREGS:
         //case RFC_PA_THERM_VREGS:         
            /* 
            *  Determine if this is a power-on condition.
            *  Need to preset QSC to known conditions
            */
                      
              if ( clkregime_power_on_flag_is_set() )
              {
                if(rf_qsc11x0_sleep_enable_flag == TRUE)
                { 
                  rfr_enable_sleep_on_off_sequence(path,FALSE);
                }

                  qsc11x0_rx_prx_init();               
                  qsc11x0_tx_preset_init();

                 /* Reset flag for next power-on condition */
                   clkregime_power_on_flag_clear();

              }           
            break;

        default:
           MSG_ERROR("Unknown Rx manage_vregs request %d path=%d", vregs, path, 0);
           break;
      }
   } 

} /* rfc_qsc11x0_manage_rfa_power_collapse */
#endif /* !RF_HAS_TCXO_SHUTDOWN_DISABLED */

/*=========================================================================

FUNCTION QSC11X0_MANAGE_VREGS

DESCRIPTION
  Manages the RFRX1, RFRX2, RFTX and PA voltage regulators for the RF Card
  to conform to the RF Chain State.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rfc_qsc11x0_manage_vregs(rfc_vregs_id_type vregs,  \
                            pm_switch_cmd_type cmd,    \
                            rf_path_enum_type path )
{
if(path == RF_PATH_0)
{
#ifndef RF_HAS_TCXO_SHUTDOWN_DISABLED
   rfc_qsc11x0_manage_rfa_power_collapse(vregs, cmd, path );
#endif
}
/*#ifdef FEATURE_PMIC_MANAGED_LDO
   rfc_qsc11x0_manage_ldo(vregs, cmd, path );   
#endif*/ /* FEATURE_PMIC_MANAGED_LDO */
//#endif /* RF_HAS_RFA_POWER_COLLAPSE */
}

/*=========================================================================

FUNCTION QSC11X0_USES_EXTERNAL_VCO

DESCRIPTION
  Returns true or false indicating whether the receiver should use an
  external VCO or not.

RETURN VALUE
  True for SHDR, otherwise false.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfc_qsc11x0_uses_external_vco( rf_path_enum_type path,
                                       rfc_vco_source_type vco_source )
{
  return FALSE;
} /* rfc_qsc11x0_uses_external_vco */

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
/*=========================================================================

FUNCTION RFC_QSC11X0_GET_HDET_COMP_VALUE

DESCRIPTION
    This function calculates and returns the compensation value for HDET
  depending on the HDET thermitor temperature and Radio Configuration.

RETURN VALUE
  HDET Compensation in 100 of a dBm uints.

SIDE EFFECTS
  None.

===========================================================================*/
int16 rfc_qsc11x0_get_hdet_comp_value(rfc_hdet_comp_cfg_type *hdet_comp_cfg)
{
  static int16 hdet_therm      = 0;
         int16 hdet_comp_therm = 0;
         int16 hdet_comp_rc    = 0;  

  hdet_therm = hdet_therm * 1; /*Redundant code added to avoid the compiler warnings*/

  /* Make sure the pointer passed is valid. */
  if (hdet_comp_cfg != NULL) 
  {
    /* Is HDET temperature compensation on? */
    if ((hdet_comp_cfg->cmd == RF_HDET_COMP_ENABLE) || \
        (hdet_comp_cfg->cmd == RF_HDET_COMP_TEMP_ENA)) 
    {
      /* Is it time to update/read the HDET thermistor. */
      /*if (hdet_comp_cfg->update_temp == TRUE) 
      {
          hdet_therm = adc_read(ADC_HDET_THERM);          
      }*/

      #if defined(FEATURE_FTM_HWTC)
#error code not present
      #endif /* FEATURE_FTM_HWTC */

      /* Get the HDET compensation value versus HDET temperature and current 
         band class. 100th of a dBm */
      #ifdef DISABLE_GET_HDET_THERM_COMP_VALUE_FUNCTION 
      /*hdet_comp_therm = rfc_qsc11x0_get_hdet_therm_comp_value(hdet_therm, \
                                                        hdet_comp_cfg->band); */
      #endif /* DISABLE_GET_HDET_THERM_COMP_VALUE_FUNCTION */
      hdet_comp_therm = 0;
    }

    /* Is HDET compensation versus Radio Configuration on? */
    if ((hdet_comp_cfg->cmd == RF_HDET_COMP_ENABLE) || \
        (hdet_comp_cfg->cmd == RF_HDET_COMP_RC_ENA))
    {
      /* Get the HDET compensation value versus current radion Configuration
         band class. 100th of a dBm */
      hdet_comp_rc = rfc_qsc11x0_get_hdet_rc_comp_value(                                 \
                                            (cai_radio_config_type)hdet_comp_cfg->rc,    \
                                            (enc_sch_rate_type)hdet_comp_cfg->r_sch_rate,\
                                            (enc_rate_type)hdet_comp_cfg->r_fch_rate,\
                                            hdet_comp_cfg->dcch_on,                      \
                                            hdet_comp_cfg->r_fch_on,                     \
                                            hdet_comp_cfg->r_sch_on);
    }

   // #if defined(RF_TEST_HDET_COMP)
    MSG_HIGH("HDET THERM=%d,hdet_comp_therm=%d,hdet_comp_rc=%d.",hdet_therm,hdet_comp_therm,hdet_comp_rc);
    //#endif /* RF_TEST_HDET_COMP */

  } /* hdet_comp_cfg != NULL */
  else
  {
    RF_FATAL_ERROR("HDET COMP ERROR:NULL POINTER!");
  }
  
  return (hdet_comp_therm + hdet_comp_rc) ;


} /* rfc_qsc11x0_get_hdet_comp_value() */

#ifdef RF_HAS_HDR
#error code not present
#endif
/*=========================================================================

FUNCTION rfc_qsc11x0_get_hdet_rc_comp_value

DESCRIPTION
  This function calculates and returns the compensation value for HDET
  depending on the Radio Configuration.  

RETURN VALUE
  HDET Compensation in 100 of a dBm uints.

SIDE EFFECTS
  None.

===========================================================================*/
static int16 rfc_qsc11x0_get_hdet_rc_comp_value(                           \
                                         cai_radio_config_type rc,         \
                                         enc_sch_rate_type     r_sch_rate, \
                                         enc_rate_type         r_fch_rate, \
                                         boolean               dcch_on,    \
                                         boolean               r_fch_on,   \
                                         boolean               r_sch_on)
{
  int16 hdet_comp  = 0;

  /* Check the current radio configuration. */
  if (rc >= CAI_RC_3) 
  { /* Is the reverse link fundamental channel on? */
    if (r_fch_on) 
    { /* Is the reverse link suplemental channel on? */
      if (r_sch_on) 
      { 
        /* R-FCH and R_SCH is ON. */
        if (r_sch_rate < ENC_NUM_SCH_RATES) 
        {
          if (dcch_on) 
          { 
            /* Get HDET compensation for R-FCH, R_SCH and DCCCH is ON. */
            hdet_comp = rfc_rc3_hdet_comp_values_table.r_sch_and_dccch[r_sch_rate];
          }
          else /* dccch_on == FALSE. */
          {
            /* Get HDET compensation for R-FCH and R_SCH is ON and DCCCH is OFF. */
            hdet_comp = rfc_rc3_hdet_comp_values_table.r_sch_and_r_fch[r_sch_rate];
          }  /* dccch_on */
        } 
        else
        {
          RF_FATAL_ERROR("R_SCH rate is out of range.");
        }
      }           
      else
      {
        if (dcch_on == FALSE) 
        {
          /* Get HDET compensation for R-FCH is ON and DCCCH is OFF. */
            if(r_fch_rate <= ENC_8TH_RATE)
            {            
              hdet_comp = rfc_rc3_hdet_comp_values_table.r_fch_only[r_fch_rate];
            }
        } /* dccch_on */
      } /* r_sch_on */
    } 
    else /* r_fch_on == FALSE */
    { 
      if ((dcch_on == TRUE) && (r_sch_on == FALSE) )
      {
        /* Get HDET compensation for DCCCH is ON and both R-FCH and \
           R-SCH are OFF. */
        hdet_comp = rfc_rc3_hdet_comp_values_table.dcch_only;
      }
    } /* r_fch_on */

  } /* rc */    

  return hdet_comp;

} /* rfc_qsc11x0_get_hdet_rc_comp_value */

/*=========================================================================

FUNCTION rfc_qsc11x0_get_hdet_temp_comp_value

DESCRIPTION
  This function calculates and returns the compensation value for HDET
  depending on the HDET thermitor temperature.

RETURN VALUE
  HDET Compensation in 100 of a dBm uints.

SIDE EFFECTS
  None.  

===========================================================================*/
/* Function is declared and not used. Disabled function */
#ifdef DISABLE_GET_HDET_THERM_COMP_VALUE_FUNCTION
static int16 rfc_qsc11x0_get_hdet_therm_comp_value(int16             temp, \
                                                   rf_card_band_type band)
{
  int16 hdet_comp  = 0;
  uint8 array_size = 0;
  int16 temp_val   = 0;
  uint8 index      = 0;
  uint8 band_index = 0;

  if ( (band == RF_BC0_BAND) ||
       (band == RF_BC10_BAND) ||
       (band == RF_BC1_BAND) ||       
       (band == RF_BC6_BAND) ||
       (band == RF_BC5_BAND) ||
       (band == RF_BC11_BAND)
     )
  {

    if ( (band == RF_BC0_BAND) || (band == RF_BC10_BAND))
    {
      band_index = 0;
    }
    else if ( (band == RF_BC1_BAND) || (band == RF_BC6_BAND) || (band == RF_BC5_BAND ) ||(band == RF_BC11_BAND ) )
    { 
      band_index = 1;
    }
    else
    {
      RF_FATAL_ERROR("Unexpected RF band!");
    }

    /* Calculate the size of the array */
    array_size  = sizeof(rfc_hdet_therm_comp_values_table);
    array_size /= sizeof(rfc_hdet_therm_comp_values_table[0]);

    if (temp > rfc_hdet_therm_comp_values_table[0].temp)      
    {
      /* if the temperature falls above the highest temperature entry (index = 0) 
         then use the highest temperature entry HDET compensation value. */
      hdet_comp = rfc_hdet_therm_comp_values_table[0].bc_comp[band_index];
    }
    else if (temp < rfc_hdet_therm_comp_values_table[array_size-1].temp) 
    {
       /* if the temperature falls bellow the lowest temperature entry (array_size-1) 
          then use the lowest temperature entry HDET compensation value. */
      hdet_comp = rfc_hdet_therm_comp_values_table[array_size-1].bc_comp[band_index];
    }
    else
    {
      /* we already check index = 0, increment the index by one. */
      index++;

      /* Ok the temperature is within the table, lets do interpolation. */
      while ((index < array_size) &&  \
              (rfc_hdet_therm_comp_values_table[index].temp > temp))
      { 
        /* Increment the index until the temperature read falls in between
           2 temperature entries. */
        index++;
      }

      /* Math:
         (((TEMP(read) - TEMP(low))/(TEMP(high) - TEMP(low))) * (COMP(high) - COMP(low))) +
         COMP(low) = HDET Compensation value (100 dBm). */ 

      /* TEMP(read) = HDET Therm (ADC reading))
         TEMP(low)  = The lower temperature between to temperature entries.
         TEMP(high) = The higher temperature between to temperature entries.
         COMP(low)  = HDET temperature compensation value for TEMP(low).
         COMP(high) = HDET temperature compensation value for TEMP(high). */

      if ((index >= 1) && (index < array_size)) 
      {
        temp_val   = rfc_hdet_therm_comp_values_table[index - 1].bc_comp[band_index]; 
        temp_val  -= rfc_hdet_therm_comp_values_table[index].bc_comp[band_index];
        temp_val  *= (temp - rfc_hdet_therm_comp_values_table[index].temp);
        temp_val  /= (int16)(rfc_hdet_therm_comp_values_table[index - 1].temp - \
                             rfc_hdet_therm_comp_values_table[index].temp);
        temp_val  += rfc_hdet_therm_comp_values_table[index].bc_comp[band_index];
        
        hdet_comp += temp_val;
      }
      else
      {
        RF_FATAL_ERROR("index value is out of bounds.");
      }
    }
  }
  else
  { /* Band not supported, return 0.*/
    hdet_comp = 0;
  }

  return hdet_comp;
}
#endif /* DISABLE_GET_HDET_THERM_COMP_VALUE_FUNCTION */
#ifdef RF_HAS_HDR
#error code not present
#endif
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */


#ifdef RF_HAS_RFCONFIG_RX_LNA
/*=========================================================================
FUNCTION  RFC_QSC6055_INIT_RX_LNA_RFCONFIG_DATA             LOCAL FUNCTION

DESCRIPTION
  Sets up configuration items for LNA on both primary and secondary Rx chains
  for all band classes listed below:
     - BC0
     - BC1
     - BC14
     - BC15
  This function is expected to be called from rfc_qsc11x0_init_card().
  
  Note for OEMs:
     If your phone supports a different set of band classes than the ones
     listed above, remove the unused ones and add support for the new ones
     by including code of the form shown below for each new band class
     (for e.g. for band class 5, <BCi> = BC5 and <bci> = bc5 in the code
      snippet below):

   //  ========== code snippet STARTS here ==========
   
   extern rfnv_digital_items_type rfnv_cdma1x_<bci>_tbl;

   // <BCi> PRx/DRx LNA config init
   prx_rfconfig_rx_lna_ptr = &rfnv_cdma1x_<bci>_tbl.rfconfig_rx_lna_info[RF_PATH_0];
   prx_rfnv_rfconfig_migration_tracker_ptr = rfnv_cdma1x_<bci>_get_rx_rfconfig_migration_tracker(RF_PATH_0);
   drx_rfconfig_rx_lna_ptr = &rfnv_cdma1x_<bci>_tbl.rfconfig_rx_lna_info[RF_PATH_1];
   drx_rfnv_rfconfig_migration_tracker_ptr = rfnv_cdma1x_<bci>_get_rx_rfconfig_migration_tracker(RF_PATH_1);

   rfconfig_qsc11x0_init_default_rx_lna_info(
      RF_<BCi>_BAND,
      prx_rfconfig_rx_lna_ptr,
      drx_rfconfig_rx_lna_ptr);
   // preserve legacy behavior for existing NV items that have been ported over to RFConfigDBase scheme;
   // note that this step is entirely optional, and can be removed once all ported NV items are removed
   // from QCN file.
   rfconfig_qsc11x0_migrate_from_rfnv(
      RF_<BCi>_BAND,
      &rfnv_cdma1x_<bci>_tbl,
      TRUE,              // use 1x NV items for initializing EVDO switch points;
                         // use FALSE if you don't want this behavior
      prx_rfconfig_rx_lna_ptr,
      prx_rfnv_rfconfig_migration_tracker_ptr,
      drx_rfconfig_rx_lna_ptr,
      drx_rfnv_rfconfig_migration_tracker_ptr);
   
   //  ========== code snippet ENDS here ==========
   
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rfc_qsc1100_init_rx_lna_rfconfig_data(void)
{
   extern rfnv_digital_items_type rfnv_cdma1x_bc0_tbl;
   extern rfnv_digital_items_type rfnv_cdma1x_bc1_tbl;
   extern rfnv_digital_items_type rfnv_cdma1x_bc14_tbl;

 #ifdef FEATURE_FTM_HWTC
#error code not present
 #endif /* FEATURE_FTM_HWTC */

   rfconfig_rx_lna_type                         *prx_rfconfig_rx_lna_ptr = NULL;
   const rfnv_rfconfig_migration_tracker_type   *prx_rfnv_rfconfig_migration_tracker_ptr = NULL;
   rfconfig_rx_lna_type                         *drx_rfconfig_rx_lna_ptr = NULL;
   const rfnv_rfconfig_migration_tracker_type   *drx_rfnv_rfconfig_migration_tracker_ptr = NULL;

   // BC0 PRx/DRx LNA config init
   prx_rfconfig_rx_lna_ptr = &rfnv_cdma1x_bc0_tbl.rfconfig_rx_lna_info[RF_PATH_0];
   prx_rfnv_rfconfig_migration_tracker_ptr = rfnv_cdma1x_bc0_get_rx_rfconfig_migration_tracker(RF_PATH_0);
   drx_rfconfig_rx_lna_ptr = &rfnv_cdma1x_bc0_tbl.rfconfig_rx_lna_info[RF_PATH_1];
   drx_rfnv_rfconfig_migration_tracker_ptr = rfnv_cdma1x_bc0_get_rx_rfconfig_migration_tracker(RF_PATH_1);

   rfconfig_qsc11x0_init_default_rx_lna_info(
      RF_BC0_BAND,
      prx_rfconfig_rx_lna_ptr,
      drx_rfconfig_rx_lna_ptr);
   // preserve legacy behavior for existing NV items that have been ported over to RFConfigDBase scheme;
   // note that this step is entirely optional, and can be removed once all ported NV items are removed
   // from QCN file.
   rfconfig_qsc11x0_migrate_from_rfnv(
      RF_BC0_BAND,
      &rfnv_cdma1x_bc0_tbl,
      FALSE,            // do NOT use 1x NV items for initializing EVDO switch points
      prx_rfconfig_rx_lna_ptr,
      prx_rfnv_rfconfig_migration_tracker_ptr,
      drx_rfconfig_rx_lna_ptr,
      drx_rfnv_rfconfig_migration_tracker_ptr);

   // BC1 PRx/DRx LNA config init
   prx_rfconfig_rx_lna_ptr = &rfnv_cdma1x_bc1_tbl.rfconfig_rx_lna_info[RF_PATH_0];
   prx_rfnv_rfconfig_migration_tracker_ptr = rfnv_cdma1x_bc1_get_rx_rfconfig_migration_tracker(RF_PATH_0);
   drx_rfconfig_rx_lna_ptr = &rfnv_cdma1x_bc1_tbl.rfconfig_rx_lna_info[RF_PATH_1];
   drx_rfnv_rfconfig_migration_tracker_ptr = rfnv_cdma1x_bc1_get_rx_rfconfig_migration_tracker(RF_PATH_1);
   rfconfig_qsc11x0_init_default_rx_lna_info(
      RF_BC1_BAND,
      prx_rfconfig_rx_lna_ptr,
      drx_rfconfig_rx_lna_ptr);
   // preserve legacy behavior for existing NV items that have been ported over to RFConfigDBase scheme;
   // note that this step is entirely optional, and can be removed once all ported NV items are removed
   // from QCN file.
   rfconfig_qsc11x0_migrate_from_rfnv(
      RF_BC1_BAND,
      &rfnv_cdma1x_bc1_tbl,
      FALSE,            // do NOT use 1x NV items for initializing EVDO switch points
      prx_rfconfig_rx_lna_ptr,
      prx_rfnv_rfconfig_migration_tracker_ptr,
      drx_rfconfig_rx_lna_ptr,
      drx_rfnv_rfconfig_migration_tracker_ptr);

#ifdef RF_HAS_SUPPORT_FOR_BAND_CLASS_15
   // BC15 PRx/DRx LNA config init
   prx_rfconfig_rx_lna_ptr = &rfnv_cdma1x_bc15_tbl.rfconfig_rx_lna_info[RF_PATH_0];
   prx_rfnv_rfconfig_migration_tracker_ptr = rfnv_cdma1x_bc15_get_rx_rfconfig_migration_tracker(RF_PATH_0);
   drx_rfconfig_rx_lna_ptr = &rfnv_cdma1x_bc15_tbl.rfconfig_rx_lna_info[RF_PATH_1];
   drx_rfnv_rfconfig_migration_tracker_ptr = rfnv_cdma1x_bc15_get_rx_rfconfig_migration_tracker(RF_PATH_1);
   rfconfig_qsc11x0_init_default_rx_lna_info(
      RF_BC15_BAND,
      prx_rfconfig_rx_lna_ptr,
      drx_rfconfig_rx_lna_ptr);
   // preserve legacy behavior for existing NV items that have been ported over to RFConfigDBase scheme;
   // note that this step is entirely optional, and can be removed once all ported NV items are removed
   // from QCN file.
   rfconfig_qsc11x0_migrate_from_rfnv(
      RF_BC15_BAND,
      &rfnv_cdma1x_bc15_tbl,
      FALSE,            // do NOT use 1x NV items for initializing EVDO switch points
      prx_rfconfig_rx_lna_ptr,
      prx_rfnv_rfconfig_migration_tracker_ptr,
      drx_rfconfig_rx_lna_ptr,
      drx_rfnv_rfconfig_migration_tracker_ptr);
#endif /* RF_HAS_SUPPORT_FOR_BAND_CLASS_15 */

   // Add support for any additional band classes as described in the header
   //  comments section
#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */
}
#endif /* RF_HAS_RFCONFIG_RX_LNA */

#ifdef RF_HAS_INTERNAL_THERM_SUPPORT
/*=========================================================================

FUNCTION QSC11X0_USES_INTERNAL_THERM

DESCRIPTION
  Query function for QSC11x0 chipset platforms to determine if target uses
  internal Chip Thermistor for Temperature readings


RETURN VALUE
  QSC11x0 is true for all bands on the Qualcomm reference platform

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean qsc11x0_uses_internal_therm( rf_card_band_type band )
{
   return TRUE;
} /* qsc11x0_uses_internal_therm */

/*=========================================================================

FUNCTION QSC11X0_READ_INTERNAL_THERM

DESCRIPTION
  Reads the internal Thermistor for QSC1100

RETURN VALUE
  Returns the 12 bit HKADC Read Value for Thermistor

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
word qsc11x0_read_internal_therm(rf_card_band_type band)
{
  word internal_therm_val = 0;

  internal_therm_val = (word)(adc_read(ADC_PTAT));

  MSG_HIGH("PTAT Therm read is %d",internal_therm_val,0,0); 

  return (internal_therm_val);
}
#endif /* RF_HAS_INTERNAL_THERM_SUPPORT*/

#ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
/*=========================================================================

FUNCTION RFC_QSC11X0_SUPPORT_TEMP_BASED_DEVICE_CONFIG

DESCRIPTION
  Returns TRUE or FALSE depending upon if Device Configration
  needs to be changed based on temperature or not for QSC1100

RETURN VALUE
  TRUE or FALSE

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfc_qsc11x0_support_temp_based_device_config(rf_card_band_type band)
{
  return TRUE;
}
#endif

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
/*=========================================================================

FUNCTION QSC11X0_USES_INTERNAL_HDET

DESCRIPTION
  Query function for QSC11x0 chipset platforms to determine if this
  target is wired to the internal HDET for a given band, or uses
  an external HDET for that band.

  In a mixed HDET configuration this function can return different values
  depending on the band.

RETURN VALUE
  QSC11x0 is true for all bands on the Qualcomm reference platform

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean qsc11x0_uses_internal_hdet( rf_card_band_type band )
{
   return TRUE;
} /* qsc11x0_uses_internal_hdet */
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */


#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
/*=========================================================================

FUNCTION RFC_QSC11x0_SET_PA_ACCESS_PROBE_CONFIG

DESCRIPTION
This function is called so that RF Driver can program different 
PA hysterisis timerand PA switchpoints for Access Probes different 
from traffic.
Returns the Backoff in PA switchpoints for Access Probe from traffic state
in 2/5th of dB

DEPENDENCIES
  None

RETURN VALUE
Returns the Backoff in PA switchpoints for Access Probe from traffic state
in 2/5th of dB
===========================================================================*/
int16 rfc_qsc11x0_set_pa_access_probe_config(rfcom_device_enum_type device,rf_card_band_type band)
{
  int2 pa_r1_rise_value; 

  //Two different variables for Bypass and non bypass PA..
  //In case backoffs are different for Bypass and non bypass config in future
  //Just to keep the things separate
  int2 rf_pa_access_probe_switch_pt;

  int16 backoff = 0;
  
  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  pa_r1_rise_value = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r1_rise);

  switch (band)
  {    
  case RF_BC0_BAND:  
  case RF_BC10_BAND:
    rf_pa_access_probe_switch_pt = 112;
    
    if (pa_r1_rise_value != RF_PA_RISE_MAX_VALUE)
    {
      backoff = pa_r1_rise_value -  rf_pa_access_probe_switch_pt;
    }
    break;
  
  case RF_BC1_BAND:  
  case RF_BC6_BAND:
  case RF_BC5_BAND:
  case RF_BC11_BAND:
    rf_pa_access_probe_switch_pt = 120;//Corresponds to -15 dbm in PA switchpoints Unit
    
    if (pa_r1_rise_value != RF_PA_RISE_MAX_VALUE)
    {
      backoff = pa_r1_rise_value -  rf_pa_access_probe_switch_pt;
    }
     break;

  default:
    backoff = 0;
    break;
  }/*switch(band)*/

  return (backoff);
}
#endif /* RF_HAS_PA_ACCESS_PROBE_CONFIG */


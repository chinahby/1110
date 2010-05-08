#ifndef RFNV_CDMA1X_H
#define RFNV_CDMA1X_H


/*===========================================================================

                         R F   1 X  N V   M a n a g e r

DESCRIPTION
  This header file contains definitions necessary inorder to interface with
  the RF 1X NV Manager.

Copyright (c) 2002, 2003, 2004 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2005, 2006, 2007 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2008             by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_cdma1x.h#10 $
$DateTime: 2008/12/04 16:23:46 $
$Author: vmahajan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/08   vm      Changed the data type to UINT16 for 8 bin Therm NV
11/24/08   vb      Changed the default values the IM2 DAC codes
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
07/03/08   vm      Added support for 12 bit HDET on QSC1100
06/13/08   vm      Updates for 16 bit PDET on QSC1100
06/11/08   sl      Added support for  NV_BCx_EXP_HDET_VS_AGC_V2_I, NV_BCx_HDET_OFF_V2_I,
                   and NV_BCx_HDET_SPN_V2_I under feature RF_HAS_HDET_UINT16.
04/29/08   sl      ULC: Changed features with ZIF_LIBRARY_SUPPORTS to RF_HAS_SUPPORT_FOR.
04/23/08   sl      Added NV_PROCESS_CAL_I, NV_DC_CAL_I, NV_SUPPLY_I,
                   NV_C0_BCx_LO_CAL_I, NV_C0_BCx_IM2_CAL_I support.
04/09/08   sl      Changed RF_PROCESS_CAL_DEFAULT_V defaults from zeroes to 128
                   For ER/EC items.
03/21/08   sl      Added NV_PROCESS_CAL_I support under rfnv_common_items_type.
11/28/07    xw     Added rfnv_validate_1x_rf_card() prototype. 
10/27/07   ans     Changed FEATURE_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST name to 
                   RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST.
                   Changed FEATURE_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST name to 
                   RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST.
10/26/07   ans     Added FEATURE_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST for backoffs
                   for R2 and R3 PA states..
10/20/07   rmd     Provided access to NV_RF_PM_CONFIG_I to the 1X side.
09/04/07   aak     Fixed compilation error
08/10/07   vm      Program the default PA Offsets to 0 for Sc2x
07/30/07   vm      Change the defaults for Thermistor bining
07/26/07   vm      Added the support for Tx Power Compensation NV based on
                   temperature and Frequency
07/22/07   dyc     Updated nv struct to support bc specific pa_r_map
07/13/07   vm      Changed the Rx PLL Lock time to 600us for Sc2X
06/25/07   adk     Set the value of RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ
                   via feature RF_HAS_PDM_LIN_TBL_SIZ_48
06/20/07   Vish    Added support for RFCONFIG Rx LNA data structures for
                   primary and secondary chains in digital NV items storage
                   on a per band class basis.
06/14/07   avi     Added support for the DRx antenna switch selection
06/05/07   vm      Change PDM vs Power table size to 64 for Beta Scaling
                   on Sc2X
05/09/07   vm      Fixed the compilation error 
05/09/07   vm      Changed the defaults for PDM Table for SC2X
05/02/07   adk     Fixed compiler errors.
04/30/07   vm      Added support for New Tx Cal for RF chips with 
                   Tx Compensation registers in RFT
04/23/07   adk     Renamed arrays for consistency when Beta Scaling is enabled.
04/07/07   adk     Updated default linearizer and PDM tables for Beta Scaling.
04/06/07   bmg     Fixed HDR PA hysteresis timer.
03/28/07   adk     Updated interface to NV for Beta Scaling.
03/23/07   bmg     Added PA timer hysteresis band config items.
03/08/07   adk     Added support for FW TX AGC with Beta Scaling.
03/01/07   sar     Added prototype for rfnv_set_card_defaults();
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
10/17/06   ycl     Modifications for initial MSM7600 build.
01/16/06   rmd     Within rfnv_1x_band_class_type, the BC14 entry was using 
                   the wrong feature.
12/15/05   sar     Fixed initializers for TX_LIN_MASTERx values to remove lint errors.
11/15/05   rmd     Added support for band class 11.
09/28/05   jfb     Fixed compile error for non power mode lib targets
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
09/07/05   bmg     Added IntelliCeiver support for rfnv_grp_delay_adj
07/25/05   bmg     Updated latest defaults for BC0_HDR_[G0]_IM_RISE/FALL
07/25/05   bmg     Updated latest defaults for BC0_HDR_G0_IM_RISE/FALL
07/18/05   bmg     Added rfnv_intelliceiver_detector_adjust RFNV and defaults
06/23/05   sar     Mainlined feature RF_HAS_ENHANCED_BAND_CLASS.
06/08/05   sar     Fixed Lint warnings and errors.
06/03/05   dyc     Added rfnv_hdr_g0_im_* items and generic default RF_HDR_G0_IM_* values
05/26/05   sar     Removed definition of NV_MAX_ANT_NO_SIZ to mainline Feature
                   RF_HAS_ANT_SENS.
05/12/05   sar     Added support for Antenna Attributes.
04/26/05   sar     Added function rfnv_get_band_mask() and changed type for 
                   rfnv_rf_bc_config to uint64 to conform to sys.h.
03/31/05   bmg     Added functions rfnv_load_dvga_offset_data and
                   rfnv_load_lna_offset_data to consolidate computation of
                   RFNV RX calibration data
                   Formatted VCO coarse tune table for single lib with
                   IntelliCeiver
                   Added IntelliCeiver calibration to RFNV structure
03/21/05   bmg     Fixed datatype for RX cal chan LRU list.
                   Added IntelliCeiver support for DC offsets.
02/21/05   dyc     Added definitions for band specific PA DVS voltage setting.
01/17/05   sar     Added definition for HDR P1 PA Rise/Fall adjustments. 
12/09/04   bmg     Expanded VCO coarse tune 2 to support 16 frequency bins
12/08/04   bmg     Added RX cal channel least-recently-used list support
                   for enhanced automated calibration.
12/05/04   sar     Added featurized support for Coarse Tune VCO defaults for 
                   non-febc build.
12/05/04   sar     Removed Coarse Tune VCO defaults.
11/11/04   sar     Defined filler values for olta items.
11/03/04   sar     Added definition for default value for RF GPS configuration.
10/25/04   bmg     Added VCO coarse tune 2 item default value
10/20/04   sar     Added prototype for rfnv_set_defaults().
10/14/04   bmg     Removed unused RX linearizer NV items from the RF NV
                   structure.  Converted to the new offset_vs_freq structure.
                   Added power mode support.
09/03/04   dbc     Added support for expected TX AGC versus HDET reading table
07/13/04   jac     Added NV_GPS_RF_CONFIG_I to common items list.
07/07/04   sar     Set default for RF_BC_CONFIG according to defined target level flags. .
06/21/04   sar     Added support for chain 1 support for vco coarse tune table and rfr_bb_filter.
06/07/04   sar     Removed members rfnv_tx/rx_cal_channels from rfnv_digital_items_type.
                   Removed default values for interpolation coefficients from
                   this file. 
05/28/04   sar     Updated feature flag names for the nv tables.
05/25/04   sar     added support for legacy cdma & pcs bands.
05/24/04   sar     Added support for PAR MAP support.
05/19/04   sar     Added default rf_hw_config.
05/21/04   dbc     Updated rfnv_backoff_chan_config_type enum
05/11/04   sar     Added support rfnv_hdr_im_fall, rfnv_hdr_im_rise and rfnv_hdr_im_level.
05/11/04   jac     Mainlined rfnv_cdma_items_ptr element in rfnv_all_data_type 
                   struct. Rf.c will not compile if this element does not exist.
04/26/04   sar     Added support NV_RF_BC_CONFIG_I and NV_RF_HW_CONFIG_I.
                   Fixed defaults for Linear Interpolation values.
04/14/04   sar     Added support for NV_RFR_BB_FILTER_I and 
                   NV_RFR_IQ_LINE_RESISTOR_I. Removed NV_XXX_RF_TUNE_RESERVED_I.
04/12/04   dyc     Added FEATURE_TRIMODE_ITEMS to all data ptr struct for rfnv_pcs_items_ptr
04/09/04   jac     Added rfnv_cdma_items_ptr to all data ptr struct for legacy support.
03/19/04   et      added interpolation (frequency comp) support
03/19/04   sar     Added support for new band classes and added new member
                   variables in rf_digital_items structure to support new NV 
                   items.
02/18/04   sar     Added support for GPS Chain Delay cal parameter rfnv_gps1_rf_delay.
01/30/04   zg      Fixed "#ifdef RF_HAS_GPS"
01/30/04   SAR     Changed lna rise/fall and im levels to be chain specific.
01/26/04   SAR     Removed support for Tx Warmup Delay, Pa Offsets, Sub-PCG 
                   Warmup delay,mismatch compensations, LNA Bypass 0-3 and 
                   LNA Non-Bypass 0-3 timers. 
12/18/03   SAR     Added constant definitions for common table.
11/13/03   SAR     Using a more meaningful name for range declaration in OLTA 
                   arrays in the digital items description. 
11/11/03   SAR     Added support for Band Class 6.
11/11/03   SAR     Added support for Coefficients used in interpolation of 
                   Frequency Compensation.
11/11/03   SAR     Added prototype for rfnv_set_digital_table() to set the 
                   default digital table pointer to the specified band.
11/11/03   dyc     Added C1 dimension to OLTA items.
11/03/03   SAR     Added support for OLTA items.
10/23/03   SAR     Added support for rfnv_c1_vco_coarse_tune_table. Removed
                   featurization in rfnv_all_data_type member definitions.
                   Deleted macro RF_SATURATE().
07/29/03   SAR     Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rex.h"
#include "nv.h"
#include "rficap.h"
#include "rfnv.h"
#include "sys.h"
#include "rfcom.h"
#ifdef RF_HAS_RFCONFIG_RX_LNA
#include "rfconfig_rx_lna.h"
#endif /* RF_HAS_RFCONFIG_RX_LNA */


/*===========================================================================

                       PUBLIC DATA DECLARATIONS

===========================================================================*/
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                            CONSTANT DEFINITIONS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/* Build Version defaults
*/       
#define RF_TEST_CODE_VER    {'N','O','N','E',' ',' ',' ',' '}
#define RF_SYS_SW_VER       {'N','O','N','E',' ',' ',' ',' '}
#define RF_RF_CAL_VER       {'N','O','N','E',' ',' ',' ',' '}
#define RF_CONFIG_VER       {'N','O','N','E',' ',' ',' ',' '}
#define RF_CAL_DATE         0x0
#define RF_NV_LOADED_DATE   0x0
#define RF_CAL_DAT_FILE     {'N','O','N','E',' ',' ',' ',' ',' '}

/* dacc_dc_offset_polarity
*/
#define RF_DACC_DC_OFFSET_POLARITY  0x0

/* rtc_time_adjust
*/
#define RF_RTC_TIME_ADJUST          0x0

#ifdef RF_HAS_RX_PLL_LOCK_WAIT_TIME_600US

#define RF_DIGITAL_PLL_LOCK_TIME    0x6

#else
/* digital_pll_lock_time 
*/
#define RF_DIGITAL_PLL_LOCK_TIME    0x8

#endif

/* minimum rx rssi in 1/10 dB (85 dB dynamic range) 
*/
#define RF_MINIMUM_RX_RSSI          -1150

/* cdma cagc dynamic range in 1/10 dB (102.4 dB dynamic range) 
*/
#define RF_CDMA_CAGC_DYNAMIC_RANGE  1024

/* default VCO coarse tune table 
*/

#ifdef RF_HAS_POWER_MODE_LIB
    #define RF_DEFAULT_VCO_COARSE_TUNE_TABLE  {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                               {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}

    #define RF_DEFAULT_VCO_COARSE_TUNE_2      {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                               {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}
#else
    #define RF_DEFAULT_VCO_COARSE_TUNE_TABLE  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    #define RF_DEFAULT_VCO_COARSE_TUNE_2      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif

#define  RF_PATHS_TABLE_SIZ        2
#define  RF_RX_AGC_VREF_VAL_V      {0,0}
#define  RF_TX_COMP_VS_FREQ_V      {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}

/*Tx Power Compensation based on frequency and temperature*/
#ifdef RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR
#define RF_TEMP_FREQ_BASED_TX_PWR_COMP_V {{{0,0,0,0,0,0,0,0}, \
                                          {0,0,0,0,0,0,0,0}, \
                                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}, \
                                          {{0,0,0,0,0,0,0,0}, \
                                          {0,0,0,0,0,0,0,0}, \
                                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}, \
                                          {{0,0,0,0,0,0,0,0}, \
                                          {0,0,0,0,0,0,0,0}, \
                                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}, \
                                          {{0,0,0,0,0,0,0,0}, \
                                          {0,0,0,0,0,0,0,0}, \
                                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}}

#endif

/* Low power table.
*/
#define RF_TX_AGC_TABLE1 \
     {511-477,511-477,511-477,511-477,511-477,511-477,511-450,511-433,511-415,511-402,511-388,511-375,511-\
      362,511-350,511-339,511-327,511-316,511-306,511-295,511-285,511-275,511-265,511-256,511-246,511-\
      237,511-227,511-218,511-209,511-200,511-191,511-181,511-173,511-162,511-154,511-144,511-132,511-132}

/* Intermediate power table. 
*/
#define RF_TX_AGC_TABLE2 \
     {511-499,511-499,511-499,511-499,511-499,511-499,511-474,511-457,511-442,511-428,511-414,511-401,511-\
      389,511-377,511-365,511-354,511-343,511-332,511-321,511-311,511-302,511-291,511-280,511-272,511-\
      262,511-252,511-243,511-233,511-224,511-214,511-205,511-196,511-187,511-177,511-166,511-155,511-155}

/* High power table. 
*/
#define RF_TX_AGC_TABLE3 \
     {511-495,511-495,511-495,511-495,511-495,511-495,511-495,511-495,511-495,511-495,511-495,511-495,511-\
      495,511-495,511-495,511-495,511-476,511-458,511-441,511-425,511-408,511-394,511-380,511-367,511-\
      354,511-341,511-329,511-319,511-307,511-296,511-286,511-275,511-264,511-253,511-241,511-221,511-221}

/* CDMA Tx linearizer master offset */
/* TABLE1,TABLE1,TABLE2,TABLE3 Original order. 
*/
#define  RF_TX_LIN_MASTER_OFF_V  {RF_TX_AGC_TABLE1, RF_TX_AGC_TABLE2, RF_TX_AGC_TABLE2, RF_TX_AGC_TABLE3}


#if defined (RF_HAS_BETA_SCALING_TX_AGC)
/* Here Beta Scaling is enabled */

#ifdef  RF_HAS_PDM_LIN_TBL_SIZ_48
#define RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ   48
#else
#define RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ   64
#endif

/*
 * TX power (in 1/10 dB units) and associated PDM values [0,511].
 * -604 <= TX power <= 420 dBm/10 (dynamic range 1024 dBm/10).
 *
 * Only the first RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ values
 * in the following #defines will be used.
 */

/* TX power for PA state 0 (in 1/10 dB units) */
/* Associated PDM values in RF_TX_CDMA_BETA_SCALING_PDM_TBL_0 */
#define RF_TX_CDMA_BETA_SCALING_LIN_TBL_0 \
{261,  258,  256,  250,  243,  234,  226,  218,  209,  198,  187,  175, \
 163,  151,  140,  128,  117,  108,   99,   84,   70,   59,   48,   36, \
  24,   11,    1,  -14,  -28,  -41,  -54,  -67,  -81,  -93, -105, -135, \
-167, -193, -225, -257, -286, -315, -350, -371, -412, -440, -465, -475, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}

/* TX power for PA state 1 (in 1/10 dB units) */
/* Associated PDM values in RF_TX_CDMA_BETA_SCALING_PDM_TBL_1 */
#define RF_TX_CDMA_BETA_SCALING_LIN_TBL_1 \
{261,  258,  256,  250,  243,  234,  226,  218,  209,  198,  187,  175, \
 163,  151,  140,  128,  117,  108,   99,   84,   70,   59,   48,   36, \
  24,   11,    1,  -14,  -28,  -41,  -54,  -67,  -81,  -93, -105, -135, \
-167, -193, -225, -257, -286, -315, -350, -371, -412, -440, -465, -475, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}

/* TX power for PA state 2 (in 1/10 dB units) */
/* Associated PDM values in RF_TX_CDMA_BETA_SCALING_PDM_TBL_2 */
#define RF_TX_CDMA_BETA_SCALING_LIN_TBL_2 \
{261,  258,  256,  250,  243,  234,  226,  218,  209,  198,  187,  175, \
 163,  151,  140,  128,  117,  108,   99,   84,   70,   59,   48,   36, \
  24,   11,    1,  -14,  -28,  -41,  -54,  -67,  -81,  -93, -105, -135, \
-167, -193, -225, -257, -286, -315, -350, -371, -412, -440, -465, -475, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}

/*
{300,  283,  266,  249,  232,  215,  198,  181,  164,  147,  130,  113, \
  96,   79,   62,   45,   28,   11,   -6,  -23,  -40,  -57,  -74,  -91, \
-108, -125, -142, -159, -176, -193, -210, -227, -244, -261, -278, -295, \
-312, -329, -346, -363, -380, -397, -414, -431, -448, -465, -482, -499, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}
*/

/* TX power for PA state 3 (in 1/10 dB units) */
/* Associated PDM values in RF_TX_CDMA_BETA_SCALING_PDM_TBL_3 */
#define RF_TX_CDMA_BETA_SCALING_LIN_TBL_3 \
{261,  258,  256,  250,  243,  234,  226,  218,  209,  198,  187,  175, \
 163,  151,  140,  128,  117,  108,   99,   84,   70,   59,   48,   36, \
  24,   11,    1,  -14,  -28,  -41,  -54,  -67,  -81,  -93, -105, -135, \
-167, -193, -225, -257, -286, -315, -350, -371, -412, -440, -465, -475, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}

/*
{420,  398,  376,  355,  333,  311,  289,  267,  246,  224,  202,  180, \
 159,  137,  115,   93,   71,   50,   28,    6,  -16,  -38,  -59,  -81, \
-103, -125, -146, -168, -190, -212, -234, -255, -277, -299, -321, -343, \
-364, -386, -408, -430, -451, -473, -495, -517, -539, -560, -582, -604, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}
*/


/* Only the first 48 values in the following #defines are used in Beta Scaling */

/* PA state 0: PDM values for which TX power is measured */
#define RF_TX_CDMA_BETA_SCALING_PDM_TBL_0 \
{  1,   5,  10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75, \
  80,  85,  90,  95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, \
 160, 165, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}

/* PA state 1: PDM values for which TX power is measured */
#define RF_TX_CDMA_BETA_SCALING_PDM_TBL_1 \
{  1,   5,  10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75, \
  80,  85,  90,  95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, \
 160, 165, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}

/* PA state 2: PDM values for which TX power is measured */
#define RF_TX_CDMA_BETA_SCALING_PDM_TBL_2 \
{  1,   5,  10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75, \
  80,  85,  90,  95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, \
 160, 165, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}

/* PA state 3: PDM values for which TX power is measured */
#define RF_TX_CDMA_BETA_SCALING_PDM_TBL_3 \
{  1,   5,  10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75, \
  80,  85,  90,  95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, \
 160, 165, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}

/*
{  1,  11,  22,  33,  43,  54,  65,  76,  87,  98, 109, 120, 130, 141, 152, 163, \
 174, 185, 196, 207, 217, 228, 239, 250, 261, 272, 283, 294, 304, 315, 326, 337, \
 348, 359, 370, 381, 391, 402, 413, 424, 435, 446, 457, 468, 478, 489, 500, 511, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}
*/

/* PDM values in the range [128, 383] */
/*
#define RF_TX_CDMA_BETA_SCALING_PDM_TBL_0 \
{128, 133, 138, 143, 148, 153, 158, 163, 168, 173, 178, 183, 188, 193, 198, 203, \
 208, 213, 218, 223, 228, 233, 238, 243, 248, 253, 258, 263, 268, 273, 278, 283, \
 288, 293, 298, 303, 308, 313, 318, 323, 328, 333, 338, 343, 348, 353, 358, 363, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}
*/
#endif /* RF_HAS_BETA_SCALING_TX_AGC */

#if defined(RF_HAS_PDM_VS_POWER_TX_CAL)

#define RF_TX_CDMA_HIDDEN_REGS_LIN_TBL_0 \
{ -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768}

/*{RF_CDMA_HIDDEN_REG_LIN_TX_PWR_NOT_USED_VAL,261,  258,  256,  250,  243,  234,  226,  218,  209,  198,  187,  175, \
 163,  151,  140,  128,  117,  108,   99,   84,   70,   59,   48,   36, \
  24,   11,    1,  -14,  -28,  -41,  -54,  -67,  -81,  -93, -105, -135, \
-167, -193, -225, -257, -286, -315, -350, -371, -412, -440, -465, -475, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}*/

/* TX power for PA state 1 (in 1/10 dB units) */
/* Associated PDM values in RF_TX_CDMA_BETA_SCALING_PDM_TBL_1 */
#define RF_TX_CDMA_HIDDEN_REGS_LIN_TBL_1 \
{ -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768}

/*{RF_CDMA_HIDDEN_REG_LIN_TX_PWR_NOT_USED_VAL,  258,  256,  250,  243,  234,  226,  218,  209,  198,  187,  175, \
 163,  151,  140,  128,  117,  108,   99,   84,   70,   59,   48,   36, \
  24,   11,    1,  -14,  -28,  -41,  -54,  -67,  -81,  -93, -105, -135, \
-167, -193, -225, -257, -286, -315, -350, -371, -412, -440, -465, -475, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}*/

/* TX power for PA state 2 (in 1/10 dB units) */
/* Associated PDM values in RF_TX_CDMA_BETA_SCALING_PDM_TBL_2 */
#define RF_TX_CDMA_HIDDEN_REGS_LIN_TBL_2 \
{ -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768}

/*{261,  258,  256,  250,  243,  234,  226,  218,  209,  198,  187,  175, \
 163,  151,  140,  128,  117,  108,   99,   84,   70,   59,   48,   36, \
  24,   11,    1,  -14,  -28,  -41,  -54,  -67,  -81,  -93, -105, -135, \
-167, -193, -225, -257, -286, -315, -350, -371, -412, -440, -465, -475, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}*/

/*
{300,  283,  266,  249,  232,  215,  198,  181,  164,  147,  130,  113, \
  96,   79,   62,   45,   28,   11,   -6,  -23,  -40,  -57,  -74,  -91, \
-108, -125, -142, -159, -176, -193, -210, -227, -244, -261, -278, -295, \
-312, -329, -346, -363, -380, -397, -414, -431, -448, -465, -482, -499, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}
*/

/* TX power for PA state 3 (in 1/10 dB units) */
/* Associated PDM values in RF_TX_CDMA_BETA_SCALING_PDM_TBL_3 */
#define RF_TX_CDMA_HIDDEN_REGS_LIN_TBL_3 \
{ -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768, \
  -32768,-32768,-32768,-32768}

/*{261,  258,  256,  250,  243,  234,  226,  218,  209,  198,  187,  175, \
 163,  151,  140,  128,  117,  108,   99,   84,   70,   59,   48,   36, \
  24,   11,    1,  -14,  -28,  -41,  -54,  -67,  -81,  -93, -105, -135, \
-167, -193, -225, -257, -286, -315, -350, -371, -412, -440, -465, -475, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}*/

/*
{420,  398,  376,  355,  333,  311,  289,  267,  246,  224,  202,  180, \
 159,  137,  115,   93,   71,   50,   28,    6,  -16,  -38,  -59,  -81, \
-103, -125, -146, -168, -190, -212, -234, -255, -277, -299, -321, -343, \
-364, -386, -408, -430, -451, -473, -495, -517, -539, -560, -582, -604, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}
*/


/* Only the first 48 values in the following #defines are used in Beta Scaling */

/* PA state 0: PDM values for which TX power is measured */
#define RF_TX_CDMA_HIDDEN_REGS_PDM_TBL_0 \
{ 32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767}

/*{  1,   5,  10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75, \
  80,  85,  90,  95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, \
 160, 165, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}*/

/* PA state 1: PDM values for which TX power is measured */
#define RF_TX_CDMA_HIDDEN_REGS_PDM_TBL_1 \
{ 32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767}

/*{  1,   5,  10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75, \
  80,  85,  90,  95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, \
 160, 165, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}*/

/* PA state 2: PDM values for which TX power is measured */
#define RF_TX_CDMA_HIDDEN_REGS_PDM_TBL_2 \
{ 32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767}

/*{  1,   5,  10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75, \
  80,  85,  90,  95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, \
 160, 165, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}*/

/* PA state 3: PDM values for which TX power is measured */
#define RF_TX_CDMA_HIDDEN_REGS_PDM_TBL_3 \
{ 32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767,32767,32767,32767,32767,32767,32767, \
  32767,32767,32767,32767}

/*{  1,   5,  10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75, \
  80,  85,  90,  95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, \
 160, 165, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, \
 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0}*/


#endif

#if defined (RF_HAS_BETA_SCALING_TX_AGC) && defined(RF_HAS_PDM_VS_POWER_TX_CAL)
RF_FATAL_ERROR(" Cannot define Beta scaling and Hidden regs Cal features together");
#endif

#if defined (RF_HAS_BETA_SCALING_TX_AGC)

/* Default PDM values (for PA states 0,1,2,3) when Beta Scaling is enabled */
#define RF_TX_CDMA_PDM_TBL_V {RF_TX_CDMA_BETA_SCALING_PDM_TBL_0, \
                              RF_TX_CDMA_BETA_SCALING_PDM_TBL_1, \
                              RF_TX_CDMA_BETA_SCALING_PDM_TBL_2, \
                              RF_TX_CDMA_BETA_SCALING_PDM_TBL_3}

/* Default TX power for the PDM values above (for PA states 0,1,2,3) */
#define RF_TX_CDMA_LIN_TBL_V {RF_TX_CDMA_BETA_SCALING_LIN_TBL_0, \
                              RF_TX_CDMA_BETA_SCALING_LIN_TBL_1, \
                              RF_TX_CDMA_BETA_SCALING_LIN_TBL_2, \
                              RF_TX_CDMA_BETA_SCALING_LIN_TBL_3}

#elif defined (RF_HAS_PDM_VS_POWER_TX_CAL) 

#define RF_TX_CDMA_PDM_TBL_V {RF_TX_CDMA_HIDDEN_REGS_PDM_TBL_0, \
                              RF_TX_CDMA_HIDDEN_REGS_PDM_TBL_1, \
                              RF_TX_CDMA_HIDDEN_REGS_PDM_TBL_2, \
                              RF_TX_CDMA_HIDDEN_REGS_PDM_TBL_3}

/* Default TX power for the PDM values above (for PA states 0,1,2,3) */
#define RF_TX_CDMA_LIN_TBL_V {RF_TX_CDMA_HIDDEN_REGS_LIN_TBL_0, \
                              RF_TX_CDMA_HIDDEN_REGS_LIN_TBL_1, \
                              RF_TX_CDMA_HIDDEN_REGS_LIN_TBL_2, \
                              RF_TX_CDMA_HIDDEN_REGS_LIN_TBL_3}
#endif /* RF_HAS_BETA_SCALING_TX_AGC */

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
#define RF_BLOCK_MASK_DEFAULT_VALUE 0xFFFFFFFF
#define RF_BLOCK_DIGITAL_ITEM_PTR_DEFAULT_VALUE NULL
#endif

/* CDMA Tx linearizer versus temperature
*/
#define  RF_TX_LIN_VS_TEMP_V  {{0,0,0,0,0,0,0,0}, \
                               {0,0,0,0,0,0,0,0}, \
                               {0,0,0,0,0,0,0,0}, \
                               {0,0,0,0,0,0,0,0}}

/* CDMA Tx slope versus temperature
*/
#define  RF_TX_SLP_VS_TEMP_V  {{0,0,0,0,0,0,0,0}, \
                               {0,0,0,0,0,0,0,0}, \
                               {0,0,0,0,0,0,0,0}, \
                               {0,0,0,0,0,0,0,0}}
                               
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 CDMA mode Tx power limit data vs temperature,
 frequency, TX_AGC_ADJ PDM setting, and HDET reading
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* CDMA Tx limit versus temperature
*/
#define  RF_TX_LIM_VS_TEMP_V  {843,843,843,843,843,843,843,843}

/* CDMA Tx limit versus frequency
*/
#define  RF_TX_LIM_VS_FREQ_V  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

///////////////////////////// HDET ////////////////////////////
#ifdef RF_HAS_HDET_UINT16

/* CDMA expected HDET versus AGC V2
*/
#define  RF_EXP_HDET_VS_AGC_V2_V {32,64,160,304,480,740,800,920,1440,1760,2080,2400,2880,3200,3600,4000,4095}
//#define  RF_EXP_HDET_VS_AGC_V2_V {8,16,40,76,120,160,200,280,360,440,520,600,720,800,920,1000,1023}

/* ADC HDET reading offset V2
*/
#define  RF_HDET_OFF_V2_V  480

/* ADC HDET reading span V2
*/
#define  RF_HDET_SPN_V2_V  2400

#define  RF_TX_AGC_VS_HDET_TABLE_SIZ 4096

#else // HDET IS OF UINT8 NOT OF RF_HAS_HDET_UINT16

/* CDMA expected HDET versus AGC
*/
#define  RF_EXP_HDET_VS_AGC_V {2,4,10,19,29,38,50,66,87,107,127,152,176,201,226,251,255}

/* ADC HDET reading offset
*/
#define  RF_HDET_OFF_V  28

/* ADC HDET reading span
*/
#define  RF_HDET_SPN_V  98

#define  RF_TX_AGC_VS_HDET_TABLE_SIZ 256

#endif // HDET IS OF UINT8

/* Expected TX AGC VS HDET */
#define  RF_EXP_TX_AGC_VS_HDET_V {0}
/////////////////////////// HDET ////////////////////////////

/* LNA bypass range offset vs. freq. compensation
*/
#ifdef RF_HAS_POWER_MODE_LIB
#define  RF_LNA_1_OFFSET_VS_FREQ_V    {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}, \
                                       {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}}
#define  RF_LNA_2_OFFSET_VS_FREQ_V    {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}, \
                                       {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}}
#define  RF_LNA_3_OFFSET_VS_FREQ_V    {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}, \
                                       {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}}
#define  RF_LNA_4_OFFSET_VS_FREQ_V    {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}, \
                                       {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}}
#else
#define  RF_LNA_1_OFFSET_VS_FREQ_V    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define  RF_LNA_2_OFFSET_VS_FREQ_V    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define  RF_LNA_3_OFFSET_VS_FREQ_V    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define  RF_LNA_4_OFFSET_VS_FREQ_V    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif /* RF_HAS_POWER_MODE_LIB */

/* Phase rotation setting for LNA Stages 2 to 5
*/
#define  RF_AGC_PHASE_OFFSET_V   0

/* LNA range polarity
*/
#define  RF_LNA_RANGE_POL_V   CAGC_RF_LNA_POL_NORM_V

/* Mixer range polarity
*/
#define  RF_MIXER_RANGE_POL_V  0x0

/* LNA range delay time from CAGC to RX Front
*/
#define  RF_LNA_RANGE_DELAY_V  85

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 Miscellaneous other NV items
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* CDMA Tx limiter loop gain, scaled by RF_CDMA_TX_LIM_ADJ_SCALE.
*/
#define  RF_TX_LIM_ADJ_GAIN_V  (RF_CDMA_TX_LIM_ADJ_SCALE / 4)

/* CDMA/PCS TX Attenuation Limit 
*/
#define  RF_TX_ATTEN_LIMIT_V  511

/* PA_R1/R3 rise values
*/
#define  RF_PA_RISE_DIS_V 255
#define  RF_R1_RISE_V  185
#define  RF_R2_RISE_V  RF_PA_RISE_DIS_V
#define  RF_R3_RISE_V  RF_PA_RISE_DIS_V



/* PA_R1/R3 fall values
*/
#define  RF_PA_FALL_DIS_V 255
#define  RF_R1_FALL_V  175
#define  RF_R2_FALL_V  RF_PA_FALL_DIS_V
#define  RF_R3_FALL_V  RF_PA_FALL_DIS_V

/* P1 PA Rise/Fall adjustments
*/
#ifdef RF_HAS_DEFAULT_PA_OFFSETS_SET_TO_ZERO

#define  RF_P1_PA_RISE_FALL_OFF_V  {{0,0,0,0,0,0,0,0}, \
                                    {0,0,0,0,0,0,0,0}}
#else
#error code not present
#endif
/* HDR P1 PA Rise/Fall adjustments 
*/
#define  RF_HDR_P1_PA_RISE_FALL_OFF_V  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
/* Low, med, and hi volts for PA backoff calibration.
*/
#define  RF_PA_BACKOFF_CAL_VOLTS_V  {32,37,42}

/* Rx Front Group Delay
*/
#ifdef RF_HAS_POWER_MODE_LIB
#define  RF_GRP_DELAY_ADJ_V  { {0, 0, 0}, {0, 0, 0} }
#else 
#define  RF_GRP_DELAY_ADJ_V  {0, 0}
#endif /* RF_HAS_POWER_MODE_LIB */

/* VGA Gain Offset
*/
#define  RF_VGA_GAIN_OFFSET_V  {2300,2300}

/* Gain Offset freq and temp comp defaults
*/
#ifdef RF_HAS_POWER_MODE_LIB
#define  RF_VGA_GAIN_OFFSET_VS_FREQ_V  {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}, \
                                        {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}}
#else
#define  RF_VGA_GAIN_OFFSET_VS_FREQ_V  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#endif /* RF_HAS_POWER_MODE_LIB */
#define  RF_VGA_GAIN_OFFSET_VS_TEMP_V  {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}}


/* DACC Accumulator defaults
*/

#ifdef RF_HAS_POWER_MODE_LIB
    #define  RF_DACC_IACCUM0_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
    #define  RF_DACC_IACCUM1_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
    #define  RF_DACC_IACCUM2_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
    #define  RF_DACC_IACCUM3_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
    #define  RF_DACC_IACCUM4_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
    #define  RF_DACC_QACCUM0_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
    #define  RF_DACC_QACCUM1_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
    #define  RF_DACC_QACCUM2_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
    #define  RF_DACC_QACCUM3_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
    #define  RF_DACC_QACCUM4_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
#else
    #define  RF_DACC_IACCUM0_V  {0x0,0x0}
    #define  RF_DACC_IACCUM1_V  {0x0,0x0}
    #define  RF_DACC_IACCUM2_V  {0x0,0x0}
    #define  RF_DACC_IACCUM3_V  {0x0,0x0}
    #define  RF_DACC_IACCUM4_V  {0x0,0x0}
    #define  RF_DACC_QACCUM0_V  {0x0,0x0}
    #define  RF_DACC_QACCUM1_V  {0x0,0x0}
    #define  RF_DACC_QACCUM2_V  {0x0,0x0}
    #define  RF_DACC_QACCUM3_V  {0x0,0x0}
    #define  RF_DACC_QACCUM4_V  {0x0,0x0}
#endif

/* RF Chipset */

#ifdef RF_HAS_POWER_MODE_LIB
    #define  RF_IM2_I_VALUE_V   {{0x40,0x40,0x40},{0x40,0x40,0x40}}
    #define  RF_IM2_Q_VALUE_V   {{0x40,0x40,0x40},{0x40,0x40,0x40}}
    #define  RF_IM2_TRANSCONDUCTOR_VALUE_V  {{0x0,0x0,0x0},{0x0,0x0,0x0}}
#else
    #define  RF_IM2_I_VALUE_V   {0x40,0x40}
    #define  RF_IM2_Q_VALUE_V   {0x40,0x40}
    #define  RF_IM2_TRANSCONDUCTOR_VALUE_V  {0x0,0x0}
#endif
/* Reserved for RF Tune */
#define  RF_TUNE_RESERVED_V 0

 /* Adjustment factor */
#define  RF_ADJ_FACTOR_V 0

/* Tx linearizer master 0 */
#define  RF_TX_LIN_MASTER0_V {0,{0}}

/* Tx linearizer master 1 */
#define  RF_TX_LIN_MASTER1_V {0,{0}}

/* Tx linearizer master 2 */
#define  RF_TX_LIN_MASTER2_V {0,{0}}

/* Tx linearizer master 3 */
#define  RF_TX_LIN_MASTER3_V {0,{0}}


/* Filler values for Coefficients used in interpolation of Frequency Compensation 
 Values for Rx and Tx */
#define  RF_CAL_CHAN_FILLER_V {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                               {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}
                               
#ifndef RF_HAS_RFCONFIG_RX_LNA
/* OLTA items */
#ifdef RF_HAS_POWER_MODE_LIB
    #define  RF_OLTA_FILLER_V  {{{0,0,0,0},{0,0,0,0},{0,0,0,0}}, \
                                {{0,0,0,0},{0,0,0,0},{0,0,0,0}}}
#else
    #define  RF_OLTA_FILLER_V  {{0,0,0,0},{0,0,0,0}}
#endif
#endif /* !RF_HAS_RFCONFIG_RX_LNA */

#ifdef RF_HAS_PA_DVS_NV
/* PA DVS voltage setting (in mV, 0 represents PA DVS disabled */
#define  PA_DVS_DISABLE_V 0
#endif
                                
/* 
* Generic filler values for HDR gain state 0 IM values.  If these values meed
* to be band specific, then the band specific header file should be used for the
* definitions.
*/
#define  RF_HDR_G0_IM_FALL_DEFAULT_V  -72
#define  RF_HDR_G0_IM_RISE_DEFAULT_V  -61
#define  RF_HDR_G0_IM_LEVEL_DEFAULT_V -61

/*
* The following are the HDR IM values used to override the existing defaults.  These
* represent the new defaults associated with RFR6500 (BC0).
*/
#define  RF_RFR6500_HDR_IM_FALL_V   -45
#define  RF_RFR6500_HDR_IM_RISE_V   -42
#define  RF_RFR6500_HDR_IM_LEVEL_V  -42

/* Default IntelliCeiver calibration.  This will result in the use of
   globally safe detector thresholds */
#define RF_INTELLICEIVER_CAL_V {{0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0}}

/* Default IntelliCeiver detector threshold adjustments.
   Zero will not move the thresholds from the default RF driver settings */
#define RF_INTELLICEIVER_DETECTOR_ADJUST_V {{{0,0,0,0,0},{0,0,0,0,0}},{{0,0,0,0,0},{0,0,0,0,0}}}

/* Default 1x PA hysteresis timer.
   Zero disables PA hysteresis, and requires power hysteresis configured
   with the PA rise and fall NV items */
#define RF_1X_PA_R_TIMER_V 0

/* Default 1xEV-DO PA hysteresis timer.
   26us delay was the previous default, and requires power hysteresis configured
   with the PA rise and fall NV items */
#define RF_1XEV_DO_PA_R_TIMER_V 0

/* Default band class specific NV item 
   Initialization false, value = 0 */
#define RF_BC_PA_R_MAP_V   {FALSE, 0}

#ifdef RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
#define  RF_P2_PA_RISE_FALL_OFF_V  {{0,0,0,0,0,0,0,0}, \
                                    {0,0,0,0,0,0,0,0}}

#define  RF_P3_PA_RISE_FALL_OFF_V  {{0,0,0,0,0,0,0,0}, \
                                    {0,0,0,0,0,0,0,0}}
#endif /* RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST */

#ifdef RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
#error code not present
#endif /* RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                    SCALE FACTORS FOR FIXED POINT MATH

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/* Scale factor for raw FM RSSI values
*/
#define RF_FM_RSSI_SCALE      ((word)100)

/* Scale factor for FM transmit agc initial setting vs. power level
*/
#define RF_FM_AGC_PWR_SCALE             2

/* Scale factor for the CDMA transmit power limiter loop gain
*/
#define RF_CDMA_TX_LIM_ADJ_SCALE        8

#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#define NV_PA_SP_OFFSET_TABLE_SIZ       8
#endif
#define NV_HDR_PA_SP_OFFSET_TABLE_SIZ   16

#define RF_NUM_TX_LINEARIZERS  NV_NUM_TX_LINEARIZERS

#define NV_MAX_NUM_CAL_CHAN    16

#ifdef FEATURE_TX_POWER_BACK_OFF
  typedef enum
  {
    PILOT_FCH                   = 0,
    PILOT_DCCH_9600             = 1,
    NUMBER_OF_BACKOFF_CONFIGS,
    BACKOFF_STEADY_STATE        = 1000,
    BACKOFF_FIRST_RUN           = 2000,
    REFRESH_BACKOFF             = 3000,
    NO_BACK_OFF_CONFIG          = 4000,
    BACKOFF_CHAN_CONFIG_VALID   = 5000
  } rfnv_backoff_chan_config_type ;
#endif

#define RF_BC6_TX_LIM_OFFSET   768

#define RF_TX_LIN_TRIM_MAX_NEG (-64) /* max negative Tx linearizer trim  */
#define RF_TX_LIN_TRIM_MAX_POS (+63) /* max positive Tx linearizer trim  */
#define RF_MAX_LNA_STAGES      4
#define RF_GPS_CHAIN_DELAY_V   0
#define RF_RFR_BB_FILTER       0
#define RF_RFR_IQ_LINE_RESISTOR 0
#define RF_BC_CONFIG           RF_DEF_BC    /* Default Band Mask            */
#define RF_HW_CONFIG           RF_DEF_HW    /* Default RF hardware          */
#define RF_PAR_MAP             0xA8         /* PA_R_MAP Value               */
#define RF_GPS_RF_CONFIG       0            /* GPS RF Configuartion         */

#ifdef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
#ifdef RF_HAS_SUPPORT_FOR_HIGH_RES_HKADC
#define RF_THERM_BIN_DEFAULT_V {0,585,1170,1755,2340,2925,3510,4094}
#else
#define RF_THERM_BIN_DEFAULT_V {0,36,72,108,144,180,216,254}
#endif
#endif

#define RF_RX_CAL_CHAN_LRU_V   {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}

#ifdef RF_HAS_HW_DZRF6285
#error code not present
#endif // RF_HAS_HW_DZRF6285 

#define RF_PM_CONFIG_DEFAULT_V 0

#ifdef RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL
//nv_process_cal_type: uint16 pr_info, uint8 rcv, uint8 rch, uint8 cc
//ER EC NV = min(max(1, round(ERV * 640) + 128), 255) in binary
//ER/EC item defaults set as if process error was measured as zeroes.
#define RF_PROCESS_CAL_DEFAULT_V {0, 128, 128, 128}
#endif //RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL

#ifdef RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL
//nv_dc_cal_type: uint8 i_dc, uint8 q_dc
#define RF_DC_CAL_DEFAULT_V {0, 0}
#endif //RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL

#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
//uint16 supply_value
#define RF_SUPPLY_VAL_DEFAULT_V 0
//nv_lo_cal_type: uint16 fs, uint16 ms
#define RF_LO_CAL_DEFAULT_V {0, 0}
#endif //RF_HAS_SUPPORT_FOR_LO_PWC_CAL

#ifdef RF_HAS_SUPPORT_FOR_LO_IM2_CAL
//nv_im2_cal_type: uint8 res, uint8 fs_i, uint8 fs_q, uint8 ms_i, uint8 ms_q, uint8 ss_i, uint8 ss_q
#define RF_IM2_CAL_DEFAULT_V {0, 0, 0, 0, 0, 0, 0}
#endif //RF_HAS_SUPPORT_FOR_LO_IM2_CAL

/* Band class specific PA_R_MAP struct */
typedef struct {
    boolean nv_initialized;     /* flag to indicate if nv is populated */
    byte value;                 /* bc specific PA_R_MAP value */
} rfnv_bc_pa_r_map_type;

#ifdef RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR
 typedef struct 
 {
   int2 rfnv_tx_comp_lin_vs_temp[NV_TEMP_TABLE_SIZ];
   int2 rfnv_tx_comp_slp_vs_temp[NV_TEMP_TABLE_SIZ];
   int2 rfnv_tx_pwr_comp_vs_freq[NV_FREQ_TABLE_SIZ];

 }rfnv_temp_freq_based_tx_pwr_comp_type;
#endif

/*-------------------------------------------*/
/*    Defines valid digital 1x bands         */
/*-------------------------------------------*/
typedef enum
{

    /* BC 0 - US CELL */
    #ifdef RF_HAS_BC0_NV_TABLE
    RF_BAND_CLASS_0,
    #endif

    /* BC 1 - US PCS */
    #ifdef RF_HAS_BC1_NV_TABLE
    RF_BAND_CLASS_1,
    #endif
    
    /* BC 2 - JTACS */
    #ifdef RF_HAS_BC2_NV_TABLE
    RF_BAND_CLASS_2,
    #endif

    /* BC 3 - JCDMA */
    #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
    #endif
    
    /* BC 4 - Korean PCS */
    #ifdef RF_HAS_BC4_NV_TABLE
    RF_BAND_CLASS_4,
    #endif
    
    /* BC 5 - NMT (450MHz) */
    #ifdef RF_HAS_BC5_NV_TABLE
    RF_BAND_CLASS_5,
    #endif
    
    /* BC 6 - IMT (2.1GHz) */
    #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
    #endif
    
    #ifdef RF_HAS_BC10_NV_TABLE
    RF_BAND_CLASS_10,
    #endif

    /* BC 11.*/
    #ifdef RF_HAS_BC11_NV_TABLE
#error code not present
    #endif
    
    /* BC 14 - US PCS + 5 MHz.*/
    #ifdef RF_HAS_BC14_NV_TABLE
    RF_BAND_CLASS_14,
    #endif
    
    /* BC 15 */
    #ifdef RF_HAS_BC15_NV_TABLE
    RF_BAND_CLASS_15,
    #endif
    

    #ifdef RF_HAS_GPS_NV_TABLE 
#error code not present
    #endif

  /* add additional mode enum field here */

  RF_BAND_CLASS_MAX

} rfnv_1x_band_class_type;
/* -----------------------------------------------------------------------
** Digital NV calibration tables
** ----------------------------------------------------------------------- */

typedef struct rfnv_band_block_digital_items /* rfnv_digital_items_type */
{
  /* Id of the band */
  rfnv_1x_band_class_type band_id;
  
  /* Holds either the base VREF value, or the difference between
     the base and switched values
  */
  byte rfnv_rx_agc_vref_val[RF_PATHS_TABLE_SIZ];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     CDMA mode RX/TX_GAIN_COMP values vs frequency
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Tx gain compensation versus frequency
  */
  int1 rfnv_cdma_tx_comp_vs_freq[RF_NUM_TX_LINEARIZERS][NV_FREQ_TABLE_SIZ];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     CDMA mode Tx AGC linearization (MSM2P data is derived from these tables)
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined (RF_HAS_BETA_SCALING_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)

  /* PDM values for TX calibration (for RF_NUM_TX_LINEARIZERS PA states) */
  int2 rfnv_cdma_tx_pdm_tbl [RF_NUM_TX_LINEARIZERS][NV_CDMA_TX_PDM_LIN_TYPE];

  /* Measured TX power for the PDM values (above) */
  int2 rfnv_cdma_tx_lin_tbl [RF_NUM_TX_LINEARIZERS][NV_CDMA_TX_PDM_LIN_TYPE];

#endif /* RF_HAS_BETA_SCALING_TX_AGC */

  /* CDMA Tx linearizer master offset
  */
  int2 rfnv_cdma_tx_lin_master_off[RF_NUM_TX_LINEARIZERS][NV_CDMA_TX_LIN_MASTER_SIZ];

  /* Tx linearizer with internal PA=00 */
  nv_tx_linearizer_type rfnv_tx_lin_master0;

  /* Tx linearizer with internal PA=01 */
  nv_tx_linearizer_type rfnv_tx_lin_master1;
  
  /* Tx linearizer with internal PA=10 
  */
  nv_tx_linearizer_type rfnv_tx_lin_master2;
  
  /* Tx linearizer with internal PA=11 
  */
  nv_tx_linearizer_type rfnv_tx_lin_master3;

  /* CDMA Tx linearizer versus temperature
  */
  int2 rfnv_cdma_tx_lin_vs_temp[RF_NUM_TX_LINEARIZERS][NV_TEMP_TABLE_SIZ];

  /* CDMA Tx slope versus temperature
  */
  int2 rfnv_cdma_tx_slp_vs_temp[RF_NUM_TX_LINEARIZERS][NV_TEMP_TABLE_SIZ];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     CDMA mode Tx power limit data vs temperature,
     frequency, TX_AGC_ADJ PDM setting, and HDET reading
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* CDMA Tx limit versus temperature
  */
  int2 rfnv_cdma_tx_lim_vs_temp[NV_TEMP_TABLE_SIZ];

  /* CDMA Tx limit versus frequency
  */
  int1 rfnv_cdma_tx_lim_vs_freq[NV_FREQ_TABLE_SIZ];

///////////////////////////////// HDET /////////////////////////////
#ifdef RF_HAS_HDET_UINT16

  /* 
  Expected HDET versus AGC V2
  */
  uint16 rfnv_cdma_exp_hdet_vs_agc[NV_CDMA_EXP_HDET_VS_AGC_SIZ+1];

#else
  /* CDMA expected HDET versus AGC
  */
  byte rfnv_cdma_exp_hdet_vs_agc[NV_CDMA_EXP_HDET_VS_AGC_SIZ+1];
#endif // HDET IS OF UINT16

  /* lookup expected AGC (TX_GAIN_CTL - 768) for any HDET value 0..255
  */
  byte rfnv_cdma_exp_agc_table[RF_TX_AGC_VS_HDET_TABLE_SIZ];

#ifdef RF_HAS_HDET_UINT16
  /* 
  HDET offset V2
  */
  uint16 rfnv_hdet_off;

  /* 
  HDET span V2
  */
  uint16 rfnv_hdet_spn;

#else // HDET IS OF UINT8 NOT OF RF_HAS_HDET_UINT16
  /* ADC HDET reading offset
  */
  byte rfnv_hdet_off;

  /* ADC HDET reading span
  */
  byte rfnv_hdet_spn;   /* ADC HDET reading span */
#endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     CDMA mode LNA on/off thresholds

     LNA rise/fall (dBm to MSM register & NV storage)

        [ (dBm Rx + 106) / 85.333 * 256 ] - 128

     LNA rise/fall (MSM register & NV storage to dBm)

        [ (value + 128) / 256 * 85.333 ] - 106
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* LNA range rise value
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int1 rfnv_lna_range_1_rise[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  int1 rfnv_lna_range_1_rise[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* LNA range fall value
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int1 rfnv_lna_range_1_fall[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  int1 rfnv_lna_range_1_fall[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* LNA range offset
  */
  int2 rfnv_lna_range_1_offset[RF_PATHS_TABLE_SIZ];

  /* 2nd LNA range rise value
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int1 rfnv_lna_range_2_rise[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  int1 rfnv_lna_range_2_rise[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* 2nd LNA range fall value
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int1 rfnv_lna_range_2_fall[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  int1 rfnv_lna_range_2_fall[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* 2nd LNA bypass range offset
  */
  int2 rfnv_lna_range_2_offset[RF_PATHS_TABLE_SIZ];

  /* 3rd LNA range rise value
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int1 rfnv_lna_range_3_rise[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  int1 rfnv_lna_range_3_rise[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* 3rd LNA range fall value
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int1 rfnv_lna_range_3_fall[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  int1 rfnv_lna_range_3_fall[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* 3rd LNA bypass range offset
  */
  int2 rfnv_lna_range_3_offset[RF_PATHS_TABLE_SIZ];

  /* 4th LNA range rise value
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int1 rfnv_lna_range_4_rise[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  int1 rfnv_lna_range_4_rise[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* 4th LNA range fall value
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int1 rfnv_lna_range_4_fall[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  int1 rfnv_lna_range_4_fall[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* 4th LNA bypass range offset
  */
  int2 rfnv_lna_range_4_offset[RF_PATHS_TABLE_SIZ];

  /* Intermod Level 1
  */
  int1 rfnv_im_level1[RF_PATHS_TABLE_SIZ];

  /* Intermod Level 2
  */
  int1 rfnv_im_level2[RF_PATHS_TABLE_SIZ];

  /* Intermod Level 3
  */
  int1 rfnv_im_level3[RF_PATHS_TABLE_SIZ];

  /* Intermod Level 4
  */
  int1 rfnv_im_level4[RF_PATHS_TABLE_SIZ];

  /* LNA non-bypass hold timer - multiples of 52us
  */
  byte rfnv_nonbypass_timer;

  /* LNA bypass hold timer - multiples of 6.6ms
  */
  byte rfnv_bypass_timer;

  /* 1st LNA bypass range offset vs. freq. compensation
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int2 rfnv_cdma_lna_1_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ][NV_FREQ_TABLE_SIZ];
#else
  int2 rfnv_cdma_lna_1_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* 2nd LNA bypass range offset vs. freq. compensation
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int2 rfnv_cdma_lna_2_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ][NV_FREQ_TABLE_SIZ];
#else
  int2 rfnv_cdma_lna_2_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* 3rd LNA bypass range offset vs. freq. compensation
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int2 rfnv_cdma_lna_3_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ][NV_FREQ_TABLE_SIZ];
#else
  int2 rfnv_cdma_lna_3_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* 4th LNA bypass range offset vs. freq. compensation
  */
#ifdef RF_HAS_POWER_MODE_LIB
  int2 rfnv_cdma_lna_4_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ][NV_FREQ_TABLE_SIZ];
#else
  int2 rfnv_cdma_lna_4_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* Phase rotation setting for LNA Stages 2 to 5
  */
  byte rfnv_agc_phase_offset;

  /* LNA range polarity
  */
  byte rfnv_lna_range_pol;

  /* Mixer range polarity
  */
  byte rfnv_mixer_range_pol;

  /* LNA range delay time from CAGC to RX Front
  */
  byte rfnv_lna_range_delay;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     Miscellaneous other NV items
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* CDMA Tx limiter loop gain, scaled by RF_CDMA_TX_LIM_ADJ_SCALE.
  */
  int2 rfnv_cdma_tx_lim_adj_gain;

  /* CDMA/PCS TX Attenuation Limit */

  /* PA_R1 rise value
  */
  byte rfnv_r1_rise;       /* approximately - 4dBm */

  /* PA_R1 fall value
  */
  byte rfnv_r1_fall;       /* approximately - 8dBm */

  /* PA_R2 rise value
  */
  byte rfnv_r2_rise;       /* disabled             */

  /* PA_R2 fall value
  */
  byte rfnv_r2_fall;       /* disabled             */

  /* PA_R3 rise value
  */
  byte rfnv_r3_rise;       /* approximately +17dBm */

  /* PA_R3 fall value
  */
  byte rfnv_r3_fall;       /* disabled             */


#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
  /* P1 PA Rise/Fall adjustments
  */
  byte rfnv_p1_pa_rise_fall_off[RF_PATHS_TABLE_SIZ][ NV_PA_SP_OFFSET_TABLE_SIZ ];
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */
#ifdef FEATURE_HDR_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
  /* HDR P1 PA Rise/Fall adjustments
  */
  byte rfnv_hdr_p1_pa_rise_fall_off[ NV_HDR_PA_SP_OFFSET_TABLE_SIZ ];
#endif /* FEATURE_HDR_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */

  /* CHIPX8 delay for SYNC80M, biased +25.
     This is the RF path delay from the antenna to the IQ
     digital MSM pins */
  uint32 rfnv_enc_btf_dly;
#ifdef FEATURE_TX_POWER_BACK_OFF
  /* dB backoff for low and medium voltage as a function of
  ** channel configuration.
  ** This table tracks enumeration type rfnv_backoff_chan_config_type
  ** Contents are dB values, scaled 3
  */
  uint8 rfnv_chan_config_backoff_med[ NV_TX_BACKOFF_TABLE_SIZE ] ;
  uint8 rfnv_chan_config_backoff_low[ NV_TX_BACKOFF_TABLE_SIZE ] ;

  // Low, med, and hi volts for PA backoff calibration.
  uint8 rfnv_pa_backoff_cal_volts[ 3 ] ;

  // Volts*10 for upper and lower Vbatt calibration.
  nv_minmax_type rfnv_vbatt_cal_volts ;
#endif
  /* ---------------------- New RF NV Items ------------------------------ */

  /* RxFront Digital items */

#ifdef RF_HAS_POWER_MODE_LIB
  byte    rfnv_grp_delay_adj[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  byte    rfnv_grp_delay_adj[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */
  int2    rfnv_vga_gain_offset[RF_PATHS_TABLE_SIZ];
#ifdef RF_HAS_POWER_MODE_LIB
  int2    rfnv_vga_gain_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ][NV_FREQ_TABLE_SIZ];
#else
  int2    rfnv_vga_gain_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */
  int2    rfnv_vga_gain_offset_vs_temp[RF_PATHS_TABLE_SIZ][NV_TEMP_TABLE_SIZ];
  
#ifdef RF_HAS_POWER_MODE_LIB
  word    rfnv_dacc_iaccum0[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  word    rfnv_dacc_iaccum1[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  word    rfnv_dacc_iaccum2[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  word    rfnv_dacc_iaccum3[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  word    rfnv_dacc_iaccum4[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  word    rfnv_dacc_qaccum0[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  word    rfnv_dacc_qaccum1[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  word    rfnv_dacc_qaccum2[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  word    rfnv_dacc_qaccum3[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  word    rfnv_dacc_qaccum4[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  word    rfnv_dacc_iaccum0[RF_PATHS_TABLE_SIZ];
  word    rfnv_dacc_iaccum1[RF_PATHS_TABLE_SIZ];
  word    rfnv_dacc_iaccum2[RF_PATHS_TABLE_SIZ];
  word    rfnv_dacc_iaccum3[RF_PATHS_TABLE_SIZ];
  word    rfnv_dacc_iaccum4[RF_PATHS_TABLE_SIZ];
  word    rfnv_dacc_qaccum0[RF_PATHS_TABLE_SIZ];
  word    rfnv_dacc_qaccum1[RF_PATHS_TABLE_SIZ];
  word    rfnv_dacc_qaccum2[RF_PATHS_TABLE_SIZ];
  word    rfnv_dacc_qaccum3[RF_PATHS_TABLE_SIZ];
  word    rfnv_dacc_qaccum4[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* RF Chipset */
#ifdef RF_HAS_POWER_MODE_LIB
  byte    rfnv_im2_i_value[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  byte    rfnv_im2_q_value[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
  byte    rfnv_im2_transconductor_value[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
  byte    rfnv_im2_i_value[RF_PATHS_TABLE_SIZ];
  byte    rfnv_im2_q_value[RF_PATHS_TABLE_SIZ];
  byte    rfnv_im2_transconductor_value[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */


#ifndef RF_HAS_RFCONFIG_RX_LNA
  /* OLTA items */
#ifdef RF_HAS_POWER_MODE_LIB
  int1    rfnv_olta_backoff[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ][RF_MAX_LNA_STAGES];
  int1    rfnv_hdr_olta_backoff[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ][RF_MAX_LNA_STAGES];
#else
  int1    rfnv_olta_backoff[RF_PATHS_TABLE_SIZ][RF_MAX_LNA_STAGES];
  int1    rfnv_hdr_olta_backoff[RF_PATHS_TABLE_SIZ][RF_MAX_LNA_STAGES];
#endif /* RF_HAS_POWER_MODE_LIB */
#endif /* !RF_HAS_RFCONFIG_RX_LNA */

  /* Coefficients used in interpolation of Frequency Compensation 
     Values for Rx and Tx 
  */

#ifdef RF_HAS_LINEAR_INTERPOLATION
  word    rfnv_rx_cal_chan[RF_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];
  word    rfnv_tx_cal_chan[RF_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];

  uint8   rfnv_rx_cal_chan_lru[RF_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];
#endif

  /* The GPS Chain Delay cal. parameter. 
  */
  word    rfnv_gps1_rf_delay;

  /* Adjustment factor 
  */
  byte    rfnv_adj_factor;

  /*  VCO Coarse Tune Table. 
  */
#ifdef RF_HAS_POWER_MODE_LIB
  byte    rfnv_vco_coarse_tune_table[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ * NV_VCO_COARSE_TUNE_TABLE_SIZ];
  byte    rfnv_vco_coarse_tune_2[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ * NV_VCO_COARSE_TUNE_TABLE_SIZ];

#else
  byte    rfnv_vco_coarse_tune_table[RF_PATHS_TABLE_SIZ][NV_VCO_COARSE_TUNE_TABLE_SIZ];
  byte    rfnv_vco_coarse_tune_2[RF_PATHS_TABLE_SIZ][NV_VCO_COARSE_TUNE_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  /* 
  Fall setting for HDR IM 
  */
  int1    rfnv_hdr_im_fall;
  
  /*
  Rise settings for HDR IM 
  */
  int1    rfnv_hdr_im_rise;

  /*
  HDR Intermode level
  */
  int1    rfnv_hdr_im_level;

  /* 
  Fall setting for HDR IM G0 
  */
  int1    rfnv_hdr_g0_im_fall;
  
  /*
  Rise settings for HDR IM G0
  */
  int1    rfnv_hdr_g0_im_rise;

  /*
  HDR Intermode level G0
  */
  int1    rfnv_hdr_g0_im_level;

#ifdef RF_HAS_PA_DVS_NV
  /*
  PA DVS Voltage setting (mV)  0 = PA DVS disabled 
  */
  uint16  rfnv_pa_dvs_voltage;
#endif
  
  /* IntelliCeiver calibration as returned from the
     ftm_1x_cal_intelliceiver API */
  uint8   rfnv_intelliceiver_cal[RF_PATHS_TABLE_SIZ][NV_INTELLICEIVER_CAL_LENGTH];

  /* 
  Relative Antenna Quality
  */
  uint16  rfnv_ant_quality[NV_MAX_ANT_NO_SIZ][NV_FREQ_TABLE_SIZ];
  
  /* IntelliCeiver detector threshold adjustments in 1/10th dB increments.
     The threshold can be adjusted +2dB to -10dB for each gain state */
  int8    rfnv_intelliceiver_detector_adjust[RF_PATHS_TABLE_SIZ][NV_INTELLICEIVER_DETECTOR_COUNT][5];

  /*
  1x PA Hysteresis timer
  */
  uint8   rfnv_1x_pa_r_timer;

  /*
  1xEV-DO PA Hysteresis timer
  */
  uint8   rfnv_1xEV_DO_pa_r_timer;

  #ifdef RF_HAS_RFCONFIG_RX_LNA
     #ifdef RF_HAS_MULTI_RX_CHAIN
  rfconfig_rx_lna_type   rfconfig_rx_lna_info[RF_PATHS_TABLE_SIZ];
     #else /* RF_HAS_MULTI_RX_CHAIN */     
  rfconfig_rx_lna_type   rfconfig_rx_lna_info[1];  // just one Rx chain
     #endif /* RF_HAS_MULTI_RX_CHAIN */
  #endif /* RF_HAS_RFCONFIG_RX_LNA */

#ifdef RF_HAS_HW_DZRF6285
#error code not present
#endif

  /* Band Class specific PA state mapping value (optional) */
  rfnv_bc_pa_r_map_type    rfnv_bc_pa_r_map;
  
  #ifdef RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR
  rfnv_temp_freq_based_tx_pwr_comp_type rfnv_tx_power_comp[RF_NUM_TX_LINEARIZERS];
  #endif

#ifdef RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
  /* P2 PA Rise/Fall adjustments
  */
  byte rfnv_p2_pa_rise_fall_off[RF_PATHS_TABLE_SIZ][ NV_PA_SP_OFFSET_TABLE_SIZ ];

  /* P3 PA Rise/Fall adjustments
  */
  byte rfnv_p3_pa_rise_fall_off[RF_PATHS_TABLE_SIZ][ NV_PA_SP_OFFSET_TABLE_SIZ ];
#endif /* RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST */

#ifdef RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
#error code not present
#endif /* RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST */

#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
  /* LO Cal Items.*/
  nv_lo_cal_type rfnv_lo_cal[ RF_PATHS_TABLE_SIZ ];
#endif //RF_HAS_SUPPORT_FOR_LO_PWC_CAL

#ifdef RF_HAS_SUPPORT_FOR_LO_IM2_CAL
  /* IM2 Cal Items.*/
  nv_im2_cal_type rfnv_im2_cal[ RF_PATHS_TABLE_SIZ ];
#endif //RF_HAS_SUPPORT_FOR_LO_IM2_CAL
#ifdef RF_HAS_SUB_BLOCK_SUPPORT
  uint32 rfnv_block_mask;
  struct rfnv_band_block_digital_items * block_item_ptr;
#endif

  /* ---------------------- End of new RF NV Items ----------------------- */

} rfnv_digital_items_type;


/* -----------------------------------------------------------------------
** Common NV calibration tables
** ----------------------------------------------------------------------- */
typedef struct {

  /* General RF Items */

  byte    rfnv_test_code_ver[NV_SIZE_OF_VERSION];
  byte    rfnv_sys_sw_ver[NV_SIZE_OF_VERSION];
  byte    rfnv_rf_cal_ver[NV_SIZE_OF_VERSION];
  byte    rfnv_rf_config_ver[NV_SIZE_OF_VERSION];
  dword   rfnv_rf_cal_date;
  dword   rfnv_rf_nv_loaded_date;
  byte    rfnv_rf_cal_dat_file[NV_QC_SERIAL_NUM_LEN];

  /* RX Front items */
  dword   rfnv_dacc_dc_offset_polarity;

  /* RF Chipset items */
  byte    rfnv_rtc_time_adjust;
  byte    rfnv_digital_pll_lock_timer;

  /* CAGC dynamic range items */
  int2    rfnv_cdma_min_rx_rssi;
  int2    rfnv_cdma_dynamic_range;
  
  /* 
  Tunes the RX baseband filter bandwidth 
  */
  uint8   rfnv_rfr_bb_filter[RF_PATHS_TABLE_SIZ];

  /* Tunes the value of the baseband resistor */
  uint8   rfnv_rfr_iq_line_resistor;

  /* PA state mapping value (common default) */
  byte    rfnv_pa_r_map;

  /* GPS RF config value */
  byte    rfnv_gps_rf_config;

#ifdef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
  /*8 bins Calibration for Thermistor*/
  uint16 rfnv_therm_bin_type[NV_TEMP_TABLE_SIZ];
#endif

  /* PMIC configuration setting for RF */
  uint8 rf_pm_config;

#ifdef RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL
  /* Process cal */
  nv_process_cal_type rfnv_process_cal;
#endif //RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL

#ifdef RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL
  /* DC Cal */
  nv_dc_cal_type rfnv_dc_cal;
#endif //RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL

#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
  /* Supply Value */
  uint16 rfnv_supply_val;
#endif //RF_HAS_SUPPORT_FOR_LO_PWC_CAL

} rfnv_common_items_type;

/* -----------------------------------------------------------------------
** FM NV calibration tables
** ----------------------------------------------------------------------- */
typedef struct {

  /* FM mode TX_AGC_ADJ initial setting vs power table
  */
  int2 rfnv_fm_agc_set_vs_pwr[NV_PWR_TABLE_SIZ];

  /* FM mode TX_AGC_ADJ initial setting vs frequency compensation table
  */
  int1 rfnv_fm_agc_set_vs_freq[NV_FREQ_TABLE_SIZ];

  /* FM mode TX_AGC_ADJ initial setting vs temperature compensation table
  */
  int2 rfnv_fm_agc_set_vs_temp[NV_TEMP_TABLE_SIZ];

  /* FM AGC adjust vs frequency table
  */
  int1 rfnv_fm_agc_adj_vs_freq[NV_FREQ_TABLE_SIZ];

  /* FM expected HDET vs temperature table
  */
  byte rfnv_fm_exp_hdet_vs_temp[NV_TEMP_TABLE_SIZ];

  /* FM expected HDET vs power level table
  */
  byte rfnv_fm_exp_hdet_vs_pwr[NV_PWR_TABLE_SIZ];

  /* FM error slope vs power level table
  */
  byte rfnv_fm_err_slp_vs_pwr[NV_PWR_TABLE_SIZ];

  /* FM AGC underflow limit
  */
  byte rfnv_dfm_agc_min;

  /* FM AGC overflow limit
  */
  byte rfnv_dfm_agc_max;

  /* FM Frequency-Sense-Gain (Tx Gain)
  */
  byte rfnv_fm_freq_sense_gain;

  /* FM RSSI offset
  */
  word rfnv_fm_rssi_raw_offset;

  /* FM RSSI span
  */
  word rfnv_fm_rssi_raw_span;

  /* New DFM Block */
  int1    rfnv_lna_fall;
  int2    rfnv_lna_offset;
  int1    rfnv_lna_rise;
  int1    rfnv_lna_offset_vs_freq[NV_FREQ_TABLE_SIZ];
  word    rfnv_lna_bypass_timer;
  word    rfnv_lna_nonbypass_timer;
  byte    rfnv_lna_follower_delay;
  word    rfnv_lna_range_delay;
  byte    rfnv_lna_decision_delay;
  byte    rfnv_lna_polarity;
  byte    rfnv_agc_acc_min_1;
  byte    rfnv_agc_im_gain;
  byte    rfnv_agc_dc_gain;
  int1    rfnv_im_level1;
  byte    rfnv_fm_pa_mac_high;

  /* New Rx Front items */
  int2    rfnv_vga_gain_offset;
  int1    rfnv_vga_gain_offset_vs_freq[NV_FREQ_TABLE_SIZ];
  int2    rfnv_vga_gain_offset_vs_temp[NV_TEMP_TABLE_SIZ];
  word    rfnv_phase_del;
  word    rfnv_dacc_iaccum0;
  word    rfnv_dacc_iaccum1;
  word    rfnv_dacc_iaccum2;
  word    rfnv_dacc_iaccum3;
  word    rfnv_dacc_iaccum4;
  word    rfnv_dacc_qaccum0;
  word    rfnv_dacc_qaccum1;
  word    rfnv_dacc_qaccum2;
  word    rfnv_dacc_qaccum3;
  word    rfnv_dacc_qaccum4;

} rfnv_fm_items_type;
/* -----------------------------------------------------------------------
** NV data
** ----------------------------------------------------------------------- */
typedef struct {
  boolean rfnv_rfcal_valid;                        /* RF CAL Valid?         */
  
  rfnv_common_items_type  *rfnv_common_items_ptr;  /* Ptr to Common NV itmes*/

  rfnv_digital_items_type *rfnv_cdma_items_ptr;    /* Ptr to BC0 NV items  */

#if defined (RF_HAS_BC1_NV_TABLE) || defined (FEATURE_TRIMODE_ITEMS)
  rfnv_digital_items_type *rfnv_pcs_items_ptr;     /* Ptr to BC1 NV items   */
#endif
  
#ifdef RF_HAS_BC2_NV_TABLE
  rfnv_digital_items_type *rfnv_bc2_items_ptr;     /* Ptr to BC2 NV items   */
#endif

#ifdef RF_HAS_BC3_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC4_NV_TABLE
  rfnv_digital_items_type *rfnv_bc4_items_ptr;     /* Ptr to BC4 NV items   */
#endif

#ifdef RF_HAS_BC5_NV_TABLE
  rfnv_digital_items_type *rfnv_bc5_items_ptr;     /* Ptr to BC5 NV items   */
#endif

#ifdef RF_HAS_BC6_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC11_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC14_NV_TABLE
  rfnv_digital_items_type *rfnv_bc14_items_ptr;    /* Ptr to BC14 NV items  */
#endif

#ifdef RF_HAS_BC15_NV_TABLE
  rfnv_digital_items_type *rfnv_bc15_items_ptr;    /* Ptr to BC15 NV items  */
#endif

#ifdef RF_HAS_GPS_NV_TABLE
#error code not present
#endif /*RF_HAS_GPS*/
  
  rfnv_fm_items_type      *rfnv_fm_items_ptr;      /* Ptr to FM NV items    */

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT

  #ifdef RF_HAS_BCX_BLOCK_0_NV
  rfnv_digital_items_type *rfnv_bcx_block_0_items_ptr;
  #endif

  #ifdef RF_HAS_BCX_BLOCK_1_NV
  rfnv_digital_items_type *rfnv_bcx_block_1_items_ptr;
  #endif

  #endif

} rfnv_all_data_type;


/*===========================================================================

FUNCTION RFNV_CDMA1X_COMMON_COPY_NV_ITEM

DESCRIPTION
  This function updates the specfied item (item_code) in the rfnv_gsm_cmn_nv_tbl
  with the data supplied via the item_ptr.  This function used to be called
  rfnv_read_nv_item().

DEPENDENCIES
  Note that there must NOT be a dependency on the read order of the NV
  calibration items.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfnv_cdma1x_copy_common_nv_items
(
  nv_items_enum_type item_code,
  nv_item_type *item_ptr
);

/*===========================================================================

FUNCTION RFNV_CDMA1X_COMMON_LIST_SIZE                   EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
word rfnv_cdma1x_common_list_size( void );
/*===========================================================================

FUNCTION RFNV_CDMA1X_COMMON_ITEM_TABLE                  EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns a void pointer to the common nv item table.

DEPENDENCIES
  None

RETURN VALUE
  void pointer to the common nv item table.

SIDE EFFECTS
  None

===========================================================================*/
void *rfnv_cdma1x_common_item_table(void);


/*===========================================================================

    L E G A C Y  F U N C T I O N S

 The following are the legacy functions that need to be supported until the
 RF code is changed to utilize the band specific nv driver api.

===========================================================================*/
/*===========================================================================

FUNCTION RFNV_COMMON_TABLE                                 REGIONAL FUNCTION

DESCRIPTION
  Returns pointer to common table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to common table.

SIDE EFFECTS
  None

===========================================================================*/
rfnv_common_items_type *rfnv_common_table( void);

/*===========================================================================

FUNCTION RFNV_FM_TABLE                                      REGIONAL FUNCTION

DESCRIPTION
  Returns pointer to FM table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to FM table.

SIDE EFFECTS
  None

===========================================================================*/
//#ifdef RF_HAS_RF
rfnv_fm_items_type *rfnv_fm_table( void);
//#endif

/*===========================================================================

FUNCTION RFNV_DIGITAL_TABLE                                 REGIONAL FUNCTION

DESCRIPTION
  Returns pointer to digital table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to digital table.

SIDE EFFECTS
  None

===========================================================================*/
rfnv_digital_items_type *rfnv_digital_table( void);

/*===========================================================================

FUNCTION RFNV_GPS_TABLE                                 REGIONAL FUNCTION

DESCRIPTION
  Returns pointer to GPS table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GPS table.

SIDE EFFECTS
  None

===========================================================================*/
#ifdef RF_HAS_GPS_NV_TABLE
#error code not present
#endif

/*===========================================================================
FUNCTION RFNV_GET_NV_DATA                               EXTERNALIZED FUNCTION

DESCRIPTION
  This function fills the structure pointed to by the pass-in parameter with
  NV items and pointers to calibration tables.  A NULL pointer indicates that
  the specific calibration table is unavailable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_get_nv_data
(
  rfnv_all_data_type *data_ptr   /* Pointer to the NV data structure */
);

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
/*===========================================================================

FUNCTION RFNV_SET_DIGITAL_TABLE                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the default digital table pointer to the specified mode.
  This default pointer is returned by rfnv_digital_table().

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_set_digital_table(rfcom_device_enum_type device, rfnv_1x_band_class_type band_class, word rf_chan);

#else
/*===========================================================================

FUNCTION RFNV_SET_DIGITAL_TABLE                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the default digital table pointer to the specified mode.
  This default pointer is returned by rfnv_digital_table().

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_set_digital_table(rfnv_1x_band_class_type);
#endif
/*=================================================================================

FUNCTION    RFNV_SET_NV_DEFAULTS

DESCRIPTION
  This function sets the default values for selected items in band class nv tables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rfnv_set_defaults
(
    void *nv_table,
    byte rfhw,
    byte band
);

/*===========================================================================

FUNCTION RFNV_LOAD_DVGA_OFFSET_DATA                         INTERNAL FUNCTION

DESCRIPTION
  This function copies DVGA offset vs frequency data from an NV
  structure into the internal RF driver NV structures.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_load_dvga_offset_data( PACKED int1 nv_offsets[],
                                 int2 *flat_rfnv_table_ptr,
                                 int2 base_offset, int count );


/*===========================================================================

FUNCTION RFNV_LOAD_LNA_OFFSET_DATA                          INTERNAL FUNCTION

DESCRIPTION
  This function copies LNA offset vs frequency data from an NV
  structure into the internal RF driver NV structures.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_load_lna_offset_data( PACKED int1 nv_offsets[],
                                int2 *flat_rfnv_table_ptr,
                                int2 base_offset, int count );

/*===========================================================================

FUNCTION RFNV_GET_BAND_MASK                                   INTERNAL FUNCTION

DESCRIPTION
  Returns enabled/disabled state of bands set in NV item, NV_RF_HW_BC_CONFIG_I.

DEPENDENCIES
  None

RETURN VALUE
 sys_band_mask_type containing enabled/disabled states of bands. 

SIDE EFFECTS
  None
===========================================================================*/
sys_band_mask_type rfnv_get_band_mask(void);
/*===========================================================================

FUNCTION RFNV_SET_CARD_DEFAULTS                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the card specific defaults for all the bands.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_set_card_defaults(rf_hw_type hw);

/*===========================================================================

FUNCTION RFNV_VALIDATE_1X_RF_CARD                       EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns whether the RF card supports 1x. 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rfnv_validate_1x_rf_card(void);

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
/*===========================================================================

FUNCTION RFNV_GET_BLOCK_NV_PTR                    INTERNAL FUNCTION

DESCRIPTION
Sets the pointer to the Block NV Table of the corresponding Block
Passed as Parameter.

DEPENDENCIES
Block Mask Should have been appropriately configured and 
this function should be called after rfnv_retrieve_nv_items

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
rfnv_digital_items_type* rfnv_get_block_nv_ptr(rf_card_blocks_type block);

/*===========================================================================

FUNCTION RFNV_GET_BAND_NV_PTR                    Internal FUNCTION

DESCRIPTION

Returns the pointer to the NV item table of the Band Class passed as
Parameter. If the Band Class Passed is not supported then
the NV Pointer returned is nULL

DEPENDENCIES
  
RETURN VALUE
  Returns the Pointer to the corresponding Band Class NV

SIDE EFFECTS
  None

===========================================================================*/
rfnv_digital_items_type* rfnv_get_band_nv_ptr(rf_card_band_type band);

/*===========================================================================

FUNCTION RFNV_UPDATE_BAND_DIGITAL_ITEM_PTR               INTERNAL FUNCTION

DESCRIPTION
Updates the rf_digital_nv_table[band] where band is the parameter
passed to the function.
Also updates the rf_bcx_item_table_ptr where "x" represents the Band Class
to point to correct NV Table to be used.

DEPENDENCIES
None

RETURN VALUE


SIDE EFFECTS
Updates the NV Table Pointers and then these will be used all through.  

===========================================================================*/
void rfnv_update_band_digital_items_ptr(rfcom_device_enum_type device,rf_card_band_type band, rfnv_digital_items_type *digital_item_ptr);
#endif
#endif /* RFNV_CDMA1X_H */


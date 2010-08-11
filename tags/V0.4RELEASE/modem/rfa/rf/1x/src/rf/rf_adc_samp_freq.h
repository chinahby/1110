#ifndef RF_ADC_SAMP_FREQ_H
#define RF_ADC_SAMP_FREQ_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     HDR MAXIMUM TRANSMIT POWER BACKOFF
                             HEADER FILE
  
Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_adc_samp_freq.h#1 $
when       who     what, where, why
--------   ---    ----------------------------------------------------------
03/05/08   vm      Clean up for QSC1100 Napolean and RICO Bringup
08/05/07   vm     Added more support for Group Delay Adjust for Sc2X
06/15/07   vm     Added the support for GPS 
05/14/07   vm     Added the API support for programming the Group delay 
                  based on Sampling frequency                   
05/10/07   bmg    Updated digital filter coefficients from Tom and Soon
04/18/07   vm     Added support for ADC and modem clock switching
                  for all the different modes of operation.
04/03/07   vm     Added the support for Notch filters for 1x and diversity
02/20/07   vm     Added the support for ADC clocking scheme for SGPS
12/08/06   vm     Added the support for resampler in RX Front
11/26/06   vm     Created 
===========================================================================*/

#include "comdef.h"
#include "rficap.h"
#include "nv_items.h"
#include "rflib_share.h"
#include "rfifreq.h"

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO

#define MAX_NUM_SAMP_FREQ_BINS_PER_BAND_CLASS 3

#define MAX_NUM_BAND_CLASS_SEGMENTS 2

#ifdef RF_HAS_POWER_MODE_LIB
#define RF_NUM_OF_POWER_MODES   2/* For time being Finally =2*///NV_RF_POWER_MODE_SIZ
#else
#define RF_NUM_OF_POWER_MODES   1
#endif

typedef struct 
{
  int8 r_h_val;
  int8 r_l_val;
  int8 i_h_val;
  int8 i_l_val;
}rf_notch_filter_coeffs_type;

typedef struct 
{
  double bin_freq[RF_NUM_OF_POWER_MODES][MAX_NUM_SAMP_FREQ_BINS_PER_BAND_CLASS+1];

  uint32 dig_filter_coeff0[RF_NUM_OF_POWER_MODES][MAX_NUM_SAMP_FREQ_BINS_PER_BAND_CLASS];

  uint32 dig_filter_coeff1[RF_NUM_OF_POWER_MODES][MAX_NUM_SAMP_FREQ_BINS_PER_BAND_CLASS];

  byte   dig_filter_gain[RF_NUM_OF_POWER_MODES][MAX_NUM_SAMP_FREQ_BINS_PER_BAND_CLASS];

}rf_adc_samp_freq_bins_type;

typedef struct 
{
  uint16 delta_offset[RF_PATH_MAX];

  double a_val[MAX_NUM_SAMP_FREQ_BINS_PER_BAND_CLASS];  

  double b_val[MAX_NUM_SAMP_FREQ_BINS_PER_BAND_CLASS];  

}rf_adc_grp_dly_adj_params_type;


typedef enum
{
    CDMA_MODE,
    HDR_MODE,
    GPS_MODE
 } rf_curr_sys_mode_type ;

 typedef enum
 {
   VCO_OFF,
   VCO_WARMUP,
   VCO_ON
 }rf_chain_vco_state_type;

 typedef enum
 {
   DIV_OFF,
   DIV_ON
 }rf_chain_div_flag_type;

typedef struct {
  byte samp_freq_bin[2];
  rf_chain_div_flag_type drx_div_flag;
  rf_chain_vco_state_type vco_state[2];
  rf_curr_sys_mode_type mode[2];
  double samp_freq[2];
  boolean gps_on_flag;
} rf_adc_clk_state_type;


/*===========================================================================

FUNCTION RFLIB_SET_BB_FILTER_COEFS                    

DESCRIPTION
  This function is used to program the Baseband filter coefficients for 
  MSM's supporting ADC Sampling clock derived from VCO

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void rflib_set_bb_filter_coefs(byte bin,rf_path_enum_type rf_path,rf_curr_sys_mode_type mode,rf_card_band_type band,uint16 chan,rflib_power_mode_type power_mode);

/*===========================================================================

FUNCTION RFLIB_GET_DIGITAL_FILTER_GAIN                    

DESCRIPTION
  This function is used to return the Baseband filter Gain for 
  MSM's supporting ADC Sampling clock derived from VCO

DEPENDENCIES
  None

RETURN VALUE
  Digital filter gain in 1/10th of db units
  
SIDE EFFECTS
  None

===========================================================================*/
byte rflib_get_digital_filter_gain(byte bin,rf_path_enum_type rf_path,rf_curr_sys_mode_type mode,rf_card_band_type band, 
                                   rflib_power_mode_type power_mode);


/*===========================================================================

FUNCTION RF_GET_ADC_SAMP_FREQ                    

DESCRIPTION
  This function returns the ADC Sampling frequency based on Band,channel
  and Mode to be HDR or 1X

DEPENDENCIES
  None

RETURN VALUE
Type:double
ADC Sampling frequency
  
SIDE EFFECTS
  None

===========================================================================*/
double rflib_get_adc_samp_freq(rf_path_enum_type path,rf_curr_sys_mode_type mode0,rf_card_band_type band0,uint16 chan0);

/*===========================================================================

FUNCTION RF_GET_SAMP_FREQ_BIN                  

DESCRIPTION
  This function returns the bin in which the ADC Sampling frquency lies 
  based on ADC Sampling frequency,Linearity Mode and Rf Mode to be 
  1X or HDR

DEPENDENCIES
  None

RETURN VALUE
Type:byte
Bin in which ADC Sampling frequency lies
  
SIDE EFFECTS
  None

===========================================================================*/
byte rf_get_samp_freq_bin(rf_path_enum_type rf_path,rf_curr_sys_mode_type mode,rf_card_band_type band,
                          double samp_freq,
                          rflib_power_mode_type power_mode);

/*===========================================================================

FUNCTION RFLIB_SET_RESAMPLER_PHASE_MAP_COEFS                  

DESCRIPTION
  This function is used to program the resampler Phase Map coefficients

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void rflib_set_resampler_phase_map_coefs(rf_path_enum_type path,double samp_freq);

/*===========================================================================

FUNCTION RFLIB_SET_RF_CHAIN_GRP_DELAY                  

DESCRIPTION
  This function is used to program the Group Delay for RF Chain
  depending upon the Sampling frequency

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void  rflib_set_rf_chain_grp_delay(rf_path_enum_type path,double samp_freq,byte bin);

/*===========================================================================
FUNCTION RF_SET_PRIM_CHAIN_GPS_OFF_TO_ON                 EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever GPS session starts on the
  second chain
  
DEPENDENCIES
  Must be called by GPS RF driver after the GPS VCO is locked and 
  second chain clock generator and ADC are turned ON.
  
  prx_on_flag must have been set to right value before 
  calling this function

RETURN VALUE
None

SIDE EFFECTS
Sets the Sampling clock source to be FB_ANT1_CLK for the Rx Front

Change Sampling clock for ADC0/1 to be from the second chain VCO /N

Reprogram the Digital filter coefficients,Resampler Phase map coefficients
and Notch filter coefficients on primary chain

Sets the gps_on_flag to TRUE


===========================================================================*/

void rf_set_prim_chain_gps_off_to_on(void);

/*===========================================================================

FUNCTION RF_SET_PRIM_CHAIN_GPS_ON_TO_OFF                 EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever GPS session ends on the
  second chain
  
DEPENDENCIES
  Must be called by GPS RF driver before secondary RF Chain is turned oFF
  implying before turning OFF Secondary ADC and Seconadry VCO 
  
  prx_on_flag must have been set to right value before 
  calling this function

RETURN VALUE
None

SIDE EFFECTS
Sets the Sampling clock source to be FB_ANT0_CLK for the Rx Front if 
Primary chain is ON

Change Sampling clock for ADC0 to be from the Primary chain VCO /N
if primary chain is awake

Reprogram the Digital filter coefficients,Resampler Phase map coefficients
and Notch filter coefficients on primary chain

Sets the gps_on_flag to FALSE


===========================================================================*/

void rf_set_prim_chain_gps_on_to_off(void);

/*===========================================================================

FUNCTION RF_SET_PRIM_CHAIN_LIN_MODE_CHANGE                 EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever the Linearity mode change occurs
  on Primary chain

DEPENDENCIES
  gps_on_flag must have been set to right value before 
  calling this function

RETURN VALUE
None

SIDE EFFECTS
May change the Sampling clock source for Harrier

May Change Sampling clock for ADC

Reprogram the Digital filter coefficients,Resampler Phase map coefficients
and Notch filter coefficients on primary chain

Sets the prx_on_flag to TRUE



===========================================================================*/
void rf_set_prim_chain_lin_mode_change(rf_path_enum_type path,
                                       rf_curr_sys_mode_type mode,
                                       rf_card_band_type band,word chan,
                                       rf_curr_sys_mode_type mode1,
                                       rf_card_band_type band1,word chan1,
                                       rflib_power_mode_type power_mode);


/*===========================================================================

FUNCTION RF_1X_CHAIN_SWITCH_ADC_CLK_SET_RXF            EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever primary/secondary chain state changes.
  This function is also Called whenever Diversity is enabled with the 
  Primary chain VCO being already ON/Locked.
  
  This is a master function Called which would Call other related functions 
  depending upon Path, current VCO State for both the Chains and the status
  of Diversity being enabled or not

DEPENDENCIES
None
  
RETURN VALUE
None

SIDE EFFECTS
-It changes the state VCO state of not only the Chain for which its is called
 but might also change the VCO State of another chain

- It also sets the coreponding diversity flag to be ON depending upon the 
VCO State of both Primary and secondary chain

-All the functions Called form within this function will be executed in
context of critical sections
 
===========================================================================*/
void rf_1x_chain_switch_adc_clk_set_rxf(rf_path_enum_type path,
                                        rf_chain_vco_state_type new_state,                                        
                                        rf_chain_div_flag_type div_flag,
                                        rf_curr_sys_mode_type mode);


#endif/*RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO*/

#endif/*RF_ADC_SAMP_FREQ_H*/

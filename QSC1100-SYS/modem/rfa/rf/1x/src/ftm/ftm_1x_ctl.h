/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM 1x CTL

GENERAL DESCRIPTION
  This is the header file for the embedded FTM RF Commands 

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2006, 2007       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_1x_ctl.h#6 $ 
  $DateTime: 2008/10/01 18:07:35 $ 
  $Author: vboda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
05/05/08   vb      Changed the enumeration for Primary Chain state control API
04/18/08   vb      Added prototype for reading Beta Prime value and putting the Primary Chain to Sleep
03/21/08   sl      Added ftm_do_internal_device_calibration.
11/09/07   ans     Added protoptype of the function.
10/03/07   bmg     Fixed ftm_set_secondary_chain prototype for RVCT funniness
06/14/07   dyc     Added ftm_set_tx_xo_cal()
06/13/07   anr     Added ftm_do_xo_dc_cal() and ftm_do_xo_ft_curve_cal() prototypes 
                   History entry updated to reflect actual date release. (dyc)
05/02/07   dyc     Added ftm_set_tx_sweep_step_size() prototype.
02/26/07   vm      Merge for Common archive
04/28/06   vm      Changed the prototype of function ftm_do_tx_rx_freq_cal_sweep
                   to support new RXTX Cal 
10/25/05   vm      Added the function ftm_synth_is_locked() for checking 
                   PLL lock status for synth device
09/12/05   hkk     Changes for TX/RX sweep cal - added prototype for new function
05/31/05   sar     Fixed band parameter type in prototypes ftm_set_digital_band
                   and ftm_set_cdma_band.
05/30/05   ra      Added support for timer services
03/20/05   bmg     Added ftm_set_rf_power_mode API
10/31/04   ra      Added ftm_configure_tx_sweep_cal prototype
06/02/04   fpe     Changed support for FEATURE_ENHANCED_BAND_CLASS
05/27/04   ra      Added support for FEATURE_ENHANCED_BAND_CLASS
05/27/04   bmg     Added closed-loop HDET tracking API
05/18/04   rsr     Appended ftm_ to mode_id_type and pdm_id_type enums
04/26/04   bmg     Removed unused DFM function ftm_set_freq_sense_gain()
03/09/04   ra      renamed FEATURE_FTM_BYPASS_NV_CAL_POINTS to FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
                   removed obsolete FTM_USES_ABSOLUTE_LNA_CAL
12/01/03   bmg     Cleaned up include files
11/21/03   bmg     Extended second chain call API to support extra modes
11/14/03   bmg     Moved second chain call helper into 1x commands
10/29/03   bmg     Added ftm_set_cdma_band() prototype
10/20/03   bmg     Added helper prototypes for HDR RF mode
10/08/03   bmg     Added experimental feature for passed in AGC calibration
09/08/03   ra      Renamed enc_stuff_zero_data_isr to ftm_enc_stuff_zero_data_isr
09/08/03   ra      Renamed GetHdet function to ftm_get_hdet_from_tx_cal_sweep.
                   The new name reflects the binding of the stored HDET values taken during
                   TX CAL SWEEP. If the user wishes to read HDET, the FTM_GET_ADC_VAL api
                   should be used  
09/05/03   bmg     Removed unused FM code.
08/07/03   ra      added ftm set secondary chain mode support
07/24/03   bmg     Added rfcom.h header, for new multi-chain parameter passing
07/23/03   bmg     Added calling support for multi-chain RF driver
                   Converted test_* names to ftm_* names
02/07/03   wd      Rename file from ftm_rf_1x_ctl.h -> ftm_1x_ctl.h
01/24/03   wd      Remove obsolete functions (per ra changes).
12/11/02   ra      mainlined CDMA1X/AMPS 
07-16-02   rjr     removed #defines so 102dB range won't be confused.
06-10-02   rjr     add fm IQ mismatch.
03-21-02   rjr     clean up comments
03-12-02   rjr     added FTM RF proto types
11-12-01   aks     Merged in changes.
10-05-01   aks     Created by raguinag.

===========================================================================*/

#ifndef FTM_1X_CTL_H
#define FTM_1X_CTL_H

#include "customer.h"
#include "target.h"
#include "rfifreq.h"
#include "rfcom.h"
#include "synth.h"
#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm_task.h"
#include "ftm.h"

#if defined (FTM_HAS_XO_FACTORY_CAL) 
#include "tcxomgr_cal.h"
#include "ftm_common_dispatch.h"
#endif /* FTM_HAS_XO_FACTORY_CAL */

typedef struct
{
 timer_type    timer;
}ftmclk_struct_type;

extern int2  rf_pa_range_vs_temp_now;
extern word rf_channel;

#define RF_CODEC_SYNC_WAIT 150
#define DMOD_TRK_MICRO_OVR_M 0x80 // when set, the micro in AGC_TRK_LO_ADJ is passed to the AGC
#define DMOD_TRK_MICRO_OVR_EN_V 0x80


typedef enum {
  FTM_RX_LNA_OFFSET0,
  FTM_RX_LNA_OFFSET1,
  FTM_RX_LNA_OFFSET2,
  FTM_RX_LNA_OFFSET3,
  FTM_RX_LNA_OFFSET4,
  FTM_RX_LNA_OFFSET5,
  FTM_RX_DVGA_OFFSET = 100,
  FTM_RX_NO_ACTION = 0xFF
} ftm_rx_action_type;

typedef enum {
    PRIMARY_CHAIN_SLEEP,
    PRIMARY_CHAIN_ON    
}ftm_primary_chain_state;

//PROTOTYPES

void ftm_Set_Dvga_Gain_Offset(rfcom_device_enum_type device, int2 dvgaGainOffset);

void ftm_tx_sweep_cal( boolean enable);
void ftm_cdma_set_tx_agc(int2);
void ftm_fm_set_tx_agc(int2);
int2 ftm_get_fm_rx_agc (void);
void ftm_init_qdsp_dfm(void);

void ftm_set_pa_r1_r0(byte);

void ftm_set_lna_range(rfcom_device_enum_type device, byte);
void ftm_set_lna_offset(rfcom_device_enum_type device, byte , int2 );

void ftm_set_digital_band(rfcom_device_enum_type device, rf_card_band_type band);
void ftm_set_pdm(ftm_pdm_id_type pdm_id, int2 val);
void ftm_set_tx(rfcom_device_enum_type device, boolean state);
void ftm_set_cdma_cw(boolean on_off);
void ftm_enc_stuff_zero_data_isr(void);
void ftm_cdma_pilot_waveform(boolean on_off);
void ftm_set_rf_mode(rfcom_device_enum_type device, ftm_mode_id_type mode);
void ftm_set_pdm_close(ftm_pdm_id_type pdm_id);
int2 ftm_get_rf_cdma_rx_agc(rfcom_device_enum_type device);

void ftm_init_qdsp_cdma(void);
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
int2 ftm_get_dvga_offset( rfcom_device_enum_type device, int2 agc_value );
word ftm_get_lna_offset( rfcom_device_enum_type device, byte index, int2 agc_value );
#else
int2 ftm_get_dvga_offset( rfcom_device_enum_type device );
word ftm_get_lna_offset( rfcom_device_enum_type device, byte index );
#endif
void test_enc_init(void);
word ftm_get_hdet_from_tx_cal_sweep(word );

void ftm_cdma_set_trk_lo(int set_level);
void ftm_set_secondary_chain(ftm_secondary_chain_control_enum_type state);

void ftm_set_rf_primary_chain_state(ftm_primary_chain_state);
void ftm_set_beta_prime_scaling(int2,word);

void ftm_switch_mode_1xEV_DO(rfcom_device_enum_type device);
void ftm_set_cdma_band(rfcom_device_enum_type device, rf_card_band_type band);
void ftm_tune_to_chan(rfcom_device_enum_type device, word rf_channel);
void ftm_1xEV_DO_tune_cb(void);

/*===========================================================================

FUNCTION       FTM_RX_ON_SECOND_CHAIN
DESCRIPTION
  This function configures the MSM muxes for demodulating the signal
  coming from the second RX chain.  Open loop power control is set by
  the calibration values loaded into the secondary receiver.
===========================================================================*/
void ftm_rx_on_second_chain(byte mode);
/*===========================================================================

FUNCTION       FTM_SET_HDET_TRACKING

DESCRIPTION
  Enables and disables hdet tracking.  If state is non-zero, the hdet
  tracking loop is turned on, and controlled to the value specified
  in set_point.  If state is zero, hdet tracking is disabled.
  
DEPENDENCIES
  Transmitter must be enabled.

RETURN VALUE
  None

SIDE EFFECTS
  Reconfigures the 1.25ms ISR for FTM use
===========================================================================*/
void ftm_set_hdet_tracking( boolean state, unsigned short set_point );

/*===========================================================================

FUNCTION       FTM_HDET_TRACKING_ISR

DESCRIPTION
  1.25ms ISR callback function for closed-loop tracking of HDET.
  Adjusts TX_AGC_ADJ PDM to control HDET to the desired set point.
===========================================================================*/
void ftm_hdet_tracking_isr( void );

typedef struct
{
  unsigned int hdet_accum;        // 16Q8
  unsigned int hdet_set_point;    // 8Q0
  unsigned int hdet_tracking_pdm; // 32Q23
} hdet_tracking_params_type;
/*===========================================================================

FUNCTION ftm_configure_tx_sweep_cal

DESCRIPTION
  This function allows the user to set the length of each step in the tx cal 
  sweep and the number of HDET readings averaged per step.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_configure_tx_sweep_cal(uint16 interval, byte num_avg);
/*===========================================================================

FUNCTION FTM_TX_SWEEP_CAL_ISR

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ftm_tx_sweep_cal_isr(void);

/*=========================================================================

FUNCTION FTM_SYNTH_IS_LOCKED

DESCRIPTION
   This API function is called for checking the status 
   of synth lock for specific synth device.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean
  Retrurns TRUE if synth is locked else return FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean ftm_synth_is_locked( rfcom_device_enum_type device,synth_pll_for_synth_check pll_device );


#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
/*===========================================================================

FUNCTION FTM_GET_DIGITAL_FILTER_GAIN                                  

DESCRIPTION
  This function returns the Gain of digital filter 

DEPENDENCIES
 Band, Channel and Linearity mode of the receiver must have been 
 configured before for the given path

RETURN VALUE
Byte: Digital filter Gain in 1/10th of dB

SIDE EFFECTS
  None

===========================================================================*/
byte ftm_get_digital_filter_gain(rf_path_enum_type path);
#endif

#ifdef RF_HAS_POWER_MODE_LIB
/*===========================================================================

FUNCTION FTM_SET_RF_POWER_MODE

DESCRIPTION
  Override the current RF power mode selection.  The power mode
  parameter is RF target dependent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  RF performance may change.

===========================================================================*/
void ftm_set_rf_power_mode(rfcom_device_enum_type,
                           rflib_power_mode_type);
#endif /* RF_HAS_POWER_MODE_LIB */

/*===========================================================================

FUNCTION FTM_DO_TX_RX_FREQ_CAL_SWEEP

DESCRIPTION
  This function configures and starts the TX/RX frequency calibration sweep.
  
DEPENDENCIES
  None

RETURN VALUE
  Error code.

SIDE EFFECTS
  None

===========================================================================*/
uint16 ftm_do_tx_rx_freq_cal_sweep(uint16 rx_chain,
                                   uint8  num_freqs, 
                                   uint8  num_steps_per_freq, 
                                   uint16 step_length, 
                                   uint16 *freq_list_ptr, 
                                   uint16 *pdm_list_ptr, 
                                   int16  *exp_agc_val_ptr,                                   
                                   uint8  *pa_range_list_ptr, 
                                   uint8  *read_hdet_ptr, 
                                   uint8  *rx_action_ptr,
                                   uint8  *rx_lpm_list_ptr);   

/*===========================================================================

FUNCTION FTM_SET_TX_SWEEP_STEP_SIZE

DESCRIPTION
  This function allows the user to set the PDM step size for the TX cal sweep.
  The PDM will be decreased by the given number of counts for each step in
  the sweep.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_set_tx_sweep_step_size(uint8 step);

/*===========================================================================

FUNCTION FTM_SET_TX_WAVEFORM

DESCRIPTION
   This function will force the CDMA, CW or Offset CW depending on parameters passed

   For CDMA Waveform set ftm_tx_waveform parameter to CDMA_WAVEFORM

   For CW Waveform with zero offset set  ftm_tx_waveform parameter to CW_WAVEFORM 
   AND ftm_tx_attrib parameter to value of 0

   For offset CW at +19.2 KHz offset ftm_tx_waveform parameter to CW_WAVEFORM 
   AND ftm_tx_attrib parameter to value of FTM_MDSP_TX_ROTATOR_REG_VAL_FOR_19_2_KHZ_OFFSET_TONE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_set_tx_waveform(ftm_tx_waveform_type ftm_tx_waveform ,ftm_tx_waveform_attrib_type ftm_tx_attrib);

#if defined (FTM_HAS_XO_FACTORY_CAL) 
/*===========================================================================

FUNCTION FTM_DO_XO_DC_CAL

DESCRIPTION
  This function is used to perform the coarse frequency dc offset calibration 
  for xo-based targets
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

ftm_rsp_pkt_type ftm_do_xo_dc_cal( ftm_xo_cal_request_type* req );

/*===========================================================================

FUNCTION FTM_DO_XO_FT_CURVE_CAL

DESCRIPTION
  This function is used to perform the fine frequency dc offset calibration 
  and calibrating the real FT curve for xo-based targets
  
DEPENDENCIES
  Need to call ftm_do_xo_dc_cal() prior.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

ftm_rsp_pkt_type ftm_do_xo_ft_curve_cal( ftm_xo_cal_request_type* req );

/*===========================================================================

FUNCTION FTM_SET_TX_XO_CAL

DESCRIPTION
   This function will set the TX state for FM or CDMA.
   Used for XO Calibration.   Same as ftm_set_tx() but uses rex_sleep() instead 
   ftm_clk_rex_wait().   This allows use of API in non-FTM task.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_set_tx_xo_cal(rfcom_device_enum_type device, boolean state);

#endif /*FTM_HAS_XO_FACTORY_CAL*/

/*===========================================================================

FUNCTION FTM_DO_INTERNAL_DEVICE_CALIBRATION

DESCRIPTION
  This function performs any required internal device calibration for all
  devices on the current target (e.g. vco coarse tuning, baseband filter cal,
  iq resistor cal, etc.). It will perform the calibration regardless of the 
  state of the NV items.
  
DEPENDENCIES
  NV task must be running.

RETURN VALUE
  16-bit error code

SIDE EFFECTS
  Writes calibration results to NV.

===========================================================================*/
uint16 ftm_do_internal_device_calibration( void );

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
/*===========================================================================

FUNCTION FTM_SET_BLOCK_MASK_OVERRIIDE

DESCRIPTION
   This function forces the Block Mask for a given tuned Band.
   This function calls the RF Function to check for the Validity
   of Block Mask to be set.

DEPENDENCIES
   None.

RETURN VALUE
   Returns tRUE if Block Mask is Valid
   Returns FALSE if Block MAsk is not valid

SIDE EFFECTS
   None.

===========================================================================*/
boolean ftm_set_block_mask_override(rfcom_device_enum_type device,int8 index,uint32 block_mask);
#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/

#endif /* FEATURE_FACTORY_TESTMODE */
#endif  // FTM_RF_H

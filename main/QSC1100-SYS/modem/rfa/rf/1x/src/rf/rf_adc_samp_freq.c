  /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        HDR MAXIMUM TRANSMIT POWER BACKOFF

GENERAL DESCRIPTION
   

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=================================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_adc_samp_freq.c#4 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------------
04/14/08   vm      Changed the comment for ADC Sampling clock
03/05/08   vm      Clean up for QSC1100 Napolean and RICO Bringup
12/06/07   ems     Support for QSC1100
11/11/07   dyc     Added prototype rflib_set_drx_clkgen_div_ratio()
11/09/07   vm      Added the support for Programming second chain ADC Clockgen
                   Divider correctly
10/26/07   vm      Changed the rex critical sections to intlocks so that
                   target team can enable L4 asserts
07/03/07   zg      Added ADC Sample Freq Div Ratio dynanmic updates supports 
                   on GPS.
06/15/07   vm      Added the support for GPS Notch filter
05/15/07   vm     Added the API support for programming the Group delay 
                  based on Sampling frequency                   
04/22/07   vm      Fixed the Sampling frequency for Diversity Mode
04/18/07   vm      Added support for ADC and modem clock switching
                   for all the different modes of operation.
03/16/07   dyc     Compiler warning fix.
03/15/07   vm      Remove the feature ADC_SAMPLING_CLK_DEBUG for ES release
03/13/07   vm      Added the fix to switch to TCXO clock source for
                   RXF_SAMP_CLK in case GPS is OFF and Prim chain is OFF
12/08/06   vm      Defined the global variable for Sampling frequency
11/27/06   vm      Created 
===================================================================================*/
#include "rf_adc_samp_freq.h"

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
#include "err.h"
#include "rf_error.h"
#include "msg.h"
#include "bbrx.h"
#include "clkregim.h"
#include "rex.h"
#include "msm_help.h"
#include "rflib.h"
#include "rfi.h"
#include "rxf.h"

#ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
#endif

extern rf_path_0_config_type rf_path_0;  
extern rf_path_1_config_type rf_path_1;
extern rf_adc_samp_freq_bins_type rf_adc_samp_freq_bins_cdma[];
extern rf_chain_state_type rf_chain_status[];

boolean rf_adc_lin_mode_change_flag = FALSE;
uint32 qsc1100_rf_debug_samp_clk_src = 0x1;

rf_adc_clk_state_type rf_adc_clk_status =
{
    {0,0},
    DIV_OFF,
    {VCO_OFF,VCO_OFF},
    {CDMA_MODE,CDMA_MODE},
    {40e6,40e6},
    FALSE
};

#define RF_ADC_SAMP_CLK_ANT1_FB_VAL 0x2
#define RF_ADC_SAMP_CLK_ANT0_FB_VAL 0x1
#define RF_ADC_SAMP_CLK_TCXO_VAL 0

#ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
#endif

void rf_set_1x_prim_chain_to_off(rf_chain_div_flag_type div_flag,
                                 rf_curr_sys_mode_type mode);


void rf_set_1x_prim_chain_to_warmup(rf_chain_div_flag_type div_flag,
                                    rf_curr_sys_mode_type mode);

void rf_set_1x_prim_chain_to_on(rf_chain_div_flag_type div_flag,
                                rf_curr_sys_mode_type mode);

void rf_set_1x_sec_chain_to_off(rf_chain_div_flag_type div_flag,
                                rf_curr_sys_mode_type mode);

void rf_set_1x_sec_chain_to_warmup(rf_chain_div_flag_type div_flag,
                                   rf_curr_sys_mode_type mode);

void rf_set_1x_sec_chain_to_on(rf_chain_div_flag_type div_flag,
                               rf_curr_sys_mode_type mode);

void rf_set_1x_sec_chain_to_div(rf_chain_div_flag_type div_flag,
                                rf_curr_sys_mode_type mode);

void rf_adc_sel_rxf_sample_clk_src(uint32 clk_src);

void  rf_adc_samp_clk_sel_src(uint32 clk_src);

//extern void rflib_set_drx_clkgen_div_ratio(boolean prx_sleep_flag,
//                                          rf_curr_sys_mode_type mode,
//                                           rf_card_band_type band, 
//                                           uint16 chan);

/*===========================================================================

FUNCTION rf_get_samp_freq_bin                   EXTERNALIZED FUNCTION

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
byte rf_get_samp_freq_bin(rf_path_enum_type rf_path,rf_curr_sys_mode_type mode,
                          rf_card_band_type band,double samp_freq,
                          rflib_power_mode_type power_mode)
{
  byte samp_freq_index = 0;
  byte index = 0;
  byte band_segment = 0;

  switch (band)
  {
  case RF_BC0_BAND:
  case RF_BC10_BAND:
  case RF_BC5_BAND:
  case RF_BC11_BAND:
    band_segment = 0;
    break;

  case RF_BC1_BAND:
  case RF_BC6_BAND:
    band_segment = 1;
    break;

  default:    
    RF_FATAL_ERROR("Band not supported");
    return(0);    
  
  }

  switch ( mode )
  {
  case  CDMA_MODE:
    switch (rf_path)
    {
    case RF_PATH_0:
    case RF_PATH_1:
      while(index < MAX_NUM_SAMP_FREQ_BINS_PER_BAND_CLASS)
      {
        if((samp_freq >= rf_adc_samp_freq_bins_cdma[band_segment].bin_freq[power_mode][index]) && 
          (rf_adc_samp_freq_bins_cdma[band_segment].bin_freq[power_mode][index+1] > samp_freq))
        {
          samp_freq_index = index;
          break;
        }

        index++;
      }
      if(index == MAX_NUM_SAMP_FREQ_BINS_PER_BAND_CLASS)
      {
        RF_FATAL_ERROR("Not able to find the bin for ADC Samp Freq");
        return(0);
      }
      break;
    default:      
      RF_FATAL_ERROR("Path not supported");
      return(0);
    }
    break;

  default:      
    RF_FATAL_ERROR("Mode not supported");
    return(0);
  }
  return (samp_freq_index);
}
/*===========================================================================

FUNCTION RF_SET_PRIM_CHAIN_LIN_MODE_CHANGE                 EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever the Linearity mode change occurs
  on Primary chain

DEPENDENCIES
  rf_adc_clk_status.gps_on_flag must have been set to right value before 
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
                                       rflib_power_mode_type power_mode)
{
  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif

  /* Right Now returning for SC2X...Since Digital Filter Coefficients are */
  /* Same for two different Linearity Modes for both 1X and HDR*/
  /* Hence we are returning from this function W/O doing anything*/
  /* Later if somehow thses coeeficients depend on Linearity Mode*/
  /* we would already have implementation in Place*/
  
  return;

//  INTLOCK();//rex_enter_crit_sect(&rf_adc_clock_crit_sect);

  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif
  
  if(path == RF_PATH_0)
  {    
    rf_adc_clk_status.samp_freq[0] = 
      rflib_get_adc_samp_freq(path,mode,band,chan);
  
    rf_adc_clk_status.samp_freq_bin[0] =
       rf_get_samp_freq_bin(path,mode,band,rf_adc_clk_status.samp_freq[path],power_mode);

    rflib_set_bb_filter_coefs(rf_adc_clk_status.samp_freq_bin[0],path,mode,band,chan,power_mode);                    
    
  }
  else
  {
    RF_FATAL_ERROR("Linearity change is not supported on second chain");
  }

  rf_adc_lin_mode_change_flag = TRUE;

  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif

  //INTFREE();//rex_leave_crit_sect(&rf_adc_clock_crit_sect);
}

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
                                        rf_curr_sys_mode_type mode)
                                       
{

  switch(path)
  {
  case RF_PATH_0:
    switch(new_state)
    {
    case VCO_OFF:
      rf_set_1x_prim_chain_to_off(div_flag,mode);
      break;

    case VCO_WARMUP:
      rf_set_1x_prim_chain_to_warmup(div_flag,mode);
      break;

    case VCO_ON:
      rf_set_1x_prim_chain_to_on(div_flag,mode);
                                 
      break;

    default:
       RF_FATAL_ERROR("VCO Mode not supported on Primary chain");
       return;
    } /* switch(new_state) */
    break; /* case RF_PATH_0 */

  case RF_PATH_1:
    switch(div_flag)
    {
    case DIV_OFF:      
      switch(new_state)
      {
      case VCO_OFF:
        rf_set_1x_sec_chain_to_off(div_flag,mode);
        break;

      case VCO_WARMUP:
        rf_set_1x_sec_chain_to_warmup(div_flag,mode);
        break;

      case VCO_ON:
        rf_set_1x_sec_chain_to_on(div_flag,mode);
        break;

      default:
        RF_FATAL_ERROR("VCO Mode not supported on Secondary chain");
        return;
      } /* switch(new_state) */
      break;/* case DIV_OFF: */

    case DIV_ON:
      switch(new_state)
      {      
      case VCO_OFF:
        rf_set_1x_sec_chain_to_div(div_flag,mode);
        break;

      case VCO_WARMUP:
        RF_FATAL_ERROR("Second Chain VCO Cannot be in Warmup Mode in case of diversity Mode");
        break;

      case VCO_ON:
        RF_FATAL_ERROR("Second Chain VCO Cannot be ON in case of diversity Mode");
        break;

      default:
        RF_FATAL_ERROR("VCO Mode not supported on Secondary chain");
        return;
      } /* switch(new_state) */
      break; /* case DIV_ON */

    default:
      RF_FATAL_ERROR("Diversity Mode can only be ON or OFF on second chain");
      return;

    }/*switch(div_flag)*/
    break; /*case RF_PATH_1*/

  default:
    RF_FATAL_ERROR("Path not supported");
    return;
  }/* switch(path) */
}/* rf_1x_chain_switch_adc_clk_set_rxf()*/

/*===========================================================================

FUNCTION RF_SET_1X_PRIM_CHAIN_TO_OFF            EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever primary chain goes off to sleep 
  through the function Call rf_sleep_cdma1x(Reciever 0).
  
DEPENDENCIES
None
  
RETURN VALUE
None

SIDE EFFECTS
- This function Sets the Primary chain VCO state to OFF.

- Depending upon the fact that second chain is ON or in Warmup mode
  this function programs the Second chain ADC and Modem Sample clock 
  sources to be derived from Second Chain VCO rather than Primary Chain VCO.
  Hence it reprograms the Digital Filer, Notch filter and resampler
  Coefficients for Second chain if it is not OFF

- This function will turmn OFF the Diversity Flag since we Cannot support 
  diversity when Primary Chain VCO goes OFF.

- This function effectively does not do anything if GPS is ON on second 
  chain  
 
===========================================================================*/
void rf_set_1x_prim_chain_to_off(rf_chain_div_flag_type div_flag_now,rf_curr_sys_mode_type mode0)
{
  //byte msm_clk_status;  
  rf_chain_vco_state_type new_state;
  rf_chain_vco_state_type prev_state ;
  rf_card_band_type band;
  word chan;

  #ifdef RF_HAS_POWER_MODE_LIB         
  rflib_power_mode_type power_mode = RFLIB_NORMAL_POWER_MODE;
  #endif
  
  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif 

  
  #ifdef RF_HAS_POWER_MODE_LIB         
  power_mode = rflib_get_power_mode(RF_PATH_0);  
  #endif

  band = rf_chain_status[0].rf_curr_cdma_band;
  chan = rf_chain_status[0].rf_chan;
  
  //INTLOCK();

  new_state = VCO_OFF;

  prev_state = rf_adc_clk_status.vco_state[0];

  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif
  
  if(new_state != VCO_OFF)
  {
    RF_FATAL_ERROR("PRx VCO should be turned OFF in this function");
  }

  rf_adc_clk_status.vco_state[0]  = VCO_OFF;

//  msm_clk_status = (HWIO_INM(MODEM_CHIPXN_CLK_MISC_CTL,0x00060000) >> 17) & 0x3;

   /*#ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
     modem_clk_halt_dump = HWIO_INM(MODEM_CLK_HALT,0xFFFFFFFF); 
     MSG_HIGH("Previous clock source for Sample clock is 0x%02X and MODEM_CLK_HALT_DUMP: 0x%02X :", msm_clk_status,modem_clk_halt_dump,0);
    #endif*/

  /*if(msm_clk_status != RF_ADC_SAMP_CLK_TCXO_VAL)//!=TCXO_CLK
  {
    rf_adc_sel_rxf_sample_clk_src(CLK_RGM_SRC_SEL_RX_SAMPLE_CLK_TCXO_V);
  }*/ 

  clk_regime_enable( CLKRGM_FB_ANT0_SAMP_CLK );

  rf_adc_samp_clk_sel_src(0x4);//Selecting CLK SRC to be MSM

  clk_regime_disable( CLKRGM_FB_ANT1_SAMP_CLK );

  /* Changing the PRX_VCO_STATE to VCO_OFF */

  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
 #endif

 //INTFREE();//rex_leave_crit_sect(&rf_adc_clock_crit_sect);
  

}/* rf_set_1x_prim_chain_to_off() */

/*===========================================================================

FUNCTION RF_SET_1X_PRIM_CHAIN_TO_WARMUP           EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever primary chain VCO goes to Warmup 
  Sate meaning it is not Locked. It Can happen during the handoff Scenario
  when Channel frequency Changes and VCO frequency changes from one to other 
  frequency. It also happens during slotted sleep Wakeup until the VCO is not locked to
  Channel frequency.Efefctively this function is Called through 
  rf_tune_to_chan_cdma1x(Receiver 0 or Receiver Div)(during Handoff scenario) 
  and f_digital_chan_change(Receiver 0)(during slotted sleep wakeup).
 
    
DEPENDENCIES
None
  
RETURN VALUE
None

SIDE EFFECTS
- This function Sets the Primary chain VCO state to Warmup state
  meaning the VCO is not locked Yet although the Primary chain
  is in Receive Mode.

- If the Secondary Chain VCO is in Warmup or OFF Mode the function will
  Set the Sample Clock source for Rx Front to be TCXO.If Seconadry Chain VCO
  is ON ,the function will set the Sample Clock as well as ADC Clock source
  to be derived from Second chain VCO

  - If the Secondary Chain VCO is ON Mode this function programs the 
  Second chain ADC and Modem Sample clock sources to be derived from 
  Second Chain VCO rather than Primary Chain VCO.Hence it reprograms 
  the Digital Filer, Notch filter and resampler Coefficients for Second 
  chain.

- If GPS is ON menaing GPS_ON_FLAG is True this function will program the
  Digital Filter, Notch filter and Resampler coefficients based on
  GPS Sampling frequency for Primary Chain

- If the Diversity is ON this function Will Set the Second Chain VCO 
  to be OFF and programs the Primary and Second chain ADC and 
  Modem Sample clock sources to be derived from Primary Chain VCO.
  Hence it reprograms the Digital Filer, Notch filter and resampler 
  Coefficients for both Primary and Second Chain based on 
  Sampling frequency derived from Primary Chain VCO.

- Thus this function Can also set the State Variable for Second Chain VCO
  to be OFF if we are in diversity Mode.  
 
===========================================================================*/
void rf_set_1x_prim_chain_to_warmup(rf_chain_div_flag_type div_flag_now,
                                    rf_curr_sys_mode_type mode0)

{

 // byte msm_clk_status;
  rf_chain_vco_state_type prev_state;
  rf_chain_vco_state_type new_state;
  rf_card_band_type band;
  word chan;

  #ifdef RF_HAS_POWER_MODE_LIB         
  rflib_power_mode_type power_mode = RFLIB_NORMAL_POWER_MODE;
  #endif

  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif 

  #ifdef RF_HAS_POWER_MODE_LIB         
  power_mode = rflib_get_power_mode(RF_PATH_0);
  #endif

  band = rf_chain_status[0].rf_curr_cdma_band;

  chan = rf_chain_status[0].rf_chan;

  //INTLOCK();//rex_enter_crit_sect(&rf_adc_clock_crit_sect);
  
  prev_state = rf_adc_clk_status.vco_state[0];

  new_state = VCO_WARMUP;
  
  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif

  if(new_state != VCO_WARMUP)
  {
    RF_FATAL_ERROR("This function is to be called when PRX VCO state is changed to warmup");
  }

  rf_adc_clk_status.vco_state[0] = VCO_WARMUP;
  
  bbrx_set_adc_source( BBRX_CHAIN_0, BBRX_PRX_VCO );    

   /*#ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
     modem_clk_halt_dump = HWIO_INM(MODEM_CLK_HALT,0xFFFFFFFF); 
      MSG_HIGH("Previous clock source for Sample clock is 0x%02X and MODEM_CLK_HALT_DUMP is 0x%02X :",msm_clk_status,modem_clk_halt_dump,0);
    #endif*/

 // msm_clk_status = (HWIO_INM(MODEM_CHIPXN_CLK_MISC_CTL,0x00060000) >> 17) & 0x3;

  /*if(msm_clk_status != RF_ADC_SAMP_CLK_TCXO_VAL)//!=ANT1_SAMP_CLK
  {
    rf_adc_sel_rxf_sample_clk_src(CLK_RGM_SRC_SEL_RX_SAMPLE_CLK_TCXO_V);
  }*/
  
  clk_regime_enable( CLKRGM_FB_ANT0_SAMP_CLK );

  rf_adc_samp_clk_sel_src(0x4);//Selecting CLK SRC to be MSM

  clk_regime_disable( CLKRGM_FB_ANT1_SAMP_CLK );

   rf_adc_clk_status.samp_freq[0] = 
       rflib_get_adc_samp_freq(RF_PATH_0,mode0,band,chan);

   rf_adc_clk_status.samp_freq_bin[0] =
       rf_get_samp_freq_bin(RF_PATH_0,mode0,band,rf_adc_clk_status.samp_freq[0],power_mode);

  rflib_set_bb_filter_coefs(rf_adc_clk_status.samp_freq_bin[0],RF_PATH_0,mode0,band,chan,power_mode); 

  rflib_set_resampler_phase_map_coefs(RF_PATH_0,rf_adc_clk_status.samp_freq[0]);

  rflib_set_rf_chain_grp_delay(RF_PATH_0,rf_adc_clk_status.samp_freq[0], rf_adc_clk_status.samp_freq_bin[0]);

  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif  

  //INTFREE();//rex_leave_crit_sect(&rf_adc_clock_crit_sect);

}/* rf_set_1x_prim_chain_to_warmup() */

/*===========================================================================

FUNCTION RF_SET_1X_PRIM_CHAIN_TO_ON           EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever primary chain VCO goes to ON
   state meaning it gets locked after coming from Warmup state.
  Sate.This function is effectively called after rf_wait_for_synth_lock()
  after tuning the PLL on primary Chain.
  This function is effectively called from rf_init_digital_band_chan(Receievr 0)
  (during slotted sleep wakeup)or through rf_tune_to_chan_cdma1x(Receiver 0 or Reciever Div)
  (during Handoff scenario) 
      
DEPENDENCIES
None
  
RETURN VALUE
None

SIDE EFFECTS
- This function Sets the Primary chain VCO state to ON state.
  Assumption is that VCO gets locked after 800 us delay in 
  rf_wait_for_synth_lock().If the VCO is not locked after this much
  delay the Phone might Crash because in this function we are 
  switching the Sample Clock for Harrier to be based on Primary Chain VCO  

- The function will set the Modem Sample Clock as well as ADC Clock source
  to be derived from Primary chain VCO  

- If the Secondary Chain VCO is in Warmup or ON Mode, this function will 
  reprogram the Digital Filer, Notch filter and resampler Coefficients for Second 
  chain based on Sampling frequency derived from Primary chain VCO.

===========================================================================*/
void rf_set_1x_prim_chain_to_on(rf_chain_div_flag_type div_flag_now,
                                 rf_curr_sys_mode_type mode0)
{
 // byte msm_clk_status;  
  rf_chain_vco_state_type prev_state;  
  rf_chain_vco_state_type new_state;
  rf_card_band_type band;
  word chan;

  #ifdef RF_HAS_POWER_MODE_LIB         
  rflib_power_mode_type power_mode = RFLIB_NORMAL_POWER_MODE;
  #endif

  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif 

 
  #ifdef RF_HAS_POWER_MODE_LIB         
  power_mode = rflib_get_power_mode(RF_PATH_0);  
  #endif

  band = rf_chain_status[0].rf_curr_cdma_band;

  chan = rf_chain_status[0].rf_chan;

  //INTLOCK();//rex_enter_crit_sect(&rf_adc_clock_crit_sect);

  prev_state = rf_adc_clk_status.vco_state[0];

  new_state = VCO_ON;

  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif

  if(new_state != VCO_ON)
  {
    RF_FATAL_ERROR("This function is to be called when PRX VCO state is changed to ON");
  }
  
  rf_adc_clk_status.vco_state[0] = VCO_ON;

  clk_regime_enable( CLKRGM_FB_ANT0_SAMP_CLK );

  #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
  #else
  rf_adc_samp_clk_sel_src(qsc1100_rf_debug_samp_clk_src);//Selecting CLK SRC to be RF VCO
  #endif

  clk_regime_disable( CLKRGM_FB_ANT1_SAMP_CLK );
   
//   msm_clk_status = (HWIO_INM(MODEM_CHIPXN_CLK_MISC_CTL,0x00060000) >> 17) & 0x3;

  /* if(msm_clk_status != RF_ADC_SAMP_CLK_ANT0_FB_VAL)//!=ANT0_SAMP_CLK
   {
     rf_adc_sel_rxf_sample_clk_src(CLK_RGM_SRC_SEL_RX_SAMPLE_CLK_ANT0_SAMP_CLK_V);
   }*/
 
  #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
  #endif  
  //INTFREE();//rex_leave_crit_sect(&rf_adc_clock_crit_sect);
}/* rf_set_1x_prim_chain_to_on() */

/*===========================================================================

FUNCTION RF_SET_1X_SEC_CHAIN_TO_OFF            EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever primary chain goes off to sleep 
  through the function Call rf_sleep_cdma1x(Reciever 1).
  
DEPENDENCIES
None
  
RETURN VALUE
None

SIDE EFFECTS
- This function Sets the Secondary chain VCO state to OFF.

- This function will set the Sample Clock to Modem to be TCXO clock
  if the Primary chain VCO is in warmup or OFF Mode.
===========================================================================*/
void rf_set_1x_sec_chain_to_off(rf_chain_div_flag_type div_flag_now,
                                 rf_curr_sys_mode_type mode1)

{
  return;  
}/*rf_set_1x_sec_chain_to_off()*/

/*===========================================================================

FUNCTION RF_SET_1X_SEC_CHAIN_TO_WARMUP           EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever Secondary chain VCO goes to Warmup 
  Sate meaning it is not Locked. It Can happen during the handoff Scenario
  when Channel frequency Changes on second chain VCO frequency changes from one to other 
  frequency. It also happens during slotted sleep Wakeup until the VCO is not locked to
  Channel frequency.Efefctively this function is Called through 
  rf_tune_to_chan_cdma1x(Receiver 1)(during Handoff scenario) 
  and rf_digital_chan_change(Receiver 1)(during slotted sleep wakeup).

 Note that this function cannot be called when diversity is enabled.
 Reason being When Diversity is ON the second chain uses PRx VCO and
 second chain VCO is Always OFF.
    
DEPENDENCIES
None
  
RETURN VALUE
None

SIDE EFFECTS
- This function Sets the Secondary chain VCO state to Warmup state
  meaning the VCO is not locked Yet although the Second chain
  is in Receive Mode.

- If the Primary Chain VCO is in Warmup or OFF Mode the function will
  Set the Sample Clock source for Rx Front to be TCXO.

- It reprograms the Digital Filer, Notch filter and resampler Coefficients 
  for Second chain.The Sampling frequency for digital or Notch filter
  will be based oN PRx VCO frequency if PRx VCO is ON otherwise it will be 
  based on DRx VCO frequency.

- This function sets the DiV Flag to OFF
===========================================================================*/
void rf_set_1x_sec_chain_to_warmup(rf_chain_div_flag_type div_flag_now,
                                 rf_curr_sys_mode_type mode1)

{
  return;  
}/*rf_set_1x_sec_chain_to_warmup()*/

/*===========================================================================

FUNCTION RF_SET_1X_SEC_CHAIN_TO_ON          EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever primary chain VCO goes to ON
   state meaning it gets locked after coming from Warmup state.
  Sate.This function is effectively called after rf_wait_for_synth_lock()
  after tuning the PLL on Secondary Chain.
  This function is effectively called from rf_init_digital_band_chan(Receievr 1)
  (during slotted sleep wakeup)or through rf_tune_to_chan_cdma1x(Receiver 1)
  (during Handoff scenario) 

  Note that this function cannot be called when diversity is enabled.
 Reason being When Diversity is ON the second chain uses PRx VCO and
 second chain VCO is Always OFF.
      
DEPENDENCIES
None
  
RETURN VALUE
None

SIDE EFFECTS
- This function Sets the Secondary chain VCO state to ON state.
  Assumption is that VCO gets locked after 800 us delay in 
  rf_wait_for_synth_lock().If the VCO is not locked after this much
  delay and the primary chain VCO is not ON then the Phone might 
  Crash because then we are switching the Sample Clock for Harrier 
  to be based on Second Chain VCO  

- This function sets the DiV Flag to OFF

===========================================================================*/
void rf_set_1x_sec_chain_to_on(rf_chain_div_flag_type div_flag_now,
                                 rf_curr_sys_mode_type mode1)
{
  return;
}/*rf_set_1x_sec_chain_to_on()*/

/*===========================================================================

FUNCTION RF_SET_1X_SEC_CHAIN_TO_DIV          EXTERNALIZED FUNCTION

DESCRIPTION
  This function is to be called whenever diversity is enabled for 1x 
  or HDR Mode at time when the PRx VCO is already ON.
  Efefctively the Calls to tis function are made from rf_enable_diversity()
  for 1X or rf_init_hdr() for HDR.
  
  Note that this function is to be called only when diversity is enabled 
  
    
DEPENDENCIES
None
  
RETURN VALUE
None

SIDE EFFECTS

- It reprograms the Digital Filer, Notch filter and resampler Coefficients 
  for Second chain.The Sampling frequency for digital or Notch filter
  will be based oN PRx VCO frequency.

- This function sets the DiV Flag to ON

-This function sets the second chain VCO state to OFF.

===========================================================================*/
void rf_set_1x_sec_chain_to_div(rf_chain_div_flag_type div_flag_now,
                                 rf_curr_sys_mode_type mode1)
{
  return;  
}/*rf_set_1x_sec_chain_to_div()*/

void rf_adc_sel_rxf_sample_clk_src(uint32 clk_src)
{

  clk_regime_sel_clk_src (CLKRGM_RXF_SAMP_CLK,clk_src);

  //MSM_W_MASKED_i(MODEM_CLK_HALT, RXF_SAMP_CLK_HALT, 1);
 
  //clk_busy_wait(1);

  //MSM_W_MASKED_i(MODEM_CHIPXN_CLK_MISC_CTL,CDMA_RX_SAMPLE_RAW_SEL,CLK_SRC);

  //MSM_W_MASKED_i(MODEM_CLK_HALT, RXF_SAMP_CLK_HALT,0);
}

void  rf_adc_samp_clk_sel_src(uint32 clk_src)
{
    HWIO_OUT(RXA2D_CLK, clk_src);
}
#endif /*RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO*/


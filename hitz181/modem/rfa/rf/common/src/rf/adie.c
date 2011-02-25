/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 A N A L O G   D I E    S E R V I C E S

GENERAL DESCRIPTION
  This file contains variable declarations to support interaction
  with the analog die of a stacked digital/analog die MSM.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1998 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/adie.c_v   1.7   12 Jul 2002 17:18:40   ehandojo  $
  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/adie.c#3 $ $DateTime: 2008/07/20 12:54:16 $ $Author: vmahajan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/08   jc/vm   Fixed HKADC Driver for QSC1100
08/28/07   ymc     Updated adie_codec_config to support mask and delay.
08/09/07   dyc     Externalize adie_codec_enable() for vocoder.
                   Remove adie_codec_enable() management from adie_codec_config()
                   Added API adie_codec_read()
06/19/07   gdp     Fixed TCXO compilation error
06/19/07   dyc     Include adie_adc.h
06/16/07   abh     Changes to support XO Thermsitor ADC.
12/14/06   ans     Changes for SC2x
12/01/06   dyc     Modified for RaMSIS CODEC
11/29/06   dyc     Stub out initializations for bringup.
07/29/06   dyc     Update EMU bit for BBRX/CODEC sbi access.
07/19/06   dyc     Updated to use local TCXO for SC2x RUMI via T_RUMI_USE_RF_TCXO
06/19/06   hkk     Fixed compile errors on 7500
05/22/06   hkk     Common Adie driver for 1x and UMTS 
01/11/06   xw      Removed rfcom.h and rflib.h inclusion.  
                   Renamed adie_bbrx_wakeup to bbrx_wakeup and moved it to bbrx.c
11/18/05   ty      Modify HKADC initialization
09/22/05   ymc     Modified for Saturn2 codec support.
           ymc     Updated Copyright.
05/24/05   et      removed conditional compile around HKADC programming
05/09/05   ra      Changed logic to exclude DORA and DEVO in adie_init.
03/04/05   ra      For DORA, removed non-necessary register writes to wake up BBRX in Adie_init.
01/13/05   jac/ra  DORA merge. Added a line in adie_init to map ANT1 to RX1 in CFG reg.
10/01/04   aks     Adjust featurization for MSM6700 for all emulation
                   platforms.
09/30/04   jac/wd  Added code for MSM6700 emulation, to wake up BBRX in
                   adie_init.
08/11/04    rv     While GPS is active, don't turn off BBRx channel 0.
04/26/04    rv     CM bug fix for 2.1 MSM. GPS handles channel1.
05/10/04    et     added rf_mode parameter to adie_init
04/30/04    rv     Optimized bbrx channel1 shutdown for more power savings.
04/23/04    et     changed clkrgm_6500.h to  CLKRGM_H  in #include
03/25/04    rv     When GPS is active, don't turn channel 0 off.
03/15/04    rv     Featurized 6500 and corrected adie_write_byte usage.
03/10/04    rv     Turn off BBrx channel 1 for both platforms during sleep.
03/08/04   dyc     update adie_clk_regime_config() to support enable/disable of
                   the analog clk regime.
02/05/04    et     writing to the SLEEP_BBR bit of TEST_ADIE_DATA instead
                   of the CODEC bit.  This fixes the 2nd chain sleepb_bbr 
                   problem which disabled the 2nd chain during div.
01/28/04    et     changed parameter to adie_bbrx_wakeup to rf_path_enum_type
                   rf_path from device type.
                   added function adie_clk_regime_config() to be called
                   by adie_bbrx_wakeup to configure the adie clock regime
                   enable at wakeup, disable at sleep, based on diff/se feature
01/23/04    rv     Updated procedure to shutdown/wakeup HKADC.
01/20/04    et     added 2 lines to take the bbrx out of sleep for MSM6500.
                   touching MSM regs TEST_ADIE_DATA and TEST_ADIE_DATA_SEL
01/12/04    rv     Disabled HKADC in adie_init().
01/07/04    rv     Turn off BBRX1 for non-GPS builds during sleep along with BBRX0.
11/03/03    et     added write to HKADC_SETUP_R for 6250
10/09/03    rv     Removed FEATURE_6250_COMPILE.
08/28/03    rv     Don't power off GPS channel during BBRX sleep.
07/01/03   bbd     Modified RX_CHAIN_CONFIG value when using baseband WCDMA PLT
06/10/03    rv     Enabled keeper function on the SBDT pin in MISC_CTL_0.
05/16/03    rv     Added support for Pluto ADIE, new 6250 reg and removed MSMHW_5200.
02/18/03   rmd     Removed support for old adie HKADC driver (removed 
                   adie_adc_read()).
02/06/03    eh     Removed FEATURE_RF_ENABLE_BATTERY_CHARGING flag checking.
10/17/02   ekc     adie_adc_read() rearranged to take care of MDSP/SBI mode
            eh     Merged  under FEATURE_RF_ENABLE_BATTERY_CHARGING.
09/19/02    eh     Use MSM_OUTSH to access MISC_CTL_0 register.
09/19/02    eh     Modified adie_init() to set codec to sleep and remove USIM_PAD_EN
                   bit setting. 
07/12/02    eh     Merged MDSP services changes.
06/29/02   thh     Removed GSM-specific initializations.
06/27/02   thh     Added GSM support under FEATURE_GSM.  This is temporary and
                   should be removed soon.  The function adie_adc_read() only
                   locks interrupts during the conversion period.  Getting the
                   read results from SBI does not requires interrupts to be
                   locked as done previously.
06/19/02    eh     Modified adie_codec_enable and adie_codec_config to support
                   common AVS and MDSP service.
05/20/02    eh     Removed usage of ADIE_* macro and replaced with adie_*
                   function call.
04/12/02    eh     Use MSMHW_MSM5200 as a compile flag for MSM5200 build.
04/02/02    eh     Set HKADC to sleep during initialization.
                   Added FEATURE_RF_MSM5200 and use rfbbadc if it is defined.
03/21/02    eh     Added MISC_CTL_0_USIM_PAD_EN_SHFT bit programming.
01/22/02    eh     Port for MSM6200
12/03/01   rmd     Re-wrote the ADIE HKADC driver.
11/26/01    et     added support for msm6000 in bbrx_wakeup, and mainlined it
11/19/01    aw     Added adie_codec_config(), adie_codec_enable() and remove
                   adie codec powerdown/up in adie_wakeup.
11/15/01   aks     Wrapped adie_wakeup() and adie_bbrx_wakeup() in T_MSM_6050.
11/13/01    et     put HKADC and CODEC writes inside T_MSM_6050
11/13/01   aks     Wrap power up/down control in T_MSM_6050.
11/12/01   aks     Added adie powerdown and powerup control during adie_init().
11/06/01   rmd     Moved in macro ADIE_ADC_MUX_VREF_MAP from adc60X0xyz.h
10/25/01    et     updated some initializations
10/19/01   aks     Use the ADIE fail/success return type from macros.  Filled
                   in the adie_wakeup() function.  Removed bbrx code.
10/12/01    et     replaced sbi_write_bytes with ADIE_OUTM_VERIFY and #defines
                   removed writes to read-only and reserved registers
10/09/01   aks     Added adie_set_system_mode(), merged phone_t changes, added
                   stub for adie_wakeup() function.
09/05/00    et     initial revision in the MSM6000 archive.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "adie.h"
#include "adie_adcdefs.h"
#include "adie_cdcdefs.h"  
#include "adie_regdefs.h"
#include "adie_adc.h"
#include "adie_sbi.h"
#include "msm.h"
#include "msg.h"
#include "err.h"
#include "bbrx.h"
#include "clk.h"
#ifdef T_MSM6500
#include CLKRGM_H
#endif
#include "target.h"


#if defined (FEATURE_GSM) && !defined(T_MSM6280)
#error code not present
#endif

#ifdef FEATURE_WCDMA_PLT
#error code not present
#endif

#ifdef T_MSM6500
#include "rfcom.h"
#include "rflib.h"
#include "rf1x.h"
#endif
#include "adie_sbi.h"

/* Enable for testing */
//#define ADIE_TEST
#ifdef ADIE_TEST
extern void adie_test(void);
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.


===========================================================================*/


#if defined(T_MSM6250) && !defined(T_MSM6280)
#error code not present
#endif


extern adie_result_type adie_bbrx_write_verify(byte reg, byte mask, byte value );


/*===========================================================================

FUNCTION adie_init

DESCRIPTION
  This function initializes the analog die for operation.

DEPENDENCIES
  sbi_init() must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
void adie_init(adie_system_mode rf_mode)
{
  word int_save;
  rf_mode = rf_mode;

  /* Put ADIE to sleep while programming it */

#if defined(T_MSM6700) || defined(T_MSM6800)

  //lesp-test
  HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0,SLEEPB_BBR_GRFC_EN),
                         ( 0 << HWIO_SHFT (MODEM_MISC_CFG0,SLEEPB_BBR_GRFC_EN)) );
  HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0,SLEEPB_BBR_ORIDE_VAL),
                         ( 0 << HWIO_SHFT (MODEM_MISC_CFG0,SLEEPB_BBR_ORIDE_VAL)) );
  HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0,CDMA_RX1_ANT_SEL),
                         ( 1 << HWIO_SHFT (MODEM_MISC_CFG0,CDMA_RX1_ANT_SEL)) );

#ifdef T_MSM7500  //lesp
#error code not present
#endif /* T_MSM7500 */

#elif defined(T_MSM6500) && !defined(T_QSC1100)

  HWIO_OUTM( SLEEP_N_ADIE, HWIO_RMSK(SLEEP_N_ADIE), ADIE_POWER_DOWN_V );


#elif defined(T_MSM6280)
#error code not present
#elif defined(T_MSM6250)
#error code not present
#endif



  /* --------------------------------------------- */
  /* Several hardware registers have nonzero       */
  /* defaults.  Make the shadow buffers match the  */
  /* hardare.                                      */
  /* --------------------------------------------- */
  bbrx_init();

  INTLOCK_SAV( int_save );

#ifndef T_QSC60X5
#error code not present
#else  /* SC2x */
  /* Initialize the CODEC */
  ADIE_OUTM( ADIE_REFERENCE_CONTROL1_R, 0xFF, ADIE_REFERENCE_CONTROL1_INIT_V);
  ADIE_OUTM( ADIE_REFERENCE_CONTROL2_R, 0xFF, ADIE_REFERENCE_CONTROL2_INIT_V);
  ADIE_OUTM( ADIE_REFERENCE_CONTROL3_R, 0xFF, ADIE_REFERENCE_CONTROL3_INIT_V);
  ADIE_OUTM( ADIE_AUX_PGA_CONTROL1_R, 0xFF, ADIE_AUX_PGA_CONTROL1_INIT_V);
  ADIE_OUTM( ADIE_AUX_PGA_CONTROL2_R, 0xFF, ADIE_AUX_PGA_CONTROL2_INIT_V);
  ADIE_OUTM( ADIE_CODEC_TX_CNTL_REGISTER1_R, 0xFF, ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V);
  ADIE_OUTM( ADIE_CODEC_TX_CNTL_REGISTER2_R, 0xFF, ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V);
  ADIE_OUTM( ADIE_CODEC_TX_CNTL_REGISTER3_R, 0xFF, ADIE_CODEC_TX_CNTL_REGISTER3_INIT_V);
  ADIE_OUTM( ADIE_CODEC_TX_CNTL_REGISTER4_R, 0xFF, ADIE_CODEC_TX_CNTL_REGISTER4_INIT_V);
  ADIE_OUTM( ADIE_CODEC_TX_CNTL_REGISTER5_R, 0xFF, ADIE_CODEC_TX_CNTL_REGISTER5_INIT_V);
  ADIE_OUTM( ADIE_CODEC_TX_CNTL_REGISTER6_R, 0xFF, ADIE_CODEC_TX_CNTL_REGISTER6_INIT_V);
  ADIE_OUTM( ADIE_CODEC_TX_CNTL_REGISTER7_R, 0xFF, ADIE_CODEC_TX_CNTL_REGISTER7_INIT_V);
  ADIE_OUTM( ADIE_CODEC_TX_CNTL_REGISTER8_R, 0xFF, ADIE_CODEC_TX_CNTL_REGISTER8_INIT_V);
  ADIE_OUTM( ADIE_ATEST_TX_REGISTER_R, 0xFF, ADIE_ATEST_TX_REGISTER_INIT_V);
  ADIE_OUTM( ADIE_GPR_MS1_REGISTER_R, 0xFF, ADIE_GPR_MS1_REGISTER_INIT_V);
  ADIE_OUTM( ADIE_GPR_MS2_REGISTER_R, 0xFF, ADIE_GPR_MS2_REGISTER_INIT_V);
  ADIE_OUTM( ADIE_GPR_MS3_REGISTER_R, 0xFF, ADIE_GPR_MS3_REGISTER_INIT_V);
  ADIE_OUTM( ADIE_DDO_MS_REGISTER_R, 0xFF, ADIE_DDO_MS_REGISTER_INIT_V);
  ADIE_OUTM( ADIE_RX_FILTER_CONTROL_REGISTER1_R, 0xFF, ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_V);
  ADIE_OUTM( ADIE_RX_FILTER_CONTROL_REGISTER2_R, 0xFF, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V);
  ADIE_OUTM( ADIE_RX_FILTER_CONTROL_REGISTER3_R, 0xFF, ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_V);
  ADIE_OUTM( ADIE_RX_FILTER_CONTROL_REGISTER4_R, 0xFF, ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V);
  ADIE_OUTM( ADIE_RX_FILTER_CONTROL_REGISTER5_R, 0xFF, ADIE_RX_FILTER_CONTROL_REGISTER5_INIT_V);
  ADIE_OUTM( ADIE_RX_PA_ENABLE_REGISTER1_R, 0xFF, ADIE_RX_PA_ENABLE_REGISTER1_INIT_V);
  ADIE_OUTM( ADIE_RX_PA_ENABLE_REGISTER2_R, 0xFF, ADIE_RX_PA_ENABLE_REGISTER2_INIT_V);
  ADIE_OUTM( ADIE_RX_PA_MUX_REGISTER1_R, 0xFF, ADIE_RX_PA_MUX_REGISTER1_INIT_V);
  ADIE_OUTM( ADIE_RX_PA_MUX_REGISTER2_R, 0xFF, ADIE_RX_PA_MUX_REGISTER2_INIT_V);
  ADIE_OUTM( ADIE_RX_PA_BIAS_REGISTER1_R, 0xFF, ADIE_RX_PA_BIAS_REGISTER1_INIT_V);
  ADIE_OUTM( ADIE_RX_PA_BIAS_REGISTER2_R, 0xFF, ADIE_RX_PA_BIAS_REGISTER2_INIT_V);
  ADIE_OUTM( ADIE_RX_PA_BIAS_REGISTER3_R, 0xFF, ADIE_RX_PA_BIAS_REGISTER3_INIT_V);
  ADIE_OUTM( ADIE_RX_PA_BIAS_REGISTER4_R, 0xFF, ADIE_RX_PA_BIAS_REGISTER4_INIT_V);
  ADIE_OUTM( ADIE_RX_PA_RESERVED_REGISTER_R, 0xFF, ADIE_RX_PA_RESERVED_REGISTER_INIT_V);
  ADIE_OUTM( ADIE_ATEST_RX_REGISTER_R, 0xFF, ADIE_ATEST_RX_REGISTER_INIT_V);

#endif /* T_QSC60X5 */
 
 /* Wake up the ADIE again */
#if defined(T_MSM6700) || defined(T_MSM6800)
   HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0,SLEEPB_BBR_ORIDE_VAL),
                          ( 1 << HWIO_SHFT (MODEM_MISC_CFG0,SLEEPB_BBR_ORIDE_VAL)) );


#ifdef T_RUMI /* lp */
#error code not present
#else
#ifdef T_MSM7500
#error code not present
#elif !defined(T_QSC1100)
  HWIO_OUTM( SLEEP_N_ADIE, HWIO_RMSK(SLEEP_N_ADIE), ADIE_POWER_UP_V );
#endif /* T_MSM7500 */
#endif /* T_RUMI */
 
#elif defined(T_MSM6250)
#error code not present
#endif /* defined(T_MSM6700) || defined(T_MSM6800) */
  
  /* initialize HKADC */
  adie_init_adc();
  adie_wakeup( ADIE_ENABLE, ADIE_HKADC_BLK ); 

  #ifdef FEATURE_XO_ADC
  /* Initialize XO Thermistor ADC */
  adie_init_xotherm_adc();
  adie_wakeup(ADIE_ENABLE,ADIE_XOTHERM_BLK);
  #endif /* FEATURE_XO */
  


#ifdef T_RUMI /* lp */
#error code not present
#endif 

  INTFREE_SAV( int_save );

#ifdef ADIE_TEST
  adie_test();
#endif /* ADIE_TEST  */
}


/*===========================================================================

FUNCTION      adie_bbrx_wakeup

DESCRIPTION

  This function wakes up or puts to sleep the BBRX core of the ADIE

PARAMETERS

  wakeup- whether the core is to be put to sleep or be woken up

DEPENDENCIES

  sbi_init() must have been called. (for sbi communication targets)
  adie_init() must have been called.


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void adie_bbrx_wakeup (rf_path_enum_type rf_path, adie_op_type wakeup)
{
  uint16 adie_path_register = 0x19;

#ifdef T_QSC60X5
  /* BBRX manaaged in RaMSIS RF driver */
  return;
#endif /* T_QDC60X5 */

  /*  As per the Saturn objective spec, primary chain is Channel 1, and secondary 
      chain is chain 0. However, channels 1 and 0 are connected to RXF0 and RXF1 
      respectively. In 1x, the GMC filter is not used, so this swap does not matter 
      for the SBI settings for either chains. However, the right chain needs to be 
      turned off while sleeping. This "swap" is handled here.  
 */


  if(rf_path==RF_PATH_0)
  {
#ifdef T_MSM6500
   adie_path_register=0x19;
#else
#error code not present
#endif
  } else

    if(rf_path==RF_PATH_1)
  {
#ifdef T_MSM6500
   adie_path_register=0x09;
#else
#error code not present
#endif
  }


   if (wakeup == ADIE_ENABLE)
   {

     #ifdef T_MSM6500

     /* configure the ADIE clock regime based on rf_path, enable/disable,
        and DIFFERENTIAL vs. SINGLE ENDED feature */
     adie_clk_regime_config(rf_path, ADIE_ENABLE);

     #endif /* T_MSM6500 */
     
     /* wake up the BBRX core, turn on channel 0 */
     /* Don't wake up BBRx1 , GPS will do it if needed */
     if ( adie_bbrx_write_verify( adie_path_register,
                            BBRX_EN1_SW_SLEEP_M,
                            BBRX_EN1_ACTIVE_MODE_V ) == ADIE_FAILURE )
     {

       MSG_ERROR( "The BBRX wakeup value has not been written correctly", 0, 0, 0 );
     }

   } else /* DISABLE */ {

     /* power down the BBRX core channel 0*/
     if( adie_bbrx_write_verify ( adie_path_register,
                            BBRX_EN1_SW_SLEEP_M,
                            BBRX_EN1_POWERDOWN_V) == ADIE_FAILURE )
     {
       MSG_ERROR( "The BBRX sleep value has not been written correctly", 0, 0, 0 );
     }

  #ifdef T_MSM6500

     /* configure the ADIE clock regime based on rf_path, enable/disable,
        and DIFFERENTIAL vs. SINGLE ENDED feature */
     adie_clk_regime_config(rf_path, ADIE_DISABLE);

     #endif /* T_MSM6500 */


   }
}


/*===========================================================================

FUNCTION      adie_codec_wakeup

DESCRIPTION

  This function wakes up or puts to sleep the CODEC core of the ADIE

PARAMETERS

  wakeup- whether the core is to be put to sleep or be woken up

DEPENDENCIES

  sbi_init() must have been called. (for sbi communication targets)
  adie_init() must have been called.


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void adie_codec_wakeup (adie_op_type wakeup)
{ 
  if (wakeup == ADIE_ENABLE)
  {
#ifndef T_QSC60X5
#error code not present
#else
    if( ADIE_OUTM_VERIFY( ADIE_GPR_MS2_REGISTER_R, 
                          ADIE_GP2_MS2_CODEC_EN_M,
                          ADIE_GP2_MS2_CODEC_EN_ENA_V) == ADIE_FAILURE )
#endif /* T_QSC60X5 */
    {
      MSG_ERROR("The CODEC wakeup value has not been written correctly", \
                0, 0, 0);
    }

  }
  else /* if wakeup is ADIE_DISABLE */
  {
#ifndef T_QSC60X5
#error code not present
#else
    if( ADIE_OUTM_VERIFY( ADIE_GPR_MS2_REGISTER_R, 
                          ADIE_GP2_MS2_CODEC_EN_M,
                          ADIE_GP2_MS2_CODEC_EN_DIS_V) == ADIE_FAILURE )
#endif /* T_QSC60X5 */
    {
      MSG_ERROR("The CODEC sleep value has not been written correctly", \
                0, 0, 0);
    }
  }

}



/*===========================================================================

FUNCTION adie_wakeup

DESCRIPTION
  This function wakes up the ADIE or allows it to sleep.  The ADIE only goes to
  sleep if none of its hardware blocks are being used.

DEPENDENCIES
  sbi_init() must have been called.
  adie_init() must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void adie_wakeup (adie_op_type action, adie_hwblock_type req_blk)
{


  switch (req_blk) {

    case ADIE_BBRX_BLK :

      return;
      adie_bbrx_wakeup(RF_PATH_0, action );

      break;

    case ADIE_HKADC_BLK :
      if (action == ADIE_ENABLE)
      {
        adie_adc_wakeup();
      }
      else
      {
        adie_adc_sleep();
      }
      break;

  case ADIE_CODEC_BLK :
      return;

      adie_codec_wakeup( action );

      break;

  #ifdef FEATURE_XO
    case ADIE_XOTHERM_BLK:
      /* Does nothing */
      break;
  #endif /* FEATURE_XO */
      

    default :
      MSG_ERROR("Non-existent ADIE block",0,0,0);
  }

} /* end of adie_wakeup */



/*===========================================================================

FUNCTION adie_codec_read

DESCRIPTION
  This function read codec register.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void adie_codec_read
(
  byte reg,
  byte mask,
  byte *value
)
{
  if (value==NULL)
    { return; }

  if (ADIE_IN(reg, value) == ADIE_FAILURE)
  {
    MSG_ERROR("Codec read failure %x %x %x", reg, mask, *value);
    return;
  }

  *value &= mask;
}


/*===========================================================================

FUNCTION adie_codec_write

DESCRIPTION
  This function write codec register.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void adie_codec_write
(
  byte reg,
  byte mask,
  byte value
)
{
  if (ADIE_OUTM_VERIFY(reg, mask, value) == ADIE_FAILURE)
  {
    MSG_ERROR("Codec write failure %x %x %x", reg, mask, value);
  }
}


/*===========================================================================

FUNCTION adie_codec_enable                                     EXTERNALIZED

DESCRIPTION
  This function enable CODEC_OUT bit in GPR register [4].

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void adie_codec_enable (adie_op_type wakeup)
{
#ifndef T_QSC60X5 
#error code not present
#else
  /* No equivalent functionality on RaMSIS */
  return;
#endif /* T_QSC60X5 */

}

/*===========================================================================

FUNCTION adie_codec_config

DESCRIPTION
  This function for writing the codec config array to Adie

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void adie_codec_config
(
  uint8               size,
  adie_codec_reg_type *codec_func_ctrl
)
{
  uint8  i;

  for (i=0;i<size;i++)
  {
    if (adie_write_verify( codec_func_ctrl[i].reg, 
                           codec_func_ctrl[i].mask,
                           codec_func_ctrl[i].val) == ADIE_FAILURE)
    {
      MSG_ERROR("Adie codec config SBI write bytes fail",0,0,0);
      break;
    }

    if (codec_func_ctrl[i].delay_us > 0)
    {
      clk_busy_wait(codec_func_ctrl[i].delay_us);
    }
  }

}
   
#ifdef T_MSM6500
/*===========================================================================

FUNCTION adie_clk_regime_config

DESCRIPTION
  This function configures the ADIE clock regime to either DIFFERENTIAL or
  SINGLE ENDED based on FEATURE_DIFFERENTIAL_RX_MOD_CLK.

  It will only configure the appropriate clock regime (RX0 or RX1) based
  on the device passed in, and it will enable the regime or disable
  the regime based on the wakeup parameter passed in.

DEPENDENCIES
  This function should be called to enable the regime before the registers
  on the ADIE are being accessed, and it should be called to disable
  the regime after the registers have been accessed.

RETURN VALUE
  None

SIDE EFFECTS
  turns on and off the clock regime.

===========================================================================*/
void adie_clk_regime_config(rf_path_enum_type path, adie_op_type wakeup)
{
  byte adie_clk_regime = CLK_RGM_MISC_ENA_CH0_ADIE_SE_EN;
  byte adie_clk_regime_analog = CLK_RGM_ANALOG0_M;

  return;

  switch ( path )
  {
    case RF_PATH_0:
    {

      #ifdef FEATURE_DIFFERENTIAL_RX_MOD_CLK
      adie_clk_regime = CLK_RGM_MISC_ENA_CH0_ADIE_DIFF_SEL;
      #else /* Single Ended */
      adie_clk_regime = CLK_RGM_MISC_ENA_CH0_ADIE_SE_EN;
      #endif

      adie_clk_regime_analog = CLK_RGM_ANALOG0_M;

      break;
    }
    case RF_PATH_1:
    {

      #ifdef FEATURE_DIFFERENTIAL_RX_MOD_CLK
      adie_clk_regime = CLK_RGM_MISC_ENA_CH1_ADIE_DIFF_SEL;
      #else /* Single Ended */
      adie_clk_regime = CLK_RGM_MISC_ENA_CH1_ADIE_SE_EN;
      #endif

      adie_clk_regime_analog = CLK_RGM_ANALOG1_M;

      break;
    }
  }

  if (wakeup == ADIE_ENABLE)
  {
    clk_regime_misc_enable(adie_clk_regime);
    clk_regime_enable(adie_clk_regime_analog);
  }
  else /* ADIE_DISABLE */
  {
    clk_regime_misc_disable(adie_clk_regime);
    clk_regime_disable(adie_clk_regime_analog);
  }
} /* adie_clk_regime_config */
#endif /* T_MSM6500 */

#ifdef ADIE_TEST
void adie_test(void)
{
    adie_codec_reg_type codec_array[]={0x48, 0xff, 0x55, 0,
                                       0x4F, 0xff, 0x11, 0,
                                       0x96, 0xff, 0xAA, 0,
                                       0xA0, 0xff, 0,    0 };


    /* Execute codec APIs */
    adie_codec_wakeup(ADIE_DISABLE);
    adie_codec_config(sizeof(codec_array), codec_array);
    adie_codec_enable(ADIE_ENABLE);
    adie_codec_wakeup(ADIE_ENABLE);
    adie_codec_wakeup(ADIE_DISABLE);
}
#endif /* ADIE_TEST */

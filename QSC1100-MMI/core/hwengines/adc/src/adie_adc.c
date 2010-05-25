/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     A D I E    A D C 

GENERAL DESCRIPTION
  This file contains support for the ADC related API's for the adc on the ADIE  
 


EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2000, 2001, 2002 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2006             by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/bbrx.c_v   1.7   12 Jul 2002 17:19:40
  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/src/adie_adc.c#3 $ $DateTime: 2003/10/09 18:33:3

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/22/08   jdt     Clean up ULC ADC driver
09/25/08   jdt     Fix for ADC read returning incorrect values
07/20/08   vm      Fixed HKADC Driver for QSC1100
04/03/08   mj      ULC updates
08/14/07   gb      Branched from SC2x for LCU
06/16/07   abh     Changes to support XO Thermsitor ADC.
01/29/07   dyc     Updated with ADIE_ADC_SBI_DEBUG feature.
12/14/06   ans     Changes for SC2x
04/13/06   ty      Changes to support FEATURE_HDET_USES_DM
11/14/05   ty      Disable HKADC digital outputs when going to sleep
07/20/05   lp      Added timeout variable when reading ADC status register
06/16/05   lp      Removed #include "sbi.h"
06/07/05   lp      Added ADC changes
03/22/05   ty      Made changes to support new TSHKADC for new ADIE
12/05/03   et      removed #include "sbi.h"
11/10/03   et      added support for MM adie_read_bytes- fixed rf cal bug
11/04/03   et      changed FEATURE_MM_ADC to FEATURE_ADIE_HAS_MM_ADC (merged change)
11/03/03   et      merged fix from 6250 archive
10/20/03   et      initial revision
============================================================================*/



#include "target.h"

#include "adie_adc.h"

#include "clk.h"
#include "adie_sbi.h"
#include "msg.h"

#ifdef ADIE_ADC_SBI_DEBUG
#error code not present
#endif /* ADIE_ADC_SBI_DEBUG */

#ifdef FEATURE_ADC_MULTI_RESOLUTION

/*===========================================================================

FUNCTION adie_adc_set_res                          EXTERNAL FUNCTION


DESCRIPTION
    This function will set the ADC up to the resolution requested for 
  subsequent ADC conversions. 

INPUT PARAMETERS
     Parameter name: adie_adc_res_type.
     - This is the requested resolution for the ADC block

RETURN VALUE
  None
     
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) adie_init()

SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the ADIE.

===========================================================================*/
void adie_adc_set_res(adie_adc_res_type res)
{
  INTLOCK();
  switch(res)
  {
    case ADIE_ADC_RES_12_BITS:
      ADIE_OUTM(ADIE_HKADC_PARAM2_R, 
                  ADIE_HKADC_PARAM2_RES_M, 
                    ADIE_HKADC_PARAM2_RES_12_BITS_V);
      break;
    case ADIE_ADC_RES_10_BITS:
      ADIE_OUTM(ADIE_HKADC_PARAM2_R, 
                  ADIE_HKADC_PARAM2_RES_M, 
                    ADIE_HKADC_PARAM2_RES_10_BITS_V);
      break;
    case ADIE_ADC_RES_8_BITS:
      ADIE_OUTM(ADIE_HKADC_PARAM2_R, 
                  ADIE_HKADC_PARAM2_RES_M, 
                    ADIE_HKADC_PARAM2_RES_8_BITS_V);
      break;
    default:
      break;
  }
  INTFREE();
}

#endif /* FEATURE_ADC_MULTI_RESOLUTION */

/*===========================================================================

FUNCTION      adie_adc_read_status

DESCRIPTION
  This function check the status of ADC conversion. It will retry once if
  status read failed.

PARAMETERS
  none

DEPENDENCIES
  This function must be called after adie_init_adc is called and after
  ADC conversion is started.

RETURN VALUE
  TRUE when ADC conversion is done.
  FALSE when error during HKADC_STATUS register read or timeout.

SIDE EFFECTS
  none

===========================================================================*/
boolean adie_adc_read_status()
{
   uint8   statusdata   = 0;
   uint8   counter      = 0;

   /* wait for conversion to finish */
   clk_busy_wait( ADIE_HKADC_READ_CONVERSION_TIME );

   /* read the EOC status up to two times */
   for ( counter = 0; counter < ADIE_HKADC_READ_CONVERSION_MAX_ATTEMPTS; counter++ )
   {
      clk_busy_wait(1);

      /* read ADC conversion status */
      if ( ADIE_IN( ADIE_HKADC_EOC_R_REG, &statusdata )
           == ADIE_SUCCESS )
      {
         /* if ADC conversion is done */
         if ( ( statusdata & ADIE_HKADC_EOC_R_REG_MASK )
                == ADIE_HKADC_DONE_VAL )
         {
            return TRUE;
         }
      }
      else
      {
         MSG_LOW( "SBI Read Failure for ADC_STATUS", 0, 0, 0 );
      }
   }

   return FALSE;

}


#if defined(ADIE_HAS_ADC) && !defined(FEATURE_ADIE_HAS_MM_ADC)
/*===========================================================================

FUNCTION      adie_adc_read

DESCRIPTION
  This function performs ADC conversion and returns an 8-bit result.

PARAMETERS
  ad_conf_par:
    - muxsel: Which mux we want to convert.
    - vref:   The voltage reference for the specified mux.

  adc_value: Use this pointer to return the AD conversion result,

DEPENDENCIES
  This function must be called after adie_init is called.

RETURN VALUE
  TRUE   - AD Conversion was successful.
  FALSE  - AD Conversion was failed.

SIDE EFFECTS
  Interrupts are locked during this function

===========================================================================*/
boolean adie_adc_read(adie_adc_config_type ad_conf_par, uint16 *adc_value )
{

   adie_data_type  convData   = 0;     /*The ADC conversion result.    */
   boolean         convFlag   = FALSE; /*Flag, FALSE= conversion fail
                                             Flag, TRUE = conversion
                                             successful.                   */
   adie_data_type  convStatus = 0;     /*Where we keep track if the
                                             conversion was successful or
                                             not.                          */
   adie_addr_type  convAddr   = 0;     /*The adie reg address that we
                                             want to read from.            */
#ifdef FEATURE_ADC_MULTI_RESOLUTION
  adie_data_type  convRes    = 0;
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
 
  int convStatusCounter = 0;
  uint8 temp_reg;

   /* Disable interrupts */
   INTLOCK();

   adie_adc_wakeup( );

   /* Do we have a valid mux for this signal */
   if ((ad_conf_par.muxsel != ADIE_AD_MUX_NONE) && (adc_value != NULL))
   {
      temp_reg = (ad_conf_par.muxsel << ADIE_ADC_MUXSEL_SHIFT_VAL) & ADIE_ADC_MUXSEL_W_REG_MASK;

      if ( ad_conf_par.muxsel == ADIE_AD_PDET )
      {
         temp_reg |= (ADIE_HKADC_ANA_CONG_REF_SEL_INTR_TX_V & ADIE_HKADC_ANA_CONG_REF_SEL_M);
      }
      else
      {
         temp_reg |= (ADIE_HKADC_ANA_CONG_REF_SEL_VDD_V & ADIE_HKADC_ANA_CONG_REF_SEL_M);
      }
      ADIE_OUTM(ADIE_HKADC_ANA_CONG_R, (ADIE_ADC_MUXSEL_W_REG_MASK | ADIE_HKADC_ANA_CONG_REF_SEL_M), temp_reg);

     /* Wait for warm up and conversion time */
    clk_busy_wait( ADIE_HKADC_POWER_UP_CONVERSION_TIME);

    #ifdef ADIE_ADC_SBI_DEBUG
#error code not present
    #endif /* ADIE_ADC_SBI_DEBUG */

    /* Read the ADC converstion status and make sure that the
           SBI was successful reading the register */
    convAddr = ADIE_HKADC_EOC_R_REG;
    
    do
    {
      clk_busy_wait(1);
      ADIE_IN(convAddr,&convStatus);
      ++convStatusCounter;
    } while (((convStatus & ADIE_HKADC_EOC_R_REG_MASK)!=ADIE_HKADC_DONE_VAL) && (convStatusCounter < 5) );

    
            /* Is the ADC done? */
      if ( (convStatus & ADIE_HKADC_EOC_R_REG_MASK) == ADIE_HKADC_DONE_VAL)
      {
                /* if we are done then read the ADC result */
#ifdef FEATURE_ADC_MULTI_RESOLUTION
        convAddr = ADIE_HKADC_RES_R;
        if (ADIE_IN(convAddr, &convRes) == ADIE_SUCCESS)
        {    
          convRes &= ADIE_HKADC_RES_M;
          convAddr = ADIE_ADC_DATA_MSB_R_REG;
          if (ADIE_IN(convAddr, &convData) == ADIE_SUCCESS)
          {
            switch(convRes)
            {
              case ADIE_HKADC_RES_8_BITS_V:
                /* If 8 bit conversion, ADIE_ADC_DATA_MSB_R_REG has the answer
                 * otherwise, need to multiplex it manually
                 */
                *adc_value = (uint16) (convData & ADIE_ADC_DATA_MSB_R_MASK);
                    convFlag = TRUE;
                break;
              case ADIE_HKADC_RES_10_BITS_V:
                *adc_value = (uint16) (
                                    ((convData & ADIE_ADC_DATA_MSB_R_MASK)
                                     << 0x02 |
                                    (((convStatus & ADIE_ADC_DATA_LSB_R_MASK)
                                     >> ADIE_ADC_DATA_LSB_SHIFT_VAL) & 0x03) ));
                *adc_value &= ADIE_ADC_DATA_10BITS_MASK;
                    convFlag = TRUE;
                break;
              case ADIE_HKADC_RES_12_BITS_V:
                *adc_value = (uint16) (
                                    ((convData & ADIE_ADC_DATA_MSB_R_MASK)
                                     << ADIE_ADC_DATA_LSB_SHIFT_VAL) |
                                    ((convStatus & ADIE_ADC_DATA_LSB_R_MASK)
                                     >> ADIE_ADC_DATA_LSB_SHIFT_VAL) );
                *adc_value &= ADIE_ADC_DATA_12BITS_MASK;
                convFlag = TRUE;
                break;
              default:
                /* error */
                convFlag = FALSE;
                break;
             }
           }
        }
#else /* FEATURE_ADC_MULTI_RESOLUTION */
        convAddr = ADIE_HKADC_DATA_RD_R;
        if (ADIE_IN(convAddr, &convData) == ADIE_SUCCESS)
        {
          *adc_value = (uint16)convData;
          convFlag = TRUE;
        }
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      }

   }
   /* Enable interrupts */
   INTFREE();


   if (!convFlag)
   {
    MSG_HIGH("The ADC failed to read, re-initialize", 0, 0, 0);
    /* Re-initialize the ADC in hopes it will work next time */
    adie_init_adc();
   }


   /* Return if we were successful or not */
   return convFlag;
}
#endif /* ADIE_HAS_ADC && !FEATURE_ADIE_HAS_MM_ADC */


#if defined(FEATURE_ADIE_HAS_MM_ADC)
#error code not present
#endif /* FEATURE_ADIE_HAS_MM_ADC */

/*===========================================================================
FUNCTION adie_init_adc                        INTERNAL FUNCTION

DESCRIPTION
    This function initializes the TSHKADC component of the ADIE.
  
INPUT PARAMETERS
  None.
  
RETURN VALUE
  None.

DEPENDENCIES
  Call the following functions before calling this function:
  1) adie_init()

SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with ADIE.

===========================================================================*/
void adie_init_adc()
{
  /* --------------------------------------------- */
  /* Initialize the TSHKADC via SBI                */
  /* --------------------------------------------- */

#ifdef ADIE_ADC_SBI_DEBUG
#error code not present
#endif
  
  INTLOCK();

  
  /* some of these register initializations are not absolutely necessary
   * because the default values are good enough. 
   */

  ADIE_OUTM(ADIE_HKADC_SETUP_R, ADIE_HKADC_SETUP_M, ADIE_HKADC_SETUP_INIT_V);
  ADIE_OUTM(ADIE_HKADC_ANA_CONG_R, ADIE_HKADC_ANA_CONG_M, ADIE_HKADC_ANA_CONG_INIT_V);
  ADIE_OUTM(ADIE_HKADC_PARAM_R, ADIE_HKADC_PARAM_M, ADIE_HKADC_PARAM_INIT_V);
  ADIE_OUTM(ADIE_HKADC_PARAM2_R, ADIE_HKADC_PARAM2_M, ADIE_HKADC_PARAM2_INIT_V);


 #ifdef ADIE_ADC_SBI_DEBUG
#error code not present
#endif /* ADIE_ADC_SBI_DEBUG */

  adie_adc_wakeup( );

  /* check the setup register to see if it is set up properly */ 
  if ( ADIE_OUTM_VERIFY_I( ADIE_HKADC_SETUP_R,
                         ADIE_HKADC_SETUP_M,
                       /*Not needed for Saturn 2*/
                           ADIE_HKADC_SETUP_INIT_V
/*                         ADIE_TSHK_SETUP_INIT_V | 0x20*/
                       ) == ADIE_FAILURE )
  {
    MSG_ERROR( "The ADC was not setup correctly", 0, 0, 0 );
  }

  INTFREE();
}

/*===========================================================================

FUNCTION adie_adc_sleep                           EXTERNAL FUNCTION


DESCRIPTION
    This function will disable the ADC module 
    
INPUT PARAMETERS

RETURN VALUE

DEPENDENCIES
  The following function must have been called:
  1) adie_init()

SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the ADIE.

===========================================================================*/
void adie_adc_sleep()
{
  INTLOCK();

  /* disable HKADC block */
  ADIE_OUTM(ADIE_HKADC_SETUP_R, 
              ADIE_HKADC_SETUP_ADC_M, 
                ADIE_HKADC_SETUP_ADC_DIS_V);

#if 0
  /* disable TCXO output for 1st mixed signal block */
  ADIE_OUTM(ADIE_TCXO_BUFR_CTRL_REG_R, 
            ADIE_TCXO_BUFR_CTRL_REG_MS_CLK_M, 
            ADIE_TCXO_BUFR_CTRL_REG_MS_CLK_DIS_V);
#endif

  INTFREE();
}

/*===========================================================================

FUNCTION adie_adc_wakeup                           EXTERNAL FUNCTION


DESCRIPTION
    This function will enable the ADC module 
    
INPUT PARAMETERS

RETURN VALUE

DEPENDENCIES
  The following function must have been called:
  1) adie_init()

SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the ADIE.

===========================================================================*/
void adie_adc_wakeup()
{
  /* SBI registers need to be set up again if powered down 
   * using RESIN_N 
   */
  INTLOCK();

#if 0
   pm_vreg_set_level(PM_VREG_CDC2_ID, 1300); // 1.3 v
   pm_vreg_set_level(PM_VREG_RFA_ID, 2200); // 2.2 v
  (void)pm_vreg_control(PM_ON_CMD, PM_VREG_RFA_M);
#endif

#if 0
  ADIE_OUTM(ADIE_TCXO_BUFR_CTRL_REG_R, 0xff, 0xA4);
#endif

  ADIE_OUTM(ADIE_HKADC_SETUP_R, 
              ADIE_HKADC_SETUP_ADC_M, 
                ADIE_HKADC_SETUP_ADC_EN_V);

  INTFREE();
}

#ifdef ADIE_ADC_SBI_DEBUG
#error code not present
#endif /* ADIE_ADC_SBI_DEBUG */

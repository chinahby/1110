#ifndef ADIE_ADC_H
#define ADIE_ADC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     A D I E    A D C

GENERAL DESCRIPTION
  This file contains support for the adc related API's for the adc on the LCU ADIE


 
EXTERNALIZED FUNCTIONS
  None.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2000 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/bbrx.c_v   1.7   12 Jul 2002 17:19:40
  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/src/adie_adc.h#3 $ $DateTime: 2003

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/22/08   jdt     Clean up ULC ADC driver
04/03/08   mj      ULC updates
08/14/07   gb      Branched from SC2x for LCU
06/19/20   dyc     Added prototype adie_init_xotherm_adc()
02/13/07   va      Update masks for HK ADC
12/14/06   ans     Changes for SC2x
05/22/06   hkk     Common Adie driver for 1x and UMTS 
04/13/06   ty      Changes to support FEATURE_HDET_USES_DM
10/25/05   ty      Correct channel assignments
03/29/05   ty      Made changes to support TSHKADC on new ADIE
11/11/03   et      protected this file
11/10/03   et      removed "static" from global declarations used outside the file
11/04/03   et      changed FEATURE_MM_ADC to FEATURE_ADIE_HAS_MM_ADC (merged change)
10/20/03   et      initial revision
============================================================================*/

#include "comdef.h"
#include "target.h"
#include "adie_adcdefs.h"
#include "adie_cdcdefs.h"  
#include "adie_regdefs.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* TSHKADC Analog Input Selection */
#define ADIE_ADC_MUXSEL_W_REG           ADIE_HKADC_ANA_CONG_R
#define ADIE_ADC_MUXSEL_W_REG_MASK      ADIE_HKADC_ANA_CONG_MUX_SEL_M

#define ADIE_ADC_MUXSEL_SHIFT_VAL       4

#define ADIE_HKADC_STATUS_R             ADIE_HK_ADC_STATUS_R
#define ADIE_HKADC_DATA_RD_R            ADIE_HK_ADC_DATA_RD_R
#define ADIE_HKADC_DATA_RD_M            ADIE_HK_ADC_DATA_RD_M

#ifdef FEATURE_ADC_MULTI_RESOLUTION
  #define ADIE_HKADC_RES_R                ADIE_HKADC_PARAM2_R
  #define ADIE_HKADC_RES_M                ADIE_HKADC_PARAM2_RES_M
  #define ADIE_HKADC_RES_8_BITS_V         ADIE_HKADC_PARAM2_RES_8_BITS_V
  #define ADIE_HKADC_RES_10_BITS_V        ADIE_HKADC_PARAM2_RES_10_BITS_V
  #define ADIE_HKADC_RES_12_BITS_V        ADIE_HKADC_PARAM2_RES_12_BITS_V
#endif /* FEATURE_ADC_MULTI_RESOLUTION */

/* TSHKADC End of Conversion Bit 
   0 -> Conversion Done, or idle
   1 -> Conversion Done, auto revert to 0

   However, for HKADC operation, the interpretation is:
   0 -> Conversion In Progress
   1 -> Conversion Done
*/
#define ADIE_HKADC_EOC_R_REG            ADIE_HKADC_STATUS_R
#define ADIE_HKADC_EOC_R_REG_MASK       ADIE_HKADC_STATUS_ADC_EOC_M
#define ADIE_HKADC_DONE_VAL             ADIE_HKADC_STATUS_ADC_EOC_CONV_DONE_V

/* A write to this register starts a conversion */
#define ADIE_ADC_START_W_REG            ADIE_HKADC_COMMAND_R

#ifdef FEATURE_ADC_MULTI_RESOLUTION
  /* This register holds the TSHKADC conversion data result, MSB */
  #define ADIE_ADC_DATA_MSB_R_REG         ADIE_HKADC_DATA_RD_R
  #define ADIE_ADC_DATA_MSB_R_MASK        ADIE_HKADC_DATA_RD_M
  /* This register holds the TSHKADC conversion data result, LSB (4 bits */
  #define ADIE_ADC_DATA_LSB_R_REG         ADIE_HKADC_STATUS_R
  #define ADIE_ADC_DATA_LSB_R_MASK        ADIE_HKADC_STATUS_DATA_M
  #define ADIE_ADC_DATA_LSB_SHIFT_VAL     4

  #define ADIE_ADC_DATA_8BITS_MASK        0xFF
  #define ADIE_ADC_DATA_10BITS_MASK       0x3FF
  #define ADIE_ADC_DATA_12BITS_MASK       0xFFF
#endif /* FEATURE_ADC_MULTI_RESOLUTION */



#ifdef FEATURE_HDET_USES_DM
#error "this feature should not be defined"
/**********************************************************************************/
/*Command MDSP uses to perform HDET read, ADIE_AD_AIN2 is the input channel number*/
/*it will need to be modified if input channel changes                            */
/**********************************************************************************/

#define  ADIE_MDSP_HDET_CMD_DATA ( (ADIE_TSHK_INTF_CONFIG_R<<16) | (ADIE_AD_AIN2<<4) )
#endif


/********************** MULTI-MODE ADIE HKADC DEFINITIONS ******************/

/* TSHKADC power-up time calculated with 2.4MHz clock
 *   5 uS for warm up
 *   30 uS for extra padding time on the ATE 
 */
#define ADIE_HKADC_POWER_UP_CONVERSION_TIME               5+30

/* TSHKADC conversion time calculated with 2.4MHz clock
 *   20 uS for Tsampling
 *   1+5 uS for Tacquisition and number of bits
 */
#define ADIE_HKADC_READ_CONVERSION_TIME                   20+1+5

/* Number of ADC conversion status read retry */
#define ADIE_HKADC_READ_CONVERSION_MAX_ATTEMPTS           3

/* This enum defines all of the HKADC analog inputs for conversion */
typedef enum
{
  ADIE_AD_MUX_NONE,    /* None           */
  ADIE_AD_AIN0,    /* Analog input 1 */
  ADIE_AD_MUX1 = ADIE_AD_AIN0,
  ADIE_AD_AIN1,    /* Analog input 2 */
  ADIE_AD_MUX2 = ADIE_AD_AIN1,
  ADIE_AD_AMUXIN,    /* Analog input 3 */
  ADIE_AD_MUX3 = ADIE_AD_AMUXIN,
  ADIE_AD_PDET,   /* Analog input 4 */
  ADIE_AD_MUX4 = ADIE_AD_PDET,
  ADIE_AD_R_MEAS,   /* Analog input 5 */
  ADIE_AD_MUX5 = ADIE_AD_R_MEAS,
  ADIE_AD_INT_TEMP_SENSOR,   /* Analog input 9 */
  ADIE_AD_MUX9 = ADIE_AD_INT_TEMP_SENSOR,
  ADIE_AD_MUX_INVALID
}adie_hkadc_muxsel_type;

/* This enum defines all of the HKADC voltage reference options */
/* TSHKADC does not support other voltage refs */
typedef enum
{
    ADIE_AD_VREF_VDDA,      /* VDD internal          */
    ADIE_AD_VREF_AIN0,
    ADIE_AD_VREF_TX_INT,
    ADIE_AD_VREF_NONE = 0xff
}adie_adc_vref_type;

typedef struct
{
  adie_hkadc_muxsel_type muxsel;
  adie_adc_vref_type   vref;
}adie_adc_config_type;

#ifdef FEATURE_ADC_MULTI_RESOLUTION
typedef enum
{
  ADIE_ADC_RES_8_BITS,
  ADIE_ADC_RES_10_BITS,
  ADIE_ADC_RES_12_BITS,
  ADIE_ADC_RES_INVALID
}adie_adc_res_type;

#ifdef FEATURE_ADC_12BIT_CALIBRATION
#define ADIE_ADC_CALIBRATION_RESOLUTION ADIE_ADC_RES_12_BITS
#else
#define ADIE_ADC_CALIBRATION_RESOLUTION ADIE_ADC_RES_8_BITS
#endif /* FEATURE_ADC_12BIT_CALIBRATION */
#endif /* FEATURE_ADIE_MULTI_RESOLUTION */

#ifdef FEATURE_ADIE_HAS_MM_ADC
#error code not present
#endif  /* FEATURE_ADIE_HAS_MM_ADC */


/**************************FUNCTIONS***************************************/

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
boolean adie_adc_read( adie_adc_config_type ad_conf_par, uint16* adc_value );

/*===========================================================================

FUNCTION      adie_adc_read_status

DESCRIPTION
  This function check the status of ADC conversion. It will retry once if
  status read failed.

PARAMETERS
  none

DEPENDENCIES
  This function must be called after adie_init is called and after
  ADC conversion is started.

RETURN VALUE
  TRUE when ADC conversion is done.
  FALSE when error during ADC_STATUS register read or timeout.

SIDE EFFECTS
  none

===========================================================================*/
boolean adie_adc_read_status(void);


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
void adie_init_adc(void);


/*===========================================================================

FUNCTION adie_adc_set_res                          EXTERNAL FUNCTION


DESCRIPTION
    This function will set the ADC up to the resolution requested for 
  subsequent ADC conversions. 

INPUT PARAMETERS
     Parameter name: adie_adc_res_type.
     - This is the requested resolution for the ADC block
     - Valid Inputs:
          ADIE_ADC_RES_8_BITS
          ADIE_ADC_RES_10_BITS
          ADIE_ADC_RES_12_BITS
          ADIE_ADC_RES_INVALID -> causes no change, and returns current setting
          
RETURN VALUE
  None

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) adie_init()

SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the ADIE.

===========================================================================*/
#ifdef FEATURE_ADC_MULTI_RESOLUTION
void adie_adc_set_res(adie_adc_res_type res);
#endif /* FEATURE_ADC_MULTI_RESOLUTION */

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
void adie_adc_wakeup(void);

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
void adie_adc_sleep(void);

#ifdef FEATURE_ADIE_HAS_MM_ADC
#error code not present
#endif /* FEATURE_ADIE_HAS_MM_ADC */

#endif /* ADIE_ADC_H */


#ifndef PM66XXADC_H
#define PM66XXADC_H

/*===========================================================================
                  P M 6 6 X X A D C   H E A D E R    F I L E

                  ADC (ANALOG MUX & SCALING CIRCUIT) DRIVER

DESCRIPTION
   This file contains functions prototypes and variable/type/constant
   declarations for supporting ADC services for the Qualcomm PM66XX chips.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/device/main/latest/driver/inc/pm66XXadc.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/21/08   jtn     Removed pm_adc_muxsel_prescaler_info from this file, added
                   prototype for pm_adc_get_prescalar() API
10/24/05   cng     Added note in pm_adc_config_mux function protoype for 
                   Panoramix which does not have output scaling circuitry 
03/23/05   Vish    Added curly brace to delimit each row in 2-D uint8 array
                   definition of 'pm_adc_muxscale_type'.
06/25/04   Vish    Added function [pm_adc_in_ref_ch_1p25V_0p625V_available()]
                   for checking if internal input reference channels 0.625V
                   and 1.25V are available on the current PMIC3 chip.
06/21/04   Vish    Added bypass mode (PM_ADC_MUXSCALE15) for AMUX scaler as
                   one of the available options. 
02/13/04   rmd     Updated comments.
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
01/21/03   rmd     Added pm_adc_can_we_turn_off_the_amux() to find out if
                   we can shut down the PMIC's AMUX depending on the
                   PMIC's Rev.
11/17/03   Vish    Added pm_adc_is_calibration_reqd()
07/21/03   Vish    Created.
===========================================================================*/


/*===========================================================================
                         TYPE DEFINITIONS
===========================================================================*/

/* This enum indicates what channel of the analog MUX to use for AD
   conversion
*/
typedef enum
{
   // Input channel selection for AD conversion  and its built-in pre-scaler
   PM_ADC_MUXSEL0,               // uses 2/3 pre-scaler
   PM_ADC_MUXSEL1,               // uses 1/2 pre-scaler
   PM_ADC_MUXSEL2,               // uses 1/8 pre-scaler
   PM_ADC_MUXSEL3,               // uses NO pre-scaler
   PM_ADC_MUXSEL4,               // uses 1/2 pre-scaler
   PM_ADC_MUXSEL5,               // uses NO pre-scaler
   PM_ADC_MUXSEL6,               // uses NO pre-scaler
   PM_ADC_MUXSEL7,               // uses NO pre-scaler
   PM_ADC_MUXSEL8,               // uses NO pre-scaler
   PM_ADC_MUXSEL9,               // uses NO pre-scaler
   PM_ADC_MUXSEL10,              // uses 2/5 pre-scaler
   PM_ADC_MUXSEL11,              // uses NO pre-scaler
   PM_ADC_MUXSEL12,              // uses NO pre-scaler
   PM_ADC_MUXSEL13,              // uses NO pre-scaler
   PM_ADC_MUXSEL14,              // uses NO pre-scaler
   PM_ADC_NUM_MUXSEL = 0x0f,     //  --- Number of avail. MUX selections ---
   PM_ADC_MUXSEL_NONE = 0x0f     //  --- Powers off MUX ---
}pm_adc_muxsel_type;

/* This enum or the PM_ADC_PRESCALER_NUM/DEN #defines denote the internal pre-scaler
   employed by a particular analog input MUX channel.

   Pre-scaler = prescaler_num / prescaler_den;

   Note: If the analog input voltage is Vin, then the output of the MUX will
         be Vin * (Pre-scaler) = Vin * (prescaler_num / prescaler_den);
*/
typedef struct
{
   uint8   prescaler_num;        // pre-scaler numerator
   uint8   prescaler_den;        // pre-scaler denominator
} pm_adc_muxsel_prescaler_info_type;

/* Pre-scaler information table ...

   Usage: If you want to get the prescaler employed by a paricular MUX input channel,
          say 7, you would use the following ratio:
          PM_ADC_PRESCALER_NUM(PM_ADC_MUXSEL7) / PM_ADC_PRESCALER_DEN(PM_ADC_MUXSEL7)
*/
#define PM_ADC_PRESCALER_NUM(muxSel)   muxSel##_PRESCALER_NUM
#define PM_ADC_PRESCALER_DEN(muxSel)   muxSel##_PRESCALER_DEN

#define PM_ADC_MUXSEL0_PRESCALER_NUM     (2)
#define PM_ADC_MUXSEL0_PRESCALER_DEN     (3)
#define PM_ADC_MUXSEL1_PRESCALER_NUM     (1)
#define PM_ADC_MUXSEL1_PRESCALER_DEN     (2)
#define PM_ADC_MUXSEL2_PRESCALER_NUM     (1)
#define PM_ADC_MUXSEL2_PRESCALER_DEN     (8)
#define PM_ADC_MUXSEL3_PRESCALER_NUM     (1)
#define PM_ADC_MUXSEL3_PRESCALER_DEN     (1)
#define PM_ADC_MUXSEL4_PRESCALER_NUM     (1)
#define PM_ADC_MUXSEL4_PRESCALER_DEN     (2)
#define PM_ADC_MUXSEL5_PRESCALER_NUM     (1)
#define PM_ADC_MUXSEL5_PRESCALER_DEN     (1)
#define PM_ADC_MUXSEL6_PRESCALER_NUM     (1)
#define PM_ADC_MUXSEL6_PRESCALER_DEN     (1)
#define PM_ADC_MUXSEL7_PRESCALER_NUM     (1)
#define PM_ADC_MUXSEL7_PRESCALER_DEN     (1)
#define PM_ADC_MUXSEL8_PRESCALER_NUM     (1)
#define PM_ADC_MUXSEL8_PRESCALER_DEN     (1)
#define PM_ADC_MUXSEL9_PRESCALER_NUM     (1)
#define PM_ADC_MUXSEL9_PRESCALER_DEN     (1)
#define PM_ADC_MUXSEL10_PRESCALER_NUM    (2)
#define PM_ADC_MUXSEL10_PRESCALER_DEN    (5)
#define PM_ADC_MUXSEL11_PRESCALER_NUM    (1)
#define PM_ADC_MUXSEL11_PRESCALER_DEN    (1)
#define PM_ADC_MUXSEL12_PRESCALER_NUM    (1)
#define PM_ADC_MUXSEL12_PRESCALER_DEN    (1)
#define PM_ADC_MUXSEL13_PRESCALER_NUM    (1)
#define PM_ADC_MUXSEL13_PRESCALER_DEN    (1)
#define PM_ADC_MUXSEL14_PRESCALER_NUM    (1)
#define PM_ADC_MUXSEL14_PRESCALER_DEN    (1)


/* This enum is used for selecting the voltage offset and the gain to use on the input
   channel selected.

   NOTE that the voltage offset and the gain are applied after the
   pre-scaler stage.
*/
typedef enum
{                               //   Offset(Volts)    Voltage Gain
                                //   -----------------------------
   PM_ADC_MUXSCALE0,            //        0                1
   PM_ADC_MUXSCALE1,            //        0                1.25
   PM_ADC_MUXSCALE2,            //        0                1.667
   PM_ADC_MUXSCALE3,            //        0                2.5
   PM_ADC_MUXSCALE4,            //        0                5
   PM_ADC_MUXSCALE5,            //        0.5              1.25
   PM_ADC_MUXSCALE6,            //        0.5              1.667
   PM_ADC_MUXSCALE7,            //        0.5              2.5
   PM_ADC_MUXSCALE8,            //        0.5              5
   PM_ADC_MUXSCALE9,            //        1                1.667
   PM_ADC_MUXSCALE10,           //        1                2.5
   PM_ADC_MUXSCALE11,           //        1                5
   PM_ADC_MUXSCALE12,           //        1.5              2.5
   PM_ADC_MUXSCALE13,           //        1.5              5
   PM_ADC_MUXSCALE14,           //        2                5
   PM_ADC_MUXSCALE15,           //        0                1 (Bypass)
   PM_ADC_NUM_MUXSCALES = 0x10  //  ---  Number of avail. MUX scales ---
}pm_adc_muxscale_type;

/* This enum denotes the voltage offset (mV) and gain employed by a
   particular analog MUX scale.

   Note: If the voltage at the output of the MUX is Vmux, then the final
         output after the application of a particular MUX scale will be
         (Vmux - offset_mv) * (gain_num / gain_den);
*/
typedef struct
{
   uint16  offset_mv;       // voltage offset in milli-volts
   uint8   gain_num;        // voltage gain numerator
   uint8   gain_den;        // voltage gain denominator
} pm_adc_muxscale_info_type;


/* Analog MUX information table ...

   Usage: If you want to get the voltage offset (in milli-volts) employed by a
          paricular MUX scale, say 3, you would use PM_ADC_VOFF_MV(PM_ADC_MUXSCALE3).
          On the other hand, if you wanted to know the gain used by say MUX
          scale 6, you would get that by using the following ratio:
          PM_ADC_GAIN_NUM(PM_ADC_MUXSEL6) / PM_ADC_GAIN_DEN(PM_ADC_MUXSEL6)
*/
#define PM_ADC_VOFF_MV(muxScale)    muxScale##_VOFF_MV
#define PM_ADC_GAIN_NUM(muxScale)   muxScale##_GAIN_NUM
#define PM_ADC_GAIN_DEN(muxScale)   muxScale##_GAIN_DEN

#define PM_ADC_MUXSCALE0_VOFF_MV          (0)
#define PM_ADC_MUXSCALE0_GAIN_NUM         (1)
#define PM_ADC_MUXSCALE0_GAIN_DEN         (1)
#define PM_ADC_MUXSCALE1_VOFF_MV          (0)
#define PM_ADC_MUXSCALE1_GAIN_NUM         (5)
#define PM_ADC_MUXSCALE1_GAIN_DEN         (4)
#define PM_ADC_MUXSCALE2_VOFF_MV          (0)
#define PM_ADC_MUXSCALE2_GAIN_NUM         (5)
#define PM_ADC_MUXSCALE2_GAIN_DEN         (3)
#define PM_ADC_MUXSCALE3_VOFF_MV          (0)
#define PM_ADC_MUXSCALE3_GAIN_NUM         (5)
#define PM_ADC_MUXSCALE3_GAIN_DEN         (2)
#define PM_ADC_MUXSCALE4_VOFF_MV          (0)
#define PM_ADC_MUXSCALE4_GAIN_NUM         (5)
#define PM_ADC_MUXSCALE4_GAIN_DEN         (1)
#define PM_ADC_MUXSCALE5_VOFF_MV        (500)
#define PM_ADC_MUXSCALE5_GAIN_NUM         (5)
#define PM_ADC_MUXSCALE5_GAIN_DEN         (4)
#define PM_ADC_MUXSCALE6_VOFF_MV        (500)
#define PM_ADC_MUXSCALE6_GAIN_NUM         (5)
#define PM_ADC_MUXSCALE6_GAIN_DEN         (3)
#define PM_ADC_MUXSCALE7_VOFF_MV        (500)
#define PM_ADC_MUXSCALE7_GAIN_NUM         (5)
#define PM_ADC_MUXSCALE7_GAIN_DEN         (2)
#define PM_ADC_MUXSCALE8_VOFF_MV        (500)
#define PM_ADC_MUXSCALE8_GAIN_NUM         (5)
#define PM_ADC_MUXSCALE8_GAIN_DEN         (1)
#define PM_ADC_MUXSCALE9_VOFF_MV       (1000)
#define PM_ADC_MUXSCALE9_GAIN_NUM         (5)
#define PM_ADC_MUXSCALE9_GAIN_DEN         (3)
#define PM_ADC_MUXSCALE10_VOFF_MV      (1000)
#define PM_ADC_MUXSCALE10_GAIN_NUM        (5)
#define PM_ADC_MUXSCALE10_GAIN_DEN        (2)
#define PM_ADC_MUXSCALE11_VOFF_MV      (1000)
#define PM_ADC_MUXSCALE11_GAIN_NUM        (5)
#define PM_ADC_MUXSCALE11_GAIN_DEN        (1)
#define PM_ADC_MUXSCALE12_VOFF_MV      (1500)
#define PM_ADC_MUXSCALE12_GAIN_NUM        (5)
#define PM_ADC_MUXSCALE12_GAIN_DEN        (2)
#define PM_ADC_MUXSCALE13_VOFF_MV      (1500)
#define PM_ADC_MUXSCALE13_GAIN_NUM        (5)
#define PM_ADC_MUXSCALE13_GAIN_DEN        (1)
#define PM_ADC_MUXSCALE14_VOFF_MV      (2000)
#define PM_ADC_MUXSCALE14_GAIN_NUM        (5)
#define PM_ADC_MUXSCALE14_GAIN_DEN        (1)
#define PM_ADC_MUXSCALE15_VOFF_MV         (0)
#define PM_ADC_MUXSCALE15_GAIN_NUM        (1)
#define PM_ADC_MUXSCALE15_GAIN_DEN        (1)

/* ADS1.2 compiler has problem folding these constants into .c files and that's
   why an alternate representation of these is provided using the extensive
   #defines above !!!
*/
static const pm_adc_muxscale_info_type pm_adc_muxscale_info[] =
{
   {    0, 1, 1 },    // PM_ADC_MUXSCALE0
   {    0, 5, 4 },    // PM_ADC_MUXSCALE1
   {    0, 5, 3 },    // PM_ADC_MUXSCALE2
   {    0, 5, 2 },    // PM_ADC_MUXSCALE3
   {    0, 5, 1 },    // PM_ADC_MUXSCALE4
   {  500, 5, 4 },    // PM_ADC_MUXSCALE5
   {  500, 5, 3 },    // PM_ADC_MUXSCALE6
   {  500, 5, 2 },    // PM_ADC_MUXSCALE7
   {  500, 5, 1 },    // PM_ADC_MUXSCALE8
   { 1000, 5, 3 },    // PM_ADC_MUXSCALE9
   { 1000, 5, 2 },    // PM_ADC_MUXSCALE10
   { 1000, 5, 1 },    // PM_ADC_MUXSCALE11
   { 1500, 5, 2 },    // PM_ADC_MUXSCALE12
   { 1500, 5, 1 },    // PM_ADC_MUXSCALE13
   { 2000, 5, 1 },    // PM_ADC_MUXSCALE14
   {    0, 1, 1 }     // PM_ADC_MUXSCALE15
};

/*===========================================================================
                            FUNCTION PROTOTYPES
===========================================================================*/

/*===========================================================================
FUNCTION pm_adc_config_mux                                  EXTERNAL FUNCTION

DESCRIPTION
   This function sets up the requested analog channel for an AD conversion.
   It does this by selecting the requested channel in the analog MUX (therby
   (selecting the hard-wired "pre-scaler" associated with that channel) and
   by applying the specified voltage "offset" and "gain" at the output
   of the analog MUX which is then fed to the AD converter.

   So the voltage as seen by the AD converter is governed by the following
   equation:
       [(AnalogInputChannelVoltage * "pre-scaler") - "offset"] * "gain"

PARAMETERS
  1) Name: muxSel
     - Analog input channel to be selected in the MUX
     Type: pm_adc_muxsel_type (enum).
     - Valid inputs:
          PM_ADC_MUXSEL0
          PM_ADC_MUXSEL1
          ...
          PM_ADC_MUXSEL14
          PM_ADC_MUXSEL_NONE

          Each of the above channels (except PM_ADC_MUXSCALE_NONE) has its own pre-scaler.
          Use PM_ADC_MUXSCALE_NONE for powering off MUX when not in use.
          Refer to the enum def for more info.
   2) Name: muxScale
      -
      Type: pm_adc_muxscale_type (enum)
      - Valid inputs:
           PM_ADC_MUXSCALE0
           PM_ADC_MUXSCALE1
           ...
           PM_ADC_MUXSCALE15

           Each of the above selection provides for a specific voltage offset and gain.
           Refer to the enum definition for more info.

           **NOTE: For Panoramix, this is a dummy paramter since it does not support
                   output scaling. Thus it can be set to any value.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE     -> Invalid input channel selection
                                            for the MUX.
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE     -> Invalid voltage offset/gain
                                            selection.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this
                                            version of the PMIC.
      PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PMIC failed
      PM_ERR_FLAG__SUCCESS               -> Successful.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_adc_config_mux
(
   pm_adc_muxsel_type muxSel,      // what channel to select for AD conversion
   pm_adc_muxscale_type muxScale   // what offset/scale to use for that
                                   // channel (not applicable to Panoramix) 
);


/*===========================================================================
FUNCTION  pm_adc_is_calibration_reqd                       EXTERNAL FUNCTION

DESCRIPTION
   This function determines if calibration is required for any of the analog
   input channels connected to the MUX.

PARAMETERS
  1) Name: flag_ptr
     Type: boolean*
     - Valid inputs:
          Non-NULL pointer to a valid boolean variable to return the result in.
          If returned result is TRUE, it means that calibration is required
          else it is not.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
        PM_ERR_FLAG__PAR1_OUT_OF_RANGE  ---> NULL pointer provided for flag_ptr
        PM_ERR_FLAG__SUCCESS            ---> Successful (result is passed via flag_ptr)

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_adc_is_calibration_reqd(boolean *flag_ptr);


/*===========================================================================
FUNCTION  pm_adc_can_we_turn_off_the_amux                EXTERNAL FUNCTION

DESCRIPTION
   This function determines if we are allowed to turn off the PMIC's AMUX.

PARAMETERS
  1) Name: flag_ptr
     Type: boolean*
     - Valid inputs:
          Non-NULL pointer to a valid boolean variable to return the result in.
          If returned result is TRUE, it means that we can turn off the PMIC's
          AMUX.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
        PM_ERR_FLAG__PAR1_OUT_OF_RANGE  ---> NULL pointer provided for flag_ptr
        PM_ERR_FLAG__SUCCESS            ---> Successful (result is passed via flag_ptr)

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_adc_can_we_turn_off_the_amux(boolean *flag_ptr);


/*===========================================================================
FUNCTION  pm_adc_in_ref_ch_1p25V_0p625V_available          EXTERNAL FUNCTION

DESCRIPTION
   This function determines if internal input reference channels 0.625V
   and 1.25V are available (on PM_ADC_MUXSEL12 and PM_ADC_MUXSEL13
   respectively).

PARAMETERS
  1) Name: flag_ptr
     Type: boolean*
     - Valid inputs:
          Non-NULL pointer to a valid boolean variable to return the result in.
          If returned result is TRUE, it means that the internal reference
          channels are available.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
        PM_ERR_FLAG__PAR1_OUT_OF_RANGE  ---> NULL pointer provided for flag_ptr
        PM_ERR_FLAG__SUCCESS            ---> Successful (result is passed via flag_ptr)

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_adc_in_ref_ch_1p25V_0p625V_available(boolean *flag_ptr);

/**
 * 
 * @param channel     This is the PMIC ADC channel
 * @param numerator   Pointer to PMIC prescaler numerator.  This variable will hold
 *                    the value of the prescaler numerator if the function completes
 *                    without error.  In case of an error the numerator value is
 *                    undefined.
 * @param denominator Pointer to PMIC prescaler denominator.  This variable will hold
 *                    the value of the prescaler denominator if the function completes
 *                    without error.  In case of an error the denominator value is
 *                    undefined.
 * 
 * @return pm_err_flag_type
 * @example 
 *     uint32 num;
 *     uint32 den;
 *     pm_err_flag_type errFlag;
 * 
 *     errFlag = pm_adc_get_prescalar (PM_ADC_MUXSEL0, &num, &den);
 * 
 *     if(PM_ERR_FLAG__SUCCESS == errFlag)
 *     {
 *      //use the numerator and denominator
 *     }
 */
extern pm_err_flag_type pm_adc_get_prescalar ( 
                       pm_adc_muxsel_type channel ,  
                       uint32 *numerator ,  
                       uint32 *denominator
                     ) ;

#endif /* PM66XXADC_H */

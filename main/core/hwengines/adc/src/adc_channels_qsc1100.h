#ifndef ADC_CHANNELS_SURF6550_H
#define ADC_CHANNELS_SURF6550_H
/*===========================================================================


               A D C   C H A N N E L S   H E A D E R    F I L E
                     F O R   S U R F 6 5 5 0

DESCRIPTION
  This file is included in adc.h by #include ADC_CHANNELS_H, defined in
  target.h

  This file contains publicly defined channel numbers for this target
  as used by the ADC API.

Copyright (c) 2003 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/inc/adc_channels_qsc1100.h#2 $ $DateTime: 2008/12/02 14:23:09 $ $Author: jthielen $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05-14-08   vb     Added the ADC logical channel ADC_PTAT_DEGC. 
04-03-08   mj     ULC updates.
09-28-07   ems    Created From SC2X 3044.
===========================================================================*/

extern void adc_calibrate(void);

/* These are the publicly defined channel numbers for this target. They are used
   to select the channel in adc_channel_definitions[] but have no connection with
   ADC input addresses */
typedef enum
  {
   // Use of RAW readings is strongly discouraged; will be removed as soon as
   // the current applications using them migrate to the converted ones down below
   ADC_BATT_ID,              // RAW reading: Battery ID
   ADC_VBATT,                // RAW reading: Main battery voltage at jack
   ADC_VCHG,                 // RAW reading: Wall charger voltage at jack
   ADC_ICHG,                 // RAW reading: Charger current going into the battery
   ADC_ICHG_OUT,             // RAW reading: Current leaving the battery

   // Converted values
                                   // ADIE_MUX_0 for the following PMIC muxed channels
   ADC_VCOIN_MV,                     // Coin cell battery/capacitor voltage
   ADC_VBATT_MV,                     // Main battery Voltage
   ADC_VCHG_MV,                      // Wall charger voltage at jack
   ADC_ICHG_MV,                      // Voltage across the charger current sense resistor
   ADC_ICHG_OUT_MV,                  // Handset supply voltage (Vdd) determining current
                                     //    leaving the battery
   ADC_BATT_ID_MV,                   // Main battery ID voltage ???
   ADC_BATT_THERM_DEGC,              // Main battery temperature
   ADC_USB_VBUS_MV,                  // USB charger voltage at jack
   ADC_PMIC_THERM_DEGC,              // PMIC die temperature
   ADC_R_EXT,                         
   ADC_HORZ_PP,
   ADC_VERT_PP,
   ADC_PTAT,
   ADC_CTAT,
   ADC_BG,
   ADC_REF,
   ADC_V_DD4,
   ADC_LO1,
   ADC_LO2,
   //     !!!  Following are for ADC's INTERNAL use only  !!!
   //        !!!  Clients of ADC shall NEVER use them  !!!
   ADC_INTERNAL_USE_CHAN_1,
   ADC_INTERNAL_USE_CHAN_2,
   ADC_PA_THERM_DEGC,
   ADC_CHG_THERM_DEGC,
   ADC_HDET_CELL,
   ADC_THERM,
   ADC_MSM_THERM_DEGC,
   ADC_XO_THERM,                    /* Raw SD ADC Reading */
   ADC_XO_THERM_DEGC,               /* Reading converted to degree C */
   ADC_PTAT_DEGC,
   #ifdef FEATURE_LCD_TOUCH_ENABLE
   ADC_TOUCHPAD_X,
   ADC_TOUCHPAD_Y,
   #endif
   // Number of available logical ADC channels
   NUMBER_OF_LOGICAL_ADC_CHANNELS

  } adc_logical_channel_type;

#endif // ADC_CHANNELS_SURF6550_H

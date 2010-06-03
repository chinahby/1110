/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         A N A L O G U E   T O   D I G I T A L   C O N V E R S I O N
              T A R G E T - S P E C I F I C   F U N C T I O N S
                       F O R   S U R F 6 5 5 0

GENERAL DESCRIPTION
  This file contains the target-specific functions and configuration table for
  A/D conversion. This is the second level of a multi-level process.

  The main component of this file is the table adc_logical_channel_definitions
  which configures each logical ADC channel expected by the user code.

  It also contains
  * the wrapper functions to read each physical input of the ADC
    These are responsible for setting up anything involved in reading a
    physical ADC input. This may include
    * Selecting the multiplexer input
    * Setting the reference voltage
    * Starting the read procedure
  * any target-specific functions to filter the readings
    A simple increment/decrement function is included in adc.c but a more
    sophisticated method may be required.
  * any target-specific adc-reading-to-result-units conversion functions
    For example, to suit the slope of a particular thermistor or the
    characteristics of HDET. This can also cope with different reference
    voltages.

EXTERNALIZED FUNCTIONS
  adc_logical_channel_definitions[]
  adc_read_batt_voltage, temp, etc., etc.
  adc_to_degC, etc., etc.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2003 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/src/adc_qsc1100.c#5 $ $DateTime: 2009/05/29 14:29:55 $ $Author: dhendry $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06-29-08   vb      Added 12 bit HKADC support for converstion of all Napoleon Channels 
05-14-08   vb      Added rf_setup_muxes function call to rf-related ADC readings. 
                   Added qsc11x0 ADC APIs.
04-03-08   mj      ULC updates.
06/16/07   abh     Changes to support XO Thermistor ADC
06/01/07   cng     Updated Battery Thermistor Table to detect missing battery
03/16/07   cng     Updated channel mappings for BATT_THERM and CHG_THERM
03/16/07   dyc     Compilation warning fix.
03/01/07   ans     Added changes for HDET therm.
02/09/07   va      Added function for converting XO thermistor ADC to degC.
12/14/06   ans     Changes for SC2x
06-05-06   ty      Fix calibration problem and update batt therm scaling
12-15-04   Vish    Made ADC_GSM_BANDGAP_VREF_MV unavailable as HKAIN[6] is
                   not usable for A/D on MSM6550.
07-06-04    hl     Inherited from 6500 and added support for joystick feature.


-------------- Inherited 6500 revision history below ------------------------
06-28-04   Vish    Replaced the use of AMUX unity gain+zero offset scaler
                   (PM_ADC_MUXSCALE0) with bypass mode (PM_ADC_MUXSCALE15)
                   for better accuracy.
06-01-04   Vish    Reverted back to const definifion of
                   adc_logical_channel_definitions but wrapped under
                   the non-definition of FEATURE_ADC_CALIBRATION.
05-12-04   Vish    Added adc_init(), adc_calibrate() and
                   adc_calibration_data_available_cb() to perform ADC
                   self-calibration at boot-up.
03-26-04   Vish    Removed comments mentioning Qualcomm's internal docs.
11-30-03   jac     featurized pmic vbatt calib
11-17-03   Vish    Made ADC_VBATT_MV's conversion fn to be backwards
                   compatible with respect to PMIC.
11-14-03   Vish    Modified ADC_VBATT_MV's conversion fn to use the calibration
                   data from vbatt driver as opposed to using PMIC's AMUX
                   constants (pre-scaler, voltage offset and gain).
11-06-03   jac     updated default values, for the first buffered read
11-05-03   jac     added ADC_CHG_THERM
10-02-03   jac     Linearily interpolate therm results, changed ADC_THERM to
                   use PA therm
09-12-03   Vish    Moved ADC_BATT_ID and ADC_RES_KYPD around so that
                   ADC_HDET_PCS and ADC_HDET_CELL have enum values 5 and 8
                   respectively. Requested by FTM team.
08-25-03   Vish    Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "target.h"
#include "comdef.h"
#include "adc.h"
#include "adci.h"
#include "adie.h"
#include "pm.h"
#include "vbatt.h"
#ifdef FEATURE_ADC_CALIBRATION
#include "adc_cal.h"

#endif //FEATURE_ADC_CALIBRATION

#ifndef FEATURE_ADC_CALIBRATION
#error  FEATURE_ADC_CALIBRATION must be defined
#endif

#include "math.h"
#include "rf_mux_select.h"
#include "hw.h"



/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
/* Reference voltage (Volts) used by ADIE's AD converter for full-scale reading */
#define ADC_ADIE_VREF                      (2.2)
#define ADC_ADIE_MAX_READING               (255)
#define ADC_ADIE_10BIT_MAX_READING         (1023)
#define ADC_ADIE_12BIT_MAX_READING         (4095)

#define ADC_ADIE_OFFSET_MV                 (0)
#define ADC_ADIE_SLOPE_MV                  (int32)( ADC_ADIE_VREF * 1000 \
                                                    * 10000 \
                                                    / ADC_ADIE_MAX_READING \
                                                  )

#define ADC_CALIBRATE_OFFSET 0
#define ADC_CALIBRATE_SLOPE  0

#ifdef FEATURE_ADC_12BIT_CALIBRATION
#define ADC_CALIBRATED_MAX_READING ADC_ADIE_12BIT_MAX_READING
#else
#define ADC_CALIBRATED_MAX_READING ADC_ADIE_MAX_READING
#endif

#define COEFF_C0 493
#define COEFF_C1 1.8583

/* ADIE MUX settings */
#define ADC_PM_AMUXOUT_ADIE_MUXSEL              ADIE_AD_MUX3
#define ADC_BATT_THERM_ADIE_MUXSEL              ADIE_AD_MUX1

#define ADC_R_EXT_ADIE_MUXSEL                   ADIE_AD_MUX4
#define ADC_HORZ_PP_ADIE_MUXSEL                 ADIE_AD_MUX4
#define ADC_VERT_PP_ADIE_MUXSEL                 ADIE_AD_MUX4
#define ADC_PTAT_ADIE_MUXSEL                    ADIE_AD_MUX4
#define ADC_CTAT_ADIE_MUXSEL                    ADIE_AD_MUX4
#define ADC_BG_ADIE_MUXSEL                      ADIE_AD_MUX4
#define ADC_REF_ADIE_MUXSEL                     ADIE_AD_MUX4
#define ADC_V_DD4_ADIE_MUXSEL                   ADIE_AD_MUX4
#define ADC_LO1_ADIE_MUXSEL                     ADIE_AD_MUX4
#define ADC_LO2_ADIE_MUXSEL                     ADIE_AD_MUX4
#define ADC_HDET_CELL_ADIE_MUXSEL               ADIE_AD_MUX4

/* PMIC internal MUX settings for various ADC channels */
#define ADC_VCOIN_PM_MUXSEL                PM_ADC_MUXSEL0
#define ADC_VCOIN_PM_MUXSCALE              PM_ADC_MUXSCALE15

#define ADC_VBATT_PM_MUXSEL                PM_ADC_MUXSEL1
#define ADC_VBATT_PM_MUXSCALE              PM_ADC_MUXSCALE15

#define ADC_VCHG_PM_MUXSEL                 PM_ADC_MUXSEL2
#define ADC_VCHG_PM_MUXSCALE               PM_ADC_MUXSCALE15

#define ADC_ICHG_PM_MUXSEL                 PM_ADC_MUXSEL3
#define ADC_ICHG_PM_MUXSCALE               PM_ADC_MUXSCALE15

#define ADC_ICHG_OUT_PM_MUXSEL             PM_ADC_MUXSEL4
#define ADC_ICHG_OUT_PM_MUXSCALE           PM_ADC_MUXSCALE15

#define ADC_BATT_THERM_PM_MUXSEL           PM_ADC_MUXSEL5
#define ADC_BATT_THERM_PM_MUXSCALE         PM_ADC_MUXSCALE15

#define ADC_BATT_ID_ADIE_MUXSEL            ADIE_AD_MUX2

#define ADC_USB_VBUS_PM_MUXSEL             PM_ADC_MUXSEL10
#define ADC_USB_VBUS_PM_MUXSCALE           PM_ADC_MUXSCALE15

#define ADC_PMIC_THERM_PM_MUXSEL           PM_ADC_MUXSEL11
#define ADC_PMIC_THERM_PM_MUXSCALE         PM_ADC_MUXSCALE15

#define ADC_XO_THERM_PM_MUXSEL             PM_ADC_MUXSEL7
#define ADC_XO_THERM_PM_MUXSCALE           PM_ADC_MUXSCALE15

/* Mapping structure for mV reading from the temp sensor to the
   equivalent temp in degree centigrade
*/
typedef struct
{
   int16   degC;
   uint16  mV;
} adc_degC_to_mV_map_type;

/*****************************************************************/
/* Thermistor tables are used to specify the temperature/voltage */
/* curve for the thermistor. The first parameter is the degrees  */
/* while the second parameter is the voltage in mV.When a        */
/* reading is taken the table is used to linearily interpolate   */
/* the result between 2 indexes. More entries can be added       */
/* between 2 points to improve accuracy.                         */
/* The table must be "ascending" or "descending" for the         */
/* interpolation to work correctly. They cannot have a reading   */
/* that maps to multiple degrees, and the table must be          */
/* specified in increasing or decreasing degrees.                */
/*****************************************************************/

/**************   Main Battery Thermistor Table    ****************/
// Source:  80-VE280-10
LOCAL const adc_degC_to_mV_map_type adc_batt_therm_map[] =
{
   { -10000, 2100}, /* Used to detect the removal of battery */
   { -30,  1887 },
   { -20,  1814 },
   { -10,  1711 },
   {   0,  1574 },
   {  10,  1408 },
   {  20,  1224 },
   {  30,  1035 },
   {  40,  856  },
   {  50,  695  },
   {  60,  558  },
   {  70,  446  },
   {  80,  355  }
};

/************   END: Main Battery Thermistor Table    **************/


/**************       XO THERM Thermistor Table        ****************/
// Source: 80-VF541-10 Rev. B
LOCAL const adc_degC_to_mV_map_type adc_xo_therm_map[] =
{
   { -30, 1980 },
   { -20, 1940 },
   { -10, 1880 },
   {   0, 1780 },
   {  10, 1640 },
   {  20, 1460 },
   {  25, 1360 },
   {  30, 1250 },
   {  40, 1030 },
   {  50,  810 },
   {  60,  630 },
   {  70,  480 },
   {  80,  360 },
   {  90,  270 }
};
/************   END: XO THERM Thermistor Table           **************/

/*===========================================================================

                FUNCTION PROTOTYPES

===========================================================================*/

extern adc_error_return_type adie_adc_start_read(
   adie_hkadc_muxsel_type adc_addr,            /* A/D input data source to select      */
#ifdef FEATURE_ADC_MULTI_RESOLUTION   
   adie_adc_res_type      adie_res,      // Resolution to use for ADC
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
   int *adc_result,                    /* Where to write A/D conversion result */
   void (*adc_completion_cb_fn)(void)  /* Call-back function when A/D conversion is complete
                                          (successfully or not) */
);

extern adc_error_return_type pm_adc_start_read(
   adie_hkadc_muxsel_type adie_muxsel,   // ADIE input selection for PMIC's MUX output
   pm_adc_muxsel_type     pm_muxsel,    // PMIC's MUX input source to select
   pm_adc_muxscale_type   pm_muxscale,  // PMIC's MUX scale
#ifdef FEATURE_ADC_MULTI_RESOLUTION   
   adie_adc_res_type      adie_res,      // Resolution to use for ADC
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
   int                   *adc_result,    // Where to write A/D conversion result
   void (*adc_completion_cb_fn)(void)    // Call-back function when A/D conversion is complete
                                         // (successfully or not)
);

LOCAL int32 adc_calc_degC_from_mV(
   const adc_degC_to_mV_map_type   *table,
   uint32                           tableSize,
   int32                            mV
);

#ifdef FEATURE_ADC_CALIBRATION
LOCAL void adc_calibrate(void);
LOCAL void adc_calibration_data_available_cb(void);
#endif //FEATURE_ADC_CALIBRATION


/*============================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION  adc_update_with_cal_info                          INTERNAL FUNCTION

DESCRIPTION
  Updates the offset and slope for the channel using the info from the
  cal_info struct

PARAMETERS
  logical_channel  The channel ID to update
  cal_info         Pointer to the cal_info data
  amux_ch          The AMUX channel that is used by this logical channel

RETURN VALUE
  none

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void adc_update_with_cal_info(adc_logical_channel_type logical_chan,
                              adc_cal_info_type *cal_info,
                              pm_adc_muxsel_type amux_ch)
{
   uint32 num, den;

   (void)pm_adc_get_prescalar(amux_ch, &num, &den);

   adc_logical_channel_definitions[logical_chan].offset_result
      = ( cal_info->voltage_mV_offset_x_10000 / 10000 ) * den / num;

   adc_logical_channel_definitions[logical_chan].result_vs_reading_slope_x_10000
      = cal_info->voltage_mV_vs_adc_count_slope_x_10000 * den / num;
}

/*===========================================================================
FUNCTION  adc_calc_degC_from_mV                           INTERNAL FUNCTION

DESCRIPTION
  Calculates the temp sensed by the temp sensor for a given sensor output
  voltage.

PARAMETERS
  table            Mapping between sensor output voltage and the actual temp
                   Table must be ascending or descending
  tableSize        Number of entries in 'table'
  mV               Temp sensor output voltage

RETURN VALUE
  int16            Temp sensed in Celsius

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL int32 adc_calc_degC_from_mV(
   const adc_degC_to_mV_map_type   *table,
   uint32                           tableSize,
   int32                            mV
)
{
    boolean descending = TRUE;
    uint32 search_index=0;

    if (table==NULL)
    {
      MSG_ERROR("Table NULL",0,0,0);
      return mV;
    }

    /* Check if table is descending or ascending */
    if (tableSize>1)
    {
      if (table[0].mV < table[1].mV)
      {
        descending=FALSE;
      }
    }

    while (search_index < tableSize)
    {
      if ( (descending==TRUE) && (table[search_index].mV < mV) )
      {
        /* table entry is less than measured value and table is descending, stop */
        break;
      }
      else if ( (descending==FALSE) && (table[search_index].mV > mV) )
      {
        /* table entry is greater than measured value and table is ascending, stop */
        break;
      }
      else
      {
        search_index++;
      }
    }


    if (search_index==0)
    {
      return table[0].degC;
    }
    else if (search_index==tableSize)
    {
      return table[tableSize-1].degC;
    }
    else
    {
      /* result is between search_index and search_index-1 */

      /* interpolate linearly */
      return (
               ( (int32)
                   (
                    (table[search_index].degC-table[search_index-1].degC)
                     *(mV-table[search_index-1].mV)
                   )
                   / (table[search_index].mV-table[search_index-1].mV)
               )
               + table[search_index-1].degC
             );

    }
}


/*=========================================================================*/
/*                                                                         */
/*             Wrapper function for each ADC input multiplexer             */
/*                                                                         */
/*=========================================================================*/

//Read resolution depends on the the ADC calibration resolution
adc_error_return_type adc_read_vcoin_cal_res(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_VCOIN_PM_MUXSEL,
      ADC_VCOIN_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_CALIBRATION_RESOLUTION,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}


adc_error_return_type adc_read_vbatt(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_VBATT_PM_MUXSEL,
      ADC_VBATT_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_8_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

//Read resolution depends on the the ADC calibration resolution
adc_error_return_type adc_read_vbatt_cal_res(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_VBATT_PM_MUXSEL,
      ADC_VBATT_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_CALIBRATION_RESOLUTION,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_vchg(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_VCHG_PM_MUXSEL,
      ADC_VCHG_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_8_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

//Read resolution depends on the the ADC calibration resolution
adc_error_return_type adc_read_vchg_cal_res(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_VCHG_PM_MUXSEL,
      ADC_VCHG_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_CALIBRATION_RESOLUTION,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_ichg(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_ICHG_PM_MUXSEL,
      ADC_ICHG_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_8_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

//Read resolution depends on the the ADC calibration resolution
adc_error_return_type adc_read_ichg_cal_res(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_ICHG_PM_MUXSEL,
      ADC_ICHG_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_CALIBRATION_RESOLUTION,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_ichg_out(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_ICHG_OUT_PM_MUXSEL,
      ADC_ICHG_OUT_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_8_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

//Read resolution depends on the the ADC calibration resolution
adc_error_return_type adc_read_ichg_out_cal_res(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_ICHG_OUT_PM_MUXSEL,
      ADC_ICHG_OUT_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_CALIBRATION_RESOLUTION,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_batt_id(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)adie_adc_start_read(
      ADC_BATT_ID_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_8_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

//Read resolution depends on the the ADC calibration resolution
adc_error_return_type adc_read_batt_id_cal_res(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)adie_adc_start_read(
      ADC_BATT_ID_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_CALIBRATION_RESOLUTION,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_batt_therm(int *result, void (*adc_completion_cb_fn)(void))
{
   if (qsc11x0_hw_rev.msm_hw_version >= 1 && qsc11x0_hw_rev.pg >= 3)
   {
      return (adc_error_return_type)pm_adc_start_read(
         ADC_PM_AMUXOUT_ADIE_MUXSEL,
         ADC_BATT_THERM_PM_MUXSEL,
         ADC_BATT_THERM_PM_MUXSCALE,
   #ifdef FEATURE_ADC_MULTI_RESOLUTION
         ADIE_ADC_RES_8_BITS,
   #endif /* FEATURE_ADC_MULTI_RESOLUTION */
         result,
         adc_completion_cb_fn);
   }
   else
   {
      return (adc_error_return_type)adie_adc_start_read(
         ADC_BATT_THERM_ADIE_MUXSEL,
   #ifdef FEATURE_ADC_MULTI_RESOLUTION
         ADIE_ADC_RES_8_BITS,
   #endif /* FEATURE_ADC_MULTI_RESOLUTION */
         result,
         adc_completion_cb_fn);
   }
}


adc_error_return_type adc_read_usb_vbus_cal_res(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_USB_VBUS_PM_MUXSEL,
      ADC_USB_VBUS_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_CALIBRATION_RESOLUTION,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_pmic_therm_cal_res(int *result, void (*adc_completion_cb_fn)(void))
{
   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      ADC_PMIC_THERM_PM_MUXSEL,
      ADC_PMIC_THERM_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_CALIBRATION_RESOLUTION,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_r_ext(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_R_EXT);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_R_EXT_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_horz_pp(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_HORZ_PP);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_HORZ_PP_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_vert_pp(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_VERT_PP);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_VERT_PP_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_ptat(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_PTAT);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_PTAT_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_ctat(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_CTAT);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_CTAT_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_bg(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_BG);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_BG_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_ref(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_REF);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_REF_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_v_dd4(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_V_DD4);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_V_DD4_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_lo1(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_LO1);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_LO1_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_lo2(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_LO2);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_LO2_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_hdet_cell(int *result, void (*adc_completion_cb_fn)(void))
{
  rf_setup_muxes(ADC_HDET_CELL);
   return (adc_error_return_type)adie_adc_start_read(
      ADC_HDET_CELL_ADIE_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */      
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_xo_therm_cal_res(int *result, void (*adc_completion_cb_fn)(void))
{
   pm_adc_muxsel_type temp_muxsel;

   if (qsc11x0_hw_rev.msm_hw_version >= 1 && qsc11x0_hw_rev.pg >= 3)
   {
      temp_muxsel = PM_ADC_MUXSEL7;
   }
   else
   {
      temp_muxsel = PM_ADC_MUXSEL5;
   }

   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      temp_muxsel,
      ADC_XO_THERM_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_CALIBRATION_RESOLUTION,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

adc_error_return_type adc_read_xo_therm_raw(int *result, void (*adc_completion_cb_fn)(void))
{
   pm_adc_muxsel_type temp_muxsel;
   
   if (qsc11x0_hw_rev.msm_hw_version >= 1 && qsc11x0_hw_rev.pg >= 3)
   {
      temp_muxsel = PM_ADC_MUXSEL7;
   }
   else
   {
      temp_muxsel = PM_ADC_MUXSEL5;
   }

   return (adc_error_return_type)pm_adc_start_read(
      ADC_PM_AMUXOUT_ADIE_MUXSEL,
      temp_muxsel,
      ADC_XO_THERM_PM_MUXSCALE,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_12_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
      result,
      adc_completion_cb_fn);
}

#ifdef FEATURE_JOYST_TASK
adc_error_return_type adc_read_joyst(int *result, void(*adc_completion_cb_fn)(void))
{
    return (adc_error_return_type)adie_adc_start_read(
       ADC_JOYST_MUXSEL,
#ifdef FEATURE_ADC_MULTI_RESOLUTION
      ADIE_ADC_RES_8_BITS,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
       result,
       adc_completion_cb_fn);
}
#endif  /*  FEATURE_JOYST_TASK */


/*===========================================================================

FUNCTION adc_not_present                                    INTERNAL FUNCTION

DESCRIPTION
  Not all ADC input sources are available on the SURF but to maintain
  compatibility through the API the logical channels may still be available.
  This null function allows adc_logical_channel_definitions[] to be
  completed.

PARAMETERS
  int *result               Where the ADC reading would be written
  void (*adc_completion_cb_fn)(void)
                            Call-back function that is called when the
                            ADC read process is completed.

RETURN VALUE
  adc_error_return_type     ADC_ERR_FLAG__CHAN_NOT_PRESENT

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
adc_error_return_type adc_not_present(int *result, void (*adc_completion_cb_fn)(void))
{
  /* parameter not used */
  /*lint -esym(715,result)*/
  /* Have lint not complain about the ignored parameter which is
  ** specified to make this routine match the template.
  */
  adc_completion_cb_fn();
  return ADC_ERR_FLAG__CHAN_NOT_PRESENT;
}

/*=========================================================================*/
/*                                                                         */
/*    Target-specific filter functions                                     */
/*                                                                         */
/*=========================================================================*/

/*=========================================================================*/
/*                                                                         */
/*    Target-specific adc-reading-to-return-value conversion functions     */
/*                                                                         */
/*=========================================================================*/


/*===========================================================================
FUNCTION  adc_batt_therm_to_degC                    INTERNAL FUNCTION

DESCRIPTION
  ADC reading to temperature conversion for the main battery temp sensor

PARAMETERS
  reading            Value read from ADC
  offset             Equivalent temp sensor voltage output at ZERO reading
  slope_x_10000      Temp sensor voltage vs. reading slope times 10000

RETURN VALUE
  adc_result_type.degC      Temp sensed by this temp sensor in Celsius

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL adc_result_type adc_batt_therm_to_degC(int reading, int32 offset, int32 slope_x_10000)
{
   adc_result_type result;

   // convert reading ---> mV
   result.mV = ((adc_as_mV) ((reading * slope_x_10000)/10000) + offset);

   // convert mV ---> degC using the table
   result.degC = adc_calc_degC_from_mV(
                    adc_batt_therm_map,
                    sizeof(adc_batt_therm_map)/sizeof(adc_batt_therm_map[0]),
                    result.mV);

   return (result);
}


/*===========================================================================
FUNCTION  adc_batt_therm_fine_to_degC                      INTERNAL FUNCTION

DESCRIPTION
  ADC reading to temperature conversion for the main battery temp sensor
  using finer settings.

PARAMETERS
  reading            Value read from ADC
  offset             Equivalent temp sensor voltage output at ZERO reading
  slope_x_10000      Temp sensor voltage vs. reading slope times 10000

RETURN VALUE
  adc_result_type.degC      Temp sensed by this temp sensor in Celsius

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL adc_result_type adc_batt_therm_fine_to_degC(int reading, int32 offset, int32 slope_x_10000)
{
   adc_result_type result;

   // convert reading ---> mV
   result.mV = ((adc_as_mV) ((reading * slope_x_10000)/10000) + offset);

   // convert mV ---> degC using the table
   result.degC = adc_calc_degC_from_mV(
                    adc_batt_therm_map,
                    sizeof(adc_batt_therm_map)/sizeof(adc_batt_therm_map[0]),
                    result.mV);

   return (result);
}


/*===========================================================================
FUNCTION  adc_pmic_therm_to_degC                    INTERNAL FUNCTION

DESCRIPTION
  ADC reading to temperature conversion for the PMIC3 die temp sensor

  Conversion equation specified in 80-VF541-10 rev. B

PARAMETERS
  reading            Value read from ADC
  offset             Equivalent temp sensor voltage output at ZERO reading
  slope_x_10000      Temp sensor voltage vs. reading slope times 10000

RETURN VALUE
  adc_result_type.degC      Temp sensed by this temp sensor in Celsius

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL adc_result_type adc_pmic_therm_to_degC(int reading, int32 offset, int32 slope_x_10000)
{
   adc_result_type result;

   // convert reading ---> mV
   result.mV = ((adc_as_mV) ((reading * slope_x_10000)/10000) + offset);

   /* we are subtracting 273 to convert the temp to degC otherwise the temp
    * is in kelvin */
   result.degC = (result.mV / 2) - 273;

   return (result);
}

/*===========================================================================
FUNCTION  adc_ptat_to_degC                              INTERNAL FUNCTION

DESCRIPTION
  ADC reading to temperature conversion for PTAT

PARAMETERS
  reading            Value read from ADC
  offset             Equivalent temp sensor voltage output at ZERO reading
  slope_x_10000      Temp sensor voltage vs. reading slope times 10000

RETURN VALUE
  adc_result_type.degC      Temp sensed by this temp sensor in Celsius

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL adc_result_type adc_ptat_to_degC(int reading, int32 offset, int32 slope_x_10000)
{
  double Temp_Est;
  adc_result_type result;


   Temp_Est = ( (reading*1.0) - COEFF_C0 ) / COEFF_C1;
    
   // Rounding off Temp_Est
   Temp_Est = Temp_Est >= 0 ? (Temp_Est+0.5) : (Temp_Est-0.5);

   result.degC = (int) Temp_Est;

   return (result);
}

/*===========================================================================
FUNCTION   adc_xo_therm_to_degC                             INTERNAL FUNCTION

DESCRIPTION
  ADC reading to temperature conversion for the XO THERM sensor
  Conversion equation is documented in 80-VF541-10 Rev. B

PARAMETERS
  reading            Value read from ADC
  offset             Equivalent temp sensor voltage output at ZERO reading
  slope_x_10000      Temp sensor voltage vs. reading slope times 10000

RETURN VALUE
  adc_result_type.degC      Temp sensed by this temp sensor in Celsius
                            Units are in degrees C

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL adc_result_type adc_xo_therm_to_degC(int reading, int32 offset, int32 slope_x_10000)
{
  adc_result_type result;

  // convert reading ---> mV
  result.mV = ((adc_as_mV) ((reading * slope_x_10000)/10000) + offset);

  // convert mV ---> degC using the table
  result.degC = adc_calc_degC_from_mV(
                    adc_xo_therm_map,
                    sizeof(adc_xo_therm_map)/sizeof(adc_xo_therm_map[0]),
                    result.mV);
  return result;
}

/*===========================================================================

FUNCTION adc_to_mV_vbatt                                INTERNAL FUNCTION

DESCRIPTION
  Converts vbatt reading into mv

PARAMETERS
  reading            Value read from ADC
  offset             Equivalent voltage output at ZERO reading
  slope_x_10000      voltage vs. reading slope times 10000


RETURN VALUE
  reading in mv
  
DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL adc_result_type adc_to_mV_vbatt(int reading, int32 offset, int32 slope_x_10000)
{
#ifdef FEATURE_PMIC_ADC_VBATT_CAL

   adc_result_type result;

   // Min calibrated reference voltage = 3.2V
   const int32  adc_vbatt_cal_volt_min_mv = 3200;
   // Max calibrated reference voltage = 4.2V
   const int32  adc_vbatt_cal_volt_max_mv = 4200;
   uint8  adc_vbatt_cal_adc_count_min = 0;
   uint8  adc_vbatt_cal_adc_count_max = ADC_CALIBRATED_MAX_READING ;

   boolean adc_vbatt_cal_reqd                     = FALSE;
   static boolean adc_vbatt_config_data_available = FALSE;
   static int32 adc_vbatt_offset_mV               = 0;
   static int32 adc_vbatt_slope_x_10000           = 0;

   // get the vbatt config info, if necessary
   if (!adc_vbatt_config_data_available)
   {
      // chk if calibration is reqd
      if ( (PM_ERR_FLAG__SUCCESS == pm_adc_is_calibration_reqd(&adc_vbatt_cal_reqd))
           && adc_vbatt_cal_reqd )
      {
         adc_vbatt_config_data_available
            = vbatt_get_adc_count_range_cal_info(
                 &adc_vbatt_cal_adc_count_min,
                 &adc_vbatt_cal_adc_count_max);
         if (adc_vbatt_config_data_available)
         {
            // update the offset and slope info in the table for ADC_VBATT_MV
            // If (x1,y1) and (x2,y2) are 2 points on a straight line, the offset
            // on the Y-axis at x=0 and the slope of the line can be calculated
            // by the following equations:
            //     Offset = (x2*y1 - x1*y2) / (x2 - x1)
            //     Slope  = (y2 - y1) / (x2 - x1)
            adc_vbatt_offset_mV
               = ( (adc_vbatt_cal_adc_count_max * adc_vbatt_cal_volt_min_mv)
                   - (adc_vbatt_cal_adc_count_min * adc_vbatt_cal_volt_max_mv) )
                 / (adc_vbatt_cal_adc_count_max - adc_vbatt_cal_adc_count_min);

            adc_vbatt_slope_x_10000
               = (adc_vbatt_cal_volt_max_mv - adc_vbatt_cal_volt_min_mv)
                 * 10000
                 / (adc_vbatt_cal_adc_count_max - adc_vbatt_cal_adc_count_min);
         }
      }
      if (!adc_vbatt_config_data_available)
      {
         // fall back on uncalibrated values if necessary
         adc_vbatt_offset_mV = offset;
         adc_vbatt_slope_x_10000 = slope_x_10000;

         adc_vbatt_config_data_available = TRUE;
      }
   }

   result.mV = ((adc_as_mV) ((reading * adc_vbatt_slope_x_10000)/10000) + adc_vbatt_offset_mV);
   return (result);
#else
 
   return adc_to_mV(reading, offset, slope_x_10000);

#endif

}

#ifdef FEATURE_ADC_CALIBRATION
/*===========================================================================
FUNCTION   adc_calibrate                                   INTERNAL FUNCTION

DESCRIPTION
  This is the helper function that actually initiates ADC self-calibration.
  It has knowledge of what PM AMUX scalers are used by various ADC logical
  channels on this phone target and performs calibration accordingly.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void adc_calibrate(void)
{
   // do whatever NV reads are required for calibration and pass it on
   // to ADC calibration module
   /* for e.g
   adc_cal_use_external_measurement(ADC_CAL_CONFIG__PM_MUXSCALEx, 1600, 157);
   adc_cal_use_external_measurement(ADC_CAL_CONFIG__PM_MUXSCALEy, 2100, 206);
   */

   adc_cal_calibrate( ADC_CAL_CONFIG__PM_MUXSCALE15,
                      ADC_PM_AMUXOUT_ADIE_MUXSEL,
                      adc_calibration_data_available_cb );
}


/*===========================================================================
FUNCTION   adc_calibration_data_available_cb               INTERNAL FUNCTION

DESCRIPTION
  This is the callback function used by the ADC calibration module to let
  this target specific ADC mapper know when the results are ready. This
  function then updates the offset and slopes for the various ADC logical
  channels whose accuracy is improved as a result of calibration.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void adc_calibration_data_available_cb(void)
{
   adc_cal_info_type   cal_info;

   if (adc_cal_get_cal_info(ADC_CAL_CONFIG__PM_MUXSCALE15, &cal_info))
   {
      // calibrate ADC_VCOIN_MV
      adc_update_with_cal_info(ADC_VCOIN_MV, &cal_info, ADC_VCOIN_PM_MUXSEL);

      // calibrate ADC_VBATT_MV
      adc_update_with_cal_info(ADC_VBATT_MV, &cal_info, ADC_VBATT_PM_MUXSEL);

      // calibrate ADC_VCHG_MV
      adc_update_with_cal_info(ADC_VCHG_MV, &cal_info, ADC_VCHG_PM_MUXSEL);

      // calibrate ADC_ICHG_MV
      adc_update_with_cal_info(ADC_ICHG_MV, &cal_info, ADC_ICHG_PM_MUXSEL);

      // calibrate ADC_ICHG_OUT_MV
      adc_update_with_cal_info(ADC_ICHG_OUT_MV, &cal_info, ADC_ICHG_OUT_PM_MUXSEL);

      // calibrate ADC_USB_VBUS_MV
      adc_update_with_cal_info(ADC_USB_VBUS_MV, &cal_info, ADC_USB_VBUS_PM_MUXSEL);

      // calibrate ADC_XO_THERM
      if (qsc11x0_hw_rev.msm_hw_version >= 1 && qsc11x0_hw_rev.pg >= 3)
      {
         adc_update_with_cal_info(ADC_XO_THERM_DEGC, &cal_info, PM_ADC_MUXSEL7);
      }
      else
      {
         adc_update_with_cal_info(ADC_XO_THERM_DEGC, &cal_info, PM_ADC_MUXSEL5);
      }

      // calibrate ADC_PMIC_THERM_DEGC
      adc_update_with_cal_info(ADC_PMIC_THERM_DEGC, &cal_info, ADC_PMIC_THERM_PM_MUXSEL);
   }
}
#endif //FEATURE_ADC_CALIBRATION


/*===========================================================================

                PUBLIC TABLE DEFINITIONS

===========================================================================*/
//  This table configures each channel expected by the user code
#ifndef FEATURE_ADC_CALIBRATION
const
#endif //FEATURE_ADC_CALIBRATION
adc_logical_channel_definition_type adc_logical_channel_definitions[NUMBER_OF_LOGICAL_ADC_CHANNELS] =
{
   // logical ch,                      adc_read_fn,               	ch_type,	min reading,			max reading,             			default result,			offset_result,						result_vs_reading_slope*10000,            smoothing fn,            conversion fn
   { ADC_BATT_ID,                      adc_read_batt_id,          	IMMEDIATE,	0,                    	ADC_ADIE_MAX_READING,    			0,						0,                                  0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_VBATT,                        adc_read_vbatt,            	IMMEDIATE,	0,                    	ADC_ADIE_MAX_READING,		    	180,					0,                                  0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_VCHG,                         adc_read_vchg,             	IMMEDIATE,	0,                    	ADC_ADIE_MAX_READING,		    	0,						0,									0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_ICHG,                         adc_read_ichg,             	IMMEDIATE,	0,                    	ADC_ADIE_MAX_READING,    			0,						0,									0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_ICHG_OUT,                     adc_read_ichg_out,         	IMMEDIATE,	0,                    	ADC_ADIE_MAX_READING,    			0,						0,									0,                                        adc_null_filter,         adc_null_conversion },
   
// New style
   // logical ch,                      adc_read_fn	,               ch_type,    min reading,          	max reading,             			default result,			offset_result,						result_vs_reading_slope*10000,            smoothing fn,            conversion fn
   { ADC_VCOIN_MV,                     adc_read_vcoin_cal_res,		IMMEDIATE,	0,                    	ADC_CALIBRATED_MAX_READING,    		3800,					ADC_CALIBRATE_OFFSET,				ADC_CALIBRATE_SLOPE,                      adc_null_filter,         adc_to_mV },
   { ADC_VBATT_MV,                     adc_read_vbatt_cal_res,		IMMEDIATE,	0,                    	ADC_CALIBRATED_MAX_READING,			3800,					ADC_CALIBRATE_OFFSET,				ADC_CALIBRATE_SLOPE,                      adc_null_filter,         adc_to_mV_vbatt },
   { ADC_VCHG_MV,                      adc_read_vchg_cal_res,		IMMEDIATE,	0,                    	ADC_CALIBRATED_MAX_READING,    		3800,					ADC_CALIBRATE_OFFSET,				ADC_CALIBRATE_SLOPE,                      adc_null_filter,         adc_to_mV },
   { ADC_ICHG_MV,                      adc_read_ichg_cal_res,		IMMEDIATE,	0,                    	ADC_CALIBRATED_MAX_READING,    		3800,					ADC_CALIBRATE_OFFSET,				ADC_CALIBRATE_SLOPE,                      adc_null_filter,         adc_to_mV },
   { ADC_ICHG_OUT_MV,                  adc_read_ichg_out_cal_res,  	IMMEDIATE,	0,                    	ADC_CALIBRATED_MAX_READING,    		3800,					ADC_CALIBRATE_OFFSET,				ADC_CALIBRATE_SLOPE,                      adc_null_filter,         adc_to_mV },
   { ADC_BATT_ID_MV,                   adc_read_batt_id_cal_res,	IMMEDIATE,	0,						ADC_CALIBRATED_MAX_READING,    		3800,					ADC_ADIE_OFFSET_MV,					ADC_ADIE_SLOPE_MV,                        adc_null_filter,         adc_to_mV },
   { ADC_BATT_THERM_DEGC,              adc_read_batt_therm,       	IMMEDIATE,	0,                    	ADC_ADIE_MAX_READING,    			50,						ADC_ADIE_OFFSET_MV,					ADC_ADIE_SLOPE_MV,                        adc_null_filter,         adc_batt_therm_to_degC },
   { ADC_USB_VBUS_MV,                  adc_read_usb_vbus_cal_res,	IMMEDIATE,	0,                    	ADC_CALIBRATED_MAX_READING,    		3800,					ADC_CALIBRATE_OFFSET,				ADC_CALIBRATE_SLOPE,                      adc_null_filter,         adc_to_mV },
   { ADC_PMIC_THERM_DEGC,              adc_read_pmic_therm_cal_res,	IMMEDIATE,	0,                    	ADC_CALIBRATED_MAX_READING,    		50,						ADC_CALIBRATE_OFFSET,				ADC_CALIBRATE_SLOPE,                      adc_null_filter,         adc_pmic_therm_to_degC },
   { ADC_R_EXT,                        adc_read_r_ext,            	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,									0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_HORZ_PP,                      adc_read_horz_pp,          	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,									0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_VERT_PP,                      adc_read_vert_pp,          	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_PTAT,                         adc_read_ptat,             	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_CTAT,                         adc_read_ctat,             	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_BG,                           adc_read_bg,               	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_REF,                          adc_read_ref,              	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_V_DD4,                        adc_read_v_dd4,            	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_LO1,                          adc_read_lo1,              	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_LO2,                          adc_read_lo2,              	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_INTERNAL_USE_CHAN_1,          adc_not_present,           	BUFFERED,	0,                  	ADC_ADIE_MAX_READING,    			50,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_INTERNAL_USE_CHAN_2,          adc_not_present,           	BUFFERED,	0,                    	ADC_ADIE_MAX_READING,    			50,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_PA_THERM_DEGC,                adc_not_present,           	IMMEDIATE,	0,                    	ADC_ADIE_MAX_READING,    			50,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_CHG_THERM_DEGC,               adc_not_present,           	IMMEDIATE,	0,                    	ADC_ADIE_MAX_READING,    			50,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_HDET_CELL,                    adc_read_hdet_cell,        	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,    		0,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_THERM,                        adc_not_present,           	IMMEDIATE,	0,                    	ADC_ADIE_MAX_READING,    			50,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_MSM_THERM_DEGC,               adc_not_present,           	IMMEDIATE,	0,                    	ADC_ADIE_MAX_READING,    			50,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_XO_THERM,                     adc_read_xo_therm_raw,     	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,     	0,						0,                                	0,                                        adc_null_filter,         adc_null_conversion },
   { ADC_XO_THERM_DEGC,                adc_read_xo_therm_cal_res,	IMMEDIATE,	0,                    	ADC_CALIBRATED_MAX_READING,     	0,						ADC_CALIBRATE_OFFSET,				ADC_CALIBRATE_SLOPE,                      adc_null_filter,         adc_xo_therm_to_degC },
   { ADC_PTAT_DEGC,                    adc_read_ptat,             	IMMEDIATE,	0,                    	ADC_ADIE_12BIT_MAX_READING,			0,						0,                                  0,                                        adc_null_filter,         adc_ptat_to_degC }
};


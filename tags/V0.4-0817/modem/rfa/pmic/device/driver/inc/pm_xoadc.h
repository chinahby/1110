/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 PMIC DRIVER SERVICES FOR PMIC XOADC

GENERAL DESCRIPTION
     This file contains functions and variable declarations to support 
  the PMIC XOADC.

INITIALIZATION AND SEQUENCING REQUIREMENTS                            
  None.

Copyright (c) 2008 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/device/main/latest/driver/inc/pm_xoadc.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/09   jtn     New file
===========================================================================*/

#ifndef PMXOADC_H
#define PMXOADC_H

/*===========================================================================
         Enum definitions used when calling APIs for PMIC XOADC
===========================================================================*/
typedef enum
{
	PM_XOADC_INPUT_PMIC,
	PM_XOADC_INPUT_XO,
	PM_XOADC_INPUT_VREFP,
	PM_XOADC_INPUT_VREFN,
	PM_XOADC_INPUT_INVALID
} pm_xoadc_input_select_type;

typedef enum
{
	PM_XOADC_DECIMATION_RATIO_1K,
	PM_XOADC_DECIMATION_RATIO_2K,
	PM_XOADC_DECIMATION_RATIO_4K,
	PM_XOADC_DECIMATION_RATIO_16K,
	PM_XOADC_DECIMATION_RATIO_32K,
	PM_XOADC_DECIMATION_RATIO_64K,
	PM_XOADC_DECIMATION_RATIO_1M,
	PM_XOADC_DECIMATION_RATIO_INVALID
} pm_xoadc_decimation_ratio_type;

typedef enum
{
	PM_XOADC_CONVERSION_RATE_TCXO_DIV_8,
	PM_XOADC_CONVERSION_RATE_TCXO_DIV_4,
	PM_XOADC_CONVERSION_RATE_TCXO_DIV_2,
	PM_XOADC_CONVERSION_RATE_TCXO_NO_DIV,
	PM_XOADC_CONVERSION_RATE_INVALID
} pm_xoadc_conversion_rate_type;

typedef enum
{
	PM_XOADC_FILTER_DISABLE_SINC1_DISABLE_SINC2,
	PM_XOADC_FILTER_ENABLE_SINC1_DISABLE_SINC2,
	PM_XOADC_FILTER_ENABLE_SINC1_ENABLE_SINC2,
	PM_XOADC_FILTER_INVALID
} pm_xoadc_filter_config_type;

typedef enum
{
	PM_XOADC_DISABLE_CMD,
	PM_XOADC_ENABLE_CMD
} pm_xoadc_enable_type;

/*===========================================================================
                  APIs for PMIC XOADC
===========================================================================*/

/**
 * pm_xoadc_init() must be called before attempting to use any
 * of the PMIC XOADC APIs
 * 
 * Initializes the XOADC registers. Upon initialization, the XOADC
 * Sinc1 and Sinc2 bits are set to enable both sinc1 and sinc2 (this
 * is the hardware default value of 11). Similarly, all other XOADC
 * registers are set to their hardware Default values.
 * 
 * Initializes static variable used for feature supported API
 * 
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS if initialization completed
 *         successfully
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED if XOADC is not supported on
 *         this PMIC
 */
extern pm_err_flag_type pm_xoadc_init(void);

/**
 * This function resets all the registers in the digital filter.
 * 
 * @return pm_err_flag_type
 * @example 
 *   pm_err_flag_type err;
 *   err = pm_xoadc_reset_filter();
 */
extern pm_err_flag_type pm_xoadc_reset_filter
(
    void
);

/**
 * This function selects the input to use when performing the XO ADC conversion.
 * 
 * @param xoadcInput PM_XOADC_INPUT_PMIC selects PMIC_IN. This is used for the
 *                   PMIC AMUX channels.
 *                   PM_XOADC_INPUT_XO selects XO_IN. This is used for the XO
 *                   THERM channel.
 *                   PM_XOADC_INPUT_VREFP shorts the xoadc input to vrefp for
 *                   gain error measurement.
 *                   PM_XOADC_INPUT_VREFN shorts the xoadc input to vrefn for
 *                   offset error measurement.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_xoadc_set_input
(
    pm_xoadc_input_select_type xoadcInput
);

/**
 * Sets the XOADC decimation ratio.
 * 
 * @param decimationRatio
 *               PM_XOADC_DECIMATION_RATIO_1K
 *               PM_XOADC_DECIMATION_RATIO_2K
 *               PM_XOADC_DECIMATION_RATIO_4K
 *               PM_XOADC_DECIMATION_RATIO_16K
 *               PM_XOADC_DECIMATION_RATIO_32K
 *               PM_XOADC_DECIMATION_RATIO_64K
 *               PM_XOADC_DECIMATION_RATIO_1M
 * 
 * @return pm_err_flag_type
 * PM_ERR_FLAG__SUCCESS
 * PM_ERR_FLAG__FEATURE_NOT_SUPPORTED
 * PM_ERR_FLAG__PAR1_OUT_OF_RANGE
 * PM_ERR_FLAG__SBI_OPT_ERR
 */
extern pm_err_flag_type pm_xoadc_set_decimation_ratio
(
    pm_xoadc_decimation_ratio_type decimationRatio
);

/**
 * Sets the XOADC clock rate
 * 
 * @param conversionRate PM_XOADC_CONVERSION_RATE_TCXO_DIV_8
 *                       PM_XOADC_CONVERSION_RATE_TCXO_DIV_4
 *                       PM_XOADC_CONVERSION_RATE_TCXO_DIV_2
 *                       PM_XOADC_CONVERSION_RATE_TCXO_NO_DIV
 * 
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE
 *         PM_ERR_FLAG__SBI_OPT_ERR
 */
extern pm_err_flag_type pm_xoadc_set_conversion_rate
(
    pm_xoadc_conversion_rate_type conversionRate
);

/**
 * Sets the XOADC decimation filter configuration
 * 
 * @param filterConfig PM_XOADC_FILTER_DISABLE_SINC1_DISABLE_SINC2
 *                     PM_XOADC_FILTER_ENABLE_SINC1_DISABLE_SINC2
 *                     PM_XOADC_FILTER_ENABLE_SINC1_ENABLE_SINC2
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_xoadc_set_filter_config
(
    pm_xoadc_filter_config_type filterConfig
);

/**
 * Enabling the modulator starts the XOADC conversion. Conversions continue to
 * take place and the result accumulates in the data register until the
 * modulator is disabled.
 * 
 * @param xoadcEnable PM_XOADC_DISABLE_CMD - disable the modulator
 *                    PM_XOADC_ENABLE_CMD - enable the modulator
 * 
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS function completed successfully
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED XOADC not available on this PMIC
 *         PM_ERR_FLAG__SBI_OPT_ERR  error communicating with the PMIC
 */
extern pm_err_flag_type pm_xoadc_enable_modulator
(
    pm_xoadc_enable_type xoadcEnable
);

/**
 * Reads the PMIC XOADC data registers and returns the value.
 * 
 * Returns the 24-bit conversion value in registers XOADC_DATA0, XOADC_DATA1
 * and XOADC_DATA2
 * 
 * Note: must read DATA 0 first
 * returns 0 in xoadc_data if there was an error reading from the PMIC
 * 
 * @param xoadc_data Pointer to a uint32 that will hold the 24-bit value of the XOADC data registers.
 *                   
 *                   xoadc_data will be set to 0 if there is an error reading the data.
 * 
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS function completed successfully
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED XOADC not available on this PMIC
 *         PM_ERR_FLAG__SBI_OPT_ERR  error communicating with the PMIC
 */
extern pm_err_flag_type pm_xoadc_read_data
(
    uint32 *xoadc_data
);

#endif


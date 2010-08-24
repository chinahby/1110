#ifndef ADIE_ADCDEFS_H
#define ADIE_ADCDEFS_H
/*===========================================================================


   A N A L O G   D I E   R E G I S T E R   D E F S   H E A D E R    F I L E

DESCRIPTION
  This file contains all the register and bit definitions for the LCU analog die.

  These definitions are used by the analog die c file and header file.  The
  definitions are the same for both targets that use memory mapped 
  communication and SBI communication.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/src/adie_adcdefs.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/08   mj     ULC updates
08/14/06   gb     Created
===========================================================================*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 LCU HKADC Definitions

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 ADIE_HKADC_SETUP_R (Register 0xA0) HKADC setup register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_HKADC_SETUP_R                                 0xAC
#define ADIE_HKADC_SETUP_M                                 0xFF

#define ADIE_HKADC_SETUP_INIT_V  ( ADIE_HKADC_SETUP_CONVERT_ON_CONFIG_WR_ENA_V | \
                                  ADIE_HKADC_SETUP_TEMP_SEN_DIS_V         | \
                                  ADIE_HKADC_SETUP_CONT_ADC_PWRUP_ONLY_DURING_CONV | \
                                  ADIE_HKADC_SETUP_R_MEAS_DIS_V | \
                                  ADIE_HKADC_SETUP_DOUBLE_CONV_DIS_V       | \
                                  ADIE_HKADC_SETUP_ADC_EN_V )

/* Initialize everything, so full register mask */
#define ADIE_HKADC_SETUP_INIT_M  ( ADIE_HKADC_SETUP_M )

/* Enable conversion on HK_ANA_CONFIG write 
 * (during HKADC operation, it also enables warm-up time)
 */
#define ADIE_HKADC_SETUP_CONVERT_ON_CONFIG_WR_M            0x80
#define ADIE_HKADC_SETUP_CONVERT_ON_CONFIG_WR_DIS_V        0x00
#define ADIE_HKADC_SETUP_CONVERT_ON_CONFIG_WR_ENA_V        0x80

/* Temperature sense enable
*/
#define ADIE_HKADC_SETUP_TEMP_SEN_M                        0x40
#define ADIE_HKADC_SETUP_TEMP_SEN_DIS_V                    0x00
#define ADIE_HKADC_SETUP_TEMP_SEN_EN_V                     0x40

#define ADIE_HKADC_SETUP_CONT_ADC_PWRUP_M                  0x20
#define ADIE_HKADC_SETUP_CONT_ADC_PWRUP_ONLY_DURING_CONV   0x00
#define ADIE_HKADC_SETUP_CONT_ADC_PWRUP_V                  0x20

#define ADIE_HKADC_SETUP_R_MEAS_ENA_M                      0x10
#define ADIE_HKADC_SETUP_R_MEAS_ENA_V                      0x10
#define ADIE_HKADC_SETUP_R_MEAS_DIS_V                      0x00

/* Double conversion enable
 */
#define ADIE_HKADC_SETUP_DOUBLE_CONV_M                      0x02
#define ADIE_HKADC_SETUP_DOUBLE_CONV_DIS_V                  0x00
#define ADIE_HKADC_SETUP_DOUBLE_CONV_EN_V                   0x02

/* ADC enable / disable to sleep state
 */
#define ADIE_HKADC_SETUP_ADC_M                              0x01
#define ADIE_HKADC_SETUP_ADC_DIS_V                          0x00
#define ADIE_HKADC_SETUP_ADC_EN_V                           0x01


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 ADIE_HKADC_ANA_CONG_R (Register 0xA1) HKADC ANA configuration register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_HKADC_ANA_CONG_R                              0xAD
#define ADIE_HKADC_ANA_CONG_M                              0xFF

#define ADIE_HKADC_ANA_CONG_INIT_V  ( ADIE_HKADC_ANA_CONG_MUX_SEL_NO_CHAN_V | \
                                      ADIE_HKADC_ANA_CONG_REF_SEL_VDD_V )

/* Initialize everything, so full register mask */
#define ADIE_HKADC_ANA_CONG_INIT_M  ( ADIE_HKADC_ANA_CONG_M )

/* Mux channel selection
 */
#define ADIE_HKADC_ANA_CONG_MUX_SEL_M                      0xF0
#define ADIE_HKADC_ANA_CONG_MUX_SEL_NO_CHAN_V              0x00
#define ADIE_HKADC_ANA_CONG_MUX_SEL_AIN0_V                 0x10
#define ADIE_HKADC_ANA_CONG_MUX_SEL_AIN1_V                 0x20
#define ADIE_HKADC_ANA_CONG_MUX_SEL_AMUXIN_V               0x30
#define ADIE_HKADC_ANA_CONG_MUX_SEL_HK_PDET_V              0x40
#define ADIE_HKADC_ANA_CONG_MUX_SEL_HK_R_MEAS_V            0x50
#define ADIE_HKADC_ANA_CONG_MUX_SEL_INT_TEMP_SENSOR_V      0x60

/* reference selector
 */
#define ADIE_HKADC_ANA_CONG_REF_SEL_M                      0x03
#define ADIE_HKADC_ANA_CONG_REF_SEL_VDD_V                  0x00
#define ADIE_HKADC_ANA_CONG_REF_SEL_AIN0_V                 0x01
#define ADIE_HKADC_ANA_CONG_REF_SEL_INTR_TX_V              0x02
#define ADIE_HKADC_ANA_CONG_REF_SEL_NONE_V                 0x03

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 ADIE_HKADC_PARAM_R (Register 0xA2) HKADC Parameter register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_HKADC_PARAM_R                                 0xAE
#define ADIE_HKADC_PARAM_M                                 0xFF

#define ADIE_HKADC_PARAM_INIT_V  ( ADIE_HKADC_PARAM_GAIN_ERR_CORR_BITS_000_V | \
                                  ADIE_HKADC_PARAM_OFFSET_CORR_BITS_000_V )

/* Initialize everything, so full register mask */
#define ADIE_HKADC_PARAM_INIT_M  ( ADIE_HKADC_PARAM_M )

/* Gain error Correction bits
 */
#define ADIE_HKADC_PARAM_GAIN_ERR_CORR_BITS_M              0x1C
#define ADIE_HKADC_PARAM_GAIN_ERR_CORR_BITS_000_V          0x00
#define ADIE_HKADC_PARAM_GAIN_ERR_CORR_BITS_001_V          0x04
#define ADIE_HKADC_PARAM_GAIN_ERR_CORR_BITS_010_V          0x08
#define ADIE_HKADC_PARAM_GAIN_ERR_CORR_BITS_011_V          0x0C
#define ADIE_HKADC_PARAM_GAIN_ERR_CORR_BITS_100_V          0x10
#define ADIE_HKADC_PARAM_GAIN_ERR_CORR_BITS_101_V          0x14
#define ADIE_HKADC_PARAM_GAIN_ERR_CORR_BITS_110_V          0x18
#define ADIE_HKADC_PARAM_GAIN_ERR_CORR_BITS_111_V          0x1C

/* SH offset Correction bits
 */
#define ADIE_HKADC_PARAM_OFFSET_CORR_BITS_M              0xE0
#define ADIE_HKADC_PARAM_OFFSET_CORR_BITS_000_V          0x00
#define ADIE_HKADC_PARAM_OFFSET_CORR_BITS_001_V          0x20
#define ADIE_HKADC_PARAM_OFFSET_CORR_BITS_010_V          0x40
#define ADIE_HKADC_PARAM_OFFSET_CORR_BITS_011_V          0x60
#define ADIE_HKADC_PARAM_OFFSET_CORR_BITS_100_V          0x80
#define ADIE_HKADC_PARAM_OFFSET_CORR_BITS_101_V          0xA0
#define ADIE_HKADC_PARAM_OFFSET_CORR_BITS_110_V          0xC0
#define ADIE_HKADC_PARAM_OFFSET_CORR_BITS_111_V          0xE0

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ADIE_HKADC_DATA_RD_R (Register 0xA3) HKADC data registers (most sig bits)
                 (read only)        (least sig bits are in HKADC_STATUS_R)

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/* Most significant bits of ADC data read only register 
 */
#define ADIE_HK_ADC_DATA_RD_R                               0xAF
#define ADIE_HK_ADC_DATA_RD_M                               0xFF


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 ADIE_HKADC_STATUS_R (Register 0xA4) Adie HKADC status register
                     (read only)

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_HK_ADC_STATUS_R                                0xB0
#define ADIE_HK_ADC_STATUS_M                                0xFF

/* 4 least significant bits of ADC data read only register 
 */
#define ADIE_HKADC_STATUS_DATA_M                           0xF0

/* HKADC operation 0 means conversion not ready, 1 means done
 */
#define ADIE_HKADC_STATUS_ADC_EOC_M                        0x01
#define ADIE_HKADC_STATUS_ADC_EOC_CONV_DONE_V              0x01

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 ADIE_HKADC_SETUP2_R (Register 0xB1) HKADC Setup2 register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_HKADC_SETUP2_R                                 0xB1
#define ADIE_HKADC_SETUP2_M                                 0xFF

#define ADIE_HKADC_SETUP2_INIT_V  ( ADIE_HKADC_SETUP2_CLK_BUFF_EN_V | \
                                  ADIE_HKADC_SETUP2_EN_HK_DIGITAL_ATEST_1_OPEN_V         | \
                                  ADIE_HKADC_SETUP2_EN_HK_ANALOG_ATEST_OPEN_V)

/* Initialize everything, so full register mask */
#define ADIE_HKADC_SETUP2_INIT_M  ( ADIE_HKADC_SETUP2_M )

/* HKADC comparator preamp2 current
 */
#define ADIE_HKADC_SETUP2_COMP_PREAMP2_M                   0xC0
#define ADIE_HKADC_SETUP2_COMP_PREAMP2_BITS_00_V           0x00
#define ADIE_HKADC_SETUP2_COMP_PREAMP2_BITS_01_V           0x40
#define ADIE_HKADC_SETUP2_COMP_PREAMP2_BITS_10_V           0x80
#define ADIE_HKADC_SETUP2_COMP_PREAMP2_BITS_11_V           0xC0

/* HKADC comparator preamp1 current
 */
#define ADIE_HKADC_SETUP2_COMP_PREAMP1_M                   0x30
#define ADIE_HKADC_SETUP2_COMP_PREAMP1_BITS_00_V           0x00
#define ADIE_HKADC_SETUP2_COMP_PREAMP1_BITS_01_V           0x10
#define ADIE_HKADC_SETUP2_COMP_PREAMP1_BITS_10_V           0x20
#define ADIE_HKADC_SETUP2_COMP_PREAMP1_BITS_11_V           0x30

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 ADIE_HKADC_COMMAND_R (Register 0xA6) Adie HKADC command register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_HKADC_COMMAND_R                               0xB2
#define ADIE_HKADC_COMMAND_M                               0xFF

/* Start a conversion when writing to this register
 */
#define ADIE_HKADC_COMMAND_CMD_WR_M                        0xFF
#define ADIE_HKADC_COMMAND_CMD_WR_V                        0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 ADIE_HKADC_RC_CAL_R (Register 0xB3) HKADC RC_CAL register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_HKADC_RC_CAL_R                                0xB3
#define ADIE_HKADC_RC_CAL_M                                0xFF

#define ADIE_HKADC_RC_CAL_INIT_V  ( ADIE_HKADC_RC_CAL_R_VAL_HIGH_V | \
                                  ADIE_HKADC_RC_CAL_CAP_STATE_CAL_V         | \
                                  ADIE_HKADC_RC_CAL_CALIBR_RC_V | \
                                  ADIE_HKADC_RC_CAL_C2_DIS_V | \
                                  ADIE_HKADC_RC_CAL_CKT_DIS_V)

/* Initialize everything, so full register mask */
#define ADIE_HKADC_RC_CAL_INIT_M  ( ADIE_HKADC_RC_CAL_M )

/* R value
 */
#define ADIE_HKADC_RC_CAL_R_VAL_M                          0x80
#define ADIE_HKADC_RC_CAL_R_VAL_HIGH_V                     0x00
#define ADIE_HKADC_RC_CAL_R_VAL_LOW_V                      0x80

/* CAP state
 */
#define ADIE_HKADC_RC_CAL_CAP_STATE_M                      0x40
#define ADIE_HKADC_RC_CAL_CAP_STATE_CAL_V                  0x00
#define ADIE_HKADC_RC_CAL_CAP_STATE_RESET_V                0x40

/* RC Calibration
 */
#define ADIE_HKADC_RC_CAL_CALIBR_M                         0x20
#define ADIE_HKADC_RC_CAL_CALIBR_RC_V                      0x00
#define ADIE_HKADC_RC_CAL_CALIBR_R_ATEST_V                 0x20

/* Select C2 for BBRX filter cap measurement
 */
#define ADIE_HKADC_RC_CAL_C2_M                             0x10
#define ADIE_HKADC_RC_CAL_C2_DIS_V                         0x00
#define ADIE_HKADC_RC_CAL_C2_EN_V                          0x10

/* RC measurement enable
 */
#define ADIE_HKADC_RC_CAL_CKT_M                            0x01
#define ADIE_HKADC_RC_CAL_CKT_DIS_V                        0x00
#define ADIE_HKADC_RC_CAL_CKT_EN_V                         0x01


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 ADIE_HKADC_PARAM2_R (Register 0xA8) HKADC PARAM2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_HKADC_PARAM2_R                            0xB4
#define ADIE_HKADC_PARAM2_M                            0xFF

#define ADIE_HKADC_PARAM2_INIT_V  ( ADIE_HKADC_PARAM2_TCXO_DIV_8_V | \
                                       ADIE_HKADC_PARAM2_SAMPLE_INT_48_V  | \
                                       ADIE_HKADC_PARAM2_RES_8_BITS_V)

/* Initialize everything, so full register mask */
#define ADIE_HKADC_PARAM2_INIT_M  ( ADIE_HKADC_PARAM2_M )

/* Division of input clock (TCXO)
   */
#define ADIE_HKADC_PARAM2_TCXO_DIV_M                       0xC0
#define ADIE_HKADC_PARAM2_TCXO_DIV_8_V                     0x00
#define ADIE_HKADC_PARAM2_TCXO_DIV_4_V                     0x40
#define ADIE_HKADC_PARAM2_TCXO_DIV_16_V                    0x80
#define ADIE_HKADC_PARAM2_TCXO_DIV_32_V                    0xC0

/* Sampling interval based on 2.4MHz clk
 * ie: 3 * (8/TCXO), 48 * (8/TCXO)
  */
#define ADIE_HKADC_PARAM2_SAMPLE_INT_M                     0x38
#define ADIE_HKADC_PARAM2_SAMPLE_INT_3_V                   0x00
#define ADIE_HKADC_PARAM2_SAMPLE_INT_24_V                  0x08
#define ADIE_HKADC_PARAM2_SAMPLE_INT_36_V                  0x10
#define ADIE_HKADC_PARAM2_SAMPLE_INT_48_V                  0x18
#define ADIE_HKADC_PARAM2_SAMPLE_INT_1_V                   0x20
#define ADIE_HKADC_PARAM2_SAMPLE_INT_2_V                   0x28
#define ADIE_HKADC_PARAM2_SAMPLE_INT_NONE_V                0x30

/* Resolution of ADC reading
 */
#define ADIE_HKADC_PARAM2_RES_M                            0x06
#define ADIE_HKADC_PARAM2_RES_8_BITS_V                     0x00
#define ADIE_HKADC_PARAM2_RES_10_BITS_V                    0x02
#define ADIE_HKADC_PARAM2_RES_12_BITS_V                    0x04


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 ADIE_HKADC_RSV1_R (Register 0xA9) HKADC RSV1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_HKADC_RSV1_R                                  0xB5
#define ADIE_HKADC_RSV1_M                                  0xFF

#define ADIE_HKADC_RSV1_INIT_V  ( ADIE_HKADC_RSV1_EN_HK_DIGITAL_ATEST_2_OPEN_V | \
                                       ADIE_HKADC_RSV1_CONT_ADC_PWRUP_DIS_V  )

/* Initialize everything, so full register mask */
#define ADIE_HKADC_RSV1_INIT_M  ( ADIE_HKADC_RSV1_M )

/* EN_HK_DIGITAL_ATEST_2
   */
#define ADIE_HKADC_RSV1_EN_HK_DIGITAL_ATEST_2_M            0x60
#define ADIE_HKADC_RSV1_EN_HK_DIGITAL_ATEST_2_OPEN_V       0x00
#define ADIE_HKADC_RSV1_EN_HK_DIGITAL_ATEST_2_MODE1_V      0x20
#define ADIE_HKADC_RSV1_EN_HK_DIGITAL_ATEST_2_MODE2_V      0x40


/* continuous power up selection 
 * (during continuous power up, it disables warm-up time)
 */
#define ADIE_HKADC_RSV1_CONT_ADC_PWRUP_M                   0x10
#define ADIE_HKADC_RSV1_CONT_ADC_PWRUP_DIS_V               0x00
#define ADIE_HKADC_RSV1_CONT_ADC_PWRUP_EN_V                0x10

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 ADIE_HKADC_RSV2_R (Register 0xA9) HKADC RSV2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_HKADC_RSV2_R                                  0xB6
#define ADIE_HKADC_RSV2_M                                  0xFF

#define ADIE_HKADC_RSV2_INIT_V                             0x00







/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 RaMSIS XO ADC Definitions

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_XOADC_ANA_PARAM_R               0x8C
#define ADIE_XOADC_ANA_PARAM_INIT_V          0xFE

#define ADIE_XOADC_DIG_PARAM_R               0x8D
#define ADIE_XOADC_DIG_PARAM_INIT_V          0x03

#define ADIE_XOADC_RSV_R                     0x8E
#define ADIE_XOADC_RSV_INIT_V                0x80

#define ADIE_XOADC_ATEST_R                   0x8F
#define ADIE_XOADC_ATEST_INIT_V              0x00

#define ADIE_XOADC_DATA2_R                   0xE9
#define ADIE_XOADC_DATA1_R                   0xC7
#define ADIE_XOADC_DATA0_R                   0xC6

#define FEATURE_ADC_MULTI_RESOLUTION

/////////////////////////////////////////////////////////////////////////////
#ifdef ADIE_SUPPORT_OLD_HKADC_DEFS

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ADIE_TSHK_PARAM_R (Register 0x4D) TSHK parameter register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_TSHK_PARAM_R                                 0x4D
#define ADIE_TSHK_PARAM_M                                 0xFF

#define ADIE_TSHK_PARAM_INIT_V  ( ADIE_TSHK_PARAM_INPUT_CLK_DIV_8_V    | \
                                  ADIE_TSHK_PARAM_PRE_CHARGE_6_4_US_V  | \
                                  ADIE_TSHK_PARAM_STAB_TIME_6_4_US_V )

/* Initialize everything, so full register mask */
#define ADIE_TSHK_PARAM_INIT_M  ( ADIE_TSHK_PARAM_M )

/* Sets the division of the input clock (TCXO)
 */
#define ADIE_TSHK_PARAM_INPUT_M                           0xC0
#define ADIE_TSHK_PARAM_INPUT_CLK_DIV_8_V                 0x00
#define ADIE_TSHK_PARAM_INPUT_CLK_DIV_4_V                 0x40
#define ADIE_TSHK_PARAM_INPUT_CLK_DIV_16_V                0x80
#define ADIE_TSHK_PARAM_INPUT_CLK_DIV_32_V                0xC0

/* Sets pre-charge time before enabling interrupt waiting
 * mode.
 */
#define ADIE_TSHK_PARAM_PRE_CHARGE_M                      0x38
#define ADIE_TSHK_PARAM_PRE_CHARGE_6_4_US_V               0x00
#define ADIE_TSHK_PARAM_PRE_CHARGE_12_8_US_V              0x08
#define ADIE_TSHK_PARAM_PRE_CHARGE_25_6_US_V              0x10
#define ADIE_TSHK_PARAM_PRE_CHARGE_51_2_US_V              0x18
#define ADIE_TSHK_PARAM_PRE_CHARGE_102_4_US_V             0x20
#define ADIE_TSHK_PARAM_PRE_CHARGE_204_8_US_V             0x28
#define ADIE_TSHK_PARAM_PRE_CHARGE_409_6_US_V             0x30
#define ADIE_TSHK_PARAM_PRE_CHARGE_ADD_OPTS_V             0x38

/* Sets stabilization time after driving the touch panel to 
 * charge panel capacitors.
 */
#define ADIE_TSHK_PARAM_STAB_TIME_M                       0x07
#define ADIE_TSHK_PARAM_STAB_TIME_6_4_US_V                0x00
#define ADIE_TSHK_PARAM_STAB_TIME_12_8_US_V               0x01
#define ADIE_TSHK_PARAM_STAB_TIME_25_6_US_V               0x02
#define ADIE_TSHK_PARAM_STAB_TIME_51_2_US_V               0x03
#define ADIE_TSHK_PARAM_STAB_TIME_102_4_US_V              0x04
#define ADIE_TSHK_PARAM_STAB_TIME_204_8_US_V              0x05
#define ADIE_TSHK_PARAM_STAB_TIME_409_6_US_V              0x06
#define ADIE_TSHK_PARAM_STAB_TIME_ADD_OPTS_V              0x07

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ADIE_TSHK_SETUP_R (Register 0x4E) TSHK setup register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_TSHK_SETUP_R                                 0x4E
#define ADIE_TSHK_SETUP_M                                 0xFF

#define ADIE_TSHK_SETUP_INIT_V  ( ADIE_TSHK_SETUP_DYN_CORR_DIS_V | \
                                  ADIE_TSHK_SETUP_EN_TS_ATEST_OPEN_V         | \
                                  ADIE_TSHK_SETUP_POWER_UP_PWRD_UP_IN_CONV_V | \
                                  ADIE_TSHK_SETUP_INIT_PRE_CHAR_DIS_V        | \
                                  ADIE_TSHK_SETUP_GLOBAL_PEN_IRQ_DIS_V       | \
                                  ADIE_TSHK_SETUP_TEMP_SEN_DIS_V             | \
                                  ADIE_TSHK_SETUP_TSHK_EN_V)

/* Initialize everything, so full register mask */
#define ADIE_TSHK_SETUP_INIT_M  ( ADIE_TSHK_SETUP_M )

/* Enable dynamic correction algorithm
 */
#define ADIE_TSHK_SETUP_DYN_CORR_M                        0x80
#define ADIE_TSHK_SETUP_DYN_CORR_DIS_V                    0x00
#define ADIE_TSHK_SETUP_DYN_CORR_EN_V                     0x80

/* Test switches
 */
#define ADIE_TSHK_SETUP_EN_TS_ATEST_M                     0x60
#define ADIE_TSHK_SETUP_EN_TS_ATEST_OPEN_V                0x00
#define ADIE_TSHK_SETUP_EN_TS_ATEST_MODE1_V               0x20
#define ADIE_TSHK_SETUP_EN_TS_ATEST_MODE2_V               0x40
#define ADIE_TSHK_SETUP_EN_TS_ATEST_MODE3_V               0x60

/* Enables continuous ADC power up. Continuous power up removes
 * need for warm up time
 */
#define ADIE_TSHK_SETUP_POWER_UP_M                        0x10
#define ADIE_TSHK_SETUP_POWER_UP_PWRD_UP_IN_CONV_V        0x00
#define ADIE_TSHK_SETUP_POWER_UP_CONT_PWRD_UP_V           0x10

/* Enable pre-charging of panel capacitors. Bit must be toggled
 * prior to asserting GLOBAL_PEN_IRQ or asserting PEN_IRQ_EN.
 * Must be high for 3 SBI operations.
 */
#define ADIE_TSHK_SETUP_INIT_PRE_CHAR_M                   0x08
#define ADIE_TSHK_SETUP_INIT_PRE_CHAR_DIS_V               0x00
#define ADIE_TSHK_SETUP_INIT_PRE_CHAR_EN_V                0x08

/* Enable PEN IRQ, penirqen is enabled even if TS/HKADC is
 * in sleep mode.
 */
#define ADIE_TSHK_SETUP_GLOBAL_PEN_IRQ_M                  0x04
#define ADIE_TSHK_SETUP_GLOBAL_PEN_IRQ_DIS_V              0x00
#define ADIE_TSHK_SETUP_GLOBAL_PEN_IRQ_EN_V               0x04

/* Enable temperature sensor
 */
#define ADIE_TSHK_SETUP_TEMP_SEN_M                        0x02
#define ADIE_TSHK_SETUP_TEMP_SEN_DIS_V                    0x00
#define ADIE_TSHK_SETUP_TEMP_SEN_EN_V                     0x02

/* Enable TSHK operation. Disable gates outputs from digital 
 * controller to ADC to their sleep state values
 */
#define ADIE_TSHK_SETUP_TSHK_M                            0x01
#define ADIE_TSHK_SETUP_TSHK_DIS_V                        0x00
#define ADIE_TSHK_SETUP_TSHK_EN_V                         0x01

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ADIE_TSHK_DIG_CONFIG_R (Register 0x4F) TSHK dig config

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_TSHK_DIG_CONFIG_R                            0x4F
#define ADIE_TSHK_DIG_CONFIG_M                            0xFF
#define ADIE_TSHK_DIG_CONFIG_INIT_V  ( ADIE_TSHK_DIG_CONFIG_SAMPLE_INT_48_V | \
                                       ADIE_TSHK_DIG_CONFIG_NUM_SAMPLE_1_V  | \
                                       ADIE_TSHK_DIG_CONFIG_RES_8_BITS_V    | \
                                       ADIE_TSHK_DIG_CONFIG_TS_SEL_HK_V     | \
                                       ADIE_TSHK_DIG_CONFIG_CONV_ON_CNFG_WR_EN_V  )

/* Initialize everything, so full register mask */
#define ADIE_TSHK_DIG_CONFIG_INIT_M  ( ADIE_TSHK_DIG_CONFIG_M )

/* Sampling interval based on 2.4MHz clk
 * ie: 3 * (8/TCXO), 48 * (8/TCXO)
 * Use ADIE_TSHK_DIG_CONFIG_SAMPLE_INT_48_V for HKADC purposes
 */
#define ADIE_TSHK_DIG_CONFIG_SAMPLE_INT_M                 0xC0
#define ADIE_TSHK_DIG_CONFIG_SAMPLE_INT_3_V               0x00
#define ADIE_TSHK_DIG_CONFIG_SAMPLE_INT_24_V              0x40
#define ADIE_TSHK_DIG_CONFIG_SAMPLE_INT_36_V              0x80
#define ADIE_TSHK_DIG_CONFIG_SAMPLE_INT_48_V              0xC0

/* Number of samples taken
 */
#define ADIE_TSHK_DIG_CONFIG_NUM_SAMPLE_M                 0x30
#define ADIE_TSHK_DIG_CONFIG_NUM_SAMPLE_1_V               0x00
#define ADIE_TSHK_DIG_CONFIG_NUM_SAMPLE_4_V               0x10
#define ADIE_TSHK_DIG_CONFIG_NUM_SAMPLE_8_V               0x20
#define ADIE_TSHK_DIG_CONFIG_NUM_SAMPLE_16_V              0x30

/* Resolution of ADC reading
 * Always use 12 bits, no drawback known
 */
#define ADIE_TSHK_DIG_CONFIG_RES_M                        0x0C
#define ADIE_TSHK_DIG_CONFIG_RES_8_BITS_V                 0x00
#define ADIE_TSHK_DIG_CONFIG_RES_10_BITS_V                0x04
#define ADIE_TSHK_DIG_CONFIG_RES_12_BITS_V                0x08

/* Indicates whether the present conversion is going to be 
 * a touch screen measurement or not
 */
#define ADIE_TSHK_DIG_CONFIG_TS_SEL_M                     0x02
#define ADIE_TSHK_DIG_CONFIG_TS_SEL_HK_V                  0x00
#define ADIE_TSHK_DIG_CONFIG_TS_SEL_TS_V                  0x02

/* Convert on write to the INTF_CONFIG register
 */
#define ADIE_TSHK_DIG_CONFIG_CONV_ON_CNFG_WR_M            0x01
#define ADIE_TSHK_DIG_CONFIG_CONV_ON_CNFG_WR_DIS_V        0x00
#define ADIE_TSHK_DIG_CONFIG_CONV_ON_CNFG_WR_EN_V         0x01

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ADIE_TSHK_INTF_CONFIG_R (Register 0x50) TSHK intf config

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_TSHK_INTF_CONFIG_R                           0x50
#define ADIE_TSHK_INTF_CONFIG_M                           0xFF
#define ADIE_TSHK_INTF_CONFIG_INIT_V  ( ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_AIN0_V | \
                                 ADIE_TSHK_INTF_CONFIG_DRV_MODE_NO_OP_V)

/* Initialize everything, so full register mask */
#define ADIE_TSHK_INTF_CONFIG_INIT_M  ( ADIE_TSHK_INTF_CONFIG_M )

/* Select input
 */
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_M                0xF0
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_NONE_V           0x00
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_AIN0_V           0x10
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_AIN1_V           0x20
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_AIN2_V           0x30
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_WIPER_V          0x40
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_XP_UL_V          0x50
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_YP_UR_V          0x60
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_YM_LR_V          0x70
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_XM_LL_V          0x80
#define ADIE_TSHK_INTF_CONFIG_MUX_IN_SEL_AIN_INT_V        0x90

/* Select driver mode
 */
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_M                  0x0F
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_NO_OP_V            0x00
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_4_WIRE_X_COORD_V   0x01
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_4_WIRE_Y_COORD_V   0x02
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_4_WIRE_Z1_COORD_V  0x03
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_4_WIRE_Z2_COORD_V  0x04
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_5_WIRE_X_COORD_V   0x05
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_5_WIRE_Y_COORD_V   0x06
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_PEN_IRQ_EN_STATE_V 0x07
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_PRECHARGE_STATE_V  0x08
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_200_UA_Z1_V        0x09
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_200_UA_Z2_V        0x0A
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_400_UA_Z1_V        0x0B
#define ADIE_TSHK_INTF_CONFIG_DRV_MODE_400_UA_Z2_V        0x0C

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ADIE_TSHK_COMMAND_R (Register 0x51) TSHK command register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_TSHK_COMMAND_R                               0x51
#define ADIE_TSHK_COMMAND_M                               0xFF

/* Start a conversion when writing to this register
 */
#define ADIE_TSHK_COMMAND_CMD_WR_M                        0xFF
#define ADIE_TSHK_COMMAND_CMD_WR_V                        0xFF

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ADIE_TSHK_ANA_CONFIG_R (Register 0x52) TSHK analog config register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_TSHK_ANA_CONFIG_R                            0x52
#define ADIE_TSHK_ANA_CONFIG_M                            0xFF
#define ADIE_TSHK_ANA_CONFIG_INIT_V  ( ADIE_TSHK_ANA_CONFIG_GAIN_EC_0_4_PF_V          | \
                                       ADIE_TSHK_ANA_CONFIG_SHCOR_NO_CORR_V           | \
                                       ADIE_TSHK_ANA_CONFIG_GAIN_EC_DIS_V             | \
                                       ADIE_TSHK_ANA_CONFIG_DIFF_REF_SEL_AVDD_AGND_V  |\
                                       ADIE_TSHK_ANA_CONFIG_DELAY_BW_CONV_NONE_V)

/* Initialize everything, so full register mask */
#define ADIE_TSHK_ANA_CONFIG_INIT_M  ( ADIE_TSHK_ANA_CONFIG_M )

/* Amount of gain error correction bits
 */
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_M                    0xE0
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_0_4_PF_V             0x00
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_0_6_PF_V             0x20
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_0_8_PF_V             0x40
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_1_0_PF_V             0x60
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_1_2_PF_V             0x80
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_1_4_PF_V             0xA0
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_1_6_PF_V             0xC0
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_1_8_PF_V             0xE0

/* Amount of sample and hold coupling correction
 */
#define ADIE_TSHK_ANA_CONFIG_SHCOR_M                      0x18
#define ADIE_TSHK_ANA_CONFIG_SHCOR_NO_CORR_V              0x00
#define ADIE_TSHK_ANA_CONFIG_SHCOR_0_5_MV_V               0x08
#define ADIE_TSHK_ANA_CONFIG_SHCOR_1_0_MV_V               0x10
#define ADIE_TSHK_ANA_CONFIG_SHCOR_1_5_MV_V               0x18

/* Enable/disable gain correction
 */
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_EN_M                 0x04
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_DIS_V                0x00
#define ADIE_TSHK_ANA_CONFIG_GAIN_EC_EN_V                 0x04

/* Choose diff ref sel
 */
#define ADIE_TSHK_ANA_CONFIG_DIFF_REF_SEL_M               0x02
#define ADIE_TSHK_ANA_CONFIG_DIFF_REF_SEL_AVDD_AGND_V     0x00
#define ADIE_TSHK_ANA_CONFIG_DIFF_REF_SEL_DRIVERS_V       0x02

/* Delay between conversion
 */
#define ADIE_TSHK_ANA_CONFIG_DELAY_BW_CONV_M              0x01
#define ADIE_TSHK_ANA_CONFIG_DELAY_BW_CONV_NONE_V         0x00
#define ADIE_TSHK_ANA_CONFIG_DELAY_BW_CONV_3_2_US_V       0x01

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ADIE_TSHK_DATA_R (Register 0x53) TSHK data registers (most sig bits)
                 (read only)      (least sig bits are in TSHK_STATUS_R)

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/* Most significant bits of ADC data read only register 
 */
#define ADIE_TSHK_DATA_R                                  0x53
#define ADIE_TSHK_DATA_M                                  0xFF

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ADIE_TSHK_STATUS_R (Register 0x54) TSHK status register
                   (read only)

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_TSHK_STATUS_R                                0x54
#define ADIE_TSHK_STATUS_M                                0xFF

/* 4 least significant bits of ADC data read only register 
 */
#define ADIE_TSHK_STATUS_DATA_M                           0xF0

/* TS operation 0 means conversion done, 1 means in progress 
 * HKADC operation 0 means conversion not ready, 1 means done
 */
#define ADIE_TSHK_STATUS_ADC_EOC_M                        0x01
#define ADIE_TSHK_STATUS_ADC_EOC_CONV_DONE_V              0x01

#endif /* ADIE_SUPPORT_OLD_HKADC_DEFS */

#endif /* ADIE_ADC_DEFS_H */


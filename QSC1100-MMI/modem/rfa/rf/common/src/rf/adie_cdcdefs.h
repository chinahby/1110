#ifndef ADIE_CDCDEFS_H
#define ADIE_CDCDEFS_H
/*===========================================================================


   A N A L O G   D I E   C O D E C   R E G I S T E R   D E F S   H E A D E R

DESCRIPTION
  This file contains all the register and bit definitions for the codec in
  analog die for RaMSIS (SC2x)

  These definitions are used by the analog die c file and header file.  The
  definitions are the same for both targets that use memory mapped
  communication and SBI communication.

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/adie_cdcdefs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/07   ymc     Updated codec register definitions.
           ymc     Updated Copyright.
06/16/07   abh     Temporary code changes to never disable XO Therm ADC Clock
05/30/07   anb     Added some Dragon-4 codec defs to support in Sc2x 2.0 
                   before code freeze
12/18/06   ans     Added changes given by CODEC team.
12/01/06   dyc     Added some CODEC defs.
11/26/06   dyc     Created

===========================================================================*/


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

 Solo / RaMSIS CODEC Definitions

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_MIX_SIGNAL_PRESET_R                  0x00
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

REFERENCE_CONTROL1 : CODEC Reference Control Register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_REFERENCE_CONTROL1_R                               0x83
#define ADIE_REFERENCE_CONTROL1_INIT_M                          0xFC
#define ADIE_REFERENCE_CONTROL1_INIT_V                          0x18

#define ADIE_REF_CNTL1_BANDGAP_OPAMP_M            0xC0
#define ADIE_REF_CNTL1_BANDGAP_OPAMP_NOMINAL_V    0x00
#define ADIE_REF_CNTL1_BANDGAP_OPAMP_5_UA_V       0x40
#define ADIE_REF_CNTL1_BANDGAP_OPAMP_10_UA_V      0x80
#define ADIE_REF_CNTL1_BANDGAP_OPAMP_15_UA_V      0xC0

#define ADIE_REF_CNTL1_RX_DAC_M                   0x30
#define ADIE_REF_CNTL1_RX_DAC_90_UA_V             0x00
#define ADIE_REF_CNTL1_RX_DAC_80_UA_V             0x10
#define ADIE_REF_CNTL1_RX_DAC_65_UA_V             0x20
#define ADIE_REF_CNTL1_RX_DAC_55_UA_V             0x30

#define ADIE_REF_CNTL1_TXADC_VOCM_M               0x0E
#define ADIE_REF_CNTL1_TXADC_VOCM_38_UA_V         0x00
#define ADIE_REF_CNTL1_TXADC_VOCM_36_UA_V         0x02
#define ADIE_REF_CNTL1_TXADC_VOCM_34_UA_V         0x04
#define ADIE_REF_CNTL1_TXADC_VOCM_32_UA_V         0x06
#define ADIE_REF_CNTL1_TXADC_VOCM_30_UA_V         0x0E

/* bit 1 reserved */
/* bit 0 reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

REFERENCE_CONTROL2 : CODEC Reference Control Register 2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_REFERENCE_CONTROL2_R                              0x84
#define ADIE_REFERENCE_CONTROL2_INIT_M                         0xE0
#define ADIE_REFERENCE_CONTROL2_INIT_V                         0x00

#define ADIE_REF_CNTL2_BANDGAP_POWER_DOWN_M                    0x80
#define ADIE_REF_CNTL2_BANDGAP_POWER_DOWN_ENA_V                0x80
#define ADIE_REF_CNTL2_BANDGAP_POWER_DOWN_DIS_V                0x00

#define ADIE_REF_CNTL2_BANDGAP_REF_POWER_DOWN_ENA_M             0x40
#define ADIE_REF_CNTL2_BANDGAP_REF_POWER_DOWN_ENA_V             0x40
#define ADIE_REF_CNTL2_BANDGAP_REF_POWER_DOWN_DIS_V             0x00

#define ADIE_REF_CNTL2_CCOMP_RC_FILTER_ENA_M                    0x20
#define ADIE_REF_CNTL2_CCOMP_RC_FILTER_ENA_V                    0x00
#define ADIE_REF_CNTL2_CCOMP_RC_FILTER_DIS_V                    0x20

/* bit [0-4] reserved */

#define ADIE_REF_CNTL2_BANDGAP_ENA_M              0x80
#define ADIE_REF_CNTL2_BANDGAP_ENA_V              0x80
#define ADIE_REF_CNTL2_BANDGAP_DIS_V              0x00

#define ADIE_REF_CNTL2_REF_ENA_M                  0x40
#define ADIE_REF_CNTL2_REF_ENA_V                  0x40
#define ADIE_REF_CNTL2_REF_DIS_V                  0x00

/* bit [0-5] reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

REFERENCE_CONTROL3 : CODEC Reference Control Register 3

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_REFERENCE_CONTROL3_R                               0x85
#define ADIE_REFERENCE_CONTROL3_INIT_M            0x00
#define ADIE_REFERENCE_CONTROL3_INIT_V            0x00

/* bit [0-7] reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

AUX_PGA_CONTROL1 : CODEC Aux Pga Control Register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_AUX_PGA_CONTROL1_R                                 0x86
#define ADIE_AUX_PGA_CONTROL1_INIT_M                            0xFC
#define ADIE_AUX_PGA_CONTROL1_INIT_V                            0x08

#define ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_M                      0x80
#define ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_V                      0x80
#define ADIE_AUX_PGA_CNTL1_LEFT_CHAN_DIS_V                      0x00

#define ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_M                     0x40
#define ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_V                     0x40
#define ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_DIS_V                     0x00

#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_M                      0x3C
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_27P5_DB_V        0x00
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_24P5_DB_V        0x04
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_21P5_DB_V        0x08
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_18P5_DB_V        0x0C
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_15P5_DB_V        0x10
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_12P5_DB_V        0x14
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_09P5_DB_V        0x18
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_06P5_DB_V        0x1C
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_03P5_DB_V        0x20
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_00P5_DB_V        0x24
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_02P5_DB_V         0x28
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_05P5_DB_V         0x2C
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_08P5_DB_V         0x30
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_11P5_DB_V         0x34
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_14P5_DB_V         0x38
#define ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_17P5_DB_V         0x3C

/* bit [0-1] reserved */

#define ADIE_AUX_PGA_CNTL1_SOFT_RESET_ENA_M       0x01
#define ADIE_AUX_PGA_CNTL1_SOFT_RESET_ENA_V       0x01
#define ADIE_AUX_PGA_CNTL1_SOFT_RESET_DIS_V       0x00

#define ADIE_AUX_PGA_CNTL1_START_UP_ENB_M         0x02
#define ADIE_AUX_PGA_CNTL1_START_UP_ENA_V         0x00
#define ADIE_AUX_PGA_CNTL1_START_UP_DIS_V         0x02


#define ADIE_AUX_PGA_CNTL1_GAIN_SET_M             0xF0
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_11D5_DB_V     0x00
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_8D5_DB_V      0x10
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_5D5_DB_V      0x20
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_2D5_DB_V      0x30
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_N0D5_DB_V     0x40
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_N3D5_DB_V     0x50
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_N6D5_DB_V     0x60
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_N9D5_DB_V     0x70
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_N12D5_DB_V    0x80
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_N15D5_DB_V    0x90
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_N18D5_DB_V    0xA0
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_N21D5_DB_V    0xB0
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_N24D5_DB_V    0xC0
#define ADIE_AUX_PGA_CNTL1_GAIN_SET_N27D5_DB_V    0xD0

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

AUX_PGA_CONTROL2 : CODEC Aux Pga Control Register 2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_AUX_PGA_CONTROL2_R                                 0x87
#define ADIE_AUX_PGA_CONTROL2_INIT_M                            0x00
#define ADIE_AUX_PGA_CONTROL2_INIT_V                            0x00

/* bit [0-7] reserved */

#define ADIE_AUX_PGA_CNTL2_PGA_OPAMP_BIAS_M            0x80
#define ADIE_AUX_PGA_CNTL2_PGA_OPAMP_BIAS_2D5_UA_V     0x80
#define ADIE_AUX_PGA_CNTL2_PGA_OPAMP_BIAS_5_UA_V       0x00

#define ADIE_AUX_PGA_CNTL2_COMMON_MODE_BIAS_M          0x40
#define ADIE_AUX_PGA_CNTL2_COMMON_MODE_BIAS_2D5_UA_V   0x40
#define ADIE_AUX_PGA_CNTL2_COMMON_MODE_BIAS_5_UA_V     0x00

/* bit [0-5] reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG1 : CODEC Tx Front End Control Register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER1_R                                      0x88
#define ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M                                 0xFF
#define ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V                                 0x02

#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_M                                 0x80
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                 0x80
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_DIS_V                                 0x00

#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_M                                0x40
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_00_DB_V                          0x00
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                          0x40

#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_USB_IN_M                              0x20
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_USB_IN_ENA_V                          0x20
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_USB_IN_DIS_V                          0x00

#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_M                        0x10
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_MONO_DIFF_V              0x00
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V    0x10

#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_M                         0x08
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V                     0x08
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_DIS_V                     0x00

#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_M                         0x04
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_ENA_V                     0x04
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_DIS_V                     0x00

#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_M                         0x02
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V                     0x02
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_DIS_V                     0x00

#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_BYPASS_M                              0x01
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_BYPASS_ENA_V                          0x01
#define ADIE_CODEC_TX_CNTL_REG1_TX_FE_BYPASS_DIS_V                          0x00

#define ADIE_CODEC_TX_CNTL_REG1_SHT_TXFE_OUTPUT_ENA_M               0x01
#define ADIE_CODEC_TX_CNTL_REG1_SHT_TXFE_OUTPUT_ENA_V               0x01
#define ADIE_CODEC_TX_CNTL_REG1_SHT_TXFE_OUTPUT_DIS_V               0x00

#define ADIE_CODEC_TX_CNTL_REG1_MICAMP_ENA_M                        0x02
#define ADIE_CODEC_TX_CNTL_REG1_MICAMP_ENA_V                        0x02
#define ADIE_CODEC_TX_CNTL_REG1_MICAMP_DIS_V                        0x00

#define ADIE_CODEC_TX_CNTL_REG1_TXFE_BIAS_ENA_M                     0x04
#define ADIE_CODEC_TX_CNTL_REG1_TXFE_BIAS_ENA_V                     0x04
#define ADIE_CODEC_TX_CNTL_REG1_TXFE_BIAS_DIS_V                     0x00

#define ADIE_CODEC_TX_CNTL_REG1_MIC1_SEL_M                          0x08
#define ADIE_CODEC_TX_CNTL_REG1_MIC1_SEL_V                          0x08
#define ADIE_CODEC_TX_CNTL_REG1_MIC1_UNSEL_V                        0x00

#define ADIE_CODEC_TX_CNTL_REG1_MIC2_SEL_M                          0x10
#define ADIE_CODEC_TX_CNTL_REG1_MIC2_SEL_V                          0x10
#define ADIE_CODEC_TX_CNTL_REG1_MIC2_UNSEL_V                        0x00

#define ADIE_CODEC_TX_CNTL_REG1_LINEI_MODE_M                        0x60
#define ADIE_CODEC_TX_CNTL_REG1_LINEI_MODE_DISCONNECT_V             0x00
#define ADIE_CODEC_TX_CNTL_REG1_LINEI_AS_MONO_DIFF_I_SEL_V          0x20
#define ADIE_CODEC_TX_CNTL_REG1_LINEI_MODE_SINGLE_ENDED_V           0x40
#define ADIE4_CODEC_TX_CNTL_REG1_FM_STEREO_TO_MONO_DIFF_SEL_V       0x60

#define ADIE_CODEC_TX_CNTL_REG1_USB_MIC_SEL_M                       0x80
#define ADIE_CODEC_TX_CNTL_REG1_USB_MIC_SEL_V                       0x80
#define ADIE_CODEC_TX_CNTL_REG1_USB_MIC_UNSEL_V                     0x00

/* Dragon-3 variations */
#define ADIE_CODEC_TX_CNTL_REG1_LINEI_AS_MONO_DIFF_I_SEL_M          0x20
#define ADIE_CODEC_TX_CNTL_REG1_LINEI_AS_MONO_DIFF_I_UNSEL_V        0x00

#define ADIE_CODEC_TX_CNTL_REG1_FM_STEREO_TO_MONO_DIFF_SEL_M        0x40
#define ADIE_CODEC_TX_CNTL_REG1_FM_STEREO_TO_MONO_DIFF_SEL_V        0x40
#define ADIE_CODEC_TX_CNTL_REG1_FM_STEREO_TO_MONO_DIFF_UNSEL_V      0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG2 : CODEC Tx Front End Control Register 2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER2_R                                      0x89
#define ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M                                 0xFC
#define ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V                                 0x34

#define ADIE_CODEC_TX_CNTL_REG2_REFAMP_ICTL_M                 0x04
#define ADIE_CODEC_TX_CNTL_REG2_REFAMP_ICTL_2D5_UA_V          0x00
#define ADIE_CODEC_TX_CNTL_REG2_REFAMP_ICTL_5_UA_V            0x04

#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_ICTL_M                 0x08
#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_ICTL_2D5_UA_V          0x00
#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_ICTL_5_UA_V            0x08

#define ADIE_CODEC_TX_CNTL_REG2_BYPASS_AAF_ENA_M              0x10
#define ADIE_CODEC_TX_CNTL_REG2_BYPASS_AAF_ENA_V              0x10
#define ADIE_CODEC_TX_CNTL_REG2_BYPASS_AAF_DIS_V              0x00

#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_GAIN_M                 0x20
#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_GAIN_24_DB_V           0x00
#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_GAIN_0_DB_V            0x20

#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_STARTUP_DIS_M          0x40
#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_STARTUP_DIS_V          0x40
#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_STARTUP_ENA_V          0x00

#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_INPUT_RESET_ENA_M      0x80
#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_INPUT_RESET_ENA_V      0x80
#define ADIE_CODEC_TX_CNTL_REG2_MICAMP_INPUT_RESET_DIS_V      0x00

/* bit [0-1] reserved */
/* bit [0] reserved */

/* Dragon-3 variations */
#define ADIE_CODEC_TX_CNTL_REG2_RESERVE_BIT1_V                0x02

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG3 : CODEC Tx Front End Control Register 3

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER3_R                                      0x8A
#define ADIE_CODEC_TX_CNTL_REGISTER3_INIT_M                                 0x00
#define ADIE_CODEC_TX_CNTL_REGISTER3_INIT_V                                 0x00

/* bit [0-7] reserved */

#define ADIE_CODEC_TX_CNTL_REG3_SIGNBIT_DIFF_SEL_M            0x01
#define ADIE_CODEC_TX_CNTL_REG3_SIGNBIT_DIFF_SEL_V            0x01
#define ADIE_CODEC_TX_CNTL_REG3_SIGNBIT_DIFF_UNSEL_V          0x00

#define ADIE_CODEC_TX_CNTL_REG3_SIGNBIT_SEL_M                 0x02
#define ADIE_CODEC_TX_CNTL_REG3_SIGNBIT_SEL_V                 0x02
#define ADIE_CODEC_TX_CNTL_REG3_SIGNBIT_UNSEL_V               0x00

#define ADIE_CODEC_TX_CNTL_REG3_CLKSEL_M                      0x04
#define ADIE_CODEC_TX_CNTL_REG3_CLKSEL_CDC_CK_SEL_V           0x04
#define ADIE_CODEC_TX_CNTL_REG3_CLKSEL_CDC_DLY_CK_SEL_V       0x00

#define ADIE_CODEC_TX_CNTL_REG3_EN_VICMBUFF_M                 0x08
#define ADIE_CODEC_TX_CNTL_REG3_EN_VICMBUFF_ENA_V             0x08
#define ADIE_CODEC_TX_CNTL_REG3_EN_VICMBUFF_DIS_V             0x00

#define ADIE_CODEC_TX_CNTL_REG3_EN_CDC_TX_BIAS_M              0x10
#define ADIE_CODEC_TX_CNTL_REG3_EN_CDC_TX_BIAS_ENA_V          0x10
#define ADIE_CODEC_TX_CNTL_REG3_EN_CDC_TX_BIAS_DIS_V          0x00

#define ADIE_CODEC_TX_CNTL_REG3_EN_CURRENTPG_M                0x20
#define ADIE_CODEC_TX_CNTL_REG3_EN_CURRENTPG_ENA_V            0x20
#define ADIE_CODEC_TX_CNTL_REG3_EN_CURRENTPG_DIS_V            0x00

#define ADIE_CODEC_TX_CNTL_REG3_BYPASS_CURRENTPG_ENA_M        0x40
#define ADIE_CODEC_TX_CNTL_REG3_BYPASS_CURRENTPG_ENA_V        0x40
#define ADIE_CODEC_TX_CNTL_REG3_BYPASS_CURRENTPG_DIS_V        0x00

#define ADIE_CODEC_TX_CNTL_REG3_EN_TXADC_M                    0x80
#define ADIE_CODEC_TX_CNTL_REG3_EN_TXADC_ENA_V                0x80
#define ADIE_CODEC_TX_CNTL_REG3_EN_TXADC_DIS_V                0x00

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG4 : CODEC Tx Control Register 4

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER4_R                        0x50
#define ADIE_CODEC_TX_CNTL_REGISTER4_INIT_M                   0xFF
#define ADIE_CODEC_TX_CNTL_REGISTER4_INIT_V                   0x31

/*enables/disables DEM counter*/
#define ADIE_CODEC_TX_CNTL_REG4_EN_DEM_M                      0x01
#define ADIE_CODEC_TX_CNTL_REG4_EN_DEM_ENA_V                  0x01
#define ADIE_CODEC_TX_CNTL_REG4_EN_DEM_DIS_V                  0x00

/*enables/disables DEM*/
#define ADIE_CODEC_TX_CNTL_REG4_DISABLE_DEM_M                 0x02
#define ADIE_CODEC_TX_CNTL_REG4_DISABLE_DEM_DIS_V             0x02
#define ADIE_CODEC_TX_CNTL_REG4_DISABLE_DEM_ENA_V             0x00

#define ADIE_CODEC_TX_CNTL_REG4_ERR_EN_M                      0x04
#define ADIE_CODEC_TX_CNTL_REG4_ERR_EN_ENA_V                  0x04
#define ADIE_CODEC_TX_CNTL_REG4_ERR_EN_DIS_V                  0x00

#define ADIE_CODEC_TX_CNTL_REG4_FROM_XOR_M                    0x08
#define ADIE_CODEC_TX_CNTL_REG4_FROM_XOR_ENA_V                0x08
#define ADIE_CODEC_TX_CNTL_REG4_FROM_XOR_DIS_V                0x00

#define ADIE_CODEC_TX_CNTL_REG4_EN_CLK_ADV_M                  0x10
#define ADIE_CODEC_TX_CNTL_REG4_EN_CLK_ADV_ENA_V              0x10
#define ADIE_CODEC_TX_CNTL_REG4_EN_CLK_ADV_DIS_V              0x00

#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_M                   0xE0
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_ID6_UA_V            0x00
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_2ID6_UA_V           0x20
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_3ID6_UA_V           0x40
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_4ID6_UA_V           0x60
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_5ID6_UA_V           0x80
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_6ID6_UA_V           0xA0
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_7ID6_UA_V           0xC0
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_8ID6_UA_V           0xE0

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG5 : CODEC Tx Control Register 5

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER5_R                        0x51
#define ADIE_CODEC_TX_CNTL_REGISTER5_INIT_M                   0x7E
#define ADIE_CODEC_TX_CNTL_REGISTER5_INIT_V                   0x12

#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_M            0x0E
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_ID6_UA_V     0x00
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_2ID6_UA_V    0x02
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_3ID6_UA_V    0x04
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_4ID6_UA_V    0x06
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_5ID6_UA_V    0x08
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_6ID6_UA_V    0x0A
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_7ID6_UA_V    0x0C
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_8ID6_UA_V    0x0E

#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_M                   0x70
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_ID6_UA_V            0x00
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_2ID6_UA_V           0x10
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_3ID6_UA_V           0x20
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_4ID6_UA_V           0x30
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_5ID6_UA_V           0x40
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_6ID6_UA_V           0x50
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_7ID6_UA_V           0x60
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_8ID6_UA_V           0x70

/* bit 0 & 7 reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG6 : CODEC Tx Control Register 6

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER6_R                        0x52
#define ADIE_CODEC_TX_CNTL_REGISTER6_INIT_M                   0xFC
#define ADIE_CODEC_TX_CNTL_REGISTER6_INIT_V                   0x24

#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_M                   0x1C
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_ID6_UA_V            0x00
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_2ID6_UA_V           0x04
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_3ID6_UA_V           0x08
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_4ID6_UA_V           0x0C
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_5ID6_UA_V           0x10
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_6ID6_UA_V           0x14
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_7ID6_UA_V           0x18
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_8ID6_UA_V           0x1C

#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_M              0xE0
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_ID6_UA_V       0x00
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_2ID6_UA_V      0x20
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_3ID6_UA_V      0x40
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_4ID6_UA_V      0x60
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_5ID6_UA_V      0x80
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_6ID6_UA_V      0xA0
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_7ID6_UA_V      0xC0
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_8ID6_UA_V      0xE0
 
/* bit 0 & 1 reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG7 : CODEC Tx Control Register 7

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER7_R                        0x53
#define ADIE_CODEC_TX_CNTL_REGISTER7_INIT_M                   0xF0
#define ADIE_CODEC_TX_CNTL_REGISTER7_INIT_V                   0x80

#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_M                 0xF0
#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_DIS_MODE_V        0x00
#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_8K_ENA_V          0x80
#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_16K_ENA_V         0xC0
#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_32K_ENA_V         0xE0
#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_48K_ENA_V         0xF0

/* bit [0-3] reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG8 : CODEC Tx Control Register 8

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER8_R                        0x54
#define ADIE_CODEC_TX_CNTL_REGISTER8_INIT_M                   0xFF
#define ADIE_CODEC_TX_CNTL_REGISTER8_INIT_V                   0x00

/* NOTE: this is reserved register for Dragon-4 */

/* Dragon-3 variations */

#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA2_LOCAL_M             0x03
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA2_LOCAL_ID1_UA_V      0x03
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA2_LOCAL_ID2_UA_V      0x02
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA2_LOCAL_ID0_UA_V      0x00

#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_M             0x3C
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_ID0_UA_V      0x00
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_ID4_UA_V      0x20
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_2ID4_UA_V     0x30
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_3ID4_UA_V     0x38
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_4ID4_UA_V     0x3C

#define ADIE_CODEC_TX_CNTL_REG8_ICTL_COMP_LOCAL_M             0xC0
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_COMP_LOCAL_ID0_UA_V      0x00
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_COMP_LOCAL_ID2_UA_V      0x80
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_COMP_LOCAL_ID1_UA_V      0xC0

#define ADIE_CODEC_TX_CNTL_REGISTER4_R                        0x50
#define ADIE_CODEC_TX_CNTL_REGISTER4_INIT_M                   0xFF
#define ADIE_CODEC_TX_CNTL_REGISTER4_INIT_V                   0x31

/*enables/disables DEM counter*/
#define ADIE_CODEC_TX_CNTL_REG4_EN_DEM_M                      0x01
#define ADIE_CODEC_TX_CNTL_REG4_EN_DEM_ENA_V                  0x01
#define ADIE_CODEC_TX_CNTL_REG4_EN_DEM_DIS_V                  0x00

/*enables/disables DEM*/
#define ADIE_CODEC_TX_CNTL_REG4_DISABLE_DEM_M                 0x02
#define ADIE_CODEC_TX_CNTL_REG4_DISABLE_DEM_DIS_V             0x02
#define ADIE_CODEC_TX_CNTL_REG4_DISABLE_DEM_ENA_V             0x00

#define ADIE_CODEC_TX_CNTL_REG4_ERR_EN_M                      0x04
#define ADIE_CODEC_TX_CNTL_REG4_ERR_EN_ENA_V                  0x04
#define ADIE_CODEC_TX_CNTL_REG4_ERR_EN_DIS_V                  0x00

#define ADIE_CODEC_TX_CNTL_REG4_FROM_XOR_M                    0x08
#define ADIE_CODEC_TX_CNTL_REG4_FROM_XOR_ENA_V                0x08
#define ADIE_CODEC_TX_CNTL_REG4_FROM_XOR_DIS_V                0x00

#define ADIE_CODEC_TX_CNTL_REG4_EN_CLK_ADV_M                  0x10
#define ADIE_CODEC_TX_CNTL_REG4_EN_CLK_ADV_ENA_V              0x10
#define ADIE_CODEC_TX_CNTL_REG4_EN_CLK_ADV_DIS_V              0x00

#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_M                   0xE0
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_ID6_UA_V            0x00
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_2ID6_UA_V           0x20
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_3ID6_UA_V           0x40
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_4ID6_UA_V           0x60
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_5ID6_UA_V           0x80
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_6ID6_UA_V           0xA0
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_7ID6_UA_V           0xC0
#define ADIE_CODEC_TX_CNTL_REG4_ICTL_OTA1_8ID6_UA_V           0xE0

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG5 : CODEC Tx Control Register 5

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER5_R                        0x51
#define ADIE_CODEC_TX_CNTL_REGISTER5_INIT_M                   0x7E
#define ADIE_CODEC_TX_CNTL_REGISTER5_INIT_V                   0x12

#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_M            0x0E
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_ID6_UA_V     0x00
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_2ID6_UA_V    0x02
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_3ID6_UA_V    0x04
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_4ID6_UA_V    0x06
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_5ID6_UA_V    0x08
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_6ID6_UA_V    0x0A
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_7ID6_UA_V    0x0C
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_VICM_BUFFER_8ID6_UA_V    0x0E

#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_M                   0x70
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_ID6_UA_V            0x00
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_2ID6_UA_V           0x10
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_3ID6_UA_V           0x20
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_4ID6_UA_V           0x30
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_5ID6_UA_V           0x40
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_6ID6_UA_V           0x50
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_7ID6_UA_V           0x60
#define ADIE_CODEC_TX_CNTL_REG5_ICTL_OTA2_8ID6_UA_V           0x70

/* bit 0 & 7 reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG6 : CODEC Tx Control Register 6

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER6_R                        0x52
#define ADIE_CODEC_TX_CNTL_REGISTER6_INIT_M                   0xFC
#define ADIE_CODEC_TX_CNTL_REGISTER6_INIT_V                   0x24

#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_M                   0x1C
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_ID6_UA_V            0x00
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_2ID6_UA_V           0x04
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_3ID6_UA_V           0x08
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_4ID6_UA_V           0x0C
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_5ID6_UA_V           0x10
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_6ID6_UA_V           0x14
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_7ID6_UA_V           0x18
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_COMP_8ID6_UA_V           0x1C

#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_M              0xE0
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_ID6_UA_V       0x00
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_2ID6_UA_V      0x20
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_3ID6_UA_V      0x40
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_4ID6_UA_V      0x60
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_5ID6_UA_V      0x80
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_6ID6_UA_V      0xA0
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_7ID6_UA_V      0xC0
#define ADIE_CODEC_TX_CNTL_REG6_ICTL_CMBUFFERS_8ID6_UA_V      0xE0
 
/* bit 0 & 1 reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG7 : CODEC Tx Control Register 7

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER7_R                        0x53
#define ADIE_CODEC_TX_CNTL_REGISTER7_INIT_M                   0xF0
#define ADIE_CODEC_TX_CNTL_REGISTER7_INIT_V                   0x80

#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_M                 0xF0
#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_DIS_MODE_V        0x00
#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_8K_ENA_V          0x80
#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_16K_ENA_V         0xC0
#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_32K_ENA_V         0xE0
#define ADIE_CODEC_TX_CNTL_REG7_ICTL_DACREF_48K_ENA_V         0xF0

/* bit [0-3] reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_CNTL_REG8 : CODEC Tx Control Register 8

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_CNTL_REGISTER8_R                        0x54
#define ADIE_CODEC_TX_CNTL_REGISTER8_INIT_M                   0xFF
#define ADIE_CODEC_TX_CNTL_REGISTER8_INIT_V                   0x00

/* NOTE: this is reserved register for Dragon-4 */

/* Dragon-3 variations */

#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA2_LOCAL_M             0x03
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA2_LOCAL_ID1_UA_V      0x03
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA2_LOCAL_ID2_UA_V      0x02
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA2_LOCAL_ID0_UA_V      0x00

#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_M             0x3C
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_ID0_UA_V      0x00
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_ID4_UA_V      0x20
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_2ID4_UA_V     0x30
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_3ID4_UA_V     0x38
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_OTA1_LOCAL_4ID4_UA_V     0x3C

#define ADIE_CODEC_TX_CNTL_REG8_ICTL_COMP_LOCAL_M             0xC0
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_COMP_LOCAL_ID0_UA_V      0x00
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_COMP_LOCAL_ID2_UA_V      0x80
#define ADIE_CODEC_TX_CNTL_REG8_ICTL_COMP_LOCAL_ID1_UA_V      0xC0

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_ADC_REG : CODEC Tx ADC Register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_ADC_REGISTER_R                                        0x8B
#define ADIE_CODEC_TX_ADC_REGISTER_INIT_M                                   0xFE
#define ADIE_CODEC_TX_ADC_REGISTER_INIT_V                                   0x04

#define ADIE_CODEC_TX_ADC_REF_M                                             0x80
#define ADIE_CODEC_TX_ADC_REF_ENA_V                                         0x80
#define ADIE_CODEC_TX_ADC_REF_DIS_V                                         0x00

#define ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_M                              0x40
#define ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                          0x40
#define ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_DIS_V                          0x00

#define ADIE_CODEC_TX_ADC_OTA1_M                                            0x20
#define ADIE_CODEC_TX_ADC_OTA1_ENA_V                                        0x20
#define ADIE_CODEC_TX_ADC_OTA1_DIS_V                                        0x00

#define ADIE_CODEC_TX_ADC_OTA2_M                                            0x10
#define ADIE_CODEC_TX_ADC_OTA2_ENA_V                                        0x10
#define ADIE_CODEC_TX_ADC_OTA2_DIS_V                                        0x00

#define ADIE_CODEC_TX_ADC_COMPARATOR_M                                      0x08
#define ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                  0x08
#define ADIE_CODEC_TX_ADC_COMPARATOR_DIS_V                                  0x00

#define ADIE_CODEC_TX_ADC_DEM_M                                             0x04
#define ADIE_CODEC_TX_ADC_DEM_ENA_V                                         0x04
#define ADIE_CODEC_TX_ADC_DEM_DIS_V                                         0x00

#define ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_M                                0x02
#define ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V                            0x02
#define ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_DIS_V                            0x00

/* bit [0] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_ADC_REG1 : CODEC Tx ADC Control Register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R                                  0x8C
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M                             0xFF
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_V                             0x58

#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_M                      0xC0
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_05_UA_V                0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                0x40
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_15_UA_V                0x80
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_20_UA_V                0xC0

#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_M                      0x30
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_080_UA_V               0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V               0x10
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_240_UA_V               0x20
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_320_UA_V               0x30

#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_M                               0x0C
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P8_VOLT_V                      0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P7_VOLT_V                      0x04
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V                      0x08
#define ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P5_VOLT_V                      0x0C

#define ADIE_CODEC_TX_ADC_CNTL_REG1_VREFMID_REF_BUFFER_BIAS_M               0x03
#define ADIE_CODEC_TX_ADC_CNTL_REG1_VREFMID_REF_BUFFER_BIAS_020_UA_V        0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG1_VREFMID_REF_BUFFER_BIAS_040_UA_V        0x01
#define ADIE_CODEC_TX_ADC_CNTL_REG1_VREFMID_REF_BUFFER_BIAS_080_UA_V        0x02
#define ADIE_CODEC_TX_ADC_CNTL_REG1_VREFMID_REF_BUFFER_BIAS_160_UA_V        0x03

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_ADC_REG2 : CODEC Tx ADC Control Register 2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R                                  0x8D
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M                             0xFF
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_V                             0x64

#define ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_M                  0xC0
#define ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_333_MVOLT_V        0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V        0x40
#define ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_378_MVOLT_V        0x80
#define ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_400_MVOLT_V        0xC0

#define ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_M                  0x30
#define ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_050_UA_V           0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_100_UA_V           0x10
#define ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V           0x20
#define ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_400_UA_V           0x30

#define ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_M                0x0C
#define ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P10_VOLT_V      0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V      0x04
#define ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P20_VOLT_V      0x08
#define ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P25_VOLT_V      0x0C

#define ADIE_CODEC_TX_ADC_CNTL_REG2_VOCM_BUFFER_BIAS_M                      0x02
#define ADIE_CODEC_TX_ADC_CNTL_REG2_VOCM_BUFFER_BIAS_050_UA_V               0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG2_VOCM_BUFFER_BIAS_100_UA_V               0x02

#define ADIE_CODEC_TX_ADC_CNTL_REG2_OUTPUT_TO_MSM_M                         0x01
#define ADIE_CODEC_TX_ADC_CNTL_REG2_OUTPUT_TO_MSM_ENA_V                     0x01
#define ADIE_CODEC_TX_ADC_CNTL_REG2_OUTPUT_TO_MSM_DIS_V                     0x00

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_ADC_REG3 : CODEC Tx ADC Control Register 3

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R                                  0x8E
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M                             0xFE
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_V                             0x64

#define ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_M                0xC0
#define ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P85_VOLT_V      0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V      0x40
#define ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P95_VOLT_V      0x80
#define ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_1P00_VOLT_V      0xC0

#define ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_M                             0x30
#define ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_010_UA_V                      0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_015_UA_V                      0x10
#define ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                      0x20
#define ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_025_UA_V                      0x30

#define ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_M                             0x0C
#define ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_005_UA_V                      0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V                      0x04
#define ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_015_UA_V                      0x08
#define ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_020_UA_V                      0x0C

#define ADIE_CODEC_TX_ADC_CNTL_REG3_COMPARATOR_BIAS_M                       0x02
#define ADIE_CODEC_TX_ADC_CNTL_REG3_COMPARATOR_BIAS_05_UA_V                 0x00
#define ADIE_CODEC_TX_ADC_CNTL_REG3_COMPARATOR_BIAS_10_UA_V                 0x02

/* bit [0] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_ADC_REG4 : CODEC Tx ADC Control Register 4

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER4_R                                  0x8F
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER4_INIT_M                             0x00
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER4_INIT_V                             0x00

/* bit [0-7] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_ADC_REG5 : CODEC Tx ADC Control Register 5

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER5_R                                  0x90
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER5_INIT_M                             0x00
#define ADIE_CODEC_TX_ADC_CNTL_REGISTER5_INIT_V                             0x00

/* bit [0-7] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_FILTER_CNTL_REG1 : CODEC Rx Filter Control Register1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_FILTER_CONTROL_REGISTER1_R                                  0x91
//#define ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M                             0xFE
//vsudhir changed for testing. - please dont use this unless confirmed 19th Mar 2008
#define ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M                             0xFF
#define ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_V                             0x00

#define ADIE_RX_FILTER_CONTROL1_FILTER_M                                    0x80
#define ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                0x80
#define ADIE_RX_FILTER_CONTROL1_FILTER_DIS_V                                0x00

#define ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_M                          0x40
#define ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                      0x40
#define ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_DIS_V                      0x00

#define ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_M                         0x20
#define ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                     0x20
#define ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_DIS_V                     0x00

#define ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_M                             0x10
#define ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_BIQUAD_V                      0x00
#define ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                   0x10

#define ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_M                          0x0C
#define ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_RISING_EDGE_RX_INPUT_CLK_V 0x00
#define ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V    0x04
#define ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_RISING_EDGE_SYNC_CLK_V     0x08
#define ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_RISING_EDGE_SYNC_CLK_01_V  0x0C

#define ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_M                  0x02
#define ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V              0x02
#define ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_DIS_V              0x00

/*Bit 0 in ULC should be set 1 in 0x91*/
#define ADIE_RX_FILTER_CONTROL1_BIT0_V                                      0x01  

#define ADIE_RX_FILTER_CONTROL1_EN_BQ_IPT_SRTL_M      0x01
#define ADIE_RX_FILTER_CONTROL1_EN_BQ_IPT_SRTL_ENA_V  0x01
#define ADIE_RX_FILTER_CONTROL1_EN_BQ_IPT_SRTL_DIS_V  0x00

#define ADIE_RX_FILTER_CONTROL1_EN_BQ_IPT_SRTR_M      0x02
#define ADIE_RX_FILTER_CONTROL1_EN_BQ_IPT_SRTR_ENA_V  0x02
#define ADIE_RX_FILTER_CONTROL1_EN_BQ_IPT_SRTR_DIS_V  0x00

#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IML_M          0x04
#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IML_ENA_V      0x04
#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IML_DIS_V      0x00

#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IPL_M          0x08
#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IPL_ENA_V      0x08
#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IPL_DIS_V      0x00

#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IMR_M          0x10
#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IMR_ENA_V      0x10
#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IMR_DIS_V      0x00

#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IPR_M          0x20
#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IPR_ENA_V      0x20
#define ADIE_RX_FILTER_CONTROL1_EN_BUF_IPR_DIS_V      0x00

#define ADIE_RX_FILTER_CONTROL1_EN_OTA1L_OUT_M        0x40
#define ADIE_RX_FILTER_CONTROL1_EN_OTA1L_OUT_ENA_V    0x40
#define ADIE_RX_FILTER_CONTROL1_EN_OTA1L_OUT_DIS_V    0x00

#define ADIE_RX_FILTER_CONTROL1_EN_OTA1R_OUT_M        0x80
#define ADIE_RX_FILTER_CONTROL1_EN_OTA1R_OUT_ENA_V    0x80
#define ADIE_RX_FILTER_CONTROL1_EN_OTA1R_OUT_DIS_V    0x00

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_CNTL_REG2 : CODEC Rx Filter Control Register2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_FILTER_CONTROL_REGISTER2_R                                  0x92
#define ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M                             0xF8
#define ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V                             0x00

#define ADIE_RX_FILTER_CONTROL2_LEFT_CHAN_LOOPBACK_M                        0x80
#define ADIE_RX_FILTER_CONTROL2_LEFT_CHAN_LOOPBACK_NORMAL_V                 0x00
#define ADIE_RX_FILTER_CONTROL2_LEFT_CHAN_LOOPBACK_TX_TO_RX_V               0x80

#define ADIE_RX_FILTER_CONTROL2_RIGHT_CHAN_LOOPBACK_M                       0x40
#define ADIE_RX_FILTER_CONTROL2_RIGHT_CHAN_LOOPBACK_NORMAL_V                0x00
#define ADIE_RX_FILTER_CONTROL2_RIGHT_CHAN_LOOPBACK_TX_TO_RX_V              0x40

#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_M         0x07
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_OFF_V     0x00
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_I_V       0x01
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_2I_V      0x02
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_3I_V      0x03
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_4I_V      0x04
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_5I_V      0x05
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_6I_V      0x06
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_7I_V      0x07

#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_8K_V      0x01
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_12K_V     0x02
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_16K_V     0x02
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_22D05K_V  0x02
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_24K_V     0x03
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_32K_V     0x03
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_44D1K_V   0x04
#define ADIE_RX_FILTER_CONTROL2_BUFFER_ICTL_48K_V     0x04

#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_M         0x38
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_OFF_V     0x00
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_I_V       0x08
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_2I_V      0x10
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_3I_V      0x18
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_4I_V      0x20
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_5I_V      0x28
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_6I_V      0x30
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_7I_V      0x38

#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_8K_V      0x08
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_12K_V     0x10
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_16K_V     0x10
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_22D05K_V  0x10
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_24K_V     0x18
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_32K_V     0x18
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_44D1K_V   0x20
#define ADIE_RX_FILTER_CONTROL2_OTA1N2_ICTL_48K_V     0x20

#define ADIE_RX_FILTER_CONTROL2_OTA_CASCODE_B0_ENB_M  0x40
#define ADIE_RX_FLTR_CNTL_REG2_OTA_CASCODE_B0_ENA_V   0x00
#define ADIE_RX_FLTR_CNTL_REG2_OTA_CASCODE_B0_DIS_V   0x40

#define ADIE_RX_FILTER_CONTROL2_OTA_CASCODE_B1_ENB_M  0x80
#define ADIE_RX_FLTR_CNTL_REG2_OTA_CASCODE_B1_ENA_V   0x00
#define ADIE_RX_FLTR_CNTL_REG2_OTA_CASCODE_B1_DIS_V   0x80


/*(Dragon4/5)--44.1/48k DAC/OTA reference values*/
#define ADIE4_RX_FLTR_CNTL_REG2_OTA1N2_48K_ENA_V      0xE4
#define ADIE4_RX_FLTR_CNTL_REG2_OTA1N2_44D1K_ENA_V    0xE4
/*(Dragon4/5)--24/32K DAC/OTA reference values*/
#define ADIE4_RX_FLTR_CNTL_REG2_OTA1N2_32K_ENA_V      0xDB
#define ADIE4_RX_FLTR_CNTL_REG2_OTA1N2_24K_ENA_V      0xDB



/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_CNTL_REG3 : CODEC Rx Filter Control Register3

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_FILTER_CONTROL_REGISTER3_R                                  0x93
#define ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M                             0xFC
#define ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_V                             0x00

#define ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_M                      0xC0
#define ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_60_UA_V                0x00
#define ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_45_UA_V                0x40
#define ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_30_UA_V                0x80
#define ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                0xC0

#define ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_M                      0x30
#define ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_300_UA_V               0x00
#define ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_225_UA_V               0x10
#define ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_150_UA_V               0x20
#define ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V               0x30

#define ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_M                      0x0C
#define ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_300_UA_V               0x00
#define ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_225_UA_V               0x04
#define ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_150_UA_V               0x08
#define ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V               0x0C


/* bit [0-1] reserved */


#define ADIE_RX_FILTER_CONTROL3_EN_PA_SHT_M           0x01
#define ADIE_RX_FLTR_CNTL_REG3_EN_PA_SHT_ENA_V        0x01
#define ADIE_RX_FLTR_CNTL_REG3_EN_PA_SHT_DIS_V        0x00

#define ADIE_RX_FILTER_CONTROL3_SYNC_SIGN_M           0x02
#define ADIE_RX_FLTR_CNTL_REG3_SYNC_SIGN_ENA_V        0x02
#define ADIE_RX_FLTR_CNTL_REG3_SYNC_SIGN_DIS_V        0x00

#define ADIE_RX_FILTER_CONTROL3_EN_RX_CKL_M           0x04
#define ADIE_RX_FLTR_CNTL_REG3_EN_RX_CKL_ENA_V        0x04
#define ADIE_RX_FLTR_CNTL_REG3_EN_RX_CKL_DIS_V        0x00

#define ADIE_RX_FILTER_CONTROL3_EN_RX_CKR_M           0x08
#define ADIE_RX_FLTR_CNTL_REG3_EN_RX_CKR_ENA_V        0x08
#define ADIE_RX_FLTR_CNTL_REG3_EN_RX_CKR_DIS_V        0x00

#define ADIE_RX_FILTER_CONTROL3_EN_BQ_L_M             0x10
#define ADIE_RX_FLTR_CNTL_REG3_EN_BQ_L_ENA_V          0x10
#define ADIE_RX_FLTR_CNTL_REG3_EN_BQ_L_DIS_V          0x00

#define ADIE_RX_FILTER_CONTROL3_EN_BQ_R_M             0x20
#define ADIE_RX_FLTR_CNTL_REG3_EN_BQ_R_ENA_V          0x20
#define ADIE_RX_FLTR_CNTL_REG3_EN_BQ_R_DIS_V          0x00

#define ADIE_RX_FILTER_CONTROL3_EN_TST_BQDOUTL_M      0x40
#define ADIE_RX_FILTER_CONTROL3_EN_TST_BQDOUTL_ENA_V  0x40
#define ADIE_RX_FILTER_CONTROL3_EN_TST_BQDOUTL_DIS_V  0x00

#define ADIE_RX_FILTER_CONTROL3_EN_TST_BQDOUTR_M      0x80
#define ADIE_RX_FILTER_CONTROL3_EN_TST_BQDOUTR_ENA_V  0x80
#define ADIE_RX_FILTER_CONTROL3_EN_TST_BQDOUTR_DIS_V  0x00

/* Dragon-3 variations */
/* bits 6 & 7 are resereved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_CNTL_REG4 : CODEC Rx Filter Control Register4

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_FILTER_CONTROL_REGISTER4_R                                  0x94
#define ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M                             0xFC
#define ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V                             0x00

#define ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_M                       0xC0
#define ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_420_UA_V                0x00
#define ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_315_UA_V                0x40
#define ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_210_UA_V                0x80
#define ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                0xC0

#define ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_M                                 0x30
#define ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_020_UA_V                          0x00
#define ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_015_UA_V                          0x10
#define ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_100_UA_V                          0x20
#define ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                          0x30

#define ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_M                                 0x0C
#define ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_20_UA_V                           0x00
#define ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_15_UA_V                           0x04
#define ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_10_UA_V                           0x08
#define ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V                           0x0C


/* bit [0-1] reserved */


#define ADIE_RX_FILTER_CONTROL4_TMUX_CTL_M_M         0x01
#define ADIE_RX_FILTER_CONTROL4_TMUX_CTL_M_ENA_V     0x01
#define ADIE_RX_FILTER_CONTROL4_TMUX_CTL_M_DIS_V     0x00

#define ADIE_RX_FILTER_CONTROL4_TMUX_CTL_P_M         0x02
#define ADIE_RX_FILTER_CONTROL4_TMUX_CTL_P_ENA_V     0x02
#define ADIE_RX_FILTER_CONTROL4_TMUX_CTL_P_DIS_V     0x00

#define ADIE_RX_FILTER_CONTROL4_TST_IBG_L_M          0x04
#define ADIE_RX_FILTER_CONTROL4_TST_IBG_L_ENA_V      0x04
#define ADIE_RX_FILTER_CONTROL4_TST_IBG_L_DIS_V      0x00

#define ADIE_RX_FILTER_CONTROL4_TST_IPTAT_L_M        0x08
#define ADIE_RX_FILTER_CONTROL4_TST_IPTAT_L_ENA_V    0x08
#define ADIE_RX_FILTER_CONTROL4_TST_IPTAT_L_DIS_V    0x00

#define ADIE_RX_FILTER_CONTROL4_SEL_DATA_TEST_M      0x10
#define ADIE_RX_FILTER_CONTROL4_SEL_DATA_TEST_ENA_V  0x10
#define ADIE_RX_FILTER_CONTROL4_SEL_DATA_TEST_DIS_V  0x00

#define ADIE_RX_FILTER_CONTROL4_SEL_DWA_TEST_M       0x20
#define ADIE_RX_FILTER_CONTROL4_SEL_DWA_TEST_ENA_V   0x20
#define ADIE_RX_FILTER_CONTROL4_SEL_DWA_TEST_DIS_V   0x00

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_CNTL_REG5 : CODEC Rx Filter Control Register5

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define ADIE_RX_FILTER_CONTROL_REGISTER5_R                                  0x95
#define ADIE_RX_FILTER_CONTROL_REGISTER5_INIT_M                             0x00
#define ADIE_RX_FILTER_CONTROL_REGISTER5_INIT_V                             0x00

/* bit [0-7] reserved */

#define ADIE_RX_FILTER_CONTROL5_SEL_LOOP_EXTERNAL_M       0x01
#define ADIE_RX_FLTR_CNTL_REG5_SEL_LOOP_EXTERNAL_SEL_V    0x01
#define ADIE_RX_FLTR_CNTL_REG5_SEL_LOOP_EXTERNAL_UNSEL_V  0x00

#define ADIE_RX_FILTER_CONTROL5_SEL_LOOP_INTERNAL_M       0x02
#define ADIE_RX_FLTR_CNTL_REG5_SEL_LOOP_INTERNAL_SEL_V    0x02
#define ADIE_RX_FLTR_CNTL_REG5_SEL_LOOP_INTERNAL_UNSEL_V  0x00

/* bit 2 reserved*/

#define ADIE_RX_FILTER_CONTROL5_LOOPBACK_L_M              0x08
#define ADIE_RX_FLTR_CNTL_REG5_LOOPBACK_L_SEL_V           0x08
#define ADIE_RX_FLTR_CNTL_REG5_LOOPBACK_L_UNSEL_V         0x00

#define ADIE_RX_FILTER_CONTROL5_LOOPBACK_R_M              0x10
#define ADIE_RX_FLTR_CNTL_REG5_LOOPBACK_R_SEL_V           0x10
#define ADIE_RX_FLTR_CNTL_REG5_LOOPBACK_R_UNSEL_V         0x00

/*bits [5-7] are reserved*/

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_CNTL_REG6 : CODEC Rx Filter Control Register 6

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_FILTER_CONTROL_REGISTER6_R                                  0x96
#define ADIE_RX_FILTER_CONTROL_REGISTER6_INIT_M                             0x00
#define ADIE_RX_FILTER_CONTROL_REGISTER6_INIT_V                             0x00

/* bit [0-7] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_ENABLE_REG1 : CODEC Rx PA Enable Register1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_ENABLE_REGISTER1_R                                       0x97
#define ADIE_RX_PA_ENABLE_REGISTER1_INIT_M                                  0xFE
#define ADIE_RX_PA_ENABLE_REGISTER1_INIT_V                                  0x00

#define ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_M                            0x80
#define ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                        0x80
#define ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_DIS_V                        0x00

#define ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_M                           0x40
#define ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                       0x40
#define ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_DIS_V                       0x00

#define ADIE_RX_PA_ENA_REG1_HPH_LEFT_M                                      0x20
#define ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                  0x20
#define ADIE_RX_PA_ENA_REG1_HPH_LEFT_DIS_V                                  0x00

#define ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_M                          0x10
#define ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                      0x10
#define ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_DIS_V                      0x00

#define ADIE_RX_PA_ENA_REG1_HPH_RIGHT_M                                     0x08
#define ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V                                 0x08
#define ADIE_RX_PA_ENA_REG1_HPH_RIGHT_DIS_V                                 0x00

#define ADIE_RX_PA_ENA_REG1_EAR_PA_REF_BUFFER_M                             0x04
#define ADIE_RX_PA_ENA_REG1_EAR_PA_REF_BUFFER_ENA_V                         0x04
#define ADIE_RX_PA_ENA_REG1_EAR_PA_REF_BUFFER_DIS_V                         0x00

#define ADIE_RX_PA_ENA_REG1_EAR_PA_M                                        0x02
#define ADIE_RX_PA_ENA_REG1_EAR_PA_ENA_V                                    0x02
#define ADIE_RX_PA_ENA_REG1_EAR_PA_DIS_V                                    0x00

/* bit [0] reserved */

#define ADIE_RX_PA_ENABLE1_EN_LOPMIC_BUF_M         0x01
#define ADIE_RX_PA_ENA_REG1_EN_LOPMIC_BUF_ENA_V    0x01
#define ADIE_RX_PA_ENA_REG1_EN_LOPMIC_BUF_DIS_V    0x00

#define ADIE_RX_PA_ENABLE1_EN_LOEXT_R_M            0x02
#define ADIE_RX_PA_ENA_REG1_EN_LOEXT_R_ENA_V       0x02
#define ADIE_RX_PA_ENA_REG1_EN_LOEXT_R_DIS_V       0x00

#define ADIE_RX_PA_ENABLE1_EN_LOEXT_L_M            0x04
#define ADIE_RX_PA_ENA_REG1_EN_LOEXT_L_ENA_V       0x04
#define ADIE_RX_PA_ENA_REG1_EN_LOEXT_L_DIS_V       0x00

#define ADIE_RX_PA_ENABLE1_EN_LOEXT_BUF_M          0x08
#define ADIE_RX_PA_ENA_REG1_EN_LOEXT_BUF_ENA_V     0x08
#define ADIE_RX_PA_ENA_REG1_EN_LOEXT_BUF_DIS_V     0x00

#define ADIE_RX_PA_ENABLE1_STARTUP_M               0x10
#define ADIE_RX_PA_ENA_REG1_STARTUP_NORMAL_V       0x00
/* DO NOT USE THIS UNLESS PROBLEM!*/
#define ADIE_RX_PA_ENA_REG1_STARTUP_GROUND_V       0x10

#define ADIE_RX_PA_ENABLE1_EN_EAR_M                0x20
#define ADIE_RX_PA_ENA_REG1_EN_EAR_ENA_V           0x20
#define ADIE_RX_PA_ENA_REG1_EN_EAR_DIS_V           0x00

#define ADIE_RX_PA_ENABLE1_EN_BUF_M                0x40
#define ADIE_RX_PA_ENA_REG1_EN_BUF_ENA_V           0x40
#define ADIE_RX_PA_ENA_REG1_EN_BUF_DIS_V           0x00

#define ADIE_RX_PA_ENABLE1_IBIAS_EN_M              0x80
#define ADIE_RX_PA_ENA_REG1_IBIAS_EN_ENA_V         0x80
#define ADIE_RX_PA_ENA_REG1_IBIAS_EN_DIS_V         0x00

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_ENABLE_REG2 : CODEC Rx PA Enable Register2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_ENABLE_REGISTER2_R                                       0x98
#define ADIE_RX_PA_ENABLE_REGISTER2_INIT_M                                  0xFF
#define ADIE_RX_PA_ENABLE_REGISTER2_INIT_V                                  0x00

#define ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_REF_BUFFER_M                      0x80
#define ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_REF_BUFFER_ENA_V                  0x80
#define ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_REF_BUFFER_DIS_V                  0x00

#define ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_M                                 0x40
#define ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_ENA_V                             0x40
#define ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_DIS_V                             0x00

#define ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_REF_BUFFER_M                     0x20
#define ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_REF_BUFFER_ENA_V                 0x20
#define ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_REF_BUFFER_DIS_V                 0x00

#define ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_M                                0x10
#define ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_ENA_V                            0x10
#define ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_DIS_V                            0x00

#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_M                 0x08
#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V             0x08
#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_DIS_V             0x00

#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_M                            0x04
#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V                        0x04
#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_DIS_V                        0x00

#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_M                0x02
#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V            0x02
#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_DIS_V            0x00

#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_M                           0x01
#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V                       0x01
#define ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_DIS_V                       0x00

#define ADIE_RX_PA_ENABLE2_HPH_MODE_SEL_M           0x01
#define ADIE_RX_PA_ENABLE2_HPH_MODE_CAPLESS_V       0x01
#define ADIE_RX_PA_ENABLE2_HPH_MODE_LEGACY_V        0x00

#define ADIE_RX_PA_ENABLE2_HPH_R_ENA_M              0x02
#define ADIE_RX_PA_ENA_REG2_HPH_R_ENA_V             0x02
#define ADIE_RX_PA_ENA_REG2_HPH_R_DIS_V             0x00

#define ADIE_RX_PA_ENABLE2_HPH_R_REF_BUF_ENA_M      0x04
#define ADIE_RX_PA_ENA_REG2_HPH_R_REF_BUF_ENA_V     0x04
#define ADIE_RX_PA_ENA_REG2_HPH_R_REF_BUF_DIS_V     0x00

/* bit 3 reserved */

#define ADIE_RX_PA_ENABLE2_HPH_L_ENA_M              0x10
#define ADIE_RX_PA_ENA_REG2_HPH_L_ENA_V             0x10
#define ADIE_RX_PA_ENA_REG2_HPH_L_DIS_V             0x00

#define ADIE_RX_PA_ENABLE2_HPH_L_REF_BUF_ENA_M      0x20
#define ADIE_RX_PA_ENA_REG2_HPH_L_REF_BUF_ENA_V     0x20
#define ADIE_RX_PA_ENA_REG2_HPH_L_REF_BUF_DIS_V     0x00

#define ADIE_RX_PA_ENABLE2_PMIC_R_ENA_M             0x40
#define ADIE_RX_PA_ENA_REG2_PMIC_R_ENA_V            0x40
#define ADIE_RX_PA_ENA_REG2_PMIC_R_DIS_V            0x00

#define ADIE_RX_PA_ENABLE2_PMIC_L_ENA_M             0x80
#define ADIE_RX_PA_ENA_REG2_PMIC_L_ENA_V            0x80
#define ADIE_RX_PA_ENA_REG2_PMIC_L_DIS_V            0x00

/* Dragon-3 variations */

#define ADIE_RX_PA_ENA_REG2_HPH_R_MODE_M            0x01
/*HPH-R Legacy mode(capacitor coupled)*/
#define ADIE_RX_PA_ENA_REG2_HPH_R_LEGACY_MODE_V     0x00
/*HPH-R Cap-less mode (ground  referenced)*/
#define ADIE_RX_PA_ENA_REG2_HPH_R_CAP_LESS_MODE_V   0x01

#define ADIE_RX_PA_ENA_REG2_HPH_L_MODE_M            0x08
/*HPH-L Legacy mode(capacitor coupled)*/
#define ADIE_RX_PA_ENA_REG2_HPH_L_LEGACY_MODE_V     0x00
/*HPH-L Cap-less mode (ground  referenced)*/
#define ADIE_RX_PA_ENA_REG2_HPH_L_CAP_LESS_MODE_V   0x08


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_CNTL_REG1 : CODEC Rx PA Control Register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_CONTROL_REGISTER1_R                                      0x99
#define ADIE_RX_PA_CONTROL_REGISTER1_INIT_M                                 0xFF
#define ADIE_RX_PA_CONTROL_REGISTER1_INIT_V                                 0x00

#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_M                      0xC0
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_NONE_V                 0x00
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_STEREO_V               0x80
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                 0xC0

#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_LINEOUT_CONNECT_M                  0x30
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_LINEOUT_CONNECT_NONE_V             0x00
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_LINEOUT_CONNECT_STEREO_V           0x20
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_LINEOUT_CONNECT_MONO_V             0x30

#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_M             0x0C
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_NONE_V        0x00
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_STEREO_V      0x08
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_MONO_V        0x0C

#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_EAR_PIECE_CONNECT_M                0x02
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_EAR_PIECE_CONNECT_NONE_V           0x00
#define ADIE_RX_PA_CONTROL1_RX_FILTER_TO_EAR_PIECE_CONNECT_V                0x02

#define ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_M                                 0x01
#define ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V                          0x00
#define ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V                         0x01

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_CNTL_REG2 : CODEC Rx PA Control Register 2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_CONTROL_REGISTER2_R                                      0x9A
#define ADIE_RX_PA_CONTROL_REGISTER2_INIT_M                                 0xFF
#define ADIE_RX_PA_CONTROL_REGISTER2_INIT_V                                 0x00

#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_M                        0xC0
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_NONE_V                   0x00
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V                 0x80
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_MONO_V                   0xC0

#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_LINEOUT_CONNECT_M                    0x30
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_LINEOUT_CONNECT_NONE_V               0x00
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_LINEOUT_CONNECT_STEREO_V             0x20
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_LINEOUT_CONNECT_MONO_V               0x30

#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_M               0x0C
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_NONE_V          0x00
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V        0x08
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_MONO_V          0x0C

#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_EAR_PIECE_CONNECT_M                  0x02
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_EAR_PIECE_CONNECT_NONE_V             0x00
#define ADIE_RX_PA_CONTROL2_AUX_PGA_TO_EAR_PIECE_CONNECT_V                  0x02

#define ADIE_RX_PA_CONTROL2_HPH_PA_RIGHT_MONO_CONFIG_M                      0x01
#define ADIE_RX_PA_CONTROL2_HPH_PA_RIGHT_MONO_CONFIG_IN_PHASE_V             0x00
#define ADIE_RX_PA_CONTROL2_HPH_PA_RIGHT_MONO_CONFIG_DIFF_V                 0x01

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_CNTL_REG3 : CODEC Rx PA Control Register 3

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_CONTROL_REGISTER3_R                                      0x9B
#define ADIE_RX_PA_CONTROL_REGISTER3_INIT_M                                 0xC0
#define ADIE_RX_PA_CONTROL_REGISTER3_INIT_V                                 0x00

#define ADIE_RX_PA_CONTROL3_EAR_PA_INPUT_TO_GROUND_M                        0x80
#define ADIE_RX_PA_CONTROL3_EAR_PA_INPUT_TO_GROUND_ENA_V                    0x80
#define ADIE_RX_PA_CONTROL3_EAR_PA_INPUT_TO_GROUND_DIS_V                    0x00

#define ADIE_RX_PA_CONTROL3_EAR_PA_INPUT_COMMON_MODE_M                      0x40
#define ADIE_RX_PA_CONTROL3_EAR_PA_INPUT_COMMON_MODE_ENA_V                  0x40
#define ADIE_RX_PA_CONTROL3_EAR_PA_INPUT_COMMON_MODE_DIS_V                  0x00

/* bit [0-5] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_CNTL_REG4 : CODEC Rx PA Control Register 4

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_CONTROL_REGISTER4_R                                                      0x9C
#define ADIE_RX_PA_CONTROL_REGISTER4_INIT_M                                                 0xFE
#define ADIE_RX_PA_CONTROL_REGISTER4_INIT_V                                                 0x2A

#define ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_M                                      0xE0
#define ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_870_UA_LEGACY_3P33_MA_CAPLESS_V        0x00
#define ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_620_UA_LEGACY_2P50_MA_CAPLESS_V        0x20
#define ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_480_UA_LEGACY_2P01_MA_CAPLESS_V        0x40
#define ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_340_UA_LEGACY_1P47_MA_CAPLESS_V        0x60
#define ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_250_UA_LEGACY_1P15_MA_CAPLESS_V        0x80
#define ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V        0xA0
#define ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_140_UA_LEGACY_0650_UA_CAPLESS_V        0xC0
#define ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_095_UA_LEGACY_0470_UA_CAPLESS_V        0xE0

#define ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_M                                  0x18
#define ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_2X_NOMINAL_V                       0x00
#define ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V                          0x08
#define ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_0P5X_NOMINAL_V                     0x10
#define ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_0P25X_NOMINAL_V                    0x18

#define ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_M                             0x06
#define ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_2X_NOMINAL_V                  0x00
#define ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V                     0x02
#define ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_0P5X_NOMINAL_V                0x04

/* bit [0] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_CNTL_REG5 : CODEC Rx PA Control Register 5

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_CONTROL_REGISTER5_R                                                      0x9D
#define ADIE_RX_PA_CONTROL_REGISTER5_INIT_M                                                 0xFF
#define ADIE_RX_PA_CONTROL_REGISTER5_INIT_V                                                 0x55

#define ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_M                                             0xC0
#define ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_12P5_UA_V                                     0x00
#define ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V                                     0x40
#define ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_07P5_UA_V                                     0x80
#define ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_05P0_UA_V                                     0xC0

#define ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_M                                  0x30
#define ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_25P0_UA_V                          0x00
#define ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_20P0_UA_V                          0x10
#define ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_15P0_UA_V                          0x20
#define ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_10P0_UA_V                          0x30

#define ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_M                                             0x0C
#define ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_12P5_UA_V                                     0x00
#define ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_10P0_UA_V                                     0x04
#define ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_07P5_UA_V                                     0x08
#define ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_05P0_UA_V                                     0x0C

#define ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_M                                         0x03
#define ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V                                 0x00
#define ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_10P0_UA_V                                 0x01
#define ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_07P5_UA_V                                 0x02
#define ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_05P0_UA_V                                 0x03

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_CNTL_REG6 : CODEC Rx PA Control Register 6

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_CONTROL_REGISTER6_R                                                      0x9E
#define ADIE_RX_PA_CONTROL_REGISTER6_INIT_M                                                 0xF0
#define ADIE_RX_PA_CONTROL_REGISTER6_INIT_V                                                 0x00

#define ADIE_RX_PA_CONTROL6_LINEOUT_LEFT_PA_POLARITY_INVERTED_M                             0x80
#define ADIE_RX_PA_CONTROL6_LINEOUT_LEFT_PA_POLARITY_INVERTED_ENA_V                         0x80
#define ADIE_RX_PA_CONTROL6_LINEOUT_LEFT_PA_POLARITY_INVERTED_DIS_V                         0x00

#define ADIE_RX_PA_CONTROL6_LINEOUT_RIGHT_PA_POLARITY_INVERTED_M                            0x40
#define ADIE_RX_PA_CONTROL6_LINEOUT_RIGHT_PA_POLARITY_INVERTED_ENA_V                        0x40
#define ADIE_RX_PA_CONTROL6_LINEOUT_RIGHT_PA_POLARITY_INVERTED_DIS_V                        0x00

#define ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_LEFT_PA_POLARITY_INVERTED_M                        0x20
#define ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_LEFT_PA_POLARITY_INVERTED_ENA_V                    0x20
#define ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_LEFT_PA_POLARITY_INVERTED_DIS_V                    0x00

#define ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_RIGHT_PA_POLARITY_INVERTED_M                       0x10
#define ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_RIGHT_PA_POLARITY_INVERTED_ENA_V                   0x10
#define ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_RIGHT_PA_POLARITY_INVERTED_DIS_V                   0x00

/* bit [0-3] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_CNTL_REG7 : CODEC Rx PA Control Register 7

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_CONTROL_REGISTER7_R                                                      0x9F
#define ADIE_RX_PA_CONTROL_REGISTER7_INIT_M                                                 0xFC
#define ADIE_RX_PA_CONTROL_REGISTER7_INIT_V                                                 0x0C

#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_M                                              0xFC
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V              0x00
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_MINUS_1P5_CAPLESS_DB_V             0x04
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_MINUS_3P0_CAPLESS_DB_V             0x08
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V      0x0C
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_06P0_LEGACY_MINUS_06P0_CAPLESS_DB_V      0x10
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_07P5_LEGACY_MINUS_07P5_CAPLESS_DB_V      0x14
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_09P0_LEGACY_MINUS_09P0_CAPLESS_DB_V      0x18
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V      0x1C
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_12P0_LEGACY_MINUS_12P0_CAPLESS_DB_V      0x20
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_13P5_LEGACY_MINUS_13P5_CAPLESS_DB_V      0x24
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_15P0_LEGACY_MINUS_15P0_CAPLESS_DB_V      0x28
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V      0x2C
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_18P0_LEGACY_MINUS_18P0_CAPLESS_DB_V      0x30
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_19P5_LEGACY_MINUS_19P5_CAPLESS_DB_V      0x34
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_21P0_LEGACY_MINUS_21P0_CAPLESS_DB_V      0x38
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V      0x3C
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_24P0_LEGACY_MINUS_24P0_CAPLESS_DB_V      0x40
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_25P5_LEGACY_MINUS_25P5_CAPLESS_DB_V      0x44
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_27P0_LEGACY_MINUS_27P0_CAPLESS_DB_V      0x48
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_28P5_LEGACY_MINUS_28P5_CAPLESS_DB_V      0x4C
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_30P0_LEGACY_MINUS_30P0_CAPLESS_DB_V      0x50
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_31P5_LEGACY_MINUS_31P5_CAPLESS_DB_V      0x54
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_33P0_LEGACY_MINUS_33P0_CAPLESS_DB_V      0x58
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V      0x5C
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_36P0_LEGACY_MINUS_36P0_CAPLESS_DB_V      0x60
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_37P5_LEGACY_MINUS_37P5_CAPLESS_DB_V      0x64
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_39P0_LEGACY_MINUS_39P0_CAPLESS_DB_V      0x68
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_40P5_LEGACY_MINUS_40P5_CAPLESS_DB_V      0x6C
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_42P0_LEGACY_MINUS_42P0_CAPLESS_DB_V      0x70
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_43P5_LEGACY_MINUS_43P5_CAPLESS_DB_V      0x74
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_45P0_LEGACY_MINUS_45P0_CAPLESS_DB_V      0x78
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_46P5_LEGACY_MINUS_46P5_CAPLESS_DB_V      0x7C
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_48P0_LEGACY_MINUS_48P0_CAPLESS_DB_V      0x80
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_49P5_LEGACY_MINUS_49P5_CAPLESS_DB_V      0x84
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_51P0_LEGACY_MINUS_51P0_CAPLESS_DB_V      0x88
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_52P5_LEGACY_MINUS_52P5_CAPLESS_DB_V      0x8C
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_54P0_LEGACY_MINUS_54P0_CAPLESS_DB_V      0x90
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_55P5_LEGACY_MINUS_55P5_CAPLESS_DB_V      0x94
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_57P0_LEGACY_MINUS_57P0_CAPLESS_DB_V      0x98
#define ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V      0x9C

/* bit [0-1] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_CNTL_REG8 : CODEC Rx PA Control Register 8

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_CONTROL_REGISTER8_R                                                      0xA0
#define ADIE_RX_PA_CONTROL_REGISTER8_INIT_M                                                 0xFC
#define ADIE_RX_PA_CONTROL_REGISTER8_INIT_V                                                 0x0C

#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_M                                             0xFC
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V             0x00
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_MINUS_1P5_CAPLESS_DB_V            0x04
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_MINUS_3P0_CAPLESS_DB_V            0x08
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V     0x0C
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_06P0_LEGACY_MINUS_06P0_CAPLESS_DB_V     0x10
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_07P5_LEGACY_MINUS_07P5_CAPLESS_DB_V     0x14
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_09P0_LEGACY_MINUS_09P0_CAPLESS_DB_V     0x18
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V     0x1C
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_12P0_LEGACY_MINUS_12P0_CAPLESS_DB_V     0x20
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_13P5_LEGACY_MINUS_13P5_CAPLESS_DB_V     0x24
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_15P0_LEGACY_MINUS_15P0_CAPLESS_DB_V     0x28
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V     0x2C
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_18P0_LEGACY_MINUS_18P0_CAPLESS_DB_V     0x30
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_19P5_LEGACY_MINUS_19P5_CAPLESS_DB_V     0x34
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_21P0_LEGACY_MINUS_21P0_CAPLESS_DB_V     0x38
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V     0x3C
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_24P0_LEGACY_MINUS_24P0_CAPLESS_DB_V     0x40
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_25P5_LEGACY_MINUS_25P5_CAPLESS_DB_V     0x44
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_27P0_LEGACY_MINUS_27P0_CAPLESS_DB_V     0x48
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_28P5_LEGACY_MINUS_28P5_CAPLESS_DB_V     0x4C
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_30P0_LEGACY_MINUS_30P0_CAPLESS_DB_V     0x50
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_31P5_LEGACY_MINUS_31P5_CAPLESS_DB_V     0x54
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_33P0_LEGACY_MINUS_33P0_CAPLESS_DB_V     0x58
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V     0x5C
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_36P0_LEGACY_MINUS_36P0_CAPLESS_DB_V     0x60
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_37P5_LEGACY_MINUS_37P5_CAPLESS_DB_V     0x64
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_39P0_LEGACY_MINUS_39P0_CAPLESS_DB_V     0x68
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_40P5_LEGACY_MINUS_40P5_CAPLESS_DB_V     0x6C
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_42P0_LEGACY_MINUS_42P0_CAPLESS_DB_V     0x70
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_43P5_LEGACY_MINUS_43P5_CAPLESS_DB_V     0x74
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_45P0_LEGACY_MINUS_45P0_CAPLESS_DB_V     0x78
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_46P5_LEGACY_MINUS_46P5_CAPLESS_DB_V     0x7C
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_48P0_LEGACY_MINUS_48P0_CAPLESS_DB_V     0x80
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_49P5_LEGACY_MINUS_49P5_CAPLESS_DB_V     0x84
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_51P0_LEGACY_MINUS_51P0_CAPLESS_DB_V     0x88
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_52P5_LEGACY_MINUS_52P5_CAPLESS_DB_V     0x8C
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_54P0_LEGACY_MINUS_54P0_CAPLESS_DB_V     0x90
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_55P5_LEGACY_MINUS_55P5_CAPLESS_DB_V     0x94
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_57P0_LEGACY_MINUS_57P0_CAPLESS_DB_V     0x98
#define ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V     0x9C

/* bit [0-1] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

GPR_MS1 : GPR MS1 Register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_GPR_MS1_REGISTER_R                               0x80
#define ADIE_GPR_MS1_REGISTER_INIT_M                          0xFF
#define ADIE_GPR_MS1_REGISTER_INIT_V                          0x40

#define ADIE_GP1_MS1_REG_CDC_DO_CONTINUITY_TEST_M             0x01
#define ADIE_GP1_MS1_REG_CDC_DO_CONTINUITY_TEST_ENA_V         0x01
#define ADIE_GP1_MS1_REG_CDC_DO_CONTINUITY_TEST_DIS_V         0x00

#define ADIE_GP1_MS1_REG_CDC_DO_FORCE_M                       0x02
#define ADIE_GP1_MS1_REG_CDC_DO_FORCE_ENA_V                   0x02
#define ADIE_GP1_MS1_REG_CDC_DO_FORCE_DIS_V                   0x00

#define ADIE_GP1_MS1_REG_CDC_DO_DRIVE_M                       0x04
#define ADIE_GP1_MS1_REG_CDC_DO_5MA_DRIVE_ENA_V               0x04
#define ADIE_GP1_MS1_REG_CDC_DO_1MA_DRIVE_ENA_V               0x00

#define ADIE_GP1_MS1_REG_CDC_CK_ENB_M                         0x08
#define ADIE_GP1_MS1_REG_CDC_CK_ENB_ENA_V                     0x00
#define ADIE_GP1_MS1_REG_CDC_CK_ENB_DIS_V                     0x08

#define ADIE_GP1_MS1_REG_CDC_DIL_ENB_M                        0x10
#define ADIE_GP1_MS1_REG_CDC_DIL_ENB_ENA_V                    0x00
#define ADIE_GP1_MS1_REG_CDC_DIL_ENB_DIS_V                    0x10

#define ADIE_GP1_MS1_REG_CDC_DIR_ENB_M                        0x20
#define ADIE_GP1_MS1_REG_CDC_DIR_ENB_ENA_V                    0x00
#define ADIE_GP1_MS1_REG_CDC_DIR_ENB_DIS_V                    0x20

#define ADIE_GP1_MS1_REG_XOADC_CK_EN_M                        0x40
#define ADIE_GP1_MS1_REG_XOADC_CK_EN_ENA_V                    0x40
#define ADIE_GP1_MS1_REG_XOADC_CK_EN_DIS_V                    0x40

#define ADIE_GP1_MS1_REG_RCMEAS_CK_EN_M                       0x80
#define ADIE_GP1_MS1_REG_RCMEAS_CK_EN_ENA_V                   0x80
#define ADIE_GP1_MS1_REG_RCMEAS_CK_EN_DIS_V                   0x00

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

GPR_MS2 : GPR MS2 Register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_GPR_MS2_REGISTER_R                   0x81
#define ADIE_GPR_MS2_REGISTER_INIT_M              0xFF
#define ADIE_GPR_MS2_REGISTER_INIT_V              0x00

#define ADIE_GP2_MS2_CODEC_EN_M                   0x01
#define ADIE_GP2_MS2_CODEC_EN_ENA_V               0x01
#define ADIE_GP2_MS2_CODEC_EN_DIS_V               0x00

#define ADIE_GP2_MS2_ATEST_CDCTX_EN_M             0x02
#define ADIE_GP2_MS2_ATEST_CDCTX_EN_ENA_V         0x02
#define ADIE_GP2_MS2_ATEST_CDCTX_EN_DIS_V         0x00

#define ADIE_GP2_MS2_ATEST_CDCRX_EN_M             0x04
#define ADIE_GP2_MS2_ATEST_CDCRX_EN_ENA_V         0x04
#define ADIE_GP2_MS2_ATEST_CDCRX_EN_DIS_V         0x00

#define ADIE_GP2_MS2_ATEST_HKADC_EN_M             0x08
#define ADIE_GP2_MS2_ATEST_HKADC_EN_ENA_V         0x08
#define ADIE_GP2_MS2_ATEST_HKADC_EN_DIS_V         0x00

#define ADIE_GP2_MS2_ATEST_XOADC_EN_M             0x10
#define ADIE_GP2_MS2_ATEST_XOADC_EN_ENA_V         0x10
#define ADIE_GP2_MS2_ATEST_XOADC_EN_DIS_V         0x00

#define ADIE_GP2_MS2_ATEST_PRXADC_EN_M            0x20
#define ADIE_GP2_MS2_ATEST_PRXADC_EN_ENA_V        0x20
#define ADIE_GP2_MS2_ATEST_PRXADC_EN_DIS_V        0x00

#define ADIE_GP2_MS2_ATEST_DRXADC_EN_M            0x40
#define ADIE_GP2_MS2_ATEST_DRXADC_EN_ENA_V        0x40
#define ADIE_GP2_MS2_ATEST_DRXADC_EN_DIS_V        0x00

#define ADIE_GP2_MS2_ATEST_TXSENSEADC_EN_M        0x80
#define ADIE_GP2_MS2_ATEST_TXSENSEADC_EN_ENA_V    0x80
#define ADIE_GP2_MS2_ATEST_TXSENSEADC_EN_DIS_V    0x00

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

GPR_MS3 : GPR MS3 Register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_GPR_MS3_REGISTER_R                   0x82
#define ADIE_GPR_MS3_REGISTER_INIT_M              0x07
#define ADIE_GPR_MS3_REGISTER_INIT_V              0x00

#define ADIE_GPR_MS3_AMUXIN_TO_HKAIN0_M           0x01
#define ADIE_GPR_MS3_AMUXIN_TO_HKAIN0_ENA_V       0x01
#define ADIE_GPR_MS3_AMUXIN_TO_HKAIN0_DIS_V       0x00

#define ADIE_GPR_MS3_ATEST1_TO_HKAIN0_M           0x02
#define ADIE_GPR_MS3_ATEST1_TO_HKAIN0_ENA_V       0x02
#define ADIE_GPR_MS3_ATEST1_TO_HKAIN0_DIS_V       0x00

#define ADIE_GPR_MS3_ATEST2_TO_HKAIN1_M           0x04
#define ADIE_GPR_MS3_ATEST2_TO_HKAIN1_ENA_V       0x04
#define ADIE_GPR_MS3_ATEST2_TO_HKAIN1_DIS_V       0x00

/* bits [3-7] are reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ATEST Register : ATEST Register for CODEC Tx & Aux PGA

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_ATEST_TX_REGISTER_R                                 0x55
#define ADIE_ATEST_TX_REGISTER_INIT_M                            0x7F
#define ADIE_ATEST_TX_REGISTER_INIT_V                            0x00

#define ADIE_ATEST_TX_TXADC_SEL_M                                0x0F
#define ADIE_ATEST_TX_TXADC_NONE_V                               0x00
#define ADIE_ATEST_TX_TXADC_ATEST1_VOCM_V                        0x01
#define ADIE_ATEST_TX_TXADC_ATEST1_VICM_OTA_V                    0x02
#define ADIE_ATEST_TX_TXADC_ATEST1_VICM_OTA_BUFF_V               0x03
#define ADIE_ATEST_TX_TXADC_ATEST1_VICM_DAC_V                    0x04
#define ADIE_ATEST_TX_TXADC_ATEST1_VICM_INP_V                    0x05
#define ADIE_ATEST_TX_TXADC_ATEST1_VREFP_V                       0x06
#define ADIE_ATEST_TX_TXADC_ATEST1_ATEST_IOTA1_V                 0x07
#define ADIE_ATEST_TX_TXADC_ATEST1_ATEST_IOTA2_V                 0x08
#define ADIE_ATEST_TX_TXADC_ATEST1_ATEST_ICMP_V                  0x09
#define ADIE_ATEST_TX_TXADC_ATEST1_ATEST_ICMBUFFER_V             0x0A
#define ADIE_ATEST_TX_TXADC_ATEST1_ATEST_IVICMBUFFER_V           0x0B
#define ADIE_ATEST_TX_TXADC_ATEST2_VREFP_CMP_V                   0x0C
#define ADIE_ATEST_TX_TXADC_ATEST2_VCM_CMP_V                     0x0D
#define ADIE_ATEST_TX_TXADC_ATEST2_VREFN_CMP_V                   0x0E
#define ADIE_ATEST_TX_TXADC_ATEST2_VSSA_V                        0x0F

#define ADIE_ATEST_TX_TXFE_AUXPGA_SEL_M                          0x70
#define ADIE_ATEST_TX_TXFE_AUXPGA_NONE_V                         0x00
#define ADIE_ATEST_TX_TXFE_AUXPGA_TXADC_SEL_V                    0x10
#define ADIE_ATEST_TX_TXFE_AUXPGA_ATEST12_AUXPGA_L_V             0x20
#define ADIE_ATEST_TX_TXFE_AUXPGA_ATEST12_AUXPGA_R_V             0x30
#define ADIE_ATEST_TX_TXFE_AUXPGA_ATEST12_VICM_VCM_CMFB_V        0x40
#define ADIE_ATEST_TX_TXFE_AUXPGA_ATEST12_MICAMP_OUT_V           0x50
#define ADIE_ATEST_TX_TXFE_AUXPGA_ATEST1_MICAMP_VCM_OUT_V        0x60

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

DDO_MS : DDO_MS Register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_DDO_MS_REGISTER_R                              0xA7
#define ADIE_DDO_MS_REGISTER_INIT_M                         0x00
#define ADIE_DDO_MS_REGISTER_INIT_V                         0x00

/* bits [0-7] are reserved */


#define ADIE_DDO_MS_TST_CDC_DO_M                  0x01
#define ADIE_DDO_MS_TST_CDC_DO_HIGH_V             0x01
#define ADIE_DDO_MS_TST_CDC_DO_LOW_V              0x00

/* bits [1-7] are reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

DDI_MS : DDI_MS Register (read only register)

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_DDI_MS_REGISTER_R                    0x84
#define ADIE_DDI_MS_REGISTER_M                    0x7F

#define ADIE_DDI_MS_CODEC_CK_M                    0x01
#define ADIE_DDI_MS_CODEC_DLYCK_M                 0x02
#define ADIE_DDI_MS_CODEC_DIL01_M                 0x04
#define ADIE_DDI_MS_CODEC_DIL23_M                 0x08
#define ADIE_DDI_MS_CODEC_DIR01_M                 0x10
#define ADIE_DDI_MS_CODEC_DIR23_M                 0x20
#define ADIE_DDI_MS_SLEEPB_M                      0x40

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ATEST_CFG_REG : ATEST Config Register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_ATEST_CONFIG_REGISTER_R                        0xB7
#define ADIE_ATEST_CONFIG_REGISTER_INIT_M                   0x0F
#define ADIE_ATEST_CONFIG_REGISTER_INIT_V                   0x00

#define ADIE_ATEST_CFG_REG_RCM_HKADC_ATEST_M                0x08
#define ADIE_ATEST_CFG_REG_RCM_HKADC_ATEST_ENA_V            0x08
#define ADIE_ATEST_CFG_REG_RCM_HKADC_ATEST_DIS_V            0x00

#define ADIE_ATEST_CFG_REG_CODEC_PA_ATEST_M                 0x04
#define ADIE_ATEST_CFG_REG_CODEC_PA_ATEST_ENA_V             0x04
#define ADIE_ATEST_CFG_REG_CODEC_PA_ATEST_DIS_V             0x00

#define ADIE_ATEST_CFG_REG_CODEC_RX_PATH_ATEST_M            0x02
#define ADIE_ATEST_CFG_REG_CODEC_RX_PATH_ATEST_ENA_V        0x02
#define ADIE_ATEST_CFG_REG_CODEC_RX_PATH_ATEST_DIS_V        0x00

#define ADIE_ATEST_CFG_REG_CODEC_TX_PATH_ATEST_M            0x01
#define ADIE_ATEST_CFG_REG_CODEC_TX_PATH_ATEST_ENA_V        0x01
#define ADIE_ATEST_CFG_REG_CODEC_TX_PATH_ATEST_DIS_V        0x00

/* bit [4-7] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TX_PATH_ATEST_CNTL_REG : CODEC TX Path ATEST Control Register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_ATEST_TX_PATH_CONTROL_REGISTER_R                                               0xB8
#define ADIE_ATEST_TX_PATH_CONTROL_REGISTER_INIT_M                                          0xFC
#define ADIE_ATEST_TX_PATH_CONTROL_REGISTER_INIT_V                                          0x00

#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_FE_OUTPUT_TO_ATEST_BUS_M                             0x80
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_FE_OUTPUT_TO_ATEST_BUS_ENA_V                         0x80
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_FE_OUTPUT_TO_ATEST_BUS_DIS_V                         0x00

#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_FE_COMMON_MODE_VOLT_TO_ATEST_BUS_M                   0x40
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_FE_COMMON_MODE_VOLT_TO_ATEST_BUS_ENA_V               0x40
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_FE_COMMON_MODE_VOLT_TO_ATEST_BUS_DIS_V               0x00

#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_ADC_ATEST_BUS_SEL_M                                  0x38
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_ADC_ATEST_BUS_SEL_LCMP1_ATEST1_LDACREF_ATEST2_V      0x08
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_ADC_ATEST_BUS_SEL_LOTA2_ATEST1_LB_10U_ATEST2_V       0x10
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_ADC_ATEST_BUS_SEL_LOTA1_ATEST1_VREFMID_ATEST2_V      0x18
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_ADC_ATEST_BUS_SEL_VICM_ATEST1_VOCM_ATEST2_V          0x20
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_ADC_ATEST_BUS_SEL_VTH_P_ATEST1_VTH_N_ATEST2_V        0x28
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_ADC_ATEST_BUS_SEL_VREF_P_ATEST1_VREF_N_ATEST2_V      0x30
#define ADIE_ATEST_TX_PATH_CNTL_REG_TX_ADC_ATEST_BUS_SEL_DIS_V                              0x00

#define ADIE_ATEST_TX_PATH_CNTL_REG_MIC_USB_ATEST_BUS_SEL_M                                 0x04
#define ADIE_ATEST_TX_PATH_CNTL_REG_MIC_USB_ATEST_BUS_SEL_PLUS_ATEST1_MINUS_ATEST2_V        0x04
#define ADIE_ATEST_TX_PATH_CNTL_REG_MIC_USB_ATEST_BUS_SEL_DIS_V                             0x00

/* bit [0-1] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PATH_ATEST_CNTL_REG : CODEC RX Path ATEST Control Register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_ATEST_RX_PATH_CONTROL_REGISTER_R                                               0xB9
#define ADIE_ATEST_RX_PATH_CONTROL_REGISTER_INIT_M                                          0xFF
#define ADIE_ATEST_RX_PATH_CONTROL_REGISTER_INIT_V                                          0x00

#define ADIE_ATEST_RX_PATH_CNTL_REG_DWA_INPUT_TO_ATEST_BUS_M                                        0x80
#define ADIE_ATEST_RX_PATH_CNTL_REG_DWA_INPUT_TO_ATEST_BUS_DIS_V                                    0x00
#define ADIE_ATEST_RX_PATH_CNTL_REG_DWA_INPUT_TO_ATEST_BUS_LEFT_CHAN_ATEST1_RIGHT_CHAN_ATEST2_V     0x80

#define ADIE_ATEST_RX_PATH_CNTL_REG_DWA_OUTPUT_TO_ATEST_BUS_M                                       0x40
#define ADIE_ATEST_RX_PATH_CNTL_REG_DWA_OUTPUT_TO_ATEST_BUS_DIS_V                                   0x00
#define ADIE_ATEST_RX_PATH_CNTL_REG_DWA_OUTPUT_TO_ATEST_BUS_LEFT_CHAN_ATEST1_RIGHT_CHAN_ATEST2_V    0x40

#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_LEFT_CHAN_FILTER_TO_ATEST_BUS_M                              0x20
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_LEFT_CHAN_FILTER_TO_ATEST_BUS_DIS_V                          0x00
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_LEFT_CHAN_FILTER_TO_ATEST_BUS_POS_ATEST1_NEG_ATEST2_V        0x20

#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_RIGHT_CHAN_FILTER_TO_ATEST_BUS_M                             0x10
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_RIGHT_CHAN_FILTER_TO_ATEST_BUS_DIS_V                         0x00
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_RIGHT_CHAN_FILTER_TO_ATEST_BUS_POS_ATEST1_NEG_ATEST2_V       0x10

#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_LEFT_CHAN_FILTER_DC_BIAS_TO_ATEST_BUS_M                      0x0C
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_LEFT_CHAN_FILTER_DC_BIAS_TO_ATEST_BUS_DIS_V                  0x00
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_LEFT_CHAN_FILTER_DC_BIAS_TO_ATEST_BUS_LPTAT_80U_ATEST1_LBG_60U_ATEST2_V  0x04
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_LEFT_CHAN_FILTER_DC_BIAS_TO_ATEST_BUS_VOCM_ATEST1_VCMFB_ATEST2_V         0x08
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_LEFT_CHAN_FILTER_DC_BIAS_TO_ATEST_BUS_VDACH_ATEST1_VICM_ATEST2_V         0x0C

#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_RIGHT_CHAN_FILTER_DC_BIAS_TO_ATEST_BUS_M                     0x03
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_RIGHT_CHAN_FILTER_DC_BIAS_TO_ATEST_BUS_DIS_V                 0x00
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_RIGHT_CHAN_FILTER_DC_BIAS_TO_ATEST_BUS_LPTAT_80U_ATEST1_LBG_60U_ATEST2_V 0x01
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_RIGHT_CHAN_FILTER_DC_BIAS_TO_ATEST_BUS_VOCM_ATEST1_VCMFB_ATEST2_V        0x02
#define ADIE_ATEST_RX_PATH_CNTL_REG_RX_RIGHT_CHAN_FILTER_DC_BIAS_TO_ATEST_BUS_VDACH_ATEST1_VICM_ATEST2_V        0x03

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_AUXPGA_PA_ATEST_CNTL_REG : CODEC AuxPGA/PA ATEST Control Register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_ATEST_AUXPGA_PA_CONTROL_REGISTER_R                                                     0xBA
#define ADIE_ATEST_AUXPGA_PA_CONTROL_REGISTER_INIT_M                                                0xFC
#define ADIE_ATEST_AUXPGA_PA_CONTROL_REGISTER_INIT_V                                                0x00

#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_M                                             0xF0
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_DIS_V                                         0x00
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_LINEOUT_COMMON_MODE_REF_ATEST1_V              0x10
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_PMIC_LINEOUT_COMMON_MODE_REF_ATEST1_V         0x20
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_PMIC_LINEOUT_LEFT_ATEST1_RIGHT_ATEST2_V       0x30
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_HPH_PA_LEFT_COMMON_MODE_REF_ATEST1_V          0x50
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_HPH_PA_RIGHT_COMMON_MODE_REF_ATEST1_V         0x60
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_EAR_PA_FSVN_ATEST1_VSS2P2_ATEST2_V            0x70
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_EAR_PA_NCASC_ATEST1_VICM_ATEST2_V             0x80
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_EAR_PA_NMIRR_ATEST1_FSVP_ATEST2_V             0x90
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_PA_TO_ATEST_BUS_EAR_PA_PMIRR_ATEST1_PTAIL1_ATEST2_V           0xA0

#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_AUXPGA_TO_ATEST_BUS_M                                         0x0C
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_AUXPGA_TO_ATEST_BUS_DIS_V                                     0x00
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_AUXPGA_TO_ATEST_BUS_PLEFT_CHAN_ATEST1_NLEFT_CHAN_ATEST2_V     0x04
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_AUXPGA_TO_ATEST_BUS_PRIGHT_CHAN_ATEST1_NRIGHT_CHAN_ATEST2_V   0x08
#define ADIE_ATEST_AUXPGA_PA_CNTL_REG_AUXPGA_TO_ATEST_BUS_COMMON_MODE_BUFR_ATEST1_IDLE_ATEST2_V     0x0C

/* bit [0-1] reserved */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

RCM_HKADC_ATEST_CNTL_REG : RCM/HKADC ATEST Control Register

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_ATEST_RCM_HKADC_CONTROL_REGISTER_R                                                 0xBB
#define ADIE_ATEST_RCM_HKADC_CONTROL_REGISTER_INIT_M                                            0xFF
#define ADIE_ATEST_RCM_HKADC_CONTROL_REGISTER_INIT_V                                            0x00

#define ADIE_ATEST_RCM_HKADC_CNTL_REG_RCM_TO_GPIO_M                                             0x80
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_RCM_TO_GPIO_DIS_V                                         0x00
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_RCM_TO_GPIO_RFA_TEST_HKADC_BBRX_CK_V                      0x80

#define ADIE_ATEST_RCM_HKADC_CNTL_REG_RCM_GPIO_OUTPUT_SEL_M                                     0x60
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_RCM_GPIO_OUTPUT_SEL_DIS_V                                 0x00
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_RCM_GPIO_OUTPUT_SEL_HALT_SIG_V                            0x20
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_RCM_GPIO_OUTPUT_SEL_RC_CLK_V                              0x40
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_RCM_GPIO_OUTPUT_SEL_BUSY_SIG_V                            0x60

#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HKADC1_TO_ATEST_BUS_M                                     0x18
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HKADC1_TO_ATEST_BUS_DIS_V                                 0x00
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HKADC1_TO_ATEST_BUS_COMP_OUT_ATEST1_EXT_TRIGR_ATEST2_V    0x08
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HKADC1_TO_ATEST_BUS_DAC_NREF_ATEST1_DAC_PREF_ATEST2_V     0x10
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HKADC1_TO_ATEST_BUS_VICM_ATEST1_VBIASP_ATEST2_V           0x18

#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HKADC2_TO_ATEST_BUS_M                                     0x06
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HKADC2_TO_ATEST_BUS_DIS_V                                 0x00
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HKADC2_TO_ATEST_BUS_LATCH_CLK_ATEST1_DELAYED_SAMPLE_CLK_ATEST2_V  0x02
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HKADC2_TO_ATEST_BUS_DACSETS_BIT11_ATEST1_AIN_SEL_BIT0_ATEST2_V    0x04
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HKADC2_TO_ATEST_BUS_REF_SEL_BIT1_ATEST1_EN_B_ATEST2_V             0x06

#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HK_AMUXIN_TO_ATEST_BUS_M                                  0x01
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HK_AMUXIN_TO_ATEST_BUS_DIS_V                              0x00
#define ADIE_ATEST_RCM_HKADC_CNTL_REG_HK_AMUXIN_TO_ATEST_BUS_ATEST1_V                           0x01

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_MUX_REG1 : CODEC Rx PA Mux Register1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_MUX_REGISTER1_R                                        0x91
#define ADIE_RX_PA_MUX_REGISTER1_INIT_M                                   0xFE
#define ADIE_RX_PA_MUX_REGISTER1_INIT_V                                   0x00


#define ADIE_RX_PA_MUX1_BQD_SEL_EAR_M                                     0x02
#define ADIE_RX_PA_MUX_REG1_BQD_SEL_EAR_CONNECT_V                         0x02
#define ADIE_RX_PA_MUX_REG1_BQD_SEL_EAR_DISCONNECT_V                      0x00

#define ADIE_RX_PA_MUX1_BQD_SEL_LOPMIC_M                                  0x0C
/*connect Rx Biquad to lineout PMIC in Stereo mode*/
#define ADIE_RX_PA_MUX_REG1_BQD_SEL_LOPMIC_CONNECT_V                      0x08
/*connect Rx Biquad LEFT to lineout PMIC in Mono-differntial mode*/
#define ADIE_RX_PA_MUX_REG1_BQD_LOPMIC_MONO_EN_CONNECT_V                  0x0C
/*completely disconnect Rxbiquad from lineout PMIC */
#define ADIE_RX_PA_MUX_REG1_BQD_LOPMIC_DISCONNECT_V                       0x00

#define ADIE_RX_PA_MUX1_BQD_SEL_LOEXT_M                                   0x30
/*connect Rx Biquad to lineout External in Stereo mode*/
#define ADIE_RX_PA_MUX_REG1_BQD_SEL_LOEXT_CONNECT_V                       0x20
/*connect Rx Biquad LEFT to lineout External in Mono-differntial mode*/
#define ADIE_RX_PA_MUX_REG1_BQD_LOEXT_MONO_EN_CONNECT_V                   0x30
/*completely disconnect Rxbiquad from lineout External */
#define ADIE_RX_PA_MUX_REG1_BQD_LOEXT_DISCONNECT_V                        0x00

#define ADIE_RX_PA_MUX1_BQD_SEL_HPH_M                                     0xC0
/*connect Rx Biquad to HPH in Stereo mode*/
#define ADIE_RX_PA_MUX_REG1_BQD_SEL_HPH_CONNECT_V                         0x80
/*connect Rx Biquad LEFT to HPH in Mono-differntial mode*/
#define ADIE_RX_PA_MUX_REG1_BQD_HPH_MONO_EN_CONNECT_V                     0xC0
/*completely disconnect Rxbiquad from HPH */
#define ADIE_RX_PA_MUX_REG1_BQD_HPH_DISCONNECT_V                          0x00

/*bit 0 reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_MUX_REG2 : CODEC Rx PA Mux Register2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_MUX_REGISTER2_R                                    0x92
#define ADIE_RX_PA_MUX_REGISTER2_INIT_M                               0xFE
#define ADIE_RX_PA_MUX_REGISTER2_INIT_V                               0x00

#define ADIE_RX_PA_MUX2_AUX_SEL_EAR_M                                 0x02
#define ADIE_RX_PA_MUX_REG2_AUX_SEL_EAR_CONNECT_V                     0x02
#define ADIE_RX_PA_MUX_REG2_AUX_SEL_EAR_DISCONNECT_V                  0x00

#define ADIE_RX_PA_MUX2_AUX_SEL_LOPMIC_M                              0x0C
/*connect AUX PGA left to lineout PMIC in stereo mode*/
#define ADIE_RX_PA_MUX_REG2_AUX_SEL_LOPMIC_CONNECT_V                  0x08
/*connect AUX PGA left to lineout PMIC in mono-differential mode*/
#define ADIE_RX_PA_MUX_REG2_AUX_SEL_LOPMIC_MONO_EN_CONNECT_V          0x0C
/*completely disconnect AuxPGA from lineout PMIC */
#define ADIE_RX_PA_MUX_REG2_AUX_LOPMIC_DISCONNECT_V                   0x00

#define ADIE_RX_PA_MUX2_AUX_SEL_LOEXT_M                               0x30
/*connect AUX PGA left to lineout external in stereo mode*/
#define ADIE_RX_PA_MUX_REG2_AUX_SEL_LOEXT_CONNECT_V                   0x20
/*connect AUX PGA left to lineout external in mono-differential mode*/
#define ADIE_RX_PA_MUX_REG2_AUX_SEL_LOEXT_MONO_EN_CONNECT_V           0x30
/*completely disconnect AuxPGA from lineout external */
#define ADIE_RX_PA_MUX_REG2_AUX_LOEXT_DISCONNECT_V                    0x00

#define ADIE_RX_PA_MUX2_AUX_SEL_HPH_M                                 0xC0
/*connect AUX PGA left to HPH in stereo mode*/
#define ADIE_RX_PA_MUX_REG2_AUX_SEL_HPH_CONNECT_V                     0x80
/*connect AUX PGA left to HPH in mono-differential mode*/
#define ADIE_RX_PA_MUX_REG2_AUX_SEL_HPH_MONO_EN_CONNECT_V             0xC0
/*completely disconnect AuxPGA from HPH*/
#define ADIE_RX_PA_MUX_REG2_AUX_HPH_DISCONNECT_V                      0x00

/*bit 0 reserved */

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_BIAS_REG1 : CODEC Rx PA Bias Register1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_BIAS_REGISTER1_R               0x93
#define ADIE_RX_PA_BIAS_REGISTER1_INIT_M          0xFF
#define ADIE_RX_PA_BIAS_REGISTER1_INIT_V          0x00

#define ADIE_RX_PA_BIAS1_LOEXT_R_M                0x03
#define ADIE_RX_PA_BIAS_LOEXT_R_20_UA_V           0x00
#define ADIE_RX_PA_BIAS_LOEXT_R_10_UA_V           0x01
#define ADIE_RX_PA_BIAS_LOEXT_R_15_UA_V           0x02
#define ADIE_RX_PA_BIAS_LOEXT_R_25_UA_V           0x03

#define ADIE_RX_PA_BIAS1_LOEXT_L_M                0x0C
#define ADIE_RX_PA_BIAS_LOEXT_L_20_UA_V           0x00
#define ADIE_RX_PA_BIAS_LOEXT_L_10_UA_V           0x04
#define ADIE_RX_PA_BIAS_LOEXT_L_15_UA_V           0x08
#define ADIE_RX_PA_BIAS_LOEXT_L_25_UA_V           0x0C

#define ADIE_RX_PA_BIAS1_EAR_REF_BUF_M            0x30
#define ADIE_RX_PA_BIAS_EAR_REF_BUF_20_UA_V       0x00
#define ADIE_RX_PA_BIAS_EAR_REF_BUF_10_UA_V       0x10
#define ADIE_RX_PA_BIAS_EAR_REF_BUF_15_UA_V       0x20
#define ADIE_RX_PA_BIAS_EAR_REF_BUF_25_UA_V       0x30

#define ADIE_RX_PA_BIAS1_EAR_M                    0xC0
#define ADIE_RX_PA_BIAS_EAR_20_UA_V               0x00
#define ADIE_RX_PA_BIAS_EAR_10_UA_V               0x40
#define ADIE_RX_PA_BIAS_EAR_15_UA_V               0x80
#define ADIE_RX_PA_BIAS_EAR_25_UA_V               0xC0

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_BIAS_REG2 : CODEC Rx PA Bias Register2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_BIAS_REGISTER2_R                    0x94
#define ADIE_RX_PA_BIAS_REGISTER2_INIT_M               0xFF
#define ADIE_RX_PA_BIAS_REGISTER2_INIT_V               0x00

#define ADIE_RX_PA_BIAS2_LOPMIC_REF_BUFF_M             0x03
#define ADIE_RX_PA_BIAS_REG2_LOPMIC_REF_BUFF_20_UA_V   0x00
#define ADIE_RX_PA_BIAS_REG2_LOPMIC_REF_BUFF_10_UA_V   0x01
#define ADIE_RX_PA_BIAS_REG2_LOPMIC_REF_BUFF_15_UA_V   0x02
#define ADIE_RX_PA_BIAS_REG2_LOPMIC_REF_BUFF_25_UA_V   0x03

#define ADIE_RX_PA_BIAS2_LOEXT_REF_BUFF_M              0x0C
#define ADIE_RX_PA_BIAS_REG2_LOEXT_REF_BUFF_20_UA_V    0x00
#define ADIE_RX_PA_BIAS_REG2_LOEXT_REF_BUFF_10_UA_V    0x04
#define ADIE_RX_PA_BIAS_REG2_LOEXT_REF_BUFF_15_UA_V    0x08
#define ADIE_RX_PA_BIAS_REG2_LOEXT_REF_BUFF_25_UA_V    0x0C

#define ADIE_RX_PA_BIAS2_PMIC_R_M                      0x30
#define ADIE_RX_PA_BIAS_REG2_PMIC_R_20_UA_V            0x00
#define ADIE_RX_PA_BIAS_REG2_PMIC_R_10_UA_V            0x10
#define ADIE_RX_PA_BIAS_REG2_PMIC_R_15_UA_V            0x20
#define ADIE_RX_PA_BIAS_REG2_PMIC_R_25_UA_V            0x30

#define ADIE_RX_PA_BIAS2_PMIC_L_M                      0xC0
#define ADIE_RX_PA_BIAS_REG2_PMIC_L_20_UA_V            0x00
#define ADIE_RX_PA_BIAS_REG2_PMIC_L_10_UA_V            0x40
#define ADIE_RX_PA_BIAS_REG2_PMIC_L_15_UA_V            0x80
#define ADIE_RX_PA_BIAS_REG2_PMIC_L_25_UA_V            0xC0

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_BIAS_REG3 : CODEC Rx PA Bias Register3

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_BIAS_REGISTER3_R                   0x95
#define ADIE_RX_PA_BIAS_REGISTER3_INIT_M              0xFF
#define ADIE_RX_PA_BIAS_REGISTER3_INIT_V              0x00

#define ADIE_RX_PA_BIAS3_HPH_L_REF_BUF2_M             0x03
#define ADIE_RX_PA_BIAS_REG3_HPH_L_REF_BUF2_20_UA_V   0x00
#define ADIE_RX_PA_BIAS_REG3_HPH_L_REF_BUF2_10_UA_V   0x01
#define ADIE_RX_PA_BIAS_REG3_HPH_L_REF_BUF2_15_UA_V   0x02
#define ADIE_RX_PA_BIAS_REG3_HPH_L_REF_BUF2_25_UA_V   0x03

#define ADIE_RX_PA_BIAS3_HPH_L_REF_BUF1_M             0x0C
#define ADIE_RX_PA_BIAS_REG3_HPH_L_REF_BUF1_20_UA_V   0x00
#define ADIE_RX_PA_BIAS_REG3_HPH_L_REF_BUF1_10_UA_V   0x04
#define ADIE_RX_PA_BIAS_REG3_HPH_L_REF_BUF1_15_UA_V   0x08
#define ADIE_RX_PA_BIAS_REG3_HPH_L_REF_BUF1_25_UA_V   0x0C

#define ADIE_RX_PA_BIAS3_HPH_L_C2_M                   0x30
#define ADIE_RX_PA_BIAS_REG3_HPH_L_C2_20_UA_V         0x00
#define ADIE_RX_PA_BIAS_REG3_HPH_L_C2_10_UA_V         0x10
#define ADIE_RX_PA_BIAS_REG3_HPH_L_C2_15_UA_V         0x20
#define ADIE_RX_PA_BIAS_REG3_HPH_L_C2_25_UA_V         0x30

#define ADIE_RX_PA_BIAS3_HPH_L_C1_M                   0xC0
#define ADIE_RX_PA_BIAS_REG3_HPH_L_C1_20_UA_V         0x00
#define ADIE_RX_PA_BIAS_REG3_HPH_L_C1_10_UA_V         0x40
#define ADIE_RX_PA_BIAS_REG3_HPH_L_C1_15_UA_V         0x80
#define ADIE_RX_PA_BIAS_REG3_HPH_L_C1_25_UA_V         0xC0

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_BIAS_REG4 : CODEC Rx PA Bias Register4

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_BIAS_REGISTER4_R                   0x96
#define ADIE_RX_PA_BIAS_REGISTER4_INIT_M              0xFF
#define ADIE_RX_PA_BIAS_REGISTER4_INIT_V              0x00

#define ADIE_RX_PA_BIAS4_HPH_R_REF_BUF2_M             0x03
#define ADIE_RX_PA_BIAS_REG4_HPH_R_REF_BUF2_20_UA_V   0x00
#define ADIE_RX_PA_BIAS_REG4_HPH_R_REF_BUF2_10_UA_V   0x01
#define ADIE_RX_PA_BIAS_REG4_HPH_R_REF_BUF2_15_UA_V   0x02
#define ADIE_RX_PA_BIAS_REG4_HPH_R_REF_BUF2_25_UA_V   0x03

#define ADIE_RX_PA_BIAS4_HPH_R_REF_BUF1_M             0x0C
#define ADIE_RX_PA_BIAS_REG4_HPH_R_REF_BUF1_20_UA_V   0x00
#define ADIE_RX_PA_BIAS_REG4_HPH_R_REF_BUF1_10_UA_V   0x04
#define ADIE_RX_PA_BIAS_REG4_HPH_R_REF_BUF1_15_UA_V   0x08
#define ADIE_RX_PA_BIAS_REG4_HPH_R_REF_BUF1_25_UA_V   0x0C

#define ADIE_RX_PA_BIAS4_HPH_R_C2_M                   0x30
#define ADIE_RX_PA_BIAS_REG4_HPH_R_C2_20_UA_V         0x00
#define ADIE_RX_PA_BIAS_REG4_HPH_R_C2_10_UA_V         0x10
#define ADIE_RX_PA_BIAS_REG4_HPH_R_C2_15_UA_V         0x20
#define ADIE_RX_PA_BIAS_REG4_HPH_R_C2_25_UA_V         0x30

#define ADIE_RX_PA_BIAS4_HPH_R_C1_M                   0xC0
#define ADIE_RX_PA_BIAS_REG4_HPH_R_C1_20_UA_V         0x00
#define ADIE_RX_PA_BIAS_REG4_HPH_R_C1_10_UA_V         0x40
#define ADIE_RX_PA_BIAS_REG4_HPH_R_C1_15_UA_V         0x80
#define ADIE_RX_PA_BIAS_REG4_HPH_R_C1_25_UA_V         0xC0

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_RX_PA_RESERVED : CODEC Rx PA Reserved

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_RX_PA_RESERVED_REGISTER_R                0x97
#define ADIE_RX_PA_RESERVED_REGISTER_INIT_M           0x00
#define ADIE_RX_PA_RESERVED_REGISTER_INIT_V           0x00

/*bits [0-7] are reserved*/

/* Dragon-3 variations */
/* none */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

ATEST_RX_REGISTER : ATESET Register for CODEC Rx

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_ATEST_RX_REGISTER_R                      0x98
#define ADIE_ATEST_RX_REGISTER_INIT_M                 0xFF
#define ADIE_ATEST_RX_REGISTER_INIT_V                 0x00

#define ADIE_ATEST_RX_ATEST2_M                        0x0F
#define ADIE_ATEST_RX_ATEST2_NONE_V                   0x00
#define ADIE_ATEST_RX_ATEST2_EAR_BIAS_I_V             0x01
#define ADIE_ATEST_RX_ATEST2_HPH_R_CM_REF_V           0x02
#define ADIE_ATEST_RX_ATEST2_HPH_R_OFFSET_I_V         0x03
#define ADIE_ATEST_RX_ATEST2_LOPMIC_CM_REF_V          0x04
#define ADIE_ATEST_RX_ATEST2_V_DIGCM_TST_R_V          0x05
#define ADIE_ATEST_RX_ATEST2_OTA2_OPL_V               0x06
#define ADIE_ATEST_RX_ATEST2_OTA2_OPR_V               0x07
#define ADIE_ATEST_RX_ATEST2_IPTAT100_L_V             0x08
#define ADIE_ATEST_RX_ATEST2_TST_OP_V                 0x09
#define ADIE_ATEST_RX_ATEST2_TST_OUTR_V               0x0A
#define ADIE_ATEST_RX_ATEST2_VOCM_OTA1_RX_FILTER_L_V  0x0B
#define ADIE_ATEST_RX_ATEST2_LOPMIC_R_V               0x0F

#define ADIE_ATEST_RX_ATEST1_M                        0xF0
#define ADIE_ATEST_RX_ATEST1_NONE_V                   0x00
#define ADIE_ATEST_RX_ATEST1_EAR_TO_CM_REF_CONNECT_V  0x10
#define ADIE_ATEST_RX_ATEST1_HPH_L_CM_REF_V           0x20
#define ADIE_ATEST_RX_ATEST1_HPH_L_OFFSET_I_V         0x30
#define ADIE_ATEST_RX_ATEST1_LOEXT_CM_REF_V           0x40
#define ADIE_ATEST_RX_ATEST1_V_DIGCM_TST_L_V          0x50
#define ADIE_ATEST_RX_ATEST1_OTA2_OML_V               0x60
#define ADIE_ATEST_RX_ATEST1_OTA2_OMR_V               0x70
#define ADIE_ATEST_RX_ATEST1_IBG100_L_V               0x80
#define ADIE_ATEST_RX_ATEST1_TST_OM_V                 0x90
#define ADIE_ATEST_RX_ATEST1_TST_OUTL_V               0xA0
#define ADIE_ATEST_RX_ATEST1_DAC_HI_V                 0xB0
#define ADIE_ATEST_RX_ATEST1_VOCM_OTA2_RX_FILTER_L_V  0xC0
#define ADIE_ATEST_RX_ATEST1_LOPMIC_L_V               0xF0

/* Dragon-3 variations */
/* none */

/*******************Ramsis codec defintions end here.***********************/




/////////////////////////////////////////////////////////////////////////////

#define ADIE_SUPPORT_OLD_CODEC_DEFS
#ifdef ADIE_SUPPORT_OLD_CODEC_DEFS //dyc 11/26/06

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

GPR_B1 : CODEC and TsHKADC general purpose register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_GPR_B1_R                                     0x30

#define ADIE_GPR_B1_INIT_M                                0xBF

#define ADIE_GPR_B1_ATEST_TO_HKADC_CONNECT_M              0x80
#define ADIE_GPR_B1_ATEST_TO_HKADC_CONNECT_V              0x80
#define ADIE_GPR_B1_ATEST_TO_HKADC_DISCONNECT_V           0x00

/* bit 6 reserved */

#define ADIE_GPR_B1_CODEC_TESTMUX_SEL_M                   0x20
#define ADIE_GPR_B1_CODEC_TESTMUX_SEL_V                   0x20
#define ADIE_GPR_B1_CODEC_TESTMUX_UNSEL_V                 0x00

#define ADIE_GPR_B1_TSHKADC_TESTMUX_SEL_M                 0x10
#define ADIE_GPR_B1_TSHKADC_TESTMUX_SEL_V                 0x10
#define ADIE_GPR_B1_TSHKADC_TESTMUX_UNSEL_V               0x00

#define ADIE_GPR_B1_ENB_CODEC_OUT_M                       0x08
#define ADIE_GPR_B1_CODEC_OUT_ENA_V                       0x00
#define ADIE_GPR_B1_CODEC_OUT_LOW_V                       0x08

#define ADIE_GPR_B1_ENB_TSHKADC_OUT_M                     0x04
#define ADIE_GPR_B1_TSHKADC_OUT_ENA_V                     0x00
#define ADIE_GPR_B1_TSHKADC_OUT_LOW_V                     0x04

#define ADIE_GPR_B1_SEL_VDD_MSM_TO_HKINT_M                0x02
#define ADIE_GPR_B1_VDD_MSM_TO_HKADC_CONNECT_V            0x02
#define ADIE_GPR_B1_VDD_MSM_TO_HKADC_DISCONNECT_V         0x00

#define ADIE_GPR_B1_EN_AUX_DDO_TST_M                      0x01
#define ADIE_GPR_B1_AUX_DDO_TST_ENA_V                     0x01
#define ADIE_GPR_B1_AUX_DDO_TST_DIS_V                     0x00

#ifdef FEATURE_SATURN2
#define ADIE_GPR_B1_INIT_V                                0x00
#else
#define ADIE_GPR_B1_INIT_V                                0x80
#endif

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

GPR_B2 : CODEC and TsHKADC general purpose register 2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_GPR_B2_R                                     0x31

#define ADIE_GPR_B2_INIT_M                                0xFF

#define ADIE_GPR_B2_ATEST2_TO_XII_SEL_M                   0x80
#define ADIE_GPR_B2_ATEST2_TO_XII_CONNECT_V               0x80
#define ADIE_GPR_B2_ATEST2_TO_XII_DISCONNECT_V            0x00

#define ADIE_GPR_B2_ATEST1_TO_XII_SEL_M                   0x40
#define ADIE_GPR_B2_ATEST1_TO_XII_CONNECT_V               0x40
#define ADIE_GPR_B2_ATEST1_TO_XII_DISCONNECT_V            0x00

#define ADIE_GPR_B2_ATEST2_TO_YUR_SEL_M                   0x20
#define ADIE_GPR_B2_ATEST2_TO_YUR_CONNECT_V               0x20
#define ADIE_GPR_B2_ATEST2_TO_YUR_DISCONNECT_V            0x00

#define ADIE_GPR_B2_ATEST1_TO_YUR_SEL_M                   0x10
#define ADIE_GPR_B2_ATEST1_TO_YUR_CONNECT_V               0x10
#define ADIE_GPR_B2_ATEST1_TO_YUR_DISCONNECT_V            0x00

#define ADIE_GPR_B2_ATEST2_TO_XUL_SEL_M                   0x08
#define ADIE_GPR_B2_ATEST2_TO_XUL_CONNECT_V               0x08
#define ADIE_GPR_B2_ATEST2_TO_XUL_DISCONNECT_V            0x00

#define ADIE_GPR_B2_ATEST1_TO_XUL_SEL_M                   0x04
#define ADIE_GPR_B2_ATEST1_TO_XUL_CONNECT_V               0x04
#define ADIE_GPR_B2_ATEST1_TO_XUL_DISCONNECT_V            0x00

#define ADIE_GPR_B2_SSBI_AUX_SW_RESET_M                   0x02
#define ADIE_GPR_B2_SSBI_AUX_SW_RESET_V                   0x02
#define ADIE_GPR_B2_SSBI_AUX_NO_SW_RESET_V                0x00

#define ADIE_GPR_B2_SSBI_AUX_TCXO_OFF_M                   0x01
#define ADIE_GPR_B2_SSBI_AUX_TCXO_OFF_V                   0x01
#define ADIE_GPR_B2_SSBI_AUX_NO_TCXO_OFF_V                0x00

#ifdef FEATURE_SATURN2
#define ADIE_GPR_B2_INIT_V                                0x00
#else
#define ADIE_GPR_B2_INIT_V                                0x80
#endif

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

DDO_B1 : CODEC and TsHKADC die-to-die output continuity register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_DDO_B1_R                                     0x32

#define ADIE_DDO_B1_INIT_M                                0x07

/* bits 7:3 reserved */

#define ADIE_DDO_B1_HSSD_IRPTB_M                          0x04
#define ADIE_DDO_B1_HSSD_IRPTB_FORCE_HIGH_V               0x04
#define ADIE_DDO_B1_HSSD_IRPTB_FORCE_LOW_V                0x00

#define ADIE_DDO_B1_TST_CODEC_DO_R_M                      0x02
#define ADIE_DDO_B1_CODEC_DO_R_FORCE_HIGH_V               0x02
#define ADIE_DDO_B1_CODEC_DO_R_FORCE_LOW_V                0x00

#define ADIE_DDO_B1_TST_CODEC_DO_L_M                      0x01
#define ADIE_DDO_B1_CODEC_DO_L_FORCE_HIGH_V               0x01
#define ADIE_DDO_B1_CODEC_DO_L_FORCE_LOW_V                0x00

#define ADIE_DDO_B1_INIT_V                                0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

DDO_B2 : CODEC and TsHKADC die-to-die output continuity register 2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_DDO_B2_R                                     0x33

#define ADIE_DDO_B2_INIT_M                                0x03

/* bits 7:2 reserved */

#define ADIE_DDO_B2_TST_PENIRQB_M                         0x02
#define ADIE_DDO_B2_TST_PENIRQB_ENA_V                     0x02
#define ADIE_DDO_B2_TST_PENIRQB_DIS_V                     0x00

#define ADIE_DDO_B2_TST_EOC_M                             0x01
#define ADIE_DDO_B2_TST_EOC_ENA_V                         0x01
#define ADIE_DDO_B2_TST_EOC_DIS_V                         0x00

#define ADIE_DDO_B2_INIT_V                                0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

DDI_B1 : CODEC and TsHKADC die-to-die input continuity register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_DDI_B1_R                                     0x34

#define ADIE_DDI_B1_INIT_M                                0x7F

/* bit 7 reserved */

#define ADIE_DDI_B1_TST_CODEC_DI_R23_M                    0x40
#define ADIE_DDI_B1_TST_CODEC_DI_R23_ENA_V                0x40
#define ADIE_DDI_B1_TST_CODEC_DI_R23_DIS_V                0x00

#define ADIE_DDI_B1_TST_CODEC_DI_R01_M                    0x20
#define ADIE_DDI_B1_TST_CODEC_DI_R01_ENA_V                0x20
#define ADIE_DDI_B1_TST_CODEC_DI_R01_DIS_V                0x00

#define ADIE_DDI_B1_TST_CODEC_DI_L23_M                    0x10
#define ADIE_DDI_B1_TST_CODEC_DI_L23_ENA_V                0x10
#define ADIE_DDI_B1_TST_CODEC_DI_L23_DIS_V                0x00

#define ADIE_DDI_B1_TST_CODEC_DI_L01_M                    0x08
#define ADIE_DDI_B1_TST_CODEC_DI_L01_ENA_V                0x08
#define ADIE_DDI_B1_TST_CODEC_DI_L01_DIS_V                0x00

#define ADIE_DDI_B1_TST_CODEC_TXCK_M                      0x04
#define ADIE_DDI_B1_TST_CODEC_TXCK_ENA_V                  0x04
#define ADIE_DDI_B1_TST_CODEC_TXCK_DIS_V                  0x00

#define ADIE_DDI_B1_TST_CODEC_RX_DLYCK_M                  0x02
#define ADIE_DDI_B1_TST_CODEC_RX_DLYCK_ENA_V              0x02
#define ADIE_DDI_B1_TST_CODEC_RX_DLYCK_DIS_V              0x00

#define ADIE_DDI_B1_TST_CODEC_RXCK_M                      0x01
#define ADIE_DDI_B1_TST_CODEC_RXCK_ENA_V                  0x01
#define ADIE_DDI_B1_TST_CODEC_RXCK_DIS_V                  0x00

#define ADIE_DDI_B1_INIT_V                                0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_EN1 : CODEC enable register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_EN1_R                                  0x35

#define ADIE_CODEC_EN1_INIT_M                             0xFF

#define ADIE_CODEC_EN1_EN_HPH_D_L_M                       0x80
#define ADIE_CODEC_EN1_HPH_D_L_ENA_V                      0x80
#define ADIE_CODEC_EN1_HPH_D_L_DIS_V                      0x00

#define ADIE_CODEC_EN1_EN_RXCK_RXCKDLY_L_M                0x40
#define ADIE_CODEC_EN1_RXCK_RXCKDLY_L_ENA_V               0x40
#define ADIE_CODEC_EN1_RXCK_RXCKDLY_L_DIS_V               0x00

#define ADIE_CODEC_EN1_EN_BIQU_L_M                        0x20
#define ADIE_CODEC_EN1_BIQU_L_ENA_V                       0x20
#define ADIE_CODEC_EN1_BIQU_L_DIS_V                       0x00

#define ADIE_CODEC_EN1_EN_EAR1_M                          0x10
#define ADIE_CODEC_EN1_EAR1_ENA_V                         0x10
#define ADIE_CODEC_EN1_EAR1_DIS_V                         0x00

#define ADIE_CODEC_EN1_EN_HPH_L_M                         0x08
#define ADIE_CODEC_EN1_HPH_L_ENA_V                        0x08
#define ADIE_CODEC_EN1_HPH_L_DIS_V                        0x00

#define ADIE_CODEC_EN1_EN_AUX_SE_M                        0x04
#define ADIE_CODEC_EN1_AUX_SE_ENA_V                       0x04
#define ADIE_CODEC_EN1_AUX_SE_DIS_V                       0x00

#define ADIE_CODEC_EN1_EN_LINE_OUT_L_M                    0x02
#define ADIE_CODEC_EN1_LINE_OUT_L_ENA_V                   0x02
#define ADIE_CODEC_EN1_LINE_OUT_L_DIS_V                   0x00

#define ADIE_CODEC_EN1_CODEC_ENABLE_M                     0x01
#define ADIE_CODEC_EN1_CODEC_ENA_V                        0x01
#define ADIE_CODEC_EN1_CODEC_DIS_V                        0x00

#define ADIE_CODEC_EN1_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_EN2 : CODEC enable register 2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_EN2_R                                  0x36

#define ADIE_CODEC_EN2_INIT_M                             0xFF

#define ADIE_CODEC_EN2_EN_HPH_D_R_M                       0x80
#define ADIE_CODEC_EN2_HPH_D_R_ENA_V                      0x80
#define ADIE_CODEC_EN2_HPH_D_R_DIS_V                      0x00

#define ADIE_CODEC_EN2_EN_RXCK_RXCKDLY_R_M                0x40
#define ADIE_CODEC_EN2_RXCK_RXCKDLY_R_ENA_V               0x40
#define ADIE_CODEC_EN2_RXCK_RXCKDLY_R_DIS_V               0x00

#define ADIE_CODEC_EN2_EN_BIQU_R_M                        0x20
#define ADIE_CODEC_EN2_BIQU_R_ENA_V                       0x20
#define ADIE_CODEC_EN2_BIQU_R_DIS_V                       0x00

#define ADIE_CODEC_EN2_EN_CHD_D_M                         0x10
#define ADIE_CODEC_EN2_CHD_D_ENA_V                        0x10
#define ADIE_CODEC_EN2_CHD_D_DIS_V                        0x00

#define ADIE_CODEC_EN2_EN_HPH_R_M                         0x08
#define ADIE_CODEC_EN2_HPH_R_ENA_V                        0x08
#define ADIE_CODEC_EN2_HPH_R_DIS_V                        0x00

#define ADIE_CODEC_EN2_EN_CHD_M                           0x04
#define ADIE_CODEC_EN2_CHD_ENA_V                          0x04
#define ADIE_CODEC_EN2_CHD_DIS_V                          0x00

#define ADIE_CODEC_EN2_EN_LINE_OUT_R_M                    0x02
#define ADIE_CODEC_EN2_LINE_OUT_R_ENA_V                   0x02
#define ADIE_CODEC_EN2_LINE_OUT_R_DIS_V                   0x00

#define ADIE_CODEC_EN2_EN_PA_REF_M                        0x01
#define ADIE_CODEC_EN2_PA_REF_ENA_V                       0x01
#define ADIE_CODEC_EN2_PA_REF_DIS_V                       0x00

#define ADIE_CODEC_EN2_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_EN3 : CODEC enable register 3

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_EN3_R                                  0x37

#define ADIE_CODEC_EN3_INIT_M                             0xFF

#define ADIE_CODEC_EN3_EN_MICBIAS_M                       0x80
#define ADIE_CODEC_EN3_MICBIAS_ENA_V                      0x80
#define ADIE_CODEC_EN3_MICBIAS_DIS_V                      0x00

#define ADIE_CODEC_EN3_EN_TXCK_L_M                        0x40
#define ADIE_CODEC_EN3_TXCK_L_ENA_V                       0x40
#define ADIE_CODEC_EN3_TXCK_L_DIS_V                       0x00

#define ADIE_CODEC_EN3_EN_DITHER_L_M                      0x20
#define ADIE_CODEC_EN3_DITHER_L_ENA_V                     0x20
#define ADIE_CODEC_EN3_DITHER_L_DIS_V                     0x00

#define ADIE_CODEC_EN3_EN_TXADC_L_M                       0x10
#define ADIE_CODEC_EN3_TXADC_L_ENA_V                      0x10
#define ADIE_CODEC_EN3_TXADC_L_DIS_V                      0x00

#define ADIE_CODEC_EN3_EN_AAF_L_M                         0x08
#define ADIE_CODEC_EN3_AAF_L_ENA_V                        0x08
#define ADIE_CODEC_EN3_AAF_L_DIS_V                        0x00

#define ADIE_CODEC_EN3_EN_MICAMP2_L_M                     0x04
#define ADIE_CODEC_EN3_MICAMP2_L_ENA_V                    0x04
#define ADIE_CODEC_EN3_MICAMP2_L_DIS_V                    0x00

#define ADIE_CODEC_EN3_EN_MICAMP1_L_M                     0x02
#define ADIE_CODEC_EN3_MICAMP1_L_ENA_V                    0x02
#define ADIE_CODEC_EN3_MICAMP1_L_DIS_V                    0x00

#define ADIE_CODEC_EN3_EN_REF_TXFE_L_M                    0x01
#define ADIE_CODEC_EN3_REF_TXFE_L_ENA_V                   0x01
#define ADIE_CODEC_EN3_REF_TXFE_L_DIS_V                   0x00

#define ADIE_CODEC_EN3_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_EN4 : CODEC enable register 4

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_EN4_R                                  0x38

#define ADIE_CODEC_EN4_INIT_M                             0x7F

/* bit 7 : reserved */

#define ADIE_CODEC_EN4_EN_TXCK_R_M                        0x40
#define ADIE_CODEC_EN4_TXCK_R_ENA_V                       0x40
#define ADIE_CODEC_EN4_TXCK_R_DIS_V                       0x00

#define ADIE_CODEC_EN4_EN_DITHER_R_M                      0x20
#define ADIE_CODEC_EN4_DITHER_R_ENA_V                     0x20
#define ADIE_CODEC_EN4_DITHER_R_DIS_V                     0x00

#define ADIE_CODEC_EN4_EN_TXADC_R_M                       0x10
#define ADIE_CODEC_EN4_TXADC_R_ENA_V                      0x10
#define ADIE_CODEC_EN4_TXADC_R_DIS_V                      0x00

#define ADIE_CODEC_EN4_EN_AAF_R_M                         0x08
#define ADIE_CODEC_EN4_AAF_R_ENA_V                        0x08
#define ADIE_CODEC_EN4_AAF_R_DIS_V                        0x00

#define ADIE_CODEC_EN4_EN_MICAMP2_R_M                     0x04
#define ADIE_CODEC_EN4_MICAMP2_R_ENA_V                    0x04
#define ADIE_CODEC_EN4_MICAMP2_R_DIS_V                    0x00

#define ADIE_CODEC_EN4_EN_MICAMP1_R_M                     0x02
#define ADIE_CODEC_EN4_MICAMP1_R_ENA_V                    0x02
#define ADIE_CODEC_EN4_MICAMP1_R_DIS_V                    0x00

#define ADIE_CODEC_EN4_EN_REF_TXFE_R_M                    0x01
#define ADIE_CODEC_EN4_REF_TXFE_R_ENA_V                   0x01
#define ADIE_CODEC_EN4_REF_TXFE_R_DIS_V                   0x00

#define ADIE_CODEC_EN4_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_EN5 : CODEC enable register 5

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_EN5_R                                  0x39

#define ADIE_CODEC_EN5_INIT_M                             0x0F

/* bits 7:4 : reserved */

#define ADIE_CODEC_EN5_EN_AUX_PGA_L_M                     0x08
#define ADIE_CODEC_EN5_AUX_PGA_L_ENA_V                    0x08
#define ADIE_CODEC_EN5_AUX_PGA_L_DIS_V                    0x00

#define ADIE_CODEC_EN5_EN_AUX_PGA_R_M                     0x04
#define ADIE_CODEC_EN5_AUX_PGA_R_ENA_V                    0x04
#define ADIE_CODEC_EN5_AUX_PGA_R_DIS_V                    0x00

#define ADIE_CODEC_EN5_EN_REF_BG_M                        0x02
#define ADIE_CODEC_EN5_REF_BG_ENA_V                       0x02
#define ADIE_CODEC_EN5_REF_BG_DIS_V                       0x00

#define ADIE_CODEC_EN5_EN_BANDGAP_M                       0x01
#define ADIE_CODEC_EN5_BANDGAP_ENA_V                      0x01
#define ADIE_CODEC_EN5_BANDGAP_DIS_V                      0x00

#define ADIE_CODEC_EN5_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_EN6 : CODEC enable register 6

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_EN6_R                                  0x3A

#define ADIE_CODEC_EN6_INIT_M                             0xFF

#define ADIE_CODEC_EN6_EN_HPH_CHD_REF_M                   0x80
#define ADIE_CODEC_EN6_HPH_CHD_REF_ENA_V                  0x80
#define ADIE_CODEC_EN6_HPH_CHD_REF_DIS_V                  0x00

#define ADIE_CODEC_EN6_EN_RX_INDATA0_TRIBUF_M             0x40
#define ADIE_CODEC_EN6_RX_INDATA0_TRIBUF_ENA_V            0x40
#define ADIE_CODEC_EN6_RX_INDATA0_TRIBUF_DIS_V            0x00

#define ADIE_CODEC_EN6_EN_RX_DWA0_TRIBUF_M                0x20
#define ADIE_CODEC_EN6_RX_DWA0_TRIBUF_ENA_V               0x20
#define ADIE_CODEC_EN6_RX_DWA0_TRIBUF_DIS_V               0x00

#define ADIE_CODEC_EN6_EN_CMOS_DRIVER_M                   0x10
#define ADIE_CODEC_EN6_CMOS_DRIVER_ENA_V                  0x10
#define ADIE_CODEC_EN6_CMOS_DRIVER_DIS_V                  0x00

#define ADIE_CODEC_EN6_EN_BIQU_TSTBUF2_M                  0x08
#define ADIE_CODEC_EN6_BIQU_TSTBUF2_ENA_V                 0x08
#define ADIE_CODEC_EN6_BIQU_TSTBUF2_DIS_V                 0x00

#define ADIE_CODEC_EN6_EN_BIQU_TSTBUF1_M                  0x04
#define ADIE_CODEC_EN6_BIQU_TSTBUF1_ENA_V                 0x04
#define ADIE_CODEC_EN6_BIQU_TSTBUF1_DIS_V                 0x00

#define ADIE_CODEC_EN6_EN_CDC_TSTBUF2_M                   0x02
#define ADIE_CODEC_EN6_CDC_TSTBUF2_ENA_V                  0x02
#define ADIE_CODEC_EN6_CDC_TSTBUF2_DIS_V                  0x00

#define ADIE_CODEC_EN6_EN_CDC_TSTBUF1_M                   0x01
#define ADIE_CODEC_EN6_CDC_TSTBUF1_ENA_V                  0x01
#define ADIE_CODEC_EN6_CDC_TSTBUF1_DIS_V                  0x00

#define ADIE_CODEC_EN6_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_EN7 : CODEC enable register 7 (0x3B : reserved)

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_EN7_R                                  0x3B

#define ADIE_CODEC_EN7_RESERVED_V                         0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C1 : CODEC control register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C1_R                                   0x3C

#define ADIE_CODEC_C1_INIT_M                              0xFF

#define ADIE_CODEC_C1_MICAMP1_GAIN_L_M                    0x80
#define ADIE_CODEC_C1_MICAMP1_GAIN_L_24_DB_V              0x00
#define ADIE_CODEC_C1_MICAMP1_GAIN_L_0_DB_V               0x80

#define ADIE_CODEC_C1_MICAMP2_GAIN_L_M                    0x7C
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_25D5_DB_V            0x00
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_24_DB_V              0x04
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_22D5_DB_V            0x08
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_21_DB_V              0x0C
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_19D5_DB_V            0x10
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_18_DB_V              0x14
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_16D5_DB_V            0x18
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_15_DB_V              0x1C
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_13D5_DB_V            0x20
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_12_DB_V              0x24
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_10D5_DB_V            0x28
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_9_DB_V               0x2C
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_7D5_DB_V             0x30
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_6_DB_V               0x34
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_4D5_DB_V             0x38
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_3_DB_V               0x3C
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_1D5_DB_V             0x40
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_0_DB_V               0x44
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_N1D5_DB_V            0x48
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_N3_DB_V              0x4C
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_N4D5_DB_V            0x50
#define ADIE_CODEC_C1_MICAMP2_GAIN_L_N6_DB_V              0x54

#define ADIE_CODEC_C1_RESET_MICAMP_INPUT_L_M              0x02
#define ADIE_CODEC_C1_MICAMP_INPUT_L_RESET_V              0x02
#define ADIE_CODEC_C1_MICAMP_INPUT_L_NO_RESET_V           0x00

#define ADIE_CODEC_C1_DIS_MICAMP_STARTUP_L_M              0x01
#define ADIE_CODEC_C1_MICAMP_STARTUP_L_DIS_V              0x01
#define ADIE_CODEC_C1_MICAMP_STARTUP_L_ENA_V              0x00

#define ADIE_CODEC_C1_INIT_V                              0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C2 : CODEC control register 2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C2_R                                   0x3D

#define ADIE_CODEC_C2_INIT_M                              0xFF

#define ADIE_CODEC_C2_MICAMP1_GAIN_R_M                    0x80
#define ADIE_CODEC_C2_MICAMP1_GAIN_R_24_DB_V              0x00
#define ADIE_CODEC_C2_MICAMP1_GAIN_R_0_DB_V               0x80

#define ADIE_CODEC_C2_MICAMP2_GAIN_R_M                    0x7C
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_25D5_DB_V            0x00
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_24_DB_V              0x04
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_22D5_DB_V            0x08
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_21_DB_V              0x0C
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_19D5_DB_V            0x10
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_18_DB_V              0x14
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_16D5_DB_V            0x18
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_15_DB_V              0x1C
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_13D5_DB_V            0x20
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_12_DB_V              0x24
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_10D5_DB_V            0x28
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_9_DB_V               0x2C
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_7D5_DB_V             0x30
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_6_DB_V               0x34
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_4D5_DB_V             0x38
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_3_DB_V               0x3C
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_1D5_DB_V             0x40
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_0_DB_V               0x44
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_N1D5_DB_V            0x48
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_N3_DB_V              0x4C
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_N4D5_DB_V            0x50
#define ADIE_CODEC_C2_MICAMP2_GAIN_R_N6_DB_V              0x54

#define ADIE_CODEC_C2_RESET_MICAMP_INPUT_R_M              0x02
#define ADIE_CODEC_C2_MICAMP_INPUT_R_RESET_V              0x02
#define ADIE_CODEC_C2_MICAMP_INPUT_R_NO_RESET_V           0x00

#define ADIE_CODEC_C2_DIS_MICAMP_STARTUP_R_M              0x01
#define ADIE_CODEC_C2_MICAMP_STARTUP_R_DIS_V              0x01
#define ADIE_CODEC_C2_MICAMP_STARTUP_R_ENA_V              0x00

#define ADIE_CODEC_C2_INIT_V                              0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C3 : CODEC control register 3

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C3_R                                   0x3E

#define ADIE_CODEC_C3_INIT_M                              0xFF

#define ADIE_CODEC_C3_MICA_CH_SEL_M                       0x80
#define ADIE_CODEC_C3_MICA_CH_L_V                         0x00
#define ADIE_CODEC_C3_MICA_CH_R_V                         0x80

#define ADIE_CODEC_C3_MIC2_CH_SEL_M                       0x40
#define ADIE_CODEC_C3_MIC2_CH_L_V                         0x00
#define ADIE_CODEC_C3_MIC2_CH_R_V                         0x40

#define ADIE_CODEC_C3_MIC1_CH_SEL_M                       0x20
#define ADIE_CODEC_C3_MIC1_CH_L_V                         0x00
#define ADIE_CODEC_C3_MIC1_CH_R_V                         0x20

#define ADIE_CODEC_C3_INPUT_SEL_L_M                       0x18
#define ADIE_CODEC_C3_INPUT_L_MIC1_PN_V                   0x00
#define ADIE_CODEC_C3_INPUT_L_MIC2_PN_V                   0x08
#define ADIE_CODEC_C3_INPUT_L_MICA_PN_V                   0x10
#define ADIE_CODEC_C3_INPUT_L_LINEI_PN_V                  0x18

#define ADIE_CODEC_C3_AAF_BYPASS_L_M                      0x04
#define ADIE_CODEC_C3_AAF_L_BYPASS_V                      0x04
#define ADIE_CODEC_C3_AAF_L_NO_BYPASS_V                   0x00

#define ADIE_CODEC_C3_MICAMP2_BYPASS_L_M                  0x02
#define ADIE_CODEC_C3_MICAMP2_L_BYPASS_V                  0x02
#define ADIE_CODEC_C3_MICAMP2_L_NO_BYPASS_V               0x00

#define ADIE_CODEC_C3_MICAMP1_BYPASS_L_M                  0x01
#define ADIE_CODEC_C3_MICAMP1_L_BYPASS_V                  0x01
#define ADIE_CODEC_C3_MICAMP1_L_NO_BYPASS_V               0x00

#define ADIE_CODEC_C3_INIT_V                              0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C4 : CODEC control register 4

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C4_R                                   0x3F

#define ADIE_CODEC_C4_INIT_M                              0xFF

#define ADIE_CODEC_C4_TXADC_CLK_ADV_ON_M                  0x80
#define ADIE_CODEC_C4_TXADC_CLK_ADV_ENA_V                 0x80
#define ADIE_CODEC_C4_TXADC_CLK_ADV_DIS_V                 0x00

#define ADIE_CODEC_C4_MICAMP1_I_CTL_M                     0x60
#define ADIE_CODEC_C4_MICAMP1_I_500_UA_V                  0x00
#define ADIE_CODEC_C4_MICAMP1_I_750_UA_V                  0x40
#define ADIE_CODEC_C4_MICAMP1_I_1_MA_V                    0x60
#define ADIE_CODEC_C4_MICAMP12_I_750_UA_V                 0x40
#define ADIE_CODEC_C4_MICAMP12_I_1_MA_V                   0x60

#define ADIE_CODEC_C4_INPUT_SEL_R_M                       0x18
#define ADIE_CODEC_C4_INPUT_R_MIC1_PN_V                   0x00
#define ADIE_CODEC_C4_INPUT_R_MIC2_PN_V                   0x08
#define ADIE_CODEC_C4_INPUT_R_MICA_PN_V                   0x10
#define ADIE_CODEC_C4_INPUT_R_LINEI_PN_V                  0x18

#define ADIE_CODEC_C4_AAF_BYPASS_R_M                      0x04
#define ADIE_CODEC_C4_AAF_R_BYPASS_V                      0x04
#define ADIE_CODEC_C4_AAF_R_NO_BYPASS_V                   0x00

#define ADIE_CODEC_C4_MICAMP2_BYPASS_R_M                  0x02
#define ADIE_CODEC_C4_MICAMP2_R_BYPASS_V                  0x02
#define ADIE_CODEC_C4_MICAMP2_R_NO_BYPASS_V               0x00

#define ADIE_CODEC_C4_MICAMP1_BYPASS_R_M                  0x01
#define ADIE_CODEC_C4_MICAMP1_R_BYPASS_V                  0x01
#define ADIE_CODEC_C4_MICAMP1_R_NO_BYPASS_V               0x00

#define ADIE_CODEC_C4_INIT_V                              0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C5 : CODEC control register 5

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C5_R                                   0x40

#define ADIE_CODEC_C5_INIT_M                              0xFF

#define ADIE_CODEC_C5_DITHER_SYNC_M                       0x80
#define ADIE_CODEC_C5_DITHER_SYNC_RISING_EDGE_V           0x80
#define ADIE_CODEC_C5_DITHER_SYNC_FALLING_EDGE_V          0x00

#define ADIE_CODEC_C5_TX_CLK_CTL_M                        0x40
#define ADIE_CODEC_C5_FORM_XOR_CLK_V                      0x40
#define ADIE_CODEC_C5_MAIN_TX_CLK_V                       0x00

#define ADIE_CODEC_C5_DITHER_I_CTL_M                      0x30
#define ADIE_CODEC_C5_DITHER_I_80_UA_V                    0x30
#define ADIE_CODEC_C5_DITHER_I_60_UA_V                    0x20
#define ADIE_CODEC_C5_DITHER_I_40_UA_V                    0x00
#define ADIE_CODEC_C5_DITHER_I_20_UA_V                    0x10

#define ADIE_CODEC_C5_ADD_0P33_ADC_M                      0x08
#define ADIE_CODEC_C5_ADD_0P33_ADC_ENA_V                  0x08
#define ADIE_CODEC_C5_ADD_0P33_ADC_DIS_V                  0x00

#define ADIE_CODEC_C5_TXADC_BIAS_SEL_M                    0x07
#define ADIE_CODEC_C5_TXADC_BIAS_20_UA_V                  0x00
#define ADIE_CODEC_C5_TXADC_BIAS_16D66_UA_V               0x01
#define ADIE_CODEC_C5_TXADC_BIAS_13D33_UA_V               0x02
#define ADIE_CODEC_C5_TXADC_BIAS_10_UA_V                  0x03
#define ADIE_CODEC_C5_TXADC_BIAS_6D66_UA_V                0x04
#define ADIE_CODEC_C5_TXADC_BIAS_3D33_UA_V                0x05

#define ADIE_CODEC_C5_INIT_V                              0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C6 : CODEC control register 6

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C6_R                                   0x41

#define ADIE_CODEC_C6_INIT_M                              0xFF

#define ADIE_CODEC_C6_TX_DATA_SYNC_POL_M                  0x80
#define ADIE_CODEC_C6_TX_DATA_SYNC_RISING_EDGE_V          0x80
#define ADIE_CODEC_C6_TX_DATA_SYNC_FALLING_EDGE_V         0x00

#define ADIE_CODEC_C6_RX_DATA_SYNC_POL_M                  0x40
#define ADIE_CODEC_C6_RX_DATA_SYNC_RISING_EDGE_V          0x40
#define ADIE_CODEC_C6_RX_DATA_SYNC_FALLING_EDGE_V         0x00

#define ADIE_CODEC_C6_BIQU_DACREF_CTL_M                   0x30
#define ADIE_CODEC_C6_BIQU_DACREF_VDD_V                   0x00
#define ADIE_CODEC_C6_BIQU_DACREF_1D9_V_V                 0x10
#define ADIE_CODEC_C6_BIQU_DACREF_2D025_V_V               0x20
#define ADIE_CODEC_C6_BIQU_DACREF_1D618_V_V               0x30

#define ADIE_CODEC_C6_BIQU_OTA_BW_CTL_M                   0x0F
#define ADIE_CODEC_C6_BIQU_OTA_BW_80_MHZ_V                0x00
#define ADIE_CODEC_C6_BIQU_OTA_BW_55_MHZ_V                0x08
#define ADIE_CODEC_C6_BIQU_OTA_BW_42_MHZ_V                0x0C
#define ADIE_CODEC_C6_BIQU_OTA_BW_22_MHZ_V                0x0E

#define ADIE_CODEC_C6_INIT_V  (ADIE_CODEC_C6_BIQU_DACREF_1D9_V_V)


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C7 : CODEC control register 7

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C7_R                                  0x42

#define ADIE_CODEC_C7_INIT_M                             0xFF

#define ADIE_CODEC_C7_AUX_PGA_TX_INP_SEL_L_M             0x80
#define ADIE_CODEC_C7_TX_TO_AUX_PGA_L_V                  0x80
#define ADIE_CODEC_C7_NO_TX_TO_AUX_PGA_L_V               0x00

#define ADIE_CODEC_C7_AUX_PGA_TX_INP_SEL_R_M             0x40
#define ADIE_CODEC_C7_TX_TO_AUX_PGA_R_V                  0x40
#define ADIE_CODEC_C7_NO_TX_TO_AUX_PGA_R_V               0x00

#define ADIE_CODEC_C7_AUX_PGA_CTL_M                      0x3C
#define ADIE_CODEC_C7_AUX_PGA_GAIN_11D5_DB_V             0x00
#define ADIE_CODEC_C7_AUX_PGA_GAIN_8D5_DB_V              0x04
#define ADIE_CODEC_C7_AUX_PGA_GAIN_5D5_DB_V              0x08
#define ADIE_CODEC_C7_AUX_PGA_GAIN_2D5_DB_V              0x0C
#define ADIE_CODEC_C7_AUX_PGA_GAIN_N0D5_DB_V             0x10
#define ADIE_CODEC_C7_AUX_PGA_GAIN_N3D5_DB_V             0x14
#define ADIE_CODEC_C7_AUX_PGA_GAIN_N6D5_DB_V             0x18
#define ADIE_CODEC_C7_AUX_PGA_GAIN_N9D5_DB_V             0x1C
#define ADIE_CODEC_C7_AUX_PGA_GAIN_N12D5_DB_V            0x20
#define ADIE_CODEC_C7_AUX_PGA_GAIN_N15D5_DB_V            0x24
#define ADIE_CODEC_C7_AUX_PGA_GAIN_N18D5_DB_V            0x28
#define ADIE_CODEC_C7_AUX_PGA_GAIN_N21D5_DB_V            0x2C
#define ADIE_CODEC_C7_AUX_PGA_GAIN_N24D5_DB_V            0x30

#define ADIE_CODEC_C7_AUX_PGA_RESET_M                    0x02
#define ADIE_CODEC_C7_AUX_PGA_RESET_V                    0x02
#define ADIE_CODEC_C7_AUX_PGA_NO_RESET_V                 0x00

#define ADIE_CODEC_C7_AUX_PGA_HWSTARTUP_DIS_M            0x01
#define ADIE_CODEC_C7_AUX_PGA_HWSTARTUP_DIS_V            0x01
#define ADIE_CODEC_C7_AUX_PGA_HWSTARTUP_ENA_V            0x00

#define ADIE_CODEC_C7_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C7B : CODEC control register 7B

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C7B_R                                  0x43

#define ADIE_CODEC_C7B_INIT_M                             0xFF

#define ADIE_CODEC_C7B_VREF_TO_CHD_SEL_M                  0x80
#define ADIE_CODEC_C7B_VREF_TO_CHD_CONNECT_V              0x80
#define ADIE_CODEC_C7B_VREF_TO_CHD_DISCONNECT_V           0x00

#define ADIE_CODEC_C7B_HSSD_TO_MIC2_CONNECT_M             0x40
#define ADIE_CODEC_C7B_HSSD_TO_MIC2_CONNECT_V             0x40
#define ADIE_CODEC_C7B_HSSD_TO_MIC2_DISCONNECT_V          0x00

#define ADIE_CODEC_C7B_AUX_PGA_VOCM_SEL_M                 0x20
#define ADIE_CODEC_C7B_AUX_PGA_VOCM_TX_V                  0x00
#define ADIE_CODEC_C7B_AUX_PGA_VOCM_LOCAL_V               0x20

#define ADIE_CODEC_C7B_AUX_PGA_VICM_SEL_M                 0x10
#define ADIE_CODEC_C7B_AUX_PGA_VICM_ONCHIP_V              0x10
#define ADIE_CODEC_C7B_AUX_PGA_VICM_NOT_ONCHIP_V          0x00

#define ADIE_CODEC_C7B_AUX_PGA_RXP_L_M                    0x08
#define ADIE_CODEC_C7B_AUX_PGA_RXP_L_SEL_V                0x08
#define ADIE_CODEC_C7B_AUX_PGA_RXP_L_UNSEL_V              0x00

#define ADIE_CODEC_C7B_AUX_PGA_RXM_L_M                    0x04
#define ADIE_CODEC_C7B_AUX_PGA_RXM_L_SEL_V                0x04
#define ADIE_CODEC_C7B_AUX_PGA_RXM_L_UNSEL_V              0x00

#define ADIE_CODEC_C7B_AUX_PGA_RXP_R_M                    0x02
#define ADIE_CODEC_C7B_AUX_PGA_RXP_R_SEL_V                0x02
#define ADIE_CODEC_C7B_AUX_PGA_RXP_R_UNSEL_V              0x00

#define ADIE_CODEC_C7B_AUX_PGA_RXM_R_M                    0x01
#define ADIE_CODEC_C7B_AUX_PGA_RXM_R_SEL_V                0x01
#define ADIE_CODEC_C7B_AUX_PGA_RXM_R_UNSEL_V              0x00

#define ADIE_CODEC_C7B_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C8 : CODEC control register 8

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C8_R                                   0x44

#define ADIE_CODEC_C8_INIT_M                              0xFF

#define ADIE_CODEC_C8_PA_POWER_UP_CTL_M                   0x80
#define ADIE_CODEC_C8_PA_POWER_UP_AUTO_V                  0x00
#define ADIE_CODEC_C8_PA_POWER_UP_MANUAL_V                0x80

#define ADIE_CODEC_C8_BYPASS_CDC_TBUF_M                   0x40
#define ADIE_CODEC_C8_CDC_TBUF_BYPASS_V                   0x40
#define ADIE_CODEC_C8_CDC_TBUF_NO_BYPASS_V                0x00

#define ADIE_CODEC_C8_PROG_I_ADD2_M                       0x20
#define ADIE_CODEC_C8_PROG_I_ADD2_ENA_V                   0x20
#define ADIE_CODEC_C8_PROG_I_ADD2_DIS_V                   0x00

#define ADIE_CODEC_C8_PROG_I_ADD_M                        0x10
#define ADIE_CODEC_C8_PROG_I_ADD_ENA_V                    0x10
#define ADIE_CODEC_C8_PROG_I_ADD_DIS_V                    0x00

#define ADIE_CODEC_C8_HSSD_MIC2_CAP_VAL_M                 0x08
#define ADIE_CODEC_C8_HSSD_MIC2_CAP_0D1_UF_V              0x00
#define ADIE_CODEC_C8_HSSD_MIC2_CAP_1_UF_V                0x08

#define ADIE_CODEC_C8_SEL_HPH_DIFF_M                      0x04
#define ADIE_CODEC_C8_HPH_DIFF_SEL_V                      0x04
#define ADIE_CODEC_C8_HPH_DIFF_UNSEL_V                    0x00

#define ADIE_CODEC_C8_CODEC_LOOPBACK_R_M                  0x02
#define ADIE_CODEC_C8_CODEC_LOOPBACK_R_ENA_V              0x02
#define ADIE_CODEC_C8_CODEC_LOOPBACK_R_DIS_V              0x00

#define ADIE_CODEC_C8_CODEC_LOOPBACK_L_M                  0x01
#define ADIE_CODEC_C8_CODEC_LOOPBACK_L_ENA_V              0x01
#define ADIE_CODEC_C8_CODEC_LOOPBACK_L_DIS_V              0x00

#define ADIE_CODEC_C8_INIT_V                              0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C9 : CODEC control register 9

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C9_R                                   0x45

#define ADIE_CODEC_C9_INIT_M                              0xDF

#define ADIE_CODEC_C9_PA_INPUT_SEL_L_M                    0x80
#define ADIE_CODEC_C9_PA_INPUT_L_TO_R_SEL_V               0x80
#define ADIE_CODEC_C9_PA_INPUT_L_TO_R_UNSEL_V             0x00

#define ADIE_CODEC_C9_SEL_BIQU_L_OUTPUT_M                 0x40
#define ADIE_CODEC_C9_BIQU_L_OUTPUT_SEL_V                 0x40
#define ADIE_CODEC_C9_BIQU_L_OUTPUT_UNSEL_V               0x00

/* bit 5 : reserved */

#define ADIE_CODEC_C9_SEL_LINE_OUT_DIFF_M                 0x10
#define ADIE_CODEC_C9_LINE_OUT_DIFF_SEL_V                 0x10
#define ADIE_CODEC_C9_LINE_OUT_DIFF_UNSEL_V               0x00

#define ADIE_CODEC_C9_PA_INPUT_SEL_R_M                    0x08
#define ADIE_CODEC_C9_PA_INPUT_R_TO_L_SEL_V               0x08
#define ADIE_CODEC_C9_PA_INPUT_R_TO_L_UNSEL_V             0x00
#define ADIE_CODEC_C9_PA_INPUT_SEL_R_TO_L_SEL_V           0x08
#define ADIE_CODEC_C9_PA_INPUT_SEL_R_TO_L_UNSEL_V         0x00

#define ADIE_CODEC_C9_SHORT_PWAMP_INPUT_M                 0x04
#define ADIE_CODEC_C9_SHORT_PWAMP_INPUT_ENA_V             0x04
#define ADIE_CODEC_C9_SHORT_PWAMP_INPUT_DIS_V             0x00

#define ADIE_CODEC_C9_CDC_BIQU_TBUF2_BYPASS_M             0x02
#define ADIE_CODEC_C9_CDC_BIQU_TBUF2_BYPASS_V             0x02
#define ADIE_CODEC_C9_CDC_BIQU_TBUF2_NO_BYPASS_V          0x00

#define ADIE_CODEC_C9_CDC_BIQU_TBUF1_BYPASS_M             0x01
#define ADIE_CODEC_C9_CDC_BIQU_TBUF1_BYPASS_V             0x01
#define ADIE_CODEC_C9_CDC_BIQU_TBUF1_NO_BYPASS_V          0x00

#define ADIE_CODEC_C9_INIT_V                              0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C10 : CODEC control register 10

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C10_R                                  0x46

#define ADIE_CODEC_C10_INIT_M                             0xFF

#define ADIE_CODEC_C10_SEL_TEMP_SENSOR_M                  0x80
#define ADIE_CODEC_C10_TEMP_SENSOR_SEL_V                  0x80
#define ADIE_CODEC_C10_TEMP_SENSOR_UNSEL_V                0x00

#define ADIE_CODEC_C10_EN_BIQU_OTA1_OUT_M                 0x40
#define ADIE_CODEC_C10_BIQU_OTA1_OUT_ENA_V                0x40
#define ADIE_CODEC_C10_BIQU_OTA1_OUT_DIS_V                0x00

#define ADIE_CODEC_C10_AUX_SE_ADD_LINEI_L_M               0x20
#define ADIE_CODEC_C10_AUX_SE_ADD_LINEI_L_ENA_V           0x20
#define ADIE_CODEC_C10_AUX_SE_ADD_LINEI_L_DIS_V           0x00

#define ADIE_CODEC_C10_EAR1_ADD_LINEI_L_M                 0x10
#define ADIE_CODEC_C10_EAR1_ADD_LINEI_L_ENA_V             0x10
#define ADIE_CODEC_C10_EAR1_ADD_LINEI_L_DIS_V             0x00

#define ADIE_CODEC_C10_HPH_L_ADD_LINEI_L_M                0x08
#define ADIE_CODEC_C10_HPH_L_ADD_LINEI_L_ENA_V            0x08
#define ADIE_CODEC_C10_HPH_L_ADD_LINEI_L_DIS_V            0x00

#define ADIE_CODEC_C10_HPH_R_ADD_LINEI_R_M                0x04
#define ADIE_CODEC_C10_HPH_R_ADD_LINEI_R_ENA_V            0x04
#define ADIE_CODEC_C10_HPH_R_ADD_LINEI_R_DIS_V            0x00

#define ADIE_CODEC_C10_LINEO_L_ADD_LINEI_L_M              0x02
#define ADIE_CODEC_C10_LINEO_L_ADD_LINEI_L_ENA_V          0x02
#define ADIE_CODEC_C10_LINEO_L_ADD_LINEI_L_DIS_V          0x00

#define ADIE_CODEC_C10_LINEO_R_ADD_LINEI_R_M              0x01
#define ADIE_CODEC_C10_LINEO_R_ADD_LINEI_R_ENA_V          0x01
#define ADIE_CODEC_C10_LINEO_R_ADD_LINEI_R_DIS_V          0x00

#define ADIE_CODEC_C10_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C11 : CODEC control register 11

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C11_R                                  0x47

#define ADIE_CODEC_C11_INIT_M                             0xFF

#define ADIE_CODEC_C11_TXADC_INP_L_SHORT_M                0x80
#define ADIE_CODEC_C11_TXADC_INP_L_SHORT_ENA_V            0x80
#define ADIE_CODEC_C11_TXADC_INP_L_SHORT_DIS_V            0x00

#define ADIE_CODEC_C11_TXADC_INP_R_SHORT_M                0x40
#define ADIE_CODEC_C11_TXADC_INP_R_SHORT_ENA_V            0x40
#define ADIE_CODEC_C11_TXADC_INP_R_SHORT_DIS_V            0x00

#define ADIE_CODEC_C11_EN_TXADC_OTA2_OUT_M                0x20
#define ADIE_CODEC_C11_TXADC_OTA2_OUT_ENA_V               0x20
#define ADIE_CODEC_C11_TXADC_OTA2_OUT_DIS_V               0x00

#define ADIE_CODEC_C11_EN_TXADC_OTA1_OUT_M                0x10
#define ADIE_CODEC_C11_TXADC_OTA1_OUT_ENA_V               0x10
#define ADIE_CODEC_C11_TXADC_OTA1_OUT_DIS_V               0x00

#define ADIE_CODEC_C11_TXADC_DAC_REF_SEL_M                0x08
#define ADIE_CODEC_C11_TXADC_DAC_REF_1D8_V_V              0x00
#define ADIE_CODEC_C11_TXADC_DAC_REF_1D9_V_V              0x08

#define ADIE_CODEC_C11_CHD2VDD_M                          0x04
#define ADIE_CODEC_C11_CHD2VDD_V                          0x04
#define ADIE_CODEC_C11_CHD2VSS_V                          0x00

#define ADIE_CODEC_C11_CHD2RAIL_M                         0x02
#define ADIE_CODEC_C11_CHD2RAIL_V                         0x02
#define ADIE_CODEC_C11_CHD_PD_TRISTATE_V                  0x00

#define ADIE_CODEC_C11_CHD_LOOPBACK_M                     0x01
#define ADIE_CODEC_C11_CHD_LOOPBACK_ENA_V                 0x01
#define ADIE_CODEC_C11_CHD_LOOPBACK_DIS_V                 0x00

#define ADIE_CODEC_C11_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C12 : CODEC control register 12

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C12_R                                  0x48

#define ADIE_CODEC_C12_INIT_M                             0xFF

#define ADIE_CODEC_C12_SEL_HSSD_M                         0x80
#define ADIE_CODEC_C12_HSSD_SEL_V                         0x80
#define ADIE_CODEC_C12_HSSD_UNSEL_V                       0x00

#define ADIE_CODEC_C12_HSSD_TO_HKADC_M                    0x40
#define ADIE_CODEC_C12_HSSD_TO_HKADC_ENA_V                0x40
#define ADIE_CODEC_C12_HSSD_TO_HKADC_DIS_V                0x00

#define ADIE_CODEC_C12_MICAMP2_I_CTL_M                    0x30
#define ADIE_CODEC_C12_MICAMP2_I_500_UA_V                 0x00
#define ADIE_CODEC_C12_MICAMP2_I_750_UA_V                 0x20
#define ADIE_CODEC_C12_MICAMP2_I_1_MA_V                   0x30

#define ADIE_CODEC_C12_AAF_I_CTL_M                        0x0F
#define ADIE_CODEC_C12_OTA1_190_UA_OTA2_230_UA_V          0x00
#define ADIE_CODEC_C12_OTA1_380_UA_OTA2_230_UA_V          0x01
#define ADIE_CODEC_C12_OTA1_190_UA_OTA2_460_UA_V          0x02
#define ADIE_CODEC_C12_OTA1_380_UA_OTA2_460_UA_V          0x04
#define ADIE_CODEC_C12_OTA1_570_UA_OTA2_460_UA_V          0x05
#define ADIE_CODEC_C12_OTA1_380_UA_OTA2_690_UA_V          0x06
#define ADIE_CODEC_C12_OTA1_760_UA_OTA2_690_UA_V          0x0D
#define ADIE_CODEC_C12_OTA1_570_UA_OTA2_920_UA_V          0x0E
#define ADIE_CODEC_C12_OTA1_760_UA_OTA2_920_UA_V          0x0F

#define ADIE_CODEC_C12_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C13 : CODEC control register 13

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C13_R                                  0x49

#define ADIE_CODEC_C13_INIT_M                             0x3F

/* bits 7:6 : reserved */

#define ADIE_CODEC_C13_HSSD_CAPLESS_MODE_M                0x20
#define ADIE_CODEC_C13_HSSD_CAPLESS_MODE_ENA_V            0x20
#define ADIE_CODEC_C13_HSSD_CAPLESS_MODE_DIS_V            0x00

#define ADIE_CODEC_C13_SEL_AUXDIFF_TO_TXAUX_M             0x10
#define ADIE_CODEC_C13_AUXDIFF_TO_TXAUX_SEL_V             0x10
#define ADIE_CODEC_C13_AUXDIFF_TO_TXAUX_UNSEL_V           0x00

#define ADIE_CODEC_C13_SEL_EAR1_TO_TXAUX_M                0x08
#define ADIE_CODEC_C13_EAR1_TO_TXAUX_SEL_V                0x08
#define ADIE_CODEC_C13_EAR1_TO_TXAUX_UNSEL_V              0x00

#define ADIE_CODEC_C13_SEL_HPH_TO_TXAUX_M                 0x04
#define ADIE_CODEC_C13_HPH_TO_TXAUX_SEL_V                 0x04
#define ADIE_CODEC_C13_HPH_TO_TXAUX_UNSEL_V               0x00

#define ADIE_CODEC_C13_SEL_LOOPBACK_EXT_M                 0x02
#define ADIE_CODEC_C13_LOOPBACK_EXT_ENA_V                 0x02
#define ADIE_CODEC_C13_LOOPBACK_EXT_DIS_V                 0x00

#define ADIE_CODEC_C13_SEL_LOOPBACK_INT_M                 0x01
#define ADIE_CODEC_C13_LOOPBACK_INT_ENA_V                 0x01
#define ADIE_CODEC_C13_LOOPBACK_INT_DIS_V                 0x00

#define ADIE_CODEC_C13_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C14 : CODEC control register 14

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C14_R                                  0x4A

#define ADIE_CODEC_C14_INIT_M                             0x3F

#define ADIE_CODEC_C14_BIQU_L_TO_AUXSE                    0x20
#define ADIE_CODEC_C14_BIQU_R_TO_LINE_R                   0x10
#define ADIE_CODEC_C14_BIQU_L_TO_LINE_L                   0x08
#define ADIE_CODEC_C14_BIQU_R_TO_HPH_R                    0x04
#define ADIE_CODEC_C14_BIQU_L_TO_HPH_L                    0x02
#define ADIE_CODEC_C14_BIQU_L_TO_EAR1                     0x01

#define ADIE_CODEC_C14_SEL_BIQU_L_TO_AUXSE_M              0x20
#define ADIE_CODEC_C14_BIQU_L_TO_AUXSE_SEL_V              0x20
#define ADIE_CODEC_C14_BIQU_L_TO_AUXSE_UNSEL_V            0x00

#define ADIE_CODEC_C14_SEL_BIQU_R_TO_LINEO_R_M            0x10
#define ADIE_CODEC_C14_BIQU_R_TO_LINEO_R_SEL_V            0x10
#define ADIE_CODEC_C14_BIQU_R_TO_LINEO_R_UNSEL_V          0x00

#define ADIE_CODEC_C14_SEL_BIQU_L_TO_LINEO_L_M            0x08
#define ADIE_CODEC_C14_BIQU_L_TO_LINEO_L_SEL_V            0x08
#define ADIE_CODEC_C14_BIQU_L_TO_LINEO_L_UNSEL_V          0x00

#define ADIE_CODEC_C14_SEL_BIQU_R_TO_HPH_R_M              0x04
#define ADIE_CODEC_C14_BIQU_R_TO_HPH_R_SEL_V              0x04
#define ADIE_CODEC_C14_BIQU_R_TO_HPH_R_UNSEL_V            0x00

#define ADIE_CODEC_C14_SEL_BIQU_L_TO_HPH_L_M              0x02
#define ADIE_CODEC_C14_BIQU_L_TO_HPH_L_SEL_V              0x02
#define ADIE_CODEC_C14_BIQU_L_TO_HPH_L_UNSEL_V            0x00

#define ADIE_CODEC_C14_SEL_BIQU_L_TO_EAR1_M               0x01
#define ADIE_CODEC_C14_BIQU_L_TO_EAR1_SEL_V               0x01
#define ADIE_CODEC_C14_BIQU_L_TO_EAR1_UNSEL_V             0x00

#define ADIE_CODEC_C14_INIT_V                             0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_C15 : CODEC control register 15 (0x4B : reserved)

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_C15_R                                  0x4B

#define ADIE_CODEC_C15_RESERVED_V                         0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

CODEC_TST : CODEC test MUX register 1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIE_CODEC_TST_R                                  0x4C

#define ADIE_CODEC_TST_INIT_M                             0xFF

#define ADIE_CODEC_TST_SEL_ATEST1_M                       0xF0
#define ADIE_CODEC_TST_ATEST1_NONE_V                      0x00
#define ADIE_CODEC_TST_ATEST1_RX_BIQU_DAC_HI_L_V          0x10
#define ADIE_CODEC_TST_ATEST1_RX_BIQU_VOCM_OTA1_L_V       0x20
#define ADIE_CODEC_TST_ATEST1_RX_CHD_V1P25U_V             0x30
#define ADIE_CODEC_TST_ATEST1_CDC_BANDGAP_V               0x40
#define ADIE_CODEC_TST_ATEST1_TXADC_VREF_1D8_V_V          0x50
#define ADIE_CODEC_TST_ATEST1_TXADC_VSIGCM_1D25_V_V       0x60
#define ADIE_CODEC_TST_ATEST1_TXADC_VICM_0D9_V_V          0x70
#define ADIE_CODEC_TST_ATEST1_TXADC_V_CASCODE_0D4_V_V     0x80
#define ADIE_CODEC_TST_ATEST1_TXADC_I_P6PTAT_60_UA_V      0x90
#define ADIE_CODEC_TST_ATEST1_RX_BIQU_VOCM_OTA2_L_V       0xA0
#define ADIE_CODEC_TST_ATEST1_TEST_HPH_L_VICM_1D2_V_V     0xB0
#define ADIE_CODEC_TST_ATEST1_TEST_IBG_BIQU_L_100_UA_V    0xC0
#define ADIE_CODEC_TST_ATEST1_TEST_IPTAT_BIQU_L_100_UA_V  0xD0
#define ADIE_CODEC_TST_ATEST1_TST_RXDATA_L_V              0xE0
#define ADIE_CODEC_TST_ATEST1_TXFE_OUTPUT_L_V             0xF0

#define ADIE_CODEC_TST_SEL_ATEST2_M                       0x0F
#define ADIE_CODEC_TST_ATEST2_NONE_V                      0x00
#define ADIE_CODEC_TST_ATEST2_RX_PA_V1P25U_1D25_V_V       0x01
#define ADIE_CODEC_TST_ATEST2_TEST_HPH_R_VICM_1D2_V_V     0x02
#define ADIE_CODEC_TST_ATEST2_TXFE_VCM_R_1D25_V_V         0x03
#define ADIE_CODEC_TST_ATEST2_BIQU_FILTER_DIGCM_L_V       0x04
#define ADIE_CODEC_TST_ATEST2_BIQU_FILTER_DIGCM_R_V       0x05
#define ADIE_CODEC_TST_ATEST2_TXADC_TESTOUT_P_OTA_V       0x06
#define ADIE_CODEC_TST_ATEST2_TXADC_TESTOUT_N_OTA_V       0x07
#define ADIE_CODEC_TST_ATEST2_NONE2_V                     0x08
#define ADIE_CODEC_TST_ATEST2_TST_RXDATA_R_V              0x09
#define ADIE_CODEC_TST_ATEST2_TXADC_BITGEN_OUT_V          0x0A
#define ADIE_CODEC_TST_ATEST2_SEL_CDC_TBUF2_IN_V          0x0B
#define ADIE_CODEC_TST_ATEST2_SEL_CDC_TBUF1_IN_V          0x0C
#define ADIE_CODEC_TST_ATEST2_MUX_BIQU_ON_V               0x0D
#define ADIE_CODEC_TST_ATEST2_MUX_BIQU_OP_V               0x0E
#define ADIE_CODEC_TST_ATEST2_TXFE_OUTPUT_R_V             0x0F

#define ADIE_CODEC_TST_INIT_V                             0x00

#endif /* ADIE_SUPPORT_OLD_CODEC_DEFS */

#endif  /* ADIE_CDCDEFS_H */


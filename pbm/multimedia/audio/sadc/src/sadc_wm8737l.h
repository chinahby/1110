#ifndef SADC_WM8737L_H
#define SADC_WM8737L_H
/*===========================================================================

               Wolfson Stereo ADC register header

DESCRIPTION

  This file contains all the definitions necessary to interface to the
  Wolfson Stereo ADC deivce

REFERENCES

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/driver/sadc/main/latest/src/sadc_wm8737l.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/05    aw     Initial version.

===========================================================================*/

#define WM8737L_CSB_LOW_DEVICE_ADDR  0x1A
#define WM8737L_CSB_HIGH_DEVICE_ADDR 0x1B

/* R0 (00h)
** Left Channel PGA 
*/

#define WM8737L_REG0_ADDR 0x00

#define WM8737L_REG0_LINVOL_M 0x00FF
#define WM8737L_REG0_LINVOL_MUTE_V 0x0000
/* Range from -97db ... +30db, in 0.5 step */
#define WM8737L_REG0_LINVOL(x) ((word)(((x)*2) + 0xC3))

#define WM8737L_REG0_LVU_M 0x0100
#define WM8737L_REG0_LVU_NOT_UPDATE_V 0x0000
#define WM8737L_REG0_LVU_UPDATE_V 0x0100


/* R1 (01h)
** Right Channel PGA
*/

#define WM8737L_REG1_ADDR 0x00

#define WM8737L_REG1_RINVOL_M 0x00FF
#define WM8737L_REG1_RINVOL_MUTE_V 0x0000
/* Range from -97db ... +30db, in 0.5 step */
#define WM8737L_REG1_RINVOL(x) ((word)(((x)*2) + 0xC3))

#define WM8737L_REG1_RVU_M 0x0100
#define WM8737L_REG1_RVU_NOT_UPDATE_V 0x0000
#define WM8737L_REG1_RVU_UPDATE_V 0x0100

/* Analogue Audio Path Control - Left Channel */
#define WM8737L_REG2_ADDR   0x02

#define WM8737L_REG2_LINSEL_M                 0x0180
#define WM8737L_REG2_LINSEL_LINPUT1_V                           0x0000
#define WM8737L_REG2_LINSEL_LINPUT2_V             0x0080
#define WM8737L_REG2_LINSEL_LINPUT3_V                           0x0100
#define WM8737L_REG2_LINSEL_DC_MEASURE_ON_LINPUT1_V             0x0180

#define WM8737L_REG2_LIMCBOOST_M                                0x0060
#define WM8737L_REG2_LIMCBOOST_13DB_V                           0x0000
#define WM8737L_REG2_LIMCBOOST_18DB_V                           0x0020
#define WM8737L_REG2_LIMCBOOST_28DB_V                           0x0040
#define WM8737L_REG2_LIMCBOOST_33DB_V                           0x0060

#define WM8737L_REG2_LMBE_M                                     0x0010
#define WM8737L_REG2_LMBE_DIS_V                                 0x0000
#define WM8737L_REG2_LMBE_ENA_V                                 0x0010


#define WM8737L_REG2_LMZC_M             0x0008
#define WM8737L_REG2_LMZC_IMMEDIATELY_V 0x0000
#define WM8737L_REG2_LMZC_ZERO_CROSSING_V 0x0008

#define WM8737L_REG2_LPZC_M             0x0004
#define WM8737L_REG2_LPZC_IMMEDIATELY_V 0x0000
#define WM8727L_REG2_LPZC_ZERO_CROSSING_V 0x0004

#define WM8737L_REG2_LZCTO_M              0x0003
#define WM8737L_REG2_LZCTO_256_FS_V       0x0000
#define WM8737L_REG2_LZCTO_512_FS_V       0x0001
#define WM8737L_REG2_LZCTO_1024_FS_V      0x0002
#define WM8737L_REG2_LZCTO_2048_FS_V      0x0003

/* R3 (03h)
** Analogue Audio Path Control - Right Channel
*/
#define WM8737L_REG3_ADDR 0x03

#define WM8737L_REG3_RINSEL_M                 0x0180
#define WM8737L_REG3_RINSEL_RINPUT1_V                           0x0000
#define WM8737L_REG3_RINSEL_RINPUT2_V             0x0080
#define WM8737L_REG3_RINSEL_RINPUT3_V                           0x0100
#define WM8737L_REG3_RINSEL_DC_MEASURE_ON_LINPUT1_V             0x0180

#define WM8737L_REG3_RIMCBOOST_M                                0x0060
#define WM8737L_REG3_RIMCBOOST_13DB_V                           0x0000
#define WM8737L_REG3_RIMCBOOST_18DB_V                           0x0020
#define WM8737L_REG3_RIMCBOOST_28DB_V                           0x0040
#define WM8737L_REG3_RIMCBOOST_33DB_V                           0x0060

#define WM8737L_REG3_RMBE_M                                     0x0010
#define WM8737L_REG3_RMBE_DIS_V                                 0x0000
#define WM8737L_REG3_RMBE_ENA_V                                 0x0010

#define WM8737L_REG3_RMZC_M             0x0008
#define WM8737L_REG3_RMZC_IMMEDIATELY_V 0x0000
#define WM8737L_REG3_RMZC_ZERO_CROSSING_V 0x0008

#define WM8737L_REG3_RPZC_M             0x0004
#define WM8737L_REG3_RPZC_IMMEDIATELY_V 0x0000
#define WM8727L_REG3_RPZC_ZERO_CROSSING_V 0x0004

#define WM8737L_REG3_RZCTO_M              0x0003
#define WM8737L_REG3_RZCTO_256_FS_V       0x0000
#define WM8737L_REG3_RZCTO_512_FS_V       0x0001
#define WM8737L_REG3_RZCTO_1024_FS_V      0x0002
#define WM8737L_REG3_RZCTO_2048_FS_V      0x0003

/* R4 (04h)
**
*/
#define WM8737L_REG4_ADDR 0x04
/* R5 (05h)
** ADC Control
*/
#define WM8737L_REG5_ADDR  0x05

#define WM8737L_REG5_MONOMIX_M        0x0180
#define WM8737L_REG5_MONOMIX_STEREO_V 0x0000
#define WM8737L_REG5_MONOMIX_ANALOGUE_MONO_MIX_V 0x0080
#define WM8737L_REG5_MONOMIX_DIGITAL_MONO_MIX_V  0x0100
#define WM8737L_REG5_MONOMIX_RESERVED_V          0x0180

/* ADC THD+N versus Power control */
#define WM8737L_REG5_LP_M 0x0004
#define WM8737L_REG5_LP_THD_PLUS_N_V 0x0000
#define WM8737L_REG5_LP_LOW_POWER_V 0x0004

#define WM8737L_REG5_MONOUT_M 0x0002
#define WM8737L_REG5_MONOUT_STEREO_V 0x0000
#define WM8737L_REG5_MONOUT_LEFT_ON_BOTH_CHANNELS_V 0x0002

/* ADC Digital filter */
#define WM8737L_REG5_ADCHPD_M 0x0001
#define WM8737L_REG5_ADCHPD_DIS_V 0x0001
#define WM8737L_REG5_ADCHPD_ENA_V 0x0000

#define WM8737L_REG5_HPOR_M 0x0010
#define WM8737L_REG5_HPOR_OFFSET_MAINTAINED_V 0x0000
#define WM8737L_REG5_HPOR_OFFSET_CONT_UPDATE_V 0x0001

#define WM8737L_REG5_POLARITY_M 0x0060
#define WM8737L_REG5_POLARITY_NOT_INVERTED_V 0x0000
#define WM8737L_REG5_POLARITY_L_INVERTED_V   0x0020
#define WM8737L_REG5_POLARITY_R_INVERTED_V   0x0040
#define WM8737L_REG5_POLARITY_BOTH_INVERTED_V   0x0060

/* R6 (06h)
** Power Management
*/
#define WM8737L_REG6_ADDR 0x06

#define WM8737L_REG6_VMID_M     0x0100
#define WM8737L_REG6_VMID_ON_V  0x0100
#define WM8737L_REG6_VMID_OFF_V 0x0000

#define WM8737L_REG6_VR_M      0x0080
#define WM8737L_REG6_VR_ON_V   0x0080
#define WM8737L_REG6_VR_OFF_V  0x0000

#define WM8737L_REG6_AI_M      0x0040
#define WM8737L_REG6_AI_ON_V   0x0040
#define WM8737L_REG6_AI_OFF_V  0x0000

#define WM8737L_REG6_PGL_M     0x0020
#define WM8737L_REG6_PGL_ON_V  0x0020
#define WM8737L_REG6_PGL_OFF_V 0x0000

#define WM8737L_REG6_PGR_M      0x0010
#define WM8737L_REG6_PGR_ON_V   0x0010
#define WM8737L_REG6_PGR_OFF_V  0x0000

#define WM8737L_REG6_ADL_M     0x0008
#define WM8737L_REG6_ADL_ON_V  0x0008
#define WM8737L_REG6_ADL_OFF_V 0x0000

#define WM8737L_REG6_ADR_M     0x0004
#define WM8737L_REG6_ADR_ON_V  0x0004
#define WM8737L_REG6_ADR_OFF_V 0x0000

#define WM8737L_REG6_MICBIAS_M 0x0003
#define WM8737L_REG6_MICBIAS_OFF_V 0x0000
#define WM8737L_REG6_MICBIAS_0P75_AVDD_V 0x0001
#define WM8737L_REG6_MICBIAS_0P90_AVDD_V 0x0002
#define WM8737L_REG6_MICBIAS_1P20_AVDD_V 0x0003


/* R7 (07h)
** Digital Audio Interface Format 
*/

#define WM8737L_REG7_ADDR 0x07

#define WM8737L_REG7_FORMAT_M          0x0003
#define WM8737L_REG7_FORMAT_DSP_MODE_V 0x0003
#define WM8737L_REG7_FORMAT_I2S_MODE_V 0x0002
#define WM8737L_REG7_FORMAT_LEFT_JUSTIFIED_V 0x0001
#define WM8737L_REG7_FORMAT_RIGHT_JUSTIFIED_V 0x0000

#define WM8737L_REG7_WL_M 0x000C
#define WM8737L_REG7_WL_32BITS_V 0x000C
#define WM8737L_REG7_WL_24BITS_V 0x0008
#define WM8737L_REG7_WL_20BITS_V 0x0004
#define WM8737L_REG7_WL_16BITS_V 0x0000

#define WM8737L_REG7_LRP_M 0x0010
#define WM8737L_REG7_LRP_DSP_MODE_B_V 0x0010
#define WM8737L_REG7_LRP_DSP_MODE_A_V 0x0000

#define WM8737L_REG7_LRP_INVERT_LRCLK_POLARITY_V 0x0010
#define WM8737L_REG7_LRP_NORMAL_LRCLK_POLARITY_V 0x0000

#define WM8737L_REG7_MS_M 0x0040
#define WM8737L_REG7_MS_MASTER_MODE_V 0x0040
#define WM8737L_REG7_MS_SLAVE_MODE_V  0x0000


#define WM8737L_REG7_SDODIS_M 0x0080
#define WM8737L_REG7_SDODIS_ADCDAT_ENA_V 0x0000
#define WM8737L_REG7_SDODIS_ADCDAT_DIS_V 0x0080

/* R8 (08h)
** Clocking and Sample Rate Control 
*/
#define WM8737L_REG8_ADDR 0x08

#define WM8737L_REG8_CLKDIV2_M 0x0040
#define WM8737L_REG8_CLKDIV2_ENA_V 0x0040
#define WM8737L_REG8_CLKDIV2_DIS_V 0x0000

#define WM8737L_REG8_USB_M 0x0001
#define WM8737L_REG8_USB_ENA_V 0x0001
#define WM8737L_REG8_USB_DIS_V 0x0000


#define WM8737L_REG8_SR_M 0x003E

/* MCLK 12.288MHz */
#define WM8737L_REG8_SR_8000_V (0x04<<1)
#define WM8737L_REG8_SR_12000_V (0x08<<1)
#define WM8737L_REG8_SR_16000_V (0x0A<<1)
#define WM8737L_REG8_SR_24000_V (0x1C<<1)
#define WM8737L_REG8_SR_32000_V (0x0C<<1)
#define WM8737L_REG8_SR_48000_V (0x00<<1)
#define WM8737L_REG8_SR_96000_V (0x14<<1)


#define WM8737L_REG8_AUTO_DETECT_M 0x0080
#define WM8737L_REG8_AUTO_DETECT_OFF_V 0x0000
#define WM8737L_REG8_AUTO_DETECT_ON_V  0x0080



/* R9 (09h)
** Microphone Preamplifiers Control 
*/
#define WM8737L_REG9_ADDR 0x09

#define WM8737L_REG9_RBYPEN_M 0x0008
#define WM8737L_REG9_RBYPEN_OPEN_V  0x0000
#define WM8737L_REG9_RBYPEN_CLOSE_V 0x0008

#define WM8737L_REG9_LBYPEN_M 0x0004
#define WM8737L_REG9_LBYPEN_OPEN_V  0x0000
#define WM8737L_REG9_LBYPEN_CLOSE_V 0x0004

#define WM8737L_REG9_MBCTRL_M      0x0003
#define WM8737L_REG9_MBCTRL_25P_V  0x0000
#define WM8737L_REG9_MBCTRL_50P_V  0x0001
#define WM8737L_REG9_MBCTRL_75P_V  0x0002
#define WM8737L_REG9_MBCTRL_100P_V 0x0003


/* R10 (0Ah)
** DC Measure Control
*/
#define WM8737L_REG10_ADDR 0x0A

#define WM8737L_REG10_RINPUT1_DC_BIAS_M  0x0001
#define WM8737L_REG10_RINPUT1_DC_BIAS_DIS_V 0x0000
#define WM8737L_REG10_RINPUT1_DC_BIAS_ENA_V 0x0001

#define WM8737L_REG10_LINPUT1_DC_BIAS_M 0x0002
#define WM8737L_REG10_LINPUT1_DC_BIAS_DIS_V 0x0000
#define WM8737L_REG10_LINPUT1_DC_BIAS_ENA_V 0x0002

/* R11 (0Bh)
** Noise Gate Control 
*/
#define WM8737L_REG11_ADDR 0x0B

#define WM8737L_REG11_NGAT_M 0x0001
#define WM8737L_REG11_NGAT_ENA_V 0x0001
#define WM8737L_REG11_NGAT_DIS_V 0x0000

#define WM8737L_REG11_NGTH_M        (0x07<<2)
#define WM8737L_REG11_NGTH_N78DBF_V (0x00<<2)
#define WM8737L_REG11_NGTH_N72DBF_V (0x01<<2)
#define WM8737L_REG11_NGTH_N66DBF_V (0x02<<2)
#define WM8737L_REG11_NGTH_N60DBF_V (0x03<<2)
#define WM8737L_REG11_NGTH_N54DBF_V (0x04<<2)
#define WM8737L_REG11_NGTH_N48DBF_V (0x05<<2)
#define WM8737L_REG11_NGTH_N42DBF_V (0x06<<2)
#define WM8737L_REG11_NGTH_N30DBF_V (0x07<<2)


/* R12 (0Ch)
** ALC Control 1
*/
#define WM8737L_REG12_ADDR            0x0C

#define WM8737L_REG12_DEFAULT_V       0x007C

#define WM8737L_REG12_ALCSEL_M        0x0180
#define WM8737L_REG12_ALCSEL_OFF_V    0x0000
#define WM8737L_REG12_ALCSEL_RIGHT_V  0x0080
#define WM8737L_REG12_ALCSEL_LEFT_V   0x0100
#define WM8737L_REG12_ALCSEL_STEREO_V 0x0180

#define WM8737L_REG12_MAXGAIN_M       0x0070
#define WM8737L_REG12_MAXGAIN_P30DB_V 0x0070
#define WM8737L_REG12_MAXGAIN_P24DB_V 0x0060
#define WM8737L_REG12_MAXGAIN_P18DB_V 0x0050
#define WM8737L_REG12_MAXGAIN_P12DB_V 0x0040
#define WM8737L_REG12_MAXGAIN_P6DB_V  0x0030
#define WM8737L_REG12_MAXGAIN_0DB_V   0x0020
#define WM8737L_REG12_MAXGAIN_N6DB_V  0x0010
#define WM8737L_REG12_MAXGAIN_N12DB_V 0x0000

#define WM8737L_REG12_ALCL_M          0x000F
#define WM8737L_REG12_ALCL_N18DB_FS_V 0x0000
#define WM8737L_REG12_ALCL_N17DB_FS_V 0x0001
#define WM8737L_REG12_ALCL_N16DB_FS_V 0x0002
#define WM8737L_REG12_ALCL_N15DB_FS_V 0x0003
#define WM8737L_REG12_ALCL_N14DB_FS_V 0x0004
#define WM8737L_REG12_ALCL_N13DB_FS_V 0x0005
#define WM8737L_REG12_ALCL_N12DB_FS_V 0x0006
#define WM8737L_REG12_ALCL_N11DB_FS_V 0x0007
#define WM8737L_REG12_ALCL_N10DB_FS_V 0x0008
#define WM8737L_REG12_ALCL_N9DB_FS_V  0x0009
#define WM8737L_REG12_ALCL_N8DB_FS_V  0x000A
#define WM8737L_REG12_ALCL_N7DB_FS_V  0x000B
#define WM8737L_REG12_ALCL_N6DB_FS_V  0x000C
#define WM8737L_REG12_ALCL_N5DB_FS_V  0x000D
#define WM8737L_REG12_ALCL_N4DB_FS_V  0x000E
#define WM8737L_REG12_ALCL_N3DB_FS_V  0x000F

/* R13 (0Dh)
** ALC Control 2
*/
#define WM8737L_REG13_ADDR                 0x0D

#define WM8737L_REG13_DEFAULT_V            0x0000

#define WM8737L_REG13_HLD_M                0x000F
#define WM8737L_REG13_HLD_2P67MS_MULT_V(x) ((x) & 0x000F)

#define WM8737L_REG13_ALCZCE_M             0x0010
#define WM8737L_REG13_ALCZCE_DIS_V         0x0000
#define WM8737L_REG13_ALCZCE_ENA_V         0x0010

/* R14 (0Eh)
** ALC Control 3
*/
#define WM8737L_REG14_ADDR 0x0E

#define WM8737L_REG14_DEFAULT_V 0x0032

#define WM8737L_REG14_ATK_M 0x000F
#define WM8737L_REG14_ATK_8P4MS_MULT_V(x) ((x) & 0x000F)
/* 1010 or higher = 8.6 */

#define WM8737L_REG14_DCY_M 0x00F0
#define WM8737L_REG14_DCY_33P6MS_MULT_V(x) (((x) & 0x000F) << 4)
/* 1010 or higher = 34.41 */

/* R15 (0Fh)
** Reset
*/
#define WM8737L_REG15_ADDR 0x0F

#endif /* SADC_WM8737L_H */

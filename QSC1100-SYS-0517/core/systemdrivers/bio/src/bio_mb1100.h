#ifndef BIO_MB11XX_H
#define BIO_MB11XX_H
/*===========================================================================

            S U R F   Q S C 1 1 0 0   B I O   H E A D E R   F I L E

DESCRIPTION
  This file contains definitions of the bits of the SURF QSC11XX main board for
  the BIO services.  These bits are the control lines which can be read
  or written using the BIO services.

  Copyright (c) 2004-2007 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/bio/bio_mb1100.h#1 $
$DateTime: 2008/02/25 14:50:57 $
$Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/07   vph     Ported for QSC11XX from QSC6055
01/22/07    cr     Port TCXO_EN.
10/14/04   ts      Ported for MSM6800 from MSM6550
09/02/04   jkl     Updated FFA settings.
08/30/04   jkl     Updated HEADSET detection.
08/17/04   trc     Added Slingshot GPIO support
08/11/04   jkl     Clean up code.
04/15/04   bmg     Ported for MSM6550 from MSM6500
===========================================================================*/
#include "bio_mb11xx_port.h"
#ifndef _ARM_ASM_
 #include "tlmm.h"
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO2 GROUP 0 (0:11)
---------------------------------------------------------------------------*/
#define BIO_GPIO_0_REG        GPIO2_REG_0 /* GPIO  0: KYPD_9  (SURF, FFA)     */
#define BIO_GPIO_0_M          0x00000001  /*                                  */
#define BIO_GPIO_0_HI_V       0x00000001
#define BIO_GPIO_0_LO_V       0x00000000

#define BIO_GPIO_1_REG        GPIO2_REG_0 /* GPIO  1: KYPD_11 (SURF, FFA)     */
#define BIO_GPIO_1_M          0x00000002  /*                                  */
#define BIO_GPIO_1_HI_V       0x00000002
#define BIO_GPIO_1_LO_V       0x00000000

#define BIO_GPIO_2_REG        GPIO2_REG_0 /* GPIO  2: KYPD_15 (SURF, FFA)     */
#define BIO_GPIO_2_M          0x00000004  /*                                  */
#define BIO_GPIO_2_HI_V       0x00000004
#define BIO_GPIO_2_CLK_LO_V   0x00000000

#define BIO_GPIO_3_REG        GPIO2_REG_0 /* GPIO  3: KYPD_17 (SURF, FFA)     */
#define BIO_GPIO_3_M          0x00000008  /*                                  */
#define BIO_GPIO_3_HI_V       0x00000008  /*                                  */
#define BIO_GPIO_3_LO_V       0x00000000

#define BIO_GPIO_4_REG        GPIO2_REG_0 /* GPIO  4: KYPD_MEMO (SURF, FFA)   */
#define BIO_GPIO_4_M          0x00000010  /*                                  */
#define BIO_GPIO_4_HI_V       0x00000010  /*                                  */
#define BIO_GPIO_4_LO_V       0x00000000

#define BIO_GPIO_5_REG        GPIO2_REG_0 /* GPIO  5: KEYSENSE_N[4]           */
#define BIO_GPIO_5_M          0x00000020  /*                                  */
#define BIO_GPIO_5_HI_V       0x00000020  /*                                  */
#define BIO_GPIO_5_LO_V       0x00000000

#define BIO_GPIO_6_REG        GPIO2_REG_0 /* GPIO  6: KEYSENSE_N[3]           */
#define BIO_GPIO_6_M          0x00000040  /*                                  */
#define BIO_GPIO_6_HI_V       0x00000040  /*                                  */
#define BIO_GPIO_6_LO_V       0x00000000

#define BIO_GPIO_7_REG        GPIO2_REG_0 /* GPIO  7: KEYSENSE_N[2]           */
#define BIO_GPIO_7_M          0x00000080  /*                                  */
#define BIO_GPIO_7_HI_V       0x00000080
#define BIO_GPIO_7_LO_V       0x00000000

#define BIO_GPIO_8_REG        GPIO2_REG_0 /* GPIO  8: KEYSENSE_N[1]           */
#define BIO_GPIO_8_M          0x00000100  /*                                  */
#define BIO_GPIO_8_HI_V       0x00000100
#define BIO_GPIO_8_LO_V       0x00000000

#define BIO_GPIO_9_REG        GPIO2_REG_0 /* GPIO  9: KEYSENSE_N[0]           */
#define BIO_GPIO_9_M          0x00000200  /*                                  */
#define BIO_GPIO_9_HI_V       0x00000200  /*                                  */
#define BIO_GPIO_9_LO_V       0x00000000

#define BIO_GPIO_10_REG       GPIO2_REG_0 /* GPIO 10:                         */
#define BIO_GPIO_10_M         0x00000400  /* Muxed:   BACKLIGHT_EN            */
#define BIO_GPIO_10_HI_V      0x00000400  /*                                  */
#define BIO_GPIO_10_LO_V      0x00000000

#define BIO_GPIO_11_REG       GPIO2_REG_0 /* GPIO 11: HEADSET_DET_N           */
#define BIO_GPIO_11_M         0x00000800  /*                                  */
#define BIO_GPIO_11_HI_V      0x00000800  /*                                  */
#define BIO_GPIO_11_LO_V      0x00000000

/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO GROUP 0 (12)
---------------------------------------------------------------------------*/
#define BIO_GPIO_12_REG       GPIO_REG_0  /* GPIO 12: GSBI0[0]                */
#define BIO_GPIO_12_M         0x00001000  /* Muxed: UIM_CLK                   */
#define BIO_GPIO_12_HI_V      0x00001000
#define BIO_GPIO_12_LO_V      0x00000000

/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO2 GROUP 0 (13)
---------------------------------------------------------------------------*/
#define BIO_GPIO_13_REG       GPIO_REG_0  /* GPIO 13: BT_PWR_EN               */
#define BIO_GPIO_13_M         0x00002000  /*                                  */
#define BIO_GPIO_13_HI_V      0x00002000
#define BIO_GPIO_13_LO_V      0x00000000

/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO GROUP 0 (14:30)
---------------------------------------------------------------------------*/
#define BIO_GPIO_14_REG       GPIO_REG_0  /* GPIO 14: GSBI0[2]                */
#define BIO_GPIO_14_M         0x00004000  /* Muxed: UIM_RESET                 */
#define BIO_GPIO_14_HI_V      0x00004000
#define BIO_GPIO_14_LO_V      0x00000000

#define BIO_GPIO_15_REG       GPIO_REG_0  /* GPIO 15: GSBI0[3]                */
#define BIO_GPIO_15_M         0x00008000  /* Muxed: UIM_DAT                   */
#define BIO_GPIO_15_HI_V      0x00008000
#define BIO_GPIO_15_LO_V      0x00000000

#define BIO_GPIO_16_REG       GPIO_REG_0  /* GPIO 16: GSBI1[0]                */
#define BIO_GPIO_16_M         0x00010000  /* Muxed: SPI_CS_N                  */
#define BIO_GPIO_16_HI_V      0x00010000  /* Muxed: UART1_RFR_N               */
#define BIO_GPIO_16_LO_V      0x00000000

#define BIO_GPIO_17_REG       GPIO_REG_0  /* GPIO 17: GSBI1[1]                */
#define BIO_GPIO_17_M         0x00020000  /* Muxed: SPI_CLK                   */
#define BIO_GPIO_17_HI_V      0x00020000  /* Muxed: UART1_CTS_N               */
#define BIO_GPIO_17_LO_V      0x00000000

#define BIO_GPIO_18_REG       GPIO_REG_0  /* GPIO 18: GSBI1[2]                */
#define BIO_GPIO_18_M         0x00040000  /* Muxed: SPI_DATA_I                */
#define BIO_GPIO_18_HI_V      0x00040000  /* Muxed: UART1_RX_DAT              */
#define BIO_GPIO_18_LO_V      0x00000000

#define BIO_GPIO_19_REG       GPIO_REG_0  /* GPIO 19: GSBI1[3]                */
#define BIO_GPIO_19_M         0x00080000  /* Muxed: SPI_DATA_O                */
#define BIO_GPIO_19_HI_V      0x00080000  /* Muxed: UART1_TX_DAT              */
#define BIO_GPIO_19_LO_V      0x00000000

#define BIO_GPIO_20_REG       GPIO_REG_0  /* GPIO 20: GSBI2[0]                */
#define BIO_GPIO_20_M         0x00100000  /* Muxed: AUX_PCM_CLK               */
#define BIO_GPIO_20_HI_V      0x00100000  /*                                  */
#define BIO_GPIO_20_LO_V      0x00000000

#define BIO_GPIO_21_REG       GPIO_REG_0  /* GPIO 21: GSBI2[1]                */
#define BIO_GPIO_21_M         0x00200000  /* Muxed: AUX_PCM_SYNC              */
#define BIO_GPIO_21_HI_V      0x00200000  /*                                  */
#define BIO_GPIO_21_LO_V      0x00000000  /*                                  */

#define BIO_GPIO_22_REG       GPIO_REG_0  /* GPIO 22: GSBI2[2]                */
#define BIO_GPIO_22_M         0x00400000  /* Muxed: AUX_PCM_DIN               */
#define BIO_GPIO_22_HI_V      0x00400000  /*                                  */
#define BIO_GPIO_22_LO_V      0x00000000

#define BIO_GPIO_23_REG       GPIO_REG_0  /* GPIO 23: GSBI2[3]                */
#define BIO_GPIO_23_M         0x00800000  /* Muxed: AUX_PCM_DOUT              */
#define BIO_GPIO_23_HI_V      0x00800000
#define BIO_GPIO_23_LO_V      0x00000000

#define BIO_GPIO_24_REG       GPIO_REG_0  /* GPIO 24: SDCC_CLK                */
#define BIO_GPIO_24_M         0x01000000  /*                                  */
#define BIO_GPIO_24_HI_V      0x01000000  /*                                  */
#define BIO_GPIO_24_LO_V      0x00000000

#define BIO_GPIO_25_REG       GPIO_REG_0  /* GPIO 25: SDCC_DATA[0]            */
#define BIO_GPIO_25_M         0x02000000  /*                                  */
#define BIO_GPIO_25_HI_V      0x02000000  /*                                  */
#define BIO_GPIO_25_LO_V      0x00000000

#define BIO_GPIO_26_REG       GPIO_REG_0  /* GPIO 26: SDCC_DATA[1]            */
#define BIO_GPIO_26_M         0x04000000  /*                                  */
#define BIO_GPIO_26_HI_V      0x04000000  /*                                  */
#define BIO_GPIO_26_LO_V      0x00000000

#define BIO_GPIO_27_REG       GPIO_REG_0  /* GPIO 27: SDCC_DATA[2]            */
#define BIO_GPIO_27_M         0x08000000
#define BIO_GPIO_27_HI_V      0x08000000
#define BIO_GPIO_27_LO_V      0x00000000

#define BIO_GPIO_28_REG       GPIO_REG_0  /* GPIO 28: SDCC_DATA[3]            */
#define BIO_GPIO_28_M         0x10000000
#define BIO_GPIO_28_HI_V      0x10000000
#define BIO_GPIO_28_LO_V      0x00000000

#define BIO_GPIO_29_REG       GPIO_REG_0  /* GPIO 29: SDCC_CMD                */
#define BIO_GPIO_29_M         0x20000000  /*                                  */
#define BIO_GPIO_29_HI_V      0x20000000  /*                                  */
#define BIO_GPIO_29_LO_V      0x00000000  /*                                  */

#define BIO_GPIO_30_REG       GPIO_REG_0  /* GPIO 30: FM_INTX                 */
#define BIO_GPIO_30_M         0x40000000  /*                                  */
#define BIO_GPIO_30_HI_V      0x40000000  /*                                  */
#define BIO_GPIO_30_LO_V      0x00000000

/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO2 GROUP 0 (31)
---------------------------------------------------------------------------*/
#define BIO_GPIO_31_REG       GPIO2_REG_0 /* GPIO 31: BAND_SEL1               */
#define BIO_GPIO_31_M         0x80000000  /*                                  */
#define BIO_GPIO_31_HI_V      0x80000000  /*                                  */
#define BIO_GPIO_31_LO_V      0x00000000

/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO2 GROUP 1 (32:33)
---------------------------------------------------------------------------*/
#define BIO_GPIO_32_REG       GPIO2_REG_1 /* GPIO 32: BAND_SEL0               */
#define BIO_GPIO_32_M         0x00000001  /*                                  */
#define BIO_GPIO_32_HI_V      0x00000001  /*                                  */
#define BIO_GPIO_32_LO_V      0x00000000

#define BIO_GPIO_33_REG       GPIO2_REG_1 /* GPIO 33: EBI_A_D[24]             */
#define BIO_GPIO_33_M         0x00000002
#define BIO_GPIO_33_HI_V      0x00000002
#define BIO_GPIO_33_LO_V      0x00000000

/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO GROUP 1 (34:41)
---------------------------------------------------------------------------*/
#define BIO_GPIO_34_REG       GPIO_REG_1  /* GPIO 34: EBI_CS_N[2]             */
#define BIO_GPIO_34_M         0x00000004  /*                                  */
#define BIO_GPIO_34_HI_V      0x00000004
#define BIO_GPIO_34_LO_V      0x00000000

#define BIO_GPIO_35_REG       GPIO_REG_1  /* GPIO 35: PA_ON[0]                */
#define BIO_GPIO_35_M         0x00000008  /*                                  */
#define BIO_GPIO_35_HI_V      0x00000008  /*                                  */
#define BIO_GPIO_35_LO_V      0x00000000

#define BIO_GPIO_36_REG       GPIO_REG_1  /* GPIO 36: PA_ON[1]                */
#define BIO_GPIO_36_M         0x00000010  /*                                  */
#define BIO_GPIO_36_HI_V      0x00000010
#define BIO_GPIO_36_LO_V      0x00000000

#define BIO_GPIO_37_REG       GPIO_REG_1  /* GPIO 37: PA_R0                   */
#define BIO_GPIO_37_M         0x00000020  /*                                  */
#define BIO_GPIO_37_HI_V      0x00000020  /*                                  */
#define BIO_GPIO_37_LO_V      0x00000000

#define BIO_GPIO_38_REG       GPIO_REG_1  /* GPIO 38: PA_ON_2                 */
#define BIO_GPIO_38_M         0x00000040  /*                                  */
#define BIO_GPIO_38_HI_V      0x00000040  /*                                  */
#define BIO_GPIO_38_LO_V      0x00000000

#define BIO_GPIO_39_REG       GPIO_REG_1  /* GPIO 39: I2C_SDA                 */
#define BIO_GPIO_39_M         0x00000080  /*                                  */
#define BIO_GPIO_39_HI_V      0x00000080  /*                                  */
#define BIO_GPIO_39_LO_V      0x00000000

#define BIO_GPIO_40_REG       GPIO_REG_1  /* GPIO 40: I2C_SCL                 */
#define BIO_GPIO_40_M         0x00000100  /*                                  */
#define BIO_GPIO_40_HI_V      0x00000100  /*                                  */
#define BIO_GPIO_40_LO_V      0x00000000

#define BIO_GPIO_41_REG       GPIO_REG_1  /* GPIO 41: CAMIF_DISABLE           */
#define BIO_GPIO_41_M         0x00000200  /*                                  */
#define BIO_GPIO_41_HI_V      0x00000200  /*                                  */
#define BIO_GPIO_41_LO_V      0x00000000

/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO GROUP 1 (42:52)
---------------------------------------------------------------------------*/
#define BIO_GPIO_42_REG       GPIO2_REG_1  /* GPIO 42: CAMIF_DATA[0]           */
#define BIO_GPIO_42_M         0x00000400
#define BIO_GPIO_42_HI_V      0x00000400
#define BIO_GPIO_42_LO_V      0x00000000

#define BIO_GPIO_43_REG       GPIO2_REG_1  /* GPIO 43: CAMIF_DATA[1]           */
#define BIO_GPIO_43_M         0x00000800
#define BIO_GPIO_43_HI_V      0x00000800
#define BIO_GPIO_43_LO_V      0x00000000

#define BIO_GPIO_44_REG       GPIO2_REG_1  /* GPIO 44: CAMIF_DATA[2]           */
#define BIO_GPIO_44_M         0x00001000
#define BIO_GPIO_44_HI_V      0x00001000
#define BIO_GPIO_44_LO_V      0x00000000

#define BIO_GPIO_45_REG       GPIO2_REG_1  /* GPIO 45: CAMIF_DATA[3]           */
#define BIO_GPIO_45_M         0x00002000   /*                                  */
#define BIO_GPIO_45_HI_V      0x00002000
#define BIO_GPIO_45_LO_V      0x00000000

#define BIO_GPIO_46_REG       GPIO2_REG_1  /* GPIO 46: CAMIF_DATA[4]           */
#define BIO_GPIO_46_M         0x00004000   /*                                  */
#define BIO_GPIO_46_HI_V      0x00004000   /*                                  */
#define BIO_GPIO_46_LO_V      0x00000000

#define BIO_GPIO_47_REG       GPIO2_REG_1  /* GPIO 47: CAMIF_DATA[5]           */
#define BIO_GPIO_47_M         0x00008000   /*                                  */
#define BIO_GPIO_47_HI_V      0x00008000   /*                                  */
#define BIO_GPIO_47_LO_V      0x00000000

#define BIO_GPIO_48_REG       GPIO2_REG_1  /* GPIO 48: CAMIF_DATA[6]           */
#define BIO_GPIO_48_M         0x00010000   /*                                  */
#define BIO_GPIO_48_HI_V      0x00010000   /*                                  */
#define BIO_GPIO_48_LO_V      0x00000000

#define BIO_GPIO_49_REG       GPIO2_REG_1  /* GPIO 49: CAMIF_DATA[7]           */
#define BIO_GPIO_49_M         0x00020000   /*                                  */
#define BIO_GPIO_49_HI_V      0x00020000   /*                                  */
#define BIO_GPIO_49_LO_V      0x00000000

#define BIO_GPIO_50_REG       GPIO2_REG_1  /* GPIO 50: CAMIF_PCLK              */
#define BIO_GPIO_50_M         0x00040000   /*                                  */
#define BIO_GPIO_50_HI_V      0x00040000   /*                                  */
#define BIO_GPIO_50_LO_V      0x00000000

#define BIO_GPIO_51_REG       GPIO2_REG_1  /* GPIO 51: CAMIF_HSYNC             */
#define BIO_GPIO_51_M         0x00080000   /*                                  */
#define BIO_GPIO_51_HI_V      0x00080000   /*                                  */
#define BIO_GPIO_51_LO_V      0x00000000

#define BIO_GPIO_52_REG       GPIO2_REG_1  /* GPIO 52: CAMIF_VSYNC             */
#define BIO_GPIO_52_M         0x00100000   /*                                  */
#define BIO_GPIO_52_HI_V      0x00100000   /*                                  */
#define BIO_GPIO_52_LO_V      0x00000000

/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO GROUP 1 (53)
---------------------------------------------------------------------------*/
#define BIO_GPIO_53_REG       GPIO_REG_1   /* GPIO 53: CAMIF_MCLK              */
#define BIO_GPIO_53_M         0x00200000   /*                                  */
#define BIO_GPIO_53_HI_V      0x00200000   /*                                  */
#define BIO_GPIO_53_LO_V      0x00000000

/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO2 GROUP 1 (54)
---------------------------------------------------------------------------*/
#define BIO_GPIO_54_REG       GPIO2_REG_1  /* GPIO 54: MDP_VSYNC_P             */
#define BIO_GPIO_54_M         0x00400000   /*                                  */
#define BIO_GPIO_54_HI_V      0x00400000   /*                                  */
#define BIO_GPIO_54_LO_V      0x00000000

/* --------------------------------------------------------------------------
                BIT I/O MASKS FOR GPIO2 GROUP 1 (55:62)
---------------------------------------------------------------------------*/
#define BIO_GPIO_55_REG       GPIO_REG_1   /* GPIO 55: EBI1_A_D[16]            */
#define BIO_GPIO_55_M         0x00800000   /*                                  */
#define BIO_GPIO_55_HI_V      0x00800000   /*                                  */
#define BIO_GPIO_55_LO_V      0x00000000

#define BIO_GPIO_56_REG       GPIO_REG_1   /* GPIO 56: EBI1_A_D[17]            */
#define BIO_GPIO_56_M         0x01000000   /*                                  */
#define BIO_GPIO_56_HI_V      0x01000000   /*                                  */
#define BIO_GPIO_56_LO_V      0x00000000

#define BIO_GPIO_57_REG       GPIO_REG_1   /* GPIO 57: EBI1_A_D[18]            */
#define BIO_GPIO_57_M         0x02000000   /*                                  */
#define BIO_GPIO_57_HI_V      0x02000000   /*                                  */
#define BIO_GPIO_57_LO_V      0x00000000

#define BIO_GPIO_58_REG       GPIO_REG_1   /* GPIO 58: EBI1_A_D[19]            */
#define BIO_GPIO_58_M         0x04000000   /*                                  */
#define BIO_GPIO_58_HI_V      0x04000000   /*                                  */
#define BIO_GPIO_58_LO_V      0x00000000

#define BIO_GPIO_59_REG       GPIO_REG_1   /* GPIO 59: EBI1_A_D[20]            */
#define BIO_GPIO_59_M         0x08000000   /*                                  */
#define BIO_GPIO_59_HI_V      0x08000000   /*                                  */
#define BIO_GPIO_59_LO_V      0x00000000

#define BIO_GPIO_60_REG       GPIO_REG_1   /* GPIO 60: EBI1_A_D[21]            */
#define BIO_GPIO_60_M         0x10000000   /*                                  */
#define BIO_GPIO_60_HI_V      0x10000000   /*                                  */
#define BIO_GPIO_60_LO_V      0x00000000

#define BIO_GPIO_61_REG       GPIO_REG_1   /* GPIO 61: EBI1_A_D[22]            */
#define BIO_GPIO_61_M         0x20000000   /*                                  */
#define BIO_GPIO_61_HI_V      0x20000000   /*                                  */
#define BIO_GPIO_61_LO_V      0x00000000

#define BIO_GPIO_62_REG       GPIO_REG_1   /* GPIO 62: EBI1_A_D[23]            */
#define BIO_GPIO_62_M         0x40000000   /*                                  */
#define BIO_GPIO_62_HI_V      0x40000000   /*                                  */
#define BIO_GPIO_62_LO_V      0x00000000

/* --------------------------------------------------------------------------
                        UI-RELATED GPIO MACROS
---------------------------------------------------------------------------*/
/*
** Headset Detection Interrupt 
*/
#define BIO_HEADSET_REG       BIO_GPIO_11_REG
#define BIO_HEADSET_M         BIO_GPIO_11_M
#define BIO_HEADSET_ON_V      BIO_GPIO_11_LO_V
#define BIO_HEADSET_OFF_V     BIO_GPIO_11_HI_V

/* 
** Backlight 
*/
#define BIO_BACKLIGHT_REG     BIO_GPIO_10_REG
#define BIO_BACKLIGHT_M       BIO_GPIO_10_M
#define BIO_BACKLIGHT_ON_V    BIO_GPIO_10_HI_V
#define BIO_BACKLIGHT_OFF_V   BIO_GPIO_10_LO_V

/* --------------------------------------------------------------------------
                 BIT I/O MASKS FOR GPIO_KEYSENSE
---------------------------------------------------------------------------*/

/* Keysense0_n  */
#define BIO_KYPD_1_M          BIO_GPIO_9_M
#define BIO_KYPD_1_HI_V       BIO_GPIO_9_HI_V
#define BIO_KYPD_1_LO_V       BIO_GPIO_9_LO_V

/* Keysense1_n  */
#define BIO_KYPD_3_M          BIO_GPIO_8_M
#define BIO_KYPD_3_HI_V       BIO_GPIO_8_HI_V
#define BIO_KYPD_3_LO_V       BIO_GPIO_8_LO_V

/* Keysense2_n  */
#define BIO_KYPD_5_M          BIO_GPIO_7_M
#define BIO_KYPD_5_HI_V       BIO_GPIO_7_HI_V
#define BIO_KYPD_5_LO_V       BIO_GPIO_7_LO_V

/* Keysense3_n  */
#define BIO_KYPD_7_M          BIO_GPIO_6_M
#define BIO_KYPD_7_HI_V       BIO_GPIO_6_HI_V
#define BIO_KYPD_7_LO_V       BIO_GPIO_6_LO_V

/* Keysense4_n  */
#define BIO_KYPD_19_M         BIO_GPIO_5_M
#define BIO_KYPD_19_HI_V      BIO_GPIO_5_HI_V 
#define BIO_KYPD_19_LO_V      BIO_GPIO_5_LO_V 

#define BIO_KYPD_MEMO_REG     BIO_GPIO_4_REG
#define BIO_KYPD_MEMO_M       BIO_GPIO_4_M  
#define BIO_KYPD_MEMO_HI_V    BIO_GPIO_4_HI_V  
#define BIO_KYPD_MEMO_LO_V    BIO_GPIO_4_LO_V  

#define BIO_KYPD_9_M          BIO_GPIO_0_M
#define BIO_KYPD_9_HI_V       BIO_GPIO_0_HI_V
#define BIO_KYPD_9_LO_V       BIO_GPIO_0_LO_V

#define BIO_KYPD_11_M         BIO_GPIO_1_M
#define BIO_KYPD_11_HI_V      BIO_GPIO_1_HI_V
#define BIO_KYPD_11_LO_V      BIO_GPIO_1_LO_V

#define BIO_KYPD_15_M         BIO_GPIO_2_M
#define BIO_KYPD_15_HI_V      BIO_GPIO_2_HI_V
#define BIO_KYPD_15_LO_V      BIO_GPIO_2_LO_V

#define BIO_KYPD_17_M         BIO_GPIO_3_M
#define BIO_KYPD_17_HI_V      BIO_GPIO_3_HI_V
#define BIO_KYPD_17_LO_V      BIO_GPIO_3_LO_V

/* --------------------------------------------------------------------------
                      UIM-RELATED GPIO MACROS
---------------------------------------------------------------------------*/
#define BIO_UIM_RST_REG       BIO_GPIO_14_REG
#define BIO_UIM_RST_M         BIO_GPIO_14_M
#define BIO_UIM_RST_HIGH_V    BIO_GPIO_14_HI_V
#define BIO_UIM_RST_LOW_V     BIO_GPIO_14_LO_V

#endif /* BIO_MB11XX_H */

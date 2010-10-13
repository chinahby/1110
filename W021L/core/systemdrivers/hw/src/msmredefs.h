#ifndef MSMREDEFS_H
#define MSMREDEFS_H
/*===========================================================================

           MSM510X REGISTER REDEFINITION HEADER FILE

DESCRIPTION
  This file contains MSM510X register redefinitions.

Copyright (c) 1998-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/msm6050redefs.h_v   1.6   20 Mar 2002 17:58:20   dlee  $
$Header: //depot/asic/qsc1100/drivers/hw/msmredefs.h#3 $ $DateTime: 2008/03/10 17:32:52 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/06/06   gmy     Now using virtual LCD-related addrs for T_QSC60X5
04/28/05   jrp     Changed MSM_END to a valid virtual address.
06/07/04   ap      Modified port names for LCDs and added support for FFAs.
05/04/04   bmg     Added FFA support for H1 QCIF
04/29/04   AP      Use correct LCD address for QVGA and QCIF.
10/21/03   dna     Removed some UART2 and UART3 redefinitions that were
                   breaking RUIM.  Really this entire file should be removed,
                   but there is not time for that right now.
05/01/03   ht      Changed the LCD base address.
06/26/02   dle     Modified LCD_DATA_WH.
03/13/02   dle     Added defines for *_PUPD_N_* and LCD_*_WH.
02/11/02   cc      Added Macro define of MSMU2_SIM_CFG.
                   Added defines for UART3.
02/11/02   rm      Added searcher1 and searcher2 aliases.
10/10/01    ro     Removed outdated voc and audio definitions.
01/07/01   ejv     Updated DMOD_R_FRSSI for MSM5100.
01/18/01   dsb     Added PS_HOLD registers from msm510xbits.h.  #ifdef'd
                   MOD_SCH_ENC_DATA_WH away for T_MSM_5100.
11/13/00   ejv     Modified DMOD_POS_PW and DMOD_POLARITY for MSM5100.
11/07/00   vlc     MSM5100 baseline based on msm5105redefs.h r1.1.
10/23/00   rmd     Initial Revision
===========================================================================*/

/* Define a common location for all non-mapped register's */
#define MSM3_DUMMY                             MSM_END  /* See def below */
#ifdef T_MSM6800B
#define MSM_END                                EBI2_GP1_BASE
#else
#define MSM_END                                GP1_BASE
#endif
/* This is just some unused address in Jaguar */

/*===========================================================================

                        START DMOD.H ALIAS

===========================================================================*/


/*---------------------------------------------------------------------------
                          Write Registers
---------------------------------------------------------------------------*/

#define DMOD_W_SRCH_CTL       SRCH_CTL_WB
#define DMOD_ETE_0            SRCH_TH_ENERGY_LOW_WB     /* Bits 7:0 */
#define DMOD_ETE_1            SRCH_TH_ENERGY_HIGH_WB    /* Bits 15:8 */
#define DMOD_SIT              SRCH_INTG_TIME_WB         /* Bits 7:0 */
#define DMOD_ETT              SRCH_TH_TIME_WB           /* Bits 7:0 */
#define DMOD_SBO_0            SRCH_OFFSET_LOW_WB        /* Bits 7:0  */
#define DMOD_SBO_1            SRCH_OFFSET_HIGH_WB       /* Bits 14:8 */
#define DMOD_SPNIM_0          SRCH_MASK_I_LOW_WB        /* Bits 7:0 */
#define DMOD_SPNIM_1          SRCH_MASK_I_HIGH_WB       /* Bits 14:8 */
#define DMOD_SPNQM_0          SRCH_MASK_Q_LOW_WB        /* Bits 7:0 */
#define DMOD_SPNQM_1          SRCH_MASK_Q_HIGH_WB       /* Bits 14:0 */
#define DMOD_SS_0             SRCH_SLEW_LOW_WB          /* Bits 7:0 */
#define DMOD_SS_1             SRCH_SLEW_MID_WB          /* Bits 15:0 */
#define DMOD_SS_2             SRCH_SLEW_HIGH_WB         /* Bits 17:16 */
#define DMOD_SAP              SRCH_ACC_PASS_WB
#define DMOD_SN_0             SRCH_NUM_LOW_WB
#define DMOD_SN_1             SRCH_NUM_HIGH_WB          /* MSM23 Only */
#define DMOD_PC_F0_CELL_EN    FFE_POWER_CTL_F0_CELLN_EN_WB
#define DMOD_PC_F1_CELL_EN    FFE_POWER_CTL_F1_CELLN_EN_WB
#define DMOD_PC_F2_CELL_EN    FFE_POWER_CTL_F2_CELLN_EN_WB
#define DMOD_PC_F3_CELL_EN    FFE_POWER_CTL_F3_CELLN_EN_WB
#define DMOD_W_PCPD           TX_GAIN_ADJ_WR_WB
#define DMOD_PCC              POWER_COMB_CTL_WB
#define DMOD_TT_K1_GAIN       FFE_TT_K1_GAIN_WB         /* K1 Gain Register */
#define DMOD_TT_K2_GAIN       FFE_TT_K2_GAIN_WB         /* K2 Gain Register */
#define DMOD_F_CTL_2          FFE_FINGER_CTL2_WB
#define DMOD_W_TTA_0          FFE_TT_ACCUM_LOWER_WB     /* Bits 7:0 */
#define DMOD_W_TTA_1          FFE_TT_ACCUM_UPPER_WB     /* Bits 15:8 */
#define DMOD_W_FRSSI          FFE_FN_RSSI_INIT_WB
#define DMOD_OFFSET_0         FFE_FN_BINARY_OFFSET_LOWER_WB  /* Bits 7:0 */
#define DMOD_OFFSET_1         FFE_FN_BINARY_OFFSET_UPPER_WB  /* Bits 14:8 */
#define DMOD_IMASK_0          FFE_FN_PN_I_MASK_LOWER_WB /* Bits 7:0 */
#define DMOD_IMASK_1          FFE_FN_PN_I_MASK_UPPER_WB /* Bits 14:8 */
#define DMOD_QMASK_0          FFE_FN_PN_Q_MASK_LOWER_WB /* Bits 7:0 */
#define DMOD_QMASK_1          FFE_FN_PN_Q_MASK_UPPER_WB /* Bits 14:0 */
#define DMOD_WALSH            FFE_FN_CODE_CHAN_ID_WB
#define DMOD_SLEW_0           FFE_SLEW_VALUE_LOWER_WB   /* Bits 7:0 */
#define DMOD_SLEW_1           FFE_SLEW_VALUE_MID_WB     /* Bits 15:8 */
#define DMOD_SLEW_2           FFE_SLEW_VALUE_UPPER_WB   /* Bits 18:16 */
#define DMOD_F_CTL_1          FFE_FINGER_CTL1_WB
#define DMOD_PILOT_GAIN       FFE_PILOT_FILT_GAIN_WB
#define DMOD_F_PWR_THRESH     FFE_FN_PWR_THRESH_WB
#define DMOD_F_LOCK_LO        FFE_LOCK_THRESH_LOWER_WB  /* Not a paged reg in MSM23 */
#define DMOD_F_LOCK_UP        FFE_LOCK_THRESH_UPPER_WB  /* Not a paged reg in MSM23 */
#define DMOD_FREQ_COMB_GAIN   FREQ_COMB_GAIN_WB
#define DMOD_FREQ_COMB_CTL    FREQ_COMB_CTL_WB
#define DMOD_CAR_FREQ_0       CARRIER_FREQ_ERR_WR_LSB_WB  /* Bits 7:0 */
#define DMOD_CAR_FREQ_1       CARRIER_FREQ_ERR_WR_MSB_WB  /* Bits 15:8 */
#define DMOD_W_SC_CTL_0       SYMB_COMB_CTL0_WB

/*MSM510X: new bits*/
#define DMOD_SC_CTL_1         SYMB_COMB_CTL1_WH

#define DMOD_POS_LATCH        SYMB_COMB_POS_DUMP_WB
#define DMOD_FREQ_ADJ_0       SYMB_COMB_FREQ_ADJ_LOWER_WB  /* Bits 7:0 */
#define DMOD_FREQ_ADJ_1       SYMB_COMB_FREQ_ADJ_UPPER_WB  /* Bits 15:8 */
#define DMOD_W_I_PDM          I_OFFSET_WR_WB
#define DMOD_W_Q_PDM          Q_OFFSET_WR_WB
#define DMOD_AGC_CTL          IQ_OFFSET_CTL_WB
#define DMOD_LONG_0           SYMB_COMB_LONG_CODE_LD_0_WB  /* Bits 7:0 */
#define DMOD_LONG_1           SYMB_COMB_LONG_CODE_LD_1_WB  /* Bits 15:8 */
#define DMOD_LONG_2           SYMB_COMB_LONG_CODE_LD_2_WB  /* Bits 23:16 */
#define DMOD_LONG_3           SYMB_COMB_LONG_CODE_LD_3_WB  /* Bits 31:24 */
#define DMOD_LONG_4           SYMB_COMB_LONG_CODE_LD_4_WB  /* Bits 39:32 */
#define DMOD_LONG_5           SYMB_COMB_LONG_CODE_LD_5_WB  /* Bits 41:40 */
#define DMOD_LONG_MASK_0      SYMB_COMB_LONG_CODE_MASK_0_WB   /* Bits 7:0 */
#define DMOD_LONG_MASK_1      SYMB_COMB_LONG_CODE_MASK_1_WB   /* Bits 15:8 */
#define DMOD_LONG_MASK_2      SYMB_COMB_LONG_CODE_MASK_2_WB   /* Bits 23:16 */
#define DMOD_LONG_MASK_3      SYMB_COMB_LONG_CODE_MASK_3_WB   /* Bits 31:24 */
#define DMOD_LONG_MASK_4      SYMB_COMB_LONG_CODE_MASK_4_WB   /* Bits 39:32 */
#define DMOD_LONG_MASK_5      SYMB_COMB_LONG_CODE_MASK_5_WB   /* Bits 41:40 */
#define DMOD_PDM2             PDM2_CTL_WB
#define DMOD_PA_VGG_CTL       PDM2_CTL_WB
#define DMOD_PDM1             PDM1_CTL_WB
#define DMOD_TX_GAIN_COMP     PDM1_CTL_WB
#define DMOD_TCXO_BLK_TSEN    TCXO_PDM_CTL_WB
#define DMOD_EPOCH            EPOCH_WR_WB
#define DMOD_F_PAGE           FFE_FINGER_PAGE_REG_WB
#define DMOD_SLEEP_CTL        SLEEP_CTL_WB
#define DMOD_SLEEP_TIME_0     POWER_DOWN_SLEEP_INTERVAL_0_WH  /* Bits 7:0 */
#define DMOD_SLEEP_TIME_1     POWER_DOWN_SLEEP_INTERVAL_0_WH  /* Bits 15:8 */
#define DMOD_SLEEP_TIME_2     POWER_DOWN_SLEEP_INTERVAL_1_WH  /* Bits 23:16 */
#define DMOD_SLEEP_TIME_3     POWER_DOWN_SLEEP_INTERVAL_1_WH  /* Bits 31:24 */
#define DMOD_WARM_TIME_0      POWER_DOWN_WU_TIME_WH     /* Bits 7:0 */
#define DMOD_WARM_TIME_1      POWER_DOWN_WU_TIME_WH     /* Bits 15:8 */
#define DMOD_FM_CLK_MDIV_0    YAMN1_CLK_MDIV_LSB_WB
#define DMOD_FM_CLK_MDIV_1    YAMN1_CLK_MDIV_MSB_WB     /* MSM23 Only */
#define DMOD_FM_CLK_NDIV_0    YAMN1_CLK_NDIV_LSB_WB
#define DMOD_FM_CLK_NDIV_1    YAMN1_CLK_NDIV_MSB_WB     /* MSM23 Only */
#define DMOD_FM_CLK_DUTY_0    YAMN1_CLK_DUTY_LSB_WB
#define DMOD_FM_CLK_DUTY_1    YAMN1_CLK_DUTY_MSB_WB     /* MSM23 Only */


/*---------------------------------------------------------------------------
                          Read Registers
---------------------------------------------------------------------------*/

#define DMOD_R_SRCH_CTL       SRCH_DMA_ERROR_RB
#define DMOD_SP_0             SRCH_POSITION_LOW_RB      /* Bits 7:0 */
#define DMOD_SP_1             SRCH_POSITION_MID_RB      /* Bits 15:8 */
#define DMOD_SP_2             SRCH_POSITION_HIGH_RB     /* Bits 17:16 */
#define DMOD_DMA_0            SRCH_DMA_DATA_RB
#define DMOD_SRCH_MAX_ENG_0   SRCH_MAX_ENERGY_LOW_RB    /* Bits 7:0 */
#define DMOD_SRCH_MAX_ENG_1   SRCH_MAX_ENERGY_HIGH_RB   /* Bits 15:8 */
#define DMOD_SRCH_MAX_SEL     SRCH_MAX_SELECT_WB        /* Bits 1:0 */
#define DMOD_SRCH_MAX_INX_0   SRCH_MAX_INDEX_LOW_RB     /* Bits 7:0 */
#define DMOD_SRCH_MAX_INX_1   SRCH_MAX_INDEX_HIGH_RB    /* Bits 10:8 */
#define DMOD_R_PCPD           TX_GAIN_ADJ_RD_RH
#define DMOD_LOCK_RSSI_GAIN   FFE_RSSI_FILT_GAIN_WB     /* Bits 7:0 */
#ifdef T_MSM_5100
#define DMOD_R_FRSSI          FN_LOCK_RSSI_RH           /* Bits 15:0 */
#else
#error code not present
#endif
#define DMOD_POS_0            FN_POSITION_LOWER_RB      /* Bits 7:0 */
#define DMOD_POS_1            FN_POSITION_MID_RB        /* Bits 15:8 */
#define DMOD_POS_2            FN_POSITION_UPPER_RB      /* Bits 17:16 */
#define DMOD_FREQ_SET_0       CARRIER_FREQ_ERR_RD_LSB_RB  /* Bits 7:0 */
#define DMOD_FREQ_SET_1       CARRIER_FREQ_ERR_RD_MSB_RB  /* Bits 15:8 */
#define DMOD_R_SC_CTL_0       SYMB_COMB_STATUS0_RB
#ifdef T_MSM_5100
#define DMOD_POS_PW           SYMB_COMB_POSITION2_RH        /* Bits 15:0 */
#else
#error code not present
#endif
#define DMOD_R_I_PDM          I_OFFSET_RD_RB
#define DMOD_R_Q_PDM          Q_OFFSET_RD_RB
#define DMOD_R_LONG_0         SYMB_COMB_LONG_CODE_RD_0_RB   /* Bits 7:0 */
#define DMOD_R_LONG_1         SYMB_COMB_LONG_CODE_RD_1_RB   /* Bits 15:8 */
#define DMOD_R_LONG_2         SYMB_COMB_LONG_CODE_RD_2_RB   /* Bits 23:16 */
#define DMOD_R_LONG_3         SYMB_COMB_LONG_CODE_RD_3_RB   /* Bits 31:24 */
#define DMOD_R_LONG_4         SYMB_COMB_LONG_CODE_RD_4_RB   /* Bits 39:32 */
#define DMOD_R_LONG_5         SYMB_COMB_LONG_CODE_RD_5_RB   /* Bits 41:40 */


/*===========================================================================

                        END DMOD.H ALIAS

===========================================================================*/

/*===========================================================================

                        START ENCI.H ALIAS

===========================================================================*/


/*---------------------------------------------------------------------------
                          Write Registers
---------------------------------------------------------------------------*/

#define ENC_I_PN_S0           I_PN_STATE_0_WB           /* bits 7:0  */
#define ENC_I_PN_S1           I_PN_STATE_1_WB           /* bits 14:8 */
#define ENC_Q_PN_S0           Q_PN_STATE_0_WB           /* bits 7:0  */
#define ENC_Q_PN_S1           Q_PN_STATE_1_WB           /* bits 14:8 */
#define ENC_U_PN_S0           U_PN_STATE_0_WB           /* bits 7:0   */
#define ENC_U_PN_S1           U_PN_STATE_1_WB           /* bits 15:8  */
#define ENC_U_PN_S2           U_PN_STATE_2_WB           /* bits 23:16 */
#define ENC_U_PN_S3           U_PN_STATE_3_WB           /* bits 31:24 */
#define ENC_U_PN_S4           U_PN_STATE_4_WB           /* bits 39:32 */
#define ENC_U_PN_S5           U_PN_STATE_5_WB           /* bits 41:40 and bozo bit */
#define ENC_U_PN_M0           U_PN_MASK_0_WB            /* bits 7:0   */
#define ENC_U_PN_M1           U_PN_MASK_1_WB            /* bits 15:8  */
#define ENC_U_PN_M2           U_PN_MASK_2_WB            /* bits 23:16 */
#define ENC_U_PN_M3           U_PN_MASK_3_WB            /* bits 31:24 */
#define ENC_U_PN_M4           U_PN_MASK_4_WB            /* bits 39:32 */
#define ENC_U_PN_M5           U_PN_MASK_5_WB            /* bits 41:40 */
#define ENC_PA_WARMUP         PA_WARMUP_WB
#define ENC_WSYM_STATE        MOD_WSYM_STATE_WB
#define ENC_TXSYNC_ST_0       TXSYNC_ST_0_WB            /* Bits 7:0   */
#define ENC_TXSYNC_ST_1       TXSYNC_ST_1_WB            /* Bits 13:8  */
#define ENC_SYSFR_STATE       SYSFR_STATE_WB            /* Bits 5:0   */
#ifndef FEATURE_MSMHWIO
#define ENC_INT_ST            ENC_INT_ST_WB
#define ENC_DATA_WR           ENC_DATA_WB
#define ENC_CTL               ENC_CTL_WB                /* Encoder Control */
#endif

#define ENC_CLK_CTL           MOD_CLK_CTL_WH            /* Clock Control */

#define ENC_MISC_CTL          MOD_MISC_CTL_WB           /* Miscellanious control */
#define ENC_FRAME_OFF         FRAME_OFF_WB

#define ENC_TX_TST            TX_DATA_TEST_WB
#define ENC_TST_CTL           MOD_TEST_CTL_WH
#define ENC_CRC_UB            CRC_MSB_WB                /* CRC Polynomial Upper Bits     */
#define ENC_CRC_LB            CRC_LSB_WB                /* CRC Polynomial Lower Bits     */
#define ENC_TEST_POINT_SEL    TEST_POINT_SEL_WH

/*---------------------------------------------------------------------------
                       Address-Activated Registers
---------------------------------------------------------------------------*/

#define ENC_RESET             MOD_RESET_WB              /* Reset the chip state. */

/*---------------------------------------------------------------------------
                             Read Registers
---------------------------------------------------------------------------*/



/*===========================================================================

                        END ENCI.H ALIAS

===========================================================================*/

/*===========================================================================

                        START CAGC.H ALIAS
                        START RFMSM.C ALIAS

===========================================================================*/

#define CAGC_CNTL             AGC_CTL_WB
#define CAGC_OUT_CTL          AGC_CTL2_WB

/* This register is [0:5] bits long but we are redefining it as a WH to keep
   the definition consistent with the other MSMs */
#define CAGC_TEST_CNTL        AGC_TEST_CTL_WB
#define AGC_CTL3_WH           AGC_CTL3_WB
#define CAGC_AGC_VALUE_MAX    AGC_VALUE_MAX_WB
#define CAGC_IM_STATE_CTL     IM_STATE_CTL_WB
#define CAGC_PA_R1_RISE       PA_R1_RISE_WB
#define RF_PA_R1_RISE         PA_R1_RISE_WB
#define CAGC_PA_R1_FALL       PA_R1_FALL_WB
#define RF_PA_R1_FALL         PA_R1_FALL_WB
#define CAGC_PA_R2_RISE       PA_R2_RISE_WB
#define RF_PA_R2_RISE         PA_R2_RISE_WB
#define CAGC_PA_R2_FALL       PA_R2_FALL_WB
#define RF_PA_R2_FALL         PA_R2_FALL_WB
#define CAGC_PA_R3_RISE       PA_R3_RISE_WB
#define RF_PA_R3_RISE         PA_R3_RISE_WB
#define CAGC_PA_R3_FALL       PA_R3_FALL_WB
#define RF_PA_R3_FALL         PA_R3_FALL_WB
#define CAGC_RAS_RAM_CNTL     AGC_RDWR_CTL_WB
#define RF_SYNTH_STATUS_R_X    PA_ON_STATUS_RH

/*===========================================================================

                        END CAGC.H ALIAS
                        END RFG.C ALIAS

===========================================================================*/

/*  Done for MSM3100, but backward compatible with MSM3000 */

/*===========================================================================

                        START DFM.H ALIAS

===========================================================================*/

/*---------------------------------------------------------------------------
                      IQ Demodulator WRITE Registers
---------------------------------------------------------------------------*/

#define DFM_INITIAL_R           DFM_INITIAL_WH
#define DFM_DC_OFFSET_GAIN_R    DFM_DC_OFFSET_GAIN_WB
#define DFM_AGC_REF_R           DFM_AGC_REF_WB
#define DFM_AGC_ACC_MIN_R       DFM_AGC_ACC_MIN_WB
#define DFM_AGC_ACC_MAX_R       DFM_AGC_ACC_MAX_WB
#define DFM_AGC_GAIN_R          DFM_AGC_GAIN_WB
#define DFM_FREQ_LOOP_CFG_R     DFM_FREQ_LOOP_CONFIG_WB
#define DFM_PDM_CFG_R           DFM_PDM_CONFIG_WB
#define DFM_I_PDM_LSB_R         DFM_DC_PDM_0_WB
#define DFM_Q_PDM_LSB_R         DFM_DC_PDM_1_WB
#define DFM_IQ_PDM_MSB_R        DFM_DC_PDM_2_WB
#define DFM_RX_AGC_PDM_R        DFM_RXAGC_PDM_0_WB
#define DFM_FREQ_TRK_PDM_LSB_R  DFM_FREQ_PDM_0_WB
#define DFM_FREQ_TRK_PDM_MSB_R  DFM_FREQ_PDM_1_WB
#define DFM_VOC_INTF_CFG_R      DFM_VOC_INTF_CONFIG_WB
#define DFM_IQDMOD_LOOP_STAT_0_R DFM_RXIQ_STATUS_0_RB
#define DFM_IQDMOD_LOOP_STAT_1_R DFM_RX_AGC_FILTER_RB
#define DFM_IQDMOD_LOOP_STAT_2_R DFM_RX_AGC_RSSI_RB

/*---------------------------------------------------------------------------
                     TX Wideband Data WRITE Registers
---------------------------------------------------------------------------*/

#define DFM_TX_WBD_MSB_R        DFM_TXWBD_INTF_0_WB
#define DFM_TX_WBD_LSB_R        DFM_TXWBD_INTF_1_WB
#define DFM_TX_AGC_PDM_LSB_R    DFM_MAX_TX_PWR_0_WB
#define DFM_TX_AGC_PDM_MSB_R    DFM_MAX_TX_PWR_1_WB
#define DFM_FREQ_SENSE_GAIN_R   DFM_FREQ_SENS_GAIN_WB
#define DFM_TX_FM_CFG_R         DFM_TXFM_CONFIG_WB

/*---------------------------------------------------------------------------
                      TX Wideband Data READ Registers
---------------------------------------------------------------------------*/

#define DFM_TX_WBD_STAT_R       DFM_TXWBD_STATUS_RB


/*---------------------------------------------------------------------------
                      RX Wideband Data WRITE Registers
---------------------------------------------------------------------------*/

#define DFM_MIN1_0_7_R          DFM_MIN1_BYTE_0_WB        /* MIN1 bits 0..7  */
#define DFM_MIN1_8_15_R         DFM_MIN1_BYTE_1_WB        /* 8..15           */
#define DFM_MIN1_16_23_R        DFM_MIN1_BYTE_2_WB        /* 16..23          */
#define DFM_RX_WBD_BW_R         DFM_RXWBD_BANDWIDTH_WB
#define DFM_RX_WBD_CFG_R        DFM_RXWBD_CONFIG_0_WB
#define DFM_RX_WBD_WR_R         DFM_RXWBD_WR_WB

/*---------------------------------------------------------------------------
                     RX Wideband Data READ Registers
---------------------------------------------------------------------------*/

#define DFM_RX_WBD_0_7_R        DFM_RXWBD_STAT_0_RB
#define DFM_RX_WBD_8_15_R       DFM_RXWBD_STAT_1_RB
#define DFM_RX_WBD_16_23_R      DFM_RXWBD_STAT_2_RB
#define DFM_RX_WBD_24_27_R      DFM_RXWBD_STAT_3_RB
#define DFM_RX_WBD_RD_R         DFM_RXWBD_RD_RB
#define DFM_WORD_SYNC_COUNT_R   DFM_WORD_SYNC_COUNT_RB

/*===========================================================================

                        END DFM.H ALIAS

===========================================================================*/


/*===========================================================================

                        START DECI.H

===========================================================================*/

#define DEC_MODE           DECMODE_WB
#define DEC_SMTDATA        SMTDATA_WB
#define DEC_OBADDR         OBADDRESS_WB

#define DEC_Q2448          QT8_QT4_WB
#define DEC_Q96192         QT2_QT1_WB


#define DEC_TESTCON        DEC_TESTCON_WB
#define DEC_TESTMUX        DEC_TESTSEL_WB
#define DEC_TEST_RAM_SEL   TEST_MEM_SEL_WH
#define DEC_TEST_POINT_SEL TEST_POINT_SEL_WH
#define DEC_SYNC           DEC_TESTSYNC_WB
#define DEC_CRC            DEC_CRC_WH
#define DEC_RESET          DEC_RESET_WB

#define DEC_CLK_CTL_1     MSM_CLK_CTL1_WH
#define DEC_CLK_CTL_2     MSM_CLK_CTL2_WH
#define DEC_CLK_CTL_3     MSM_CLK_CTL3_WH
#define DEC_CLK_CTL_4     MSM_CLK_CTL4_WH

#define DEC_DATA          DECDATA_RH
#define DEC_STATUS        DECSTATUS_RB

#define DEC_SER8          SER8_RB
#define DEC_SER4          SER4_RB
#define DEC_SER2          SER2_RB
#define DEC_SER1          SER1_RB

#define DEC_TESTOUT       DEC_TESTOUT_RH

//UNUSED REGISTERS
#define DEC_SYMIN          MSM3_DUMMY
#define DEC_CLK            MSM3_DUMMY
#define DEC_PINCON         MSM3_DUMMY
#define DEC_SCAN_IN        MSM3_DUMMY
#define DEC_TEST_RAM_CS    MSM3_DUMMY
#define DEC_TEST_RAM_ADDR  MSM3_DUMMY
#define DEC_TEST_DATA      MSM3_DUMMY
#define DEC_SCAN_CTL       MSM3_DUMMY
#define DEC_SCAN_MUX       MSM3_DUMMY
#define DEC_PHI1           MSM3_DUMMY
#define DEC_PHI2           MSM3_DUMMY
#define DEC_ID             MSM3_DUMMY
#define DEC_ACSRAM         MSM3_DUMMY
#define DEC_SCAN_OUT       MSM3_DUMMY

/*===========================================================================

                        END DECI.H

===========================================================================*/



/*===========================================================================

                        START DLOAD2I.H SIOG2.C

===========================================================================*/
#ifndef FEATURE_MSMHWIO
#define UART_MR1     UART_MR1_WH
#endif
#define UART_TXWAT   UART_TFWR_WH
#define UART_RXWAT   UART_RFWR_WH
#ifndef FEATURE_MSMHWIO
#define UART_MR2     UART_MR2_WB
#define UART_SR      UART_SR_RB
#define UART_CSR     UART_CSR_WB
#endif
#define UART_RFIFO   UART_RF_RB
#define UART_TFIFO   UART_TF_WB
#ifndef FEATURE_MSMHWIO
#define UART_MISR    UART_MISR_RB
#define UART_CR      UART_CR_WB
#define UART_ISR     UART_ISR_RB
#define UART_IMR     UART_IMR_WB
#define UART_IPR     UART_IPR_WB
#define UART_HCR     UART_HCR_WB
#endif
#define UART_MVR     UART_MREG_WB
#define UART_NVR     UART_NREG_WB
#define UART_DVR     UART_DREG_WB
#define UART_MNDR    UART_MNDREG_WB

#define MSMU_MR1     UART_MR1_WH       /* Mode Register 1                  */
#define MSMU_TFWR    UART_TFWR_WH      /* TX FIFO Watermark Register       */
#define MSMU_RFWR    UART_RFWR_WH      /* RX FIFO watermark Register       */
#define MSMU_IPR     UART_IPR_WH       /* Interrupt Programming Register   */

#define MSMU_MR2     UART_MR2_WB       /* Mode Register 2                  */
#define MSMU_SR      UART_SR_RB        /* Status Register                  */
#define MSMU_CSR     UART_CSR_WB       /* Clock Select Register            */
#define MSMU_RF      UART_RF_RB        /* Receive FIFO                     */
#define MSMU_TF      UART_TF_WB        /* Transmit FIFO                    */
#define MSMU_MISR    UART_MISR_RB      /* Masked Interrupt Status Register */
#define MSMU_CR      UART_CR_WB        /* Command Register                 */
#define MSMU_ISR     UART_ISR_RB       /* Interrupt Status Register        */
#define MSMU_IMR     UART_IMR_WB       /* Interrupt Mask Register          */


#define MSMU_HCR     UART_HCR_WB       /* Hunt Character Register          */
#define MSMU_MVR     UART_MREG_WB      /* M Value Register                 */
#define MSMU_NVR     UART_NREG_WB      /* N Value Register                 */
#define MSMU_DVR     UART_DREG_WB      /* D Value Register                 */
#define MSMU_MNDR    UART_MNDREG_WB    /* Extra Value Register             */
#define MSMU_IRDA    UART_IRDA_WB      /* UART IRDA Register               */

/* These are definitions for second UART */
#ifndef FEATURE_MSMHWIO
#define UART2_MR1     UART2_MR1_WB
#define UART2_MR2     UART2_MR2_WB
#define UART2_SR      UART2_SR_RB
#define UART2_CSR     UART2_CSR_WB
#endif
#define UART2_RFIFO   UART2_RF_RB
#define UART2_TFIFO   UART2_TF_WB
#ifndef FEATURE_MSMHWIO
#define UART2_MISR    UART2_MISR_RB
#define UART2_CR      UART2_CR_WB
#define UART2_ISR     UART2_ISR_RB
#define UART2_IMR     UART2_IMR_WB
#define UART2_IPR     UART2_IPR_WB
#endif
#define UART2_TXWAT   UART2_TFWR_WB
#define UART2_RXWAT   UART2_RFWR_WB
#ifndef FEATURE_MSMHWIO
#define UART2_HCR     UART2_HCR_WB
#endif
#define UART2_MVR     UART2_MREG_WB
#define UART2_NVR     UART2_NREG_WB
#define UART2_DVR     UART2_DREG_WB
#define UART2_MNDR    UART2_MNDREG_WB


#define MSMU2_MR1     UART2_MR1_WB       /* Mode Register 1                  */
#define MSMU2_MR2     UART2_MR2_WB       /* Mode Register 2                  */
#define MSMU2_SR      UART2_SR_RB        /* Status Register                  */
#define MSMU2_CSR     UART2_CSR_WB       /* Clock Select Register            */
#define MSMU2_RF      UART2_RF_RB        /* Receive FIFO                     */
#define MSMU2_TF      UART2_TF_WB        /* Transmit FIFO                    */
#define MSMU2_MISR    UART2_MISR_RB      /* Masked Interrupt Status Register */
#define MSMU2_CR      UART2_CR_WB        /* Command Register                 */
#define MSMU2_ISR     UART2_ISR_RB       /* Interrupt Status Register        */
#define MSMU2_IMR     UART2_IMR_WB       /* Interrupt Mask Register          */
#define MSMU2_IPR     UART2_IPR_WB       /* Interrupt Programming Register   */
#define MSMU2_TFWR    UART2_TFWR_WB      /* TX FIFO Watermark Register       */
#define MSMU2_RFWR    UART2_RFWR_WB      /* RX FIFO watermark Register       */
#define MSMU2_HCR     UART2_HCR_WB       /* Hunt Character Register          */
#define MSMU2_IRDA    UART2_IRDA_WB      /* UART IRDA Register               */
#define MSMU2_SIM_CFG UART2_SIM_CFG_WB   /* UART2 SIM interface cinfigure    */

/* These are definitions for third uart */
#define MSMU3_MR1     UART3_MR1_WB       /* Mode Register 1                  */
#define MSMU3_MR2     UART3_MR2_WB       /* Mode Register 2                  */
#define MSMU3_SR      UART3_SR_RB        /* Status Register                  */
#define MSMU3_CSR     UART3_CSR_WB       /* Clock Select Register            */
#define MSMU3_RF      UART3_RF_RB        /* Receive FIFO                     */
#define MSMU3_TF      UART3_TF_WB        /* Transmit FIFO                    */
#define MSMU3_MISR    UART3_MISR_RB      /* Masked Interrupt Status Register */
#define MSMU3_CR      UART3_CR_WB        /* Command Register                 */
#define MSMU3_ISR     UART3_ISR_RB       /* Interrupt Status Register        */
#define MSMU3_IMR     UART3_IMR_WB       /* Interrupt Mask Register          */
#define MSMU3_IPR     UART3_IPR_WB       /* Interrupt Programming Register   */
#define MSMU3_TFWR    UART3_TFWR_WB      /* TX FIFO Watermark Register       */
#define MSMU3_RFWR    UART3_RFWR_WB      /* RX FIFO watermark Register       */
#define MSMU3_HCR     UART3_HCR_WB       /* Hunt Character Register          */
#define MSMU3_IRDA    UART3_IRDA_WB      /* UART IRDA Register               */
#define MSMU3_SIM_CFG UART3_SIM_CFG_WB   /* UART3 SIM interface cinfigure    */

/*===========================================================================

                        UARTDM block remapping

===========================================================================*/
#define HWIO_UART1DM_MR1_ADDR         HWIO_GSBI1_UART_DM_MR1_ADDR
#define HWIO_UART1DM_MR2_ADDR         HWIO_GSBI1_UART_DM_MR2_ADDR
#define HWIO_UART1DM_IRDA_ADDR        HWIO_GSBI1_UART_DM_IRDA_ADDR
#define HWIO_UART1DM_IMR_ADDR         HWIO_GSBI1_UART_DM_IMR_ADDR
#define HWIO_UART1DM_ISR_ADDR         HWIO_GSBI1_UART_DM_ISR_ADDR
#define HWIO_UART1DM_TF_ADDR          HWIO_GSBI1_UART_DM_TF_ADDR
#define HWIO_UART1DM_RF_ADDR          HWIO_GSBI1_UART_DM_RF_ADDR
#define HWIO_UART1DM_RFWR_ADDR        HWIO_GSBI1_UART_DM_RFWR_ADDR
#define HWIO_UART1DM_CSR_ADDR         HWIO_GSBI1_UART_DM_CSR_ADDR
#define HWIO_UART1DM_CR_ADDR          HWIO_GSBI1_UART_DM_CR_ADDR
#define HWIO_UART1DM_SR_ADDR          HWIO_GSBI1_UART_DM_SR_ADDR
#define HWIO_UART1DM_TFWR_ADDR        HWIO_GSBI1_UART_DM_TFWR_ADDR
#define HWIO_UART1DM_IPR_ADDR         HWIO_GSBI1_UART_DM_IPR_ADDR
#define HWIO_UART1DM_HCR_ADDR         HWIO_GSBI1_UART_DM_HCR_ADDR
#define HWIO_UART1DM_MISR_ADDR        HWIO_GSBI1_UART_DM_MISR_ADDR
#define HWIO_UART1DM_DMRX_ADDR        HWIO_GSBI1_UART_DM_DMRX_ADDR
#define HWIO_UART1DM_RXFS_ADDR        HWIO_GSBI1_UART_DM_RXFS_ADDR
#define HWIO_UART1DM_DMEN_ADDR        HWIO_GSBI1_UART_DM_DMEN_ADDR
#define HWIO_UART1DM_BADR_ADDR        HWIO_GSBI1_UART_DM_BADR_ADDR
#define HWIO_UART1DM_RX_TOTAL_SNAP_ADDR         HWIO_GSBI1_UART_DM_RX_TOTAL_SNAP_ADDR
#define HWIO_UART1DM_NO_CHARS_FOR_TX_ADDR       HWIO_GSBI1_UART_DM_NO_CHARS_FOR_TX_ADDR

/*===========================================================================

                        END DLOAD2I.H SIOG2.C

===========================================================================*/




/*===========================================================================

                        START INTERRUPT ALIAS

===========================================================================*/
#define DMOD_POLARITY           INT_POLARITY_2_WB
#define DMOD_RX_DATA_INT_POL_M  INT_POLARITY_2_WB__UART1_DP_RX_DATA_MASK



/*=============================================================================

               SEARCHER1 AND SEARCHER2 ALIASES.


============================================================================= */

#define SEARCH_STATUS_DUMP_CTL_WB                           SRCH_STATUS_DUMP_CTL_WB
#define SEARCH_STATUS_DUMP_CTL_WB_MASK                        SRCH_STATUS_DUMP_CTL_WB_MASK
#define SEARCH_STATUS_DUMP_CTL_WB__SEARCH2_STATUS_CLEAR_MASK  SRCH_STATUS_DUMP_CTL_WB__SEARCH2_STATUS_CLEAR_MASK
#define SEARCH_STATUS_DUMP_CTL_WB__SEARCH1_STATUS_CLEAR_MASK  SRCH_STATUS_DUMP_CTL_WB__SEARCH1_STATUS_CLEAR_MASK
#define SEARCH_STATUS_DUMP_CTL_WB__SEARCH2_DUMP_EN_MASK       SRCH_STATUS_DUMP_CTL_WB__SEARCH2_DUMP_EN_MASK
#define SEARCH_STATUS_DUMP_CTL_WB__SEARCH1_DUMP_EN_MASK       SRCH_STATUS_DUMP_CTL_WB__SEARCH1_DUMP_EN_MASK


#define SEARCH_STATUS_DUMP_STATUS_RB                             SRCH_STATUS_DUMP_STATUS_RB
#define SEARCH_STATUS_DUMP_STATUS_RB_MASK                        SRCH_STATUS_DUMP_STATUS_RB_MASK
#define SEARCH_STATUS_DUMP_STATUS_RB__MICRO_DUMP_STATUS_MASK     SRCH_STATUS_DUMP_STATUS_RB__MICRO_DUMP_STATUS_MASK
#define SEARCH_STATUS_DUMP_STATUS_RB__SEARCH2_DUMP_STATUS_MASK   SRCH_STATUS_DUMP_STATUS_RB__SEARCH2_DUMP_STATUS_MASK
#define SEARCH_STATUS_DUMP_STATUS_RB__SEARCH1_DUMP_STATUS_MASK   SRCH_STATUS_DUMP_STATUS_RB__SEARCH1_DUMP_STATUS_MASK

#define DEM_FRAME_OFF_WB                                        DEM_FRAME_CTL_WB
#define DEM_FRAME_OFF_WB_MASK                 DEM_FRAME_CTL_WB_MASK
#define DEM_FRAME_OFF_WB__SET_ROLL_SYNC_MASK                    DEM_FRAME_CTL_WB__SET_ROLL_SYNC_MASK
#define DEM_FRAME_OFF_WB__FRAME_SYNC_BYPASS_MASK                DEM_FRAME_CTL_WB__FRAME_SYNC_BYPASS_MASK
#define DEM_FRAME_OFF_WB__HISTORY_LOG_DISABLE_MASK              DEM_FRAME_CTL_WB__HISTORY_LOG_DISABLE_MASK

#define SYMB_COMB_CTL0_WH__SYMB_COMB_FIN_EN_n_MASK              SYMB_COMB_CTL0_WH__SYMB_COMB_FIN_EN_N_MASK
#ifndef T_MSM6100
#error code not present
#endif

#define SEARCH2_CTL_WB                                           SRCH2_CTL_WB
#define SEARCH2_CTL_WB_MASK                                      SRCH2_CTL_WB_MASK
#define SEARCH2_CTL_WB__SCALE_MODE_MASK                          SRCH2_CTL_WB__SCALE_MODE_MASK
#define SEARCH2_CTL_WB__SHOULDER_MODE_MASK                       SRCH2_CTL_WB__SHOULDER_MODE_MASK
#define SEARCH2_CTL_WB__ALL_ENERGIES_MASK      SRCH2_CTL_WB__ALL_ENERGIES_MASK
#define SEARCH2_CTL_WB__RESET_QUEUE_PTRS_MASK                    SRCH2_CTL_WB__WR_RES_QUEUE_PTRS_MASK
#define SEARCH2_CTL_WB__RESET_RESULTS_PTRS_MASK                  SRCH2_CTL_WB__WR_RES_RESULTS_PTRS_MASK
#define SEARCH2_CTL_WB__SEARCH_ABORT_MASK                        SRCH2_CTL_WB__WR_SEARCH_ABORT_MASK
#define SEARCH2_CTL_WB__STATUS_DUMP_MASK                         SRCH2_CTL_WB__WR_STATUS_DUMP_MASK
#define SEARCH2_QUEUE_WH                                         SRCH2_QUEUE_WH
#define SEARCH2_QUEUE_WH_MASK                                    SRCH2_QUEUE_WH_MASK

#define  SEARCH2_POSITION_LOW_RH                                 SRCH2_POSITION_LOW_RH
#define  SEARCH2_POSITION_LOW_RH_MASK                            SRCH2_POSITION_LOW_RH_MASK

#define  SEARCH2_POSITION_HIGH_RH                                SRCH2_POSITION_HIGH_RH
#define  SEARCH2_POSITION_HIGH_RH_MASK                           SRCH2_POSITION_HIGH_RH_MASK


#define SEARCH2_RESULTS_RH                                       SRCH2_RESULTS_RH
#define SEARCH2_RESULTS_RH_MASK                                  SRCH2_RESULTS_RH_MASK
#define SEARCH2_QUEUE_PTRS_RH                                    SRCH2_QUEUE_PTRS_RH
#define SEARCH2_QUEUE_PTRS_RH_MASK                               SRCH2_QUEUE_PTRS_RH_MASK
#define SEARCH2_QUEUE_PTRS_RH__QUEUE_WRITE_PTR_MASK              SRCH2_QUEUE_PTRS_RH__QUEUE_WRITE_PTR_MASK
#define SEARCH2_QUEUE_PTRS_RH__QUEUE_WRITE_SUB_PTR_MASK          SRCH2_QUEUE_PTRS_RH__QUEUE_WRITE_SUB_PTR_MASK
#define SEARCH2_QUEUE_PTRS_RH__QUEUE_READ_PTR_MASK               SRCH2_QUEUE_PTRS_RH__QUEUE_READ_PTR_MASK
#define SEARCH2_QUEUE_PTRS_RH__QUEUE_READ_SUB_PTR_MASK           SRCH2_QUEUE_PTRS_RH__QUEUE_READ_SUB_PTR_MASK

#define SEARCH2_RESULTS_PTRS_RH                                  SRCH2_RESULTS_PTRS_RH
#define SEARCH2_RESULTS_PTRS_RH_MASK                             SRCH2_RESULTS_PTRS_RH_MASK
#define SEARCH2_RESULTS_PTRS_RH__RESULTS_WRITE_PTR_MASK          SRCH2_RESULTS_PTRS_RH__RESULTS_WRITE_PTR_MASK
#define SEARCH2_RESULTS_PTRS_RH__RESULTS_WRITE_SUB_PTR_MASK      SRCH2_RESULTS_PTRS_RH__RESULTS_WRITE_SUB_PTR_MASK
#define SEARCH2_RESULTS_PTRS_RH__RESULTS_READ_PTR_MASK           SRCH2_RESULTS_PTRS_RH__RESULTS_READ_PTR_MASK
#define SEARCH2_RESULTS_PTRS_RH__RESULTS_READ_SUB_PTR_MASK       SRCH2_RESULTS_PTRS_RH__RESULTS_READ_SUB_PTR_MASK

/*===========================================================================

                        END INTERRUPT ALIAS

===========================================================================*/
/* Needed to initialize pull-ups
*/
#define GPIO_PUPD_N_2_WH_DEFAULT_MASK                                0x13a3
#define GPIO_PUPD_N_3_WB_DEFAULT_MASK                                  0xf8

#ifndef FEATURE_L4
/* L4LINUX_TODO -- need to define for L4 */

/*  Needed to access the LCD device
*/
#ifdef T_FFA
#error code not present
#else /* SURF */
  #define QVGA_CMD_WH                         0x20000000    /* QVGA */
  #define QVGA_DATA_WH                        0x20100000

  #define QCIF_CMD_WH                         0x38000000    /* QCIF */
  #define QCIF_DATA_WH                        0x38100000
#endif /* T_FFA */

#else /* L4 */
#error code not present
#endif /* FEATURE_L4 */

/*===========================================================================

USBH

===========================================================================*/

#ifndef USBH_BASE
#define USBH_BASE (HWIO_USB_OTG_HS_ID_ADDR)
#endif

#endif /*MSMREDEFS_H*/


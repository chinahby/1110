#ifndef QDSPEXT_H
#define QDSPEXT_H

/*
WARNING: This file is auto-generated.

Generated at:    Fri Apr 29 05:50:08 2011
Generated using: MSMSHARED_EXT_PARSER.00.11.00.01.07
Generated from:

-------------------------------------------------------------------------------
Configuration file:
//depot/asic/qsc1100/services/adsp/qdspext.cfg#31
-------------------------------------------------------------------------------
IMAGE[0] voc_combo
//depot/asic/qsc1100/services/adsp/firmware/voc/voc_icc_ngec.rpt#9
//depot/asic/qsc1100/services/adsp/firmware/voc/voc_icc_ngec.ext#9
-------------------------------------------------------------------------------
IMAGE[1] voc_combo
//depot/asic/qsc1100/services/adsp/firmware/voc/voc.rpt#1
//depot/asic/qsc1100/services/adsp/firmware/voc/voc.ext#1
-------------------------------------------------------------------------------
IMAGE[2] synth
1839833 05/27/2009 01:26:05 firmware\qsynth_cmx\qsynth_cmx.rpt
9351393 05/27/2009 01:26:06 firmware\qsynth_cmx\qsynth_cmx.ext
-------------------------------------------------------------------------------
IMAGE[3] synth
1000280 04/05/2010 01:50:42 firmware\qsynth_midi\qsynth_midi.rpt
1433908 04/05/2010 01:50:43 firmware\qsynth_midi\qsynth_midi.ext
-------------------------------------------------------------------------------
IMAGE[4] enh_aac_plus
//depot/asic/qsc1100/services/adsp/firmware/qtv/qtv.rpt#7
//depot/asic/qsc1100/services/adsp/firmware/qtv/qtv.ext#7
-------------------------------------------------------------------------------
IMAGE[5] qtv_aac
//depot/asic/qsc1100/services/adsp/firmware/qtv/qtv.rpt#7
//depot/asic/qsc1100/services/adsp/firmware/qtv/qtv.ext#7
-------------------------------------------------------------------------------
IMAGE[6] qtv
//depot/asic/qsc1100/services/adsp/firmware/qtv/qtv.rpt#7
//depot/asic/qsc1100/services/adsp/firmware/qtv/qtv.ext#7
-------------------------------------------------------------------------------
IMAGE[7] qtv
//depot/asic/qsc1100/services/adsp/firmware/qtv/qtv.rpt#7
//depot/asic/qsc1100/services/adsp/firmware/qtv/qtv.ext#7
-------------------------------------------------------------------------------
IMAGE[8] jpegd
   2206 06/03/2009 22:26:05 firmware\jpegd\jpegd.h
 104944 06/03/2009 22:26:05 firmware\jpegd\jpegd.ext
-------------------------------------------------------------------------------
IMAGE[9] crypt
//depot/asic/qsc1100/services/adsp/firmware/sec/sscrypto_sfs.rpt#3
//depot/asic/qsc1100/services/adsp/firmware/sec/sscrypto_sfs.ext#3
-------------------------------------------------------------------------------
IMAGE[10] sa_ds05
//depot/asic/qsc1100/services/adsp/firmware/sa_ds/sa_ds051.h#1
//depot/asic/qsc1100/services/adsp/firmware/sa_ds/sa_ds051.ext#2
-------------------------------------------------------------------------------
IMAGE[11] cam_audio_conc
//depot/asic/qsc1100/services/adsp/firmware/acc/acc.rpt#4
//depot/asic/qsc1100/services/adsp/firmware/acc/acc.ext#4
-------------------------------------------------------------------------------
IMAGE[12] qcamcorder_combo
//depot/asic/qsc1100/services/adsp/firmware/qcamcorder/qcamcorder20.rpt#5
//depot/asic/qsc1100/services/adsp/firmware/qcamcorder/qcamcorder20.ext#5
-------------------------------------------------------------------------------
IMAGE[13] qcamcorder_combo
//depot/asic/qsc1100/services/adsp/firmware/qcamcorder/qcamcorder20.rpt#5
//depot/asic/qsc1100/services/adsp/firmware/qcamcorder/qcamcorder20.ext#5
-------------------------------------------------------------------------------
IMAGE[14] i2ssbc
//depot/asic/qsc6055/services/adsp/firmware/i2ssbc/qsc2x_i2ssbc.rpt#1
//depot/asic/qsc6055/services/adsp/firmware/i2ssbc/qsc2x_i2ssbc.ext#1
-------------------------------------------------------------------------------
*/

/*===========================================================================

                    Q D S P   I M A G E   S O U R C E
       A N D   A C C E S S   F U N C T I O N S   D E F I N I T I O N

DESCRIPTION
  Definition of the firmware images that can be dowloaded to the QDSP.
  Definition of the QDSP variables and the access functions.

REFERENCES

Copyright (c) 2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#ifdef FEATURE_QDSP_DEBUG_IO_MACROS
#include "msg.h"
#endif /* FEATURE_QDSP_DEBUG_IO_MACROS */
#include "msm.h"
#include "qdspimage.h"
#include "qdspmem.h"

#if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
    defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING) || \
    defined(FEATURE_QDSP_SET_IMAGE_HW_RESOURCES)
#include "clkregim.h"      /* Clock Regime functions             */
#endif
#include "msmaud.h"

/* <EJECT> */
/*===========================================================================

                      D A T A   D E F I N I T I O N S

===========================================================================*/

extern uint32* qdsp_current_dma_table;

/* The DMA default (nop) address */
extern uint32 qdsp_dummy_var;
#define QDSP_NOP_ADDRESS &qdsp_dummy_var

/* Register value to set the DSP to Run Mode.
*/
#define QDSP_RUN_VAL   0x0

/* Register value to set the DSP to Download Mode.
*/
#define QDSP_RESET_VAL 0x1

/* Length, in words, of DSP control registers
*/
#define QDSP_REG_WORD_SIZE 2

/* Offset of microprocessor to DSP IRQ register from the DSP_RESET register
*/
#define QDSP_UP_TO_DSP_IRQ_OFFSET (1)

/* Offset of microprocessor to DSP IRQ TEST register from the DSP_RESET register
*/
#define QDSP_UP_TO_DSP_IRQ_TEST_OFFSET (2)

/* Base address of ROM in ARM space */
#define QDSP_ROM_BASE_ADDRESS 0x0

/* Base address of DSP and DSP hardware registers */
extern const qdspext_data_type qdsp_data_table;
#define QDSP_BASE_ADDRESS ADSP_BASE
#define QDSP_RAMA_OFFSET  0x0
#define QDSP_RAMB_OFFSET  0x200000
#define QDSP_RAMC_OFFSET  0x400000
#define QDSP_RAMI_OFFSET  0x800000
#define QDSP_REG_OFFSET   0xc00000
#define QDSP_CTRL_REGISTER ((QDSP_BASE_ADDRESS) + (QDSP_REG_OFFSET))

/* Definitions to allow configuration of DSP memory */
#define QDSP_MEM_SHARE_MASK     0
#define QDSP_MEM_SHARE_ENABLE_NONE 0

/* (sizeof() of smallest addressable data unit in DME mode) - 1 */
#define QDSP_DME_ADDRESSABLE_DATA_SIZE 1


#if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
    defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING) || \
    defined(FEATURE_QDSP_MODULE_RESOURCE_MGMT)
/* Set module specific DSP clock rate, DME mode etc. */
#define QDSP_SET_MODULE_HW_RESOURCES
#endif

#if (defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
     defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING) || \
     defined(FEATURE_QDSP_MODULE_RESOURCE_MGMT)) && \
    !defined(FEATURE_QDSP_SET_IMAGE_HW_RESOURCES)
/* Set image specific DSP clock rate, DME mode etc. */
#define FEATURE_QDSP_SET_IMAGE_HW_RESOURCES
#endif



/*===========================================================================
    QDSP firmware DMA symbols
===========================================================================*/
/* BEGIN SYMBOLS */
#define QDSP_AF1_erl                                  (volatile uint16*)qdsp_current_dma_table[0]
#define QDSP_AF1_erl_SIZE                             16
#define QDSP_AF1_offset                               (volatile uint16*)qdsp_current_dma_table[1]
#define QDSP_AF1_offset_SIZE                          16
#define QDSP_AF1_preset_coefs                         (volatile uint16*)qdsp_current_dma_table[2]
#define QDSP_AF1_preset_coefs_SIZE                    16
#define QDSP_AF1_taps                                 (volatile uint16*)qdsp_current_dma_table[3]
#define QDSP_AF1_taps_SIZE                            16
#define QDSP_AF1_twoalpha                             (volatile uint16*)qdsp_current_dma_table[4]
#define QDSP_AF1_twoalpha_SIZE                        16
#define QDSP_AF2_erl                                  (volatile uint16*)qdsp_current_dma_table[5]
#define QDSP_AF2_erl_SIZE                             16
#define QDSP_AF2_offset                               (volatile uint16*)qdsp_current_dma_table[6]
#define QDSP_AF2_offset_SIZE                          16
#define QDSP_AF2_preset_coefs                         (volatile uint16*)qdsp_current_dma_table[7]
#define QDSP_AF2_preset_coefs_SIZE                    16
#define QDSP_AF2_taps                                 (volatile uint16*)qdsp_current_dma_table[8]
#define QDSP_AF2_taps_SIZE                            16
#define QDSP_AF2_twoalpha                             (volatile uint16*)qdsp_current_dma_table[9]
#define QDSP_AF2_twoalpha_SIZE                        16
#define QDSP_AF_erl                                   (volatile uint16*)qdsp_current_dma_table[10]
#define QDSP_AF_erl_SIZE                              16
#define QDSP_AF_erl_bg                                (volatile uint16*)qdsp_current_dma_table[11]
#define QDSP_AF_erl_bg_SIZE                           16
#define QDSP_AF_limit                                 (volatile uint16*)qdsp_current_dma_table[12]
#define QDSP_AF_limit_SIZE                            16
#define QDSP_AF_offset                                (volatile uint16*)qdsp_current_dma_table[13]
#define QDSP_AF_offset_SIZE                           16
#define QDSP_AF_preset_coefs                          (volatile uint16*)qdsp_current_dma_table[14]
#define QDSP_AF_preset_coefs_SIZE                     16
#define QDSP_AF_taps                                  (volatile uint16*)qdsp_current_dma_table[15]
#define QDSP_AF_taps_SIZE                             16
#define QDSP_AF_taps_bg                               (volatile uint16*)qdsp_current_dma_table[16]
#define QDSP_AF_taps_bg_SIZE                          16
#define QDSP_AF_twoalpha                              (volatile uint16*)qdsp_current_dma_table[17]
#define QDSP_AF_twoalpha_SIZE                         16
#define QDSP_AVSyncInterruptFlag                      (volatile uint16*)qdsp_current_dma_table[18]
#define QDSP_AVSyncInterruptFlag_SIZE                 16
#define QDSP_AVSyncInterruptInterval                  (volatile uint32*)qdsp_current_dma_table[19]
#define QDSP_AVSyncInterruptInterval_SIZE             32
#define QDSP_AVSyncNumBytesCounterDword               (volatile uint32*)qdsp_current_dma_table[20]
#define QDSP_AVSyncNumBytesCounterDword_SIZE          32
#define QDSP_AVSyncNumBytesCounterMSW                 (volatile uint16*)qdsp_current_dma_table[21]
#define QDSP_AVSyncNumBytesCounterMSW_SIZE            16
#define QDSP_AVSyncNumSamplesCounterDword             (volatile uint32*)qdsp_current_dma_table[22]
#define QDSP_AVSyncNumSamplesCounterDword_SIZE        32
#define QDSP_AVSyncNumSamplesCounterMSW               (volatile uint16*)qdsp_current_dma_table[23]
#define QDSP_AVSyncNumSamplesCounterMSW_SIZE          16
#define QDSP_CSMode                                   (volatile uint16*)qdsp_current_dma_table[24]
#define QDSP_CSMode_SIZE                              16
#define QDSP_CSPCD_threshold                          (volatile uint16*)qdsp_current_dma_table[25]
#define QDSP_CSPCD_threshold_SIZE                     16
#define QDSP_CS_TuningMode                            (volatile uint16*)qdsp_current_dma_table[26]
#define QDSP_CS_TuningMode_SIZE                       16
#define QDSP_CS_echo_path_delay                       (volatile uint16*)qdsp_current_dma_table[27]
#define QDSP_CS_echo_path_delay_SIZE                  16
#define QDSP_CS_echo_path_delay_by_80                 (volatile uint16*)qdsp_current_dma_table[28]
#define QDSP_CS_echo_path_delay_by_80_SIZE            16
#define QDSP_DENS_CNI_level                           (volatile uint16*)qdsp_current_dma_table[29]
#define QDSP_DENS_CNI_level_SIZE                      16
#define QDSP_DENS_NFE_blocksize                       (volatile uint16*)qdsp_current_dma_table[30]
#define QDSP_DENS_NFE_blocksize_SIZE                  16
#define QDSP_DENS_NL_atten                            (volatile uint16*)qdsp_current_dma_table[31]
#define QDSP_DENS_NL_atten_SIZE                       16
#define QDSP_DENS_gamma_e_alpha                       (volatile uint16*)qdsp_current_dma_table[32]
#define QDSP_DENS_gamma_e_alpha_SIZE                  16
#define QDSP_DENS_gamma_e_dt                          (volatile uint16*)qdsp_current_dma_table[33]
#define QDSP_DENS_gamma_e_dt_SIZE                     16
#define QDSP_DENS_gamma_e_high                        (volatile uint16*)qdsp_current_dma_table[34]
#define QDSP_DENS_gamma_e_high_SIZE                   16
#define QDSP_DENS_gamma_e_low                         (volatile uint16*)qdsp_current_dma_table[35]
#define QDSP_DENS_gamma_e_low_SIZE                    16
#define QDSP_DENS_gamma_e_rescue                      (volatile uint16*)qdsp_current_dma_table[36]
#define QDSP_DENS_gamma_e_rescue_SIZE                 16
#define QDSP_DENS_gamma_n                             (volatile uint16*)qdsp_current_dma_table[37]
#define QDSP_DENS_gamma_n_SIZE                        16
#define QDSP_DENS_limit_NS                            (volatile uint16*)qdsp_current_dma_table[38]
#define QDSP_DENS_limit_NS_SIZE                       16
#define QDSP_DENS_spdet_act                           (volatile uint16*)qdsp_current_dma_table[39]
#define QDSP_DENS_spdet_act_SIZE                      16
#define QDSP_DENS_spdet_near                          (volatile uint16*)qdsp_current_dma_table[40]
#define QDSP_DENS_spdet_near_SIZE                     16
#define QDSP_DENS_tail_alpha                          (volatile uint16*)qdsp_current_dma_table[41]
#define QDSP_DENS_tail_alpha_SIZE                     16
#define QDSP_DENS_tail_portion                        (volatile uint16*)qdsp_current_dma_table[42]
#define QDSP_DENS_tail_portion_SIZE                   16
#define QDSP_DET_THRESHOLD                            (volatile uint16*)qdsp_current_dma_table[43]
#define QDSP_DET_THRESHOLD_SIZE                       16
#define QDSP_DNNS_AcThreshold                         (volatile uint16*)qdsp_current_dma_table[44]
#define QDSP_DNNS_AcThreshold_SIZE                    16
#define QDSP_DNNS_EchoAlphaRev                        (volatile uint16*)qdsp_current_dma_table[45]
#define QDSP_DNNS_EchoAlphaRev_SIZE                   16
#define QDSP_DNNS_EchoGammaHi                         (volatile uint16*)qdsp_current_dma_table[46]
#define QDSP_DNNS_EchoGammaHi_SIZE                    16
#define QDSP_DNNS_EchoGammaLo                         (volatile uint16*)qdsp_current_dma_table[47]
#define QDSP_DNNS_EchoGammaLo_SIZE                    16
#define QDSP_DNNS_EchoYcomp                           (volatile uint16*)qdsp_current_dma_table[48]
#define QDSP_DNNS_EchoYcomp_SIZE                      16
#define QDSP_DNNS_NoiseBiasComp                       (volatile uint16*)qdsp_current_dma_table[49]
#define QDSP_DNNS_NoiseBiasComp_SIZE                  16
#define QDSP_DNNS_NoiseGainMinN                       (volatile uint16*)qdsp_current_dma_table[50]
#define QDSP_DNNS_NoiseGainMinN_SIZE                  16
#define QDSP_DNNS_NoiseGainMinS                       (volatile uint16*)qdsp_current_dma_table[51]
#define QDSP_DNNS_NoiseGainMinS_SIZE                  16
#define QDSP_DNNS_NoiseGammaN                         (volatile uint16*)qdsp_current_dma_table[52]
#define QDSP_DNNS_NoiseGammaN_SIZE                    16
#define QDSP_DNNS_NoiseGammaS                         (volatile uint16*)qdsp_current_dma_table[53]
#define QDSP_DNNS_NoiseGammaS_SIZE                    16
#define QDSP_DNNS_WbThreshold                         (volatile uint16*)qdsp_current_dma_table[54]
#define QDSP_DNNS_WbThreshold_SIZE                    16
#define QDSP_ErlStep                                  (volatile uint16*)qdsp_current_dma_table[55]
#define QDSP_ErlStep_SIZE                             16
#define QDSP_FixCalFactorLeft                         (volatile uint16*)qdsp_current_dma_table[56]
#define QDSP_FixCalFactorLeft_SIZE                    16
#define QDSP_FixCalFactorRight                        (volatile uint16*)qdsp_current_dma_table[57]
#define QDSP_FixCalFactorRight_SIZE                   16
#define QDSP_InputGain                                (volatile uint16*)qdsp_current_dma_table[58]
#define QDSP_InputGain_SIZE                           16
#define QDSP_MAX_NOISE_FLOOR                          (volatile uint16*)qdsp_current_dma_table[59]
#define QDSP_MAX_NOISE_FLOOR_SIZE                     16
#define QDSP_MinimumErl_par                           (volatile uint16*)qdsp_current_dma_table[60]
#define QDSP_MinimumErl_par_SIZE                      16
#define QDSP_MpThreshold                              (volatile uint16*)qdsp_current_dma_table[61]
#define QDSP_MpThreshold_SIZE                         16
#define QDSP_NLPP_gain                                (volatile uint16*)qdsp_current_dma_table[62]
#define QDSP_NLPP_gain_SIZE                           16
#define QDSP_NLPP_limit                               (volatile uint16*)qdsp_current_dma_table[63]
#define QDSP_NLPP_limit_SIZE                          16
#define QDSP_OutputGain                               (volatile uint16*)qdsp_current_dma_table[64]
#define QDSP_OutputGain_SIZE                          16
#define QDSP_PCD_offset                               (volatile uint16*)qdsp_current_dma_table[65]
#define QDSP_PCD_offset_SIZE                          16
#define QDSP_PCD_threshold                            (volatile uint16*)qdsp_current_dma_table[66]
#define QDSP_PCD_threshold_SIZE                       16
#define QDSP_PCD_twoalpha                             (volatile uint16*)qdsp_current_dma_table[67]
#define QDSP_PCD_twoalpha_SIZE                        16
#define QDSP_SF_init_table0                           (volatile uint16*)qdsp_current_dma_table[68]
#define QDSP_SF_init_table0_SIZE                      16
#define QDSP_SF_init_table1                           (volatile uint16*)qdsp_current_dma_table[69]
#define QDSP_SF_init_table1_SIZE                      16
#define QDSP_SF_taps                                  (volatile uint16*)qdsp_current_dma_table[70]
#define QDSP_SF_taps_SIZE                             16
#define QDSP_SF_twoalpha                              (volatile uint16*)qdsp_current_dma_table[71]
#define QDSP_SF_twoalpha_SIZE                         16
#define QDSP_SPDET_far                                (volatile uint16*)qdsp_current_dma_table[72]
#define QDSP_SPDET_far_SIZE                           16
#define QDSP_SPDET_mic                                (volatile uint16*)qdsp_current_dma_table[73]
#define QDSP_SPDET_mic_SIZE                           16
#define QDSP_SPDET_xclip                              (volatile uint16*)qdsp_current_dma_table[74]
#define QDSP_SPDET_xclip_SIZE                         16
#define QDSP_WB_Gamma_E                               (volatile uint16*)qdsp_current_dma_table[75]
#define QDSP_WB_Gamma_E_SIZE                          16
#define QDSP_WB_Gamma_NN                              (volatile uint16*)qdsp_current_dma_table[76]
#define QDSP_WB_Gamma_NN_SIZE                         16
#define QDSP_WB_Gamma_SN                              (volatile uint16*)qdsp_current_dma_table[77]
#define QDSP_WB_Gamma_SN_SIZE                         16
#define QDSP_WB_echo_ratio                            (volatile uint16*)qdsp_current_dma_table[78]
#define QDSP_WB_echo_ratio_SIZE                       16
#define QDSP_WB_echo_ratio_2mic                       (volatile uint16*)qdsp_current_dma_table[79]
#define QDSP_WB_echo_ratio_2mic_SIZE                  16
#define QDSP_WgThreshold                              (volatile uint16*)qdsp_current_dma_table[80]
#define QDSP_WgThreshold_SIZE                         16
#define QDSP_a3dExtRomBase                            (volatile uint32*)qdsp_current_dma_table[81]
#define QDSP_a3dExtRomBase_SIZE                       32
#define QDSP_a3dQxDirectGain                          (volatile uint16*)qdsp_current_dma_table[82]
#define QDSP_a3dQxDirectGain_SIZE                     16
#define QDSP_a3dQxEnable                              (volatile uint16*)qdsp_current_dma_table[83]
#define QDSP_a3dQxEnable_SIZE                         16
#define QDSP_a3dQxGeometry                            (volatile uint16*)qdsp_current_dma_table[84]
#define QDSP_a3dQxGeometry_SIZE                       16
#define QDSP_a3dQxOutputMode                          (volatile uint16*)qdsp_current_dma_table[85]
#define QDSP_a3dQxOutputMode_SIZE                     16
#define QDSP_a3dQxReset                               (volatile uint16*)qdsp_current_dma_table[86]
#define QDSP_a3dQxReset_SIZE                          16
#define QDSP_a3dQxSampRate                            (volatile uint16*)qdsp_current_dma_table[87]
#define QDSP_a3dQxSampRate_SIZE                       16
#define QDSP_a3dQxSpread                              (volatile uint16*)qdsp_current_dma_table[88]
#define QDSP_a3dQxSpread_SIZE                         16
#define QDSP_aacConcealFlag                           (volatile uint16*)qdsp_current_dma_table[89]
#define QDSP_aacConcealFlag_SIZE                      16
#define QDSP_aacFormatFlag                            (volatile uint16*)qdsp_current_dma_table[90]
#define QDSP_aacFormatFlag_SIZE                       16
#define QDSP_aacScalefactorDataResilienceFlag         (volatile uint16*)qdsp_current_dma_table[91]
#define QDSP_aacScalefactorDataResilienceFlag_SIZE    16
#define QDSP_aacSectionDataResilienceFlag             (volatile uint16*)qdsp_current_dma_table[92]
#define QDSP_aacSectionDataResilienceFlag_SIZE        16
#define QDSP_aacSpectralDataResilienceFlag            (volatile uint16*)qdsp_current_dma_table[93]
#define QDSP_aacSpectralDataResilienceFlag_SIZE       16
#define QDSP_adecAdpcmActive0                         (volatile uint16*)qdsp_current_dma_table[94]
#define QDSP_adecAdpcmActive0_SIZE                    16
#define QDSP_adecAdpcmActive1                         (volatile uint16*)qdsp_current_dma_table[95]
#define QDSP_adecAdpcmActive1_SIZE                    16
#define QDSP_adecAdpcmActive2                         (volatile uint16*)qdsp_current_dma_table[96]
#define QDSP_adecAdpcmActive2_SIZE                    16
#define QDSP_adecAdpcmActive3                         (volatile uint16*)qdsp_current_dma_table[97]
#define QDSP_adecAdpcmActive3_SIZE                    16
#define QDSP_adecAdpcmDataIsNeeded0                   (volatile uint16*)qdsp_current_dma_table[98]
#define QDSP_adecAdpcmDataIsNeeded0_SIZE              16
#define QDSP_adecAdpcmDataIsNeeded1                   (volatile uint16*)qdsp_current_dma_table[99]
#define QDSP_adecAdpcmDataIsNeeded1_SIZE              16
#define QDSP_adecAdpcmDataIsNeeded2                   (volatile uint16*)qdsp_current_dma_table[100]
#define QDSP_adecAdpcmDataIsNeeded2_SIZE              16
#define QDSP_adecAdpcmDataIsNeeded3                   (volatile uint16*)qdsp_current_dma_table[101]
#define QDSP_adecAdpcmDataIsNeeded3_SIZE              16
#define QDSP_adecAdpcmDataReadPtrOffset0              (volatile uint16*)qdsp_current_dma_table[102]
#define QDSP_adecAdpcmDataReadPtrOffset0_SIZE         16
#define QDSP_adecAdpcmDataReadPtrOffset1              (volatile uint16*)qdsp_current_dma_table[103]
#define QDSP_adecAdpcmDataReadPtrOffset1_SIZE         16
#define QDSP_adecAdpcmDataReadPtrOffset2              (volatile uint16*)qdsp_current_dma_table[104]
#define QDSP_adecAdpcmDataReadPtrOffset2_SIZE         16
#define QDSP_adecAdpcmDataReadPtrOffset3              (volatile uint16*)qdsp_current_dma_table[105]
#define QDSP_adecAdpcmDataReadPtrOffset3_SIZE         16
#define QDSP_adecChannelModeCh0                       (volatile uint16*)qdsp_current_dma_table[106]
#define QDSP_adecChannelModeCh0_SIZE                  16
#define QDSP_adecChannelModeCh1                       (volatile uint16*)qdsp_current_dma_table[107]
#define QDSP_adecChannelModeCh1_SIZE                  16
#define QDSP_adecChannelModeCh2                       (volatile uint16*)qdsp_current_dma_table[108]
#define QDSP_adecChannelModeCh2_SIZE                  16
#define QDSP_adecChannelModeCh3                       (volatile uint16*)qdsp_current_dma_table[109]
#define QDSP_adecChannelModeCh3_SIZE                  16
#define QDSP_adecDataBuf                              (volatile uint16*)qdsp_current_dma_table[110]
#define QDSP_adecDataBuf_SIZE                         16
#define QDSP_adecDataBufReadyFlag1Ch0                 (volatile uint16*)qdsp_current_dma_table[111]
#define QDSP_adecDataBufReadyFlag1Ch0_SIZE            16
#define QDSP_adecDataBufReadyFlag1Ch1                 (volatile uint16*)qdsp_current_dma_table[112]
#define QDSP_adecDataBufReadyFlag1Ch1_SIZE            16
#define QDSP_adecDataBufReadyFlag1Ch2                 (volatile uint16*)qdsp_current_dma_table[113]
#define QDSP_adecDataBufReadyFlag1Ch2_SIZE            16
#define QDSP_adecDataBufReadyFlag1Ch3                 (volatile uint16*)qdsp_current_dma_table[114]
#define QDSP_adecDataBufReadyFlag1Ch3_SIZE            16
#define QDSP_adecDataBufReadyFlag2Ch0                 (volatile uint16*)qdsp_current_dma_table[115]
#define QDSP_adecDataBufReadyFlag2Ch0_SIZE            16
#define QDSP_adecDataBufReadyFlag2Ch1                 (volatile uint16*)qdsp_current_dma_table[116]
#define QDSP_adecDataBufReadyFlag2Ch1_SIZE            16
#define QDSP_adecDataBufReadyFlag2Ch2                 (volatile uint16*)qdsp_current_dma_table[117]
#define QDSP_adecDataBufReadyFlag2Ch2_SIZE            16
#define QDSP_adecDataBufReadyFlag2Ch3                 (volatile uint16*)qdsp_current_dma_table[118]
#define QDSP_adecDataBufReadyFlag2Ch3_SIZE            16
#define QDSP_adecDataBufSize                          (volatile uint16*)qdsp_current_dma_table[119]
#define QDSP_adecDataBufSize_SIZE                     16
#define QDSP_adecDataBufSize1Ch0                      (volatile uint16*)qdsp_current_dma_table[120]
#define QDSP_adecDataBufSize1Ch0_SIZE                 16
#define QDSP_adecDataBufSize1Ch1                      (volatile uint16*)qdsp_current_dma_table[121]
#define QDSP_adecDataBufSize1Ch1_SIZE                 16
#define QDSP_adecDataBufSize1Ch2                      (volatile uint16*)qdsp_current_dma_table[122]
#define QDSP_adecDataBufSize1Ch2_SIZE                 16
#define QDSP_adecDataBufSize1Ch3                      (volatile uint16*)qdsp_current_dma_table[123]
#define QDSP_adecDataBufSize1Ch3_SIZE                 16
#define QDSP_adecDataBufSize2Ch0                      (volatile uint16*)qdsp_current_dma_table[124]
#define QDSP_adecDataBufSize2Ch0_SIZE                 16
#define QDSP_adecDataBufSize2Ch1                      (volatile uint16*)qdsp_current_dma_table[125]
#define QDSP_adecDataBufSize2Ch1_SIZE                 16
#define QDSP_adecDataBufSize2Ch2                      (volatile uint16*)qdsp_current_dma_table[126]
#define QDSP_adecDataBufSize2Ch2_SIZE                 16
#define QDSP_adecDataBufSize2Ch3                      (volatile uint16*)qdsp_current_dma_table[127]
#define QDSP_adecDataBufSize2Ch3_SIZE                 16
#define QDSP_adecDataBufStartMSW1Ch0                  (volatile uint16*)qdsp_current_dma_table[128]
#define QDSP_adecDataBufStartMSW1Ch0_SIZE             16
#define QDSP_adecDataBufStartMSW1Ch1                  (volatile uint16*)qdsp_current_dma_table[129]
#define QDSP_adecDataBufStartMSW1Ch1_SIZE             16
#define QDSP_adecDataBufStartMSW1Ch2                  (volatile uint16*)qdsp_current_dma_table[130]
#define QDSP_adecDataBufStartMSW1Ch2_SIZE             16
#define QDSP_adecDataBufStartMSW1Ch3                  (volatile uint16*)qdsp_current_dma_table[131]
#define QDSP_adecDataBufStartMSW1Ch3_SIZE             16
#define QDSP_adecDataBufStartMSW2Ch0                  (volatile uint16*)qdsp_current_dma_table[132]
#define QDSP_adecDataBufStartMSW2Ch0_SIZE             16
#define QDSP_adecDataBufStartMSW2Ch1                  (volatile uint16*)qdsp_current_dma_table[133]
#define QDSP_adecDataBufStartMSW2Ch1_SIZE             16
#define QDSP_adecDataBufStartMSW2Ch2                  (volatile uint16*)qdsp_current_dma_table[134]
#define QDSP_adecDataBufStartMSW2Ch2_SIZE             16
#define QDSP_adecDataBufStartMSW2Ch3                  (volatile uint16*)qdsp_current_dma_table[135]
#define QDSP_adecDataBufStartMSW2Ch3_SIZE             16
#define QDSP_adecDataFreeLen                          (volatile uint16*)qdsp_current_dma_table[136]
#define QDSP_adecDataFreeLen_SIZE                     16
#define QDSP_adecDataHostStickyInt                    (volatile uint16*)qdsp_current_dma_table[137]
#define QDSP_adecDataHostStickyInt_SIZE               16
#define QDSP_adecDataMinDLSize                        (volatile uint16*)qdsp_current_dma_table[138]
#define QDSP_adecDataMinDLSize_SIZE                   16
#define QDSP_adecDataWritePtr                         (volatile uint16*)qdsp_current_dma_table[139]
#define QDSP_adecDataWritePtr_SIZE                    16
#define QDSP_adecDecoderTypeCh0                       (volatile uint16*)qdsp_current_dma_table[140]
#define QDSP_adecDecoderTypeCh0_SIZE                  16
#define QDSP_adecDecoderTypeCh1                       (volatile uint16*)qdsp_current_dma_table[141]
#define QDSP_adecDecoderTypeCh1_SIZE                  16
#define QDSP_adecDecoderTypeCh2                       (volatile uint16*)qdsp_current_dma_table[142]
#define QDSP_adecDecoderTypeCh2_SIZE                  16
#define QDSP_adecDecoderTypeCh3                       (volatile uint16*)qdsp_current_dma_table[143]
#define QDSP_adecDecoderTypeCh3_SIZE                  16
#define QDSP_adecErrorReg                             (volatile uint16*)qdsp_current_dma_table[144]
#define QDSP_adecErrorReg_SIZE                        16
#define QDSP_adecFlush                                (volatile uint16*)qdsp_current_dma_table[145]
#define QDSP_adecFlush_SIZE                           16
#define QDSP_adecFlushProcessed                       (volatile uint16*)qdsp_current_dma_table[146]
#define QDSP_adecFlushProcessed_SIZE                  16
#define QDSP_adecFrameSize                            (volatile uint16*)qdsp_current_dma_table[147]
#define QDSP_adecFrameSize_SIZE                       16
#define QDSP_adecMVolume                              (volatile uint16*)qdsp_current_dma_table[148]
#define QDSP_adecMVolume_SIZE                         16
#define QDSP_adecPause                                (volatile uint16*)qdsp_current_dma_table[149]
#define QDSP_adecPause_SIZE                           16
#define QDSP_adecPauseFlagCh1                         (volatile uint16*)qdsp_current_dma_table[150]
#define QDSP_adecPauseFlagCh1_SIZE                    16
#define QDSP_adecPauseFlagCh2                         (volatile uint16*)qdsp_current_dma_table[151]
#define QDSP_adecPauseFlagCh2_SIZE                    16
#define QDSP_adecPauseFlagCh3                         (volatile uint16*)qdsp_current_dma_table[152]
#define QDSP_adecPauseFlagCh3_SIZE                    16
#define QDSP_adecReset                                (volatile uint16*)qdsp_current_dma_table[153]
#define QDSP_adecReset_SIZE                           16
#define QDSP_adecReset1                               (volatile uint16*)qdsp_current_dma_table[154]
#define QDSP_adecReset1_SIZE                          16
#define QDSP_adecReset2                               (volatile uint16*)qdsp_current_dma_table[155]
#define QDSP_adecReset2_SIZE                          16
#define QDSP_adecReset3                               (volatile uint16*)qdsp_current_dma_table[156]
#define QDSP_adecReset3_SIZE                          16
#define QDSP_adecStereoPan                            (volatile uint16*)qdsp_current_dma_table[157]
#define QDSP_adecStereoPan_SIZE                       16
#define QDSP_adecStereoPan1                           (volatile uint16*)qdsp_current_dma_table[158]
#define QDSP_adecStereoPan1_SIZE                      16
#define QDSP_adecStereoPan2                           (volatile uint16*)qdsp_current_dma_table[159]
#define QDSP_adecStereoPan2_SIZE                      16
#define QDSP_adecStereoPan3                           (volatile uint16*)qdsp_current_dma_table[160]
#define QDSP_adecStereoPan3_SIZE                      16
#define QDSP_adecStopFlagCh0                          (volatile uint16*)qdsp_current_dma_table[161]
#define QDSP_adecStopFlagCh0_SIZE                     16
#define QDSP_adecStopFlagCh1                          (volatile uint16*)qdsp_current_dma_table[162]
#define QDSP_adecStopFlagCh1_SIZE                     16
#define QDSP_adecStopFlagCh2                          (volatile uint16*)qdsp_current_dma_table[163]
#define QDSP_adecStopFlagCh2_SIZE                     16
#define QDSP_adecStopFlagCh3                          (volatile uint16*)qdsp_current_dma_table[164]
#define QDSP_adecStopFlagCh3_SIZE                     16
#define QDSP_adecVolume                               (volatile uint16*)qdsp_current_dma_table[165]
#define QDSP_adecVolume_SIZE                          16
#define QDSP_adecVolume1                              (volatile uint16*)qdsp_current_dma_table[166]
#define QDSP_adecVolume1_SIZE                         16
#define QDSP_adecVolume2                              (volatile uint16*)qdsp_current_dma_table[167]
#define QDSP_adecVolume2_SIZE                         16
#define QDSP_adecVolume3                              (volatile uint16*)qdsp_current_dma_table[168]
#define QDSP_adecVolume3_SIZE                         16
#define QDSP_adpcmDecoderType                         (volatile uint16*)qdsp_current_dma_table[169]
#define QDSP_adpcmDecoderType_SIZE                    16
#define QDSP_adpcmPacketBuf                           (volatile uint16*)qdsp_current_dma_table[170]
#define QDSP_adpcmPacketBuf_SIZE                      16
#define QDSP_adpcmPacketRate                          (volatile uint16*)qdsp_current_dma_table[171]
#define QDSP_adpcmPacketRate_SIZE                     16
#define QDSP_adpcmPacketReg                           (volatile uint16*)qdsp_current_dma_table[172]
#define QDSP_adpcmPacketReg_SIZE                      16
#define QDSP_adpcmVersion                             (volatile uint16*)qdsp_current_dma_table[173]
#define QDSP_adpcmVersion_SIZE                        16
#define QDSP_adpcmcodecRxGain                         (volatile uint16*)qdsp_current_dma_table[174]
#define QDSP_adpcmcodecRxGain_SIZE                    16
#define QDSP_adrcFlag                                 (volatile uint16*)qdsp_current_dma_table[175]
#define QDSP_adrcFlag_SIZE                            16
#define QDSP_adtsSamplingFrequency                    (volatile uint16*)qdsp_current_dma_table[176]
#define QDSP_adtsSamplingFrequency_SIZE               16
#define QDSP_audioObjectType                          (volatile uint16*)qdsp_current_dma_table[177]
#define QDSP_audioObjectType_SIZE                     16
#define QDSP_audioPause                               (volatile uint16*)qdsp_current_dma_table[178]
#define QDSP_audioPause_SIZE                          16
#define QDSP_audioPauseProcessing                     (volatile uint16*)qdsp_current_dma_table[179]
#define QDSP_audioPauseProcessing_SIZE                16
#define QDSP_autoSampSlipSwitch                       (volatile uint16*)qdsp_current_dma_table[180]
#define QDSP_autoSampSlipSwitch_SIZE                  16
#define QDSP_avcFlinkHeadroom                         (volatile uint16*)qdsp_current_dma_table[181]
#define QDSP_avcFlinkHeadroom_SIZE                    16
#define QDSP_avcFlinkSmoothK                          (volatile uint16*)qdsp_current_dma_table[182]
#define QDSP_avcFlinkSmoothK_SIZE                     16
#define QDSP_avcFlinkTargetGainList                   (volatile uint16*)qdsp_current_dma_table[183]
#define QDSP_avcFlinkTargetGainList_SIZE              16
#define QDSP_avcFlinkThresholdList                    (volatile uint16*)qdsp_current_dma_table[184]
#define QDSP_avcFlinkThresholdList_SIZE               16
#define QDSP_avcRlinkBNERamp                          (volatile uint16*)qdsp_current_dma_table[185]
#define QDSP_avcRlinkBNERamp_SIZE                     16
#define QDSP_avcRlinkSensitivityOffset                (volatile uint16*)qdsp_current_dma_table[186]
#define QDSP_avcRlinkSensitivityOffset_SIZE           16
#define QDSP_biosStateReg                             (volatile uint16*)qdsp_current_dma_table[187]
#define QDSP_biosStateReg_SIZE                        16
#define QDSP_biosuPCommandBuf                         (volatile uint16*)qdsp_current_dma_table[188]
#define QDSP_biosuPCommandBuf_SIZE                    16
#define QDSP_biosuPCommandReg                         (volatile uint16*)qdsp_current_dma_table[189]
#define QDSP_biosuPCommandReg_SIZE                    16
#define QDSP_block0                                   (volatile uint16*)qdsp_current_dma_table[190]
#define QDSP_block0_SIZE                              16
#define QDSP_channelConfiguration                     (volatile uint16*)qdsp_current_dma_table[191]
#define QDSP_channelConfiguration_SIZE                16
#define QDSP_chooseModeThreshold                      (volatile uint16*)qdsp_current_dma_table[192]
#define QDSP_chooseModeThreshold_SIZE                 16
#define QDSP_codecRxGain                              (volatile uint16*)qdsp_current_dma_table[193]
#define QDSP_codecRxGain_SIZE                         16
#define QDSP_codecRxOffsetL                           (volatile uint16*)qdsp_current_dma_table[194]
#define QDSP_codecRxOffsetL_SIZE                      16
#define QDSP_codecRxOffsetR                           (volatile uint16*)qdsp_current_dma_table[195]
#define QDSP_codecRxOffsetR_SIZE                      16
#define QDSP_codecSTGain                              (volatile uint16*)qdsp_current_dma_table[196]
#define QDSP_codecSTGain_SIZE                         16
#define QDSP_codecTxGain                              (volatile uint16*)qdsp_current_dma_table[197]
#define QDSP_codecTxGain_SIZE                         16
#define QDSP_compAttackK                              (volatile uint16*)qdsp_current_dma_table[198]
#define QDSP_compAttackK_SIZE                         16
#define QDSP_compDelay                                (volatile uint16*)qdsp_current_dma_table[199]
#define QDSP_compDelay_SIZE                           16
#define QDSP_compFlinkAIGAttackK                      (volatile uint16*)qdsp_current_dma_table[200]
#define QDSP_compFlinkAIGAttackK_SIZE                 16
#define QDSP_compFlinkAIGFlag                         (volatile uint16*)qdsp_current_dma_table[201]
#define QDSP_compFlinkAIGFlag_SIZE                    16
#define QDSP_compFlinkAIGLeakDown                     (volatile uint16*)qdsp_current_dma_table[202]
#define QDSP_compFlinkAIGLeakDown_SIZE                16
#define QDSP_compFlinkAIGLeakUp                       (volatile uint16*)qdsp_current_dma_table[203]
#define QDSP_compFlinkAIGLeakUp_SIZE                  16
#define QDSP_compFlinkAIGMax                          (volatile uint16*)qdsp_current_dma_table[204]
#define QDSP_compFlinkAIGMax_SIZE                     16
#define QDSP_compFlinkAIGMin                          (volatile uint16*)qdsp_current_dma_table[205]
#define QDSP_compFlinkAIGMin_SIZE                     16
#define QDSP_compFlinkAIGReleaseK                     (volatile uint16*)qdsp_current_dma_table[206]
#define QDSP_compFlinkAIGReleaseK_SIZE                16
#define QDSP_compFlinkAIGSmLeakRateFast               (volatile uint16*)qdsp_current_dma_table[207]
#define QDSP_compFlinkAIGSmLeakRateFast_SIZE          16
#define QDSP_compFlinkAIGSmLeakRateSlow               (volatile uint16*)qdsp_current_dma_table[208]
#define QDSP_compFlinkAIGSmLeakRateSlow_SIZE          16
#define QDSP_compFlinkAttackK                         (volatile uint16*)qdsp_current_dma_table[209]
#define QDSP_compFlinkAttackK_SIZE                    16
#define QDSP_compFlinkDelay                           (volatile uint16*)qdsp_current_dma_table[210]
#define QDSP_compFlinkDelay_SIZE                      16
#define QDSP_compFlinkReleaseK                        (volatile uint16*)qdsp_current_dma_table[211]
#define QDSP_compFlinkReleaseK_SIZE                   16
#define QDSP_compFlinkRmsTav                          (volatile uint16*)qdsp_current_dma_table[212]
#define QDSP_compFlinkRmsTav_SIZE                     16
#define QDSP_compFlinkSlope                           (volatile uint16*)qdsp_current_dma_table[213]
#define QDSP_compFlinkSlope_SIZE                      16
#define QDSP_compFlinkStaticGain                      (volatile uint16*)qdsp_current_dma_table[214]
#define QDSP_compFlinkStaticGain_SIZE                 16
#define QDSP_compFlinkThreshold                       (volatile uint16*)qdsp_current_dma_table[215]
#define QDSP_compFlinkThreshold_SIZE                  16
#define QDSP_compReleaseK                             (volatile uint16*)qdsp_current_dma_table[216]
#define QDSP_compReleaseK_SIZE                        16
#define QDSP_compRlinkAIGAttackK                      (volatile uint16*)qdsp_current_dma_table[217]
#define QDSP_compRlinkAIGAttackK_SIZE                 16
#define QDSP_compRlinkAIGFlag                         (volatile uint16*)qdsp_current_dma_table[218]
#define QDSP_compRlinkAIGFlag_SIZE                    16
#define QDSP_compRlinkAIGLeakDown                     (volatile uint16*)qdsp_current_dma_table[219]
#define QDSP_compRlinkAIGLeakDown_SIZE                16
#define QDSP_compRlinkAIGLeakUp                       (volatile uint16*)qdsp_current_dma_table[220]
#define QDSP_compRlinkAIGLeakUp_SIZE                  16
#define QDSP_compRlinkAIGMax                          (volatile uint16*)qdsp_current_dma_table[221]
#define QDSP_compRlinkAIGMax_SIZE                     16
#define QDSP_compRlinkAIGMin                          (volatile uint16*)qdsp_current_dma_table[222]
#define QDSP_compRlinkAIGMin_SIZE                     16
#define QDSP_compRlinkAIGReleaseK                     (volatile uint16*)qdsp_current_dma_table[223]
#define QDSP_compRlinkAIGReleaseK_SIZE                16
#define QDSP_compRlinkAIGSmLeakRateFast               (volatile uint16*)qdsp_current_dma_table[224]
#define QDSP_compRlinkAIGSmLeakRateFast_SIZE          16
#define QDSP_compRlinkAIGSmLeakRateSlow               (volatile uint16*)qdsp_current_dma_table[225]
#define QDSP_compRlinkAIGSmLeakRateSlow_SIZE          16
#define QDSP_compRlinkAttackK                         (volatile uint16*)qdsp_current_dma_table[226]
#define QDSP_compRlinkAttackK_SIZE                    16
#define QDSP_compRlinkDelay                           (volatile uint16*)qdsp_current_dma_table[227]
#define QDSP_compRlinkDelay_SIZE                      16
#define QDSP_compRlinkReleaseK                        (volatile uint16*)qdsp_current_dma_table[228]
#define QDSP_compRlinkReleaseK_SIZE                   16
#define QDSP_compRlinkRmsTav                          (volatile uint16*)qdsp_current_dma_table[229]
#define QDSP_compRlinkRmsTav_SIZE                     16
#define QDSP_compRlinkSlope                           (volatile uint16*)qdsp_current_dma_table[230]
#define QDSP_compRlinkSlope_SIZE                      16
#define QDSP_compRlinkStaticGain                      (volatile uint16*)qdsp_current_dma_table[231]
#define QDSP_compRlinkStaticGain_SIZE                 16
#define QDSP_compRlinkThreshold                       (volatile uint16*)qdsp_current_dma_table[232]
#define QDSP_compRlinkThreshold_SIZE                  16
#define QDSP_compRmsTav                               (volatile uint16*)qdsp_current_dma_table[233]
#define QDSP_compRmsTav_SIZE                          16
#define QDSP_compSlope                                (volatile uint16*)qdsp_current_dma_table[234]
#define QDSP_compSlope_SIZE                           16
#define QDSP_compThreshold                            (volatile uint16*)qdsp_current_dma_table[235]
#define QDSP_compThreshold_SIZE                       16
#define QDSP_csOutputGain                             (volatile uint16*)qdsp_current_dma_table[236]
#define QDSP_csOutputGain_SIZE                        16
#define QDSP_dcsCommandReg                            (volatile uint16*)qdsp_current_dma_table[237]
#define QDSP_dcsCommandReg_SIZE                       16
#define QDSP_decFlagWord                              (volatile uint16*)qdsp_current_dma_table[238]
#define QDSP_decFlagWord_SIZE                         16
#define QDSP_decMarginBin                             (volatile uint16*)qdsp_current_dma_table[239]
#define QDSP_decMarginBin_SIZE                        16
#define QDSP_decPacketBuf                             (volatile uint16*)qdsp_current_dma_table[240]
#define QDSP_decPacketBuf_SIZE                        16
#define QDSP_decPacketMiss                            (volatile uint16*)qdsp_current_dma_table[241]
#define QDSP_decPacketMiss_SIZE                       16
#define QDSP_decPacketRate                            (volatile uint16*)qdsp_current_dma_table[242]
#define QDSP_decPacketRate_SIZE                       16
#define QDSP_decPacketReg                             (volatile uint16*)qdsp_current_dma_table[243]
#define QDSP_decPacketReg_SIZE                        16
#define QDSP_decoderMemoPlaybackMux                   (volatile uint16*)qdsp_current_dma_table[244]
#define QDSP_decoderMemoPlaybackMux_SIZE              16
#define QDSP_dlsDrumDefBase                           (volatile uint32*)qdsp_current_dma_table[245]
#define QDSP_dlsDrumDefBase_SIZE                      32
#define QDSP_dlsHqDrumPtrPtr                          (volatile uint32*)qdsp_current_dma_table[246]
#define QDSP_dlsHqDrumPtrPtr_SIZE                     32
#define QDSP_dlsHqDrumPtrSize                         (volatile uint16*)qdsp_current_dma_table[247]
#define QDSP_dlsHqDrumPtrSize_SIZE                    16
#define QDSP_dlsHqProgPtrPtr                          (volatile uint32*)qdsp_current_dma_table[248]
#define QDSP_dlsHqProgPtrPtr_SIZE                     32
#define QDSP_dlsHqProgPtrSize                         (volatile uint16*)qdsp_current_dma_table[249]
#define QDSP_dlsHqProgPtrSize_SIZE                    16
#define QDSP_dlsHqWfInfoBase                          (volatile uint32*)qdsp_current_dma_table[250]
#define QDSP_dlsHqWfInfoBase_SIZE                     32
#define QDSP_dlsProgDefBase                           (volatile uint32*)qdsp_current_dma_table[251]
#define QDSP_dlsProgDefBase_SIZE                      32
#define QDSP_dlsWfInfoBase                            (volatile uint32*)qdsp_current_dma_table[252]
#define QDSP_dlsWfInfoBase_SIZE                       32
#define QDSP_dmNLPPgain                               (volatile uint16*)qdsp_current_dma_table[253]
#define QDSP_dmNLPPgain_SIZE                          16
#define QDSP_dmNLPPlimit                              (volatile uint16*)qdsp_current_dma_table[254]
#define QDSP_dmNLPPlimit_SIZE                         16
#define QDSP_dmeQueueBreakUpOverride                  (volatile uint16*)qdsp_current_dma_table[255]
#define QDSP_dmeQueueBreakUpOverride_SIZE             16
#define QDSP_dsState                                  (volatile uint16*)qdsp_current_dma_table[256]
#define QDSP_dsState_SIZE                             16
#define QDSP_dsVersion                                (volatile uint16*)qdsp_current_dma_table[257]
#define QDSP_dsVersion_SIZE                           16
#define QDSP_dspExtRamBase                            (volatile uint32*)qdsp_current_dma_table[258]
#define QDSP_dspExtRamBase_SIZE                       32
#define QDSP_dspExtRomBase                            (volatile uint32*)qdsp_current_dma_table[259]
#define QDSP_dspExtRomBase_SIZE                       32
#define QDSP_dspMessageBuf                            (volatile uint16*)qdsp_current_dma_table[260]
#define QDSP_dspMessageBuf_SIZE                       16
#define QDSP_dspMessageReg                            (volatile uint16*)qdsp_current_dma_table[261]
#define QDSP_dspMessageReg_SIZE                       16
#define QDSP_dspOutBuf1Ptr                            (volatile uint32*)qdsp_current_dma_table[262]
#define QDSP_dspOutBuf1Ptr_SIZE                       32
#define QDSP_dspOutBuf2Ptr                            (volatile uint32*)qdsp_current_dma_table[263]
#define QDSP_dspOutBuf2Ptr_SIZE                       32
#define QDSP_dtmfDetectSwitch                         (volatile uint16*)qdsp_current_dma_table[264]
#define QDSP_dtmfDetectSwitch_SIZE                    16
#define QDSP_dtmfGenPathSelect                        (volatile uint16*)qdsp_current_dma_table[265]
#define QDSP_dtmfGenPathSelect_SIZE                   16
#define QDSP_dtmfMixingOn                             (volatile uint16*)qdsp_current_dma_table[266]
#define QDSP_dtmfMixingOn_SIZE                        16
#define QDSP_ecModeNew                                (volatile uint16*)qdsp_current_dma_table[267]
#define QDSP_ecModeNew_SIZE                           16
#define QDSP_ecParametersUpdated                      (volatile uint16*)qdsp_current_dma_table[268]
#define QDSP_ecParametersUpdated_SIZE                 16
#define QDSP_echo_path_delay                          (volatile uint16*)qdsp_current_dma_table[269]
#define QDSP_echo_path_delay_SIZE                     16
#define QDSP_echo_path_delay_by_80                    (volatile uint16*)qdsp_current_dma_table[270]
#define QDSP_echo_path_delay_by_80_SIZE               16
#define QDSP_ecnsNewOrOld                             (volatile uint16*)qdsp_current_dma_table[271]
#define QDSP_ecnsNewOrOld_SIZE                        16
#define QDSP_eightHalfPixelSearchEnable               (volatile uint16*)qdsp_current_dma_table[272]
#define QDSP_eightHalfPixelSearchEnable_SIZE          16
#define QDSP_encCodecMode                             (volatile uint16*)qdsp_current_dma_table[273]
#define QDSP_encCodecMode_SIZE                        16
#define QDSP_encDtxMode                               (volatile uint16*)qdsp_current_dma_table[274]
#define QDSP_encDtxMode_SIZE                          16
#define QDSP_encFlagWord                              (volatile uint16*)qdsp_current_dma_table[275]
#define QDSP_encFlagWord_SIZE                         16
#define QDSP_encMaxRate                               (volatile uint16*)qdsp_current_dma_table[276]
#define QDSP_encMaxRate_SIZE                          16
#define QDSP_encMinRate                               (volatile uint16*)qdsp_current_dma_table[277]
#define QDSP_encMinRate_SIZE                          16
#define QDSP_encPacketBuf                             (volatile uint16*)qdsp_current_dma_table[278]
#define QDSP_encPacketBuf_SIZE                        16
#define QDSP_encPacketRate                            (volatile uint16*)qdsp_current_dma_table[279]
#define QDSP_encPacketRate_SIZE                       16
#define QDSP_encPacketReg                             (volatile uint16*)qdsp_current_dma_table[280]
#define QDSP_encPacketReg_SIZE                        16
#define QDSP_encVopReconBuf1Ptr                       (volatile uint32*)qdsp_current_dma_table[281]
#define QDSP_encVopReconBuf1Ptr_SIZE                  32
#define QDSP_encVopReconBuf2Ptr                       (volatile uint32*)qdsp_current_dma_table[282]
#define QDSP_encVopReconBuf2Ptr_SIZE                  32
#define QDSP_encodeHeight                             (volatile uint16*)qdsp_current_dma_table[283]
#define QDSP_encodeHeight_SIZE                        16
#define QDSP_encodeWidth                              (volatile uint16*)qdsp_current_dma_table[284]
#define QDSP_encodeWidth_SIZE                         16
#define QDSP_encodedFrameBuf1Ptr                      (volatile uint32*)qdsp_current_dma_table[285]
#define QDSP_encodedFrameBuf1Ptr_SIZE                 32
#define QDSP_encodedFrameBuf2Ptr                      (volatile uint32*)qdsp_current_dma_table[286]
#define QDSP_encodedFrameBuf2Ptr_SIZE                 32
#define QDSP_encodedFrameBufInd                       (volatile uint16*)qdsp_current_dma_table[287]
#define QDSP_encodedFrameBufInd_SIZE                  16
#define QDSP_encoderMuteOnTtyDetect                   (volatile uint16*)qdsp_current_dma_table[288]
#define QDSP_encoderMuteOnTtyDetect_SIZE              16
#define QDSP_enhanceMeuFilter1                        (volatile uint16*)qdsp_current_dma_table[289]
#define QDSP_enhanceMeuFilter1_SIZE                   16
#define QDSP_enhanceMeuFilter2                        (volatile uint16*)qdsp_current_dma_table[290]
#define QDSP_enhanceMeuFilter2_SIZE                   16
#define QDSP_epConfig                                 (volatile uint16*)qdsp_current_dma_table[291]
#define QDSP_epConfig_SIZE                            16
#define QDSP_expFlinkSlope                            (volatile uint16*)qdsp_current_dma_table[292]
#define QDSP_expFlinkSlope_SIZE                       16
#define QDSP_expFlinkThreshold                        (volatile uint16*)qdsp_current_dma_table[293]
#define QDSP_expFlinkThreshold_SIZE                   16
#define QDSP_expRlinkSlope                            (volatile uint16*)qdsp_current_dma_table[294]
#define QDSP_expRlinkSlope_SIZE                       16
#define QDSP_expRlinkThreshold                        (volatile uint16*)qdsp_current_dma_table[295]
#define QDSP_expRlinkThreshold_SIZE                   16
#define QDSP_filteredyuvBufPtr                        (volatile uint32*)qdsp_current_dma_table[296]
#define QDSP_filteredyuvBufPtr_SIZE                   32
#define QDSP_fixOverEstimation                        (volatile uint16*)qdsp_current_dma_table[297]
#define QDSP_fixOverEstimation_SIZE                   16
#define QDSP_frontHpfOverride                         (volatile uint16*)qdsp_current_dma_table[298]
#define QDSP_frontHpfOverride_SIZE                    16
#define QDSP_hostOutIntfActive                        (volatile uint16*)qdsp_current_dma_table[299]
#define QDSP_hostOutIntfActive_SIZE                   16
#define QDSP_hostOutIntfBufferFull                    (volatile uint16*)qdsp_current_dma_table[300]
#define QDSP_hostOutIntfBufferFull_SIZE               16
#define QDSP_hostOutIntfDSPReadPtr                    (volatile uint32*)qdsp_current_dma_table[301]
#define QDSP_hostOutIntfDSPReadPtr_SIZE               32
#define QDSP_hostOutIntfDSPWritePtr                   (volatile uint32*)qdsp_current_dma_table[302]
#define QDSP_hostOutIntfDSPWritePtr_SIZE              32
#define QDSP_hostOutIntfDataIsReady                   (volatile uint16*)qdsp_current_dma_table[303]
#define QDSP_hostOutIntfDataIsReady_SIZE              16
#define QDSP_hostOutIntfFrameCounter                  (volatile uint32*)qdsp_current_dma_table[304]
#define QDSP_hostOutIntfFrameCounter_SIZE             32
#define QDSP_hostOutIntfFrameSize                     (volatile uint16*)qdsp_current_dma_table[305]
#define QDSP_hostOutIntfFrameSize_SIZE                16
#define QDSP_hostOutIntfSBCActive                     (volatile uint16*)qdsp_current_dma_table[306]
#define QDSP_hostOutIntfSBCActive_SIZE                16
#define QDSP_hostOutIntfXferDone                      (volatile uint16*)qdsp_current_dma_table[307]
#define QDSP_hostOutIntfXferDone_SIZE                 16
#define QDSP_hybridMode                               (volatile uint16*)qdsp_current_dma_table[308]
#define QDSP_hybridMode_SIZE                          16
#define QDSP_iirFiltActiveFlag                        (volatile uint16*)qdsp_current_dma_table[309]
#define QDSP_iirFiltActiveFlag_SIZE                   16
#define QDSP_iirFiltCoeffExtRamBase                   (volatile uint32*)qdsp_current_dma_table[310]
#define QDSP_iirFiltCoeffExtRamBase_SIZE              32
#define QDSP_iirFiltNumBands                          (volatile uint16*)qdsp_current_dma_table[311]
#define QDSP_iirFiltNumBands_SIZE                     16
#define QDSP_iirFiltReset                             (volatile uint16*)qdsp_current_dma_table[312]
#define QDSP_iirFiltReset_SIZE                        16
#define QDSP_intraFrameCounter                        (volatile uint16*)qdsp_current_dma_table[313]
#define QDSP_intraFrameCounter_SIZE                   16
#define QDSP_intraFramePeriod                         (volatile uint16*)qdsp_current_dma_table[314]
#define QDSP_intraFramePeriod_SIZE                    16
#define QDSP_jpegdChromaAcHuffCodeCount               (volatile uint16*)qdsp_current_dma_table[315]
#define QDSP_jpegdChromaAcHuffCodeCount_SIZE          16
#define QDSP_jpegdChromaAcHuffCodeValue               (volatile uint16*)qdsp_current_dma_table[316]
#define QDSP_jpegdChromaAcHuffCodeValue_SIZE          16
#define QDSP_jpegdChromaDcHuffCodeCount               (volatile uint16*)qdsp_current_dma_table[317]
#define QDSP_jpegdChromaDcHuffCodeCount_SIZE          16
#define QDSP_jpegdChromaDcHuffCodeValue               (volatile uint16*)qdsp_current_dma_table[318]
#define QDSP_jpegdChromaDcHuffCodeValue_SIZE          16
#define QDSP_jpegdCommandBuf                          (volatile uint32*)qdsp_current_dma_table[319]
#define QDSP_jpegdCommandBuf_SIZE                     32
#define QDSP_jpegdCommandReg                          (volatile uint32*)qdsp_current_dma_table[320]
#define QDSP_jpegdCommandReg_SIZE                     32
#define QDSP_jpegdErrorSequence                       (volatile uint32*)qdsp_current_dma_table[321]
#define QDSP_jpegdErrorSequence_SIZE                  32
#define QDSP_jpegdErrorType                           (volatile uint32*)qdsp_current_dma_table[322]
#define QDSP_jpegdErrorType_SIZE                      32
#define QDSP_jpegdLumaAcHuffCodeCount                 (volatile uint16*)qdsp_current_dma_table[323]
#define QDSP_jpegdLumaAcHuffCodeCount_SIZE            16
#define QDSP_jpegdLumaAcHuffCodeValue                 (volatile uint16*)qdsp_current_dma_table[324]
#define QDSP_jpegdLumaAcHuffCodeValue_SIZE            16
#define QDSP_jpegdLumaDcHuffCodeCount                 (volatile uint16*)qdsp_current_dma_table[325]
#define QDSP_jpegdLumaDcHuffCodeCount_SIZE            16
#define QDSP_jpegdLumaDcHuffCodeValue                 (volatile uint16*)qdsp_current_dma_table[326]
#define QDSP_jpegdLumaDcHuffCodeValue_SIZE            16
#define QDSP_jpegdMsgIdleCompleteReg                  (volatile uint32*)qdsp_current_dma_table[327]
#define QDSP_jpegdMsgIdleCompleteReg_SIZE             32
#define QDSP_jpegdMsgIllegalCommandReg                (volatile uint32*)qdsp_current_dma_table[328]
#define QDSP_jpegdMsgIllegalCommandReg_SIZE           32
#define QDSP_jpegdMsgInputRequestReg                  (volatile uint32*)qdsp_current_dma_table[329]
#define QDSP_jpegdMsgInputRequestReg_SIZE             32
#define QDSP_jpegdMsgOutputProducedBuf                (volatile uint32*)qdsp_current_dma_table[330]
#define QDSP_jpegdMsgOutputProducedBuf_SIZE           32
#define QDSP_jpegdMsgOutputProducedReg                (volatile uint32*)qdsp_current_dma_table[331]
#define QDSP_jpegdMsgOutputProducedReg_SIZE           32
#define QDSP_jpegdQuantTableChroma                    (volatile uint16*)qdsp_current_dma_table[332]
#define QDSP_jpegdQuantTableChroma_SIZE               16
#define QDSP_jpegdQuantTableLuma                      (volatile uint16*)qdsp_current_dma_table[333]
#define QDSP_jpegdQuantTableLuma_SIZE                 16
#define QDSP_jpegdState                               (volatile uint16*)qdsp_current_dma_table[334]
#define QDSP_jpegdState_SIZE                          16
#define QDSP_jpegdVersion                             (volatile uint32*)qdsp_current_dma_table[335]
#define QDSP_jpegdVersion_SIZE                        32
#define QDSP_jpegdWaitIndicatorFlag                   (volatile uint16*)qdsp_current_dma_table[336]
#define QDSP_jpegdWaitIndicatorFlag_SIZE              16
#define QDSP_jpegeCommandBuf                          (volatile uint32*)qdsp_current_dma_table[337]
#define QDSP_jpegeCommandBuf_SIZE                     32
#define QDSP_jpegeCommandReg                          (volatile uint32*)qdsp_current_dma_table[338]
#define QDSP_jpegeCommandReg_SIZE                     32
#define QDSP_jpegeLpmVersion                          (volatile uint32*)qdsp_current_dma_table[339]
#define QDSP_jpegeLpmVersion_SIZE                     32
#define QDSP_jpegeMcuFragIndex                        (volatile uint16*)qdsp_current_dma_table[340]
#define QDSP_jpegeMcuFragIndex_SIZE                   16
#define QDSP_jpegeMcuHoriIndex                        (volatile uint16*)qdsp_current_dma_table[341]
#define QDSP_jpegeMcuHoriIndex_SIZE                   16
#define QDSP_jpegeMcuVertiIndex                       (volatile uint16*)qdsp_current_dma_table[342]
#define QDSP_jpegeMcuVertiIndex_SIZE                  16
#define QDSP_jpegeMsgIdleCompleteReg                  (volatile uint32*)qdsp_current_dma_table[343]
#define QDSP_jpegeMsgIdleCompleteReg_SIZE             32
#define QDSP_jpegeMsgIllegalCommandReg                (volatile uint32*)qdsp_current_dma_table[344]
#define QDSP_jpegeMsgIllegalCommandReg_SIZE           32
#define QDSP_jpegeMsgOutputProducedBuf                (volatile uint32*)qdsp_current_dma_table[345]
#define QDSP_jpegeMsgOutputProducedBuf_SIZE           32
#define QDSP_jpegeMsgOutputProducedReg                (volatile uint32*)qdsp_current_dma_table[346]
#define QDSP_jpegeMsgOutputProducedReg_SIZE           32
#define QDSP_jpegeNumBitsHuffDc                       (volatile uint32*)qdsp_current_dma_table[347]
#define QDSP_jpegeNumBitsHuffDc_SIZE                  32
#define QDSP_jpegeQuantTableChroma                    (volatile uint16*)qdsp_current_dma_table[348]
#define QDSP_jpegeQuantTableChroma_SIZE               16
#define QDSP_jpegeQuantTableLuma                      (volatile uint16*)qdsp_current_dma_table[349]
#define QDSP_jpegeQuantTableLuma_SIZE                 16
#define QDSP_jpegeState                               (volatile uint16*)qdsp_current_dma_table[350]
#define QDSP_jpegeState_SIZE                          16
#define QDSP_jpegeVideoDoneStatus                     (volatile uint16*)qdsp_current_dma_table[351]
#define QDSP_jpegeVideoDoneStatus_SIZE                16
#define QDSP_lensRolloffResetWaitCycles               (volatile uint16*)qdsp_current_dma_table[352]
#define QDSP_lensRolloffResetWaitCycles_SIZE          16
#define QDSP_lpmCommandBuf                            (volatile uint32*)qdsp_current_dma_table[353]
#define QDSP_lpmCommandBuf_SIZE                       32
#define QDSP_lpmCommandReg                            (volatile uint32*)qdsp_current_dma_table[354]
#define QDSP_lpmCommandReg_SIZE                       32
#define QDSP_lpmLumaHistogram                         (volatile uint32*)qdsp_current_dma_table[355]
#define QDSP_lpmLumaHistogram_SIZE                    32
#define QDSP_lpmMsgDataReadyReg                       (volatile uint32*)qdsp_current_dma_table[356]
#define QDSP_lpmMsgDataReadyReg_SIZE                  32
#define QDSP_lpmMsgIdleCommandCompleteReg             (volatile uint32*)qdsp_current_dma_table[357]
#define QDSP_lpmMsgIdleCommandCompleteReg_SIZE        32
#define QDSP_lpmMsgIllegalCommandReg                  (volatile uint32*)qdsp_current_dma_table[358]
#define QDSP_lpmMsgIllegalCommandReg_SIZE             32
#define QDSP_lpmState                                 (volatile uint16*)qdsp_current_dma_table[359]
#define QDSP_lpmState_SIZE                            16
#define QDSP_lpmVignetteCorrectFactorTable            (volatile uint32*)qdsp_current_dma_table[360]
#define QDSP_lpmVignetteCorrectFactorTable_SIZE       32
#define QDSP_lpmVignetteSlopeAndRadiusTable           (volatile uint32*)qdsp_current_dma_table[361]
#define QDSP_lpmVignetteSlopeAndRadiusTable_SIZE      32
#define QDSP_mainChainGain                            (volatile uint16*)qdsp_current_dma_table[362]
#define QDSP_mainChainGain_SIZE                       16
#define QDSP_masterPan                                (volatile uint16*)qdsp_current_dma_table[363]
#define QDSP_masterPan_SIZE                           16
#define QDSP_masterVolume                             (volatile uint16*)qdsp_current_dma_table[364]
#define QDSP_masterVolume_SIZE                        16
#define QDSP_maxMESearches                            (volatile uint16*)qdsp_current_dma_table[365]
#define QDSP_maxMESearches_SIZE                       16
#define QDSP_mbEqActiveFlag                           (volatile uint16*)qdsp_current_dma_table[366]
#define QDSP_mbEqActiveFlag_SIZE                      16
#define QDSP_mbEqCoeffExtRamBase                      (volatile uint32*)qdsp_current_dma_table[367]
#define QDSP_mbEqCoeffExtRamBase_SIZE                 32
#define QDSP_mbEqNumBands                             (volatile uint16*)qdsp_current_dma_table[368]
#define QDSP_mbEqNumBands_SIZE                        16
#define QDSP_mbEqReset                                (volatile uint16*)qdsp_current_dma_table[369]
#define QDSP_mbEqReset_SIZE                           16
#define QDSP_memoPlaybackTxMux                        (volatile uint16*)qdsp_current_dma_table[370]
#define QDSP_memoPlaybackTxMux_SIZE                   16
#define QDSP_midiDecoderType                          (volatile uint16*)qdsp_current_dma_table[371]
#define QDSP_midiDecoderType_SIZE                     16
#define QDSP_midiMessageBuf                           (volatile uint16*)qdsp_current_dma_table[372]
#define QDSP_midiMessageBuf_SIZE                      16
#define QDSP_mp3SamplingRate                          (volatile uint16*)qdsp_current_dma_table[373]
#define QDSP_mp3SamplingRate_SIZE                     16
#define QDSP_mp4eFrameRhoTable                        (volatile uint32*)qdsp_current_dma_table[374]
#define QDSP_mp4eFrameRhoTable_SIZE                   32
#define QDSP_mp4eRateCtrlQpHighRange                  (volatile uint16*)qdsp_current_dma_table[375]
#define QDSP_mp4eRateCtrlQpHighRange_SIZE             16
#define QDSP_mp4eRateCtrlQpLowRange                   (volatile uint16*)qdsp_current_dma_table[376]
#define QDSP_mp4eRateCtrlQpLowRange_SIZE              16
#define QDSP_mp4eZeroBlockChromaThreshold             (volatile uint16*)qdsp_current_dma_table[377]
#define QDSP_mp4eZeroBlockChromaThreshold_SIZE        16
#define QDSP_mp4eZeroBlockLumaThreshold               (volatile uint16*)qdsp_current_dma_table[378]
#define QDSP_mp4eZeroBlockLumaThreshold_SIZE          16
#define QDSP_mvSearchBestSSD                          (volatile uint32*)qdsp_current_dma_table[379]
#define QDSP_mvSearchBestSSD_SIZE                     32
#define QDSP_mvSearchesCompleted                      (volatile uint16*)qdsp_current_dma_table[380]
#define QDSP_mvSearchesCompleted_SIZE                 16
#define QDSP_pcmDmaMissed                             (volatile uint16*)qdsp_current_dma_table[381]
#define QDSP_pcmDmaMissed_SIZE                        16
#define QDSP_pcmFiltCoeff                             (volatile uint16*)qdsp_current_dma_table[382]
#define QDSP_pcmFiltCoeff_SIZE                        16
#define QDSP_pcmFiltCompFactor                        (volatile uint16*)qdsp_current_dma_table[383]
#define QDSP_pcmFiltCompFactor_SIZE                   16
#define QDSP_pcmFiltFlag                              (volatile uint16*)qdsp_current_dma_table[384]
#define QDSP_pcmFiltFlag_SIZE                         16
#define QDSP_pcmFiltLength                            (volatile uint16*)qdsp_current_dma_table[385]
#define QDSP_pcmFiltLength_SIZE                       16
#define QDSP_qcamcorderVersion                        (volatile uint16*)qdsp_current_dma_table[386]
#define QDSP_qcamcorderVersion_SIZE                   16
#define QDSP_qdspState                                (volatile uint16*)qdsp_current_dma_table[387]
#define QDSP_qdspState_SIZE                           16
#define QDSP_qdspVersion                              (volatile uint16*)qdsp_current_dma_table[388]
#define QDSP_qdspVersion_SIZE                         16
#define QDSP_qsynthVersion                            (volatile uint16*)qdsp_current_dma_table[389]
#define QDSP_qsynthVersion_SIZE                       16
#define QDSP_quantOutput                              (volatile uint16*)qdsp_current_dma_table[390]
#define QDSP_quantOutput_SIZE                         16
#define QDSP_quantizerScaleInter                      (volatile uint16*)qdsp_current_dma_table[391]
#define QDSP_quantizerScaleInter_SIZE                 16
#define QDSP_quantizerScaleIntra                      (volatile uint16*)qdsp_current_dma_table[392]
#define QDSP_quantizerScaleIntra_SIZE                 16
#define QDSP_ramVersion                               (volatile uint16*)qdsp_current_dma_table[393]
#define QDSP_ramVersion_SIZE                          16
#define QDSP_rateModulationCmd                        (volatile uint16*)qdsp_current_dma_table[394]
#define QDSP_rateModulationCmd_SIZE                   16
#define QDSP_rcvdPadWord                              (volatile uint16*)qdsp_current_dma_table[395]
#define QDSP_rcvdPadWord_SIZE                         16
#define QDSP_reducedRateLevel                         (volatile uint16*)qdsp_current_dma_table[396]
#define QDSP_reducedRateLevel_SIZE                    16
#define QDSP_reinitVocBNE                             (volatile uint16*)qdsp_current_dma_table[397]
#define QDSP_reinitVocBNE_SIZE                        16
#define QDSP_requestNotCoded                          (volatile uint16*)qdsp_current_dma_table[398]
#define QDSP_requestNotCoded_SIZE                     16
#define QDSP_romVersion                               (volatile uint16*)qdsp_current_dma_table[399]
#define QDSP_romVersion_SIZE                          16
#define QDSP_rotationFlag                             (volatile uint16*)qdsp_current_dma_table[400]
#define QDSP_rotationFlag_SIZE                        16
#define QDSP_rxAgcEnableFlag                          (volatile uint16*)qdsp_current_dma_table[401]
#define QDSP_rxAgcEnableFlag_SIZE                     16
#define QDSP_rxAvcEnableFlag                          (volatile uint16*)qdsp_current_dma_table[402]
#define QDSP_rxAvcEnableFlag_SIZE                     16
#define QDSP_rxBudget                                 (volatile uint16*)qdsp_current_dma_table[403]
#define QDSP_rxBudget_SIZE                            16
#define QDSP_rxDtmfConfigBuf                          (volatile uint16*)qdsp_current_dma_table[404]
#define QDSP_rxDtmfConfigBuf_SIZE                     16
#define QDSP_rxDtmfGenGain                            (volatile uint16*)qdsp_current_dma_table[405]
#define QDSP_rxDtmfGenGain_SIZE                       16
#define QDSP_rxIirPcmFilterSwitch                     (volatile uint16*)qdsp_current_dma_table[406]
#define QDSP_rxIirPcmFilterSwitch_SIZE                16
#define QDSP_rxMute                                   (volatile uint16*)qdsp_current_dma_table[407]
#define QDSP_rxMute_SIZE                              16
#define QDSP_rxPcmFiltCoeff                           (volatile uint16*)qdsp_current_dma_table[408]
#define QDSP_rxPcmFiltCoeff_SIZE                      16
#define QDSP_rxPcmFiltLoad                            (volatile uint16*)qdsp_current_dma_table[409]
#define QDSP_rxPcmFiltLoad_SIZE                       16
#define QDSP_rxPcmIirACoeff1                          (volatile uint16*)qdsp_current_dma_table[410]
#define QDSP_rxPcmIirACoeff1_SIZE                     16
#define QDSP_rxPcmIirACoeff2                          (volatile uint16*)qdsp_current_dma_table[411]
#define QDSP_rxPcmIirACoeff2_SIZE                     16
#define QDSP_rxPcmIirACoeff3                          (volatile uint16*)qdsp_current_dma_table[412]
#define QDSP_rxPcmIirACoeff3_SIZE                     16
#define QDSP_rxPcmIirBCoeff1                          (volatile uint16*)qdsp_current_dma_table[413]
#define QDSP_rxPcmIirBCoeff1_SIZE                     16
#define QDSP_rxPcmIirBCoeff2                          (volatile uint16*)qdsp_current_dma_table[414]
#define QDSP_rxPcmIirBCoeff2_SIZE                     16
#define QDSP_rxPcmIirBCoeff3                          (volatile uint16*)qdsp_current_dma_table[415]
#define QDSP_rxPcmIirBCoeff3_SIZE                     16
#define QDSP_rxPcmRdBuf                               (volatile uint16*)qdsp_current_dma_table[416]
#define QDSP_rxPcmRdBuf_SIZE                          16
#define QDSP_rxPcmRdEnable                            (volatile uint16*)qdsp_current_dma_table[417]
#define QDSP_rxPcmRdEnable_SIZE                       16
#define QDSP_rxPcmRdMiss                              (volatile uint16*)qdsp_current_dma_table[418]
#define QDSP_rxPcmRdMiss_SIZE                         16
#define QDSP_rxPcmRdReg                               (volatile uint16*)qdsp_current_dma_table[419]
#define QDSP_rxPcmRdReg_SIZE                          16
#define QDSP_rxPcmWrBuf                               (volatile uint16*)qdsp_current_dma_table[420]
#define QDSP_rxPcmWrBuf_SIZE                          16
#define QDSP_rxPcmWrEnable                            (volatile uint16*)qdsp_current_dma_table[421]
#define QDSP_rxPcmWrEnable_SIZE                       16
#define QDSP_rxPcmWrMiss                              (volatile uint16*)qdsp_current_dma_table[422]
#define QDSP_rxPcmWrMiss_SIZE                         16
#define QDSP_rxPcmWrReg                               (volatile uint16*)qdsp_current_dma_table[423]
#define QDSP_rxPcmWrReg_SIZE                          16
#define QDSP_rxToneStatus                             (volatile uint16*)qdsp_current_dma_table[424]
#define QDSP_rxToneStatus_SIZE                        16
#define QDSP_rxVolume                                 (volatile uint16*)qdsp_current_dma_table[425]
#define QDSP_rxVolume_SIZE                            16
#define QDSP_sampSlipCnt                              (volatile uint16*)qdsp_current_dma_table[426]
#define QDSP_sampSlipCnt_SIZE                         16
#define QDSP_sampStuffCnt                             (volatile uint16*)qdsp_current_dma_table[427]
#define QDSP_sampStuffCnt_SIZE                        16
#define QDSP_sbcBitRate                               (volatile uint32*)qdsp_current_dma_table[428]
#define QDSP_sbcBitRate_SIZE                          32
#define QDSP_sbrOnFlag                                (volatile uint16*)qdsp_current_dma_table[429]
#define QDSP_sbrOnFlag_SIZE                           16
#define QDSP_sbrPsOnFlag                              (volatile uint16*)qdsp_current_dma_table[430]
#define QDSP_sbrPsOnFlag_SIZE                         16
#define QDSP_secondDecPacketBuf                       (volatile uint16*)qdsp_current_dma_table[431]
#define QDSP_secondDecPacketBuf_SIZE                  16
#define QDSP_secondDecPacketRate                      (volatile uint16*)qdsp_current_dma_table[432]
#define QDSP_secondDecPacketRate_SIZE                 16
#define QDSP_secondDecoderConfig                      (volatile uint16*)qdsp_current_dma_table[433]
#define QDSP_secondDecoderConfig_SIZE                 16
#define QDSP_seqMVolume                               (volatile uint16*)qdsp_current_dma_table[434]
#define QDSP_seqMVolume_SIZE                          16
#define QDSP_spaCoeffReady                            (volatile uint16*)qdsp_current_dma_table[435]
#define QDSP_spaCoeffReady_SIZE                       16
#define QDSP_spaCoefficientBuf                        (volatile uint16*)qdsp_current_dma_table[436]
#define QDSP_spaCoefficientBuf_SIZE                   16
#define QDSP_spaSampleInterval                        (volatile uint16*)qdsp_current_dma_table[437]
#define QDSP_spaSampleInterval_SIZE                   16
#define QDSP_ssMajorRev                               (volatile uint16*)qdsp_current_dma_table[438]
#define QDSP_ssMajorRev_SIZE                          16
#define QDSP_ssMinorRev                               (volatile uint16*)qdsp_current_dma_table[439]
#define QDSP_ssMinorRev_SIZE                          16
#define QDSP_stfActiveFlag                            (volatile uint16*)qdsp_current_dma_table[440]
#define QDSP_stfActiveFlag_SIZE                       16
#define QDSP_stfCoeffExtRamBase                       (volatile uint32*)qdsp_current_dma_table[441]
#define QDSP_stfCoeffExtRamBase_SIZE                  32
#define QDSP_stfNumBands                              (volatile uint16*)qdsp_current_dma_table[442]
#define QDSP_stfNumBands_SIZE                         16
#define QDSP_stfPreGain                               (volatile uint16*)qdsp_current_dma_table[443]
#define QDSP_stfPreGain_SIZE                          16
#define QDSP_stfReset                                 (volatile uint16*)qdsp_current_dma_table[444]
#define QDSP_stfReset_SIZE                            16
#define QDSP_synthMVolume                             (volatile uint16*)qdsp_current_dma_table[445]
#define QDSP_synthMVolume_SIZE                        16
#define QDSP_targetHeader                             (volatile uint16*)qdsp_current_dma_table[446]
#define QDSP_targetHeader_SIZE                        16
#define QDSP_targetHeaderMask                         (volatile uint16*)qdsp_current_dma_table[447]
#define QDSP_targetHeaderMask_SIZE                    16
#define QDSP_ttyEncFlag                               (volatile uint16*)qdsp_current_dma_table[448]
#define QDSP_ttyEncFlag_SIZE                          16
#define QDSP_ttyNoGainFlag                            (volatile uint16*)qdsp_current_dma_table[449]
#define QDSP_ttyNoGainFlag_SIZE                       16
#define QDSP_ttyToneGain                              (volatile uint16*)qdsp_current_dma_table[450]
#define QDSP_ttyToneGain_SIZE                         16
#define QDSP_ttyWriteFlag                             (volatile uint16*)qdsp_current_dma_table[451]
#define QDSP_ttyWriteFlag_SIZE                        16
#define QDSP_tuning_mode                              (volatile uint16*)qdsp_current_dma_table[452]
#define QDSP_tuning_mode_SIZE                         16
#define QDSP_txAgcEnableFlag                          (volatile uint16*)qdsp_current_dma_table[453]
#define QDSP_txAgcEnableFlag_SIZE                     16
#define QDSP_txComfortNoiseEnable                     (volatile uint16*)qdsp_current_dma_table[454]
#define QDSP_txComfortNoiseEnable_SIZE                16
#define QDSP_txDtmfConfigBuf                          (volatile uint16*)qdsp_current_dma_table[455]
#define QDSP_txDtmfConfigBuf_SIZE                     16
#define QDSP_txIirPcmFilterSwitch                     (volatile uint16*)qdsp_current_dma_table[456]
#define QDSP_txIirPcmFilterSwitch_SIZE                16
#define QDSP_txMargin                                 (volatile uint16*)qdsp_current_dma_table[457]
#define QDSP_txMargin_SIZE                            16
#define QDSP_txPcmFiltCoeff                           (volatile uint16*)qdsp_current_dma_table[458]
#define QDSP_txPcmFiltCoeff_SIZE                      16
#define QDSP_txPcmFiltLoad                            (volatile uint16*)qdsp_current_dma_table[459]
#define QDSP_txPcmFiltLoad_SIZE                       16
#define QDSP_txPcmFilterBeforeEC                      (volatile uint16*)qdsp_current_dma_table[460]
#define QDSP_txPcmFilterBeforeEC_SIZE                 16
#define QDSP_txPcmIirACoeff1                          (volatile uint16*)qdsp_current_dma_table[461]
#define QDSP_txPcmIirACoeff1_SIZE                     16
#define QDSP_txPcmIirACoeff2                          (volatile uint16*)qdsp_current_dma_table[462]
#define QDSP_txPcmIirACoeff2_SIZE                     16
#define QDSP_txPcmIirACoeff3                          (volatile uint16*)qdsp_current_dma_table[463]
#define QDSP_txPcmIirACoeff3_SIZE                     16
#define QDSP_txPcmIirACoeff4                          (volatile uint16*)qdsp_current_dma_table[464]
#define QDSP_txPcmIirACoeff4_SIZE                     16
#define QDSP_txPcmIirACoeff5                          (volatile uint16*)qdsp_current_dma_table[465]
#define QDSP_txPcmIirACoeff5_SIZE                     16
#define QDSP_txPcmIirBCoeff1                          (volatile uint16*)qdsp_current_dma_table[466]
#define QDSP_txPcmIirBCoeff1_SIZE                     16
#define QDSP_txPcmIirBCoeff2                          (volatile uint16*)qdsp_current_dma_table[467]
#define QDSP_txPcmIirBCoeff2_SIZE                     16
#define QDSP_txPcmIirBCoeff3                          (volatile uint16*)qdsp_current_dma_table[468]
#define QDSP_txPcmIirBCoeff3_SIZE                     16
#define QDSP_txPcmIirBCoeff4                          (volatile uint16*)qdsp_current_dma_table[469]
#define QDSP_txPcmIirBCoeff4_SIZE                     16
#define QDSP_txPcmIirBCoeff5                          (volatile uint16*)qdsp_current_dma_table[470]
#define QDSP_txPcmIirBCoeff5_SIZE                     16
#define QDSP_txPcmRdBuf                               (volatile uint16*)qdsp_current_dma_table[471]
#define QDSP_txPcmRdBuf_SIZE                          16
#define QDSP_txPcmRdEnable                            (volatile uint16*)qdsp_current_dma_table[472]
#define QDSP_txPcmRdEnable_SIZE                       16
#define QDSP_txPcmRdMiss                              (volatile uint16*)qdsp_current_dma_table[473]
#define QDSP_txPcmRdMiss_SIZE                         16
#define QDSP_txPcmRdReg                               (volatile uint16*)qdsp_current_dma_table[474]
#define QDSP_txPcmRdReg_SIZE                          16
#define QDSP_txPcmWrBuf                               (volatile uint16*)qdsp_current_dma_table[475]
#define QDSP_txPcmWrBuf_SIZE                          16
#define QDSP_txPcmWrEnable                            (volatile uint16*)qdsp_current_dma_table[476]
#define QDSP_txPcmWrEnable_SIZE                       16
#define QDSP_txPcmWrMiss                              (volatile uint16*)qdsp_current_dma_table[477]
#define QDSP_txPcmWrMiss_SIZE                         16
#define QDSP_txPcmWrReg                               (volatile uint16*)qdsp_current_dma_table[478]
#define QDSP_txPcmWrReg_SIZE                          16
#define QDSP_txToneMuteFlag                           (volatile uint16*)qdsp_current_dma_table[479]
#define QDSP_txToneMuteFlag_SIZE                      16
#define QDSP_txToneStatus                             (volatile uint16*)qdsp_current_dma_table[480]
#define QDSP_txToneStatus_SIZE                        16
#define QDSP_txVolume                                 (volatile uint16*)qdsp_current_dma_table[481]
#define QDSP_txVolume_SIZE                            16
#define QDSP_uPAppletCmdReg                           (volatile uint16*)qdsp_current_dma_table[482]
#define QDSP_uPAppletCmdReg_SIZE                      16
#define QDSP_uPCommandBuf                             (volatile uint16*)qdsp_current_dma_table[483]
#define QDSP_uPCommandBuf_SIZE                        16
#define QDSP_uPCommandReg                             (volatile uint16*)qdsp_current_dma_table[484]
#define QDSP_uPCommandReg_SIZE                        16
#define QDSP_uPContextCacheFlush                      (volatile uint16*)qdsp_current_dma_table[485]
#define QDSP_uPContextCacheFlush_SIZE                 16
#define QDSP_uPContextCacheHit                        (volatile uint16*)qdsp_current_dma_table[486]
#define QDSP_uPContextCacheHit_SIZE                   16
#define QDSP_uPContextCacheMiss                       (volatile uint16*)qdsp_current_dma_table[487]
#define QDSP_uPContextCacheMiss_SIZE                  16
#define QDSP_uPCryptoCmdProgress                      (volatile uint16*)qdsp_current_dma_table[488]
#define QDSP_uPCryptoCmdProgress_SIZE                 16
#define QDSP_uPCryptoRetStatusFlag                    (volatile uint16*)qdsp_current_dma_table[489]
#define QDSP_uPCryptoRetStatusFlag_SIZE               16
#define QDSP_uPProgMemCacheHit                        (volatile uint16*)qdsp_current_dma_table[490]
#define QDSP_uPProgMemCacheHit_SIZE                   16
#define QDSP_uPProgMemCacheMiss                       (volatile uint16*)qdsp_current_dma_table[491]
#define QDSP_uPProgMemCacheMiss_SIZE                  16
#define QDSP_upDmaPathControl                         (volatile uint16*)qdsp_current_dma_table[492]
#define QDSP_upDmaPathControl_SIZE                    16
#define QDSP_vcodec_coeff0                            (volatile uint32*)qdsp_current_dma_table[493]
#define QDSP_vcodec_coeff0_SIZE                       32
#define QDSP_vcodec_coeff1                            (volatile uint32*)qdsp_current_dma_table[494]
#define QDSP_vcodec_coeff1_SIZE                       32
#define QDSP_vcodec_delay0                            (volatile uint16*)qdsp_current_dma_table[495]
#define QDSP_vcodec_delay0_SIZE                       16
#define QDSP_vcodec_delay1                            (volatile uint16*)qdsp_current_dma_table[496]
#define QDSP_vcodec_delay1_SIZE                       16
#define QDSP_vcodec_len0                              (volatile uint16*)qdsp_current_dma_table[497]
#define QDSP_vcodec_len0_SIZE                         16
#define QDSP_vcodec_len1                              (volatile uint16*)qdsp_current_dma_table[498]
#define QDSP_vcodec_len1_SIZE                         16
#define QDSP_vcodec_thr0                              (volatile uint16*)qdsp_current_dma_table[499]
#define QDSP_vcodec_thr0_SIZE                         16
#define QDSP_vcodec_thr1                              (volatile uint16*)qdsp_current_dma_table[500]
#define QDSP_vcodec_thr1_SIZE                         16
#define QDSP_vfeCamifErrorCount                       (volatile uint16*)qdsp_current_dma_table[501]
#define QDSP_vfeCamifErrorCount_SIZE                  16
#define QDSP_vfeCommandBuf                            (volatile uint32*)qdsp_current_dma_table[502]
#define QDSP_vfeCommandBuf_SIZE                       32
#define QDSP_vfeCommandReg                            (volatile uint32*)qdsp_current_dma_table[503]
#define QDSP_vfeCommandReg_SIZE                       32
#define QDSP_vfeConfigMode                            (volatile uint16*)qdsp_current_dma_table[504]
#define QDSP_vfeConfigMode_SIZE                       16
#define QDSP_vfeGammaTable                            (volatile uint16*)qdsp_current_dma_table[505]
#define QDSP_vfeGammaTable_SIZE                       16
#define QDSP_vfeMicroDMAAddress                       (volatile uint32*)qdsp_current_dma_table[506]
#define QDSP_vfeMicroDMAAddress_SIZE                  32
#define QDSP_vfeMsgAEAWBStatsBuf                      (volatile uint32*)qdsp_current_dma_table[507]
#define QDSP_vfeMsgAEAWBStatsBuf_SIZE                 32
#define QDSP_vfeMsgAEAWBStatsReg                      (volatile uint32*)qdsp_current_dma_table[508]
#define QDSP_vfeMsgAEAWBStatsReg_SIZE                 32
#define QDSP_vfeMsgAFStatsBuf                         (volatile uint32*)qdsp_current_dma_table[509]
#define QDSP_vfeMsgAFStatsBuf_SIZE                    32
#define QDSP_vfeMsgAFStatsReg                         (volatile uint32*)qdsp_current_dma_table[510]
#define QDSP_vfeMsgAFStatsReg_SIZE                    32
#define QDSP_vfeMsgAHBOverflowReg                     (volatile uint32*)qdsp_current_dma_table[511]
#define QDSP_vfeMsgAHBOverflowReg_SIZE                32
#define QDSP_vfeMsgAsyncTimer1DoneReg                 (volatile uint32*)qdsp_current_dma_table[512]
#define QDSP_vfeMsgAsyncTimer1DoneReg_SIZE            32
#define QDSP_vfeMsgAsyncTimer2DoneReg                 (volatile uint32*)qdsp_current_dma_table[513]
#define QDSP_vfeMsgAsyncTimer2DoneReg_SIZE            32
#define QDSP_vfeMsgCamifDoneReg                       (volatile uint32*)qdsp_current_dma_table[514]
#define QDSP_vfeMsgCamifDoneReg_SIZE                  32
#define QDSP_vfeMsgCamifErrorReg                      (volatile uint32*)qdsp_current_dma_table[515]
#define QDSP_vfeMsgCamifErrorReg_SIZE                 32
#define QDSP_vfeMsgConfigCompleteReg                  (volatile uint32*)qdsp_current_dma_table[516]
#define QDSP_vfeMsgConfigCompleteReg_SIZE             32
#define QDSP_vfeMsgEpochReg                           (volatile uint32*)qdsp_current_dma_table[517]
#define QDSP_vfeMsgEpochReg_SIZE                      32
#define QDSP_vfeMsgHistStatsBuf                       (volatile uint32*)qdsp_current_dma_table[518]
#define QDSP_vfeMsgHistStatsBuf_SIZE                  32
#define QDSP_vfeMsgHistStatsReg                       (volatile uint32*)qdsp_current_dma_table[519]
#define QDSP_vfeMsgHistStatsReg_SIZE                  32
#define QDSP_vfeMsgIdleCompleteReg                    (volatile uint32*)qdsp_current_dma_table[520]
#define QDSP_vfeMsgIdleCompleteReg_SIZE               32
#define QDSP_vfeMsgIllegalCommandBuf                  (volatile uint32*)qdsp_current_dma_table[521]
#define QDSP_vfeMsgIllegalCommandBuf_SIZE             32
#define QDSP_vfeMsgIllegalCommandReg                  (volatile uint32*)qdsp_current_dma_table[522]
#define QDSP_vfeMsgIllegalCommandReg_SIZE             32
#define QDSP_vfeMsgInvalidConfigParamReg              (volatile uint32*)qdsp_current_dma_table[523]
#define QDSP_vfeMsgInvalidConfigParamReg_SIZE         32
#define QDSP_vfeMsgMicroDMAOverflowReg                (volatile uint32*)qdsp_current_dma_table[524]
#define QDSP_vfeMsgMicroDMAOverflowReg_SIZE           32
#define QDSP_vfeMsgOutput1EndOfFrameBuf               (volatile uint32*)qdsp_current_dma_table[525]
#define QDSP_vfeMsgOutput1EndOfFrameBuf_SIZE          32
#define QDSP_vfeMsgOutput1EndOfFrameReg               (volatile uint32*)qdsp_current_dma_table[526]
#define QDSP_vfeMsgOutput1EndOfFrameReg_SIZE          32
#define QDSP_vfeMsgOutput2EndOfFrameBuf               (volatile uint32*)qdsp_current_dma_table[527]
#define QDSP_vfeMsgOutput2EndOfFrameBuf_SIZE          32
#define QDSP_vfeMsgOutput2EndOfFrameReg               (volatile uint32*)qdsp_current_dma_table[528]
#define QDSP_vfeMsgOutput2EndOfFrameReg_SIZE          32
#define QDSP_vfeMsgUpdateCompleteReg                  (volatile uint32*)qdsp_current_dma_table[529]
#define QDSP_vfeMsgUpdateCompleteReg_SIZE             32
#define QDSP_vfeOutput1FrameMessageDropCount          (volatile uint16*)qdsp_current_dma_table[530]
#define QDSP_vfeOutput1FrameMessageDropCount_SIZE     16
#define QDSP_vfeOutput2FrameMessageDropCount          (volatile uint16*)qdsp_current_dma_table[531]
#define QDSP_vfeOutput2FrameMessageDropCount_SIZE     16
#define QDSP_vfeRolloffRadiusSquareTable              (volatile uint32*)qdsp_current_dma_table[532]
#define QDSP_vfeRolloffRadiusSquareTable_SIZE         32
#define QDSP_vfeRolloffSlopeAndCorrectFactorTable_B   (volatile uint32*)qdsp_current_dma_table[533]
#define QDSP_vfeRolloffSlopeAndCorrectFactorTable_B_SIZE 32
#define QDSP_vfeRolloffSlopeAndCorrectFactorTable_G   (volatile uint32*)qdsp_current_dma_table[534]
#define QDSP_vfeRolloffSlopeAndCorrectFactorTable_G_SIZE 32
#define QDSP_vfeRolloffSlopeAndCorrectFactorTable_R   (volatile uint32*)qdsp_current_dma_table[535]
#define QDSP_vfeRolloffSlopeAndCorrectFactorTable_R_SIZE 32
#define QDSP_vfeState                                 (volatile uint16*)qdsp_current_dma_table[536]
#define QDSP_vfeState_SIZE                            16
#define QDSP_vfeStopFlag                              (volatile uint16*)qdsp_current_dma_table[537]
#define QDSP_vfeStopFlag_SIZE                         16
#define QDSP_vfeVersion                               (volatile uint16*)qdsp_current_dma_table[538]
#define QDSP_vfeVersion_SIZE                          16
#define QDSP_videoCommandBuf                          (volatile uint16*)qdsp_current_dma_table[539]
#define QDSP_videoCommandBuf_SIZE                     16
#define QDSP_videoCommandReg                          (volatile uint16*)qdsp_current_dma_table[540]
#define QDSP_videoCommandReg_SIZE                     16
#define QDSP_videoEncCommandBuf                       (volatile uint16*)qdsp_current_dma_table[541]
#define QDSP_videoEncCommandBuf_SIZE                  16
#define QDSP_videoEncCommandReg                       (volatile uint16*)qdsp_current_dma_table[542]
#define QDSP_videoEncCommandReg_SIZE                  16
#define QDSP_videoEncConfigDoneReg                    (volatile uint16*)qdsp_current_dma_table[543]
#define QDSP_videoEncConfigDoneReg_SIZE               16
#define QDSP_videoEncFrameDoneBuf                     (volatile uint16*)qdsp_current_dma_table[544]
#define QDSP_videoEncFrameDoneBuf_SIZE                16
#define QDSP_videoEncFrameDoneReg                     (volatile uint16*)qdsp_current_dma_table[545]
#define QDSP_videoEncFrameDoneReg_SIZE                16
#define QDSP_videoEncIdleDoneReg                      (volatile uint16*)qdsp_current_dma_table[546]
#define QDSP_videoEncIdleDoneReg_SIZE                 16
#define QDSP_videoEncSelectDoneReg                    (volatile uint16*)qdsp_current_dma_table[547]
#define QDSP_videoEncSelectDoneReg_SIZE               16
#define QDSP_videoEncSleepDoneReg                     (volatile uint16*)qdsp_current_dma_table[548]
#define QDSP_videoEncSleepDoneReg_SIZE                16
#define QDSP_videoEncStartDoneReg                     (volatile uint16*)qdsp_current_dma_table[549]
#define QDSP_videoEncStartDoneReg_SIZE                16
#define QDSP_videoEncState                            (volatile uint16*)qdsp_current_dma_table[550]
#define QDSP_videoEncState_SIZE                       16
#define QDSP_videoEncStateError                       (volatile uint16*)qdsp_current_dma_table[551]
#define QDSP_videoEncStateError_SIZE                  16
#define QDSP_videoEncUpdateDoneReg                    (volatile uint16*)qdsp_current_dma_table[552]
#define QDSP_videoEncUpdateDoneReg_SIZE               16
#define QDSP_videoFatalErrorReg                       (volatile uint16*)qdsp_current_dma_table[553]
#define QDSP_videoFatalErrorReg_SIZE                  16
#define QDSP_videoFrameDoneReg                        (volatile uint16*)qdsp_current_dma_table[554]
#define QDSP_videoFrameDoneReg_SIZE                   16
#define QDSP_videoInInitReg                           (volatile uint16*)qdsp_current_dma_table[555]
#define QDSP_videoInInitReg_SIZE                      16
#define QDSP_videoPacketBuf                           (volatile uint16*)qdsp_current_dma_table[556]
#define QDSP_videoPacketBuf_SIZE                      16
#define QDSP_videoPacketBufFreeLen                    (volatile uint16*)qdsp_current_dma_table[557]
#define QDSP_videoPacketBufFreeLen_SIZE               16
#define QDSP_videoPacketBufLength                     (volatile uint16*)qdsp_current_dma_table[558]
#define QDSP_videoPacketBufLength_SIZE                16
#define QDSP_videoPacketBufWritePtr                   (volatile uint16*)qdsp_current_dma_table[559]
#define QDSP_videoPacketBufWritePtr_SIZE              16
#define QDSP_videoPacketDLFinishFlag                  (volatile uint16*)qdsp_current_dma_table[560]
#define QDSP_videoPacketDLFinishFlag_SIZE             16
#define QDSP_videoPacketReg                           (volatile uint16*)qdsp_current_dma_table[561]
#define QDSP_videoPacketReg_SIZE                      16
#define QDSP_videoState                               (volatile uint16*)qdsp_current_dma_table[562]
#define QDSP_videoState_SIZE                          16
#define QDSP_vocoderVersion                           (volatile uint16*)qdsp_current_dma_table[563]
#define QDSP_vocoderVersion_SIZE                      16
#define QDSP_volumeLookupIndex                        (volatile uint16*)qdsp_current_dma_table[564]
#define QDSP_volumeLookupIndex_SIZE                   16
#define QDSP_vopReconBuf0Ptr                          (volatile uint32*)qdsp_current_dma_table[565]
#define QDSP_vopReconBuf0Ptr_SIZE                     32
#define QDSP_vopReconBuf1Ptr                          (volatile uint32*)qdsp_current_dma_table[566]
#define QDSP_vopReconBuf1Ptr_SIZE                     32
#define QDSP_vopReconBuf2Ptr                          (volatile uint32*)qdsp_current_dma_table[567]
#define QDSP_vopReconBuf2Ptr_SIZE                     32
#define QDSP_vopReconBufInd                           (volatile uint16*)qdsp_current_dma_table[568]
#define QDSP_vopReconBufInd_SIZE                      16
#define QDSP_xmitPadWord                              (volatile uint16*)qdsp_current_dma_table[569]
#define QDSP_xmitPadWord_SIZE                         16
#define QDSP_ycbcrDownsizeCommandBuf                  (volatile uint32*)qdsp_current_dma_table[570]
#define QDSP_ycbcrDownsizeCommandBuf_SIZE             32
#define QDSP_ycbcrDownsizeCommandReg                  (volatile uint32*)qdsp_current_dma_table[571]
#define QDSP_ycbcrDownsizeCommandReg_SIZE             32
#define QDSP_ycbcrDownsizeMsgDoneReg                  (volatile uint32*)qdsp_current_dma_table[572]
#define QDSP_ycbcrDownsizeMsgDoneReg_SIZE             32
#define QDSP_ycbcrDownsizeMsgIllegalCmdReg            (volatile uint32*)qdsp_current_dma_table[573]
#define QDSP_ycbcrDownsizeMsgIllegalCmdReg_SIZE       32
#define QDSP_ycbcrDownsizeMsgStoppedReg               (volatile uint32*)qdsp_current_dma_table[574]
#define QDSP_ycbcrDownsizeMsgStoppedReg_SIZE          32
/* END SYMBOLS */

#define QDSP_NUM_TABLE_SYMBOLS 575
#define QDSP_NUM_SOLO_SYMBOLS 0
#define QDSP_NUM_SYMBOLS 575

/* Definitions that allow QDSP firmware images to be
** checked for at compile time.
** Firmware image length in bytes.
*/
#if defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER)
#define QDSP_IMAGE_VOC_COMBO_DEFINED
#define QDSP_IMAGE_VOC_COMBO_LENGTH 327916
#define QDSP_VOC_COMBO_VERSION 0xb024
#endif
#if defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)
#define QDSP_IMAGE_VOC_COMBO_DEFINED
#define QDSP_IMAGE_VOC_COMBO_LENGTH 242820
#define QDSP_VOC_COMBO_VERSION 0xb028
#endif
#if defined(FEATURE_QSYNTH) && !defined(FEATURE_STD_MIDI)
#define QDSP_IMAGE_SYNTH_DEFINED
#define QDSP_IMAGE_SYNTH_LENGTH 766490
#endif
#if defined(FEATURE_QSYNTH) && defined(FEATURE_STD_MIDI)
#define QDSP_IMAGE_SYNTH_DEFINED
#define QDSP_IMAGE_SYNTH_LENGTH 159880
#endif
#if defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS)
#define QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
#define QDSP_IMAGE_ENH_AAC_PLUS_LENGTH 383204
#endif
#if defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)
#define QDSP_IMAGE_QTV_AAC_DEFINED
#define QDSP_IMAGE_QTV_AAC_LENGTH 383204
#endif
#if (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && defined(FEATURE_AMR_VOCODER)
#define QDSP_IMAGE_QTV_DEFINED
#define QDSP_IMAGE_QTV_LENGTH 383204
#endif
#if (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && !defined(FEATURE_AMR_VOCODER)
#define QDSP_IMAGE_QTV_DEFINED
#define QDSP_IMAGE_QTV_LENGTH 383204
#endif
#ifdef FEATURE_JPEG_DECODER
#define QDSP_IMAGE_JPEGD_DEFINED
#define QDSP_IMAGE_JPEGD_LENGTH 16932
#endif
#ifdef FEATURE_SEC
#define QDSP_IMAGE_CRYPT_DEFINED
#define QDSP_IMAGE_CRYPT_LENGTH 20664
#endif
#ifdef FEATURE_CAMERA
#ifdef T_QSC1110 // Gemsea Add
#define QDSP_IMAGE_SA_DS05_DEFINED
#define QDSP_IMAGE_SA_DS05_LENGTH 6970
#endif
#endif
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
#define QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
#define QDSP_IMAGE_CAM_AUDIO_CONC_LENGTH 141094
#endif
#if (defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)
#define QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED
#define QDSP_IMAGE_QCAMCORDER_COMBO_LENGTH 188300
#endif
#if (defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER)
#define QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED
#define QDSP_IMAGE_QCAMCORDER_COMBO_LENGTH 188300
#endif
#if defined (FEATURE_AVS_I2SSBC)
#define QDSP_IMAGE_I2SSBC_DEFINED
#define QDSP_IMAGE_I2SSBC_LENGTH 13132
#endif

#define QDSP_IMAGE_ALL_LENGTH 3605314

/* The QDSP firmware images */
typedef enum
{
  QDSP_IMAGE_VOC_COMBO,
  QDSP_IMAGE_SYNTH,
  QDSP_IMAGE_ENH_AAC_PLUS,
  QDSP_IMAGE_QTV_AAC,
  QDSP_IMAGE_QTV,
  QDSP_IMAGE_JPEGD,
  QDSP_IMAGE_CRYPT,
  QDSP_IMAGE_SA_DS05,
  QDSP_IMAGE_CAM_AUDIO_CONC,
  QDSP_IMAGE_QCAMCORDER_COMBO,
  QDSP_IMAGE_I2SSBC,
  QDSP_IMAGE_AMR, //Gemsea Add
  QDSP_IMAGE_MAX,

  /* DO NOT USE: Force this enum to be a 32bit type to improve speed */
  QDSP_IMAGE_32BIT_DUMMY = 0x10000
} qdsp_image_type;

/* Enable QDSP interrupt related code */
#define QDSP_INTERRUPTS_USED

/* Function type used for registering ISRs */
typedef void (*qdsp_isr_func_type)(void);

/* Enumeration of interrupt lines used for intercepting QDSP interrupts */
typedef enum
{
  QDSP_ISR_INT0,
  QDSP_ISR_INT1,
  QDSP_ISR_MAX,
  QDSP_ISR_UNUSED = QDSP_ISR_MAX
} qdsp_isr_type;


/*===========================================================================
   DSP Clock rate and DME requirements
===========================================================================*/
#ifdef FEATURE_QDSP_SET_IMAGE_HW_RESOURCES
typedef enum
{
  QDSP_CLOCK_RATE_0,
  QDSP_CLOCK_RATE_1,
  QDSP_CLOCK_RATE_2,
  QDSP_CLOCK_RATE_3,
  QDSP_CLOCK_RATE_4,
  QDSP_CLOCK_RATE_5,
  QDSP_CLOCK_RATE_MAX
} qdsp_clock_rate_type;
#endif

/* ------------------ MODULE DATA ------------------ */

#define QDSP_MASTER_CONTROLLER_USED

/* Definitions that allow firmware modules to be
** checked for at compile time.
*/
#if ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_13K_DEC_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_13K_ENC_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_13K_DEFINED
#endif
#if ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_EVRC_DEC_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_EVRC_ENC_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_EVRC_DEFINED
#endif
#if ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_AMR_DEC_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_AMR_ENC_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_AMR_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_EFR_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_FR_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_HR_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_4GV_DEC_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_4GV_ENC_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_4GV_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_G711_DEFINED
#endif
#if ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined (FEATURE_AVS_I2SSBC)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_QSYNTH) && defined(FEATURE_STD_MIDI)) || \
 (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY)) || \
 (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS)) || \
 (defined(FEATURE_QSYNTH) && !defined(FEATURE_STD_MIDI)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER))
#define QDSP_MODULE_BA_MONO_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_PCM_DEFINED
#endif
#if (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_VOC_ADPCM_DEFINED
#endif
#if (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#ifdef T_QSC1110// Gemsea Add
#define QDSP_MODULE_VFE05_DEFINED
#endif
#endif
#if (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_JPEGE_DEFINED
#endif
#if (defined(FEATURE_QSYNTH) && defined(FEATURE_STD_MIDI)) || \
 (defined(FEATURE_QSYNTH) && !defined(FEATURE_STD_MIDI))
#define QDSP_MODULE_QSYNTH_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_QSYNTH) && defined(FEATURE_STD_MIDI)) || \
 (defined(FEATURE_QSYNTH) && !defined(FEATURE_STD_MIDI)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_ADPCM_DEFINED
#endif
#if (defined (FEATURE_AVS_I2SSBC)) || \
 (defined(FEATURE_QSYNTH) && defined(FEATURE_STD_MIDI)) || \
 (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY)) || \
 (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS)) || \
 (defined(FEATURE_QSYNTH) && !defined(FEATURE_STD_MIDI)) || \
 (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER))
#define QDSP_MODULE_BA_STEREO_DEFINED
#endif
#if ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS)) || \
 ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER))
#define QDSP_MODULE_MP2TS_DEFINED
#endif
#if (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS)) || \
 (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER))
#define QDSP_MODULE_AAC_DEFINED
#endif
#if (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS))
#define QDSP_MODULE_AAC_PP1_DEFINED
#endif
#if (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS))
#define QDSP_MODULE_AAC_PP2_DEFINED
#endif
#if (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS))
#define QDSP_MODULE_AAC_PP3_DEFINED
#endif
#if (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER))
#define QDSP_MODULE_MP4D_AAC_DEFINED
#endif
#if (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER))
#define QDSP_MODULE_H264D_AAC_DEFINED
#endif
#if ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_MP4D_DEFINED
#endif
#if ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_H264D_DEFINED
#endif
#if (defined(FEATURE_JPEG_DECODER))
#define QDSP_MODULE_JPEGD_DEFINED
#endif
#if (defined(FEATURE_SEC))
#define QDSP_MODULE_CRYPTO_STANDALONE_DEFINED
#endif
#if (defined(FEATURE_SEC))
#define QDSP_MODULE_CRYPTO_DEFINED
#endif
#if (defined(FEATURE_CAMERA))
#ifdef T_QSC1110// Gemsea Add
#define QDSP_MODULE_SA_DS05_DEFINED
#endif
#endif
#if (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY))
#ifdef T_QSC1110// Gemsea Add
#define QDSP_MODULE_VFE05_CAPTURE_DEFINED
#endif
#endif
#if (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY))
#define QDSP_MODULE_ACC_ADPCM_DEFINED
#endif
#if (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY))
#define QDSP_MODULE_TUNES_DEFINED
#endif
#if (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY))
#define QDSP_MODULE_TUNES_PP1_DEFINED
#endif
#if (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY))
#define QDSP_MODULE_TUNES_PP2_DEFINED
#endif
#if (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY))
#define QDSP_MODULE_TUNES_PP3_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_MP4E_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_H264E_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_MP4D_AUDDUB_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_H264D_AUDDUB_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_MP4D_MP4E_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_MP4E_VFE1P_DEFINED
#endif
#if ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER))
#define QDSP_MODULE_H264E_VFE1P_DEFINED
#endif
#if (defined (FEATURE_AVS_I2SSBC))
#define QDSP_MODULE_I2SSBC_DEFINED
#endif


/* Firmware modules */
typedef enum
{
   QDSP_MODULE_13K_DEC,
   QDSP_MODULE_13K_ENC,
   QDSP_MODULE_13K,
   QDSP_MODULE_EVRC_DEC,
   QDSP_MODULE_EVRC_ENC,
   QDSP_MODULE_EVRC,
   QDSP_MODULE_AMR_DEC,
   QDSP_MODULE_AMR_ENC,
   QDSP_MODULE_AMR,
   QDSP_MODULE_EFR,
   QDSP_MODULE_FR,
   QDSP_MODULE_HR,
   QDSP_MODULE_4GV_DEC,
   QDSP_MODULE_4GV_ENC,
   QDSP_MODULE_4GV,
   QDSP_MODULE_G711,
   QDSP_MODULE_BA_MONO,
   QDSP_MODULE_PCM,
   QDSP_MODULE_VOC_ADPCM,
   QDSP_MODULE_VFE05,
   QDSP_MODULE_JPEGE,
   QDSP_MODULE_QSYNTH,
   QDSP_MODULE_ADPCM,
   QDSP_MODULE_BA_STEREO,
   QDSP_MODULE_MP2TS,
   QDSP_MODULE_AAC,
   QDSP_MODULE_AAC_PP1,
   QDSP_MODULE_AAC_PP2,
   QDSP_MODULE_AAC_PP3,
   QDSP_MODULE_MP4D_AAC,
   QDSP_MODULE_H264D_AAC,
   QDSP_MODULE_MP4D,
   QDSP_MODULE_H264D,
   QDSP_MODULE_JPEGD,
   QDSP_MODULE_CRYPTO_STANDALONE,
   QDSP_MODULE_CRYPTO,
   QDSP_MODULE_SA_DS05,
   QDSP_MODULE_VFE05_CAPTURE,
   QDSP_MODULE_ACC_ADPCM,
   QDSP_MODULE_TUNES,
   QDSP_MODULE_TUNES_PP1,
   QDSP_MODULE_TUNES_PP2,
   QDSP_MODULE_TUNES_PP3,
   QDSP_MODULE_MP4E,
   QDSP_MODULE_H264E,
   QDSP_MODULE_MP4D_AUDDUB,
   QDSP_MODULE_H264D_AUDDUB,
   QDSP_MODULE_MP4D_MP4E,
   QDSP_MODULE_MP4E_VFE1P,
   QDSP_MODULE_H264E_VFE1P,
   QDSP_MODULE_I2SSBC,
   QDSP_MODULE_MAX,

   /* DO NOT USE: Force this enum to be a 32bit type to improve speed */
   QDSP_MODULE_32BIT_DUMMY = 0x10000
} qdsp_module_type;

/* Mode data */
typedef struct
{
   const qdsp_module_type *mode;
   const uint16           memory_config;
} qdsp_mode_data_type;

/* Data type holds all the modes supported by an image */
typedef struct
{
   const byte                num_modes;
   const qdsp_mode_data_type *modes;
} qdsp_modes_type;

/* Module selection bitmask type */
typedef uint64 qdsp_module_bitmask_type;


/* Firmware clock switching interface types */
#ifdef QDSP_SET_MODULE_HW_RESOURCES

#define QDSP_CLOCK_SWITCH_INT_DCS_DEFINED
#define QDSP_CLOCK_SWITCH_INT_VOC_DEFINED

typedef enum {
   QDSP_CLOCK_SWITCH_INT_NONE,
   QDSP_CLOCK_SWITCH_INT_DCS,
   QDSP_CLOCK_SWITCH_INT_VOC
} qdsp_clock_switch_int_type;
#endif

/* Data type holds all the module related info for an image */
typedef struct
{
   const qdsp_module_type         *modules;
   const qdsp_module_bitmask_type *bitmasks;
#ifdef QDSP_SET_MODULE_HW_RESOURCES
   const qdsp_clock_switch_int_type clk_switch_int;
   const qdsp_clock_rate_type     *clock_rates;
   const clkrgm_dme_mode_type     *dme_modes;
#endif
   const qdsp_modes_type          mode_info;
   const uint32                   mc_irq;
   const byte                     bitmask_size; /* 16 bit words */
   const boolean                  *reset_to_load;
} qdsp_module_info_type;

/* Table containing module info of all images */
extern const qdsp_module_info_type qdsp_modules_info[];

extern const qdsp_isr_type qdsp_controller_ints[];

/* Flag that enables reporting with F3 messages or Halting ADSP with
   ERR_FATAL on invalid DSP interface variable access.
*/
#ifdef FEATURE_QDSP_DEBUG_IO_MACROS
extern const boolean qdsp_debug_mode;
#endif /* FEATURE_QDSP_DEBUG_IO_MACROS */

/* Type used to return the status of a firmware download
*/
typedef enum
{
  QDSP_DOWNLOAD_SUCCESS,
  QDSP_DOWNLOAD_FAIL
} qdsp_download_status_type;

/* Type used to specify a QDSP RAM bank */
typedef enum
{
  QDSP_BANK_RAMA,
  QDSP_BANK_RAMB,
  QDSP_BANK_RAMC,
  QDSP_BANK_RAMI
} qdsp_bank_type;


/* <EJECT> */
/*===========================================================================

             A C C E S S   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/


/*===========================================================================

FUNCTION QDSP_FW_CTRL_FUNC

DESCRIPTION
  Control whether the QDSP is in Download or Run mode.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void qdsp_fw_ctrl_func ( qdspext_fw_ctrl_type fw_state );

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_IS_SUPPORTED_IMAGE

DESCRIPTION
  Parameters:
  - QDSP firmware image

  Checks if the specified image is supported in this build.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if supported; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qdsp_is_supported_image (
  /* Image to check */
  const qdsp_image_type image
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DOWNLOAD_IMAGE

DESCRIPTION
  Parameters:
  - image, index to a download image descriptor

  Performs the download of the QDSP.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  - QDSP_DOWNLOAD_SUCCESS - Firmware download successful.
  - QDSP_DOWNLOAD_FAIL    - Firmware download failed.

SIDE EFFECTS
  The QDSP will be downloaded, but the FW will not be running.

===========================================================================*/
extern qdsp_download_status_type qdsp_download_image (
  /* Index of the image to download */ 
  const qdsp_image_type image
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_START_IMAGE

DESCRIPTION
  Start the currently loaded FW image.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void qdsp_start_image ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_UP_TO_DSP_IRQ

DESCRIPTION
  Generate a microprocessor to QDSP IRQ.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
/*lint -e773 */
#if (QDSP_REG_WORD_SIZE==1)
#define qdsp_up_to_dsp_irq()                                     \
  (*((volatile word *) ((QDSP_CTRL_REGISTER) +                   \
                        (QDSP_UP_TO_DSP_IRQ_OFFSET<<1)))) = 0x1
#else
#define qdsp_up_to_dsp_irq()                                       \
  (*((volatile uint32 *) ((QDSP_CTRL_REGISTER) +                   \
                          (QDSP_UP_TO_DSP_IRQ_OFFSET<<2)))) = 0x1
#endif
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_GEN_INTERRUPT

DESCRIPTION
  Generate any combination QDSP interrupts.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  For QDSP2, only the lower 15 bits of irq_vector are valid.
  For QDSP3 and QDSP4, the lower 31 bits of irq_vector are valid.
  
  Bit 15 for QDSP2, or 31 for QDSP3 and QDSP4 is used to control whether
  bits are  SET or RESET.  This routine always sets interrupt bits.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
/*lint -e773 */
#if (QDSP_REG_WORD_SIZE==1)
#define qdsp_gen_interrupt( irq_vector )                             \
  (*((volatile word *) ((QDSP_CTRL_REGISTER) +                       \
                        (QDSP_UP_TO_DSP_IRQ_TEST_OFFSET<<1)))) =     \
                                    (uint16) (((irq_vector) & 0xffff) | 0x8000)
#else
#define qdsp_gen_interrupt( irq_vector )                                \
  (*((volatile uint32 *) ((QDSP_CTRL_REGISTER) +                        \
                         (QDSP_UP_TO_DSP_IRQ_TEST_OFFSET<<2)))) =       \
                                                    ((irq_vector) | 0x80000000)
#endif
/*lint +e773 */

#ifdef FEATURE_QDSP_DEBUG_IO_MACROS
/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_READ

DESCRIPTION
  Reads one value from the QDSP/ARM  shared memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  Value of the QDSP variable read.

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_read(address)                          \
  ( qdsp_debug_mode ?                               \
    ( qdsp_debug_read(                              \
        (void *)(address),                          \
        msg_file,                                   \
        "READ " #address " = 0x%08X",               \
        (uint32)(*(address)),                       \
        (uint32)__LINE__                            \
      ),                                            \
      ( ((uint32*)(address) != QDSP_NOP_ADDRESS) ?  \
          (*(address)) :                            \
          (0xDEADDEAD)                              \
      )                                             \
    ) :                                             \
    ( ((uint32*)(address) != QDSP_NOP_ADDRESS) ?    \
        (*(address)) :                              \
        (0xDEADDEAD)                                \
    )                                               \
  )

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_WRITE

DESCRIPTION
  Writes one value to the QDSP/ARM  shared memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  Val must be cast to uint16 or uint32 depending on the address written to.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e773 */
#define qdsp_write(address, val)                    \
  if (qdsp_debug_mode) {                            \
    qdsp_debug_write(                               \
     (void *)(address),                             \
     (uint32)(val),                                 \
     msg_file,                                      \
     "WRITE 0x%08X to " #address,                   \
     (uint32)__LINE__                               \
     );                                             \
     if ((uint32*)(address) != QDSP_NOP_ADDRESS) {  \
       (*(address)) = (val);                        \
     }                                              \
  }                                                 \
  else {                                            \
    if ((uint32*)(address) != QDSP_NOP_ADDRESS) {   \
      (*(address)) = (val);                         \
    }                                               \
  }
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_READ_OFFSET

DESCRIPTION
  Reads one value from the QDSP/ARM  shared memory area.  The
  variable address is the base address plus the offset in words or dwords.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  Value of the QDSP variable read.

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_read_offset(address, offset)           \
  ( qdsp_debug_mode ?                               \
    ( qdsp_debug_read_offset(                       \
        (void *)(address),                          \
        (uint32)(offset),                           \
        msg_file,                                   \
        "READ " #address "+%x = 0x%08X",            \
        (uint32)(*((address)+offset)),              \
        (uint32)__LINE__                            \
      ),                                            \
      ( ((uint32*)(address) != QDSP_NOP_ADDRESS)  ? \
          (*((address)+offset))  :                  \
          (0xDEADDEAD)                              \
      )                                             \
    ) :                                             \
    ( ((uint32*)(address) != QDSP_NOP_ADDRESS)  ?   \
        (*((address)+offset))  :                    \
        (0xDEADDEAD)                                \
    )                                               \
  )

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_WRITE_OFFSET

DESCRIPTION
  Writes one value to the QDSP/ARM  shared memory area.  The
  variable address is the base address plus the offset in words or dwords.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e773 */
#define qdsp_write_offset(address, offset, val)     \
  if (qdsp_debug_mode) {                            \
    qdsp_debug_write_offset(                        \
     (void *)(address),                             \
     (uint32)(offset),                              \
     (uint32)(val),                                 \
     msg_file,                                      \
     "WRITE 0x%08X to " #address "+%x",             \
     (uint32)__LINE__                               \
     );                                             \
    if ((uint32*)(address) != QDSP_NOP_ADDRESS) {   \
      (*((address) + offset)) = (val);              \
    }                                               \
  }                                                 \
  else {                                            \
    if ((uint32*)(address) != QDSP_NOP_ADDRESS) {   \
      (*((address) + offset)) = (val);              \
    }                                               \
  }
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_READ

DESCRIPTION
  Copies a block of 16-bit values from the QDSP to the ARM memory area.
  
  Parameters:
    uint16  *var,     // The QDSP variable to read from
    int     offset,   // The offset in words from the base address
    uint16  *dest,    // The destination buffer for data read from the QDSP
    int     wlen,     // The length of the buffer in words
    boolean swap      // TRUE = swap bytes

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_read(address, offset, dest, wlen, swap)  \
  if ((uint32*)(address) != QDSP_NOP_ADDRESS) {             \
    qdsp_memcpy(                                            \
     (dest),                                                \
     (uint16*)qdsp_address((address),(uint16)(offset)),     \
     (wlen),                                                \
     (swap) );                                              \
  }                                                         \
  else {                                                    \
    if (qdsp_debug_mode) {                                  \
      qdsp_debug_address(                                   \
       (void *)(address),                                   \
       (uint32)(offset),                                    \
       msg_file,                                            \
       "READ " #address "+%x LENGTH = 0x%X",                \
       (uint32)(wlen),                                      \
       (uint32)__LINE__ ) ;                                 \
    }                                                       \
  }

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_READ32

DESCRIPTION
  Copies a block of 32-bit values from the QDSP to the ARM memory area.
  
  Parameters:
    uint32  *var,     // The QDSP variable to read from
    int     offset,   // The offset in words from the base address
    uint32  *dest,    // The destination buffer for data read from the QDSP
    int     len,      // The length of the buffer in dwords
    boolean swap      // TRUE = swap bytes

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_read32(address, offset, dest, len, swap) \
  if ((uint32*)(address) != QDSP_NOP_ADDRESS) {             \
    qdsp_memcpy32(                                          \
     (dest),                                                \
     (uint32*)qdsp_address((address),(uint16)(offset)),     \
     (len),                                                 \
     (swap) );                                              \
    }                                                       \
  else {                                                    \
    if (qdsp_debug_mode) {                                  \
      qdsp_debug_address(                                   \
       (void *)(address),                                   \
       (uint32)(offset),                                    \
       msg_file,                                            \
       "READ32 " #address "+%x LENGTH = 0x%X",              \
       (uint32)(len),                                       \
       (uint32)__LINE__ ) ;                                 \
    }                                                       \
  }

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_WRITE

DESCRIPTION
  Copies a block of 16-bit values from the ARM to the QDSP memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint16  *var,     // The QDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint16  *src,     // The source buffer to write to the QDSP
    int     wlen,     // The length of the buffer in words
    boolean swap      // TRUE = swap bytes


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_write(address, offset, src, wlen, swap)  \
  if ((uint32*)(address) != QDSP_NOP_ADDRESS) {             \
    qdsp_memcpy(                                            \
     (uint16*)qdsp_address((address),(uint16)(offset)),     \
     (src),                                                 \
     (wlen),                                                \
     (swap) );                                              \
  }                                                         \
  else {                                                    \
    if (qdsp_debug_mode) {                                  \
      qdsp_debug_address(                                   \
       (void *)(address),                                   \
       (uint32)(offset),                                    \
       msg_file,                                            \
       "WRITE " #address "+%x LENGTH = 0x%X",               \
       (uint32)(wlen),                                      \
       (uint32)__LINE__ ) ;                                 \
    }                                                       \
  }

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_WRITE32

DESCRIPTION
  Copies a block of 16-bit values from the ARM to the QDSP memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint32  *var,     // The QDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint32  *src,     // The source buffer to write to the QDSP
    int     len,      // The length of the buffer in dwords
    boolean swap      // TRUE = swap bytes


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_write32(address, offset, src, len, swap) \
  if ((uint32*)(address) != QDSP_NOP_ADDRESS) {             \
    qdsp_memcpy32(                                          \
     (uint32*)qdsp_address((address),(uint16)(offset)),     \
     (src),                                                 \
     (len),                                                 \
     (swap) );                                              \
  }                                                         \
  else {                                                    \
    if (qdsp_debug_mode) {                                  \
      qdsp_debug_address(                                   \
       (void *)(address),                                   \
       (uint32)(offset),                                    \
       msg_file,                                            \
       "WRITE32 " #address "+%x LENGTH = 0x%X",             \
       (uint32)(len),                                       \
       (uint32)__LINE__ ) ;                                 \
    }                                                       \
  }

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_SET

DESCRIPTION
  Initializes a block of 16-bit values in the QDSP memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint16  *var,     // The QDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint16  *src,     // The source buffer to write to the QDSP
    int     wlen      // The length of the buffer in words


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_set(address, offset, val, wlen)          \
  if ((uint32*)(address) != QDSP_NOP_ADDRESS) {             \
    qdsp_memset(                                            \
     (uint16*)qdsp_address((address),(uint16)(offset)),     \
     (val),                                                 \
     (wlen) );                                              \
  }                                                         \
  else {                                                    \
    if (qdsp_debug_mode) {                                  \
      qdsp_debug_address(                                   \
       (void *)(address),                                   \
       (uint32)(offset),                                    \
       msg_file,                                            \
       "BLOCK SET " #address "+%x LENGTH = 0x%X",           \
       (uint32)(wlen),                                      \
       (uint32)__LINE__ ) ;                                 \
    }                                                       \
  }

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_SET32

DESCRIPTION
  Initializes a block of 32-bit values in the QDSP memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint32  *var,     // The QDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint32  *src,     // The source buffer to write to the QDSP
    int     len       // The length of the buffer in words


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_set32(address, offset, val, len)         \
  if ((uint32*)(address) != QDSP_NOP_ADDRESS) {             \
    qdsp_memset32(                                          \
     (uint32*)qdsp_address((address),(uint16)(offset)),     \
     (val),                                                 \
     (len) );                                               \
  }                                                         \
  else {                                                    \
    if (qdsp_debug_mode) {                                  \
      qdsp_debug_address(                                   \
       (void *)(address),                                   \
       (uint32)(offset),                                    \
       msg_file,                                            \
       "BLOCK SET32 " #address "+%x LENGTH = 0x%X",         \
       (uint32)(len),                                       \
       (uint32)__LINE__ ) ;                                 \
    }                                                       \
  }

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_ADDRESS

DESCRIPTION
  Returns the address of a QDSP variable in the ARM address space.

  Parameters:
    uintXX *address // The QDSP variable used as the base address
    uint16 offset   // The offset in words from the base address

DEPENDENCIES
  None

RETURN VALUE
  The ARM address of the QDSP variable.

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_address(address, offset)                       \
  ( qdsp_debug_mode ?                                       \
    ( qdsp_debug_address(                                   \
        (void *)(address),                                  \
        (uint32)(offset),                                   \
        msg_file,                                           \
        "ADDRESS to " #address "+%x = 0x%08X",              \
        (uint32)((address) + (offset)),                     \
        (uint32)__LINE__                                    \
      ),                                                    \
      ((address) + (offset))                                \
    ) : ((address) + (offset))                              \
  )

#else /* FEATURE_QDSP_DEBUG_IO_MACROS */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_READ

DESCRIPTION
  Reads one value from the QDSP/ARM  shared memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  Value of the QDSP variable read.

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_read(address) (*(address))

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_WRITE

DESCRIPTION
  Writes one value to the QDSP/ARM  shared memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  Val must be cast to uint16 or uint32 depending on the address written to.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e773 */
#define qdsp_write(address, val) (*(address)) = (val)
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_READ_OFFSET

DESCRIPTION
  Reads one value from the QDSP/ARM  shared memory area.  The
  variable address is the base address plus the offset in words or dwords.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  Value of the QDSP variable read.

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_read_offset(address, offset) (*((address)+offset))

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_WRITE_OFFSET

DESCRIPTION
  Writes one value to the QDSP/ARM  shared memory area.  The
  variable address is the base address plus the offset in words or dwords.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e773 */
#define qdsp_write_offset(address, offset, val) \
   (*((address) + offset)) = (val)
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_READ

DESCRIPTION
  Copies a block of 16-bit values from the QDSP to the ARM memory area.
  
  Parameters:
    uint16  *var,     // The QDSP variable to read from
    int     offset,   // The offset in words from the base address
    uint16  *dest,    // The destination buffer for data read from the QDSP
    int     wlen,     // The length of the buffer in words
    boolean swap      // TRUE = swap bytes

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_read(address, offset, dest, wlen, swap)  \
    qdsp_memcpy(                                            \
     (dest),                                                \
     (uint16*)qdsp_address((address),(uint16)(offset)),     \
     (wlen),                                                \
     (swap) )

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_READ32

DESCRIPTION
  Copies a block of 32-bit values from the QDSP to the ARM memory area.
  
  Parameters:
    uint32  *var,     // The QDSP variable to read from
    int     offset,   // The offset in words from the base address
    uint32  *dest,    // The destination buffer for data read from the QDSP
    int     len,      // The length of the buffer in dwords
    boolean swap      // TRUE = swap bytes

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_read32(address, offset, dest, len, swap) \
    qdsp_memcpy32(                                          \
     (dest),                                                \
     (uint32*)qdsp_address((address),(uint16)(offset)),     \
     (len),                                                 \
     (swap) )

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_WRITE

DESCRIPTION
  Copies a block of 16-bit values from the ARM to the QDSP memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint16  *var,     // The QDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint16  *src,     // The source buffer to write to the QDSP
    int     wlen,     // The length of the buffer in words
    boolean swap      // TRUE = swap bytes


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_write(address, offset, src, wlen, swap)  \
    qdsp_memcpy(                                            \
     (uint16*)qdsp_address((address),(uint16)(offset)),     \
     (src),                                                 \
     (wlen),                                                \
     (swap) )

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_WRITE32

DESCRIPTION
  Copies a block of 16-bit values from the ARM to the QDSP memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint32  *var,     // The QDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint32  *src,     // The source buffer to write to the QDSP
    int     len,      // The length of the buffer in dwords
    boolean swap      // TRUE = swap bytes


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_write32(address, offset, src, len, swap) \
    qdsp_memcpy32(                                          \
     (uint32*)qdsp_address((address),(uint16)(offset)),     \
     (src),                                                 \
     (len),                                                 \
     (swap) )

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_SET

DESCRIPTION
  Initializes a block of 16-bit values in the QDSP memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint16  *var,     // The QDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint16  *src,     // The source buffer to write to the QDSP
    int     wlen      // The length of the buffer in words


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_set(address, offset, val, wlen)          \
    qdsp_memset(                                            \
     (uint16*)qdsp_address((address),(uint16)(offset)),     \
     (val),                                                 \
     (wlen) )

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BLOCK_SET32

DESCRIPTION
  Initializes a block of 32-bit values in the QDSP memory area.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint32  *var,     // The QDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint32  *src,     // The source buffer to write to the QDSP
    int     len       // The length of the buffer in words


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_block_set32(address, offset, val, len)          \
    qdsp_memset32(                                          \
     (uint32*)qdsp_address((address),(uint16)(offset)),     \
     (val),                                                 \
     (len) )

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_ADDRESS

DESCRIPTION
  Returns the address of a QDSP variable in the ARM address space.

  Parameters:
    uintXX *address // The QDSP variable used as the base address
    uint16 offset   // The offset in words from the base address

DEPENDENCIES
  None

RETURN VALUE
  The ARM address of the QDSP variable.

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_address(address, offset) ((address) + (offset))

#endif /* FEATURE_QDSP_DEBUG_IO_MACROS */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DSP_ADDRESS

DESCRIPTION
  Returns the address of a QDSP variable in the QDSP address space.
  
  Parameters:
    uintXX *var   // Index of the QDSP variable used as the base address
    uint16 offset // The offset in from the base address

DEPENDENCIES
  None

RETURN VALUE
  The QDSP address of the QDSP variable.

SIDE EFFECTS
  None

===========================================================================*/
#define qdsp_dsp_address(address, offset) (uint32)( (((uint32)(address))-qdsp_data_table.base_address + ((offset) << (address##_SIZE / 16)) ) >> (address##_SIZE / 16))

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_BANK_ADDRESS

DESCRIPTION
  Returns an ARM address pointer given a bank-offset and QDSP memory bank.

DEPENDENCIES
  None

RETURN VALUE
  The ARM address of the QDSP variable.

SIDE EFFECTS
  None

===========================================================================*/
extern uint16* qdsp_bank_address (
  /* The memory bank to use as the base */
  qdsp_bank_type bank,
  /* The offset in words from the start of the memory bank */
  uint16 offset
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DIRECT_WRITE

DESCRIPTION
  Writes one 16-bit value to the QDSP/ARM shared memory area.
  
  Parameters:
    port // The DSP address of the QDSP variable to write to
    val  // The value to write into QDSP/ARM shared memory

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e773 */
#define qdsp_direct_write(port, val) \
  (*((volatile word *) (QDSP_BASE_ADDRESS+((port)<<1)))) = (word)(val)
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DIRECT_WRITE32

DESCRIPTION
  Writes one 32-bit value to the QDSP/ARM shared memory area.
  
  Parameters:
    port // The DSP address of the QDSP variable to write to
    val  // The value to write into QDSP/ARM shared memory

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e773 */
#define qdsp_direct_write32(port, val) \
  (*((volatile dword *) (QDSP_BASE_ADDRESS+((port)<<2)))) = (dword)(val)
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_VALID_ADDRESS

DESCRIPTION
  Checks if any address in a block of memory is within the QDSP/ARM  shared
  memory area.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - Part of the memory block is within the shared memory area.
  FALSE - No part of the memory block is within the shared memory area.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qdsp_valid_address (
  /* The start address of the memory block */
  void* start_address,
  /* The length in bytes of the memory block */
  uint16 length
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_MEMSHARE_CTRL_FUNC

DESCRIPTION
  Parameters:
  - mask for selecting the RAM bank

  Reconfigures DSP memory banks.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void qdsp_memshare_ctrl_func (
  qdsp_dsp_memconfig_type mem_share_bank
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_UP_TO_DSP_DME_ADDRESS

DESCRIPTION
  Parameters:
   - uP (ARM) memory type
   - address of the ARM location where the data is stored

  Translates the uP (ARM) address to an address in the DSP memory space. This
   address can then be used by the DSP to perform DME operations.

DEPENDENCIES
  None

RETURN VALUE
  DSP address
  
SIDE EFFECTS
  None

===========================================================================*/
extern uint32 qdsp_uP_to_dsp_dme_address (
  const qdspext_dme_ctrl_type  ext_type,
  const void                  *dme_data_ptr
);

#if ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_QSYNTH) && defined(FEATURE_STD_MIDI)) || \
 (defined(FEATURE_AUDIO_CAMERA_CONCURRENCY)) || \
 (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS)) || \
 (defined(FEATURE_QSYNTH) && !defined(FEATURE_STD_MIDI)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && !defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER)) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AVS_NEXTGEN_EC) && defined(FEATURE_AMR_VOCODER)) || \
 ((defined(FEATURE_VIDEO_ENCODE) && !defined(FEATURE_OEMSBL_AUDIBLE_TONE)) && !defined(FEATURE_AMR_VOCODER)) || \
 (defined(FEATURE_AAC) || defined(FEATURE_ENHANCED_AAC_PLUS) || defined(FEATURE_MP4_DECODER))
/*===========================================================================

FUNCTION QDSP_DME_CTRL_FUNC

DESCRIPTION
  Parameters:
   - type of memory on which the DME actions will be performed
   - address of the array where the data is stored

  Write the address of the DME data segment into the appropriate DSP register,
   so that the DSP can perform DME operations on the data, immediately after
   the image is downloaded to the DSP.

DEPENDENCIES
  The clock regime driving the QDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_dme_ctrl_func (
  const qdspext_dme_ctrl_type  ext_type,
  const void                  *dme_data_ptr,
  const uint32                 size_in_bytes
);
#endif

/* <EJECT> */
#ifdef QDSP_SET_MODULE_HW_RESOURCES
/*===========================================================================

FUNCTION QDSP_PREPARE_FOR_DSP_CLOCK_RATE_SWITCH

DESCRIPTION
  Parameters:
   - rate to switch to.

  Prepare to switch to the new DSP clock rate specified.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_prepare_for_clock_rate_switch (
  const qdsp_clock_rate_type clock_rate
);

/*===========================================================================

FUNCTION QDSP_GET_MODULE_DME_MODE

DESCRIPTION
  Parameters:
   - image for which the DME mode is required
   - module for which the DME mode is required

  Gets the QDSP DME mode for the specified module.

DEPENDENCIES
  None.

RETURN VALUE
  DME mode.
  
SIDE EFFECTS
  None

===========================================================================*/
extern clkrgm_dme_mode_type qdsp_get_module_dme_mode (
  const qdsp_image_type   image,
  const qdsp_module_type  module
);
#endif /* QDSP_SET_MODULE_HW_RESOURCES */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CONFIG_HW_RESOURCES_FOR_IMAGE

DESCRIPTION
  Parameters:
   - image for which the hardware must be configured.

  Configure the clock rate and DME requirement for the image specified.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_config_hw_resources_for_image (
  const qdsp_image_type image
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CONFIG_HW_RESOURCES_FOR_SLEEP_MODE

DESCRIPTION
  Configure the hardware resources for sleep mode (when no applications are
  enabled).

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_config_hw_resources_for_sleep_mode (void);

#ifdef FEATURE_QDSP_SET_IMAGE_HW_RESOURCES
/*===========================================================================

FUNCTION QDSP_GET_IMAGE_CLOCK_RATE

DESCRIPTION
  Parameters:
   - image for which the clock rate is required

  Gets the default QDSP clock rate for the specified image.

DEPENDENCIES
  None.

RETURN VALUE
  Clock rate.
  
SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_clock_rate_type qdsp_get_image_clock_rate (
  const qdsp_image_type   image
);

/*===========================================================================

FUNCTION QDSP_GET_IMAGE_DME_MODE

DESCRIPTION
  Parameters:
   - image for which the DME mode is required

  Gets the default QDSP DME mode for the specified image.

DEPENDENCIES
  None.

RETURN VALUE
  DME mode.
  
SIDE EFFECTS
  None

===========================================================================*/
extern clkrgm_dme_mode_type qdsp_get_image_dme_mode (
  const qdsp_image_type   image
);

#endif /* FEATURE_QDSP_SET_IMAGE_HW_RESOURCES */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_GET_MODULE_BITMASK

DESCRIPTION
  Parameters:
  - image to which the module belongs
  - module for which the bitmask is required

  Gets the Master Controller bitmask used to enable the specified module.

DEPENDENCIES
  None.

RETURN VALUE
  - Bitmask; Zero if the module does not require a bitmask

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_module_bitmask_type qdsp_get_module_bitmask (
  const qdsp_image_type   image,
  const qdsp_module_type  module
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_REGISTER_DSP_SERVICES_ISRS

DESCRIPTION
  Registers QDSP Services ISRs with tramp. These ISRs will intercept
  the QDSP interrupts and in turn invoke the ISRs of all clients that
  are currently enabled and have a valid ISR registered for the incoming
  interrupt.

DEPENDENCIES
  Tramp service must be initialized.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void qdsp_register_dsp_services_isrs (void);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_GET_PREFERRED_IMAGE

DESCRIPTION
  Parameters:
  - module for which the preferred image is required

  Gets the preferred image to use (if any), for the specified module.

DEPENDENCIES
  None.

RETURN VALUE
  image if present, QDSP_IMAGE_MAX otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern void qdsp_get_preferred_image (
  const qdsp_module_type module,
  qdsp_image_type        *image,
  boolean                *preempt
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_SET_MEMORY_ACCESSOR_FUNCS

DESCRIPTION
  Parameters:
  - Structure containing function pointers that will assist with
    accessing DSP memory

  Sets the function pointers necessary for accessing DSP memory
  on some DSP configurations when default methods provided by
  the system are not applicable.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void qdsp_set_memory_accessor_funcs (
  qdspext_accessor_funcs_struct_type *accessor_funcs
);
/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DEBUG_READ

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void qdsp_debug_read (
  void * address,
  char const * const file,
  char const * const msg,
  uint32 val,
  uint32 line
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DEBUG_WRITE

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void qdsp_debug_write (
  void * address,
  uint32 val,
  char const * const file,
  char const * const msg,
  uint32 line
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DEBUG_READ_OFFSET

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void qdsp_debug_read_offset (
  void * address,
  uint32 offset,
  char const * const file,
  char const * const msg,
  uint32 val,
  uint32 line
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DEBUG_WRITE_OFFSET

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void qdsp_debug_write_offset (
  void * address,
  uint32 offset,
  uint32 val,
  char const * const file,
  char const * const msg,
  uint32 line
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DEBUG_ADDRESS

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void qdsp_debug_address (
  void * address,
  uint32 offset,
  char const * const file,
  char const * const msg,
  uint32 val,
  uint32 line
);
#endif /* QDSPEXT_H */

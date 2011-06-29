#ifndef MDSPEXT_H
#define MDSPEXT_H

/*
WARNING: This file is auto-generated.

Generated at:    Sat Apr 24 00:58:44 2010
Generated using: MSMSHARED_EXT_PARSER.00.11.00
Generated from:

-------------------------------------------------------------------------------
Configuration file:
   1478 04/09/2008 11:19:25 mdspext.cfg
-------------------------------------------------------------------------------
IMAGE[0] cdma
  83964 06/15/2009 10:51:44 ..\..\cdma\bin\pedro_cdma.h
  83964 06/15/2009 10:51:44 ..\..\cdma\bin\pedro_cdma.h
 135044 06/15/2009 10:51:44 ..\..\cdma\bin\pedro_cdma.ext
-------------------------------------------------------------------------------
*/

/*===========================================================================

                    M D S P   I M A G E   S O U R C E
       A N D   A C C E S S   F U N C T I O N S   D E F I N I T I O N

DESCRIPTION
  Definition of the firmware images that can be dowloaded to the MDSP.
  Definition of the MDSP variables and the access functions.

REFERENCES

Copyright (c) 2010 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "msm.h"
#include "qdspimage.h"
#include "qdspmem.h"

#if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
    defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING) || \
    defined(FEATURE_MDSP_SET_IMAGE_HW_RESOURCES)
#include "clkregim.h"      /* Clock Regime functions             */
#endif

/* Composite firmware version info for link-time checking */
extern uint8 mdsp_cdma_cdma110;
#define MDSP_VERSION_CHECK() if (!( \
 mdsp_cdma_cdma110 )) { ERR_FATAL("Firmware/library mismatch",0,0,0); } 

/* <EJECT> */
/*===========================================================================

                      D A T A   D E F I N I T I O N S

===========================================================================*/

extern uint32* mdsp_current_dma_table;

/* Register value to set the DSP to Run Mode.
*/
#define MDSP_RUN_VAL   0x2

/* Register value to set the DSP to Download Mode.
*/
#define MDSP_RESET_VAL 0x3

/* Length, in words, of DSP control registers
*/
#define MDSP_REG_WORD_SIZE 2

/* Offset of microprocessor to DSP IRQ register from the DSP_RESET register
*/
#define MDSP_UP_TO_DSP_IRQ_OFFSET (1)

/* Offset of microprocessor to DSP IRQ TEST register from the DSP_RESET register
*/
#define MDSP_UP_TO_DSP_IRQ_TEST_OFFSET (2)

/* Base address of DSP and DSP hardware registers */
extern const qdspext_data_type mdsp_data_table;
#define MDSP_BASE_ADDRESS MICRO_MDSP_BASE
#define MDSP_REG_OFFSET   0xc00000
#define MDSP_CTRL_REGISTER ((MDSP_BASE_ADDRESS) + (MDSP_REG_OFFSET))

/* Definitions to allow configuration of DSP memory */
#define MDSP_MEM_SHARE_MASK     0
#define MDSP_MEM_SHARE_ENABLE_NONE 0


#if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
    defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING) || \
    defined(FEATURE_MDSP_MODULE_RESOURCE_MGMT)
/* Set module specific DSP clock rate, DME mode etc. */
#define MDSP_SET_MODULE_HW_RESOURCES
#define MDSP_SERVICES_MIPS_BASED_CLOCK_CTRL
#endif

#if (defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
     defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING) || \
     defined(FEATURE_MDSP_MODULE_RESOURCE_MGMT)) && \
    !defined(FEATURE_MDSP_SET_IMAGE_HW_RESOURCES)
/* Set image specific DSP clock rate, DME mode etc. */
#define FEATURE_MDSP_SET_IMAGE_HW_RESOURCES
#endif


/*===========================================================================
    MDSP firmware constant definitions
===========================================================================*/
#define HDR_MPC_CMD_INTERRUPT                            1
#define HPQ_RESULTS_SIZE                                 1
#define HPQ_RESULTS__RESULTS_BUFFER_OFFSET               0
#define HPQ_STATE_STRUCT_SIZE                            20
#define HPQ_STATE_STRUCT__ABORT_DONE_OFFSET              15
#define HPQ_STATE_STRUCT__ABORT_ISSUED_OFFSET            2
#define HPQ_STATE_STRUCT__DMA_QUEUE_PRIORITY_OFFSET      5
#define HPQ_STATE_STRUCT__FREQ_OFFSET_OFFSET             13
#define HPQ_STATE_STRUCT__HPQ_HDR_SHIFT_PTR_OFFSET       18
#define HPQ_STATE_STRUCT__NEW_HOST_TASK_OFFSET           1
#define HPQ_STATE_STRUCT__NUM_RESULTS_OFFSET             16
#define HPQ_STATE_STRUCT__NUM_TASKS_OFFSET               11
#define HPQ_STATE_STRUCT__NUM_TASKS_TO_PROCESS_OFFSET    4
#define HPQ_STATE_STRUCT__NUM_TASKS_WORKING_OFFSET       10
#define HPQ_STATE_STRUCT__PRIORITY_OFFSET                6
#define HPQ_STATE_STRUCT__RAW_RESULTS_READY_OFFSET       3
#define HPQ_STATE_STRUCT__RESULTS_PTR_OFFSET             9
#define HPQ_STATE_STRUCT__SAMPS_NEEDED_OFFSET            7
#define HPQ_STATE_STRUCT__SEARCH_DONE_OFFSET             14
#define HPQ_STATE_STRUCT__SEARCH_PARAMS_OFFSET           19
#define HPQ_STATE_STRUCT__SEARCH_TYPE_OFFSET             12
#define HPQ_STATE_STRUCT__START_POSITION_PTR_OFFSET      17
#define HPQ_STATE_STRUCT__STATE_OFFSET                   0
#define HPQ_STATE_STRUCT__TASK_QUEUE_PTR_OFFSET          8
#define MASK_HPQ1xSearchParams_0__WINDOW_SIZE            0x0000FFFF
#define MASK_HPQ1xSearchParams_1__PN_POSITION            0x00007FFF
#define MASK_HPQ1xSearchParams_2__COH_LENGTH             0x00000FFF
#define MASK_HPQ1xSearchParams_3__NONCOH_LENGTH          0x0000007F
#define MASK_HPQ1xSearchParams_3__PILOT_INDEX            0x0000FF80
#define MASK_HPQ1xSearchParams_4__COH_TRUCNT             0x00003800
#define MASK_HPQ1xSearchParams_4__DIVERSITY              0x0000C000
#define MASK_HPQ1xSearchParams_4__QOF                    0x00000600
#define MASK_HPQ1xSearchParams_4__WALSH_INDEX            0x000001FF
#define MASK_HPQHdrSearchParams_0__WINDOW_SIZE           0x0000FFFF
#define MASK_HPQHdrSearchParams_1__PN_POSITION           0x00007FFF
#define MASK_HPQHdrSearchParams_2__COH_LENGTH            0x00000FFF
#define MASK_HPQHdrSearchParams_3__BURST_LENGTH          0x0000000C
#define MASK_HPQHdrSearchParams_3__NONCOH_LENGTH         0x00000003
#define MASK_HPQHdrSearchParams_3__PILOT_INDEX           0x0000FF80
#define MASK_HPQHdrSearchParams_4__COH_TRUCNT            0x00003800
#define MASK_HPQHdrSearchParams_4__DIVERSITY             0x0000C000
#define MASK_HPQHdrSearchParams_4__QOF                   0x00000600
#define MASK_HPQHdrSearchParams_4__WALSH_INDEX           0x000001FF
#define MASK_LPQSearchParams_0__WINDOW_SIZE              0x0000FFFF
#define MASK_LPQSearchParams_1__PN_POSITION              0x00007FFF
#define MASK_LPQSearchParams_2__COH_LENGTH               0x00000FFF
#define MASK_LPQSearchParams_3__NONCOH_LENGTH            0x0000007F
#define MASK_LPQSearchParams_3__PILOT_INDEX              0x0000FF80
#define MASK_LPQSearchParams_4__COH_TRUCNT               0x00003800
#define MASK_LPQSearchParams_4__DIVERSITY                0x0000C000
#define MASK_LPQSearchParams_4__QOF                      0x00000600
#define MASK_LPQSearchParams_4__WALSH_INDEX              0x000001FF
#define MASK_MOD_PRMBL_CTL_0__PREAMBLE_ABORT             0x00002000
#define MASK_MOD_PRMBL_CTL_0__PREAMBLE_ENA               0x00000001
#define MASK_MOD_PRMBL_CTL_0__PREAMBLE_FRAC_DURATION     0x000001E0
#define MASK_MOD_PRMBL_CTL_0__PREAMBLE_NUM_FRAC          0x0000001E
#define MASK_MOD_PRMBL_CTL_0__PREAMBLE_PCG_POSITION      0x00001E00
#define MASK_MOD_PRMBL_CTL_0__PWR_CTRL_PUNCTURE_ENA      0x00004000
#define MASK_MOD_PRMBL_CTL_1__PREAMBLE_ADD_DURATION      0x000001F0
#define MASK_MOD_PRMBL_CTL_1__PREAMBLE_OFF_DURATION      0x0000000F
#define MASK_microSrchCmd__ABORT                         0x00008000
#define MASK_microSrchCmd__HPQ0                          0x00000002
#define MASK_microSrchCmd__HPQ1                          0x00000004
#define MASK_microSrchCmd__LPQ                           0x00000001
#define MASK_microSrchCmd__QPCH                          0x00002000
#define MASK_microSrchCmd__SCHEDULE_SR_FLUSH             0x00001000
#define MASK_microSrchCmd__SR_FLUSH                      0x00004000
#define MDSP_CDMA_APP_PRESENT                            0x40000003
#define MDSP_CDMA_BUILD_DATE                             0x0615
#define MDSP_CDMA_BUILD_TIME                             0x1051
#define MDSP_CDMA_BUILD_YEAR                             0x2009
#define MDSP_CDMA_FIRMWARE_VERSION                       0x0110
#define MDSP_LOG_MAX_CHUNK_SIZE                          64
#define MDSP_MC_APP_BT                                   2
#define MDSP_MC_APP_CDMA1X                               0
#define MDSP_MC_APP_DV                                   7
#define MDSP_MC_APP_FLO                                  11
#define MDSP_MC_APP_G2W                                  10
#define MDSP_MC_APP_GPRS                                 5
#define MDSP_MC_APP_GPS                                  3
#define MDSP_MC_APP_GSM                                  5
#define MDSP_MC_APP_HDR                                  4
#define MDSP_MC_APP_MC                                   30
#define MDSP_MC_APP_SRCH                                 1
#define MDSP_MC_APP_UBM                                  8
#define MDSP_MC_APP_W2G                                  9
#define MDSP_MC_APP_WCDMA                                6
#define MDSP_MC_CMD_APP_CONFIG                           1
#define MDSP_MC_CMD_INTERRUPT                            18
#define MDSP_MC_CMD_RESUME                               5
#define MDSP_MC_CMD_SUSPEND                              4
#define MDSP_MC_CMD_XMEMC_READ                           2
#define MDSP_MC_CMD_XMEMC_WRITE                          3
#define NUM_SORTED_PEAKS                                 6
#define NUM_WORDS_PER_PEAK                               4
#define POS_HPQ1xSearchParams_0__WINDOW_SIZE             0
#define POS_HPQ1xSearchParams_1__PN_POSITION             0
#define POS_HPQ1xSearchParams_2__COH_LENGTH              0
#define POS_HPQ1xSearchParams_3__NONCOH_LENGTH           0
#define POS_HPQ1xSearchParams_3__PILOT_INDEX             7
#define POS_HPQ1xSearchParams_4__COH_TRUCNT              11
#define POS_HPQ1xSearchParams_4__DIVERSITY               14
#define POS_HPQ1xSearchParams_4__QOF                     9
#define POS_HPQ1xSearchParams_4__WALSH_INDEX             0
#define POS_HPQHdrSearchParams_0__WINDOW_SIZE            0
#define POS_HPQHdrSearchParams_1__PN_POSITION            0
#define POS_HPQHdrSearchParams_2__COH_LENGTH             0
#define POS_HPQHdrSearchParams_3__BURST_LENGTH           2
#define POS_HPQHdrSearchParams_3__NONCOH_LENGTH          0
#define POS_HPQHdrSearchParams_3__PILOT_INDEX            7
#define POS_HPQHdrSearchParams_4__COH_TRUCNT             11
#define POS_HPQHdrSearchParams_4__DIVERSITY              14
#define POS_HPQHdrSearchParams_4__QOF                    9
#define POS_HPQHdrSearchParams_4__WALSH_INDEX            0
#define POS_LPQSearchParams_0__WINDOW_SIZE               0
#define POS_LPQSearchParams_1__PN_POSITION               0
#define POS_LPQSearchParams_2__COH_LENGTH                0
#define POS_LPQSearchParams_3__NONCOH_LENGTH             0
#define POS_LPQSearchParams_3__PILOT_INDEX               7
#define POS_LPQSearchParams_4__COH_TRUCNT                11
#define POS_LPQSearchParams_4__DIVERSITY                 14
#define POS_LPQSearchParams_4__QOF                       9
#define POS_LPQSearchParams_4__WALSH_INDEX               0
#define POS_MOD_PRMBL_CTL_0__PREAMBLE_ABORT              13
#define POS_MOD_PRMBL_CTL_0__PREAMBLE_ENA                0
#define POS_MOD_PRMBL_CTL_0__PREAMBLE_FRAC_DURATION      5
#define POS_MOD_PRMBL_CTL_0__PREAMBLE_NUM_FRAC           1
#define POS_MOD_PRMBL_CTL_0__PREAMBLE_PCG_POSITION       9
#define POS_MOD_PRMBL_CTL_0__PWR_CTRL_PUNCTURE_ENA       14
#define POS_MOD_PRMBL_CTL_1__PREAMBLE_ADD_DURATION       4
#define POS_MOD_PRMBL_CTL_1__PREAMBLE_OFF_DURATION       0
#define POS_microSrchCmd__ABORT                          15
#define POS_microSrchCmd__HPQ0                           1
#define POS_microSrchCmd__HPQ1                           2
#define POS_microSrchCmd__LPQ                            0
#define POS_microSrchCmd__QPCH                           13
#define POS_microSrchCmd__SCHEDULE_SR_FLUSH              12
#define POS_microSrchCmd__SR_FLUSH                       14
#define RXF_CMD_INTERRUPT                                23
#define SIZE_HPQ1xSearchParams_0__WINDOW_SIZE            16
#define SIZE_HPQ1xSearchParams_1__PN_POSITION            15
#define SIZE_HPQ1xSearchParams_2__COH_LENGTH             12
#define SIZE_HPQ1xSearchParams_3__NONCOH_LENGTH          7
#define SIZE_HPQ1xSearchParams_3__PILOT_INDEX            9
#define SIZE_HPQ1xSearchParams_4__COH_TRUCNT             3
#define SIZE_HPQ1xSearchParams_4__DIVERSITY              2
#define SIZE_HPQ1xSearchParams_4__QOF                    2
#define SIZE_HPQ1xSearchParams_4__WALSH_INDEX            9
#define SIZE_HPQHdrSearchParams_0__WINDOW_SIZE           16
#define SIZE_HPQHdrSearchParams_1__PN_POSITION           15
#define SIZE_HPQHdrSearchParams_2__COH_LENGTH            12
#define SIZE_HPQHdrSearchParams_3__BURST_LENGTH          2
#define SIZE_HPQHdrSearchParams_3__NONCOH_LENGTH         2
#define SIZE_HPQHdrSearchParams_3__PILOT_INDEX           9
#define SIZE_HPQHdrSearchParams_4__COH_TRUCNT            3
#define SIZE_HPQHdrSearchParams_4__DIVERSITY             2
#define SIZE_HPQHdrSearchParams_4__QOF                   2
#define SIZE_HPQHdrSearchParams_4__WALSH_INDEX           9
#define SIZE_LPQSearchParams_0__WINDOW_SIZE              16
#define SIZE_LPQSearchParams_1__PN_POSITION              15
#define SIZE_LPQSearchParams_2__COH_LENGTH               12
#define SIZE_LPQSearchParams_3__NONCOH_LENGTH            7
#define SIZE_LPQSearchParams_3__PILOT_INDEX              9
#define SIZE_LPQSearchParams_4__COH_TRUCNT               3
#define SIZE_LPQSearchParams_4__DIVERSITY                2
#define SIZE_LPQSearchParams_4__QOF                      2
#define SIZE_LPQSearchParams_4__WALSH_INDEX              9
#define SIZE_MOD_PRMBL_CTL_0__PREAMBLE_ABORT             1
#define SIZE_MOD_PRMBL_CTL_0__PREAMBLE_ENA               1
#define SIZE_MOD_PRMBL_CTL_0__PREAMBLE_FRAC_DURATION     4
#define SIZE_MOD_PRMBL_CTL_0__PREAMBLE_NUM_FRAC          4
#define SIZE_MOD_PRMBL_CTL_0__PREAMBLE_PCG_POSITION      4
#define SIZE_MOD_PRMBL_CTL_0__PWR_CTRL_PUNCTURE_ENA      1
#define SIZE_MOD_PRMBL_CTL_1__PREAMBLE_ADD_DURATION      5
#define SIZE_MOD_PRMBL_CTL_1__PREAMBLE_OFF_DURATION      4
#define SIZE_microSrchCmd__ABORT                         1
#define SIZE_microSrchCmd__HPQ0                          1
#define SIZE_microSrchCmd__HPQ1                          1
#define SIZE_microSrchCmd__LPQ                           1
#define SIZE_microSrchCmd__QPCH                          1
#define SIZE_microSrchCmd__SCHEDULE_SR_FLUSH             1
#define SIZE_microSrchCmd__SR_FLUSH                      1
#define SRCH_CMD_INTERRUPT                               22
#define SRCH_MAX_HPQ_TASKS                               16
#define SRCH_MAX_LPQ_TASKS                               16
#define SRCH_MICRO_TASK_SIZE                             5
#define SRCH_NUM_HPQ                                     1
#define VAL_HPQ1xSearchParams_4__DIVERSITY_ANTENNA_0     0x0
#define VAL_HPQ1xSearchParams_4__DIVERSITY_ANTENNA_1     0x1
#define VAL_HPQ1xSearchParams_4__DIVERSITY_DUAL_ANTENNA  0x10
#define VAL_HPQHdrSearchParams_4__DIVERSITY_ANTENNA_0    0x0
#define VAL_HPQHdrSearchParams_4__DIVERSITY_ANTENNA_1    0x1
#define VAL_HPQHdrSearchParams_4__DIVERSITY_DUAL_ANTENNA 0x10
#define VAL_HPQStateStruct_SearchType__1X                0
#define VAL_HPQStateStruct_SearchType__HDR               1
#define VAL_HPQStateStruct_State__ABORTING               0
#define VAL_HPQStateStruct_State__INACTIVE               2
#define VAL_HPQStateStruct_State__ISSUED                 3
#define VAL_HPQStateStruct_State__NUMSTATES              5
#define VAL_HPQStateStruct_State__READY                  4
#define VAL_HPQStateStruct_State__SEARCH_DONE            1
#define VAL_LPQSearchParams_4__DIVERSITY_ANTENNA_0       0x0
#define VAL_LPQSearchParams_4__DIVERSITY_ANTENNA_1       0x1
#define VAL_LPQSearchParams_4__DIVERSITY_DUAL_ANTENNA    0x10


/*===========================================================================
    MDSP firmware DMA symbols
===========================================================================*/
/* BEGIN SYMBOLS */
#define CDMA1X_MOD_DCCH_GAIN                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200594)
#define CDMA1X_MOD_DCCH_GAIN_SIZE                16
#define CDMA1X_MOD_FCH_GAIN                      (volatile uint16*)(MICRO_MDSP_BASE + 0x200596)
#define CDMA1X_MOD_FCH_GAIN_SIZE                 16
#define CDMA1X_MOD_PCH_GAIN                      (volatile uint16*)(MICRO_MDSP_BASE + 0x200592)
#define CDMA1X_MOD_PCH_GAIN_SIZE                 16
#define CDMA1X_MOD_PRMBL_CTL_0                   (volatile uint16*)(MICRO_MDSP_BASE + 0x20059c)
#define CDMA1X_MOD_PRMBL_CTL_0_SIZE              16
#define CDMA1X_MOD_PRMBL_CTL_1                   (volatile uint16*)(MICRO_MDSP_BASE + 0x20059e)
#define CDMA1X_MOD_PRMBL_CTL_1_SIZE              16
#define CDMA1X_MOD_PRMBL_GAIN                    (volatile uint16*)(MICRO_MDSP_BASE + 0x200590)
#define CDMA1X_MOD_PRMBL_GAIN_SIZE               16
#define CDMA1X_MOD_SCH_GAIN                      (volatile uint16*)(MICRO_MDSP_BASE + 0x200598)
#define CDMA1X_MOD_SCH_GAIN_SIZE                 16
#define CDMA1X_REACH_TX_RATE_ADJ                 (volatile uint16*)(MICRO_MDSP_BASE + 0x20059a)
#define CDMA1X_REACH_TX_RATE_ADJ_SIZE            16
#define CDMA1X_TX_RATE_ADJ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x2005a0)
#define CDMA1X_TX_RATE_ADJ_SIZE                  16
#define CDMA1X_afcCrossMode                      (volatile uint16*)(MICRO_MDSP_BASE + 0xc48)
#define CDMA1X_afcCrossMode_SIZE                 16
#define CDMA1X_afcMncntrNval                     (volatile uint32*)(MICRO_MDSP_BASE + 0x400c00)
#define CDMA1X_afcMncntrNval_SIZE                32
#define CDMA1X_afcRxFreqGain                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2005f6)
#define CDMA1X_afcRxFreqGain_SIZE                16
#define CDMA1X_afcRxRotFreq                      (volatile uint32*)(MICRO_MDSP_BASE + 0x400bf8)
#define CDMA1X_afcRxRotFreq_SIZE                 32
#define CDMA1X_afcTxRotFreq                      (volatile uint32*)(MICRO_MDSP_BASE + 0x400c04)
#define CDMA1X_afcTxRotFreq_SIZE                 32
#define CDMA1X_afcTxRxFreqRatio                  (volatile uint16*)(MICRO_MDSP_BASE + 0x2005f4)
#define CDMA1X_afcTxRxFreqRatio_SIZE             16
#define CDMA1X_afcXoFreqAcc                      (volatile uint32*)(MICRO_MDSP_BASE + 0x400bfc)
#define CDMA1X_afcXoFreqAcc_SIZE                 32
#define CDMA1X_afcXoFreqError                    (volatile uint32*)(MICRO_MDSP_BASE + 0x400c40)
#define CDMA1X_afcXoFreqError_SIZE               32
#define CDMA1X_afcXoGain                         (volatile uint16*)(MICRO_MDSP_BASE + 0xc46)
#define CDMA1X_afcXoGain_SIZE                    16
#define CDMA1X_afcXoLoadFreqAcc                  (volatile uint16*)(MICRO_MDSP_BASE + 0x200624)
#define CDMA1X_afcXoLoadFreqAcc_SIZE             16
#define CDMA1X_afcXoLoadStb                      (volatile uint16*)(MICRO_MDSP_BASE + 0x200622)
#define CDMA1X_afcXoLoadStb_SIZE                 16
#define CDMA1X_afcXoMndEnable                    (volatile uint16*)(MICRO_MDSP_BASE + 0xc4c)
#define CDMA1X_afcXoMndEnable_SIZE               16
#define CDMA1X_afcXoMndMBase                     (volatile uint32*)(MICRO_MDSP_BASE + 0x4005c0)
#define CDMA1X_afcXoMndMBase_SIZE                32
#define CDMA1X_afcXoMndRndBits                   (volatile uint16*)(MICRO_MDSP_BASE + 0x774)
#define CDMA1X_afcXoMndRndBits_SIZE              16
#define CDMA1X_afcXoMndRndFactor                 (volatile uint32*)(MICRO_MDSP_BASE + 0x4005c4)
#define CDMA1X_afcXoMndRndFactor_SIZE            32
#define CDMA1X_cAccessProbes                     (volatile uint16*)(MICRO_MDSP_BASE + 0xc60)
#define CDMA1X_cAccessProbes_SIZE                16
#define CDMA1X_cAccessProbesInit                 (volatile uint16*)(MICRO_MDSP_BASE + 0x20015e)
#define CDMA1X_cAccessProbesInit_SIZE            16
#define CDMA1X_cBetapTimeDetla                   (volatile uint16*)(MICRO_MDSP_BASE + 0x20016a)
#define CDMA1X_cBetapTimeDetla_SIZE              16
#define CDMA1X_cPaFrameParamsAvailable           (volatile uint16*)(MICRO_MDSP_BASE + 0xc6e)
#define CDMA1X_cPaFrameParamsAvailable_SIZE      16
#define CDMA1X_cPaHysteresisTimerValue           (volatile uint16*)(MICRO_MDSP_BASE + 0x200166)
#define CDMA1X_cPaHysteresisTimerValue_SIZE      16
#define CDMA1X_cPaNewParamsAvailable             (volatile uint16*)(MICRO_MDSP_BASE + 0x200164)
#define CDMA1X_cPaNewParamsAvailable_SIZE        16
#define CDMA1X_cPaOffsetParams                   (volatile uint16*)(MICRO_MDSP_BASE + 0x200104)
#define CDMA1X_cPaOffsetParams_SIZE              16
#define CDMA1X_cPaRangeDelayH2LValue             (volatile uint16*)(MICRO_MDSP_BASE + 0x78e)
#define CDMA1X_cPaRangeDelayH2LValue_SIZE        16
#define CDMA1X_cPaRangeDelayL2HValue             (volatile uint16*)(MICRO_MDSP_BASE + 0x78c)
#define CDMA1X_cPaRangeDelayL2HValue_SIZE        16
#define CDMA1X_cPaRangeMap                       (volatile uint16*)(MICRO_MDSP_BASE + 0x78a)
#define CDMA1X_cPaRangeMap_SIZE                  16
#define CDMA1X_cPaRangeMapEn                     (volatile uint16*)(MICRO_MDSP_BASE + 0x788)
#define CDMA1X_cPaRangeMapEn_SIZE                16
#define CDMA1X_cPaRangeOverride                  (volatile uint16*)(MICRO_MDSP_BASE + 0x20063e)
#define CDMA1X_cPaRangeOverride_SIZE             16
#define CDMA1X_cPaRangeValue                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2005f0)
#define CDMA1X_cPaRangeValue_SIZE                16
#define CDMA1X_cPaRangeWr                        (volatile uint16*)(MICRO_MDSP_BASE + 0xc74)
#define CDMA1X_cPaRangeWr_SIZE                   16
#define CDMA1X_cPaReset                          (volatile uint16*)(MICRO_MDSP_BASE + 0x784)
#define CDMA1X_cPaReset_SIZE                     16
#define CDMA1X_cPaRfPnRot                        (volatile uint16*)(MICRO_MDSP_BASE + 0x20013a)
#define CDMA1X_cPaRfPnRot_SIZE                   16
#define CDMA1X_cPaStateOverride                  (volatile uint16*)(MICRO_MDSP_BASE + 0xc70)
#define CDMA1X_cPaStateOverride_SIZE             16
#define CDMA1X_cPaStateThreshParams              (volatile uint16*)(MICRO_MDSP_BASE + 0x200124)
#define CDMA1X_cPaStateThreshParams_SIZE         16
#define CDMA1X_cPaStateValue                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2005ec)
#define CDMA1X_cPaStateValue_SIZE                16
#define CDMA1X_cPaStateWr                        (volatile uint16*)(MICRO_MDSP_BASE + 0xc72)
#define CDMA1X_cPaStateWr_SIZE                   16
#define CDMA1X_cRfPnRollTime                     (volatile uint16*)(MICRO_MDSP_BASE + 0x20013e)
#define CDMA1X_cRfPnRollTime_SIZE                16
#define CDMA1X_cRpcRatchetForceDown              (volatile uint16*)(MICRO_MDSP_BASE + 0x2005de)
#define CDMA1X_cRpcRatchetForceDown_SIZE         16
#define CDMA1X_cRpcRatchetForceUp                (volatile uint16*)(MICRO_MDSP_BASE + 0x2005dc)
#define CDMA1X_cRpcRatchetForceUp_SIZE           16
#define CDMA1X_cRxAntenna                        (volatile uint16*)(MICRO_MDSP_BASE + 0x2005f2)
#define CDMA1X_cRxAntenna_SIZE                   16
#define CDMA1X_cTempHysteresisEn                 (volatile uint16*)(MICRO_MDSP_BASE + 0x786)
#define CDMA1X_cTempHysteresisEn_SIZE            16
#define CDMA1X_cTxAgcActionTime                  (volatile uint16*)(MICRO_MDSP_BASE + 0xc78)
#define CDMA1X_cTxAgcActionTime_SIZE             16
#define CDMA1X_cTxAgcAdjOverride                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200642)
#define CDMA1X_cTxAgcAdjOverride_SIZE            16
#define CDMA1X_cTxAgcAdjVal                      (volatile uint32*)(MICRO_MDSP_BASE + 0x400be0)
#define CDMA1X_cTxAgcAdjVal_SIZE                 32
#define CDMA1X_cTxAgcAdjWr                       (volatile uint32*)(MICRO_MDSP_BASE + 0x400c54)
#define CDMA1X_cTxAgcAdjWr_SIZE                  32
#define CDMA1X_cTxAgcUpdateH2L                   (volatile uint16*)(MICRO_MDSP_BASE + 0x200168)
#define CDMA1X_cTxAgcUpdateH2L_SIZE              16
#define CDMA1X_cTxAgcUpdateL2H                   (volatile uint16*)(MICRO_MDSP_BASE + 0x790)
#define CDMA1X_cTxAgcUpdateL2H_SIZE              16
#define CDMA1X_cTxAttenLimitWr                   (volatile uint16*)(MICRO_MDSP_BASE + 0x77e)
#define CDMA1X_cTxAttenLimitWr_SIZE              16
#define CDMA1X_cTxBetapEnable                    (volatile uint16*)(MICRO_MDSP_BASE + 0x77a)
#define CDMA1X_cTxBetapEnable_SIZE               16
#define CDMA1X_cTxBetapGain                      (volatile uint16*)(MICRO_MDSP_BASE + 0x200644)
#define CDMA1X_cTxBetapGain_SIZE                 16
#define CDMA1X_cTxBetapGainOverride              (volatile uint16*)(MICRO_MDSP_BASE + 0xc7e)
#define CDMA1X_cTxBetapGainOverride_SIZE         16
#define CDMA1X_cTxBetapGainWr                    (volatile uint16*)(MICRO_MDSP_BASE + 0x20064a)
#define CDMA1X_cTxBetapGainWr_SIZE               16
#define CDMA1X_cTxBetapLutIdx                    (volatile uint16*)(MICRO_MDSP_BASE + 0xc7c)
#define CDMA1X_cTxBetapLutIdx_SIZE               16
#define CDMA1X_cTxBetapOverride                  (volatile uint16*)(MICRO_MDSP_BASE + 0x200646)
#define CDMA1X_cTxBetapOverride_SIZE             16
#define CDMA1X_cTxBetapVal                       (volatile uint16*)(MICRO_MDSP_BASE + 0xc7a)
#define CDMA1X_cTxBetapVal_SIZE                  16
#define CDMA1X_cTxBetapWr                        (volatile uint16*)(MICRO_MDSP_BASE + 0x200648)
#define CDMA1X_cTxBetapWr_SIZE                   16
#define CDMA1X_cTxCombProcFreq                   (volatile uint16*)(MICRO_MDSP_BASE + 0x20015a)
#define CDMA1X_cTxCombProcFreq_SIZE              16
#define CDMA1X_cTxEnable                         (volatile uint16*)(MICRO_MDSP_BASE + 0xc5c)
#define CDMA1X_cTxEnable_SIZE                    16
#define CDMA1X_cTxGainAdjClrOverride             (volatile uint16*)(MICRO_MDSP_BASE + 0xc62)
#define CDMA1X_cTxGainAdjClrOverride_SIZE        16
#define CDMA1X_cTxGainAdjMax                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2005d8)
#define CDMA1X_cTxGainAdjMax_SIZE                16
#define CDMA1X_cTxGainAdjMin                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2005da)
#define CDMA1X_cTxGainAdjMin_SIZE                16
#define CDMA1X_cTxGainAdjOverride                (volatile uint16*)(MICRO_MDSP_BASE + 0x200636)
#define CDMA1X_cTxGainAdjOverride_SIZE           16
#define CDMA1X_cTxGainAdjStepSize                (volatile uint16*)(MICRO_MDSP_BASE + 0x77c)
#define CDMA1X_cTxGainAdjStepSize_SIZE           16
#define CDMA1X_cTxGainAdjVal                     (volatile uint16*)(MICRO_MDSP_BASE + 0xc66)
#define CDMA1X_cTxGainAdjVal_SIZE                16
#define CDMA1X_cTxGainAdjWr                      (volatile uint16*)(MICRO_MDSP_BASE + 0xc64)
#define CDMA1X_cTxGainAdjWr_SIZE                 16
#define CDMA1X_cTxGainLimitWr                    (volatile uint16*)(MICRO_MDSP_BASE + 0x200162)
#define CDMA1X_cTxGainLimitWr_SIZE               16
#define CDMA1X_cTxInit                           (volatile uint16*)(MICRO_MDSP_BASE + 0x778)
#define CDMA1X_cTxInit_SIZE                      16
#define CDMA1X_cTxLimitFrameParamsAvailable      (volatile uint16*)(MICRO_MDSP_BASE + 0xc6c)
#define CDMA1X_cTxLimitFrameParamsAvailable_SIZE 16
#define CDMA1X_cTxLimitNewParamsAvailable        (volatile uint16*)(MICRO_MDSP_BASE + 0x200160)
#define CDMA1X_cTxLimitNewParamsAvailable_SIZE   16
#define CDMA1X_cTxLimitingMax                    (volatile uint16*)(MICRO_MDSP_BASE + 0xc0e)
#define CDMA1X_cTxLimitingMax_SIZE               16
#define CDMA1X_cTxLimitingMin                    (volatile uint16*)(MICRO_MDSP_BASE + 0xc12)
#define CDMA1X_cTxLimitingMin_SIZE               16
#define CDMA1X_cTxLinFrameParamsAvailable        (volatile uint16*)(MICRO_MDSP_BASE + 0x200640)
#define CDMA1X_cTxLinFrameParamsAvailable_SIZE   16
#define CDMA1X_cTxLinLutPa0                      (volatile uint16*)(MICRO_MDSP_BASE + 0x4f8)
#define CDMA1X_cTxLinLutPa0_SIZE                 16
#define CDMA1X_cTxLinLutPa1                      (volatile uint16*)(MICRO_MDSP_BASE + 0x578)
#define CDMA1X_cTxLinLutPa1_SIZE                 16
#define CDMA1X_cTxLinLutPa2                      (volatile uint16*)(MICRO_MDSP_BASE + 0x5f8)
#define CDMA1X_cTxLinLutPa2_SIZE                 16
#define CDMA1X_cTxLinLutPa3                      (volatile uint16*)(MICRO_MDSP_BASE + 0x678)
#define CDMA1X_cTxLinLutPa3_SIZE                 16
#define CDMA1X_cTxLinNewParamsAvailable          (volatile uint16*)(MICRO_MDSP_BASE + 0xc76)
#define CDMA1X_cTxLinNewParamsAvailable_SIZE     16
#define CDMA1X_cTxLinSlopeParams                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200114)
#define CDMA1X_cTxLinSlopeParams_SIZE            16
#define CDMA1X_cTxLinXOffsetParams               (volatile uint16*)(MICRO_MDSP_BASE + 0x20011c)
#define CDMA1X_cTxLinXOffsetParams_SIZE          16
#define CDMA1X_cTxLinYOffsetParams               (volatile uint16*)(MICRO_MDSP_BASE + 0x20010c)
#define CDMA1X_cTxLinYOffsetParams_SIZE          16
#define CDMA1X_cTxLogAddr                        (volatile uint16*)(MICRO_MDSP_BASE + 0x734)
#define CDMA1X_cTxLogAddr_SIZE                   16
#define CDMA1X_cTxLogLength                      (volatile uint16*)(MICRO_MDSP_BASE + 0x732)
#define CDMA1X_cTxLogLength_SIZE                 16
#define CDMA1X_cTxMaskDelayValue                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200638)
#define CDMA1X_cTxMaskDelayValue_SIZE            16
#define CDMA1X_cTxMaskDelayWr                    (volatile uint32*)(MICRO_MDSP_BASE + 0x400c4c)
#define CDMA1X_cTxMaskDelayWr_SIZE               32
#define CDMA1X_cTxOpenLoopOverride               (volatile uint16*)(MICRO_MDSP_BASE + 0x2005f8)
#define CDMA1X_cTxOpenLoopOverride_SIZE          16
#define CDMA1X_cTxOpenLoopThresh                 (volatile uint16*)(MICRO_MDSP_BASE + 0x20015c)
#define CDMA1X_cTxOpenLoopThresh_SIZE            16
#define CDMA1X_cTxOpenLoopVal                    (volatile uint16*)(MICRO_MDSP_BASE + 0x2005fc)
#define CDMA1X_cTxOpenLoopVal_SIZE               16
#define CDMA1X_cTxOpenLoopWr                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2005fa)
#define CDMA1X_cTxOpenLoopWr_SIZE                16
#define CDMA1X_cTxPaRangePolarity                (volatile uint16*)(MICRO_MDSP_BASE + 0x20013c)
#define CDMA1X_cTxPaRangePolarity_SIZE           16
#define CDMA1X_cTxPdmTablePa0                    (volatile uint32*)(MICRO_MDSP_BASE + 0x4001c0)
#define CDMA1X_cTxPdmTablePa0_SIZE               32
#define CDMA1X_cTxPdmTablePa1                    (volatile uint32*)(MICRO_MDSP_BASE + 0x4002c0)
#define CDMA1X_cTxPdmTablePa1_SIZE               32
#define CDMA1X_cTxPdmTablePa2                    (volatile uint32*)(MICRO_MDSP_BASE + 0x4003c0)
#define CDMA1X_cTxPdmTablePa2_SIZE               32
#define CDMA1X_cTxPdmTablePa3                    (volatile uint32*)(MICRO_MDSP_BASE + 0x4004c0)
#define CDMA1X_cTxPdmTablePa3_SIZE               32
#define CDMA1X_cTxPowerDetEnable                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200140)
#define CDMA1X_cTxPowerDetEnable_SIZE            16
#define CDMA1X_cTxPowerDetThresh                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200142)
#define CDMA1X_cTxPowerDetThresh_SIZE            16
#define CDMA1X_cTxPowerHighIntFlag               (volatile uint16*)(MICRO_MDSP_BASE + 0x200144)
#define CDMA1X_cTxPowerHighIntFlag_SIZE          16
#define CDMA1X_cTxRateAdjVal                     (volatile uint16*)(MICRO_MDSP_BASE + 0x20063c)
#define CDMA1X_cTxRateAdjVal_SIZE                16
#define CDMA1X_cTxRateAdjWr                      (volatile uint16*)(MICRO_MDSP_BASE + 0x20063a)
#define CDMA1X_cTxRateAdjWr_SIZE                 16
#define CDMA1X_cTxSbiHwreqCpy1                   (volatile uint32*)(MICRO_MDSP_BASE + 0x400be8)
#define CDMA1X_cTxSbiHwreqCpy1_SIZE              32
#define CDMA1X_cTxSbiHwreqCpy2                   (volatile uint32*)(MICRO_MDSP_BASE + 0x400bec)
#define CDMA1X_cTxSbiHwreqCpy2_SIZE              32
#define CDMA1X_cTxSbiHwreqCpy3                   (volatile uint32*)(MICRO_MDSP_BASE + 0x400bf0)
#define CDMA1X_cTxSbiHwreqCpy3_SIZE              32
#define CDMA1X_cTxSbiHwreqCpy4                   (volatile uint32*)(MICRO_MDSP_BASE + 0x400bf4)
#define CDMA1X_cTxSbiHwreqCpy4_SIZE              32
#define CDMA1X_cTxSsbiPort                       (volatile uint16*)(MICRO_MDSP_BASE + 0x200130)
#define CDMA1X_cTxSsbiPort_SIZE                  16
#define CDMA1X_cTxSsbiReg1Addr                   (volatile uint16*)(MICRO_MDSP_BASE + 0x200132)
#define CDMA1X_cTxSsbiReg1Addr_SIZE              16
#define CDMA1X_cTxSsbiReg2Addr                   (volatile uint16*)(MICRO_MDSP_BASE + 0x200134)
#define CDMA1X_cTxSsbiReg2Addr_SIZE              16
#define CDMA1X_cTxSsbiReg3Addr                   (volatile uint16*)(MICRO_MDSP_BASE + 0x200136)
#define CDMA1X_cTxSsbiReg3Addr_SIZE              16
#define CDMA1X_cTxSsbiReg4Addr                   (volatile uint16*)(MICRO_MDSP_BASE + 0x200138)
#define CDMA1X_cTxSsbiReg4Addr_SIZE              16
#define CDMA1X_cTxTotalGainVal                   (volatile uint16*)(MICRO_MDSP_BASE + 0xc6a)
#define CDMA1X_cTxTotalGainVal_SIZE              16
#define CDMA1X_cdma1xPassThruEnable              (volatile uint16*)(MICRO_MDSP_BASE + 0x2005a4)
#define CDMA1X_cdma1xPassThruEnable_SIZE         16
#define CDMA1X_cdma1xVersion                     (volatile uint16*)(MICRO_MDSP_BASE + 0x4f6)
#define CDMA1X_cdma1xVersion_SIZE                16
#define CDMA1X_ch0AccFrame                       (volatile uint16*)(MICRO_MDSP_BASE + 0x2005c0)
#define CDMA1X_ch0AccFrame_SIZE                  16
#define CDMA1X_ch1AccFrame                       (volatile uint16*)(MICRO_MDSP_BASE + 0x2005c2)
#define CDMA1X_ch1AccFrame_SIZE                  16
#define CDMA1X_ch2AccFrame                       (volatile uint16*)(MICRO_MDSP_BASE + 0x2005c4)
#define CDMA1X_ch2AccFrame_SIZE                  16
#define CDMA1X_comRSSILockThreshCh0              (volatile uint16*)(MICRO_MDSP_BASE + 0xc04)
#define CDMA1X_comRSSILockThreshCh0_SIZE         16
#define CDMA1X_comRSSILockThreshCh2              (volatile uint16*)(MICRO_MDSP_BASE + 0xc06)
#define CDMA1X_comRSSILockThreshCh2_SIZE         16
#define CDMA1X_divRSSILockThreshCh0              (volatile uint16*)(MICRO_MDSP_BASE + 0x2005e4)
#define CDMA1X_divRSSILockThreshCh0_SIZE         16
#define CDMA1X_divRSSILockThreshCh2              (volatile uint16*)(MICRO_MDSP_BASE + 0x2005e6)
#define CDMA1X_divRSSILockThreshCh2_SIZE         16
#define CDMA1X_ebNtMagTargetFch                  (volatile uint16*)(MICRO_MDSP_BASE + 0xc38)
#define CDMA1X_ebNtMagTargetFch_SIZE             16
#define CDMA1X_ebNtMagTargetSch                  (volatile uint16*)(MICRO_MDSP_BASE + 0xc3a)
#define CDMA1X_ebNtMagTargetSch_SIZE             16
#define CDMA1X_fing0ComCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x2003bc)
#define CDMA1X_fing0ComCombLock_SIZE             16
#define CDMA1X_fing0ComFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x400960)
#define CDMA1X_fing0ComFiltRSSI_SIZE             32
#define CDMA1X_fing0ComFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x9f2)
#define CDMA1X_fing0ComFingLock_SIZE             16
#define CDMA1X_fing0ComPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0x9e2)
#define CDMA1X_fing0ComPilotFiltI_SIZE           16
#define CDMA1X_fing0ComPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x2003b4)
#define CDMA1X_fing0ComPilotFiltQ_SIZE           16
#define CDMA1X_fing0ComPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0x9dc)
#define CDMA1X_fing0ComPilotGain_SIZE            16
#define CDMA1X_fing0ComWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xa08)
#define CDMA1X_fing0ComWtI_SIZE                  16
#define CDMA1X_fing0ComWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x2003d4)
#define CDMA1X_fing0ComWtQ_SIZE                  16
#define CDMA1X_fing0CombEnable                   (volatile uint16*)(MICRO_MDSP_BASE + 0x9d4)
#define CDMA1X_fing0CombEnable_SIZE              16
#define CDMA1X_fing0DivCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x9f4)
#define CDMA1X_fing0DivCombLock_SIZE             16
#define CDMA1X_fing0DivFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x400964)
#define CDMA1X_fing0DivFiltRSSI_SIZE             32
#define CDMA1X_fing0DivFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x2003be)
#define CDMA1X_fing0DivFingLock_SIZE             16
#define CDMA1X_fing0DivPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0x9e4)
#define CDMA1X_fing0DivPilotFiltI_SIZE           16
#define CDMA1X_fing0DivPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x2003b6)
#define CDMA1X_fing0DivPilotFiltQ_SIZE           16
#define CDMA1X_fing0DivPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0x9e8)
#define CDMA1X_fing0DivPilotGain_SIZE            16
#define CDMA1X_fing0DivWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xa0a)
#define CDMA1X_fing0DivWtI_SIZE                  16
#define CDMA1X_fing0DivWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x2003d6)
#define CDMA1X_fing0DivWtQ_SIZE                  16
#define CDMA1X_fing0FiltRSSI                     (volatile uint32*)(MICRO_MDSP_BASE + 0x40095c)
#define CDMA1X_fing0FiltRSSI_SIZE                32
#define CDMA1X_fing0FingCfgWr                    (volatile uint32*)(MICRO_MDSP_BASE + 0x400954)
#define CDMA1X_fing0FingCfgWr_SIZE               32
#define CDMA1X_fing0FingCurrentCfg               (volatile uint32*)(MICRO_MDSP_BASE + 0x400958)
#define CDMA1X_fing0FingCurrentCfg_SIZE          32
#define CDMA1X_fing0ForceLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0x2003ae)
#define CDMA1X_fing0ForceLock_SIZE               16
#define CDMA1X_fing0ForceLockState               (volatile uint16*)(MICRO_MDSP_BASE + 0x9d8)
#define CDMA1X_fing0ForceLockState_SIZE          16
#define CDMA1X_fing0Init                         (volatile uint16*)(MICRO_MDSP_BASE + 0xa12)
#define CDMA1X_fing0Init_SIZE                    16
#define CDMA1X_fing0LockInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2003ac)
#define CDMA1X_fing0LockInit_SIZE                16
#define CDMA1X_fing0MmseFiltA00I                 (volatile uint16*)(MICRO_MDSP_BASE + 0x2003c2)
#define CDMA1X_fing0MmseFiltA00I_SIZE            16
#define CDMA1X_fing0MmseFiltA01I                 (volatile uint16*)(MICRO_MDSP_BASE + 0x9fa)
#define CDMA1X_fing0MmseFiltA01I_SIZE            16
#define CDMA1X_fing0MmseFiltA01Q                 (volatile uint16*)(MICRO_MDSP_BASE + 0x2003c0)
#define CDMA1X_fing0MmseFiltA01Q_SIZE            16
#define CDMA1X_fing0MmseFiltA11I                 (volatile uint16*)(MICRO_MDSP_BASE + 0x9f8)
#define CDMA1X_fing0MmseFiltA11I_SIZE            16
#define CDMA1X_fing0MmseG00I                     (volatile uint16*)(MICRO_MDSP_BASE + 0x9fc)
#define CDMA1X_fing0MmseG00I_SIZE                16
#define CDMA1X_fing0MmseG01I                     (volatile uint16*)(MICRO_MDSP_BASE + 0x9fe)
#define CDMA1X_fing0MmseG01I_SIZE                16
#define CDMA1X_fing0MmseG01Q                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2003c4)
#define CDMA1X_fing0MmseG01Q_SIZE                16
#define CDMA1X_fing0MmseG11I                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2003c6)
#define CDMA1X_fing0MmseG11I_SIZE                16
#define CDMA1X_fing0PilotStr                     (volatile uint16*)(MICRO_MDSP_BASE + 0x9ea)
#define CDMA1X_fing0PilotStr_SIZE                16
#define CDMA1X_fing0PowerLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0x9f6)
#define CDMA1X_fing0PowerLock_SIZE               16
#define CDMA1X_fing0PowerLockThresh              (volatile uint16*)(MICRO_MDSP_BASE + 0x2003b2)
#define CDMA1X_fing0PowerLockThresh_SIZE         16
#define CDMA1X_fing0PwrCtlCell                   (volatile uint16*)(MICRO_MDSP_BASE + 0x9de)
#define CDMA1X_fing0PwrCtlCell_SIZE              16
#define CDMA1X_fing0QlicCfg                      (volatile uint32*)(MICRO_MDSP_BASE + 0x40097c)
#define CDMA1X_fing0QlicCfg_SIZE                 32
#define CDMA1X_fing0QlicSectorId                 (volatile uint16*)(MICRO_MDSP_BASE + 0x2003b8)
#define CDMA1X_fing0QlicSectorId_SIZE            16
#define CDMA1X_fing0RSSIInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0x9d6)
#define CDMA1X_fing0RSSIInit_SIZE                16
#define CDMA1X_fing0SectorId                     (volatile uint16*)(MICRO_MDSP_BASE + 0x9e0)
#define CDMA1X_fing0SectorId_SIZE                16
#define CDMA1X_fing0State                        (volatile uint16*)(MICRO_MDSP_BASE + 0x2003ba)
#define CDMA1X_fing0State_SIZE                   16
#define CDMA1X_fing0TTComELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0x9da)
#define CDMA1X_fing0TTComELGain_SIZE             16
#define CDMA1X_fing0TTDivELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0x9e6)
#define CDMA1X_fing0TTDivELGain_SIZE             16
#define CDMA1X_fing0TTEnable                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2003b0)
#define CDMA1X_fing0TTEnable_SIZE                16
#define CDMA1X_fing0TdShift                      (volatile uint16*)(MICRO_MDSP_BASE + 0x9ec)
#define CDMA1X_fing0TdShift_SIZE                 16
#define CDMA1X_fing1ComCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x2003fc)
#define CDMA1X_fing1ComCombLock_SIZE             16
#define CDMA1X_fing1ComFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x400990)
#define CDMA1X_fing1ComFiltRSSI_SIZE             32
#define CDMA1X_fing1ComFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0xa36)
#define CDMA1X_fing1ComFingLock_SIZE             16
#define CDMA1X_fing1ComPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0xa26)
#define CDMA1X_fing1ComPilotFiltI_SIZE           16
#define CDMA1X_fing1ComPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x2003f4)
#define CDMA1X_fing1ComPilotFiltQ_SIZE           16
#define CDMA1X_fing1ComPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0xa20)
#define CDMA1X_fing1ComPilotGain_SIZE            16
#define CDMA1X_fing1ComWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xa4c)
#define CDMA1X_fing1ComWtI_SIZE                  16
#define CDMA1X_fing1ComWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x200414)
#define CDMA1X_fing1ComWtQ_SIZE                  16
#define CDMA1X_fing1CombEnable                   (volatile uint16*)(MICRO_MDSP_BASE + 0xa18)
#define CDMA1X_fing1CombEnable_SIZE              16
#define CDMA1X_fing1DivCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0xa38)
#define CDMA1X_fing1DivCombLock_SIZE             16
#define CDMA1X_fing1DivFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x400994)
#define CDMA1X_fing1DivFiltRSSI_SIZE             32
#define CDMA1X_fing1DivFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x2003fe)
#define CDMA1X_fing1DivFingLock_SIZE             16
#define CDMA1X_fing1DivPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0xa28)
#define CDMA1X_fing1DivPilotFiltI_SIZE           16
#define CDMA1X_fing1DivPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x2003f6)
#define CDMA1X_fing1DivPilotFiltQ_SIZE           16
#define CDMA1X_fing1DivPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0xa2c)
#define CDMA1X_fing1DivPilotGain_SIZE            16
#define CDMA1X_fing1DivWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xa4e)
#define CDMA1X_fing1DivWtI_SIZE                  16
#define CDMA1X_fing1DivWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x200416)
#define CDMA1X_fing1DivWtQ_SIZE                  16
#define CDMA1X_fing1FiltRSSI                     (volatile uint32*)(MICRO_MDSP_BASE + 0x40098c)
#define CDMA1X_fing1FiltRSSI_SIZE                32
#define CDMA1X_fing1FingCfgWr                    (volatile uint32*)(MICRO_MDSP_BASE + 0x400984)
#define CDMA1X_fing1FingCfgWr_SIZE               32
#define CDMA1X_fing1FingCurrentCfg               (volatile uint32*)(MICRO_MDSP_BASE + 0x400988)
#define CDMA1X_fing1FingCurrentCfg_SIZE          32
#define CDMA1X_fing1ForceLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0x2003ee)
#define CDMA1X_fing1ForceLock_SIZE               16
#define CDMA1X_fing1ForceLockState               (volatile uint16*)(MICRO_MDSP_BASE + 0xa1c)
#define CDMA1X_fing1ForceLockState_SIZE          16
#define CDMA1X_fing1Init                         (volatile uint16*)(MICRO_MDSP_BASE + 0xa56)
#define CDMA1X_fing1Init_SIZE                    16
#define CDMA1X_fing1LockInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2003ec)
#define CDMA1X_fing1LockInit_SIZE                16
#define CDMA1X_fing1MmseFiltA00I                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200402)
#define CDMA1X_fing1MmseFiltA00I_SIZE            16
#define CDMA1X_fing1MmseFiltA01I                 (volatile uint16*)(MICRO_MDSP_BASE + 0xa3e)
#define CDMA1X_fing1MmseFiltA01I_SIZE            16
#define CDMA1X_fing1MmseFiltA01Q                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200400)
#define CDMA1X_fing1MmseFiltA01Q_SIZE            16
#define CDMA1X_fing1MmseFiltA11I                 (volatile uint16*)(MICRO_MDSP_BASE + 0xa3c)
#define CDMA1X_fing1MmseFiltA11I_SIZE            16
#define CDMA1X_fing1MmseG00I                     (volatile uint16*)(MICRO_MDSP_BASE + 0xa40)
#define CDMA1X_fing1MmseG00I_SIZE                16
#define CDMA1X_fing1MmseG01I                     (volatile uint16*)(MICRO_MDSP_BASE + 0xa42)
#define CDMA1X_fing1MmseG01I_SIZE                16
#define CDMA1X_fing1MmseG01Q                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200404)
#define CDMA1X_fing1MmseG01Q_SIZE                16
#define CDMA1X_fing1MmseG11I                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200406)
#define CDMA1X_fing1MmseG11I_SIZE                16
#define CDMA1X_fing1PilotStr                     (volatile uint16*)(MICRO_MDSP_BASE + 0xa2e)
#define CDMA1X_fing1PilotStr_SIZE                16
#define CDMA1X_fing1PowerLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0xa3a)
#define CDMA1X_fing1PowerLock_SIZE               16
#define CDMA1X_fing1PowerLockThresh              (volatile uint16*)(MICRO_MDSP_BASE + 0x2003f2)
#define CDMA1X_fing1PowerLockThresh_SIZE         16
#define CDMA1X_fing1PwrCtlCell                   (volatile uint16*)(MICRO_MDSP_BASE + 0xa22)
#define CDMA1X_fing1PwrCtlCell_SIZE              16
#define CDMA1X_fing1QlicCfg                      (volatile uint32*)(MICRO_MDSP_BASE + 0x4009ac)
#define CDMA1X_fing1QlicCfg_SIZE                 32
#define CDMA1X_fing1QlicSectorId                 (volatile uint16*)(MICRO_MDSP_BASE + 0x2003f8)
#define CDMA1X_fing1QlicSectorId_SIZE            16
#define CDMA1X_fing1RSSIInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0xa1a)
#define CDMA1X_fing1RSSIInit_SIZE                16
#define CDMA1X_fing1SectorId                     (volatile uint16*)(MICRO_MDSP_BASE + 0xa24)
#define CDMA1X_fing1SectorId_SIZE                16
#define CDMA1X_fing1State                        (volatile uint16*)(MICRO_MDSP_BASE + 0x2003fa)
#define CDMA1X_fing1State_SIZE                   16
#define CDMA1X_fing1TTComELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0xa1e)
#define CDMA1X_fing1TTComELGain_SIZE             16
#define CDMA1X_fing1TTDivELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0xa2a)
#define CDMA1X_fing1TTDivELGain_SIZE             16
#define CDMA1X_fing1TTEnable                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2003f0)
#define CDMA1X_fing1TTEnable_SIZE                16
#define CDMA1X_fing1TdShift                      (volatile uint16*)(MICRO_MDSP_BASE + 0xa30)
#define CDMA1X_fing1TdShift_SIZE                 16
#define CDMA1X_fing2ComCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x20043c)
#define CDMA1X_fing2ComCombLock_SIZE             16
#define CDMA1X_fing2ComFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x4009c0)
#define CDMA1X_fing2ComFiltRSSI_SIZE             32
#define CDMA1X_fing2ComFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0xa7a)
#define CDMA1X_fing2ComFingLock_SIZE             16
#define CDMA1X_fing2ComPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0xa6a)
#define CDMA1X_fing2ComPilotFiltI_SIZE           16
#define CDMA1X_fing2ComPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x200434)
#define CDMA1X_fing2ComPilotFiltQ_SIZE           16
#define CDMA1X_fing2ComPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0xa64)
#define CDMA1X_fing2ComPilotGain_SIZE            16
#define CDMA1X_fing2ComWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xa90)
#define CDMA1X_fing2ComWtI_SIZE                  16
#define CDMA1X_fing2ComWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x200454)
#define CDMA1X_fing2ComWtQ_SIZE                  16
#define CDMA1X_fing2CombEnable                   (volatile uint16*)(MICRO_MDSP_BASE + 0xa5c)
#define CDMA1X_fing2CombEnable_SIZE              16
#define CDMA1X_fing2DivCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0xa7c)
#define CDMA1X_fing2DivCombLock_SIZE             16
#define CDMA1X_fing2DivFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x4009c4)
#define CDMA1X_fing2DivFiltRSSI_SIZE             32
#define CDMA1X_fing2DivFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x20043e)
#define CDMA1X_fing2DivFingLock_SIZE             16
#define CDMA1X_fing2DivPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0xa6c)
#define CDMA1X_fing2DivPilotFiltI_SIZE           16
#define CDMA1X_fing2DivPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x200436)
#define CDMA1X_fing2DivPilotFiltQ_SIZE           16
#define CDMA1X_fing2DivPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0xa70)
#define CDMA1X_fing2DivPilotGain_SIZE            16
#define CDMA1X_fing2DivWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xa92)
#define CDMA1X_fing2DivWtI_SIZE                  16
#define CDMA1X_fing2DivWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x200456)
#define CDMA1X_fing2DivWtQ_SIZE                  16
#define CDMA1X_fing2FiltRSSI                     (volatile uint32*)(MICRO_MDSP_BASE + 0x4009bc)
#define CDMA1X_fing2FiltRSSI_SIZE                32
#define CDMA1X_fing2FingCfgWr                    (volatile uint32*)(MICRO_MDSP_BASE + 0x4009b4)
#define CDMA1X_fing2FingCfgWr_SIZE               32
#define CDMA1X_fing2FingCurrentCfg               (volatile uint32*)(MICRO_MDSP_BASE + 0x4009b8)
#define CDMA1X_fing2FingCurrentCfg_SIZE          32
#define CDMA1X_fing2ForceLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0x20042e)
#define CDMA1X_fing2ForceLock_SIZE               16
#define CDMA1X_fing2ForceLockState               (volatile uint16*)(MICRO_MDSP_BASE + 0xa60)
#define CDMA1X_fing2ForceLockState_SIZE          16
#define CDMA1X_fing2Init                         (volatile uint16*)(MICRO_MDSP_BASE + 0xa9a)
#define CDMA1X_fing2Init_SIZE                    16
#define CDMA1X_fing2LockInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0x20042c)
#define CDMA1X_fing2LockInit_SIZE                16
#define CDMA1X_fing2MmseFiltA00I                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200442)
#define CDMA1X_fing2MmseFiltA00I_SIZE            16
#define CDMA1X_fing2MmseFiltA01I                 (volatile uint16*)(MICRO_MDSP_BASE + 0xa82)
#define CDMA1X_fing2MmseFiltA01I_SIZE            16
#define CDMA1X_fing2MmseFiltA01Q                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200440)
#define CDMA1X_fing2MmseFiltA01Q_SIZE            16
#define CDMA1X_fing2MmseFiltA11I                 (volatile uint16*)(MICRO_MDSP_BASE + 0xa80)
#define CDMA1X_fing2MmseFiltA11I_SIZE            16
#define CDMA1X_fing2MmseG00I                     (volatile uint16*)(MICRO_MDSP_BASE + 0xa84)
#define CDMA1X_fing2MmseG00I_SIZE                16
#define CDMA1X_fing2MmseG01I                     (volatile uint16*)(MICRO_MDSP_BASE + 0xa86)
#define CDMA1X_fing2MmseG01I_SIZE                16
#define CDMA1X_fing2MmseG01Q                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200444)
#define CDMA1X_fing2MmseG01Q_SIZE                16
#define CDMA1X_fing2MmseG11I                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200446)
#define CDMA1X_fing2MmseG11I_SIZE                16
#define CDMA1X_fing2PilotStr                     (volatile uint16*)(MICRO_MDSP_BASE + 0xa72)
#define CDMA1X_fing2PilotStr_SIZE                16
#define CDMA1X_fing2PowerLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0xa7e)
#define CDMA1X_fing2PowerLock_SIZE               16
#define CDMA1X_fing2PowerLockThresh              (volatile uint16*)(MICRO_MDSP_BASE + 0x200432)
#define CDMA1X_fing2PowerLockThresh_SIZE         16
#define CDMA1X_fing2PwrCtlCell                   (volatile uint16*)(MICRO_MDSP_BASE + 0xa66)
#define CDMA1X_fing2PwrCtlCell_SIZE              16
#define CDMA1X_fing2QlicCfg                      (volatile uint32*)(MICRO_MDSP_BASE + 0x4009dc)
#define CDMA1X_fing2QlicCfg_SIZE                 32
#define CDMA1X_fing2QlicSectorId                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200438)
#define CDMA1X_fing2QlicSectorId_SIZE            16
#define CDMA1X_fing2RSSIInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0xa5e)
#define CDMA1X_fing2RSSIInit_SIZE                16
#define CDMA1X_fing2SectorId                     (volatile uint16*)(MICRO_MDSP_BASE + 0xa68)
#define CDMA1X_fing2SectorId_SIZE                16
#define CDMA1X_fing2State                        (volatile uint16*)(MICRO_MDSP_BASE + 0x20043a)
#define CDMA1X_fing2State_SIZE                   16
#define CDMA1X_fing2TTComELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0xa62)
#define CDMA1X_fing2TTComELGain_SIZE             16
#define CDMA1X_fing2TTDivELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0xa6e)
#define CDMA1X_fing2TTDivELGain_SIZE             16
#define CDMA1X_fing2TTEnable                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200430)
#define CDMA1X_fing2TTEnable_SIZE                16
#define CDMA1X_fing2TdShift                      (volatile uint16*)(MICRO_MDSP_BASE + 0xa74)
#define CDMA1X_fing2TdShift_SIZE                 16
#define CDMA1X_fing3ComCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x20047c)
#define CDMA1X_fing3ComCombLock_SIZE             16
#define CDMA1X_fing3ComFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x4009f0)
#define CDMA1X_fing3ComFiltRSSI_SIZE             32
#define CDMA1X_fing3ComFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0xabe)
#define CDMA1X_fing3ComFingLock_SIZE             16
#define CDMA1X_fing3ComPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0xaae)
#define CDMA1X_fing3ComPilotFiltI_SIZE           16
#define CDMA1X_fing3ComPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x200474)
#define CDMA1X_fing3ComPilotFiltQ_SIZE           16
#define CDMA1X_fing3ComPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0xaa8)
#define CDMA1X_fing3ComPilotGain_SIZE            16
#define CDMA1X_fing3ComWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xad4)
#define CDMA1X_fing3ComWtI_SIZE                  16
#define CDMA1X_fing3ComWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x200494)
#define CDMA1X_fing3ComWtQ_SIZE                  16
#define CDMA1X_fing3CombEnable                   (volatile uint16*)(MICRO_MDSP_BASE + 0xaa0)
#define CDMA1X_fing3CombEnable_SIZE              16
#define CDMA1X_fing3DivCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0xac0)
#define CDMA1X_fing3DivCombLock_SIZE             16
#define CDMA1X_fing3DivFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x4009f4)
#define CDMA1X_fing3DivFiltRSSI_SIZE             32
#define CDMA1X_fing3DivFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x20047e)
#define CDMA1X_fing3DivFingLock_SIZE             16
#define CDMA1X_fing3DivPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0xab0)
#define CDMA1X_fing3DivPilotFiltI_SIZE           16
#define CDMA1X_fing3DivPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x200476)
#define CDMA1X_fing3DivPilotFiltQ_SIZE           16
#define CDMA1X_fing3DivPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0xab4)
#define CDMA1X_fing3DivPilotGain_SIZE            16
#define CDMA1X_fing3DivWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xad6)
#define CDMA1X_fing3DivWtI_SIZE                  16
#define CDMA1X_fing3DivWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x200496)
#define CDMA1X_fing3DivWtQ_SIZE                  16
#define CDMA1X_fing3FiltRSSI                     (volatile uint32*)(MICRO_MDSP_BASE + 0x4009ec)
#define CDMA1X_fing3FiltRSSI_SIZE                32
#define CDMA1X_fing3FingCfgWr                    (volatile uint32*)(MICRO_MDSP_BASE + 0x4009e4)
#define CDMA1X_fing3FingCfgWr_SIZE               32
#define CDMA1X_fing3FingCurrentCfg               (volatile uint32*)(MICRO_MDSP_BASE + 0x4009e8)
#define CDMA1X_fing3FingCurrentCfg_SIZE          32
#define CDMA1X_fing3ForceLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0x20046e)
#define CDMA1X_fing3ForceLock_SIZE               16
#define CDMA1X_fing3ForceLockState               (volatile uint16*)(MICRO_MDSP_BASE + 0xaa4)
#define CDMA1X_fing3ForceLockState_SIZE          16
#define CDMA1X_fing3Init                         (volatile uint16*)(MICRO_MDSP_BASE + 0xade)
#define CDMA1X_fing3Init_SIZE                    16
#define CDMA1X_fing3LockInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0x20046c)
#define CDMA1X_fing3LockInit_SIZE                16
#define CDMA1X_fing3MmseFiltA00I                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200482)
#define CDMA1X_fing3MmseFiltA00I_SIZE            16
#define CDMA1X_fing3MmseFiltA01I                 (volatile uint16*)(MICRO_MDSP_BASE + 0xac6)
#define CDMA1X_fing3MmseFiltA01I_SIZE            16
#define CDMA1X_fing3MmseFiltA01Q                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200480)
#define CDMA1X_fing3MmseFiltA01Q_SIZE            16
#define CDMA1X_fing3MmseFiltA11I                 (volatile uint16*)(MICRO_MDSP_BASE + 0xac4)
#define CDMA1X_fing3MmseFiltA11I_SIZE            16
#define CDMA1X_fing3MmseG00I                     (volatile uint16*)(MICRO_MDSP_BASE + 0xac8)
#define CDMA1X_fing3MmseG00I_SIZE                16
#define CDMA1X_fing3MmseG01I                     (volatile uint16*)(MICRO_MDSP_BASE + 0xaca)
#define CDMA1X_fing3MmseG01I_SIZE                16
#define CDMA1X_fing3MmseG01Q                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200484)
#define CDMA1X_fing3MmseG01Q_SIZE                16
#define CDMA1X_fing3MmseG11I                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200486)
#define CDMA1X_fing3MmseG11I_SIZE                16
#define CDMA1X_fing3PilotStr                     (volatile uint16*)(MICRO_MDSP_BASE + 0xab6)
#define CDMA1X_fing3PilotStr_SIZE                16
#define CDMA1X_fing3PowerLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0xac2)
#define CDMA1X_fing3PowerLock_SIZE               16
#define CDMA1X_fing3PowerLockThresh              (volatile uint16*)(MICRO_MDSP_BASE + 0x200472)
#define CDMA1X_fing3PowerLockThresh_SIZE         16
#define CDMA1X_fing3PwrCtlCell                   (volatile uint16*)(MICRO_MDSP_BASE + 0xaaa)
#define CDMA1X_fing3PwrCtlCell_SIZE              16
#define CDMA1X_fing3QlicCfg                      (volatile uint32*)(MICRO_MDSP_BASE + 0x400a0c)
#define CDMA1X_fing3QlicCfg_SIZE                 32
#define CDMA1X_fing3QlicSectorId                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200478)
#define CDMA1X_fing3QlicSectorId_SIZE            16
#define CDMA1X_fing3RSSIInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0xaa2)
#define CDMA1X_fing3RSSIInit_SIZE                16
#define CDMA1X_fing3SectorId                     (volatile uint16*)(MICRO_MDSP_BASE + 0xaac)
#define CDMA1X_fing3SectorId_SIZE                16
#define CDMA1X_fing3State                        (volatile uint16*)(MICRO_MDSP_BASE + 0x20047a)
#define CDMA1X_fing3State_SIZE                   16
#define CDMA1X_fing3TTComELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0xaa6)
#define CDMA1X_fing3TTComELGain_SIZE             16
#define CDMA1X_fing3TTDivELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0xab2)
#define CDMA1X_fing3TTDivELGain_SIZE             16
#define CDMA1X_fing3TTEnable                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200470)
#define CDMA1X_fing3TTEnable_SIZE                16
#define CDMA1X_fing3TdShift                      (volatile uint16*)(MICRO_MDSP_BASE + 0xab8)
#define CDMA1X_fing3TdShift_SIZE                 16
#define CDMA1X_fing4ComCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x2004bc)
#define CDMA1X_fing4ComCombLock_SIZE             16
#define CDMA1X_fing4ComFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x400a20)
#define CDMA1X_fing4ComFiltRSSI_SIZE             32
#define CDMA1X_fing4ComFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0xb02)
#define CDMA1X_fing4ComFingLock_SIZE             16
#define CDMA1X_fing4ComPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0xaf2)
#define CDMA1X_fing4ComPilotFiltI_SIZE           16
#define CDMA1X_fing4ComPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x2004b4)
#define CDMA1X_fing4ComPilotFiltQ_SIZE           16
#define CDMA1X_fing4ComPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0xaec)
#define CDMA1X_fing4ComPilotGain_SIZE            16
#define CDMA1X_fing4ComWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xb18)
#define CDMA1X_fing4ComWtI_SIZE                  16
#define CDMA1X_fing4ComWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x2004d4)
#define CDMA1X_fing4ComWtQ_SIZE                  16
#define CDMA1X_fing4CombEnable                   (volatile uint16*)(MICRO_MDSP_BASE + 0xae4)
#define CDMA1X_fing4CombEnable_SIZE              16
#define CDMA1X_fing4DivCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0xb04)
#define CDMA1X_fing4DivCombLock_SIZE             16
#define CDMA1X_fing4DivFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x400a24)
#define CDMA1X_fing4DivFiltRSSI_SIZE             32
#define CDMA1X_fing4DivFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x2004be)
#define CDMA1X_fing4DivFingLock_SIZE             16
#define CDMA1X_fing4DivPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0xaf4)
#define CDMA1X_fing4DivPilotFiltI_SIZE           16
#define CDMA1X_fing4DivPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x2004b6)
#define CDMA1X_fing4DivPilotFiltQ_SIZE           16
#define CDMA1X_fing4DivPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0xaf8)
#define CDMA1X_fing4DivPilotGain_SIZE            16
#define CDMA1X_fing4DivWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xb1a)
#define CDMA1X_fing4DivWtI_SIZE                  16
#define CDMA1X_fing4DivWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x2004d6)
#define CDMA1X_fing4DivWtQ_SIZE                  16
#define CDMA1X_fing4FiltRSSI                     (volatile uint32*)(MICRO_MDSP_BASE + 0x400a1c)
#define CDMA1X_fing4FiltRSSI_SIZE                32
#define CDMA1X_fing4FingCfgWr                    (volatile uint32*)(MICRO_MDSP_BASE + 0x400a14)
#define CDMA1X_fing4FingCfgWr_SIZE               32
#define CDMA1X_fing4FingCurrentCfg               (volatile uint32*)(MICRO_MDSP_BASE + 0x400a18)
#define CDMA1X_fing4FingCurrentCfg_SIZE          32
#define CDMA1X_fing4ForceLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0x2004ae)
#define CDMA1X_fing4ForceLock_SIZE               16
#define CDMA1X_fing4ForceLockState               (volatile uint16*)(MICRO_MDSP_BASE + 0xae8)
#define CDMA1X_fing4ForceLockState_SIZE          16
#define CDMA1X_fing4Init                         (volatile uint16*)(MICRO_MDSP_BASE + 0xb22)
#define CDMA1X_fing4Init_SIZE                    16
#define CDMA1X_fing4LockInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2004ac)
#define CDMA1X_fing4LockInit_SIZE                16
#define CDMA1X_fing4MmseFiltA00I                 (volatile uint16*)(MICRO_MDSP_BASE + 0x2004c2)
#define CDMA1X_fing4MmseFiltA00I_SIZE            16
#define CDMA1X_fing4MmseFiltA01I                 (volatile uint16*)(MICRO_MDSP_BASE + 0xb0a)
#define CDMA1X_fing4MmseFiltA01I_SIZE            16
#define CDMA1X_fing4MmseFiltA01Q                 (volatile uint16*)(MICRO_MDSP_BASE + 0x2004c0)
#define CDMA1X_fing4MmseFiltA01Q_SIZE            16
#define CDMA1X_fing4MmseFiltA11I                 (volatile uint16*)(MICRO_MDSP_BASE + 0xb08)
#define CDMA1X_fing4MmseFiltA11I_SIZE            16
#define CDMA1X_fing4MmseG00I                     (volatile uint16*)(MICRO_MDSP_BASE + 0xb0c)
#define CDMA1X_fing4MmseG00I_SIZE                16
#define CDMA1X_fing4MmseG01I                     (volatile uint16*)(MICRO_MDSP_BASE + 0xb0e)
#define CDMA1X_fing4MmseG01I_SIZE                16
#define CDMA1X_fing4MmseG01Q                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2004c4)
#define CDMA1X_fing4MmseG01Q_SIZE                16
#define CDMA1X_fing4MmseG11I                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2004c6)
#define CDMA1X_fing4MmseG11I_SIZE                16
#define CDMA1X_fing4PilotStr                     (volatile uint16*)(MICRO_MDSP_BASE + 0xafa)
#define CDMA1X_fing4PilotStr_SIZE                16
#define CDMA1X_fing4PowerLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0xb06)
#define CDMA1X_fing4PowerLock_SIZE               16
#define CDMA1X_fing4PowerLockThresh              (volatile uint16*)(MICRO_MDSP_BASE + 0x2004b2)
#define CDMA1X_fing4PowerLockThresh_SIZE         16
#define CDMA1X_fing4PwrCtlCell                   (volatile uint16*)(MICRO_MDSP_BASE + 0xaee)
#define CDMA1X_fing4PwrCtlCell_SIZE              16
#define CDMA1X_fing4QlicCfg                      (volatile uint32*)(MICRO_MDSP_BASE + 0x400a3c)
#define CDMA1X_fing4QlicCfg_SIZE                 32
#define CDMA1X_fing4QlicSectorId                 (volatile uint16*)(MICRO_MDSP_BASE + 0x2004b8)
#define CDMA1X_fing4QlicSectorId_SIZE            16
#define CDMA1X_fing4RSSIInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0xae6)
#define CDMA1X_fing4RSSIInit_SIZE                16
#define CDMA1X_fing4SectorId                     (volatile uint16*)(MICRO_MDSP_BASE + 0xaf0)
#define CDMA1X_fing4SectorId_SIZE                16
#define CDMA1X_fing4State                        (volatile uint16*)(MICRO_MDSP_BASE + 0x2004ba)
#define CDMA1X_fing4State_SIZE                   16
#define CDMA1X_fing4TTComELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0xaea)
#define CDMA1X_fing4TTComELGain_SIZE             16
#define CDMA1X_fing4TTDivELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0xaf6)
#define CDMA1X_fing4TTDivELGain_SIZE             16
#define CDMA1X_fing4TTEnable                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2004b0)
#define CDMA1X_fing4TTEnable_SIZE                16
#define CDMA1X_fing4TdShift                      (volatile uint16*)(MICRO_MDSP_BASE + 0xafc)
#define CDMA1X_fing4TdShift_SIZE                 16
#define CDMA1X_fing5ComCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x2004fc)
#define CDMA1X_fing5ComCombLock_SIZE             16
#define CDMA1X_fing5ComFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x400a50)
#define CDMA1X_fing5ComFiltRSSI_SIZE             32
#define CDMA1X_fing5ComFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0xb46)
#define CDMA1X_fing5ComFingLock_SIZE             16
#define CDMA1X_fing5ComPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0xb36)
#define CDMA1X_fing5ComPilotFiltI_SIZE           16
#define CDMA1X_fing5ComPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x2004f4)
#define CDMA1X_fing5ComPilotFiltQ_SIZE           16
#define CDMA1X_fing5ComPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0xb30)
#define CDMA1X_fing5ComPilotGain_SIZE            16
#define CDMA1X_fing5ComWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xb5c)
#define CDMA1X_fing5ComWtI_SIZE                  16
#define CDMA1X_fing5ComWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x200514)
#define CDMA1X_fing5ComWtQ_SIZE                  16
#define CDMA1X_fing5CombEnable                   (volatile uint16*)(MICRO_MDSP_BASE + 0xb28)
#define CDMA1X_fing5CombEnable_SIZE              16
#define CDMA1X_fing5DivCombLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0xb48)
#define CDMA1X_fing5DivCombLock_SIZE             16
#define CDMA1X_fing5DivFiltRSSI                  (volatile uint32*)(MICRO_MDSP_BASE + 0x400a54)
#define CDMA1X_fing5DivFiltRSSI_SIZE             32
#define CDMA1X_fing5DivFingLock                  (volatile uint16*)(MICRO_MDSP_BASE + 0x2004fe)
#define CDMA1X_fing5DivFingLock_SIZE             16
#define CDMA1X_fing5DivPilotFiltI                (volatile uint16*)(MICRO_MDSP_BASE + 0xb38)
#define CDMA1X_fing5DivPilotFiltI_SIZE           16
#define CDMA1X_fing5DivPilotFiltQ                (volatile uint16*)(MICRO_MDSP_BASE + 0x2004f6)
#define CDMA1X_fing5DivPilotFiltQ_SIZE           16
#define CDMA1X_fing5DivPilotGain                 (volatile uint16*)(MICRO_MDSP_BASE + 0xb3c)
#define CDMA1X_fing5DivPilotGain_SIZE            16
#define CDMA1X_fing5DivWtI                       (volatile uint16*)(MICRO_MDSP_BASE + 0xb5e)
#define CDMA1X_fing5DivWtI_SIZE                  16
#define CDMA1X_fing5DivWtQ                       (volatile uint16*)(MICRO_MDSP_BASE + 0x200516)
#define CDMA1X_fing5DivWtQ_SIZE                  16
#define CDMA1X_fing5FiltRSSI                     (volatile uint32*)(MICRO_MDSP_BASE + 0x400a4c)
#define CDMA1X_fing5FiltRSSI_SIZE                32
#define CDMA1X_fing5FingCfgWr                    (volatile uint32*)(MICRO_MDSP_BASE + 0x400a44)
#define CDMA1X_fing5FingCfgWr_SIZE               32
#define CDMA1X_fing5FingCurrentCfg               (volatile uint32*)(MICRO_MDSP_BASE + 0x400a48)
#define CDMA1X_fing5FingCurrentCfg_SIZE          32
#define CDMA1X_fing5ForceLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0x2004ee)
#define CDMA1X_fing5ForceLock_SIZE               16
#define CDMA1X_fing5ForceLockState               (volatile uint16*)(MICRO_MDSP_BASE + 0xb2c)
#define CDMA1X_fing5ForceLockState_SIZE          16
#define CDMA1X_fing5Init                         (volatile uint16*)(MICRO_MDSP_BASE + 0xb66)
#define CDMA1X_fing5Init_SIZE                    16
#define CDMA1X_fing5LockInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2004ec)
#define CDMA1X_fing5LockInit_SIZE                16
#define CDMA1X_fing5MmseFiltA00I                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200502)
#define CDMA1X_fing5MmseFiltA00I_SIZE            16
#define CDMA1X_fing5MmseFiltA01I                 (volatile uint16*)(MICRO_MDSP_BASE + 0xb4e)
#define CDMA1X_fing5MmseFiltA01I_SIZE            16
#define CDMA1X_fing5MmseFiltA01Q                 (volatile uint16*)(MICRO_MDSP_BASE + 0x200500)
#define CDMA1X_fing5MmseFiltA01Q_SIZE            16
#define CDMA1X_fing5MmseFiltA11I                 (volatile uint16*)(MICRO_MDSP_BASE + 0xb4c)
#define CDMA1X_fing5MmseFiltA11I_SIZE            16
#define CDMA1X_fing5MmseG00I                     (volatile uint16*)(MICRO_MDSP_BASE + 0xb50)
#define CDMA1X_fing5MmseG00I_SIZE                16
#define CDMA1X_fing5MmseG01I                     (volatile uint16*)(MICRO_MDSP_BASE + 0xb52)
#define CDMA1X_fing5MmseG01I_SIZE                16
#define CDMA1X_fing5MmseG01Q                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200504)
#define CDMA1X_fing5MmseG01Q_SIZE                16
#define CDMA1X_fing5MmseG11I                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200506)
#define CDMA1X_fing5MmseG11I_SIZE                16
#define CDMA1X_fing5PilotStr                     (volatile uint16*)(MICRO_MDSP_BASE + 0xb3e)
#define CDMA1X_fing5PilotStr_SIZE                16
#define CDMA1X_fing5PowerLock                    (volatile uint16*)(MICRO_MDSP_BASE + 0xb4a)
#define CDMA1X_fing5PowerLock_SIZE               16
#define CDMA1X_fing5PowerLockThresh              (volatile uint16*)(MICRO_MDSP_BASE + 0x2004f2)
#define CDMA1X_fing5PowerLockThresh_SIZE         16
#define CDMA1X_fing5PwrCtlCell                   (volatile uint16*)(MICRO_MDSP_BASE + 0xb32)
#define CDMA1X_fing5PwrCtlCell_SIZE              16
#define CDMA1X_fing5QlicCfg                      (volatile uint32*)(MICRO_MDSP_BASE + 0x400a6c)
#define CDMA1X_fing5QlicCfg_SIZE                 32
#define CDMA1X_fing5QlicSectorId                 (volatile uint16*)(MICRO_MDSP_BASE + 0x2004f8)
#define CDMA1X_fing5QlicSectorId_SIZE            16
#define CDMA1X_fing5RSSIInit                     (volatile uint16*)(MICRO_MDSP_BASE + 0xb2a)
#define CDMA1X_fing5RSSIInit_SIZE                16
#define CDMA1X_fing5SectorId                     (volatile uint16*)(MICRO_MDSP_BASE + 0xb34)
#define CDMA1X_fing5SectorId_SIZE                16
#define CDMA1X_fing5State                        (volatile uint16*)(MICRO_MDSP_BASE + 0x2004fa)
#define CDMA1X_fing5State_SIZE                   16
#define CDMA1X_fing5TTComELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0xb2e)
#define CDMA1X_fing5TTComELGain_SIZE             16
#define CDMA1X_fing5TTDivELGain                  (volatile uint16*)(MICRO_MDSP_BASE + 0xb3a)
#define CDMA1X_fing5TTDivELGain_SIZE             16
#define CDMA1X_fing5TTEnable                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2004f0)
#define CDMA1X_fing5TTEnable_SIZE                16
#define CDMA1X_fing5TdShift                      (volatile uint16*)(MICRO_MDSP_BASE + 0xb40)
#define CDMA1X_fing5TdShift_SIZE                 16
#define CDMA1X_fingLockAlgorithm                 (volatile uint16*)(MICRO_MDSP_BASE + 0x20014e)
#define CDMA1X_fingLockAlgorithm_SIZE            16
#define CDMA1X_fingLowerLockThresh               (volatile uint16*)(MICRO_MDSP_BASE + 0x20014c)
#define CDMA1X_fingLowerLockThresh_SIZE          16
#define CDMA1X_fingRelativeLockThresh            (volatile uint16*)(MICRO_MDSP_BASE + 0x76a)
#define CDMA1X_fingRelativeLockThresh_SIZE       16
#define CDMA1X_fingUpperLockThresh               (volatile uint16*)(MICRO_MDSP_BASE + 0x768)
#define CDMA1X_fingUpperLockThresh_SIZE          16
#define CDMA1X_fpcHistory                        (volatile uint16*)(MICRO_MDSP_BASE + 0xbf4)
#define CDMA1X_fpcHistory_SIZE                   16
#define CDMA1X_fpcHistoryValid                   (volatile uint16*)(MICRO_MDSP_BASE + 0xbf6)
#define CDMA1X_fpcHistoryValid_SIZE              16
#define CDMA1X_fundFrameOffset                   (volatile uint16*)(MICRO_MDSP_BASE + 0x2005a6)
#define CDMA1X_fundFrameOffset_SIZE              16
#define CDMA1X_fundFrameOffsetImmLoadCmd         (volatile uint16*)(MICRO_MDSP_BASE + 0x2005a2)
#define CDMA1X_fundFrameOffsetImmLoadCmd_SIZE    16
#define CDMA1X_fundWeightedNtMagAccFrame         (volatile uint16*)(MICRO_MDSP_BASE + 0x2005c6)
#define CDMA1X_fundWeightedNtMagAccFrame_SIZE    16
#define CDMA1X_fundWeightedPilotAccFrame         (volatile uint16*)(MICRO_MDSP_BASE + 0x2005cc)
#define CDMA1X_fundWeightedPilotAccFrame_SIZE    16
#define CDMA1X_microRxfCmd                       (volatile uint16*)(MICRO_MDSP_BASE + 0x20064e)
#define CDMA1X_microRxfCmd_SIZE                  16
#define CDMA1X_mmseRyyFeedBckGain                (volatile uint16*)(MICRO_MDSP_BASE + 0x200152)
#define CDMA1X_mmseRyyFeedBckGain_SIZE           16
#define CDMA1X_mmseRyyFeedFwdGain                (volatile uint16*)(MICRO_MDSP_BASE + 0x76c)
#define CDMA1X_mmseRyyFeedFwdGain_SIZE           16
#define CDMA1X_ntIoFiltGain                      (volatile uint16*)(MICRO_MDSP_BASE + 0x770)
#define CDMA1X_ntIoFiltGain_SIZE                 16
#define CDMA1X_passthruGainScale                 (volatile uint16*)(MICRO_MDSP_BASE + 0x772)
#define CDMA1X_passthruGainScale_SIZE            16
#define CDMA1X_pcBitAccFrame                     (volatile uint16*)(MICRO_MDSP_BASE + 0x2005ce)
#define CDMA1X_pcBitAccFrame_SIZE                16
#define CDMA1X_pcFiltGain                        (volatile uint16*)(MICRO_MDSP_BASE + 0x76e)
#define CDMA1X_pcFiltGain_SIZE                   16
#define CDMA1X_pilotFiltGain                     (volatile uint16*)(MICRO_MDSP_BASE + 0x20014a)
#define CDMA1X_pilotFiltGain_SIZE                16
#define CDMA1X_qlicEngineIirScale                (volatile uint16*)(MICRO_MDSP_BASE + 0xc14)
#define CDMA1X_qlicEngineIirScale_SIZE           16
#define CDMA1X_qlicNtFiltGain                    (volatile uint16*)(MICRO_MDSP_BASE + 0x200158)
#define CDMA1X_qlicNtFiltGain_SIZE               16
#define CDMA1X_qlicSector0NoiseVal               (volatile uint16*)(MICRO_MDSP_BASE + 0xb6e)
#define CDMA1X_qlicSector0NoiseVal_SIZE          16
#define CDMA1X_qlicSector0NormVal                (volatile uint16*)(MICRO_MDSP_BASE + 0xb6c)
#define CDMA1X_qlicSector0NormVal_SIZE           16
#define CDMA1X_qlicSector0NtEstFilt              (volatile uint16*)(MICRO_MDSP_BASE + 0xbe4)
#define CDMA1X_qlicSector0NtEstFilt_SIZE         16
#define CDMA1X_qlicSector0PilotWtSum             (volatile uint16*)(MICRO_MDSP_BASE + 0xb70)
#define CDMA1X_qlicSector0PilotWtSum_SIZE        16
#define CDMA1X_qlicSector1NoiseVal               (volatile uint16*)(MICRO_MDSP_BASE + 0xb74)
#define CDMA1X_qlicSector1NoiseVal_SIZE          16
#define CDMA1X_qlicSector1NormVal                (volatile uint16*)(MICRO_MDSP_BASE + 0xb72)
#define CDMA1X_qlicSector1NormVal_SIZE           16
#define CDMA1X_qlicSector1NtEstFilt              (volatile uint16*)(MICRO_MDSP_BASE + 0xbe6)
#define CDMA1X_qlicSector1NtEstFilt_SIZE         16
#define CDMA1X_qlicSector1PilotWtSum             (volatile uint16*)(MICRO_MDSP_BASE + 0xb76)
#define CDMA1X_qlicSector1PilotWtSum_SIZE        16
#define CDMA1X_qlicSector2NoiseVal               (volatile uint16*)(MICRO_MDSP_BASE + 0xb7a)
#define CDMA1X_qlicSector2NoiseVal_SIZE          16
#define CDMA1X_qlicSector2NormVal                (volatile uint16*)(MICRO_MDSP_BASE + 0xb78)
#define CDMA1X_qlicSector2NormVal_SIZE           16
#define CDMA1X_qlicSector2NtEstFilt              (volatile uint16*)(MICRO_MDSP_BASE + 0xbe8)
#define CDMA1X_qlicSector2NtEstFilt_SIZE         16
#define CDMA1X_qlicSector2PilotWtSum             (volatile uint16*)(MICRO_MDSP_BASE + 0xb7c)
#define CDMA1X_qlicSector2PilotWtSum_SIZE        16
#define CDMA1X_qlicSector3NoiseVal               (volatile uint16*)(MICRO_MDSP_BASE + 0xb80)
#define CDMA1X_qlicSector3NoiseVal_SIZE          16
#define CDMA1X_qlicSector3NormVal                (volatile uint16*)(MICRO_MDSP_BASE + 0xb7e)
#define CDMA1X_qlicSector3NormVal_SIZE           16
#define CDMA1X_qlicSector3NtEstFilt              (volatile uint16*)(MICRO_MDSP_BASE + 0xbea)
#define CDMA1X_qlicSector3NtEstFilt_SIZE         16
#define CDMA1X_qlicSector3PilotWtSum             (volatile uint16*)(MICRO_MDSP_BASE + 0xb82)
#define CDMA1X_qlicSector3PilotWtSum_SIZE        16
#define CDMA1X_qlicSector4NoiseVal               (volatile uint16*)(MICRO_MDSP_BASE + 0xb86)
#define CDMA1X_qlicSector4NoiseVal_SIZE          16
#define CDMA1X_qlicSector4NormVal                (volatile uint16*)(MICRO_MDSP_BASE + 0xb84)
#define CDMA1X_qlicSector4NormVal_SIZE           16
#define CDMA1X_qlicSector4NtEstFilt              (volatile uint16*)(MICRO_MDSP_BASE + 0xbec)
#define CDMA1X_qlicSector4NtEstFilt_SIZE         16
#define CDMA1X_qlicSector4PilotWtSum             (volatile uint16*)(MICRO_MDSP_BASE + 0xb88)
#define CDMA1X_qlicSector4PilotWtSum_SIZE        16
#define CDMA1X_qlicSector5NoiseVal               (volatile uint16*)(MICRO_MDSP_BASE + 0xb8c)
#define CDMA1X_qlicSector5NoiseVal_SIZE          16
#define CDMA1X_qlicSector5NormVal                (volatile uint16*)(MICRO_MDSP_BASE + 0xb8a)
#define CDMA1X_qlicSector5NormVal_SIZE           16
#define CDMA1X_qlicSector5NtEstFilt              (volatile uint16*)(MICRO_MDSP_BASE + 0xbee)
#define CDMA1X_qlicSector5NtEstFilt_SIZE         16
#define CDMA1X_qlicSector5PilotWtSum             (volatile uint16*)(MICRO_MDSP_BASE + 0xb8e)
#define CDMA1X_qlicSector5PilotWtSum_SIZE        16
#define CDMA1X_qpIndI                            (volatile uint16*)(MICRO_MDSP_BASE + 0xc20)
#define CDMA1X_qpIndI_SIZE                       16
#define CDMA1X_qpIndQ                            (volatile uint16*)(MICRO_MDSP_BASE + 0x20060a)
#define CDMA1X_qpIndQ_SIZE                       16
#define CDMA1X_qpThresh1                         (volatile uint16*)(MICRO_MDSP_BASE + 0xc22)
#define CDMA1X_qpThresh1_SIZE                    16
#define CDMA1X_qpThresh2                         (volatile uint16*)(MICRO_MDSP_BASE + 0x20060c)
#define CDMA1X_qpThresh2_SIZE                    16
#define CDMA1X_qpUseFiltPilot                    (volatile uint16*)(MICRO_MDSP_BASE + 0x200150)
#define CDMA1X_qpUseFiltPilot_SIZE               16
#define CDMA1X_rpcHistory                        (volatile uint16*)(MICRO_MDSP_BASE + 0xbf0)
#define CDMA1X_rpcHistory_SIZE                   16
#define CDMA1X_rpcHistoryValid                   (volatile uint16*)(MICRO_MDSP_BASE + 0xbf2)
#define CDMA1X_rpcHistoryValid_SIZE              16
#define CDMA1X_rssiFiltGain                      (volatile uint16*)(MICRO_MDSP_BASE + 0x766)
#define CDMA1X_rssiFiltGain_SIZE                 16
#define CDMA1X_rxf0Deboost                       (volatile uint16*)(MICRO_MDSP_BASE + 0x792)
#define CDMA1X_rxf0Deboost_SIZE                  16
#define CDMA1X_rxf0JamdetCfg                     (volatile uint32*)(MICRO_MDSP_BASE + 0x4005c8)
#define CDMA1X_rxf0JamdetCfg_SIZE                32
#define CDMA1X_rxf0JamdetEn                      (volatile uint16*)(MICRO_MDSP_BASE + 0x200102)
#define CDMA1X_rxf0JamdetEn_SIZE                 16
#define CDMA1X_rxf0JamdetThresh                  (volatile uint32*)(MICRO_MDSP_BASE + 0x4001bc)
#define CDMA1X_rxf0JamdetThresh_SIZE             32
#define CDMA1X_rxfCmdIntCount                    (volatile uint16*)(MICRO_MDSP_BASE + 0xc82)
#define CDMA1X_rxfCmdIntCount_SIZE               16
#define CDMA1X_rxfIntCount                       (volatile uint16*)(MICRO_MDSP_BASE + 0xc86)
#define CDMA1X_rxfIntCount_SIZE                  16
#define CDMA1X_rxfIntFlag                        (volatile uint16*)(MICRO_MDSP_BASE + 0xc84)
#define CDMA1X_rxfIntFlag_SIZE                   16
#define CDMA1X_rxfIrqStatus                      (volatile uint32*)(MICRO_MDSP_BASE + 0x4005cc)
#define CDMA1X_rxfIrqStatus_SIZE                 32
#define CDMA1X_schIqAccScale                     (volatile uint16*)(MICRO_MDSP_BASE + 0x200618)
#define CDMA1X_schIqAccScale_SIZE                16
#define CDMA1X_suppWeightedNtMagAccFrame         (volatile uint16*)(MICRO_MDSP_BASE + 0x2005c8)
#define CDMA1X_suppWeightedNtMagAccFrame_SIZE    16
#define CDMA1X_suppWeightedPilotAccFrame         (volatile uint16*)(MICRO_MDSP_BASE + 0x2005ca)
#define CDMA1X_suppWeightedPilotAccFrame_SIZE    16
#define CDMA1X_ttInit                            (volatile uint16*)(MICRO_MDSP_BASE + 0x200148)
#define CDMA1X_ttInit_SIZE                       16
#define CDMA1X_ttK1Gain                          (volatile uint16*)(MICRO_MDSP_BASE + 0x764)
#define CDMA1X_ttK1Gain_SIZE                     16
#define CDMA1X_ttK2Gain                          (volatile uint16*)(MICRO_MDSP_BASE + 0x200146)
#define CDMA1X_ttK2Gain_SIZE                     16
#define CDMA1X_txRateAdjLatchOffset              (volatile uint16*)(MICRO_MDSP_BASE + 0x200156)
#define CDMA1X_txRateAdjLatchOffset_SIZE         16
#define CDMA1X_walshPwrCmd                       (volatile uint16*)(MICRO_MDSP_BASE + 0x20062a)
#define CDMA1X_walshPwrCmd_SIZE                  16
#define CDMA1X_walshPwrData                      (volatile uint32*)(MICRO_MDSP_BASE + 0x400a70)
#define CDMA1X_walshPwrData_SIZE                 32
#define CDMA1X_walshPwrReady                     (volatile uint16*)(MICRO_MDSP_BASE + 0xc58)
#define CDMA1X_walshPwrReady_SIZE                16
#define CDMA1X_walshPwrSector                    (volatile uint16*)(MICRO_MDSP_BASE + 0xc56)
#define CDMA1X_walshPwrSector_SIZE               16
#define MDSP_buildDate                           (volatile uint16*)(MICRO_MDSP_BASE + 0x4)
#define MDSP_buildDate_SIZE                      16
#define MDSP_buildTime                           (volatile uint16*)(MICRO_MDSP_BASE + 0x6)
#define MDSP_buildTime_SIZE                      16
#define MDSP_buildYear                           (volatile uint16*)(MICRO_MDSP_BASE + 0x2)
#define MDSP_buildYear_SIZE                      16
#define MDSP_dspActive                           (volatile uint16*)(MICRO_MDSP_BASE + 0x20000c)
#define MDSP_dspActive_SIZE                      16
#define MDSP_dspHalt                             (volatile uint16*)(MICRO_MDSP_BASE + 0x200000)
#define MDSP_dspHalt_SIZE                        16
#define MDSP_fwVersion                           (volatile uint16*)(MICRO_MDSP_BASE + 0x0)
#define MDSP_fwVersion_SIZE                      16
#define MDSP_logAutoUpdate                       (volatile uint16*)(MICRO_MDSP_BASE + 0x80)
#define MDSP_logAutoUpdate_SIZE                  16
#define MDSP_logBuffer                           (volatile uint16*)(MICRO_MDSP_BASE + 0x84)
#define MDSP_logBuffer_SIZE                      16
#define MDSP_logBufferSize                       (volatile uint16*)(MICRO_MDSP_BASE + 0x20008c)
#define MDSP_logBufferSize_SIZE                  16
#define MDSP_logReadOffset                       (volatile uint16*)(MICRO_MDSP_BASE + 0x20008a)
#define MDSP_logReadOffset_SIZE                  16
#define MDSP_logWatermark                        (volatile uint16*)(MICRO_MDSP_BASE + 0x20008e)
#define MDSP_logWatermark_SIZE                   16
#define MDSP_logWriteOffset                      (volatile uint16*)(MICRO_MDSP_BASE + 0x200088)
#define MDSP_logWriteOffset_SIZE                 16
#define MDSP_mdspAppEnable                       (volatile uint32*)(MICRO_MDSP_BASE + 0x40012c)
#define MDSP_mdspAppEnable_SIZE                  32
#define MDSP_mdspAppEnableCfg                    (volatile uint32*)(MICRO_MDSP_BASE + 0x400124)
#define MDSP_mdspAppEnableCfg_SIZE               32
#define MDSP_mdspAppPresent                      (volatile uint32*)(MICRO_MDSP_BASE + 0x400128)
#define MDSP_mdspAppPresent_SIZE                 32
#define MDSP_microMCCmd                          (volatile uint16*)(MICRO_MDSP_BASE + 0x200084)
#define MDSP_microMCCmd_SIZE                     16
#define MDSP_regAddress                          (volatile uint16*)(MICRO_MDSP_BASE + 0x4f4)
#define MDSP_regAddress_SIZE                     16
#define MDSP_regValue                            (volatile uint32*)(MICRO_MDSP_BASE + 0x4001b8)
#define MDSP_regValue_SIZE                       32
#define MDSP_swID                                (volatile uint16*)(MICRO_MDSP_BASE + 0x200002)
#define MDSP_swID_SIZE                           16
#define MDSP_timeLogEnable                       (volatile uint16*)(MICRO_MDSP_BASE + 0xa)
#define MDSP_timeLogEnable_SIZE                  16
#define MDSP_timerPeriod                         (volatile uint32*)(MICRO_MDSP_BASE + 0x400004)
#define MDSP_timerPeriod_SIZE                    32
#define SRCH_cdmaAntSelect                       (volatile uint16*)(MICRO_MDSP_BASE + 0xc94)
#define SRCH_cdmaAntSelect_SIZE                  16
#define SRCH_hdrSearchScaleQ8                    (volatile uint16*)(MICRO_MDSP_BASE + 0xc92)
#define SRCH_hdrSearchScaleQ8_SIZE               16
#define SRCH_microHpqAbortDone                   (volatile uint16*)(MICRO_MDSP_BASE + 0x200a1e)
#define SRCH_microHpqAbortDone_SIZE              16
#define SRCH_microHpqFreqOffset                  (volatile uint16*)(MICRO_MDSP_BASE + 0x200a1a)
#define SRCH_microHpqFreqOffset_SIZE             16
#define SRCH_microHpqHPQResults                  (volatile uint16*)(MICRO_MDSP_BASE + 0x200674)
#define SRCH_microHpqHPQResults_SIZE             16
#define SRCH_microHpqNumResults                  (volatile uint16*)(MICRO_MDSP_BASE + 0x200a20)
#define SRCH_microHpqNumResults_SIZE             16
#define SRCH_microHpqNumTasks                    (volatile uint16*)(MICRO_MDSP_BASE + 0x200a16)
#define SRCH_microHpqNumTasks_SIZE               16
#define SRCH_microHpqPriority                    (volatile uint16*)(MICRO_MDSP_BASE + 0x200a0c)
#define SRCH_microHpqPriority_SIZE               16
#define SRCH_microHpqSearchDone                  (volatile uint16*)(MICRO_MDSP_BASE + 0x200a1c)
#define SRCH_microHpqSearchDone_SIZE             16
#define SRCH_microHpqSearchParams                (volatile uint16*)(MICRO_MDSP_BASE + 0x200a26)
#define SRCH_microHpqSearchParams_SIZE           16
#define SRCH_microHpqSearchType                  (volatile uint16*)(MICRO_MDSP_BASE + 0x200a18)
#define SRCH_microHpqSearchType_SIZE             16
#define SRCH_microSrchCmd                        (volatile uint16*)(MICRO_MDSP_BASE + 0x2009f8)
#define SRCH_microSrchCmd_SIZE                   16
#define SRCH_microSrchData                       (volatile uint16*)(MICRO_MDSP_BASE + 0x2009fa)
#define SRCH_microSrchData_SIZE                  16
#define SRCH_oqpchDiversityMode                  (volatile uint16*)(MICRO_MDSP_BASE + 0xeda)
#define SRCH_oqpchDiversityMode_SIZE             16
#define SRCH_oqpchFiltLength                     (volatile uint16*)(MICRO_MDSP_BASE + 0xeb8)
#define SRCH_oqpchFiltLength_SIZE                16
#define SRCH_oqpchFreqEst                        (volatile uint16*)(MICRO_MDSP_BASE + 0xeb6)
#define SRCH_oqpchFreqEst_SIZE                   16
#define SRCH_oqpchIndLength                      (volatile uint16*)(MICRO_MDSP_BASE + 0xeb2)
#define SRCH_oqpchIndLength_SIZE                 16
#define SRCH_oqpchIndPosition                    (volatile uint16*)(MICRO_MDSP_BASE + 0xeb4)
#define SRCH_oqpchIndPosition_SIZE               16
#define SRCH_oqpchNumPaths                       (volatile uint16*)(MICRO_MDSP_BASE + 0xec0)
#define SRCH_oqpchNumPaths_SIZE                  16
#define SRCH_oqpchPath0Phase                     (volatile uint16*)(MICRO_MDSP_BASE + 0xec4)
#define SRCH_oqpchPath0Phase_SIZE                16
#define SRCH_oqpchPath0PosCx1                    (volatile uint16*)(MICRO_MDSP_BASE + 0xec2)
#define SRCH_oqpchPath0PosCx1_SIZE               16
#define SRCH_oqpchPath1Phase                     (volatile uint16*)(MICRO_MDSP_BASE + 0xec8)
#define SRCH_oqpchPath1Phase_SIZE                16
#define SRCH_oqpchPath1PosCx1                    (volatile uint16*)(MICRO_MDSP_BASE + 0xec6)
#define SRCH_oqpchPath1PosCx1_SIZE               16
#define SRCH_oqpchPath2Phase                     (volatile uint16*)(MICRO_MDSP_BASE + 0xecc)
#define SRCH_oqpchPath2Phase_SIZE                16
#define SRCH_oqpchPath2PosCx1                    (volatile uint16*)(MICRO_MDSP_BASE + 0xeca)
#define SRCH_oqpchPath2PosCx1_SIZE               16
#define SRCH_oqpchPath3Phase                     (volatile uint16*)(MICRO_MDSP_BASE + 0xed0)
#define SRCH_oqpchPath3Phase_SIZE                16
#define SRCH_oqpchPath3PosCx1                    (volatile uint16*)(MICRO_MDSP_BASE + 0xece)
#define SRCH_oqpchPath3PosCx1_SIZE               16
#define SRCH_oqpchPath4Phase                     (volatile uint16*)(MICRO_MDSP_BASE + 0xed4)
#define SRCH_oqpchPath4Phase_SIZE                16
#define SRCH_oqpchPath4PosCx1                    (volatile uint16*)(MICRO_MDSP_BASE + 0xed2)
#define SRCH_oqpchPath4PosCx1_SIZE               16
#define SRCH_oqpchPath5Phase                     (volatile uint16*)(MICRO_MDSP_BASE + 0xed8)
#define SRCH_oqpchPath5Phase_SIZE                16
#define SRCH_oqpchPath5PosCx1                    (volatile uint16*)(MICRO_MDSP_BASE + 0xed6)
#define SRCH_oqpchPath5PosCx1_SIZE               16
#define SRCH_oqpchPilotIndex                     (volatile uint16*)(MICRO_MDSP_BASE + 0xeba)
#define SRCH_oqpchPilotIndex_SIZE                16
#define SRCH_oqpchSampRamWritePtr                (volatile uint16*)(MICRO_MDSP_BASE + 0xebe)
#define SRCH_oqpchSampRamWritePtr_SIZE           16
#define SRCH_oqpchWalsh                          (volatile uint16*)(MICRO_MDSP_BASE + 0xebc)
#define SRCH_oqpchWalsh_SIZE                     16
#define SRCH_pathFreqOffset                      (volatile uint16*)(MICRO_MDSP_BASE + 0x2009ec)
#define SRCH_pathFreqOffset_SIZE                 16
#define SRCH_qpchBreakPoint                      (volatile uint16*)(MICRO_MDSP_BASE + 0x20066e)
#define SRCH_qpchBreakPoint_SIZE                 16
#define SRCH_qpchResultsReady                    (volatile uint16*)(MICRO_MDSP_BASE + 0x200bc4)
#define SRCH_qpchResultsReady_SIZE               16
#define SRCH_searchVersion                       (volatile uint16*)(MICRO_MDSP_BASE + 0xc88)
#define SRCH_searchVersion_SIZE                  16
#define SRCH_softIResult                         (volatile uint16*)(MICRO_MDSP_BASE + 0x200bbc)
#define SRCH_softIResult_SIZE                    16
#define SRCH_softQResult                         (volatile uint16*)(MICRO_MDSP_BASE + 0x200bbe)
#define SRCH_softQResult_SIZE                    16
#define SRCH_srchIntFlag                         (volatile uint16*)(MICRO_MDSP_BASE + 0x200bb4)
#define SRCH_srchIntFlag_SIZE                    16
#define SRCH_startQpch                           (volatile uint16*)(MICRO_MDSP_BASE + 0x200bc2)
#define SRCH_startQpch_SIZE                      16
#define SRCH_sumSoftI                            (volatile uint32*)(MICRO_MDSP_BASE + 0x400fe0)
#define SRCH_sumSoftI_SIZE                       32
#define SRCH_sumSoftQ                            (volatile uint32*)(MICRO_MDSP_BASE + 0x400fe4)
#define SRCH_sumSoftQ_SIZE                       32
#define SRCH_sumTh1                              (volatile uint32*)(MICRO_MDSP_BASE + 0x400fe8)
#define SRCH_sumTh1_SIZE                         32
#define SRCH_th1Result                           (volatile uint16*)(MICRO_MDSP_BASE + 0x200bc0)
#define SRCH_th1Result_SIZE                      16
/* END SYMBOLS */

#define MDSP_NUM_TABLE_SYMBOLS 0
#define MDSP_NUM_SOLO_SYMBOLS 535
#define MDSP_NUM_SYMBOLS 535

/* Definitions that allow MDSP firmware images to be
** checked for at compile time.
** Firmware image length in bytes.
*/
#define MDSP_IMAGE_CDMA_DEFINED
#define MDSP_IMAGE_CDMA_LENGTH 18634

#define MDSP_IMAGE_ALL_LENGTH 18634

/* The MDSP firmware images */
typedef enum
{
  MDSP_IMAGE_CDMA,
  MDSP_IMAGE_MAX,

  /* DO NOT USE: Force this enum to be a 32bit type to improve speed */
  MDSP_IMAGE_32BIT_DUMMY = 0x10000
} mdsp_image_type;

/* Enable MDSP interrupt related code */
#define MDSP_INTERRUPTS_USED

/* Function type used for registering ISRs */
typedef void (*mdsp_isr_func_type)(void);

/* Enumeration of interrupt lines used for intercepting MDSP interrupts */
typedef enum
{
  MDSP_ISR_INT0,
  MDSP_ISR_MAX,
  MDSP_ISR_UNUSED = MDSP_ISR_MAX
} mdsp_isr_type;


/*===========================================================================
   DSP Clock rate and DME requirements
===========================================================================*/
#ifdef FEATURE_MDSP_SET_IMAGE_HW_RESOURCES
typedef enum
{
  MDSP_CLOCK_RATE_0,
  MDSP_CLOCK_RATE_MAX
} mdsp_clock_rate_type;
#endif

/* ------------------ MODULE DATA ------------------ */

#define MDSP_MASTER_CONTROLLER_USED

/* Definitions that allow firmware modules to be
** checked for at compile time.
*/
#define MDSP_MODULE_SRCH_DEFINED
#define MDSP_MODULE_DEMOD_1X_DEFINED
#define MDSP_MODULE_MC_DEFINED


/* Firmware modules */
typedef enum
{
   MDSP_MODULE_SRCH,
   MDSP_MODULE_DEMOD_1X,
   MDSP_MODULE_MC,
   MDSP_MODULE_MAX,

   /* DO NOT USE: Force this enum to be a 32bit type to improve speed */
   MDSP_MODULE_32BIT_DUMMY = 0x10000
} mdsp_module_type;

/* Mode data */
typedef struct
{
   const mdsp_module_type *mode;
   const uint16           memory_config;
} mdsp_mode_data_type;

/* Data type holds all the modes supported by an image */
typedef struct
{
   const byte                num_modes;
   const mdsp_mode_data_type *modes;
} mdsp_modes_type;

/* Module selection bitmask type */
typedef uint32 mdsp_module_bitmask_type;

/* Data type holds all the module related info for an image */
typedef struct
{
   const mdsp_module_type         *modules;
   const mdsp_module_bitmask_type *bitmasks;
#ifdef MDSP_SET_MODULE_HW_RESOURCES
   const uint16                   *mips_reqd;
   const clkrgm_dme_mode_type     *dme_modes;
#endif
   const mdsp_modes_type          mode_info;
   const uint32                   mc_irq;
   const byte                     bitmask_size; /* 16 bit words */
   const boolean                  *reset_to_load;
} mdsp_module_info_type;

/* Table containing module info of all images */
extern const mdsp_module_info_type mdsp_modules_info[];

extern const mdsp_isr_type mdsp_controller_ints[];

/* Type used to return the status of a firmware download
*/
typedef enum
{
  MDSP_DOWNLOAD_SUCCESS,
  MDSP_DOWNLOAD_FAIL
} mdsp_download_status_type;

/* Type used to specify a MDSP RAM bank */
typedef enum
{
  MDSP_BANK_RAMA,
  MDSP_BANK_RAMB,
  MDSP_BANK_RAMC,
  MDSP_BANK_RAMI
} mdsp_bank_type;


/* <EJECT> */
/*===========================================================================

             A C C E S S   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/


/*===========================================================================

FUNCTION MDSP_FW_CTRL_FUNC

DESCRIPTION
  Control whether the MDSP is in Download or Run mode.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void mdsp_fw_ctrl_func ( qdspext_fw_ctrl_type fw_state );

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_IS_SUPPORTED_IMAGE

DESCRIPTION
  Parameters:
  - MDSP firmware image

  Checks if the specified image is supported in this build.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if supported; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mdsp_is_supported_image (
  /* Image to check */
  const mdsp_image_type image
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_DOWNLOAD_IMAGE

DESCRIPTION
  Parameters:
  - image, index to a download image descriptor

  Performs the download of the MDSP.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  - MDSP_DOWNLOAD_SUCCESS - Firmware download successful.
  - MDSP_DOWNLOAD_FAIL    - Firmware download failed.

SIDE EFFECTS
  The MDSP will be downloaded, but the FW will not be running.

===========================================================================*/
extern mdsp_download_status_type mdsp_download_image (
  /* Index of the image to download */ 
  const mdsp_image_type image
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_START_IMAGE

DESCRIPTION
  Start the currently loaded FW image.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void mdsp_start_image ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_UP_TO_DSP_IRQ

DESCRIPTION
  Generate a microprocessor to QDSP IRQ.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
/*lint -e773 */
#if (MDSP_REG_WORD_SIZE==1)
#define mdsp_up_to_dsp_irq()                                     \
  (*((volatile word *) ((MDSP_CTRL_REGISTER) +                   \
                        (MDSP_UP_TO_DSP_IRQ_OFFSET<<1)))) = 0x1
#else
#define mdsp_up_to_dsp_irq()                                       \
  (*((volatile uint32 *) ((MDSP_CTRL_REGISTER) +                   \
                          (MDSP_UP_TO_DSP_IRQ_OFFSET<<2)))) = 0x1
#endif
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_GEN_INTERRUPT

DESCRIPTION
  Generate any combination MDSP interrupts.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
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
#if (MDSP_REG_WORD_SIZE==1)
#define mdsp_gen_interrupt( irq_vector )                             \
  (*((volatile word *) ((MDSP_CTRL_REGISTER) +                       \
                        (MDSP_UP_TO_DSP_IRQ_TEST_OFFSET<<1)))) =     \
                                    (uint16) (((irq_vector) & 0xffff) | 0x8000)
#else
#define mdsp_gen_interrupt( irq_vector )                                \
  (*((volatile uint32 *) ((MDSP_CTRL_REGISTER) +                        \
                         (MDSP_UP_TO_DSP_IRQ_TEST_OFFSET<<2)))) =       \
                                                    ((irq_vector) | 0x80000000)
#endif
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_READ

DESCRIPTION
  Reads one value from the MDSP/ARM  shared memory area.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  Value of the MDSP variable read.

SIDE EFFECTS
  None

===========================================================================*/
#define mdsp_read(address) (*(address))

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_WRITE

DESCRIPTION
  Writes one value to the MDSP/ARM  shared memory area.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.
  
  Val must be cast to uint16 or uint32 depending on the address written to.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e773 */
#define mdsp_write(address, val) (*(address)) = (val)
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_READ_OFFSET

DESCRIPTION
  Reads one value from the MDSP/ARM  shared memory area.  The
  variable address is the base address plus the offset in words or dwords.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  Value of the MDSP variable read.

SIDE EFFECTS
  None

===========================================================================*/
#define mdsp_read_offset(address, offset) (*((address)+offset))

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_WRITE_OFFSET

DESCRIPTION
  Writes one value to the MDSP/ARM  shared memory area.  The
  variable address is the base address plus the offset in words or dwords.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e773 */
#define mdsp_write_offset(address, offset, val) (*((address) + (offset))) = (val)
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_BLOCK_READ

DESCRIPTION
  Copies a block of 16-bit values from the MDSP to the ARM memory area.
  
  Parameters:
    uint16  *var,     // The MDSP variable to read from
    int     offset,   // The offset in words from the base address
    uint16  *dest,    // The destination buffer for data read from the MDSP
    int     wlen,     // The length of the buffer in words
    boolean swap      // TRUE = swap bytes

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define mdsp_block_read(address, offset, dest, wlen, swap) \
  mdsp_memcpy((dest), (uint16*)mdsp_address((address), (uint16)(offset)), (wlen), (swap));

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_BLOCK_READ32

DESCRIPTION
  Copies a block of 32-bit values from the MDSP to the ARM memory area.
  
  Parameters:
    uint32  *var,     // The MDSP variable to read from
    int     offset,   // The offset in words from the base address
    uint32  *dest,    // The destination buffer for data read from the MDSP
    int     len,      // The length of the buffer in dwords
    boolean swap      // TRUE = swap bytes

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define mdsp_block_read32(address, offset, dest, len, swap) \
  mdsp_memcpy32((dest), (uint32*)mdsp_address((address), (uint16)(offset)), (len), (swap));

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_BLOCK_WRITE

DESCRIPTION
  Copies a block of 16-bit values from the ARM to the MDSP memory area.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint16  *var,     // The MDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint16  *src,     // The source buffer to write to the MDSP
    int     wlen,     // The length of the buffer in words
    boolean swap      // TRUE = swap bytes


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define mdsp_block_write(address, offset, src, wlen, swap) \
  mdsp_memcpy((uint16*)mdsp_address((address), (uint16)(offset)), (src), (wlen), (swap));

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_BLOCK_WRITE32

DESCRIPTION
  Copies a block of 16-bit values from the ARM to the MDSP memory area.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint32  *var,     // The MDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint32  *src,     // The source buffer to write to the MDSP
    int     len,      // The length of the buffer in dwords
    boolean swap      // TRUE = swap bytes


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define mdsp_block_write32(address, offset, src, len, swap) \
  mdsp_memcpy32((uint32*)mdsp_address((address), (uint16)(offset)), (src), (len), (swap));

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_BLOCK_SET

DESCRIPTION
  Initializes a block of 16-bit values in the MDSP memory area.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint16  *var,     // The MDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint16  *src,     // The source buffer to write to the MDSP
    int     wlen      // The length of the buffer in words


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define mdsp_block_set(address, offset, val, wlen) \
    qdsp_memset((uint16*)mdsp_address((address), (uint16)(offset)), (val), (wlen));

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_BLOCK_SET32

DESCRIPTION
  Initializes a block of 32-bit values in the MDSP memory area.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.
  
  Parameters:
    uint32  *var,     // The MDSP variable to write to
    int     offset,   // The offset in words from the base address
    uint32  *src,     // The source buffer to write to the MDSP
    int     len       // The length of the buffer in words


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define mdsp_block_set32(address, offset, val, len)              \
    qdsp_memset32((uint32*)mdsp_address((address), (uint16)(offset)), (val), (len));

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_ADDRESS

DESCRIPTION
  Returns the address of a MDSP variable in the ARM address space.

  Parameters:
    uintXX *address // The MDSP variable used as the base address
    uint16 offset   // The offset in words from the base address

DEPENDENCIES
  None

RETURN VALUE
  The ARM address of the MDSP variable.

SIDE EFFECTS
  None

===========================================================================*/
#define mdsp_address(address, offset) ((address) + (offset))

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_DSP_ADDRESS

DESCRIPTION
  Returns the address of a MDSP variable in the MDSP address space.
  
  Parameters:
    uintXX *var   // Index of the MDSP variable used as the base address
    uint16 offset // The offset in from the base address

DEPENDENCIES
  None

RETURN VALUE
  The MDSP address of the MDSP variable.

SIDE EFFECTS
  None

===========================================================================*/
#define mdsp_dsp_address(address, offset) (uint32)( (((uint32)(address))-mdsp_data_table.base_address + ((offset) << (address##_SIZE / 16)) ) >> (address##_SIZE / 16))

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_BANK_ADDRESS

DESCRIPTION
  Returns an ARM address pointer given a bank-offset and MDSP memory bank.

DEPENDENCIES
  None

RETURN VALUE
  The ARM address of the MDSP variable.

SIDE EFFECTS
  None

===========================================================================*/
extern uint16* mdsp_bank_address (
  /* The memory bank to use as the base */
  mdsp_bank_type bank,
  /* The offset in words from the start of the memory bank */
  uint16 offset
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_DIRECT_WRITE

DESCRIPTION
  Writes one 16-bit value to the MDSP/ARM shared memory area.
  
  Parameters:
    port // The DSP address of the MDSP variable to write to
    val  // The value to write into MDSP/ARM shared memory

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e773 */
#define mdsp_direct_write(port, val) \
  (*((volatile word *) (MDSP_BASE_ADDRESS+((port)<<1)))) = (word)(val)
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_DIRECT_WRITE32

DESCRIPTION
  Writes one 32-bit value to the MDSP/ARM shared memory area.
  
  Parameters:
    port // The DSP address of the MDSP variable to write to
    val  // The value to write into MDSP/ARM shared memory

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e773 */
#define mdsp_direct_write32(port, val) \
  (*((volatile dword *) (MDSP_BASE_ADDRESS+((port)<<2)))) = (dword)(val)
/*lint +e773 */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_VALID_ADDRESS

DESCRIPTION
  Checks if any address in a block of memory is within the MDSP/ARM  shared
  memory area.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - Part of the memory block is within the shared memory area.
  FALSE - No part of the memory block is within the shared memory area.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mdsp_valid_address (
  /* The start address of the memory block */
  void* start_address,
  /* The length in bytes of the memory block */
  uint16 length
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_MEMSHARE_CTRL_FUNC

DESCRIPTION
  Parameters:
  - mask for selecting the RAM bank

  Reconfigures DSP memory banks.

DEPENDENCIES
  The clock regime driving the MDSP/ARM shared memory
  must be enabled before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void mdsp_memshare_ctrl_func (
  qdsp_dsp_memconfig_type mem_share_bank
);

/* <EJECT> */
#ifdef MDSP_SET_MODULE_HW_RESOURCES
/*===========================================================================

FUNCTION MDSP_PREPARE_FOR_DSP_CLOCK_RATE_SWITCH

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
extern void mdsp_prepare_for_clock_rate_switch (
  const mdsp_clock_rate_type clock_rate
);

/*===========================================================================

FUNCTION MDSP_GET_MODULE_DME_MODE

DESCRIPTION
  Parameters:
   - image for which the DME mode is required
   - module for which the DME mode is required

  Gets the MDSP DME mode for the specified module.

DEPENDENCIES
  None.

RETURN VALUE
  DME mode.
  
SIDE EFFECTS
  None

===========================================================================*/
extern clkrgm_dme_mode_type mdsp_get_module_dme_mode (
  const mdsp_image_type   image,
  const mdsp_module_type  module
);
#endif /* MDSP_SET_MODULE_HW_RESOURCES */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_CONFIG_HW_RESOURCES_FOR_IMAGE

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
extern void mdsp_config_hw_resources_for_image (
  const mdsp_image_type image
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_CONFIG_HW_RESOURCES_FOR_SLEEP_MODE

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
extern void mdsp_config_hw_resources_for_sleep_mode (void);

#ifdef FEATURE_MDSP_SET_IMAGE_HW_RESOURCES
/*===========================================================================

FUNCTION MDSP_GET_IMAGE_CLOCK_RATE

DESCRIPTION
  Parameters:
   - image for which the clock rate is required

  Gets the default MDSP clock rate for the specified image.

DEPENDENCIES
  None.

RETURN VALUE
  Clock rate.
  
SIDE EFFECTS
  None

===========================================================================*/
extern mdsp_clock_rate_type mdsp_get_image_clock_rate (
  const mdsp_image_type   image
);

/*===========================================================================

FUNCTION MDSP_GET_IMAGE_DME_MODE

DESCRIPTION
  Parameters:
   - image for which the DME mode is required

  Gets the default MDSP DME mode for the specified image.

DEPENDENCIES
  None.

RETURN VALUE
  DME mode.
  
SIDE EFFECTS
  None

===========================================================================*/
extern clkrgm_dme_mode_type mdsp_get_image_dme_mode (
  const mdsp_image_type   image
);

/*===========================================================================

FUNCTION MDSP_GET_MIPS_FOR_CLOCK_RATE

DESCRIPTION
  Parameters:
   - clock_rate whose MIPS value is required.

  Gets the MIPS value for the specified clock rate.

DEPENDENCIES
  None.

RETURN VALUE
  MIPS value; 0 if clock_rate is invalid.
  
SIDE EFFECTS
  None

===========================================================================*/
uint16 mdsp_get_mips_for_clock_rate (
  const mdsp_clock_rate_type clock_rate
);

#endif /* FEATURE_MDSP_SET_IMAGE_HW_RESOURCES */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_GET_MODULE_BITMASK

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
extern mdsp_module_bitmask_type mdsp_get_module_bitmask (
  const mdsp_image_type   image,
  const mdsp_module_type  module
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_REGISTER_DSP_SERVICES_ISRS

DESCRIPTION
  Registers MDSP Services ISRs with tramp. These ISRs will intercept
  the MDSP interrupts and in turn invoke the ISRs of all clients that
  are currently enabled and have a valid ISR registered for the incoming
  interrupt.

DEPENDENCIES
  Tramp service must be initialized.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void mdsp_register_dsp_services_isrs (void);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_GET_PREFERRED_IMAGE

DESCRIPTION
  Parameters:
  - module for which the preferred image is required

  Gets the preferred image to use (if any), for the specified module.

DEPENDENCIES
  None.

RETURN VALUE
  image if present, MDSP_IMAGE_MAX otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern void mdsp_get_preferred_image (
  const mdsp_module_type module,
  mdsp_image_type        *image,
  boolean                *preempt
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_SET_MEMORY_ACCESSOR_FUNCS

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
extern void mdsp_set_memory_accessor_funcs (
  qdspext_accessor_funcs_struct_type *accessor_funcs
);
#endif /* MDSPEXT_H */

#ifndef MSMBITS_H
#define MSMBITS_H

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//                                                       _                 --
//                                                     _/ \_               --
//                CDMA DIGITAL CELLULAR               / \_/ \              --
//                                                    \_/ \_/              --
//                                                    / \_/ \              --
//        MOBILE STATION MODEM - MSM6500              \_/ \_/              --
//                                                      \_/                --
//                                                                         --
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
//   This file define bits for all the registers in MSM6500.
//
//---------------------------------------------------------------------------
//   QUALCOMM PROPRIETARY
//       Copyright (c) 2000, 2001 by Qualcomm Incorporated
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//                      EDIT HISTORY FOR FILE
//
// $PVCSPath:  L:/src/asw/MSM6100/vcs/msm6100bits.h_v   1.7   10 Mar 2002 21:59:00   gswiech  $
// $Header: //depot/asic/qsc1100/drivers/hw/msmbits.h#1 $ $DateTime: 2007/05/17 13:25:59 $ $Author: hvo $
//
//  This section contains comments describing changes made to the module.
//  Notice that changes are listed in reverse chronological order.
//
//
// when       who     what, where, why
// --------   ---     -------------------------------------------------------
// 12/12/06   mh      Renamed UART bits to UART1DM
// 10/21/03   dna     Changed UART3_CLK_SRC_SEL for MSM6500 clock regimes,
//                    for RUIM operation.
// 05/20/02   gb,cr,rp     Created from msm6050bits.h + autoport
//---------------------------------------------------------------------------

#include "target.h"
#include "msmshift.h"

/*===========================================================================

                          BIT DEFINITIONS

========================================================================== */

/* GP1_CS */
#define LEDS_VALUE   0x18000000


#ifdef T_MSM6100B

/*---------------------------------------------------------------------------
   MSM_CLK_FS_ON
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] ADSP_WR_CLK_FS_ON, Set (1) to enable footswitch logic in the
//      adsp_wr_clk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__ADSP_WR_CLK_FS_ON_DIS                 (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,ADSP_WR_CLK_FS_ON))

#define MSM_CLK_FS_ON_WW__ADSP_WR_CLK_FS_ON_ENA                 (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,ADSP_WR_CLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [1] ADSP_PCLK_FS_ON, Set (1) to enable footswitch logic in the
//      adsp_pclk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__ADSP_PCLK_FS_ON_DIS                   (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,ADSP_PCLK_FS_ON))

#define MSM_CLK_FS_ON_WW__ADSP_PCLK_FS_ON_ENA                   (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,ADSP_PCLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [2] ADSP_SCLK_FS_ON, Set (1) to enable footswitch logic in the
//      adsp_sclk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__ADSP_SCLK_FS_ON_DIS                   (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,ADSP_SCLK_FS_ON))

#define MSM_CLK_FS_ON_WW__ADSP_SCLK_FS_ON_ENA                   (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,ADSP_SCLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [3] ADSP_CLK_FS_ON, Set (1) to enable footswitch logic in the
//      adsp_clk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__ADSP_CLK_FS_ON_DIS                    (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,ADSP_CLK_FS_ON))

#define MSM_CLK_FS_ON_WW__ADSP_CLK_FS_ON_ENA                    (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,ADSP_CLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [4] RXDSP_WR_CLK_FS_ON, Set (1) to enable footswitch logic in the
//      rxdsp_wr_clk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__RXDSP_WR_CLK_FS_ON_DIS                (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,RXDSP_WR_CLK_FS_ON))

#define MSM_CLK_FS_ON_WW__RXDSP_WR_CLK_FS_ON_ENA                (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,RXDSP_WR_CLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [5] RXDSP_PCLK_FS_ON, Set (1) to enable footswitch logic in the
//      rxdsp_pclk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__RXDSP_PCLK_FS_ON_DIS                  (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,RXDSP_PCLK_FS_ON))

#define MSM_CLK_FS_ON_WW__RXDSP_PCLK_FS_ON_ENA                  (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,RXDSP_PCLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [6] RXDSP_SCLK_FS_ON, Set (1) to enable footswitch logic in the
//      rxdsp_sclk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__RXDSP_SCLK_FS_ON_DIS                  (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,RXDSP_SCLK_FS_ON))

#define MSM_CLK_FS_ON_WW__RXDSP_SCLK_FS_ON_ENA                  (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,RXDSP_SCLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [7] RXDSP_CLK_FS_ON, Set (1) to enable footswitch logic in the
//      rxdsp_clk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__RXDSP_CLK_FS_ON_DIS                   (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,RXDSP_CLK_FS_ON))

#define MSM_CLK_FS_ON_WW__RXDSP_CLK_FS_ON_ENA                   (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,RXDSP_CLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [8] XMEM_HCLK_FS_ON, Set (1) to enable footswitch logic in the
//      xmem_hclk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__XMEM_HCLK_FS_ON_DIS                   (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,XMEM_HCLK_FS_ON))

#define MSM_CLK_FS_ON_WW__XMEM_HCLK_FS_ON_ENA                   (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,XMEM_HCLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [9] MPMC_HCLK_FS_ON, Set (1) to enable footswitch logic in the
//      mpmc_hclk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__MPMC_HCLK_FS_ON_DIS                   (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,MPMC_HCLK_FS_ON))

#define MSM_CLK_FS_ON_WW__MPMC_HCLK_FS_ON_ENA                   (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,MPMC_HCLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [10] GRP_CLK_FS_ON, Set (1) to enable footswitch logic in the
//      grp_clk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__GRP_CLK_FS_ON_DIS                     (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,GRP_CLK_FS_ON))

#define MSM_CLK_FS_ON_WW__GRP_CLK_FS_ON_ENA                     (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,GRP_CLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [11] ETM_CLK_FS_ON, Set (1) to enable footswitch logic in the
//      etm_clk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__ETM_CLK_FS_ON_DIS                     (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,ETM_CLK_FS_ON))

#define MSM_CLK_FS_ON_WW__ETM_CLK_FS_ON_ENA                     (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,ETM_CLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [12] MMCM_CLK_FS_ON, Set (1) to enable footswitch logic in the
//      mmcm_clk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__MMCM_CLK_FS_ON_DIS                    (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,MMCM_CLK_FS_ON))

#define MSM_CLK_FS_ON_WW__MMCM_CLK_FS_ON_ENA                    (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,MMCM_CLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [13] AHB_WR_CLK_FS_ON, Set (1) to enable footswitch logic in the
//      ahb_wr_clk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__AHB_WR_CLK_FS_ON_DIS                  (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,AHB_WR_CLK_FS_ON))

#define MSM_CLK_FS_ON_WW__AHB_WR_CLK_FS_ON_ENA                  (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,AHB_WR_CLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [14] HCLK_FS_ON, Set (1) to enable footswitch logic in the
//      hclk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__HCLK_FS_ON_DIS                        (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,HCLK_FS_ON))

#define MSM_CLK_FS_ON_WW__HCLK_FS_ON_ENA                        (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,HCLK_FS_ON))

/*-------------------------------------------------------------------------*/
//      Bit [15] MICRO_CLK_FS_ON, Set (1) to enable footswitch logic in the
//      micro_clk regime.  Clear (0) to allow footswitch controller in the
//      FsCSC to control the logic.  Defaults to 0.

#define MSM_CLK_FS_ON_WW__MICRO_CLK_FS_ON_DIS                   (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,MICRO_CLK_FS_ON))

#define MSM_CLK_FS_ON_WW__MICRO_CLK_FS_ON_ENA                   (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_ON,MICRO_CLK_FS_ON))

/*---------------------------------------------------------------------------
   MSM_CLK_FS_CNT0
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bits [2:0] ADSP_CLK_FS_WAKE, these bits set the number of input clock
//      cycles to wait before turning the clock back on after clk_halt is
//      deasserted and the logic_on bit is asserted.

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_WAKE_0CYCLES            (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_WAKE_1CYCLES            (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_WAKE_2CYCLES            (2U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_WAKE_3CYCLES            (3U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_WAKE_4CYCLES            (4U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_WAKE_5CYCLES            (5U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_WAKE_6CYCLES            (6U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_WAKE_7CYCLES            (7U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_WAKE))

/*-------------------------------------------------------------------------*/
//      Bit [3] RESERVED

/*-------------------------------------------------------------------------*/
//      Bits [6:4] ADSP_CLK_FS_SLEEP, these bits set the number of input clock
//      cycles to wait before deasserting logic_on after a clk_halt is
//      received by the footswitch controller.

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_SLEEP_0CYCLES           (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_SLEEP_1CYCLES           (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_SLEEP_2CYCLES           (2U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_SLEEP_3CYCLES           (3U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_SLEEP_4CYCLES           (4U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_SLEEP_5CYCLES           (5U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_SLEEP_6CYCLES           (6U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__ADSP_CLK_FS_SLEEP_7CYCLES           (7U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,ADSP_CLK_FS_SLEEP))

/*-------------------------------------------------------------------------*/
//      Bit [7] RESERVED

/*-------------------------------------------------------------------------*/
//      Bits [10:8] RXDSP_CLK_FS_WAKE, these bits set the number of input clock
//      cycles to wait before turning the clock back on after clk_halt is
//      deasserted and the logic_on bit is asserted.

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_WAKE_0CYCLES           (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_WAKE_1CYCLES           (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_WAKE_2CYCLES           (2U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_WAKE_3CYCLES           (3U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_WAKE_4CYCLES           (4U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_WAKE_5CYCLES           (5U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_WAKE_6CYCLES           (6U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_WAKE_7CYCLES           (7U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_WAKE))

/*-------------------------------------------------------------------------*/
//      Bit [11] RESERVED

/*-------------------------------------------------------------------------*/
//      Bits [14:12] RXDSP_CLK_FS_SLEEP, these bits set the number of input clock
//      cycles to wait before deasserting logic_on after a clk_halt is
//      received by the footswitch controller.

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_SLEEP_0CYCLES          (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_SLEEP_1CYCLES          (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_SLEEP_2CYCLES          (2U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_SLEEP_3CYCLES          (3U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_SLEEP_4CYCLES          (4U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_SLEEP_5CYCLES          (5U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_SLEEP_6CYCLES          (6U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__RXDSP_CLK_FS_SLEEP_7CYCLES          (7U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,RXDSP_CLK_FS_SLEEP))

/*-------------------------------------------------------------------------*/
//      Bit [15] RESERVED

/*-------------------------------------------------------------------------*/
//      Bits [18:16] AHB_CLK_FS_WAKE, these bits set the number of input clock
//      cycles to wait before turning the clock back on after clk_halt is
//      deasserted and the logic_on bit is asserted.

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_WAKE_0CYCLES             (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_WAKE_1CYCLES             (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_WAKE_2CYCLES             (2U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_WAKE_3CYCLES             (3U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_WAKE_4CYCLES             (4U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_WAKE_5CYCLES             (5U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_WAKE_6CYCLES             (6U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_WAKE_7CYCLES             (7U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_WAKE))

/*-------------------------------------------------------------------------*/
//      Bit [19] RESERVED

/*-------------------------------------------------------------------------*/
//      Bits [22:20] AHB_CLK_FS_SLEEP, these bits set the number of input clock
//      cycles to wait before deasserting logic_on after a clk_halt is
//      received by the footswitch controller.

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_SLEEP_0CYCLES            (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_SLEEP_1CYCLES            (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_SLEEP_2CYCLES            (2U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_SLEEP_3CYCLES            (3U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_SLEEP_4CYCLES            (4U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_SLEEP_5CYCLES            (5U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_SLEEP_6CYCLES            (6U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__AHB_CLK_FS_SLEEP_7CYCLES            (7U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,AHB_CLK_FS_SLEEP))

/*-------------------------------------------------------------------------*/
//      Bit [23] RESERVED

/*-------------------------------------------------------------------------*/
//      Bits [26:24] MICRO_CLK_FS_WAKE, these bits set the number of input clock
//      cycles to wait before turning the clock back on after clk_halt is
//      deasserted and the logic_on bit is asserted.

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_WAKE_0CYCLES           (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_WAKE_1CYCLES           (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_WAKE_2CYCLES           (2U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_WAKE_3CYCLES           (3U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_WAKE_4CYCLES           (4U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_WAKE_5CYCLES           (5U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_WAKE_6CYCLES           (6U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_WAKE))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_WAKE_7CYCLES           (7U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_WAKE))

/*-------------------------------------------------------------------------*/
//      Bit [27] RESERVED

/*-------------------------------------------------------------------------*/
//      Bits [30:28] MICRO_CLK_FS_SLEEP, these bits set the number of input clock
//      cycles to wait before deasserting logic_on after a clk_halt is
//      received by the footswitch controller.

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_SLEEP_0CYCLES          (0U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_SLEEP_1CYCLES          (1U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_SLEEP_2CYCLES          (2U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_SLEEP_3CYCLES          (3U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_SLEEP_4CYCLES          (4U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_SLEEP_5CYCLES          (5U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_SLEEP_6CYCLES          (6U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_SLEEP))

#define MSM_CLK_FS_CNT0_WW__MICRO_CLK_FS_SLEEP_7CYCLES          (7U <<  \
        HWIO_SHFT(MSM_CLK_FS_CNT0,MICRO_CLK_FS_SLEEP))

#endif // T_MSM6100B

 /*---------------------------------------------------------------------------
   DPLL_INTERNAL1
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Set bits 31:22 to zero to bypass accumulator contents

#define DPLL_INTERNAL1_WW__BYPASS_ACCUM_ENA                     (0U <<   \
        HWIO_SHFT(DPLL_INTERNAL1,BYPASS_ACCUM))
/*-------------------------------------------------------------------------*/
//      Define the values of the 12-bit register containing the value of a
//      feedback counter.

#define DPLL_INTERNAL1_WW__CNTB_96MHZ                           (10U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

#define DPLL_INTERNAL1_WW__CNTB_105P6MHZ                        (11U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

#define DPLL_INTERNAL1_WW__CNTB_115P2MHZ                        (12U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

#define DPLL_INTERNAL1_WW__CNTB_124P8MHZ                        (13U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

#define DPLL_INTERNAL1_WW__CNTB_134P4MHZ                        (14U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

#define DPLL_INTERNAL1_WW__CNTB_144MHZ                          (15U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

#define DPLL_INTERNAL1_WW__CNTB_153P6MHZ                        (16U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

#define DPLL_INTERNAL1_WW__CNTB_163P2MHZ                        (17U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

#define DPLL_INTERNAL1_WW__CNTB_172P8MHZ                        (18U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

#define DPLL_INTERNAL1_WW__CNTB_182P4MHZ                        (19U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

#define DPLL_INTERNAL1_WW__CNTB_192MHZ                          (20U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTB))

/*-------------------------------------------------------------------------*/
//      Define the values of the 10-bit register containing the value of a
//      reference counter.

#define DPLL_INTERNAL1_WW__CNTA_96MHZ                           (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

#define DPLL_INTERNAL1_WW__CNTA_105P6MHZ                        (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

#define DPLL_INTERNAL1_WW__CNTA_115P2MHZ                        (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

#define DPLL_INTERNAL1_WW__CNTA_124P8MHZ                        (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

#define DPLL_INTERNAL1_WW__CNTA_134P4MHZ                        (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

#define DPLL_INTERNAL1_WW__CNTA_144MHZ                          (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

#define DPLL_INTERNAL1_WW__CNTA_153P6MHZ                        (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

#define DPLL_INTERNAL1_WW__CNTA_163P2MHZ                        (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

#define DPLL_INTERNAL1_WW__CNTA_172P8MHZ                        (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

#define DPLL_INTERNAL1_WW__CNTA_182P4MHZ                        (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

#define DPLL_INTERNAL1_WW__CNTA_192MHZ                          (2U <<  \
        HWIO_SHFT(DPLL_INTERNAL1,CNTA))

/*---------------------------------------------------------------------------
   DPLL_INTERNAL2
---------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
//      Default value for this register.  This value sets all recommended
//      values and is common for all clock frequencies.

#define DPLL_INTERNAL2_REC_VALS                                 0x4DE4F380UL

/*-------------------------------------------------------------------------*/
//      Set bits to represent recommended configuration for all clock settings

#define DPLL_INTERNAL2_WW__CONFIG_RECOMMENDED   (DPLL_INTERNAL2_REC_VALS  <<  \
        HWIO_SHFT(DPLL_INTERNAL2,CONFIG))

/*---------------------------------------------------------------------------
   MSM_CLK_SLEEPOSC
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [6:0] RESERVED

/*-------------------------------------------------------------------------*/
//      Bit [7] SLEEP_XTAL_EN, this bit specifies if the sleep controller is
//      to be run off of the SLEEP_XTAL_IN pin or TCXO/4
//DA In Jaguar, bit 7 of MSM_CLK_SLEEPOSC (SLEEP_XTAL_EN) is reserved (RESERVED_BITS7_0)
#define MSM_CLK_SLEEPOSC_WH__SLEEP_XTAL_EN_DIS              (0U <<  \
        HWIO_SHFT(MSM_CLK_SLEEPOSC,SLEEP_XTAL_EN))

#define MSM_CLK_SLEEPOSC_WH__SLEEP_XTAL_EN_ENA              (1U <<  \
        HWIO_SHFT(MSM_CLK_SLEEPOSC,SLEEP_XTAL_EN))

/*-------------------------------------------------------------------------*/
//      Bit [8] SLEEP_OSC_RD_BYPASS, this bit disables the output
//      resistance of the SLEEP_XTAL_OUT path
#define MSM_CLK_SLEEPOSC_WH__SLEEP_OSC_RD_BYPASS_ENA        (1U <<  \
        HWIO_SHFT(MSM_CLK_SLEEPOSC,SLEEP_OSC_RD_BYPASS))

#define MSM_CLK_SLEEPOSC_WH__SLEEP_OSC_RD_BYPASS_DIS        (0U <<  \
        HWIO_SHFT(MSM_CLK_SLEEPOSC,SLEEP_OSC_RD_BYPASS))

/*-------------------------------------------------------------------------*/
//      Bit [9] SLEEP_OSC_RF_BYPASS, this bit disables the internal
//      resistance on the schmitt trigger path
#define MSM_CLK_SLEEPOSC_WH__SLEEP_OSC_RF_BYPASS_ENA        (1U <<  \
        HWIO_SHFT(MSM_CLK_SLEEPOSC,SLEEP_OSC_RF_BYPASS))

#define MSM_CLK_SLEEPOSC_WH__SLEEP_OSC_RF_BYPASS_DIS        (0U <<  \
        HWIO_SHFT(MSM_CLK_SLEEPOSC,SLEEP_OSC_RF_BYPASS))

/*-------------------------------------------------------------------------*/
//      Bit [11:10] SLEEP_OSC_GAIN, configure gain of internal
//      oscillator circuit
#define MSM_CLK_SLEEPOSC_WH__SLEEP_OSC_GAIN_000             (0U <<  \
        HWIO_SHFT(MSM_CLK_SLEEPOSC,SLEEP_OSC_GAIN))

#define MSM_CLK_SLEEPOSC_WH__SLEEP_OSC_GAIN_001             (1U <<  \
        HWIO_SHFT(MSM_CLK_SLEEPOSC,SLEEP_OSC_GAIN))

#define MSM_CLK_SLEEPOSC_WH__SLEEP_OSC_GAIN_010             (2U <<  \
        HWIO_SHFT(MSM_CLK_SLEEPOSC,SLEEP_OSC_GAIN))

#define MSM_CLK_SLEEPOSC_WH__SLEEP_OSC_GAIN_011             (3U <<  \
        HWIO_SHFT(MSM_CLK_SLEEPOSC,SLEEP_OSC_GAIN))

/*-------------------------------------------------------------------------*/
//      Bit [12] RESERVED

/*-------------------------------------------------------------------------*/
//      Bit [14:13] TCXO_SEL, select the expected input TCXO frequency.
//      This is used by the DFM block to determine the correct divide ratio
//      for generating a 720kHz clock
//DA From Jaguar SW manual "Only TCXO=19.2MHz is supported in Jaguar, therefore
//   tcxo_sel=”10” is hardcorded and no register bit to program it."


/*---------------------------------------------------------------------------
   MISC_CLK_CTL
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] TXDAC_SLEEP, sets txdac_sleep bit output to TLMM
#define MISC_CLK_CTL_WW__TXDAC_SLEEP_DIS                    (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,TXDAC_SLEEP))

#define MISC_CLK_CTL_WW__TXDAC_SLEEP_ENA                    (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,TXDAC_SLEEP))

/*-------------------------------------------------------------------------*/
//      Bit [1] AC_DC_N, mode bit for TCXO pad
#define MISC_CLK_CTL_WW__AC_DC_N_MODE_DC                    (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,AC_DC_N))

#define MISC_CLK_CTL_WW__AC_DC_N_MODE_AC                    (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,AC_DC_N))

/*-------------------------------------------------------------------------*/
//      Bit [2] PDM_SLEEP_EN, enable synchronous sleeping of the pdm_clk.
#define MISC_CLK_CTL_WW__PDM_SLEEP_EN_DIS                   (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,PDM_SLEEP_EN))

#define MISC_CLK_CTL_WW__PDM_SLEEP_EN_ENA                   (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,PDM_SLEEP_EN))

/*-------------------------------------------------------------------------*/
//      Bit [3] AGC_SLEEP_EN, enable synchronous sleeping of the agc_clk.
#define MISC_CLK_CTL_WW__AGC_SLEEP_EN_DIS                   (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,AGC_SLEEP_EN))

#define MISC_CLK_CTL_WW__AGC_SLEEP_EN_ENA                   (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,AGC_SLEEP_EN))

/*-------------------------------------------------------------------------*/
//      Bit [4] SRCH4_SLEEP_EN, enable aDSP to put Searcher4 into sleep mode
#define MISC_CLK_CTL_WW__SRCH4_SLEEP_EN_DIS                 (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,SRCH4_SLEEP_EN))

#define MISC_CLK_CTL_WW__SRCH4_SLEEP_EN_ENA                 (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,SRCH4_SLEEP_EN))

/*-------------------------------------------------------------------------*/
//      Bit [5] AHB_WR_CLK_EN, when set (1), ahb_wr_clk will be a free-running
//      hclk.  when cleared, the clock only toggles when a write is performed
//      to the ahb_regs address space.
#define MISC_CLK_CTL_WW__AHB_WR_CLK_EN_DIS                   (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,AHB_WR_CLK_EN))

#define MISC_CLK_CTL_WW__AHB_WR_CLK_EN_ENA                   (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,AHB_WR_CLK_EN))

#ifdef T_MSM6100B

/*-------------------------------------------------------------------------*/
    //      Bit [6] MOT_EST_SLEEP_EN, when set (1), enables asynchronous sleeping
    //      of the motion estimator clock from an external sleep bit.
    #define MISC_CLK_CTL_WW__MOT_EST_SLEEP_EN_DIS            (0U <<  \
            HWIO_SHFT(MISC_CLK_CTL,MOT_EST_SLEEP_EN))

    #define MISC_CLK_CTL_WW__MOT_EST_SLEEP_EN_ENA            (1U <<  \
            HWIO_SHFT(MISC_CLK_CTL,MOT_EST_SLEEP_EN))

#endif // ifdef T_MSM6100B

/*-------------------------------------------------------------------------*/
//      Bit [7] RINGOSC_CNT_EN, set (1) to enable the ring oscillator counter
//      used to estimate the ring oscillator frequency.
#define MISC_CLK_CTL_WW__RINGOSC_CNT_EN_DIS                 (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RINGOSC_CNT_EN))

#define MISC_CLK_CTL_WW__RINGOSC_CNT_EN_ENA                 (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RINGOSC_CNT_EN))

/*-------------------------------------------------------------------------*/
//      Bit [8] TCXO_CNT_EN, set (1) to enable the TCXO counter
//      used to estimate the ring oscillator frequency.
#define MISC_CLK_CTL_WW__TCXO_CNT_EN_DIS                    (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,TCXO_CNT_EN))

#define MISC_CLK_CTL_WW__TCXO_CNT_EN_ENA                    (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,TCXO_CNT_EN))

/*-------------------------------------------------------------------------*/
//      Bit [9] PLLOUT_DIV5_EN, enable the divide by 5n logic following the
//      main PLL
#define MISC_CLK_CTL_WW__PLLOUT_DIV5_EN_DIS                 (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,PLLOUT_DIV5_EN))

#define MISC_CLK_CTL_WW__PLLOUT_DIV5_EN_ENA                 (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,PLLOUT_DIV5_EN))

/*-------------------------------------------------------------------------*/
//      Bit [10] PLLOUT_DIV3_EN, enable the divide by 3n logic following the
//      main PLL
#define MISC_CLK_CTL_WW__PLLOUT_DIV3_EN_DIS                 (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,PLLOUT_DIV3_EN))

#define MISC_CLK_CTL_WW__PLLOUT_DIV3_EN_ENA                 (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,PLLOUT_DIV3_EN))

/*-------------------------------------------------------------------------*/
//      Bit [11] PLLOUT_DIV2_5_EN, enable the divide by 2.5n logic following
//      the main PLL
#define MISC_CLK_CTL_WW__PLLOUT_DIV2_5_EN_DIS               (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,PLLOUT_DIV2_5_EN))

#define MISC_CLK_CTL_WW__PLLOUT_DIV2_5_EN_ENA               (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,PLLOUT_DIV2_5_EN))

/*-------------------------------------------------------------------------*/
//      Bit [12] PLLOUT_DIV2_EN, enable the divide by 2n logic following
//      the main PLL
#define MISC_CLK_CTL_WW__PLLOUT_DIV2_EN_DIS                 (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,PLLOUT_DIV2_EN))

#define MISC_CLK_CTL_WW__PLLOUT_DIV2_EN_ENA                 (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,PLLOUT_DIV2_EN))

/*-------------------------------------------------------------------------*/
//      Bit [13] RX_MOD_CLK_DIFF_SEL, enable the rx_mod_clk differential
//      output
#define MISC_CLK_CTL_WW__RX_MOD_CLK_DIFF_SEL_SINGLE_ENDED   (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_MOD_CLK_DIFF_SEL))

#define MISC_CLK_CTL_WW__RX_MOD_CLK_DIFF_SEL_DIFF_OUTPUT    (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_MOD_CLK_DIFF_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [14] RX_TCXO_DIFF_SEL, enable the rx_tcxo differential
//      output
#define MISC_CLK_CTL_WW__RX_TCXO_DIFF_SEL_SINGLE_ENDED      (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_TCXO_DIFF_SEL))

#define MISC_CLK_CTL_WW__RX_TCXO_DIFF_SEL_DIFF_OUTPUT       (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_TCXO_DIFF_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [15] RX_MOD_CLK_EN, enable the rx_mod_clk clock.
#define MISC_CLK_CTL_WW__RX_MOD_CLK_EN_DIS                  (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_MOD_CLK_EN))

#define MISC_CLK_CTL_WW__RX_MOD_CLK_EN_ENA                  (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_MOD_CLK_EN))

/*-------------------------------------------------------------------------*/
//      Bit [16] RX_TCXO_EN, enable the rx_tcxo clock.
#define MISC_CLK_CTL_WW__RX_TCXO_EN_DIS                     (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_TCXO_EN))

#define MISC_CLK_CTL_WW__RX_TCXO_EN_ENA                     (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_TCXO_EN))

/*-------------------------------------------------------------------------*/
//      Bit [17] SVD_SLEEP_EN, enable synchronous sleeping of the svd_clk
//      based on turbo_dec_sleep from the decoder.
#define MISC_CLK_CTL_WW__SVD_SLEEP_EN_DIS                   (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,SVD_SLEEP_EN))

#define MISC_CLK_CTL_WW__SVD_SLEEP_EN_ENA                   (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,SVD_SLEEP_EN))

/*-------------------------------------------------------------------------*/
//      Bit [18] TURBO_DEC_SLEEP_EN, enable synchronous sleeping of the
//      turbo_dec_clk based on turbo_dec_sleep from the decoder.
#define MISC_CLK_CTL_WW__TD_SLEEP_EN_DIS                    (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,TD_SLEEP_EN))

#define MISC_CLK_CTL_WW__TD_SLEEP_EN_ENA                    (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,TD_SLEEP_EN))

/*-------------------------------------------------------------------------*/
//      Bit [19] DEM_CX8_SLP_EN_N, set (1) to disable synchronous sleeping
//      of the demchipx8_clk based on rxchipx8_clk_enable from the sleep
//      controller.  Clear (0) to enable sleep.
#define MISC_CLK_CTL_WW__DEM_CX8_SLP_EN_N_ENA_SLEEP         (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,DEM_CX8_SLP_EN_N))

#define MISC_CLK_CTL_WW__DEM_CX8_SLP_EN_N_DIS_SLEEP         (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,DEM_CX8_SLP_EN_N))

/*-------------------------------------------------------------------------*/
//      Bit [20] MAC_CLK_SLEEP
#define MISC_CLK_CTL_WW__MAC_CLK_SLEEP_DIS         (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,MAC_CLK_SLEEP))

#define MISC_CLK_CTL_WW__MAC_CLK_SLEEP_ENA         (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,MAC_CLK_SLEEP))

#ifdef T_MSM6100B

    /*-------------------------------------------------------------------------*/
    //      Bit [21] HCLK_SLEEP_EN, when set (1), enables the following bus clock
    //      regimes to sleep when MICRO_CLK_OFF is written:  hclk, ahb_wr_clk,
    //      grp_clk, xmem_hclk, mmcm_clk, etm_clk and mpmc_hclk.  When cleared (0)
    //      the above remain on during micro_clk sleep.  If this bit is cleared and
    //      SDRAM is being used, enusre that the sdram_preset bit is cleared (0) to
    //      prevent self-refresh from being entered when SDRAM is being used by
    //      DMA/DME.

    #define MISC_CLK_CTL_WW__HCLK_SLEEP_EN_DIS            (0U <<  \
            HWIO_SHFT(MISC_CLK_CTL,HCLK_SLEEP_EN))

    #define MISC_CLK_CTL_WW__HCLK_SLEEP_EN_ENA            (1U <<  \
            HWIO_SHFT(MISC_CLK_CTL,HCLK_SLEEP_EN))

#endif // ifdef T_MSM6100B

/*-------------------------------------------------------------------------*/
//      Bit [22] XMEM_HCLK_SLEEP
#define MISC_CLK_CTL_WW__XMEM_HCLK_SLEEP_DIS         (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,XMEM_HCLK_SLEEP))

#define MISC_CLK_CTL_WW__XMEM_HCLK_SLEEP_ENA         (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,XMEM_HCLK_SLEEP))


/*-------------------------------------------------------------------------*/
//      Bit [24:23] RX_MOD_CLK_DRIVE
#define MISC_CLK_CTL_WW__RX_MOD_CLK_DRIVE_DRIVE0         (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_MOD_CLK_DRIVE))

#define MISC_CLK_CTL_WW__RX_MOD_CLK_DRIVE_DRIVE1         (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_MOD_CLK_DRIVE))

#define MISC_CLK_CTL_WW__RX_MOD_CLK_DRIVE_DRIVE2         (2U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_MOD_CLK_DRIVE))

#define MISC_CLK_CTL_WW__RX_MOD_CLK_DRIVE_DRIVE3         (3U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_MOD_CLK_DRIVE))


/*-------------------------------------------------------------------------*/
//      Bit [26:25] RX_TCXO_CLK_DRIVE
#define MISC_CLK_CTL_WW__RX_TCXO_CLK_DRIVE_DRIVE0         (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_TCXO_CLK_DRIVE))

#define MISC_CLK_CTL_WW__RX_TCXO_CLK_DRIVE_DRIVE1         (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_TCXO_CLK_DRIVE))

#define MISC_CLK_CTL_WW__RX_TCXO_CLK_DRIVE_DRIVE2         (2U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_TCXO_CLK_DRIVE))

#define MISC_CLK_CTL_WW__RX_TCXO_CLK_DRIVE_DRIVE3         (3U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_TCXO_CLK_DRIVE))

/*-------------------------------------------------------------------------*/
//      Bit [27] RX_MOD_CLK_SE_EN, enable the rx_mod_clk differential
//      output
#define MISC_CLK_CTL_WW__RX_MOD_CLK_SE_EN_DIS   (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_MOD_CLK_SE_EN))

#define MISC_CLK_CTL_WW__RX_MOD_CLK_SE_EN_ENA    (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_MOD_CLK_SE_EN))

/*-------------------------------------------------------------------------*/
//      Bit [28] RX_TCXO_CLK_SE_EN, enable the rx_mod_clk differential
//      output
#define MISC_CLK_CTL_WW__RX_TCXO_CLK_SE_EN_DIS   (0U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_TCXO_CLK_SE_EN))

#define MISC_CLK_CTL_WW__RX_TCXO_CLK_SE_EN_ENA    (1U <<  \
        HWIO_SHFT(MISC_CLK_CTL,RX_TCXO_CLK_SE_EN))

#ifdef T_MSM6100B

    /*-------------------------------------------------------------------------*/
    //      Bit [29] RXF_GPS_MF_CLK_ON, when set (1), enables the RX_FRONT gps
    //      matched filter clock.  Clear to allow hardware to enable/disable
    //      the clock for power savings.
    #define MISC_CLK_CTL_WW__RXF_GPS_MF_CLK_ON_DIS            (0U <<  \
            HWIO_SHFT(MISC_CLK_CTL,RXF_GPS_MF_CLK_ON))

    #define MISC_CLK_CTL_WW__RXF_GPS_MF_CLK_ON_ENA            (1U <<  \
            HWIO_SHFT(MISC_CLK_CTL,RXF_GPS_MF_CLK_ON))

    /*-------------------------------------------------------------------------*/
    //      Bit [30] RX_SAMP_CLK_SLEEP, when set (1), disables the rx_samp_clk
    //      regime.  When cleared (0), allows other hardware and software bits control.
    #define MISC_CLK_CTL_WW__RX_SAMP_CLK_SLEEP_DIS            (0U <<  \
            HWIO_SHFT(MISC_CLK_CTL,RX_SAMP_CLK_SLEEP))

    #define MISC_CLK_CTL_WW__RX_SAMP_CLK_SLEEP_ENA            (1U <<  \
            HWIO_SHFT(MISC_CLK_CTL,RX_SAMP_CLK_SLEEP))


#endif // ifdef T_MSM6100B

/*---------------------------------------------------------------------------
   CPLL_CTL
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [3:0] CPLL_PREDIV, select the codec PLL pre divide by N counter
#define CPLL_CTL_WH__CPLL_PREDIV_DIV1                       (0U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV2                       (1U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV3                       (2U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV4                       (3U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV5                       (4U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV6                       (5U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV7                       (6U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV8                       (7U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV9                       (8U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV10                      (9U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV11                      (10U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV12                      (11U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV13                      (12U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV14                      (13U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV15                      (14U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

#define CPLL_CTL_WH__CPLL_PREDIV_DIV16                      (15U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV))

/*-------------------------------------------------------------------------*/
//      Bits [6:4] RESERVED

/*-------------------------------------------------------------------------*/
//      Bit [7] CPLL_CLKSEL, selects the CPLL clock source
#define CPLL_CTL_WH__CPLL_CLKSEL_TCXO                       (0U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_CLKSEL))

#define CPLL_CTL_WH__CPLL_CLKSEL_USB_XTAL                   (1U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_CLKSEL))

/*-------------------------------------------------------------------------*/
//      Bit [8] CPLL_PREDIV_EN, enables the TCXO predivide before coming
//      into the CPLL
#define CPLL_CTL_WH__CPLL_PREDIV_EN_DIS                     (0U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV_EN))

#define CPLL_CTL_WH__CPLL_PREDIV_EN_ENA                     (1U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_PREDIV_EN))

/*-------------------------------------------------------------------------*/
//      Bit [10:9] UNUSED

/*-------------------------------------------------------------------------*/
//      Bit [11] CPLL_MNCNTR_DUAL_MODE, enables dual mode operation of the
//      CPLL internal MN counter
#define CPLL_CTL_WH__CPLL_MNCNTR_DUAL_MODE_DIS              (0U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_MNCNTR_DUAL_MODE))

#define CPLL_CTL_WH__CPLL_MNCNTR_DUAL_MODE_ENA              (1U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_MNCNTR_DUAL_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [12] CPLL_MNCNTR_RES, reset for the CPLL internal MN counter.
//      Always use this before changing MND values
#define CPLL_CTL_WH__CPLL_MNCNTR_RES_NO_RESET                     (0U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_MNCNTR_RES))

#define CPLL_CTL_WH__CPLL_MNCNTR_RES_RESET                     (1U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_MNCNTR_RES))

/*-------------------------------------------------------------------------*/
//      Bit [13] CPLL_MNCNTR_EN, enables the CPLL internal MN counter
#define CPLL_CTL_WH__CPLL_MNCNTR_EN_DIS                     (0U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_MNCNTR_EN))

#define CPLL_CTL_WH__CPLL_MNCNTR_EN_ENA                     (1U <<  \
        HWIO_SHFT(CPLL_CTL,CPLL_MNCNTR_EN))


/*---------------------------------------------------------------------------
   SWITCH_CLK
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] SWITCH, selects which MICRO_CLK_SOURCE register to use as
//      the clock source select for the microprocessor.
#define SWITCH_CLK_WB_SEL_MICRO_CLK_SOURCE0                 (0U <<  \
        HWIO_SHFT(SWITCH_CLK,SWITCH))

#define SWITCH_CLK_WB_SEL_MICRO_CLK_SOURCE1                 (1U <<  \
        HWIO_SHFT(SWITCH_CLK,SWITCH))


/*---------------------------------------------------------------------------
   MICRO_CLK_SOURCE0
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] MICRO_CLK_SOURCE0, selects the clock source for mux 0
#define MICRO_CLK_SOURCE0_WB__UPCLK_SOURCE0_TCXO                (0U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE0,UPCLK_SOURCE0))

#define MICRO_CLK_SOURCE0_WB__UPCLK_SOURCE0_PLLOUT              (1U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE0,UPCLK_SOURCE0))

#define MICRO_CLK_SOURCE0_WB__UPCLK_SOURCE0_EXT_SOURCE          (2U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE0,UPCLK_SOURCE0))

#define MICRO_CLK_SOURCE0_WB__UPCLK_SOURCE0_SLEEP_SCHMITT_OUT   (3U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE0,UPCLK_SOURCE0))

#define MICRO_CLK_SOURCE0_WB__UPCLK_SOURCE0_RING_OSC_CLK        (4U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE0,UPCLK_SOURCE0))

#define MICRO_CLK_SOURCE0_WB__UPCLK_SOURCE0_USB_CLK             (5U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE0,UPCLK_SOURCE0))

#define MICRO_CLK_SOURCE0_WB__UPCLK_SOURCE0_DPLLOUT             (6U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE0,UPCLK_SOURCE0))

#define MICRO_CLK_SOURCE0_WB__UPCLK_SOURCE0_PLLOUT_DIV_2P5      (7U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE0,UPCLK_SOURCE0))

/*---------------------------------------------------------------------------
   MICRO_CLK_SOURCE1
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] MICRO_CLK_SOURCE1, selects the clock source for mux 1
#define MICRO_CLK_SOURCE1_WB__UPCLK_SOURCE1_TCXO                (0U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE1,UPCLK_SOURCE1))

#define MICRO_CLK_SOURCE1_WB__UPCLK_SOURCE1_PLLOUT              (1U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE1,UPCLK_SOURCE1))

#define MICRO_CLK_SOURCE1_WB__UPCLK_SOURCE1_EXT_SOURCE          (2U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE1,UPCLK_SOURCE1))

#define MICRO_CLK_SOURCE1_WB__UPCLK_SOURCE1_SLEEP_SCHMITT_OUT   (3U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE1,UPCLK_SOURCE1))

#define MICRO_CLK_SOURCE1_WB__UPCLK_SOURCE1_RING_OSC_CLK        (4U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE1,UPCLK_SOURCE1))

#define MICRO_CLK_SOURCE1_WB__UPCLK_SOURCE1_USB_CLK             (5U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE1,UPCLK_SOURCE1))

#define MICRO_CLK_SOURCE1_WB__UPCLK_SOURCE1_DPLLOUT             (6U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE1,UPCLK_SOURCE1))

#define MICRO_CLK_SOURCE1_WB__UPCLK_SOURCE1_PLLOUT_DIV_2P5      (7U <<  \
        HWIO_SHFT(MICRO_CLK_SOURCE1,UPCLK_SOURCE1))


/*---------------------------------------------------------------------------
   MICRO_CLK_DIV
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [2:0] MICRO_CLK_DIVX, sets the uP clock pre-divider
#define MICRO_CLK_DIV_WB__MICRO_CLK_DIVX_1                  (0U <<  \
        HWIO_SHFT(MICRO_CLK_DIV,UPCLK_DIVX))

#define MICRO_CLK_DIV_WB__MICRO_CLK_DIVX_2                  (1U <<  \
        HWIO_SHFT(MICRO_CLK_DIV,UPCLK_DIVX))

#define MICRO_CLK_DIV_WB__MICRO_CLK_DIVX_3                  (2U <<  \
        HWIO_SHFT(MICRO_CLK_DIV,UPCLK_DIVX))

#define MICRO_CLK_DIV_WB__MICRO_CLK_DIVX_4                  (3U <<  \
        HWIO_SHFT(MICRO_CLK_DIV,UPCLK_DIVX))

#define MICRO_CLK_DIV_WB__MICRO_CLK_DIVX_8                  (4U <<  \
        HWIO_SHFT(MICRO_CLK_DIV,UPCLK_DIVX))

#define MICRO_CLK_DIV_WB__MICRO_CLK_DIVX_16                 (5U <<  \
        HWIO_SHFT(MICRO_CLK_DIV,UPCLK_DIVX))

#define MICRO_CLK_DIV_WB__MICRO_CLK_DIVX_32                 (6U <<  \
        HWIO_SHFT(MICRO_CLK_DIV,UPCLK_DIVX))

#define MICRO_CLK_DIV_WB__MICRO_CLK_DIVX_64                 (7U <<  \
        HWIO_SHFT(MICRO_CLK_DIV,UPCLK_DIVX))



/*---------------------------------------------------------------------------
   HCLK_DIV
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [2:0] MICRO_CLK_DIVX, sets the uP clock pre-divider
#define HCLK_DIV_WB__HCLK_DIVX_1                  (0U <<  \
        HWIO_SHFT(HCLK_DIV,HCLK_DIVX))

#define HCLK_DIV_WB__HCLK_DIVX_2                  (1U <<  \
        HWIO_SHFT(HCLK_DIV,HCLK_DIVX))

#define HCLK_DIV_WB__HCLK_DIVX_3                  (2U <<  \
        HWIO_SHFT(HCLK_DIV,HCLK_DIVX))

#define HCLK_DIV_WB__HCLK_DIVX_4                  (3U <<  \
        HWIO_SHFT(HCLK_DIV,HCLK_DIVX))

/*---------------------------------------------------------------------------
   MSM_CLK_USBOSC
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [14] USB_OSC_EN_N, disables the 48MHz xtal when high.
#define MSM_CLK_USBOSC_WH__USB_OSC_EN_N_ENA                 (0U <<  \
        HWIO_SHFT(MSM_CLK_USBOSC,USB_OSC_EN_N))

#define MSM_CLK_USBOSC_WH__USB_OSC_EN_N_DIS                 (1U <<  \
        HWIO_SHFT(MSM_CLK_USBOSC,USB_OSC_EN_N))

//      Bit [13] USB_OSC_GAIN, controls the various gain inverters to drive
//      the oscillator feedback.
#define MSM_CLK_USBOSC_WH__USB_OSC_GAIN_MIN                 (0U <<  \
        HWIO_SHFT(MSM_CLK_USBOSC,USB_OSC_GAIN))

#define MSM_CLK_USBOSC_WH__USB_OSC_GAIN_MAX                 (1U <<  \
        HWIO_SHFT(MSM_CLK_USBOSC,USB_OSC_GAIN))

//      Bit [12] USB_OSC_RF_BYPASS, Disables the internal resistance on the
//      schmitt trigger path.
#define MSM_CLK_USBOSC_WH__USB_OSC_RF_BYPASS_ENA                 (1U <<  \
        HWIO_SHFT(MSM_CLK_USBOSC,USB_OSC_RF_BYPASS))

#define MSM_CLK_USBOSC_WH__USB_OSC_RF_BYPASS_DIS                 (0U <<  \
        HWIO_SHFT(MSM_CLK_USBOSC,USB_OSC_RF_BYPASS))

//      Bit [11] USB_OSC_RD_BYPASS, Disables the output resistance
//      on the USB_XTAL_OUT path.
#define MSM_CLK_USBOSC_WH__USB_OSC_RD_BYPASS_ENA                 (1U <<  \
        HWIO_SHFT(MSM_CLK_USBOSC,USB_OSC_RD_BYPASS))

#define MSM_CLK_USBOSC_WH__USB_OSC_RD_BYPASS_DIS                 (0U <<  \
        HWIO_SHFT(MSM_CLK_USBOSC,USB_OSC_RD_BYPASS))

//      Bit [10] UNUSED

//      Bit [9] ADSP_RPCM_OFF_EN_N, Grants permission to ADSP
//      for stopping the RPCM clock.  Ignore the _N for establishing
//      the grant value -- GRANT is 1.  Bit name to be fixed, then update here.
#define MSM_CLK_USBOSC_WH__ADSP_RPCM_OFF_EN_N_GRANT              (1U <<  \
        HWIO_SHFT(MSM_CLK_USBOSC,ADSP_RPCM_OFF_EN_N))

#define MSM_CLK_USBOSC_WH__ADSP_RPCM_OFF_EN_N_NOGRANT            (0U <<  \
        HWIO_SHFT(MSM_CLK_USBOSC,ADSP_RPCM_OFF_EN_N))

#ifdef T_MSM6100B

/*-------------------------------------------------------------------------*/
    //      Bit [8:6] USB_CLK_SRC_SEL, selects the USB 48MHz raw clock source
    #define MSM_CLK_USBOSC_WH__USB_CLK_SRC_SEL_EXT_XTAL              (0U <<  \
            HWIO_SHFT(MSM_CLK_USBOSC,USB_CLK_SRC_SEL))

    #define MSM_CLK_USBOSC_WH__USB_CLK_SRC_SEL_PLLOUT                (1U <<  \
            HWIO_SHFT(MSM_CLK_USBOSC,USB_CLK_SRC_SEL))

    #define MSM_CLK_USBOSC_WH__USB_CLK_SRC_SEL_DPLL_DIV1             (2U <<  \
            HWIO_SHFT(MSM_CLK_USBOSC,USB_CLK_SRC_SEL))

    #define MSM_CLK_USBOSC_WH__USB_CLK_SRC_SEL_CPLLOUT               (3U <<  \
            HWIO_SHFT(MSM_CLK_USBOSC,USB_CLK_SRC_SEL))

    #define MSM_CLK_USBOSC_WH__USB_CLK_SRC_SEL_DPLL_DIV2             (4U <<  \
            HWIO_SHFT(MSM_CLK_USBOSC,USB_CLK_SRC_SEL))

    #define MSM_CLK_USBOSC_WH__USB_CLK_SRC_SEL_DPLL_DIV4             (5U <<  \
            HWIO_SHFT(MSM_CLK_USBOSC,USB_CLK_SRC_SEL))

    #define MSM_CLK_USBOSC_WH__USB_CLK_SRC_SEL_DPLL_DIV3             (6U <<  \
            HWIO_SHFT(MSM_CLK_USBOSC,USB_CLK_SRC_SEL))

#else

       /*-------------------------------------------------------------------------*/
       //      Bit [8:6] USB_CLK_SRC_SEL, selects the USB 48MHz raw clock source
   #define MSM_CLK_USBOSC_WH__USB_CLK_SRC_SEL_EXT_XTAL              (0U <<  \
           HWIO_SHFT(MSM_CLK_USBOSC,USB_CLK_SRC_SEL))

   #define MSM_CLK_USBOSC_WH__USB_CLK_SRC_SEL_DPLL_DIV1             (1U <<  \
           HWIO_SHFT(MSM_CLK_USBOSC,USB_CLK_SRC_SEL))

   #define MSM_CLK_USBOSC_WH__USB_CLK_SRC_SEL_DPLL_DIV2             (2U <<  \
           HWIO_SHFT(MSM_CLK_USBOSC,USB_CLK_SRC_SEL))

#endif // ifdef T_MSM6100B




/*---------------------------------------------------------------------------
   DPLL_CTL
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [3:0] DPLL_DIV_SEL, sets the Mod-N divide for the DPLL
#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV1                       (0U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV2                       (1U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV3                       (2U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV4                       (3U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV5                       (4U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV6                       (5U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV7                       (6U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV8                       (7U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV9                       (8U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV10                      (9U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV11                      (10U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV12                      (11U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV13                      (12U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV14                      (13U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV15                      (14U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

#define DPLL_CTL_WH__DPLL_DIV_SEL_DIV16                      (15U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [6:4] RESERVED

/*-------------------------------------------------------------------------*/
//      Bit [7] DPLL_SRC_SEL, selects the DPLL clock source
#define DPLL_CTL_WH__DPLL_SRC_SEL_TCXO                       (0U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_SRC_SEL))

#define DPLL_CTL_WH__DPLL_SRC_SEL_SLEEP_XTAL                 (1U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [8] DPLL_DIV_EN, enables the TCXO predivide before coming
//      into the DPLL
#define DPLL_CTL_WH__DPLL_DIV_EN_DIS                     (0U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_EN))

#define DPLL_CTL_WH__DPLL_DIV_EN_ENA                     (1U <<  \
        HWIO_SHFT(DPLL_CTL,DPLL_DIV_EN))

/*---------------------------------------------------------------------------
   CLK_TEST
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [6:5] UNUSED

/*-------------------------------------------------------------------------*/
//      Bit [4:0] CLK_TEST_SEL, selects the output of the DBG_CLK_OUT pin
#define CLK_TEST_WB__CLK_TEST_SEL_MICRO                     ( 5U <<  \
        HWIO_SHFT(CLK_TEST,CLK_TEST_SEL))

#define CLK_TEST_WB__CLK_TEST_SEL_RX_SAMPLE                 (10U <<  \
        HWIO_SHFT(CLK_TEST,CLK_TEST_SEL))

#define CLK_TEST_WB__CLK_TEST_SEL_SBI                       (13U <<  \
        HWIO_SHFT(CLK_TEST,CLK_TEST_SEL))

#define CLK_TEST_WB__CLK_TEST_SEL_RXCX8                     (14U <<  \
        HWIO_SHFT(CLK_TEST,CLK_TEST_SEL))

#define CLK_TEST_WB__CLK_TEST_SEL_GPS_DFM_360_KHZ           (24U <<  \
        HWIO_SHFT(CLK_TEST,CLK_TEST_SEL))

#define CLK_TEST_WB__CLK_TEST_SEL_GPS_CX8                   (25U <<  \
        HWIO_SHFT(CLK_TEST,CLK_TEST_SEL))

#define CLK_TEST_WB__CLK_TEST_SEL_CPLLOUT                   (27U <<  \
        HWIO_SHFT(CLK_TEST,CLK_TEST_SEL))

#define CLK_TEST_WB__CLK_TEST_SEL_PLLOUT                    (31U <<  \
        HWIO_SHFT(CLK_TEST,CLK_TEST_SEL))



/*---------------------------------------------------------------------------
   MSM_CLK_MNCNTRS
---------------------------------------------------------------------------*/

#ifndef T_MSM6100B

/*-------------------------------------------------------------------------*/
//      Bit [0] MDDI_CNTR_RES
#define MSM_CLK_MNCNTRS_WW__MDDI_CNTR_RES_NO_RESET       (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,MDDI_CNTR_RES))

#define MSM_CLK_MNCNTRS_WW__MDDI_CNTR_RES_RESET          (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,MDDI_CNTR_RES))

#endif // T_MSM6100B

//      Bit [1] I2C_CNTR_RES
#define MSM_CLK_MNCNTRS_WW__I2C_CNTR_RES_NO_RESET        (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,I2C_CNTR_RES))

#define MSM_CLK_MNCNTRS_WW__I2C_CNTR_RES_RESET           (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,I2C_CNTR_RES))

//      Bit [2] ICODEC_MNCNTR_DUAL_MODE, set (1) for dual mode operation of
//      the icodec MN counter
#define MSM_CLK_MNCNTRS_WW__ICODEC_MNCNTR_DUAL_MODE_DIS       (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ICODEC_MNCNTR_DUAL_MODE))

#define MSM_CLK_MNCNTRS_WW__ICODEC_MNCNTR_DUAL_MODE_ENA       (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ICODEC_MNCNTR_DUAL_MODE))

//      Bit [3] ICODEC_MNCNTR_RES
#define MSM_CLK_MNCNTRS_WW__ICODEC_MNCNTR_RES_NO_RESET        (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ICODEC_MNCNTR_RES))

#define MSM_CLK_MNCNTRS_WW__ICODEC_MNCNTR_RES_RESET           (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ICODEC_MNCNTR_RES))

//      Bit [4] ICODEC_MNCNTR_EN
#define MSM_CLK_MNCNTRS_WW__ICODEC_MNCNTR_EN_DIS        (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ICODEC_MNCNTR_EN))

#define MSM_CLK_MNCNTRS_WW__ICODEC_MNCNTR_EN_ENA        (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ICODEC_MNCNTR_EN))


//      Bit [5] GENERAL_MNCNTR_DUAL_MODE, set (1) for dual mode operation of
//      the GENERAL MN counter


#define MSM_CLK_MNCNTRS_WW__GENERAL_MNCNTR_DUAL_MODE_DIS       (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GENERAL_MNCNTR_DUAL_MODE))

#define MSM_CLK_MNCNTRS_WW__GENERAL_MNCNTR_DUAL_MODE_ENA       (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GENERAL_MNCNTR_DUAL_MODE))

//      Bit [6] GENERAL_MNCNTR_RES
#define MSM_CLK_MNCNTRS_WW__GENERAL_MNCNTR_RES_NO_RESET        (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GENERAL_MNCNTR_RES))

#define MSM_CLK_MNCNTRS_WW__GENERAL_MNCNTR_RES_RESET           (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GENERAL_MNCNTR_RES))

//      Bit [7] GENERAL_MNCNTR_EN
#define MSM_CLK_MNCNTRS_WW__GENERAL_MNCNTR_EN_DIS             (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GENERAL_MNCNTR_ENABLE))

#define MSM_CLK_MNCNTRS_WW__GENERAL_MNCNTR_EN_ENA             (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GENERAL_MNCNTR_ENABLE))


//      Bit [8] CHIP_MNCNTR_DUAL_MODE, set (1) for dual mode operation of
//      the CDMA chipxN MN counter
#define MSM_CLK_MNCNTRS_WW__CHIP_MNCNTR_DUAL_MODE_DIS       (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,CHIP_MNCNTR_DUAL_MODE))

#define MSM_CLK_MNCNTRS_WW__CHIP_MNCNTR_DUAL_MODE_ENA       (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,CHIP_MNCNTR_DUAL_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [9] CHIP_MNCNTR_RES, set (1) to enable the CDMA chipxn MN counter
#define MSM_CLK_MNCNTRS_WW__CHIP_MNCNTR_RES_NO_RESET              (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,CHIP_MNCNTR_RES))

#define MSM_CLK_MNCNTRS_WW__CHIP_MNCNTR_RES_RESET              (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,CHIP_MNCNTR_RES))

/*-------------------------------------------------------------------------*/
//      Bit [10] CHIP_MNCNTR_EN, set (1) to enable the CDMA chipxn MN counter
#define MSM_CLK_MNCNTRS_WW__CHIP_MNCNTR_EN_DIS              (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,CHIP_MNCNTR_EN))

#define MSM_CLK_MNCNTRS_WW__CHIP_MNCNTR_EN_ENA              (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,CHIP_MNCNTR_EN))

/*-------------------------------------------------------------------------*/
//      Bit [11] SDAC_MNCNTR_DUAL_MODE, set (1) to enable dual mode
//      operation of the SDAC MN counter
#define MSM_CLK_MNCNTRS_WW__SDAC_MNCNTR_DUAL_MODE_DIS       (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SDAC_MNCNTR_DUAL_MODE))

#define MSM_CLK_MNCNTRS_WW__SDAC_MNCNTR_DUAL_MODE_ENA       (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SDAC_MNCNTR_DUAL_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [12] SDAC_MNCNTR_RES, set (1) to reset the SDAC MN counter
#define MSM_CLK_MNCNTRS_WW__SDAC_MNCNTR_RES_NO_RESET        (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SDAC_MNCNTR_RES))

#define MSM_CLK_MNCNTRS_WW__SDAC_MNCNTR_RES_RESET           (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SDAC_MNCNTR_RES))

/*-------------------------------------------------------------------------*/
//      Bit [13] SDAC_MNCNTR_EN, set (1) to enable the SDAC MN counter
#define MSM_CLK_MNCNTRS_WW__SDAC_MNCNTR_EN_DIS              (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SDAC_MNCNTR_EN))

#define MSM_CLK_MNCNTRS_WW__SDAC_MNCNTR_EN_ENA              (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SDAC_MNCNTR_EN))

/*-------------------------------------------------------------------------*/
//      Bit [14] GPS_MNCNTR_DUAL_MODE, set (1) to enable dual mode
//      operation of the GPS MN counter
#define MSM_CLK_MNCNTRS_WW__GPS_MNCNTR_DUAL_MODE_DIS        (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GPS_MNCNTR_DUAL_MODE))

#define MSM_CLK_MNCNTRS_WW__GPS_MNCNTR_DUAL_MODE_ENA        (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GPS_MNCNTR_DUAL_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [15] GPS_MNCNTR_RES
#define MSM_CLK_MNCNTRS_WW__GPS_MNCNTR_RES_NO_RESET        (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GPS_MNCNTR_RES))

#define MSM_CLK_MNCNTRS_WW__GPS_MNCNTR_RES_RESET           (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GPS_MNCNTR_RES))

/*-------------------------------------------------------------------------*/
//      Bit [16] GPS_MNCNTR_EN, set (1) to enable the GPS MN counter
#define MSM_CLK_MNCNTRS_WW__GPS_MNCNTR_EN_DIS              (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GPS_MNCNTR_EN))

#define MSM_CLK_MNCNTRS_WW__GPS_MNCNTR_EN_ENA              (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,GPS_MNCNTR_EN))

/*-------------------------------------------------------------------------*/
//      Bit [17] PLL_MNCNTR_DUAL_MODE, set (1) to enable dual mode
//      operation of the main PLL internal MN counter
#define MSM_CLK_MNCNTRS_WW__PLL_MNCNTR_DUAL_MODE_DIS       (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,PLL_MNCNTR_DUAL_MODE))

#define MSM_CLK_MNCNTRS_WW__PLL_MNCNTR_DUAL_MODE_ENA       (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,PLL_MNCNTR_DUAL_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [18] PLL_MNCNTR_RES
#define MSM_CLK_MNCNTRS_WW__PLL_MNCNTR_RES_NO_RESET        (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,PLL_MNCNTR_RES))

#define MSM_CLK_MNCNTRS_WW__PLL_MNCNTR_RES_RESET           (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,PLL_MNCNTR_RES))

/*-------------------------------------------------------------------------*/
//      Bit [19] PLL_MNCNTR_EN, set (1) to enable the main PLL internal
//      MN counter
#define MSM_CLK_MNCNTRS_WW__PLL_MNCNTR_EN_DIS              (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,PLL_MNCNTR_EN))

#define MSM_CLK_MNCNTRS_WW__PLL_MNCNTR_EN_ENA              (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,PLL_MNCNTR_EN))

/*-------------------------------------------------------------------------*/
//      Bit [20] BT_MNCNTR_DUAL_MODE, set (1) to enable dual mode
//      operation of the main BT internal MN counter
#define MSM_CLK_MNCNTRS_WW__BT_MNCNTR_DUAL_MODE_DIS        (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,BT_MNCNTR_DUAL_MODE))

#define MSM_CLK_MNCNTRS_WW__BT_MNCNTR_DUAL_MODE_ENA        (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,BT_MNCNTR_DUAL_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [21] BT_MNCNTR_RES
#define MSM_CLK_MNCNTRS_WW__BT_MNCNTR_RES_NO_RESET         (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,BT_MNCNTR_RES))

#define MSM_CLK_MNCNTRS_WW__BT_MNCNTR_RES_RESET            (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,BT_MNCNTR_RES))

/*-------------------------------------------------------------------------*/
//      Bit [22] BT_MNCNTR_EN, set (1) to enable the main BT internal
//      MN counter
#define MSM_CLK_MNCNTRS_WW__BT_MNCNTR_EN_DIS               (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,BT_MNCNTR_EN))

#define MSM_CLK_MNCNTRS_WW__BT_MNCNTR_EN_ENA               (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,BT_MNCNTR_EN))

/*-------------------------------------------------------------------------*/
//      Bit [23] SBI_MNCNTR_DUAL_MODE, set (1) to enable dual mode
//      operation of the SBI MN counter
#define MSM_CLK_MNCNTRS_WW__SBI_MNCNTR_DUAL_MODE_DIS       (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SBI_MNCNTR_DUAL_MODE))

#define MSM_CLK_MNCNTRS_WW__SBI_MNCNTR_DUAL_MODE_ENA       (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SBI_MNCNTR_DUAL_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [24] SBI_MNCNTR_RES
#define MSM_CLK_MNCNTRS_WW__SBI_MNCNTR_RES_NO_RESET        (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SBI_MNCNTR_RES))

#define MSM_CLK_MNCNTRS_WW__SBI_MNCNTR_RES_RESET           (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SBI_MNCNTR_RES))

/*-------------------------------------------------------------------------*/
//      Bit [25] SBI_MNCNTR_EN, set (1) to enable SBI MN counter
#define MSM_CLK_MNCNTRS_WW__SBI_MNCNTR_EN_DIS              (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SBI_MNCNTR_EN))

#define MSM_CLK_MNCNTRS_WW__SBI_MNCNTR_EN_ENA              (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,SBI_MNCNTR_EN))

/*-------------------------------------------------------------------------*/
//      Bit [26] RXDSP_MNCNTR_DUAL_MODE, set (1) to enable dual mode
//      operation of the mDSP MN counter
#define MSM_CLK_MNCNTRS_WW__RXDSP_MNCNTR_DUAL_MODE_DIS     (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,RXDSP_MNCNTR_DUAL_MODE))

#define MSM_CLK_MNCNTRS_WW__RXDSP_MNCNTR_DUAL_MODE_ENA     (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,RXDSP_MNCNTR_DUAL_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [27] RXDSP_MNCNTR_RES
#define MSM_CLK_MNCNTRS_WW__RXDSP_MNCNTR_RES_NO_RESET      (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,RXDSP_MNCNTR_RES))

#define MSM_CLK_MNCNTRS_WW__RXDSP_MNCNTR_RES_RESET         (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,RXDSP_MNCNTR_RES))

/*-------------------------------------------------------------------------*/
//      Bit [28] RXDSP_MNCNTR_EN, set (1) to enable the mDSP MN counter
#define MSM_CLK_MNCNTRS_WW__RXDSP_MNCNTR_EN_DIS            (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,RXDSP_MNCNTR_EN))

#define MSM_CLK_MNCNTRS_WW__RXDSP_MNCNTR_EN_ENA            (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,RXDSP_MNCNTR_EN))

/*-------------------------------------------------------------------------*/
//      Bit [29] ADSP_MNCNTR_DUAL_MODE, set (1) to enable dual mode
//      operation of the aDSP MN counter
#define MSM_CLK_MNCNTRS_WW__ADSP_MNCNTR_DUAL_MODE_DIS      (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ADSP_MNCNTR_DUAL_MODE))

#define MSM_CLK_MNCNTRS_WW__ADSP_MNCNTR_DUAL_MODE_ENA      (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ADSP_MNCNTR_DUAL_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [30] ADSP_MNCNTR_RES
#define MSM_CLK_MNCNTRS_WW__ADSP_MNCNTR_RES_NO_RESET       (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ADSP_MNCNTR_RES))

#define MSM_CLK_MNCNTRS_WW__ADSP_MNCNTR_RES_RESET          (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ADSP_MNCNTR_RES))

/*-------------------------------------------------------------------------*/
//      Bit [31] ADSP_MNCNTR_EN, set (1) to enable the aDSP MN counter
#define MSM_CLK_MNCNTRS_WW__ADSP_MNCNTR_EN_DIS             (0U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ADSP_MNCNTR_EN))

#define MSM_CLK_MNCNTRS_WW__ADSP_MNCNTR_EN_ENA             (1U <<  \
        HWIO_SHFT(MSM_CLK_MNCNTRS,ADSP_MNCNTR_EN))



/*---------------------------------------------------------------------------
   MSM_CLK_SRCSEL1
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [1:0] DEC_CLK_DIV_SEL, define the division ratio for the
//      decoder clock source
#define MSM_CLK_SRCSEL1_WW__DEC_CLK_DIV_SEL_DIV1           (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_DIV_SEL_DIV2           (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_DIV_SEL_DIV3           (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_DIV_SEL_DIV4           (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_DIV_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [5:2] DEC_CLK_SRC_SEL, selects the clock source for the decoder,
//      interleaver, viterbi decoder, and turbo decoder
#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_TCXO           (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_PLLOUT         (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_CDMA_CX8       (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_PLLOUTDIV3     (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_DLLOUT         (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_PLLOUTDIV4     (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_SLEEP_XTAL     (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_PLLOUTDIV5     (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_RINGOSC        (8U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_PLLOUTDIV2     (9U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_EXT_TEST_CLK   (10U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_USB_XTAL       (11U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_CPLLOUT        (13U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__DEC_CLK_SRC_SEL_GND            (15U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,DEC_CLK_SRC_SEL))


/*-------------------------------------------------------------------------*/
//      Bit [6] GPS_CHIPX32_SEL, selects the clock source for GPS cx32
#define MSM_CLK_SRCSEL1_WW__GPS_CHIPX32_SEL_GPS_DIRECT     (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,GPS_CHIPX32_SEL))

#define MSM_CLK_SRCSEL1_WW__GPS_CHIPX32_SEL_GPS_MNCNTR_OUT (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,GPS_CHIPX32_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [7] RXDSP_CLK_DIV_SEL, selects clock divider for the mDSP.
//      ie. enable/disable the mDSP MN counter
#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_DIV_SEL_DIRECT       (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_DIV_SEL_MNCNTR_OUT   (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_DIV_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [8] RXDSP_CLK_SYNC,
#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SYNC_ENA             (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SYNC))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SYNC_DIS             (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SYNC))

/*-------------------------------------------------------------------------*/
//      Bits [12:9] RXDSP_CLK_SRC_SEL, selects clock mDSP clock source
#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_TCXO         (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_PLLOUT       (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_CDMA_CX8     (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_PLLOUTDIV3   (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_DPLLOUT      (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_PLLOUTDIV4   (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_SLEEP_XTAL   (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_PLLOUTDIV5   (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_RING_OSC     (8U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_PLLOUTDIV2   (9U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_EXT_TST_CLK  (10U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_USB_XTAL     (11U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_CPLLOUT      (13U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__RXDSP_CLK_SRC_SEL_GND          (15U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,RXDSP_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [13] UNUSED

/*-------------------------------------------------------------------------*/
//      Bits [15:14] UART1_CLK_SRC_SEL, selects UART1 clock source
#define MSM_CLK_SRCSEL1_WW__UART1_CLK_SRC_SEL_TCXODIV4     (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,UART1_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__UART1_CLK_SRC_SEL_SLEEP_XTAL   (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,UART1_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__UART1_CLK_SRC_SEL_TCXO         (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,UART1_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__UART1_CLK_SRC_SEL_EXT_CLK_SRC  (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,UART1_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [18:16] SBI_CLK_SRC_SEL, selects SBI clock source
#define MSM_CLK_SRCSEL1_WW__SBI_CLK_SRC_SEL_TCXODIV4       (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,SBI_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__SBI_CLK_SRC_SEL_TCXO           (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,SBI_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__SBI_CLK_SRC_SEL_RING_OSC       (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,SBI_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__SBI_CLK_SRC_SEL_SLEEP_XTAL     (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,SBI_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__SBI_CLK_SRC_SEL_SBI_MNCNTR     (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,SBI_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [19] GENERAL_CLK_SRC_SEL, selects clock source for ringer,
//      time tick, PDM1/2 (General clock regime)
#define MSM_CLK_SRCSEL1_WW__GENERAL_CLK_SRC_SEL_TCXODIV4   (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,GENERAL_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__GENERAL_CLK_SRC_SEL_SLEEP_XTAL (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,GENERAL_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [20] ADSP_CLK_DIV_SEL, selects clock divider for the aDSP.
//      ie. enable/disable the aDSP MN counter
#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_DIV_SEL_DIRECT        (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_DIV_SEL_MNCNTR_OUT    (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_DIV_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [21] ADSP_CLK_SYNC, when set (1), adsp clk src is micro clk src,
//      so the adsp can perform synchronous bus transactions
#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SYNC_ENA              (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SYNC))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SYNC_DIS              (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SYNC))

/*-------------------------------------------------------------------------*/
//      Bits [25:22] ADSP_CLK_SRC_SEL, selects aDSP clock source
#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_TCXO         (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_PLLOUT       (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_SLEEP_XTAL   (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_PLLOUTDIV3   (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_DPLLOUT      (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_PLLOUTDIV4   (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_ADSP_EXT_CLK (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_PLLOUTDIV5   (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_PLLOUTDIV2   (9U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_USB_XTAL     (11U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_CPLLOUT      (13U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__ADSP_CLK_SRC_SEL_GND          (15U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,ADSP_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [26] SBI_CLK_DIV_SEL, selects clock divider for the SBI.
//      ie. enable/disable the mDSP MN counter
#define MSM_CLK_SRCSEL1_WW__SBI_CLK_DIV_SEL_DIRECT        (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,SBI_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL1_WW__SBI_CLK_DIV_SEL_MNCNTR_OUT    (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,SBI_CLK_DIV_SEL))


/*-------------------------------------------------------------------------*/
//      Bits [28:27] UART2_CLK_SRC_SEL, selects UART2 clock source
#define MSM_CLK_SRCSEL1_WW__UART2_CLK_SRC_SEL_TCXODIV4            \
        CLK_RGM_SRC_SEL_UART2_TCXODIV4_V

#define MSM_CLK_SRCSEL1_WW__UART2_CLK_SRC_SEL_RUIM_CLK_SRC        \
        CLK_RGM_SRC_SEL_UART2_GSMDIV2_V

#define MSM_CLK_SRCSEL1_WW__UART2_CLK_SRC_SEL_TCXO                \
        CLK_RGM_SRC_SEL_UART2_TCXO_V

#define MSM_CLK_SRCSEL1_WW__UART2_CLK_SRC_SEL_UART_EXT_CLK_SRC    \
        CLK_RGM_SRC_SEL_UART2_EXTCLK_V

/*-------------------------------------------------------------------------*/
//      Bits [29] CHIPX8_SEL, set (1) to bypass the divide by 2 used to
//      generate chipx8 from chipx16.  This is used to directly insert a
//      desired frequency on chipx8.
#define MSM_CLK_SRCSEL1_WW__CHIPX8_SEL_CX8_DIV2_FROM_CX16       (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,CHIPX8_SEL))

#define MSM_CLK_SRCSEL1_WW__CHIPX8_SEL_CX8_BYPASS_DIVIDER       (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,CHIPX8_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [31:30] GPS_CLK_SRC_SEL, selects the clock source for GPS cx32
#define MSM_CLK_SRCSEL1_WW__GPS_CLK_SRC_SEL_PLLOUT              (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,GPS_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__GPS_CLK_SRC_SEL_EXT_TST_CLK         (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,GPS_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL1_WW__GPS_CLK_SRC_SEL_PLLOUTDIV2          (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL1,GPS_CLK_SRC_SEL))

#ifdef T_MSM6100B
   #define MSM_CLK_SRCSEL1_WW__GPS_CLK_SRC_SEL_CPLLOUT             (3U <<  \
           HWIO_SHFT(MSM_CLK_SRCSEL1,GPS_CLK_SRC_SEL))
#else
   #define MSM_CLK_SRCSEL1_WW__GPS_CLK_SRC_SEL_PLLOUTDIV3          (3U <<  \
           HWIO_SHFT(MSM_CLK_SRCSEL1,GPS_CLK_SRC_SEL))
#endif

/*---------------------------------------------------------------------------
   MSM_CLK_SRCSEL2
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [1:0] HKADC_CLK_SRC_SEL, controls the source for HKADC

#define MSM_CLK_SRCSEL2_WW__HKADC_CLK_SRC_SEL_TCXODIV4     (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,HKADC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__HKADC_CLK_SRC_SEL_SLEEP_XTAL   (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,HKADC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__HKADC_CLK_SRC_SEL_TCXO         (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,HKADC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__HKADC_CLK_SRC_SEL_UART_EXT_CLK_SRC (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,HKADC_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [4:2] AHB_CLK_SRC_SEL, selects the clock source for
//      the AHB PLL output, which may or may not be chosen as input to
//      the uP.
#define MSM_CLK_SRCSEL2_WW__AHB_CLK_SRC_SEL_PLLOUT              (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,AHB_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__AHB_CLK_SRC_SEL_PLLOUTDIV3          (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,AHB_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__AHB_CLK_SRC_SEL_PLLOUTDIV5          (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,AHB_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__AHB_CLK_SRC_SEL_PLL_MNCNTR_OUT      (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,AHB_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__AHB_CLK_SRC_SEL_CPLLOUT             (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,AHB_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__AHB_CLK_SRC_SEL_CPLLOUTDIV3         (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,AHB_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__AHB_CLK_SRC_SEL_CPLLOUTDIV5         (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,AHB_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__AHB_CLK_SRC_SEL_CPLLOUT_MNCNTR_OUT  (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,AHB_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [7:5] RX_SAMP_CLK_SRC_SEL, selects the clock source for
//      the Rx sampling clock
#define MSM_CLK_SRCSEL2_WW__RX_SAMP_CLK_SRC_SEL_TCXO            (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RX_SAMP_CLK_SRC_SEL))

        /* CDMA mode */
#define MSM_CLK_SRCSEL2_WW__RX_SAMP_CLK_SRC_SEL_CDMA_CX16       (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RX_SAMP_CLK_SRC_SEL))

        /* GPS mode */
#define MSM_CLK_SRCSEL2_WW__RX_SAMP_CLK_SRC_SEL_GPS_CX32        (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RX_SAMP_CLK_SRC_SEL))

        /* DFM QRF */
#define MSM_CLK_SRCSEL2_WW__RX_SAMP_CLK_SRC_SEL_TCXODIV4        (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RX_SAMP_CLK_SRC_SEL))

        /* DFM ZIF */
#define MSM_CLK_SRCSEL2_WW__RX_SAMP_CLK_SRC_SEL_TCXODIV2        (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RX_SAMP_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [9:8] RX_FRONT_CLK_SRC_SEL, selects the clock source for
//      the Rx front end (baseband filter, DVGA, etc)
#define MSM_CLK_SRCSEL2_WW__RX_FRONT_CLK_SRC_SEL_TCXO           (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RX_FRONT_CLK_SRC_SEL))

        /* CDMA */
#define MSM_CLK_SRCSEL2_WW__RX_FRONT_CLK_SRC_SEL_CDMA_CX16      (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RX_FRONT_CLK_SRC_SEL))

        /* GPS */
#define MSM_CLK_SRCSEL2_WW__RX_FRONT_CLK_SRC_SEL_GPS_CX16       (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RX_FRONT_CLK_SRC_SEL))

        /* DFM */
#define MSM_CLK_SRCSEL2_WW__RX_FRONT_CLK_SRC_SEL_DFM_720K       (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RX_FRONT_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [11] ICODEC_CLK_DIV_SEL, selects the division ratio for the
//      selected clocks source for the internal wideband codec circuit.
#define MSM_CLK_SRCSEL2_WW__ICODEC_CLK_DIV_SEL_DIRECT             (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ICODEC_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL2_WW__ICODEC_CLK_DIV_SEL_MNCNTR_OUT         (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ICODEC_CLK_DIV_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [14:12] ICODEC_CLK_SRC_SEL, selects the clock source for the
//      internal wideband codec circuit
#define MSM_CLK_SRCSEL2_WW__ICODEC_CLK_SRC_SEL_TCXO             (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ICODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ICODEC_CLK_SRC_SEL_CPLLOUT          (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ICODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ICODEC_CLK_SRC_SEL_CPLLOUTDIV10     (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ICODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ICODEC_CLK_SRC_SEL_CPLLOUTDIV12     (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ICODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ICODEC_CLK_SRC_SEL_PLLOUT           (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ICODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ICODEC_CLK_SRC_SEL_PLLOUTDIV2       (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ICODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ICODEC_CLK_SRC_SEL_PLLOUTDIV12      (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ICODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ICODEC_CLK_SRC_SEL_EXT_CLK_SRC      (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ICODEC_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [17:15] ECODEC_CLK_SRC_SEL, selects the clock source for the
//      external codec clock regime
#define MSM_CLK_SRCSEL2_WW__ECODEC_CLK_SRC_SEL_PLLOUTDIV48      (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ECODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ECODEC_CLK_SRC_SEL_PLLOUTDIV60      (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ECODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ECODEC_CLK_SRC_SEL_PLLOUTDIV24      (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ECODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ECODEC_CLK_SRC_SEL_PLLOUTDIV72      (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ECODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ECODEC_CLK_SRC_SEL_PLLOUTDIV96      (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ECODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ECODEC_CLK_SRC_SEL_CPLL_MNCNTR_OUT  (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ECODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ECODEC_CLK_SRC_SEL_BT_MNCNTR_OUT    (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ECODEC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__ECODEC_CLK_SRC_SEL_CPLLOUTDIV30     (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,ECODEC_CLK_SRC_SEL))


/*-------------------------------------------------------------------------*/
//      Bits [18] SRCH4_CX8_CLK_SRC_SEL, selects the clock source for the
//      Searcher4 CX8 clock regime
#define MSM_CLK_SRCSEL2_WW__SRCH4_CX8_SRC_SEL_CDMA_CX8          (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,SRCH4_CX8_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__SRCH4_CX8_SRC_SEL_GPS_CX8           (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,SRCH4_CX8_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [21:19] SRCH4_CLK_SRC_SEL, selects the clock source for the
//      Searcher4 clock regime
#define MSM_CLK_SRCSEL2_WW__SRCH4_CLK_SRC_SEL_CDMA_CX8          (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,SRCH4_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__SRCH4_CLK_SRC_SEL_GPS_CX8           (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,SRCH4_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__SRCH4_CLK_SRC_SEL_PLLOUTDIV3        (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,SRCH4_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__SRCH4_CLK_SRC_SEL_PLLOUTDIV5        (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,SRCH4_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__SRCH4_CLK_SRC_SEL_PLLOUTDIV6        (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,SRCH4_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__SRCH4_CLK_SRC_SEL_RING_OSC_DIV      (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,SRCH4_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__SRCH4_CLK_SRC_SEL_EXT_CLK_SRC       (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,SRCH4_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__SRCH4_CLK_SRC_SEL_PLLOUTDIV8        (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,SRCH4_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [23:22] RING_OSC_DIV_SEL, selects the division ratio for the
//      external divider on the ring oscillator div source.
#define MSM_CLK_SRCSEL2_WW__RING_OSC_DIV_SEL_DIV1               (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RING_OSC_DIV_SEL))

#define MSM_CLK_SRCSEL2_WW__RING_OSC_DIV_SEL_DIV2               (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RING_OSC_DIV_SEL))

#define MSM_CLK_SRCSEL2_WW__RING_OSC_DIV_SEL_DIV3               (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RING_OSC_DIV_SEL))

#define MSM_CLK_SRCSEL2_WW__RING_OSC_DIV_SEL_DIV4               (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,RING_OSC_DIV_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [25:24] MMCC_CLK_DIV_SEL, selects the division ratio for the
//      external divider on the MMCC div source.
#define MSM_CLK_SRCSEL2_WW__MMCC_CLK_DIV_SEL_DIV1               (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,MMCC_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL2_WW__MMCC_CLK_DIV_SEL_DIV2               (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,MMCC_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL2_WW__MMCC_CLK_DIV_SEL_DIV3               (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,MMCC_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL2_WW__MMCC_CLK_DIV_SEL_DIV4               (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,MMCC_CLK_DIV_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [27:26] MMCC_CLK_SRC_SEL, selects the source for the MMCC
//      clk.
#define MSM_CLK_SRCSEL2_WW__MMCC_CLK_SRC_SEL_TCXO               (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,MMCC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__MMCC_CLK_SRC_SEL_PLLOUTDIV5               (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,MMCC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__MMCC_CLK_SRC_SEL_PLLOUTDIV3               (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,MMCC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__MMCC_CLK_SRC_SEL_EXT_CLK_SRC              (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,MMCC_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [28] MMCDIV_CLK_SRC_SEL, selects the clock source for the
//      MMCDIV
#define MSM_CLK_SRCSEL2_WW__MMCDIV_CLK_SRC_SEL_INT_CLK_SRC         (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,MMCDIV_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL2_WW__MMCDIV_CLK_SRC_SEL_EXT_CLK_SRC         (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL2,MMCDIV_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [31:30] UART3_CLK_SRC_SEL, selects the clock source for UART3
#define MSM_CLK_SRCSEL2_WW__UART3_CLK_SRC_SEL_TCXODIV4                  \
        CLK_RGM_SRC_SEL_UART3_TCXODIV4_V

#define MSM_CLK_SRCSEL2_WW__UART3_CLK_SRC_SEL_RUIM_CLK_SRC              \
        CLK_RGM_SRC_SEL_UART3_GSMDIV2_V

#define MSM_CLK_SRCSEL2_WW__UART3_CLK_SRC_SEL_TCXO                      \
        CLK_RGM_SRC_SEL_UART3_TCXO_V

#define MSM_CLK_SRCSEL2_WW__UART3_CLK_SRC_SEL_UART_EXT_CLK_SRC          \
        CLK_RGM_SRC_SEL_UART3_EXTCLK_V


/*---------------------------------------------------------------------------
   MSM_CLK_SRCSEL3
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [1:0] PDM_CLK_SRC_SEL, selects the PDM CX16 clock source.  Also
//      used for the rx_front PDM
#define MSM_CLK_SRCSEL3_WH__PDM_CLK_SRC_SEL_CDMA_CX16           (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,PDM_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__PDM_CLK_SRC_SEL_GPS_CX16            (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,PDM_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__PDM_CLK_SRC_SEL_TCXO                (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,PDM_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [2] AGC_CLK_SRC_SEL, selects the AGC CX8 clock source.
#define MSM_CLK_SRCSEL3_WH__AGC_CLK_SRC_SEL_CDMA_CX8            (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,AGC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__AGC_CLK_SRC_SEL_GPS_CX8             (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,AGC_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [4:3] SDAC_CLK_SRC_SEL, selects the SDAC clock source.
#define MSM_CLK_SRCSEL3_WH__SDAC_CLK_SRC_SEL_SDAC_MNCNTR_OUT    (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,SDAC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__SDAC_CLK_SRC_SEL_1P024_MHZ          (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,SDAC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__SDAC_CLK_SRC_SEL_EXT_TST_CLK        (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,SDAC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__SDAC_CLK_SRC_SEL_EXT_AUX_CODEC_CLK  (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,SDAC_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [7:5] RX_MOD_CLK_SRC_SEL, selects the SD modulator clock source
#define MSM_CLK_SRCSEL3_WH__RX_MOD_CLK_SRC_SEL_TCXODIV2         (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,RX_MOD_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__RX_MOD_CLK_SRC_SEL_TCXODIV4         (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,RX_MOD_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__RX_MOD_CLK_SRC_SEL_GPS_CX32         (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,RX_MOD_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__RX_MOD_CLK_SRC_SEL_PLLOUTDIV5       (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,RX_MOD_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__RX_MOD_CLK_SRC_SEL_PLLOUTDIV10      (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,RX_MOD_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__RX_MOD_CLK_SRC_SEL_PLLOUTDIV12P5    (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,RX_MOD_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__RX_MOD_CLK_SRC_SEL_EXT_TST_CLK      (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,RX_MOD_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__RX_MOD_CLK_SRC_SEL_PLLOUTDIV15      (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,RX_MOD_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [8] GENERAL_CLK_DIV_SEL, selects clock divider for GENERAL.
//      ie. enable/disable the GENERAL MN counter
#define MSM_CLK_SRCSEL3_WW__GENERAL_CLK_DIV_SEL_DIRECT              (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,GENERAL_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL3_WW__GENERAL_CLK_DIV_SEL_MNCNTR_OUT          (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,GENERAL_CLK_DIV_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [10:9] GENERAL_CLK_SRC_SEL, selects the GENERAL clock source
#define MSM_CLK_SRCSEL3_WH__GENERAL_CLK_SRC_SEL_TCXO                (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,GENERAL_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__GENERAL_CLK_SRC_SEL_PLLOUT              (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,GENERAL_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__GENERAL_CLK_SRC_SEL_PLLOUTDIV2          (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,GENERAL_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__GENERAL_CLK_SRC_SEL_CPLLOUT             (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,GENERAL_CLK_SRC_SEL))
#ifdef T_MSM6100B


    /*-------------------------------------------------------------------------*/
    //      Bits [12:11] MOT_EST_CLK_DIV_SEL.  These bits define the division
    //      ration for the motion estimator clock source.
    #define MSM_CLK_SRCSEL3_WH__MOT_EST_CLK_DIV_SEL_DIV1            (0U <<  \
            HWIO_SHFT(MSM_CLK_SRCSEL3,MOT_EST_CLK_DIV_SEL))

    #define MSM_CLK_SRCSEL3_WH__MOT_EST_CLK_DIV_SEL_DIV2            (1U <<  \
            HWIO_SHFT(MSM_CLK_SRCSEL3,MOT_EST_CLK_DIV_SEL))

    #define MSM_CLK_SRCSEL3_WH__MOT_EST_CLK_DIV_SEL_DIV3            (2U <<  \
            HWIO_SHFT(MSM_CLK_SRCSEL3,MOT_EST_CLK_DIV_SEL))

    #define MSM_CLK_SRCSEL3_WH__MOT_EST_CLK_DIV_SEL_DIV4            (3U <<  \
            HWIO_SHFT(MSM_CLK_SRCSEL3,MOT_EST_CLK_DIV_SEL))

    /*-------------------------------------------------------------------------*/
    //      Bits [14:13] MOT_EST_CLK_SRC_SEL.  These bits select the motion
    //      estimator clock source.  Intended to be 40-60 MHz
    #define MSM_CLK_SRCSEL3_WH__MOT_EST_CLK_SRC_SEL_TCXO            (0U <<  \
            HWIO_SHFT(MSM_CLK_SRCSEL3,MOT_EST_CLK_SRC_SEL))

    #define MSM_CLK_SRCSEL3_WH__MOT_EST_CLK_SRC_SEL_ADSP_CLK        (1U <<  \
            HWIO_SHFT(MSM_CLK_SRCSEL3,MOT_EST_CLK_SRC_SEL))

    #define MSM_CLK_SRCSEL3_WH__MOT_EST_CLK_SRC_SEL_MDSP_CLK        (2U <<  \
            HWIO_SHFT(MSM_CLK_SRCSEL3,MOT_EST_CLK_SRC_SEL))

    #define MSM_CLK_SRCSEL3_WH__MOT_EST_CLK_SRC_SEL_GND             (3U <<  \
            HWIO_SHFT(MSM_CLK_SRCSEL3,MOT_EST_CLK_SRC_SEL))


#endif // T_MSM6100B

/*-------------------------------------------------------------------------*/
//      Bits [24:15] UNUSED

/*-------------------------------------------------------------------------*/
//      Bit [25] MAC_CLK_SRC_SEL, selects the MAC clock source.
#define MSM_CLK_SRCSEL3_WH__MAC_CLK_SRC_SEL_CDMA_CX16            (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,MAC_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__MAC_CLK_SRC_SEL_CDMA_CX8             (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,MAC_CLK_SRC_SEL))

/*-------------------------------------------------------------------------*/
//      Bits [27:26] UNUSED

/*-------------------------------------------------------------------------*/
//      Bit [28] CAMIF_CLK_SRC_SEL, selects the CAMIF clock source.
#define MSM_CLK_SRCSEL3_WH__CAMIF_CLK_SRC_SEL_TCXODIV4            (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,CAMIF_CLK_SRC_SEL))

#define MSM_CLK_SRCSEL3_WH__CAMIF_CLK_SRC_SEL_CAMIF_EXT_SRC       (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL3,CAMIF_CLK_SRC_SEL))


/*---------------------------------------------------------------------------
   MSM_CLK_SRCSEL4
---------------------------------------------------------------------------*/

//      Bit [3:0] PLLDIVSEL, Select the main PLL pre divide by N counter
#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV1                  (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV2                  (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV3                  (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV4                  (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV5                  (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV6                  (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV7                  (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV8                  (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV9                  (8U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV10                 (9U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV11                 (10U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV12                 (11U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV13                 (12U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV14                 (13U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV15                 (14U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))

#define MSM_CLK_SRCSEL4_WW__PLLDIVSEL_DIV16                 (15U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLDIVSEL))


/*-------------------------------------------------------------------------*/
//      Bit [5:4] PLLSRCSEL, selects the clock source for the main PLL
#define MSM_CLK_SRCSEL4_WW__PLLSRCSEL_TCXO                  (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLSRCSEL))

#define MSM_CLK_SRCSEL4_WW__PLLSRCSEL_USB_XTAL              (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLSRCSEL))

#define MSM_CLK_SRCSEL4_WW__PLLSRCSEL_RING_OSC              (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLLSRCSEL))

/*-------------------------------------------------------------------------*/
//      Bit [9:6] CHIPX16SEL, selects the clock source for the CDMA
//      chipx16 clock
#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_TCXO                    (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_PLLOUTDIV10             (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_EXT_GPIO_CLK            (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_PLLOUT                  (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_PLLOUTDIV5              (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_RING_OSC                (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_PLLOUTDIV12P5           (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_RESERVED                (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_TCXO_AFT_MND            (8U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_PLLOUTDIV10_AFT_MND     (9U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_EXT_GPIO_CLK_AFT_MND    (10U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_PLLOUT_AFT_MND          (11U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_PLLOUTDIV5_AFT_MND      (12U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_RING_OSC_AFT_MND        (13U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_PLLOUTDIV12P5_AFT_MND   (14U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

#define MSM_CLK_SRCSEL4_WW__CHIPX16_SEL_RESERVED_AFT_MND        (15U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,CHIPX16_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [12:10] GPSCLK_DELAY, set the delay for the GPS mncntr reset.
//      MN counter
#define MSM_CLK_SRCSEL4_WW__GPSCLK_DELAY_0                  (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,GPSCLK_DELAY))

#define MSM_CLK_SRCSEL4_WW__GPSCLK_DELAY_1                  (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,GPSCLK_DELAY))

#define MSM_CLK_SRCSEL4_WW__GPSCLK_DELAY_2                  (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,GPSCLK_DELAY))

#define MSM_CLK_SRCSEL4_WW__GPSCLK_DELAY_3                  (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,GPSCLK_DELAY))

#define MSM_CLK_SRCSEL4_WW__GPSCLK_DELAY_4                  (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,GPSCLK_DELAY))

#define MSM_CLK_SRCSEL4_WW__GPSCLK_DELAY_5                  (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,GPSCLK_DELAY))

#define MSM_CLK_SRCSEL4_WW__GPSCLK_DELAY_6                  (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,GPSCLK_DELAY))

#define MSM_CLK_SRCSEL4_WW__GPSCLK_DELAY_7                  (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,GPSCLK_DELAY))


/*-------------------------------------------------------------------------*/
//      Bit [13] PLL_DIV_EN, enables the PLL predivide by N counter
#define MSM_CLK_SRCSEL4_WW__PLL_DIV_EN_DIS                  (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLL_DIV_EN))

#define MSM_CLK_SRCSEL4_WW__PLL_DIV_EN_ENA                  (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLL_DIV_EN))


/*-------------------------------------------------------------------------*/
//      Bit [14] PLL_PREMULT_SEL, selects the optional 2x input to the PLL
#define MSM_CLK_SRCSEL4_WW__PLL_PREMULT_SEL_BYPASS          (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLL_PREMULT_SEL))

#define MSM_CLK_SRCSEL4_WW__PLL_PREMULT_SEL_ENABLE          (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,PLL_PREMULT_SEL))


/*-------------------------------------------------------------------------*/
//      Bit [20:15] I2C_CLK_DIV_SEL, sourced by micro_clk_src
#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV1          (0U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV2          (1U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV3          (2U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV4          (3U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV5          (4U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV6          (5U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV7          (6U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV8          (7U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV9          (8U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV10          (9U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV11          (10U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV12          (11U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV13          (12U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV14          (13U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV15          (14U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV16          (15U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV17          (16U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV18          (17U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV19          (18U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV20          (19U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV21          (20U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV22          (21U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV23          (22U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV24          (23U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV25          (24U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV26          (25U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV27          (26U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV28          (27U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV29          (28U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV30          (29U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV31          (30U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))

#define MSM_CLK_SRCSEL4_WW__I2C_CLK_DIV_SEL_DIV32          (31U <<  \
        HWIO_SHFT(MSM_CLK_SRCSEL4,I2C_CLK_DIV_SEL))


/*---------------------------------------------------------------------------
   RING_OSC
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [5:4] RING_OSC_EN, enable bits to both ring oscillator core
#define RING_OSC_WB__RING_OSC_EN_CORE1DIS_CORE2DIS               (0U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_EN))

#define RING_OSC_WB__RING_OSC_EN_CORE1ENA_CORE2DIS               (1U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_EN))

#define RING_OSC_WB__RING_OSC_EN_CORE1DIS_CORE2ENA               (2U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_EN))

#define RING_OSC_WB__RING_OSC_EN_CORE1ENA_CORE2ENA               (3U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_EN))

/*-------------------------------------------------------------------------*/
//      Bit [3:0] RING_OSC_SEL, selects the divide ratio for the internal
//      dividers of the ring oscillator, and the desired core to use as
//      the ring_osc_div_clk.  Note that only one core is used as the source
//      for ring_osc_div_clk
#define RING_OSC_WB__RING_OSC_SEL_CORE1_DIV1                     (0U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_SEL))

#define RING_OSC_WB__RING_OSC_SEL_CORE1_DIV2                     (1U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_SEL))

#define RING_OSC_WB__RING_OSC_SEL_CORE1_DIV4                     (2U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_SEL))

#define RING_OSC_WB__RING_OSC_SEL_CORE1_DIV8                     (3U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_SEL))

#define RING_OSC_WB__RING_OSC_SEL_CORE1_DIV16                    (4U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_SEL))

#define RING_OSC_WB__RING_OSC_SEL_CORE2_DIV1                     (8U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_SEL))

#define RING_OSC_WB__RING_OSC_SEL_CORE2_DIV2                     (9U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_SEL))

#define RING_OSC_WB__RING_OSC_SEL_CORE2_DIV4                     (10U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_SEL))

#define RING_OSC_WB__RING_OSC_SEL_CORE2_DIV8                     (11U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_SEL))

#define RING_OSC_WB__RING_OSC_SEL_CORE2_DIV16                    (12U <<  \
        HWIO_SHFT(RING_OSC,RING_OSC_SEL))





//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
//                Program Clock Control CONFIGURATION
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/*---------------------------------------------------------------------------
   DEINT_PGM_CLK_CNTL0
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [6] RW_CLK_CTL_EN, controls deint N0/N1 RAM clock falling edge timing
//      in order to fine tune speed.
#define DEINT_PGM_CLK_CNTL0_WB__RW_CLK_CTL_EN_READ_WRITE_SAME_CLKS             (0U <<  \
        HWIO_SHFT(DEINT_PGM_CLK_CNTL0,RW_CLK_CTL_EN))

#define DEINT_PGM_CLK_CNTL0_WB__RW_CLK_CTL_EN_PROGABLE_READ_DEFAULT_WRITE_CLKS (1U <<  \
        HWIO_SHFT(DEINT_PGM_CLK_CNTL0,RW_CLK_CTL_EN))

/*-------------------------------------------------------------------------*/
//      Bit [5] CLK_CTL_EN, controls deint N0/N1 RAM clock falling edge timing
//      in order to fine tune speed.
#define DEINT_PGM_CLK_CNTL0_WB__CLK_CTL_EN_DEFAULT_CLK             (0U <<  \
        HWIO_SHFT(DEINT_PGM_CLK_CNTL0,CLK_CTL_EN))

#define DEINT_PGM_CLK_CNTL0_WB__CLK_CTL_EN_ENABLE_PROGRAMMABLE     (1U <<  \
        HWIO_SHFT(DEINT_PGM_CLK_CNTL0,CLK_CTL_EN))


/*---------------------------------------------------------------------------
   DEINT_PGM_CLK_CNTL1
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [6] RW_CLK_CTL_EN, controls deint P0/P1 RAM clock falling edge timing
//      in order to fine tune speed.
#define DEINT_PGM_CLK_CNTL1_WB__RW_CLK_CTL_EN_READ_WRITE_SAME_CLKS             (0U <<  \
        HWIO_SHFT(DEINT_PGM_CLK_CNTL1,RW_CLK_CTL_EN))

#define DEINT_PGM_CLK_CNTL1_WB__RW_CLK_CTL_EN_PROGABLE_READ_DEFAULT_WRITE_CLKS (1U <<  \
        HWIO_SHFT(DEINT_PGM_CLK_CNTL1,RW_CLK_CTL_EN))

/*-------------------------------------------------------------------------*/
//      Bit [5] CLK_CTL_EN, controls deint P0/P1 RAM clock falling edge timing
//      in order to fine tune speed.
#define DEINT_PGM_CLK_CNTL1_WB__CLK_CTL_EN_DEFAULT_CLK             (0U <<  \
        HWIO_SHFT(DEINT_PGM_CLK_CNTL1,CLK_CTL_EN))

#define DEINT_PGM_CLK_CNTL1_WB__CLK_CTL_EN_ENABLE_PROGRAMMABLE     (1U <<  \
        HWIO_SHFT(DEINT_PGM_CLK_CNTL1,CLK_CTL_EN))


/*---------------------------------------------------------------------------
   APPRAM_PGM_CLK_CNTL
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [6] RW_CLK_CTL_EN, controls turbo decoder appRAM clock falling edge timing
//      in order to fine tune speed.
#define APPRAM_PGM_CLK_CNTL_WB__RW_CLK_CTL_EN_READ_WRITE_SAME_CLKS             (0U <<  \
        HWIO_SHFT(APPRAM_PGM_CLK_CNTL,RW_CLK_CTL_EN))

#define APPRAM_PGM_CLK_CNTL_WB__RW_CLK_CTL_EN_PROGABLE_READ_DEFAULT_WRITE_CLKS (1U <<  \
        HWIO_SHFT(APPRAM_PGM_CLK_CNTL,RW_CLK_CTL_EN))

/*-------------------------------------------------------------------------*/
//      Bit [5] CLK_CTL_EN, controls deint N0/N1 RAM clock falling edge timing
//      in order to fine tune speed.
#define APPRAM_PGM_CLK_CNTL_WB__CLK_CTL_EN_DEFAULT_CLK             (0U <<  \
        HWIO_SHFT(APPRAM_PGM_CLK_CNTL,CLK_CTL_EN))

#define APPRAM_PGM_CLK_CNTL_WB__CLK_CTL_EN_ENABLE_PROGRAMMABLE     (1U <<  \
        HWIO_SHFT(APPRAM_PGM_CLK_CNTL,CLK_CTL_EN))


/*---------------------------------------------------------------------------
   SAMPRAM_PGM_CLK_CNTL
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [6] RW_CLK_CTL_EN, controls searcher 4 SAMPRAM clock falling edge timing
//      in order to fine tune speed.
#define SAMPRAM_PGM_CLK_CNTL_WB__RW_CLK_CTL_EN_READ_WRITE_SAME_CLKS             (0U <<  \
        HWIO_SHFT(SAMPRAM_PGM_CLK_CNTL,RW_CLK_CTL_EN))

#define SAMPRAM_PGM_CLK_CNTL_WB__RW_CLK_CTL_EN_PROGABLE_READ_DEFAULT_WRITE_CLKS (1U <<  \
        HWIO_SHFT(SAMPRAM_PGM_CLK_CNTL,RW_CLK_CTL_EN))

/*-------------------------------------------------------------------------*/
//      Bit [5] CLK_CTL_EN, controls deint N0/N1 RAM clock falling edge timing
//      in order to fine tune speed.
#define SAMPRAM_PGM_CLK_CNTL_WB__CLK_CTL_EN_DEFAULT_CLK             (0U <<  \
        HWIO_SHFT(SAMPRAM_PGM_CLK_CNTL,CLK_CTL_EN))

#define SAMPRAM_PGM_CLK_CNTL_WB__CLK_CTL_EN_ENABLE_PROGRAMMABLE     (1U <<  \
        HWIO_SHFT(SAMPRAM_PGM_CLK_CNTL,CLK_CTL_EN))


/*---------------------------------------------------------------------------
   VDSP_PGM_CLK_CNTL0
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [6] RW_CLK_CTL_EN, controls VDSP RAMA clock falling edge timing
//      in order to fine tune speed.
#define VDSP_PGM_CLK_CNTL0_WB__RW_CLK_CTL_EN_READ_WRITE_SAME_CLKS             (0U <<  \
        HWIO_SHFT(VDSP_PGM_CLK_CNTL0,RW_CLK_CTL_EN))

#define VDSP_PGM_CLK_CNTL0_WB__RW_CLK_CTL_EN_PROGABLE_READ_DEFAULT_WRITE_CLKS (1U <<  \
        HWIO_SHFT(VDSP_PGM_CLK_CNTL0,RW_CLK_CTL_EN))

/*-------------------------------------------------------------------------*/
//      Bit [5] CLK_CTL_EN, controls deint N0/N1 RAM clock falling edge timing
//      in order to fine tune speed.
#define VDSP_PGM_CLK_CNTL0_WB__CLK_CTL_EN_DEFAULT_CLK             (0U <<  \
        HWIO_SHFT(VDSP_PGM_CLK_CNTL0,CLK_CTL_EN))

#define VDSP_PGM_CLK_CNTL0_WB__CLK_CTL_EN_ENABLE_PROGRAMMABLE     (1U <<  \
        HWIO_SHFT(VDSP_PGM_CLK_CNTL0,CLK_CTL_EN))


/*---------------------------------------------------------------------------
   VDSP_PGM_CLK_CNTL1
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [6] RW_CLK_CTL_EN, controls VDSP RAMB clock falling edge timing
//      in order to fine tune speed.
#define VDSP_PGM_CLK_CNTL1_WB__RW_CLK_CTL_EN_READ_WRITE_SAME_CLKS             (0U <<  \
        HWIO_SHFT(VDSP_PGM_CLK_CNTL1,RW_CLK_CTL_EN))

#define VDSP_PGM_CLK_CNTL1_WB__RW_CLK_CTL_EN_PROGABLE_READ_DEFAULT_WRITE_CLKS (1U <<  \
        HWIO_SHFT(VDSP_PGM_CLK_CNTL1,RW_CLK_CTL_EN))

/*-------------------------------------------------------------------------*/
//      Bit [5] CLK_CTL_EN, controls deint N0/N1 RAM clock falling edge timing
//      in order to fine tune speed.
#define VDSP_PGM_CLK_CNTL1_WB__CLK_CTL_EN_DEFAULT_CLK             (0U <<  \
        HWIO_SHFT(VDSP_PGM_CLK_CNTL1,CLK_CTL_EN))

#define VDSP_PGM_CLK_CNTL1_WB__CLK_CTL_EN_ENABLE_PROGRAMMABLE     (1U <<  \
        HWIO_SHFT(VDSP_PGM_CLK_CNTL1,CLK_CTL_EN))

#ifdef T_MSM6100B

    /*---------------------------------------------------------------------------
       USB_CLK_CNTL
    ---------------------------------------------------------------------------*/

    /*-------------------------------------------------------------------------*/
    //      Bits [1:0] CLK_BY_4_DELAY, delays free_rclk_by_4 by 0 ns, 3 ns,
    //      6 ns or 9 ns.
    #define USB_CLK_CNTL_WB__CLK_BY_4_DELAY_0NS                   (0U <<  \
            HWIO_SHFT(USB_CLK_CNTL,CLK_BY_4_DELAY))

    #define USB_CLK_CNTL_WB__CLK_BY_4_DELAY_3NS                   (1U <<  \
            HWIO_SHFT(USB_CLK_CNTL,CLK_BY_4_DELAY))

    #define USB_CLK_CNTL_WB__CLK_BY_4_DELAY_6NS                   (2U <<  \
            HWIO_SHFT(USB_CLK_CNTL,CLK_BY_4_DELAY))

    #define USB_CLK_CNTL_WB__CLK_BY_4_DELAY_9NS                   (3U <<  \
            HWIO_SHFT(USB_CLK_CNTL,CLK_BY_4_DELAY))

    /*-------------------------------------------------------------------------*/
    //      Bits [3:2] RX_CLK_DELAY, delays rx_clk by 0 ns, 3 ns, 6 ns or 9 ns.
    #define USB_CLK_CNTL_WB__RX_CLK_DELAY_0NS                     (0U <<  \
            HWIO_SHFT(USB_CLK_CNTL,RX_CLK_DELAY))

    #define USB_CLK_CNTL_WB__RX_CLK_DELAY_3NS                     (1U <<  \
            HWIO_SHFT(USB_CLK_CNTL,RX_CLK_DELAY))

    #define USB_CLK_CNTL_WB__RX_CLK_DELAY_6NS                     (2U <<  \
            HWIO_SHFT(USB_CLK_CNTL,RX_CLK_DELAY))

    #define USB_CLK_CNTL_WB__RX_CLK_DELAY_9NS                     (3U <<  \
            HWIO_SHFT(USB_CLK_CNTL,RX_CLK_DELAY))


#endif // T_MSM6100B


/*---------------------------------------------------------------------------
   MODEM_RAM_SELECT
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [5] DEC_BOOT_RAM_EN, configures AHB to decode DEINT RAM to 0xFFFF0000
//      for Boot SRAM.  bits 4:0 must be set to 0x0 in this case, so the RAM
//      is remapped to allow booting from it.
#define MODEM_RAM_SELECT_WH__DEC_BOOT_RAM_EN_ENABLE_BOOT_SRAM     (1U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,DEC_BOOT_RAM_EN))

#define MODEM_RAM_SELECT_WH__DEC_BOOT_RAM_EN_DISABLE_BOOT_SRAM    (0U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,DEC_BOOT_RAM_EN))

/*-------------------------------------------------------------------------*/
//      Bits [7:6] Reserved

/*-------------------------------------------------------------------------*/
//      Bit [8] SRCH4_RAM_CFG, configures SRCH4 to use its dedicated sample ram
//      or else use deinterleaver RAM -- bits 1:0 should be 10 in this case.
#define MODEM_RAM_SELECT_WH__SRCH4_RAM_CFG_USES_DEDICATED_RAM     (1U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,SRCH4_RAM_CFG))

#define MODEM_RAM_SELECT_WH__SRCH4_RAM_CFG_USES_DEINT_RAM         (0U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,SRCH4_RAM_CFG))


//      Bit [4:0] MODEM_RAM_SEL, configures the RAM to be used by different
//      blocks.
#define MODEM_RAM_SELECT_WH__DEINT_RAM_USED_BY_BOOT_BLOCK          (0U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,MODEM_RAM_SEL))
#define MODEM_RAM_SELECT_WH__DEINT_RAM_USED_BY_MDSP                (1U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,MODEM_RAM_SEL))
#define MODEM_RAM_SELECT_WH__DEINT_RAM_USED_BY_SEARCHER4           (2U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,MODEM_RAM_SEL))
#define MODEM_RAM_SELECT_WH__DEINT_RAM_USED_BY_DEINT               (3U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,MODEM_RAM_SEL))



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
//                AHB CONFIGURATION
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/*---------------------------------------------------------------------------
   EBI1_CFG
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [6] PRIORITY, sets ARM vs DSP AHB priority to XMEMC
#define EBI1_CFG_WH__PRIORITY_ARM_HIGH                         (0U <<  \
        HWIO_SHFT(EBI1_CFG,PRIORITY))

#define EBI1_CFG_WH__PRIORITY_DSP_HIGH                         (1U <<  \
        HWIO_SHFT(EBI1_CFG,PRIORITY))

/*-------------------------------------------------------------------------*/
//      Bit [7] PWRSAVE_CLK_DIS, sets burst clock to always toggle or only
//      during accesses
#define EBI1_CFG_WH__PWRSAVE_CLK_DIS_ALWAYS_TOGGLE        (0U <<  \
        HWIO_SHFT(EBI1_CFG,PWRSAVE_CLK_DIS))

#define EBI1_CFG_WH__PWRSAVE_CLK_DIS_ONLY_DURING_ACCESSES (1U <<  \
        HWIO_SHFT(EBI1_CFG,PWRSAVE_CLK_DIS))

/*-------------------------------------------------------------------------*/
//      Bit [8] SDRAM_PRESENT
#define EBI1_CFG_WH__SDRAM_PRESENT_DIS                    (0U <<  \
        HWIO_SHFT(EBI1_CFG,SDRAM_PRESENT))

#define EBI1_CFG_WH__SDRAM_PRESENT_ENA                    (1U <<  \
        HWIO_SHFT(EBI1_CFG,SDRAM_PRESENT))

#ifdef T_MSM6100B

    /*-------------------------------------------------------------------------*/
    //      Bit [10] PWRSAVE_HANDSHAKE_CTL, when set (1), allows the
    //      PWRSAVE_CLK_DIS bit to turn the ROM1_CLK ON/OFF.
    #define EBI1_CFG_WH__PWRSAVE_HANDSHAKE_CTL_DIS            (0U <<  \
            HWIO_SHFT(EBI1_CFG,PWRSAVE_HANDSHAKE_CTL))

    #define EBI1_CFG_WH__PWRSAVE_HANDSHAKE_CTL_ENA            (1U <<  \
            HWIO_SHFT(EBI1_CFG,PWRSAVE_HANDSHAKE_CTL))

    /*-------------------------------------------------------------------------*/
    //      Bit [11] FBCLK_CLKOUT_DLY_ENA, setting this bit to zero means that the
    //      CSC in the clock block that generates FBCLK will use the undelayed clock
    //      source.  Setting this bit to 1 will mean that the CSC in the clock block
    //      that generates FBCLK will have the delay associated with the PGM_DLY_CTL
    //      value in its path.
    #define EBI1_CFG_WH__FBCLK_CLKOUT_DLY_ENA_UNDELAYED       (0U <<  \
            HWIO_SHFT(EBI1_CFG,FBCLK_CLKOUT_DLY_ENA))

    #define EBI1_CFG_WH__FBCLK_CLKOUT_DLY_ENA_DELAYED         (1U <<  \
            HWIO_SHFT(EBI1_CFG,FBCLK_CLKOUT_DLY_ENA))

    /*-------------------------------------------------------------------------*/
    //      Bit [20] EBI1_FBCLK_POL, when set to 1, the polarity of EBI1_FBCLK is
    //      inverted.  When set to 0, the polarity of EBI1_FBCLK remains unchanged.
    #define EBI1_CFG_WH__EBI1_FBCLK_POL_NONINV                    (0U <<  \
            HWIO_SHFT(EBI1_CFG,EBI1_FBCLK_POL))

    #define EBI1_CFG_WH__EBI1_FBCLK_POL_INV                       (1U <<  \
            HWIO_SHFT(EBI1_CFG,EBI1_FBCLK_POL))

    /*-------------------------------------------------------------------------*/
    //      Bit [21] EBI1_DSP_LOCK_ENA, when set to 1, the DSP's HMASTLOCK signal
    //      may be used by the Busrt Memory and MPMC controllers.  When set to 0,
    //      the HMASTLOCK is gated OFF.
    #define EBI1_CFG_WH__EBI1_DSP_LOCK_ENA_DIS                    (0U <<  \
            HWIO_SHFT(EBI1_CFG,EBI1_DSP_LOCK_ENA))

    #define EBI1_CFG_WH__EBI1_DSP_LOCK_ENA_ENA                    (1U <<  \
            HWIO_SHFT(EBI1_CFG,EBI1_DSP_LOCK_ENA))

    /*-------------------------------------------------------------------------*/
    //      Bit [22] PWRSAVE_CLK_GATE_MODE, when set to 1, ensures that the gating
    //      of the ROM1_CLK happens on the falling edge of HCLK.
    #define EBI1_CFG_WH__PWRSAVE_CLK_GATE_MODE_RISING_EDGE        (0U <<  \
            HWIO_SHFT(EBI1_CFG,PWRSAVE_CLK_GATE_MODE))

    #define EBI1_CFG_WH__PWRSAVE_CLK_GATE_MODE_FALLING_EDGE       (1U <<  \
            HWIO_SHFT(EBI1_CFG,PWRSAVE_CLK_GATE_MODE))

    /*-------------------------------------------------------------------------*/
    //      Bit [23] READ_DATA_PIPELINE_ENA, when set to 1 there is an extra
    //      clock cycle inserted for every burst of accesses to a sync burst
    //      memory.  When set to 0, there is no such delay.
    #define EBI1_CFG_WH__READ_DATA_PIPELINE_ENA_NOEXTRACYCLE      (0U <<  \
            HWIO_SHFT(EBI1_CFG,READ_DATA_PIPELINE_ENA))

    #define EBI1_CFG_WH__READ_DATA_PIPELINE_ENA_EXTRACYCLE        (1U <<  \
            HWIO_SHFT(EBI1_CFG,READ_DATA_PIPELINE_ENA))


#endif // T_MSM6100B


/*---------------------------------------------------------------------------
   EBI1_MPMC_STDY_SEL
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] MPMC_STDY_RAMCS2_SEL, select type of memory on CS2
#define EBI1_MPMC_STDY_SEL_WB__MPMC_STDY_RAMCS2_SEL_STATIC   (0U <<  \
        HWIO_SHFT(EBI1_MPMC_STDY_SEL,MPMC_STDY_RAMCS2_SEL))

#define EBI1_MPMC_STDY_SEL_WB__MPMC_STDY_RAMCS2_SEL_DYNAMIC  (1U <<  \
        HWIO_SHFT(EBI1_MPMC_STDY_SEL,MPMC_STDY_RAMCS2_SEL))

/*-------------------------------------------------------------------------*/
//      Bit [1] MPMC_STDY_RAMCS3_SEL, select type of memory on CS3
#define EBI1_MPMC_STDY_SEL_WB__MPMC_STDY_RAMCS3_SEL_STATIC   (0U <<  \
        HWIO_SHFT(EBI1_MPMC_STDY_SEL,MPMC_STDY_RAMCS3_SEL))

#define EBI1_MPMC_STDY_SEL_WB__MPMC_STDY_RAMCS3_SEL_DYNAMIC  (1U <<  \
        HWIO_SHFT(EBI1_MPMC_STDY_SEL,MPMC_STDY_RAMCS3_SEL))


/*---------------------------------------------------------------------------
   EBI1_PSRAM_CRE
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] PSRAM_CRE, controls CRE pin for supporting synchronous Micron PSRAM
#define EBI1_PSRAM_CRE_WB__PSRAM_CRE_OUTPUT_0                (0U <<  \
        HWIO_SHFT(EBI1_PSRAM_CRE,PSRAM_CRE))

#define EBI1_PSRAM_CRE_WB__PSRAM_CRE_OUTPUT_1                (1U <<  \
        HWIO_SHFT(EBI1_PSRAM_CRE,PSRAM_CRE))


/*---------------------------------------------------------------------------
   EBI2_CFG
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] EBI2_PRIORITY, sets priority between ARM and DSP to XMEMC
#define EBI2_CFG_WB__EBI2_PRIORITY_ARM_HIGH                (0U <<  \
        HWIO_SHFT(EBI2_CFG,EBI2_PRIORITY))

#define EBI2_CFG_WB__EBI2_PRIORITY_DSP_HIGH                (1U <<  \
        HWIO_SHFT(EBI2_CFG,EBI2_PRIORITY))

/*-------------------------------------------------------------------------*/
//      Bit [1] NAND_BUS_EN, enables NAND controller to drive NAND Chip select
#define EBI2_CFG_WB__NAND_BUS_EN_DIS                      (0U <<  \
        HWIO_SHFT(EBI2_CFG,NAND_BUS_EN))

#define EBI2_CFG_WB__NAND_BUS_EN_ENA                      (1U <<  \
        HWIO_SHFT(EBI2_CFG,NAND_BUS_EN))


/*---------------------------------------------------------------------------
   MODEM_RAM_SELECT
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [8] SRCH4_RAM_CFG, select which RAM Srch4 uses
#define MODEM_RAM_SELECT_WH__SRCH4_RAM_CFG_DEINT_RAM      (0U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,SRCH4_RAM_CFG))

#define MODEM_RAM_SELECT_WH__SRCH4_RAM_CFG_SAMPLE_RAM     (1U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,SRCH4_RAM_CFG))

/*-------------------------------------------------------------------------*/
//      Bit [5] DEC_BOOT_RAM_EN, enables 0xFFFF0000 ram for boot from NAND
#define MODEM_RAM_SELECT_WH__DEC_BOOT_RAM_EN_DIS          (0U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,DEC_BOOT_RAM_EN))

#define MODEM_RAM_SELECT_WH__DEC_BOOT_RAM_EN_ENA          (1U <<  \
        HWIO_SHFT(MODEM_RAM_SELECT,DEC_BOOT_RAM_EN))




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
//                GRP (GRaphics Processor) CONFIGURATION
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/*---------------------------------------------------------------------------
   GRP_CONFIG0
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] BPP, sets the bits per pixel value for the GRP
#define GRP_CONFIG0_WW__BPP_8BPP                         (0U <<  \
        HWIO_SHFT(GRP_CONFIG0,BPP))

#define GRP_CONFIG0_WW__BPP_16BPP                        (1U <<  \
        HWIO_SHFT(GRP_CONFIG0,BPP))

/*---------------------------------------------------------------------------
   GRP_INTR_MASK
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] CIT_EN, enables interrupts for Command Item Termination
#define GRP_INTR_MASK_WW__CIT_EN_DIS                   (0U <<  \
        HWIO_SHFT(GRP_INTR_MASK,CIT_EN))

#define GRP_INTR_MASK_WW__CIT_EN_ENA                   (1U <<  \
        HWIO_SHFT(GRP_INTR_MASK,CIT_EN))

/*-------------------------------------------------------------------------*/
//      Bit [1] CQA_EN, enables interrupts for command queue available
#define GRP_INTR_MASK_WW__CQA_EN_DIS                   (0U <<  \
        HWIO_SHFT(GRP_INTR_MASK,CQA_EN))

#define GRP_INTR_MASK_WW__CQA_EN_ENA                   (1U <<  \
        HWIO_SHFT(GRP_INTR_MASK,CQA_EN))

/*-------------------------------------------------------------------------*/
//      Bit [2] MBERROR_EN, enables interrupts for AHB Master bus errors
#define GRP_INTR_MASK_WW__MBERROR_EN_DIS                   (0U <<  \
        HWIO_SHFT(GRP_INTR_MASK,MBERROR_EN))

#define GRP_INTR_MASK_WW__MBERROR_EN_ENA                   (1U <<  \
        HWIO_SHFT(GRP_INTR_MASK,MBERROR_EN))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
//                DLOAD CODE BITS DEFINITIONS
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/*      Automatic RFR control         */
#define UART_MR1_RFRC               (0x01 << \
        HWIO_SHFT(UART1DM_MR1,RX_RDY_CTL))

/*      Automatic CTS control         */
#define UART_MR1_CTSC               (0x01 << \
        HWIO_SHFT(UART1DM_MR1,CTS_CTL))

/*      8 bits per character          */
#define UART_MR2_8BPC               (0x03 << \
        HWIO_SHFT(UART1DM_MR2,BITS_PER_CHAR))

/*      1 stop bit                    */
#define UART_MR2_1SB                (0x01 << \
        HWIO_SHFT(UART1DM_MR2,STOP_BIT_LEN))

/*      Hunt character received       */
#define UART_SR_HUNT_RXD            (0x01 << \
        HWIO_SHFT(UART1DM_SR,HUNT_CHAR))

/*      Break received                */
#define UART_SR_BREAK_RXD           (0x01 << \
        HWIO_SHFT(UART1DM_SR,RX_BREAK))

/*      Parity or Framing error       */
#define UART_SR_PF_ERR              (0x01 << \
        HWIO_SHFT(UART1DM_SR,PAR_FRAME_ERR))

/*      Overrun error                 */
#define UART_SR_OVR_ERR             (0x01 << \
        HWIO_SHFT(UART1DM_SR,UART_OVERRUN))

/*      Transmitter empty             */
#define UART_SR_TXEMT               (0x01 << \
        HWIO_SHFT(UART1DM_SR,TXEMT))

/*      Transmitter ready             */
#define UART_SR_TXRDY               (0x01 << \
        HWIO_SHFT(UART1DM_SR,TXRDY))

/*      Receiver full                 */
#define UART_SR_RXFULL              (0x01 << \
        HWIO_SHFT(UART1DM_SR,RXFULL))

/*      Receiver ready                */
#define UART_SR_RXRDY               (0x01 << \
        HWIO_SHFT(UART1DM_SR,RXRDY))

/*      Reset error status            */
#define UART_CR_RESET_ERR           (0x03 << \
        HWIO_SHFT(UART1DM_CR,CHANNEL_COMMAND))

/*      Reset transmitter             */
#define UART_CR_RESET_TX            (0x02 << \
        HWIO_SHFT(UART1DM_CR,CHANNEL_COMMAND))

/*      Reset receiver                */
#define UART_CR_RESET_RX            (0x01 << \
        HWIO_SHFT(UART1DM_CR,CHANNEL_COMMAND))

/*      Enable receiver               */
#define UART_CR_RX_ENA              (0x01 << \
        HWIO_SHFT(UART1DM_CR,UART_RX_EN))

/*      Enable transmitter            */
#define UART_CR_TX_ENA              (0x01 << \
        HWIO_SHFT(UART1DM_CR,UART_TX_EN))

/*      Tx FIFO at or below the mark  */
#define UART_ISR_TXLEV              (0x01 << \
        HWIO_SHFT(UART_ISR,TXLEV))

/* For UART IRDA Register */
#define MSMU_IRDA_DISABLE           0x00  /* Disable UART IRDA transceiver */


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
//               ENCODER CODE BITS DEFINITIONS
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/*---------------------------------------------------------------------------
                          Write Registers
----------------------------------------------------------------------------*/

#define ENC_U_PN_M                  HWIO_FMSK(U_PN_STATE_5,U_PN_STATE)
  /*    Mask to get/set bozo bit ENC_U_PN_S5 */
#define ENC_BOZO_M                  HWIO_FMSK(U_PN_STATE_5,BOZO)

#define ENC_BOZO_V                  (0x01 << \
        HWIO_SHFT(U_PN_STATE_5,BOZO))

#define ENC_NO_BOZO_V               (0x00 << \
        HWIO_SHFT(U_PN_STATE_5,BOZO))

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*      data rate mask */
#define ENC_FCH_DATA_RATE_M         HWIO_FMSK(MOD_CH2_ENC_CTL_0,FCH_ENC_RATE)

#define ENC_FCH_CODE_RATE_M         HWIO_FMSK(MOD_CH2_ENC_CTL_0,FCH_CODE_RATE)

#ifdef T_MSM_5100
   #define ENC_FCH_RADIO_CONFIG_M      0x000C        /* radio config mask */

   #define ENC_FCH_FRAME_SIZE_M        0x0003        /* frame size mask */
   #define ENC_FCH_FRAME_SIZE_V        0x0002        /* frame size value */

   #define ENC_FCH_ENCODE_TIME_M       0x0003        /* encoding time mask */
   #define ENC_FCH_ENCODE_TIME_V       0x0003        /* encoding time value */
   #define ENC_DCCH_ENCODE_TIME_V      0x0002

   #define ENC_FCH_PCG_COUNT_M         0x0004        /* pcg count mask */
   #define ENC_FCH_PCG_COUNT_V         0x0000        /* pcg count value */

   #define ENC_DCCH_GAIN_M             0xFF00
#endif /* T_MSM_5100 */

#define ENC_PCH_GAIN_M              HWIO_FMSK(MOD_PCH_GAIN,PCH_GAIN)
#define ENC_SCH_GAIN_M              HWIO_FMSK(MOD_SCH_FCH_GAIN,SCH_GAIN)
#define ENC_FCH_GAIN_M              HWIO_FMSK(MOD_SCH_FCH_GAIN,FCH_GAIN)

#define ENC_TX_VERY_EARLY_FRAME_ADV_M    HWIO_FMSK(TX_VERY_EARLY_FRAME_CTL,VERY_EARLY_FRAME_ADV)
#define ENC_TX_VERY_EARLY_FRAME_PRIOD_M  HWIO_FMSK(TX_VERY_EARLY_FRAME_CTL,VERY_EARLY_FRAME_PER)

#define ENC_IS95_MODE_M             HWIO_FMSK(MOD_CH2_ENC_CTL_0,IS_95_C)
#define ENC_IS95_C_V                (0x01 << HWIO_SHFT(MOD_CH2_ENC_CTL_0,IS_95_C))

#define ENC_IS95_A_V                (0x00 << HWIO_SHFT(MOD_CH2_ENC_CTL_0,IS_95_C))

#define ENC_IS95_FCH_ON_M           HWIO_FMSK(MOD_CH2_ENC_CTL_0,FCH_EN)
#define ENC_IS95C_FCH_ON_V          (0x01 << HWIO_SHFT(MOD_CH2_ENC_CTL_0,FCH_EN))

#define ENC_IS95C_FCH_OFF_V         (0x00 << HWIO_SHFT(MOD_CH2_ENC_CTL_0,FCH_EN))


#define ENC_IS95_SCH_ON_M           HWIO_FMSK(MOD_CH3_ENC_CTL_0,SCH_EN)
#define ENC_IS95C_SCH_ON_V          (0x01 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,SCH_EN))

#define ENC_IS95C_SCH_OFF_V         (0x00 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,SCH_EN))


#define ENC_WALSHCOVER_SEL_M        0x0FFF
#define ENC_WALSHCOVER_SEL_SCH_M    0x000F
#define ENC_WALSHCOVER_SEL_SCH_1_V  0x0001
#define ENC_WALSHCOVER_SEL_SCH_2_V  0x0002
#define ENC_WALSHCOVER_SEL_FCH_M    0x00F0
#define ENC_WALSHCOVER_SEL_FCH_V    0x0040
#define ENC_WALSHCOVER_SEL_DCCH_M   0x0F00
#define ENC_WALSHCOVER_SEL_DCCH_V   0x0800

#define ENC_SCH_ENCODE_TIME_M       0x0003
#define ENC_SCH_ENCODE_TIME_V       0x0000

#define ENC_SCH_PCG_COUNT_M         0x0004
#define ENC_SCH_PCG_COUNT_V         0x0000

#define ENC_SCH_DATA_RATE_M         0x001E
#define ENC_SCH_RATE_SET_M          0x0020

#define ENC_SCH_FRAME_SIZE_M        0x0003
#define ENC_SCH_FRAME_SIZE_5_V      0x0000
#define ENC_SCH_FRAME_SIZE_10_V     0x0001
#define ENC_SCH_FRAME_SIZE_20_V     0x0002

#define ENC_SCH_SELECT_M            0x000C
#define ENC_SCH_SELECT_NONE_V       0x0000
#define ENC_SCH_SELECT_SCH_V        0x0004
#define ENC_SCH_SELECT_CCCH_V       0x0008
#define ENC_SCH_SELECT_EACH_V       0x000C

#define ENC_SCH_INTLV_SIZE_M        0x00E0
#define ENC_SCH_INTLV_SIZE_1X_V     0x0040
#define ENC_SCH_INTLV_SIZE_2X_V     0x0040
#define ENC_SCH_INTLV_SIZE_4X_V     0x0060
#define ENC_SCH_INTLV_SIZE_8X_V     0x0080
#define ENC_SCH_INTLV_SIZE_16X_V    0x00A0

#define ENC_SCH_LOW_REPEAT_M        0x0E00
#define ENC_SCH_LOW_REPEAT_1_V      0x0000
#define ENC_SCH_LOW_REPEAT_2_V      0x0200
#define ENC_SCH_LOW_REPEAT_4_V      0x0400
#define ENC_SCH_LOW_REPEAT_8_V      0x0600
#define ENC_SCH_LOW_REPEAT_16_V     0x0800
#define ENC_SCH_LOW_REPEAT_32_V     0x0A00

#define ENC_SCH_MULTIFRAME_M        0x3000
#define ENC_SCH_MULTIFRAME_1_V      0x0000
#define ENC_SCH_MULTIFRAME_2_V      0x1000
#define ENC_SCH_MULTIFRAME_4_V      0x2000

#define ENC_SCH_CODE_RATE_M         HWIO_FMSK(MOD_CH3_ENC_CTL_0,SCH_CODE_RATE)

#ifdef T_MSM6700
#define ENC_PICH_M                   HWIO_FMSK(MOD_MISC_CTL,CDMA2000_PICH_EN)
#define ENC_PICH_ON_V                (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PICH_EN))

#define ENC_PICH_OFF_V               (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PICH_EN))
#else
#define ENC_PCH_M                   HWIO_FMSK(MOD_MISC_CTL,CDMA2000_PCH_EN)
#define ENC_PCH_ON_V                (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PCH_EN))

#define ENC_PCH_OFF_V               (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PCH_EN))
#endif


#define ENC_NO_MOD_ROTATION_V       0x0000
#define ENC_FCH_CTL_DIS_V           0x0000
#define ENC_SCH_CTL_DIS_V           0x0000
#define ENC_ZERO_GAIN_V             0x0000
#define ENC_CLEAR_CRC_POLY_V        0x0000
#define ENC_SCH_LTU_SIZE_ZERO_V     0x0000

#define ENC_PUNC_PATTERN_NONE_V     0xFFFF

#define ENC_TURBO_CODE_M            HWIO_FMSK(MOD_CH3_ENC_CTL_0,TURBO_ENCODE)
#define ENC_TURBO_CODE_V            (0x01 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,TURBO_ENCODE))

#define ENC_CONV_CODE_V             (0x00 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,TURBO_ENCODE))

#define ENC_LTU_ENA_M               HWIO_FMSK(MOD_CH3_ENC_CTL_0,SCH_LTU_EN)
#define ENC_LTU_ENA_V               (0x01 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,SCH_LTU_EN))

#define ENC_LTU_DIS_V               (0x00 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,SCH_LTU_EN))

#define ENC_SCH_RC4_TURBO_PUNCT_0_PATT    0xA000
#define ENC_SCH_RC4_TURBO_PUNCT_1_PATT    0x00DD

/*      CRC Length; 0 means disable */
#define ENC_DCCH_CRCLEN_M         0x3E00  /* CRC Length; 0 means disable */

#define ENC_FCH_CRCLEN_M          0x1F00  /* CRC Length; 0 means disable */

/*      CRC Length; 0 means disable */
#define ENC_SCH_CRCLEN_M            HWIO_FMSK(MOD_CH3_ENC_CTL_0,SCH_CRC_LENGTH)

#define ENC_CRCDIS_V                0x00    /* Don't Generate and insert CRC */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*      Codec Control */
#define ENC_CODEC_CTL_M             HWIO_FMSK(MOD_CLK_CTL,CODEC_CTL)
/*      CODEC_CLK and CODEC_SYNC low    */

#define ENC_CODEC_LO_V              (0x00 << \
        HWIO_SHFT(MOD_CLK_CTL,CODEC_CTL))

/*      CODEC_CLK low, CODEC_SYNC hi    */
#define ENC_CODEC_LOHI_V            (0x01 << \
        HWIO_SHFT(MOD_CLK_CTL,CODEC_CTL))

/*      CODEC_CLK hi, CODEC_SYNC low    */
#define ENC_CODEC_HILO_V            (0x02 << \
        HWIO_SHFT(MOD_CLK_CTL,CODEC_CTL))

/*      CODEC_CLK and CODEC_SYNC high   */
#define ENC_CODEC_HI_V              (0x03 << \
        HWIO_SHFT(MOD_CLK_CTL,CODEC_CTL))

/*      CODEC_CLK and CODEC_SYNC normal */
#define ENC_CODEC_NORM_V            (0x04 << \
        HWIO_SHFT(MOD_CLK_CTL,CODEC_CTL))

/*      Vocoder frame reference disable - if set, SET VOC_FR_REF, VOC_CS_B,
        VOC_WR_B, VOC_OE_B low */
#define ENC_VOC_REF_DIS_M           HWIO_FMSK(MOD_CLK_CTL,VOC_REF_DISABLE)

#define ENC_VOC_REF_DIS_V           (0x01 << \
        HWIO_SHFT(MOD_CLK_CTL,VOC_REF_DISABLE))

#define ENC_VOC_REF_ENA_V           (0x00 << \
        HWIO_SHFT(MOD_CLK_CTL,VOC_REF_DISABLE))

/*      Transmit clocks disable     */
#define ENC_TX_CLKS_DIS_M           HWIO_FMSK(MOD_CLK_CTL,TX_CLKS_DISABLE)

/*      Transmit clocks disable on  */
#define ENC_TX_CLKS_DIS_V           (0x01 << \
        HWIO_SHFT(MOD_CLK_CTL,TX_CLKS_DISABLE))

/*      Transmit clocks disable off */
#define ENC_TX_CLKS_ENA_V           (0x00 << \
        HWIO_SHFT(MOD_CLK_CTL,TX_CLKS_DISABLE))

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*      Power amplifier control    */
#define ENC_TXP_CTL_M               HWIO_FMSK(MOD_MISC_CTL,PA_CTL)

/*      TX_PUNCT off (low)         */
#define ENC_TXP_OFF_V               (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,PA_CTL))

/*      TX_PUNCT follows MASK_DATA */
#define ENC_TXP_MASK_V              (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,PA_CTL))

/*      TX_PUNCT on (high)         */
#define ENC_TXP_ON_V                (0x02 << \
        HWIO_SHFT(MOD_MISC_CTL,PA_CTL))


/*      Access channel transmitted */
#define ENC_ACCESS_M                HWIO_FMSK(MOD_MISC_CTL,IS95A_ACCESS_CH)

/*      yes */
#define ENC_ACCESS_V                (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,IS95A_ACCESS_CH))

/*      no  */
#define ENC_NO_ACCESS_V             (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,IS95A_ACCESS_CH))

#ifdef T_MSM6700
/*      IS95 C Pilot transmitted */
#define ENC_IS95C_PILOT_M           HWIO_FMSK(MOD_MISC_CTL,CDMA2000_PICH_EN)

/*      yes */
#define ENC_IS95C_PILOT_V           (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PICH_EN))

/*      no  */
#define ENC_NO_IS95C_PILOT_V        (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PICH_EN))
#else
/*      IS95 C Pilot transmitted */
#define ENC_IS95C_PILOT_M           HWIO_FMSK(MOD_MISC_CTL,CDMA2000_PCH_EN)

/*      yes */
#define ENC_IS95C_PILOT_V           (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PCH_EN))

/*      no  */
#define ENC_NO_IS95C_PILOT_V        (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PCH_EN))
#endif


#define ENC_ZERO_PCBIT_V            0x0000

#ifdef T_MSM6700
/*      TX in offset-bin or 2'c comp */
#define ENC_TX_FMT_M                HWIO_FMSK(MOD_MISC_CTL,RESERVED_BITS6)
/*      TX in 2's comp               */
#define ENC_TX_2S_V                 (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,RESERVED_BITS6))
#define ENC_TX_OFF_V                (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,RESERVED_BITS6))
#else
/*      TX in offset-bin or 2'c comp */
#define ENC_TX_FMT_M                HWIO_FMSK(MOD_MISC_CTL,TX_DATA_FORMAT)
/*      TX in 2's comp               */
#define ENC_TX_2S_V                 (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,TX_DATA_FORMAT))
		/*      TX offset-binary             */
#define ENC_TX_OFF_V                (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,TX_DATA_FORMAT))
#endif


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                             CORDIC ROTATOR                              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Enable the Cordic Rotator          */
#define ENC_SET_CORDIC_EN_V                (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,CORDIC_EN))

/* Disable the Cordic Rotator         */
#define ENC_NO_SET_CORDIC_EN_V             (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,CORDIC_EN))

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                             GATING RATE                                 */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* latch the gating rate bits every 5ms */
#define ENC_PCH_GRATE_5MS_LAT_V                (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,PCH_GATE_5MS_LAT))

/* latch the gating rate bits every 20ms */
#define ENC_PCH_GRATE_20MS_LAT_V                (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,PCH_GATE_5MS_LAT))


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                             TEST REGISTERS                              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*      Set IQ Phase Waveform         */
#define ENC_SET_IQ_M                HWIO_FMSK(MOD_TEST_CTL,SET_IQ_HIGH)

/*      I & Q high after spreading    */
#define ENC_SET_IQ_V                (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,SET_IQ_HIGH))

/*      Don't force I & Q             */
#define ENC_NO_SET_IQ_V             (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,SET_IQ_HIGH))

/*      Internal Master Mask Control  */
#define ENC_MASK_CTL_M              HWIO_FMSK(MOD_TEST_CTL,MASK_CTL)

/*      Set MASK low                  */
#define ENC_MASK_LO_V               (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,MASK_CTL))

/*      MASK functions normally       */
#define ENC_MASK_V                  (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,MASK_CTL))

/*      Set MASK high                 */
#define ENC_MASK_HI_V               (0x03 << \
        HWIO_SHFT(MOD_TEST_CTL,MASK_CTL))

/*      Transmit data control         */
#define ENC_TX_DAT_CTL_M            HWIO_FMSK(MOD_TEST_CTL,TX_DATA_CTL)

/*      source = FIR filter           */
#define ENC_TX_FIR_V                (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,TX_DATA_CTL))

/*      source = TX_DATA_TEST         */
#define ENC_TX_TEST_V               (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,TX_DATA_CTL))

/*      Rx spectral inversion         */
#define ENC_RX_POL_M                HWIO_FMSK(RXF_SRCc_CTL,SPECTRAL_INVERSION)

/*      Invert                        */
/* //DA need to confirm this is correct... */
#define ENC_RX_INV_V                (0x01 << \
        HWIO_SHFT(RXF_SRCc_CTL,SPECTRAL_INVERSION))

/*      Normal                        */
#define ENC_RX_NORM_V               (0x00 << \
        HWIO_SHFT(RXF_SRCc_CTL,SPECTRAL_INVERSION))

/*      Tx spectral inversion         */
#define ENC_TX_POL_M                HWIO_FMSK(MOD_TEST_CTL,TX_SPECTRAL_INVERSION)

/*      Invert                        */
#define ENC_TX_INV_V                (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,TX_SPECTRAL_INVERSION))

/*      Normal                        */
#define ENC_TX_NORM_V               (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,TX_SPECTRAL_INVERSION))

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define ENC_STMR_CMD_M                (HWIO_RMSK(MOD_STMR_CMD))
#define ENC_STMR_CMD_LC_STATE_NOP_V   (0x00 << \
        HWIO_SHFT(MOD_STMR_CMD, TX_LC_STATE_LD))
#define ENC_STMR_CMD_LC_STATE_80MS_V  (0x01 << \
        HWIO_SHFT(MOD_STMR_CMD, TX_LC_STATE_LD))
#define ENC_STMR_CMD_LC_MASK_NOP_V    (0x00 << \
        HWIO_SHFT(MOD_STMR_CMD, TX_MASK_LD))
#define ENC_STMR_CMD_LC_MASK_IMMED_V  (0x01 << \
        HWIO_SHFT(MOD_STMR_CMD, TX_MASK_LD))
#define ENC_STMR_CMD_LC_MASK_20MS_V   (0x02 << \
        HWIO_SHFT(MOD_STMR_CMD, TX_MASK_LD))

#ifdef T_MSM6700
#define ENC_PICH_GATING_RATE_M         0x0600
#define ENC_PICH_GATING_RATE_ALL_V     0x0000
#define ENC_PICH_GATING_RATE_HALF_V    0x0200
#define ENC_PICH_GATING_RATE_QUARTER_V 0x0400
#else
#define ENC_PCH_GATING_RATE_M         0x0600
#define ENC_PCH_GATING_RATE_ALL_V     0x0000
#define ENC_PCH_GATING_RATE_HALF_V    0x0200
#define ENC_PCH_GATING_RATE_QUARTER_V 0x0400
#endif

#define ENC_FCH_EIGHTH_GATE_M         0x0800
#define ENC_FCH_EIGHTH_GATE_ENA_V     0x0800
#define ENC_FCH_EIGHTH_GATE_DIS_V     0x0000

#define ENC_RETRANS_M                 0x1000
#define ENC_RETRANS_ENA_V             0x1000
#define ENC_RETRANS_DIS_V             0x0000

#define ENC_IS95A_MODE_M              0x2000
#define ENC_IS95A_MODE_ENA_V          0x2000
#define ENC_IS95A_MODE_DIS_V          0x0000

#define ENC_DCCH_EN_M                 0x0001
#define ENC_DCCH_EN_V                 0x0001
#define ENC_DCCH_DIS_V                0x0000

/* AGC_CTL8 MASK_DELAY bit definitions */
#define AGC_CTL_MASK_DELAY_1PCG 0x00 
#define AGC_CTL_MASK_DELAY_2PCG 0x01 
#define AGC_CTL_MASK_DELAY_3PCG 0x02 
#define AGC_CTL_MASK_DELAY_4PCG 0x03 
#define AGC_CTL_MASK_DELAY_5PCG 0x04 
#define AGC_CTL_MASK_DELAY_6PCG 0x05 
#define AGC_CTL_MASK_DELAY_7PCG 0x06 
#define AGC_CTL_MASK_DELAY_8PCG 0x07 

#define AGC_CTL8_WB__MASK_DELAY_1PCG   AGC_CTL_MASK_DELAY_1PCG
#define AGC_CTL8_WB__MASK_DELAY_2PCG   AGC_CTL_MASK_DELAY_2PCG
#define AGC_CTL8_WB__MASK_DELAY_3PCG   AGC_CTL_MASK_DELAY_3PCG
#define AGC_CTL8_WB__MASK_DELAY_4PCG   AGC_CTL_MASK_DELAY_4PCG
#define AGC_CTL8_WB__MASK_DELAY_5PCG   AGC_CTL_MASK_DELAY_5PCG
#define AGC_CTL8_WB__MASK_DELAY_6PCG   AGC_CTL_MASK_DELAY_6PCG
#define AGC_CTL8_WB__MASK_DELAY_7PCG   AGC_CTL_MASK_DELAY_7PCG
#define AGC_CTL8_WB__MASK_DELAY_8PCG   AGC_CTL_MASK_DELAY_8PCG

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
//               HW CODE BITS DEFINITIONS
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/*---------------------------------------------------------------------------
   DEM_CTL
---------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
//      Bit [0] SYNC_BYPASS, reserved, bypass synchronizers between DEMCX8
//      & RXDSP_CLK
#define DEM_CTL_WB__SYNC_BYPASS_NORMAL                  (0 << \
        HWIO_SHFT(DEM_CTL,SYNC_BYPASS))

#define DEM_CTL_WB__SYNC_BYPASS_ENA                     (1 << \
        HWIO_SHFT(DEM_CTL,SYNC_BYPASS))

/*-------------------------------------------------------------------------*/
//      Bit [1] RESERVED

/*-------------------------------------------------------------------------*/
//      Bit [2] Q_CHAN_OFF, reserved, demod doesn't send Q ch soft decisions
//      to Viterbi decoder
#define DEM_CTL_WB__Q_CHAN_OFF_Q_CHAN_SENT              (0 << \
        HWIO_SHFT(DEM_CTL,Q_CHAN_OFF))

#define DEM_CTL_WB__Q_CHAN_OFF_Q_CHAN_NOT_SENT          (1 << \
        HWIO_SHFT(DEM_CTL,Q_CHAN_OFF))

/*-------------------------------------------------------------------------*/
//      Bit [3] MAC_EN_CH2, set to enable Channel 2 processing in the MAC
#define DEM_CTL_WB__MAC_EN_CH2_DIS                     (0 << \
        HWIO_SHFT(DEM_CTL,MAC_EN_CH2))

#define DEM_CTL_WB__MAC_EN_CH2_ENA                     (1 << \
        HWIO_SHFT(DEM_CTL,MAC_EN_CH2))

/*-------------------------------------------------------------------------*/
//      Bit [4] MAC_EN_CH1, set to enable Channel 1 processing in the MAC
#define DEM_CTL_WB__MAC_EN_CH1_DIS                     (0 << \
        HWIO_SHFT(DEM_CTL,MAC_EN_CH1))

#define DEM_CTL_WB__MAC_EN_CH1_ENA                     (1 << \
        HWIO_SHFT(DEM_CTL,MAC_EN_CH1))

/*-------------------------------------------------------------------------*/
//      Bit [7:6] REP_FACTOR, set the repetition factor
#define DEM_CTL_WB__REP_FACTOR_1                      (0 << \
        HWIO_SHFT(DEM_CTL,REP_FACTOR))

#define DEM_CTL_WB__REP_FACTOR_2                      (1 << \
        HWIO_SHFT(DEM_CTL,REP_FACTOR))

#define DEM_CTL_WB__REP_FACTOR_4                      (2 << \
        HWIO_SHFT(DEM_CTL,REP_FACTOR))

/*---------------------------------------------------------------------------
   DEM_CTL2
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] RESERVED

/*-------------------------------------------------------------------------*/
//      Bit [1] RC4, set this bit to indicate to hardware that channel 0 is
//      in RC4
#define DEM_CTL2_WH__RC4_DIS                         (0 << \
        HWIO_SHFT(DEM_CTL2,RC4))

#define DEM_CTL2_WH__RC4_ENA                         (1 << \
        HWIO_SHFT(DEM_CTL2,RC4))

/*-------------------------------------------------------------------------*/
//      Bits [7:2] RESERVED

/*-------------------------------------------------------------------------*/
//      Bits [11:8] CH0_TD_SIGN, timing pattern for channel 0, period of 4 PN words
#define DEM_CTL2_WH__CH0_TD_SIGN_RC3                 (5 << \
        HWIO_SHFT(DEM_CTL2,CH0_TD_SIGN))

#define DEM_CTL2_WH__CH0_TD_SIGN_RC4                 (3 << \
        HWIO_SHFT(DEM_CTL2,CH0_TD_SIGN))

#define DEM_CTL2_WH__CH0_TD_SIGN_OTHER_RC            (0 << \
        HWIO_SHFT(DEM_CTL2,CH0_TD_SIGN))

/*-------------------------------------------------------------------------*/
//      Bits [15:12] CH1_TD_SIGN, timing pattern for channel 1, period of 4 PN words
#define DEM_CTL2_WH__CH1_TD_SIGN_RC3                 (5 << \
        HWIO_SHFT(DEM_CTL2,CH1_TD_SIGN))

#define DEM_CTL2_WH__CH1_TD_SIGN_RC4                 (3 << \
        HWIO_SHFT(DEM_CTL2,CH1_TD_SIGN))

#define DEM_CTL2_WH__CH1_TD_SIGN_OTHER_RC            (0 << \
        HWIO_SHFT(DEM_CTL2,CH1_TD_SIGN))


/*---------------------------------------------------------------------------
   DEM_FRAME_OFF
---------------------------------------------------------------------------*/
//-----------------------------------------------------------------------------
/* MDR Supplemental Channel Frame Sync Control enable/disable */
#define DEM_FRAME_CTL_WB__SUP_SYNC_BYPASS_ENA           (1 << \
        HWIO_SHFT(DEM_FRAME_CTL,FRAME_SYNC_BYPASS))

#define DEM_FRAME_CTL_WB__SUP_SYNC_BYPASS_DIS           (0 << \
        HWIO_SHFT(DEM_FRAME_CTL,FRAME_SYNC_BYPASS))

/*---------------------------------------------------------------------------
   WEB_MISC_WR
---------------------------------------------------------------------------*/
#define  WEB_MISC_WR_WH__BT_SLEEP_STOP_CMD_ENA           (1 << \
        HWIO_SHFT(WEB_MISC_WR,BT_SLEEP_STOP_CMD))

#define  WEB_MISC_WR_WH__BT_SLEEP_STOP_CMD_DIS           (0 << \
        HWIO_SHFT(WEB_MISC_WR,BT_SLEEP_STOP_CMD))

#define  WEB_MISC_WR_WH__UART_NAND_SEL_ENA            (0 << \
        HWIO_SHFT(WEB_MISC_WR,UART_NAND_SEL))

#define  WEB_MISC_WR_WH__UART_NAND_SEL_DIS            (1 << \
        HWIO_SHFT(WEB_MISC_WR,UART_NAND_SEL))

#define  WEB_MISC_WR_WH__BT_MDDI_EN_ENA                  (1 << \
        HWIO_SHFT(WEB_MISC_WR,MDDI_EN))

#define  WEB_MISC_WR_WH__BT_MDDI_EN_DIS                  (0 << \
        HWIO_SHFT(WEB_MISC_WR,MDDI_EN))

#define  WEB_MISC_WR_WH__MMC_DATA_BI_SEL_SRC0            (0 << \
        HWIO_SHFT(WEB_MISC_WR,MMC_DATA_BI_SEL))

#define  WEB_MISC_WR_WH__MMC_DATA_BI_SEL_SRC1            (1 << \
        HWIO_SHFT(WEB_MISC_WR,MMC_DATA_BI_SEL))

#define  WEB_MISC_WR_WH__PCDAC_TEST_SEL_INTERNAL         (0 << \
        HWIO_SHFT(WEB_MISC_WR,PCDAC_TEST_SEL))

#define  WEB_MISC_WR_WH__PCDAC_TEST_SEL_PINS             (1 << \
        HWIO_SHFT(WEB_MISC_WR,PCDAC_TEST_SEL))

#define  WEB_MISC_WR_WH__TXDAC_TEST_SEL_INTERNAL         (0 << \
        HWIO_SHFT(WEB_MISC_WR,TXDAC_TEST_SEL))

#define  WEB_MISC_WR_WH__TXDAC_TEST_SEL_PINS             (1 << \
        HWIO_SHFT(WEB_MISC_WR,TXDAC_TEST_SEL))

#define  WEB_MISC_WR_WH__SBDT_KEEPER_DIS_DIS             (1 << \
        HWIO_SHFT(WEB_MISC_WR,SBDT_KEEPER_DIS))

#define  WEB_MISC_WR_WH__SBDT_KEEPER_DIS_ENA             (0 << \
        HWIO_SHFT(WEB_MISC_WR,SBDT_KEEPER_DIS))

#endif /*MSMBITS_H*/


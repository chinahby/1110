#ifndef MSM6100BITSP_H
#define MSM6100BITSP_H

/*===========================================================================
                                                         _
                                                       _/ \_
                  CDMA DIGITAL CELLULAR               / \_/ \
                                                      \_/ \_/
                                                      / \_/ \
          MOBILE STATION MODEM - MSM6100              \_/ \_/
                                                        \_/
  

  DESCRIPTION:
     This file define bits for all the registers in MSM6100.


     QUALCOMM PROPRIETARY
         Copyright (c) 2000, 2001 by Qualcomm Incorporated

===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

 $Header: //depot/asic/qsc1100/drivers/hw/msmbitsp.h#1 $ $DateTime: 2007/05/17 13:25:59 $ $Author: hvo $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 11/27/02   cr      Created from original msm6100bits.h
===========================================================================*/

#include "target.h"


/*===========================================================================

                          BIT DEFINITIONS

========================================================================== */

/*---------------------------------------------------------------------------
   SBI_CTL
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [5:0] SLAVE_ID, unique SBI slave ID, clear after initial reset
#define SBI_CTL_WW__SLAVE_ID_CLEAR                              (0U <<  \
        HWIO_SHFT(SBIc_CTL,SLAVE_ID))

/*-------------------------------------------------------------------------*/
//      Bit [7:6] I3Q_MODE, selects the I3Q mode control for a microprocessor
//      triggered SBI transaction
#define SBI_CTL_WW__I3Q_MODE_ITM                                (0U <<  \
        HWIO_SHFT(SBIc_CTL,I3Q_MODE))

#define SBI_CTL_WW__I3Q_MODE_FTM                                (1U <<  \
        HWIO_SHFT(SBIc_CTL,I3Q_MODE))

#define SBI_CTL_WW__I3Q_MODE_BTM                                (2U <<  \
        HWIO_SHFT(SBIc_CTL,I3Q_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [8] LAST_WORD, set (1) to tells the msbi controller to terminate
//      the transaction after this word
#define SBI_CTL_WW__LAST_WORD_TERM_ON_WR_BUF_EMPTY              (0U <<  \
        HWIO_SHFT(SBIc_CTL,LAST_WORD))

#define SBI_CTL_WW__LAST_WORD_TERM_ON_THIS_WORD                 (1U <<  \
        HWIO_SHFT(SBIc_CTL,LAST_WORD))

/*-------------------------------------------------------------------------*/
//      Bit [9] DIAG_MODE, reserved - provides snapshot of each
//      activity to the read buffer
#define SBI_CTL_WW__DIAG_MODE_DIS                               (0U <<  \
        HWIO_SHFT(SBIc_CTL,DIAG_MODE))

#define SBI_CTL_WW__DIAG_MODE_ENA                               (1U <<  \
        HWIO_SHFT(SBIc_CTL,DIAG_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [10] OVR_MODE, reserved - allows MSM to "bit program" the SBI
#define SBI_CTL_WW__OVR_MODE_DIS                                (0U <<  \
        HWIO_SHFT(SBIc_CTL,OVR_MODE))

#define SBI_CTL_WW__OVR_MODE_ENA                                (1U <<  \
        HWIO_SHFT(SBIc_CTL,OVR_MODE))

/*-------------------------------------------------------------------------*/
//      Bit [11] MSM_SBI_EN, gives SBI control of the ADC pins (always
//      enable)
#define SBI_CTL_WW__MSM_SBI_EN_DIS                              (0U <<  \
        HWIO_SHFT(SBIc_CTL,MSM_SBI_EN))

#define SBI_CTL_WW__MSM_SBI_EN_ENA                              (1U <<  \
        HWIO_SHFT(SBIc_CTL,MSM_SBI_EN))

/*-------------------------------------------------------------------------*/
//      Bit [12] I2C_I3Q_N, selects which type of device the MSM will access
#define SBI_CTL_WW__I2C_I3Q_N_SEL_I3Q                           (0U <<  \
        HWIO_SHFT(SBIc_CTL,I2C_I3Q_N))

#define SBI_CTL_WW__I2C_I3Q_N_SEL_I2C                           (1U <<  \
        HWIO_SHFT(SBIc_CTL,I2C_I3Q_N))

/*-------------------------------------------------------------------------*/
//      Bit [13] HW_REQ1_EN, enables the hw_req1 feature
#define SBI_CTL_WW__HW_REQ1_EN_DIS                              (0U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ1_EN))

#define SBI_CTL_WW__HW_REQ1_EN_ENA                              (1U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ1_EN))

/*-------------------------------------------------------------------------*/
//      Bits [15:14] HW_REQ1_PRIORITY, sets relative priority of HW request
//      port1
#define SBI_CTL_WW__HW_REQ1_PRIORITY_1ST                        (0U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ1_PRIORITY))

#define SBI_CTL_WW__HW_REQ1_PRIORITY_2ND                        (1U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ1_PRIORITY))

#define SBI_CTL_WW__HW_REQ1_PRIORITY_3RD                        (2U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ1_PRIORITY))

#define SBI_CTL_WW__HW_REQ1_PRIORITY_4TH                        (3U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ1_PRIORITY))

/*-------------------------------------------------------------------------*/
//      Bit [16] HW_REQ2_EN, enables the hw_req2 feature
#define SBI_CTL_WW__HW_REQ2_EN_DIS                              (0U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ2_EN))

#define SBI_CTL_WW__HW_REQ2_EN_ENA                              (1U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ2_EN))

/*-------------------------------------------------------------------------*/
//      Bits [18:17] HW_REQ2_PRIORITY, sets relative priority of HW request
//      port2
#define SBI_CTL_WW__HW_REQ2_PRIORITY_1ST                        (0U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ2_PRIORITY))

#define SBI_CTL_WW__HW_REQ2_PRIORITY_2ND                        (1U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ2_PRIORITY))

#define SBI_CTL_WW__HW_REQ2_PRIORITY_3RD                        (2U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ2_PRIORITY))

#define SBI_CTL_WW__HW_REQ2_PRIORITY_4TH                        (3U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ2_PRIORITY))

/*-------------------------------------------------------------------------*/
//      Bit [19] HW_REQ3_EN, enables the hw_req3 feature (interrupt transfer
//      mode only)
#define SBI_CTL_WW__HW_REQ3_EN_DIS                              (0U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ3_EN))

#define SBI_CTL_WW__HW_REQ3_EN_ENA                              (1U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ3_EN))

/*-------------------------------------------------------------------------*/
//      Bits [21:20] HW_REQ3_PRIORITY, sets relative priority of HW request
//      port3
#define SBI_CTL_WW__HW_REQ3_PRIORITY_1ST                        (0U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ3_PRIORITY))

#define SBI_CTL_WW__HW_REQ3_PRIORITY_2ND                        (1U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ3_PRIORITY))

#define SBI_CTL_WW__HW_REQ3_PRIORITY_3RD                        (2U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ3_PRIORITY))

#define SBI_CTL_WW__HW_REQ3_PRIORITY_4TH                        (3U <<  \
        HWIO_SHFT(SBIc_CTL,HW_REQ3_PRIORITY))


/*---------------------------------------------------------------------------
   SBI_CLK_CTL
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [5:0] CLKCTL, value to divide the SBI clock source by before
//      feeding it into the SBI block

/*-------------------------------------------------------------------------*/
//      Bit [6] CNT_EN, enables the source clock divider
#define SBI_CLK_CTL_WB__CNT_EN_DIS                              (0U <<  \
        HWIO_SHFT(SBIc_CLK_CTL,CNT_EN))

#define SBI_CLK_CTL_WB__CNT_EN_ENA                              (1U <<  \
        HWIO_SHFT(SBIc_CLK_CTL,CNT_EN))

/*-------------------------------------------------------------------------*/
//      Bit [7] MICRO_RESET, writing a 1 causes a state machine reset and a
//      clock divider refresh.  Writing a 0 causes only a clock divider
//      refresh.
#define SBI_CLK_CTL_WB__MICRO_RESET_REFRESH_ONLY                (0U <<  \
        HWIO_SHFT(SBIc_CLK_CTL,MICRO_RESET))

#define SBI_CLK_CTL_WB__MICRO_RESET_RST_STATE_MCHN_AND_REFRESH  (1U <<  \
        HWIO_SHFT(SBIc_CLK_CTL,MICRO_RESET))


/*---------------------------------------------------------------------------
   SBI_WR
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [15] RD_WRN, set (1) to indicate a read request, (0) to a write
//      request
#define SBI_WR_WH__RD_WR_N_WRITE                                 (0U <<  \
        HWIO_SHFT(SBIc_WR,RD_WR_N))

#define SBI_WR_WH__RD_WR_N_READ                                  (1U <<  \
        HWIO_SHFT(SBIc_WR,RD_WR_N))


/*---------------------------------------------------------------------------
   SBI_START_CTL
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [0] RD_WRN, set (1) to enable an SBI transaction
#define SBI_START_CTL_WB__START_FLAG_DIS                         (0U <<  \
        HWIO_SHFT(SBIc_START_CTL,START_FLAG))

#define SBI_START_CTL_WB__START_FLAG_ENA                         (1U <<  \
        HWIO_SHFT(SBIc_START_CTL,START_FLAG))


#endif /*MSM6100BITSP_H*/


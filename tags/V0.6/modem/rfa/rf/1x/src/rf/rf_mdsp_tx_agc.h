#ifndef RF_MDSP_TX_AGC_H
#define RF_MDSP_TX_AGC_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   T X    A G C    M D S P    M A C R O S


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000, 2001, 2002 by QUALCOMM, Inc.  All Rights Reserved.
  Copyright (c) 2003, 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
  Copyright (c) 2006, 2007       by QUALCOMM, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_mdsp_tx_agc.h#3 $
$DateTime: 2008/09/16 10:17:01 $
$Author: vmahajan $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -------------------------------------------------------
09/14/08   vm      Added the support for Tx DAC Backoff
08/06/08     vm      Added macros for reading PA switchpoints
03/28/08     vm      Added the support for Tx AGC interface with MDSP for QSC1100
07/12/07     vm      Changed the Variable names  fro HDR PDM Tables to be
                     same as 1X for Sc2x
07/06/07     adk     Made macros HDR_*_TX_PDM_* visible without having to
                     define the flag RF_HAS_BETA_SCALING_TX_AGC
07/03/07     adk     Added macros to set PDM values in the HDR mode.
06/25/07     adk     Minor modifications.
06/22/07     avi     Added macros to control BetaP control time update for 
                     BetaP mode
06/12/07     avi     Added macros to control SSBI action time update for 
                     BetaP mode
05/02/07     adk     Added macros to support Beta Scaling in HDR mode.
04/25/07     adk     Featurized macros based on CDMA1X_cTxBetapEnable.
04/18/07     adk     Added macros to enable/disable Beta Scaling.
04/11/07     zg      Added macros for gps intelliceiver handling.
03/28/07     adk     Updated comments for Beta Scaling TX AGC.
03/08/07     adk     Added macros for Beta Scaling TX AGC; updated comments.
02/08/07     adk     Minor fixes based on inspection.
10/15/06     vm      Created
==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "comdef.h"
#include "mdspext.h"
#include "rficap.h"


/*==========================================================================

                     MACRO DEFINITIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

#define DSP_READ(port)           (* (port) )
#define DSP_WRITE(port, val)     (* (port) ) = ((word) (val))
#define MDSP_READ32(port)        (*((volatile dword *) (port) ))
#define MDSP_WRITE32(port, val)  (*((volatile dword *) (port) ) = ((dword) (val)))

/* For MSM6500, the DSP interface file provides absolute DSP reg addresses */
#define MDSP_READ( reg )        DSP_READ( reg )
#define MDSP_WRITE( reg, val )  DSP_WRITE( (reg), (val) )

/*************************************************************************
 *                                                                       *
 *                        DSP Status Macros                              *
 *                                                                       *
 *************************************************************************/

/*=======================================================================
 MACRO          :  CDMA_MDSP_ENABLE_TX_FOR_1X
 
 DESCRIPTION    : This macro enables the FW to know that
                  1X Owns the transmitter. Should be set
                  to -1 whenever 1X owns transmitter in MDSP

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_ENABLE_TX_FOR_1X()   \
    MDSP_WRITE( CDMA1X_cTxInit, 0xFFFF );   \
    MDSP_WRITE( CDMA1X_cTxEnable, 0xFFFF )

/*=======================================================================
 MACRO          :  CDMA_MDSP_DISABLE_TX_FOR_1X
 
 DESCRIPTION    : This macro enables the FW to know that
                  1X  no longer Owns the transmitter. Should be set
                  to 0 whenever 1X is not owning the transmitter in MDSP.

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_DISABLE_TX_FOR_1X()   MDSP_WRITE( CDMA1X_cTxEnable, 0)

/*=======================================================================
 MACRO          : CDMA_MDSP_ENABLE_TX_OPEN_LOOP_OVERRIDE

 DESCRIPTION    : This macro freezes the TX Open loop Accumulator
                  and permit writing directly to it through 
                  cTxOpenLoopWr

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_ENABLE_TX_OPEN_LOOP_OVERRIDE()   MDSP_WRITE( CDMA1X_cTxOpenLoopOverride, 0xFFFF )

 /*=======================================================================
 MACRO          : CDMA_MDSP_SET_TX_OPEN_LOOP_NORMAL

 DESCRIPTION    : This macro permits the normal operation for TX
                  Open Loop Accumulator

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_TX_OPEN_LOOP_NORMAL()   MDSP_WRITE( CDMA1X_cTxOpenLoopOverride, 0x0 )

 /*=======================================================================
 MACRO          : CDMA_MDSP_WR_TX_OPEN_LOOP_VAL

 DESCRIPTION    : This macro writes to the TX Open Loop Accumulator
                  if cTxOpenLoopOverride is set to 0xFFFF;

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_WR_TX_OPEN_LOOP_VAL( val )   MDSP_WRITE( CDMA1X_cTxOpenLoopWr, val )
 

 /*=======================================================================
 MACRO          : CDMA_MDSP_RD_TX_OPEN_LOOP_VAL

 DESCRIPTION    : This macro Reads the TX Open Loop Accumulator

 Return Value   : Value in the TX Open loop Accumulator

 =========================================================================*/
 #define CDMA_MDSP_RD_TX_OPEN_LOOP_VAL()   MDSP_READ( CDMA1X_cTxOpenLoopVal )

 /*=======================================================================
 MACRO          : CDMA_MDSP_WR_TX_RATE_ADJ

 DESCRIPTION    : This macro writes programs the value for TX Rate ADJ

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_WR_TX_RATE_ADJ( val )   MDSP_WRITE( CDMA1X_cTxRateAdjWr, val )

 /*=======================================================================
 MACRO          : CDMA_MDSP_RD_TX_RATE_ADJ

 DESCRIPTION    : This macro reads the value for TX Rate ADJ

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_RD_TX_RATE_ADJ()   MDSP_READ( CDMA1X_cTxRateAdjVal )


 /*=======================================================================
 MACRO          : CDMA_MDSP_WR_TX_GAIN_ADJ_STEP_SIZE

 DESCRIPTION    : This macro programs the closed loop TX_GAIN_ADJ Step Size

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_WR_TX_GAIN_ADJ_STEP_SIZE( val )   MDSP_WRITE( CDMA1X_cTxGainAdjStepSize, val )

 /*=======================================================================
 MACRO          : CDMA_MDSP_ENABLE_TX_CLOSED_LOOP_OVERRIDE

 DESCRIPTION    : This macro freezes the TX Closed loop Accumulator
                  and permit writing directly to it through 
                  cTxOpenLoopWr

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_ENABLE_TX_CLOSED_LOOP_OVERRIDE()   MDSP_WRITE( CDMA1X_cTxGainAdjOverride, 0xFFFF )

 /*=======================================================================
 MACRO          : CDMA_MDSP_SET_TX_CLOSED_LOOP_NORMAL

 DESCRIPTION    : This macro freezes the TX Closed loop Accumulator
                  and permit writing directly to it through 
                  cTxClosedLoopAccumWr

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_TX_CLOSED_LOOP_NORMAL()   MDSP_WRITE( CDMA1X_cTxGainAdjOverride , 0x0 )

/*=======================================================================
 MACRO          :   CDMA_MDSP_GET_TX_CLOSED_LOOP_STATUS

 DESCRIPTION    :  This Macro determines whether closed loop power
                   control is enabled or not

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_GET_TX_CLOSED_LOOP_STATUS()   MDSP_READ( CDMA1X_cTxGainAdjOverride )
 
/*=======================================================================
 MACRO          : CDMA_MDSP_WR_TX_CLOSED_LOOP_VAL

 DESCRIPTION    : This macro writes to the TX Closed Loop Accumulator
                  if cTxOpenLoopOverride is set to 0xFFFF;

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_WR_TX_CLOSED_LOOP_VAL( val )   MDSP_WRITE( CDMA1X_cTxGainAdjWr, val )

 /*=======================================================================
 MACRO          : CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL

 DESCRIPTION    : This macro Reads the TX Closed Loop Accumulator

 Return Value   : Value in the TX Open loop Accumulator

 =========================================================================*/
 #define CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL()   MDSP_READ( CDMA1X_cTxGainAdjVal )

 /*=======================================================================
 MACRO          : CDMA_MDSP_TX_CAGC_UP_RATCHET_MODE_NORMAL

 DESCRIPTION    : This macro enables the TX cagc ratchet mode for up decision on
                  Tx Power

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_TX_CAGC_UP_RATCHET_MODE_NORMAL()   \
              MDSP_WRITE( CDMA1X_cRpcRatchetForceUp, 0 )

 /*=======================================================================
 MACRO          :  CDMA_MDSP_TX_CAGC_UP_RATCHET_MODE_FORCED

 DESCRIPTION    : This macro disables the TX cagc ratchet mode operations

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_TX_CAGC_UP_RATCHET_MODE_FORCED()   \
           MDSP_WRITE( CDMA1X_cRpcRatchetForceUp, 0xFFFF)

 /*=======================================================================
 MACRO          : CDMA_MDSP_TX_CAGC_DOWN_RATCHET_MODE_NORMAL

 DESCRIPTION    : This macro enables the TX cagc ratchet mode for down/attenuation 
                  decision on Tx Power

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_TX_CAGC_DOWN_RATCHET_MODE_NORMAL()    \
           MDSP_WRITE( CDMA1X_cRpcRatchetForceDown, 0) 

 /*=======================================================================
 MACRO          :  CDMA_MDSP_TX_CAGC_DOWN_RATCHET_MODE_FORCED

 DESCRIPTION    : This macro disables the TX cagc ratchet mode for down/attenuation 
                  decision on Tx Power operations

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_TX_CAGC_DOWN_RATCHET_MODE_FORCED()   \
               MDSP_WRITE( CDMA1X_cRpcRatchetForceDown, 0xFFFF ) 

 /*=======================================================================
 MACRO          : CDMA_MDSP_RD_TX_MIN_LIMIT_RATCHET_STATUS

 DESCRIPTION    : This macro gives the status if Total TX Power in closed loop
                  is below the minimum limit specified in CTxAttenLimitWr

 Return Value   : Value in the TX Open loop Accumulator

 =========================================================================*/
 #define CDMA_MDSP_RD_TX_MIN_LIMIT_RATCHET_STATUS()   \
               MDSP_READ( CDMA1X_cTxLimitingMin )

 /*=======================================================================
 MACRO          : CDMA_MDSP_RD_TX_MAX_LIMIT_RATCHET_STATUS

 DESCRIPTION    : This macro gives the status if Toatl TX Power in closed loop
                  is above the maximum limit specified in cTxGainLimitWr

 Return Value   : Value in the TX Open loop Accumulator

 =========================================================================*/
 #define CDMA_MDSP_RD_TX_MAX_LIMIT_RATCHET_STATUS()  \
               MDSP_READ( CDMA1X_cTxLimitingMax ) 

 /*=======================================================================
 MACRO          :  CDMA_MDSP_SET_TX_MASK_DATA_DELAY

 DESCRIPTION    : The software should write to this MDSP register 
                  HW programmed value + 2.Sets the MASK_DATA Delay for
                  different rates

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_TX_MASK_DATA_DELAY( val )   MDSP_WRITE( CDMA1X_cTxMaskDelayWr, val)
 
/*=======================================================================
 MACRO          :  CDMA_MDSP_RD_TRANSMIT_GAIN

 DESCRIPTION    : This Macro reads the Total Transmit Power
 

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_RD_TRANSMIT_GAIN()   MDSP_READ( CDMA1X_cTxTotalGainVal )

 /*=======================================================================
 MACRO          :  CDMA_MDSP_SET_TX_MIN_PWR

 DESCRIPTION    : This Maro sets the limit for Minimum Total TX Power
 

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_TX_MIN_PWR( val )   MDSP_WRITE( CDMA1X_cTxAttenLimitWr, val )

/*=======================================================================
 MACRO          :  CDMA_MDSP_SET_TX_MAX_PWR

 DESCRIPTION    : This Maro sets the limit for Maximum Total TX power
 

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_TX_MAX_PWR( val )   MDSP_WRITE( CDMA1X_cTxGainLimitWr, val )

 /*=======================================================================
 MACRO          : CDMA_MDSP_ENABLE_PA_RANGE_MAP

 DESCRIPTION    : This Macro enables the PA Range Map

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_ENABLE_PA_RANGE_MAP()   MDSP_WRITE( CDMA1X_cPaRangeMapEn, 0xFFFF )

 /*=======================================================================
 MACRO          :  CDMA_MDSP_DISABLE_PA_RANGE_MAP

 DESCRIPTION    :  This Macro disables the PA Range Map

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_DISABLE_PA_RANGE_MAP()   MDSP_WRITE( CDMA1X_cPaRangeMapEn, 0x0 )

/*=======================================================================
 MACRO          :  CDMA_MDSP_SET_PA_RANGE_MAP

 DESCRIPTION    : This macro Sets the PA Range Map

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_PA_RANGE_MAP( val )   MDSP_WRITE( CDMA1X_cPaRangeMap, val )

/*=======================================================================
 MACRO          :  CDMA_MDSP_ENABLE_PA_RANGE_OVERRIDE

 DESCRIPTION    : This macro is used to override the data form TX AGC units
                  on PA_R0 and PA_R1 pins 
              
 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_ENABLE_PA_RANGE_OVERRIDE()   MDSP_WRITE( CDMA1X_cPaRangeOverride, 0xFFFF )

 /*=======================================================================
 MACRO          :  CDMA_MDSP_SET_PA_RANGE_NORMAL

 DESCRIPTION    : This Macro lets the normal operation based on TX AGC unit

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_PA_RANGE_NORMAL()   MDSP_WRITE( CDMA1X_cPaRangeOverride, 0 )

 /*=======================================================================
 MACRO          :  CDMA_MDSP_SET_PA_R0_R1_OVERRIDE_VAL

 DESCRIPTION    : This Macro outputs the value on PA_R0 and PA_R1 pins
                  if PA Range Override is enabled by setting register
                  cPaRangeOverride

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_PA_R0_R1_OVERRIDE_VAL( val )   MDSP_WRITE( CDMA1X_cPaRangeWr, val )

 /*=======================================================================
 MACRO          :  CDMA_MDSP_RD_PA_R0_R1_PIN_STATUS

 DESCRIPTION    : This macro reads back the value which drives the pins
                  PA_R0 and PA_R1

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_RD_PA_R0_R1_PIN_STATUS()   MDSP_READ( CDMA1X_cPaRangeValue )
 
/*=======================================================================
 MACRO          :   CDMA_MDSP_GET_INTERNAL_PA_STATE

 DESCRIPTION    : This macro reads back the internal PA state

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_GET_INTERNAL_PA_STATE()   MDSP_READ( CDMA1X_cPaStateValue )

/*=======================================================================
 MACRO          :  CDMA_MDSP_SET_PA_L2H_GAIN_STATE_DLY

 DESCRIPTION    : This Macro is used to set the delay for the PA gain state
                  changing from Low to High
 
 Return Value   : None
=========================================================================*/
 #define CDMA_MDSP_SET_PA_L2H_GAIN_STATE_DLY( val )   MDSP_WRITE( CDMA1X_cPaRangeDelayL2HValue, val )
 
/*=======================================================================
 MACRO          :  CDMA_MDSP_SET_PA_H2L_GAIN_STATE_DLY

 DESCRIPTION    : This Macro is used to set the delay for the PA gain 
                  state change from High to Low
 
 Return Value   : None
=========================================================================*/
 #define CDMA_MDSP_SET_PA_H2L_GAIN_STATE_DLY( val )   MDSP_WRITE( CDMA1X_cPaRangeDelayH2LValue, val )

 /*=======================================================================
 MACRO          :  CDMA_MDSP_SET_PA_HYSTERISIS_TIMER_VAL

 DESCRIPTION    : This Macro is used to program the hysterisis timer for PA
                  state change from High to Low
 
 Return Value   : None
=========================================================================*/
 #define CDMA_MDSP_SET_PA_HYSTERISIS_TIMER_VAL( val )   MDSP_WRITE( CDMA1X_cPaHysteresisTimerValue, val )

/*=======================================================================
 MACRO          :  CDMA_MDSP_RESET_PA_STATE_MACHINE

 DESCRIPTION    : This Macro is used to reset the PA State machine
                  for debugging purposes
 
 Return Value   : None
=========================================================================*/
 #define CDMA_MDSP_RESET_PA_STATE_MACHINE()   MDSP_WRITE( CDMA1X_cPaReset, 0xFFFF )

/*=======================================================================
 MACRO          :  CDMA_MDSP_ENABLE_TX_AGC_ADJ_OVERRIDE

 DESCRIPTION    : This macro is used to override the TX_AGC_ADJ when
                  the software controls TX_AGC_ADJ
              
 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_ENABLE_TX_AGC_ADJ_OVERRIDE()   MDSP_WRITE( CDMA1X_cTxAgcAdjOverride, 0xFFFF )

 /*=======================================================================
 MACRO          :  CDMA_MDSP_SET_TX_AGC_ADJ_NORMAL

 DESCRIPTION    : This Macro is used to disable the override of TX_AGC_ADJ when
                  the software controls TX_AGC_ADJ
 
 Return Value   : None
=========================================================================*/
 #define CDMA_MDSP_SET_TX_AGC_ADJ_NORMAL()   MDSP_WRITE( CDMA1X_cTxAgcAdjOverride, 0 )

 #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
 /*=======================================================================
 MACRO          :  CDMA_MDSP_SET_TX_AGC_ADJ_VAL

 DESCRIPTION    : This Macro allows the SW to set the PDM value provided 
                  the MDSP register cTxAgcAdjOverride is in override mode
                  and Hardware register MOD_TX_AGC_ADJ_CTL is set
                  for software controlled PDM.The value is set in 32 bits
 
 Return Value   : None
=========================================================================*/
 #define CDMA_MDSP_SET_TX_AGC_ADJ_VAL( val )   MDSP_WRITE32( CDMA1X_cTxAgcAdjWr, val )

 #else
 /*=======================================================================
 MACRO          :  CDMA_MDSP_SET_TX_AGC_ADJ_VAL

 DESCRIPTION    : This Macro allows the SW to set the PDM value provided 
                  the MDSP register cTxAgcAdjOverride is in override mode
                  and Hardware register MOD_TX_AGC_ADJ_CTL is set
                  for software controlled PDM
 
 Return Value   : None
=========================================================================*/
 #define CDMA_MDSP_SET_TX_AGC_ADJ_VAL( val )   MDSP_WRITE( CDMA1X_cTxAgcAdjWr, val )
 #endif/*RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE*/

/*=======================================================================
 MACRO          :   CDMA_MDSP_GET_TX_AGC_ADJ_VAL

 DESCRIPTION    :   This macro reads the PDM value from MDSP

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_GET_TX_AGC_ADJ_VAL()   MDSP_READ( CDMA1X_cTxAgcAdjVal )

/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_LIN0_ENTRY

 DESCRIPTION    : Writes one entry to the TX Linearizer table for PA range 00

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_AGC_NUM_LINEARIZER_VAL (=33)
                                       when Beta Prime is not enabled
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                                       when Beta Prime is enabled
                  val - Data value for the element

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_LIN0_ENTRY( n, val )   MDSP_WRITE(CDMA1X_cTxLinLutPa0+(n), val)

/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_LIN1_ENTRY

 DESCRIPTION    : Writes one entry to the TX Linearizer table for PA range 01

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_AGC_NUM_LINEARIZER_VAL (=33)
                                       when Beta Prime is not enabled
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                                       when Beta Prime is enabled
                  val - Data value for the element

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_LIN1_ENTRY( n, val )   MDSP_WRITE(CDMA1X_cTxLinLutPa1+(n), val)

/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_LIN2_ENTRY

 DESCRIPTION    : Writes one entry to the TX Linearizer table for PA range 10

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_AGC_NUM_LINEARIZER_VAL (=33)
                                       when Beta Prime is not enabled
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                                       when Beta Prime is enabled
                  val - Data value for the element

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_LIN2_ENTRY( n, val )   MDSP_WRITE(CDMA1X_cTxLinLutPa2+(n), val)

/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_LIN3_ENTRY

 DESCRIPTION    : Writes one entry to the TX Linearizer table for PA range 11

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_AGC_NUM_LINEARIZER_VAL (=33)
                                       when Beta Prime is not enabled
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                                       when Beta Prime is enabled
                  val - Data value for the element

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_LIN3_ENTRY( n, val )   MDSP_WRITE(CDMA1X_cTxLinLutPa3+(n), val)

/*========================================================================
 MACRO          : HDRAGC_FRAME_LOAD_TX_PA_PARAMS()

 DESCRIPTION    : Triggers Tx PA double-buffered parameters to be re-
                  loaded and used by the DSP.

 INPUTS         : None

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_FRAME_LOAD_TX_PA_PARAMS()            MDSP_WRITE(CDMA1X_cPaFrameParamsAvailable, 0xFFFF)

/* PA Threshold structure offsets
*/
#define CDMA_MDSP_PA_R1_RISE_OFFSET    (0)
#define CDMA_MDSP_PA_R2_RISE_OFFSET    (1)
#define CDMA_MDSP_PA_R3_RISE_OFFSET    (2)
#define CDMA_MDSP_PA_R1_FALL_OFFSET    (3)
#define CDMA_MDSP_PA_R2_FALL_OFFSET    (4)
#define CDMA_MDSP_PA_R3_FALL_OFFSET    (5)


/*========================================================================
 MACRO          : CMD_MDSP_SET_PA_R1_RISE

 DESCRIPTION    : Sets PA R1 rise value (for transitioning from PA
                  state 0 to 1).

 INPUTS         : Value - The value to set.

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_SET_PA_R1_RISE( val )                               \
          MDSP_WRITE( CDMA1X_cPaStateThreshParams+CDMA_MDSP_PA_R1_RISE_OFFSET, (val))


/*========================================================================
 MACRO          : CMD_MDSP_READ_PA_R1_RISE

 DESCRIPTION    : Reads PA R1 rise value (for transitioning from PA
                  state 0 to 1).

 INPUTS         : Value - The value to set.

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_READ_PA_R1_RISE()                               \
          MDSP_READ( CDMA1X_cPaStateThreshParams+CDMA_MDSP_PA_R1_RISE_OFFSET)

/*========================================================================
 MACRO          : CMD_MDSP_SET_PA_R2_RISE

 DESCRIPTION    : Sets PA R2 rise value (for transitioning from PA
                  state 1 to 2).

 INPUTS         : Value - The value to set.

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_SET_PA_R2_RISE( val )                               \
         MDSP_WRITE( CDMA1X_cPaStateThreshParams+CDMA_MDSP_PA_R2_RISE_OFFSET, (val))


/*========================================================================
 MACRO          :  CMD_MDSP_SET_PA_R3_RISE

 DESCRIPTION    : Sets PA R3 rise value (for transitioning from PA
                  state 2 to 3).

 INPUTS         : Value - The value to set.

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_SET_PA_R3_RISE( val )                               \
          MDSP_WRITE( CDMA1X_cPaStateThreshParams+CDMA_MDSP_PA_R3_RISE_OFFSET, (val))
          
/*========================================================================
 MACRO          : CDMA_MDSP_SET_PA_R1_FALL

 DESCRIPTION    : Sets PA R1 fall value (for transitioning from PA
                  state 1 to 0).

 INPUTS         : Value - The value to set.

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_SET_PA_R1_FALL( val )                               \
          MDSP_WRITE( CDMA1X_cPaStateThreshParams+CDMA_MDSP_PA_R1_FALL_OFFSET, (val))


/*========================================================================
 MACRO          : CMD_MDSP_READ_PA_R1_FALL

 DESCRIPTION    : Reads PA R1 FAll value (for transitioning from PA
                  state 0 to 1).

 INPUTS         : Value - The value to set.

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_READ_PA_R1_FALL()                               \
          MDSP_READ( CDMA1X_cPaStateThreshParams+CDMA_MDSP_PA_R1_FALL_OFFSET)

/*========================================================================
 MACRO          : CDMA_MDSP_SET_PA_R2_FALL

 DESCRIPTION    : Sets PA R2 fall value (for transitioning from PA
                  state 2 to 1).

 INPUTS         : Value - The value to set.

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_SET_PA_R2_FALL( val )                               \
           MDSP_WRITE( CDMA1X_cPaStateThreshParams+CDMA_MDSP_PA_R2_FALL_OFFSET, (val))

/*========================================================================
 MACRO          : CDMA_MDSP_SET_PA_R3_FALL

 DESCRIPTION    : Sets PA R3 fall value (for transitioning from PA
                  state 3 to 2).

 INPUTS         : Value - The value to set.

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_SET_PA_R3_FALL( val )                               \
           MDSP_WRITE( (CDMA1X_cPaStateThreshParams+(CDMA_MDSP_PA_R3_FALL_OFFSET)), (val))

/*========================================================================
 MACRO          : CDMA_MDSP_SET_PA_OFFSET

 DESCRIPTION    : Sets PA offset value.

 INPUTS         : State - the PA state to apply it to.
                  Value - The value to set.

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_PA_OFFSET( state, val )                         \
          MDSP_WRITE((CDMA1X_cPaOffsetParams + (state)),  (val))

/*========================================================================
 MACRO          : CDMA_MDSP_SET_PA_RANGE_POLARITY

 DESCRIPTION    : This Macro sets the polarity of PA_R0 and PA_R1 pins

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_PA_RANGE_POLARITY( val )                         \
          MDSP_WRITE( CDMA1X_cPaRangePolarity, val )
          
/*========================================================================
 MACRO          : CDMA_MDSP_SET_RF_PN_ROLL_DELAY

 DESCRIPTION    : Sets the RF PN roll delay value, in chips.
                  
 INPUTS         : The delay to set, in chips.

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_SET_RF_PN_ROLL_DELAY(val)                               \
          MDSP_WRITE( CDMA1X_cRfPnRollTime, (val))

/*========================================================================
 MACRO          : HDRAGC_SET_TX_LIN_X_OFFSET()

 DESCRIPTION    : Sets the Tx X offset to the linearizer. The shifts
                  the linearizer table along the x axis (power).

 INPUTS         : table - the linearizer table to write to.
                  value - The value to shift.

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_LIN_X_OFFSET( table, val )                      \
          MDSP_WRITE((CDMA1X_cTxLinXOffsetParams + (table)), val)


/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_LIN_Y_OFFSET

 DESCRIPTION    : Sets the Tx Y offset to the linearizer. The shifts
                  the linearizer table along the y axis (pdm val).

 INPUTS         : value - The value to shift.

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_LIN_Y_OFFSET( table, val )                      \
          MDSP_WRITE((CDMA1X_cTxLinYOffsetParams + (table)), val)


/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_LIN_SLOPE

 DESCRIPTION    : Sets the Tx slope offset to the linearizer.  Each
                  element in the table is increased proportionally to it's
                  place in the table (element# * value).

 INPUTS         : value - The slope value.

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_LIN_SLOPE( table, val )                   \
          MDSP_WRITE((CDMA1X_cTxLinSlopeParams + (table)), val)

/*========================================================================
 MACRO          : CDMA_MDSP_FRAME_LOAD_TX_LIN_PARAMS

 DESCRIPTION    : Triggers Tx Linearizer double-buffered parameters to be re-
                  loaded and used by the DSP.

 INPUTS         : None

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_FRAME_LOAD_TX_LIN_PARAMS()                 \
          MDSP_WRITE( CDMA1X_cTxLinFrameParamsAvailable, 0xFFFF )
          
/*=======================================================================
 MACRO          : CDMA_MDSP_ENABLE_PA_STATE_OVERRIDE

 DESCRIPTION    : This macro Overrides the PA state 

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_ENABLE_PA_STATE_OVERRIDE()   MDSP_WRITE( CDMA1X_cPaStateOverride, 0xFFFF )

 /*=======================================================================
 MACRO          : CDMA_MDSP_SET_PA_STATE_NORMAL

 DESCRIPTION    : This macro is set to set the normal Operation of
                  PA State machine
 
Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_PA_STATE_NORMAL()   MDSP_WRITE( CDMA1X_cPaStateOverride, 0x0 )

 /*=======================================================================
 MACRO          : CDMA_MDSP_SET_PA_STATE

 DESCRIPTION    : This macro is set to set the internal PA state when
                  it is placed in override mode

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_PA_STATE( val )   MDSP_WRITE( CDMA1X_cPaStateWr, val )

 /*=======================================================================
 MACRO          : CDMA_MDSP_ENABLE_PA_HYSTERISIS_TIMER

 DESCRIPTION    :  This Macro enables the PA hysteris timer when the PA 
                   switches from High to Low gain state

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_ENABLE_PA_HYSTERISIS_TIMER()   MDSP_WRITE( CDMA1X_cTempHysteresisEn, 0xFFFF )

 /*=======================================================================
 MACRO          : CDMA_MDSP_DISABLE_PA_HYSTERISIS_TIMER

 DESCRIPTION    : This Macro disables the PA hysteris timer when the PA 
                  switches from High to Low gain state

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_DISABLE_PA_HYSTERISIS_TIMER()   MDSP_WRITE( CDMA1X_cTempHysteresisEn, 0x0 )

/*========================================================================
 MACRO          : CDMA_MDSP_SET_MOD_PHASE_ROTATOR

 DESCRIPTION    : This Macro sets the PN phase rotation value in modulator
                  for diffrerent PA ranges

 RETURN VALUE   : None
 ========================================================================*/

 #define CDMA_MDSP_SET_MOD_PHASE_ROTATOR( val )   MDSP_WRITE(CDMA1X_cPaRfPnRot , val)

/*=======================================================================
 MACRO          : CDMA_MDSP_NEW_PA_THRESH_FRAME_PARAMS_AVAILABLE

 DESCRIPTION    : This macro is used to inform DSP that PA Threholds/Offsets
                  have been latched to be picked at frame boundary.

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_NEW_PA_THRESH_FRAME_PARAMS_AVAILABLE()  MDSP_WRITE(CDMA1X_cPaFrameParamsAvailable, 0xFFFF)

/*=======================================================================
 MACRO          : CDMA_MDSP_NEW_PA_THRESH_SLOT_PARAMS_AVAILABLE

 DESCRIPTION    : This macro is used to inform DSP that PA Threholds have 
                  been latched to be picked at slot boundary.

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_NEW_PA_THRESH_SLOT_PARAMS_AVAILABLE() MDSP_WRITE(CDMA1X_cPaNewParamsAvailable, 0xFFFF)

 /*=======================================================================
 MACRO          : CDMA_MDSP_NEW_TX_LIMIT_FRAME_PARAMS_AVAILABLE

 DESCRIPTION    : This macro is used to inform DSP that TX Power MAX/MIN
                  limits have been latched to be picked at frame boundary.

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_NEW_TX_LIMIT_FRAME_PARAMS_AVAILABLE()   MDSP_WRITE(CDMA1X_cTxLimitFrameParamsAvailable, 0xFFFF)

/*=======================================================================
 MACRO          : CDMA_MDSP_NEW_TX_LIMIT_SLOT_PARAMS_AVAILABLE

 DESCRIPTION    : This macro is used to inform DSP that TX Power MAX/MIN
                  limits have been latched to be picked at slot boundary.

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_NEW_TX_LIMIT_SLOT_PARAMS_AVAILABLE()  MDSP_WRITE(CDMA1X_cTxLimitNewParamsAvailable, 0xFFFF)

/*=======================================================================
 MACRO          : CDMA_MDSP_NEW_TX_LIN_FRAME_PARAMS_AVAILABLE

 DESCRIPTION    : This macro is used to inform DSP that TX Linearizer 
                 Offset/Slope limits have been latched to be picked at 
                 frame boundary.

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_NEW_TX_LIN_FRAME_PARAMS_AVAILABLE()     MDSP_WRITE(CDMA1X_cTxLinFrameParamsAvailable, 0xFFFF)

/*=======================================================================
 MACRO          : CDMA_MDSP_NEW_TX_LIN_SLOT_PARAMS_AVAILABLE

 DESCRIPTION    : This macro is used to inform DSP that TX Linearizer 
                 Offset/Slope limits have been latched to be picked at 
                 slot boundary.

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_NEW_TX_LIN_SLOT_PARAMS_AVAILABLE()      MDSP_WRITE(CDMA1X_cTxLinNewParamsAvailable, 0xFFFF)

/*=======================================================================
 MACRO          : CDMA_MDSP_GET_PA_THRESH_FRAME_PARAMS_LOAD_STATUS

 DESCRIPTION    : This Macro is used to get the current ststus on whether
                  new PA threshold parameters can be loaded at frame boundary

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_GET_PA_THRESH_FRAME_PARAMS_LOAD_STATUS()  MDSP_READ(CDMA1X_cPaFrameParamsAvailable)

/*=======================================================================
 MACRO          : CDMA_MDSP_GET_PA_THRESH_SLOT_PARAMS_LOAD_STATUS

 DESCRIPTION    : This Macro is used to get the current ststus on whether
                  new PA threshold parameters can be loaded at slot boundary

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_GET_PA_THRESH_SLOT_PARAMS_LOAD_STATUS()   MDSP_READ(CDMA1X_cPaNewParamsAvailable)

 /*=======================================================================
 MACRO          : CDMA_MDSP_GET_TX_LIMIT_FRAME_PARAMS_LOAD_STATUS

 DESCRIPTION    : This Macro is used to get the current ststus on whether
                  new TX Max/Min limit threshold parameters can be loaded 
                  at frame boundary

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_GET_TX_LIMIT_FRAME_PARAMS_LOAD_STATUS()   MDSP_READ(CDMA1X_cTxLimitFrameParamsAvailable)

/*=======================================================================
 MACRO          : CDMA_MDSP_GET_TX_LIMIT_SLOT_PARAMS_LOAD_STATUS

 DESCRIPTION    : This Macro is used to get the current ststus on whether
                  new TX Max/Min limit threshold parameters can be loaded 
                  at slot boundary

 Return Value   : None

 =========================================================================*/
 #define  CDMA_MDSP_GET_TX_LIMIT_SLOT_PARAMS_LOAD_STATUS()  MDSP_READ(CDMA1X_cTxLimitNewParamsAvailable)

/*=======================================================================
 MACRO          : CDMA_MDSP_GET_TX_LIN_FRAME_PARAMS_LOAD_STATUS

 DESCRIPTION    : This Macro is used to get the current ststus on whether
                  new TX Linearizer parameters can be loaded 
                  at frame boundary

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_GET_TX_LIN_FRAME_PARAMS_LOAD_STATUS()     MDSP_READ(CDMA1X_cTxLinFrameParamsAvailable)

/*=======================================================================
 MACRO          : CDMA_MDSP_GET_TX_LIN_SLOT_PARAMS_LOAD_STATUS

 DESCRIPTION    : This Macro is used to get the current ststus on whether
                  new TX Linearizer parameters can be loaded 
                  at slot boundary

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_GET_TX_LIN_SLOT_PARAMS_LOAD_STATUS()      MDSP_READ(CDMA1X_cTxLinNewParamsAvailable)

/*=======================================================================
 MACRO          : CDMA_MDSP_CLEAR_PA_THRESH_FRAME_FLAG

 DESCRIPTION    : This Macro clears the flag for loading the PA Threshols
                  parameter at frame boundary
 
 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_CLEAR_PA_THRESH_FRAME_FLAG()              MDSP_WRITE(CDMA1X_cPaFrameParamsAvailable, 0)

/*=======================================================================
 MACRO          : CDMA_MDSP_RECOVER_TX_CLOSED_LOOP_NORMAL

 DESCRIPTION    : 
 
 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_RECOVER_TX_CLOSED_LOOP_NORMAL()          MDSP_WRITE(CDMA1X_cTxGainAdjClrOverride, 0xFFFF )

/*=======================================================================
 MACRO          : CDMA_MDSP_SET_TX_SSBI_PORT

 DESCRIPTION    : This Macro passes the SSBI Port at which MDSP would send
                  the TX AGC Updates to RFT
 
 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_TX_SSBI_PORT(val)                     MDSP_WRITE(CDMA1X_cTxSsbiPort, val )                     

/*=======================================================================
 MACRO          : CDMA_MDSP_SET_RFT_REG1_ADDR

 DESCRIPTION    : This Macro passes the register address in RFT which
                  should be written when the TX AGC ADJ value to be set 
                  by MDSP is even 
 
 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_RFT_REG1_ADDR(val)                    MDSP_WRITE(CDMA1X_cTxSsbiReg1Addr, val )

/*=======================================================================
 MACRO          : CDMA_MDSP_SET_RFT_REG2_ADDR

 DESCRIPTION    : This Macro passes the register address in RFT which
                  should be written when the TX AGC ADJ value to be set 
                  by MDSP is odd
 
 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_RFT_REG2_ADDR(val)                    MDSP_WRITE(CDMA1X_cTxSsbiReg2Addr, val )

 #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
/*=======================================================================
 MACRO          : CDMA_MDSP_SET_RFT_REG3_ADDR

 DESCRIPTION    : This Macro passes the register address in RFT which
                  should be written when the TX AGC ADJ value to be set 
                  by MDSP is even 
 
 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_RFT_REG3_ADDR(val)                    MDSP_WRITE(CDMA1X_cTxSsbiReg3Addr, val )

/*=======================================================================
 MACRO          : CDMA_MDSP_SET_RFT_REG4_ADDR

 DESCRIPTION    : This Macro passes the register address in RFT which
                  should be written when the TX AGC ADJ value to be set 
                  by MDSP is odd
 
 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_RFT_REG4_ADDR(val)                    MDSP_WRITE(CDMA1X_cTxSsbiReg4Addr, val )
 #endif

/*=======================================================================
 MACRO          : CDMA_MDSP_SET_COMBINER_FREQUENCY

 DESCRIPTION    : 

 Return Value   : None

 =========================================================================*/
 #define CDMA_MDSP_SET_COMBINER_FREQUENCY(val)               MDSP_WRITE(CDMA1X_cTxCombProcFreq, val)



#ifdef RF_HAS_BETA_SCALING_TX_AGC

/*=======================================================================
 MACRO          :  CDMA_MDSP_ENABLE_TX_BETAP

 DESCRIPTION    : Enable Beta Scaling in FW TX AGC.

 Return Value   : None
 =========================================================================*/

#define CDMA_MDSP_ENABLE_TX_BETAP()            MDSP_WRITE( CDMA1X_cTxBetapEnable, 0xFFFF )

/*=======================================================================
 MACRO          :  CDMA_MDSP_DISABLE_TX_BETAP

 DESCRIPTION    : Disable Beta Scaling in FW TX AGC.

 Return Value   : None
 =========================================================================*/

#define CDMA_MDSP_DISABLE_TX_BETAP()           MDSP_WRITE( CDMA1X_cTxBetapEnable, 0 )

/*=======================================================================
 MACRO          :  CDMA_MDSP_ENABLE_TX_BETAP_OVERRIDE

 DESCRIPTION    : Enable the override mode. In this mode
                  the Betap value can be set manually.

 Return Value   : None
 =========================================================================*/

#define CDMA_MDSP_ENABLE_TX_BETAP_OVERRIDE()   MDSP_WRITE( CDMA1X_cTxBetapGainOverride, 0xFFFF )

 /*=======================================================================
 MACRO          :  CDMA_MDSP_SET_TX_BETAP_NORMAL

 DESCRIPTION    : Disable the override mode. In this mode
                  the Betap value cannot be set manually.

 Return Value   : None
=========================================================================*/

#define CDMA_MDSP_SET_TX_BETAP_NORMAL()   MDSP_WRITE( CDMA1X_cTxBetapGainOverride, 0 )

/*=======================================================================
 MACRO          :  CDMA_MDSP_SET_TX_BETAP_VAL

 DESCRIPTION    : Manually set the TX Betap value (bits 7:0 in Q8 format).
                  Betap Override mode must be enabled.

 Return Value   : None
 =========================================================================*/

#define CDMA_MDSP_SET_TX_BETAP_VAL( val )   MDSP_WRITE( CDMA1X_cTxBetapGainWr, val)

/*=======================================================================
 MACRO          : CDMA_MDSP_RD_TX_BETAP_VAL

 DESCRIPTION    : Read the TX Betap value written to HW (bits 7:0 in Q8 format)
                  Note: cTxBetapVal = (exp10(-(cTxBetapGain * 5243)) >> 7);

 Return Value   : TX Betap value written to HW
 =========================================================================*/

#define CDMA_MDSP_RD_TX_BETAP_VAL()   MDSP_READ( CDMA1X_cTxBetapVal )

/*=======================================================================
 MACRO          : CDMA_MDSP_RD_TX_BETAP_GAIN

 DESCRIPTION    : Read the TX Betap gain value in units of -1/640 dB.
                  Note: cTxBetapVal = (exp10(-(cTxBetapGain * 5243)) >> 7);

 Return Value   : TX Betap gain value
 =========================================================================*/

#define CDMA_MDSP_RD_TX_BETAP_GAIN()   MDSP_READ( CDMA1X_cTxBetapGain )

/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_PDM0_ENTRY

 DESCRIPTION    : Writes one entry to the TX PDM table for PA range 00

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                  val - Data for the element

 RETURN VALUE   : None
 ========================================================================*/
#ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_PDM0_ENTRY

 DESCRIPTION    : Writes one entry to the TX PDM table for PA range 01.

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                  val - Data for the element

 RETURN VALUE   : None
========================================================================*/
#define CDMA_MDSP_SET_TX_PDM0_ENTRY( n, val )   MDSP_WRITE32(CDMA1X_cTxPdmTablePa0+(n), val)
/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_PDM1_ENTRY

 DESCRIPTION    : Writes one entry to the TX PDM table for PA range 01

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                  val - Data for the element

 RETURN VALUE   : None
========================================================================*/
#define CDMA_MDSP_SET_TX_PDM1_ENTRY( n, val )   MDSP_WRITE32(CDMA1X_cTxPdmTablePa1+(n), val)
/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_PDM2_ENTRY

 DESCRIPTION    : Writes one entry to the TX PDM table for PA range 10

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                  val - Data for the element

 RETURN VALUE   : None
 ========================================================================*/
#define CDMA_MDSP_SET_TX_PDM2_ENTRY( n, val )   MDSP_WRITE32(CDMA1X_cTxPdmTablePa2+(n), val)

/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_PDM3_ENTRY

 DESCRIPTION    : Writes one entry to the TX PDM table for PA range 11

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                  val - Data for the element

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_PDM3_ENTRY( n, val )   MDSP_WRITE32(CDMA1X_cTxPdmTablePa3+(n), val)

#else
/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_PDM0_ENTRY

 DESCRIPTION    : Writes one entry to the TX PDM table for PA range 01.

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                  val - Data for the element

 RETURN VALUE   : None
========================================================================*/

#define CDMA_MDSP_SET_TX_PDM0_ENTRY( n, val )   MDSP_WRITE(CDMA1X_cTxPdmTablePa0+(n), val)

/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_PDM1_ENTRY

 DESCRIPTION    : Writes one entry to the TX PDM table for PA range 01

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                  val - Data for the element

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_PDM1_ENTRY( n, val )   MDSP_WRITE(CDMA1X_cTxPdmTablePa1+(n), val)

/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_PDM2_ENTRY

 DESCRIPTION    : Writes one entry to the TX PDM table for PA range 10

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                  val - Data for the element

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_PDM2_ENTRY( n, val )   MDSP_WRITE(CDMA1X_cTxPdmTablePa2+(n), val)

/*========================================================================
 MACRO          : CDMA_MDSP_SET_TX_PDM3_ENTRY

 DESCRIPTION    : Writes one entry to the TX PDM table for PA range 11

 INPUTS         : n   - Element to write:
                         0 <= n < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ (=64,48)
                  val - Data for the element

 RETURN VALUE   : None
 ========================================================================*/

#define CDMA_MDSP_SET_TX_PDM3_ENTRY( n, val )   MDSP_WRITE(CDMA1X_cTxPdmTablePa3+(n), val)

#endif/*RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE*/
#endif /* RF_HAS_BETA_SCALING_TX_AGC */



#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */


/*=======================================================================
 MACRO          : GPS_MDSP_GET_POWER_HIGH_FLAG

 DESCRIPTION    : Read the flag indicating high TX power is detected. 

 Return Value   : -1 when mdsp generates the isr to ARM.
 =========================================================================*/
#define GPS_MDSP_GET_POWER_HIGH_FLAG()                      MDSP_READ(CDMA1X_cTxPowerHighIntFlag)

/*=======================================================================
 MACRO          : GPS_MDSP_SET_POWER_HIGH_FLAG

 DESCRIPTION    : ARM reset the flag when high tx isr is served. should set
                  the flag to 0.

 Return Value   : None.
 =========================================================================*/ 
#define GPS_MDSP_SET_POWER_HIGH_FLAG(val)                   MDSP_WRITE(CDMA1X_cTxPowerHighIntFlag, val)

/*=======================================================================
 MACRO          : GPS_MDSP_SET_TX_POWER_DET_ENABLE

 DESCRIPTION    : Enable mdsp to detect possible high TX power;
                  -1: Enable; 0: Disable

 Return Value   : None.
=========================================================================*/
#define GPS_MDSP_SET_TX_POWER_DET_ENABLE(val)               MDSP_WRITE(CDMA1X_cTxPowerDetEnable, val)

/*=======================================================================
 MACRO          : GPS_MDSP_SET_TX_POWER_DET_THRESH

 DESCRIPTION    : Set Tx high power detect threshold with resolution of 
                  - 1/640dB.
                  
 Return Value   : None.
 =========================================================================*/
#define GPS_MDSP_SET_TX_POWER_DET_THRESH(val)               MDSP_WRITE(CDMA1X_cTxPowerDetThresh, val)
 
 
/*=======================================================================
 MACRO          : CDMA_MDSP_SET_TX_AGC_UPDATE_L2H

 DESCRIPTION    : Set the SSBI action time update ahead of the PA switch time 
                  in resolution of cx1 for PA transition from Low to High
                  
 Return Value   : None.
 =========================================================================*/
#define CDMA_MDSP_SET_TX_AGC_UPDATE_L2H(val)               MDSP_WRITE(CDMA1X_cTxAgcUpdateL2H, val)

/*=======================================================================
 MACRO          : CDMA_MDSP_SET_TX_AGC_UPDATE_H2L

 DESCRIPTION    : Set the SSBI action time update ahead of the PA switch time 
                  in resolution of cx1 for PA transition from High to Low
                  
 Return Value   : None.
 =========================================================================*/
#define CDMA_MDSP_SET_TX_AGC_UPDATE_H2L(val)               MDSP_WRITE(CDMA1X_cTxAgcUpdateH2L, val)

/*=======================================================================
 MACRO          : HDR_MDSP_SET_TX_AGC_UPDATE_L2H

 DESCRIPTION    : Set the SSBI action time update ahead of the PA switch time 
                  in resolution of cx1 for PA transition from Low to High
                  
 Return Value   : None.
 =========================================================================*/
#define HDR_MDSP_SET_TX_AGC_UPDATE_L2H(val)               MDSP_WRITE(HDR_txAgcUpdateL2H, val)

/*=======================================================================
 MACRO          : HDR_MDSP_SET_TX_AGC_UPDATE_H2L

 DESCRIPTION    : Set the SSBI action time update ahead of the PA switch time 
                  in resolution of cx1 for PA transition from High to Low
                  
 Return Value   : None.
 =========================================================================*/
#define HDR_MDSP_SET_TX_AGC_UPDATE_H2L(val)               MDSP_WRITE(HDR_txAgcUpdateH2L, val)

/*=======================================================================
 MACRO          : CDMA_MDSP_SET_BETAP_TIME_DELTA

 DESCRIPTION    : Set the time delay to update the BetaP gains after the PDM vlaue change

                  
 Return Value   : None.
 =========================================================================*/
#define CDMA_MDSP_SET_BETAP_TIME_DELTA(val)               MDSP_WRITE(CDMA1X_cBetapTimeDetla, val)

#ifdef RF_HAS_TX_DAC_BACKOFF_DIFF_FROM_LEGACY
#define CDMA_MDSP_SET_TX_DAC_BACKOFF_VAL(val)              MDSP_WRITE(CDMA1X_passthruGainScale, val)
#endif

#endif /* RF_MDSP_TX_AGC_H */


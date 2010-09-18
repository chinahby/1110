#ifndef CUSTUIM_HW_11X0_H
#define CUSTUIM_HW_11X0_H
/*===========================================================================

            " C U S T -  U I M - H W - 11X0 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for UIM hardware specifics for QSC11X0.

  Copyright (c) 2008 - 2010 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/su/cust/rel/07H1_2/custuim_hw_qsc11x0.h#8 $ $DateTime: 2010/03/22 01:15:47 $ $Author: rmandala $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/17/10   rm     Disable FEATURE_UIM_UICC when FEATURE_CSIM is not defined
02/02/10   ps     Disable FEATURE_UIM_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME
12/22/09   yb     Re-enable FEATURE_UIM_DEBUG_LOG feature
09/23/09   yb     Enabling feature FEATURE_UIM_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME
09/15/09   yb     Disable FEATURE_UIM_DEBUG_LOG feature temporarily
03/04/09   rm     Remove FEATURE_UIM_QSC11X0_SPEED_FIX_SLOWBITRATE_CARDS feature
                  as it is not needed anymore
01/29/09   rm     Enable the speed enhancement fix for QSC11X0
06/18/08   jk     Initial revision.

===========================================================================*/
/* 
 *  Features specific to QSC11X0 ONLY
*/ 


#define FEATURE_UIM_CONTROL_UIM_UART_CLK_REGIME

/* Use TCXO clock, not GSM */
#undef FEATURE_UIM_GSM_CLK_SRC

/* Allows for the UIM to runtime calcualte GCD for MND Calculations. */
#define FEATURE_UIM_NO_LEGACY_MND_TABLE

/* Feature to indicate that the source to the UART is TCXO */
#define FEATURE_UIM_TCXO_CLK_FREQ

/* Feature to indicate that the clock to the card is TCXO/5 */
#define FEATURE_UIM_USE_TCXO_DIV_5

/* Feature to enable QSC 11X0 platform specific driver code */
#define FEATURE_UIM_QSC11X0_DRIVER

/* Enables UART DM / Word RX & TX specific code */
#define FEATURE_UIM_UART_DM

/* Needed for T=1*/
#define FEATURE_UIM_T_1_SUPPORT
#define FEATURE_UIM_BLOCK_MODE_SUPPORT
#define FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG

#if defined(FEATURE_RUIM) && !defined(FEATURE_CSIM)
#undef FEATURE_UIM_UICC
#endif /* FEATURE_RUIM && !FEATURE_CSIM */

#endif /* CUSTUIM_HW_11X0_H */

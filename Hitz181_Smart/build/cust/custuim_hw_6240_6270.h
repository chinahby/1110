#ifndef CUSTUIM_HW_6270_H
  #define CUSTUIM_HW_6270_H
/*===========================================================================

            " C U S T -  U I M - H W - 62X0 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for UIM hardware specifics for MSM6270.

  Copyright (c) 2008 - 2009 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/su/cust/rel/07H1_2/custuim_hw_6240_6270.h#5 $ $DateTime: 2009/04/15 09:22:58 $ $Author: rmandala $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
04/13/09   rm     Disabled FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
03/25/09   rm     Re-enable FEATURE_UIM_DEBUG_LOG feature
03/06/09   rm     Disable FEATURE_UIM_DEBUG_LOG feature temporarily
01/12/09   vs     Define UIM USB UICC feature only if HS-USB UICC is defined
11/17/08   as/nk  Initial Version Hardware Specific File for 62x0

===========================================================================*/

/* Features specific to T_QSC6270 ONLY */

  #if defined(T_QSC6270) || defined (T_QSC6240) 
    /* Feature to not use the legacy MND table */
    #define FEATURE_UIM_NO_LEGACY_MND_TABLE
    /* Feature to indicate that the source to the UART is TCXO */
    #define FEATURE_UIM_TCXO_CLK_FREQ
    /* Feature to indicate that the clock to the card is TCXO/5 */
    #define FEATURE_UIM_USE_TCXO_DIV_5
    #define FEATURE_UIM_USES_NEW_SLEEP_API
    /* LCU uses the VREG on PMIC */
    #define FEATURE_UIM_USE_ONLY_VREG_ON_PMIC
    /* Enable changing the SIM mode via a WARM RESET */
    #define FEATURE_UIM_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET
    /* Enables UART DM UIM Driver */
    #define FEATURE_UIM_UART_DM
    /* UIM USB UICC feature support */
    #ifdef FEATURE_HS_USB_UICC
      #define FEATURE_UIM_USB_UICC
    #endif /* FEATURE_HS_USB_UICC */
    /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC is not needed for 6270 */
    #undef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  #endif /* defined(T_QSC6270) || defined (T_QSC6240) */

#endif /* CUSTUIM_HW_6270_H */

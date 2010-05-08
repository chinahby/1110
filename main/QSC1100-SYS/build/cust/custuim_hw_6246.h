#ifndef CUSTUIM_HW_6246_H
#define CUSTUIM_HW_6246_H
/*===========================================================================

            " C U S T -  U I M - H W - 6246 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for UIM hardware specifics for MSM6246.

  Copyright (c) 2008 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/su/cust/rel/07H1_2/custuim_hw_6246.h#2 $ $DateTime: 2009/01/13 01:39:08 $ $Author: viveks $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/09   vs      Define UIM USB UICC feature only if HS-USB UICC is defined
09/15/08   js      Removed FEATURE_UIM_USB_UICC_RST_HIGH
09/15/08   vs      Removed featurization FEATURE_UIM_USB_UICC_SUSPEND_RESUME
08/05/08   vs      Added FEATURE_UIM_USB_UICC_SUSPEND_RESUME
07/17/08   vs      Added FEATURE_UIM_USB_UICC, FEATURE_UIM_USB_UICC_RST_HIGH
06/03/08   rm      Enabling the TA1=0x97 feature macros.
16/04/08   rm      Initial revision.

===========================================================================*/

/*
 *  Features specific to T_MSM6246 ONLY
 */

#ifdef T_MSM6246
  #define FEATURE_UIM_USE_ONLY_VREG_ON_PMIC
  #undef  FEATURE_UIM_PMIC_ON_UIM1
  #undef  FEATURE_UIM_PMIC_ON_UIM2
  #undef  FEATURE_UIM_USES_OLD_PMIC_API

  /* Enable TA1=0x97 Feature */
  #define FEATURE_UIM_HANDLE_TA1_0X97_ATR
  /* Enable changing the SIM mode via a WARM RESET */
  #define FEATURE_UIM_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET
  /* Enabling UIM USB UICC */
  #ifdef FEATURE_HS_USB_UICC
    #define FEATURE_UIM_USB_UICC
  #endif /* FEATURE_HS_USB_UICC */
#endif /* T_MSM6246 */

#endif /* CUSTUIM_HW_6246_H */

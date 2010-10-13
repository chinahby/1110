
#ifndef CUSTUIM_HW_8x50_H
#define CUSTUIM_HW_8x50_H
/*===========================================================================

            " C U S T -  U I M - H W - 8x50 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for UIM hardware specifics for 8250 and 8650.

  Copyright (c) 2008 - 2009 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/su/cust/rel/07H1_2/custuim_hw_8x50.h#4 $ $DateTime: 2009/05/22 03:30:02 $ $Author: sratnu $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/09   ssr     Enable FEATURE_RUIM_CDMA_REFRESH
05/04/09   xz      define FEATURE_GSTK_REMOTE_TIME
12/08/08   jar     Enable Multiple Voltage support for FFA and not for Surf
11/13/08   nk      Initial revision.

===========================================================================*/

/*
 *  Features specific to T_MSM8650 ONLY
 */

  /* IMPORTANT NOTE: There is a H/W issue with the PMIC interface to UIM that
     is using the PMIC level translation. So, we have changed H/W to bypass
     the PMIC so that we use VREG_UIM1 from PMIC only for the UIM power.

     If you already have your target ready to go with the direct connection
     to bypass PMIC, you must use the below setting for your H/W.

     #define FEATURE_UIM_USE_ONLY_VREG_ON_PMIC
     #undef  FEATURE_UIM_PMIC_ON_UIM1
     #undef  FEATURE_UIM_PMIC_ON_UIM2
  */

#ifdef T_MSM8650A
  /* Feature to indicate that the source to the UART is TCXO */
  #define FEATURE_UIM_TCXO_CLK_FREQ
  /* Feature to indicate that the clock to the card is TCXO/5 */
  #define FEATURE_UIM_USE_TCXO_DIV_5

  #if !defined(T_FFA)
    /*
     *  The Surf Reference Design doesn't use a PMIC for
     *  Voltage translation. I/O, CLK and RST will not go
     *  through PMIC.
     */
    #define FEATURE_UIM_USE_ONLY_VREG_ON_PMIC
    #undef  FEATURE_UIM_PMIC_ON_UIM1
    #undef  FEATURE_UIM_PMIC_ON_UIM2
  #else
    /* The FFA uses the PMIC for Voltage translation */
    #undef FEATURE_UIM_USE_ONLY_VREG_ON_PMIC
    #define  FEATURE_UIM_PMIC_ON_UIM1
  #endif /* !defined (T_FFA) */

  #ifdef FEATURE_GSTK
    #define FEATURE_GSTK_REMOTE_TIME
  #endif /* FEATURE_GSTK */

  #ifdef FEATURE_UIM_RUIM
    #define FEATURE_RUIM_CDMA_REFRESH
  #endif /* FEATURE_UIM_RUIM */

#endif /* T_MSM8650 */

#endif /* CUSTUIM_HW_8650_H */

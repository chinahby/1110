#ifndef CUSTUIM_HW_8200_H
  #define CUSTUIM_HW_8200_H
/*===========================================================================

            " C U S T -  U I M - H W - 8 2 0 0 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for UIM hardware specifics for MDM8200.  Please note that
  the 8200 Cofniguration is controlled by the T_MSDM8200 Marco definition

  Copyright (c) 2009 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/su/cust/rel/07H1_2/custuim_hw_8200.h#2 $ $DateTime: 2009/05/06 11:20:59 $ $Author: xiaominz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   xz      define FEATURE_GSTK_REMOTE_TIME
02/25/09   yk      Initial Revision for 8200
===========================================================================*/

  /*
   *  Features specific to T_MDM8200 ONLY.
   */
  #if defined(T_MDM8200)

    /* Feature to enable MDM8200 platform specific driver code */
    #define FEATURE_UIM_MDM8200_DRIVER

    /* Feature to indicate that the source to the UART is TCXO */
    #define FEATURE_UIM_TCXO_CLK_FREQ

    /* Feature to indicate that the clock to the card is TCXO/5 */
    #define FEATURE_UIM_USE_TCXO_DIV_5

    /*
     *  The 8200 SURF and FFA Reference Design Does not use the PMIC for Voltage
     *  Translation. It makes use of the ability to RAIL Different Voltages to
     *  the MSM UIM GPIOs
     */
    #define FEATURE_UIM_USE_ONLY_VREG_ON_PMIC
    #undef  FEATURE_UIM_PMIC_ON_UIM1
    #undef  FEATURE_UIM_PMIC_ON_UIM2
    
    #ifdef FEATURE_GSTK
      #define FEATURE_GSTK_REMOTE_TIME
    #endif /* FEATURE_GSTK */

  #else
    /*
     *  Incorrect Macro Definitions for this target
     */
    #error "Incorrect Target Macro Definitions"

  #endif /* T_MDM8200 */

#endif /* CUSTUIM_HW_8200_H */

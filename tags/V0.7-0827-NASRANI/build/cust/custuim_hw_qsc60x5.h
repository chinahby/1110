#ifndef CUSTUIM_HW_QSC60X5_H
#define CUSTUIM_HW_QSC60X5_H
/*===========================================================================

            " C u s t -  U I M - Hw - QSC60X5 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for UIM hardware specifics for QSC60X5.

  Copyright (c) 2008 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/08   rm     Initial revision.

===========================================================================*/

/*
 *  Features specific to T_QSC60X5 ONLY
 */

#ifdef T_QSC60X5
  #define FEATURE_UIM_USE_ONLY_VREG_ON_PMIC
  #undef  FEATURE_UIM_PMIC_ON_UIM1
  #undef  FEATURE_UIM_PMIC_ON_UIM2
  #undef  FEATURE_UIM_USES_OLD_PMIC_API
#endif /* T_QSC60X5 */

#endif /* CUSTUIM_HW_QSC60X5_H */

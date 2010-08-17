#ifndef CUSTUIM_HW_7225_H
#define CUSTUIM_HW_7225_H
/*===========================================================================

            " C U S T -  U I M - H W - 7 2 2 5 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for UIM hardware specifics for MSM7225.  Please note that
  the 7225 Cofniguration is controlled by the T_MSM7625 Marco definition

  Copyright (c) 2008 - 2009 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   xz      define FEATURE_GSTK_REMOTE_TIME
6/10/08    jar     Initial Revision for 7225

===========================================================================*/

/*
 *  Features specific to T_MSM7225 ONLY.
 *  Note there are distinctions made between a SURF and FFA
 */

#if defined(T_MSM7625) && defined (T_FFA)
#error code not present
#elif defined(T_MSM7625) && !defined (T_FFA)
  /*
   *  The Surf Reference Design Requires the use of a PMIC for
   *  Voltage translation.
   */
  #undef   FEATURE_UIM_USE_ONLY_VREG_ON_PMIC
  #define  FEATURE_UIM_PMIC_ON_UIM1
  #undef   FEATURE_UIM_PMIC_ON_UIM2

  #ifdef FEATURE_GSTK
    #define FEATURE_GSTK_REMOTE_TIME
  #endif /* FEATURE_GSTK */

#else
  /*
   *  Incorrect Macro Definitions for this target
   */
  #error "Incorrect Target Macro Definitions"

#endif /* T_MSM7625 && T_FFA */

#endif /* CUSTUIM_HW_7225_H */


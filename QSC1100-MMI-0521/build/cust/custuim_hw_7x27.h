#ifndef CUSTUIM_HW_7X27_H
  #define CUSTUIM_HW_7X27_H
/*===========================================================================

            " C U S T -  U I M - H W - 7 X 2 5 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for UIM hardware specifics for MSM7x27.  Please note that
  the 7x27 Cofniguration is controlled by the T_MSM7x27 Marco definition

  Copyright (c) 2009 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/su/cust/rel/07H1_2/custuim_hw_7x27.h#5 $ $DateTime: 2009/05/07 17:29:43 $ $Author: jkomenda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/09   jk      Added FEATURE_UIM_USES_NEW_SLEEP_API
05/04/09   xz      define FEATURE_GSTK_REMOTE_TIME
01/23/09   yk      Fixed compilation error
01/06/09   yk      Initial Revision for 7x27

===========================================================================*/

  /*
   *  Features specific to T_MSM7x27 ONLY.
   */
  #if defined(T_MSM76XXT)
#error code not present
  #else
    /*
     *  Incorrect Macro Definitions for this target
     */
    #error "Incorrect Target Macro Definitions"

  #endif /* T_MSM7X25 */

#endif /* CUSTUIM_HW_7X27_H */


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  LPM QDSP Driver Interface for MSM6550

GENERAL DESCRIPTION
  This module contains functions of LPM driver for the QDSP

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  See header file for more details

Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camera/6k/snapshotprocess/lpm/main/latest/src/lpmqdsp.c#1 $ $Author: c_caluru $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/07   arv     Modified the firmware app id to QDSP_APP_JPEGE
05/24/06   dl      Modified the firmware image name to JPEGE to make it 
                   uniform across all targets    
02/28/06   dl      Resolved lint issues.
09/01/05   dl      Removed lint warnings.
07/21/05   dl      Change the command buffer size to number of words. 
07/06/05   dl      Added the following feature flags 
                   FEATURE_CAMERA_5x5_ASF
05/26/05   dl      Added 5x5 asf support
05/17/05   dl      Define LPM ony if camera is defined.
03/10/05   dl      Removed reading lpm state variable before dsp download.
                   Used correct dwords value as start cmd buffer size.
03/04/05   dl      Initial version.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                       INCLUDE FILES FOR MODULE

                
===========================================================================*/
#include "clk.h"             /* Clock services         */
#include "lpmqdsp.h"         /* LPMQDSP driver typedefs and prototype      */
#include "msg.h"             /* Message logging/reporting services         */
#include "qdsp.h"            /* QDSP services                              */
#include "qdspext.h"

#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif /* FEATURE_CAMERA_LPM */


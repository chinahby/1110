#ifndef LPM_H
#define LPM_H

/*=============================================================================
FILE:       lpm.h

SERVICES:   LUMA PROCESSING MODULE INTERFACE

DESCRIPTION:
   This file provides the necessary interfaces for the LPM. 

LIMITATIONS:
   

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/camera/6k/snapshotprocess/lpm/main/latest/inc/lpm.h#1 $ 
 $DateTime: 2008/05/09 16:40:03 $ $Author: karenp $

=============================================================================*/
/* <EJECT> */
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/19/06   dl      Moved luma adaptation to service layer.
08/04/06   dl      Moved lpm parameters from camsensor to chromatix.
06/06/06   dl      Move color subsample from info to data structure.
06/04/06   dl      Added Luma Adaptation.
03/23/06   dl      Added Chroma Suppression.
11/03/05   anb     Added LPM_PROCESS_START to indicate that the LPM process
                   will start.
10/27/05   dl      Renamed lpm_terminate to lpm_abort. Removed callback pointer
                   requirment for abort.
07/06/05   dl      Added the following feature flags 
                   FEATURE_CAMERA_LVCM
                   FEATURE_CAMERA_5x5_ASF
05/26/05   dl      Added 5x5 asf support
05/17/05   dl      Define LPM ony if camera is defined.
07/08/04   dl      Initial version

=============================================================================*/


/*-----------------------------------------------------------------------------
      Include Files
-----------------------------------------------------------------------------*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "lpmqdsp.h"
#include "camsensor.h"

#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif /* FEATURE_CAMERA_LPM */

#endif /* LPM_H */


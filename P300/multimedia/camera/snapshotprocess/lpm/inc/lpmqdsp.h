#ifndef LPMQDSP_H
#define LPMQDSP_H

/*=============================================================================
FILE:       LPMQDSP.h

SERVICES:   LPM QDSP Driver Interface

DESCRIPTION:
   This file provides necessary interfaces for the LPM to interface with
   QDSP.
   
LIMITATIONS:
   lpmqdsp_initialize() must be called first.

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/camera/6k/snapshotprocess/lpm/main/latest/inc/lpmqdsp.h#1 $ $DateTime: 2008/05/16 02:04:09 $ $Author: c_caluru $

============================================================================*/
/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/06/05   dl      Added FEATURE_CAMERA_LPM feature flag.
05/26/05   dl      Added 5x5 asf support
05/17/05   dl      Define LPM ony if camera is defined.
03/10/05   dl      Added input and output data description.
03/04/05   dl      Initial version.

===========================================================================*/

/*---------------------------------------------------------------------------
      Include Files
----------------------------------------------------------------------------*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "camsensor.h"

#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif /* FEATURE_CAMERA_LPM */

#endif /* LPMQDSP_H */


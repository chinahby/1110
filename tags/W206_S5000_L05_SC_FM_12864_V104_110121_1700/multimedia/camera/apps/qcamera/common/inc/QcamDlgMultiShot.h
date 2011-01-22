#ifndef QCAMDLGMULTISHOT_H
#define QCAMDLGMULTISHOT_H

/*=============================================================================

FILE: QCamDlgMultiShot.h

SERVICES: QCamera Dialog

GENERAL DESCRIPTION: Multishot Setting Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2006 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/inc/QcamDlgMultiShot.h#1 $
$DateTime: 2008/05/12 10:09:09 $
$Author: karenp $
$Change: 657887 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
04/17/06   NC      Created QCamDlgMultishot Header File

=============================================================================*/
#include "AEE.h"

/*===========================================================================

CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: QCamDlgMultiShot_EventHandler

DESCRIPTION: Handles Set Size Dialog's events

PARAMETERS:
  *pUser: QCam object pointer
  evt: event code
  w: word event param
  dw: dword event param

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean QCamDlgMultiShot_EventHandler(void *pUser, AEEEvent evt, 
                                      uint16 w, uint32 dw);


#endif /* End of QCAMDLGMULTISHOT_H */

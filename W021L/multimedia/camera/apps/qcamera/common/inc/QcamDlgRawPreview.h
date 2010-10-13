#ifndef QCAMDLGRAWPREVIEW_H
#define QCAMDLGRAWPREVIEW_H

/*=============================================================================

FILE: QCamDlgRawPreview.h

SERVICES: QCamera Dialog

GENERAL DESCRIPTION: Raw Preview Setting Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2006 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/inc/QcamDlgRawPreview.h#1 $
$DateTime: 2008/12/18 21:50:16 $
$Author: kgelli $
$Change: 808978 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
04/17/06   KC      Created QcamDlgRawPreview Header File

=============================================================================*/
#include "AEE.h"

/*===========================================================================

CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: QcamDlgRawPreview_EventHandler

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
boolean QCamDlgRawPreview_EventHandler(void *pUser, AEEEvent evt, 
                                      uint16 w, uint32 dw);


#endif /* End of QCAMDLGRAWPREVIEW_H */

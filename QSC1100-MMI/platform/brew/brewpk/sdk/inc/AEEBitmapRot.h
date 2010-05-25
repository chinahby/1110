#ifndef  AEEBITMAPROT_H
#define  AEEBITMAPROT_H
/*=================================================================================
FILE:          AEEBitmapRot.h

SERVICES:      AEE Device bitmap specific interface

DESCRIPTION:   This file provides definitions for the bitmap rotation interfaces 
made available by the AEE to application developers  This is a standard header
file that must be included by all applications using IBitmapRot interface of AEE.

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEEQueryInterface.h"


/*---------------------------------------------------------------------------------
      Interface IDs
---------------------------------------------------------------------------------*/

// IBitmapRot 
#define AEEIID_BITMAPROT       0x0103e8d9


/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/



//*********************************************************************************
//
// IBitmapRot Interface
//
//*********************************************************************************


typedef struct IBitmapRot IBitmapRot;

#define INHERIT_IBitmapRot(iname) \
   INHERIT_IQueryInterface(iname); \
   int      (*GetRotation)          (iname *po)


AEEINTERFACE(IBitmapRot)
{
   INHERIT_IBitmapRot(IBitmapRot);
};


// Access macros for IBitmapRot
#define  IBITMAPROT_AddRef(p)                    AEEGETPVTBL(p,IBitmapRot)->AddRef(p)
#define  IBITMAPROT_Release(p)                   AEEGETPVTBL(p,IBitmapRot)->Release(p)
#define  IBITMAPROT_QueryInterface(p,i,p2)       AEEGETPVTBL(p,IBitmapRot)->QueryInterface(p,i,p2)
#define  IBITMAPROT_GetRotation(p)               AEEGETPVTBL(p,IBitmapRot)->GetRotation(p)



/*=================================================================================
   INTERFACES DOCUMENTATION
===================================================================================

IBitmapRot Interface

Description:
   This interface is used to query the rotation of bitmaps.  It
   is obtained by calling a bitmap's QueryInterface method 
   with an interface ID of AEEIID_BITMAPROT.

The following header file is required:~
AEEBitmap.h

==============================================================================
Function: IBITMAPROT_AddRef()

Description:
	This function is inherited from IQI_AddRef().

==============================================================================
Function: IBITMAPROT_Release()

Description:
	This function is inherited from IQI_Release().

==============================================================================
Function: IBITMAPROT_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().
	
==============================================================================
Function: IBITMAPROT_GetRotation()

Description:
   This function gets the current rotation of the bitmap relative to 
   the default. 

Prototype:
   int IBITMAPROT_GetRotation(IBitmapRot *po);

Parameters:
   po       [in]: Pointer to IBitmapRot interface.

Return Value:
   The number of degrees that the device bitmap has been rotated relative to 
   the default. Valid values are 0, 90, 180 and 270 degrees. They represent 
   the number of degrees clockwise that a bitmap has been rotated relative to 
   the default(0 degrees). For example, if the default display is portrait and
   the application rotates clock wise to landscape mode then a call to 
   IBITMAPROT_GetRotation will return 90. If the display were rotated 
   counter-clockwise to landscape mode, a call to IBITMAPROT_GetRotation will 
   return 270 degrees. 

Comments:
   

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IDISPLAY_SetPrefs

==============================================================================*/
#endif /* AEEBITMAPROT_H */


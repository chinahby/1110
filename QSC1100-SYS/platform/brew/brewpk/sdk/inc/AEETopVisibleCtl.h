#ifndef AEETOPVISIBLECTL_H
#define AEETOPVISIBLECTL_H

/*============================================================================
FILE: AEETopVisibleCtl.h

SERVICES:  BREW ITopVisible Acess Control Services

DESCRIPTION:
   This file defines ITopVisibleCtl interface that allows BREW applications to
   acquire and relinquish access control over resources.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/
#include "AEE.h"
#include "AEEResourceCtl.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//=============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//=============================================================================

//-------------------------------------------------------------------
// Interface Definitions - C Style
//-------------------------------------------------------------------

//*****************************************************************************
//
// ITopVisibleCtl Interface
//
//*****************************************************************************

#define INHERIT_ITopVisibleCtl(iname) \
   INHERIT_IResourceCtl(iname); \
   int   (*GetArgs)(iname *po, char *pszArgs, int *pnSize); \
   int   (*SetArgs)(iname *po, const char *pszArgs, int pnSize)
 
AEEINTERFACE_DEFINE(ITopVisibleCtl);

#define ITOPVISIBLECTL_AddRef(p)                        AEEGETPVTBL(p, ITopVisibleCtl)->AddRef(p)
#define ITOPVISIBLECTL_Release(p)                       AEEGETPVTBL(p, ITopVisibleCtl)->Release(p)
#define ITOPVISIBLECTL_QueryInterface(p, i, p2)         AEEGETPVTBL(p, ITopVisibleCtl)->QueryInterface(p, i, p2)

#define ITOPVISIBLECTL_Acquire(p, n, pdw)               AEEGETPVTBL(p, ITopVisibleCtl)->Request(p, n, pdw)
#define ITOPVISIBLECTL_CanAcquire(p, n, pdw)            AEEGETPVTBL(p, ITopVisibleCtl)->CanAcquire(p, n, pdw)
#define ITOPVISIBLECTL_Free(p, pdw)                     AEEGETPVTBL(p, ITopVisibleCtl)->Request(p, RESCTL_REASON_RELINQUISH, pdw)
#define ITOPVISIBLECTL_SetRelinquishCtl(p, pCls, n)     AEEGETPVTBL(p, ITopVisibleCtl)->SetRelinquishCtl(p, pCls, n)
#define ITOPVISIBLECTL_GetStatus(p, pdw)                AEEGETPVTBL(p, ITopVisibleCtl)->GetStatus(p, pdw)
#define ITOPVISIBLECTL_OnStatusChangeNotify(p, dw, pcb) AEEGETPVTBL(p, ITopVisibleCtl)->OnStatusChangeNotify(p, dw, pcb)
#define ITOPVISIBLECTL_GetArgs(p, psz, pn)              AEEGETPVTBL(p, ITopVisibleCtl)->GetArgs(p, psz, pn)
#define ITOPVISIBLECTL_SetArgs(p, psz, pn)              AEEGETPVTBL(p, ITopVisibleCtl)->SetArgs(p, psz, pn)

#define ITOPVISIBLECTL_SetRelinquishCtlAll(p)           ITopVisibleCTL_SetRelinquishCtl(p, NULL, -1)
#define ITOPVISIBLECTL_SetRelinquishCtlNone(p)          ITopVisibleCTL_SetRelinquishCtl(p, NULL, 0)

/*=======================================================================
  DATA STRUCTURE DOCUMENTATION
=========================================================================

=========================================================================
  INTERFACE DOCUMENTATION
=========================================================================

ITopVisibleCtl Interface

Description:
   The ITopVisibleCtl interface is used to control the ownership of a resource
   which must be limited to a single owner at a time.

   Each managed resource implements a version of this interface.  From a
   managed resource, one can use IQI_QueryInterface 
=============================================================================
Function: ITOPVISIBLECTL_AddRef()

Description:
This function is inherited from IQI_AddRef().

==============================================================================
Function: ITOPVISIBLECTL_Release()

Description:
This function is inherited from IQI_Release().

==============================================================================
Function: ITOPVISIBLECTL_QueryInterface()

Description:
This function is inherited from IQI_QueryInterface().

==============================================================================
Function: ITOPVISIBLECTL_Acquire()

Description:
This function is inherited from IRESOURCECTL_Acquire().

==============================================================================
Function: ITOPVISIBLECTL_CanAcquire()

Description:
This function is inherited from IRESOURCECTL_CanAcquire().

==============================================================================
Function: ITOPVISIBLECTL_Free()

Description:
This function is inherited from IRESOURCECTL_Free().

==============================================================================
Function: ITOPVISIBLECTL_SetRelinquishCtl()

Description:
This function is inherited from IRESOURCECTL_SetRelinquishCtl().

==============================================================================
Function: ITOPVISIBLECTL_GetStatus()

Description:
This function is inherited from IRESOURCECTL_GetStatus().

==============================================================================
Function: ITOPVISIBLECTL_OnStatusChangeNotify()

Description:
This function is inherited from IRESOURCECTL_OnStatusChangeNotify().

==============================================================================
Function: ITOPVISIBLECTL_SetArgs()

Description:
   This function is called by BREW to set Args property in ITopVisibleCtl
   so it later can be retrieved by OEMResArbiter.
   

Prototype:
   int ITOPVISIBLECTL_SetArgs(ITopVisibleCtl * po, const char *pszArgs, int pnSize)

Parameters:
   po             [in]: Pointer to ITOPVISIBLECTL interface.
   pszArgs        [in]: Null terminated string containing arguments.
   pnSize        [in]: Length of the string containing arguments.

Return Value:
   SUCCESS: if there's enough memory to allocate a copy of a passed string.

Comments:
   pszSize does not count terminating 0.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ITOPVISIBLECTL_GetArgs()

==============================================================================
Function: ITOPVISIBLECTL_GetArgs()

Description:
   This function can be called by OEM to get Args property in ITopVisibleCtl
   set by BREW when an applet was started by CShell_StartApplet or
   CShell_WakeStart or IAPPLETCTL_Start.
   

Prototype:
   int ITOPVISIBLECTL_GetArgs(ITopVisibleCtl * po, char *pszArgs, int *pnSize)

Parameters:
   po             [in]: Pointer to ITOPVISIBLECTL interface.
   pszArgs        [in]: Pointer to a buffer into which to copy arguments.
   pnSize    [in/out]: Max number of characters to place in the above buffer.

Return Value:
   SUCCESS: if arguments are correct and there's enough memory to allocate
   a copy of a passed string.
   ENOMEMORY: if the buffer is too small for the value.

Comments:
   pnSize cannot be NULL.
   If pszArgs is NULL pnSize contains buffer size to accommodate pszArgs.
   pszArgs is a null-terminated string and pszSize does count null character.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ITOPVISIBLECTL_SetArgs()

==============================================================================*/

#endif // AEETOPVISIBLEECTL_H

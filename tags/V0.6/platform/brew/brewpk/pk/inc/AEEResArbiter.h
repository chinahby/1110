#ifndef AEERESARBITER_H
#define AEERESARBITER_H

/*============================================================================
FILE: AEEResArbiter.h

SERVICES:  BREW Resource Arbiter Services

DESCRIPTION:
   This file defines the Resource Arbiter Interface.  The Arbiter
   allows an OEM to customize arbitration for resource contention
   among competing objects.
   between applications

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  BREW *MUST* be running in the
application context of the requestor before calling IRESARBITER_ConfirmAcquire.
This is necessary for performing valid privilege checks.

        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

#define AEECLSID_RESARBITER   0x101e52d

typedef struct IResArbiter    IResArbiter;

typedef struct AEEResCtlInfo
{
   AEECLSID       clsId;         // owner/requester CLS ID
   IResourceCtl * pCtl;          // IResourceCtl object
   uint32         dwReason;      // request reason (RESCTL_REASON)
   AEECLSID    *  pClsList;      // pointer to array of class & group ids
                                 // that owner is willing to relinquish to
   int            nClsCount;     // count of ids on pClsList.
} AEEResCtlInfo;

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================

// Temp location until IResourceCtl files are revamped for derived class implementations.
extern int IResourceCtl_New(IShell * ps, AEECLSID cls, IBase * pQIObject, void **ppif);

//-------------------------------------------------------------------
// Interface Definitions - C Style
//-------------------------------------------------------------------


AEEINTERFACE(IResArbiter)
{
   INHERIT_IQueryInterface(IResArbiter);

   int   (*ConfirmAcquire)(IResArbiter * po,
                           AEECLSID resCtlId,
                           AEEResCtlInfo * pOwner,
                           AEEResCtlInfo * pRequester);
};

#define IRESARBITER_AddRef(p)                   AEEGETPVTBL(p, IResArbiter)->AddRef(p)
#define IRESARBITER_Release(p)                  AEEGETPVTBL(p, IResArbiter)->Release(p)
#define IRESARBITER_QueryInterface(p,i,p2)      AEEGETPVTBL(p, IResArbiter)->QueryInterface(p,i,p2)
#define IRESARBITER_ConfirmAcquire(p,id,po,pr)  AEEGETPVTBL(p, IResArbiter)->ConfirmAcquire(p,id,po,pr)

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

===================================================================== 
  INTERFACES   DOCUMENTATION
===================================================================== 

IResArbiter Interface

Description:
   The IResArbiter is an OEM/Carrier customizable extension which 
   allows custom arbitration decisions for transfering ownership
   of a resource.
   
   For resources which implement Resource Management, the 
   IRESARBITER_ConfirmAcquire function is called when there is
   contention for a resource: i.e. one entity owns the resource
   and a second entity is requesting to use it.  Examples for these
   resources include ICAMERA, ...

See Also:
   IResourceCtl
   
==============================================================================
Function: IRESARBITER_AddRef()

This function is inherited from IQI_AddRef().

==============================================================================
Function: IRESARBITER_Release()

This function is inherited from IQI_Release().

==============================================================================
Function: IRESARBITER_QueryInterface()

This function is inherited from IQI_QueryInterface().


==============================================================================
Function: IRESARBITER_ConfirmAcquire()

Description:
   This function is called when a resource with a resource control is
   busy and a request for the object is made.

Prototype:
   int   IRESARBITER_ConfirmAcquire)(IResArbiter * po, AEECLSID resCtlId,
                           AEEResCtlInfo * pOwner, AEEResCtlInfo * pRequester);

Parameters:
   po             [in]: Pointer to IRESOURCECTL interface.
   resCtlId       [in]: Resource Ctl ID of resource being requested
   pOwner         [in]: Information about current owner of resource
   pRequestor     [in]: Information about requestor
   
Return Value:
   SUCCESS if requestor is granted access to the resource.  it is up to the
   calling ResourceCtl to perform whatever action is required to change
   ownership of the resource.

Comments:  
   While there is a default implementation, an OEM or carrier may change
   this implementation to make any arbitrary determination for granting
   access..

Side Effects: 
   None

Version:
   Introduced BREW SDK 3.1

See Also:
   
============================================================================= */

#endif // AEERESARBITER_H


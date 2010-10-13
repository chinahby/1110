#ifndef AEEIRESOURCECTL_H
#define AEEIRESOURCECTL_H

/*=====================================================================
FILE: AEEIResourceCtl.h

SERVICES: BREW Resource Access Control Services

DESCRIPTION:    
   This file defines IResourceCtl interface that allows BREW applications to
   acquire and relinquish access control over resources.

Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================================*/
#include "AEEIQI.h"
#include "AEECallback.h"

#define AEEIID_IResourceCtl       0x101e52e   // IResourceCtl interface

//
// Status notification sent to client by IResourceCtl.
//
#define RESCTL_STATUS_FREE             0x00000001  // Resource is available
#define RESCTL_STATUS_ACQUIRED         0x00000002  // Resource is acquired by this client
#define RESCTL_STATUS_BUSY             0x00000004  // Resource is acquired by some other client   
#define RESCTL_STATUS_ALL              0x0000000F  // Mask for all the above status
#define RESCTL_STATUS_USER_BASE        0x00010000  // Start of user defined status codes

//
// Reason given by client to acquire a resource.
//
// NOTE: Users (OEMs) can define their own reason codes
//       by fetching new class IDs (as group IDs) from 
//       BREW Developer Extranet.
#define RESCTL_REASON_BASE             0x1000
#define RESCTL_REASON_RELINQUISH       (RESCTL_REASON_BASE + 0)
#define RESCTL_REASON_ANY              (RESCTL_REASON_BASE + 1)
#define RESCTL_REASON_MT_VOICE_CALL    (RESCTL_REASON_BASE + 2)
#define RESCTL_REASON_MO_VOICE_CALL    (RESCTL_REASON_BASE + 3)
#define RESCTL_REASON_MT_DATA_CALL     (RESCTL_REASON_BASE + 4)
#define RESCTL_REASON_MO_DATA_CALL     (RESCTL_REASON_BASE + 5)
#define RESCTL_REASON_MT_SMS           (RESCTL_REASON_BASE + 6)
#define RESCTL_REASON_MO_SMS           (RESCTL_REASON_BASE + 7)
#define RESCTL_REASON_PIM              (RESCTL_REASON_BASE + 8)
#define RESCTL_REASON_GAME             (RESCTL_REASON_BASE + 9)
#define RESCTL_REASON_MEDIA_PLAYBACK   (RESCTL_REASON_BASE + 10)
#define RESCTL_REASON_MEDIA_RECORD     (RESCTL_REASON_BASE + 11)
#define RESCTL_REASON_VOLUME_CONTROL   (RESCTL_REASON_BASE + 12)
#define RESCTL_REASON_AUDIO_DEVICE     (RESCTL_REASON_BASE + 13)
#define RESCTL_REASON_BUSY             (RESCTL_REASON_BASE + 14)

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
// IResourceCtl Interface
//
//*****************************************************************************

#define INHERIT_IResourceCtl(iname) \
   INHERIT_IQI(iname); \
   int      (*Request)(iname * po, uint32 dwReason, uint32 * pdwStatus); \
   int      (*CanAcquire)(iname * po, uint32 dwReason, uint32 * pdwStatus); \
   int      (*SetRelinquishCtl)(iname * po, AEECLSID * pClsList, int nCount);\
   uint32   (*GetStatus)(iname * po, uint32 * pdwReason); \
   int      (*OnStatusChangeNotify)(iname * po, uint32 dwStatusMask, AEECallback * pcb)

AEEINTERFACE_DEFINE(IResourceCtl);

static __inline uint32 IResourceCtl_AddRef(IResourceCtl *p)
{
   return AEEGETPVTBL(p, IResourceCtl)->AddRef(p);
}
static __inline uint32 IResourceCtl_Release(IResourceCtl *p)
{
   return AEEGETPVTBL(p, IResourceCtl)->Release(p);
}
static __inline int IResourceCtl_QueryInterface(IResourceCtl *p, AEEIID i, void **p2)
{
   return AEEGETPVTBL(p, IResourceCtl)->QueryInterface(p, i, p2);
}

static __inline int IResourceCtl_Acquire(IResourceCtl *p, uint32 n, uint32 *pdw)
{
   return AEEGETPVTBL(p, IResourceCtl)->Request(p, n, pdw);
}
static __inline int IResourceCtl_CanAcquire(IResourceCtl *p, uint32 n, uint32 *pdw)
{
   return AEEGETPVTBL(p, IResourceCtl)->CanAcquire(p, n, pdw);
}
static __inline int IResourceCtl_Free(IResourceCtl *p, uint32 *pdw)
{
   return AEEGETPVTBL(p, IResourceCtl)->Request(p, RESCTL_REASON_RELINQUISH, pdw);
}
static __inline int IResourceCtl_SetRelinquishCtl(IResourceCtl *p, AEECLSID *pCls, int n)
{
   return AEEGETPVTBL(p, IResourceCtl)->SetRelinquishCtl(p, pCls, n);
}
static __inline uint32 IResourceCtl_GetStatus(IResourceCtl *p, uint32 *pdw)
{
   return AEEGETPVTBL(p, IResourceCtl)->GetStatus(p, pdw);
}
static __inline int IResourceCtl_OnStatusChangeNotify(IResourceCtl *p, uint32 dw, AEECallback *pcb)
{
   return AEEGETPVTBL(p, IResourceCtl)->OnStatusChangeNotify(p, dw, pcb);
}

static __inline int IResourceCtl_SetRelinquishCtlAll(IResourceCtl *p)
{
   return AEEGETPVTBL(p, IResourceCtl)->SetRelinquishCtl(p, NULL, -1);
}
static __inline int IResourceCtl_SetRelinquishCtlNone(IResourceCtl *p)
{
   return AEEGETPVTBL(p, IResourceCtl)->SetRelinquishCtl(p, NULL, 0);
}

/*=======================================================================
  DATA STRUCTURE DOCUMENTATION
=========================================================================

=========================================================================
  INTERFACE DOCUMENTATION
=========================================================================

IResourceCtl Interface

Description:
   The IResourceCtl interface is used to control the ownership of a resource
   which must be limited to a single owner at a time.

   Each managed resource implements a version of this interface.  From a
   managed resource, one can use IQI_QueryInterface 
=============================================================================
Resource Control reasons

These reasons specify why a requestor needs use the resource.
   
RESCTL_REASON_RELINQUISH : Owner does not need the resource and wishes to release it

RESCTL_REASON_ANY : No specific reason for resource specified

RESCTL_REASON_MT_VOICE_CALL : Resource is needed for a mobile terminated voice call

RESCTL_REASON_MO_VOICE_CALL : Resource is needed for a mobile originated voice call

RESCTL_REASON_MT_DATA_CALL : Resource is needed for a mobile terminated data call

RESCTL_REASON_MO_DATA_CALL : Resource is needed for a mobile originated data call

RESCTL_REASON_MT_SMS : Resource is needed for mobile terminated SMS

RESCTL_REASON_MO_SMS : Resource is needed for mobile originated SMS

RESCTL_REASON_PIM : Resource is needed for PIM application

RESCTL_REASON_GAME : : Resource is needed for a game application

RESCTL_REASON_MEDIA_PLAYBACK : Resource is needed for media playbay

RESCTL_REASON_MEDIA_RECORD : Resource is needed for media recording

RESCTL_REASON_VOLUME_CONTROL : Resource is needed for adjusting volume

RESCTL_REASON_AUDIO_DEVICE : Resource is needed for audio playback

RESCTL_REASON_BUSY : Current owner needs resource to finish a key operation.  This reason should be given for only a short duration.
   
==============================================================================
Function: IResourceCtl_AddRef()

This function is inherited from IQI_AddRef().

==============================================================================
Function: IResourceCtl_Release()

This function is inherited from IQI_Release().

==============================================================================
Function: IResourceCtl_QueryInterface()

This function is inherited from IQI_QueryInterface().

==============================================================================
Function: IResourceCtl_Acquire()

Description:
   This function is used to acquire control of the resource.
   It is also used to update the reason for holding the reason, if the
   reason changes.

Prototype:
   int IResourceCtl_Acquire(IResourceCtl * po, uint32 dwReason, uint32 * pdwStatus)

Parameters:
   po             [in]: Pointer to IRESOURCECTL interface.
   dwReason       [in]: Reason for requesting/holding resource.  This
                        can be either a RESCTL_REASON or a group/privilege id.
   pdwStatus      [out]: current RESCTL_STATUS of the resource.

Return Value:
   AEE_SUCCESS: if caller has acquired resource.
   Error code if not successful.

Comments:  
   The underlying Request function manages both the Acquire() and Free() functionality.
   If a dwReason of RESCTL_REASON_RELINQUISH is given, the resource is free'd.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IResourceCtl_Free()

==============================================================================
Function: IResourceCtl_Free()

Description:
   This function is used to release the resource.

Prototype:
   int IResourceCtl_Free(IResourceCtl * po, uint32 * pdwStatus)

Parameters:
   po             [in]: Pointer to IRESOURCECTL interface.
   pdwStatus      [out]: current RESCTL_STATUS of the resource.

Return Value:
   AEE_SUCCESS: if caller has acquired resource.
   Error code if not successful.

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IResourceCtl_Acquire()

==============================================================================
Function: IResourceCtl_CanAcquire()

Description:
   This function is used to query whether the caller may acquire control of
   the resource.
   It is also used to update the reason for holding the reason, if the
   reason changes.
   It is alwasy possible, however that the state of the resource may change
   at anytime such that a subsequent call to IResourceCtl_Acquire() may still
   fail.

Prototype:
   int IResourceCtl_CanAcquire(IResourceCtl * po, uint32 dwReason, uint32 * pdwStatus)

Parameters:
   po             [in]: Pointer to IRESOURCECTL interface.
   dwReason       [in]: Reason for requesting/holding resource.  This
                        can be either a RESCTL_REASON or a group/privilege id.
   pdwStatus      [out]: current RESCTL_STATUS of the resource.

Return Value:
   AEE_SUCCESS: if caller can acquire resource.
   Error code if not successful.

Comments:  
   The underlying Request function manages both the Acquire() and Free() functionality.
   If a dwReason of RESCTL_REASON_RELINQUISH is given, the resource is free'd.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IResourceCtl_Free()

==============================================================================
Function: IResourceCtl_SetRelinquishCtl()

Description:
   This function allows the caller to define a list of class Ids, group ids
   and/or RESCTL_REASONs for releasing the resource to another requestor.
   
   This is a privileged function and only applications which have the privilege
   associated with the resource may modify the Relinquish list.

   It is normally called before acquiring a resource, but it may be called
   at any time to update the Relinquish list.
   
Prototype:
   int IResourceCtl_SetRelinquishCtl(IResourceCtl * po, AEECLSID pClsList, int nCount )

Parameters:
   po             [in]: Pointer to IRESOURCECTL interface.
   pClsList       [in]: pointer to class/privilege/reason list. The list is copied.
   nCount         [in]: Number of elements on the pClsList.  Use -1 if anyone is allowed
                        to acquire the resource (default).  Use 0 if no one is allowed.
                        
Return Value:
   AEE_SUCCESS: if RelinqishCtl is successfully updated.
   Error code if not successful.

Comments:
   The relinquish list can be overridden by BREW.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IResourceCtl_Acquire()
   IRESOURCECTL_SetRelinqishCtlAll()
   IRESOURCECTL_SetRelinqishCtlNone()

==============================================================================
Function: IRESOURCECTL_SetRelinqishCtlAll()

Description:
   This function sets the relinquish control list to allow any requestor
   to acquire the resource.
   
   This is a privileged function and only applications which have the privilege
   associated with the resource may modify the Relinquish list.

   It is normally called before acquiring a resource, but it may be called
   at any time to update the Relinquish list.
   
Prototype:
   int IResourceCtl_SetRelinquishCtlAll(IResourceCtl * po)

Parameters:
   po             [in]: Pointer to IRESOURCECTL interface.
                        
Return Value:
   AEE_SUCCESS: if RelinqishCtl is successfully updated.
   Error code if not successful.

Comments:
   The relinquish list can be overridden by BREW.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IResourceCtl_Acquire()
   IRESOURCECTL_SetRelinqishCtl

==============================================================================
Function: IRESOURCECTL_SetRelinqishCtlNone()

Description:
   This function sets the relinquish control list to prohibit any requestor
   from acquiring the resource..
   
   This is a privileged function and only applications which have the privilege
   associated with the resource may modify the Relinquish list.

   It is normally called before acquiring a resource, but it may be called
   at any time to update the Relinquish list.
   
Prototype:
   int IResourceCtl_SetRelinquishCtlNone(IResourceCtl * po)

Parameters:
   po             [in]: Pointer to IRESOURCECTL interface.
                        
Return Value:
   AEE_SUCCESS: if RelinqishCtl is successfully updated.
   Error code if not successful.

Comments:
   The relinquish list can be overridden by BREW.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IResourceCtl_Acquire()
   IRESOURCECTL_SetRelinqishCtl()

==============================================================================
Function: IResourceCtl_GetStatus()

Description:
   This function returns the current ownership status of the resource.
   
Prototype:
   uint32 IResourceCtl_GetStatus(IResourceCtl * po, uint32 * pdwReason)

Parameters:
   po             [in]: Pointer to IRESOURCECTL interface.
   pdwReason      [out]: Points to last RESCTL_REASON_XXX specified
                  in IResourceCtl_Acquire() or IResourceCtl_CanAcquire()
                        
Return Value:
   current RESCTL_STATUS of resource.

Comments:
   None.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IResourceCtl_Acquire()
   IResourceCtl_CanAcquire()

==============================================================================
Function: IResourceCtl_OnStatusChangeNotify()

Description:
   This function allows the caller to receive a callback when the status of the
   resource changes.  The caller may specify what type of status changes triggers
   a callback.
   
Prototype:
   int IResourceCtl_OnStatusChangeNotify(IResourceCtl * po, uint32 dwStatusMask, AEECallback *pcb)

Parameters:
   po             [in]: Pointer to IRESOURCECTL interface.
   dwStatusMask   [in]: RESCTL_STATUS mask
   pcb            [in]: Pointer to AEECallback.
                        
Return Value:
   AEE_SUCCESS: callback successfully registered
   Error code if not successful.

Comments:
   The caller must re-register on each callback to continue receiving notifications.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:

============================================================================= */

#endif // AEEIRESOURCECTL_H

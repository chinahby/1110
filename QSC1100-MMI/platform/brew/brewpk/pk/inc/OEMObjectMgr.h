/*
  ============================================================================

FILE:  OEMObjectMgr.h

SERVICES: OEM ObjectMgr Services
       

GENERAL DESCRIPTION:
   	Definitions, typedefs, etc. for OEM ObjectMgr Service.

REVISION HISTORY: 

  ============================================================================
  ============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#ifndef OEMOBJECTMGR_H
#define OEMOBJECTMGR_H 

#include "AEE.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/*-------------------------------------------------------------------
      Macros Declarations
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
typedef struct AEEObjectMgr   AEEObjectMgr;
typedef uint32                AEEObjectHandle;

/*-------------------------------------------------------------------
      AEE-OEM interface
-------------------------------------------------------------------*/
//
// Global ObjectMgr:
// (1) Global ObjectMgr always exists. No need to create and release it.
// (2) Use following APIs to access the global ObjectMgr...
//
int      AEEObjectMgr_Register(void * pObj, AEEObjectHandle * phObj);
int      AEEObjectMgr_Unregister(AEEObjectHandle hObj);
void *   AEEObjectMgr_GetObject(AEEObjectHandle hObj);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================
AEEObjectHandle

Description:
Object handle returned by AEEObjectMgr_Register() API.

Definition:
typedef uint32 AEEObjectHandle

Members:
None

Comments:
Following registration, use the object handle to get the object pointer.

See Also:
None 

===================================================================================

=================================================================================
  INTERFACE DOCUMENTATION
=============================================================================
Interface Name: AEEObjectMgr

Description: 
   Object Manager
   (1) Manages the contexts of BREW objects (for example, objects created in app context
       and of finite lifetime) that participate in asynchronous operations of infinite 
       timeout
   (2) Facilitates the validation of the objects in asynchronous callbacks
===H2>
   Why is ObjectMgr needed?
===/H2>
   To illustrate the usage of ObjectMgr, consider the following situation:
   (1) An app creates IMedia-based object and calls IMEDIA_Play()
   (2) IMEDIA_Play() in OEM layer implementation calls lower-layer device 
       multimedia API that takes a callback function and user data to correlate
       the transaction. Assume we set user data to IMedia object pointer
   (3) The callback function gets fired when multimedia task sends events to BREW 
       in that task's context. BREW correlates the transaction, identifies the IMedia
       object, saves the event info and requests for a context switch.
   (4) When BREW gets scheduled, BREW processes the event info corresponding to 
       IMedia object and delivers the event to app
   If the app is unloaded just before step (3) occurs, then IMedia object pointer 
   returned in callback event in is invalid. Also, step (3) could occur anytime.
   
   ObjectMgr solves the problem as follows:
   
   When the IMedia object gets created, the OEM layer implementation must register 
   IMedia object with ObjectMgr. ObjectMgr saves object info and returns 
   an opaque object handle. This handle must be used as user data in
   asynchronous operations. In the callbacks, first query ObjectMgr with object 
   handle to obtain the object pointer. If the pointer is NULL, then the object 
   has been freed either by app or due to app unloading. In this case, drop the
   callback.
===H2>
   ObjectMgr API:
===/H2>
   AEEObjectMgr_Register():  Registers the BREW object and returns the AEEObjectHandle
   AEEObjectMgr_Unregister():Unregisters the BREW object and the object handle is
                           no more valid
   AEEObjectMgr_GetObject(): Given object handle, returns the object pointer

   Notes: 
   (1) ObjectMgr is never released. It is automatically released by BREW when
       BREW exits.

=============================================================================

Function: AEEObjectMgr_Register()

Description:
   This function registers a BREW object with the ObjectMgr. ObjectMgr
   returns an opaque handle to the caller.
  
Prototype:
   int AEEObjectMgr_Register(void * pObject, AEEObjectHandle * phObject);

Parameters:
   pObject: Object to be registered
   phObject: Handle to the object

Return Value:
   SUCCESS: ObjectMgr cannot allocate handle.
   Otherwise error.

Comments: 
   None

Side Effects: 
   None

See Also:
   None   
=======================================================================

Function: AEEObjectMgr_Unregister()

Description:
   This function unregisters the object and calls the caller-registered
   function, if any.
  
Prototype:
   int AEEObjectMgr_Unregister(AEEObjectHandle hObj);

Parameters:
   hObj: Handle of the object

Return Value:
   SUCCESS: Unregister succeeded
   Otherwise Unregister failed

Comments: 
   The handle should not be used after unregistration.

Side Effects: 
   None

See Also:
   None
=======================================================================

Function: AEEObjectMgr_GetObject()

Description:
   Given object handle, this function retrieves the object.
  
Prototype:
   void * AEEObjectMgr_GetObject(AEEObjectHandle hObj);

Parameters:
   hObj: Handle of the object

Return Value:
   NULL: Object does not exist
   Otherwise valid object pointer

Comments: 
   None.

Side Effects: 
   None

See Also:
   None
===========================================================================*/

#endif /* #ifndef OEMOBJECTMGR_H */

/*=============================================================================

FILE: SysModelBase.c

SERVICES:  SysModelBase implementation

GENERAL DESCRIPTION:
   The SysModelBase class implements an IModel interface that may be shared
   by multiple BREW applets. 


PUBLIC CLASSES AND STATIC FUNCTIONS:
   IModel

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/oem/Src/SysModelBase.c#4 $
$DateTime: 2008/03/11 13:46:26 $
  $Author: satishk $
  $Change: 623281 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/06/05   mjv     Correctly handle the canceling of any listener during a
                   notify cycle 
05/24/05   jas     Fixing include file case for Linux builds.
10/29/04   mjv     Cancel any remaining model listeners in the _Dtor, to 
                   prevent misbehaving listeners from crashing if they do a
                   LISTENER_Cancel() after the model is destructed.
06/09/04   tjc     Add NULL pointer check
06/01/04   mjv     Ported to latest Forms/Widget library
05/17/04   mjv     Added some more comments. 
04/30/04   mjv     SysModelBase now manually deallocates its system objects
                   when the associated applet stops listening to the Model,
                   instead of waiting for BREW to automatically deallocate
                   the object when the applet exits.  This prevents BREW 
                   from complaining about a "System Object Leak" whenever
                   the associated applet exits.
03/26/04   mjv     Clean up appList pointer on destruction
03/23/04   mjv     Initial revision

=============================================================================*/



/*===========================================================================

                    INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEE_OEMHeap.h"
#include "AEE_OEMDispatch.h"
#include "SysModelBase_priv.h"


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    FUNCTION IMPLEMENTATIONS

===========================================================================*/


/*=============================================================================
FUNCTION: SysModelBase_Ctor

DESCRIPTION:  Constructs a SysModelBase object 

PARAMETERS:
  *pMe     [in]: SysModelBase object to construct
  *pvt     [in]: IModel interface function table
   pDtor   [in]: Derived class's destructor

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SysModelBase_Ctor(SysModelBase         *pMe,
                       AEEVTBL(IModel)      *pvt,
                       PFNSYSMODELBASE_DTOR  pDtor)
{
   if (pMe->nRefs > 0) {
      // Already initialized.  Add a ref and return. 
      (void) pvt->AddRef(SYSMODELBASE_TO_IMODEL(pMe));
      return;
   }

   // Init the VTBL 
   pMe->pvt = pvt;
   pvt->AddRef         = SysModelBase_AddRef;
   pvt->Release        = SysModelBase_Release;
   pvt->QueryInterface = SysModelBase_QueryInterface;
   pvt->AddListener    = SysModelBase_AddListener;
   pvt->Notify         = SysModelBase_Notify;

   // Init the destructor
   if (pDtor) {
      pMe->pDtor = pDtor;        // Use supplied destructor
   } else {
      pMe->pDtor = SysModelBase_Dtor; // Use default destructor
   }

   // Init data members
   pMe->nRefs = 1;
   pMe->pNextNotifyApp = NULL;
   pMe->pNextNotifyListener = NULL;
}


/*=============================================================================
FUNCTION: SysModelBase_Dtor

DESCRIPTION:  Destructs a SysModelBase object 

PARAMETERS:
  *pMe  [in]: SysModelBase object to destruct

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SysModelBase_Dtor(SysModelBase *pMe)
{
   SysModelAppInfo *info, *oldInfo;

   // Release the app info
   info = pMe->appList;
   while (info != NULL) {

      // Cancel the callback.
      CALLBACK_Cancel(&info->cbSysObject);

      // Cancel all misbehaving listeners, the model is going away.
      {
         ModelListener *pListener = info->pListeners;
         while (pListener != NULL) {
            pListener->pfnCancel = NULL;
            pListener = pListener->pNext;
         }
      }

      oldInfo = info;
      info = info->pNext;

      // Free the memory
      (void) AEEHeap_Realloc(oldInfo, 0);
   }

   pMe->appList = NULL;
}


/*=============================================================================
FUNCTION: SysModelBase_AddRef

DESCRIPTION:  Implements the IMODEL_AddRef method 

PARAMETERS:
  *p  [in]:  IModel interface

RETURN VALUE:
   int:  The new reference count for the object

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 SysModelBase_AddRef(IModel *p)
{
   SysModelBase *pMe = IMODEL_TO_SYSMODELBASE(p);

   return ++pMe->nRefs;
}


/*=============================================================================
FUNCTION: SysModelBase_Release

DESCRIPTION:  Implements the IMODEL_Release method 

PARAMETERS:
  *p  [in]:  IModel interface

RETURN VALUE:
   int:  The new reference count for the object

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 SysModelBase_Release(IModel *p)
{
   SysModelBase *pMe = IMODEL_TO_SYSMODELBASE(p);

   if (pMe->nRefs > 0) {
      pMe->nRefs--;
   }

   if (0 == pMe->nRefs) {
      pMe->pDtor(pMe);
      return 0;
   }

   return pMe->nRefs;
}


/*=============================================================================
FUNCTION: SysModelBase_QueryInterface

DESCRIPTION:  Implements the IMODEL_QueryInterface method

PARAMETERS:
   *p     [in]:  IModel interface
    id    [in]:
  **ppif [out]:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int SysModelBase_QueryInterface(IModel *p, AEECLSID id, void **ppif)
{
   if (id == AEEIID_MODEL) {
      *ppif = p;
      IMODEL_AddRef(p);
      return 0;
   }
   return ECLASSNOTSUPPORT;
}


/*=============================================================================
FUNCTION: SysModelBase_AddListener

DESCRIPTION:  Implements the IMODEL_AddListener method

PARAMETERS:
   *p          [in]: IModel interface
   *pListener  [in]:

RETURN VALUE: 
   int

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int SysModelBase_AddListener(IModel *p, ModelListener *pListener)
{
   SysModelBase    *pMe = IMODEL_TO_SYSMODELBASE(p);
   SysModelAppInfo *info;

   if (pListener == NULL) {
      return EBADPARM;
   }

   // Find the app info for the current app context
   // (this will also create the app info if it doesn't already exist)
   info = SysModelBase_GetAppInfo(pMe);
   if (NULL == info) {
      DBGPRINTF_ERROR("Unable to get app info");
      return ENOMEMORY;
   }
   
   // Cancel listener if already registered
   if (LISTENER_IsRegistered(pListener)) {
      LISTENER_Cancel(pListener);
   }

   // Setup the cancel callback 
   pListener->pfnCancel = SysModelBase_CancelListener;
   pListener->pCancelData = info;

   pListener->pNext = pListener->pPrev = 0;

   if (NULL == info->pListeners) {
      // New list
      info->pListeners = pListener;
   } else {
      // Link element into head
      info->pListeners->pPrev = pListener;
      pListener->pNext = info->pListeners;
      info->pListeners = pListener;
   }

   return SUCCESS;
}


/*=============================================================================
FUNCTION: SysModelBase_Notify

DESCRIPTION:  Implements the IMODEL_Notify method

PARAMETERS:
   *p       [in]:  IModel interface
   *pEvent  [in]:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SysModelBase_Notify(IModel *p, ModelEvent *pEvent)
{
   SysModelBase    *pMe = IMODEL_TO_SYSMODELBASE(p);
   SysModelAppInfo *info;

   if (NULL == pEvent) {
      return;
   }

   info = pMe->appList;

   while (info != NULL) {
      ModelListener *pListener;
      ACONTEXT      *pCtxOld;
      
      // Save the next app off the stack, so we can easily move to the
      // next-next app if the next app is indirectly canceled by
      // invoking the current app
      pMe->pNextNotifyApp = info->pNext; 

      pCtxOld = AEE_EnterAppContext(info->appCtx);

      pEvent->pModel = p;
      pListener = info->pListeners;
      while (pListener != NULL) {
         // Save the next listener off the stack, so we can easily move to the
         // next-next listener if the next listener is indirectly canceled by
         // invoking the current listener

         pMe->pNextNotifyListener = pListener->pNext;

         if (pListener->pfnListener) {
            pListener->pfnListener(pListener->pListenerData, pEvent);
         }
         pListener = pMe->pNextNotifyListener;
      }

      AEE_LeaveAppContext(pCtxOld);
      
      info = pMe->pNextNotifyApp;
   }
}


/*=============================================================================
FUNCTION: SysModelBase_CancelListener

DESCRIPTION:  Model listener cancel callback.

PARAMETERS:
   *pListener  [in]:  The model listen to cancel

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SysModelBase_CancelListener(ModelListener *pListener) 
{
   SysModelAppInfo *info = (SysModelAppInfo *) pListener->pCancelData;

   // If the next listener to be notified is canceled, then move to the next
   if (pListener == info->ownerSysModel->pNextNotifyListener) {
      info->ownerSysModel->pNextNotifyListener = info->ownerSysModel->pNextNotifyListener->pNext;
   }
  
   //
   // Remove the listener from the app info doubly-linked list
   //
   if (pListener->pNext) {
      pListener->pNext->pPrev = pListener->pPrev;
   }
   
   if (pListener->pPrev) {
      pListener->pPrev->pNext = pListener->pNext;
   } else if(info->pListeners != NULL) {
      // No prev, this is head node
      info->pListeners = info->pListeners->pNext;
   }

   // If this app info has no more listeners, release it to avoid 
   // a BREW system object leak warning on applet exist.
   if (NULL == info->pListeners) {
      CALLBACK_Cancel(&info->cbSysObject);
      SysModelBase_FreeAppInfo(info);
      info = NULL;
   }

   // NULL out the listener's data
   pListener->pfnCancel = NULL;
   pListener->pNext = pListener->pPrev = NULL;
}


/*=============================================================================
FUNCTION: SysModelBase_GetAppInfo

DESCRIPTION:  Returns a SysModelAppInfo structure for the current 
              app context, or creates one if the current app context
              does not have one yet.

PARAMETERS:
   *pMe  [in]:

RETURN VALUE:
   SysModelAppInfo *:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
SysModelAppInfo *SysModelBase_GetAppInfo(SysModelBase *pMe)
{
   SysModelAppInfo *info;
   ACONTEXT        *appCtx = AEE_GetAppContext();

   // Run through the existing app info list, looking for one that 
   // matches the current app context.
   info = pMe->appList;
   while ( (info != NULL) && (info->appCtx != appCtx) ) {
      info = info->pNext;
   }

   if (NULL == info) {
      //
      // Didn't find it.  Lets create a new one:
      //

      // Allocate some memory...
      info = AEEHeap_Realloc(NULL, sizeof(*info));

      if (info) {
         info->appCtx = appCtx;
         info->ownerSysModel = pMe;

         // Allow BREW to manage the app info for us, so that it will
         // be released if the applet exists without releasing all 
         // its references to us.
         CALLBACK_Init(&info->cbSysObject, 
                       (PFNNOTIFY) SysModelBase_FreeAppInfo, 
                       (void *) info);

         AEE_LinkSysObject(&info->cbSysObject);

         // Insert it at the front of the singly-linked list.
         info->pNext = pMe->appList;
         pMe->appList = info;
      }
   }
   return info;
}


/*=============================================================================
FUNCTION: SysModelBase_FreeAppInfo

DESCRIPTION:  Callback that is invoked when an applet exits

PARAMETERS:
   *thisInfo [in]:  SysModelAppInfo structure for the applet that just exited

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SysModelBase_FreeAppInfo(SysModelAppInfo *thisInfo)
{
   SysModelAppInfo *currInfo, *prevInfo;

   // If the next app to be notified is going away, move to the next
   if (thisInfo->ownerSysModel->pNextNotifyApp == thisInfo) {
      thisInfo->ownerSysModel->pNextNotifyApp = thisInfo->pNext;
   }
   
   currInfo = thisInfo->ownerSysModel->appList;
   prevInfo = NULL;

   // Remove "thisInfo" from the SysModelAppInfo singly-linked list stored
   // in the SysModel
   while (currInfo != NULL) {
      if (currInfo == thisInfo) {
         if (prevInfo) {
            prevInfo->pNext = currInfo->pNext;
         } else {
            thisInfo->ownerSysModel->appList = currInfo->pNext;
         }
         break;
      }

      prevInfo = currInfo;
      currInfo = currInfo->pNext;
   }

   // Free the memory
   (void) AEEHeap_Realloc(thisInfo, NULL);
}


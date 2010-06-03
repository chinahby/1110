/*
================================================================================

FILE:  OEMSysMMOverlayRegistry.c

SERVICES: Provides Display Configurator related services.


================================================================================

Copyright © 1999-2006 QUALCOMM Incorporated 
All Rights Reserved.
QUALCOMM Proprietary

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMSysMMOverlayRegistry.c#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

================================================================================
*/
#include "OEMFeatures.h"

#ifdef FEATURE_MMOVERLAY

#include "AEESysMMOverlayRegistry.h"
#include "AEEStdLib.h"
#include "AEECriticalSection.h"
#include "AEE_OEM.h"
#include "AEECLSID_SysMMOverlayRegistry.bid"
#include "AEEBase.h"
#include "AEEAppletCtl.h"
#include "AEEClassIDs.h"
#include "OEMDisp.h"
#include "AEEBitmap.h"
#include "AEEMMOverlay.h"
#include "OEMOverlay_priv.h"

#include "BREWVersion.h"
#if MIN_BREW_VERSION(4,0)
#include "AEEIBitmapCtl.h"
#endif

typedef struct RegisteredOverlayNode RegisteredOverlayNode;

struct RegisteredOverlayNode
{
   RegisteredOverlayNode * pPrev;
   RegisteredOverlayNode * pNext;
   RegisteredOverlayNode ** ppHead;
   IMMOverlay *pMMOverlay;
   AEECallback cbCleanup;
   AEECLSID OwnerID;
};

static RegisteredOverlayNode *gpDisplay1HeadNode;
static RegisteredOverlayNode *gpDisplay2HeadNode;
static RegisteredOverlayNode *gpDisplay3HeadNode;
static RegisteredOverlayNode *gpDisplay4HeadNode;

//SysMMOverlayRegistry
struct ISysMMOverlayRegistry
{
   const AEEVTBL(ISysMMOverlayRegistry) *pvt;// Function Table

   IShell*               pShell;       // Pointer to the shell
   uint32                dwRefs;       // Reference count for this object
};

/*==============================================================================
SysMMOverlayRegistry Implementation
==============================================================================*/
/*==============================================================================
DESTRUCTOR
==============================================================================*/

static void SysMMOverlayRegistry_Delete(ISysMMOverlayRegistry *pMe)
{
   if(NULL != pMe->pShell)
   {
      (void)ISHELL_Release(pMe->pShell);
      pMe->pShell = NULL;
   }
   FREE(pMe);
}

/*==============================================================================
INTERFACE FUNCTIONS
==============================================================================*/

/*
================================================================================

ISysMMOverlayRegistry_AddRef()

This function is inherited from IQI_AddRef().
   
================================================================================
*/
static uint32 SysMMOverlayRegistry_AddRef(ISysMMOverlayRegistry *pMe)
{
   return ++pMe->dwRefs;
}

/*
================================================================================

ISysMMOverlayRegistry_Release()

This function is inherited from IQI_Release().
   
================================================================================
*/
static uint32 SysMMOverlayRegistry_Release(ISysMMOverlayRegistry *pMe)
{
   uint32 dwRefs;

   dwRefs = --pMe->dwRefs;
   if (dwRefs == 0)
      SysMMOverlayRegistry_Delete(pMe);
   return (dwRefs);
}

/*
================================================================================

ISysMMOverlayRegistry_QueryInterface()

This function is inherited from IQI_QueryInterface().
   
================================================================================
*/
static int SysMMOverlayRegistry_QueryInterface(ISysMMOverlayRegistry *pMe, AEECLSID id,
                                     void **ppo)
{
   switch (id)
   {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_SysMMOverlayRegistry:
      case AEEIID_ISysMMOverlayRegistry:
         *ppo = (void*)pMe;
         (void)IQI_AddRef(*ppo);
         return SUCCESS;
      default:
         break;
   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
} 

/*
================================================================================
RemoveNode()

Description: 
This function removes the input node from the list.

Prototype:

void RemoveNode(RegisteredOverlayNode *pNode);

Parameters:
===pre>
pNode   :  Pointer to the node to remove.
===/pre>

Return Value:
None

Comments:  
None

Side Effects: 
None

See Also:
None

================================================================================
*/
static void RemoveNode(RegisteredOverlayNode *pNode)
{
   CALLBACK_Cancel(&(pNode->cbCleanup));

   //Remove from list;
   if(pNode->pPrev == NULL)
   {
      //At head node
      *(pNode->ppHead) = pNode->pNext;
   }
   else
   {
      pNode->pPrev->pNext = pNode->pNext;
   }

   if(pNode->pNext != NULL)
   {
      pNode->pNext->pPrev = pNode->pPrev;
   }

   FREE(pNode);
}

/*
================================================================================
AppCloseCB()

Description: 
This function is called when the application that created a node is closed.

Prototype:

void AppCloseCB(void *po);

Parameters:
===pre>
po      :  Pointer to the node to remove.
===/pre>

Return Value:
None

Comments:  
None

Side Effects: 
None

See Also:
None

================================================================================
*/
static void AppCloseCB(void *po)
{
   RegisteredOverlayNode *pNode = (RegisteredOverlayNode *)po;
   RemoveNode(pNode);
}

/*
================================================================================

ISysMMOverlayRegistry_RegisterOverlay()

Description: 
This function registers an additional overlay with the ISysMMOverlayRegistry interface.
This overlay will be updated whenever an IDisplay_Update occurs.
This overlay should be pointing to the device bitmap.

Prototype:

int ISysMMOverlayRegistry_RegisterOverlay(ISysMMOverlayRegistry * pMe, IMMOverlay *pMMOverlay, 
                                boolean bAdd)

Parameters:
===pre>
p       :  Pointer to the ISysMMOverlayRegistry interface object.
pMMOverlay:  Pointer to an IOverlay that will be registered/unregistered with this
           interface. If this is NULL and bAdd is FALSE all additional overlays
           that this application registered are removed.
bAdd    :  TRUE to register the overlay, FALSE to unregister it.
===/pre>

Return Value:
SUCCESS          - Operation Successful
EFAILED          - Generic failure

Comments:  
If there are any overlays registered that are enabled the default display
update behavior will not occur. The user should remove all overlays before
their application is unloaded. The display the overlay is assigned to should
not be changed while the overlay is registered.

Side Effects: 
None

See Also:
None

================================================================================
*/
static int SysMMOverlayRegistry_RegisterOverlay(ISysMMOverlayRegistry * pMe, IMMOverlay *pMMOverlay, 
                                boolean bAdd)
{
   AEECLSID currentApplication;
   IAppletCtl *pAppletCtl;

   if(SUCCESS != ISHELL_CreateInstance(pMe->pShell, AEECLSID_APPLETCTL,
                                       (void **)&pAppletCtl))
   {
      return EFAILED;
   }

   currentApplication = IAPPLETCTL_RunningApplet(pAppletCtl);
   (void)IAPPLETCTL_Release(pAppletCtl);

   if(TRUE == bAdd)
   {
      if(NULL != pMMOverlay)
      {
         //Add MMoverlay
         RegisteredOverlayNode * pNewNode;
         uint32 p1, p2;

         //Make sure this is not already in the list
         (void)ISysMMOverlayRegistry_RegisterOverlay(pMe, pMMOverlay, FALSE);

         //Create a node
         pNewNode = MALLOCREC(RegisteredOverlayNode);
         if(NULL == pNewNode)
         {
            return ENOMEMORY;
         }

         pNewNode->OwnerID = currentApplication;
         pNewNode->pPrev = NULL;

         if(SUCCESS != IMMOVERLAY_GetParam(pMMOverlay, OV_PARM_DISPLAY, &p1, &p2))
         {
            FREE(pNewNode);
            return EFAILED;
         }

         switch(p1)
         {
            case AEECLSID_DISPLAY1:
               pNewNode->ppHead = &gpDisplay1HeadNode;
               break;
            case AEECLSID_DISPLAY2:
               pNewNode->ppHead = &gpDisplay2HeadNode;
               break;
            case AEECLSID_DISPLAY3:
               pNewNode->ppHead = &gpDisplay3HeadNode;
               break;
            case AEECLSID_DISPLAY4:
               pNewNode->ppHead = &gpDisplay4HeadNode;
               break;
            default:
               FREE(pNewNode);
               return EFAILED;
         }

         pNewNode->pNext = *(pNewNode->ppHead);
         if(NULL != pNewNode->pNext)
         {
            pNewNode->pNext->pPrev = pNewNode;
         }
         *(pNewNode->ppHead) = pNewNode;
         pNewNode->pMMOverlay = pMMOverlay;

         CALLBACK_Init(&(pNewNode->cbCleanup), AppCloseCB, pNewNode);
         ISHELL_OnAppClose(pMe->pShell, &(pNewNode->cbCleanup));

         return SUCCESS;


      }
      else
      {
         return EBADPARM;
      }
   }
   else
   {
      if(NULL != pMMOverlay)
      {
         //Remove MMOverlay
         int i;

         for (i=0; i<4; i++)
         {
            RegisteredOverlayNode *pCurrent;
            switch (i)
            {
               case 0:
                  pCurrent = gpDisplay1HeadNode;
                  break;
               case 1:
                  pCurrent = gpDisplay2HeadNode;
                  break;
               case 2:
                  pCurrent = gpDisplay3HeadNode;
                  break;
               case 3:
                  pCurrent = gpDisplay4HeadNode;
                  break;
               default:
                  return EFAILED;
            }

            while(NULL != pCurrent)
            {
               RegisteredOverlayNode *pNext = pCurrent->pNext;
               if(pCurrent->pMMOverlay == pMMOverlay)
               {
                  //Found it
                  RemoveNode(pCurrent);
                  return SUCCESS;
               }
               pCurrent = pNext;
            }
         }

         return EFAILED;

      }
      else
      {
         //Remove all MMOverlay that belong to current app
         int i;

         for (i=0; i<4; i++)
         {
            RegisteredOverlayNode *pCurrent;
            switch (i)
            {
               case 0:
                  pCurrent = gpDisplay1HeadNode;
                  break;
               case 1:
                  pCurrent = gpDisplay2HeadNode;
                  break;
               case 2:
                  pCurrent = gpDisplay3HeadNode;
                  break;
               case 3:
                  pCurrent = gpDisplay4HeadNode;
                  break;
               default:
                  return EFAILED;
            }

            while(NULL != pCurrent)
            {
               RegisteredOverlayNode *pNext = pCurrent->pNext;
               if(pCurrent->OwnerID == currentApplication)
               {
                  //Found it
                  RemoveNode(pCurrent);
               }
               pCurrent = pNext;
            }
         }

         return SUCCESS;
      }
   }
}

/*
================================================================================

ISysMMOverlayRegistry_AreOverlaysRegistered()

Description: 
This function retrieves if there are any overlays registered and enabled on
this display.

Prototype:

int ISysMMOverlayRegistry_AreOverlaysRegistered(ISysMMOverlayRegistry * pMe,
                                      AEECLSID displayCls,
                                      boolean * pbRegistered)

Parameters:
===pre>
pMe          :  Pointer to the ISysMMOverlayRegistry interface object.
displayCls   : Class ID of the Display that should be checked.
pbRegistered :  Pointer to a boolean that will contain TRUE if the is at least
                one overlay registered and enabled.
===/pre>

Return Value:
SUCCESS          - Operation Successful
EFAILED          - Generic failure

Comments:  
This function is not intended to be used by applications.

Side Effects: 
None.

See Also:
None

================================================================================
*/
static int SysMMOverlayRegistry_AreOverlaysRegistered(ISysMMOverlayRegistry * pMe,
                                            AEECLSID displayCls,
                                            boolean * pbRegistered)
{
   RegisteredOverlayNode * pCurrent;
   (void)pMe;
   *pbRegistered = FALSE;

   switch (displayCls) {
      case AEECLSID_DISPLAY1:
         pCurrent = gpDisplay1HeadNode;
         break;
      case AEECLSID_DISPLAY2:
         pCurrent = gpDisplay2HeadNode;
         break;
      case AEECLSID_DISPLAY3:
         pCurrent = gpDisplay3HeadNode;
         break;
      case AEECLSID_DISPLAY4:
         pCurrent = gpDisplay4HeadNode;
         break;
      default:
         return EFAILED;
   }

   while(NULL != pCurrent)
   {
      uint32 p1, p2;

      if(SUCCESS == IMMOVERLAY_GetParam(pCurrent->pMMOverlay, OV_PARM_ENABLE, &p1, &p2))
      {
         if(1 == p1)
         {
            *pbRegistered = TRUE;
            break;
         }
      }

      pCurrent = pCurrent->pNext;
   }

   return SUCCESS;
}

/*
================================================================================

ISysMMOverlayRegistry_Update()

Description: 
   This function causes the specified region of the display to be updated.
   If called in the system context it will update all of the "BREW" overlays
   that are active.

Prototype:

   int ISysMMOverlayRegistry_Update(ISysMMOverlayRegistry *pMe, AEECLSID displayCls, AEERect * prc)

Parameters:
===pre>
   p          : Pointer to the ISysMMOverlayRegistry interface object.
   displayCls : Class ID of the Display that should be updated
   prc        : Pointer to an AEERect that specifies what region to update,
                in the screen's coordinate space.
===/pre>

Return Value:
   SUCCESS          - Operation Successful
   EFAILED          - Generic failure

Comments:  
   This function is not intended to be used by applications.

Side Effects: 
   None

See Also:
   None

================================================================================
*/
static int SysMMOverlayRegistry_Update(ISysMMOverlayRegistry * pMe,
                             AEECLSID displayCls,
                             const AEERect * prc)
{
   RegisteredOverlayNode * pCurrent;
   (void)pMe;

   switch (displayCls) {
      case AEECLSID_DISPLAY1:
         pCurrent = gpDisplay1HeadNode;
         break;
      case AEECLSID_DISPLAY2:
         pCurrent = gpDisplay2HeadNode;
         break;
      case AEECLSID_DISPLAY3:
         pCurrent = gpDisplay3HeadNode;
         break;
      case AEECLSID_DISPLAY4:
         pCurrent = gpDisplay4HeadNode;
         break;
      default:
         return EFAILED;
   }

   while(NULL != pCurrent)
   {
      uint32 p1, p2;

      if(SUCCESS == IMMOVERLAY_GetParam(pCurrent->pMMOverlay, OV_PARM_ENABLE, &p1, &p2))
      {
         if(1 == p1)
         {
            AEERect newRect;

            if(prc != NULL)
            {
               newRect = *prc;
            }
            else
            {
               SETAEERECT(&newRect, 0, 0, 0 ,0);
            }
            if(SUCCESS == IMMOVERLAY_GetParam(pCurrent->pMMOverlay, OV_PARM_BITMAP, &p1, &p2))
            {
               IBitmap *pBitmap = (IBitmap *)p1;
               IBitmapCtl *pBitmapCtl;
#if MIN_BREW_VERSION(4,0)
               if(SUCCESS == IBITMAP_QueryInterface(pBitmap, AEEIID_IBitmapCtl, (void **)&pBitmapCtl))
#else
               if(SUCCESS == IBITMAP_QueryInterface(pBitmap, AEEIID_BITMAPCTL, (void **)&pBitmapCtl))
#endif
               {
                  AEEPoint in,out;
                  in.x = newRect.x;
                  in.y = newRect.y;
#if MIN_BREW_VERSION(4,0)
                  out = IBitmapCtl_ScreenToApp(pBitmapCtl, in);
                  (void) IBitmapCtl_Release(pBitmapCtl);
#else
                  out = IBITMAPCTL_ScreenToApp(pBitmapCtl, in);
                  (void) IBITMAPCTL_Release(pBitmapCtl);
#endif
                  newRect.x = out.x;
                  newRect.y = out.y;
               }
               (void)IBITMAP_Release(pBitmap);
            }

            (void) IMMOVERLAY_Update(pCurrent->pMMOverlay, &newRect);

         }
      }

      pCurrent = pCurrent->pNext;
   }

   return SUCCESS;
}


/*==============================================================================
CONSTRUCTOR
==============================================================================*/
int SysMMOverlayRegistry_New(IShell *pShell, AEECLSID ClsId, void **ppObj)
{
   ISysMMOverlayRegistry *pNew;
   int nErr = SUCCESS;
   static const VTBL(ISysMMOverlayRegistry) SysMMOverlayRegistryFuncs = 
   {
      SysMMOverlayRegistry_AddRef,
      SysMMOverlayRegistry_Release,
      SysMMOverlayRegistry_QueryInterface,
      SysMMOverlayRegistry_RegisterOverlay,
      SysMMOverlayRegistry_AreOverlaysRegistered,
      SysMMOverlayRegistry_Update
   };

   // Make sure its us
   if (ClsId != AEECLSID_SysMMOverlayRegistry)
      return ECLASSNOTSUPPORT;

   // Get the object
   pNew = MALLOCREC(ISysMMOverlayRegistry);
   if (!pNew)
   {
      DBGPRINTF_FATAL("Error MALLOC FAILED");
      return ENOMEMORY;
   }

   pNew->pvt = &SysMMOverlayRegistryFuncs;

   // Now initialize it
   pNew->pShell = pShell;
   (void)ISHELL_AddRef(pNew->pShell);

   pNew->dwRefs = 1;

   *ppObj = pNew;
   return nErr;
}

#endif /* FEATURE_MMOVERLAY */



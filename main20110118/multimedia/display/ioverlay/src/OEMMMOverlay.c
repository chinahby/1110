/*
================================================================================

FILE:  OEMOverlay.c

SERVICES: Provides overlay related services.


================================================================================

Copyright © 1999-2005 QUALCOMM Incorporated 
All Rights Reserved.
QUALCOMM Proprietary

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMMMOverlay.c#8 $
$Author: jbrasen $
$DateTime: 2009/02/11 14:53:01 $

================================================================================
*/
#include "OEMFeatures.h"

#ifdef FEATURE_MMOVERLAY

#include "AEEMMOverlay.h"
#include "AEEDisp.h"
#include "OEMDisp.h"
#include "AEEComdef.h"
#include "AEEStdLib.h"
#include "AEECriticalSection.h"
#include "AEE_OEM.h"
#include "IYCbCr.h"
#include "AEEModel.h"
#include "AEEBase.h"
#include "OEMDisp.h"
#include "BREWVersion.h"
#if MIN_BREW_VERSION(4,0)
#include "AEEIBitmapCtl.h"
#endif

#include "OEMOverlayHelper.h"
#include "OEMOverlay_priv.h"

#include "AEELogicalDisplayCfg.h"
#include "AEECLSID_LOGICALDISPLAYCFG.BID"

#include "AEECLSID_PHYSICALDISPLAY.BID"
#include "AEEPhysicalDisplayOverlay.h"

#include "AEECLSID_SysMMOverlayRegistry.bid"
#include "AEESysMMOverlayRegistry.h"

#define MAX_PENDING_UPDATES 10

#ifdef RELEASEIF
#undef RELEASEIF
#endif
#define RELEASEIF(p) \
   if (p != NULL) { (void)IBASE_Release((IBase*)(void *)(p)); (p) = NULL; }

/*====================================================================
OVERLAY Implementation
====================================================================*/
//Overlay Model
typedef struct IMMOverlayModel {
   AEEBASE_INHERIT( IModel, IMMOverlay );
} IMMOverlayModel;

typedef struct {
   IPhysicalDisplayOverlay *pPhysicalDisplayOverlay;
   ModelListener     physicalListener;
   AEECLSID clsID;
   uint32 ID;
   boolean bSeen;
} OverlayPhysicalDisplayMapType;

#define OVERLAY_MAX_PHYSICAL 4

typedef struct
{
   boolean bInUse;
   uint32  dwUser;
   int     nErr;
   boolean bPending[OVERLAY_MAX_PHYSICAL];
} OverlayAsyncUpdateEntryType;

//Storage array
static const OverlayParamType gParamIndex[] = 
{
      OV_PARM_BITMAP,
      OV_PARM_SRC_REGION,
      OV_PARM_DEST_REGION,
      OV_PARM_DISPLAY,
      OV_PARM_LAYER,
      OV_PARM_TRANSPARENCY,
      OV_PARM_ALPHA_BLEND_PCT,
      OV_PARM_ENABLE,
      OV_PARM_AUTO_DISABLE,
      OV_PARM_ROTATE,
      OV_PARM_REFLECT,
      OV_PARM_SCALE_X,
      OV_PARM_SCALE_Y,
      OV_PARM_DEST_REGION_SCREEN,
      OV_PARM_VSYNC,
      OV_PARM_MINIMIZE_CPU,
      OV_PARM_SYSTEM_OVERLAY,
      OV_PARM_NO_AUTOCLEAR,
      OV_PARM_ALLOW_NEGATIVE_REGION,
      OV_PARM_EDGE_SCALING
};

static int paramIDToIndex(OverlayParamType param)
{
   int i;
   for(i = 0; i < (int)(sizeof(gParamIndex)/sizeof(OverlayParamType)); i++)
   {
      if(param == gParamIndex[i])
      {
         break;
      }
   }

   if(i == (int)(sizeof(gParamIndex)/sizeof(OverlayParamType)))
   {
      i = -1;
   }

   return i;
}

//Overlay 
struct IMMOverlay
{
   const AEEVTBL(IMMOverlay) *pvt;// Function Table

   IShell*             pShell;          // Pointer to the shell
   uint32              dwRefs;          // Reference count for this object
   AEECallback         CBEnable;        //Display Enable Callback
   AEECallback         CBExit;          //On Exit Callback
   boolean             bCleanup;        //Should we cleanup all resources
   IMMOverlayModel       theModel;        //Model for this interface
   OverlayHelper_Listener *   pListeners;      //The listeners on the model
   ILogicalDisplayCfg *pLogicalDisplayCfg;     //The Logical Display Config Interface
   ModelListener       logicalListener;        //The Listener on the logical interface
   IDisplay *          pIDisplay;              //The Display Ptr for this application

   //Array of physical displays that this overlay is on
   OverlayPhysicalDisplayMapType physicalDisplays[OVERLAY_MAX_PHYSICAL]; 
                                     
   //Data Stores for this Overlay
   uint32 currentParam1[sizeof(gParamIndex)/sizeof(OverlayParamType)]; //Data store for the
                                                       //current values of 
                                                       //parameter 1
   uint32 currentParam2[sizeof(gParamIndex)/sizeof(OverlayParamType)]; //Data store for the
                                                       //current values of 
                                                       //parameter 2

   //Special Cases - Value set does not need to equal current value
   AEERect rcCurrentSrcRegion;             // The current Source Region
   AEERect rcSetSrcRegion;                 // The Set Source region
   AEERect rcCurrentDestRegion;            // The current Destination region
   AEERect rcSetDestRegion;                // The set destination region
   AEERect rcCurrentScreenRegion;          // The current Screen region that this
                                           // Overlay is assigned to
   AEERect rcCurrentAppRegion;             // The current applicaition region that
                                           // this overlay is assigned to. Should
                                           // be a subset of the above region.
   AEEBitmapInfo currentBitmapInfo;        // The current size of the bitmap
   boolean bSetEnableState;                // The set enabled state

   boolean bFirstUpdate;                   // One update must occur before registering
                                           // The overlays
   boolean bNeedClear;                     // Has something changed that makes me need
                                           // to clear the screen?

   uint32 dwSetScaleX, dwSetScaleY;           // The set scale factors for the X and Y dimensions
   uint32 dwSetScaleFlagsX, dwSetScaleFlagsY; // The set scale flags for the X and Y dimensions
   boolean bScaleYSetLast;
   OverlayAsyncUpdateEntryType OverlayAsyncUpdateEntry[MAX_PENDING_UPDATES];
   boolean bSetDestRegion;

};

/*==============================================================================
Overlay helper functions
==============================================================================*/
static void PhysicalListenerCB(void * pData, const ModelEvent * pEvent)
{
   IMMOverlay * pMe = (IMMOverlay *) pData;
   switch (pEvent->evCode)
   {
      case EVT_MDL_PDC_ASYNC_UPDATE_COMPLETE:
      {
         PhysicalDisplayAsyncUpdateCompleteType *pUpdateInfo = 
            (PhysicalDisplayAsyncUpdateCompleteType *) pEvent->dwParam;
         if(pMe == pUpdateInfo->pOverlay)
         {
            int index;
            //This is mine
            //Verify that the updateID is in use
            if(pMe->OverlayAsyncUpdateEntry[pUpdateInfo->dwUnique].bInUse == FALSE)
            {
               return;
            }

            if(SUCCESS != pUpdateInfo->nErr)
            {
               pMe->OverlayAsyncUpdateEntry[pUpdateInfo->dwUnique].nErr = pUpdateInfo->nErr;
            }

            //Mark the update as non pending
            for(index = 0; index < OVERLAY_MAX_PHYSICAL; index++)
            {
               if(pMe->physicalDisplays[index].clsID == pUpdateInfo->clsPhysicalDisplay)
               {
                  pMe->OverlayAsyncUpdateEntry[pUpdateInfo->dwUnique].bPending[index] =
                     FALSE;
                  break;
               }
            }

            if(index == OVERLAY_MAX_PHYSICAL)
            {
               DBGPRINTF("Unknown Physical Display");
            }

            //Check to see if we are done
            for(index = 0; index < OVERLAY_MAX_PHYSICAL; index++)
            {
               if(pMe->OverlayAsyncUpdateEntry[pUpdateInfo->dwUnique].bPending[index] == TRUE)
               {
                  break;
               }
            }

            if(index == OVERLAY_MAX_PHYSICAL)
            {
               //Send the notification
               OverlayAsyncUpdateCompleteType notification;
               notification.dwUnique = pUpdateInfo->dwUnique;
               notification.nErr = pMe->OverlayAsyncUpdateEntry[pUpdateInfo->dwUnique].nErr;
               notification.dwUser = pMe->OverlayAsyncUpdateEntry[pUpdateInfo->dwUnique].dwUser;
               pMe->OverlayAsyncUpdateEntry[pUpdateInfo->dwUnique].bInUse = FALSE;
               OverlayHelper_Notify((void*) pMe, 
                                    EVT_MDL_OV_ASYNC_UPDATE_COMPLETE,
                                    (uint32)&notification);

            }
         }
         break;
      }
      default:
         break;
   }
}

static void registerPhysicalDisplay(IMMOverlay *pMe, uint32 i, AEECLSID physicalDisplay)
{
   if((pMe->physicalDisplays[i].pPhysicalDisplayOverlay) == NULL)
   {
      IPhysicalDisplay *pPhysicalDisplay;
      if(SUCCESS == ISHELL_CreateInstance(pMe->pShell, physicalDisplay, (void **)&pPhysicalDisplay))
      {
         if(SUCCESS != IPHYSICALDISPLAY_QueryInterface(pPhysicalDisplay, AEEIID_PHYSICALDISPLAYOVERLAY,
                                                      (void **)&pMe->physicalDisplays[i].pPhysicalDisplayOverlay))
         {
            pMe->physicalDisplays[i].pPhysicalDisplayOverlay = NULL;
         }
         (void)IPHYSICALDISPLAY_Release(pPhysicalDisplay);
      }
      else
      {
         pMe->physicalDisplays[i].pPhysicalDisplayOverlay = NULL;
      }
   }

   if((pMe->physicalDisplays[i].pPhysicalDisplayOverlay) != NULL)
   {
      if(pMe->physicalDisplays[i].ID == 0)
      {
         if(SUCCESS != IPHYSICALDISPLAYOVERLAY_OverlayRegister(
            pMe->physicalDisplays[i].pPhysicalDisplayOverlay,
            pMe,
            &pMe->physicalDisplays[i].ID))
         {
            pMe->physicalDisplays[i].ID =0;
            (void) IPHYSICALDISPLAYOVERLAY_Release(pMe->physicalDisplays[i].pPhysicalDisplayOverlay);
            pMe->physicalDisplays[i].pPhysicalDisplayOverlay = NULL;
         }
         else
         {
            IModel *pModel;
            pMe->physicalDisplays[i].clsID = physicalDisplay;
            LISTENER_Init(&pMe->physicalDisplays[i].physicalListener, PhysicalListenerCB, (void *) pMe);

            //Register the listener
            if(SUCCESS == IPHYSICALDISPLAYOVERLAY_QueryInterface(pMe->physicalDisplays[i].pPhysicalDisplayOverlay,
                                                                 AEEIID_MODEL, (void **)&pModel))
            {

               if(SUCCESS != IMODEL_AddListener(pModel, &pMe->physicalDisplays[i].physicalListener))
               {
                  //Deregister the Overlay bad things happened;
                  (void) IPHYSICALDISPLAYOVERLAY_OverlayDeregister(pMe->physicalDisplays[i].pPhysicalDisplayOverlay,
                                                            pMe->physicalDisplays[i].ID);
                  LISTENER_Cancel(&pMe->physicalDisplays[i].physicalListener);
                  (void) IPHYSICALDISPLAYOVERLAY_Release(pMe->physicalDisplays[i].pPhysicalDisplayOverlay);
                  pMe->physicalDisplays[i].pPhysicalDisplayOverlay = NULL;
                  pMe->physicalDisplays[i].ID = 0;
                  pMe->physicalDisplays[i].clsID = 0;
               }
               (void) IMODEL_Release(pModel);
            }
         }
      }
   }
}

static void UpdateLinkState(IMMOverlay *pMe)
{
   uint32 i;
   int32 dwDisplays;
   int index;
   if(NULL == pMe->pLogicalDisplayCfg)
   {
      return;
   }

   index = paramIDToIndex(OV_PARM_DISPLAY);
   if(-1 == index)
   {
      return;
   }
   if(SUCCESS == ILOGICALDISPLAYCFG_GetLinkedDisplays(pMe->pLogicalDisplayCfg, 
                                                      pMe->currentParam1[index],
                                                      NULL,
                                                      &dwDisplays))
   {
      AEECLSID *pPhysicalDisplays;

      pPhysicalDisplays = MALLOC((uint32)dwDisplays * sizeof(AEECLSID));

      if(NULL == pPhysicalDisplays)
      {
         return;
      }

      for(i=0; i< OVERLAY_MAX_PHYSICAL; i++)
      {
         pMe->physicalDisplays[i].bSeen = 0;
      }

      if(SUCCESS == ILOGICALDISPLAYCFG_GetLinkedDisplays(pMe->pLogicalDisplayCfg, 
                                                      pMe->currentParam1[index],
                                                      pPhysicalDisplays,
                                                      &dwDisplays))
      {
         int j;
         for(j = 0; j < dwDisplays; j++)
         {
            IPhysicalDisplay *pPhysicalDisplay;
            if(SUCCESS == ISHELL_CreateInstance(pMe->pShell, pPhysicalDisplays[j], (void **) &pPhysicalDisplay))
            {
               pPhysicalDisplays[j] = IPHYSICALDISPLAY_GetDisplayHandler(pPhysicalDisplay);
               (void)IPHYSICALDISPLAY_Release(pPhysicalDisplay);
               if(0 == pPhysicalDisplays[j])
               {
                  continue;
               }
            }
            for(i=0; i< OVERLAY_MAX_PHYSICAL; i++)
            {
               if(pMe->physicalDisplays[i].clsID == pPhysicalDisplays[j])
               {
                  pMe->physicalDisplays[i].bSeen = 1;
                  break;
               }
            }
            if(i == OVERLAY_MAX_PHYSICAL)
            {
               for(i=0; i< OVERLAY_MAX_PHYSICAL; i++)
               {
                  if(pMe->physicalDisplays[i].clsID == 0)
                  {
                     pMe->physicalDisplays[i].clsID = pPhysicalDisplays[j];
                     pMe->physicalDisplays[i].bSeen = 1;
                     break;
                  }
               }
            }
         }
      }

      
      FREE(pPhysicalDisplays);
      pPhysicalDisplays = NULL;

      for(i=0; i< OVERLAY_MAX_PHYSICAL; i++)
      {
         if(pMe->physicalDisplays[i].bSeen == 0)
         {
            if(pMe->physicalDisplays[i].pPhysicalDisplayOverlay != NULL)
            {
               if(pMe->physicalDisplays[i].ID != 0)
               {
                  (void) IPHYSICALDISPLAYOVERLAY_OverlayDeregister(pMe->physicalDisplays[i].pPhysicalDisplayOverlay,
                     pMe->physicalDisplays[i].ID);
               }
               LISTENER_Cancel(&pMe->physicalDisplays[i].physicalListener);
               (void) IPHYSICALDISPLAYOVERLAY_Release(pMe->physicalDisplays[i].pPhysicalDisplayOverlay);
            }
            pMe->physicalDisplays[i].pPhysicalDisplayOverlay = NULL;
            pMe->physicalDisplays[i].ID = 0;
            pMe->physicalDisplays[i].clsID = 0;
         }
      }

      for(i=0; i< OVERLAY_MAX_PHYSICAL; i++)
      {
         if(pMe->physicalDisplays[i].bSeen == 1)
         {
            if((pMe->physicalDisplays[i].pPhysicalDisplayOverlay == NULL) || (pMe->physicalDisplays[i].ID == 0))
            {
               registerPhysicalDisplay(pMe, i, pMe->physicalDisplays[i].clsID);
            }
         }
      }
   }

   return;
}


static int calculateClippingRegion(const IMMOverlay *pMe, AEERect *pNewSrc, 
                                   AEERect *pNewDest) 
{
   AEERect ClippingRegion;
   int indexScaleX, indexScaleY, indexRotate, indexDestRegion;
   int indexNegRegion = paramIDToIndex(OV_PARM_ALLOW_NEGATIVE_REGION);
   
   //Set up the clipping region

   ClippingRegion.x  = pMe->rcSetDestRegion.x;
   ClippingRegion.y  = pMe->rcSetDestRegion.y;
   ClippingRegion.dx = pMe->rcSetDestRegion.dx;
   ClippingRegion.dy = pMe->rcSetDestRegion.dy;

   //Get the regions for this application
   //Is the upper left corner invalid?
   if((-1 != indexNegRegion) && (0 == pMe->currentParam1[indexNegRegion]))
   {
      if(0 > ClippingRegion.x)
      {
         ClippingRegion.x = 0;
      }
   }

   if(ClippingRegion.dx == 0)
   {
      ClippingRegion.dx = pMe->rcCurrentAppRegion.dx - ClippingRegion.x;
   }
   else if((ClippingRegion.dx + ClippingRegion.x) >
      (pMe->rcCurrentAppRegion.dx))
   {
      ClippingRegion.dx = (pMe->rcCurrentAppRegion.dx) - ClippingRegion.x;
   }

   if((-1 != indexNegRegion) && (0 == pMe->currentParam1[indexNegRegion]))
   {
      if(0 > ClippingRegion.y)
      {
         ClippingRegion.y = 0;
      }
   }

   if(ClippingRegion.dy == 0)
   {
      ClippingRegion.dy = pMe->rcCurrentAppRegion.dy - ClippingRegion.y;
   }
   else if((ClippingRegion.dy + ClippingRegion.y) >
      (pMe->rcCurrentAppRegion.dy))
   {
      ClippingRegion.dy = (pMe->rcCurrentAppRegion.dy) - ClippingRegion.y;
   }

   //Calculate the Source Region and Destination Region
   //Assume no Clipping

   //Source Rectangle
   pNewSrc->x = pMe->rcSetSrcRegion.x;
   pNewSrc->y = pMe->rcSetSrcRegion.y;

   if((pMe->rcSetSrcRegion.dx == 0) ||
      ((uint16)(pMe->rcSetSrcRegion.dx - pNewSrc->x) >
        pMe->currentBitmapInfo.cx))
   {
      pNewSrc->dx = (int16) pMe->currentBitmapInfo.cx - pNewSrc->x;

   }
   else
   {
      pNewSrc->dx = pMe->rcSetSrcRegion.dx;
   }

   if((pMe->rcSetSrcRegion.dy == 0) ||
      ((uint16)(pMe->rcSetSrcRegion.dy - pNewSrc->y) >
        pMe->currentBitmapInfo.cy))
   {
      pNewSrc->dy = (int16) pMe->currentBitmapInfo.cy - pNewSrc->y;

   }
   else
   {
      pNewSrc->dy = pMe->rcSetSrcRegion.dy;
   }

   indexScaleX = paramIDToIndex(OV_PARM_SCALE_X);
   if(-1 == indexScaleX)
   {
      return EFAILED;
   }
   
   indexScaleY = paramIDToIndex(OV_PARM_SCALE_Y);
   if(-1 == indexScaleY)
   {
      return EFAILED;
   }
   
   indexRotate = paramIDToIndex(OV_PARM_ROTATE);
   if(-1 == indexRotate)
   {
      return EFAILED;
   }
   
   indexDestRegion = paramIDToIndex(OV_PARM_DEST_REGION);
   if(-1 == indexDestRegion)
   {
      return EFAILED;
   }

   return OverlayHelper_CalculateSrcDest(pNewSrc, pNewDest, &ClippingRegion,
                                   pMe->currentParam2[indexScaleX], 
                                   pMe->currentParam2[indexScaleY], 
                                   (boolean)(((pMe->currentParam1[indexRotate] / 90) % 2) == 1),
                                   pMe->currentParam2[indexDestRegion]);

}

//Changes the rectangle from the apps coords to the screen coords
//based on the IDisplay that is passed in.

static void Overlay_AppToScreen(IMMOverlay *pMe, AEERect * prc)
{
   IBitmap *pDevBitmap;
   
   if (SUCCESS == IDISPLAY_GetDeviceBitmap(pMe->pIDisplay, &pDevBitmap))
   {
      AEEBitmapInfo bi;
      IBitmapCtl *pBitmapCtl;
      AEEPoint src, dest;
      dest.x = 0;
      dest.y = 0;
#if MIN_BREW_VERSION(4,0)
      if(SUCCESS == IBITMAP_QueryInterface(pDevBitmap, AEEIID_IBitmapCtl, (void **)&pBitmapCtl))
#else
      if(SUCCESS == IBITMAP_QueryInterface(pDevBitmap, AEEIID_BITMAPCTL, (void **)&pBitmapCtl))
#endif
      {
         src.x = 0;
         src.y = 0;
#if MIN_BREW_VERSION(4,0)
         dest = IBitmapCtl_AppToScreen(pBitmapCtl, src);
         (void)IBitmapCtl_Release(pBitmapCtl);
#else
         dest = IBITMAPCTL_AppToScreen(pBitmapCtl, src);
         (void)IBITMAPCTL_Release(pBitmapCtl);
#endif
      }

      if(NULL != prc)
      {
         prc->x += dest.x;
         prc->y += dest.y;
      }
      //Calculate app bitmap information
      if(SUCCESS ==  IBITMAP_GetInfo(pDevBitmap, &bi, sizeof(AEEBitmapInfo)))
      {
         SETAEERECT(&pMe->rcCurrentAppRegion, dest.x, dest.y, bi.cx, bi.cy);
         SETAEERECT(&pMe->rcCurrentScreenRegion, 0, 0, 
                    (int16)bi.cx + dest.x, (int16)bi.cy + dest.y);
      }
      (void)IBITMAP_Release(pDevBitmap);
   }
}

static void LogicalListenerCB(void * pData, const ModelEvent * pEvent)
{
   IMMOverlay * pMe = (IMMOverlay *) pData;
   int index = paramIDToIndex(OV_PARM_DISPLAY);
   if(-1 == index)
   {
      return;
   }

   switch (pEvent->evCode)
   {
      case EVT_MDL_LDC_LINKCHANGE:
         if(pEvent->dwParam == pMe->currentParam1[index])
         {
            AEERect rc;
            SETAEERECT(&rc, 0,0,0,0);
            //Overlay Registration
            UpdateLinkState(pMe);

            if(!pMe->bFirstUpdate)
            {
               (void) IMMOVERLAY_Update(pMe, &rc);
            }
         }
         return;


      default:
         return;
   }
}

static void DisplayEnableCallback(void *pData)
{
   IMMOverlay *pMe = (IMMOverlay *) pData;
   uint32 p1, p2;

   //Reset the Notify Bit
   //Recheck auto disable
   p1 = (pMe->bSetEnableState? 1: 0);
   p2 = 0;
   (void) IMMOVERLAY_SetParam(pMe, OV_PARM_ENABLE, p1, p2);


   if(pMe->pIDisplay != NULL)
   {
      (void)IDISPLAY_NotifyEnable(pMe->pIDisplay, &(pMe->CBEnable));
   }
}


//Function called to add a listener to the list
static int Overlay_AddListener( IMMOverlay *pMe, ModelListener *pListener )
{
   return OverlayHelper_AddListener( &pMe->pListeners, pListener );
}

//Function called to notify applications about the event
static void IMMOverlay_NotifyListeners( IMMOverlay *pMe, ModelEvent *pEvent )
{
   (void) IMMOVERLAY_AddRef(pMe);

   OverlayHelper_NotifyListeners( pMe->pListeners, pEvent );

   (void) IMMOVERLAY_Release(pMe);
}

/*======================================================================= 
=======================================================================*/
static int IMMOverlayModel_AddListener( IModel *po, ModelListener *pListener )
{
   IMMOverlayModel *pme = (void *)po;

   return Overlay_AddListener( pme->pMe, pListener );

}

/*======================================================================= 
=======================================================================*/
static void IMMOverlayModel_Notify( IModel *po, ModelEvent *pEvent )
{
   IMMOverlayModel *pme = (void *)po;

   IMMOverlay_NotifyListeners( pme->pMe, pEvent );
}

/*==============================================================================
Global list management
==============================================================================*/
void Overlay_Delete(IMMOverlay *pMe)
{
   uint32 i;

   // Delete everything
   // Clear first
   (void) IMMOVERLAY_Clear(pMe);

   for(i=0; i< OVERLAY_MAX_PHYSICAL; i++)
   {
      //DeRegister
      if(pMe->physicalDisplays[i].pPhysicalDisplayOverlay)
      {
         if(pMe->physicalDisplays[i].ID)
         {
            LISTENER_Cancel(&(pMe->physicalDisplays[i].physicalListener));
            (void) IPHYSICALDISPLAYOVERLAY_OverlayDeregister(pMe->physicalDisplays[i].pPhysicalDisplayOverlay,
                                                      pMe->physicalDisplays[i].ID);
            pMe->physicalDisplays[i].ID = 0;
         }
         RELEASEIF(pMe->physicalDisplays[i].pPhysicalDisplayOverlay);
      }
   }

   CALLBACK_Cancel(&(pMe->CBEnable));
   CALLBACK_Cancel(&(pMe->CBExit));

   if(pMe->pLogicalDisplayCfg != NULL)
   {
      LISTENER_Cancel(&pMe->logicalListener);
   }

   if(pMe->bCleanup)
   {
      int index = paramIDToIndex(OV_PARM_BITMAP);
      ISysMMOverlayRegistry *pSysMMOverlayRegistry;

      if(SUCCESS == ISHELL_CreateInstance(pMe->pShell, AEECLSID_SysMMOverlayRegistry,
                                          (void **)&pSysMMOverlayRegistry))
      {
         (void)ISysMMOverlayRegistry_RegisterOverlay(pSysMMOverlayRegistry, pMe, FALSE);
         (void)ISysMMOverlayRegistry_Release(pSysMMOverlayRegistry);
      }

      RELEASEIF(pMe->pIDisplay);
      RELEASEIF(pMe->pLogicalDisplayCfg);
      if(-1 != index)
      {
         RELEASEIF(pMe->currentParam1[index]);
      }
      RELEASEIF(pMe->pShell);
   }

   FREE(pMe);
}

static void Overlay_DeleteCB(void *po)
{
   IMMOverlay *pMe = (IMMOverlay *) po;
   pMe->bCleanup = FALSE;
   Overlay_Delete(pMe);
}

/*==============================================================================
INTERFACE FUNCTIONS
==============================================================================*/
/*==============================================================================

IMMOVERLAY_AddRef()

Description:

This function increments the reference count of the IMMOverlay interface object,
allowing the interface to be shared by multiple callers.  The object is freed
when the reference count reaches 0 (zero).

Prototype:

uint32 IMMOVERLAY_AddRef(IMMOverlay *p);

Parameters:
===pre>
p:  Pointer to the IMMOverlay interface object
===/pre>

Return Value:
Returns the incremented reference count for the IMMOverlay interface object.

Comments:
None

Side Effects:
None

See Also:
IMMOVERLAY_Release()

==============================================================================*/
static uint32 Overlay_AddRef(IMMOverlay *pMe)
{
   return ++pMe->dwRefs;
}

/*==============================================================================

IMMOVERLAY_Release()

Description:

This function decrements the reference count of the IMMOverlay interface object. 
The object is freed from memory and is no longer valid once its reference count
reaches 0 (zero).

Prototype:

uint32 IMMOVERLAY_Release(IMMOverlay *p);

Parameters:
===pre>
p:  Pointer to the IMMOverlay interface object
===/pre>

Return Value:
Returns the decremented reference count for the IMMOverlay interface object.  If
the returned reference count is 0 (zero), the object has been freed and will no
longer be valid.

Comments:
None

Side Effects:
None

See Also:
IMMOVERLAY_AddRef()

==============================================================================*/
static uint32 Overlay_Release(IMMOverlay *pMe)
{
   uint32 dwRefs;
   dwRefs = --pMe->dwRefs;
   if (dwRefs == 0)
   {
      Overlay_Delete(pMe);
   }
   return (dwRefs);
}


/*==============================================================================

IMMOVERLAY_QueryInterface()

Description: 
This function asks an object for another API from the object in question.

Prototype:

int IMMOVERLAY_QueryInterface(IMMOverlay *p, AEECLSID c, void **d)

Parameters:
===pre>
p:  Pointer to the IMMOverlay interface object.
c:  Requested class ID exposed by the object
d:  Pointer to the returned IMMOverlayMgr interface object.
===/pre>

Return Value:
SUCCESS          - Interface found
ENOMEMORY        - Insufficient memory
ECLASSNOTSUPPORT - Requested interface is unsupported

Comments:  
The IMMOverlay pointer in 'p' is set to the new interface (with refcount positive)
, or NULL if the ClassID is not supported by the object.  'p' must _not_ be set
to NULL.

Side Effects: 
None

See Also:
IQI_QueryInterface()


==============================================================================*/
static int Overlay_QueryInterface(IMMOverlay *pif, AEECLSID id, void **ppo)
{
   switch (id)
   {
      case AEECLSID_QUERYINTERFACE:
      case AEEIID_MMOVERLAY:
         *ppo = (void*)pif;
         (void)IQI_AddRef(*ppo);
         return SUCCESS;
      case AEEIID_MODEL:
         *ppo = (void *)&pif->theModel;
         (void) AEEBase_AddRef( (AEEBase *) *ppo );
         return SUCCESS;

      default:
         break;
   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
} 


/*==============================================================================

IMMOVERLAY_GetCapabilities()

Description: 
This function retrieves the list of possible values for a parameter to 
IMMOVERLAY_SetParam. These values represent what the given overlay supports at the
current moment. The values that are supported can change at any time.

Prototype:

int IMMOVERLAY_GetCapabilities(IMMOverlay *p, OverlayParamType paramID,
                             int32 * pSize, OverlayCapabilitiesType * pBuff);

Parameters:
===pre>
p        :  Pointer to the IMMOverlay interface object.
paramID  :  Parameter we are interested in.
pSize    :  Pointer to a variable the will return the size of the buffer needed.
pBuff    :  Pointer to a buffer to fill the possible values in. If this is NULL
            pSize will be set, but the buffer will not be used.
===/pre>

Return Value:
SUCCESS          - Operation successful
EBADPARM         - One or more of the parameters passed in is not valid
EFAILED          - Generic failure

Comments:  
The table in "comments section of OverlayParamType" documentation gives the list
of IMMOverlay supported capability parameters.

Side Effects: 
None

See Also:
IMMOVERLAYMGR_GetCapabilities()
IMMOVERLAY_SetParam()
IMMOVERLAY_GetParam()

==============================================================================*/
/*lint -save -e818 pMe can not be const */
static int Overlay_GetCapabilities(IMMOverlay *pMe, OverlayParamType paramID,
                                   int32 * pSize,
                                   OverlayCapabilitiesType * pBuff)
{
   return OverlayHelper_GetCapabilities(paramID, pSize, pBuff, pMe);
}
/*lint -restore */

/*==============================================================================

IMMOVERLAY_SetParam()

Description: 
This function sets specified the IMMOverlay Interface object parameters. 

Prototype:

int IMMOVERLAY_SetParam(IMMOverlay *p, OverlayParamType paramID,
                      uint32 p1, uint32 p2);

Parameters:
===pre>
p        :  Pointer to the IMMOverlay interface object.
paramID  :  Parameter we are interested in.
p1       :  Depends on paramID.
p2       :  Depends on paramID.
===/pre>

Return Value:
SUCCESS          - Operation successful
EBADPARM         - One or more of the parameters passed in is not valid
EFAILED          - Generic failure

Comments:  
The table in "comments section of OverlayParamType" documentation gives the list
of IMMOverlay supported settable parameters.

Side Effects: 
If the values that are specified are valid, but can not be set at the current
time, the closest value that is possible will be used and the listeners
specified in the IModel interface of IMMOverlay will be called.

See Also:
IMMOVERLAY_GetCapabilities()
IMMOVERLAY_GetParam()

==============================================================================*/
static int Overlay_SetParam(IMMOverlay *pMe, OverlayParamType paramID,
                            uint32 p1, uint32 p2)
{
   int nErr = EUNSUPPORTED;
   boolean bNonStandardStorage = FALSE;
   boolean bMayCauseRegionChange = FALSE;
   boolean bMayCauseScaleChange = FALSE;
   int paramIndex = paramIDToIndex(paramID);
   if(-1 == paramIndex)
   {
      return EBADPARM;
   }

   //Pre-Storage Special Cases
   if(paramID == OV_PARM_BITMAP)
   {
      IBitmap *pBitmap = (IBitmap *)p1;
      AEEBitmapInfo bi = {0,0,0};
      int i;
      boolean bChecked = FALSE;
      int displayIndex = paramIDToIndex(OV_PARM_DISPLAY);
      if(-1 == displayIndex)
      {
         return EFAILED;
      }

      //Validate the parameters
      nErr = OverlayHelper_VerifyParameters(pMe, paramID, p1, p2);

      if (SUCCESS != nErr)
      {
         return nErr;
      }


      if(SUCCESS != IBITMAP_GetInfo(pBitmap, &bi, sizeof(AEEBitmapInfo)))
      {
         return EFAILED;
      }

      if((bi.cx == 0) || (bi.cy == 0))
      {
         return EUNSUPPORTED;
      }

      //Update for all physical displays
      for(i=0; i< OVERLAY_MAX_PHYSICAL; i++)
      {
         //PhysicalDisplay Clear
         if(pMe->physicalDisplays[i].pPhysicalDisplayOverlay != NULL)
         {
            bChecked = TRUE;
            if(SUCCESS != IPHYSICALDISPLAYOVERLAY_ValidateBitmap(pMe->physicalDisplays[i].pPhysicalDisplayOverlay, pBitmap))
            {
               return EUNSUPPORTED;
            }
         }
      }

      if(!bChecked)
      {
         //We are not linked to any displays, Check Display 1
         IPhysicalDisplay *pPhysical;

         if(SUCCESS == ISHELL_CreateInstance(pMe->pShell, AEECLSID_PHYSICALDISPLAY1, (void **)&pPhysical))
         {
            IPhysicalDisplayOverlay *pPhysicalDisplayOverlay;
            if(SUCCESS == IPHYSICALDISPLAY_QueryInterface(pPhysical, AEEIID_PHYSICALDISPLAYOVERLAY,
                                                          (void **) &pPhysicalDisplayOverlay))
            {
               if(SUCCESS != IPHYSICALDISPLAYOVERLAY_ValidateBitmap(pPhysicalDisplayOverlay, pBitmap))
               {
                  (void) IPHYSICALDISPLAY_Release(pPhysical);
                  (void) IPHYSICALDISPLAYOVERLAY_Release(pPhysicalDisplayOverlay);
                  return EUNSUPPORTED;
               }
               (void) IPHYSICALDISPLAYOVERLAY_Release(pPhysicalDisplayOverlay);
            }
            (void) IPHYSICALDISPLAY_Release(pPhysical);
         }
      }
           
      if(MEMCMP(&pMe->currentBitmapInfo, &bi, sizeof(AEEBitmapInfo)) != 0)
      {
         pMe->currentBitmapInfo = bi;
         bMayCauseRegionChange = TRUE;
         bMayCauseScaleChange = TRUE;
      }

      RELEASEIF((pMe->currentParam1[paramIndex]));
      (void)IBITMAP_AddRef(pBitmap);
   }

   if(paramID == OV_PARM_SRC_REGION)
   {
      //Store someplace else
      if(0 == MEMCMP(&pMe->rcSetSrcRegion, (AEERect *)p1, sizeof(AEERect)))
      {
         return SUCCESS;
      }

      //Validate the parameters
      nErr = OverlayHelper_VerifyParameters(pMe, paramID, p1, p2);

      if (SUCCESS != nErr)
      {
         return nErr;
      }

      (void)MEMCPY(&pMe->rcSetSrcRegion, (AEERect *)p1, sizeof(AEERect));
      bNonStandardStorage = TRUE;
      bMayCauseRegionChange = TRUE;
      bMayCauseScaleChange = TRUE;
   }

   if(paramID == OV_PARM_DEST_REGION)
   {
      if((pMe->bSetDestRegion == TRUE) &&
         (0 == MEMCMP(&pMe->rcSetDestRegion, (AEERect *)p1, sizeof(AEERect))) &&
         (pMe->currentParam2[paramIndex] == p2))
      {
         return SUCCESS;
      }

      //Validate the parameters
      nErr = OverlayHelper_VerifyParameters(pMe, paramID, p1, p2);

      if (SUCCESS != nErr)
      {
         return nErr;
      }
      pMe->bSetDestRegion = TRUE;

      (void)MEMCPY(&pMe->rcSetDestRegion, (AEERect *)p1, sizeof(AEERect));
      
      Overlay_AppToScreen(pMe, NULL);
      pMe->currentParam2[paramIndex] = p2;
      bNonStandardStorage = TRUE;
      bMayCauseRegionChange = TRUE;
      bMayCauseScaleChange = TRUE;
   }

   if(paramID == OV_PARM_ENABLE)
   {
      int adIndex = paramIDToIndex(OV_PARM_AUTO_DISABLE);
      if( -1 == adIndex)
      {
         return EFAILED;
      }

      //Validate the parameters
      nErr = OverlayHelper_VerifyParameters(pMe, paramID, p1, p2);

      if (SUCCESS != nErr)
      {
         return nErr;
      }

      pMe->bSetEnableState = (p1 == 1);
      if((pMe->currentParam1[adIndex] == 1) &&
         (pMe->bSetEnableState == 1))
      {
         if((pMe->pIDisplay == NULL) ||
            (!IDISPLAY_IsEnabled(pMe->pIDisplay)))
         {
            p1 = 0;
         }
      }
   }

   if(paramID == OV_PARM_DISPLAY)
   {
      IDisplay * pIDisplay = pMe->pIDisplay;

      //Validate the parameters
      nErr = OverlayHelper_VerifyParameters(pMe, paramID, p1, p2);

      if (SUCCESS != nErr)
      {
         return nErr;
      }


      if(ISHELL_CreateInstance(pMe->pShell, p1,
                              (void **) &(pMe->pIDisplay)) == SUCCESS)
      {
         if(pMe->pIDisplay == NULL)
         {
            return EFAILED;
         }
      }
      else
      {
         return EFAILED;
      }

      if(pIDisplay != NULL)
      {
         CALLBACK_Cancel(&(pMe->CBEnable));
      }

      //Register for display enable/disable events
      if(SUCCESS != IDISPLAY_NotifyEnable(pMe->pIDisplay, &(pMe->CBEnable)))
      {
         (void)IDISPLAY_Release(pMe->pIDisplay);
         pMe->pIDisplay = pIDisplay;
         (void)IDISPLAY_NotifyEnable(pMe->pIDisplay, &(pMe->CBEnable));
         return EFAILED;
      }

      if(pIDisplay != NULL)
      {
         (void)IDISPLAY_Release(pIDisplay);
         pIDisplay = NULL;
      }

      bMayCauseRegionChange = TRUE;
      bMayCauseScaleChange = TRUE;
   }

   if(paramID == OV_PARM_SCALE_X)
   {
      //Store someplace else
      if((pMe->dwSetScaleFlagsX == p1) &&
         (pMe->dwSetScaleX == p2))
      {
         return SUCCESS;
      }

      //Validate the parameters
      nErr = OverlayHelper_VerifyParameters(pMe, paramID, p1, p2);

      if (SUCCESS != nErr)
      {
         return nErr;
      }

      pMe->dwSetScaleFlagsX = p1;
      pMe->dwSetScaleX = p2;
      bNonStandardStorage = TRUE;
      bMayCauseScaleChange = TRUE;
      pMe->bScaleYSetLast = FALSE;
   }

   if(paramID == OV_PARM_SCALE_Y)
   {
      //Store someplace else
      if((pMe->dwSetScaleFlagsY == p1) &&
         (pMe->dwSetScaleY == p2))
      {
         return SUCCESS;
      }

      //Validate the parameters
      nErr = OverlayHelper_VerifyParameters(pMe, paramID, p1, p2);

      if (SUCCESS != nErr)
      {
         return nErr;
      }

      pMe->dwSetScaleFlagsY = p1;
      pMe->dwSetScaleY = p2;
      bNonStandardStorage = TRUE;
      bMayCauseScaleChange = TRUE;
      pMe->bScaleYSetLast = TRUE;
   }


   if(!bNonStandardStorage)
   {
      boolean bChanged = FALSE;
      //Move data to current storage
      if(pMe->currentParam1[paramIndex] != p1)
      {
         bChanged = TRUE;
      }

      if(pMe->currentParam2[paramIndex] != p2)
      {
         bChanged = TRUE;
      }

      //Notify Listeners
      if(bChanged)
      {
         OverlayParamChangedType changeInfo;
         //Validate the parameters
         nErr = OverlayHelper_VerifyParameters(pMe, paramID, p1, p2);

         if (SUCCESS != nErr)
         {
            return nErr;
         }

         pMe->currentParam1[paramIndex] = p1;
         pMe->currentParam2[paramIndex] = p2;

         changeInfo.nParamID = paramID;
         changeInfo.p1 = p1;
         changeInfo.p2 = p2;
         OverlayHelper_Notify((void*) pMe, 
            EVT_MDL_OV_PARAMETER_CHANGED, (uint32)&changeInfo);
      }
      else
      {
         return SUCCESS;
      }
   }

   //Post Storage Special Cases
   //Calculate the scale factors
   if(bMayCauseScaleChange)
   {
      boolean bScaleXChanged = FALSE;
      boolean bScaleYChanged = FALSE;

      uint32 dwScaleFlagsX, dwScaleFlagsY;
      uint32 dwScaleX = pMe->dwSetScaleX;
      uint32 dwScaleY = pMe->dwSetScaleY;
      AEERect destRegion, srcRegion;

      //Need to add checks here
      int indexScaleX = paramIDToIndex(OV_PARM_SCALE_X);
      int indexScaleY = paramIDToIndex(OV_PARM_SCALE_Y);
      int indexRotate = paramIDToIndex(OV_PARM_ROTATE);

       if ((indexScaleX == -1) ||(indexScaleY ==-1) ||(indexRotate ==-1))
	 {
      	  	 DBGPRINTF_FATAL("Error wrong params");
			 return EFAILED;
	 }		
	  

      //Calculate the scale flags for each dimensions
         dwScaleFlagsX = pMe->dwSetScaleFlagsX;
         dwScaleFlagsY = pMe->dwSetScaleFlagsY;

      if(pMe->bScaleYSetLast == FALSE)
      {
         //X Wins
         if((pMe->dwSetScaleFlagsX & OV_FLAG_SCALE_KEEP_ASPECT))
         {
            dwScaleFlagsY |= OV_FLAG_SCALE_KEEP_ASPECT;
         }
         else
         {
            dwScaleFlagsY &= ~OV_FLAG_SCALE_KEEP_ASPECT;
         }
      }
      else
      {
         //Y Wins
         if((pMe->dwSetScaleFlagsY & OV_FLAG_SCALE_KEEP_ASPECT))
         {
            dwScaleFlagsX |= OV_FLAG_SCALE_KEEP_ASPECT;
         }
         else
         {
            dwScaleFlagsX &= ~OV_FLAG_SCALE_KEEP_ASPECT;
         }
      }

      if(pMe->currentParam1[indexScaleX] != dwScaleFlagsX)
      {
         pMe->currentParam1[indexScaleX] = dwScaleFlagsX;
         bScaleXChanged = TRUE;
      }

      
      if(pMe->currentParam1[indexScaleY] != dwScaleFlagsY)
      {
         pMe->currentParam1[indexScaleY] = dwScaleFlagsY;
         bScaleYChanged = TRUE;
      }

      //Calculate the source and destination regions
      

      destRegion.x  = pMe->rcSetDestRegion.x;
      destRegion.y  = pMe->rcSetDestRegion.y;
      destRegion.dx = pMe->rcSetDestRegion.dx;
      destRegion.dy = pMe->rcSetDestRegion.dy;

      //Get the regions for this application
      //Is the upper left corner invalid?
      if(0 > destRegion.x)
      {
         destRegion.x = 0;
      }

      if(destRegion.dx == 0)
      {
         destRegion.dx = pMe->rcCurrentAppRegion.dx;
      }
      else if((destRegion.dx + destRegion.x) > (pMe->rcCurrentAppRegion.dx))
      {
         destRegion.dx = pMe->rcCurrentAppRegion.dx - destRegion.x;
      }

      if(0 > destRegion.y)
      {
         destRegion.y = 0;
      }

      if(destRegion.dy == 0)
      {
         destRegion.dy = pMe->rcCurrentAppRegion.dy;
      }
      else if((destRegion.dy + destRegion.y) > (pMe->rcCurrentAppRegion.dy))
      {
         destRegion.dy = pMe->rcCurrentAppRegion.dy - destRegion.y;
      }

      //Calculate the Source Region and Destination Region
      //Assume no Clipping

      //Source Rectangle
      srcRegion.x = pMe->rcSetSrcRegion.x;
      srcRegion.y = pMe->rcSetSrcRegion.y;

      if((pMe->rcSetSrcRegion.dx == 0) ||
         ((uint16)(pMe->rcSetSrcRegion.dx + srcRegion.x) >
                    pMe->currentBitmapInfo.cx))
      {
         srcRegion.dx = (int16) pMe->currentBitmapInfo.cx - srcRegion.x;

      }
      else
      {
         srcRegion.dx = pMe->rcSetSrcRegion.dx;
      }

      if((pMe->rcSetSrcRegion.dy == 0) ||
         ((uint16)(pMe->rcSetSrcRegion.dy + srcRegion.y) >
                    pMe->currentBitmapInfo.cy))
      {
         srcRegion.dy = (int16) pMe->currentBitmapInfo.cy - srcRegion.y;

      }
      else
      {
         srcRegion.dy = pMe->rcSetSrcRegion.dy;
      }

      (void) OverlayHelper_CalculateScaleFactor(pMe, &srcRegion, &destRegion,
                                               (boolean)(((pMe->currentParam1[indexRotate] / 90) % 2) == 1),
                                               pMe->bScaleYSetLast,
                                               &dwScaleX, &dwScaleY, dwScaleFlagsX, dwScaleFlagsY);

      if(dwScaleX != pMe->currentParam1[indexScaleX])
      {
         pMe->currentParam2[indexScaleX] = dwScaleX;
         bScaleXChanged = TRUE;
         bMayCauseRegionChange = TRUE;
      }

      if(dwScaleY != pMe->currentParam1[indexScaleY])
      {
         pMe->currentParam2[indexScaleY] = dwScaleY;
         bScaleYChanged = TRUE;
         bMayCauseRegionChange = TRUE;
      }

      if(bScaleXChanged)
      {
         OverlayParamChangedType changeInfo;
         changeInfo.nParamID = OV_PARM_SCALE_X;
         changeInfo.p1 = pMe->currentParam1[indexScaleX];
         changeInfo.p2 = pMe->currentParam2[indexScaleX];
         OverlayHelper_Notify((void *)pMe, EVT_MDL_OV_PARAMETER_CHANGED, (uint32) &changeInfo);
      }
      if(bScaleYChanged)
      {
         OverlayParamChangedType changeInfo;
         changeInfo.nParamID = OV_PARM_SCALE_Y;
         changeInfo.p1 = pMe->currentParam1[indexScaleX];
         changeInfo.p2 = pMe->currentParam2[indexScaleY];
         OverlayHelper_Notify((void *)pMe, EVT_MDL_OV_PARAMETER_CHANGED,(uint32) &changeInfo); 
      }
   }

   //Need to recalculate Dest and Source Regions
   //for parameters that might have changed them.
   if((paramID == OV_PARM_ROTATE) ||
      (paramID == OV_PARM_REFLECT)||
      (paramID == OV_PARM_ALLOW_NEGATIVE_REGION))
   {
      bMayCauseRegionChange = TRUE;
   }

   if(bMayCauseRegionChange)
   {
      AEERect rcNewSrcRegion, rcNewDestRegion;
      if( SUCCESS == calculateClippingRegion(pMe, &rcNewSrcRegion,
                                                  &rcNewDestRegion))
      {
         if(MEMCMP(&pMe->rcCurrentSrcRegion, &rcNewSrcRegion, sizeof(AEERect)) != 0)
         {
            int indexSrcRegion = paramIDToIndex(OV_PARM_SRC_REGION);
            if(-1 != indexSrcRegion)
            {
               OverlayParamChangedType changeInfo;
               (void)MEMCPY(&pMe->rcCurrentSrcRegion, &rcNewSrcRegion, 
                            sizeof(AEERect)); 
               changeInfo.nParamID = OV_PARM_SRC_REGION;
               changeInfo.p1 = pMe->currentParam1[indexSrcRegion];
               changeInfo.p2 = pMe->currentParam2[indexSrcRegion];
               OverlayHelper_Notify((void *)pMe, EVT_MDL_OV_PARAMETER_CHANGED, 
                                    (uint32) &changeInfo); 
            }
         }

         if(MEMCMP(&pMe->rcCurrentDestRegion, &rcNewDestRegion, sizeof(AEERect)) != 0)
         {
            int indexDestRegion = paramIDToIndex(OV_PARM_DEST_REGION);
            int indexAutoClear = paramIDToIndex(OV_PARM_NO_AUTOCLEAR);
            if((-1 != indexAutoClear) && (0 == pMe->currentParam1[indexAutoClear]))
            {
               pMe->bNeedClear = TRUE;
            }
            if(-1 != indexDestRegion)
            {
               OverlayParamChangedType changeInfo;
               (void)MEMCPY(&pMe->rcCurrentDestRegion, &rcNewDestRegion, 
                      sizeof(AEERect));
               changeInfo.nParamID = OV_PARM_DEST_REGION;
               changeInfo.p1 = pMe->currentParam1[indexDestRegion];
               changeInfo.p2 = pMe->currentParam2[indexDestRegion];
               OverlayHelper_Notify((void *)pMe, EVT_MDL_OV_PARAMETER_CHANGED, 
                                    (uint32) &changeInfo); 
            }
         }
      }
   }

   if(paramID == OV_PARM_DISPLAY)
   {
      //Update Physical Display state
      if(!pMe->bFirstUpdate)
      {
         UpdateLinkState(pMe);
      }

      //Recheck auto disable
      p1 = (pMe->bSetEnableState? 1: 0);
      (void) Overlay_SetParam(pMe, OV_PARM_ENABLE, p1, p2);
   }

   if(paramID == OV_PARM_SYSTEM_OVERLAY)
   {
      ISysMMOverlayRegistry *pSysMMOverlayRegistry;
      if(SUCCESS == ISHELL_CreateInstance(pMe->pShell, AEECLSID_SysMMOverlayRegistry,
                                          (void **)&pSysMMOverlayRegistry))
      {
         nErr = ISysMMOverlayRegistry_RegisterOverlay(pSysMMOverlayRegistry,
                                                      pMe, 
                                                      (boolean)(1 == p1));
         (void)ISysMMOverlayRegistry_Release(pSysMMOverlayRegistry);
      }
   }

   return nErr;
}


/*==============================================================================

IMMOVERLAY_GetParam()

Description: 
This function gets specified the IMMOverlay Interface object parameters. 

Prototype:

int IMMOVERLAY_GetParam(IMMOverlay *p, OverlayParamType paramID,
                      uint32 * pP1, uint32 * pP2);

Parameters:
===pre>
p        :  Pointer to the IMMOverlay interface object.
paramID  :  Parameter we are interested in.
pP1      :  Depends on paramID.
pP2      :  Depends on paramID.
===/pre>

Return Value:
SUCCESS          - Operation successful
EBADPARM         - One or more of the parameters passed in is not valid
EFAILED          - Generic failure

Comments:  
The table in "comments section of OverlayParamType" documentation gives the list
of IMMOverlay supported gettable parameters.

Side Effects: 
None

See Also:
IMMOVERLAY_GetCapabilities()
IMMOVERLAY_SetParam()

==============================================================================*/
/*lint -save -e818 pMe can not be const */
static int Overlay_GetParam(IMMOverlay *pMe, OverlayParamType paramID,
                            uint32 * pP1, uint32 * pP2)
{
   boolean bNonStandardReturn = FALSE;

   int paramIndex = paramIDToIndex(paramID);
   if(-1 == paramIndex)
   {
      return EBADPARM;
   }

   if(pP1 == NULL)
      return EBADPARM;

   if(pP2 == NULL)
      return EBADPARM;

   
   //Special cases
   if(paramID == OV_PARM_BITMAP)
   {
      if(pMe->currentParam1[paramIndex] == NULL)
      {
         return EFAILED;
      }
      (void)IBITMAP_AddRef((IBitmap *)pMe->currentParam1[paramIndex]);
   }

   if(paramID == OV_PARM_SRC_REGION)
   {
      if(*pP1 == NULL)
      {
         return EBADPARM;
      }

      (void)MEMCPY((AEERect *)*pP1, &pMe->rcCurrentSrcRegion, sizeof(AEERect));
      *pP2 = pMe->currentParam2[paramIndex];

      bNonStandardReturn = TRUE;
   }

   if(paramID == OV_PARM_DEST_REGION)
   {
      if(*pP1 == NULL)
      {
         return EBADPARM;
      }

      (void)MEMCPY((AEERect *)*pP1, &pMe->rcCurrentDestRegion, sizeof(AEERect));
      *pP2 = pMe->currentParam2[paramIndex];

      bNonStandardReturn = TRUE;
   }

   if(paramID == OV_PARM_DEST_REGION_SCREEN)
   {
      if(*pP1 == NULL)
      {
         return EBADPARM;
      }

      if(*pP2 == NULL)
      {
         return EBADPARM;
      }

      (void)MEMCPY((AEERect *)*pP1, &pMe->rcCurrentDestRegion, sizeof(AEERect));
      (void)MEMCPY((AEERect *)*pP2, &pMe->rcCurrentAppRegion, sizeof(AEERect));

      Overlay_AppToScreen(pMe, (AEERect *)*pP1);

      bNonStandardReturn = TRUE;
   }


   if(!bNonStandardReturn)
   {
      *pP1 = pMe->currentParam1[paramIndex];
      *pP2 = pMe->currentParam2[paramIndex];
   }

   return SUCCESS;

}
/*lint -restore */
/*
================================================================================

IMMOVERLAY_UpdateEx()

Description: 
   This function updates the IMMOverlay on the screen. 

Prototype:

   uint32 IMMOVERLAY_UpdateEx(IMMOverlay *p, AEERect rect, uint32 dwUser, boolean bAsync);

Parameters:
===pre>
   p        :  Pointer to the IMMOverlay interface object.
   rect     :  Rectangle that describes the dirty rectangle.
               If the dx/dy values are 0 the entire Overlay will be redrawn.
               These coordinates are relative to the Bitmap that is passed in.
   dwUser   :  A user specified value. This will only be passed to the listener
               event for this update.
   bAsync   :  Is this update processed Asynchronusly.
===/pre>

Return Value:
   A unique ID assigned for this update. If this value is 0xFFFFFFFF an error
   occured and the update will not happen.

Comments:  
   None.

Side Effects: 
   This call will result cause the listeners specified in the 
   IModel interface of IMMOverlay will be called with the event 
   EVT_MDL_OV_ASYNC_UPDATE_COMPLETE.

See Also:
   IMMOverlay_AddListener

================================================================================
*/
static uint32 Overlay_UpdateEx(IMMOverlay *pMe, const AEERect *pDirtyRect,
                               uint32 dwUser, boolean bAsync)
{
   AEERect newDirty, dirtyRect;
   uint32 i;
   uint32 dwUpdateID = 0;
   int paramIndex = paramIDToIndex(OV_PARM_ENABLE);
   int indexAutoClear = paramIDToIndex(OV_PARM_NO_AUTOCLEAR);

   if(-1 == paramIndex)
   {
      return EBADPARM;
   }

   if(-1 == indexAutoClear)
   {
      return EBADPARM;
   }

   if(NULL != pDirtyRect)
   {
      dirtyRect = *pDirtyRect;
   }
   else
   {
      SETAEERECT(&dirtyRect, 0, 0, 0, 0);
   }

   if(pMe->currentParam1[paramIndex] == FALSE)
   {
      return EFAILED;
   }

   if(pMe->bNeedClear && (0 == pMe->currentParam1[indexAutoClear]))
   {
      (void) IMMOVERLAY_Clear(pMe);
   }
   pMe->bNeedClear = FALSE;

   if(dirtyRect.dx < 0)
   {
      dirtyRect.x += dirtyRect.dx;
      dirtyRect.dx = 0 - dirtyRect.dx;
   }

   if(dirtyRect.dy < 0)
   {
      dirtyRect.y += dirtyRect.dy;
      dirtyRect.dy = 0 - dirtyRect.dy;
   }

   //Commit the changes to the listeners
   OverlayHelper_Notify((void *)pMe, EVT_MDL_OV_CHANGES_COMMITTED, 0);

   //If this is the first update we need to register with the physical displays
   //Fake a LDC link change
   if(pMe->bFirstUpdate)
   {
      UpdateLinkState(pMe);
      pMe->bFirstUpdate = FALSE;
   }

   //Ensure DirtyRect is valid
   if(dirtyRect.dx == 0)
   {
      dirtyRect.dx = pMe->rcCurrentSrcRegion.x + pMe->rcCurrentSrcRegion.dx;
   }

   if(dirtyRect.dy == 0)
   {
      dirtyRect.dy = pMe->rcCurrentSrcRegion.y + pMe->rcCurrentSrcRegion.dy;
   }

   if(TRUE == bAsync)
   {
      //Find an unused entry
      unsigned index;
      for(index = 0; index < MAX_PENDING_UPDATES; index++)
      {
         if(pMe->OverlayAsyncUpdateEntry[index].bInUse == FALSE)
         {
            break;
         }
      }

      if(index == MAX_PENDING_UPDATES)
      {
         dwUpdateID = 0xFFFFFFFF;
      }
      else
      {
         dwUpdateID = index;
         pMe->OverlayAsyncUpdateEntry[index].bInUse = TRUE;
         pMe->OverlayAsyncUpdateEntry[index].dwUser = dwUser;
         pMe->OverlayAsyncUpdateEntry[index].nErr = SUCCESS;
      }
   }

   if(OverlayHelper_IntersectRect(&newDirty, &dirtyRect, &pMe->rcCurrentSrcRegion))
   {
      if((FALSE == bAsync) || (dwUpdateID != 0xFFFFFFFF))
      {
         //Update for all physical displays
         for(i=0; i< OVERLAY_MAX_PHYSICAL; i++)
         {
            //PhysicalDisplay Clear
            if(pMe->physicalDisplays[i].pPhysicalDisplayOverlay != NULL)
            {

               if(TRUE == bAsync)
               {
                  if(SUCCESS == IPHYSICALDISPLAYOVERLAY_OverlayUpdateAsync(pMe->physicalDisplays[i].pPhysicalDisplayOverlay,
                                                pMe->physicalDisplays[i].ID,
                                                &newDirty,
                                                dwUpdateID))
                  {
                     pMe->OverlayAsyncUpdateEntry[dwUpdateID].bPending[i] = TRUE;
                  }

               }
               else
               {
                  (void)IPHYSICALDISPLAYOVERLAY_OverlayUpdate(pMe->physicalDisplays[i].pPhysicalDisplayOverlay,
                                                pMe->physicalDisplays[i].ID,
                                                &newDirty);
               }
            }
         }
      }
   }
   else if(FALSE == bAsync)
   {
      dwUpdateID = 0xFFFFFFFF;
   }

   if((TRUE == bAsync) && (dwUpdateID != 0xFFFFFFFF))
   {
      int index;
      for(index = 0; index < OVERLAY_MAX_PHYSICAL; index++)
      {
         if(pMe->OverlayAsyncUpdateEntry[dwUpdateID].bPending[index] == TRUE)
         {
            break;
         }
      }
      if(index == OVERLAY_MAX_PHYSICAL)
      {
         //No updates pending
         pMe->OverlayAsyncUpdateEntry[index].bInUse = FALSE;
         dwUpdateID = 0xFFFFFFFF;
      }
   }

   return dwUpdateID;
}


/*==============================================================================

IMMOVERLAY_Update()

Description: 
This function updates the IMMOverlay on the screen. 

Prototype:

int IMMOVERLAY_Update(IMMOverlay *p, AEERect rect);

Parameters:
===pre>
p        :  Pointer to the IMMOverlay interface object.
rect     :  Rectangle that describes the dirty rectangle.
If the dx/dy values are 0 the entire Overlay will be redrawn.
These coordinates are relative to the Bitmap that is passed in.
===/pre>

Return Value:
SUCCESS          - Operation successful
EBADPARM         - One or more of the parameters passed in is not valid
EFAILED          - Generic failure

Comments:  
None.

Side Effects: 
None.

See Also:
None.

==============================================================================*/
static int Overlay_Update(IMMOverlay *pMe, const AEERect *pDirtyRect)
{
   int nErr = SUCCESS;
   uint32 dwUnique = Overlay_UpdateEx(pMe, pDirtyRect, 0, FALSE);

   if(0xFFFFFFFF == dwUnique)
   {
      nErr = EFAILED;
   }

   return nErr;
}

/*
================================================================================

IMMOVERLAY_UpdateAsync()

Description: 
   This function updates the IMMOverlay asynchronously on the screen. 

Prototype:

   uint32 IMMOVERLAY_Update(IMMOverlay *p, AEERect rect, uint32 dwUser);

Parameters:
===pre>
   p        :  Pointer to the IMMOverlay interface object.
   rect     :  Rectangle that describes the dirty rectangle.
               If the dx/dy values are 0 the entire Overlay will be redrawn.
               These coordinates are relative to the Bitmap that is passed in.
   dwUser   :  A user specified value. This will only be passed to the listener
               event for this update.
===/pre>

Return Value:
   A unique ID assigned for this update. If this value is 0xFFFFFFFF an error
   occured and the update will not happen.

Comments:  
   None.

Side Effects: 
   This call will result cause the listeners specified in the 
   IModel interface of IMMOverlay will be called with the event 
   EVT_MDL_OV_ASYNC_UPDATE_COMPLETE.

See Also:
   IMMOverlay_AddListener

================================================================================
*/

static uint32 Overlay_UpdateAsync(IMMOverlay *pMe, const AEERect *pDirtyRect, uint32 dwUser)
{
   return Overlay_UpdateEx(pMe, pDirtyRect, dwUser, TRUE);
}

/*==============================================================================

IMMOVERLAY_Clear()

Description: 
This function clears the IMMOverlay from the screen. 

Prototype:

int IMMOVERLAY_Clear(IMMOverlay *p);

Parameters:
===pre>
p        :  Pointer to the IMMOverlay interface object.
===/pre>

Return Value:
SUCCESS          - Operation successful
EFAILED          - Generic failure

Comments:  
None.

Side Effects: 
None.

See Also:
None.

==============================================================================*/
/*lint -save -e818 pMe can not be const */
static int Overlay_Clear(IMMOverlay *pMe)
{
   int nErr = SUCCESS;
   uint32 i;

   for(i=0; i< OVERLAY_MAX_PHYSICAL; i++)
   {
     //PhysicalDisplay Clear
     if(pMe->physicalDisplays[i].pPhysicalDisplayOverlay != NULL)
     {
         int nLocalErr = IPHYSICALDISPLAYOVERLAY_OverlayClear(
                                                             pMe->physicalDisplays[i].pPhysicalDisplayOverlay,
                                                             pMe->physicalDisplays[i].ID);
         if(SUCCESS != nLocalErr)
         {
            nErr = nLocalErr;
         }
     }
   }

   return nErr;
}
/*lint -restore */
/*==============================================================================
CONSTRUCTOR
==============================================================================*/
IMMOverlay * Overlay_Create(IShell * pShell)
{
   int paramIndex;
   IMMOverlay *pNew;

   static const VTBL(IMMOverlay) OverlayFuncs = 
   {
      Overlay_AddRef,
      Overlay_Release,
      Overlay_QueryInterface,
      Overlay_GetCapabilities,
      Overlay_SetParam,
      Overlay_GetParam,
      Overlay_Update,
      Overlay_UpdateAsync,
      Overlay_Clear
   };

   static IModelVtbl OverlayModelFuncs = {
      AEEBASE_AddRef(IModel),
      AEEBASE_Release(IModel),
      AEEBASE_QueryInterface(IModel),
      IMMOverlayModel_AddListener,
      IMMOverlayModel_Notify
   };

   pNew = MALLOCREC(IMMOverlay);
   if (!pNew)
   {
      DBGPRINTF_FATAL("Error MALLOC FAILED");
      return NULL;
   }

   //Clear the record
   (void)MEMSET(pNew, 0, sizeof(IMMOverlay));

   pNew->pvt = &OverlayFuncs;

   // Now initialize it
   pNew->pShell = pShell;
   (void)ISHELL_AddRef(pNew->pShell);

   pNew->dwRefs = 1;

   //Initialize the Overlay to Default configuration.
   paramIndex = paramIDToIndex(OV_PARM_LAYER);
   if(-1 != paramIndex)
   {
      pNew->currentParam1[paramIndex] = OV_LAYER_TOP_LAYER;
   }

   paramIndex = paramIDToIndex(OV_PARM_TRANSPARENCY);
   if(-1 != paramIndex)
   {
      pNew->currentParam2[paramIndex] = RGB_MASK_COLOR;
   }
   

   paramIndex = paramIDToIndex(OV_PARM_ALPHA_BLEND_PCT);
   if(-1 != paramIndex)
   {
      pNew->currentParam1[paramIndex] = 256;
   }   

   pNew->bSetEnableState = TRUE;
   paramIndex = paramIDToIndex(OV_PARM_ENABLE);
   if(-1 != paramIndex)
   {
      pNew->currentParam1[paramIndex] = TRUE;
   }     

   paramIndex = paramIDToIndex(OV_PARM_AUTO_DISABLE);
   if(-1 != paramIndex)
   {
      pNew->currentParam1[paramIndex] = TRUE;
   }

   paramIndex = paramIDToIndex(OV_PARM_MINIMIZE_CPU);
   if(-1 != paramIndex)
   {
      pNew->currentParam1[paramIndex] = TRUE;
   }

   pNew->dwSetScaleFlagsX = OV_FLAG_SCALE_KEEP_ASPECT|OV_FLAG_SCALE_TO_FIT|OV_FLAG_SCALE_NO_CROP;
   pNew->dwSetScaleX = 256;
   paramIndex = paramIDToIndex(OV_PARM_SCALE_X);
   if(-1 != paramIndex)
   {
      pNew->currentParam1[paramIndex] = pNew->dwSetScaleFlagsX;
      pNew->currentParam2[paramIndex] = pNew->dwSetScaleX;
   }     

   pNew->dwSetScaleFlagsY = OV_FLAG_SCALE_KEEP_ASPECT|OV_FLAG_SCALE_TO_FIT|OV_FLAG_SCALE_NO_CROP;
   pNew->dwSetScaleY = 256;
   paramIndex = paramIDToIndex(OV_PARM_SCALE_Y);
   if(-1 != paramIndex)
   {
      pNew->currentParam1[paramIndex] = pNew->dwSetScaleFlagsY;
      pNew->currentParam2[paramIndex] = pNew->dwSetScaleY;
   }     

   paramIndex = paramIDToIndex(OV_PARM_SRC_REGION);
   if(-1 != paramIndex)
   {
      pNew->currentParam1[paramIndex] = (uint32) &pNew->rcCurrentSrcRegion;
   }     

   paramIndex = paramIDToIndex(OV_PARM_DEST_REGION);
   if(-1 != paramIndex)
   {
      pNew->currentParam1[paramIndex] = (uint32) &pNew->rcCurrentDestRegion;
   }     

   pNew->bFirstUpdate = TRUE;

   AEEBASE_INIT( pNew, theModel, &OverlayModelFuncs );

   CALLBACK_Init(&(pNew->CBEnable), DisplayEnableCallback, pNew);
   CALLBACK_Init(&(pNew->CBExit), Overlay_DeleteCB, pNew);
   pNew->bCleanup = TRUE;
   AEE_LinkSysObject(&(pNew->CBExit));

   if(SUCCESS != ISHELL_CreateInstance(pNew->pShell, AEECLSID_LOGICALDISPLAYCFG, (void **) &pNew->pLogicalDisplayCfg))
   {
      pNew->pLogicalDisplayCfg = NULL;
   }


   if(pNew->pLogicalDisplayCfg != NULL)
   {
      IModel *pModel;
      LISTENER_Init(&pNew->logicalListener, LogicalListenerCB, (void *) pNew);

      //Register the listener
      if(SUCCESS == ILOGICALDISPLAYCFG_QueryInterface(pNew->pLogicalDisplayCfg, AEEIID_MODEL, (void **) &pModel))
      {
         if(SUCCESS != IMODEL_AddListener(pModel, &pNew->logicalListener))
         {
            (void) ILOGICALDISPLAYCFG_Release(pNew->pLogicalDisplayCfg);
            pNew->pLogicalDisplayCfg = NULL;
         }
         
         (void)IMODEL_Release(pModel);
      }
   }

   return pNew;
}


#endif /* FEATURE_OVERLAY */


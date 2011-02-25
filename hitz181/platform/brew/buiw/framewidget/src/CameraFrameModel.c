/*
  ========================================================================

  FILE: CameraFrameModel.c
  
  SERVICES: 

  GENERAL DESCRIPTION:  


  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "CameraFrameModel.h"
#include "AEEBase.h"
#include "AEEDirectMode.h"
#include "fbase.h"
#include "util.h"


#define MAKE_ME   CameraFrameModel *me = (CameraFrameModel*)po

#define FARF_EVENTS      0

#ifndef FARF
#define FARF(x, p)  if (0 != FARF_##x) DBGPRINTF p 
#endif


typedef struct CameraFrameModel CameraFrameModel;


struct IDirectMode {
   AEEBASE_INHERIT(IDirectMode, CameraFrameModel);
};

struct CameraFrameModel {

   ModelBase         base;       // model base
   ICamera *         piCamera;   // ICamera object
   int               cx;         // width of the frame
   int               cy;         // height of the frame
   int               x;          // horz offset of the frame
   int               y;          // vert offset of the frame
   PFNCAMERANOTIFY   pfnNotify;  // notify callback
   void *            pUser;      // notify context
   int               nDirect;
   IDirectMode       directMode;
   AEEVTBL(IDirectMode) vtDirectMode;
   flg               bFrameReady:1;
};

/////////////////////////////////////////////////////////////////
// CameraFrameModel


static __inline void CameraFrameModel_SetCameraSize(CameraFrameModel *me) {
   if (me->piCamera && me->cx > 0 && me->cy > 0) {
      AEESize size;
      size.cx = me->cx;
      size.cy = me->cy;
      ICAMERA_SetDisplaySize(me->piCamera, &size);
   }
}


static void CameraFrameModel_NotifyCB(CameraFrameModel *me, AEECameraNotify *pCmd)
{
   FARF(EVENTS, ("nCmd=%d, nSub=%d, nStatus=%d, pData=%lx, dwSize=%d",
             pCmd->nCmd, pCmd->nSubCmd, pCmd->nStatus, 
             pCmd->pData, pCmd->dwSize));

   if (pCmd->nCmd == CAM_CMD_START && pCmd->nStatus == CAM_STATUS_FRAME) {
      ModelEvent mev;
      mev.evCode  = EVT_FRAMEMDL_FRAME;
      mev.pModel  = (IModel*)&me->base;
      mev.dwParam = 0;     
      me->bFrameReady = TRUE;
      ModelBase_Notify((IModel*)&me->base, &mev);
      me->bFrameReady = FALSE;



   } else if (me->pfnNotify) {
      me->pfnNotify(me->pUser, pCmd);
   }
}


static int CameraFrameModel_GetFrame(ICameraFrameModel *po, IBitmap **ppo)
{
   MAKE_ME;

   if (!me->piCamera || !me->bFrameReady) {
      *ppo = 0;
      return EBADSTATE;
   }
   return ICAMERA_GetFrame(me->piCamera, ppo);
}


static void CameraFrameModel_SetSize(ICameraFrameModel *po, int cx, int cy)
{
   MAKE_ME;
   
   if ((me->cx != cx) || (me->cy != cy)) {
      me->cx = cx;
      me->cy = cy;
      CameraFrameModel_SetCameraSize(me);
   }
}


static void CameraFrameModel_GetSize(ICameraFrameModel *po, int *pcx, int *pcy)
{
   MAKE_ME;

   *pcx = me->cx;
   *pcy = me->cy;
}


static void CameraFrameModel_SetOffset(ICameraFrameModel *po, int x, int y)
{
   MAKE_ME;
   
   me->x = x;
   me->y = y;
}


static void CameraFrameModel_GetOffset(ICameraFrameModel *po, int *px, int *py)
{
   MAKE_ME;

   *px = me->x;
   *py = me->y;
}


static int CameraFrameModel_SetICamera(ICameraFrameModel *po, ICamera *piCamera)
{
   MAKE_ME;
   int nErr = SUCCESS;

   if(me->piCamera) {
      (void) ICAMERA_RegisterNotify(me->piCamera, NULL, NULL);
      ICAMERA_Release(me->piCamera);
   }
   me->piCamera = piCamera;

   if (me->piCamera) {
      
      ICAMERA_AddRef(me->piCamera);
      me->bFrameReady = FALSE;
      // register our notify callback
      nErr = ICAMERA_RegisterNotify(me->piCamera, (PFNCAMERANOTIFY)CameraFrameModel_NotifyCB, me);

      // Set the frame display size where the captured data is displayed
      CameraFrameModel_SetCameraSize(me);
   }

   return nErr;
}


static void CameraFrameModel_GetICamera(ICameraFrameModel *po, ICamera **ppo)
{
   MAKE_ME;

   *ppo = me->piCamera;
   ADDREFIF(me->piCamera);
}


static void CameraFrameModel_RegisterNotify(ICameraFrameModel *po, PFNCAMERANOTIFY pfnNotify, void * pUser)
{
   MAKE_ME;
   me->pfnNotify  = pfnNotify;
   me->pUser      = pUser;
}


static int CameraFrameModel_QueryInterface(ICameraFrameModel *po, AEECLSID id, void **ppo)
{
   MAKE_ME;
   if (id == AEEIID_CAMERAFRAMEMODEL || id == AEEIID_FRAMEMODEL) {
      *ppo = po;
      ModelBase_AddRef((IModel*)po);
      return SUCCESS;
   }
   else if (id == AEEIID_DIRECTMODE) {
      *ppo = &me->directMode;
      (void)ModelBase_AddRef((IModel*) po);
      return SUCCESS;
   }
   return ModelBase_QueryInterface((IModel*)po, id, ppo);
}


static void CameraFrameModel_Dtor(CameraFrameModel *me)
{
   if(me->piCamera) {
      (void) ICAMERA_RegisterNotify(me->piCamera, NULL, NULL);
      ICAMERA_Release(me->piCamera);
      me->piCamera = NULL;
   }
}


static uint32 CameraFrameModel_Release(ICameraFrameModel *po)
{
   MAKE_ME;

   if (me->base.nRefs == 1) {
      CameraFrameModel_Dtor(me);
   }
   return ModelBase_Release((IModel*)po);
}

static int CameraFrameModel_SetDirectMode(IDirectMode *po, int nDirect, AEERect *prc) 
{
   CameraFrameModel *me = (CameraFrameModel*)po->pMe;
   int nErr = SUCCESS;
   AEESize size;
#ifndef CAM_PARM_LCD_DIRECT_ACCESS
#define CAM_PARM_LCD_DIRECT_ACCESS  (CAM_PARM_BASE + 18)    // [Set/Get] Camera services layer renders frames directly on LCD
#endif
   if(me->piCamera && (nDirect & DMF_ENABLE)) {
      size.cx = prc->x;
      size.cy = prc->y;
      nErr = ICAMERA_SetParm(me->piCamera,CAM_PARM_LCD_DIRECT_ACCESS, (int32) TRUE,  (int32) &size);
      me->x = prc->x;
      me->y = prc->y;
      me->cx = prc->dx;
      me->cy = prc->dy;
      CameraFrameModel_SetCameraSize(me);
   } else {
      size.cx = me->x;
      size.cy = me->y;
      nErr = ICAMERA_SetParm(me->piCamera,CAM_PARM_LCD_DIRECT_ACCESS, (int32) FALSE,  (int32) &size);
   }
   if(SUCCESS == nErr) {
      me->nDirect = nDirect;
   }
   return nErr;

}
static int CameraFrameModel_GetDirectMode(IDirectMode *po, int *pnDirect, AEERect *prc) 
{
   CameraFrameModel *me = (CameraFrameModel*) po->pMe;

   if(pnDirect) {
      *pnDirect = me->nDirect;
   }
   if(prc) {
      prc->x = (int16) me->x;
      prc->y = (int16) me->y;
      prc->dx = (int16) me->cx;
      prc->dy = (int16) me->cy;
   }
   return SUCCESS;

}

static void CameraFrameModel_Ctor(CameraFrameModel *me, AEEVTBL(ICameraFrameModel) *pvt, IModule *piModule)
{
   // construct base and init IModel vtbl
   ModelBase_Ctor(&me->base, (AEEVTBL(IModel)*) pvt, piModule);

   // init ICameraFrameModel vtbl
   pvt->Release         = CameraFrameModel_Release;          // override Release
   pvt->QueryInterface  = CameraFrameModel_QueryInterface;   // override QI
   pvt->GetFrame        = CameraFrameModel_GetFrame;
   pvt->SetSize         = CameraFrameModel_SetSize;
   pvt->GetSize         = CameraFrameModel_GetSize;
   pvt->SetOffset       = CameraFrameModel_SetOffset;
   pvt->GetOffset       = CameraFrameModel_GetOffset;
   pvt->SetICamera      = CameraFrameModel_SetICamera;
   pvt->GetICamera      = CameraFrameModel_GetICamera;
   pvt->RegisterNotify  = CameraFrameModel_RegisterNotify;
   
   AEEBASE_INIT(me, directMode, &me->vtDirectMode);
   me->vtDirectMode.AddRef         = AEEBASE_AddRef(IDirectMode);
   me->vtDirectMode.Release        = AEEBASE_Release(IDirectMode);
   me->vtDirectMode.QueryInterface = AEEBASE_QueryInterface(IDirectMode);
   me->vtDirectMode.SetDirectMode  = CameraFrameModel_SetDirectMode;
   me->vtDirectMode.GetDirectMode  = CameraFrameModel_GetDirectMode;
}


int CameraFrameModel_New(ICameraFrameModel **ppo, IModule *piModule)
{
   CameraFrameModel *me = MALLOCREC_VTBL(CameraFrameModel, ICameraFrameModel);

   *ppo = (ICameraFrameModel*)me;

   if (!me) {
      return ENOMEMORY;
   }

   CameraFrameModel_Ctor(me, GETVTBL(me,ICameraFrameModel), piModule);
   return SUCCESS;
}




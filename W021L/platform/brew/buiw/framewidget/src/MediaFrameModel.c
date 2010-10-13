/*
  ========================================================================

  FILE: MediaFrameModel.c
  
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
#include "MediaFrameModel.h"
#include "AEEBase.h"
#include "AEEDirectMode.h"
#include "fbase.h"
#include "util.h"


#define MAKE_ME   MediaFrameModel *me = (MediaFrameModel*)po

#define FARF_EVENTS     0

#ifndef FARF
#define FARF(x, p)  if (0 != FARF_##x) DBGPRINTF p 
#endif

typedef struct MediaFrameModel MediaFrameModel;


struct IDirectMode {
   AEEBASE_INHERIT(IDirectMode, MediaFrameModel);
};

struct MediaFrameModel {

   ModelBase         base;       // model base
   IMedia *          piMedia;    // IMedia object
   int               cx;         // width of the frame
   int               cy;         // height of the frame
   int               x;          // horz offset of the frame
   int               y;          // vert offset of the frame
   PFNMEDIANOTIFY    pfnNotify;  // notify callback
   void *            pUser;      // notify context
   int               nDirect;
   IDirectMode       directMode;
   AEEVTBL(IDirectMode) vtDirectMode;
   flg               bFrameReady:1;
};


/////////////////////////////////////////////////////////////////
// MediaFrameModel


static __inline void MediaFrameModel_SetMediaRect(MediaFrameModel *me) {
   if (me->piMedia) {
      AEERect rc;
      rc.x = me->x;
      rc.y = me->y;
      rc.dx = me->cx;
      rc.dy = me->cy;
      IMEDIA_SetRect(me->piMedia, &rc, &rc);
   }
}


static void MediaFrameModel_NotifyCB(MediaFrameModel *me, AEEMediaCmdNotify *pCmd)
{
   boolean bSendEvent = FALSE;
   uint32 eCode = EVT_FRAMEMDL_FRAME;

   FARF(EVENTS, ("(%lx), nCmd=%d, nSub=%d, nStatus=0x%lx, pCmdData=0x%lx, dwSize=%d",
             pCmd->clsMedia, pCmd->nCmd, pCmd->nSubCmd, pCmd->nStatus, 
             pCmd->pCmdData, pCmd->dwSize));

   // Frame callback
   if( (pCmd->nCmd == MM_CMD_PLAY) && (pCmd->nStatus == MM_STATUS_FRAME)){
	   bSendEvent = TRUE;
	   me->bFrameReady = TRUE;
   }

   // Seek occurred
   bSendEvent |= (pCmd->nCmd == MM_CMD_PLAY && pCmd->nStatus == MM_STATUS_SEEK);

   // Video stopped
   if((pCmd->nCmd == MM_CMD_PLAY && pCmd->nStatus == MM_STATUS_DONE)) {
      bSendEvent |=TRUE;
      eCode = EVT_FRAMEMDL_DONE;
   }

   // IMedia was set
   bSendEvent |= (pCmd->nCmd == MM_CMD_SETMEDIAPARM && pCmd->nStatus == MM_STATUS_DONE 
      && pCmd->nSubCmd == MM_PARM_MEDIA_DATA);

   if (bSendEvent) {
      ModelEvent mev;
      mev.evCode  = eCode;
      mev.pModel  = (IModel*)&me->base;
      mev.dwParam = 0;
      ModelBase_Notify((IModel*)&me->base, &mev);
   }

   if (me->pfnNotify) {
      me->pfnNotify(me->pUser, pCmd);
   }
   // Frame callback
   if( (pCmd->nCmd == MM_CMD_PLAY) && (pCmd->nStatus == MM_STATUS_FRAME)){
      me->bFrameReady = FALSE;
   }
}


static int MediaFrameModel_GetFrame(IMediaFrameModel *po, IBitmap **ppo)
{
   MAKE_ME;

   if (!me->piMedia || !me->bFrameReady) {
      *ppo = 0;
      return EBADSTATE;
   }
   return IMEDIA_GetFrame(me->piMedia, ppo);
}


static void MediaFrameModel_SetSize(IMediaFrameModel *po, int cx, int cy)
{
   MAKE_ME;
   me->cx = cx;
   me->cy = cy;
   MediaFrameModel_SetMediaRect(me);
}


static void MediaFrameModel_GetSize(IMediaFrameModel *po, int *pcx, int *pcy)
{
   MAKE_ME;
   *pcx = me->cx;
   *pcy = me->cy;
}


static void MediaFrameModel_SetOffset(IMediaFrameModel *po, int x, int y)
{
   MAKE_ME;
   me->x = x;
   me->y = y;
   MediaFrameModel_SetMediaRect(me);
}


static void MediaFrameModel_GetOffset(IMediaFrameModel *po, int *px, int *py)
{
   MAKE_ME;
   *px = me->x;
   *py = me->y;
}


static int MediaFrameModel_SetIMedia(IMediaFrameModel *po, IMedia *piMedia)
{
   MAKE_ME;
   int nErr = SUCCESS;

   RELEASEIF(me->piMedia);

   if (piMedia) {
      
      me->piMedia = piMedia;
      ADDREFIF(me->piMedia);

	  me->bFrameReady = FALSE;

      // register our notify callback
      nErr = IMEDIA_RegisterNotify(me->piMedia, (PFNMEDIANOTIFY)MediaFrameModel_NotifyCB, me);

      // enable frame callbacks
      if (!nErr) {
         nErr = IMEDIA_EnableFrameCallback(me->piMedia, TRUE);
      }

      if (!nErr) {
         MediaFrameModel_SetMediaRect(me);
      }
   }

   // Send notification to frame widget so that it can invalidate and redraw.
   // Also send notification that the media has changed.
   if ((IModel*)&me->base) {
      
      ModelEvent mev;
      mev.pModel  = (IModel*)&me->base;
      mev.dwParam = 0;

      mev.evCode = EVT_FRAMEMDL_MEDIACHANGED;
      ModelBase_Notify((IModel*)&me->base, &mev);

      mev.evCode  = EVT_FRAMEMDL_FRAME;
      ModelBase_Notify((IModel*)&me->base, &mev);
   }

   return nErr;
}


static void MediaFrameModel_GetIMedia(IMediaFrameModel *po, IMedia **ppo)
{
   MAKE_ME;

   *ppo = me->piMedia;
   ADDREFIF(me->piMedia);
}


static void MediaFrameModel_RegisterNotify(IMediaFrameModel *po, PFNMEDIANOTIFY pfnNotify, void * pUser)
{
   MAKE_ME;
   me->pfnNotify  = pfnNotify;
   me->pUser      = pUser;
}


static int MediaFrameModel_QueryInterface(IMediaFrameModel *po, AEECLSID id, void **ppo)
{
   MediaFrameModel *me = (MediaFrameModel*) po;
   if (id == AEEIID_MEDIAFRAMEMODEL || id == AEEIID_FRAMEMODEL ) {
      *ppo = po;
      ModelBase_AddRef((IModel*)po);
      return SUCCESS;
   }
   else if (id == AEEIID_DIRECTMODE) {
      *ppo = &me->directMode;
      (void)ModelBase_AddRef((IModel*)po);
      return SUCCESS;
   }

   return ModelBase_QueryInterface((IModel*)po, id, ppo);
}


static void MediaFrameModel_Dtor(MediaFrameModel *me)
{
   RELEASEIF(me->piMedia);
}


static uint32 MediaFrameModel_Release(IMediaFrameModel *po)
{
   MAKE_ME;

   if (me->base.nRefs == 1) {
      MediaFrameModel_Dtor(me);
   }
   return ModelBase_Release((IModel*)po);
}


static int MediaFrameModel_SetDirectMode(IDirectMode *po, int nDirect, AEERect *prc) 
{
   MediaFrameModel *me = (MediaFrameModel*)(po->pMe);
   int nErr = SUCCESS;

   me->nDirect = nDirect;
   if(me->piMedia) {
      nErr = IMEDIA_EnableFrameCallback(me->piMedia, !(me->nDirect & DMF_ENABLE));
   }
   if(nDirect && me->piMedia && prc && 
      ((me->x != prc->x) ||(me->y != prc->y) ||
      (me->cx != prc->dx) || (me->cy != prc->dy))) {
      me->x = prc->x;
      me->y = prc->y;
      me->cx = prc->dx;
      me->cy = prc->dy;
      (void)IMEDIA_SetRect(me->piMedia, prc, prc);
   }

   return nErr;

}
static int MediaFrameModel_GetDirectMode(IDirectMode *po, int *pnDirect, AEERect *prc) 
{
   MediaFrameModel *me = (MediaFrameModel*)(po->pMe);

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

static void MediaFrameModel_Ctor(MediaFrameModel *me, AEEVTBL(IMediaFrameModel) *pvt, IModule *piModule)
{
   // construct base and init IModel vtbl
   ModelBase_Ctor(&me->base, (AEEVTBL(IModel)*) pvt, piModule);

   // init IMediaFrameModel vtbl
   pvt->Release         = MediaFrameModel_Release;          // override Release
   pvt->QueryInterface  = MediaFrameModel_QueryInterface;   // override QI
   pvt->GetFrame        = MediaFrameModel_GetFrame;
   pvt->SetSize         = MediaFrameModel_SetSize;
   pvt->GetSize         = MediaFrameModel_GetSize;
   pvt->SetOffset       = MediaFrameModel_SetOffset;
   pvt->GetOffset       = MediaFrameModel_GetOffset;
   pvt->SetIMedia       = MediaFrameModel_SetIMedia;
   pvt->GetIMedia       = MediaFrameModel_GetIMedia;
   pvt->RegisterNotify  = MediaFrameModel_RegisterNotify;
}


int MediaFrameModel_New(IMediaFrameModel **ppo, IModule *piModule)
{
   MediaFrameModel *me = MALLOCREC_VTBL(MediaFrameModel, IMediaFrameModel);

   *ppo = (IMediaFrameModel*)me;

   if (!me) {
      return ENOMEMORY;
   }

   MediaFrameModel_Ctor(me, GETVTBL(me,IMediaFrameModel), piModule);
   
   
   AEEBASE_INIT(me, directMode, &me->vtDirectMode);
   me->vtDirectMode.AddRef         = AEEBASE_AddRef(IDirectMode);
   me->vtDirectMode.Release        = AEEBASE_Release(IDirectMode);
   me->vtDirectMode.QueryInterface = AEEBASE_QueryInterface(IDirectMode);
   me->vtDirectMode.SetDirectMode  = MediaFrameModel_SetDirectMode;
   me->vtDirectMode.GetDirectMode  = MediaFrameModel_GetDirectMode;

   return SUCCESS;
}




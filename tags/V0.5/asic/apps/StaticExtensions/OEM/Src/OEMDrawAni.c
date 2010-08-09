/*******************************************************************
 *	Copyright(c)2009-2010 TCL CDMA SW developement Team Reserved
 *
 ********************************************************************/
/********************************************************************
 *
 * Team:	            CDMA JRD SW
 * $Author:	         Qiong.chen Create$
 * $Rev:: 27          $
 * $Date:: 2009-03-27#$
 *	Qiong.chen Create 2009/03/13
 *
 *********************************************************************/

#include "AEEDrawAni.h"

//-----------------------------------------------------------------------
//    Types and Macros
//-----------------------------------------------------------------------
typedef struct CAniCtl
{
   AEEVTBL(IAniCtl) *pvt;
   long  m_nRefs;
   IDisplay * m_pDisplay;
   IImage *pImg;
   IShell *m_pShell;
   DRAW_MSG_CB_TYPE app_callback;
   void *pUser;


   int32 aniRate;
   int16 framecount;
   int16 frame_indx;
   rect pos;
   rect ImageSize;

   draw_play_times_style playstyle;
   int16 playstyletimes;
   int16 playcustcount;

   draw_info_data_type mdata;
} CAniCtl;

#define OEMDRAWANI_ANIRATE_MIN         150//150ms
#define OEMDRAWANI_DEFAULT_ANIRATE     300//300ms

//--------------------------------------------------------------------
//   Function Prototypes
//--------------------------------------------------------------------
int AniCtl_New(IShell * pIShell, AEECLSID cls, void ** ppobj);

static uint32 OEMCAniCtl_AddRef(IAniCtl *po);

static uint32 OEMCAniCtl_Release(IAniCtl *po);

static int OEMCAniCtl_Init(CAniCtl *pMe);

static int OEMCAniCtl_RegisterCallBack(IAniCtl *po,
                                       DRAW_MSG_CB_TYPE callback,
                                       void *pUser);

static void OEMCAniCtl_DrawCallBack(void * po, draw_event_type event);

static boolean OEMCAniCtl_SetPosParm(IAniCtl *po, int32 px, int32 py);

static boolean OEMCAniCtl_SetAniRate(IAniCtl *p, int32 AniTime);

static boolean OEMCAniCtl_SetImageSize(IAniCtl *po, int32 wx, int32 hy);

static boolean OEMCAniCtl_SetFrameCount(IAniCtl *po, uint16 num);

static boolean OEMCAniCtl_SetPlayTimes(IAniCtl *po,
                                       draw_play_times_style style,
                                       int16 num);

static int OEMCAniCtl_StartDisplay(IAniCtl * pMe, IImage *pImg);

static int OEMCAniCtl_Display(IAniCtl * pMe);

static int OEMCAniCtl_StartAni(IAniCtl *pMe);

static int OEMCAniCtl_StopAni(IAniCtl *pMe);


//--------------------------------------------------------------------
//   Globals
//--------------------------------------------------------------------
static const VTBL(IAniCtl)gCAniCtlFuncs = {
   OEMCAniCtl_AddRef,
   OEMCAniCtl_Release,
   OEMCAniCtl_RegisterCallBack,
   OEMCAniCtl_DrawCallBack,
   OEMCAniCtl_SetPosParm,
   OEMCAniCtl_SetAniRate,
   OEMCAniCtl_SetImageSize,
   OEMCAniCtl_SetFrameCount,
   OEMCAniCtl_SetPlayTimes,
   OEMCAniCtl_StartDisplay,
   OEMCAniCtl_Display,
   OEMCAniCtl_StartAni,
   OEMCAniCtl_StopAni,
};

//--------------------------------------------------------------------
//   Function Definitions
//--------------------------------------------------------------------
/*=====================================================================

  Public Method - Increments the reference count on the menu class object.

  ======================================================================*/
int AniCtl_New(IShell * pIShell, AEECLSID cls, void ** ppobj)
{
   CAniCtl * pme = NULL;

   if (!pIShell || !ppobj)
      return(EBADPARM);

   *ppobj = NULL;

   pme = (CAniCtl *)AEE_NewClass((AEEVTBL(IBase) *)&gCAniCtlFuncs,
                                 sizeof(CAniCtl));

   if (pme == NULL)
   {
      return ENOMEMORY;
   }

   pme->m_pShell  = pIShell;
   (void)ISHELL_AddRef(pIShell);

   if(SUCCESS != OEMCAniCtl_Init(pme))
   {
      OEMCAniCtl_Release((IAniCtl *)pme);
      return EFAILED;
   }

   pme->m_nRefs = 1;
   *ppobj = pme;
   return AEE_SUCCESS;
}

static uint32 OEMCAniCtl_AddRef(IAniCtl * pMe)
{
   return(++((CAniCtl*)pMe)->m_nRefs);
}

static uint32 OEMCAniCtl_Release(IAniCtl *po)
{
   CAniCtl *pMe = (CAniCtl *)po;

   if (pMe == NULL)
   {
      return SUCCESS;
   }

   if (--pMe->m_nRefs != 0)
   {
      return pMe->m_nRefs;
   }

   if (pMe->pImg != NULL)
   {
      IIMAGE_Release(pMe->pImg);
      pMe->pImg = NULL;
   }

   if(pMe->m_pDisplay != NULL)
   {
      IDISPLAY_Release(pMe->m_pDisplay);
      pMe->m_pDisplay = NULL;
   }

   OEMCAniCtl_StopAni(po);

   ISHELL_Release(pMe->m_pShell);

   MEMSET(pMe,0,sizeof(CAniCtl));

   FREE(pMe);
   return SUCCESS;
}

static int OEMCAniCtl_Init(CAniCtl *pMe)
{
   int nRet = SUCCESS;

   if(pMe == NULL)
   {
      return FALSE;
   }

   pMe->app_callback = NULL;
   pMe->pUser = NULL;

   pMe->pImg = NULL;
   pMe->framecount = 1;
   pMe->aniRate = OEMDRAWANI_DEFAULT_ANIRATE;
   pMe->frame_indx = 0;
   pMe->playstyle = DRAW_PLAY_ONCE;
   pMe->playstyletimes = 0;
   pMe->playcustcount = 0;

   if(NULL == pMe->m_pDisplay)
   {
      nRet = ISHELL_CreateInstance(pMe->m_pShell,
                                   AEECLSID_DISPLAY,
                                   (void **)&pMe->m_pDisplay);
      if(SUCCESS != nRet)
      {
         return EFAILED;
      }
   }

   return SUCCESS;
}

static int OEMCAniCtl_RegisterCallBack(IAniCtl *po,
                                       DRAW_MSG_CB_TYPE callback,
                                       void *pUser)
{
   CAniCtl *pMe = (CAniCtl *)po;

   if(callback == NULL || pUser == NULL)
   {
      DBGPRINTF("Register call back : bad parameter!");
      return FALSE;
   }

   pMe->app_callback = callback;
   pMe->pUser = pUser;
   return TRUE;
}

static void OEMCAniCtl_DrawCallBack(void *po,draw_event_type event)
{
   CAniCtl *pMe = (CAniCtl *)po;
   draw_event_type m_event;

   switch(event)
   {
   case DRAW_EVENT_START:

      break;

   case DRAW_EVENT_NEXT:
      OEMCAniCtl_StopAni((IAniCtl *)pMe);

      if(pMe->frame_indx >= 6)
      {
         return ;
      }
      pMe->frame_indx++;
      DBGPRINTF("222 pMe->frame_indx =  %d",pMe->frame_indx);

      ISHELL_SetTimer(pMe->m_pShell,2000,(PFNNOTIFY)OEMCAniCtl_StartDisplay,pMe);
      break;

   case DRAW_EVENT_PRV:
      OEMCAniCtl_StopAni(po);

      if(pMe->frame_indx == 0)
      {
         return ;
      }
      pMe->frame_indx--;
      OEMCAniCtl_StartDisplay(po,pMe->pImg);

      break;

   case DRAW_EVENT_DONE:
      break;

   case DRAW_EVENT_STOP:
      break;

   default:

      break;

   }
}

static boolean OEMCAniCtl_SetPosParm(IAniCtl *po,int32 px,int32 py)
{
   CAniCtl *pMe = (CAniCtl *)po;
   DBGPRINTF("OEMCAniCtl_SetPosParm");

   pMe->pos.posx = px;
   pMe->pos.posy = py;
   return TRUE;
}

static boolean OEMCAniCtl_SetAniRate(IAniCtl *po,int32 AniTime)
{
   CAniCtl *pMe = (CAniCtl *)po;

   if(AniTime < OEMDRAWANI_ANIRATE_MIN)
   {
      DBGPRINTF("OEMCAniCtl_SetAniRate too short");
      AniTime = OEMDRAWANI_ANIRATE_MIN;
   }

   pMe->aniRate = AniTime;
   return TRUE;
}

static boolean OEMCAniCtl_SetImageSize(IAniCtl *po,int32 wx,int32 hy)
{
   CAniCtl *pMe = (CAniCtl *)po;

   DBGPRINTF("OEMCAniCtl_SetImageSize");
   pMe->ImageSize.posx = wx;
   pMe->ImageSize.posy = hy;
   return TRUE;
}

static boolean OEMCAniCtl_SetFrameCount(IAniCtl *po,uint16 num)
{
   CAniCtl *pMe = (CAniCtl *)po;

   if(num == 0)
   {
      return FALSE;
   }

   DBGPRINTF("OEMCAniCtl_SetFrameCount");
   pMe->framecount = num;
   return TRUE;
}

static boolean OEMCAniCtl_SetPlayTimes(IAniCtl *po,draw_play_times_style style,int16 num)
{
   CAniCtl *pMe = (CAniCtl *)po;

   if ( num <= 0)
   {
      DBGPRINTF("Bad parameter");
      return FALSE;
   }


   pMe->playstyle = style;

   pMe->playstyletimes = num;

   return TRUE;
}

static int OEMCAniCtl_StartDisplay(IAniCtl *po,IImage *pImg)
{
   CAniCtl *pMe = (CAniCtl *)po;
   AEEImageInfo  ImgInfo;

   if(NULL != pImg)
   {
      DBGPRINTF("Start Display : bad parameter");
      return EFAILED;
   }

   if(NULL != pMe->pImg)
   {
      IIMAGE_Release(pMe->pImg);
      pMe->pImg = NULL;
   }

   pMe->pImg = pImg;
   IIMAGE_AddRef(pImg);

   IIMAGE_GetInfo(pMe->pImg, &ImgInfo);

   if(ImgInfo.bAnimated == FALSE)
   {
      if(pMe->framecount > 1)
      {
         IIMAGE_SetFrameCount(pMe->pImg, pMe->framecount);
         IIMAGE_SetDrawSize( pMe->pImg,
                            ImgInfo.cx/pMe->framecount,
                            ImgInfo.cy);

      }
   }
   else
   {
      pMe->framecount = ImgInfo.cxFrame;
   }

   pMe->frame_indx = 0;
   pMe->playstyletimes = 0;
   pMe->playcustcount = 0;

   OEMCAniCtl_StartAni(po);

   return TRUE;
}

static int OEMCAniCtl_Display(IAniCtl *po)
{
   draw_event_type m_event;
   CAniCtl *pMe = (CAniCtl *)po;
   DBGPRINTF("OEMCAniCtl_Display");

   if(pMe->framecount == 0)
   {
      pMe->framecount = 6;
      IIMAGE_SetFrameCount(pMe->pImg, 6);
   }
   else
   {
      IIMAGE_SetFrameCount(pMe->pImg, pMe->framecount);
   }


   IIMAGE_DrawFrame(pMe->pImg,pMe->frame_indx,pMe->pos.posx,pMe->pos.posy);
   IDISPLAY_Update(pMe->m_pDisplay);
   return SUCCESS;
}

static int OEMCAniCtl_SetPlayStyleOption(IAniCtl *po)
{
   draw_event_type m_event;
   CAniCtl *pMe = (CAniCtl *)po;

   switch(pMe->playstyle)
   {
   case DRAW_PLAY_CIRCULATION:

      pMe->frame_indx = 0;
      m_event = DRAW_EVENT_NEXT;
      pMe->app_callback(po,m_event);

      break;

   case DRAW_PLAY_CUSTOM:
      if(pMe->playcustcount >= pMe->playstyletimes)
      {
         m_event = DRAW_EVENT_DONE;
         pMe->app_callback(po,m_event);
      }
      else
      {
         pMe->frame_indx = 0;
         pMe->playcustcount++;
         m_event = DRAW_EVENT_NEXT;
         pMe->app_callback(po,m_event);
      }

      break;

   default:
      m_event = DRAW_EVENT_DONE;
      pMe->app_callback(po,m_event);
      break;

   }
   return TRUE;
}

static int OEMCAniCtl_StartAni(IAniCtl *po)
{
   draw_event_type m_event;
   CAniCtl *pMe = (CAniCtl *)po;

   if(pMe->frame_indx >= pMe->framecount)
   {
      if(TRUE == OEMCAniCtl_SetPlayStyleOption(po))
      {
         return TRUE;
      }
   }

   OEMCAniCtl_Display(po);
   pMe->frame_indx++;

   if(pMe->aniRate != 0)
   {
      ISHELL_SetTimer(pMe->m_pShell,
                      pMe->aniRate,
                      (PFNNOTIFY)OEMCAniCtl_Display,
                      pMe);
   }

   return TRUE;
}

static int OEMCAniCtl_StopAni(IAniCtl *po)
{
   CAniCtl *pMe = (CAniCtl *)po;
   void    *pOldContext;
   DBGPRINTF("OEMCAniCtl_StopAni");

   pOldContext = AEE_EnterAppContext(NULL);
   ISHELL_CancelTimer(pMe->m_pShell,
                      (PFNNOTIFY)OEMCAniCtl_StartDisplay,
                      pMe);
   AEE_LeaveAppContext(pOldContext);
   return TRUE;
}


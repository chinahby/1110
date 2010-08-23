#ifndef _DRAW_ANI_H_
#define _DRAW_ANI_H_
/*******************************************************************
 *	Copyright(c)2009-2010 TCL CDMA SW developement Team Reserved
 *
 ********************************************************************/
/********************************************************************
 *
 * Team	:	CDMA JRD SW
 * $Author:	Qiong.chen Create$
 * $Rev: 26 $
 * $Date: 2009-03-27 17:30:54 +0800 (星期五, 27 三月 2009) $
 *	Qiong.chen Create 2009/03/13
 *
 *********************************************************************/
#include "AEEImage.h"
#include "AEE.h"
#include "AEEImageInfo.h"

#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif

#include "AEEShell.h"
#include "AEEAppGen.h"
#include "AEEStdLib.h"
#include "AEEHeap.h"
#include "AEEMenu.h"
#include "AEETime.h"
#include "AEEModTable.h"
#include "AppComFunc.h"
#include "OEMClassIDs.h"
#include "Appscommon.h"
#include "AEE_OEM.h"

typedef struct IAniCtl  IAniCtl;


//-----------------------------------------------------------------------
//    Types and Macros
//-----------------------------------------------------------------------

typedef enum
{
   DRAW_EVENT_START = 0,
   DRAW_EVENT_NEXT,
   DRAW_EVENT_PRV,
   DRAW_EVENT_STOP,
   DRAW_EVENT_DONE,
   DRAW_EVENT_ERROR
}draw_event_type;

typedef enum
{
   DRAW_ERROR_NONE = 0 ,
   DRAW_ERROR_SIZE_NONE,
   DRAW_ERROR_RATE_TIME_NONE
}draw_error_type;

typedef enum
{
   DRAW_PLAY_ONCE,
   DRAW_PLAY_CIRCULATION,
   DRAW_PLAY_CUSTOM
}draw_play_times_style;

typedef struct _draw_info_data_type
{
   AEEImageInfo ImgData;
}draw_info_data_type;

typedef struct _rect
{
   int32 posx;
   int32 posy;
}rect;

typedef void (*DRAW_MSG_CB_TYPE)(void *pApp, draw_event_type event);

QINTERFACE(IAniCtl)
{
   INHERIT_IBase(IAniCtl);

   int     (*RegisterCallBack)(IAniCtl * pAniCtl,DRAW_MSG_CB_TYPE mcallback,void *pUser);
   void    (*DrawCallBack)(void * pAniCtl,draw_event_type event);
   boolean (*SetPosParm)(IAniCtl * pAniCtl,int32 px,int32 py);
   boolean (*SetAniRate)(IAniCtl * pAniCtl,int32 AniTime);
   boolean (*SetImageSize)(IAniCtl * pAniCtl,int32 wx,int32 hy);
   boolean (*SetFrameCount)(IAniCtl *pAniCtl,uint16 num);
   boolean (*SetPlayTimes)(IAniCtl * pAniCtl,draw_play_times_style style,int16 num);
   int     (*StartDisplay)(IAniCtl * pAniCtl,IImage *pImg);
   int     (*Display)(IAniCtl * pAniCtl);
   int     (*StartAni)(IAniCtl * pAniCtl);
   int     (*StopAni)(IAniCtl * pAniCtl);
};

#define IAniCtl_AddRef(p)                  AEEGETPVTBL(p,IAniCtl)->AddRef(p)
#define IAniCtl_Release(p)                 AEEGETPVTBL(p,IAniCtl)->Release(p)
#define IAniCtl_RegisterCallBack(p,cb,pu)  AEEGETPVTBL(p,IAniCtl)->RegisterCallBack(p, cb,pu)
#define IAniCtl_DrawCallBack(p,e,d)        AEEGETPVTBL(p,IAniCtl)->DrawCallBack(p, e)
#define IAniCtl_SetPosParm(p,x,y)          AEEGETPVTBL(p,IAniCtl)->SetPosParm(p, x, y)
#define IAniCtl_SetAniRate(p,t)            AEEGETPVTBL(p,IAniCtl)->SetAniRate(p, t)
#define IAniCtl_SetImageSize(p,w,h)        AEEGETPVTBL(p,IAniCtl)->SetImageSize(p, w, h)
#define IAniCtl_SetFrameCount(p,n)         AEEGETPVTBL(p,IAniCtl)->SetFrameCount(p, n)
#define IAniCtl_SetPlayTimes(p,s,n)        AEEGETPVTBL(p,IAniCtl)->SetPlayTimes(p,s,n)
#define IAniCtl_StartDisplay(p,i)          AEEGETPVTBL(p,IAniCtl)->StartDisplay(p,i)
#define IAniCtl_Display(p)                 AEEGETPVTBL(p,IAniCtl)->Display(p)
#define IAniCtl_StartAni(p)                AEEGETPVTBL(p,IAniCtl)->StartAni(p)
#define IAniCtl_StopAni(p)                 AEEGETPVTBL(p,IAniCtl)->StopAni(p)

#endif

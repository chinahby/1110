/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEEAClock.c

SERVICES:  AEE Analog Clock Control Interface

GENERAL DESCRIPTION:  Provides support for IAClockCtl services
in AEE.

PUBLIC CLASSES AND STATIC FUNCTIONS: IAClockCtl

Copyright © 1999-2002 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/

/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "math.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "AEEGraphics.h"
#include "AEEAClock.h"
#include "AEEAClock_priv.h"
#include "AEEDisp_priv.h"

/*===========================================================================

DEFINITIONS

===========================================================================*/

#define MS_TO_SEC      (1000l)
#define MS_TO_MIN      (1000l * 60l)

//
// Default RGB values
//

#define OUTLINE_R       (0)
#define OUTLINE_G       (0)
#define OUTLINE_B       (0)

#define SEC_R           (0)
#define SEC_G           (0)
#define SEC_B           (0)

#define MIN_R           (0)
#define MIN_G           (255)
#define MIN_B           (255)

#define HOUR_R          (0)
#define HOUR_G          (0)
#define HOUR_B          (255)

#define SEC_HAND_WIDTH  (2)
#define FACE_WIDTH      (2)
#define WIDTH           (100)

#define PI              (3.14)

#define GET_RVAL(c)     (byte)((c >> 8) & 0x0ff)
#define GET_GVAL(c)     (byte)((c >> 16) & 0x0ff)
#define GET_BVAL(c)     (byte)((c >> 24) & 0x0ff)

/*===========================================================================

Class Definitions

===========================================================================*/

OBJECT(AClock)
{
   DECLARE_VTBL(IAClockCtl)

   uint32         m_nRefs;
   IShell *       m_pShell;
   IGraphics   *  m_pg;
   IImage *       m_pFace;
   uint32         m_dwProps;
   AEERect        m_rcFaceImage;
   AEERect        m_rcFace;
   uint32         m_dwTime;

   byte           m_cFace[3];
   byte           m_cHandFrame[3];
   byte           m_cSec[3];
   byte           m_cMin[3];
   byte           m_cHour[3];
};

//---------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------

static uint32  AClock_AddRef(IAClockCtl * po);
static uint32  AClock_Release(IAClockCtl * po);
static boolean AClock_HandleEvent(IAClockCtl * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean AClock_Redraw(IAClockCtl * po);
static void    AClock_SetActive(IAClockCtl * po, boolean bActive);
static boolean AClock_IsActive(IAClockCtl * po);
static void    AClock_SetRect(IAClockCtl * po, const AEERect * prc);
static void    AClock_GetRect(IAClockCtl * po, AEERect * prc);
static void    AClock_SetProperties(IAClockCtl * po, uint32 dwProp);
static uint32  AClock_GetProperties(IAClockCtl * po);
static void    AClock_Reset(IAClockCtl * po);
static void    AClock_Start(IAClockCtl * pc);
static void    AClock_Stop(IAClockCtl * pc);
static uint32  AClock_SetTime(IAClockCtl * pme, uint32 dwTime);
static void    AClock_SetFaceColor(IAClockCtl * pme, RGBVAL rgb);
static void    AClock_SetHandFrameColor(IAClockCtl * pme, RGBVAL rgb);
static void    AClock_SetSecColor(IAClockCtl * pme,RGBVAL rgb);
static void    AClock_SetMinColor(IAClockCtl * pme,RGBVAL rgb);
static void    AClock_SetHourColor(IAClockCtl * pme,RGBVAL rgb);
static void    AClock_SetFaceImage(IAClockCtl * po,IImage * pi,AEERect * prc);
static void    AClock_GetFaceImageRect(IAClockCtl * po,AEERect * prc);
static void    AClock_CalcOffset(AClock * pme, int cx, int cy);

static int     AClock_GetX(AClock * pme,int nTicks,double nWert,double nFactor);
static int     AClock_GetY(AClock * pme,int nTicks,double nWert,double nFactor);

//---------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------

static const VTBL(IAClockCtl) gAClockFuncs = {         
    AClock_AddRef,                                
    AClock_Release,
    AClock_HandleEvent,
    AClock_Redraw,
    AClock_SetActive,
    AClock_IsActive,
    AClock_SetRect,
    AClock_GetRect,
    AClock_SetProperties,
    AClock_GetProperties,
    AClock_Reset,
    AClock_Start,
    AClock_Stop,
    AClock_SetTime,
    AClock_SetFaceColor,
    AClock_SetHandFrameColor,
    AClock_SetSecColor,
    AClock_SetMinColor,
    AClock_SetHourColor,
    AClock_SetFaceImage,
    AClock_GetFaceImageRect
};

/*===========================================================================

EXTERNAL - Called from AEEControls.c.  Supports creation of analog clock class.

===========================================================================*/
int AClockCtl_New(IShell * ps, AEECLSID cls, void ** ppObj)
{
   AClock *       pme;

   *ppObj = NULL;

  // allocate memory for object

   if((pme = (AClock *)AEE_NewClass((IBaseVtbl *)&gAClockFuncs, sizeof(AClock))) == NULL)
      return(ENOMEMORY);

   *ppObj = pme;

   pme->m_pShell = ps;
   ISHELL_CreateInstance(ps,AEECLSID_GRAPHICS,(void **)(&pme->m_pg));

   if(IGRAPHICS_GetColorDepth(pme->m_pg) >= 8){
      pme->m_cFace[0] = 128;
      pme->m_cFace[1] = 128;
      pme->m_cFace[2] = 128;

      pme->m_cHandFrame[0] = OUTLINE_R;
      pme->m_cHandFrame[1] = OUTLINE_G;
      pme->m_cHandFrame[2] = OUTLINE_B;

      pme->m_cSec[0] = SEC_R;
      pme->m_cSec[1] = SEC_G;
      pme->m_cSec[2] = SEC_B;

      pme->m_cMin[0] = MIN_R;
      pme->m_cMin[1] = MIN_G;
      pme->m_cMin[2] = MIN_B;

      pme->m_cHour[0] = HOUR_R;
      pme->m_cHour[1] = HOUR_G;
      pme->m_cHour[2] = HOUR_B;
   }
   else{

   // Mono - Black face, white hands...

      pme->m_cHandFrame[0] = 255;
      pme->m_cHandFrame[1] = 255;
      pme->m_cHandFrame[2] = 255;

      MEMCPY(pme->m_cSec,pme->m_cHandFrame,sizeof(pme->m_cSec));
   }
   pme->m_dwProps = (ACPROP_MINS|ACPROP_HOURS);
   return(0);
}

/*=====================================================================

Public Method - Decrements the reference count.  When it reaches 0, the object
is freed.

======================================================================*/
static uint32 AClock_Release(IAClockCtl * po)
{
   AClock * pme = (AClock *)po;

   if(pme->m_nRefs){
      if(--pme->m_nRefs)
         return(pme->m_nRefs);
      ISHELL_CancelTimer(pme->m_pShell,NULL,pme);
      aee_releaseobj((void **)(&pme->m_pg));
      aee_releaseobj((void **)(&pme->m_pFace));
      FREE(pme);
   }
   return(0);
}

/*=====================================================================

Public Method - Increments the reference count on the object.

======================================================================*/
static uint32 AClock_AddRef(IAClockCtl * po)
{
   AClock * pme = (AClock *)po;

   return(++pme->m_nRefs);
}

/*===========================================================================

Public Method - Sets the time for the clock.  See AEEAClock.h

===========================================================================*/
static uint32 AClock_SetTime(IAClockCtl * po, uint32 dwTime)
{  
   AClock * pme = (AClock *)po;
   uint32   dwLast = pme->m_dwTime;

   ISHELL_CancelTimer(pme->m_pShell,(PFNNOTIFY)(AClock_Start),pme);
   pme->m_dwTime = dwTime;
   AClock_Redraw(po);

   return(dwLast);
}

/*===========================================================================

Public Method - Starts the auto-update feature. See AEEAClock.h

===========================================================================*/
static void AClock_Start(IAClockCtl * po)
{
   AClock * pme = (AClock *)po;
   uint32   n,nMS,nNow;

   pme->m_dwTime = GETTIMESECONDS();
   AClock_Redraw(po);

   nNow = GETTIMEMS();
   if(pme->m_dwProps & ACPROP_SECS){
      n = (nNow / MS_TO_SEC);
      nMS = MS_TO_SEC - (nNow - (n * MS_TO_SEC));
   }
   else{
      n = (nNow / MS_TO_MIN);
      nMS = MS_TO_MIN - ((n * MS_TO_MIN) - nNow);
   }
   ISHELL_SetTimer(pme->m_pShell,nMS,(PFNNOTIFY)(AClock_Start),pme);
}

/*===========================================================================

Public Method - Stops the auto-update feature. See AEEAClock.h

===========================================================================*/
static void AClock_Stop(IAClockCtl * po)
{
   AClock * pme = (AClock *)po;

   ISHELL_CancelTimer(pme->m_pShell,(PFNNOTIFY)(AClock_Start),pme);
}

/*===========================================================================

Public Method - Returns the rectangle of the clock. See AEEAClock.h

===========================================================================*/
static void AClock_GetRect(IAClockCtl * po, AEERect * prc)
{
   AClock * pme = (AClock *)po;

   *prc = pme->m_rcFace;
}

/*===========================================================================

Public Method - Sets the rectangle for the clock. See AEEAClock.h

===========================================================================*/
static void AClock_SetRect(IAClockCtl * po, const AEERect * prc)
{
   AClock * pme = (AClock *)po;

   pme->m_rcFace.x = prc->x;
   pme->m_rcFace.y = prc->y;
   AClock_CalcOffset(pme,prc->dx, prc->dy);
}

/*===========================================================================

Public Method - Ignored. See AEEAClock.h

===========================================================================*/
static void AClock_SetActive(IAClockCtl * po, boolean bActive)
{
}

/*===========================================================================

Public Method - Ignored. See AEEAClock.h

===========================================================================*/
static boolean AClock_IsActive(IAClockCtl * po)
{
   return(FALSE);
}

/*===========================================================================

Public Method - Ignored. See AEEAClock.h

===========================================================================*/
static boolean AClock_HandleEvent(IAClockCtl * po, AEEEvent evt, uint16 wp, uint32 dwp)
{
   return(FALSE);
}

/*===========================================================================

Public Method - Resets the clock control. See AEEAClock.h

===========================================================================*/
static void AClock_Reset(IAClockCtl * po)
{
   AClock * pme   = (AClock *)po;

   aee_releaseobj((void **)(&pme->m_pFace));
   ISHELL_CancelTimer(pme->m_pShell,NULL,pme);
}

/*===========================================================================

Public Method - Redraws the clock. See AEEAClock.h

===========================================================================*/
static boolean AClock_Redraw(IAClockCtl * po)
{
   AClock *    pme = (AClock *)po;
   AEECircle   c;
   JulianType  j;
   double      second,minute,hour;
   IGraphics * pg;
   AEEPolygon  g;
   int         nDiameter,nRadius;
   AEEPoint    pt[4];
   AEELine     l;


   pg          = pme->m_pg;
   nDiameter   = pme->m_rcFace.dx;

   if(!nDiameter)
      return(FALSE);

   nRadius = nDiameter / 2;

   c.r   = nRadius;
   c.cx  = pme->m_rcFace.x + nRadius;
   c.cy  = pme->m_rcFace.y + nRadius;

   IGRAPHICS_SetFillMode(pg,TRUE);

   if(!pme->m_pFace || !AEEIntersectRect(NULL, &pme->m_rcFace, &pme->m_rcFaceImage)){
      IGRAPHICS_SetPointSize(pg,FACE_WIDTH);
      IGRAPHICS_SetColor(pg,pme->m_cFace[0],pme->m_cFace[1],pme->m_cFace[2],0);
      IGRAPHICS_DrawCircle(pg, &c);
   }
   else
      IGRAPHICS_ClearRect(pg,&pme->m_rcFace);

   if(pme->m_pFace)
      IIMAGE_Draw(pme->m_pFace,pme->m_rcFaceImage.x,pme->m_rcFaceImage.y);

   // Tell them to redraw whatever they need to...

   if(pme->m_dwProps & ACPROP_NOTIFY){
      CtlValChange   cc;
      MEMSET(&cc,0,sizeof(cc));
      cc.pc = (IControl *)pme;
      ISHELL_HandleEvent(pme->m_pShell, EVT_CTL_CHANGING, 0, (uint32)(&cc));
   }

   ISHELL_GetJulianDate(ps,pme->m_dwTime,&j);

   second = (double)j.wSecond;
   minute = (double)j.wMinute + (second / 60);
   hour = (double)j.wHour + (minute / 60);

   if(pme->m_dwProps & ACPROP_SECS){
      IGRAPHICS_SetColor(pg,pme->m_cSec[0],pme->m_cSec[1],pme->m_cSec[2],0);
      IGRAPHICS_SetPointSize(pg,SEC_HAND_WIDTH);

      l.sx = pme->m_rcFace.x + nRadius;
      l.sy = pme->m_rcFace.y + nRadius;
      l.ex = AClock_GetX(pme,60,second,2.1);
      l.ey = AClock_GetY(pme,60,second,2.1);
      IGRAPHICS_DrawLine(pg,&l);
   }

   g.len = 4;
   g.points = pt;

   MEMSET(pt,0,sizeof(pt));

   IGRAPHICS_SetColor(pg,pme->m_cHandFrame[0],pme->m_cHandFrame[1],pme->m_cHandFrame[2],0);

   // Draw minute hand - enclosed...

   if(pme->m_dwProps & ACPROP_MINS){
      IGRAPHICS_SetFillColor(pg,pme->m_cMin[0],pme->m_cMin[1],pme->m_cMin[2],0);
      pt[0].x = AClock_GetX(pme,60,minute+30,12);  // Center x
      pt[0].y = AClock_GetY(pme,60,minute+30,12);  // Center y
      pt[1].x = AClock_GetX(pme,60,minute-15,20);
      pt[1].y = AClock_GetY(pme,60,minute-15,20);
      pt[2].x = AClock_GetX(pme,60,minute,2.3);    // End point x
      pt[2].y = AClock_GetY(pme,60,minute,2.3);    // End point y
      pt[3].x = AClock_GetX(pme,60,minute+15,20);
      pt[3].y = AClock_GetY(pme,60,minute+15,20);
      IGRAPHICS_DrawPolygon(pg,&g);
   }

   // Draw hour hand - enclosed.

   if(pme->m_dwProps & ACPROP_HOURS){
      IGRAPHICS_SetFillColor(pg,pme->m_cHour[0],pme->m_cHour[1],pme->m_cHour[2],0);
      pt[0].x = AClock_GetX(pme,12,hour+6,10);
      pt[0].y = AClock_GetY(pme,12,hour+6,10);
      pt[1].x = AClock_GetX(pme,12,hour-3,18);
      pt[1].y = AClock_GetY(pme,12,hour-3,18);
      pt[2].x = AClock_GetX(pme,12,hour,3);
      pt[2].y = AClock_GetY(pme,12,hour,3);
      pt[3].x = AClock_GetX(pme,12,hour+3,18);
      pt[3].y = AClock_GetY(pme,12,hour+3,18);
      IGRAPHICS_DrawPolygon(pg,&g);
   }
   IGRAPHICS_Update(pg);

   return(TRUE);
}

/*===========================================================================

Public Method - Sets the color of the face background.

===========================================================================*/
static void AClock_SetFaceColor(IAClockCtl * po, RGBVAL rgb)
{
   AClock * pme = (AClock *)po;

   pme->m_cFace[0] = GET_RVAL(rgb);
   pme->m_cFace[1] = GET_GVAL(rgb);
   pme->m_cFace[2] = GET_BVAL(rgb);
}

/*===========================================================================

Public Method - Sets the color of the frame around the hands. See AEEAClock.h

===========================================================================*/
static void AClock_SetHandFrameColor(IAClockCtl * po, RGBVAL rgb)
{
   AClock * pme = (AClock *)po;

   pme->m_cHandFrame[0] = GET_RVAL(rgb);
   pme->m_cHandFrame[1] = GET_GVAL(rgb);
   pme->m_cHandFrame[2] = GET_BVAL(rgb);
}

/*===========================================================================

Public Method - Sets the color of the second hand. See AEEAClock.h

===========================================================================*/
static void AClock_SetSecColor(IAClockCtl * po,RGBVAL rgb)
{
   AClock * pme = (AClock *)po;

   pme->m_cSec[0] = GET_RVAL(rgb);
   pme->m_cSec[1] = GET_GVAL(rgb);
   pme->m_cSec[2] = GET_BVAL(rgb);
}

/*===========================================================================

Public Method - Sets the color of the minute hand. See AEEAClock.h

===========================================================================*/
static void AClock_SetMinColor(IAClockCtl * po,RGBVAL rgb)
{
   AClock * pme = (AClock *)po;

   pme->m_cMin[0] = GET_RVAL(rgb);
   pme->m_cMin[1] = GET_GVAL(rgb);
   pme->m_cMin[2] = GET_BVAL(rgb);
}

/*===========================================================================

Public Method - Sets the color of the hour hand. See AEEAClock.h

===========================================================================*/
static void AClock_SetHourColor(IAClockCtl * po,RGBVAL rgb)
{
   AClock * pme = (AClock *)po;

   pme->m_cHour[0] = GET_RVAL(rgb);
   pme->m_cHour[1] = GET_GVAL(rgb);
   pme->m_cHour[2] = GET_BVAL(rgb);
}

/*===========================================================================

Public Method - Sets the background image for the face. See AEEAClock.h

===========================================================================*/
static void AClock_SetFaceImage(IAClockCtl * po,IImage * pi,AEERect * prc)
{
   AClock *       pme = (AClock *)po;
   AEEImageInfo   ii;

   aee_releaseobj((void **)(&pme->m_pFace));
   if(pi)
      IIMAGE_AddRef(pi);
   pme->m_pFace = pi;

   if(pi){
      if(prc)
         pme->m_rcFaceImage = *prc;
      else
         pme->m_rcFaceImage = pme->m_rcFace;

      IIMAGE_GetInfo(pi,&ii);
      pme->m_rcFaceImage.dx = ii.cxFrame;
      pme->m_rcFaceImage.dy = ii.cy;
      if(!pme->m_rcFace.dx || !pme->m_rcFace.dy)
         AClock_CalcOffset(pme, ii.cxFrame, ii.cy);
   }
}

/*===========================================================================

Public Method - Returns the rectangle of the face image. See AEEAClock.h

===========================================================================*/
static void AClock_GetFaceImageRect(IAClockCtl * po,AEERect * prc)
{
   AClock * pme = (AClock *)po;

   MEMCPY(prc,&pme->m_rcFaceImage,sizeof(AEERect));
}

/*===========================================================================

Public Method - Sets flags/properties for the analog clock. See AEEAClock.h

===========================================================================*/
static void AClock_SetProperties(IAClockCtl * po, uint32 dwProps)
{
   AClock * pme = (AClock *)po;

   pme->m_dwProps = dwProps;
}

/*===========================================================================

Public Method - Returns the flags/properties for the analog clock. See AEEAClock.h

===========================================================================*/
static uint32 AClock_GetProperties(IAClockCtl * po)
{
   AClock * pme = (AClock *)po;
   
   return(pme->m_dwProps);
}

/*===========================================================================




Local Methods



===========================================================================*/

/*===========================================================================

Local Method - Computes the x value given the ticks, wert and factor

===========================================================================*/
static int AClock_GetX(AClock * pme,int nTicks,double nWert,double nFactor)
{
   int   nDiameter = pme->m_rcFace.dx;

   return((int)pme->m_rcFace.x + (nDiameter/2 - (int)(sin(PI + PI / ((double)nTicks/2) * nWert) * (double)nDiameter/nFactor)));
}

/*===========================================================================

Local Method - Computes the y value given the ticks, wert and factor

===========================================================================*/
static int AClock_GetY(AClock * pme,int nTicks,double nWert,double nFactor)
{
   int   nDiameter = pme->m_rcFace.dx;

   return(pme->m_rcFace.y + (int)(cos(PI + PI / ((double)nTicks/2)*nWert)*(double)nDiameter/nFactor)+nDiameter/2);
}

/*===========================================================================

Local Method - Computes the offset of the clock given cx/cy.

===========================================================================*/
static void AClock_CalcOffset(AClock * pme, int cx, int cy)
{
   int   nDiameter;

   if(cx > cy)
      nDiameter = cy;
   else
      nDiameter = cx;

   pme->m_rcFace.dx = pme->m_rcFace.dy = nDiameter;
}




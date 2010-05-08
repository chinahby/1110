/*======================================================
FILE: CaretWidget.c

SERVICES: 
   IWidget implementation for text caret

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEStdLib.h"
#include "AEEWidget.h"
#include "AEEWProperties.h"
#include "WidgetBase.h"
#include "CaretWidget.h"
#include "wutil.h"
#include "AEEDisplayCanvas.h"
#include "AEEContainer.h"


#define ME_FROM_WIDGET     CaretWidget *me = (CaretWidget*)po
#define CARETEND           0x50      // small 'serif'  (I-beam)
#define CARETBAR           0x20
#define CARETWIDTH         5
#define CARETHEIGHT        12


static RGBVAL ColorBlend(RGBVAL rgb1, RGBVAL rgb2, int nPercent)
{
   RGBVAL rgb = 0;
   int i, nReduce;
   
   nReduce = MAX(0, 100 - nPercent);
   if (!nReduce)
      return rgb1;

   for (i=0; i < 3; i++) {
      int c1, c2, cmin, cmax;

      c1 = 0xff & (rgb1 >> (24 - (8 * i)));
      c2 = 0xff & (rgb2 >> (24 - (8 * i))); 

      cmin = MIN(c1, c2);
      cmax = MAX(c1, c2);

      rgb |= ((cmax * 100) - ((cmax-cmin) * nPercent)) / 100;
      rgb <<= 8;
   }

   return rgb;
}

static void CaretWidget_Pulse(IWidget *po)
{
   ME_FROM_WIDGET;
   uint32 dwAnimFlags = 0;
   
   me->msTime = me->msFade;
   
   // this loop controls the caret fade.  It iterates from 100 down to min and then
   // back up. nDir = 1 means going down and -1 means going up.
   if (me->nBlend >= 100 && me->nDir == -1) { // wrap back down
      me->nBlend = 100 - me->nInc;
      me->nDir = 1;
      me->msTime = me->msTimeOn;
      me->bLastFrame = FALSE;
   } else if (me->nBlend <= me->nMin && me->nDir == 1) { // wrap back up
      me->nBlend  = me->nMin;
      me->nDir = -1;
   } else { // just increment
      me->nBlend -= (me->nInc * me->nDir);
      if (me->nBlend - (me->nInc * me->nDir) >= 100 && !me->bLastFrame) {      // last frame
         dwAnimFlags |= (EVT_STEP_FINAL | EVT_STEP_REPEAT);
         me->bLastFrame = TRUE;
      }
   }

   WidgetBase_Invalidate((IWidget *)me, ICIF_REDRAW);
   if (me->bStepEnabled) {
      IMODEL_StepNotify(WBASE(me)->piViewModel, dwAnimFlags, me->nPulseCount++, me->nPulseRep);
   }

   // if it's the last one, reset out internal counters
   if (dwAnimFlags & EVT_STEP_FINAL) {
      ++me->nPulseRep;
      me->nPulseCount = 0;
   }
   
   // we're pulsing still
   if (me->msTime > 0) {
      ISHELL_SetTimerEx(me->piShell, me->msTime, &me->cbk); // start pulse
   }  
}

boolean CaretWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   switch(evt) {
   
      case EVT_WDG_SETPROPERTY:

         if (wParam == PROP_FGCOLOR) {
            me->rgbFG = (RGBVAL)dwParam;
            return TRUE;
         } else if (wParam == PROP_BGCOLOR) {
            me->rgbBG = (RGBVAL)dwParam;
            return TRUE;
         } else if (wParam == PROP_CARETPULSE) {
            if (dwParam != 0) {
               CaretPulse *pcp = (CaretPulse*)dwParam;
               me->nMin = pcp->nMin;
	            me->msTimeOn = pcp->msTimeOn;
	            me->msFade = (pcp->nFadeSteps > 0) ? pcp->msFadeTime / pcp->nFadeSteps : 0;
	            me->nInc = (pcp->nFadeSteps > 0) ? (100 - pcp->nMin) / pcp->nFadeSteps : 0;
               me->msTime = me->msTimeOn;

               // reset out internal pulse data trackers
               me->nPulseCount = me->nPulseRep = 0;
               me->nBlend = 100;
               me->bLastFrame = FALSE;
               me->nDir = 1;
               if (me->msTime) {
                  WidgetBase_Invalidate((IWidget*)me, ICIF_REDRAW);
                  if (me->bStepEnabled) {
                     IMODEL_StepNotify(WBASE(me)->piViewModel, EVT_STEP_PRE, me->nPulseCount++, me->nPulseRep);
                  }
                 
                  ISHELL_SetTimerEx(me->piShell, me->msTime, &me->cbk);
               }
               else {
                  CALLBACK_Cancel(&me->cbk);
               }
            }
            return TRUE;
         } else if (wParam == PROP_ANIMATE_FLAGS) {
            me->bStepEnabled = dwParam & AF_ENABLE_EVT_STEP;
            return TRUE;
         }
   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}

void CaretWidget_CreateCaret(IWidget *po, IDisplay *piDisplay)
{
   ME_FROM_WIDGET;
   uint16 cyHeight = me->base.extent.height;

   if (SUCCESS == CreateMonoDIB(piDisplay, &me->piDibCaret, CARETWIDTH, cyHeight, TRUE)) {
      int i;
      uint8 *pBmp = me->piDibCaret->pBmp;
      // set the bits in the bitmap data array
      pBmp[0] = CARETEND;                 // top of i-beam
      for (i=1; i < cyHeight-1; i++) {    
         pBmp[i] = CARETBAR;              // vertical bar (sized to char height)
      }
      pBmp[cyHeight-1] = CARETEND;        // bottom of i-beam
   }
}

void CaretWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;
   IDisplay *piDisplay;
   AEEBitmapInfo bi;
   RGBVAL rgb;

   if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      if (!me->piDibCaret) {
         CaretWidget_CreateCaret(po, piDisplay);
      }

      if (me->piDibCaret) {
         rgb = ColorBlend(me->rgbFG, me->rgbBG, me->nBlend);
         InitMonoDIBPalette(me->piDibCaret, rgb);
         IBITMAP_GetInfo(IDIB_TO_IBITMAP(me->piDibCaret), &bi, sizeof(bi));
         IDISPLAY_BitBlt(piDisplay, x, y, bi.cx, bi.cy, me->piDibCaret, 0, 0, AEE_RO_TRANSPARENT);
      }

      IDISPLAY_Release(piDisplay);
   }
}

void CaretWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   pweOut->width = CARETWIDTH;
   pweOut->height = CARETHEIGHT;
}

void CaretWidget_SetExtent(IWidget *po, WExtent *pWExtent)
{
   ME_FROM_WIDGET;

   WidgetBase_SetExtent(po, pWExtent);

   // Release the bmp so it will be re-created with the appropriate height
   RELEASEIF(me->piDibCaret);
}

boolean CaretWidget_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
   ME_FROM_WIDGET;
   AEERect rc;
   SETAEERECT(&rc, 0, 0, me->base.extent.width, me->base.extent.height);
   return IntersectRect(prcOut, prcIn, &rc);
}

uint32 CaretWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;
   
   uint32 nRefs = --me->base.nRefs;

   if (nRefs == 0) {
      CaretWidget_Dtor(me);
      FREE(me);
   }
   return nRefs;
}

uint32 CaretWidget_AddRef(IWidget *po)
{
   ME_FROM_WIDGET;
   return ++me->base.nRefs;
}

void CaretWidget_Dtor(CaretWidget *me)
{
   CALLBACK_Cancel(&me->cbk); // stop pulse

   RELEASEIF(me->piShell);
   RELEASEIF(me->piDibCaret);

   WidgetBase_Dtor(&me->base);
}

void CaretWidget_Ctor(CaretWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(CaretWidget_HandleEvent));

   pvt->AddRef                = CaretWidget_AddRef;
   pvt->Release               = CaretWidget_Release;
   pvt->Draw                  = CaretWidget_Draw;
   pvt->GetPreferredExtent    = CaretWidget_GetPreferredExtent;
   pvt->SetExtent             = CaretWidget_SetExtent;

   me->rgbFG = RGB_BLACK;
   me->rgbBG = RGB_WHITE;
   me->nBlend = 100;

   me->piShell = piShell;
   ISHELL_AddRef(me->piShell);

   CALLBACK_Init(&me->cbk, CaretWidget_Pulse, CAST(IWidget*,me));
}


int CaretWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   CaretWidget *me = MALLOCREC_VTBL(CaretWidget, IWidget);
   if (!me) {
      return ENOMEMORY;
   }
   CaretWidget_Ctor(me, GETVTBL(me,IWidget), piShell, piModule, 0);
   *ppo = (IWidget*) me;
   return SUCCESS;
}


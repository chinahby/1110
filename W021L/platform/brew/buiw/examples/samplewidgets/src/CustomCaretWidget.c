/*======================================================
FILE: CustomCaretWidget.c

SERVICES: 
   CustomCaretWidget is a sample custom implementation of a 
   Caret Widget

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEStdLib.h"
#include "AEEWidget.h"
#include "AEEWProperties.h"
#include "AEECaretWidget.h"
#include "AEEDisplayCanvas.h"
#include "AEEContainer.h"
#include "CustomWidgetBase.h"
#include "samplewidgets_util.h"

////////////////////////////////////////////////////////////////
// CustomCaretWidget structure

typedef struct _CustomCaretWidget {
   WidgetBase		base;
   IDIB *			piDibCaret;
   RGBVAL         rgbFG;
   RGBVAL         rgbBG;
   IShell *       piShell;
   IWidget *      piw;

   int            nDir;
	int            nBlend;
	int            nInc;       // current increment
	int            nMin;       // minimum blend amount (max is always 100)
	int            msTimeOn;   // time in ms that caret is on full
	int            msFade;     // pulse interval
	int            msTime;
	AEECallback    cbk;
	IContainer *   piContainer;
	AEERect        rcCaret;
} CustomCaretWidget;

////////////////////////////////////////////////////////////////
// Function prototypes

static void     CustomCaretWidget_Ctor                 (CustomCaretWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler);
static void     CustomCaretWidget_Dtor                 (CustomCaretWidget *me);
static boolean  CustomCaretWidget_HandleEvent          (IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
static uint32   CustomCaretWidget_Release              (IWidget *po);
static void     CustomCaretWidget_CreateCaret          (IWidget *po, IDisplay *piDisplay);
static void     CustomCaretWidget_Draw                 (IWidget *po, ICanvas *piCanvas, int x, int y);
static void     CustomCaretWidget_GetPreferredExtent   (IWidget *po, WExtent *pweOut);
static void     CustomCaretWidget_SetExtent            (IWidget *po, WExtent *pWExtent);


#define ME_FROM_WIDGET     CustomCaretWidget *me = (CustomCaretWidget*)po
#define CARETWIDTH         5
#define CARETHEIGHT        12


////////////////////////////////////////////////////////////////
// CustomCaretWidget Implentation

//--------------------------------------------------------------
// Helper Methods
//--------------------------------------------------------------

/*--------------------------------------------------------------
   ColorBlend - returns a RGBVAL representation obtained by
   blending two RGBVALs
   --- */

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



/* -----------------------------------------------------------------------
    CustomCaretWidget_Pulse - adjust color for pulsing of widget 
   --- */
static void CustomCaretWidget_Pulse(IWidget *po)
{
   ME_FROM_WIDGET;

   me->msTime = me->msFade;

   me->nBlend -= (me->nInc * me->nDir);

   if (me->nBlend >= 100) {
      me->nBlend  = 100;
      me->nDir = 1;
      me->msTime = me->msTimeOn;
   }

   if (me->nBlend <= me->nMin) {
      me->nBlend  = me->nMin;
      me->nDir = -1;
   }
   
   WidgetBase_InvalidateExtent((IWidget *)me);
}


/* -----------------------------------------------------------------------
    CustomCaretWidget_HandleEvent - handle events ...
   --- */
static boolean CustomCaretWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
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
               me->nDir = 1;
	            me->nMin = pcp->nMin;
	            me->msTimeOn = pcp->msTimeOn;
	            me->msFade = (pcp->nFadeSteps > 0) ? pcp->msFadeTime / pcp->nFadeSteps : 0;
	            me->nInc = (pcp->nFadeSteps > 0) ? (100 - pcp->nMin) / pcp->nFadeSteps : 0;
               me->msTime = me->msTimeOn;
            }
            return TRUE;
         }
   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}


/* -----------------------------------------------------------------------
    CustomCaretWidget_CreateCaret - creates the bitmap to use for the 
    caret.  As implemented here it will create a bottom justified 
    horizontal line.
   --- */

static void CustomCaretWidget_CreateCaret(IWidget *po, IDisplay *piDisplay)
{
   ME_FROM_WIDGET;
   uint16 cyHeight = me->base.extent.height;

   if (SUCCESS == CreateMonoDIB(piDisplay, &me->piDibCaret, CARETWIDTH, cyHeight, TRUE)) {
      int i;
      uint8 *pBmp = me->piDibCaret->pBmp;
      // set the bits in the bitmap data array
      for (i=0; i < cyHeight-1; i++) {    
         pBmp[i] = 0x00;              // everything but bottom line is blank
      }
      pBmp[cyHeight-1] = 0x7E;        // horizontal line on the bottom
   }
}


/* -----------------------------------------------------------------------
    CustomCaretWidget_Draw - draw the caret
   --- */
static void CustomCaretWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;
   IDisplay *piDisplay;
   AEEBitmapInfo bi;
   RGBVAL rgb;

   if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      if (!me->piDibCaret) {
         CustomCaretWidget_CreateCaret(po, piDisplay);
      }

      if (me->piDibCaret) {
         rgb = ColorBlend(me->rgbFG, me->rgbBG, me->nBlend);
         InitMonoDIBPalette(me->piDibCaret, rgb);
         IBITMAP_GetInfo(IDIB_TO_IBITMAP(me->piDibCaret), &bi, sizeof(bi));
         IDISPLAY_BitBlt(piDisplay, x, y, bi.cx, bi.cy, me->piDibCaret, 0, 0, AEE_RO_TRANSPARENT);

         if (me->msTime > 0)
            CALLBACK_Timer(&me->cbk, CustomCaretWidget_Pulse, me, me->piShell, me->msTime); // start pulse
      }

      IDISPLAY_Release(piDisplay);
   }
}


/* -----------------------------------------------------------------------
    CustomCaretWidget_GetPreferredExtent - returns the preferred extent of
    the caret
   --- */
static void CustomCaretWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   pweOut->width = CARETWIDTH;
   pweOut->height = CARETHEIGHT;
}

/* -----------------------------------------------------------------------
    CustomCaretWidget_SetExtent - sets the caret's extent
   --- */
static void CustomCaretWidget_SetExtent(IWidget *po, WExtent *pWExtent)
{
   ME_FROM_WIDGET;

   WidgetBase_SetExtent(po, pWExtent);

   // Release the bmp so it will be re-created with the appropriate height
   RELEASEIF(me->piDibCaret);
}


/* -----------------------------------------------------------------------
    CustomCaretWidget_Release - release this widget
   --- */
static uint32 CustomCaretWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;
   
   if (me->base.nRefs == 1) {
      CustomCaretWidget_Dtor(me);
   }
   return WidgetBase_Release(po);
}

/* -----------------------------------------------------------------------
    CustomCaretWidget_Dtor 
   --- */
static void CustomCaretWidget_Dtor(CustomCaretWidget *me)
{
   CALLBACK_Cancel(&me->cbk); // stop pulse

   RELEASEIF(me->piShell);
   RELEASEIF(me->piDibCaret);

   WidgetBase_Dtor(&me->base);
}

/* -----------------------------------------------------------------------
    CustomCaretWidget_Ctor  
   --- */
static void CustomCaretWidget_Ctor(CustomCaretWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(CustomCaretWidget_HandleEvent));

   pvt->Release               = CustomCaretWidget_Release;
   pvt->Draw                  = CustomCaretWidget_Draw;
   pvt->GetPreferredExtent    = CustomCaretWidget_GetPreferredExtent;
   pvt->SetExtent             = CustomCaretWidget_SetExtent;

   me->rgbFG = RGB_BLACK;
   me->rgbBG = RGB_WHITE;
   me->nBlend = 100;

   me->piShell = piShell;
   ISHELL_AddRef(me->piShell);

}

/* -----------------------------------------------------------------------
    SampleWidget_New  
   --- */
int CustomCaretWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   CustomCaretWidget *me = MALLOCREC_VTBL(CustomCaretWidget, IWidget);
   if (!me) {
      return ENOMEMORY;
   }
   CustomCaretWidget_Ctor(me, GETVTBL(me,IWidget), piShell, piModule, 0);
   *ppo = (IWidget*) me;
   return SUCCESS;
}
/*
  ========================================================================

  FILE:  SliderWidget.c
  
  SERVICES: 

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#define READYFORPRIMETIME

#ifdef READYFORPRIMETIME

#include "AEEDisplayCanvas.h"
#include "ModelBase.h"
#include "SliderWidget.h"
#include "wutil.h"


#define DECL(name)   name *me = (name*)po


/////////////////////////////////////////////////////////////////////////////

static __inline uint16 SliderWidget_GetPos(SliderWidget *me) {
   uint16 pos = 0;
   if (me->base.piModel) {
      pos = (uint16)(uint32)IVALUEMODEL_GetValue((IValueModel *)me->base.piModel, NULL);
   }
   return pos;
}
   
static __inline void SliderWidget_SetPos(SliderWidget *me, uint16 nPos) {
   if (me->base.piModel) {
      IVALUEMODEL_SetValue((IValueModel*)me->base.piModel, (void*)nPos, 0, 0);
   }
}


void SliderWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   DECL(SliderWidget);

   if (me->bVert) {
      pweOut->width = me->cyFont + 1;
      pweOut->height  = pweOut->width * 6;
   } else {
      pweOut->height = me->cyFont + 1;
      pweOut->width  = pweOut->height * 6;
   }
}

void SliderWidget_SetExtent(IWidget *po, WExtent *pe)
{
   DECL(SliderWidget);

   WidgetBase_SetExtent((IWidget*)&me->base, pe);
   
   me->bextent = *pe;

   if (me->bVert) {
      me->bextent.width = MAX(5, pe->width-6);
   } else {
      me->bextent.height = MAX(5, pe->height-6);
   }
   Border_CalcClientRect(&me->border);
}


static __inline int CalcHandleOffset(SliderWidget *me, int nPixelRange, int nPos) {
   return ((((nPixelRange * 100) / me->nMaxPos) * nPos) + 50) / 100;
}


static void SliderWidget_DrawHandle(SliderWidget *me, IDisplay *piDisplay, int x, int y)
{
#define HANDLE_CX 5
   AEERect rc;
   int nPixelRange;

   uint16 nPos = SliderWidget_GetPos(me);

   if (me->bVert) {
      SETAEERECT(&rc, x, y, me->base.extent.width, HANDLE_CX);
      nPixelRange = me->base.extent.height - HANDLE_CX;
      rc.y += nPixelRange - CalcHandleOffset(me, nPixelRange, nPos);
   } else {
      SETAEERECT(&rc, x, y, HANDLE_CX, me->base.extent.height);
      nPixelRange = me->base.extent.width - HANDLE_CX;
      rc.x += CalcHandleOffset(me, nPixelRange, nPos);
   }

   // draw 'handle'
   IDISPLAY_DrawRect(piDisplay, &rc, me->rgbHandleBorder, 
                     RGBA_STRIPALPHA(Border_GetCurrentBorderColor(&me->border)), IDF_RECT_FRAME|IDF_RECT_FILL);
}


void SliderWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   DECL(SliderWidget);

   IDisplay *piDisplay = 0;

   if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      int xSlider, ySlider;

      if (me->bVert) {
         ySlider = y;
         xSlider = x + (me->base.extent.width - me->bextent.width) / 2;
      } else {
         xSlider = x;
         ySlider = y + (me->base.extent.height - me->bextent.height) / 2;
      }

//      // @@@ debug only
//      SETAEERECT(&rc, x, y, me->base.extent.width, me->base.extent.height);
//      IDISPLAY_FillRect(piDisplay, &rc, MAKE_RGB(255,255,0));

      Border_Draw(&me->border, piCanvas, xSlider, ySlider);
      SliderWidget_DrawHandle(me, piDisplay, x, y);

      IDISPLAY_Release(piDisplay);
   }
}


int SliderWidget_SetModel(IWidget *po, IModel *piModel)
{
   DECL(SliderWidget);
   IValueModel *pivm = 0;
   int nErr = IMODEL_QueryInterface(piModel, AEEIID_VALUEMODEL, (void **)&pivm);

   if (!nErr) {
      uint16 pos;

      WidgetBase_SetModel(po, piModel);

      IMODEL_AddListenerEx(piModel, &me->ml, (PFNLISTENER)WidgetBase_InvalidateContent, me);
      WidgetBase_InvalidateExtent(po);

      // ensure pos doesn't exceed maxpos
      pos = SliderWidget_GetPos(me);
      SliderWidget_SetPos(me, (uint16)MIN(me->nMaxPos, pos));
   }

   RELEASEIF(pivm);
   return nErr;
}


/////////////////////////////////////////////////////////////////////////////


boolean SliderWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   DECL(SliderWidget);

   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      return TRUE;
   }

   switch (evt) {
      case EVT_KEY: {
         uint16 nPos = SliderWidget_GetPos(me);
         if (!me->bVert && wParam == AVK_LEFT ||
              me->bVert && wParam == AVK_DOWN) {
            nPos = MAX(0, nPos-1);
         } else if (!me->bVert && wParam == AVK_RIGHT ||
                     me->bVert && wParam == AVK_UP) {
            nPos = MIN(me->nMaxPos, nPos+1);
         } else {
            break;
         }
         SliderWidget_SetPos(me, nPos);
         return TRUE;
      }

      case EVT_WDG_SETPROPERTY:

         switch (wParam) {
            case PROP_SLIDER_MAXPOS: {
               int nPos = SliderWidget_GetPos(me);
               me->nMaxPos = (uint16)dwParam;
               if (nPos > me->nMaxPos) {
                  SliderWidget_SetPos(me, me->nMaxPos);
               }
               return TRUE;
            }

            case PROP_SLIDER_HANDLEBORDERCOLOR:
               me->rgbHandleBorder = (RGBVAL)dwParam;
               WidgetBase_Invalidate(po, 0);
               return TRUE;

            case PROP_LAYOUTSTYLE:
               me->bVert = (dwParam == LAYOUT_VERT) ? 1 : 0;
               WidgetBase_Invalidate(po, 0);
               return TRUE;
         }
         break;

      case EVT_WDG_GETPROPERTY:

         switch (wParam) {
            case PROP_SLIDER_MAXPOS:
               *(uint16*)dwParam = me->nMaxPos;
               return TRUE;
      
            case PROP_SLIDER_HANDLEBORDERCOLOR:
               *(RGBVAL*)dwParam = me->rgbHandleBorder;
               return TRUE;
      
            case PROP_LAYOUTSTYLE:
               *(uint32*)dwParam = (me->bVert ? LAYOUT_VERT : LAYOUT_HORZ);
               return TRUE;
         }
         break;
   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}


/////////////////////////////////////////////////////////////////////////////


void SliderWidget_Dtor(SliderWidget *me)
{
   LISTENER_Cancel(&me->ml);
   RELEASEIF(me->piShell);
   Border_Dtor(&me->border);
   WidgetBase_Dtor(&me->base);
}


void SliderWidget_Delete(SliderWidget *me)
{
   SliderWidget_Dtor(me);
   FREE(me);
}


uint32 SliderWidget_Release(IWidget *po)
{
   DECL(SliderWidget);

   if (me->base.nRefs == 1) {
      SliderWidget_Dtor(me);
   }

   return WidgetBase_Release(po);
}



int SliderWidget_Construct(SliderWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell,
                           IModule *piModule, PFNHANDLER pfnDefHandler)
{
   int nErr = 0;

   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(SliderWidget_HandleEvent));

#define VTFUNC(name) pvt->name = SliderWidget_##name
   VTFUNC( Release );
   VTFUNC( GetPreferredExtent );
   VTFUNC( SetExtent );
   VTFUNC( Draw );
   VTFUNC( SetModel );
#undef VTFUNC

   Border_Ctor(&me->border, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->bextent, TRUE, &WBASE(me)->piViewModel);
   Border_SetBGColor(&me->border, 0, MAKE_RGBA(0xED, 0xEC, 0xFE, 0xFF));
   Border_SetBorderColor(&me->border, 0, MAKE_RGBA(0x6E, 0x6E, 0xA0, 0xFF));
   
   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   me->rgbHandleBorder = RGB_WHITE;

   me->nMaxPos = 10;
   me->bVert   = 0;     // horizontal by default

   me->cyFont = 10;

   // get normal font size to base preferred extent upon
   {
      IFont *piFont = 0;
      if (SUCCESS == ISHELL_CreateFont(me->piShell, AEECLSID_FONTSYSNORMAL, &piFont)) {
         AEEFontInfo ai;
         IFONT_GetInfo(piFont, &ai, sizeof(ai));
         me->cyFont = ai.nAscent + ai.nDescent;
         IFONT_Release(piFont);
      }
   }

   // create the value model
   {
      IModel *piModel = 0;
      nErr = ValueModel_New((IValueModel**)&piModel, piModule);
      if (!nErr) {
         SliderWidget_SetModel((IWidget*)me, piModel);
      }
      RELEASEIF(piModel);
   }

   return nErr;
}


int SliderWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   int nErr = 0;
   SliderWidget *me = MALLOCREC_VTBL(SliderWidget, IWidget);

   if (!me) {
      return ENOMEMORY;
   }

   nErr = SliderWidget_Construct(me, GETVTBL(me, IWidget), piShell, piModule, 0);

   if (!nErr) {
      *ppo = (IWidget*)me;
   } else {
      SliderWidget_Delete(me);
      *ppo = 0;
   }

   return nErr;
}







#endif   /* READYFORPRIMETIME */




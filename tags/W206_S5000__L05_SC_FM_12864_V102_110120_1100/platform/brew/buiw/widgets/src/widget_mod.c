/*
  ========================================================================

  FILE:  widget_mod.c
  
  SERVICES:  

  GENERAL DESCRIPTION: ModWrapper for Widget classes

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "ModelBase.h"
#include "BitmapWidget.h"
#include "BlendWidget.h"
#include "BorderWidget.h"
#include "CheckWidget.h"
#include "ConstraintContainer.h"
#include "CursorWidget.h"
#include "ImageWidget.h"
#include "ListWidget.h"
#include "ProgressWidget.h"
#include "RootContainer.h"
#include "ScrollWidget.h"
#include "SoftkeyWidget.h"
#include "StaticWidget.h"
#include "TitleWidget.h"
#include "TextController.h"
#include "TextModel.h"
#include "TextWidget.h"
#include "VectorModel.h"
#include "ViewportWidget.h"
#include "XYContainer.h"
#include "PropContainer.h"
#include "Transition.h"
#include "CardContainer.h"
#include "ImageStaticWidget.h"
#include "TabWidget.h"
#include "MenuModel.h"
#include "DisplayCanvas.h"
#include "DrawDecoratorWidget.h"
#include "SliderWidget.h"
#include "GridContainer.h"
#include "DateTimeWidget.h"
#include "TextFormatter.h"
#include "ResFile.h"
#include "DefaultLocale.h"
#include "HFontOutline.h"
#include "FontMapModel.h"
#include "TextLayout.h"
#include "xmod.h"

#include "wutil.h"

#include "bid/AEECLSID_OEMLOCALE.bid"
#include "bid/AEECLSID_WIDGETSMOD.bid"
#include "bid/AEECLSID_DEFAULTTEXTCONTROLLER.bid"

#ifdef AEE_STATIC

// Need to rename AEEMod_Load() to prevent duplicate symbols
#define AEEMOD_LOAD           WidgetMod_Load

// Need to rename our module's CreateInstance method to prevent duplicate
// symbols
#define AEEMOD_CREATEINSTANCE WidgetMod_CreateInstance

// Inline a copy of xmodstub for our module
#include "xmodstub.c"

static const AEECLSID      gWidgetExtClasses[] = {
   AEECLSID_ARRAYMODEL, AEECLSID_BITMAPWIDGET, AEECLSID_BLENDWIDGET, AEECLSID_BORDERWIDGET, 
   AEECLSID_CHECKWIDGET, AEECLSID_INTERFACEMODEL, AEECLSID_RADIOWIDGET, AEECLSID_CONSTRAINTCONTAINER, 
   AEECLSID_CURSORWIDGET, AEECLSID_IMAGEWIDGET, AEECLSID_LISTWIDGET, AEECLSID_MODEL, 
   AEECLSID_PICKWIDGET, AEECLSID_GRIDWIDGET, AEECLSID_PROGRESSWIDGET, AEECLSID_ROOTCONTAINER, AEECLSID_ROOTCONTAINER_1,
   AEECLSID_SCROLLBARWIDGET, AEECLSID_SCROLLINDICATORWIDGET, AEECLSID_SOFTKEYWIDGET, AEECLSID_STATICWIDGET, 
   AEECLSID_TEXTMODEL, AEECLSID_TEXTWIDGET, AEECLSID_TITLEWIDGET, AEECLSID_VALUEMODEL, 
   AEECLSID_VECTORMODEL, AEECLSID_VIEWPORTWIDGET, AEECLSID_XYCONTAINER, AEECLSID_PROPCONTAINER, AEECLSID_PROPCONTAINER_1,
   AEECLSID_CARDCONTAINER, AEECLSID_FADER, AEECLSID_MOVER, AEECLSID_IMAGESTATICWIDGET, 
   AEECLSID_TABWIDGET, AEECLSID_MENUMODEL, AEECLSID_DRAWDECORATORWIDGET, AEECLSID_DISPLAYCANVAS,
   AEECLSID_SLIDERWIDGET, AEECLSID_DATEWIDGET, AEECLSID_TIMEWIDGET, AEECLSID_TEXTFORMATTER, AEECLSID_RESFILE,
   AEECLSID_CURRENTLOCALE, AEECLSID_HFONT_OUTLINE, AEECLSID_FONTMAPMODEL, AEECLSID_TEXTLAYOUT,
   0 
};
    
// Returns static module information
PFNMODENTRY WidgetMod_GetModInfo(IShell * ps,AEECLSID ** ppClasses, AEEAppInfo ** pApps, uint16 * pnApps,uint16 * pwMinPriv)
{
   *pApps = NULL;
   *pnApps = 0;
   *ppClasses = (AEECLSID*)gWidgetExtClasses;
   return((PFNMODENTRY)AEEMOD_LOAD);
}

#else 

// Select the default "xModule_CreateInstance" name for our module's
// CreateInstance method
#define AEEMOD_CREATEINSTANCE xModule_CreateInstance

#endif /* AEE_STATIC */


int AEEMOD_CREATEINSTANCE(IModule *piModule, IShell *piShell, AEECLSID clsid, void **ppo)
{
   int result = ECLASSNOTSUPPORT;

   switch(clsid) {
   case AEECLSID_ARRAYMODEL:
      result = ArrayModel_New((IArrayModel **)ppo, piModule);
      break;
   case AEECLSID_BITMAPWIDGET:
      result = BitmapWidget_New((IWidget **)ppo, piShell, piModule);
      break;
   case AEECLSID_BLENDWIDGET:
      result = BlendWidget_New((IDecorator **)ppo, piModule);
      break;
   case AEECLSID_BORDERWIDGET:
      result = BorderWidget_New((IDecorator **)ppo, piShell, piModule);
      break;
   case AEECLSID_CHECKWIDGET:
      result = CheckWidget_New((IWidget **)ppo, 0, piShell, piModule);
      break;
   case AEECLSID_INTERFACEMODEL:
      result = InterfaceModel_New((IInterfaceModel **)ppo, piModule);
      break;
   case AEECLSID_RADIOWIDGET:
      result = RadioWidget_New((IWidget **)ppo, 0, piShell, piModule);
      break;
   case AEECLSID_CONSTRAINTCONTAINER:
      result = ConstraintContainer_New((IConstraintContainer **)ppo, piShell, piModule);
      break;
   case AEECLSID_CURSORWIDGET:
      result = CursorWidget_New((IWidget **)ppo, piShell, piModule);
      break;
   case AEECLSID_IMAGEWIDGET:
      result = ImageWidget_New((IDecorator **)ppo, piShell, piModule);
      break;
   case AEECLSID_LISTWIDGET:
      result = ListWidget_New((IDecorator **)ppo, 0, piShell, piModule);
      break;
   case AEECLSID_MODEL:
      result = ModelBase_New((IModel **)ppo, piModule);
      break;
   case AEECLSID_PICKWIDGET:
      result = PickWidget_New((IDecorator **)ppo, 0, piShell, piModule);
      break;
   case AEECLSID_GRIDWIDGET:
      result = GridWidget_New((IDecorator **)ppo, 0, piShell, piModule);
      break;
   case AEECLSID_PROGRESSWIDGET:
      result = ProgressWidget_New((IWidget **)ppo, 0, piShell, piModule);
      break;
   case AEECLSID_ROOTCONTAINER_1:
   case AEECLSID_ROOTCONTAINER:
      result = RootContainer_New((IRootContainer **)ppo, piShell, piModule);
      break;
   case AEECLSID_SCROLLBARWIDGET:
      result = ScrollbarWidget_New((IDecorator **)ppo, piShell, piModule);
      break;
   case AEECLSID_SCROLLINDICATORWIDGET:
      result = ScrollIndicatorWidget_New((IDecorator **)ppo, piShell, piModule);
      break;
   case AEECLSID_SOFTKEYWIDGET:
      result = SoftkeyWidget_New((IWidget **)ppo, piShell, piModule);
      break;
   case AEECLSID_STATICWIDGET:
      result = StaticWidget_New((IWidget **)ppo, 0, piShell, piModule);
      break;
   case AEECLSID_TEXTMODEL:
      result = TextModel_New((ITextModel **)ppo, piModule);
      break;
   case AEECLSID_TEXTWIDGET:
      result = TextWidget_New((IWidget **)ppo, 0, piShell, piModule);
      break;
   case AEECLSID_TITLEWIDGET:
      result = TitleWidget_New((IDecorator **)ppo, piShell, piModule);
      break;
   case AEECLSID_VALUEMODEL:
      result = ValueModel_New((IValueModel **)ppo, piModule);
      break;
   case AEECLSID_VECTORMODEL:
      result = VectorModel_New((IVectorModel **)ppo, piModule);
      break;
   case AEECLSID_VIEWPORTWIDGET:
      result = ViewportWidget_New((IDecorator **)ppo, piShell, piModule);
      break;
   case AEECLSID_XYCONTAINER:
      result = XYContainer_New((IXYContainer **)ppo, piShell, piModule);
      break;
   case AEECLSID_PROPCONTAINER_1:
   case AEECLSID_PROPCONTAINER:
      result = PropContainer_New((IPropContainer **)ppo, piShell, piModule);
      break;
   case AEECLSID_CARDCONTAINER:
      result = CardContainer_New((ICardContainer**)ppo, piShell, piModule);
      break;
   case AEECLSID_FADER:
      result = Fader_New((ITransition **)ppo, piShell, piModule);
      break;
   case AEECLSID_MOVER:
      result = Mover_New((ITransition **)ppo, piShell, piModule);
      break;
   case AEECLSID_IMAGESTATICWIDGET:
      result = ImageStaticWidget_New((IWidget **)ppo, piShell, piModule);
      break;
   case AEECLSID_TABWIDGET:
      result = TabWidget_New((IDecorator**)ppo, piShell, piModule);
      break;
   case AEECLSID_MENUMODEL:
      result = MenuModel_New((IMenuModel **)ppo, piModule);
      break;
   case AEECLSID_DRAWDECORATORWIDGET:
      result = DrawDecoratorWidget_New((IDrawDecorator **)ppo, piModule);
      break;
   case AEECLSID_DISPLAYCANVAS:
      result = DisplayCanvas_New((IDisplayCanvas**)ppo, piModule);
      break;
   case AEECLSID_SLIDERWIDGET:
      result = SliderWidget_New((IWidget**)ppo, piShell, piModule);
      break;
   case AEECLSID_DATEWIDGET:
      result = DateWidget_New((IWidget**)ppo, piShell, piModule);
      break;
   case AEECLSID_TIMEWIDGET:
      result = TimeWidget_New((IWidget**)ppo, piShell, piModule);
      break;
   case AEECLSID_DEFAULTTEXTCONTROLLER:
      result = TextController_New((IController**)ppo, piShell, piModule);
      break;
   case AEECLSID_TEXTFORMATTER:
      result = TextFormatter_New((ITextFormatter**)ppo, piShell, piModule);
      break;
   case AEECLSID_RESFILE_1:
   case AEECLSID_RESFILE:   
      result = ResFile_New((IResFile **)ppo, piModule, piShell);
	  break;
   case AEECLSID_CURRENTLOCALE:
      result = ISHELL_CreateInstance(piShell, AEECLSID_OEMLOCALE, ppo);
      if (SUCCESS != result) {
         // no OEMLOCALE has been defined on this device.  Fall back and
         // create an instance of our default ILocale object instead.
         result = DefaultLocale_New((ILocale **)ppo, piShell, piModule); 
      }
      break;
   case AEECLSID_HFONT_OUTLINE:
      result = HFontOutline_New((IHFont **)ppo, piShell, piModule);
      break;
   case AEECLSID_GRIDCONTAINER:
      result = GridContainer_New((IGridContainer**)ppo, piShell, piModule);
      break;
   case AEECLSID_JULIANDATEWIDGET:
      result = JulianDateWidget_New((IWidget **)ppo, piShell, piModule);
      break;
   case AEECLSID_JULIANTIMEWIDGET:
      result = JulianTimeWidget_New((IWidget **)ppo, piShell, piModule);
      break;
   case AEECLSID_FONTMAPMODEL:
      result = FontMapModel_New((IFontMapModel **)ppo, piModule);
      break;
   case AEECLSID_TEXTLAYOUT:
      result = TextLayout_New((ITextLayout **)ppo, piShell, piModule);
      break;
   case AEECLSID_WIDGETSMOD:
      if (piModule) {
         IMODULE_AddRef(piModule);
         *(IModule**)ppo = piModule;
         result = SUCCESS;
      }
      break;
   default:
      result = ECLASSNOTSUPPORT;
   }
   
   return result;
}

/*======================================================
FILE:       CustomWidget.c

SERVICES:   Custom Widget class that does nothing

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "CustomWidget_priv.h"
#include "CustomWidgetBase.h"
#include "AEEWProperties.h"
#include "samplewidgets_util.h"

/////////////////////////////////////////////////////////////////////
// CustomWidget structure

typedef struct CustomWidget 
{
   WidgetBase     base;
   ModelListener  modelListener;
   uint32         dwFlags;
} CustomWidget;

/////////////////////////////////////////////////////////////////////
// Function prototypes

static void CustomWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut);
static int CustomWidget_SetModel(IWidget *po, IModel *piModel);
static void CustomWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y);
static boolean CustomWidget_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn);
static uint32 CustomWidget_Release(IWidget *po);
static boolean CustomWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam);

static __inline void CustomWidget_Invalidate(CustomWidget *me);
static void CustomWidget_ModelChanged(CustomWidget *me, ModelEvent *pEvent);

static void CustomWidget_Ctor(CustomWidget *me, AEEVTBL(IWidget) *pvt, IModule *piModule, PFNHANDLER pfnDefHandler);
static void CustomWidget_Dtor(CustomWidget *me);
static int CustomWidget_Initialize(CustomWidget *me, IShell *piShell);

/////////////////////////////////////////////////////////////////////
// CustomWidget implementation

//--------------------------------------------------------------------
//    IWidget Method Overrides
//--------------------------------------------------------------------

/* -----------------------------------------------------------------------
    CustomWidget_GetPreferredExtent - returns the preferred extent of this
    widget
   --- */
static void CustomWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
//   CustomWidget *me = (CustomWidget *)po;
   
   // calculate and return preferred extent ...
   pweOut->width  = 10;
   pweOut->height = 10;
}


/* -----------------------------------------------------------------------
    CustomWidget_SetModel - set this widget's model, if supported
   --- */
static int CustomWidget_SetModel(IWidget *po, IModel *piModel)
{
   CustomWidget *me = (CustomWidget *)po;
   IInterfaceModel *piim = NULL;

   if (SUCCESS != IMODEL_QueryInterface(piModel, AEEIID_INTERFACEMODEL, (void **)&piim)) {
      return ECLASSNOTSUPPORT;
   }

   WidgetBase_SetModel(po, (IModel*)piim);
   RELEASEIF(piim);

   // Re-register listener
   LISTENER_Cancel(&me->modelListener);
   LISTENER_Init(&me->modelListener, CustomWidget_ModelChanged, me);
   IMODEL_AddListener(me->base.piModel, &me->modelListener);

   CustomWidget_ModelChanged(me, NULL);
   return SUCCESS;
}


/* -----------------------------------------------------------------------
    CustomWidget_Draw - actually draw ourselves
   --- */
static void CustomWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
 //  CustomWidget *me = (CustomWidget *)po;
   AEERect  rcClip;
     
   ZEROAT(&rcClip);
   ICANVAS_GetClipRect(piCanvas, &rcClip);

   // draw ourselves ...
   //CustomWidget_DrawWidget(me, x, y, &rcClip);
}


/* -----------------------------------------------------------------------
    CustomWidget_IntersectOpaque - retrieve the intersection of the given 
    rectangle with the largest opaque region of a given widget
   --- */
static boolean CustomWidget_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
//   CustomWidget *me = (CustomWidget *)po;
   return WidgetBase_IntersectOpaque(po, prcOut, prcIn);
}


/* -----------------------------------------------------------------------
    CustomWidget_Release - release this widget
   --- */
static uint32 CustomWidget_Release(IWidget *po)
{
   CustomWidget *me = (CustomWidget *)po;

   if (me->base.nRefs == 1) {
      CustomWidget_Dtor(me);
   }
   return WidgetBase_Release(po);
}


/* -----------------------------------------------------------------------
    CustomWidget_HandleEvent - handle events ...
   --- */
static boolean CustomWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   CustomWidget *me = (CustomWidget *)po;

   switch(evt) {

      // get property ...
      case EVT_WDG_GETPROPERTY:
         if (wParam == PROP_FLAGS) {
            *((uint32 *)dwParam) = me->dwFlags;
            return TRUE;
         }

      // set property
      case EVT_WDG_SETPROPERTY:
         if (wParam == PROP_FLAGS ){
            me->dwFlags = dwParam;
         } else {
            // unknown property, let WidgetBase try.
            break;
         }
         
         // property set.  Invalidate ourselves.
         CustomWidget_Invalidate(me);
         return TRUE;

      // handle other events ...
      //[...]

      // unrecognized event
      default:
         break;
   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}


/* -----------------------------------------------------------------------
    CustomWidget_Invalidate - invalidate ourselves 
   --- */
static __inline void CustomWidget_Invalidate(CustomWidget *me) 
{ 
   WidgetBase_InvalidateExtent((IWidget*)me); 
}


/* -----------------------------------------------------------------------
    CustomWidget_ModelChanged 
   --- */
static void CustomWidget_ModelChanged(CustomWidget *me, ModelEvent *pEvent)
{
   // do what's needed when our model changes ...

   // most likely we'll need to redraw ourselves ...
   CustomWidget_Invalidate(me);
}


/* -----------------------------------------------------------------------
    CustomWidget_Ctor 
   --- */
static void CustomWidget_Ctor(CustomWidget *me, AEEVTBL(IWidget) *pvt, IModule *piModule, PFNHANDLER pfnDefHandler)
{
   // contruct base
   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(CustomWidget_HandleEvent));

   // construct derived members
   pvt->Release            = CustomWidget_Release;
   pvt->GetPreferredExtent = CustomWidget_GetPreferredExtent;
   pvt->Draw               = CustomWidget_Draw;
   pvt->IntersectOpaque    = CustomWidget_IntersectOpaque;
   pvt->SetModel           = CustomWidget_SetModel;

   // initialize other members
   //[...]
}


/* -----------------------------------------------------------------------
    CustomWidget_Dtor
   --- */
static void CustomWidget_Dtor(CustomWidget *me)
{
   // release internal members (if any) ...

   // free ourselves
   WidgetBase_Dtor(&me->base);
}


/* -----------------------------------------------------------------------
    CustomWidget_Initialize
   --- */
static int CustomWidget_Initialize(CustomWidget *me, IShell *piShell)
{
   int dwResult = SUCCESS;

   /* 
   
     Create a model here if so desired ...

   IModel *piModel = 0;

   dwResult = ISHELL_CreateInstance(piShell, AEECLSID_INTERFACEMODEL, (void**)&piModel);
   
   if (!dwResult) {
      CustomWidget_SetModel((IWidget*)me, piModel);
   }

   RELEASEIF(piModel);

   */ 
   return dwResult;
}


/* -----------------------------------------------------------------------
    CustomWidget_New  
   --- */
int CustomWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   int dwResult;
   CustomWidget *me = MALLOCREC_EX(CustomWidget, sizeof(AEEVTBL(IWidget)));

   *ppo = 0;

   if (!me) {
      return ENOMEMORY;
   }

   CustomWidget_Ctor(me, GETVTBL(me,IWidget), piModule, 0);
   dwResult = CustomWidget_Initialize(me, piShell);

   if (!dwResult) {
      *ppo = (IWidget*)me;
   } else {
      CustomWidget_Dtor(me);
      FREE(me);
   }

   return dwResult;
}


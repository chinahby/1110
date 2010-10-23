/*
  ========================================================================

  FILE:  ImageStaticWidget.c
  
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

#include "ImageStaticWidget.h"
#include "wutil.h"

#include "AEEImageWidget.h"
#include "AEEStaticWidget.h"
#include "AEEBorderWidget.h"
#include "AEEDecorator.h"


#define ME_FROM_WIDGET     ImageStaticWidget *me = (ImageStaticWidget*)po->pMe

/*
   We are assuming that the ImageStaticInfo data is persistent and does not need 
   to be copied in this call
*/
static void ImageStaticWidget_ModelChanged(ImageStaticWidget *me, ModelEvent *pEvent)
{
   if (me->pivm && (pEvent->evCode == EVT_MDL_VALUE)) {

      int nLen;
      ImageStaticInfo *pInfo = IVALUEMODEL_GetValue(me->pivm, &nLen);

      if (pInfo) {

         IInterfaceModel *piim = 0;
         IValueModel *pivm = 0;

         if (SUCCESS == IWIDGET_GetModel(me->piImage, AEEIID_INTERFACEMODEL, (IModel **)&piim)) {
            IINTERFACEMODEL_SetIPtr(piim, (IBase *)pInfo->piImage, AEEIID_IMAGE);
            IINTERFACEMODEL_Release(piim);
         }

         if (SUCCESS == IWIDGET_GetModel(me->piLabel, AEEIID_VALUEMODEL, (IModel**)&pivm)) {
            IVALUEMODEL_SetTextPtr(pivm, pInfo->pwText, -1);
            IVALUEMODEL_Release(pivm);
         }

         if (!(me->dwFlags & ISWF_NOTEXT)) {
            WExtent weImage, weLabel;
            int  layoutStyle = LAYOUT_NONE;

            IWIDGET_GetPreferredExtent(me->piImage, &weImage);
            IWIDGET_GetExtent(me->piLabel, &weLabel);

            IWIDGET_GetLayoutStyle(&me->base.base.widget, &layoutStyle);

            if (LAYOUT_VERT == layoutStyle) {
               // Set the width of the image to the width of the label
               // but allow it to indicate how high it wants to be
               weImage.width = weLabel.width;
            } else {
               // Set the height of the image to the height of the label
               // but allow it to indicate how wide it wants to be
               weImage.height = weLabel.height;
            }

            IWIDGET_SetExtent(me->piImage, &weImage);
         }
      }
   }
}


static __inline void ImageStaticWidget_UpdateModel(ImageStaticWidget *me)
{
   ModelEvent ev;
   ev.evCode = EVT_MDL_VALUE;
   ImageStaticWidget_ModelChanged(me, &ev);
}


static int ImageStaticWidget_GetModel(IWidget *po, AEECLSID iid, IModel **ppwm)
{
   ME_FROM_WIDGET;
   return IVALUEMODEL_QueryInterface(me->pivm, iid, (void**)ppwm);
}


static int ImageStaticWidget_SetModel(IWidget *po, IModel *piModel)
{
   ME_FROM_WIDGET;

   RELEASEIF(me->pivm);

   if (SUCCESS != IMODEL_QueryInterface(piModel, AEEIID_VALUEMODEL, (void **)&me->pivm)) {
      return ECLASSNOTSUPPORT;
   }

   // force model update
   ImageStaticWidget_UpdateModel(me);

   // then add listener for future updates
   IMODEL_AddListenerEx((IModel*)me->pivm, &me->modelListener, (PFNLISTENER)ImageStaticWidget_ModelChanged, me);
   return SUCCESS;
}


static void ImageStaticWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   ME_FROM_WIDGET;
   WExtent wei, wes, weis;

   if (me->dwFlags & ISWF_NOIMAGE) {
      wei.width = wei.height = 0;
   } else {
      IWIDGET_GetPreferredExtent(me->piImage, &wei);
   }

   if (me->dwFlags & ISWF_NOTEXT) {
      wes.width = wes.height = 0;
   } else {
      IWIDGET_GetPreferredExtent(me->piLabel, &wes);
   }

   weis.width = wei.width + wes.width;
   weis.height = MAX(wei.height, wes.height);

   Border_CalcPreferredExtent(&me->base.base.border, pweOut, &weis);
}



static void ImageStaticWidget_HideTextWidget(ImageStaticWidget *me, boolean bHide)
{
   WidgetProp wp;
   if (SUCCESS == IPROPCONTAINER_GetProp((IPropContainer*)me, me->piLabel, &wp)) {
      wp.bVisible = !bHide;
      IPROPCONTAINER_SetProp((IPropContainer*)me, me->piLabel, me->piLabel, &wp);
   }
   if (SUCCESS == IPROPCONTAINER_GetProp((IPropContainer*)me, me->piImage, &wp)) {
      wp.prop = (bHide ? 1 : WIDGET_SIZE_TO_FIT);
      IPROPCONTAINER_SetProp((IPropContainer*)me, me->piImage, me->piImage, &wp);
   }
}

static void ImageStaticWidget_HideImageWidget(ImageStaticWidget *me, boolean bHide)
{
   WidgetProp wp;
   if (SUCCESS == IPROPCONTAINER_GetProp((IPropContainer*)me, me->piImage, &wp)) {
      wp.bVisible = !bHide;
      IPROPCONTAINER_SetProp((IPropContainer*)me, me->piImage, me->piImage, &wp);
   }
}


// we do nothing when piwNew == 0, we do not allow the widget to be removed from
// the prop container, only to be hidden
static int ImageStaticWidget_SetWidget(ImageStaticWidget *me, IWidget *piwNew, boolean bImage)
{
   WidgetProp wp;
   IWidget **ppi = bImage ? &me->piImage : &me->piLabel;
   boolean bImageRight = (me->dwFlags & ISWF_LAYOUT_IMAGERIGHT) != 0;
   // figure out where to insert the widget
   IWidget *piwInsert = (bImage == bImageRight) ? WIDGET_ZTOPMOST : WIDGET_ZBOTTOMMOST;

   if (!piwNew) {
      return FALSE;
   }

   //
   IPROPCONTAINER_GetProp((IPropContainer*)me, *ppi, &wp);

   // Remove the current label...
   IPROPCONTAINER_Remove((IPropContainer*)me, *ppi);
   IWIDGET_Release(*ppi);

   // Add the new one...
   *ppi = piwNew;

   IWIDGET_AddRef(piwNew);

   IPROPCONTAINER_Insert((IPropContainer*)me, piwNew, piwInsert, &wp);

   // force model update
   ImageStaticWidget_UpdateModel(me);
   return TRUE;
}


static boolean ImageStaticWidget_DefHandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   // let PropContainer have first crack at all events
   // except for SetProperty/Flags
   if (evt != EVT_WDG_SETPROPERTY || wParam != PROP_FLAGS) {
      if (PropContainer_DefHandleEvent(po, evt, wParam, dwParam)) {
         return TRUE;
      }
   }

   if (evt == EVT_WDG_GETPROPERTY) {

      switch(wParam) {
         case PROP_IMAGESTATIC_IMAGEWIDGET:
            *(IWidget **)dwParam = me->piImage;
            ADDREFIF(me->piImage);
            return TRUE;
   
         case PROP_IMAGESTATIC_STATICWIDGET:
            *(IWidget **)dwParam = me->piLabel;
            ADDREFIF(me->piLabel);
            return TRUE;
      }

   } else if (evt == EVT_WDG_SETPROPERTY) {

      switch(wParam) {
         case PROP_HINT_WIDTH:
            {
               int  layoutStyle = LAYOUT_NONE;

               IWIDGET_GetLayoutStyle(&me->base.base.widget, &layoutStyle);

               // If we are in a horizontal layout and have an image, adjust
               // the caller's hint width by the width of our image widget
               // before passing the hint down to the static widget.
               if ( (LAYOUT_HORZ == layoutStyle) &&
                    !(me->dwFlags & ISWF_NOIMAGE) ) {
                  WExtent wei;
      
                  IWIDGET_GetPreferredExtent(me->piImage, &wei);

                  dwParam = MAX(0, (int)dwParam - wei.width);
               }

               break; // pass PROP_HINT_WIDTH on to StaticWidget
            }

         case PROP_SELECTED:
            IWIDGET_SetSelected(me->piImage, (boolean)dwParam);
            IWIDGET_SetSelected(me->piLabel, (boolean)dwParam);
            return TRUE;
   
         case PROP_IMAGESTATIC_IMAGEWIDGET:
            return ImageStaticWidget_SetWidget(me, (IWidget*)dwParam, 1);
   
         case PROP_IMAGESTATIC_STATICWIDGET:
            return ImageStaticWidget_SetWidget(me, (IWidget*)dwParam, 0);
   
         case PROP_FLAGS: {
            IWidget *piwInsert = 0;
   
            uint32 dwDiff;
            uint32 dwOldFlags = me->dwFlags;
            me->dwFlags = (dwParam & 0xFF000000);

            dwDiff = me->dwFlags ^ dwOldFlags;

            // if layout changed
            if (dwDiff & ISWF_LAYOUT_IMAGERIGHT) {

               WidgetProp prop;
               piwInsert = (me->dwFlags & ISWF_LAYOUT_IMAGERIGHT) ? WIDGET_ZTOPMOST 
                                                                  : WIDGET_ZBOTTOMMOST;
               IPROPCONTAINER_GetProp((IPropContainer*)me, me->piImage, &prop);
               IPROPCONTAINER_SetProp((IPropContainer*)me, me->piImage, piwInsert, &prop);
            }

            // if display image flag changed
            if (dwDiff & ISWF_NOIMAGE) {
               ImageStaticWidget_HideImageWidget(me, (boolean)((me->dwFlags & ISWF_NOIMAGE) != 0));

            }
            // if display text flag changed
            if (dwDiff & ISWF_NOTEXT) {
               ImageStaticWidget_HideTextWidget(me, (boolean)((me->dwFlags & ISWF_NOTEXT) != 0));
            }
            // if image widget flag(s) changed, pass to image widget
            if (dwDiff & IWF_HASSELECTIMAGE) {
               IWIDGET_SetFlags(me->piImage, (me->dwFlags & IWF_HASSELECTIMAGE));
            }

            // strip out ISW specific flags
            dwParam &= ~(ISWF_LAYOUT_IMAGERIGHT | ISWF_NOIMAGE | ISWF_NOTEXT | IWF_HASSELECTIMAGE);

         } break; // pass PROP_FLAGS on to StaticWidget
      }
   }

   return IWIDGET_HandleEvent(me->piLabel, evt, wParam, dwParam);
}


static void ImageStaticWidget_Dtor(ImageStaticWidget *me)
{
   LISTENER_Cancel(&me->modelListener);
   RELEASEIF(me->piImage);
   RELEASEIF(me->piLabel);
   RELEASEIF(me->pivm);
}


uint32 ImageStaticWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (me->base.base.nRefs == 1) {
      ImageStaticWidget_Dtor(me);
   }      

   return ContainerBase_Release((IContainer*)me);
}


void ImageStaticWidget_Ctor(ImageStaticWidget *me, AEEVTBL(IPropContainer) *pvt, 
                            IShell *piShell, IModule *piModule) 
{
   PropContainer_Ctor(&me->base, pvt, piModule, (PFNHANDLER)ImageStaticWidget_DefHandleEvent);
   
   // override IWidget method(s)
#  define WVT(name)    me->base.base.vtWidget.name = ImageStaticWidget_##name
   WVT(SetModel);
   WVT(GetModel);
   WVT(Release);
   WVT(GetPreferredExtent);
}


int ImageStaticWidget_Construct(ImageStaticWidget *me, IShell *piShell, IWidget **ppo) 
{
   int nErr;
   IValueModel *pivm = 0;
   
   // create the value model
   nErr = ISHELL_CreateInstance(piShell, AEECLSID_VALUEMODEL, (void **)&pivm);

   // create the Image
   if (!nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_IMAGEWIDGET, (void **)&me->piImage);
   }

   // create the static
   if (!nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_STATICWIDGET, (void **)&me->piLabel);
   }

   if (!nErr) {
      WidgetProp prop;

      IWIDGET_SetFlags(me->piImage, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
      
      // Add image
      prop.bVisible  = 1;
      prop.prop      = WIDGET_SIZE_TO_FIT;   // take up widget's preferred extent
      nErr = IPROPCONTAINER_Insert((IPropContainer*)me, me->piImage, WIDGET_ZNORMAL, &prop);
   }

   if (!nErr) {
      WidgetProp prop;

      IWIDGET_SetFlags(me->piLabel, IDF_ALIGN_LEFT | IDF_ALIGN_MIDDLE);
      IWIDGET_SetLeftPadding(me->piLabel, 2);

      // Add label
      prop.bVisible  = 1;
      prop.prop      = 1;  // take up all remaining space
      nErr = IPROPCONTAINER_Insert((IPropContainer*)me, me->piLabel, WIDGET_ZNORMAL, &prop);
   }

   if (!nErr) {
      
      // get the widget ptr
      IWidget *piw = &me->base.base.widget;
      
      // Set layout style to horizontal
      IWIDGET_SetLayoutStyle(piw, LAYOUT_HORZ);
      
      // Set border properties
      IWIDGET_SetBorderWidth(piw, 0);

      ImageStaticWidget_SetModel(piw, (IModel*)pivm);

      *ppo = piw;

   } else {
      ImageStaticWidget_Dtor(me);
      FREE(me);
      *ppo = 0;
   }

   RELEASEIF(pivm);

   return nErr;
}


int ImageStaticWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   ImageStaticWidget *me = MALLOCREC_VTBL(ImageStaticWidget, IWidget);

   if (!me) {
      *ppo = 0;
      return ENOMEMORY;
   }

   ImageStaticWidget_Ctor(me, GETVTBL(me,IPropContainer), piShell, piModule);

   return ImageStaticWidget_Construct(me, piShell, ppo);
}



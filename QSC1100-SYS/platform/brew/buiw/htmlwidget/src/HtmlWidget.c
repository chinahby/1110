/*
  ========================================================================

  FILE:  HtmlWidget.c
  
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
#include "AEEHtmlWidget.h"
#include "AEEDisplayCanvas.h"
#include "AEETextWidget.h"
#include "HtmlWidget.h"
#include "DocModel.h"
#include "AEEWProperties.h"
#include "HtmlViewModel.h"
#include "wutil.h"


#define ME_FROM_WIDGET   HtmlWidget *me = (HtmlWidget*)po

#define DOCMODEL(p)     ((IDocModel*)((p)->base.piModel))
#define VIEWMODEL(p)    ((IHtmlViewModel*)((p)->base.piViewModel))

/////////////////////////////////////////////////////////////////////////////

// forward decls
void HtmlWidget_Cancel(HtmlWidget *me, int nCode);

/////////////////////////////////////////////////////////////////////////////

void HtmlWidget_Peek(HtmlWidget *me)
{
   char *pc;
   int32 cbAvail;
   int32 nCode;

   // set busy state
   me->isbusy = 1;
   pc = (char*)DocModel_Peek(DOCMODEL(me), &cbAvail, &nCode);

   if (cbAvail > 0) {
      DoxParse_Write(&me->doxparse, pc, cbAvail);
      DocModel_Advance(DOCMODEL(me), cbAvail);
   }

   if (nCode == IPEEK_END || nCode == IPEEK_ERROR) {
      HtmlWidget_Cancel(me, (nCode == IPEEK_ERROR ? EFAILED : SUCCESS));
	  me->isbusy = 0;
   }

   DocModel_Peekable(DOCMODEL(me), &me->cbkPeek);
}

void HtmlWidget_ResetDoc(HtmlWidget *me)
{
   DoxParse_Ctor(&me->doxparse, &me->dox);
   if (!CALLBACK_IsQueued(&me->cbkPeek)) {
      CALLBACK_Init(&me->cbkPeek, HtmlWidget_Peek, me);
      DocModel_Peekable(DOCMODEL(me), &me->cbkPeek);
   }
   WidgetBase_InvalidateContent((IWidget*)me);
}


void HtmlWidget_Listener(HtmlWidget *me, ModelEvent *pEvent)
{
   switch (pEvent->evCode) {

      case EVT_HDM_RESET:
         HtmlWidget_ResetDoc(me);
         break;
      
      case EVT_HDM_STOP:
         if (CALLBACK_IsQueued(&me->cbkPeek)) {
            HtmlWidget_Cancel(me, EFAILED);
         }
         break;
   }
}



void HtmlWidget_Cancel(HtmlWidget *me, int nCode)
{
   DoxParse_Terminate(&me->doxparse);

   me->dox.fSendContentDone = 1;
   xDox_Layout(&me->dox);

   CALLBACK_Cancel(&me->cbkPeek);
   IHTMLVIEWMODEL_NotifyDone(VIEWMODEL(me), nCode);

   WidgetBase_InvalidateContent((IWidget*)me);
}


void HtmlWidget_UseFont(HtmlWidget *me, IFont *piFont)
{
   if (piFont) {
      RELEASEIF(me->piFont);
      me->piFont = piFont;
      IFONT_AddRef(me->piFont);
      // @@@ recalc layout here ???
   }
}


void HtmlWidget_DoUpdate(HtmlWidget *me)
{
   while (xDox_Update(&me->dox))
      ;
   
   WidgetBase_InvalidateContent((IWidget*)me);
}


void HtmlWidget_InvalidateCB(HtmlWidget *me)
{
   CALLBACK_Resume(&me->cbkUpdate, HtmlWidget_DoUpdate, me, me->piShell);
}

/////////////////////////////////////////////////////////////////////////////
// 


/*
:: HtmlWidget_Draw
*/
void HtmlWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;
   
   Border_Draw(&me->border, piCanvas, x, y);
   Border_AdjustDisplayClipRect(&me->border, piCanvas, x, y);

   Dox_Draw(&me->dox, piCanvas, x + me->border.rcClient.x, y + me->border.rcClient.y);
}


/*
:: HtmlWidget_GetPreferredExtent
*/
void HtmlWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   WExtent weClient;
   ME_FROM_WIDGET;

   weClient.width = 20;
   weClient.height = 40;

   if (me->piFont) {
     weClient.width = me->dox.rc.dx;
	  weClient.height = me->dox.dsi.yDocHeight;
   }

   Border_CalcPreferredExtent(&me->border, pweOut, &weClient);
}


/*
:: HtmlWidget_SetExtent
*/
void HtmlWidget_SetExtent(IWidget *po, WExtent *pe)
{
   ME_FROM_WIDGET;
   WidgetBase_SetExtent((IWidget*)&me->base, pe);
   Border_CalcClientRect(&me->border);
   xDox_SetRect(&me->dox, &me->border.rcClient);
}


/*
:: HtmlWidget_SetModel
*/
int HtmlWidget_SetModel(IWidget *po, IModel *piModel)
{
   return EUNSUPPORTED;
}


/*
:: HtmlWidget_IntersectOpaque
*/
boolean HtmlWidget_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
   ME_FROM_WIDGET;
   return Border_IntersectOpaque(&me->border, prcOut, prcIn);
}


/////////////////////////////////////////////////////////////////////////////
// Event Handler


/*
:: HtmlWidget_HandleEvent
*/
static boolean HtmlWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   int dx = me->border.rcClient.dx;
   int dy = me->border.rcClient.dy;

   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      if (dx != me->border.rcClient.dx ||
          dy != me->border.rcClient.dy) {
         xDox_SetRect(&me->dox, &me->border.rcClient);
         xDox_Invalidate(&me->dox, DOX_INVALWRAP);
      }
      return TRUE;
   }

   if (EVT_KEY == evt && xDox_HandleEvent(&me->dox, evt, wParam, dwParam)) {
      return TRUE;
   }

   switch (evt) {
      
      case EVT_KEY:
         if (wParam == AVK_UP || wParam == AVK_DOWN) {
            return xDox_Move(&me->dox, (boolean)(wParam == AVK_DOWN));
         }
         break;
      case EVT_WDG_SETFOCUS:
         xDox_SetActive(&me->dox, (boolean)(wParam != 0));
         return TRUE;

      case EVT_WDG_SETPROPERTY:
         if (wParam == PROP_FONT_CLASS) {
            IFont *piFont = 0;
            if (SUCCESS == ISHELL_CreateInstance(me->piShell, (AEECLSID)dwParam, (void**)&piFont)) {
               HtmlWidget_UseFont(me, piFont);
            }
            RELEASEIF(piFont);
            return TRUE;
         
         } else if (wParam == PROP_FONT) {
            HtmlWidget_UseFont(me, (IFont*)dwParam);
            return TRUE;
         } else if (wParam == PROP_HTMLW_FONT_NORMAL) {
           Dox_SetFont(&me->dox, AEE_FONT_NORMAL, (IFont*)dwParam);
           return TRUE;
         } else if (wParam == PROP_HTMLW_FONT_BOLD) {
           Dox_SetFont(&me->dox, AEE_FONT_BOLD, (IFont*)dwParam);
           return TRUE;
         } else if (wParam == PROP_HTMLW_FONT_LARGE) {
           Dox_SetFont(&me->dox, AEE_FONT_LARGE, (IFont*)dwParam);
           return TRUE;
         } else if (wParam == PROP_FLAGS) {
            Dox_SetFlags(&me->dox, dwParam);
            return TRUE;
         
         } else if (wParam == PROP_HTMLW_URL) {
            xDox_SetUrl(&me->dox, (const char*)dwParam);
            return TRUE;

         } else if (wParam == PROP_HTMLW_IWEB) {
            xDox_SetIWeb(&me->dox, (IWeb*)dwParam);
            return TRUE;

         } else if (wParam == PROP_HTMLW_INDENT) {
            me->dox.cfg.xIndent = (int32)dwParam;
            xDox_Invalidate(&me->dox, DOX_INVALWRAP);
            return TRUE;

         } else if (wParam == PROP_HTMLW_VSPACE) {
            me->dox.cfg.ySpace = (int32)dwParam;
            xDox_Invalidate(&me->dox, DOX_INVALWRAP);
            return TRUE;

         } else if (wParam == PROP_HTMLW_BUFFERSIZE) {
            if (SUCCESS == xDox_SetBufferSize(&me->dox, dwParam)) {
               DoxParse_Resize(&me->doxparse, &me->dox);
               return TRUE;
            }
            return FALSE;

         } else if (wParam == PROP_HTMLW_FOCUSPOS) {
            xDox_SetFocusPos(&me->dox, (int32)dwParam);
            return TRUE;

         } else if (wParam == PROP_HTMLW_SCROLLPOS) {
            xDox_SetScrollPos(&me->dox, (int32)dwParam);
            return TRUE;

         } else if (wParam == PROP_HTMLW_SCROLLMLLINK) {
            xDox_SetScrollMLLink(&me->dox, (boolean)dwParam);
            return TRUE;

         } else if (wParam == PROP_HTMLW_SELECTPOS) {
            return xDox_HandleEvent(&me->dox, EVT_KEY, AVK_SELECT, NULL);

         } else if(wParam == PROP_HTMLW_LINK_COLOR) {
            xDox_SetLinkColor(&me->dox, dwParam);
            return TRUE;

         } else if(wParam == PROP_HTMLW_DOC_BGCOLOR) {
            xDox_SetDocBGColor(&me->dox, dwParam);
            return TRUE;
	 }


      case EVT_WDG_GETPROPERTY:
         if (wParam == PROP_FLAGS) {
            *(uint32*)dwParam = Dox_GetFlags(&me->dox);
            return TRUE;

         } else if (wParam == PROP_HTMLW_INDENT) {
            *(int32*)dwParam = me->dox.cfg.xIndent;
            return TRUE;

         } else if (wParam == PROP_HTMLW_VSPACE) {
            *(int32*)dwParam = me->dox.cfg.ySpace;
            return TRUE;

         } else if (wParam == PROP_HTMLW_BUFFERSIZE) {
            *(uint32*)dwParam = xDox_GetBufferSize(&me->dox);
            return TRUE;

         } else if (wParam == PROP_HTMLW_FOCUSPOS) {
            *((int32*)dwParam) = xDox_GetFocusPos(&me->dox);
            return TRUE;

         } else if (wParam == PROP_HTMLW_SCROLLPOS) {
            *((int32*)dwParam) = xDox_GetScrollPos(&me->dox);
            return TRUE;

         } else if (wParam == PROP_HTMLW_SCROLLMLLINK) {
            *((boolean*)dwParam) = xDox_GetScrollMLLink(&me->dox);
            return TRUE;

         } else if(wParam == PROP_HTMLW_LINK_COLOR) {
            *(RGBVAL*)dwParam = xDox_GetLinkColor(&me->dox);
            return TRUE;

         } else if(wParam == PROP_HTMLW_DOC_BGCOLOR) {
            *(RGBVAL*)dwParam = xDox_GetDocBGColor(&me->dox);
            return TRUE;

         }
         break;   // EVT_WDG_GETPROPERTY
   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}


/////////////////////////////////////////////////////////////////////////////
// Widget creation and cleanup

static void HtmlWidget_Dtor(HtmlWidget *me)
{
   CALLBACK_Cancel(&me->cbkPeek);
   CALLBACK_Cancel(&me->cbkUpdate);
   LISTENER_Cancel(&me->ml);
   RELEASEIF(me->piShell);
   RELEASEIF(me->piFont);
   Border_Dtor(&me->border);
   WidgetBase_Dtor(&me->base);
   xDox_Dtor(&me->dox);
   DoxParse_Dtor(&me->doxparse);
}


static void HtmlWidget_Delete(HtmlWidget *me)
{
   HtmlWidget_Dtor(me);
   FREE(me);
}


static uint32 HtmlWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (me->base.nRefs == 1) {
      HtmlWidget_Dtor(me);
   }      
   return WidgetBase_Release(po);
}


int HtmlWidget_Construct(HtmlWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell,
                         IModule *piModule, PFNHANDLER pfnDefHandler)
{
   IDocModel *piDocModel = 0;
   IHtmlViewModel *piViewModel = 0;
   IFont *piFont = 0;
   int nErr = 0;

   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(HtmlWidget_HandleEvent));
   
#define VTFUNC(name) pvt->name = HtmlWidget_##name
   // vtable initialization
   VTFUNC( Release );
   VTFUNC( GetPreferredExtent );
   VTFUNC( SetExtent );
   VTFUNC( Draw );
   VTFUNC( IntersectOpaque );
   VTFUNC( SetModel );

#undef VTFUNC

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   nErr = ISHELL_CreateFont(piShell, AEECLSID_FONTSYSNORMAL, &piFont);
   
   if (!nErr) {
      HtmlWidget_UseFont(me, piFont);

      Border_Ctor(&me->border, piShell, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.extent, TRUE, NULL);
      Border_SetBorderColor(&me->border, 0, RGBA_BLACK);
      Border_SetBGColor(&me->border, 0, RGBA_NONE);
   }

   // create IDocModel
   if (!nErr) {
      nErr = DocModel_New(&piDocModel, piShell, piModule);
   }
   if (!nErr) {
      nErr = WidgetBase_SetModel((IWidget*)&me->base, (IModel*)piDocModel);
   }
   if (!nErr) {
      IMODEL_AddListenerEx((IModel*)piDocModel, &me->ml, (PFNLISTENER)HtmlWidget_Listener, me);
   }

   // create view model IHtmlViewModel
   if (!nErr) {
      nErr = HtmlViewModel_New(&piViewModel, piShell, piModule, me);
   }
   if (!nErr) {
      WidgetBase_SetViewModel(&me->base, (IModel*)piViewModel);
   }

   // construct the Dox object
   if (!nErr) {
      nErr = Dox_Construct(&me->dox, piShell, piViewModel, (IWidget*)me);
   }

   if (!nErr) {
      xDox_SetInvalCB(&me->dox, (DoxInvalFn)HtmlWidget_InvalidateCB, me);
      HtmlWidget_ResetDoc(me);
   }

   // set busy state to true
   me->isbusy = 1;

   RELEASEIF(piDocModel);
   RELEASEIF(piViewModel);
   RELEASEIF(piFont);
   return nErr;
}

int HtmlWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   int nErr = 0;
   HtmlWidget *me = MALLOCREC_VTBL(HtmlWidget, IWidget);

   *ppo = 0;

   if (!me) {
      return ENOMEMORY;
   }

   nErr = HtmlWidget_Construct(me, GETVTBL(me, IWidget), piShell, piModule, NULL);

   if (!nErr) {
      *ppo = (IWidget*)me;
   } else {
      HtmlWidget_Delete(me);
   }

   return nErr;
}



/*
/////////////////////////////////////////////////////////////////////////////

Development Notes:


API Translation

   IHtmlViewer API      HtmlWidget+Model(s)
   ---------------      -------------------
   HandleEvent          IWIDGET_HandleEvent
   Redraw               IWIDGET_Draw
   SetActive            IWIDGET_MoveFocus (on container)
   IsActive             IWIDGET_HasFocus
   SetRect              IWIDGET_SetExtent
   GetRect              IWIDGET_GetExtent or ICONTAINER_Locate
   SetProperties        IWIDGET_SetProperty(PROP_FLAGS)
   GetProperties        IWIDGET_GetProperty(PROP_FLAGS)
   SetOption            IWIDGET_SetProperty( {NEW_PROPS} )
   GetOption            IWIDGET_GetProperty( {NEW_PROPS} )
   LoadStream           - not supported -
   LoadSource           IDocModel_LoadSource
   SetNotifyFn          not needed -- use IHtmlViewModel listener instead
   Reset                IDocModel_Reset
   SetData              IDocModel_Write (new semantics: append data to document)
   - none -             IDocModel_Writeable (new method)
   - none -             IDocModel_WriteTerminate (for new Write semantics)
   IsBusy               IHtmlViewModel_IsBusy
   Stop                 IHtmlViewModel_Stop
   FindElem             IHtmlViewModel_FindElem
   GetElemText          IHtmlViewModel_GetElemText
   GetAttrValue         IHtmlViewModel_GetAttrValue
   SetViewState         IHtmlViewModel_RestoreState
   GetViewState         IHtmlViewModel_SaveState



IDocModel
   o Description
      - Data model for HtmlWidget. 
      - Has method(s) for html viewer/parser to get data (GetSource, then Read/Readable) and
        methods for clients to feed data in (SetSource,ParseData,etc.), and methods to
        traverse and interrogate the parsed document (FindElem,GetElemText,GetAttrValue)
      - Note: The IDocModel is created by the widget at creation time, and is inextricably 
        bound to the widget, e.g., can not be replaced by SetModel().  On creation, the 
        widget calls the private method DocModel_Peekable() which schedules a callback
        that will be called when data becomes available.
   o Methods
      - LoadSource
      - Write
      - Writeable
      - Reset
      - Stop
   o Notifications
      - EVT_HDM_RESET
      - EVT_HDM_STOP

IHtmlViewModel
   o Description
      - Widget's view model -- acquired by GetViewModel
      - Used to save and restore the viewer's state (Get/SetViewState). 
      - Sends notfications to listener(s) (HVN_DONE,HVN_JUMP,etc.)
   o Methods
      - IsBusy
      - FindElem
      - GetElemText
      - GetAttrValue
      - RestoreState
      - SaveState
   o Notifications
      - EVT_HVM_DONE
      - EVT_HVM_PAGEDONE
      - EVT_HVM_CONTENTDONE
      - EVT_HVM_JUMP
      - EVT_HVM_SUBMIT
      - EVT_HVM_FOCUS

*/
/////////////////////////////////////////////////////////////////////////////



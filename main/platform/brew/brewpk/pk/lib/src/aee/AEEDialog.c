/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEEDialog.c

SERVICES:  AEE Dialog Interface

GENERAL DESCRIPTION:  Provides support for IDialog interfaces in AEE.

PUBLIC CLASSES AND STATIC FUNCTIONS: IDialog, IStatic

Copyright ?1999-2007 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/

#include "OEMConfig.h"
#include "AEE_OEM.h"
#include "AEE_OEMEvent.h"
#include "AEEStdLib.h"
//#include "AEEDialog_priv.h"
//#include "AEE_MIF.h"
#include "AEEText.h"
#include "AEEPointerHelpers.h"
#ifdef FEATURE_RANDOM_MENU_COLOR
#include "Appscommon.h"
#include "AEEAnnunciator.h"
#endif
/*===========================================================================

Class DEFINITIONS

===========================================================================*/
OBJECT(DlgControl)
{
    DlgControl   * pNext;
    IControl     * pc;
    uint16         wID;                 // control ID in dialog
};
typedef struct _ADialog          ADialog;
struct _ADialog
{
   DECLARE_VTBL(IDialog)

   uint16         m_wDialogID;
   uint32         m_dwProps;
   flg            m_bReady:1;
   flg            m_bPrompt:1;
   flg            m_bCopyright:1;
   flg            m_bClosing:1;
   AEERect        m_rc;
   uint32         m_nRefs;
   ADialog *      m_pNext;
   IShell *       m_pShell;   
   void     *     m_paOwner;
   DlgControl *   m_pControls;
   DlgControl *   m_pFocus;
   uint16         m_wDefCommand;
   uint16         m_ySKFrame;
   PFNAEEEVENT    m_pfnEvent;
   void *         m_pUserEvent;
   IDisplay *     m_pDisplay;
   uint32         m_cxScreen;
   uint32         m_cyScreen;
   IStatic      * m_pPrmpStatic;
   uint32         m_dwStaticProps;
   boolean       m_bKeyIsReady;
   
};

// Dialog Routines

static uint32           ADialog_AddRef(IDialog * po);
static uint32           ADialog_Release(IDialog * po);
static int16            ADialog_SetFocus(IDialog * po, int16 wID);
static IControl *       ADialog_GetControl(IDialog * po, int16 wID);
static uint16           ADialog_GetID(IDialog * po);
static void             ADialog_SetEventHandler(IDialog * pme, PFNAEEEVENT pfn, void * pUser);
static void             ADialog_Redraw(IDialog * po);
static void             ADialog_GetSize(ADialog * pme, uint32 dwProps, AEERect * pr);
static DlgControl *     ADialog_FindControl(ADialog * pme, int16 wID);
static boolean          ADialog_SendEvent(ADialog * pme, AEEEvent evt, uint16 wp, uint32 dwp);
static RGBVAL           ADialog_GetSysColor(ADialog * pme, RGBVAL c);
static void             ADialog_Timeout(ADialog * pme);
static void             ADialog_Dismiss(ADialog * pme, uint16 wParam);
static boolean          ADialog_ShowCopyright(ADialog * pme);
static int              ADialog_AdjustSoftkeyMenu(ADialog * pme, IControl * psk, AEERect * pr, boolean bFrame);
static void             ADialog_SetProperties(IDialog *po, uint32 props);
static uint32           ADialog_GetProperties(IDialog *po);
static void             AdjustControlRect(IControl * pc, AEERect * prc, AEERect * prcd);
static void             FreePtr(void ** pp);
static AECHAR *         LoadDynamicText(IShell * po, const char * pszRes, uint16 wID, boolean bTitle, boolean * bResExist);

static const VTBL(IDialog) gDialogMethods = {ADialog_AddRef,
                                             ADialog_Release,
                                             ADialog_SetFocus,
                                             ADialog_GetControl,
                                             ADialog_SetEventHandler,
                                             ADialog_GetID,
                                             ADialog_SetProperties,
                                             ADialog_GetProperties,
                                             ADialog_Redraw};

/*===========================================================================

DEFINITIONS

===========================================================================*/

#define HAS_BORDER(p)            (boolean)(((p)->m_dwProps & (CP_BORDER|CP_3D_BORDER)) ? TRUE : FALSE)
#define PROMPT_TEXT_ALLOC        (128)
#define PROMPT_TITLE_ALLOC       (64)

#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

// Copyright Text
typedef struct _CRText
{
   uint16         wID;
   const char  *  pszRes;
   AECHAR         sz[24];
   int            cx;
}CRText;


/*===========================================================================

EXTERNAL - Entry Point for Dialog Support. Supports creation of Dialog class.

===========================================================================*/
int ADialog_New(IShell * ps,AEECLSID cls, void **ppif)
{
   void *pae = AEE_GetAppContext();
   ADialog *   pme;
   IBitmap *      pbmDst;
   AEEBitmapInfo  bi;
   int         nErr;

   if(!pae)
      pae = AEE_GetStackedAppContext();

   pme = (ADialog *)AEE_NewClass((IBaseVtbl *)&gDialogMethods, sizeof(ADialog));
   
   *ppif = pme;

   if (NULL == pme) {
      return ENOMEMORY;
   }

   nErr = ISHELL_CreateInstance(ps, AEECLSID_DISPLAY, (void **) &pme->m_pDisplay);
   if (SUCCESS != nErr) {
      FREE(pme);
      *ppif = NULL;
      return nErr;
   }

#ifdef FEATURE_RANDOM_MENU_COLOR
    {
        byte nRandomMenu = 0;
        
        (void)OEM_GetConfig(CFGI_RANDOM_MENU, (void*)&nRandomMenu, sizeof(nRandomMenu));
        if(nRandomMenu == 2)
        {
            RGBVAL nColor;
            
            nColor = Appscommon_GetRandomColor(110);
            (void)OEM_SetConfig(CFGI_MENU_BGCOLOR, (void*)&nColor, sizeof(nColor));
        }
    }
#endif
   pbmDst = IDISPLAY_GetDestination(pme->m_pDisplay);
   IBITMAP_GetInfo(pbmDst, &bi, sizeof(bi));
   IBITMAP_Release(pbmDst);
   pme->m_cxScreen = bi.cx;
   pme->m_cyScreen = bi.cy;

   // Stack currently active dialog.
   pme->m_pNext      = (ADialog *)AEE_GetAppContextDlg(pae);
 
   // Ensure that its control in focus is set inactive
   if (pme->m_pNext && pme->m_pNext->m_pFocus)
      ICONTROL_SetActive(pme->m_pNext->m_pFocus->pc, FALSE);

   // Make new dialog active dialog
   AEE_SetAppContextDlg(pae,(IDialog *)pme);
   pme->m_pShell              = ps;
   pme->m_paOwner             = pae;
#ifndef CUST_EDITION
   pme->m_cPenEvent.pHitCtl   = NULL;
   pme->m_cPenEvent.wID       = 0;
#endif   
   pme->m_bKeyIsReady = FALSE;
   return SUCCESS;
}

/*===========================================================================

Interface Method: Increments Reference Count for IDialog

===========================================================================*/
static uint32 ADialog_AddRef(IDialog * po)
{
   return((po ? ++((ADialog *)po)->m_nRefs : 0));
}


/*===========================================================================

Interface Method: Decrements Reference Count for IDialog. Does clean up when ref count
reaches 0

===========================================================================*/
static uint32 ADialog_Release(IDialog * po)
{
   ADialog *      pme = (ADialog *)po;
   ADialog *      pdPrev;
   ADialog *      pd;
   ADialog *      pdNext;
   DlgControl  *  pdc;
   DlgControl *   pdcNext;
   void     *     pae;

   if(pme->m_nRefs > 0){

      if(--pme->m_nRefs)
         return(pme->m_nRefs);

   // Public reference count is 0 BUT the internal reference count is still high.  We 
   // must not destroy the object!

      if(pme->m_bClosing)
         return(0);

   // Cancel all pending operations...
   
      ISHELL_CancelTimer(pme->m_pShell, NULL, po);

   // Find it in the list of dialogs managed for this app entry...

      pae = pme->m_paOwner;

        for(pdPrev = NULL, pd = (ADialog *)AEE_GetAppContextDlg(pae); pd; pd = pd->m_pNext){
         if(pd == pme)
            break;
         pdPrev = pd;
      }

      pdNext = pme->m_pNext;
   
      if(pdPrev)
         pdPrev->m_pNext = pdNext;
      else
         AEE_SetAppContextDlg(pae,(IDialog *)pdNext);

   // Notify the owner...
      if(!pme->m_bPrompt){
         if(pme->m_bReady){
            ADialog_SendEvent(pme, (AEEEvent)(pme->m_bCopyright ? EVT_COPYRIGHT_END : EVT_DIALOG_END), (uint16)pme->m_wDialogID, (uint32)pme);
         }

   // In case the user created another during the callback!
         pdNext = (ADialog *)AEE_GetAppContextDlg(pae);
      }

   // Release all of the controls...

      for(pdc = pme->m_pControls; pdc; pdc = pdcNext){
         pdcNext = pdc->pNext;
         if(pdc->pc)
            ICONTROL_Release(pdc->pc);
         FREE(pdc);
      }

      IDISPLAY_Release(pme->m_pDisplay);

      FREE(pme);
      
      if(pdNext && !(((ADialog *)pdNext)->m_dwProps & DLG_NOT_REDRAW_AFTER_START))
         ADialog_Redraw((IDialog *)pdNext);

   }
   return(0);
}

/*===========================================================================

Internal Method: Sends Event to the owner

===========================================================================*/
static boolean ADialog_CallEventHandler(ADialog * pme, AEEEvent evt, uint16 wp, uint32 dwp)
{
   boolean     bRet;
   void *      pac;

   pac = AEE_EnterAppContext(pme->m_paOwner);
   bRet = pme->m_pfnEvent(pme->m_pUserEvent, evt, wp, dwp);
   AEE_LeaveAppContext(pac);
   return(bRet);
}

/*===========================================================================

Internal Method: Sends Event to the dialog or the owner

===========================================================================*/
static boolean ADialog_SendEvent(ADialog * pme, AEEEvent evt, uint16 wp, uint32 dwp)
{
   if(pme->m_pfnEvent)
      return(ADialog_CallEventHandler(pme,evt,wp,dwp));

   if(pme->m_paOwner)
      return(AEE_AppCtxtSendEvent(pme->m_paOwner, evt, wp, dwp));

   return(FALSE);
}

/*===========================================================================

Internal Method: Gets System Color associated with given item

===========================================================================*/
static RGBVAL ADialog_GetSysColor(ADialog * pme, RGBVAL c)
{
   // Load the system color into a user entry...
   // Save the old value (real frame color)
   // Reset the old value and return the color of the system color...

   c = IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_FRAME, c);
   return(IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_FRAME, c));
}

/*===========================================================================

Internal Method: Creates and Loads the Dialog

===========================================================================*/
int ADialog_Load(IDialog * po, const char * pszResFile, uint16 wID, DialogInfo * pInfo)
{
   IShell *       ps;
   ADialog *      pme = (ADialog *)po;
   DlgControl *   pdc;
   DListItem  *   pii = NULL;       // Init to prevent ARM warning...
   DialogInfoHead *pih = NULL;      // Init to prevent ARM warning...
   DialogItemHead *pdih = NULL;     // Init to prevent ARM warning...
   DialogInfoRes  *pInfoRes = NULL; // Init to prevent ARM warning...
   int            i, nItem;
   AEERect        rc,rcd;
   CtlAddItem     cai;
   IControl *     psk;
   int            ySK;
   boolean        bLoadFromRes = (pInfo == NULL);
   void       *     pae;

	MEMSET(&cai, 0, sizeof(CtlAddItem));

   ps = pme->m_pShell;

   if(!pInfo){
      pInfoRes = (DialogInfoRes *)ISHELL_LoadResData(ps, pszResFile, wID, RESTYPE_DIALOG);
      if(!pInfoRes)
         goto abort;

#if defined(FEATURE_BIG_ENDIAN)
      LETOHS_D( pInfoRes->h.wID );
      LETOHS_D( pInfoRes->h.nControls );
      LETOHS_D( pInfoRes->h.rc.x );
      LETOHS_D( pInfoRes->h.rc.y );
      LETOHS_D( pInfoRes->h.rc.dx );
      LETOHS_D( pInfoRes->h.rc.dy );
      LETOHL_D( pInfoRes->h.dwProps );
      LETOHS_D( pInfoRes->h.wTitle );
      LETOHS_D( pInfoRes->h.wFocusID );
      {
        uint32 idx;
        for( idx = 0; idx < pInfoRes->h.nControls; idx++ ) {
          LETOHS_D( pInfoRes->wControlIDs[idx] );
        }
      }
#endif

      pih = &pInfoRes->h;
   }
   else
      pih = &pInfo->h;

   pae = pme->m_paOwner;

   rcd = pih->rc;
   ADialog_GetSize(pme, pih->dwProps, &rcd);

   // Create the controls.

   psk = NULL;
   ySK = 0;

   pme->m_wDialogID = pih->wID;

   if(!bLoadFromRes)
      pdih = (DialogItemHead *)(pInfo->controls);

   for(i = 0; i < pih->nControls; i++){
      if(bLoadFromRes){
         pdih = (DialogItemHead *) ISHELL_LoadResData(ps, pszResFile, pInfoRes->wControlIDs[i], RESTYPE_CONTROL);
         if(!pdih)
            continue;

         LETOHL_D( pdih->cls );
         LETOHS_D( pdih->wID );
         LETOHS_D( pdih->nItems );
         LETOHL_D( pdih->dwProps );
         LETOHS_D( pdih->wTextID );
         LETOHS_D( pdih->wTitleID );
         LETOHS_D( pdih->rc.x );
         LETOHS_D( pdih->rc.y );
         LETOHS_D( pdih->rc.dx );
         LETOHS_D( pdih->rc.dy );
      }
   
      pdc = (DlgControl *)MALLOC(sizeof(DlgControl));

      if(pdc){
         if(!ISHELL_CreateInstance(ps, pdih->cls, (void **)(&pdc->pc))){
            pdc->pNext = pme->m_pControls;
            pdc->wID = pdih->wID;
            pme->m_pControls = pdc;

            if(pdih->cls == AEECLSID_SOFTKEYCTL || pdih->cls == AEECLSID_SOFTKEYCTL_10)
               psk = pdc->pc;

            if(pdih->dwProps)
               ICONTROL_SetProperties(pdc->pc, pdih->dwProps);
         }
         else
            FREE(pdc);
      }

      if(!bLoadFromRes){
         if(pdih->nItems)
            pdih = (DialogItemHead *)((byte *)pdih + sizeof(DialogItem) + (sizeof(DListItem) * (pdih->nItems - 1)));
         else
            pdih = (DialogItemHead *)((byte *)pdih + sizeof(DialogItem) - sizeof(DListItem));
      }
      else
         ISHELL_FreeResData(ps, pdih);
   }

   pdc = ADialog_FindControl(pme,pih->wFocusID);
   if(!pdc)
      pdc = pme->m_pControls;
   pme->m_pFocus = pdc;

   // Do not prevent startup if they did not respond to one posted to a message box...

   if(pae && !ADialog_SendEvent(pme,EVT_DIALOG_INIT, pih->wID, (uint32)pme))
      goto abort;

   ySK = ADialog_AdjustSoftkeyMenu(pme, psk, &rcd,HAS_BORDER(pme));

   // Position the controls and fill them...

   if(!bLoadFromRes)
      pdih = (DialogItemHead *)(pInfo->controls);

   for(i = 0; i < pih->nControls; i++){

   // If we are loading from a resource, then read the control information into
   // the automatic variable and point to it...

      if (bLoadFromRes){
         pdih = (DialogItemHead *) ISHELL_LoadResData(ps, pszResFile, ((DialogInfoRes *)pih)->wControlIDs[i], RESTYPE_CONTROL);
         if(!pdih)
            continue;

         LETOHL_D( pdih->cls );
         LETOHS_D( pdih->wID );
         LETOHS_D( pdih->nItems );
         LETOHL_D( pdih->dwProps );
         LETOHS_D( pdih->wTextID );
         LETOHS_D( pdih->wTitleID );
         LETOHS_D( pdih->rc.x );
         LETOHS_D( pdih->rc.y );
         LETOHS_D( pdih->rc.dx );
         LETOHS_D( pdih->rc.dy );

#if defined(FEATURE_BIG_ENDIAN)
         {
            uint32 idx;
            DialogItemRes * pDialogItemRes = (DialogItemRes *)pdih;

            for( idx = 0; idx < pdih->nItems; idx++ ) {
               LETOHS_D( pDialogItemRes->wItemIDs[idx] );
           }
         }
#endif

      }

      pdc = ADialog_FindControl(pme, pdih->wID);

      if(!pdc){
         if(bLoadFromRes)
            ISHELL_FreeResData(ps, pdih);
         continue;
      }

      ICONTROL_GetRect(pdc->pc, &rc);

   // If any of the coordinates/sizes are specified in the resource, use them...

      if(pdih->rc.x != -1 && pdih->rc.y != -1)
         rc = pdih->rc;

   // Alter the x,y by the position of the dialog...

      AdjustControlRect(pdc->pc, &rc, &rcd);

   // Special case, if the control overlaps the SK bar, them move it...

      if(pdih->rc.x == -1 && pdih->rc.y == -1){
         if(ySK && pdih->cls != AEECLSID_SOFTKEYCTL && pdih->cls != AEECLSID_SOFTKEYCTL_10 && rc.y + rc.dy >= ySK)
            rc.dy = ySK - rc.y;
      }
      ICONTROL_SetRect(pdc->pc, &rc);

   // See if it's a text control.  If so, we may need to tell it about the softkey control..

      if(pdih->cls == AEECLSID_TEXTCTL && psk)
         ITEXTCTL_SetSoftKeyMenu((ITextCtl *)pdc->pc, (IMenuCtl *)psk);
         
   // See if it has a title.  If so, load and set it...

      if(pdih->wTitleID != 0xffff)
         ICONTROL_HandleEvent(pdc->pc, EVT_CTL_SET_TITLE, pdih->wTitleID, (uint32)pszResFile);

      if(pdih->wTextID != 0xffff)
         ICONTROL_HandleEvent(pdc->pc, EVT_CTL_SET_TEXT, pdih->wTextID, (uint32)pszResFile);

      cai.pszResText = (char *)pszResFile;

      // for(pii = pdih->items, nItem = 0; nItem < pdih->nItems; nItem++, pii++){
      
      if(!bLoadFromRes)
         pii = ((DialogItem *)pdih)->items;

      for (nItem = 0; nItem < pdih->nItems; nItem++){
         if (bLoadFromRes){
            DListItem  dmi;

            {
               void   *p  = &dmi;
               uint32  dw = sizeof(dmi);

               AEE_GetResData(pszResFile, ((DialogItemRes *)pdih)->wItemIDs[nItem], RESTYPE_LISTITEM, &p, &dw);
            }

            LETOHS_D( dmi.wID );
            LETOHS_D( dmi.wTextID );
            LETOHS_D( dmi.wIconID );
            LETOHS_D( dmi.pad );
            LETOHL_D( dmi.dwData );

            pii = &dmi;
         }

         cai.wImage = pii->wIconID;
         cai.wItemID = pii->wID;
         cai.wText = pii->wTextID;
         cai.dwData = pii->dwData;
         ICONTROL_HandleEvent(pdc->pc, EVT_CTL_ADD_ITEM, 0, (uint32)&cai);

         if (!bLoadFromRes)
            pii++;
      }

      if (!bLoadFromRes){
         if(pdih->nItems)
            pdih = (DialogItemHead *)((byte *)pdih + sizeof(DialogItem) + (sizeof(DListItem) * (pdih->nItems - 1)));
         else
            pdih = (DialogItemHead *)((byte *)pdih + sizeof(DialogItem) - sizeof(DListItem));
      }
      else
         ISHELL_FreeResData(ps, pdih);
   }

   if(pae && !ADialog_SendEvent(pme, EVT_DIALOG_START, pih->wID, (uint32)pme)){
abort:
      ADialog_Release(po);

      if(bLoadFromRes){
         ISHELL_FreeResData(ps, pInfoRes);
      }
      return(EFAILED);
   }

   pme->m_bReady = TRUE;
   
   if(!(pme->m_dwProps & DLG_NOT_REDRAW_AFTER_START))
   {
      ADialog_Redraw(po);
   }

   if(bLoadFromRes){
      ISHELL_FreeResData(ps, pInfoRes);
   }
   return(0);
}


/*===========================================================================

Internal Method: Event Handler for the Dialog. Passes event to appropriate control

===========================================================================*/
boolean ADialog_HandleEvent(IDialog * po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ADialog *      pme = (ADialog *)po;
   DlgControl *   pdc = NULL;
   DlgControl *   pdcNew;

   if(pme->m_bReady){

   if(!pme->m_bKeyIsReady)
   {
       if(evt == EVT_KEY || evt == EVT_KEY_RELEASE)
       {
           return TRUE;
       }
       else if(evt == EVT_KEY_PRESS)
       {
           pme->m_bKeyIsReady = TRUE;
       }
   }

   // Prompt dailogs are special - they eat all events...

      if(pme->m_bPrompt){
         if(evt == EVT_COMMAND || (evt == EVT_KEY && wParam == AEE_GetAppCloseKey())){
            boolean bRet;

            //bRet = (pme->m_paOwner && pme->m_paOwner->pia);
            bRet = (pme->m_paOwner && AEE_GetAppContextApplet(pme->m_paOwner));
            ADialog_Dismiss(pme,(uint16)(evt == EVT_COMMAND ? wParam : pme->m_wDefCommand));
            return(bRet);
         }

   // Copyright dialog is dismissed with any key...

         if(evt == EVT_KEY && pme->m_bCopyright){
            ADialog_Timeout(pme);
            return(TRUE);
         }
      }

      // Is there a need to switch controls focus?
      if( evt == EVT_POINTER_STALE_MOVE || evt == EVT_POINTER_MOVE || evt == EVT_POINTER_DOWN || evt == EVT_POINTER_UP ){
         AEERect     rctGeneral;
         IControl *  pcF   = NULL;

         if( evt == EVT_POINTER_DOWN ){
            int16 wXHit = (int16)AEE_POINTER_GET_X((const char *)dwParam);
            int16 wYHit = (int16)AEE_POINTER_GET_Y((const char *)dwParam);
#ifdef FEATURE_LCD_TOUCH_ENABLE  //add by ydc
			if ((wXHit>118)&&(wXHit<=176)&&(wYHit>184)&&(wYHit<202))
			{
				evt = EVT_KEY;
				wParam = AVK_CLR;
				for(pdc = pme->m_pControls; pdc != NULL ; pdc = pdc->pNext){
					if(ICONTROL_HandleEvent(pdc->pc, evt, wParam, dwParam))
						return(TRUE);
				}
			}
			else if ((wXHit>136)&&(wXHit<=176)&&(wYHit>=0)&&(wYHit<20))
			{
				evt = EVT_KEY_RELEASE;
				wParam = AVK_POUND;
				for(pdc = pme->m_pControls; pdc != NULL ; pdc = pdc->pNext){
					if(ICONTROL_HandleEvent(pdc->pc, evt, wParam, dwParam))
						return(TRUE);
				}
			}
#endif       //add by ydc

            // First see if it is in the Dialog itself
            if( PT_IN_RECT(wXHit, wYHit, pme->m_rc) ){
               IControl * pHandler = NULL;
               // Now check the focus control's rect
               // This sort of handles overlapped controls by giving
               // the focus control first crack at it
               if( pme->m_pFocus ){
                  if( ((pcF = pme->m_pFocus->pc) != NULL) ){
                     ICONTROL_GetRect(pcF, &rctGeneral);
                  }
               }
               // If it is in the active control, just give the event to them
               // If handled, try to notify the Dialog handler
               // Apps distinguish handled vs. not handled via
               // Dlg Handler Event CB [Handled] or App Event [not handled]
               if( pcF && PT_IN_RECT(wXHit, wYHit, rctGeneral) ){
                  pHandler = pcF;
               }else{
                  // Otherwise, find if it is in a control we own
                  for(pdc = pme->m_pControls; pdc != NULL ;pdc = pdc->pNext){
                     if(pdc == pme->m_pFocus)
                        continue;
                     ICONTROL_GetRect(pdc->pc, &rctGeneral);
                     if( PT_IN_RECT(wXHit, wYHit, rctGeneral) ){
                        // This gives the first control ID precedence
                        // if the spot was an overlapped control
                        // If they are overlapped, we don't have a way to query 
                        // who was last drawn, so give preference to first control
                        ADialog_SetFocus(po, pdc->wID);
                        if( pme->m_pFocus && pme->m_pFocus->wID == pdc->wID ){
                           pHandler = pme->m_pFocus->pc;
                        }
                     }
                  }
               }
               if( pHandler ){
                  if( ICONTROL_HandleEvent(pHandler, evt, wParam, dwParam) ){
                     if( (pme->m_dwProps & DLG_HANDLE_ALL_EVENTS) && pme->m_pfnEvent ) {
                        ADialog_CallEventHandler(pme,evt, wParam, dwParam);
                     }
                     return TRUE;
                  }
               }
            }
            return FALSE;
         }else{
            if( pme->m_pFocus ){
               pcF = pme->m_pFocus->pc;
               if( pcF ){
                  if( ICONTROL_HandleEvent(pcF, evt, wParam, dwParam) ){
                     if( (pme->m_dwProps & DLG_HANDLE_ALL_EVENTS) && pme->m_pfnEvent ) {
                        ADialog_CallEventHandler(pme,evt, wParam, dwParam);
                     }
                     return TRUE;
                  }
               }
            }
         }
         return FALSE;
      }
   // There is no control to send the event to...
         
      if(!pme->m_pFocus)
         return(FALSE);

   // Handle tabbing between controls...


      if(evt == EVT_CTL_TAB)
      {
        if(pme->m_dwProps & DLG_NOT_SET_FOCUS_AUTO)
        {
            return(FALSE);
        }
         pdcNew = NULL;

   // If we are moving forward, look for the next.  Assumes the list is in order of left-right, top-bottom...

         if(wParam){
            pdcNew = pme->m_pFocus->pNext;
            if(!pdcNew)
               pdcNew = pme->m_pControls;
         }
         else{
            for(pdc = pme->m_pControls; pdc != NULL ;pdc = pdc->pNext){
               if(pdc == pme->m_pFocus)
                  break;
               pdcNew = pdc;
            }
            if(!pdcNew)
               for(pdcNew = pme->m_pControls; pdcNew != NULL && pdcNew->pNext != NULL ; pdcNew = pdcNew->pNext);
         }
         //if(pdcNew){
         if ((pdcNew) && pdcNew->pc != (IControl *)dwParam)
         {// 只有发出该事件的控件与新控件不同时才需重新设置焦点
            ADialog_SetFocus(po, pdcNew->wID);
            return(TRUE);
         }
         return(FALSE);
      }

   // Send the event to all of the controls...

      for(pdc = pme->m_pControls; pdc != NULL ; pdc = pdc->pNext){
         if(ICONTROL_HandleEvent(pdc->pc, evt, wParam, dwParam))
            return(TRUE);
      }

      //check if we need to pass all un-handled events to the registered
      //event handler 
      if(pme->m_dwProps & DLG_HANDLE_ALL_EVENTS) {
         if(pme->m_pfnEvent)
            return(ADialog_CallEventHandler(pme,evt, wParam, dwParam));
      }

   }
   return(FALSE);
}

/*===========================================================================

Interface Method: Set Focus to the given Control

===========================================================================*/
static int16 ADialog_SetFocus(IDialog * po, int16 wID)
{
   ADialog *      pme = (ADialog *)po;
   DlgControl  *  pFocus;
   uint16         wIDLast;

   pFocus = ADialog_FindControl(pme, wID);

   wIDLast = (pme->m_pFocus ? pme->m_pFocus->wID : 0);

   if(pFocus && wIDLast != wID){

      if(wIDLast)
         ICONTROL_SetActive(pme->m_pFocus->pc, FALSE);

      ICONTROL_SetActive(pFocus->pc, TRUE);
   
      if(ICONTROL_IsActive(pFocus->pc))
         pme->m_pFocus = pFocus;
      else{
         if(pme->m_pFocus)
            ICONTROL_SetActive(pme->m_pFocus->pc,TRUE);
      }
   }
   return(wIDLast);
}

/*===========================================================================

Interface Method: Returns ptr to the control with given ID

===========================================================================*/
static IControl * ADialog_GetControl(IDialog * po, int16 wID)
{
   ADialog *      pme = (ADialog *)po;
   DlgControl  *  pFocus;

   pFocus = ADialog_FindControl(pme, wID);
   if(pFocus)
      return(pFocus->pc);
   return(NULL);
}

/*===========================================================================

InterFace Method: Returns ID of the Dialog

===========================================================================*/
static uint16 ADialog_GetID(IDialog * po)
{
   return(((ADialog *)po)->m_wDialogID);
}

/*===========================================================================

Interface Method: Sets the event handler for the Dialog

===========================================================================*/
static void ADialog_SetEventHandler(IDialog * po, PFNAEEEVENT pfn, void * pUser)
{
   ADialog *      pme = (ADialog *)po;
   
   pme->m_pfnEvent      = pfn;
   pme->m_pUserEvent    = pUser;
}

/*===========================================================================

Interface Method: Sets properties of the Dialog

===========================================================================*/
static   void  ADialog_SetProperties(IDialog *po, uint32 dwProps)
{
   ADialog *pme = (ADialog *)po;
   pme->m_dwProps |= dwProps;
}

/*===========================================================================

Interface Method: Gets Properties of the Dialog

===========================================================================*/
static uint32   ADialog_GetProperties(IDialog *po)
{
   return(((ADialog *)po)->m_dwProps);
}

/*===========================================================================

Internal Method: Helps to locate control with given ID

===========================================================================*/
static DlgControl * ADialog_FindControl(ADialog * pme, int16 wID)
{
   DlgControl * pdc;

   for(pdc = pme->m_pControls; pdc ; pdc = pdc->pNext){
      if(pdc->wID == (uint16)wID)
         break;
   }
   return(pdc);
}

/*===========================================================================

Internal Method: Redraws the dialog and all its controls

===========================================================================*/
static void ADialog_Redraw(IDialog * po)
{
   ADialog *         pme = (ADialog *)po;
   DlgControl *      pdc;
   IDisplay    *     pd;
   AEERect           rc;

   rc = pme->m_rc;
   pd = pme->m_pDisplay;
   if(!(pme->m_dwProps & DLG_NOT_CLEARSCREEN_ONREDRAW))
   {
    IDISPLAY_FillRect(pd, &rc, CLR_SYS_WIN);
   }

   if(pme->m_bCopyright)
      ADialog_ShowCopyright(pme);
   else{
#ifdef FEATURE_RANDOM_MENU_COLOR
            byte nRandomMenu = 0;
            
            (void)OEM_GetConfig(CFGI_RANDOM_MENU, (void*)&nRandomMenu, sizeof(nRandomMenu));
            if(nRandomMenu == 2)
            {
                IAnnunciator *pIAnn = NULL;
                
                if (AEE_SUCCESS == ISHELL_CreateInstance(pme->m_pShell,AEECLSID_ANNUNCIATOR,(void **)&pIAnn))
                {
                    IANNUNCIATOR_Redraw(pIAnn);
                    IANNUNCIATOR_Release(pIAnn);
                    pIAnn = NULL;
                }
            }
#endif
      if(HAS_BORDER(pme))
         IDISPLAY_DrawFrame(pd, &rc, (AEEFrameType)(pme->m_dwProps & CP_3D_BORDER ? AEE_FT_RAISED : AEE_FT_BOX),CLR_SYS_WIN);

      rc.y = pme->m_ySKFrame;
      if(rc.y){
         rc.dy = 1;
         IDISPLAY_FillRect(pd, &rc, CLR_SYS_FRAME_LO);
      }

      for(pdc = pme->m_pControls; pdc != NULL; pdc = pdc->pNext){
         ICONTROL_SetActive(pdc->pc, (boolean)(pdc == pme->m_pFocus));
         ICONTROL_Redraw(pdc->pc);
      }
	  #ifdef FEATURE_VERSION_X3
	  if(pme->m_pControls == NULL)
   	  {
		 return;
      }
	  #endif
      if(!(pme->m_dwProps & DLG_NOT_UPDATE_ONREDRAW))
      {
          IDISPLAY_Update(pd);
      }
   }
}

/*===========================================================================

Internal Method: Loads text from the given resource file

===========================================================================*/
static AECHAR * LoadDynamicText(IShell * po, const char * pszRes, uint16 wID, boolean bTitle, boolean * bResExist)
{
   AECHAR   * pTemp;
   AECHAR   * pText = NULL;
   uint32     nResSize;
   uint16     wBuffSize = ((uint16)(bTitle ? PROMPT_TITLE_ALLOC : PROMPT_TEXT_ALLOC)+1)*sizeof(AECHAR);

   // BACKWARD COMPATIBILITY NOTES
   // Check if the resource exists, if it doesn't then the dialog 
   // needs to ignore it and show nothing in this field.
   // If the resource exists and the resource is too large, then 
   // the dialog needs to fail and not be created.
   // Note that if the allocation fails, bResExist is still set to 
   // TRUE and a failure will occur.
   if(!ISHELL_GetResSize(po, pszRes, wID, RESTYPE_STRING, &nResSize)){
      if(bResExist){
         *bResExist = FALSE;
      }
   }else{
      if(bResExist){
         *bResExist = TRUE;
      }
      if(nResSize <= wBuffSize){
         pTemp = (AECHAR *)MALLOC(wBuffSize);
         if(pTemp){
            if(ISHELL_LoadResString(po, pszRes, wID, pTemp, wBuffSize))
               pText = WSTRDUP(pTemp);
            FREE(pTemp);
         }
      }
   }
   return(pText);
}

/*===========================================================================

Internal Method: De-acts controls on a dialog, so they don't step on each other.

===========================================================================*/
void ADialog_PromptEnableScrolling(IDialog * po, boolean bEnable)
{
   ADialog *      pme = (ADialog *)po;
   IStatic *      ps = pme->m_pPrmpStatic;

   if(pme->m_bPrompt && ps){
      if(bEnable){
         // Since there is no Resume Scrolling or function that says
         // Stop/Start scrolling such as this enables and disables
         // It can be tricked into start scrolling by doing a SetProperties
         // then a Redraw.
         if(!(pme->m_dwStaticProps & ST_NOSCROLL)){
            ISTATIC_SetProperties(ps, pme->m_dwStaticProps);
            ISTATIC_Redraw(ps);
         }
      }else{
         // Since the IStatic considers itself inactive
         // even though it is actively scrolling/drawing
         // on a timer, SetActive will not do much.
         // So, tell it to stop scrolling.
         if(!(pme->m_dwStaticProps & ST_NOSCROLL)){
            ISTATIC_SetProperties(ps, pme->m_dwStaticProps | ST_NOSCROLL);
         }
      }
   }
}

/*===========================================================================

Internal Method: Displays dialogs for prompts and copyrights

===========================================================================*/
boolean ADialog_Prompt(IDialog * po,AEEPromptInfo * pi)
{
   ADialog *      pme = (ADialog *)po;
   AEERect        rcd;
   DlgControl *   pdc;
   CtlAddItem     cai;
   uint16      *  pwb;
   AEECLSID       cls;
   boolean        bStatic = TRUE,
                  bTextResExist, bTitleResExist;

   // Special Case:  NULL Info structure displays copyright message...

   if(!pi){
      if(ADialog_ShowCopyright(pme))
         return(TRUE);
      ADialog_Release(po);
      return(FALSE);
   }

   // Nothing to display...

   if(!pi->pTitle && !pi->pText && (!pi->pszRes || (!pi->wTextID && !pi->wTitleID))){
      ADialog_Release(po);
      return(FALSE);
   }

   // See if we need to allocate and load the text...

   if(!pi->pText && pi->pszRes && pi->wTextID){
      pi->pText = LoadDynamicText(pme->m_pShell, pi->pszRes, pi->wTextID, FALSE, &bTextResExist);
      pi->dwProps |= ST_TEXTALLOC;
      if(!pi->pText && bTextResExist){
         ADialog_Release(po);
         return(FALSE);
      }
   }

   // See if we need to allocate and load the title..

   if(!pi->pTitle && pi->pszRes && pi->wTitleID){
      pi->pTitle = LoadDynamicText(pme->m_pShell, pi->pszRes, pi->wTitleID, TRUE, &bTitleResExist);
      pi->dwProps |= ST_TITLEALLOC; // This belongs here in case of failure with title
      if(!pi->pTitle && bTitleResExist){

   // Must free the text if we own it!
abort:
         if(pi->pTitle && (pi->dwProps & ST_TITLEALLOC))
            FREE((void *)pi->pTitle);

         if(pi->pText && (pi->dwProps & ST_TEXTALLOC))
            FREE((void *)pi->pText);

         ADialog_Release(po);
         return(FALSE);
      }
   }
   if(!pi->pTitle && !pi->pText){
      goto abort;
   }

   if(pi->dwProps & CP_USE_DEFAULT){

      AEEDeviceInfo di;

      di.wStructSize = sizeof(di);
      AEE_GetDeviceInfo(&di);

      pi->dwProps = (di.dwPromptProps & 0x0000ffffl) | (pi->dwProps & 0xffff0000l);
   }

   SETAEERECT(&rcd, -1, -1, -1, -1);
   ADialog_GetSize(pme, pi->dwProps, &rcd);
      
   if(pi->pBtnIDs){

      pme->m_wDefCommand = (uint16)(pi->wDefBtn ? pi->wDefBtn : pi->pBtnIDs[0]);

      pme->m_pControls = pdc = (DlgControl *)MALLOC(sizeof(DlgControl));

      if(!pdc)
         goto abort;

      pdc->wID = (uint16)-1;

      cls = (AEECLSID)(pi->pText != NULL ? AEECLSID_SOFTKEYCTL : AEECLSID_MENUCTL);

      ISHELL_CreateInstance(pme->m_pShell, cls, (void **)(&pdc->pc));

      if(!pdc->pc)
         goto abort;

      if(cls == AEECLSID_SOFTKEYCTL && (pi->dwProps & CP_ICON))
         IMENUCTL_SetProperties((IMenuCtl *)pdc->pc, (MP_MAXSOFTKEYITEMS|MP_TRI_STATE_IMAGE));

      for(pwb = (uint16 *)pi->pBtnIDs; *pwb != 0; pwb++){

   // Make sure we clear the structure each time.  This prevents a bug whereby
   // the image is set incorrectly...

         MEMSET(&cai, 0, sizeof(CtlAddItem));
         cai.pszResText = (char *)pi->pszRes;

         cai.wItemID = *pwb;
         if(pi->dwProps & CP_ICON)
            cai.wImage = cai.wItemID;
         else
            cai.wText = cai.wItemID;
         ICONTROL_HandleEvent(pdc->pc, EVT_CTL_ADD_ITEM, 0, (uint32)&cai);
      }

   // If the menu is comprised of tri-state buttons, then tell it to use the tri-state 
   // image setting...

      if(cls == AEECLSID_SOFTKEYCTL){
         if(pi->dwProps & CP_ICON)
            IMENUCTL_SetProperties((IMenuCtl *)pdc->pc, (MP_MAXSOFTKEYITEMS|MP_TRI_STATE_IMAGE));
         rcd.dy = ADialog_AdjustSoftkeyMenu(pme, pdc->pc, &rcd, HAS_BORDER(pme)) - rcd.y;
      }            
      else{
         if(pi->pTitle){
            bStatic = FALSE;
            IMENUCTL_SetTitle((IMenuCtl *)pdc->pc, NULL, 0, (AECHAR *)pi->pTitle);
            if(pi->dwProps & ST_TITLEALLOC)
               FreePtr((void **)&pi->pTitle);
         }
      }
      pme->m_pFocus = pdc;
   }

   // Create the text control...

   if(bStatic){
      pdc = (DlgControl *)MALLOC(sizeof(DlgControl));
      if(!pdc)
         goto abort;

      if(pme->m_pControls)
         pme->m_pControls->pNext = pdc;
      else
         pme->m_pControls = pdc;

      pdc->wID = (uint16)-2;
      ISHELL_CreateInstance(pme->m_pShell, AEECLSID_STATIC, (void **)(&pdc->pc));

      if(!pdc->pc)
         goto abort;

      ISTATIC_SetRect((IStatic *)pdc->pc, &rcd);
      ISTATIC_SetProperties((IStatic *)pdc->pc, pi->dwProps);
      ISTATIC_SetText((IStatic *)pdc->pc, (AECHAR *)pi->pTitle, (AECHAR *)pi->pText,pi->fntTitle, pi->fntText);
      pme->m_dwStaticProps = pi->dwProps;
      pme->m_pPrmpStatic = (IStatic *)pdc->pc;
   }

   pme->m_bPrompt = TRUE;
   pme->m_bReady = TRUE;
   if(pi->dwTimeout)
      ISHELL_SetTimer(pme->m_pShell, pi->dwTimeout, (PFNNOTIFY)ADialog_Timeout, pme);

   ADialog_Redraw(po);
   return(TRUE);
}

/*===========================================================================

Internal Method: Shows and Processes Copyright Dialogs

===========================================================================*/
static boolean ADialog_ShowCopyright(ADialog * pme)
{
   IShell *       pShell;
   IDisplay *     pd;
   void       *   pmc;
   void       *   pae;
   AEEAppInfo     ai;
   IImage *       pi;
   AEEImageInfo   ii;
   CRText         t[4];
   CRText *       pt;
   int            x,cy,cyFont,cx,i,nText,cxMax;
   AEERect        rc;
   char   *       pszMIF;

   pShell = pme->m_pShell;
   pd = pme->m_pDisplay;
   pae = AEE_GetAppContext();

   // If it failed, then just resume the applet...

   if (!pae || (pmc = AEE_GetAppModuleContext(pae)) == NULL){
#if defined(AEE_SIMULATOR)
      DBGPRINTF("Problem in ADialog_ShowCopyright:app=%d", pae);
#endif
      return(FALSE);
   }

   // Retrieve the information about the applet...

   ISHELL_QueryClass(pShell, AEE_GetAppContextCls(pae), &ai);

   MEMSET(t, 0, sizeof(t));
   t[0].wID = APPR_NAME(ai);
   t[0].pszRes = ai.pszMIF;

   pszMIF = (char *)AEE_GetModuleContextMIF(pmc);

   if (pszMIF){
      t[1].wID = IDS_MIF_COPYRIGHT;
      t[2].wID = IDS_MIF_COMPANY;
	   t[3].wID = IDS_MIF_VERSION;
      t[1].pszRes = t[2].pszRes = t[3].pszRes = pszMIF;
   }

   cxMax = pme->m_cxScreen - ((AEE_3D_FRAME_SIZE + 4) * 2);

   cx = 0;
   for (pt = t, i = nText = 0; i < 4; i++, pt++){
      if (pt->pszRes && ISHELL_LoadResString(pShell,pt->pszRes, pt->wID, pt->sz, sizeof(pt->sz))){
         pt->cx = IDISPLAY_MeasureText(pd, AEE_FONT_BOLD, pt->sz);
         if (pt->cx > cxMax)
            pt->cx = cxMax;

         nText++; 
         if (pt->cx > cx)
            cx = pt->cx;
      }
   }

   cyFont = IDISPLAY_GetFontMetrics(pd, AEE_FONT_BOLD, NULL, NULL);

   // Account for the frame size + the padding...

   cy = ((cyFont * nText) + ((AEE_3D_FRAME_SIZE + 4) * 2));
   pi = ISHELL_LoadResImage(pShell,ai.pszMIF,APPR_ICON(ai));
   
   if (pi){
      IIMAGE_SetParm(pi, IPARM_ROP, AEE_RO_MASK, 0);
      IIMAGE_GetInfo(pi,&ii);
      
	  if (cy + ii.cy > (int)pme->m_cyScreen || ii.cxFrame > cxMax){
         IIMAGE_Release(pi);
         pi = NULL;
      }
      else{
         cy += ii.cy;
         if(cx < ii.cxFrame)
            cx = ii.cxFrame;
      }
   }
   cx += ((AEE_3D_FRAME_SIZE + 4) * 2);

   SETAEERECT(&rc, (pme->m_cxScreen - cx) / 2, (pme->m_cyScreen - cy) / 2, cx, cy);
   IDISPLAY_DrawFrame(pd, &rc, AEE_FT_RAISED, CLR_SYS_WIN);

   // IDISPLAY_DrawFrame adjusts rc to account for the frame
   // We need to only adjust for the extra 4 pixel pad.
   rc.y += 4;
   rc.dy -= (4 * 2);     // Account for 4 pixel padding on top and bottom

   for (pt = t, i = 0; i < 4; i++, pt++){
      if (*pt->sz){
         IDISPLAY_DrawText(pd,(AEEFont)(i ? AEE_FONT_NORMAL : AEE_FONT_BOLD),pt->sz, -1, -1, rc.y, &rc, IDF_ALIGN_CENTER);
         rc.y += cyFont;
      }
      
	  if (!i && pi){
         x = rc.x + ((rc.dx - ii.cxFrame) / 2);
         IIMAGE_Draw(pi,x,rc.y);
         rc.y += ii.cy;
      }
   }

   if (pi)
      IIMAGE_Release(pi);

   pme->m_bReady = pme->m_bPrompt = TRUE;
   pme->m_bCopyright = TRUE;

   IDISPLAY_Update(pd);
   ISHELL_SetTimer(pShell, COPYRIGHT_TIMEOUT, (PFNNOTIFY)ADialog_Timeout, pme);

   return(TRUE);
}

/*===========================================================================

Internal Method: Processes dismissals of dialog based on timers. Ex: Copyright Dialogs

===========================================================================*/

static void ADialog_Timeout(ADialog * pme)
{
   ADialog_Dismiss(pme, pme->m_wDefCommand);
}

/*===========================================================================

Internal Method: Helps to dismiss current dialog and return control to previous

===========================================================================*/
static void ADialog_Dismiss(ADialog * pme, uint16 wParam)
{
   if(pme->m_bClosing)
      return;

   pme->m_bClosing = TRUE;
   ADialog_AddRef((IDialog *)pme);

   if(wParam)
      ADialog_SendEvent(pme, EVT_COMMAND, wParam, 0);

   pme->m_bClosing = FALSE;
   if(!pme->m_nRefs)
      pme->m_nRefs++;

   if(ADialog_Release((IDialog *)pme)){

   // If we did not already notify them, then tell them during the release...

      if(!wParam)
         pme->m_bPrompt = FALSE;

      ADialog_Release((IDialog *)pme);
   }
}

/*===========================================================================

Internal Method: Gets Total screen size needed by current dialog

===========================================================================*/
static void ADialog_GetSize(ADialog * pme, uint32 dwProps, AEERect * pr)
{
   int nBorder;

   if(pr->x == -1 || pr->y == -1)
      SETAEERECT(pr, 0, 0, pme->m_cxScreen, pme->m_cyScreen);

   pme->m_rc = *pr;

   if(dwProps & (CP_3D_BORDER|CP_BORDER))
      nBorder = (int)((dwProps & CP_3D_BORDER) ? AEE_3D_FRAME_SIZE : AEE_FRAME_SIZE);
   else
      nBorder = 0;

   pme->m_dwProps = dwProps;

   if(nBorder){
      pr->x += nBorder;
      pr->dx -= (nBorder * 2);
      pr->y += nBorder;
      pr->dy -= (nBorder * 2);
   }
}

/*===========================================================================

Internal Method: Re-calibrates the soft key menu coordinates to fit exactly in the dialog

===========================================================================*/
static int ADialog_AdjustSoftkeyMenu(ADialog * pme, IControl * psk, AEERect * pr, boolean bFrame)
{
   AEERect  rc;
   int      ySK;
   RGBVAL   cMenu, cWin;

   if(psk){
      ICONTROL_GetRect(psk, &rc);

   // Adjust the softkey menu.

      rc.x = pr->x;
      rc.dx = pr->dx;
      rc.y = pr->y + (pr->dy - rc.dy);

   // Add a single pixel offset so that it does not touch the frame...

      if(bFrame)
         rc.y--;

      ICONTROL_SetRect(psk,&rc);
      ySK = rc.y;

   // See if we need to draw a line between the SK menu and the other controls.  This
   // is true if the CLR_SYS_ITEM color differs from CLR_SYS_WIN...

      cMenu = ADialog_GetSysColor(pme, CLR_SYS_ITEM);
      cWin = ADialog_GetSysColor(pme, CLR_SYS_WIN);
      if(cWin != cMenu)
         pme->m_ySKFrame = --ySK;
   }
   else
      ySK = 0;
   return(ySK);
}

/*===========================================================================

Internal Method: Validates and suitably adjusts bounding rectangle of the dialog control

===========================================================================*/
static void AdjustControlRect(IControl * pc, AEERect * prc, AEERect * prcd)
{
   int   n;

   // Add the offset for the dialog...

   prc->x += prcd->x;
   prc->y += prcd->y;

   // Verify that it falls into range on the screen...

   n = (prc->y + prc->dy) - (prcd->y + prcd->dy);
   if(n > 0)
      prc->dy -= n;

   n = (prc->x + prc->dx) - (prcd->x + prcd->dx);
   if(n > 0)
      prc->dx -= n;
}

/*===========================================================================

Internal Method: Helper Routine to free a ptr

===========================================================================*/
static void FreePtr(void ** pp)
{
   if(*pp){
      FREE(*pp);
      *pp = NULL;
   }
}


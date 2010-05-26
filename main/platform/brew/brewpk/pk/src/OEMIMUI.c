/*=============================================================================

FILE: OEMIMUI.c

SERVICES: OEM Functions Input Method UI

GENERAL DESCRIPTION:
         OEM Input Method UI control implementation

        Copyright 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "OEMIMUI.h"
#include "AEE_OEM.h"
#include "AEEDisp.h"
#include "AEEStdLib.h"
#include "AEEMenu.h"
#include "AEEPointerHelpers.h"

//#define PROFILE_IMUI_MENUCTL

#define APPROXIMATE_PER_PAGE        (8)
#define APPROXIMATE_MAX_PAGE_CHARS  (32)

// Try and keep me aligned 32bits
OBJECT(CIMPage){
   uint16   wPage;      // Page number
   uint16   wItemStart; // Item # this page starts with
   uint16   wItemCount; // Item count on this page
   uint16   wPrevStart; // Item # last page started with
   uint32   nMaxItems;  // Maximum number of items in this menu
};

OBJECT(CIMUI){
   AEEVTBL(IIMUI) *    pvt;

   uint32               nRefs;
   AEECLSID             clsMe;
   uint32               dwProps;
   AEERect              rctMe;
   IShell *             pShell;
   IDisplay *           pDisplay;
   IIMMgr *             pIMMgr;
   IMenuCtl *           pSKLetterComp;
   AECHAR               szwLetterComp[APPROXIMATE_MAX_PAGE_CHARS];
   IMenuCtl *           pSKCharacterList;
   AECHAR               szwCharList[APPROXIMATE_MAX_PAGE_CHARS];
   AEEFont              fntLetterComp;
   AEEFont              fntCharList;
   IControl *           pActiveCtl;
   CIMPage              pgLetterListData;
   CIMPage              pgCharListData;
   boolean              bActive;
   boolean              bWordAssoc;
   AEEMenuColors        mcColors[4];
   AEEItemStyle         isStyles[8];
};

// Do not rely on HandleEvent to distinguish active
// Due to UI design both controls should remain active
// to show the selection indicator.
// Use these macros to check active state before calling HandleEvent
// Use IMUI_SetActiveSK() to SetActive() between SK
#define CHARSK_ACTIVE(p)            (boolean)(((IControl *)(p)->pSKCharacterList == (p)->pActiveCtl) ? TRUE : FALSE)
#define LETTERSK_ACTIVE(p)          (boolean)(((IControl *)(p)->pSKLetterComp == (p)->pActiveCtl) ? TRUE : FALSE)

#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

int                     OEMIMUI_New(IShell * pIShell, AEECLSID clsID, void ** ppobj);

static uint32           OEMIMUI_AddRef(IIMUI * po);
static uint32           OEMIMUI_Release(IIMUI * po);
static boolean          OEMIMUI_HandleEvent(IIMUI * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean          OEMIMUI_Redraw(IIMUI * po);
static void             OEMIMUI_SetActive(IIMUI * po, boolean bActive);
static boolean          OEMIMUI_IsActive(IIMUI * po);
static void             OEMIMUI_SetRect(IIMUI * po, const AEERect * prc);
static void             OEMIMUI_GetRect(IIMUI * po, AEERect * prc);
static void             OEMIMUI_SetProperties(IIMUI * po, uint32 dwProperties);
static uint32           OEMIMUI_GetProperties(IIMUI * po);
static void             OEMIMUI_Reset(IIMUI * po);

static int              OEMIMUI_SetClsPtr(IIMUI * po, AEECLSID cls, IBase * ptr);
static int              OEMIMUI_SetOpt(IIMUI * po, uint16 wOption, void * pData, uint32 nSize);
static int              OEMIMUI_GetOpt(IIMUI * po, uint16 wOption, void * pData, uint32 nSize);

static void             IMUI_RecalcChildren(CIMUI * pme);
static int              IMUI_RecalcLetterSK(CIMUI * pme, boolean bPageBack);
static int              IMUI_RecalcCharSK(CIMUI * pme, boolean bPageBack);
static int              IMUI_MeasureMenuPage(CIMUI * pme, IControl * pList, AECHAR ** ppBuff, uint16 * pwItems, boolean bPageBack);
static int              IMUI_CalcRectHeight(CIMUI * pme);
static int              IMUI_SetActiveSK(CIMUI * pme, IControl * pActive, boolean bRedraw);
static int              IMUI_InitDefaultColors(CIMUI * pme);
static boolean          IMUI_ProcessSelectedCandidate(CIMUI * pme);

static const AEEVTBL(IIMUI) gpvtIIMUI = { OEMIMUI_AddRef,
                                          OEMIMUI_Release,
                                          OEMIMUI_HandleEvent,
                                          OEMIMUI_Redraw,
                                          OEMIMUI_SetActive,
                                          OEMIMUI_IsActive,
                                          OEMIMUI_SetRect,
                                          OEMIMUI_GetRect,
                                          OEMIMUI_SetProperties,
                                          OEMIMUI_GetProperties,
                                          OEMIMUI_Reset,
                                          OEMIMUI_SetClsPtr,
                                          OEMIMUI_SetOpt,
                                          OEMIMUI_GetOpt };

//------------------------------------------------------------------------------------------
// Static Class Specifics
//------------------------------------------------------------------------------------------
int OEMIMUI_New(IShell * pIShell, AEECLSID clsID, void ** ppobj)
{
   CIMUI *        pme = NULL;
   int            nErr;

   *ppobj = NULL;

   switch( clsID ){
   case AEECLSID_OEMIMUI:
      break;
   default:
      return EUNSUPPORTED;
   }

   if( (pme = (CIMUI *)AEE_NewClassEx((IBaseVtbl*)&gpvtIIMUI, sizeof(CIMUI), FALSE)) == NULL ){
      return ENOMEMORY;
   }
   if( (nErr = ISHELL_CreateInstance(pIShell, AEECLSID_SOFTKEYCTL, (void **)&pme->pSKCharacterList)) != AEE_SUCCESS ){
      OEMIMUI_Release((IIMUI *)pme);
      return EUNABLETOLOAD;
   }
   if( (nErr = ISHELL_CreateInstance(pIShell, AEECLSID_SOFTKEYCTL, (void **)&pme->pSKLetterComp)) != AEE_SUCCESS ){
      OEMIMUI_Release((IIMUI *)pme);
      return EUNABLETOLOAD;
   }

   IMENUCTL_SetProperties(pme->pSKCharacterList, IMENUCTL_GetProperties(pme->pSKCharacterList) |MP_MAXSOFTKEYITEMS|MP_NO_REDRAW);
   IMENUCTL_SetProperties(pme->pSKLetterComp, IMENUCTL_GetProperties(pme->pSKLetterComp) |MP_MAXSOFTKEYITEMS|MP_NO_REDRAW);
   // Needless notifications for us.
   IMENUCTL_EnableCommand(pme->pSKCharacterList, FALSE); 
   IMENUCTL_EnableCommand(pme->pSKLetterComp, FALSE);

   // Set up the members
   ISHELL_AddRef(pIShell);
   pme->pShell          = pIShell;
   pme->pDisplay        = NULL;
   pme->pIMMgr          = NULL;
   pme->nRefs           = 1;
   pme->clsMe           = clsID;
   OEMIMUI_Reset((IIMUI *)pme);

   MEMSET((void *)&pme->rctMe, 0, sizeof(AEERect));

   // Return ourself
   *ppobj = (void *)pme;

   return AEE_SUCCESS;
}

//------------------------------------------------------------------------------------------
// IControl methods
//------------------------------------------------------------------------------------------
static uint32 OEMIMUI_AddRef(IIMUI * po)
{
   return ++((CIMUI *)po)->nRefs;
}
static uint32 OEMIMUI_Release(IIMUI * po)
{
   CIMUI * pme = (CIMUI *)po;

   if( --pme->nRefs ){
      return pme->nRefs;
   }
   if( pme->pShell ){
      ISHELL_Release( pme->pShell );
      pme->pShell = NULL;
   }
   if( pme->pDisplay ){
      IDISPLAY_Release( pme->pDisplay );
      pme->pDisplay = NULL;
   }
   if( pme->pSKCharacterList ){
      IMENUCTL_Release( pme->pSKCharacterList );
      pme->pSKCharacterList = NULL;
   }
   if( pme->pSKLetterComp ){
      IMENUCTL_Release( pme->pSKLetterComp );
      pme->pSKLetterComp = NULL;
   }
   if( pme->pIMMgr ){
      IIMMGR_Release( pme->pIMMgr );
      pme->pIMMgr = NULL;
   }

   FREE( (void *)pme );
   return 0;
}
static boolean OEMIMUI_HandleEvent(IIMUI * po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   CIMUI * pme = (CIMUI *)po;

   if( (eCode >= EVT_POINTER_DOWN && eCode <= EVT_POINTER_STALE_MOVE) ){
      if( eCode == EVT_POINTER_STALE_MOVE || eCode == EVT_POINTER_MOVE ){
         return FALSE;
      }
      if( eCode == EVT_POINTER_DOWN ){
         AEERect  rctChar, rctLetter;
         boolean  bRet  = FALSE;
         int16   wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
         int16   wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);
         
         IMENUCTL_GetRect(pme->pSKCharacterList, &rctChar);
         IMENUCTL_GetRect(pme->pSKLetterComp, &rctLetter);
         if( PT_IN_RECT(wXPos, wYPos, rctChar) ){
            // In the Character SK!
            if( LETTERSK_ACTIVE(pme) ){
               (void)IMUI_SetActiveSK(pme, (IControl *)pme->pSKCharacterList, TRUE);
            }
            bRet = TRUE;
            (void)IMENUCTL_HandleEvent(pme->pSKCharacterList, eCode, wParam, dwParam);
            IMENUCTL_Redraw(pme->pSKLetterComp);
            IMENUCTL_Redraw(pme->pSKCharacterList);
         }else if( PT_IN_RECT(wXPos, wYPos, rctLetter) ){
            // In Letter SK
            if( CHARSK_ACTIVE(pme) ){
               (void)IMUI_SetActiveSK(pme, (IControl *)pme->pSKLetterComp, TRUE);
            }
            bRet = TRUE;
            (void)IMENUCTL_HandleEvent(pme->pSKLetterComp, eCode, wParam, dwParam);
            // Reset this to the first page
            pme->pgCharListData.nMaxItems = 0;
            (void)IMUI_RecalcCharSK(pme, FALSE);

            IMENUCTL_Redraw(pme->pSKLetterComp);
            IMENUCTL_Redraw(pme->pSKCharacterList);
         }else if( PT_IN_RECT(wXPos, wYPos, pme->rctMe) ){
            bRet = TRUE; // In me, but nothing to do!
         }
         return bRet;
      }else if( eCode == EVT_POINTER_UP ){ // protect very unlikely additions or redefines
         if( ICONTROL_HandleEvent(pme->pActiveCtl, eCode, wParam, dwParam) ){
            boolean bRet = TRUE;

            IMENUCTL_Redraw(pme->pSKLetterComp);
            IMENUCTL_Redraw(pme->pSKCharacterList);
            if( CHARSK_ACTIVE(pme) ){
               // This looks crazy and it is a little, but due to notificaiton restrictions...
               // If the IMUI_ProcessSelectedCandidate returns FALSE it means there is no 
               // more word association But, we are probably contained in an IDialog
               // A Pen Up event in Dialog's dispatching logic needs to be handled to be passed 
               // to the registered handler, so flip it otherwise it will go to the app needlessly.
               // Then, our trigger for telling the OEMText layer (registered handler) that this 
               // was handled is by setting ourself inactive.
               bRet = IMUI_ProcessSelectedCandidate(pme);
               if( !bRet ){
                  OEMIMUI_SetActive(po, FALSE);
                  bRet = TRUE; // Notify the pen registered dialog holder
               }
            }
            return bRet;
         }
      }
      return FALSE;
   }
   if( (eCode < EVT_KEY || eCode > EVT_CTL_TEXT_MODECHANGED) || !pme->bActive || !pme->pIMMgr ){
      return FALSE;
   }
   if( eCode == EVT_KEY && pme->pIMMgr && pme->pDisplay ){
      switch( wParam ){
      case AVK_UP:
      case AVK_DOWN:
      case AVK_SELECT: // Override EVT_COMMAND, AKA menus won't get this key
         if( LETTERSK_ACTIVE(pme) ){
            // Top control has focus, so if Up is pressed, then EVT_CTL_TAB is sent
            if( wParam == AVK_UP ){
               ISHELL_HandleEvent(pme->pShell, EVT_CTL_TAB, 0, (uint32)pme);
               return TRUE;
            }
            // Otherwise Down or Select was pressed so focus can move down
            (void)IMUI_SetActiveSK(pme, (IControl *)pme->pSKCharacterList, TRUE);
            return TRUE;
         }
         if( CHARSK_ACTIVE(pme) ){
            boolean bRet = FALSE;
            // Bottom control has focus, so if down was pressed, then EVT_CTL_TAB is sent
            if( wParam == AVK_DOWN ){
               ISHELL_HandleEvent(pme->pShell, EVT_CTL_TAB, 1, (uint32)pme);
               return TRUE;
            }
            // If the key was not Select, then switch the focus menu
            if( wParam != AVK_SELECT ){
               (void)IMUI_SetActiveSK(pme, (IControl *)pme->pSKLetterComp, TRUE);
               bRet = TRUE;
            }else{
               bRet = IMUI_ProcessSelectedCandidate(pme);
            }
            return bRet;
         }
         return TRUE;
      case AVK_LEFT:
      case AVK_RIGHT:
         // If the Letter SK is active, check if it will handle the event
         // If it doesn't and there is 1 item, pretend it was handled to avoid
         // feeding the event to the other control
         if( LETTERSK_ACTIVE(pme)
             && (IMENUCTL_HandleEvent(pme->pSKLetterComp, eCode, wParam, dwParam) 
             || IMENUCTL_GetItemCount(pme->pSKLetterComp) == 1) ){

            if( IMENUCTL_GetSel(pme->pSKLetterComp) == 0 && wParam == AVK_RIGHT ){
               // This is when they wrapped around, so go to the next page
            
               if( (pme->pgLetterListData.wItemStart + pme->pgLetterListData.wItemCount) != (uint16)pme->pgLetterListData.nMaxItems ){
                  if( !IMUI_RecalcLetterSK(pme, FALSE) ){
                     IMENUCTL_Redraw(pme->pSKLetterComp);
                  }
               }else{
                  IMENUCTL_SetSel(pme->pSKLetterComp, (uint16)(pme->pgLetterListData.wItemCount-1));
                  return TRUE;
               }
            }else if( (IMENUCTL_GetSel(pme->pSKLetterComp) == pme->pgLetterListData.wItemCount-1)
                       && wParam == AVK_LEFT ){

               // This is when they wrapped the other way, but prevent wrap to end
               if( pme->pgLetterListData.wPage == 0 ){
                  IMENUCTL_SetSel(pme->pSKLetterComp, 0);
                  return TRUE; // don't recalc other menu, nothing happened
               }else{
                  (void)IMUI_RecalcLetterSK(pme, TRUE);
               }
            }
            // Reset this to the first page
            pme->pgCharListData.nMaxItems = 0;
            (void)IMUI_RecalcCharSK(pme, FALSE);

            IMENUCTL_Redraw(pme->pSKLetterComp);
            IMENUCTL_Redraw(pme->pSKCharacterList);
            return TRUE;
         }
         // If the Char SK is active, check if it will handle the event
         // If it doesn't and there is 1 item, pretend it was handled to avoid
         // skipping important logic
         if( CHARSK_ACTIVE(pme)
             && (IMENUCTL_HandleEvent(pme->pSKCharacterList, eCode, wParam, dwParam) 
             || IMENUCTL_GetItemCount(pme->pSKCharacterList) == 1) ){

            if( IMENUCTL_GetSel(pme->pSKCharacterList) == 0 && wParam == AVK_RIGHT ){
               // This is when they wrapped around, so go to the next page
      
               if( (pme->pgCharListData.wItemStart + pme->pgCharListData.wItemCount) != (uint16)pme->pgCharListData.nMaxItems ){
                  (void)IMUI_RecalcCharSK(pme, FALSE);

                  IMENUCTL_Redraw(pme->pSKCharacterList);
               }else{
                  IMENUCTL_SetSel(pme->pSKCharacterList, (uint16)(pme->pgCharListData.wItemCount-1));
               }
               return TRUE;
            }else if( (IMENUCTL_GetSel(pme->pSKCharacterList) == pme->pgCharListData.wItemCount-1)
                      && wParam == AVK_LEFT ){

               // This is when they wrapped the other way, but prevent wrap to end
               if( pme->pgCharListData.wPage == 0 ){
                  IMENUCTL_SetSel(pme->pSKCharacterList, 0);
               }else{
                  (void)IMUI_RecalcCharSK(pme, TRUE);
               }
            }
            IMENUCTL_Redraw(pme->pSKCharacterList);
            return TRUE;
         }
         return TRUE;
      case AVK_CLR:
         // If it is word associating and CLR is pressed, commit the word
         if( pme->bWordAssoc ){
            CtlAddItem ai;

            if( IMENUCTL_GetItem(pme->pSKLetterComp, 0, &ai)
               && ai.pText && *ai.pText ){
               IIMMGR_SetData(pme->pIMMgr, ai.pText);
            }
            return FALSE;
         }
         if( LETTERSK_ACTIVE(pme) ){
            if( !IIMMGR_HandleEvent(pme->pIMMgr, eCode, wParam, dwParam) ){
               // Kill Dialog w/o char
               // Force it NULL!
               IIMMGR_SetData(pme->pIMMgr, NULL);
               return FALSE;
            }else{
               int nElement = 0;
               (void)IIMMGR_GetCompositionCount(pme->pIMMgr, &nElement);
               if( !nElement ){
                  IIMMGR_SetData(pme->pIMMgr, NULL);
                  return FALSE;
               }else{
                  pme->pgLetterListData.nMaxItems = 0;
                  if( !IMUI_RecalcLetterSK(pme, FALSE) ){
      
                     pme->pgCharListData.nMaxItems = 0;
                     (void)IMUI_RecalcCharSK(pme, FALSE);
                  }
               }
            }
         }else{
            // FALSE param as redraw happens immediately below
            (void)IMUI_SetActiveSK(pme, (IControl *)pme->pSKLetterComp, FALSE);
         }
         IMENUCTL_Redraw(pme->pSKCharacterList);
         IMENUCTL_Redraw(pme->pSKLetterComp);
         return TRUE;
      }
      if( wParam >= AVK_0 && wParam <= AVK_9 ){
#ifdef PROFILE_IMUI_MENUCTL // {
         uint32 nProfile;

         DBGPRINTF("---------------------");
#endif // } PROFILE_IMUI_MENUCTL
         // If it is word associating and a numerical key is pressed, commit the word
         if( pme->bWordAssoc ){
            CtlAddItem ai;

            if( IMENUCTL_GetItem(pme->pSKLetterComp, 0, &ai)
               && ai.pText && *ai.pText ){
               IIMMGR_SetData(pme->pIMMgr, ai.pText);
            }
            return FALSE;
         }

         if( !IIMMGR_HandleEvent(pme->pIMMgr, eCode, wParam, dwParam) ){
            int nNumCandidate;
            // If there are no candidates in composition
            // Then the first key was invalid, so dialog needs to go away
            if( IIMMGR_GetCompositionCandidates(pme->pIMMgr, NULL, 0, 0, &nNumCandidate, NULL)
                || !nNumCandidate ){
               // Kill Dialog w/o char
               // Force it NULL!
               IIMMGR_SetData(pme->pIMMgr, NULL);
               return FALSE;
            }
            // Otherwise, they have been composing successfully and
            // a beep could be given to tell them their next key is 
            // invalid and keep around their current typing.
            // If you want to play a sound for an error, do so here.
            // The usual case of coming here is that another character
            // was pressed in the sequence that didn't offer any choices.
            // ISHELL_Beep(pme->pIShell, BEEP_ALERT, FALSE);
         }else{
#ifdef PROFILE_IMUI_MENUCTL // {
            nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
            pme->pgLetterListData.nMaxItems = 0;
            if( !IMUI_RecalcLetterSK(pme, FALSE) ){
               // 3rd param FALSE, redraw happens after re-calc
               (void)IMUI_SetActiveSK(pme, (IControl *)pme->pSKLetterComp, FALSE);

               pme->pgCharListData.nMaxItems = 0;
               (void)IMUI_RecalcCharSK(pme, FALSE);
#ifdef PROFILE_IMUI_MENUCTL // {
               DBGPRINTF("%d\tRecalced!!!", GETTIMEMS() - nProfile);
               nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
               IMENUCTL_Redraw(pme->pSKLetterComp);
               IMENUCTL_Redraw(pme->pSKCharacterList);
#ifdef PROFILE_IMUI_MENUCTL // {
               DBGPRINTF("%d\tRedrawn!!!", GETTIMEMS() - nProfile);
               nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
            }
         }
      }
   }

   if( pme->dwProps & IUIP_TRAPEVENTS ){
      return TRUE;
   }

   return FALSE;
}
static boolean OEMIMUI_Redraw(IIMUI * po)
{
   CIMUI * pme = (CIMUI *)po;

   if( pme->pDisplay ){
      IDISPLAY_FillRect(pme->pDisplay, &pme->rctMe, CLR_SYS_ITEM);

      if( pme->dwProps & CP_3D_BORDER ){
         IDISPLAY_DrawFrame(pme->pDisplay, &pme->rctMe, AEE_FT_3D_EMPTY, CLR_SYS_ITEM);
      }else if( pme->dwProps & CP_BORDER ){
         IDISPLAY_FrameRect(pme->pDisplay, &pme->rctMe);
      }
      if( pme->pSKCharacterList ){
         IMENUCTL_Redraw(pme->pSKCharacterList);
      }
      if( pme->pSKLetterComp ){
         IMENUCTL_Redraw(pme->pSKLetterComp );
      }
      IDISPLAY_Update(pme->pDisplay);
      return TRUE;
   }
   return FALSE;
}
static void OEMIMUI_SetActive(IIMUI * po, boolean bActive)
{
   CIMUI * pme = (CIMUI *)po;

   if( !bActive || !(pme->dwProps & CP_STATIC) ){
      pme->bActive   = bActive;
      if( pme->pSKCharacterList ){
         IMENUCTL_SetActive(pme->pSKCharacterList, bActive);
      }
      if( pme->pSKLetterComp ){
         IMENUCTL_SetActive(pme->pSKLetterComp, bActive);
      }
   }
}
static boolean OEMIMUI_IsActive(IIMUI * po)
{
   CIMUI * pme = (CIMUI *)po;

   return pme->bActive;
}
static void OEMIMUI_SetRect(IIMUI * po, const AEERect * prc)
{
   CIMUI * pme = (CIMUI *)po;

   MEMCPY((void *)&pme->rctMe, (void *)prc, sizeof(AEERect));

   IMUI_RecalcChildren(pme);
}
static void OEMIMUI_GetRect(IIMUI * po, AEERect * prc)
{
   CIMUI * pme = (CIMUI *)po;
   MEMCPY((void *)prc, (void *)&pme->rctMe, sizeof(AEERect));
}
static void OEMIMUI_SetProperties(IIMUI * po, uint32 dwProperties)
{
   CIMUI * pme    = (CIMUI *)po;

   if( dwProperties & CP_USE_DEFAULT ){
      dwProperties = IUIP_TRAPEVENTS | CP_USE_DEFAULT;
   }
   pme->dwProps   = dwProperties;
}
static uint32 OEMIMUI_GetProperties(IIMUI * po)
{
   CIMUI * pme = (CIMUI *)po;

   return pme->dwProps;
}
static void OEMIMUI_Reset(IIMUI * po)
{
   CIMUI *        pme = (CIMUI *)po;
   int            i;
   AEEItemStyle   isSel, isNon;

   pme->dwProps      = IUIP_TRAPEVENTS;
   if( pme->pSKCharacterList ){
      IMENUCTL_Reset(pme->pSKCharacterList);
   }
   if( pme->pSKLetterComp ){
      IMENUCTL_Reset(pme->pSKLetterComp);
   }
   MEMSET((void *)&pme->pgLetterListData, 0, sizeof(CIMPage));
   MEMSET((void *)&pme->pgCharListData, 0, sizeof(CIMPage));
   MEMSET((void *)pme->szwLetterComp, 0, sizeof(pme->szwLetterComp));
   MEMSET((void *)pme->szwCharList, 0, sizeof(pme->szwCharList));
   OEMIMUI_SetActive(po, FALSE);
   pme->pActiveCtl   = (IControl *)pme->pSKLetterComp;
   pme->bWordAssoc   = FALSE;

   pme->fntLetterComp   = AEE_FONT_BOLD;
   pme->fntCharList     = AEE_FONT_BOLD;
   (void)IMUI_InitDefaultColors(pme);
   (void)OEMIMUI_SetOpt(po, IUIO_COLORS, (void *)pme->mcColors, sizeof(pme->mcColors));

   // Initialise with default styles.
   ISHELL_GetItemStyle(pme->pShell, AEE_IT_SOFTKEY, &isNon, &isSel);
   for( i = 0; i < ARRAY_SIZE(pme->isStyles); i+=2 ){
      MEMCPY((void *)&pme->isStyles[i], (void *)&isNon, sizeof(AEEItemStyle));
      MEMCPY((void *)&pme->isStyles[i+1], (void *)&isSel, sizeof(AEEItemStyle));
   }
   (void)OEMIMUI_SetOpt(po, IUIO_MENUSTYLE, (void *)pme->isStyles, sizeof(pme->isStyles));
}

//------------------------------------------------------------------------------------------
// IMUI Specifics
//------------------------------------------------------------------------------------------
static int OEMIMUI_SetClsPtr(IIMUI * po, AEECLSID cls, IBase * ptr)
{
   CIMUI * pme = (CIMUI *)po;
   
   switch( cls ){
   case AEECLSID_DISPLAY:
      if( pme->pDisplay ){
         IDISPLAY_Release( pme->pDisplay );
      }
      pme->pDisplay = (IDisplay *)ptr;
      if( ptr ){
         IMUI_CalcRectHeight(pme);
      }
      break;
   case AEECLSID_OEMIMMGR:
      if( pme->pIMMgr ){
         IIMMGR_Release( pme->pIMMgr );
      }
      pme->pIMMgr = (IIMMgr *)ptr;
      break;
   default:
      return EUNSUPPORTED;
   }

   if( ptr ){
      IBASE_AddRef( ptr );
   }
   return AEE_SUCCESS;
}
static int OEMIMUI_SetOpt(IIMUI * po, uint16 wOption, void * pData, uint32 nSize)
{
   CIMUI * pme = (CIMUI *)po;

   if( !pData ){
      return EBADPARM;
   }
   switch( wOption ){
   case IUIO_COLORS:
      if( nSize != (sizeof(AEEMenuColors)<<2) ){
         return EBADPARM;
      }
      MEMCPY((void *)pme->mcColors, pData, (sizeof(AEEMenuColors)<<2));
      (void)IMUI_SetActiveSK(pme, pme->pActiveCtl, FALSE);

      break;
   case IUIO_FONTS:
      {
         IMUIFontStruct * pif = (IMUIFontStruct *)pData;

         if( nSize != sizeof(IMUIFontStruct) || pif->wSize != sizeof(IMUIFontStruct) ){
            return EBADPARM;
         }
         pme->fntLetterComp   = pif->fntLetterComp;
         if( !pme->fntLetterComp ){
            pme->fntLetterComp   = AEE_FONT_BOLD;
         }
         pme->fntCharList     = pif->fntCharList;
         if( !pme->fntCharList ){
            pme->fntCharList     = AEE_FONT_BOLD;
         }
      }
      break;
   case IUIO_MENUSTYLE:
      {
         if( nSize != (sizeof(AEEItemStyle)<<3) ){
            return EBADPARM;
         }
         MEMCPY((void *)pme->isStyles, pData, (sizeof(AEEItemStyle)<<3));
         (void)IMUI_SetActiveSK(pme, pme->pActiveCtl, FALSE);
      }
      break;
   default:
      return EUNSUPPORTED;
   }
   return AEE_SUCCESS;
}
static int OEMIMUI_GetOpt(IIMUI * po, uint16 wOption, void * pData, uint32 nSize)
{
   CIMUI * pme = (CIMUI *)po;
   
   if( !pData ){
      return EBADPARM;
   }
   switch( wOption ){
   case IUIO_COLORS:
      if( nSize != (sizeof(AEEMenuColors)<<2) ){
         return EBADPARM;
      }
      MEMCPY(pData, (void *)pme->mcColors, (sizeof(AEEMenuColors)<<2));
      break;
   case IUIO_FONTS:
      {
         IMUIFontStruct * pif = (IMUIFontStruct *)pData;

         if( nSize != sizeof(IMUIFontStruct) || pif->wSize != sizeof(IMUIFontStruct) ){
            return EBADPARM;
         }
         pif->fntLetterComp   = pme->fntLetterComp;
         pif->fntCharList     = pme->fntCharList;
      }
      break;
   case IUIO_MENUSTYLE:
      {
         if( nSize != (sizeof(AEEItemStyle)<<3) ){
            return EBADPARM;
         }
         MEMCPY(pData, (void *)pme->isStyles, (sizeof(AEEItemStyle)<<3));
      }
      break;
   default:
      return EUNSUPPORTED;
   }
   return AEE_SUCCESS;
}
//------------------------------------------------------------------------------------------
// IMUI Privates
//------------------------------------------------------------------------------------------
static void IMUI_RecalcChildren(CIMUI * pme)
{
   AEERect  rct,
            rctCpy;
   int      nBorder     = 0,
            nRectHeight = 0;

   if( pme->dwProps & CP_3D_BORDER ){
      nBorder = 2;
   }else if( pme->dwProps & CP_BORDER ){
      nBorder = 1;
   }
   // Calc Top rect height if it can be calculated yet
   if( pme->pDisplay ){
      int i, nTmp, nFrameTop = 0, nPadTop = 0;

      nRectHeight = IDISPLAY_GetFontMetrics(pme->pDisplay, pme->fntLetterComp, NULL, NULL);
      // Get offset affecting values
      // Top
      for( i = 0; i < (ARRAY_SIZE(pme->isStyles)>>1); i++ ){
         nTmp = IDISPLAY_DrawFrame(pme->pDisplay, NULL, pme->isStyles[i].ft, RGB_NONE);
         if( nTmp > nFrameTop ){
            nFrameTop = nTmp;
         }
         if( pme->isStyles[i].yOffset > nPadTop ){
            nPadTop = pme->isStyles[i].yOffset;
         }
      }
      // Frame top and bottom plus pad top and bottom
      // PLUS (1Pad SK Border Top and SK bottom)
      nRectHeight += ((nFrameTop + nPadTop + 1)<<1);
   }
   // ATTENTION: Hack for UI guideline conformance, subtracting 2 to rid of borders
   // and make the menus appear to stick to each other. See /*HACK*/
   // If you are not particular about coloring the borders you can remove that and see OEMText.c for the rest
   if( nRectHeight ){
      SETAEERECT(&rct, pme->rctMe.x + nBorder, pme->rctMe.y + nBorder /*HACK*/-2, (pme->rctMe.dx - (nBorder<<1)), nRectHeight );
   }else{
      SETAEERECT(&rct, pme->rctMe.x + nBorder, pme->rctMe.y + nBorder, (pme->rctMe.dx - (nBorder<<1)), ((pme->rctMe.dy - (nBorder<<1))/2) );
   }
   MEMCPY((void *)&rctCpy, (void *)&rct, sizeof(AEERect));
   IMENUCTL_SetRect(pme->pSKLetterComp, &rct);
   SETAEERECT(&rct, rctCpy.x, rctCpy.y + rctCpy.dy /*HACK*/-2, rctCpy.dx, ((pme->rctMe.dy - (nBorder<<1)) - nRectHeight));
   IMENUCTL_SetRect(pme->pSKCharacterList, &rct);
}
static int IMUI_RecalcLetterSK(CIMUI * pme, boolean bPageBack)
{
   int         nNumCandidate  = 0,
               i;
   AECHAR *    pszwItem;
   uint16      wStart;
#ifdef PROFILE_IMUI_MENUCTL // {
   uint32 nProfile   = GETTIMEMS();

   DBGPRINTF("---------------------");
   DBGPRINTF("RecalcLetterSK");
   DBGPRINTF("%u\tInit", nProfile);
#endif // } PROFILE_IMUI_MENUCTL

   if( !pme->pgLetterListData.nMaxItems ){
      // Get Count!!!
      IIMMGR_GetCompositionCandidates(pme->pIMMgr, NULL, 0, 0, &nNumCandidate, NULL);
      MEMSET((void *)&pme->pgLetterListData, 0, sizeof(CIMPage));
      pme->pgLetterListData.nMaxItems = nNumCandidate;
   }
   nNumCandidate = APPROXIMATE_PER_PAGE;
   if( bPageBack ){
      wStart         = MAX(0,pme->pgLetterListData.wItemStart-APPROXIMATE_PER_PAGE);
      if( !wStart ){
         nNumCandidate  += (pme->pgLetterListData.wItemStart-APPROXIMATE_PER_PAGE);
         pme->pgLetterListData.wPage        = 0;
         pme->pgLetterListData.wItemStart   = 0;
      }

      if( pme->pgLetterListData.wPage ){
         pme->pgLetterListData.wPage--;
      }
   }else{
      pme->pgLetterListData.wItemStart += pme->pgLetterListData.wItemCount;
      wStart                           = pme->pgLetterListData.wItemStart;
      pme->pgLetterListData.wPage++;
   }

#ifdef PROFILE_IMUI_MENUCTL // {
   DBGPRINTF("%d\tGetSize", GETTIMEMS() - nProfile);
   nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
#ifdef PROFILE_IMUI_MENUCTL // {
   DBGPRINTF("%d\tAlloc/Copy", GETTIMEMS() - nProfile);
   nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
   // Get all candidates to max! 
   // Change here if you want a shorter list, or initialise that object for smaller default.
   // IMGRO_MAXCANDIDATE:64
   if( !IIMMGR_GetCompositionCandidates(pme->pIMMgr, pme->szwLetterComp, sizeof(pme->szwLetterComp), wStart, &nNumCandidate, NULL) ){
      uint16      wItems   = (uint16)nNumCandidate;
      CtlAddItem  ai;
      MEMSET((void *)&ai, 0, sizeof(CtlAddItem));

#ifdef PROFILE_IMUI_MENUCTL // {
      DBGPRINTF("%d\tGetList", GETTIMEMS() - nProfile);
      nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
      // Set up the Composition menu
      IMENUCTL_DeleteAll(pme->pSKLetterComp);
      pszwItem = pme->szwLetterComp;
#ifdef PROFILE_IMUI_MENUCTL // {
      DBGPRINTF("%d\tDeleteMenu", GETTIMEMS() - nProfile);
      nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
      // Measure the page, so we can only add what is needed
      IMUI_MeasureMenuPage(pme, (IControl *)pme->pSKLetterComp, &pszwItem, &wItems, bPageBack);
#ifdef PROFILE_IMUI_MENUCTL // {
      DBGPRINTF("%d\tMeasureMenu", GETTIMEMS() - nProfile);
      nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
      // This is the count of items living in this page, the next page would start there
      pme->pgLetterListData.wItemCount   = wItems;
      for( i = 0; i < wItems; i++ ){
         ai.pText    = pszwItem;
         ai.wItemID  = i;
         ai.wFont    = (uint16)pme->fntLetterComp;
         IMENUCTL_AddItemEx(pme->pSKLetterComp, &ai);
         pszwItem += WSTRLEN(pszwItem)+1;
      }
      if( bPageBack ){
         if( pme->pgLetterListData.wItemStart ){
            pme->pgLetterListData.wItemStart   -= wItems;
         }
         IMENUCTL_SetSel(pme->pSKLetterComp, (uint16)--i);
      }
#ifdef PROFILE_IMUI_MENUCTL // {
      DBGPRINTF("%d\tBuildMenu", GETTIMEMS() - nProfile);
      nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
   }
#ifdef PROFILE_IMUI_MENUCTL // {
   DBGPRINTF("%u\tEnd", nProfile);
   DBGPRINTF("---------------------\n");
#endif // } PROFILE_IMUI_MENUCTL
   return AEE_SUCCESS;
}
static int IMUI_RecalcCharSK(CIMUI * pme, boolean bPageBack)
{
   int         nNumCandidate  = 0,
               nMaxChars      = 0,
               nRet           = EFAILED,
               i;
   AECHAR *    pszwItem;
   CtlAddItem  ai;
   uint16      wStart;
#ifdef PROFILE_IMUI_MENUCTL // {
   uint32 nProfile   = GETTIMEMS();

   DBGPRINTF("---------------------");
   DBGPRINTF("RecalcCharSK");
   DBGPRINTF("%d\tInit", nProfile);
#endif // } PROFILE_IMUI_MENUCTL
   // Get the item selected in the compositional menu
   // This is the seed for word or word association candidates to be queried
   if( IMENUCTL_GetItem(pme->pSKLetterComp, IMENUCTL_GetSel(pme->pSKLetterComp), &ai) 
      && (ai.pText && *ai.pText) ){
#ifdef PROFILE_IMUI_MENUCTL // {
      DBGPRINTF("%d\tGetItem", GETTIMEMS() - nProfile);
      nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
      // See if our buffer is big enough to hold the compositional data.
      // It should always be, but it isn't guarenteed.
      // TODO: check length overflow
      WSTRCPY(pme->szwCharList, ai.pText);
      // See how many candidates they're prepared to give us!
      nNumCandidate = nMaxChars = 0;
#ifdef PROFILE_IMUI_MENUCTL // {
      DBGPRINTF("%d\tItemAlloc/Copy", GETTIMEMS() - nProfile);
      nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL

      if( !pme->pgCharListData.nMaxItems ){

         // Get Count!!!
         nNumCandidate = 0;
         if( pme->bWordAssoc ){
            IIMMGR_GetAssociatedCandidates(pme->pIMMgr, pme->szwCharList, 0, 0, &nNumCandidate, NULL);
            // If no associated candidates, must allow caller to finish
            if( !nNumCandidate ){
               return EFAILED;
            }
         }else{
            IIMMGR_GetCompleteCandidates(pme->pIMMgr, pme->szwCharList, 0, 0, &nNumCandidate, NULL);
         }
         MEMSET((void *)&pme->pgCharListData, 0, sizeof(CIMPage));
         pme->pgCharListData.nMaxItems = nNumCandidate;
      }
      nNumCandidate = APPROXIMATE_PER_PAGE;
      if( bPageBack ){
         wStart         = MAX(0,pme->pgCharListData.wItemStart-APPROXIMATE_PER_PAGE);
         if( !wStart ){
            nNumCandidate  += (pme->pgCharListData.wItemStart-APPROXIMATE_PER_PAGE);
            pme->pgCharListData.wPage        = 0;
            pme->pgCharListData.wItemStart   = 0;
         }
         if( pme->pgCharListData.wPage ){
            pme->pgCharListData.wPage--;
         }
      }else{
         pme->pgCharListData.wItemStart   += pme->pgCharListData.wItemCount;
         wStart                           = pme->pgCharListData.wItemStart;
         pme->pgCharListData.wPage++;
      }
#ifdef PROFILE_IMUI_MENUCTL // {
      DBGPRINTF("%d\tGetSize", GETTIMEMS() - nProfile);
      nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
      // Now get them!
      if( pme->bWordAssoc ){
         nRet = IIMMGR_GetAssociatedCandidates(pme->pIMMgr, pme->szwCharList, sizeof(pme->szwCharList), wStart, &nNumCandidate, NULL);
      }else{
         nRet = IIMMGR_GetCompleteCandidates(pme->pIMMgr, pme->szwCharList, sizeof(pme->szwCharList), wStart, &nNumCandidate, NULL);
      }
      if( !nRet ){
         uint16      wItems   = (uint16)nNumCandidate;
         CtlAddItem  ai;

         MEMSET((void *)&ai, 0, sizeof(CtlAddItem));
         // Set up the menu now!
#ifdef PROFILE_IMUI_MENUCTL // {
         DBGPRINTF("%d\tGetList", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
         IMENUCTL_DeleteAll(pme->pSKCharacterList);
         pszwItem = pme->szwCharList;
#ifdef PROFILE_IMUI_MENUCTL // {
         DBGPRINTF("%d\tDeleteMenu", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
         // Measure the page, so we can only add what is needed
         IMUI_MeasureMenuPage(pme, (IControl *)pme->pSKCharacterList, &pszwItem, &wItems, bPageBack);
#ifdef PROFILE_IMUI_MENUCTL // {
         DBGPRINTF("%d\tMeasureMenu", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
         // This is the count of items living in this page, the next page would start there
         pme->pgCharListData.wItemCount   = wItems;
         for( i = 0; i < wItems; i++ ){
            ai.pText    = pszwItem;
            ai.wItemID  = i;
            ai.wFont    = (uint16)pme->fntCharList;
            IMENUCTL_AddItemEx(pme->pSKCharacterList, &ai);
            pszwItem += WSTRLEN(pszwItem)+1;
         }
         if( bPageBack ){
            if( pme->pgCharListData.wItemStart ){
               pme->pgCharListData.wItemStart   -= wItems;
            }
            IMENUCTL_SetSel(pme->pSKCharacterList, (uint16)--i);
         }
#ifdef PROFILE_IMUI_MENUCTL // {
         DBGPRINTF("%d\tBuildMenu", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMUI_MENUCTL
      }
   }
#ifdef PROFILE_IMUI_MENUCTL // {
   DBGPRINTF("%d\tEnd", nProfile);
   DBGPRINTF("---------------------\n");
#endif // } PROFILE_IMUI_MENUCTL
   return AEE_SUCCESS;
}
// pnItems [IN/OUT] IN: Items in pBuff, OUT: Items fit on page
// ppBuff [IN/OUT] IN: Full words array, OUT: Repositioned to first word on page [when page back is asked]
static int IMUI_MeasureMenuPage(CIMUI * pme, IControl * pList, AECHAR ** ppBuff, uint16 * pwItems, boolean bPageBack)
{
   uint16   i,
            nTally      = 0,
            wMaxItem    = 0,
            wMaxCalc    = 0,
            wItems      = *pwItems,
            wSKEnd;
   AECHAR * pItem       = *ppBuff;
   AEERect  rct;
   AEEFont  fnt;
   int      nXpadSel, nXpadNon;

   ICONTROL_GetRect(pList, &rct);
   if( pList == (IControl *)pme->pSKCharacterList ){
      fnt      = pme->fntCharList;
      // Get the XPadding for sel items and non sel items
      if( CHARSK_ACTIVE(pme) ){
         nXpadSel = pme->isStyles[IUIMS_BOT_ACTIVE_SEL].xOffset;
         nXpadNon = pme->isStyles[IUIMS_BOT_ACTIVE_UNSEL].xOffset;
      }else{
         nXpadSel = pme->isStyles[IUIMS_BOT_INACTIVE_SEL].xOffset;
         nXpadNon = pme->isStyles[IUIMS_BOT_INACTIVE_UNSEL].xOffset;
      }
   }else{
      fnt      = pme->fntLetterComp;
      // Get the XPadding for sel items and non sel items
      if( LETTERSK_ACTIVE(pme) ){
         nXpadSel = pme->isStyles[IUIMS_TOP_ACTIVE_SEL].xOffset;
         nXpadNon = pme->isStyles[IUIMS_TOP_ACTIVE_UNSEL].xOffset;
      }else{
         nXpadSel = pme->isStyles[IUIMS_TOP_INACTIVE_SEL].xOffset;
         nXpadNon = pme->isStyles[IUIMS_TOP_INACTIVE_UNSEL].xOffset;
      }
   }
   if( wItems > 1 ){
      nTally   = (rct.x + 5); // border + arrow + space
      wSKEnd   = (rct.x + rct.dx) - 5;
   }else{
      nTally   = rct.x;
      wSKEnd   = (rct.x + rct.dx);
   }
   if( bPageBack ){
      AECHAR * lpszwWalker = pItem,
             * lpszwSave   = pItem;
      for( i = 0; i < (wItems-1); i++ ){
         lpszwWalker += WSTRLEN(lpszwWalker)+1;
      }

      for( i = 0; i < wItems; i++ ){
         // Border space on each side
         wMaxCalc = (1<<1);
         // PLUS XPadding on each side
         // 1 item has to be selected always for this menu
         // so use the xpad sel value first, then move to unsel xpad
         wMaxCalc += (((i==0)?nXpadSel:nXpadNon)<<1);
         wMaxCalc += IDISPLAY_MeasureText(pme->pDisplay, fnt, pItem);
         nTally   += wMaxCalc;

         // This max calculation is for MP_MAXSOFTKEYITEMS
         if( wMaxCalc > wMaxItem ){
            wMaxItem = wMaxCalc;
         }
         if( nTally > wSKEnd 
            || ((i+1)*wMaxItem) > rct.dx ){
            *ppBuff = lpszwSave;
            break;
         }

         lpszwSave   = lpszwWalker;
         lpszwWalker -= 2;
         while( *lpszwWalker ){
            lpszwWalker--;
         }
         lpszwWalker++;
      }
   }else{
      for( wMaxCalc = wMaxItem = i = 0; i < wItems; i++ ){

         // Border space on each side
         wMaxCalc = (1<<1);
         // PLUS XPadding on each side
         // 1 item has to be selected always for this menu
         // so use the xpad sel value first, then move to unsel xpad
         wMaxCalc += (((i==0)?nXpadSel:nXpadNon)<<1);
         wMaxCalc += IDISPLAY_MeasureText(pme->pDisplay, fnt, pItem);
         nTally   += wMaxCalc;

         // This max calculation is for MP_MAXSOFTKEYITEMS
         if( wMaxCalc > wMaxItem ){
            wMaxItem = wMaxCalc;
         }
         if( nTally > wSKEnd 
            || ((i+1)*wMaxItem) > rct.dx ){
            break;
         }

         pItem += WSTRLEN(pItem)+1;
      }
   }
   *pwItems = i;

   return AEE_SUCCESS;
}
static int IMUI_CalcRectHeight(CIMUI * pme)
{
   int         nRectHeight;
   AEEFont     fntOne   = pme->fntLetterComp,
               fntTwo   = pme->fntCharList;
   int         nFrameTop = 0, nFrameBot = 0,
               nPadTop = 0, nPadBot = 0,
               i, nTmp;

   // See how big the full rect of both SK are
   nRectHeight = IDISPLAY_GetFontMetrics(pme->pDisplay, fntOne, NULL, NULL);
   if( fntOne == fntTwo ){
      nRectHeight <<= 1; // Need 2 lines of text
   }else{
      nRectHeight += IDISPLAY_GetFontMetrics(pme->pDisplay, fntTwo, NULL, NULL);
   }

   // Get offset affecting values
   // Top
   for( i = 0; i < (ARRAY_SIZE(pme->isStyles)>>1); i++ ){
      nTmp = IDISPLAY_DrawFrame(pme->pDisplay, NULL, pme->isStyles[i].ft, RGB_NONE);
      if( nTmp > nFrameTop ){
         nFrameTop = nTmp;
      }
      if( pme->isStyles[i].yOffset > nPadTop ){
         nPadTop = pme->isStyles[i].yOffset;
      }
   }
   // Bottom
   for( i = 4; i < (ARRAY_SIZE(pme->isStyles)); i++ ){
      nTmp = IDISPLAY_DrawFrame(pme->pDisplay, NULL, pme->isStyles[i].ft, RGB_NONE);
      if( nTmp > nFrameBot ){
         nFrameBot = nTmp;
      }
      if( pme->isStyles[i].yOffset > nPadBot ){
         nPadBot = pme->isStyles[i].yOffset;
      }
   }
   // If the border is wanted the nRectHeight should be updated
   // ((checkPad SK top and SK bottom) for each SK) PLUS ((checkFrame SK top and SK bottom) for each SK)
   // PLUS ((1Pad SK Border Top and SK bottom)*2SK)
   // optionally PLUS (3D Border or Border?)
   nRectHeight += (nPadTop<<1) + (nPadBot<<1) + (nFrameTop<<1) + (nFrameBot<<1) + ((1<<1)<<1);
   if( pme->dwProps & CP_BORDER || pme->dwProps & CP_3D_BORDER ){
      nRectHeight += ((pme->dwProps & CP_BORDER) ?  2 : 4);
   }

   if( pme->dwProps & IUIP_ALIGNBOTTOM ){
      pme->rctMe.y   = ((pme->rctMe.y + pme->rctMe.dy) - nRectHeight);
      pme->rctMe.dy  = nRectHeight;
   }else{
      pme->rctMe.dy  = nRectHeight;
   }
   OEMIMUI_SetRect((IIMUI *)pme, &pme->rctMe);
   return AEE_SUCCESS;
}
static int IMUI_SetActiveSK(CIMUI * pme, IControl * pActive, boolean bRedraw)
{
   pme->pActiveCtl   = pActive;
   // Set colors and styles as appropriate
   if( LETTERSK_ACTIVE(pme) ){
      IMENUCTL_SetColors(pme->pSKLetterComp, &pme->mcColors[LETTERCOLOR_ACTIVE]);
      IMENUCTL_SetColors(pme->pSKCharacterList, &pme->mcColors[CHARCOLOR_INACTIVE]);

      IMENUCTL_SetStyle(pme->pSKLetterComp, &pme->isStyles[IUIMS_TOP_ACTIVE_UNSEL], &pme->isStyles[IUIMS_TOP_ACTIVE_SEL]);
      IMENUCTL_SetStyle(pme->pSKCharacterList, &pme->isStyles[IUIMS_BOT_INACTIVE_UNSEL], &pme->isStyles[IUIMS_BOT_INACTIVE_SEL]);
   }else{
      IMENUCTL_SetColors(pme->pSKLetterComp, &pme->mcColors[LETTERCOLOR_INACTIVE]);
      IMENUCTL_SetColors(pme->pSKCharacterList, &pme->mcColors[CHARCOLOR_ACTIVE]);

      IMENUCTL_SetStyle(pme->pSKLetterComp, &pme->isStyles[IUIMS_TOP_INACTIVE_UNSEL], &pme->isStyles[IUIMS_TOP_INACTIVE_SEL]);
      IMENUCTL_SetStyle(pme->pSKCharacterList, &pme->isStyles[IUIMS_BOT_ACTIVE_UNSEL], &pme->isStyles[IUIMS_BOT_ACTIVE_SEL]);
   }
   if( bRedraw ){
      IMENUCTL_Redraw(pme->pSKLetterComp);
      IMENUCTL_Redraw(pme->pSKCharacterList);
   }

   return AEE_SUCCESS;
}
static int IMUI_InitDefaultColors(CIMUI * pme)
{
   pme->mcColors[LETTERCOLOR_ACTIVE].wMask            = MC_BACK|MC_TEXT|MC_SEL_BACK|MC_SEL_TEXT|MC_FRAME|MC_SCROLLBAR|MC_SCROLLBAR_FILL|MC_TITLE|MC_TITLE_TEXT;
   pme->mcColors[LETTERCOLOR_ACTIVE].cBack            = CLR_SYS_ITEM;
   pme->mcColors[LETTERCOLOR_ACTIVE].cText            = CLR_SYS_ITEM_TEXT;
   pme->mcColors[LETTERCOLOR_ACTIVE].cSelBack         = CLR_SYS_ITEM_SEL;
   pme->mcColors[LETTERCOLOR_ACTIVE].cSelText         = CLR_SYS_ITEM_SEL_TEXT;
   pme->mcColors[LETTERCOLOR_ACTIVE].cFrame           = CLR_SYS_FRAME_LO;
   pme->mcColors[LETTERCOLOR_ACTIVE].cScrollbar       = CLR_SYS_SCROLLBAR;
   pme->mcColors[LETTERCOLOR_ACTIVE].cScrollbarFill   = CLR_SYS_SCROLLBAR_FILL;
   pme->mcColors[LETTERCOLOR_ACTIVE].cTitle           = CLR_SYS_TITLE;
   pme->mcColors[LETTERCOLOR_ACTIVE].cTitleText       = CLR_SYS_TITLE_TEXT;
   MEMCPY((void *)&pme->mcColors[CHARCOLOR_ACTIVE], (void *)pme->mcColors, sizeof(AEEMenuColors));
   MEMCPY((void *)&pme->mcColors[LETTERCOLOR_INACTIVE], (void *)pme->mcColors, sizeof(AEEMenuColors));
   pme->mcColors[LETTERCOLOR_INACTIVE].cSelBack       = CLR_SYS_ITEM;
   pme->mcColors[LETTERCOLOR_INACTIVE].cSelText       = CLR_SYS_ITEM_TEXT;
   MEMCPY((void *)&pme->mcColors[CHARCOLOR_INACTIVE], (void *)pme->mcColors, sizeof(AEEMenuColors));
   pme->mcColors[CHARCOLOR_INACTIVE].cSelBack         = CLR_SYS_ITEM;
   pme->mcColors[CHARCOLOR_INACTIVE].cSelText         = CLR_SYS_ITEM_TEXT;

   return AEE_SUCCESS;
}
// Local method - Used to process a selected candidate
// Returns TRUE when word association is linked
// Returns FALSE when no more words may be associated
static boolean IMUI_ProcessSelectedCandidate(CIMUI * pme)
{
   // Otherwise they've made a choice and it must be processed
   CtlAddItem  ai;
   // Get selected item
   IMENUCTL_GetItem(pme->pSKCharacterList, IMENUCTL_GetSel(pme->pSKCharacterList), &ai);
   // Check if we are still editing
   if( IIMMGR_IsCompleteCandidate(pme->pIMMgr, ai.pText) ){
      AECHAR *    pWords = NULL;
      CtlAddItem  aiAdd;
      MEMSET((void *)&aiAdd, 0, sizeof(CtlAddItem));
      // TODO: Word association

      // Append the character to the top menu
      aiAdd.wFont    = (uint16)pme->fntLetterComp;
      aiAdd.wItemID  = 0;
      if( !pme->bWordAssoc ){
         // First time need to get rid of the compositional
         // materials as the top menu transforms to known
         // associations of words, then use the only words
         // chosen to associate with
         IMENUCTL_DeleteAll(pme->pSKLetterComp);
         aiAdd.pText = ai.pText;
      }else{
         CtlAddItem  aiOld;
         // If in WA, there should be a menu item already
         if( !IMENUCTL_GetItem(pme->pSKLetterComp, 0, &aiOld) ){
            // Never come here
            IIMMGR_SetData(pme->pIMMgr, ai.pText);
            return FALSE; 
         }
         // Get room for already associated words and new word
         pWords = (AECHAR *)MALLOC( (WSTRLEN(aiOld.pText)+WSTRLEN(ai.pText)+1)*sizeof(AECHAR) );
         if( !pWords ){
            // Can't associate, no memory
            IIMMGR_SetData(pme->pIMMgr, ai.pText);
            return FALSE;
         }
         WSTRCPY(pWords, aiOld.pText);
         WSTRCAT(pWords, ai.pText);
         aiAdd.pText = pWords;
         IMENUCTL_DeleteAll(pme->pSKLetterComp);
      }
      IMENUCTL_AddItemEx(pme->pSKLetterComp, &aiAdd);
      if( pWords ){
         FREE( pWords );
      }
      IMENUCTL_SetSel(pme->pSKLetterComp, 0);

      // Work with the bottom menu to process completed candidates
      // Now it is word associating, this flag will notify the recalc
      // to do the right thing.
      pme->bWordAssoc               = TRUE;
      pme->pgCharListData.nMaxItems = 0;
      if( IMUI_RecalcCharSK(pme, FALSE) != AEE_SUCCESS ){
         IMENUCTL_GetItem(pme->pSKLetterComp, 0, &aiAdd);
         // No associated words, so ...
         pme->bWordAssoc = FALSE;
         IIMMGR_SetData(pme->pIMMgr, aiAdd.pText);
         return FALSE;
      }
      (void)IMUI_SetActiveSK(pme, (IControl *)pme->pSKCharacterList, TRUE);

      return TRUE;
   }else{
      // Still editing, so bring the character to the top menu
      // set focus there and get new choices based on this selection
      IMENUCTL_DeleteAll(pme->pSKLetterComp);
      ai.wFont    = (uint16)pme->fntLetterComp;
      ai.wItemID  = 0;
      IMENUCTL_AddItemEx(pme->pSKLetterComp, &ai);
      IMENUCTL_SetSel(pme->pSKLetterComp, 0);
      pme->pgCharListData.nMaxItems = 0;
      (void)IMUI_RecalcCharSK(pme, FALSE);

      (void)IMUI_SetActiveSK(pme, (IControl *)pme->pSKLetterComp, TRUE);
   }
   return TRUE;
}

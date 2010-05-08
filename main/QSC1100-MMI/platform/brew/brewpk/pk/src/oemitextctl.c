
/*===========================================================================
                              OEMITextCtl.c

  ITextCtl interface Implementation
  
  This file contains implementation of the ITextCtl BREW interface
    
        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

//=====================================================================
// INCLUDES AND VARIABLE DEFINITIONS
//=====================================================================


//---------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------
#include "OEMFeatures.h"

#include "OEMText.h"
#include "AEEStdLib.h"
#include "AEEClassIDs.h"
#include "AEEControls_res.h"
#include "AEEPointerHelpers.h"

/*=====================================================================

                      PUBLIC DATA DECLARATIONS

=====================================================================*/
//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------


#define DEF_TEXT_SIZE      32       //  Default text allocation size...
#define MAX_TEXT_SIZE      256

// ID of Menu Item for displaying different Text Modes
#define IDC_TEXT_MODE      IDC_COMMAND_RESERVED+1

// Base ID of Menu Item IDs that this control can use
#define IDC_TEXT_MODE_BASE IDC_TEXT_MODE+1

// Symbol dialog padding constants
#define SYMDLG_X_ITEMBASE  (0x1)
#define SYMDLG_X_IDXPAD    (0x1)
#define SYMDLG_X_IDXAPAD   (0x2)
#define SYMDLG_X_ARROWPOS  (0x1)
#define SYMDLG_Y_ARROWPOS  (0x0)

// Pen tracking bit flags
#define  PTRCK_HIT_ABOVE   (0x01)      // Above the thumb, y position < thumb.y OR x position < thumb.x
#define  PTRCK_HIT_THUMB   (0x02)      // On the thumb
#define  PTRCK_HIT_ITEM    (0x02)      // Matches above, overloaded for non SB entities
#define  PTRCK_HIT_BELOW   (0x04)      // Below the thumb, y position > thumb.y + thumb.dy OR x position < thumb.x + thumb.dx
#define  PTRCK_HIT_SCRL    (0x08)      // 1: Hit Scrollbar; 0: Didn't hit Scrollbar
#define  PTRCK_NOTTHUMB    (PTRCK_HIT_BELOW | PTRCK_HIT_ABOVE)

// Pen Tracking General bit flags
#define  PTRCK_GEN_TMRSET  (0x01)   // Timer is set

typedef struct
{
   uint8    cbHit;
   uint8    cbFlags;
   uint16   wData;
   AEEPoint ptPosition;
} PenTracker;

#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

OBJECT(CTextCtl)
{
   DECLARE_VTBL(ITextCtl)

   // private object data
   long                       m_nRefs;                // reference count
   flg                        m_bActive:1;            // control is active flag
   flg                        m_bSendCmd:1;
   IMenuCtl *                 m_pSoftKey;
   IMenuCtl *                 m_pModeMenu;            // Popup - Text Mode
   int                        m_nCmdId;
   int                        m_nSymbolPage;
   boolean                    m_bShowSyms;
   AECHAR                     m_chEntry;
   AECHAR                     m_chShift;
   uint32                     m_dwProps;
   int                        m_nFontHeight;          // height of char cell of menu font
   uint16                     m_nMaxChars;            // Maximum number of chars to input
   IDisplay *                 m_pIDisplay;            // IDisplay interface ptr
   AECHAR *                   m_pTitle;               // title
   IShell *                   m_pIShell;              // shell interface ptr
   AEERect                    m_rc;                   // requested rectangle (Set)
   AEERect                    m_rcGet;                // actual rectangle (Get)
   OEMCONTEXT                 m_pText;
   AEETextInputMode           m_tmLast;               // Mode ID of previously selected Mode
   int                        m_dyTextMax;
   uint16                     m_nModeEnum;            // Internal BREW mode iterator
   AEEBitmapInfo              m_bi;                   // info for destination bitmap
   PenTracker                 m_ptTracker;
   uint32                     m_clsMe;                // My Class ID
   uint32                     m_dwPropsEx;            // Not used yet.
};

#define LINEHEIGHT            (pme->m_nFontHeight)

//---------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------
static uint32           CTextCtl_AddRef(ITextCtl * pITextCtl);
static uint32           CTextCtl_Release(ITextCtl * pITextCtl);
static boolean          CTextCtl_HandleEvent(ITextCtl * pITextCtl, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean          CTextCtl_Redraw(ITextCtl * pITextCtl);
static void             CTextCtl_SetActive(ITextCtl * pITextCtl, boolean bActive);
static boolean          CTextCtl_IsActive(ITextCtl * pITextCtl);
static void             CTextCtl_SetRect(ITextCtl * pITextCtl, const AEERect * prc);
static void             CTextCtl_GetRect(ITextCtl * pITextCtl, AEERect * prc);
static void             CTextCtl_SetProperties(ITextCtl * pITextCtl, uint32 nProperties);
static uint32           CTextCtl_GetProperties(ITextCtl * pITextCtl);
static void             CTextCtl_Reset(ITextCtl * pITextCtl);
static boolean          CTextCtl_SetTitle(ITextCtl * pITextCtl, const char * pszResFile, uint16 wResID, AECHAR * pText);
static boolean          CTextCtl_SetText(ITextCtl * pITextCtl, const AECHAR * psz, int cch);
static boolean          CTextCtl_GetText(ITextCtl * pITextCtl, AECHAR * pBuffer, int nMaxChars);
static AECHAR *         CTextCtl_GetTextPtr(ITextCtl * pITextCtl);
static void             CTextCtl_EnableCommand(ITextCtl * pITextCtl, boolean bEnable, uint16 nCmdId);
static void             CTextCtl_SetMaxSize(ITextCtl * pITextCtl, uint16 nMaxSize);
static void             CTextCtl_SetSoftKeyMenu(ITextCtl * pITextCtl,IMenuCtl * pm);
static AEETextInputMode CTextCtl_SetInputMode(ITextCtl * po, AEETextInputMode m);
static int32            CTextCtl_GetCursorPos(ITextCtl * po);
static void             CTextCtl_SetCursorPos(ITextCtl * po, int32 nOffset);
static AEETextInputMode CTextCtl_GetInputMode(ITextCtl * po, AEETextInputModeInfo * pmInfo);
static void             CTextCtl_EnumModeInit(ITextCtl *po);
static AEETextInputMode CTextCtl_EnumNextMode(ITextCtl *po, AEETextInputModeInfo * pmInfo);

// Text Control 2 API
static void             CTextCtl_SetSelection(ITextCtl * po, uint32 dwSel);
static uint32           CTextCtl_GetSelection(ITextCtl * po);
static void             CTextCtl_SetPropertiesEx(ITextCtl * pITextCtl, uint32 nPropsEx);
static uint32           CTextCtl_GetPropertiesEx(ITextCtl * pITextCtl);

static void     TextCtl_SetMode              (ITextCtl * pITextCtl, 
                                              boolean bEdit, 
                                              AEETextInputMode mode, 
                                              boolean bRedraw
                                              );

static void     TextCtl_ShowSymbolDialog     (CTextCtl * pme);
static void     TextCtl_ShowModeMenu         (CTextCtl * pme);
static void     TextCtl_CheckEntryMode       (CTextCtl * pme);
static void     TextCtl_GetSymbDlgInfo       (IDisplay * pd, AEEBitmapInfo * pbi, AECHAR * pszwBuff, uint16 wBuffSize, int * pcxAdd, int * pcyAdd, int16 * pwDx, int16 * pwDy);
static void     TextCtl_ShowSymbolPage       (CTextCtl * pme, int nDir);
static boolean  TextCtl_SymbolHandlePen      (CTextCtl * pme, AEEEvent evtCode, int16 wXPos, int16 wYPos);
static uint8    TextCtl_SymbolPagePenHitItem (CTextCtl * pme, int16 wXPos, int16 wYPos, uint16 * pwData);
static boolean  TextCtl_IsSymbolsSupported   (CTextCtl * pme);
static void     TextCtl_KillModeMenu         (CTextCtl * pme);
static void     TextCtl_ClearScreen          (CTextCtl * pme);
static AECHAR   TextCtl_GetSymChar           (CTextCtl * pme, int nPos);

static void     TextCtl_ReturnToEditMode     (CTextCtl * pme, 
                                              AEETextInputMode mode
                                              );

static void     TextCtl_CloseSymbolDialog    (CTextCtl * pme, AECHAR ch);

static void     TextCtl_CalcRects            (CTextCtl * pme);
static void     TextCtl_Delete               (CTextCtl * pme);

static boolean  TextCtl_SetText              (CTextCtl * pme, 
                                              boolean bSetText, 
                                              uint16 wID, 
                                              AECHAR * pText
                                             );

static boolean  TextCtl_HandlePens(CTextCtl * pme, AEEEvent evtCode, uint16 wParam, uint32 dwParam);


//---------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------
static const AEETextInputModeInfo sBREWTextModes[] =
{
   {AEE_TM_SYMBOLS, {'S','y','m','b','o','l','s',0}},
   {AEE_TM_RESERVE_LAST, {0}}
};
//---------------------------------------------------------------------
// Global Data Definitions
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------
// VTBL for ITimeCtl

static const VTBL(ITextCtl) gQTextMethods = {CTextCtl_AddRef,
                                             CTextCtl_Release,
                                             CTextCtl_HandleEvent,
                                             CTextCtl_Redraw,
                                             CTextCtl_SetActive,
                                             CTextCtl_IsActive,
                                             CTextCtl_SetRect,
                                             CTextCtl_GetRect,
                                             CTextCtl_SetProperties,
                                             CTextCtl_GetProperties,
                                             CTextCtl_Reset,
                                             CTextCtl_SetTitle,
                                             CTextCtl_SetText,
                                             CTextCtl_GetText,
                                             CTextCtl_GetTextPtr,
                                             CTextCtl_EnableCommand,
                                             CTextCtl_SetMaxSize,
                                             CTextCtl_SetSoftKeyMenu,
                                             CTextCtl_SetInputMode,
                                             CTextCtl_GetCursorPos,
                                             CTextCtl_SetCursorPos,
                                             CTextCtl_GetInputMode,
                                             CTextCtl_EnumModeInit,
                                             CTextCtl_EnumNextMode,
                                             CTextCtl_SetSelection,
                                             CTextCtl_GetSelection,
                                             CTextCtl_SetPropertiesEx,
                                             CTextCtl_GetPropertiesEx
                                             };

//=====================================================================
// FUNCTION DEFINITIONS
//=====================================================================
/*=================================================================
Function: TextCtl_New

Description: 
   This method is used to create and initialize CTextCtl object of 
   the specified identifier and get an ITextCtl interface to that.

Prototype:
   boolean TextCtl_New(IShell* pIShell, 
                  ITextCtl** ppobj, 
                  AEECLSID clsID)

Parameter(s):
   pIShell: pointer to IShell interface to shell
   ppobj: place holder for ITextCtl interface
   clsID: AEECLSID_TEXTCTL

Return Value: 
   If successful, TRUE.
   Otherwise FALSE.

Comments: none

Side Effects: none

See Also: none
=================================================================*/
int TextCtl_New(IShell * pIShell, AEECLSID clsID, void ** ppobj)
{
   CTextCtl *     pme = NULL;
   IBitmap *      pbmDst;
   int            nErr;

   *ppobj = NULL;

  // allocate memory for object

   if((pme = (CTextCtl *)(MALLOC(sizeof(CTextCtl)))) == NULL)
      return(ENOMEMORY);

   INIT_VTBL(pme, ITextCtl, gQTextMethods);
   IBASE_AddRef((IBase *)pme);

  // initialize object data

   pme->m_pIShell = pIShell;
   pme->m_tmLast = AEE_TM_CURRENT;
   pme->m_nModeEnum = 0;
   pme->m_clsMe = clsID;

   // Increment the reference count on the shell...

   ISHELL_AddRef(pIShell);

  // create IDisplay interface

   nErr = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_DISPLAY, (void **)&pme->m_pIDisplay);
   if(!pme->m_pIDisplay){
      TextCtl_Delete(pme);
      return(nErr);
   }

   pbmDst = IDISPLAY_GetDestination(pme->m_pIDisplay);
   if (NULL == pbmDst) {
      TextCtl_Delete(pme);
      return EFAILED;
   }

   nErr = IBITMAP_GetInfo(pbmDst, &pme->m_bi, sizeof(pme->m_bi));
   IBITMAP_Release(pbmDst);
   if (SUCCESS != nErr) {
      TextCtl_Delete(pme);
      return nErr;
   }

   // Set the default size

   pme->m_nMaxChars = DEF_TEXT_SIZE;

   // obtain font metrics for later use

   pme->m_nFontHeight = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);

   // get display metrics

   pme->m_dyTextMax = pme->m_bi.cy - LINEHEIGHT;
   SETAEERECT(&pme->m_rc, 0, 0, pme->m_bi.cx, pme->m_dyTextMax);

   MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));

   TextCtl_CalcRects(pme);

   *ppobj = (ITextCtl*) pme;

   return(0);
}

/*=====================================================================

Function:  CTextCtl_AddRef()

Description:
    This function increments the reference count for the text control object

Prototype:
   uint32 CTextCtl_AddRef(ITextCtl * pITextCtl)

Parameters:
    pITextCtl: Pointer to the ITextCtl interface whose reference count needs 
              to be incremented.

Return Value:
    The updated reference count

Comments: None

Side Effects: None

======================================================================*/
static uint32 CTextCtl_AddRef(ITextCtl * pITextCtl)
{
   return(++((CTextCtl*)pITextCtl)->m_nRefs);
}

/*======================================================================

Function:  CTextCtl_Release()

Description:
   This function decrements the reference count for the text control object 
   and does appropriate cleanup if the reference count reaches zero.

Prototype:
   uint32 CTextCtl_Release(ITextCtl * pITextCtl)

Parameters:
    pITextCtl: Pointer to the ITextCtl interface whose reference count needs to 
              be decremented.

Return Value:
   The updated reference count

Comments:  None

Side Effects: None

=====================================================================*/
static uint32 CTextCtl_Release(ITextCtl *pITextCtl)
{
   CTextCtl *pme = (CTextCtl *) pITextCtl;

   if (--pme->m_nRefs != 0)
      return(pme->m_nRefs);
   
   TextCtl_Delete(pme);

   return(0);
}

/*=====================================================================

Function:  CTextCtl_HandleEvent()

Description: 
   This function is used by text control object to handle events received 
   by it.

Prototype:

   // C Users
   boolean CTextCtl_HandleEvent(ITextCtl * pITextCtl, AEEEvent evt, uint16 wp, uint32 dwp);

   // C++ Users
   boolean CTextCtl::HandleEvent(AEEEvent evt, uint16 wp, uint32 dwp);

Parameters:
   pITextCtl: Pointer to the text control object
   evt: Event code
   wp: 16-bit event data
   dwp: 32-bit event data

Return Value:

   Returns TRUE if the event was processed by the text control.

Comments:  
Side Effects: None
See Also:

=====================================================================*/
static boolean CTextCtl_HandleEvent(ITextCtl * pITextCtl, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;
   uint32 mode;

   // First handle the pens, most time sensitive events we get
   if( eCode >= EVT_POINTER_DOWN && eCode <= EVT_POINTER_STALE_MOVE ){
      boolean bRet = FALSE;
      // Stale move is not handled and likely the largest stream of events
      // For speed, abort quick.
      if( eCode == EVT_POINTER_STALE_MOVE ){
         return FALSE;
      }
      // See if the Mode menu is displayed... let it handle the event first
      if (pme->m_pModeMenu != NULL) {
         if (IMENUCTL_HandleEvent(pme->m_pModeMenu,eCode,wParam,dwParam)) {
            return TRUE;
         }
      }
      if(pme->m_bShowSyms){
         int16 wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
         int16 wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);
         return TextCtl_SymbolHandlePen(pme, eCode, wXPos, wYPos);
      }
      if( (bRet = TextCtl_HandlePens(pme, eCode, wParam, dwParam)) == TRUE ){
         if (!(pme->m_dwProps & TP_NODRAW))
            OEM_TextUpdate(pme->m_pText);
         TextCtl_CheckEntryMode(pme);
         // This belongs above the Softkey!
         OEM_TextDrawIMEDlg(pme->m_pText);
         if (!(pme->m_dwProps & TP_NODRAW))
            IDISPLAY_Update(pme->m_pIDisplay);
      }
      return bRet;
   }
   // See if it was a command from the softkey menu...
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      case EVT_DIALOG_START:
      case EVT_DIALOG_END:
         {
            boolean bRet = OEM_TextDialogEvt(pme->m_pText, eCode, wParam, dwParam);
            if( bRet && eCode == EVT_DIALOG_END ){
               if (!(pme->m_dwProps & TP_NODRAW))
                  OEM_TextUpdate(pme->m_pText);
               TextCtl_CheckEntryMode(pme);
               if (!(pme->m_dwProps & TP_NODRAW))
                  IDISPLAY_Update(pme->m_pIDisplay);
            }
            return bRet;
         }

      case EVT_CTL_SET_TEXT:
      case EVT_CTL_SET_TITLE:
         return(TextCtl_SetText(pme, 
                                (boolean)(eCode == EVT_CTL_SET_TEXT), 
                                wParam, 
                                (AECHAR *)dwParam)
                               );
         
      case EVT_COMMAND:

         if(!pme->m_pSoftKey)
            return(FALSE);

         switch(wParam)
         {
            case IDC_TEXT_MODE:
               TextCtl_ShowModeMenu(pme);
               return(TRUE);

            default:
               if(wParam >= IDC_TEXT_MODE_BASE)
               {
                  //This is a Menu Item added by TextCtl. The ItemData contains
                  // the ID of the Mode.

                  IMENUCTL_GetItemData(pme->m_pModeMenu,wParam,&mode);

                  //mode indicates the new mode. 
                  if((AEETextInputMode)mode == AEE_TM_SYMBOLS)
                  {
                     if( TextCtl_IsSymbolsSupported(pme) ){
                        TextCtl_ShowSymbolDialog(pme);
                        ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TEXT_MODECHANGED, (uint16)mode, (uint32)pme);
                        return(TRUE);
                     }
                     else
                        return FALSE;
                  }
                  else
                  {
                     //Its an OEM defined mode
                     TextCtl_ReturnToEditMode(pme,(AEETextInputMode)mode);
                     return(TRUE);
                  }
               }
               else
                  return FALSE;

         }

   // FALL THRU!!!!!

      case EVT_KEY:
         if (wParam == AVK_UNDEFINED) {
            // This allows us to force an asynchronous update!
            TextCtl_CheckEntryMode(pme);
            if (!(pme->m_dwProps & TP_NODRAW))
               IDISPLAY_Update(pme->m_pIDisplay);
            return TRUE;
         }
      
   // See if the Mode menu is displayed...

         if (pme->m_pModeMenu != NULL) {
            if (IMENUCTL_HandleEvent(pme->m_pModeMenu,eCode,wParam,dwParam)) {
               return TRUE;
            } else if (AVK_CLR == wParam) {
               TextCtl_KillModeMenu(pme);
               IMENUCTL_SetActive(pme->m_pSoftKey, TRUE);
               CTextCtl_Redraw((ITextCtl *)pme);
               return TRUE;
            } else {
               return FALSE;
            }
         } 

   // Handle the UP/DOWN key to return us to edit mode/Send Tab event...
         if(pme->m_pSoftKey &&  IMENUCTL_IsActive(pme->m_pSoftKey)){
            if(wParam == AVK_UP){
               TextCtl_ReturnToEditMode(pme, pme->m_tmLast);
               return TRUE;
            } else if(wParam == AVK_DOWN){
               return ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)1, (uint32)pme);
            }
         }

   // See if we have the symbols showing...

         if(pme->m_bShowSyms){
            switch(wParam){
               case AVK_UP:
                  TextCtl_ShowSymbolPage(pme, -1);
                  break;

               case AVK_DOWN:
                  TextCtl_ShowSymbolPage(pme, 1);
                  break;

               case AVK_CLR:
                  TextCtl_CloseSymbolDialog(pme, 0);
                  return(TRUE);
               
               default:
                  if(wParam < AVK_1 || wParam > AVK_9)
                     return(FALSE);
                  TextCtl_CloseSymbolDialog(pme, TextCtl_GetSymChar(pme,(wParam - AVK_1)));
            }
            return(TRUE);
         }

   // If they pressed the select key, activate the menu and deactivate us...Before processing the
   //AVK_SELECT key ourselves, give it to the OEMs and see if they want to handle it. This is
   //specifically for multibyte character sets (Japanes) where pressing the SELECT key after
   //pressing a key is the means to complete the character in that language.

         //Do not process keys if text ctl is not active
         if(pme->m_pText && pme->m_bActive )
         {

            // CLEAR ALL will be passed in as EVT_KEY_HELD here
            // and requires no special handling

NormalKeyEvent:
            if(OEM_TextKeyPress(pme->m_pText,eCode,wParam,dwParam) != FALSE)
            {
               if (!(pme->m_dwProps & TP_NODRAW))
                  OEM_TextUpdate(pme->m_pText);
               TextCtl_CheckEntryMode(pme);
               // This belongs above the Softkey, sorry!
               OEM_TextDrawIMEDlg(pme->m_pText);
               if (!(pme->m_dwProps & TP_NODRAW))
                  IDISPLAY_Update(pme->m_pIDisplay);
               return(TRUE);
            }
         }
        // If they pressed the select key, activate the menu and deactivate us...Have the special
        // check for EVT_KEY_RELEASe because we might have come here as a result of the goto
        //statement in EVT_KEY_RELEASE
         if(eCode != EVT_KEY_RELEASE && (wParam == AVK_SELECT || wParam == AVK_DOWN) && pme->m_bActive && pme->m_pSoftKey ) {
            TextCtl_SetMode(pITextCtl, FALSE, AEE_TM_CURRENT, TRUE);
            IMENUCTL_SetSel(pme->m_pSoftKey, (uint16)-1);
            IMENUCTL_SetActive(pme->m_pSoftKey, TRUE);
            return(TRUE);
         }
         // Checks for Tabbing events, or CR
         if(eCode == EVT_KEY && pme->m_bActive && wParam == AVK_UP){
            return ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)0, (uint32)pme);
         }
         // If no Softkey and the Down was not handled this will generate a tabbing event.
         if(eCode == EVT_KEY && pme->m_bActive && wParam == AVK_DOWN){
            return ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)1, (uint32)pme);
         }
         break;
      case EVT_CHAR:
      case EVT_UPDATECHAR:
         if( pme->m_bActive ){
            if( wParam == (uint16)0x08 ){
               // Backspace char will do same as CLR key
               CTextCtl_HandleEvent(pITextCtl, EVT_KEY, (uint16)AVK_CLR, 0);
            }
            else{
               OEM_TextAddChar(pme->m_pText, (AECHAR)wParam, FALSE);
               if( eCode == EVT_UPDATECHAR ) // Updating char, keep it selected
                  OEM_TextSetSel(pme->m_pText, OEM_TextGetCursorPos(pme->m_pText)-1, OEM_TextGetCursorPos(pme->m_pText));
               if (!(pme->m_dwProps & TP_NODRAW)){
                  OEM_TextUpdate(pme->m_pText);
                  IDISPLAY_Update(pme->m_pIDisplay);
               }
            }
            return TRUE;
         }
         break;
      case EVT_KEY_RELEASE:

   // This will help us to handle the '*' key, etc.

         if(pme->m_pText && pme->m_bActive)
            goto NormalKeyEvent;
         break;
                  
   }
   return(FALSE);   // did not handle the event
}

/*===================================================================== 

Function:  CTextCtl_Redraw()

Description: 
   This method instructs the text control object to redraw it's contents.
   The text control object does not redraw its contents every text the 
   underlying data behind the text control changes. This allows several data
   updates to occur while minimizing screen flashes. For example, several 
   changes can be made to the contents of the text control object with no 
   visible effect until the Redraw method is called.

Prototype:

   // C Users
   boolean CTextCtl_Redraw(ITextCtl * pITextCtl);

   // C++ Users
   boolean CTextCtl::Redraw();

Parameters:
   pITextCtl: Pointer to the text control object

Return Value:

   Returns TRUE if the text control was redrawn.

Comments:  
Side Effects: None
See Also:

=====================================================================*/ 
static boolean CTextCtl_Redraw(ITextCtl * pITextCtl)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;
   AEERect       qrc;

   if (!(pme->m_dwProps & TP_NODRAW)) {
      // clear the old
      IDISPLAY_FillRect(pme->m_pIDisplay, &pme->m_rc,CLR_SYS_WIN);
   
      // draw the title text
      if (pme->m_pTitle) {
         qrc = pme->m_rc;
         qrc.dy = pme->m_nFontHeight;
         IDISPLAY_DrawText(pme->m_pIDisplay, AEE_FONT_BOLD,pme->m_pTitle, -1, 
                           qrc.x, qrc.y,&qrc,IDF_RECT_FILL);
      }


      if(pme->m_pText) {
         OEM_TextDraw(pme->m_pText);
      }

      if (!(pme->m_dwProps & TP_NOUPDATE))
         IDISPLAY_Update(pme->m_pIDisplay);
   }

   return TRUE;
}

/*===================================================================== 

Function: CTextCtl_SetActive()

Description:  
   This method instructs the text control to enter/leave focus or
   selected mode.  The concept of focus is left somewhat to the control.
   In terms of text controls it means the control is active and in edit 
   mode. This call also results in the text control redrawing it's 
   contents.

   It is important to note that text control object will still have 
   its HandleEvent method called even when its inactive.  This allows 
   it to process special events such as scrolling multi-line text 
   controls.

Prototype:

   // C Users
   void CTextCtl_SetActive(ITextCtl * pITextCtl);

   // C++ Users
   void CTextCtl::SetActive();

Parameters:
   pITextCtl: Pointer to the text control object

Return Value: None
Comments: None
Side Effects: None
See Also: None

=====================================================================*/ 
static void CTextCtl_SetActive(ITextCtl * pITextCtl, boolean bActive)
{
   CTextCtl * pme = (CTextCtl *)pITextCtl;

   TextCtl_SetMode(pITextCtl, bActive, pme->m_tmLast, TRUE);
   if( pme->m_clsMe == AEECLSID_TEXTCTL && !(pme->m_dwProps & TP_FOCUS_NOSEL) && bActive ){
      CTextCtl_SetSelection(pITextCtl, TC_MAKE_SEL(TC_SEL_START, TC_SEL_END));
   }
}

/*===================================================================== 

Function:  CTextCtl_IsActive()

Description: 
   This method returns the active state of the text control object.

Prototype:
   // C Users
   boolean CTextCtl_IsActive(ITextCtl * pITextCtl);

   // C++ Users
   boolean CTextCtl::IsActive();

Parameters:
   pITextCtl: Pointer to the text control object

Return Value: 
   TRUE if the text control is active
   FALSE otherwise

Comments: None
Side Effects: None
See Also: None

=====================================================================*/ 
static boolean CTextCtl_IsActive(ITextCtl * pITextCtl)
{
	return (((CTextCtl*)pITextCtl)->m_bActive == 0 ? FALSE: TRUE);
}

/*===================================================================== 

Function:  CTextCtl_SetRect()

Description: 
   This method sets the active screen coordinates of the text control.

Prototype:

   // C Users
   void CTextCtl_SetRect(ITextCtl * pITextCtl, const AEERect * prc);

   // C++ Users
   void CTextCtl::SetRect(const AEERect * prc);

Parameters:
   pITextCtl: Pointer to the text control object
   prc: The bounding rectangle for the text control

Return Value: None
Comments: None
Side Effects: None
See Also: CTextCtl_GetRect

=====================================================================*/ 
static void CTextCtl_SetRect(ITextCtl * pITextCtl, const AEERect * prc)
{
   CTextCtl *  pme = (CTextCtl *)pITextCtl;
   AEERect     rcOld;

   if (!prc) {
      return;
   }

   rcOld     = pme->m_rc;
   pme->m_rc = *prc;
   
   if (!(pme->m_dwProps & TP_NODRAW)) {
      IDISPLAY_FillRect(pme->m_pIDisplay, &rcOld, CLR_SYS_WIN);
   } else if ((rcOld.dx == prc->dx) && (rcOld.dy == prc->dy)) {
      return;
   }
   
   TextCtl_CalcRects(pme);
   CTextCtl_Redraw((ITextCtl *)pme);
}

/*===================================================================== 

Function:  CTextCtl_GetRect()

Description: 
   This method fills a pointer to an input RECT with the active screen 
   coordinates for the text control.  This is particularly useful after 
   a control is created to determine it's optimal/default size and 
   position.

Prototype:
   // C Users
   void CTextCtl_GetRect(ITextCtl * pITextCtl, AEERect * prc);

   // C++ Users
   void CTextCtl::GetRect(AEERect * prc);

Parameters:
   pITextCtl: Pointer to the text control object
   prc: The rectangle to be filled with the coordinates of the text control

Return Value: None
Comments: None
Side Effects: None
See Also: CTextCtl_SetRect

=====================================================================*/ 
static void CTextCtl_GetRect(ITextCtl * pITextCtl, AEERect * prc)
{
   if (prc)
      *prc = ((CTextCtl*)pITextCtl)->m_rcGet;
}

/*===================================================================== 

Function:  CTextCtl_SetProperties()

Description: 
   This method sets text control specific properties or flags. 

Prototype:
   // C Users
   void CTextCtl_SetProperties(ITextCtl * pITextCtl, uint32 dwProps);

   // C++ Users
   void CTextCtl::SetProperties(uint32 dwProps);

Parameters:
   pITextCtl: Pointer to the text control object
   dwProps: 32-bit set of flags/properties

Return Value: None
Comments: None
Side Effects: 
See Also: CTextCtl_GetProperties

=====================================================================*/ 
static void CTextCtl_SetProperties(ITextCtl * pITextCtl, uint32 nProperties)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;

   //   if (((pme->m_dwProps ^ nProperties) & ~ TP_NODRAW) == 0) {
   //      // nothing changed (except maybe TP_NODRAW)
   //      pme->m_dwProps = nProperties;
   //      return;
   //   }
   
   pme->m_dwProps = nProperties;
   if (pme->m_pText)
      OEM_TextSetProperties(pme->m_pText, pme->m_dwProps);
   TextCtl_CalcRects(pme);
}

/*===================================================================== 

Function:  CTextCtl_GetProperties()

Description: 
   This method returns the text control specific properties or flags.
   
Prototype:
   // C Users
   uint32 CTextCtl_GetProperties(ITextCtl * pITextCtl);

   // C++ Users
   uint32 CTextCtl::SetProperties();

Parameters:
   pITextCtl: Pointer to the text control object

Return Value: 32-bit properties for the text control
Comments: None
Side Effects: None
See Also: CTextCtl_SetProperties

=====================================================================*/ 
static uint32 CTextCtl_GetProperties(ITextCtl * pITextCtl)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;

   return(pme->m_dwProps);
}

/*===================================================================== 
ITEXTCTL_SetPropertiesEx()

Description:
    This function sets extended text control-specific properties or flags.

Prototype:
    void ITEXTCTL_SetPropertiesEx(ITextCtl * pITextCtl, uint32 dwProps)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    dwProps  :  32-bit set of flags/properties.
    PlaceHolder: No Extended Properties at this time.

Return Value:
    None

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 3.1.3

See Also:
    ITEXTCTL_GetPropertiesEx()
    ITEXTCTL_SetProperties()
    ITEXTCTL_GetProperties()
=====================================================================*/ 
static void CTextCtl_SetPropertiesEx(ITextCtl * pITextCtl, uint32 nPropsEx)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;
   
   pme->m_dwPropsEx = nPropsEx;
   if (pme->m_pText)
      OEM_TextSetProperties(pme->m_pText, pme->m_dwPropsEx);
}

/*===================================================================== 
ITEXTCTL_GetPropertiesEx()

Description:
    This function returns the extended text control-specific properties or flags.

Prototype:
    uint32 ITEXTCTL_GetPropertiesEx(ITextCtl * pITextCtl)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.

Return Value:
    32-bit properties for the text control.
    Following properties are returned by the text control object:~
~
    Placeholder, No Extended Properties at this time.
*

Comments:
    None

Version:
   Introduced BREW Client 3.1.3

See Also:
    ITEXTCTL_SetPropertiesEx()
    ITEXTCTL_SetProperties()
    ITEXTCTL_GetProperties()
=====================================================================*/ 
static uint32 CTextCtl_GetPropertiesEx(ITextCtl * pITextCtl)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;

   return(pme->m_dwPropsEx);
}

/*===================================================================== 

Function:  CTextCtl_Reset()

Description: 
   This method instructs the text control to reset (free/delete) it's
   contents as well as to immediately leave active/focus mode.

Prototype:
   // C Users
   void CTextCtl_Reset(ITextCtl * pITextCtl);

   // C++ Users
   void CTextCtl::Reset();

Parameters:
   pITextCtl: Pointer to the text control object

Return Value: None
Comments: None
Side Effects: None
See Also: CTextCtl_SetActive

=====================================================================*/ 
static void CTextCtl_Reset(ITextCtl * pITextCtl)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;

   CTextCtl_SetSoftKeyMenu(pITextCtl, NULL);

   if (pme->m_pTitle){ 
      FREE(pme->m_pTitle);
      pme->m_pTitle = NULL;
   }

   // clear text
   pme->m_tmLast  = AEE_TM_CURRENT;
   pme->m_bActive = FALSE;

   MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));

   if(pme->m_pText)
   {
      OEM_TextDelete(pme->m_pText);
      pme->m_pText = NULL;
   }

   if(pme->m_pModeMenu)
   {
      IMENUCTL_Release(pme->m_pModeMenu);
      pme->m_pModeMenu = NULL;
   }
   CTextCtl_EnumModeInit(pITextCtl);
}

/*===================================================================== 

Function: CTextCtl_SetTitle

Description: 
   This method is used to set title of a text control object. This 
   reads title string corresponding to the given resource identifier 
   from resource file and sets it as the title of the text control 
   object.

Prototype:
   boolean CTextCtl_SetTitle(ITextCtl * pITextCtl, 
                               const char * pszResFile, 
                               uint16 wResID, 
                               AECHAR * pText)

Parameter(s):
   pITextCtl: pointer to ITextCtl interface to the text control object
   pszResFile: file containing resource string
   wResID: resource identifier
   pText: resource string if pszResFile is NULL
          place holder for resource string otherwise

Return Value:
   TRUE  - Successful
   FALSE - Failure

Comments: none

Side Effects: none

See Also: none

=====================================================================*/ 
static boolean CTextCtl_SetTitle(ITextCtl * pITextCtl, const char * pszResFile, uint16 wResID, AECHAR * pText)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;
   AECHAR  szBuff[32];

	MEMSET(szBuff, 0, sizeof(szBuff));

   if (pme->m_pTitle){ 
      FREE(pme->m_pTitle);
      pme->m_pTitle = NULL;
   }
  
   if (!pText && pszResFile &&
       ISHELL_LoadResString(pme->m_pIShell, pszResFile, wResID, szBuff, 
                            sizeof(szBuff))) {
      pText = szBuff;
   }

   if (pText) 
      pme->m_pTitle = WSTRDUP(pText);

   TextCtl_CalcRects(pme);

   return(pme->m_pTitle != NULL);
}

/*=================================================================
Function: CTextCtl_SetText

Description: 
   This method is used to assign given string as text of the text 
   control object.

Prototype:
   boolean CTextCtl_SetText(ITextCtl* pITextCtl, 
                              const AECHAR* psz, 
                              int cch)

Parameter(s):
   pITextCtl:  pointer to ITextCtl interface to text control object
   psz: string
   cch: number of characters to be assigned from the string to the 
        text of the text control object

Return Value:
   TRUE  - Success
   FALSE - Failure

Comments: none

Side Effects: none

See Also: none
=================================================================*/
static boolean CTextCtl_SetText(ITextCtl * pITextCtl, const AECHAR * psz, int cch)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;
   boolean bRet = FALSE;

   if (psz && pme->m_nMaxChars){

	   int len = WSTRLEN((AECHAR*)psz);

      if ((len && cch < 0) || cch > len) 
         cch = len;

      if (cch > pme->m_nMaxChars)
         cch = pme->m_nMaxChars;
   }
   else
      cch = 0;

   if(pme->m_pText == NULL)
      TextCtl_CalcRects(pme);

   if(pme->m_pText) 
      bRet = OEM_TextSet(pme->m_pText, psz, cch);

   CTextCtl_Redraw(pITextCtl);
   return bRet;
}

/*=================================================================
Function: CTextCtl_GetText

Description: 
   This method is used to read text associated with the text control 
   object in the given buffer subject to the maximum of nMaxChars.

Prototype:
   boolean CTextCtl_GetText(ITextCtl* pITextCtl, 
                              AECHAR* pBuffer, 
                              unsigned int nMaxChars)

Parameter(s):
   pITextCtl: pointer to ITextCtl interface to text control object
   pBuffer: place holder for the text
   nMaxChars: maximum number of characters to be read

Return Value:
   TRUE  - Success
   FALSE - Failure

Comments: none

Side Effects: none

See Also: none
=================================================================*/
static boolean CTextCtl_GetText(ITextCtl * pITextCtl, AECHAR * pBuffer, int nMaxChars)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;
   AECHAR   * pszText;

   if((pszText = OEM_TextGet(pme->m_pText)) != NULL)
   {
      WSTRNCOPYN(pBuffer, nMaxChars, pszText, WSTRLEN(pszText));
      return(TRUE);
   }
   return FALSE;
}

/*===========================================================================
Function: CTextCtl_GetTextPtr

Description: 
   It returns the pointer to the text maintained by the ITextCtl object. The 
   difference between this function and GetText is that that latter copies the 
   content to a destination buffer and the former just returns the pointer to 
   the text inside the ITextCtl object.

Prototype:
   AECHAR* CTextCtl_GetTextPtr(ITextCtl * pITextCtl)

Parameter(s):
   pITextCtl: pointer to ITextCtl object interface

Return Value: pointer to the text

Comments: none

Side Effects: none

See Also: none
===========================================================================*/
static AECHAR * CTextCtl_GetTextPtr(ITextCtl * pITextCtl)
{
   CTextCtl * pme = (CTextCtl*)pITextCtl;

   if(pme->m_pText)
   {
      AECHAR * pText = OEM_TextGet(pme->m_pText);
      return (pText ? pText : (AECHAR*)"\0");
   }
   else
   {
      return NULL;
   }
}

/*===================================================================== 

Function: CTextCtl_EnableCommand

Description: 
   This method is used to enable sending of given command by the text 
   control object to the shell.

Prototype:
   void CTextCtl_EnableCommand(ITextCtl* pITextCtl, 
                                 boolean bEnable, 
                                 uint16 nCmdId)

Parameter(s):
   pITextCtl: pointer to ITextCtl interface to the text control object
   bEnable: boolean value for enable flag
   nCmdId: command id

Return Value: none

Comments: none

Side Effects: none

See Also: none

=====================================================================*/ 
static void CTextCtl_EnableCommand(ITextCtl * pITextCtl, boolean bEnable, uint16 nCmdId)
{
#if 0 // {
   // Not going to be supported
   CTextCtl * pme = (CTextCtl*)pITextCtl;

   pme->m_bSendCmd = (bEnable != 0);
   if (bEnable) 
   {
      pme->m_nCmdId = nCmdId;
   }
#endif // } 0
}

/*=================================================================
Function: CTextCtl_SetMaxSize

Description: 
   This method is used to set maximum text size supported by the 
   text control object. If the size being set is more than the size 
   already set, this leads to the freeing up of the memory associated 
   with the previous size and allocation of the memory as per new size.

Prototype:
   void CTextCtl_SetMaxSize(ITextCtl* pITextCtl, uint16 nMaxSize)

Parameter(s):
   pITextCtl: pointer to ITextCtl interface to text control object
   nMaxSize: maximum text size

Return Value: none

Comments: none

Side Effects: none

See Also: none
=================================================================*/   
static void CTextCtl_SetMaxSize(ITextCtl * pITextCtl, uint16 nMaxSize)
{
   CTextCtl *pme = (CTextCtl*) pITextCtl;
   
   pme->m_nMaxChars = nMaxSize;
   if (pme->m_pText)
      OEM_TextSetMaxChars(pme->m_pText, pme->m_nMaxChars);
}

/*===========================================================================
Function: CTextCtl_SetSoftKeyMenu

Description: 
   It brings a new soft key menu to the ITextCtl.

Prototype:
   void CTextCtl_SetSoftKeyMenu(ITextCtl * pITextCtl, IMenuCtl * pm)

Parameter(s):
   pITextCtl: pointer to the public ITextCtl object interface
   pm: new menu control object for the soft key menu

Return Value: none

Comments: none

Side Effects: 
   IMenuCtl's reference count is bumped up and a new menu item is added to the 
   menu if an entry mode string is maintained by the text manager.

See Also: none

SDK SPECIFIC INFORMATION:

SDK Description: This function is not implemented in SDK.
===========================================================================*/
static void CTextCtl_SetSoftKeyMenu(ITextCtl * pITextCtl, IMenuCtl * pm)
{
   CTextCtl *  pme = (CTextCtl *)pITextCtl;
   AECHAR  szItem[MAX_TEXT_MODE_SIZE];

   if(pme->m_pSoftKey) 
      IMENUCTL_Release(pme->m_pSoftKey);

   pme->m_pSoftKey = pm;

   if(pm)
   {
      IMENUCTL_AddRef(pm);

      if(!(pme->m_dwProps & CP_STATIC)){
         OEM_TextGetModeString(pme->m_pText, szItem, sizeof(szItem));
         IMENUCTL_AddItem(pm, NULL, 0, IDC_TEXT_MODE, szItem, 0);
      }
   }
}

static AEETextInputMode CTextCtl_SetInputMode(ITextCtl * po, AEETextInputMode m)
{
   CTextCtl * pme = (CTextCtl *)po;
   AEETextInputMode tmCurr;
   
   // Create the control if it isn't present
   if(pme->m_pText == NULL)
      TextCtl_CalcRects(pme);

   if( pme->m_bShowSyms )
      TextCtl_CloseSymbolDialog(pme, (AECHAR)0);
   // See if they are setting a BREW mode
   if( m == AEE_TM_SYMBOLS ){
      if( TextCtl_IsSymbolsSupported(pme) ){
         TextCtl_ShowSymbolDialog(pme);
         ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TEXT_MODECHANGED, (uint16)m, (uint32)pme);
         return m;
      }
      return(OEM_TextGetCurrentMode(pme->m_pText));
   }
   // If not, try it with the OEM
   tmCurr   = CTextCtl_GetInputMode(po, NULL);
   if(m != OEM_TextGetCurrentMode(pme->m_pText)){
      AEETextInputMode  tmNext;
      OEM_TextSetEdit(pme->m_pText, (boolean)pme->m_bActive, m);
      tmNext   = CTextCtl_GetInputMode(po, NULL);
      if(tmCurr != tmNext)
         ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TEXT_MODECHANGED, (uint16)tmNext, (uint32)pme);
      TextCtl_CheckEntryMode(pme);
   }
   return(OEM_TextGetCurrentMode(pme->m_pText));
}
static AEETextInputMode CTextCtl_GetInputMode(ITextCtl *po, AEETextInputModeInfo * pmInfo)
{
   CTextCtl * pme = (CTextCtl *)po;

   // Check if we are in a BREW mode, this case is symbols
   if(pme->m_bShowSyms){
      if( pmInfo ){
         MEMCPY(pmInfo, &sBREWTextModes[0], sizeof(AEETextInputModeInfo));
      }
      return sBREWTextModes[0].tmMode;
   }
   if( !pmInfo )
      return OEM_TextGetCurrentMode(pme->m_pText);

   pmInfo->tmMode = OEM_TextGetCurrentMode(pme->m_pText);
   OEM_TextGetModeString(pme->m_pText, pmInfo->modeString, sizeof(pmInfo->modeString));
   return pmInfo->tmMode;
}
static void CTextCtl_EnumModeInit(ITextCtl *po)
{
   CTextCtl * pme = (CTextCtl *)po;
   OEM_TextEnumModesInit();
   pme->m_nModeEnum = 0;
}
static AEETextInputMode CTextCtl_EnumNextMode(ITextCtl *po, AEETextInputModeInfo * pmInfo)
{
   CTextCtl * pme = (CTextCtl *)po;
   AEETextMode    tmMode;

   tmMode.wID           = AEE_TM_NONE;
   tmMode.pszTitle[0]   = (AECHAR)0;

   if( pmInfo ){
      pmInfo->tmMode = AEE_TM_NONE;
      *pmInfo->modeString  = (AECHAR)0;
   }

   if( OEM_TextEnumMode(&tmMode) ){
      if( pmInfo ){
         pmInfo->tmMode = tmMode.wID;
         WSTRLCPY(pmInfo->modeString, tmMode.pszTitle, MAX_TEXT_MODE_SIZE);
      }
   }
   else{
      if( sBREWTextModes[pme->m_nModeEnum].tmMode == AEE_TM_SYMBOLS && !TextCtl_IsSymbolsSupported(pme) )
         pme->m_nModeEnum++;

      if( sBREWTextModes[pme->m_nModeEnum].tmMode != AEE_TM_RESERVE_LAST ){
         tmMode.wID = sBREWTextModes[pme->m_nModeEnum].tmMode;
         if( pmInfo )
            MEMCPY(pmInfo, &sBREWTextModes[pme->m_nModeEnum], sizeof(AEETextInputModeInfo));
         pme->m_nModeEnum++;
      }
   }
   return (AEETextInputMode)tmMode.wID;
}

/*===========================================================================
Function: TextCtl_SetText

Description: It sets the new text in the text control

Prototype:
   static boolean TextCtl_SetText(CTextCtl * pme, 
                                  boolean bSetText, 
                                  uint16 wID, 
                                  AECHAR * pText)

Parameter(s):
   bSetText: Flag to indicate whether to set the text or the title. If true, it sets
   the text within the text control. If false, it sets of the title of the text
   control.
   
   wID: Denotes the ID of the string resource in the resource files from where the
   string for the text/title needs to be taken

   pText: Name of the resource file

Return Value: none

Comments: none

Side Effects: 

See Also: none

SDK SPECIFIC INFORMATION:

===========================================================================*/
static boolean TextCtl_SetText(CTextCtl * pme, boolean bSetText, uint16 wID, AECHAR * pText)
{
   const char  *pRes;
   int         nLen;
   boolean     bFree = FALSE, bRet;

   if(wID)
   {
      pRes = (const char *)pText;  // File names are char* only. Not AECHAR
      pText = NULL;
   }
   else
      pRes = NULL;
   
   if(bSetText)
   {
      if(!pText){
         pText = (AECHAR *)MALLOC(MAX_TEXT_SIZE);
         if(!pText) 
            return(FALSE);
         ISHELL_LoadResString(pme->m_pIShell, pRes, wID, pText, MAX_TEXT_SIZE);
         bFree = TRUE;
      }

      nLen = WSTRLEN(pText);
      if(nLen > pme->m_nMaxChars)
         pme->m_nMaxChars = nLen;

      bRet = CTextCtl_SetText((ITextCtl *)pme, pText, -1);

      if(bFree) 
         FREE(pText);

      return(bRet);
   }
   return(CTextCtl_SetTitle((ITextCtl *)pme, pRes, wID, pText));
}

/*===========================================================================
Function: TextCtl_CalcRects()

Description: This function does a bunch of initializations for the text control. It
does the following:
   a.Calculates the coordinates required for the text control
   b.Creates a new OEM_textControl object
   c.Sets the default mode for the text control and displays the mode.

Prototype:
   void TextCtl_CalcRects(CTextCtl * pme)

Parameter(s):
   pme: Pointer to CTextCtl that should be used for TextCtl initialization

Return Value: none

Comments: none

Side Effects: 

See Also: none

SDK SPECIFIC INFORMATION:

===========================================================================*/
static void TextCtl_CalcRects(CTextCtl * pme)
{
   AEERect     rc;
   boolean     bFixRect  = (pme->m_dwProps & TP_FIXSETRECT) != 0;
   boolean     bIntegral = (pme->m_dwProps & TP_INTEGRALHEIGHT) != 0;
   boolean     bMulti    = (pme->m_dwProps & TP_MULTILINE) != 0;
   boolean     bFixOEM   = (pme->m_dwProps & TP_FIXOEM) != 0;

   rc = pme->m_rc;

   if (pme->m_pTitle)
      rc.y += LINEHEIGHT;

   if (bMulti) {      // multi-line
      // adjust for title area
      if (!bFixRect || pme->m_pTitle)
         rc.dy -= LINEHEIGHT;
      // round rc.dy to nearest line increment
      if (bIntegral) {
         int lines = (rc.dy + (LINEHEIGHT/2)) / LINEHEIGHT;
         rc.dy = (LINEHEIGHT * lines) + 6;
      }
   }
   else {                                    // single-line
      rc.dy = (LINEHEIGHT + 6);
   }

   // check within min/max 
   if (bFixRect) {
       int dyMax = pme->m_dyTextMax;
       boolean bFix = rc.dy >= dyMax;
       if (pme->m_pTitle)
          dyMax -= LINEHEIGHT;
       rc.dy = CONSTRAIN(rc.dy, (LINEHEIGHT+6), dyMax);
       if (bFix && bIntegral && bMulti)
          rc.dy = ((rc.dy / LINEHEIGHT) * LINEHEIGHT) + 6;
   }

   // calc rectangle of entire control, which will be
   // stored in m_rcGet and returned in GetRect()
   if (!bFixRect) {
      // !TP_FIXSETRECT means mimic old behavior which blindly
      // returns the rectangle passed to SetRect
      pme->m_rcGet = pme->m_rc;
   } 
   else {
      // otherwise, calculate the control rectangle from
      // the calculated/adjusted text rectangle
      pme->m_rcGet = rc;
      if (pme->m_pTitle) {
         pme->m_rcGet.y -= LINEHEIGHT;
         pme->m_rcGet.dy += LINEHEIGHT;
      }
   }

   if (bFixOEM && pme->m_pText) {

      OEM_TextSetRect(pme->m_pText, &rc);

   } else {
      boolean  bActive = CTextCtl_IsActive((ITextCtl *)pme);
      int32    nCursor = 0;
      AECHAR * pText = NULL;
      
      if (pme->m_pText) {
         if(OEM_TextGet(pme->m_pText))
            pText = WSTRDUP(OEM_TextGet(pme->m_pText));
         nCursor = CTextCtl_GetCursorPos((ITextCtl *)pme);
         OEM_TextDelete(pme->m_pText);
      }
   
      pme->m_pText = OEM_TextCreate(pme->m_pIShell, pme->m_pIDisplay, &rc, pme->m_clsMe);
      
      OEM_TextSetProperties(pme->m_pText, pme->m_dwProps);
      if (pText) 
      {
         OEM_TextSet(pme->m_pText, pText, -1);
         FREE(pText);
      }
      OEM_TextSetMaxChars(pme->m_pText, pme->m_nMaxChars);
      CTextCtl_SetCursorPos((ITextCtl *)pme, nCursor);
      if(bActive){
         CTextCtl_SetActive((ITextCtl *)pme, bActive);
      }
   }
   
   TextCtl_CheckEntryMode(pme);
   
}



/*===========================================================================
Function: TextCtl_Delete()

Description: This function deletes the given text control and does the
required cleanup.

Prototype:
   void TextCtl_Delete(CTextCtl * pme)

Parameter(s):
   pme: Pointer to CTextCtl that needs to be deleted

Return Value: none

Comments: none

Side Effects: 

See Also: none

SDK SPECIFIC INFORMATION:

===========================================================================*/
static void TextCtl_Delete(CTextCtl *pme)
{
   CTextCtl_Reset((ITextCtl *) pme);

   IDISPLAY_Release(pme->m_pIDisplay);
   ISHELL_Release(pme->m_pIShell);
   FREE(pme);
}


/*===========================================================================
Function: TextCtl_CheckEntryMode(CTextCtl * pme)

Description: This functions gets the string from the OEM that needs to be displayed
in the soft key menu and re-draws the soft key menu to display that string.

Prototype:
   static void TextCtl_CheckEntryMode(CTextCtl * pme)

Parameter(s):
   

Return Value: None

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static void TextCtl_CheckEntryMode(CTextCtl * pme)
{
   AECHAR  szMode[MAX_TEXT_MODE_SIZE];

#if 0   
   if(pme->m_pText)
      pme->m_tmLast = OEM_TextGetCurrentMode(pme->m_pText);
#endif

   if(pme->m_pSoftKey != NULL)
   {
      OEM_TextGetModeString(pme->m_pText, szMode, sizeof(szMode));
      IMENUCTL_SetItemText(pme->m_pSoftKey, IDC_TEXT_MODE, NULL, 0, szMode);
      IMENUCTL_Redraw(pme->m_pSoftKey);
   }
}

/*===========================================================================
Function: TextCtl_GetSymbDlgInfo()

Description: This function gets information to prepare for displaying the symbol 
dialog. It is used internally as a local method and parameters are not checked.

Prototype:
   static void TextCtl_GetSymbDlgInfo(IDisplay * pd, AEEBitmapInfo * pbi, AECHAR * pszwBuff, uint16 wBuffSize, int * pcxAdd, int * pcyAdd, int16 * pwDx, int16 * pwDy)

Parameter(s):   
      pd :        IDisplay for measuring fonts and text
      pbi :       Provide for calculations
      pszwBuff :  [OUT] Buffer to fill with symbols
      wBuffSize : Size of input buffer
      pcxAdd :    [OUT] Column spacing
      pcyAdd :    [OUT] Row Spacing 
      pwDx :      [OUT] Column item spacing
      pwDy :      [OUT] Column item height

Return Value: None

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static void TextCtl_GetSymbDlgInfo(IDisplay * pd, AEEBitmapInfo * pbi, AECHAR * pszwBuff, uint16 wBuffSize, int * pcxAdd, int * pcyAdd, int16 * pwDx, int16 * pwDy)
{
   AECHAR sz[2];
   int16 dx, dy;
   int            nFits;
   int16          nWidth;
   int            i;

   OEM_TextQuerySymbols(pszwBuff, wBuffSize);

   // Determine the largest character size in the array...
   dx = 0;
   sz[1] = 0;
   for(i = 0; i < 9 ; i++){
      *sz = (AECHAR)('1' + i); // (HS_CHR_KEY_1 + i);
      nWidth = IDISPLAY_MeasureTextEx(pd, AEE_FONT_NORMAL, sz, 1, -1, &nFits);
      if(nWidth > dx)
         dx = nWidth;
   }
   *pwDx = dx + 1;

   dy = IDISPLAY_GetFontMetrics(pd, AEE_FONT_BOLD, NULL, NULL) + 1;
   *pcyAdd = (pbi->cy - dy) / 3;
   *pcxAdd = pbi->cx / 3;
   
   *pwDy = dy;
}

/*===========================================================================
Function: TextCtl_ShowSymbolPage()

Description: This function displays the symbols supported. It is invoked
when the text control is in SYMBOLS mode.

Prototype:
   static void TextCtl_ShowSymbolPage(CTextCtl * pme, int nDir)

Parameter(s):
   pME: Pointer to Text control where list of symbols needs to be displayed
   nDir: Direction (up / down) for scrolling. 

Return Value: None

Comments: none

Side Effects: 

See Also: none

SDK SPECIFIC INFORMATION:

===========================================================================*/
static void TextCtl_ShowSymbolPage(CTextCtl * pme, int nDir)
{

   AECHAR          szBuff[64],sz[2];
   int            nPage, x, y,cyAdd, cxAdd, nRow, nCol;
   IDisplay *     pd = pme->m_pIDisplay;
   AEERect        rc;
   AECHAR   *      pszSym;
   int            nPages, nSymsTotal,i;

   szBuff[0] = (AECHAR)'^';
   szBuff[1] = (AECHAR)'v';
   szBuff[2] = (AECHAR)' ';
   WSTRLCPY(szBuff + 3, sBREWTextModes[0].modeString, sizeof(szBuff) - (sizeof(AECHAR) * 3));
   TextCtl_ClearScreen(pme);
   IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szBuff, -1, SYMDLG_X_ARROWPOS, SYMDLG_Y_ARROWPOS, NULL,0);

   nPage = pme->m_nSymbolPage;

   //Query Symbols From OEM and get dimensions for rows and columns
   TextCtl_GetSymbDlgInfo(pd, &pme->m_bi, szBuff, sizeof(szBuff), &cxAdd, &cyAdd, &rc.dx, &rc.dy);
   y = rc.dy; // Skip scrollbar and title line

   nSymsTotal = WSTRLEN(szBuff);
   nPages = (nSymsTotal / 9);
   if(nSymsTotal % 9)
      nPages++;

   nPage += nDir;
   if(nPage < 0)
      nPage = nPages - 1;
   else{
      if(nPage > nPages - 1)
         nPage = 0;
   }

   pme->m_nSymbolPage = nPage;

   pszSym = szBuff + (nPage * 9);
   for(nRow = i = 0; nRow < 3; nRow++, y += cyAdd){
      for(x = SYMDLG_X_ITEMBASE, nCol = 0; nCol < 3; nCol++, x += cxAdd, i++, pszSym++){

         // See if we hit the end...
         if(!*pszSym){
            nRow = 3;
            break;
         }

         rc.x = x;
         rc.y = y;
         *sz = (AECHAR)('1' + i);   // (HS_CHR_KEY_1 + i);
         IDISPLAY_DrawText(pd, AEE_FONT_NORMAL, sz, 1, rc.x + SYMDLG_X_IDXPAD,y, &rc,IDF_RECT_FILL);
         rc.x += rc.dx + SYMDLG_X_IDXAPAD;
         IDISPLAY_DrawText(pd, AEE_FONT_BOLD, pszSym, 1, rc.x, y, NULL,0);
      }
   }
   if (!(pme->m_dwProps & TP_NODRAW))
      IDISPLAY_Update(pd);
   
}

/*===========================================================================
Function: TextCtl_SymbolHandlePen()

Description: This function handles pen events when the text control is in SYMBOLS mode.

Prototype:
   static boolean TextCtl_SymbolHandlePen(CTextCtl * pme, AEEEvent evtCode, int16 wXPos, int16 wYPos)

Parameter(s):
   pme:        Pointer to ITextCtl Interface object
   evtcode :   Code for pen event
   wXPos :     X Coordinate of the pen event
   wYPos :     Y Coordinate of the pen event

Return Value: 
   TRUE :   Pen event handled
   FALSE :  Pen event not handled

Comments: The mode is NOT checked internally. It is required to be checked by the caller.
See the documentation for expectations of pen handling.

Side Effects: 

See Also: none

===========================================================================*/
static boolean TextCtl_SymbolHandlePen(CTextCtl * pme, AEEEvent evtCode, int16 wXPos, int16 wYPos)
{
   switch( evtCode ){
   case EVT_POINTER_MOVE:
      // Not handled, yet! Later I want to have a pendown on item or arrow give visual 
      // feedback by color inversion until mouse moved off the region of item or arrow
      break; 
   case EVT_POINTER_DOWN:
      {
         pme->m_ptTracker.cbHit = TextCtl_SymbolPagePenHitItem(pme, wXPos, wYPos, &pme->m_ptTracker.wData);
      }
      return TRUE; // Full screen, always in our rect!
   case EVT_POINTER_UP:
      {
         int bRet = FALSE;
         if( pme->m_ptTracker.cbHit ){
            uint16   wData;
            uint8    cbHit = TextCtl_SymbolPagePenHitItem(pme, wXPos, wYPos, &wData);
            if( cbHit == pme->m_ptTracker.cbHit ){
               if( cbHit & PTRCK_HIT_SCRL ){
                  TextCtl_ShowSymbolPage(pme, (cbHit & PTRCK_HIT_ABOVE) ? -1 : 1);
                  bRet = TRUE;
               }else{
                  if( wData == pme->m_ptTracker.wData ){
                     TextCtl_CloseSymbolDialog(pme, TextCtl_GetSymChar(pme,wData));
                     bRet = TRUE;
                  }
               }
            }
         }
         return bRet;
      }
   }
   return FALSE;
}
static uint8 TextCtl_SymbolPagePenHitItem(CTextCtl * pme, int16 wXPos, int16 wYPos, uint16 * pwData)
{
   AEERect        rctInspect;
   IDisplay *     pd       = pme->m_pIDisplay;
   int            nHeight  = IDISPLAY_GetFontMetrics(pd, AEE_FONT_BOLD, NULL, NULL);
   AECHAR         szBuff[16];
   uint8          cbReturn = 0;

   rctInspect = pme->m_rc;
   if( wYPos < (rctInspect.y + nHeight + SYMDLG_Y_ARROWPOS) 
      && wYPos >= SYMDLG_Y_ARROWPOS ){
      szBuff[0] = (AECHAR)'^';
      szBuff[1] = (AECHAR)0;
      rctInspect.x = SYMDLG_X_ARROWPOS;
      rctInspect.dx  = rctInspect.x + IDISPLAY_MeasureText(pd,AEE_FONT_BOLD,szBuff);
      if( wXPos >= rctInspect.x && wXPos < (rctInspect.x + rctInspect.dx) ){
         // Hit Up arrow!
         cbReturn = PTRCK_HIT_ABOVE | PTRCK_HIT_SCRL;
      }else{
         szBuff[0] = (AECHAR)'v';
         szBuff[1] = (AECHAR)0;
         rctInspect.x += rctInspect.dx;
         rctInspect.dx  = rctInspect.x + IDISPLAY_MeasureText(pd,AEE_FONT_BOLD,szBuff);
         if( wXPos >= rctInspect.x && wXPos < (rctInspect.x + rctInspect.dx) ){
            // Hit Down arrow!
            cbReturn = PTRCK_HIT_BELOW | PTRCK_HIT_SCRL;
         }
      }
   }else{
      AECHAR         szBuff[64],
             *       pszMeasure;
      int            cyAdd, cxAdd;
      IDisplay *     pd = pme->m_pIDisplay;
      int            i, nWidth = 0, nPage = pme->m_nSymbolPage;
     // Haven't hit our page 'scrollbar'
      TextCtl_GetSymbDlgInfo(pd, &pme->m_bi, szBuff, sizeof(szBuff), &cxAdd, &cyAdd, &rctInspect.dx, &rctInspect.dy);
      // Be nice and make our columns the same width also, so they don't have to try so hard
      // to tap or click  | . ! and similar
      pszMeasure = szBuff + (nPage * 9);
      for( i = 0; i < 9 && *pszMeasure; i++, pszMeasure++ ){
         int nTmp = IDISPLAY_MeasureTextEx(pd, AEE_FONT_NORMAL, pszMeasure, 1, -1, NULL);
         if( nTmp > nWidth ){
            nWidth = nTmp;
         }
      } // Do not play with i iterator, it is re-used below to keep track of quantity of items

      // This represents the bounding rect of the page items, below the title and arrows
      if( wXPos >= SYMDLG_X_ITEMBASE && wYPos >= (SYMDLG_Y_ARROWPOS + rctInspect.dy) ){
         int8 wCol = -1, wRow = -1, wIter;
         for( wIter = 0; wIter < 3; wIter++ ){
            uint16 wBegin = (wIter*cxAdd) + (SYMDLG_X_ITEMBASE+SYMDLG_X_IDXPAD);
            if( wXPos >= wBegin && wXPos < (wBegin+rctInspect.dx) ){
               wCol = wIter;
               break;
            }else if(wXPos >= (wBegin+rctInspect.dx+SYMDLG_X_IDXAPAD) && wXPos < (wBegin+(rctInspect.dx<<1)+SYMDLG_X_IDXAPAD) ){
               wCol = wIter;
               break;
            }
         }
         for( wIter = 0; wIter < 3; wIter++ ){
            if( wYPos >= (((wIter)*cyAdd)+rctInspect.dy) && wYPos < ((((wIter)*cyAdd)+cyAdd)+rctInspect.dy) ){
               wRow = wIter;
               break;
            }
         }
         if( wRow != -1 && wCol != -1 ){
            uint16 wData = (wRow*3) + wCol;
            if( wData < i ){
               cbReturn = PTRCK_HIT_ITEM;
               *pwData = wData;
            }
         }
      }
   }
   return cbReturn;
}

/*===========================================================================
Function: TextCtl_IsSymbolsSupported()

Description: This function finds out if the OEM wants to support symbols mode.

Prototype:
   static boolean TextCtl_IsSymbolsSupported(CTextCtl * pme)

Parameter(s):
   pme: Pointer to Text control where list of symbols needs to be displayed

Return Value: TRUE - supported, FALSE - unsupported

Comments: none

Side Effects: 

See Also: none

SDK SPECIFIC INFORMATION:

===========================================================================*/
static boolean TextCtl_IsSymbolsSupported(CTextCtl * pme)
{
   AECHAR          szBuff[64];

   *szBuff = (AECHAR)0; // in case OEM is misbehaved.
   if( OEM_TextQuerySymbols(szBuff, sizeof(szBuff)) && *szBuff )
      return TRUE;

   return FALSE;
}
/*===========================================================================
Function: TextCtl_GetSymChar()

Description: This function returns the symbol at the specified position in the
symbols array

Prototype:
   static AECHAR TextCtl_GetSymChar(CTextCtl * pme, int nPos)

Parameter(s):
   pme: Pointer to text control 

Return Value: Symbol at the specified position

Comments: none

Side Effects: 

See Also: none

SDK SPECIFIC INFORMATION:

===========================================================================*/
static AECHAR TextCtl_GetSymChar(CTextCtl * pme, int nPos)
{
   AECHAR  szSyms[64];

   if(OEM_TextQuerySymbols(szSyms, sizeof(szSyms)) && *szSyms){
      nPos += (pme->m_nSymbolPage * 9);
      if(nPos < WSTRLEN(szSyms))
         return(szSyms[nPos]);
   }
   return((AECHAR)0);
}



/*===========================================================================
Function: TextCtl_ClearScreen()

Description: This function clears the entire phone screen by filling it
with the default background color (WHITE)

Prototype:
   static void TextCtl_ClearScreen(CTextCtl * pme)

Parameter(s):
   pme: Pointer to text control 

Return Value: None

Comments: none

Side Effects: 

See Also: none

SDK SPECIFIC INFORMATION:

===========================================================================*/
static void TextCtl_ClearScreen(CTextCtl * pme)
{
   AEERect       rc;

   SETAEERECT(&rc, 0, 0, pme->m_bi.cx, pme->m_bi.cy);
   IDISPLAY_FillRect(pme->m_pIDisplay,&rc, RGB_WHITE);
}


/*===========================================================================
Function: TextCtl_KillModeMenu()

Description: This function erases the menu showing the different text modes.

Prototype:
   static void TextCtl_KillModeMenu(CTextCtl * pme)

Parameter(s):
   pme: Pointer to text control 

Return Value: None

Comments: none

Side Effects: 

See Also: none

SDK SPECIFIC INFORMATION:

===========================================================================*/
static void TextCtl_KillModeMenu(CTextCtl * pme)
{
   if(pme->m_pModeMenu)
   {
      IMENUCTL_Release(pme->m_pModeMenu);
      TextCtl_ClearScreen(pme);
      pme->m_pModeMenu = NULL;
   }
}


/*===========================================================================
Function: TextCtl_CloseSymbolDialog()

Description: This function is closed when the user makes a selection from the
list of Symbols dialog box and the dialog box is to be closed. This function
passes the user selected symbol to the OEM and requests to add this symbol at
current cursor position within the text control.

Prototype:
   static void TextCtl_CloseSymbolDialog(CTextCtl * pme, AECHAR ch)

Parameter(s):
   pme: Pointer to text control whose symbols dialog is closing
   ch:  Symbol selected by the user before closing dialog

Return Value: None

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static void TextCtl_CloseSymbolDialog(CTextCtl * pme, AECHAR ch)
{
   boolean  bDoItDoIt   = FALSE,
            bSym        = pme->m_bShowSyms;

   TextCtl_ClearScreen(pme);
   if( !(pme->m_dwProps & TP_FOCUS_NOSEL) ){
      // Backwards compatibility, didn't want to move the SetActive block in case
      // of other side effects, so add the property that could cause the unwanted effect
      // and remove it later when appropriate!
      bDoItDoIt = TRUE;
      pme->m_dwProps |= TP_FOCUS_NOSEL; 
   }
   CTextCtl_SetActive((ITextCtl *)pme, TRUE);
   if( bDoItDoIt ){
      pme->m_dwProps &= ~TP_FOCUS_NOSEL; 
   }

   pme->m_bShowSyms = FALSE;
   if(ch)
      OEM_TextAddChar(pme->m_pText,ch,FALSE);

   // Needed because softkey was drawn before a character was added. This may interfere
   // with the Caps state, so check it again!
   if(bSym){
      TextCtl_CheckEntryMode(pme);
   }

   if(pme->m_pSoftKey)
      IMENUCTL_Redraw(pme->m_pSoftKey);
   CTextCtl_Redraw((ITextCtl *)pme);

   ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TEXT_MODECHANGED, (uint16)CTextCtl_GetInputMode((ITextCtl *)pme, NULL), (uint32)pme);
}


/*===========================================================================
Function: TextCtl_ReturnToEditMode()

Description: This function returns the text control to the requested edit mode.

Prototype:
   static void TextCtl_ReturnToEditMode(CTextCtl * pme, uint16 mode)

Parameter(s):
   pme: Pointer to text control whose edit mode is to be set
   mode: Specifies the new edit mode.

Return Value: None

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static void TextCtl_ReturnToEditMode(CTextCtl * pme, AEETextInputMode mode)
{
   AEETextInputMode tmCur, tmNext;

   tmCur = CTextCtl_GetInputMode((ITextCtl *)pme, NULL);
   TextCtl_KillModeMenu(pme);

   IMENUCTL_SetActive(pme->m_pSoftKey, FALSE);
   TextCtl_SetMode((ITextCtl *)pme, TRUE, mode, TRUE);
   tmNext = CTextCtl_GetInputMode((ITextCtl *)pme, NULL);
   TextCtl_CheckEntryMode(pme);
   CTextCtl_Redraw((ITextCtl *)pme);
   if(tmCur != tmNext)
      ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TEXT_MODECHANGED, (uint16)tmNext, (uint32)pme);
}


/*===========================================================================
Function: TextCtl_ShowSymbolDialog()

Description: This function displays the list of symbols that the user
can pick from.

Prototype:
   static void TextCtl_ShowSymbolDialog(CTextCtl * pme)

Parameter(s):
   pme: Pointer to text control for which the symbols need to be displayed

Return Value: None

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static void TextCtl_ShowSymbolDialog(CTextCtl * pme)
{
   IMenuCtl * pm;
   uint32   nProps;

   pme->m_nSymbolPage = 0;

   // Disable the softkey but do not draw it!!!

   pm = pme->m_pSoftKey;
   if(pm){
      nProps = IMENUCTL_GetProperties(pm);
      IMENUCTL_SetProperties(pm, nProps | MP_NO_REDRAW);
      IMENUCTL_SetActive(pm, FALSE);
      IMENUCTL_SetProperties(pm, nProps);
   }
   CTextCtl_SetActive((ITextCtl *)pme, FALSE);

   TextCtl_KillModeMenu(pme);
   pme->m_bShowSyms = TRUE;


   TextCtl_ShowSymbolPage(pme, 0);
}


/*===========================================================================
Function: TextCtl_ShowModeMenu()

Description: This function shows a menu that lists all the text modes currently supported
by the text control.

Prototype:
   static void TextCtl_ShowModeMenu(CTextCtl * pme)

Parameter(s):
   pme: Pointer to text control where menu of modes needs to be shown.

Return Value: None

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static void TextCtl_ShowModeMenu(CTextCtl * pme)
{
   IMenuCtl *           pm;
   uint16               wCurID;
   AEETextInputModeInfo TextModes;
   int                  count;

   ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MENUCTL, (void **)&pm);

   //Get ID of current Mode. It is assumed that the ModeID is the same
   // as the menu id of that item.
   wCurID = OEM_TextGetCurrentMode(pme->m_pText);

   if(pm != NULL)
   {
      pme->m_pModeMenu = pm;

      //Set the title of the menu to "Text Mode".
      IMENUCTL_SetTitle(pm, OEMAEECONTROLS_LNGRES_FILE, AEE_IDS_MODE_TITLE, NULL);

      //Now, build the list using modes supported by OEM & Us.
      CTextCtl_EnumModeInit((ITextCtl *)pme);

      TextModes.tmMode = AEE_TM_NONE;
      count = 0;
      while(CTextCtl_EnumNextMode((ITextCtl *)pme, &TextModes) != AEE_TM_NONE)
      {
         //The MenuItem ID is an offset from IDC_TEXT_MODE_BASE. 
         // Set user data to be the actual
         // mode ID that OEM passed us. This will come to HandleEvent in dwParam and
         // we can directly pass it to them.
         IMENUCTL_AddItem(pm, NULL, 0, (uint16)(IDC_TEXT_MODE_BASE+count),TextModes.modeString,TextModes.tmMode);
         if(wCurID == TextModes.tmMode)
            wCurID = IDC_TEXT_MODE_BASE+count;
         count++;
      }

      if(wCurID)
      {
         IMENUCTL_SetSel(pm, wCurID);
      }

      if(pme->m_pSoftKey)
      {
         IMENUCTL_SetActive(pme->m_pSoftKey, FALSE);
         CTextCtl_SetActive((ITextCtl *)pme, FALSE);
      }

   // SAS - Added to allow application-level control over style of menu, etc.

      ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_MENU_OPEN, 0, (uint32)pm);
      IMENUCTL_SetActive(pm, TRUE);
      IMENUCTL_Redraw(pm);
   }
}

/*===========================================================================
Function: TextCtl_SetMode()

Description: This function sets the edit mode for the text control.

Prototype:
   static void TextCtl_SetMode(ITextCtl * pITextCtl, 
                            boolean bEdit, 
                            uint16 mode, 
                            boolean bRedraw


Parameter(s):
   pme: Pointer to text control whose edit mode needs to be set
   bEdit: flag to indicate if this control is editable
   mode: Specifies the new mode
   bRedraw: flag to indicate if the text control needs to be redrawn

Return Value: None

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static void TextCtl_SetMode(ITextCtl * pITextCtl, 
                            boolean bEdit, 
                            AEETextInputMode mode, 
                            boolean bRedraw
                           )
{
   CTextCtl *  pme = (CTextCtl *)pITextCtl;

   if(pme->m_pText == NULL)
      TextCtl_CalcRects(pme);

   if(bEdit && (pme->m_dwProps & CP_STATIC))
      bEdit = FALSE;

   pme->m_bActive = bEdit;

   OEM_TextSetEdit(pme->m_pText,bEdit,mode);
   TextCtl_CheckEntryMode(pme);

   if(bRedraw)
      CTextCtl_Redraw(pITextCtl);
}
/*===========================================================================
Function: CTextCtl_GetCursorPos()

Description: This function gets the position of the cursor for the text control.

Prototype:
   static int32 CTextCtl_GetCursorPos(ITextCtl * po)


Parameter(s):
   po: Pointer to text control whose cursor needs to be retrieved

Return Value: Absolute Position of the cursor

Comments: none

Side Effects: 

See Also: none

===========================================================================*/
static int32 CTextCtl_GetCursorPos(ITextCtl * po)
{
   CTextCtl * pme = (CTextCtl*)po;
   return OEM_TextGetCursorPos(pme->m_pText);
}
/*===========================================================================
Function: CTextCtl_SetCursorPos()

Description: This function gets the position of the cursor for the text control.

Prototype:
   static void CTextCtl_SetCursorPos(ITextCtl * po,
                                 int32 nOffset)


Parameter(s):
   po: Pointer to text control whose cursor needs to be retrieved
   nOffset: Absolute offset to place cursor

Return Value: none

Comments: If offset is <= 0 cursor will go to beginning. If offset is > Text length
         Cursor will go to the end of text. Following defined are available:
         TC_CURSOREND - Pass in for nOffset to place cursor at end of text
         TC_CURSORSTART - Pass in for nOffset to place cursor at beginning of text

Side Effects: 

See Also: none

===========================================================================*/
static void CTextCtl_SetCursorPos(ITextCtl * po, int32 nOffset)
{
   CTextCtl * pme = (CTextCtl*)po;
   OEM_TextSetCursorPos(pme->m_pText, nOffset);
   if (!(pme->m_dwProps & TP_NODRAW))
      IDISPLAY_Update(pme->m_pIDisplay);
}
/*===========================================================================
ITEXTCTL_SetSelection()

Description:
    This function sets a selection over the given character positions. 
    The selection will be an inverse of the foreground and background colors.

Prototype:
    void ITEXTCTL_SetSelection(ITextCtl * pITextCtl, uint32 dwSel)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.
    dwSel  :  Lower word is for selection start. Upper word for selection end.

Return Value:
    None

Comments:
    The selection is bound by the start and end of the text string. Any region
    beyond the end of the text string will be ended at the end of the string. The
    cursor will be placed at the selection end position. When the start position is equal
    to the ending position, there will be no selection it will take a similar action to 
    ITEXTCTL_SetCursorPos(). 
    
    If you want the cursor to be at the beginning of the selection, the selection start value
    should be less than the selection end. If a key that is handled by the current input mode 
    is handled by the text control a selection will be erased and replaced by the character 
    chosen by that input mode. A call to ITEXTCTL_SetCursorPos() will unselect the selection
    without removing characters. A call to ITEXTCTL_SetSelection() will change the selection
    and will not remove the characters of an existing selection.

    If you set the selection position between an element of a grapheme, the position
    will be updated such that it is in the next available character position after the 
    complete grapheme.

    You can use the helper macros to set the selection: .
    TC_SEL_START - Macro to use the start of the text string
    TC_SEL_END - Macro to use the end of the text String
    TC_MAKE_SEL(s,e):
       s - Selection Start
       e - Selection End

Version:
   Introduced BREW Client 3.1.3
   Text Control 2

See Also:
    ITEXTCTL_GetSelection()
    ITEXTCTL_SetCursorPos()
    ITEXTCTL_GetCursorPos()
===========================================================================*/
static void CTextCtl_SetSelection(ITextCtl * po, uint32 dwSel)
{
   CTextCtl * pme = (CTextCtl*)po;
   OEM_TextSetSel(pme->m_pText, TC_SEL_GET_START(dwSel), TC_SEL_GET_END(dwSel));
   if (!(pme->m_dwProps & TP_NODRAW)){
      OEM_TextUpdate(pme->m_pText);
      IDISPLAY_Update(pme->m_pIDisplay);
   }
}
/*===========================================================================
ITEXTCTL_GetSelection()

Description:
    This function gets the selected text.

Prototype:
    uint32 ITEXTCTL_GetSelection(ITextCtl * pITextCtl)

Parameters:
    pITextCtl  :  Pointer to the ITextCtl Interface object.

Return Value:
    Double word indicating the selection positions. The lower word is for selection start. 
    The upper word is for selection end.

Comments:
    If there is no selection the start position will equal the end position where the cursor
    currently is. The start and end will have a value equal to ITEXTCTL_GetCursorPos(). 
    
    The selection takes the cursor position into consideration, the selection start value
    will be less than the selection end when the cursor is at the beginning of the selection. 
    If a key that is handled by the current input mode is handled by the text control a 
    selection will be erased and replaced by the character chosen by that input mode. 
    A call to ITEXTCTL_SetCursorPos() will unselect the selection without removing characters. 
    A call to ITEXTCTL_SetSelection() will change the selection and will not remove the 
    characters of an existing selection.

    You can use the helper macros to get the selection values: .
    TC_SEL_GET_START(dw) - Gets the Selection Start Value
    TC_SEL_GET_END(dw) - Gets the Selection End Value

Version:
   Introduced BREW Client 3.1.3
   Text Control 2

See Also:
    ITEXTCTL_SetSelection()
    ITEXTCTL_SetCursorPos()
    ITEXTCTL_GetCursorPos()
===========================================================================*/
static uint32 CTextCtl_GetSelection(ITextCtl * po)
{
   CTextCtl *  pme = (CTextCtl*)po;
   int         dwStart = 0, dwEnd = 0;
   uint32      dwRet;

   OEM_TextGetSel(pme->m_pText, &dwStart, &dwEnd);
   dwRet = TC_MAKE_SEL(dwStart, dwEnd);
   return dwRet;
}
/*===========================================================================
Function: TextCtl_HandlePens()

Description: This function handles pen events and dispatches to other handlers as needed.

Prototype:
   static boolean TextCtl_HandlePens(CTextCtl * pme, AEEEvent evtCode, uint16 wParam, uint32 dwParam)

Parameter(s):
   pme:        Pointer to ITextCtl Interface object
   evtcode :   Code for pen event
   wParam :    uint16 size of character string pointed to by dwParam including terminating NUL, in bytes
   dwParam:    uint32 dwParam for EVT_POINTER_* events

Return Value: 
   TRUE :   Pen event handled
   FALSE :  Pen event not handled

Comments: See the documentation for expectations of pen handling.

Side Effects: 

See Also: none

===========================================================================*/
static boolean TextCtl_HandlePens(CTextCtl * pme, AEEEvent evtCode, uint16 wParam, uint32 dwParam)
{
   switch( evtCode ){
      case EVT_POINTER_STALE_MOVE:
      case EVT_POINTER_MOVE:
      {
         if( (pme->m_pSoftKey && IMENUCTL_IsActive(pme->m_pSoftKey))
            || pme->m_pModeMenu || pme->m_bShowSyms ){ // For now, unhandled!
            return FALSE;
         }
         // See if they handle it
         return OEM_TextHandlePenEvt(pme->m_pText, evtCode, wParam, dwParam);
      }

      case EVT_POINTER_DOWN:
      {
         AEERect rctSK;
         int16 wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
         int16 wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);

         MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));
         // Our TextCtl Object acts as though it were a dialog in managing it's attached
         // Softkey meny, managing Symbols mode, and managing the mode menu, so mimic the 
         // Dialog behavior in those instances. Other Pen Events in the text field are 
         // uninteresting here as the text measurement and scrolling characteristics are 
         // in the OEMText layer
         if( pme->m_pSoftKey ){
            boolean bActive   = IMENUCTL_IsActive(pme->m_pSoftKey);
            boolean bInSKRect;
            
            IMENUCTL_GetRect(pme->m_pSoftKey, &rctSK);
            bInSKRect = PT_IN_RECT(wXPos, wYPos, rctSK);
            if( !bActive && bInSKRect ){  // Overlap Rect priority!!!
               if( pme->m_bActive ){
                  TextCtl_SetMode((ITextCtl *)pme, FALSE, AEE_TM_CURRENT, TRUE);
               }
               IMENUCTL_SetActive(pme->m_pSoftKey, TRUE);
               (void)IMENUCTL_HandleEvent(pme->m_pSoftKey, evtCode, wParam, dwParam);
               return TRUE;
            }else if( bActive && bInSKRect ){
               return FALSE; // Must return false, so handler can give it to SK for action!
            }else if( bActive && PT_IN_RECT(wXPos, wYPos, pme->m_rc) && !pme->m_bActive/* redundant safety */ ){ 
               // Overlapped SK accounted for at first if()
               TextCtl_ReturnToEditMode(pme, AEE_TM_CURRENT);
               return TRUE;
            }
         }
         if( pme->m_pModeMenu ){
            IMENUCTL_HandleEvent(pme->m_pModeMenu,evtCode,wParam,dwParam);
            return TRUE;
         } 
         if(pme->m_bShowSyms){
            return TextCtl_SymbolHandlePen(pme, evtCode, wXPos, wYPos);
         }

         if( !pme->m_bActive ){
            return FALSE;
         }

         // Let them decide if it is in their rect or CAPTURED... still dragging outside
         // the rect and selecting text along the way, yay!
         return OEM_TextHandlePenEvt(pme->m_pText, evtCode, wParam, dwParam);
      }

      case EVT_POINTER_UP:
      {
         boolean bRet = FALSE;
         int16 wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
         int16 wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);

         // Softkey should not handle this event directly...
         // Pen Down activates it and feeds it the down event.
         // At that point the menu is capable to respond on it's own to pen up
         // This sort of mimics IDialog which is what an ITextCtl becomes when
         // a SK is set to it (or when the mode menu or symbols box arises)
         if( !(pme->m_pSoftKey && IMENUCTL_IsActive(pme->m_pSoftKey)) ){
            if( pme->m_pModeMenu ){
               if( IMENUCTL_HandleEvent(pme->m_pModeMenu,evtCode,wParam,dwParam) ){
                  bRet = TRUE;
               } 
               MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));
               return bRet;
            } 
         }
         if(pme->m_bShowSyms){
            bRet = TextCtl_SymbolHandlePen(pme, evtCode, wXPos, wYPos);
         }else{
            if( pme->m_bActive ){
               bRet = OEM_TextHandlePenEvt(pme->m_pText, evtCode, wParam, dwParam);
            }
         }
         MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));
         return bRet;
      }
   }

   return FALSE;
}
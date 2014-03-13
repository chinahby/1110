/*===========================================================================
                              OEMITextCtl.c

  ITextCtl interface Implementation

  This file contains implementation of the ITextCtl BREW interface

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

/*=============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/01/08   bw     Build for T9

=============================================================================*/

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
#ifdef FEATURE_USES_ZI
#include "OEMZIText.h"
#else
#include "OEMText.h"
#endif
#include "AEEStdLib.h"
#include "AEEClassIDs.h"
#include "AEEPen.h"
#include "AEE_OEM.h"
#ifndef AEE_SIMULATOR
#include "Err.h"
#else
#include "BREWSimFeatures.h"
#endif
#include "AEEMenu.h"
#include "Appscommon.h"
#include "appscommonimages.brh"
#include "appscommon.brh"
#include "WMSApp.h" 
#include "AEEAnnunciator.h"

/*=====================================================================

                      PUBLIC DATA DECLARATIONS

=====================================================================*/

//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------
//add by ydc
#define TP_STARKEYSWITCH   0x08000000  //indicate if the text control switch its inputmode with star ("*") key.
#define TP_HIDEMODETITLE   0x00008000//indecate that the text control hides mode titile.

#define DEF_TEXT_SIZE               32       //  Default text allocation size...

#define MAX_CURRENT_INPUT_MODE      10

// ID of Menu Item for displaying different Text Modes
#define IDC_TEXT_MODE               IDC_COMMAND_RESERVED+1

// Base ID of Menu Item IDs that this control can use
#define IDC_TEXT_MODE_BASE          IDC_TEXT_MODE+1
// Symbol dialog padding constants
#define SYMDLG_X_ITEMBASE  (0x1)
#define SYMDLG_X_IDXPAD    (0x1)
#define SYMDLG_X_IDXAPAD   (0x2)
#define SYMDLG_X_ARROWPOS  (0x1)
#define SYMDLG_Y_ARROWPOS  (0x0)

#define OEM_IME_DIALOG_CTL_ITSIM    (0x7ffc)         //the ID of the ITSIM control.

#define OEM_IME_DIALOG_CTL_IMUI     (0x7FFE)         //the ID of the IMUI control.


#define  PTRCK_HIT_ABOVE   (0x01)      // Above the thumb, y position < thumb.y OR x position < thumb.x
#define  PTRCK_HIT_THUMB   (0x02)      // On the thumb
#define  PTRCK_HIT_ITEM    (0x02)      // Matches above, overloaded for non SB entities
#define  PTRCK_HIT_BELOW   (0x04)      // Below the thumb, y position > thumb.y + thumb.dy OR x position < thumb.x + thumb.dx
#define  PTRCK_HIT_SCRL    (0x08)      // 1: Hit Scrollbar; 0: Didn't hit Scrollbar
#define  PTRCK_HIT_ABTRI   (0x10)
#define  PTRCK_HIT_BLTRI   (0x20)
#define  PTRCK_NOTTHUMB    (PTRCK_HIT_BELOW | PTRCK_HIT_ABOVE)
#define  PTRCK_HIT_TRIAN   (PTRCK_HIT_ABTRI | PTRCK_HIT_BLTRI)

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
   long                       m_nRefs;              // reference count
   flg                        m_bActive:1;          // control is active flag
   flg                        m_bSendCmd:1;
   IMenuCtl *                 m_pSoftKey;
   IMenuCtl *                 m_pModeMenu;          // Popup - Text Mode
   int                        m_nCmdId;
   int                        m_nSymbolPage;
   boolean                    m_bShowSyms;
   boolean                    m_bShowFaceSyms;
   boolean                    m_bShowNetSyms;   
   AECHAR                     m_chEntry;
   AECHAR                     m_chShift;
   uint32                     m_dwProps;
   int                        m_nFontHeight;        // height of char cell of menu font
   AECHAR*                    m_text;
   uint16                     m_textSize;           // Size of the m_text buffer in bytes.
   uint16                     m_nMaxChars;          // Maximum number of chars to input
   IDisplay *                 m_pIDisplay;          // IDisplay interface ptr
   AECHAR *                   m_pTitle;             // title
   IShell *                   m_pIShell;            // shell interface ptr
   AEERect                    m_rc;                 // requested rectangle (Set)
   AEERect                    m_rcGet;              // actual rectangle (Get)
   OEMCONTEXT                 m_pText;
   AEETextInputMode           m_tmLast;             // Mode ID of previously selected Mode
   int                        m_dyTextMax;
   uint16                     m_nModeEnum;          // Internal BREW mode iterator
   uint32                     m_clsMe;              // My Class ID
   uint32                     m_dwPropsEx;          // Not used yet.

   PenTracker                 m_ptTracker;
   AEERect                    m_tmrc;                 //the rectangle in which the modetext is
   AEEBitmapInfo              m_bi;                   // info for destination bitmap
   // 当前输入法索引号
   byte                       m_nCurrInputMode;     // current input mode
   byte                       m_nCurrInputModeList[MAX_CURRENT_INPUT_MODE]; // current input mode list
   byte                       m_nCurrInputModeCount;// current input mode count
   
   // 文本控件顶部标题栏右侧提示字符串
   AECHAR                     m_RightTopStr[10];   
   //一般符号的总页数
   uint16                     m_SymPageNum;
   uint16                     m_SymOnePageNum;   
   int                        m_nAutoSetCurPos ;
   MultitapAutoSetState       m_eAutoState;
   uint16                     m_wResID;
#ifdef FEATURE_FUNCS_THEME   
   RGBVAL                     m_themeColor;  
#endif //FEATURE_FUNCS_THEME
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
   boolean                    m_quickkey;
   boolean                    m_quickheldkey;
   AEEEvent                   m_quickkeyeCode;
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
   IImage                    *m_pImageBg;
   uint16                     m_nBgImgResID;
   char                       m_strBgImgResFile[MAX_FILE_NAME];
   #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_S1000T)||defined(FEATURE_VERSION_W208S)
   boolean                    m_isshift;
   boolean                    m_isAlt;
   boolean                    m_isCap;
   #endif
   #ifdef FEATURE_VERSION_C01
   boolean                    m_isAvk1;
   //boolean                    m_isSymbol;
   #endif
};

#define LINEHEIGHT            (pme->m_nFontHeight)
#define TEXT_PTR(pme)         (pme->m_text)
#define TEXT_SIZE(pme)        (WSTRSIZE(pme->m_text))
#define TEXT_LENGTH(pme)      (WSTRLEN(pme->m_text))

//---------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------
static uint32           CTextCtl_AddRef(ITextCtl * pITextCtl) ;
static uint32           CTextCtl_Release(ITextCtl * pITextCtl) ;
static boolean          CTextCtl_HandleEvent(ITextCtl * pITextCtl, AEEEvent eCode, uint16 wParam, uint32 dwParam) ;
static boolean          CTextCtl_Redraw(ITextCtl * pITextCtl) ;
static void             CTextCtl_SetActive(ITextCtl * pITextCtl, boolean bActive) ;
static boolean          CTextCtl_IsActive(ITextCtl * pITextCtl) ;
static void             CTextCtl_SetRect(ITextCtl * pITextCtl, const AEERect * prc) ;
static void             CTextCtl_GetRect(ITextCtl * pITextCtl, AEERect * prc) ;
static void             CTextCtl_SetProperties(ITextCtl * pITextCtl, uint32 nProperties) ;
static uint32           CTextCtl_GetProperties(ITextCtl * pITextCtl) ;
static void             CTextCtl_Reset(ITextCtl * pITextCtl) ;
static boolean          CTextCtl_SetTitle(ITextCtl * pITextCtl, const char * pszResFile, uint16 wResID, AECHAR * pText) ;
static boolean          CTextCtl_SetText(ITextCtl * pITextCtl, const AECHAR * psz, int cch) ;
static boolean          CTextCtl_GetText(ITextCtl * pITextCtl, AECHAR * pBuffer, int nMaxChars) ;
static AECHAR *         CTextCtl_GetTextPtr(ITextCtl * pITextCtl) ;
static void             CTextCtl_EnableCommand(ITextCtl * pITextCtl, boolean bEnable, uint16 nCmdId) ;
static void             CTextCtl_SetMaxSize(ITextCtl * pITextCtl, uint16 nMaxSize) ;
static void             CTextCtl_SetSoftKeyMenu(ITextCtl * pITextCtl,IMenuCtl * pm) ;
static AEETextInputMode CTextCtl_SetInputMode(ITextCtl * po, AEETextInputMode m) ;
static int32            CTextCtl_GetCursorPos(ITextCtl * po) ;
static void             CTextCtl_SetCursorPos(ITextCtl * po, int32 nOffset) ;
static AEETextInputMode CTextCtl_GetInputMode(ITextCtl * po, AEETextInputModeInfo * pmInfo) ;
static void             CTextCtl_EnumModeInit(ITextCtl *po) ;
static AEETextInputMode CTextCtl_EnumNextMode(ITextCtl *po, AEETextInputModeInfo * pmInfo) ;

static void             CTextCtl_SetSelection(ITextCtl * po, uint32 dwSel);
static uint32           CTextCtl_GetSelection(ITextCtl * po);
static void             CTextCtl_SetPropertiesEx(ITextCtl * pITextCtl, uint32 nPropsEx);
static uint32           CTextCtl_GetPropertiesEx(ITextCtl * pITextCtl);

static void     TextCtl_SetMode (ITextCtl * pITextCtl,boolean bEdit,AEETextInputMode mode,boolean bRedraw) ;
static void     TextCtl_ShowSymbolDialog(CTextCtl * pme) ;
#ifdef FEATURE_CARRIER_CHINA_TELCOM
static uint16   TextCtl_QuerySymbols(CTextCtl *pme, AECHAR *pszOut, uint16 size) ;
#else 
static uint16   OEM_TextQuerySymbols(CTextCtl *pme, AECHAR *pszOut, uint16 size);

#endif // FEATURE_CARRIER_CHINA_TELCOM
static void     TextCtl_ShowFaceSymbolDialog(CTextCtl * pme) ;
static void     TextCtl_ShowNetSymbolDialog(CTextCtl * pme) ;
static void     TextCtl_ShowModeMenu(CTextCtl * pme) ;
static void     TextCtl_CheckEntryMode(CTextCtl * pme) ;
static void     TextCtl_ShowSymbolPage(CTextCtl * pme, int nDir) ; 
static void     TextCtl_ShowFaceSymbolPage(CTextCtl * pme, int nDir) ; 
static void     TextCtl_ShowNetSymbolPage(CTextCtl * pme, int nDir) ;
static boolean  TextCtl_IsSymbolsSupported(CTextCtl * pme) ;
static void     TextCtl_KillModeMenu(CTextCtl * pme) ;
static void     TextCtl_ClearScreen(CTextCtl * pme) ;
static AECHAR   TextCtl_GetSymChar(CTextCtl * pme, int nPos) ;
static void     TextCtl_SetTextSize(CTextCtl * pme, uint16 nMaxSize) ;
static void     TextCtl_ReturnToEditMode(CTextCtl * pme,AEETextInputMode mode) ;
static void     TextCtl_CloseSymbolDialog(CTextCtl * pme, AECHAR ch) ;
static void     TextCtl_CloseNetSymbolDialog (CTextCtl * pme, AECHAR* string) ;
static void     TextCtl_CloseFaceSymbolDialog (CTextCtl * pme, AECHAR* string) ;
static void     TextCtl_CalcRects (CTextCtl * pme) ;
static void     TextCtl_Delete(CTextCtl * pme) ;
static boolean  TextCtl_SetText(CTextCtl * pme,boolean bSetText,uint16 wID,AECHAR * pText) ;
static void		OEM_SetInputMode(CTextCtl * pme)  ;
static void		TextCtl_SetInputList(CTextCtl *pme) ;
static boolean	TextCtl_SetNextInputMode(CTextCtl *pme) ;
static void		TextCtl_3STYLE_MultitapTimer(void *pUser);
static boolean	CTextCtl_GetInfo(ITextCtl *po, uint32 type, uint32 * pvoid);
static int32    CTextCtl_GetT9End(ITextCtl * po) ;
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
static void TextCtl_QuickTimer(void *pUser);
#endif //FEATURE_ARPHIC_LAYOUT_ENGINE
static int TextCtl_Oemmode_Textmode(byte oeminputmode);
static void TextCtl_DrawBackGround(CTextCtl * pme, AEERect *pRect);

static void CTextCtl_SetBackGround(ITextCtl * po, char *pstrImgResFile, uint16 nImgResID);

static boolean TextCtl_HandlePens(CTextCtl * pme, AEEEvent evtCode, uint16 wParam, int16 wXPos, int16 wYPos); 
static boolean          TextCtl_SymbolHandlePen(CTextCtl * pme,
												AEEEvent evtCode,
												int16 wXPos,
												int16 wYPos);
static uint8            TextCtl_SymbolPagePenHitItem(CTextCtl * pme,
													 int16 wXPos,
													 int16 wYPos,
													 uint16 * pwData);
static void             TextCtl_GetSymbDlgInfo(CTextCtl * pme,
											   IDisplay * pd,
											   AEEBitmapInfo * pbi,
											   AECHAR * pszwBuff,
											   uint16 wBuffSize,
											   int * pcxAdd,
											   int * pcyAdd,
											   int16 * pwDx,
											   int16 * pwDy);

//---------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------
static boolean shortkey = TRUE;
static const AEETextInputModeInfo *sBREWTextModes;
#define MAX_SYMBOL_PAGE     (4)

//---------------------------------------------------------------------
// Global Data Definitions
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------

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
                                             CTextCtl_GetPropertiesEx,
                                             CTextCtl_GetInfo,
                                             CTextCtl_GetT9End,
                                             CTextCtl_SetBackGround,
                                             }; // add the code for when the cursor in begin, the button should be changed

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
   AEEDeviceInfo  dm;
   int            nErr;

   *ppobj = NULL;

   // allocate memory for object
   sBREWTextModes = sBREWTextModes_en;

   if((pme = (CTextCtl *)(MALLOC(sizeof(CTextCtl)))) == NULL)
      return(ENOMEMORY);

   GET_PVTBL(pme, ITextCtl)  = (VTBL(ITextCtl) * )&gQTextMethods;
   IBASE_AddRef((IBase *)pme);

   // initialize object data
   pme->m_pIShell = pIShell;
   pme->m_tmLast = AEE_TM_CURRENT;
   pme->m_nModeEnum = 0;
   pme->m_SymPageNum = 0;
   pme->m_pImageBg = NULL;
   pme->m_clsMe = clsID;
   #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_S1000T)||defined(FEATURE_VERSION_W208S)
   pme->m_isshift = FALSE;
   pme->m_isAlt = FALSE;
   pme->m_isCap = FALSE;
   #endif

   // Increment the reference count on the shell...
   ISHELL_AddRef(pIShell);

   // create IDisplay interface
   nErr = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_DISPLAY, (void **)&pme->m_pIDisplay);
   if(!pme->m_pIDisplay){
      TextCtl_Delete(pme);
      return(nErr);
   }

   // Set the default buffer...
   TextCtl_SetTextSize(pme, DEF_TEXT_SIZE);

   // obtain font metrics for later use
   pme->m_nFontHeight = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL); 
   
   // get display metrics
   ISHELL_GetDeviceInfo(pme->m_pIShell, &dm);
   
   pme->m_dyTextMax = dm.cyScreen - BOTTOMBAR_HEIGHT;

   SETAEERECT(&pme->m_rc, 0, 0, dm.cxScreen, pme->m_dyTextMax);

   TextCtl_CalcRects(pme);
   
   // Define Input List, we can change Input list here!
   MSG_FATAL("TextCtl_SetInputList...............",0,0,0);
   TextCtl_SetInputList(pme);

    // 取输入法提示文本
    pme->m_wResID = 0;   
    
#ifdef FEATURE_FUNCS_THEME   //根据feature控制，以下所有用到此feature的地方用#else处理   
    {
        // 获取主题参数
        Theme_Param_type    Theme_Param;
        Appscom_GetThemeParameters(&Theme_Param);
        pme->m_themeColor = Theme_Param.themeColor;        
    }
#endif /* FEATURE_FUNCS_THEME */ 

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
   pme->m_quickkey = TRUE; 
   pme->m_quickheldkey = FALSE; 
   pme->m_quickkeyeCode = EVT_KEY;
#endif //'#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
#ifdef FEATURE_VERSION_C01
   pme->m_isAvk1 = FALSE;
   //pme->m_isSymbol = FALSE;
#endif
   pme->m_nBgImgResID = 0;
   MEMSET(pme->m_strBgImgResFile, 0, sizeof(pme->m_strBgImgResFile)); 

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
    
    if(pme->m_pImageBg != NULL)
    {
        IImage_Release(pme->m_pImageBg);
        pme->m_pImageBg = NULL;
    }
    if (pme->m_dwProps & TP_STARKEY_SWITCH)
    {   // 保存最近使用的输入法
#ifdef FEATURE_DEFAULT_INPUT_MODE_ENG   
#ifdef FEATURE_MT_ENGLISH_CAPLOW
        pme->m_nCurrInputMode=OEM_MODE_ENGLISH_CAPLOW;
#else
        pme->m_nCurrInputMode=OEM_MODE_ENGLISH_LOW;
#endif
#endif
        (void)OEM_SetConfig(CFGI_INPUTMODE, &pme->m_nCurrInputMode, sizeof(byte));
    }
    
    if (--pme->m_nRefs != 0)
    {
        return(pme->m_nRefs);
    }

     /*quit the number method in add the contact number and ip number set on 080903*/
    OEM_ExitIputMethod(pme->m_pText);
    /* add the code end */
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    (void) ISHELL_CancelTimer((IShell *) pme->m_pIShell,
                    TextCtl_QuickTimer,
                    pme);                  
#endif //'#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
	
    TextCtl_Delete(pme);
    
    SetArrowFlagonIM(FALSE); 

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
static boolean CTextCtl_HandleEvent(ITextCtl * pITextCtl, 
                                    AEEEvent eCode, uint16 wParam, uint32 dwParam )
{
    CTextCtl * pme = (CTextCtl*)pITextCtl;
    uint32 mode;
    #ifdef  FEATURE_MYANMAR_INPUT_MOD
    boolean b_isStar = OEM_TextMyaStar(pme->m_pText);
    #endif
    if(wParam ==AVK_DEL)
    {
        wParam =AVK_CLR;
        dwParam = 1;
    }
    #ifdef FEATURE_QQ_APP
	if(wParam ==AVK_SOFT2)
    {
        wParam =AVK_CLR;
        dwParam = 1;
    }
	#endif
    MSG_FATAL("CTextCtl_HandleEvent-----eCode=%d",eCode,0,0);
#ifdef FEATURE_LCD_TOUCH_ENABLE   //add by ydc
   //if( eCode >= EVT_PEN_UP  && eCode <= EVT_PEN_STALE_MOVE ){   modi by ydc  090520
	if (eCode == EVT_PEN_UP){
      boolean bRet = FALSE;
	  AEEDeviceInfo devinfo;
	  int nBarH ;
	  AEERect rc;
	  int16   xpos, ypos;
      xpos = AEE_GET_X(dwParam);
      ypos = AEE_GET_Y(dwParam);
      // Stale move is not handled and likely the largest stream of events
      // For speed, abort quick.
      if( eCode == EVT_PEN_STALE_MOVE ){
         return FALSE;
      }
      if (pme->m_pModeMenu != NULL)
      {
          return(TextCtl_HandlePens(pme, eCode, wParam,
          (int16)AEE_GET_X(dwParam), (int16)AEE_GET_Y(dwParam)));
      }
      if (pme->m_bShowSyms)
      {
          return(TextCtl_HandlePens(pme, eCode, wParam,
          (int16)AEE_GET_X(dwParam), (int16)AEE_GET_Y(dwParam)));
      }
      if ((pme->m_dwProps & TP_STARKEYSWITCH)
      && (!(pme->m_dwProps & TP_HIDEMODETITLE))
      && (!pme->m_pSoftKey)
      && (pme->m_pTitle))
      {
          
          MSG_FATAL("TSIM_DragBar-----xpos=%d--ypos=%d",xpos,ypos,0);
          if((eCode == EVT_PEN_UP )
          && (PT_IN_RECT(xpos,ypos,pme->m_tmrc)))
          {
              return(CTextCtl_HandleEvent(pITextCtl, EVT_KEY, AVK_STAR, 0));
          }
      }
      if( (bRet = TextCtl_HandlePens(pme, eCode, wParam, (int16)AEE_GET_X(dwParam), (int16)AEE_GET_Y(dwParam))) == TRUE)
      {
          if (!(pme->m_dwProps & TP_NODRAW))
          {
              OEM_TextUpdate(pme->m_pText);
              if (OEM_TextGetCurrentMode(pme->m_pText) == AEE_TM_TSIM)
              {
                  IDISPLAY_Update(pme->m_pIDisplay);
                  return TRUE;
              }
          }
          TextCtl_CheckEntryMode(pme);
          // This belongs above the Softkey!
          OEM_TextDrawIMEDlg(pme->m_pText);
          if (!(pme->m_dwProps & TP_NODRAW))
          {
             IDISPLAY_Update(pme->m_pIDisplay);
          }
      }	
	  if(bRet)
	  {
	     return bRet;
	  }
		 
		nBarH = GetBottomBarHeight(pme->m_pIDisplay);

		MEMSET(&devinfo, 0, sizeof(devinfo));
		ISHELL_GetDeviceInfo(pme->m_pIShell, &devinfo);
		SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
		MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

		if(PT_IN_RECT(xpos,ypos,rc))
		{
			if(xpos >= rc.x + (rc.dx/3)*2 && xpos < rc.x + (rc.dx/3)*3 )//左
			{						
				eCode = EVT_KEY;
				wParam =AVK_CLR;
			}
		}
						
   
   }
#endif  //end by ydc
    switch(eCode)
    {
        case EVT_CTL_SET_TEXT:
        case EVT_CTL_SET_TITLE:
            return(TextCtl_SetText(pme,
                (boolean)(eCode == EVT_CTL_SET_TEXT),
                wParam,
                (AECHAR *)dwParam));

        case EVT_COMMAND:          // SoftKey 菜单的命令
            if (!pme->m_pSoftKey)
            {
                return FALSE;
            }

            switch(wParam)
            {
                case IDC_TEXT_MODE:
                    // 显示输入法列表
                    TextCtl_ShowModeMenu(pme);
                    return(TRUE);
    
                default:
                    if ((NULL != pme->m_pModeMenu) &&
                        IMENUCTL_IsActive(pme->m_pModeMenu) &&
                        (wParam >= IDC_TEXT_MODE_BASE))
                    {
                        //This is a Menu Item added by TextCtl. 
                        //The ItemData contains the ID of the Mode.
                        IMENUCTL_GetItemData(pme->m_pModeMenu,wParam,&mode);
    
                        //mode indicates the new mode.
                        if ((AEETextInputMode)mode == AEE_TM_SYMBOLS)
                        {
                            if (TextCtl_IsSymbolsSupported(pme))
                            {
                                TextCtl_ShowSymbolDialog(pme);
                                
                                if ((pme->m_pSoftKey) || (!(pme->m_dwProps & TP_STARKEY_SWITCH)))
                                {
                                    ISHELL_HandleEvent(pme->m_pIShell,
                                                EVT_CTL_TEXT_MODECHANGED,
                                                (uint16)mode,
                                                (uint32)pme);
                                }
                                
                                return(TRUE);
                            }
                            else // not support Symbols
                            {    
                                return FALSE;
                            }
                        }
                        else
                        {
                            //Its an OEM defined mode
                            TextCtl_ReturnToEditMode(pme,(AEETextInputMode)mode);
                            return(TRUE);
                        }
                    }
                    else
                    {
                        return FALSE;
                    }
                    // end default
            }
            return FALSE;
            
        case EVT_KEY_HELD:     
            // Press and hold of keys 0 through 9 should be passed to the OEM   
            MSG_FATAL("EVT_KEY_HELD::::%x",eCode,0,0);
		
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
			if ((wParam == AVK_SHIFT))
			{
				MSG_FATAL("***zzg CTextCtl_HandleEvent EVT_KEY_HELD AVK_SHIFT AVK_LCTRL***", 0, 0, 0);
				
				
				if(pme->m_dwProps & TP_STARKEY_SWITCH)
				{
					MSG_FATAL("***zzg CTextCtl_HandleEvent TP_STARKEY_ID_SWITCH***", 0, 0, 0);
					
					if(pme->m_nCurrInputMode == OEM_MODE_T9_MT_THAI)	//OEM_MODE_T9_MT_ENGLISH_UP
					{
						MSG_FATAL("***zzg CTextCtl_HandleEvent OEM_MODE_T9_RAPID_ENGLISH_UP***", 0, 0, 0);
						
						 pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH_UP; 
						 SetArrowFlagonIM(FALSE);
						 ISHELL_HandleEvent(pme->m_pIShell, EVT_UPDATE_ARROW_BUTTON, 0,0);
						 OEM_SetInputMode(pme);
						 return TRUE;						 
					}
					else if(pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_UP)
					{
						MSG_FATAL("***zzg CTextCtl_HandleEvent OEM_MODE_T9_RAPID_ENGLISH***", 0, 0, 0);
						
						pme->m_nCurrInputMode = OEM_MODE_T9_MT_THAI; 
						SetArrowFlagonIM(FALSE);
						ISHELL_HandleEvent(pme->m_pIShell, EVT_UPDATE_ARROW_BUTTON, 0, 0);
						OEM_SetInputMode(pme);
						return TRUE;
					}
					
				}
			}
			
#endif

            if ( OEM_TextKeyPress(pme->m_pText,eCode,wParam,dwParam) != FALSE)
            {
            		CTextCtl_Redraw((ITextCtl *)pme);
                    TextCtl_CheckEntryMode(pme);
                    return(TRUE);
            }
            if (!pme->m_bActive ||
                (wParam != AVK_CLR &&
                 wParam != AVK_STAR &&
                 wParam != AVK_POUND &&  
                 (wParam < AVK_0 || wParam > AVK_9)))
            {
                return(FALSE);
            } 

             if ( (!pme->m_pSoftKey) 
                 && (wParam == AVK_CLR) 
                 && (!pme->m_bShowSyms || !pme->m_bShowFaceSyms || !pme->m_bShowNetSyms))                  
            {              	
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE            
                if(TEXT_LENGTH(pme) >200 )
                {
                    if(!pme->m_quickheldkey)
                    { 
                        pme->m_quickkeyeCode = eCode;                     
                        // setup a Timer
                        (void) ISHELL_SetTimer((IShell *) pme->m_pIShell,
                                        2000,
                                        TextCtl_QuickTimer,
                                        pme);                     
                         return FALSE;                  
                    }
                    pme->m_quickheldkey = FALSE;
                }
#endif //FEATURE_ARPHIC_LAYOUT_ENGINE 
                goto NormalKeyEvent;
                return TRUE;
            }  

#if defined (FEATURE_ALL_KEY_PAD)	
			if ( (!pme->m_pSoftKey) 
                 && (pme->m_dwProps & TP_STARKEY_SWITCH) 
                 && (wParam == AVK_SYMBOL) 
                 && (!(pme->m_dwProps & TP_NOSYMBOL))
                 && (!pme->m_bShowSyms || !pme->m_bShowFaceSyms || !pme->m_bShowNetSyms))                  
                // 按Symbol键切换到符号输入法
#else
			if ( (!pme->m_pSoftKey) 
             && (pme->m_dwProps & TP_STARKEY_SWITCH) 
#ifdef  FEATURE_MYANMAR_INPUT_MOD
			 &&b_isStar
#endif
             && (wParam == AVK_STAR 
             #ifdef FEATURE_VERSION_C01
             || (wParam == AVK_1 && pme->m_nCurrInputMode == OEM_MODE_T9_MT_ARABIC)
             #endif
             ) 
             && (!(pme->m_dwProps & TP_NOSYMBOL))
             && (!pme->m_bShowSyms || !pme->m_bShowFaceSyms || !pme->m_bShowNetSyms))                  
            // 按*键切换到符号输入法
#endif
            {              	
            	#ifdef FEATURE_VERSION_C01
				if(wParam == AVK_1)
				{
					pme->m_isAvk1 = TRUE;
				}
				else
				{
					pme->m_isAvk1 = FALSE;
				}
				#endif
                (void)CTextCtl_SetInputMode((ITextCtl *)pme, AEE_TM_SYMBOLS);
                return TRUE;
            }  
#if defined (FEATURE_ALL_KEY_PAD)
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
            if ((!pme->m_pSoftKey) && 
                (pme->m_dwProps & TP_STARKEY_SWITCH) &&
                (wParam == AVK_0) &&
                ((!pme->m_bShowSyms)&&(!pme->m_bShowFaceSyms)&&(!pme->m_bShowNetSyms)))
#else
			if ((!pme->m_pSoftKey) && 
                (pme->m_dwProps & TP_STARKEY_SWITCH) &&
                (wParam == AVK_LCTRL) &&
                ((!pme->m_bShowSyms)&&(!pme->m_bShowFaceSyms)&&(!pme->m_bShowNetSyms)))		

#endif
#else
			if ((!pme->m_pSoftKey) && 
                (pme->m_dwProps & TP_STARKEY_SWITCH) &&
                (wParam == AVK_POUND) &&
                ((!pme->m_bShowSyms)&&(!pme->m_bShowFaceSyms)&&(!pme->m_bShowNetSyms)))
#endif
            {       
                if (!pme->m_bActive)
                {
                    return FALSE;
                }             

#if defined FEATURE_T9_MT_THAI  || defined FEATURE_T9_RAPID_THAI
                if ( !TextCtl_SetNextInputMode(pme) )
                {
                    pme->m_nCurrInputMode = pme->m_nCurrInputModeList[2];
                }
				MSG_FATAL("1pme->m_nCurrInputMode===%d",pme->m_nCurrInputMode,0,0);
                SetArrowFlagonIM(FALSE);
                ISHELL_HandleEvent(pme->m_pIShell, EVT_UPDATE_ARROW_BUTTON, 0, 0);
                OEM_SetInputMode((CTextCtl *)pme);
                shortkey = FALSE;     
                return TRUE; 
#endif //#if defined FEATURE_T9_MT_THAI  || defined FEATURE_T9_RAPID_THAI

                switch (pme->m_nCurrInputMode)
                {
#ifdef FEATURE_T9_ZHUYIN                   
                    case OEM_MODE_T9_ZHUYIN:                       
                        if ( !TextCtl_SetNextInputMode(pme) )
                        {
                            pme->m_nCurrInputMode = pme->m_nCurrInputModeList[2];
                        }
                        OEM_SetInputMode((CTextCtl *)pme);       
                         shortkey = FALSE;
                        break; 
#endif //FEATURE_T9_MT_THAI     

#ifdef FEATURE_T9_MT_THAI                   
                    case OEM_MODE_T9_MT_THAI:                       
                        if ( !TextCtl_SetNextInputMode(pme) )
                        {
                            pme->m_nCurrInputMode = pme->m_nCurrInputModeList[2];
                        }
                        OEM_SetInputMode((CTextCtl *)pme);   
                         shortkey = FALSE;                        
                        break; 
#endif //FEATURE_T9_MT_THAI  

#ifdef FEATURE_T9_RAPID_THAI                   
                    case OEM_MODE_T9_RAPID_THAI:                       
                        if ( !TextCtl_SetNextInputMode(pme) )
                        {
                            pme->m_nCurrInputMode = pme->m_nCurrInputModeList[2];
                        }
                        OEM_SetInputMode((CTextCtl *)pme);  
                         shortkey = FALSE;                        
                        break; 
  #endif //FEATURE_T9_MT_THAI    
  
                    default:
                         shortkey =TRUE;                       
                        break;                         
                }
                return TRUE;
            }             
             break;
            //return FALSE;
      
       
        case EVT_KEY:
        MSG_FATAL("EVT_KEY................1",0,0,0);
        
        MSG_FATAL("2pme->m_nCurrInputMode===%d %d",pme->m_nCurrInputMode,pme->m_dwProps,0);
		if((pme->m_dwProps & TP_MB_PROPERTY) && (wParam == AVK_UP || wParam == AVK_DOWN))
		{
			return FALSE;
		}
        #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
        if((pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_UP) && (wParam == AVK_SHIFT))
         	{
         			pme->m_nCurrInputMode = OEM_MODE_T9_MT_THAI; 
         			SetArrowFlagonIM(FALSE);
					ISHELL_HandleEvent(pme->m_pIShell, EVT_UPDATE_ARROW_BUTTON, 0, 0);
					OEM_SetInputMode(pme);
					MSG_FATAL("000000000000000011",0,0,0);
					return TRUE;
         	}
         		
         #endif
#if 1   //modi by yangdecai    //  SWITCH  INPUTMOD  EVT_RELEASE MOVE TO EVT_KEY 

#if defined (FEATURE_ALL_KEY_PAD)
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
if ((!pme->m_pSoftKey) && 
                (pme->m_dwProps & TP_STARKEY_SWITCH) &&
                (wParam == AVK_0) && 
                (!OEM_TextAltStatus(pme->m_pText)) &&
                (shortkey == TRUE) &&
                ((!pme->m_bShowSyms)&&(!pme->m_bShowFaceSyms)&&(!pme->m_bShowNetSyms)))

#else
			if ((!pme->m_pSoftKey) && 
                (pme->m_dwProps & TP_STARKEY_SWITCH) &&
                (wParam == AVK_LCTRL) &&
                (shortkey == TRUE) &&
                ((!pme->m_bShowSyms)&&(!pme->m_bShowFaceSyms)&&(!pme->m_bShowNetSyms)))
#endif
#else
			if ((!pme->m_pSoftKey) && 
                (pme->m_dwProps & TP_STARKEY_SWITCH) &&
                (wParam == AVK_POUND) &&
                (shortkey == TRUE) &&
                ((!pme->m_bShowSyms)&&(!pme->m_bShowFaceSyms)&&(!pme->m_bShowNetSyms)))
#endif
            {
            	MSG_FATAL("000000000000000012",0,0,0);
                if (!pme->m_bActive)
                {
                	MSG_FATAL("000000000000000013",0,0,0);
                    return FALSE;
                }
				MSG_FATAL("2pme->m_nCurrInputMode===%d",pme->m_nCurrInputMode,0,0);
#if defined FEATURE_T9_MT_THAI  || defined FEATURE_T9_RAPID_THAI
                 //goto NormalKeyEvent;   
#endif //#if defined FEATURE_T9_MT_THAI  || defined FEATURE_T9_RAPID_THAI

                switch (pme->m_nCurrInputMode)
                {
#ifdef FEATURE_T9_ZHUYIN                   
                    case OEM_MODE_T9_ZHUYIN:                       
                        goto NormalKeyEvent;                              
                        break; 
#endif //FEATURE_T9_MT_THAI     

#ifdef FEATURE_T9_MT_THAI                   
                    case OEM_MODE_T9_MT_THAI:                       
                        //goto NormalKeyEvent;                              
                        break; 
#endif //FEATURE_T9_MT_THAI  

#ifdef FEATURE_T9_RAPID_THAI                   
                    case OEM_MODE_T9_RAPID_THAI:                       
                        //goto NormalKeyEvent;                              
                        break; 
  #endif //FEATURE_T9_MT_THAI    
#ifdef FEATURE_MYANMAR_INPUT_MOD
					case OEM_MODE_MYANMAR:
						OEM_TextRestart(pme->m_pText);
						break;
#endif
                    default:
                        break;                    
                }  
				MSG_FATAL("3pme->m_nCurrInputMode===%d",pme->m_nCurrInputMode,0,0);
                if ( !TextCtl_SetNextInputMode(pme) )
                {
                    pme->m_nCurrInputMode = pme->m_nCurrInputModeList[2];
                }
                MSG_FATAL("4pme->m_nCurrInputMode===%d",pme->m_nCurrInputMode,0,0);
                SetArrowFlagonIM(FALSE);
                ISHELL_HandleEvent(pme->m_pIShell, EVT_UPDATE_ARROW_BUTTON, 0, 0);
                OEM_SetInputMode((CTextCtl *)pme);
                return TRUE;
            }  
#if defined (FEATURE_DISP_160X128)
#if 0
            if ((!pme->m_pSoftKey) && (pme->m_dwProps & TP_STARKEY_SWITCH) &&
                (wParam == AVK_CAPLK) && (shortkey == TRUE) &&
                ((!pme->m_bShowSyms)&&(!pme->m_bShowFaceSyms)&&(!pme->m_bShowNetSyms)))
                {
                    if(pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_ENGLISH;
                    }
                    else if(pme->m_nCurrInputMode == OEM_MODE_T9_RAPID_ENGLISH)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH;
                    }
				#ifdef FEATURE_INPUTMODE_INDONESIAN
					else if(pme->m_nCurrInputMode == OEM_MODE_T9_MT_INDONESIAN)
					{
						pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_INDONESIAN;
					}
					else if(pme->m_nCurrInputMode == OEM_MODE_T9_RAPID_INDONESIAN)
					{
						pme->m_nCurrInputMode = OEM_MODE_T9_MT_INDONESIAN;
					}
				#endif
                    OEM_SetInputMode((CTextCtl *)pme);
                    return TRUE;
                }
#endif
#endif
#else //AEE_SIMULATOR       
#if MIN_BREW_VERSION(3,0)
            // do not want to handle au
            if (((dwParam & KB_AUTOREPEAT) != 0) &&
                ((wParam == AVK_STAR) ||(wParam == AVK_POUND)||(wParam == AVK_CLR)) &&
                ((!pme->m_bShowSyms || !pme->m_bShowFaceSyms || !pme->m_bShowNetSyms)))
            {
                return TRUE;
            }
#endif                
            
            //if((wParam == AVK_STAR) ||(wParam == AVK_POUND) )
            //{
             //   return FALSE;                
            //}  
            
#endif //AEE_SIMULATOR   
#ifdef FEATURE_ALL_KEY_PAD
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pITextCtl);
                
                if((pme->m_nMaxChars == WSTRLEN(pwstrText)) && (wParam >= AVK_0 && wParam <= AVK_9))
                {
                    ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TEXT_TEXTFULL, 0, 0);
                    MSG_FATAL("EVT_KEY................2",0,0,0);
                    return FALSE;
                }
            }
#endif
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
            if (wParam >= AVK_0 && wParam <= AVK_9)
            {
                if(!pme->m_quickkey)
                { 
                     return FALSE;                  
                }
                else
                {
                    pme->m_quickkey = FALSE;     
                    pme->m_quickkeyeCode = eCode;                       
                    // setup a Timer
                    (void) ISHELL_SetTimer((IShell *) pme->m_pIShell,
                                    100,
                                    TextCtl_QuickTimer,
                                    pme);                  
                }
            }
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE  

            if (wParam == AVK_UNDEFINED) 
            {
                // This allows us to force an asynchronous update!
                TextCtl_CheckEntryMode(pme);
                
                if (!(pme->m_dwProps & TP_NODRAW))
                {
                    IDISPLAY_Update(pme->m_pIDisplay);
                }
                return TRUE;
            }
    
            // See if the Mode menu is displayed...
            if (pme->m_pModeMenu != NULL) 
            {
            	MSG_FATAL("EVT_KEY................3",0,0,0);
                if (IMENUCTL_HandleEvent(pme->m_pModeMenu,eCode,wParam,dwParam)) 
                {
                    return TRUE;
                } 
                else if (AVK_CLR == wParam || AVK_SOFT2== wParam) 
                {
                    TextCtl_KillModeMenu(pme);
                    
                    IMENUCTL_SetActive(pme->m_pSoftKey, TRUE);
                    CTextCtl_Redraw((ITextCtl *)pme);
                    return TRUE;
                } 
                else 
                {
                    return FALSE;
                }
            }
    
            // Handle the UP/DOWN key to return us to edit mode/Send Tab event...
            if (pme->m_pSoftKey &&  IMENUCTL_IsActive(pme->m_pSoftKey))
            {
                if (wParam == AVK_UP)
                {
                    TextCtl_ReturnToEditMode(pme, pme->m_tmLast);
                    return TRUE;
                } 
                else if(wParam == AVK_DOWN)
                {
                    return ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)1, (uint32)pme);
                }
            }

            if ( pme->m_bShowSyms )
            {
                switch (wParam)
                {
                    case AVK_UP:
                        TextCtl_ShowSymbolPage(pme, -1);
                        break;
    
                    case AVK_DOWN:
                        TextCtl_ShowSymbolPage(pme, 1);
                        break;

					//Add By zzg 2010_08_13
					case AVK_LEFT:
					case AVK_RIGHT:		
						TextCtl_ShowSymbolPage(pme, 0);
                        break;
                        
					//Add End

					//wangliang add! 2010-10-06
					case AVK_SOFT2:
                    case AVK_CLR:
                       TextCtl_CloseSymbolDialog(pme, 0);
                        return(TRUE);

                    case AVK_SELECT:
                    case AVK_INFO:
                        return TRUE;//在输入标点符号时，这两个按键无效
    
                    default:
                        if (wParam < AVK_1 || wParam > AVK_9)
                        {
							TextCtl_ShowSymbolPage(pme, 0);
							break;
                        }
                        TextCtl_CloseSymbolDialog(pme, TextCtl_GetSymChar(pme,(wParam - AVK_1)));

                }
                return(TRUE);
            }

            // See if we have the symbols showing...
            if ( pme->m_bShowFaceSyms )
            {
                switch (wParam)
                {
                    case AVK_UP:
                        TextCtl_ShowFaceSymbolPage(pme, -1);
                        break;
    
                    case AVK_DOWN:
                        TextCtl_ShowFaceSymbolPage(pme, 1);
                        break;

					//wangliang add! 2010-10-06
    				case AVK_SOFT2:
                    case AVK_CLR:
                       TextCtl_CloseFaceSymbolDialog(pme, NULL);
                        return(TRUE);

                    case AVK_SELECT:
                    case AVK_INFO:
                        return TRUE;//Q1 要求在输入符号时 select 跟info 键无效
    
                    default:
                        if (wParam < AVK_1 || wParam > AVK_9)
                        {
                            return(FALSE);
                        }
                        {
                            int    index;
                            index =  (wParam - AVK_1)+pme->m_SymOnePageNum*pme->m_nSymbolPage;                            
                           if(((wParam - AVK_1) >= pme->m_SymOnePageNum) ||(index >= MAX_FACE_SYMBOL))
                            {
                                return(FALSE);
                            }                                  
                            TextCtl_CloseFaceSymbolDialog(pme, (AECHAR*)sszFaceSymbolList[index]);
                        }

                }
                return(TRUE);
            }
            
            // See if we have the symbols showing...
            if ( pme->m_bShowNetSyms )
            {
                switch (wParam)
                {
                    case AVK_UP:
                        TextCtl_ShowNetSymbolPage(pme, -1);
                        break;
    
                    case AVK_DOWN:
                        TextCtl_ShowNetSymbolPage(pme, 1);
                        break;
                        
    				//wangliang add! 2010-10-06
					case AVK_SOFT2:
                    case AVK_CLR:
                    case AVK_SELECT: 

                       TextCtl_CloseNetSymbolDialog(pme, NULL);
                        return(TRUE);
    
                    default:
                        if (wParam < AVK_1 || wParam > AVK_9)
                        {
                            return(FALSE);
                        }
                        {
                            int    index;                            
                            index =  (wParam - AVK_1)+pme->m_SymOnePageNum*pme->m_nSymbolPage;                            
                           if(((wParam - AVK_1) >= pme->m_SymOnePageNum) ||(index >= MAX_NET_SYMBOL))
                            {
                                return(FALSE);
                            }                                                        
                            TextCtl_CloseNetSymbolDialog(pme, (AECHAR*)sszNetSymbolList[index]);
                        }

                }
                return(TRUE);
            }
            else
            {
#if defined (FEATURE_ALL_KEY_PAD)
				if ((wParam == AVK_SYMBOL))/*&&((pme->m_nCurrInputMode == OEM_MODE_T9_PINYIN)||(pme->m_nCurrInputMode == OEM_MODE_T9_STROKE))*/ //modi by yangdecai
#else
	
				if ((wParam == AVK_STAR 
#ifdef FEATURE_VERSION_C01
                || ((wParam == AVK_1) && (pme->m_nCurrInputMode == OEM_MODE_T9_MT_ARABIC) &&(!pme->m_bShowSyms))
#endif
				)
#ifdef  FEATURE_MYANMAR_INPUT_MOD
				&&b_isStar
#endif
                && (pme->m_dwProps & TP_STARKEY_SWITCH)
			    )
#endif
				{
#ifdef FEATURE_VERSION_C01
					if(wParam == AVK_1)
					{
						pme->m_isAvk1 = TRUE;
					}
					else
					{
						pme->m_isAvk1 = FALSE;
					}
#endif
					//Add By zzg 2010_08_14 防止重复载入SYMBOL输入法				
					if (AEE_TM_SYMBOLS != CTextCtl_GetInputMode((ITextCtl *)pme, NULL)
                        && (!(pme->m_dwProps & TP_NOSYMBOL)))
					{						
						(void)CTextCtl_SetInputMode((ITextCtl *)pme, AEE_TM_SYMBOLS);
						return TRUE;
					}
					//Add End
				}
/*
#ifdef FEATURE_VERSION_C01

			 if ((wParam == AVK_1) && (pme->m_nCurrInputMode == OEM_MODE_T9_MT_THAI) &&(!pme->m_bShowSyms))
            {
            	//(void)CTextCtl_SetInputMode((ITextCtl *)pme, AEE_TM_SYMBOLS);
            	//return TRUE;
				#ifdef FEATURE_VERSION_C01
				if(wParam == AVK_1)
				{
					   pme->m_isAvk1 = TRUE;
					   //pme->m_isSymbol = TRUE;
				}
				else
				{
					pme->m_isAvk1 = FALSE;
				}
				#endif

            	//Add By zzg 2010_08_14	防止重复载入SYMBOL输入法            	
            	if (AEE_TM_SYMBOLS != CTextCtl_GetInputMode((ITextCtl *)pme, NULL))
            	{	            		
					(void)CTextCtl_SetInputMode((ITextCtl *)pme, AEE_TM_SYMBOLS);
					return TRUE;
				}
            	//Add End                
 #endif
            }*/
            }
            // If they pressed the select key, activate the menu and deactivate us...Before processing the
            //AVK_SELECT key ourselves, give it to the OEMs and see if they want to handle it. This is
            //specifically for multibyte character sets (Japanes) where pressing the SELECT key after
            //pressing a key is the means to complete the character in that language.
    
            //Do not process keys if text ctl is not active
            if ( pme->m_pText && TEXT_PTR(pme)!=NULL && pme->m_bActive )
            {
                // CLEAR ALL will be passed in as EVT_KEY_HELD here
                // and requires no special handling
    
NormalKeyEvent:
                MSG_FATAL("3pme->m_nCurrInputMode===%d",pme->m_nCurrInputMode,0,0);

				#ifdef FEATURE_USES_ZI
				if ( OEM_MODE_ZI_MT_ENGLISH_LOW == pme->m_nCurrInputMode
                     ||OEM_MODE_ZI_MT_ENGLISH_UP == pme->m_nCurrInputMode 
                     ||OEM_MODE_ZI_MT_ENGLISH == pme->m_nCurrInputMode )
				#else
                if ( OEM_MODE_T9_MT_ENGLISH_LOW == pme->m_nCurrInputMode
                     ||OEM_MODE_T9_MT_ENGLISH_UP == pme->m_nCurrInputMode 
                     ||OEM_MODE_T9_MT_ENGLISH == pme->m_nCurrInputMode )
                #endif
                {
                    // if MultitapCapsState have been change to all small in OEMText,
                    //then change input mode
                    #ifdef FEATURE_USES_ZI
                    if ( OEM_MODE_ZI_MT_ENGLISH == pme->m_nCurrInputMode
                         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
                    #else
					if ( OEM_MODE_T9_MT_ENGLISH == pme->m_nCurrInputMode
                         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
					#endif
                    {
                       // change to the sll small mode
                       #ifdef FEATURE_USES_ZI
                       pme->m_nCurrInputMode = OEM_MODE_ZI_MT_ENGLISH_LOW;
					   #else
					   pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH_LOW;
					   #endif
                       OEM_SetInputMode((CTextCtl *)pme);
                       pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
                    }
                    
                    // if it have been Auto set to first cap and move the cursor then 
                    //change to all small mode
                    /*else if ( ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState 
                           || MULTITAP_USER_DENY_AUTOSET == pme->m_eAutoState )
                         && OEM_TextGetSelEnd(pme->m_pText) != pme->m_nAutoSetCurPos
                         && OEM_TextGetSelEnd(pme->m_pText) == OEM_TextGetCursorPos(pme->m_pText) )
                    {
                        if ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState )
                        {
                            // change to the all small mode
                            pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH_LOW;
                            OEM_SetInputMode((CTextCtl *)pme);
                        }
                        
                        pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
                    } */        
                               
                    // if the cursor is in Right position then change to FirstCap.
                    else if (
						#ifdef FEATURE_USES_ZI
						OEM_MODE_ZI_MT_ENGLISH_LOW == pme->m_nCurrInputMode
						#else
						OEM_MODE_T9_MT_ENGLISH_LOW == pme->m_nCurrInputMode
						#endif
                         // && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState 
                         &&( MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState ||   MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState)
                         && OEM_isFirstCap(pme->m_pText))
                    {
                       // change to the fist cap mode
                       #ifdef FEATURE_USES_ZI
                       pme->m_nCurrInputMode = OEM_MODE_ZI_MT_ENGLISH;
					   #else
					   pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH;
					   #endif
                       OEM_SetInputMode((CTextCtl *)pme);
                       
                       // set flag
                       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
                       
                       // save the Cursor Pos when Auto Set to first cap.
                       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
                    }                      
                }
				#ifdef FEATURE_USES_ZI
                if ( OEM_MODE_ZI_RAPID_ENGLISH_LOW == pme->m_nCurrInputMode
                     ||OEM_MODE_ZI_RAPID_ENGLISH_UP == pme->m_nCurrInputMode 
                     ||OEM_MODE_ZI_RAPID_ENGLISH == pme->m_nCurrInputMode )
                #else
				if ( OEM_MODE_T9_RAPID_ENGLISH_LOW == pme->m_nCurrInputMode
                     ||OEM_MODE_T9_RAPID_ENGLISH_UP == pme->m_nCurrInputMode 
                     ||OEM_MODE_T9_RAPID_ENGLISH == pme->m_nCurrInputMode )
				#endif
                {
                    // if MultitapCapsState have been change to all small in OEMText,
                    //then change input mode
                    #ifdef FEATURE_USES_ZI
                    if ( OEM_MODE_ZI_RAPID_ENGLISH == pme->m_nCurrInputMode
                         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
                    #else
					if ( OEM_MODE_T9_RAPID_ENGLISH == pme->m_nCurrInputMode
                         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
					#endif
                    {
                       // change to the sll small mode
                       #ifdef FEATURE_USES_ZI
					   pme->m_nCurrInputMode = OEM_MODE_ZI_RAPID_ENGLISH_LOW;
					   #else
                       pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_ENGLISH_LOW;
					   #endif
                       OEM_SetInputMode((CTextCtl *)pme);
                       pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
                    }
                    
                    // if it have been Auto set to first cap and move the cursor then 
                    //change to all small mode
                    /*else if ( ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState 
                           || MULTITAP_USER_DENY_AUTOSET == pme->m_eAutoState )
                         && OEM_TextGetSelEnd(pme->m_pText) != pme->m_nAutoSetCurPos
                         && OEM_TextGetSelEnd(pme->m_pText) == OEM_TextGetCursorPos(pme->m_pText) )
                    {
                        if ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState )
                        {
                            // change to the all small mode
                            pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_ENGLISH_LOW;
                            OEM_SetInputMode((CTextCtl *)pme);
                        }
                        
                        pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
                    }*/         
                               
                    // if the cursor is in Right position then change to FirstCap.
                    else if ( 
						#ifdef FEATURE_USES_ZI
						OEM_MODE_ZI_RAPID_ENGLISH_LOW == pme->m_nCurrInputMode
						#else
						OEM_MODE_T9_RAPID_ENGLISH_LOW == pme->m_nCurrInputMode
						#endif
                         && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState
                         && OEM_isFirstCap(pme->m_pText))
                    {
                       // change to the fist cap mode
                       #ifdef FEATURE_USES_ZI
					   pme->m_nCurrInputMode = OEM_MODE_ZI_RAPID_ENGLISH;
					   #else
                       pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_ENGLISH;
					   #endif
                       OEM_SetInputMode((CTextCtl *)pme);
                       
                       // set flag
                       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
                       
                       // save the Cursor Pos when Auto Set to first cap.
                       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
                    }
                }                

#ifdef FEATURE_T9_MT_SPANISH
		 if ( OEM_MODE_T9_MT_SPANISH_LOW == pme->m_nCurrInputMode
                     ||OEM_MODE_T9_MT_SPANISH_UP == pme->m_nCurrInputMode 
                     ||OEM_MODE_T9_MT_SPANISH == pme->m_nCurrInputMode )
                {
                    // if MultitapCapsState have been change to all small in OEMText,
                    //then change input mode
                    if ( OEM_MODE_T9_MT_SPANISH == pme->m_nCurrInputMode
                         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
                    {
                       // change to the sll small mode
                       pme->m_nCurrInputMode = OEM_MODE_T9_MT_SPANISH_LOW;
                       OEM_SetInputMode((CTextCtl *)pme);
                       pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
                    }       
                               
                    // if the cursor is in Right position then change to FirstCap.
                    else if (OEM_MODE_T9_MT_SPANISH_LOW == pme->m_nCurrInputMode
                         // && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState  
                         &&( MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState ||   MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState)
                         && OEM_isFirstCap(pme->m_pText))
                    {
                       // change to the fist cap mode
                       pme->m_nCurrInputMode = OEM_MODE_T9_MT_SPANISH;
                       OEM_SetInputMode((CTextCtl *)pme);
                       
                       // set flag
                       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
                       
                       // save the Cursor Pos when Auto Set to first cap.
                       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
                    }                      
                }
#endif
#ifdef FEATURE_T9_RAPID_SPANISH
		 if ( OEM_MODE_T9_RAPID_SPANISH_LOW == pme->m_nCurrInputMode
                     ||OEM_MODE_T9_RAPID_SPANISH_UP == pme->m_nCurrInputMode 
                     ||OEM_MODE_T9_RAPID_SPANISH == pme->m_nCurrInputMode )
                {
                    // if MultitapCapsState have been change to all small in OEMText,
                    //then change input mode
                    if ( OEM_MODE_T9_RAPID_SPANISH == pme->m_nCurrInputMode
                         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
                    {
                       // change to the sll small mode
                       pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_SPANISH_LOW;
                       OEM_SetInputMode((CTextCtl *)pme);
                       pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
                    }
		      // if the cursor is in Right position then change to FirstCap.
                    else if ( OEM_MODE_T9_RAPID_SPANISH_LOW == pme->m_nCurrInputMode
                         && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState
                         && OEM_isFirstCap(pme->m_pText))
                    {
                       // change to the fist cap mode
                       pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_SPANISH;
                       OEM_SetInputMode((CTextCtl *)pme);
                       
                       // set flag
                       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
                       
                       // save the Cursor Pos when Auto Set to first cap.
                       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
                    }
		  }
#endif
#ifdef FEATURE_T9_MT_FRENCH
		 if ( OEM_MODE_T9_MT_FRENCH_LOW == pme->m_nCurrInputMode
                     ||OEM_MODE_T9_MT_FRENCH_UP == pme->m_nCurrInputMode 
                     ||OEM_MODE_T9_MT_FRENCH == pme->m_nCurrInputMode )
                {
                    // if MultitapCapsState have been change to all small in OEMText,
                    //then change input mode
                    if ( OEM_MODE_T9_MT_FRENCH == pme->m_nCurrInputMode
                         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
                    {
                       // change to the sll small mode
                       pme->m_nCurrInputMode = OEM_MODE_T9_MT_FRENCH_LOW;
                       OEM_SetInputMode((CTextCtl *)pme);
                       pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
                    }       
                               
                    // if the cursor is in Right position then change to FirstCap.
                    else if (OEM_MODE_T9_MT_FRENCH_LOW == pme->m_nCurrInputMode
                         // && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState  
                         &&( MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState ||   MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState)
                         && OEM_isFirstCap(pme->m_pText))
                    {
                       // change to the fist cap mode
                       pme->m_nCurrInputMode = OEM_MODE_T9_MT_FRENCH;
                       OEM_SetInputMode((CTextCtl *)pme);
                       
                       // set flag
                       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
                       
                       // save the Cursor Pos when Auto Set to first cap.
                       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
                    }                      
                }
#endif
#ifdef FEATURE_T9_RAPID_FRENCH
		 if ( OEM_MODE_T9_RAPID_FRENCH_LOW == pme->m_nCurrInputMode
                     ||OEM_MODE_T9_RAPID_FRENCH_UP == pme->m_nCurrInputMode 
                     ||OEM_MODE_T9_RAPID_FRENCH == pme->m_nCurrInputMode )
                {
                    // if MultitapCapsState have been change to all small in OEMText,
                    //then change input mode
                    if ( OEM_MODE_T9_RAPID_FRENCH == pme->m_nCurrInputMode
                         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
                    {
                       // change to the sll small mode
                       pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_FRENCH_LOW;
                       OEM_SetInputMode((CTextCtl *)pme);
                       pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
                    }
		      // if the cursor is in Right position then change to FirstCap.
                    else if ( OEM_MODE_T9_RAPID_FRENCH_LOW == pme->m_nCurrInputMode
                         && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState
                         && OEM_isFirstCap(pme->m_pText))
                    {
                       // change to the fist cap mode
                       pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_FRENCH;
                       OEM_SetInputMode((CTextCtl *)pme);
                       
                       // set flag
                       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
                       
                       // save the Cursor Pos when Auto Set to first cap.
                       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
                    }
		  }
#endif
				MSG_FATAL("OEM_TextKeyPress::::%x = %x",eCode,wParam,0);
				#ifdef FEATURE_ALL_KEY_PAD
				if((wParam == AVK_CLR) && (dwParam == 0))
				{
					return FALSE;
				}
				else
				#endif
				{
                if ( OEM_TextKeyPress(pme->m_pText,eCode,wParam,dwParam) != FALSE)
                {
                    if (!(pme->m_dwProps & TP_NODRAW))
                    {
                        OEM_TextUpdate(pme->m_pText);
                    }
                    
                    TextCtl_CheckEntryMode(pme);
                    
                    if (!(pme->m_dwProps & TP_NODRAW))
                    {
                        IDISPLAY_Update(pme->m_pIDisplay);
                    }
                    
                    return(TRUE);
                }
                }
            }
            
            // If they pressed the select key, activate the menu and deactivate us...Have the special
            // check for EVT_KEY_RELEASe because we might have comere here as a result of the goto
            //statement in EVT_KEY_RELEASE
            if (eCode != EVT_KEY_RELEASE && 
                ((wParam == AVK_SELECT)||(wParam == AVK_INFO)) && 
                pme->m_bActive && pme->m_pSoftKey ) 
            {
                // First try to change focus using the EVT_CTL_TAB event.  This will work if the ITextCtl
                // is contained in a dialog or the applet is handling EVT_CTL_TAB events.
                //
                // Unfortunatly this will not necessarily make the pSoftKey control active if
                // there is a third control in the dialog.  However we need to do this inorder to
                // inform the IDialog object that we are no longer the active control.
                ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)1, (uint32)pme);
    
                // The ISHELL_HandleEvent() call is blocking call, so if we are still the active control
                // then the EVT_CTL_TAB event didn't work.   Manually change focus to pSoftkey...
                if (pme->m_bActive) 
                {
                    TextCtl_SetMode(pITextCtl, FALSE, AEE_TM_CURRENT, TRUE);
                    IMENUCTL_SetSel(pme->m_pSoftKey, (uint16)-1);
                    IMENUCTL_SetActive(pme->m_pSoftKey, TRUE);
                }
                return(TRUE);
            }
          
            // Checks for Tabbing events, or CR
            if(eCode == EVT_KEY && pme->m_bActive && wParam == AVK_UP)
            {
                return ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)0, (uint32)pme);
            }
            // If no Softkey and the Down was not handled this will generate a tabbing event.
        
            if(eCode == EVT_KEY && pme->m_bActive && wParam == AVK_DOWN)
            {
                return ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)1, (uint32)pme);
            }
            
            break;
        	

        case EVT_KEY_RELEASE:  

			if ((wParam != AVK_SYMBOL))
         	{
         		#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)|| defined(FEATURE_VERSION_S1000T)||defined(FEATURE_VERSION_W208S)
         		boolean TempShift = FALSE;
         		boolean TempAlt = FALSE;
         		boolean TempCap = FALSE;
         		MSG_FATAL("EVT_KEY_RELEASE  AVK_SHIFT",0,0,0);
         		TempCap = OEM_TextCapStatus(pme->m_pText);
         		TempShift = OEM_TextShiftStatus(pme->m_pText);
         		TempAlt = OEM_TextAltStatus(pme->m_pText);
         		MSG_FATAL("EVT_KEY_RELEASE  AVK_SHIFT TempShift=%d",TempShift,0,0);
         		MSG_FATAL("EVT_KEY_RELEASE  AVK_SHIFT pme->m_isshift=%d",pme->m_isshift,0,0);
         		if((TempShift != pme->m_isshift) || ((TempAlt != pme->m_isAlt)) || ((TempCap != pme->m_isCap)))
         		{
         			MSG_FATAL("EVT_KEY_RELEASE  AVK_SHIFT 2222",0,0,0);
					CTextCtl_Redraw((ITextCtl *)pme);
				}
				
				#endif
			}
#if defined (FEATURE_ALL_KEY_PAD)
			if ((wParam == AVK_SYMBOL))/*&&((pme->m_nCurrInputMode == OEM_MODE_T9_PINYIN)||(pme->m_nCurrInputMode == OEM_MODE_T9_STROKE))*/ //modi by yangdecai
#else

			 if ((wParam == AVK_STAR)
#ifdef  FEATURE_MYANMAR_INPUT_MOD
			 &&b_isStar
#endif
             && (pme->m_dwProps & TP_STARKEY_SWITCH) 
		    )
#endif
            {
            	//(void)CTextCtl_SetInputMode((ITextCtl *)pme, AEE_TM_SYMBOLS);
            	//return TRUE;
								
            	//Add By zzg 2010_08_14	防止重复载入SYMBOL输入法            	
            	if (AEE_TM_SYMBOLS != CTextCtl_GetInputMode((ITextCtl *)pme, NULL)
                    && AEE_TM_NUMBERS!= CTextCtl_GetInputMode((ITextCtl *)pme, NULL)
                    && (!(pme->m_dwProps & TP_NOSYMBOL)))
            	{
					(void)CTextCtl_SetInputMode((ITextCtl *)pme, AEE_TM_SYMBOLS);
					return TRUE;
				}
            	//Add End
            }
#ifndef FEATURE_ALL_KEY_PAD
            if ((!pme->m_pSoftKey) &&       // press the * key switch the Ab(ab,AB) and the En(en,EN) inputmethod 
                (pme->m_dwProps & TP_STARKEY_SWITCH) &&
                (wParam == AVK_STAR) &&
                ((!pme->m_bShowSyms)&&(!pme->m_bShowFaceSyms)&&(!pme->m_bShowNetSyms)))
            {
                if (!pme->m_bActive)
                {
                    return FALSE;
                }                

                // if the cursor is in Right position then change to FirstCap.
                //it should check Key release event 
                        #ifdef FEATURE_USES_ZI
						if ( ( OEM_MODE_ZI_MT_ENGLISH == pme->m_nCurrInputMode         // Ab --> ab --> AB --> Ab inputmethod switch
		                       ||OEM_MODE_ZI_MT_ENGLISH_LOW == pme->m_nCurrInputMode )
		                       ||OEM_MODE_ZI_MT_ENGLISH_UP == pme->m_nCurrInputMode )
						#else
		                if ( ( OEM_MODE_T9_MT_ENGLISH == pme->m_nCurrInputMode         // Ab --> ab --> AB --> Ab inputmethod switch
		                       ||OEM_MODE_T9_MT_ENGLISH_LOW == pme->m_nCurrInputMode )
		                       ||OEM_MODE_T9_MT_ENGLISH_UP == pme->m_nCurrInputMode )
		                #endif
		                {
		                	#ifdef FEATURE_USES_ZI
							if (OEM_MODE_ZI_MT_ENGLISH == pme->m_nCurrInputMode)
		                    {
		                        pme->m_nCurrInputMode = OEM_MODE_ZI_MT_ENGLISH_LOW;    // Ab --> ab inputmethod switch
		                    }
		                    else if (OEM_MODE_ZI_MT_ENGLISH_LOW == pme->m_nCurrInputMode)
		                    {
		                        pme->m_nCurrInputMode = OEM_MODE_ZI_MT_ENGLISH_UP;     // ab --> AB inputmethod switch
		                    }
		                    else if(OEM_MODE_ZI_MT_ENGLISH_UP == pme->m_nCurrInputMode)
		                    {
		                        pme->m_nCurrInputMode = OEM_MODE_ZI_MT_ENGLISH;        // AB --> Ab inputmethod switch
		                    }
							#else
		                    if (OEM_MODE_T9_MT_ENGLISH == pme->m_nCurrInputMode)
		                    {
		                        pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH_LOW;    // Ab --> ab inputmethod switch
		                    }
		                    else if (OEM_MODE_T9_MT_ENGLISH_LOW == pme->m_nCurrInputMode)
		                    {
		                        pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH_UP;     // ab --> AB inputmethod switch
		                    }
		                    else if(OEM_MODE_T9_MT_ENGLISH_UP == pme->m_nCurrInputMode)
		                    {
		                        pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH;        // AB --> Ab inputmethod switch
		                    }
							#endif
		        

		                   OEM_SetInputMode((CTextCtl *)pme);
		                   
		                   // set flag
		                   pme->m_eAutoState = MULTITAP_USER_DENY_AUTOSET;
                   
                		}
                

                // if the cursor is in Right position then change to FirstCap.
                //it should check Key release event 
                #ifdef FEATURE_USES_ZI
				if ( ( OEM_MODE_ZI_RAPID_ENGLISH == pme->m_nCurrInputMode         // En --> en --> EN --> En inputmethod switch
                       ||OEM_MODE_ZI_RAPID_ENGLISH_LOW == pme->m_nCurrInputMode )
                       ||OEM_MODE_ZI_RAPID_ENGLISH_UP == pme->m_nCurrInputMode )
                {
                    if (OEM_MODE_ZI_RAPID_ENGLISH == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_ZI_RAPID_ENGLISH_LOW;    // En --> en inputmethod switch
                    }
                    else if (OEM_MODE_ZI_RAPID_ENGLISH_LOW == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_ZI_RAPID_ENGLISH_UP;     // en --> EN inputmethod switch
                    }
                    else if(OEM_MODE_ZI_RAPID_ENGLISH_UP == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_ZI_RAPID_ENGLISH;        // EN --> En inputmethod switch
                    }

                   OEM_SetInputMode((CTextCtl *)pme);
                   
                   // set flag
                   pme->m_eAutoState = MULTITAP_USER_DENY_AUTOSET;
                }
				#else
                if ( ( OEM_MODE_T9_RAPID_ENGLISH == pme->m_nCurrInputMode         // En --> en --> EN --> En inputmethod switch
                       ||OEM_MODE_T9_RAPID_ENGLISH_LOW == pme->m_nCurrInputMode )
                       ||OEM_MODE_T9_RAPID_ENGLISH_UP == pme->m_nCurrInputMode )
                {
                    if (OEM_MODE_T9_RAPID_ENGLISH == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_ENGLISH_LOW;    // En --> en inputmethod switch
                    }
                    else if (OEM_MODE_T9_RAPID_ENGLISH_LOW == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_ENGLISH_UP;     // en --> EN inputmethod switch
                    }
                    else if(OEM_MODE_T9_RAPID_ENGLISH_UP == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_ENGLISH;        // EN --> En inputmethod switch
                    }

                   OEM_SetInputMode((CTextCtl *)pme);
                   
                   // set flag
                   pme->m_eAutoState = MULTITAP_USER_DENY_AUTOSET;
                }     
				#endif
#ifdef FEATURE_T9_MT_SPANISH
		 if ( ( OEM_MODE_T9_MT_SPANISH == pme->m_nCurrInputMode
                       ||OEM_MODE_T9_MT_SPANISH_LOW == pme->m_nCurrInputMode )
                       ||OEM_MODE_T9_MT_SPANISH_UP == pme->m_nCurrInputMode )
                {
                    if (OEM_MODE_T9_MT_SPANISH == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_MT_SPANISH_LOW;
                    }
                    else if (OEM_MODE_T9_MT_SPANISH_LOW == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_MT_SPANISH_UP;
                    }
                    else if(OEM_MODE_T9_MT_SPANISH_UP == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_MT_SPANISH;
                    }

                   OEM_SetInputMode((CTextCtl *)pme);
                   
                   // set flag
                   pme->m_eAutoState = MULTITAP_USER_DENY_AUTOSET;
                   
                } 
#endif

#ifdef FEATURE_T9_RAPID_SPANISH
		if ( ( OEM_MODE_T9_RAPID_SPANISH== pme->m_nCurrInputMode
                       ||OEM_MODE_T9_RAPID_SPANISH_LOW == pme->m_nCurrInputMode )
                       ||OEM_MODE_T9_RAPID_SPANISH_UP == pme->m_nCurrInputMode )
                {
                    if (OEM_MODE_T9_RAPID_SPANISH == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_SPANISH_LOW;
                    }
                    else if (OEM_MODE_T9_RAPID_SPANISH_LOW == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_SPANISH_UP;
                    }
                    else if(OEM_MODE_T9_RAPID_SPANISH_UP == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_SPANISH;
                    }

                   OEM_SetInputMode((CTextCtl *)pme);
                   
                   // set flag
                   pme->m_eAutoState = MULTITAP_USER_DENY_AUTOSET;
                } 
#endif
#ifdef FEATURE_T9_MT_FRENCH
		 if ( ( OEM_MODE_T9_MT_FRENCH== pme->m_nCurrInputMode
                       ||OEM_MODE_T9_MT_FRENCH_LOW == pme->m_nCurrInputMode )
                       ||OEM_MODE_T9_MT_FRENCH_UP == pme->m_nCurrInputMode )
                {
                    if (OEM_MODE_T9_MT_FRENCH == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_MT_FRENCH_LOW;
                    }
                    else if (OEM_MODE_T9_MT_FRENCH_LOW == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_MT_FRENCH_UP;
                    }
                    else if(OEM_MODE_T9_MT_FRENCH_UP == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_MT_FRENCH;
                    }

                   OEM_SetInputMode((CTextCtl *)pme);
                   
                   // set flag
                   pme->m_eAutoState = MULTITAP_USER_DENY_AUTOSET;
                   
                } 
#endif

#ifdef FEATURE_T9_RAPID_FRENCH
		if ( ( OEM_MODE_T9_RAPID_FRENCH== pme->m_nCurrInputMode
                       ||OEM_MODE_T9_RAPID_FRENCH_LOW == pme->m_nCurrInputMode )
                       ||OEM_MODE_T9_RAPID_FRENCH_UP == pme->m_nCurrInputMode )
                {
                    if (OEM_MODE_T9_RAPID_FRENCH == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_FRENCH_LOW;
                    }
                    else if (OEM_MODE_T9_RAPID_FRENCH_LOW == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_FRENCH_UP;
                    }
                    else if(OEM_MODE_T9_RAPID_FRENCH_UP == pme->m_nCurrInputMode)
                    {
                        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_FRENCH;
                    }

                   OEM_SetInputMode((CTextCtl *)pme);
                   
                   // set flag
                   pme->m_eAutoState = MULTITAP_USER_DENY_AUTOSET;
                } 
#endif
            }   
#endif

            if ((!pme->m_pSoftKey) && 
                (pme->m_dwProps & TP_STARKEY_SWITCH) &&
                (wParam == AVK_POUND) &&
                (shortkey == FALSE) &&
                ((!pme->m_bShowSyms)&&(!pme->m_bShowFaceSyms)&&(!pme->m_bShowNetSyms)))
            {
                shortkey =TRUE;  
                return TRUE;

            }                       
        // This will help us to handle the '*' '#'key, etc.
            if(pme->m_pText && pme->m_bActive)
            {
                goto NormalKeyEvent;
            }
            break;
            
        case EVT_CHAR:
        case EVT_UPDATECHAR:			
            if ( pme->m_bActive )
            {
                if ( wParam == (uint16)0x08 )
                {
                    // Backspace char will do same as CLR key
                    CTextCtl_HandleEvent(pITextCtl, EVT_KEY, (uint16)AVK_CLR, 0);
                }
                else
                {
                    OEM_TextAddChar(pme->m_pText, (AECHAR)wParam, FALSE);
                    if( eCode == EVT_UPDATECHAR ) // Updating char, keep it selected
                    {
                        OEM_TextSetSel(pme->m_pText, 
                            OEM_TextGetCursorPos(pme->m_pText)-1, OEM_TextGetCursorPos(pme->m_pText));
                    }
                    if (!(pme->m_dwProps & TP_NODRAW))
                    {
                        OEM_TextUpdate(pme->m_pText);
                        IDISPLAY_Update(pme->m_pIDisplay);
                    }
                }
                return TRUE;
            }
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
    AEERect    qrc;

    if (!(pme->m_dwProps & TP_NODRAW)) 
    {
         MSG_FATAL("pme->m_rc.dx=%d---pme->m_rc.dy=%d----pme->m_rc.y=%d",pme->m_rc.dx,pme->m_rc.dy,pme->m_rc.y);
        // clear the old
        if(pme->m_dwProps & TP_GRAPHIC_BG)
        {
            TextCtl_DrawBackGround(pme, &pme->m_rc);
            // IDISPLAY_FillRect(pme->m_pIDisplay, &pme->m_rc, RGB_WHITE);//CLR_SYS_WIN);
            //(void)IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_BACKGROUND, MAKE_RGB(255,255,255));
        }
        else
        {
            IDISPLAY_FillRect(pme->m_pIDisplay, &pme->m_rc, RGB_WHITE);//CLR_SYS_WIN);
            (void)IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_BACKGROUND, MAKE_RGB(255,255,255));          
        }
        
        // draw the title text
        if (pme->m_pTitle) 
        {
        	AEETextInputMode m_Mode = CTextCtl_GetInputMode((ITextCtl *)pme, NULL); 
            qrc = pme->m_rc;
            qrc.dy = GetTitleBarHeight(pme->m_pIDisplay);//pme->m_nFontHeight;

            //Draw title
#ifdef FEATURE_FUNCS_THEME              
            if(pme->m_dwProps & TP_GRAPHIC_BG)
            {
                TextCtl_DrawBackGround(pme, &qrc);
            }
            else
            {
                IDISPLAY_FillRect(pme->m_pIDisplay, &qrc,  pme->m_themeColor);
            }
#else
            #ifndef FEATURE_VERSION_SKY
            {
                TitleBar_Param_type TitleBar;
                
                MEMSET(&TitleBar, 0, sizeof(TitleBar_Param_type));
                TitleBar.prc = &qrc;
                DrawTitleBar(pme->m_pIDisplay, &TitleBar); 
            }
           #endif
            

#endif //FEATURE_FUNCS_THEME
            IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_TEXT, RGB_WHITE);//临时改变文本颜色  
            IDISPLAY_DrawText(pme->m_pIDisplay, 
                AEE_FONT_BOLD,pme->m_pTitle, -1,
                qrc.x, qrc.y,&qrc,IDF_TEXT_TRANSPARENT | IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER);
            IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_TEXT, RGB_BLACK);//恢复文本显示颜色     

            //MSG_FATAL("0pme->m_wResID:::::::::::::::::::%d,m_Mode=%d",pme->m_wResID,m_Mode,0);
            if ((!pme->m_pSoftKey)&&(pme->m_dwProps&TP_STARKEY_SWITCH))
            {
            	
                // 取输入法提示文本
                if(pme->m_wResID != 0)
                {
                    IImage *RightTopImg; 
                    AEEImageInfo m_Imageinfo = {0};
                    RightTopImg = NULL;   
					MSG_FATAL("1pme->m_wResID:::::::::::::::::::%d,m_Mode=%d",pme->m_wResID,m_Mode,0);
                    
					MSG_FATAL("1pme->m_wResID:::::::::::::::::::m_nCurrInputMode=%d,m_Mode=%d",pme->m_nCurrInputMode,m_Mode,0);
                    RightTopImg = ISHELL_LoadResImage(pme->m_pIShell,
                                        AEE_APPSCOMMONRES_IMAGESFILE,
                                        pme->m_wResID);
                    if(RightTopImg != NULL)
                    {
                    	IImage_GetInfo(RightTopImg,&m_Imageinfo);
                        IIMAGE_Draw(RightTopImg, qrc.dx-m_Imageinfo.cx-2, qrc.y + 1);
                        IIMAGE_Release(RightTopImg);
                        RightTopImg = NULL;
                    }                    
                }
                IDISPLAY_Update(pme->m_pIDisplay);
            }

			
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_S1000T)||defined(FEATURE_VERSION_W208S)
            pme->m_isshift = OEM_TextShiftStatus(pme->m_pText);
            pme->m_isAlt   = OEM_TextAltStatus(pme->m_pText);
            pme->m_isCap   = OEM_TextCapStatus(pme->m_pText);
            MSG_FATAL("pme->m_isshift............................",0,0,0);
            if(pme->m_isshift)
            {
            	AECHAR Shiftbuf[20];
                ISHELL_LoadResString(pme->m_pIShell,
                                        AEE_APPSCOMMONRES_LANGFILE,
                                        IDS_SHIFT,
                                        Shiftbuf,
                                        sizeof(Shiftbuf));                
            	MSG_FATAL("qrc.x===%d,qrc.y====%d",qrc.x,qrc.y,0);

            	IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_TEXT, RGB_WHITE);//临时改变文本颜色
    	
            	IDISPLAY_DrawText(pme->m_pIDisplay,
#ifdef FEATURE_VERSION_W208S
				AEE_FONT_NORMAL,
#else
                AEE_FONT_BOLD,
#endif
                Shiftbuf, -1,
                
#if defined (FEATURE_VERSION_S1000T)||defined(FEATURE_VERSION_W208S)
                qrc.dx-80,
#else
                qrc.dx-60,
#endif
                qrc.y,&qrc,IDF_TEXT_TRANSPARENT);
                IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_TEXT, RGB_BLACK);//恢复文本显示颜色
            }
            else if(pme->m_isAlt )
            {
            	AECHAR Altbuf[20];
                ISHELL_LoadResString(pme->m_pIShell,
                                        AEE_APPSCOMMONRES_LANGFILE,
                                        IDS_ALT,
                                        Altbuf,
                                        sizeof(Altbuf));                 
            	MSG_FATAL("qrc.x===%d,qrc.y====%d",qrc.x,qrc.y,0);

            	IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_TEXT, RGB_WHITE);//临时改变文本颜色
           	
            	IDISPLAY_DrawText(pme->m_pIDisplay,
#ifdef FEATURE_VERSION_W208S
				AEE_FONT_NORMAL,
#else
				AEE_FONT_BOLD,
#endif

                Altbuf, -1,
#if defined (FEATURE_VERSION_S1000T)||defined(FEATURE_VERSION_W208S)
                qrc.dx-80,
#else
                qrc.dx-60,
#endif 
                qrc.y,&qrc,IDF_TEXT_TRANSPARENT);
                IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_TEXT, RGB_BLACK);//恢复文本显示颜色
            }
            else if(pme->m_isCap)
            {
            	AECHAR Altbuf[20];
                ISHELL_LoadResString(pme->m_pIShell,
                                        AEE_APPSCOMMONRES_LANGFILE,
                                        IDS_CAP,
                                        Altbuf,
                                        sizeof(Altbuf));                  
            	MSG_FATAL("qrc.x===%d,qrc.y====%d",qrc.x,qrc.y,0);

            	IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_TEXT, RGB_WHITE);//临时改变文本颜色
       	
            	IDISPLAY_DrawText(pme->m_pIDisplay,
#ifdef FEATURE_VERSION_W208S
				AEE_FONT_NORMAL,
#else
				AEE_FONT_BOLD,
#endif

                Altbuf, -1,
#if defined (FEATURE_VERSION_S1000T)||defined(FEATURE_VERSION_W208S)
                qrc.dx-80,
#else
                qrc.dx-60,
#endif
                qrc.y,&qrc,IDF_TEXT_TRANSPARENT);
                IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_TEXT, RGB_BLACK);//恢复文本显示颜色
            }
#endif
        }
        
        if (pme->m_pText) 
        {
            OEM_TextDraw(pme->m_pText);
        }

        if (!(pme->m_dwProps & TP_NOUPDATE))
        {
            IDISPLAY_Update(pme->m_pIDisplay);
        }
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
   //MSG_FATAL("CTextCtl_SetActive............",0,0,0);
   TextCtl_SetMode(pITextCtl, bActive, pme->m_tmLast, TRUE);
   if (pme->m_clsMe == AEECLSID_TEXTCTL && !(pme->m_dwProps & TP_FOCUS_NOSEL) && bActive)
   {
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
   MSG_FATAL("***prc.y=%d,prc.dx=%d,prc.dy=%d",prc->y,prc->dx,prc->dy);
   if (!(pme->m_dwProps & TP_NODRAW)) {
     if(pme->m_dwProps & TP_GRAPHIC_BG)
     {
         TextCtl_DrawBackGround(pme, &rcOld);
         //IDISPLAY_FillRect(pme->m_pIDisplay, &rcOld, CLR_SYS_WIN);
     }
     else
     {
         IDISPLAY_FillRect(pme->m_pIDisplay, &rcOld, CLR_SYS_WIN);
     }
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
    AEEAppInfo ai;
    ISHELL_QueryClass(pme->m_pIShell, ISHELL_ActiveApplet(pme->m_pIShell), &ai);
    MSG_FATAL("ISHELL_QueryClass %d %d",ai.wFlags & AFLAG_HIDDEN,ai.wFlags & AFLAG_STATIC,0);
    if ((ai.wFlags & AFLAG_HIDDEN) || (ai.wFlags & AFLAG_STATIC))
    {
	    //Add By zzg 2010_09_02
	    nProperties |= TP_FOCUS_NOSEL;
	    //Add End
    }
	
    pme->m_dwProps = nProperties;
    if (nProperties & TP_LARGE_FONT) 
    {
        pme->m_nFontHeight = IDISPLAY_GetFontMetrics(pme->m_pIDisplay,
        AEE_FONT_LARGE,
        NULL,
        NULL);
    } 
    else 
    {
        pme->m_nFontHeight = IDISPLAY_GetFontMetrics(pme->m_pIDisplay,
        AEE_FONT_NORMAL,
        NULL,
        NULL);
    }

    if(nProperties & TP_GRAPHIC_BG)
    {
        if(pme->m_pImageBg == NULL)
        {

            if(pme->m_nBgImgResID != 0 && STRLEN(pme->m_strBgImgResFile) != 0)
            {
            	#ifndef FEATURE_LOW_MEM_BIGFONT
                pme->m_pImageBg = ISHELL_LoadResImage(pme->m_pIShell, pme->m_strBgImgResFile, pme->m_nBgImgResID);
				#endif
            }
            else
            {
            	#ifdef FEATURE_VERSION_C316
				if(nProperties & TP_GRAPHIC_BGBLUE)
				{
				    #ifndef FEATURE_VERSION_K232_Y105A
					pme->m_pImageBg = ISHELL_LoadResImage(pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_BACKGROUD_BLUE);
					#else
					OEM_Text_BLUEBackGround(pme->m_pText, TRUE);
                    #endif
                    
				}
				else
				{
					pme->m_pImageBg = ISHELL_LoadResImage(pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
				}
				#else
				#ifndef FEATURE_LOW_MEM_BIGFONT
                pme->m_pImageBg = ISHELL_LoadResImage(pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
				#endif
				#endif
            }
        }
        OEM_TextSetBackGround(pme->m_pText, pme->m_pImageBg);
    }

    if (pme->m_pText)
    {
        OEM_TextSetProperties(pme->m_pText, pme->m_dwProps);
    }
    TextCtl_CalcRects(pme);
    
    // press * key to change input mode
    if ((!pme->m_pSoftKey) && (pme->m_dwProps & TP_STARKEY_SWITCH))
    {    
        (void)OEM_GetConfig(CFGI_INPUTMODE, &pme->m_nCurrInputMode, sizeof(byte));         
        MSG_FATAL("MIAOXIAOMING CFGI_INPUTMODE",0,0,0);
#ifdef FEATURE_CARRIER_BENGAL_CITYCELL
// xiaoguo.xie modify the code on 080802
#if defined FEATURE_CARRIER_VENEZUELA_MOVILNET 
	    // default input mode always is OEM_MODE_T9_RAPID_ENGLISH	
	    pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_ENGLISH;
#elif defined FEATURE_CARRIER_MAROC_WANA  // xiaoguo.xie add the code on 081126
         pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_FRENCH;
#else
	    // default input mode always is OEM_MODE_T9_MT_ENGLISH
	    pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH;
#endif
// xiaoguo.xie add end
            pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;//MULTITAP_MAX_AUTO_STATE;
            
            pme->m_nAutoSetCurPos = 0;
            
            // setup a Timer
            (void) ISHELL_SetTimer((IShell *) pme->m_pIShell,
                            MULTITAP_TIMEOUT,
                            TextCtl_3STYLE_MultitapTimer,
                            pme);
#endif //FEATURE_CARRIER_BENGAL_CITYCELL           
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT
               if(pme->m_dwProps & TP_NOSYMBOL)
            {
                if(pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH || 
                    pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_LOW ||
                    pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_UP)
                {
                    pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH_LOW;
                    pme->m_eAutoState = MULTITAP_USER_DENY_AUTOSET;
                }
                else if( pme->m_nCurrInputMode == OEM_MODE_T9_RAPID_THAI ||
                    pme->m_nCurrInputMode == OEM_MODE_T9_MT_THAI)
                {
                    pme->m_nCurrInputMode = OEM_MODE_T9_MT_THAI;
                }
                else
                 {
                    pme->m_nCurrInputMode = OEM_MODE_NUMBERS;
                }
            }            
#endif
		if(pme->m_dwProps & TP_STARKEY_ID_SWITCH)
		{
			#ifdef FEATURE_USES_ZI
			if(pme->m_nCurrInputMode == OEM_MODE_ZI_MT_ENGLISH || 
                    pme->m_nCurrInputMode == OEM_MODE_ZI_MT_ENGLISH_LOW ||
                    pme->m_nCurrInputMode == OEM_MODE_ZI_MT_ENGLISH_UP ||
                    pme->m_nCurrInputMode == OEM_MODE_NUMBERS)
                {
                #ifdef FEATURE_ZI_CAP_LOWER_ENGLISH   //add by yangdecai
                	pme->m_nCurrInputMode = OEM_MODE_ZI_CAP_LOWER_ENGLISH;
                #endif
                }
                else
                {
                	pme->m_nCurrInputMode = OEM_MODE_NUMBERS;
                }
		    #else
			if(pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH || 
                    pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_LOW ||
                    pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_UP ||
                    pme->m_nCurrInputMode == OEM_MODE_NUMBERS)
                {
                #ifdef FEATURE_T9_CAP_LOWER_ENGLISH   //add by yangdecai
                	pme->m_nCurrInputMode = OEM_MODE_T9_CAP_LOWER_ENGLISH;
                #endif
                }
                else
                {
                	pme->m_nCurrInputMode = OEM_MODE_NUMBERS;
                }
			#endif
                
		}
        OEM_SetInputMode((CTextCtl *)pme);
    }
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

   if(pme->m_pText)
   {
      OEM_TextDelete(pme->m_pText);
      pme->m_pText = NULL;
   }
   

   //Reset the text too. Else, for ex: doing a re-draw soon after a reset will draw the old text
   if(TEXT_PTR(pme))
      MEMSET(TEXT_PTR(pme), 0, TEXT_SIZE(pme));

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
   int len = 0;

   if (psz){
       len = WSTRLEN((AECHAR*)psz);
      if (cch < 0 || cch > len)
         cch = len;
      else if (cch == 0)
         return TRUE;
   }

   if(TEXT_PTR(pme)){
      if (psz && cch > 0)
         WSTRNCOPYN(TEXT_PTR(pme), pme->m_textSize / sizeof(AECHAR), psz, cch);
      else
         MEMSET(TEXT_PTR(pme), 0, TEXT_SIZE(pme));

      if(pme->m_pText)
         OEM_TextSet(pme->m_pText, pme->m_text,pme->m_textSize);
      else   
      {
         // Create the control if it isn't present
         TextCtl_CalcRects(pme);
         
         OEM_TextSet(pme->m_pText, pme->m_text,pme->m_textSize);
      }
      
      CTextCtl_Redraw(pITextCtl);
   }
   return TRUE;
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
      return OEM_TextGet(pme->m_pText);
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
   CTextCtl * pme = (CTextCtl*)pITextCtl;

   pme->m_bSendCmd = (bEnable != 0);
   if (bEnable)
   {
      pme->m_nCmdId = nCmdId;
   }
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
   pITextCtl: pointer to ITextCtl interface to text control obje   nMaxSize: maximum text size

Return Value: none

Comments: none

Side Effects: none

See Also: none
=================================================================*/
static void CTextCtl_SetMaxSize(ITextCtl * pITextCtl, uint16 nMaxSize)
{
   CTextCtl *pme = (CTextCtl*) pITextCtl;
   TextCtl_SetTextSize(pme, nMaxSize);
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

/*=====================================================================

Function: CTextCtl_SetInputMode

Description:
   This method is used to Set Input Mode

Prototype:
   void CTextCtl_SetInputMode(ITextCtl * po, AEETextInputMode m)

Parameter(s):
   pITextCtl: pointer to ITextCtl interface to the text control object
   m: Input mode

Return Value: none

Comments: none

Side Effects: none

See Also: none

=====================================================================*/
static AEETextInputMode CTextCtl_SetInputMode(ITextCtl * po, AEETextInputMode m)
{
    CTextCtl * pme = (CTextCtl *)po;
    AEETextInputMode tmCurr;
    AEETextInputMode tmSetMode = m;
	AEETextInputMode tempMode;

    // Create the control if it isn't present
    if (pme->m_pText == NULL)
    {
        TextCtl_CalcRects(pme);
    }

    if ( pme->m_bShowSyms )
    {
        TextCtl_CloseSymbolDialog(pme, (AECHAR)0);
    }

    // See if they are setting a BREW mode
    if ( AEE_TM_SYMBOLS == tmSetMode )
    {
        if ( TextCtl_IsSymbolsSupported(pme) )
        {
            TextCtl_ShowSymbolDialog(pme);

            if ((pme->m_pSoftKey)||!(pme->m_dwProps & TP_STARKEY_SWITCH))
            {
                ISHELL_HandleEvent(pme->m_pIShell,
                            EVT_CTL_TEXT_MODECHANGED,
                            (uint16)tmSetMode,
                            (uint32)pme);
            }
            return tmSetMode;
        }
        return(OEM_TextGetCurrentMode(pme->m_pText));
    }

    if ( pme->m_bShowNetSyms )
    {
        TextCtl_CloseNetSymbolDialog(pme, NULL);
    }      
        // See if they are setting a BREW mode
    if ( AEE_TM_NET_SYMBOL == tmSetMode )
    {
        if ( TextCtl_IsSymbolsSupported(pme) )
        {
            TextCtl_ShowNetSymbolDialog(pme);

            if ((pme->m_pSoftKey)||!(pme->m_dwProps & TP_STARKEY_SWITCH))
            {
                ISHELL_HandleEvent(pme->m_pIShell,
                            EVT_CTL_TEXT_MODECHANGED,
                            (uint16)tmSetMode,
                            (uint32)pme);
            }
            return tmSetMode;
        }
        return(OEM_TextGetCurrentMode(pme->m_pText));
    }

    if ( pme->m_bShowFaceSyms )
    {
        TextCtl_CloseFaceSymbolDialog(pme, NULL);
    }    
        // See if they are setting a BREW mode
    if ( AEE_TM_FACE_SYMBOL == tmSetMode )
    {
        if ( TextCtl_IsSymbolsSupported(pme) )
        {
            TextCtl_ShowFaceSymbolDialog(pme);

            if ((pme->m_pSoftKey)||!(pme->m_dwProps & TP_STARKEY_SWITCH))
            {
                ISHELL_HandleEvent(pme->m_pIShell,
                            EVT_CTL_TEXT_MODECHANGED,
                            (uint16)tmSetMode,
                            (uint32)pme);
            }
            return tmSetMode;
        }
        return(OEM_TextGetCurrentMode(pme->m_pText));
    }
    
    // If not, try it with the OEM
    tmCurr   = CTextCtl_GetInputMode(po, NULL);
	MSG_FATAL("tmSetMode==%d",tmSetMode,0,0);
	tempMode = OEM_TextGetCurrentMode(pme->m_pText);
	MSG_FATAL("tempMode==%d",tempMode,0,0);
#ifdef FEATURE_VERSION_C316
		if((tmSetMode == TEXT_MODE_T9_MT_HINDI)&&( pme->m_dwProps & TP_NO_HI))
		{
		  tmSetMode = TEXT_MODE_MULTITAP;
		}
#endif

//because
	
	MSG_FATAL("MIAOXIAOMING tmSetMode==%d",tmSetMode,0,0);
    if (tmSetMode != OEM_TextGetCurrentMode(pme->m_pText))
    {
        AEETextInputMode  tmNext;

        switch(tmSetMode)
        {
            case TEXT_MODE_MULTITAP://字母输入模式
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW
	         pme->m_wResID = IDB_MODE_T9_MT_HEBREW_ENGLISH;
#else
#ifdef FEATURE_MT_ENGLISH_NEW
#ifdef FEATURE_MT_ENGLISH_CAPLOW //if FEATURE_MT_ENGLISH_CAPLOW is not defined, TEXT_MODE_MULTITAP==TEXT_MODE_MULTITAP_LOW
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
#endif//FEATURE_MT_ENGLISH_CAPLOW  
#else
#if defined (FEATURE_ALL_KEY_PAD)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else
			pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;
#endif
#else
			#if  defined(FEATURE_VERSION_W516) ||defined(FEATURE_VERSION_VG68) || defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
			#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
			#endif
#endif	         
#endif//FEATURE_MT_ENGLISH_NEW
#endif  // FEATURE_PREPAID_ISRAEL_HEBREW            
                break;
#ifdef FEATURE_USES_ZI
#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH  //add by yangdecai 2010-0909
			case TEXT_MODE_ZI_CAP_LOWER_ENGLISH:
				{
					#ifdef FEATURE_ALL_KEY_PAD
#ifdef FEATURE_WHITE_BG
                    pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
                    pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
					#else
                    MSG_FATAL("IDB_MODE_T9_MT_ENGLISH_UP",0,0,0);
#ifdef FEATURE_WHITE_BG
                    pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else
                    pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;
#endif
					#endif
				}
				break;
#endif

#else
#ifdef FEATURE_T9_CAP_LOWER_ENGLISH  //add by yangdecai 2010-0909
			case TEXT_MODE_T9_CAP_LOWER_ENGLISH:
				{
					#ifdef FEATURE_ALL_KEY_PAD
#ifdef FEATURE_WHITE_BG
                    pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
                    pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
					#else
                    MSG_FATAL("IDB_MODE_T9_MT_ENGLISH_UP",0,0,0);
#ifdef FEATURE_WHITE_BG
                    pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else
                    pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;
#endif
					#endif
				}
				break;
#endif
#endif
#ifdef FEATURE_USES_ZI
			case TEXT_MODE_ZI_RAPID_ENGLISH:
#else
#ifndef FEATURE_VERSION_K232_Y105A
            case TEXT_MODE_T9_RAPID_ENGLISH:
#endif
#endif
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW
	         pme->m_wResID = IDB_MODE_T9_RAPID_HEBREW_ENGLISH;
#else

#if defined (FEATURE_ALL_KEY_PAD)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH_BLACK;
#else
#ifndef FEATURE_VERSION_K232_Y105A
			pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH;
#endif
#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW				
                break;
#ifdef FEATURE_USES_ZI
#ifdef FEATURE_ZI_MT_SPANISH
	      case TEXT_MODE_ZI_MT_SPANISH://字母输入模式
                pme->m_wResID = IDB_MODE_T9_MT_SPANISH;
                break;
#endif

#ifdef FEATURE_ZI_RAPID_SPANISH
	     case TEXT_MODE_ZI_RAPID_SPANISH:
		 pme->m_wResID = IDB_MODE_T9_RAPID_SPANISH;
		 break;
#endif
            case TEXT_MODE_NUMBERS:
#ifdef FEATURE_WHITE_BG
                pme->m_wResID = IDB_MODE_NUMBERS_BLACK;
#else
                pme->m_wResID = IDB_MODE_NUMBERS;
#endif
                break;
				
#ifdef FEATURE_ZI_MT_ARABIC
			case TEXT_MODE_ZI_MT_ARABIC:	
				pme->m_wResID = IDB_MODE_T9_MT_ARABIC;
				break;
#endif //FEATURE_ZI_MT_ARABIC
				
#ifdef FEATURE_ZI_RAPID_ARABIC
			case TEXT_MODE_ZI_RAPID_ARABIC:
				pme->m_wResID = IDB_MODE_T9_RAPID_ARABIC;
				break;
#endif //FEATURE_ZI_RAPID_ARABIC

#else
#ifdef FEATURE_T9_MT_SPANISH
	      case TEXT_MODE_T9_MT_SPANISH://字母输入模式
                pme->m_wResID = IDB_MODE_T9_MT_SPANISH;
                break;
#endif

#ifdef FEATURE_T9_RAPID_SPANISH
	     case TEXT_MODE_T9_RAPID_SPANISH:
		 pme->m_wResID = IDB_MODE_T9_RAPID_SPANISH;
		 break;
#endif
            case TEXT_MODE_NUMBERS:
#ifdef FEATURE_WHITE_BG
                pme->m_wResID = IDB_MODE_NUMBERS_BLACK;
#else                
                pme->m_wResID = IDB_MODE_NUMBERS;
#endif
                break;
				
#ifdef FEATURE_T9_MT_ARABIC
			case TEXT_MODE_T9_MT_ARABIC:	
				pme->m_wResID = IDB_MODE_T9_MT_ARABIC;
				break;
#endif //FEATURE_T9_MT_ARABIC
				
#ifdef FEATURE_T9_RAPID_ARABIC
			case TEXT_MODE_T9_RAPID_ARABIC:
				pme->m_wResID = IDB_MODE_T9_RAPID_ARABIC;
				break;
#endif //FEATURE_T9_RAPID_ARABIC

#ifdef FEATURE_T9_MT_HINDI
			case TEXT_MODE_T9_MT_HINDI:	
				pme->m_wResID = IDB_MODE_T9_MT_HINDI;
				break;
 //FEATURE_T9_MT_ARABIC
#endif
#endif
#ifdef FEATURE_MYANMAR_INPUT_MOD
			case TEXT_MODE_MYANMAR:
				pme->m_wResID = IDI_MODE_MYANMAR;
				break;
#endif
#ifdef FEATURE_MT_MYANMRA
			case TEXT_MODE_MT_MYANMAR:
				pme->m_wResID = IDI_MODE_MYANMAR;
				break;
#endif

#ifdef FEATURE_T9_MT_RUSSIAN
            case TEXT_MODE_MT_RUSSIAN:
                pme->m_wResID = IDI_MODE_MT_RUSSIAN;
                break;
#endif
#ifdef FEATURE_T9_RAPID_RUSSIAN
            case TEXT_MODE_RAPID_RUSSIAN:
                pme->m_wResID = IDI_MODE_RAPID_RUSSIAN;
                break;
#endif
#ifdef FEATURE_T9_PINYIN
			case TEXT_MODE_T9_PINYIN:
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_PINYIN_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_PINYIN;
#endif
				 break;
#endif



            default:
                break;
        }
		//MSG_FATAL("OEM_TextSetEdit000000000",0,0,0);
        OEM_TextSetEdit(pme->m_pText, (boolean)pme->m_bActive, tmSetMode);
        tmNext = CTextCtl_GetInputMode(po, NULL);
        if (tmCurr != tmNext)
        {

            ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TEXT_MODECHANGED, (uint16)tmNext, (uint32)pme);
        }
        TextCtl_CheckEntryMode(pme);
    }
    // 刷新控件
    MSG_FATAL("3pme->m_wResID:::::::::::::::::::=%d,tmCurr=%d",pme->m_wResID,tmCurr,0);
    (void)CTextCtl_Redraw((ITextCtl *)pme);
    tmCurr = OEM_TextGetCurrentMode(pme->m_pText);
    return tmCurr;

}

/*=====================================================================

Function: CTextCtl_GetInputMode

Description:
   This method is used to Get Input Mode

Prototype:
   void CTextCtl_GetInputMode(ITextCtl * po, AEETextInputModeInfo * pmInfo)

Parameter(s):
   po: pointer to ITextCtl interface to the text control object
   pmInfo: Input mode

Return Value: none

Comments: none

Side Effects: none

See Also: none

=====================================================================*/
static AEETextInputMode CTextCtl_GetInputMode(ITextCtl *po, AEETextInputModeInfo * pmInfo)
{
   CTextCtl * pme = (CTextCtl *)po;
   AEETextInputMode retm;

   // Check if we are in a BREW mode, this case is symbols
   
   if(pme->m_bShowSyms){
      if( pmInfo ){
         MEMCPY(pmInfo, &sBREWTextModes[0], sizeof(AEETextInputModeInfo));
      }
      return sBREWTextModes[0].tmMode;
   }
   
   if(pme->m_bShowNetSyms){
      if( pmInfo ){
         MEMCPY(pmInfo, &sBREWTextModes[1], sizeof(AEETextInputModeInfo));
      }
      return sBREWTextModes[1].tmMode;
   }        
   
   if(pme->m_bShowFaceSyms){
      if( pmInfo ){
         MEMCPY(pmInfo, &sBREWTextModes[2], sizeof(AEETextInputModeInfo));
      }
      return sBREWTextModes[2].tmMode;
   }  

   if( !pmInfo )
   {
      retm = OEM_TextGetCurrentMode(pme->m_pText);
      return retm;

   }

   pmInfo->tmMode = OEM_TextGetCurrentMode(pme->m_pText);
   OEM_TextGetModeString(pme->m_pText, pmInfo->modeString, sizeof(pmInfo->modeString));

   return pmInfo->tmMode;

}

static boolean CTextCtl_GetInfo(ITextCtl *po, uint32 type, uint32 *pvoid)
{
    CTextCtl * pme = (CTextCtl *)po;
   
    if(!pvoid)
        return FALSE;
    
    switch(type)
    {
        case AEE_TMINFO_ICONID:
            *pvoid = (uint32)pme->m_wResID;
            break;
        case AEE_TMINFO_NOCCINPUT:
        {
            byte Mode = pme->m_nCurrInputMode;
            boolean b_IsCCIM;

            b_IsCCIM =            
#ifdef  FEATURE_T9_PINYIN
                                            Mode == OEM_MODE_T9_PINYIN ||
#endif
#ifdef FEATURE_T9_STROKE
                                            Mode== OEM_MODE_T9_STROKE ||
#endif
#ifdef FEATURE_T9_ZHUYIN
                                            Mode ==OEM_MODE_T9_ZHUYIN ||
#endif
                                            FALSE;

            if( !b_IsCCIM)
            {
                return FALSE;
            }
            if(FOCUS_TEXT == OEM_TextGetFocusState(pme->m_pText))
            {
                 *pvoid = FALSE;
            }
            else
            {
                 *pvoid = TRUE;
            }
        }
        break;

        case AEE_TMINFO_KEYBUFLEN:
        {
            *pvoid = OEM_TextGetKeyBufLen(pme->m_pText);
        }
        break;
        default:
            return FALSE;
    }
    
    return TRUE;
}

/*=====================================================================

Function: CTextCtl_EnumModeInit

Description:
   This method is used to Init EnumMode

Prototype:
   static void CTextCtl_EnumModeInit(ITextCtl *po)

Parameter(s):
   po: pointer to ITextCtl interface to the text control object

Return Value: none

Comments: none

Side Effects: none

See Also: none

=====================================================================*/
static void CTextCtl_EnumModeInit(ITextCtl *po)
{
   CTextCtl * pme = (CTextCtl *)po;
   OEM_TextEnumModesInit();
   pme->m_nModeEnum = 0;
}

/*=====================================================================

Function: CTextCtl_EnumNextMode

Description:
   This method is used to Enum Next Mode

Prototype:
   static void CTextCtl_EnumModeInit(ITextCtl *po)

Parameter(s):
   po: pointer to ITextCtl interface to the text control object

Return Value: none

Comments: none

Side Effects: none

See Also: none

=====================================================================*/
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
      
      if( sBREWTextModes[pme->m_nModeEnum].tmMode == AEE_TM_NET_SYMBOL )
         pme->m_nModeEnum++;

      if( sBREWTextModes[pme->m_nModeEnum].tmMode == AEE_TM_FACE_SYMBOL )
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
         pText = (AECHAR *)MALLOC(MAX_INPUTTEXT_SIZE);
         if(!pText)
            return(FALSE);
         ISHELL_LoadResString(pme->m_pIShell, pRes, wID, pText, MAX_INPUTTEXT_SIZE);
         bFree = TRUE;
      }

      nLen = WSTRLEN(pText);
      if(nLen > WSTRLEN(pme->m_text))
         TextCtl_SetTextSize(pme, (uint16)nLen);

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
   //MSG_FATAL("pme->m_rc.X=%d,pme->m_rc.y=%d",pme->m_rc.x,pme->m_rc.y,0);
   //MSG_FATAL("pme->m_rc.dX=%d,pme->m_rc.dy=%d",pme->m_rc.dx,pme->m_rc.dy,0);
   rc = pme->m_rc;

   if (pme->m_pTitle)
      rc.y += TITLEBAR_HEIGHT;//LINEHEIGHT;

   if (bMulti) {      // multi-line
      // adjust for title area
      if (!bFixRect || pme->m_pTitle)
         rc.dy -= TITLEBAR_HEIGHT;//LINEHEIGHT;
      // round rc.dy to nearest line increment
      if (bIntegral) {
         int lines = (rc.dy + (LINEHEIGHT/2)) / LINEHEIGHT;
         rc.dy = (LINEHEIGHT * lines);
      }
   }
   else {                                    // single-line
      if (!bFixOEM)
      {
         rc.dy = (LINEHEIGHT);
      }
   }

   // check within min/max
   if (bFixRect) {
       int dyMax = pme->m_dyTextMax;
       boolean bFix = rc.dy >= dyMax;
       if (pme->m_pTitle)
          dyMax -= TITLEBAR_HEIGHT;//LINEHEIGHT; 
       if (bFixOEM)
       {
          rc.dy = CONSTRAIN(rc.dy, (LINEHEIGHT), dyMax);
       }
       else
       {
          rc.dy = CONSTRAIN(rc.dy, (LINEHEIGHT), dyMax);
       }
       if (bFix && bIntegral && bMulti)
          rc.dy = ((rc.dy / LINEHEIGHT) * LINEHEIGHT);
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
         pme->m_rcGet.y -= TITLEBAR_HEIGHT;//LINEHEIGHT; 
         pme->m_rcGet.dy += TITLEBAR_HEIGHT; //LINEHEIGHT; 
      }
   }

   if (bFixOEM && pme->m_pText) {

      OEM_TextSetRect(pme->m_pText, &rc);

   } else {
      AEETextInputMode mode = AEE_TM_CURRENT;

      if (pme->m_pText) {
         mode = OEM_TextGetCurrentMode(pme->m_pText);
         OEM_TextDelete(pme->m_pText);
      }

      //pme->m_pText = OEM_TextCreate(pme->m_pIShell, pme->m_pIDisplay, &rc);
      MSG_FATAL("rc.X=%d,rc.y=%d",rc.x,rc.y,0);
      MSG_FATAL("rc.dX=%d,rc.dy=%d",rc.dx,rc.dy,0);
      pme->m_pText = OEM_TextCreate(pme->m_pIShell, pme->m_pIDisplay, &rc, pme->m_clsMe);

      OEM_TextSetProperties(pme->m_pText, pme->m_dwProps);
      OEM_TextSet(pme->m_pText, pme->m_text, pme->m_textSize);
      OEM_TextSetMaxChars(pme->m_pText, pme->m_nMaxChars);
      OEM_TextSetBackGround(pme->m_pText, pme->m_pImageBg);
	  //MSG_FATAL("OEM_TextSetEdit00000000222",0,0,0);
      if (mode != AEE_TM_CURRENT) {
         OEM_TextSetEdit(pme->m_pText, (boolean)pme->m_bActive, mode);
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

    (void) ISHELL_CancelTimer((IShell *) pme->m_pIShell,
                TextCtl_3STYLE_MultitapTimer,
                pme);

   CTextCtl_Reset((ITextCtl *) pme);
   if(TEXT_PTR(pme))
      FREE(TEXT_PTR(pme));
   
    ISHELL_Release(pme->m_pIShell);   
    if(pme->m_pIDisplay != NULL)
    {
       IDISPLAY_Release(pme->m_pIDisplay);
       pme->m_pIDisplay = NULL;
    }
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

   AECHAR         szBuff[128]; //szBuff[64];
   AECHAR         sz[2],szText[15],szbottomText[15];
   int            nPage, x, y,cyAdd, cxAdd,nFits, nRow, nCol;
   int16          nWidth;
   AEEDeviceInfo  dm;
   IDisplay *     pd = pme->m_pIDisplay;
   AEERect        rc;
   AECHAR   *      pszSym;
   int            nPages, nSymsTotal,i;
//   AEERect        titlerect;  
//   AEERect        bottomrect; 
   TitleBar_Param_type TitleBar;
   BottomBar_Param_type BottomBar;
   RGBVAL    nOldFontColor;
   MSG_FATAL("TextCtl_ShowSymbolPage Start",0,0,0);
   ISHELL_GetDeviceInfo(pme->m_pIShell, &dm); 

    szBuff[0] = (AECHAR)0x2195; 
    szBuff[1] = (AECHAR)'\0';

    ISHELL_LoadResString(pme->m_pIShell,
                                AEECONTROLS_RES_FILE,
                                AEE_IDS_MODE_SYMBOLS,
                                szText,
                                sizeof(szText));
    ISHELL_LoadResString(pme->m_pIShell,
                            AEE_APPSCOMMONRES_LANGFILE,
                            IDS_BACK,
                            szbottomText,
                            sizeof(szbottomText));
    TextCtl_ClearScreen(pme);
    {
        IAnnunciator *pIAnn;
        if (AEE_SUCCESS == ISHELL_CreateInstance(pme->m_pIShell,AEECLSID_ANNUNCIATOR,(void **)&pIAnn))
        {
            IANNUNCIATOR_SetFieldIsActiveEx(pIAnn,FALSE);
            IANNUNCIATOR_SetFieldText(pIAnn,szText);
            IANNUNCIATOR_Release(pIAnn);
            pIAnn = NULL;            
        }  	        
    }   
#if 0    
    //Draw title  //这里将统一的titlebar画上去
    {
        MEMSET(&TitleBar, 0, sizeof(TitleBar_Param_type));
        TitleBar.pwszTitle = szText;
        TitleBar.dwAlignFlags =IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
        DrawTitleBar(pme->m_pIDisplay, &TitleBar);
    }
#endif
#if 0 //去掉原来填充颜色的titlebar
    SETAEERECT(&titlerect, 0, 0, dm.cxScreen, pme->m_nFontHeight);   
#ifdef FEATURE_FUNCS_THEME     
    IDISPLAY_FillRect(pme->m_pIDisplay, &titlerect,  pme->m_themeColor);
#endif //FEATURE_FUNCS_THEME 
    //IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szText, -1, 0, 0, &titlerect,IDF_TEXT_TRANSPARENT);
    IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szText, -1, 0, 0, &titlerect,IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
#endif    

    //Draw bottom bar
    {
        MEMSET(&BottomBar, 0, sizeof(BottomBar_Param_type));
        BottomBar.eBBarType = BTBAR_BACK;
        DrawBottomBar(pme->m_pIDisplay, &BottomBar);
    }
#if 0 //去掉原来的bottombar   
    SETAEERECT(&bottomrect, 0, dm.cyScreen-pme->m_nFontHeight-1, dm.cxScreen, pme->m_nFontHeight+1);   
#ifdef FEATURE_FUNCS_THEME     
    IDISPLAY_FillRect(pme->m_pIDisplay, &bottomrect,  pme->m_themeColor);
#endif //FEATURE_FUNCS_THEME  
    IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szBuff, -1, 0, dm.cyScreen-pme->m_nFontHeight-1, &bottomrect,IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
    IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szbottomText, -1, 0, dm.cyScreen-pme->m_nFontHeight-1, &bottomrect,IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
#endif
   nPage = pme->m_nSymbolPage;
   
   //Query Symbols From OEM
#ifndef FEATURE_CARRIER_CHINA_TELCOM  // modify the code for the Unicom Spec on 081206
   OEM_TextQuerySymbols(pme,szBuff, sizeof(szBuff)); 
#else
   TextCtl_QuerySymbols(pme, szBuff, sizeof(szBuff));
#endif
   
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

   // Determine the largest character size in the array...
   rc.dx = 0;
   sz[1] = 0;
   for(i = 0; i < 9 ; i++){
      *sz = (AECHAR)('1' + i); // (HS_CHR_KEY_1 + i);
      nWidth = IDISPLAY_MeasureTextEx(pd, AEE_FONT_NORMAL, sz, 1, -1, &nFits);
      if(nWidth > rc.dx)
         rc.dx = nWidth;
   }
   rc.dx += 1;

   //y = TITLEBAR_HEIGHT*3/2;//pme->m_nFontHeight + 1;
  // cyAdd = (dm.cyScreen - TITLEBAR_HEIGHT - BOTTOMBAR_HEIGHT) / 3;
   cyAdd = (dm.cyScreen - BOTTOMBAR_HEIGHT) / 3;
   y = (cyAdd - pme->m_nFontHeight)/2;
   //y = TITLEBAR_HEIGHT + (cyAdd - pme->m_nFontHeight)/2;
   cxAdd = dm.cxScreen / 3;

   rc.dy = pme->m_nFontHeight;//y;

   pszSym = szBuff + (nPage * 9);
   #ifdef FEATURE_VERSION_K232_Y105A
      if(pme->m_dwProps & TP_GRAPHIC_BGBLUE)
      {
           if(pme->m_dwProps & TP_GRAPHIC_BG)
           {
                nOldFontColor = RGB_BLACK;
           }
           else
           {
                nOldFontColor = RGB_WHITE;
           }
      }
      else
      {
           if(pme->m_dwProps & TP_GRAPHIC_BG)
           {
                nOldFontColor = TEXT_GRAPHIC_FONT_COLOR;
           }
           else
           {
                nOldFontColor = TEXT_FONT_COLOR;
           }
      }
   #else
       if(pme->m_dwProps & TP_GRAPHIC_BG)
       {
            nOldFontColor = TEXT_GRAPHIC_FONT_COLOR;
       }
       else
       {
            nOldFontColor = TEXT_FONT_COLOR;
       }
   #endif
   nOldFontColor = IDISPLAY_SetColor(pd, CLR_USER_TEXT, nOldFontColor);
   for(nRow = i = 0; nRow < 3; nRow++, y += cyAdd){
      for(x = dm.cxScreen/10, nCol = 0; nCol < 3; nCol++, x += cxAdd, i++, pszSym++)
      {

   // See if we hit the end...
         if(!*pszSym){
            nRow = 3;
            break;
         }

         rc.x = x;
         rc.y = y;
         *sz = (AECHAR)('1' + i);   // (HS_CHR_KEY_1 + i);
         IDISPLAY_DrawText(pd, AEE_FONT_NORMAL, sz, 1, rc.x + 1,y, &rc,IDF_TEXT_TRANSPARENT/*IDF_RECT_FILL*/);
         rc.x += rc.dx + 2;
         
         if (*pszSym == '\n')
         {
           // AECHAR szbuf[2];
           // szbuf[0] = (AECHAR)0x3130;
           // szbuf[1] = (AECHAR)'\0';
           // AECHAR szbuf[] = {'E', 'n', 't', 'e', 'r', '\0'};
           // IDISPLAY_DrawText(pd, AEE_FONT_NORMAL, szbuf, -1, rc.x+10, y, NULL,IDF_TEXT_TRANSPARENT/*0*/);
              IImage *enterImg; 
              enterImg = NULL;   
#if defined (FEATURE_DISP_128X160) || defined (FEATURE_DISP_240X320)
            if(pme->m_dwProps & TP_GRAPHIC_BG)
           {
                //enterImg = ISHELL_LoadResImage(pme->m_pIShell,
                //                  AEE_APPSCOMMONRES_IMAGESFILE,
                //                  IDB_MMS_ENTER);
           }
           else
           {
               // enterImg = ISHELL_LoadResImage(pme->m_pIShell,
               //                   AEE_APPSCOMMONRES_IMAGESFILE,
               //                   IDI_ENTER_BLACK);
           }
            
#else
             enterImg = ISHELL_LoadResImage(pme->m_pIShell,
                                  AEE_APPSCOMMONRES_IMAGESFILE,
                                  IDB_MMS_ENTER); 
#endif              
              
              if(enterImg != NULL)
              {
                  IIMAGE_Draw(enterImg, rc.x+10, y + 1);
                  IIMAGE_Release(enterImg);
                  enterImg = NULL;
              } 
         }
         else if(*pszSym == ' ')
         {
            //AECHAR szbuf[] = {'S', 'P','\0'};
            //IDISPLAY_DrawText(pd, AEE_FONT_NORMAL, szbuf, -1, rc.x+10, y, NULL,IDF_TEXT_TRANSPARENT/*0*/);
             IImage *spaceImg; 
              spaceImg = NULL;   
#if defined (FEATURE_DISP_128X160) || defined (FEATURE_DISP_240X320)
            if(pme->m_dwProps & TP_GRAPHIC_BG)
           {
                spaceImg = ISHELL_LoadResImage(pme->m_pIShell,
                                  AEE_APPSCOMMONRES_IMAGESFILE,
                                  IDB_MMS_SPACE);
           }
           else
           {
               // spaceImg = ISHELL_LoadResImage(pme->m_pIShell,
                //                  AEE_APPSCOMMONRES_IMAGESFILE,
               //                   IDI_SPACE_BLACK);
           }
            
#else
             spaceImg = ISHELL_LoadResImage(pme->m_pIShell,
                                  AEE_APPSCOMMONRES_IMAGESFILE,
                                  IDB_MMS_SPACE);   
#endif
              
              if(spaceImg != NULL)
              {
                  IIMAGE_Draw(spaceImg, rc.x+10, y + 1);
                  IIMAGE_Release(spaceImg);
                  spaceImg = NULL;
              } 
         }
         else
         {
            IDISPLAY_DrawText(pd, AEE_FONT_NORMAL, pszSym, 1, rc.x+10, y, NULL,IDF_TEXT_TRANSPARENT/*0*/);
         }
      }  
   }     
   (void)IDISPLAY_SetColor(pd, CLR_USER_TEXT, nOldFontColor);
   if (!(pme->m_dwProps & TP_NODRAW))
      IDISPLAY_Update(pd);
   MSG_FATAL("TextCtl_ShowSymbolPage End",0,0,0);
}

static void TextCtl_ShowFaceSymbolPage(CTextCtl * pme, int nDir)
{
   AECHAR         szBuff[2],sz[2],szText[15],szbottomText[15],szTitle[15];
   int            nPage, x, y;
   AEEDeviceInfo  dm;
   IDisplay *     pd = pme->m_pIDisplay;
   int            nPages;
//   AEERect        titlerect;  
//   AEERect        bottomrect;
   MSG_FATAL("TextCtl_ShowFaceSymbolPage Start",0,0,0);
   ISHELL_GetDeviceInfo(pme->m_pIShell, &dm);

    szBuff[0] = (AECHAR)0x2195; 
    szBuff[1] = (AECHAR)'\0';

    ISHELL_LoadResString(pme->m_pIShell,
                                AEECONTROLS_RES_FILE,
                                AEE_IDS_MODE_SYMBOLS,
                                szTitle,
                                sizeof(szTitle));
    ISHELL_LoadResString(pme->m_pIShell,
                            AEE_APPSCOMMONRES_LANGFILE,
                            IDS_BACK,
                            szbottomText,
                            sizeof(szbottomText));
    TextCtl_ClearScreen(pme);
    {
        IAnnunciator *pIAnn;
        if (AEE_SUCCESS == ISHELL_CreateInstance(pme->m_pIShell,AEECLSID_ANNUNCIATOR,(void **)&pIAnn))
        {
            IANNUNCIATOR_SetFieldIsActiveEx(pIAnn,FALSE);
            IANNUNCIATOR_SetFieldText(pIAnn,szText);
            IANNUNCIATOR_Release(pIAnn);
            pIAnn = NULL;            
        }  	        
    }    
    
#if 0
    {
        TitleBar_Param_type TitleBar;
        
        MEMSET(&TitleBar, 0, sizeof(TitleBar_Param_type));
        TitleBar.pwszTitle = szTitle;
        TitleBar.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
        DrawTitleBar(pme->m_pIDisplay, &TitleBar);
    }
#endif
#if 0  //去掉原来填充颜色的画titlebar的方式  
    SETAEERECT(&titlerect, 0, 0, dm.cxScreen, pme->m_nFontHeight);   
#ifdef FEATURE_FUNCS_THEME     
    IDISPLAY_FillRect(pme->m_pIDisplay, &titlerect,  pme->m_themeColor);
#endif //FEATURE_FUNCS_THEME
    //IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szText, -1, 0, 0, &titlerect,IDF_TEXT_TRANSPARENT);
    IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szText, -1, 0, 0, &titlerect,IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
#endif    

    //Draw bottom bar 使用统一的底条
    {
        BottomBar_Param_type BottomBar;
        
        MEMSET(&BottomBar, 0, sizeof(BottomBar_Param_type));
        BottomBar.eBBarType = BTBAR_BACK;
        DrawBottomBar(pme->m_pIDisplay, &BottomBar);
    }
#if 0 //去掉原来的方式
    SETAEERECT(&bottomrect, 0, dm.cyScreen-pme->m_nFontHeight-1, dm.cxScreen, pme->m_nFontHeight+1);   
#ifdef FEATURE_FUNCS_THEME     
    IDISPLAY_FillRect(pme->m_pIDisplay, &bottomrect,  pme->m_themeColor);
#endif //FEATURE_FUNCS_THEME  
//#if MAX_FACE_SYMBOL > 6    // add the statement on 080805
    IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szBuff, -1, 0, dm.cyScreen-pme->m_nFontHeight-1, &bottomrect,IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
//#endif
    IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szbottomText, -1, 0, dm.cyScreen-pme->m_nFontHeight-1, &bottomrect,IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
#endif   
   //pme->m_SymOnePageNum = (dm.cyScreen-TITLEBAR_HEIGHT - BOTTOMBAR_HEIGHT)/pme->m_nFontHeight -2;
   pme->m_SymOnePageNum = ((dm.cyScreen- BOTTOMBAR_HEIGHT)/pme->m_nFontHeight -2); 
   
   if(pme->m_SymOnePageNum > 9)
   {
        pme->m_SymOnePageNum = 9;
   }
   
   nPage = pme->m_nSymbolPage;

   nPages = (MAX_FACE_SYMBOL / pme->m_SymOnePageNum);
   
   if(MAX_FACE_SYMBOL % pme->m_SymOnePageNum)
   {
   		nPages++;
   }

   pme->m_SymPageNum = nPages;
   
   nPage += nDir;
   
   if (nPage < 0)
   {
   		nPage = nPages - 1;
   }
   else
   {
      if (nPage > nPages - 1)
      {
      	nPage = 0;
      }
   }

   pme->m_nSymbolPage = nPage;

   {
        int   count;
        RGBVAL nOldFontColor;
        x = 0;

		/*
		if (pme->m_nSymbolPage%2 == 1)
        {
        	//y = TITLEBAR_HEIGHT + 3*(pme->m_nFontHeight)/2;
            y = 3*(pme->m_nFontHeight)/2;
		}
		else
		{
			//y = TITLEBAR_HEIGHT - (pme->m_nFontHeight)/2;
			y = (pme->m_nFontHeight)/2;
		}
		*/
		y = (pme->m_nFontHeight)/2;		
	
       if (pme->m_dwProps & TP_GRAPHIC_BG)
       {
            nOldFontColor = TEXT_GRAPHIC_FONT_COLOR;
       }
       else
       {
            nOldFontColor = TEXT_FONT_COLOR;
       }

       nOldFontColor = IDISPLAY_SetColor(pd, CLR_USER_TEXT, nOldFontColor);
        for (count = pme->m_SymOnePageNum*pme->m_nSymbolPage; count < pme->m_SymOnePageNum*(pme->m_nSymbolPage+1); count++)
        {
            if(count>=MAX_FACE_SYMBOL)
            {
                break;
            }
#if 0
            x = 2;
            y += pme->m_nFontHeight;
#else
            x = dm.cxScreen/10;

            if(count%2 == 1)
            {
                x += dm.cxScreen/2;				
            }
            else if ((count != 0) && (count != 10))
            {
                y += (2*pme->m_nFontHeight);				
            }
#endif            

            *sz = (AECHAR)('1' + (count-pme->m_SymOnePageNum*pme->m_nSymbolPage));  
            IDISPLAY_DrawText(pd, 
                    AEE_FONT_NORMAL, 
                    sz, 
                    1, 
                    x, y, 
                    NULL,
                    IDF_TEXT_TRANSPARENT);
            x += 10;

            WSTRCPY ( szText,  sszFaceSymbolList[count] );
            
            IDISPLAY_DrawText(pd, 
                    AEE_FONT_NORMAL, 
                    szText, 
                    -1,
                    x, y, 
                    NULL,
                    IDF_TEXT_TRANSPARENT);
        }
        (void)IDISPLAY_SetColor(pd, CLR_USER_TEXT, nOldFontColor);
   }

   if (!(pme->m_dwProps & TP_NODRAW))
      IDISPLAY_Update(pd);
    MSG_FATAL("TextCtl_ShowFaceSymbolPage End",0,0,0);
}

static void TextCtl_ShowNetSymbolPage(CTextCtl * pme, int nDir)
{
   AECHAR         szBuff[2],sz[2],szText[15],szbottomText[15];
   int            nPage, x, y;
   AEEDeviceInfo  dm;
   IDisplay *     pd = pme->m_pIDisplay;
   int            nPages;
//   AEERect        titlerect;  
//   AEERect        bottomrect; 
   TitleBar_Param_type TitleBar;
   BottomBar_Param_type BottomBar;
   MSG_FATAL("TextCtl_ShowNetSymbolPage Start",0,0,0);
   ISHELL_GetDeviceInfo(pme->m_pIShell, &dm); 
   
   szBuff[0] = (AECHAR)0x2195; 
   szBuff[1] = (AECHAR)'\0';

   ISHELL_LoadResString(pme->m_pIShell,
                                AEECONTROLS_RES_FILE,
                                AEE_IDS_MODE_SYMBOLS,
                                szText,
                                sizeof(szText));
  ISHELL_LoadResString(pme->m_pIShell,
                            AEE_APPSCOMMONRES_LANGFILE,
                            IDS_BACK,
                            szbottomText,
                            sizeof(szbottomText));
   TextCtl_ClearScreen(pme);
   #ifndef FEATURE_VERSION_SKY
   {
       TitleBar.pwszTitle = szText;
       TitleBar.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_BOTTOM;
       DrawTitleBar(pme->m_pIDisplay, &TitleBar); 
   }
   #endif
#if 0   
   SETAEERECT(&titlerect, 0, 0, dm.cxScreen, pme->m_nFontHeight);   
#ifdef FEATURE_FUNCS_THEME     
   IDISPLAY_FillRect(pme->m_pIDisplay, &titlerect,  pme->m_themeColor);
#endif //FEATURE_FUNCS_THEME
   //IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szText, -1, 0, 0, &titlerect,IDF_TEXT_TRANSPARENT);
   IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szText, -1, 0, 0, &titlerect,IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
#endif   

   {
       BottomBar.eBBarType = BTBAR_BACK;
       DrawBottomBar(pme->m_pIDisplay, &BottomBar);
   }
#if 0

   SETAEERECT(&bottomrect, 0, dm.cyScreen-pme->m_nFontHeight-1, dm.cxScreen, pme->m_nFontHeight+1);   
#ifdef FEATURE_FUNCS_THEME     
   IDISPLAY_FillRect(pme->m_pIDisplay, &bottomrect,  pme->m_themeColor);
#endif //FEATURE_FUNCS_THEME  
#if MAX_NET_SYMBOL > 6    // add the statement on 080805
   IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szBuff, -1, 0, dm.cyScreen-pme->m_nFontHeight-1, &bottomrect,IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
#endif
   IDISPLAY_DrawText(pd, AEE_FONT_BOLD, szbottomText, -1, 0, dm.cyScreen-pme->m_nFontHeight-1, &bottomrect,IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
#endif

   pme->m_SymOnePageNum = (dm.cyScreen-pme->m_nFontHeight)/pme->m_nFontHeight -1; 
   
   if(pme->m_SymOnePageNum > 9)
   {
        pme->m_SymOnePageNum = 9;
   }
   
   nPage = pme->m_nSymbolPage;

   nPages = (MAX_NET_SYMBOL / pme->m_SymOnePageNum);
   if(MAX_NET_SYMBOL % pme->m_SymOnePageNum)
      nPages++;

   pme->m_SymPageNum = nPages;
   
   nPage += nDir;
   if(nPage < 0)
      nPage = nPages - 1;
   else{
      if(nPage > nPages - 1)
         nPage = 0;
   }

   pme->m_nSymbolPage = nPage;

   {
        int   count;
        RGBVAL nOldFontColor;
        x = 0;
        y = 0;
 
       if(pme->m_dwProps & TP_GRAPHIC_BG)
       {
            nOldFontColor = TEXT_GRAPHIC_FONT_COLOR;
       }
       else
       {
            nOldFontColor = TEXT_FONT_COLOR;
       }

       nOldFontColor = IDISPLAY_SetColor(pd, CLR_USER_TEXT, nOldFontColor);
        for (count = pme->m_SymOnePageNum*pme->m_nSymbolPage; count < pme->m_SymOnePageNum*(pme->m_nSymbolPage+1); count++)
        {
            if(count>=MAX_NET_SYMBOL)
            {
                break;
            }
            x = 2;
            y += pme->m_nFontHeight;
            
            *sz = (AECHAR)('1' + (count-pme->m_SymOnePageNum*pme->m_nSymbolPage));  
            IDISPLAY_DrawText(pd, 
                    AEE_FONT_NORMAL, 
                    sz, 
                    1, 
                    x, y, 
                    NULL,
                    IDF_TEXT_TRANSPARENT);
            x += 10;

            WSTRCPY ( szText,  sszNetSymbolList[count] );
            
            IDISPLAY_DrawText(pd, 
                    AEE_FONT_NORMAL, 
                    szText, 
                    -1,
                    x, y, 
                    NULL,
                    IDF_TEXT_TRANSPARENT);
        }
        (void)IDISPLAY_SetColor(pd, CLR_USER_TEXT, nOldFontColor);
   }

   if (!(pme->m_dwProps & TP_NODRAW))
      IDISPLAY_Update(pd);
    MSG_FATAL("TextCtl_ShowNetSymbolPage End",0,0,0);
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
   AECHAR          szBuff[128];//szBuff[64];

   *szBuff = (AECHAR)0; // in case OEM is misbehaved.
#ifndef FEATURE_CARRIER_CHINA_TELCOM
   if( OEM_TextQuerySymbols(pme,szBuff, sizeof(szBuff)) && *szBuff ) 
#else   
   if( TextCtl_QuerySymbols(pme, szBuff, sizeof(szBuff)) && *szBuff )
#endif
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
   AECHAR  szSyms[128]; //szSyms[64];

#ifndef FEATURE_CARRIER_CHINA_TELCOM
   if(OEM_TextQuerySymbols(pme,szSyms, sizeof(szSyms)) && *szSyms) 
#else
   if(TextCtl_QuerySymbols(pme, szSyms, sizeof(szSyms)) && *szSyms)
#endif
   {
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
   AEEDeviceInfo dm;
   AEERect       rc;

   ISHELL_GetDeviceInfo(pme->m_pIShell, &dm);
   SETAEERECT(&rc, 0, 0, dm.cxScreen, dm.cyScreen);
   if(pme->m_dwProps & TP_GRAPHIC_BG)
   {
       TextCtl_DrawBackGround(pme, &rc);
   }
   else
   {
//#ifdef FEATURE_WHITE_BG
       //IDISPLAY_FillRect(pme->m_pIDisplay,&rc, RGB_BLACK);     
//#else
       IDISPLAY_FillRect(pme->m_pIDisplay,&rc, RGB_WHITE);
//#endif
   }
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
	AEECLSID  pCLSID;
    boolean  bDoItDoIt   = FALSE;

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
	SetArrowFlagonIM(FALSE);     
	pme->m_bShowSyms = FALSE;
	if(ch)
	    OEM_TextAddChar(pme->m_pText,ch,FALSE);

	if(pme->m_pSoftKey)
	   IMENUCTL_Redraw(pme->m_pSoftKey);

	CTextCtl_Redraw((ITextCtl *)pme);

	pCLSID = ISHELL_ActiveApplet(pme->m_pIShell);

	MSG_FATAL("***zzg ISHELL_ActiveApplet=%x***", pCLSID, 0, 0);
	
	(void)ISHELL_PostEvent(pme->m_pIShell,
                           pCLSID,
                           EVT_USER_REDRAW,
                           0,
                           0);    

   ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TEXT_MODECHANGED, (uint16)CTextCtl_GetInputMode((ITextCtl *)pme, NULL), (uint32)pme);
}

/*===========================================================================
Function: TextCtl_CloseNetSymbolDialog()

Description: This function is closed when the user makes a selection from the
list of Symbols dialog box and the dialog box is to be closed. This function
passes the user selected symbol to the OEM and requests to add this symbol at
current cursor position within the text control.

Prototype:
   static void     TextCtl_CloseNetSymbolDialog (CTextCtl * pme, AECHAR* string)

Parameter(s):
   pme: Pointer to text control whose symbols dialog is closing
   string:  NetSymbol selected by the user before closing dialog

Return Value: None

Comments: none

Side Effects:

See Also: none

===========================================================================*/
static void TextCtl_CloseNetSymbolDialog(CTextCtl * pme, AECHAR* string)
{
   TextCtl_ClearScreen(pme); 

   CTextCtl_SetActive((ITextCtl *)pme, TRUE);

   pme->m_bShowNetSyms = FALSE;
   if(string)
      OEM_TextAddString(pme->m_pText,string);

   if(pme->m_pSoftKey)
      IMENUCTL_Redraw(pme->m_pSoftKey);
   CTextCtl_Redraw((ITextCtl *)pme);

   //Add By zzg 2010_08_13
   (void)ISHELL_PostEvent(pme->m_pIShell,
                          ISHELL_ActiveApplet(pme->m_pIShell),
                          EVT_USER_REDRAW,
                          0,
                          0);  
   //Add End

   ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TEXT_MODECHANGED, (uint16)CTextCtl_GetInputMode((ITextCtl *)pme, NULL), (uint32)pme);
}

static void TextCtl_CloseFaceSymbolDialog(CTextCtl * pme, AECHAR* string)
{
   TextCtl_ClearScreen(pme); 

   CTextCtl_SetActive((ITextCtl *)pme, TRUE);
   SetArrowFlagonIM(FALSE);    

   pme->m_bShowFaceSyms = FALSE;
   if(string)
      OEM_TextAddString(pme->m_pText,string);

   if(pme->m_pSoftKey)
      IMENUCTL_Redraw(pme->m_pSoftKey);
   CTextCtl_Redraw((ITextCtl *)pme);

   //Add By zzg 2010_08_13
   (void)ISHELL_PostEvent(pme->m_pIShell,
                          ISHELL_ActiveApplet(pme->m_pIShell),
                          EVT_USER_REDRAW,
                          0,
                          0);  
   //Add End

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
   //MSG_FATAL("TextCtl_ReturnToEditMode......",0,0,0);
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
   if(pm)
   {
      nProps = IMENUCTL_GetProperties(pm);
      IMENUCTL_SetProperties(pm, nProps | MP_NO_REDRAW);
      IMENUCTL_SetActive(pm, FALSE);
      IMENUCTL_SetProperties(pm, nProps);
   }
   SetArrowFlagonIM(TRUE);//增加软键栏翻页图标
   CTextCtl_SetActive((ITextCtl *)pme, FALSE);

   TextCtl_KillModeMenu(pme);
   pme->m_bShowSyms = TRUE;


   TextCtl_ShowSymbolPage(pme, 0);
}

static void TextCtl_ShowFaceSymbolDialog(CTextCtl * pme)
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
   SetArrowFlagonIM(TRUE);//增加软键栏翻页图标
   CTextCtl_SetActive((ITextCtl *)pme, FALSE);

   TextCtl_KillModeMenu(pme);
   pme->m_bShowFaceSyms = TRUE;


   TextCtl_ShowFaceSymbolPage(pme, 0);
}

static void TextCtl_ShowNetSymbolDialog(CTextCtl * pme)
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
   pme->m_bShowNetSyms = TRUE;


   TextCtl_ShowNetSymbolPage(pme, 0);
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
      IMENUCTL_SetTitle(pm, AEECONTROLS_RES_FILE, AEE_IDS_MODE_TITLE, NULL);

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

/*=================================================================
Function: TextCtl_SetTextSize

Description:
   This method is used to set text size supported by the
   text buffer. If the size being set is more than the size
   already set, this leads to the freeing up of the memory associated
   with the previous size and allocation of the memory as per new size.
   Text contents are preserved if the buffer length is increased.

Prototype:
   void TextCtl_SetTextSize(CTextCtl *pTextCtl, uint16 nTextSize)

Parameter(s):
   pTextCtl: pointer to CTextCtl object
   nTextSize: desired text buffer size in AECHAR characters excluding NUL

Return Value: none

Comments: none

Side Effects: none

See Also: none
=================================================================*/
static void TextCtl_SetTextSize(CTextCtl *pme, uint16 nTextSize)
{
   AECHAR *pMem;
   uint32 nMemSize;

   //if(pme->m_text == NULL || nTextSize >= TEXT_SIZE(pme)){
   if(pme->m_text == NULL || nTextSize >= WSTRLEN(pme->m_text)){
      nMemSize = (nTextSize + 1) * sizeof(AECHAR);
      if (nMemSize < 0xffff)
         nTextSize = (uint16) nMemSize;
      else
         nTextSize = 0xffff;
      if((pMem = (AECHAR *)REALLOC(pme->m_text,nTextSize)) != NULL){
         pme->m_text = pMem;
         pme->m_textSize = nTextSize;  // Size of the buffer in bytes.
      }
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
   //MSG_FATAL("OEM_TextSetEdit0000000333",0,0,0);
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
   //MSG_FATAL("CTextCtl_SetSelection................",0,0,0);
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

// add the code for when the cursor in begin, the button should be changed
/*===========================================================================
Function: CTextCtl_GetT9End()

Description: This function gets the position of the cursor for the text control.

Prototype:
   static int32 CTextCtl_GetT9End(ITextCtl * po)


Parameter(s):
   po: Pointer to text control whose cursor needs to be retrieved

Return Value: Absolute Position of the cursor

Comments: none

Side Effects:

See Also: none

===========================================================================*/
static int32 CTextCtl_GetT9End(ITextCtl * po)
{
   CTextCtl * pme = (CTextCtl*)po;
   return OEM_TextGetEnd(pme->m_pText);
}
// add end for cursor and button
/*==============================================================================
函数：
       OEM_SetInputMode

说明：
       文本控件输入法设置函数。
       用于不同语言版本的输入法切换

参数：
       pme [in]：CTextCtl t对象指针。

返回值：
        无。

备注：:
        
==============================================================================*/
static void OEM_SetInputMode(CTextCtl * pme)
{
    AEETextInputMode  wMode = TEXT_MODE_MULTITAP;
    boolean is_Taimod = FALSE;
    if ( pme == NULL )
    {
        return;
    }
    
    if ((NULL != pme->m_pSoftKey) || (!(pme->m_dwProps & TP_STARKEY_SWITCH)))
    {   //  只有在设置了 TP_STARKEY_SWITCH 属性且没使用软键的条件下切换输入法
        return;
    }
#ifdef FEATURE_VERSION_C316
	if((pme->m_nCurrInputMode == OEM_MODE_T9_MT_HINDI)&&( pme->m_dwProps & TP_NO_HI))
	{
		  pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH_LOW;
	}
#endif
    MSG_FATAL("OEM_SetInputMode m_nCurrInputMode=%d",pme->m_nCurrInputMode,0,0);
    switch (pme->m_nCurrInputMode)
    {   //输入模式
        case OEM_MODE_NUMBERS:
            wMode = TEXT_MODE_NUMBERS;//数字输入模式
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_NUMBERS_BLACK;
#else              
            pme->m_wResID = IDB_MODE_NUMBERS;
#endif
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break; 
#ifdef FEATURE_MT_ENGLISH_UP
        case OEM_MODE_ENGLISH_UP:
            wMode = TEXT_MODE_MULTITAP_UP;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else

            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;

#endif
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;
#endif
#ifdef FEATURE_MT_ENGLISH_LOW
        case OEM_MODE_ENGLISH_LOW:
            wMode = TEXT_MODE_MULTITAP_LOW;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;
#endif
#ifdef FEATURE_MT_ENGLISH_CAPLOW
        case OEM_MODE_ENGLISH_CAPLOW:
            wMode = TEXT_MODE_MULTITAP;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;
#endif
#ifdef FEATURE_USES_ZI
#ifdef FEATURE_ZI_MT_ENGLISH
        case OEM_MODE_ZI_MT_ENGLISH:
            wMode = TEXT_MODE_MULTITAP;//首字母大写字母输入模式
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW 
	        pme->m_wResID = IDB_MODE_T9_MT_HEBREW_ENGLISH;
#else

#if defined(FEATURE_ALL_KEY_PAD)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;
#endif
#else
			#if  defined(FEATURE_VERSION_W516) ||defined(FEATURE_VERSION_VG68) || defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
			#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
			#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW
			#if defined(FEATURE_VERSION_W515V3)
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP); 
            #else
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP); 
            #endif
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;            
            
        case OEM_MODE_ZI_MT_ENGLISH_LOW:
            wMode = TEXT_MODE_MULTITAP;//小写字母输入模式
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW 
	        pme->m_wResID = IDB_MODE_T9_MT_HEBREW_ENGLISH_LOW;
#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
#endif  // FEATURE_PREPAID_ISRAEL_HEBREW
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL);  
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;

#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH  //add by yangdecai 2010-0909
		case OEM_MODE_ZI_CAP_LOWER_ENGLISH:
			MSG_FATAL("OEM_MODE_ZI_CAP_LOWER_ENGLISH:::::::::::::::::::::::::::::",0,0,0);
			#ifndef  FEATURE_ALL_KEY_PAD
			wMode = AEE_TM_CAPLOWER;//小写字母输入模式
			OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;
#endif
	        #else
			wMode = AEE_TM_CAPLOWER;//大小写字母输入模?
            #if  defined(FEATURE_VERSION_W516) ||defined(FEATURE_VERSION_VG68) || defined(FEATURE_VERSION_C01) || defined(FEATURE_VERSION_W208S)|| defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
			#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
			#endif
			#endif
			
			//OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP);  
			(void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
	        
			break;
#endif
#ifdef FEATURE_MYANMAR_INPUT_MOD
		case OEM_MODE_MYANMAR:
			wMode = AEE_TM_MYANMAR;   //add by yangdecai   2010-12-23
			pme->m_wResID = IDI_MODE_MYANMAR;
			(void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
			break;
#endif

#ifdef FEATURE_T9_MT_RUSSIAN
        case OEM_MODE_T9_MT_RUSSIAN:
            wMode = AEE_TM_RUSSIAN;   
			pme->m_wResID = IDI_MODE_MT_RUSSIAN;
			(void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;
#endif
#ifdef FEATURE_T9_RAPID_RUSSIAN
        case OEM_MODE_T9_RAPID_RUSSIAN:
            wMode = AEE_TM_RUSSIAN_R;   
			pme->m_wResID = IDI_MODE_RAPID_RUSSIAN;
			(void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;
#endif

        case OEM_MODE_ZI_MT_ENGLISH_UP:
            wMode = AEE_TM_LETTERS;//大写字母输入模式
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW 
	        pme->m_wResID = IDB_MODE_T9_MT_HEBREW_ENGLISH_UP;
#else

#if defined(FEATURE_ALL_KEY_PAD)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;
#endif
#else
			#if  defined(FEATURE_VERSION_W516)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
            #else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
			#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW         
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;            
#endif //FEATURE_ZI_MT_ENGLISH

#ifdef FEATURE_ZI_RAPID_ENGLISH
        case OEM_MODE_ZI_RAPID_ENGLISH:
            wMode = TEXT_MODE_ZI_RAPID_ENGLISH;
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW 
	        pme->m_wResID = IDB_MODE_T9_RAPID_HEBREW_ENGLISH;
#else

#if defined(FEATURE_ALL_KEY_PAD)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH;
#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP);  
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;

        case OEM_MODE_ZI_RAPID_ENGLISH_LOW:
            wMode = TEXT_MODE_ZI_RAPID_ENGLISH;
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW  
	        pme->m_wResID = IDB_MODE_T9_RAPID_HEBREW_ENGLISH_LOW;
#else

#if defined(FEATURE_ALL_KEY_PAD)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH;
#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW			
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL);   
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;

        case OEM_MODE_ZI_RAPID_ENGLISH_UP:
            wMode = TEXT_MODE_ZI_RAPID_ENGLISH;
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW 
	        pme->m_wResID = IDB_MODE_T9_RAPID_HEBREW_ENGLISH_UP;
#else

#if defined(FEATURE_ALL_KEY_PAD)
			 pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH_UP;
#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH;
#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS);   
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;            
#endif //FEATURE_MODE_ZI_RAPID_ENGLISH

#ifdef FEATURE_ZI_PINYIN
        case OEM_MODE_ZI_PINYIN:
            wMode = TEXT_MODE_ZI_PINYIN;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_PINYIN_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_PINYIN;
#endif
            break;
#endif //FEATURE_ZI_PINYIN

#ifdef FEATURE_ZI_STROKE
        case OEM_MODE_ZI_STROKE:
            wMode = TEXT_MODE_ZI_STROKE;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_STROKE_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_STROKE;
#endif
            break;
#endif //FEATURE_ZI_STROKE

#ifdef FEATURE_ZI_ZHUYIN
        case OEM_MODE_ZI_ZHUYIN:
            wMode = TEXT_MODE_ZI_ZHUYIN;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_ZHUYIN_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_ZHUYIN;
#endif
            break;
#endif //FEATURE_ZI_ZHUYIN

#ifdef FEATURE_ZI_MT_ARABIC
        case OEM_MODE_ZI_MT_ARABIC:
            wMode = TEXT_MODE_ZI_MT_ARABIC;
            pme->m_wResID = IDB_MODE_T9_MT_ARABIC;
            break;
#endif //FEATURE_ZI_MT_ARABIC

#ifdef FEATURE_ZI_RAPID_ARABIC
        case OEM_MODE_ZI_RAPID_ARABIC:
            wMode = TEXT_MODE_ZI_RAPID_ARABIC;
            pme->m_wResID = IDB_MODE_T9_RAPID_ARABIC;
            break;
#endif //FEATURE_ZI_RAPID_ARABIC

#ifdef FEATURE_ZI_MT_HEBREW
        case OEM_MODE_ZI_MT_HEBREW:   // FEATURE_T9_MT_HEBREW: 
            wMode = TEXT_MODE_ZI_MT_HEBREW;
            pme->m_wResID = IDB_MODE_T9_MT_HEBREW;
            break;
#endif //FEATURE_ZI_MT_HEBREW

#ifdef FEATURE_ZI_RAPID_HEBREW
        case OEM_MODE_ZI_RAPID_HEBREW:
            wMode = TEXT_MODE_ZI_RAPID_HEBREW;
            pme->m_wResID = IDB_MODE_T9_RAPID_HEBREW;
            break;
#endif //FEATURE_ZI_RAPID_HEBREW

#ifdef FEATURE_ZI_MT_HINDI
        case OEM_MODE_ZI_MT_HINDI:
            wMode = TEXT_MODE_ZI_MT_HINDI;
            pme->m_wResID = IDB_MODE_ZI_MT_HINDI;
            break;
#endif //FEATURE_ZI_MT_HINDI

#ifdef FEATURE_ZI_RAPID_HINDI
        case OEM_MODE_ZI_RAPID_HINDI:
            wMode = TEXT_MODE_ZI_RAPID_HINDI;
            pme->m_wResID = IDB_MODE_ZI_RAPID_HINDI;
            break;
#endif //FEATURE_ZI_RAPID_HINDI

#ifdef FEATURE_ZI_MT_THAI
        case OEM_MODE_ZI_MT_THAI:
            wMode = TEXT_MODE_ZI_MT_THAI;
            pme->m_wResID = IDB_MODE_T9_MT_THAI;
            break;
#endif //FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_RAPID_THAI
        case OEM_MODE_ZI_RAPID_THAI:
        	is_Taimod = TRUE;
            wMode = TEXT_MODE_ZI_RAPID_THAI;
            pme->m_wResID = IDB_MODE_T9_RAPID_THAI;
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;
#endif //FEATURE_ZI_RAPID_THAI

#ifdef FEATURE_ZI_MT_SPANISH
        case OEM_MODE_ZI_MT_SPANISH:
            wMode = TEXT_MODE_ZI_MT_SPANISH;
            pme->m_wResID = IDB_MODE_T9_MT_SPANISH;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP);
            break;
	     case OEM_MODE_ZI_MT_SPANISH_LOW:
            wMode = TEXT_MODE_ZI_MT_SPANISH;
            pme->m_wResID = IDB_MODE_T9_MT_SPANISH_LOW;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL); 
            break;
	     case OEM_MODE_ZI_MT_SPANISH_UP:
            wMode = TEXT_MODE_ZI_MT_SPANISH;
            pme->m_wResID = IDB_MODE_T9_MT_SPANISH_UP;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
            break;
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_RAPID_SPANISH
        case OEM_MODE_ZI_RAPID_SPANISH:
            wMode = TEXT_MODE_ZI_RAPID_SPANISH;
            pme->m_wResID = IDB_MODE_T9_RAPID_SPANISH;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP); 
            break;
	     case OEM_MODE_ZI_RAPID_SPANISH_LOW:
            wMode = TEXT_MODE_ZI_RAPID_SPANISH;
            pme->m_wResID = IDB_MODE_T9_RAPID_SPANISH_LOW;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL); 
            break;
	     case OEM_MODE_ZI_RAPID_SPANISH_UP:
            wMode = TEXT_MODE_ZI_RAPID_SPANISH;
            pme->m_wResID = IDB_MODE_T9_RAPID_SPANISH_UP;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
            break;
#endif //FEATURE_ZI_RAPID_SPANISH

#ifdef FEATURE_ZI_MT_PORTUGUESE
        case OEM_MODE_ZI_MT_PORTUGUESE:
            wMode = TEXT_MODE_ZI_MT_PORTUGUESE;
            pme->m_wResID = IDB_MODE_T9_MT_PORTUGUESE;
            break;
#endif //FEATURE_ZI_MT_PORTUGUESE

#ifdef FEATURE_ZI_RAPID_PORTUGUESE
        case OEM_MODE_ZI_RAPID_PORTUGUESE:
            wMode = TEXT_MODE_ZI_RAPID_PORTUGUESE;
            pme->m_wResID = IDB_MODE_T9_RAPID_PORTUGUESE;
            break;
#endif //FEATURE_ZI_RAPID_PORTUGUESE

#ifdef FEATURE_ZI_MT_INDONESIAN
        case OEM_MODE_ZI_MT_INDONESIAN:
            wMode = TEXT_MODE_ZI_MT_INDONESIAN;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_INDONESIAN_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_INDONESIAN; 
#endif
            break;
#endif //FEATURE_ZI_MT_INDONESIAN

#ifdef FEATURE_ZI_RAPID_INDONESIAN
        case OEM_MODE_ZI_RAPID_INDONESIAN:
            wMode = TEXT_MODE_ZI_RAPID_INDONESIAN;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID= IDB_MODE_T9_RAPID_INDONESIAN_BLACK;
#else
            pme->m_wResID= IDB_MODE_T9_RAPID_INDONESIAN;//IDB_MODE_T9_TEZ_INDONESIAN;  
#endif            
            break;
#endif //FEATURE_ZI_RAPID_INDONESIAN

#ifdef FEATURE_ZI_MT_VIETNAMESE
        case OEM_MODE_ZI_MT_VIETNAMESE:
            wMode = TEXT_MODE_ZI_MT_VIETNAMESE;
            pme->m_wResID = IDB_MODE_T9_MT_VIETNAMESE;
            break;
#endif //FEATURE_ZI_MT_VIETNAMESE

#ifdef FEATURE_ZI_RAPID_VIETNAMESE
        case OEM_MODE_ZI_RAPID_VIETNAMESE:
            wMode = TEXT_MODE_ZI_RAPID_VIETNAMESE;
            pme->m_wResID = IDB_MODE_T9_RAPID_VIETNAMESE;
            break;
#endif //FEATURE_ZI_RAPID_VIETNAMESE

#ifdef FEATURE_ZI_MT_FRENCH
        case OEM_MODE_ZI_MT_FRENCH:
            wMode = TEXT_MODE_ZI_MT_FRENCH;
            pme->m_wResID = IDB_MODE_T9_MT_FRENCH;
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP); 
            break;
	     case OEM_MODE_ZI_MT_FRENCH_LOW:
            wMode = TEXT_MODE_ZI_MT_FRENCH;
            pme->m_wResID = IDB_MODE_T9_MT_FRENCH_LOW;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL); 
            break;
	     case OEM_MODE_ZI_MT_FRENCH_UP:
            wMode = TEXT_MODE_ZI_MT_FRENCH;
            pme->m_wResID = IDB_MODE_T9_MT_FRENCH_UP;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
            break;
#endif //FEATURE_ZI_MT_FRENCH

#ifdef FEATURE_ZI_RAPID_FRENCH
        case OEM_MODE_ZI_RAPID_FRENCH:
            wMode = TEXT_MODE_ZI_RAPID_FRENCH;
            pme->m_wResID = IDB_MODE_T9_RAPID_FRENCH;
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP); 
            break;
	     case OEM_MODE_ZI_RAPID_FRENCH_LOW:
            wMode = TEXT_MODE_ZI_RAPID_FRENCH;
            pme->m_wResID = IDB_MODE_T9_RAPID_FRENCH_LOW;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL); 
            break;
	     case OEM_MODE_ZI_RAPID_FRENCH_UP:
            wMode = TEXT_MODE_ZI_RAPID_FRENCH;
            pme->m_wResID = IDB_MODE_T9_RAPID_FRENCH_UP;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
            break;
#endif //FEATURE_ZI_RAPID_FRENCH

#ifdef FEATURE_ZI_JAPANESE
        case OEM_MODE_ZI_JAPANESE:
            wMode = TEXT_MODE_ZI_JAPANESE;
            pme->m_wResID = IDB_MODE_T9_JAPANESE;
            break;
#endif //FEATURE_ZI_JAPANESE

#ifdef FEATURE_ZI_KOREAN
        case OEM_MODE_T9_KOREAN:
            wMode = TEXT_MODE_ZI_KOREAN;
            pme->m_wResID = IDB_MODE_T9_KOREAN;
            break;
#endif //FEATURE_ZI_KOREAN

#ifdef FEATURE_ZI_ITALIAN
        case OEM_MODE_ZI_ITALIAN:
            wMode = TEXT_MODE_ZI_ITALIAN;
            pme->m_wResID = IDB_MODE_T9_ITALIAN;
            break;
#endif //FEATURE_ZI_ITALIAN

#else
#ifdef FEATURE_T9_MT_ENGLISH
        case OEM_MODE_T9_MT_ENGLISH:
            wMode = TEXT_MODE_MULTITAP;//首字母大写字母输入模式
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW 
	        pme->m_wResID = IDB_MODE_T9_MT_HEBREW_ENGLISH;
#else

#if defined(FEATURE_ALL_KEY_PAD)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;
#endif
#else
			#if  defined(FEATURE_VERSION_W516)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
            #elif defined(FEATURE_VERSION_VG68) || defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;
#endif
			#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
			#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW
			#if defined(FEATURE_VERSION_W515V3)
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP); 
            #elif defined(FEATURE_VERSION_VG68) || defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180)
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS);
            #else
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP); 
            #endif
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;            
            
        case OEM_MODE_T9_MT_ENGLISH_LOW:
            wMode = TEXT_MODE_MULTITAP;//小写字母输入模式
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW 
	        pme->m_wResID = IDB_MODE_T9_MT_HEBREW_ENGLISH_LOW;
#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
#endif  // FEATURE_PREPAID_ISRAEL_HEBREW
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL);  
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;

#ifdef FEATURE_T9_CAP_LOWER_ENGLISH  //add by yangdecai 2010-0909
		case OEM_MODE_T9_CAP_LOWER_ENGLISH:
			MSG_FATAL("OEM_MODE_T9_CAP_LOWER_ENGLISH:::::::::::::::::::::::::::::",0,0,0);
			#ifndef  FEATURE_ALL_KEY_PAD
			wMode = AEE_TM_CAPLOWER;//小写字母输入模式
			OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;
#endif
            MSG_FATAL("IDB_MODE_T9_MT_ENGLISH_UP",0,0,0);
	        #else
			wMode = AEE_TM_CAPLOWER;//大小写字母输入模?
			#if  defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W208S)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
			#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
			#endif
			#endif
			
			//OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP);  
			(void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
	        
			break;
#endif



        case OEM_MODE_T9_MT_ENGLISH_UP:
            wMode = AEE_TM_LETTERS;//大写字母输入模式
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW 
	        pme->m_wResID = IDB_MODE_T9_MT_HEBREW_ENGLISH_UP;
#else

#if defined(FEATURE_ALL_KEY_PAD)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_UP;
#endif
#else
			#if  defined(FEATURE_VERSION_W516) ||defined(FEATURE_VERSION_VG68) || defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_C11) || defined(FEATURE_VERSION_C180)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
			#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH;
#endif
			#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW         
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;            
#endif //FEATURE_T9_MT_ENGLISH

#ifdef FEATURE_MYANMAR_INPUT_MOD
		case OEM_MODE_MYANMAR:
			wMode = AEE_TM_MYANMAR;   //add by yangdecai   2010-12-23
			pme->m_wResID = IDI_MODE_MYANMAR;
			(void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
			break;
#endif
#ifdef FEATURE_MT_MYANMRA
		case OEM_MT_MODE_MYANMAR:
			wMode = AEE_TM_MYANMAR_R;	//add by yangdecai	 2010-12-23
			pme->m_wResID = IDI_MODE_MYANMAR;
			(void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
							  (void*)&is_Taimod,
							  sizeof(boolean));
			break;
#endif

#ifdef FEATURE_T9_MT_RUSSIAN
        case OEM_MODE_T9_MT_RUSSIAN:
            wMode = AEE_TM_RUSSIAN;   
            pme->m_wResID = IDI_MODE_MT_RUSSIAN;
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
                              (void*)&is_Taimod,
                              sizeof(boolean));
            break;
#endif

#ifdef FEATURE_T9_RAPID_RUSSIAN
        case OEM_MODE_T9_RAPID_RUSSIAN:
            wMode = AEE_TM_RUSSIAN_R;   
            pme->m_wResID = IDI_MODE_RAPID_RUSSIAN;
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
                              (void*)&is_Taimod,
                              sizeof(boolean));
            break;
#endif

#ifdef FEATURE_T9_RAPID_ENGLISH
        case OEM_MODE_T9_RAPID_ENGLISH:
            wMode = TEXT_MODE_T9_RAPID_ENGLISH;
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW 
	        pme->m_wResID = IDB_MODE_T9_RAPID_HEBREW_ENGLISH;
#else

#if defined(FEATURE_ALL_KEY_PAD)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH;
#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP);  
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;

        case OEM_MODE_T9_RAPID_ENGLISH_LOW:
            wMode = TEXT_MODE_T9_RAPID_ENGLISH;
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW  
	        pme->m_wResID = IDB_MODE_T9_RAPID_HEBREW_ENGLISH_LOW;
#else

#if defined(FEATURE_ALL_KEY_PAD)
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_ENGLISH_LOW;
#endif
#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH;
#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW			
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL);   
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;

        case OEM_MODE_T9_RAPID_ENGLISH_UP:
            wMode = TEXT_MODE_T9_RAPID_ENGLISH;
#ifdef FEATURE_PREPAID_ISRAEL_HEBREW 
	        pme->m_wResID = IDB_MODE_T9_RAPID_HEBREW_ENGLISH_UP;
#else

#if defined(FEATURE_ALL_KEY_PAD)
			 pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH_UP;
#else
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_RAPID_ENGLISH;
#endif
#endif

#endif  // FEATURE_PREPAID_ISRAEL_HEBREW
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS);   
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;            
#endif //FEATURE_MODE_T9_RAPID_ENGLISH

#ifdef FEATURE_T9_PINYIN
        case OEM_MODE_T9_PINYIN:
            wMode = TEXT_MODE_T9_PINYIN;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_PINYIN_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_PINYIN;
#endif
            break;
#endif //FEATURE_T9_PINYIN

#ifdef FEATURE_T9_STROKE
        case OEM_MODE_T9_STROKE:
            wMode = TEXT_MODE_T9_STROKE;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_STROKE_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_STROKE;
#endif
            break;
#endif //FEATURE_T9_STROKE

#ifdef FEATURE_T9_ZHUYIN
        case OEM_MODE_T9_ZHUYIN:
            wMode = TEXT_MODE_T9_ZHUYIN;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_ZHUYIN_BLACK;    
#else
            pme->m_wResID = IDB_MODE_T9_ZHUYIN;
#endif
            break;
#endif //FEATURE_T9_ZHUYIN

#ifdef FEATURE_T9_MT_ARABIC
        case OEM_MODE_T9_MT_ARABIC:
            wMode = TEXT_MODE_T9_MT_ARABIC;
            pme->m_wResID = IDB_MODE_T9_MT_ARABIC;
            break;
#endif //FEATURE_T9_MT_ARABIC

#ifdef FEATURE_T9_RAPID_ARABIC
        case OEM_MODE_T9_RAPID_ARABIC:
            wMode = TEXT_MODE_T9_RAPID_ARABIC;
            pme->m_wResID = IDB_MODE_T9_RAPID_ARABIC;
            break;
#endif //FEATURE_T9_RAPID_ARABIC

#ifdef FEATURE_T9_MT_HEBREW
        case OEM_MODE_T9_MT_HEBREW:   // FEATURE_T9_MT_HEBREW: 
            wMode = TEXT_MODE_T9_MT_HEBREW;
            pme->m_wResID = IDB_MODE_T9_MT_HEBREW;
            break;
#endif //FEATURE_T9_MT_HEBREW

#ifdef FEATURE_T9_RAPID_HEBREW
        case OEM_MODE_T9_RAPID_HEBREW:
            wMode = TEXT_MODE_T9_RAPID_HEBREW;
            pme->m_wResID = IDB_MODE_T9_RAPID_HEBREW;
            break;
#endif //FEATURE_T9_RAPID_HEBREW

#ifdef FEATURE_T9_MT_HINDI
        case OEM_MODE_T9_MT_HINDI:
            wMode = TEXT_MODE_T9_MT_HINDI;

            pme->m_wResID = IDB_MODE_T9_MT_HINDI;

            break;
#endif //FEATURE_T9_MT_HINDI


#ifdef FEATURE_T9_RAPID_HINDI
        case OEM_MODE_T9_RAPID_HINDI:
            wMode = TEXT_MODE_T9_RAPID_HINDI;
            pme->m_wResID = IDB_MODE_T9_RAPID_HINDI;
            break;
#endif //FEATURE_T9_RAPID_HINDI

#ifdef FEATURE_T9_MT_THAI
        case OEM_MODE_T9_MT_THAI:
            wMode = TEXT_MODE_T9_MT_THAI;
            pme->m_wResID = IDB_MODE_T9_MT_THAI;
            break;
#endif //FEATURE_T9_MT_THAI

#ifdef FEATURE_T9_RAPID_THAI
        case OEM_MODE_T9_RAPID_THAI:
        	is_Taimod = TRUE;
            wMode = TEXT_MODE_T9_RAPID_THAI;
            pme->m_wResID = IDB_MODE_T9_RAPID_THAI;
            (void)OEM_SetConfig(CFGI_LANGUAGE_MOD,
	                          (void*)&is_Taimod,
	                          sizeof(boolean));
            break;
#endif //FEATURE_T9_RAPID_THAI

#ifdef FEATURE_T9_MT_SPANISH
        case OEM_MODE_T9_MT_SPANISH:
            wMode = TEXT_MODE_T9_MT_SPANISH;
            pme->m_wResID = IDB_MODE_T9_MT_SPANISH;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP);
            break;
	     case OEM_MODE_T9_MT_SPANISH_LOW:
            wMode = TEXT_MODE_T9_MT_SPANISH;
            pme->m_wResID = IDB_MODE_T9_MT_SPANISH_LOW;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL); 
            break;
	     case OEM_MODE_T9_MT_SPANISH_UP:
            wMode = TEXT_MODE_T9_MT_SPANISH;
            pme->m_wResID = IDB_MODE_T9_MT_SPANISH_UP;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
            break;
#endif //FEATURE_T9_MT_SPANISH

#ifdef FEATURE_T9_RAPID_SPANISH
        case OEM_MODE_T9_RAPID_SPANISH:
            wMode = TEXT_MODE_T9_RAPID_SPANISH;
            pme->m_wResID = IDB_MODE_T9_RAPID_SPANISH;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP); 
            break;
	     case OEM_MODE_T9_RAPID_SPANISH_LOW:
            wMode = TEXT_MODE_T9_RAPID_SPANISH;
            pme->m_wResID = IDB_MODE_T9_RAPID_SPANISH_LOW;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL); 
            break;
	     case OEM_MODE_T9_RAPID_SPANISH_UP:
            wMode = TEXT_MODE_T9_RAPID_SPANISH;
            pme->m_wResID = IDB_MODE_T9_RAPID_SPANISH_UP;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
            break;
#endif //FEATURE_T9_RAPID_SPANISH

#ifdef FEATURE_T9_MT_PORTUGUESE
        case OEM_MODE_T9_MT_PORTUGUESE:
            wMode = TEXT_MODE_T9_MT_PORTUGUESE;
            pme->m_wResID = IDB_MODE_T9_MT_PORTUGUESE;
            break;
#endif //FEATURE_T9_MT_PORTUGUESE

#ifdef FEATURE_T9_RAPID_PORTUGUESE
        case OEM_MODE_T9_RAPID_PORTUGUESE:
            wMode = TEXT_MODE_T9_RAPID_PORTUGUESE;
            pme->m_wResID = IDB_MODE_T9_RAPID_PORTUGUESE;
            break;
#endif //FEATURE_T9_RAPID_PORTUGUESE

#ifdef FEATURE_T9_MT_INDONESIAN
        case OEM_MODE_T9_MT_INDONESIAN:
            wMode = TEXT_MODE_T9_MT_INDONESIAN;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_T9_MT_INDONESIAN_BLACK;
#else
            pme->m_wResID = IDB_MODE_T9_MT_INDONESIAN; 
#endif
            break;
#endif //FEATURE_T9_MT_INDONESIAN

#ifdef FEATURE_T9_RAPID_INDONESIAN
        case OEM_MODE_T9_RAPID_INDONESIAN:
            wMode = TEXT_MODE_T9_RAPID_INDONESIAN;
#ifdef FEATURE_WHITE_BG
            pme->m_wResID= IDB_MODE_T9_RAPID_INDONESIAN_BLACK;
#else
            pme->m_wResID= IDB_MODE_T9_RAPID_INDONESIAN;//IDB_MODE_T9_TEZ_INDONESIAN;  
#endif  
            break;
#endif //FEATURE_T9_RAPID_INDONESIAN

#ifdef FEATURE_T9_MT_VIETNAMESE
        case OEM_MODE_T9_MT_VIETNAMESE:
            wMode = TEXT_MODE_T9_MT_VIETNAMESE;
            pme->m_wResID = IDB_MODE_T9_MT_VIETNAMESE;
            break;
#endif //FEATURE_T9_MT_VIETNAMESE

#ifdef FEATURE_T9_RAPID_VIETNAMESE
        case OEM_MODE_T9_RAPID_VIETNAMESE:
            wMode = TEXT_MODE_T9_RAPID_VIETNAMESE;
            pme->m_wResID = IDB_MODE_T9_RAPID_VIETNAMESE;
            break;
#endif //FEATURE_T9_RAPID_VIETNAMESE

#ifdef FEATURE_T9_MT_FRENCH
        case OEM_MODE_T9_MT_FRENCH:
            wMode = TEXT_MODE_T9_MT_FRENCH;
            pme->m_wResID = IDB_MODE_T9_MT_FRENCH;
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP); 
            break;
	     case OEM_MODE_T9_MT_FRENCH_LOW:
            wMode = TEXT_MODE_T9_MT_FRENCH;
            pme->m_wResID = IDB_MODE_T9_MT_FRENCH_LOW;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL); 
            break;
	     case OEM_MODE_T9_MT_FRENCH_UP:
            wMode = TEXT_MODE_T9_MT_FRENCH;
            pme->m_wResID = IDB_MODE_T9_MT_FRENCH_UP;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
            break;
#endif //FEATURE_T9_MT_FRENCH

#ifdef FEATURE_T9_RAPID_FRENCH
        case OEM_MODE_T9_RAPID_FRENCH:
            wMode = TEXT_MODE_T9_RAPID_FRENCH;
            pme->m_wResID = IDB_MODE_T9_RAPID_FRENCH;
            OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_FIRST_CAP); 
            break;
	     case OEM_MODE_T9_RAPID_FRENCH_LOW:
            wMode = TEXT_MODE_T9_RAPID_FRENCH;
            pme->m_wResID = IDB_MODE_T9_RAPID_FRENCH_LOW;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_SMALL); 
            break;
	     case OEM_MODE_T9_RAPID_FRENCH_UP:
            wMode = TEXT_MODE_T9_RAPID_FRENCH;
            pme->m_wResID = IDB_MODE_T9_RAPID_FRENCH_UP;
	        OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
            break;
#endif //FEATURE_T9_RAPID_FRENCH

#ifdef FEATURE_T9_JAPANESE
        case OEM_MODE_T9_JAPANESE:
            wMode = TEXT_MODE_T9_JAPANESE;
            pme->m_wResID = IDB_MODE_T9_JAPANESE;
            break;
#endif //FEATURE_T9_JAPANESE

#ifdef FEATURE_T9_KOREAN
        case OEM_MODE_T9_KOREAN:
            wMode = TEXT_MODE_T9_KOREAN;
            pme->m_wResID = IDB_MODE_T9_KOREAN;
            break;
#endif //FEATURE_T9_KOREAN

#ifdef FEATURE_T9_ITALIAN
        case OEM_MODE_T9_ITALIAN:
            wMode = TEXT_MODE_T9_ITALIAN;
            pme->m_wResID = IDB_MODE_T9_ITALIAN;
            break;
#endif //FEATURE_T9_ITALIAN
#endif

        default:
            wMode = TEXT_MODE_NUMBERS;//数字输入模式
#ifdef FEATURE_WHITE_BG
            pme->m_wResID = IDB_MODE_NUMBERS_BLACK;
#else              
            pme->m_wResID = IDB_MODE_NUMBERS;
#endif
            break;
        }
	MSG_FATAL("2pme->m_wResID=%d,wMode=%d",pme->m_wResID,wMode,0);
    //设置输入法
    (void)CTextCtl_SetInputMode((ITextCtl *)pme, wMode);
}

/*=================================================================
Function: TextCtl_SetInputList

Description:

Prototype:
   static void TextCtl_SetInputList(CTextCtl *pme, uint16 nInputSet)

Parameter(s):
   pTextCtl: pointer to CTextCtl object
   

Return Value: none

Comments: none

Side Effects: none

See Also: none
=================================================================*/
static void TextCtl_SetInputList(CTextCtl *pme)
{
    int i=0;
#ifdef FEATURE_MT_ENGLISH_LOW
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ENGLISH_LOW;
#endif
#ifdef FEATURE_MT_ENGLISH_UP
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ENGLISH_UP;
#endif
#ifdef FEATURE_MT_ENGLISH_CAPLOW
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ENGLISH_CAPLOW;
#endif
#ifdef FEATURE_USES_ZI
#ifdef FEATURE_ZI_RAPID_SPANISH
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_SPANISH;
#endif //FEATURE_ZI_RAPID_SPANISH

#ifdef FEATURE_ZI_MT_SPANISH
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_SPANISH;
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_RAPID_FRENCH
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_FRENCH;
#endif //FEATURE_ZI_RAPID_FRENCH

#ifdef FEATURE_ZI_MT_FRENCH
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_FRENCH;
#endif //FEATURE_ZI_MT_FRENCH

#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
	MSG_FATAL("TextCtl_SetInputList...000",0,0,0);
    pme->m_nCurrInputModeList[i++] = OEM_MODE_NUMBERS; 

#ifdef FEATURE_ZI_RAPID_ENGLISH
	MSG_FATAL("TextCtl_SetInputList...111",0,0,0);
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_ENGLISH;
#endif //FEATURE_MODE_ZI_RAPID_ENGLISH

#ifdef FEATURE_ZI_MT_ENGLISH
	MSG_FATAL("TextCtl_SetInputList...222",0,0,0);
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_ENGLISH; 
#endif //FEATURE_ZI_MT_ENGLISH

#else //FEATURE_CARRIER_VENEZUELA_MOVILNET

#ifdef FEATURE_ZI_MT_ENGLISH
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| (defined(FEATURE_VERSION_C01)&&defined(FEATURE_LANG_ARABIC))
	pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_ENGLISH_LOW;
	pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_ENGLISH_UP;
#else
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_ENGLISH;
#endif
#endif //FEATURE_ZI_MT_ENGLISH

#ifdef FEATURE_ZI_RAPID_ENGLISH
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_ENGLISH;
#endif //FEATURE_MODE_ZI_RAPID_ENGLISH
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
#else
#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH   //add by yangdecai 2010-09-09
	pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_CAP_LOWER_ENGLISH;
	#if defined(FEATURE_VERSION_C01)||defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180)|| defined(FEATURE_VERSION_W317A)
	OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
	#endif
#endif
#endif
    pme->m_nCurrInputModeList[i++] = OEM_MODE_NUMBERS;  

#endif // FEATURE_CARRIER_VENEZUELA_MOVILNET
#ifdef FEATURE_MYANMAR_INPUT_MOD    //add by yangdecai 20101223
		pme->m_nCurrInputModeList[i++] = OEM_MODE_MYANMAR;

#endif
#ifdef FEATURE_MT_MYANMRA  //add by yangdecai 20101223
		pme->m_nCurrInputModeList[i++] = OEM_MT_MODE_MYANMAR;

#endif

#ifdef FEATURE_T9_MT_RUSSIAN
        pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_RUSSIAN;
#endif

#ifdef FEATURE_T9_RAPID_RUSSIAN
        pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_RUSSIAN;
#endif


#ifdef FEATURE_ZI_PINYIN
	MSG_FATAL("TextCtl_SetInputList...333",0,0,0);
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_PINYIN;
#endif //FEATURE_ZI_PINYIN

#ifdef FEATURE_ZI_ZHUYIN
   pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_ZHUYIN;
#endif //FEATURE_ZI_ZHUYIN

#ifdef FEATURE_ZI_STROKE
	MSG_FATAL("TextCtl_SetInputList...444",0,0,0);
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_STROKE;
#endif //FEATURE_ZI_STROKE

#ifdef FEATURE_ZI_MT_ARABIC
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_ARABIC;
#endif //FEATURE_ZI_MT_ARABIC

#ifdef FEATURE_ZI_RAPID_ARABIC
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_ARABIC;
#endif //FEATURE_ZI_RAPID_ARABIC

#ifdef FEATURE_ZI_MT_HEBREW
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_HEBREW;
#endif //FEATURE_T9_MT_HEBREW

#ifdef FEATURE_ZI_RAPID_HEBREW
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_HEBREW;
#endif //FEATURE_ZI_RAPID_HEBREW

#ifdef FEATURE_ZI_MT_HINDI
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_HINDI;
#endif //FEATURE_ZI_MT_HINDI

#ifdef FEATURE_ZI_RAPID_HINDI
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_HINDI;
#endif //FEATURE_ZI_RAPID_HINDI

#ifdef FEATURE_ZI_MT_THAI
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_THAI;
#endif //FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_RAPID_THAI
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_THAI;
#endif //FEATURE_ZI_RAPID_THAI

#ifdef FEATURE_ZI_MT_PORTUGUESE
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_PORTUGUESE;
#endif //FEATURE_ZI_MT_PORTUGUESE

#ifdef FEATURE_ZI_RAPID_PORTUGUESE
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_PORTUGUESE;
#endif //FEATURE_ZI_RAPID_PORTUGUESE

#ifdef FEATURE_ZI_MT_INDONESIAN
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_INDONESIAN;
#endif //FEATURE_ZI_MT_INDONESIAN

#ifdef FEATURE_ZI_RAPID_INDONESIAN
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_INDONESIAN;
#endif //FEATURE_ZI_RAPID_INDONESIAN

#ifdef FEATURE_ZI_MT_VIETNAMESE
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_MT_VIETNAMESE;
#endif //FEATURE_ZI_MT_VIETNAMESE

#ifdef FEATURE_ZI_RAPID_VIETNAMESE
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_RAPID_VIETNAMESE;
#endif //FEATURE_ZI_RAPID_VIETNAMESE

#ifdef FEATURE_ZI_JAPANESE
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_JAPANESE;
#endif //FEATURE_ZI_JAPANESE

#ifdef FEATURE_ZI_KOREAN
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_KOREAN;
#endif //FEATURE_ZI_KOREAN

#ifdef FEATURE_ZI_ITALIAN
    pme->m_nCurrInputModeList[i++] = OEM_MODE_ZI_ITALIAN;
#endif //FEATURE_ZI_ITALIAN

#else
#ifdef FEATURE_T9_RAPID_SPANISH
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_SPANISH;
#endif //FEATURE_T9_RAPID_SPANISH

#ifdef FEATURE_T9_MT_SPANISH
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_SPANISH;
#endif //FEATURE_T9_MT_SPANISH

#ifdef FEATURE_T9_RAPID_FRENCH
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_FRENCH;
#endif //FEATURE_T9_RAPID_FRENCH

#ifdef FEATURE_T9_MT_FRENCH
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_FRENCH;
#endif //FEATURE_T9_MT_FRENCH

#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
	MSG_FATAL("TextCtl_SetInputList...000",0,0,0);
    pme->m_nCurrInputModeList[i++] = OEM_MODE_NUMBERS; 

#ifdef FEATURE_T9_RAPID_ENGLISH
	MSG_FATAL("TextCtl_SetInputList...111",0,0,0);
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_ENGLISH;
#endif //FEATURE_MODE_T9_RAPID_ENGLISH

#ifdef FEATURE_T9_MT_ENGLISH
	MSG_FATAL("TextCtl_SetInputList...222",0,0,0);
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_ENGLISH; 
#endif //FEATURE_T9_MT_ENGLISH

#else //FEATURE_CARRIER_VENEZUELA_MOVILNET

#ifdef FEATURE_T9_MT_ENGLISH
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)//|| (defined(FEATURE_VERSION_C01)&&defined(FEATURE_LANG_ARABIC))
	pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_ENGLISH_LOW;
	pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_ENGLISH_UP;
#else
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_ENGLISH;
#endif
#endif //FEATURE_T9_MT_ENGLISH

#ifdef FEATURE_T9_RAPID_ENGLISH
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_ENGLISH;
#endif //FEATURE_MODE_T9_RAPID_ENGLISH
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
#else
#ifdef FEATURE_T9_CAP_LOWER_ENGLISH   //add by yangdecai 2010-09-09
	pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_CAP_LOWER_ENGLISH;
	#if defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W317A)
	OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
	#endif
#endif
#endif
    pme->m_nCurrInputModeList[i++] = OEM_MODE_NUMBERS;  

#endif // FEATURE_CARRIER_VENEZUELA_MOVILNET
#ifdef FEATURE_MYANMAR_INPUT_MOD    //add by yangdecai 20101223
		pme->m_nCurrInputModeList[i++] = OEM_MODE_MYANMAR;

#endif
#ifdef FEATURE_MT_MYANMRA  //add by yangdecai 20101223
		pme->m_nCurrInputModeList[i++] = OEM_MT_MODE_MYANMAR;

#endif

#ifdef FEATURE_T9_MT_RUSSIAN
        pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_RUSSIAN;
#endif

#ifdef FEATURE_T9_RAPID_RUSSIAN
        pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_RUSSIAN;
#endif


#ifdef FEATURE_T9_PINYIN
	MSG_FATAL("TextCtl_SetInputList...333",0,0,0);
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_PINYIN;
#endif //FEATURE_T9_PINYIN

#ifdef FEATURE_T9_ZHUYIN
   pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_ZHUYIN;
#endif //FEATURE_T9_ZHUYIN

#ifdef FEATURE_T9_STROKE
	MSG_FATAL("TextCtl_SetInputList...444",0,0,0);
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_STROKE;
#endif //FEATURE_T9_STROKE

#ifdef FEATURE_T9_MT_ARABIC
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_ARABIC;
#endif //FEATURE_T9_MT_ARABIC

#ifdef FEATURE_T9_RAPID_ARABIC
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_ARABIC;
#endif //FEATURE_T9_RAPID_ARABIC

#ifdef FEATURE_T9_MT_HEBREW
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_HEBREW;
#endif //FEATURE_T9_MT_HEBREW

#ifdef FEATURE_T9_RAPID_HEBREW
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_HEBREW;
#endif //FEATURE_T9_RAPID_HEBREW

#ifdef FEATURE_T9_MT_HINDI
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_HINDI;
#endif //FEATURE_T9_MT_HINDI

#ifdef FEATURE_T9_RAPID_HINDI
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_HINDI;
#endif //FEATURE_T9_RAPID_HINDI

#ifdef FEATURE_T9_MT_THAI
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_THAI;
#endif //FEATURE_T9_MT_THAI

#ifdef FEATURE_T9_RAPID_THAI
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_THAI;
#endif //FEATURE_T9_RAPID_THAI

#ifdef FEATURE_T9_MT_PORTUGUESE
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_PORTUGUESE;
#endif //FEATURE_T9_MT_PORTUGUESE

#ifdef FEATURE_T9_RAPID_PORTUGUESE
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_PORTUGUESE;
#endif //FEATURE_T9_RAPID_PORTUGUESE

#ifdef FEATURE_T9_MT_INDONESIAN
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_INDONESIAN;
#endif //FEATURE_T9_MT_INDONESIAN

#ifdef FEATURE_T9_RAPID_INDONESIAN
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_INDONESIAN;
#endif //FEATURE_T9_RAPID_INDONESIAN

#ifdef FEATURE_T9_MT_VIETNAMESE
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_MT_VIETNAMESE;
#endif //FEATURE_T9_MT_VIETNAMESE

#ifdef FEATURE_T9_RAPID_VIETNAMESE
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_RAPID_VIETNAMESE;
#endif //FEATURE_T9_RAPID_VIETNAMESE

#ifdef FEATURE_T9_JAPANESE
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_JAPANESE;
#endif //FEATURE_T9_JAPANESE

#ifdef FEATURE_T9_KOREAN
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_KOREAN;
#endif //FEATURE_T9_KOREAN

#ifdef FEATURE_T9_ITALIAN
    pme->m_nCurrInputModeList[i++] = OEM_MODE_T9_ITALIAN;
#endif //FEATURE_T9_ITALIAN
#endif
    pme->m_nCurrInputModeCount = i;

}

/*=================================================================
Function: TextCtl_SetNextInputMode

Description:

Prototype:
   static boolean TextCtl_SetNextInputMode(CTextCtl *pme)

Parameter(s):
   CTextCtl: pointer to CTextCtl object

Return Value: none

Comments: none

Side Effects: none

See Also: none
=================================================================*/
static boolean TextCtl_SetNextInputMode(CTextCtl *pme)
{
    boolean ret = FALSE;
    int i;
    MSG_FATAL("Mode==%d",pme->m_nCurrInputMode,0,0);
	MSG_FATAL("ModeCount::=%d",pme->m_nCurrInputModeCount,0,0);
	#ifdef FEATURE_VERSION_C01
	pme->m_isAvk1 = FALSE;
	#endif
	#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
	#ifdef FEATURE_USES_ZI
	if((pme->m_nCurrInputMode == OEM_MODE_ZI_MT_THAI))
	{
		pme->m_nCurrInputMode = OEM_MODE_ZI_RAPID_ENGLISH_LOW;
	}
	else
	{
		pme->m_nCurrInputMode = OEM_MODE_ZI_MT_THAI;
	}
	#else
	if((pme->m_nCurrInputMode == OEM_MODE_T9_MT_THAI))
	{
		pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_ENGLISH_LOW;
	}
	else
	{
		pme->m_nCurrInputMode = OEM_MODE_T9_MT_THAI;
	}
	#endif
	#else
	if(pme->m_dwProps & TP_STARKEY_ID_SWITCH)
	{
		#ifdef FEATURE_USES_ZI
		if(pme->m_nCurrInputMode == OEM_MODE_ZI_MT_ENGLISH || 
                pme->m_nCurrInputMode == OEM_MODE_ZI_MT_ENGLISH_LOW ||
                pme->m_nCurrInputMode == OEM_MODE_ZI_MT_ENGLISH_UP ||
                pme->m_nCurrInputMode == OEM_MODE_NUMBERS)
            {
            	#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH   //add by yangdecai
                	pme->m_nCurrInputMode = OEM_MODE_ZI_CAP_LOWER_ENGLISH;
                	#if defined(FEATURE_VERSION_C01)||defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W317A)
                	OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
                	#endif
                #endif
            }
            else
            {
            	pme->m_nCurrInputMode = OEM_MODE_NUMBERS;
            }
		#else
		if(pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH || 
                pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_LOW ||
                pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_UP ||
                pme->m_nCurrInputMode == OEM_MODE_NUMBERS)
            {
            	#ifdef FEATURE_T9_CAP_LOWER_ENGLISH   //add by yangdecai
                	pme->m_nCurrInputMode = OEM_MODE_T9_CAP_LOWER_ENGLISH;
                	#if defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W317A)
                	OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
                	#endif
                #endif
            }
            else
            {
            	pme->m_nCurrInputMode = OEM_MODE_NUMBERS;
            }
		#endif
	}
	#ifdef FEATURE_USES_ZI

	if((pme->m_nCurrInputMode == OEM_MODE_ZI_MT_ENGLISH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_ZI_MT_ENGLISH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_ZI_MT_ENGLISH;
    }

    if((pme->m_nCurrInputMode == OEM_MODE_ZI_RAPID_ENGLISH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_ZI_RAPID_ENGLISH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_ZI_RAPID_ENGLISH;
    }    
#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH
	if(pme->m_nCurrInputMode == OEM_MODE_ZI_CAP_LOWER_ENGLISH)
	{
		pme->m_nCurrInputMode = OEM_MODE_ZI_CAP_LOWER_ENGLISH;
		#if defined(FEATURE_VERSION_C01)||defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W317A)
		OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
		#endif
	}
#endif

#ifdef FEATURE_ZI_MT_SPANISH
    if((pme->m_nCurrInputMode == OEM_MODE_ZI_MT_SPANISH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_T9_MT_SPANISH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_ZI_MT_SPANISH;
    }
#endif
#ifdef FEATURE_ZI_RAPID_SPANISH
    if((pme->m_nCurrInputMode == OEM_MODE_ZI_RAPID_SPANISH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_ZI_RAPID_SPANISH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_ZI_RAPID_SPANISH;
    }
#endif
#ifdef FEATURE_ZI_MT_FRENCH
    if((pme->m_nCurrInputMode == OEM_MODE_ZI_MT_FRENCH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_ZI_MT_FRENCH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_ZI_MT_FRENCH;
    }
#endif
#ifdef FEATURE_ZI_RAPID_FRENCH
    if((pme->m_nCurrInputMode == OEM_MODE_ZI_RAPID_FRENCH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_ZI_RAPID_FRENCH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_ZI_RAPID_FRENCH;
    }
#endif
	
	#else
    if((pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH;
    }

    if((pme->m_nCurrInputMode == OEM_MODE_T9_RAPID_ENGLISH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_T9_RAPID_ENGLISH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_ENGLISH;
    }    
#ifdef FEATURE_T9_CAP_LOWER_ENGLISH
	if(pme->m_nCurrInputMode == OEM_MODE_T9_CAP_LOWER_ENGLISH)
	{
		pme->m_nCurrInputMode = OEM_MODE_T9_CAP_LOWER_ENGLISH;
		#if defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W317A)
		OEM_TextSetMultiCaps(pme->m_pText,MULTITAP_ALL_CAPS); 
		#endif
	}
#endif

#ifdef FEATURE_T9_MT_SPANISH
    if((pme->m_nCurrInputMode == OEM_MODE_T9_MT_SPANISH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_T9_MT_SPANISH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_T9_MT_SPANISH;
    }
#endif
#ifdef FEATURE_T9_RAPID_SPANISH
    if((pme->m_nCurrInputMode == OEM_MODE_T9_RAPID_SPANISH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_T9_RAPID_SPANISH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_SPANISH;
    }
#endif
#ifdef FEATURE_T9_MT_FRENCH
    if((pme->m_nCurrInputMode == OEM_MODE_T9_MT_FRENCH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_T9_MT_FRENCH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_T9_MT_FRENCH;
    }
#endif
#ifdef FEATURE_T9_RAPID_FRENCH
    if((pme->m_nCurrInputMode == OEM_MODE_T9_RAPID_FRENCH_LOW) || (pme->m_nCurrInputMode == OEM_MODE_T9_RAPID_FRENCH_UP))
    {
        pme->m_nCurrInputMode = OEM_MODE_T9_RAPID_FRENCH;
    }
#endif
#endif
#endif
    for (i=0; i<pme->m_nCurrInputModeCount; i++)
    {
        MSG_FATAL("setnextinputmode------i=%d Mode=%d Curr=%d",i,pme->m_nCurrInputModeList[i],pme->m_nCurrInputMode);
		
        if ( pme->m_nCurrInputMode == pme->m_nCurrInputModeList[i] )
        {
			#ifdef FEATURE_VERSION_C316
        	if(pme->m_dwProps & TP_NO_HI)
			{
				if(pme->m_nCurrInputMode == 0)
				{
					i++; 
				}
			}
			#endif
        	 MSG_FATAL("setnextinputmode------i=%d-----pme->m_nCurrInputMode%d",i,pme->m_nCurrInputMode,0);	
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT
            if(pme->m_dwProps & TP_NOSYMBOL)
            {
                if(pme->m_nCurrInputMode == OEM_MODE_NUMBERS)
                {                    
                    pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH_LOW;
                    pme->m_eAutoState = MULTITAP_USER_DENY_AUTOSET;
                }
                else if(pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH || 
                    pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_LOW ||
                    pme->m_nCurrInputMode == OEM_MODE_T9_MT_ENGLISH_UP)
                {
                    pme->m_nCurrInputMode = OEM_MODE_T9_MT_THAI;
                }
                else
                {
                    pme->m_nCurrInputMode = OEM_MODE_NUMBERS;
                }
            }
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT            
            // SMS Thailand version: 
            // when the size have been bigger than 70, it can't switch to Thai Input Mode
            // it is bad to do this here!
           if(pme->m_dwProps & TP_DISPLAY_UNICODE_ASCII_COUNT)
            {
                AECHAR * pszText;
                int nLen = 0;
                int max_unicode;
                pszText = OEM_TextGet(pme->m_pText);
                if ( pszText != NULL )
                {
                    nLen = WSTRLEN ( pszText );
                }
                if(pme->m_dwProps & TP_DISPLAY_SMSCOUNT)
                {
                    //max_unicode = MAX_UNICODMSG_PAYLOAD;
                    max_unicode = OEM_TextGetMaxChars(pme->m_pText)/2;
                }
                else
                {
                    max_unicode = OEM_TextGetMaxChars(pme->m_pText)/2;                    
                }
                
                if ( (nLen > max_unicode) 
                     && IsDoubleBytesIME(TextCtl_Oemmode_Textmode(pme->m_nCurrInputModeList[i+1])))
                {
                    pme->m_nCurrInputMode = pme->m_nCurrInputModeList[0];
                }
                else if (  i == pme->m_nCurrInputModeCount-1 )
                {
                    pme->m_nCurrInputMode = pme->m_nCurrInputModeList[0];
                }
                else
                {
                    pme->m_nCurrInputMode = pme->m_nCurrInputModeList[i+1];
                }

            }
            else
            {
                // if meet the end , then return the first one
                MSG_FATAL("tMode::::end1=%d",pme->m_nCurrInputMode,0,0);
                #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                #else
                if((pme->m_dwProps & TP_STARKEY_ID_SWITCH))
                {
                	;
                }
                else
                {
                	if (  i == pme->m_nCurrInputModeCount-1 )
                	{
                		MSG_FATAL("1111111============%d",i,0,0);
                    	pme->m_nCurrInputMode = pme->m_nCurrInputModeList[0];
                	}
                	else
                	{
                		MSG_FATAL("222222============%d,%d",i,pme->m_nCurrInputModeCount,0);
                		#if defined(FEATURE_VERSION_MYANMAR)||defined(FEATURE_VERSION_MYANMAR_MTM)
                		if(!(pme->m_dwProps & TP_MULTILINE)/*&&( 2== i)*/)
                		{
                            MSG_FATAL("TP_MULTILINE",0,0,0);
                			pme->m_nCurrInputModeCount = 7;
                		}
                		#endif
                    	 
                    	#if defined(FEATURE_VERSION_MYANMAR)||defined(FEATURE_VERSION_MYANMAR_MTM)
                    	pme->m_nCurrInputMode = pme->m_nCurrInputModeList[i+1]; 
                		#else
                		pme->m_nCurrInputMode = pme->m_nCurrInputModeList[i+1];                        
                		#endif
                    }
                }
                #endif
            }
            ret = TRUE;
            break;
        }
    }
    
    #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
    ret = TRUE;
    #endif
    MSG_FATAL("Mode::::end::=%d",pme->m_nCurrInputMode,0,0);
    return ret;
}


/*=================================================================
Function: TextCtl_3STYLE_MultitapTimer

Description:
   timer for checking the conversion between AllSmall mode and FirstCap mode

Parameter(s):

Return Value:

Comments: none

Side Effects: none

=================================================================*/
static void TextCtl_3STYLE_MultitapTimer(void *pUser)
{
    register CTextCtl *pme = (CTextCtl *) pUser;
    
    // if MultitapCapsState have been change to all small in OEMText,
    //then change input mode
    #ifdef FEATURE_USES_ZI
	if ( OEM_MODE_ZI_MT_ENGLISH == pme->m_nCurrInputMode
         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
	#else
    if ( OEM_MODE_T9_MT_ENGLISH == pme->m_nCurrInputMode
         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
    #endif
    {
       // change to the sll small mode
       #ifdef FEATURE_USES_ZI
	   pme->m_nCurrInputMode = OEM_MODE_ZI_MT_ENGLISH_LOW;
	   #else
       pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH_LOW;
	   #endif
       OEM_SetInputMode((CTextCtl *)pme);
    }
    
    // if it have been Auto set to first cap and move the cursor then 
    //change to all small mode
    else if ( ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState 
           || MULTITAP_USER_DENY_AUTOSET == pme->m_eAutoState )
         && OEM_TextGetSelEnd(pme->m_pText) != pme->m_nAutoSetCurPos 
         && OEM_TextGetSelEnd(pme->m_pText) == OEM_TextGetCursorPos(pme->m_pText) )
    {
        if ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState )
        {
            // change to the all small mode
            #ifdef FEATURE_USES_ZI
			pme->m_nCurrInputMode = OEM_MODE_ZI_MT_ENGLISH_LOW;
			#else
            pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH_LOW;
			#endif
            OEM_SetInputMode(pme);
        }
        
        pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
    }

    // if the cursor is in Right position then change to FirstCap.
    //it will be checked in timer
    #ifdef FEATURE_USES_ZI
	else if ( ( OEM_MODE_ZI_MT_ENGLISH_LOW == pme->m_nCurrInputMode
           ||OEM_MODE_ZI_MT_ENGLISH_UP == pme->m_nCurrInputMode )
         && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState 
         && OEM_isFirstCap(pme->m_pText) )
	#else
    else if ( ( OEM_MODE_T9_MT_ENGLISH_LOW == pme->m_nCurrInputMode
           ||OEM_MODE_T9_MT_ENGLISH_UP == pme->m_nCurrInputMode )
         && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState 
         && OEM_isFirstCap(pme->m_pText) )
    #endif
    {
       // change to the fist cap mode
       #ifdef FEATURE_USES_ZI
	   pme->m_nCurrInputMode = OEM_MODE_ZI_MT_ENGLISH;
	   #else
       pme->m_nCurrInputMode = OEM_MODE_T9_MT_ENGLISH;
	   #endif
       OEM_SetInputMode((CTextCtl *)pme);
       
       // set flag
       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
       
       // save the Cursor Pos when Auto Set to first cap.
       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
    }
	#ifdef FEATURE_USES_ZI
	#ifdef FEATURE_ZI_MT_SPANISH
    // if MultitapCapsState have been change to all small in OEMText,
    //then change input mode
    if ( OEM_MODE_ZI_MT_SPANISH == pme->m_nCurrInputMode
         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
    {
       // change to the sll small mode
       pme->m_nCurrInputMode = OEM_MODE_T9_MT_SPANISH_LOW;
       OEM_SetInputMode((CTextCtl *)pme);
    }
    
    // if it have been Auto set to first cap and move the cursor then 
    //change to all small mode
    else if ( ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState 
           || MULTITAP_USER_DENY_AUTOSET == pme->m_eAutoState )
         && OEM_TextGetSelEnd(pme->m_pText) != pme->m_nAutoSetCurPos 
         && OEM_TextGetSelEnd(pme->m_pText) == OEM_TextGetCursorPos(pme->m_pText) )
    {
        if ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState )
        {
            // change to the all small mode
            pme->m_nCurrInputMode = OEM_MODE_ZI_MT_SPANISH_LOW;
            OEM_SetInputMode(pme);
        }
        
        pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
    }

    // if the cursor is in Right position then change to FirstCap.
    //it will be checked in timer
    else if ( ( OEM_MODE_ZI_MT_SPANISH_LOW == pme->m_nCurrInputMode
           ||OEM_MODE_ZI_MT_SPANISH_UP == pme->m_nCurrInputMode )
         && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState 
         && OEM_isFirstCap(pme->m_pText) )
    {
       // change to the fist cap mode
       pme->m_nCurrInputMode = OEM_MODE_ZI_MT_SPANISH;
       OEM_SetInputMode((CTextCtl *)pme);
       
       // set flag
       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
       
       // save the Cursor Pos when Auto Set to first cap.
       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
    }
#endif // FEATURE_T9_MT_SPANISH
#ifdef FEATURE_ZI_MT_FRENCH
    // if MultitapCapsState have been change to all small in OEMText,
    //then change input mode
    if ( OEM_MODE_ZI_MT_FRENCH == pme->m_nCurrInputMode
         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
    {
       // change to the sll small mode
       pme->m_nCurrInputMode = OEM_MODE_ZI_MT_FRENCH_LOW;
       OEM_SetInputMode((CTextCtl *)pme);
    }
    
    // if it have been Auto set to first cap and move the cursor then 
    //change to all small mode
    else if ( ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState 
           || MULTITAP_USER_DENY_AUTOSET == pme->m_eAutoState )
         && OEM_TextGetSelEnd(pme->m_pText) != pme->m_nAutoSetCurPos 
         && OEM_TextGetSelEnd(pme->m_pText) == OEM_TextGetCursorPos(pme->m_pText) )
    {
        if ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState )
        {
            // change to the all small mode
            pme->m_nCurrInputMode = OEM_MODE_ZI_MT_FRENCH_LOW;
            OEM_SetInputMode(pme);
        }
        
        pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
    }

    // if the cursor is in Right position then change to FirstCap.
    //it will be checked in timer
    else if ( ( OEM_MODE_ZI_MT_FRENCH_LOW == pme->m_nCurrInputMode
           ||OEM_MODE_ZI_MT_FRENCH_UP == pme->m_nCurrInputMode )
         && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState 
         && OEM_isFirstCap(pme->m_pText) )
    {
       // change to the fist cap mode
       pme->m_nCurrInputMode = OEM_MODE_ZI_MT_FRENCH;
       OEM_SetInputMode((CTextCtl *)pme);
       
       // set flag
       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
       
       // save the Cursor Pos when Auto Set to first cap.
       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
    }
#endif // FEATURE_ZI_MT_FRENCH
	#else
#ifdef FEATURE_T9_MT_SPANISH
    // if MultitapCapsState have been change to all small in OEMText,
    //then change input mode
    if ( OEM_MODE_T9_MT_SPANISH == pme->m_nCurrInputMode
         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
    {
       // change to the sll small mode
       pme->m_nCurrInputMode = OEM_MODE_T9_MT_SPANISH_LOW;
       OEM_SetInputMode((CTextCtl *)pme);
    }
    
    // if it have been Auto set to first cap and move the cursor then 
    //change to all small mode
    else if ( ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState 
           || MULTITAP_USER_DENY_AUTOSET == pme->m_eAutoState )
         && OEM_TextGetSelEnd(pme->m_pText) != pme->m_nAutoSetCurPos 
         && OEM_TextGetSelEnd(pme->m_pText) == OEM_TextGetCursorPos(pme->m_pText) )
    {
        if ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState )
        {
            // change to the all small mode
            pme->m_nCurrInputMode = OEM_MODE_T9_MT_SPANISH_LOW;
            OEM_SetInputMode(pme);
        }
        
        pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
    }

    // if the cursor is in Right position then change to FirstCap.
    //it will be checked in timer
    else if ( ( OEM_MODE_T9_MT_SPANISH_LOW == pme->m_nCurrInputMode
           ||OEM_MODE_T9_MT_SPANISH_UP == pme->m_nCurrInputMode )
         && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState 
         && OEM_isFirstCap(pme->m_pText) )
    {
       // change to the fist cap mode
       pme->m_nCurrInputMode = OEM_MODE_T9_MT_SPANISH;
       OEM_SetInputMode((CTextCtl *)pme);
       
       // set flag
       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
       
       // save the Cursor Pos when Auto Set to first cap.
       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
    }
#endif // FEATURE_T9_MT_SPANISH
#ifdef FEATURE_T9_MT_FRENCH
    // if MultitapCapsState have been change to all small in OEMText,
    //then change input mode
    if ( OEM_MODE_T9_MT_FRENCH == pme->m_nCurrInputMode
         && MULTITAP_ALL_SMALL == OEM_TextGetMultiCaps(pme->m_pText) )
    {
       // change to the sll small mode
       pme->m_nCurrInputMode = OEM_MODE_T9_MT_FRENCH_LOW;
       OEM_SetInputMode((CTextCtl *)pme);
    }
    
    // if it have been Auto set to first cap and move the cursor then 
    //change to all small mode
    else if ( ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState 
           || MULTITAP_USER_DENY_AUTOSET == pme->m_eAutoState )
         && OEM_TextGetSelEnd(pme->m_pText) != pme->m_nAutoSetCurPos 
         && OEM_TextGetSelEnd(pme->m_pText) == OEM_TextGetCursorPos(pme->m_pText) )
    {
        if ( MULTITAP_AUTOSET_FIRSTCAP == pme->m_eAutoState )
        {
            // change to the all small mode
            pme->m_nCurrInputMode = OEM_MODE_T9_MT_FRENCH_LOW;
            OEM_SetInputMode(pme);
        }
        
        pme->m_eAutoState = MULTITAP_MAX_AUTO_STATE;
    }

    // if the cursor is in Right position then change to FirstCap.
    //it will be checked in timer
    else if ( ( OEM_MODE_T9_MT_FRENCH_LOW == pme->m_nCurrInputMode
           ||OEM_MODE_T9_MT_FRENCH_UP == pme->m_nCurrInputMode )
         && MULTITAP_MAX_AUTO_STATE == pme->m_eAutoState 
         && OEM_isFirstCap(pme->m_pText) )
    {
       // change to the fist cap mode
       pme->m_nCurrInputMode = OEM_MODE_T9_MT_FRENCH;
       OEM_SetInputMode((CTextCtl *)pme);
       
       // set flag
       pme->m_eAutoState = MULTITAP_AUTOSET_FIRSTCAP;
       
       // save the Cursor Pos when Auto Set to first cap.
       pme->m_nAutoSetCurPos = OEM_TextGetSelEnd(pme->m_pText);
    }
#endif // FEATURE_T9_MT_FRENCH
#endif
    (void) ISHELL_SetTimer((IShell *) pme->m_pIShell,
                            MULTITAP_TIMEOUT,
                            TextCtl_3STYLE_MultitapTimer,
                            pme);
                            
}

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
/*=================================================================
Function: TextCtl_QuickTimer

Description:
   timer for checking the conversion between AllSmall mode and FirstCap mode

Parameter(s):

Return Value:

Comments: none

Side Effects: none

=================================================================*/
static void TextCtl_QuickTimer(void *pUser)
{
    register CTextCtl *pme = (CTextCtl *) pUser;    
    if(EVT_KEY == pme->m_quickkeyeCode)
    {
        pme->m_quickkey = TRUE;         
    }
    else if(EVT_KEY_HELD == pme->m_quickkeyeCode)
    {
        pme->m_quickheldkey = TRUE;           
    }    
}
#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE

/*=============================================================================
FUNCTION: OEM_TextQuerySymbols

DESCRIPTION:

PARAMETERS:
   *pszOut:
   size:

RETURN VALUE:
   uint16:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
#ifndef FEATURE_CARRIER_CHINA_TELCOM
uint16 OEM_TextQuerySymbols(CTextCtl *pme,AECHAR *pszOut, uint16 size)
{
   MSG_FATAL("pme->m_nCurrInputMode=%d",pme->m_nCurrInputMode,0,0);

#ifdef FEATURE_LANG_THAI
#ifdef FEATURE_VERSION_C01
					if ((pme->m_nCurrInputMode == OEM_MODE_T9_MT_THAI) && !pme->m_isAvk1)
#elif defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_VG68)
					if ((pme->m_nCurrInputMode == OEM_MODE_T9_MT_THAI))
#else
                    if ((pme->m_nCurrInputMode == OEM_MODE_T9_RAPID_THAI))
#endif
                    {
							if (!pszOut || size < (sizeof(sszSymbolListTH)/sizeof(sszSymbolListTH[0])))
      								return(0);

   							MEMCPY(pszOut, sszSymbolListTH, sizeof(sszSymbolListTH));

  							return(sizeof(sszSymbolListTH)/sizeof(sszSymbolListTH[0])) - 1;
					}
					else
                       
#endif
				   {
   							if (!pszOut || size < (sizeof(sszSymbolList)/sizeof(sszSymbolList[0])))
      								return(0);

   							MEMCPY(pszOut, sszSymbolList, sizeof(sszSymbolList));

  							return(sizeof(sszSymbolList)/sizeof(sszSymbolList[0])) - 1;
				  }
						
				
}
#else
static uint16 TextCtl_QuerySymbols(CTextCtl *pme, AECHAR *pszOut, uint16 size) 
{
    MSG_FATAL("TextCtl_QuerySymbols Start",0,0,0);
#if 0   //modi by yangdecai  2010/06/24
   if ((pme->m_nCurrInputMode == OEM_MODE_T9_PINYIN)||(pme->m_nCurrInputMode == OEM_MODE_T9_STROKE))
   {
       if (!pszOut || size < (sizeof(sszSymbolListCn)/sizeof(sszSymbolListCn[0])))
          return(0);

       MEMCPY(pszOut, sszSymbolListCn, sizeof(sszSymbolListCn));

       return(sizeof(sszSymbolListCn)/sizeof(sszSymbolListCn[0])) - 1; 
   }
   else
   {
   #endif   
       if (!pszOut || size < (sizeof(sszSymbolListEn)/sizeof(sszSymbolListEn[0])))
          return(0);

       MEMCPY(pszOut, sszSymbolListEn, sizeof(sszSymbolListEn));

       return(sizeof(sszSymbolListEn)/sizeof(sszSymbolListEn[0])) - 1;     
   //}
}
#endif // FEATURE_CARRIER_CHINA_TELCOM  
/*=============================================================================
FUNCTION: TextCtl_Oemmode_Textmode

DESCRIPTION:

PARAMETERS:
   *pszOut:
   size:

RETURN VALUE:
   uint16:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int TextCtl_Oemmode_Textmode(byte oeminputmode)
{
    AEETextInputMode  wMode = TEXT_MODE_MULTITAP;

    switch (oeminputmode)
    {   //输入模式
        case OEM_MODE_NUMBERS:
            wMode = TEXT_MODE_NUMBERS;//数字输入模式
            break; 
#ifdef FEATURE_MT_ENGLISH_UP
        case OEM_MODE_ENGLISH_UP:
            wMode = TEXT_MODE_MULTITAP_UP;
            break; 
#endif
#ifdef FEATURE_MT_ENGLISH_LOW
        case OEM_MODE_ENGLISH_LOW:
            wMode = TEXT_MODE_MULTITAP_LOW;
            break; 
#endif
#ifdef FEATURE_MT_ENGLISH_CAPLOW
        case OEM_MODE_ENGLISH_CAPLOW:
            wMode = TEXT_MODE_MULTITAP;
            break; 
#endif
#ifdef FEATURE_USES_ZI
#ifdef FEATURE_ZI_MT_ENGLISH
        case OEM_MODE_ZI_MT_ENGLISH:
            wMode = TEXT_MODE_MULTITAP;//首字母大写字母输入模式          
            break;            
            
        case OEM_MODE_ZI_MT_ENGLISH_LOW:
            wMode = TEXT_MODE_MULTITAP;//小写字母输入模式          
            break;
            
        case OEM_MODE_ZI_MT_ENGLISH_UP:
            wMode = AEE_TM_LETTERS;//大写字母输入模式          
            break;            
#endif //FEATURE_ZI_MT_ENGLISH
#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH  //add by yangdecai   2010-09-09
		case TEXT_MODE_ZI_CAP_LOWER_ENGLISH:
			wMode = AEE_TM_CAPLOWER;
			break;
#endif
#ifdef FEATURE_MYANMAR_INPUT_MOD
		case TEXT_MODE_MYANMAR:
			wMode = AEE_TM_MYANMAR;   //add by yangdecai   2010-12-23
			break;
#endif

#ifdef FEATURE_MT_MYANMRA
		case TEXT_MODE_MT_MYANMAR:
			wMode = AEE_TM_MYANMAR_R;   //add by yangdecai   2011-12-23
			break;
#endif

#ifdef FEATURE_T9_MT_RUSSIAN
        case TEXT_MODE_MT_RUSSIAN:
            wMode = AEE_TM_RUSSIAN;
            break;
#endif

#ifdef FEATURE_T9_RAPID_RUSSIAN
        case TEXT_MODE_RAPID_RUSSIAN:
            wMode = AEE_TM_RUSSIAN_R;
            break;
#endif


#ifdef FEATURE_ZI_RAPID_ENGLISH
        case OEM_MODE_ZI_RAPID_ENGLISH:
            wMode = TEXT_MODE_ZI_RAPID_ENGLISH;          
            break;

        case OEM_MODE_ZI_RAPID_ENGLISH_LOW:
            wMode = TEXT_MODE_ZI_RAPID_ENGLISH;            
            break;

        case OEM_MODE_ZI_RAPID_ENGLISH_UP:
            wMode = TEXT_MODE_ZI_RAPID_ENGLISH;            
            break;            
#endif //FEATURE_MODE_ZI_RAPID_ENGLISH

#ifdef FEATURE_ZI_PINYIN
        case OEM_MODE_ZI_PINYIN:
            wMode = TEXT_MODE_ZI_PINYIN;
            break;
#endif //FEATURE_ZI_PINYIN

#ifdef FEATURE_ZI_STROKE
        case OEM_MODE_ZI_STROKE:
            wMode = TEXT_MODE_ZI_STROKE;
            break;
#endif //FEATURE_ZI_STROKE

#ifdef FEATURE_ZI_ZHUYIN
        case OEM_MODE_ZI_ZHUYIN:
            wMode = TEXT_MODE_ZI_ZHUYIN;
            break;
#endif //FEATURE_ZI_ZHUYIN

#ifdef FEATURE_ZI_MT_ARABIC
        case OEM_MODE_ZI_MT_ARABIC:
            wMode = TEXT_MODE_ZI_MT_ARABIC;
            break;
#endif //FEATURE_ZI_MT_ARABIC

#ifdef FEATURE_ZI_RAPID_ARABIC
        case OEM_MODE_ZI_RAPID_ARABIC:
            wMode = TEXT_MODE_ZI_RAPID_ARABIC;
            break;
#endif //FEATURE_ZI_RAPID_ARABIC

#ifdef FEATURE_ZI_MT_HEBREW
        case OEM_MODE_ZI_MT_HEBREW:   
            wMode = TEXT_MODE_ZI_MT_HEBREW;
            break;
#endif //FEATURE_ZI_MT_HEBREW

#ifdef FEATURE_ZI_RAPID_HEBREW
        case OEM_MODE_T9_RAPID_HEBREW:
            wMode = TEXT_MODE_ZI_RAPID_HEBREW;
            break;
#endif //FEATURE_ZI_RAPID_HEBREW

#ifdef FEATURE_ZI_MT_HINDI
        case OEM_MODE_ZI_MT_HINDI:
            wMode = TEXT_MODE_ZI_MT_HINDI;
            break;
#endif //FEATURE_ZI_MT_HINDI

#ifdef FEATURE_ZI_RAPID_HINDI
        case OEM_MODE_ZI_RAPID_HINDI:
            wMode = TEXT_MODE_ZI_RAPID_HINDI;
            break;
#endif //FEATURE_ZI_RAPID_HINDI

#ifdef FEATURE_ZI_MT_THAI
        case OEM_MODE_ZI_MT_THAI:
            wMode = TEXT_MODE_ZI_MT_THAI;
            break;
#endif //FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_RAPID_THAI
        case OEM_MODE_ZI_RAPID_THAI:
            wMode = TEXT_MODE_ZI_RAPID_THAI;
            break;
#endif //FEATURE_ZI_RAPID_THAI

#ifdef FEATURE_ZI_MT_SPANISH
        case OEM_MODE_ZI_MT_SPANISH:
            wMode = TEXT_MODE_ZI_MT_SPANISH;
            break;

         case OEM_MODE_ZI_MT_SPANISH_LOW:
            wMode = TEXT_MODE_ZI_MT_SPANISH;
            
            break;
         case OEM_MODE_ZI_MT_SPANISH_UP:
            wMode = TEXT_MODE_ZI_MT_SPANISH;
            break;
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_RAPID_SPANISH
        case OEM_MODE_ZI_RAPID_SPANISH:
            wMode = TEXT_MODE_ZI_RAPID_SPANISH;
            break;
            
         case OEM_MODE_ZI_RAPID_SPANISH_LOW:
            wMode = TEXT_MODE_ZI_RAPID_SPANISH;
            break;
            
         case OEM_MODE_ZI_RAPID_SPANISH_UP:
            wMode = TEXT_MODE_ZI_RAPID_SPANISH;
            break;
#endif //FEATURE_ZI_RAPID_SPANISH

#ifdef FEATURE_ZI_MT_PORTUGUESE
        case OEM_MODE_ZI_MT_PORTUGUESE:
            wMode = TEXT_MODE_ZI_MT_PORTUGUESE;
            break;
#endif //FEATURE_ZI_MT_PORTUGUESE

#ifdef FEATURE_ZI_RAPID_PORTUGUESE
        case OEM_MODE_ZI_RAPID_PORTUGUESE:
            wMode = TEXT_MODE_ZI_RAPID_PORTUGUESE;
            break;
#endif //FEATURE_ZI_RAPID_PORTUGUESE

#ifdef FEATURE_ZI_MT_INDONESIAN
        case OEM_MODE_ZI_MT_INDONESIAN:
            wMode = TEXT_MODE_ZI_MT_INDONESIAN;
            break;
#endif //FEATURE_ZI_MT_INDONESIAN

#ifdef FEATURE_ZI_RAPID_INDONESIAN
        case OEM_MODE_ZI_RAPID_INDONESIAN:
            wMode = TEXT_MODE_ZI_RAPID_INDONESIAN;
            break;
#endif //FEATURE_ZI_RAPID_INDONESIAN

#ifdef FEATURE_ZI_MT_VIETNAMESE
        case OEM_MODE_ZI_MT_VIETNAMESE:
            wMode = TEXT_MODE_ZI_MT_VIETNAMESE;
            break;
#endif //FEATURE_ZI_MT_VIETNAMESE

#ifdef FEATURE_ZI_RAPID_VIETNAMESE
        case OEM_MODE_ZI_RAPID_VIETNAMESE:
            wMode = TEXT_MODE_ZI_RAPID_VIETNAMESE;
            break;
#endif //FEATURE_ZI_RAPID_VIETNAMESE

#ifdef FEATURE_ZI_MT_FRENCH
        case OEM_MODE_ZI_MT_FRENCH:
            wMode = TEXT_MODE_ZI_MT_FRENCH;
            break;

         case OEM_MODE_ZI_MT_FRENCH_LOW:
            wMode = TEXT_MODE_ZI_MT_FRENCH;
            break;

         case OEM_MODE_ZI_MT_FRENCH_UP:
            wMode = TEXT_MODE_ZI_MT_FRENCH;
            break;
#endif //FEATURE_ZI_MT_FRENCH

#ifdef FEATURE_ZI_RAPID_FRENCH
        case OEM_MODE_ZI_RAPID_FRENCH:
            wMode = TEXT_MODE_ZI_RAPID_FRENCH;
            break;

         case OEM_MODE_ZI_RAPID_FRENCH_LOW:
            wMode = TEXT_MODE_ZI_RAPID_FRENCH;
            break;

         case OEM_MODE_ZI_RAPID_FRENCH_UP:
            wMode = TEXT_MODE_ZI_RAPID_FRENCH;
            break;
#endif //FEATURE_ZI_RAPID_FRENCH

#ifdef FEATURE_ZI_JAPANESE
        case OEM_MODE_ZI_JAPANESE:
            wMode = TEXT_MODE_ZI_JAPANESE;
            break;
#endif //FEATURE_ZI_JAPANESE

#ifdef FEATURE_ZI_KOREAN
        case OEM_MODE_ZI_KOREAN:
            wMode = TEXT_MODE_ZI_KOREAN;
            break;
#endif //FEATURE_ZI_KOREAN

#ifdef FEATURE_ZI_ITALIAN
        case OEM_MODE_ZI_ITALIAN:
            wMode = TEXT_MODE_ZI_ITALIAN;
            break;
#endif //FEATURE_ZI_ITALIAN

#else
#ifdef FEATURE_T9_MT_ENGLISH
        case OEM_MODE_T9_MT_ENGLISH:
            wMode = TEXT_MODE_MULTITAP;//首字母大写字母输入模式          
            break;            
            
        case OEM_MODE_T9_MT_ENGLISH_LOW:
            wMode = TEXT_MODE_MULTITAP;//小写字母输入模式          
            break;
            
        case OEM_MODE_T9_MT_ENGLISH_UP:
            wMode = AEE_TM_LETTERS;//大写字母输入模式          
            break;            
#endif //FEATURE_T9_MT_ENGLISH
#ifdef FEATURE_T9_CAP_LOWER_ENGLISH  //add by yangdecai   2010-09-09
		case TEXT_MODE_T9_CAP_LOWER_ENGLISH:
			wMode = AEE_TM_CAPLOWER;
			break;
#endif
#ifdef FEATURE_MYANMAR_INPUT_MOD
		case TEXT_MODE_MYANMAR:
			wMode = AEE_TM_MYANMAR;   //add by yangdecai   2010-12-23
			break;
#endif

#ifdef FEATURE_T9_MT_RUSSIAN
        case TEXT_MODE_MT_RUSSIAN:
            wMode = AEE_TM_RUSSIAN;
            break;
#endif

#ifdef FEATURE_T9_RAPID_RUSSIAN
        case TEXT_MODE_RAPID_RUSSIAN:
            wMode = AEE_TM_RUSSIAN_R;
            break;
#endif


#ifdef FEATURE_T9_RAPID_ENGLISH
        case OEM_MODE_T9_RAPID_ENGLISH:
            wMode = TEXT_MODE_T9_RAPID_ENGLISH;          
            break;

        case OEM_MODE_T9_RAPID_ENGLISH_LOW:
            wMode = TEXT_MODE_T9_RAPID_ENGLISH;            
            break;

        case OEM_MODE_T9_RAPID_ENGLISH_UP:
            wMode = TEXT_MODE_T9_RAPID_ENGLISH;            
            break;            
#endif //FEATURE_MODE_T9_RAPID_ENGLISH

#ifdef FEATURE_T9_PINYIN
        case OEM_MODE_T9_PINYIN:
            wMode = TEXT_MODE_T9_PINYIN;
            break;
#endif //FEATURE_T9_PINYIN

#ifdef FEATURE_T9_STROKE
        case OEM_MODE_T9_STROKE:
            wMode = TEXT_MODE_T9_STROKE;
            break;
#endif //FEATURE_T9_STROKE

#ifdef FEATURE_T9_ZHUYIN
        case OEM_MODE_T9_ZHUYIN:
            wMode = TEXT_MODE_T9_ZHUYIN;
            break;
#endif //FEATURE_T9_ZHUYIN

#ifdef FEATURE_T9_MT_ARABIC
        case OEM_MODE_T9_MT_ARABIC:
            wMode = TEXT_MODE_T9_MT_ARABIC;
            break;
#endif //FEATURE_T9_MT_ARABIC

#ifdef FEATURE_T9_RAPID_ARABIC
        case OEM_MODE_T9_RAPID_ARABIC:
            wMode = TEXT_MODE_T9_RAPID_ARABIC;
            break;
#endif //FEATURE_T9_RAPID_ARABIC

#ifdef FEATURE_T9_MT_HEBREW
        case OEM_MODE_T9_MT_HEBREW:   
            wMode = TEXT_MODE_T9_MT_HEBREW;
            break;
#endif //FEATURE_T9_MT_HEBREW

#ifdef FEATURE_T9_RAPID_HEBREW
        case OEM_MODE_T9_RAPID_HEBREW:
            wMode = TEXT_MODE_T9_RAPID_HEBREW;
            break;
#endif //FEATURE_T9_RAPID_HEBREW

#ifdef FEATURE_T9_MT_HINDI
        case OEM_MODE_T9_MT_HINDI:
            wMode = TEXT_MODE_T9_MT_HINDI;
            break;
#endif //FEATURE_T9_MT_HINDI

#ifdef FEATURE_T9_RAPID_HINDI
        case OEM_MODE_T9_RAPID_HINDI:
            wMode = TEXT_MODE_T9_RAPID_HINDI;
            break;
#endif //FEATURE_T9_RAPID_HINDI

#ifdef FEATURE_T9_MT_THAI
        case OEM_MODE_T9_MT_THAI:
            wMode = TEXT_MODE_T9_MT_THAI;
            break;
#endif //FEATURE_T9_MT_THAI

#ifdef FEATURE_T9_RAPID_THAI
        case OEM_MODE_T9_RAPID_THAI:
            wMode = TEXT_MODE_T9_RAPID_THAI;
            break;
#endif //FEATURE_T9_RAPID_THAI

#ifdef FEATURE_T9_MT_SPANISH
        case OEM_MODE_T9_MT_SPANISH:
            wMode = TEXT_MODE_T9_MT_SPANISH;
            break;

         case OEM_MODE_T9_MT_SPANISH_LOW:
            wMode = TEXT_MODE_T9_MT_SPANISH;
            
            break;
         case OEM_MODE_T9_MT_SPANISH_UP:
            wMode = TEXT_MODE_T9_MT_SPANISH;
            break;
#endif //FEATURE_T9_MT_SPANISH

#ifdef FEATURE_T9_RAPID_SPANISH
        case OEM_MODE_T9_RAPID_SPANISH:
            wMode = TEXT_MODE_T9_RAPID_SPANISH;
            break;
            
         case OEM_MODE_T9_RAPID_SPANISH_LOW:
            wMode = TEXT_MODE_T9_RAPID_SPANISH;
            break;
            
         case OEM_MODE_T9_RAPID_SPANISH_UP:
            wMode = TEXT_MODE_T9_RAPID_SPANISH;
            break;
#endif //FEATURE_T9_RAPID_SPANISH

#ifdef FEATURE_T9_MT_PORTUGUESE
        case OEM_MODE_T9_MT_PORTUGUESE:
            wMode = TEXT_MODE_T9_MT_PORTUGUESE;
            break;
#endif //FEATURE_T9_MT_PORTUGUESE

#ifdef FEATURE_T9_RAPID_PORTUGUESE
        case OEM_MODE_T9_RAPID_PORTUGUESE:
            wMode = TEXT_MODE_T9_RAPID_PORTUGUESE;
            break;
#endif //FEATURE_T9_RAPID_PORTUGUESE

#ifdef FEATURE_T9_MT_INDONESIAN
        case OEM_MODE_T9_MT_INDONESIAN:
            wMode = TEXT_MODE_T9_MT_INDONESIAN;
            break;
#endif //FEATURE_T9_MT_INDONESIAN

#ifdef FEATURE_T9_RAPID_INDONESIAN
        case OEM_MODE_T9_RAPID_INDONESIAN:
            wMode = TEXT_MODE_T9_RAPID_INDONESIAN;
            break;
#endif //FEATURE_T9_RAPID_INDONESIAN

#ifdef FEATURE_T9_MT_VIETNAMESE
        case OEM_MODE_T9_MT_VIETNAMESE:
            wMode = TEXT_MODE_T9_MT_VIETNAMESE;
            break;
#endif //FEATURE_T9_MT_VIETNAMESE

#ifdef FEATURE_T9_RAPID_VIETNAMESE
        case OEM_MODE_T9_RAPID_VIETNAMESE:
            wMode = TEXT_MODE_T9_RAPID_VIETNAMESE;
            break;
#endif //FEATURE_T9_RAPID_VIETNAMESE

#ifdef FEATURE_T9_MT_FRENCH
        case OEM_MODE_T9_MT_FRENCH:
            wMode = TEXT_MODE_T9_MT_FRENCH;
            break;

         case OEM_MODE_T9_MT_FRENCH_LOW:
            wMode = TEXT_MODE_T9_MT_FRENCH;
            break;

         case OEM_MODE_T9_MT_FRENCH_UP:
            wMode = TEXT_MODE_T9_MT_FRENCH;
            break;
#endif //FEATURE_T9_MT_FRENCH

#ifdef FEATURE_T9_RAPID_FRENCH
        case OEM_MODE_T9_RAPID_FRENCH:
            wMode = TEXT_MODE_T9_RAPID_FRENCH;
            break;

         case OEM_MODE_T9_RAPID_FRENCH_LOW:
            wMode = TEXT_MODE_T9_RAPID_FRENCH;
            break;

         case OEM_MODE_T9_RAPID_FRENCH_UP:
            wMode = TEXT_MODE_T9_RAPID_FRENCH;
            break;
#endif //FEATURE_T9_RAPID_FRENCH

#ifdef FEATURE_T9_JAPANESE
        case OEM_MODE_T9_JAPANESE:
            wMode = TEXT_MODE_T9_JAPANESE;
            break;
#endif //FEATURE_T9_JAPANESE

#ifdef FEATURE_T9_KOREAN
        case OEM_MODE_T9_KOREAN:
            wMode = TEXT_MODE_T9_KOREAN;
            break;
#endif //FEATURE_T9_KOREAN

#ifdef FEATURE_T9_ITALIAN
        case OEM_MODE_T9_ITALIAN:
            wMode = TEXT_MODE_T9_ITALIAN;
            break;
#endif //FEATURE_T9_ITALIAN
#endif
        default:
            wMode = TEXT_MODE_NUMBERS;
            break;
        }

    return wMode;

}


static void TextCtl_DrawBackGround(CTextCtl * pme, AEERect *pRect)
{
    if(pme->m_pImageBg == NULL)
    {
        if(pme->m_nBgImgResID != 0 && STRLEN(pme->m_strBgImgResFile) != 0)
        {
        	#ifndef FEATURE_LOW_MEM_BIGFONT
            pme->m_pImageBg = ISHELL_LoadResImage(pme->m_pIShell, pme->m_strBgImgResFile, pme->m_nBgImgResID);
			#endif
        }
        else
        {
            
			#ifdef FEATURE_VERSION_C316
			if(pme->m_dwProps & TP_GRAPHIC_BGBLUE)
			{
			    #ifndef FEATURE_VERSION_K232_Y105A
				pme->m_pImageBg = ISHELL_LoadResImage(pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_BACKGROUD_BLUE);
                #endif
			}
			else
			{
				pme->m_pImageBg = ISHELL_LoadResImage(pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
			}
			#else
			#ifndef FEATURE_LOW_MEM_BIGFONT
			pme->m_pImageBg = ISHELL_LoadResImage(pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
			#endif
			#endif
        }
        OEM_TextSetBackGround(pme->m_pText, pme->m_pImageBg);
    }
    #ifdef FEATURE_VERSION_K232_Y105A
    if(pme->m_dwProps & TP_GRAPHIC_BGBLUE)
	{
	    Appscommon_ResetBackground(pme->m_pIDisplay, pme->m_pImageBg, RGB_WHITE, pRect, 0, 0);
        OEM_Text_BLUEBackGround(pme->m_pText, TRUE);
    }
    else
    {
        Appscommon_ResetBackground(pme->m_pIDisplay, pme->m_pImageBg, APPSCOMMON_BG_COLOR, pRect, 0, 0);
    }
    #else
    Appscommon_ResetBackground(pme->m_pIDisplay, pme->m_pImageBg, APPSCOMMON_BG_COLOR, pRect, 0, 0);
    #endif
}
//end added

static void CTextCtl_SetBackGround(ITextCtl * po, char *pstrImgResFile, uint16 nImgResID)
{
    CTextCtl * pme = (CTextCtl *) po;
    
    if(pstrImgResFile != NULL && STRLEN(pstrImgResFile) != 0 && nImgResID != 0)
    {
        pme->m_nBgImgResID = nImgResID;
        MEMSET(pme->m_strBgImgResFile, 0, sizeof(pme->m_strBgImgResFile));
        STRCPY(pme->m_strBgImgResFile, pstrImgResFile);

        if(pme->m_pImageBg != NULL)
        {
            IImage_Release(pme->m_pImageBg);
            pme->m_pImageBg = NULL;
        }
        pme->m_pImageBg = ISHELL_LoadResImage(pme->m_pIShell, pme->m_strBgImgResFile, pme->m_nBgImgResID);
        OEM_TextSetBackGround(pme->m_pText, pme->m_pImageBg);
    }
}

//add by ydc
/*===========================================================================
Function: TextCtl_HandlePens()

Description: This function handles pen events and dispatches to other handlers as needed.

Prototype:
static boolean TextCtl_HandlePens(CTextCtl * pme, AEEEvent evtCode, uint16 wParam, int16 wXPos, int16 wYPos)

Parameter(s):
pme:        Pointer to ITextCtl Interface object
evtcode :   Code for pen event
wParam :    uint16 lower 16 bits of time of pen event occurrence
wXPos :     X Coordinate of the pen event
wYPos :     Y Coordinate of the pen event

Return Value: 
TRUE :   Pen event handled
FALSE :  Pen event not handled

Comments: See the documentation for expectations of pen handling.

Side Effects: 

See Also: none

===========================================================================*/
static boolean TextCtl_HandlePens(CTextCtl * pme, AEEEvent evtCode, uint16 wParam, int16 wXPos, int16 wYPos)
{
	switch( evtCode ){
	  case EVT_PEN_STALE_MOVE:
	  case EVT_PEN_MOVE:
		  {
			  if( (pme->m_pSoftKey && IMENUCTL_IsActive(pme->m_pSoftKey))
				  || pme->m_pModeMenu || pme->m_bShowSyms )
			  { // For now, unhandled!
				  return FALSE;
			  }
			  // See if they handle it
			  return OEM_HandlePenEvt(pme->m_pText, evtCode, wParam, wXPos, wYPos);
		  }
		  //return FALSE;
	  case EVT_PEN_DOWN :
		  {
			  AEERect rctSK;
			  MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));
			  // Our TextCtl Object acts as though it were a dialog in managing it's attached
			  // Softkey meny, managing Symbols mode, and managing the mode menu, so mimic the 
			  // Dialog behavior in those instances. Other Pen Events in the text field are 
			  // uninteresting here as the text measurement and scrolling characteristics are 
			  // in the OEMText layer
			  if( pme->m_pModeMenu ){
				  (void)IMENUCTL_HandleEvent(pme->m_pModeMenu,evtCode,wParam,AEE_SET_XY(wXPos,wYPos));
				  return TRUE;
			  }

			  if(pme->m_bShowSyms){
				  return TextCtl_SymbolHandlePen(pme, evtCode, wXPos, wYPos);
			  }

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
					  (void)IMENUCTL_HandleEvent(pme->m_pSoftKey, evtCode, wParam, (uint32)AEE_SET_XY((int)wXPos,(int)wYPos));
					  return TRUE;
				  }else if( bActive && bInSKRect ){
					  return FALSE; // Must return false, so handler can give it to SK for action!
				  }else if( bActive && PT_IN_RECT(wXPos, wYPos, pme->m_rc) && !pme->m_bActive/* redundant safety */ ){ 
					  // Overlapped SK accounted for at first if()
					  TextCtl_ReturnToEditMode(pme, AEE_TM_CURRENT);
					  return TRUE;
				  }
			  }
			  //         if( pme->m_pModeMenu ){
			  //            (void)IMENUCTL_HandleEvent(pme->m_pModeMenu,evtCode,wParam,AEE_SET_XY(wXPos,wYPos));
			  //            return TRUE;
			  //         } 
			  //         if(pme->m_bShowSyms){
			  //          ERR("TextCtl_SymbolHandlePen", 0, 0, 0);
			  //            return TextCtl_SymbolHandlePen(pme, evtCode, wXPos, wYPos);
			  //         }

			  if( !pme->m_bActive ){
				  return FALSE;
			  }

			  // Let them decide if it is in their rect or CAPTURED... still dragging outside
			  // the rect and selecting text along the way, yay!
			  return OEM_HandlePenEvt(pme->m_pText, evtCode, wParam, wXPos, wYPos);
		  }
		  //return FALSE;

	  case EVT_PEN_UP:
		  {
			  boolean bRet = FALSE;
			  // Softkey should not handle this event directly...
			  // Pen Down activates it and feeds it the down event.
			  // At that point the menu is capable to respond on it's own to pen up
			  // This sort of mimics IDialog which is what an ITextCtl becomes when
			  // a SK is set to it (or when the mode menu or symbols box arises)
			  if( !(pme->m_pSoftKey && IMENUCTL_IsActive(pme->m_pSoftKey)) ){
				  if( pme->m_pModeMenu ){
					  if( IMENUCTL_HandleEvent(pme->m_pModeMenu,evtCode,wParam,AEE_SET_XY(wXPos,wYPos)) ){
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
					  bRet = OEM_HandlePenEvt(pme->m_pText, evtCode, wParam, wXPos, wYPos);
				  }
			  }
			  MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));
			  return bRet;
		  }
	  default:
		  break;
	}

	return FALSE;
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
   case EVT_PEN_MOVE:
	   // Not handled, yet! Later I want to have a pendown on item or arrow give visual 
	   // feedback by color inversion until mouse moved off the region of item or arrow
	   break; 
   case EVT_PEN_DOWN :
	   {
		   pme->m_ptTracker.cbHit = TextCtl_SymbolPagePenHitItem(pme, wXPos, wYPos, &pme->m_ptTracker.wData);
	   }
	   return TRUE; // Full screen, always in our rect!
   case EVT_PEN_UP:
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
					   if( wData == pme->m_ptTracker.wData )
					   {
						   if (pme->m_nSymbolPage == MAX_SYMBOL_PAGE)
						   {
							   if(wData < 7)
							   {
								   TextCtl_CloseNetSymbolDialog(pme, (AECHAR*)sszNetSymbolList[wData]);
							   }
							   else
							   {
								   TextCtl_CloseNetSymbolDialog(pme, NULL);
							   }
						   }
						   else
						   {
							   TextCtl_CloseSymbolDialog(pme, TextCtl_GetSymChar(pme,wData));
						   }
						   bRet = TRUE;
					   }
				   }
			   }
		   }
		   return bRet;
	   }
	   //break;
   default:
	   break;
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
			rctInspect.dx  = IDISPLAY_MeasureText(pd,AEE_FONT_BOLD,szBuff);
			if( wXPos >= rctInspect.x && wXPos < (rctInspect.x + rctInspect.dx) ){
				// Hit Up arrow!
				cbReturn = PTRCK_HIT_ABOVE | PTRCK_HIT_SCRL;
			}else{
				szBuff[0] = (AECHAR)'v';
				szBuff[1] = (AECHAR)0;
				rctInspect.x += rctInspect.dx;
				rctInspect.dx  = IDISPLAY_MeasureText(pd,AEE_FONT_BOLD,szBuff);
				if( wXPos >= rctInspect.x && wXPos < (rctInspect.x + rctInspect.dx) ){
					// Hit Down arrow!
					cbReturn = PTRCK_HIT_BELOW | PTRCK_HIT_SCRL;
				}
			}
	}else{
		AECHAR         szBuff[64];
		//             *       pszMeasure;
		int            cyAdd, cxAdd;
		IDisplay *     tmppd = pme->m_pIDisplay;
		//      int            i, nWidth = 0, nPage = pme->m_nSymbolPage;
		// Haven't hit our page 'scrollbar'
		TextCtl_GetSymbDlgInfo(pme, tmppd, &pme->m_bi, szBuff, sizeof(szBuff), &cxAdd, &cyAdd, &rctInspect.dx, &rctInspect.dy);
		// Be nice and make our columns the same width also, so they don't have to try so hard
		// to tap or click  | . ! and similar
		//      pszMeasure = szBuff + (nPage * 9);
		//      for( i = 0; i < 9 && *pszMeasure; i++, pszMeasure++ ){
		//         int nTmp = IDISPLAY_MeasureTextEx(tmppd, AEE_FONT_NORMAL, pszMeasure, 1, -1, NULL);
		//         if( nTmp > nWidth ){
		//            nWidth = nTmp;
		//         }
		//      } // Do not play with i iterator, it is re-used below to keep track of quantity of items

		// This represents the bounding rect of the page items, below the title and arrows
		if( wXPos >= SYMDLG_X_ITEMBASE && wYPos >= (SYMDLG_Y_ARROWPOS + rctInspect.dy) ){
			int8 wCol = -1, wRow = -1, wIter;
			for( wIter = 0; wIter < 3; wIter++ ){
				uint16 wBegin = (wIter*cxAdd) + (SYMDLG_X_ITEMBASE+SYMDLG_X_IDXPAD);
				if( wXPos >= wBegin && wXPos < (wBegin+cxAdd) ){
					wCol = wIter;
					break;
				}
				//            else if(wXPos >= (wBegin+rctInspect.dx+SYMDLG_X_IDXAPAD) && wXPos < (wBegin+(rctInspect.dx<<1)+SYMDLG_X_IDXAPAD) ){
				//               wCol = wIter;
				//               break;
				//            }
			}
			for( wIter = 0; wIter < 3; wIter++ ){
				if( wYPos >= (((wIter)*cyAdd)+rctInspect.dy) && wYPos < ((((wIter)*cyAdd)+rctInspect.dy)+rctInspect.dy) ){
					wRow = wIter;
					break;
				}
			}
			if( wRow != -1 && wCol != -1 ){
				uint16 wData = (uint16)(uint8)((wRow*3) + wCol);
				if(pme->m_nSymbolPage == MAX_SYMBOL_PAGE)
				{
					if(wData == 7)
					{
						wData = 6;
					}
				}
				if( wData < 9 ){
					cbReturn = PTRCK_HIT_ITEM;
					*pwData = wData;
				}
			}
		}
	}
	return cbReturn;
}

/*===========================================================================
Function: TextCtl_GetSymbDlgInfo()

Description: This function gets information to prepare for displaying the symbol 
dialog. It is used internally as a local method and parameters are not checked.

Prototype:
static void TextCtl_GetSymbDlgInfo(CTextCtl * pme, IDisplay * pd, AEEBitmapInfo * pbi, AECHAR * pszwBuff, uint16 wBuffSize, int * pcxAdd, int * pcyAdd, int16 * pwDx, int16 * pwDy)

Parameter(s):   
CTextCtl * pme
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
static void TextCtl_GetSymbDlgInfo(CTextCtl * pme, IDisplay * pd, AEEBitmapInfo * pbi, AECHAR * pszwBuff, uint16 wBuffSize, int * pcxAdd, int * pcyAdd, int16 * pwDx, int16 * pwDy)
{
	AECHAR sz[2];
	int16  dx, dy;
	int    nFits;
	int16  nWidth;
	int    i;
#ifndef FEATURE_CARRIER_CHINA_TELCOM 
    (void)OEM_TextQuerySymbols(pme, pszwBuff, wBuffSize);
#else
    (void)TextCtl_QuerySymbols(pme, pszwBuff, wBuffSize);
#endif

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
	if(pme->m_pSoftKey == NULL)
	{
		*pcyAdd = (int)((pbi->cy - dy - 26) / 3);
	}
	else
	{
		*pcyAdd = (int)((pbi->cy - dy) / 3);
	}
	*pcxAdd = (int)(pbi->cx / 3);

	*pwDy = dy;
}
    
/*---------------------------------- eof ---------------------------------*/

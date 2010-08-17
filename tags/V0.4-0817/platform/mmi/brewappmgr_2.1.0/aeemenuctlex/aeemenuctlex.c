/*===========================================================================
FILE:  aeemenuctlex.c

SERVICES: Provides extended supports for IMenuCtl services in AEE.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS: IMenuCtl

Copyright © 2008 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

========================================================================== */

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //brewapps/bam2/HEAD/aeemenuctlex/aeemenuctlex.c#23 $

===========================================================================*/

/*=========================================================================
INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEEInterface.h"
#include "AEEPointerHelpers.h"

#include "aeemenuctlex_priv.h"
#include "aeemenuctlutils.h"
#include "aeemenuctlex.bid"    // Applet-specific header that contains class ID for menuctl extension


//#define _MCEX_DEBUG
/*===========================================================================*/
// define AEEMENUCLT_STATIC to be static compiled aeemenuctl into app
//#define AEEMENUCLT_STATIC
/*===========================================================================*/

/*===========================================================================

DEFINITIONS

===========================================================================*/

#define __MAX(x,y)               ((x) > (y) ? (x) : (y))       // Max of X and Y
#define __MIN(x,y)               ((x) < (y) ? (x) : (y))       // Min of X and Y
#define IS_PROP_SET(dw,dwProp)   ((dw) & (dwProp))

#define IS_ALLOWING_DUPE(p)      IS_PROP_SET(p->m_dwProps,MP_ALLOW_DUPE_IDS)
#define IS_ICON_VIEW(p)          ((p)->m_cls == AEECLSID_ICONVIEWCTLEX)
#define SINGLE_FRAME(p)          IS_PROP_SET(p->m_dwProps,MP_ICON_SINGLE_FRAME)
#define IS_TRI_STATE(p)          IS_PROP_SET(p->m_dwProps,MP_TRI_STATE_IMAGE)
#define IS_ANIMATED(p)           IS_PROP_SET(p->m_dwProps,MP_ICON_ANIMATED)
#define IS_MULTI_IMAGE(p)        IS_PROP_SET(p->m_dwProps,(MP_TRI_STATE_IMAGE|MP_BI_STATE_IMAGE))
#define IS_MULTI_SEL(p)          IS_PROP_SET(p->m_dwProps,MP_MULTI_SEL)

#define IS_OD_CB(p)             ((p)->m_pfnCB)
#define IS_TITLE_OD(p)          ((p)->m_nODFlags & ODF_TITLE_OD)

#define SINGLE_CLICK_LAUNCH(p)  IS_PROP_SET(p->m_dwProps,MP_EX_SINGLE_CLICK_LAUNCH)


#define IS_SOFTKEY_MENU(p)      ((p)->m_cls == AEECLSID_SOFTKEYCTLEX || (p)->m_cls == AEECLSID_SOFTKEYCTL)
#define IS_LIST_MENU(p)         ((p)->m_cls == AEECLSID_LISTCTLEX)
#define IS_MENU(p)              ((p)->m_cls == AEECLSID_MENUCTLEX)
#define IS_ICON_VIEW(p)         ((p)->m_cls == AEECLSID_ICONVIEWCTLEX)
                             
#define MENU_IMAGE_PAD          (5)                           // Space between image and menu text
#define LIST_TITLE_PAD          (5)                           // Space between list title and text

#define AUTO_SCROLL_PIXELS      (6)                           // number of pixels to scroll during autoscroll
#define AUTO_SCROLL_START       (1000)                        // Auto Scroll - (Re)Start Delay
#define AUTO_SCROLL_TIME        (150) 

#define BORDER_FRAME_COLOR      MAKE_RGB(0, 0, 0)
/*===============================================================================

      FUNCTION DECLARATIONS

===============================================================================*/
static int           MCEX_New(IShell * pIShell, AEECLSID cls, void ** ppobj);
static uint32        AEEMENUCTL_AddRef(IMenuCtl * po);
static uint32        AEEMENUCTL_Release(IMenuCtl * po);
static boolean       AEEMENUCTL_HandleEvent(IMenuCtl * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean       AEEMENUCTL_Redraw(IMenuCtl * po);
static void          AEEMENUCTL_SetActive(IMenuCtl * po, boolean bActive);
static boolean       AEEMENUCTL_IsActive(IMenuCtl * po);
static void          AEEMENUCTL_SetRect(IMenuCtl * po, const AEERect * prc);
static void          AEEMENUCTL_GetRect(IMenuCtl * po, AEERect * prc);
static void          AEEMENUCTL_SetProperties(IMenuCtl * po, uint32 nProperties);
static uint32        AEEMENUCTL_GetProperties(IMenuCtl * po);
static void          AEEMENUCTL_Reset(IMenuCtl * po);
static boolean       AEEMENUCTL_SetTitle(IMenuCtl * po, const char * pszResFile, uint16 wResID, AECHAR * pText);
static boolean       AEEMENUCTL_AddItem(IMenuCtl * po, const char * pszResFile, uint16 wResID, uint16 nItemID, AECHAR * pText, uint32 lData);
static boolean       AEEMENUCTL_AddItemEx(IMenuCtl * po, CtlAddItem *pai);
static boolean       AEEMENUCTL_GetItemData(IMenuCtl * po, uint16 nItemID, uint32 * plData);
static boolean       AEEMENUCTL_DeleteItem(IMenuCtl * po, uint16 nItemID);
static boolean       AEEMENUCTL_DeleteAll(IMenuCtl * po);
static void          AEEMENUCTL_SetSel(IMenuCtl * po, uint16 nSelect);
static uint16        AEEMENUCTL_GetSel(IMenuCtl * po);
static void          AEEMENUCTL_EnableCommand(IMenuCtl * po, boolean bEnable);
static void          AEEMENUCTL_SetItemText(IMenuCtl * po, uint16 nItemID, const char * pszResFile, uint16 wResID, AECHAR * pText);
static void          AEEMENUCTL_SetItemTime(IMenuCtl * po, uint16 nItemID, uint16 wTime, uint16 wDuration);
static int           AEEMENUCTL_GetItemTime(IMenuCtl * po, uint16 nItemID, uint16 * pwDuration);
static void          AEEMENUCTL_SetStyle(IMenuCtl * po, AEEItemStyle * pNormal, AEEItemStyle * pSel);
static void          AEEMENUCTL_SetColors(IMenuCtl * po, AEEMenuColors * pc);
static int16         AEEMENUCTL_MoveItem(IMenuCtl * po, uint16 wID1, int nDir);

static boolean       AEEMENUCTL_SetItem(IMenuCtl * po, uint16 wID,uint16 wFlags,CtlAddItem * pai);
static int           AEEMENUCTL_GetItemCount(IMenuCtl * po);
static uint16        AEEMENUCTL_GetItemID(IMenuCtl * po, int nIdx);
static void          AEEMENUCTL_Sort(IMenuCtl * po, AEEMenuSortType t, PFNMENUSORTCOMPARE pfn, void * pUser);
static boolean       AEEMENUCTL_GetItem(IMenuCtl * po, uint16 wID, CtlAddItem * pai);

static void          AEEMENUCTL_SetSelEx(IMenuCtl * po, uint16 nItemID, boolean bSelected);
static boolean       AEEMENUCTL_EnumSelInit(IMenuCtl * po);
static uint16        AEEMENUCTL_EnumNextSel(IMenuCtl * po);

static void          AEEMENUCTL_GetStyle(IMenuCtl * pIMenuCtl,AEEItemStyle * pNormal, AEEItemStyle * pSel);
static void          AEEMENUCTL_GetColors(IMenuCtl * pIMenuCtl, AEEMenuColors  * pc);
static void          AEEMENUCTL_GetItemRect(IMenuCtl * pIMenuCtl, uint16 nIdx, AEERect * prc);
static int           AEEMENUCTL_SetOwnerDrawCB(IMenuCtl * po, PFNOWNERDRAWCB pfnCB, void * pvContext, uint16 nODFlags);


static boolean       MCEX_ReleaseObj(void ** ppObj);
static boolean       MCEX_FreeObj(void ** ppObj);
static void          MCEX_FreeMenuItem(CMenuItem** ppItem);

// this function creates an instance of aeemenuctlex object
int MCEX_MenuCtlClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod,  void **ppObj) ;

static boolean MCEX_ResolveImage(CMenuCtl * pme, CtlAddItem * pai,const char ** ppszResImage, IImage ** ppImage);
static IImage * MCEX_LoadImage(CMenuCtl * pme, const char * pszRes, uint16 wID, boolean * pbCached);
static IImage* MCEX_LoadImageEx(CMenuCtl * pme, const char* pszResImage, uint16 wImage, IImage * pImage);
static void MCEX_SetupImage(CMenuCtl * pme, CMenuItem * pmi, IImage * pImage);

static void MCEX_DrawItemCB(void * pvContext, AEEODCBData* is);
static int MCEX_GetItemIndex(CMenuCtl* pme, int itemID);
static boolean MCEX_HandlePens(CMenuCtl * pme, AEEEvent evtCode, uint16 wParam, uint32 dwParam);
static void MCEX_Recalc(CMenuCtl * pme);
static CMenuItem* MCEX_FindItem(CMenuCtl * pme, uint16 nItemID);
static void MCEX_DrawItem(CMenuCtl * pme, CMenuItem * p, AEERect * prc, boolean bSel, int nTruncExtent);
static void MCEX_DrawScrollBar(CMenuCtl * pme, AEEODCBData* is);
static void MCEX_AdjustRect(AEERect * prc, AEEItemStyle * ps);
static void MCEX_GetStyleOffsets(CMenuCtl * pme,int * pcx, int * pcy);

static boolean MCEX_IsPtInRect(int x, int y, AEERect* prc);
static boolean MCEX_IsRcInRc(AEERect* prc1, AEERect* prc2);
static boolean MCEX_IsRectEqual(AEERect* prc1, AEERect* prc2);
static boolean MCEX_GetMenuDisplayRect(IMenuCtl* pMenu, AEERect* prc);

static boolean MCEX_SetItemText(CMenuCtl * pme, CMenuItem* pitem, const char* pszResText, uint16 wText, AECHAR* pwszText);
static int MCEX_GetItemWidth(CMenuItem * pi);

static void MCEX_ResetAutoScroll(CMenuCtl * pme);
static void MCEX_AutoScroll(CMenuCtl * pme);
static boolean MCEX_ItemScrolls(CMenuCtl * pme, CMenuItem *pi, int nIdx);

#ifndef AEEMENUCLT_STATIC
/*==================================================================
  This function is called by AEEModGen when class is being created.
==================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod,  void **ppObj)
{
  switch(ClsId)
  {
    case AEECLSID_ICONVIEWCTLEX:
    case AEECLSID_LISTCTLEX:
    case AEECLSID_MENUCTLEX:
    case AEECLSID_SOFTKEYCTLEX:
      return MCEX_MenuCtlClsCreateInstance(ClsId, pIShell, pMod, ppObj);
    default:
      return EUNSUPPORTED;
  }
}
#endif //AEEMENUCLT_STATIC

/*==================================================================
  Creates an instance of the class and initializes it.
==================================================================*/
int MCEX_MenuCtlClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod,  void **ppObj)
{
  CMenuCtl* pme;

  AEECLSID  ctlID;

  switch(ClsId)
  {
    case AEECLSID_ICONVIEWCTLEX:
      ctlID = AEECLSID_ICONVIEWCTL;
      break;
    case AEECLSID_LISTCTLEX:
      ctlID = AEECLSID_LISTCTL;
      break;
    case AEECLSID_MENUCTLEX:
      ctlID = AEECLSID_MENUCTL;
      break;
    case AEECLSID_SOFTKEYCTLEX:
      ctlID = AEECLSID_SOFTKEYCTL;
      break;
    default:
      return EFAILED;
  }

  if( MCEX_New(pIShell, ClsId, ppObj) == SUCCESS)
  {
    int             cx,cy;
    AEEDeviceInfo   dm;
    AEEMenuColors   color;

    pme = (CMenuCtl *) *ppObj;
    AEEMENUCTL_AddRef((IMenuCtl *)pme);

    // Create the main application controls.
    if(ISHELL_CreateInstance(pIShell, ctlID, (void**)(&pme->m_pIMenu)) != SUCCESS)
    {
      return FALSE;
    }

    if(SUCCESS != ISHELL_CreateInstance(pIShell, AEECLSID_DISPLAY, (void **)&pme->m_pIDisplay))
    {
      IMENUCTL_Release(pme->m_pIMenu);
      FREE(pme);
      return FALSE;
    }

    // Increment the reference count on the shell...
    ISHELL_AddRef(pIShell);
    pme->m_pIShell    = pIShell;           // IShell Pointer
    pme->m_cls        = ClsId;             // Control Class
    
    pme->m_pIGraphics = NULL;
    if(ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_GRAPHICS,(void**)&pme->m_pIGraphics) != SUCCESS)
    {
      DBGPRINTF_ERROR("MCEX: failed to create IGraphics instance.");
    }

    if(pMod)
    {
      IMODULE_AddRef(pMod);
    }
    pme->m_pIModule   = pMod;

    pme->m_bSendCmd   = 1;                 // defaults to on
    pme->m_dwProps    = 0;

    // get display metrics
    ISHELL_GetDeviceInfo(pme->m_pIShell, &dm);
    IMENUCTL_GetColors(pme->m_pIMenu, &color);

    pme->m_ScrollBarInfo.nSize = sizeof(AEEScrollBarInfo);
    pme->m_ScrollBarInfo.bDisable = FALSE;
    pme->m_ScrollBarInfo.wWidth = dm.cxScrollBar;
    pme->m_ScrollBarInfo.cFrame = color.cFrame;
    pme->m_ScrollBarInfo.cScrollbar = color.cScrollbar;
    pme->m_ScrollBarInfo.cThumb = color.cScrollbarFill;
    pme->m_ScrollBarInfo.pImageScrollbar = NULL;
    pme->m_ScrollBarInfo.pImageThumb = NULL;
    pme->m_ScrollBarInfo.pszResImage = NULL;
    pme->m_ScrollBarInfo.wImageScrollbar = 0;
    pme->m_ScrollBarInfo.wImageThumb = 0;

    MEMSET(&pme->m_ItemInfo, 0, sizeof(AEEMenuExItemInfo));
    pme->m_ItemInfo.nSize = sizeof(AEEMenuExItemInfo);
    pme->m_ItemInfo.cText = color.cText;
    pme->m_ItemInfo.cTextSel = color.cSelText;
    pme->m_ItemInfo.pImage = NULL;
    pme->m_ItemInfo.pImageSelect = NULL;
    pme->m_ItemInfo.pszResImage = NULL;
    pme->m_ItemInfo.wImage = 0;
    pme->m_ItemInfo.wImageSelect = 0;

    pme->m_bScrollBar = FALSE; 
    pme->m_bSingleClick = TRUE;
 
    pme->m_nCurrentItemID  = -1;
    pme->m_nCurrentItemIdx = -1;
    MEMSET(&pme->m_CurrentItemRc, 0, sizeof(AEERect));
    pme->m_bPtDown = FALSE;
    pme->m_bPtMove = FALSE;

    pme->m_pItemList = QcListMake();
    QcListRegisterFreeNodeCB(pme->m_pItemList, (FreeDataCB)&MCEX_FreeMenuItem);

    pme->m_nOWDType = OWD_NONE;
    // set owner draw only on AEECLSID_MENUCTLEX
    if(IS_MENU(pme))
    {
      if(SUCCESS == IMENUCTL_SetOwnerDrawCB(pme->m_pIMenu, (PFNOWNERDRAWCB)&MCEX_DrawItemCB, pme, ODA_DRAW_ITEM | ODF_SB_OD))
      {
        DBGPRINTF_ERROR("MCEX: failed to set owner draw.");
      }
      pme->m_nOWDType = OWD_CTL;
    }

    // obtain font metrics for later use
    pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, AEE_FONT_BOLD, NULL, NULL);
    pme->m_cBorderFrame = BORDER_FRAME_COLOR;

    MCEX_GetStyleOffsets(pme, &cx, &cy);
    pme->m_cxMaxItem  = 0;
    pme->m_cyMaxItem  = pme->m_cyFont + cy;
    pme->m_cxItemExtra = 0;
    pme->m_cyItemExtra = 0;
    pme->m_bAutoScroll = TRUE;
    
    // Default Control Rect
    if (ClsId == IS_SOFTKEY_MENU(pme)) 
    {
       pme->m_rc.x    = 0;
       pme->m_rc.dx   = dm.cxScreen;
       MCEX_GetStyleOffsets(pme,&cx,&cy);
       pme->m_rc.dy   = pme->m_cyFont + cy;
       pme->m_rc.y    = (int16)(dm.cyScreen - pme->m_rc.dy); // Bottom of Screen
    }
    else 
    {
       SETAEERECT(&pme->m_rc, 0, 0, dm.cxScreen, dm.cyScreen);  // Entire Screen
    }
    MEMCPY(&pme->m_rcMenu, &pme->m_rc, sizeof(AEERect));


   // Auto Scroll - Update Delay
   if(dm.wMenuTextScroll)
      pme->m_wMenuTextScroll = (uint32)dm.wMenuTextScroll;
   else
      pme->m_wMenuTextScroll = AUTO_SCROLL_TIME;

    pme->m_nPointerPosX = 0;
    pme->m_nPointerPosY = 0;
    MEMSET(&pme->m_rcScrollBar, 0, sizeof(AEERect));

    return SUCCESS;
  }

  return EFAILED;
}

/*==================================================================
  Creates VTB
==================================================================*/
int MCEX_New(IShell *pIShell, AEECLSID cls, void ** ppobj)
{
  CMenuCtl         *pme = NULL;
  VTBL(IMenuCtl)   *gCMenuCtlExtFuncs;

  if( !ppobj || !pIShell  )
    return EFAILED;

  *ppobj = NULL;

  // Allocate the module’s struct and initialize it.
  if( (pme = (CMenuCtl*)MALLOC(sizeof(CMenuCtl) +sizeof(VTBL(IMenuCtl)))) == NULL )
    return ENOMEMORY;

  gCMenuCtlExtFuncs = (VTBL(IMenuCtl)*)((byte *)pme + sizeof(CMenuCtl));

  //Initialize individual entries in the VTBL
  gCMenuCtlExtFuncs->AddRef         = AEEMENUCTL_AddRef;
  gCMenuCtlExtFuncs->Release       = AEEMENUCTL_Release;
  gCMenuCtlExtFuncs->HandleEvent   = AEEMENUCTL_HandleEvent;
  gCMenuCtlExtFuncs->Redraw         = AEEMENUCTL_Redraw;
  gCMenuCtlExtFuncs->SetActive     = AEEMENUCTL_SetActive;
  gCMenuCtlExtFuncs->IsActive       = AEEMENUCTL_IsActive;
  gCMenuCtlExtFuncs->SetRect       = AEEMENUCTL_SetRect;
  gCMenuCtlExtFuncs->GetRect       = AEEMENUCTL_GetRect;
  gCMenuCtlExtFuncs->SetProperties = AEEMENUCTL_SetProperties;
  gCMenuCtlExtFuncs->GetProperties = AEEMENUCTL_GetProperties;
  gCMenuCtlExtFuncs->Reset         = AEEMENUCTL_Reset;
  gCMenuCtlExtFuncs->SetTitle       = AEEMENUCTL_SetTitle;
  gCMenuCtlExtFuncs->AddItem       = AEEMENUCTL_AddItem;
  gCMenuCtlExtFuncs->AddItemEx     = AEEMENUCTL_AddItemEx;
  gCMenuCtlExtFuncs->GetItemData   = AEEMENUCTL_GetItemData;
  gCMenuCtlExtFuncs->DeleteItem     = AEEMENUCTL_DeleteItem;
  gCMenuCtlExtFuncs->DeleteAll     = AEEMENUCTL_DeleteAll;
  gCMenuCtlExtFuncs->SetSel         = AEEMENUCTL_SetSel;
  gCMenuCtlExtFuncs->GetSel         = AEEMENUCTL_GetSel;
  gCMenuCtlExtFuncs->EnableCommand = AEEMENUCTL_EnableCommand;
  gCMenuCtlExtFuncs->SetItemText   = AEEMENUCTL_SetItemText;
  gCMenuCtlExtFuncs->SetItemTime   = AEEMENUCTL_SetItemTime;
  gCMenuCtlExtFuncs->GetItemTime   = AEEMENUCTL_GetItemTime;
  gCMenuCtlExtFuncs->SetStyle       = AEEMENUCTL_SetStyle;
  gCMenuCtlExtFuncs->SetColors     = AEEMENUCTL_SetColors;
  gCMenuCtlExtFuncs->MoveItem       = AEEMENUCTL_MoveItem;
  gCMenuCtlExtFuncs->GetItemCount   = AEEMENUCTL_GetItemCount;
  gCMenuCtlExtFuncs->GetItemID     = AEEMENUCTL_GetItemID;
  gCMenuCtlExtFuncs->GetItem       = AEEMENUCTL_GetItem;
  gCMenuCtlExtFuncs->SetItem       = AEEMENUCTL_SetItem;
  gCMenuCtlExtFuncs->Sort           = AEEMENUCTL_Sort;
  gCMenuCtlExtFuncs->SetSelEx       = AEEMENUCTL_SetSelEx;
  gCMenuCtlExtFuncs->EnumSelInit   = AEEMENUCTL_EnumSelInit;
  gCMenuCtlExtFuncs->EnumNextSel   = AEEMENUCTL_EnumNextSel;
  gCMenuCtlExtFuncs->GetStyle       = AEEMENUCTL_GetStyle;
  gCMenuCtlExtFuncs->GetColors     = AEEMENUCTL_GetColors;
  gCMenuCtlExtFuncs->GetItemRect   = AEEMENUCTL_GetItemRect;
  gCMenuCtlExtFuncs->SetOwnerDrawCB= AEEMENUCTL_SetOwnerDrawCB;

  // initialize the vtable
  pme->pvt = gCMenuCtlExtFuncs;
  *ppobj = pme;

  return SUCCESS;
}

/*=====================================================================

Public Method - Increments the reference count on the menu class object.

======================================================================*/
static uint32 AEEMENUCTL_AddRef(IMenuCtl * po)
{
  // since we maintian the referencce count, no need to path this to IMenu
   return(++((CMenuCtl*)po)->m_nRefs);
}

/*=====================================================================

Public Method - Decrements the reference count on the menu class object.
If the value reaches 0, it frees the object and all associated data.

======================================================================*/
static uint32 AEEMENUCTL_Release(IMenuCtl * po)
{
   CMenuCtl * pme = (CMenuCtl*)po;


    if(pme->m_nRefs)
    {
      if (--pme->m_nRefs)
         return(pme->m_nRefs);

      IMENUCTL_Release(pme->m_pIMenu);

      // Cancel All Timers
      ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);

      // Delete All Items
     // AEEMENUCTL_DeleteAll((IMenuCtl*)pme);

      // Free Title
      MCEX_FreeObj((void **)&pme->m_pTitle);

      // Reset Calendar
      //Cal_Reset(pme);

//      CALLBACK_Cancel(&pme->m_cbCommand);

      // Release IShell & IDisplay
      MCEX_ReleaseObj((void **)&pme->m_pIShell);
      MCEX_ReleaseObj((void **)&pme->m_pIModule);
      MCEX_ReleaseObj((void **)&pme->m_pIDisplay);
      MCEX_ReleaseObj((void **)&pme->m_pIGraphics);

      MCEX_ReleaseObj((void **)&pme->m_ScrollBarInfo.pImageScrollbar);
      MCEX_ReleaseObj((void **)&pme->m_ScrollBarInfo.pImageThumb);

      MCEX_ReleaseObj((void **)&pme->m_ItemInfo.pImageSelect);
      MCEX_ReleaseObj((void **)&pme->m_ItemInfo.pImage);

      QcListRegisterFreeNodeCB(pme->m_pItemList, (FreeDataCB)&MCEX_FreeMenuItem);
      QcListFree(&pme->m_pItemList);
      // Free Menu Object
      FREE(pme);
   }
   return(0);
}

/*=====================================================================

Public Method - Handles events send by the app to the menu.

======================================================================*/
static boolean AEEMENUCTL_HandleEvent(IMenuCtl * po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  CMenuCtl * pme = (CMenuCtl*)po;
  boolean    bHandled;

  bHandled = IMENUCTL_HandleEvent(pme->m_pIMenu, eCode, wParam, dwParam);

  // First handle the pens, most time sensitive events we get
  if(IMENUCTL_IsActive(pme->m_pIMenu) && !IS_SOFTKEY_MENU(pme) && eCode >= EVT_POINTER_DOWN && eCode <= EVT_POINTER_STALE_MOVE )
  {
     // Stale move is not currently handled by aeemenuctl
     if( eCode == EVT_POINTER_STALE_MOVE )
     {
        return bHandled;
     }
     // we should not alternate the return
     (void)MCEX_HandlePens(pme, eCode, wParam, dwParam);
  }
  return bHandled;
}

/*=====================================================================

Public Method - Instructs the menu to redraw the contents.

======================================================================*/
static boolean AEEMENUCTL_Redraw(IMenuCtl * po)
{
   return IMENUCTL_Redraw(((CMenuCtl*)po)->m_pIMenu);
}

/*=====================================================================

Public Method - Sets the menu in an "active" state whereby keys are
processed.

======================================================================*/
static void AEEMENUCTL_SetActive(IMenuCtl * po, boolean bActive)
{
   IMENUCTL_SetActive(((CMenuCtl*)po)->m_pIMenu, bActive);
}

/*=====================================================================

Public Method - Returns the active state of the menu.

======================================================================*/
static boolean AEEMENUCTL_IsActive(IMenuCtl * po)
{
   return IMENUCTL_IsActive(((CMenuCtl*)po)->m_pIMenu);
}

/*=====================================================================

Public Method - Sets the rectangle for the menu.

======================================================================*/
static void AEEMENUCTL_SetRect(IMenuCtl * po, const AEERect * prc)
{
   IMENUCTL_SetRect(((CMenuCtl*)po)->m_pIMenu, prc);
}

/*=====================================================================

Public Method - Returns the rectangle for the menu.

======================================================================*/
static void AEEMENUCTL_GetRect(IMenuCtl * po, AEERect * prc)
{
   IMENUCTL_GetRect(((CMenuCtl*)po)->m_pIMenu, prc);
}

/*=====================================================================

Public Method - Sets the properties (flags) for the menu.

======================================================================*/
static void AEEMENUCTL_SetProperties(IMenuCtl * po, uint32 nProperties)
{
  CMenuCtl * pme = (CMenuCtl*)po;
  if(NULL != pme)
  {
    // save user's setting
    pme->m_dwProps |= nProperties;

    IMENUCTL_SetProperties(pme->m_pIMenu, nProperties);
  }
}

/*=====================================================================

Public Method - Returns the active properties for the menu.

======================================================================*/
static uint32 AEEMENUCTL_GetProperties(IMenuCtl * po)
{
   return IMENUCTL_GetProperties(((CMenuCtl*)po)->m_pIMenu);
}

/*=====================================================================

Public Method - Clears the menu contents and cancels all pending
activities.

======================================================================*/
static void AEEMENUCTL_Reset(IMenuCtl * po)
{
  CMenuCtl * pme = (CMenuCtl*)po;
  int cx, cy;

  // cancel timer if we have any
  ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);
  MCEX_FreeObj((void **)&pme->m_pTitle);
  QcListRegisterFreeNodeCB(pme->m_pItemList, (FreeDataCB)&MCEX_FreeMenuItem);
  QcListFree(&pme->m_pItemList);

  MCEX_GetStyleOffsets(pme, &cx, &cy);
  pme->m_cxMaxItem  = 0;
  pme->m_cyMaxItem  = pme->m_cyFont + cy;
  pme->m_bScrollBar = FALSE;
  IMENUCTL_Reset(pme->m_pIMenu);
}

/*=====================================================================

Public Method - Sets the menu title per AEEMenu.h

======================================================================*/
static boolean AEEMENUCTL_SetTitle(IMenuCtl * po, const char * pszResFile, uint16 wResID, AECHAR * pText)
{
   CMenuCtl * pme = (CMenuCtl*)po;
   AECHAR  szBuff[32];
   boolean bRet = FALSE;

   // Return FALSE if title is owner drawn
   if(IS_TITLE_OD(pme))
      return bRet;

   MEMSET(szBuff, 0, sizeof(szBuff));

   // Load Title if necessary
   if (!pText && pszResFile
      && ISHELL_LoadResString(pme->m_pIShell, pszResFile, wResID, szBuff, sizeof(szBuff))) {
      pText = szBuff;
   }

   // Allocate Memory
   if (pText)
   {
      // Free Old Title
      MCEX_FreeObj((void **)&pme->m_pTitle);
      pme->m_pTitle = WSTRDUP(pText);
   }

   return IMENUCTL_SetTitle(((CMenuCtl*)po)->m_pIMenu, pszResFile, wResID, pText);
}

/*=====================================================================

Public Method - Adds a menu item per AEEMenu.h

======================================================================*/
static boolean AEEMENUCTL_AddItem(IMenuCtl * po, const char * pszResFile, uint16 wResID, uint16 nItemID, AECHAR * pText, uint32 lData)
{
  CMenuCtl*  pme;
  CMenuItem* pitem = 0;
  //int        len = 0;
  boolean    bRet = FALSE;

  pme = (CMenuCtl*)po;
  if(NULL == pme)
  {
    DBGPRINTF_ERROR("MCEX: invalid parm.");
    return FALSE;
  }

  // create a menu item
  pitem = (CMenuItem*)MALLOC(sizeof(CMenuItem));
  if(NULL == pitem)
  {
    DBGPRINTF_ERROR("MCEX: failed to allocate memory.");
    return bRet;
  }
  MEMSET(pitem, 0, sizeof(CMenuItem));

  if(pszResFile)
  {
    pitem->pszResText = STRDUP(pszResFile);
  }
  pitem->nItemID    = nItemID;
  pitem->wText      = wResID;
  pitem->dwData     = lData;
  pitem->nFont      = AEE_FONT_NORMAL;

  if(NULL == pszResFile && NULL == pText)
  {
    // client wants to handle drawing
    pitem->m_nOWDType = OWD_USER;
  }
  else if(OWD_CTL== pme->m_nOWDType)
  {
    // owner draw will be handled by ctl
    pitem->m_nOWDType = OWD_CTL;
    if(!MCEX_SetItemText(pme, pitem, pszResFile, wResID, pText))
    {
      return FALSE;
    }
  }
  else
  {
    pitem->m_nOWDType = OWD_NONE;
  }

  if(NULL == pme->m_pItemList)
  {
    pme->m_pItemList = QcListMake();
    if(NULL == pme->m_pItemList)
    {
      MCEX_FreeMenuItem(&pitem);
      return FALSE;
    }
  }

  if(OWD_USER == pitem->m_nOWDType || OWD_CTL == pitem->m_nOWDType)
  {
    bRet = IMENUCTL_AddItem(((CMenuCtl*)po)->m_pIMenu, NULL, wResID, nItemID, NULL, lData);
  }
  else
  {
     bRet = IMENUCTL_AddItem(((CMenuCtl*)po)->m_pIMenu, pszResFile, wResID, nItemID, pText, lData);
  }

  if(TRUE == bRet)
  {
    if(FALSE == (bRet = QcListAddNode(pme->m_pItemList, (void*)pitem)))
    {
      (void)IMENUCTL_DeleteItem(((CMenuCtl*)po)->m_pIMenu, nItemID); 
      DBGPRINTF_ERROR("MCEX: failed to add node.");
    }
  }
  
  if(FALSE == bRet)
  {
    DBGPRINTF_ERROR("MCEX: failed to add menu item.");
    MCEX_FreeMenuItem(&pitem);
  }

  if(pitem && pitem->pText)
  {
    pitem->cxText = IDISPLAY_MeasureText(pme->m_pIDisplay, pitem->nFont, pitem->pText);
    MCEX_Recalc(pme);
  }
  return bRet;
}

/*=====================================================================

Public Method - Enhanced mechanism to add menu items.  See AEEMenu.h

======================================================================*/
static boolean AEEMENUCTL_AddItemEx(IMenuCtl * po, CtlAddItem * pai)
{
  CMenuCtl*  pme;
  CMenuItem* pitem;
  //int        len = 0;
  IImage *       pImage = NULL;
  const char *   pszResImage = NULL;
  boolean        bRet = FALSE;

  pme = (CMenuCtl*)po;
  if(NULL == pme || NULL == pai)
  {
    DBGPRINTF_ERROR("MCEX: invalid parm.");
    return FALSE;
  }

  if(OWD_NONE != pme->m_nOWDType)
  {
    // create a menu item
    pitem = (CMenuItem*)MALLOC(sizeof(CMenuItem));
    if(NULL == pitem)
    {
      DBGPRINTF_ERROR("MCEX: failed to allocate memory.");
      return FALSE;
    }
    MEMSET(pitem, 0, sizeof(CMenuItem));

    // search for DUP
    //..........................

    if(pai->pszResText)
      pitem->pszResText  = STRDUP(pai->pszResText);
    if(pai->pszResImage)
      pitem->pszImage    = STRDUP(pai->pszResImage);
    pitem->nItemID     = pai->wItemID;
    pitem->wText       = pai->wText;
    pitem->wFont       = pai->wFont;
    pitem->dwData      = pai->dwData;
    pitem->wImageID    = pai->wImage;
    pitem->nFont       = (AEEFont)pai->wFont;

    // owner draw will be handled by ctl
    pitem->m_nOWDType = OWD_CTL;

    if(!MCEX_SetItemText(pme, pitem, pai->pszResText, pai->wText, (AECHAR*)pai->pText))
    {
      return FALSE;
    }

    if(!MCEX_ResolveImage(pme, pai, &pszResImage, &pImage))
    {
      DBGPRINTF_ERROR("MCEX: MCEX_ResolveImage failed.");
      MCEX_FreeMenuItem(&pitem);
      return(FALSE);
    }

    // Pre-calc the image stuff in case we unload it
    if(pImage)
    {
      if(IS_MULTI_IMAGE(pme))
        IIMAGE_SetParm(pImage,IPARM_NFRAMES,(int)(IS_TRI_STATE(pme) ? 3 : 2),0);
      MCEX_SetupImage(pme,pitem,pImage);
    }

    if(NULL == pme->m_pItemList)
    {
      pme->m_pItemList = QcListMake();
      if(NULL == pme->m_pItemList)
      {
        MCEX_FreeMenuItem(&pitem);
       return FALSE;
      }
    }

    bRet = QcListAddNode(pme->m_pItemList, (void*)pitem);
    if(OWD_CTL == pitem->m_nOWDType)
    {
      // for owner draw, we have to use IMENUCTL_AddItem and res file ptr and pText have to be NULL
      bRet &= IMENUCTL_AddItem(((CMenuCtl*)po)->m_pIMenu, NULL, pai->wText, pai->wItemID, NULL, pai->dwData);
    }
    //else if(OWD_NONE == pitem->m_nOWDType)
    //{
    //  bRet &= IMENUCTL_AddItemEx(((CMenuCtl*)po)->m_pIMenu, pai);
    //}
    else
    {
      DBGPRINTF_ERROR("MCEX: unknown mode.");
    }

    if(pitem->pText)
      pitem->cxText = IDISPLAY_MeasureText(pme->m_pIDisplay, pitem->nFont, pitem->pText);
    MCEX_Recalc(pme);
  }
  else
  {
    bRet &= IMENUCTL_AddItemEx(((CMenuCtl*)po)->m_pIMenu, pai);
  }

  return bRet;
}

/*=====================================================================

Public Method - Moves an item up/down in the list.

======================================================================*/
static int16 AEEMENUCTL_MoveItem(IMenuCtl * po, uint16 wID, int nDir)
{
  return IMENUCTL_MoveItem(((CMenuCtl*)po)->m_pIMenu, wID, nDir);
}

/*=====================================================================

Public Method - Updates information regarding a menu item. See AEEMenu.h

======================================================================*/
static boolean AEEMENUCTL_SetItem(IMenuCtl * po, uint16 wID, uint16 wFlags, CtlAddItem * pai)
{
  CMenuCtl* pme = (CMenuCtl*)po;
  CMenuItem* pmi;
  IImage *       pImage = NULL;
  const char *   pszResImage = NULL;

  if(NULL == pme)
    return FALSE;

  pmi = MCEX_FindItem(pme, wID);
  if(pmi)
  {
    if(wFlags & MSIF_DATA)
      pmi->dwData = pai->dwData;

    if(pmi->m_nOWDType != OWD_USER)
    {
      // set followings only when item is not owner draw type

      // set font
      if(wFlags & MSIF_FONT)
        pmi->nFont = (AEEFont)pai->wFont;

      // delete current res string
      MCEX_FreeObj((void **)&pmi->pszResText);
      MCEX_FreeObj((void **)&pmi->pszImage);
     
      if(pai->pszResText)
      {
        pmi->pszResText = STRDUP(pai->pszResText);
      }
      if(pai->pszResImage)
      {
        pmi->pszImage = STRDUP(pai->pszResImage);
      }

     if(wFlags & MSIF_TEXT)
     {
       (void)MCEX_SetItemText(pme, pmi, pai->pszResText, pai->wText, (AECHAR *)pai->pText);
     }

     if(wFlags & MSIF_IMAGE)
     {
       if(pai->pszResImage && pai->wImage && !MCEX_ResolveImage(pme, pai, &pszResImage, &pImage))
       {
         DBGPRINTF_ERROR("MCEX: MCEX_ResolveImage failed.");
       }

      // Pre-calc the image stuff in case we unload it
      if(pImage)
      {
        if(IS_MULTI_IMAGE(pme))
          IIMAGE_SetParm(pImage,IPARM_NFRAMES,(int)(IS_TRI_STATE(pme) ? 3 : 2),0);
        MCEX_SetupImage(pme, pmi, pImage);
      }

     }
    }
  }
  return IMENUCTL_SetItem(((CMenuCtl*)po)->m_pIMenu, wID, wFlags, pai);
}

/*=====================================================================

Public Method - Returns the item count in a menu.

======================================================================*/
static int AEEMENUCTL_GetItemCount(IMenuCtl * po)
{
   return IMENUCTL_GetItemCount(((CMenuCtl*)po)->m_pIMenu);
}

/*=====================================================================

Public Method - Returns the item ID given the index value.

======================================================================*/
static uint16 AEEMENUCTL_GetItemID(IMenuCtl * po, int nIdx)
{
   return IMENUCTL_GetItemID(((CMenuCtl*)po)->m_pIMenu, nIdx);
}

/*=====================================================================

Public Method - Sorts the menu.  See AEEMenu.h

======================================================================*/
static void  AEEMENUCTL_Sort(IMenuCtl * po, AEEMenuSortType t, PFNMENUSORTCOMPARE pfn,void * pUser)
{
  IMENUCTL_Sort(((CMenuCtl*)po)->m_pIMenu, t);
}

/*=====================================================================

Public Method - Returns information about the item.  See AEEMenu.h

======================================================================*/
static boolean AEEMENUCTL_GetItem(IMenuCtl * po, uint16 wID, CtlAddItem *pai)
{
  CMenuCtl* pme = (CMenuCtl*)po;
  CMenuItem* pitem;

  // query from pIMenu to make sure this item is still valid
  if(!IMENUCTL_GetItem(pme->m_pIMenu, wID, pai))
  {
    return FALSE;
  }

  if (!IS_MENU(pme))
  {
     return TRUE;
  }

  if(NULL != (pitem = MCEX_FindItem(pme, wID)))
  {
    pai->pText        = pitem->pText; 
    pai->pImage       = pitem->pImage; 
    pai->wItemID      = pitem->nItemID;   
    pai->pszResText   = pitem->pszResText;
    pai->wText        = pitem->wText;     
    pai->wFont        = pitem->wFont;     
    pai->dwData       = pitem->dwData;    
    pai->pszResImage  = pitem->pszImage;  
    pai->wImage       = pitem->wImageID;  
    pai->wFont        = pitem->nFont; 

    return TRUE;
  }

  return FALSE;
}

/*=====================================================================

Public Method - Returns information about the item.  See AEEMenu.h

======================================================================*/
static boolean AEEMENUCTL_GetItemData(IMenuCtl * po, uint16 nItemID, uint32 * plData)
{
   return IMENUCTL_GetItemData(((CMenuCtl*)po)->m_pIMenu, nItemID, plData);
}

/*=====================================================================

Public Method - Deletes an item from the menu.  See AEEMenu.h

======================================================================*/
static boolean AEEMENUCTL_DeleteItem(IMenuCtl * po, uint16 nItemID)
{
  CMenuCtl * pme = (CMenuCtl*)po;

  if(NULL == pme)
  {
    return FALSE;
  }

  if(pme->m_pItemList)
  {
    CMenuItem* pmi = MCEX_FindItem(pme, nItemID);
    if(pmi)
    {
      if(FALSE == QcListRemoveNode(pme->m_pItemList, pmi))
      {
        DBGPRINTF_ERROR("MCEX: failed to remove node, nItemID = &d.", nItemID);
      }
    }
  }

  pme->m_bScrollBar = FALSE;
  return IMENUCTL_DeleteItem(((CMenuCtl*)po)->m_pIMenu, nItemID);
}

/*=====================================================================

Public Method - Deletes all items from the menu.  See AEEMenu.h

======================================================================*/
static boolean AEEMENUCTL_DeleteAll(IMenuCtl * po)
{
  CMenuCtl* pme = (CMenuCtl*)po;

  QcListRegisterFreeNodeCB(pme->m_pItemList, (FreeDataCB)&MCEX_FreeMenuItem);
  QcListFree(&pme->m_pItemList);

  pme->m_bScrollBar = FALSE;
  return IMENUCTL_DeleteAll(pme->m_pIMenu);
}

/*=====================================================================

Public Method - Sets selection in single-selection menu. See AEEMenu.h

======================================================================*/
static void AEEMENUCTL_SetSel(IMenuCtl * po, uint16 nItemID)
{
   IMENUCTL_SetSel( ((CMenuCtl*)po)->m_pIMenu, nItemID);
}

static void AEEMENUCTL_SetSelEx(IMenuCtl * po, uint16 nItemID, boolean bSelected)
{
   IMENUCTL_SetSelEx(((CMenuCtl*)po)->m_pIMenu, nItemID, bSelected);
}

/*=====================================================================

Public Method - Returns selection on single-selection menu. See AEEMenu.h

======================================================================*/
static uint16 AEEMENUCTL_GetSel(IMenuCtl * po)
{
   return IMENUCTL_GetSel(((CMenuCtl*)po)->m_pIMenu);
}

/*=====================================================================

Public Method - Enables the sending of the EVT_COMMAND event. See AEEMenu.h

======================================================================*/
static void AEEMENUCTL_EnableCommand(IMenuCtl * po, boolean bEnable)
{
  ((CMenuCtl*)po)->m_bSendCmd = (bEnable != 0);
  IMENUCTL_EnableCommand(((CMenuCtl*)po)->m_pIMenu, bEnable);
}

/*=====================================================================

Public Method - Sets the text for a menu item. See AEEMenu.h

======================================================================*/
static void AEEMENUCTL_SetItemText(IMenuCtl * po, uint16 nItemID, const char * pszResFile, uint16 wResID, AECHAR * pText)
{
  CMenuCtl* pme = (CMenuCtl*)po;
  CMenuItem* pmi;


  if(NULL == pme)
    return;

  pmi = MCEX_FindItem(pme, nItemID);
  if(pmi)
  {
    (void)MCEX_SetItemText(pme, pmi, pszResFile, wResID, (AECHAR *)pText);
  }
  IMENUCTL_SetItemText(((CMenuCtl*)po)->m_pIMenu, nItemID, pszResFile, wResID, pText);
}

/*=====================================================================

Public Method - Sets the time for a calendar menu item. See AEEMenu.h

======================================================================*/
static void AEEMENUCTL_SetItemTime(IMenuCtl * po, uint16 nItemID, uint16 wMinStart, uint16 wDuration)
{
   IMENUCTL_SetItemTime(((CMenuCtl*)po)->m_pIMenu, nItemID, wMinStart, wDuration);
}

/*=====================================================================

Public Method - Returns the time for a calendar menu item. See AEEMenu.h

======================================================================*/
static int AEEMENUCTL_GetItemTime(IMenuCtl * po, uint16 nItemID, uint16 * pwDuration)
{
   return IMENUCTL_GetItemTime(((CMenuCtl*)po)->m_pIMenu, nItemID, pwDuration);
}

/*=====================================================================

Public Method - Sets the menu item style. See AEEMenu.h

======================================================================*/
static void AEEMENUCTL_SetStyle(IMenuCtl * po, AEEItemStyle * pNormal, AEEItemStyle * pSel)
{
   IMENUCTL_SetStyle(((CMenuCtl*)po)->m_pIMenu, pNormal, pSel);
}

/*=====================================================================

Public Method - Gets the menu item style. See AEEMenu.h

======================================================================*/
static void AEEMENUCTL_GetStyle(IMenuCtl * po, AEEItemStyle * pNormal, AEEItemStyle * pSel)
{
   IMENUCTL_GetStyle(((CMenuCtl*)po)->m_pIMenu, pNormal, pSel);
}

/*=====================================================================

Public Method - Sets the menu colors. See AEEMenu.h

======================================================================*/
static void AEEMENUCTL_SetColors(IMenuCtl * po, AEEMenuColors * pc)
{

  //TODO: need to update scrollbar color info
   IMENUCTL_SetColors(((CMenuCtl*)po)->m_pIMenu, pc);
}

/*=====================================================================

Public Method - Gets the menu colors. See AEEMenu.h

======================================================================*/
static void AEEMENUCTL_GetColors(IMenuCtl * po, AEEMenuColors * pc)
{
  //TODO: scrollbar color info?
   IMENUCTL_GetColors(((CMenuCtl*)po)->m_pIMenu, pc);
}

/*=====================================================================

Public Method - Gets the menu item rect. See AEEMenu.h

======================================================================*/
static void AEEMENUCTL_GetItemRect(IMenuCtl * po, uint16 nIdx, AEERect * prc)
{
  IMENUCTL_GetItemRect(((CMenuCtl*)po)->m_pIMenu, nIdx, prc);
}

/*=====================================================================

Public Method - Initializes a multi selection menu for selection enumeration

======================================================================*/
static boolean AEEMENUCTL_EnumSelInit(IMenuCtl * po)
{
   return IMENUCTL_EnumSelInit(((CMenuCtl*)po)->m_pIMenu);
}

/*=====================================================================

Public Method - Returns next selection in enumeration of a multi selection
menu

======================================================================*/
static uint16 AEEMENUCTL_EnumNextSel(IMenuCtl * po)
{
   return IMENUCTL_EnumNextSel(((CMenuCtl*)po)->m_pIMenu);
}

/*=====================================================================

Public Method - Sets callbacks to be used for drawing/measuring owner draw
item. See AEEMenu.h

======================================================================*/
static int AEEMENUCTL_SetOwnerDrawCB(IMenuCtl * po, PFNOWNERDRAWCB pfnCB, void * pvContext, uint16 nODFlags)
{
  AEEODCBData cbData;
  CMenuCtl* pme = (CMenuCtl*)po;

  // If callback and flag are NULL, reset menu and nullify owner drawn stuff
  if (!pfnCB && !nODFlags)
  {
    AEEMENUCTL_Reset(po);
    pme->m_pfnCB = NULL;
    pme->m_pvContext = NULL;
    pme->m_nODFlags = 0;
  }

  // Make sure that callback and flags are valid
  if (!pfnCB || !nODFlags)
     return EFAILED;

  // save owner draw information
  pme->m_pfnCB = pfnCB;
  pme->m_pvContext = pvContext;
  pme->m_nODFlags = nODFlags;

  // Check if scroll bar is owner drawn. If so, measure scroll bar width
  if(nODFlags & ODF_SB_OD)
  {

// TODO need a local for image replacement

    if(pme->m_ScrollBarInfo.pImageScrollbar)
    {
      MCEX_ReleaseObj((void **)&pme->m_ScrollBarInfo.pImageScrollbar);
    }
    if(pme->m_ScrollBarInfo.pImageThumb)
    {
      MCEX_ReleaseObj((void **)&pme->m_ScrollBarInfo.pImageThumb);
    }

    // ask client for scroll bar info
    MEMSET(&cbData, 0, sizeof(AEEODCBData));
    cbData.nSize = sizeof(AEEODCBData);
    cbData.nAction = ODA_INFO_SB;
    cbData.nActionInfo = (int)&pme->m_ScrollBarInfo;

    pme->m_pfnCB(pme->m_pvContext, &cbData);


    pme->m_ScrollBarInfo.pImageScrollbar = MCEX_LoadImageEx(pme, pme->m_ScrollBarInfo.pszResImage,
                                                        pme->m_ScrollBarInfo.wImageScrollbar,
                                                        pme->m_ScrollBarInfo.pImageScrollbar);

    pme->m_ScrollBarInfo.pImageThumb = MCEX_LoadImageEx(pme, pme->m_ScrollBarInfo.pszResImage,
                                                        pme->m_ScrollBarInfo.wImageThumb,
                                                        pme->m_ScrollBarInfo.pImageThumb);


    // scroll bar width
    MEMSET(&cbData, 0, sizeof(AEEODCBData));
    cbData.nSize = sizeof(AEEODCBData);
    cbData.nAction = ODA_MEASURE_SB;
    cbData.rc.dx = pme->m_ScrollBarInfo.wWidth;

    // Measure ScrollBar Width
    pme->m_pfnCB(pme->m_pvContext, &cbData);
    // Cache ScrollBar Width
    pme->m_ScrollBarInfo.wWidth = cbData.rc.dx;
    if(pme->m_ScrollBarInfo.wWidth)
      pme->m_bScrollBar = TRUE;

    if(SUCCESS != IMENUCTL_SetOwnerDrawCB(pme->m_pIMenu, (PFNOWNERDRAWCB)&MCEX_DrawItemCB, pme, ODA_DRAW_ITEM | ODF_SB_OD))
    {
      DBGPRINTF_ERROR("MCEX: failed to set owner draw.");
    }
  }

  if(nODFlags & ODA_DRAW_ITEM)
  {
    MCEX_ReleaseObj((void **)&pme->m_ItemInfo.pImageSelect);
    MCEX_ReleaseObj((void **)&pme->m_ItemInfo.pImage);

    MEMSET(&cbData, 0, sizeof(AEEODCBData));
    cbData.nSize = sizeof(AEEODCBData);
    cbData.nAction = ODA_INFO_ITEM;
    cbData.nActionInfo = (int)&pme->m_ItemInfo;

    pme->m_pfnCB(pme->m_pvContext, &cbData);

    pme->m_ItemInfo.pImageSelect = MCEX_LoadImageEx(pme, pme->m_ItemInfo.pszResImage,
                                                        pme->m_ItemInfo.wImageSelect,
                                                        pme->m_ItemInfo.pImageSelect);

    pme->m_ItemInfo.pImage = MCEX_LoadImageEx(pme, pme->m_ItemInfo.pszResImage,
                                                        pme->m_ItemInfo.wImage,
                                                        pme->m_ItemInfo.pImage);

  }

  return SUCCESS; //IMENUCTL_SetOwnerDrawCB(((CMenuCtl*)po)->m_pIMenu, pfnCB, pvContext, nODFlags);
}

/*=====================================================================

Local Method - Handles all the pen events, moved here for handleevent clarity

======================================================================*/
boolean MCEX_HandlePens(CMenuCtl * pme, AEEEvent evtCode, uint16 wParam, uint32 dwParam)
{

  if(NULL == pme)
    return FALSE;

  //int nClickCount = AEE_POINTER_GET_CLICKCOUNT((const char *)dwParam);
  pme->m_nPointerPosX = (int16)AEE_POINTER_GET_X((const char *)dwParam);
  pme->m_nPointerPosY = (int16)AEE_POINTER_GET_Y((const char *)dwParam);

  switch( evtCode )
  {
    case EVT_POINTER_MOVE:
    {
      AEERect RcSelect;

#ifdef _MCEX_DEBUG
      DBGPRINTF_HIGH("MCEX: received EVT_POINTER_MOVE");
#endif //#ifdef _MCEX_DEBUG

      if(MCEX_IsPtInRect((int)pme->m_nPointerPosX, (int)pme->m_nPointerPosY, &pme->m_rcScrollBar))
      {
        ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);
#ifdef _MCEX_DEBUG
        DBGPRINTF_HIGH("MCEX: EVT_POINTER_MOVE cancel timer");
#endif //_MCEX_DEBUG
        pme->m_bAutoScroll = FALSE;
      }

      if(pme->m_nCurrentItemIdx < 0)
      {
        pme->m_nCurrentItemIdx = MCEX_GetItemIndex(pme, pme->m_nCurrentItemID);
      }

      if(pme->m_bPtDown && FALSE == pme->m_bPtMove)
      {
        IMENUCTL_GetItemRect(pme->m_pIMenu, (uint16)pme->m_nCurrentItemIdx, &RcSelect);
        if(!MCEX_IsRectEqual(&RcSelect, &pme->m_CurrentItemRc))
        {
          boolean bRedraw = FALSE;
          if(!pme->m_bPtMove)
          {
            // redraw once to erase selection on current item
            bRedraw = TRUE;
          }
          //pointer moved out of the original rect
          pme->m_bPtMove = TRUE;
          if(bRedraw)
          {
            (void)IMENUCTL_Redraw(pme->m_pIMenu);
          }
        }
      }
      else if(pme->m_bPtDown && TRUE == pme->m_bPtMove)
      {
        IMENUCTL_SetSel(pme->m_pIMenu, 0);
      }
      return FALSE;
    }
    case EVT_POINTER_DOWN:
    {
#ifdef _MCEX_DEBUG
      DBGPRINTF_HIGH("MCEX: received EVT_POINTER_DOWN");
#endif //#ifdef _MCEX_DEBUG

      pme->m_nCurrentItemID  = IMENUCTL_GetSel(pme->m_pIMenu);
      pme->m_nCurrentItemIdx = MCEX_GetItemIndex(pme, pme->m_nCurrentItemID);
      if(pme->m_nCurrentItemIdx >= 0)
      {
        // save current item rect
        IMENUCTL_GetItemRect(pme->m_pIMenu, (uint16)pme->m_nCurrentItemIdx, &pme->m_CurrentItemRc);
      }

      if(MCEX_IsPtInRect((int)pme->m_nPointerPosX, (int)pme->m_nPointerPosY, &pme->m_CurrentItemRc))

      {
        pme->m_bPtDown = TRUE;
      }
      else
      {

        pme->m_nCurrentItemID = -1;
        pme->m_nCurrentItemIdx = -1;
        MEMSET(&pme->m_CurrentItemRc, 0, sizeof(AEERect));
        pme->m_bPtDown = FALSE;
      }

      pme->m_bPtMove = FALSE;
      (void)IMENUCTL_Redraw(pme->m_pIMenu);

      return FALSE;
    }
    case EVT_POINTER_UP:
    {
#ifdef _MCEX_DEBUG
      DBGPRINTF_HIGH("MCEX: received EVT_POINTER_UP");
#endif //#ifdef _MCEX_DEBUG
      
      if(pme->m_bPtDown && pme->m_nCurrentItemID >= 0)
      {
        if(!pme->m_bPtMove && (pme->m_nPointerPosX > pme->m_CurrentItemRc.x && pme->m_nPointerPosX < pme->m_CurrentItemRc.x + pme->m_CurrentItemRc.dx &&
           pme->m_nPointerPosY > pme->m_CurrentItemRc.y && pme->m_nPointerPosY < pme->m_CurrentItemRc.y + pme->m_CurrentItemRc.dy))
        {
          int currentItemID;
          uint32 Data = 0;

          currentItemID = IMENUCTL_GetSel(pme->m_pIMenu);
          //make sure we are on same item
          if(pme->m_nCurrentItemID == currentItemID)
          {
             // print dbg info
          }

          if(pme->m_bSendCmd)
          {
            if(IMENUCTL_GetItemData(pme->m_pIMenu, (uint16)currentItemID, (uint32*)&Data))
              ISHELL_HandleEvent(pme->m_pIShell, EVT_COMMAND, (uint16)currentItemID, (uint32)Data);
            else
              ISHELL_HandleEvent(pme->m_pIShell, EVT_COMMAND, (uint16)currentItemID, (uint32)NULL);
          }
          pme->m_bPtDown = TRUE;
        }
      }
      pme->m_bPtMove = FALSE;
      pme->m_nCurrentItemID = -1;
      pme->m_nCurrentItemIdx = -1;
      MEMSET(&pme->m_CurrentItemRc, 0, sizeof(AEERect));
      pme->m_bPtDown = FALSE;

      return FALSE;
    }
    default:
      break;
  }

  return FALSE;
}

static boolean MCEX_ReleaseObj(void ** ppObj)
{
   if ( ppObj && *ppObj )
   {
      (void)IBASE_Release(((IBase *)*ppObj));
      *ppObj = NULL;
      return(TRUE);
   }

   return(FALSE);
}

static boolean MCEX_FreeObj(void ** ppObj)
{
   if (ppObj) {
         if (*ppObj) {
            FREE(*ppObj);
            *ppObj = NULL;
            return (TRUE);
         }
      }
   return (FALSE);
}

static void MCEX_FreeMenuItem(CMenuItem** ppItem)
{
  CMenuItem* p = *ppItem;

  if ( ppItem && *ppItem )
  {
    MCEX_FreeObj((void **)&p->pText);
    MCEX_FreeObj((void **)&p->pszResText);
    MCEX_FreeObj((void **)&p->pszImage);
    MCEX_ReleaseObj((void **)&(p->pImage));

    MCEX_FreeObj((void **)ppItem);
  }

}
int MCEX_GetItemIndex(CMenuCtl* pme, int itemID)
{
  int i, count;
  int id;

  count = IMENUCTL_GetItemCount(pme->m_pIMenu);
  for(i = 0; i < count; i++)
  {
    id = IMENUCTL_GetItemID(pme->m_pIMenu, i);
    if(id == itemID)
    {
      return i;
    }
  }

  return -1;
}

void MCEX_DrawItemCB(void * pvContext, AEEODCBData* is)
{
      CMenuCtl * pme = (CMenuCtl*)pvContext;
      uint32 info;

      if(NULL == pme || NULL == is)
      {
        return;
      }

      switch (is->nAction)
      {
         case ODA_MEASURE_TITLE:
            is->rc.dy = 30;
            break;

         case ODA_DRAW_TITLE:
            IDISPLAY_DrawText(is->pIDisplay, AEE_FONT_NORMAL, (AECHAR*)L"AppDrawnMenu", -1, is->rc.x, is->rc.y, &is->rc, IDF_ALIGN_NONE|IDF_TEXT_TRANSPARENT);
            IDISPLAY_Update(is->pIDisplay);
            break;

         case ODA_MEASURE_SB:
           {
#ifdef _MCEX_DEBUG
             DBGPRINTF_HIGH("MCEX: received ODA_MEASURE_SB");
#endif//_MCEX_DEBUG

             if(pme->m_ScrollBarInfo.wWidth > 0)
             {
               is->rc.dx = pme->m_ScrollBarInfo.wWidth;
             }
           }
            break;

         case ODA_DRAW_SB:
           // TODO: check local m_nODFlags
#ifdef _MCEX_DEBUG
             DBGPRINTF_HIGH("MCEX: received ODA_DRAW_SB");
#endif//_MCEX_DEBUG

           if(pme->m_ScrollBarInfo.wWidth > 0)
           {
             MCEX_DrawScrollBar(pme, is);
           }
           else
           {
             if(pme->m_pfnCB)
             {
               pme->m_pfnCB(pme->m_pvContext, is);
             }
           }

           // enable scrollbar
           pme->m_bScrollBar = TRUE;
           break;

         case ODA_MEASURE_ITEM:
           {
             CMenuItem* pmi;
             uint16     itemID;

#ifdef _MCEX_DEBUG
             DBGPRINTF_HIGH("MCEX: received ODA_MEASURE_ITEM");
#endif//_MCEX_DEBUG

             itemID = (uint16)(is->nActionInfo >> 16);
             pmi = MCEX_FindItem(pme, itemID);
             if(pmi)
             {
               if(pmi->m_nOWDType == OWD_USER)
               {
                 pme->m_pfnCB(pme->m_pvContext, is);
                 break;
               }
             }
           }
           is->rc.dy = pme->m_cyMaxItem;
           break;

         case ODA_DRAW_ITEM:
           {
             CMenuItem* pmi;
             uint16     itemID;
             boolean    bSlected;

#ifdef _MCEX_DEBUG
             DBGPRINTF_HIGH("MCEX: received ODA_DRAW_ITEM");
#endif//_MCEX_DEBUG

             info = is->nActionInfo;
             info = info << 31;
             info = info >> 31;

             bSlected = (boolean)info;

             itemID = (uint16)(is->nActionInfo >> 16);
             pmi = MCEX_FindItem(pme, itemID);
             if(pmi)
             {
               if(pmi->m_nOWDType == OWD_USER)
               {
                 pme->m_pfnCB(pme->m_pvContext, is);
                 break;
               }

               MCEX_DrawItem(pme, pmi, &is->rc, bSlected, 0);
               if(bSlected && is->rc.dx != 0 && is->rc.dy != 0)
               {
                 //ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);
                 //DBGPRINTF_HIGH("~~~ ODA_DRAW_ITEM cancel timer");

                 MCEX_ResetAutoScroll(pme);
                 MEMCPY(&pme->m_rcAutoScroll, &is->rc, sizeof(AEERect));
               }

             }
           }
           break;
      }

}

// local helper functions

/*=====================================================================

Local Method - Determines whether the image specified is in a resource
or associated with a previously cached version.

======================================================================*/
boolean MCEX_ResolveImage(CMenuCtl * pme, CtlAddItem * pai,const char ** ppszResImage, IImage ** ppImage)
{
   IImage *       pImage = NULL;
   const char *   pszResImage = NULL;
   boolean        bCached = FALSE;
   boolean        bSingleFrame = (boolean)(SINGLE_FRAME(pme) ? TRUE : FALSE);

   pImage = pai->pImage;

   if(!pImage){
      if(pai->pszResImage || (pai->wImage && pai->wImage != 0xffff)){

         // Use image resource file if available. Otherwise use text resource file.
         pszResImage = (const char *)(pai->pszResImage ? pai->pszResImage : pai->pszResText);

         if(!bSingleFrame){
            // Load Image
            if ((pImage = MCEX_LoadImage(pme,pszResImage,pai->wImage,&bCached)) == NULL)
               return(FALSE);
         }
         else{
            // Do not load the image if we are in single frame mode. Just ensure that this is a valid resource.
            if(!ISHELL_IsValidResource(pme->m_pIShell, pszResImage, pai->wImage,RESTYPE_IMAGE,HTYPE_VIEWER))
               return(FALSE);
         }
      }
      else{
         if(IS_ICON_VIEW(pme))
            return(FALSE);                // Need image for icon view
      }
   }
   else
      IIMAGE_AddRef(pImage); // AddRef Image

   *ppszResImage = pszResImage;
   *ppImage = pImage;
   return(TRUE);
}

/*=====================================================================

Local Method - Loads an image for a menu item from either a resource
file or cached version in another item.  If cached, it saves the file
name and increments the reference count on the IImage.

======================================================================*/
IImage * MCEX_LoadImage(CMenuCtl * pme, const char * pszRes, uint16 wID, boolean * pbCached)
{
//   QcList * pl;
   QcNode * pn;
   CMenuItem* pitem;
   // Need valid resource file
   if(!pszRes || !*pszRes)
      return(NULL);

   // Search all items for the same image.  If found, add to it's reference count and return it
   pn = QcListGetFirstNode(pme->m_pItemList);
   do
   {
     if(pn)

     {
       pitem = (CMenuItem*)pn->m_pData;
       if(pitem)
       {
         if(pitem && (pitem->pImage && pitem->pszImage && pitem->wImageID == wID && !STRCMP(pitem->pszImage, pszRes)))
         {
           *pbCached = TRUE;
           IIMAGE_AddRef(pitem->pImage);
           return(pitem->pImage);
         }
       }
       pn = QcListGetNextNode(pn);
     }
   }while(NULL != pn);

      //if(pi->pImage && pi->pszImage && pi->wImageID == wID && !STRCMP(pi->pszImage, pszRes)){
      //   *pbCached = TRUE;
      //   IIMAGE_AddRef(pi->pImage);
      //   return(pi->pImage);
      //}


   *pbCached = FALSE;

   return(ISHELL_LoadResImage(pme->m_pIShell, pszRes, wID));
}

/*=====================================================================

Local Method - Determines the size of an image and deals with animation.

======================================================================*/
void MCEX_SetupImage(CMenuCtl * pme, CMenuItem * pmi, IImage * pImage)
{
   AEEImageInfo   ii;

   IIMAGE_GetInfo(pImage, &ii);

   pmi->cxImage = ii.cxFrame;       // cache image width
   pmi->cyImage = ii.cy;            // cache image height

   if(IS_ICON_VIEW(pme) && IS_ANIMATED(pme)){
      if(!ii.bAnimated){
         // This might change width, if animated
         IIMAGE_SetFrameCount(pImage, -1);
         IIMAGE_GetInfo(pImage, &ii);
         pmi->cxImage = ii.cxFrame;
      }
   }

   MCEX_ReleaseObj((void **)&pmi->pImage);
   pmi->pImage = pImage;            // cache image pointer
}

// this function retrieves first menu item with given ID
CMenuItem* MCEX_FindItem(CMenuCtl * pme, uint16 nItemID)
{
   QcList * pl      = pme->m_pItemList;
   QcNode * pn      = NULL;
   CMenuItem* pitem = NULL;

   pn = QcListGetFirstNode(pl);
   do
   {
     if(pn)
     {
       pitem = (CMenuItem*)pn->m_pData;
       if(pitem)
       {
         if(pitem && pitem->nItemID == nItemID)
         {
           return pitem;
         }
       }
       pn = QcListGetNextNode(pn);
     }
   }while(NULL != pn);

   return NULL;
}

void MCEX_DrawItem(CMenuCtl * pme, CMenuItem * p, AEERect * prc, boolean bSel, int nTruncExtent)
{
   int            x,y,cx,cxText,yImage,nFont;
   boolean        bSelect = bSel;
   boolean        b3State;
   boolean        bMulti;
   IDisplay    *  pd = pme->m_pIDisplay;
   AECHAR *       pText = p->pText;
   AEEItemStyle   style;
   boolean        bTitleRightAligned = FALSE;
   AEERect        rcText;
   uint32         dwItemTextAlignment = 0;
   AEEMenuColors  color;

   RGBVAL         cText;  // default text color
   RGBVAL         cFrame; // default frame color

   if(MCEX_IsPtInRect((int)pme->m_nPointerPosX, (int)pme->m_nPointerPosY, &pme->m_rcScrollBar))
   {
#ifdef _MCEX_DEBUG
     DBGPRINTF_HIGH("MCEX: MCEX_DrawItem(), bSel = %d, set bSelect to false*****.", bSel);
#endif //#ifdef _MCEX_DEBUG
     bSelect = FALSE;
   }

   (void)MCEX_GetMenuDisplayRect(pme->m_pIMenu, &pme->m_rcMenu);

#ifdef _MCEX_DEBUG
   {
     char buf[64];

     DBGPRINTF_HIGH("*****MCEX: MCEX_DrawItem(),  rc(%d, %d, %d, %d).", pme->m_rcMenu.x, pme->m_rcMenu.y, pme->m_rcMenu.dx, pme->m_rcMenu.dy);
     
     (void)WSTRTOSTR( p->pText, buf, sizeof(buf));
     DBGPRINTF_HIGH("*****MCEX: MCEX_DrawItem() item, rc(%d, %d, %d, %d), sel = %d, id = %d, text = %s.",
       prc->x, prc->y, prc->dx, prc->dy, bSelect, p->nItemID, buf);
   }
#endif //#ifdef _MCEX_DEBUG

   // get colors
   IMENUCTL_GetColors(pme->m_pIMenu, &color);

   if(bSelect && !pme->m_bPtMove)
   {
     cText = IDISPLAY_SetColor(pd, CLR_USER_TEXT, pme->m_ItemInfo.cTextSel);
   }
   else
   {
     cText =  IDISPLAY_SetColor(pd, CLR_USER_TEXT, pme->m_ItemInfo.cText);
   }
   //IDISPLAY_SetColor(pd, CLR_USER_TEXT, color.cText);
   cFrame = IDISPLAY_SetColor(pd, CLR_USER_FRAME, color.cFrame);

   if(bSelect)
   {
     // determin if the text needs to be scrolling
     pme->m_bAutoScroll = MCEX_ItemScrolls(pme, p, 0);
#ifdef _MCEX_DEBUG
     DBGPRINTF_HIGH("*****MCEX: pme->m_bAutoScroll = %d.", pme->m_bAutoScroll);
#endif //#ifdef _MCEX_DEBUG

     // If item is selected and m_bAutoScroll is set, update position to start drawing
     // text from
     if(pme->m_bAutoScroll)
     {
        MEMCPY(&pme->m_rcAutoScroll, prc, sizeof(AEERect));
     }
   }

   //{
   //  AEERect rcMenu;
   //  IMENUCTL_GetRect(pme->m_pIMenu, &rcMenu);
   //  if(pme->m_ItemInfo.pImage)
   //  {
   //    AEEImageInfo   ii;

   //    IIMAGE_GetInfo(pme->m_ItemInfo.pImage, &ii);
   //    // resize and draw thumb image
   //    IIMAGE_SetParm(pme->m_ItemInfo.pImage, IPARM_SCALE, rcMenu.dx, rcMenu.dy );
   //    IIMAGE_Draw(pme->m_ItemInfo.pImage, rcMenu.x , rcMenu.y);
   //  }

   //}

   // If item is selected and m_bAutoScroll is set, update position to start drawing
   // text from
   if(bSelect && !pme->m_bPtMove)
   {
     if(pme->m_ItemInfo.pImageSelect)
     {
       AEEImageInfo   ii;

       IIMAGE_GetInfo(pme->m_ItemInfo.pImageSelect, &ii);
       // resize and draw thumb image
       IIMAGE_SetParm(pme->m_ItemInfo.pImageSelect, IPARM_ROP, AEE_RO_MASK, 0);
       IIMAGE_SetParm(pme->m_ItemInfo.pImageSelect, IPARM_SCALE, prc->dx, prc->dy );
       IIMAGE_Draw(pme->m_ItemInfo.pImageSelect, prc->x , prc->y);
     }
     else
     {
       IDISPLAY_FillRect(pme->m_pIDisplay, prc, color.cSelBack);
       IDISPLAY_FrameRect(pd, prc);
     }
   }
   else
   {
     if(pme->m_ItemInfo.pImage)
     {
       AEEImageInfo   ii;

       IIMAGE_GetInfo(pme->m_ItemInfo.pImage, &ii);
       // resize and draw thumb image
       IIMAGE_SetParm(pme->m_ItemInfo.pImage, IPARM_ROP, AEE_RO_MASK, 0);
       IIMAGE_SetParm(pme->m_ItemInfo.pImage, IPARM_SCALE, prc->dx, prc->dy );
       IIMAGE_Draw(pme->m_ItemInfo.pImage, prc->x , prc->y);
     }
     else
     {
       IDISPLAY_FillRect(pme->m_pIDisplay, prc, color.cBack);
       IDISPLAY_FrameRect(pd, prc);
     }
   }


   // get stype
   IMENUCTL_GetStyle(pme->m_pIMenu, bSelect ? NULL: &style, bSelect ? &style: NULL);

   // icons are drawn according to the alignment of the menu title ...
   if (pme->m_pTitle) {
      bTitleRightAligned = (ParagraphAlignment(pme, pme->m_pTitle, WSTRLEN(pme->m_pTitle))) == IDF_ALIGN_RIGHT;
   }

   b3State = (boolean)(IS_MULTI_IMAGE(pme) ? TRUE : FALSE);
   bMulti  = (boolean)(IS_MULTI_SEL(pme) ? TRUE : FALSE);

//   MCEX_AdjustRect(prc,&style);

   x = prc->x;
   y = prc->y;
   cx = prc->dx;

   cxText = p->cxText;

   if(p->pImage){
      IIMAGE_SetParm(p->pImage, IPARM_ROP, (b3State ? AEE_RO_MASK : style.roImage), 0);

      // If no text and its softkey control, center the image...

      if(!cxText && IS_SOFTKEY_MENU(pme))
         x += ((prc->dx - p->cxImage) / 2);

      // If text and a right aligned title, draw the image on the right
      if (cxText && bTitleRightAligned) {
         x += cx - p->cxImage;
      }

      yImage = y + ((prc->dy - p->cyImage) / 2);

      //if(b3State)
      //   IIMAGE_DrawFrame(p->pImage, (p->bPressed ? 2 : (bSel ? 1 : 0)), x, yImage);
      //else
         IIMAGE_Draw(p->pImage, x , yImage);

      if(cxText){
         x += MENU_IMAGE_PAD + p->cxImage;
         cx -= p->cxImage + MENU_IMAGE_PAD;
      }
   }

  if(cxText){
      // For softkey, we draw text in center of remaining space
    if(pme->m_cls == AEECLSID_SOFTKEYCTLEX && cxText < cx)
      x += (cx - cxText) / 2;

      // Bidirectional Text Support      -jdboyd 05/09/03
      //
      // In addition to the changes above to draw the icon on the right side of menus that
      // are right aligned, the code below draws the correct portion of the text in the
      // menu item's rectangle.
      //
      // In all cases, the clipping rectangle is computed from the item's rectangle while
      // taking the size and position of the icon into account if one exists.
      //
      // Left to Right Text
      //
      // The menu item text to be drawn is drawn Left Aligned if it will fit.
      // If it does not fit, then as much text that will fit is drawn from the starting
      // coordinate, clipped by the text rectangle.  The autoscroll value, which  contains
      // a pixel offset rather than a character count, is used to determine where to
      // start drawing the text.  As only text within the rectangle actually appears on
      // screen, this has the effect of scrolling the item to the left.
      //
      // Right to Left Text
      //
      // The menu item text to be drawn is drawn Right ALigned if it will fit.
      // If it does not fit, then only that portion of the text starting at the rightmost
      // end of the entire text string is drawn.  A block is drawn starting at the
      // rightmost end of the text, incrementing to the left each time the AutoScroll
      // routine fires.  This makes right aligned menu items scroll to the right.
      //
      // Note, when testing this without a DrawText implementation that does not render text
      // right to left, expect to see the following:  The last characters in the menu
      // item string are drawn first, then scrolls to the right until the first character
      // fits on screen.  In other words, you'll see a reverse scroll of the case above.

      // Adjust start and width of text if we drew a right aligned icon
      if (IS_MENU(pme) && p->pImage) {
         if (bTitleRightAligned ) {
            x = prc->x;
         }
      }
      // Use the MenuItem font instead of default MenuCtl font
      nFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, p->nFont, NULL, NULL);
      // use pme->m_cyFont if EFAILED is returned by IDISPLAY_GetFontMetrics
      if (nFont == EFAILED) {
         nFont = pme->m_cyFont;
      }
      y += ((prc->dy - nFont) / 2);

      // adjust the clipping rectangle for the text
      rcText = *prc;
      if (IS_MENU(pme) && p->pImage) {
         if (!bTitleRightAligned)
            rcText.x += p->cxImage + MENU_IMAGE_PAD;
         rcText.dx -= p->cxImage + MENU_IMAGE_PAD;
      }

      // determine the direction of this menu item's text
      dwItemTextAlignment = ParagraphAlignment(pme, pText, WSTRLEN(pText));

      // determine start position of text based on auto scroll, clipping rectangle and alignment
      if (MCEX_ItemScrolls(pme, p, 0)) {
         if (dwItemTextAlignment == IDF_ALIGN_LEFT) {
            // adjust the start position of the text only if it's selected
            if (bSelect)
            x -= pme->m_nAutoScrollIdx;
         } else {
            // right aligned text, always adjust the start position.  But don't
            // factor in the auto scroll index if we're not selected.
            x -= IDISPLAY_MeasureText(pme->m_pIDisplay, p->nFont, pText) - rcText.dx - (bSelect ? pme->m_nAutoScrollIdx : 0);
         }
      }

      //// draw the entire text.  Align it unless it's scrolling or a softkey.
      if (MCEX_ItemScrolls(pme, p, 0) || (pme->m_cls == AEECLSID_SOFTKEYCTLEX))
         IDISPLAY_DrawText(pd, p->nFont, pText, -1, x, y, &rcText, IDF_TEXT_TRANSPARENT);
      else
         IDISPLAY_DrawText(pd, p->nFont, pText, -1, x, y, &rcText, dwItemTextAlignment|IDF_TEXT_TRANSPARENT);
  }

   // Reset the background color - it may have changed above...

   //IDISPLAY_SetColor(pd,CLR_USER_BACKGROUND,cBack);

   // draw top and bottom border line
   if(pme->m_pIGraphics)
   {
     AEELine  line;
     uint32  c;
     uint8   r,g,b;
     AEEMenuColors pc;

     //IDISPLAY_DrawHLine(pme->m_pIDisplay, 0, rRect.y, rRect.dx);
     //IDISPLAY_DrawHLine(pme->m_pIDisplay, 0, rRect.y + rRect.dy, rRect.dx);

     IMENUCTL_GetColors(pme->m_pIMenu, &pc);

     c = (uint32)pme->m_cBorderFrame;
     r = (uint8)((c << 16) >> 24);
     g = (uint8)((c << 8) >> 24);
     b = (uint8)(c >> 24);

     IGRAPHICS_SetColor(pme->m_pIGraphics, r, g, b, 1); 
 
     line.sx = 0;
     line.ex = pme->m_rcMenu.dx;
 
     // top line
     line.sy = pme->m_rcMenu.y;
     line.ey = pme->m_rcMenu.y;
     (void)IGRAPHICS_DrawLine(pme->m_pIGraphics, &line);
 
     // bottom line
     line.sy = pme->m_rcMenu.y + pme->m_rcMenu.dy;
     line.ey = pme->m_rcMenu.y + pme->m_rcMenu.dy;
     (void)IGRAPHICS_DrawLine(pme->m_pIGraphics, &line);
   }

   // restore default text color
   (void)IDISPLAY_SetColor(pd, CLR_USER_TEXT, cText);
   // restore default frame color
   (void)IDISPLAY_SetColor(pd, CLR_USER_FRAME, cFrame);
}

IImage* MCEX_LoadImageEx(CMenuCtl * pme, const char* pszResImage, uint16 wImage, IImage * pImage)
{
  IImage* pi = NULL;

  if(!pImage)
  {
    if(pszResImage && (wImage && wImage!= 0xffff))
    {
      pi = ISHELL_LoadResImage(pme->m_pIShell, pszResImage, wImage);
    }
  }
  else
  {
    IIMAGE_AddRef(pImage);
    pi = pImage;
  }

  return pi;
}

void MCEX_DrawScrollBar(CMenuCtl * pme, AEEODCBData* is)
{
  AEERect rc;

  // draw scrollbar fram
  SETAEERECT(&pme->m_rcScrollBar, is->rc.x, is->rc.y, is->rc.dx, is->rc.dy);

  if(pme->m_ScrollBarInfo.pImageScrollbar)
  {
    AEEImageInfo   ii;

    IIMAGE_GetInfo(pme->m_ScrollBarInfo.pImageScrollbar, &ii);
    // resize and draw thumb image
    IIMAGE_SetParm(pme->m_ScrollBarInfo.pImageScrollbar, IPARM_ROP, AEE_RO_MASK, 0);
    IIMAGE_SetParm(pme->m_ScrollBarInfo.pImageScrollbar, IPARM_SCALE, pme->m_rcScrollBar.dx, pme->m_rcScrollBar.dy );
    IIMAGE_Draw(pme->m_ScrollBarInfo.pImageScrollbar, pme->m_rcScrollBar.x , pme->m_rcScrollBar.y);
  }
  else
  {
    IDISPLAY_DrawFrame(is->pIDisplay, &pme->m_rcScrollBar, AEE_FT_BOX, pme->m_ScrollBarInfo.cScrollbar);
  }

  // calculate thumb rc
  SETAEERECT(&rc, is->rc.x, OD_SB_POS(is->nActionInfo), is->rc.dx, OD_SB_LEN(is->nActionInfo));
  if(pme->m_ScrollBarInfo.pImageThumb)
  {
    AEEImageInfo   ii;

    IIMAGE_GetInfo(pme->m_ScrollBarInfo.pImageThumb, &ii);
    IIMAGE_SetParm(pme->m_ScrollBarInfo.pImageThumb, IPARM_ROP, AEE_RO_MASK, 0);
    // resize and draw thumb image
    IIMAGE_SetParm(pme->m_ScrollBarInfo.pImageThumb, IPARM_SCALE, rc.dx,rc.dy );
    IIMAGE_Draw(pme->m_ScrollBarInfo.pImageThumb, rc.x , rc.y);
  }
  else
  {
    // fill thumb rc
    IDISPLAY_FillRect(is->pIDisplay, &rc, pme->m_ScrollBarInfo.cThumb);
  }
}
/*=====================================================================

Local Method - Recalculates the size information in the menu.

======================================================================*/
void MCEX_Recalc(CMenuCtl * pme)
{
   CMenuItem *    pi;
   int            cy,cx,cyMax,cxMax,cys,cxs,nFrame,cxMaxItem,cyMaxItem;
   AEERect        rc;
   QcNode * pn;

   IMENUCTL_GetRect(pme->m_pIMenu, &rc);

//   int nFont;
   cxMax = cyMax = 0;

//   pme->m_bHasText = FALSE;

   // Get extra pixels added to height and width because of style
   MCEX_GetStyleOffsets(pme, &cxs, &cys);

   nFrame = (uint16)(AEE_FRAME_SIZE * 2);
  cxMaxItem = rc.dx - cxs - nFrame;     // Max item width
  cyMaxItem = rc.dy - cys - nFrame;     // Max item height

   if(SINGLE_FRAME(pme)){
      cxMax = cxMaxItem;                        // Width of widest item - Max item width
      cyMax = cyMaxItem;                        // Height of tallest item - Max item height
   }


   // Search all items for the same image.  If found, add to it's reference count and return it
   pn = QcListGetFirstNode(pme->m_pItemList);
   do
   {
     if(pn)
     {
       pi = (CMenuItem*)pn->m_pData;
       if(pi)
       {

         // Measure Item Text Width
         if (!pi->cxText && pi->pText)
           pi->cxText = IDISPLAY_MeasureText(pme->m_pIDisplay, pi->nFont, pi->pText);

      // Icon Views only measure the image. The text is shown as a title or below the menu.
      if(IS_ICON_VIEW(pme)){
         cx = pi->cxImage;
         cy = pi->cyImage;

         if (pi->m_nOWDType == OWD_USER)
         {
            // Adjust for offset so that comparison is fair
            cx -= cxs;
            cy -= cys;
         }

         // Make sure that we avoid going off-screen
         if(cy > cyMaxItem)
            cy = cyMaxItem;

         if(cx > cxMaxItem)
            cx = cxMaxItem;
      }
      else{
         // Item Width (Includes Text and Padding)
         cx = (pi->m_nOWDType == OWD_USER) ? pi->cxImage : MCEX_GetItemWidth(pi);

         // Bigger of font/image height
         cy = (int)(pi->pText ? pme->m_cyFont : 0);
         if(pi->cyImage > cy)
            cy = pi->cyImage;

         if (pi->m_nOWDType == OWD_USER)
         {
            // Adjust for offset so that comparison is fair
            cx -= cxs;
            cy -= cys;
         }
      }

      // Need tallest/widest
      if(cx > cxMax)
         cxMax = cx;
      if(cy > cyMax)
         cyMax = cy;

      //if(pi->cxText || pi->cyText)
      //   pme->m_bHasText = TRUE;
       }
       pn = QcListGetNextNode(pn);
     }
   }while(NULL != pn);


   cxMax += cxs;              // Add Width Offset
   cyMax += cys;              // Add Height Offset

   pme->m_cxMaxItem = cxMax;  // Width of widest item
   pme->m_cyMaxItem = cyMax;  // Height of tallest item

   switch(pme->m_cls){
      case AEECLSID_ICONVIEWCTLEX:
         // Refresh screen layout for icon view
        // Icon_CalcRows(pme);
         break;

      case AEECLSID_SOFTKEYCTLEX:
         //{
         //   int   nAdd;

         //   // If SK control height isn't same as the height of tallest item, adjust it
         //   if(rc.dy != cyMax){
         //      nAdd = rc.dy - cyMax;
         //      rc.dy = cyMax;
         //      rc.y += nAdd;
         //   }

         //   // If max SK items are to be displayed, calculate page items using width of
         //   // the widest item
         //   if(MAX_SK_ITEMS(pme)){
         //      if(cxMax)
         //         pme->m_nPageItems = rc.dx / cxMax;
         //      else
         //         pme->m_nPageItems = __MIN(pme->m_nItems, MAX_SOFT_PAGE_ITEMS);
         //   }
         //}
         break;

      default:
        break;

         }

}

/*=====================================================================

Local Method - Returns the width of a menu item

======================================================================*/
int MCEX_GetItemWidth(CMenuItem * pi)
{
   int   cx;

   cx = pi->cxText;  // Text Width

   // Add padding space and image width
   if(pi->cxImage){
      if(cx)
         cx += MENU_IMAGE_PAD;
      cx += pi->cxImage;
   }
   return(cx);
}

/*=====================================================================

Local Method - Adjusts the size of the rect based upon the style.

======================================================================*/
void MCEX_AdjustRect(AEERect * prc, AEEItemStyle * ps)
{
   prc->x += ps->xOffset;
   prc->y += ps->yOffset;
   prc->dx -= (ps->xOffset * 2);
   prc->dy -= (ps->yOffset * 2);
}

/*=====================================================================

Local Method - Returns the style offset for a menu.

======================================================================*/
void MCEX_GetStyleOffsets(CMenuCtl * pme,int * pcx, int * pcy)
{
   int            nFrame = 0, cxMax = 0, cyMax = 0, cx, cy, i;
   AEEItemStyle   style[2];
   AEEItemStyle * ps = style;

   IMENUCTL_GetStyle(pme->m_pIMenu, &style[0], &style[1]);

   // For styles (Normal/Select), calculate max pixels added to width &
   // height by any of these
   for(i = 0; i < sizeof(style)/sizeof(AEEItemStyle); i++, ps++){
      // Calculate pixels required to draw frame
    nFrame = IDISPLAY_DrawFrame(pme->m_pIDisplay, NULL, ps->ft, RGB_NONE);

      cx = (ps->xOffset + nFrame) * 2;
      cy = (ps->yOffset + nFrame) * 2;

      if(cx > cxMax)
         cxMax = cx;

      if(cy > cyMax)
         cyMax = cy;
   }

   pme->m_cxItemExtra = cxMax;      // Cache extra pixels added to width
   pme->m_cyItemExtra = cyMax;      // Cache extra pixels added to height

   *pcx = cxMax;                    // Return extra pixels added to width
   *pcy = cyMax;                    // Return extra pixels added to height
}

boolean MCEX_IsPtInRect(int x, int y, AEERect* prc)
{
  if(NULL  == prc)
  {
    DBGPRINTF_ERROR("MCEX: NULL ptr.");
    return FALSE;
  }

  if((x > prc->x) && (x < (prc->x + prc->dx)) &&
     (y > prc->y) && (y < (prc->y + prc->dy)))
  {
    return TRUE;
  }
  return FALSE;
}

boolean MCEX_IsRectEqual(AEERect* prc1, AEERect* prc2)
{

  if(!prc1 || !prc2)
  {
    DBGPRINTF_ERROR("MCEX: NULL ptr.");
    return FALSE;
  }
  return (prc1->x == prc2->x && prc1->y == prc2->y && prc1->dx == prc2->dx && prc1->dy == prc2->dy);

}

static boolean MCEX_IsRcInRc(AEERect* prc1, AEERect* prc2)
{
  if(!prc1 || !prc2)
  {
    DBGPRINTF_ERROR("MCEX: NULL ptr.");
    return FALSE;
  }
  return (prc1->x >= prc2->x &&
          prc1->y >= prc2->y &&
          (prc1->x + prc1->dx)<= (prc2->x + prc2->dx) &&
          (prc1->y + prc1->dy)<= (prc2->y + prc2->dy));
}

boolean MCEX_SetItemText(CMenuCtl * pme, CMenuItem* pitem, const char* pszResText, uint16 wText, AECHAR* pwszText)
{
  AECHAR* pwszBuf;
  int len;

  if(NULL == pitem)
  {
    return FALSE;
  }

  if(pwszText)
  {
    // save item text
    len = __MIN((WSTRLEN(pwszText) + 1), MAX_MENU_TEXT);
    if(len > 1)
    {
      pwszBuf = (AECHAR*)MALLOC(sizeof(AECHAR)*(len + 1));
      if(NULL == pwszBuf)
      {
        DBGPRINTF_ERROR("MCEX: failed to allocate memory.");
        return FALSE;
      }
      MCEX_FreeObj((void **)&pitem->pText);
      pitem->pText = pwszBuf;
      WSTRLCPY(pitem->pText, pwszText, len + 1);
    }
  }
  else if(pszResText && wText)
  {
    // we need to load text from resource, allocate max buffer size
    pwszBuf = (AECHAR*)MALLOC(sizeof(AECHAR)*(MAX_MENU_TEXT + 1));
    if(NULL == pwszBuf)
    {
      DBGPRINTF_ERROR("MCEX: failed to allocate memory.");
      return FALSE;
    }

    // load string now
    if(!ISHELL_LoadResString(pme->m_pIShell, pszResText, wText, pwszBuf, sizeof(AECHAR) * MAX_MENU_TEXT))
    {
       DBGPRINTF_ERROR("MCEX: failed to load string from res.");
       MCEX_FreeObj((void **)&pwszBuf);
       return FALSE;
    }

    MCEX_FreeObj((void **)&pitem->pText);
    pitem->pText = pwszBuf;
  }
  else
  {
    DBGPRINTF_ERROR("MCEX: invalid parm.");
    return FALSE;
  }

  return TRUE;
}

// this function returns rectangle around all visiable menu items
boolean MCEX_GetMenuDisplayRect(IMenuCtl* pMenu, AEERect* prc)
{
  int i, y;
  int cnt;
  int nFirst = -1;
  AEERect rcItem, rcMenu;

  if(NULL == pMenu || NULL == prc)
  {
    return FALSE;
  }

  if(0 == (cnt = IMENUCTL_GetItemCount(pMenu)))
  {
    return FALSE;
  }

  IMENUCTL_GetRect(pMenu, &rcMenu);
  // calculate the last visable item rectangle
  y = 0;
  for(i=0; i < cnt; i++)
  {
    MEMSET(&rcItem, 0,  sizeof(AEERect));
    IMENUCTL_GetItemRect(pMenu, i, &rcItem);
    if(nFirst >= 0 && ((rcItem.y + rcItem.dy) > (rcMenu.y + rcMenu.dy) || ((rcItem.y + rcItem.dy) == 0)))
    {
      break;
    }
    if(nFirst < 0 && (rcItem.y + rcItem.dy) > 0)
    {
      // first visiable item
      nFirst = i;
    }
    y = rcItem.y + rcItem.dy;
  }

  if(nFirst >= 0)
  {
    IMENUCTL_GetItemRect(pMenu, nFirst, &rcItem);
 
    prc->x = rcMenu.x;
    prc->dx = rcMenu.dx;
    prc->y = rcItem.y - 1; // off i pixel to match the menuctl
    prc->dy = y - prc->y;

    return TRUE;
  }

  return FALSE;
}

/*=====================================================================

Local Method - Resets the auto-scroll timer for a menu item.

======================================================================*/
void MCEX_ResetAutoScroll(CMenuCtl * pme)
{
   CMenuItem   * pi;
   uint16      nitemID; // selected item ID

   //// Get current selection
   nitemID = IMENUCTL_GetSel(pme->m_pIMenu);

#ifdef _MCEX_DEBUG
   DBGPRINTF_HIGH("MCEX: MCEX_ResetAutoScroll(), menu item id = %d", nitemID);
#endif //#ifdef _MCEX_DEBUG

   if(NULL == (pi = MCEX_FindItem(pme, nitemID)))
   {
     ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);

#ifdef _MCEX_DEBUG
     DBGPRINTF_HIGH("MCEX: MCEX_ResetAutoScroll(), cancel timer");
#endif //#ifdef _MCEX_DEBUG
     return;
   }

   pme->m_nAutoScrollIdx = 0;

   if(pi && IMENUCTL_IsActive(pme->m_pIMenu) && MCEX_ItemScrolls(pme, pi, 0))
   {
      pme->m_bAutoScroll = TRUE;

#ifdef _MCEX_DEBUG
      DBGPRINTF_HIGH("MCEX: MCEX_ResetAutoScroll(), start text scroll timer ******, item id = %d", nitemID);
#endif //#ifdef _MCEX_DEBUG

      // We set the timer to go off pretty quickly.  That's because the AutoScroll function
      // will automatically set a longer timer for the first and last characters...

      ISHELL_SetTimer(pme->m_pIShell,pme->m_wMenuTextScroll, (PFNNOTIFY)(MCEX_AutoScroll), pme);
   }
   else
   {
#ifdef _MCEX_DEBUG
      DBGPRINTF_HIGH("MCEX:**** MCEX_ResetAutoScroll(), menu item id = %d, timer canceled.", nitemID);
#endif //#ifdef _MCEX_DEBUG

      // Caccel Auto Scroll Timer
      ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);
      pme->m_bAutoScroll = FALSE;

      // Reset Auto Scroll Rect
     // MEMSET(&pme->m_rcAutoScroll, 0, sizeof(AEERect));
   }
}

void MCEX_AutoScroll(CMenuCtl * pme)
{
   CMenuItem * pi;
   int         n,nIdxNew,nIdx;
   AEERect     rc;
   uint16      nitemID;

   if(!pme->m_bAutoScroll/* || pme->m_bScrollBarClick == FALSE*/)
   {
     ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);
#ifdef _MCEX_DEBUG
     DBGPRINTF_HIGH("MCEX: MCEX_AutoScroll(), pme->m_bAutoScroll fause, cancel timer");
#endif //#ifdef _MCEX_DEBUG
     return;
   }
   
   // Find the selected item
   nitemID = IMENUCTL_GetSel(pme->m_pIMenu);
   if(NULL == (pi = MCEX_FindItem(pme, nitemID)))
   {
     return;
   }

#ifdef _MCEX_DEBUG
   {
     char buf[64];

     DBGPRINTF_HIGH("MCEX: -----MCEX_AutoScroll(), Menu rc(%d, %d, %d, %d).", pme->m_rcMenu.x, pme->m_rcMenu.y, pme->m_rcMenu.dx, pme->m_rcMenu.dy);
     (void)WSTRTOSTR( pi->pText, buf, sizeof(buf));
     DBGPRINTF_HIGH("MCEX: -----MCEX_AutoScroll(), Draw item, id = %d, text = %s.",  pi->nItemID, buf);
   }
#endif //#ifdef _MCEX_DEBUG

   if(!MCEX_IsRcInRc(&pme->m_rcAutoScroll, &pme->m_rcMenu))
   {
     // selected rc is invisable
      // Caccel Auto Scroll Timer
      ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);
#ifdef _MCEX_DEBUG
      DBGPRINTF_HIGH("MCEX: MCEX_AutoScroll(), MCEX_IsRcInRc returns fause, cancel timer");
#endif //#ifdef _MCEX_DEBUG

      pme->m_bAutoScroll = FALSE;
      return;
   }

   // See if the index is within range for the string...
   nIdx  = nIdxNew = pme->m_nAutoScrollIdx;
   n     = IDISPLAY_MeasureText(pme->m_pIDisplay, pi->nFont, pi->pText);

   if(nIdx < n){

      // Measure the text starting from the auto-scroll offset
      if(MCEX_ItemScrolls(pme, pi, nIdx))
         nIdxNew += AUTO_SCROLL_PIXELS;
      else
         nIdxNew = 0;

      rc = pme->m_rcAutoScroll;  // Scroll Rect

      if(rc.dx){
         // Initialize CLR_USER_BACKGROUND, CLR_USER_TEXT and CLR_USER_FRAME and
         // save current background, text and frame colors for restoration
//         MCEX_InitColors(pme, &clrBack, &clrText, &clrFrame);

         // Draw Menu Item
         MCEX_DrawItem(pme, pi, &rc, TRUE, 0);

         // Restore Colors
//         ResetColors(pme->m_pIDisplay, clrBack, clrText,clrFrame);
         IDISPLAY_Update(pme->m_pIDisplay);
      }
   }        
   else
      nIdxNew = 0;   // Reset Scroll Character Index

   pme->m_nAutoScrollIdx = nIdxNew;

   // Delay for first/last character
   ISHELL_SetTimer(pme->m_pIShell,(uint32)(nIdx && nIdxNew ? pme->m_wMenuTextScroll : AUTO_SCROLL_START), (PFNNOTIFY)(MCEX_AutoScroll), pme);
}

boolean MCEX_ItemScrolls(CMenuCtl * pme, CMenuItem *pi, int nIdx)
{
   int   cx,cxMax;

   // Measure item text width starting from index
   cx = IDISPLAY_MeasureText(pme->m_pIDisplay, pi->nFont, pi->pText) - nIdx + pme->m_cxItemExtra;

   // Adjust with image width & padding
   if(pi->cxImage){
      if(cx)
         cx += MENU_IMAGE_PAD;
      cx += pi->cxImage;
   }

   cxMax = pme->m_rc.dx;   // Max Width

   // Adjust Max Width for scroll bar
   if (pme->m_bScrollBar)
   {
     cxMax -= pme->m_ScrollBarInfo.wWidth;
   }

   return(cx > cxMax);
}


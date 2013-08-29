/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEEMenu.c

SERVICES:  AEE Menu Interfaces

GENERAL DESCRIPTION:  Provides support for IMenuCtl services
in AEE.  These include:

- Menus
- Lists
- Icon Views
- Calendar List View
- Softkey Menus

PUBLIC CLASSES AND STATIC FUNCTIONS: IMenuCtl

Copyright ? 1999-2005 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/
 
/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "AEEMenu_priv.h"
#include "AEEStatic_priv.h"
#include "AEEModTable.h"
#include "AEEPointerHelpers.h"
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
#include "nv.h"
#endif
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
#include "Appscommon.h"
#include "appscommonimages.brh"
#include "err.h"
#endif //defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    
/*===========================================================================

DEFINITIONS

===========================================================================*/
#define __MAX(x,y)            ((x) > (y) ? (x) : (y))       // Max of X and Y
#define __MIN(x,y)            ((x) < (y) ? (x) : (y))       // Min of X and Y

#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
static int IMAGE_WIDTH           = 5;                           // Frame Width for AEE_IDB_SMALLFONTS
#else
#define IMAGE_WIDTH       (4)                           // Frame Width for AEE_IDB_SMALLFONTS   
#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

#define SOFT_PAGE_ITEMS       (3)                           // Initializes # of soft key items visible at once
#define MAX_SOFT_PAGE_ITEMS   (12)                          // Max SK items/screen
#ifdef FEATURE_DISP_240X320
#define CXARROW               (5)                           // Arrow Width
#define CYARROW               (10)                           // Arrow Height
#define ARROWMARGIN           (10)                           // Spacing for arrow (margins and pad)
#else
#define CXARROW               (3)                           // Arrow Width
#define CYARROW               (5)                           // Arrow Height
#define ARROWMARGIN           (5)                           // Spacing for arrow (margins and pad)
#endif
#define CELL_LMARGIN          (1 + CXARROW + 1)             // Left Margin - space, arrow, space, border
#define CELL_RMARGIN          CELL_LMARGIN                  // Right Margin - same as left margin
#define CELL_SIDE_WIDTH       (CELL_LMARGIN + CELL_RMARGIN) // space taken up by both cell margins
#define MIN_CELL_WIDTH        (10)

#define MENU_IMAGE_PAD        (5)                           // Space between image and menu text
#define LIST_TITLE_PAD        (5)                           // Space between list title and text
#define AUTO_SCROLL_TIME      (150)                         // Auto Scroll - Update Delay when Device Info shows zero
#define AUTO_SCROLL_PIXELS    (6)                           // number of pixels to scroll during autoscroll
#define IMAGE_DELAY_TIME      (200)                         // Image Draw Delay when Device Info shows zero delay
#define AUTO_SCROLL_START     (1000)                        // Auto Scroll - (Re)Start Delay
#define ARROW_LINES           (5)
#define INVALID_MARK          (-100)
#ifdef FEATURE_MENUTITLE_AUTOSCROLL
#define MENUTEXT_XOFFSET      (0)
#define MENU_SBWIDTH_XPAD     (0)
#endif //FEATURE_MENUTITLE_AUTOSCROLL

#if defined(FEATURE_DISP_160X128)
#define MAX_NUM_MENUPOP                 (6) //(6)
#elif defined(FEATURE_DISP_128X160)
#define MAX_NUM_MENUPOP                 (8) //(6)
#elif defined(FEATURE_DISP_128X128)
#define MAX_NUM_MENUPOP                 (6) //(6)
#elif defined(FEATURE_DISP_176X220)
#define MAX_NUM_MENUPOP                 (8) //(6)
#elif defined(FEATURE_DISP_220X176)
#ifdef FEATURE_VERSION_EC99
#define MAX_NUM_MENUPOP                 (4) //(6)
#else
#define MAX_NUM_MENUPOP                 (6) //(6)
#endif
#elif defined(FEATURE_DISP_240X320)
#ifdef FEATURE_VERSION_K212
#define MAX_NUM_MENUPOP                 (6) //(6)
#else
#define MAX_NUM_MENUPOP                 (9) //(6)
#endif
#elif defined(FEATURE_DISP_320X240)
#define MAX_NUM_MENUPOP                 (8) //(6)
#else
#define MAX_NUM_MENUPOP                 (6) //(6)
#endif

#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

/*===========================================================================

Class Definitions

===========================================================================*/
// Pen Tracking Hit flags 8-bit (1 byte)
#define  PTRCK_HIT_ABOVE   (0x01)      // Above the thumb, y position < thumb.y OR x position < thumb.x
#define  PTRCK_HIT_THUMB   (0x02)      // On the thumb
#define  PTRCK_HIT_ITEM    (0x02)      // Matches above, overloaded for non SB entities
#define  PTRCK_HIT_BELOW   (0x04)      // Below the thumb, y position > thumb.y + thumb.dy OR x position < thumb.x + thumb.dx
#define  PTRCK_HIT_SCRL    (0x08)      // 1: Hit Scrollbar; 0: Didn't hit Scrollbar
#define  PTRCK_NOTTHUMB    (PTRCK_HIT_BELOW | PTRCK_HIT_ABOVE)

// Pen Tracking General flags
#define  PTRCK_GEN_TMRSET  (0x01)   // Timer is set

typedef struct
{
   uint8    cbHit;
   uint8    cbFlags;
   uint16   wData;
   AEEPoint ptPosition;
} PenTracker;

// PenTracker::cbHit is:
// SK:
// PTRCK_HIT_ABOVE - Left arrow
// PTRCK_HIT_ITEM - An Item was hit
// PTRCK_HIT_BELOW - Right arrow
// PenTracker::wData is:
// Index of Selection
//
// List:
// PTRCK_HIT_ITEM - An Item was hit
// PenTracker::wData is:
// Unused
// 
// Menu and IconView:
// if( !(PenTracker::cbFlags & PTRCK_HIT_SCRL) )
// PTRCK_HIT_ITEM - An Item was hit
// PenTracker::wData is:
// Index of Selection
//
// if( PenTracker::cbFlags & PTRCK_HIT_SCRL )
// PTRCK_HIT_ABOVE - Above thumb
// PTRCK_HIT_THUMB - On Thumb
// PTRCK_HIT_BELOW - Below thumb
// PenTracker::wData is:
// Offset hit inside Thumb


OBJECT(CMenuItem)
{
   CMenuItem *    pNext;         // Pointer to next item in the list
   uint32         lData;         // 32-bit user data value
   uint16         nItemID;       // Item ID
   AEEFont        nFont;         // Font for text
   uint16         wCalStart;     // Calendar item start time in minutes
   uint16         wCalEnd;       // Calendar item end time in minutes
   uint16         y;             // Y position on screen
   uint16         cxText;        // Text length (pixels)
   uint16         cyImage;       // Height of image
   uint16         cxImage;       // Width of image
   const char *   pszImage;      // Resource file for cached image
   uint16         wImageID;      // Image ID in resource file
   IImage *       pImage;        // Image object
   boolean        bPressed;      // Down/Up state
   boolean        bIsSel;        // Is item sel, for multi sel
   boolean        bOD;           // Item is owner drawn
   uint16         cyText;        // Text Height for owner drawn icon view
   AECHAR         text[1];       // text starts here
};

//
// Structure for softkey menu item layout
//

OBJECT(SKDrawItem) {
   int         x;                // item cell x position
   int         dx;               // item cell width
   int         xTruncatedExtent; // Width of truncated item text
   int         cbFits;           // Number of item text characters which fit in item width
   boolean     bTrunc;           // Flag to indicate if item text is truncated
   boolean     bSelected;        // Item is selected
   CMenuItem * pmi;              // Menu Item
};

//
// Menu manager object (IMenuCtl)
//
OBJECT(CMenuCtl)
{
   DECLARE_VTBL(IMenuCtl)

   // private object data;

   long           m_nRefs;                   // reference count

   flg            m_bActive:1;               // control is active flag
   flg            m_bSendCmd:1;              // enable sending of EVT_COMMAND event
   flg            m_bIconVScroll:1;          // Multi-line icon view - Vertical Scroll
   flg            m_bIconHScroll:1;          // Single-line icon view - Horizontal Scroll
   flg            m_bHasText:1;              // Menu items have text
   flg            m_bAutoScroll:1;           // Auto Scroll
   flg            m_bColor:1;                // If color screens - color depth > 1
   flg            m_bSKPageFullNotify:1;     // Set if the SK menu has displayed a full page
   flg            m_bResumeCmd:1;            // Set if using async EVT_COMMAND posting - Post-1.0

   IImage *       m_pFont;                   // Holds AEE_IDB_SMALLFONTS for Calendar Menu
   int            m_yLastTimeMark;           // Previous position of mark that indicates current time

   int16          m_nSBWidth;                // Scroll Bar Width (cxScrollBar of AEEDeviceInfo)

   uint32         m_dwProps;                 // menu control properties
   uint32         m_dwOemProps;              // menu control Oem properties
   AEECLSID       m_cls;                     // menu control class
   CMenuItem *    m_pItemList;               // list of menu items
   IShell *       m_pIShell;                 // IShell interface ptr
   IDisplay *     m_pIDisplay;               // IDisplay interface ptr
   uint16         m_nItems;                  // number of items in list
   uint16         m_nSelect;                 // index of current menu selection
   uint16         m_nScrollItem;             // index of current scroll position (row index for grid icon view, item index otherwise)
   uint16         m_nPageItems;              // number of items that can fit in a page
   int            m_nAscent;                 // ascent in m_cyFont font
   int            m_cyFont;                  // height of char cell of menu font 
   AECHAR *       m_pTitle;                  // menu title
   int            m_nVisibleItems;           // number of items displayed at current scroll position

   SKDrawItem     m_draw[MAX_SOFT_PAGE_ITEMS];

   // Styles
   AEEItemStyle   m_style[2];                // Style for normal and selection
   uint16         m_cxItemExtra;             // Extra pixels added to width for style
   uint16         m_cyItemExtra;             // Extra pixels added to height for style

   AECHAR         m_szEllipses[2];           // Holds AEE_SYM_ELLIPSES for Normal Font
   int            m_cxDotWidth;              // Holds Ellipses symbol width for Bold Font

   // Icon View Stuff
   int            m_nTotalRows;              // Total number of icon rows
   int            m_nCols;                   // Number of displayed icon cols
   int            m_nRows;                   // Number of displayed icon rows
   int            m_xImagePad;               // Horizontal padding around item icon
   int            m_yImagePad;               // Vertical padding around item icon

   int            m_cxMaxItem;               // Max width of a menu item
   int            m_cyMaxItem;               // Max height of a menu item

   AEERect        m_rc;                      // control rectangle
//   AEERect        m_seltextrc;               // 被选中项显示文本部份矩形大小
   AEEPoint       m_ptPrevious;  // Previous pen position to facilitate scroll

   // Auto-Scroll stuff
   int            m_nAutoScrollIdx;          // Auto Scroll - Scroll Character Index
   AEERect        m_rcAutoScroll;            // Auto Scroll Rect
   uint32         m_wMenuTextScroll;         // Auto Scroll - Update Timer
   uint32         m_wMenuImageDelay;         // Delay for SINGLE_FRAME Icon View with MP_ICON_DELAY_IMAGE property
   
   AEEMenuColors  m_c;                       // Control Colors

   // Posted-COMMANDs   
   AEECallback    m_cbCommand;               // Callback for posting EVT_COMMAND

   CMenuItem *    m_pEnum;                   // Used for Menu Item enumeration
   CMenuItem *    m_pLastItem;               // Last Item in List, quicker AddItem when Dupe opt on!

   // Owner Draw Stuff
   PFNOWNERDRAWCB m_pfnCB;                   // Callback for Owner Draw
   void *         m_pvContext;               // Context for Owner Draw Callback
   uint16         m_nODFlags;                // Owner Draw Flags
   AEERect        m_rcTitle;                 // Title Rect

   // Pen handling memebers
   PenTracker     m_ptTracker;
   AEEAutoRepeat  m_arPenRepeat;
   
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

    IImage *imageSelectBar;
    BottomBar_e_Type bottomBarType;
    boolean userSetStyle;
    boolean SetPopMenu;
    
    int    theDrawnItem; // the item is currently drawn
#endif
#ifdef FEATURE_CUSTOMIZED_MENU_STYLE
    IImage *m_pBgImage; // for OEMMP_GRAPHIC_BG
#ifdef FEATURE_RANDOM_MENU_COLOR
    byte m_nRandomMenu; // for CFGI_RANDOM_MENU
#endif
#endif
#ifdef FEATURE_MENUTITLE_AUTOSCROLL
    int     m_nTitleAutoScrollIdx;
    boolean m_bTitleAutoScroll;
#endif //FEATURE_MENUTITLE_AUTOSCROLL
    boolean m_bAutoScrollMenuItem;
#ifdef FEATURE_CUSTOMIZED_MENU_STYLE
    uint16  nBgImgResID;
    char    strBgImgResFile[MAX_FILE_NAME];
#endif
    int     m_menuReallyDy;//add by xuhui
};

typedef struct 
{
   AEEMenuSortType      t;
   PFNMENUSORTCOMPARE   pfn;
   void *               pUser;
} MSortArgs;

typedef struct
{
   CMenuItem *          pmi;
   MSortArgs *          pa;
} MSortItem;

#define IS_PROP_SET(dw,dwProp)   ((dw) & (dwProp))

#define WRAP_SCROLL(p)        IS_PROP_SET(p->m_dwProps,MP_WRAPSCROLL)
#define MAX_SK_ITEMS(p)       IS_PROP_SET(p->m_dwProps,MP_MAXSOFTKEYITEMS)
#define NO_REDRAW(p)          IS_PROP_SET(p->m_dwProps,MP_NO_REDRAW)
#define NO_ACTIVE_REDRAW(p)   IS_PROP_SET(p->m_dwProps,MP_ACTIVE_NO_REDRAW)
#define IS_CALENDAR_VIEW(p)   IS_PROP_SET(p->m_dwProps,MP_CALENDAR)
#define AUTO_SCROLL(p)        IS_PROP_SET(p->m_dwProps,MP_AUTOSCROLLTIME)
#define ALIGN_TEXT_TOP(p)     IS_PROP_SET(p->m_dwProps,MP_ICON_TEXT_TOP)
#define NO_ARROWS(p)          IS_PROP_SET(p->m_dwProps,MP_NO_ARROWS)
#define SINGLE_FRAME(p)       IS_PROP_SET(p->m_dwProps,MP_ICON_SINGLE_FRAME)
#define UNDERLINE(p)          IS_PROP_SET(p->m_dwProps,MP_UNDERLINE_TITLE)
#define IS_ANIMATED(p)        IS_PROP_SET(p->m_dwProps,MP_ICON_ANIMATED)
#define IS_MULTI_IMAGE(p)     IS_PROP_SET(p->m_dwProps,(MP_TRI_STATE_IMAGE|MP_BI_STATE_IMAGE))
#define IS_TRI_STATE(p)       IS_PROP_SET(p->m_dwProps,MP_TRI_STATE_IMAGE)
#define DELAY_IMAGE(p)        IS_PROP_SET(p->m_dwProps,MP_ICON_DELAY_IMAGE)
#define IS_MULTI_SEL(p)       IS_PROP_SET(p->m_dwProps,MP_MULTI_SEL)
#define IS_ALLOWING_DUPE(p)   IS_PROP_SET(p->m_dwProps,MP_ALLOW_DUPE_IDS)

#define IS_OD_CB(p)           ((p)->m_pfnCB)
#define IS_TITLE_OD(p)        ((p)->m_nODFlags & ODF_TITLE_OD)
#define IS_SB_OD(p)           ((p)->m_nODFlags & ODF_SB_OD)
#define OD_ITEMS_ALLOWED(p)   ((p)->m_nODFlags & ODF_ITEM_OD)
#define IS_MEASURE_ONCE(p)    ((p)->m_nODFlags & ODF_MEASURE_ONCE)            
#define IS_ITEM_OD(pi)        ((pi)->bOD)

#define ITEM_IS_VISIBLE(p,n)  (n >= (p)->m_nScrollItem && n < (p)->m_nScrollItem + (p)->m_nPageItems)
#define ICON_IS_VISIBLE(p,n)  (n >= ((p)->m_nScrollItem * (p)->m_nCols) && n < ((p)->m_nScrollItem * (p)->m_nCols) + ((p)->m_nRows * (p)->m_nCols))

#define ICON_IS_SINGLEROW_VISIBLE(p,n)  (n >= (p)->m_nScrollItem && n < (p)->m_nScrollItem + (p)->m_nCols)

#define IS_SOFTKEY_MENU(p)    ((p)->m_cls == AEECLSID_SOFTKEYCTL)
#define IS_LIST_MENU(p)       ((p)->m_cls == AEECLSID_LISTCTL)
#define IS_MENU(p)            ((p)->m_cls == AEECLSID_MENUCTL)
#define IS_ICON_VIEW(p)       ((p)->m_cls == AEECLSID_ICONVIEWCTL)

#ifdef FEATURE_USES_MMS 
#define IS_INFO_SELECT(p)       IS_PROP_SET(p->m_dwProps,OEMMP_USE_MENU_INFO_SELECT)
#endif
//
// Local function prototypes
//

static int           Menu_New(IShell * pIShell, AEECLSID cls, void ** ppobj);
static uint32        IMenuCtl_AddRef(IMenuCtl * po);
static uint32        IMenuCtl_Release(IMenuCtl * po);
static boolean       IMenuCtl_HandleEvent(IMenuCtl * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean       IMenuCtl_Redraw(IMenuCtl * po);
static void          IMenuCtl_SetActive(IMenuCtl * po, boolean bActive);
static boolean       IMenuCtl_IsActive(IMenuCtl * po);
static void          IMenuCtl_SetRect(IMenuCtl * po, const AEERect * prc);
static void          IMenuCtl_GetRect(IMenuCtl * po, AEERect * prc);
static void          IMenuCtl_SetProperties(IMenuCtl * po, uint32 nProperties);
static uint32        IMenuCtl_GetProperties(IMenuCtl * po);
static void          IMenuCtl_Reset(IMenuCtl * po);
static boolean       IMenuCtl_SetTitle(IMenuCtl * po, const char * pszResFile, uint16 wResID, AECHAR * pText);
static boolean       IMenuCtl_AddItem(IMenuCtl * po, const char * pszResFile, uint16 wResID, uint16 nItemID, AECHAR * pText, uint32 lData);
static boolean       IMenuCtl_AddItemEx(IMenuCtl * po, CtlAddItem *pai);
static boolean       IMenuCtl_GetItemData(IMenuCtl * po, uint16 nItemID, uint32 * plData);
static boolean       IMenuCtl_DeleteItem(IMenuCtl * po, uint16 nItemID);
static boolean       IMenuCtl_DeleteAll(IMenuCtl * po);
static void          IMenuCtl_SetSel(IMenuCtl * po, uint16 nSelect);
static uint16        IMenuCtl_GetSel(IMenuCtl * po);
static void          IMenuCtl_EnableCommand(IMenuCtl * po, boolean bEnable);
static void          IMenuCtl_SetItemText(IMenuCtl * po, uint16 nItemID, const char * pszResFile, uint16 wResID, AECHAR * pText);
static void          IMenuCtl_SetItemTime(IMenuCtl * po, uint16 nItemID, uint16 wTime, uint16 wDuration);
static int           IMenuCtl_GetItemTime(IMenuCtl * po, uint16 nItemID, uint16 * pwDuration);
static void          IMenuCtl_SetStyle(IMenuCtl * po, AEEItemStyle * pNormal, AEEItemStyle * pSel);
static void          IMenuCtl_SetColors(IMenuCtl * po, AEEMenuColors * pc);
static int16         IMenuCtl_MoveItem(IMenuCtl * po, uint16 wID1, int nDir);

static boolean       IMenuCtl_SetItem(IMenuCtl * po, uint16 wID,uint16 wFlags,CtlAddItem * pai);
static int           IMenuCtl_GetItemCount(IMenuCtl * po);
static uint16        IMenuCtl_GetItemID(IMenuCtl * po, int nIdx);
static void          IMenuCtl_Sort(IMenuCtl * po, AEEMenuSortType t, PFNMENUSORTCOMPARE pfn, void * pUser);
static boolean       IMenuCtl_GetItem(IMenuCtl * po, uint16 wID, CtlAddItem * pai);

static void          IMenuCtl_SetSelEx(IMenuCtl * po, uint16 nItemID, boolean bSelected);
static boolean       IMenuCtl_EnumSelInit(IMenuCtl * po);
static uint16        IMenuCtl_EnumNextSel(IMenuCtl * po);
   
static void          IMenuCtl_GetStyle(IMenuCtl * pIMenuCtl,AEEItemStyle * pNormal, AEEItemStyle * pSel);
static void          IMenuCtl_GetColors(IMenuCtl * pIMenuCtl, AEEMenuColors  * pc);
static void          IMenuCtl_GetItemRect(IMenuCtl * pIMenuCtl, uint16 nIdx, AEERect * prc);
static int           IMenuCtl_SetOwnerDrawCB(IMenuCtl * po, PFNOWNERDRAWCB pfnCB, void * pvContext, uint16 nODFlags);

static boolean       MenuCtl_AddODItem(CMenuCtl * pme, uint16 nItemID, uint32 lData);
static void          Menu_FreeItem(CMenuCtl * pme, CMenuItem * pmi);
static void          Menu_SetItemColor(CMenuCtl * pme, IDisplay * pd, boolean bSel,RGBVAL * pcBack);
static void          Menu_NotifySelChange(CMenuCtl * pme);
static void          Menu_DrawScrollBar(CMenuCtl * pme);
static int           Menu_GetScrollBarRects(CMenuCtl * pme, AEERect * prcFrame, AEERect * prcThumb, uint16 * pwPos, uint16 * pwLen);
static void          Menu_ScrollTimerCB(CMenuCtl *pme);
static boolean       Menu_ScrollByPos(CMenuCtl * pme, int nDir, int16 wXPos, int16 wYPos);
static boolean       Menu_Draw(CMenuCtl * pme);
static void          Menu_MoveSelect(CMenuCtl * pme, int nDir, boolean bRedraw);
static void          Menu_Recalc(CMenuCtl * pme);
static void          Menu_GetStyleOffsets(CMenuCtl * pme,int * pcx, int * pcy);
static void          Menu_AutoScroll(CMenuCtl * pme);
static void          Menu_ResetAutoScroll(CMenuCtl * pme);
static IImage *      Menu_LoadImage(CMenuCtl * pme, const char * pszRes, uint16 wID, boolean * pbCached);
static boolean       Menu_ItemScrolls(CMenuCtl * pme, CMenuItem *pi, int nIdx);
static void          Menu_DrawItem(CMenuCtl * pme, CMenuItem * p, AEERect * prc, boolean bSel, int xTrunc);
static void          Menu_DrawODItem(CMenuCtl * pme, CMenuItem * p, AEERect * prc, boolean bSel);
static void          Menu_InitColors(CMenuCtl * pme, RGBVAL * pBack, RGBVAL * pText, RGBVAL * pFrame);
static void          Menu_DrawArrows(CMenuCtl * pme, int x, int y, int xEnd);
static void          Menu_SetupImage(CMenuCtl * pme, CMenuItem * pmi, IImage * pImage);
static boolean       Menu_ResolveImage(CMenuCtl * pme, CtlAddItem * pai,const char ** ppszResImage, IImage ** ppImage);
static CMenuItem *   Menu_AllocItem(CMenuCtl * pme, CMenuItem *piOrig, AECHAR * pText, const char * pszResFile,CMenuItem * piPrev);
static boolean       Menu_HandleSelection(CMenuCtl * pme, int nSelect);
static uint16        Menu_HandleTap(CMenuCtl * pme, int x, int y);
static void          Menu_SendCommand(CMenuCtl * pme);
static int           Menu_SortCB(const void * p1, const void * p2);
static void          Menu_DrawNewSelectItem(CMenuCtl * pme, int nIdx,int nIdxSel);
static void          Menu_HandleChar(CMenuCtl * pme, AECHAR ch);
static boolean       Menu_HandlePens(CMenuCtl * pme, AEEEvent evtCode, uint16 wParam, uint32 dwParam);

static boolean       List_Draw(CMenuCtl * pme);
static void          List_MoveSelect(CMenuCtl * pme, int nDir);

static boolean       SK_Draw(CMenuCtl * pme);
static boolean       SK_CalcLayout(CMenuCtl * pme);
static boolean       SK_MoveSelect(CMenuCtl * pme, int nDir);
static int           SK_SetDrawItems(CMenuCtl * pme, int nDir, int * pnVisibleItems, int * pnLastItem,boolean * pbNoFit);
static uint8         SK_HandlePen(CMenuCtl * pme, int16 wXPos, int16 wYPos, uint16 * pwExtraData, uint16 * pwIndex);

static boolean       Icon_HandleKey(CMenuCtl * pme, uint16 wKey);
static boolean       Icon_Draw(CMenuCtl * pme);
static void          Icon_DrawAllItems(CMenuCtl * pme);
static void          Icon_CalcGridRect(CMenuCtl * pme, AEERect * prc, int nSel);
static void          Icon_DrawItem(CMenuCtl * pme, CMenuItem * pmi,AEERect * prc, boolean bSel);
static void          Icon_DrawItemText(CMenuCtl * pme, CMenuItem * pmi, uint16 cyText, boolean bSel);
static void          Icon_DrawODItem(CMenuCtl * pme, CMenuItem * pmi, AEERect * prc, boolean bSel);
static void          Icon_DrawODItemText(CMenuCtl * pme, CMenuItem * pmi, uint16 cyText, boolean bSel);
static int           Icon_CalcCols(CMenuCtl * pme, int cx);
static void          Icon_CalcRows(CMenuCtl * pme);
static void          Icon_StopAnimation(CMenuCtl * pme);
static uint16        Icon_GetTextHeight(CMenuCtl * pme);
static uint16        Icon_GetMaxTextHeight(CMenuCtl * pme);
static __inline boolean Icon_MapPointToItem(CMenuCtl * pme, int16 wXPos, int16 wYPos, int * pnItem, int * pnIdx);
#define              Icon_GetImageOffset(p)   (Icon_GetMaxTextHeight(p) + AEE_FRAME_SIZE)


static void          ResetColors(IDisplay * pd, RGBVAL clrBack, RGBVAL clrText,RGBVAL cFrame);
static CMenuItem *   GetItemByID(CMenuItem * pl, int nID, CMenuItem ** pLast);
static CMenuItem *   GetItemByIDEx(CMenuItem * pl, int nID, CMenuItem ** pLast, int* pIndex);
static CMenuItem *   GetItemByIdx(CMenuItem * pl, int nIdx);
static void          AdjustRect(AEERect * prc, AEEItemStyle * ps);
static int           GetItemWidth(CMenuItem * pi);
static boolean       Intersects(AEERect * prc, int x, int y);
static uint16        GetItemHeight(CMenuItem * p, uint16 * pwLeft, uint16 wLine);

static void          Cal_DrawTime(CMenuCtl * pme, IDisplay * pd, uint16 wTimeMin, int x, int yOrig, int y);
static void          Cal_Reset(CMenuCtl * pme);
static void          Cal_CalcPageItems(CMenuCtl * pme);
static void          Cal_AutoScroll(CMenuCtl * pme, boolean bForceSel);
static void          Cal_AutoScrollCB(CMenuCtl * pme);
static uint16        Cal_ShowTimeMark(CMenuCtl * pme, CMenuItem * pi, uint16 wIdx, uint16 wTimeNow);
static CMenuItem *   Cal_FindNowItem(CMenuCtl * pme, uint16 * pwNow, uint16 * pwIdx);
static void          Cal_DrawItem(CMenuCtl * pme, CMenuItem * pi, int nIdx, boolean bClear);
static void          Cal_DrawArrow(CMenuCtl * pme, int y);

static void          SK_CalcItemRect(CMenuCtl * pme, uint16 nIdx, AEERect* prc);
static void          Menu_CalcItemRect(CMenuCtl * pme, int nIdx, AEERect* prc);
static void          List_CalcItemRect(CMenuCtl * pme, AEERect * prc, AEEFont nFont);
static void          Icon_CalcItemRect(CMenuCtl * pme, uint16 nIdx, AEERect* prc);

static void          Menu_SetSelbyIdx(CMenuCtl * pme, uint16 nItemID, int nIdx);
static void          Icon_CalcStartEndItemIndx(CMenuCtl* pme, int* pnStart, int* pnEnd);

#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
static void IMenuCtl_SetBottomBarType( IMenuCtl *po, int bottomBar_e_Type);
static void IMenuCtl_SetPopMenuRect( IMenuCtl *po);
#ifdef FEATURE_MENUTITLE_AUTOSCROLL
static void Menu_DrawTitle(CMenuCtl * pme, uint16 wChars);
static boolean Menu_TitleScrolls(CMenuCtl * pme, int nIdx);
static void Menu_TitleAutoScroll(CMenuCtl * pme);
static void Menu_ResetTitleAutoScroll(CMenuCtl * pme);
#endif //FEATURE_MENUTITLE_AUTOSCROLL
#endif

static void          IMenuCtl_GetSelItemRect(IMenuCtl *po, AEERect * prc);

static void IMenuCtl_SetOemProperties(IMenuCtl * po, uint32 nProperties);
static uint32 IMenuCtl_GetOemProperties(IMenuCtl * po);

static void IMenuCtl_SetLastItemSel(IMenuCtl *po, boolean bSelected);
#ifdef FEATURE_CUSTOMIZED_MENU_STYLE
static void IMenuCtl_SetBackGround(IMenuCtl *po, char *pstrImgResFile, uint16 nImgResID);
#endif
static void Menu_DrawBackGround(CMenuCtl * pme, AEERect *pRect);

//
// Menu vtable
//

const AEEStaticClass gMenuClassList[] = {
   {AEECLSID_ICONVIEWCTL,     ASCF_UPGRADE,0,NULL,Menu_New},        
   {AEECLSID_ICONVIEWCTL_10,  ASCF_UPGRADE,0,NULL,Menu_New},   // Pre 1.10
   {AEECLSID_MENUCTL,         ASCF_UPGRADE,0,NULL,Menu_New},
   {AEECLSID_MENUCTL_10,      ASCF_UPGRADE,0,NULL,Menu_New},   // Pre 1.10
   {AEECLSID_SOFTKEYCTL,      ASCF_UPGRADE,0,NULL,Menu_New},
   {AEECLSID_SOFTKEYCTL_10,   ASCF_UPGRADE,0,NULL,Menu_New},   // Pre 1.10
   {AEECLSID_LISTCTL,         ASCF_UPGRADE,0,NULL,Menu_New},
   {AEECLSID_LISTCTL_10,      ASCF_UPGRADE,0,NULL,Menu_New},   // Pre 1.10
   {0,0,0,NULL,NULL},
};

static const VTBL(IMenuCtl) gCMenuCtlFuncs = {IMenuCtl_AddRef,
                                             IMenuCtl_Release,
                                             IMenuCtl_HandleEvent,
                                             IMenuCtl_Redraw,
                                             IMenuCtl_SetActive,
                                             IMenuCtl_IsActive,
                                             IMenuCtl_SetRect,
                                             IMenuCtl_GetRect,
                                             IMenuCtl_SetProperties,
                                             IMenuCtl_GetProperties,
                                             IMenuCtl_Reset,
                                             IMenuCtl_SetTitle,
                                             IMenuCtl_AddItem,
                                             IMenuCtl_AddItemEx,
                                             IMenuCtl_GetItemData,
                                             IMenuCtl_DeleteItem,
                                             IMenuCtl_DeleteAll,
                                             IMenuCtl_SetSel,
                                             IMenuCtl_GetSel,
                                             IMenuCtl_EnableCommand,
                                             IMenuCtl_SetItemText,
                                             IMenuCtl_SetItemTime,
                                             IMenuCtl_GetItemTime,
                                             IMenuCtl_SetStyle,
                                             IMenuCtl_SetColors,
                                             IMenuCtl_MoveItem,
                                             IMenuCtl_GetItemCount,
                                             IMenuCtl_GetItemID,
                                             IMenuCtl_GetItem,
                                             IMenuCtl_SetItem,
                                             IMenuCtl_Sort,
                                             IMenuCtl_SetSelEx,
                                             IMenuCtl_EnumSelInit,
                                             IMenuCtl_EnumNextSel,
                                             IMenuCtl_GetStyle,
                                             IMenuCtl_GetColors,
                                             IMenuCtl_GetItemRect,
                                             IMenuCtl_SetOwnerDrawCB,
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
                                            IMenuCtl_SetBottomBarType,
                                            IMenuCtl_SetPopMenuRect,
#endif
                                             IMenuCtl_GetSelItemRect,
                                             IMenuCtl_SetOemProperties,
                                             IMenuCtl_GetOemProperties,
                                             IMenuCtl_SetLastItemSel,
#ifdef FEATURE_CUSTOMIZED_MENU_STYLE
                                             IMenuCtl_SetBackGround,
#endif
};

/*===========================================================================

EXTERNAL - Called from OEM static table.  Supports creation of menu classes.

===========================================================================*/
static int Menu_New(IShell * pIShell, AEECLSID cls, void ** ppobj)
{
   CMenuCtl *     pme = NULL;
   int            descent,cx,cy;
   AEEItemType    t;
   AEEDeviceInfo  dm;
   boolean        bResumeCmd;
   IBitmap *      pbmDst;
   AEEBitmapInfo  bi;
   int            nErr = EFAILED,
                  nSize;

  // allocate memory for object

   if (!pIShell || !ppobj)
      return(EBADPARM);

   *ppobj = NULL;

   if((pme = (CMenuCtl *)AEE_NewClass((IBaseVtbl *)&gCMenuCtlFuncs, sizeof(CMenuCtl))) == NULL)
      return(ENOMEMORY);

  // create IDisplay interface

   ISHELL_CreateInstance(pIShell, AEECLSID_DISPLAY, (void **)&pme->m_pIDisplay);
   if (!pme->m_pIDisplay) goto Error;

   // Get destination bitmap info
   pbmDst = IDISPLAY_GetDestination(pme->m_pIDisplay);
   if (!pbmDst)
      goto Error;
   nErr = IBITMAP_GetInfo(pbmDst, &bi, sizeof(bi));
   IBITMAP_Release(pbmDst);
   if (SUCCESS != nErr)
      goto Error;

   // Increment the reference count on the shell...

   ISHELL_AddRef(pIShell);

   t = AEE_IT_MENU;
   
   // Post-1.0 uses an async EVT_COMMAND posting mechanism...

   bResumeCmd = (boolean)(cls < AEECLSID_MENUCTL ? FALSE : TRUE);

  // Get the OEM defaults.  Note that IShell catches the case where the OEM has failed to set them...

   switch(cls){

   // Fixup ID for new stuff...

      case AEECLSID_ICONVIEWCTL_10:
         cls = AEECLSID_ICONVIEWCTL;
      case AEECLSID_ICONVIEWCTL:
         t = AEE_IT_ICONVIEW;
         break;

   // Fixup ID for new stuff...

      case AEECLSID_SOFTKEYCTL_10:
         cls = AEECLSID_SOFTKEYCTL;
      case AEECLSID_SOFTKEYCTL:
         t = AEE_IT_SOFTKEY;
         break;

   // Fixup ID for new stuff...

      case AEECLSID_MENUCTL_10:
         cls = AEECLSID_MENUCTL;
         break;

   // Fixup ID for new stuff...

      case AEECLSID_LISTCTL_10:
         cls = AEECLSID_LISTCTL;
         break;

   }
/*
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    pme->userSetStyle   = FALSE;
    if( t == AEE_IT_MENU)
    {
        pme->m_style[0].ft         = AEE_FT_NONE;
        pme->m_style[0].xOffset  = 4;///2;
        pme->m_style[0].yOffset  = 3;///2;
        pme->m_style[0].roImage = AEE_RO_TRANSPARENT;

        pme->m_style[1].ft          = AEE_FT_NONE;
        pme->m_style[1].xOffset   = 4;///2;
        pme->m_style[1].yOffset   = 3;///2;
        pme->m_style[1].roImage  = AEE_RO_TRANSPARENT;

        pme->imageSelectBar = NULL;
    }
    else if( t == AEE_IT_SOFTKEY)
    {
        pme->m_style[0].ft         = AEE_FT_NONE;
        pme->m_style[0].xOffset  = 4;///2;
        pme->m_style[0].yOffset  = 3;///2;
        pme->m_style[0].roImage = AEE_RO_TRANSPARENT;
    
        pme->m_style[1].ft         = AEE_FT_NONE;
        pme->m_style[1].xOffset  = 4;///2;
        pme->m_style[1].yOffset  = 3;///2;
        pme->m_style[1].roImage = AEE_RO_TRANSPARENT;
        
        pme->imageSelectBar = NULL;
    }
    else
    {
        ISHELL_GetItemStyle(pIShell, t, &pme->m_style[0], &pme->m_style[1]);
    }
#else //defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   // Get and Cache Style for Normal and Selected Items
   ISHELL_GetItemStyle(pIShell, t, &pme->m_style[0], &pme->m_style[1]);
#endif //defined( FEATURE_CUSTOMIZED_MENU_STYLE)
*/
   
   pme->m_pIShell          = pIShell;           // IShell Pointer
   pme->m_cls              = cls;               // Control Class
   pme->m_nPageItems       = SOFT_PAGE_ITEMS;   // initialized to # of soft key items visible at once
   pme->m_bSendCmd         = 1;                 // defaults to on
   pme->m_bResumeCmd       = bResumeCmd;        // Set TRUE for Post 1.0

    // Default Properties
    IMenuCtl_SetProperties( (IMenuCtl *)pme, 0);    
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    if(t == AEE_IT_MENU)
    {
        pme->bottomBarType = BTBAR_NONE;
        IMenuCtl_SetProperties( (IMenuCtl *)pme, MP_WRAPSCROLL);
    }
#endif //defined( FEATURE_CUSTOMIZED_MENU_STYLE)

   // Default Color Values
   IMenuCtl_SetColors((IMenuCtl *)pme, NULL);
#ifdef FEATURE_RANDOM_MENU_COLOR
   if(cls == AEECLSID_MENUCTL)
   {
       OEM_GetConfig(CFGI_RANDOM_MENU, (void*)&pme->m_nRandomMenu, sizeof(pme->m_nRandomMenu));
   }
#endif
   pme->m_yLastTimeMark = INVALID_MARK;

   // obtain font metrics for later use
   pme->m_cyFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, AEE_FONT_BOLD, &pme->m_nAscent, &descent);
   
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
       pme->userSetStyle   = FALSE;
       /* 为了方便控制菜单的高度，一般用MENUITEM_HEIGHT来统一设置高度
        这样子只需要在Appscommon.h里面修改该宏的值就可以自动调整菜单显示*/
       if( t == AEE_IT_MENU)
       {
           pme->m_style[0].ft         = AEE_FT_NONE;
           pme->m_style[0].xOffset  = 4;///2;
           pme->m_style[0].yOffset  = (MENUITEM_HEIGHT - pme->m_cyFont)/2;
           pme->m_style[0].roImage = AEE_RO_TRANSPARENT;
   
           pme->m_style[1].ft          = AEE_FT_NONE;
           pme->m_style[1].xOffset   = 4;///2;
           pme->m_style[1].yOffset   = (MENUITEM_HEIGHT - pme->m_cyFont)/2;
           pme->m_style[1].roImage  = AEE_RO_TRANSPARENT;
   
           pme->imageSelectBar = NULL;
       }
       else if( t == AEE_IT_SOFTKEY)
       {
           pme->m_style[0].ft         = AEE_FT_NONE;
           pme->m_style[0].xOffset  = 4;///2;
           pme->m_style[0].yOffset  = (MENUITEM_HEIGHT - pme->m_cyFont)/2;
           pme->m_style[0].roImage = AEE_RO_TRANSPARENT;
       
           pme->m_style[1].ft         = AEE_FT_NONE;
           pme->m_style[1].xOffset  = 4;///2;
           pme->m_style[1].yOffset  = (MENUITEM_HEIGHT - pme->m_cyFont)/2;
           pme->m_style[1].roImage = AEE_RO_TRANSPARENT;
           
           pme->imageSelectBar = NULL;
       }
       else
       {
           ISHELL_GetItemStyle(pIShell, t, &pme->m_style[0], &pme->m_style[1]);
       }
#else //defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      // Get and Cache Style for Normal and Selected Items
      ISHELL_GetItemStyle(pIShell, t, &pme->m_style[0], &pme->m_style[1]);
#endif //defined( FEATURE_CUSTOMIZED_MENU_STYLE)
#ifdef FEATURE_CUSTOMIZED_MENU_STYLE
    pme->m_pBgImage = NULL;
#endif
#ifdef FEATURE_CUSTOMIZED_MENU_STYLE
    pme->nBgImgResID = 0;
    MEMSET(pme->strBgImgResFile, 0, sizeof(pme->strBgImgResFile));
#endif
   // get display metrics
   ISHELL_GetDeviceInfo(pme->m_pIShell, &dm);
#ifdef FEATURE_DISP_240X320
	pme->m_nSBWidth = CXARROW;                     // Scroll Bar Width
#else
   pme->m_nSBWidth = dm.cxScrollBar;               // Scroll Bar Width
#endif
   pme->m_bColor = (boolean)(bi.nDepth > 1);  // Color Depth

   pme->m_nSBWidth += 2*AEE_FRAME_SIZE;            // Adjust for frame

   // Image Draw Delay for SINGLE_FRAME Icon View with MP_ICON_DELAY_IMAGE property
   if(dm.wMenuImageDelay)
      pme->m_wMenuImageDelay = (uint32)dm.wMenuImageDelay;
   else
      pme->m_wMenuImageDelay = IMAGE_DELAY_TIME;

   // Auto Scroll - Update Delay
   if(dm.wMenuTextScroll)
      pme->m_wMenuTextScroll = (uint32)dm.wMenuTextScroll;
   else
      pme->m_wMenuTextScroll = AUTO_SCROLL_TIME;

   // Default Control Rect
   if (cls == AEECLSID_SOFTKEYCTL) {
      pme->m_rc.x    = 0;
      pme->m_rc.dx   = (int16)bi.cx;
      Menu_GetStyleOffsets(pme,&cx,&cy);
      pme->m_rc.dy   = pme->m_cyFont + cy;
      pme->m_rc.y    = (int16)(bi.cy - pme->m_rc.dy); // Bottom of Screen
   }
   else 
      SETAEERECT(&pme->m_rc, 0, 0, bi.cx, bi.cy);  // Entire Screen

   // Default Title Rect
   pme->m_rcTitle.dx = (int16)bi.cy;

#if defined( FEATURE_CUSTOMIZED_MENU_STYLE) && defined( FEATURE_FUNCS_TITLE_BAR)
    pme->m_rcTitle.dy = GetTitleBarHeight( pme->m_pIDisplay);
#else
    pme->m_rcTitle.dy = pme->m_cyFont;
#endif //defined( FEATURE_CUSTOMIZED_MENU_STYLE)

   // Load Ellipses Symbol & Cache its width
   pme->m_szEllipses[0] = (AECHAR)IDISPLAY_GetSymbol(pme->m_pIDisplay,AEE_SYM_ELLIPSES,AEE_FONT_NORMAL);
   pme->m_cxDotWidth = IDISPLAY_MeasureText(pme->m_pIDisplay, AEE_FONT_BOLD, (const AECHAR *)pme->m_szEllipses);
   
   pme->m_pEnum   = NULL;
   pme->m_arPenRepeat.wItem = AEEREPEAT_SCROLLBAR;
   nSize = sizeof(AEEAutoRepeat);
   ISHELL_GetDeviceInfoEx(pIShell, AEE_DEVICEITEM_REPEAT_TIME, (void *)&pme->m_arPenRepeat, &nSize);
   MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));

   *ppobj = pme;
   pme->m_bAutoScrollMenuItem = FALSE;
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    pme->SetPopMenu = FALSE;
#endif //defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    pme->m_menuReallyDy = 0;//add by xuhui
   return(0);

Error:
   IMenuCtl_Release((IMenuCtl *)pme);
   return nErr;
}

/*=====================================================================

Public Method - Increments the reference count on the menu class object.

======================================================================*/
static uint32 IMenuCtl_AddRef(IMenuCtl * po)
{
   return(++((CMenuCtl*)po)->m_nRefs);
}

/*=====================================================================

Public Method - Decrements the reference count on the menu class object.
If the value reaches 0, it frees the object and all associated data.

======================================================================*/
static uint32 IMenuCtl_Release(IMenuCtl * po)
{
   CMenuCtl * pme = (CMenuCtl*)po;

   if(pme->m_nRefs){
      if (--pme->m_nRefs)
         return(pme->m_nRefs);

      // Cancel All Timers
      ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);

      // Delete All Items
      IMenuCtl_DeleteAll((IMenuCtl*)pme);

      // Free Title
      aee_freeobj((void **)&pme->m_pTitle);
   
      // Reset Calendar
      Cal_Reset(pme);
      
      CALLBACK_Cancel(&pme->m_cbCommand);

      // Release IShell & IDisplay
      aee_releaseobj((void **)&pme->m_pIShell);
      aee_releaseobj((void **)&pme->m_pIDisplay);

#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      if(pme->imageSelectBar != NULL)       
      {
          IIMAGE_Release(pme->imageSelectBar);
          pme->imageSelectBar = NULL;
      }
      if(pme->m_pBgImage != NULL)       
      {
          IIMAGE_Release(pme->m_pBgImage);
          pme->m_pBgImage = NULL;
      }
#endif //defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      
      // Free Menu Object
      FREE(pme);
   }      
   return(0);
}

/*=====================================================================

Public Method - Handles events send by the app to the menu.  

======================================================================*/
static boolean IMenuCtl_HandleEvent(IMenuCtl * po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   CMenuCtl * pme = (CMenuCtl*)po;

   if (!pme->m_bActive && eCode != EVT_CTL_SET_TITLE && eCode != EVT_CTL_ADD_ITEM)
      return(FALSE);  // Event not handled.
      
   DBGPRINTF("IMenuCtl_HandleEvent 0x%x 0x%x 0x%x",eCode,wParam,dwParam);
   // First handle the pens, most time sensitive events we get

#ifdef FEATURE_LCD_TOUCH_ENABLE//Andrew ADD FOR LCD TOUCH
	//if(( eCode == EVT_PEN_UP ) || (eCode == EVT_PEN_UP))
	if(eCode == EVT_PEN_UP )//wlh 20090409 mod
	{
		return Menu_HandlePens(pme, eCode, wParam, dwParam);
	}
	if(eCode == EVT_USER)//wlh 这里响应类似于日程表中左右方向键
	{
		eCode = EVT_KEY;
	}
#endif//FEATURE_LCD_TOUCH_ENABLE

   if( eCode >= EVT_POINTER_DOWN && eCode <= EVT_POINTER_STALE_MOVE ){
      // Stale move is not handled and likely the largest stream of events
      // For speed, abort quick.
      if( eCode == EVT_POINTER_STALE_MOVE ){
         return FALSE;
      }
      return Menu_HandlePens(pme, eCode, wParam, dwParam);
   }
   switch (eCode) {

      // Menu Title Changed - Event from Dialog      
      case EVT_CTL_SET_TITLE:
      {
         AECHAR *pText;
         const char *pRes;

         if(wParam){
            pRes = (const char *)dwParam;
            pText = NULL;
         }
         else{
            pRes = NULL;
            pText = (AECHAR *)dwParam;
         }   
         return(IMenuCtl_SetTitle(po, pRes, wParam, pText));
      }
      
      // Add Item - Event from Dialog      
      case EVT_CTL_ADD_ITEM:
         return(IMenuCtl_AddItemEx(po, (CtlAddItem *)dwParam));
#ifdef FEATURE_LCD_TOUCH_ENABLE//Andrew ADD FOR LCD TOUCH
	  case EVT_USER_REDRAW:
#endif//FEATURE_LCD_TOUCH_ENABLE
      case EVT_KEY:
         if (pme->m_dwOemProps & OEMMP_SWITCHNAVIGATIONKEY)
         {
             switch (wParam)
             {
                 case AVK_LEFT:
                     wParam = AVK_UP;
                     break;
                       
                 case AVK_RIGHT:
                     wParam = AVK_DOWN;
                     break;
                       
                 case AVK_UP:
                     wParam = AVK_LEFT;
                     break;
                       
                 case AVK_DOWN:
                     wParam = AVK_RIGHT;
                     break;
                
                 default:
                     break;
             }
         }
         
         if ((!(pme->m_dwOemProps & OEMMP_DISTINGUISH_INFOKEY_SELECTKEY)) &&
             (wParam == AVK_INFO))
         {
             wParam = AVK_SELECT;
         }
        
         if(pme->m_cls == AEECLSID_ICONVIEWCTL && Icon_HandleKey(pme, wParam))
            return(TRUE);
         
         //Check if its a TAP event (i.e. EVT_KEY event with wParam set to AVK_TAP)

         if(wParam == AVK_TAP){
            wParam = Menu_HandleTap(pme, AEE_GET_X(dwParam), AEE_GET_Y(dwParam));
            if( wParam == AVK_UNDEFINED ) 
               return(FALSE);
         }
         
         switch (wParam) {
            case AVK_SELECT:
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
               if(IS_MULTI_SEL(pme)){
                  break;/*do not handle this key*/
               }
               else
#endif/*if defined( FEATURE_CUSTOMIZED_MENU_STYLE)*/
               {
#ifdef FEATURE_USES_MMS                 
                  if(IS_INFO_SELECT(pme))
                  {
                    break;
                  }
                  else
#endif                    
                  {
                      DBGPRINTF("AVK_SELECT 0x%x %d",eCode,wParam);
                      return Menu_HandleSelection(pme, pme->m_nSelect);
                  }
               }

         // Note: We have already handled keys for AEECLSID_ICONVIEWCTL so here we only care
         // about remaining control types

            // AVK_LEFT and AVK_RIGHT needs to be handled only by softkey control. For others,
            // we generate EVT_CTL_TAB
            //add by yangdecai 2010 - 08-06
            case AVK_INFO:
				{
				  if(IS_MULTI_SEL(pme))
				  {
	                  CMenuItem   * pItem;
	                  if((pItem = GetItemByIdx(pme->m_pItemList, pme->m_nSelect)) != NULL)
					  {
	                     pItem->bIsSel = !pItem->bIsSel;
	                     IMenuCtl_SetSelEx((IMenuCtl *)pme, pItem->nItemID, pItem->bIsSel);
	                     if(!NO_REDRAW(pme))
	                     {
	                     	MSG_FATAL("AVK_INFO............",0,0,0);
	                        IMenuCtl_Redraw((IMenuCtl *)pme);
	                     }
	                     return TRUE;
	                  }
					  else
					  {
	                     return FALSE;
	                  }
                  }
				}
				break;

            case AVK_LEFT:
               if (IS_SOFTKEY_MENU(pme))
                  return SK_MoveSelect(pme, -1);

               if( ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)0, (uint32)po) )
                  return TRUE;
               break;

            case AVK_RIGHT:
               if (IS_SOFTKEY_MENU(pme))
                  return SK_MoveSelect(pme, 1);

#if 0 //defined( FEATURE_CUSTOMIZED_MENU_STYLE)   change by xuhui
               if(IS_MULTI_SEL(pme)){
                  CMenuItem   * pItem;
                  if((pItem = GetItemByIdx(pme->m_pItemList, pme->m_nSelect)) != NULL){
                     pItem->bIsSel = !pItem->bIsSel;
                     IMenuCtl_SetSelEx((IMenuCtl *)pme, pItem->nItemID, pItem->bIsSel);
                     if(!NO_REDRAW(pme))
                     {
                        IMenuCtl_Redraw((IMenuCtl *)pme);
                     }

                     return TRUE;
                  }else{
                     return FALSE;
                  }
               }
               else
#endif/*if defined( FEATURE_CUSTOMIZED_MENU_STYLE)*/
               {
                  if( ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)1, (uint32)po) )
                     return TRUE;
               }
               break;

            // AVK_UP and AVK_DOWN need not be handled by softkey control. For softkey control,
            // we generate EVT_CTL_TAB

            case AVK_UP:
               if (!IS_SOFTKEY_MENU(pme)) {
                  if ((!WRAP_SCROLL(pme)) && (pme->m_nSelect == 0))
                  {
                      return FALSE;
                  }
                  if (IS_LIST_MENU(pme))
                     List_MoveSelect(pme, -1);
                  else
                     Menu_MoveSelect(pme, -1, TRUE);
                  return(TRUE);
               }

               if( ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)0, (uint32)po) )
                  return TRUE;
               break;

            case AVK_DOWN:
               if (!IS_SOFTKEY_MENU(pme)) {
                  if ((!WRAP_SCROLL(pme)) && (pme->m_nSelect == pme->m_nItems-1))
                  {
                      return FALSE;
                  }
                  if (IS_LIST_MENU(pme))
                     List_MoveSelect(pme, 1);
                  else
                     Menu_MoveSelect(pme, 1, TRUE);
                  return(TRUE);
               }

               if( ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)1, (uint32)po) )
                  return TRUE;
               break;

          // For calendar, we treat numeric key press as input hour. Zero is treated as 10. We
         // iterate items to check if an entry starts at input hour or (input + 12) hour and 
         // if so, that entry is set as selection
            default:
            {
               CMenuItem * p;
               int         i;
               uint16      nHour;

               if(wParam >= AVK_0 && wParam <= AVK_9 && IS_CALENDAR_VIEW(pme)){
                  nHour = (uint16)(wParam - AVK_0);
                  if(wParam == AVK_0)
                     nHour = (uint16)10;
                  for(i = 0; i < 2; i++, nHour += 12){
                     for(p = pme->m_pItemList; p ; p = p->pNext){
                        if((uint16)(p->wCalStart / 60) == nHour){
                           IMenuCtl_SetSel(po, (uint16)p->nItemID);
                           return(TRUE);
                        }
                     }
                  }
               }
// add these @08.01.16
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

			/*
              else if( !pme->userSetStyle   && 
                       wParam >= AVK_0      && 
                       wParam <= AVK_9      && 
                       IS_MENU( pme)        &&
                       IS_PROP_SET( pme->m_dwProps, MP_BIND_ITEM_TO_NUMBER_KEY) && 
                       pme->m_nItems <= 10
               )
               */
               //Modify by zzg 2011_12_12:  有些菜单有超过10个ITEM
               else if( !pme->userSetStyle   && 
                       wParam >= AVK_0      && 
                       wParam <= AVK_9      && 
                       IS_MENU( pme)        &&
                       IS_PROP_SET( pme->m_dwProps, MP_BIND_ITEM_TO_NUMBER_KEY) 
               )
               //Modify end
               {
                   int index = (wParam - AVK_1 + 10) % 10;
                   if( index > pme->m_nItems - 1)
                   {
                       break;
                   }
                   {
                        uint16 nItemID = IMENUCTL_GetItemID( po, index);
                       //uint32 oldProperties = IMENUCTL_GetProperties(po);
                       //IMENUCTL_SetProperties(po, MP_NO_REDRAW);
                       //IMENUCTL_SetSel( po, nItemID);
                       //IMENUCTL_SetProperties(po, oldProperties);
                       DBGPRINTF("IMenuCtl_HandleEvent EVT_COMMAND %d 0x%x",nItemID,wParam);
                       ISHELL_HandleEvent(pme->m_pIShell, EVT_COMMAND, (uint16)nItemID, (uint32)pme);
                       //ISHELL_HandleEvent( pme->m_pIShell, EVT_KEY, AVK_SELECT, 0);
                   }
                   return TRUE;
               }
                
#endif//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
               break;
            }
         }
         break;

   // We compare entered character with first character of a menu item text and upon match
   // set the item as current selection
      case EVT_CHAR:
         Menu_HandleChar(pme, (AECHAR)wParam);
         return TRUE;

      default:
         break;
   }
   return(FALSE);   // did not handle the event
}

/*=====================================================================

Public Method - Instructs the menu to redraw the contents.

======================================================================*/
static boolean IMenuCtl_Redraw(IMenuCtl * po)
{
   CMenuCtl * pme = (CMenuCtl*)po;
   MSG_FATAL("IMenuCtl_Redraw......00000",0,0,0);
   switch(pme->m_cls){

      case AEECLSID_SOFTKEYCTL:
	  	 MSG_FATAL("IMenuCtl_Redraw......AEECLSID_SOFTKEYCTL",0,0,0);
         SK_CalcLayout(pme);
         return(SK_Draw(pme));

      case AEECLSID_LISTCTL:
	  	MSG_FATAL("IMenuCtl_Redraw......AEECLSID_LISTCTL",0,0,0);
         return(List_Draw(pme));

      case AEECLSID_MENUCTL:
	  	 MSG_FATAL("IMenuCtl_Redraw......AEECLSID_MENUCTL",0,0,0);
         Menu_Draw(pme);
         Menu_ResetAutoScroll(pme);
#ifdef FEATURE_MENUTITLE_AUTOSCROLL
         Menu_ResetTitleAutoScroll(pme);
#endif //FEATURE_MENUTITLE_AUTOSCROLL		 
         return(TRUE);

      case AEECLSID_ICONVIEWCTL:
	  	 MSG_FATAL("IMenuCtl_Redraw......AEECLSID_ICONVIEWCTL",0,0,0);
         return(Icon_Draw(pme));
   }
   MSG_FATAL("IMenuCtl_Redraw......11111",0,0,0);
   return(FALSE);
}

/*=====================================================================

Public Method - Sets the menu in an "active" state whereby keys are
processed.

======================================================================*/
static void IMenuCtl_SetActive(IMenuCtl * po, boolean bActive)
{
   CMenuCtl * pme = (CMenuCtl *)po;

   if(pme->m_bActive != bActive) {

      pme->m_bActive = bActive;

      if(AUTO_SCROLL(pme) && bActive)
         Cal_AutoScroll(pme, TRUE);       // Active Calendar Menu with Auto Scroll
      else{
         pme->m_yLastTimeMark = INVALID_MARK;            // Invalidate time mark
         ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);  // Cancel calendar menu updates

         if(IS_MENU(pme) && bActive)
            Menu_ResetAutoScroll(pme);    // Active Menu - Reset item text auto scroll
#ifdef FEATURE_MENUTITLE_AUTOSCROLL
            Menu_ResetTitleAutoScroll(pme);
#endif //FEATURE_MENUTITLE_AUTOSCROLL

   // Icon views redraw async - If this is a disable, do not allow it!
      
		MSG_FATAL("pme->m_cls==========%x",pme->m_cls,0,0);
         if(!NO_REDRAW(pme)){
            if(IS_ICON_VIEW(pme) && !bActive)
               Icon_StopAnimation(pme);         // Don't animate if its not selection
            else
            	{
            		#ifdef FEATURE_LCD_TOUCH_ENABLE
   					if((pme->m_cls!=AEECLSID_MENUCTL)&&(pme->m_cls!=AEECLSID_LISTCTL)&&
		   			(pme->m_cls!=AEECLSID_ICONVIEWCTL))
					#endif
					{
            			MSG_FATAL("IMenuCtl_Redraw redraw.......0000",0,0,0);
              			IMenuCtl_Redraw(po);              // Redraw
              			MSG_FATAL("IMenuCtl_Redraw redraw.......0000end",0,0,0);
            		}
					#ifdef FEATURE_LCD_TOUCH_ENABLE
					else
					{
						uint32 temp = NO_ACTIVE_REDRAW(pme);
						MSG_FATAL("temp=========%d",temp,0,0);
						if(!NO_ACTIVE_REDRAW(pme))
						{
							MSG_FATAL("IMenuCtl_Redraw redraw.......111111",0,0,0);
							IMenuCtl_Redraw(po);              // Redraw
						}
					}
					#endif
         		}
      		}
         // Reset pme->m_nScrollItem (default is 0) in horizontal scroll case
         if (pme->m_bIconHScroll){
            int start, end;
			MSG_FATAL("................",0,0,0);
            Icon_CalcStartEndItemIndx(pme, &start, &end);
            pme->m_nScrollItem = start;
         }
      }
   }
}

/*=====================================================================

Public Method - Returns the active state of the menu.

======================================================================*/
static boolean IMenuCtl_IsActive(IMenuCtl * po)
{
   return((boolean)((CMenuCtl*)po)->m_bActive);
}

/*=====================================================================

Public Method - Sets the rectangle for the menu.

======================================================================*/
static void IMenuCtl_SetRect(IMenuCtl * po, const AEERect * prc)
{
   CMenuCtl * pme = (CMenuCtl*)po;
   pme->SetPopMenu = FALSE;

   if (prc) {
      pme->m_rc = *prc;
      Menu_Recalc(pme);
   }
}

/*=====================================================================

Public Method - Returns the rectangle for the menu.

======================================================================*/
static void IMenuCtl_GetRect(IMenuCtl * po, AEERect * prc)
{
   if(prc)
      *prc = ((CMenuCtl*)po)->m_rc;
}

/*=====================================================================

Public Method - Sets the properties (flags) for the menu.

======================================================================*/
static void IMenuCtl_SetProperties(IMenuCtl * po, uint32 nProperties)
{
   CMenuCtl *     pme = (CMenuCtl*)po;

   // Force this property regardless of what they set!
   if (IS_SOFTKEY_MENU(pme) || IS_LIST_MENU(pme))
   {
	   
      //nProperties |= MP_WRAPSCROLL;
   }
   else if (!IS_MENU(pme))
   {
    
	   nProperties &= ~MP_WRAPSCROLL;
   }

   // Calendar isn't owner drawn - ensure that for owner draw menu,
   // calendar specific properties aren't set

   if (IS_OD_CB(pme))
      nProperties &= ~(MP_CALENDAR|MP_AUTOSCROLLTIME);

   pme->m_dwProps = nProperties;

   if (MAX_SK_ITEMS(pme))
      pme->m_nPageItems = __MIN(pme->m_nItems, MAX_SOFT_PAGE_ITEMS);
}
   
/*=====================================================================

Public Method - Returns the active properties for the menu.

======================================================================*/
static uint32 IMenuCtl_GetProperties(IMenuCtl * po)
{
   return(((CMenuCtl *)po)->m_dwProps);
}

/*=====================================================================

Public Method - Clears the menu contents and cancels all pending 
activities.

======================================================================*/
static void IMenuCtl_Reset(IMenuCtl * po)
{
   CMenuCtl * pme = (CMenuCtl*)po;

   CALLBACK_Cancel(&pme->m_cbCommand);

   IMenuCtl_DeleteAll(po);

   aee_freeobj((void **)&pme->m_pTitle);

   pme->m_bActive = 0;
   pme->m_pEnum   = NULL;
   MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));

   Cal_Reset(pme);
}

/*=====================================================================

Public Method - Sets the menu title per AEEMenu.h

======================================================================*/
static boolean IMenuCtl_SetTitle(IMenuCtl * po, const char * pszResFile, uint16 wResID, AECHAR * pText)
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
   if (pText){
      // Free Old Title
      aee_freeobj((void **)&pme->m_pTitle);
      pme->m_pTitle = WSTRDUP(pText);
      bRet = TRUE;
   }

   return(bRet && pme->m_pTitle != NULL);
}

/*=====================================================================

Public Method - Adds a menu item per AEEMenu.h

======================================================================*/
static boolean IMenuCtl_AddItem(IMenuCtl * po, const char * pszResFile, uint16 wResID, uint16 nItemID, AECHAR * pText, uint32 lData)
{
   CtlAddItem  ai;
   CMenuCtl* pme = (CMenuCtl*)po;

   // Check if they have given us something to draw
   // If not, add item if flag required to allow owner draw items is set
   // Otherwise fail
   if (!pszResFile && !pText){
      if (OD_ITEMS_ALLOWED(pme))
         return MenuCtl_AddODItem(pme, nItemID, lData);
      else
         return FALSE;
   }

	/*
	//Add By zzg 2011_12_29	
   	{
		char tempstr[256];

		WSTRTOSTR(pText, tempstr, 256);		
		
		UTF8TOWSTR((byte*)tempstr, 256, pText, 	(256)*sizeof(AECHAR));	

		DBGPRINTF("***zzg IMenuCtl_AddItem tempstr=%s***", tempstr);
		DBGPRINTF("***zzg IMenuCtl_AddItem pText=%s***", pText);	
	}	
	//Add End
	*/
   
   
   // Non owner drawn menu item
   MEMSET(&ai, 0, sizeof(CtlAddItem));
   ai.pszResText  = pszResFile;
   ai.pText       = pText;
   ai.wText       = wResID;
   ai.wItemID     = nItemID;
   ai.dwData      = lData;
   return(IMenuCtl_AddItemEx(po, &ai));
}

/*=====================================================================

Public Method - Enhanced mechanism to add menu items.  See AEEMenu.h

======================================================================*/
static boolean IMenuCtl_AddItemEx(IMenuCtl * po, CtlAddItem * pai)
{
   CMenuCtl *     pme = (CMenuCtl*)po;
   CMenuItem *    pmiLast = NULL;
   CMenuItem *    pmi = NULL;
   AECHAR *       pText = NULL;
   IImage *       pImage = NULL;
   const char *   pszResImage = NULL;
   boolean        bFreeText = FALSE;
   boolean        bSingleFrame = (boolean)(SINGLE_FRAME(pme) ? TRUE : FALSE);

   // Can't do any thing without pai
   if (!pai)
   {
   	return(FALSE);
   }

   // If it is not allowing Duplicate ID's, do check
   if (!(IS_ALLOWING_DUPE(pme)))
   {
	   for (pmiLast = NULL, pmi = pme->m_pItemList; pmi != NULL; pmi = pmi->pNext) 
	   {
	    	pmiLast = pmi;
			
	   		if(pmi->nItemID == pai->wItemID)
	        {
	        	return(FALSE);
	   		}
	   }
   }
   else
   {
      pmiLast = pme->m_pLastItem;
   }

   // Check Sanity of Image & Text Pointers
   if(pai->pImage && !AEE_CHECK_PTR("IMENU_AddItemEx", pai->pImage, 1, FALSE) || 
      pai->pText && !AEE_CHECK_PTR("IMENU_AddItemEx", pai->pText, 1, FALSE)) 
   {
      return(FALSE);
   }

   // See if they passed us an image.  If so, just AddRef the object.
   // If not, see they specified valid ID and resource file. If yes,
   // then try to load it. Otherwise fail if this is Icon View.
   // We need to get out on failure.
   if (!Menu_ResolveImage(pme, pai, &pszResImage, &pImage))
   {
   		return(FALSE);
   }
   
   // Try to load the text if specified
   pText = (AECHAR *)pai->pText;

   // We cannot show anything!
   if (!pImage && !bSingleFrame && !pText && (!pai->pszResText || !pai->wText))
   {
   		return(FALSE);
   }

   // If text not specified, load it from resource file
   // Note: Icon views do NOT require a text string
   // Note: pai->wText is 0xffff by default for item IDs created in resource editor
   if (!pText && pai->wText && pai->wText != 0xffff)
   {
      // Allocate memory for item text
      if ((pText = (AECHAR *)MALLOC(MAX_MENU_TEXT * sizeof(AECHAR))) != NULL)
      {
      	bFreeText = TRUE;    // Note that we now need to free the text
      }
      else 
	  {
         aee_releaseobj((void **)&pImage);
         return FALSE;
      }

      // Load item text
      if(!ISHELL_LoadResString(pme->m_pIShell, pai->pszResText, pai->wText, pText, sizeof(AECHAR) * MAX_MENU_TEXT)) 
	  {
         aee_releaseobj((void **)&pImage);
         if (bFreeText)
         {
         	FREE(pText);
         }
         return FALSE;
      }
   }

   // Allocate memory for the item. If allocation is successful, item is added to the
   // end of the list
   if ((pmi = Menu_AllocItem(pme, NULL, pText, pszResImage, pmiLast)) == NULL) 
   {
      aee_releaseobj((void **)&pImage);
      if (bFreeText)
      {
      	FREE(pText);
      }
      return(FALSE);
   }

   // If we are caching image resource file name, we need to cache resource ID also.
   if (pmi->pszImage)
   {
   		pmi->wImageID = pai->wImage;
   	}

   // Pre-calc the image stuff in case we unload it
   if(pImage){
      if(IS_MULTI_IMAGE(pme))
         IIMAGE_SetParm(pImage,IPARM_NFRAMES,(int)(IS_TRI_STATE(pme) ? 3 : 2),0);
      Menu_SetupImage(pme,pmi,pImage);
   }

   // copy other stuff
   pmi->lData     = pai->dwData;
   pmi->nItemID   = pai->wItemID;
   pmi->nFont     = (AEEFont)pai->wFont;   

   // AEE_FONT_BOLD is default for soft key and icon view
   if(!pmi->nFont)
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    {
		if( !pme->userSetStyle)
		{
            pmi->nFont = (AEEFont)(IS_ICON_VIEW(pme) ? AEE_FONT_BOLD : AEE_FONT_NORMAL);
		}
		else
		{
            pmi->nFont = (AEEFont)(IS_SOFTKEY_MENU(pme) || IS_ICON_VIEW(pme) ? AEE_FONT_BOLD : AEE_FONT_NORMAL);
		}
    }
#else
    pmi->nFont = (AEEFont)(IS_SOFTKEY_MENU(pme) || IS_ICON_VIEW(pme) ? AEE_FONT_BOLD : AEE_FONT_NORMAL);
#endif //defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      

   // Free Text memory if we allocated
   if(bFreeText)
      FREE(pText);
   
   // Recalc the entire layout of the menu based upon the size of the items and the style(s)
   Menu_Recalc(pme);

   // For softkey, send EVT_CTL_SKMENU_PAGE_FULL if page full upon addition of this item
   if(IS_SOFTKEY_MENU(pme) && !pme->m_bSKPageFullNotify && pme->m_nItems >= pme->m_nPageItems){
      pme->m_bSKPageFullNotify = TRUE;
      ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_SKMENU_PAGE_FULL, 0, (uint32)pme);
   }

   // For icon view, we keep height of item text
   if (IS_ICON_VIEW(pme) && *pmi->text)
      pmi->cyText = Icon_GetTextHeight(pme);

   return(TRUE);
}

/*=====================================================================

Public Method - Moves an item up/down in the list.

======================================================================*/
static int16 IMenuCtl_MoveItem(IMenuCtl * po, uint16 wID, int nDir)
{
   CMenuCtl *  pme = (CMenuCtl *)po;
   CMenuItem * pi1, * pi1Prev;
   CMenuItem * pi2, * pi2Prev;
   CMenuItem * pTemp;
   CMenuItem * piSel;
   int16       nRet;

   // Get specified item and its previous item
   pi1 = GetItemByID(pme->m_pItemList, wID, &pi1Prev);

   if(!pi1 || !nDir)
      return(-1);

   pi2Prev = NULL;

   if(nDir > 0){
      // Find nth item after specified item and also that items previous item
      for(pi2 = pi1; pi2 != NULL && nDir; pi2 = pi2->pNext, nDir--)
         pi2Prev = pi2;

      if( pi2 && pi2 == pme->m_pLastItem ){
         pme->m_pLastItem = pi1;
      }
   }
   else{
      // Find nth item before specified item and also that items previous item
      pi2 = pi1Prev;
      while(pi2){
         GetItemByID(pme->m_pItemList, pi2->nItemID, &pi2Prev);
         if(!++nDir)
            break;
         pi2 = pi2Prev;
      }
      if( pi2 && pi1 == pme->m_pLastItem ){
         pme->m_pLastItem = pi1Prev;
      }
   }

   if(!pi2)
      return(-1);

   nRet = pi2->nItemID;

   // Save item corresponding to the selection
   piSel = GetItemByIdx(pme->m_pItemList, pme->m_nSelect);

   // Special Case: pi1->pi2 or pi2->pi1

   if(pi2Prev == pi1 || pi1Prev == pi2){  // CASE : Move Down/Up one position
      if(pi1Prev == pi2){  // CASE : Move Up one position
         // Exchange pi1 and pi2
         pTemp = pi1;
         pi1 = pi2;
         pi2 = pTemp;
         // Exchange pi1Prev and pi2Prev
         pTemp = pi1Prev;
         pi1Prev = pi2Prev;
         pi2Prev = pTemp;
      }

   // Start:   pi0 -> pi1 -> pi2 - pi3
   // 1) pi1 -> pi3
   // 2) pi2 -> pi1
   // 1) pi0 -> pi2
   // Result:  pi0 -> pi2 -> pi1 -> pi3

      pi1->pNext = pi2->pNext;
      pi2->pNext = pi1;
      if(pi1Prev)
         pi1Prev->pNext = pi2;
      else
         pme->m_pItemList = pi2;
   }
   else{

   // pi0 -> pi1 -> piA -> pi2
   // 1)  pi0 -> pi1 -> NULL, pi2 -> piA -> pi2 (loop)
   // 2)  pi0 -> pi2 -> piA -> pi2 (loop)
   // 3)  piA -> pi1 -> NULL
   // Result:  pi0 -> pi2 -> piA -> pi1 - NULL

      pTemp = pi1->pNext;
      pi1->pNext = pi2->pNext;
      pi2->pNext = pTemp;

      if(pi1Prev)
         pi1Prev->pNext = pi2;
      else
         pme->m_pItemList = pi2;
      if(pi2Prev)
         pi2Prev->pNext = pi1;
      else
         pme->m_pItemList = pi1;
   }

   // Restore Selection
   if(piSel)
      IMenuCtl_SetSel(po, piSel->nItemID);

   return(nRet);
}

/*=====================================================================

Public Method - Updates information regarding a menu item. See AEEMenu.h

======================================================================*/
static boolean IMenuCtl_SetItem(IMenuCtl * po, uint16 wID, uint16 wFlags, CtlAddItem * pai)
{
   CMenuCtl *  pme = (CMenuCtl *)po;
   CMenuItem * pmi,*pmiPrev;
   IImage *    pImage;
   const char *pszResImage;

   if(!pai)
      return(FALSE);

   pmi = GetItemByID(pme->m_pItemList,(int)wID, &pmiPrev);

   if(!pmi)
      return(FALSE);

   // Set the data 
   
   if(wFlags & MSIF_DATA)
      pmi->lData = pai->dwData;

   // If item is owner drawn, Ignore others

   if (IS_ITEM_OD(pmi))
      return(TRUE);

   // Set the font

   if(wFlags & MSIF_FONT)
      pmi->nFont = (AEEFont)pai->wFont;


   // Set the text

   if(wFlags & MSIF_TEXT){
      IMenuCtl_SetItemText(po,wID,pai->pszResText,pai->wText,(AECHAR *)pai->pText);

   // Must refetch the item - may have been reallocated...

      pmi = GetItemByID(pme->m_pItemList,(int)wID, NULL);

      if(!pmi)
         return(FALSE);
   }

   // Set the image
   
   if(wFlags & MSIF_IMAGE){
      aee_releaseobj((void **)&pmi->pImage);
      if(!Menu_ResolveImage(pme, pai, &pszResImage, &pImage))
         return(FALSE);

      pmi = Menu_AllocItem(pme, pmi, pmi->text, pszResImage, pmiPrev);

      if(!pmi){
         aee_releaseobj((void **)&pImage);
         return(FALSE);
      }
      if(pmi->pszImage) 
      {
        pmi->wImageID = pai->wImage;
        STRCPY((char*)pmi->pszImage, pai->pszResImage);     
      }

   // Pre-calc the image stuff in case we unload it...

      if(pImage){
         if(IS_MULTI_IMAGE(pme))
            IIMAGE_SetParm(pImage,IPARM_NFRAMES,(int)(IS_TRI_STATE(pme) ? 3 : 2),0);
         Menu_SetupImage(pme,pmi,pImage);
      }
   }

   // Recalculate Menu Layout if a new font or image is specified
   if ((wFlags & MSIF_FONT) || (wFlags & MSIF_IMAGE))
      Menu_Recalc(pme);

   return(TRUE);
}

/*=====================================================================

Public Method - Returns the item count in a menu.

======================================================================*/
static int IMenuCtl_GetItemCount(IMenuCtl * po)
{
   CMenuCtl * pme = (CMenuCtl *)po;

   return(pme->m_nItems);
}

/*=====================================================================

Public Method - Returns the item ID given the index value.

======================================================================*/
static uint16 IMenuCtl_GetItemID(IMenuCtl * po, int nIdx)
{
   CMenuCtl * pme = (CMenuCtl *)po;
   CMenuItem * pmi;
 
   if((pmi = GetItemByIdx(pme->m_pItemList, nIdx)) != NULL)
      return(pmi->nItemID);
   return(0);
}

/*=====================================================================

Public Method - Sorts the menu.  See AEEMenu.h

======================================================================*/
static void  IMenuCtl_Sort(IMenuCtl * po, AEEMenuSortType t, PFNMENUSORTCOMPARE pfn,void * pUser)
{
   CMenuCtl *     pme = (CMenuCtl *)po;
   MSortArgs      ms;
   MSortItem *    psi;
   CMenuItem *    pmi,*pmiNext;
   int            i;
   
   if(!pme->m_nItems)
      return;

   // If owner draw items are allowed, only custom and data based sorting are allowed
   if (OD_ITEMS_ALLOWED(pme) && ((t == MCS_NAME_DESCENDING) || (t == MCS_NAME_ASCENDING)))
      return;

   MEMSET(&ms, 0, sizeof(ms));
   ms.pfn   = pfn;
   ms.pUser = pUser;
   ms.t     = t;

   psi = (MSortItem *)MALLOC(sizeof(MSortItem) * pme->m_nItems);
   if(psi){
      for(pmi = pme->m_pItemList,i = 0; pmi && i < pme->m_nItems; pmi = pmi->pNext,i++){
         psi[i].pa = &ms;
         psi[i].pmi = pmi;
      }

   // Save the selected item.  We need to reset the index!

      pme->m_nSelect = 0;

   // Sort - Menu_SortCB does the hard work...

      QSORT(psi, pme->m_nItems, sizeof(MSortItem), Menu_SortCB);

   // List has been sorted, reorder the pointers

      pmi = pme->m_pItemList = psi->pmi;

      for(i = 1; i < pme->m_nItems;i++){
         pmiNext     = psi[i].pmi;
         pmi->pNext  = pmiNext;
         pmi         = pmiNext;
      }
      pme->m_pLastItem = pmi;
      pmi->pNext = NULL;

      FREE(psi);

   // Only redraw if it's allowed...

      if(!NO_REDRAW(pme))
      	{
      	 MSG_FATAL("IMenuCtl_Redraw redraw.......1111",0,0,0);
         IMenuCtl_Redraw(po);
      	}
   }
}

/*=====================================================================

Public Method - Returns information about the item.  See AEEMenu.h

======================================================================*/
static boolean IMenuCtl_GetItem(IMenuCtl * po, uint16 wID, CtlAddItem *pai)
{
   CMenuCtl *  pme = (CMenuCtl *)po;
   CMenuItem * pmi;

   if(!pai)
      return(FALSE);

   pmi = GetItemByID(pme->m_pItemList, (int)wID, NULL);
   if(!pmi)
      return(FALSE);

   MEMSET(pai,0,sizeof(CtlAddItem));
   pai->wFont        = (uint16)pmi->nFont;
   pai->dwData       = pmi->lData;
   pai->pImage       = pmi->pImage;
   pai->pszResImage  = pmi->pszImage;
   pai->wImage       = pmi->wImageID;
   pai->wItemID      = wID;
   pai->pText        = pmi->text;
   return(TRUE);
}

/*=====================================================================

Public Method - Returns information about the item.  See AEEMenu.h

======================================================================*/
static boolean IMenuCtl_GetItemData(IMenuCtl * po, uint16 nItemID, uint32 * plData)
{
   CMenuCtl *  pme = (CMenuCtl*)po;
   CMenuItem * pItem;

   if (plData) {
      pItem = GetItemByID(pme->m_pItemList, nItemID, NULL);
      if (pItem){
         *plData = pItem->lData;
         return(TRUE);
      }
   }
   return(FALSE);
}

/*=====================================================================

Public Method - Deletes an item from the menu.  See AEEMenu.h

======================================================================*/
static boolean IMenuCtl_DeleteItem(IMenuCtl * po, uint16 nItemID)
{
   CMenuCtl *  pme = (CMenuCtl*)po;
   CMenuItem * pmi, *pmiLast;
   int         nItemIdx;

   if((pmi = GetItemByIDEx(pme->m_pItemList, nItemID, &pmiLast, &nItemIdx)) != NULL){
      if(pmiLast){
         pmiLast->pNext = pmi->pNext;
         if( pmi == pme->m_pLastItem ){
            // Deleting the last item, so engage to prev [maybe NULL in empty list]
            pme->m_pLastItem = pmiLast;
         }
      }else{
         pme->m_pItemList = pmi->pNext;
         if(!pme->m_pItemList)
            pme->m_pLastItem = NULL;
      }
      Menu_FreeItem(pme, pmi);
      pme->m_nItems--;

      // Recalculate select if its more than or equal to the index of deleted item - CR05183
      if (pme->m_nSelect >= nItemIdx) {
         if (pme->m_nSelect)    
            pme->m_nSelect--; 
      }

      // Recalculate Menu Layout
      Menu_Recalc(pme);

      // Redraw Menu
      MSG_FATAL("IMenuCtl_Redraw redraw.......2222",0,0,0);
      IMenuCtl_Redraw(po);

      return(TRUE);
   }
   return(FALSE);
}

/*=====================================================================

Public Method - Deletes all items from the menu.  See AEEMenu.h

======================================================================*/
static boolean IMenuCtl_DeleteAll(IMenuCtl * po)
{
   CMenuCtl *  pme = (CMenuCtl*)po;
   CMenuItem * pmi = NULL;

   while ((pmi = pme->m_pItemList) != NULL){
      pme->m_pItemList = pmi->pNext;
      Menu_FreeItem(pme, pmi);
   }

   pme->m_bSKPageFullNotify = FALSE;
   pme->m_pItemList     = NULL;
   pme->m_pLastItem     = NULL;
   pme->m_nItems        = 0;
   pme->m_nSelect       = 0;
   pme->m_nScrollItem   = 0;
   return(TRUE);
}

/*=====================================================================

Public Method - Sets selection in single-selection menu. See AEEMenu.h

======================================================================*/
static void IMenuCtl_SetSel(IMenuCtl * po, uint16 nItemID)
{
   // Actual handling is done at Menu_SetSelbyIdx(). Passing an index value of -1
   // will resort to first item which matches the itemId
   Menu_SetSelbyIdx( (CMenuCtl *)po, nItemID, -1);

}

/*=====================================================================

Private Method - Sets selection in single-selection menu based on ItemID 
and item index. This can be used on menus with duplicate item IDs.

======================================================================*/
static void Menu_SetSelbyIdx(CMenuCtl * pme, uint16 nItemID, int nIdx)
{
   int         i,nDir,nCurr,nCurrScroll;
   CMenuItem * pmi;
   uint32      dwProps;

   // Find the item ID in the list.  Scroll the list if necessary to show it...

   pmi = pme->m_pItemList;


   // Find the matching item.  Need to do this rather than doing FindByID
   // so that we have the index value.

   for(i = 0; pmi ; pmi = pmi->pNext, i++){
      if( (pmi->nItemID == nItemID) && ( (nIdx == -1) || (nIdx == i) ) )
         break;
   }
   // If it's -1, and did not match an item reset the menu...
   if(nItemID == (uint16)-1 && !pmi){
      pmi = pme->m_pItemList;
      if(!pmi)
         return;
      nItemID = pmi->nItemID;
      i = 0;
   }

   if(!pmi)
      return;


   // If it's already selected, return...

   nCurr       = pme->m_nSelect;
   nCurrScroll = pme->m_nScrollItem;

   nDir = i - pme->m_nSelect;
   if(!nDir)
      return;

   dwProps = pme->m_dwProps;
   pme->m_dwProps |= MP_NO_REDRAW;

   switch(pme->m_cls){

   // If it is a menu, then do the following:
   //
   // - If visible, just show the item;
   // - Above visible viewport - make it top visible item
   // - Below visible viewport - make it last visible item
        
      case AEECLSID_MENUCTL:
         if(!ITEM_IS_VISIBLE(pme, i)){
            if(i < pme->m_nScrollItem)
               nDir = i;
            else{
               nDir = (i + 1) - pme->m_nPageItems;
               if(nDir < 0)
                  nDir = 0;
            }
            pme->m_nScrollItem = (uint16)nDir;
         }
         break;

   // If it's a softkey, cycle the selection until it's found...

      case AEECLSID_SOFTKEYCTL:
         while(pme->m_nSelect != i)
            SK_MoveSelect(pme, nDir);
         break;

   // If it is a icon menu, then do the following:
   //
   // - If visible, just show the item;
        
      case AEECLSID_ICONVIEWCTL:
         if (pme->m_bIconHScroll)
         {
            // - Left of visible viewport - make it leftmost visible item
            // - Right of visible viewport - make it rightmost visible item
            /*if(!ICON_IS_SINGLEROW_VISIBLE(pme, i)){
               if(i < pme->m_nScrollItem)
                  nDir = i;
               else{
                  nDir = (i + 1) - pme->m_nCols;
                  if(nDir < 0)
                     nDir = 0;
               }
               pme->m_nScrollItem = (uint16)nDir;
            } */
            int start, end;
            pme->m_nSelect = i; //Need this for Icon_CalcStartEndItemIndx()
            Icon_CalcStartEndItemIndx(pme, &start, &end);
            pme->m_nScrollItem = start;
         }
         else
         {
            // - Above visible viewport - make item row first visible row
            // - Below visible viewport - make item row last visible row
            if(!ICON_IS_VISIBLE(pme, i)){
               if(i < pme->m_nScrollItem * pme->m_nCols)
                  nDir = i/pme->m_nCols; // Index of item row
               else{
                  nDir = (i/pme->m_nCols) + 1;
                  nDir -= pme->m_nRows;
                  if(nDir < 0)
                     nDir = 0;
               }
               pme->m_nScrollItem = (uint16)nDir;
            }
         }
         break;
   }

   pme->m_nSelect = i;
   pme->m_dwProps = dwProps;

   if(!NO_REDRAW(pme)){
      if(pme->m_cls == AEECLSID_MENUCTL && 
         pme->m_nScrollItem == nCurrScroll && 
         ITEM_IS_VISIBLE(pme,nCurr) && 
         ITEM_IS_VISIBLE(pme,i))
         Menu_DrawNewSelectItem(pme,nCurr,i);
      else
      	{
      	  MSG_FATAL("IMenuCtl_Redraw redraw.......3333",0,0,0);
         IMenuCtl_Redraw((IMenuCtl *)pme);
      	}
   }

   // Notify them that the selection changed.
   if( !IS_MULTI_SEL(pme) ){
      if(nCurr != i)
         Menu_NotifySelChange(pme);
   }
}

static void IMenuCtl_SetSelEx(IMenuCtl * po, uint16 nItemID, boolean bSelected)
{
   CMenuCtl *     pme = (CMenuCtl*)po;
   CMenuItem *    pmi;

   // Make sure it is multi-sel
   if( !IS_MULTI_SEL(pme) )
      return;

   // Find the item
   for( pmi = pme->m_pItemList; pmi; pmi = pmi->pNext ){
      if( pmi->nItemID == nItemID )
         break;
   }
   // If the item id is -1 and noone else has this value...
   // Then set all menu items as asked and reset the menu
   if( !pmi && nItemID == (uint16)-1 ){
      for( pmi = pme->m_pItemList; pmi; pmi = pmi->pNext )
         pmi->bIsSel = bSelected;
      IMENUCTL_SetFocus(po, nItemID);
      Menu_NotifySelChange(pme);
      return;
   }
   // Invalid ID and not -1 special case
   if( !pmi )
      return;
   // Only if they differ update and notify
   if( pmi->bIsSel != bSelected ){
      pmi->bIsSel = bSelected;
      Menu_NotifySelChange(pme);
   }
   // Always Change Focus
   IMENUCTL_SetFocus(po, pmi->nItemID);
}

/*=====================================================================

Public Method - Returns selection on single-selection menu. See AEEMenu.h

======================================================================*/
static uint16 IMenuCtl_GetSel(IMenuCtl * po)
{
   CMenuCtl * pme = (CMenuCtl *)po;
   CMenuItem * pmi;
 
   if((pmi = GetItemByIdx(pme->m_pItemList, pme->m_nSelect)) != NULL)
      return(pmi->nItemID);
   return(0);
}

/*=====================================================================

Public Method - Enables the sending of the EVT_COMMAND event. See AEEMenu.h

======================================================================*/
static void IMenuCtl_EnableCommand(IMenuCtl * po, boolean bEnable)
{
   ((CMenuCtl*)po)->m_bSendCmd = (bEnable != 0);
}

/*=====================================================================

Public Method - Sets the text for a menu item. See AEEMenu.h

======================================================================*/
static void IMenuCtl_SetItemText(IMenuCtl * po, uint16 nItemID, const char * pszResFile, uint16 wResID, AECHAR * pText)
{
   CMenuCtl *  pme = (CMenuCtl*)po;
   CMenuItem * pmi, * pmiLast;
   boolean     bFreeText = FALSE;

   // must supply one of these

   pmi = GetItemByID(pme->m_pItemList, nItemID, &pmiLast);

   if (!pmi || (!pText && !pszResFile) || IS_ITEM_OD(pmi))
      return;

  // allocate and initialize a new menu item

   if(!pText && pszResFile){
      // Need to load text from resource file

      pText = (AECHAR *)MALLOC(MAX_MENU_TEXT * sizeof(AECHAR));

      if(!pText)
         return;

      bFreeText = TRUE;

      ISHELL_LoadResString(pme->m_pIShell, pszResFile, wResID, pText, MAX_MENU_TEXT * sizeof(AECHAR));
   }

   // Allocate the new item.  Note that the structure ALREADY contains the 
   // room for the 0 terminator on the string.  Thus, the size we need to allocate
   // is actually less the 0 terminator...

   Menu_AllocItem(pme, pmi, pText, NULL, pmiLast);

   Menu_Recalc(pme);

   if(bFreeText)
      FREE(pText);
}

/*=====================================================================

Public Method - Sets the time for a calendar menu item. See AEEMenu.h

======================================================================*/
static void IMenuCtl_SetItemTime(IMenuCtl * po, uint16 nItemID, uint16 wMinStart, uint16 wDuration)
{
   CMenuCtl *  pme = (CMenuCtl*)po;
   CMenuItem * pmi;

   // must supply one of these

   if(pme && !OD_ITEMS_ALLOWED(pme)){
      pme->m_dwProps |= MP_CALENDAR;
      pmi = GetItemByID(pme->m_pItemList, nItemID, NULL);
      if(pmi){
         pmi->wCalStart = wMinStart;
         pmi->wCalEnd = wMinStart + wDuration;
      }
   }
}

/*=====================================================================

Public Method - Returns the time for a calendar menu item. See AEEMenu.h

======================================================================*/
static int IMenuCtl_GetItemTime(IMenuCtl * po, uint16 nItemID, uint16 * pwDuration)
{
   CMenuCtl *  pme = (CMenuCtl*)po;
   CMenuItem * pmi;

   if (OD_ITEMS_ALLOWED(pme))
      return(-1);
    
   // must supply one of these

   pmi = GetItemByID(pme->m_pItemList, nItemID, NULL);

   if (pmi){
      if(pwDuration)
         *pwDuration = pmi->wCalEnd - pmi->wCalStart;
      return((int)pmi->wCalStart);
   }
   return(-1);
}

/*=====================================================================

Public Method - Sets the menu item style. See AEEMenu.h

======================================================================*/
static void IMenuCtl_SetStyle(IMenuCtl * po, AEEItemStyle * pNormal, AEEItemStyle * pSel)
{
   CMenuCtl * pme = (CMenuCtl *)po;

   if(pNormal)
   {
       MEMCPY(&pme->m_style[0], pNormal, sizeof(AEEItemStyle));
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
       pme->userSetStyle = TRUE;  
#endif
   }
   if(pSel)
   {
      MEMCPY(&pme->m_style[1], pSel, sizeof(AEEItemStyle));
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
       pme->userSetStyle = TRUE;  
#endif
   }

#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   if( pme->userSetStyle)
   {
       IMenuCtl_SetColors( (IMenuCtl*)pme, NULL);
   }
#endif   
   Menu_Recalc(pme);
}

/*=====================================================================

Public Method - Gets the menu item style. See AEEMenu.h

======================================================================*/
static void IMenuCtl_GetStyle(IMenuCtl * po, AEEItemStyle * pNormal, AEEItemStyle * pSel)
{
   CMenuCtl * pme = (CMenuCtl *)po;

   if(pNormal)
      MEMCPY(pNormal, &pme->m_style[0], sizeof(AEEItemStyle));

   if(pSel)
      MEMCPY(pSel, &pme->m_style[1], sizeof(AEEItemStyle));
}

/*=====================================================================

Public Method - Sets the menu colors. See AEEMenu.h

======================================================================*/
static void IMenuCtl_SetColors(IMenuCtl * po, AEEMenuColors * pc)
{
   CMenuCtl * pme = (CMenuCtl *)po;

   if(pc){
      if(pc->wMask & MC_BACK)
         pme->m_c.cBack = pc->cBack;
      if(pc->wMask & MC_TEXT)
         pme->m_c.cText = pc->cText;
      if(pc->wMask & MC_SEL_BACK)
         pme->m_c.cSelBack = pc->cSelBack;
      if(pc->wMask & MC_SEL_TEXT)
         pme->m_c.cSelText = pc->cSelText;
      if(pc->wMask & MC_FRAME)
         pme->m_c.cFrame = pc->cFrame;
      if(pc->wMask & MC_SCROLLBAR)
         pme->m_c.cScrollbar = pc->cScrollbar;
      if(pc->wMask & MC_SCROLLBAR_FILL)
         pme->m_c.cScrollbarFill = pc->cScrollbarFill;
      if(pc->wMask & MC_TITLE)
         pme->m_c.cTitle = pc->cTitle;
      if(pc->wMask & MC_TITLE_TEXT)
         pme->m_c.cTitleText = pc->cTitleText;
   #if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
       pme->userSetStyle = TRUE;  
   #endif
   }
   else{
	   
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
		if( !pme->userSetStyle && ( IS_MENU( pme) || IS_SOFTKEY_MENU( pme)))
		{
#if defined( FEATURE_FUNCS_THEME)		
		    Theme_Param_type themeParms;
		    Appscom_GetThemeParameters( &themeParms);
			pme->m_c.cBack          = themeParms.bkcolor;
#if defined( FEATURE_CONTROL_BG_USE_IMAGE)
			pme->m_c.cText          = IS_SOFTKEY_MENU( pme) ? themeParms.textColor : themeParms.bkcolor;
#else
			pme->m_c.cText          = themeParms.textColor;
#endif//#if defined( FEATURE_CONTROL_BG_USE_IMAGE)
			pme->m_c.cSelBack       = themeParms.themeColor;
			pme->m_c.cSelText       = themeParms.seltextColor;
			pme->m_c.cFrame         = CLR_SYS_FRAME_LO;
			pme->m_c.cScrollbar     = themeParms.textColor;
			pme->m_c.cScrollbarFill = themeParms.themeColor;
			pme->m_c.cTitle         = themeParms.themeColor;
			pme->m_c.cTitleText     = themeParms.textColor;
#else //#if defined( FEATURE_FUNCS_THEME)
            pme->m_c.cBack          = RGB_BLACK;
#ifdef FEATURE_WHITE_BG
            pme->m_c.cText          = RGB_BLACK;//MAKE_RGB( 0xde, 0xde, 0xde);
            pme->m_c.cSelText       = RGB_WHITE;//RGB_BLACK; //RGB_YELLOW_EX;//RGB_BLACK;//wlh mod 更改选中条的文字颜色
#else
            pme->m_c.cText          = MAKE_RGB( 0xde, 0xde, 0xde);
            pme->m_c.cSelText       = RGB_BLACK; //RGB_YELLOW_EX;//RGB_BLACK;//wlh mod 更改选中条的文字颜色
#endif            
            pme->m_c.cSelBack       = MAKE_RGB( 0xde, 0xde, 0xde);            
            pme->m_c.cFrame         = CLR_SYS_FRAME_LO;
            pme->m_c.cScrollbar     = MAKE_RGB( 0xde, 0xde, 0xde);
            pme->m_c.cScrollbarFill = MAKE_RGB(0xfe, 0x61, 0x01);
            pme->m_c.cTitle         = MAKE_RGB(0xfe, 0x61, 0x01);
            pme->m_c.cTitleText     = RGB_WHITE;
#endif//#if defined( FEATURE_FUNCS_THEME)  
		}
		else
		{

			pme->m_c.cBack          = CLR_SYS_ITEM;
			pme->m_c.cText          = CLR_SYS_ITEM_TEXT;
			pme->m_c.cSelBack       = CLR_SYS_ITEM_SEL;
			pme->m_c.cSelText       = CLR_SYS_ITEM_SEL_TEXT;
			pme->m_c.cFrame         = CLR_SYS_FRAME_LO;
			pme->m_c.cScrollbar     = CLR_SYS_SCROLLBAR;
			pme->m_c.cScrollbarFill = CLR_SYS_SCROLLBAR_FILL;
			pme->m_c.cTitle         = CLR_SYS_TITLE;
			pme->m_c.cTitleText     = CLR_SYS_TITLE_TEXT;
		}
#else//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
		pme->m_c.cBack          = CLR_SYS_ITEM;
		pme->m_c.cText          = CLR_SYS_ITEM_TEXT;
		pme->m_c.cSelBack       = CLR_SYS_ITEM_SEL;
		pme->m_c.cSelText       = CLR_SYS_ITEM_SEL_TEXT;
		pme->m_c.cFrame         = CLR_SYS_FRAME_LO;
		pme->m_c.cScrollbar     = CLR_SYS_SCROLLBAR;
		pme->m_c.cScrollbarFill = CLR_SYS_SCROLLBAR_FILL;
		pme->m_c.cTitle         = CLR_SYS_TITLE;
		pme->m_c.cTitleText     = CLR_SYS_TITLE_TEXT;		   
#endif//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   }
   
    if( pme->userSetStyle && IS_SOFTKEY_MENU(pme))
    {
        
        CMenuItem *pi = NULL;
        for( pi = pme->m_pItemList; pi != NULL; pi = pi->pNext)
        {
            pi->nFont = AEE_FONT_BOLD;
        }
        Menu_Recalc( pme);
    }   
}

/*=====================================================================

Public Method - Gets the menu colors. See AEEMenu.h

======================================================================*/
static void IMenuCtl_GetColors(IMenuCtl * po, AEEMenuColors * pc)
{
   CMenuCtl * pme = (CMenuCtl *)po;

   if(pc)
      MEMCPY(pc, &pme->m_c, sizeof(AEEMenuColors));
}

static void IMenuCtl_GetSelItemRect(IMenuCtl *po, AEERect * prc)
{
    CMenuCtl * pme = (CMenuCtl *)po;
    
//    // Check NULL prc
//    if(!prc)
//    {
//        return;
//    }
//    
//    *prc = pme->m_seltextrc;
    
// add these @08.03.04
    if( prc != NULL)
    {
        CMenuItem* item = GetItemByIdx( pme->m_pItemList, pme->m_nSelect);
        IMenuCtl_GetItemRect( po, pme->m_nSelect, prc);
        
        prc->x  += item->cxImage + 4;
        prc->dx -= item->cxImage + 4;
        
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
		 //pme->m_nItems <= 10
         if( !pme->userSetStyle && 
             ( (IS_PROP_SET( pme->m_dwProps, MP_BIND_ITEM_TO_NUMBER_KEY) && pme->m_nItems <= MENU_ITEM_MAX) ||
               IS_MULTI_SEL( pme)
             )
         )
         {
             prc->x  += 13;
             prc->dx -= 13;
         }
#endif//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE) 
    }

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    {
        nv_language_enum_type language;
        
        OEM_GetConfig(CFGI_LANGUAGE_SELECTION, &language, sizeof(language));
        if (  0 
#ifdef NV_LANGUAGE_HEBREW
        || (NV_LANGUAGE_HEBREW == language)
#endif
#ifdef NV_LANGUAGE_ARABIC
        || (NV_LANGUAGE_ARABIC == language)
#endif        
        )
        {
            prc->x = pme->m_rc.dx - (prc->x+prc->dx);
        }
    }
#endif
}

/*=====================================================================

Public Method - Gets the menu item rect. See AEEMenu.h

======================================================================*/
static void IMenuCtl_GetItemRect(IMenuCtl * po, uint16 nIdx, AEERect * prc)
{
   CMenuCtl * pme = (CMenuCtl *)po;

   // Check NULL prc
   if (!prc)
      return;

   // Clear prc contents
   MEMSET(prc, 0, sizeof(AEERect));

   // Return if index isn't valid
   if (nIdx > (pme->m_nItems - 1))
      return;

   // Calculate Item Rect
   switch(pme->m_cls){

      case AEECLSID_SOFTKEYCTL:
         SK_CalcItemRect(pme, nIdx, prc);
         break;

      case AEECLSID_LISTCTL:
         if (pme->m_nSelect == nIdx) {
            CMenuItem * pItem;
            if (NULL != (pItem = GetItemByIdx(pme->m_pItemList, pme->m_nSelect))) {
               List_CalcItemRect(pme, prc, pItem->nFont);
            }
         }
         break;

      case AEECLSID_MENUCTL:
         if (ITEM_IS_VISIBLE(pme, nIdx))
            Menu_CalcItemRect(pme, nIdx, prc);
         break;

      case AEECLSID_ICONVIEWCTL:
         Icon_CalcItemRect(pme, nIdx, prc);
         break;
   }

}

/*=====================================================================

Public Method - Initializes a multi selection menu for selection enumeration

======================================================================*/
static boolean IMenuCtl_EnumSelInit(IMenuCtl * po)
{
   CMenuCtl * pme = (CMenuCtl *)po;

   if( IS_MULTI_SEL(pme) ){
      pme->m_pEnum = pme->m_pItemList;
      if( !pme->m_pEnum )
         return FALSE;

      return TRUE;
   }
   return FALSE;
}

/*=====================================================================

Public Method - Returns next selection in enumeration of a multi selection 
menu

======================================================================*/
static uint16 IMenuCtl_EnumNextSel(IMenuCtl * po) 
{
   CMenuCtl *  pme = (CMenuCtl *)po;

   if( IS_MULTI_SEL(pme) ){
      CMenuItem * pItem;

      for( pItem = pme->m_pEnum; pItem; pItem = pItem->pNext ){
         if( pItem->bIsSel ){
            uint16 nRet    = pItem->nItemID;
            pme->m_pEnum   = pItem->pNext;
            return nRet;
         }
      }
      pme->m_pEnum = NULL;
   }
   return (uint16)-1;
}

/*=====================================================================

Public Method - Sets callbacks to be used for drawing/measuring owner draw
item. See AEEMenu.h

======================================================================*/
static int IMenuCtl_SetOwnerDrawCB(IMenuCtl * po, PFNOWNERDRAWCB pfnCB, void * pvContext, uint16 nODFlags)
{
   CMenuCtl *  pme = (CMenuCtl *)po;
   AEEODCBData cbData;

   // If callback and flag are NULL, reset menu and nullify owner drawn stuff
   if (!pfnCB && !nODFlags)
   {
      IMenuCtl_Reset(po);
      pme->m_pfnCB = NULL;
      pme->m_pvContext = NULL;
      pme->m_nODFlags = 0;
      return AEE_SUCCESS;
   }

   // Make sure that callback and flags are valid
   if (!pfnCB || !nODFlags)
      return EFAILED;

   pme->m_pfnCB = pfnCB;
   pme->m_pvContext = pvContext;
   pme->m_nODFlags = nODFlags;

   // Check if title is owner drawn. If so, ensure that there is no title text and measure
   // title rect
   if(IS_TITLE_OD(pme))
   {
      // Free Old Title
      aee_freeobj((void **)&pme->m_pTitle);

      // Initialize Callback Data
      MEMSET(&cbData, 0, sizeof(AEEODCBData));

      cbData.nSize = sizeof(AEEODCBData);
      cbData.nAction = ODA_MEASURE_TITLE;

      // Measure Title Rect
      pme->m_pfnCB(pme->m_pvContext, &cbData);

      // Cache Title Rect
      pme->m_rcTitle = cbData.rc;
   }

   // Check if scroll bar is owner drawn. If so, measure scroll bar width
   if(IS_SB_OD(pme))
   {
      MEMSET(&cbData, 0, sizeof(AEEODCBData));

      cbData.nSize = sizeof(AEEODCBData);
      cbData.nAction = ODA_MEASURE_SB;

      // Measure ScrollBar Width
      pme->m_pfnCB(pme->m_pvContext, &cbData);

      // Cache ScrollBar Width
      pme->m_nSBWidth = cbData.rc.dx;
   }

   return AEE_SUCCESS;
}

/*=====================================================================



   Local/Private Menu Methods



======================================================================*/
/*=====================================================================

Local Method - Returns first owner draw item from a menu

======================================================================*/
CMenuItem* MenuCtl_GetFirstODItem(CMenuCtl* pme)
{
   CMenuItem * pmi = pme->m_pItemList;

   while(pmi)
   {
      if (IS_ITEM_OD(pmi))
         return pmi;
      else
         pmi = pmi->pNext;
   }
   
   return NULL;
}

/*=====================================================================

Local Method - Adds an owner drawn menu item to the menu

======================================================================*/
static boolean MenuCtl_AddODItem(CMenuCtl * pme, uint16 nItemID, uint32 lData)
{
   CMenuItem *    pmiLast = NULL;
   CMenuItem *    pmi = NULL;
   CMenuItem *    pmiOD = NULL;
   AEEODCBData    cbData;

   // Check if we already added this item
   for(pmiLast = NULL, pmi = pme->m_pItemList; pmi != NULL; pmi = pmi->pNext) {
      pmiLast = pmi;
      if(pmi->nItemID == nItemID)
         return(FALSE);
   }

   // Allocate memory for the item. If allocation is successful, item is added to the
   // end of the list
   if ((pmi = Menu_AllocItem(pme, NULL, NULL, NULL, pmiLast)) == NULL)
      return(FALSE);

   // copy other stuff and set OD flag
   pmi->lData     = lData;
   pmi->nItemID   = nItemID;
   pmi->bOD       = TRUE;

   if (IS_MEASURE_ONCE(pme) && ((pmiOD = MenuCtl_GetFirstODItem(pme)) != NULL))
   {
      pmi->cxImage = pmiOD->cxImage;
      pmi->cyImage = pmiOD->cyImage;
      
      if (IS_ICON_VIEW(pme))
         pmi->cyText = pmiOD->cyText;
   }
   else
   {
      // Measure Height and Width of this item
      MEMSET(&cbData, 0, sizeof(AEEODCBData));

      cbData.nSize = sizeof(AEEODCBData);
      cbData.nAction = ODA_MEASURE_ITEM;
      cbData.nActionInfo = nItemID;
      cbData.nActionInfo <<= 16;
      pme->m_pfnCB(pme->m_pvContext, &cbData);

      // We cache height and width as image height and width
      pmi->cxImage = cbData.rc.dx;
      pmi->cyImage = cbData.rc.dy;

      // Check if they plan to draw text also
      if (IS_ICON_VIEW(pme))
      {  
         // Measure Text Height of this item
         MEMSET(&cbData, 0, sizeof(AEEODCBData));

         cbData.nSize = sizeof(AEEODCBData);
         cbData.nAction = ODA_MEASURE_ITEM_TEXT;
         cbData.nActionInfo = nItemID;
         cbData.nActionInfo <<= 16;
         pme->m_pfnCB(pme->m_pvContext, &cbData);

         // Cache text height
         pmi->cyText = cbData.rc.dy;
      }
   }

   // Recalc the entire layout of the menu based upon the size of the items and the style
   Menu_Recalc(pme);

   // For softkey, send EVT_CTL_SKMENU_PAGE_FULL if page full upon addition of this item
   if(IS_SOFTKEY_MENU(pme) && !pme->m_bSKPageFullNotify && pme->m_nItems >= pme->m_nPageItems){
      pme->m_bSKPageFullNotify = TRUE;
      ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_SKMENU_PAGE_FULL, 0, (uint32)pme);
   }
   
   return(TRUE);
}

/*=====================================================================

Local Method - Loads an image for a menu item from either a resource 
file or cached version in another item.  If cached, it saves the file
name and increments the reference count on the IImage.

======================================================================*/
static IImage * Menu_LoadImage(CMenuCtl * pme, const char * pszRes, uint16 wID, boolean * pbCached)
{
   CMenuItem * pi;

   // Need valid resource file
   if(!pszRes || !*pszRes)
      return(NULL);

   // Search all items for the same image.  If found, add to it's reference count and return it   
   for(pi = pme->m_pItemList; pi ; pi = pi->pNext){
      if(pi->pImage && pi->pszImage && pi->wImageID == wID && !STRCMP(pi->pszImage, pszRes)){
         *pbCached = TRUE;
         IIMAGE_AddRef(pi->pImage);
         return(pi->pImage);
      }
   }

   *pbCached = FALSE;

   return(ISHELL_LoadResImage(pme->m_pIShell, pszRes, wID));
}

/*=====================================================================

Local Method - Function resumed by the dispatcher to send commands
from a menu based upon the SELECT key being pressed.

======================================================================*/
static void Menu_SendCommand(CMenuCtl * pme)
{
   CMenuItem   *  pItem;
   DBGPRINTF("Menu_SendCommand EVT_COMMAND");
   if((pItem = GetItemByIdx(pme->m_pItemList, pme->m_nSelect)) != NULL)
      ISHELL_HandleEvent(pme->m_pIShell, EVT_COMMAND, (uint16)pItem->nItemID, (uint32)pItem->lData);
}

/*=====================================================================

Local Method - Compares two menu items.

======================================================================*/
static int Menu_SortCB(const void * p1, const void * p2)
{
   MSortItem *    psi1 = (MSortItem *)p1;
   MSortItem *    psi2 = (MSortItem *)p2;
   CMenuItem *    pi1,*pi2;


   if(p1 == NULL || p2 == NULL)
   {
      return 0;
   }

   pi1 = psi1->pmi;
   pi2 = psi2->pmi;

   if(psi1->pa == NULL)
   {
      return 0;
   }

   switch(psi1->pa->t){
      case MCS_NAME_ASCENDING:
         return(WSTRICMP(pi1->text,pi2->text));
      case MCS_NAME_DESCENDING:
         return(WSTRICMP(pi2->text,pi1->text));
      case MCS_PARAM_ASCENDING:
         return((int)pi1->lData - (int)pi2->lData);
      case MCS_PARAM_DESCENDING:
         return((int)pi2->lData - (int)pi1->lData);
      default:
         if(psi1->pa->pfn) {
            return(psi1->pa->pfn(psi1->pa->pUser, pi1->lData, pi2->lData));
         }
   }
   return(0);
}

/*=====================================================================

Local Method - Determines whether the image specified is in a resource
or associated with a previously cached version.

======================================================================*/
static boolean Menu_ResolveImage(CMenuCtl * pme, CtlAddItem * pai,const char ** ppszResImage, IImage ** ppImage)
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
            if ((pImage = Menu_LoadImage(pme,pszResImage,pai->wImage,&bCached)) == NULL)
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

Local Method - Determines the size of an image and deals with animation.

======================================================================*/
static void Menu_SetupImage(CMenuCtl * pme, CMenuItem * pmi, IImage * pImage)
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
   pmi->pImage = pImage;            // cache image pointer
}
/*=====================================================================

Local Method - Handles the Char message for typing first char to select.

======================================================================*/
static void Menu_HandleChar(CMenuCtl * pme, AECHAR ch)
{
   CMenuItem * pWalker, *pStart;
   AECHAR      szwTyped[2];
   AECHAR      szwBuf[2];

   MEMSET(szwTyped, 0, sizeof(szwTyped));
   MEMSET(szwBuf, 0, sizeof(szwBuf));
   *szwTyped = ch;
   WSTRUPPER(szwTyped);
   if((pStart = pWalker = GetItemByIdx(pme->m_pItemList, pme->m_nSelect)) != NULL){
      for( pWalker = (pWalker->pNext != NULL) ? pWalker->pNext : pme->m_pItemList; 
            pWalker != pStart; 
            pWalker = (pWalker->pNext != NULL) ? pWalker->pNext : pme->m_pItemList){
         if(pWalker->cxText){
            *szwBuf = pWalker->text[0];
            WSTRUPPER(szwBuf);
            if(*szwBuf == *szwTyped){
               Menu_ResetAutoScroll(pme);
               IMENUCTL_SetSel((IMenuCtl *)pme, pWalker->nItemID);
               Menu_AutoScroll(pme);
               return;
            }
         }
      }
   }
}

/*=====================================================================

Local Method - Handles all the pen events, moved here for handleevent clarity

======================================================================*/
static boolean Menu_HandlePens(CMenuCtl * pme, AEEEvent evtCode, uint16 wParam, uint32 dwParam)//int16 wXPos, int16 wYPos)
{
#ifndef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
	int16 wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
	int16 wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);
	int nClickCount = AEE_POINTER_GET_CLICKCOUNT((const char *)dwParam);
#else
   	int16 wXPos = (int16)AEE_GET_X(dwParam);
	int16 wYPos = (int16)AEE_GET_Y(dwParam);
	//int nClickCount = AEE_POINTER_GET_CLICKCOUNT((const char *)dwParam);//wlh 不需要双击启动
#endif //FEATURE_LCD_TOUCH_ENABLE 
    MSG_FATAL("Menu_HandlePens, evtCode=0x%x, m_cls=%d",evtCode, pme->m_cls, 0);
   	switch( evtCode ){

#ifndef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH		
		case EVT_POINTER_MOVE:
#else
		case EVT_PEN_MOVE:
#endif//FEATURE_LCD_TOUCH_ENABLE
      	{
         int         y, cy;
         int         nScrollItem, nItems, nPageItems, nPos;
         uint16      nRange;
         
         if( pme->m_ptTracker.cbHit & PTRCK_HIT_SCRL )
		 {
           // Need to do things in relation to where the pen is and whether on thumb or off
            if( pme->m_ptTracker.cbHit & PTRCK_HIT_THUMB ){
               // This logic is married to Menu_GetScrollBarRects()
               // It is inverted as in Solve for pme->m_nIdx
               if( wXPos >= ((pme->m_rc.x + pme->m_rc.dx) - pme->m_nSBWidth)
                  && wXPos < (pme->m_rc.x + pme->m_rc.dx) ){

                  if(IS_ICON_VIEW(pme)){

                     nItems = pme->m_nTotalRows;
                     nPageItems = pme->m_nRows;
                  }
                  else{
                     nItems = pme->m_nItems;
                     nPageItems = pme->m_nPageItems;
                  }
                  nScrollItem = pme->m_nScrollItem;

                  if (nItems > nPageItems) {
                     // Adjust for frame size
                     //cx = IS_SB_OD(pme) ? pme->m_nSBWidth : (pme->m_nSBWidth - 2*AEE_FRAME_SIZE);
                     //x = (pme->m_rc.x + pme->m_rc.dx - cx);
                     y = pme->m_rc.y;

                     if(IS_MENU(pme))
                        cy = nPageItems * pme->m_cyMaxItem;
                     else
                        cy = pme->m_rc.dy;

                     // In an icon view, the scrollbar sits beside the entire icon window.  That
                     // means everything EXCEPT the text area...
                     if(IS_ICON_VIEW(pme)){
                        if(pme->m_bHasText){
                           uint16 cyText = Icon_GetMaxTextHeight(pme);
                           cy -= cyText;
                           if(ALIGN_TEXT_TOP(pme))
                              y += cyText;
                        }
                     }
                     else{       
                        if(pme->m_pTitle || IS_TITLE_OD(pme)){
                           if(!IS_MENU(pme))
                              cy -= pme->m_rcTitle.dy;
                           y += pme->m_rcTitle.dy;
                        } 
                     }    

                     if (IS_SB_OD(pme))
                        nRange = cy;
                     else
                     {
                        nRange = cy - 2*AEE_FRAME_SIZE;
                        if (WRAP_SCROLL(pme))
                           nRange -= 2*CXARROW;
                     }
 
                     nRange *= 10;    // multiply by 10 for rounding

                     //nLen = (nRange * nPageItems)/nItems;      // Scroll Bar Length    
                     nPos = (nRange * nScrollItem)/nItems;     // Scroll Bar Position
                     nPos = (((((wYPos+5)-y)-pme->m_ptTracker.wData)*10)*nItems)/nRange; // Rounding
                     if(nPos < 0 ){
                        nPos = 0;
                     }else if(nPos > (nItems - nPageItems) ){
                        nPos = nItems - nPageItems;
                     }
                     pme->m_nScrollItem = (uint16)nPos;
					 MSG_FATAL("IMenuCtl_Redraw redraw.......4444",0,0,0);
                     IMenuCtl_Redraw((IMenuCtl *)pme);
                     if(IS_ICON_VIEW(pme)){
                        // Icons need to redraw text, as focus is not moved!
                        CMenuItem * pmi = GetItemByIdx(pme->m_pItemList, pme->m_nSelect);
                        if(pmi){
                           if (IS_ITEM_OD(pmi))
                              Icon_DrawODItemText(pme, pmi, 0, TRUE);
                           else
                              Icon_DrawItemText(pme, pmi, 0, TRUE);
                        }
                     }
                  }
               }
            }else{
               // Need to check the timer, since this only cares
               // IF the pen is moved when the thumb is stalled
               // AND the move was in the direction of the flow
               if( !(pme->m_ptTracker.cbFlags & PTRCK_GEN_TMRSET) ){
                  Menu_ScrollByPos(pme, ((pme->m_ptTracker.cbHit & PTRCK_HIT_BELOW) ? 1 : -1), wXPos, wYPos);
               }
            }
            return TRUE;
         }
		 else 
		 { // pen down outside of scrollbar area
#if 0
            int         nRounding = 1;
            
            if( IS_SOFTKEY_MENU(pme) || IS_LIST_MENU(pme) )
               return TRUE;//no scrolling on softkey/list menu
            else if(IS_ICON_VIEW(pme)){
               nItems = pme->m_nTotalRows;
               nPageItems = pme->m_nRows;
            }
            else{
               nItems = pme->m_nItems;
               nPageItems = pme->m_nPageItems;
            }
            nScrollItem = pme->m_nScrollItem;
            
            if ( (nItems > nPageItems) && (nPageItems > 0) ){
               // Adjust for frame size
               //cx = IS_SB_OD(pme) ? pme->m_nSBWidth : (pme->m_nSBWidth - 2*AEE_FRAME_SIZE);
               //x = (pme->m_rc.x + pme->m_rc.dx - cx);
               //y = pme->m_rc.y;

               if(IS_MENU(pme))
                  cy = nPageItems * pme->m_cyMaxItem;
               else
                  cy = pme->m_rc.dy;

               // In an icon view, the scrollbar sits beside the entire icon window.  That
               // means everything EXCEPT the text area...
               if(IS_ICON_VIEW(pme)){
                  if(pme->m_bHasText){
                     uint16 cyText = Icon_GetMaxTextHeight(pme);
                     cy -= cyText;
                     //if(ALIGN_TEXT_TOP(pme))
                       // y += cyText;
                  }
               }
               else{       
                  if(pme->m_pTitle || IS_TITLE_OD(pme)){
                     if(!IS_MENU(pme))
                        cy -= pme->m_rcTitle.dy;
                     //y += pme->m_rcTitle.dy;
                  } 
               }    

               if (IS_SB_OD(pme))
                  nRange = cy;
               else
               {
                  nRange = cy - 2*AEE_FRAME_SIZE;
                  if (WRAP_SCROLL(pme))
                     nRange -= 2*CXARROW;
               }

               nRounding = nRange/nPageItems; // this is average height of one item

               /* Move one position for every scrolling equal to 1/3 the height of item */
               nPos = (int)( ((double)(wYPos - pme->m_ptPrevious.y) / nRounding) * 3 );  
               
               //Move the contents in the same direction as mouse/finger. Scrollbar moves in opposite direction
               nPos *= -1; 

               if(nPos > (nItems - nPageItems) )
                  nPos = nItems - nPageItems;

               if( (nPos != 0) && ( nPos + pme->m_nScrollItem >= 0)
                  && (nPos + pme->m_nScrollItem + nPageItems <= nItems) ) //scrolling necessary
               {
                  pme->m_nScrollItem += (uint16)nPos;
                  IMenuCtl_Redraw((IMenuCtl *)pme);
                  if(IS_ICON_VIEW(pme)){
                     // Icons need to redraw text, as focus is not moved!
                     CMenuItem * pmi = GetItemByIdx(pme->m_pItemList, pme->m_nSelect);
                     if(pmi){
                        if (IS_ITEM_OD(pmi))
                           Icon_DrawODItemText(pme, pmi, 0, TRUE);
                        else
                           Icon_DrawItemText(pme, pmi, 0, TRUE);
                     }
                  }
                  pme->m_ptPrevious.x = wXPos;
                  pme->m_ptPrevious.y = wYPos;
               }// nPos != 0
            } //(nItems > nPageItems)
#endif
            return TRUE;
         	}
      	}
      //return FALSE; //never reaches here
#ifndef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH			
		case EVT_POINTER_UP:
#else
		case EVT_PEN_UP :
#endif//FEATURE_LCD_TOUCH_ENABLE
		{
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH 只对EVT_PEN_UP 的响应做处理，这里说的是发送事件回当前applet
			AEECLSID  pCLSID = ISHELL_ActiveApplet (pme->m_pIShell);
            MSG_FATAL("Menu_HandlePens, EVT_PEN_UP==%x",pme->m_cls,0,0);
			if(pme->m_cls != AEECLSID_SOFTKEYCTL)
		  	{
			  	AEERect bottomBarRect;
				//int ht;
				int nBarH ;
				AEEDeviceInfo devinfo;
				nBarH = GetBottomBarHeight(pme->m_pIDisplay);
			  	//bottomBarRect.x = pme->m_rc.x;
				if(pme->bottomBarType != BTBAR_NONE)
				{
					//ht = pme->m_rc.y - (pme->m_rc.dy - GetTitleBarHeight( pme->m_pIDisplay));
					//if(ht < 0)
					//	ht = 0 - ht;
					//bottomBarRect.y = ht;
					MEMSET(&devinfo, 0, sizeof(devinfo));
					ISHELL_GetDeviceInfo(pme->m_pIShell, &devinfo);
					SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
				}
				else
				{
					bottomBarRect.y = pme->m_rc.y;
				}
				//bottomBarRect.dx = pme->m_rc.dx;
				//bottomBarRect.dy = GetBottomBarHeight( pme->m_pIDisplay); 
				MSG_FATAL("bottomBarRect.y [%d] wXPos [%d] wYPos [%d]",bottomBarRect.y,wXPos,wYPos);
				if( PT_IN_RECT(wXPos, wYPos, bottomBarRect) )
				{
					//判断左中右
					if(wXPos >= bottomBarRect.x && wXPos < bottomBarRect.x + (bottomBarRect.dx/3) )//左
					{
						uint16 selid = IMENUCTL_GetSel ((IMenuCtl *)&pme->vtIMenuCtl);//wlh 获得当前选中的条目资源ID
						boolean rt =  ISHELL_PostEvent(pme->m_pIShell,pCLSID,EVT_USER,AVK_SELECT,selid);
						return rt;
					}
					else if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)   && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pme->m_pIShell,pCLSID,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pme->m_pIShell,pCLSID,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}
		  	}
#endif//FEATURE_LCD_TOUCH_ENABLE
         	if( PT_IN_RECT(wXPos, wYPos, pme->m_rc) ){
            MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));
            switch( pme->m_cls ){
            case AEECLSID_SOFTKEYCTL:
               {
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH	
				  CMenuItem * pItem;
#endif//FEATURE_LCD_TOUCH_ENABLE
                  uint16 wIdx;
                  MSG_FATAL("Menu_HandlePens, AEECLSID_SOFTKEYCTL",0,0,0);
                  pme->m_ptTracker.cbHit = SK_HandlePen(pme, wXPos, wYPos, &pme->m_ptTracker.wData, &wIdx);
                  Menu_SetSelbyIdx(pme, pme->m_ptTracker.wData, -1);//add by xuhui
                  /* Index returned above may not be correct if there are 
                  * duplicate item ids - Need to address this
                  */ 
                  if( pme->m_ptTracker.cbHit == PTRCK_HIT_ITEM ){
                     // If an item is hit, change focus. Do not select it at this time.
                     IMENUCTL_SetFocus((IMenuCtl *)pme, pme->m_ptTracker.wData);
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH	
                     pme->m_ptTracker.cbHit = PTRCK_HIT_ITEM;
                     if( (pItem = GetItemByIdx(pme->m_pItemList, wIdx)) != NULL ){
					 	if( pItem->nItemID == IMENUCTL_GetFocus((IMenuCtl *)pme)) {
                        	(void)Menu_HandleSelection(pme, wIdx);
                         } else {
                             IMENUCTL_SetFocus((IMenuCtl *)pme, pme->m_ptTracker.wData);
                         }
                     }
#endif//FEATURE_LCD_TOUCH_ENABLE
                  }
               }
               break;
            case AEECLSID_MENUCTL:
               {
                  AEERect  rct;
                  int      nIdx  = pme->m_nScrollItem;
                  int      nMax  = pme->m_nPageItems + nIdx;

                  if(nMax > pme->m_nItems)
                     nMax = pme->m_nItems;

                  if( !IS_CALENDAR_VIEW(pme) ){
                     // Get a Displayed Item's rect
                     Menu_CalcItemRect(pme, nIdx, &rct);
                     if( wXPos < (rct.x + rct.dx) ){
                        // In an Item
                        for(; nIdx < nMax; nIdx++){
                           if( PT_IN_RECT(wXPos, wYPos, rct) ){
                              CMenuItem * pItem;
                              pme->m_ptTracker.cbHit = PTRCK_HIT_ITEM;
                              if( (pItem = GetItemByIdx(pme->m_pItemList, nIdx)) != NULL ){
#ifndef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH	
                                 if( pItem->nItemID == IMENUCTL_GetFocus((IMenuCtl *)pme) &&
                                    (2 == nClickCount)) {
#else
								if( pItem->nItemID == IMENUCTL_GetFocus((IMenuCtl *)pme)) {
#endif//FEATURE_LCD_TOUCH_ENABLE
                                    (void)Menu_HandleSelection(pme, nIdx);
                                 } else {
                                    pme->m_ptTracker.wData = (uint16)nIdx;
                                    //Need to pass both id & index as there can be duplicate item ids
                                    Menu_SetSelbyIdx(pme, pItem->nItemID, nIdx);
									(void)Menu_HandleSelection(pme, nIdx);//点击触发动作
                                 }
                              }
                              break;
                           }
                           rct.y += rct.dy;
                        }
                     }else{
                        // In the scrollbar
                        AEERect rctFrame, rctThumb;
                        if( !Menu_GetScrollBarRects(pme, &rctFrame, &rctThumb, NULL, NULL) ){
                           // Check the overall frame first to see if it is within the vertical scrollbar element
                           if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){
                              // Check the thumb first!
                              // Don't want to restrict the thumb from the scroll edges, 
                              // so entire width is valid given it is in vertical thumb range
                              if( wYPos >= rctThumb.y && wYPos <= (rctThumb.y + rctThumb.dy) ){
                                 pme->m_ptTracker.cbHit = PTRCK_HIT_THUMB;
                              }else if( wYPos < rctThumb.y ){
                                 pme->m_ptTracker.cbHit = PTRCK_HIT_ABOVE;
                              }else{
                                 pme->m_ptTracker.cbHit = PTRCK_HIT_BELOW;
                              }
                              pme->m_ptTracker.cbHit |= PTRCK_HIT_SCRL;
                              pme->m_ptTracker.wData = wYPos - rctThumb.y;
                              pme->m_ptTracker.ptPosition.x = wXPos;
                              pme->m_ptTracker.ptPosition.y = wYPos;
                              if( pme->m_ptTracker.cbHit & PTRCK_NOTTHUMB ){
                                 Menu_ScrollTimerCB(pme);
                                 if( !ISHELL_SetTimer(pme->m_pIShell, pme->m_arPenRepeat.dwStart, (PFNNOTIFY)Menu_ScrollTimerCB, (void *)pme) ){
                                    pme->m_ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
                                 }
                              }
                           }
                        }
                     }
                  }else{
                     int nFont;
                     // Get a Displayed Item's rect
                     Menu_CalcItemRect(pme, nIdx, &rct);
                     if( wXPos < (rct.x + rct.dx) ){
                        // In an Item
                        int         i;
                        AEERect     rct;
                        CMenuItem * p = GetItemByIdx(pme->m_pItemList, pme->m_nScrollItem);
                        uint16         cyLeft,cyFont;
                        // Check if no menu item is present
                        if (p != NULL ) {
                           // Use the MenuItem font instead of default MenuCtl font
                           nFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, p->nFont, NULL, NULL);	
                           // use pme->m_cyFont if EFAILED is returned by IDISPLAY_GetFontMetrics
                           if (nFont == EFAILED) {
                              nFont = pme->m_cyFont;
                           }
                        } else {
                           nFont = pme->m_cyFont;
                        }
                        cyFont = nFont;
                     Cal_CalcPageItems(pme);
                     rct = pme->m_rc;
                     cyLeft = rct.dy;

                     if(pme->m_pTitle || IS_TITLE_OD(pme)){
                        cyLeft -= pme->m_rcTitle.dy;
                        rct.y += pme->m_rcTitle.dy;
                        rct.dy -= pme->m_rcTitle.dy; 
                     }

                     for (i = 0; i < pme->m_nPageItems && p; p = p->pNext, i++){
                        rct.dy = GetItemHeight(p, &cyLeft, cyFont);
                        if( PT_IN_RECT(wXPos, wYPos, rct) ){
                           pme->m_ptTracker.cbHit = PTRCK_HIT_ITEM;
                           pme->m_ptTracker.wData = (uint16) i + pme->m_nScrollItem;
                              //Need to pass both id & index as there can be duplicate item ids
                              Menu_SetSelbyIdx(pme, p->nItemID, pme->m_ptTracker.wData );
                           break;
                        }
                        rct.y += rct.dy;
                        }
                     } else { 
                        // In the scrollbar
                        AEERect rctFrame, rctThumb;
                        if( !Menu_GetScrollBarRects(pme, &rctFrame, &rctThumb, NULL, NULL) ){
                           // Check the overall frame first to see if it is within the vertical scrollbar element
                           if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){
                              // Check the thumb first!
                              // Don't want to restrict the thumb from the scroll edges, 
                              // so entire width is valid given it is in vertical thumb range
                              if( wYPos >= rctThumb.y && wYPos <= (rctThumb.y + rctThumb.dy) ){
                                 pme->m_ptTracker.cbHit = PTRCK_HIT_THUMB;
                              }else if( wYPos < rctThumb.y ){
                                 pme->m_ptTracker.cbHit = PTRCK_HIT_ABOVE;
                              }else{
                                 pme->m_ptTracker.cbHit = PTRCK_HIT_BELOW;
                              }
                              pme->m_ptTracker.cbHit |= PTRCK_HIT_SCRL;
                              pme->m_ptTracker.wData = wYPos - rctThumb.y;
                              pme->m_ptTracker.ptPosition.x = wXPos;
                              pme->m_ptTracker.ptPosition.y = wYPos;
                              if( pme->m_ptTracker.cbHit & PTRCK_NOTTHUMB ){
                                 Menu_ScrollTimerCB(pme);
                                 if( !ISHELL_SetTimer(pme->m_pIShell, pme->m_arPenRepeat.dwStart, (PFNNOTIFY)Menu_ScrollTimerCB, (void *)pme) ){
                                    pme->m_ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
                                 }
                              }
                           }
                        }
                     }
                  }
               }
				  return FALSE;
               break;
            case AEECLSID_ICONVIEWCTL:
               {
                  AEERect rctFrame, rctThumb;
                  if( !Menu_GetScrollBarRects(pme, &rctFrame, &rctThumb, NULL, NULL) ){

                     if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){
                        // Scrollbar!

                        // Check the thumb first!
                        // Don't want to restrict the thumb from the scroll edges, 
                        // so entire width is valid given it is in vertical thumb range
                        if( wYPos >= rctThumb.y && wYPos <= (rctThumb.y + rctThumb.dy) ){
                           pme->m_ptTracker.cbHit = PTRCK_HIT_THUMB;
                        }else if( wYPos < rctThumb.y ){
                           pme->m_ptTracker.cbHit = PTRCK_HIT_ABOVE;
                        }else{
                           pme->m_ptTracker.cbHit = PTRCK_HIT_BELOW;
                        }
                        pme->m_ptTracker.cbHit |= PTRCK_HIT_SCRL;
                        pme->m_ptTracker.wData = wYPos - rctThumb.y;
                        pme->m_ptTracker.ptPosition.x = wXPos;
                        pme->m_ptTracker.ptPosition.y = wYPos;
                        if( pme->m_ptTracker.cbHit & PTRCK_NOTTHUMB ){
                           Menu_ScrollTimerCB(pme);
                           if( !ISHELL_SetTimer(pme->m_pIShell, pme->m_arPenRepeat.dwStart, (PFNNOTIFY)Menu_ScrollTimerCB, (void *)pme) ){
                              pme->m_ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
                           }
                        }
                     }else{ //outside of scrollbar 
                        int nItem, nID;
                        CMenuItem *pItem;
                        if( Icon_MapPointToItem(pme, wXPos, wYPos, &nItem, &nID) ){
                           pme->m_ptTracker.cbHit = PTRCK_HIT_ITEM;
                           pme->m_ptTracker.wData = (uint16)nID;
                           if ( (pItem = GetItemByIdx(pme->m_pItemList, nID)) != NULL) {
#ifndef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
							  if ( pItem->nItemID == IMENUCTL_GetFocus((IMenuCtl *)pme) &&
                                 (2 == nClickCount) ) {
#else
							  if ( pItem->nItemID == IMENUCTL_GetFocus((IMenuCtl *)pme)) {
#endif//FEATURE_LCD_TOUCH_ENABLE
                                    (void)Menu_HandleSelection(pme, nID);
                                 }
                              else {
                                 /* IMENUCTL_SETFocus or SetSel may not highlight 
                                 the correct item when there are duplicate item 
                                 ids. Need to highlight based on index.
                                 */
                                 Menu_SetSelbyIdx(pme, (uint16)nItem, nID);
                              }
                           }
                        }
                     }
                  }else{ //no vertical scrollbar
                     
                     //if( pme->m_bIconHScroll ){ //single line icon view
                        AEERect  rctText;
                        uint16   cyText = Icon_GetMaxTextHeight(pme);

                        SETAEERECT(&rctText, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, cyText);
                        if(!ALIGN_TEXT_TOP(pme))
                           rctText.y += (pme->m_rc.dy - cyText);
                        rctText.dy++;

                        if( (pme->m_bIconHScroll) && PT_IN_RECT(wXPos, wYPos, rctText) ){
                           if( !NO_ARROWS(pme) ){
                              uint16 nKey = AVK_UNDEFINED;
                              if( wXPos <= (rctText.x + (CXARROW + 1)) ){
                                 pme->m_ptTracker.cbHit = PTRCK_HIT_ABOVE;
                                 nKey = AVK_LEFT;
                              }else if( wXPos >= ((rctText.x + rctText.dx) - (CXARROW + 1)) ){
                                 pme->m_ptTracker.cbHit = PTRCK_HIT_BELOW;
                                 nKey = AVK_RIGHT;
                              }
                              /* As this is single icon per frame, its just easier to 
                              * call HandleKey() instead of handling the scrolling here
                              */
                              Icon_HandleKey(pme, nKey);
                           }
                        
                        }else if ( (pme->m_bIconHScroll) && (pme->m_nCols == 1) ) { //single icon per frame case
                           /* Calling Icon_MapPointToItem is not of much use in this case as
                           * it will always return the first item (row=col=0). Since its 
                           * single icon per frame, the selection or focus can be applied on
                           * current selected item.
                           */
                           AEERect rct;
                           IMENUCTL_GetItemRect( (IMenuCtl *)pme, pme->m_nSelect, &rct);
                           //check to make sure the click is in icon's rectangle area
                           if (PT_IN_RECT (wXPos, wYPos, rct)) {
#ifndef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH						   	
                              if (2 == nClickCount)
                                 (void)Menu_HandleSelection(pme, pme->m_nSelect);
                              else
                              {  
                                 CMenuItem   * pItem;
                                 if((pItem = GetItemByIdx(pme->m_pItemList, pme->m_nSelect)) != NULL)
                                    Menu_SetSelbyIdx(pme, pItem->nItemID, pme->m_nSelect);
                                 //Need to pass both id & index as there can be duplicate item ids
                              }
#else
							(void)Menu_HandleSelection(pme, pme->m_nSelect);
#endif//FEATURE_LCD_TOUCH_ENABLE
                           }    
                        }else{//clicked elsewhere on an icon area
                           int nItem, nID;
                           CMenuItem *pItem;
                           if( Icon_MapPointToItem(pme, wXPos, wYPos, &nItem, &nID) ){
                              pme->m_ptTracker.cbHit = PTRCK_HIT_ITEM;
                              pme->m_ptTracker.wData = (uint16)nID;
                              if ( (pItem =  GetItemByIdx(pme->m_pItemList, nID)) != NULL) {
#ifndef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
								 if ( pItem->nItemID == IMENUCTL_GetFocus((IMenuCtl *)pme) &&
                                    (2 == nClickCount) ) {
#else
								if ( pItem->nItemID == IMENUCTL_GetFocus((IMenuCtl *)pme) ) {
#endif//FEATURE_LCD_TOUCH_ENABLE
                                       (void)Menu_HandleSelection(pme, nID);
                                    }
                                 else {
                                    //Need to pass both id & index as there can be duplicate items
                                    Menu_SetSelbyIdx(pme, (uint16)nItem, nID);
                                 }
                              }
                           }
                        }
                     } // non scrollbar area
               }
               break;
            case AEECLSID_LISTCTL:
               {
                  pme->m_ptTracker.cbHit = PTRCK_HIT_ITEM;
#ifndef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
                  if (2 == AEE_POINTER_GET_CLICKCOUNT((const char *)dwParam))
#endif//FEATURE_LCD_TOUCH_ENABLE
				  {
                     (void)Menu_HandleSelection(pme, pme->m_nSelect);
                  }
               }
               break;
            }
            pme->m_ptPrevious.x = wXPos;
            pme->m_ptPrevious.y = wYPos;
            return TRUE;
         }
      }
      return FALSE;
#ifndef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH		
	  case EVT_POINTER_DOWN:
#else
      case EVT_PEN_DOWN:
#endif//FEATURE_LCD_TOUCH_ENABLE
      {
         boolean bRet = FALSE;
         if( PT_IN_RECT(wXPos, wYPos, pme->m_rc) ){
            switch( pme->m_cls ){
            case AEECLSID_SOFTKEYCTL:
               {
                  uint16   wData, wIdx;
                  uint8    cbHit;
                  if( ((cbHit = SK_HandlePen(pme, wXPos, wYPos, &wData, &wIdx)) == pme->m_ptTracker.cbHit) ){
                     switch( cbHit ){
                     case PTRCK_HIT_ITEM:
                        if( wData == pme->m_ptTracker.wData ){
                           (void)Menu_HandleSelection(pme, wIdx);
                           bRet = TRUE;
                        }
                        break;
                     case PTRCK_HIT_BELOW:
                        SK_MoveSelect(pme, 1);
                        bRet = TRUE;
                        break;
                     case PTRCK_HIT_ABOVE:
                        SK_MoveSelect(pme, -1);
                        bRet = TRUE;
                        break;
                     }
                  }
               }
               break;
            case AEECLSID_MENUCTL:
               {
                  if( pme->m_ptTracker.cbHit == PTRCK_HIT_ITEM ){
                     if( !IS_CALENDAR_VIEW(pme) ){
                        // Regular menus don't need additional action. Command is 
                        // executed for dobule-click in EVT_POINTER_DOWN
                     }else{
                        int         i, nFont;
                        AEERect     rct;
                        uint16          cyLeft,cyFont;
                        CMenuItem * p = GetItemByIdx(pme->m_pItemList, pme->m_nScrollItem);

                        // Check if no menu item is present
                        if (p != NULL ) {
                           // Use the MenuItem font instead of default MenuCtl font
                           nFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, p->nFont, NULL, NULL);	
                           // use pme->m_cyFont if EFAILED is returned by IDISPLAY_GetFontMetrics
                           if (nFont == EFAILED) {
                              nFont = pme->m_cyFont;
                           }
                        } else {
                           nFont = pme->m_cyFont;
                        }
                        cyFont = nFont;
                     
                        Cal_CalcPageItems(pme);
                        rct = pme->m_rc;
                        cyLeft = rct.dy;

                        if(pme->m_pTitle || IS_TITLE_OD(pme)){
                           cyLeft -= pme->m_rcTitle.dy;
                           rct.y += pme->m_rcTitle.dy;
                           rct.dy -= pme->m_rcTitle.dy; 
                        }

                        for (i = 0; i < pme->m_nPageItems && p; p = p->pNext, i++){
                           rct.dy = GetItemHeight(p, &cyLeft, cyFont);
                           if( PT_IN_RECT(wXPos, wYPos, rct) ){
                              if( pme->m_ptTracker.wData == (uint16)(i + pme->m_nScrollItem) ){
                                 (void)Menu_HandleSelection(pme, pme->m_ptTracker.wData);
                                 bRet = TRUE;
                              }
                              break;
                           }
                           rct.y += rct.dy;
                        }
                     }
                  }else{
                     // Scrollbar needs no extra action, Timer is killed below
                  }
               }
               break;
            case AEECLSID_ICONVIEWCTL:
               //{
               //   if( pme->m_ptTracker.cbHit == PTRCK_HIT_ITEM ){
               //      int nItem, nID;

               //      if( pme->m_bIconVScroll && Icon_MapPointToItem(pme, wXPos, wYPos, &nItem, &nID) ){
               //         if( pme->m_ptTracker.wData == (uint16)nID ){
               //            (void)Menu_HandleSelection(pme, nID);
               //            bRet = TRUE;
               //         }
               //      }else if( pme->m_bIconHScroll ){
               //         AEERect  rctText;
               //         uint16   cyText = Icon_GetMaxTextHeight(pme);

               //         SETAEERECT(&rctText, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, cyText);
               //         if(!ALIGN_TEXT_TOP(pme))
                            //      rctText.y += (pme->m_rc.dy - cyText);
               //         rctText.dy++;

               //         if( !PT_IN_RECT(wXPos, wYPos, rctText) && pme->m_ptTracker.wData == pme->m_nSelect ){
               //            (void)Menu_HandleSelection(pme, pme->m_nSelect);
               //            bRet= TRUE;
               //         }
               //      }else if (Icon_MapPointToItem(pme, wXPos, wYPos, &nItem, &nID)){
               //         if( pme->m_ptTracker.wData == (uint16)nID ){
               //            (void)Menu_HandleSelection(pme, nID);
               //            bRet = TRUE;
               //         }
               //      }
               //   }else{
               //      if( pme->m_bIconHScroll && !NO_ARROWS(pme) ){
               //         AEERect  rctText;
               //         uint16   cyText = Icon_GetMaxTextHeight(pme);

               //         SETAEERECT(&rctText, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, cyText);
               //         if(!ALIGN_TEXT_TOP(pme))
                            //      rctText.y += (pme->m_rc.dy - cyText);
               //         rctText.dy++;


               //         if( PT_IN_RECT(wXPos, wYPos, rctText) ){
               //            // Handle as if a appropriate key was pressed for code savings...
               //            if( wXPos <= (rctText.x + (CXARROW + 1)) ){
               //               (void)Icon_HandleKey(pme, AVK_LEFT);
               //               bRet = TRUE;
               //            }else if( wXPos >= ((rctText.x + rctText.dx) - (CXARROW + 1)) ){
               //               (void)Icon_HandleKey(pme, AVK_RIGHT);
               //               bRet = TRUE;
               //            }
               //         }
               //      }
               //      // VScrollbar needs no extra action, Timer is killed below
               //   }
               //}
               break;
            }
            pme->m_ptTracker.ptPosition.x = pme->m_ptTracker.ptPosition.y = 0;
         }
         if( pme->m_cls == AEECLSID_MENUCTL && (pme->m_ptTracker.cbHit & PTRCK_HIT_SCRL) ){
            bRet = TRUE;
         }
         if( pme->m_ptTracker.cbFlags & PTRCK_GEN_TMRSET ){
            ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)Menu_ScrollTimerCB, (void *)pme);
         }
         MEMSET((void *)&pme->m_ptTracker, 0, sizeof(pme->m_ptTracker));
         return bRet;
      }
   }

   return FALSE;
}
/*=====================================================================

Local Method - Recalculates the size information in the menu.

======================================================================*/
static void Menu_Recalc(CMenuCtl * pme)
{
   CMenuItem *    pi;
   int            cy,cx,cyMax,cxMax,cys,cxs,nFrame,cxMaxItem,cyMaxItem;
   int nFont;
   cxMax = cyMax = 0;

   pme->m_bHasText = FALSE;

   // Get extra pixels added to height and width because of style
   Menu_GetStyleOffsets(pme, &cxs, &cys);

   nFrame = (uint16)(AEE_FRAME_SIZE * 2);
   cxMaxItem = pme->m_rc.dx - cxs - nFrame;     // Max item width
   cyMaxItem = pme->m_rc.dy - cys - nFrame;     // Max item height

   if(SINGLE_FRAME(pme)){
      cxMax = cxMaxItem;                        // Width of widest item - Max item width
      cyMax = cyMaxItem;                        // Height of tallest item - Max item height
   }

   // Calculate width of widest item and height of tallest item
   for(pi = pme->m_pItemList; pi; pi = pi->pNext){
      // Measure Item Text Width
      if (!pi->cxText && *pi->text)
         pi->cxText = IDISPLAY_MeasureText(pme->m_pIDisplay, pi->nFont, pi->text);

      // Icon Views only measure the image. The text is shown as a title or below the menu.
      if(IS_ICON_VIEW(pme)){
         cx = pi->cxImage;
         cy = pi->cyImage;

         if (IS_ITEM_OD(pi))
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
         cx = (IS_ITEM_OD(pi) ? pi->cxImage : GetItemWidth(pi));

         // Use the MenuItem font instead of default MenuCtl font
         nFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, pi->nFont, NULL, NULL);	 
         // use pme->m_cyFont if EFAILED is returned by IDISPLAY_GetFontMetrics
         if (nFont == EFAILED) {
            nFont = pme->m_cyFont;
         }
         // Bigger of font/image height
         cy = (int)(pi->text[0] ? nFont : 0);
         if(pi->cyImage > cy)
            cy = pi->cyImage;

         if (IS_ITEM_OD(pi))
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

      if(pi->cxText || pi->cyText)
         pme->m_bHasText = TRUE;
   }

#if 1
 if( pme->m_dwOemProps & OEMMP_ICON_FIX_ITEMMAX )
 {
        cxMax =MIN(cxMax, ICONMENU_FIX_ITEMMAX);
        cyMax =MIN(cyMax, ICONMENU_FIX_ITEMMAX);        
 }
 #endif

   cxMax += cxs;              // Add Width Offset
   cyMax += cys;
   
#ifdef FEATURE_CUSTOMIZED_MENU_STYLE
   /*  这里取两者之中较大值为cyMax赋值以消除当设置的
    MENUITEM_HEIGHT宏数值与字体高度差值为奇数时坐标错*/
   cyMax = MAX(cyMax, MENUITEM_HEIGHT);
#endif
	

   pme->m_cxMaxItem = cxMax;  // Width of widest item
   pme->m_cyMaxItem = cyMax;  // Height of tallest item

   switch(pme->m_cls){
      case AEECLSID_ICONVIEWCTL:
         // Refresh screen layout for icon view
         Icon_CalcRows(pme);
         break;

      case AEECLSID_SOFTKEYCTL:
         {
            int   nAdd;
  
            // If SK control height isn't same as the height of tallest item, adjust it
            if(pme->m_rc.dy != cyMax){
               nAdd = pme->m_rc.dy - cyMax;
               pme->m_rc.dy = cyMax;
               pme->m_rc.y += nAdd;
            }

            // If max SK items are to be displayed, calculate page items using width of
            // the widest item
            if(MAX_SK_ITEMS(pme)){
               if(cxMax)
                  pme->m_nPageItems = pme->m_rc.dx / cxMax;
               else
                  pme->m_nPageItems = __MIN(pme->m_nItems, MAX_SOFT_PAGE_ITEMS);
            }   
         }
         break;

      default:
         // Calculate items on a page full of the control
         if(cyMax){
            cy = pme->m_rc.dy;
            if(pme->m_pTitle || IS_TITLE_OD(pme))
               cy -= pme->m_rcTitle.dy;
            
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
            if( (!pme->userSetStyle) && (pme->bottomBarType != BTBAR_NONE))
            {
                cy -= GetBottomBarHeight( pme->m_pIDisplay);
            }
#endif
            pme->m_nPageItems = (cy / cyMax);
         }
         else
            pme->m_nPageItems = 0;

         // Regular Menu - Reset Auto Scroll
         Menu_ResetAutoScroll(pme);
   }
}
/*=====================================================================

Local Method - Draws only new selected item in a menu.

======================================================================*/
static void Menu_CalcItemRect(CMenuCtl * pme, int nIdx, AEERect* prc)
{
   int         x = pme->m_rc.x;
   int         y = pme->m_rc.y;
   uint16      cyLeft;
   uint16      cy,cx;

   cyLeft = pme->m_rc.dy;

   if(pme->m_pTitle || IS_TITLE_OD(pme)){
      cyLeft -= pme->m_rcTitle.dy;
      y += pme->m_rcTitle.dy;
   }
   
   cx = pme->m_rc.dx;
   cy = pme->m_cyMaxItem;

   if (pme->m_nItems > pme->m_nPageItems) 
      cx -= pme->m_nSBWidth;

   SETAEERECT(prc, x, y + ((nIdx - pme->m_nScrollItem) * cy), cx, cy);
}

/*=====================================================================

Local Method - Draws only new selected item in a menu.

======================================================================*/
static void Menu_DrawNewSelectItem(CMenuCtl * pme, int nIdx,int nIdxSel)
{
   IDisplay *  pd = pme->m_pIDisplay;
   AEERect     qrc;
   RGBVAL      clrBack, clrText, clrFrame;
   CMenuItem * pItem = NULL;

   // Nothing to do...
   if(!pme->m_bActive)
      return;

   // Initialize CLR_USER_BACKGROUND, CLR_USER_TEXT and CLR_USER_FRAME and
   // save current background, text and frame colors for restoration
   Menu_InitColors(pme, &clrBack, &clrText, &clrFrame);

   pItem = GetItemByIdx(pme->m_pItemList, nIdx);

   if(IS_CALENDAR_VIEW(pme))
      Cal_DrawItem(pme, pItem, nIdx, TRUE);
   else
   {
      Menu_CalcItemRect(pme, nIdx, &qrc);

// add these @08.01.16
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      pme->theDrawnItem = ( nIdx + 1);
#endif//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      
      if (IS_ITEM_OD(pItem))
         Menu_DrawODItem(pme, pItem, &qrc, FALSE);
      else
         Menu_DrawItem(pme, pItem, &qrc, FALSE, 0);
   }

   // Draw the selected item...
   
   pItem = GetItemByIdx(pme->m_pItemList, nIdxSel);

   pme->m_bAutoScroll = Menu_ItemScrolls(pme, pItem, 0);

   if(IS_CALENDAR_VIEW(pme))
      Cal_DrawItem(pme, pItem, nIdxSel, TRUE);
   else
   {
      Menu_CalcItemRect(pme, nIdxSel, &qrc);

//      pme->m_seltextrc = qrc;
//      pme->m_seltextrc.x = (pItem->cxImage+4);
//      pme->m_seltextrc.dx -= (pItem->cxImage+4);
      
// add these @08.01.16
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      //pme->m_nItems <= 10		
      if( !pme->userSetStyle && 
          ( (IS_PROP_SET( pme->m_dwProps, MP_BIND_ITEM_TO_NUMBER_KEY) && pme->m_nItems <= MENU_ITEM_MAX) ||
            IS_MULTI_SEL( pme)
          )
      )
      {
//          pme->m_seltextrc.x  += 13;
//          pme->m_seltextrc.dx -= 13;
            
          pme->theDrawnItem = ( nIdxSel + 1);
      }
#endif//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      
      if (IS_ITEM_OD(pItem))
         Menu_DrawODItem(pme, pItem, &qrc, TRUE);
      else
         Menu_DrawItem(pme, pItem, &qrc, TRUE, 0);
   }
 
   // Reset the colors.  If this is not done here, it will screw-up the drawing of the scrollbar...
   ResetColors(pd, clrBack, clrText,clrFrame);

   // Draw Scroll Bar
   Menu_DrawScrollBar(pme);

   IDISPLAY_Update(pd);
}


/*=====================================================================

Local Method - Sets the text and background color for drawing a selected/normal 
menu item. Returns previous background color in pcBack.

======================================================================*/
static void Menu_SetItemColor(CMenuCtl * pme, IDisplay * pd, boolean bSel,RGBVAL * pcBack)
{
   RGBVAL  nText, nBack;

#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   if( bSel && !( IS_SOFTKEY_MENU( pme) && pme->m_nItems <= 1)){
#else
   if(bSel){
#endif//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      nText = pme->m_c.cSelText;
      nBack = pme->m_c.cSelBack;
   }
   else{
      nText = pme->m_c.cText;
      nBack = pme->m_c.cBack;
   }
	
   nBack = IDISPLAY_SetColor(pd, CLR_USER_BACKGROUND, nBack);
   
	//#ifdef FEATURE_VERSION_C337
	//IDISPLAY_SetColor(pd, CLR_USER_TEXT, RGB_WHITE);
	//#else
	IDISPLAY_SetColor(pd, CLR_USER_TEXT, nText);
	//#endif   

   if(pcBack)
      *pcBack = nBack;
}


/*=====================================================================

Local Method - Frees the memory for a menu item

======================================================================*/
static void Menu_FreeItem(CMenuCtl * pme, CMenuItem * pmi)
{
   aee_releaseobj((void **)&pmi->pImage);
   FREE(pmi);
}

/*=====================================================================

Local Method - Draws a standard menu

======================================================================*/
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

static void Menu_DrawSelectBar( CMenuCtl *pme, AEERect *prc, AEEFrameType ft, uint8 BarHeight)
{
    AEERect oldClip;
#ifdef FEATURE_USES_MMS 
    AEEImageInfo         imageInfo   = {0}; 
#endif
    if (pme->m_dwOemProps & OEMMP_NODRAWSELECTBAR)
    {
        return;
    }
    
    if( pme->imageSelectBar == NULL)
    {
        if(pme->SetPopMenu == TRUE)
        {
            /*if( pme->imageSelectBar != NULL)
            {
                IIMAGE_Release(pme->imageSelectBar);
            }*/

            pme->imageSelectBar = ISHELL_LoadResImage( pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_SELECT_BAR);
        }
        else if(IS_PROP_SET( pme->m_dwProps, MP_TRANSPARENT_UNSEL))
        {
            /*if( pme->imageSelectBar != NULL)
            {
                IIMAGE_Release(pme->imageSelectBar);
            }*/

            pme->imageSelectBar = ISHELL_LoadResImage( pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_SELECT_BAR_SHORT);
        }
        else
        {
            /*if( pme->imageSelectBar != NULL)
            {
                IIMAGE_Release(pme->imageSelectBar);
            }*/

            pme->imageSelectBar = ISHELL_LoadResImage( pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_SELECT_BAR);
        }
    }
    
    if( pme->imageSelectBar)
    {
        IDISPLAY_GetClipRect( pme->m_pIDisplay, &oldClip);

        IDISPLAY_SetClipRect( pme->m_pIDisplay, prc);
    
#if !defined( FEATURE_CONTROL_BG_USE_IMAGE)
        if( IS_MENU( pme)&& (!IS_PROP_SET( pme->m_dwProps, MP_TRANSPARENT_UNSEL)))
        {
           IDISPLAY_DrawFrame( pme->m_pIDisplay, prc, ft, pme->m_c.cBack);
        }

#endif //#if !defined( FEATURE_CONTROL_BG_USE_IMAGE)

#ifdef FEATURE_USES_MMS  
        if(pme->m_cyFont < BarHeight)
        {
            IIMAGE_GetInfo( pme->imageSelectBar, &imageInfo);
            IIMAGE_SetParm(pme->imageSelectBar,IPARM_SCALE, imageInfo.cx, BarHeight);
        }
#endif  

        IIMAGE_Draw( pme->imageSelectBar, prc->x, prc->y);
        IIMAGE_Release(pme->imageSelectBar);
        pme->imageSelectBar = NULL;
    
        IDISPLAY_SetClipRect( pme->m_pIDisplay, &oldClip);
    }
}

#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

static boolean Menu_Draw(CMenuCtl * pme)
{
   int         i;
   AEERect     qrc;
   uint16      cyLeft;
   uint16      cyMenuItem,cxMenuItem;
   RGBVAL      clrBack, clrText,clrFrame;
   boolean     bSel;
   int         cyFont = pme->m_cyFont;
   int         x = pme->m_rc.x;
   int         y = pme->m_rc.y;
   CMenuItem * p = NULL;
   IDisplay *  pd = pme->m_pIDisplay;
#ifdef FEATURE_RANDOM_MENU_COLOR
   byte nOldRandMenu = pme->m_nRandomMenu;
#endif
#ifdef FEATURE_RANDOM_MENU_COLOR
   if(!IS_PROP_SET( pme->m_dwProps, MP_TRANSPARENT_UNSEL)  &&
       pme->m_dwOemProps & OEMMP_GRAPHIC_BG)
   {
       (void)OEM_GetConfig(CFGI_RANDOM_MENU, (void*)&pme->m_nRandomMenu, sizeof(pme->m_nRandomMenu));
       if(pme->m_nRandomMenu == 0)
       {
            pme->m_c.cBack = RGB_BLACK;
            if(nOldRandMenu != 0  &&
                pme->m_pBgImage != NULL)
            {
                IImage_Release(pme->m_pBgImage);
                pme->m_pBgImage = NULL;
            }
       }
       else
       {
            OEM_GetConfig(CFGI_MENU_BGCOLOR, (void*)&pme->m_c.cBack, sizeof(pme->m_c.cBack));
            if(nOldRandMenu == 0 &&
                pme->m_pBgImage != NULL)
            {
                IImage_Release(pme->m_pBgImage);
                pme->m_pBgImage = NULL;
            }
       }
   }
#endif

   Menu_InitColors(pme, &clrBack, &clrText, &clrFrame);

#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    
#if defined( FEATURE_CONTROL_BG_USE_IMAGE)
    if( pme->userSetStyle)
    {
        IDISPLAY_FillRect( pd, &pme->m_rc, CLR_USER_BACKGROUND);
    }
    else
    {
        drawControlBg( pd, &pme->m_rc);
    }
#else
    if(!IS_PROP_SET( pme->m_dwProps, MP_TRANSPARENT_UNSEL) )
    {
        if(pme->m_dwOemProps & OEMMP_GRAPHIC_BG)
        {
            Menu_DrawBackGround(pme, &pme->m_rc);
        }
        else
        {
            IDISPLAY_FillRect( pd, &pme->m_rc, CLR_USER_BACKGROUND);  
        }
    }
    else
    {
        IDISPLAY_FillRect( pd, &pme->m_rc, MAKE_RGB(102, 102, 102));  
    }
#endif //#if defined( FEATURE_CONTROL_BG_USE_IMAGE) 
    
    if( pme->userSetStyle)
    {
        pme->m_rcTitle.dy = GetTitleBarHeight( pme->m_pIDisplay);
    }

#else //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

    // Erase Previous Menu - Use default system color
    if(!IS_PROP_SET( pme->m_dwProps, MP_TRANSPARENT_UNSEL) )
    {
         IDISPLAY_FillRect( pd, &pme->m_rc, CLR_USER_BACKGROUND);
    }
    else
    {       
         IDISPLAY_FillRect( pd, &pme->m_rc, MAKE_RGB(102, 102, 102));     
    }  

#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
              

   // draw the title text
   cyLeft = pme->m_rc.dy;

   if(pme->m_pTitle || IS_TITLE_OD(pme))
   {

      SETAEERECT(&qrc, x, y, pme->m_rc.dx, pme->m_rcTitle.dy);
        
      if (pme->m_pTitle)
      {
#ifdef FEATURE_MENUTITLE_AUTOSCROLL
         Menu_DrawTitle(pme,0);
#else //FEATURE_MENUTITLE_AUTOSCROLL

#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
        if( pme->userSetStyle)
        {
            IDISPLAY_DrawText( pd, 
                  AEE_FONT_BOLD, 
                  pme->m_pTitle, 
                  -1, 
                  x, 
                  y, 
                  &qrc, 
                  IDF_ALIGN_MIDDLE | IDF_RECT_FILL | ParagraphAlignment(pme->m_pTitle, WSTRLEN(pme->m_pTitle))

              );
        }
        else
        {
            TitleBar_Param_type titleBarParms;
            MEMSET( &titleBarParms, 0, sizeof( TitleBar_Param_type));
            
            titleBarParms.dwAlignFlags  = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;

            titleBarParms.pwszTitle     = pme->m_pTitle;
            titleBarParms.prc           = &qrc;
            if(pme->m_dwOemProps & OEMMP_ARROW_TITLE)
            {
                titleBarParms.eTBarType = TBAR_ARROW;
            }
            
            DrawTitleBar( pd, &titleBarParms);
        }
#else
        IDISPLAY_DrawText(pd, AEE_FONT_BOLD, pme->m_pTitle, -1, x, y, &qrc, IDF_RECT_FILL | ParagraphAlignment(pme->m_pTitle, WSTRLEN(pme->m_pTitle)));
#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
#endif
      }
      else
      {
         // Title is owner draw

         AEEODCBData cbData;
         MEMSET(&cbData, 0, sizeof(AEEODCBData));

         cbData.nSize = sizeof(AEEODCBData);
         cbData.nAction = ODA_DRAW_TITLE;
         cbData.pIDisplay = pd;
         cbData.rc = qrc;

         // Draw Title
         pme->m_pfnCB(pme->m_pvContext, &cbData);
      }
      
      
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
        if(pme->bottomBarType == BTBAR_NONE)
        {
            cyLeft -= pme->m_rcTitle.dy;
            y      += pme->m_rcTitle.dy;

            if(pme->userSetStyle && UNDERLINE( pme)) 
            {
                qrc.y = y - 1;
                qrc.dy = 1;
                IDISPLAY_FillRect(pme->m_pIDisplay, &qrc, CLR_USER_FRAME);
            }
        }
        else
        {
/*
            cyLeft -= GetBottomBarHeight( pme->m_pIDisplay);
            y      += GetBottomBarHeight( pme->m_pIDisplay);
*/
            cyLeft -= (pme->m_rcTitle.dy + GetBottomBarHeight( pme->m_pIDisplay));
            y       += pme->m_rcTitle.dy;
        }
#else //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
        cyLeft -= pme->m_rcTitle.dy;
        y    += pme->m_rcTitle.dy;

        if(UNDERLINE(pme))
        {
            qrc.y = y - 1;
            qrc.dy = 1;
            IDISPLAY_FillRect(pme->m_pIDisplay, &qrc, CLR_USER_FRAME);
        }
#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   }
   
   pme->m_yLastTimeMark = INVALID_MARK;
   if(IS_CALENDAR_VIEW(pme))
   {
       Cal_CalcPageItems(pme);
   }

   pme->m_nVisibleItems = 0;
   cxMenuItem           = pme->m_rc.dx;
   cyMenuItem           = pme->m_cyMaxItem;

   if (pme->m_nItems > pme->m_nPageItems)
       cxMenuItem -= pme->m_nSBWidth;      

   p = GetItemByIdx(pme->m_pItemList, pme->m_nScrollItem);
        
   for (i = pme->m_nScrollItem; p && i < pme->m_nScrollItem + pme->m_nPageItems; i++, p = p->pNext) 
   {
      SETAEERECT(&qrc, x, y, cxMenuItem, cyMenuItem);
        
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
		 //pme->m_nItems <= 10

		 //Modify by zzg 2011_03_02	 	
         if( !pme->userSetStyle && 
             IS_PROP_SET( pme->m_dwProps, MP_BIND_ITEM_TO_NUMBER_KEY) &&
             pme->m_nItems <= MENU_ITEM_MAX
         )
         
         {
             pme->theDrawnItem   = ( i + 1);			 
         }

#endif//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE) 
         
      bSel = (boolean)(pme->m_bActive && i == pme->m_nSelect);

	  if (IS_CALENDAR_VIEW(pme))
	  {
         p->cyImage  = cyMenuItem = GetItemHeight(p, &cyLeft, (uint16)cyFont);
         p->y        = y;
         
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
         Cal_DrawItem(pme, p, i, FALSE);
#else
         Menu_SetItemColor(pme,pd,bSel,NULL);
         Cal_DrawItem(pme, p, i, FALSE);
#endif
      }
      else
      {  
         if (IS_ITEM_OD(p))
         {
         	Menu_DrawODItem(pme, p, &qrc, bSel);
         }
         else
         {
         	Menu_DrawItem(pme, p, &qrc, bSel, 0);
         }
      }
      y += cyMenuItem;
      pme->m_nVisibleItems++;
   }

#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    if( !pme->userSetStyle && pme->bottomBarType != BTBAR_NONE)
    {
        AEERect rect;
        BottomBar_Param_type bottomBarParms;
        MEMSET( &bottomBarParms, 0, sizeof( BottomBar_Param_type));
        
        SETAEERECT( &rect, 
              pme->m_rc.x, 
              pme->m_rc.y + pme->m_rc.dy - GetBottomBarHeight( pme->m_pIDisplay),
              pme->m_rc.dx,
              GetBottomBarHeight( pme->m_pIDisplay));
        if(pme->SetPopMenu == FALSE)
        {
            bottomBarParms.prc         = &rect;
        }
        bottomBarParms.eBBarType   = pme->bottomBarType;
        
        DrawBottomBar( pd, &bottomBarParms);
    }
#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   
   // Reset the colors.  If this is not done here, it will screw-up the drawing of the scrollbar...
   ResetColors(pd, clrBack, clrText,clrFrame);

   // Draw Scrollbar
   Menu_DrawScrollBar(pme);

   if(IS_CALENDAR_VIEW(pme) && AUTO_SCROLL(pme))
      Cal_ShowTimeMark(pme, NULL,0, 0);
      
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    if(pme->SetPopMenu == TRUE)
    {
        //AEERect titlerect;
        AEERect framerect;
        Theme_Param_type Theme_Param;
        
        Appscom_GetThemeParameters(&Theme_Param);
#if 0
        SETAEERECT( &titlerect,  
            pme->m_rc.x,  
            pme->m_rc.y - GetTitleBarHeight( pme->m_pIDisplay), 
            pme->m_rc.dx, 
            GetTitleBarHeight( pme->m_pIDisplay));
        IDISPLAY_FillRect(pme->m_pIDisplay, &titlerect, Theme_Param.themeColor);  

        SETAEERECT( &framerect,  
            pme->m_rc.x-AEE_FRAME_SIZE,  
            pme->m_rc.y - GetTitleBarHeight( pme->m_pIDisplay), 
            pme->m_rc.dx +2*AEE_FRAME_SIZE, 
            pme->m_rc.dy + GetTitleBarHeight( pme->m_pIDisplay));
        
        IDISPLAY_DrawRect(pme->m_pIDisplay,
                                        &framerect,
                                        Theme_Param.themeColor, 
                                        0, 
                                        IDF_RECT_FRAME);            
#else //if 0

        //滚动条和边框不能并存，否则在需要滚动条的时候，边框也出现了
        //看起来像有个双边框。只有当不需要滚动条时，才来边框
        if(pme->m_menuReallyDy <= (SCREEN_HEIGHT - GetBottomBarHeight(pme->m_pIDisplay)))//add by xuhui
        {
            SETAEERECT( &framerect,  
                pme->m_rc.x-AEE_FRAME_SIZE,  
                pme->m_rc.y- AEE_FRAME_SIZE*2,
                pme->m_rc.dx +2*AEE_FRAME_SIZE, 
                AEE_FRAME_SIZE*2);
                
            IDisplay_FillRect(pme->m_pIDisplay, &framerect, RGB_WHITE);

            SETAEERECT( &framerect,  
                pme->m_rc.dx +AEE_FRAME_SIZE,  
                pme->m_rc.y- AEE_FRAME_SIZE*2,
                2, 
                pme->m_rc.dy + 2 - GetBottomBarHeight(pme->m_pIDisplay));
                
            IDisplay_FillRect(pme->m_pIDisplay, &framerect, RGB_WHITE);
        }
#endif //if 0
    }
#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

    if(pme->m_bAutoScrollMenuItem)
    {
        pme->m_bAutoScrollMenuItem = FALSE;
        return TRUE;   //popMenu which has autoscroll menuitem on preview window should not update display tlc.lb.20081219
    }

    IDISPLAY_Update(pd);

   return(TRUE);
}

/*=====================================================================

Local Method - Draws a standard menu item

======================================================================*/
static void Menu_DrawItem(CMenuCtl * pme, CMenuItem * p, AEERect * prc, boolean bSel, int nTruncExtent)
{
   int            x,y,cx,cxText,yImage,nFont;
   int                  buf = 0;

   RGBVAL               cBack;
   boolean              b3State;
   boolean              bMulti;
   IDisplay*            pd = pme->m_pIDisplay;
   AECHAR*              pText = p->text;
   AEEItemStyle*        ps = &pme->m_style[(bSel ? 1 : 0)];
   boolean              bTitleRightAligned = FALSE;
   AEERect              rcText;
   uint32               dwItemTextAlignment;
   AECHAR               wszIndex[5];	//7
   AECHAR               wszFmt[4];		//6//
   //Theme_Param_type     themeParms;
   AEERect              rect; 
   //IImage*              checkBox; 
   AEEImageInfo         imageInfo   = {0}; 
   //IImage*              underline;
   int                  menuwidth, xMenu;

   
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    AEEDeviceInfo DeviceInfo;
    
    ISHELL_GetDeviceInfo(pme->m_pIShell, &DeviceInfo);
	
    if((LNG_HEBREW == DeviceInfo.dwLang) || (LNG_ARABIC == DeviceInfo.dwLang))
    {
        bTitleRightAligned = TRUE;
    }
    else
    {
        bTitleRightAligned = FALSE;
    }
#else

   // icons are drawn according to the alignment of the menu title ...
   if (pme->m_pTitle) 
   {
      bTitleRightAligned = (ParagraphAlignment(pme->m_pTitle, WSTRLEN(pme->m_pTitle))) == IDF_ALIGN_RIGHT;
   }
#endif   

   menuwidth = prc->dx;
   xMenu     = prc->x;
   b3State = (boolean)(IS_MULTI_IMAGE(pme) ? TRUE : FALSE);
   bMulti  = (boolean)(IS_MULTI_SEL(pme) ? TRUE : FALSE);

   // If item is selected and m_bAutoScroll is set, update position to start drawing
   // text from
   if (bSel && pme->m_bAutoScroll)
   {
      MEMCPY(&pme->m_rcAutoScroll, prc, sizeof(AEERect));
   }

   // Draw the frame and fill with the sel/unsel color.  Note that if this
   // is a tabbed-dialog, the "masked" pixels will draw with the CLR_USER_BACKGROUND
   // color.  This means we must preserve it below...

   if (!b3State)
   {
      AEEFrameType   ft;
      // If it is not multi, go with regular behavior
      // Otherwise, if it has focus but is not sel draw a box
      // OR draw the frame type given as select/not select
      if (!bMulti)
      {  
      	ft = ps->ft;
      }
      else
	  {
         //if( bSel && !p->bIsSel )
           // ft = AEE_FT_BOX;
         //else
            ft = (AEEFrameType)(p->bIsSel ? pme->m_style[1].ft : pme->m_style[0].ft);
      }
      

#if !defined( FEATURE_CUSTOMIZED_MENU_STYLE)
       if(!IS_PROP_SET( pme->m_dwProps, MP_TRANSPARENT_UNSEL))
       {
           IDISPLAY_DrawFrame(pd, prc, ft, (bSel ? pme->m_c.cSelBack : pme->m_c.cBack));
       }
#else //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

    if(IS_PROP_SET( pme->m_dwProps, MP_TRANSPARENT_UNSEL))
    {
        prc->dx+=2;
    }
    

    if( !pme->userSetStyle && ( IS_MENU( pme) || ( IS_SOFTKEY_MENU( pme) && pme->m_nItems > 1)) && bSel)
    {
        Menu_DrawSelectBar( pme, prc, ft, p->cyImage);
    }
#if !defined( FEATURE_CONTROL_BG_USE_IMAGE)
    else if( pme->userSetStyle || IS_MENU( pme))
    {
       if(pme->m_dwOemProps & OEMMP_GRAPHIC_BG)
       {
           Menu_DrawBackGround(pme, prc);
       }
       else if(!IS_PROP_SET( pme->m_dwProps, MP_TRANSPARENT_UNSEL))
        {
#ifdef CUST_EDITION
//只是为了解决背景和前景相同颜色的一个bug，这个bug由于应用导致
		if (pme->m_c.cSelBack == pme->m_c.cBack||((pme->m_c.cSelBack<CLR_SYS_LAST)&&(pme->m_c.cBack<CLR_SYS_LAST)))
		{
			pme->m_c.cSelBack=MAKE_RGB(0xff, 0x00, 0x00);
	    }
              IDISPLAY_DrawFrame(pd, prc, ft, (bSel ? pme->m_c.cSelBack : pme->m_c.cBack));
#else
              IDISPLAY_DrawFrame(pd, prc, ft, (bSel ? pme->m_c.cSelBack : pme->m_c.cBack));
#endif
        }
    }
#endif

    // add these @08.01.15
    
	//pme->m_nItems <= 10	
	//Modify by zzg 2011_03_02
    if( !pme->userSetStyle && 
        ((IS_PROP_SET( pme->m_dwProps, MP_BIND_ITEM_TO_NUMBER_KEY) && pme->m_nItems <= MENU_ITEM_MAX) || bMulti)
    )
    {
    if( bMulti)
    {
       IImage *pImage = NULL;
       //AEERect rect = {0};

#if defined( FEATURE_CONTROL_BG_USE_IMAGE)
       static IImage* sel1	= 0;
       static IImage* sel2	= 0;
       void *pOldContext = AEE_EnterAppContext( NULL);

          if( !sel1)
          {
             sel1 = ISHELL_LoadResImage( pme->m_pIShell, 
                                          AEE_APPSCOMMONRES_IMAGESFILE, 
                                          IDI_MENU_SEL1);
          }
          if( !sel2)
          {
             sel2 = ISHELL_LoadResImage( pme->m_pIShell, 
                                          AEE_APPSCOMMONRES_IMAGESFILE, 
                                          IDI_MENU_SEL2);
          }
          AEE_LeaveAppContext( pOldContext);
       }

       pImage = p->bIsSel ? sel2 : sel1;
       if( pImage)
       {
          IIMAGE_Draw( pImage, prc->x + prc->dx - 13, prc->y + ( ( prc->dy - 13) >> 1));
          IIMAGE_Release( pImage);
       }
#else
     /*  SETAEERECT( &rect,  prc->x + prc->dx - 12, prc->y + ( ( prc->dy - 12) >> 1), 12, 12);
       IDISPLAY_DrawRect( pd, &rect, RGB_WHITE, 0, IDF_RECT_FRAME);
       */

          pImage = ISHELL_LoadResImage( pme->m_pIShell,
                AEE_APPSCOMMONRES_IMAGESFILE,
                (uint16)(p->bIsSel ? IDB_CHECK_ON : IDB_CHECK_OFF));

          if( pImage)
          {
             IIMAGE_GetInfo( pImage, &imageInfo);
             IIMAGE_Draw( pImage, 
                   menuwidth - MENU_IMAGE_PAD - imageInfo.cx, 
                   prc->y + (prc->dy - imageInfo.cy)/2);
             IIMAGE_Release( pImage);
          }
#endif

        prc->dx -= 13;
    }
    else
    {
        /*  这个if用来检测MP_BIND_ITEM_TO_NUMBER_KEY属性是否被设置，有，在菜单前面画一个内嵌序号的正方形 */
        
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
        if(bTitleRightAligned){
            SETAEERECT( &rect, prc->x+prc->dx-pme->m_cyFont,prc->y + ps->yOffset, pme->m_cyFont+1, prc->dy);
		
			STRTOWSTR(".%d", wszFmt, sizeof(wszFmt));
            
            WSPRINTF(wszIndex,sizeof(wszIndex),wszFmt,pme->theDrawnItem);
    
    #if !defined( FEATURE_CONTROL_BG_USE_IMAGE)
            if(bSel){
                IDISPLAY_SetColor(pd,CLR_USER_BACKGROUND,RGB_WHITE);
                IDISPLAY_SetColor(pd,CLR_USER_TEXT,RGB_BLACK);//wlh mod 更改选中条的序号颜色
            }
            else{
                IDISPLAY_SetColor(pd,CLR_USER_BACKGROUND,RGB_BLACK);
                IDISPLAY_SetColor(pd,CLR_USER_TEXT,RGB_WHITE);
            }
    #endif
            IDISPLAY_DrawText(pd, p->nFont, wszIndex, -1, rect.x, rect.y, &rect, IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
            
            prc->dx -= rect.dx;
        }
        else{
            SETAEERECT( &rect, ps->xOffset/*prc->x*/,prc->y + ps->yOffset, pme->m_cyFont+1, prc->dy);
			
            STRTOWSTR("%d.", wszFmt, sizeof(wszFmt));	
			            
            WSPRINTF(wszIndex,sizeof(wszIndex),wszFmt,pme->theDrawnItem);
    
    #if !defined( FEATURE_CONTROL_BG_USE_IMAGE)
            if(bSel){
                IDISPLAY_SetColor(pd,CLR_USER_BACKGROUND,RGB_WHITE);
                IDISPLAY_SetColor(pd,CLR_USER_TEXT,RGB_BLACK);//wlh mod 更改选中条的序号颜色
            }
            else{
                IDISPLAY_SetColor(pd,CLR_USER_BACKGROUND,RGB_BLACK);
                IDISPLAY_SetColor(pd,CLR_USER_TEXT,RGB_WHITE);
            }
    #endif
            IDISPLAY_DrawText(pd, p->nFont, wszIndex, -1, rect.x, rect.y, &rect, IDF_TEXT_TRANSPARENT);
            
            prc->x  += rect.dx;
            prc->dx -= rect.dx;
        }
#else
        //下面的20 和12可能要改成公式计算

		#if defined( FEATURE_VERSION_C337)||defined( FEATURE_VERSION_K202)	
		SETAEERECT( &rect, ps->xOffset/*prc->x*/,prc->y + ps->yOffset, 3*pme->m_cyFont/2, prc->dy);
		#else
		SETAEERECT( &rect, ps->xOffset/*prc->x*/,prc->y + ps->yOffset, pme->m_cyFont, prc->dy);
		#endif

        STRTOWSTR("%d.", wszFmt, sizeof(wszFmt));				
		
        WSPRINTF(wszIndex,sizeof(wszIndex),wszFmt,pme->theDrawnItem);
		
#ifdef FEATURE_VERSION_C337		
		{
			IImage      *pBarImg = NULL;

            if (IS_PROP_SET( pme->m_dwProps, MP_ICON_ANIMATED))
            {
            
#if defined (FEATURE_DISP_176X220) || defined (FEATURE_DISP_128X160)
                switch(pme->theDrawnItem)
                {
                    case 1:
                    {
                        pBarImg = ISHELL_LoadResImage(pme->m_pIShell,
    		                                      AEE_APPSCOMMONRES_IMAGESFILE,
    		                                      IDI_MENU_ITEM_PROFILE_ALERT);    
                        break;
                    }
                    case 2:
                    {
                        pBarImg = ISHELL_LoadResImage(pme->m_pIShell,
    		                                      AEE_APPSCOMMONRES_IMAGESFILE,
    		                                      IDI_MENU_ITEM_PROFILE_SILENCE);    
                        break;
                    }
                    case 3:
                    {
                        pBarImg = ISHELL_LoadResImage(pme->m_pIShell,
    		                                      AEE_APPSCOMMONRES_IMAGESFILE,
    		                                      IDI_MENU_ITEM_PROFILE_MEETING);
                        break;
                    }
                    case 4:
                    {
                        pBarImg = ISHELL_LoadResImage(pme->m_pIShell,
    		                                      AEE_APPSCOMMONRES_IMAGESFILE,
    		                                      IDI_MENU_ITEM_PROFILE_OUTDOOR);
                        break;
                    }
                    default:
                        break;
                }    
#endif                
            }
            else
            {
                if (bSel)
    			{
    				pBarImg = ISHELL_LoadResImage(pme->m_pIShell,
    		                                      AEE_APPSCOMMONRES_IMAGESFILE,
    		                                      IDI_MENU_ITEM_FOCUS);
    			}
    			else
    			{
    				pBarImg = ISHELL_LoadResImage(pme->m_pIShell,
    		                                      AEE_APPSCOMMONRES_IMAGESFILE,
    		                                      IDI_MENU_ITEM);
    			}		
            }

			if (NULL != pBarImg)
	        {	        	
	            IIMAGE_Draw(pBarImg, rect.x, rect.y);
	            IIMAGE_Release(pBarImg);
	            pBarImg = NULL;
	        }

			prc->x  += rect.dx;
        	prc->dx -= rect.dx;
		}		
#else		

#if !defined( FEATURE_CONTROL_BG_USE_IMAGE)
        if(bSel){			
            IDISPLAY_SetColor(pd,CLR_USER_BACKGROUND,RGB_WHITE);
#ifdef FEATURE_WHITE_BG
            IDISPLAY_SetColor(pd,CLR_USER_TEXT,RGB_WHITE);  //RGB_BLACK);//wlh mod 更改选中条的序号颜色
#else
            IDISPLAY_SetColor(pd,CLR_USER_TEXT,RGB_BLACK);//wlh mod 更改选中条的序号颜色
#endif            
        }
        else{			
            IDISPLAY_SetColor(pd,CLR_USER_BACKGROUND,RGB_BLACK);
#ifdef FEATURE_WHITE_BG
            IDISPLAY_SetColor(pd,CLR_USER_TEXT,RGB_BLACK);  //RGB_WHITE);
#else
            IDISPLAY_SetColor(pd,CLR_USER_TEXT,RGB_WHITE);
#endif            
        }
#endif
        IDISPLAY_DrawText(pd, p->nFont, wszIndex, -1, rect.x, rect.y, &rect, IDF_TEXT_TRANSPARENT);
        
        prc->x  += rect.dx;
        prc->dx -= rect.dx;
		#endif
#endif
    }
 }

#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   }

   // Now setup the background colors for the item.  This will insure the image
   // is drawn correctly during animation, etc.

   Menu_SetItemColor(pme, pd, bSel, &cBack);

   AdjustRect(prc,ps);
           
   x = prc->x;
   y = prc->y;
   cx = prc->dx;

   cxText = p->cxText;
    
   if(p->pImage){
      IIMAGE_SetParm(p->pImage, IPARM_ROP, (b3State ? AEE_RO_MASK : ps->roImage), 0);

      // If no text and its softkey control, center the image...

      if(!cxText && IS_SOFTKEY_MENU(pme))
         x += ((prc->dx - p->cxImage) / 2);

      // If text and a right aligned title, draw the image on the right
      //if (cxText && bTitleRightAligned) {
      if ((cxText && bTitleRightAligned) ||(IS_PROP_SET( pme->m_dwProps, MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT))) {
        buf = x;
         x += cx - p->cxImage;
      }

      yImage = y + ((prc->dy - p->cyImage) / 2);

      if(b3State)
         IIMAGE_DrawFrame(p->pImage, (p->bPressed ? 2 : (bSel ? 1 : 0)), x, yImage);
      else
         IIMAGE_Draw(p->pImage, x , yImage);

      if(IS_PROP_SET( pme->m_dwProps, MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT))
      {
             x = buf;
      }

      if(cxText){
         x += MENU_IMAGE_PAD + p->cxImage;
         if(IS_PROP_SET( pme->m_dwProps, MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT))
         {
             x -= p->cxImage;
         }
         cx -= p->cxImage + MENU_IMAGE_PAD;
      }
   }

    if(cxText){
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
        if( !pme->userSetStyle)
        {
            if( bSel && pme->m_nItems > 1) {
                p->nFont = AEE_FONT_BOLD;
            }
            else {
                p->nFont = AEE_FONT_NORMAL;
            }
        }
#endif
            
      // For softkey, we draw text in center of remaining space
        if(pme->m_cls == AEECLSID_SOFTKEYCTL && cxText < cx)
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
         {
            if(!IS_PROP_SET( pme->m_dwProps, MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT))
            {
               rcText.x += p->cxImage + MENU_IMAGE_PAD;
            }
         }
         rcText.dx -= p->cxImage + MENU_IMAGE_PAD;  
      }

      // determine the direction of this menu item's text
      dwItemTextAlignment = ParagraphAlignment(pText, WSTRLEN(pText));

      // determine start position of text based on auto scroll, clipping rectangle and alignment
      if (Menu_ItemScrolls(pme, p, 0)) {
         if (dwItemTextAlignment == IDF_ALIGN_LEFT) {
            // adjust the start position of the text only if it's selected
            if (bSel)
            x -= pme->m_nAutoScrollIdx;
         } else {
            // right aligned text, always adjust the start position.  But don't
            // factor in the auto scroll index if we're not selected.
            x -= IDISPLAY_MeasureText(pme->m_pIDisplay, p->nFont, pText) - rcText.dx - (bSel ? pme->m_nAutoScrollIdx : 0);
         }
      }       
        // draw the entire text.  Align it unless it's scrolling or a softkey.
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
	
    if(Menu_ItemScrolls(pme, p, 0) || (pme->m_cls == AEECLSID_SOFTKEYCTL))
    {
    	
         IDISPLAY_DrawText( pd, 
                 p->nFont, 
                 pText, 
                 -1, 
                 x, 
                 y, 
                 &rcText, 
                 pme->userSetStyle ? 0 : IDF_TEXT_TRANSPARENT
              );
    }
    else 
    {
        if( pme->m_dwOemProps & OEMMP_IDF_ALIGN_CENTER)
        {
            dwItemTextAlignment = IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT;
        }
        else if( !pme->userSetStyle)
        {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
           if(bTitleRightAligned) {
                dwItemTextAlignment = (IDF_ALIGN_RIGHT | IDF_TEXT_TRANSPARENT);
           }
           else {
                dwItemTextAlignment = (IDF_ALIGN_LEFT | IDF_TEXT_TRANSPARENT);
           }
#else
            dwItemTextAlignment = IDF_TEXT_TRANSPARENT;
#endif            
        }
		 
        IDISPLAY_DrawText(pd,
                p->nFont,
                pText,
                -1,
                x,
                y,
                &rcText,
                dwItemTextAlignment
            );
    }
    
#else
    if (Menu_ItemScrolls(pme, p, 0) || (pme->m_cls == AEECLSID_SOFTKEYCTL)) {
        IDISPLAY_DrawText(pd, p->nFont, pText, -1, x, y, &rcText, 0);
    }
    else {
        IDISPLAY_DrawText(pd, p->nFont, pText, -1, x, y, &rcText, dwItemTextAlignment);
    }

#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    }
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
#if defined(FEATURE_LANG_THAI)
    //draw line
    if(pme->m_dwOemProps & OEMMP_GRAPHIC_UNDERLINE)
    {
        IImage*              underline;
        
       	//Add By zzg 2010_07_25
        underline = ISHELL_LoadResImage(pme->m_pIShell, 
                                           AEE_APPSCOMMONRES_IMAGESFILE,
                                           IDI_UNDERLINE);

	   //Add End
		
        
        if(NULL != underline)
        {
            /* 由于AdjustRect中为文字显示多加了一个像素，这里补偿回来*/
            SETAEERECT(&rect, xMenu, prc->y + prc->dy + ps->yOffset-1, menuwidth, AEE_FRAME_SIZE);
            IIMAGE_GetInfo(underline, &imageInfo);
            
            Appscommon_ResetBackground(pd, underline, pme->m_c.cBack, &rect, rect.x, rect.y);
            IIMAGE_Release(underline);
        }
    }
#endif
#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

   // Reset the background color - it may have changed above...	
   IDISPLAY_SetColor(pd,CLR_USER_BACKGROUND,cBack);
}

/*=====================================================================

Local Method - Draws a standard menu owner drawn item

======================================================================*/
static void Menu_DrawODItem(CMenuCtl * pme, CMenuItem * p, AEERect * prc, boolean bSel)
{
   RGBVAL         cBack;
   AEEODCBData    cbData;
   IDisplay    *  pd = pme->m_pIDisplay;

   // Prepare display context with the right background color for the item
   Menu_SetItemColor(pme, pd, bSel, &cBack);

   // Initialize callback for owner draw item        
   MEMSET(&cbData, 0, sizeof(AEEODCBData));

   cbData.nSize = sizeof(AEEODCBData);
   cbData.nAction = ODA_DRAW_ITEM;
   cbData.pIDisplay = pd;
   cbData.rc = *prc;
   cbData.nActionInfo = p->nItemID;
   cbData.nActionInfo <<= 16;
   if (bSel)
      cbData.nActionInfo |= 0x00000001;

   // Draw Item 
   pme->m_pfnCB(pme->m_pvContext, &cbData);

   // Reset the background color   
   IDISPLAY_SetColor(pd,CLR_USER_BACKGROUND,cBack);
}

/*=====================================================================

Local Method - Auto-scroll logic for standard menu items.

Right to Left text support - jdboyd 05/05/03

This algorithm has been changed from returning an index into the
charcters to be drawn to an actual pixel offset.

Before, this routine would store the first character at which drawing was
to start in the m_nAutoScrollIdx member.

Now, this routine stores the pixel offset within a block of text at
which to start drawing while taking the clipping rect into effect.  Note,
when calculating this value, we don't care how the text is rendered, 
rather only how long it is.

======================================================================*/
static void Menu_AutoScroll(CMenuCtl * pme)
{
   CMenuItem * pi;
   int         n,nIdxNew,nIdx;
   AEERect     rc;
   RGBVAL      clrBack,clrText,clrFrame;

   if(!pme->m_bAutoScroll)
      return;
   
   // Find the selected item
   if ((pi = GetItemByIdx(pme->m_pItemList, pme->m_nSelect)) == NULL)
      return;

   // See if the index is within range for the string...

   nIdx  = nIdxNew = pme->m_nAutoScrollIdx;
   n     = IDISPLAY_MeasureText(pme->m_pIDisplay, pi->nFont, pi->text);

   if(nIdx < n){

      // Measure the text starting from the auto-scroll offset
      if(Menu_ItemScrolls(pme, pi, nIdx))
         nIdxNew += AUTO_SCROLL_PIXELS;
      else
         nIdxNew = 0;

      rc = pme->m_rcAutoScroll;  // Scroll Rect

      if(rc.dx){
         // Initialize CLR_USER_BACKGROUND, CLR_USER_TEXT and CLR_USER_FRAME and
         // save current background, text and frame colors for restoration
         Menu_InitColors(pme, &clrBack, &clrText, &clrFrame);

// add these @08.01.17
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
         pme->theDrawnItem = ( pme->m_nSelect + 1);
#endif//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

         // Draw Menu Item
         if(IS_PROP_SET( pme->m_dwProps, MP_TRANSPARENT_UNSEL))
         {
            pme->m_bAutoScrollMenuItem = TRUE;
            Menu_Draw(pme);
         }
         
         Menu_DrawItem(pme, pi, &rc, TRUE, 0);

         // Restore Colors
         ResetColors(pme->m_pIDisplay, clrBack, clrText,clrFrame);

         IDISPLAY_Update(pme->m_pIDisplay);
      }
   }        
   else
      nIdxNew = 0;   // Reset Scroll Character Index

   pme->m_nAutoScrollIdx = nIdxNew;

   // Delay for first/last character
   ISHELL_SetTimer(pme->m_pIShell,(uint32)(nIdx && nIdxNew ? pme->m_wMenuTextScroll : AUTO_SCROLL_START), (PFNNOTIFY)(Menu_AutoScroll), pme);
}

/*=====================================================================

Local Method - Resets the auto-scroll timer for a menu item.

======================================================================*/
static void Menu_ResetAutoScroll(CMenuCtl * pme)
{
   CMenuItem   * pi;

   // Get current selection
   pi = GetItemByIdx(pme->m_pItemList, pme->m_nSelect);
   pme->m_nAutoScrollIdx = 0;
// add these @08.01.15
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   if( !pme->userSetStyle && pme->m_nItems > 1)
    {
        pi->nFont = AEE_FONT_BOLD;
    }
#endif//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE) 

   if(pi && pme->m_bActive && Menu_ItemScrolls(pme, pi, 0)){
      pme->m_bAutoScroll = TRUE;

      // We set the timer to go off pretty quickly.  That's because the AutoScroll function
      // will automatically set a longer timer for the first and last characters...

      ISHELL_SetTimer(pme->m_pIShell,pme->m_wMenuTextScroll, (PFNNOTIFY)(Menu_AutoScroll), pme);
   }
   else{
      // Caccel Auto Scroll Timer
      ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)(Menu_AutoScroll), pme);

      pme->m_bAutoScroll = FALSE;

      // Reset Auto Scroll Rect
      MEMSET(&pme->m_rcAutoScroll, 0, sizeof(AEERect));
   }
}

/*=====================================================================

Local Method - Determines whether a menu item scrolls

======================================================================*/
static boolean Menu_ItemScrolls(CMenuCtl * pme, CMenuItem *pi, int nIdx)
{
   int   cx,cxMax;

   // Measure item text width starting from index
   cx = IDISPLAY_MeasureText(pme->m_pIDisplay, pi->nFont, pi->text) - nIdx + pme->m_cxItemExtra;

   // Adjust with image width & padding
   if(pi->cxImage){
      if(cx)
         cx += MENU_IMAGE_PAD;
      cx += pi->cxImage;
   }

   cxMax = pme->m_rc.dx;   // Max Width
   
// add these @08.01.15
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   //pme->m_nItems <= 10    
   if( !pme->userSetStyle && 
        ( (IS_PROP_SET( pme->m_dwProps, MP_BIND_ITEM_TO_NUMBER_KEY) && pme->m_nItems <= MENU_ITEM_MAX) ||
          IS_MULTI_SEL( pme)
        )
    )
    {
        cxMax -= 13;
    }

   if( !pme->userSetStyle && (IS_PROP_SET( pme->m_dwProps, MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT) ) )
    {
    	//10 + MENU_IMAGE_PA
        cxMax -= (MENU_ITEM_MAX + MENU_IMAGE_PAD);//10 should be cx of picture check_yes.bmp 
    }

#endif//#if defined( FEATURE_CUSTOMIZED_MENU_STYLE) 
    
   // Adjust Max Width for scroll bar
   if (pme->m_nItems > pme->m_nPageItems) 
      cxMax -= pme->m_nSBWidth;

   return(cx > cxMax);
}

/*=====================================================================

Local Method - Moves the selection on a standard menu.

======================================================================*/
static void Menu_MoveSelect(CMenuCtl * pme, int nDir, boolean bRedraw)
{
   uint16 nSaveSelect;
   uint16 nSaveScroll;
   int    nDiff;

   Cal_CalcPageItems(pme);

   nSaveSelect = pme->m_nSelect;
   nSaveScroll = pme->m_nScrollItem;

   if (nDir < 0) {   
      if (WRAP_SCROLL(pme) && (pme->m_nSelect == 0)&& (pme->m_nItems > 0)) {
         pme->m_nSelect = pme->m_nItems-1;
         pme->m_nScrollItem = (uint16)__MAX(0, (int)pme->m_nItems - (int)pme->m_nPageItems);
      }
      else {
         if (pme->m_nSelect > 0) {
            pme->m_nSelect--;
            if (pme->m_nSelect < pme->m_nScrollItem) 
               pme->m_nScrollItem = pme->m_nSelect;
         }
      }
   }
   else {
      if (WRAP_SCROLL(pme) && (pme->m_nSelect == pme->m_nItems-1)) {
         pme->m_nSelect = 0;
         pme->m_nScrollItem = 0;
      }
      else {
         if (pme->m_nSelect < pme->m_nItems-1) {
            pme->m_nSelect++;

      // If the list item sizes are dynamic, we need to keep recalculating until it fits...

            while(1){
               nDiff = (int)(pme->m_nSelect - (pme->m_nScrollItem + pme->m_nPageItems)) + 1;
               if(nDiff <= 0)
                  break;
               pme->m_nScrollItem += nDiff;
               Cal_CalcPageItems(pme);
            }
         } 
      }
   }

   if (bRedraw && (nSaveSelect != pme->m_nSelect || nSaveScroll != pme->m_nScrollItem)){
      if(nSaveSelect != pme->m_nSelect)
         Menu_ResetAutoScroll(pme);

      if(nSaveScroll == pme->m_nScrollItem && ITEM_IS_VISIBLE(pme, nSaveSelect) && ITEM_IS_VISIBLE(pme, pme->m_nSelect))
         Menu_DrawNewSelectItem(pme,nSaveSelect,pme->m_nSelect);
      else
            Menu_Draw(pme);
   }

   // Notify them that the selection changed.
   if(nSaveSelect != pme->m_nSelect)
      Menu_NotifySelChange(pme);
}

/*=====================================================================

Local Method - Notifies the owner when selection changes.

======================================================================*/
static void Menu_NotifySelChange(CMenuCtl * pme)
{
   CMenuItem * pmi = GetItemByIdx(pme->m_pItemList,pme->m_nSelect);

   if(pmi)
      ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_SEL_CHANGED, (uint16)pmi->nItemID, (uint32)pme);
}

/*=====================================================================

Local Method - Draws a standard menu scroll bar

======================================================================*/
static void Menu_DrawScrollBar(CMenuCtl * pme)
{
   IDisplay *  pd = pme->m_pIDisplay;
   AEERect rctFrame, rctThumb;
   uint16   wPos, wLen;

   if (!(pme->m_dwProps&MP_WRAPSCROLL)) 
   {
   	return;
   }
   if( !Menu_GetScrollBarRects(pme, &rctFrame, &rctThumb, &wPos, &wLen) ){
      if (IS_SB_OD(pme))
      {
         // Scroll Bar is owner draw

         AEEODCBData cbData;
         MEMSET(&cbData, 0, sizeof(AEEODCBData));

         cbData.nSize = sizeof(AEEODCBData);
         cbData.nAction = ODA_DRAW_SB;
         cbData.pIDisplay = pd;
         SETAEERECT(&cbData.rc, rctFrame.x, rctFrame.y, rctFrame.dx, rctFrame.dy);
         cbData.nActionInfo = (uint32)(rctFrame.y + wPos);
         cbData.nActionInfo <<= 16;
         cbData.nActionInfo |= wLen;

         // Draw Scroll Bar
         pme->m_pfnCB(pme->m_pvContext, &cbData);
      }
      else
      {
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
#ifdef FEATURE_SCROLLBAR_USE_STYLE
        {
            AEERect ScrollbarDarkRect;
            //RGBVAL  ScrollbarDarkClr = MAKE_RGB(0xFF, 0x70, 0x00);//wlh mod
			RGBVAL  ScrollbarDarkClr = MAKE_RGB(0xFF, 0x70, 0x00); //modify by miaoxiaoming indonesian
             
            pme->m_c.cScrollbar     = MAKE_RGB(0xDE, 0xDE, 0xDE);
            //pme->m_c.cScrollbarFill = MAKE_RGB(0xFF, 0xC0, 0x60);//wlh mod
			pme->m_c.cScrollbarFill = MAKE_RGB(0xFF, 0x70, 0x00); //modify by miaoxiaoming indonesian
            SETAEERECT(&ScrollbarDarkRect, 
                                rctThumb.x + AEE_FRAME_SIZE, 
                                rctThumb.y - AEE_FRAME_SIZE, 
                                rctThumb.dx - 2*AEE_FRAME_SIZE, 
                                rctThumb.dy + 2*AEE_FRAME_SIZE);
            
            IDISPLAY_FillRect(pd, &rctFrame, pme->m_c.cScrollbar);
            IDISPLAY_FillRect(pd, &rctThumb, pme->m_c.cScrollbarFill);
            IDISPLAY_FillRect(pd, &ScrollbarDarkRect, ScrollbarDarkClr);
         }
#else
        if( !pme->userSetStyle)
        {
            Theme_Param_type themeParms;
            Appscom_GetThemeParameters( &themeParms);

            pme->m_c.cScrollbar     = themeParms.textColor;
            pme->m_c.cScrollbarFill = themeParms.themeColor;
        }
         // Draw the frame
         IDISPLAY_DrawFrame(pd, &rctFrame, AEE_FT_BOX, pme->m_c.cScrollbar);

         // Display Top/Bottom arrows if wrap scroll property is set
         if(WRAP_SCROLL(pme)){
            AEERect rc;

            // Never Executed!
            // Note that this is a 3x3 arrow. That's why CXARROW is used
            // for x as well as y calculation

            // draw top arrow
            int xa = rctFrame.x + ((rctFrame.dx - CXARROW)/2);
            SETAEERECT(&rc, xa + 1, rctFrame.y, 1, 1);
            IDISPLAY_FillRect(pd, &rc, pme->m_c.cFrame);

            SETAEERECT(&rc, xa, rctFrame.y + 1, CXARROW, 1);
            IDISPLAY_FillRect(pd, &rc, pme->m_c.cFrame);

            // draw bottom arrow
            SETAEERECT(&rc, xa + 1, rctFrame.y + rctFrame.dy - 1, 1, 1);
            IDISPLAY_FillRect(pd, &rc, pme->m_c.cFrame);

            SETAEERECT(&rc, xa, rctFrame.y + rctFrame.dy - 2, CXARROW, 1);
            IDISPLAY_FillRect(pd, &rc, pme->m_c.cFrame);
         }
         
         IDISPLAY_FillRect(pd, &rctThumb, pme->m_c.cScrollbarFill);
#endif //ifdef FEATURE_SCROLLBAR_USE_STYLE
#endif //if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      }
   }
}

/*=====================================================================

Local Method - Gets rectangles of the scrollbar if it has one
pwLen and pwPos are not required and can be passed in as NULL.

======================================================================*/
static int Menu_GetScrollBarRects(CMenuCtl * pme, AEERect * prcFrame, AEERect * prcThumb, uint16 * pwPos, uint16 * pwLen)
{
   int         cx;
   int         nScrollItem, x, y, cy, nItems, nPageItems;
   uint16      nRange, nPos, nLen;

   if(IS_ICON_VIEW(pme)){

      if(!pme->m_bIconVScroll)
         return EFAILED;

      nItems = pme->m_nTotalRows;
      nPageItems = pme->m_nRows;
   }
   else{
      nItems = pme->m_nItems;
      nPageItems = pme->m_nPageItems;
   }
   nScrollItem = pme->m_nScrollItem;

   if (nItems > nPageItems) {
      // Adjust for frame size
      cx = IS_SB_OD(pme) ? pme->m_nSBWidth : (pme->m_nSBWidth - 2*AEE_FRAME_SIZE);
      x = (pme->m_rc.x + pme->m_rc.dx - cx);
      y = pme->m_rc.y;

      // each item in calendar view could have different cy, so m_cyMaxItem can't tell 
      // us how much the actual cy should be
      if(IS_MENU(pme) && !IS_CALENDAR_VIEW(pme))   
         cy = nPageItems * pme->m_cyMaxItem;
      else
         cy = pme->m_rc.dy;

      // In an icon view, the scrollbar sits beside the entire icon window.  That
      // means everything EXCEPT the text area...
      if(IS_ICON_VIEW(pme)){
         if(pme->m_bHasText){
            uint16 cyText = Icon_GetMaxTextHeight(pme);
            cy -= cyText;
            if(ALIGN_TEXT_TOP(pme))
               y += cyText;
         }
      }
      else
      { 
         if(pme->m_pTitle || IS_TITLE_OD(pme))
         {
            if(!IS_MENU(pme))
            {
              cy -= pme->m_rcTitle.dy;
            }
            
            y += pme->m_rcTitle.dy;
         } 
      }    

      if (IS_SB_OD(pme))
         nRange = cy;
      else
      {
#ifdef FEATURE_SCROLLBAR_USE_STYLE
         nRange = cy - 4*AEE_FRAME_SIZE;
#else
         nRange = cy - 2*AEE_FRAME_SIZE;
         if (WRAP_SCROLL(pme))
            nRange -= 2*CXARROW;
#endif
      }
    
      nRange *= 10;    // multiply by 10 for rounding
  
      nLen = (nRange * nPageItems)/nItems;      // Scroll Bar Length    
      nPos = (nRange * nScrollItem)/nItems;     // Scroll Bar Position
   
      nPos = (nPos + 5) / 10;     // rounding
      nLen = (nLen + 5) / 10;     // rounding

      if( pwPos )
      {
         *pwPos = nPos;
      }
      if( pwLen )
      {
         *pwLen = nLen;
      }
      SETAEERECT(prcFrame, x, y , cx, cy);
#ifdef FEATURE_SCROLLBAR_USE_STYLE
      y += (2*AEE_FRAME_SIZE);
      cy -= (4*AEE_FRAME_SIZE);
#else
      if(WRAP_SCROLL(pme))
      {
         // Never executed!
         y += CXARROW;
      }
      
      // Adjust x, y and cx for scroll bar fill
      y += AEE_FRAME_SIZE;
      x += AEE_FRAME_SIZE;
      cx -= (AEE_FRAME_SIZE * 2);
      cy -= (AEE_FRAME_SIZE * 2);
#endif

      nLen = MAX(1,nLen);
      SETAEERECT(prcThumb, x, y + MIN(cy-nLen,nPos), cx, nLen);
      return AEE_SUCCESS;
   }
   return EFAILED;
}

/*=====================================================================

Local Method - Scrolls per where pen hit!

======================================================================*/
static void Menu_ScrollTimerCB(CMenuCtl *pme)
{
   pme->m_ptTracker.cbFlags &= ~PTRCK_GEN_TMRSET;
   Menu_ScrollByPos(pme, ((pme->m_ptTracker.cbHit & PTRCK_HIT_BELOW) ? 1 : -1), pme->m_ptTracker.ptPosition.x, pme->m_ptTracker.ptPosition.y);
}

/*=====================================================================

Local Method - Scrolls text in the Menu by direction and point
                  in a non-thumb area.

======================================================================*/
static boolean Menu_ScrollByPos(CMenuCtl * pme, int nDir, int16 wXPos, int16 wYPos)
{
   AEERect  rctFrame, rctThumb;

   if( !Menu_GetScrollBarRects(pme, &rctFrame, &rctThumb, NULL, NULL) ){
      boolean  bMoved = FALSE;

      if( PT_IN_RECT(wXPos, wYPos, rctFrame) ){
         // Selection offset from top of visible page
         int      nOffset;
         int      nScrl;   // New Scroll item
         uint16   wFocus;  // New Focus item

         // If the point is in the segment expected
         if( nDir > 0 ){
            if( wYPos > (rctThumb.y+rctThumb.dy) ){   // Already bound by SB Frame
               if(IS_ICON_VIEW(pme)){

                  if(!pme->m_bIconVScroll)
                     return FALSE;

                  nOffset  = pme->m_nSelect - (pme->m_nScrollItem*pme->m_nCols);
                  nScrl    = pme->m_nRows + pme->m_nScrollItem; // Next Page!

                  if( nOffset < 0 )
                     nOffset = 0;
                  wFocus   = ((uint16)nScrl*pme->m_nCols) + (uint16)nOffset;

                  // Scroll by full page view only
                  if((nScrl*pme->m_nCols) > (pme->m_nItems - (pme->m_nRows*pme->m_nCols)) ){
                     nScrl = ((pme->m_nItems+(pme->m_nCols-1))/pme->m_nCols) - pme->m_nRows;
                     // Fix the focus
                     if( wFocus > pme->m_nItems ){
                        // Keep focus where we were, page moved, selected item still on page
                        wFocus = pme->m_nSelect;
                     }else{
                        // Item no longer on page, focus to top of page
                        wFocus = (nScrl*pme->m_nCols) + (uint16)nOffset;
                     }
                  }
               }else{
                  nOffset = pme->m_nSelect - pme->m_nScrollItem;
                  nScrl    = pme->m_nPageItems + pme->m_nScrollItem; // Next Page!

                  if( nOffset < 0 )
                     nOffset = 0;
                  wFocus   = nScrl + (uint16)nOffset;   

                  // Scroll by full page view only
                  if(nScrl > (pme->m_nItems - pme->m_nPageItems) ){
                     nScrl = pme->m_nItems - pme->m_nPageItems;
                     // Fix the focus
                     if( wFocus > pme->m_nItems ){
                        // Keep focus where we were, page moved, selected item still on page
                        wFocus = pme->m_nSelect;
                     }else{
                        // Item no longer on page, focus to top of page
                        wFocus = (uint16)nScrl;
                     }
                  }
               }
               if( pme->m_nScrollItem != (uint16)nScrl 
                  || wFocus != pme->m_nSelect ){
                  bMoved = TRUE;
               }
            }
         }else{
            if( wYPos < rctThumb.y ){  // Already bound by SB Frame
               if(IS_ICON_VIEW(pme)){

                  if(!pme->m_bIconVScroll)
                     return FALSE;

                  nOffset  = pme->m_nSelect - (pme->m_nScrollItem*pme->m_nCols);
                  nScrl    = pme->m_nScrollItem - pme->m_nRows;

                  if( nScrl < 0 ){
                     nScrl = 0;

                     if( pme->m_nSelect >= (pme->m_nRows*pme->m_nCols) ){
                        wFocus = (uint16)nOffset;
                     }else{
                        wFocus = pme->m_nSelect;
                     }
                  }else{
                     // Keep focus as if a Page Up occurred, focus is same spot on prev screen
                     wFocus   = (nScrl*pme->m_nCols) + (uint16)nOffset;
                  }
               }else{
                  // Go back a page
                  nOffset  = pme->m_nSelect - pme->m_nScrollItem;
                  nScrl    = pme->m_nScrollItem - pme->m_nPageItems;

                  if( nScrl < 0 ){
                     // Not too far, back to the beginning
                     nScrl = 0;
                     // If the focus is off the screen bring it to the last item
                     if( pme->m_nSelect >= pme->m_nPageItems ){
                        wFocus = pme->m_nPageItems-1;
                     }else{
                        // Otherwise keep same item focused since it is still in screen
                        wFocus = pme->m_nSelect;
                     }
                  }else{
                     // Keep focus as if a Page Up occurred, focus is same spot on prev screen
                     wFocus   = (uint16)nScrl + (uint16)nOffset;
                  }
               }
               if( pme->m_nScrollItem != nScrl 
                  || wFocus != pme->m_nSelect ){
                  bMoved = TRUE;
               }
            }
         }
         if( bMoved ){
            uint32   dwProps = pme->m_dwProps;
            CMenuItem * pFocus;

            pme->m_nScrollItem = (uint16)nScrl;
            if( !ISHELL_SetTimer(pme->m_pIShell, pme->m_arPenRepeat.dwRate, (PFNNOTIFY)Menu_ScrollTimerCB, (void *)pme) ){
               pme->m_ptTracker.cbFlags |= PTRCK_GEN_TMRSET;
            }
            pme->m_dwProps &= ~MP_NO_REDRAW; // Prevent potential flicker
            if( (pFocus = GetItemByIdx(pme->m_pItemList, wFocus)) != NULL ){
               IMENUCTL_SetFocus((IMenuCtl *)pme, pFocus->nItemID);
            }
            pme->m_dwProps = dwProps;
			MSG_FATAL("IMenuCtl_Redraw redraw.......55555",0,0,0);
            IMenuCtl_Redraw((IMenuCtl *)pme);
            if(IS_ICON_VIEW(pme)){
               // Icons need to redraw text, as focus is not moved!
               CMenuItem * pmi = GetItemByIdx(pme->m_pItemList, pme->m_nSelect);
               if(pmi){
                  if (IS_ITEM_OD(pmi))
                     Icon_DrawODItemText(pme, pmi, 0, TRUE);
                  else
                     Icon_DrawItemText(pme, pmi, 0, TRUE);
               }
            }
            return TRUE;
         }
      }
   }
   return FALSE;
}

/*=====================================================================

Local Method - Initializes background, text and frame colors with
CLR_USER_BACKGROUND, CLR_USER_TEXT and CLR_USER_FRAME.

======================================================================*/
static void Menu_InitColors(CMenuCtl * pme, RGBVAL * pBack, RGBVAL * pText, RGBVAL * pFrame)
{
   IDisplay * pd = pme->m_pIDisplay;
	
   *pBack = IDISPLAY_SetColor(pd, CLR_USER_BACKGROUND, pme->m_c.cBack);
   *pText = IDISPLAY_SetColor(pd, CLR_USER_TEXT, pme->m_c.cText);
   *pFrame = IDISPLAY_SetColor(pd, CLR_USER_FRAME, pme->m_c.cFrame);
}


/*=====================================================================

Local Method - Draws scrollbar arrows in a menu.

======================================================================*/
static void Menu_DrawArrows(CMenuCtl * pme, int x, int y, int xEnd)
{
   IDisplay *  pd = pme->m_pIDisplay;
   AEERect     rc;
   RGBVAL      c = pme->m_c.cText;

   // Draw left arrow using text color
   SETAEERECT(&rc, x+0, y+2, 1, 1);
   IDISPLAY_FillRect(pd, &rc, c);
   
   SETAEERECT(&rc, x+1, y+1, 1, 3);
   IDISPLAY_FillRect(pd, &rc, c);
   
   SETAEERECT(&rc, x+2, y+0, 1, 5);
   IDISPLAY_FillRect(pd, &rc, c);
   
   x = xEnd;
       
   // draw right arrow using text color  
   SETAEERECT(&rc, x+0, y+0, 1, 5);
   IDISPLAY_FillRect(pd, &rc, c);
   
   SETAEERECT(&rc, x+1, y+1, 1, 3);
   IDISPLAY_FillRect(pd, &rc, c);
   
   SETAEERECT(&rc, x+2, y+2, 1, 1);
   IDISPLAY_FillRect(pd, &rc, c);
}

/*=====================================================================

Local Method - Handles Selection type events for Menu. (AVK_SELECT and Pens)

======================================================================*/
static boolean Menu_HandleSelection(CMenuCtl * pme, int nSelect)
{
   DBGPRINTF("Menu_HandleSelection %d %d %d",pme->m_bSendCmd,nSelect,pme->m_bResumeCmd);
   if (pme->m_bSendCmd) {
      CMenuItem   * pItem;
      if((pItem = GetItemByIdx(pme->m_pItemList, nSelect)) != NULL){
         if( IS_MULTI_SEL(pme) ){
            pItem->bIsSel = !pItem->bIsSel;
            IMenuCtl_SetSelEx((IMenuCtl *)pme, pItem->nItemID, pItem->bIsSel);
			MSG_FATAL("IMenuCtl_Redraw redraw.......66666",0,0,0);
            IMenuCtl_Redraw((IMenuCtl *)pme);
         }
         if(IS_TRI_STATE(pme)){
            pItem->bPressed = TRUE;
			MSG_FATAL("IMenuCtl_Redraw redraw.......77777",0,0,0);
            IMenuCtl_Redraw((IMenuCtl *)pme);
            pItem->bPressed = FALSE;
         }
         if(pme->m_bResumeCmd){
            CALLBACK_Cancel(&pme->m_cbCommand);
            CALLBACK_Init(&pme->m_cbCommand,Menu_SendCommand,pme);
            ISHELL_Resume(pme->m_pIShell,&pme->m_cbCommand);
            return(TRUE);
         }
         return(ISHELL_HandleEvent(pme->m_pIShell, EVT_COMMAND, (uint16)pItem->nItemID, (uint32)pme));
      }
   }
   return FALSE;
}

/*=====================================================================

Local Method - Handles TAP events for a menu.

======================================================================*/
static uint16 Menu_HandleTap(CMenuCtl * pme, int x, int y)
{
   AEERect        rc;
   int            nIdx,nMax;
   SKDrawItem *   pdi = NULL;
   CMenuItem *    pi = NULL;
   AEECLSID       cls = pme->m_cls;

   // Ignore if control is empty or TAP was outside control rect 
   if(!pme->m_pItemList || !Intersects(&pme->m_rc,x,y))
      return(AVK_UNDEFINED);

   // Treat it as AVK_SELECT for AEECLSID_LISTCTL
   if(cls == AEECLSID_LISTCTL)
      return(AVK_SELECT);

   MEMSET(&rc, 0, sizeof(AEERect));
   nIdx = 0;

   if(cls == AEECLSID_SOFTKEYCTL){
      // Calculate Softkey Layout
      SK_CalcLayout(pme);

      pdi = &pme->m_draw[0];        // first draw item
      nMax = pme->m_nVisibleItems;  // visible items

      // SK has fixed y, dy
      rc.y = pme->m_rc.y;
      rc.dy = pme->m_rc.dy;
   }
   else{
      if(cls == AEECLSID_ICONVIEWCTL)
         nMax = pme->m_nItems;
      else
      {
         // For AEECLSID_MENUCTL, we need to examine items between scroll item
         // and (scroll item + items displayable on a page)
         nIdx  = pme->m_nScrollItem;
         nMax  = pme->m_nPageItems + nIdx;

         if(nMax > pme->m_nItems)
            nMax = pme->m_nItems;

         // Menu has fixed x, dx
         rc = pme->m_rc;

         if(pme->m_pTitle || IS_TITLE_OD(pme))
            rc.y += pme->m_rcTitle.dy;

         rc.dy = pme->m_cyMaxItem;  // Height of tallest item
         rc.y -= rc.dy;             // Initial y for AEECLSID_MENUCTL
      }
   }

   // For each item, calculate item cell and check if that intersects with TAP position.
   // If yes, we change pme->m_nSelect and return AVK_SELECT.
   for(; nIdx < nMax; nIdx++)
   {
      switch(cls)
      {
         case AEECLSID_ICONVIEWCTL:
            // Calculate grid rect
            Icon_CalcGridRect(pme, &rc, nIdx);
            break;

         case AEECLSID_SOFTKEYCTL:
            //Update pdi to point to next item
            pdi = &pme->m_draw[nIdx];
            rc.x = pdi->x;
            rc.dx = pdi->dx;

            // Must lookup the idx for the item...
            if(Intersects(&rc,x,y)){
               for(nIdx = 0, pi = pme->m_pItemList; pi != NULL; pi = pi->pNext, nIdx++){
                  if(pi == pdi->pmi){
                     pme->m_nSelect = nIdx;
                     return(AVK_SELECT);
                  }
               }
            }
            break;

         default:
            // Calculate y position
            rc.y += rc.dy;
      }
      if(cls != AEECLSID_SOFTKEYCTL && Intersects(&rc, x, y)){
         pme->m_nSelect = nIdx;
         return(AVK_SELECT);
      }         
   }

   // No menu items intersects with TAP position ...

   // Let's see if user tapped on left arrow. If so, move left and return AVK_LEFT
   SETAEERECT( &rc, pme->m_rc.x, pme->m_rc.y, 5, pme->m_rc.dy );
   if( Intersects( &rc, x, y ) ) {
     // tap on left arrow
     // if there are items not displayed, scroll display
     if( pme->m_nItems > pme->m_nPageItems ) {
       if(pme->m_nScrollItem == 0 )
         pme->m_nScrollItem = pme->m_nItems - pme->m_nPageItems;
       else
         pme->m_nScrollItem--;
     }

     return( AVK_LEFT );
   }

   // Let's see if user tapped on right arrow. If so, move left and return AVK_RIGHT
   SETAEERECT( &rc, pme->m_rc.x + pme->m_rc.dx - 5, pme->m_rc.y,5, pme->m_rc.dy );

   if( Intersects( &rc, x, y ) ) {
     // tap on right arrow
     // if there are items not displayed, scroll display
     if( pme->m_nItems > pme->m_nPageItems ) {
       if(pme->m_nScrollItem >=  pme->m_nItems - pme->m_nPageItems)
         pme->m_nScrollItem = 0;
       else
         pme->m_nScrollItem++;
     }

     return( AVK_RIGHT );
   }

   return(AVK_UNDEFINED);
}
         
/*=====================================================================

Local Method - Returns the style offset for a menu.

======================================================================*/
static void Menu_GetStyleOffsets(CMenuCtl * pme,int * pcx, int * pcy)
{
   int            nFrame = 0, cxMax = 0, cyMax = 0, cx, cy, i;
   AEEItemStyle * ps = pme->m_style;

   // For styles (Normal/Select), calculate max pixels added to width &
   // height by any of these
   for(i = 0; i < sizeof(pme->m_style)/sizeof(AEEItemStyle); i++, ps++){
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

/*=====================================================================

Local Method - (Re)allocates a menu item.

======================================================================*/
static CMenuItem * Menu_AllocItem(CMenuCtl * pme, CMenuItem *piOrig, AECHAR * pText, const char * pszResImage, CMenuItem * piPrev)
{   
   int            nSize, nSizeBase, nStrSize;
   CMenuItem   *  pi;

   // If no resource file specified, use original's resource file if there
   if(!pszResImage && piOrig)
      pszResImage = piOrig->pszImage;

   // Text String Size
   if(pText)
      nStrSize = WSTRSIZE(pText);
   else
      nStrSize = 0;

   // Base Size
   nSize = nSizeBase = sizeof(CMenuItem) + nStrSize;

   // Image Resource File Name String Size
   if(pszResImage)
      nSize += STRLEN(pszResImage) + 1;

   // Allocate Memory
   pi = (CMenuItem *)MALLOC(nSize);

   if(pi){
      // If original is there, copy its content to new
      if(piOrig)
         MEMCPY(pi, piOrig, sizeof(CMenuItem));

      if(pszResImage){
         pi->pszImage = (const char *)pi + nSizeBase;
         STRCPY((char *)pi->pszImage, pszResImage);
      }
      else
         pi->pszImage = NULL;

      // Copy Item Text
      if(nStrSize)
      {
         WSTRCPY(pi->text, pText);
         if (piOrig)
            pi->cxText = IDISPLAY_MeasureText(pme->m_pIDisplay, pi->nFont, pi->text);
      }
      else
      {
         *pi->text = 0;
         pi->cxText = 0;
      }

      // Put it into the right spot in the list
      if(piPrev)
         piPrev->pNext = pi;
      else
        pme->m_pItemList = pi;

      if(piOrig){
         if( pme->m_pLastItem == piOrig ){
            pme->m_pLastItem = pi;  // Last item changed, our key is the pointer val!
         }
         FREE(piOrig);        // Free Original
      }else{
         pme->m_nItems++;     // Increase Item Count
         pme->m_pLastItem = pi;
      }
   }
   return(pi);
}


//===========================================================================
//
//
//
//                LIST ROUTINES
//
//
//
//===========================================================================
/*=====================================================================

Local Method - Returns List Item Rect

======================================================================*/
static void List_CalcItemRect(CMenuCtl * pme, AEERect * prc, AEEFont nFont)
{
   IDisplay *  pd = pme->m_pIDisplay;

   // draw the title text
   *prc = pme->m_rc;

   if (pme->m_pTitle || IS_TITLE_OD(pme)){
      if (pme->m_pTitle)
         prc->x += IDISPLAY_MeasureText(pd, nFont, pme->m_pTitle) + LIST_TITLE_PAD;
      else
         // Title is owner draw
         prc->x += pme->m_rcTitle.dx + LIST_TITLE_PAD;
   }
}

/*=====================================================================

Local Method - Draws a list menu.

======================================================================*/
static boolean List_Draw(CMenuCtl * pme)
{
   AEERect     qrc;
   RGBVAL      clrBack, clrText;
   IDisplay *  pd = pme->m_pIDisplay;
   CMenuItem * p = GetItemByIdx(pme->m_pItemList, pme->m_nSelect);

   // erase previous menu
   IDISPLAY_FillRect(pd, &pme->m_rc, pme->m_c.cBack); 

   clrBack = IDISPLAY_SetColor(pd, CLR_USER_BACKGROUND, pme->m_c.cTitle);
   clrText = IDISPLAY_SetColor(pd, CLR_USER_TEXT, pme->m_c.cTitleText);   

   // draw the title text
   qrc = pme->m_rc;

   if (pme->m_pTitle || IS_TITLE_OD(pme)){
      if (pme->m_pTitle)
      {
         // Draw title on the appropriate side of the list menu
         if (ParagraphAlignment((AECHAR *)(pme->m_pTitle), WSTRLEN(pme->m_pTitle)) == IDF_ALIGN_RIGHT) {
            qrc.dx -= IDISPLAY_MeasureText(pd, AEE_FONT_NORMAL, pme->m_pTitle) + 5;
            IDISPLAY_DrawText(pd, AEE_FONT_NORMAL,pme->m_pTitle, -1, qrc.x + qrc.dx + 5, qrc.y+2,NULL,IDF_RECT_NONE);
         } else {
            IDISPLAY_DrawText(pd, AEE_FONT_NORMAL,pme->m_pTitle, -1, qrc.x, qrc.y+2,NULL,IDF_RECT_NONE);
            qrc.x += IDISPLAY_MeasureText(pd, AEE_FONT_NORMAL, pme->m_pTitle) + 5;
         }
      }
      else
      {
         // Title is owner draw

         AEEODCBData cbData;
         MEMSET(&cbData, 0, sizeof(AEEODCBData));

         cbData.nSize = sizeof(AEEODCBData);
         cbData.nAction = ODA_DRAW_TITLE;
         cbData.pIDisplay = pd;
         SETAEERECT(&cbData.rc, qrc.x, qrc.y, pme->m_rcTitle.dx, pme->m_rcTitle.dy);
         qrc.x += pme->m_rcTitle.dx + LIST_TITLE_PAD;

         // Draw Title
         pme->m_pfnCB(pme->m_pvContext, &cbData);
      }
   }

   //If there is no item in the list, return
   if(!p)
       return(FALSE);

   Menu_SetItemColor(pme, pd, (boolean)pme->m_bActive, NULL);

   if (IS_ITEM_OD(p))
   {   
      AEEODCBData cbData;

      // Initialize callback for owner draw item
         
      MEMSET(&cbData, 0, sizeof(AEEODCBData));

      cbData.nSize = sizeof(AEEODCBData);
      cbData.nAction = ODA_DRAW_ITEM;
      cbData.pIDisplay = pd;
      cbData.rc = qrc;
      cbData.nActionInfo = p->nItemID;
      cbData.nActionInfo <<= 16;
      if (pme->m_bActive)
         cbData.nActionInfo |= 0x00000001;

      // Draw Item
      pme->m_pfnCB(pme->m_pvContext, &cbData);
   }
   else
   {
      // Draw Item Text
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
       boolean width = IDISPLAY_MeasureText( pme->m_pIDisplay, p->nFont, p->text);
	   uint32 alignment = IDF_RECT_FILL | ParagraphAlignment((AECHAR *)(p->text), WSTRLEN(p->text));
       if( pme->m_dwOemProps & OEMMP_IDF_ALIGN_CENTER)
       {
           if( width <= qrc.dx)
           {
               alignment = IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE;
           }
           else
           {
               alignment = IDF_ALIGN_MIDDLE|ParagraphAlignment((AECHAR *)(p->text), WSTRLEN(p->text));
           }
       }
       
	   IDISPLAY_DrawText(pme->m_pIDisplay, p->nFont,p->text, -1, qrc.x , qrc.y + 2, &qrc, alignment);
#else
      IDISPLAY_DrawText(pme->m_pIDisplay, p->nFont,p->text, -1, qrc.x , qrc.y + 2, &qrc, IDF_RECT_FILL | ParagraphAlignment((AECHAR *)(p->text), WSTRLEN(p->text)));
#endif
   }
	
   IDISPLAY_SetColor(pd, CLR_USER_BACKGROUND, clrBack);
   IDISPLAY_SetColor(pd, CLR_USER_TEXT, clrText);
   
   IDISPLAY_Update(pme->m_pIDisplay);
   return(TRUE);
}

/*=====================================================================

Local Method - Moves the selection in a list menu.

======================================================================*/
static void List_MoveSelect(CMenuCtl * pme, int nDir)
{
   uint16 nSaveSelect = pme->m_nSelect;

   if (nDir < 0) {   
      if (WRAP_SCROLL(pme) && (pme->m_nSelect == 0)) {
         pme->m_nSelect = pme->m_nItems-1;
      }
      else {
         if (pme->m_nSelect > 0)
            pme->m_nSelect--;
      }
   }
   else {
      if (WRAP_SCROLL(pme) && (pme->m_nSelect == pme->m_nItems-1)) {
         pme->m_nSelect = 0;
      }
      else {
         if (pme->m_nSelect < pme->m_nItems-1)
            pme->m_nSelect++;
      }
   }

   if (nSaveSelect != pme->m_nSelect){
      List_Draw(pme);
      Menu_NotifySelChange(pme);
   }
}


//===========================================================================
//
//
//
//                SOFTKEY ROUTINES
//
//
//
//===========================================================================
/*=====================================================================

Local Method - Moves the selection in a softkey menu

======================================================================*/
static boolean SK_MoveSelect(CMenuCtl * pme, int nDir)
{
   uint16 nSaveSelect = pme->m_nSelect;

   if (nDir < 0) {   
      if (pme->m_nSelect == 0){
         pme->m_nSelect = pme->m_nItems-1;
         pme->m_nScrollItem = (uint16)__MAX(0, (int)pme->m_nItems - (int)pme->m_nPageItems);
      }
      else
      if (pme->m_nSelect > 0) {
         pme->m_nSelect--;
         if (pme->m_nSelect < pme->m_nScrollItem)
            pme->m_nScrollItem = pme->m_nSelect;
      } 
   }
   else {
      if (pme->m_nSelect == pme->m_nItems-1) {
         pme->m_nSelect = 0;
         pme->m_nScrollItem = 0;
      }
      else
      if (pme->m_nSelect < pme->m_nItems-1) {
         pme->m_nSelect++;
         if (pme->m_nSelect - pme->m_nScrollItem > pme->m_nPageItems - 1)
            pme->m_nScrollItem++;
      }
   }

   if (nSaveSelect != pme->m_nSelect) {
      int i;

      // Scroll till current selection and scroll items are visible
      for (i = 0; i < pme->m_nPageItems; i++) {
         if (SK_CalcLayout(pme))
            break;
         pme->m_nScrollItem++;
      }

      // Draw
      if(!NO_REDRAW(pme))
         SK_Draw(pme);

      // Notify Change
      Menu_NotifySelChange(pme);
      return(TRUE);
   }
   
   return(FALSE);
}

/*=====================================================================

Local Method - Sets the SKDrawItem list for a softkey menu.  Called only
from SK_CalcLayout.

======================================================================*/
static int SK_SetDrawItems(CMenuCtl * pme, int nDir, int * pnVisibleItems, int * pnLastItem,boolean * pbNoFit)
{
   boolean        bNoFit = 0;
   int            i, x, dx,nMaxText;
   int            nRemain;
   int            nItem, nLastItem;
   CMenuItem *    p = NULL;
   SKDrawItem *   pdi = &pme->m_draw[0];

   // initial remaining space is full skb width
   nRemain = pme->m_rc.dx - CELL_SIDE_WIDTH;

   // initial x position for SK items
   x = pme->m_rc.x + CELL_LMARGIN;

   if (nDir < 0) {   // R -> L
      x += nRemain;  // Rightmost position to start drawing from
      nItem = nLastItem = pme->m_nSelect;       // First item to draw
   }
   else              // L -> R
      nItem = nLastItem = pme->m_nScrollItem;   // First item to draw

   // get starting menu item
   p = GetItemByIdx(pme->m_pItemList, nItem);

   // traverse item list from L->R or R->L

   for (i = 0; nRemain && p && i < pme->m_nPageItems; i++) {

      // Mark selected if item is selected
      pdi->bSelected = (nItem == pme->m_nSelect);

      // Size of any cell is the text width + frame + offset...
      dx = (IS_ITEM_OD(p) ? p->cxImage : (GetItemWidth(p) + pme->m_cxItemExtra));

   // see if item fits in remaining space

      if ((nRemain - dx) < 0) {           // Item doesn't fit

         // Max Text Space = Remaining Space - dot width - width added for style
         nMaxText = nRemain - (IS_ITEM_OD(p) ? dx : (pme->m_cxDotWidth + pme->m_cxItemExtra));

         if (p->pImage || nMaxText <= 0 || nRemain < MIN_CELL_WIDTH) 
            break;         // don't count item

         // We are clipping so we will fill the entire rect...

         dx = nRemain;

         // Width of truncated text
         pdi->xTruncatedExtent = IDISPLAY_MeasureTextEx(pme->m_pIDisplay, p->nFont, p->text, -1, nMaxText, &pdi->cbFits);

         bNoFit = 1;       // Doesn't fit
      }
      else
         pdi->xTruncatedExtent = 0;       // Items Fits

      // if going R->L, subtract cell width first to get x position
      if (nDir < 0)
         x -= dx;

      // add entry to draw item array

      pdi->x         = x;        // x position
      pdi->dx        = dx;       // width
      pdi->bTrunc    = bNoFit;   // Truncated
      pdi->pmi       = p;        // Item
      pdi++;

      // if going L->R, add cell width to get next x position
      if (nDir > 0)
         x += dx;

      nRemain -= dx;
      nLastItem = nItem;

      // Next item 
      if (nDir < 0) {
         if (nItem >= 0)
            p = GetItemByIdx(pme->m_pItemList, --nItem);
      }
      else {
         nItem++;
         p = p->pNext;
      }
   }

   if (pnVisibleItems)
      *pnVisibleItems = i;          // Number of visible items

   if (pnLastItem)
      *pnLastItem = nLastItem;      // Index of item added last

   if (pbNoFit)
      *pbNoFit = bNoFit;            // Flag to indicate that last item was truncated

   return nRemain;                  // Remaining space 
}

/*=====================================================================

Local Method - Handles the Pen events to map to hit data

======================================================================*/
static uint8 SK_HandlePen(CMenuCtl * pme, int16 wXPos, int16 wYPos, uint16 * pwExtraData, uint16 * pwIndex)
{
   SKDrawItem *   pdi   = NULL;
   AEERect        rct;
   int            i;
   MSG_FATAL("SK_HandlePen, Start, wXPos=%d, wYPos=%d, m_nVisibleItems=%d",wXPos,wYPos,pme->m_nVisibleItems);
   // Looking to Fill in the Visible Items and their rects
   SK_CalcLayout(pme);
   // Each item's rect is inside a fixed y boundary
   rct.y = pme->m_rc.y;
   rct.dy = pme->m_rc.dy;
   for( i = 0; i < pme->m_nVisibleItems; i++ ){
      // Cycle through the items to see whether one was hit
      pdi = &pme->m_draw[i];
      rct.x = pdi->x;
      rct.dx = pdi->dx;
      MSG_FATAL("SK_HandlePen x=%d, dx=%d, y=%d",rct.x, rct.dx, rct.y);
      MSG_FATAL("SK_HandlePen dy=%d", rct.dy,0,0);
      if(PT_IN_RECT(wXPos,wYPos,rct)){
         int nIdx;
         // Hit! Now store the selection item for later use.
         CMenuItem * pItem = GetItemByIDEx(pme->m_pItemList, pdi->pmi->nItemID, NULL, &nIdx);
         if( pItem ){
            MSG_FATAL("SK_HandlePen nIdx=%d, nItemID=%d", (uint16)nIdx, pdi->pmi->nItemID, 0);
            *pwIndex  = (uint16)nIdx;
            *pwExtraData = pdi->pmi->nItemID;
            return PTRCK_HIT_ITEM;
         }
         return 0;
      }
   }
   // Check right arrow first, as it is the most likely repeat hit candidate
   SETAEERECT(&rct, pme->m_rc.x + pme->m_rc.dx - ARROWMARGIN, pme->m_rc.y,ARROWMARGIN, pme->m_rc.dy);
   if( PT_IN_RECT(wXPos, wYPos, rct) ) {
      return PTRCK_HIT_BELOW;
   }
   // Now check the left arrow
   SETAEERECT(&rct, pme->m_rc.x, pme->m_rc.y, ARROWMARGIN, pme->m_rc.dy);
   if( PT_IN_RECT(wXPos, wYPos, rct) ) {
      return PTRCK_HIT_ABOVE;
   }
   return 0;
}
/*=====================================================================

Local Method - Recalculates the layout of a softkey menu.

======================================================================*/
static boolean SK_CalcLayout(CMenuCtl * pme)
{
   boolean bNoFit = 0;
   int i;
   int nRemain;
   int nLastItem;

   // we must not exceed the array bounds
   if (pme->m_nPageItems > MAX_SOFT_PAGE_ITEMS)
      pme->m_nPageItems = MAX_SOFT_PAGE_ITEMS;

   // traverse left->right
   nRemain = SK_SetDrawItems(pme, 1, &pme->m_nVisibleItems, &nLastItem, &bNoFit);

   // selected item is not visible, signal that a scroll is required
   if (pme->m_nSelect > nLastItem)
      return(FALSE);

   // if last item is the selected item and did not fit
   // traverse from right->left
   if (bNoFit && (nLastItem == pme->m_nSelect)) {
      nRemain = SK_SetDrawItems(pme, -1, &pme->m_nVisibleItems, &nLastItem, &bNoFit);
      // current scroll item is not visible, signal that a scroll is required
      if (nLastItem > pme->m_nScrollItem)
         return(FALSE);
   }

   // See if we should evenly distribute the buttons.  We do this if:
   // - There is space left; AND
   // - The number of items visible is > 0; AND
   // - (It's a text-only menu; OR It's an icon menu and there are more items to display (offscreen))

   if((nRemain && pme->m_nVisibleItems > 0) && (pme->m_bHasText || pme->m_nVisibleItems < pme->m_nItems)) {

      int cxPerItem = nRemain / pme->m_nVisibleItems;
      int j;
      nRemain %= pme->m_nVisibleItems;
      if (cxPerItem > 0) {
         for (i=0; i < pme->m_nVisibleItems; i++) {
            pme->m_draw[i].dx += cxPerItem;           // Increase Item Cell Width
            
            for (j=i+1; j < pme->m_nVisibleItems; j++)
               pme->m_draw[j].x += cxPerItem;         // Adjust cell start position of remaining cells
         }
      }
      if (nRemain) {
         for (i=0; i < pme->m_nVisibleItems; i++) {
            pme->m_draw[i].dx++;                      // Increment Item Cell Width
            
            for (j=i+1; j < pme->m_nVisibleItems; j++)
               pme->m_draw[j].x++;                    // Adjust cell start position of remaining cells

            if (--nRemain == 0) 
               break;
         }
      }
   }

   return(TRUE);
}

/*=====================================================================

Local Method - Returns Item Rect corresponding to the index

======================================================================*/
static void SK_CalcItemRect(CMenuCtl * pme, uint16 nIdx, AEERect* prc)
{
   int            i;
   SKDrawItem *   pdi = &pme->m_draw[0];
   CMenuItem *    pmi = NULL;
   CMenuItem *    pmiTemp = NULL;

   pmiTemp = GetItemByIdx(pme->m_pItemList, nIdx);
   
   for (i = 0; i < pme->m_nVisibleItems; i++, pdi++) {

      pmi = pdi->pmi;   // Item

      if (pmi->nItemID == pmiTemp->nItemID)
      {
         SETAEERECT(prc, pdi->x, pme->m_rc.y, pdi->dx, pme->m_rc.dy);
         return;
      }
   }
}

/*=====================================================================

Local Method - Draws the softkey menu.

======================================================================*/
static boolean SK_Draw(CMenuCtl * pme)
{
   int            i;
   AEERect        qrc;
   int            cx;                                      // x origin of text
   int            cyCell;                                  // height of cell
   int            yCell;                                   // y origin of cell
   int            yArrow;                                  // y origin of arrow
   int            xRight, xLeft;
   RGBVAL         clrBack, clrText,clrFrame;
   boolean        bSel;
   AEEFrameType   ft;
   SKDrawItem *   pdi = &pme->m_draw[0];
   IDisplay *     pd = pme->m_pIDisplay;
   CMenuItem *    pmi = NULL;

   cyCell      = pme->m_rc.dy;              
   yCell   = pme->m_rc.y;
   
   yArrow   = yCell + ((cyCell - 2) / 2); // Position arrow in the middle
   
   // initialize with menu colors
   Menu_InitColors(pme, &clrBack, &clrText, &clrFrame);
   
   // erase the previous soft key bar
   SETAEERECT(&qrc, pme->m_rc.x, yCell, pme->m_rc.dx, cyCell);

#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   if( !pme->userSetStyle)
   {
       
#if defined( FEATURE_FUNCS_THEME)       
       Theme_Param_type themeParms;
       Appscom_GetThemeParameters( &themeParms);
       
       IDISPLAY_FillRect(pd, &qrc, themeParms.themeColor);
#else
       IDISPLAY_FillRect(pd, &qrc, MAKE_RGB(0xfe, 0x61, 0x01));
#endif   
   }
   else
#endif
       IDISPLAY_FillRect(pd, &qrc, CLR_USER_BACKGROUND);

    // Make sure we draw the appropriate line across any empty tabs.  This is
    // necessary to cover the tab borders above the arrows and any empty (unpainted) cells
    ft = pme->m_style[0].ft;
   if(!IS_MULTI_IMAGE(pme) && (ft == AEE_FT_TAB_BOTTOM || ft == AEE_FT_TAB_TOP)){
        qrc = pme->m_rc;
      IDISPLAY_DrawFrame(pd, &qrc, ft, pme->m_c.cBack);
   }

   // draw each item
   for (i = 0; i < pme->m_nVisibleItems; i++, pdi++) {
      pmi = pdi->pmi;   // Item

      // Center the text in the rect...

      cx = pdi->dx;     // Cell Width

   // Reset the rect each time.  SKDrawItem bombs it...

      SETAEERECT(&qrc, pdi->x, yCell, cx, cyCell);

      bSel = (boolean)(pdi->bSelected && pme->m_bActive);

      if (IS_ITEM_OD(pmi))
         Menu_DrawODItem(pme, pmi, &qrc, bSel);
      else
         Menu_DrawItem(pme, pmi, &qrc, bSel, pdi->xTruncatedExtent);
   }
   
   // draw the arrows if multiple items

   if (pme->m_nItems > 1) {
      xLeft = pme->m_rc.x + 1;
      xRight = pme->m_rc.x + pme->m_rc.dx - CELL_RMARGIN + 1;
      Menu_DrawArrows(pme, xLeft, yArrow, xRight);
   }

   // Restore Colors
   ResetColors(pd, clrBack, clrText, clrFrame);
   
   IDISPLAY_Update(pd);
   
   return(TRUE);
}

//===========================================================================
//
//
//
//                ICON VIEW CODE
//
//
//
//===========================================================================
/*=====================================================================

Local Method - Handles keys of interest to the icon view menu.

======================================================================*/
static boolean Icon_HandleKey(CMenuCtl * pme, uint16 wKey)
{
   int            nSel, nLast, nMax, nDir, nRow, nCol, nNewRow;
   AEERect        rc, rcNew;
   boolean        bScroll;
   RGBVAL         clrBack, clrText, clrFrame;
   CMenuItem *    pmi = NULL;
   CMenuItem *    pmiNew = NULL;

   nSel = nLast = pme->m_nSelect;   // Index of current selection
   nMax = pme->m_nItems - 1;        // Index of last item

   switch(wKey){

      case AVK_RIGHT:
      case AVK_LEFT:

         // Only allow right/left if the displayed columns is > 1 OR there is only 1 row...
         if(pme->m_nCols <= 1 && pme->m_nRows > 1)
            return(TRUE);
   
         // Direction
         nDir = (wKey == AVK_RIGHT ? 1 : -1);

         // Change Selection
         nSel += nDir;

         // Wrap
         if(nSel < 0)
            nSel = nMax;
         else {
            if(nSel > nMax)
               nSel = 0;
         }

         // If MP_ICON_NO_CENTERSEL, don't keep position of selection fixed
         if (pme->m_bIconHScroll && (pme->m_nItems > pme->m_nCols) && (pme->m_dwProps & MP_ICON_NO_CENTERSEL))
         {
            if (nDir > 0)
            {
               if (nSel == 0)
                  pme->m_nScrollItem = 0;
               else {
                  if (nSel >= pme->m_nCols)
                     pme->m_nScrollItem++;
               }
            }
            else
            {
               if (nSel == pme->m_nItems - 1)
                  pme->m_nScrollItem = nSel - pme->m_nCols + 1;
               else
               {
                  if (nSel < pme->m_nScrollItem)
                     pme->m_nScrollItem--;
               }
            }
         }
         break;

      case AVK_DOWN:
      case AVK_UP:

         // Only allow up/down if the displayed rows > 1
         // Modified by dpark : return right away if col count is 0
         if(pme->m_bIconHScroll || pme->m_nCols == 0)
            return(TRUE);

         // Need to adjust selection by number of displayable cols
         nDir = pme->m_nCols;

         if(wKey == AVK_UP)
            nDir = -nDir;
         
         // Get the current row and col...
         nRow = nSel / pme->m_nCols;
         nCol = nSel % pme->m_nCols;

         // Adjust Selection
         nSel += nDir;

         // If the new selection is < 0, see if we can land on the same col in the last row...
         if(nSel < 0){
            nSel = ((pme->m_nTotalRows - 1) * pme->m_nCols) + nCol;

            // If we are still too high, put us on the last item...
            if(nSel > nMax)
               nSel = nMax;
         }
         else{

            // If the new sel is > max index, If so:
            // - See if we are on the last row.  If so, then wrap to row 0;
            // - See if we are above the last row.  If so, place on the last item...
            if(nSel > nMax){
               if(nRow == pme->m_nTotalRows - 1)
                  nSel = nCol;
               else
                  nSel = nMax;
            }
         }         
         break;

      default:
         return(FALSE);
   }

   // Let's not redraw if there is no change in index of current selection or
   // index of current selection looks goofy
   if(nSel == nLast || nSel < 0 || nSel >= pme->m_nItems)
      return(TRUE);

   pme->m_nSelect = nSel;
   
   // Re-draw for single-row of icons...
   if(pme->m_bIconHScroll){
           
      //calculate and set pme->m_nScrollItem if MP_ICON_NO_CENTERSEL is not set
      // based on pme->m_nSelect
      if (!(pme->m_dwProps & MP_ICON_NO_CENTERSEL)){
         int start, end;
         Icon_CalcStartEndItemIndx(pme, &start, &end);
         pme->m_nScrollItem = start;
      }
      
      Icon_Draw(pme);
      Menu_NotifySelChange(pme);
      return(TRUE);
   }

   // Redraw for multi-line of icons...
   Icon_StopAnimation(pme);

   pmi = GetItemByIdx(pme->m_pItemList, nLast);    // Previous Selection
   pmiNew = GetItemByIdx(pme->m_pItemList, nSel);  // New Selection

   if(pmi == NULL || pmiNew == NULL)
      return(TRUE);

   Icon_CalcGridRect(pme, &rcNew, nSel);           // Get rect of new selection
   Icon_CalcGridRect(pme, &rc, nLast);             // Get rect of old selection

   bScroll = FALSE;

   nNewRow = nSel/pme->m_nCols;                    // Index of new selection row

   // If rect of new selection falls outside, we need to redraw entire control
   if(rcNew.y + pme->m_cyMaxItem >= pme->m_rc.y + pme->m_rc.dy - (pme->m_bHasText ? Icon_GetImageOffset(pme) : 0)){
      pme->m_nScrollItem = nNewRow - (pme->m_nRows - 1);
      bScroll = TRUE;
   }
   else{
      if(rcNew.y <= pme->m_rc.y + (pme->m_bHasText && ALIGN_TEXT_TOP(pme) ? Icon_GetImageOffset(pme) : 0)){
         pme->m_nScrollItem = nNewRow;
         bScroll = TRUE;
      }
   }
 
   if(bScroll)
      Icon_Draw(pme);   // Draw Entire Control
   else{
      // Just update the previous and current selection
      Menu_InitColors(pme, &clrBack, &clrText, &clrFrame);

      if (IS_ITEM_OD(pmi))
         Icon_DrawODItem(pme, pmi, &rc, FALSE);
      else
         Icon_DrawItem(pme, pmi, &rc, FALSE);
      
      if (IS_ITEM_OD(pmiNew))
         Icon_DrawODItem(pme, pmiNew, &rcNew, TRUE);
      else
         Icon_DrawItem(pme, pmiNew, &rcNew, TRUE);

      ResetColors(pme->m_pIDisplay, clrBack, clrText,clrFrame);
      IDISPLAY_Update(pme->m_pIDisplay);
   }

   if(nSel != nLast)
      Menu_NotifySelChange(pme);

   return(TRUE);
}

/*=====================================================================

Local Method - Calculates rect required to display nSel in grid view.

======================================================================*/
static void Icon_CalcGridRect(CMenuCtl * pme, AEERect * prc, int nSel)
{
   int   nRow;
   int   nCol,nFrame;

   nRow = (nSel / pme->m_nCols) - pme->m_nScrollItem;    // Item Row
   nCol = nSel % pme->m_nCols;                           // Item Col

   // Frame Size
   nFrame = (int)(pme->m_bHasText ? AEE_FRAME_SIZE : 0);

   // nSel rect
   SETAEERECT(prc,  pme->m_rc.x + nFrame + pme->m_xImagePad + (nCol * (pme->m_cxMaxItem + pme->m_xImagePad)),
                  pme->m_rc.y + nFrame + pme->m_yImagePad + (nRow * (pme->m_cyMaxItem + pme->m_yImagePad)),
                  pme->m_cxMaxItem, pme->m_cyMaxItem);


   // Adjust if text on top
   if(nFrame && ALIGN_TEXT_TOP(pme))
      prc->y += Icon_GetImageOffset(pme);
}

/*=====================================================================

Local Method - Returns the height of text for the icon menu.

======================================================================*/
static uint16 Icon_GetTextHeight(CMenuCtl * pme)
{
	uint16 cy = pme->m_cyFont;
	if(SINGLE_FRAME(pme))
		cy += (pme->m_style[0].yOffset * 2);
	return(cy);
}

/*=====================================================================

Local Method - Returns the height of tallest text as height may vary if
item text is owner drawn

======================================================================*/
static uint16 Icon_GetMaxTextHeight(CMenuCtl * pme)
{
   uint16 cyTextMax = 0;
   CMenuItem * pItem = pme->m_pItemList;

   if (OD_ITEMS_ALLOWED(pme))
   {
      while (pItem)
      {
         if (pItem->cyText > cyTextMax)
            cyTextMax = pItem->cyText;

         pItem = pItem->pNext;
      }
   }
   else
      cyTextMax = Icon_GetTextHeight(pme);

	return(cyTextMax);
}

/*=====================================================================

Local Method - Returns the item number that a point hit
If TRUE, pnItem is filled, otherwise not filled

======================================================================*/
static __inline boolean Icon_MapPointToItem(CMenuCtl * pme, int16 wXPos, int16 wYPos, int * pnItem, int * pnIdx)
{
   int nCol;
   int nRow;
   int nFrame  = (pme->m_bHasText ? AEE_FRAME_SIZE : 0);
   int nYAdj   = 0;
   CMenuItem *pItem;

   // Calculate offset of images clicked in!
   if( nFrame && ALIGN_TEXT_TOP(pme) ){
      nYAdj = Icon_GetImageOffset(pme);
   }
   // Estimate Row and Column
   nCol = (wXPos - pme->m_rc.x - nFrame - pme->m_xImagePad)/(pme->m_cxMaxItem + pme->m_xImagePad);
   nRow = ((wYPos - nYAdj) - pme->m_rc.y - nFrame - pme->m_yImagePad)/(pme->m_cyMaxItem + pme->m_yImagePad);
   // If the Row and Column are on screen AND it is below the top aligned text
   if( nCol < pme->m_nCols && nRow < pme->m_nRows && wYPos > nYAdj ){
      AEERect  rctItem;
      int      nItem;
      
      if (pme->m_bIconHScroll){
         /* When horizontal scroll is enabled, single row behaves like a circular queue.
          A row with 5 items and only 4 columns to display, might show any of the below
          data
           Index of Items Displayed: 1 2 3 4   OR    3 4 0 1  OR    0 1 2 3
           Items outside the display:   0               2              4
           
           User selected col:          0              2              2
           Corresponding item idx:     1              0              2
          pme->m_nScrollItem in this case holds the first displayed item's index. 1 in the 
          fist case above and 3, 0 in the rem'g cases respectively.
         */
         nItem = nCol + pme->m_nScrollItem;
         nItem %= pme->m_nItems; //Rollover if necessary

      }else{
         nItem = ((nRow + pme->m_nScrollItem) * pme->m_nCols)+ nCol;
         // Check the rectangle of the estimated item
         Icon_CalcGridRect(pme, &rctItem, nItem);
         if(!PT_IN_RECT(wXPos, wYPos, rctItem) )
            return FALSE;
      }

      pItem = GetItemByIdx(pme->m_pItemList, nItem);
      if( pItem ){
         *pnItem = pItem->nItemID;
         *pnIdx = nItem;
         return TRUE;
      }
   }
   return FALSE;
}

/*=====================================================================

Local Method - Turns off animation in an icon menu.

======================================================================*/
static void Icon_StopAnimation(CMenuCtl * pme)
{
   CMenuItem   *  pmi;
   boolean        bUnload;

   // Cancel our timers
   ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);

   bUnload = (boolean)(SINGLE_FRAME(pme) ? TRUE : FALSE);

   for(pmi = pme->m_pItemList; pmi != NULL ; pmi = pmi->pNext)
   {      
      if(pmi->pImage)
      {
         // Stop animation
         IIMAGE_Stop(pmi->pImage);

         // Unload the image if we are in single-frame mode and it was
         // not passed directly to me..
         if(bUnload && pmi->pszImage)
            aee_releaseobj((void **)&pmi->pImage);
      }
   }
}
   
/*=====================================================================

Local Method - Redraws an icon menu.

======================================================================*/
static boolean Icon_Draw(CMenuCtl * pme)
{
   IDisplay *     pd = pme->m_pIDisplay;
   CMenuItem *    pmi = NULL;

   // Stop animation
   Icon_StopAnimation(pme);

   // If it's a single-frame update and we want the update deferred, only update the text and
   // delay Icon_DrawAllItems. Otherwise immediately call Icon_DrawAllItems
   if(SINGLE_FRAME(pme) && DELAY_IMAGE(pme)){
      pmi = GetItemByIdx(pme->m_pItemList, pme->m_nSelect);
      if(pmi){
         if (IS_ITEM_OD(pmi))
            Icon_DrawODItemText(pme, pmi, 0, TRUE);
         else
            Icon_DrawItemText(pme, pmi, 0, TRUE);
         ISHELL_SetTimer(pme->m_pIShell, pme->m_wMenuImageDelay , (PFNNOTIFY)(Icon_DrawAllItems), pme);
         IDISPLAY_Update(pd);
      }      
   }
   else
      Icon_DrawAllItems(pme);
   return(TRUE);
}

/*=====================================================================

Local Method - Calculates Index of first and last displayable items in
a horizontal scroll icon view menu

======================================================================*/
static void Icon_CalcStartEndItemIndx(CMenuCtl* pme, int* pnStart, int* pnEnd)
{
   int nCount, nCols, nColsLeft, nColsRight, i, nEnd;

   nCount = pme->m_nItems;
   nCols = pme->m_nCols;

   // Get Index of first and last displayable item
   if(nCount <= nCols)
   {
      // Start Index
      i = 0;

      // End Index
        if(nCount == 0)
           nEnd = 0;
        else 
           nEnd = nCount - 1;
   }
   else if (pme->m_dwProps & MP_ICON_NO_CENTERSEL)
   {
      // Start Index
      i = pme->m_nScrollItem;

      // End Index
      nEnd = i + pme->m_nCols - 1;
      if(nEnd >= nCount)
         nEnd -= nCount;
   }
   else{
      // If we are displaying a single row and MP_ICON_NO_CENTERSEL is not sel, 
      // attempt to draw the current selection in the middle of the screen...

      // See what col we want to show the selected item in...

      nColsLeft = nColsRight = nCols / 2;

      // If it's not an odd boundary, we need to backup 1...
      if(!(nCols & 0x0001)){
         nColsLeft--;
         nColsRight++;
      }

      // Now we need to get the start and end indexes...

      i = pme->m_nSelect - nColsLeft;
      if(i < 0)
         i += nCount;

      nEnd = pme->m_nSelect + nColsRight;
      if(nEnd >= nCount)
         nEnd -= nCount;
   }
   *pnStart = i;
   *pnEnd = nEnd;
}

/*=====================================================================

Local Method - Calculates Item Rect

======================================================================*/
static void Icon_CalcItemRect(CMenuCtl * pme, uint16 nIdx, AEERect* prc)
{
   CMenuItem   *  pmiTemp = NULL;
   CMenuItem   *  pmi = NULL;
   AEERect        rc;
   int            i, xStart, xPad, yPad, nFrame, yOffset;

   // Get Item
   pmiTemp = GetItemByIdx(pme->m_pItemList, nIdx);

   rc = pme->m_rc;

   nFrame = 0;

   if(pme->m_bHasText){
      yOffset = Icon_GetImageOffset(pme);    // Text Height + Frame Size
      rc.dy -= Icon_GetMaxTextHeight(pme);   // Adjust control rect height by text height

      // For top aligned text, frame should appear underneath text
      if(ALIGN_TEXT_TOP(pme))
         rc.y += yOffset;
   }
   else{
      yOffset = 0;
   }			

   xPad = pme->m_xImagePad;
   yPad = pme->m_yImagePad;
   xStart = pme->m_rc.x + nFrame + xPad;

   SETAEERECT(&rc, xStart, pme->m_rc.y + nFrame + yPad, pme->m_cxMaxItem, pme->m_cyMaxItem);

   // Adjust for space taken by text
   if(nFrame && ALIGN_TEXT_TOP(pme))
      rc.y += yOffset;

   if(pme->m_bIconHScroll){
      int nEnd, nCount;

      nCount = pme->m_nItems;

      Icon_CalcStartEndItemIndx(pme, &i, &nEnd);

      // Update Draw Rect if item is displayable
      while(1){

         pmi = GetItemByIdx(pme->m_pItemList, i);

         if (pmi->nItemID == pmiTemp->nItemID)
         {
            *prc = rc;
            return;
         }

         rc.x += pme->m_cxMaxItem + xPad;
   
         if(i == nEnd)
            break;

         if(++i >= nCount)
            i = 0;
      }
   }
   else{
      int nRow, nCol;

      // Fast-forward to the right item row...

      i = pme->m_nCols * pme->m_nScrollItem;
      pmi = GetItemByIdx(pme->m_pItemList, i);

      for(nCol = nRow = 0; pmi != NULL; pmi = pmi->pNext, i++){

         if (pmi->nItemID == pmiTemp->nItemID)
         {
            *prc = rc;
            return;
         }
         
         // Update the x coordinate...
         rc.x += pme->m_cxMaxItem + xPad;
         nCol++;

         // If we are falling out, reset x and update y
         if(nCol >= pme->m_nCols){
            nCol = 0;
            rc.x = xStart;
            rc.y += pme->m_cyMaxItem + yPad;

            // If we are falling out here, break as we have checked all displayable items
            if(++nRow >= pme->m_nRows)
               break;
         }
      }
   }
}

/*=====================================================================

Local Method - Redraws all icon menu items.

======================================================================*/
static void Icon_DrawAllItems(CMenuCtl * pme)
{
   IDisplay *     pd = pme->m_pIDisplay;
   CMenuItem   *  pmi = NULL;
   AEERect        rc;
   int            i, xStart, xPad, yPad, nFrame, yOffset;
   RGBVAL         clrBack, clrText, clrFrame;

   // Initialize Colors
   Menu_InitColors(pme,&clrBack, &clrText, &clrFrame);

   // Fill control rect with CLR_USER_BACKGROUND
   IDISPLAY_FillRect(pd, &pme->m_rc, CLR_USER_BACKGROUND);

   rc = pme->m_rc;

   if(pme->m_bHasText){
        yOffset = Icon_GetImageOffset(pme);    // Text Height + Frame Size
      rc.dy -= Icon_GetMaxTextHeight(pme);   // Adjust control rect height by text height

      // For top aligned text, frame should appear underneath text
      if(ALIGN_TEXT_TOP(pme))
         rc.y += yOffset;

      // Draw Frame
      IDISPLAY_DrawFrame(pd, &rc, AEE_FT_BOX, CLR_USER_BACKGROUND);
      nFrame = AEE_FRAME_SIZE;
   }
   else{
        yOffset = 0;
      nFrame = 0;
    }           

   xPad = pme->m_xImagePad;
   yPad = pme->m_yImagePad;
   xStart = pme->m_rc.x + nFrame + xPad;

   SETAEERECT(&rc, xStart, pme->m_rc.y + nFrame + yPad, pme->m_cxMaxItem, pme->m_cyMaxItem);

   // Adjust for space taken by text
   if(nFrame && ALIGN_TEXT_TOP(pme))
      rc.y += yOffset;

   if(pme->m_bIconHScroll){
      int nEnd, nCount;

      nCount = pme->m_nItems;

      Icon_CalcStartEndItemIndx(pme, &i, &nEnd);

      // Draw all of the items...
      while(1){

         pmi = GetItemByIdx(pme->m_pItemList, i);

         if (pmi)
         {
            if (IS_ITEM_OD(pmi))
               Icon_DrawODItem(pme, pmi, &rc, (boolean)(pme->m_bActive && (i == pme->m_nSelect)));
            else
               Icon_DrawItem(pme, pmi, &rc, (boolean)(pme->m_bActive && (i == pme->m_nSelect)));

            rc.x += pme->m_cxMaxItem + xPad;
         }
   
         if(i == nEnd)
            break;

         if(++i >= nCount)
            i = 0;
      }
      // Restore background, text and frame colors
      ResetColors(pd, clrBack, clrText,clrFrame);
   }
   else{
      int nCol, nRow;

   // Fast-forward to the right item row...

      i = pme->m_nCols * pme->m_nScrollItem;
      pmi = GetItemByIdx(pme->m_pItemList, i);

      for(nCol = nRow = 0; pmi != NULL; pmi = pmi->pNext, i++){

         if (IS_ITEM_OD(pmi))
            Icon_DrawODItem(pme, pmi, &rc, (boolean)(pme->m_bActive && (i == pme->m_nSelect)));
         else
            Icon_DrawItem(pme, pmi,&rc,(boolean)(pme->m_bActive && (i == pme->m_nSelect)));


         // Update the x coordinate...
         rc.x += pme->m_cxMaxItem + xPad;
         nCol++;

         // If we are falling out, reset x and update y
         if(nCol >= pme->m_nCols){
            nCol = 0;
            rc.x = xStart;
            rc.y += pme->m_cyMaxItem + yPad;

            // If we are falling out here, break as we have displayed all
            // we could display
            if(++nRow >= pme->m_nRows)
               break;
         }
      }

      // Must reset the colors before drawing the scrollbar
      ResetColors(pd, clrBack, clrText,clrFrame);

      // Draw scroll bar
      Menu_DrawScrollBar(pme);
   }
   IDISPLAY_Update(pd);
}

/*=====================================================================

Local Method - Redraws an icon menu item.

======================================================================*/
static void Icon_DrawItem(CMenuCtl * pme, CMenuItem * pmi, AEERect * prc, boolean bSel)
{
   AEERect           rc;
   boolean           bSelDraw, bSelMark, bCached;
   AEERasterOp       ro;
   AEEItemStyle *    ps = NULL;
   IImage *          pi = NULL;
   IDisplay *        pd = pme->m_pIDisplay;
    uint16             cyText;

    if(!pmi)
        return;

   bSelMark = pme->m_bActive ? bSel : FALSE;

   pi = pmi->pImage;

    cyText = Icon_GetMaxTextHeight(pme);

   rc = *prc;
   ps = &pme->m_style[(bSelMark ? 1 : 0)];

   // If it's a single-frame icon view:
   //
   // - Draw the image UNSELECTED
   // - Draw the text SELECTED
   //

   // NOTE - Force reload of image if necessary...

   if(SINGLE_FRAME(pme)){

   // Must decrease the rect of the area by the text if necessary...

      if(pme->m_bHasText)
         rc.dy -= cyText;

      bSelDraw = FALSE;

      // Load Image & Calculate Size
      if(!pi && pmi->pszImage){
         pi = Menu_LoadImage(pme, pmi->pszImage, pmi->wImageID, &bCached);
         if(pi)
            Menu_SetupImage(pme, pmi, pi);
      }
   }
   else{
      bSelDraw = bSelMark;

      // Draw Frame
      IDISPLAY_DrawFrame(pd, &rc, ps->ft, (bSelMark ? pme->m_c.cSelBack : pme->m_c.cBack));

      // Adjust Rect for Style
      AdjustRect(&rc,ps);
   }

   if(pi){
      int      x, y, cx, cy;
      boolean  bLimit = FALSE;

      // Ensure image fits in. Limit the image size otherwise

      if(pmi->cxImage > rc.dx){
         cx = rc.dx;
         bLimit = TRUE;
      }
      else
         cx = pmi->cxImage;

      if(pmi->cyImage > rc.dy){
         cy = rc.dy;
         bLimit = TRUE;
      }
      else
         cy = pmi->cyImage;

      x = rc.x + ((rc.dx - cx) / 2);
      y = rc.y + ((rc.dy - cy) / 2);

      if(bLimit)
      {
         if( pme->m_dwOemProps & OEMMP_ICON_FIX_ITEMMAX )
         {
            IIMAGE_SetParm(pi, IPARM_SCALE, rc.dx, rc.dy);
            x = rc.x;
            y = rc.y;
         }
         else
            IIMAGE_SetParm(pi, IPARM_SIZE, cx, cy);
      }

      ro = ps->roImage;

      // Just copy if this isn't selected item or device is monochrome
      if(!bSelDraw && !pme->m_bColor)
         ro = AEE_RO_COPY;

        IIMAGE_SetParm(pi, IPARM_ROP, (int)ro, 0);

      // Make sure we reset the color of the background here.  This ensures that 
      // the image will be displayed in the correct color and will continue to do
      // so during animation...
      IDISPLAY_SetColor(pd, CLR_USER_BACKGROUND, (bSelDraw ? pme->m_c.cSelBack : pme->m_c.cBack));

      // Animate if image is animation and item is selected item. Draw otherwise.
      if(IS_ANIMATED(pme) && bSelMark)
         IIMAGE_Start(pi, x, y);
      else
         IIMAGE_Draw(pi, x ,y);
   } 

   // Draw Item Text
   Icon_DrawItemText(pme, pmi, cyText, bSel);
}

/*=====================================================================

Local Method - Requests draw for an owner drawn icon menu item.

======================================================================*/
static void Icon_DrawODItem(CMenuCtl * pme, CMenuItem * pmi, AEERect * prc, boolean bSel)
{
    uint16     cyText;
   AEEODCBData cbData;
   IDisplay * pd = pme->m_pIDisplay;

    if(!pmi)
        return;

   cbData.rc = *prc;
   bSel &= pme->m_bActive;

    cyText = Icon_GetMaxTextHeight(pme);

   if(SINGLE_FRAME(pme)){
      if(pme->m_bHasText)
         cbData.rc.dy -= cyText;
   }

   // Make sure we set the color of the background in display context
   IDISPLAY_SetColor(pd, CLR_USER_BACKGROUND, (bSel ? pme->m_c.cSelBack : pme->m_c.cBack));

   // Initialize callback for owner draw item        
   MEMSET(&cbData, 0, sizeof(AEEODCBData));

   cbData.nSize = sizeof(AEEODCBData);
   cbData.nAction = ODA_DRAW_ITEM;
   cbData.pIDisplay = pd;
   cbData.nActionInfo = pmi->nItemID;
   cbData.nActionInfo <<= 16;
   if (bSel)
      cbData.nActionInfo |= 0x00000001;

   // Draw Item 
   pme->m_pfnCB(pme->m_pvContext, &cbData);

   // Draw Item Text
   Icon_DrawODItemText(pme, pmi, cyText, bSel);
}

/*=====================================================================

Local Method - Redraws icon menu text.

======================================================================*/
static void Icon_DrawItemText(CMenuCtl * pme, CMenuItem * pmi, uint16 cyText, boolean bSel)
{
   AEERect        rc;
   IDisplay    *  pd = pme->m_pIDisplay;

   if(bSel && pme->m_bHasText){
      uint16 cyMaxText = Icon_GetMaxTextHeight(pme);

      if(!cyText)
          cyText = cyMaxText;

      SETAEERECT(&rc, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, cyText);

      // Adjust if text is at the bottom
      if(!ALIGN_TEXT_TOP(pme))
            rc.y += (pme->m_rc.dy - cyText);
   
      // We actually increment the size of the text by 1 pixel so that it hits the 
      // border of the box...
      rc.dy++;

      if(*pmi->text){

         uint16 adjust = (cyMaxText - Icon_GetTextHeight(pme))/2;

         // Adjust text rect for arrows
         if(!NO_ARROWS(pme) && pme->m_bIconHScroll){
            rc.x += (CXARROW + 1);        // Arrow + Space
            rc.dx -= 2*(CXARROW + 1);
         }

         // Set text/background color as SELECTED
         Menu_SetItemColor(pme, pd, TRUE,NULL);

         // Draw item text
         IDISPLAY_DrawText(pd, pmi->nFont, pmi->text, -1, rc.x, rc.y+adjust, &rc, IDF_RECT_FILL|IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);

         // Draw Arrows
         if(pme->m_bIconHScroll && !NO_ARROWS(pme))
            Menu_DrawArrows(pme, pme->m_rc.x, rc.y + ((rc.dy - CYARROW)/2), rc.x + rc.dx + 1);
      }
      else
         IDISPLAY_FillRect(pd, &rc, pme->m_c.cSelBack);  // Fill with selection background color
   }
}


/*=====================================================================

Local Method - Calls draw for owner drawn icon menu text.

======================================================================*/
static void Icon_DrawODItemText(CMenuCtl * pme, CMenuItem * pmi, uint16 cyText, boolean bSel)
{
   AEERect        rc;
   IDisplay    *  pd = pme->m_pIDisplay;

   if(bSel && pme->m_bHasText){

      AEEODCBData cbData;

      if(!cyText)
          cyText = Icon_GetMaxTextHeight(pme);

      SETAEERECT(&rc, pme->m_rc.x, pme->m_rc.y, pme->m_rc.dx, cyText);

      // Adjust if text is at the bottom
      if(!ALIGN_TEXT_TOP(pme))
            rc.y += (pme->m_rc.dy - cyText);
   
      // We actually increment the size of the text by 1 pixel so that it hits the 
      // border of the box...
      rc.dy++;

      // Adjust text rect for arrows
      if(!NO_ARROWS(pme) && pme->m_bIconHScroll){
         rc.x += (CXARROW + 1);        // Arrow + Space
         rc.dx -= 2*(CXARROW + 1);
      }

      // Set text/background color as SELECTED
      Menu_SetItemColor(pme, pd, TRUE,NULL);

      // Initialize cbData
      MEMSET(&cbData, 0, sizeof(AEEODCBData));

      cbData.nSize = sizeof(AEEODCBData);
      cbData.nAction = ODA_DRAW_ITEM_TEXT;
      cbData.pIDisplay = pd;
      cbData.rc = rc;
      cbData.nActionInfo = pmi->nItemID;
      cbData.nActionInfo <<= 16;
      if (bSel)
         cbData.nActionInfo |= 0x00000001;

      // Draw Item 
      pme->m_pfnCB(pme->m_pvContext, &cbData);

      // Draw Arrows
      if(pme->m_bIconHScroll && !NO_ARROWS(pme))
         Menu_DrawArrows(pme, pme->m_rc.x, rc.y + ((rc.dy - CYARROW)/2), rc.x + rc.dx + 1);
   }
}

/*=====================================================================

Local Method - Recalculates the number of rows in an icon menu.

======================================================================*/
static void Icon_CalcRows(CMenuCtl * pme)
{
   uint16   cxMax,cyMax;


   // Single Frame has one one row/col
   if(SINGLE_FRAME(pme)){
      pme->m_nRows = pme->m_nCols = 1;
      pme->m_bIconHScroll = TRUE;
      return;
   }

   // We need height of tallest item
   if(!pme->m_cyMaxItem)
       return;

   cyMax = pme->m_rc.dy;
   cxMax = pme->m_rc.dx;

   // If items have text - Adjust for frame and text
   if(pme->m_bHasText){
      cyMax -= (Icon_GetMaxTextHeight(pme) + (2*AEE_FRAME_SIZE));
      cxMax -= (AEE_FRAME_SIZE * 2);
      if(cyMax < pme->m_cyMaxItem)
        pme->m_cyMaxItem = cyMax;
      if(cxMax < pme->m_cxMaxItem)
        pme->m_cxMaxItem = cxMax;

   }

   // Number of displayable icon rows
   pme->m_nRows = cyMax / pme->m_cyMaxItem;

   // If there are more rows than displayable and displayable rows are
   // atleast 2, we show vertical scroll bar. Otherwise we set horizontal
   // scroll flag to indicate horizontal scrolling
   if(Icon_CalcCols(pme, cxMax) > pme->m_nRows && pme->m_nRows > 1){

      // Adjust available area for scroll bar width
      cxMax -= pme->m_nSBWidth;

      // Recalculate displayable cols and total rows
      Icon_CalcCols(pme, cxMax);

      pme->m_bIconVScroll = TRUE; // Vertical Scroll
   }
   else
   {
      pme->m_bIconHScroll =( (pme->m_nRows == 1 && pme->m_nTotalRows > 1)? TRUE : FALSE);
      //pme->m_bIconHScroll = (pme->m_nTotalRows/*m_nCols*/ == 1 ? FALSE : TRUE);   // Horizontal Scroll
   }
   
   // Calculate horizontal/vertical padding around icons
   pme->m_xImagePad = (cxMax - (pme->m_nCols * pme->m_cxMaxItem)) / (pme->m_nCols + 1);
   pme->m_yImagePad = (cyMax - (pme->m_nRows * pme->m_cyMaxItem)) / (pme->m_nRows + 1);

}

/*=====================================================================

Local Method - Recalculates the number of columns in an icon menu.

======================================================================*/
static int Icon_CalcCols(CMenuCtl * pme, int cx)
{
   int   nRows,nCols;

   // Need width of widest item
   if(! pme->m_cxMaxItem)
       return 0;

   // Number of displayable icon cols
   nCols = cx / pme->m_cxMaxItem;

   // There is atleast one col
   if(nCols == 0)
      nCols = 1;

   // Cache number of displayable icon cols
   pme->m_nCols = nCols;
   
   // Let's calculate total number of rows in this view
   nRows = pme->m_nItems / nCols;

   if(pme->m_nItems % nCols)
      nRows++;

   return((pme->m_nTotalRows = nRows));
}



/*===========================================================================


  Calendar Menu Routines


===========================================================================*/
/*=====================================================================

Local Method - Draws the time in a calendar menu view.

======================================================================*/
static void Cal_DrawTime(CMenuCtl * pme, IDisplay * pd, uint16 wTimeMin, int x, int yOrig, int y)
{  
   int16       wHour, wMins, wSecs, wHSecs;
   boolean     bPM;
   char        szTemp[12], *psz, ch;
   AEERect     rc;

   // Load small font image
   if(!pme->m_pFont){
       
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE) 
        IMAGE_WIDTH = pme->userSetStyle ? 4 : 5;
        pme->m_pFont = 
              pme->userSetStyle ?
              ISHELL_LoadResImage(pme->m_pIShell, AEECONTROLS_RES_FILE, AEE_IDB_SMALLFONTS):        
              ISHELL_LoadResImage(pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_NUMBER);
#else
        pme->m_pFont = ISHELL_LoadResImage(pme->m_pIShell, AEECONTROLS_RES_FILE, AEE_IDB_SMALLFONTS);
#endif
      if(!pme->m_pFont)
         return;
      IIMAGE_SetParm(pme->m_pFont, IPARM_CXFRAME, IMAGE_WIDTH, 0);
   }

   // get time string
   aee_MSECSToHM((uint32)(wTimeMin * 60l) * 1000l, &wHour, &wMins, &wSecs, &wHSecs, &bPM);
   SPRINTF(szTemp,"%02d:%02d",wHour,wMins);

   // indicate PM
   if(bPM){
      SETAEERECT(&rc, x, yOrig, 2, 2);
      IDISPLAY_FillRect(pd, &rc, CLR_USER_FRAME);
   }

   x++;
   
   for(psz = szTemp; (ch = *psz) != 0; psz++){
      if(ch == ':'){    // Draw :
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)     
            if( pme->userSetStyle)
            {
                SETAEERECT(&rc, x + 1, y, 2, 1);
                IDISPLAY_FillRect(pd, &rc, CLR_USER_TEXT);
                rc.y += 3;
                IDISPLAY_FillRect(pd, &rc, CLR_USER_TEXT);
            }
            else
            {
                IIMAGE_DrawFrame(pme->m_pFont, 10, x, y);
            }
#else
            SETAEERECT(&rc, x + 1, y, 2, 1);
            IDISPLAY_FillRect(pd, &rc, CLR_USER_TEXT);
            rc.y += 3;
            IDISPLAY_FillRect(pd, &rc, CLR_USER_TEXT);
#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      }
      else{             // Draw Time
         if(ch != '0' || psz != szTemp)
            IIMAGE_DrawFrame(pme->m_pFont, (int)(ch - '0'), x, y);
      }

      x += IMAGE_WIDTH;
   }
}

/*=====================================================================

Local Method - Resets the calendar menu.

======================================================================*/
static void Cal_Reset(CMenuCtl * pme)
{
   pme->m_dwProps &= ~(MP_CALENDAR|MP_AUTOSCROLLTIME);

   pme->m_yLastTimeMark = INVALID_MARK;

   ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);

   aee_releaseobj((void **)&pme->m_pFont);
}

/*=====================================================================

Local Method - Recalculates the page layout of a cal menu. Updates m_nPageItems
with visible calendar menu items.

======================================================================*/
static void Cal_CalcPageItems(CMenuCtl * pme)
{
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
    if(IS_CALENDAR_VIEW(pme))
    {
        CMenuItem   *pp     = GetItemByIdx(pme->m_pItemList, pme->m_nScrollItem);
        uint16      cyLeft  = pme->m_rc.dy;
        uint16      cyFont  = pme->m_cyFont;

        if(pme->m_pTitle || IS_TITLE_OD(pme))
        {
            cyLeft -= pme->m_rcTitle.dy;
        }
        if( !pme->userSetStyle && pme->bottomBarType != BTBAR_NONE)
        {
            cyLeft -= GetBottomBarHeight( pme->m_pIDisplay);
        }
            
        pme->m_nPageItems = 0;
        for(; pp && cyLeft > 0; pp = pp->pNext)
        {
            if(GetItemHeight(pp, &cyLeft, (uint16)cyFont))
            {
                pme->m_nPageItems ++;
            }
        }
    }          
#else //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   CMenuItem * pp = GetItemByIdx(pme->m_pItemList, pme->m_nScrollItem);
   uint16         cyLeft,cyFont,nFont;
   
   // Check if no menu item is present
   if (pp != NULL ) {
      // Use the MenuItem font instead of default MenuCtl font
      nFont = IDISPLAY_GetFontMetrics(pme->m_pIDisplay, pp->nFont, NULL, NULL);	
      // use pme->m_cyFont if EFAILED is returned by IDISPLAY_GetFontMetrics
      if (nFont == EFAILED) {
         nFont = pme->m_cyFont;
      }
   } else {
      nFont = pme->m_cyFont;
   }

   if(IS_CALENDAR_VIEW(pme)){
      cyLeft = pme->m_rc.dy;
      cyFont = pme->m_cyFont;

      if(pme->m_pTitle || IS_TITLE_OD(pme))
         cyLeft -= pme->m_rcTitle.dy;

      pme->m_nPageItems = 0;
      for (; pp && cyLeft > 0; pp = pp->pNext){
         if(GetItemHeight(pp, &cyLeft, (uint16)cyFont))
            pme->m_nPageItems++;
      }
   }
#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
}

/*=====================================================================

Local Method - Timer callback to auto-scroll a calendar menu based on time.

======================================================================*/
static void Cal_AutoScrollCB(CMenuCtl * pme)
{
   Cal_AutoScroll(pme, FALSE);
}

/*=====================================================================

Local Method - Auto-scrolls a calendar menu based on time.

======================================================================*/
static void Cal_AutoScroll(CMenuCtl * pme, boolean bForceSel)
{
   CMenuItem *    pi;
   uint16         i, wMinNow, wMins;

   // Don't scroll if told not to do so
   if(!AUTO_SCROLL(pme))
      return;

   // Find item corresponding to the current time
   pi = Cal_FindNowItem(pme, &wMinNow, &i);

   // No item corresponding to current time
   // If its a forced selection, draw again. Get out nevertheless.
   if(!pi){
      if(bForceSel)
         Menu_Draw(pme);
      return;
   }

   if(bForceSel || (i != pme->m_nSelect && !ITEM_IS_VISIBLE(pme, i))){
      pme->m_nSelect = pme->m_nScrollItem = i;
      Menu_Draw(pme);
   }

   // Show the time marker...
   wMins = Cal_ShowTimeMark(pme, pi, i, wMinNow);

   // Update as soon as minutes corresponding to one pixel size have lapsed
   if(wMins)
      ISHELL_SetTimer(pme->m_pIShell, (uint32)(wMins * (60l * 1000l)), (PFNNOTIFY)(Cal_AutoScrollCB), pme);
}

/*=====================================================================

Local Method - Returns the item closest to the current time.

======================================================================*/
static CMenuItem * Cal_FindNowItem(CMenuCtl * pme, uint16 * pwNow, uint16 * pwIdx)
{
   CMenuItem *    pi;
   uint16         i, wMinNow;
   JulianType     jdate;

   GETJULIANDATE(GETTIMESECONDS(), &jdate);
   wMinNow = (jdate.wHour * 60) + jdate.wMinute;

   for(i = 0; (pi = GetItemByIdx(pme->m_pItemList, i)) != NULL; i++){
      if(wMinNow >= pi->wCalStart && wMinNow < pi->wCalEnd)
         break;
   }
   *pwIdx = i;
   *pwNow = wMinNow;
   return(pi);
}

/*=====================================================================

Local Method - Draws the arrows in a calendar menu.

======================================================================*/
static void Cal_DrawArrow(CMenuCtl * pme, int y)
{
   IDisplay *  pd = pme->m_pIDisplay;
   AEERect       rc;
   int         xInc, i;

   SETAEERECT(&rc, (pme->m_rc.x + pme->m_rc.dx) - 2, y - (ARROW_LINES / 2), 1, 1);

   // Adjust x for scroll bar
   if (pme->m_nItems > pme->m_nPageItems) 
      rc.x -= pme->m_nSBWidth;

   xInc = -1;

   for(i = 0; i < ARROW_LINES; i++){

      if(rc.y >= pme->m_rc.y && rc.y < pme->m_rc.y + pme->m_rc.dy)
         IDISPLAY_InvertRect(pd, &rc);

      rc.y++;

      // Invert at center point
      if(i == ARROW_LINES / 2)
         xInc = -xInc;

      rc.dx -= xInc;
      rc.x += xInc;
   }
}

/*=====================================================================

Local Method - Shows the time marker in a calendar menu.

======================================================================*/
static uint16 Cal_ShowTimeMark(CMenuCtl * pme, CMenuItem * pi, uint16 i, uint16 wMinNow)
{
   uint16         wRange, wMinPerPixel, wCenter;
   IDisplay *     pd = pme->m_pIDisplay;
   int            y;

   // Find the entry in the list duration of which spans current time

   if(!pi && (pi = Cal_FindNowItem(pme, &wMinNow, &i)) == NULL)
      return(0);

   // Erase previous mark
   if(pme->m_yLastTimeMark != INVALID_MARK)
      Cal_DrawArrow(pme, pme->m_yLastTimeMark);

   wMinPerPixel = 0;

   if(pi->cyImage && ITEM_IS_VISIBLE(pme, i)){  // Draw new mark

      wRange = pi->wCalEnd - pi->wCalStart;  // Range of time in item...

      wMinPerPixel = wRange / pi->cyImage;

      if(!wMinPerPixel)
         wMinPerPixel++;

      wCenter = (wMinNow - pi->wCalStart) / wMinPerPixel;

      y = (int)(pi->y + (wCenter - 1));
      Cal_DrawArrow(pme, y);
   }
   else{
      if(i < pme->m_nScrollItem)
         wMinPerPixel = 15;      

      y = INVALID_MARK;
   }

   pme->m_yLastTimeMark = y;

   IDISPLAY_Update(pd);

   return(wMinPerPixel);
}

/*=====================================================================

Local Method - Draws a caledar menu item.

======================================================================*/
static void Cal_DrawItem(CMenuCtl * pme, CMenuItem * pi, int nIdx, boolean bClear)
{
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   IDisplay *  pd = pme->m_pIDisplay;
   AEERect     rc;
   int         cx,cy;
   boolean     bActive = (boolean)(nIdx == pme->m_nSelect);

   Menu_SetItemColor(pme,pd,bActive,NULL);

   cx = pme->m_rc.dx;
   cy = pi->cyImage;

   if (pme->m_nItems > pme->m_nPageItems) 
      cx -= pme->m_nSBWidth;

   SETAEERECT(&rc, pme->m_rc.x, pi->y, cx, cy);

#if defined( FEATURE_CONTROL_BG_USE_IMAGE)
   if(bClear && pme->userSetStyle)
      IDISPLAY_FillRect(pd, &rc, RGB_NONE);
   else if( bClear)
       drawControlBg( pd, &rc);
#else //#if defined( FEATURE_CONTROL_BG_USE_IMAGE)
   if(bClear)
      IDISPLAY_FillRect(pd, &rc, pme->userSetStyle ? RGB_NONE : RGB_BLACK);
   
   if( !pme->userSetStyle)
   {
       
      rc.dx = IMAGE_WIDTH * 6;
      IDISPLAY_FillRect( pd, &rc, pme->m_c.cTitle);
      rc.dx = cx; 
   }
#endif //#if defined( FEATURE_CONTROL_BG_USE_IMAGE)
   
  if( bActive && !pme->userSetStyle)
  {
      

      rc.x  += IMAGE_WIDTH * 6;
      rc.dx -= IMAGE_WIDTH * 6;
 
      Menu_DrawSelectBar( pme, &rc, pme->m_style[1].ft, pi->cyImage);
      rc.x  -= IMAGE_WIDTH * 6;
      rc.dx = cx;  
  }

  if( pme->userSetStyle && !bActive)
  {
      
      if(nIdx > pme->m_nScrollItem){
         rc.y--;
         rc.dy++;
      }
      IDISPLAY_FrameRect(pd, &rc);
      rc.y = pi->y;
      rc.dy = cy;      
  }
  else
  {
       
      RGBVAL oldFrameColor = IDISPLAY_SetColor( pd, 
              CLR_USER_FRAME, 
              RGB_WHITE);
      
      if(nIdx > pme->m_nScrollItem)
      {
          
         rc.y--;
         rc.dy++;
      }
      IDISPLAY_FrameRect( pd, &rc);
      rc.y = pi->y;
      rc.dy = cy;
      IDISPLAY_SetColor( pd, CLR_USER_FRAME, oldFrameColor);
   }
      
   // Draw Time
   Cal_DrawTime(pme, pd, pi->wCalStart, rc.x , rc.y, rc.y + ((cy - IMAGE_WIDTH)/2));

   // If this is active, invert time string
   if( pme->userSetStyle && bActive){
      rc.dx = IMAGE_WIDTH * 6;
      IDISPLAY_InvertRect(pd, &rc);
      rc.dx = cx;
   }
   
   // Adjust for tims string

   rc.x  += IMAGE_WIDTH * 6;
   rc.dx -= IMAGE_WIDTH * 6;


   // If not active, ensure that text doesn't touch frame
   if(!bActive){
      rc.x++;
      rc.y++;
      rc.dx -= 2;
      rc.dy -= 2;
   }

   // Draw Text
   IDISPLAY_DrawText(pd, 
           pi->nFont,
           pi->text,
           -1,
           rc.x,
           rc.y,
           &rc,
           IDF_ALIGN_MIDDLE | ( pme->userSetStyle ? IDF_RECT_FILL : IDF_TEXT_TRANSPARENT) | ParagraphAlignment(pi->text, WSTRLEN(pi->text))
    );
#else //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
   IDisplay *  pd = pme->m_pIDisplay;
   AEERect     rc;
   int         cx,cy;
   boolean     bActive = (boolean)(nIdx == pme->m_nSelect);

   Menu_SetItemColor(pme,pd,bActive,NULL);

   cx = pme->m_rc.dx;
   cy = pi->cyImage;

   if (pme->m_nItems > pme->m_nPageItems) 
      cx -= pme->m_nSBWidth;

   SETAEERECT(&rc, pme->m_rc.x, pi->y, cx, cy);

   if(bClear)
      IDISPLAY_FillRect(pd, &rc, RGB_NONE);
   
   // If this isn't active, draw a frame around
   if(!bActive){
      if(nIdx > pme->m_nScrollItem){
         rc.y--;
         rc.dy++;
      }
      IDISPLAY_FrameRect(pd, &rc);
      rc.y = pi->y;
      rc.dy = cy;
   }

   // WARNING - This will not work properly if the colors are inverted!!!!

   // Draw Time
   Cal_DrawTime(pme, pd, pi->wCalStart, rc.x , rc.y, rc.y + ((cy - IMAGE_WIDTH)/2));

   // If this is active, invert time string
   if(bActive){
      rc.dx = IMAGE_WIDTH * 6;
      IDISPLAY_InvertRect(pd, &rc);
      rc.dx = cx;
   }

   // Adjust for tims string

   rc.x  += IMAGE_WIDTH * 6;
   rc.dx -= IMAGE_WIDTH * 6;


   // If not active, ensure that text doesn't touch frame
   if(!bActive){
      rc.x++;
      rc.y++;
      rc.dx -= 2;
      rc.dy -= 2;
   }

   // Draw Text
   IDISPLAY_DrawText(pd, pi->nFont, pi->text, -1, rc.x, rc.y, &rc,IDF_RECT_FILL|IDF_ALIGN_MIDDLE|ParagraphAlignment(pi->text, WSTRLEN(pi->text)));
#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
}


/*===========================================================================





                 UTILITY ROUTINES






===========================================================================*/
/*=====================================================================

Local Method - Returns the item based upon the item ID.

======================================================================*/
static CMenuItem * GetItemByID(CMenuItem * pl, int nID, CMenuItem ** pLast)
{
   if(pLast)
      *pLast = NULL;
   for(; pl; pl = pl->pNext){
      if(pl->nItemID == nID)
         return(pl);
      if(pLast)
         *pLast = pl;
   }
   return(NULL);
}

/*=====================================================================

Local Method - Returns the item and its index based upon the item ID

======================================================================*/
static CMenuItem * GetItemByIDEx(CMenuItem * pl, int nID, CMenuItem ** pLast, int* pIndex)
{
   if(pLast)
      *pLast = NULL;
   if (pIndex)
      *pIndex = 0;
   for(; pl; pl = pl->pNext){
      if(pl->nItemID == nID)
         return(pl);
      if(pLast)
         *pLast = pl;
      if(pIndex)
         (*pIndex)++;
   }
   return(NULL);
}

/*=====================================================================

Local Method - Returns TRUE if the x/y intersects the rectangle.

======================================================================*/
static boolean Intersects(AEERect * prc, int x, int y)
{
   return(x >= prc->x && x < prc->x + prc->dx && y >= prc->y && y < prc->y + prc->dy);
}


/*=====================================================================

Local Method - Returns the item based upon the index value.

======================================================================*/
static CMenuItem * GetItemByIdx(CMenuItem * pl, int nIdx)
{
   int   i;

   for(i = 0; pl; pl = pl->pNext, i++){
      if(i == nIdx)
         return(pl);
   }

   return(NULL);
}

/*=====================================================================

Local Method - Resets the display colors.

======================================================================*/
static void ResetColors(IDisplay * pd, RGBVAL clrBack, RGBVAL clrText,RGBVAL clrFrame)
{
   IDISPLAY_SetColor(pd, CLR_USER_BACKGROUND, clrBack);
   IDISPLAY_SetColor(pd, CLR_USER_TEXT, clrText);
   IDISPLAY_SetColor(pd, CLR_USER_FRAME, clrFrame);
}


/*=====================================================================

Local Method - Returns the calendar menu item height in a menu. Also 
updates pwLeft with the space taken in drwaing specified item.

======================================================================*/
static uint16 GetItemHeight(CMenuItem * p, uint16 * pwLeft, uint16 wLine)
{
   uint16   wh,wDur,wl;

   wl = *pwLeft;

   wDur = p->wCalEnd - p->wCalStart;

   wh = wLine;

   if(wDur <= 30 && !p->text[0] && wl >= wLine + (IMAGE_WIDTH + 2))
      wh = IMAGE_WIDTH + 6;
   else{
      if(wDur >= 120)
         wh += wLine;
   }
   
   if(wh > wl){
      wh = wl;
      wl = 0;
   }
   else{
      wl -= wh;
      if(wl < wLine){
         wh += wl;
         wl = 0;
      }
   }

   *pwLeft = wl;
   return(wh);
}

/*=====================================================================

Local Method - Adjusts the size of the rect based upon the style.

 这里一般用来调整item字符串的显示矩形框
======================================================================*/
static void AdjustRect(AEERect * prc, AEEItemStyle * ps)
{
   prc->x += ps->xOffset;
   /* 因为显示字体总在字体下方空余一个像素，为了显示美观，
    总是在纵坐标方向多向下偏移一个像素使之看起来更加居中*/
   prc->y += (ps->yOffset);
   prc->dx -= (ps->xOffset * 2);
   prc->dy -= (ps->yOffset * 2);
}

/*=====================================================================

Local Method - Returns the width of a menu item

======================================================================*/
static int GetItemWidth(CMenuItem * pi)
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


#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
static void IMenuCtl_SetBottomBarType( IMenuCtl *po, int bottomBar_e_Type)
{
    CMenuCtl* pme = (CMenuCtl*)po;
    
    pme->bottomBarType = bottomBar_e_Type;
    Menu_Recalc( pme);
    
    // 2008.03.18
    if( !ITEM_IS_VISIBLE( pme, pme->m_nSelect))
    {

        int nDir = 0;

        if( pme->m_nSelect < pme->m_nScrollItem)
        {
            nDir = pme->m_nSelect;
        }
        else
        {
            nDir = ( pme->m_nSelect + 1) - pme->m_nPageItems;
            if( nDir < 0)
            {
                nDir = 0;
            }
        }
        pme->m_nScrollItem = (uint16)nDir;
    }
}

/*=====================================================================

Local Method - Set pop menu rect

======================================================================*/
static void IMenuCtl_SetPopMenuRect( IMenuCtl *po)
{
    CMenuCtl *pme = (CMenuCtl*)po;
    AEEDeviceInfo   devinfo;
    IImage                *BgImg;
    int                       cy,cyMax,cxMax,cys,cxs,nFrame;

    //Draw shadow for screen
    BgImg = ISHELL_LoadResImage(pme->m_pIShell,
                            AEE_APPSCOMMONRES_IMAGESFILE,
                            IDB_BACKGROUND);
    if(BgImg != NULL)
    {                  

        int             x    = 0;
        int             y    = 0;
        AEEImageInfo    ii   = {0};
        
#if defined( AEE_SIMULATOR)
        AEEDeviceInfo dm = {0};
        ISHELL_GetDeviceInfo(pme->m_pIShell, &dm);
        SETAEERECT( &pme->m_rc, 0, 0, dm.cxScreen, dm.cyScreen);
#endif
        IIMAGE_GetInfo( BgImg, &ii);
        IIMAGE_Draw(BgImg,x,y);
/*
        while( x < pme->m_rc.dx)
        {

            y = 0;
            while( y < pme->m_rc.dy)
            {
                IIMAGE_Draw( BgImg, x, y);
                ERR("DRAW BG IMG SUCCESSFUL",0,0,0);
                y += ii.cy;
            }
            x += ii.cx;
        }
*/   

        IIMAGE_Release(BgImg);
        BgImg = NULL;
    }
    else
    {
        return;
    }    

    pme->SetPopMenu = TRUE;
    
    MEMSET(&devinfo, 0, sizeof(devinfo));
    ISHELL_GetDeviceInfo(pme->m_pIShell, &devinfo);    
    
    // Get extra pixels added to height and width because of style
    Menu_GetStyleOffsets(pme, &cxs, &cys);
    
    nFrame = (uint16)(AEE_FRAME_SIZE * 2);   
    cxMax = devinfo.cxScreen;     // Max pop menu width
    //cyMax = devinfo.cyScreen*3/4 - cys - nFrame;     // Max pop menu  height
    cyMax = (pme->m_cyFont+ cys)*MAX_NUM_MENUPOP + GetBottomBarHeight(pme->m_pIDisplay) + AEE_FRAME_SIZE*10;

    cy = (pme->m_cyFont + cys)* (pme->m_nItems) + GetBottomBarHeight( pme->m_pIDisplay) + AEE_FRAME_SIZE*(pme->m_nItems + 4); 
    if(cy > cyMax)
    {
        cy  = cyMax;
    }
    pme->m_menuReallyDy = cy;//add by xuhui
    //Set pop menu rect
    pme->m_rc.dx = cxMax;   //modi by yangdecai 2010-07-30
    pme->m_rc.dy = (pme->m_cyFont+ cys)*MAX_NUM_MENUPOP + GetBottomBarHeight(pme->m_pIDisplay)+AEE_FRAME_SIZE*4;
    pme->m_rc.x = 0;
    pme->m_rc.y = 0;//devinfo.cyScreen - pme->m_rc.dy;
    

    {
        uint32 wMask = IMENUCTL_GetOemProperties(po);

        IMENUCTL_SetOemProperties(po, wMask |OEMMP_USE_MENU_STYLE);
    }
    Menu_Recalc(pme);
}
#endif

// Public Method - Sets the OEM properties (flags) for the menu.
static void IMenuCtl_SetOemProperties(IMenuCtl * po, uint32 nProperties)
{
    CMenuCtl *pme = (CMenuCtl*)po;
    
    pme->m_dwOemProps = nProperties;
}
   
// Public Method - Returns the active OEM properties for the menu.
static uint32 IMenuCtl_GetOemProperties(IMenuCtl * po)
{
    return(((CMenuCtl *)po)->m_dwOemProps);
}

/*=====================================================================

Set last item's select value(TRUE or FALSE)

======================================================================*/
static void IMenuCtl_SetLastItemSel(IMenuCtl *po, boolean bSelected)
{
    CMenuCtl *pme = (CMenuCtl*)po;
    
    pme->m_pLastItem->bIsSel = bSelected;
}

#ifdef FEATURE_CUSTOMIZED_MENU_STYLE
/*=====================================================================

Set menu background image

======================================================================*/
static void IMenuCtl_SetBackGround(IMenuCtl *po, char *pstrImgResFile, uint16 nImgResID)
{
    CMenuCtl *pme = (CMenuCtl*)po;

    if(pstrImgResFile != NULL && STRLEN(pstrImgResFile) != 0 && nImgResID != 0)
    {
        pme->nBgImgResID = nImgResID;
        MEMSET(pme->strBgImgResFile, 0, sizeof(pme->strBgImgResFile));
        STRCPY(pme->strBgImgResFile, pstrImgResFile);
        if(pme->m_pBgImage != NULL)
        {
            IImage_Release(pme->m_pBgImage);
            pme->m_pBgImage = NULL;
        }
        pme->m_pBgImage = ISHELL_LoadResImage(pme->m_pIShell,
                           pme->strBgImgResFile,
                           pme->nBgImgResID);
    }
}
#endif
/*=====================================================================

Draw menu background image

======================================================================*/
static void Menu_DrawBackGround(CMenuCtl * pme, AEERect *pRect)
{
#ifdef FEATURE_RANDOM_MENU_COLOR
	#if 0
    if(pme->m_nRandomMenu != 0)
    {
        if(pme->m_pBgImage == NULL)
        {
            pme->m_pBgImage = ISHELL_LoadResImage(pme->m_pIShell,
                                   AEE_APPSCOMMONRES_IMAGESFILE,
                                   IDI_MENU_BACKGROUND_TRANS);
        }
        IDISPLAY_FillRect( pme->m_pIDisplay, pRect, pme->m_c.cBack);
        Appscommon_ResetBackground(pme->m_pIDisplay, pme->m_pBgImage, pme->m_c.cBack, pRect, APPSCOMMON_MENUBG_XPOS, APPSCOMMON_MENUBG_YPOS);
    }
    else
    #endif
#endif 
    {
       if(pme->m_pBgImage == NULL)
       {
            if(STRLEN(pme->strBgImgResFile) != 0 && pme->nBgImgResID != 0)
            {
                pme->m_pBgImage = ISHELL_LoadResImage(pme->m_pIShell,
                                   pme->strBgImgResFile,
                                   pme->nBgImgResID);
            }
            else
            {
                pme->m_pBgImage = ISHELL_LoadResImage(pme->m_pIShell,
                                   AEE_APPSCOMMONRES_IMAGESFILE,
                                   IDB_BACKGROUND);  //modi by yangdecai
            }
       }
       Appscommon_ResetBackground(pme->m_pIDisplay, pme->m_pBgImage, pme->m_c.cBack, pRect, 0, 0);
    }
}
#ifdef FEATURE_MENUTITLE_AUTOSCROLL
/*=====================================================================

Local Method - Draws a standard menu title

======================================================================*/
static void Menu_DrawTitle(CMenuCtl * pme, uint16 wChars)
{
    IDisplay *  pd = pme->m_pIDisplay;
    int         x;
    int         y;
    AEERect     qrc;
    uint16      cyLeft;
    AECHAR     *pTitle;

    AEERect     TextRect;

    if(!pme->m_pTitle)
    {
        return;
    }
    
    pTitle = &pme->m_pTitle[wChars];
    x = pme->m_rc.x;
    y = pme->m_rc.y;
    //cyFont = pme->m_cyFont;
    cyLeft = pme->m_rc.dy;

 
    SETAEERECT(&qrc, x, y, pme->m_rc.dx, pme->m_rcTitle.dy);//cyFont);
    TextRect = qrc;

#ifdef FEATURE_LANG_BIDI
    if(ParagraphAlignment(pme->m_pTitle, WSTRLEN(pme->m_pTitle))&IDF_ALIGN_RIGHT)
    {
        TextRect.dx -= MENUTEXT_XOFFSET;//a little leading
    }
    else
#endif /*FEATURE_LANG_BIDI*/
    {
        TextRect.x  += MENUTEXT_XOFFSET;//a little leading
        TextRect.dx -= MENUTEXT_XOFFSET;//a little leading
    }

	//wlh 20090410 mod 屏蔽下面一句代码
    //IDISPLAY_FillRect(pme->m_pIDisplay, &TextRect, MAKE_RGB(0, 0, 0));
	//IDISPLAY_FillRect(pme->m_pIDisplay, &TextRect, MAKE_RGB(20, 47, 91));
 
 #if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
        if( pme->userSetStyle)
        {
            IDISPLAY_DrawText( pd, 
                  AEE_FONT_NORMAL, 
                  pTitle, 
                  -1, 
                  x, 
                  y, 
                  &TextRect, 
                  //IDF_RECT_FILL | ParagraphAlignment(pme->m_pTitle, WSTRLEN(pme->m_pTitle))
                  IDF_ALIGN_MIDDLE | IDF_RECT_FILL | ParagraphAlignment(pme->m_pTitle, WSTRLEN(pme->m_pTitle))
              );
        }
        else
        {
            TitleBar_Param_type titleBarParms;
            MEMSET( &titleBarParms, 0, sizeof( TitleBar_Param_type));
            
            //titleBarParms.dwAlignFlags  = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER;
            titleBarParms.dwAlignFlags  = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
            titleBarParms.pwszTitle     = pTitle;
            titleBarParms.prc           = &TextRect;
            
            if(pme->m_dwOemProps & OEMMP_ARROW_TITLE)
            {
                titleBarParms.eTBarType = TBAR_ARROW;
            }         
            DrawTitleBar( pd, &titleBarParms);
        }
#else
	    IDISPLAY_DrawText(pd, AEE_FONT_NORMAL, pTitle, -1, x, y, &TextRect, IDF_RECT_FILL | ParagraphAlignment(pme->m_pTitle, WSTRLEN(pme->m_pTitle)));
#endif //#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)

#if 0
    if(UNDERLINE(pme)){
        qrc.y = qrc.dy - 1;
        qrc.dy = 1;
        IDISPLAY_FillRect(pme->m_pIDisplay, &qrc, CLR_USER_TEXT);
    }//UNDERLINE(pme)
#endif
}// Menu_DrawTitle

/*=====================================================================

Local Method - Determines whether a menu item scrolls

======================================================================*/
static boolean Menu_TitleScrolls(CMenuCtl * pme, int nIdx)
{
    int   cx,cxMax;

    cx = IDISPLAY_MeasureText(pme->m_pIDisplay, AEE_FONT_BOLD, pme->m_pTitle + nIdx);
   
    cxMax = pme->m_rc.dx;    
    
    return(cx > cxMax);
}// Menu_TitleScrolls

/*=====================================================================

Local Method - Auto-scroll logic for standard menu title.

======================================================================*/
static void Menu_TitleAutoScroll(CMenuCtl * pme)
{
    int         n,nIdxNew,nIdx;
    RGBVAL      clrBack,clrText,clrFrame;

    if(!pme->m_pTitle)
    {
        return;
    }
    
    if(!pme->m_bTitleAutoScroll)
    {
        return;
    }
    
    // See if the index is within range for the string...
    nIdx  = nIdxNew = pme->m_nTitleAutoScrollIdx;
    n     = WSTRLEN(pme->m_pTitle);

    if(nIdx < n)
    {
        // Measure the text starting from the auto-scroll offset
        if(Menu_TitleScrolls(pme, nIdx))
        {
            nIdxNew++;
        }
        else
        {
            nIdxNew = 0;
        }
        
        Menu_InitColors(pme, &clrBack, &clrText, &clrFrame);
        Menu_DrawTitle(pme, (uint16)nIdx);
        ResetColors(pme->m_pIDisplay, clrBack, clrText,clrFrame);
        IDISPLAY_Update(pme->m_pIDisplay);
    }
    else
    {
        nIdxNew = 0;
    }

    pme->m_nTitleAutoScrollIdx = nIdxNew;

    ISHELL_SetTimer( pme->m_pIShell,
                    (uint32)(nIdx && nIdxNew ? AUTO_SCROLL_TIME : AUTO_SCROLL_START),
                    (PFNNOTIFY)(Menu_TitleAutoScroll), 
                    pme);
}// Menu_TitleAutoScroll

/*=====================================================================

Local Method - Resets the auto-scroll timer for a menu title.

======================================================================*/
static void Menu_ResetTitleAutoScroll(CMenuCtl * pme)
{
    if(!pme->m_pTitle)
    {
        return;
    }
    
    pme->m_nTitleAutoScrollIdx = 0;
    
    if( pme->m_bActive && Menu_TitleScrolls(pme, 0))
    {
        pme->m_bTitleAutoScroll = TRUE;

    // We set the timer to go off pretty quickly.  That's because the AutoScroll function
    // will automatically set a longer timer for the first and last characters...

        ISHELL_SetTimer(pme->m_pIShell, AUTO_SCROLL_TIME, (PFNNOTIFY)(Menu_TitleAutoScroll), pme);
    }
    else
    {
        ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)(Menu_TitleAutoScroll), pme);
        pme->m_bTitleAutoScroll = FALSE;
    }
}// Menu_ResetTitleAutoScroll
#endif //FEATURE_MENUTITLE_AUTOSCROLL

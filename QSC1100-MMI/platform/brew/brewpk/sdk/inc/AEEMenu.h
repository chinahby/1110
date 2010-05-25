#ifndef AEEMENU_H
#define AEEMENU_H
/*======================================================
FILE:  AEEMenu.h

SERVICES:  AEE Menu Interface

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEEMenu

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEE.h"
#include "AEEShell.h"
#ifdef CUST_EDITION	
#include "OEMFeatures.h"
#endif /*CUST_EDITION*/

typedef struct _IMenuCtl      IMenuCtl;

//************************************************************************************************
//
// IMenuCtl Interface
//
//************************************************************************************************


// Menu properties
#ifdef CUST_EDITION	
// add MP_BIND_ITEM_TO_KEY @08.01.15
#define MP_BIND_ITEM_TO_NUMBER_KEY   0x00000100  // bind item to number key 0 - 9

#define MP_TRANSPARENT_UNSEL 0x00001000

//draw icon align right but the text align left
#define MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT    0x00000400
#endif /*CUST_EDITION*/
#define MP_WRAPSCROLL         0x00010000  // Menu ONLY (softkey/icons - auto ON, supported but default OFF for MenuCtl)
#define MP_NO_ARROWS          0x00020000  // No arrows even if scroll is possible (IconView)
#define MP_ALLOW_DUPE_IDS     0x00040000  // Allows duplicate IDs, Optimises AddItem
#define MP_NO_REDRAW          0x00080000  // No auto redraw
#define MP_MAXSOFTKEYITEMS    0x00100000
#define MP_CALENDAR           0x00200000  // Special type - calendar event list view...
#define MP_AUTOSCROLLTIME     0x00400000  // Special type - autoscroll if in calendar list view...
#define MP_ICON_TEXT_TOP      0x00800000  // Icon View - Text at top
#define MP_ICON_NO_CENTERSEL  0x01000000  // Icon View - No Selection Always in Center
#define MP_ICON_SINGLE_FRAME  0x02000000  // Icon View - Single Frame
#define MP_UNDERLINE_TITLE    0x04000000  // Menu - Underline title
#define MP_ICON_ANIMATED      0x08000000  // Animate selected icon (assumes animated type or square image)
#define MP_BI_STATE_IMAGE     0x10000000  // 2 state image with no framing (unsel/sel)
#define MP_TRI_STATE_IMAGE    0x20000000  // 3 state image with no framing (unsel/sel/pressed)
#define MP_ICON_DELAY_IMAGE   0x40000000  // Single-frame image delayed
#define MP_MULTI_SEL          0x80000000  // Single-frame image delayed

#define MAX_MENU_TEXT         128         // Max number of chars in a menu item's string
#ifdef CUST_EDITION	
// Please define OEM properties at here
#define OEMMP_SWITCHNAVIGATIONKEY             0x00000001  // switch Up<-->Left Down<--->Right key
#define OEMMP_DISTINGUISH_INFOKEY_SELECTKEY   0x00000002  // ÉèÖÃ´ËÑÚÂë½«Çø±ð¶Ô´ýAVK_INFO¡¢AVK_SELECT
#define OEMMP_NODRAWSELECTBAR                 0x00000004  // ÉèÖÃ´ËÑÚÂë½«²»»­Ñ¡ÖÐÏîµÄÑ¡ÔñÌõ
#define OEMMP_IDF_ALIGN_CENTER               0X00000008
#define OEMMP_ICON_FIX_ITEMMAX     0x0000010

#define OEMMP_ARROW_TITLE 0x0000020  //ÉèÖÃ´ËÑÚÂë½«ÔÚ±êÌâÀ¸»æÖÆ×óÓÒÁ½¸ö¼ýÍ·

#define OEMMP_GRAPHIC_UNDERLINE 0x0000040  //ÉèÖÃ´ËÑÚÂë½«»æÖÆ²Ëµ¥ÏÂ»®Ïß
#define OEMMP_GRAPHIC_BG             0x0000080  //ÉèÖÃ´ËÑÚÂë½«»æÖÆÄ¬ÈÏÍ¼Æ¬±³¾°
#define OEMMP_USE_MENU_STYLE        0x00000C0  //ÉèÖÃ´ËÑÚÂë½«ÉèÖÃÍ³Ò»ÁÐ±íÏÔÊ¾·ç¸ñ
#endif /*CUST_EDITION*/
//
// Menu Color - The following bits are passed to IMENUCTL_SetColors() to
// indicate what values in the structure are being applied.
//

#define MC_BACK            0x0001   // Unselected item background
#define MC_TEXT            0x0002   // Unselected item text
#define MC_SEL_BACK        0x0004   // Selected item background
#define MC_SEL_TEXT        0x0008   // Selected item text
#define MC_FRAME           0x0010   // Simple frame color
#define MC_SCROLLBAR       0x0040   // Scrollbar frame color
#define MC_SCROLLBAR_FILL  0x0080   // Scrollbar fill color
#define MC_TITLE           0x0100   // Title background color
#define MC_TITLE_TEXT      0x0200   // Title text color
#ifdef CUST_EDITION	
#define ICONMENU_FIX_ITEMMAX    30
#endif /*CUST_EDITION*/

typedef struct
{
   uint16      wMask;      // Mask of bits to pay attention to in struct...
   RGBVAL      cBack;
   RGBVAL      cText;
   RGBVAL      cSelBack;
   RGBVAL      cSelText;
   RGBVAL      cFrame;
   RGBVAL      cScrollbar;
   RGBVAL      cScrollbarFill;
   RGBVAL      cTitle;
   RGBVAL      cTitleText;
}  AEEMenuColors;


//
// SetItem Flags
//

#define MSIF_TEXT     0x0001      // pText or wText is valid
#define MSIF_IMAGE    0x0002      // pImage or wImage is valid
#define MSIF_DATA     0x0004      // dwData is valid
#define MSIF_FONT     0x0008      // wFont is valid

//
// Sorting Arguments
//
// NOTE: Menu items are NOT sorted on insertion.
//

typedef int (*PFNMENUSORTCOMPARE)(void * pUser, uint32 dwData1, uint32 dwData2);

enum {
   MCS_CUSTOM,  // Passed PFNMENUSORTCOMPARE
   MCS_NAME_DESCENDING,
   MCS_NAME_ASCENDING,
   MCS_PARAM_DESCENDING,
   MCS_PARAM_ASCENDING
};
#ifdef WIN32
typedef unsigned AEEMenuSortType;
#else
typedef int8 AEEMenuSortType;
#endif

//
// Owner Draw Callback Data
//
typedef struct
{
   uint16 nSize;                // Structure Size
   uint16 nAction;              // Action
   uint32 nActionInfo;          // Information necessary to perform action
   AEERect rc;                  // Rect to be used for measuring/rendering
   IDisplay * pIDisplay;        // Display context for draw
} AEEODCBData;

//
// Owner Draw Callback
//
typedef void (*PFNOWNERDRAWCB)(void * pvContext, AEEODCBData* is);

//
// Owner Draw Actions
//
#define ODA_MEASURE_TITLE        1       // Measure Title
#define ODA_DRAW_TITLE           2       // Draw Title
#define ODA_MEASURE_ITEM         3       // Measure Item
#define ODA_DRAW_ITEM            4       // Draw Item
#define ODA_MEASURE_ITEM_TEXT    5       // Measure Item Text of IconView Menu
#define ODA_DRAW_ITEM_TEXT       6       // Draw Item Text of IconView Menu
#define ODA_MEASURE_SB           7       // Measure ScrollBar
#define ODA_DRAW_SB              8       // Draw ScrollBar

//
// Owner Draw Flags
//
#define ODF_TITLE_OD             0x0001      // Specifies that title is owner drawn
#define ODF_SB_OD                0x0002      // Specifies that scroll bar is owner drawn
#define ODF_ITEM_OD              0x0004      // Specifies that items can be owner drawn
#define ODF_MEASURE_ONCE         0x0008      // Specifies that items are of same dimensions and thus need to be measured only once

//
// ScrollBar Position/Length
//
#define OD_SB_POS(n)           (uint16)((n >> 16) & 0x0000ffff)
#define OD_SB_LEN(n)           (uint16)( n & 0x0000ffff)

//
// Item Info
//
#define OD_ITEM_ID(n)          (uint16)((n >> 16) & 0x0000ffff)
#define IS_SEL(n)              (boolean)(n & 0x00000001)

QINTERFACE(IMenuCtl)
{
   DECLARE_IBASE(IMenuCtl)

   DECLARE_ICONTROL(IMenuCtl)

   boolean     (*SetTitle)     (IMenuCtl * pIMenuCtl, const char * pszResFile, uint16 wResID, AECHAR * pText);
   boolean     (*AddItem)      (IMenuCtl * pIMenuCtl, const char * pszResFile, uint16 wResID, uint16 nItemID, AECHAR * pText, uint32 lData);
   boolean     (*AddItemEx)    (IMenuCtl * pIMenuCtl, CtlAddItem * pai);
   boolean     (*GetItemData)  (IMenuCtl * pIMenuCtl, uint16 nItemID, uint32 * plData);
   boolean     (*DeleteItem)   (IMenuCtl * pIMenuCtl, uint16 nItemID);
   boolean     (*DeleteAll)    (IMenuCtl * pIMenuCtl);
   void        (*SetSel)       (IMenuCtl * pIMenuCtl, uint16 nItemID);
   uint16      (*GetSel)       (IMenuCtl * pIMenuCtl);
   void        (*EnableCommand)(IMenuCtl * pIMenuCtl, boolean bEnable);
   void        (*SetItemText)  (IMenuCtl * pIMenuCtl, uint16 nItemID, const char * pszResFile, uint16 wResID, AECHAR * pText);
   void        (*SetItemTime)  (IMenuCtl * pIMenuCtl, uint16 nItemID, uint16 wMinStart, uint16 wDuration);
   int         (*GetItemTime)  (IMenuCtl * pIMenuCtl, uint16 nItemID, uint16 * pwDuration);
   void        (*SetStyle)     (IMenuCtl * pIMenuCtl,AEEItemStyle * pNormal, AEEItemStyle * pSel);
   void        (*SetColors)    (IMenuCtl * pIMenuCtl, AEEMenuColors  * pc);
   int16       (*MoveItem)     (IMenuCtl * po, uint16 wID1, int nDir);

   // v1.NNN Methods...

   int         (*GetItemCount) (IMenuCtl * po);
   uint16      (*GetItemID)    (IMenuCtl * po, int nIdx);
   boolean     (*GetItem)      (IMenuCtl * po, uint16 wID, CtlAddItem * pai);
   boolean     (*SetItem)      (IMenuCtl * po, uint16 wID, uint16 wFlags, CtlAddItem * pai);
   void        (*Sort)         (IMenuCtl * po, AEEMenuSortType t, PFNMENUSORTCOMPARE pfn, void * pUser);

   // v1.2+ Methods
   void        (*SetSelEx)     (IMenuCtl * po, uint16 wID, boolean bSelected);
   boolean     (*EnumSelInit)  (IMenuCtl *po);
   uint16      (*EnumNextSel)  (IMenuCtl *po);

   // v3.NNN Methods
   void        (*GetStyle)     (IMenuCtl * pIMenuCtl,AEEItemStyle * pNormal, AEEItemStyle * pSel);
   void        (*GetColors)    (IMenuCtl * pIMenuCtl, AEEMenuColors  * pc);
   void        (*GetItemRect)  (IMenuCtl * pIMenuCtl, uint16 nIdx, AEERect * prc);
   int         (*SetOwnerDrawCB)(IMenuCtl * po, PFNOWNERDRAWCB pfnCB, void * pvContext, uint16 nODFlags);
#ifdef CUST_EDITION	
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)   
   void        (*SetBottomBarType)( IMenuCtl *po, int bottomBar_e_Type);
   void        (*SetPopMenuRect)( IMenuCtl *po);
#endif
   void        (*GetSelItemRect)(IMenuCtl * pIMenuCtl, AEERect * prc);
   
   void        (*SetOemProperties)(IMenuCtl * pIMenuCtl, uint32 dwProps);
   uint32      (*GetOemProperties)(IMenuCtl * pIMenuCtl);

   void        (*SetLastItemSel)(IMenuCtl * po, boolean bSelected);
   #ifdef FEATURE_CUSTOMIZED_MENU_STYLE
   void        (*SetBackGround)(IMenuCtl *po, char *pstrImgResFile, uint16 nImgResID);
   #endif
#endif /*CUST_EDITION*/
};


#define IMENUCTL_AddRef(p)                      GET_PVTBL(p,IMenuCtl)->AddRef(p)
#define IMENUCTL_Release(p)                     GET_PVTBL(p,IMenuCtl)->Release(p)
#define IMENUCTL_HandleEvent(p,ec,wp,dw)        GET_PVTBL(p,IMenuCtl)->HandleEvent(p,ec,wp,dw)
#define IMENUCTL_Redraw(p)                      GET_PVTBL(p,IMenuCtl)->Redraw(p)
#define IMENUCTL_SetActive(p,b)                 GET_PVTBL(p,IMenuCtl)->SetActive(p,b)
#define IMENUCTL_IsActive(p)                    GET_PVTBL(p,IMenuCtl)->IsActive(p)
#define IMENUCTL_SetRect(p,prc)                 GET_PVTBL(p,IMenuCtl)->SetRect(p,prc)
#define IMENUCTL_GetRect(p,prc)                 GET_PVTBL(p,IMenuCtl)->GetRect(p,prc)
#define IMENUCTL_SetProperties(p,v)             GET_PVTBL(p,IMenuCtl)->SetProperties(p,v)
#define IMENUCTL_GetProperties(p)               GET_PVTBL(p,IMenuCtl)->GetProperties(p)
#define IMENUCTL_Reset(p)                       GET_PVTBL(p,IMenuCtl)->Reset(p)
#define IMENUCTL_SetTitle(p,pr, rid, pt)        GET_PVTBL(p,IMenuCtl)->SetTitle(p, pr, rid, pt)
#define IMENUCTL_AddItem(p,pr,rid,i,pt,d)       GET_PVTBL(p,IMenuCtl)->AddItem(p,pr,rid,i,pt,d)
#define IMENUCTL_AddItemEx(p,pai)               GET_PVTBL(p,IMenuCtl)->AddItemEx(p,pai)
#define IMENUCTL_GetItemData(p,i,pd)            GET_PVTBL(p,IMenuCtl)->GetItemData(p,i,pd)
#define IMENUCTL_DeleteItem(p,i)                GET_PVTBL(p,IMenuCtl)->DeleteItem(p,i)
#define IMENUCTL_DeleteAll(p)                   GET_PVTBL(p,IMenuCtl)->DeleteAll(p)
#define IMENUCTL_SetSel(p,i)                    GET_PVTBL(p,IMenuCtl)->SetSel(p,i)
#define IMENUCTL_GetSel(p)                      GET_PVTBL(p,IMenuCtl)->GetSel(p)
#define IMENUCTL_EnableCommand(p,b)             GET_PVTBL(p,IMenuCtl)->EnableCommand(p,b)
#define IMENUCTL_SetItemText(p, n, psz, wr, pt) GET_PVTBL(p,IMenuCtl)->SetItemText(p, n, psz, wr, pt)
#define IMENUCTL_SetItemTime(p, i, ws, wd)      GET_PVTBL(p,IMenuCtl)->SetItemTime(p, i, ws, wd)
#define IMENUCTL_GetItemTime(p, i, pwd)         GET_PVTBL(p,IMenuCtl)->GetItemTime(p, i, pwd)
#define IMENUCTL_SetStyle(p,pn,ps)              GET_PVTBL(p,IMenuCtl)->SetStyle(p,pn,ps)
#define IMENUCTL_SetColors(p, pc)               GET_PVTBL(p,IMenuCtl)->SetColors(p, pc)
#define IMENUCTL_MoveItem(p, w1, n)             GET_PVTBL(p,IMenuCtl)->MoveItem(p,w1,n)

#define IMENUCTL_SetItem(p,wID,wf,pai)          GET_PVTBL(p,IMenuCtl)->SetItem(p,wID,wf,pai)
#define IMENUCTL_GetItemCount(p)                GET_PVTBL(p,IMenuCtl)->GetItemCount(p)
#define IMENUCTL_GetItemID(p,nIdx)              GET_PVTBL(p,IMenuCtl)->GetItemID(p,nIdx)
#define IMENUCTL_SortCustom(p,pfn,pu)           GET_PVTBL(p,IMenuCtl)->Sort(p,MCS_CUSTOM,pfn,pu)
#define IMENUCTL_Sort(p,t)                      GET_PVTBL(p,IMenuCtl)->Sort(p,t,NULL,NULL)
#define IMENUCTL_GetItem(p,wID,pai)             GET_PVTBL(p,IMenuCtl)->GetItem(p,wID,pai)

#define IMENUCTL_SetSelEx(p,wID,s)              GET_PVTBL(p,IMenuCtl)->SetSelEx(p,wID,s)
#define IMENUCTL_SetFocus(p,wID)                GET_PVTBL(p,IMenuCtl)->SetSel(p,wID)
#define IMENUCTL_GetFocus(p)                    GET_PVTBL(p,IMenuCtl)->GetSel(p)
#define IMENUCTL_EnumSelInit(p)                 GET_PVTBL(p,IMenuCtl)->EnumSelInit(p)
#define IMENUCTL_EnumNextSel(p)                 GET_PVTBL(p,IMenuCtl)->EnumNextSel(p)

// >= 3.0
#define IMENUCTL_GetStyle(p,pn,ps)              GET_PVTBL(p,IMenuCtl)->GetStyle(p,pn,ps)
#define IMENUCTL_GetColors(p, pc)               GET_PVTBL(p,IMenuCtl)->GetColors(p, pc)
#define IMENUCTL_GetItemRect(p, nIdx, prc)      GET_PVTBL(p,IMenuCtl)->GetItemRect(p, nIdx, prc)
#define IMENUCTL_AddOwnerDrawItem(p, i, d)      GET_PVTBL(p,IMenuCtl)->AddItem(p, NULL, 0, i, NULL, d)
#define IMENUCTL_SetOwnerDrawCB(p, pf, pc, odf) GET_PVTBL(p,IMenuCtl)->SetOwnerDrawCB(p, pf, pc, odf)
#ifdef CUST_EDITION	
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
#define IMENUCTL_SetBottomBarType(p, t)         GET_PVTBL(p,IMenuCtl)->SetBottomBarType( p, t)
#define IMENUCTL_SetPopMenuRect(p)              GET_PVTBL(p,IMenuCtl)->SetPopMenuRect(p)
#endif
#define IMENUCTL_GetSelItemRect(p, prc)         GET_PVTBL(p,IMenuCtl)->GetSelItemRect(p, prc)

#define IMENUCTL_SetOemProperties(p,v)          GET_PVTBL(p,IMenuCtl)->SetOemProperties(p,v)
#define IMENUCTL_GetOemProperties(p)            GET_PVTBL(p,IMenuCtl)->GetOemProperties(p)

#define IMENUCTL_SetLastItemSel(p,s)              GET_PVTBL(p,IMenuCtl)->SetLastItemSel(p,s)
#ifdef FEATURE_CUSTOMIZED_MENU_STYLE
#define IMENUCTL_SetBackGround(p,s,n)      GET_PVTBL(p,IMenuCtl)->SetBackGround(p, s, n)
#endif
#endif /*CUST_EDITION*/
#endif    // AEEMENU_H

/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
AEE IMenuCtl Properties

Description:
These are the properties defined for IMenuCtl Interface .

Definition:

MP_WRAPSCROLL:       Wrap when scrolling off the end of screen. This property is
                     only applicable to AEECLSID_SOFTKEYCTL, AEECLSID_LISTCTL and 
                     AEECLSID_MENUCTL controls.
                     Although this property will not appear to be set for
                     AEECLSID_ICONVIEWCTL controls, this control still wraps.
MP_NO_ARROWS:        If set, no arrows even if scroll is possible
MP_ALLOW_DUPE_IDS    If set, allows duplicate IDs to be added, be careful to ensure unique IDs otherwise it may be difficult to handle EVT_COMMAND
MP_NO_REDRAW:        If set, IMENUCTL_Redraw() function is not internally called in IMENUCTL_SetActive()
                     or when changing selection
MP_UNDERLINE_TITLE:  If set, underline title
MP_BI_STATE_IMAGE:   If set, 2 state image with no framing (unsel/sel)
MP_TRI_STATE_IMAGE:  If set, 3 state image with no framing (unsel/sel/pressed)

Properties valid only for SoftKey list view of the menu control object:~

MP_MAXSOFTKEYITEMS:  If set, show maximum number of soft key items per screen

Properties valid only for icon list view of the menu control object:~

MP_ICON_TEXT_TOP:    If set, Icon View: Text at top
MP_ICON_TEXT_NOSEL:  If set, Icon View: Non reverse-video text
MP_ICON_SINGLE_FRAME:If set, Icon View: Single Frame
MP_ICON_DELAY_IMAGE: If set, Single-frame image delayed.
MP_ICON_ANIMATED:    If set, Animate selected icon (assumes animated type or square image).~
                     If the MP_ICON_ANIMATED property is set and the image has a shape where 
                     the width is an even interval of height, the image will be broken up 
                     into square segments and animated based on the number of times the width 
                     is evenly divisible by the height.
                     Example: if an image is 80 wide by 20 tall and the MP_ICON_ANIMATED 
                     property is set, then the image will be split into 4 frames of 20X20 
                     square and animated.
MP_ICON_NO_CENTERSEL:For an icon control menu with the height just enough to show single 
                     row of icons, if there are more items in the menu than can be displayed, 
                     the selected item is always kept in the center. This behavior is default 
                     and inconsistent with the behavior when in the number of items in menu 
                     is less than the number of display items. This property would make 
                     toolbar behavior consistent in both of the following cases:
                     Number of items able to be displayed < Number of menu items
                     Number of items able to be displayed > Number of menu items

Properties valid only for calendar event list view of the menu control object:

MP_CALENDAR:         If set, menu control object is in calendar event list view
MP_AUTOSCROLLTIME:   If set, auto-scroll if in calendar list view

Properties only valid for Softkey and MenuCtl objects:
MP_MULTI_SEL:        If set, the menu allows multiple selections.
                     Does not work with MP_BI_STATE_IMAGE and MP_TRI_STATE_IMAGE properties.
                     (uint16)-1 is a special flag for this type. Do not allow items to have this value.

Members:
None

Comments:
    If MP_MULTI_SEL is set with MP_BI_STATE_IMAGE a small selection frame will appear
with the text for the item that currently has selection focus.~

    If MP_MULTI_SEL is set with MP_TRI_STATE_IMAGE there will be no frame, the image's
states should indicate to the user whether the item is unselected/selected/chosen.
It is recommended to use text with menu items when the MP_MULTI_SEL style is set.

See Also:
IMENUCTL_SetProperties()
IMENUCTL_GetProperties()

=============================================================================

CtlAddItem

Description:
An encapsulation for a control item added to the control.

Definition:
typedef struct _CtlAddItem
{
   const AECHAR * pText;
   IImage * pImage;
   const char * pszResImage;
   const char * pszResText;
   uint16 wText;
   uint16 wFont;
   uint16 wImage;
   uint16 wItemID;
   uint32 dwData;
} CtlAddItem;

Members:

pText : Text in the item
pImage : Image in the item
pszResImage : Name of the resource file
pszResText : Name of the resource file
wText : Resource ID of the text string
wFont : 0 (zero): Default
wImage : Resource ID of the Image
wItemID : Control item ID
dwData : Data value associated with menu item


Comments:
pText and pImage are used by default. If they are not set (NULL), the pszResImage
and pszResText are used with wText and wImage to load the text and/or image respectively.

See Also:
None

=======================================================================

AEEMenuColors

Description:
AEEMenuColors is used to specify overriding color values for various menu control
object elements.

Definition:
typedef struct
{
   uint16 wMask;
   RGBVALcBack;
   RGBVALcText;
   RGBVALcSelBack;
   RGBVALcSelText;
   RGBVALcFrame;
   RGBVALcScrollbar;
   RGBVALcScrollbarFill;
   RGBVALcTitle;
   RGBVALcTitleText;
} AEEMenuColors;

Members:

wMask : Mask of bits to pay attention to in this struct
cBack : Background color of unselected items
cText: Text color for unselected items and Arrows
cSelBack: Background color for selected items
cSelText : Text color for selected items
cFrame: Simple frame color
cScrollbar : Scrollbar frame color
cScrollbarFill : Scrollbar fill color
cTitle: Background of title text
cTitleText: Color of title text


Comments:
None

See Also:
IMENUCTL_SetColors()
AEEMenuColors Mask

=============================================================================

AEEMenuColors Mask

Description:
Set of masks to indicate the item whose color needs to be changed.

Definition:

MC_BACK:       Unselected item background
MC_TEXT:       Unselected item text
MC_SEL_BACK:   Selected item background
MC_SEL_TEXT:   Selected item text
MC_FRAME:      Simple frame color
MC_SCROLLBAR:  Scrollbar frame color
MC_SCROLLBAR_FILL: Scrollbar fill color
MC_TITLE:      Title background color (are supported for list control only)
MC_TITLE_TEXT: Title text color (are supported for list control only)

Members:
None

Comments:
None.

See Also:
AEEMenuColors

=============================================================================

AEE Owner Draw Actions

Description:
These are the actions defined for owner draw menu.

Definition:

ODA_MEASURE_TITLE        // Measure Title
ODA_DRAW_TITLE           // Draw Title
ODA_MEASURE_ITEM         // Measure Item
ODA_DRAW_ITEM            // Draw Item
ODA_MEASURE_ITEM_TEXT    // Measure Item Text of IconView Menu
ODA_DRAW_ITEM_TEXT       // Draw Item Text of IconView Menu
ODA_MEASURE_SB           // Measure ScrollBar
ODA_DRAW_SB              // Draw ScrollBar

Members:
None

Comments:
None

See Also:
IMENUCTL_SetOwnerDrawCB()
AEEODCBData

=============================================================================
AEEODCBData

Description:
Owner Draw Callback Data

Definition:
typedef struct
{
   uint16 nSize;                // Structure Size
   uint16 nAction;              // Action
   uint32 nActionInfo;          // Information necessary to perform action
   AEERect rc;                  // Rect to be used for measuring/rendering
   IDisplay * pIDisplay;        // Display context for draw
} AEEODCBData;

Members:
nSize : Specifies structure size. Apps need to rely on structure size for version of AEEODCBData structure

nAction : Specifies action to be performed

nActionInfo : Provides additional information on the basis of nAction
              - Specifies ItemID for ODA_MEASURE_ITEM and ODA_MEASURE_ITEM_TEXT and ItemID
                with a flag to indicate if item is current selection for ODA_DRAW_ITEM and
                ODA_DRAW_ITEM_TEXT. Use OD_ITEM_ID(nActionInfo) to access item ID and
                IS_SEL(nActionInfo) to check if item is current selection.
              - Specifies scrol bar position and length for ODA_DRAW_SB. Use OD_SB_POS(nActionInfo)
                and OD_SB_LEN(nActionInfo) to access these.
              - Not used for ODA_MEASURE_TITLE, ODA_DRAW_TITLE and and ODA_MEASURE_SB

rc : Used according to the action as described below:
     - Used to get title height for ODA_MEASURE_TITLE
     - Used to specify rect to be used for title rendering for ODA_DRAW_TITLE
     - Used to get item height and width for ODA_MEASURE_ITEM
     - Used to specify rect to be used for item rendering for ODA_DRAW_ITEM
     - Used to get scroll bar width for and ODA_MEASURE_SB
     - Used to specify rect specifying scroll bar rectangle's position and dimensions for ODA_DRAW_SB
     - Used to get text height for ODA_MEASURE_ITEM_TEXT
     - Used to specify rect to be used for item text rendering for ODA_DRAW_ITEM_TEXT

pIDisplay : Pointer to IDisplay

Comments:
None

See Also:
IMENUCTL_SetOwnerDrawCB()

=============================================================================
AEEMenuSortType

Description:
    AEEMenuSortType specifies sort type.

Definition:
    typedef enum { 
    MCS_CUSTOM,
    MCS_NAME_DESCENDING,
    MCS_NAME_ASCENDING,
    MCS_PARAM_DESCENDING,
    MCS_PARAM_ASCENDING
    } AEEMenuSortType;

Members:
    MCS_CUSTOM :  Custom sort. 
    MCS_NAME_DESCENDING :  Name descending.
    MCS_NAME_ASCENDING :  Name ascending.
    MCS_PARAM_DESCENDING :  User data descending.
    MCS_PARAM_ASCENDING :  User data ascending.

Comments:
    None.

See Also:
    PFNMENUSORTCOMPARE
    IMENUCTL_Sort()

=============================================================================
PFNMENUSORTCOMPARE

Description:
    PFNMENUSORTCOMPARE specifies the comparison function to be used for custom sort
in IMENUCTL_SortCustom().

Definition:
    typedef int (*PFNMENUSORTCOMPARE)
    (
    void * pUser,
    uint32 dwData1,
    uint32 dwData2
    );

Members:
    pUser:  User data.~
    dwData1:  DataItem1.~
    dwData2:  DataItem2.~

Comments:
    The routine must compare the elements and then return one of the following values:~
~
    < 0 if DataItem1 is less than DataItem2.~
    = 0 if DataItem1 is equivalent to DataItem 2.~
    > 0 if DataItem1 is greater than DataItem 2.~
*

See Also:
    IMENUCTL_SortCustom()

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

IMenuCtl Interface

Menu controls allow the device user to make a selection from a list of items. The
UP, DOWN, LEFT and RIGHT arrow keys are used to identify the currently selected
menu item, which appears highlighted on the screen. When the device user presses
the SELECT key and command sending is enabled (see later in this section), an EVT_COMMAND
is sent to the application or dialog that created the menu, which includes the identifier
of the currently selected item. Following are four types of menu controls:

 -    A standard menu control (ClassID AEECLSID_MENUCTL) displays one menu item
per row on the screen, with each row containing the item's icon or text string.
If all the items do not fit on the screen, use the UP and DOWN arrow keys to scroll
the menu up or down.

 -    A list control (ClassID AEECLSID_LISTCTL) displays only the currently selected
menu item on the screen. This type of menu is useful in applications where the available
screen real estate is limited. Items in a list control menu contain only text (there
are no images). Use the UP and DOWN arrow keys to navigate to the specified menu
selection.

 -    A Soft Key menu control (ClassID AEECLSID_SOFTKEYCTL) displays the menu items
side by side along the bottom portion of the screen. Use the LEFT and RIGHT arrow
keys to designate the selected menu item. Soft Key menu controls contain icons and/or
text strings.

 -    An Icon View menu control (ClassID AEECLSID_ICONVIEWCTL) uses a bitmap icon
to represent each menu item. The bitmap icons are displayed in one or more rows
on the screen, and the arrow keys are used to move between rows and between the
icons in each row. The text string corresponding to the currently selected item
appears at the bottom of the screen.

Select the type you want by specifying its ClassID when you create an instance of
the menu control. IMENUCTL_HandleEvent() handles the UP,
DOWN, LEFT, and RIGHT arrow keys and the SELECT key.

 The menus also handle the Character
event. When you pass the EVT_CHAR message to an active menu, it will search for
a menu entry which has text beginning with that letter (case-insensitive). This
is type to select for a single character only. This will wrap around the menu if
the selection is not found by the end of the last item. If there are no matches,
the selection will not change. If the item doesn't have text associated with it,
it is skipped.

 If a calendar view menu is specified, a standard menu control
also handles, the device number keys (AVK_0 through AVK_9), which are used to enter
the time of a calendar appointment. Except for SoftKey menus, a menu control sends
a control tabbing event (EVT_CTL_TAB) when the device user presses the LEFT and
RIGHT keys. The softkey menu will send the control tabbing event when the UP or
DOWN key is pressed. Use control tabbing to move between controls in a multicontrol
screen. If your menu control is part of a dialog, the dialog intercepts the control
tabbing events and changes control focus appropriately.

Menu controls support a number of properties that can be set with IMENUCTL_SetProperties().
The following property names are the names of the bitmask constants you use to get
and set the properties:

 -    MP_WRAPSCROLL causes scrolling to wrap around (for example, the first menu
item is selected when the user presses the DOWN key while the last menu item is
selected). This property is always set for soft key and list control menus. This property
is also supported (disabled by default) for menuctl.

 -    MP_NO_ARROWS applies only to Icon View menus for which the MP_ICON_SINGLE_FRAME
property is set. It prevents the drawing of arrows on either side of the item text.

 -    MP_NO_REDRAW suppresses the redrawing of the menu each time the selected item
of a menu changes or the menu is set to active.

 -    MP_MAXSOFTKEYITEMS increases to 12, the number of Soft Key menu items that
can be displayed on the screen at once. By default, a maximum of three items are
displayed.

 -    MP_CALENDAR allows a standard menu to be used as a as a Day View in a calendar
program. Menu items (appointments) have the following characteristics that are not
described:
~
Horizontal lines are drawn between the menu items, and each item represents a calendar
appointment at a particular hour of the day.~
–   A small square appears in the upper left corner to indicate PM. No square means
AM.~
–   A triangle will be drawn on the right edge of the menu to indicate the current
time relative to the menu item wMinStart set via IMENUCTL_SetItemTime().~
–   The height of the menu items is proportional to the value of wDuration set via
IMENUCTL_SetItemTime().~
The device's number keys are used to select the hour of an 
appointment, and the application can set menu-item text describing the appointment. ~
*

 -    MP_AUTOSCROLLTIME automatically scrolls a Calendar View menu so that the entry
corresponding to the current time appears on the screen when the calendar is displayed.
This only happens when IMENUCTL_SetActive() is called with bActive=TRUE and the
menu is currently NOT active. An IMENUCTL_Redraw() after this would also be necessary.

 -    MP_ICON_TEXT_TOP causes the text string of the currently selected item in
an Icon View menu to appear at the top of the screen instead of the bottom.

 -    MP_ICON_SINGLE_FRAME displays only the icon of the currently selected menu
item on the screen. By default, an Icon View menu displays all the icons in rows
and columns and highlights the selected one.

 -    MP_UNDERLINE_TITLE causes the menu's title to be underlined.

 -    MP_MULTI_SEL allows multiple choices on one menu. Choices are toggled when
the user presses the Select key on a menu item. These pressed will not cause an
EVT_COMMAND message to be sent to the application. The applet should handle the
EVT_CTL_TAB control tabbing message to either set focus to another control or collect
relevant data.

 -    MP_BI_STATE_IMAGE ensures that the selected menu or soft key item looks different
from unselected menu or soft key items.

 -    MP_TRI_STATE_IMAGE ensures that the selected menu or soft key, pressed menu
or soft key, and unselected menu or soft key items look different from each other.

 -    MP_ICON_ANIMATED ensures that a selected icon of an animated type or a square
image can be animated.

Menu controls implement several functions in addition to those in the IControl Interface.~
~
IMENUCTL_SetTitle() specifies a value for the menu's title, which is drawn at the
top of its rectangle.

IMENUCTL_EnableCommand() enables or disables the sending of
EVT_COMMAND events to your application when the device user presses the SELECT key
(command sending is enabled by default).

IMENUCTL_SetStyle() customizes the appearance
of the selected item and of the non-selected items in a menu including special pixel
borders, padding space around each item, and other appearance elements.~
*

After creating a menu, you must specify the items that the menu contains. 
~

IMENUCTL_AddItem()adds items to the menu that do not contain bitmap icons. When calling this function,
you specify the item's text string either from a resource file or defined in your
code, an integer item ID, and an optional double-word data pointer. When the device
user selects the item, your application's IAPPLET_HandleEvent() function is called
with an EVT_COMMAND event; the item ID and double-word pointer are specified as
the wParam and dwParam parameters in this function call. 

IMENUCTL_AddItemEx() is
an extended version of IMENUCTL_AddItem() that provides additional information about
the added menu item including its bitmap icon and the font used to display its text
string.

IMENUCTL_DeleteItem() deletes a menu item with a particular ID, and IMENUCTL_DeleteAll()
deletes all the items from a menu.

IMENUCTL_GetSel() returns the item ID of the currently selected menu item. You use
this function when the device user's selection is not obtained by pressing the SELECT
key. For example, you may retrieve the device user's selection when the dialog containing
the menu control is terminated. IMENUCTL_SetSel() specifies the item ID that to
be the currently selected one. The menu control normally determines this based on
the device user pressing the arrow keys, but you can use IMENUCTL_SetSel() to designate
the initially selected item when the menu is first displayed.

IMENUCTL_GetItemData() retrieves the double-word data pointer of a menu item.

IMENUCTL_SetItemText() changes the text string of an existing menu item.

IMENUCTL_SetItemTime() sets the start time and duration associated with an item
in a calendar menu (see above), and

IMENUCTL_GetItemTime() retrieves the time information
of a calendar-menu item.
*

===H2>
To create and use a menu control
===/H2>

1.   Call ISHELL_CreateInstance() to create the menu control instance and obtain
an interface pointer to it, specifying which of the four types of menu control you
choose by its ClassID.

2.   Call IMENUCTL_SetRect() to define the screen rectangle in which the menu is
to be drawn.

3.   Call IMENUCTL_SetProperties() to set any of the menu control properties, and
call IMENUCTL_SetStyle to customize the appearance of your menu items.

4.   Call IMENUCTL_SetTitle() to specify a menu title, and call IMENUCTL_AddItem()
or IMENUCTL_AddItemEx() for each item to be added to the menu.

5.   When you have completely specified the properties and contents of the menu
control, call IMENUCTL_SetActive() to draw the control on the screen and enable
it to receive key events from the device user. While the menu control is active,
your application's IAPPLET_HandleEvent() function must call IMENUCTL_HandleEvent()
to pass all handled key events to the menu control for processing.

6.   Determine how you are to obtain the device user's menu selection. If you process
the selection when the device user presses the SELECT key, the IAPPLET_HandleEvent()
function can contain logic to handle the selection of each menu item when the EVT_COMMAND
is received. If your application receives EVT_COMMAND events from more than one
control, be sure that the item IDs passed in the wParam parameter are unique. If
you retrieve the device user's selection at some other time, you must call IMENUCTL_GetSel()
or IMENUCTL_GetItemData() at this time to access the currently selected menu item
and its double-word data, if any.

7.   When you no longer need the menu control, call IMENUCTL_Release() to free it.


The following header file is required:~
AEEMenu.h

=============================================================================

IMENUCTL_AddItem()

Description:
    This function adds a new menu item to a menu. The text string identifying the
menu can be specified from a resource file or by using the pText parameter.
~
    If pText is a valid string, the function uses this string as the item name.~
    If pText is NULL, the function reads the string corresponding to the given resource
identifier, wResID, from the resource file and sets it as the item name.~
    If the text string and the resource information are invalid, this function fails.~
    If the text string in the pText field and the resource file information are
both valid, the pText parameter field takes precedence and is used for the menu
item name.~
    If the object identified by IMenuCtl Interface is a soft key menu, this function
also determines the extent of the new items in x-axis and the maximum number of
items that can be displayed on the soft key bar.~
*

    Use the lData to store a double-word data value associated with the menu item
to be created. Use IMENUCTL_GetItemData() to get back the parameter value.

Prototype:
    boolean IMENUCTL_AddItem
    (
    IMenuCtl * pIMenuCtl,
    const char * pszResFile,
    uint16 wResID,
    uint16 nItemID,
    AECHAR * pText,
    uint32 lData
    )

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    pszResFile  :  NULL-terminated string that contains the resource file name.
    wResID  :  Resource identifier that identifies a text string resource.
    nItemID  :  Item identifier that uniquely identifies a menu item.
    pText  :  NULL-terminated string that contains the menu item name.
    lData  :  Item data associated with the menu item.

Return Value:
    TRUE:  if successful.
    FALSE:  if unsuccessful.

Comments:
   This function does not automatically update the screen. For the new menu item
to take effect on the device screen, use IMENUCTL_Redraw(). If the menu is to be
activated subsequent to adding menu items, you don’t need to use IMENUCTL_Redraw()
because IMENUCTL_SetActive() updates the screen with the new menu items. 

   Use IMENUCTL_DeleteItem() to remove an item from the menu.

   Its preferable that control rect and properties are set before adding items.

Side Effects:
    When a device user activates the newly added menu item, nItemID and lData values
are sent to the applet event handling the function in the short-data and long-data
fields, respectively.

Version:
    Introduced BREW Client 1.0

See Also:
    IMENUCTL_DeleteItem()
    IMENUCTL_Redraw()
    IMENUCTL_SetActive()

=============================================================================

IMENUCTL_AddItemEx()

Description:
    This function adds an item to a menu, list, or icon view. The new item’s properties
are indicated by the CtlAddItem parameter.
    If pai->pText is a valid string, the function uses this string as the item name.
    If pai->pText is NULL, the function reads the string corresponding to the given
resource identifier associated with pai->wText and pai->pszResText. If this fails,
the function returns FALSE.
    If pai->pImage is a valid IImage, the function calls IIMAGE_AddRef() and uses
the image. This allows the caller to release the image object.
    If the text string in pai->pText and the resource file information are both
valid, pai->pText takes precedence and is used for the menu item name.
    If pai->pImage is NULL and pai->wImage is specified, the function attempts to
load the image from the specified resource. If the image fails to load, the function
returns FALSE.

Prototype:
    boolean IMENUCTL_AddItemEx(IMenuCtl * pIMenuCtl, CtlAddItem * pai)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    pai  :  Pointer to the CtlAddItem structure.

Return Value:
    TRUE:  if successful.
    FALSE:  if unsuccessful.

Comments:
    This function does not automatically update the screen. For the new menu item
to take effect on the device screen, use IMENUCTL_Redraw(). If the menu is to be
activated subsequent to adding menu items, you don’t need to use IMENUCTL_Redraw()
because IMENUCTL_SetActive() updates the screen with the new menu items.

   Its preferable that control rect and properties are set before adding items.

Side Effects:
    When a device user activates the newly added menu item, pai->wItemID and 
	pai->dwData values are sent to the applet event handling the function in 
	the short-data and long-data fields, respectively.

Version:
    Introduced BREW Client 1.0

See Also:
    IMENUCTL_Redraw()
    IMENUCTL_AddItem()
    IMENUCTL_AddItemEx()

=============================================================================

IMENUCTL_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 


=============================================================================

IMENUCTL_DeleteAll()

Description:
    This function deletes all the menu items from a menu control object.

Prototype:
    boolean IMENUCTL_DeleteAll(IMenuCtl * pIMenuCtl)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.

Return Value:
    TRUE:  if successful.
    FALSE:  if unsuccessful.

Comments:
    This function does not update the device screen when invoked. To update the
device screen, use IMENUCTL_Redraw().

Side Effects:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    None

=============================================================================

IMENUCTL_DeleteItem()

Description:
    This function deletes a menu item from the menu control object. The nItemID
parameter identifies the menu item to be deleted. Then the function re-draws the
entire Menu.

Prototype:
    boolean IMENUCTL_DeleteItem(IMenuCtl * pIMenuCtl,uint16 nItemID)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    nItemID  :  Menu item identifier used when the menu item was added to the menu.

Return Value:
    TRUE:  if successful.
    FALSE:  if unsuccessful.

Comments:    
   This function does not update the device screen when invoked.
To update the device screen use IMENUCTL_Redraw().

Side Effects:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    None

=============================================================================

IMENUCTL_EnableCommand()

Description:
    This function enables the menu control object to send a specified command to
the AEE Shell when it receives an event generated by a device user pressing the
SELECT key.

Prototype:
    void IMENUCTL_EnableCommand(IMenuCtl * pIMenuCtl, boolean bEnable)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface of the menu control object.
    bEnable  :  Enable/Disable flag.

Return Value:
    None

Comments:
    The SELECT key is located in different physical positions on a device depending
on the manufacturer or model. When a device user presses this key, the applet receives
the event type AVK_SELECT.

Version:
    Introduced BREW Client 1.0

See Also:
    IMENUCTL_HandleEvent()

=============================================================================

IMENUCTL_EnumNextSel()
    DESCRIPTION:
    This function should only be used only with Multi Select menus (MP_MULTI_SEL).
This function gets the ID of the next selected Item.

Prototype:
    uint16 IMENUCTL_EnumNextSel(IMenuCtl * pIMenuCtl);

Parameters:
    pIMenuCtl :  Pointer to IMenuCtl Interface object

Return Value:
    ID of the next selected item. When no more items are selected the value (uint16)-1
is returned. You should avoid using this value as an item ID when using multiple
select menus

Comments:
    None

Version:
    Introduced BREW Client 2.0

See Also:
    IMENUCTL_EnumSelInit()

=============================================================================

IMENUCTL_EnumSelInit()
    DESCRIPTION:
    This function should only be used only with Multi Select menus (MP_MULTI_SEL).
This function initializes the enumeration of menu items that have selection.

Prototype:
    boolean IMENUCTL_EnumSelInit(IMenuCtl * pIMenuCtl);

Parameters:
    pIMenuCtl :  Pointer to IMenuCtl Interface object

Return Value:
    TRUE:  if successful.
    FALSE:  if unsuccessful.

Comments:
    This function MUST be called before IMENUCTL_EnumNextSel()

Version:
    Introduced BREW Client 2.0

See Also:
    IMENUCTL_EnumNextSel()

=============================================================================

IMENUCTL_GetFocus()

Description:
    This function should only be used only with Multi Select menus (MP_MULTI_SEL).
This function will get the item ID of the menu item which has the user input focus.

Prototype:
    uint16 IMENUCTL_GetFocus(IMenuCtl * pIMenuCtl);

Parameters:
    pIMenuCtl :  Pointer to IMenuCtl Interface object

Return Value:
    The ID of the item which has the user input focus.

Comments:
    None

Version:
    Introduced BREW Client 2.0

See Also:
    IMENUCTL_SetFocus()

=============================================================================

IMENUCTL_GetItem()

Description:
    This function queries all of the relevant values associated with a menu item,
including the following fields:
    Image
    Image ID (if a resource based)
    Image File Name
    Text
    Item ID
    Data
    Font
    All objects and pointers returned are owned by the menu control. If used, the
caller should make copies or increment the reference count of the object. For example,
the caller should call IIMAGE_AddRef() on the image pointer returned if the pointer
is to be used. If this is done, the caller must also call IIMAGE_Release() when
the pointer is no longer needed.

Prototype:
    boolean IMENUCTL_GetItem
    (
    IMenuCtl * pIMenuCtl, 
    uint16 wID, 
    CtlAddItem * pai
    )

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    wID :  Menu item ID.
    pai :  Item information structure.

Return Value:
    TRUE:  if successful.
    FALSE:  if unsuccessful.

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    CtlAddItem
    IMENUCTL_SetItem()

=============================================================================

IMENUCTL_GetItemCount()

Description:
    This function returns the count of the number of items in a menu.

Prototype:
    int IMENUCTL_GetItemCount(IMenuCtl * pIMenuCtl)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.

Return Value:
    Number of menu items.

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    IMENUCTL_GetItemID()

=============================================================================

IMENUCTL_GetItemData()

Description:
    This function gets the data associated with a menu item in the given menu control
object. The nItemID parameter identifies the menu item from which the data is requested.

Prototype:
    boolean IMENUCTL_GetItemData
    (
    IMenuCtl * pIMenuCtl,
    uint16 nItemID,
    uint32 * plData
    )

Parameters:
    pIMenuCtl  :  [in]  :  Pointer to the IMenuCtl Interface object.
    nItemID  :  [in]  :  Menu item identifier used when this item is added to the
                         menu.
    plData  :  [out]  :  Placeholder for item data.

Return Value:
    TRUE:  if successful.
    FALSE:  if unsuccessful.

Comments:
    The retrieved data is associated with a menu when the menu item is added. See
IMENUCTL_AddItem() for more information on menu item data.

Version:
    Introduced BREW Client 1.0

See Also:
    IMENUCTL_AddItem()

=============================================================================

IMENUCTL_GetItemID()

Description:
    This function returns the item ID given the index of the item.

Prototype:
    uint16 IMENUCTL_GetItemID(IMenuCtl * pIMenuCtl, int nIdx)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    nIdx :  Index of the item.

Return Value:
    Item ID.

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    None

=============================================================================

IMENUCTL_GetItemTime()

Description:
    If the menu control is calendar, this function retrieves the start time and
duration (the difference between the end time and the start time) of the menu item
specified by the menu item ID.

Prototype:
    int IMENUCTL_GetItemTime
    (
    IMenuCtl * pIMenuCtl,
    uint16 nItemID,
    uint16 * pwDuration
    )

Parameters:
    pIMenuCtl  :  [in]  :  Pointer to the IMenuCtl Interface object.
    nItemID  :  [in]  :  Menu item Identifier.
    pwDuration  :  [out]  :  Duration in minutes.

Return Value:
    Start time set in IMENUCTL_SetItemTime().
    Value of -1, if the menu item cannot be found.

Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    IMENUCTL_SetItemTime()

=============================================================================

IMENUCTL_GetProperties()

Description:
    This function retrieves the menu control-specific properties or flags.

Prototype:
    uint32 IMENUCTL_GetProperties(IMenuCtl * pIMenuCtl)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.

Return Value:
    The following 32-bit properties are defined for the menu control object:
~
    MP_WRAPSCROLL, if set, wrap when scrolling off the end of screen
	(applicable to Soft Key, MenuCtl and List controls).

    MP_NO_ARROWS, if set, no arrows even if scroll is possible (Icon View).

    MP_MAXSOFTKEYITEMS, shows maximum number of soft key items per screen.

    MP_ALLOW_DUPE_IDS, If set, allows duplicate IDs to be added,
	be careful to ensure unique IDs otherwise it may be difficult to handle EVT_COMMAND

    MP_NO_REDRAW, if set, IMENUCTL_Redraw() function is not internally called in
IMENUCTL_SetActive() or when changing selection.

*
    The following attributes are defined only for the Icon View of the menu control
object:
~
    MP_ICON_TEXT_TOP, Icon View: Text at top.

    MP_ICON_SINGLE_FRAME, Icon View: Single frame.

    MP_UNDERLINE_TITLE, Menu: Underline title.

    MP_ICON_ANIMATED, if set, Animate selected icon. If the MP_ICON_ANIMATED property
is set and the image has a shape where the width is an even interval of height,
the image will be broken up into square segments and animated based on the number
of times the width is evenly divisible by the height. Example: if an image is 80
wide by 20 tall and the MP_ICON_ANIMATED property is set, then the image will be
split into 4 frames of 20X20 square and animated.
*
    The following two attributes are defined only for the calendar view of the menu
control object:
~
    MP_CALENDAR (see Illustration) allows a standard menu to be used as a as a Day
View in a calendar program Menu items (appointments) have the following characteristics
     -    A small square appears in the upper left corner to indicate PM. No square
means AM.
     -    A triangle is drawn on the right edge of the menu to indicate the current
time relative to the menu item wMinStart set via IMENUCTL_SetItemTime().
     -    The height of the menu items is proportional to the value of wDuration
set via IMENUCTL_SetItemTime().
*

    MP_AUTOSCROLLTIME, if set, specifies that auto-scroll is set in calendar view.
This only happens when IMENUCTL_SetActive() is called with bActive=TRUE, and the
menu is not currently active. An IMENUCTL_Redraw() after this is also necessary.
*

    The following attributes are defined only for the standard menu or soft key
control objects:
~
    MP_BI_STATE_IMAGE, if set, 2 state image with no framing (unsel/sel).

    MP_TRI_STATE_IMAGE, if set, 3 state image with no framing (unsel/sel/pressed).
*

    Property only valid for Softkey and plain menu control
~
    MP_MULTI_SEL, if set, Multiple pressed entries are allowed.
*
Comments:
    None

Version:
    Introduced BREW Client 1.0

See Also:
    IMENUCTL_SetProperties()

=============================================================================

IMENUCTL_GetRect()

Description:
    This function returns the control rectangle value of the menu control object.
It is useful for determining the optimal/default size and position of a control
object after it is created.

Prototype:
    void IMENUCTL_GetRect(IMenuCtl * pIMenuCtl, AEERect * prc)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    prc  :  Rectangle to be filled with the rectangle coordinates of the menu control.

Return Value:
    None

Comments:
    The menu control object is displayed in a rectangular area of the screen specified
by the coordinates stored in the menu control object. By default, these coordinates
correspond to the coordinates of the entire display screen when the menu control
object is instantiated. Use IMENUCTL_SetRect() to change the coordinates and encompass
any section of the display screen. Use IMENUCTL_GetRect() to retrieve the current
specifications of the control rectangle.

Version:
    Introduced BREW Client 1.0

See Also:
    AEERect
    IMENUCTL_SetRect()

=============================================================================

IMENUCTL_GetSel()

Description:
    This function gets the menu item identifier of the menu control object’s current
selection.

Prototype:
    uint16 IMENUCTL_GetSel(IMenuCtl * pIMenuCtl)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.

Return Value:
    Returns the ID of the current menu item selection of the IMenuCtl Interface
object.

Comments:
    This will get the position of selection, not chosen properties for a menu that
has MP_MULTI_SEL.

Version:
   Introduced BREW Client 1.0

See Also:
    IMENUCTL_SetSel()

=============================================================================

IMENUCTL_HandleEvent()

Description:
    The menu control object uses this function to handle events it receives. An
active menu control object handles key press, set title, and add item events. An
inactive menu control object handles only set title and add item events. The typical
key press events processed by the menu control object are called by pressing the
UP, DOWN, LEFT, and RIGHT keys. If command sending is enabled for the menu control
object, and when it receives the event generated by pressing the SELECT key, the
menu control object sends a command event to the AEE Shell.

Prototype:
    boolean IMENUCTL_HandleEvent
    (
    IMenuCtl * pIMenuCtl,
    AEEEvent evt,
    uint16 wp,
    uint32 dwp
    )

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    evt  :  Event code.
    wp  :  16-bit event data.
    dwp  :  32-bit event data.

Return Value:
    TRUE:  if the event was processed by the IMenuCtl Interface object.
    FALSE:  if the event was not processed.

Comments:
    The menu control event handler is used in applets to handle menu related inputs
from a device user. For example, when a device user presses the down arrow key to
move from one menu item selection to another, this event is handled by the menu
control event handler. In this case, the applet event handler function receives
the corresponding event. The applet event handler at this time can call the menu
control object event handler to process this event. If the applet event handler
does not call the menu control event handler, this event remains unprocessed.
    The SELECT key is located in different physical locations on a device, depending
on the manufacturer or model. When a device user presses this key, the applet receives
the event word parameter wParam of type AVK_SELECT.

Version:
   Introduced BREW Client 1.0

See Also:
    IMENUCTL_EnableCommand()

=============================================================================

IMENUCTL_IsActive()

Description:
    This function returns the active/inactive state of the menu control object.

Prototype:
    boolean IMENUCTL_IsActive(IMenuCtl * pIMenuCtl)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.

Return Value:
    TRUE:  if the menu control is active.
    FALSE:  if otherwise.

Comments:
    None

See Also:
    IMENUCTL_SetActive()

=============================================================================

IMENUCTL_MoveItem()

Description:
    This function moves a menu item by one position in the specified direction.

Prototype:
    void IMENUCTL_MoveItem(IMenuCtl * pIMenuCtl, uint16 nItemID, int nDir);

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object
    nItemID  :  Menu item identifier
    nDir  :  Direction:
             Value of -1:UP
             Value of  1:DOWN

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    None

=============================================================================

IMENUCTL_Redraw()

Description:
    This function instructs the menu control object to redraw its contents. The
menu control object does not redraw its contents every time the underlying data
behind the menu control changes, allowing several data updates to occur while minimizing
screen flashes. For example, several changes can be made to the contents of the
menu control object with no visible effect until the Redraw function is called.

Prototype:
    boolean IMENUCTL_Redraw(IMenuCtl * pIMenuCtl)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.

Return Value:
    TRUE:  if the menu control was redrawn.
    FALSE:  if unsuccessful.

Comments:
    The menu control object is displayed in a rectangular area of the screen specified
by the coordinates stored in the menu control object. By default, these coordinates
correspond to the coordinates of the entire display screen when the menu control
object is instantiated. Use IMENUCTL_SetRect() to change the coordinates and encompass
any section of the display screen. Use IMENUCTL_Redraw() to redraw only the menu
in the area of the screen bounded by the current specifications of the menu control
object rectangle coordinates.

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

IMENUCTL_Release()

Description:
    This function is inherited from IBASE_Release(). 

Version:
   Introduced BREW Client 1.0

See Also:
    IMENUCTL_AddRef()

=============================================================================

IMENUCTL_Reset()

Description:
    This function instructs the menu control to reset (free or delete) its contents
and to immediately leave active/focus mode. Resetting the menu control object removes
the menu items from the menu and removes the title associated with the menu.

Prototype:
    void IMENUCTL_Reset(IMenuCtl * pIMenuCtl)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.

Return Value:
    None

Comments:
    A reset of a menu control object does not update the device screen, so all the
graphics associated with the menu remain on the device screen. To remove the menu
from the device screen, use IMENUCTL_Redraw() for the menu control object.

Side Effects:
    A Reset menu control object does not handle any events received from a device
user, such as pressing the UP, DOWN, LEFT, and RIGHT arrow keys.

Version:
   Introduced BREW Client 1.0

See Also:
    IMENUCTL_SetActive()
    IMENUCTL_Redraw()

=============================================================================

IMENUCTL_SetActive()

Description:
    This function makes a menu control object active or inactive. Only an active
menu control object handles the event sent to it. An inactive menu control object
ignores the events.
    NOTE: This function redraws the menu control if the specified active state does
not match the current state of the menu. Thus, calling IMENUCTL_SetActive() does
not redraw the menu if the state is not changed as a result of the function call.

Prototype:
    void IMENUCTL_SetActive(IMenuCtl * pIMenuCtl, boolean bActive)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    bActive  :  Boolean flag that specifies whether to activate (TRUE) or
    deactivate (FALSE) the menu control object.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IMENUCTL_IsActive()

=============================================================================

IMENUCTL_SetColors()

Description:
    By default, the color of menu elements are determined by entries in the system
color table defined by the device manufacturer. These entries include:
    CLR_SYS_ITEM 
     Background color of unselected items.
    CLR_SYS_ITEM_TEXT 
     Text color for unselected items and arrows.
    CLR_SYS_ITEM_SEL 
     Background color for selected items.
    CLR_SYS_ITEM_SEL_TEXT 
     Text color for selected items.
    CLR_USER_FRAME 
     Simple frame color.
    CLR_SYS_SCROLLBAR 
     Scrollbar frame color.
    CLR_SYS_SCROLLBAR_FILL 
     Fill color of scrollbar.
    CLR_SYS_TITLE 
     Background of title text.
    CLR_SYS_TITLE_TEXT 
     Color of title text.
    This function allows the caller to override most of these settings for the menu
control objects.

Prototype:
    void IMENUCTL_SetColors(IMenuCtl * pIMenuCtl, AEEMenuColors * pc)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    pc :  Pointer to the structure containing both a bitmask to indicate the item
          whose color needs to be changed, and the color values.
		  AEEMenuColors Mask MC_TITLE and MC_TITLETEXT are supported for list control only.

Return Value:
    None

Comments:
    Passing NULL to this function resets the menu colors. The caller cannot override
the system settings for 3-D framed objects.

Version:
   Introduced BREW Client 1.0

See Also:
    AEEMenuColors

=============================================================================

IMENUCTL_SetFocus()

Description:
    This function is only to be used with Multi Select menus (MP_MULTI_SEL). This
function sets the focus to the menu item indicated.

Prototype:
    void IMENUCTL_SetFocus(IMenuCtl * pIMenuCtl, uint16 wID);

Parameters:
    pIMenuCtl :  Pointer to IMenuCtl Interface object
    wID :  ID of the item to set.

Return Value:
    None

Comments:
    If (uint16)-1 is given and is not found in the menu focus will go to the first
item.

Version:
   Introduced BREW Client 1.1

See Also:
    IMENUCTL_GetFocus()

=============================================================================

IMENUCTL_SetItem()

Description:
    This function sets any field in a menu item after the item has been created,
allowing the item's icon, text, etc., to be modified after the item has been created.
The valid fields in the CtlAddItem structure are specified by the mask of bits in
wFlags. These bits are defined as:
    MSIF_TEXT (Update text)
    MSIF_IMAGE (Update image)
    MSIF_DATA (Update data)
    MSIF_FONT (Update font)

Prototype:
    boolean IMENUCTL_SetItem
    (
    IMenuCtl * pIMenuCtl,
    uint16 wID,
    uint16 wFlags,
    CtlAddItem * pai
    )

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    wID :  Menu Item ID.
    wFlags :  Mask of valid fields in CtlAddItem structure.
    pai :  Item information structure.

Return Value:
    TRUE:  if successful.
    FALSE:  if unsuccessful.

Comments:
    This function does not update the device screen when invoked. For the new title
to take effect on the device screen, use IMENUCTL_Redraw(). The title of a soft
key control menu is not shown.

Version:
   Introduced BREW Client 2.0

See Also:
    CtlAddItem
    IMENUCTL_GetItem()
    IMENUCTL_Redraw()

=============================================================================

IMENUCTL_SetItemText()

Description:
    This function sets a new menu item text name, given the menu item ID. The new
text is passed through a string pointer, or through a resource file name and an
ID. If both are provided, the string pointer takes precedence.

Prototype:
    void IMENUCTL_SetItemText
    (
    IMenuCtl * pIMenuCtl,
    uint16 nItemID,
    const char * pszResFile,
    uint16 wResID,
    AECHAR * pText
    )

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    nItemID  :  Menu item identifier.
    pszResFile  :  NULL-terminated string containing the resource file name.
    wResID  :  Resource Id.
    pText  :  New menu item text in string form.

Return Value:
    None

Comments:
    This function overwrites the menu name of a currently existing menu item. In
this case, the nItemID parameter contains the currently existing menu item ID.

Version:
   Introduced BREW Client 1.0

See Also:
    None

=============================================================================

IMENUCTL_SetItemTime()

Description:
    This function sets the calendar menu control. It sets the start time and duration
associated with an item in a calendar menu, 

Prototype:
    void IMENUCTL_SetItemTime
    (
    IMenuCtl * pIMenuCtl,
    uint16 nItemID,
    uint16 wMinStart,
    uint16 wDuration
    )

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    nItemID  :  Menu item identifier.
    wMinStart  :  Start time in minutes.
    wDuration  :  Duration in minutes.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IMENUCTL_GetItemTime()

=============================================================================

IMENUCTL_SetProperties()

Description:
    This function sets menu control-specific properties or flags. These properties
define the behavior of the menu control object.

Prototype:
    void IMENUCTL_SetProperties(IMenuCtl * pIMenuCtl, uint32 dwProps)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    dwProps  :  32-bit set of flags/properties.

    The following 32-bit properties are defined for the menu control object -
~
    MP_WRAPSCROLL  -  If set, wrap when scrolling off the end of screen - (applicable 
to Soft Key, MenuCtl and List controls).
    MP_NO_ARROWS  -  If set, no arrows even if scroll is possible (IconView).
    MP_ALLOW_DUPE_IDS - If set, allows duplicate IDs to be added, be careful to ensure 
unique IDs otherwise it may be difficult to handle EVT_COMMAND
    MP_NO_REDRAW  -  If set, IMENUCTL_Redraw() is not internally called in IMENUCTL_SetActive()
or when changing selection.
*

    The following attributes are defined only for Icon View of the menu control
object.
~
    MP_ICON_TEXT_TOP  -  Icon View: Text at top.
    MP_ICON_SINGLE_FRAME  -  Icon View: Single Frame.
    MP_UNDERLINE_TITLE  -  Menu: Underline title.
    MP_ICON_DELAY_IMAGE -  Single-frame image delayed 
    MP_ICON_ANIMATED  -  If set, animate selected icon of animated type or square
image).
~

    If the MP_ICON_ANIMATED property is set and the image has a shape where the
width is an even interval of height, the image will be broken up into square segments
and animated based on the number of times the width is evenly divisible by the height.
Example: if an image is 80 wide by 20 tall and the MP_ICON_ANIMATED property is
set, then the image will be split into 4 frames of 20X20 square and animated.
*

    MP_ICON_NO_CENTERSEL -  For an icon control menu with the height just enough
to show single row of icons, if there are more items in the menu than can be displayed,
the selected item is always kept in the center. This behavior is default and inconsistent
with the behavior when in the number of items in menu is less than the number of
items able to be displayed. This property would make toolbar behavior consistent
in both of the following cases -
~

    Number of items able to be displayed < Number of menu items

    Number of items able to be displayed > Number of menu items
*
*
    The following two attributes are defined only for calendar view of the menu
control object  -
~
    MP_CALENDAR  -  If set, MP_CALENDAR (see Illustration) allows a standard menu
to be used as a as a Day View in a calendar program. Horizontal lines are drawn
between the menu items, and each item represents a calendar appointment at a particular
hour of the day. <<A small square appears in the upper left corner of a menu item
to indicate PM. No square indicates AM. Each menu item's height is proportional
to its duration.>> The device's number keys are used to select the hour of an appointment,
and the application can <<set>> menu-item text describing the appointment. <<A triangle
on the right edge of the menu indicates the current time.>>

    MP_AUTOSCROLLTIME  -  If set, specifies that auto-scroll is set in calendar
view. This only happens when IMENUCTL_SetActive() is called with bActive=TRUE, and
the menu is not currently active. An IMENUCTL_Redraw() after this is also necessary.
*

    The following attributes are defined only for the standard menu or soft key
control objects -
~

    MP_BI_STATE_IMAGE -  If set, 2 state image with no framing (unsel/sel).

    MP_TRI_STATE_IMAGE -  If set, 3 state image with no framing (unsel/sel/pressed).

    MP_MAXSOFTKEYITEMS  -  Shows maximum number of soft key items per screen.

    Property only valid for Softkey and plain menu control

    MP_MULTI_SEL -  If set, Multiple pressed entries are allowed.
*

Return Value:
    None

Comments:
    To reset/clear the properties, set dwProps = 0. For example:
    IMENUCTL_SetProperties(pME, 0)

Version:
   Introduced BREW Client 1.0

See Also:
    IMENUCTL_GetProperties()

=============================================================================

IMENUCTL_SetRect()

Description:
    This function sets the coordinates of the control rectangle of the menu control
object. This control rectangle represents the area on the device screen where the
menu is drawn. The values of the control rectangle are stored within the menu control
object. This function is used to change the value of the control rectangle to a
new one.

Prototype:
    void IMENUCTL_SetRect(IMenuCtl * pIMenuCtl, const AEERect * prc)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    prc  :  Control rectangle for the menu control object. This rectangle specifies
the coordinates on the device screen where the menu is to be drawn.

Return Value:
    None

Comments:
    By default, the control rectangle coordinates stored in the menu control object
correspond to the entire device display screen. Use IMENUCTL_SetRect() to set the
control rectangle coordinates to a new value. If no other menu draw operations follow
this function, use IMENUCTL_Redraw() for the new control rectangle coordinates to
take effect.

    For a soft key menu, the default control rectangle encompasses the entire device
screen, and the soft key menu is drawn at the bottom of the device screen. When
using IMENUCTL_SetRect() to set the control rectangle to a new area, such as the
top half of the device screen, the soft key menu is placed at the bottom of new
control rectangle area, in this case in the middle of the screen.

Version:
   Introduced BREW Client 1.0

See Also:
    AEERect
    IMENUCTL_GetRect()

=============================================================================

IMENUCTL_SetSel()

Description:
    This function makes a menu item the current selection of the menu control object
and requests a redraw if the MP_NO_REDRAW property is not set.

Prototype:
    void IMENUCTL_SetSel(IMenuCtl * pIMenuCtl, uint16 nItemID)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    nItemID  :  Menu item identifier used when this menu item is added to the menu.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    IMENUCTL_GetSel()

=============================================================================

IMENUCTL_SetSelEx()

Description:
    This function must be used only with Multi Select menus (MP_MULTI_SEL). This
function will set an item to be selected/unselected depending on the bSelected parameter.
TRUE will set this item to be selected while FALSE will set the item to be unselected.
To set all the items to a selected/unselected state, ensure none of your menu items
have an ID of (uint16)-1 and pass in (uint16)-1 as the wID parameter. All the items
in the menu will be set according to the value of the bSelected parameter. If you
have an item with the (uint16)-1 value only this Menu Item will be set according
to the bSelected parameter.

Prototype:
    void IMENUCTL_SetSelEx
    (
    IMenuCtl * pIMenuCtl, 
    uint16 wID, 
    boolean bSelected
    )

Parameters:
    pIMenuCtl :  Pointer to IMenuCtl Interface object
    wID :  ID of the item to set.
    bSelected :  Select/Deselect the item.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    None

=============================================================================

IMENUCTL_SetStyle

Description:
    This function sets the display style for the menu items. Different styles can
be set for normal (not selected) and selected menu items. You can also use the function
to change the menu cursor size.

Prototype:
    void IMENUCTL_SetStyle
    (
    IMenuCtl * pIMenuCtl,
    AEEItemStyle * pNormal,
    AEEItemStyle * pSel
    )

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    pNormal  :  Style for normal menu items.
    pSel  :  Style for selected menu items.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    AEEItemStyle

=============================================================================

IMENUCTL_SetTitle()

Description:
    This function sets the title of a menu control object. The text string identifying
the title can be specified from a resource file or by using the pText parameter.
    If pText is a valid string, the function uses this string as the title name.
    If pText is NULL, the function reads the string corresponding to the given resource
identifier, wResID, from resource file and sets it as title name.
    If the text string and the resource information are invalid, the function fails.
    If the title string in the pText field and the resource file information are
both valid, the pText parameter field takes precedence and is used for the menu
title.

Prototype:
    boolean IMENUCTL_SetTitle
    (
    IMenuCtl * pIMenuCtl,
    const char * pszResFile,
    uint16 wResID,
    AECHAR * pText
    )

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    pszResFile  :  NULL-terminated string containing resource file name.
    wResID  :  Resource identifier.
    pText  :  NULL-terminated character string to be used for the menu title.

Return Value:
    TRUE:  if successful.
    FALSE, if unsuccessful.

Comments:
    This function does not update the device screen when invoked. For the new title
to take effect on the device screen, use IMENUCTL_Redraw().
    The title of a soft key control menu is not shown.

Version:
   Introduced BREW Client 1.0

See Also:
    IMENUCTL_Redraw()

=============================================================================

IMENUCTL_Sort()

Description:
    This function sorts the items in a menu. The sort type is specified by the caller
and can be applied to either the name or user data. The function updates the screen
if the menu MP_NO_REDRAW flag is unset.

Prototype:
    void IMENUCTL_Sort(IMenuCtl * pIMenuCtl, AEEMenuSortType t)

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    t :  Sort type.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    AEEMenuSortType
    IMENUCTL_SortCustom()

=============================================================================

IMENUCTL_SortCustom()

Description:
    This function sorts a menu using a user-supplied comparison callback, allowing
menus to be sorted by criteria other than simple name/value comparisons. The function
updates the screen if the menu MP_NO_REDRAW flag is unset.

Prototype:
    void IMENUCTL_SortCustom
    (
    IMenuCtl * pIMenuCtl,
    PFNMENUSORTCOMPARE pfn,
    void * pUser
    )

Parameters:
    pIMenuCtl  :  Pointer to the IMenuCtl Interface object.
    pfn :  User sort comparison callback.
    pUser :  Pointer to user context passed to comparison function.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.1

See Also:
    PFNMENUSORTCOMPARE
    IMENUCTL_Sort()

=======================================================================

IMENUCTL_GetStyle()

Description:
    This function Gets the display style for the menu items.

Prototype:
    void IMENUCTL_GetStyle(IMenuCtl * pIMenuCtl, AEEItemStyle * pNormal, AEEItemStyle * pSel)

Parameters:
    pIMenuCtl : Pointer to the IMenuCtl Interface object
    pNormal : Style for normal menu items
    pSel : Style for selected menu items

Return Value:
    None

Comments:
    None

Side Effects:
    None

Version:
   Introduced BREW Client 3.1

See Also:
    None.

=======================================================================

IMENUCTL_GetColors()

Description:
    This function allows the caller to get the color of menu elements.

Prototype:
    void IMENUCTL_GetColors(IMenuCtl * pIMenuCtl, AEEMenuColors * pc);

Parameters:
    pIMenuCtl : Pointer to the menu control interface object.
    pc : Color values

Return Value:
    None

Comments:
    None

Side Effects:
    None

Version:
    Introduced BREW Client 3.1

See Also:
    AEEMenuColors

=======================================================================

IMENUCTL_GetItemRect()

Description:
    This function allows the caller to get rect used to draw that item
    in menu control.

Prototype:
    void IMENUCTL_GetItemRect(IMenuCtl * pIMenuCtl, uint16 nIdx, AEERect * prc);

Parameters:
    pIMenuCtl : Pointer to the menu control interface object.
    nIdx : Item Index
    prc : Item Rect

Return Value:
    None

Comments:
    None

Side Effects:
    None

Version:
    Introduced BREW Client 3.1

See Also:
    None


=======================================================================

IMENUCTL_AddOwnerDrawItem()

Description:
    If owner draw CB function is set by using IMENUCTL_SetOwnerDrawCB() with
ODF_ITEM_OD, this method adds an owner draw menu item to a menu and return TRUE.
Otherwise it returns FALSE. For measuring height and with of the item,
BREW calls owner draw callback function with ODA_MEASURE_ITEM. For specifying
need to redraw item, BREW calls owner draw callback function with ODA_DRAW_ITEM.
The lData is used to store a double-word data value associated with the menu
item to be created. IMENUCTL_GetItemData() can be used to get back the
parameter value.

Prototype:
    boolean IMENUCTL_AddOwnerDrawItem(IMenuCtl * pIMenuCtl, uint16 nItemID, uint32 lData)

Parameters:
    pIMenuCtl : Pointer to the IMenuCtl Interface object
    nItemID : Item identifier that uniquely identifies a menu item
    lData : Item data associated with the menu item

Return Value:
    TRUE : If successful
    FALSE : If unsuccessful

Comments:
    This function does not automatically update the screen. For the new menu
item to take effect on the device screen, use IMENUCTL_Redraw() . If the menu
is to be activated subsequent to adding menu items, IMENUCTL_Redraw() does not
need to used since IMENUCTL_SetActive() updates the screen with the new menu
items. IMENUCTL_DeleteItem() can be used to remove an item from the menu

Side Effects:
    When the newly added menu item is activated by a user, nItemID and lData
values are sent to the applet event handling function in the short-data and
long-data fields respectively.

Version:
    Introduced BREW Client 3.1

See Also:
    IMENUCTL_DeleteItem()
    IMENUCTL_Redraw()
    IMENUCTL_SetActive()


=======================================================================

IMENUCTL_SetOwnerDrawCB()

Description:
    This function sets callback function for an owner draw menu.

Prototype:
    int IMENUCTL_SetOwnerDrawCB(IMenuCtl * pIMenuCtl, PFNOWNERDRAWCB pfnCB, void * pvContext, uint16 nODFlags);

Parameters:
    pIMenuCtl : Pointer to the IMenuCtl Interface object
    pfnCB : call back function
    pvContext: Context Pointer
    nODFLags : Owner draw Flags
               ODF_TITLE_OD specifies that title is owner drawn
               ODF_SB_OD specifies that scroll bar is owner drawn
               ODF_ITEM_OD specifies that menu can contain owner drawn items
               ODF_MEASURE_ONCE specifies that all items are of same height and width so call back for
               measuring item height and width and item text height (icon view only) needs to be called
               only once

Return Value:
    None

Comments:
   Following is a typical implementation of owner drawn callback function
===pre>
   void App_MenuDraw(void* pi, AEEODCBData* is)
   {
      CApp * pMe = (CApp*)pi;
      AEERect rc;

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
            is->rc.dx = 10;
            break;

         case ODA_DRAW_SB:
            SETAEERECT(&rc, is->rc.x, is->rc.y, is->rc.dx, is->rc.dy);
            IDISPLAY_DrawRect(is->pIDisplay, &rc, RGB_BLACK, 0, IDF_RECT_FRAME);
            SETAEERECT(&rc, is->rc.x, OD_SB_POS(is->nActionInfo), is->rc.dx, OD_SB_LEN(is->nActionInfo));
            IDISPLAY_FillRect(is->pIDisplay, &rc, RGB_BLACK);
            IDISPLAY_Update(is->pIDisplay);
            break;

         case ODA_MEASURE_ITEM:
            is->rc.dx = 50;
            is->rc.dy = 50;
            break;

         case ODA_DRAW_ITEM:
            SETAEERECT(&rc, is->rc.x+1, is->rc.y+1, is->rc.dx-2, is->rc.dy-2);
            IDISPLAY_DrawRect(is->pIDisplay, &rc, RGB_BLACK, 0, IDF_RECT_FRAME);
            IDISPLAY_UpdateEx(is->pIDisplay);
            break;
      }
   }

===/pre>

Side Effects:
    None

Version:
   Introduced BREW Client 3.1

See Also:
    None

===========================================================================*/

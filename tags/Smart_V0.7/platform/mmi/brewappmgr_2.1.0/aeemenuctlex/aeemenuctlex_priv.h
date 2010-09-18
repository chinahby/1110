/*============================================================================
FILE:   aeemenuctlex_priv.h

SERVICES: private header file for aeemmenuctlex

DESCRIPTION:


PUBLIC CLASSES:  

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/* =======================================================================
                             Edit History
$Header: //brewapps/bam2/HEAD/aeemenuctlex/aeemenuctlex_priv.h#7 $
$DateTime: 2008/06/24 11:51:02 $
$Change: 406908 $

========================================================================== */
#ifndef AEEMENUCTLEX_PRIV_H
#define AEEMENUCTLEX_PRIV_H
#include "AEEGraphics.h"
#include "AEEMenuCtlEx.h"
#include "QcList.h"

typedef enum {
  OWD_USER          // owner draw by user
  ,OWD_CTL          // owner draw by aeemenuctlex
  ,OWD_NONE         // no owner draw
}OWNER_DRAW_TYPE;

OBJECT(CMenuItem)
{
  uint16         nItemID;       // menu item ID
  AECHAR       * pText;         // menu text string
  uint16         cxText;        // text string width
  uint16         cyText;        // text string height
  AEEFont        nFont;         // Font for text

  const char *   pszResText;    // Resource for Text
  uint16         wText;         // Text ID
  uint16         wFont;         // 0 - Default
  uint32         dwData;        // user's data

  const char *   pszImage;      // Resource file for cached image
  uint16         wImageID;      // Image ID in resource file
  IImage *       pImage;        // Image object
  uint16         cyImage;       // Height of image
  uint16         cxImage;       // Width of image

  OWNER_DRAW_TYPE  m_nOWDType;  // owner draw type
};

//
// Menu manager object (IMenuCtl)
//
OBJECT(CMenuCtl)
{
   AEEVTBL(IMenuCtl) *pvt;                       // v table, interfaces defined in IMenuCtl

   long               m_nRefs;                   // reference count
   AEECLSID           m_cls;                     // menu control class
   IShell            *m_pIShell;                 // IShell interface ptr
   IModule	         *m_pIModule;                // module ptr
   IDisplay          *m_pIDisplay;               // IDisplay interface ptr
   IGraphics         *m_pIGraphics;              // IGraphics ptr
   IMenuCtl          *m_pIMenu;                  // embedded menuctl ptr
   QcList            *m_pItemList;               // list of menu items, Note, the list only serves for seraching 
                                                 // item infomation, it may have differen order than m_pIMenu.

   AEERect           m_rc;                       // control rectangle
   AEERect           m_rcMenu;                   // this is the visiable rect of the menu (calculated according to displyed items)
   uint32            m_dwProps;                  // IMenuCtl properties set by user
  
   AECHAR *          m_pTitle;                   // menu title
   int               m_cyFont;                   // height of char cell of menu font 
   RGBVAL            m_cBorderFrame;             // top and bottom bar color
   uint16            m_cxItemExtra;              // Extra pixels added to width for style
   uint16            m_cyItemExtra;              // Extra pixels added to height for style

   int               m_cxMaxItem;                // Max width of a menu item
   int               m_cyMaxItem;                // Max height of a menu item
   boolean           m_bSendCmd;                 // enable sending of EVT_COMMAND event
   boolean           m_bSingleClick;             // enable/disable single click
   int               m_nCurrentItemID;           // current item ID
   int               m_nCurrentItemIdx;          // current item idx.
   AEERect           m_CurrentItemRc;            // current item rect
   boolean           m_bPtDown;                  // pointer is currently in down position
   boolean           m_bPtMove;                  // pointer moved out of the original rect

   // Owner Draw Stuff
   OWNER_DRAW_TYPE   m_nOWDType;                 // default owner draw type
   PFNOWNERDRAWCB    m_pfnCB;                    // Callback for Owner Draw
   void *            m_pvContext;                // Context for Owner Draw Callback
   uint16            m_nODFlags;                 // Owner Draw Flags

   uint16            m_wTextEncoding;            // text encoding info
   boolean           m_bHasTextEncodingInfo;     // if encoding info is available

   AEEScrollBarInfo  m_ScrollBarInfo;            // scrollbar info
   AEEMenuExItemInfo m_ItemInfo;                 // item draw info
   boolean           m_bScrollBar;               // TRUE, when scrollbar is currently displayed
   AEERect           m_rcScrollBar;              // scroll bar rect

   boolean           m_bAutoScroll;              // text string is longer than item rc, need shifting
   boolean           m_nAutoScrollIdx;           // starting idx
   uint32            m_wMenuTextScroll;          // Auto Scroll - Update Timer
   AEERect           m_rcAutoScroll;             // scrolling rc

   int16             m_nPointerPosX;             // pointer X pos
   int16             m_nPointerPosY;             // pointer Y pos
};


#endif //AEEMENUCTLEX_PRIV_H



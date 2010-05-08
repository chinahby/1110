#ifndef APPMGRUTILS_H
#define APPMGRUTILS_H

#include "AEEDownload.h"
#include "AEE.h"
#include "AEEMenu.h"

/*======================================================

                    IMPORTANT NOTICE

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement, 
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  AppMgrUtils.h

SERVICES:  This files has common stuff between AppMgr and MobileShop
   applets.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
	List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 2000-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "MobileShop.bid"
#include "AppMgr.bid"
#include "appmgrln_res.h"
#include "appmgrls_res.h"

#define			CONFIG_FILE_VER_1          "ver:1"
#define			CONFIG_FILE_VER_2          "ver:2"
#define			CONFIG_FILE	               "config.txt"
#define			CLSID_LENGTH			      11

#define        MARGIN                     3

#define        ONE_K                      (1024l)
#define        ONE_MB                     (ONE_K * ONE_K)
#define        ONE_GB                     (ONE_K * ONE_K * ONE_K)

#define        SECS_PER_DAY               (60l * 60l * 24l)

#define        PREV_ENTRY                 -1 // use the previous item
#define        NO_ENTRY                   -2 // cannot go BACK on this item

// Color Mask
#define        MENU_COLOR_MASK 	         (MC_BACK | MC_SEL_BACK | MC_SEL_TEXT)

// 256 Color Menu
#define        COLOR_MENU_BACKGROUND	         RGB_WHITE
#define        COLOR_MENU_SELECT_BACKGROUND	   MAKE_RGB(192, 192, 192)
#define        COLOR_MENU_SELECT_TEXT	         RGB_BLACK

// 256 Color Softkey
#define        COLOR_SK_BACKGROUND	            MAKE_RGB(192, 192, 192)
#define        COLOR_SK_SELECT_BACKGROUND	      MAKE_RGB(192, 192, 192)
#define        COLOR_SK_SELECT_TEXT	            RGB_BLACK

// 256 Color IconView
#define        COLOR_ICON_BACKGROUND	         RGB_WHITE
#define        COLOR_ICON_SELECT_BACKGROUND	   MAKE_RGB(192, 192, 192)
#define        COLOR_ICON_SELECT_TEXT	         RGB_BLACK

// 4Grey/Mono Menu/Softkey Colors
#define        MENU_BACKGROUND	               RGB_WHITE
#define        MENU_SELECT_BACKGROUND	         RGB_BLACK
#define        MENU_SELECT_TEXT	               RGB_WHITE

#define        MAX_STR_CHARS                    (512)
#define        MAX_STR_SIZE                     (sizeof(AECHAR) * MAX_STR_CHARS)

#define        ADD_MENUITEM_TIMER               2
#define        ITEM_HEIGHT                      12

// Macro that initilizes state handlers in state machine
#define SET_STATE(p,nIdx,pfn,nb,bc)      {(p)->m_states[(nIdx)].pfnReq = pfn; (p)->m_states[(nIdx)].nBack = nb; (p)->m_states[(nIdx)].bClear = bc;}

// Configuration Version
#define MSHOP_CFG_VER   0x0001

// Configuration Struct
typedef struct MShopCfg
{
   flg      bSearch:1;     // Search for apps
   flg      bUpgrade:1;    // Upgrade Apps
} MShopCfg;

AECHAR *       AppendWStr(AECHAR * pszIn, AECHAR * pszAdd, AECHAR chAppend);
AECHAR *       LoadAppNames(IShell * pIShell, AEEAppInfo * pai, int nCount);
boolean        GetDateString(IShell * ps, uint32 dwDate, AECHAR * pszDest, int nSize);
boolean        GetExpireString(IShell* pIShell, const char* pResFile, uint32 dwNow, AEELicenseType lt, uint32 dwValue, AECHAR * pszDest, int nSize, boolean bShowInvalid, boolean bAdjust);

void           DrawHist(IDisplay * pd, AEERect * prc, uint16 wPct);
void           FormatMem(uint32 dwSize, AECHAR * pszDest,boolean bWrap);
AECHAR *       FormatString(IShell * ps,const char * pszRes, uint16 wFormat, AECHAR * psz);
AECHAR *       FormatStringEx(IShell * ps, const char * pszRes, uint16 wFormat, AECHAR * psz1, AECHAR * psz2);
void           FreePtr(void ** ppObj);
void           FreeObj(void ** ppObj);
void           SetMenuStyleAndColor(IMenuCtl* pMenu, AEECLSID nMenuCls, uint16 nColorDepth);
DLITEMID       GetItemIDFromClassID(IDownload * pIDownload, AEECLSID cls);

char* AllocSecretString(
   IShell*  piShell,
   boolean  bUsePrevWindow);

#endif // APPMGRUTILS_H

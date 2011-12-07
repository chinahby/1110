#ifndef COMMON_H
#define COMMON_H

#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEAppGen.h"
#include "AEEShell.h"           // Shell interface definitions
#include "AEEMenu.h"            // Menu    interface defintions
#include "AEERinger.h"         //  ringer interface

#if defined(AEE_STATIC)
//#include "AppComFuncs.h"       // set system menu common propertly
#include "uiutils.h"           // button data
//#include "AEEMenuExt.h"        // menu control exterinterface
#include "Appscommon.h"

#endif  //AEE_STATIC
////////////////////////////////////////////
//key value definition
///////////////////////////////////////////
#if defined(AEE_STATIC)
#define MEDIA_PATH            "gamemusic"                 //save data path
#else

#define MEDIA_PATH            ""                          //save data path
//key define
#define AVK_F0							AVK_SOFT1
#define AVK_F1							AVK_SOFT2
#define AVK_F2							AVK_SOFT3
#define AVK_F3							AVK_SOFT4
#endif //AEE_STATIC


#define PROMPT_INFO_HEIGHT       20  //prompt information height   
#define DISP_POPMENU_ITEM_NUM    4   //num of popmenu item
#define MAX_CONTENT_SIZE         64 
#define MAX_HELP_INFO_SIZE       1024  //the size of help information buffer
#define MAX_TITLE_SIZE_LEN       AEE_MAX_FILE_NAME


// Based on Menu style sheet:
#define MENU8_FT                 AEE_FT_NONE
#define MENU8_SELECT_FT          AEE_FT_BOX
#define MENU8_RO                 AEE_RO_TRANSPARENT
#define MENU8_SELECT_RO          AEE_RO_TRANSPARENT
#define MENU8_COLOR_MASK         (MC_BACK | MC_SEL_BACK | MC_SEL_TEXT | MC_TITLE)
#define MENU8_BACKGROUND         MAKE_RGB(255,255,225)
#define MENU8_SELECT_BACKGROUND  MAKE_RGB(128,128,64)
#define MENU8_SELECT_TEXT        RGB_WHITE
#define MENU8_TITLE              MAKE_RGB(39,222,75)
#define TB8_BACKGROUND           MAKE_RGB(192,192,192)
#define TB8_SELECT_BACKGROUND    MAKE_RGB(0,128,0)
#define SOTFKEY_BACKGROUND       MAKE_RGB(0,128,0)
#define SOTFKEY_SEL_BACKGROUND   MAKE_RGB(0,128,0)
#define MENU_BAR_FILLCOLOR       MAKE_RGB(0,128,255)
#define MENU_SCROOL_BAR          RGB_WHITE
//set popMenu color
#define POPMENU_FT                  AEE_FT_NONE
#define POPMENU_SELECT_FT           POPMENU_FT
#define POPMENU_SELECT_TEXT         MENU8_SELECT_TEXT
#define POPMENU_UNSELECT_TEXT       MAKE_RGB(0,128,255)
#define POPMENU_SELECT_BACKGROUND   MAKE_RGB(0,128,255)
#define POPMENU_BACKGROUND          RGB_WHITE
#define POPMENU_FRAME_LINE          MAKE_RGB(144,0,0)

//grid line color define
#define GRID_R_COLOR            0
#define GRID_G_COLOR            0
#define GRID_B_COLOR            0

#define BOTTOM_PROMPT_TEXE_COLOR MAKE_RGB(128,255,255)  //提示当前时间字体颜色

typedef enum
{
    DROP_PIECE_MEDIA,               //down speed sound type
    DANGER_MEIDA,                   //clean one line or mult Line sound type
    FAILURE_MEDIA,                  //fail chess
    SUCCESS_MEIDA                   //game over sound type

}MediaType;


extern void    Brew_SetMenuAttr(IMenuCtl * pMenu, AEECLSID clsMenu, AEERect * pRect, uint32 dwProps);
extern boolean Brew_AddMenuItem(IMenuCtl * pMenu ,char *pszResFile,uint16 wTextID,
                                AECHAR * pText, uint16 wImageID,
                                uint16 wItemID, uint32 dwData);
extern void    Brew_AddSoftItem(IMenuCtl *pMenu,char *pszResFile,AEERect *pRect, uint16 wResID, uint16 nItemID, uint16 wResID2, uint16 nItemID2);
extern boolean UpAndDownScroll_HandleEvent(IShell* ps, void* p, AEEEvent eCode, uint16 wParam);
extern boolean Common_CreateObj(IShell *pIShell, AEECLSID clsID, void **ppObj);
extern void    Common_ReleaseObj(void ** ppObj);

extern boolean JudgeHaveNoneASCIIChar ( AECHAR * pWstr, int * pPostion);
extern void PlayerSound(IRingerMgr* pRingerMgr,MediaType type);

#endif /*COMMON_H*/
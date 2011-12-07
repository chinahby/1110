 /******************************************************************************
FILE:  PinTu.h

AUTHOR: jhaihui

         Copyright (C) 2004-2005 ...Software Co., LTD
                     All Rights Reserved.
******************************************************************************/
#ifndef  CHINA_CHESS_H
#define  CHINA_CHESS_H

#include "AEE.h"
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEDisp.h"            // Display interface defintions
#include "AEEMenu.h"            // Menu    interface defintions
#include "AEEGraphics.h"        // Graphics interface defintions
#include "AEEStdLib.h"
#include "AEEFile.h"            //file interface
#include "AEEDB.h"
#include "AEEImageCtl.h"        //image ctrl interface
//#include "PinTu_res.h"
#include "Appscommon.h"
#include "pintu.brh"
#include "AEEAnnunciator.h"
#include "err.h"
#include "Common.h"             //common function defintions

#if defined(AEE_STATIC)
#include "OEMClassIDs.h"        //applet classID definitions
//#include "OEMMacros.h"
#else
#include "PinTu.bid"
#endif  /*AEE_STATIC*/

//*****************************************************************************
// DEFINITIONS
//*****************************************************************************

//For BREW3.0 support the muti-language re-define the RES maco
#define PINTU_RES_FILE_LANG      AEE_RES_LANGDIR PINTU_RES_FILE

#if defined(AEE_STATIC)
#define MAX_CATALOG_NUM    2                  //number of list catalog
#define AMUSEMENT_DIR         "shared/img"
#define PRIVATE_DIR           "image/wallpaper"
#define DOWNLOAD_IMAGE_DIR    PRIVATE_DIR //"shared/comm/download/photos"      //the download image by network


#else
#define MAX_CATALOG_NUM    2                  //number of list catalog
#define AMUSEMENT_DIR         "shared/img"
#define DOWNLOAD_IMAGE_DIR    "bmp"
#endif /*AEE_STATIC*/
#define PINTU_RING_DIR        AMUSEMENT_DIR     //the image of phone mobile


#define DIRECTORY_CHAR	   '/'//路径字符
#if defined(AEE_STATIC)
#define PINTU_SOTFKEY_CY               BOTTOMBAR_HEIGHT  //softkey height
#else
#define PINTU_SOTFKEY_CY               23  //softkey height
#endif //AEE_STATIC
 
#define MAX_CONTENT_SIZE         64            
#define MAX_HELP_INFO_SIZE       1024         //the size of help information buffer
#define BASE_BMP_ITEM_ID   400                //list image base menu item ID
#define MAX_FILES_NUM      100                //list the max file number
#define MAX_FILENAME_LEN   64                 //len of list file name
#define DISP_POPMENU_ITEM_NUM    4            //num of popmenu item

#define  MAX_RAND_NUM      300                //max rand number
#define  MOVE_DIRTORY_NUM  4

#define  IMAGE_BEGIN_X     0                  // from begin x of selected image
#define  IMAGE_BEGIN_Y     0                  // from begin y of selected image  
#define  SCREEN_X_OFFSET   5                  // draw block in screen x offset   
#define  SCREEN_Y_OFFSET   5                  // draw block in screen y offset   
#define  MIN_MOVE_NUM      1                    
#define  BETWEEN_LEN       1                  // between len of block cell

//define the data of parameter of spelling
#define  SETTING_PARAM_ITEM_NUN  4            // add menu item in the state of setting parameter   
#define  SINGLE_SELECTED         0x01
#define  LIMIT_THREE_MINTUES     0x01
#define  LIMIT_FIVE_DEGREES      0x02
#define  LIMIT_TRHEE_AND_THREE   0x04
#define  DISP_NUMBER_LABLE       0x08
#define  LIMIT_FOUR_AND_FOUR     0x10


#define  MAX_VIEW_PIC_TIME       5       // preview the picture times during game doing

#define  MAX_GRID_COL            4       // max grid col during the game
#define  MAX_GRID_ROW            4       // max grid row during the game    

#define  NUMBER_HEIGHT           8       // height pels
#define  NUMBER_WIDTH            6       // width  pels
#define  NUMBER_ONE_WIDTH        4       // width of number one
#define  EXIT_BUTTON_WIDTH       12
#define  EXIT_BUTTON_HEIGHT      12


//define event deal
#define USRE_DRAW                100     //event of redraw the screeen
#define SOFTKEY_PROMPT_DRAW      101     //event of draw softkey prompt
//////////////////////////////////////////
//time data definition
//////////////////////////////////////////
#define DISP_PREVIEW_PIC_TIMER   2000    // preview the picture time during the spelling block 
#define GAME_OVER_TIMER          500        
#define TITLE_FILE_NAME_AUTO     200     // title name auto run time in ms
#define ALRAM_TIMER              1200    // timer of alaram information
#define RECORD_TIMER             (3*60)  // record count down time in second
#define SPLASH_TIMER_DURATION    1000    //display the main image timer
//
#define MAX_MENU_LEVEL           3       // max menu level 
#define CFG_VERSION              4//3       // record config version of current applet


//#define LEFT_KEY_ID       1     
//#define MIDDLE_KEY_ID     2
//#define RIGHT_KEY_ID      4

//
// private color definition
//
#define BACK_GROUND_COLOR       MAKE_RGB(0,128,64)
#define EMPTY_GRID_COLOR        MAKE_RGB(0,128,255)
#define   PARAM_NOT_REF(x)
#define SetUniMenuConfig(p)         //do nothing SetUniMenuConfig(p)
#define SetUniTextConfig(pT)        //do nothing SetUniTextConfig(pT)

//Predefine some common function name
#define CreateObj      Common_CreateObj     //create obj
#define ReleaseObj     Common_ReleaseObj    //release obj

// The application state machine values

typedef enum
{
  APP_STATE_MAIN_MENU,      //main menu
  APP_STATE_START,          //start new game
  APP_STATE_MEDIA_OPTION,   //select backgroud ringer
  APP_STATE_IMAGE_SELECT,   //select image to pintu
  APP_STATE_IAMGE_PREVIEW,  //preview selected image
  APP_STATE_SETTING,        //set paramter
  APP_STATE_HELP,           //view help information
  APP_STATE_DISP_INFO,      //alert information
  APP_STATE_ABOUT,          //about the game
  APP_STATE_SOUNDSET,       //set sound by tcl wgs added 050120 
  LAST_STATE                //

} EAppStateType;

//definition alaram information  type
typedef enum
{
    PRVIEW_IMAGE,            
    NO_FOUND_IMAGE,        //select image list  
    AVAIBLE_IMAGE          //start new game   
    
} AlaramType;

//define grid information

typedef struct _gridInfo
{
   uint16 begin_x;
   uint16 begin_y;
   uint16 width;
   uint16 height;
   uint16 ID;
}gridInfo;


typedef struct _site
{
   int16 x;
   int16 y;
}site;


//definition the information of make block
typedef struct _EreaInfo
{
    int16 row;
    int16 col;
    int16 max_block;
    int16 screen_xOffSet;
    int16 screen_yOffSet;
}EreaInfo;

//definition config data struct

typedef struct _CPrefData
{
    boolean         flag;
    uint16   settingWparam;    //bak the config of setting parameter
    char           imagename[64];
}PrefData;
//
//typedef struct _CPinTuPrefData
//{
//   boolean         flag;
//   char           imagename[20];         //save data of the first go
// 
//}CPinTuPrefData;

#ifndef AEE_STATIC

//define softkey prompt struct
typedef struct _SoftkeyPrompt
{
   uint16  leftItemID;      //left softkey
   uint16  middleItemID;    //middle softkey
   uint16  rightItemID;     //right softkey
   boolean keyAvaibleFlg;
   AVKType type;
   
}SoftkeyPrompt;

#endif //AEE_STATIC


//*****************************************************************************
// GLOBAL APPLICATION DATA STRUCTURES
//*****************************************************************************
// All the data and GUI object pointers need for controls that are not
// defines in the application's resource file
typedef struct _PinTu PinTu;

struct _PinTu
{
   AEEApplet      a ;            // Manditory AEEApplet data member appears first
   IGraphics*     m_pGraphics;   //graphics interface
   //App specific data   
   IMenuCtl*      m_pMenuCtl;    //menu menu
   IStatic*       m_pStaticCtl;             

   IImageCtl*     m_pImageCtl;           // image control for move the selected in the image list    
   IImage*        m_pSrcImage;
   IImage*        m_pViewSelectImage;    // preview the selected image in the list image   
   IImage*        m_pTopBGImage;         // draw background bmp of title
   IImage*        m_pBottomImage;        // draw background bmp of softkey
   IImage*        m_pNumberLableImage;   // draw the number lable  
   IRingerMgr*    m_promptVoice; // chess piece operation for drop piece   

   AEEDeviceInfo  m_dInfo;                        // device information
   int            m_nNChSize;                     // normal char size

   // Physical screen
   int16          m_cxScreen;                       // Physical screen size (pixels)
   int16          m_cyScreen;                       // Physical screen size (pixels)
   EAppStateType  m_eAppState;                      // machine state of handle
   boolean        m_continueFlg;                    // flag of continue game
   boolean        m_endGameFlg;                     // flag of end game   
   uint16         m_Resultarray[MAX_GRID_COL*MAX_GRID_ROW];
   gridInfo       m_curGrid;                        //current grid position
   gridInfo       m_historyGrid;                    //history grid position   
   AEERect        m_popWinFrameRect;                //rect of pop menu frame   
   int16          m_siteinfo[MAX_GRID_ROW][MAX_GRID_COL];
   uint16         m_selImageItemID;                 //current selected image item id
   uint16         m_lastSelImageID;                 //last selected image item id
   char           m_loadImageFile[MAX_FILENAME_LEN];//save load selected image file from image list
   uint16         m_firstItemID[MAX_CATALOG_NUM];   //record first item id of file from different catalogs
   boolean        m_keyAvaible;                     //judge key of up down right left control availble
   int16          m_menuLevelStack[MAX_MENU_LEVEL]; //record menu level select item id
   AECHAR         m_szwinTitle[MAX_TITLE_SIZE_LEN];
   int16          m_girdHeight;                     //heigth of gird
   int16          m_girdWidth;                      //width of gird
   int16          m_moveStep;                       //total step of move
   int16          m_recordClock;                    //record clock   
   uint16         m_paramAttr;                      //attribute of parameter
   uint16         m_paramHistoryAttr;
   int16          m_viewOrigPicTime;                //time of view the originality picture

   //uint8         *m_pStateKeyID;                  // different state softkey ID include left,middle,right 
   //uint16         m_leftKeyMenuItemID;   
   EreaInfo       m_arrayInfo;                          
   AlaramType     m_promptType;                     // type of prompt user
   PrefData       m_CfgData; 
  // CPinTuPrefData      m_PinTuCfgData;                       // struct of saving config data   
#if defined(AEE_STATIC)   
   BottomBar_Param_type      m_BottomBar;                      // bottom variable for screen 
#else
   SoftkeyPrompt  m_softkeyPrompt;                  // softkey prompt content   
   int16          m_topTitleLen;                    //top title wstren len   
   boolean        m_AllASCIICharFlg;   
#endif  //AEE_STATIC   
   boolean        m_Soundflag;                      //added by tcl wgs the flag of backsound
   IAnnunciator   *m_pIAnn;
};

//bottom bar of definition 
#if defined(AEE_STATIC) 
// t 表示提示条的类型
#define DRAW_PROMPT_BAR(x)                      \
        {                                          \
            BottomBar_Param_type BarParam;                  \
    		MEMSET(&BarParam, 0, sizeof(BarParam));         \
   			BarParam.eBBarType = x;                         \
    		DrawBottomBar(pMe->a.m_pIDisplay, &BarParam);      \
        }
#endif //AEE_STATIC

#endif /* CHINA_CHESS_H */

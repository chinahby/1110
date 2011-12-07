 /******************************************************************************
FILE:  GTFive.h

AUTHOR: jhaihui

         Copyright (C) 2003-2003 ...Software Co., LTD
                     All Rights Reserved.
******************************************************************************/
#ifndef  _GMFIVE_H


#include "AEE.h"
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEDisp.h"            // Display interface defintions
#include "AEEMenu.h"            // Menu    interface defintions
#include "AEEGraphics.h"        // Graphics interface defintions
#include "AEEStdLib.h"
//#include "gmfive_res.h"
#include "Appscommon.h"
#include "gmfive.brh"
#include "AppComFunc.h"
#if defined(AEE_STATIC)
#include "OEMClassIDs.h"        //applet classID definitions
//#include "OEMMacros.h"

//in VC building force the cl.exe to us search the file
// before include "AEEConfig.h" in \pk\inc
#ifdef AEE_SIMULATOR
#include "OEMConfig.h"          
#endif // AEE_SIMUALTOR
#include "AEEConfig.h"          //congfig interface defintions
#include "AEESound.h"           //sound interface
//#include "OEMSoundx.h"
#else
#include "gmfive.bid"
#endif  /*AEE_STATIC*/
#include "Common.h"
#include "err.h"
#include "AEEAnnunciator.h"

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
//For BREW3.0 support the muti-language re-define the RES maco
#define GMFIVE_RES_FILE_LANG  AEE_RES_LANGDIR GMFIVE_RES_FILE

//#define ONLY_MAN_CHESS_DEBUG              //test for only black piece chess
#define  SOUND_FEATURE                    //feature of playing sound

#ifdef  SOUND_FEATURE
#include "AEEFile.h"			// AEEFile Services
#define MAX_RING_LEN          64


//#define AMUSEMENT_DIR         "shared/music"
#define DOWNLOAD_RIN_DIR      "ringers"
#define GMFIVE_RING_DIR       "shared/music" //AMUSEMENT_DIR               //the ringer of phone mobile

#define DIRECTORY_CHAR		   '/'//路径字符
#define BASE_SOUND_ITEM_ID    200
#define GMFIVE_MAX_FILES_NUM  AEE_MAX_FILE_NAME
#define MAX_SOUND_MODE        2
#endif /*SOUND_FEATURE*/

#define GAME_OVER_DISP_TIMER     1000  //the timer of displaying game result information


///////////////////////////////////////////
//time data definition
//////////////////////////////////////////
#define SPLASH_TIMER_DURATION    1000  //display the main image timer
#define ALERT_DISPLAY_TIMER      2500
#define SPLASH_PIECE_TIMER       600    //alternation timer of splashing piece
#define TITLE_FILE_NAME_AUTO     200    // title name auto run time in ms
#if defined(AEE_STATIC)
#define GMFIVE_SOTFKEY_CY        BOTTOMBAR_HEIGHT     //softkey height
#else
#define GMFIVE_SOTFKEY_CY        23     //softkey height
#endif //AEE_STATIC


//chess broad information
#define   MAX_LEVEL              4     // 游戏最高级别         Don't modify
#define   INIT_LEVEL             1     // 游戏初始级别         1 ~ 4
#define   PIECE_WIDTH            10    // 棋子宽度             Don't modify
#define   PIECE_HEIGHT           10    // 棋子高度             Don't modify
#define   GRID_WIDTH             12    // 棋盘栅格宽度         Don't modify
#define   GRID_HEIGHT            12    // 棋盘栅格高度         Don't modify
#define   X_OFF                  10    // 棋盘起点 X 坐标.     Can modify freely,but must more than PIECE_WIDTH/2.
#define   Y_OFF                  7     // 棋盘起点 Y 坐标.     Can modify freely,but must more than PIECE_HEIGHT/2.
#define   CHESS_LEN_X            14    // 棋盘 X 方向格子数+1. Can modify freely.
#if defined(AEE_STATIC)
#define   CHESS_LEN_Y            15    // 棋盘 Y 方向格子数+1. Can modify freely.
#else
#define   CHESS_LEN_Y            15    // 棋盘 Y 方向格子数+1. Can modify freely.
#endif//AEE_STATIC
#define   MAX_CHESS_PIECE        (CHESS_LEN_X*CHESS_LEN_Y/2)         //max played pieces of anyone

#define   DIS_END_X              (X_OFF+GRID_WIDTH*(CHESS_LEN_X-1))  //the end place of x direction
#define   DIS_END_Y              (Y_OFF+GRID_HEIGHT*(CHESS_LEN_Y-1)) //the end place of y direction

#define   WHITE_CHESS            0    // white color chess
#define   BLACK_CHESS            1
#define   OTHER_CHESS            2

#define   MAX_CHESS_MODE         2
#define   MAN                    1    // 人   Don't modify
#define   MACHINE                2    // 机   Don't modify
#define   INIT_WIN               0    // 初始 Don't modify
#define   MAN_WIN                1    // 人嬴 Don't modify
#define   MACHINE_WIN            2    // 机嬴 Don't modify
#define   ALL_NO_WIN             3    // 和棋 Don't modify

#define   NEW                    1    // 新局 Don't modify
#define   ESCAPE                 2    // 退出 Don't modify
#define   CHECK                  3    // 查看 Don't modify

#define   THRESHOLD              10   // 防守门槛 Don't modify
#define   DEEP                   12   // 预测深度 Can modify, but must more than 5.
#define   MANY                   8    // 危棋限数 Can modify, but must more than 2.

#define   REPENT_LEN             6    // 最大悔棋次数+1 Can modify, but must more than 2.
//define event deal
#define USRE_DRAW                100  //event of redraw the screeen
#define MACHINE_PLAY_CHESS       102  //event of machine play chess fall

//added by TCL wangbin-2004-03-25
#define START_ROW   7    // 棋子开始位置x  Can modify, but must more than 5.
#define START_COL   5    // 棋子开始位置y   Can modify, but must more than 5.
//end TCL wangbin-2004-03-25

//current square color
#define SQUARE_R_COLOR          0
#define SQUARE_G_COLOR          0
#define SQUARE_B_COLOR          128
//splash piece color
#define SPLASH_R_COLOR          0
#define SPLASH_G_COLOR          0
#define SPLASH_B_COLOR          255

#define BACK_GROUND_COLOR       MAKE_RGB(255,155,106)
#define BACKGD_R_COLOR          255
#define BACKGD_G_COLOR          155
#define BACKGD_B_COLOR          106

#define GAME_VERSION            4  //definition version of current game pref.dat

//softkey id definition
#define LEFT_KEY_ID       1     
#define MIDDLE_KEY_ID     2
#define RIGHT_KEY_ID      4


//Predefine some common function name
#define CreateObj    Common_CreateObj    //create obj
#define ReleaseObj   Common_ReleaseObj   //release obj
#define   PARAM_NOT_REF(x)
#define SetUniMenuConfig(p)         //do nothing SetUniMenuConfig(p)
#define SetUniTextConfig(pT)        //do nothing SetUniTextConfig(pT)


//state of applet
typedef enum
{
  APP_STATE_MAIN_MENU,            //display main menu
  APP_STATE_NEWGAME,              //new game
  APP_STATE_SELECT_CHESS_MODE,    //play chess mode
  APP_STATE_SELECT_CHESS_LEVEL,   //chess level
  APP_STATE_HERO_SCORE,           //hero score
#ifdef  SOUND_FEATURE
  APP_STATE_SELECT_RINGER,        //select background ringer file
  APP_STATE_SOUND_MODE,           //set sound for colse or open
#endif
  APP_STATE_REPREND_GAME,         //repend game
  APP_STATE_HELP,                 //display user help
#if defined(AEE_STATIC)
#else
  APP_STATE_ABOUT,                //exit applet state
#endif
  APP_STATE_DISP_INFO,            //display common alert information
  LAST_STATE                      //  

} EAppStateType;


typedef struct _checkPiece checkPiece;

struct _checkPiece
{
   int16 row;                    //row the piece
   int16 col;                    //col the piece
};

typedef struct
{
   int16 man_x[REPENT_LEN];      //x coordinate of man's piece
   int16 man_y[REPENT_LEN];      //y coordinate of man's piece
   int16 machine_x[REPENT_LEN];  //x coordinate of machine's piece
   int16 machine_y[REPENT_LEN];  //y coordinate of machine's piece
   int16 head_point;             //head point
   int16 tail_point;
   int16 tail_ptr_var;
   int16 total;   // 用来在悔棋时,判起始, 等于当前人下的棋数.
}  REPENT_QUEUE;

typedef struct
{
   char map[CHESS_LEN_Y][CHESS_LEN_X]; // 0 : null, MAN, MACHINE
   uint16 attack_value[CHESS_LEN_Y][CHESS_LEN_X];
   uint16 defence_value[CHESS_LEN_Y][CHESS_LEN_X];
   char who_go;                           // MAN, MACHINE
   char who_win;                          //who win the game
   char begin_flag;                       // 0 : null, MAN, MACHINE
   byte particular;
   byte level;                           //the level of current game
   uint16 put_chess_counter;             // 用来判和棋,等于当前机下的棋数.
   REPENT_QUEUE repent_queue;

}  CHESS_OBJECT;

//config information
typedef struct _CPrefData  CPrefData;

struct _CPrefData
{
   uint16         levelScore[4];
   char           who_go;         //save data of the first go
   char           level;          //save data of level  
};


//define softkey prompt struct
typedef struct _SoftkeyPrompt
{
   uint16 leftItemID;      //left softkey
   uint16 middleItemID;    //middle softkey
   uint16 rightItemID;     //right softkey
   boolean keyAvaibleFlg;
   AVKType type;   
}SoftkeyPrompt;





//five chess object struct
typedef struct _GmFive GmFive;

struct _GmFive
{
   AEEApplet      a ;          //First element of this structure must be AEEApplet.
   AEEDeviceInfo  m_dInfo;     // device information
   IGraphics*     m_pGraphics;   //graphics interface   
   IMenuCtl*      m_pSoftKeyCtl; //softkey menu
   IMenuCtl*      m_pMenuCtl;   //menu menu
   IStatic*       m_pStaticCtrl;
   IImage*        m_pTopBGImage;         // draw background bmp of title
   IImage*        m_pBottomImage;        // draw background bmp of softkey   

#if defined(AEE_STATIC)
   IConfig*       m_pConfig;     // IConfig interface
   byte           m_beepVol;     //beep volum
#endif
   IRingerMgr*    m_promptVoice; // chess piece operation for drop piece

   byte           m_RingVol;     // Ring Volume  TCL wangbin040817 added
   int            m_nNChSize;    // Large char size
   int            m_nLChSize;    // Normal char size

   // Physical screen
   int16          m_cxScreen;    // Physical screen size (pixels)
   int16          m_cyScreen;    // Physical screen size (pixels)
   EAppStateType  m_eAppState;   // state of handle

   CHESS_OBJECT   m_chess_object;        //chess object struct
   boolean        m_gameOverFlag;        //flag of game over
   boolean        m_gameStartFlag;       //flag of start game

   char           m_global_game_level;   //game level
   char           m_global_who_go;       //current who chess
   char           m_old_who_go;          //last who chess
   checkPiece     m_curManPiece;         //current place of man's piece
   checkPiece     m_lastManPiece;        //last place of man's play piece
   checkPiece     m_tempPiece;           //infor of temp piece
   boolean        m_pieceSplashFlg;      //machine splash piece
   boolean        m_squareSplashFlg;     //squareShape flag
   boolean        m_winPieceSplashFlg;   //squareShape flag
   uint16         m_SelMenuItem;         //selected mainmenu item
   CPrefData      m_ChessCfgData;        //chess info saved to pref.dat file
   int16          m_manTotalPiece;       //num piece of man chess
   AEERect        m_TextRect;            //the rect of displaying text
   int16          m_splashNum;           //splash num piece
   AEERect        m_frameRect;           //out frame rect
   SoftkeyPrompt  m_softkeyPrompt;       //softkey prompt content
   boolean        m_forbidPieceFlg;      // flag of forid chess piece

#ifdef  SOUND_FEATURE
   char           m_playFile[MAX_RING_LEN];//save the file name of selected ringer
   IRingerMgr*    m_pIRingerMgr;           //ringer interface
   uint16         m_ringerSelItemID;       //current selected  ringer id
   uint16         m_soundMode;             //mode of open or close
   uint16         m_dloadRingerID;         //record the download ringer to judge the exist of download ringer
   boolean        m_preListenFlag;         // pre listen ringer flag
   //added by tclwgs 041216
   boolean        m_toneFlag;
   //end added by tclwgs 041216
#endif

#if defined(AEE_STATIC)   
   BottomBar_Param_type      m_BottomBar;            // bottom variable for screen 
#else
   AECHAR         m_szwinTitle[32];   
   int16          m_topTitleLen;          //top title wstren len
   boolean        m_AllASCIICharFlg;      
#endif  //AEE_STATIC
   IAnnunciator   *m_pIAnn;

};

//bottom bar of definition 
#if defined(AEE_STATIC) 
// t 表示提示条的类型
/*
	   MEMSET(&pMe->m_BottomBar, 0, sizeof(pMe->m_BottomBar));\
	   pMe->m_BottomBar.rcScreen.x	= 0;	\
	   pMe->m_BottomBar.rcScreen.y	= 0;	\
	   pMe->m_BottomBar.rcScreen.dx = pMe->m_cxScreen; \
	   pMe->m_BottomBar.rcScreen.dy = pMe->m_cyScreen; \
	   TCLDrawBottomBar( pMe->a.m_pIShell,	  \
						 pMe->a.m_pIDisplay,  \
						&pMe->m_BottomBar,	  \
						 type); 			  \
	   IDISPLAY_Update(pMe->a.m_pIDisplay);   \
	   */

#define DRAW_PROMPT_BAR(x)                      \
        {                                          \
            BottomBar_Param_type BarParam;                  \
	    	MEMSET(&BarParam, 0, sizeof(BarParam));         \
	   		BarParam.eBBarType = x;                         \
	    	DrawBottomBar(pMe->a.m_pIDisplay, &BarParam);      \
        }
#endif //AEE_STATIC


#define  _GMFIVE_H
#endif   //_GMFIVE_H


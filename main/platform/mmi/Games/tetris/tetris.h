/*===============================================================================

FILE: tetris.h

AUTHOR: whewei
================================================================================*/

/*===============================================================================
                            INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEMenu.h"
#include "AEEDisp.h"
#include "AEEGraphics.h"
#include "AEERinger.h"
#include "AEEStdLib.h"
#include "Appscommon.h"
#include "tetris.brh"
#include "AEEAnnunciator.h"
#include "err.h"
#if defined AEE_STATIC
    //#include "OEMMacros.h"
    #include "OEMClassIDs.h" 
    //#include "AppComFuncs.h"
    #include "uiutils.h"
#else
    #include "tetris.bid"
#endif

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
//For BREW3.0 support the muti-language re-define the RES maco
#define TETRIS_RES_FILE_LANG  AEE_RES_LANGDIR TETRIS_RES_FILE


#define BLOCK_SIZE       10             // Size of a single block
#define BLOCK_NUM        4              // Number of blocks 
#define MAX_LEVEL        3              // Max number of levels supported
#define NEXT_MAX_YNUM    2              // Max number of blocks in vertical direction 
#define NEXT_MAX_XNUM    4              // Max number of blocks in horizontal direction 
#if defined(FEATURE_DISP_128X160)
#define NEXT_BLOCK_SIZE  9              // Size of the next block 
#else
#define NEXT_BLOCK_SIZE  11              // Size of the next block 
#endif
#define MAX_REM_LINES    4              // Max number of lines deleted once
#define MAX_FLASH_TIMES  3              // Max flash times when delete a full line
#define MAX_BLOCK_KIND   7              // Max number of types supported
#define MAX_DOWN_UNIT    3              // Max number of units when moving down

#define INIT_TIME        1200           // The initial time in easy level a block stops 
#define TIME_SLICE       50             // The time decrement when level adds(ms)
#define FLASH_TIME       200            // Used to set timer
#define OVER_TIME        100            // Used to set timer when game is over
#define LEVEL_SCORE      500            // Max score in a level  

#define CLR_MBG   MAKE_RGB(  0,  0,  0) // Main region background color
#define CLR_FRAME MAKE_RGB( 20, 90,141) // Frame color
#define CLR_TEXT  MAKE_RGB(238, 48,167) // Text  color used in game screen
#define CLR_FLASH MAKE_RGB(205,179,139) // Flash color
#define CLR_LINE  MAKE_RGB(255,255,255) // Line  color
#define CLR_OVER  MAKE_RGB(  0,104,139) // Color used to update screen when game is over
#define CLR_BT    MAKE_RGB(0x63,0xb8,0xff)
//#define CLR_TEXT  MAKE_RGB(0x91,0x2c,0xee)

#ifdef FEATURE_LCD_TOUCH_ENABLE
#define LARGE_POP_WIN_X    25
#define LARGE_POP_WIN_Y    45
#define LARGE_POP_WIN_DX   140
#define LARGE_POP_WIN_DY   200

#define SMALL_POP_WIN_X    35
#define SMALL_POP_WIN_Y    60
#define SMALL_POP_WIN_DX   120
#define SMALL_POP_WIN_DY   160

#else
#if defined(FEATURE_DISP_128X160)
#define LARGE_POP_WIN_X    14
#define LARGE_POP_WIN_Y    40
#define LARGE_POP_WIN_DX   100
#define LARGE_POP_WIN_DY   80

#define SMALL_POP_WIN_X    24
#define SMALL_POP_WIN_Y    50
#define SMALL_POP_WIN_DX   80
#define SMALL_POP_WIN_DY   60
#elif defined(FEATURE_DISP_176X220)
#define LARGE_POP_WIN_X    16
#define LARGE_POP_WIN_Y    40
#define LARGE_POP_WIN_DX   120
#define LARGE_POP_WIN_DY   100

#define SMALL_POP_WIN_X    28
#define SMALL_POP_WIN_Y    50
#define SMALL_POP_WIN_DX   100
#define SMALL_POP_WIN_DY   100
#else
#define LARGE_POP_WIN_X    25
#define LARGE_POP_WIN_Y    45
#define LARGE_POP_WIN_DX   140
#define LARGE_POP_WIN_DY   200

#define SMALL_POP_WIN_X    35
#define SMALL_POP_WIN_Y    60
#define SMALL_POP_WIN_DX   120
#define SMALL_POP_WIN_DY   160
#endif
#endif



//Styles of the menu
#define MENU_COLOR_MASK             (MC_BACK | MC_SEL_BACK | MC_SEL_TEXT | MC_SCROLLBAR | MC_SCROLLBAR_FILL)        
#define MENU_BACKGROUND             MAKE_RGB(255,255,225)   //Unselected menu item 
#define MENU_SEL_BACKGROUND         MAKE_RGB(  0,128,255)   //Selected menu item 
#define MENU_TEXT                   RGB_BLACK               //Unselected menu item text color
#define MENU_SEL_TEXT               RGB_BLACK               //Selected menu item text color
#define MENU_FRAME                  RGB_WHITE               //Frame color
#define MENU_SCROLLBAR              RGB_WHITE               //Scroll bar color
#define MENU_SCROLLBAR_FILL         MAKE_RGB(228,  0,139)   //Scroll bar fill color

//Styles of the pop menu
#define POP_MENU_BACKGROUND         MAKE_RGB(240,240,240)   //Unselected pop menu item 
#define POP_MENU_SEL_BACKGROUND     MAKE_RGB(  0,128,255)   //Selected pop menu item
#define POP_MENU_SCROLLBAR_FILL     MAKE_RGB( 90,  0,220)   //Scroll bar color 
 
#define SOFTKEY_BACKGROUND       MAKE_RGB(0,128,0)
#define SOFTKEY_SEL_BACKGROUND   MAKE_RGB(0,128,0)

#if defined AEE_STATIC
    #define TETRIS_SOFTKEY_DY  BOTTOMBAR_HEIGHT
#else
    #define TETRIS_SOFTKEY_DY  BOTTOMBAR_HEIGHT       // The height of the softkey menu
#endif

#define EVT_PEN_DOWN 0x504
#define EVT_PEN_MOVE 0x505
#define EVT_PEN_UP   0x506

#define BUTTON_DY    23
#define CLR_DEF_TEXT MAKE_RGB(0, 0, 255)// The color of the text used in initial menus
#define TETRIS_VERSION 2                 
#define   PARAM_NOT_REF(x)
#define SetUniMenuConfig(p)         //do nothing SetUniMenuConfig(p)
#define SetUniTextConfig(pT)        //do nothing SetUniTextConfig(pT)

#if defined AEE_STATIC
    static const AEEAppInfo gaiTetris =
    {
        AEECLSID_TETRIS,
        TETRIS_RES_FILE,
        IDS_TETRIS_TITLE,
        NULL,
        0,
        0,
        0,
        AFLAG_HIDDEN
    };
#endif

typedef enum
{
    APP_STATE_SPLASH,
    APP_STATE_MAIN,
    APP_STATE_LEVEL,
    APP_STATE_SOUND,
    APP_STATE_GRID,
    APP_STATE_HERO,
    APP_STATE_HELP,
    APP_STATE_RUNNING,
    APP_STATE_PAUSED,
    APP_STATE_NEXTLEVEL,
    APP_STATE_LASTLEVEL,
    APP_STATE_OVER,
    APP_STATE_REPORT
}EAPPSTATE;                             // Current state of the applet                  

typedef enum
{
    ACTION_NONE,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_ROTATE
}ActionType;                            // Indicate the move and rotate event

typedef enum
{
    MUSIC_PLACE,
    MUSIC_SINLINE,
    MUSIC_MULLINES,
    MUSIC_NEXTSPEED,
    MUSIC_NEXTLEVEL,
    MUSIC_GAMEOVER
}MusicType;

typedef struct _CPrefData
{
    uint8    m_nLevel;
    boolean  m_bSound;
    boolean  m_bGrid;
    uint16   Score[MAX_LEVEL];          // Record the highest score at each level
}CPrefData;                             

typedef PACKED struct _CBlock
{
    uint16 XPos;
    uint16 YPos;
}CBlock;                                // A single block

typedef struct _CTetris
{
    int8 XPos[BLOCK_NUM];
    int8 YPos[BLOCK_NUM];
}CTetris;                               // A group of blocks

typedef struct _CPos
{
    boolean IsFull;
    uint8   ClrIndex;
}CPos;                                  // Record state of a rect in the game region

typedef struct _CTetrisApp
{
    AEEApplet   a;

    IMenuCtl *     m_pMainMenu; 
    IMenuCtl *     m_pMenu;
    IMenuCtl *     m_pSoftKeyMenu;
    IGraphics*     m_pGraphics;
    IStatic  *     m_pStatic;
    IImage   *     m_pImage;
    IRingerMgr *   m_pPlayer;
    
#if defined(AEE_STATIC)   
    BottomBar_Param_type      m_BottomBar;         // Bottom bar used in this applet. 
#endif 

    EAPPSTATE   m_AppState;
    CPrefData   m_CfgData;              // Used to record the highest score

    uint8       m_rRand;                // Random number to identify the shape of blocks
    uint8       m_rKind;                // Shape of blocks
    uint8       m_rLineHeight;          // Height of current font
    uint16      m_rScreenWidth;         
    uint16      m_rScreenHeight;

    uint16      m_rHMid;                // Horizontal base position of the current blocks
    uint16      m_rVStart;              // Vertical base position of the current blocks
    uint16      m_rNextStartX;          // Horizontal base position of the next blocks
    uint16      m_rNextStartY;          // Vertical base position of the next blocks
    uint16      m_rHNum;                // Max number of blocks contained in a row
    uint16      m_rVNum;                // Max number of blocks contained in a col
    uint16      m_rHorMinVal;
    uint16      m_rHorMaxVal;
    uint16      m_rVerMinVal;
    uint16      m_rVerMaxVal;

    uint8       m_rLevel;               // Game level
    uint16      m_rSelMenuItem;         // Selected main menu item.
    uint16      m_rSpeed;               // Game speed
    uint16      m_rCurMinYPos;          // Current minimal vertical position
    uint16      m_rScore;               // Game score
 
    boolean     m_bContinue;            // Can the game be continued
    boolean     m_bGrid;                // Draw the gridding or not
    boolean     m_bSound;               // Enable or disable sound in the play
    uint8       m_rFlashTimes;          // Current flash times
    uint8       m_rRmLines;             // Lines need to delete
    uint16      m_rTime;                // Current time a block stops
 
    AEERect     m_MainRect;             // Main game regin
    AEERect     m_Rect;                 // Used to update screen after game is over

    uint16      m_WaitingTime[MAX_LEVEL]; 
    uint16      m_LinesToRm[MAX_REM_LINES]; 
                                        // Record the lines to be removed
    RGBVAL *    m_BlockClr;             // Colors to display blocks
    CTetris     m_BlockDataLib[MAX_BLOCK_KIND];
                                        // Shapes of blocks supported 
    CBlock      m_MainBlock;            // Central block
    CBlock      m_Blocks[BLOCK_NUM];    // Current blocks
    CPos   **   m_Pos;                  // Current state of the main game regin
    IAnnunciator                    *m_pIAnn;
}CTetrisApp;

#if defined AEE_STATIC
    #define DRAW_PROMPT_BAR(x)             \
    {                                          \
        BottomBar_Param_type BarParam;                  \
    	MEMSET(&BarParam, 0, sizeof(BarParam));         \
   		BarParam.eBBarType = x;                         \
    	DrawBottomBar(pMe->a.m_pIDisplay, &BarParam);      \
    }
#endif

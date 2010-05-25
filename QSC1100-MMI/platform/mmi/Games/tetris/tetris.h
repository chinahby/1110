/** ----------------------------------------------------------------------------
 * tetris.h
 * 
 * Copyright(c) 2007 Anylook
 * 
 * 
 * History:
 * 
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2007.09.30      refactor, logic bugs fixing
 * 
 * -----------------------------------------------------------------------------
 */


#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEMenu.h"
#include "AEEGraphics.h"
#include "AEERinger.h"
#include "AEEStdLib.h"

#include "tetris.brh"
#include "tetrisimages.brh"

#if defined( AEE_STATIC)
    #include "OEMClassIDs.h" 
#else
    #include "tetris.bid"
#endif

#include "AEEConfig.h"


#define MAX_LEVEL        3  // Max number of levels supported
#define MAX_REM_LINES    4  // Max number of lines deleted once
#define MAX_FLASH_TIMES  3  // Max flash times when delete a full line
#define MAX_BLOCK_KIND   7  // Max number of types supported

#define INIT_TIME        900    // The initial time in easy level a block stops
#define TIME_SLICE       50     // The time decrement when level adds(ms)
#define FLASH_TIME       200    // Used to set timer
#define OVER_TIME        100    // Used to set timer when game is over
#define LEVEL_SCORE      500    // Max score in a level  

#define COLOR_BACKGROUND    MAKE_RGB(  0,  0,  0)

#define TETRIS_VERSION 2  
#ifndef WIN32
#if defined( AEE_SIMULATOR)
#define TETRIS_RES_FILE_LANGUAGE   ("fs:/mod/tetris/en/"TETRIS_RES_FILE)
#define TETRIS_RES_FILE_IMAGES        ("fs:/mod/tetris/en/"TETRISIMAGES_RES_FILE)
#else
#define TETRIS_RES_FILE_LANGUAGE   ( AEE_RES_LANGDIR TETRIS_RES_FILE)
#define TETRIS_RES_FILE_IMAGES       ( AEE_RES_LANGDIR TETRISIMAGES_RES_FILE)
#endif
#else
#define TETRIS_RES_FILE_LANGUAGE   ( AEE_RES_LANGDIR TETRIS_RES_FILE)
#define TETRIS_RES_FILE_IMAGES       ( AEE_RES_LANGDIR TETRISIMAGES_RES_FILE)
#endif//WIN32
//end added
typedef enum
{
    GAME_STATE_INIT = -3,
    GAME_STATE_SUSPENDED,
    GAME_STATE_SPLASH,
    
    GAME_STATE_MAIN_MENU = 0,
    GAME_STATE_LEVEL_SETTING,
    GAME_STATE_SOUND_SETTING,
    GAME_STATE_GRID_SETTING,
    GAME_STATE_HERO,
    
    GAME_STATE_HELP,
    GAME_STATE_RUNNING,
    GAME_STATE_PAUSED,
    GAME_STATE_NEXTLEVEL,
    GAME_STATE_LASTLEVEL,
    GAME_STATE_OVER,
    GAME_STATE_REPORT
} GameStateEnum;                   


typedef enum
{
    ACTION_NONE,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_ROTATE  
} ActionTypeEnum;

typedef enum
{
    MUSIC_PLACE,
    MUSIC_SINLINE,
    MUSIC_MULLINES,
    MUSIC_NEXTSPEED,
    MUSIC_NEXTLEVEL,
    MUSIC_GAMEOVER
} MusicTypeEnum;

typedef struct _CPrefData
{
    uint8    gameLevel;
    boolean  soundOn;
    boolean  drawGridLines;
    uint16   topScore[MAX_LEVEL];
} CPrefData;                             

typedef PACKED struct _CBlock
{
    int16 x;
    int16 y;
} CBlock;

typedef struct _CTetris
{
    
    byte axis;       // whick block is the rotate axis
    byte model;      // which model
    boolean pinned;  // if can not move downward, it is called pinned
    CBlock block[4]; // the 4 blocks consisting a tetris
} CTetris;

typedef struct Grid
{
    boolean dirty;
    uint8   tetrisModel;
} Grid;

typedef struct _CTetrisApp
{
    AEEApplet   a;

    IMenuCtl *      menuListStyle;
    IMenuCtl *      menuSoftkey;
    
    IGraphics       *graphics;
    IStatic         *staticControl;
    IImage          *image;
    IRingerMgr      *ringerPlayer;
    IConfig         *config;
    
    GameStateEnum       previousGameState;
    GameStateEnum       gameState;
    CPrefData           configData;

    uint8       fontHeight; 
    uint16      screenWidth;         
    uint16      screenHeight;

    uint16      xWhereToLaunchTetris;
    uint16      yWhereToLaunchTetris;
    uint16      xWhereToDrawTheNextFallingTetris;
    uint16      yWhereToDrawTheNextFallingTetris;
    
    uint8       gridSideLength;   // grid side length, in pixel
    uint16      gridColumnNumber; // the playing zone grid column number
    uint16      gridRowNumber;    // the playing zone grid row number
    
    uint8       gameLevel;              // Game level
    uint16      selectedMenuItem[5];
    uint16      theTopmostRowOfThePinnedTetrises; // start from 0
    uint16      gameScore;              // Game score
 
    uint16      sleepTime;              // period between two moving downward
    uint16      gameSpeed;              // Game speed
    
    boolean     continueGame;           // Can the game be continued
    boolean     drawGridLines;          // Draw the gridding or not
    boolean     soundOn;                // Enable or disable sound in the play
    boolean     moveDownwardAccelerated;
    
    AEERect     playingZone; 
    AEERect     gridToClear;
 
    uint16      dirtyRowIndex[MAX_REM_LINES];
    uint8       flashCounterWhenDeleteDirtyRows; 
    uint8       dirtyRowNumber;
                                        
    CTetris     theFallingTetris;
    CTetris     theNextFallingTetris;
    Grid        **gridMatrix;
    // every unint16 map a row
    uint16      *gridRowMap;
    uint16      gridRowMask;
    
    byte keyBeepVolumeSetting;

    IImage *imageScore;
    IImage *imageSpeed;
    IImage *imageNumber;
    
} CTetrisApp;




static CTetrisApp* me;
static CTetris TETRIS_MODEL[] = {
        
    //   * *
    // * *
    // the 1st block is the axis
    { 0, 0, FALSE, { {0, 0}, {1, 0}, {-1, 1}, {0, 1} } },
    
    // * * * * 
    // the 2nd block is the axis
    { 1, 1, FALSE, { {-1, 0}, {0, 0}, {1, 0}, {2, 0}} },
    
    // * *
    //   * *
    // the 2nd block is the axis
    { 1, 1, FALSE, { {0, 0}, {1, 0}, {1, 1}, {2, 1} } },
    
    // *
    // * * *
    // the 2nd block is the axis
    { 1, 1, FALSE, { {0, 0}, {0 ,1}, {1, 1}, {2, 1} } },

    //   *
    // * * *
    // the 3rd block is the axis
    { 2, 2, FALSE, { {0, 0}, {-1, 1}, {0, 1}, {1, 1} } },

    //     *
    // * * *
    // the 4th block is the axis
    { 3, 3, FALSE, { {1, 0}, {-1, 1}, {0, 1}, {1, 1} } },

    // * *
    // * *
    // no rotation operation
    { 1, 0, FALSE, { {0, 0}, {1, 0}, {0, 1}, {1, 1} } }
};          
    
static RGBVAL COLOR_PALETTE[] = {
    MAKE_RGB( 0xff, 0x66, 0x40),
    MAKE_RGB( 0xff, 0x1b, 0xf1),
    MAKE_RGB( 0xff, 0x01, 0x58),
    MAKE_RGB( 0xa8, 0xff, 0x02),
    MAKE_RGB( 0x69, 0x00, 0xda),
    MAKE_RGB( 0xf8, 0x7a, 0x00),
    MAKE_RGB( 0x00, 0x16, 0xd9)
};


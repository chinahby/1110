/** ----------------------------------------------------------------------------
 * tetris.c
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

#include "tetris.h"
#ifdef WIN32
#include "oemhelperfunctype.h"
#endif
#if !defined( AEE_SIMULATOR)
    #include "OEMCFGI.h"
    #include "db.h"
    #include "nv_items.h"
#endif

#include "Appscommon.h"
#define debug DBGPRINTF

//------------------ mod related method declaration
#if defined( AEE_STATIC)
    static int TetrisMod_CreateInstance(AEECLSID ClsID, IShell * pIShell, IModule * po, void ** ppObj);
    int TetrisMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif

//------------------ applet related method declaration
static int      Tetris_InitAppData( void);
static void     Tetris_FreeAppData( void);

static void     initParametersAccordingToScreenSize( void);
static void     initGameData( void);

static boolean  Tetris_HandleEvent( CTetris *ptetris, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean  processKey( uint16 keyCode);
static boolean  processCommand( uint16 commandId);

static void     setGameState(GameStateEnum gameState);
static void     gotoNextLevel( boolean bIsLastLevel);

static void     startOneNewGame( void);
static void     launchOneTetris( void);

static uint8    generateRandomTetrisModel( void);
static void     calculateAbsoluteCoordinates( CTetris* tetris, byte model, int16 xOrigin, int16 yOrigin);
static void     drawGameScoreAndGameSpeed( void);
static void     clearHerosScore( void);

static boolean  moveDownwardsIf( void);
static void     moveLeftwardsIf( void);
static void     moveRightwardsIf( void);
static void     rotateIf( void);
static boolean  pinTheFallingTetrisIf( void);
static boolean  deleteDirtyRowsIf( void);
static boolean  canDoAction( CTetris* tetris, ActionTypeEnum action);

static boolean  theGridIsDirtyOrInvalid( int16 x, int16 y);
static boolean  theGridIsDirty( int16 x, int16 y);

static void     saveGameDataAndDisplayGameScore( void);
static void     saveSettingData( void);
static void     gameOver( void);

static boolean  pauseOrResumeGame( void);
#define theRowIsDirty( row) ( me->gridRowMap[row] == me->gridRowMask)

//-------------------- UI related method declaration
static void     redrawTheScreen( void);

static void     resetControlsAndClearScreen( boolean clearScreen);
static void     addMenuItem(IMenuCtl * pMenu, uint16 ItemID, uint16 ImageID);
static void     drawPopWindowFrameBorder( uint16 titleID, AEERect * fRect);

static void     draw3DText( int16 x, int16 y, AECHAR* text);

static void     displaySplashScreen( void);
static void     displayMainMenu( void);
static void     displayLevelSettingScreen( void);
static void     displaySoundSettingScreen( void);
static void     displayGridSettingScreen( void);
static void     displayHerosScreen( void);
static void     displayHelpScreen( void);
static void     displayGameScoreScreen( uint16 strID, uint32 Score, boolean NewRecord);
static void     clearPlayingZoneAfterGameOver( void);

static void     drawPlayingZoneBorder( void);
static void     drawGridLines( void);
static void     promptTheGameIsPausedNow( void);
static void     drawTetris( CTetris *tetris, boolean bOverDraw);
static void     drawTheFallingTetris( void);
static void     clearTheFallingTetris( void);
static void     drawTheNextFallingTetris( void);
static void     clearTheNextFallingTetris( void);

static void     refreshPlayingZoneAfterDeleteDirtyRows( void);
static void     flashWhenDeleteDirtyRows( void);
static void     drawTheFinalTetrisWhichFillupThePlayingZone( int16 YPos);
static boolean  updateGameScoreAndGoToNextLevelIf( void);

static boolean  playMusic( MusicTypeEnum type);

//---------------------- mod related method definition

#if defined(AEE_STATIC)
int TetrisMod_Load(IShell *ps, void *pHelpers, IModule **pMod)
{

   return AEEStaticMod_New(sizeof(AEEMod),
                           ps,
                           pHelpers,
                           pMod,
                           (PFNMODCREATEINST)TetrisMod_CreateInstance,
                           (PFNFREEMODDATA)NULL);
}
#endif  /*AEE_STATIC*/

#if defined(AEE_STATIC)
int TetrisMod_CreateInstance( AEECLSID ClsID, IShell * pIShell, IModule * po, void ** ppObj)
#else
int AEEClsCreateInstance( AEECLSID ClsID,IShell * pIShell, IModule * po,void ** ppObj)
#endif
{

    if( ClsID == AEECLSID_GAME_TETRIS)
    {
        if( AEEApplet_New( sizeof(CTetrisApp),
                           ClsID,
                           pIShell,
                           po,
                           (IApplet**)ppObj,
                           (AEEHANDLER)Tetris_HandleEvent,
                           (PFNFREEAPPDATA)Tetris_FreeAppData
                   ) == TRUE
        )
        {

            me = ( CTetrisApp *)*ppObj;
            if( Tetris_InitAppData() == SUCCESS)
            {
                debug( ";Tetris_InitAppData success");
                return AEE_SUCCESS;
            }
            else
            {
                Tetris_FreeAppData();
                IAPPLET_Release((IApplet *)*ppObj);
                *ppObj = NULL;
                return EFAILED;
            }
        }
    }
    return EFAILED;
}

//--------------------- applet related method declaration
static int Tetris_InitAppData( void)
{

    int i           = 0;
    AEEDeviceInfo *pDeviceInfo = NULL;

    // create menu and static controls, graphics object and ringer player
    if( ( i = ISHELL_CreateInstance( me->a.m_pIShell, AEECLSID_MENUCTL, (void **)&me->menuListStyle)) != SUCCESS)
    {
        debug( "creating list style menu control failed!, 0x%x", i);
        return i;
    }

    if( ( i = ISHELL_CreateInstance( me->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&me->menuSoftkey)) != SUCCESS)
    {
        debug( "creating softkey menu control failed!, 0x%x", i);
        return i;
    }

    if( ( i = ISHELL_CreateInstance( me->a.m_pIShell, AEECLSID_GRAPHICS, (void **)&me->graphics)) != SUCCESS)
    {
        debug( "creating graphics object failed!, 0x%x", i);
        return i;
    }

    if( ( i = ISHELL_CreateInstance( me->a.m_pIShell, AEECLSID_STATIC, (void **)&me->staticControl)) != SUCCESS)
    {
        debug("creating static control failed!, 0x%x", i);
        return i;
    }                                   // Static interface object.

    if( ( i = ISHELL_CreateInstance( me->a.m_pIShell, AEECLSID_RINGERMGR, (void **)&me->ringerPlayer)) != SUCCESS)
    {
        debug("creating ringer manager object failed!, 0x%x", i);
        return i;
    }

#if !defined( AEE_SIMULATOR)
    if((i = ISHELL_CreateInstance(me->a.m_pIShell, AEECLSID_CONFIG, (void **)&me->config)) != SUCCESS)
    {
        DBGPRINTF("creating config object failed!, 0x%x", i);
        return i;
    }
#endif

    // add it @08.01.17
//    IMENUCTL_SetProperties( me->menuListStyle, MP_BIND_ITEM_TO_NUMBER_KEY | MP_WRAPSCROLL);

    // get device infomation
    pDeviceInfo = ( AEEDeviceInfo*)MALLOC( sizeof( AEEDeviceInfo));
    if( pDeviceInfo == NULL)
    {
        debug( ";creating AEEDeviceInfo instance failed");
        return ENOMEMORY;
    }
    ISHELL_GetDeviceInfo( me->a.m_pIShell, pDeviceInfo);

    me->screenWidth  = pDeviceInfo->cxScreen;
    me->screenHeight = pDeviceInfo->cyScreen;   // The width and height of the device.
    debug( ";initApp, screen, cx = %d, cy = %d", pDeviceInfo->cxScreen, pDeviceInfo->cyScreen);

    me->fontHeight = IDISPLAY_GetFontMetrics( me->a.m_pIDisplay, AEE_FONT_NORMAL, 0, 0);;
    debug( ";initApp, font height = %d", me->fontHeight);

    // calculate screen parameters according to the device infomation
    initParametersAccordingToScreenSize();

    me->gridRowMap = MALLOC( me->gridRowNumber * sizeof( uint16));
    if( me->gridRowMap == NULL)
    {
        debug( ";creating gridRowMap failed.");
        return ENOMEMORY;
    }
    me->gridRowMask = 0;
    for( i = 0; i < me->gridColumnNumber; i ++)
    {
        me->gridRowMask |= 1 << i;
    }
    for( i = 0; i < me->gridRowNumber; i ++)
    {
        me->gridRowMap[i] = 0;
    }
    debug( ";initApp, rows = %d, gridRowMask = 0x%x", me->gridRowNumber, me->gridRowMask);

    me->gridMatrix = MALLOC( me->gridRowNumber * sizeof( Grid*));
    if( me->gridMatrix == NULL)
    {
        debug( ";creating gridMatrix row dimension failed.");
        return ENOMEMORY;
    }

    for( i = 0; i < me->gridRowNumber; i ++)
    {

        me->gridMatrix[i] = MALLOC( me->gridColumnNumber * sizeof(Grid));
        if( me->gridMatrix[i] == NULL)
        {
            debug( "allocating memory for gridMatrix column %d dimension failed.", i);
            return ENOMEMORY;
        }
    }

    // load configuration data
    if( ISHELL_GetPrefs( me->a.m_pIShell,
                       AEECLSID_GAME_TETRIS,
                       TETRIS_VERSION,
                       &( me->configData),
                       sizeof( CPrefData)
                     ) != SUCCESS
    )
    {

        me->configData.gameLevel        = 1;
        me->configData.drawGridLines    = FALSE;
        me->configData.soundOn      = FALSE;

        for( i = 0; i < MAX_LEVEL; i ++)
        {
            me->configData.topScore[i] = 0;
        }
    }

    me->sleepTime       = INIT_TIME;
    me->gameSpeed       = 0;
    me->gameLevel       = me->configData.gameLevel;
    me->soundOn         = me->configData.soundOn;
    me->drawGridLines   = me->configData.drawGridLines;

    me->selectedMenuItem[GAME_STATE_MAIN_MENU] = IDS_TETRIS_START;

    me->imageScore  = ISHELL_LoadResImage( me->a.m_pIShell, TETRIS_RES_FILE_LANGUAGE, IDI_SCORE);
    me->imageSpeed  = ISHELL_LoadResImage( me->a.m_pIShell, TETRIS_RES_FILE_LANGUAGE, IDI_SPEED);
	#ifdef FEATURE_VERSION_IVIO
	me->imageNumber = ISHELL_LoadResImage( me->a.m_pIShell, TETRISIMAGES_IVIO_RES_FILE, IDI_NUMBER);
	#else
    me->imageNumber = ISHELL_LoadResImage( me->a.m_pIShell, TETRISIMAGES_RES_FILE, IDI_NUMBER);
	#endif

    FREEIF( pDeviceInfo);

    me->previousGameState   = GAME_STATE_INIT;
    me->gameState           = GAME_STATE_INIT;

    return SUCCESS;
} // Tetris_InitAppData

static void Tetris_FreeAppData(  void)
{
    int i;

    for( i = 0; i < me->gridRowNumber; i ++)
    {
        FREEIF( me->gridMatrix[i]);
    }
    FREEIF( me->gridMatrix);
    FREEIF( me->gridRowMap);

    if( me->image != NULL)
    {
        IIMAGE_Release( me->image);
        me->image = NULL;
    }
    if( me->imageScore != NULL)
    {
        IIMAGE_Release( me->imageScore);
        me->imageScore = NULL;
    }
    if( me->imageSpeed != NULL)
    {
        IIMAGE_Release( me->imageSpeed);
        me->imageSpeed = NULL;
    }
    if( me->imageNumber != NULL)
    {
        IIMAGE_Release( me->imageNumber);
        me->imageNumber = NULL;
    }
    if( me->menuListStyle != NULL)
    {
        IMENUCTL_Release( me->menuListStyle);
        me->menuListStyle = NULL;
    }
    if( me->menuSoftkey != NULL)
    {
        IMENUCTL_Release( me->menuSoftkey);
        me->menuSoftkey = NULL;
    }
    if( me->graphics != NULL)
    {
        IGRAPHICS_Release( me->graphics);
        me->graphics = NULL;
    }

    if( me->staticControl != NULL)
    {
        ISTATIC_Release( me->staticControl);
        me->staticControl = NULL;
    }

    if( me->ringerPlayer != NULL)
    {
        IRINGERMGR_Release( me->ringerPlayer);
        me->ringerPlayer = NULL;
    }

#if !defined( AEE_SIMULATOR)
    if(me->config != NULL)
    {
        ICONFIG_Release(me->config);
        me->config = NULL;
    }
#endif
} // Tetris_FreeAppData


static void initParametersAccordingToScreenSize( void)
{

    uint16 marginHorizontal = 0;
    uint16 marginVertical   = 0;

    // the playing zone has a fixed column number
    me->gridColumnNumber    = 12;

    // determine grid size according to screen size,
    // there are two borders, totally occupy 4 pixels
    // and the next tetris occupies 4 grids
    me->gridSideLength = ( me->screenWidth - 4) / ( me->gridColumnNumber + 4);
    debug( ";init, grid, w / 16, %d", me->gridSideLength);
    if( me->gridSideLength > 4)
    {

        if( ( (me->screenHeight - 4) / me->gridSideLength) < 16)
        {

            me->gridSideLength = ( me->screenHeight - 4) / 16;
            me->gridSideLength = me->gridSideLength < 4 ? 4: me->gridSideLength;
            debug( ";init, grid, rows < 16, %d", me->gridSideLength);
        }
    }

    // grid side is determined, now we determine grid row number
    // it shall not more than 21
    me->gridRowNumber = (me->screenHeight - 4) / me->gridSideLength;
    me->gridRowNumber = me->gridRowNumber > 21 ? 21 : me->gridRowNumber;
    debug( ";grid, rowNumber = %d, columnNumber = %d", me->gridRowNumber, me->gridColumnNumber);

    // calculate the margin
    marginHorizontal    = ( me->screenWidth - 4 - (me->gridSideLength * 17)) / 3;
    marginVertical      = ( me->screenHeight - 4 -
            me->gridRowNumber * me->gridSideLength) >> 1;

    SETAEERECT( &me->playingZone,
            marginHorizontal + 2,
            marginVertical + 2,
            me->gridSideLength * 12,
            me->gridSideLength * me->gridRowNumber
            );

    me->xWhereToDrawTheNextFallingTetris = me->playingZone.x + me->playingZone.dx +
                            marginHorizontal + ( me->gridSideLength << 1);
    me->yWhereToDrawTheNextFallingTetris = me->gridSideLength;

    me->yWhereToLaunchTetris = me->playingZone.y;
    me->xWhereToLaunchTetris = me->playingZone.x +
        (  (me->playingZone.dx - me->gridSideLength * (me->gridColumnNumber % 2)) >> 1);
} // initParametersAccordingToScreenSize

static boolean Tetris_HandleEvent( CTetris *ptetris,
                AEEEvent eCode,
                uint16 wParam,
                uint32 dwParam
)
{

    switch (eCode)
    {
        case EVT_APP_START:
        {
        #if !defined( AEE_SIMULATOR)
            byte mute = OEMSOUND_MUTE_VOL;
            byte returnValue = 0;
            returnValue = ICONFIG_GetItem(me->config, CFGI_BEEP_VOL, &me->keyBeepVolumeSetting, sizeof(byte));
            returnValue = ICONFIG_SetItem(me->config, CFGI_BEEP_VOL, &mute, sizeof(byte));
        #endif
            displaySplashScreen();
            return TRUE;
        } // EVT_APP_START

        case EVT_APP_STOP:
        {
        #if !defined( AEE_SIMULATOR)
            ICONFIG_SetItem(me->config, CFGI_BEEP_VOL, &me->keyBeepVolumeSetting, sizeof(byte));
        #endif

            IRINGERMGR_Stop( me->ringerPlayer);
            ISHELL_CancelTimer( me->a.m_pIShell, NULL, me);
            saveSettingData();
            return TRUE;
        } // EVT_APP_STOP

        case EVT_FLIP:
        {
            if( wParam == FALSE && me->gameState == GAME_STATE_RUNNING)
            {
                pauseOrResumeGame();
                return TRUE;
            }
        }

        case EVT_APP_SUSPEND:
        {
        #if !defined( AEE_SIMULATOR)
            ICONFIG_SetItem(me->config, CFGI_BEEP_VOL, &me->keyBeepVolumeSetting, sizeof(byte));
        #endif

            resetControlsAndClearScreen(TRUE);
            ISHELL_CancelTimer(me->a.m_pIShell, NULL, me);
            IRINGERMGR_Stop(me->ringerPlayer);
            setGameState(GAME_STATE_SUSPENDED);
            return TRUE;
        } // EVT_APP_SUSPEND

        case EVT_APP_RESUME:
        {

        #if !defined( AEE_SIMULATOR)
            byte mute = OEMSOUND_MUTE_VOL;
            byte returnValue = 0;
            returnValue = ICONFIG_SetItem(me->config, CFGI_BEEP_VOL, &mute, sizeof(byte));
        #endif

            switch(me->previousGameState)
            {
                case GAME_STATE_SPLASH:
                {
                    displaySplashScreen();
                    return TRUE;
                }

                case GAME_STATE_MAIN_MENU:
                case GAME_STATE_LEVEL_SETTING:
                case GAME_STATE_GRID_SETTING:
                case GAME_STATE_SOUND_SETTING:
                case GAME_STATE_LASTLEVEL:
                case GAME_STATE_REPORT:
                case GAME_STATE_HERO:
                {
                    GameStateEnum previousGameState = me->previousGameState;

                    displayMainMenu();
                    if(previousGameState == GAME_STATE_LEVEL_SETTING)
                    {
                        displayLevelSettingScreen();
                    }
                    else if(previousGameState == GAME_STATE_GRID_SETTING)
                    {
                        displayGridSettingScreen();
                    }
                    else if(previousGameState == GAME_STATE_SOUND_SETTING)
                    {
                        displaySoundSettingScreen();
                    }
                    else if(previousGameState == GAME_STATE_LASTLEVEL)
                    {
                        gotoNextLevel(TRUE);
                    }
                    else if(previousGameState == GAME_STATE_REPORT)
                    {
                        saveGameDataAndDisplayGameScore();
                    }
                    else if(previousGameState == GAME_STATE_HERO)
                    {
                        displayHerosScreen();
                    }
                    return TRUE;
                }

                case GAME_STATE_HELP:
                {
                    displayHelpScreen();
                    return TRUE;
                }

                case GAME_STATE_RUNNING:
                case GAME_STATE_NEXTLEVEL:
                case GAME_STATE_PAUSED:
                {
                    if(me->previousGameState == GAME_STATE_NEXTLEVEL)
                    {
                        setGameState(GAME_STATE_NEXTLEVEL);
                    }
                    else
                    {
                        setGameState(GAME_STATE_PAUSED);
                    }
                    redrawTheScreen();
                    return TRUE;
                }

                case GAME_STATE_OVER:
                {
                    clearPlayingZoneAfterGameOver();
                    return TRUE;
                }
            } // switch( me->appState)

        } // case EVT_APP_RESUME

        case EVT_COMMAND:
        {
            if(processCommand(wParam))
            {
                return TRUE;
            }
            return FALSE;
        }
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
		case EVT_USER:
#endif//FEATURE_LCD_TOUCH_ENABLE
        case EVT_KEY:
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
        {
            boolean needToReturn = FALSE;

#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
			if(eCode == EVT_USER)
			{
				if((wParam == AVK_SELECT) || (wParam == AVK_INFO) || ( wParam == AVK_CLR ))
				{
					eCode = EVT_KEY;
				}
			}
#endif//FEATURE_LCD_TOUCH_ENABLE

            if(!needToReturn &&
                IMENUCTL_IsActive(me->menuListStyle) &&
                IMENUCTL_HandleEvent(me->menuListStyle, eCode, wParam, dwParam) )
            {
                needToReturn = TRUE;
            }

            if(!needToReturn &&
                IMENUCTL_IsActive(me->menuSoftkey) &&
                IMENUCTL_HandleEvent(me->menuSoftkey, eCode, wParam, dwParam) )
            {
                needToReturn = TRUE;
            }

            if(eCode == EVT_KEY &&
                (wParam == AVK_UP || wParam == AVK_DOWN) &&
                ( me->gameState == GAME_STATE_MAIN_MENU     ||
                  me->gameState == GAME_STATE_LEVEL_SETTING ||
                  me->gameState == GAME_STATE_SOUND_SETTING ||
                  me->gameState == GAME_STATE_GRID_SETTING ) )
            {

                me->selectedMenuItem[me->gameState] = IMENUCTL_GetSel(me->menuListStyle);
            }

            if( eCode == EVT_KEY &&
                ( wParam == AVK_LEFT || wParam == AVK_RIGHT) &&
                ( me->gameState == GAME_STATE_HERO) )
            {
                me->selectedMenuItem[GAME_STATE_HERO] = IMENUCTL_GetSel(me->menuSoftkey);
            }

            if(needToReturn)
            {
                return TRUE;
            }
            if( ( me->gameState == GAME_STATE_HELP       ||
                  me->gameState == GAME_STATE_LASTLEVEL  ||
                  me->gameState == GAME_STATE_NEXTLEVEL  ||
                  me->gameState == GAME_STATE_REPORT
                 ) && ISTATIC_HandleEvent(me->staticControl, eCode, wParam, dwParam))
            {
                return TRUE;
            }

            if(eCode == EVT_KEY && processKey(wParam))
            {
                return TRUE;
            }

        }
        break;

#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
		case EVT_PEN_UP:
		{
			if((me->gameState == GAME_STATE_MAIN_MENU) || (me->gameState == GAME_STATE_GRID_SETTING) || 
				(me->gameState == GAME_STATE_SOUND_SETTING) || (me->gameState == GAME_STATE_LEVEL_SETTING))
				return IMENUCTL_HandleEvent(me->menuListStyle, eCode, wParam, dwParam);
			if((me->gameState == GAME_STATE_HERO) || (me->gameState == GAME_STATE_HELP))
				return IMENUCTL_HandleEvent(me->menuSoftkey, eCode, wParam, dwParam);
		
		}
		break;
#endif//FEATURE_LCD_TOUCH_ENABLE

    }
    return FALSE;
} // Tetris_HandleEvent


static boolean processCommand( uint16 commandId)
{

    if( me->gameState == GAME_STATE_MAIN_MENU     ||
        me->gameState == GAME_STATE_LEVEL_SETTING ||
        me->gameState == GAME_STATE_SOUND_SETTING ||
        me->gameState == GAME_STATE_GRID_SETTING
    )
    {

        me->selectedMenuItem[me->gameState] = IMENUCTL_GetSel(me->menuListStyle);
    }

    switch(commandId)
    {
        case IDS_TETRIS_START:
        {
            (void)IDISPLAY_SetPrefs(me->a.m_pIDisplay, "a:0", STRLEN("a:0"));
            {
                AEEDeviceInfo di;
                AEERect  rc;
                
                ISHELL_GetDeviceInfo( me->a.m_pIShell, &di);
                me->screenWidth = di.cxScreen;
                me->screenHeight= di.cyScreen;
                SETAEERECT(&rc, 0, 0, me->screenWidth, me->screenHeight);
                IDisplay_SetClipRect(me->a.m_pIDisplay, &rc);
            }
            initParametersAccordingToScreenSize();
            startOneNewGame();
            return TRUE;
        }
        case IDS_TETRIS_CONTINUE:
        {
            me->continueGame = FALSE;
            (void)IDISPLAY_SetPrefs(me->a.m_pIDisplay, "a:0", STRLEN("a:0"));
            {
                AEEDeviceInfo di;
                AEERect    rc;
                
                ISHELL_GetDeviceInfo( me->a.m_pIShell, &di);
                me->screenWidth = di.cxScreen;
                me->screenHeight= di.cyScreen;
                SETAEERECT(&rc, 0, 0, me->screenWidth, me->screenHeight);
                IDisplay_SetClipRect(me->a.m_pIDisplay, &rc);                
            }
            initParametersAccordingToScreenSize();
            setGameState(GAME_STATE_PAUSED);
            redrawTheScreen();
            return TRUE;
        }
        case IDS_TETRIS_LEVEL:
        {
            displayLevelSettingScreen();
            return TRUE;
        }
        case IDS_TETRIS_SOUND:
        {
            displaySoundSettingScreen();
            return TRUE;
        }
        case IDS_TETRIS_GRID:
        {
            displayGridSettingScreen();
            return TRUE;
        }
        case IDS_TETRIS_SCORE:
        {
            displayHerosScreen();
            return TRUE;
        }
        case IDS_TETRIS_HELP:
        {
            displayHelpScreen();
            return TRUE;
        }
        case IDS_TETRIS_LEVEL_1:
        case IDS_TETRIS_LEVEL_2:
        case IDS_TETRIS_LEVEL_3:
        {
            uint16 selID = IMENUCTL_GetSel( me->menuListStyle) - IDS_TETRIS_LEVEL_1;

            // If choose another level before continuing the game, reset the score.
            if( me->continueGame && me->gameLevel != selID + 1)
            {
                me->gameScore = 0;
            }

            me->gameLevel = selID + 1;
            me->sleepTime = ( MAX_LEVEL - selID) * INIT_TIME / MAX_LEVEL;
            me->gameSpeed = ( INIT_TIME - me->sleepTime) / TIME_SLICE;

            displayMainMenu();
            return TRUE;
        }
        // Choose to enter the next level.
        case IDS_YES:
        {
            me->gameLevel += 1;
            setGameState(GAME_STATE_RUNNING);
            redrawTheScreen();
            return TRUE;
        }
        case IDS_NO: // Refuse to enter the next level
        case IDS_TETRIS_OK:
        {

            displayMainMenu();
            return TRUE;
        }
        // Reset the scores.
        case IDS_CLEAR:
        {
            clearHerosScore();
            return TRUE;
        }
        case IDS_GRID_ON:
        case IDS_GRID_OFF:
        {
            if(me->gameState == GAME_STATE_GRID_SETTING)
            {
                me->drawGridLines = (commandId == IDS_GRID_ON) ? TRUE : FALSE;
            }
            else if(me->gameState == GAME_STATE_SOUND_SETTING)
            {
                me->soundOn = (commandId == IDS_GRID_ON) ? TRUE : FALSE;;
            }
            displayMainMenu();
            return TRUE;
        }

    } // switch( commandId)

    return FALSE;
} // processCommand

static boolean processKey(  uint16 keyCode)
{

    if(me->gameState == GAME_STATE_SPLASH)
    {

        ISHELL_CancelTimer( me->a.m_pIShell, (PFNNOTIFY)displayMainMenu, me);
        displayMainMenu();
        return TRUE;
    }

    switch( keyCode)
    {

        // Move leftward
        case AVK_LEFT:
        case AVK_4:
        {

            if(me->gameState == GAME_STATE_RUNNING  &&
                !me->theFallingTetris.pinned        &&
                me->flashCounterWhenDeleteDirtyRows == 0
            )
            {

                moveLeftwardsIf();
                return TRUE;
            }
        }
        break;

        // Move rightward
        case AVK_RIGHT:
        case AVK_6:
        {

            if(me->gameState == GAME_STATE_RUNNING  &&
                !me->theFallingTetris.pinned        &&
                me->flashCounterWhenDeleteDirtyRows == 0
            )
            {

                moveRightwardsIf();
                return TRUE;
            }
        }
        break;

        // Rotate
        case AVK_UP:
        case AVK_2:
        {

            if(me->gameState == GAME_STATE_RUNNING  &&
                me->theFallingTetris.model != 6     &&
                !me->theFallingTetris.pinned        &&
                me->flashCounterWhenDeleteDirtyRows == 0
            )
            {

                rotateIf();
                return TRUE;
            }
        }
        break;

        // Move downward
        case AVK_DOWN:
        case AVK_8:
        {
            if(me->gameState == GAME_STATE_RUNNING  &&
                !me->theFallingTetris.pinned        &&
                me->flashCounterWhenDeleteDirtyRows == 0
            )
            {

                ISHELL_CancelTimer(me->a.m_pIShell, (PFNNOTIFY)moveDownwardsIf, me);
                me->moveDownwardAccelerated = TRUE;
                moveDownwardsIf();
                return TRUE;
            }
        }
        break;

        case AVK_5:
        case AVK_SEND:
#if defined( AEE_SIMULATOR)
        case AVK_SOFT1:
#else
        case AVK_SELECT:
#endif
#if defined( AEE_SIMULATOR)
        case AVK_SELECT:
#else
        case AVK_INFO:
#endif
        {
            uint16 menuItemId = 0;

            menuItemId = IMENUCTL_GetSel(
                    IMENUCTL_IsActive( me->menuSoftkey) ?
                            me->menuSoftkey : me->menuListStyle
                    );
            if( menuItemId == 0)
            {
                if( me->gameState == GAME_STATE_HERO        ||
                    me->gameState == GAME_STATE_HELP        ||
                    me->gameState == GAME_STATE_REPORT      ||
                    me->gameState == GAME_STATE_LASTLEVEL
                )
                {
                    displayMainMenu();
                    return TRUE;
                }
                else if(me->gameState == GAME_STATE_NEXTLEVEL)
                {
                    me->gameLevel += 1;
                    setGameState(GAME_STATE_RUNNING);

                    redrawTheScreen();
                    return TRUE;
                }
                else if( pauseOrResumeGame())
                {
                    return TRUE;
                }
            }
            else
            {
                return ISHELL_PostEvent(me->a.m_pIShell,
                            AEECLSID_GAME_TETRIS,
                            EVT_COMMAND,
                            menuItemId,
                            0
                        );
            }

        }
        break;

/*#if defined( AEE_SIMULATOR)
        case AVK_SELECT:
#else
        case AVK_INFO:
#endif
        {
            if(me->gameState == GAME_STATE_HERO         ||
                me->gameState == GAME_STATE_HELP        ||
                me->gameState == GAME_STATE_REPORT      ||
                me->gameState == GAME_STATE_LASTLEVEL
            )
            {
                displayMainMenu();
                return TRUE;
            }
            else if(me->gameState == GAME_STATE_NEXTLEVEL)
            {
                me->gameLevel += 1;
                setGameState(GAME_STATE_RUNNING);

                redrawTheScreen();
                return TRUE;
            }
            else if( pauseOrResumeGame())
            {
                return TRUE;
            }
        }
        break;
*/

        case AVK_CLR:
        case AVK_SOFT2:
        {
            switch(me->gameState)
            {
                case GAME_STATE_MAIN_MENU:
                {
                    IRINGERMGR_Stop(me->ringerPlayer);
                    (void)ISHELL_CloseApplet(me->a.m_pIShell, FALSE);
                    return TRUE;
                }

                case GAME_STATE_LEVEL_SETTING:
                case GAME_STATE_SOUND_SETTING:
                case GAME_STATE_HELP:
                case GAME_STATE_REPORT:
                case GAME_STATE_GRID_SETTING:
                case GAME_STATE_HERO:
                case GAME_STATE_NEXTLEVEL:
                {
                    displayMainMenu();
                    return TRUE;
                }

                case GAME_STATE_RUNNING:
                case GAME_STATE_PAUSED:
                {
                    IRINGERMGR_Stop(me->ringerPlayer);
                    (void)ISHELL_CancelTimer(me->a.m_pIShell, NULL, me);
                    me->continueGame = TRUE;
                    displayMainMenu();
                    return TRUE;
                }

                default:
                    return TRUE;
            } // switch( me->appState)

        } // AVK_CLR, AVK_SOFT2

    } // switch( wParam)

    return FALSE;
} // processKey


static void setGameState(GameStateEnum gameState)
{
    me->previousGameState   = me->gameState;
    me->gameState           = gameState;
}

static void startOneNewGame(  void)
{

    resetControlsAndClearScreen(  TRUE);
    initGameData();
    drawPlayingZoneBorder();
    drawGameScoreAndGameSpeed();

    setGameState(GAME_STATE_RUNNING);

    me->theFallingTetris.pinned = FALSE;
    calculateAbsoluteCoordinates( &me->theNextFallingTetris,
            generateRandomTetrisModel(),
            me->xWhereToDrawTheNextFallingTetris,
            me->yWhereToDrawTheNextFallingTetris);
    launchOneTetris();
} // startOneNewGame

static void launchOneTetris( void)
{

    uint8  i;

    if( pinTheFallingTetrisIf())
    {
        if( deleteDirtyRowsIf() && !updateGameScoreAndGoToNextLevelIf())
        {
            return;
        }
    }

    calculateAbsoluteCoordinates( &me->theFallingTetris,
            me->theNextFallingTetris.model,
            me->xWhereToLaunchTetris,
            me->yWhereToLaunchTetris
        );
    for( i = 0; i < 4; i ++)
    {
        if(theGridIsDirtyOrInvalid(me->theFallingTetris.block[i].x, me->theFallingTetris.block[i].y))
        {
            drawTheFinalTetrisWhichFillupThePlayingZone(me->theFallingTetris.block[i].y);

            playMusic(MUSIC_GAMEOVER);
            ISHELL_SetTimer(me->a.m_pIShell, OVER_TIME, (PFNNOTIFY)gameOver, me);
            return;
        }
    }
    drawTheFallingTetris();
    me->theFallingTetris.pinned = FALSE;

    clearTheNextFallingTetris();
    calculateAbsoluteCoordinates( &me->theNextFallingTetris,
            generateRandomTetrisModel(),
            me->xWhereToDrawTheNextFallingTetris,
            me->yWhereToDrawTheNextFallingTetris);
    drawTheNextFallingTetris();

    me->moveDownwardAccelerated = FALSE;
    ISHELL_SetTimer(me->a.m_pIShell, me->sleepTime, (PFNNOTIFY)moveDownwardsIf, me);
} // launchOneTetris

static void initGameData(  void)
{

    int i = 0;
    int j = 0;

    if( me->gameLevel == 0 || me->gameLevel > MAX_LEVEL)
    {
        me->gameLevel = 1;
    }

    me->continueGame                = FALSE;
    me->flashCounterWhenDeleteDirtyRows = 0;
    me->dirtyRowNumber          = 0;
    me->gameScore                   = 0;

    me->theTopmostRowOfThePinnedTetrises = me->gridRowNumber - 1;
    me->sleepTime   = ( MAX_LEVEL - me->gameLevel + 1) * INIT_TIME / MAX_LEVEL;
    me->gameSpeed   = ( INIT_TIME - me->sleepTime) / TIME_SLICE;

    me->gridToClear.x  = me->playingZone.x;
    me->gridToClear.y  = me->playingZone.y + me->playingZone.dy;
    me->gridToClear.dx = me->gridSideLength - 1;
    me->gridToClear.dy = me->gridSideLength - 1;

    for(i = 0; i < MAX_REM_LINES; i++)
    {
        me->dirtyRowIndex[i]  = 0;
    }

    for( i = 0; i < me->gridRowNumber; i++)
    {
        for( j = 0; j < me->gridColumnNumber; j++)
        {
            me->gridMatrix[i][j].dirty      = FALSE;
            me->gridMatrix[i][j].tetrisModel = MAX_BLOCK_KIND;
        }
        me->gridRowMap[i] = 0;
    }
} // initGameData

static uint8 generateRandomTetrisModel( void)
{
    uint8   array[1];
    uint8   rand = 0;
    uint32  sum  = 0;

    GETRAND( array, 1);
    sum  = GETTIMESECONDS() + array[0];
    rand = sum % MAX_BLOCK_KIND;

    return rand;
}

static void calculateAbsoluteCoordinates(  CTetris* tetris,
        byte model,
        int16 xOrigin,
        int16 yOrigin
)
{

    int i = 0;

    tetris->model = model;
    for( i = 0; i < 4; i ++)
    {
        tetris->block[i].x = xOrigin + TETRIS_MODEL[model].block[i].x * me->gridSideLength;
        tetris->block[i].y = yOrigin + TETRIS_MODEL[model].block[i].y * me->gridSideLength;
    }
}

static boolean canDoAction( CTetris* tetris, ActionTypeEnum action)
{

    byte    i = 0;
    int16   x = 0;
    int16   y = 0;
    boolean returnValue = FALSE;

    switch( action)
    {

        case ACTION_DOWN:
        {

            for( i = 0; i < 4; i ++)
            {
                x = tetris->block[i].x;
                y = tetris->block[i].y + me->gridSideLength;

                if( theGridIsDirtyOrInvalid( x, y) ||
                    ( y + me->gridSideLength > me->playingZone.y + me->playingZone.dy)
                )
                {
                    returnValue = FALSE;
                    goto _canDoAction_return_;
                }

                tetris->block[i].y = y;
            }
            returnValue = TRUE;
            goto _canDoAction_return_;
        }

        case ACTION_LEFT:
        {

            for( i = 0; i < 4; i ++)
            {
                x = tetris->block[i].x - me->gridSideLength;
                y = tetris->block[i].y;

                if( theGridIsDirtyOrInvalid( x, y) || ( x < me->playingZone.x))
                {
                    returnValue = FALSE;
                    goto _canDoAction_return_;
                }

                tetris->block[i].x = x;
            }
            returnValue = TRUE;
            goto _canDoAction_return_;
        }

        case ACTION_RIGHT:
        {

            for( i = 0; i < 4; i ++)
            {
                x = tetris->block[i].x + me->gridSideLength;
                y = tetris->block[i].y;

                if( theGridIsDirtyOrInvalid( x, y) ||
                    ( x + me->gridSideLength > me->playingZone.x + me->playingZone.dx)
                )
                {
                    returnValue = FALSE;
                    goto _canDoAction_return_;
                }

                tetris->block[i].x = x;
            }
            returnValue = TRUE;
            goto _canDoAction_return_;
        }

        case ACTION_ROTATE:
        {

            int16 xAxis = tetris->block[tetris->axis].x;
            int16 yAxis = tetris->block[tetris->axis].y;
            int   xMin  =  3000;
            int   xMax  = -3000;
            int   yMin  =  3000;
            int   yMax  = -3000;

            for( i = 0; i < 4; i ++)
            {
                x = xAxis + yAxis - tetris->block[i].y;
                y = yAxis + tetris->block[i].x - xAxis;

                if( theGridIsDirty( x, y) ||
                    y + me->gridSideLength > me->playingZone.y + me->playingZone.dy
                )
                {

                    debug( ";rotate, grid[%d][%d] is dirty",
                            (x - me->playingZone.x) / me->gridSideLength,
                            (y - me->playingZone.y) / me->gridSideLength
                         );
                    returnValue = FALSE;
                    goto _canDoAction_return_;
                }

                tetris->block[i].x = x;
                tetris->block[i].y = y;

                if( xMax < x)
                {
                    xMax = x;
                }
                if( yMax < y)
                {
                    yMax = y;
                }
                if( xMin > x)
                {
                    xMin = x;
                }
                if( yMin > y)
                {
                    yMin = y;
                }
            }
            debug( ";xMin: %d, xMax: %d, yMin: %d, yMax: %d", xMin, xMax, yMin, yMax);

            // left blocked
            if( xMin < me->playingZone.x)
            {
                int distance = me->playingZone.x - xMin;

                for( i = 0; i < 4; i ++)
                {
                    tetris->block[i].x += distance;

                    if( theGridIsDirty( tetris->block[i].x, tetris->block[i].y) ||
                        tetris->block[i].y + me->gridSideLength > me->playingZone.y + me->playingZone.dy
                    )
                    {
                        debug( ";rotate, moveRight %d, grid[%d][%d] is dirty",
                                distance,
                                (tetris->block[i].x - me->playingZone.x) / me->gridSideLength,
                                (tetris->block[i].y - me->playingZone.y) / me->gridSideLength
                             );

                        returnValue = FALSE;
                        goto _canDoAction_return_;
                    }
                }
            }
            // right blocked
            if( xMax + me->gridSideLength > me->playingZone.x + me->playingZone.dx)
            {
                int distance = ( xMax + me->gridSideLength) - ( me->playingZone.x + me->playingZone.dx);

                for( i = 0; i < 4; i ++)
                {
                    tetris->block[i].x -= distance;

                    if( theGridIsDirty( tetris->block[i].x, tetris->block[i].y) ||
                        tetris->block[i].y + me->gridSideLength > me->playingZone.y + me->playingZone.dy
                    )
                    {
                        debug( ";rotate, moveLeft %d, grid[%d][%d] is dirty",
                                distance,
                                (tetris->block[i].x - me->playingZone.x) / me->gridSideLength,
                                (tetris->block[i].y - me->playingZone.y) / me->gridSideLength
                             );

                        returnValue = FALSE;
                        goto _canDoAction_return_;
                    }
                }
            }
            // top blocked
            if( yMin < me->playingZone.y)
            {
                int distance = me->playingZone.y - yMin;

                for( i = 0; i < 4; i ++)
                {
                    tetris->block[i].y += distance;

                    if( theGridIsDirty( tetris->block[i].x, tetris->block[i].y) ||
                        tetris->block[i].y + me->gridSideLength > me->playingZone.y + me->playingZone.dy
                    )
                    {
                        debug( ";rotate, moveDown %d, grid[%d][%d] is dirty",
                                distance,
                                (tetris->block[i].x - me->playingZone.x) / me->gridSideLength,
                                (tetris->block[i].y - me->playingZone.y) / me->gridSideLength
                             );

                        returnValue = FALSE;
                        goto _canDoAction_return_;
                    }
                }
            }

            returnValue = TRUE;
            goto _canDoAction_return_;
        }

    } // switch( action)

_canDoAction_return_:

    return returnValue;
} // canDoAction

static boolean moveDownwardsIf( void)
{

    boolean returnValue = TRUE;
    CTetris tetris;

    memcpy( &tetris, &me->theFallingTetris, sizeof( tetris));
    clearTheFallingTetris();

    if( me->moveDownwardAccelerated)
    {

        while( canDoAction( &tetris, ACTION_DOWN))
        {
            memcpy( &me->theFallingTetris, &tetris, sizeof( tetris));
        }
        me->theFallingTetris.pinned = TRUE;
    }
    else
    {

        if( canDoAction( &tetris, ACTION_DOWN))
        {
            memcpy( &me->theFallingTetris, &tetris, sizeof( tetris));
        }
        else
        {
            me->theFallingTetris.pinned = TRUE;
        }
    }
    drawTheFallingTetris();

    if(me->theFallingTetris.pinned  || !canDoAction(&tetris, ACTION_DOWN) )
    {
        me->theFallingTetris.pinned = TRUE;

        launchOneTetris();
        returnValue = FALSE;
        goto _moveDownwardIf_return_;
    }

    ISHELL_SetTimer(me->a.m_pIShell, me->sleepTime, (PFNNOTIFY)moveDownwardsIf, me);

_moveDownwardIf_return_:
    return returnValue;
} // moveDownwardsIf

static void doLeftRightRotateAction( ActionTypeEnum action)
{

    if( action == ACTION_LEFT || action == ACTION_RIGHT || action == ACTION_ROTATE)
    {
        CTetris tetris;
        memcpy( &tetris, &me->theFallingTetris, sizeof( tetris));
        if( canDoAction( &tetris, action))
        {
            clearTheFallingTetris();
            memcpy( &me->theFallingTetris, &tetris, sizeof( tetris));
            drawTheFallingTetris();
        }
    }
}

static void moveLeftwardsIf( void)
{
    doLeftRightRotateAction( ACTION_LEFT);
}

static void moveRightwardsIf( void)
{
    doLeftRightRotateAction( ACTION_RIGHT);
}

static void rotateIf( void)
{
    doLeftRightRotateAction( ACTION_ROTATE);
}

static boolean pinTheFallingTetrisIf( void)
{

    if( me->theFallingTetris.pinned)
    {
        int i   = 0;
        int row = 0;
        int col = 0;

        for( i = 0; i < 4; i ++)
        {
            row = ( me->theFallingTetris.block[i].y - me->playingZone.y) / me->gridSideLength;
            col = ( me->theFallingTetris.block[i].x - me->playingZone.x) / me->gridSideLength;

            me->gridMatrix[row][col].dirty       = TRUE;
            me->gridMatrix[row][col].tetrisModel = me->theFallingTetris.model;
            me->gridRowMap[row] |= 1 << ( me->gridColumnNumber - 1 - col);

            if( row < me->theTopmostRowOfThePinnedTetrises)
            {
                me->theTopmostRowOfThePinnedTetrises = row;
            }
        }
    } // if( pinned)

    return me->theFallingTetris.pinned;
} //pinTheFallingTetrisIf

static boolean theGridIsDirtyOrInvalid( int16 x, int16 y)
{
    int16 row       = (y - me->playingZone.y) / me->gridSideLength;
    int16 column    = (x - me->playingZone.x) / me->gridSideLength;

    if( row < 0     || row >= me->gridRowNumber         ||
        column < 0  || column >= me->gridColumnNumber
    )
    {
        return TRUE;
    }
    else
    {
        return me->gridMatrix[row][column].dirty;
    }
}

static boolean theGridIsDirty( int16 x, int16 y)
{
    int16 row       = (y - me->playingZone.y) / me->gridSideLength;
    int16 column    = (x - me->playingZone.x) / me->gridSideLength;

    if( row < 0     || row >= me->gridRowNumber         ||
        column < 0  || column >= me->gridColumnNumber
    )
    {
        return FALSE;
    }
    else
    {
        return me->gridMatrix[row][column].dirty;
    }
}

static int getTheBottomMostYCoordinateOfTheFallingTetris( void)
{

    int i = 0;
    int y = 0;

    for( i = 0; i < 4; i ++)
    {
        if( y < me->theFallingTetris.block[i].y)
        {
            y = me->theFallingTetris.block[i].y;
        }
    }

    return y;
}

static boolean deleteDirtyRowsIf(  void)
{

    int i = 0;
    int j = 0;
    int k = 0;

    int row                             = 0;
    int yOfTheFallingTetrisBottomMost   = 0;

    AEERect rect;
    rect.dx = me->gridSideLength - 1;
    rect.dy = rect.dx;

    yOfTheFallingTetrisBottomMost = getTheBottomMostYCoordinateOfTheFallingTetris();
    row = ( yOfTheFallingTetrisBottomMost - me->playingZone.y) / me->gridSideLength;
    for( i = 0; row >= 0 && i < 4; i ++)
    {

        if( !theRowIsDirty( row))
        {
            row --;
            continue;
        }

        me->dirtyRowIndex[me->dirtyRowNumber] = row - me->dirtyRowNumber;
        me->dirtyRowNumber += 1;

        for(j = row; j >= (me->theTopmostRowOfThePinnedTetrises - 1); j--)
        {
            for( k = 0; k < me->gridColumnNumber; k ++)
            {
                me->gridMatrix[j][k].dirty       = me->gridMatrix[j-1][k].dirty;
                me->gridMatrix[j][k].tetrisModel = me->gridMatrix[j-1][k].tetrisModel;
                me->gridRowMap[j]                = me->gridRowMap[j-1];
            }
        }

    } // for( i = 0; row >= 0 && i < 4; i ++)

    if( me->dirtyRowNumber != 0)
    {

        if( me->dirtyRowNumber == 1)
        {
            playMusic( MUSIC_SINLINE);
        }
        else
        {
            playMusic( MUSIC_MULLINES);
        }
        (void)ISHELL_CancelTimer(me->a.m_pIShell, (PFNNOTIFY)moveDownwardsIf, me);
        drawTheFallingTetris();
        flashWhenDeleteDirtyRows();// Produce flash effect where there are full lines.
    }
    else
    {
        playMusic( MUSIC_PLACE);
    }

    return me->dirtyRowNumber > 0;
} //deleteDirtyRowsIf

static boolean pauseOrResumeGame( void)
{

    // to pause game
    if(me->gameState == GAME_STATE_RUNNING)
    {

        if( me->flashCounterWhenDeleteDirtyRows != 0)
        {

            refreshPlayingZoneAfterDeleteDirtyRows();

            drawTheFallingTetris();
            me->flashCounterWhenDeleteDirtyRows = 0;
            me->dirtyRowNumber = 0;
        }

        ISHELL_CancelTimer( me->a.m_pIShell, NULL, me);
        setGameState(GAME_STATE_PAUSED);

        promptTheGameIsPausedNow();
        return TRUE;
    }
    else if(me->gameState == GAME_STATE_PAUSED)
    {

        setGameState(GAME_STATE_RUNNING);

        redrawTheScreen();
        return TRUE;
    }

    return FALSE;
} // pauseOrResume

//---------------------------- UI related method definition
static void displaySplashScreen(  void)
{

    IImage          *pImage = NULL;
    AEERect         rect;
    AEEImageInfo    rImageInfo;

    SETAEERECT( &rect, 0, 0, me->screenWidth, me->screenHeight);
	#ifdef FEATURE_VERSION_IVIO
	pImage = ISHELL_LoadResImage( me->a.m_pIShell, TETRISIMAGES_IVIO_RES_FILE, IDB_TETRIS_SPLASH);
	#else
    pImage = ISHELL_LoadResImage( me->a.m_pIShell, TETRISIMAGES_RES_FILE, IDB_TETRIS_SPLASH);
	#endif

    if( pImage != NULL)
    {
        AECHAR title[16];

        // draw splash image
        IDISPLAY_ClearScreen( me->a.m_pIDisplay);
        IDISPLAY_FillRect( me->a.m_pIDisplay, &rect, MAKE_RGB( 0, 0xcf, 0xff));
        IIMAGE_GetInfo( pImage, &rImageInfo);

        IIMAGE_Draw( pImage,
                     ( me->screenWidth - rImageInfo.cx)  >> 1,
                     me->screenHeight - rImageInfo.cy
                );

        // draw title
        ISHELL_LoadResString( me->a.m_pIShell, TETRIS_RES_FILE_LANGUAGE, IDS_TETRIS_TITLE, title, 32);
        rect.dx  = IDISPLAY_MeasureText( me->a.m_pIDisplay, AEE_FONT_NORMAL, title);
        rect.x   = ( me->screenWidth - rect.dx) >> 1;
        rect.y   = ( me->screenHeight - rImageInfo.cy - me->fontHeight) >> 1;
        draw3DText( rect.x, rect.y, title);

        // refresh the screen
        IDISPLAY_UpdateEx( me->a.m_pIDisplay, FALSE);
        IIMAGE_Release( pImage);

        ISHELL_SetTimer(me->a.m_pIShell, INIT_TIME, (PFNNOTIFY)displayMainMenu, me);
        setGameState(GAME_STATE_SPLASH);
    }
    else
    {
        displayMainMenu();
    }

} // displaySplashScreen

static void drawText3( int16 x, int16 y, AECHAR* text, RGBVAL color, uint32 flags)
{

    RGBVAL oldColor = IDISPLAY_SetColor( me->a.m_pIDisplay, CLR_USER_TEXT, color);

    IDISPLAY_DrawText( me->a.m_pIDisplay,
            AEE_FONT_NORMAL,
            text,
            -1,
            x,
            y,
            NULL,
            IDF_TEXT_TRANSPARENT | flags
    );

    IDISPLAY_SetColor( me->a.m_pIDisplay, CLR_USER_TEXT, oldColor);
}

static void draw3DText( int16 x, int16 y, AECHAR* text)
{

//    drawText3( x, y - 1, text, RGB_WHITE, 0);
//    drawText3( x - 1, y + 1, text, RGB_BLACK, 0);
//    drawText3( x + 1, y + 1, text, RGB_BLACK, 0);
    drawText3( x, y, text, MAKE_RGB( 0xff, 0xcc, 0x00), 0);
}

static void displayMainMenu( void)
{

    AEERect rect;

    (void)IDISPLAY_SetPrefs(me->a.m_pIDisplay, "a:1", STRLEN("a:1"));
    {
        AEEDeviceInfo di;
        
        ISHELL_GetDeviceInfo( me->a.m_pIShell, &di);
        me->screenWidth = di.cxScreen;
        me->screenHeight= di.cyScreen;
    }

    SETAEERECT(&rect, 0, 0, me->screenWidth, me->screenHeight);

    setGameState(GAME_STATE_MAIN_MENU);
    resetControlsAndClearScreen(TRUE);

    IMENUCTL_SetTitle(me->menuListStyle, TETRIS_RES_FILE_LANGUAGE, IDS_TETRIS_TITLE, NULL );

    addMenuItem( me->menuListStyle, IDS_TETRIS_START,IDB_TETRIS_START);
    if( me->continueGame)
    {
        addMenuItem( me->menuListStyle, IDS_TETRIS_CONTINUE, IDB_TETRIS_CONTINUE);
    }
    addMenuItem( me->menuListStyle, IDS_TETRIS_LEVEL, IDB_TETRIS_LEVEL);
    addMenuItem( me->menuListStyle, IDS_TETRIS_SOUND, IDB_TETRIS_SOUND);
    addMenuItem( me->menuListStyle, IDS_TETRIS_GRID,  IDB_TETRIS_GRID);
    addMenuItem( me->menuListStyle, IDS_TETRIS_SCORE, IDB_TETRIS_SCORE);
    addMenuItem( me->menuListStyle, IDS_TETRIS_HELP,  IDB_TETRIS_HELP);

    IMENUCTL_SetOemProperties(me->menuListStyle, OEMMP_USE_MENU_STYLE);
    IMENUCTL_SetRect( me->menuListStyle, &rect);
    IMENUCTL_SetSel(me->menuListStyle, me->selectedMenuItem[GAME_STATE_MAIN_MENU]);
    IMENUCTL_SetBottomBarType( me->menuListStyle, BTBAR_SELECT_BACK);
    IMENUCTL_SetActive(me->menuListStyle, TRUE);
} // displayMainMenu

static void displayLevelSettingScreen( void)
{

    uint16  i               = 0;
    uint16  imageId         = 0;
    uint16  itemId          = 0;
    uint16  selectedMenuItemId  = IDS_TETRIS_LEVEL_1;
    AEERect rect;

    resetControlsAndClearScreen( FALSE);

    rect.dx = me->screenWidth * 2 / 3;
    rect.dy = MENUITEM_HEIGHT * 3;
    rect.x  = me->screenWidth / 6;
    rect.y  = ( ( me->screenHeight - rect.dy) >> 1);
    drawPopWindowFrameBorder(  IDS_TETRIS_LEVEL, &rect);

    for( i = 0; i < MAX_LEVEL; i ++)
    {

        itemId = i + IDS_TETRIS_LEVEL_1;
        if( me->gameLevel == (i + 1))
        {
            imageId         = IDB_GAME_LEVEL_SEL;
            selectedMenuItemId  = itemId;
        }
        else
        {
            imageId = IDB_GAME_LEVEL_UNSEL;
        }

        addMenuItem( me->menuListStyle, itemId, imageId);
    }

    IMENUCTL_SetOemProperties(me->menuListStyle, OEMMP_USE_MENU_STYLE);
    if(me->previousGameState == GAME_STATE_SUSPENDED)
    {
        IMENUCTL_SetSel(me->menuListStyle, me->selectedMenuItem[GAME_STATE_LEVEL_SETTING]);
    }
    else
    {
        IMENUCTL_SetSel(me->menuListStyle, selectedMenuItemId);
    }

    IMENUCTL_SetRect(me->menuListStyle, &rect);
    IMENUCTL_SetBottomBarType( me->menuListStyle, BTBAR_NONE);
    IMENUCTL_SetActive(me->menuListStyle, TRUE);
    setGameState(GAME_STATE_LEVEL_SETTING);
} // displayLevelSettingScreen

static void displaySoundSettingScreen( void)
{

    uint16  selectedMenuItemId = 0;
    AEERect rect;

    resetControlsAndClearScreen( FALSE);

    rect.dx = me->screenWidth * 2 / 3;
    rect.dy = MENUITEM_HEIGHT << 1;
    rect.x  = me->screenWidth / 6;
    rect.y  = ( ( me->screenHeight - rect.dy) >> 1);
    drawPopWindowFrameBorder(  IDS_TETRIS_SOUND, &rect);

    selectedMenuItemId = me->soundOn ? IDS_GRID_ON : IDS_GRID_OFF;
    addMenuItem(me->menuListStyle, IDS_GRID_ON, me->soundOn ? IDB_GAME_LEVEL_SEL : IDB_GAME_LEVEL_UNSEL );
    addMenuItem(me->menuListStyle, IDS_GRID_OFF, me->soundOn ? IDB_GAME_LEVEL_UNSEL : IDB_GAME_LEVEL_SEL );

    IMENUCTL_SetOemProperties(me->menuListStyle, OEMMP_USE_MENU_STYLE);

    if(me->previousGameState == GAME_STATE_SUSPENDED)
    {
        IMENUCTL_SetSel(me->menuListStyle, me->selectedMenuItem[GAME_STATE_SOUND_SETTING]);
    }
    else
    {
        IMENUCTL_SetSel(me->menuListStyle, selectedMenuItemId);
    }

    IMENUCTL_SetRect(me->menuListStyle, &rect);
    IMENUCTL_SetBottomBarType( me->menuListStyle, BTBAR_NONE);
    IMENUCTL_SetActive(me->menuListStyle, TRUE);
    setGameState(GAME_STATE_SOUND_SETTING);
} // displaySoundSettingScreen

static void displayGridSettingScreen( void)
{

    AEERect rect;

    resetControlsAndClearScreen(  FALSE);

    rect.dx = me->screenWidth * 2 / 3;
    rect.dy = MENUITEM_HEIGHT << 1;
    rect.x  = me->screenWidth / 6;
    rect.y  = ( ( me->screenHeight - rect.dy) >> 1);
    drawPopWindowFrameBorder(  IDS_TETRIS_GRID, &rect);

     addMenuItem( me->menuListStyle,
             IDS_GRID_ON,
             me->drawGridLines ? IDB_GAME_LEVEL_SEL : IDB_GAME_LEVEL_UNSEL );
     addMenuItem( me->menuListStyle,
             IDS_GRID_OFF,
             me->drawGridLines ? IDB_GAME_LEVEL_UNSEL : IDB_GAME_LEVEL_SEL );

    IMENUCTL_SetOemProperties(me->menuListStyle, OEMMP_USE_MENU_STYLE);

    if(me->previousGameState == GAME_STATE_SUSPENDED)
    {
        IMENUCTL_SetSel(me->menuListStyle, me->selectedMenuItem[GAME_STATE_GRID_SETTING]);
    }
    else
    {
        IMENUCTL_SetSel(me->menuListStyle, me->drawGridLines ? IDS_GRID_ON:IDS_GRID_OFF);
    }
    IMENUCTL_SetRect(me->menuListStyle, &rect);
    IMENUCTL_SetBottomBarType( me->menuListStyle, BTBAR_NONE);
    IMENUCTL_SetActive(me->menuListStyle, TRUE);
    setGameState(GAME_STATE_GRID_SETTING);
} // displayGridSettingScreen

static void displayHerosScreen( void)
{

    int  i;
    int  y;
    char szFormat[] = "%d";
    AEERect rect;
    AECHAR Buff[30];
    AECHAR Score[20];
    AECHAR wstrFor[50];
    RGBVAL OldTextClr;

    resetControlsAndClearScreen(  FALSE);

    // draw frame border
    rect.dx = me->screenWidth * 5 / 6;
    rect.dy = me->fontHeight * 5 + 6;
    rect.x  = me->screenWidth / 12;
    rect.y  = ( ( me->screenHeight - rect.dy - me->fontHeight - 13) >> 1) + 5 + me->fontHeight;
    drawPopWindowFrameBorder(  IDS_TETRIS_SCORE, &rect);
//    IDISPLAY_FillRect( me->a.m_pIDisplay, &rect, 0);

    OldTextClr = IDISPLAY_SetColor( me->a.m_pIDisplay, CLR_USER_TEXT, MAKE_RGB( 0x80, 0, 0));
    (void)ISHELL_LoadResString( me->a.m_pIShell,
                               TETRIS_RES_FILE_LANGUAGE,
                               IDS_HERO_TITLE,
                               Buff,
                               sizeof(Buff));

    // draw title
    y  = rect.y;
    IDISPLAY_DrawText( me->a.m_pIDisplay,
                            AEE_FONT_NORMAL,
                            Buff,
                            -1,
                            0,
                            y,
                            &rect,
                            IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);

    // draw the top score list
    STRTOWSTR( szFormat, wstrFor, sizeof(wstrFor));
    IDISPLAY_SetColor(me->a.m_pIDisplay, CLR_USER_TEXT, MAKE_RGB( 0, 0, 0xff));
    rect.dx >>= 1;
    for( i = 0; i < MAX_LEVEL; i ++)
    {

        y += me->fontHeight;
        MEMSET(Buff, 0 , sizeof(Buff));
        MEMSET(Buff, 0 , sizeof(Score));

        ISHELL_LoadResString( me->a.m_pIShell,
                             TETRIS_RES_FILE_LANGUAGE,
                             (int16)(i + IDS_TETRIS_LEVEL_1),
                             Buff,
                             sizeof(Buff));
        rect.x = me->screenWidth / 12;
        IDISPLAY_DrawText( me->a.m_pIDisplay, AEE_FONT_NORMAL,
                                Buff,
                                -1,
                                0,
                                y,
                                &rect,
                                IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);

        WSPRINTF( Score, sizeof(Score), wstrFor, me->configData.topScore[i]);
        rect.x += rect.dx;
        IDISPLAY_DrawText(me->a.m_pIDisplay,
                                AEE_FONT_NORMAL,
                                Score,
                                -1,
                                0,
                                y,
                                &rect,
                                IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);

    }

    IDISPLAY_SetColor(me->a.m_pIDisplay, CLR_USER_TEXT, OldTextClr);

    IMENUCTL_AddItem(me->menuSoftkey, TETRIS_RES_FILE_LANGUAGE, IDS_TETRIS_OK,
                           IDS_TETRIS_OK, NULL, NULL );
    IMENUCTL_AddItem(me->menuSoftkey, TETRIS_RES_FILE_LANGUAGE, IDS_CLEAR, IDS_CLEAR,
                           NULL, NULL );

    rect.dx = me->screenWidth * 5 / 6;
    rect.x  = me->screenWidth / 12;
    rect.y  = rect.y + rect.dy - GetBottomBarHeight( me->a.m_pIDisplay);
    rect.dy = GetBottomBarHeight( me->a.m_pIDisplay);

    IMENUCTL_SetRect( me->menuSoftkey, &rect);
    if(me->previousGameState == GAME_STATE_SUSPENDED)
    {
        IMENUCTL_SetSel(me->menuSoftkey, me->selectedMenuItem[GAME_STATE_HERO]);
    }

    IMENUCTL_SetActive(me->menuSoftkey, TRUE);
    setGameState(GAME_STATE_HERO);
} // displayHerosScreen

static void displayHelpScreen( void)
{

    AEERect  rect;
    AECHAR  *lineEscapePosition;
    AECHAR   title[32];
    AECHAR   helpString[1200];

    resetControlsAndClearScreen( TRUE);

    // load help content
    ISHELL_LoadResString(me->a.m_pIShell, TETRIS_RES_FILE_LANGUAGE, IDS_TETRIS_HELP,
                          title, sizeof(title) );

    ISHELL_LoadResString(me->a.m_pIShell, TETRIS_RES_FILE_LANGUAGE, IDS_THELP_CONTENT,
                          helpString, sizeof(helpString) );

    lineEscapePosition = WSTRCHR( helpString,(AECHAR)'\\');
    while( lineEscapePosition != NULL)
    {
        *lineEscapePosition = (AECHAR)'\n'; // replace '\' in the string to '\n'.
        lineEscapePosition  = WSTRCHR( helpString, (AECHAR)'\\');
    }

    // show the help content with the static control
    rect.x  = 0;
    rect.y  = 0;
    rect.dx = me->screenWidth;
    rect.dy = me->screenHeight - GetBottomBarHeight( me->a.m_pIDisplay)- 4;

    ISTATIC_SetRect( me->staticControl, &rect);
    ISTATIC_SetProperties( me->staticControl, ST_CENTERTITLE | ST_UNDERLINE | ST_NOSCROLL);
    ISTATIC_SetText(me->staticControl, title, helpString, AEE_FONT_BOLD, AEE_FONT_NORMAL);
    ISTATIC_SetActive(me->staticControl, TRUE);
    ISTATIC_Redraw( me->staticControl);

    // draw softkey menu
    IMENUCTL_AddItem( me->menuSoftkey,
                      TETRIS_RES_FILE_LANGUAGE,
                      IDS_TETRIS_OK,
                      IDS_TETRIS_OK,
                      NULL,
                      NULL
                 );
    SETAEERECT( &rect,
                0,
                me->screenHeight - GetBottomBarHeight( me->a.m_pIDisplay) - 4,
                me->screenWidth,
                GetBottomBarHeight( me->a.m_pIDisplay) + 4
            );
    IMENUCTL_SetRect( me->menuSoftkey, &rect);
    IMENUCTL_SetActive( me->menuSoftkey, TRUE);
    
    setGameState(GAME_STATE_HELP);
} // displayHelpScreen

static void gotoNextLevel( boolean isLastLevel)
{

    uint16  gameLevelStartFromZero;
    uint16  levelInitSpeed;
    uint16  titleId;
    uint16  totalScore;
    uint32  staticProperties;
    AEERect rect;
    AECHAR  formatString[128];
    AECHAR  displayString[128];

    ISHELL_CancelTimer( me->a.m_pIShell, NULL, me);
    resetControlsAndClearScreen( FALSE);

    gameLevelStartFromZero  = me->gameLevel - 1;
    titleId                 = IDS_TETRIS_LEVEL_1 + gameLevelStartFromZero;

    rect.dx = me->screenWidth * 5 / 6;
    rect.dy = me->screenHeight * 5 / 6 - me->fontHeight - 13;
    rect.x  = me->screenWidth / 12;
    rect.y  = rect.x + me->fontHeight + 5;
    drawPopWindowFrameBorder( titleId, &rect);

    levelInitSpeed = (INIT_TIME - ( ( MAX_LEVEL - gameLevelStartFromZero) * INIT_TIME / MAX_LEVEL)) / TIME_SLICE;
    totalScore = (me->gameSpeed - levelInitSpeed) * LEVEL_SCORE;
    totalScore += isLastLevel ? me->gameScore : 0;
    if( me->configData.topScore[gameLevelStartFromZero] < totalScore)
    {
        me->configData.topScore[gameLevelStartFromZero] = totalScore;
        saveSettingData();
    }
    ISHELL_LoadResString( me->a.m_pIShell,
                    TETRIS_RES_FILE_LANGUAGE,
                    isLastLevel ? IDS_PASS_LAST : IDS_PASS,
                    formatString,
                    sizeof( formatString)
                 );
    WSPRINTF( displayString, sizeof( displayString), formatString, totalScore);

    rect.dy -=  me->fontHeight + 4;
    staticProperties = ST_CENTERTITLE | ST_UNDERLINE | ST_CENTERTEXT;
    ISTATIC_SetRect( me->staticControl, &rect);
    ISTATIC_SetProperties( me->staticControl, staticProperties);
    ISTATIC_SetText( me->staticControl,
                NULL,
                displayString,
                AEE_FONT_NORMAL,
                AEE_FONT_NORMAL
             );

    ISTATIC_SetActive( me->staticControl, TRUE);
    ISTATIC_Redraw( me->staticControl);

    rect.y  = rect.y + rect.dy;
    rect.dy = GetBottomBarHeight( me->a.m_pIDisplay);

    if( isLastLevel)
    {
       IMENUCTL_AddItem( me->menuSoftkey,
                         TETRIS_RES_FILE_LANGUAGE,
                         IDS_TETRIS_OK,
                         IDS_TETRIS_OK,
                         NULL,
                         NULL
            );
    }
    else
    {
        IMENUCTL_AddItem( me->menuSoftkey,
                          TETRIS_RES_FILE_LANGUAGE,
                          IDS_YES,
                          IDS_YES,
                          NULL,
                          NULL
            );
        IMENUCTL_AddItem( me->menuSoftkey,
                          TETRIS_RES_FILE_LANGUAGE,
                          IDS_NO,
                          IDS_NO,
                          NULL,
                          NULL
            );
    }

    IMENUCTL_SetRect( me->menuSoftkey, &rect);
    IMENUCTL_SetActive( me->menuSoftkey, TRUE);
    setGameState(isLastLevel ? GAME_STATE_LASTLEVEL : GAME_STATE_NEXTLEVEL);
} // gotoNextLevel

static void clearHerosScore( void)
{
    int i;
    boolean returnToMainMenu;

    returnToMainMenu = TRUE;
    for( i = 0; i < MAX_LEVEL; i ++)
    {
        if( me->configData.topScore[i] != 0)
        {
            returnToMainMenu = FALSE;
            me->configData.topScore[i] = 0;
        }
    }

    if( returnToMainMenu)
    {
        displayMainMenu();
    }
    else
    {
        saveSettingData();
        displayHerosScreen();
    }

} // clearHerosScore

static void resetControlsAndClearScreen( boolean clearScreen)
{

    if( IMENUCTL_IsActive( me->menuListStyle))
    {

        IMENUCTL_Reset( me->menuListStyle);
        IMENUCTL_SetActive( me->menuListStyle, FALSE);
    }

    if( IMENUCTL_IsActive( me->menuSoftkey))
    {

        IMENUCTL_Reset( me->menuSoftkey);
        IMENUCTL_SetActive( me->menuSoftkey, FALSE);
    }

    if( ISTATIC_IsActive( me->staticControl))
    {

        ISTATIC_Reset( me->staticControl);
        ISTATIC_SetActive( me->staticControl, FALSE);
    }

    if( clearScreen)
    {

        IDISPLAY_ClearScreen( me->a.m_pIDisplay);
    }
} // resetControlsAndClearScreen

static void addMenuItem( IMenuCtl * pMenu, uint16 itemId, uint16 imageId)
{

    CtlAddItem ai;

    ai.pText        = NULL;
    ai.pImage       = NULL;
	#ifdef FEATURE_VERSION_IVIO
	ai.pszResImage  = TETRISIMAGES_IVIO_RES_FILE;
	#else
    ai.pszResImage  = TETRISIMAGES_RES_FILE;
	#endif
    ai.pszResText   = TETRIS_RES_FILE_LANGUAGE;
    ai.wFont        = AEE_FONT_NORMAL;
    ai.dwData       = 0;

    ai.wText    = itemId;
    ai.wImage   = imageId;
    ai.wItemID  = itemId;

    IMENUCTL_AddItemEx( pMenu, &ai);
} // addMenuItem


static void drawPopWindowFrameBorder(  uint16 titleID, AEERect *rectFrame)
{

    AEERect rect;
    AECHAR  title[50];
    RGBVAL  oldTextColor;

    rect.x  = rectFrame->x  - 5;
    rect.y  = rectFrame->y  - 5 - me->fontHeight;
    rect.dx = rectFrame->dx + 10;
    rect.dy = rectFrame->dy + 13 + me->fontHeight;

    if( !IGRAPHICS_GetFillMode( me->graphics))
    {
        IGRAPHICS_SetFillMode( me->graphics, TRUE);
    }
    IGRAPHICS_SetFillColor( me->graphics, 0xcd, 0xcd, 0xcd, 0);
    IGRAPHICS_SetColor( me->graphics, 0, 0, 0, 0);
    IGRAPHICS_DrawRoundRectangle( me->graphics, &rect, 20, 20);

    rect.x  += 5;
    rect.y  += 2 + me->fontHeight;
    rect.dx -= 10;
    rect.dy  = 3;
    IDISPLAY_FillRect( me->a.m_pIDisplay, &rect, 0);
    rect.y = rectFrame->y + rectFrame->dy;
    IDISPLAY_FillRect( me->a.m_pIDisplay, &rect, 0);

    ISHELL_LoadResString( me->a.m_pIShell,
                          TETRIS_RES_FILE_LANGUAGE,
                          titleID,
                          title,
                          sizeof(title)
              );

    rect.x  = rectFrame->x;
    rect.y  = rectFrame->y  - 3 - me->fontHeight;
    rect.dx = rectFrame->dx;
    rect.dy = 22;
    oldTextColor = IDISPLAY_SetColor( me->a.m_pIDisplay,
                                      CLR_USER_TEXT,
                                      MAKE_RGB( 0xff, 0x66, 0x00)
                                );
    IDISPLAY_DrawText( me->a.m_pIDisplay,
                            AEE_FONT_NORMAL,
                            title,
                            -1,
                            0,
                            0,
                            &rect,
                            IDF_ALIGN_CENTER | IDF_ALIGN_TOP | IDF_TEXT_TRANSPARENT
           );

    IDISPLAY_SetColor( me->a.m_pIDisplay, CLR_USER_TEXT, oldTextColor);
    IGRAPHICS_Update( me->graphics);
} // drawFrameBorder

static void drawPlayingZoneBorder( void)
{

    AEERect rect;

    // to clear the screen with background color
    rect.x  = 0;
    rect.y  = 0;
    rect.dx = me->screenWidth;
    rect.dy = me->screenHeight;
    IDISPLAY_FillRect( me->a.m_pIDisplay, &rect, COLOR_BACKGROUND);

    // to draw the border of the playing zone
    // 1. draw inner border
    rect.x  =  me->playingZone.x - 1;
    rect.y  =  me->playingZone.y - 1;
    rect.dx = me->playingZone.dx + 2;
    rect.dy = me->playingZone.dy + 2;
    IDISPLAY_DrawRect( me->a.m_pIDisplay,
                      &rect,
                      MAKE_RGB( 20, 90,141),
                      0,
                      IDF_RECT_FRAME
                      );
    // 2. draw outer border
    rect.x  -=  1;
    rect.y  -=  1;
    rect.dx += 2;
    rect.dy += 2;
    IDISPLAY_DrawRect( me->a.m_pIDisplay,
                      &rect,
                      MAKE_RGB( 0xf0, 0x80, 0x80),
                      0,
                      IDF_RECT_FRAME
                      );

    // to draw grid lines
    if( me->drawGridLines)
    {
        drawGridLines();
    }

}

static void drawGridLines( void)
{

    int     i = 0;
    AEELine line;

    IGRAPHICS_SetColor( me->graphics, 0x00, 0x00, 0x8b, 0);
    line.sx = me->playingZone.x - 1;
    line.sy = me->playingZone.y - 1;
    line.ex = line.sx;
    line.ey = me->playingZone.y + me->playingZone.dy - 1;
    for(i = 1; i < me->gridColumnNumber; i ++)
    {
        line.sx += me->gridSideLength;
        line.ex += me->gridSideLength;
        IGRAPHICS_DrawLine(me->graphics, &line);
    }
} // drawGridLines

static void promptTheGameIsPausedNow( void)
{

    int16   x       = 0;
    int16   y       = 0;
    IImage  *imagePaused = NULL;
    AEEImageInfo info;

    imagePaused = ISHELL_LoadResImage( me->a.m_pIShell, TETRIS_RES_FILE_LANGUAGE, IDI_PAUSED);
    if( imagePaused == NULL)
    {
        return;
    }

    IIMAGE_GetInfo( imagePaused, &info);
    x = ( ( me->playingZone.dx - info.cx) >> 1) + me->playingZone.x;
    y = ( ( me->playingZone.dy - info.cy) >> 1) + me->playingZone.y;
    IIMAGE_Draw( imagePaused, x, y);

    IIMAGE_Release( imagePaused);
    IDISPLAY_UpdateEx( me->a.m_pIDisplay, FALSE);
} // promptTheGameIsPausedNow

static void drawGameScoreAndGameSpeed( void)
{

    uint16  x           = 0;
    uint16  y           = 0;
    uint16  digitWidth  = 0;
    int8    i           = 0;
    uint16  power       = 0;
    AEEImageInfo imageInfo;
#ifndef WIN32
#if !defined( AEE_SIMULATOR)
     extern int OEM_GetConfig( AEEConfigItem i, void * pBuff, int nSize);
     nv_language_enum_type language = 0;
    OEM_GetConfig( CFGI_LANGUAGE_SELECTION, &language, sizeof(language));
#endif
#else
	extern int OEM_GetConfig( AEEConfigItem i, void * pBuff, int nSize);
     nv_language_enum_type language = 0;
    OEM_GetConfig( CFGI_LANGUAGE_SELECTION, &language, sizeof(language));
#endif//WIN32
    IIMAGE_GetInfo( me->imageScore, &imageInfo);
    x = me->screenWidth - imageInfo.cx;
    y = me->yWhereToDrawTheNextFallingTetris + 4 * me->gridSideLength;
    if( language == NV_LANGUAGE_HEBREW) 
    {
	 IIMAGE_Draw( me->imageScore, x, y);
    }
    else
    {
    	IIMAGE_Draw( me->imageScore, x-2, y);
    }
    y += imageInfo.cy + 2;
    IIMAGE_GetInfo( me->imageNumber, &imageInfo);
    digitWidth = imageInfo.cx / 10;
    IIMAGE_SetDrawSize( me->imageNumber, digitWidth, imageInfo.cy);
    x = me->screenWidth;
    for( i = 0, power = 1; i < 4; i ++, power *= 10)
    {

        x -= digitWidth + 2;
        IIMAGE_SetOffset( me->imageNumber, digitWidth * ( me->gameScore / power % 10), 0);
        IIMAGE_Draw( me->imageNumber, x, y);
    }

    IIMAGE_GetInfo( me->imageSpeed, &imageInfo);
    x = me->screenWidth - imageInfo.cx;
    y += imageInfo.cy + 6;
    if( language == NV_LANGUAGE_HEBREW) 
    {
	 IIMAGE_Draw( me->imageSpeed, x, y);
    }
    else
    {
    	IIMAGE_Draw( me->imageSpeed, x-2, y);
    }

    y += imageInfo.cy + 2;
    x = me->screenWidth;
    for( i = 0, power = 1; i < 4; i ++, power *= 10)
    {

        x -= digitWidth + 2;
        IIMAGE_SetOffset( me->imageNumber, digitWidth * ( me->gameSpeed / power % 10), 0);
        IIMAGE_Draw( me->imageNumber, x, y);
    }

} // drawGameScoreAndGameSpeed

static void redrawTheScreen( void)
{
    int  i,j;
    char tetrisModel;
    AEERect rect;

    resetControlsAndClearScreen(  TRUE);
    drawPlayingZoneBorder();
    drawGameScoreAndGameSpeed();

    me->flashCounterWhenDeleteDirtyRows = 0;
    me->dirtyRowNumber                  = 0;
    rect.dx                             = me->gridSideLength - 1;
    rect.dy                             = rect.dx;

    for( i = me->gridRowNumber - 1; i >= me->theTopmostRowOfThePinnedTetrises; i --)
    {

        for( j = 0; j < me->gridColumnNumber; j ++)
        {

            if( me->gridMatrix[i][j].dirty)
            {
                rect.x      = j * me->gridSideLength + me->playingZone.x;
                rect.y      = i * me->gridSideLength + me->playingZone.y;
                tetrisModel     = me->gridMatrix[i][j].tetrisModel;

                IDISPLAY_DrawRect( me->a.m_pIDisplay,
                                    &rect,
                                    0,
                                    COLOR_PALETTE[tetrisModel],
                                    IDF_RECT_FILL );
            }
        }
    }

    drawTheFallingTetris();
    drawTheNextFallingTetris();

    if(me->gameState == GAME_STATE_PAUSED)
    {
        promptTheGameIsPausedNow();
    }
    else if(me->gameState == GAME_STATE_NEXTLEVEL)
    {
        gotoNextLevel(FALSE);
    }
    else if(me->gameState == GAME_STATE_RUNNING)
    {
        moveDownwardsIf();
    }
    else
    {
        IDISPLAY_UpdateEx(me->a.m_pIDisplay, FALSE);
    }
}

static void drawTetris( CTetris *tetris, boolean bOverDraw)
{

    int     i = 0;
    RGBVAL  color = 0;
    AEERect rect;

    color = bOverDraw ? COLOR_BACKGROUND : COLOR_PALETTE[tetris->model];
    for( i = 0; i < 4; i ++)
    {

        rect.x  = tetris->block[i].x;
        rect.y  = tetris->block[i].y;
        rect.dx = me->gridSideLength - 1;
        rect.dy = me->gridSideLength - 1;

        IDISPLAY_DrawRect( me->a.m_pIDisplay, &rect, 0, color, IDF_RECT_FILL);
    }

    IDISPLAY_UpdateEx( me->a.m_pIDisplay, FALSE);
}

static void drawTheFallingTetris( void)
{

    drawTetris( &me->theFallingTetris, FALSE);
}

static void clearTheFallingTetris( void)
{

    drawTetris( &me->theFallingTetris, TRUE);
}

static void drawTheNextFallingTetris( void)
{

    drawTetris( &me->theNextFallingTetris, FALSE);
}

static void clearTheNextFallingTetris( void)
{

    drawTetris( &me->theNextFallingTetris, TRUE);
}

static boolean playMusic( MusicTypeEnum type)
{

    char fileName[64];
    byte volumeLevel = 0;
    byte volchangedValue = 5;
#if !defined( AEE_SIMULATOR)
    db_items_value_type dbItemValue = {0};
    db_get( DB_IN_USE, &dbItemValue);
    if( dbItemValue.in_use)
    {
        return FALSE;
    }
#endif

    if( me->soundOn == FALSE)
    {
        return FALSE;
    }
(void)ICONFIG_SetItem( me->config,
                      CFGI_RINGER_VOL,
                      &volchangedValue,
                      sizeof(byte)
                );

#if !defined( AEE_SIMULATOR)
    (void)ICONFIG_GetItem( me->config,
                          CFGI_RINGER_VOL,
                          &volumeLevel,
                          sizeof(byte)
                    );

    if(volumeLevel == 0)
    {
        return FALSE;
    }
#endif

    MEMSET(fileName, 0, sizeof(fileName));
    STRCAT(fileName, "fs:/mod/tetris/");

    switch(type)
    {
        case MUSIC_PLACE:
            STRCAT( fileName, "blink.mid");
            break;

        case MUSIC_SINLINE:
            STRCAT( fileName, "singleline.mid");
            break;

        case MUSIC_MULLINES:
            STRCAT( fileName, "mullines.mid");
            break;

        case MUSIC_NEXTSPEED:
            STRCAT( fileName, "hero.mid");
            break;

        case MUSIC_NEXTLEVEL:
            STRCAT( fileName, "success.mid");
            break;

        case MUSIC_GAMEOVER:
            STRCAT( fileName, "fail.mid");
            break;

        default:
            return FALSE;
    }

    IRINGERMGR_Stop(me->ringerPlayer);
    IRINGERMGR_PlayFile(me->ringerPlayer, fileName, 0);
    return TRUE;
} // playMusic

static void refreshPlayingZoneAfterDeleteDirtyRows( void)
{

    int i = 0;
    int j = 0;
    int tetrisModel;
    AEERect rect;

    for( i = me->theTopmostRowOfThePinnedTetrises; i <= me->dirtyRowIndex[0]; i++)
    {
        for(j = 0; j < me->gridColumnNumber; j++)
        {
            rect.x = j * me->gridSideLength + me->playingZone.x;
            rect.y = i * me->gridSideLength + me->playingZone.y;
            rect.dx = rect.dy = me->gridSideLength - 1;
            if( !me->gridMatrix[i][j].dirty)    // Use background color.
            {
                IDISPLAY_DrawRect(me->a.m_pIDisplay, &rect, 0, COLOR_BACKGROUND, IDF_RECT_FILL);
            }
            else                                    // Use color of the block.
            {
                tetrisModel = me->gridMatrix[i][j].tetrisModel;
                IDISPLAY_DrawRect(me->a.m_pIDisplay, &rect, 0, COLOR_PALETTE[tetrisModel], IDF_RECT_FILL);
            }
        }
    }
    me->theTopmostRowOfThePinnedTetrises += me->dirtyRowNumber;
    IDISPLAY_UpdateEx(me->a.m_pIDisplay, FALSE);
} // refreshPlayingZoneAfterDeleteDirtyRows

static boolean updateGameScoreAndGoToNextLevelIf( void)
{

    switch( me->dirtyRowNumber)
    {
        case 1:
            me->gameScore += 10;
            break;

        case 2:
            me->gameScore += 30;
            break;

        case 3:
            me->gameScore += 50;
            break;

        case 4:
            me->gameScore += 100;
            break;
    }

    if( me->gameScore >= LEVEL_SCORE)
    {

        playMusic( MUSIC_NEXTSPEED);

        me->gameSpeed += 1;
        me->gameScore -= LEVEL_SCORE;
        me->sleepTime -= TIME_SLICE;
        drawGameScoreAndGameSpeed();

        if(me->sleepTime <= FLASH_TIME)
        {

            playMusic( MUSIC_NEXTLEVEL);
            (void)ISHELL_CancelTimer(me->a.m_pIShell, NULL, me);
            gotoNextLevel( TRUE);
            return FALSE;
        }
        else if( me->gameSpeed ==
                 ( INIT_TIME - (MAX_LEVEL - me->gameLevel + 1) * INIT_TIME
                         / MAX_LEVEL
                 ) / TIME_SLICE
        )
        {

            playMusic( MUSIC_NEXTLEVEL);

            ISHELL_CancelTimer(me->a.m_pIShell, NULL, me);
            gotoNextLevel( FALSE);
            return FALSE;
        }
    }

    drawGameScoreAndGameSpeed();
    return TRUE;
} // updateGameScoreAndGoToNextLevelIf

static void drawTheFinalTetrisWhichFillupThePlayingZone( int16 yFrowWhereToDrawTheTetris)
{

    if( yFrowWhereToDrawTheTetris <= me->playingZone.y)
    {
        return;
    }
    else
    {

        int i       = 0;
        int height  = 0;
        int row     = 0;
        int col     = 0;
        AEERect rect;

        rect.dx = me->gridSideLength - 1;
        rect.dy = rect.dx;

        height  = yFrowWhereToDrawTheTetris - me->playingZone.y;
        row     = 0;
        col     = 0;
        for( i = 0; i < 4; i++)
        {

            rect.x = me->theFallingTetris.block[i].x;
            rect.y = me->theFallingTetris.block[i].y - height;

            if( rect.y < me->playingZone.y)
            {
                continue;
            }

            IDISPLAY_DrawRect( me->a.m_pIDisplay,
                              &rect,
                              0,
                              COLOR_PALETTE[me->theFallingTetris.model],
                              IDF_RECT_FILL
                     );
            row = (rect.y - me->playingZone.y) / me->gridSideLength;
            col = (rect.x - me->playingZone.x) / me->gridSideLength;

            me->gridMatrix[row][col].dirty      = TRUE;
            me->gridMatrix[row][col].tetrisModel = me->theFallingTetris.model;
        }

        IDISPLAY_UpdateEx(me->a.m_pIDisplay, FALSE);
    }
} // drawTheFinalTetrisWhichFillupThePlayingZone

static void flashWhenDeleteDirtyRows( void)
{
    int     i       = 0;
    int     j       = 0;
    uint32  color   = 0;
    AEERect rect;

    if( me->flashCounterWhenDeleteDirtyRows % 2 == 0)
    {
        color = MAKE_RGB(205,179,139);
    }
    else
    {
        color = COLOR_BACKGROUND;
    }

    rect.dx = me->gridSideLength - 1;
    rect.dy = me->gridSideLength - 1;
    for( i = 0; i < me->dirtyRowNumber; i++)
    {

        rect.x = me->playingZone.x;
        rect.y = me->dirtyRowIndex[i] * me->gridSideLength + me->playingZone.y;
        for( j = 0; j < me->gridColumnNumber; j++)
        {
            IDISPLAY_FillRect( me->a.m_pIDisplay, &rect, color);
            rect.x += me->gridSideLength;
        }
    }
    IDISPLAY_UpdateEx(me->a.m_pIDisplay, FALSE);

    if(me->flashCounterWhenDeleteDirtyRows < MAX_FLASH_TIMES)
    {

        me->flashCounterWhenDeleteDirtyRows += 1;
        ISHELL_SetTimer(me->a.m_pIShell, FLASH_TIME, (PFNNOTIFY)flashWhenDeleteDirtyRows, me);
    }
    else
    {

        refreshPlayingZoneAfterDeleteDirtyRows();

        me->flashCounterWhenDeleteDirtyRows = 0;
        me->dirtyRowNumber = 0;
        drawTheFallingTetris();

        ISHELL_SetTimer(me->a.m_pIShell, me->sleepTime, (PFNNOTIFY)moveDownwardsIf, me);
    }
} // flashWhenDeleteDirtyRows

static void saveSettingData( void)
{

    me->configData.gameLevel        = me->gameLevel;
    me->configData.drawGridLines    = me->drawGridLines;
    me->configData.soundOn          = me->soundOn;

    ISHELL_SetPrefs( me->a.m_pIShell,
                     AEECLSID_GAME_TETRIS,
                     TETRIS_VERSION,
                     &( me->configData),
                     sizeof( CPrefData)
          );
}

static void gameOver( void)
{

    setGameState(GAME_STATE_OVER);

    ISHELL_CancelTimer(me->a.m_pIShell, NULL, me);
    clearPlayingZoneAfterGameOver();
}

static void clearPlayingZoneAfterGameOver( void)
{

    int column = 0;

    if( me->gridToClear.y > me->playingZone.y)
    {
        me->gridToClear.y -= me->gridSideLength;
    }
    else
    {
        saveGameDataAndDisplayGameScore();
        return;
    }

    me->gridToClear.x = me->playingZone.x;
    for( column = 0; column < me->gridColumnNumber; column ++)
    {
        IDISPLAY_FillRect( me->a.m_pIDisplay, &me->gridToClear, MAKE_RGB( 0, 0xcf, 0xff));
        me->gridToClear.x += me->gridSideLength;
    }
    IDISPLAY_UpdateEx(me->a.m_pIDisplay, FALSE);

    ISHELL_SetTimer(me->a.m_pIShell, OVER_TIME, (PFNNOTIFY)clearPlayingZoneAfterGameOver, me);
}

static void saveGameDataAndDisplayGameScore( void)
{

    int     gameLevelStartFromZero;
    uint16  baseSpeed;
    uint16  totalScore;

    gameLevelStartFromZero      = me->gameLevel - 1;
    baseSpeed   = (INIT_TIME - (( MAX_LEVEL - gameLevelStartFromZero) * INIT_TIME / MAX_LEVEL)) / TIME_SLICE;
    totalScore  = (me->gameSpeed - baseSpeed) * LEVEL_SCORE + me->gameScore;
    if( totalScore > me->configData.topScore[gameLevelStartFromZero])
    {
        me->configData.topScore[gameLevelStartFromZero] = totalScore;
        saveSettingData();
        displayGameScoreScreen( IDS_REPORT_RECORD, totalScore, TRUE);
    }
    else
    {
        displayGameScoreScreen( IDS_REPORT_SCORE, totalScore, FALSE);
    }
} // saveGameDataAndDisplayGameScore

static void displayGameScoreScreen( uint16 resourceId, uint32 theScore, boolean recordBroken)
{

    uint16   titleResourceId;
    uint32   staticProperties;
    AECHAR   title[16];
    AECHAR   scoreFormatString[128];
    AECHAR   scoreString[128];
    AEERect  rect;

    resetControlsAndClearScreen( FALSE);

    ISHELL_LoadResString( me->a.m_pIShell,
                        TETRIS_RES_FILE_LANGUAGE,
                        resourceId,
                        scoreFormatString,
                        sizeof( scoreFormatString)
                     );
    if( recordBroken)
    {
        WSPRINTF( scoreString, sizeof(scoreString), scoreFormatString, theScore);
    }
    else
    {
        WSPRINTF( scoreString, sizeof(scoreString), scoreFormatString, theScore, me->configData.topScore[me->gameLevel - 1]);
    }

    titleResourceId = me->gameLevel + IDS_TETRIS_LEVEL_1 - 1;
    ISHELL_LoadResString( me->a.m_pIShell,
                    TETRIS_RES_FILE_LANGUAGE,
                    titleResourceId,
                    title,
                    sizeof( title)
                  );

    rect.dx = me->screenWidth - 16;
    rect.dy = me->screenHeight - 6 - me->fontHeight - 13;
    rect.x  = ( me->screenWidth - rect.dx) >> 1;
    rect.y  = 3 + me->fontHeight + 5;
    drawPopWindowFrameBorder( titleResourceId, &rect);

    staticProperties = ST_CENTERTITLE | ST_UNDERLINE | ST_CENTERTEXT | ST_MIDDLETEXT | ST_NOSCROLL;
    ISTATIC_SetRect(me->staticControl, &rect);
    ISTATIC_SetProperties( me->staticControl, staticProperties);
    ISTATIC_SetText( me->staticControl,
                NULL,
                scoreString,
                AEE_FONT_BOLD | AEE_FONT_LARGE,
                AEE_FONT_NORMAL
            );
    ISTATIC_SetActive( me->staticControl, TRUE);
    ISTATIC_Redraw( me->staticControl);
    IDISPLAY_Update( me->a.m_pIDisplay);        // Display the relevant information.

    setGameState(GAME_STATE_REPORT);
} // displayGameScoreScreen


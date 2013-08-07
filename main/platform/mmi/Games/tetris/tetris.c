/*===============================================================================

FILE: tetris.c

===============================================================================*/
/*=============================================================================
                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  
when           who      what, where, why
----------     ---      ----------------------------------------------------------
2004-11-17     whw      Modify the source code, to support touch screen, solve 
                        problems existed, better the layout of screen.
2004-11-08     whw      Create this file.
    
=============================================================================*/

/*===============================================================================
                            INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "tetris.h"



#ifdef FEATURE_LCD_TOUCH_ENABLE
#define TETRIS_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif//FEATURE_LCD_TOUCH_ENABLE

/*-------------------------------------------------------------------
                            Function Prototypes
-------------------------------------------------------------------*/
#if defined AEE_STATIC
    static int TetrisMod_CreateInstance(AEECLSID ClsID, IShell * pIShell, 
                                        IModule * po, void ** ppObj);
    int TetrisMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif
    
static boolean Tetris_HandleEvent(CTetrisApp * pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean Tetris_InitAppData(CTetrisApp * pMe);
static boolean ProcessKey(CTetrisApp * pMe,uint16 wParam);
static void    Tetris_FreeAppData(CTetrisApp * pMe);

static void    SetGameScreen(CTetrisApp * pMe);
static void    SetGameData(CTetrisApp * pMe);
static void    InitBlockData(CTetrisApp * pMe);
static void    ResumeScreen(CTetrisApp * pMe);

static void    DisplaySplashScreen(CTetrisApp * pMe);
static void    DisplayMainMenu(CTetrisApp * pMe);
static void    DisplayLevelScreen(CTetrisApp * pMe);
static void    DisplaySoundScreen(CTetrisApp * pMe);
static void    DisplayGridScreen(CTetrisApp * pMe);
static void    DisplayHerosScreen(CTetrisApp * pMe);
static void    DisplayHelpScreen(CTetrisApp * pMe);
static void    EnterNextLevel(CTetrisApp * pMe, boolean bIsLastLevel);

static void    Reset(CTetrisApp * pMe, boolean bErase);
static void    SetMenuAttr(IMenuCtl * pMenu, uint32 clsID, boolean bPopMenu);
static void    AddMenuItem(IMenuCtl * pMenu, uint16 ItemID, uint16 ImageID);
static void    DrawCtrlFrame(CTetrisApp * pMe,uint16 titleID,AEERect * fRect);
static void    AddSoftKeyMenu(CTetrisApp * pMe, uint16 LeftItem, uint16 RightItem);
static boolean IMENUCTL_UpAndDownScroll_HandleEvent(IShell* ps, void* p, AEEEvent eCode, uint16 wParam);

static void    DrawBorder(CTetrisApp * pMe);
static void    DrawGrid(CTetrisApp * pMe);
static void    DrawButtons(CTetrisApp * pMe, boolean bDrawTwo, boolean bPaused);

static void    StartGame(CTetrisApp * pMe);
static void    MainFlow(CTetrisApp * pMe);

static uint8   CreateRandNum(CTetrisApp * pMe);
static void    SetBlockPoint(CTetrisApp * pMe, uint16 HMid, uint16 VStart);
static void    UpdateScore(CTetrisApp * pMe);
static void    ClearHerosScore(CTetrisApp * pMe);

static void    DrawBlocks(CTetrisApp * pMe, boolean bOverDraw);
static void    DrawNextBlocks(CTetrisApp * pMe);
static boolean IsFeasible(CTetrisApp * pMe, ActionType type);

static boolean JudgeAction(CTetrisApp * pMe, uint32 dwParam);
static void    GetMinMaxVal(CTetrisApp * pMe);
static void    MoveDown(CTetrisApp * pMe, boolean bAcc);
static void    MoveLeftRight(CTetrisApp * pMe, ActionType type);
static void    Rotate(CTetrisApp * pMe);

static void    Record(CTetrisApp * pMe);
static boolean HasBlock(CTetrisApp * pMe, uint16 row, uint16 col);
static void    RemoveLines(CTetrisApp * pMe);
static boolean PlayMusic(CTetrisApp * pMe, MusicType type);
static void    UpdateLines(CTetrisApp * pMe);
static void    Flash(CTetrisApp * pMe);
static boolean ChangeScore(CTetrisApp * pMe);
static void    CutBlocks(CTetrisApp * pMe, uint16 YPos);

static uint16  GetFinalYPos(CTetrisApp * pMe);
static void    SetCfgData(CTetrisApp * pMe);
static void    SaveSettingData(CTetrisApp * pMe);
static void    GameOver(CTetrisApp * pMe); 
static void    UpdateScreen(CTetrisApp * pMe);
static void    ReportScore(CTetrisApp * pMe, uint16 strID, uint32 Score, boolean NewRecord);
/*===============================================================================
                            FUNCTION DEFINITIONS
=============================================================================== */

/*=============================================================================
FUNCTION:  Tetris_GetModInfo

DESCRIPTION: Returns module information

PARAMETERS:
   *ps:
   **ppClasses:
   **pApps:
   *pnApps:
   *pwMinPriv:

RETURN VALUE:
   PFNMODENTRY:
=============================================================================*/
#if defined(AEE_STATIC)
PFNMODENTRY Tetris_GetModInfo(IShell      *ps,
                              AEECLSID   **ppClasses,
                              AEEAppInfo **pApps,
                              uint16      *pnApps,
                              uint16      *pwMinPriv)
{
   PARAM_NOT_REF(ps)
   PARAM_NOT_REF(ppClasses)
   PARAM_NOT_REF(pwMinPriv)

   *pApps = (AEEAppInfo *)&gaiTetris;
   *pnApps = sizeof(gaiTetris) / sizeof(AEEAppInfo);
   return (PFNMODENTRY)TetrisMod_Load;
}
/*=============================================================================
FUNCTION:  TetrisMod_Load

DESCRIPTION: Called by BREW to initialize the static module.

PARAMETERS:
   *ps:
   *pHelpers:
   **pMod:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
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
/*===========================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION
    This function is invoked while the app is being loaded. All Modules must provide this 
    function. Ensure to retain the same name and parameters for this function.
    In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
    that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
    clsID: [in]: Specifies the ClassID of the applet which is being loaded

    pIShell: [in]: Contains pointer to the IShell object. 

    pIModule: pin]: Contains pointer to the IModule object to the current module to which
    this app belongs

    ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
    of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
    int TetrisMod_CreateInstance(AEECLSID ClsID, IShell * pIShell, 
                                IModule * po, void ** ppObj)
#else
    int AEEClsCreateInstance(AEECLSID ClsID,IShell * pIShell,
                                IModule * po,void ** ppObj)
#endif
{
    CTetrisApp * pMe = NULL;
   
    if(ClsID == AEECLSID_TETRIS)
    {
        if(AEEApplet_New(sizeof(CTetrisApp),
                          ClsID,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)Tetris_HandleEvent,
                          (PFNFREEAPPDATA)Tetris_FreeAppData) == TRUE)    
        {
            pMe = (CTetrisApp *)*ppObj;
            if(Tetris_InitAppData(pMe) == TRUE)
            {
                return AEE_SUCCESS;
            }
            else
            {
                Tetris_FreeAppData(pMe);
                IAPPLET_Release((IApplet *)*ppObj);
                *ppObj = NULL;
                return EFAILED;
            }
        }
    }
    return EFAILED;
}
/*===========================================================================
FUNCTION Tetris_HandleEvent

DESCRIPTION
    This is the EventHandler for this app. All events to this app are handled in this
    function. All APPs must supply an Event Handler.

PROTOTYPE:
    boolean Tetris_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
    pi: Pointer to the AEEApplet structure. This structure contains information specific
    to this applet. It was initialized during the AEEClsCreateInstance() function.

    ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  None.
===========================================================================*/
static boolean Tetris_HandleEvent(CTetrisApp * pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{ 
    uint16  SelID;

    switch (eCode) 
    {
        case EVT_APP_START: 
			if(pMe->m_pIAnn != NULL)
            {	    			
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);				
				MSG_FATAL("***zzg Application EVT_APP_START EnableAnnunciatorBar TRUE",0,0,0);				   
            }
            #if 0
			{
                AECHAR WTitle[20] = {0};

				(void)ISHELL_LoadResString(pMe->a.m_pIShell,
										TETRIS_RES_FILE_LANG,								  
										IDS_TETRIS_TITLE,
										WTitle,
										sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
    			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            	}
            }
            #else
            if(pMe->m_pIAnn != NULL)
              {
  			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,NULL);
          	  }
            #endif
            DisplaySplashScreen(pMe);
            return TRUE;

        case EVT_APP_STOP:
            IRINGERMGR_Stop(pMe->m_pPlayer);
            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
            SaveSettingData(pMe);
            (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE;                // Close the applet 

        case EVT_APP_SUSPEND:        
            Reset(pMe, TRUE);
            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
            IRINGERMGR_Stop(pMe->m_pPlayer);
            return TRUE;

        case EVT_APP_RESUME:            // Resume the former status
            switch(pMe->m_AppState)
            {
            case APP_STATE_SPLASH:
                DisplaySplashScreen(pMe);
                return TRUE;

            case APP_STATE_MAIN:
            case APP_STATE_LEVEL:
            case APP_STATE_GRID:
            case APP_STATE_SOUND:
            case APP_STATE_LASTLEVEL:
            case APP_STATE_REPORT:
            case APP_STATE_HERO:
                DisplayMainMenu(pMe);
                return TRUE;

            case APP_STATE_HELP:
                DisplayHelpScreen(pMe);
                return TRUE;

            case APP_STATE_RUNNING:
                ResumeScreen(pMe);
                MoveDown(pMe, FALSE);
                return TRUE;

            case APP_STATE_NEXTLEVEL:
                ResumeScreen(pMe);
                EnterNextLevel(pMe, FALSE);
                return TRUE;

            case APP_STATE_PAUSED:
                ResumeScreen(pMe);
                return TRUE;

            case APP_STATE_OVER:
				MSG_FATAL("APP_STATE_OVER..............",0,0,0);
                UpdateScreen(pMe);
                return TRUE;

            default:
                break;
            }

        case EVT_COMMAND:
            if(IMENUCTL_IsActive(pMe->m_pMainMenu))
            {
                pMe->m_rSelMenuItem = IMENUCTL_GetSel(pMe->m_pMainMenu); 
            }       
			if(pMe->m_pIAnn != NULL)
            {	    			
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);				
				MSG_FATAL("***zzg Application EVT_APP_START EnableAnnunciatorBar TRUE",0,0,0);				   
            }// Record the selected item of the main menu

            switch(wParam)
            {
            case IDS_TETRIS_START:      // Start the game
                StartGame(pMe);
                return TRUE;

            case IDS_TETRIS_CONTINUE:   // Continue
                pMe->m_bContinue = FALSE;
                pMe->m_AppState = APP_STATE_RUNNING;
                ResumeScreen(pMe);
                MoveDown(pMe, FALSE);
                return TRUE;

            case IDS_TETRIS_LEVEL:      // Select level
                DisplayLevelScreen(pMe);
                return TRUE;

            case IDS_TETRIS_SOUND:      // Sound Setting
                DisplaySoundScreen(pMe);
                return TRUE;
                
            case IDS_TETRIS_GRID:       // Grid Setting
                DisplayGridScreen(pMe);
                return TRUE;
                                    
            case IDS_TETRIS_SCORE:      // Scores
                DisplayHerosScreen(pMe);
                return TRUE;

            case IDS_TETRIS_HELP:       // Help
                DisplayHelpScreen(pMe);
                return TRUE;
                
            case IDS_TETRIS_LEVEL_1:    // Set game level 
            case IDS_TETRIS_LEVEL_2:
            case IDS_TETRIS_LEVEL_3:
                SelID = IMENUCTL_GetSel(pMe->m_pMenu) - IDS_TETRIS_LEVEL_1;
                if(pMe->m_bContinue == TRUE && pMe->m_rLevel != SelID + 1)
                {
                    pMe->m_rScore = 0;
                }                       // If choose another level before continuing the game,
                                        // reset the score.
                pMe->m_rLevel = SelID + 1;
                pMe->m_rTime  = pMe->m_WaitingTime[SelID]; 
                pMe->m_rSpeed = (INIT_TIME - pMe->m_rTime) / TIME_SLICE;
                DisplayMainMenu(pMe);
                return TRUE;  
                
            case IDS_YES:               // Choose to enter the next level.
                pMe->m_rLevel += 1; 
                pMe->m_AppState = APP_STATE_RUNNING;
                ResumeScreen(pMe);
                MoveDown(pMe, FALSE);
                return TRUE;
              
            case IDS_NO:                // Refuse to enter the next level
            case IDS_TETRIS_OK:         
                DisplayMainMenu(pMe);   // Return to the main menu.
                return TRUE;

            case IDS_CLEAR:
                ClearHerosScore(pMe);   // Reset the scores.
                return TRUE;

            case IDS_GRID_ON:
                if(pMe->m_AppState == APP_STATE_GRID)
                {
                    pMe->m_bGrid = TRUE;
                }                       // Set grid in the game screen.
                else if(pMe->m_AppState == APP_STATE_SOUND)
                {
                    pMe->m_bSound = TRUE;
                }                       // Open the sound setting.
                DisplayMainMenu(pMe);
                return TRUE;

            case IDS_GRID_OFF:
                if(pMe->m_AppState == APP_STATE_GRID)
                {
                    pMe->m_bGrid = FALSE;
                }
                else if(pMe->m_AppState == APP_STATE_SOUND)
                {
                    pMe->m_bSound = FALSE;
                }
                DisplayMainMenu(pMe);
                return TRUE;

            default:
                break;
            }
#ifdef FEATURE_LCD_TOUCH_ENABLE
        case EVT_PEN_DOWN:
        case EVT_PEN_MOVE:
            
#if defined AEE_STATIC
            if(pMe->m_AppState == APP_STATE_MAIN ||
               pMe->m_AppState == APP_STATE_GRID ||
               pMe->m_AppState == APP_STATE_LEVEL||
               pMe->m_AppState == APP_STATE_HERO ||
               pMe->m_AppState == APP_STATE_HELP ||
               pMe->m_AppState == APP_STATE_SOUND)
            {
                /*if(TCLDrawBottomBar_HandleEvent(&pMe->m_BottomBar,
                                                pMe->a.m_pIShell,
                                                pMe->a.m_pIDisplay,
                                                eCode,
                                                wParam,
                                                dwParam))*/
                {
                    return TRUE;
                }
            }         
#endif
            break;           
            
        case EVT_PEN_UP:
           MSG_FATAL("Tetris_HandleEvent EVT_PEN_UP",0,0,0);
#if 0//defined AEE_STATIC       
            if(pMe->m_AppState == APP_STATE_MAIN ||
               pMe->m_AppState == APP_STATE_GRID ||
               pMe->m_AppState == APP_STATE_LEVEL||
               pMe->m_AppState == APP_STATE_HERO ||
               pMe->m_AppState == APP_STATE_HELP ||
               pMe->m_AppState == APP_STATE_SOUND)
            {
                /*if(TCLDrawBottomBar_HandleEvent(&pMe->m_BottomBar,
                                                pMe->a.m_pIShell,
                                                pMe->a.m_pIDisplay,
                                                eCode,
                                                wParam,
                                                dwParam))*/
                {
                    MSG_FATAL("Tetris_HandleEvent EVT_PEN_UP 1",0,0,0);
                    return TRUE;
                }
            }         
#endif    
            {
    			AEEDeviceInfo devinfo;
    			int nBarH ;
    			AEERect rc;
    			int16 wXPos = (int16)AEE_GET_X(dwParam);
    			int16 wYPos = (int16)AEE_GET_Y(dwParam);
                MSG_FATAL("Tetris_HandleEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
    			nBarH = GetBottomBarHeight(pMe->a.m_pIDisplay);
        
    			MEMSET(&devinfo, 0, sizeof(devinfo));
    			ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &devinfo);
    			SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

    			if(TETRIS_PT_IN_RECT(wXPos,wYPos,rc) && (pMe->m_AppState != APP_STATE_RUNNING))
    			{
    				if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//×ó
    				{
    					boolean rt =  Tetris_HandleEvent(pMe,EVT_KEY,AVK_SELECT,0);
    					return rt;
    				}
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//ÓÒ
    				{						
    					 boolean rt = Tetris_HandleEvent(pMe,EVT_KEY,AVK_CLR,0);
    					 return rt;
    				}
    			}    
                if(JudgeAction(pMe, dwParam) == TRUE)
                {
                    MSG_FATAL("Tetris_HandleEvent EVT_PEN_UP 2",0,0,0);
                    return TRUE;                // Handle stroke events 
                }
                if(IMENUCTL_IsActive(pMe->m_pMainMenu))
                {
                    MSG_FATAL("Tetris_HandleEvent EVT_PEN_UP 3",0,0,0);
                   if(IMENUCTL_HandleEvent(pMe->m_pMainMenu, eCode, wParam, dwParam))
                   {
                       MSG_FATAL("Tetris_HandleEvent EVT_PEN_UP 4",0,0,0); 
                       return TRUE; 
                   }
                }   
                if(IMENUCTL_IsActive(pMe->m_pMenu))
                {
                    MSG_FATAL("Tetris_HandleEvent EVT_PEN_UP 5",0,0,0);
                   if(IMENUCTL_HandleEvent(pMe->m_pMenu, eCode, wParam, dwParam))
                   {
                        MSG_FATAL("Tetris_HandleEvent EVT_PEN_UP 6",0,0,0);
                       return TRUE; 
                   }
                }   
                if(ISTATIC_IsActive(pMe->m_pStatic))
                {
                    if(ISTATIC_HandleEvent(pMe->m_pStatic, eCode, wParam, dwParam))
                    {
                        return TRUE;
                    }
                }  
                if (IMENUCTL_IsActive(pMe->m_pSoftKeyMenu))
                {
                    if(IMENUCTL_HandleEvent(pMe->m_pSoftKeyMenu, eCode, wParam, dwParam))
                    {
                        return TRUE;
                    }
                }                
                break;
        }
#endif
        case EVT_KEY:
		
            if(IMENUCTL_UpAndDownScroll_HandleEvent(pMe->a.m_pIShell, 
                                                    pMe->m_pMainMenu, 
                                                    eCode, 
                                                    wParam))
                return TRUE;
                        
            if(IMENUCTL_UpAndDownScroll_HandleEvent(pMe->a.m_pIShell, 
                                                    pMe->m_pMenu, 
                                                    eCode, 
                                                    wParam))
                return TRUE;            // Handle scroll event of the menu          
            if (ProcessKey(pMe,wParam) == TRUE)
            {
                return TRUE;
            }
            break;
    }

    if (IMENUCTL_IsActive(pMe->m_pMainMenu))
    {
        if(IMENUCTL_HandleEvent(pMe->m_pMainMenu, eCode, wParam, dwParam))
        {
            return TRUE;
        }
    }

    if (IMENUCTL_IsActive(pMe->m_pMenu))
    {
        if(IMENUCTL_HandleEvent(pMe->m_pMenu, eCode, wParam, dwParam))
        {
            return TRUE;
        }
    }

    if (IMENUCTL_IsActive(pMe->m_pSoftKeyMenu))
    {
        if(IMENUCTL_HandleEvent(pMe->m_pSoftKeyMenu, eCode, wParam, dwParam))
        {
            return TRUE;
        }
    }
	if(pMe->m_AppState == APP_STATE_HELP )
	{
		MSG_FATAL("ISTATIC_IsActive..................",0,0,0);
    	if(ISTATIC_HandleEvent(pMe->m_pStatic, eCode, wParam, dwParam))
    	{
        	return TRUE;
    	}
	}

    

    return FALSE;
}
/*===============================================================================
FUNCTION: ProcessKey

DESCRIPTION:
    This function handles key event in this applet. 

PARAMETERS:
    pMe    [in] - CTetrisApp Structure
    wParam [in] - The code of key pressed.

DEPENDENCIES:
    Assumes pMe is not NULL

RETURN VALUE:
    TRUE: If the pressing-key event has been processed.
    FALSE: Pressing- key event is not processed.

SIDE EFFECTS:
    None
===============================================================================*/
static boolean ProcessKey(CTetrisApp * pMe,uint16 wParam)
{
    if (pMe == NULL) 
    {
        return FALSE;
    }

    switch(wParam)
    {
    case AVK_LEFT:              // Move leftward
        if(pMe->m_AppState == APP_STATE_RUNNING && pMe->m_rFlashTimes == 0)
        {
            MoveLeftRight(pMe, ACTION_LEFT);
            return TRUE;
        }
        break;

    case AVK_RIGHT:             // Move rightward
        if(pMe->m_AppState == APP_STATE_RUNNING && pMe->m_rFlashTimes == 0)
        {
            MoveLeftRight(pMe, ACTION_RIGHT);
            return TRUE;
        }   
        break;
        
    case AVK_UP:
    case AVK_SOFT1:             // Rotate
        if(pMe->m_AppState == APP_STATE_RUNNING && pMe->m_rKind != 6 
           && pMe->m_rFlashTimes == 0)
        {
            Rotate(pMe);
            return TRUE;
        }
        else if(wParam == AVK_SOFT1)
        {
            if(pMe->m_AppState == APP_STATE_HELP ||
               pMe->m_AppState == APP_STATE_REPORT)
            {
                DisplayMainMenu(pMe);
                return TRUE;                //tcl whewei 041124 add
            }
        }
        break;

    case AVK_DOWN:              // Move down
        if(pMe->m_AppState == APP_STATE_RUNNING && pMe->m_rFlashTimes == 0)
        {
            MoveDown(pMe, TRUE);
            return TRUE;
        }
        break;

    case AVK_STAR:              // Pause or continue
        if(pMe->m_AppState == APP_STATE_RUNNING)
        {
            if(pMe->m_rFlashTimes != 0)
            {
                UpdateLines(pMe);               // Flash times reached, redraw these lines.
                DrawBlocks(pMe, FALSE);
                pMe->m_rFlashTimes = 0;
                pMe->m_rRmLines = 0;
            }

            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
            pMe->m_AppState = APP_STATE_PAUSED;
            DrawButtons(pMe, FALSE, TRUE);
            IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
            return TRUE;
        }
        else if(pMe->m_AppState == APP_STATE_PAUSED)
        {
            pMe->m_AppState = APP_STATE_RUNNING;
            DrawButtons(pMe, FALSE, FALSE);
            IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
            MoveDown(pMe, FALSE);
            return TRUE;
        }
        break;

    case AVK_SELECT:            // Return to the main menu.
        if (pMe->m_AppState == APP_STATE_HERO || 
            pMe->m_AppState == APP_STATE_HELP  || 
            pMe->m_AppState == APP_STATE_REPORT ||
            pMe->m_AppState == APP_STATE_LASTLEVEL)
        {
            DisplayMainMenu(pMe);
            return TRUE;
        }
        else if(pMe->m_AppState == APP_STATE_NEXTLEVEL)
        {
            pMe->m_rLevel += 1; 
            pMe->m_AppState = APP_STATE_RUNNING;
            ResumeScreen(pMe);
            MoveDown(pMe, FALSE);
            return TRUE;
        }
        break;
        
    case AVK_CLR:
        switch(pMe->m_AppState)
        {
        case APP_STATE_MAIN:    // Close the current applet.
            IRINGERMGR_Stop(pMe->m_pPlayer);
            (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            return TRUE;
            
        case APP_STATE_LEVEL:   
        case APP_STATE_SOUND:
        case APP_STATE_HELP:
        case APP_STATE_REPORT:
        case APP_STATE_GRID:  
        case APP_STATE_HERO:
        case APP_STATE_NEXTLEVEL:
            DisplayMainMenu(pMe);       
            return TRUE;        // Return to the main menu.    
        
        case APP_STATE_RUNNING: // When playing, press "C" to return to the main menu.
        case APP_STATE_PAUSED:
            IRINGERMGR_Stop(pMe->m_pPlayer);
            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
            pMe->m_bContinue = TRUE;
            DisplayMainMenu(pMe);
            return TRUE;        
        }
    }
    return FALSE;       
}
/*===============================================================================
FUNCTION: Tetris_InitAppData

DESCRIPTION:
    This function initializes app specific data and allocates required memory 
    for app data. It also functions to create all necessary GUI objects.

PARAMETERS:
    pMe [in] - CTetrisApp Structure

DEPENDENCIES:
    Assumes pMe is not NULL

RETURN VALUE:
    TRUE: If the app has app data is allocated and initialized successfully
    FALSE: App data could not be allocated or initialized

SIDE EFFECTS:
    None
===============================================================================*/
static boolean Tetris_InitAppData(CTetrisApp * pMe)
{
    int i;
    int Ascent,Descent;
    AEEDeviceInfo * pDeviceInfo;
    
    Ascent  = 0;
    Descent = 0;

    if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL,
                                (void **)&pMe->m_pMainMenu) != SUCCESS)
    {
        DBGPRINTF("The creation of main menu has failed!" );
        return FALSE;
    }                                   // Object used as main menu.

    if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL,
                                (void **)&pMe->m_pMenu) != SUCCESS)
    {
        DBGPRINTF("The creation of sub menu has failed!" );
        return FALSE;
    }                                   // Menu object used in other menus.

    if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL,
                                (void **)&pMe->m_pSoftKeyMenu) != SUCCESS)
    {
        DBGPRINTF("The creation of soft key menu has failed!" );
        return FALSE;
    }                                   // Softkey menu object.                         

    if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_GRAPHICS,
                                (void **)&pMe->m_pGraphics) != SUCCESS)
    {
        DBGPRINTF("The creation of graphics interface has failed!" );
        return FALSE;
    }                                   // Graphics interface object.                       

    if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC,
                                (void **)&pMe->m_pStatic) != SUCCESS)
    {
        DBGPRINTF("The creation of static interface has failed!" );
        return FALSE;
    }                                   // Static interface object.

    if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_RINGERMGR,
                                (void **)&pMe->m_pPlayer) != SUCCESS)
    {
        DBGPRINTF("The creation of sound player has failed!" );
        return FALSE;
    }                                   // SoundPlayer interface object.

    pDeviceInfo = NULL;
    pDeviceInfo = (AEEDeviceInfo *)MALLOC(sizeof(AEEDeviceInfo));
    if(pDeviceInfo == NULL)
    {
        return FALSE;
    }
	if (ISHELL_CreateInstance(pMe->a.m_pIShell,
									 AEECLSID_ANNUNCIATOR,
									 (void **) &pMe->m_pIAnn))
	{
		return FALSE;
	}
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell, pDeviceInfo);

    pMe->m_rScreenWidth  = pDeviceInfo->cxScreen;
    pMe->m_rScreenHeight = pDeviceInfo->cyScreen;   // The width and height of the device.

    (void)IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, &Ascent, &Descent);
    pMe->m_rLineHeight = Ascent + Descent;          // Get the height of the font.
    
    SetGameScreen(pMe);   
    
    pMe->m_Pos = NULL;
    pMe->m_Pos = MALLOC(pMe->m_rVNum * sizeof(CPos *));
    if(pMe->m_Pos == NULL)
    {
        return FALSE;
    }
    
    for(i = 0; i < pMe->m_rVNum; i++)
    {
        pMe->m_Pos[i] = NULL;
        pMe->m_Pos[i] = MALLOC(pMe->m_rHNum * sizeof(CPos));
        if(pMe->m_Pos[i] == NULL)
        {
            return FALSE;
        }
    }                                               // To record the status of the game region
    
    if(ISHELL_GetPrefs(pMe->a.m_pIShell, 
                       AEECLSID_TETRIS, 
                       TETRIS_VERSION, 
                       &(pMe->m_CfgData), 
                       sizeof(CPrefData)) != SUCCESS)
    {
        pMe->m_CfgData.m_nLevel = 1;
        pMe->m_CfgData.m_bGrid  = FALSE;
        pMe->m_CfgData.m_bSound = FALSE;
        for(i = 0; i < MAX_LEVEL; i++)
        {
            pMe->m_CfgData.Score[i] = 0;            // Initialize the highest scores.
        }
    }

    pMe->m_rSpeed = 0;
    pMe->m_rTime  = INIT_TIME;
    pMe->m_pImage = NULL;
    pMe->m_rLevel = pMe->m_CfgData.m_nLevel;
    pMe->m_bSound = FALSE;
    pMe->m_bGrid  = pMe->m_CfgData.m_bGrid;
                             
    pMe->m_rSelMenuItem = IDS_TETRIS_START; 
  
    InitBlockData(pMe);

    FREEIF(pDeviceInfo);
    return TRUE;
}
/*===============================================================================
FUNCTION: SetGameScreen

DESCRIPTION:
    This function is used to initialize the data members about the layout of the
    game screen. It is implemented according to the width and height of the 
    actual device. The position to display the next block is also identified.

PARAMETERS:
    pMe [in] - CTetrisApp structure.

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void SetGameScreen(CTetrisApp * pMe)
{
    int HMargin,VMargin;                // Margin in horizontal and vertical directions.
    uint16 HNum, VNum;                  // Number of blocks contained in two directions.

    HMargin = VMargin = 0;
    HNum = VNum =0;
#if defined(FEATURE_DISP_128X160)
    HMargin = (pMe->m_rScreenWidth - (NEXT_MAX_XNUM) * NEXT_BLOCK_SIZE) % BLOCK_SIZE;
    HNum    = (pMe->m_rScreenWidth - (NEXT_MAX_XNUM ) * NEXT_BLOCK_SIZE) / BLOCK_SIZE;
#elif defined(FEATURE_VERSION_K212)
	HMargin = (pMe->m_rScreenWidth - (NEXT_MAX_XNUM + 2) * NEXT_BLOCK_SIZE) % BLOCK_SIZE;
    HNum    = (pMe->m_rScreenWidth - (NEXT_MAX_XNUM + 2) * NEXT_BLOCK_SIZE) / BLOCK_SIZE;
#else
    HMargin = (pMe->m_rScreenWidth - (NEXT_MAX_XNUM + 1) * NEXT_BLOCK_SIZE) % BLOCK_SIZE;
    HNum    = (pMe->m_rScreenWidth - (NEXT_MAX_XNUM + 1) * NEXT_BLOCK_SIZE) / BLOCK_SIZE;
#endif	
    if(HMargin < NEXT_BLOCK_SIZE / 2)
    {
        HMargin = BLOCK_SIZE;
        HNum = HNum - 1;                // Set margin and number of blocks in horizontal direction.
    }

    VMargin = pMe->m_rScreenHeight % BLOCK_SIZE;
    VNum    = pMe->m_rScreenHeight / BLOCK_SIZE;
    if(VMargin == 0)
    {
        VMargin = BLOCK_SIZE;
        VNum  = VNum - 1;               // Set margin and number of blocks in vertical way.
    }

    SETAEERECT(&pMe->m_MainRect, HMargin / 2, VMargin / 2, HNum * BLOCK_SIZE, VNum * BLOCK_SIZE);

    pMe->m_rHNum = HNum;
    pMe->m_rVNum = VNum;
#if defined(FEATURE_DISP_128X160)
    pMe->m_rNextStartX = pMe->m_MainRect.x + pMe->m_MainRect.dx + 1 * NEXT_BLOCK_SIZE+6;
#else
	pMe->m_rNextStartX = pMe->m_MainRect.x + pMe->m_MainRect.dx + 2 * NEXT_BLOCK_SIZE;
#endif	

#ifdef FEATURE_LCD_TOUCH_ENABLE    
    pMe->m_rNextStartY = pMe->m_MainRect.y + pMe->m_MainRect.dy / 2 - BUTTON_DY
                        - 3 * pMe->m_rLineHeight - NEXT_MAX_YNUM * NEXT_BLOCK_SIZE / 2;
                                        // The position where next block displays.
#else
    pMe->m_rNextStartY = pMe->m_MainRect.y + pMe->m_MainRect.dy / 2 - 3 * pMe->m_rLineHeight - NEXT_MAX_YNUM * NEXT_BLOCK_SIZE / 2;
                                        // The position where next block displays.
#endif

    if(pMe->m_rHNum % 2 == 0)
    {
        pMe->m_rHMid = pMe->m_MainRect.x + pMe->m_MainRect.dx / 2;
    }
    else
    {
        pMe->m_rHMid = pMe->m_MainRect.x + (pMe->m_MainRect.dx - BLOCK_SIZE) / 2;
    }
    pMe->m_rVStart = pMe->m_MainRect.y;             // Starting position of the block.
}
/*===============================================================================
FUNCTION: SetGameData

DESCRIPTION:
    Initializes data members used before game starts.

PARAMETERS:
    pMe [in] - CTetrisApp structure.

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void SetGameData(CTetrisApp * pMe)
{
    int i,j;

    if(pMe->m_rLevel == 0 || pMe->m_rLevel > MAX_LEVEL)
    {
        pMe->m_rLevel = 1;              
    }
    
    pMe->m_bContinue = FALSE; 
    pMe->m_rFlashTimes = 0;             
    pMe->m_rRmLines = 0;
    pMe->m_rScore = 0; 

    pMe->m_rCurMinYPos = pMe->m_rVNum - 1;
    pMe->m_rTime  = pMe->m_WaitingTime[pMe->m_rLevel - 1];
    pMe->m_rSpeed = (INIT_TIME - pMe->m_WaitingTime[pMe->m_rLevel - 1]) / TIME_SLICE;
 
    for(i = 0; i < BLOCK_NUM; i++)
    {
        pMe->m_Blocks[i].XPos = 0;
        pMe->m_Blocks[i].YPos = 0;
    }                               

    pMe->m_Rect.x = pMe->m_MainRect.x; 
    pMe->m_Rect.y = pMe->m_MainRect.y + pMe->m_MainRect.dy;
    pMe->m_Rect.dx = pMe->m_Rect.dy = BLOCK_SIZE - 1;
    
    for(i = 0; i < MAX_REM_LINES; i++)
    {
        pMe->m_LinesToRm[i]  = 0;
    }

    for(i = 0; i < pMe->m_rVNum; i++)   
    {
        for(j = 0; j < pMe->m_rHNum; j++)
        {
            pMe->m_Pos[i][j].IsFull = FALSE;
            pMe->m_Pos[i][j].ClrIndex = MAX_BLOCK_KIND;
        }
    }
}
/*===============================================================================
FUNCTION: InitBlockData

DESCRIPTION:
    Initializes data members concerning block used throughout this application.

PARAMETERS:
    pMe [in] - CTetrisApp structure.

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void InitBlockData(CTetrisApp * pMe)
{
    CTetris Blocks[MAX_BLOCK_KIND] = 
    {
        {-1, 0, 1, 2, 0, 0, 0, 0},          // * * * *

        { 0, 1, 1, 2, 0, 0, 1, 1},          // * *
                                            //   * *

        { 0, 1,-1, 0, 0, 0, 1, 1},          //   * *
                                            // * *

        { 0,-1, 0, 1, 0, 1, 1, 1},          //   *
                                            // * * *

        { 0, 0, 1, 2, 0, 1, 1, 1},          // *
                                            // * * *

        { 1,-1, 0, 1, 0, 1, 1, 1},          //     *
                                            // * * *

        { 0, 1, 0, 1, 0, 0, 1, 1},          // * *
                                            // * *
    };                                  

    MEMCPY(pMe->m_BlockDataLib, Blocks, sizeof(CTetris) * MAX_BLOCK_KIND);

    pMe->m_BlockClr = (RGBVAL *) MALLOC (sizeof(RGBVAL) * (MAX_BLOCK_KIND + 1));
    pMe->m_BlockClr[0] = MAKE_RGB(255,   0,   0);
    pMe->m_BlockClr[1] = MAKE_RGB(  0,   0, 255);
    pMe->m_BlockClr[2] = MAKE_RGB(255,   0, 128);
    pMe->m_BlockClr[3] = MAKE_RGB(  0, 128,  64);
    pMe->m_BlockClr[4] = MAKE_RGB(128, 128,   0);
    pMe->m_BlockClr[5] = MAKE_RGB(128,   0,   0);
    pMe->m_BlockClr[6] = MAKE_RGB(255, 128,   0);
    pMe->m_BlockClr[7] = MAKE_RGB(  0,   0,   0);       
                                        // Colors for different shapes of blocks.

    pMe->m_WaitingTime[0] = INIT_TIME;
    pMe->m_WaitingTime[1] = INIT_TIME / MAX_LEVEL * 2;
    pMe->m_WaitingTime[2] = INIT_TIME / MAX_LEVEL;
                                        // Waiting time for different levels.
}
/*===============================================================================
FUNCTION: Tetris_FreeAppData

DESCRIPTION;
    This function frees data contained in app data and memory for app data itself.

PARAMETERS:
    pMe [in] - CTetrisApp structure.

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void Tetris_FreeAppData(CTetrisApp * pMe)
{
    int i;

    for(i = 0; i < pMe->m_rVNum; i++)
    {
        FREEIF(pMe->m_Pos[i]);
    }
    FREEIF(pMe->m_Pos);                 // Free the array used for recording status.

    if(pMe->m_BlockClr != NULL)
    {
        FREEIF(pMe->m_BlockClr);        // Free the array used to record colors.
    }

    if(pMe->m_pImage != NULL)
    {
        IIMAGE_Release(pMe->m_pImage);
        pMe->m_pImage = NULL;           // Free image resource.
    }

    if(pMe->m_pMainMenu != NULL)
    {
        IMENUCTL_Release(pMe->m_pMainMenu);
        pMe->m_pMainMenu = NULL;        // Free menu object used as main menu.
    }

    if(pMe->m_pMenu != NULL)
    {
        IMENUCTL_Release(pMe->m_pMenu);
        pMe->m_pMenu = NULL;            
    }

    if(pMe->m_pSoftKeyMenu != NULL)
    {
        IMENUCTL_Release(pMe->m_pSoftKeyMenu);
        pMe->m_pSoftKeyMenu = NULL;
    }
    
    if(pMe->m_pGraphics != NULL)
    {
        IGRAPHICS_Release(pMe->m_pGraphics);
        pMe->m_pGraphics = NULL; 
    }

    if(pMe->m_pStatic != NULL)
    {
        ISTATIC_Release(pMe->m_pStatic);
        pMe->m_pStatic = NULL; 
    }

    if(pMe->m_pPlayer != NULL)
    {
        IRINGERMGR_Release(pMe->m_pPlayer);
        pMe->m_pPlayer = NULL; 
    }
	if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
    //IAPPLET_Release((IApplet *)pMe);
    pMe = NULL;
}
/*===============================================================================
FUNCTION: DisplaySplashScreen 

DESCRIPTION:
    This function displays the splash screen.  A timer is set to call function
    DisplayMainMenu( ) after the timer fires to start the application.
    If the splash image cannot be loaded, then just display the main menu.

PARAMETERS:
    pMe [in] - Pointer to the CTetrisApp structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    Causes the phone display to be updated.
    Set application state to APP_STATE_SPLASH.
===============================================================================*/
static void DisplaySplashScreen(CTetrisApp * pMe)
{
    IImage  * pImage;
    AEERect   rect;
    AEEImageInfo rImageInfo;

    pImage = NULL;
    SETAEERECT(&rect, 0, 0, pMe->m_rScreenWidth, pMe->m_rScreenHeight);
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, TETRIS_RES_FILE_LANG, IDB_TETRIS_SPLASH);
    
    if(pImage)                          // Load the image, then display the main menu.
    {
        IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
        IDISPLAY_FillRect(pMe->a.m_pIDisplay, &rect, CLR_OVER);
        IIMAGE_GetInfo(pImage, &rImageInfo);

        DBGPRINTF("rImageInfo.cx = %d, rIamgeInfo.cy = %d", rImageInfo.cx,rImageInfo.cy);   
        IIMAGE_Draw(pImage, (pMe->m_rScreenWidth - rImageInfo.cx) / 2,
                    (pMe->m_rScreenHeight - rImageInfo.cy) / 2);
        IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
        IIMAGE_Release(pImage);      

        (void)ISHELL_SetTimer(pMe->a.m_pIShell, INIT_TIME, (PFNNOTIFY)DisplayMainMenu, pMe);
    }
    else
    {
        DisplayMainMenu(pMe);
    }

    pMe->m_AppState = APP_STATE_SPLASH;
}
/*===========================================================================
FUNCTION: DisplayMainMenu

DESCRIPTION:
    This function displays the application Main Menu.

PARAMETERS:
    pMe [in] - Pointer to the CTetirsApp structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    Causes the phone display to be updated.
    Set application state to APP_STATE_MAIN.
===========================================================================*/
static void DisplayMainMenu(CTetrisApp * pMe)
{
    AEERect rect;

    Reset(pMe, TRUE);

    rect.x = rect.y = 0;
    rect.dx = pMe->m_rScreenWidth;
    rect.dy = pMe->m_rScreenHeight - TETRIS_SOFTKEY_DY; 
    
    (void)IMENUCTL_SetTitle(pMe->m_pMainMenu, 
                            TETRIS_RES_FILE_LANG, 
                            IDS_TETRIS_TITLE, 
                            NULL);
 
    AddMenuItem(pMe->m_pMainMenu, IDS_TETRIS_START,IDB_TETRIS_START);
    if(pMe->m_bContinue == TRUE)
    {
        AddMenuItem(pMe->m_pMainMenu, IDS_TETRIS_CONTINUE, IDB_TETRIS_CONTINUE);
    }
    AddMenuItem(pMe->m_pMainMenu, IDS_TETRIS_LEVEL, IDB_TETRIS_LEVEL);
	#if 0
    AddMenuItem(pMe->m_pMainMenu, IDS_TETRIS_SOUND, IDB_TETRIS_SOUND);
	#endif
    AddMenuItem(pMe->m_pMainMenu, IDS_TETRIS_GRID,  IDB_TETRIS_GRID);
    AddMenuItem(pMe->m_pMainMenu, IDS_TETRIS_SCORE, IDB_TETRIS_SCORE);
    AddMenuItem(pMe->m_pMainMenu, IDS_TETRIS_HELP,  IDB_TETRIS_HELP);
    
#if defined AEE_STATIC
    SetUniMenuConfig(pMe->m_pMainMenu);
#else
    SetMenuAttr(pMe->m_pMainMenu, AEECLSID_MENUCTL, FALSE);
#endif

    IMENUCTL_SetRect(pMe->m_pMainMenu, &rect);
    IMENUCTL_SetProperties(pMe->m_pMainMenu, MP_UNDERLINE_TITLE);
    IMENUCTL_SetActive(pMe->m_pMainMenu, TRUE);
    IMENUCTL_SetSel(pMe->m_pMainMenu, pMe->m_rSelMenuItem);
    IMENUCTL_Redraw(pMe->m_pMainMenu);

#if defined AEE_STATIC
    DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
#else
    AddSoftKeyMenu(pMe, IDS_CHOOSE, IDS_RETURN);    // Draw softkey menu at the bottom.
    IDISPLAY_Update(pMe->a.m_pIDisplay);
#endif

    pMe->m_AppState = APP_STATE_MAIN;
}
/*===============================================================================
FUNCTION: DisplayLevelScreen

DESCRIPTION:
    This function displays the levels supported. It draws a pop window in the center
    of the screen. The other region will be kept.

PARAMETERS:
    pMe [in] - Pointer to the CTetrisApp structure.
    
RETURN VALUE:
    None.

SIDE EFFECTS:
    Cause the phone display to be updated.
    Set application state to APP_STATE_LEVEL.
===============================================================================*/
static void DisplayLevelScreen(CTetrisApp * pMe)
{
    uint16    i;
    uint16    ImageID;
    uint16    ItemID,SelItemID;
    AEERect   rect;                     // The area used to display pop menu.

    ImageID = NULL;
    SelItemID = IDS_TETRIS_LEVEL_1;

    Reset(pMe, FALSE);
    SETAEERECT(&rect, LARGE_POP_WIN_X, LARGE_POP_WIN_Y, LARGE_POP_WIN_DX, LARGE_POP_WIN_DY);
    DrawCtrlFrame(pMe, IDS_TETRIS_LEVEL, &rect);

    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rect);
    
    (void)IMENUCTL_DeleteAll(pMe->m_pMenu);
    for(i = 0; i < MAX_LEVEL; i++)
    {
        ItemID = i + IDS_TETRIS_LEVEL_1;
        if(pMe->m_rLevel == (i + 1) )
        {
            ImageID = IDB_GAME_LEVEL_SEL;
            SelItemID = ItemID;
        }
        else
        {
            ImageID = IDB_GAME_LEVEL_UNSEL;
        }
        
        AddMenuItem(pMe->m_pMenu, ItemID, ImageID);
    }                                   // Display levels supported.
    IMENUCTL_SetSel(pMe->m_pMenu,SelItemID);
    IMENUCTL_SetRect(pMe->m_pMenu, &rect);

#if defined AEE_STATIC
    SetUniMenuConfig(pMe->m_pMenu);
#else
    SetMenuAttr(pMe->m_pMenu,AEECLSID_MENUCTL, TRUE);
#endif
    
    IMENUCTL_SetActive(pMe->m_pMenu, TRUE);
    (void)IMENUCTL_Redraw(pMe->m_pMenu);
    IDISPLAY_Update(pMe->a.m_pIDisplay);

    pMe->m_AppState = APP_STATE_LEVEL;
}
/*===============================================================================
FUNCTION: DisplaySoundScreen

DESCRIPTION:
    This function displays the sound setting screen. It draws a pop window in the center
    of the screen. The other region will be kept.

PARAMETERS:
    pMe [in] - Pointer to CTetrisApp structure.
    
RETURN VALUE:
    Cause the phone display to be updated.
    Set application state to APP_STATE_SOUND.
===============================================================================*/
static void DisplaySoundScreen(CTetrisApp * pMe)
{
    uint16  SelItemID;                          // tcl whewei 041124 add
    AEERect rect;
    
    Reset(pMe, FALSE);
    SETAEERECT(&rect, SMALL_POP_WIN_X, SMALL_POP_WIN_Y, SMALL_POP_WIN_DX, SMALL_POP_WIN_DY);
    DrawCtrlFrame(pMe, IDS_TETRIS_SOUND, &rect);
    
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rect);
    (void)IMENUCTL_DeleteAll(pMe->m_pMenu);
    if(pMe->m_bSound == TRUE)
    {
        SelItemID = IDS_GRID_ON;                // tcl whewei 041124 add
        AddMenuItem(pMe->m_pMenu, IDS_GRID_ON,  IDB_GAME_LEVEL_SEL);
        AddMenuItem(pMe->m_pMenu, IDS_GRID_OFF, IDB_GAME_LEVEL_UNSEL);
    }
    else
    {
        SelItemID = IDS_GRID_OFF;               // tcl whewei 041124 add
        AddMenuItem(pMe->m_pMenu, IDS_GRID_ON, IDB_GAME_LEVEL_UNSEL);
        AddMenuItem(pMe->m_pMenu, IDS_GRID_OFF, IDB_GAME_LEVEL_SEL);
    }
    
#if defined AEE_STATIC
    SetUniMenuConfig(pMe->m_pMenu);
#else
    SetMenuAttr(pMe->m_pMenu,AEECLSID_MENUCTL, TRUE);
#endif

    IMENUCTL_SetSel(pMe->m_pMenu, SelItemID);   // tcl whewei 041124 add
    IMENUCTL_SetRect(pMe->m_pMenu, &rect);
    IMENUCTL_SetActive(pMe->m_pMenu, TRUE);
    (void)IMENUCTL_Redraw(pMe->m_pMenu);
    IDISPLAY_Update(pMe->a.m_pIDisplay);
    
    pMe->m_AppState = APP_STATE_SOUND;
}
/*===============================================================================
FUNCTION: DisplayGridScreen

DESCRIPTION:
    This function displays the grid setting screen. It draws a pop window in the center
    of the screen. The other region will be kept.

PARAMETERS:
    pMe [in] - Pointer to CTetrisApp structure.
    
RETURN VALUE:
    Cause the phone display to be updated.
    Set application state to APP_STATE_GRID.
===============================================================================*/
static void DisplayGridScreen(CTetrisApp * pMe)
{
    uint16  SelItemID;                          // tcl whewei 041124 add
    AEERect rect;
    
    Reset(pMe, FALSE);
    SETAEERECT(&rect, SMALL_POP_WIN_X, SMALL_POP_WIN_Y, SMALL_POP_WIN_DX, SMALL_POP_WIN_DY);
    DrawCtrlFrame(pMe, IDS_TETRIS_GRID, &rect);

    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rect);
    (void)IMENUCTL_DeleteAll(pMe->m_pMenu);
    if(pMe->m_bGrid == TRUE)
    {
        SelItemID = IDS_GRID_ON;                // tcl whewei 041124 add
        AddMenuItem(pMe->m_pMenu, IDS_GRID_ON,  IDB_GAME_LEVEL_SEL);
        AddMenuItem(pMe->m_pMenu, IDS_GRID_OFF, IDB_GAME_LEVEL_UNSEL);
    }
    else
    {
        SelItemID = IDS_GRID_OFF;
        AddMenuItem(pMe->m_pMenu, IDS_GRID_ON, IDB_GAME_LEVEL_UNSEL);
        AddMenuItem(pMe->m_pMenu, IDS_GRID_OFF, IDB_GAME_LEVEL_SEL);
    }
    
#if defined AEE_STATIC
    SetUniMenuConfig(pMe->m_pMenu);
#else
    SetMenuAttr(pMe->m_pMenu,AEECLSID_MENUCTL, TRUE);
#endif

    IMENUCTL_SetSel(pMe->m_pMenu, SelItemID);   // tcl whewei 041124 add

    IMENUCTL_SetRect(pMe->m_pMenu, &rect);
    IMENUCTL_SetActive(pMe->m_pMenu, TRUE);
    (void)IMENUCTL_Redraw(pMe->m_pMenu);        // tcl whewei 041124 add    
    IDISPLAY_Update(pMe->a.m_pIDisplay);        // tcl whewei 041124 add

    pMe->m_AppState = APP_STATE_GRID;
}
/*===============================================================================
FUNCTION: DisplayHerosScreen

DESCRIPTION:
    This function displays the highest score records corresponding to certain leves.  
    The records may be all resetted to zero.It draws a pop window in the center of 
    the screen. The other region will be kept.
    
PARAMETERS:
    pMe [in] - Pointer to CTetrisApp structure.
      
RETURN VALUE:
    Cause the phone display to be updated.
    Set application state to APP_STATE_HERO.
===============================================================================*/
static void DisplayHerosScreen(CTetrisApp * pMe)
{
    int  i;
    int  StartX,StartY;
    char szFormat[] = "  %d";
    AEERect rect;
    AECHAR Buff[30];
    AECHAR Score[20];
    AECHAR wstrFor[50];
    RGBVAL OldTextClr;

    Reset(pMe, FALSE);
    SETAEERECT(&rect, LARGE_POP_WIN_X, LARGE_POP_WIN_Y, LARGE_POP_WIN_DX, LARGE_POP_WIN_DY);
    DrawCtrlFrame(pMe, IDS_TETRIS_SCORE, &rect);
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rect);

    OldTextClr = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, MAKE_RGB(128, 0, 0));
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                               TETRIS_RES_FILE_LANG, 
                               IDS_HERO_TITLE, 
                               Buff, 
                               sizeof(Buff));
    StartX  = rect.x;
    StartY  = rect.y;
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, 
                            AEE_FONT_LARGE | AEE_FONT_BOLD, 
                            Buff,
                            -1,
                            StartX, 
                            StartY, 
                            &rect, 
                            IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);

    StartX += 5;
    (void)STRTOWSTR(szFormat, wstrFor, sizeof(wstrFor));
    (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, CLR_DEF_TEXT);
    for(i = 0; i < MAX_LEVEL; i++)
    {
        StartY += pMe->m_rLineHeight;
        MEMSET(Buff, 0 , sizeof(Buff));
        MEMSET(Buff, 0 , sizeof(Score));

        ISHELL_LoadResString(pMe->a.m_pIShell, 
                             TETRIS_RES_FILE_LANG, 
                             (int16)(i + IDS_TETRIS_LEVEL_1), 
                             Buff, 
                             sizeof(Buff));
        (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL,
                                Buff,
                                -1,
                                StartX,
                                StartY,
                                NULL,
                                IDF_TEXT_TRANSPARENT);

        WSPRINTF(Score, sizeof(Score), wstrFor, pMe->m_CfgData.Score[i]);
        (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                AEE_FONT_NORMAL,
                                Score,
                                -1,
                                StartX + 4 * pMe->m_rLineHeight+5,
                                StartY,
                                NULL,
                                IDF_TEXT_TRANSPARENT);
        
    }                                   // Display the highest score in every level.

    (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, OldTextClr);
                                        
    SetMenuAttr(pMe->m_pSoftKeyMenu, AEECLSID_SOFTKEYCTL, TRUE);
    (void)IMENUCTL_DeleteAll(pMe->m_pSoftKeyMenu);
    (void)IMENUCTL_AddItem(pMe->m_pSoftKeyMenu, 
                           TETRIS_RES_FILE_LANG, 
                           IDS_TETRIS_OK,
                           IDS_TETRIS_OK, 
                           NULL, 
                           NULL);
    (void)IMENUCTL_AddItem(pMe->m_pSoftKeyMenu, 
                           TETRIS_RES_FILE_LANG, 
                           IDS_CLEAR,
                           IDS_CLEAR, 
                           NULL, 
                           NULL);
    
    rect.y  = rect.y + rect.dy - TETRIS_SOFTKEY_DY;
    rect.dy = TETRIS_SOFTKEY_DY;

    IMENUCTL_SetRect(pMe->m_pSoftKeyMenu, &rect);
    IMENUCTL_SetActive(pMe->m_pSoftKeyMenu, TRUE);
    IMENUCTL_Redraw(pMe->m_pSoftKeyMenu);   // Draw softkey menu.

    pMe->m_AppState = APP_STATE_HERO;
}
/*===============================================================================
FUNCTION: DisplayHelpScreen

DESCRIPTION:
    This function displays the help information on the screen.

PARAMETERS:
    pMe [in] - Pointer to CTetrisApp structure.

RETURN VALUE:
    Cause the phone display to be updated.
    Set application state to APP_STATE_HELP.
===============================================================================*/
static void DisplayHelpScreen(CTetrisApp * pMe)
{    
    AEERect  Rect;                      
    AECHAR * pTemp;
    AECHAR   Title[32];                
    AECHAR   Content[1200];              

    Reset(pMe, TRUE);
    
    Rect.x = 0;
    Rect.y = 0;
    Rect.dx = pMe->m_rScreenWidth;
    Rect.dy = pMe->m_rScreenHeight - TETRIS_SOFTKEY_DY;
 
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                               TETRIS_RES_FILE_LANG, 
                               IDS_TETRIS_HELP, 
                               Title, 
                               sizeof(Title));
                                
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                               TETRIS_RES_FILE_LANG, 
                               IDS_THELP_CONTENT, 
                               Content, 
                               sizeof(Content));                            
                                
    pTemp = WSTRCHR(Content,(AECHAR)'\\');
    while (pTemp)
    {
        *pTemp = (AECHAR)'\n';
        pTemp  = WSTRCHR(Content, (AECHAR)'\\');
    }                                   // replace '\' in the string to '\n'.
    
    ISTATIC_SetRect(pMe->m_pStatic, &Rect);
    ISTATIC_SetProperties(pMe->m_pStatic, ST_CENTERTITLE | ST_UNDERLINE | ST_NOSCROLL);
    (void)ISTATIC_SetText(pMe->m_pStatic, 
                          Title, 
                          Content, 
                          AEE_FONT_BOLD | AEE_FONT_LARGE, 
                          AEE_FONT_NORMAL);
    ISTATIC_Redraw(pMe->m_pStatic);     // Display the help information.
    MSG_FATAL("pMe->m_pStatic.....TRUE",0,0,0);
    ISTATIC_SetActive(pMe->m_pStatic, TRUE);

#if defined AEE_STATIC
    DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
#else
    Rect.y = pMe->m_rScreenHeight - TETRIS_SOFTKEY_DY;
    Rect.dy = TETRIS_SOFTKEY_DY;
    
    SetMenuAttr(pMe->m_pSoftKeyMenu, AEECLSID_SOFTKEYCTL, TRUE);
    (void)IMENUCTL_DeleteAll(pMe->m_pSoftKeyMenu);
    (void)IMENUCTL_AddItem(pMe->m_pSoftKeyMenu, 
                           TETRIS_RES_FILE_LANG, 
                           IDS_TETRIS_OK,
                           IDS_TETRIS_OK, 
                           NULL, 
                           NULL);
                                        // Draw softkey menu.
    IMENUCTL_SetRect(pMe->m_pSoftKeyMenu, &Rect);
    IMENUCTL_SetActive(pMe->m_pSoftKeyMenu, TRUE);
    IMENUCTL_Redraw(pMe->m_pSoftKeyMenu);                                         
#endif

    pMe->m_AppState = APP_STATE_HELP;
}
/*===============================================================================
FUNCTION: EnterNextLevel

DESCRIPTION:
    This function draws a pop window in the center of the screen. The other region
    will be kept. It is called when the game enters a new level or passes the last
    level.

PARAMETERS:
    pMe [in] - Pointer to CTetrisApp structure.
    bIsLastLevel [in] - boolean. Is the current level last?

RETURN VALUE:
    Cause the phone display to be updated.
    Set application state to APP_STATE_NEXTLEVEL or APP_STATE_LASTLEVEL.
===============================================================================*/
static void EnterNextLevel(CTetrisApp * pMe, boolean bIsLastLevel)
{
    int     index, BaseSpeed;
    uint16  titleID, TotalScore;
    uint32  StaticProp;
    AEERect Rect;
    AECHAR  Str[200];
    AECHAR  Content[200];
    
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
    Reset(pMe, FALSE);
    
    index = pMe->m_rLevel - 1;
    titleID = IDS_TETRIS_LEVEL_1 + index;
    SETAEERECT(&Rect, LARGE_POP_WIN_X, LARGE_POP_WIN_Y, LARGE_POP_WIN_DX, LARGE_POP_WIN_DY + 20);
    DrawCtrlFrame(pMe, titleID, &Rect);

    BaseSpeed = (INIT_TIME - pMe->m_WaitingTime[index]) / TIME_SLICE;
    TotalScore = (pMe->m_rSpeed - BaseSpeed) * LEVEL_SCORE;
    (void)ISHELL_GetPrefs(pMe->a.m_pIShell, 
                          AEECLSID_TETRIS, 
                          TETRIS_VERSION, 
                          &(pMe->m_CfgData),
                          sizeof(CPrefData)); 
    pMe->m_CfgData.Score[index] = TotalScore;
    (void)ISHELL_SetPrefs(pMe->a.m_pIShell, 
                          AEECLSID_TETRIS, 
                          TETRIS_VERSION, 
                          &(pMe->m_CfgData),
                          sizeof(CPrefData)); 

    if(bIsLastLevel == TRUE)
    {
        TotalScore += pMe->m_rScore;
        (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                   TETRIS_RES_FILE_LANG, 
                                   IDS_PASS_LAST, 
                                   Str, 
                                   sizeof(Str));
    }                                   // Has passed the last level.
    else
    {
        (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                   TETRIS_RES_FILE_LANG, 
                                   IDS_PASS, 
                                   Str, 
                                   sizeof(Str));
    }                                   // Pass another level.                          
    
    WSPRINTF(Content, sizeof(Content), Str, TotalScore);

    Rect.dy -= TETRIS_SOFTKEY_DY;
    StaticProp = ST_CENTERTITLE | ST_UNDERLINE | ST_CENTERTEXT;
    ISTATIC_SetRect(pMe->m_pStatic, &Rect);
    ISTATIC_SetProperties(pMe->m_pStatic, StaticProp);
    (void)ISTATIC_SetText(pMe->m_pStatic, 
                          NULL, 
                          Content, 
                          NULL, 
                          AEE_FONT_NORMAL);
    
    ISTATIC_SetActive(pMe->m_pStatic, TRUE);
    (void)ISTATIC_Redraw(pMe->m_pStatic);   // Display information in the pop window.
  
    Rect.y = Rect.y + Rect.dy;
    Rect.dy = TETRIS_SOFTKEY_DY;
    
    SetMenuAttr(pMe->m_pSoftKeyMenu, AEECLSID_SOFTKEYCTL, TRUE);
    (void)IMENUCTL_DeleteAll(pMe->m_pSoftKeyMenu);
    if(bIsLastLevel == TRUE)
    {
        (void)IMENUCTL_AddItem(pMe->m_pSoftKeyMenu, 
                               TETRIS_RES_FILE_LANG, 
                               IDS_TETRIS_OK,
                               IDS_TETRIS_OK,
                               NULL, 
                               NULL);
    }                                   // Has passed the last level.
    else
    {
        (void)IMENUCTL_AddItem(pMe->m_pSoftKeyMenu, 
                               TETRIS_RES_FILE_LANG, 
                               IDS_YES,
                               IDS_YES,
                               NULL, 
                               NULL);
        (void)IMENUCTL_AddItem(pMe->m_pSoftKeyMenu, 
                               TETRIS_RES_FILE_LANG,
                               IDS_NO,
                               IDS_NO, 
                               NULL, 
                               NULL);
    }                                   // Pass another level.

    IMENUCTL_SetRect(pMe->m_pSoftKeyMenu, &Rect);
    IMENUCTL_SetActive(pMe->m_pSoftKeyMenu, TRUE);
    IMENUCTL_Redraw(pMe->m_pSoftKeyMenu);    // Draw softkey menu at the bottom of the screen.                                     
    
    if(bIsLastLevel == TRUE)
    {
        pMe->m_AppState = APP_STATE_LASTLEVEL;
    }
    else
    {
        pMe->m_AppState = APP_STATE_NEXTLEVEL;
    }
}
/*===============================================================================
FUNCTION: ClearHerosScore

DESCRIPTION:
    This function is used to reset the score record in hero scores. If any one record 
    is non-zero, the new records will display. Otherwise, it will directly call
    DisplayMainMenu( ) to display the main menu.

PARAMETERS:
    pMe [in] - Pointer to CTetrisApp structure.

RETURN VALUE:
    None.
===============================================================================*/
static void ClearHerosScore(CTetrisApp * pMe)
{
    int i;
    boolean bRedrawScore;
    
    bRedrawScore = FALSE;
    (void)ISHELL_GetPrefs(pMe->a.m_pIShell, 
                          AEECLSID_TETRIS, 
                          TETRIS_VERSION, 
                          &(pMe->m_CfgData),
                          sizeof(CPrefData)); 
    for(i = 0; i < MAX_LEVEL; i++)
    {
        if(pMe->m_CfgData.Score[i] != 0)
        {
            bRedrawScore = TRUE;
            pMe->m_CfgData.Score[i] = 0;
        }
    }
    
    if(bRedrawScore == TRUE)
    {
        (void)ISHELL_SetPrefs(pMe->a.m_pIShell, 
                              AEECLSID_TETRIS, 
                              TETRIS_VERSION,
                              &(pMe->m_CfgData), 
                              sizeof(CPrefData));
        
        DisplayHerosScreen(pMe);
        bRedrawScore = FALSE;
    }                                   // The scores changes, display the new records.
    else
    {
        DisplayMainMenu(pMe);
    }                                   // Return to the main menu.
}
/*===============================================================================
FUNCTION: Reset

DESCRIPTION:
    This is a generic function which is called each time a new screen is displayed. 
    The function resets key controls, sets their active status to FALSE, clears the 
    screen if the second parameter is true.

PARAMETERS:
    pMe    [in] - Pointer to CTetrisApp structure.
    bErase [in] - boolen, indicate whether to erase the screen.

RETURN VALUE:
    None.
===============================================================================*/
static void Reset(CTetrisApp * pMe, boolean bErase)
{
    if(IMENUCTL_IsActive(pMe->m_pMainMenu))
    {
        IMENUCTL_SetActive(pMe->m_pMainMenu, FALSE);
        IMENUCTL_Reset(pMe->m_pMainMenu);
    }   
    
    if(IMENUCTL_IsActive(pMe->m_pMenu))
    {
        IMENUCTL_SetActive(pMe->m_pMenu, FALSE);
        IMENUCTL_Reset(pMe->m_pMenu);
    }
    
    if(IMENUCTL_IsActive(pMe->m_pSoftKeyMenu))
    {
        IMENUCTL_SetActive(pMe->m_pSoftKeyMenu, FALSE);
        IMENUCTL_Reset(pMe->m_pSoftKeyMenu);
    }

    if(ISTATIC_IsActive(pMe->m_pStatic))
    {
    	MSG_FATAL("pMe->m_pStatic, FALSE",0,0,0);
        ISTATIC_SetActive(pMe->m_pStatic, FALSE);
        ISTATIC_Reset(pMe->m_pStatic);
    }

    if(bErase == TRUE)
    {
        IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    }
}
/*===============================================================================
FUNCTION: SetMenuAttr

DESCRIPTION:
    This is a generic function which is called each time a new menu is displayed. 
    The function sets the item style and color of the menu.

PARAMETERS:
    pMenu [in] - Pointer to IMenuCtl interface.
    clsID [in] - The type of this menu.

RETURN VALUE:
    None.
===============================================================================*/
static void SetMenuAttr(IMenuCtl * pMenu, uint32 clsID, boolean bPopMenu)
{
    AEEItemStyle    normal,sel;
    AEEMenuColors   col;

    normal.ft = AEE_FT_NONE;
    normal.xOffset = 0;
    normal.yOffset = 0;
    normal.roImage = AEE_RO_TRANSPARENT;            // Style of unselected items.

    sel.ft = AEE_FT_RAISED;
    sel.xOffset = 0;
    sel.yOffset = 0;
    sel.roImage = AEE_RO_TRANSPARENT;               // Style of selected items.

    IMENUCTL_SetStyle(pMenu, &normal, &sel);

    col.wMask = MENU_COLOR_MASK;
    col.cFrame = MENU_FRAME;
    
    if (!bPopMenu)
    {
        col.cBack = MENU_BACKGROUND;
        col.cSelBack = MENU_SEL_BACKGROUND;
        col.cText = MENU_TEXT;
        col.cSelText = MENU_SEL_TEXT;
        col.cScrollbar = MENU_SCROLLBAR;
        col.cScrollbarFill = MENU_SCROLLBAR_FILL;
    }
    else
    {
        col.cBack = POP_MENU_BACKGROUND;
        col.cSelBack = POP_MENU_SEL_BACKGROUND;
        col.cText = MENU_TEXT;
        col.cSelText = MENU_TEXT;
        col.cScrollbar = MENU_SCROLLBAR;
        col.cScrollbarFill = POP_MENU_SCROLLBAR_FILL;
    }
   
    if(clsID == AEECLSID_SOFTKEYCTL)
    {
        col.cBack = SOFTKEY_BACKGROUND;
        col.cSelBack = SOFTKEY_SEL_BACKGROUND;
        //col.cSelBack = MAKE_RGB(0x63, 0xb8, 0xff);
        //clr.cSelBack = MAKE_RGB(0x64, 0x95, 0xed);
    }
 
    IMENUCTL_SetColors(pMenu, &col);
    IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE);
}
/*===============================================================================
FUNCTION: AddMenuItem

DESCRIPTION:
    This is a generic function which is called each time to add a menu item. 

PARAMETERS:
    pMenu   [in] - Pointer to IMenuCtl interface.
    ItemID  [in] - The ID of this item.
    ImageID [in] - The image ID of this item.

RETURN VALUE:
    None.
===============================================================================*/
static void AddMenuItem(IMenuCtl * pMenu, uint16 ItemID, uint16 ImageID)
{
    CtlAddItem ai;
    
    ai.pText = NULL;
    ai.pImage = NULL;
    ai.pszResImage = TETRIS_RES_FILE_LANG;
    ai.pszResText  = TETRIS_RES_FILE_LANG;
    ai.wFont = AEE_FONT_NORMAL;
    ai.dwData = 0;
    
    ai.wText = ItemID;
    ai.wImage = ImageID;
    ai.wItemID = ItemID;
    
    (void)IMENUCTL_AddItemEx(pMenu, &ai);
}
/*===============================================================================
FUNCTION: IMENUCTL_UpAndDownScroll_HandleEvent

DESCRIPTION:
    This is a general purpose function. It is used to handle the scroll event of the 
    menu.

PARAMETERS:
    ps     [in] - Pointer to IShell interface.
    pMenu  [in] - Pointer to IMenuCtl object.
    eCode  [in] - AEEEvent, the code of the current event.
    wParam [in] - uint16, event specific data.
    dwParem[in] - uint32, event specific data.

RETURN VALUE:
    TRUE  - The scroll event has been handled.
    FALSE - The current event is not handled by this function.
===============================================================================*/
static boolean IMENUCTL_UpAndDownScroll_HandleEvent(IShell* ps, void* p, AEEEvent eCode, uint16 wParam)
{
    uint16 nCurSel,nFirst,nLast;
    int nCount;
    
    if(!p)  
    {
        return FALSE;
    }
    if((eCode != EVT_KEY)||((wParam != AVK_DOWN)&&(wParam != AVK_UP)))
    {
        return FALSE;
    }
    if(!GET_PVTBL(p,IMenuCtl)->IsActive(p))
    {
        return FALSE;
    }
    
    nCurSel = GET_PVTBL(p,IMenuCtl)->GetSel(p);
    nCount  = GET_PVTBL(p,IMenuCtl)->GetItemCount(p);
    nFirst  = GET_PVTBL(p,IMenuCtl)->GetItemID(p, 0);
    nLast   = GET_PVTBL(p,IMenuCtl)->GetItemID(p, nCount - 1);
    
    if((nCurSel == nFirst)&&((AVKType)wParam == AVK_UP))
    {
        nCurSel = nLast;
        GET_PVTBL(p,IMenuCtl)->SetSel(p,nCurSel);
        return TRUE;
    }
    else if((nCurSel == nLast)&&((AVKType)wParam == AVK_DOWN))
    {
        nCurSel = nFirst;
        GET_PVTBL(p,IMenuCtl)->SetSel(p,nCurSel);
        return TRUE;
    }
    return FALSE;
}
/*===============================================================================
FUNCTION: AddSoftKeyMenu

DESCRIPTION:
    This is a general purpose function. It draws a bar at the bottom of the screen,
    the bar is used as a softkey menu.

PARAMETERS:
    pMe       [in] - Pointer to CTetrisApp structure.
    LeftItem  [in] - The string ID of the left item.
    RightItem [in] - The string ID of the right item .

RETURN VALUE:
    None.
===============================================================================*/
static void AddSoftKeyMenu(CTetrisApp * pMe, uint16 LeftItem, uint16 RightItem)
{
    int          i, width;
    uint8        offset;
    IImage *     pImage;
    AEEImageInfo ImageInfo;
    AEERect      rect;
    AEELine      line;
    AECHAR       Str[10];

    i = 0;
    pImage = NULL;

    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, TETRIS_RES_FILE_LANG, IDB_SOFTKEY_BOTTOM);
    if(pImage)
    {
        IIMAGE_GetInfo(pImage, &ImageInfo);

        rect.x = 0;
        rect.y = pMe->m_rScreenHeight - ImageInfo.cy;
        rect.dx = pMe->m_rScreenWidth;
        rect.dy = ImageInfo.cy;
        IIMAGE_Draw(pImage, rect.x, rect.y);

        if(pMe->m_rScreenWidth > ImageInfo.cx)
        {
            IIMAGE_SetOffset(pImage,(pMe->m_rScreenWidth - ImageInfo.cx), 0);
            IIMAGE_Draw(pImage, ImageInfo.cx, rect.y);
        }
        IIMAGE_Release(pImage);
        pImage = NULL;
    }                           // Draw the background image at the bottom.
 
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                               TETRIS_RES_FILE_LANG,
                               LeftItem, 
                               Str, 
                               sizeof(Str));
    width = IDISPLAY_MeasureText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, Str);
    offset = (rect.dx / 3 - width) / 2;
    
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                            AEE_FONT_NORMAL, 
                            Str, 
                            -1, 
                            rect.x + offset, 
                            rect.y + 2,
                            NULL, 
                            IDF_TEXT_TRANSPARENT);
                                // Draw the content of left key.

    MEMSET(Str, 0, sizeof(Str));
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                               TETRIS_RES_FILE_LANG, 
                               RightItem, 
                               Str, 
                               sizeof(Str));
    width = IDISPLAY_MeasureText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, Str);
    offset = (rect.dx / 3 - width) / 2;
    
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                            AEE_FONT_NORMAL, 
                            Str, 
                            -1,
                            rect.dx * 2 / 3 + offset, 
                            rect.y + 2,
                            NULL, 
                            IDF_TEXT_TRANSPARENT);
                                // Draw the content of right key.
    
    (void)IGRAPHICS_SetColor(pMe->m_pGraphics, 0, 0, 0, 0);

    line.sx = line.ex = rect.dx / 3;
    line.sy = rect.y;
    line.ey = rect.y + rect.dy;
    IGRAPHICS_DrawLine(pMe->m_pGraphics, &line);
    line.sx = line.ex = rect.dx * 2 / 3;
    IGRAPHICS_DrawLine(pMe->m_pGraphics, &line);
    
    IGRAPHICS_Update(pMe->m_pGraphics);
 }
/*===============================================================================
FUNCTION:   DrawCtlFrame

DESCRIPTION:
    This is a general purpose function. It draws a frame in the center of the screen. 

PARAMETER:
    pMe     [in] - CTetrisApp structure
    titleID [in] - uint16,  the title of the menu.
    fRect   [in] - AEERect, the region of the frame;

RETURN VALUE:
    None.

SIDE EFFECTS:
    None
===============================================================================*/
static void DrawCtrlFrame(CTetrisApp * pMe, uint16 titleID, AEERect * fRect)
{
    AEERect frameRect;
    AEERect tempRect;
    AECHAR  szTile[50];
    RGBVAL  oldTextColor;
    
    if(!IGRAPHICS_GetFillMode(pMe->m_pGraphics))
    {
        (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics,TRUE);
    }
    IGRAPHICS_SetFillColor(pMe->m_pGraphics, 205, 205, 205, 0);
    IGRAPHICS_SetColor(pMe->m_pGraphics, 0, 0, 0, 0);

    frameRect.x  = fRect->x  - 5;
    frameRect.y  = fRect->y  - 32;
    frameRect.dx = fRect->dx + 10;
    frameRect.dy = fRect->dy + 38;
    (void)MEMCPY(&tempRect,&frameRect,sizeof(AEERect));
    
    (void)IGRAPHICS_DrawRoundRectangle(pMe->m_pGraphics, 
                                       &frameRect, 
                                       32, 
                                       32);

    oldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, 
                                     CLR_USER_TEXT, 
                                     CLR_DEF_TEXT);
    
    ISHELL_LoadResString(pMe->a.m_pIShell,
                         TETRIS_RES_FILE_LANG,
                         titleID,
                         szTile,
                         sizeof(szTile));
    
    tempRect.y += 2;
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                            AEE_FONT_NORMAL,
                            szTile,
                            -1,
                            0,
                            0,
                            &tempRect,
                            IDF_ALIGN_CENTER | IDF_ALIGN_TOP | IDF_TEXT_TRANSPARENT);
    
    (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, oldTextColor);
    IGRAPHICS_Update(pMe->m_pGraphics);
}
/*===============================================================================
FUNCTION:   StartGame

DESCRIPTION:
    This function prepares for the start of this game. And creates first group of 
    blocks, then it calls function MoveDown( ) to continue the game. When blocks
    can't move down, the program will go to function MainFlow( ).

PARAMETER:
    pMe[in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None
===============================================================================*/
static void StartGame(CTetrisApp * pMe)
{
    Reset(pMe, TRUE);                   // Stop controls used in applet.
    SetGameData(pMe);                   // Initialize the data used.
    DrawBorder(pMe);                    // Display the frame of the game screen.
    UpdateScore(pMe);                   // Display score and speed.
    pMe->m_AppState = APP_STATE_RUNNING;

    pMe->m_rRand = CreateRandNum(pMe);  // Get a random number.
    pMe->m_rKind = pMe->m_rRand % MAX_BLOCK_KIND;

    SetBlockPoint(pMe, pMe->m_rHMid, pMe->m_rVStart);
    DrawBlocks(pMe, FALSE);             // Display the first block.

    pMe->m_rRand = CreateRandNum(pMe);
    DrawNextBlocks(pMe);                // Draw the next block.

    MoveDown(pMe, FALSE);               // Move down the current block.
}
/*===============================================================================
FUNCTION:   MainFlow

DESCRIPTION:
    This function is the frame of the application. It calls other functions to 
    implement the main flow of this game.

PARAMETER:
    pMe[in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None
===============================================================================*/
static void MainFlow(CTetrisApp * pMe)
{
    uint8  i;
    uint16 YPos;

    Record(pMe);                        // Record status.
    RemoveLines(pMe);                   
    if(ChangeScore(pMe) == FALSE)       // If exist full lines, change score.
    {
        return;
    }
  
    pMe->m_rKind = pMe->m_rRand % MAX_BLOCK_KIND;
    DBGPRINTF("The random number is %d", pMe->m_rKind);
    
    SetBlockPoint(pMe, pMe->m_rHMid, pMe->m_rVStart);

    for(i = 0; i < BLOCK_NUM; i++)
    {
        if(HasBlock(pMe, pMe->m_Blocks[i].XPos, pMe->m_Blocks[i].YPos))
        {
            YPos = GetFinalYPos(pMe);
            CutBlocks(pMe, YPos);       // Handle the last block when the game is over.
            PlayMusic(pMe, MUSIC_GAMEOVER);
            (void)ISHELL_SetTimer(pMe->a.m_pIShell, OVER_TIME, (PFNNOTIFY)GameOver, pMe);
            return;
        }
    }

    DrawBlocks(pMe, FALSE);
    pMe->m_rRand = CreateRandNum(pMe);
    DrawNextBlocks(pMe);

    ISHELL_SetTimer(pMe->a.m_pIShell, pMe->m_rTime, (PFNNOTIFY)MoveDown, pMe);
}
/*===============================================================================
FUNCTION: DrawBorder

DESCRIPTION:
    This function draws the borders used in this application, which include main
    game region, score region, next block region.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void DrawBorder(CTetrisApp * pMe)
{
    AEERect rect;
    
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    rect.x = rect.y = 0;
    rect.dx = pMe->m_rScreenWidth;
    rect.dy = pMe->m_rScreenHeight;
    
    IDISPLAY_FillRect(pMe->a.m_pIDisplay, &rect, CLR_MBG);
    
    rect.x =  pMe->m_MainRect.x - 1;
    rect.y =  pMe->m_MainRect.y - 1;
    rect.dx = pMe->m_MainRect.dx + 1;
    rect.dy = pMe->m_MainRect.dy + 1;
    
    IDISPLAY_DrawRect(pMe->a.m_pIDisplay, 
                      &rect, 
                      CLR_FRAME,
                      0, 
                      IDF_RECT_FRAME);  // Draw the border of the game region.

    rect.x -=  1;
    rect.y -=  1;
    rect.dx += 2;
    rect.dy += 2;

    IDISPLAY_DrawRect(pMe->a.m_pIDisplay, 
                      &rect, 
                      MAKE_RGB(0xf0, 0x80, 0x80),
                      0, 
                      IDF_RECT_FRAME);  // Draw the border of the game region.

    if(pMe->m_bGrid == TRUE)
    {
        DrawGrid(pMe);                 
    }
    DrawButtons(pMe, TRUE, FALSE);
}
/*===============================================================================
FUNCTION: DrawGrid

DESCRIPTION:
    This function draws gridding in the main region.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void DrawGrid(CTetrisApp * pMe)
{
    int     i;
    AEELine line;
    
    IGRAPHICS_SetColor(pMe->m_pGraphics, 0x00, 0x00, 0x8b, 0);
    line.sx = line.ex = pMe->m_MainRect.x - 1;
    line.sy = pMe->m_MainRect.y - 1;
    line.ey = pMe->m_MainRect.y + pMe->m_MainRect.dy - 1;
    for(i = 1; i < pMe->m_rHNum; i++)
    {
        line.sx += BLOCK_SIZE;
        line.ex += BLOCK_SIZE;
        IGRAPHICS_DrawLine(pMe->m_pGraphics, &line);
    }
    
   // line.sy = line.ey = pMe->m_MainRect.y - 1;
   //    line.sx = pMe->m_MainRect.x - 1;
   //    line.ex = pMe->m_MainRect.x + pMe->m_MainRect.dx - 1;
   //    for(i = 1; i < pMe->m_rVNum; i++)
   //    {
   //        line.sy += BLOCK_SIZE;
   //        line.ey += BLOCK_SIZE;
   //        IGRAPHICS_DrawLine(pMe->m_pGraphics, &line);
   //    }
   
}
/*===============================================================================
FUNCTION: DrawButtons

DESCRIPTION:
    This function draws two buttons in the right side of screen. These two buttons
    is used to support Pause/Continue and Exit operation. 

PARAMETERS:
    pMe      [in] - CTetrisApp structure
    bDrawTwo [in] - boolean, draw two buttons or only one.
    bPaused  [in] - boolean, is the game paused or not.

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void DrawButtons(CTetrisApp * pMe, boolean bDrawTwo, boolean bPaused)
{
    uint16   ResID;
    AECHAR   Content[10];
    AEERect  rect;

#ifdef FEATURE_LCD_TOUCH_ENABLE
    rect.x = pMe->m_MainRect.x + pMe->m_MainRect.dx + 2;
    rect.y = pMe->m_rNextStartY + NEXT_MAX_YNUM * NEXT_BLOCK_SIZE + 6 * pMe->m_rLineHeight;
    rect.dx = pMe->m_rScreenWidth - rect.x;
    rect.dy = BUTTON_DY;

    //IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rect);
    pMe->m_pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, TETRIS_RES_FILE_LANG, IDB_RECTBG);
    IIMAGE_Draw(pMe->m_pImage, rect.x, rect.y-2);

    if(bPaused == TRUE)
    {
        ResID = IDS_GAME_CONTINUE;
    }
    else
    {
        ResID = IDS_GAME_PAUSE;
    }

    MEMSET(Content, 0, sizeof(Content));
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                               TETRIS_RES_FILE_LANG,
                               ResID,
                               Content,
                               sizeof(Content));
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                            AEE_FONT_NORMAL,
                            Content,
                            -1,
                            0,
                            0,
                            &rect,
                            IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);

    if(bDrawTwo == FALSE)
    {
        return;                         // No need to draw the second button.
    }

    rect.y += rect.dy + 10;
    IIMAGE_Draw(pMe->m_pImage, rect.x, rect.y-2);
    
    MEMSET(Content, 0, sizeof(Content));
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                               TETRIS_RES_FILE_LANG,
                               IDS_GAME_EXIT,
                               Content,
                               sizeof(Content));
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                            AEE_FONT_NORMAL,
                            Content,
                            -1,
                            0,
                            0,
                            &rect,
                            IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);
#endif    
}
/*===============================================================================
FUNCTION: UpdateScore

DESCRIPTION:
    This function displays the score at the given position when it increases or 
    the game starts.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void UpdateScore(CTetrisApp * pMe)
{
    uint16  XPos;
    uint16  YPos;
    char    szNum[10];
    AECHAR  Score[10];
    AECHAR  Speed[10];
    AEERect rect;
    RGBVAL  OldTextColor;
  
    XPos = pMe->m_rNextStartX - NEXT_BLOCK_SIZE - 4;
    YPos = pMe->m_rNextStartY + BLOCK_NUM * NEXT_BLOCK_SIZE;

    OldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, CLR_TEXT);

    (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                               TETRIS_RES_FILE_LANG, 
                               IDS_GAME_SCORE, 
                               Score, 
                               sizeof(Score));
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, 
                            AEE_FONT_BOLD, 
                            Score,
                            -1, 
                            XPos, 
                            YPos, 
                            0, 
                            IDF_TEXT_TRANSPARENT);       // Draw text "Score:".             
    
    SPRINTF(szNum, "%d", pMe->m_rScore);
    MEMSET(Score, 0, sizeof(Score));
    (void)STRTOWSTR(szNum, Score, sizeof(Score));        // Get current score.

    YPos += pMe->m_rLineHeight;
    rect.x = XPos;
    rect.y = YPos;
    rect.dx = pMe->m_rScreenWidth - XPos;
    rect.dy = pMe->m_rLineHeight;

    (void)IDISPLAY_FillRect(pMe->a.m_pIDisplay, &rect, CLR_MBG);
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, 
                            AEE_FONT_BOLD, 
                            Score,
                            -1, 
                            XPos,
                            YPos, 
                            0, 
                            IDF_TEXT_TRANSPARENT);       // Display the current score.
    
    YPos += pMe->m_rLineHeight + 5; 
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                               TETRIS_RES_FILE_LANG, 
                               IDS_GAME_SPEED, 
                               Speed, 
                               sizeof(Speed));
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, 
                            AEE_FONT_BOLD, 
                            Speed,
                            -1,
                            XPos, 
                            YPos, 
                            0, 
                            IDF_TEXT_TRANSPARENT);       // Draw text "Speed:".
    
    SPRINTF(szNum, "%d", pMe->m_rSpeed);
    MEMSET(Speed, 0, sizeof(Speed));
    (void)STRTOWSTR(szNum, Speed, sizeof(Speed));        // Get current speed.

    YPos += pMe->m_rLineHeight; 
    rect.y = YPos;
    rect.dy = pMe->m_rLineHeight;
    
    (void)IDISPLAY_FillRect(pMe->a.m_pIDisplay, &rect, CLR_MBG);
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, 
                            AEE_FONT_BOLD, 
                            Speed, 
                            -1, 
                            XPos, 
                            YPos, 
                            0, 
                            IDF_TEXT_TRANSPARENT);       // Display the current speed.

    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, OldTextColor);
}
/*===============================================================================
FUNCTION: ResumeScreen

DESCRIPTION:
    Resume the game region after case EVT_APP_RESUME is invoked or choosing to
    continue the game.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void ResumeScreen(CTetrisApp * pMe)
{
    int  i,j;
    char ClrIndex;
    AEERect rect;

    Reset(pMe, TRUE);
    DrawBorder(pMe);
    UpdateScore(pMe);

    if(pMe->m_rFlashTimes != 0)         
    {
        pMe->m_rFlashTimes = 0;
    }

    pMe->m_rRmLines = 0;
    rect.dx = rect.dy = BLOCK_SIZE - 1;

    for(i = pMe->m_rVNum - 1; i >= pMe->m_rCurMinYPos; i--)
    {
        for(j = 0; j < pMe->m_rHNum; j++)
        {
            if(pMe->m_Pos[i][j].IsFull == TRUE)
            {
                rect.x = j * BLOCK_SIZE + pMe->m_MainRect.x;
                rect.y = i * BLOCK_SIZE + pMe->m_MainRect.y;
                ClrIndex = pMe->m_Pos[i][j].ClrIndex;
                IDISPLAY_DrawRect(pMe->a.m_pIDisplay, &rect, 0,
                                  pMe->m_BlockClr[ClrIndex], IDF_RECT_FILL);
            }
        }
    }

    DrawBlocks(pMe, FALSE);             // Redraw the block not handled before game pauses.
    DrawNextBlocks(pMe);
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
}
/*===============================================================================
FUNCTION: CreateRandNum

DESCRIPTION:
    Create a random number to get the relevant shape of blocks. In order to 
    get better random effect, this function gets an average from 50 integers.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    The random number.

SIDE EFFECTS:
    None.
===============================================================================*/
static uint8 CreateRandNum(CTetrisApp * pMe)
{
    uint8   Array[1];
    uint8   Rand;
    uint32  Sum;

    Sum     = 0;
    Rand    = 0;
     
    GETRAND(Array, 1);
    Sum = GETTIMESECONDS( ) + Array[0];
    Rand = Sum % MAX_BLOCK_KIND;

    return Rand;                                
}
/*===============================================================================
FUNCTION: SetBlockPoint

DESCRIPTION:
    This function sets the initial position of blocks, and the central block.

PARAMETERS:
    pMe    [in] - CTetrisApp structure
    HMid   [in] - Integer. Horizontal benchmark position
    VStart [in] - Integer. Vertical   benchmark position
RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void SetBlockPoint(CTetrisApp * pMe, uint16 HMid, uint16 VStart)
{
    int i;

    DBGPRINTF("HMid = %d, VStart =%d",HMid,VStart);
    DBGPRINTF("pMe->m_rKind = %d",pMe->m_rKind);

    for(i=0; i<BLOCK_NUM; i++)
    {
        pMe->m_Blocks[i].XPos = HMid + pMe->m_BlockDataLib[pMe->m_rKind].XPos[i] * BLOCK_SIZE;
        pMe->m_Blocks[i].YPos = VStart + pMe->m_BlockDataLib[pMe->m_rKind].YPos[i] * BLOCK_SIZE;
        DBGPRINTF("XPos = %d, YPos = %d", 
                  pMe->m_BlockDataLib[pMe->m_rKind].XPos[i],
                  pMe->m_BlockDataLib[pMe->m_rKind].YPos[i]);
    }

    switch(pMe->m_rKind)                // Set the main block, used when rotating.
    {
    case 0:                             // * * * *
    
    case 1:                             // * *
                                        //   * *

    case 4:                             // *
                                        // * * *
    
        pMe->m_MainBlock.XPos = pMe->m_Blocks[1].XPos;
        pMe->m_MainBlock.YPos = pMe->m_Blocks[1].YPos;
        break;
    
    case 2:                             //   * *
                                        // * *
    
        pMe->m_MainBlock.XPos = pMe->m_Blocks[0].XPos;
        pMe->m_MainBlock.YPos = pMe->m_Blocks[0].YPos;
        break;

    case 3:                             //   *
                                        // * * *
    
        pMe->m_MainBlock.XPos = pMe->m_Blocks[2].XPos;
        pMe->m_MainBlock.YPos = pMe->m_Blocks[2].YPos;
        break;


    case 5:                             //     *
                                        // * * *
    
        pMe->m_MainBlock.XPos = pMe->m_Blocks[3].XPos;
        pMe->m_MainBlock.YPos = pMe->m_Blocks[3].YPos;
        break;

    default:
        break;
    }
}
/*===============================================================================
FUNCTION: DrawBlocks

DESCRIPTION:
    This function draws the blocks at the given position. It uses background
    color or foreground color according to the second parameter.

PARAMETERS:
    pMe [in] - CTetrisApp structure
    bOverDraw [in] - Bool type. 

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void DrawBlocks(CTetrisApp * pMe, boolean bOverDraw)
{
    int i;
    AEERect rect;

    for(i = 0; i < BLOCK_NUM; i++)
    {
        rect.x = pMe->m_Blocks[i].XPos;
        rect.y = pMe->m_Blocks[i].YPos;
        rect.dx = BLOCK_SIZE - 1;
        rect.dy = BLOCK_SIZE - 1;

        if(bOverDraw)
        {
            IDISPLAY_DrawRect(pMe->a.m_pIDisplay, 
                              &rect, 
                              0,
                              CLR_MBG,
                              IDF_RECT_FILL);       // Erase the current block.
        }
        else
        {
            IDISPLAY_DrawRect(pMe->a.m_pIDisplay, 
                              &rect, 
                              0, 
                              pMe->m_BlockClr[pMe->m_rKind],  
                              IDF_RECT_FILL);       // Draw the current block.
        }
    }
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
}
/*===============================================================================
FUNCTION: DrawNextBlocks

DESCRIPTION:
    This function draws the next group of blocks.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void DrawNextBlocks(CTetrisApp * pMe)
{
    int i;
    int index;
    AEERect rect;                               
    
    index = pMe->m_rRand % MAX_BLOCK_KIND;

    rect.x = pMe->m_rNextStartX - NEXT_BLOCK_SIZE;
    rect.y = pMe->m_rNextStartY;
    rect.dx = NEXT_MAX_XNUM * NEXT_BLOCK_SIZE;
    rect.dy = NEXT_MAX_YNUM * NEXT_BLOCK_SIZE;

    IDISPLAY_DrawRect(pMe->a.m_pIDisplay, 
                              &rect, 
                              0, 
                              CLR_MBG,  
                              IDF_RECT_FILL);     

    rect.dx = NEXT_BLOCK_SIZE - 1;
    rect.dy = NEXT_BLOCK_SIZE - 1;

    for(i = 0; i < BLOCK_NUM; i++)
    {                                   // Set the position of next block, then display.
        rect.x = pMe->m_rNextStartX + pMe->m_BlockDataLib[index].XPos[i] * NEXT_BLOCK_SIZE;
        rect.y = pMe->m_rNextStartY + pMe->m_BlockDataLib[index].YPos[i] * NEXT_BLOCK_SIZE;

        IDISPLAY_DrawRect(pMe->a.m_pIDisplay, 
                          &rect, 
                          0, 
                          pMe->m_BlockClr[index],  
                          IDF_RECT_FILL);
    }
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
}
/*===============================================================================
FUNCTION: JudgeAction

DESCRIPTION:
    This function is used to determine the action based on the position of the stroke.
    The action may be rotation, moving down, moving left, moving right.

PARAMETERS:
    pMe     [in] - CTetrisApp structure
    dwParam [in] - uint32

RETURN VALUE:
    TRUE: If the stroke event has been processed.
    FALSE:The event is not be procesed.

SIDE EFFECTS:
    None.
===============================================================================*/
static boolean JudgeAction(CTetrisApp * pMe, uint32 dwParam)
{
    uint16 XPos,YPos;

    XPos = AEE_GET_X(dwParam);
    YPos = AEE_GET_Y(dwParam);

    if(XPos > pMe->m_MainRect.x + pMe->m_MainRect.dx)
    {
        int BTYPos;
        BTYPos = pMe->m_rNextStartY + NEXT_MAX_YNUM * NEXT_BLOCK_SIZE + 6 * pMe->m_rLineHeight;
        if(YPos > BTYPos && YPos < BTYPos + BUTTON_DY+10)
        {
            if(pMe->m_AppState == APP_STATE_RUNNING)
            {
                DrawButtons(pMe, FALSE, TRUE);
            }
            else if(pMe->m_AppState == APP_STATE_PAUSED)
            {
                DrawButtons(pMe, FALSE, FALSE);
            }
            else
            {
                return FALSE;
            }
            IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
            ProcessKey(pMe, AVK_STAR);
            return TRUE;
        }
        else if(YPos > BTYPos + BUTTON_DY + pMe->m_rLineHeight && 
                YPos < BTYPos + 2 * BUTTON_DY + pMe->m_rLineHeight+10)
        {
            if(pMe->m_AppState == APP_STATE_RUNNING  ||
               pMe->m_AppState == APP_STATE_PAUSED   ||
               pMe->m_AppState == APP_STATE_NEXTLEVEL||
               pMe->m_AppState == APP_STATE_LASTLEVEL||
               pMe->m_AppState == APP_STATE_REPORT)
            {
                ProcessKey(pMe, AVK_CLR);
                return TRUE;
            }
        }
        return FALSE;
    }
    
    if(pMe->m_AppState == APP_STATE_RUNNING)
    {
        GetMinMaxVal(pMe);
        if(YPos >= pMe->m_rVerMinVal)
        {
            if(XPos <= pMe->m_rHorMinVal)
            {
                MoveLeftRight(pMe, ACTION_LEFT);
            }
            else if(XPos >= pMe->m_rHorMaxVal)
            {
                MoveLeftRight(pMe, ACTION_RIGHT);
            }
            else if(YPos >= pMe->m_rVerMaxVal)
            {
                MoveDown(pMe, TRUE);
            }
        }
        else if(YPos <= pMe->m_rVerMinVal)
        {
            Rotate(pMe);
        }
        return TRUE;                    // Current game is running.
    }
    
    return FALSE;
}
/*===============================================================================
FUNCTION: GetMinMaxVal

DESCRIPTION:
    This function calculates the minimum and maximum horizontal and vertical 
    coordinate of the current blocks. It is called by function JudgeAction to
    determine the action according to the place of stroke.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void GetMinMaxVal(CTetrisApp * pMe)
{
    int i;
    
    pMe->m_rHorMaxVal = pMe->m_rVerMaxVal = 0;
    pMe->m_rHorMinVal = pMe->m_rVerMinVal = 1000;

    for(i = 0; i < BLOCK_NUM; i++)
    {
        if(pMe->m_Blocks[i].XPos < pMe->m_rHorMinVal)
        {
            pMe->m_rHorMinVal = pMe->m_Blocks[i].XPos;
        }

        if(pMe->m_Blocks[i].XPos >= pMe->m_rHorMaxVal)
        {
            pMe->m_rHorMaxVal = pMe->m_Blocks[i].XPos;
        }

        if(pMe->m_Blocks[i].YPos < pMe->m_rVerMinVal)
        {
            pMe->m_rVerMinVal = pMe->m_Blocks[i].YPos;
        }
        if(pMe->m_Blocks[i].YPos >= pMe->m_rVerMaxVal)
        {
            pMe->m_rVerMaxVal = pMe->m_Blocks[i].YPos;
        }
    }
    pMe->m_rHorMaxVal += BLOCK_SIZE;
    pMe->m_rVerMaxVal += BLOCK_SIZE;
}
/*===============================================================================
FUNCTION: IsFeasible

DESCRIPTION:
    This function determines if the current action is feasible. The action is passed 
    in the second parameter, which includes down, left, right and rotate.

PARAMETERS:
    pMe  [in] - CTetrisApp structure
    type [in] - ActionType 

RETURN VALUE:
    TRUE:  If the action is feasible.
    FALSE: The action is not allowable. 

SIDE EFFECTS:
    None.
===============================================================================*/
static boolean IsFeasible(CTetrisApp * pMe, ActionType type)
{
    int i;
    uint16 XPos,YPos;

    i = 0;
    XPos = 0;
    YPos = 0;

    switch(type)
    {
    case ACTION_DOWN:
        for(i = 0; i < BLOCK_NUM; i++)
        {
            YPos = pMe->m_Blocks[i].YPos + BLOCK_SIZE;
            if((pMe->m_Blocks[i].YPos + 2 * BLOCK_SIZE > pMe->m_MainRect.y + pMe->m_MainRect.dy)||
                HasBlock(pMe, pMe->m_Blocks[i].XPos, YPos))
            {
                return FALSE;
            }
        }
        return TRUE;            

    case ACTION_LEFT:
        for(i = 0; i < BLOCK_NUM; i++)
        {
            XPos = pMe->m_Blocks[i].XPos - BLOCK_SIZE;
            if((pMe->m_Blocks[i].XPos - BLOCK_SIZE < pMe->m_MainRect.x)||
                HasBlock(pMe, XPos, pMe->m_Blocks[i].YPos))
            {
                return FALSE;
            }
        }
        return TRUE;            

    case ACTION_RIGHT:
        for(i = 0; i < BLOCK_NUM; i++)
        {
            XPos = pMe->m_Blocks[i].XPos + BLOCK_SIZE;
            if((pMe->m_Blocks[i].XPos + 2 * BLOCK_SIZE > pMe->m_MainRect.x + pMe->m_MainRect.dx)||
                HasBlock(pMe, XPos, pMe->m_Blocks[i].YPos))
            {
                return FALSE;
            }
        }
        return TRUE;            

    case ACTION_ROTATE:
        for(i = 0; i < BLOCK_NUM; i++)
        {
            XPos = pMe->m_MainBlock.XPos + pMe->m_MainBlock.YPos - pMe->m_Blocks[i].YPos;
            YPos = pMe->m_MainBlock.YPos + pMe->m_Blocks[i].XPos - pMe->m_MainBlock.XPos;

            if((XPos < pMe->m_MainRect.x) || (YPos < pMe->m_MainRect.y) ||
                (XPos + BLOCK_SIZE > pMe->m_MainRect.x + pMe->m_MainRect.dx) ||
                (YPos + BLOCK_SIZE > pMe->m_MainRect.y + pMe->m_MainRect.dy) ||
                HasBlock(pMe, XPos, YPos))
            {
                return FALSE;
            }
        }
        return TRUE;           

    default:
        return FALSE;
    }
}
/*===============================================================================
FUNCTION: MoveDown

DESCRIPTION:
    This function can move down the current blocks a unit. Also, it implements
    the moving down continuously until the operation is not feasible.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void MoveDown(CTetrisApp * pMe, boolean bAcc)
{
    int i;

    if(!IsFeasible(pMe, ACTION_DOWN))
    {
        MainFlow(pMe);                          
        return;
    }

    DrawBlocks(pMe,TRUE);
    
    if(bAcc == TRUE)                            // Handle the event to accelerate.
    {
        while(IsFeasible(pMe, ACTION_DOWN))
        {
            for(i = 0; i < BLOCK_NUM; i++)
            {
                pMe->m_Blocks[i].YPos += BLOCK_SIZE;
            }
            
            pMe->m_MainBlock.YPos += BLOCK_SIZE;
        }
    }
    else
    {
        for(i = 0; i < BLOCK_NUM; i++)
        {
            pMe->m_Blocks[i].YPos += BLOCK_SIZE;
        }

        pMe->m_MainBlock.YPos += BLOCK_SIZE;
    }
    
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, pMe->m_rTime, (PFNNOTIFY)MoveDown, pMe);

    DrawBlocks(pMe,FALSE);
}
/*===============================================================================
FUNCTION: MoveLeftRight

DESCRIPTION:
    This function implements the left and right moving of the current block if feasible.

PARAMETERS:
    pMe  [in] - CTetrisApp structure
    type [in] - ActionType

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void MoveLeftRight(CTetrisApp * pMe, ActionType type)
{
    int i;

    if(IsFeasible(pMe, type))
    {
        DrawBlocks(pMe, TRUE);

        switch(type)
        {
        case ACTION_LEFT:                           // Handle the event of moving left.
            for(i = 0; i < BLOCK_NUM; i++)
            {
                pMe->m_Blocks[i].XPos -= BLOCK_SIZE;
            }
            pMe->m_MainBlock.XPos -= BLOCK_SIZE;
            break;

        case ACTION_RIGHT:                          // Handle the event of moving right.
            for(i = 0; i < BLOCK_NUM; i++)
            {
                pMe->m_Blocks[i].XPos += BLOCK_SIZE;
            }
            pMe->m_MainBlock.XPos += BLOCK_SIZE;
            break;
            
        default:
            break;
        }

        DrawBlocks(pMe, FALSE);
    }
}
/*===============================================================================
FUNCTION: Rotate

DESCRIPTION:
    This function implements the rotation of the current blocks if feasible.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void Rotate(CTetrisApp * pMe)
{
    int i;
    int XPos;
    if(IsFeasible(pMe,ACTION_ROTATE))
    {
        XPos = 0;
        DrawBlocks(pMe,TRUE);
        for(i = 0; i < BLOCK_NUM; i++)
        {
            XPos = pMe->m_Blocks[i].XPos;
            pMe->m_Blocks[i].XPos = pMe->m_MainBlock.XPos + pMe->m_MainBlock.YPos
                                    - pMe->m_Blocks[i].YPos;

            pMe->m_Blocks[i].YPos = pMe->m_MainBlock.YPos + XPos - pMe->m_MainBlock.XPos;
        }
        DrawBlocks(pMe,FALSE);
    }
}
/*===============================================================================
FUNCTION: Record

DESCRIPTION:
    Record the state of the main region's units, including color. Also, it changes
    the current minimal vertical position.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void Record(CTetrisApp * pMe)
{
    int i;
    int row,col;

    i = 0;
    row = 0;
    col = 0;

    for(i = 0; i < BLOCK_NUM; i++)
    {
        row = (pMe->m_Blocks[i].YPos - pMe->m_MainRect.y) / BLOCK_SIZE;
        col = (pMe->m_Blocks[i].XPos - pMe->m_MainRect.x) / BLOCK_SIZE;
        
        pMe->m_Pos[row][col].IsFull   = TRUE;       // Modify the relevant status.
        pMe->m_Pos[row][col].ClrIndex = pMe->m_rKind;

        if(row < pMe->m_rCurMinYPos)
        {
            pMe->m_rCurMinYPos = row;               
        }
    }
}
/*===============================================================================
FUNCTION: HasBlock

DESCRIPTION:
    Check to see if a unit is full, indicated by second and third parameter.
    
PARAMETERS:
    pMe [in] - CTetrisApp structure
    row [in] - Integer type, the horizontal position of the current block
    col [in] - Integer type, the vertical   position of the current block

RETURN VALUE:
    TRUE: If there is a block at the unit.
    FALSE: No block at the unit.

SIDE EFFECTS:
    None.
===============================================================================*/
static boolean HasBlock(CTetrisApp * pMe, uint16 row, uint16 col)
{
    uint16 i,j;

    i = 0;
    j = 0;

    i = (col - pMe->m_MainRect.y) / BLOCK_SIZE;
    j = (row - pMe->m_MainRect.x) / BLOCK_SIZE;

    if(pMe->m_Pos[i][j].IsFull == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
/*===============================================================================
FUNCTION: GetMaxYPos

DESCRIPTION:
    Get the max vertical position from the current group of blocks. This function
    is called to determine the lines need to delete in the function RemoveLines( ).

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    The max vertical position in the current group of blocks.

SIDE EFFECTS:
    None.
===============================================================================*/
static int GetMaxYPos(CTetrisApp * pMe)
{
    int i;
    int YMax;
    
    i = 0;
    YMax = 0;

    for(i = 0; i < BLOCK_NUM; i++)
    {
        if(YMax < pMe->m_Blocks[i].YPos)
        {
            YMax = pMe->m_Blocks[i].YPos;
        }
    }                                   
    return YMax;
}
/*===============================================================================
FUNCTION: RemoveLines

DESCRIPTION:
    Record the lines which is full,and change the relevant unit's state. 
    If there are lines to delete, call function Flash( ) to produce flash effect.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void RemoveLines(CTetrisApp * pMe)
{
    int i,j,k;                          
    int index;
    int YMax;
    boolean bRemove;
    AEERect rect;

    index = 0;
    YMax = 0;
    rect.dx = rect.dy = BLOCK_SIZE - 1;

    YMax = GetMaxYPos(pMe);
    index = (YMax - pMe->m_MainRect.y) / BLOCK_SIZE;

    for(i = 0; i < BLOCK_NUM; i++)
    {
        if(index < 0)
        {
            break;
        }
        bRemove = TRUE;
        for(j = 0; j < pMe->m_rHNum; j++)
        {
            if(pMe->m_Pos[index][j].IsFull == FALSE)
            {
                bRemove = FALSE;
                index = index - 1;
                break;
            }
        }
        if(bRemove)                     // There are full lines, change the relevant data.
        {
            pMe->m_LinesToRm[pMe->m_rRmLines] = index - pMe->m_rRmLines;
            pMe->m_rRmLines += 1;

            for(j = index; j >= (pMe->m_rCurMinYPos - 1) && j > 0; j--)
            {
                for(k = 0; k < pMe->m_rHNum; k++)
                {
                    pMe->m_Pos[j][k].IsFull = pMe->m_Pos[j-1][k].IsFull;
                    pMe->m_Pos[j][k].ClrIndex = pMe->m_Pos[j-1][k].ClrIndex;
                }
            }

            if(j == 0)
            {
                for(k = 0; k < pMe->m_rHNum; k++)
                {
                    pMe->m_Pos[j][k].IsFull = FALSE;
                    pMe->m_Pos[j][k].ClrIndex = MAX_BLOCK_KIND;
                }
            }
        }
    }

    if(pMe->m_rRmLines != 0)
    {
        if(pMe->m_rRmLines == 1)
        {
            PlayMusic(pMe, MUSIC_SINLINE);
        }
        else
        {
            PlayMusic(pMe, MUSIC_MULLINES);
        }
        (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)MoveDown, pMe);
        DrawBlocks(pMe, FALSE);
        Flash(pMe);                     // Produce flash effect where there are full lines.
    }
    //else
    //{
    //    PlayMusic(pMe, MUSIC_PLACE);
    //}
}
/*===============================================================================
FUNCTION: PlayMusic

DESCRIPTION:
    This function is used to play different kind of music according to the value of 
    second parameter.

PARAMETERS:
    pMe [in] - CTetrisApp structure
    type[in] - MusicType, indicates which kind of music will be played

RETURN VALUE:
    TRUE : If the MusicType exists.
    FALSE: Current kind of MusicType doesn't exist, or the sound setting is off.

SIDE EFFECTS:
    None.
===============================================================================*/
static boolean PlayMusic(CTetrisApp * pMe, MusicType type)
{
    //AEESoundPlayerInfo  PlayerInfo;
    char fileName[64];
    byte btVolLevel = 0;
    IConfig   * pConfig;
 
    if(pMe->m_bSound == FALSE)
    {
        return FALSE;
    }
    
    if (AEE_SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell, 
                                             AEECLSID_CONFIG, 
                                             (void **)&pConfig))
    {
        (void)ICONFIG_GetItem(pConfig,
                              CFGI_RINGER_VOL,
                              &btVolLevel,
                              sizeof(byte));
        
        if(btVolLevel == 0)
        {
            ICONFIG_Release(pConfig);
            pConfig = NULL;
            return FALSE;
        }
    } 
    //PlayerInfo.eInput = SDT_FILE;
    MSG_FATAL("play sound sucesss btVolLevel===%d",btVolLevel,0,0);
    MEMSET(fileName, 0, sizeof(fileName));
    STRCAT(fileName, "fs:/gamemusic/");
    
    switch(type)
    {
    case MUSIC_PLACE:
        STRCAT(fileName, "select.wav");
        break;
        
    case MUSIC_SINLINE:
        STRCAT(fileName, "singleline.wav");
        break;

    case MUSIC_MULLINES:
        STRCAT(fileName, "mullines.wav");
        break;

    case MUSIC_NEXTSPEED:
        STRCAT(fileName, "hero.wav");
        break;

    case MUSIC_NEXTLEVEL:
        STRCAT(fileName, "success.wav");
        break;

    case MUSIC_GAMEOVER:
        STRCAT(fileName, "fail.wav");
        break;

    default:
        DBGPRINTF("This music type isn't supported!");
        return FALSE;
    }
    //PlayerInfo.dwSize = 0;

    //if(AEE_SUCCESS != IRINGERMGR_SetInfo(pMe->m_pPlayer, &PlayerInfo))
    //{
    //    return FALSE;
    //}
    //IRINGERMGR_SetVolume(pMe->m_pPlayer, AEE_MAX_VOLUME);
    //IRINGERMGR_GetInfo(pMe->m_pPlayer, &PlayerInfo);
    //DBGPRINTF("eInput = %d, pData = %s", PlayerInfo.eInput, PlayerInfo.pData);

    //IRINGERMGR_RegisterNotify(pMe->m_pPlayer, NULL, NULL);
    //IRINGERMGR_Play(pMe->m_pPlayer);
	MSG_FATAL("play sound sucesss  000",0,0,0);

    (void)IRINGERMGR_Stop(pMe->m_pPlayer);
    (void)IRINGERMGR_PlayFile(pMe->m_pPlayer, fileName, 0);
	MSG_FATAL("play sound sucesss",0,0,0);
    return TRUE;
}

/*===============================================================================
FUNCTION: UpdateLines

DESCRIPTION:
    This function is used only when there are lines to delete. It updates the main 
    region according to the changed records after function flash().

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void UpdateLines(CTetrisApp * pMe)
{
    int i, j;
    int ClrIndex;
    AEERect rect;

    for(i = pMe->m_rCurMinYPos; i <= pMe->m_LinesToRm[0]; i++)
    {
        for(j = 0; j < pMe->m_rHNum; j++)
        {
            rect.x = j * BLOCK_SIZE + pMe->m_MainRect.x;
            rect.y = i * BLOCK_SIZE + pMe->m_MainRect.y;
            rect.dx = rect.dy = BLOCK_SIZE - 1;
            if(pMe->m_Pos[i][j].IsFull == FALSE)    // Use background color.
            {
                IDISPLAY_DrawRect(pMe->a.m_pIDisplay, &rect, 0, CLR_MBG, IDF_RECT_FILL);
            }
            else                                    // Use color of the block.
            {
                ClrIndex = pMe->m_Pos[i][j].ClrIndex;
                IDISPLAY_DrawRect(pMe->a.m_pIDisplay, &rect, 0, 
                                  pMe->m_BlockClr[ClrIndex], IDF_RECT_FILL);
            }
        }       
    }
    pMe->m_rCurMinYPos += pMe->m_rRmLines; //tclwhewei050801 change
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
}
/*===============================================================================
FUNCTION: ChangeScore

DESCRIPTION:
    This function is used only when there are lines to delete.
    It changes the current score when lines full of blocks have been removed.
    The score increases according to the number of lines removed, and
    the level of the game changes based on the current score. Then it calls
    function UpdateScore( ) to update the current score and level.

PARAMETERS:
    pMe [in] - CTetrisApp structure.
    lines [in] - integer.

DEPENDENCIES;
    Assume pMe is not null and lines is bigger than zero.

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static boolean ChangeScore(CTetrisApp * pMe)
{
    switch(pMe->m_rRmLines)             
    {
    case 1:
        pMe->m_rScore += 10;
        break;
        
    case 2:
        pMe->m_rScore += 30;
        break;
        
    case 3:
        pMe->m_rScore += 50;
        break;
        
    case 4:
        pMe->m_rScore += 100;
        break;
        
    default:
        break;
    }

    if(pMe->m_rScore >= LEVEL_SCORE)    
    {
        PlayMusic(pMe, MUSIC_NEXTSPEED);
        pMe->m_rSpeed += 1;   
        pMe->m_rScore -= LEVEL_SCORE;
        pMe->m_rTime -= TIME_SLICE;
        UpdateScore(pMe);
        if(pMe->m_rTime <= FLASH_TIME)
        {
            PlayMusic(pMe, MUSIC_NEXTLEVEL);
            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
            EnterNextLevel(pMe, TRUE);
            return FALSE;
        }                               // Has passed the last level.
        else if(pMe->m_rSpeed == (INIT_TIME - pMe->m_WaitingTime[pMe->m_rLevel]) / TIME_SLICE)
        {
            PlayMusic(pMe, MUSIC_NEXTLEVEL);
            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
            EnterNextLevel(pMe, FALSE);
            return FALSE;
        }                               // Passes another level.
    }
  
    UpdateScore(pMe);                   
    return TRUE;
}
/*===============================================================================
Function: GetFinalYPos

DESCRIPTION:
    This function is used to return the minimal vertical coordinate value to display 
    blocks. It is called after the current group of blocks can't display totally, 
    in other words, current game is over.

PARAMETERS:
    pMe [in] - CTetrisApp structure.

RETURN VALUE:
    uint16, the minimal vertical coordinate value when the game is over.

SIDE EFFECTS:
    None.
===============================================================================*/
static uint16 GetFinalYPos(CTetrisApp * pMe)
{
    int    i,j;
    uint8  col;
    uint16 MinYPos;

    MinYPos = pMe->m_rVNum;;

    for(j = 0; j < BLOCK_NUM; j++)
    {
        col = (pMe->m_Blocks[j].XPos - pMe->m_MainRect.x) / BLOCK_SIZE;
        for(i = 0; i < pMe->m_rVNum; i++)
        {
            if(pMe->m_Pos[i][col].IsFull == TRUE && i < MinYPos)
                MinYPos = i;
        }
    }

    DBGPRINTF("The minimal position is: %d", MinYPos);
    MinYPos = MinYPos * BLOCK_SIZE + pMe->m_MainRect.y;
    return MinYPos;
}
/*===============================================================================
Function: CutBlocks

DESCRIPTION:
    This function deals with the last group of blocks which can't be displayed
    totally. It ignores these blocks, changes the other blocks' vertical position,
    and records the revlevant position's state.

PARAMETERS:
    pMe [in] - CTetrisApp structure.

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void CutBlocks(CTetrisApp * pMe, uint16 YPos)
{
    int i;
    int height;
    int row,col;
    AEERect rect;

    if(YPos == pMe->m_MainRect.y)       // The initial place of block is full.
        return;

    height = YPos - pMe->m_MainRect.y;  // Lines need to delete.
    row = col = 0;
    for(i = 0; i < BLOCK_NUM; i++)
    {
        rect.x = pMe->m_Blocks[i].XPos;
        rect.y = pMe->m_Blocks[i].YPos - height;
        rect.dx = BLOCK_SIZE - 1;
        rect.dy = BLOCK_SIZE - 1;

        if(rect.y < pMe->m_MainRect.y)
        {
            continue;                   // The block is above the top line.
        }

        IDISPLAY_DrawRect(pMe->a.m_pIDisplay, 
                          &rect, 
                          0, 
                          pMe->m_BlockClr[pMe->m_rKind], 
                          IDF_RECT_FILL);
        
        row = (rect.y - pMe->m_MainRect.y) / BLOCK_SIZE;
        col = (rect.x - pMe->m_MainRect.x) / BLOCK_SIZE;

        pMe->m_Pos[row][col].IsFull = TRUE;
        pMe->m_Pos[row][col].ClrIndex = pMe->m_rKind;
    }
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
}
/*===============================================================================
FUNCTION: Flash

DESCRIPTION:
    This function is used only when there are lines to delete.
    It is used to produce flash effect at the lines which will be deleted,
    then calls another function UpdateLines( ) to update the main region. 

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void Flash(CTetrisApp * pMe)
{
    int i,j;
    AEERect rect;
    uint32 color;

    rect.dx = BLOCK_SIZE - 1;
    rect.dy = BLOCK_SIZE - 1;

    if(pMe->m_rFlashTimes % 2 == 0)     // Use flash color.
    {
        color = CLR_FLASH;
    }
    else                                // Use background color.
    {
        color = CLR_MBG;   
    }

    for(i = 0; i < pMe->m_rRmLines; i++)
    {
        rect.x = pMe->m_MainRect.x;
        rect.y = pMe->m_LinesToRm[i] * BLOCK_SIZE + pMe->m_MainRect.y;
        for(j = 0; j < pMe->m_rHNum; j++)
        {
            IDISPLAY_FillRect(pMe->a.m_pIDisplay, &rect, color);
            rect.x += BLOCK_SIZE;
        }
    }                                   // Refresh the full lines in selected color.
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
    
    if(pMe->m_rFlashTimes < MAX_FLASH_TIMES)
    {
        pMe->m_rFlashTimes += 1;
        (void)ISHELL_SetTimer(pMe->a.m_pIShell, FLASH_TIME, (PFNNOTIFY)Flash, pMe);
    }
    else
    {
        UpdateLines(pMe);               // Flash times reached, redraw these lines.
        pMe->m_rFlashTimes = 0;
        pMe->m_rRmLines = 0;
        DrawBlocks(pMe, FALSE);
        (void)ISHELL_SetTimer(pMe->a.m_pIShell, pMe->m_rTime, (PFNNOTIFY)MoveDown, pMe);
    }
}
/*===============================================================================
FUNCTION: SaveSettingData

DESCRIPTION:
    Save current level, grid setting, sound setting for next use.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void SaveSettingData(CTetrisApp * pMe)
{
    (void)ISHELL_GetPrefs(pMe->a.m_pIShell, 
                          AEECLSID_TETRIS, 
                          TETRIS_VERSION, 
                          &(pMe->m_CfgData),
                          sizeof(CPrefData)); 

    pMe->m_CfgData.m_nLevel = pMe->m_rLevel;
    pMe->m_CfgData.m_bGrid  = pMe->m_bGrid;
    pMe->m_CfgData.m_bSound = FALSE;

    ISHELL_SetPrefs(pMe->a.m_pIShell, 
                    AEECLSID_TETRIS,
                    TETRIS_VERSION,
                    &(pMe->m_CfgData),
                    sizeof(CPrefData));
}
/*===============================================================================
FUNCTION: GameOver

DESCRIPTION:
    This function fills the main region in the background color, then calls 
    UpdateScreen to indicate the game is over.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void GameOver(CTetrisApp * pMe)
{
    pMe->m_AppState = APP_STATE_OVER;
    MSG_FATAL("GameOver..............",0,0,0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
    UpdateScreen(pMe);                  
}
/*===============================================================================
FUNCTION: UpdateScreen

DESCRIPTION:
    Update the main region after game is over. Display a row of blocks after a 
    certain time from the bottom to the top.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void UpdateScreen(CTetrisApp * pMe)
{
    int j;
    MSG_FATAL("pMe->m_Rect.y==%d,,,pMe->m_MainRect.y=%d",pMe->m_Rect.y,pMe->m_MainRect.y,0);
    if(pMe->m_Rect.y > pMe->m_MainRect.y)
    {
        pMe->m_Rect.y -= BLOCK_SIZE;    // Change vertical position, refresh the next line.
    }
    else
    {
    	MSG_FATAL("SetCfgData..............",0,0,0);
        SetCfgData(pMe);
        return;
    }

    pMe->m_Rect.x = pMe->m_MainRect.x;  // Reset the value of horizontal coordinate.
    for(j = 0; j < pMe->m_rHNum; j++)
    {
        IDISPLAY_FillRect(pMe->a.m_pIDisplay, &pMe->m_Rect, CLR_OVER);
        pMe->m_Rect.x += BLOCK_SIZE;
    }
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
    MSG_FATAL("ISHELL_SetTimer..............",0,0,0);
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, OVER_TIME, (PFNNOTIFY)UpdateScreen, pMe);
}
/*===============================================================================
FUNCTION: SetCfgData

DESCRIPTION:
    This function changes the score record if current score is bigger than the one
    in the same difficulty.

PARAMETERS:
    pMe [in] - CTetrisApp structure

RETURN VALUE:
    None.

SIDE EFFECTS:
    None.
===============================================================================*/
static void SetCfgData(CTetrisApp * pMe)
{
    int     index;
    uint16  strID, BaseSpeed;
    uint16  TotalScore;

    index = pMe->m_rLevel - 1;
    BaseSpeed = (INIT_TIME - pMe->m_WaitingTime[index]) / TIME_SLICE;
    TotalScore = (pMe->m_rSpeed - BaseSpeed) * LEVEL_SCORE + pMe->m_rScore;
    if(TotalScore > pMe->m_CfgData.Score[index])            // Current scord breaks the record.
    {
        (void)ISHELL_GetPrefs(pMe->a.m_pIShell, 
                          AEECLSID_TETRIS, 
                          TETRIS_VERSION, 
                          &(pMe->m_CfgData),
                          sizeof(CPrefData)); 

        pMe->m_CfgData.Score[index] = TotalScore;
        (void)ISHELL_SetPrefs(pMe->a.m_pIShell, 
                              AEECLSID_TETRIS, 
                              TETRIS_VERSION, 
                              &(pMe->m_CfgData),
                              sizeof(CPrefData)); 
                                                            // Save new record.
        strID = IDS_REPORT_RECORD;
        ReportScore(pMe, strID, TotalScore, TRUE);
    }
    else            
    {
        strID = IDS_REPORT_SCORE;
        ReportScore(pMe, strID, TotalScore, FALSE);
    }
}
/*===============================================================================
FUNCTION: ReportScore

DESCRIPTION:
    Displays current score on the screen. According to the score, there are two 
    different kinds of information.

PARAMETERS:
    pMe   [in] - Pointer to CTetrisApp structure.
    strID [in] - ID of the string resource in the resource file.
    Score [in] - The current score.
    NewRecord [in] - Is current score bigger than the one on the record.

RETURN VALUE:
    None.
===============================================================================*/
static void ReportScore(CTetrisApp * pMe, uint16 strID, uint32 Score, boolean NewRecord)
{
    uint16   TitleID;
    uint32   StaticProp;
    AECHAR   Title[20];                  
    AECHAR   Str[200];
    AECHAR   Text[200];                  
    AEERect  Rect;

    Reset(pMe, FALSE);

    SETAEERECT(&Rect, LARGE_POP_WIN_X, LARGE_POP_WIN_Y, LARGE_POP_WIN_DX, LARGE_POP_WIN_DY);
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, TETRIS_RES_FILE_LANG, 
                               strID, Str, sizeof(Str));
    if(NewRecord == TRUE)               
    {
        WSPRINTF(Text, sizeof(Text), Str, Score);   
    }
    else
    {
        WSPRINTF(Text, sizeof(Text), Str, Score, pMe->m_CfgData.Score[pMe->m_rLevel - 1]);
    }                                   

    TitleID = pMe->m_rLevel + IDS_TETRIS_LEVEL_1 - 1;
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                               TETRIS_RES_FILE_LANG, 
                               TitleID, 
                               Title, 
                               sizeof(Title));
    DrawCtrlFrame(pMe, TitleID, &Rect);
    
    StaticProp = ST_CENTERTITLE | ST_UNDERLINE | ST_CENTERTEXT | ST_MIDDLETEXT|ST_NOSCROLL;
    ISTATIC_SetRect(pMe->m_pStatic, &Rect);
    ISTATIC_SetProperties(pMe->m_pStatic, StaticProp);
    (void)ISTATIC_SetText(pMe->m_pStatic, 
                          NULL, 
                          Text, 
                          AEE_FONT_BOLD | AEE_FONT_LARGE, 
                          AEE_FONT_NORMAL);     

    ISTATIC_SetActive(pMe->m_pStatic, TRUE);
	MSG_FATAL("pMe->m_pStatic, TRUE",0,0,0);
    (void)ISTATIC_Redraw(pMe->m_pStatic);       
    IDISPLAY_Update(pMe->a.m_pIDisplay);        // Display the relevant information.       

    pMe->m_AppState = APP_STATE_REPORT;
}

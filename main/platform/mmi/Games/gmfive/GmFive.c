/*======================================================
FILE:  GmFive.c

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

=====================================================*/

/*=============================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
08/17/2004   tcl_wb  (wangbin040817)铃声音量为零时不播放背景音乐.
08/09/2004   tcl_wb  (wangbin040809)删除按键音的全局变量"gKeyBeepVolumeCache".
12/03/2003   jhh     Create this file.

=============================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "gmfive.h"
#ifdef AEE_STATIC
#include "AEEAlert.h"
#endif //
/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static boolean GmFive_HandleEvent(IApplet * pi, AEEEvent eCode,
                                      uint16 wParam, uint32 dwParam);

static void GmFive_Reset(GmFive *pMe);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */
static boolean GmFive_Init(IApplet* pi);
static boolean GmFive_Release(IApplet* pi);
static void GmFive_DisplaySplashScreen( GmFive* pMe );
static void GmFive_DisplayMainScreen(GmFive * pMe);
static void SelectPlayChessModeScreen(GmFive *pMe);
static void SelectChessLevelScreen(GmFive *pMe);
static void GmFive_DispHelpInforScreen(GmFive* pMe);
static void DispHeroScoreScreen(GmFive* pMe);
static void DrawGameOverScreen(GmFive *pMe);
#ifdef  SOUND_FEATURE
static void GMFive_SetSoundModeScreen(GmFive *pMe);
static boolean GMFive_ShowRingViewScreen(GmFive *pMe);
static void GetMenuItemData(GmFive *pMe);
static void PreListenRinger(GmFive *pMe);
#endif //SOUND_FEATURE
static boolean KeyProcess(GmFive * pMe,uint16 wParam);
static void SetPopMenuAttr(GmFive *pMe,uint16 titleID, AEERect * pRect, uint32 dwProps);
static void GmFive_DrawBoard(GmFive *pMe);
static void GmFive_NewGameStart(GmFive * pMe);
static void InitChess(GmFive *pMe);
static void InitRepentQueue(CHESS_OBJECT *chess_object);
static void RestartRepentQueue(CHESS_OBJECT *chess_object);
static void AddToRepentQueue(CHESS_OBJECT *chess_object, int16 x, int16 y);
static void DrawCurSquare(GmFive *pMe);
static void ClearHistorySquare(GmFive *pMe ,boolean clrFlag);
static void DrawScreenBackground(GmFive * pMe);
static uint32 GmFive_GetRndNumber(uint32 max);
static char ManGo(GmFive * pMe);
static void MachineGo(GmFive * pMe);
static void FlashDrawPiece(GmFive * pMe,char col, char row);
static byte ConnectSub(CHESS_OBJECT *chess_object, byte left, byte right, uint16  board_pattern);
static dword Connect(CHESS_OBJECT *chess_object, uint16  x, uint16  y, byte who);
static byte MachinePutChess(CHESS_OBJECT *chess_object, uint16 *put_x, uint16 *put_y, char who);
static uint16 CalValue(CHESS_OBJECT *chess_object, dword connect_result);
static boolean JudgeWin(CHESS_OBJECT *chess_object);
static boolean ChessPlayOver(GmFive * pMe);
static void MachineDrawDifColrPiece(GmFive * pMe);
static void SplashCurSquare(GmFive * pMe);
static boolean MoveFromRepentQueue(GmFive* pMe);
static void AlertDispCallBack(GmFive *pMe);
static void SaveChessLevelScore(GmFive* pMe);
static void SaveChessConfigData(GmFive* pMe);
static void GetChessConfigData(GmFive* pMe);
static void ResumeHeroInitValue(GmFive* pMe);
static void DrawCtrlFrame(GmFive *pMe,uint16 titleID,AEERect *fRect);
static void DispAlertInfo(GmFive *pMe,uint16 titleID,uint16 contentID);
#if defined(AEE_STATIC)
static void SetSystemKeyBeepVol(GmFive *pMe,boolean resume);//tcl jhh20040118
extern void CRingerMgr_SetVol(IRingerMgr * po, uint32 vol, boolean bEscalate);
#else
static void GmFive_AddSoftkeyPrompt(GmFive *pMe,SoftkeyPrompt *prompt);
static void RedrawSoftKeyPrompt(GmFive *pMe,uint16 leftID,uint16 middleID,uint16 rightID);
#endif /*AEE_STATIC*/
static boolean JudgeThreeAndThreeRule(GmFive * pMe);
static boolean PenProcess(GmFive *pMe, uint32 dwParam);
static void ResumeJustChess(GmFive* pMe);
//added by tcl wgs 041217
static void DrawExitButton(GmFive *pMe); 
static void DrawSoundButton(GmFive *pMe);
//end added by tcl wgs 041217
/*===========================================================================

                      STATIC/LOCAL DATA

===========================================================================*/
#if defined(AEE_STATIC)

static int     GmFive_CreateInstance(AEECLSID  ClsId,
                                     IShell   *pIShell,
                                     IModule  *po,
                                     void    **ppObj);

int     GmFive_Load(IShell *ps, void *pHelpers, IModule **pMod);

static const AEEAppInfo    gaiGmFive =
{
   AEECLSID_GMFIVE,
   GMFIVE_RES_FILE_LANG,
   IDS_APP_TITLE,
   0,0,0,0,
   AFLAG_HIDDEN
};

#endif //AEE_STATIC

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#if defined(AEE_STATIC)
/*=============================================================================
FUNCTION:  GmFive_GetModInfo

DESCRIPTION: Returns module information

PARAMETERS:
   *ps:
   **ppClasses:
   **pApps:
   *pnApps:
   *pwMinPriv:

RETURN VALUE:
   PFNMODENTRY:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
PFNMODENTRY GmFive_GetModInfo(IShell      *ps,
                              AEECLSID   **ppClasses,
                              AEEAppInfo **pApps,
                              uint16      *pnApps,
                              uint16      *pwMinPriv)
{
   PARAM_NOT_REF(ps)
   PARAM_NOT_REF(ppClasses)
   PARAM_NOT_REF(pwMinPriv)

   *pApps = (AEEAppInfo *)&gaiGmFive;
   *pnApps = sizeof(gaiGmFive) / sizeof(AEEAppInfo);
   return (PFNMODENTRY)GmFive_Load;
}

/*=============================================================================
FUNCTION:  CGmTShape_Load

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
//no static for mod table ex use
int GmFive_Load(IShell    *ps,
                           void      *pHelpers,
                           IModule **pMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           ps,
                           pHelpers,
                           pMod,
                           (PFNMODCREATEINST)GmFive_CreateInstance,
                           (PFNFREEMODDATA)NULL);
}
#endif  /*AEE_STATIC*/

/*===============================================================================
//applet create instance
=============================================================================== */
#if defined(AEE_STATIC)
int GmFive_CreateInstance(AEECLSID  ClsId,
                         IShell    *pIShell,
                         IModule   *po,
                         void     **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
   *ppObj = NULL;

   if(ClsId == AEECLSID_GMFIVE)
   {
      if(AEEApplet_New(sizeof(GmFive),
                   ClsId,
                   pIShell,
                   po,
                   (IApplet**)ppObj,
                   (AEEHANDLER)GmFive_HandleEvent,
                   (PFNFREEAPPDATA)GmFive_Release) == TRUE)
      {
         if(GmFive_Init((IApplet*)*ppObj))
            return AEE_SUCCESS;
         else
         {
            //Release the applet. This will free the memory allocated for the applet when
            //lint -e665*/
            IAPPLET_Release((IApplet*)*ppObj);
            *ppObj = NULL;
            return EFAILED;
         }
      }
   }
   return EFAILED;
}

///////////////////////////////////////////////
//applet handle event
//////////////////////////////////////////////
static boolean GmFive_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   GmFive*    pMe = (GmFive *)pi;
   IShell*    ps  = pMe->a.m_pIShell;        
      
   switch (eCode)
   {
      case EVT_APP_START:
	  	  if(pMe->m_pIAnn != NULL)
          {	    			
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);				
				MSG_FATAL("***zzg Application EVT_APP_START EnableAnnunciatorBar TRUE",0,0,0);				   
          }
		  {
                AECHAR WTitle[20] = {0};

				(void)ISHELL_LoadResString(pMe->a.m_pIShell,
										GMFIVE_RES_FILE_LANG,								  
										IDS_APP_TITLE,
										WTitle,
										sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                	int i = WSTRLEN(WTitle);
                	MSG_FATAL("===============%d",i,0,0);
    			    IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
            	}
         }
	  	  MSG_FATAL("GmFive_HandleEvent..................",0,0,0);
          GmFive_DisplaySplashScreen(pMe);
          return(TRUE);

      case EVT_APP_STOP:

         #ifdef SOUND_FEATURE
         if(pMe->m_pIRingerMgr) //close the playing ringer
            (void)IRINGERMGR_Stop(pMe->m_pIRingerMgr);
         #endif

   #if defined(AEE_STATIC)
         SetSystemKeyBeepVol(pMe,TRUE);
   #endif
         SaveChessConfigData(pMe);
         return TRUE;

      case EVT_APP_SUSPEND:  //suspend event

      #ifdef AEE_STATIC
         SetSystemKeyBeepVol(pMe,TRUE);
      #endif //AEE_STATIC
         (void)ISHELL_CancelTimer(pMe->a.m_pIShell,NULL,pMe);

      #ifdef SOUND_FEATURE
         if(pMe->m_pIRingerMgr) //close the playing ringer
         {
            (void)IRINGERMGR_Stop(pMe->m_pIRingerMgr);
            pMe->m_preListenFlag = FALSE;
            
         }
         if (pMe->m_promptVoice)
         {
            (void)IRINGERMGR_Stop(pMe->m_promptVoice);
            
         }
      #endif //SOUND_FEATURE
         GmFive_Reset(pMe);//set all control no active
         return TRUE;

      case EVT_APP_RESUME:
      {
         uint16   eventWparam = USRE_DRAW;
 
         switch(pMe->m_eAppState)
         {
            case APP_STATE_NEWGAME:
            #ifdef AEE_STATIC
               SetSystemKeyBeepVol(pMe,FALSE);
            #endif //AEE_STATIC
               ResumeJustChess(pMe);
               if(pMe->m_gameOverFlag)
               {
                  (void)ISHELL_CancelTimer(pMe->a.m_pIShell,NULL,pMe);
                  ISHELL_SetTimer(pMe->a.m_pIShell,
                                  GAME_OVER_DISP_TIMER,
                                 (PFNNOTIFY) DrawGameOverScreen,
                                  pMe);
               }
               return TRUE;

            case APP_STATE_HELP:
               eventWparam = IDS_GAME_HELP;
               break;

         #ifdef  SOUND_FEATURE
            case APP_STATE_SELECT_RINGER:
               eventWparam = IDS_BACKRINGER_SELECT;
               break;
         #endif
            default:
               eventWparam = USRE_DRAW;
               break;
         }
         (void)ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_GMFIVE, EVT_COMMAND, eventWparam, 0);
         return TRUE;
      }

      case EVT_COMMAND:          // Process menu command event
         if((wParam == IDS_GAME_NEW)||(wParam == IDS_PLAY_CHESS_MODE)||
            (wParam == IDS_CHESS_LEVLE)||(wParam == IDS_GAME_HELP)||
            (wParam == IDS_GAME_ABOUT)||(wParam == IDS_CONTINUE_GAME)||
         #ifdef  SOUND_FEATURE
            (wParam == IDS_BACKRINGER_SELECT)||(wParam == IDS_SOUND_SET)||
         #endif
            (wParam == IDS_HERO_SCORE))
         {
            pMe->m_SelMenuItem = IMENUCTL_GetSel(pMe->m_pMenuCtl);

         }

         switch( wParam )
         {            
        #if defined(AEE_STATIC)
            //case IDI_BOTTOMBAR_PLAY:
            case 1://IDI_BOTTOMBAR_STOP:
                KeyProcess(pMe,AVK_F2);
                return TRUE;
        #endif     
        
            case IDS_HERO_OK: //frome hero screen
            case IDS_OK:     //from help
            case USRE_DRAW:  // redraw mainmenu screen
               //pMe->m_eAppState = APP_STATE_MAIN_MENU;
               GmFive_DisplayMainScreen(pMe);
               return TRUE;

            case IDS_GAME_NEW:  //new game
               pMe->m_eAppState = APP_STATE_NEWGAME;
               GmFive_NewGameStart(pMe);
               return TRUE;

            case IDS_CONTINUE_GAME:   //continue game
               pMe->m_eAppState = APP_STATE_NEWGAME;
               ResumeJustChess(pMe);
               return TRUE;

            case IDS_PLAY_CHESS_MODE://select play chess mode
            {
               pMe->m_eAppState = APP_STATE_SELECT_CHESS_MODE;
               SelectPlayChessModeScreen(pMe);
           #if defined(AEE_STATIC)     
               DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
           #else               
               RedrawSoftKeyPrompt(pMe,NULL,NULL,IDS_RINGER_CANCEL);               
           #endif
               return TRUE;
            }

            case IDS_CHESS_LEVLE:  //chess level select
               pMe->m_eAppState = APP_STATE_SELECT_CHESS_LEVEL;
               SelectChessLevelScreen(pMe);
           #if defined(AEE_STATIC)     
               DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
           #else                   
               RedrawSoftKeyPrompt(pMe,NULL,NULL,IDS_RINGER_CANCEL);               
           #endif    
               return TRUE;

            case MACHINE_PLAY_CHESS:  //machine play chess at this time
               if((pMe->m_chess_object.who_go != MAN )|| (pMe->m_chess_object.who_win != INIT_WIN))
               {
                  ClearHistorySquare(pMe,TRUE);
                  MachineGo(pMe);
               }
               if(!ChessPlayOver(pMe))
               {
                  DrawCurSquare(pMe);
               }
               return TRUE;

            case IDS_HERO_SCORE:  //hero score                              
               pMe->m_eAppState = APP_STATE_HERO_SCORE;
               DispHeroScoreScreen(pMe);
           #if defined(AEE_STATIC)     
               DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
           #else           
               RedrawSoftKeyPrompt(pMe,NULL,NULL,IDS_RINGER_CANCEL);               
           #endif               
               return TRUE;

            case IDS_HERO_RESUME_INIT://set hero score again
               ResumeHeroInitValue(pMe);
               return TRUE;

         #ifdef  SOUND_FEATURE
            case IDS_BACKRINGER_SELECT:  //background sound select
               if(!GMFive_ShowRingViewScreen(pMe))
               {
                  pMe->m_eAppState = APP_STATE_DISP_INFO;
                  DispAlertInfo(pMe,wParam,IDS_NO_RINGER);
              #if defined(AEE_STATIC)     
                  DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
              #endif                    
                  return TRUE;
               }               
               return TRUE;

            case IDS_SOUND_SET:
               pMe->m_eAppState = APP_STATE_SOUND_MODE;               
               GMFive_SetSoundModeScreen(pMe);               
           #if defined(AEE_STATIC)     
               DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
           #else           
               RedrawSoftKeyPrompt(pMe,NULL,NULL,IDS_RINGER_CANCEL);                   
           #endif     
               return TRUE;

            case IDS_SOUND_OPEN_MODE:
            case IDS_SOUND_CLOSE_MODE:
               pMe->m_soundMode = wParam;
               AlertDispCallBack(pMe);
               return TRUE;
         #endif //SOUND_FEATURE
         
         #ifndef AEE_STATIC         
            case IDS_GAME_ABOUT://about applet information
               IDISPLAY_ClearScreen( pMe->a.m_pIDisplay);
               ISHELL_ShowCopyright(ps);
               pMe->m_eAppState = APP_STATE_ABOUT;
               return TRUE;
         #endif //AEE_STATIC

            case IDS_GAME_EXIT:  //exit applet
               (void)ISHELL_CloseApplet(ps,FALSE);
               return TRUE;

            case IDS_GAME_HELP:  //display help information
               pMe->m_eAppState = APP_STATE_HELP;
               GmFive_DispHelpInforScreen(pMe);               
               return TRUE;

            case IDS_GAME_GO_MODE_1://man play chess first
            case IDS_GAME_GO_MODE_2:
            {
               char tmpVal = pMe->m_global_who_go;

               pMe->m_global_who_go = MAN + (wParam -IDS_GAME_GO_MODE_1);
               if(tmpVal != pMe->m_global_who_go) pMe->m_gameStartFlag = FALSE;
               AlertDispCallBack(pMe);
               return TRUE;
            }

            case IDS_GAME_LEVEL_1://chess level 1
            case IDS_GAME_LEVEL_2:
            case IDS_GAME_LEVEL_3:
            case IDS_GAME_LEVEL_4:
            {
               char tmpVal = pMe->m_global_game_level;

               pMe->m_global_game_level = INIT_LEVEL+(wParam -IDS_GAME_LEVEL_1);
               if(tmpVal != pMe->m_global_game_level)  pMe->m_gameStartFlag = FALSE;
               AlertDispCallBack(pMe);
               return TRUE;
            }
         }//switch(wParam)
         break;

      case EVT_KEY:              // Process key event
         if (UpAndDownScroll_HandleEvent(pMe->a.m_pIShell,pMe->m_pMenuCtl, eCode, wParam))
            return TRUE;
         if (KeyProcess(pMe, wParam))
         {
            return TRUE;
         }
         break;      
        
      case EVT_PEN_DOWN:
         if((pMe->m_eAppState == APP_STATE_NEWGAME)&&(!pMe->m_gameOverFlag) && (pMe->m_chess_object.who_go == MAN))
         {
            return PenProcess(pMe, dwParam);
         }
         
      case EVT_PEN_UP:
      case EVT_PEN_MOVE:    
         if (pMe->m_eAppState != APP_STATE_NEWGAME )
         {
          #ifdef AEE_STATIC      
             if (AEE_GET_Y(dwParam) >= (pMe->m_cyScreen - GMFIVE_SOTFKEY_CY))   
             {
                 //if (TCLDrawBottomBar_HandleEvent ( &pMe->m_BottomBar,  pMe->a.m_pIShell, pMe->a.m_pIDisplay, eCode, wParam,dwParam))
                 {
                     return TRUE;
                 }
             }
          #endif //AEE_STATIC   
         }            
         break;

      case EVT_COPYRIGHT_END:    // Copyright dialog ended
         GmFive_DisplayMainScreen(pMe);
         return TRUE;

      default:
         break;
   }

   // The application did not need to handle the event so pass it to each
   // of the controls to let then deal with it.
   if (IMENUCTL_IsActive(pMe->m_pMenuCtl))
   {
       if( IMENUCTL_HandleEvent( pMe->m_pMenuCtl, eCode, wParam, dwParam ) )
          return TRUE;
   }

   if (IMENUCTL_IsActive(pMe->m_pSoftKeyCtl))
   {
       if( IMENUCTL_HandleEvent( pMe->m_pSoftKeyCtl, eCode, wParam, dwParam ) )
          return TRUE;
   }
   
   if (pMe->m_eAppState == APP_STATE_HELP)
   {
       if( ISTATIC_HandleEvent( pMe->m_pStaticCtrl, eCode, wParam, dwParam ) )
           return TRUE;
   }
   else
   {
       if (ISTATIC_IsActive(pMe->m_pStaticCtrl))
       {
           if( ISTATIC_HandleEvent( pMe->m_pStaticCtrl, eCode, wParam, dwParam ) )
               return TRUE;
       }    
   }   
   
   return FALSE;
}//GmFive_HandleEvent()

///////////////////////////////////////////////
//applet init
//////////////////////////////////////////////
static boolean GmFive_Init(IApplet* pi)
{
   GmFive *pMe = (GmFive *)(void *)pi;
   IShell   *ps  = pMe->a.m_pIShell;
   int       nAscent, nDescent;
   AEEDeviceInfo pdi;

   if(pMe == NULL)  return FALSE;

   //create common obj
   if(!CreateObj(ps, AEECLSID_GRAPHICS, (void **)&pMe->m_pGraphics))
      return FALSE;

   if(!CreateObj(ps, AEECLSID_STATIC, (void **)&pMe->m_pStaticCtrl))
      return FALSE;

   if(!CreateObj(ps, AEECLSID_SOFTKEYCTL, (void **)&pMe->m_pSoftKeyCtl))
      return FALSE;

   if(!CreateObj(ps, AEECLSID_MENUCTL, (void **)&pMe->m_pMenuCtl))
      return FALSE;

#ifdef  SOUND_FEATURE
   if(!CreateObj(ps, AEECLSID_RINGERMGR, (void **)&pMe->m_pIRingerMgr))
      return FALSE;
#endif
   if(!CreateObj(ps, AEECLSID_RINGERMGR, (void **)&pMe->m_promptVoice))
      return FALSE;   
   if (ISHELL_CreateInstance(ps, AEECLSID_ANNUNCIATOR,(void **) &pMe->m_pIAnn))
	{
		return FALSE;
	}
#if defined(AEE_STATIC)   
   // Create IConfig interface
   if(!CreateObj(ps, AEECLSID_CONFIG, (void **)&pMe->m_pConfig))
      return FALSE;
   {
        byte  RingVol;
                
        //初始化音量
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_RINGER_VOL,
                          &RingVol,
                          sizeof(byte));
		#ifdef  SOUND_FEATURE
        IRINGERMGR_SetRinger( pMe->m_pIRingerMgr, 
                           GET_ISOUND_VOL_LEVEL((uint8) RingVol), 
                           TRUE);    
		#endif
   }   
   (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_BEEP_VOL, &pMe->m_beepVol, sizeof(byte));
   (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_RINGER_VOL, &pMe->m_RingVol, sizeof(byte));
#endif //AEE_STATIC
   
   // obtain device information
   ISHELL_GetDeviceInfo(ps,&pdi);
   pMe->m_cxScreen = (int16)pdi.cxScreen;
   pMe->m_cyScreen = (int16)pdi.cyScreen;


   IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_LARGE, &nAscent, &nDescent);
   pMe->m_nLChSize = nAscent + nDescent;

   IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, &nAscent, &nDescent);
   pMe->m_nNChSize  = nAscent + nDescent;

   IDISPLAY_SetColor(pMe->a.m_pIDisplay,CLR_USER_FRAME, POPMENU_FRAME_LINE);
   pMe->m_eAppState = APP_STATE_MAIN_MENU;
   //get chess level score information
   GetChessConfigData(pMe);
#ifdef SOUND_FEATURE
   MEMSET(pMe->m_playFile,0,sizeof(pMe->m_playFile));
   pMe->m_ringerSelItemID = 0;
   pMe->m_soundMode = IDS_SOUND_CLOSE_MODE;
#endif

   SETAEERECT(&pMe->m_frameRect, pMe->m_cxScreen/8, pMe->m_cyScreen/4, (pMe->m_cxScreen*3)/4, DISP_POPMENU_ITEM_NUM*(pMe->m_nNChSize+3));


   pMe->m_global_who_go = pMe->m_ChessCfgData.who_go;
   pMe->m_gameOverFlag  = FALSE;
   pMe->m_gameStartFlag = FALSE;   
   pMe->m_global_game_level = pMe->m_ChessCfgData.level;
   pMe->m_winPieceSplashFlg = FALSE;
   
   pMe->m_pTopBGImage  = ISHELL_LoadResImage( pMe->a.m_pIShell, GMFIVE_RES_FILE_LANG, IDB_TOP_BGBMP );
   if(pMe->m_pTopBGImage)
   {
      IIMAGE_SetParm(pMe->m_pTopBGImage,IPARM_ROP, AEE_RO_TRANSPARENT, 0) ;
   }

   //load softkey bottom image
   pMe->m_pBottomImage = ISHELL_LoadResImage( pMe->a.m_pIShell, GMFIVE_RES_FILE_LANG, IDB_BOTTOM);
   if (pMe->m_pBottomImage)
   {
        IIMAGE_SetParm(pMe->m_pBottomImage,IPARM_ROP, AEE_RO_TRANSPARENT, 0) ;
   }

   return TRUE;

}//GmFive_Init()

///////////////////////////////////////////////
//applet release interface
//////////////////////////////////////////////
static boolean GmFive_Release(IApplet* pi)
{
   GmFive *pMe = (GmFive *)(void *)pi;

   if(pMe == NULL)
   {
      return FALSE;
   }
   ReleaseObj((void **)&pMe->m_pGraphics);
   ReleaseObj((void **)&pMe->m_pStaticCtrl);
   ReleaseObj((void **)&pMe->m_pSoftKeyCtl);
   ReleaseObj((void **)&pMe->m_pMenuCtl);
#ifdef  SOUND_FEATURE
   ReleaseObj((void **)&pMe->m_pIRingerMgr);
#endif
#if defined(AEE_STATIC)   
   ReleaseObj((void **)&pMe->m_pConfig);
#endif
   ReleaseObj((void **)&pMe->m_pTopBGImage);
   ReleaseObj((void **)&pMe->m_pBottomImage);   
   ReleaseObj((void **)&pMe->m_promptVoice);   
   ReleaseObj((void **)&pMe->m_pIAnn);  
   return TRUE;
}//GmFive_Release()


//////////////////////////////////////////////
//display image when applet start
//////////////////////////////////////////////
static void GmFive_DisplaySplashScreen( GmFive* pMe )
{
    IImage*       pSplash = NULL;
    AEEImageInfo  rImageInfo;
   
  //added  by tcl wgs 041217
    AEERect       rc;
    RGBVAL        flashbcrgbvalue;

	rc.x = 0;
	rc.y = 0;
	rc.dx = pMe->m_cxScreen;
	rc.dy = pMe->m_cyScreen;
  
    flashbcrgbvalue = MAKE_RGB(0,104,139);
  
    // The current app state is APP_STATE_START, clear the screen, load the resource image
    // and display it.
  
    
    IDISPLAY_FillRect(pMe->a.m_pIDisplay,&rc,flashbcrgbvalue);
   //end added by tcl wgs 041217
   pSplash = ISHELL_LoadResImage( pMe->a.m_pIShell, GMFIVE_RES_FILE_LANG, IDB_SPLASH );

  if( pSplash )
  {
    // Get image information
    IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
    MSG_FATAL("pSplash IS NOT null.............",0,0,0);
    IIMAGE_GetInfo( pSplash, &rImageInfo );
    
    // Draw the image in the center of the screen
    IIMAGE_Draw( pSplash, ( pMe->m_cxScreen / 2 ) - ( rImageInfo.cx / 2 ),
               ( pMe->m_cyScreen / 2 ) - ( rImageInfo.cy / 2 ) );
    // The image is no longer needed so release it

    IIMAGE_Release( pSplash );
    pSplash = NULL;
    // Set the callback timer to call this function again
    ISHELL_SetTimer( pMe->a.m_pIShell, SPLASH_TIMER_DURATION, ( PFNNOTIFY ) GmFive_DisplayMainScreen, pMe );
  }
  else
  {
  	MSG_FATAL("pSplash IS null.............",0,0,0);
    GmFive_DisplayMainScreen( pMe );
  }
  MSG_FATAL("pSplash IDISPLAY_Update .............",0,0,0);
  IDISPLAY_Update( pMe->a.m_pIDisplay );
}//GmFive_DisplaySplashScreen()


///////////////////////////////////////////////
//keyProcess event
//////////////////////////////////////////////
static boolean KeyProcess(GmFive * pMe,uint16 wParam)
{
   if(pMe == NULL)
      return FALSE;

   switch( wParam )
   {
      case AVK_SELECT:
      case AVK_5:
      case AVK_F0:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_NEWGAME:
               if((pMe->m_chess_object.who_go == MAN)||(pMe->m_chess_object.who_win ))
               {
                  (void)ManGo(pMe);                  
                  return TRUE;
               }
               break;

         #ifdef SOUND_FEATURE
            case APP_STATE_SELECT_RINGER:
               GetMenuItemData(pMe);  //get selected ringer filename
               if(pMe->m_pIRingerMgr) //close the preplaying ringer
               {
                  (void)IRINGERMGR_Stop(pMe->m_pIRingerMgr);
               }
               return TRUE;
         #endif
         
            case APP_STATE_HELP:
                GmFive_DisplayMainScreen(pMe);  
                return TRUE;

            default:
               break;
         }
         break;

      case AVK_CLR:  // Handle actions when the user presses the clear key
         switch( pMe->m_eAppState )
         {
         #ifdef SOUND_FEATURE
            case APP_STATE_SOUND_MODE:
            case APP_STATE_SELECT_RINGER:
         #endif
            case APP_STATE_DISP_INFO:
            case APP_STATE_NEWGAME:
               (void)ISHELL_CancelTimer(pMe->a.m_pIShell,NULL,pMe);
            #ifdef SOUND_FEATURE
               pMe->m_preListenFlag = FALSE;
               if(pMe->m_pIRingerMgr) //close the playing ringer
               {
                  (void)IRINGERMGR_Stop(pMe->m_pIRingerMgr);
               }
            #endif

               #ifdef AEE_STATIC
               if(APP_STATE_NEWGAME == pMe->m_eAppState)
                  SetSystemKeyBeepVol(pMe,TRUE);
               #endif //AEE_STATIC

            case APP_STATE_HELP:
         #if defined(AEE_STATIC)
         #else
            case APP_STATE_ABOUT:
         #endif
            case APP_STATE_SELECT_CHESS_MODE:
            case APP_STATE_SELECT_CHESS_LEVEL:
            case APP_STATE_HERO_SCORE:
               GmFive_DisplayMainScreen(pMe);
               //pMe->m_eAppState = APP_STATE_MAIN_MENU;
               return TRUE;

            case APP_STATE_MAIN_MENU:
               (void)ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);
               return TRUE;

            default:
               break;
         }
         break;

      case AVK_UP:
         if(pMe->m_eAppState == APP_STATE_DISP_INFO)
            return TRUE;
      case AVK_2:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_NEWGAME:
               if((pMe->m_curManPiece.row >= 0)&&(!pMe->m_gameOverFlag)&&(pMe->m_chess_object.who_go == MAN))
               {
                  pMe->m_lastManPiece.row = pMe->m_curManPiece.row;
                  pMe->m_lastManPiece.col = pMe->m_curManPiece.col;
                  pMe->m_curManPiece.row --;
                  DrawCurSquare(pMe);
               }
               return TRUE;

            case APP_STATE_HELP:
               if(wParam != AVK_UP )
                  return TRUE;
               break;

            default:
               break;
         }
         break;

      case AVK_DOWN:
         if(pMe->m_eAppState == APP_STATE_DISP_INFO)
            return TRUE;
      case AVK_8:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_NEWGAME:
            {
               if((pMe->m_curManPiece.row < (CHESS_LEN_Y - 2))&&
                  (!pMe->m_gameOverFlag)&&
                  (pMe->m_chess_object.who_go == MAN))
               {
                  pMe->m_lastManPiece.row = pMe->m_curManPiece.row;
                  pMe->m_lastManPiece.col = pMe->m_curManPiece.col;
                  pMe->m_curManPiece.row ++;
                  DrawCurSquare(pMe);
               }
               return TRUE;
            }

            case APP_STATE_HELP:
               if(wParam != AVK_DOWN )
                  return TRUE;
               break;

            default:
               break;
         }
         break;

      case AVK_LEFT:
      case AVK_4:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_NEWGAME:
               if((pMe->m_curManPiece.col >= 0)&&(!pMe->m_gameOverFlag)&&
                  ((pMe->m_chess_object.who_go == MAN)))
               {
                  pMe->m_lastManPiece.row = pMe->m_curManPiece.row;
                  pMe->m_lastManPiece.col = pMe->m_curManPiece.col;
                  pMe->m_curManPiece.col --;
                  DrawCurSquare(pMe);
               }
               return TRUE;

            case APP_STATE_HELP:
               if(wParam != AVK_LEFT )
                  return TRUE;
               break;

            default:
               break;
         }
         break;

      case AVK_RIGHT:
      case AVK_6:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_NEWGAME:
            {
               if((pMe->m_curManPiece.col < (CHESS_LEN_X - 2))&&(!pMe->m_gameOverFlag)&&
                  ((pMe->m_chess_object.who_go == MAN)))
               {
                  pMe->m_lastManPiece.row = pMe->m_curManPiece.row;
                  pMe->m_lastManPiece.col = pMe->m_curManPiece.col;
                  pMe->m_curManPiece.col ++;
                  DrawCurSquare(pMe);
               }
               return TRUE;
            }

            case APP_STATE_HELP:
               if(wParam != AVK_RIGHT )
                  return TRUE;
               break;

            default:
               break;
         }
         break;

      case AVK_1:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_NEWGAME:
            {
               if((pMe->m_curManPiece.col >=0)&&(!pMe->m_gameOverFlag)&&
                  (pMe->m_curManPiece.row >=0)&&
                  ((pMe->m_chess_object.who_go == MAN)))
               {
                  pMe->m_lastManPiece.row = pMe->m_curManPiece.row;
                  pMe->m_lastManPiece.col = pMe->m_curManPiece.col;
                  pMe->m_curManPiece.col --;
                  pMe->m_curManPiece.row --;
                  DrawCurSquare(pMe);
               }
               return TRUE;
            }

            case APP_STATE_HELP:
               return TRUE;

            default:
               break;
         }
         break;

      case AVK_9:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_NEWGAME:
            {
               if((pMe->m_curManPiece.col < (CHESS_LEN_X - 2))&&(!pMe->m_gameOverFlag)&&
                  (pMe->m_curManPiece.row < (CHESS_LEN_Y - 2))&&
                  ((pMe->m_chess_object.who_go == MAN)))
               {
                  pMe->m_lastManPiece.row = pMe->m_curManPiece.row;
                  pMe->m_lastManPiece.col = pMe->m_curManPiece.col;
                  pMe->m_curManPiece.col ++;
                  pMe->m_curManPiece.row ++;
                  DrawCurSquare(pMe);
               }
               return TRUE;
            }

            case APP_STATE_HELP:
               return TRUE;

            default:
               break;
         }
         break;

      case AVK_3:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_NEWGAME:
            {
               if((pMe->m_curManPiece.col < (CHESS_LEN_X - 2))&&(!pMe->m_gameOverFlag)&&
                  (pMe->m_curManPiece.row >=0)&&
                  ((pMe->m_chess_object.who_go == MAN)))
               {
                  pMe->m_lastManPiece.row = pMe->m_curManPiece.row;
                  pMe->m_lastManPiece.col = pMe->m_curManPiece.col;
                  pMe->m_curManPiece.col ++;
                  pMe->m_curManPiece.row --;
                  DrawCurSquare(pMe);
               }
               return TRUE;
            }

            case APP_STATE_HELP:
               return TRUE;

            default:
               break;
         }
         break;

      case AVK_7:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_NEWGAME:
            {
               if((pMe->m_curManPiece.row < (CHESS_LEN_Y - 2))&&(!pMe->m_gameOverFlag)&&
                  (pMe->m_curManPiece.col >=0)&&((pMe->m_chess_object.who_go == MAN)))
               {
                  pMe->m_lastManPiece.row = pMe->m_curManPiece.row;
                  pMe->m_lastManPiece.col = pMe->m_curManPiece.col;
                  pMe->m_curManPiece.col --;
                  pMe->m_curManPiece.row ++;
                  DrawCurSquare(pMe);
               }
               return TRUE;
            }

            case APP_STATE_HELP:
               return TRUE;

            default:
               break;
         }
         break;

      case AVK_F2://only deal event of prelisten ringer
         switch( pMe->m_eAppState )
         {
            #ifdef SOUND_FEATURE
            case APP_STATE_SELECT_RINGER://prelisten the ringer
               if(pMe->m_preListenFlag)
               {
                  pMe->m_preListenFlag = FALSE;
              #if defined(AEE_STATIC)                
                  DRAW_PROMPT_BAR(BTBAR_SELECT_PLAY_BACK);
              #else   
                  RedrawSoftKeyPrompt(pMe,IDS_RINGER_SELECT,IDS_PRE_LISTEN_RINGER,IDS_MENU_RETURN);
              #endif    
                  (void)IRINGERMGR_Stop(pMe->m_pIRingerMgr);
               }
               else
               {
                  pMe->m_preListenFlag = TRUE;
              #if defined(AEE_STATIC)                   
                  DRAW_PROMPT_BAR(BTBAR_SELECT_STOP_BACK);
              #else
                  RedrawSoftKeyPrompt(pMe,IDS_RINGER_SELECT,IDS_STOP_RINGER,IDS_MENU_RETURN);
              #endif                  
                  PreListenRinger(pMe);
               }
               return TRUE;
            #endif

            default:
               break;
         }
         break;

      case AVK_STAR:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_NEWGAME:
               if(pMe->m_chess_object.put_chess_counter > 0)
               {
                  pMe->m_tempPiece.col = -1;
                  pMe->m_tempPiece.row = -1;
                  (void)ISHELL_CancelTimer(pMe->a.m_pIShell,
                                          (PFNNOTIFY)MachineDrawDifColrPiece,
                                          pMe);
                  if(MoveFromRepentQueue(pMe))
                  {
                     if(pMe->m_manTotalPiece > 0)
                        pMe->m_manTotalPiece --;                  
                  }
               }
               return TRUE;

            default:
               break;
         }
         break;

      case AVK_POUND:
      #ifdef SOUND_FEATURE
          if(pMe->m_pIRingerMgr&&(pMe->m_eAppState == APP_STATE_NEWGAME)&&(pMe->m_ringerSelItemID != 0))
          { 
            if(pMe->m_toneFlag == TRUE)
            {
	            if(pMe->m_soundMode == IDS_SOUND_CLOSE_MODE)
	            {
	               char szFile[MAX_RING_LEN] = {'\0'};

	               if(pMe->m_playFile[0] != '\0')
	               {
	                  if((pMe->m_ringerSelItemID >= pMe->m_dloadRingerID)&&(pMe->m_dloadRingerID != 0))
	                  {
	                     STRCPY(szFile, DOWNLOAD_RIN_DIR);
	                  }
	                  else
	                  {
	                     STRCPY(szFile, GMFIVE_RING_DIR);
	                  }
	                  STRCAT(szFile, DIRECTORY_STR);
	                  STRCAT(szFile, pMe->m_playFile);
	                  #if defined(AEE_STATIC)
	                  if(pMe->m_RingVol != OEMSOUND_MUTE_VOL)//TCL wangbin040817 added
	                 #endif
	                  {
	                     (void)IRINGERMGR_Stop(pMe->m_promptVoice);
	                     (void)IRINGERMGR_PlayFile(pMe->m_pIRingerMgr,szFile,1000);
	                  }
	                  pMe->m_soundMode = IDS_SOUND_OPEN_MODE;
	                  #if defined(AEE_STATIC)
	                  SetSystemKeyBeepVol(pMe,FALSE);
	                  #endif  //AEE_STATIC
	                  MSG_FATAL("1135m_toneflag == %d",pMe->m_toneFlag,0,0);
	                  MSG_FATAL("1136m_soundmode == %d",pMe->m_soundMode,0,0);	  
	               }//if
	            }//if
	            else
	            {
	               (void)IRINGERMGR_Stop(pMe->m_pIRingerMgr);
	               pMe->m_soundMode = IDS_SOUND_CLOSE_MODE;
	               #if defined(AEE_STATIC)
	               SetSystemKeyBeepVol(pMe,TRUE);
	               #endif  //AEE_STATIC
	               MSG_FATAL("1145m_toneflag == %d",pMe->m_toneFlag,0,0);
	               MSG_FATAL("1146m_soundmode == %d",pMe->m_soundMode,0,0);
	            }
            }
            else
	     {
	           (void)IRINGERMGR_Stop(pMe->m_pIRingerMgr);
	           //pMe->m_soundMode = IDS_SOUND_CLOSE_MODE;
	          // #if defined(AEE_STATIC)
	           //SetSystemKeyBeepVol(pMe,TRUE);
	          // #endif  //AEE_STATIC
	           if(pMe->m_soundMode == IDS_SOUND_CLOSE_MODE)//因为在keyprocess里会吧pme->m_soundmode状态翻转
	           {                                                                                //所以在这儿翻转一下以此来抵消
	           pMe->m_soundMode = IDS_SOUND_OPEN_MODE;
	           }//if you want to shut off the music please set it IDA_SOUND_OPEN_MODE
	           else
	           {
	           pMe->m_soundMode = IDS_SOUND_CLOSE_MODE;
	           }
	           MSG_FATAL("1156m_toneflag == %d",pMe->m_toneFlag,0,0);
	           MSG_FATAL("1157m_soundmode == %d",pMe->m_soundMode,0,0);
	            }
         }//if(pMe->m_pIRingerMgr)
         else
         {
             if(pMe->m_soundMode == IDS_SOUND_CLOSE_MODE)//因为在keyprocess里会吧pme->m_soundmode状态翻转
             {                                                                                //所以在这儿翻转一下以此来抵消
                 pMe->m_soundMode = IDS_SOUND_OPEN_MODE;
             }//if you want to shut off the music please set it IDA_SOUND_OPEN_MODE
             else
             {
                 pMe->m_soundMode = IDS_SOUND_CLOSE_MODE;
             }
             MSG_FATAL("1170m_toneflag == %d",pMe->m_toneFlag,0,0);
             MSG_FATAL("1171m_soundmode == %d",pMe->m_soundMode,0,0);	 
         }
      #endif
      break;

      default:
      break;
  }
            
   return FALSE;

}//KeyProcess()

///////////////////////////////////////////////
//Pen Process
//////////////////////////////////////////////
static boolean PenProcess(GmFive *pMe, uint32 dwParam)
{
    if(pMe == NULL)
        return FALSE;
    switch( pMe->m_eAppState )
    {
        case APP_STATE_NEWGAME:
            {
                int16 row = CHESS_LEN_Y - 2;
                int16 col = CHESS_LEN_X - 2;

                int16 i = 0;
                int16 j = 0;

                uint16 x = AEE_GET_X(dwParam);
                uint16 y = AEE_GET_Y(dwParam);
                //added by tcl wgs 041217
				if(y<(int16)(Y_OFF + (GRID_HEIGHT*14) + (GRID_HEIGHT/2)))
				{
					for (i = 0;i<CHESS_LEN_Y;i++)
					{
						if ( y <= (int16)(Y_OFF + (GRID_HEIGHT*i) + (GRID_HEIGHT/2)) )
						{
							row = i - 1;
							break;
						}
					}
					for (j = 0;j<CHESS_LEN_X;j++)
					{
						if ( x <= (int16)(X_OFF + (GRID_WIDTH*j) + (GRID_WIDTH/2)) )
						{
							col = j - 1;
							break;
						}
					}

					pMe->m_lastManPiece.row = pMe->m_curManPiece.row;
					pMe->m_lastManPiece.col = pMe->m_curManPiece.col;
					pMe->m_curManPiece.col = col;
					pMe->m_curManPiece.row = row;
					ClearHistorySquare(pMe,FALSE);//tclWGS 050111 added 
					ManGo(pMe);
				}
				else
				{
                    if(x>(int16)(X_OFF + (GRID_WIDTH*5))&&x<(int16)(X_OFF + (GRID_WIDTH*7)))
					{
						KeyProcess( pMe,AVK_CLR);
					}
					if(x>(int16)(X_OFF + (GRID_WIDTH*21/2))&&x<(int16)(X_OFF + (GRID_WIDTH*25/2)))
					{
						if(pMe->m_toneFlag)
						{
							pMe->m_toneFlag = FALSE;
							DrawSoundButton(pMe);
							if(pMe->m_soundMode == IDS_SOUND_CLOSE_MODE)//因为在keyprocess里会吧pme->m_soundmode状态翻转
							{                                                                                //所以在这儿翻转一下以此来抵消
							    pMe->m_soundMode = IDS_SOUND_OPEN_MODE;
							}//if you want to shut off the music please set it IDA_SOUND_OPEN_MODE
							else
							{
							    pMe->m_soundMode = IDS_SOUND_CLOSE_MODE;
							}
							
                                                 //pMe->m_soundMode = IDS_SOUND_OPEN_MODE;
                                                 MSG_FATAL("1244m_toneflag == %d",pMe->m_toneFlag,0,0);
                                                 MSG_FATAL("1245m_soundmode == %d",pMe->m_soundMode,0,0);
							KeyProcess(pMe,AVK_POUND);
						}
						else
						{
							pMe->m_toneFlag = TRUE;
							DrawSoundButton(pMe);
                            
							if(pMe->m_soundMode == IDS_SOUND_CLOSE_MODE)//因为在keyprocess里会吧pme->m_soundmode状态翻转
							{                                                                                //所以在这儿翻转一下以此来抵消
							    pMe->m_soundMode = IDS_SOUND_OPEN_MODE;
							}//if you want to shut off the music please set it IDA_SOUND_OPEN_MODE
							else
							{
							    pMe->m_soundMode = IDS_SOUND_CLOSE_MODE;
							}
							MSG_FATAL("m_toneflag == %d",pMe->m_toneFlag,0,0);
                                                 MSG_FATAL("m_soundmode == %d",pMe->m_soundMode,0,0);
							KeyProcess(pMe,AVK_POUND);             //find the answer in KeyProcess
						}
					}
				}// end added by tcl wgs 041217
            }
            return TRUE;
        default:
            break;
    }
    return  FALSE;
}//PenProcess()

/*===========================================================================
   callback of draw background bmp
===========================================================================*/
#ifndef AEE_STATIC 

static void DrawTopTitleProc(GmFive * pMe)
{
    AECHAR   szTitle[64];
    AECHAR*  pBuf;
    uint16   nMaxUnicode = 7;

    if(pMe == NULL) return ;

    MEMSET(szTitle,0,sizeof(szTitle));
    pBuf   = pMe->m_szwinTitle;
    pBuf += pMe->m_topTitleLen;

    if (pMe->m_AllASCIICharFlg)
    {
        nMaxUnicode = 17;
    } 	
    
    WSTRNCOPYN(szTitle,sizeof(szTitle)/sizeof(AECHAR),pBuf,nMaxUnicode);
    if(pMe->m_pTopBGImage)
    {
        IIMAGE_Draw(pMe->m_pTopBGImage,0,0);
    }
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szTitle,  -1,  2, 1, NULL, IDF_TEXT_TRANSPARENT);
    if(pMe->m_topTitleLen > (int16)(WSTRLEN(pMe->m_szwinTitle) - 1))
    {
        pMe->m_topTitleLen = 0;
    }
    else
    {
        pMe->m_topTitleLen += 1;
    }

    ISHELL_SetTimer(pMe->a.m_pIShell,TITLE_FILE_NAME_AUTO,(PFNNOTIFY)DrawTopTitleProc,pMe);

    IDISPLAY_Update(pMe->a.m_pIDisplay);
}//DrawTopTitleProc()
#endif //AEE_STATIC

/*===========================================================================
   draw background bmp of screen title
===========================================================================*/
static uint16 DrawTopBackGroundBMP(GmFive * pMe,uint16 resID,AECHAR * pText)
{

#if defined(AEE_STATIC)    
    if(pMe ==  NULL) return 0;
    
    SetUniMenuConfig(pMe->m_pMenuCtl);
    (void)IMENUCTL_SetTitle ( pMe->m_pMenuCtl, GMFIVE_RES_FILE_LANG,  resID,  NULL );
    return 0;

#else
    AEEImageInfo  ImageInfo;
    AECHAR        szTitle[32];
    int16         len;
    
    if(pMe ==  NULL) return 0;
    
    if(pMe->m_pTopBGImage)
    {
        // Get image information
        IIMAGE_GetInfo( pMe->m_pTopBGImage, &ImageInfo );
        // Draw the image in the center of the screen
        IIMAGE_Draw(pMe->m_pTopBGImage,0,0);
    }
    if(resID)
    {
        ISHELL_LoadResString(pMe->a.m_pIShell, GMFIVE_RES_FILE_LANG, resID, szTitle, sizeof(szTitle));
    }
    else if(pText)
    {
        WSTRCPY(szTitle,pText);
    }
    
    pMe->m_AllASCIICharFlg  = FALSE;

    if(!JudgeHaveNoneASCIIChar(szTitle,NULL))
    {
        pMe->m_AllASCIICharFlg = TRUE;
    }            
    
    len = IDISPLAY_MeasureText(pMe->a.m_pIDisplay,AEE_FONT_NORMAL,szTitle);        
    if (len < 117)
    {
        ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)(DrawTopTitleProc), pMe);
        (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szTitle,  -1,  2, 1, NULL, IDF_TEXT_TRANSPARENT);
    }
    else
    {
        pMe->m_topTitleLen = 0;
        MEMSET(pMe->m_szwinTitle,0,sizeof(pMe->m_szwinTitle));
        (void)WSTRCPY(pMe->m_szwinTitle,szTitle);
        DrawTopTitleProc(pMe);
    }

    return ImageInfo.cy;
    
#endif  //AEE_STATIC   
}


///////////////////////////////////////////////
//display main menu screen
//////////////////////////////////////////////
static void GmFive_DisplayMainScreen(GmFive * pMe)
{
    AEERect   rectMenu;
    int      dx = pMe->m_cxScreen;
    int      dy = pMe->m_cyScreen;
    IDisplay *pd =  pMe->a.m_pIDisplay;
    int16     startPoint;

    if (pMe == NULL) return;
    MSG_FATAL("GmFive_DisplayMainScreen......",0,0,0);
    GmFive_Reset(pMe);
    IDISPLAY_ClearScreen(pd);
	MSG_FATAL("GmFive_DisplayMainScreen......000000",0,0,0);
    startPoint =  DrawTopBackGroundBMP(pMe,IDS_APP_TITLE,NULL);
    dy -= (GMFIVE_SOTFKEY_CY  + startPoint) ;//for softkey prompt
    SETAEERECT(&rectMenu, 1, startPoint, (dx - 2), dy );
    MSG_FATAL("GmFive_DisplayMainScreen......111111",0,0,0);
    (void)Brew_SetMenuAttr(pMe->m_pMenuCtl,AEECLSID_MENUCTL,&rectMenu,MP_MAXSOFTKEYITEMS);//set menu attible
    (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,IDS_GAME_NEW,  NULL, IDB_GAME_NEW,IDS_GAME_NEW,0); //game new

    if(pMe->m_gameStartFlag)
    {
        (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,IDS_CONTINUE_GAME,  NULL, IDB_GAME_NEW,IDS_CONTINUE_GAME,0); //continue game
    }

    (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,IDS_PLAY_CHESS_MODE,  NULL, IDB_CHESS_MODE,IDS_PLAY_CHESS_MODE,0); //play chess mode
    (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,IDS_CHESS_LEVLE,  NULL, IDB_GAME_LEVEL,IDS_CHESS_LEVLE,0);//chess level
#if 0
    if(pMe->m_ringerSelItemID != 0) // 没有选择背景铃声将不出现声音设置菜单
        (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,IDS_SOUND_SET,  NULL, IDB_SOUND_SET,IDS_SOUND_SET,0);  //sound set for open or close
    (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,IDS_BACKRINGER_SELECT,  NULL, IDB_RINGER_VIEW,IDS_BACKRINGER_SELECT,0);  //set background sound
#endif
    (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,IDS_HERO_SCORE,  NULL, IDB_HERO_SCORE,IDS_HERO_SCORE,0);  //hero score
    (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,IDS_GAME_HELP, NULL, IDB_GAME_HELP,IDS_GAME_HELP,0);    //game help
#ifndef AEE_STATIC
    (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,IDS_GAME_ABOUT,NULL, IDB_GAME_ABOUT,IDS_GAME_ABOUT,0);  //game about
#endif
    (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,IDS_GAME_EXIT, NULL, IDB_GAME_EXIT,IDS_GAME_EXIT,0);    //game exit
    IMENUCTL_SetSel(pMe->m_pMenuCtl,pMe->m_SelMenuItem);
    IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
    IMENUCTL_Redraw(pMe->m_pMenuCtl);    
	MSG_FATAL("GmFive_DisplayMainScreen......22222",0,0,0);
    pMe->m_eAppState = APP_STATE_MAIN_MENU;
#if defined(AEE_STATIC) 
    DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
#else
    //add softkey prompt
    pMe->m_softkeyPrompt.leftItemID  = IDS_RINGER_SELECT;
    pMe->m_softkeyPrompt.middleItemID = NULL;
    pMe->m_softkeyPrompt.rightItemID  = IDS_MENU_RETURN;
    GmFive_AddSoftkeyPrompt(pMe,&pMe->m_softkeyPrompt);
	
    IDISPLAY_Update(pd);
#endif   
	MSG_FATAL("GmFive_DisplayMainScreen......3333333",0,0,0);
    IDISPLAY_Update(pd);

}//GmFive_DisplayMainScreen()


///////////////////////////////////////////////
//select chess level screen
//////////////////////////////////////////////
static void SelectChessLevelScreen(GmFive *pMe)
{
   IDisplay *pd =  pMe->a.m_pIDisplay;
   uint16    wImageID = NULL;
   uint16    itemID;
   uint16    i;
   uint16    selItemID = IDS_GAME_LEVEL_1;

   if (pMe == NULL) return ;      

   GmFive_Reset(pMe);
   SetPopMenuAttr(pMe,IDS_CHESS_LEVLE,&pMe->m_frameRect,MP_MAXSOFTKEYITEMS);

   for(i = 0;i<MAX_LEVEL;i++)
   {
      itemID = i+ IDS_GAME_LEVEL_1;
      if(pMe->m_global_game_level == (i+INIT_LEVEL))
      {
         wImageID = IDB_GAME_RADIO_SEL;
         selItemID = itemID;
      }
      else
      {
         wImageID = IDB_GAME_RADIO_UNSEL;
      }
      (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,itemID,  NULL, wImageID,itemID,0);
   }
   
   IMENUCTL_SetSel(pMe->m_pMenuCtl,selItemID);
   IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
   IMENUCTL_Redraw(pMe->m_pMenuCtl);
   IDISPLAY_Update(pd);

}//SelectChessLevelScreen()


///////////////////////////////////////////////
//select chess mode screen
//////////////////////////////////////////////
static void SelectPlayChessModeScreen(GmFive *pMe)
{
   IDisplay *pd =  pMe->a.m_pIDisplay;
   uint16    wImageID = NULL;
   uint16    itemID;
   uint16    i;
   uint16    selItemID = IDS_GAME_GO_MODE_1;

   if (pMe == NULL)    return ;   

   GmFive_Reset(pMe);
   SetPopMenuAttr(pMe,IDS_PLAY_CHESS_MODE,&pMe->m_frameRect,MP_MAXSOFTKEYITEMS);
   for(i = 0;i < MAX_CHESS_MODE;i++)
   {
      itemID = i+ IDS_GAME_GO_MODE_1;
      if(pMe->m_global_who_go == (i+MAN))
      {
         wImageID = IDB_GAME_RADIO_SEL;
         selItemID = itemID;
      }
      else
      {
         wImageID = IDB_GAME_RADIO_UNSEL;
      }
      (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,itemID,  NULL, wImageID,itemID,0); //machine play first
   }
   
   IMENUCTL_SetSel(pMe->m_pMenuCtl,selItemID);
   IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
   IMENUCTL_Redraw(pMe->m_pMenuCtl);
   IDISPLAY_Update(pd);

}//SelectPlayChessModeScreen()

/*===========================================================================
   This function adds control reset
===========================================================================*/
static void GmFive_Reset(GmFive *pMe)
{
    if(pMe->m_pSoftKeyCtl)
    {
        IMENUCTL_SetActive(pMe->m_pSoftKeyCtl,FALSE);
        IMENUCTL_Reset(pMe->m_pSoftKeyCtl);
    }

    if(pMe->m_pMenuCtl)
    {
        IMENUCTL_SetActive(pMe->m_pMenuCtl,FALSE);
        IMENUCTL_Reset(pMe->m_pMenuCtl);    
    }

    if(pMe->m_pStaticCtrl)
    {
        ISTATIC_SetActive(pMe->m_pStaticCtrl,FALSE);
        ISTATIC_Reset(pMe->m_pStaticCtrl);
    }

}//GmFive_Reset()

///////////////////////////////////////////////
//init chess information
//////////////////////////////////////////////
static void InitChess(GmFive *pMe)
{
   int16 i, j;
   CHESS_OBJECT *chess_object = (CHESS_OBJECT *)&pMe->m_chess_object;

   for(i = 0; i < CHESS_LEN_Y; i++)
   {
      for(j = 0; j < CHESS_LEN_X; j++)
      {
         chess_object->map[i][j] = 0; // Chess's map clear.
         chess_object->attack_value[i][j] = 0;
         chess_object->defence_value[i][j] = 0;
      }
   }
   chess_object->particular = 0;
   chess_object->level = pMe->m_global_game_level;//global_game_level;   // is must 1 ~ 4 stadge.
   chess_object->begin_flag = pMe->m_global_who_go;  // is must MAN or MACHINE when initialize.
   chess_object->who_go  = pMe->m_global_who_go;      // is must MAN or MACHINE when initialize.
   chess_object->who_win = INIT_WIN;
   pMe->m_old_who_go     = MAN;
   chess_object->put_chess_counter = 0;
   InitRepentQueue(chess_object);

   pMe->m_curManPiece.row  = START_ROW;
   pMe->m_curManPiece.col  = START_COL;
   pMe->m_lastManPiece.row = pMe->m_curManPiece.row;
   pMe->m_lastManPiece.col = pMe->m_curManPiece.col;

   pMe->m_tempPiece.col = -1;
   pMe->m_tempPiece.row = -1;
   pMe->m_pieceSplashFlg  = TRUE;
   pMe->m_squareSplashFlg = TRUE;

}//InitChess()

///////////////////////////////////////////////
//init repent queue
//////////////////////////////////////////////

static void InitRepentQueue(CHESS_OBJECT *chess_object)
{
   chess_object->repent_queue.total = 0;
   chess_object->repent_queue.head_point = 0;
   chess_object->repent_queue.tail_point = 0;
   chess_object->repent_queue.tail_ptr_var = -1;
}//InitRepentQueue()


///////////////////////////////////////////////
//draw board of chess
//////////////////////////////////////////////
static void GmFive_DrawBoard(GmFive *pMe)
{
   uint16   i = 0,j = 0;
   AEELine  pLine;

   IGRAPHICS_SetColor(pMe->m_pGraphics,GRID_R_COLOR,GRID_G_COLOR,GRID_B_COLOR,0);
   pLine.sx = X_OFF;
   pLine.ex = DIS_END_X;
   for (i = 0;i<CHESS_LEN_Y;i++)
   {
      pLine.sy = Y_OFF+(GRID_HEIGHT*i);
      pLine.ey = pLine.sy;
      (void)IGRAPHICS_DrawLine(pMe->m_pGraphics,&pLine);
   }
   pLine.sy = Y_OFF;
   pLine.ey = DIS_END_Y;
   for (j = 0;j<CHESS_LEN_X;j++)
   {
      pLine.sx = X_OFF+(GRID_WIDTH*j);
      pLine.ex = pLine.sx;
      (void)IGRAPHICS_DrawLine(pMe->m_pGraphics,&pLine);
   }
   IGRAPHICS_Update(pMe->m_pGraphics);
}//GmFive_DrawBoard()

///////////////////////////////////////////////
//new game start
//////////////////////////////////////////////
static void GmFive_NewGameStart(GmFive * pMe)
{
   if(pMe == NULL)
      return;

   GmFive_Reset(pMe);
   IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );

   pMe->m_gameOverFlag  = FALSE;
   pMe->m_gameStartFlag = TRUE;
   pMe->m_forbidPieceFlg= FALSE;
   pMe->m_manTotalPiece = 0;
   pMe->m_toneFlag = TRUE;

   (void)IGRAPHICS_SetPaintMode( pMe->m_pGraphics, AEE_PAINT_COPY );
   DrawScreenBackground(pMe);
   GmFive_DrawBoard(pMe);
   InitChess(pMe);
   //added by tcl wgs 041217
   DrawExitButton(pMe);
   DrawSoundButton(pMe);
   //added by tcl wgs 041217
#ifdef  SOUND_FEATURE
   if(pMe->m_pIRingerMgr)
   {
      char szFile[MAX_RING_LEN] = {'\0'};

      if(pMe->m_playFile[0] != '\0')
      {
         if((pMe->m_ringerSelItemID >= pMe->m_dloadRingerID)&&(pMe->m_dloadRingerID != 0))
         {
            STRCPY(szFile, DOWNLOAD_RIN_DIR);
         }
         else
         {
            STRCPY(szFile, GMFIVE_RING_DIR);
         }
         STRCAT(szFile, DIRECTORY_STR);
         STRCAT(szFile, pMe->m_playFile);
         //TCL wangbin040817 mod
     #if defined(AEE_STATIC)
         if(pMe->m_soundMode == IDS_SOUND_OPEN_MODE && pMe->m_RingVol != OEMSOUND_MUTE_VOL)
     #else
         if(pMe->m_soundMode == IDS_SOUND_OPEN_MODE)
     #endif
         {
            (void)IRINGERMGR_Stop(pMe->m_promptVoice);
            (void)IRINGERMGR_PlayFile(pMe->m_pIRingerMgr,szFile,1000);
         }
      }
   }

#ifdef AEE_STATIC
   SetSystemKeyBeepVol(pMe,FALSE);
#endif //AEE_STATIC
#endif //SOUND_FEATURE
   if(pMe->m_chess_object.who_go == MAN)
   {
      DrawCurSquare(pMe);
   }
   else if(pMe->m_chess_object.who_go == MACHINE)
   {
      (void)ISHELL_PostEvent(pMe->a.m_pIShell,
                             AEECLSID_GMFIVE,
                             EVT_COMMAND,
                             MACHINE_PLAY_CHESS,
                             0);
   }


   IDISPLAY_Update( pMe->a.m_pIDisplay );

}//GmFive_NewGameStart()

///////////////////////////////////////////////
//draw one chess piece
//////////////////////////////////////////////
static void DrawPiece(GmFive * pMe,int16 cord_x, int16 cord_y,int16 color)
{
   AEECircle   tempCircle;
   AEEPoint    pPoint;

   if(!IGRAPHICS_GetFillMode(pMe->m_pGraphics))
   {
      (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics,TRUE);
   }

   if(color == WHITE_CHESS)
   {
      IGRAPHICS_SetFillColor(pMe->m_pGraphics,255,255,255,0);
      IGRAPHICS_SetColor(pMe->m_pGraphics,255,255,255,0);

   }
   else if(color == BLACK_CHESS)
   {
      IGRAPHICS_SetFillColor(pMe->m_pGraphics,0,0,0,0);
      IGRAPHICS_SetColor(pMe->m_pGraphics,0,0,0,0);
   }
   else
   {
      IGRAPHICS_SetFillColor(pMe->m_pGraphics,SPLASH_R_COLOR,SPLASH_G_COLOR,SPLASH_B_COLOR,0);
      IGRAPHICS_SetColor(pMe->m_pGraphics,SPLASH_R_COLOR,SPLASH_G_COLOR,SPLASH_B_COLOR,0);
   }
   tempCircle.cx = cord_x;
   tempCircle.cy = cord_y;
   tempCircle.r  = PIECE_WIDTH/2;
   (void)IGRAPHICS_DrawCircle(pMe->m_pGraphics,&tempCircle);

   //draw point of chess
   if(color == WHITE_CHESS)
   {
      IGRAPHICS_SetColor(pMe->m_pGraphics,0,0,0,0);
      pPoint.x = cord_x + 2;
      pPoint.y = cord_y + 2;
      IGRAPHICS_DrawPoint(pMe->m_pGraphics,&pPoint);
   }
   else if(color == BLACK_CHESS)
   {
      IGRAPHICS_SetColor(pMe->m_pGraphics,255,255,255,0);
      pPoint.x = cord_x - 2;
      pPoint.y = cord_y - 2;
      IGRAPHICS_DrawPoint(pMe->m_pGraphics,&pPoint);
   }
   IGRAPHICS_Update(pMe->m_pGraphics);

}//DrawPiece()

//////////////////////////////////////////////////
//chess game is over
//////////////////////////////////////////////////
static boolean ChessPlayOver(GmFive * pMe)
{

   CHESS_OBJECT *chess_object = &pMe->m_chess_object;


#ifndef ONLY_MAN_CHESS_DEBUG
   if(pMe->m_old_who_go != chess_object->who_go)
#else
   if(pMe->m_old_who_go == chess_object->who_go)
#endif
   {
      if(JudgeWin(chess_object)||JudgeThreeAndThreeRule(pMe))
      {
        
         (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) MachineDrawDifColrPiece, pMe);
         pMe->m_splashNum = 4;
         pMe->m_winPieceSplashFlg = FALSE;      
         ISHELL_SetTimer(pMe->a.m_pIShell,
                         GAME_OVER_DISP_TIMER,
                         (PFNNOTIFY) DrawGameOverScreen,
                         pMe);      
         pMe->m_gameOverFlag = TRUE;
         pMe->m_gameStartFlag = FALSE;
         pMe->m_old_who_go = MAN;
         InitRepentQueue(chess_object);      

      #if defined(AEE_STATIC)
         SetSystemKeyBeepVol(pMe,TRUE);
      #endif  //AEE_STATIC              
         return TRUE;
      }
      pMe->m_old_who_go = chess_object->who_go;
   }
   return FALSE;
}//ChessPlayOver()

///////////////////////////////////////////////
//man play chess
//////////////////////////////////////////////
static char ManGo(GmFive * pMe)
{
    int16 col,row;
    char ret = 0;
    
    CHESS_OBJECT *chess_object = &pMe->m_chess_object;
    col = pMe->m_curManPiece.col+1;
    row = pMe->m_curManPiece.row+1;

    if(chess_object->map[row][col] == 0) // the point is not MAN or MACHINE
    {
        RestartRepentQueue(chess_object);
        DrawPiece(pMe,(int16)(col*GRID_WIDTH+X_OFF), (int16)(row*GRID_HEIGHT+Y_OFF),BLACK_CHESS);
        chess_object->map[row][col] = MAN;
        AddToRepentQueue(chess_object, col, row);
        
     #ifndef ONLY_MAN_CHESS_DEBUG
        chess_object->who_go = MACHINE;
     #else
        chess_object->who_go = MAN;
     #endif
        ret = 1;
        pMe->m_manTotalPiece++;     
        (void)ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)SplashCurSquare,pMe);            
     #ifndef ONLY_MAN_CHESS_DEBUG
        ClearHistorySquare(pMe,TRUE);
     #endif
        if(ChessPlayOver(pMe))
        {
           return ret;
        }
        
        chess_object->put_chess_counter++;
     #ifndef ONLY_MAN_CHESS_DEBUG
        (void)ISHELL_CancelTimer(pMe->a.m_pIShell,
                                (PFNNOTIFY) MachineDrawDifColrPiece,
                                pMe);
     #endif
        //将机器刚下的棋颜色复原
        if((pMe->m_tempPiece.col != -1)||(pMe->m_tempPiece.row != -1))
        {
           DrawPiece(pMe,(int16)(pMe->m_tempPiece.col*GRID_WIDTH +X_OFF), (int16)(pMe->m_tempPiece.row*GRID_HEIGHT+Y_OFF), WHITE_CHESS);
        }
        
     #ifndef ONLY_MAN_CHESS_DEBUG
        (void)ISHELL_PostEvent(pMe->a.m_pIShell,
                               AEECLSID_GMFIVE,
                               EVT_COMMAND,
                               MACHINE_PLAY_CHESS,
                               0);
     #endif
    
    }//end of if(chess_object->map[row][col] == 0)
	//added by tcl wgs 041217
    if(pMe->m_toneFlag)
	{
	    MSG_FATAL("1849m_toneflag == %d",pMe->m_toneFlag,0,0);
	    MSG_FATAL("1850m_soundmode == %d",pMe->m_soundMode,0,0);
	    if(IDS_SOUND_CLOSE_MODE == pMe->m_soundMode)
	    {
            PlayerSound(pMe->m_promptVoice,DROP_PIECE_MEDIA);  //player background music
        }
    }
    
	//end added by tcl wgs 041217
	return ret;   
}//ManGo()

///////////////////////////////////////////////
//get rand num
//////////////////////////////////////////////
static uint32 GmFive_GetRndNumber(uint32 max)
{
   uint32 rnd;

   //gain rand number
   GETRAND( (void *) &rnd, sizeof(rnd));

   return rnd % max;
}//GmFive_GetRndNumber()


///////////////////////////////////////////////
//FlashDrawPiece
//////////////////////////////////////////////
static void FlashDrawPiece(GmFive * pMe,char col, char row)
{
   pMe->m_tempPiece.row = row;
   pMe->m_tempPiece.col = col;
   MachineDrawDifColrPiece(pMe);
}//FlashDrawPiece()

///////////////////////////////////////////////
//wait machine play chess
//////////////////////////////////////////////
static void MachineDrawDifColrPiece(GmFive * pMe)
{
   int16 pos_x, pos_y;

   if((pMe->m_tempPiece.row == -1) ||(pMe->m_tempPiece.col == -1)) return ;
   pos_x = pMe->m_tempPiece.col*GRID_WIDTH +X_OFF;
   pos_y = pMe->m_tempPiece.row*GRID_HEIGHT+Y_OFF;

   if(pMe->m_pieceSplashFlg)
   {
      DrawPiece(pMe,pos_x, pos_y, OTHER_CHESS);
      pMe->m_pieceSplashFlg = FALSE;
   }
   else
   {
      DrawPiece(pMe,pos_x, pos_y, WHITE_CHESS);
      pMe->m_pieceSplashFlg = TRUE;
   }
   ISHELL_SetTimer(pMe->a.m_pIShell,
                  SPLASH_PIECE_TIMER,
                  (PFNNOTIFY) MachineDrawDifColrPiece,
                  pMe);

   IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,FALSE);
}//DrawDifferColorPiece()

///////////////////////////////////////////////
//machine play chess
//////////////////////////////////////////////
static void MachineGo(GmFive * pMe)
{
   typedef struct {int16 x; int16 y;} CHESS_POINT;

   int16 x=0, y=0, xx, yy, temp_x, temp_xx, temp_y, temp_yy;
   byte  put_status = 1; // this is normal.
   byte  who = MACHINE;
   int16 step, step_deep = 0;
   CHESS_POINT save_xy[DEEP+1];
   int16  many = 0;
   CHESS_POINT temp_xy[MANY];
   int16 randNum = 0;


   CHESS_OBJECT *chess_object = &pMe->m_chess_object;

   if( chess_object->begin_flag == MAN ) // if man first_go then...
   {
      if(chess_object->map[(CHESS_LEN_Y-1)/2][(CHESS_LEN_X-1)/2] != 0)
      {
         loop_again:
         randNum = (int16)GmFive_GetRndNumber(8);
         switch(randNum)
         {
            case 0:
               x = (CHESS_LEN_X-1)/2 - 1; y = (CHESS_LEN_Y-1)/2;
               if(chess_object->map[y][x] != 0) goto loop_again;/*lint !e801*/
               break;
            case 1:
               x = (CHESS_LEN_X-1)/2 - 1; y = (CHESS_LEN_Y-1)/2 - 1;
               if(chess_object->map[y][x] != 0) goto loop_again;/*lint !e801*/
               break;
            case 2:
               x = (CHESS_LEN_X-1)/2    ; y = (CHESS_LEN_Y-1)/2 - 1;
               if(chess_object->map[y][x] != 0) goto loop_again;/*lint !e801*/
               break;
            case 3:
               x = (CHESS_LEN_X-1)/2 + 1; y = (CHESS_LEN_Y-1)/2 - 1;
               if(chess_object->map[y][x] != 0) goto loop_again;/*lint !e801*/
               break;
            case 4:
               x = (CHESS_LEN_X-1)/2 + 1; y = (CHESS_LEN_Y-1)/2;
               if(chess_object->map[y][x] != 0) goto loop_again;/*lint !e801*/
               break;
            case 5:
               x = (CHESS_LEN_X-1)/2 + 1; y = (CHESS_LEN_Y-1)/2 + 1;
               if(chess_object->map[y][x] != 0) goto loop_again;/*lint !e801*/
               break;
            case 6:
               x = (CHESS_LEN_X-1)/2    ; y = (CHESS_LEN_Y-1)/2 + 1;
               if(chess_object->map[y][x] != 0) goto loop_again;/*lint !e801*/
               break;
            case 7:
               x = (CHESS_LEN_X-1)/2 - 1; y = (CHESS_LEN_Y-1)/2 + 1;
               if(chess_object->map[y][x] != 0) goto loop_again;/*lint !e801*/
               break;
            default:
               break;
         }
      }
      else
      {
         x = (CHESS_LEN_X-1)/2;
         y = (CHESS_LEN_Y-1)/2;
      }
      AddToRepentQueue(chess_object, x, y);
      chess_object->begin_flag = 0;
   }
   else if( chess_object->begin_flag == MACHINE) // if machin first_go then...
   {
      x = (CHESS_LEN_X-1)/2;
      y = (CHESS_LEN_Y-1)/2;
      chess_object->begin_flag = 0;
   }
   else // isn't first step then...
   {
     put_status = MachinePutChess(chess_object, (uint16 *)&x, (uint16 *)&y, (char)who);

     if(chess_object->level == 4)
     {
        temp_xy[many].x = temp_xy[many].y = 0;
        for(xx=0;xx<CHESS_LEN_X;xx++) for(yy=0;yy<CHESS_LEN_Y;yy++)
        {
           if( (put_status&0xf0) == 0 && chess_object->defence_value[yy][xx] >= THRESHOLD )
           {
              temp_xy[many].x = xx; temp_xy[many].y = yy;
              many++;
              if(many > MANY-1) break;
           }
        }

        if( many >= 1)
        {
            chess_object->map[y][x] = (char)who;         // assume machine goes this point.

            many = (int16)GmFive_GetRndNumber((int32)many);//Random(many);
            temp_xx = temp_xy[many].x;
            temp_yy = temp_xy[many].y;
            while(1)/*lint !e716*/
            {
               who = (who==MAN)? MACHINE : MAN;    // change player
               put_status = MachinePutChess(chess_object, (uint16 *)&temp_x, (uint16 *)&temp_y, (char)who);
               chess_object->map[temp_y][temp_x] = (char)who;         // man or machin
               save_xy[step_deep].x = temp_x;
               save_xy[step_deep].y = temp_y;

               if(put_status == 0x81)
               {
                  if(who == MAN) // if man will win, then...
                  {
                     chess_object->map[y][x] = 0;
                     x = temp_xx;
                     y = temp_yy;
                  }
                  for(step=0; step<=step_deep; step++)
                  {
                     chess_object->map[save_xy[step].y][save_xy[step].x] = 0;
                  }
                  break;
               }//if(put_status == 0x81)
               step_deep++;
               if(step_deep > DEEP || put_status == 0)
               {
                  for(step=0; step<step_deep; step++)
                     chess_object->map[save_xy[step].y][save_xy[step].x] = 0;
                  step_deep = 0;
                  break;
               }
            }//while(1)
        }//if( many >= 1)
     }//if(chess_object->level == 4)
     AddToRepentQueue(chess_object, x, y);
   }//else // isn't first step then...

   if(put_status == 0)  // all_no_win
   {
      chess_object->map[y][x] = MACHINE;
      chess_object->put_chess_counter = (CHESS_LEN_X)*(CHESS_LEN_Y)/2+1 ;
   }
   else
   {
      chess_object->map[y][x] = MACHINE;
      //add sound place
      FlashDrawPiece(pMe,(char)x, (char)y);   
   }
   chess_object->who_go = MAN;
   //return;
}//MachineGo()

///////////////////////////////////////////////
//ConnectSub
//////////////////////////////////////////////
static byte ConnectSub(CHESS_OBJECT *chess_object, byte left, byte right, uint16  board_pattern)
{
   static const byte connect_pattern[]=
   {
      0x1f,                          // 5  --  1  [0]
      0x0f, 0x17, 0x1b, 0x1d, 0x1e,  // 4  --  5  [ 1] ~ [ 5]
      0x07, 0x0b, 0x0d, 0x0e,        // 3  --  4  [ 6] ~ [ 9]
      0x03, 0x05, 0x06,              // 2  --  2  [10] ~ [12]
      0x5d
   };
   int16   i, j;
   byte    begin, end;
   uint16  temp = 0;

   if( chess_object->level != 1 )
   {
       temp = board_pattern & 0x00ff;
       if( temp == 0x005d || temp == 0xdd || temp == 0xba || temp == 0xbb )
       {
            chess_object->particular = 1;
            return (byte)0x2d;
       }
   }

  begin = 0; end = left + right - 4;             // Pattern 0 ~ 5
  for(j=0; j<=5; j++)
  {
    for(i=begin;i<=end;i++)
    {
      temp = connect_pattern[j] << i;
      if((board_pattern&temp) == temp)
      {
         if(i-1<0 || i>left+right-4)
            temp = 0;
         else if(board_pattern&(0x21<<(i-1)))
                  temp = 0;
               else
                  temp = 0x40;
         break;
      }
    }
    if(i<=end) break;
  }
  if(j==0)
    return (byte)0x80;
  else if(j<=5)
         return j | temp;

  if(left>3) begin++; if(right>3) end--; end++;// Pattern 6 ~ 9
  for(j=6; j<=9; j++)
  {
    for(i=begin;i<=end;i++)
    {
      temp = connect_pattern[j] << i;
      if((board_pattern&temp) == temp)
      {
         if(i-1<0 || i>left+right-4)
            temp = 0;
         else if(board_pattern&(0x21<<(i-1)))
                  temp = 0;
               else
                  temp = 0x20;
         break;
      }
    }
    if(i<=end) break;
  }
  if(j<=9) return j | temp;

  if(left>2) begin++; if(right>2) end--; end++;// Pattern 10 ~ 12
  for(j=10; j<=12; j++)
  {
    for(i=begin;i<=end;i++)
    {
      temp = connect_pattern[j] << i;
      if((board_pattern&temp) == temp) break;
    }
    if(i<=end) break;
  }
  if(j<=12) return (byte)j;
  return (byte)0x1f;
}//ConnectSub()

///////////////////////////////////////////////
//connect
//////////////////////////////////////////////
static dword Connect(CHESS_OBJECT *chess_object, uint16  x, uint16  y, byte who)
{
   byte enemy;
   int16 i, j, left, right;
   dword board_pattern, pattern_temp, ret_value;

   if(chess_object->map[y][x]!=0)
      return 0x08000000l;

   chess_object->map[y][x]=who;
   enemy = (who==MAN)? MACHINE : MAN;     /* enemy = (~who) & 3; */
   ret_value = 0l;

   // Up <-> Down
   pattern_temp = 0;
   for(j=y-1,right=0;j>=0&&right<4;right++,j--)
    if(chess_object->map[j][x]==enemy) break;
   if(j>=0&&chess_object->map[j][x]!=enemy)
   {
     if(chess_object->map[j][x]==who)
         pattern_temp |= 0xc000;
     else
         pattern_temp |= 0x8000;
   }
   for(j=y+1,left=0;j<CHESS_LEN_Y&&left<4;left++,j++)
    if(chess_object->map[j][x]==enemy) break;
   if(j<CHESS_LEN_Y&&chess_object->map[j][x]!=enemy)
   {
     if(chess_object->map[j][x]==who)
         pattern_temp |= 0x3000;
     else
         pattern_temp |= 0x2000;
   }
   if(right+left+1>=5)
   {
     ret_value |= 0x80000000l;
     board_pattern = 0;
     for(j=y-right;j<=y+left;j++)
     {
         board_pattern <<= 1;
         if(chess_object->map[j][x]==who) board_pattern |= 1;
     }
     board_pattern |= pattern_temp;
     i = (int16)ConnectSub(chess_object, (byte)left, (byte)right, (uint16)board_pattern);
     if(i&0x10)
         ret_value &= 0x7fffffffl;
     else
     {
         if(i&0x80)
            ret_value |= 0x00c00000l;
         else if(i&0x40)
                  ret_value |= 0x00800000l;
              else if(i&0x20)
                     ret_value |= 0x00400000l;
         ret_value |= (((dword)i)&0xf)<<12;
      }
   }

   // Up.right <-> Down.left
   pattern_temp = 0;
   for(i=x-1,j=y+1,right=0;i>=0&&j<CHESS_LEN_Y&&right<4;right++,i--,j++)
    if(chess_object->map[j][i]==enemy) break;
   if(i>=0&&j<CHESS_LEN_Y&&chess_object->map[j][i]!=enemy)
   {
      if(chess_object->map[j][i]==who)
         pattern_temp |= 0xc000;
      else
         pattern_temp |= 0x8000;
   }
   for(i=x+1,j=y-1,left=0;i<CHESS_LEN_X&&j>=0&&left<4;left++,i++,j--)
    if(chess_object->map[j][i]==enemy) break;
   if(i<CHESS_LEN_X&&j>=0&&chess_object->map[j][i]!=enemy)
   {
      if(chess_object->map[j][i]==who)
         pattern_temp |= 0x3000;
      else
         pattern_temp |= 0x2000;
   }
   if(right+left+1>=5)
   {
      ret_value |= 0x40000000l;
      board_pattern = 0;
      for(i=x-right,j=y+right;i<=x+left;i++,j--)
      {
        board_pattern <<= 1;
        if(chess_object->map[j][i]==who) board_pattern |= 1;
      }
      board_pattern |= pattern_temp;
      i = (byte)ConnectSub(chess_object, (byte)left, (byte)right, (uint16)board_pattern);
      if(i&0x10)
        ret_value &= 0xbfffffffl;
      else
      {
         if(i&0x80)
            ret_value |= 0x00300000l;
         else if(i&0x40)
                  ret_value |= 0x00200000l;
              else if(i&0x20)
                     ret_value |= 0x00100000l;
         ret_value |= (((dword)i)&0xf)<<8;
      }
   }

   // right <-> left
   pattern_temp = 0;
   for(i=x-1,right=0;i>=0&&right<4;right++,i--)
      if(chess_object->map[y][i]==enemy) break;
   if(i>=0&&chess_object->map[y][i]!=enemy)
   {
      if(chess_object->map[y][i]==who)
         pattern_temp |= 0xc000;
      else
         pattern_temp |= 0x8000;
   }
   for(i=x+1,left=0;i<CHESS_LEN_X&&left<4;left++,i++)
      if(chess_object->map[y][i]==enemy) break;
   if(i<CHESS_LEN_X&&chess_object->map[y][i]!=enemy)
   {
      if(chess_object->map[y][i]==who)
         pattern_temp |= 0x3000;
      else
         pattern_temp |= 0x2000;
   }
   if(right+left+1>=5)
   {
      ret_value |= 0x20000000l;
      board_pattern = 0;
      for(i=x-right;i<=x+left;i++)
      {
         board_pattern <<= 1;
         if(chess_object->map[y][i]==who) board_pattern |= 1;
      }
      board_pattern |= pattern_temp;
      i = (byte)ConnectSub(chess_object, (byte)left, (byte)right, (uint16)board_pattern);
      if(i&0x10)
         ret_value &= 0xdfffffffl;
      else
      {
         if(i&0x80)
            ret_value |= 0x000c0000l;
         else if(i&0x40)
                  ret_value |= 0x00080000l;
               else if(i&0x20)
                        ret_value |= 0x00040000l;
         ret_value |= (i&0xf)<<4;
      }
   }

   // Down.right <-> Up.left
   pattern_temp = 0;
   for(i=x-1,j=y-1,right=0;i>=0&&j>=0&&right<4;right++,i--,j--)
      if(chess_object->map[j][i]==enemy) break;
   if(i>=0&&j>=0&&chess_object->map[j][i]!=enemy)
   {
      if(chess_object->map[j][i]==who)
         pattern_temp |= 0xc000;
      else
         pattern_temp |= 0x8000;
   }
   for(i=x+1,j=y+1,left=0;i<CHESS_LEN_X&&j<CHESS_LEN_Y&&left<4;left++,i++,j++)
      if(chess_object->map[j][i]==enemy) break;
   if(i<CHESS_LEN_X&&j<CHESS_LEN_Y&&chess_object->map[j][i]!=enemy)
   {
      if(chess_object->map[j][i]==who)
         pattern_temp |= 0x3000;
      else
         pattern_temp |= 0x2000;
   }
   if(right+left+1>=5)
   {
      ret_value |= 0x10000000l;
      board_pattern = 0;
      for(i=x-right,j=y-right;i<=x+left;i++,j++)
      {
         board_pattern <<= 1;
         if(chess_object->map[j][i]==who) board_pattern |= 1;
      }
      board_pattern |= pattern_temp;
      i =(int16)ConnectSub(chess_object, (byte)left, (byte)right, (uint16)board_pattern);
      if(i&0x10)
         ret_value &= 0xefffffffl;
      else
      {
         if(i&0x80)
            ret_value |= 0x00030000l;
         else if(i&0x40)
                  ret_value |= 0x00020000l;
               else if(i&0x20)
                        ret_value |= 0x00010000l;
         ret_value |= i&0xf;
      }
   }

   chess_object->map[y][x]=0;
   return ret_value;
}
///////////////////////////////////////////////
//CalValue
//////////////////////////////////////////////
static uint16 CalValue(CHESS_OBJECT *chess_object, dword connect_result)
{
   static const  uint16 mode_value[]=
   {/* 0   1   2   3   4   5   6   7   8   9   a   b   c   d   */
      19, 15,  9,  9,  9, 15,  7,  4,  4,  7,  3,  2,  3, 15    // level value of o+o+o+o
   };
   byte mode, multi;
   uint16 ret_value;

   ret_value = multi = 0;
   if(connect_result&0x80000000l)
   {
      mode = (byte)(connect_result>>12)&0xf;
      ret_value += mode_value[mode];
      switch((connect_result>>22)&3)
      {
         case 3: ret_value |= 0x8000; multi++; break;
         case 2: ret_value |= 0x4000; multi++; break;
         case 1: if( chess_object->level != 1 && chess_object->particular == 1 )
                 { //handle particular case:o+o+o+o
                     ret_value |= 0x2000;
                     chess_object->particular = 0;
                 }
                 multi ++;
                 break;
         default:
                 if(mode<=5) multi++;
                 break;  //jhh add
      }
   }

   if(connect_result&0x40000000l)
   {
      mode = (byte)(connect_result>>8)&0xf;
      ret_value += mode_value[mode];
      switch((connect_result>>20)&3)
      {
         case 3: ret_value |= 0x8000; multi++; break;
         case 2: ret_value |= 0x4000; multi++; break;
         case 1: if( chess_object->level != 1 && chess_object->particular == 1 )
                 { //handle particular case:o+o+o+o
                     ret_value |= 0x2000;            //
                     chess_object->particular = 0;
                 }
                 multi ++;
                 break;
         default:
         if(mode<=5) multi++;
      }
   }

   if(connect_result&0x20000000l)
   {
      mode = (byte)(connect_result>>4)&0xf;
      ret_value += mode_value[mode];
      switch((connect_result>>18)&3)
      {
         case 3: ret_value |= 0x8000; multi++; break;
         case 2: ret_value |= 0x4000; multi++; break;
         case 1: if( chess_object->level != 1 && chess_object->particular == 1 )
                 { //handle particular case:o+o+o+o
                     ret_value |= 0x2000;             //
                     chess_object->particular = 0;
                 }
                 multi ++;
                 break;
         default:
         if(mode<=5) multi++;
      }
   }

   if(connect_result&0x10000000l)
   {
      mode = (byte)connect_result&0xf;
      ret_value += mode_value[mode];
      switch((connect_result>>16)&3)
      {
         case 3: ret_value |= 0x8000; multi++; break;
         case 2: ret_value |= 0x4000; multi++; break;
         case 1: if( chess_object->level != 1 && chess_object->particular == 1 )
                 { //handle particular case:o+o+o+o
                     ret_value |= 0x2000;             //
                     chess_object->particular = 0;
                 }
                 multi ++;
                 break;
         default:
            if(mode<=5) multi++;
            break;
      }
   }

   if(multi>=2) ret_value |= 0x2000;

   return ret_value;
}//CalValue()

// return value:
// [1] who win    = 0x81 (attack)
// [2] all_no_win = 0x00
// [3] normal     = 0x01
// [4] important  = 0x80 ~ 0x20  or 0x41 ~ 0x21
static byte MachinePutChess(CHESS_OBJECT *chess_object, uint16 *put_x, uint16 *put_y, char who)
{
  uint16 x, y, max;
  char enemy = (who==MAN)? MACHINE : MAN;

  for(x=0;x<CHESS_LEN_X;x++) for(y=0;y<CHESS_LEN_Y;y++)
  {
    chess_object->attack_value[y][x] = chess_object->defence_value[y][x] = 0;
    if(chess_object->map[y][x]==0)
    {
      chess_object->attack_value[y][x]=CalValue(chess_object, Connect(chess_object, x, y, who));
      chess_object->defence_value[y][x]=CalValue(chess_object, Connect(chess_object, x, y, enemy));
      if(chess_object->attack_value[y][x]&0x8000)
      { *put_x = x; *put_y = y; return 0x81; }
    }
  }

  // [1] first, consider defence.
  *put_x = *put_y = 0;
  for(x=0;x<CHESS_LEN_X;x++) for(y=0;y<CHESS_LEN_Y;y++)
    if(chess_object->defence_value[y][x]&0x8000)
      if(chess_object->defence_value[y][x]>chess_object->defence_value[*put_y][*put_x])
      { *put_x = x; *put_y = y; }
  if(chess_object->defence_value[*put_y][*put_x]&0x8000) return 0x80;

  // [2] second, consider attack.
  *put_x = *put_y = 0;
  for(x=0;x<CHESS_LEN_X;x++) for(y=0;y<CHESS_LEN_Y;y++)
    if(chess_object->attack_value[y][x]&0x4000)
      if(chess_object->attack_value[y][x]>chess_object->attack_value[*put_y][*put_x])
      { *put_x = x; *put_y = y; }
  if(chess_object->attack_value[*put_y][*put_x]&0x4000) return 0x41;

  // [3] 3th, consider defence.
  *put_x = *put_y = 0;
  for(x=0;x<CHESS_LEN_X;x++) for(y=0;y<CHESS_LEN_Y;y++)
    if(chess_object->defence_value[y][x]&0x4000)
      if(chess_object->defence_value[y][x]>chess_object->defence_value[*put_y][*put_x])
      { *put_x = x; *put_y = y; }
  if(chess_object->defence_value[*put_y][*put_x]&0x4000) return 0x40;

  // [4] 4th, consider attack.
  *put_x = *put_y = 0;
  for(x=0;x<CHESS_LEN_X;x++) for(y=0;y<CHESS_LEN_Y;y++)
    if(chess_object->attack_value[y][x]&0x2000)
      if(chess_object->attack_value[y][x]>chess_object->attack_value[*put_y][*put_x])
      {
         *put_x = x; *put_y = y;
         if(chess_object->level<3) chess_object->attack_value[y][x]=5;
      }
  if(chess_object->attack_value[*put_y][*put_x]&0x2000) return 0x21;

  // [5] 5th, consider defence.
  *put_x = *put_y = 0;
  for(x=0;x<CHESS_LEN_X;x++) for(y=0;y<CHESS_LEN_Y;y++)
    if(chess_object->defence_value[y][x]&0x2000)
      if(chess_object->defence_value[y][x]>chess_object->defence_value[*put_y][*put_x])
      {
         *put_x = x; *put_y = y;
         if(chess_object->level==1) chess_object->defence_value[y][x]=5;
      }
  if(chess_object->defence_value[*put_y][*put_x]&0x2000) return 0x20;

  // [6] 6th, normal
  *put_x = *put_y = 0;
  for(x=0;x<CHESS_LEN_X;x++) for(y=0;y<CHESS_LEN_Y;y++)
    if(chess_object->attack_value[y][x]>chess_object->attack_value[*put_y][*put_x])
    { *put_x = x; *put_y = y; }
    else
    if(chess_object->level>=3&&chess_object->attack_value[y][x]==chess_object->attack_value[*put_y][*put_x])
      if(chess_object->defence_value[y][x]>chess_object->defence_value[*put_y][*put_x])
         { *put_x = x; *put_y = y; }

  max = 0;
  if(chess_object->level>=3)
    for(x=*put_x;x<CHESS_LEN_X;x++) for(y=0;y<CHESS_LEN_Y;y++)
    { if(chess_object->attack_value[y][x]==chess_object->attack_value[*put_y][*put_x]
         && chess_object->defence_value[y][x]==chess_object->defence_value[*put_y][*put_x])
         max++;
    }
  else
    for(x=*put_x;x<CHESS_LEN_X;x++) for(y=0;y<CHESS_LEN_Y;y++)
      if(chess_object->attack_value[y][x]==chess_object->attack_value[*put_y][*put_x])
         max++;

  if(max!=1)
  {
     max = (int16)(GmFive_GetRndNumber(max)+1); //Random(max) + 1;
     for(x=*put_x;x<CHESS_LEN_X;x++) for(y=0;y<CHESS_LEN_Y;y++)
     {
       if(chess_object->level>=3 && chess_object->attack_value[y][x]==chess_object->attack_value[*put_y][*put_x]
          && chess_object->defence_value[y][x]==chess_object->defence_value[*put_y][*put_x])
            max--;
       else
         if(chess_object->level<3 && chess_object->attack_value[y][x]==chess_object->attack_value[*put_y][*put_x])
            max--;
       if(max==0)
       { *put_x = x; *put_y = y; goto done; }
     }
  }
done:
  if(chess_object->attack_value[*put_y][*put_x]+chess_object->defence_value[*put_y][*put_x] == 0 && chess_object->map[*put_y][*put_x] != 0)
      return 0;
  else
      return 1;
}//MachinePutChess()


///////////////////////////////////////////////
//draw piece
//////////////////////////////////////////////
static void RestartRepentQueue(CHESS_OBJECT *chess_object)
{
   if(chess_object->repent_queue.tail_ptr_var != -1)
   {
      chess_object->repent_queue.tail_point = chess_object->repent_queue.tail_ptr_var;
      chess_object->repent_queue.tail_ptr_var = -1;;
   }
}//RestartRepentQueue()

//////////////////////////////////////////////////
//clear history square area
//////////////////////////////////////////////////
static void ClearHistorySquare(GmFive *pMe, boolean clrFlag)
{

   AEERect      rc;
   checkPiece  *temPiece = NULL;

   if(IGRAPHICS_GetFillMode(pMe->m_pGraphics))
   {
      (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics,FALSE);
   }
   if(!clrFlag)
      temPiece = &pMe->m_lastManPiece;
   else
      temPiece = &pMe->m_curManPiece;

   //draw last piece
   IGRAPHICS_SetColor(pMe->m_pGraphics,BACKGD_R_COLOR,BACKGD_G_COLOR,BACKGD_B_COLOR,0);

   SETAEERECT(&rc, X_OFF+temPiece->col*GRID_WIDTH+GRID_WIDTH/2,
              Y_OFF+temPiece->row*GRID_HEIGHT+GRID_HEIGHT/2,
              GRID_WIDTH+1,
              GRID_HEIGHT+1);
   (void)IGRAPHICS_DrawRect(pMe->m_pGraphics,&rc);
   if(!clrFlag)
   {
      AEEPoint     pPoint;
      //draw line blank point  left -> right up -> down
      //left point
      IGRAPHICS_SetColor(pMe->m_pGraphics,GRID_R_COLOR,GRID_G_COLOR,GRID_B_COLOR,0);
      if(temPiece->col >= 0)
      {
         pPoint.x = rc.x;
         pPoint.y = rc.y + GRID_HEIGHT/2;
         IGRAPHICS_DrawPoint(pMe->m_pGraphics,&pPoint);
      }
      //right point
      if(temPiece->col < CHESS_LEN_X - 2)
      {
         pPoint.x = rc.x+rc.dx-1;
         pPoint.y = rc.y + GRID_HEIGHT/2;
         IGRAPHICS_DrawPoint(pMe->m_pGraphics,&pPoint);
      }
      //up point
      if(temPiece->row >= 0)
      {
         pPoint.x = rc.x+GRID_WIDTH/2;
         pPoint.y = rc.y;
         IGRAPHICS_DrawPoint(pMe->m_pGraphics,&pPoint);
      }
      //down point
      if(temPiece->row < CHESS_LEN_Y - 2)
      {
         pPoint.x = rc.x+GRID_WIDTH/2;
         pPoint.y = rc.y+rc.dy-1;
         IGRAPHICS_DrawPoint(pMe->m_pGraphics,&pPoint);
      }
   }//if(!clrFlag)
   IGRAPHICS_Update(pMe->m_pGraphics);
}//ClearHistorySquare()

//////////////////////////////////////////////////
//draw current man square
//////////////////////////////////////////////////
static void DrawCurSquare(GmFive *pMe)
{
   AEERect      rc;
   checkPiece  *temPiece = &pMe->m_curManPiece;

   ClearHistorySquare(pMe,FALSE);
   if(IGRAPHICS_GetFillMode(pMe->m_pGraphics))
   {
      (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics,FALSE);
   }
   //draw current piece
   IGRAPHICS_SetColor(pMe->m_pGraphics,SQUARE_R_COLOR,SQUARE_G_COLOR,SQUARE_B_COLOR,0);

   SETAEERECT(&rc, X_OFF+temPiece->col*GRID_WIDTH+GRID_WIDTH/2,
              Y_OFF+temPiece->row*GRID_HEIGHT+GRID_HEIGHT/2,
              GRID_WIDTH+1 ,
              GRID_HEIGHT+1);
   (void)IGRAPHICS_DrawRect(pMe->m_pGraphics,&rc);

   IGRAPHICS_Update(pMe->m_pGraphics);
   IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,FALSE);
   SplashCurSquare(pMe);

}//DrawCurSquare()

///////////////////////////////////////////////
//DrawManChessSquare
//////////////////////////////////////////////

static void DrawManChessSquare(GmFive * pMe,boolean isEmpty)
{

   AEERect      rc;
   checkPiece  *temPiece = &pMe->m_curManPiece;

   if(IGRAPHICS_GetFillMode(pMe->m_pGraphics))
   {
      (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics,FALSE);
   }

   if(isEmpty)
   {
      AEEPoint    pPoint;

      IGRAPHICS_SetColor(pMe->m_pGraphics,BACKGD_R_COLOR,BACKGD_G_COLOR,BACKGD_B_COLOR,0);

      SETAEERECT(&rc, X_OFF+temPiece->col*GRID_WIDTH+GRID_WIDTH/2,
              Y_OFF+temPiece->row*GRID_HEIGHT+GRID_HEIGHT/2,
              GRID_WIDTH+1,
              GRID_HEIGHT+1);
      (void)IGRAPHICS_DrawRect(pMe->m_pGraphics,&rc);

      IGRAPHICS_SetColor(pMe->m_pGraphics,GRID_R_COLOR,GRID_G_COLOR,GRID_B_COLOR,0);

      if(temPiece->col >= 0)
      {
         pPoint.x = rc.x;
         pPoint.y = rc.y + GRID_HEIGHT/2;
         IGRAPHICS_DrawPoint(pMe->m_pGraphics,&pPoint);
      }
      //right point
      if(temPiece->col < CHESS_LEN_X - 2)
      {
         pPoint.x = rc.x+rc.dx-1;
         pPoint.y = rc.y + GRID_HEIGHT/2;
         IGRAPHICS_DrawPoint(pMe->m_pGraphics,&pPoint);
      }
      //up point
      if(temPiece->row >= 0)
      {
         pPoint.x = rc.x+GRID_WIDTH/2;
         pPoint.y = rc.y;
         IGRAPHICS_DrawPoint(pMe->m_pGraphics,&pPoint);
      }
      //down point
      if(temPiece->row < CHESS_LEN_Y - 2)
      {
         pPoint.x = rc.x+GRID_WIDTH/2;
         pPoint.y = rc.y+rc.dy-1;
         IGRAPHICS_DrawPoint(pMe->m_pGraphics,&pPoint);
      }
   }
   else
   {
      //draw current square of man's chess
      IGRAPHICS_SetColor(pMe->m_pGraphics,SQUARE_R_COLOR,SQUARE_G_COLOR,SQUARE_B_COLOR,0);
      SETAEERECT(&rc, X_OFF+temPiece->col*GRID_WIDTH+GRID_WIDTH/2,
                 Y_OFF+temPiece->row*GRID_HEIGHT+GRID_HEIGHT/2,
                 GRID_WIDTH+1 ,
                 GRID_HEIGHT+1);
      (void)IGRAPHICS_DrawRect(pMe->m_pGraphics,&rc);
   }
   IGRAPHICS_Update(pMe->m_pGraphics);
   IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,FALSE);

}//DrawManChessSquare()
///////////////////////////////////////////////
//splash cur man's chess square
//////////////////////////////////////////////
static void SplashCurSquare(GmFive * pMe)
{
   if(pMe->m_squareSplashFlg)
   {
      DrawManChessSquare(pMe,FALSE);
      pMe->m_squareSplashFlg = FALSE;
   }
   else
   {
      DrawManChessSquare(pMe,TRUE);
      pMe->m_squareSplashFlg = TRUE;
   }
   ISHELL_SetTimer(pMe->a.m_pIShell,
                  SPLASH_PIECE_TIMER,
                  (PFNNOTIFY) SplashCurSquare,
                  pMe);

}//SplashCurSquare()


///////////////////////////////////////////////
//set screen background color
//////////////////////////////////////////////

static void DrawScreenBackground(GmFive * pMe)
{
   uint16 cx = pMe->m_cxScreen;
   uint16 cy = pMe->m_cyScreen;   
   AEERect     rc;
   RGBVAL      oldBGroudColr;

   SETAEERECT(&rc, 0, 0, cx, cy);
   oldBGroudColr = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND, BACK_GROUND_COLOR);
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay,&rc);
   (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,oldBGroudColr);
   IDISPLAY_Update( pMe->a.m_pIDisplay );

}//DrawScreenBackground()

///////////////////////////////////////////////
//add chess piece infor to repent queue
//////////////////////////////////////////////
static void AddToRepentQueue(CHESS_OBJECT *chess_object, int16 x, int16 y)
{
   if(chess_object->who_go == MAN)
   {
      chess_object->repent_queue.man_x[chess_object->repent_queue.tail_point] = x;
      chess_object->repent_queue.man_y[chess_object->repent_queue.tail_point] = y;
   }
   else  // MACHINE
   {
      chess_object->repent_queue.machine_x[chess_object->repent_queue.tail_point] = x;
      chess_object->repent_queue.machine_y[chess_object->repent_queue.tail_point] = y;
      chess_object->repent_queue.tail_point++;
      if(chess_object->repent_queue.tail_point > REPENT_LEN-1)
         chess_object->repent_queue.tail_point = 0;
      if(chess_object->repent_queue.tail_point == chess_object->repent_queue.head_point)
      {
         chess_object->repent_queue.head_point++;
         if(chess_object->repent_queue.head_point > REPENT_LEN - 1)
            chess_object->repent_queue.head_point = 0;
      }
      chess_object->repent_queue.total++;
   }
}//AddToRepentQueue()

//由方杰提供
///////////////////////////////////////////////
//judging rule(three and three)
//////////////////////////////////////////////
static boolean JudgeThreeAndThreeRule(GmFive * pMe)
{

   char   who;
   int16  i,j,a,b,c,d,col = 0,row = 0;
   int16  Three1=0,Three2=0,Three3=0,Three4=0,ThreeSum;
   CHESS_OBJECT *chess_object = NULL;

   if(!pMe)
   {
     return FALSE;
   }
   chess_object = &pMe->m_chess_object;

#ifndef ONLY_MAN_CHESS_DEBUG
   if(chess_object->who_go == MACHINE)
   {
      col = pMe->m_curManPiece.col+1;
      row = pMe->m_curManPiece.row+1;
   }
   else if(chess_object->who_go == MAN)
   {
      col = pMe->m_tempPiece.col;
      row = pMe->m_tempPiece.row;
   }
#else    //only man go
   col = pMe->m_curManPiece.col+1;
   row = pMe->m_curManPiece.row+1;
#endif //ONLY_MAN_CHESS_DEBUG
   who = chess_object->map[row][col];

//fjie 2004.1.7
   //left and right inspect
   for(a=0, j=-1; col+j>=0; j--)
   {
      if(chess_object->map[row][col+j] == who)
         a++;
      else
         break;
   }
   for(j=1; col+j<CHESS_LEN_X; j++)
   {
      if(chess_object->map[row][col+j]==who)
         a++;
      else
         break;
   }
   if(a==2)
   {
      if((chess_object->map[row][col+j]==0)&&
         (chess_object->map[row][col+(j-4)]==0)&&
         (col+j<CHESS_LEN_X)&&(col+(j-4)>=0))
      {
        Three1=100;
      }
   }
   if((chess_object->map[row][col-1]==0)&&
      (chess_object->map[row][col-2]==who)&&
      (chess_object->map[row][col-3]==who)&&
      (chess_object->map[row][col+1]==0)&&
      (chess_object->map[row][col-4]==0)&&
      (col-4>=0)&&(col+1<CHESS_LEN_X))   //横向左1
   {
     Three1=100;
   }
   if((chess_object->map[row][col+1]==0)&&
      (chess_object->map[row][col+2]==who)&&
      (chess_object->map[row][col+3]==who)&&
      (chess_object->map[row][col-1]==0)&&
      (chess_object->map[row][col+4]==0)&&
      (col-1>=0)&&(col+4<CHESS_LEN_X))   //横向右1
   {
     Three1=100;
   }
   if((chess_object->map[row][col-1]==who)&&
      (chess_object->map[row][col-2]==0)&&
      (chess_object->map[row][col-3]==who)&&
      (chess_object->map[row][col+1]==0)&&
      (chess_object->map[row][col-4]==0)&&
      (col-4>=0)&&(col+1<CHESS_LEN_X))   //横向左2
   {
     Three1=100;
   }
   if((chess_object->map[row][col+1]==who)&&
      (chess_object->map[row][col+2]==0)&&
      (chess_object->map[row][col+3]==who)&&
      (chess_object->map[row][col-1]==0)&&
      (chess_object->map[row][col+4]==0)&&
      (col-1>=0)&&(col+4<CHESS_LEN_X))   //横向右2
   {
     Three1=100;
   }
   if((chess_object->map[row][col-1]==0)&&
      (chess_object->map[row][col-2]==who)&&
      (chess_object->map[row][col+1]==who)&&
      (chess_object->map[row][col+2]==0)&&
      (chess_object->map[row][col-3]==0)&&
      (col-3>=0)&&(col+2<CHESS_LEN_X))   //横向左3
   {
     Three1=100;
   }
   if((chess_object->map[row][col+1]==0)&&
      (chess_object->map[row][col+2]==who)&&
      (chess_object->map[row][col-1]==who)&&
      (chess_object->map[row][col-2]==0)&&
      (chess_object->map[row][col+3]==0)&&
      (col-2>=0)&&(col+3<CHESS_LEN_X))   //横向右3
   {
     Three1=100;
   }

   // up and down inspect
   for(b=0, i=-1; row+i>=0; i--)
   {
      if(chess_object->map[row+i][col]==who)
         b++;
      else
         break;
   }
   for(i=1; row+i<CHESS_LEN_Y; i++)
   {
      if(chess_object->map[row+i][col]==who)
         b++;
      else
         break;
   }
   if(b==2)
   {
      if((chess_object->map[row+i][col]==0)&&
         (chess_object->map[row+(i-4)][col]==0)&&
         (row+i<CHESS_LEN_Y)&&(row+(i-4)>=0))
      {
        Three2=100;
      }
   }
   if((chess_object->map[row+1][col]==0)&&
      (chess_object->map[row+2][col]==who)&&
      (chess_object->map[row+3][col]==who)&&
      (chess_object->map[row-1][col]==0)&&
      (chess_object->map[row+4][col]==0)&&
      (row-1>=0)&&(row+4<CHESS_LEN_Y))    //竖向下1
   {
     Three2=100;
   }
   if((chess_object->map[row-1][col]==0)&&
      (chess_object->map[row-2][col]==who)&&
      (chess_object->map[row-3][col]==who)&&
      (chess_object->map[row+1][col]==0)&&
      (chess_object->map[row-4][col]==0)&&
      (row-4>=0)&&(row+1<CHESS_LEN_Y))    //竖向上1
   {
     Three2=100;
   }
   if((chess_object->map[row+1][col]==who)&&
      (chess_object->map[row+2][col]==0)&&
      (chess_object->map[row+3][col]==who)&&
      (chess_object->map[row-1][col]==0)&&
      (chess_object->map[row+4][col]==0)&&
      (row-1>=0)&&(row+4<CHESS_LEN_Y))    //竖向下2
   {
     Three2=100;
   }
   if((chess_object->map[row-1][col]==who)&&
      (chess_object->map[row-2][col]==0)&&
      (chess_object->map[row-3][col]==who)&&
      (chess_object->map[row+1][col]==0)&&
      (chess_object->map[row-4][col]==0)&&
      (row-4>=0)&&(row+1<CHESS_LEN_Y))    //竖向上2
   {
     Three2=100;
   }
   if((chess_object->map[row+1][col]==0)&&
      (chess_object->map[row+2][col]==who)&&
      (chess_object->map[row-1][col]==who)&&
      (chess_object->map[row-2][col]==0)&&
      (chess_object->map[row+3][col]==0)&&
      (row-2>=0)&&(row+3<CHESS_LEN_Y))    //竖向下3
   {
     Three2=100;
   }
   if((chess_object->map[row-1][col]==0)&&
      (chess_object->map[row-2][col]==who)&&
      (chess_object->map[row+1][col]==who)&&
      (chess_object->map[row+2][col]==0)&&
      (chess_object->map[row-3][col]==0)&&
      (row-3>=0)&&(row+2<CHESS_LEN_Y))    //竖向上3
   {
     Three2=100;
   }

   // left_up and right_down inspect
   for(c=0, i=-1, j=-1; row+i>=0 && col+j>=0; i--, j--)
   {
      if(chess_object->map[row+i][col+j]==who)
         c++;
      else
         break;
   }
   for(i=1, j=1; row+i<CHESS_LEN_Y && col+j<CHESS_LEN_X; i++, j++)
   {
      if(chess_object->map[row+i][col+j]==who)
         c++;
      else
         break;
   }
   if(c==2)
   {
      if((chess_object->map[row+i][col+j]==0)&&
         (chess_object->map[row+(i-4)][col+(j-4)]==0)&&
         (row+i<CHESS_LEN_Y)&&(col+j<CHESS_LEN_X)&&
         (row+(i-4)>=0)&&(col+(j-4)>=0))
      {
        Three3=100;
      }
   }
   if((chess_object->map[row+1][col+1]==0)&&
      (chess_object->map[row+2][col+2]==who)&&
      (chess_object->map[row+3][col+3]==who)&&
      (chess_object->map[row-1][col-1]==0)&&
      (chess_object->map[row+4][col+4]==0)&&
      (col-1>=0)&&(col+4<CHESS_LEN_X)&&
      (row-1>=0)&&(row+4<CHESS_LEN_Y))    //左斜下1
   {
     Three3=100;
   }
   if((chess_object->map[row-1][col-1]==0)&&
      (chess_object->map[row-2][col-2]==who)&&
      (chess_object->map[row-3][col-3]==who)&&
      (chess_object->map[row+1][col+1]==0)&&
      (chess_object->map[row-4][col-4]==0)&&
      (col-4>=0)&&(col+1<CHESS_LEN_X)&&
      (row-4>=0)&&(row+1<CHESS_LEN_Y))    //左斜上1
   {
     Three3=100;
   }
   if((chess_object->map[row+1][col+1]==who)&&
      (chess_object->map[row+2][col+2]==0)&&
      (chess_object->map[row+3][col+3]==who)&&
      (chess_object->map[row-1][col-1]==0)&&
      (chess_object->map[row+4][col+4]==0)&&
      (col-1>=0)&&(col+4<CHESS_LEN_X)&&
      (row-1>=0)&&(row+4<CHESS_LEN_Y))    //左斜下2
   {
     Three3=100;
   }
   if((chess_object->map[row-1][col-1]==who)&&
      (chess_object->map[row-2][col-2]==0)&&
      (chess_object->map[row-3][col-3]==who)&&
      (chess_object->map[row+1][col+1]==0)&&
      (chess_object->map[row-4][col-4]==0)&&
      (col-4>=0)&&(col+1<CHESS_LEN_X)&&
      (row-4>=0)&&(row+1<CHESS_LEN_Y))    //左斜上2
   {
     Three3=100;
   }
   if((chess_object->map[row+1][col+1]==0)&&
      (chess_object->map[row+2][col+2]==who)&&
      (chess_object->map[row-1][col-1]==who)&&
      (chess_object->map[row-2][col-2]==0)&&
      (chess_object->map[row+3][col+3]==0)&&
      (col-2>=0)&&(col+3<CHESS_LEN_X)&&
      (row-2>=0)&&(row+3<CHESS_LEN_Y))    //左斜下3
   {
     Three3=100;
   }
   if((chess_object->map[row-1][col-1]==0)&&
      (chess_object->map[row-2][col-2]==who)&&
      (chess_object->map[row+1][col+1]==who)&&
      (chess_object->map[row+2][col+2]==0)&&
      (chess_object->map[row-3][col-3]==0)&&
      (col-3>=0)&&(col+2<CHESS_LEN_X)&&
      (row-3>=0)&&(row+2<CHESS_LEN_Y))    //左斜上3
   {
     Three3=100;
   }

   // left_down and right_up inspect
   for(d=0, i=1, j=-1; row+i<CHESS_LEN_Y && col+j>=0; i++, j--)
   {
      if(chess_object->map[row+i][col+j]==who)
         d++;
      else
         break;
   }
   for(i=-1, j=1; row+i>=0 && col+j<CHESS_LEN_X; i--, j++)
   {
      if(chess_object->map[row+i][col+j]==who)
         d++;
      else
         break;
   }
   if(d==2)
   {
      if((chess_object->map[row+i][col+j]==0)&&
         (chess_object->map[row+(i+4)][col+(j-4)]==0)&&
         (row+i>=0)&&(col+j<CHESS_LEN_X)&&
         (row+(i+4)<CHESS_LEN_Y)&&(col+(j-4)>=0))
      {
        Three4=100;
      }
   }
   if((chess_object->map[row+1][col-1]==0)&&
      (chess_object->map[row+2][col-2]==who)&&
      (chess_object->map[row+3][col-3]==who)&&
      (chess_object->map[row-1][col+1]==0)&&
      (chess_object->map[row+4][col-4]==0)&&
      (col-4>=0)&&(col+1<CHESS_LEN_X)&&
      (row-1>=0)&&(row+4<CHESS_LEN_Y))    //右斜下1
   {
     Three4=100;
   }
   if((chess_object->map[row-1][col+1]==0)&&
      (chess_object->map[row-2][col+2]==who)&&
      (chess_object->map[row-3][col+3]==who)&&
      (chess_object->map[row+1][col-1]==0)&&
      (chess_object->map[row-4][col+4]==0)&&
      (col-1>=0)&&(col+4<CHESS_LEN_X)&&
      (row-4>=0)&&(row+1<CHESS_LEN_Y))    //右斜上1
   {
     Three4=100;
   }
   if((chess_object->map[row+1][col-1]==who)&&
      (chess_object->map[row+2][col-2]==0)&&
      (chess_object->map[row+3][col-3]==who)&&
      (chess_object->map[row-1][col+1]==0)&&
      (chess_object->map[row+4][col-4]==0)&&
      (col-4>=0)&&(col+1<CHESS_LEN_X)&&
      (row-1>=0)&&(row+4<CHESS_LEN_Y))    //右斜下2
   {
     Three4=100;
   }
   if((chess_object->map[row-1][col+1]==who)&&
      (chess_object->map[row-2][col+2]==0)&&
      (chess_object->map[row-3][col+3]==who)&&
      (chess_object->map[row+1][col-1]==0)&&
      (chess_object->map[row-4][col+4]==0)&&
      (col-1>=0)&&(col+4<CHESS_LEN_X)&&
      (row-4>=0)&&(row+1<CHESS_LEN_Y))    //右斜上2
   {
     Three4=100;
   }
   if((chess_object->map[row+1][col-1]==0)&&
      (chess_object->map[row+2][col-2]==who)&&
      (chess_object->map[row-1][col+1]==who)&&
      (chess_object->map[row-2][col+2]==0)&&
      (chess_object->map[row+3][col-3]==0)&&
      (col-3>=0)&&(col+2<CHESS_LEN_X)&&
      (row-2>=0)&&(row+3<CHESS_LEN_Y))    //右斜下3
   {
     Three4=100;
   }
   if((chess_object->map[row-1][col+1]==0)&&
      (chess_object->map[row-2][col+2]==who)&&
      (chess_object->map[row+1][col-1]==who)&&
      (chess_object->map[row+2][col-2]==0)&&
      (chess_object->map[row-3][col+3]==0)&&
      (col-2>=0)&&(col+3<CHESS_LEN_X)&&
      (row-3>=0)&&(row+2<CHESS_LEN_Y))    //右斜上3
   {
     Three4=100;
   }

   ThreeSum=Three1+Three2+Three3+Three4;  //fjie end
   if((ThreeSum>=200)&&( pMe->m_global_who_go == MAN)&&(who == MAN))
   {
      chess_object->who_win = MACHINE;
      pMe->m_forbidPieceFlg = TRUE;
      return TRUE;
   }
   else if((ThreeSum>=200)&&( pMe->m_global_who_go == MACHINE)&&(who == MACHINE))
   {
      chess_object->who_win = MAN;
      pMe->m_forbidPieceFlg = TRUE;
      return TRUE;
   }
   return FALSE;
}//JudgeThreeAndThreeRule()

///////////////////////////////////////////////
//JudgeWin
//////////////////////////////////////////////
static boolean JudgeWin(CHESS_OBJECT *chess_object)
{
   char   who;
   int16  row, col ,i ,j, n;

   if( chess_object->put_chess_counter > (CHESS_LEN_X)*(CHESS_LEN_Y)/2 )
   {
      chess_object->who_win = ALL_NO_WIN;   
      return TRUE;
   }

   for(row = 0; row < CHESS_LEN_Y; row++)
      for(col = 0; col < CHESS_LEN_X; col++)
      {
         if((who = chess_object->map[row][col]) != 0)
         {
            // left and right inspect
            for(n=0, j=-1; col+j>=0; j--)
            {
               if(chess_object->map[row][col+j]==who)
               {               
                  n++;
               }
               else
                  break;
            }
            for(j=1; col+j<CHESS_LEN_X; j++)
            {
               if(chess_object->map[row][col+j]==who)
               {               
                  n++;
               }
               else
                  break;
            }
            if(n == 4)
            {             
               chess_object->who_win = who;
               return TRUE;
            }
           //fjie 2003.1.1
            if(n>4)
            {
               if(who==MAN)
               {
                  chess_object->who_win = MACHINE;
                  return TRUE;
               }
               if(who==MACHINE)
               {
                  chess_object->who_win = MAN;
                  return TRUE;
               }
            }//fjie end
            // up and down inspect
            for(n=0, i=-1; row+i>=0; i--)
            {
               if(chess_object->map[row+i][col]==who)
               {               
                  n++;
               }
               else
                  break;
            }
            for(i=1; row+i<CHESS_LEN_Y; i++)
            {
               if(chess_object->map[row+i][col]==who)
               {               
                  n++;
               }
               else
                  break;
            }
            if(n == 4)
            {             
               chess_object->who_win = who;
               return TRUE;
            }
            //fjie 2003.1.1
            if(n>4)
            {
               if(who==MAN)
               {
                  chess_object->who_win = MACHINE;
                  return TRUE;
               }
               if(who==MACHINE)
               {
                  chess_object->who_win = MAN;
                  return TRUE;
               }
            }//fjie end
            // left_up and right_down inspect
            for(n=0, i=-1, j=-1; row+i>=0 && col+j>=0; i--, j--)
            {
               if(chess_object->map[row+i][col+j]==who)
               {               
                  n++;
               }
               else
                  break;
            }
            for(i=1, j=1; row+i<CHESS_LEN_Y && col+j<CHESS_LEN_X; i++, j++)
            {
               if(chess_object->map[row+i][col+j]==who)
               {               
                  n++;
               }
               else
                  break;
            }
            if(n == 4)
            {            
               chess_object->who_win = who;
               return TRUE;
            }
            //fjie 2003.1.1
            if(n>4)
            {
               if(who==MAN)
               {
                  chess_object->who_win = MACHINE;
                  return TRUE;
               }
               if(who==MACHINE)
               {
                  chess_object->who_win = MAN;
                  return TRUE;
               }
            }//fjie end
            // left_down and right_up inspect
            for(n=0, i=1, j=-1; row+i<CHESS_LEN_Y && col+j>=0; i++, j--)
            {
               if(chess_object->map[row+i][col+j]==who)
               {               
                  n++;
               }
               else
                  break;
            }
            for(i=-1, j=1; row+i>=0 && col+j<CHESS_LEN_X; i--, j++)
            {
               if(chess_object->map[row+i][col+j]==who)
               {               
                  n++;
               }
               else
                  break;
            }
            if(n == 4)
            {            
               chess_object->who_win = who;
               return TRUE;
            }
            //fjie 2004.1.7
            if(n>4)
            {
               if(who==MAN)
               {
                  chess_object->who_win = MACHINE;
                  return TRUE;
               }
               if(who==MACHINE)
               {
                  chess_object->who_win = MAN;
                  return TRUE;
               }
            }
         }//if()
      }
   return FALSE;
}//JudgeWin()


///////////////////////////////////////////////
//callback of game over
//////////////////////////////////////////////
static void AlertDispCallBack(GmFive *pMe)
{

   (void)ISHELL_PostEvent(pMe->a.m_pIShell,
                          AEECLSID_GMFIVE,
                          EVT_COMMAND,
                          USRE_DRAW,
                          0);

}//AlertDispCallBack()

///////////////////////////////////////////////
//
///////////////////////////////////////////////
static void DrawGameOverScreen(GmFive *pMe)
{
   CHESS_OBJECT *chess_object = &pMe->m_chess_object;
   int16      strID;   
   MediaType  type = SUCCESS_MEIDA;

#ifdef  SOUND_FEATURE
   if(pMe->m_pIRingerMgr)
   {
      (void)IRINGERMGR_Stop(pMe->m_pIRingerMgr);
   }
#endif

   if(chess_object->who_win == MACHINE_WIN)
   {
      type = FAILURE_MEDIA;   
      if(pMe->m_forbidPieceFlg)
         strID = IDS_CHESS_RESULT_4;//你下了三三禁手，输棋了!
      else
         strID = IDS_CHESS_RESULT_1;      
   }
   else if(chess_object->who_win == MAN_WIN)
   {
      uint16 level = pMe->m_global_game_level;

      if(pMe->m_forbidPieceFlg)
         strID = IDS_CHESS_RESULT_5;//手机下三三禁手，你赢棋了!
      else
         strID = IDS_CHESS_RESULT_2;//普通提示
      //add chess level score information
      if(pMe->m_manTotalPiece < pMe->m_ChessCfgData.levelScore[level-1])
      {
         SaveChessLevelScore(pMe);
      }
   }//else if
   else
   {
      strID = IDS_CHESS_RESULT_3;
   }
   //added by tcl wgs 041217  
   if(pMe->m_toneFlag)
   {
       PlayerSound(pMe->m_promptVoice,type);  //player background music
   }
   //ended by tcl wgs 041217
   DispAlertInfo(pMe,IDS_CHESS_RESULT_TITLE,strID);

}//DrawGameOverScreen()

/*===========================================================================
   display alert information
===========================================================================*/
static void DispAlertInfo(GmFive *pMe,uint16 titleID,uint16 contentID)
{

   AEERect     dispRect;
   AECHAR      textBuf[MAX_CONTENT_SIZE];
   RGBVAL      oldTextColor;
   if(pMe == NULL)
      return ;

   DrawCtrlFrame(pMe,titleID,&pMe->m_frameRect);
   (void)MEMCPY(&dispRect,&pMe->m_frameRect,sizeof(AEERect));

   oldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, MAKE_RGB(0,0,0));
   ISTATIC_SetProperties(pMe->m_pStaticCtrl, ST_CENTERTEXT|ST_CENTERTITLE |ST_MIDDLETEXT);
   ISTATIC_SetRect(pMe->m_pStaticCtrl, &dispRect);

   ISHELL_LoadResString(pMe->a.m_pIShell,
                        GMFIVE_RES_FILE_LANG,
                        contentID,
                        textBuf,
                        sizeof(textBuf));
   ISTATIC_SetText(pMe->m_pStaticCtrl,NULL, textBuf, AEE_FONT_NORMAL, AEE_FONT_NORMAL );
   ISTATIC_Redraw(pMe->m_pStaticCtrl);
   (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, oldTextColor);
   ISHELL_SetTimer(pMe->a.m_pIShell,
                  ALERT_DISPLAY_TIMER,
                  (PFNNOTIFY) AlertDispCallBack,
                  pMe);

}//DispAlertInfo()


/*===========================================================================
   This function add '\n' add new line
===========================================================================*/
void AddNewLine(AECHAR * szText, AECHAR chDest, AECHAR chSrc)
{
	AECHAR * pTemp = WSTRCHR(szText,chDest);

	while (pTemp)
	{
		*pTemp = chSrc;
		pTemp = WSTRCHR(szText,chDest);
	}
}//AddNewLine()

/*===========================================================================
   This function adds control reset
===========================================================================*/
static void GmFive_DispHelpInforScreen(GmFive* pMe)
{
   AECHAR   szBuf[MAX_HELP_INFO_SIZE*2];
   AECHAR   szTitle[16];
   AEERect  rc;   

   GmFive_Reset(pMe);
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);      
   
   SETAEERECT(&rc, 0, 0, pMe->m_cxScreen, pMe->m_cyScreen - GMFIVE_SOTFKEY_CY);
   
   ISTATIC_SetRect(pMe->m_pStaticCtrl, &rc);   
   ISTATIC_SetProperties(pMe->m_pStaticCtrl, ST_CENTERTITLE | ST_UNDERLINE | ST_NOSCROLL);   
   //title name 
   ISHELL_LoadResString(pMe->a.m_pIShell, GMFIVE_RES_FILE_LANG, IDS_HELP_TITLE, szTitle, sizeof(szTitle));    
   //content
   ISHELL_LoadResString(pMe->a.m_pIShell, GMFIVE_RES_FILE_LANG, IDS_HELP_CONTENT, szBuf, sizeof(szBuf));
   AddNewLine(szBuf, (AECHAR)'\\', (AECHAR)'\n');//在资源文件中以\表示需要换行
   ISTATIC_SetText(pMe->m_pStaticCtrl, szTitle, szBuf, AEE_FONT_BOLD, AEE_FONT_NORMAL);

   ISTATIC_Redraw(pMe->m_pStaticCtrl);
   ISTATIC_SetActive(pMe->m_pStaticCtrl, TRUE);   
#if defined(AEE_STATIC) 
    DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
#else   
   //add softkey prompt
   pMe->m_softkeyPrompt.leftItemID  = NULL;
   pMe->m_softkeyPrompt.middleItemID = NULL;
   pMe->m_softkeyPrompt.rightItemID  = IDS_MENU_RETURN;
   GmFive_AddSoftkeyPrompt(pMe,&pMe->m_softkeyPrompt);
   IDISPLAY_Update(pMe->a.m_pIDisplay);
#endif    
}//GmFive_DispHelpInforScreen()

/*===========================================================================
   This function adds control reset
===========================================================================*/
void ClearPiece(GmFive* pMe, int16 col, int16 row)
{
   AEECircle   tempCircle;
   AEELine     tempLine;

   if(!IGRAPHICS_GetFillMode(pMe->m_pGraphics))
   {
      (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics,TRUE);
   }
   IGRAPHICS_SetFillColor(pMe->m_pGraphics,BACKGD_R_COLOR,BACKGD_G_COLOR,BACKGD_B_COLOR,0);
   IGRAPHICS_SetColor(pMe->m_pGraphics,BACKGD_R_COLOR,BACKGD_G_COLOR,BACKGD_B_COLOR,0);

   tempCircle.cx = col*GRID_WIDTH +X_OFF;
   tempCircle.cy = row*GRID_HEIGHT+Y_OFF;

   tempCircle.r  = PIECE_WIDTH/2;
   (void)IGRAPHICS_DrawCircle(pMe->m_pGraphics,&tempCircle);

   IGRAPHICS_SetColor(pMe->m_pGraphics,GRID_R_COLOR,GRID_G_COLOR,GRID_B_COLOR,0);
   //clear horizontal line
   tempLine.sx = tempCircle.cx - tempCircle.r;
   tempLine.sy = tempCircle.cy;
   tempLine.ex = tempCircle.cx + tempCircle.r;
   tempLine.ey = tempCircle.cy;
   (void)IGRAPHICS_DrawLine(pMe->m_pGraphics,&tempLine);

   //clear vertical line
   tempLine.sx = tempCircle.cx ;
   tempLine.sy = tempCircle.cy - tempCircle.r;
   tempLine.ex = tempCircle.cx;
   tempLine.ey = tempCircle.cy + tempCircle.r;
   (void)IGRAPHICS_DrawLine(pMe->m_pGraphics,&tempLine);
   if((col <= 0 )|| (col >= CHESS_LEN_X-1))
   {
      IGRAPHICS_SetColor(pMe->m_pGraphics,BACKGD_R_COLOR,BACKGD_G_COLOR,BACKGD_B_COLOR,0);
      //clear horizontal line
      if(col <= 0)
         tempLine.sx = tempCircle.cx - tempCircle.r;
      else
         tempLine.sx = tempCircle.cx+1;
      tempLine.sy = tempCircle.cy;
      if(col >= CHESS_LEN_X-1)
         tempLine.ex = tempCircle.cx + tempCircle.r;
      else
         tempLine.ex = tempCircle.cx-1;
      tempLine.ey = tempCircle.cy;
      (void)IGRAPHICS_DrawLine(pMe->m_pGraphics,&tempLine);
   }
   if((row <= 0)||(row >= CHESS_LEN_Y-1))
   {
      IGRAPHICS_SetColor(pMe->m_pGraphics,BACKGD_R_COLOR,BACKGD_G_COLOR,BACKGD_B_COLOR,0);
      //clear vertical line

      tempLine.sx = tempCircle.cx ;
      if(row <= 0)
         tempLine.sy = tempCircle.cy - tempCircle.r;
      else
         tempLine.sy = tempCircle.cy+1;
      tempLine.ex = tempCircle.cx;
      if(row >= CHESS_LEN_Y-1)
         tempLine.ey = tempCircle.cy + tempCircle.r;
      else
         tempLine.ey = tempCircle.cy-1;
      (void)IGRAPHICS_DrawLine(pMe->m_pGraphics,&tempLine);
   }

}//ClearPiece()

/*===========================================================================
   This function adds control reset
===========================================================================*/
static boolean MoveFromRepentQueue(GmFive* pMe)
{
   int16 man_x, man_y, machine_x, machine_y;
   CHESS_OBJECT *chess_object = &pMe->m_chess_object;

   if( chess_object->repent_queue.tail_ptr_var == chess_object->repent_queue.head_point
       || chess_object->repent_queue.tail_point == chess_object->repent_queue.head_point
     )
   {
      return FALSE;  // queue is null.
   }

   if(chess_object->repent_queue.tail_ptr_var == -1)
   {
      chess_object->repent_queue.tail_ptr_var = chess_object->repent_queue.tail_point;
   }

   chess_object->repent_queue.tail_ptr_var--;
   if(chess_object->repent_queue.tail_ptr_var < 0)
   {
      chess_object->repent_queue.tail_ptr_var = REPENT_LEN - 1;
   }

   machine_x = chess_object->repent_queue.machine_x[chess_object->repent_queue.tail_ptr_var];
   machine_y = chess_object->repent_queue.machine_y[chess_object->repent_queue.tail_ptr_var];
   chess_object->map[machine_y][machine_x] = 0;
   ClearPiece(pMe, machine_x, machine_y);//machine chess
   man_x = chess_object->repent_queue.man_x[chess_object->repent_queue.tail_ptr_var];
   man_y = chess_object->repent_queue.man_y[chess_object->repent_queue.tail_ptr_var];
   chess_object->map[man_y][man_x] = 0;
   ClearPiece(pMe, man_x, man_y);  //man chess

   chess_object->put_chess_counter--;
   if(--chess_object->repent_queue.total == 0)
   {
      chess_object->begin_flag = MAN;
   }

   IGRAPHICS_Update(pMe->m_pGraphics);
   IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,FALSE);
   return TRUE;
}//MoveFromRepentQueue()

/*===========================================================================
   redraw the just game state
===========================================================================*/
static void ResumeJustChess(GmFive* pMe)
{

   int16  row,col;
   char   who;
   int16  pos_x,pos_y;

   GmFive_Reset(pMe);  //make IMenuCtl control inefficacy
   DrawScreenBackground(pMe);
   GmFive_DrawBoard(pMe);
#ifdef AEE_STATIC
   SetSystemKeyBeepVol(pMe,FALSE);//close key beep voice
#endif //AEE_STATIC

   if(pMe->m_chess_object.who_go == MAN)
   {
      DrawCurSquare(pMe);
   }

   for(row = 0; row < CHESS_LEN_Y; row++)
   {
      for(col = 0; col < CHESS_LEN_X; col++)
      {
         who = pMe->m_chess_object.map[row][col];
         if(who!= 0 )
         {
            pos_x = col*GRID_WIDTH +X_OFF;
            pos_y = row*GRID_HEIGHT+Y_OFF;
            if(who == MAN)
               DrawPiece(pMe,pos_x, pos_y, BLACK_CHESS);
            else
               DrawPiece(pMe,pos_x, pos_y, WHITE_CHESS);

         }//if()
      }//for()
   }//for()
   DrawCurSquare(pMe);           //resume the man chess square
   MachineDrawDifColrPiece(pMe); //resume machine chess piece
   IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,FALSE); //CB05-1 test
#ifdef  SOUND_FEATURE
   if(pMe->m_pIRingerMgr)
   {
      char szFile[MAX_RING_LEN] = {'\0'};

      if(pMe->m_playFile[0] != '\0')
      {
         if((pMe->m_ringerSelItemID >= pMe->m_dloadRingerID)&&(pMe->m_dloadRingerID != 0))
         {
            STRCPY(szFile, DOWNLOAD_RIN_DIR);
         }
         else
         {
            STRCPY(szFile, GMFIVE_RING_DIR);
         }
         STRCAT(szFile, DIRECTORY_STR);
         STRCAT(szFile, pMe->m_playFile);
         //TCL wangbin040817 mod
     #if defined(AEE_STATIC)
         if(pMe->m_soundMode == IDS_SOUND_OPEN_MODE && pMe->m_RingVol != OEMSOUND_MUTE_VOL)
     #else
         if(pMe->m_soundMode == IDS_SOUND_OPEN_MODE)    
     #endif
         {
            (void)IRINGERMGR_Stop(pMe->m_promptVoice);
            (void)IRINGERMGR_PlayFile(pMe->m_pIRingerMgr,szFile,1000);
         }
      }
   }//if(pMe->m_pIRingerMgr)
#endif //SOUND_FEATURE
	 DrawExitButton(pMe);  //tcl wgs 050111 added
	 DrawSoundButton(pMe);  //tcl wgs 050111 added 
}//ResumeJustChess()


/*===========================================================================
This function sets the menu attributes based on BREW Style Sheet for
menu control.
set popMenu color option
===========================================================================*/
static void SetPopMenuAttr(GmFive *pMe,uint16 titleID, AEERect * pRect, uint32 dwProps)
{
   
   IDisplay*         pIDisplay = pMe->a.m_pIDisplay;
   IMenuCtl*         pMenu     = pMe->m_pMenuCtl;
   AEERect           tempRect;

   
#ifdef AEE_STATIC   // Menu Style
   IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE);
#else
   AEEItemStyle      sel, normal;
   AEEMenuColors     col;
   normal.ft = MENU8_FT;
   normal.xOffset = 0;
   normal.yOffset = 0;
   normal.roImage = MENU8_RO;

   sel.ft = POPMENU_SELECT_FT;
   sel.xOffset = 0;
   sel.yOffset = 0;
   sel.roImage = MENU8_SELECT_RO;
   // Menu Colors
   col.cSelText =    POPMENU_SELECT_TEXT;
   col.cText    =    POPMENU_UNSELECT_TEXT;
   col.wMask    =    MENU8_COLOR_MASK;
   col.cFrame   =    POPMENU_FRAME_LINE;
   col.cBack    =    POPMENU_BACKGROUND;
   col.cSelBack =    POPMENU_SELECT_BACKGROUND;
   col.cScrollbar =  MENU_SCROOL_BAR;
   col.cScrollbarFill = MENU_BAR_FILLCOLOR;

   dwProps |= IMENUCTL_GetProperties(pMenu);
   dwProps |= MP_UNDERLINE_TITLE | MP_WRAPSCROLL;
   IMENUCTL_SetStyle(pMenu, &normal, &sel);
   IMENUCTL_SetColors(pMenu, &col);
   IMENUCTL_SetProperties(pMenu, dwProps);
#endif //AEE_STATIC

   (void)MEMCPY(&tempRect,pRect,sizeof(AEERect));
   DrawCtrlFrame(pMe,titleID,pRect);
   IDISPLAY_EraseRect(pIDisplay,pRect);
   tempRect.x  += 3;
   tempRect.y  += 3;
   tempRect.dx -= 6;
   tempRect.dy -= 3;   
   if (pRect)
   {
      IMENUCTL_SetRect(pMenu, &tempRect);
   }
}//SetPopMenuAttr()

/*===========================================================================
   get chess config data
===========================================================================*/
static void GetChessConfigData(GmFive* pMe)
{
   if(ISHELL_GetPrefs(pMe->a.m_pIShell,
                      AEECLSID_GMFIVE,
                      GAME_VERSION,
                      &(pMe->m_ChessCfgData),
                      sizeof(CPrefData))!= SUCCESS)
   {
      uint16  i;

      for(i= 0;i < MAX_LEVEL;i++)
         pMe->m_ChessCfgData.levelScore[i] = MAX_CHESS_PIECE;   
      pMe->m_ChessCfgData.who_go    = MAN;
      pMe->m_ChessCfgData.level     = INIT_LEVEL;

   }
}//GetChessConfigData()

/*===========================================================================
   save chess config data
===========================================================================*/
static void SaveChessConfigData(GmFive* pMe)
{
   if(pMe == NULL) return;

   pMe->m_ChessCfgData.level     = pMe->m_global_game_level;
   pMe->m_ChessCfgData.who_go    = pMe->m_global_who_go;

   (void)ISHELL_SetPrefs(pMe->a.m_pIShell,
                         AEECLSID_GMFIVE,
                         GAME_VERSION,
                         &(pMe->m_ChessCfgData),
                         sizeof(CPrefData));
}//SaveChessConfigData()

/*===========================================================================
   set chess level hero score
===========================================================================*/
static void SaveChessLevelScore(GmFive* pMe)
{
   uint16 level = pMe->m_global_game_level;

   pMe->m_ChessCfgData.levelScore[level-1] = pMe->m_manTotalPiece;

}//SaveChessLevelScore()

/*===========================================================================
   drawtext hero score of chess level
===========================================================================*/
static void DrawHeroScoreText(GmFive* pMe,AEERect *textRect)
{

   IDisplay*   pd =  pMe->a.m_pIDisplay;
   IShell*     ps  = pMe->a.m_pIShell;
   char        szFormat[] = "%d";
   AECHAR      wstrFor[10];
   AECHAR      textBuf[MAX_CONTENT_SIZE];
   AECHAR      szContent[MAX_CONTENT_SIZE];
   AECHAR      szTile[MAX_CONTENT_SIZE];
   int16       i;
   RGBVAL      oldTextColor;

   IDISPLAY_EraseRect(pd,textRect);

   //disp title of hero
   oldTextColor = IDISPLAY_SetColor(pd, CLR_USER_TEXT, MAKE_RGB(0,0,255));

   ISHELL_LoadResString(ps,GMFIVE_RES_FILE_LANG, IDS_HERO_SCORE_TITLE, szTile,sizeof(szTile));
   (void)IDISPLAY_SetColor(pd, CLR_USER_TEXT, MAKE_RGB(0,0,128));
   (void)IDISPLAY_DrawText(pd,
                           AEE_FONT_NORMAL,
                           szTile,
                           -1,
                           textRect->x,
                           textRect->y,
                           NULL,
                           NULL);

   STRTOWSTR(szFormat,wstrFor,10);
   for(i= 0; i<MAX_LEVEL;i++)
   {

      ISHELL_LoadResString(pMe->a.m_pIShell,GMFIVE_RES_FILE_LANG,(int16)(IDS_GAME_LEVEL_1+i), textBuf, sizeof(textBuf));
      WSPRINTF(szContent,sizeof(szContent),wstrFor,pMe->m_ChessCfgData.levelScore[i]);
      //display name of level
      (void)IDISPLAY_DrawText(pd,
                              AEE_FONT_NORMAL,
                              textBuf,
                              -1,
                              (textRect->x+6),
                              (textRect->y+2+(pMe->m_nLChSize)*(1+i)),
                              NULL,
                              NULL);
      //display score of level
      (void)IDISPLAY_DrawText(pd,
                              AEE_FONT_NORMAL,
                              szContent,
                              -1,
                              (textRect->x+pMe->m_nLChSize*3+6),
                              (textRect->y+(pMe->m_nLChSize)*(1+i)),
                              NULL,
                              NULL);
   }//for()
   (void)IDISPLAY_SetColor(pd, CLR_USER_TEXT, oldTextColor);   

}//DrawHeroScoreText()

/*===========================================================================
   display hero score of chess level
===========================================================================*/
static void DispHeroScoreScreen(GmFive* pMe)
{

   AEERect     dispRect;
   AEERect     textRect;
   AEERect     softKeyRect;
   uint16      cx = pMe->m_cxScreen;
   uint16      cy = pMe->m_cyScreen;
   IDisplay*   pd = pMe->a.m_pIDisplay;

   GmFive_Reset(pMe);
   SETAEERECT(&dispRect, 7, cy/5, (cx -14), (pMe->m_nLChSize*7+2));


   IDISPLAY_EraseRect(pd,&dispRect);
   DrawCtrlFrame(pMe,IDS_HERO_SCORE,&dispRect);

   textRect.x  = dispRect.x;
   textRect.y  = dispRect.y;
   textRect.dx = dispRect.dx;
   textRect.dy = dispRect.dy - GMFIVE_SOTFKEY_CY - 5;
   (void)MEMCPY(&pMe->m_TextRect,&textRect,sizeof(AEERect));
   DrawHeroScoreText(pMe,&dispRect);
   //set sofkey rect area
   softKeyRect.x  = dispRect.x;
   softKeyRect.y  = dispRect.dy+16;
   softKeyRect.dx = dispRect.dx;
   softKeyRect.dy = GMFIVE_SOTFKEY_CY;
   //add softkey prompt 
   Brew_AddSoftItem(pMe->m_pSoftKeyCtl,GMFIVE_RES_FILE_LANG,&softKeyRect, IDS_HERO_OK, IDS_HERO_OK, IDS_HERO_RESUME_INIT, IDS_HERO_RESUME_INIT);      
}//DispHeroScoreScreen()

/*===========================================================================
   resume the hero init value
===========================================================================*/
static void ResumeHeroInitValue(GmFive* pMe)
{
    uint16  i;
    
    for(i= 0;i < MAX_LEVEL;i++)
    {
        pMe->m_ChessCfgData.levelScore[i] = MAX_CHESS_PIECE;
    }
    
    DrawHeroScoreText(pMe,&pMe->m_TextRect);
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,FALSE);
    
    (void)ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_GMFIVE,EVT_COMMAND, USRE_DRAW, 0);
}//ResumeHeroInitValue()

#ifdef SOUND_FEATURE

/*===========================================================================
   get fileName postfix
===========================================================================*/
static boolean CheckPostfix(const char * fileName)
{
   char *   psz;
   char     szExt[10];

   psz = STRRCHR(fileName, '.');
   if (!psz)  return FALSE;

   psz++;
   if (!*psz) return FALSE;

   if(STRLEN(psz) <= 4 && STRLEN(psz) >= 3)
   {
      STRCPY(szExt, psz);
      if((STRCMP(STRLOWER(szExt),"mid") == 0)||(STRCMP(STRLOWER(szExt),"midi") == 0)||
       /*  (STRCMP(STRLOWER(szExt),"mp3") == 0)||*/(STRCMP(STRLOWER(szExt),"mmf")  == 0))
      {   
         return TRUE;
      }
   }
   return FALSE;
}

/*===========================================================================
   get ringer file name
===========================================================================*/
static char * GmFive_GetFileName(const char * psz)
{
   char *   pszName = STRRCHR(psz, (int)DIRECTORY_CHAR);

   if (pszName)
      pszName++;
   else
      pszName = (char *)psz;

   return pszName;
}//GmFive_GetFileName()

/*===========================================================================
   set sound mode
===========================================================================*/
static void GMFive_SetSoundModeScreen(GmFive *pMe)
{
   IDisplay *pd =  pMe->a.m_pIDisplay;
   uint16    wImageID = NULL;
   uint16    itemID;
   uint16    i;
   uint16    selItemID = IDS_SOUND_CLOSE_MODE;

   if (pMe == NULL)
      return ;

   GmFive_Reset(pMe);
   SetPopMenuAttr(pMe,IDS_SOUND_SET,&pMe->m_frameRect,MP_MAXSOFTKEYITEMS);
   for(i = 0;i < MAX_CHESS_MODE;i++)
   {
      itemID = i+ IDS_SOUND_CLOSE_MODE;
      if(pMe->m_soundMode == itemID)
      {
         wImageID = IDB_GAME_RADIO_SEL;
         selItemID = itemID;
      }
      else
      {
         wImageID = IDB_GAME_RADIO_UNSEL;
      }
      (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,itemID,  NULL, wImageID,itemID,0); //machine play first
   }
   IMENUCTL_SetSel(pMe->m_pMenuCtl,selItemID);
   IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
   IMENUCTL_Redraw(pMe->m_pMenuCtl);
   IDISPLAY_Update(pd);


}//GMFive_SetSoundModeScreen()

/*===========================================================================
   show ringer of background
===========================================================================*/
static boolean GMFive_ShowRingViewScreen(GmFive *pMe)
{
	IFileMgr * pIFileMgr = NULL;
	uint16     wItemID = 0;
	FileInfo   fi;
	AEERect	   rc;
	AECHAR     szBuf[MAX_RING_LEN];
	uint16     wImageID;
	uint16     i = 0;
	int       dx = pMe->m_cxScreen;
    int       dy = pMe->m_cyScreen;
    
    int16     startPoint;

	if(!CreateObj(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr))
	{
		return FALSE;
	}

	if((IFILEMGR_Test(pIFileMgr,GMFIVE_RING_DIR) != SUCCESS)&&
	   (IFILEMGR_Test(pIFileMgr,DOWNLOAD_RIN_DIR) != SUCCESS))
	{
	   ReleaseObj((void **)&pIFileMgr);
	   return FALSE;
	}

	//reset all control 
	GmFive_Reset(pMe);
	
	// Enumerate ring directory
	pMe->m_dloadRingerID = 0;
	IFILEMGR_EnumInit(pIFileMgr, GMFIVE_RING_DIR, FALSE);
	while((wItemID < GMFIVE_MAX_FILES_NUM)&& IFILEMGR_EnumNext(pIFileMgr, &fi))
	{
        char * szName;

        // Add only the file name
        szName = GmFive_GetFileName(fi.szName);
        if(!CheckPostfix(szName)) continue;

        MEMSET(szBuf,0,sizeof(szBuf));
        (void)UTF8TOWSTR((byte *)szName, STRLEN(szName)+1,szBuf,sizeof(szBuf));
        if(pMe->m_ringerSelItemID == (wItemID+BASE_SOUND_ITEM_ID))
            wImageID = IDB_GAME_RADIO_SEL;
        else
            wImageID = IDB_GAME_RADIO_UNSEL;
        (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,NULL,szBuf, wImageID,(uint16)(wItemID+BASE_SOUND_ITEM_ID),0); //game new
        wItemID++;
	}//while()

	//list download ringers
	IFILEMGR_EnumInit(pIFileMgr, DOWNLOAD_RIN_DIR, FALSE);
	while((wItemID < GMFIVE_MAX_FILES_NUM)&& IFILEMGR_EnumNext(pIFileMgr, &fi))
	{
        char * szName;

        // Add only the file name
        szName = GmFive_GetFileName(fi.szName);
        if(!CheckPostfix(szName)) continue;

        MEMSET(szBuf,0,sizeof(szBuf));
        (void)UTF8TOWSTR((byte *)szName, STRLEN(szName)+1, szBuf, sizeof(szBuf));
        if(pMe->m_ringerSelItemID == (wItemID+BASE_SOUND_ITEM_ID))
            wImageID = IDB_GAME_RADIO_SEL;
        else
            wImageID = IDB_GAME_RADIO_UNSEL;
        (void)Brew_AddMenuItem(pMe->m_pMenuCtl,GMFIVE_RES_FILE_LANG,NULL,szBuf, wImageID,(uint16)(wItemID+BASE_SOUND_ITEM_ID),0); //game new
        if(i == 0)
        {
            pMe->m_dloadRingerID = wItemID+BASE_SOUND_ITEM_ID;
            i++;
        }
        wItemID++;
	}//while()

	ReleaseObj((void **)&pIFileMgr);
	//not any ringer file
	if(wItemID == 0)
	{
		return FALSE;
	}			
	
	//clean the all screen
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);	
	//draw the title text and background 
    startPoint = DrawTopBackGroundBMP(pMe,IDS_BACKRINGER_SELECT,NULL);
    
    dy -= (GMFIVE_SOTFKEY_CY  + startPoint) ;//for softkey prompt
    SETAEERECT(&rc, 1, startPoint, (dx - 2), dy );
	(void)Brew_SetMenuAttr(pMe->m_pMenuCtl,AEECLSID_MENUCTL,&rc,MP_MAXSOFTKEYITEMS);//set menu attible
	
    IMENUCTL_SetSel(pMe->m_pMenuCtl,pMe->m_ringerSelItemID);
    IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
    IMENUCTL_Redraw(pMe->m_pMenuCtl);
    pMe->m_eAppState = APP_STATE_SELECT_RINGER;
#if defined(AEE_STATIC) 
    DRAW_PROMPT_BAR(BTBAR_SELECT_PLAY_BACK);
#else    
    //add softkey prompt
    pMe->m_softkeyPrompt.leftItemID   = IDS_RINGER_SELECT;
    pMe->m_softkeyPrompt.middleItemID = IDS_PRE_LISTEN_RINGER;
    pMe->m_softkeyPrompt.rightItemID  = IDS_MENU_RETURN;
    GmFive_AddSoftkeyPrompt(pMe,&pMe->m_softkeyPrompt);
#endif     
    return TRUE;

}//GMFive_ShowRingViewScreen()

/*===========================================================================
   prelisten the current sel ringer
===========================================================================*/

static void PreListenRinger(GmFive *pMe)
{
    CtlAddItem ai={0};
    uint16 nSelect;
    char tempFile[MAX_RING_LEN]={'\0'};
    char szFile[MAX_RING_LEN] = {'\0'};


    nSelect = IMENUCTL_GetFocus(pMe->m_pMenuCtl);
    IMENUCTL_GetItem(pMe->m_pMenuCtl, nSelect, &ai);
    (void)WSTRTOUTF8(ai.pText,WSTRLEN(ai.pText),(byte *)tempFile,sizeof(tempFile));

    if((nSelect >= pMe->m_dloadRingerID)&&(pMe->m_dloadRingerID != 0))
    {
        STRCPY(szFile, DOWNLOAD_RIN_DIR);
    }
    else
    {
        STRCPY(szFile, GMFIVE_RING_DIR);
    }

    STRCAT(szFile, DIRECTORY_STR);
    STRCAT(szFile, tempFile);
#if defined(AEE_STATIC)
    if(pMe->m_pIRingerMgr && pMe->m_RingVol != OEMSOUND_MUTE_VOL)    
#else    
    if(pMe->m_pIRingerMgr)
#endif
    {
        //byte  RingVol;
        (void)IRINGERMGR_Stop(pMe->m_promptVoice);
        (void)IRINGERMGR_Stop(pMe->m_pIRingerMgr);
#ifdef AEE_STATIC
        //初始化音量
        /*(void) ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_RINGER_VOL,
                          &RingVol,
                          sizeof(byte));
        CRingerMgr_SetVol( pMe->m_pIRingerMgr, 
                           GET_ISOUND_VOL_LEVEL((uint8) RingVol), 
                           TRUE);
         */                  
#endif  //AEE_STATIC                           
        (void)IRINGERMGR_PlayFile(pMe->m_pIRingerMgr,szFile,1000);
    }

}//PreListenRinger()

/*===========================================================================
   get menu item data
===========================================================================*/
static void GetMenuItemData(GmFive *pMe)
{
	CtlAddItem ai;
	uint16 nSelect;
	char pFileName[MAX_RING_LEN]={'\0'};

   // Initialize CtlAddItem
	MEMSET(&ai, 0, sizeof(ai));

	//gain current selected menu item
	nSelect = IMENUCTL_GetFocus(pMe->m_pMenuCtl);
	pMe->m_ringerSelItemID = nSelect;
	if(pMe->m_ringerSelItemID > 0)
	{
	   pMe->m_soundMode = IDS_SOUND_OPEN_MODE;//open the mode of ringer play
	}

	IMENUCTL_GetItem(pMe->m_pMenuCtl, nSelect, &ai);
	(void)WSTRTOUTF8(ai.pText,WSTRLEN(ai.pText),(byte *)pFileName,sizeof(pFileName));
	STRCPY(pMe->m_playFile, pFileName);
	(void)ISHELL_PostEvent(pMe->a.m_pIShell,
                          AEECLSID_GMFIVE,
                          EVT_COMMAND,
                          USRE_DRAW,
                          0);
}//GetMenuItemData()

#endif //SOUND_FEATURE

/*===========================================================================
   draw frame of menu or static
===========================================================================*/
static void DrawCtrlFrame(GmFive *pMe,uint16 titleID,AEERect *fRect)
{

   AEERect frameRect;
   AEERect tempRect;
   AECHAR  szTile[32];
   RGBVAL  oldTextColor;


   if(!IGRAPHICS_GetFillMode(pMe->m_pGraphics))
   {
      (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics,TRUE);
   }
   IGRAPHICS_SetFillColor(pMe->m_pGraphics,205,205,205,0);
   IGRAPHICS_SetColor(pMe->m_pGraphics,0,0,0,0);
   //draw window frame

   frameRect.x  = fRect->x-5;
   frameRect.y  = fRect->y-18;
   frameRect.dx = fRect->dx+10;
   frameRect.dy = fRect->dy+22;
   (void)MEMCPY(&tempRect,&frameRect,sizeof(AEERect));

   (void)IGRAPHICS_DrawRoundRectangle(pMe->m_pGraphics,&frameRect,15,15);
   oldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, MAKE_RGB(0,0,255));
   ISHELL_LoadResString(pMe->a.m_pIShell,
                        GMFIVE_RES_FILE_LANG,
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
                           IDF_ALIGN_CENTER|IDF_ALIGN_TOP|IDF_TEXT_TRANSPARENT);

   (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, oldTextColor);
   IGRAPHICS_Update(pMe->m_pGraphics);
}//DrawCtrlFrame()

//mod by tcl wangbin040809 for del Globe variable "gKeyBeepVolumeCache"
#if defined(AEE_STATIC)
/*===========================================================================
   set system key beep to vol
   wParam
   pMe:
   resume: TRUE is resume system set beep vol or set beep vol min
===========================================================================*/
static void SetSystemKeyBeepVol(GmFive *pMe,boolean resume)
{
   byte         vol = OEMSOUND_MUTE_VOL;
   byte         KeyBeepVol;

   if(pMe == NULL) return ;

   if(resume)
      KeyBeepVol = pMe->m_beepVol;
   else
      KeyBeepVol = vol;

   (void) ICONFIG_SetItem(pMe->m_pConfig,
                          CFGI_BEEP_VOL,
                          &KeyBeepVol,
                          sizeof(byte));
} //SetSystemKeyBeepVol()

//end tcl wangbin040809
#else

/*===========================================================================
   display softkey prompt
   wParam
   pMe:
   leftItemID:  left softkey resource id
   rightItemID: right softkey resource id
   softkeyRect: softkey rect area
===========================================================================*/
static void GmFive_AddSoftkeyPrompt(GmFive *pMe,SoftkeyPrompt *prompt)
{
    AECHAR    textBuf[16] = {'\0'};
    AEERect   softkeyRect;
    AEEImageInfo  ImageInfo;
	IImage*   pBottomBmp = NULL;

    if (pMe == NULL) return;

    softkeyRect.x  = 0;
    softkeyRect.y  = pMe->m_cyScreen - GMFIVE_SOTFKEY_CY;
    softkeyRect.dx = pMe->m_cxScreen;
    softkeyRect.dy = GMFIVE_SOTFKEY_CY;

    pBottomBmp = ISHELL_LoadResImage( pMe->a.m_pIShell, GMFIVE_RES_FILE_LANG, IDB_SOFTKEY_BUTTOM );
    if ( pBottomBmp )
    {
        AEEImageInfo pi;

        IIMAGE_GetInfo(pBottomBmp, &pi);
        IIMAGE_Draw(pBottomBmp, softkeyRect.x, pMe->m_cyScreen - pi.cy);
        if(pMe->m_cxScreen > pi.cx)
        {
            IIMAGE_SetOffset(pBottomBmp,(pMe->m_cxScreen - pi.cx),0);
            IIMAGE_Draw(pBottomBmp, pi.cx, pMe->m_cyScreen - pi.cy);
        }

        IIMAGE_Release( pBottomBmp );
        pBottomBmp = NULL;
    }

    // Get image information
    if (pMe->m_pBottomImage)
    {
        IIMAGE_GetInfo( pMe->m_pBottomImage, &ImageInfo );
    }

    //draw left softkey text
    if (prompt->leftItemID)
    {
        if(pMe->m_pBottomImage)
        {
            // Draw the image in the center of the screen
            IIMAGE_Draw(pMe->m_pBottomImage,softkeyRect.x,pMe->m_cyScreen - ImageInfo.cy);
        }

        ISHELL_LoadResString(pMe->a.m_pIShell,GMFIVE_RES_FILE_LANG,prompt->leftItemID, textBuf, sizeof(textBuf));
        (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, textBuf,-1,(softkeyRect.x + 4),(int)(softkeyRect.y +3),NULL,IDF_TEXT_TRANSPARENT);        
        
    }

    if (prompt->middleItemID)
    {
        if (pMe->m_pBottomImage)
        {
            // Draw the image in the center of the screen
            IIMAGE_Draw(pMe->m_pBottomImage,(softkeyRect.dx - ImageInfo.cx)/2,pMe->m_cyScreen - ImageInfo.cy);
        }

        ISHELL_LoadResString(pMe->a.m_pIShell,GMFIVE_RES_FILE_LANG,prompt->middleItemID, textBuf, sizeof(textBuf));
        (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, textBuf,-1,(int)(softkeyRect.dx/2 - 16),(int)(softkeyRect.y+3),NULL,IDF_TEXT_TRANSPARENT);        
    }

    //draw right softkey text
    if (prompt->rightItemID)
    {
        if (pMe->m_pBottomImage)
        {
            // Draw the image in the center of the screen
            IIMAGE_Draw(pMe->m_pBottomImage,(softkeyRect.dx - ImageInfo.cx),pMe->m_cyScreen - ImageInfo.cy);
        }

        ISHELL_LoadResString(pMe->a.m_pIShell,GMFIVE_RES_FILE_LANG,prompt->rightItemID, textBuf, sizeof(textBuf));
        (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, textBuf,-1,(int)(softkeyRect.dx - 38),(int)(softkeyRect.y+3),NULL,IDF_TEXT_TRANSPARENT);        
    }

    IDISPLAY_Update( pMe->a.m_pIDisplay );

}//GmFive_AddSoftkeyPrompt()

/*===========================================================================
   redraw softkey prompt
   wParam
   pMe:
===========================================================================*/
static void RedrawSoftKeyPrompt(GmFive *pMe,uint16 leftID,uint16 middleID,uint16 rightID)
{
    if(pMe == NULL) return;

    pMe->m_softkeyPrompt.leftItemID   = NULL;
    pMe->m_softkeyPrompt.middleItemID = NULL;
    pMe->m_softkeyPrompt.rightItemID  = NULL;    
    if(leftID)
    {
        pMe->m_softkeyPrompt.leftItemID   = leftID;        
    }    
    if(middleID)
    {
        pMe->m_softkeyPrompt.middleItemID = middleID;        

    }
    if(rightID)
    {
        pMe->m_softkeyPrompt.rightItemID  = rightID;        
    }
        
    GmFive_AddSoftkeyPrompt(pMe,&pMe->m_softkeyPrompt);
}//RedrawSoftKeyPrompt()
#endif // AEE_STATIC

/*===========================================================================
   draw the exit button during the game
===========================================================================*/
static void DrawExitButton(GmFive *pMe)
{
    IImage* pExitButton;

    if (pMe == NULL) return ;


    pExitButton = ISHELL_LoadResImage(pMe->a.m_pIShell,GMFIVE_RES_FILE_LANG,IDB_GAME_EXIT);
    if (pExitButton)
    {
        AEEImageInfo  ImageInfo;

        IIMAGE_SetParm(pExitButton,IPARM_ROP, AEE_RO_TRANSPARENT, 0);
        IIMAGE_GetInfo(pExitButton, &ImageInfo);
        IIMAGE_Draw(pExitButton,(pMe->m_cxScreen/3 + ImageInfo.cx + 6),(pMe->m_cyScreen - (ImageInfo.cy + PROMPT_INFO_HEIGHT)/2 - 2));
        IIMAGE_Release( pExitButton );
        pExitButton = NULL;
    }
} //DrawExitButton()


/*===========================================================================
   draw the Sound button during the game
===========================================================================*/
static void DrawSoundButton(GmFive *pMe)
{
    IImage* pSoundButton;
	AEEImageInfo  ImageInfo;
	AEERect       rc;
	
    if (pMe == NULL) return ;

	if(pMe->m_toneFlag)
	{
	    
        pSoundButton = ISHELL_LoadResImage(pMe->a.m_pIShell,GMFIVE_RES_FILE_LANG,IDB_SOUNDON);
    	IIMAGE_SetParm(pSoundButton,IPARM_ROP, AEE_RO_TRANSPARENT, 0);
    	IIMAGE_GetInfo(pSoundButton, &ImageInfo);
    	
    	rc.x = pMe->m_cxScreen*2/3 + ImageInfo.cx*3/2 + 8;
    	rc.y = pMe->m_cyScreen - (ImageInfo.cy + PROMPT_INFO_HEIGHT)/2 - 2;
    	rc.dx = ImageInfo.cx;
    	rc.dy = ImageInfo.cy;
    
    	if (pSoundButton)
		{
		    IGRAPHICS_ClearRect(pMe->m_pGraphics,&rc);
		    IGRAPHICS_SetFillMode(pMe->m_pGraphics,TRUE);
			IGRAPHICS_SetColor(pMe->m_pGraphics,255,155,106,0);
            IGRAPHICS_SetFillColor(pMe->m_pGraphics,255,155,106,0);
            
            IGRAPHICS_DrawRect(pMe->m_pGraphics,&rc);
			
			IIMAGE_Draw(pSoundButton,(pMe->m_cxScreen*2/3 + ImageInfo.cx*3/2 + 8),(pMe->m_cyScreen - (ImageInfo.cy + PROMPT_INFO_HEIGHT)/2 - 2));
			
			IIMAGE_Release( pSoundButton );

			pSoundButton = NULL;
		}

		IGRAPHICS_Update(pMe->m_pGraphics);
	}
	else
	{
        pSoundButton = ISHELL_LoadResImage(pMe->a.m_pIShell,GMFIVE_RES_FILE_LANG,IDB_SOUNDOFF);
    	IIMAGE_SetParm(pSoundButton,IPARM_ROP, AEE_RO_TRANSPARENT, 0);
    	IIMAGE_GetInfo(pSoundButton, &ImageInfo);
        
        rc.x = pMe->m_cxScreen*2/3 + ImageInfo.cx*3/2 + 8;
    	rc.y = pMe->m_cyScreen - (ImageInfo.cy + PROMPT_INFO_HEIGHT)/2 - 2;
    	rc.dx = ImageInfo.cx;
    	rc.dy = ImageInfo.cy;
    	
		if (pSoundButton)
		{
		    IGRAPHICS_ClearRect(pMe->m_pGraphics,&rc);
		    IGRAPHICS_SetFillMode(pMe->m_pGraphics,TRUE);
			IGRAPHICS_SetColor(pMe->m_pGraphics,255,155,106,0);
            IGRAPHICS_SetFillColor(pMe->m_pGraphics,255,155,106,0);
			
			IGRAPHICS_DrawRect(pMe->m_pGraphics,&rc);
			
			IIMAGE_Draw(pSoundButton,(pMe->m_cxScreen*2/3 + ImageInfo.cx*3/2 + 8),(pMe->m_cyScreen - (ImageInfo.cy + PROMPT_INFO_HEIGHT)/2 - 2));
			
			IIMAGE_Release( pSoundButton );

			pSoundButton = NULL;
		}

		IGRAPHICS_Update(pMe->m_pGraphics);
	}
    
} //DrawSoundButton()
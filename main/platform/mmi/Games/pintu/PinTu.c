/*======================================================
FILE:  PinTu.c

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
03/19/2004   jhh     Create this file.
15/11/2004   wgs    Modified this file.

=============================================================================*/
#include "PinTu.h"
#ifdef FEATURE_LCD_TOUCH_ENABLE
#define PINTU_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif//FEATURE_LCD_TOUCH_ENABLE


/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */
static boolean  PinTu_Init(IApplet* pi);
static boolean  PinTu_Release(IApplet* pi);
static void     PinTu_Reset(PinTu *pMe);
static boolean  PinTu_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean  ProcessKeyEvent(PinTu *pMe, uint16 wParam);
static void     PinTu_DisplayMainScreen(PinTu * pMe);
static void     AlertDispCallBack(PinTu *pMe);
#ifndef AEE_STATIC
static void     PinTu_AddSoftkeyPrompt(PinTu *pMe,uint16 leftID,uint16 middleID,uint16 rightID);
#endif //AEE_STATIC
static boolean  StartNewGame(PinTu *pMe);
static void     GetRowAndCol(PinTu * pMe,int16 *row,int16 *col,int16 curID);
static void     DrawPicBroad(PinTu *pMe);
static IImage*  GetNeedImage(PinTu *pMe,IImage *pSrcImage,int16 offset_x,int16 offset_y);
static void     DrawEmptyDiamonds(PinTu *pMe);
static int16    JudgeIsMove(PinTu *pMe,int16 selGridID);
static void     DrawNextGirdPlace(PinTu *pMe,int16 nextGridID);
static boolean  PreViewSelectedPic(PinTu *pMe);
static boolean  JudgeGameResult(PinTu *pMe);
static void     DrawGridBmp(PinTu *pMe,IImage *pImage,int16 gridID);
static void     DispAlertInfo(PinTu *pMe,uint16 titleID,uint16 contentID,AECHAR *szBuf);
static void     GameOverCallBack(PinTu *pMe);
static uint32   GetRndNumber(uint32 max);

static boolean  ShowOrigentBmpScreen(PinTu *pMe);
static void     SetParamScreen(PinTu *pMe);
static void     GetMenuItemData(PinTu *pMe);
static boolean  PreViewSelectImageScreen(PinTu *pMe);
static void     DispSelectPreviewImage(PinTu *pMe);
static void     DispMainScrCallBack(PinTu *pMe);
static void     DispMoveStepInfo(PinTu *pMe);
static void     DispHelpInforScreen(PinTu* pMe,uint16 wParam);
static void     DispRecordClockTimer(PinTu *pMe);
static void     SetMenuItemData(PinTu *pMe);
static void     ViewPicWarnCallBack(PinTu *pMe);
static void     ViewPicWarning(PinTu *pMe);
static void     DispCommonInfo(PinTu *pMe,uint16 titleID,uint16 contentID,AECHAR *szBuf);
static void     DrawCtrlFrame(PinTu *pMe,uint16 titleID,AEERect *fRect);
static boolean  GetPlaceByPenDown(PinTu *pMe,uint32 dwParam);
static void     DispTopNumberLable(PinTu *pMe,int16 ID,int16 begin_x,int16 begin_y);
static void     DrawExitButton(PinTu *pMe);
static void     DrawSoundButton(PinTu *pMe);//tcl wgs  050118 added 
static void     PinTu_SetSoundMode(PinTu *pMe);
//static void SavePinTuConfigData(PinTu* pMe);
//static boolean AddRecordToDB(PinTu * pMe);
//static boolean ReadRecordFromDB(PinTu* pMe);
/*===========================================================================

                      STATIC/LOCAL DATA

===========================================================================*/

#if defined(AEE_STATIC)

static int     PinTu_CreateInstance(AEECLSID  ClsId,
                                     IShell   *pIShell,
                                     IModule  *po,
                                     void    **ppObj);

       int     PinTu_Load(IShell *ps, void *pHelpers, IModule **pMod);

static const AEEAppInfo    gaiPinTu =
{
   AEECLSID_PINTU,
   PINTU_RES_FILE_LANG,
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
FUNCTION:  PinTu_GetModInfo

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
PFNMODENTRY PinTu_GetModInfo(IShell      *ps,
                              AEECLSID   **ppClasses,
                              AEEAppInfo **pApps,
                              uint16      *pnApps,
                              uint16      *pwMinPriv)
{
   PARAM_NOT_REF(ps)
   PARAM_NOT_REF(ppClasses)
   PARAM_NOT_REF(pwMinPriv)

   *pApps = (AEEAppInfo *)&gaiPinTu;
   *pnApps = sizeof(gaiPinTu) / sizeof(AEEAppInfo);
   return (PFNMODENTRY)PinTu_Load;
}

/*=============================================================================
FUNCTION:  PinTu_Load

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
int PinTu_Load(IShell    *ps,
                           void      *pHelpers,
                           IModule **pMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           ps,
                           pHelpers,
                           pMod,
                           (PFNMODCREATEINST)PinTu_CreateInstance,
                           (PFNFREEMODDATA)NULL);
}
#endif  /*AEE_STATIC*/

/*===============================================================================
//applet create instance
=============================================================================== */
#if defined(AEE_STATIC)
int PinTu_CreateInstance(AEECLSID  ClsId,
                         IShell    *pIShell,
                         IModule   *po,
                         void     **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
   *ppObj = NULL;

   if(ClsId == AEECLSID_PINTU)
   {
      if(AEEApplet_New(sizeof(PinTu),
                   ClsId,
                   pIShell,
                   po,
                   (IApplet**)ppObj,
                   (AEEHANDLER)PinTu_HandleEvent,
                   (PFNFREEAPPDATA)PinTu_Release) == TRUE)
      {
         if(PinTu_Init((IApplet*)*ppObj))
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
//applet init
//////////////////////////////////////////////
static boolean PinTu_Init(IApplet* pi)
{

   PinTu *pMe   = (PinTu *)(void *)pi;
   IShell   *ps  = pMe->a.m_pIShell;
   int       nAscent, nDescent;
   char *filename = NULL;    //tcl wgs 050127 added
   AEEDeviceInfo pdi;
   char       catalogIdx[MAX_CATALOG_NUM][MAX_FILENAME_LEN] = {PINTU_RING_DIR,DOWNLOAD_IMAGE_DIR};
   MSG_FATAL("PinTu_Init..........................",0,0,0);
   if(pMe == NULL)  return FALSE;

   //create common obj
   if(!CreateObj(ps, AEECLSID_GRAPHICS, (void **)&pMe->m_pGraphics))
      return FALSE;

   if(!CreateObj(ps, AEECLSID_STATIC, (void **)&pMe->m_pStaticCtl))
      return FALSE;


   if(!CreateObj(ps, AEECLSID_MENUCTL, (void **)&pMe->m_pMenuCtl))
      return FALSE;

   if(!CreateObj(ps, AEECLSID_IMAGECTL, (void **)&pMe->m_pImageCtl))
      return FALSE;
      
    if(!CreateObj(ps, AEECLSID_RINGERMGR, (void **)&pMe->m_promptVoice))
        return FALSE;           
  /* if((filename = (char *)MALLOC(MAX_FILENAME_LEN)) == NULL)//tcl wgs 050127 added 
   {   
       DBGPRINTF("malloc the filename space error");
       return FALSE;
   }
   else
   {
       MEMSET((void *)filename, 0, MAX_FILENAME_LEN);
   }*/
   if (ISHELL_CreateInstance(ps, AEECLSID_ANNUNCIATOR,(void **) &pMe->m_pIAnn))
	{
		return FALSE;
	}
#ifdef FEATURE_DEBUG
    CreateImageEffectObj(pMe);
    pMe->m_ImageEffectType = 0;

#endif //FEATURE_DEBUG
   // obtain device information
   ISHELL_GetDeviceInfo(ps,&pdi);
   pMe->m_cxScreen = (int16)pdi.cxScreen;
   pMe->m_cyScreen = (int16)pdi.cyScreen;

   if (ISHELL_GetPrefs(pMe->a.m_pIShell, AEECLSID_PINTU, CFG_VERSION, &(pMe->m_CfgData),
                      sizeof(PrefData)) != SUCCESS)
   {
        pMe->m_paramAttr  = LIMIT_THREE_MINTUES |LIMIT_TRHEE_AND_THREE;
        pMe->m_pSrcImage    = ISHELL_LoadResImage( pMe->a.m_pIShell, PINTU_RES_FILE_LANG, IDB_DEFAULT_IMAGE );//ISHELL_LoadImage(ps,(char*)"defaultres.png");
   }
   else
   {
        if((filename = (char *)MALLOC(MAX_FILENAME_LEN)) == NULL)//tcl wgs 050127 added 
       {   
           DBGPRINTF("malloc the filename space error");
           return FALSE;
       }
       else
       {
           MEMSET((void *)filename, 0, MAX_FILENAME_LEN);
       }
        pMe->m_paramAttr = pMe->m_CfgData.settingWparam;
        
        if(pMe->m_CfgData.flag)
       {
           DBGPRINTF("GET THE IMAGE CATALOG = %s",catalogIdx[0]);
           STRCAT(filename,catalogIdx[0]);
       }
       else
       {
           DBGPRINTF("GET THE IMAGE CATALOG = %s",catalogIdx[1]);
           STRCAT(filename,catalogIdx[1]);
       }
       STRCAT(filename, DIRECTORY_STR);
       STRCAT( filename, pMe->m_CfgData.imagename);
       DBGPRINTF("the full imagename is %s",filename);
       DBGPRINTF("THE IMAGENAME IS %s",pMe->m_CfgData.imagename);
       pMe->m_pSrcImage = ISHELL_LoadImage(pMe->a.m_pIShell, (const char *)filename);
       //若出现先前设置的图片被删除，再此重新使用默认的图片  //tcl jhh050606
       if (!pMe->m_pSrcImage)
       {
           pMe->m_pSrcImage = ISHELL_LoadResImage( pMe->a.m_pIShell, PINTU_RES_FILE, IDB_DEFAULT_IMAGE );
       }
       
       FREE(filename);
   }
   pMe->m_paramHistoryAttr = pMe->m_paramAttr;

   //get normal font information
   IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, &nAscent, &nDescent);
   pMe->m_nNChSize  = nAscent + nDescent;

   //set popWindow frame color
   IDISPLAY_SetColor(pMe->a.m_pIDisplay,CLR_USER_FRAME, POPMENU_FRAME_LINE);
   //set popWindow rect area
   SETAEERECT(&pMe->m_popWinFrameRect, pMe->m_cxScreen/8, pMe->m_cyScreen/4, (pMe->m_cxScreen*3)/4, DISP_POPMENU_ITEM_NUM*(pMe->m_nNChSize+3));
   pMe->m_menuLevelStack[0] = IDS_NEW_GAME;
   pMe->m_eAppState   = APP_STATE_MAIN_MENU;
   pMe->m_continueFlg = FALSE;
   pMe->m_endGameFlg  = FALSE;
   pMe->m_keyAvaible  = TRUE;
   pMe->m_selImageItemID = 0;
   pMe->m_lastSelImageID = 0;
   pMe->m_viewOrigPicTime= MAX_VIEW_PIC_TIME;
   MEMSET(pMe->m_loadImageFile,0x0,sizeof(pMe->m_loadImageFile));
   pMe->m_pViewSelectImage = NULL;

   MEMSET(pMe->m_menuLevelStack,0x0,sizeof(pMe->m_menuLevelStack));
  
   //loade top background image for title
   pMe->m_pTopBGImage  = ISHELL_LoadResImage( pMe->a.m_pIShell, PINTU_RES_FILE_LANG, IDB_TOP_BGBMP );
   if(pMe->m_pTopBGImage)
   {
      IIMAGE_SetParm(pMe->m_pTopBGImage,IPARM_ROP, AEE_RO_TRANSPARENT, 0) ;
   }

   //load softkey bottom image
   pMe->m_pBottomImage = ISHELL_LoadResImage( pMe->a.m_pIShell, PINTU_RES_FILE_LANG, IDB_BOTTOM);
   if (pMe->m_pBottomImage)
   {
        IIMAGE_SetParm(pMe->m_pBottomImage,IPARM_ROP, AEE_RO_TRANSPARENT, 0) ;
   }
   //load number lable image
   pMe->m_pNumberLableImage = ISHELL_LoadResImage( pMe->a.m_pIShell, PINTU_RES_FILE_LANG, IDB_NUMBER);
   if (pMe->m_pNumberLableImage)
   {
        IIMAGE_SetParm(pMe->m_pNumberLableImage,IPARM_ROP, AEE_RO_TRANSPARENT, 0) ;
   }
   pMe->m_Soundflag = TRUE;
  
   return TRUE;
}//PinTu_Init()

///////////////////////////////////////////////
//applet release interface
//////////////////////////////////////////////
static boolean PinTu_Release(IApplet* pi)
{
   PinTu *pMe = (PinTu *)(void *)pi;

   if(pMe == NULL)  return FALSE;

   ReleaseObj((void **)&pMe->m_pGraphics);
   ReleaseObj((void **)&pMe->m_pStaticCtl);
   ReleaseObj((void **)&pMe->m_pMenuCtl);
   ReleaseObj((void **)&pMe->m_pImageCtl);
   ReleaseObj((void **)&pMe->m_pSrcImage);
   ReleaseObj((void **)&pMe->m_pViewSelectImage);
   ReleaseObj((void **)&pMe->m_pTopBGImage);
   ReleaseObj((void **)&pMe->m_promptVoice);

   /*if (pMe->m_pStateKeyID)
   {
        FREE(pMe->m_pStateKeyID);
        pMe->m_pStateKeyID = NULL;
   }
   */

   if (pMe->m_pBottomImage)
   {
       IIMAGE_Release(pMe->m_pBottomImage);
       pMe->m_pBottomImage = NULL;
   }

   if (pMe->m_pNumberLableImage)
   {
       IIMAGE_Release(pMe->m_pNumberLableImage);
       pMe->m_pNumberLableImage = NULL;
   }
   if (pMe->m_pIAnn)
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
   //save setting parameter
   pMe->m_CfgData.settingWparam = pMe->m_paramAttr;
   ISHELL_SetPrefs(pMe->a.m_pIShell, AEECLSID_PINTU, CFG_VERSION, &(pMe->m_CfgData),sizeof(PrefData));
   return TRUE;
}//PinTu_Release()

/*===========================================================================
   This function adds control reset
===========================================================================*/
static void PinTu_Reset(PinTu *pMe)
{
   if(pMe == NULL) return ;

   if(pMe->m_pMenuCtl)
   {
	  IMENUCTL_SetActive(pMe->m_pMenuCtl,FALSE);
      IMENUCTL_Reset(pMe->m_pMenuCtl);
   }

   if(pMe->m_pStaticCtl)
   {
      ISTATIC_SetActive(pMe->m_pStaticCtl,FALSE);
      ISTATIC_Reset(pMe->m_pStaticCtl);
   }

   if(pMe->m_pImageCtl)
   {
	  IIMAGECTL_SetActive(pMe->m_pImageCtl,FALSE);
      IIMAGECTL_Reset(pMe->m_pImageCtl);
   }
   ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
}//PinTu_Reset()


//////////////////////////////////////////////
//display image when applet start
//////////////////////////////////////////////
static void PinTu_DisplaySplashScreen( PinTu* pMe )
{
  IImage*       pSplash;
  AEEImageInfo  rImageInfo;

  // The current app state is APP_STATE_START, clear the screen, load the resource image
  // and display it.
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
  
    IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
    IDISPLAY_FillRect(pMe->a.m_pIDisplay,&rc,flashbcrgbvalue);
  //end added by tcl wgs 041217

  pSplash = ISHELL_LoadResImage( pMe->a.m_pIShell, PINTU_RES_FILE_LANG, IDB_START_FLASH );

  if( pSplash )
  {
    // Get image information
    IIMAGE_GetInfo( pSplash, &rImageInfo );

    // Draw the image in the center of the screen
    IIMAGE_Draw( pSplash, ( pMe->m_cxScreen / 2 ) - ( rImageInfo.cx / 2 ),
               ( pMe->m_cyScreen / 2 ) - ( rImageInfo.cy / 2 ) );
    // The image is no longer needed so release it

    IIMAGE_Release( pSplash );
    pSplash = NULL;
    // Set the callback timer to call this function again
    ISHELL_SetTimer( pMe->a.m_pIShell, SPLASH_TIMER_DURATION, ( PFNNOTIFY ) PinTu_DisplayMainScreen, pMe );
  }
  else
  {
    PinTu_DisplayMainScreen( pMe );
  }
  IDISPLAY_Update( pMe->a.m_pIDisplay );
}//PinTu_DisplaySplashScreen()

///////////////////////////////////////////////
//applet handle event
//////////////////////////////////////////////
static boolean PinTu_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   PinTu*    pMe = (PinTu *)pi;
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
										PINTU_RES_FILE_LANG,								  
										IDS_APP_TITLE,
										WTitle,
										sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
    			    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            	}
         }
         PinTu_DisplaySplashScreen(pMe);
         return TRUE;

      case EVT_APP_STOP:
         return TRUE;

      case EVT_APP_SUSPEND:      //suspend event
         ReleaseObj((void **)&pMe->m_promptVoice);
         (void)ISHELL_CancelTimer(pMe->a.m_pIShell,NULL,pMe);
         return TRUE;

      case EVT_APP_RESUME:
         {
            uint16   eventWparam = USRE_DRAW;

            if(!CreateObj(ps, AEECLSID_RINGERMGR, (void **)&pMe->m_promptVoice))
               return FALSE;   

            switch(pMe->m_eAppState)
            {
               case APP_STATE_START:            //continue game
                   
                  eventWparam = IDS_GAME_CONTINUE;
                  break;

               case APP_STATE_IMAGE_SELECT:     //image list
                  eventWparam = IDS_SELECT_IMAGE;
                  break;

               case APP_STATE_IAMGE_PREVIEW:    //preview the selected image
                  DispSelectPreviewImage(pMe);
                  return TRUE;

               default:
                  eventWparam = USRE_DRAW;      //main menu manage
                  break;
            }//end of switch()
            (void)ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_PINTU, EVT_COMMAND, eventWparam, 0);
            return TRUE;
         }

      case EVT_COMMAND:
         //deal the especially key event command
         if ((pMe->m_eAppState == APP_STATE_IMAGE_SELECT) && (wParam >= BASE_BMP_ITEM_ID))
         {
            ProcessKeyEvent(pMe,AVK_F2);
         }
		 if(pMe->m_pIAnn != NULL)
         {	    			
                IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);				
				MSG_FATAL("***zzg Application EVT_APP_START EnableAnnunciatorBar TRUE",0,0,0);				   
         }
         //record the selected ItemID
         if(wParam == IDS_NEW_GAME     || wParam == IDS_GAME_CONTINUE || wParam == IDS_SELECT_IMAGE ||
            wParam == IDS_CONTROL_HELP || wParam == IDS_ABOUT_VERSION || wParam == IDS_SET_PARAM|| wParam == IDS_SOUND_SET)
         {
            pMe->m_menuLevelStack[0] = IMENUCTL_GetSel(pMe->m_pMenuCtl);
         }
         switch(wParam)
         {

        #if defined(AEE_STATIC)
            //case IDI_BOTTOMBAR_MODIFY:
            //case IDI_BOTTOMBAR_PRIVIEW:
                ProcessKeyEvent(pMe,AVK_F2);
                return TRUE;
        #endif
        
            case IDS_OK:
            case USRE_DRAW:
               //pMe->m_eAppState = APP_STATE_MAIN_MENU;
               PinTu_DisplayMainScreen(pMe);
               return TRUE;

            case IDS_NEW_GAME:     //game new
            case IDS_GAME_CONTINUE://game continue
               pMe->m_continueFlg  = (wParam == IDS_GAME_CONTINUE)?TRUE:FALSE;
               if (StartNewGame(pMe))
               {
                   pMe->m_eAppState = APP_STATE_START;
                   pMe->m_endGameFlg = FALSE;
                   return TRUE;
               }
               else
               {
                   pMe->m_promptType = AVAIBLE_IMAGE;
                   ViewPicWarning(pMe);
               }
               break;

            case IDS_SELECT_IMAGE: //select image to spelling
               pMe->m_eAppState = APP_STATE_IMAGE_SELECT;
               if(!ShowOrigentBmpScreen(pMe))
               {
                  pMe->m_eAppState = APP_STATE_DISP_INFO;
                  DispAlertInfo(pMe,wParam,IDS_NOT_SELECT_IMAGE,NULL);
                  return TRUE;
               }
               return TRUE;

            case IDS_SET_PARAM: //set paramter of spelling
               pMe->m_eAppState = APP_STATE_SETTING;
               SetParamScreen(pMe);
               break;

            case IDS_CONTROL_HELP:  //view the help information
            case IDS_ABOUT_VERSION: //viewer the version
               DispHelpInforScreen(pMe,wParam);
               return TRUE;

            case IDS_EXIT_GAME:    //exit the game
               
               (void)ISHELL_CloseApplet(ps,FALSE);
               return TRUE;
               
            case IDS_SOUND_SET:
               PinTu_SetSoundMode(pMe);
               return TRUE;
               
            case IDS_SOUND_ON:
               pMe->m_Soundflag = TRUE;
               PinTu_DisplayMainScreen(pMe);
               return TRUE;
                
            case IDS_SOUND_OFF:
               pMe->m_Soundflag = FALSE;
               PinTu_DisplayMainScreen(pMe);
               return TRUE;
                
            default:
               break;
         }
         break;

      case EVT_KEY:              // Process key event
         if(pMe->m_eAppState == APP_STATE_SOUNDSET)
         {
            if( TRUE == IMENUCTL_HandleEvent(pMe->m_pMenuCtl,eCode, wParam,dwParam) )
            return TRUE;
         }
         if(UpAndDownScroll_HandleEvent(pMe->a.m_pIShell,pMe->m_pMenuCtl, eCode, wParam))
            return TRUE;
         if(ProcessKeyEvent(pMe,wParam)) return TRUE;
         else break;

#ifdef FEATURE_LCD_TOUCH_ENABLE

      case EVT_PEN_UP:
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
          
              if(PINTU_PT_IN_RECT(wXPos,wYPos,rc) && (pMe->m_eAppState != APP_STATE_START))
              {
                  if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                  {
                      boolean rt =  PinTu_HandleEvent((IApplet*)pMe,EVT_KEY,AVK_SELECT,0);
                      return rt;
                  }
                  else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                  {                       
                       boolean rt = PinTu_HandleEvent((IApplet*)pMe,EVT_KEY,AVK_CLR,0);
                       return rt;
                  }
              }    
                
             MSG_FATAL("PinTu_HandleEvent EVT_PEN_UP m_eAppState=%d",pMe->m_eAppState,0,0);
             if((pMe->m_eAppState == APP_STATE_START)&&(!pMe->m_endGameFlg) && pMe->m_keyAvaible) //during the game state
             {
                return GetPlaceByPenDown(pMe,dwParam);
             }
             break;
        }
      
      case EVT_PEN_MOVE:
      case EVT_PEN_DOWN:         
         if (pMe->m_eAppState != APP_STATE_START )
         {
         #ifdef AEE_STATIC                  
             //judge avaible area of bottomBar    
             if (AEE_GET_Y(dwParam) >= (pMe->m_cyScreen - PINTU_SOTFKEY_CY))   
             {
                 //if (TCLDrawBottomBar_HandleEvent ( &pMe->m_BottomBar,  pMe->a.m_pIShell, pMe->a.m_pIDisplay, eCode, wParam,dwParam))
                 {
                     return TRUE;
                 }
             }
         #endif //AEE_STATIC    
         }
         break;
#endif		 

      default:
         break;

   }//switch (eCode)

   // The application did not need to handle the event so pass it to each
   // of the controls to let then deal with it.

   if (IMENUCTL_IsActive(pMe->m_pMenuCtl))
   {
	   if( IMENUCTL_HandleEvent( pMe->m_pMenuCtl, eCode, wParam, dwParam ) )
		   return TRUE;
   }

   //the image control has some up and down moving currently ,so remove this codes 
   if (IIMAGECTL_IsActive(pMe->m_pImageCtl))
   {
	   if( IIMAGECTL_HandleEvent( pMe->m_pImageCtl, eCode, wParam, dwParam ) )
		   return TRUE;
   }
         
   if (pMe->m_eAppState == APP_STATE_HELP)
   {
       if( ISTATIC_HandleEvent( pMe->m_pStaticCtl, eCode, wParam, dwParam ) )       
           return TRUE;       
   }
   else
   {
       if (ISTATIC_IsActive(pMe->m_pStaticCtl))
       {   
           if( ISTATIC_HandleEvent( pMe->m_pStaticCtl, eCode, wParam, dwParam ) )       
               return TRUE;    
       }    
   }

   return FALSE;
}//PinTu_HandleEvent()

/*===========================================================================
   callback of draw background bmp
===========================================================================*/
#ifndef AEE_STATIC

static void DrawTopTitleProc(PinTu * pMe)
{
    AECHAR   szTitle[32];
    AECHAR*  pBuf;
    uint16   nMaxUnicode = 7;

    if(pMe == NULL) return ;

    MEMSET(szTitle,0,sizeof(szTitle));
    pBuf   = pMe->m_szwinTitle;
    pBuf += pMe->m_topTitleLen;

    if (pMe->m_AllASCIICharFlg)
    {
        nMaxUnicode = 18;
    }    

    WSTRNCOPYN(szTitle,WSTRLEN(szTitle),pBuf,nMaxUnicode);
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
static uint16 DrawTopBackGroundBMP(PinTu * pMe,uint16 resID,AECHAR * pText)
{
    
#if defined(AEE_STATIC)    
    if(pMe ==  NULL) return 0;
    
    //SetUniMenuConfig(pMe->m_pMenuCtl);
    (void)IMENUCTL_SetTitle ( pMe->m_pMenuCtl, PINTU_RES_FILE_LANG,  resID,  NULL );
    return 0;

#else    
    
    AEEImageInfo  ImageInfo;
    AECHAR        szTitle[32];
    uint16        len;

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
        ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG, resID, szTitle, sizeof(szTitle));
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
static void PinTu_DisplayMainScreen(PinTu * pMe)
{
   AEERect   rectMenu;
   int       dx = pMe->m_cxScreen;
   int       dy = pMe->m_cyScreen;
   IDisplay *pd = pMe->a.m_pIDisplay;
   int16     startPoint;

   if (pMe == NULL)  return;

   PinTu_Reset(pMe);
   IDISPLAY_ClearScreen(pd);

   startPoint =  DrawTopBackGroundBMP(pMe,IDS_APP_TITLE,NULL);
   dy -= (PINTU_SOTFKEY_CY  + startPoint) ;//for softkey prompt
   SETAEERECT(&rectMenu, 1, startPoint, (dx - 2), dy );
   //set menu control attribute
   (void)Brew_SetMenuAttr(pMe->m_pMenuCtl,AEECLSID_MENUCTL,&rectMenu,MP_MAXSOFTKEYITEMS);//set menu attible

   (void)Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG,IDS_NEW_GAME,      NULL, IDB_START_GAME,IDS_NEW_GAME,     0);       //game new
   if(pMe->m_continueFlg && pMe->m_recordClock > 0)
      (void)Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG,IDS_GAME_CONTINUE, NULL, IDB_CONTINUE,IDS_GAME_CONTINUE,0);    //game continue
   //(void)Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG,IDS_SELECT_IMAGE,     NULL, IDB_SELECT_IAMGE,IDS_SELECT_IMAGE,0);    //select image to spelling
   (void)Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG,IDS_SET_PARAM,        NULL, IDB_SETTING,IDS_SET_PARAM,   0);    //set paramter
   #if 0
   (void)Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG,IDS_SOUND_SET,        NULL, IDB_SOUND_SET,IDS_SOUND_SET,   0);    //set sound  added by tcl wgs 050120
   #endif
   (void)Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG,IDS_CONTROL_HELP,  NULL, IDB_HELP,      IDS_CONTROL_HELP, 0);       //game help
#ifndef AEE_STATIC
   (void)Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG,IDS_ABOUT_VERSION, NULL, IDB_ABOUT_GAME,IDS_ABOUT_VERSION,0);       //game about
#endif   
   (void)Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG,IDS_EXIT_GAME,     NULL, IDB_EXIT_GAME, IDS_EXIT_GAME,    0);       //game exit
   IMENUCTL_SetSel(pMe->m_pMenuCtl,pMe->m_menuLevelStack[0]);
   IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
   IMENUCTL_Redraw(pMe->m_pMenuCtl);
   //add softkey prompt
#if defined(AEE_STATIC) 
    DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
#else   
   PinTu_AddSoftkeyPrompt(pMe,IDS_SELECT,NULL,IDS_RETURN);
#endif //AEE_STATIC   
   //IDISPLAY_Update(pd);
   pMe->m_eAppState = APP_STATE_MAIN_MENU;
   
}//PinTu_DisplayMainScreen()

/*===========================================================================
   display softkey prompt
   wParam
   pMe:
   leftItemID:  left softkey resource id
   rightItemID: right softkey resource id
   softkeyRect: softkey rect area
===========================================================================*/

#ifndef AEE_STATIC

static void PinTu_AddSoftkeyPrompt(PinTu *pMe,uint16 leftID,uint16 middleID,uint16 rightID)
{
    AECHAR    textBuf[16] = {'\0'};
    AEERect   softkeyRect;
    AEEImageInfo  ImageInfo;
	IImage*   pBottomBmp = NULL;

    if (pMe == NULL) return;

    softkeyRect.x  = 0;
    softkeyRect.y  = pMe->m_cyScreen - PINTU_SOTFKEY_CY;
    softkeyRect.dx = pMe->m_cxScreen;
    softkeyRect.dy = PINTU_SOTFKEY_CY;

	pMe->m_softkeyPrompt.leftItemID   = leftID;
	pMe->m_softkeyPrompt.middleItemID = middleID;
	pMe->m_softkeyPrompt.rightItemID  = rightID;

    pBottomBmp = ISHELL_LoadResImage( pMe->a.m_pIShell, PINTU_RES_FILE_LANG, IDB_SOFTKEY_BUTTOM );
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
    if (leftID)
    {
        if(pMe->m_pBottomImage)
        {
            // Draw the image in the center of the screen
            IIMAGE_Draw(pMe->m_pBottomImage,softkeyRect.x,pMe->m_cyScreen - ImageInfo.cy);
        }

        ISHELL_LoadResString(pMe->a.m_pIShell,PINTU_RES_FILE_LANG,leftID, textBuf, sizeof(textBuf));
        (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, textBuf,-1,(softkeyRect.x + 4),(int)(softkeyRect.y +3),NULL,IDF_TEXT_TRANSPARENT);
        //record the softkey ID for pen using
        //pMe->m_pStateKeyID[pMe->m_eAppState] |= LEFT_KEY_ID;
    }

    if (middleID)
    {
        if (pMe->m_pBottomImage)
        {
            // Draw the image in the center of the screen
            IIMAGE_Draw(pMe->m_pBottomImage,(softkeyRect.dx - ImageInfo.cx)/2,pMe->m_cyScreen - ImageInfo.cy);
        }

        ISHELL_LoadResString(pMe->a.m_pIShell,PINTU_RES_FILE_LANG,middleID, textBuf, sizeof(textBuf));
        (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, textBuf,-1,(int)(softkeyRect.dx/2 - 16),(int)(softkeyRect.y+3),NULL,IDF_TEXT_TRANSPARENT);
        //record the softkey ID for pen using
        //pMe->m_pStateKeyID[pMe->m_eAppState] |= MIDDLE_KEY_ID;
    }

    //draw right softkey text
    if (rightID)
    {
        if (pMe->m_pBottomImage)
        {
            // Draw the image in the center of the screen
            IIMAGE_Draw(pMe->m_pBottomImage,(softkeyRect.dx - ImageInfo.cx),pMe->m_cyScreen - ImageInfo.cy);
        }

        ISHELL_LoadResString(pMe->a.m_pIShell,PINTU_RES_FILE_LANG,rightID, textBuf, sizeof(textBuf));
        (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, textBuf,-1,(int)(softkeyRect.dx - 38),(int)(softkeyRect.y+3),NULL,IDF_TEXT_TRANSPARENT);
        //record the softkey ID for pen using
        //pMe->m_pStateKeyID[pMe->m_eAppState] |= RIGHT_KEY_ID;
    }

    IDISPLAY_Update( pMe->a.m_pIDisplay );

}//PinTu_AddSoftkeyPrompt()

#endif //AEE_STATIC

/*===========================================================================
   get row and col value
===========================================================================*/
static void GetRowAndCol(PinTu * pMe,int16 *row,int16 *col,int16 curID)
{
   if(curID <= pMe->m_arrayInfo.max_block)
   {
      *row = (curID -1)/pMe->m_arrayInfo.row;
      *col = (curID -1)%pMe->m_arrayInfo.col;
   }
}//GetRowAndCol()

/*===========================================================================
   //draw the step of move
===========================================================================*/
static void DispMoveStepInfo(PinTu *pMe)
{
   AECHAR szBuf[16],szTemp[16];
   RGBVAL  oldTextColor;
   AEERect tempRect;

   oldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, BOTTOM_PROMPT_TEXE_COLOR);
   ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG,IDS_MOVE_TOTAL_STEPS, szTemp, sizeof(szTemp));

   WSPRINTF(szBuf,16 *sizeof(AECHAR),szTemp,pMe->m_moveStep);
   SETAEERECT(&tempRect,0,(pMe->m_cyScreen - PROMPT_INFO_HEIGHT),(pMe->m_cxScreen/3-2),PROMPT_INFO_HEIGHT);
   IDISPLAY_FillRect(pMe->a.m_pIDisplay,&tempRect,BACK_GROUND_COLOR);
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                           AEE_FONT_NORMAL,szBuf, -1,0, 0,&tempRect,
                           IDF_ALIGN_CENTER|IDF_ALIGN_TOP|IDF_TEXT_TRANSPARENT);
   (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, oldTextColor);
}//DispMoveStepInfo()

/*===========================================================================
   //display the time of game
===========================================================================*/
static void DispRecordClockTimer(PinTu *pMe)
{
   AECHAR szBuf[16],szTemp[16];
   RGBVAL  oldTextColor;
   AEERect tempRect;

   if(pMe == NULL) return;
   if(pMe->m_recordClock < 0)
   {
      pMe->m_keyAvaible = FALSE;
      DispAlertInfo(pMe,IDS_ALERT_INFO,IDS_GAME_INTERRUPT,NULL);
      return;
   }

   oldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, BOTTOM_PROMPT_TEXE_COLOR);
   ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG,IDS_RECORD_CLOCK, szTemp, sizeof(szTemp));
   WSPRINTF(szBuf,16*sizeof(AECHAR),szTemp,pMe->m_recordClock);
   pMe->m_recordClock--;
   SETAEERECT(&tempRect,(pMe->m_cxScreen/2 +10 ), (pMe->m_cyScreen - PROMPT_INFO_HEIGHT),pMe->m_cxScreen/4,PROMPT_INFO_HEIGHT);
   IDISPLAY_FillRect(pMe->a.m_pIDisplay,&tempRect,BACK_GROUND_COLOR);
   (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                           AEE_FONT_NORMAL,szBuf, -1,0, 0,&tempRect,
                           IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_TEXT_TRANSPARENT);
   (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, oldTextColor);
   (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                          1000,
                          (PFNNOTIFY)DispRecordClockTimer,
                          pMe);
   IDISPLAY_Update(pMe->a.m_pIDisplay);

}//DispRecordClockTimer()

/*===========================================================================
   draw frame of broad
===========================================================================*/
static void DispPicFrame(PinTu * pMe,AEERect *pFrameRect)
{
    if(pMe == NULL) return;

    IDISPLAY_DrawRect(pMe->a.m_pIDisplay, pFrameRect,MAKE_RGB(64,0,0), 0,IDF_RECT_FRAME);
    //draw middle line of Rect
    pFrameRect->x  -= 1;
    pFrameRect->y  -= 1;
    pFrameRect->dx += 2;
    pFrameRect->dy += 2;
    IDISPLAY_DrawRect(pMe->a.m_pIDisplay, pFrameRect,MAKE_RGB(255,255,0), 0,IDF_RECT_FRAME);

    //draw middle line of Rect
    pFrameRect->x  -= 1;
    pFrameRect->y  -= 1;
    pFrameRect->dx += 2;
    pFrameRect->dy += 2;
    IDISPLAY_DrawRect(pMe->a.m_pIDisplay, pFrameRect,MAKE_RGB(64,0,0), 0,IDF_RECT_FRAME);
}


/*===========================================================================
   disp the number lable
===========================================================================*/
static void DispTopNumberLable(PinTu *pMe,int16 ID,int16 begin_x,int16 begin_y)
{
    int16 tmpVal;

    if (pMe == NULL || pMe->m_pNumberLableImage == NULL)  return;

    tmpVal = pMe->m_paramAttr;
    if (!(tmpVal & DISP_NUMBER_LABLE)) return ;

    //for 3*3 mode
    if (ID <= 9)
    {
        IIMAGE_SetDrawSize(pMe->m_pNumberLableImage,NUMBER_WIDTH,NUMBER_HEIGHT);
        IIMAGE_SetOffset(pMe->m_pNumberLableImage,(ID -1)*NUMBER_WIDTH,0);
        IIMAGE_Draw(pMe->m_pNumberLableImage,(begin_x + 2),(begin_y+2));
    }
    else
    {
        // draw second number lable
        if (ID < 20)
        {
            IIMAGE_SetDrawSize(pMe->m_pNumberLableImage,NUMBER_ONE_WIDTH ,NUMBER_HEIGHT);
            IIMAGE_SetOffset(pMe->m_pNumberLableImage,0,0);
        }
        else
        {
            IIMAGE_SetDrawSize(pMe->m_pNumberLableImage,NUMBER_WIDTH ,NUMBER_HEIGHT);
            IIMAGE_SetOffset(pMe->m_pNumberLableImage,(ID/10 - 1)*NUMBER_WIDTH,0);
        }
        IIMAGE_Draw(pMe->m_pNumberLableImage,(begin_x + 2),(begin_y+2));

        //draw second number lable for example 10,11 and so on
        if (ID%10 == 0)  // get zero number for 20,30 and so on
        {
            IIMAGE_SetOffset(pMe->m_pNumberLableImage,(10 -1)*NUMBER_WIDTH,0);
        }
        else
        {
            IIMAGE_SetOffset(pMe->m_pNumberLableImage,(ID%10 -1)*NUMBER_WIDTH,0);
        }
        IIMAGE_SetDrawSize(pMe->m_pNumberLableImage,NUMBER_WIDTH,NUMBER_HEIGHT);
        if (ID < 20)
        {
            IIMAGE_Draw(pMe->m_pNumberLableImage,(begin_x + 2 + NUMBER_ONE_WIDTH),(begin_y+2));
        }
        else
        {
            IIMAGE_Draw(pMe->m_pNumberLableImage,(begin_x + 2 + NUMBER_WIDTH),(begin_y+2));
        }
    }// end of else
} //DispTopNumberLable()


/*===========================================================================
   draw picture gird
===========================================================================*/
static void DrawPicBroad(PinTu *pMe)
{

   int16 col,row,i,j;
   int16 begin_x ,begin_y,offset_x,offset_y;
   IImage *pImage;
   uint16  tmpVal;
   AEERect frameRect;


   if(pMe == NULL) return;

   //draw the block cell
   for(i = 0;i < pMe->m_arrayInfo.row;i++)
      for(j = 0;j < pMe->m_arrayInfo.col;j++)
      {
         uint16 blockID = i*pMe->m_arrayInfo.row + j;

         GetRowAndCol(pMe,&row,&col,pMe->m_Resultarray[blockID]);
         offset_x = IMAGE_BEGIN_X + col*pMe->m_girdWidth;
         offset_y = IMAGE_BEGIN_Y + row*pMe->m_girdHeight;
         if(pMe->m_Resultarray[blockID] == pMe->m_arrayInfo.max_block)
         {

            pMe->m_curGrid.begin_x = pMe->m_arrayInfo.screen_xOffSet + j*(pMe->m_girdWidth + BETWEEN_LEN);
            pMe->m_curGrid.begin_y = pMe->m_arrayInfo.screen_yOffSet + i*(pMe->m_girdHeight +BETWEEN_LEN);
            pMe->m_curGrid.width  = (pMe->m_girdWidth );
            pMe->m_curGrid.height = (pMe->m_girdHeight);
            pMe->m_curGrid.ID = ( blockID + 1);
            MEMCPY(&pMe->m_historyGrid,&pMe->m_curGrid,sizeof(gridInfo));
            DrawEmptyDiamonds(pMe);
            continue;
         }

         if(pMe->m_pSrcImage)
         {
            pImage = GetNeedImage(pMe,pMe->m_pSrcImage,offset_x,offset_y);
            if(pImage)
            {
               begin_x = pMe->m_arrayInfo.screen_xOffSet + j*(pMe->m_girdWidth + BETWEEN_LEN);
               begin_y = pMe->m_arrayInfo.screen_yOffSet + i*(pMe->m_girdHeight + BETWEEN_LEN);
               IIMAGE_Draw(pImage,begin_x,begin_y);
               //draw the number lable for prompt user
               DispTopNumberLable(pMe,pMe->m_Resultarray[blockID],begin_x ,begin_y);

            }
         }
      }//end of for()

    //draw line
    for(i = 1;i < pMe->m_arrayInfo.col;i++)
       IDISPLAY_DrawHLine(pMe->a.m_pIDisplay,pMe->m_arrayInfo.screen_xOffSet,pMe->m_arrayInfo.screen_yOffSet+i*pMe->m_girdHeight+(i-1)*BETWEEN_LEN,pMe->m_arrayInfo.col*pMe->m_girdWidth+(pMe->m_arrayInfo.col -1)*BETWEEN_LEN);
    for(i = 1;i < pMe->m_arrayInfo.row;i++)
       IDISPLAY_DrawVLine(pMe->a.m_pIDisplay,pMe->m_arrayInfo.screen_xOffSet+i*pMe->m_girdWidth+(i-1)*BETWEEN_LEN,pMe->m_arrayInfo.screen_yOffSet,pMe->m_arrayInfo.row*pMe->m_girdHeight+(pMe->m_arrayInfo.row -1)*BETWEEN_LEN);
   //set rect area
    SETAEERECT(&frameRect,pMe->m_arrayInfo.screen_xOffSet-1,pMe->m_arrayInfo.screen_yOffSet-1,
              pMe->m_arrayInfo.col*pMe->m_girdWidth+(pMe->m_arrayInfo.col -1)*BETWEEN_LEN +2,
              pMe->m_arrayInfo.row*pMe->m_girdHeight+(pMe->m_arrayInfo.row -1)*BETWEEN_LEN+2);
    //draw the picture frame
    DispPicFrame(pMe,&frameRect);

   //draw the step of move information
   DispMoveStepInfo(pMe);
   //draw the exit button
   DrawExitButton(pMe);
   //draw the sound button tcl wgs 050119 added
   //DrawSoundButton(pMe);
   //end tcl wgs 050119 added 
   tmpVal = pMe->m_paramAttr;
   if(tmpVal & LIMIT_THREE_MINTUES)
      DispRecordClockTimer(pMe);
}//DrawPicBroad()

/*===========================================================================
   init
===========================================================================*/
static void InitArray(PinTu *pMe)
{
   site emptySite,tmpSite;
   int16 temp,i,j;

   if(pMe == NULL) return ;

   //init data

   emptySite.x = pMe->m_arrayInfo.col;
   emptySite.y = pMe->m_arrayInfo.row;

   for(i = 0;i<pMe->m_arrayInfo.row;i++)
      for(j = 0;j<pMe->m_arrayInfo.col;j++)
         pMe->m_siteinfo[i][j] = (i*pMe->m_arrayInfo.col)+j+1;

   for(i = 0;i < MAX_RAND_NUM;i++)
   {
      temp = (int16)GetRndNumber(MOVE_DIRTORY_NUM + 1);
      switch(temp)
      {
         case 1://left
            if(emptySite.x > 1)
            {
               tmpSite.x = emptySite.x -1;
               tmpSite.y = emptySite.y;
               pMe->m_siteinfo[emptySite.y -1][emptySite.x -1] = pMe->m_siteinfo[tmpSite.y-1][tmpSite.x-1];
               emptySite.x -= 1;
               pMe->m_siteinfo[emptySite.y-1][emptySite.x-1] = pMe->m_arrayInfo.max_block;//移动后的位置置为空(9)
            }
            break;
         case 2://right
            if(emptySite.x < pMe->m_arrayInfo.col)
            {
               tmpSite.x = emptySite.x+1;
               tmpSite.y = emptySite.y;
               pMe->m_siteinfo[emptySite.y -1][emptySite.x -1] = pMe->m_siteinfo[tmpSite.y-1][tmpSite.x-1];
               emptySite.x += 1;
               pMe->m_siteinfo[emptySite.y-1][emptySite.x-1] = pMe->m_arrayInfo.max_block;//移动后的位置置为空(9)
            }
            break;
         case 3://up
            if(emptySite.y > 1)
            {
               tmpSite.x = emptySite.x;
               tmpSite.y = emptySite.y - 1;
               pMe->m_siteinfo[emptySite.y -1][emptySite.x -1] = pMe->m_siteinfo[tmpSite.y-1][tmpSite.x-1];
               emptySite.y -= 1;
               pMe->m_siteinfo[emptySite.y-1][emptySite.x-1] = pMe->m_arrayInfo.max_block;//移动后的位置置为空(9)
            }
            break;
         case 4://down
            if(emptySite.y < pMe->m_arrayInfo.row)
            {
               tmpSite.x = emptySite.x;
               tmpSite.y = emptySite.y + 1;
               pMe->m_siteinfo[emptySite.y -1][emptySite.x -1] = pMe->m_siteinfo[tmpSite.y-1][tmpSite.x-1];
               emptySite.y += 1;
               pMe->m_siteinfo[emptySite.y-1][emptySite.x-1] = pMe->m_arrayInfo.max_block;//移动后的位置置为空(9)
            }
            break;
         default:
            break;
      }//switch(temp)
   }//for(i=0;i<300;i++)

   for(i = 0;i<pMe->m_arrayInfo.row;i++)
      for(j = 0;j<pMe->m_arrayInfo.col;j++)
      {
         pMe->m_Resultarray[i*pMe->m_arrayInfo.row + j]   = pMe->m_siteinfo[i][j];
      }
      pMe->m_moveStep        = 0;
      pMe->m_recordClock     = RECORD_TIMER;        //allow the time of move
      pMe->m_viewOrigPicTime = MAX_VIEW_PIC_TIME;   //resume default value to five times

}//InitArray()

/*===========================================================================
   get image
===========================================================================*/
static IImage* GetImage(PinTu *pMe,int16 itemID)
{
   char    catalogIdx[MAX_CATALOG_NUM][MAX_FILENAME_LEN] = {PINTU_RING_DIR,DOWNLOAD_IMAGE_DIR};
   MSG_FATAL("GetImage..........0000000",0,0,0);
   if(pMe == NULL) return NULL;
   MSG_FATAL("GetImage..........000",0,0,0);
   if(itemID)
   {
      char szFile[MAX_FILENAME_LEN] = {'\0'};
      int16 i;
      MSG_FATAL("GetImage..........111",0,0,0);
      for(i = 0;i < MAX_CATALOG_NUM;i++)
      {
         if(i < MAX_CATALOG_NUM - 1 && pMe->m_firstItemID[i + 1] != 0)
         {
            if((itemID >= pMe->m_firstItemID[i]) &&(itemID < pMe->m_firstItemID[i + 1]))
               break;
         }
         else
         {
            if(itemID >= pMe->m_firstItemID[i]) break;
         }
      }//end of for()
	  MSG_FATAL("GetImage..........222",0,0,0);
      if(0 == i)
      {
        pMe->m_CfgData.flag = TRUE;
      }
      else
      {
        pMe->m_CfgData.flag = FALSE;
      }
      MEMSET(pMe->m_CfgData.imagename,0,sizeof(pMe->m_CfgData.imagename));
      STRCPY(pMe->m_CfgData.imagename,pMe->m_loadImageFile);
      MSG_FATAL("THE SAVE IMAGE FILE %s",pMe->m_CfgData.imagename,0,0);
      STRCPY(szFile,catalogIdx[i]);
      STRCAT(szFile, DIRECTORY_STR);
      STRCAT(szFile,pMe->m_loadImageFile);
       MSG_FATAL("THE SAVE IMAGE FILE %s",pMe->m_loadImageFile,0,0);
      MSG_FATAL("szFile =%s",szFile,0,0); 
      return ISHELL_LoadImage(pMe->a.m_pIShell,szFile);
   }//end of if()
   else
   {
   		return ISHELL_LoadResImage(pMe->a.m_pIShell,PINTU_RES_FILE_LANG,IDB_DEFAULT_IMAGE);
   }

   return  NULL;
}//GetImage()

/*===========================================================================
   start new game
===========================================================================*/
static boolean StartNewGame(PinTu *pMe)
{
   IImage*  pTempImage;
   AEEImageInfo pi;
   RGBVAL   oldTextColor;
   int16    tmpVal;

   if(pMe == NULL) return FALSE;
   MSG_FATAL("start new gamepMe->m_selImageItemID=%d",pMe->m_selImageItemID,0,0);
 // ReadRecordFromDB(pMe);
   pTempImage = GetImage(pMe,pMe->m_selImageItemID);
   if(pTempImage)
   {
      if(pMe->m_pSrcImage)
      {
         IIMAGE_Release(pMe->m_pSrcImage);
         pMe->m_pSrcImage = NULL;
      }
      DBGPRINTF("CHANGE THE DEFAULT IMAGE");
      pMe->m_pSrcImage = pTempImage;
   }
   else
   {
   DBGPRINTF("THE PTMPIMAGE  POINTER IS NULL"); 
   }//end of if()
   if (pMe->m_pSrcImage == NULL) return FALSE;

   PinTu_Reset(pMe);
   oldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND, BACK_GROUND_COLOR);
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

   pMe->m_keyAvaible = TRUE;
   IIMAGE_GetInfo(pMe->m_pSrcImage, &pi);

   //拼图方式3*3 or 4*4
   tmpVal = pMe->m_paramAttr;
   if (tmpVal & LIMIT_TRHEE_AND_THREE)
   {
        pMe->m_arrayInfo.row = 3;
        pMe->m_arrayInfo.col = 3;
   }
   else
   {
        pMe->m_arrayInfo.row = 4;
        pMe->m_arrayInfo.col = 4;
   }
   pMe->m_arrayInfo.max_block = pMe->m_arrayInfo.row*pMe->m_arrayInfo.col;

   //init the width and heigth of gird

   if (pMe->m_pSrcImage)
   {
        int16 height,width;

        height = pMe->m_cyScreen - SCREEN_Y_OFFSET*2 - PROMPT_INFO_HEIGHT;
        width  = pMe->m_cxScreen - SCREEN_X_OFFSET*2;
        if (pi.cy < height)
        {
            pMe->m_girdHeight = pi.cy / pMe->m_arrayInfo.row;
            pMe->m_arrayInfo.screen_yOffSet = (pMe->m_cyScreen - pi.cy -PROMPT_INFO_HEIGHT)/2;
        }
        else
        {
            pMe->m_girdHeight = height/pMe->m_arrayInfo.row;
			pMe->m_arrayInfo.screen_yOffSet = SCREEN_Y_OFFSET;
        }
        if (pi.cx < width)
        {
            pMe->m_girdWidth = pi.cx / pMe->m_arrayInfo.col;
			pMe->m_arrayInfo.screen_xOffSet = (pMe->m_cxScreen - pi.cx)/2;
        }
        else
        {
            pMe->m_girdWidth = width / pMe->m_arrayInfo.col;
			pMe->m_arrayInfo.screen_xOffSet = SCREEN_X_OFFSET;
        }
   }

   if(!pMe->m_continueFlg)  InitArray(pMe);
   DrawPicBroad(pMe);
   //DrawSoundButton(pMe);
   IDISPLAY_Update(pMe->a.m_pIDisplay);
   (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND, oldTextColor);

   return TRUE;

}//StartNewGame()

/*===========================================================================
   judge game success or fail
===========================================================================*/
static boolean JudgeGameResult(PinTu *pMe)
{
   int16 i;
   boolean successFlg = FALSE;

   if(pMe == NULL) return FALSE;

   for(i = 0;i < pMe->m_arrayInfo.max_block;i++)
   {
      if(pMe->m_Resultarray[i] == (uint16)(i+1))
         successFlg = TRUE;
      else
      {
         successFlg = FALSE;
         break;
      }
   }//end of for(;;)

   return successFlg;
}//JudgeGameResult()

/*===========================================================================
   process key event
===========================================================================*/
static boolean ProcessKeyEvent(PinTu *pMe, uint16 wParam)
{

   if(pMe == NULL) return FALSE;

   switch(wParam)
   {
      case AVK_F0:
      case AVK_SELECT:
         switch( pMe->m_eAppState )
         {
            case APP_STATE_IMAGE_SELECT:  //image list
               GetMenuItemData(pMe);
               return TRUE;

            case APP_STATE_IAMGE_PREVIEW: //preview selected image
               pMe->m_selImageItemID = pMe->m_menuLevelStack[1];
               if(pMe->m_selImageItemID != pMe->m_lastSelImageID)//重新选择图片将取消继续功能
               {
            	   pMe->m_lastSelImageID = pMe->m_selImageItemID;
            	   pMe->m_continueFlg = FALSE;
               }
               (void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_PINTU, EVT_COMMAND, IDS_SELECT_IMAGE, 0);
               return TRUE;

            case APP_STATE_SETTING: //set param of spelling
               (void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_PINTU, EVT_COMMAND, USRE_DRAW, 0);
               return TRUE;

            case APP_STATE_HELP:
                PinTu_DisplayMainScreen(pMe);
                return TRUE;
            //tcl wgs 050124 added    
            case APP_STATE_SOUNDSET:
                PinTu_DisplayMainScreen(pMe);
                return TRUE;
                 
            default:
               break;
         }
         break;

      case AVK_CLR:
         switch(pMe->m_eAppState)
         {
            case APP_STATE_START:
               ISHELL_CancelTimer(pMe->a.m_pIShell,NULL,pMe);
               pMe->m_keyAvaible = TRUE;
               pMe->m_continueFlg = TRUE;

           case APP_STATE_SETTING:
           case APP_STATE_HELP:
           case APP_STATE_ABOUT:
           case APP_STATE_SOUNDSET://tcl wgs 050124 added
               PinTu_DisplayMainScreen(pMe);
               //pMe->m_eAppState  = APP_STATE_MAIN_MENU;
               return TRUE;

            case APP_STATE_IMAGE_SELECT:
               AlertDispCallBack(pMe);       //return main menu
               pMe->m_menuLevelStack[1] = 0; //delect data of saving in the next menu level
               return TRUE;

            case APP_STATE_IAMGE_PREVIEW:
                if (pMe->m_pViewSelectImage) IIMAGE_Stop(pMe->m_pViewSelectImage);  //stop the

               (void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_PINTU, EVT_COMMAND, IDS_SELECT_IMAGE, 0);
               return TRUE;

            case APP_STATE_MAIN_MENU:
            //SavePinTuConfigData( pMe);
               // AddRecordToDB( pMe);//tcl wgs 050127 added
               (void)ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);
               return TRUE;

            default:
               break;
         }
         break;

      case AVK_DOWN:
         if((pMe->m_curGrid.ID > pMe->m_arrayInfo.col)&&(pMe->m_eAppState == APP_STATE_START)&&(!pMe->m_endGameFlg) && pMe->m_keyAvaible)
         {
            MEMCPY(&pMe->m_historyGrid,&pMe->m_curGrid,sizeof(gridInfo));
            pMe->m_curGrid.ID -= pMe->m_arrayInfo.col;
            pMe->m_curGrid.begin_y -= (pMe->m_girdHeight + BETWEEN_LEN );
            pMe->m_moveStep++;
            DispMoveStepInfo(pMe);
         }
         break;

      case AVK_UP:
         if((pMe->m_curGrid.ID <=( pMe->m_arrayInfo.max_block - pMe->m_arrayInfo.col))&&(pMe->m_eAppState == APP_STATE_START)&&(!pMe->m_endGameFlg) && pMe->m_keyAvaible)
         {
            MEMCPY(&pMe->m_historyGrid,&pMe->m_curGrid,sizeof(gridInfo));
            pMe->m_curGrid.ID += pMe->m_arrayInfo.col;
            pMe->m_curGrid.begin_y += (pMe->m_girdHeight + BETWEEN_LEN);
            pMe->m_moveStep++;
            DispMoveStepInfo(pMe);
         }
         break;

      case AVK_RIGHT:
         if((pMe->m_eAppState == APP_STATE_START)&&(!pMe->m_endGameFlg)  && pMe->m_keyAvaible)
         {
            uint16 i;

            for(i = 0;i<pMe->m_arrayInfo.row;i++)
            {
                if (pMe->m_curGrid.ID == (pMe->m_arrayInfo.col*i + 1))
                    return FALSE;

            }
            MEMCPY(&pMe->m_historyGrid,&pMe->m_curGrid,sizeof(gridInfo));
            pMe->m_curGrid.ID -= MIN_MOVE_NUM;
            pMe->m_curGrid.begin_x -= (pMe->m_girdWidth + BETWEEN_LEN);
            pMe->m_moveStep++;
            DispMoveStepInfo(pMe);

         }

         break;

      case AVK_LEFT:
         if((pMe->m_eAppState == APP_STATE_START)&&(!pMe->m_endGameFlg)  && pMe->m_keyAvaible)
         {
            uint16 i;

            for(i = 1;i<=pMe->m_arrayInfo.row;i++)
            {
                if (pMe->m_curGrid.ID == (pMe->m_arrayInfo.col*i))
                    return FALSE;
            }
            MEMCPY(&pMe->m_historyGrid,&pMe->m_curGrid,sizeof(gridInfo));
            pMe->m_curGrid.ID += MIN_MOVE_NUM;
            pMe->m_curGrid.begin_x += (pMe->m_girdWidth + BETWEEN_LEN);
            pMe->m_moveStep++;
            DispMoveStepInfo(pMe);

         }
         break;

      case AVK_F1://preview select picture for pintu
      //case AVK_STAR:
         if(pMe->m_eAppState == APP_STATE_START)
         {
            pMe->m_keyAvaible = FALSE;
            ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY) DispRecordClockTimer,pMe);
            if(pMe->m_viewOrigPicTime <= 0)
            {
               DispCommonInfo(pMe,IDS_ALERT_INFO,IDS_VIEW_PIC_WARNING,NULL);
               ISHELL_SetTimer(pMe->a.m_pIShell,ALRAM_TIMER,(PFNNOTIFY) ViewPicWarnCallBack,pMe);
            }
            else
            {
               return PreViewSelectedPic(pMe);
            }
         }

         return FALSE;

      case AVK_F2://preview select picture for pintu
         switch(pMe->m_eAppState)
         {
            case APP_STATE_IMAGE_SELECT:
                  pMe->m_eAppState = APP_STATE_IAMGE_PREVIEW;
                  PreViewSelectImageScreen(pMe);
			      return TRUE;

            case APP_STATE_SETTING:
               SetMenuItemData(pMe);
               return TRUE;

            default:
               break;
         }
         break;

      default:
         break;
   }//switch()

   if((pMe->m_eAppState == APP_STATE_START)&&(!pMe->m_endGameFlg) && pMe->m_keyAvaible &&
      (wParam == AVK_UP  || wParam == AVK_DOWN || wParam == AVK_LEFT || wParam == AVK_RIGHT ))
   {
      uint16 moveToID;

      moveToID = JudgeIsMove(pMe,pMe->m_curGrid.ID);      
      if(moveToID != 0) //can move block 
      {
         MediaType type = DROP_PIECE_MEDIA;      //default drop piece   
         
         MEMCPY(&pMe->m_historyGrid,&pMe->m_curGrid,sizeof(gridInfo));
         DrawNextGirdPlace(pMe,moveToID);
         if(pMe->m_endGameFlg)
         {
            type = SUCCESS_MEIDA;   //speiling image successfully
            ISHELL_SetTimer(pMe->a.m_pIShell, GAME_OVER_TIMER,(PFNNOTIFY) GameOverCallBack, pMe);
         }
         if(pMe->m_Soundflag)
         {
         	MSG_FATAL("type===============%d",type,0,0);
             PlayerSound(pMe->m_promptVoice,type);  //player background music  
         }       
      }
   }

   return FALSE;
}//ProcessKeyEvent()

/*===========================================================================
   draw a empty diamond
===========================================================================*/
static void DrawEmptyDiamonds(PinTu *pMe)
{
   AEERect tmpRect;

   if(pMe == NULL) return;

   SETAEERECT(&tmpRect,pMe->m_historyGrid.begin_x,
               pMe->m_historyGrid.begin_y,
               pMe->m_historyGrid.width,
               pMe->m_historyGrid.height);

   IDISPLAY_FillRect(pMe->a.m_pIDisplay,&tmpRect,EMPTY_GRID_COLOR);
}//DrawEmptyDiamonds()

/*===========================================================================
   get division image
===========================================================================*/
static IImage *GetNeedImage(PinTu *pMe,IImage *pSrcImage,int16 offset_x,int16 offset_y)
{
   if(pSrcImage)
   {
      IIMAGE_SetOffset(pSrcImage,offset_x, offset_y);
      IIMAGE_SetDrawSize(pSrcImage, pMe->m_girdWidth, pMe->m_girdHeight);
      return pSrcImage;
   }
   return NULL;
}//GetNeedImage()

/*===========================================================================
   darw division image
===========================================================================*/
static void DrawGridBmp(PinTu *pMe,IImage *pImage,int16 gridID)
{
   if(pMe == NULL) return;

   if(pImage)
   {
      int16 begin_x,begin_y,i,j;

      GetRowAndCol(pMe,&i,&j,gridID);
      begin_x = pMe->m_arrayInfo.screen_xOffSet + j*(pMe->m_girdWidth + BETWEEN_LEN);
      begin_y = pMe->m_arrayInfo.screen_yOffSet + i*(pMe->m_girdHeight + BETWEEN_LEN);
      IIMAGE_Draw(pImage,begin_x,begin_y);
      DispTopNumberLable(pMe,pMe->m_Resultarray[gridID -1],begin_x ,begin_y );

      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,FALSE);
   }//if(pImage)()

}//DrawGridBmp()

/*===========================================================================
   get rand num
===========================================================================*/
static uint32 GetRndNumber(uint32 max)
{
   uint32 rnd;

   //gain rand number
   GETRAND( (void *) &rnd, sizeof(rnd));

   return rnd % max;
}//GetRndNumber()

/*===========================================================================
   当选择一个方块时，判断当前选中的方块是否可以移动
===========================================================================*/
static int16 JudgeIsMove(PinTu *pMe,int16 selGridID)
{
   int16 i ,j,k,emptyGridID = 0;
   uint16 tmpVal = 0;

   if((pMe == NULL)&&(pMe->m_Resultarray[selGridID-1] != pMe->m_arrayInfo.max_block)) return emptyGridID;

   for(i = 0;i<pMe->m_arrayInfo.row;i++)
   {
        for(j = 1;j<=pMe->m_arrayInfo.col;j++)
        {
            tmpVal = i*pMe->m_arrayInfo.col+j;

            if(pMe->m_Resultarray[tmpVal - 1] != pMe->m_arrayInfo.max_block)
                continue;

            if (ABS(selGridID - tmpVal) == 1)
            {
                //same the row diamond
                for(k = 0;k<pMe->m_arrayInfo.row;k++)
                {
                    uint16 leftVal,rightVal;
                    leftVal  = pMe->m_arrayInfo.col*k+1;
                    rightVal = pMe->m_arrayInfo.col*(k+1);

                    if((selGridID >= leftVal && selGridID <= rightVal) &&
                       (tmpVal    >= leftVal && tmpVal <= rightVal))
                    {
                        return tmpVal;
                    }
                }
            }// end of if(ABS)
            else if (ABS(selGridID - tmpVal) == pMe->m_arrayInfo.col)
            {
                return tmpVal;
            }
        }
   } //end of for()

   return 0;

} //JudgeIsMove()

/*===========================================================================
   将选择中的方块移到空位置处
===========================================================================*/
static void DrawNextGirdPlace(PinTu *pMe,int16 nextGridID)
{
   uint16  tmpVal = pMe->m_Resultarray[pMe->m_historyGrid.ID - 1];

   if(pMe == NULL) return ;

   if(nextGridID <= pMe->m_arrayInfo.max_block) pMe->m_Resultarray[nextGridID - 1] = tmpVal;
   if(pMe->m_historyGrid.ID <= pMe->m_arrayInfo.max_block) pMe->m_Resultarray[pMe->m_historyGrid.ID - 1] = pMe->m_arrayInfo.max_block;
   if(tmpVal <= pMe->m_arrayInfo.max_block)
   {
      int16 offset_x,offset_y,row,col;

      //draw the last block for game success
      if(JudgeGameResult(pMe))
      {
         GetRowAndCol(pMe,&row,&col,pMe->m_curGrid.ID);
         offset_x = IMAGE_BEGIN_X + col*pMe->m_girdWidth;
         offset_y = IMAGE_BEGIN_Y + row*pMe->m_girdHeight;
         if(pMe->m_pSrcImage)
         {
            IImage *pImage = GetNeedImage(pMe,pMe->m_pSrcImage,offset_x,offset_y);
            DrawGridBmp(pMe,pImage,pMe->m_curGrid.ID);
         }
         pMe->m_endGameFlg = TRUE;
         pMe->m_continueFlg = FALSE;
      }
      else
      {
         DrawEmptyDiamonds(pMe);
      }

      //draw selected block to next place
      GetRowAndCol(pMe,&row,&col,tmpVal);
      offset_x = IMAGE_BEGIN_X + col*pMe->m_girdWidth;
      offset_y = IMAGE_BEGIN_Y + row*pMe->m_girdHeight;
      if(pMe->m_pSrcImage)
      {
         IImage *pImage = GetNeedImage(pMe,pMe->m_pSrcImage,offset_x,offset_y);
         DrawGridBmp(pMe,pImage,nextGridID);

      }//end of if(pMe->m_pSrcImage)
   }//end of if(tmpVal <= pMe->m_arrayInfo.max_block)
}//DrawNextGirdPlace()

/*===========================================================================

===========================================================================*/
static void ViewPicWarnCallBack(PinTu *pMe)
{

    uint16 wParam;
   if(pMe == NULL) return;

   pMe->m_keyAvaible = TRUE;

   switch(pMe->m_promptType)
   {
        case PRVIEW_IMAGE:
            wParam = IDS_GAME_CONTINUE;
            break;

        case AVAIBLE_IMAGE:
            wParam = USRE_DRAW;
            break;

        default:
            return;
   }

   (void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_PINTU, EVT_COMMAND, wParam, 0);
}

/*===========================================================================

===========================================================================*/
static void DispCommonInfo(PinTu *pMe,uint16 titleID,uint16 contentID,AECHAR *szBuf)
{
   AEERect     dispRect;
   AECHAR      textBuf[MAX_CONTENT_SIZE];
   RGBVAL      oldTextColor;

   if(pMe == NULL)  return ;

   DrawCtrlFrame(pMe,titleID,&pMe->m_popWinFrameRect);
   (void)MEMCPY(&dispRect,&pMe->m_popWinFrameRect,sizeof(AEERect));

   oldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, MAKE_RGB(0,0,0));
   ISTATIC_SetProperties(pMe->m_pStaticCtl, ST_CENTERTEXT|ST_CENTERTITLE |ST_MIDDLETEXT);
   ISTATIC_SetRect(pMe->m_pStaticCtl, &dispRect);

   if(szBuf)
   {
      ISTATIC_SetText(pMe->m_pStaticCtl,NULL, szBuf, AEE_FONT_NORMAL, AEE_FONT_NORMAL );
   }
   else
   {
      ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG, contentID, textBuf,sizeof(textBuf));
      ISTATIC_SetText(pMe->m_pStaticCtl,NULL, textBuf, AEE_FONT_NORMAL, AEE_FONT_NORMAL );
   }
   ISTATIC_Redraw(pMe->m_pStaticCtl);
   (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, oldTextColor);
}

/*===========================================================================

===========================================================================*/
static void ViewPicWarning(PinTu *pMe)
{
   AECHAR szBuf[MAX_CONTENT_SIZE],szTmp[MAX_CONTENT_SIZE];

   if(pMe == NULL) return ;

   switch(pMe->m_promptType)
   {
        case PRVIEW_IMAGE:
            ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG,IDS_VIEW_PIC_INFO, szTmp, sizeof(szTmp));
            WSPRINTF(szBuf,MAX_CONTENT_SIZE *sizeof(AECHAR),szTmp,pMe->m_viewOrigPicTime);
            break;

        case AVAIBLE_IMAGE:
            ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG,IDS_START_GAME_ALARM, szBuf, sizeof(szBuf));
            break;

        default:
            return;
   }

   DispCommonInfo(pMe,IDS_ALERT_INFO,NULL,szBuf);
   ISHELL_SetTimer(pMe->a.m_pIShell,ALRAM_TIMER,(PFNNOTIFY) ViewPicWarnCallBack,pMe);
}//ViewPicWarning()


/*===========================================================================

===========================================================================*/
static void DispMainScrCallBack(PinTu *pMe)
{
   int16  tmpVal;

   if(pMe == NULL) return;


   tmpVal = pMe->m_paramAttr;
   if(tmpVal & LIMIT_FIVE_DEGREES)
   {
      if(pMe->m_viewOrigPicTime > 0) pMe->m_viewOrigPicTime -- ;
      {
          pMe->m_promptType = PRVIEW_IMAGE;
          ViewPicWarning(pMe);
      }
   }
   else
   {
      pMe->m_keyAvaible = TRUE;
      (void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_PINTU, EVT_COMMAND, IDS_GAME_CONTINUE, 0);
   }
}//DispMainScrCallBack()

/*===========================================================================
   preview list picture of bmp format to select play
===========================================================================*/
static boolean PreViewSelectedPic(PinTu *pMe)
{
   boolean retVal = FALSE;
   AEERect tempRect,frameRect;
   RGBVAL  oldTextColor;

   if(pMe == NULL) return retVal;

   if(pMe->m_pSrcImage)
   {
      PinTu_Reset(pMe);

      oldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND, BACK_GROUND_COLOR);
      SETAEERECT(&tempRect,0,0,pMe->m_cxScreen,pMe->m_cyScreen - PROMPT_INFO_HEIGHT);
      IDISPLAY_EraseRect(pMe->a.m_pIDisplay,&tempRect);
      IIMAGE_SetOffset(pMe->m_pSrcImage,IMAGE_BEGIN_X, IMAGE_BEGIN_Y);
      IIMAGE_SetDrawSize(pMe->m_pSrcImage, pMe->m_girdWidth*pMe->m_arrayInfo.col, pMe->m_girdHeight*pMe->m_arrayInfo.row);
      IIMAGE_Draw(pMe->m_pSrcImage,pMe->m_arrayInfo.screen_xOffSet,pMe->m_arrayInfo.screen_yOffSet);
      //draw the picture frame
      SETAEERECT(&frameRect,pMe->m_arrayInfo.screen_xOffSet,pMe->m_arrayInfo.screen_yOffSet,pMe->m_girdWidth*pMe->m_arrayInfo.col,pMe->m_girdHeight*pMe->m_arrayInfo.row);
      DispPicFrame(pMe,&frameRect);
      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,FALSE);
      (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND, oldTextColor);
      ISHELL_SetTimer(pMe->a.m_pIShell,DISP_PREVIEW_PIC_TIMER,(PFNNOTIFY) DispMainScrCallBack,pMe);
      retVal = TRUE;
   }//end of if(pMe->m_pSrcImage)
   return retVal;
}//PreViewSelectedPic()

///////////////////////////////////////////////
//callback of alert display
//////////////////////////////////////////////
static void AlertDispCallBack(PinTu *pMe)
{
   if(pMe == NULL) return;

   (void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_PINTU, EVT_COMMAND, USRE_DRAW, 0);

}//AlertDispCallBack()

/*===========================================================================
   draw frame of menu or static
===========================================================================*/
static void DrawCtrlFrame(PinTu *pMe,uint16 titleID,AEERect *fRect)
{
   AEERect frameRect;
   AEERect tempRect;
   AECHAR  szTile[16];
   RGBVAL  oldTextColor;

   if(pMe == NULL)  return ;

   if(!IGRAPHICS_GetFillMode(pMe->m_pGraphics))
   {
      (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics,TRUE);
   }
   IGRAPHICS_SetFillColor(pMe->m_pGraphics,205,205,205,0);
   IGRAPHICS_SetColor(pMe->m_pGraphics,0,0,0,0);
   //draw window frame

   frameRect.x  = fRect->x-5;
   frameRect.y  = fRect->y-32;
   frameRect.dx = fRect->dx+10;
   frameRect.dy = fRect->dy+38;
   (void)MEMCPY(&tempRect,&frameRect,sizeof(AEERect));

   (void)IGRAPHICS_DrawRoundRectangle(pMe->m_pGraphics,&frameRect,32,32);
   oldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, MAKE_RGB(0,0,255));
   if(titleID)
   {
      ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG,titleID, szTile, sizeof(szTile));
      tempRect.y += 2;
      (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL,szTile, -1,0, 0,&tempRect,
                              IDF_ALIGN_CENTER|IDF_ALIGN_TOP|IDF_TEXT_TRANSPARENT);
   }
   (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, oldTextColor);
   IGRAPHICS_Update(pMe->m_pGraphics);
}//DrawCtrlFrame()

/*===========================================================================
   display alert information
===========================================================================*/
static void DispAlertInfo(PinTu *pMe,uint16 titleID,uint16 contentID,AECHAR *szBuf)
{

   AEERect     dispRect;
   AECHAR      textBuf[MAX_CONTENT_SIZE];
   RGBVAL      oldTextColor;

   if(pMe == NULL)  return ;

   DrawCtrlFrame(pMe,titleID,&pMe->m_popWinFrameRect);
   (void)MEMCPY(&dispRect,&pMe->m_popWinFrameRect,sizeof(AEERect));

   oldTextColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, MAKE_RGB(0,0,0));
   ISTATIC_SetProperties(pMe->m_pStaticCtl, ST_CENTERTEXT|ST_CENTERTITLE |ST_MIDDLETEXT);
   ISTATIC_SetRect(pMe->m_pStaticCtl, &dispRect);

   if(szBuf)
   {
      ISTATIC_SetText(pMe->m_pStaticCtl,NULL, szBuf, AEE_FONT_NORMAL, AEE_FONT_NORMAL );
   }
   else
   {
      ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG, contentID, textBuf,sizeof(textBuf));
      ISTATIC_SetText(pMe->m_pStaticCtl,NULL, textBuf, AEE_FONT_NORMAL, AEE_FONT_NORMAL );
   }
   ISTATIC_Redraw(pMe->m_pStaticCtl);
   (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, oldTextColor);
   ISHELL_SetTimer(pMe->a.m_pIShell, ALRAM_TIMER,(PFNNOTIFY) AlertDispCallBack, pMe);
}//DispAlertInfo()

/*===========================================================================
   display alert information
===========================================================================*/
static void GameOverCallBack(PinTu *pMe)
{
   if(pMe == NULL)  return ;

   DispAlertInfo(pMe,IDS_ALERT_INFO,IDS_GAME_OVER,NULL);
}

/*===========================================================================
   get need format file for bmp picture
===========================================================================*/
static boolean CheckNeedFormatFile(const char * fileName)
{
   char *pFile;
   char tmpName[5];

   pFile = STRRCHR(fileName,(int)'.');
   if(pFile == NULL) return FALSE;

   pFile++;
   if(STRLEN(pFile) >=3 && STRLEN(pFile) <= 4)
   {
      STRCPY(tmpName,pFile);
      if((STRCMP(STRLOWER(tmpName),"bmp")  == 0) ||(STRCMP(STRLOWER(tmpName),"png")  == 0)) return TRUE;

   }
   return FALSE;
}//CheckNeedFormatFile()

/*===========================================================================
   get bmp file name
===========================================================================*/
static char * GetFileName(const char * psz)
{
   char *   pszName = STRRCHR(psz, (int)DIRECTORY_CHAR);

   if (pszName)
      pszName++;
   else
      pszName = (char *)psz;

   return pszName;
}//GetFileName()

/*===========================================================================
   show origent bmp
===========================================================================*/
static boolean ShowOrigentBmpScreen(PinTu *pMe)
{
	IFileMgr * pIFileMgr = NULL;
	uint16     wItemID = 0;
	FileInfo   fi;
	AEERect	   rc;
	AECHAR     szBuf[MAX_FILENAME_LEN];
	uint16     wImageID;
	uint16     i = 0,j = 0;
	boolean    isFile = FALSE;	
	int16      startPoint;	
	char       catalogIdx[MAX_CATALOG_NUM][MAX_FILENAME_LEN] = {PINTU_RING_DIR,DOWNLOAD_IMAGE_DIR};

   if(pMe == NULL) return FALSE;

   if(!CreateObj(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr)) return FALSE;

   MEMSET(pMe->m_firstItemID,0,sizeof(pMe->m_firstItemID));
   for(i = 0;i < 2;i++)
   {
      if(IFILEMGR_Test(pIFileMgr,catalogIdx[i]) != SUCCESS)
         isFile = FALSE;
      else
	   {
         isFile = TRUE;
         break;
	   }
   }
   if(!isFile)
   {
      ReleaseObj((void **)&pIFileMgr);
	   return FALSE;
   }

	PinTu_Reset(pMe);

	// Enumerate ring directory
	for(i = 0;i < MAX_CATALOG_NUM;i++)
	{
       	IFILEMGR_EnumInit(pIFileMgr, catalogIdx[i], FALSE);
       	while((wItemID < MAX_FILES_NUM)&& IFILEMGR_EnumNext(pIFileMgr, &fi))
       	{
            char * szName;

            if (fi.dwSize == 0) continue; //tcl jhh050317 过虑文件大小为 0 字节的文件
       		// Add only the file name
       		szName = GetFileName(fi.szName);
       		if(!CheckNeedFormatFile(szName)) continue;

       	    MEMSET(szBuf,0,sizeof(szBuf));
       		(void)UTF8TOWSTR((byte *)szName, STRLEN(szName)+1,szBuf,sizeof(szBuf));
            if(pMe->m_selImageItemID == (wItemID+BASE_BMP_ITEM_ID))
                wImageID = IDB_PINTU_RADIO_SEL;
             else
                wImageID = IDB_PINTU_RADIO_UNSEL;
       		(void)Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG,NULL, szBuf, wImageID, (uint16)(wItemID+BASE_BMP_ITEM_ID), 0);
       		if( j == 0)
       		{
       		   pMe->m_firstItemID[i] = (uint16)(wItemID + BASE_BMP_ITEM_ID);
       		   j++;
       	    }
       	    wItemID++;
       	}//while()
       	j = 0;
   } //end of for()
   ReleaseObj((void **)&pIFileMgr);
   //not any ringer file
   if(wItemID == 0) return FALSE;

   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

   startPoint =  DrawTopBackGroundBMP(pMe,IDS_SELECT_IMAGE,NULL);
   //set menu area
	rc.x = 0;
	rc.y = startPoint;
	rc.dx = pMe->m_cxScreen;
	rc.dy = pMe->m_cyScreen - PINTU_SOTFKEY_CY - startPoint;
   (void)Brew_SetMenuAttr(pMe->m_pMenuCtl,AEECLSID_MENUCTL,&rc,MP_MAXSOFTKEYITEMS);//set menu attible

   if(pMe->m_menuLevelStack[1] == 0)
       IMENUCTL_SetSel(pMe->m_pMenuCtl,pMe->m_selImageItemID);
   else
       IMENUCTL_SetSel(pMe->m_pMenuCtl,pMe->m_menuLevelStack[1]);
   IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
   IMENUCTL_Redraw(pMe->m_pMenuCtl);
#if defined(AEE_STATIC) 
    DRAW_PROMPT_BAR(BTBAR_PRIVIEW_BACK);
#else
   //add softkey prompt
   PinTu_AddSoftkeyPrompt(pMe,IDS_SELECT,IDS_PRVIEW_IMAGE,IDS_RETURN);
#endif //AEE_STATIC   
   //IDISPLAY_Update(pMe->a.m_pIDisplay);
   return TRUE;
}//ShowOrigentBmpScreen()

/*===========================================================================
   get current menu item data
===========================================================================*/
static int16 GetCurSelectMenuItemID(PinTu *pMe,char *pFileName)
{
   CtlAddItem ai;
	uint16     nSelect;
	char       fileName[MAX_FILENAME_LEN]={'\0'};

   if(pMe == NULL) return 0;

   // Initialize CtlAddItem
	MEMSET(&ai, 0, sizeof(ai));

	//gain current selected menu item
	nSelect = IMENUCTL_GetFocus(pMe->m_pMenuCtl);
	IMENUCTL_GetItem(pMe->m_pMenuCtl, nSelect, &ai);
	(void)WSTRTOUTF8(ai.pText,WSTRLEN(ai.pText),(byte *)fileName,sizeof(fileName));
	STRCPY(pFileName,fileName);
	return nSelect;
}//GetCurSelectMenuItemID()

/*===========================================================================
   get menu item data
===========================================================================*/
static void GetMenuItemData(PinTu *pMe)
{
   char pFileName[MAX_FILENAME_LEN]={'\0'};

   if(pMe == NULL) return;

	pMe->m_selImageItemID = GetCurSelectMenuItemID(pMe,pFileName);
	if(pMe->m_selImageItemID != pMe->m_lastSelImageID)//重新选择图片将取消继续功能
	{
	   pMe->m_lastSelImageID = pMe->m_selImageItemID;
	   pMe->m_continueFlg = FALSE;
	}
	STRCPY(pMe->m_loadImageFile, pFileName);
	(void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_PINTU, EVT_COMMAND,USRE_DRAW, 0);
}//GetMenuItemData()

/*===========================================================================
   preview the select image
===========================================================================*/
static boolean PreViewSelectImageScreen(PinTu *pMe)
{
   AEERect	  rc;
   IImage*    pTmpImage = NULL;
   int16      selectID;
   char       fileName[MAX_FILENAME_LEN]={'\0'};
   AECHAR     szFileName[MAX_FILENAME_LEN];
   AECHAR     titleName[MAX_TITLE_SIZE_LEN];
   char      *pTemp;
   int16     startPoint;

   if(pMe == NULL) return FALSE;

   //init title of window buffer
   MEMSET(pMe->m_szwinTitle,0,sizeof(pMe->m_szwinTitle));
   selectID = GetCurSelectMenuItemID(pMe,fileName);
   pMe->m_menuLevelStack[1] = selectID;//record the pre select the menu item

   STRCPY(pMe->m_loadImageFile, fileName);
   pTmpImage = GetImage(pMe,selectID);
   if(pTmpImage == NULL) return FALSE;

   pTemp = STRRCHR(fileName, '.');
   *pTemp = 0;

   (void)UTF8TOWSTR((byte *)fileName, STRLEN(fileName)+1,szFileName,sizeof(szFileName));
   WSTRNCOPYN(titleName,MAX_TITLE_SIZE_LEN,szFileName,MAX_TITLE_SIZE_LEN);
   WSTRCPY(pMe->m_szwinTitle,titleName);
	//set area of image ctrl
   startPoint = DrawTopBackGroundBMP(pMe,NULL,pMe->m_szwinTitle);
   rc.x  = 0;
   rc.y  = startPoint;
   rc.dx = pMe->m_cxScreen;
   rc.dy = pMe->m_cyScreen - PINTU_SOTFKEY_CY - startPoint;
   IIMAGECTL_SetRect(pMe->m_pImageCtl, &rc);

   ReleaseObj((void **)&pMe->m_pViewSelectImage); //release the pre image
   pMe->m_pViewSelectImage = pTmpImage;

   PinTu_Reset(pMe);
   DispSelectPreviewImage(pMe);
   return TRUE;
}//PreViewSelectImageScreen()


/*===========================================================================
   preview the selected picture
===========================================================================*/
static void DispSelectPreviewImage(PinTu *pMe)
{

    AEEImageInfo  ImageInfo;
    
    if((pMe == NULL) || (pMe->m_pViewSelectImage == NULL)) return;
    
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    if (pMe->m_pSrcImage)
    {
        IIMAGE_Stop(pMe->m_pSrcImage);  //stop the
    }
    //draw the window title
    //(void)DrawTopBackGroundBMP(pMe,NULL,pMe->m_szwinTitle);
    
    //get image information
    IIMAGE_GetInfo(pMe->m_pViewSelectImage,&ImageInfo);
    if ((ImageInfo.cy <= (pMe->m_cyScreen - PINTU_SOTFKEY_CY)) &&
        (ImageInfo.cx <= pMe->m_cxScreen))
    {
        IIMAGE_Draw(pMe->m_pViewSelectImage,(pMe->m_cxScreen - ImageInfo.cx)/2,
                    (pMe->m_cyScreen - PINTU_SOTFKEY_CY - ImageInfo.cy)/2);
    }
    else
    {
        if (pMe->m_pViewSelectImage)
        {
            IIMAGE_SetParm(pMe->m_pViewSelectImage,IPARM_OFFSET, 5, 5) ;
        }
        
        IIMAGECTL_SetImage(pMe->m_pImageCtl,pMe->m_pViewSelectImage);
        
        IIMAGECTL_Redraw(pMe->m_pImageCtl);
        // Tell the control it is active - handles keypad events...
        IIMAGECTL_SetActive(pMe->m_pImageCtl,TRUE);
    }
    
    //add softkey prompt
#if defined(AEE_STATIC) 
    DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
#else   
    PinTu_AddSoftkeyPrompt(pMe,IDS_SELECT,NULL,IDS_CANCLE);
#endif //AEE_STATIC   
//DrawSoundButton( pMe);//tcl wgs 050120 added
   //IDISPLAY_Update(pMe->a.m_pIDisplay);
}//DispSelectPreviewImage()

/*===========================================================================
   This function add '\n' add new line
===========================================================================*/
static void AddNewLine(AECHAR * szText, AECHAR chDest, AECHAR chSrc)
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
static void DispHelpInforScreen(PinTu* pMe,uint16 wParam)
{
   AECHAR   szBuf[MAX_HELP_INFO_SIZE];
   AECHAR   szTitle[16];
   AEERect  rc;
   uint16   titleID,contentID;
   int16    startPoint;

   PinTu_Reset(pMe);
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

   if(wParam == IDS_CONTROL_HELP)
   {
      titleID   = IDS_CONTROL_HELP ;
      contentID = IDS_HELP_CONTENT;
      pMe->m_eAppState = APP_STATE_HELP;
   }
   else
   {
      titleID   = IDS_ABOUT_VERSION ;
      contentID = IDS_GAME_INFORMATION;
      pMe->m_eAppState = APP_STATE_ABOUT;
   }

   startPoint =  DrawTopBackGroundBMP(pMe,titleID,NULL);
   //title
   rc.x = 0;
   rc.y = startPoint;
   rc.dx = pMe->m_cxScreen;
   rc.dy = pMe->m_cyScreen - PINTU_SOTFKEY_CY - startPoint;


   ISTATIC_SetRect(pMe->m_pStaticCtl, &rc);

   ISTATIC_SetProperties(pMe->m_pStaticCtl, ST_CENTERTITLE | ST_UNDERLINE | ST_NOSCROLL);

   //title name
   ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG, titleID, szTitle, sizeof(szTitle));
   //content
   ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG, contentID, szBuf, sizeof(szBuf));
   AddNewLine(szBuf, (AECHAR)'\\', (AECHAR)'\n');//在资源文件中以\表示需要换行
   ISTATIC_SetText(pMe->m_pStaticCtl, szTitle, szBuf, AEE_FONT_BOLD, AEE_FONT_NORMAL);
   ISTATIC_SetActive(pMe->m_pStaticCtl, TRUE);
   ISTATIC_Redraw(pMe->m_pStaticCtl);

   //add softkey prompt
#if defined(AEE_STATIC) 
    DRAW_PROMPT_BAR(BTBAR_SELECT_BACK);
#else   
   PinTu_AddSoftkeyPrompt(pMe,NULL,NULL,IDS_RETURN);
#endif  //AEE_STATIC   
   //IDISPLAY_Update(pMe->a.m_pIDisplay);
}//DispHelpInforScreen()

/*===========================================================================
   set parameter screen
===========================================================================*/
static void SetParamScreen(PinTu *pMe)
{
   IDisplay *pd =  pMe->a.m_pIDisplay;
   uint16    wImageID = NULL;
   uint16    itemID,wTextID;
   uint16    i,tmpVal;
   AEERect   rc;
   int16     startPoint;
   MSG_FATAL("SetParamScreen Start",0,0,0);
   if (pMe == NULL)   return ;

   PinTu_Reset(pMe);
   IDISPLAY_ClearScreen(pd);

   startPoint =  DrawTopBackGroundBMP(pMe,IDS_SET_PARAM,NULL);
	//set menu area
   rc.x = 0;
   rc.y = startPoint;
   rc.dx = pMe->m_cxScreen;
   rc.dy = pMe->m_cyScreen - PINTU_SOTFKEY_CY - startPoint;
   (void)Brew_SetMenuAttr(pMe->m_pMenuCtl,AEECLSID_MENUCTL,&rc,MP_MAXSOFTKEYITEMS);//set menu attible

   tmpVal = pMe->m_paramAttr;
   for(i = 0;i < SETTING_PARAM_ITEM_NUN;i++)
   {
      itemID = i+ IDS_LIMIT_TIMER;
      wTextID = itemID;
      if((tmpVal >> i) & SINGLE_SELECTED)
         wImageID = IDB_PINTU_RADIO_SEL;
      else
      {
         if(itemID == IDS_MODE_3)
         {
			   wTextID = (tmpVal & LIMIT_TRHEE_AND_THREE) ? IDS_MODE_3 : IDS_MODE_4;
			   wImageID = IDB_PINTU_RADIO_SEL;
		 }
	     else
            wImageID = IDB_PINTU_RADIO_UNSEL;


      }
      (void)Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG,wTextID,  NULL, wImageID,itemID,0); //machine play first
   }

   IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
   IMENUCTL_Redraw(pMe->m_pMenuCtl);
   //add softkey prompt
#if defined(AEE_STATIC) 
    MSG_FATAL("SetParamScreen 1",0,0,0);
    DRAW_PROMPT_BAR(BTBAR_MODIFY_BACK);
#else   
   MSG_FATAL("SetParamScreen 2",0,0,0); 
   PinTu_AddSoftkeyPrompt(pMe,IDS_OK,IDS_MODI_PARAM,IDS_RETURN);
#endif //AEE_STATIC   
   //IDISPLAY_Update(pd);
    MSG_FATAL("SetParamScreen End",0,0,0);
}//SetParamScreen()

/*===========================================================================
   set menu item data
===========================================================================*/
static void SetMenuItemData(PinTu *pMe)
{
   uint16 itemID;
   CtlAddItem pai;
   uint16     tmpVal,histroyVal;
   boolean    selFlg = FALSE;

   if(pMe == NULL || pMe->m_pMenuCtl == NULL) return;

   itemID = IMENUCTL_GetSel(pMe->m_pMenuCtl);
   tmpVal = pMe->m_paramAttr;
   switch(itemID)
   {
      case IDS_LIMIT_TIMER:
         if(tmpVal & LIMIT_THREE_MINTUES)
         {
            pMe->m_paramAttr ^= LIMIT_THREE_MINTUES;
            selFlg = FALSE;
         }
         else
         {
            pMe->m_paramAttr |= LIMIT_THREE_MINTUES;
            selFlg = TRUE;
         }
         break;

      case IDS_LIMIT_PREVIEW_TIMER:
         if(tmpVal & LIMIT_FIVE_DEGREES)
         {
            pMe->m_paramAttr ^= LIMIT_FIVE_DEGREES;
            selFlg = FALSE;
         }
         else
         {
            pMe->m_paramAttr |= LIMIT_FIVE_DEGREES;
            selFlg = TRUE;
            pMe->m_viewOrigPicTime = MAX_VIEW_PIC_TIME;
         }
         break;

      case IDS_MODE_3:
         if(tmpVal & LIMIT_TRHEE_AND_THREE)
         {
            pMe->m_paramAttr ^= LIMIT_TRHEE_AND_THREE;
            pMe->m_paramAttr |= LIMIT_FOUR_AND_FOUR;
         }
         else
         {
            pMe->m_paramAttr |= LIMIT_TRHEE_AND_THREE;
            pMe->m_paramAttr ^= LIMIT_FOUR_AND_FOUR;
         }

         histroyVal = pMe->m_paramHistoryAttr;
         tmpVal = pMe->m_paramAttr;
         if ((histroyVal & LIMIT_TRHEE_AND_THREE) && ( tmpVal & LIMIT_FOUR_AND_FOUR) ||
			 (histroyVal & LIMIT_FOUR_AND_FOUR)   && ( tmpVal & LIMIT_TRHEE_AND_THREE))
         {
            pMe->m_continueFlg = FALSE;
         }
         pMe->m_paramHistoryAttr = pMe->m_paramAttr;
         break;


      case IDS_DISP_NUMBER_LABEL:       // display number lable
         if(tmpVal & DISP_NUMBER_LABLE)
         {
            pMe->m_paramAttr ^= DISP_NUMBER_LABLE;
            selFlg = FALSE;
         }
         else
         {
            pMe->m_paramAttr |= DISP_NUMBER_LABLE;
            selFlg = TRUE;
         }
         break;

      default:
         break;
   }
   MEMSET(&pai,0x0,sizeof(pai));
   pai.pszResImage = PINTU_RES_FILE_LANG;
   if (itemID == IDS_MODE_3)
   //if(itemID == IDS_LIMIT_TIMER || itemID == IDS_LIMIT_PREVIEW_TIMER || IDS_DISP_NUMBER_LABEL)
   {
      AECHAR szTmp[32];

      tmpVal = pMe->m_paramAttr;
      pai.wText = (tmpVal & LIMIT_TRHEE_AND_THREE) ? IDS_MODE_3 : IDS_MODE_4;

      ISHELL_LoadResString(pMe->a.m_pIShell, PINTU_RES_FILE_LANG,pai.wText, szTmp, sizeof(szTmp));
      pai.pText = szTmp;
      (void)IMENUCTL_SetItem(pMe->m_pMenuCtl,itemID,MSIF_TEXT,&pai);
   }
   else
   {
      pai.wImage = selFlg ? IDB_PINTU_RADIO_SEL : IDB_PINTU_RADIO_UNSEL;
      (void)IMENUCTL_SetItem(pMe->m_pMenuCtl,itemID,MSIF_IMAGE,&pai);
   }
   IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
   IMENUCTL_Redraw(pMe->m_pMenuCtl);

}//SetMenuItemData()

/*===========================================================================

FUNCTION:  GetDirection

DESCRIPTION: selected diamonds will move the direction

PARAMETERS:
    dest: empty diamonds
    src : selected diamonds

RETURN VALUE:
   int16:
   -1:  can not move
    0:  move to up
    1:  move to left
    2:  move to right
    3:  move to down
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

===========================================================================*/
static AVKType GetDirection(PinTu *pMe,int16 dest,int16 src)
{
    int16 i,leftVal,rightVal;
    AVKType direct = AEE_INVALID_CODE;

    if (src < 0) return direct;

    //judge left or right move block
    if (ABS(dest - src) == 1)
    {
        //same the row diamond
        for(i = 0;i<pMe->m_arrayInfo.row;i++)
        {
            //left borderline value in the same row
            leftVal  = pMe->m_arrayInfo.col*i+1;
            //right borderline value in the same row
            rightVal = pMe->m_arrayInfo.col*(i+1);

            if ((dest >= leftVal && dest <= rightVal)&& (src  >= leftVal && src <= rightVal))
            {
                direct = ((dest < src) ? AVK_LEFT:AVK_RIGHT);
            }
        }
    }

    //judge up or down move block
    else if (ABS(dest - src) == pMe->m_arrayInfo.col)
    {
        direct = ((dest < src) ? AVK_UP:AVK_DOWN);
    }
    return direct;
}

/*===========================================================================
   get place by pen down to move different block
===========================================================================*/
static boolean GetPlaceByPenDown(PinTu *pMe,uint32 dwParam)
{
    AVKType type;
    int16 row = 0,col = 0,gridNum = -1;

    uint16 x = AEE_GET_X(dwParam);
    uint16 y = AEE_GET_Y(dwParam);
    MSG_FATAL("GetPlaceByPenDown Start",0,0,0);
    if (pMe == NULL ) return FALSE;


    //judge exit button during the game
    if ((y > pMe->m_cyScreen - (EXIT_BUTTON_HEIGHT + PROMPT_INFO_HEIGHT)/2 - 2 ) &&
       ((x > pMe->m_cxScreen/3 + EXIT_BUTTON_WIDTH/2 ) && (x < pMe->m_cxScreen/2 + (EXIT_BUTTON_WIDTH*3)/2-4 ))) 
    {
        return ProcessKeyEvent(pMe,AVK_CLR);
    }
   else if((y > pMe->m_cyScreen - (EXIT_BUTTON_HEIGHT + PROMPT_INFO_HEIGHT)/2 - 2 ) &&
       ((x >pMe->m_cxScreen*2/3 + (EXIT_BUTTON_WIDTH*3)/2 + 10)))
   {
       if(pMe->m_Soundflag)
       {
           //pMe->m_Soundflag = FALSE;
       }
	else
	{
	     //pMe->m_Soundflag = TRUE;
	}
	//DrawSoundButton( pMe);
   	return TRUE;
   }
    //avaible area in the y direction for moving block
    else if ((y < pMe->m_cyScreen - pMe->m_arrayInfo.screen_yOffSet - PROMPT_INFO_HEIGHT) &&  (y > pMe->m_arrayInfo.screen_yOffSet)) //return FALSE;
    {
        row = (y - pMe->m_arrayInfo.screen_yOffSet)/pMe->m_girdHeight ;
        if ((x >= pMe->m_arrayInfo.screen_xOffSet) && (x <= pMe->m_cxScreen - pMe->m_arrayInfo.screen_xOffSet))
        {
            col = ((x - pMe->m_arrayInfo.screen_xOffSet)/pMe->m_girdWidth + 1);
        }

        gridNum = row*pMe->m_arrayInfo.row + col;

        type = GetDirection(pMe,pMe->m_historyGrid.ID,gridNum);

        return ProcessKeyEvent(pMe,(uint16)type);
    }
    return FALSE;
} //GetPlaceByPenDown()

/*===========================================================================
   draw the exit button during the game
===========================================================================*/
static void DrawExitButton(PinTu *pMe)
{
    IImage* pExitButton;

    if (pMe == NULL) return ;


    pExitButton = ISHELL_LoadResImage(pMe->a.m_pIShell,PINTU_RES_FILE_LANG,IDB_EXIT_GAME);
    if (pExitButton)
    {
        AEEImageInfo  ImageInfo;

        IIMAGE_SetParm(pExitButton,IPARM_ROP, AEE_RO_TRANSPARENT, 0);
        IIMAGE_GetInfo(pExitButton, &ImageInfo);
        IIMAGE_Draw(pExitButton,(pMe->m_cxScreen/3 + ImageInfo.cx/2 + 5),(pMe->m_cyScreen - (ImageInfo.cy + PROMPT_INFO_HEIGHT)/2 - 2));
        IIMAGE_Release( pExitButton );
        pExitButton = NULL;
    }
} //DrawExitButton()

//begin tcl wgs 050118 added
static void DrawSoundButton(PinTu *pMe)
{
    IImage* pSoundButton;
    AEEImageInfo  ImageInfo;
    AEERect       rc;

    if (pMe == NULL) return ;

    if(pMe->m_Soundflag)
    {
        pSoundButton = ISHELL_LoadResImage(pMe->a.m_pIShell,PINTU_RES_FILE_LANG,IDB_SOUNDON);
    }
    else
    {    
        pSoundButton = ISHELL_LoadResImage(pMe->a.m_pIShell,PINTU_RES_FILE_LANG,IDB_SOUNDOFF);
    }
    

    
    if (pSoundButton)
   {
	 IIMAGE_SetParm(pSoundButton,IPARM_ROP, AEE_RO_TRANSPARENT, 0);
        IIMAGE_GetInfo(pSoundButton, &ImageInfo);
    	
        rc.x = pMe->m_cxScreen - ImageInfo.cx*2 - 8;
        rc.y = pMe->m_cyScreen - ((ImageInfo.cy + PROMPT_INFO_HEIGHT)/2 )-2;
        rc.dx = ImageInfo.cx + 12;
        rc.dy = ImageInfo.cy + 4;
    
	 IGRAPHICS_ClearRect(pMe->m_pGraphics,&rc);
	 IGRAPHICS_SetFillMode(pMe->m_pGraphics,TRUE);
	 IGRAPHICS_SetColor(pMe->m_pGraphics,0,128,64,0);
        IGRAPHICS_SetFillColor(pMe->m_pGraphics,0,128,64,0);
            
        IGRAPHICS_DrawRect(pMe->m_pGraphics,&rc);
		
	 IIMAGE_Draw(pSoundButton,(pMe->m_cxScreen - ImageInfo.cx*2 - 4),(pMe->m_cyScreen - (ImageInfo.cy + PROMPT_INFO_HEIGHT)/2 - 2));
		
	 IIMAGE_Release( pSoundButton );

	 pSoundButton = NULL;
	}

	IGRAPHICS_Update(pMe->m_pGraphics);
 
} //DrawSoundButton()
//end tcl wgs 050118 added

/*===========================================================================
FUNCTION:  Pintu_SetSoundMode

DESCRIPTION:
    This function sets the sound mode.

PARAMETERS:
    pMe [in] - PinTu structure    
    
RETURN VALUE:
    None.

SIDE EFFECTS:
    Cause the phone display to be updated.
    Set application state to APP_STATE_SOUNDSET.  
===========================================================================*/
static void PinTu_SetSoundMode(PinTu *pMe)
{ 
    int16     x  ;
    int16     fontHeight;
    AEERect   dispRect;

    uint16    cx = pMe->m_cxScreen;
    uint16    cy = pMe->m_cyScreen;
   
    fontHeight = IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay,
                                                     AEE_FONT_NORMAL,
                                                     NULL,
                                                     NULL);
    x = 2 * fontHeight;
  
    
    if(IMENUCTL_IsActive(pMe->m_pMenuCtl))
    {
        IMENUCTL_SetActive(pMe->m_pMenuCtl,FALSE);
        IMENUCTL_Reset(pMe->m_pMenuCtl);
    }  
    
  /*  if(IMENUCTL_IsActive(pMe->m_BottomBar))
    {
        IMENUCTL_SetActive(pMe->m_BottomBar,FALSE);
        IMENUCTL_Reset(pMe->m_BottomBar);
    }
  */
    //画框
  IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR, 0); 
    SETAEERECT(&dispRect, x, 3*x - 4, cx - 2*x, cy - 2*x);
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &dispRect);
        
    if(!IGRAPHICS_GetFillMode(pMe->m_pGraphics))
    {
       (void)IGRAPHICS_SetFillMode(pMe->m_pGraphics, TRUE);
    }
    
    IGRAPHICS_SetFillColor(pMe->m_pGraphics, 232, 232, 232, 0);
    IGRAPHICS_SetColor(pMe->m_pGraphics,
                       GRID_R_COLOR,
                       GRID_G_COLOR,
                       GRID_B_COLOR, 0);
    dispRect.x  -= 2;
    dispRect.y  -= 2;
    dispRect.dx += 4;
    dispRect.dy += 4;
    (void)IGRAPHICS_DrawRoundRectangle(pMe->m_pGraphics, &dispRect, 15, 15);
    IGRAPHICS_Update(pMe->m_pGraphics);
    
    //画菜单
    dispRect.x  += 4;
    dispRect.y  += 4;
    dispRect.dx -= 8;
    dispRect.dy -= 8;
    
    (void)IMENUCTL_DeleteAll(pMe->m_pMenuCtl);
    //IMENUCTL_ResetUniMenu(pMe->m_pMenuCtl);  
    IMENUCTL_SetRect(pMe->m_pMenuCtl, &dispRect);
    //SetUniMenuConfig(pMe->m_pMenuCtl);
    
    if(pMe->m_Soundflag == TRUE)//声音"开"
    {
        Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG, IDS_SOUND_ON, NULL,IDB_SOUND_SELON, IDS_SOUND_ON, 0 );
        Brew_AddMenuItem(pMe->m_pMenuCtl, PINTU_RES_FILE_LANG, IDS_SOUND_OFF, NULL, IDB_SOUND_UNSELOFF, IDS_SOUND_OFF ,0 );
        IMENUCTL_SetSel(pMe->m_pMenuCtl, IDS_SOUND_ON);
    }
    else
    {
        Brew_AddMenuItem(pMe->m_pMenuCtl,PINTU_RES_FILE_LANG, IDS_SOUND_ON, NULL, IDB_SOUND_UNSELOFF,IDS_SOUND_ON, 0 );
        Brew_AddMenuItem(pMe->m_pMenuCtl, PINTU_RES_FILE_LANG, IDS_SOUND_OFF, NULL,IDB_SOUND_SELON, IDS_SOUND_OFF ,0 );
        IMENUCTL_SetSel(pMe->m_pMenuCtl, IDS_SOUND_OFF);
    }
    
    IMENUCTL_SetActive(pMe->m_pMenuCtl, TRUE);
    IMENUCTL_Redraw(pMe->m_pMenuCtl);
    
    pMe->m_eAppState = APP_STATE_SOUNDSET;    
    
}//PinTu_SetSoundMode


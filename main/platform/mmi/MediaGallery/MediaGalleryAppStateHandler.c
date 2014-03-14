/**--------------------------------------------------------------------------
 * MediaGalleryAppStateHandler.c -- This file implements all media gallery 
 * state machine handlers which are uesed by the applet to bring up various
 * scenario to the end user.
 *
 * Copyright(c) 2008 Anylook
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008.10.11       Create.
 *
 **--------------------------------------------------------------------------
 */

#include "MediaGalleryApp.h"

/*===========================================================================
 *                      MACRO DECLARATIONs
 * ==========================================================================
 */
// This macro is used to update the previous state with the smsState and
// then update the smsState variable with the nextState.
#define MGMOVE_TO_STATE(pMe, nextState)  \
                             do{                                       \
                                 MGAppState eNext = nextState;\
                                 pMe->m_ePreState = pMe->m_eCurState;\
                                 pMe->m_eCurState = eNext;           \
                              }while(0)

/*===========================================================================
 *                      TYPE DECLARATIONs
 * ==========================================================================
 */
typedef NextFSMAction (*MG_FSMSTATE_HANDLER)(CMediaGalleryApp* pMe);

/*===========================================================================
 *
 *                      FUNCTION DECLARATIONS
 *
 * ==========================================================================
*/

// List of state handlers (all these are local functions and used only by the
// MediaGalleryAppStateHandler.c)
static NextFSMAction MGStateNoneOperationHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateInitHandler(CMediaGalleryApp* pMe);
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
static NextFSMAction MGStatePwdHandler(CMediaGalleryApp *pMe);
#endif
static NextFSMAction MGStateMainMenuHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStatePhoneMemHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateCardMemHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateMemStatHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateUDiskStateHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateMediaMenuHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateExitHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateImageViewerHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateRenameHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateSetAsHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateOptionHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateSavetoplaylistHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateDetailHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateSortHandler(CMediaGalleryApp *pMe);
static NextFSMAction MGStateSelectPathHandler(CMediaGalleryApp* pMe);

static NextFSMAction MGStateMusicAddHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateVideoAddHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateImageSettingHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGStateSoundSettingHandler(CMediaGalleryApp* pMe);
static NextFSMAction MGState_OnPopupMenuRet(CMediaGalleryApp* pMe);
static void MediaGalleryApp_ShowMsgBoxDlg(CMediaGalleryApp* pMe,
                                 const char * pszResFile,
                                 uint16 nTextResId,
                                 PromptMsg_e_Type eMsgBoxType,
                                 BottomBar_e_Type eMsgBoxBottomBar);
static __inline NextFSMAction MGState_ExitMediaMenuDialog(
                                                   CMediaGalleryApp* pMe);
static __inline NextFSMAction MGState_ExitOperateDialog(
                                                   CMediaGalleryApp* pMe);
static boolean MGState_StartFileExplorer(CMediaGalleryApp* pMe,
                                          uint8 nStoreMedium,
                                          const char *cpszPath,
                                          uint16 eMimeType);


/*===========================================================================
 *                      Public Function Definitions
 *===========================================================================
 */
/*===========================================================================
 * FUNCTION: MediaGalleryApp_ProcessState
 *
 * DESCRIPTION:
 *    According to the internal state variable, this function routes the call
 *    to the state handler.
 *
 * PARAMETERS:
 *    *pMe[in]: Pointer to the Media gallery Applet structure. This structure
 *          contains information specific to this applet.
 *
 * RETURN VALUE:
 *    NFSMACTION_CONTINUE: To indicate that there are subsequent states to be
 *          followed and hence the state machine should not be stopped.
 *    NFSMACTION_WAIT: To indicate that the state machine can be suspended as
 *          we are going to show a dialog to the user.
 *===========================================================================
 */
NextFSMAction MediaGalleryApp_ProcessState(CMediaGalleryApp* pMe)
{
   NextFSMAction retVal = NFSMACTION_WAIT;

   if(pMe)
   {
      MG_FSMSTATE_HANDLER fcnPtr = NULL;

      switch(pMe->m_eCurState)
      {
         case STATE_NONE:
            fcnPtr = MGStateNoneOperationHandler;
            break;

         case STATE_INIT:
            fcnPtr = MGStateInitHandler;
            break;
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
		case STATE_PWD:
			fcnPtr = MGStatePwdHandler;
			break;
#endif
         case STATE_MAINMENU:
            fcnPtr = MGStateMainMenuHandler;
            break;

         case STATE_PHONEMEM:
            fcnPtr = MGStatePhoneMemHandler;
            break;

         case STATE_CARDMEM:
            fcnPtr = MGStateCardMemHandler;
            break;

         case STATE_MEMSTAT:
            fcnPtr = MGStateMemStatHandler;
            break;

         case STATE_UDISK:
            fcnPtr = MGStateUDiskStateHandler;
            break;

         case STATE_MEDIAMENU:
            fcnPtr = MGStateMediaMenuHandler;
            break;

         case STATE_EXIT:
            fcnPtr = MGStateExitHandler;
            break;

/*          case STATE_PLAY:
 *          break;
 */
         case STATE_VIEW:
            fcnPtr = MGStateImageViewerHandler;
            break;

         case STATE_SETAS:
            fcnPtr = MGStateSetAsHandler;
            break;

         case STATE_OPTION:
            fcnPtr = MGStateOptionHandler;
            break;
            
		 case STATE_SAVETOPLAYLIST:
		 	fcnPtr = MGStateSavetoplaylistHandler;
		 	break;
         case STATE_RENAME:
            fcnPtr = MGStateRenameHandler;
            break;
/*
 *          case STATE_DELETE:
 *             break;
 *          case STATE_DELALL:
 *             break;
 *          case STATE_BLUETOOTH:
 *             break;
 *          case STATE_COPY:
 *             break;
 *          case STATE_MOVE:
 *             fcnPtr = MGStateMoveHandler;
 *             break;
 */
         case STATE_SELECTPATH:
            fcnPtr = MGStateSelectPathHandler;
            break;

         case STATE_DETAIL:
            fcnPtr = MGStateDetailHandler;
            break;

         case STATE_SORT:
            fcnPtr = MGStateSortHandler;
            break;

         case STATE_MUSIC_ADD:
            fcnPtr = MGStateMusicAddHandler;
            break;

         case STATE_VIDEO_ADD:
            fcnPtr = MGStateVideoAddHandler;
            break;

         case STATE_IMAGE_SETTING:
            fcnPtr = MGStateImageSettingHandler;
            break;

         case STATE_MUSIC_SETTING:
            fcnPtr = MGStateSoundSettingHandler;
            break;

         default:
            fcnPtr = MGStateNoneOperationHandler;
            break;
      }

      retVal = fcnPtr(pMe);
   }

   return retVal;
}

/*===========================================================================
 *    Function Definitions(used only in MediaGalleryAppStateHandler.c file)
 *===========================================================================
 */

/*==========================================================================
 * FUNCTION: MGStateNoneOperationHandler
 *
 * DESCRIPTION:
 *   Currently called in response to the STATE_NONE state being set. This
 *   state handler just return and exit.
 *
 * PARAMETERS:
 *     *pMe[in]: Pointer to the Media gallery Applet structure. This structure
 *          contains information specific to this applet.
 *==========================================================================
 */
static NextFSMAction MGStateNoneOperationHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   MG_FARF(STATE, ("STATE_NONE"));
   MGMOVE_TO_STATE(pMe,STATE_EXIT);

   return NFSMACTION_CONTINUE;
}//MGStateNoneOperationHandler

/*==========================================================================
 * FUNCTION: MGStateInitHandler
 *
 * DESCRIPTION:
 *   Currently called in response to the STATE_INIT state being set. This
 *   state handler initialize the user interface.
 *
 * PARAMETERS:
 *     *pMe[in]: Pointer to the Media gallery Applet structure. This structure
 *          contains information specific to this applet.
 *==========================================================================
 */
static NextFSMAction MGStateInitHandler(CMediaGalleryApp* pMe)
{
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)

	boolean locksel;

	if(!pMe)
	{
      return NFSMACTION_WAIT;
	}
		
	OEM_GetConfig( CFGI_MEDIAGALLERY_LOCK_CHECK, &locksel, sizeof( locksel));

	if(locksel)
	{
		MGMOVE_TO_STATE(pMe, STATE_PWD);
	}
	else
	{
		switch(pMe->m_eDlgRet)
		{
			case MGDLGRET_CREATE:
			{
				switch(pMe->m_StartMode)
				{
					case MGSM_SETTING_WALLPAPER:
					case MGSM_SETTING_RINGTONE:
						MGMOVE_TO_STATE(pMe,STATE_SETAS);
						break;

					default:
						MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
						break;
				}
				break;
			}
			default:
		    {
				MGMOVE_TO_STATE(pMe,STATE_EXIT);
		        break;
			}
		}
	}
#else
	if(!pMe)
	{
      return NFSMACTION_WAIT;
	}
	
	switch(pMe->m_eDlgRet)
	{
		case MGDLGRET_CREATE:
		{
			switch(pMe->m_StartMode)
			{
				case MGSM_SETTING_WALLPAPER:
				case MGSM_SETTING_RINGTONE:
					MGMOVE_TO_STATE(pMe,STATE_SETAS);
					break;

				default:
					MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
					break;
			}
			break;
		}
		default:
	    {
			MGMOVE_TO_STATE(pMe,STATE_EXIT);
	        break;
		}
	}
#endif

   return NFSMACTION_CONTINUE;
}//MGStateInitHandler


#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
static NextFSMAction MGStatePwdHandler(CMediaGalleryApp *pMe)
{
	DBGPRINTF("***zzg MGStatePwdHandler m_eDlgRet=%x***", pMe->m_eDlgRet);
	
	switch(pMe->m_eDlgRet)
    {
        case MGDLGRET_CREATE:
            pMe->m_bNotOverwriteDlgRet = FALSE;
            if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_PWD))
            {
                MGMOVE_TO_STATE(pMe, STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
            return NFSMACTION_WAIT;

        case MGDLGRET_CANCELED:
            MGMOVE_TO_STATE(pMe, STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case MGDLGRET_PASS:            
            switch(pMe->m_StartMode)
			{
				case MGSM_SETTING_WALLPAPER:
				case MGSM_SETTING_RINGTONE:
					MGMOVE_TO_STATE(pMe,STATE_SETAS);
					break;

				default:
					MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
					break;
			}
            return NFSMACTION_CONTINUE;

        case MGDLGRET_FAILD:    			
			MediaGalleryApp_ShowMsgBoxDlg(pMe,
							               MGRES_LANGFILE,
							               IDS_INVALID,
							               MESSAGE_INFORMATION,
							               BTBAR_NONE);         
            return NFSMACTION_WAIT;
		
        case MGDLGRET_MSGBOX_OK:
            MGMOVE_TO_STATE(pMe, STATE_PWD);
            return NFSMACTION_CONTINUE;        

        default:
            MGMOVE_TO_STATE(pMe, STATE_EXIT);
            return NFSMACTION_CONTINUE;
    }
}
#endif

/*==========================================================================
 * FUNCTION: MGStateExitHandler
 *
 * DESCRIPTION:
 *   Currently called in response to the STATE_EXIT state being set. This
 *   state handler close applet.
 *
 * PARAMETERS:
 *     *pMe[in]: Pointer to the Media gallery Applet structure. This structure
 *          contains information specific to this applet.
 *==========================================================================
 */
static NextFSMAction MGStateExitHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   MG_FARF(STATE, ("STATE_EXIT"));
   ISHELL_CloseApplet(pMe->m_pShell, FALSE);
   return NFSMACTION_WAIT;
}//MGStateExitHandler

/*==========================================================================
 * FUNCTION: MGStateMainMenuHandler
 *
 * DESCRIPTION:
 *   Currently called in response to the STATE_MAINMENU state being set. This
 *   state handler enter the main menu of the applet.
 *
 * PARAMETERS:
 *     *pMe[in]: Pointer to the Media gallery Applet structure. This structure
 *          contains information specific to this applet.
 *==========================================================================
 */
static NextFSMAction MGStateMainMenuHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
      case MGDLGRET_MSGBOX_OK:
         {
            if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_MAINMENU))
            {
               MGMOVE_TO_STATE(pMe,STATE_EXIT);
               break;
            }
            return NFSMACTION_WAIT;
         }

      case MGDLGRET_CANCELED:
         MGMOVE_TO_STATE(pMe,STATE_EXIT);
        break;

      case MGDLGRET_YES:
        {
           uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

           AEECLSID clsid;

           if(MG_MSGID_MP3PLAYBACKGROUND == nMsgBoxId ||
                 MG_MSGID_FMPLAYBACKGROUND == nMsgBoxId)
           {
              if(MG_MSGID_MP3PLAYBACKGROUND == nMsgBoxId)
                 clsid = AEECLSID_APP_MUSICPLAYER;
              else
                 clsid = AEECLSID_APP_FMRADIO;

              ISHELL_SendEvent(pMe->m_pShell, clsid, EVT_CLOSEAPP, 0, 0);
              if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_MAINMENU))
              {
                 MGMOVE_TO_STATE(pMe,STATE_EXIT);
                 break;
              }
              return NFSMACTION_WAIT;

           }
           break;
        }

      case MGDLGRET_NO:
        {
           uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

           if(MG_MSGID_MP3PLAYBACKGROUND == nMsgBoxId ||
                 MG_MSGID_FMPLAYBACKGROUND == nMsgBoxId)
           {
              MGMOVE_TO_STATE(pMe,STATE_EXIT);
           }
           break;
        }

      case MGDLGRET_NOEXTCARD:
        /*no SD Card insert*/
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               MGRES_LANGFILE,
               IDS_MG_NOEXTCARD,
               MESSAGE_INFORMATION,
               BTBAR_NONE);
         return NFSMACTION_WAIT;

      case MGDLGRET_USBConnect:
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               MGRES_LANGFILE,
               IDS_USB_CONNECTED,
               MESSAGE_INFORMATION,
               BTBAR_NONE);
         return NFSMACTION_WAIT;

      case MGDLGRET_USBNotConnect:
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               MGRES_LANGFILE,
               IDS_USB_NOT_CONNECTED,
               MESSAGE_INFORMATION,
               BTBAR_NONE);
         return NFSMACTION_WAIT;

      case MGDLGRET_BGPLAYPROMPT:
         {
            uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);
            //uint16   nTextResId;
            #ifdef FEATURE_VERSION_K232_Y105A
            MediaGalleryApp_ShowMsgBoxDlg(pMe,
                                          APPSCOMMON_RES_LANG_FILE,
                                          (uint16)(nMsgBoxId == MG_MSGID_FMPLAYBACKGROUND ?
                                                   IDS_CONFIRM_OFF_FM : IDS_EXIT_MUSIC),
                                          MESSAGE_CONFIRM,
                                          BTBAR_OK_CANCEL);
            #else
            MediaGalleryApp_ShowMsgBoxDlg(pMe,
                                          APPSCOMMON_RES_LANG_FILE,
                                          (uint16)(nMsgBoxId == MG_MSGID_FMPLAYBACKGROUND ?
                                                   IDS_CONFIRM_OFF_FM : IDS_CONFIRM_OFF_MP),
                                          MESSAGE_CONFIRM,
                                          BTBAR_OK_CANCEL);
            #endif
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_PHONEMEM:
         switch(pMe->m_StartMode)
         {
         case MGSM_NORMAL_EXPLORER:
            MGMOVE_TO_STATE(pMe,STATE_PHONEMEM);
            break;

         case MGSM_FILE_SELECT:
            {
               uint16 nMimeBase;
               uint16 nFileType;
               const char *cpszPath = NULL;

               nFileType = pMe->m_pExpPara->nFileType;

               nMimeBase = MGMediaInfo_FileType2MimeBase(nFileType);
               MGMediaInfo_GetForderFromMimeBase(nMimeBase,
                                                 MG_STMED_HANDSET,
                                                 &cpszPath);
#if 1
               pMe->m_StoreMedium = MG_STMED_HANDSET;

               MGExplorer_Init(&pMe->m_Explorer,
                               pMe->m_pFileMgr,
                               cpszPath,
                               nMimeBase);
#else
               MGState_StartFileExplorer(pMe,
                                         MG_STMED_HANDSET,
                                         cpszPath,
                                         nMimeBase);
#endif
               MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
            }

            break;

         case MGSM_PHOTO_BROWSE:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_HANDSET,
                                      MG_PHONEPICTURE_PATH,
                                      MG_MIME_IMGBASE);
            MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
            break;

         case MGSM_VIDEO_BROWSE:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_HANDSET,
                                      MG_PHONEVIDEOS_PATH,
                                      MG_MIME_VIDEOBASE);
            MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
            break;

         case MGSM_RECORD_VIEW:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_HANDSET,
                                      MG_PHONECALLMEMO_PATH,
                                      MG_MIME_SNDBASE);
            MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
            break;
			

         case MGSM_MUSIC_ADD:
		 	/*
		 	#ifdef FEATURE_VERSION_C337
			MGState_StartFileExplorer(pMe,
                                      MG_STMED_HANDSET,
                                      MG_MASSCARD_ROOTDIR,
                                      MG_MIME_SNDBASE);
			#else
			*/
			MGState_StartFileExplorer(pMe,
                                      MG_STMED_HANDSET,
                                      MG_PHONEMUSIC_PATH,
                                      MG_MIME_SNDBASE);
			//#endif
            
            MGMOVE_TO_STATE(pMe,STATE_MUSIC_ADD);
            break;

         case MGSM_VIDEO_ADD:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_HANDSET,
                                      MG_PHONEVIDEOS_PATH,
                                      MG_MIME_VIDEOBASE);
            MGMOVE_TO_STATE(pMe,STATE_VIDEO_ADD);
            break;

         case MGSM_IMAGE_SETTING:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_HANDSET,
                                      MG_PHONEPICTURE_PATH,
                                      MG_MIME_IMGBASE);
            MGMOVE_TO_STATE(pMe,STATE_IMAGE_SETTING);
            break;

         case MGSM_MUSIC_SETTING:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_HANDSET,
                                      MG_PHONEMUSIC_PATH,
                                      MG_MIME_SNDBASE);
            MGMOVE_TO_STATE(pMe,STATE_MUSIC_SETTING);
            break;

         default:
            break;
         }
         break;

      case MGDLGRET_CARDMEM:
         switch(pMe->m_StartMode)
         {
         case MGSM_NORMAL_EXPLORER:
#ifdef FEATURE_MG_MASSCARD_EXPLORER
            MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
#else
            MGMOVE_TO_STATE(pMe,STATE_CARDMEM);
#endif
            break;

         case MGSM_FILE_SELECT:
            {
               uint16 nMimeBase;
               uint16 nFileType;
               const char *cpszPath = NULL;

               nFileType = pMe->m_pExpPara->nFileType;

               nMimeBase = MGMediaInfo_FileType2MimeBase(nFileType);
               MGMediaInfo_GetForderFromMimeBase(nMimeBase,
                                                 MG_STMED_MASSCARD,
                                                 &cpszPath);
#if 1
               pMe->m_StoreMedium = MG_STMED_HANDSET;

               MGExplorer_Init(&pMe->m_Explorer,
                               pMe->m_pFileMgr,
                               cpszPath,
                               nMimeBase);
#else
               MGState_StartFileExplorer(pMe,
                                         MG_STMED_MASSCARD,
                                         cpszPath,
                                         nMimeBase);
#endif
               MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
            }
            break;

         case MGSM_PHOTO_BROWSE:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_MASSCARD,
                                      MG_MASSCARDPICTURE_PATH,
                                      MG_MIME_IMGBASE);
            MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
            break;

         case MGSM_VIDEO_BROWSE:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_MASSCARD,
                                      MG_MASSCARDVIDEO_PATH,
                                      MG_MIME_VIDEOBASE);
            MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
            break;

         case MGSM_RECORD_VIEW:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_MASSCARD,
                                      MG_MASSCARDVOICEMEMO_PATH,
                                      MG_MIME_SNDBASE);
            MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
            break;

         case MGSM_MUSIC_ADD:		
		 {
		 	//Add By zzg 2010_08_17
			IImage * pIImage = ISHELL_LoadBusyImage(pMe->m_pShell);		

			if (pIImage)
			{			
				AEEImageInfo ii;
				IIMAGE_GetInfo(pIImage, &ii);
				IIMAGE_SetParm(pIImage, IPARM_ROP, AEE_RO_TRANSPARENT, 0);
				IIMAGE_Draw(pIImage, (pMe->m_rc.dx - ii.cx)/2, (pMe->m_rc.dy - TITLEBAR_HEIGHT - BOTTOMBAR_HEIGHT - ii.cy)/2);
				IIMAGE_Release(pIImage);
				IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
			}
			//Add End
	        
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_MASSCARD,
                                      //MG_MASSCARD_ROOTDIR,
                                      MG_MASSCARDMUSIC_PATH,
                                      MG_MIME_SNDBASE);
            MGMOVE_TO_STATE(pMe,STATE_MUSIC_ADD);
            break;
         }

         case MGSM_VIDEO_ADD:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_MASSCARD,
                                      MG_MASSCARDVIDEO_PATH,
                                      MG_MIME_VIDEOBASE);
            MGMOVE_TO_STATE(pMe,STATE_VIDEO_ADD);
            break;

         case MGSM_IMAGE_SETTING:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_MASSCARD,
                                      MG_MASSCARDPICTURE_PATH,
                                      MG_MIME_IMGBASE);
            MGMOVE_TO_STATE(pMe,STATE_IMAGE_SETTING);
            break;

         case MGSM_MUSIC_SETTING:
            MGState_StartFileExplorer(pMe,
                                      MG_STMED_MASSCARD,
                                      MG_MASSCARDMUSIC_PATH,
                                      MG_MIME_SNDBASE);
            MGMOVE_TO_STATE(pMe,STATE_MUSIC_SETTING);
            break;

         default:

            break;
         }
         break;

      case MGDLGRET_MEMSTAT:
         if(pMe->m_StartMode == MGSM_NORMAL_EXPLORER)
         {
            MGMOVE_TO_STATE(pMe,STATE_MEMSTAT);
         }
         else
         {
            MG_FARF(STATE, ("MGStateMainMenuHandler: ERR status"));
         }
         break;

      case MGDLGRET_UDISK:
         if(pMe->m_StartMode == MGSM_NORMAL_EXPLORER)
         {
            MGMOVE_TO_STATE(pMe,STATE_UDISK);
         }
         else
         {
            MG_FARF(STATE, ("MGStateMainMenuHandler: ERR status"));
         }
         break;

      default:
         return NFSMACTION_WAIT;
   }

   return NFSMACTION_CONTINUE;
}//MGStateMainMenuHandler

/*===========================================================================
 * FUNCTION:MGStatePhoneMemHandler
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *     *pMe[in]: Pointer to the Media gallery Applet structure. This structure
 *          contains information specific to this applet.
 * ==========================================================================
 */
static NextFSMAction MGStatePhoneMemHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
      case MGDLGRET_MSGBOX_OK:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_PHONEMEM))
         {
            MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
            break;
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_MEDIAMENU:
         MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
         break;

      case MGDLGRET_NOFILES:
         MediaGalleryApp_ShowMsgBoxDlg(pMe, MGRES_LANGFILE,IDS_MG_EMPTY,
                                 MESSAGE_INFORMATION,
                                 BTBAR_BACK);
         return NFSMACTION_WAIT;

      //go to main menu, up
      case MGDLGRET_CANCELED:
         MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
         break;

      default:
        return NFSMACTION_WAIT;
   }

   return NFSMACTION_CONTINUE;
}//MGStatePhoneMemHandler

/*===========================================================================
 * FUNCTION:MGStateCardMemHandler
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *     *pMe[in]: Pointer to the Media gallery Applet structure. This structure
 *          contains information specific to this applet.
 * ==========================================================================
 */
static NextFSMAction MGStateCardMemHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
      case MGDLGRET_MSGBOX_OK:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_CARDMEM))
         {
            MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
            break;
         }
         return NFSMACTION_WAIT;

      //go to main menu, up
      case MGDLGRET_CANCELED:
         MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
         break;

      case MGDLGRET_MEDIAMENU:
         MGMOVE_TO_STATE(pMe,STATE_MEDIAMENU);
         break;

      case MGDLGRET_NOFILES:
         MediaGalleryApp_ShowMsgBoxDlg(pMe, MGRES_LANGFILE,IDS_MG_EMPTY,
                                 MESSAGE_INFORMATION,
                                 BTBAR_BACK);
         return NFSMACTION_WAIT;

      default:
#ifdef FEATURE_MG_MASSCARD_EXPLORER
         return MGState_OnPopupMenuRet(pMe);
#else
         return NFSMACTION_WAIT;
#endif
   }

   return NFSMACTION_CONTINUE;
}//MGStateCardMemHandler

/*===========================================================================
 * FUNCTION:MGStateMemStatHandler
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *     *pMe[in]: Pointer to the Media gallery Applet structure. This structure
 *          contains information specific to this applet.
 * ==========================================================================
 */
static NextFSMAction MGStateMemStatHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_MEMSTAT))
         {
            MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
            break;
         }
         return NFSMACTION_WAIT;

      //go to main menu, up
      case MGDLGRET_CANCELED:
         MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
         break;

      default:
         return NFSMACTION_WAIT;
   }

   return NFSMACTION_CONTINUE;
}//MGStateMemStatHandler


/*
 * ==========================================================================
 * FUNCTION     :  MGStateUDiskStateHandler
 * DESCRIPTION  :  U-Disk function operation state handle
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static NextFSMAction MGStateUDiskStateHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_UDISK))
         {
            MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
            break;
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
         MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
         break;

      case MGDLGRET_UDISKSTART:
         MediaGalleryApp_ShowMsgBoxDlg(pMe,APPSCOMMON_RES_LANG_FILE,IDS_POP_DONE,
                                       MESSAGE_INFORMATIVE,
                                       BTBAR_OK);
         return NFSMACTION_WAIT;

      case MGDLGRET_UDISKSTOP:
        #ifdef FEATURE_VERSION_W515V3
        MediaGalleryApp_ShowMsgBoxDlg(pMe,MGRES_LANGFILE,IDS_MG_STOPV3DATACARD,
                                      MESSAGE_INFORMATION,
                                      BTBAR_OK);       
        #elif defined(FEATURE_VERSION_W317A)
        MediaGalleryApp_ShowMsgBoxDlg(pMe,MGRES_LANGFILE,IDS_MG_DISSTORAGE,
                                      MESSAGE_INFORMATION,
                                      BTBAR_OK); 
        #else
        MediaGalleryApp_ShowMsgBoxDlg(pMe,MGRES_LANGFILE,IDS_MG_STOPUDISK,
                                      MESSAGE_INFORMATION,
                                      BTBAR_OK);
        #endif
         
         return NFSMACTION_WAIT;

      case MGDLGRET_NOEXTCARD:
        /*no SD Card insert*/
         MediaGalleryApp_ShowMsgBoxDlg(pMe,MGRES_LANGFILE,IDS_MG_NOEXTCARD,
                                       MESSAGE_INFORMATION,
                                       BTBAR_BACK);
         return NFSMACTION_WAIT;

      case MGDLGRET_USBConnect:
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               MGRES_LANGFILE,
               IDS_USB_CONNECTED,
               MESSAGE_INFORMATION,
               BTBAR_NONE);
         return NFSMACTION_WAIT;

      case MGDLGRET_USBNotConnect:
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               MGRES_LANGFILE,
               IDS_USB_NOT_CONNECTED,
               MESSAGE_INFORMATION,
               BTBAR_NONE);
         return NFSMACTION_WAIT;         

      case MGDLGRET_NOUSBCABLE:
         /*no USB cable connect*/
         MediaGalleryApp_ShowMsgBoxDlg(pMe,MGRES_LANGFILE,IDS_MG_NOUSBCABLE,
                                       MESSAGE_INFORMATION,
                                       BTBAR_BACK);
         return NFSMACTION_WAIT;

      case MGDLGRET_MSGBOX_OK:
         MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
         break;

      default:
         return NFSMACTION_WAIT;
   }

   return NFSMACTION_CONTINUE;
}//MGStateUDiskStateHandler

/*===========================================================================
 * FUNCTION:MGStateMediaMenuHandler
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *     *pMe[in]: Pointer to the Media gallery Applet structure. This structure
 *          contains information specific to this applet.
 * ==========================================================================
 */
static NextFSMAction MGStateMediaMenuHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
      case MGDLGRET_MSGBOX_OK:
      {
         uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

         if(nMsgBoxId == MG_MSGID_NOFILE &&
            pMe->m_eDlgRet == MGDLGRET_MSGBOX_OK)
         {
            return MGState_ExitMediaMenuDialog(pMe);
         }

         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_MEDIAMENU))
         {
            return MGState_ExitMediaMenuDialog(pMe);
         }

         return NFSMACTION_WAIT;
     }

#if defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)|| defined(FEATURE_VERSION_K232_Y100A) || defined (FEATURE_VERSION_KK5)
      case MGDLGRET_BGPLAYPROMPT:
         {
            uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);
            //uint16   nTextResId;
            #ifdef FEATURE_VERSION_K232_Y105A
            MediaGalleryApp_ShowMsgBoxDlg(pMe,
                                          APPSCOMMON_RES_LANG_FILE,
                                          (uint16)(nMsgBoxId == MG_MSGID_FMPLAYBACKGROUND ?
                                                   IDS_CONFIRM_OFF_FM : IDS_EXIT_MUSIC),
                                          MESSAGE_CONFIRM,
                                          BTBAR_OK_CANCEL);
            #else
            MediaGalleryApp_ShowMsgBoxDlg(pMe,
                                          APPSCOMMON_RES_LANG_FILE,
                                          (uint16)(nMsgBoxId == MG_MSGID_FMPLAYBACKGROUND ?
                                                   IDS_CONFIRM_OFF_FM : IDS_CONFIRM_OFF_MP),
                                          MESSAGE_CONFIRM,
                                          BTBAR_OK_CANCEL);
            #endif
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_NO:
        {
           uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

           if(MG_MSGID_MP3PLAYBACKGROUND == nMsgBoxId ||
                 MG_MSGID_FMPLAYBACKGROUND == nMsgBoxId)
           {
              MGMOVE_TO_STATE(pMe,STATE_EXIT);
           }
           break;
        }		

      case MGDLGRET_YES:
        {
           uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

           AEECLSID clsid;

           if(MG_MSGID_MP3PLAYBACKGROUND == nMsgBoxId ||
                 MG_MSGID_FMPLAYBACKGROUND == nMsgBoxId)
           {
              if(MG_MSGID_MP3PLAYBACKGROUND == nMsgBoxId)
                 clsid = AEECLSID_APP_MUSICPLAYER;
              else
                 clsid = AEECLSID_APP_FMRADIO;

              ISHELL_SendEvent(pMe->m_pShell, clsid, EVT_CLOSEAPP, 0, 0);
              if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_MAINMENU))
              {
                 MGMOVE_TO_STATE(pMe,STATE_EXIT);
                 break;
              }
              return NFSMACTION_WAIT;

           }
           break;
        }	  
#endif		

      case MGDLGRET_DIRNOTEMPTY:
         MediaGalleryApp_ShowMsgBoxDlg(pMe, APPSCOMMON_RES_LANG_FILE,
			         IDS_MG_FOLDERNOTEMPTY,
                                 MESSAGE_INFORMATION,
                                 BTBAR_OK_CANCEL);
         return NFSMACTION_WAIT;


      case MGDLGRET_NOFILES:
         MediaGalleryApp_SetMsgBoxID(pMe, MG_MSGID_NOFILE);
         MediaGalleryApp_ShowMsgBoxDlg(pMe, MGRES_LANGFILE,IDS_MG_EMPTY,
               MESSAGE_INFORMATION,
               BTBAR_BACK);

         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
         return MGState_ExitMediaMenuDialog(pMe);

      case MGDLGRET_SETTING_SELECT:
         MGMOVE_TO_STATE(pMe, STATE_EXIT);
         break;

      default:
         return MGState_OnPopupMenuRet(pMe);
   }

   return NFSMACTION_CONTINUE;
}//MGStateMediaMenuHandler


/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */static NextFSMAction MGStateImageViewerHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_IMAGEVIEWER))
         {
            return MGState_ExitOperateDialog(pMe);
         }

         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
          return MGState_ExitOperateDialog(pMe);

      case MGDLGRET_DETAIL:
          MGMOVE_TO_STATE(pMe, STATE_DETAIL);
          break;

      case MGDLGRET_SETTING_SELECT:
         MGMOVE_TO_STATE(pMe, STATE_EXIT);
         break;

      default:
         return NFSMACTION_WAIT;
   }

   return NFSMACTION_CONTINUE;
}//MGStateImageViewerHandler

/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */static NextFSMAction MGStateRenameHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
   {
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_RENAME))
         {
             return MGState_ExitOperateDialog(pMe);
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
          return MGState_ExitOperateDialog(pMe);

      default:
         MGMOVE_TO_STATE(pMe,STATE_EXIT);
         break;
   }

   return NFSMACTION_CONTINUE;
}//MGStateRenameHandler

/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */static NextFSMAction MGStateSetAsHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_SETAS))
         {
             return MGState_ExitOperateDialog(pMe);
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
          return MGState_ExitOperateDialog(pMe);

      default:
         MGMOVE_TO_STATE(pMe,STATE_EXIT);
         break;
   }

   return NFSMACTION_CONTINUE;
}//MGStateSetAsHandler


static NextFSMAction MGStateOptionHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;
   
   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_OPTION))
         {
             return MGState_ExitOperateDialog(pMe);
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
          MGMOVE_TO_STATE(pMe,STATE_MUSIC_ADD);
          break;          
         
      default:
         MGMOVE_TO_STATE(pMe,STATE_EXIT);
         break;
   }

   return NFSMACTION_CONTINUE;
}//MGStateSetAsHandler

/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static NextFSMAction MGStateSavetoplaylistHandler(CMediaGalleryApp* pMe)
{
	if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_SAVETOPLAYLIST))
         {
             return MGState_ExitOperateDialog(pMe);
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
          return MGState_ExitOperateDialog(pMe);

      default:
         MGMOVE_TO_STATE(pMe,STATE_EXIT);
         break;
   }

   return NFSMACTION_CONTINUE;
}//MGStateSavetoplaylistHandler

/*===========================================================================
 * FUNCTION:    MGStateDetailHandler
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */static NextFSMAction MGStateDetailHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_DETAIL))
         {
             return MGState_ExitOperateDialog(pMe);
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
          if(pMe->m_PopupOps == MG_OP_VIEWIMG){
             MGMOVE_TO_STATE(pMe, STATE_VIEW);
             break;
          }
          else
             return MGState_ExitOperateDialog(pMe);

      default:
         MGMOVE_TO_STATE(pMe,STATE_EXIT);
         break;
   }

   return NFSMACTION_CONTINUE;
}//MGStateDetailHandler


/*
 * ==========================================================================
 * FUNCTION     :  MGStateSortHandler
 * DESCRIPTION  :  改变文件/目录排序方式
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static NextFSMAction MGStateSortHandler(CMediaGalleryApp *pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_SORT))
         {
            return MGState_ExitOperateDialog(pMe);
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
         return MGState_ExitOperateDialog(pMe);

      default:
         MGMOVE_TO_STATE(pMe, STATE_EXIT);
         break;
   }

   return NFSMACTION_CONTINUE;
}//MGStateSortHandler


/*
 * ==========================================================================
 * FUNCTION     :  MGStateSelectPathHandler
 * DESCRIPTION  :  选择拷贝/复制的目标路径
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static NextFSMAction MGStateSelectPathHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
      case MGDLGRET_MSGBOX_OK:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_SELECTPATH))
         {
            MGMOVE_TO_STATE(pMe, STATE_MAINMENU);
            break;
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
         return MGState_ExitOperateDialog(pMe);
         break;

      case MGDLGRET_DONE:
         {
            NextFSMAction nNext = MGState_ExitOperateDialog(pMe);

            return nNext;
         }
         break;

      case MGDLGRET_NOEXTCARD:
         /*no SD Card insert*/
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
                                       MGRES_LANGFILE,
                                       IDS_MG_NOEXTCARD,
                                       MESSAGE_INFORMATION,
                                       BTBAR_NONE);
         return NFSMACTION_WAIT;

      case MGDLGRET_USBConnect:
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               MGRES_LANGFILE,
              IDS_USB_CONNECTED,
               MESSAGE_INFORMATION,
               BTBAR_NONE);
         return NFSMACTION_WAIT;

      case MGDLGRET_USBNotConnect:
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               MGRES_LANGFILE,
               IDS_USB_NOT_CONNECTED,
               MESSAGE_INFORMATION,
               BTBAR_NONE);
         return NFSMACTION_WAIT;         

      default:
         return NFSMACTION_WAIT;
   }

   return NFSMACTION_CONTINUE;
}//MGStateSelectPathHandler


/*===========================================================================
 * FUNCTION: MGStateMusicAddHandler
 *
 * DESCRIPTION: 添加音乐文件状态处理
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static NextFSMAction MGStateMusicAddHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
      {
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_MUSICADD))
         {
            MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
            break;
         }
         return NFSMACTION_WAIT;
      }

      case MGDLGRET_MSGBOX_OK:
         MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
         break;

      case MGDLGRET_CANCELED:
         MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
         break;

      case MGDLGRET_OPTION:
         MGMOVE_TO_STATE(pMe,STATE_OPTION);  
         break;

      case MGDLGRET_FILEADD_FINISH:
         MGMOVE_TO_STATE(pMe, STATE_EXIT);
         break;

      default:
         return NFSMACTION_WAIT;
   }

   return NFSMACTION_CONTINUE;
}//MGStateMusicAddHandler


/*
 * ==========================================================================
 * FUNCTION     :  MGStateVideoAddHandler
 * DESCRIPTION  :  添加视频文件到视频播放器列表。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static NextFSMAction MGStateVideoAddHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_VIDEOADD))
         {
            MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
            break;
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_MSGBOX_OK:
         pMe->m_PopupOps = MG_OP_PLAYVIDEO;
         MediaGalleryApp_PlayNext(pMe, MG_PLAY_CUR);
         MGExplorer_FreeMediaMenuItem(pMe->m_pMediaMenu);
         MGMOVE_TO_STATE(pMe, STATE_EXIT);
         break;

#if defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)|| defined(FEATURE_VERSION_K232_Y100A)  || defined (FEATURE_VERSION_KK5)
      case MGDLGRET_BGPLAYPROMPT:
         {
            uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);
            //uint16   nTextResId;
            #ifdef FEATURE_VERSION_K232_Y105A
            MediaGalleryApp_ShowMsgBoxDlg(pMe,
                                          APPSCOMMON_RES_LANG_FILE,
                                          (uint16)(nMsgBoxId == MG_MSGID_FMPLAYBACKGROUND ?
                                                   IDS_CONFIRM_OFF_FM : IDS_EXIT_MUSIC),
                                          MESSAGE_CONFIRM,
                                          BTBAR_OK_CANCEL);
            #else
            MediaGalleryApp_ShowMsgBoxDlg(pMe,
                                          APPSCOMMON_RES_LANG_FILE,
                                          (uint16)(nMsgBoxId == MG_MSGID_FMPLAYBACKGROUND ?
                                                   IDS_CONFIRM_OFF_FM : IDS_CONFIRM_OFF_MP),
                                          MESSAGE_CONFIRM,
                                          BTBAR_OK_CANCEL);
            #endif
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_NO:
        {
           uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

           if(MG_MSGID_MP3PLAYBACKGROUND == nMsgBoxId ||
                 MG_MSGID_FMPLAYBACKGROUND == nMsgBoxId)
           {
              MGMOVE_TO_STATE(pMe,STATE_EXIT);
           }
           break;
        }

      case MGDLGRET_YES:
        {
           uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

           AEECLSID clsid;

           if(MG_MSGID_MP3PLAYBACKGROUND == nMsgBoxId ||
                 MG_MSGID_FMPLAYBACKGROUND == nMsgBoxId)
           {
              if(MG_MSGID_MP3PLAYBACKGROUND == nMsgBoxId)
                 clsid = AEECLSID_APP_MUSICPLAYER;
              else
                 clsid = AEECLSID_APP_FMRADIO;

              ISHELL_SendEvent(pMe->m_pShell, clsid, EVT_CLOSEAPP, 0, 0);
              if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_MAINMENU))
              {
                 MGMOVE_TO_STATE(pMe,STATE_EXIT);
                 break;
              }
              return NFSMACTION_WAIT;

           }
           break;
        }	  
#endif	

      case MGDLGRET_DONE:
      MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_MESSAGEBOX);
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               APPSCOMMON_RES_LANG_FILE,
               IDS_POP_DONE,
               MESSAGE_INFORMATION,
               BTBAR_BACK);
         return NFSMACTION_WAIT;

#if defined(FEATURE_MG_NOHSVIDEO)
      case MGDLGRET_NOEXTCARD:
      MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_MESSAGEBOX);
         /*no SD Card insert*/
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               MGRES_LANGFILE,
               IDS_MG_NOEXTCARD,
               MESSAGE_INFORMATION,
               BTBAR_NONE);
         return NFSMACTION_WAIT;
#endif

      case MGDLGRET_USBConnect:
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               MGRES_LANGFILE,
               IDS_USB_CONNECTED,
               MESSAGE_INFORMATION,
               BTBAR_NONE);
         return NFSMACTION_WAIT;
         
      case MGDLGRET_USBNotConnect:
         MediaGalleryApp_ShowMsgBoxDlg(pMe,
               MGRES_LANGFILE,
               IDS_USB_NOT_CONNECTED,
               MESSAGE_INFORMATION,
               BTBAR_NONE);
         return NFSMACTION_WAIT;         

      case MGDLGRET_NOFILES:
         MediaGalleryApp_ShowMsgBoxDlg(pMe, MGRES_LANGFILE,IDS_MG_EMPTY,
               MESSAGE_INFORMATION,
               BTBAR_BACK);
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
         MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
         break;

      default:
         return  MGState_OnPopupMenuRet(pMe);
   }

   return NFSMACTION_CONTINUE;
}//MGStateVideoAddHandler



/*
 * ==========================================================================
 * FUNCTION     :  MGStateImageSettingHandler
 * DESCRIPTION  :  图片设置
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static NextFSMAction MGStateImageSettingHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_IMAGESETTING))
         {
            MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
            break;
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
         MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
         break;

      case MGDLGRET_SETTING_SELECT:
         MGMOVE_TO_STATE(pMe, STATE_EXIT);
         break;

      default:
         return  MGState_OnPopupMenuRet(pMe);
   }

   return NFSMACTION_CONTINUE;
}//MGStateImageSettingHandler



/*
 * ==========================================================================
 * FUNCTION     :  MGStateSoundSettingHandler
 * DESCRIPTION  :  铃声设置状态处理
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static NextFSMAction MGStateSoundSettingHandler(CMediaGalleryApp* pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_CREATE:
         if(SUCCESS != MediaGalleryApp_ShowDialog(pMe, IDD_MG_MUSICETTING))
         {
            MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
            break;
         }
         return NFSMACTION_WAIT;

      case MGDLGRET_CANCELED:
         MGMOVE_TO_STATE(pMe, STATE_MAINMENU);
         break;

      case MGDLGRET_SETTING_SELECT:
         MGMOVE_TO_STATE(pMe, STATE_EXIT);
         break;

      default:
         MGMOVE_TO_STATE(pMe,STATE_EXIT);
         break;
   }

   return NFSMACTION_CONTINUE;
}//MGStateSoundSettingHandler


/*===========================================================================
 * FUNCTION:      MGState_OnPopupMenuRet
 *
 * DESCRIPTION:   响应弹出菜单，跳转状态。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */static NextFSMAction MGState_OnPopupMenuRet(CMediaGalleryApp* pMe)
{
   if(!pMe)
      return NFSMACTION_WAIT;

   switch(pMe->m_eDlgRet)
   {
      case MGDLGRET_VIEW:
         MGMOVE_TO_STATE(pMe,STATE_VIEW);
         break;

      case MGDLGRET_PLAY:
         MGMOVE_TO_STATE(pMe,STATE_PLAY);
         break;

         /*
          * case MGDLGRET_WALLPAPER:
          * break;
          */

      case MGDLGRET_SETAS:
         MGMOVE_TO_STATE(pMe,STATE_SETAS);
         break;
	  case MGDLGRET_SAVETOPLAYLIST:
	     MGMOVE_TO_STATE(pMe,STATE_SAVETOPLAYLIST);
	  	 break;
      case MGDLGRET_RENAME:
         MGMOVE_TO_STATE(pMe,STATE_RENAME);
         break;

         /*
          * case MGDLGRET_DELETE:
          * case MGDLGRET_DELALL:
          * case MGDLGRET_BLUETOOTH:
          * case MGDLGRET_COPY:
          * case MGDLGRET_MOVE:
          * break;
          * */

      case MGDLGRET_SELECTPATH:
         MGMOVE_TO_STATE(pMe, STATE_SELECTPATH);
         break;

      case MGDLGRET_SORT:
         MGMOVE_TO_STATE(pMe, STATE_SORT);
         break;

      case MGDLGRET_DETAIL:
         MGMOVE_TO_STATE(pMe,STATE_DETAIL);
         break;

      default:
         MG_FARF(STATE, ("Default state handler, move to preceed state!"));
         return MGState_ExitOperateDialog(pMe);
   }

   return NFSMACTION_CONTINUE;//NFSMACTION_WAIT, just like Message box?
}//MGState_OnPopupMenuRet



/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static boolean MGState_StartFileExplorer(CMediaGalleryApp* pMe,
                                         uint8 nStoreMedium,
                                         const char *cpszPath,
                                         uint16 eMimeType)
{
   if(!pMe || !pMe->m_pFileMgr || !pMe->m_pFolderList)
   {
      MG_FARF(STATE, ("MGState_ExitMediaMenuDialog BAD PARAMETER"));
      return FALSE;
   }

   pMe->m_StoreMedium = nStoreMedium;

   MGExplorer_Init(&pMe->m_Explorer, pMe->m_pFileMgr, cpszPath, eMimeType);

   if(SUCCESS != MGExplorer_EnumFoldersList(pMe->m_pFileMgr,
                                            cpszPath,
                                            pMe->m_pFolderList,
                                            TRUE))
   {
      MG_FARF(STATE, ("Enum files failed!"));
   }

   return TRUE;
}//MGState_StartFileExplorer

/*===========================================================================
 * FUNCTION: MGState_ExitMediaMenuDialog
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static __inline NextFSMAction MGState_ExitMediaMenuDialog(
                                                   CMediaGalleryApp* pMe)
{
   MGStartMode       eStartMode;
   if(!pMe)
   {
      MG_FARF(STATE, ("MGState_ExitMediaMenuDialog BAD PARAMETER"));
      return NFSMACTION_WAIT;
   }

   eStartMode = pMe->m_StartMode;

   if(eStartMode == MGSM_NORMAL_EXPLORER)
   {
      if(pMe->m_StoreMedium == MG_STMED_HANDSET)
      {
         MGMOVE_TO_STATE(pMe,STATE_PHONEMEM);
      }
      else if(pMe->m_StoreMedium == MG_STMED_MASSCARD)
      {
#ifdef FEATURE_MG_MASSCARD_EXPLORER
         MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
#else
         MGMOVE_TO_STATE(pMe,STATE_CARDMEM);
#endif
      }
   }
   else
   {
      MGMOVE_TO_STATE(pMe,STATE_MAINMENU);
      MG_FARF(STATE, ("back to main menu!start mode %d",eStartMode));
   }

   return NFSMACTION_CONTINUE;
}//MGState_ExitMediaMenuDialog


/*===========================================================================
 * FUNCTION: MGState_ExitOperateDialog
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static __inline NextFSMAction MGState_ExitOperateDialog(
                                                   CMediaGalleryApp* pMe)

{
   MGAppState eToState;

   if(!pMe)
   {
      MG_FARF(STATE, ("MGState_ExitOperateDialog BAD PARAMETER"));
      return NFSMACTION_WAIT;
   }

   switch(pMe->m_StartMode ){
      case MGSM_PHOTO_BROWSE:
      case MGSM_VIDEO_BROWSE:
      case MGSM_RECORD_VIEW:
      case MGSM_NORMAL_EXPLORER:
         eToState = STATE_MEDIAMENU;
         break;

      case MGSM_VIDEO_ADD:
         eToState = STATE_VIDEO_ADD;
         break;

      case MGSM_MUSIC_ADD:
         eToState = STATE_MUSIC_ADD;
         break;

      case MGSM_IMAGE_SETTING:
         eToState = STATE_IMAGE_SETTING;
         break;

      case MGSM_MUSIC_SETTING:
         eToState = STATE_MUSIC_SETTING;
         break;

      default:
         eToState = pMe->m_ePreState;
         break;
   }

   MGMOVE_TO_STATE(pMe,eToState);

   MG_FARF(STATE, ("ExitOperateDialog prestate:%d, tostate:%d, curstate",
            pMe->m_ePreState,eToState,pMe->m_eCurState));

   return NFSMACTION_CONTINUE;
} //MGState_ExitOperateDialog

/*===========================================================================
 * FUNCTION:MediaGalleryApp_ShowMsgBoxDlg
 * DESCRIPTION:
 * This function is used by various state handlers to bring up a dialog with
 * just a static control in it. The text (resource id) to be put in the static
 * control is also passed as a parameter.
 *
 * RETURN VALUE:None.
 *===========================================================================
 */
static void MediaGalleryApp_ShowMsgBoxDlg(CMediaGalleryApp* pMe,
                                 const char * pszResFile,
                                 uint16 nTextResId,
                                 PromptMsg_e_Type eMsgBoxType,
                                 BottomBar_e_Type eMsgBoxBottomBar)
{
   if(pMe)
   {
      pMe->m_nMsgBoxTextId = nTextResId;
      pMe->m_eMsgBoxType = eMsgBoxType;
      pMe->m_eMsgBoxBottomBar = eMsgBoxBottomBar;
      pMe->m_pszMsgBoxResFile = pszResFile;

      /*create message box dialog*/
      MediaGalleryApp_ShowDialog(pMe, IDD_MG_MSGBOX);
   }
}//MediaGalleryApp_ShowMsgBoxDlg
/* ===================== End of File ===================================== */

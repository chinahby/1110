/** ----------------------------------------------------------------------------
 *
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008-11-3    chunfeng.hou   create
 *
 * -----------------------------------------------------------------------------
 */

#include "recorder_priv.h"
#include "aee_oem.h"
#include "err.h"
static boolean g_m_recorder_is_on = FALSE;
#ifdef FEATURE_APP_RECORDER
boolean g_runningflag = FALSE;
#endif
static void Recorder_RunFSM( Recorder* pme);

static int        Recorder_CreateInstance( AEECLSID ClsId,IShell * pIShell,IModule* po,void** ppObj);
static boolean    Recorder_HandleEvent( Recorder* pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean    Recorder_Init( Recorder* pme);
static void       Recorder_Free( Recorder* pme);


int Recorder_Load( IShell* ps, void* pHelpers, IModule** pMod)
{
	return ( AEEStaticMod_New( (int16)( sizeof(AEEMod)), ps, pHelpers, pMod, (PFNMODCREATEINST)Recorder_CreateInstance, (PFNFREEMODDATA)NULL));
}

static int Recorder_CreateInstance( AEECLSID ClsId, IShell* pIShell, IModule* po, void** ppObj)
{

	Recorder* pme = NULL;

	*ppObj = NULL;

	if( ClsId != AEECLSID_RECORDER)
	{
		return ECLASSNOTSUPPORT;
	}
	ERR("Recorder_CreateInstance:000000000000000000000",0,0,0);
	if( !AEEApplet_New( sizeof(Recorder), ClsId, pIShell, po, (IApplet**)ppObj, (AEEHANDLER)Recorder_HandleEvent, (PFNFREEAPPDATA)Recorder_Free))
	{
		return ENOMEMORY;
	}

	pme 				= (Recorder*)*ppObj;
	pme->m_pShell		= pme->a.m_pIShell;
	pme->m_pDisplay		= pme->a.m_pIDisplay;
	ERR("Recorder_CreateInstance:111111111111111111111111111",0,0,0);
	if( !Recorder_Init( (Recorder*)( *ppObj)))
	{
		Recorder_Free( pme);
		return EFAILED;
	}
	ERR("Recorder_CreateInstance:2222222222222222222222222",0,0,0);
	return AEE_SUCCESS;
}

static boolean Recorder_Init( Recorder* pme)
{

	if( !recorder_init_media( &pme->m_Media))
	{
		return FALSE;
	}
    if (AEE_SUCCESS != ISHELL_CreateInstance(pme->m_pShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pme->m_pIAnn))
    {
        return EFAILED;
    }
#ifdef FEATURE_APP_RECORDER
    g_runningflag = TRUE;
#endif
	if( ISHELL_CreateInstance( pme->m_pShell, AEECLSID_SOUND, (void**)&pme->m_pSound) != SUCCESS)
    {
        debug( ";Failed to create ringermgr");
        return FALSE;
    }

	{
		uint16 	resId[] = { IDI_RECORDER_BG_RECORD,
							IDI_RECORDER_BG_PLAY,
							IDI_RECORDER_PROGRESSBAR1,
							IDI_RECORDER_PROGRESSBAR2,
							IDI_RECORDER_VOLUME0,
							IDI_RECORDER_VOLUME1,
							IDI_RECORDER_VOLUME2,
							IDI_RECORDER_VOLUME3,
							IDI_RECORDER_VOLUME4,
							IDI_RECORDER_VOLUME5
						};
		int 	i 		= 0;
		int		number	= ARRAY_SIZE( resId);
		char*	resFile = AEE_RECORDER_IMAGES_RES_FILE;

		for( i = 0; i < number; i ++)
		{
			if( !( pme->m_pImage[i] = ISHELL_LoadResImage( pme->m_pShell, resFile, resId[i])))
			{
				return FALSE;
			}
		}

	}

	{
		Theme_Param_type themeParms  = {0};
		Appscom_GetThemeParameters( &themeParms);
		#if 0//def FEATURE_VERSION_K212
		pme->m_ThemeTextColor = RGB_BLACK;
		#else
		pme->m_ThemeTextColor = themeParms.textColor;
		#endif
	}
	pme->m_pFileManager = pme->m_Media.m_pFileManager;
	pme->m_bLockkey		= FALSE;
	
	return TRUE;
}

static void Recorder_Free( Recorder* pme)
{

	recorder_release_media_if( &pme->m_Media);
	recorder_release_filemgr_if( &pme->m_Media);
#ifdef FEATURE_APP_RECORDER

    g_runningflag = FALSE;
#endif
	if( pme->m_pSound)
	{
		ISOUND_Release( pme->m_pSound);
		pme->m_pSound = NULL;
	}

	{
		int i 		= 0;
		int number 	= ARRAY_SIZE( pme->m_pImage);

		for( i = 0; i < number; i ++)
		{
			if( pme->m_pImage[i])
			{
				IIMAGE_Release( pme->m_pImage[i]);
				pme->m_pImage[i] = NULL;
			}
		}
	}
	if (pme->m_pIAnn)
    {
        IANNUNCIATOR_Release(pme->m_pIAnn);
		pme->m_pIAnn = NULL;
    }
}

static void Recorder_APPIsReadyTimer( void* pi)
{
	Recorder* pme = (Recorder*)pi;

	if (NULL == pme)
	{
		return;
	}

	postEvent( EVT_APPISREADY);
}

static boolean Recorder_HandleEvent( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam)
{

	AEEAppStart* pAppStart = 0;
	static byte keyBeepVolumeSetting = 0;
	static byte alertTypeCall			= 0;
	static byte alertTypeSms			= 0;
	static byte alertTypeVibrate		= OEMNV_SMS_VIBONLY;

	MSG_FATAL("***zzg Recorder_HandleEvent evt=%x, wParam=%x, dwParam=%x***", evt, wParam, dwParam);
	
	switch( evt)
	{	   
		case EVT_APP_START:
		{
            AEEDeviceInfo di = {0};
			byte mute = OEMSOUND_MUTE_VOL;
			AECHAR wszTitle[16] = {0};
			OEM_GetConfig( CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));			
			OEM_GetConfig( CFGI_ALERT_TYPE, &alertTypeCall, sizeof( alertTypeCall));
			OEM_GetConfig( CFGI_SMS_RINGER, &alertTypeSms, sizeof( alertTypeSms));

			MSG_FATAL("***zzg Recorder_HandleEvent EVT_APP_START alertTypeCall=%x***", alertTypeCall, 0, 0);
			
			OEM_SetConfig( CFGI_BEEP_VOL, &mute, sizeof(byte));
            OEM_SetConfig( CFGI_ALERT_TYPE, &alertTypeVibrate, sizeof( alertTypeVibrate));
            OEM_SetConfig( CFGI_SMS_RINGER, &alertTypeVibrate, sizeof( alertTypeVibrate));

			pme->m_bAppIsReady 			= FALSE;
			pme->m_bInactive 			= FALSE;
			pme->m_bSuspended			= FALSE;
			pme->m_bSelect              = FALSE;

			pAppStart = (AEEAppStart*)dwParam;
			pme->m_rc = pAppStart->rc;
            ISHELL_GetDeviceInfo( pme->m_pShell, &di);
			pme->m_rc.dy = di.cyScreen;
            IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);

			#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)|| defined(FEATURE_VERSION_K232_Y100A)|| defined(FEATURE_VERSION_KK5)
			ISHELL_LoadResString( pme->a.m_pIShell,
                                      AEE_RECORDER_RES_FILE,
                                      IDS_TITLE_C337,
                                      wszTitle,
                                      sizeof(wszTitle));
			#elif defined FEATURE_VERSION_W317A
			ISHELL_LoadResString( pme->a.m_pIShell,
                                      AEE_RECORDER_RES_FILE,
                                      IDS_RECORDER,
                                      wszTitle,
                                      sizeof(wszTitle));
			#else
            ISHELL_LoadResString( pme->a.m_pIShell,
                                      AEE_RECORDER_RES_FILE,
                                      IDS_TITLE,
                                      wszTitle,
                                      sizeof(wszTitle));
			#endif
			
		    IANNUNCIATOR_SetFieldTextEx(pme->m_pIAnn,wszTitle,FALSE);
			Recorder_RunFSM( pme);
		}
		return TRUE;
        
		case EVT_APP_STOP:
		{            
			pme->m_bInactive = TRUE;
			ISHELL_CancelTimer( pme->m_pShell, 0, pme);	

			MSG_FATAL("***zzg Recorder_HandleEvent EVT_APP_STOP alertTypeCall=%x***", alertTypeCall, 0, 0);

			OEM_SetConfig( CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
            OEM_SetConfig( CFGI_ALERT_TYPE, &alertTypeCall, sizeof( alertTypeCall));
            OEM_SetConfig( CFGI_SMS_RINGER, &alertTypeSms, sizeof( alertTypeSms));
		}
		return TRUE;

       
		case EVT_APP_SUSPEND:
		{
			pme->m_bInactive 	= TRUE;
			pme->m_bSuspended	= TRUE;
			
			//Add By zzg 2012_03_13
			OEM_SetConfig( CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
            OEM_SetConfig( CFGI_ALERT_TYPE, &alertTypeCall, sizeof( alertTypeCall));
            OEM_SetConfig( CFGI_SMS_RINGER, &alertTypeSms, sizeof( alertTypeSms));
			//Add End						
			
			ISHELL_CancelTimer( pme->m_pShell, 0, pme);
		}
		return TRUE;
        
        
		case EVT_APP_RESUME:
		{
			//Add By zzg 2012_03_13
			byte mute = OEMSOUND_MUTE_VOL;
			
			OEM_SetConfig( CFGI_BEEP_VOL, &mute, sizeof(byte));
            OEM_SetConfig( CFGI_ALERT_TYPE, &alertTypeVibrate, sizeof( alertTypeVibrate));
            OEM_SetConfig( CFGI_SMS_RINGER, &alertTypeVibrate, sizeof( alertTypeVibrate));
			//Add End
			
			Recorder_RunFSM( pme);
		}
       
		return TRUE;

		case EVT_DIALOG_INIT:
		{
			if( wParam == OEM_IME_DIALOG)
			{
				return TRUE;
			}

			pme->m_bAppIsReady = FALSE;
			pme->m_pActiveDialog = (IDialog*)dwParam;
			pme->m_nActiveDialogId = wParam;
			return Recorder_RouteDialogEvent( pme, evt, wParam, dwParam);
		}

		case EVT_DIALOG_START:
		{
			if( wParam == OEM_IME_DIALOG)
			{
				return TRUE;
			}            
			return Recorder_RouteDialogEvent( pme, evt, wParam, dwParam);
		}

		case EVT_DIALOG_END:
		{
			if( wParam == OEM_IME_DIALOG)
			{
				repaint( TRUE);
				return TRUE;
			}

			if( wParam == 0)
			{
				return TRUE;
			}

			pme->m_bAppIsReady = FALSE;
			(void)Recorder_RouteDialogEvent( pme, evt, wParam, dwParam);

			if( !pme->m_bInactive)
			{
				Recorder_RunFSM( pme);
			}
		}
		return TRUE;

		case EVT_USER_REDRAW:
		{
			Recorder_RouteDialogEvent( pme, evt, wParam, dwParam);
			ISHELL_SetTimer( pme->m_pShell, APPISREADY_TIMER, Recorder_APPIsReadyTimer, pme);
		}
		return TRUE;

		case EVT_APPISREADY:
		{
			Recorder_RouteDialogEvent( pme, evt, wParam, dwParam);
			pme->m_bAppIsReady 	= TRUE;
			pme->m_bInactive 	= FALSE;
			pme->m_bSuspended	= FALSE;
		}
		return TRUE;

		case EVT_KEY:
		case EVT_COMMAND:
		{  
			#if !defined(FEATURE_VERSION_K212) && !defined(FEATURE_VERSION_EC99)&&!defined(FEATURE_QVGA_INHERIT_K212)&&!defined(FEATURE_LOW_MEM_BIGFONT)&&!defined(FEATURE_VERSION_IN50A)&& !defined(FEATURE_VERSION_KK5)
			if( !pme->m_bAppIsReady || pme->m_bLockkey)
			{
				return TRUE;
			}
			#else
			if(pme->m_bLockkey)
			{
				return TRUE;
			}
			#endif
			return Recorder_RouteDialogEvent( pme, evt, wParam, dwParam);
		}
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_USER:
		{
			if (wParam == AVK_CLR)
			{
				evt = EVT_KEY;
			}
			else if ((wParam == AVK_SELECT)||(wParam == AVK_INFO))
			{
				if (dwParam == 0)
				{
					MSG_FATAL("AVK_SELECT.....AVK_INFO",0,0,0);
					evt = EVT_KEY;
				}
				else
				{
					evt = EVT_COMMAND;
				}
			}
			return Recorder_RouteDialogEvent(pme,evt,wParam,dwParam);
		}
		return TRUE;
			
#endif
		default:
		{
#if defined( FEATURE_RECODER_TEST)
			if( evt == EVT_KEY_PRESS && wParam == AVK_STAR)
			{
				ISHELL_StartApplet( pme->m_pShell, 0x0100610f);
				return TRUE;
			}
#endif
			return Recorder_RouteDialogEvent( pme, evt, wParam, dwParam);
		}
	}
}

static void Recorder_RunFSM( Recorder* pme)
{
    NextFSMActionEnum nextFSMAction = NFSMACTION_WAIT;

    for ( ; ; )
    {
        nextFSMAction = Recorder_ProcessState( pme);

        if( pme->m_bNotOverwriteDlgRet)
        {
            pme->m_bNotOverwriteDlgRet = FALSE;
        }
        else
        {
            pme->m_eDlgRet = DLGRET_CREATE;
        }

        if( nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

boolean recorder_init_media( Media* pme)
{


	pme->m_eState				= MEDIA_STATE_INIT;
	pme->m_nLastOperationError	= -1;
	pme->m_nDuration			= 0;
	pme->m_nTick				= 1000;
	pme->m_nElapsedTime			= 0;
	pme->m_nTimeLimit			= RECORDER_TIME_LIMIT_NO_LIMIT;
	pme->m_nVolume				= AEE_MAX_VOLUME / 2;
	pme->m_bMediaError			= FALSE;
	
	//pme->m_pszSaveDir			= RECORDER_MEMO_SAVE_DIR_INTERNAL;
	pme->m_pszSaveDir			= RECORDER_MEMO_SAVE_DIR_CARD0;		//Add By zzg 2012_03_15
	
	pme->m_pszFileExtension		= RECORDER_MEMO_FILE_EXTENSION_QCP;
	pme->m_boardingAppletClsId  = ISHELL_ActiveApplet( AEE_GetShell());

	return recorder_create_filemgr( pme);
}

int32 recorder_set_media_device( Media* pme, int32 device)
{
	int result = 0;

    if( pme->m_pMedia)
    {
    	if( ( result = IMEDIA_SetMediaParm( pme->m_pMedia, MM_PARM_AUDIO_DEVICE, device, 0)) != SUCCESS &&
    			 result != MM_PENDING
    	)
    	{
    		debug( ";recorder_set_media_device failed, 0x%x", result);
    	}
    }
	return result;
}
static void recorder_set_media_path( Media* pme, boolean reverse)
{

	char* name[] = { "forward recorder", "reverse recorder"};

	pme->m_bReverse = reverse;
	pme->m_pszName  = name[reverse];
}

void recorder_set_media_path_to_forward( Media* pme)
{

	recorder_set_media_path( pme, 0);
}

void recorder_set_media_path_to_reverse( Media* pme)
{

	recorder_set_media_path( pme, 1);
}

void recorder_set_media_event_notify( Media* pme, PFNMEDIAEVENTNOTIFY pEventNotify, void* pUser)
{
	pme->m_pEventNotify = pEventNotify;
	pme->m_pUser		= pUser;
}

void recorder_set_media_time_limit( Media* pme, uint32 limit)
{
	pme->m_nTimeLimit = limit;
}

boolean recorder_create_media_if( Media* pme)
{
	if( pme->m_pMedia)
	{
		return TRUE;
	}

	if( ISHELL_CreateInstance( AEE_GetShell(), AEECLSID_MEDIAQCP, (void**)&pme->m_pMedia) != SUCCESS)
	{
		return FALSE;
	}

	recorder_init_media( pme);
	MSG_FATAL( ";recorder_create_media_if, [%s] ok", pme->m_pszName,0,0);
	g_m_recorder_is_on = TRUE;
	return TRUE;
}

void recorder_release_media_if( Media* pme)
{
	if( pme->m_pMedia)
	{		
		debug( ";recorder_release_media_if, [%s] ok", pme->m_pszName);
		g_m_recorder_is_on = FALSE;
		IMEDIA_Release( pme->m_pMedia);
		pme->m_pMedia = NULL;
	}
}
boolean get_recorder_play_state(void)
{
    //Note:if g_m_recorder_is_on = 0 ,Recorder not use or in background;
    return g_m_recorder_is_on;
}
boolean recorder_create_filemgr( Media* pme)
{

	if( !pme->m_pFileManager && ISHELL_CreateInstance( AEE_GetShell(), AEECLSID_FILEMGR, (void**)&pme->m_pFileManager) != SUCCESS)
	{
		return FALSE;
	}

	debug( ";recorder_create_filemgr, ok");
	return TRUE;
}

void recorder_release_filemgr_if( Media* pme)
{
	if( pme->m_pFileManager)
	{
		debug( ";recorder_release_filemgr_if, ok");
		IFILEMGR_Release( pme->m_pFileManager);
		pme->m_pFileManager = NULL;
	}
}

int recorder_set_media_volume( Media* pme, int32 volume)
{

	if( volume < 0)
	{
		volume = 0;
	}
	else if( volume > AEE_MAX_VOLUME)
	{
		volume = AEE_MAX_VOLUME;
	}

	pme->m_nVolume = volume;
	if( pme->m_pMedia)
	{
		return IMEDIA_SetVolume( pme->m_pMedia, volume);
	}
	return SUCCESS;
}

void recorder_set_save_dir( Media* pme, char* dir)
{

	pme->m_pszSaveDir = dir;
}

int recorder_get_memo_number( Media* pme)
{

	int number = 0;

	debug( ";----------------------------");
	debug( ";recorder_get_memo_number, 0x%x", pme->m_pFileManager);
	debug( ";to enum %s", pme->m_pszSaveDir);
	if( pme->m_pFileManager && IFILEMGR_EnumInit( pme->m_pFileManager, pme->m_pszSaveDir, 0) == SUCCESS)
	{
		FileInfo info = {0};

		while( IFILEMGR_EnumNext( pme->m_pFileManager, &info))
		{

			if( STRENDS( pme->m_pszFileExtension, info.szName))
			{
				number ++;
			}
			debug( ";%d, %s", number, info.szName);
		}
	}

	return number;
}

int  recorder_get_error_1( int system_error)
{

	switch( system_error)
	{
		case EBADSTATE:
			return MEDIA_ERROR_BAD_STATE;
		case EBADPARM:
			return MEDIA_ERROR_BAD_PARAM;
		case EUNSUPPORTED:
			return MEDIA_ERROR_UNSPPORTED;
		case EITEMBUSY:
			return MEDIA_ERROR_ITEM_BUSY;
		case MM_EBADMEDIADATA:
			return MEDIA_ERROR_BAD_MEDIA_DATA;
		case ENOMEMORY:
			return MEDIA_ERROR_SPACE_ERROR;
		default:
			return MEDIA_ERROR_UNKNOWN;
	}
}

int  recorder_get_error_2( int system_error)
{

	switch( system_error)
	{
		case MM_STATUS_ABORT:
			return MEDIA_ERROR_ABORTED;
		case MM_STATUS_SPACE_WARNING:
			return MEDIA_ERROR_SPACE_WARNING;
		case MM_STATUS_SPACE_ERROR:
			return MEDIA_ERROR_SPACE_ERROR;
		default:
			return MEDIA_ERROR_UNKNOWN;
	}
}

void recorder_process_media_operation_result( Media* pme, int result, RecorderMediaStateEnum shallState)
{

	if( result == SUCCESS)
	{
		pme->m_eState = shallState;
	}
	else
	{
		if( !( pme->m_nLastOperationError >= FILE_ERROR && pme->m_nLastOperationError <= ENOMEDIA))
		{
			pme->m_nLastOperationError = recorder_get_error_1( result);
		}

		pme->m_bMediaError = TRUE;
	}
	repaint( TRUE);
}
#ifdef FEATURE_APP_RECORDER

boolean recorder_GetRunningFlag(void)
{
    return g_runningflag;
}
#endif

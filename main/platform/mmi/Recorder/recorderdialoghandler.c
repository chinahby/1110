/** ----------------------------------------------------------------------------
 *
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008-11-3      chunfeng.hou        create
 *
 * -----------------------------------------------------------------------------
 */


#include "recorder_priv.h"
#include "aeemediaformats.h"
#include "appscommonimages.brh"
#if defined( FEATURE_RECORDER_SUPPORT_CHINESE_FILE_NAME)
#if defined( FEATURE_LANG_CHINESE)
#include "gbk2unicode.h"
#elif defined( FEATURE_LANG_TCHINESE)
#include "big52unicode.h"
#endif
#endif
#include "Appscommon.h"
#include "appscommon.brh"
#include "recorder_images.brh"

#if !defined( AEE_SIMULATOR)
#include "cmx.h"
#include "oemnvint.h"
#include "oemcfgi.h"
#if defined( FEATURE_RECORDER_PLAY_PAUSE)
#include "db.h"
#endif
#endif

#if defined( FEATURE_RECORDER_RENAME)
#include "aeetext.h"
#endif

#ifdef FEATURE_LCD_TOUCH_ENABLE 
#include "AEEPointerHelpers.h"
#endif

#include "fmradiols.brh"
#define FMRADIO_RES_FILE ("fs:/mod/fmradio/" AEE_RES_LANGDIR "fmradiols.bar")

#include "AEECM.h"
#include "AEEBatt.h"
Recorder* pRecorder;
//-------------------------------------------------------------------------------------------------
static boolean  dialog_handler_of_state_main( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam);
static boolean  dialog_handler_of_state_record( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam);
static boolean  dialog_handler_of_state_record_list( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam);
#if defined( FEATURE_RECORDER_SET_AS)
static boolean  dialog_handler_of_state_set_as( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam);
#endif
static boolean dialog_handler_of_state_play_msg( Recorder* pMe, AEEEvent evt, uint16 wParam, uint32 dwParam);
static boolean  dialog_handler_of_state_storage_setup( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam);

static void drawModalDialog( IDisplay* pd, IStatic* pStatic, uint16 stringResId, boolean confirm);

static void recorder_play( Media* pme);
static void recorder_record2( Media* pme);
static void recorder_replay( Media* pme);
static int32 recorder_set_media_device_auto( Media* pme);

static int32 recorder_set_media_device_auto_ex( Media* pme);	//Add By zzg 2010_07_28

#if !defined( AEE_SIMULATOR)
#if defined( FEATURE_RECORDER_SET_AS_RINGTONE) || defined( FEATURE_RECORDER_SET_AS)
static boolean recorder_list_set_as_call_ringtone( Recorder* pme);
static void recorder_delete_memo_ringer_if( IFileMgr* pFileManager, const char* path);
#endif
#if defined( FEATURE_RECORDER_SET_AS)
static boolean recorder_list_set_as_alarm_ringtone( Recorder* pme);
#endif
#endif
static int recorder_list_create_menu( Recorder* pme, IMenuCtl* pMenu, char* toSelect);
static boolean recorder_format_file_name( Media* pme, char* fileNameBuffer, int fileNameBufferLenght);
#if defined( FEATURE_AUTO_SAVE_WHEN_TIME_LIMIT_REACHED)
static void recorder_close_record_dialog_cb( Recorder* pme);
#endif
static void recorder_scroll_title( Recorder* pme);
#ifdef FEATURE_VERSION_C316
static void recorder_Sleep( Recorder* pme);  //add by pyuangui 2013-01-07
#endif
extern void OEMOS_Sleep(uint32 nMSecs);
//-------------------------------------------------------------------------------------------------
int Recorder_ShowDialog( Recorder* pme, uint16 dlgResId)
{
	int returnValue = 0;

	// At most one dialog open at once
	if( ISHELL_GetActiveDialog( pme->m_pShell) != NULL)
	{
		return EFAILED;
	}

	returnValue = ISHELL_CreateDialog( pme->m_pShell, AEE_RECORDER_IMAGES_RES_FILE, dlgResId, NULL);

	if( returnValue != SUCCESS)
	{
	}
	return returnValue;
}

boolean Recorder_RouteDialogEvent( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
	switch( pme->m_nActiveDialogId)
	{
		case IDD_MAIN:
			MSG_FATAL("IDD_MAIN.............",0,0,0);
			return dialog_handler_of_state_main( pme, evt, wParam, dwParam);
		case IDD_RECORD:
			MSG_FATAL("IDD_RECORD.............",0,0,0);
			return dialog_handler_of_state_record( pme, evt, wParam, dwParam);
		case IDD_RECORD_LIST:
			MSG_FATAL("IDD_RECORD_LIST.............evt=%d,wParam=%d",evt,wParam,0);
			return dialog_handler_of_state_record_list( pme, evt, wParam, dwParam);
#if defined( FEATURE_RECORDER_SET_AS)
		case IDD_SET_AS_RECORD:
			MSG_FATAL("IDD_SET_AS_RECORD.............",0,0,0);
			return dialog_handler_of_state_set_as( pme, evt, wParam, dwParam);
#endif
		case IDD_SET_PLAY_MSG:
			MSG_FATAL("IDD_SET_PLAY_MSG.............",0,0,0);
			return dialog_handler_of_state_play_msg( pme, evt, wParam, dwParam);
		case IDD_STORAGE_SETUP:
			MSG_FATAL("IDD_STORAGE_SETUP.............",0,0,0);
			return dialog_handler_of_state_storage_setup( pme, evt, wParam, dwParam);
		default:
			return FALSE;
	}
}


//-----------------------------------------------------------------------------------------------
static void drawModalDialog1( IDisplay* pd, IStatic* pStatic, AECHAR* ptext, boolean confirm)
{

	PromptMsg_Param_type    parm;

	MEMSET( &parm, 0, sizeof( parm));
	parm.ePMsgType  = confirm ? MESSAGE_CONFIRM : MESSAGE_INFORMATIVE;
	parm.eBBarType  = confirm ? BTBAR_OK_CANCEL : BTBAR_BACK;
	parm.pwszMsg 	= ptext;
	DrawPromptMessage( pd, pStatic, &parm);
}

static void drawModalDialog2( IDisplay* pd, IStatic* pStatic, char* resFile, uint16 stringResId, boolean confirm)
{

	AECHAR text[128] = {0};

	ISHELL_LoadResString( AEE_GetShell(),
					   resFile,
					   stringResId,
					   text,
					   256
				   );
	drawModalDialog1( pd, pStatic, text, confirm);
}

static void drawModalDialog( IDisplay* pd, IStatic* pStatic, uint16 stringResId, boolean confirm)
{
	drawModalDialog2( pd, pStatic, AEE_RECORDER_RES_FILE, stringResId, confirm);
}

static void drawWaitingDialog( IDisplay* pd, IStatic* pStatic, uint16 stringResId)
{

	AECHAR text[128] = {0};
	PromptMsg_Param_type    parm;

	ISHELL_LoadResString( AEE_GetShell(),
					   AEE_RECORDER_RES_FILE,
					   stringResId,
					   text,
					   256
				   );

	MEMSET( &parm, 0, sizeof( parm));
	parm.ePMsgType  = MESSAGE_WAITING;
	parm.eBBarType  = BTBAR_NONE;
	parm.pwszMsg 	= text;
	DrawPromptMessage( pd, pStatic, &parm);
}

static void drawImage1( Recorder* pme, AEERect* pclip, const char* resFile, uint16 resId)
{

	IImage* image = ISHELL_LoadResImage( pme->m_pShell, resFile, resId);

	if( image != NULL)
	{
		int             x    = 0;
		int             y    = 0;
		int				x1   = pclip->x + pclip->dx;
		int				y1   = pclip->y + pclip->dy;
		AEEImageInfo    ii   = {0};

		IIMAGE_GetInfo( image, &ii);
		IDISPLAY_SetClipRect( pme->m_pDisplay, pclip);

		x = pclip->x;
		while( x < x1)
		{

			y = pclip->y;
			while( y < y1)
			{
				IIMAGE_Draw( image, x, y);
				y += ii.cy;
			}
			x += ii.cx;
		}

		IDISPLAY_SetClipRect( pme->m_pDisplay, 0);
		IIMAGE_Release( image);
	}
}

#if defined( AEE_SIMULATOR)
static void drawImage( Recorder* pme, uint16 resId)
{

	drawImage1( pme, &pme->m_rc, AEE_RECORDER_IMAGES_RES_FILE, resId);
}
#endif
#if 0
static void drawBgMask( Recorder* pme, boolean fullScreen)
{

	AEERect rc = pme->m_rc;

	if( !fullScreen)
	{
		rc.dy = pme->m_rc.dy / 2;
		rc.y  = pme->m_rc.dy / 4;
	}
	drawImage1( pme, &rc, AEE_APPSCOMMONRES_IMAGESFILE, IDI_DIALOG);
}
#endif
static void drawImageWithOffset( Recorder* pme, IImage* image, int x, int y, AEERect *pRectOffset)
{

    if( image != NULL)
    {

        AEERect oldClip = {0};
        AEERect clip    = {0};

        SETAEERECT( &clip, x, y, pRectOffset->dx, pRectOffset->dy);
        IDISPLAY_GetClipRect( pme->a.m_pIDisplay, &oldClip);
        IDISPLAY_SetClipRect( pme->a.m_pIDisplay, &clip);

        IIMAGE_SetOffset( image, pRectOffset->x, pRectOffset->y);
        IIMAGE_SetDrawSize( image, pRectOffset->dx, pRectOffset->dy);
        IIMAGE_Draw( image, x, y);
        IIMAGE_SetOffset( image, 0, 0);
        IIMAGE_SetDrawSize( image, 9999, 9999);

        IDISPLAY_SetClipRect( pme->a.m_pIDisplay, &oldClip);
    }
}

static void drawText( IDisplay* pDisplay,
		AECHAR   *text,
		AEERect  *rect,
		RGBVAL   theTextColor,
		AEEFont  font,
		uint32   dwAlignFlags
)
{

	RGBVAL  oldColor       = 0;
	int     textLength     = ( text == NULL) ? 0 : WSTRLEN( text);
	int     textOffset     = 0;
	int     fontHeight     = 0;
	int     y              = rect->y;
	int     lineNumber     = 0;
	int     maxLineNumber  = 0;
	AEERect oldClip        = {0};
	AEERect rectDrawing    = *rect;

	if( pDisplay == NULL || rect == NULL || textLength == 0)
	{
		return;
	}

	fontHeight = IDISPLAY_GetFontMetrics( pDisplay, font, 0, 0);
	oldColor   = IDISPLAY_SetColor( pDisplay, CLR_USER_TEXT, theTextColor);
	IDISPLAY_GetClipRect( pDisplay, &oldClip);
	IDISPLAY_SetClipRect( pDisplay, rect);

	maxLineNumber = rectDrawing.dy / fontHeight;

	do
	{

		int     fits = 0;
		boolean newLineFound = FALSE;

		if( IDISPLAY_MeasureTextEx( pDisplay,
							  font,
							  text + textOffset,
							  textLength - textOffset,
							  rectDrawing.dx,
							  &fits
					   ) == 0
		)
		{
			break;
		}


		{
			AECHAR* result = WSTRCHR( text + textOffset, '\\');
			int     length = result - text - textOffset;
			if( result != NULL && (length + textOffset < textLength) && result[1] == 'n')
			{
				newLineFound = TRUE;
				fits = min( fits, length);
			}
			else
			{
				newLineFound = FALSE;
			}
		}

		textOffset += fits;
		lineNumber ++;

		if( newLineFound)
		{
			textOffset += 2;
		}
	}
	while( textOffset < textLength);

	y = max( rectDrawing.y - ( maxLineNumber - 1) * fontHeight / 2,
			rectDrawing.y - ( lineNumber - 1) * fontHeight / 2);
	rectDrawing.y = y;
	textOffset = 0;

	do
	{

		int     fits = 0;
		AECHAR  line[64];
		boolean newLineFound = FALSE;

		if( IDISPLAY_MeasureTextEx( pDisplay,
							  font,
							  text + textOffset,
							  textLength - textOffset,
							  rectDrawing.dx,
							  &fits
					   ) == 0
		)
		{
			break;
		}

		{
			AECHAR* result = WSTRCHR( text + textOffset, '\\');
			int     length = result - text - textOffset;
			if( result != NULL && (length + textOffset < textLength) && result[1] == 'n')
			{
				newLineFound = TRUE;
				fits = min( fits, length);
			}
			else
			{
				newLineFound = FALSE;
			}
		}
		WSTRNCOPYN( line, 64, text + textOffset, fits);
		IDISPLAY_DrawText( pDisplay,
					font,
					line,
					-1,
					rectDrawing.x,
					y,
					&rectDrawing,
					dwAlignFlags | IDF_TEXT_TRANSPARENT
				);

		textOffset      += fits;
		y               += fontHeight;
		rectDrawing.y   += fontHeight;

		if( newLineFound)
		{
			textOffset += 2;
		}
	}
	while( textOffset < textLength);

	IDISPLAY_SetColor( pDisplay, CLR_USER_TEXT, oldColor);
	IDISPLAY_SetClipRect( pDisplay, &oldClip);
}

static void drawText1( Recorder* pme, AECHAR* text, AEERect* pclip, uint32 dwAlignFlags)
{

	drawText( pme->m_pDisplay, text, pclip, pme->m_ThemeTextColor, AEE_FONT_NORMAL, dwAlignFlags);
}

static void drawText5( Recorder* pme, const char* resFile, int16 resId, AEERect* pclip, uint32 dwAlignFlags)
{

	AECHAR text[256]   = {0};
	ISHELL_LoadResString( pme->m_pShell,
						  resFile,
						  resId,
						  text,
						  sizeof( text)
					  );

	drawText1( pme, text, pclip, dwAlignFlags);
}

static void drawText2( Recorder* pme, int16 resId, AEERect* pclip, uint32 dwAlignFlags)
{

	drawText5( pme, AEE_RECORDER_RES_FILE, resId, pclip, dwAlignFlags);
}

static void drawText3( Recorder* pme, int16 resId)
{

	AECHAR text[256]   = {0};
	ISHELL_LoadResString( pme->m_pShell,
						  AEE_RECORDER_RES_FILE,
						  resId,
						  text,
						  sizeof( text)
					  );

#if !defined( FEATURE_AUTO_SAVE_WHEN_TIME_LIMIT_REACHED)
	if( resId == IDS_RECORD_REACH_TIME_LIMIT)
	{
		AECHAR* dot = WSTRCHR( text, ',');
		if( dot != text)
		{
			dot[0] = 0;
		}
	}
#endif

	drawText( pme->m_pDisplay, text, &pme->m_rc, RGB_BLACK, AEE_FONT_NORMAL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
}

static void drawSoftkeyText( Recorder* pme, uint16 resId, uint32 dwAlignFlags)
{

	drawText5( pme, AEE_APPSCOMMONRES_LANGFILE, resId, &pme->m_rc, dwAlignFlags);
}

static boolean dialog_handler_of_state_main( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam)
{

	static IMenuCtl* 	pMenu 		= 0;
	static uint16		selected	= 0;
	static boolean		reserve		= 0;
    MSG_FATAL("dialog_handler_of_state_main---evt=%d,--wParam=%d----%d",evt,wParam,dwParam);
	switch (evt)
	{
        
        case EVT_DIALOG_INIT:
        {
            pMenu       = (IMenuCtl*)IDIALOG_GetControl( pme->m_pActiveDialog, IDC_MAIN_MENU);
            pRecorder   = pme;
            reserve     = 0;
            if( pme->m_ePreState == STATE_INIT && !pme->m_bSuspended)
            {
                selected = 0;
            }
        }
        return TRUE;

        case EVT_DIALOG_START:
        {

            if( recorder_get_memo_number( &pme->m_Media) < RECORDER_FILE_NUMBER_LIMIT)
            {
                IMENUCTL_AddItem( pMenu, AEE_RECORDER_RES_FILE, IDS_NEW_RECORD, IDS_NEW_RECORD, 0, 0);
            }
            IMENUCTL_AddItem( pMenu, AEE_RECORDER_RES_FILE, IDS_RECORD_LIST, IDS_RECORD_LIST, 0, 0);

			
			//IMENUCTL_AddItem( pMenu, AEE_RECORDER_RES_FILE, IDS_STORAGE_SETUP, IDS_STORAGE_SETUP, 0, 0);
			#if 0
            IMENUCTL_SetTitle( pMenu, AEE_RECORDER_RES_FILE, IDS_TITLE, 0);
			#else
			{
				AECHAR wszTitle[40] = {0};

				#ifdef FEATURE_VERSION_C337
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
                if(pme->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pme->m_pIAnn,wszTitle);
                }
			}
			#endif
            IMENUCTL_SetProperties( pMenu, IMENUCTL_GetProperties( pMenu) | MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetBottomBarType( pMenu, BTBAR_SELECT_BACK);
            IMENUCTL_SetSel( pMenu, selected);
            repaint( TRUE);
			ISHELL_GetPrefs( pme->m_pShell,
					AEECLSID_RECORDER,
					1,
					&pme->m_prefs,
			    	sizeof( RecorderPreference)
			    );
        	if( pme->m_prefs.storage < 0 || pme->m_prefs.storage > 1)
        	{
        		pme->m_prefs.storage = 0;//1;
        	}
        }
		return TRUE;

		case EVT_DIALOG_END:
		{
			if( !reserve && !pme->m_bSuspended)
			{
				selected = 0;
			}
		}
		return TRUE;

		case EVT_USER_REDRAW:
		{
			MSG_FATAL("pme->m_prefs.storage===%d",pme->m_prefs.storage,0,0);
			IMENUCTL_Redraw( pMenu);
		}
		return TRUE;

		case EVT_KEY:
		{
			switch( wParam)
			{
				case AVK_SOFT2:
				case AVK_CLR:
				{
					MSG_FATAL("AVK_CLR........",0,0,0);
					CLOSE_DIALOG(DLGRET_CANCELED);
				}
				break;
			}
		}
		return TRUE;

		case EVT_COMMAND:
		{
            selected = wParam;
			reserve = TRUE;
            wParam = IMENUCTL_GetSel(pMenu);
			switch( wParam)
			{
				case IDS_NEW_RECORD:
				{
					CLOSE_DIALOG( DLGRET_NEW_RECORD);
				}
				break;

				case IDS_RECORD_LIST:
				{
					CLOSE_DIALOG( DLGRET_RECORD_LIST);
				}
				break;
#if 0                
				case IDS_STORAGE_SETUP:
				{
					CLOSE_DIALOG( DLGRET_STORAGE_SETUP);
				}
				break;
#endif                
			}
		}
		return TRUE;

		case EVT_CTL_SEL_CHANGED:
		{
			selected = IMENUCTL_GetSel( pMenu);
		}
		return TRUE;
	}

	return FALSE;
} // dialog_handler_of_state_main

void recorder_get_time_string( Media* pme, AECHAR time[])
{

	AECHAR* format   = 0;
	int		second   = 0;
	int		hour	 = 0;
	int		minute   = 0;
	int		i 		 = 0;
	int		number   = 1;
	int		length	 = 0;

	if( pme->m_eState == MEDIA_STATE_DONE)
	{
		second = pme->m_nDuration / 1000;
	}
	else if( pme->m_eTimeDisplayType == MEDIA_TIME_DISPLAY_TYPE_ELAPSED ||
			 pme->m_eTimeDisplayType == MEDIA_TIME_DISPLAY_TYPE_ELAPSED_AND_TOTAL
	)
	{
		second = pme->m_nElapsedTime / 1000;
	}
	else if( pme->m_eTimeDisplayType == MEDIA_TIME_DISPLAY_TYPE_LEFT ||
			 pme->m_eTimeDisplayType == MEDIA_TIME_DISPLAY_TYPE_LEFT_AND_TOTAL
	)
	{
		if( pme->m_nDuration == RECORDER_TIME_LIMIT_NO_LIMIT || pme->m_nDuration == 0)
		{
			time[0] = 0;
			return;
		}
		second = ( pme->m_nDuration - pme->m_nElapsedTime) / 1000;
	}

	if( pme->m_eState != MEDIA_STATE_DONE &&
		pme->m_nDuration != 0 &&
		( pme->m_eTimeDisplayType == MEDIA_TIME_DISPLAY_TYPE_LEFT_AND_TOTAL ||
		  pme->m_eTimeDisplayType == MEDIA_TIME_DISPLAY_TYPE_ELAPSED_AND_TOTAL
		)
	)
	{
		number = 2;
	}

	hour	 = second / 3600;
	minute   = ( second % 3600) / 60;
	second   = ( second % 3600) % 60;

	for( i = 0; i < number; i ++)
	{
		if( hour == 0)
		{
			format = L"%02d:%02d";
			if( i == 0 &&
				pme->m_eState != MEDIA_STATE_DONE &&
				( pme->m_eTimeDisplayType == MEDIA_TIME_DISPLAY_TYPE_LEFT ||
				  pme->m_eTimeDisplayType == MEDIA_TIME_DISPLAY_TYPE_LEFT_AND_TOTAL
				)
			)
			{
				format = L"-%02d:%02d";
			}
			WSPRINTF( time + length, 16, format, minute, second);
		}
		else
		{
			format = L"%02d:%02d:%02d";
			if( i == 0 &&
				pme->m_eState != MEDIA_STATE_DONE &&
				( pme->m_eTimeDisplayType == MEDIA_TIME_DISPLAY_TYPE_LEFT ||
				  pme->m_eTimeDisplayType == MEDIA_TIME_DISPLAY_TYPE_LEFT_AND_TOTAL
				)
			)
			{
				format = L"-%02d:%02d:%02d";
			}
			WSPRINTF( time + length, 32, format, hour, minute, second);
		}

		if( i == 0 && number == 2 && pme->m_nDuration > 0 && pme->m_nDuration < RECORDER_TIME_LIMIT_NO_LIMIT)
		{
			length = WSTRLEN( time);
			time[length] = '/';
			length ++;

			second   = pme->m_nDuration / 1000;
			hour	 = second / 3600;
			minute   = ( second % 3600) / 60;
			second   = ( second % 3600) % 60;
		}
	}
}

static void recorder_show_media_spec( Media* pme)
{

	static uint16 resIds[]  = { IDS_MM_STATUS_INIT,
				IDS_MM_STATUS_READY,
				IDS_MM_STATUS_PAUSE,
				IDS_MM_STATUS_DONE,
				IDS_MM_STATUS_RECORDING,
				IDS_MM_STATUS_PLAYING,
				IDS_MM_STATUS_DATA_IO_DELAY,
				IDS_MM_STATUS_DONE
			};

	int 			i 			= pme->m_nVolume / 20;
	AEEImageInfo	info 		= {0};
	IImage*			image   	= pRecorder->m_pImage[RECORDER_IMAGE_PROGRESSBAR1];
	int				x			= 0;
	int				y			= pRecorder->m_rc.dy - GetBottomBarHeight( pRecorder->m_pDisplay) - 9;
	AEERect			rect    	= {0};
	int*			subState	= (int*)pRecorder->m_ptr[0];

	// progressbar
	IIMAGE_GetInfo( image, &info);
	if( pme->m_bRecorder)
	{
		x = ( pRecorder->m_rc.dx - info.cx) / 2;
	}
	IIMAGE_Draw( image, x, y);
	if( ( !pme->m_bRecorder && pme->m_nDuration > 0) ||
		( pme->m_bRecorder && pme->m_nTimeLimit > 0)
	)
	{
		uint32 total = pme->m_bRecorder ? pme->m_nTimeLimit : pme->m_nDuration;

		SETAEERECT( &rect,
					0,
					0,
					pme->m_eState == MEDIA_STATE_DONE ? info.cx : ( ( pme->m_nElapsedTime * info.cx) / total),
					1
				);
		drawImageWithOffset( pRecorder, pRecorder->m_pImage[RECORDER_IMAGE_PROGRESSBAR2], x, y, &rect);
	}

	// draw time
	{
		AECHAR time[32] = {0};

		recorder_get_time_string( pme, time);
		SETAEERECT( &rect,
		            0,
		            y - GetBottomBarHeight( pRecorder->m_pDisplay),
		            pme->m_bRecorder ? pRecorder->m_rc.dx :
		            		max( info.cx, IDISPLAY_MeasureText( pRecorder->m_pDisplay, AEE_FONT_NORMAL, time)),
		            GetTitleBarHeight( pRecorder->m_pDisplay)
				);
		drawText1( pRecorder, time, &rect, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	}

	// volume
	if( !pme->m_bRecorder)
	{
		image   = pRecorder->m_pImage[RECORDER_IMAGE_VOLUME0 + i];
		IIMAGE_GetInfo( image, &info);
		x = pRecorder->m_rc.dx - info.cx;
		y = pRecorder->m_rc.dy - GetBottomBarHeight( pRecorder->m_pDisplay) - info.cy -8;
		IIMAGE_Draw( image, x, y);
	}

}

static void recorder_draw_button( Recorder* pme, uint16 resId, int type)
{

	uint32 	props[] = { IDF_ALIGN_LEFT, IDF_ALIGN_CENTER};
	AEERect rect 	= pme->m_rc;

	rect.x ++;
	rect.dy -= 1;
	drawText2( pme, resId, &rect, props[type] | IDF_ALIGN_BOTTOM);
}

static boolean recorder_str_2_wstr( char* pSrc, int srcLength, AECHAR* pDest, int destLength)
{

	debug( ";---------------------------------");
	debug( ";recorder_str_2_wstr");

	if( !pDest || !pSrc || destLength <= 0)
	{
		return FALSE;
	}
	else if( UTF8TOWSTR( (const byte*)pSrc, srcLength, pDest, destLength))
	{
		debug( ";it is UTF8");
		return TRUE;
	}
#if defined( FEATURE_RECORDER_SUPPORT_CHINESE_FILE_NAME)
#if defined( FEATURE_LANG_TCHINESE) // traditional chinese
	else
	{

		int 	counter 	= 0;
		int 	byte1		= 0;
		int 	byte2 		= 0;
		char*	src			= pSrc;
		int 	subset		= 0;
		boolean found 		= 0;
		int 	row			= 0;
		int 	cell		= 0;
		int		subsets		= ARRAY_SIZE( big5);

		debug( ";it is not UTF8, try big5");

		while( ((pSrc -src + 1) <= srcLength) && ( byte1 = (int)pSrc[0]) && counter < destLength)
		{
			if( byte1 < 0x80)
			{
				debug( ";ascii, %c", byte1);
				pDest[counter] = byte1;
				pSrc += 1;
			}
			else if( (pSrc -src + 2) <= srcLength && (byte2 = (int)pSrc[1]))
			{

				for( subset = 0; subset < subsets; subset ++)
				{
					if( byte1 >= big5_byte1_interval[subset][0] &&
						byte1 <= big5_byte1_interval[subset][1] &&
						(
							( byte2 >= big5_byte2_interval[subset][0] &&
							  byte2 <= big5_byte2_interval[subset][1]
							)
							||
							( byte2 >= big5_byte2_interval[subset][2] &&
							  byte2 <= big5_byte2_interval[subset][3]
							)
						)
					)
					{
						row		= byte1 - big5_byte1_interval[subset][0];
						cell	= byte2 - (byte2 < 0xa1 ? big5_byte2_interval[subset][0] : (big5_byte2_interval[subset][2] - 63));
						pDest[counter] 	= big5[subset][row][cell];
						found 			= TRUE;

						debug( ";0x%02x%02x found at big5[%d][%d][%d] = 0x%04x", byte1, byte2, subset, row, cell, pDest[counter]);
						break;
					}
				}

				if( !found)
				{
					debug( ";not found, ->0xfffd");
					pDest[counter] = 0xfffd;
				}
				pSrc += 2;
			}
			else
			{
				debug( ";no match, ->0xfffd");
				pDest[counter] = 0xfffd;
				pSrc += 1;
			}

			counter ++;
		}
		pDest[counter] = 0;
		return TRUE;
	}
#elif defined( FEATURE_LANG_CHINESE) // simplified chinese
	else
	{

		int 	counter 	= 0;
		int 	byte1		= 0;
		int 	byte2 		= 0;
		char*	src			= pSrc;
		int 	subset		= 0;
		boolean found 		= 0;
		int 	row			= 0;
		int 	cell		= 0;
		int		subsets		= ARRAY_SIZE( gbk);

		debug( ";it is not UTF8, try gbk");

		while( ((pSrc -src + 1) <= srcLength) && ( byte1 = (int)pSrc[0]) && counter < destLength)
		{
			if( byte1 < 0x80)
			{
				debug( ";ascii, %c", byte1);
				pDest[counter] = byte1;
				pSrc += 1;
			}
			else if( byte1 == 0x80)
			{
				debug( ";0x80 -> 0x20ac");
				pDest[counter] = 0x20ac;
				pSrc += 1;
			}
			else if( (pSrc -src + 2) <= srcLength && (byte2 = (int)pSrc[1]))
			{
				for( subset = 0; subset < subsets; subset ++)
				{
					if( byte1 >= gbk_byte1_interval[subset][0] &&
						byte1 <= gbk_byte1_interval[subset][1] &&
						byte2 >= gbk_byte2_interval[subset][0] &&
						byte2 <= gbk_byte2_interval[subset][1]
					)
					{
						row		= byte1 - gbk_byte1_interval[subset][0];
						cell	= byte2 - gbk_byte2_interval[subset][0];
						pDest[counter] = gbk[subset][row][cell];
						found = TRUE;
						debug( ";0x%02x%02x found at gbk[%d][%d][%d] = 0x%04x", byte1, byte2, subset, row, cell, pDest[counter]);
						break;
					}
				}

				if( !found)
				{
					debug( ";not found, ->0xfffd");
					pDest[counter] = 0xfffd;
				}
				pSrc += 2;
			}
			else
			{
				debug( ";no match, ->0xfffd");
				pDest[counter] = 0xfffd;
				pSrc += 1;
			}

			counter ++;
		}
		pDest[counter] = 0;
		return TRUE;
	}
#endif
#else
	else
	{
		int theChar = pSrc[srcLength];
		pSrc[srcLength] = 0;
		STRTOWSTR( pSrc, pDest, destLength);
		pSrc[srcLength] = theChar;
		return TRUE;
	}

#endif
}

static void recorder_wstr_2_str( const AECHAR* pSrc, int srcLength, char* pDest, int destLength)
{


	debug( ";---------------------------------");
	debug( ";recorder_wstr_2_str");

	MEMSET( pDest, 0, destLength);
#if defined( FEATURE_RECORDER_SUPPORT_CHINESE_FILE_NAME)
#if defined( FEATURE_LANG_TCHINESE) // traditional chinese
	{
		int		srcCounter 	= 0;
		int		destCounter	= 0;
		int 	byte1		= 0;
		int 	byte2 		= 0;
		int 	subset		= 0;
		boolean found 		= 0;
		int 	row			= 0;
		int 	cell		= 0;
		int		subsets		= ARRAY_SIZE( big5);

		debug( ";try big5");
		destLength --;
		while( srcCounter < srcLength && pSrc[srcCounter] && destCounter < destLength)
		{

			if( pSrc[srcCounter] < 0x80)
			{
				pDest[destCounter ++] = (char)pSrc[srcCounter];
				debug( ";ascii, %c", (char)pSrc[srcCounter]);
			}
			else
			{
				for( subset = 0; subset < subsets; subset ++)
				{
					for( byte1 = big5_byte1_interval[subset][0]; byte1 <= big5_byte1_interval[subset][1]; byte1 ++)
					{
						row = byte1 - big5_byte1_interval[subset][0];
						for( byte2 = big5_byte2_interval[subset][0]; byte2 <= big5_byte2_interval[subset][1]; byte2 ++)
						{
							cell = byte2 - big5_byte2_interval[subset][0];

							if( pSrc[srcCounter] == big5[subset][row][cell])
							{
								debug( ";0x%04x, big5[%d][%d][%d] = 0x%02x%02x", pSrc[srcCounter], subset, row, cell, byte1, byte2);
								if( destCounter + 2 <= destLength)
								{
									found = TRUE;
									pDest[destCounter ++] = byte1;
									pDest[destCounter ++] = byte2;
									debug( ";0x%04x found at big5[%d][%d][%d] = 0x%02x%02x", pSrc[srcCounter], subset, row, cell, byte1, byte2);
								}
								goto __recorder_wstr_2_str_find_result__;
							}
						}
						for( byte2 = big5_byte2_interval[subset][2]; byte2 <= big5_byte2_interval[subset][3]; byte2 ++)
						{
							cell = byte2 - big5_byte2_interval[subset][2] + 63;

							if( pSrc[srcCounter] == big5[subset][row][cell])
							{
								debug( ";0x%04x, big5[%d][%d][%d] = 0x%02x%02x", pSrc[srcCounter], subset, row, cell, byte1, byte2);
								if( destCounter + 2 <= destLength)
								{
									found = TRUE;
									pDest[destCounter ++] = byte1;
									pDest[destCounter ++] = byte2;
									debug( ";0x%04x found at big5[%d][%d][%d] = 0x%02x%02x", pSrc[srcCounter], subset, row, cell, byte1, byte2);
								}
								break;
							}
						}
					}
				}
__recorder_wstr_2_str_find_result__:
				if( !found)
				{
					debug( ";not found, ->underline");
					pDest[destCounter ++] = '_';
				}
			}

			srcCounter ++;
		}
		pDest[destCounter] = 0;
	}
#elif defined( FEATURE_LANG_CHINESE) // simplified chinese
	{
		int 	subset		= 0;
		int		byte1		= 0;
		int		byte2		= 0;
		int		srcCounter 	= 0;
		int		destCounter	= 0;
		boolean found 		= 0;
		int 	row			= 0;
		int 	cell		= 0;
		int		subsets		= ARRAY_SIZE( gbk);

		DBGPRINTF( ";try gbk");
		destLength --;
		while( srcCounter < srcLength && pSrc[srcCounter] && destCounter < destLength)
		{

			if( pSrc[srcCounter] < 0x80)
			{
				pDest[destCounter ++] = (char)pSrc[srcCounter];
				DBGPRINTF( ";ascii, %c", (char)pSrc[srcCounter]);
			}
			else if( pSrc[srcCounter] == 0x80)
			{
				if( destCounter +2 <= destLength)
				{
					pDest[destCounter ++] = 0x20;
					pDest[destCounter ++] = 0xac;
					DBGPRINTF( ";0x80, euro dollar");
				}
				else
				{
					pDest[destCounter ++] = '$';
					DBGPRINTF( ";0x80, not enough buffer, $");
				}
			}
			else
			{
				for( subset = 0; subset < subsets; subset ++)
				{
					for( byte1 = gbk_byte1_interval[subset][0]; byte1 <= gbk_byte1_interval[subset][1]; byte1 ++)
					{
						row = byte1 - gbk_byte1_interval[subset][0];
						for( byte2 = gbk_byte2_interval[subset][0];	byte2 <= gbk_byte2_interval[subset][1]; byte2 ++)
						{
							cell = byte2 - gbk_byte2_interval[subset][0];
							if( pSrc[srcCounter] == gbk[subset][row][cell])
							{
								DBGPRINTF( ";0x%04x, gbk[%d][%d][%d] = 0x%02x%02x", pSrc[srcCounter], subset, row, cell, byte1, byte2);
								if( destCounter + 2 <= destLength)
								{
									found = TRUE;
									pDest[destCounter ++] = byte1;
									pDest[destCounter ++] = byte2;
									DBGPRINTF( ";0x%04x found at gbk[%d][%d][%d] = 0x%02x%02x", pSrc[srcCounter], subset, row, cell, byte1, byte2);
								}
								break;
							}
						}
					}
				}

				if( !found)
				{
					DBGPRINTF( ";0x%04x not found, ->underline", pSrc[srcCounter]);
					pDest[destCounter ++] = '_';
				}
			}

			srcCounter ++;
		}
		pDest[destCounter] = 0;
	}
#endif
#else
	if( !WSTRTOUTF8( pSrc, srcLength, (byte*)pDest, destLength))
	{
		debug( ";to utf8 failed, try WSTRTOSTR");
		WSTRTOSTR( pSrc, pDest, destLength);
	}
	else
	{
		debug( ";to utf8 successful");
	}

#endif

}

static void recorder_compose_title( Recorder* pme)
{

	uint32 	total 	= (pme->m_Media.m_bRecorder ? pme->m_Media.m_nTimeLimit : pme->m_Media.m_nDuration) / 1000;
	int		hour	= total / 3600;
	int		minute  = ( total % 3600) / 60;
	int		second 	= ( total % 3600) % 60;
	int 	length 	= STRLEN( pme->m_Media.m_FileName + STRLEN( pme->m_Media.m_pszSaveDir)) - 4;
	int		width	= 0;

	if( pme->m_Media.m_bRecorder)
	{
		total = 0;
	}

	MEMSET( pme->m_Title, 0, sizeof( pme->m_Title));

	DBGPRINTF("***zzg recorder_compose_title m_FileName=%s***", pme->m_Media.m_FileName);
	DBGPRINTF("***zzg recorder_compose_title m_pszSaveDir=%s, length=%d***", pme->m_Media.m_pszSaveDir, STRLEN( pme->m_Media.m_pszSaveDir));
	
	recorder_str_2_wstr( pme->m_Media.m_FileName + STRLEN( pme->m_Media.m_pszSaveDir), length, pme->m_Title, sizeof( pme->m_Title));
	length = WSTRLEN( pme->m_Title);

	DBGPRINTF("***zzg recorder_compose_title total=%d, length=%d***", total, length);
	
	if( total == 0)
	{
		pme->m_Title[length] = 0;
	}
	else
	{
		pme->m_Title[length] = '(';
		length ++;

		if( hour == 0)
		{
			WSPRINTF( pme->m_Title + length, sizeof( pme->m_Title), L"%02d:%02d", minute, second);
		}
		else
		{
			WSPRINTF( pme->m_Title + length, sizeof( pme->m_Title), L"%02d:%02d:%02d", hour, minute, second);
		}
		length = WSTRLEN( pme->m_Title);
		pme->m_Title[length] = ')';
	}

	width = IDISPLAY_MeasureText( pme->m_pDisplay, AEE_FONT_NORMAL, pme->m_Title);
	if( width > pme->m_rc.dx)
	{
		pme->m_bScroll = TRUE;
		recorder_scroll_title( pme);
	}
	else
	{
		pme->m_bScroll 	= FALSE;
		pme->m_nTitleX 	= ( pme->m_rc.dx - width) / 2;
	}
}

#if !defined( FEATURE_RECODER_REPLAY)
static void recorder_record_back_timer_cb( Recorder* pme)
{
	CLOSE_DIALOG(DLGRET_CANCELED);
	pme->m_bLockkey = FALSE;
}
#endif
static void recorder_media_event_handler( void* pUser, MediaEventNotify* pEventNotify)
{

	Recorder* pme = (Recorder*)pUser;

	switch( pEventNotify->eEvent)
	{
		case MEDIA_EVENT_TICK_UPDATE:
			break;

		case MEDIA_EVENT_TOTAL_TIME:
		{
			recorder_compose_title( pme);
		}
		break;

		case MEDIA_EVENT_TIME_LIMIT_REACHED:
		{
#if defined( FEATURE_AUTO_SAVE_WHEN_TIME_LIMIT_REACHED)
			ISHELL_SetTimer( pme->m_pShell, 1500, (PFNNOTIFY)recorder_close_record_dialog_cb, pme);
#endif
		}
		break;

		case MEDIA_EVENT_ERROR:
		case MEDIA_EVENT_SPACE:
		case MEDIA_EVENT_PAUSED:
			if( pEventNotify->eEvent == MEDIA_EVENT_PAUSED)
			{
				pme->m_bLockkey = FALSE;
			}			
			break;

		case MEDIA_EVENT_STARTED:
		{
			if( *((int*)pme->m_ptr[0]) == 2)
			{
				Media* 	pmedia 	= pEventNotify->pMedia;
				int		result	= 0;

				debug( ";********* it is pseudo paused");
				debug( ";to seek to %d", pmedia->m_nElapsedTime);
				if( ( result = IMEDIA_Seek( pmedia->m_pMedia, MM_SEEK_CURRENT, pmedia->m_nElapsedTime)) != SUCCESS)
				{
					debug( ";seek failed, 0x%x", result);
				}
				*((int*)pme->m_ptr[0]) = 0;
			}
			pme->m_bLockkey = FALSE;
		}
		break;

		case MEDIA_EVENT_RESUME:
		{
			int* subState = (int*)pme->m_ptr[0];

			if( pme->m_Media.m_bRecorder && *subState == 1)
			{
				*subState = 0;
			}
			pme->m_bLockkey = FALSE;
		}
		break;

#if !defined( FEATURE_RECODER_REPLAY)
		case MEDIA_EVENT_DONE:
		{
			pme->m_bLockkey = TRUE;
			ISHELL_SetTimer( pme->m_pShell, 500, (PFNNOTIFY)recorder_record_back_timer_cb, pme);
		}
		break;
#endif
	}
}

static void recorder_draw_softkey( Recorder* pme, uint16 soft1, uint16 soft2)
{

	if( soft1 != 0)
	{
		drawSoftkeyText( pme, soft1, IDF_ALIGN_LEFT|IDF_ALIGN_BOTTOM);
	}
	if( soft2 != 0)
	{
		drawSoftkeyText( pme, soft2, IDF_ALIGN_RIGHT|IDF_ALIGN_BOTTOM);
	}
}

#if defined( AEE_SIMULATOR)
static void recorder_tick_simulator( Recorder* pme)
{
	repaint( TRUE);

	if( pme->m_Media.m_eState == MEDIA_STATE_PLAYING || pme->m_Media.m_eState == MEDIA_STATE_RECORDING)
	{
		pme->m_Media.m_nElapsedTime += pme->m_Media.m_nTick;
	}

	if( pme->m_Media.m_nElapsedTime >= pme->m_Media.m_nDuration ||
		pme->m_Media.m_nElapsedTime >= pme->m_Media.m_nTimeLimit
	)
	{
		pme->m_Media.m_eState = MEDIA_STATE_DONE;
	}
	else
	{
		ISHELL_SetTimer( pme->m_pShell, 1000, recorder_tick_simulator, pme);
	}
}
#endif

//Add by pyuangui 2013-01-07
#ifdef FEATURE_VERSION_C316
static void recorder_Sleep( Recorder* pme)
{
	if(pme->m_bFristStart)
	  dialog_handler_of_state_record(pme, EVT_KEY, AVK_INFO, 0);
}
#endif
//Add end

static void recorder_scroll_title( Recorder* pme)
{

	if( pme->m_bScroll)
	{
		pme->m_nTitleX -= 8;
		if( pme->m_nTitleX <= -IDISPLAY_MeasureText( pme->m_pDisplay, AEE_FONT_NORMAL, pme->m_Title))
		{
			pme->m_nTitleX = pme->m_rc.dx;
		}
	}

	repaint( TRUE);
	ISHELL_SetTimer( pme->m_pShell, 500, (PFNNOTIFY)recorder_scroll_title, pme);
}

uint16 recorder_get_res_id_from_error_code( Media* pme)
{
	uint16 	resId   = 0;
	int		error	= pme->m_nLastOperationError;


	if( error >= FILE_ERROR && error <= ENOMEDIA)
	{
		resId = IDS_RECORD_OPEN_FILE_ERROR_1 + error - FILE_ERROR;
	}
	else
	{
		uint16 resIds[] = { IDS_RECORD_MEDIA_ERROR,
					IDS_RECORD_MEDIA_ERROR_EBADSTATE,
					IDS_RECORD_MEDIA_ERROR_EBADPARM,
					IDS_RECORD_MEDIA_ERROR_EUNSUPPORTED,
					IDS_RECORD_MEDIA_ERROR_EITEMBUSY,
					IDS_RECORD_MEDIA_ERROR_MM_EBADMEDIADATA,
					IDS_MM_STATUS_ABORT,
					IDS_MM_STATUS_SPACE_WARNING,
					IDS_MM_STATUS_SPACE_ERROR,
					IDS_RECORD_REACH_TIME_LIMIT
				};
		int lastIndex = ARRAY_SIZE( resIds) - 1;

		if( error < 0 || error > lastIndex)
		{
			error = lastIndex;
		}
		resId = resIds[error];
	}

	return resId;
}

#if defined( FEATURE_AUTO_SAVE_WHEN_TIME_LIMIT_REACHED)
static void recorder_close_record_dialog_cb( Recorder* pme)
{

	*((boolean*)pme->m_ptr[1]) = TRUE;
	CLOSE_DIALOG( DLGRET_CANCELED);
}
#endif

static void recorder_notify_fmradio( boolean toCloseFmradio)
{

	debug( ";------recorder_notify_fmradio, %d", toCloseFmradio);
	ISHELL_SendEvent( AEE_GetShell(), AEECLSID_APP_FMRADIO, EVT_ALARM, toCloseFmradio, 0);
}

static void recorder_pseudo_pause_playing( Media* pme)
{

	if( pme->m_pMedia)
	{
		int result = 0;


		if( ( result = IMEDIA_Stop( pme->m_pMedia)) != SUCCESS)
		{
			debug( ";stop failed, 0x%x", result);
		}
		OEMOS_Sleep( 200);

	}
}

static void recorder_pseudo_resume_playing( Media* pme)
{

	if( pme->m_pMedia)
	{
		int result = 0;

		debug( ";--------------------------------------");
		debug( ";recorder_pseudo_resume, [%s]", pme->m_pszName);


		if( ( result = IMEDIA_Play( pme->m_pMedia)) != SUCCESS)
		{
			debug( ";replay failed, 0x%x", result);
		}
		recorder_process_media_operation_result( pme, result, MEDIA_STATE_PLAYING);
	}
}

static boolean  dialog_handler_of_state_record( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam)
{

	static boolean 	reserve 	= 0;
	// 1, discard the memo confirm, the original state is recording
	// 2, pseudo paused the playing while suspended
	// 3, discard the memo confirm, the original state is paused
	static int		subState	= 0;

	static IStatic* pStatic		= 0;


	switch (evt)
	{
		case EVT_DIALOG_INIT:
		{    
			pStatic = (IStatic*)IDIALOG_GetControl( pme->m_pActiveDialog, IDC_RECORDER_RECORD_STATIC);


			if( !pme->m_bSuspended)
			{
				pme->m_Media.m_bMediaError = FALSE;
			}
			reserve = 0;
			if( pme->m_ePreState == STATE_MAIN && !pme->m_bSuspended)
			{
				subState = 0;
			}
			pme->m_ptr[0] = (int)&subState;
			pme->m_ptr[1] = (int)&reserve;
			#ifdef FEATURE_VERSION_C316
			pme->m_bFristStart = TRUE;
			#endif
			recorder_set_media_event_notify( &pme->m_Media, recorder_media_event_handler, pme);
		}
		return TRUE;

		case EVT_DIALOG_START:
		{			
#if defined( AEE_SIMULATOR)
			if( pme->m_Media.m_eState != MEDIA_STATE_PAUSED)
			{
				pme->m_Media.m_eState = ( pme->m_Media.m_bRecorder ? MEDIA_STATE_RECORDING : MEDIA_STATE_PLAYING);
				if( pme->m_Media.m_bRecorder)
				{
					recorder_format_file_name( &pme->m_Media, pme->m_Media.m_FileName, sizeof( pme->m_Media.m_FileName));
				}

				pme->m_Media.m_nDuration 	= 180000;
				pme->m_Media.m_nElapsedTime = 0;
				pme->m_Media.m_eTimeDisplayType	= MEDIA_TIME_DISPLAY_TYPE_LEFT_AND_TOTAL;
				ISHELL_SetTimer( pme->m_pShell, 1000, recorder_tick_simulator, pme);
			}
			recorder_compose_title( pme);
			pme->m_bScroll = TRUE;
#else
			if( !pme->m_bSuspended)
			{
				if( pme->m_Media.m_bRecorder)
				{
					recorder_record2( &pme->m_Media);
				}
				else
				{
					recorder_play( &pme->m_Media);
				}
			}
#if defined( FEATURE_RECORDER_PLAY_PAUSE)
			else
			{
				if( subState == 2)
				{
					pme->m_Media.m_eState = MEDIA_STATE_PAUSED;
				}
			}
#endif
				recorder_compose_title( pme);
#endif
			if( pme->m_bScroll)
			{
				pme->m_nTitleX = 0;
				ISHELL_SetTimer( pme->m_pShell, 100, (PFNNOTIFY)recorder_scroll_title, pme);
			}
			repaint( TRUE);
		}
		return TRUE;

		case EVT_DIALOG_END:
		{		
			ISHELL_CancelTimer( pme->m_pShell, 0, pme);
			#ifdef FEATURE_VERSION_C316
            pme->m_bFristStart = TRUE;
			#endif
#if !defined( AEE_SIMULATOR)

			if (pme->m_bSuspended)
			{
                if (pme->m_Media.m_eState == MEDIA_STATE_RECORDING)
				{
                	recorder_pause( &pme->m_Media);
                }

				if( pme->m_Media.m_eState == MEDIA_STATE_PLAYING)
				{
					db_items_value_type dbItemValue      = {0};
					db_get( DB_IN_USE, &dbItemValue);

					recorder_stop_if( &pme->m_Media);		//Add By zzg 2012_03_01
					//recorder_pause( &pme->m_Media);
					if( dbItemValue.in_use)
					{
						MOVE_TO_STATE( STATE_RECORD_LIST);
					}
				}
			}
			else
			{
#if !defined( FEATURE_RECORDER_PLAY_PAUSE) && !defined( FEATURE_RECORDER_RECORD_PAUSE)
__dialog_handler_of_state_record_stop__:
#endif
				subState = pme->m_bSuspended ? subState : 0;
				recorder_stop_if( &pme->m_Media);
#if 0 // END
				if( !reserve && pme->m_Media.m_bRecorder)
				{
					recorder_cancel_record( &pme->m_Media);
				}
#endif
			}

			if( !pme->m_bSuspended)
			{
				recorder_init_media( &pme->m_Media);
			}
#if !defined( FEATURE_RECORDER_PLAY_PAUSE) && !defined( FEATURE_RECORDER_RECORD_PAUSE)
			else
			{
				if( pme->m_Media.m_eState == MEDIA_STATE_RECORDING)
				{
					MOVE_TO_STATE( STATE_MAIN);
				}
				else
				{
					MOVE_TO_STATE( STATE_RECORD_LIST);
				}
			}
#endif
#endif
		}
		return TRUE;

		case EVT_USER_REDRAW:
		{
			#if 0
			IMENUCTL_SetTitle( pMenu, AEE_RECORDER_RES_FILE, IDS_RECORD_LIST_TITLE, 0);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pme->m_pShell,
                        AEE_RECORDER_RES_FILE,                                
                        IDS_RECORD_LIST_TITLE,
                        WTitle,
                        sizeof(WTitle));
                if(pme->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
                }
		    }
			#endif
#if defined( AEE_SIMULATOR)
			drawImage( pme, pme->m_Media.m_bRecorder ? IDI_RECORDER_BG_RECORD : IDI_RECORDER_BG_PLAY);
#else
			IIMAGE_Draw( pme->m_pImage[pme->m_Media.m_bRecorder ? RECORDER_IMAGE_BG_RECORD : RECORDER_IMAGE_BG_PLAY], 0, 0);
#endif


			IDISPLAY_SetColor( pme->m_pDisplay, CLR_USER_TEXT, pme->m_ThemeTextColor);
			IDISPLAY_DrawText( pme->m_pDisplay, AEE_FONT_NORMAL, pme->m_Title, -1, pme->m_nTitleX, 0, 0, IDF_TEXT_TRANSPARENT);

			MSG_FATAL("****pyg****pme->m_Media.m_bMediaError=%d",pme->m_Media.m_bMediaError,0,0);
			if( pme->m_Media.m_bMediaError)
			{
				uint16 	resId   = recorder_get_res_id_from_error_code( &pme->m_Media);
				drawModalDialog( pme->m_pDisplay, pStatic, resId, FALSE);

			}
			else
			{

				static BottomBar_e_Type	button[][2] = { { 0, IDS_BACK}, { IDS_SAVE, IDS_BACK}};
				#ifdef FEATURE_VERSION_C316     //Add by pyuangui 2013-01-07
				static uint16			resId[]	  	= { IDS_RECORD_START, IDS_RECORD_PAUSE};
				#else
				static uint16			resId[]	  	= { IDS_RECORD_RESUME, IDS_RECORD_PAUSE};
				#endif
				int						state	  	= ( pme->m_Media.m_eState == MEDIA_STATE_PAUSED ? 0 : 1);
				Media*					pmedia    	= &pme->m_Media;

				recorder_show_media_spec( pmedia);
				recorder_draw_softkey( pme, button[pmedia->m_bRecorder][0], button[pmedia->m_bRecorder][1]);
#if defined( FEATURE_RECODER_REPLAY)
				if( pmedia->m_eState == MEDIA_STATE_DONE && !pmedia->m_bRecorder)
				{
					recorder_draw_button( pme, IDS_RECORD_REPLAY, pmedia->m_bRecorder);
				}
				else
#else
				if( pmedia->m_eState != MEDIA_STATE_DONE || pmedia->m_bRecorder)
#endif
				{
#if !defined( FEATURE_RECORDER_PLAY_PAUSE) && !defined( FEATURE_RECORDER_RECORD_PAUSE)
					if( FALSE)
#else
					if( 0
#if defined( FEATURE_RECORDER_RECORD_PAUSE)
						|| pmedia->m_bRecorder
#endif
#if defined( FEATURE_RECORDER_PLAY_PAUSE)
					    || !pmedia->m_bRecorder
#endif
					)
#endif
                  #if 0  
       			   if(pme->m_bFristStart)
                   {
                      pme->m_Media.m_eState = MEDIA_STATE_PAUSED;
       			      subState = 0;
       			      recorder_pause( &pme->m_Media);
                   }
		          #endif
					recorder_draw_button( pme, resId[state], pmedia->m_bRecorder);
				}
			}

			if( subState == 1 || subState == 3)
			{
				drawModalDialog( pme->m_pDisplay, pStatic, IDS_RECORD_CANCEL_CONFIRM, TRUE);
			}
			IDISPLAY_UpdateEx( pme->m_pDisplay, FALSE);
			MSG_FATAL("pme->m_bAppIsReady=%d ---- pme->m_bLockkey=%d",pme->m_bAppIsReady, pme->m_bLockkey,0);
            //Add by pyuangui 2013-01-07
            #ifdef FEATURE_VERSION_C316
			if(pme->m_bFristStart)
			{
			  ISHELL_SetTimer( pme->m_pShell, 500, (PFNNOTIFY)recorder_Sleep, pme);
			}
			#endif
			//Add end
		}
		return TRUE;

		case EVT_KEY:
		{
			switch( wParam)
			{
#if defined( AEE_SIMULATOR)
				case AVK_SOFT1:
#else
				case AVK_SELECT:					
#endif
					MSG_FATAL("AVK_SELECT.................",0,0,0);
					MSG_FATAL("***zzg AVK_SELECT m_bRecorder=%x, subState=%x***", pme->m_Media.m_bRecorder, subState, 0);
					
					if( pme->m_Media.m_bRecorder && ( subState == 1 || subState == 3))
					{
						wParam = AVK_CLR;
						goto __dialog_handler_of_state_record_discard__;
					}
					else if( !pme->m_Media.m_bRecorder)
					{
						MSG_FATAL("AVK_SELECT...............RENAME",0,0,0);
						goto __dialog_handler_of_state_record_pause_resume__;
					}
					else if( pme->m_Media.m_bMediaError)
					{
						break;
					}
				case AVK_CLR:
				case AVK_SOFT2:
				{		
					if( pme->m_Media.m_bRecorder && ( subState == 1 || subState == 3))
					{						
						if( !pme->m_Media.m_bMediaError &&
							pme->m_Media.m_eState == MEDIA_STATE_PAUSED
						)
						{							
							if( subState == 1)
							{								
								pme->m_bLockkey = TRUE;
								recorder_resume( &pme->m_Media);
							}
							else if( subState == 3)
							{								
								goto __dialog_handler_of_state_record_discard_back__;
							}
						}
						else
						{
__dialog_handler_of_state_record_discard_back__:
							subState = 0;
							repaint( TRUE);
						}
					}
					else if( pme->m_Media.m_bMediaError)
					{						
						recorder_stop_if( &pme->m_Media);
						CLOSE_DIALOG( DLGRET_CANCELED);
					}
					else
#if defined( FEATURE_AUTO_SAVE_WHEN_TIME_LIMIT_REACHED)
					if( pme->m_Media.m_nLastOperationError != MEDIA_ERROR_TIME_LIMIT_REACHED)
#endif
					{
						if( wParam == AVK_CLR && pme->m_Media.m_bRecorder && subState == 0)
						{							
							if( !pme->m_Media.m_bMediaError &&
								pme->m_Media.m_eState == MEDIA_STATE_RECORDING
							)
							{								
								pme->m_bLockkey = TRUE;
								recorder_pause( &pme->m_Media);
								subState = 1;
							}
							else
							{								
								subState = 3;								
							}
							
							repaint( TRUE);
							break;
						}
__dialog_handler_of_state_record_discard__:
#if !defined( AEE_SIMULATOR)	
						MSG_FATAL("***zzg __dialog_handler_of_state_record_discard__***", 0, 0, 0);
						recorder_stop_if( &pme->m_Media);
						if( wParam == AVK_CLR && pme->m_Media.m_bRecorder)
						{							
							recorder_cancel_record( &pme->m_Media);
						}
#endif
						reserve = TRUE;
						CLOSE_DIALOG( DLGRET_CANCELED);
					}
					else
					{
						CLOSE_DIALOG( DLGRET_CANCELED);
					}
				}
				break;

#if defined( AEE_SIMULATOR)
				case AVK_SELECT:
#else
				case AVK_INFO:
#endif
				{
__dialog_handler_of_state_record_pause_resume__:

#if defined( FEATURE_RECODER_REPLAY)
					if( pme->m_Media.m_eState == MEDIA_STATE_DONE && !pme->m_Media.m_bRecorder)
					{
#if defined( AEE_SIMULATOR)
						pme->m_Media.m_eState = MEDIA_STATE_PLAYING;
#else
						recorder_replay( &pme->m_Media);
#endif
					}
					else
#endif
                    if( pme->m_Media.m_eState == MEDIA_STATE_PLAYING || pme->m_Media.m_eState == MEDIA_STATE_RECORDING)
					{
						if( 0
#if defined( FEATURE_RECORDER_RECORD_PAUSE)
						    || pme->m_Media.m_bRecorder
#endif
#if defined( FEATURE_RECORDER_PLAY_PAUSE)
						    || !pme->m_Media.m_bRecorder
#endif
						)
						{
#if defined( AEE_SIMULATOR)
							pme->m_Media.m_eState = MEDIA_STATE_PAUSED;
#else
                            pme->m_bLockkey = TRUE;
							recorder_pause( &pme->m_Media);
#endif
						}
						else if( pme->m_Media.m_bRecorder)
						{
							recorder_stop_if( &pme->m_Media);
							reserve = TRUE;
							CLOSE_DIALOG( DLGRET_CANCELED);
						}
					}
					else if( pme->m_Media.m_eState == MEDIA_STATE_PAUSED)
					{
						if( 0
#if defined( FEATURE_RECORDER_RECORD_PAUSE)
						    || pme->m_Media.m_bRecorder
#endif
#if defined( FEATURE_RECORDER_PLAY_PAUSE)
						    || !pme->m_Media.m_bRecorder
#endif
						)
						{
#if defined( AEE_SIMULATOR)
							pme->m_Media.m_eState = ( pme->m_Media.m_bRecorder ? MEDIA_STATE_RECORDING : MEDIA_STATE_PLAYING);
#else
#if defined( FEATURE_RECORDER_PLAY_PAUSE)
							if( !pme->m_Media.m_bRecorder && subState == 2)
							{
								pme->m_bLockkey = TRUE;
								recorder_pseudo_resume_playing( &pme->m_Media);
							}
							else
#endif
							{
								pme->m_bLockkey = TRUE;
								recorder_resume( &pme->m_Media);
							}
#endif
						}
					}
					else
					{
						CLOSE_DIALOG( DLGRET_CANCELED);
					}
					#ifdef FEATURE_VERSION_C316
                    pme->m_bFristStart = FALSE;
					#endif
#if defined( AEE_SIMULATOR)
					repaint( TRUE);
#endif
				}
				break;

				case AVK_UP:
				case AVK_DOWN:
				{
					MSG_FATAL("pme->m_Media.m_eState====%d",pme->m_Media.m_eState,0,0);
					if( pme->m_Media.m_eState == MEDIA_STATE_PLAYING || pme->m_Media.m_eState == MEDIA_STATE_RECORDING)
					{
						recorder_set_media_volume( &pme->m_Media, pme->m_Media.m_nVolume + ( wParam == AVK_DOWN ? -20 : 20));
						repaint( TRUE);
					}
				}
				break;
				#ifdef FEATURE_ADD_VOLUP_VOLDN //xxzhen
				case AVK_VOLUME_DOWN:
				case AVK_VOLUME_UP:
					{
					if( pme->m_Media.m_eState == MEDIA_STATE_PLAYING || pme->m_Media.m_eState == MEDIA_STATE_RECORDING)
					{
						recorder_set_media_volume( &pme->m_Media, pme->m_Media.m_nVolume + ( wParam == AVK_VOLUME_DOWN ? -20 : 20));
						repaint( TRUE);
					}
				    }
				break;
				#endif
				#ifdef FEATURE_VERSION_K202_LM129C//xxzhen compatible with lm126c
				case AVK_STAR:
				case AVK_POUND:
					{
					if( pme->m_Media.m_eState == MEDIA_STATE_PLAYING || pme->m_Media.m_eState == MEDIA_STATE_RECORDING)
					{
						recorder_set_media_volume( &pme->m_Media, pme->m_Media.m_nVolume + ( wParam == AVK_STAR ? -20 : 20));
						repaint( TRUE);
					}
				    }
				break;
				#endif

				case AVK_I:
				case AVK_O:
				{
					if( pme->m_Media.m_eState == MEDIA_STATE_PLAYING || pme->m_Media.m_eState == MEDIA_STATE_RECORDING)
					{
						recorder_set_media_volume( &pme->m_Media, pme->m_Media.m_nVolume + ( wParam == AVK_I ? -20 : 20));
						repaint( TRUE);
					}
				}
				break;
				case AVK_LEFT:
				case AVK_RIGHT:
				{
					if( pme->m_Media.m_eState == MEDIA_STATE_PLAYING || pme->m_Media.m_eState == MEDIA_STATE_RECORDING)
					{
						recorder_set_media_volume( &pme->m_Media, pme->m_Media.m_nVolume + ( wParam == AVK_LEFT ? -20 : 20));
						repaint( TRUE);
					}
				}
				break;
			}
		}
		return TRUE;

		case EVT_HEADSET:
		{
			recorder_set_media_device_auto( &pme->m_Media);
		}
		return TRUE;
		case EVT_NOTIFY:
		{
			AEENotify* pNotify = (AEENotify*)dwParam;

			if( pNotify->cls == AEECLSID_BATT_NOTIFIER &&
				pNotify->dwMask == AEEBATTSTATUS_LOW
			)
			{
				recorder_stop_if( &pme->m_Media);
				CLOSE_DIALOG( DLGRET_CANCELED);
				return TRUE;
			}
		}
		break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				AEERect bottomBarRect;
				AEERect sondadd;
				AEERect sonddel;
				//int ht;
				int nBarH ;
				AEEDeviceInfo devinfo;
				nBarH = GetBottomBarHeight(pme->m_pDisplay);
				
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pme->m_pShell, &devinfo);
				SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
				if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect))
				{
					if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//右
					{						
						boolean rt = ISHELL_PostEvent(pme->m_pShell,AEECLSID_RECORDER,EVT_USER,AVK_CLR,0);
						return rt;
					}
					else if((wXPos >= bottomBarRect.x) && (wXPos < bottomBarRect.x + (bottomBarRect.dx/3)))//左
					{						
						
						boolean rt = ISHELL_PostEvent(pme->m_pShell,AEECLSID_RECORDER,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3) && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*2 )
					{
						boolean rt = ISHELL_PostEvent(pme->m_pShell,AEECLSID_RECORDER,EVT_USER,AVK_INFO,0);
						return rt;
					}
					
				}
				MSG_FATAL("wXPos====%d,wXPos====%d",wXPos,wYPos,0);
				
					
				SETAEERECT(&sonddel, 185, 245, 20, 20);
				SETAEERECT(&sondadd, 224, 252, 20, 20);
				if(TOUCH_PT_IN_RECT(wXPos, wYPos, sondadd))
				{
					recorder_set_media_volume( &pme->m_Media, pme->m_Media.m_nVolume + 20);
					repaint( TRUE);
				}
				if(TOUCH_PT_IN_RECT(wXPos, wYPos, sonddel))
				{
					recorder_set_media_volume( &pme->m_Media, pme->m_Media.m_nVolume  -20);
					repaint( TRUE);
				}			
			}
		break;
#endif
	}

	return FALSE;
} // dialog_handler_of_state_record

static void* recorder_list_strip_path_name_from_menu( Recorder* pme, IMenuCtl* pMenu)
{

	CtlAddItem item = {0};

	if( IMENUCTL_GetItem( pMenu, IMENUCTL_GetSel( pMenu), &item) && item.dwData)
	{
		FileInfo* pFileInfo = (FileInfo*)item.dwData;

		MEMSET( pme->m_FileName, 0, sizeof( pme->m_FileName));
		STRCPY( pme->m_FileName, pFileInfo->szName);
		return pFileInfo;
	}

	return NULL;
}

static void recorder_list_do_operation( Recorder* pme, IMenuCtl* pMenu, DialogReturnValueEnum operation)
{
	recorder_list_strip_path_name_from_menu( pme, pMenu);
	debug( ";-----%s", pme->m_FileName);
	MEMSET( pme->m_Media.m_FileName, 0, sizeof(pme->m_Media.m_FileName));
	STRCPY( pme->m_Media.m_FileName, pme->m_FileName);
    if(APP_MEDIA_ALLOW != app_media_scheduler())
    {
        MOVE_TO_STATE(STATE_PLAY_MSG);
        CLOSE_DIALOG(DLGRET_CREATE);
        pme->m_nActiveDialogId = IDD_SET_PLAY_MSG;

        return;
    }
	CLOSE_DIALOG( operation);
}

static void recorder_list_play( Recorder* pme, IMenuCtl* pMenu)
{
	recorder_list_do_operation( pme, pMenu, DLGRET_PLAY);
}

static void recorder_list_set_as( Recorder* pme, IMenuCtl* pMenu)
{
	recorder_list_do_operation( pme, pMenu, DLGRET_SET_AS);
}

#if defined( FEATURE_RECODER_TEST)
static void recorder_clear_all_memos( Recorder* pme)
{

	FileInfo info = {0};

	debug( ";-------------------------------------------");
	debug( ";recorder_clear_all_memos");

	if( IFILEMGR_EnumInit( pme->m_pFileManager, pme->m_Media.m_pszSaveDir, 0) == SUCCESS)
	{
		while( IFILEMGR_EnumNext( pme->m_pFileManager, &info))
		{
			if( STRENDS( RECORDER_MEMO_FILE_EXTENSION_QCP, info.szName))
			{
				debug( ";delete %s", info.szName);
				IFILEMGR_Remove( pme->m_pFileManager, info.szName);
			}
		}
	}

	if( IFILEMGR_EnumInit( pme->m_pFileManager, AEEFS_RINGERS_DIR, 0) == SUCCESS)
	{
		while( IFILEMGR_EnumNext( pme->m_pFileManager, &info))
		{
			if( STRENDS( RECORDER_MEMO_FILE_EXTENSION_QCP, info.szName))
			{
				debug( ";delete %s", info.szName);
				IFILEMGR_Remove( pme->m_pFileManager, info.szName);
			}
		}
	}
}
#endif

static void recorder_list_free_menu_data_if( Recorder* pme, IMenuCtl* pMenu)
{

	int count = IMENUCTL_GetItemCount( pMenu);
	if( count > 0)
	{

		int 		i 	 	= 0;
		CtlAddItem 	item 	= {0};

		for( i = 0; i < count; i ++)
		{
			if( IMENUCTL_GetItem( pMenu, IMENUCTL_GetItemID( pMenu, i), &item) && item.dwData)
			{
				FREEIF( item.dwData);
			}
		}
	}
}

static int recorder_list_delete_if( Recorder* pme, IMenuCtl* pMenu)
{

	debug( ";--------------------------");
	debug( ";recorder_list_delete_if");

	pme->m_nLastOperationError = SUCCESS;
	if( IMENUCTL_GetItemCount( pMenu) == 0)
	{

		debug( ";no item");
		return 1;
	}
	else
	{

		int 	sel 	= IMENUCTL_GetSel( pMenu);
		void* 	data 	= 0;

		if( !( data = recorder_list_strip_path_name_from_menu( pme, pMenu)))
		{
			debug( ";get item failed, item count is %d", IMENUCTL_GetItemCount( pMenu));
			return 2;
		}

		if( IFILEMGR_Remove( pme->m_pFileManager, pme->m_FileName) == SUCCESS)
		{
			debug( ";delete [%s] success", pme->m_FileName);
			IMENUCTL_DeleteItem( pMenu, sel);
			FREEIF( data);
		}
		else
		{
			pme->m_nLastOperationError = IFILEMGR_GetLastError( pme->m_pFileManager);
			debug( ";delete [%s] failed: 0x%x", pme->m_FileName, pme->m_nLastOperationError);
			return 3;
		}

		return 0;
	}
}

static int recorder_list_delete_all( Recorder* pme, IMenuCtl* pMenu)
{

	int 	i 		= 0;
	int 	count 	= IMENUCTL_GetItemCount( pMenu);
	int 	result	= 0;

	debug( ";-----------------------------------------------------------");
	debug( ";recorder_list_delete_all");
	if( count == 0)
	{
		debug( ";count is 0");
		return 1;
	}

	while( ( result = recorder_list_delete_if( pme, pMenu)) == 0)
	{
		i ++;
		if( i >= count)
			break;
	}
	debug( ";delete %d files totally", i);

	return result;
}

static void recorder_list_create_option_menu_if( IMenuCtl* pOption)
{

	if( IMENUCTL_GetItemCount( pOption) == 0)
	{
		IMENUCTL_AddItem( pOption, AEE_RECORDER_RES_FILE, IDS_RECORD_LIST_OPTION_PLAY, IDS_RECORD_LIST_OPTION_PLAY, 0, 0);
#if defined( FEATURE_RECORDER_SET_AS_RINGTONE)
		IMENUCTL_AddItem( pOption, AEE_RECORDER_RES_FILE, IDS_RECORD_LIST_OPTION_SET_AS_RINGTONE, IDS_RECORD_LIST_OPTION_SET_AS_RINGTONE, 0, 0);
#endif
#if defined( FEATURE_RECORDER_SET_AS)
		IMENUCTL_AddItem( pOption, AEE_RECORDER_RES_FILE, IDS_RECORD_LIST_OPTION_SET_AS, IDS_RECORD_LIST_OPTION_SET_AS, 0, 0);
#endif
#if defined( FEATURE_RECORDER_RENAME)
		IMENUCTL_AddItem( pOption, AEE_RECORDER_RES_FILE, IDS_RECORD_LIST_OPTION_RENAME, IDS_RECORD_LIST_OPTION_RENAME, 0, 0);
#endif
		IMENUCTL_AddItem( pOption, AEE_RECORDER_RES_FILE, IDS_RECORD_LIST_OPTION_DELETE, IDS_RECORD_LIST_OPTION_DELETE, 0, 0);
		IMENUCTL_AddItem( pOption, AEE_RECORDER_RES_FILE, IDS_RECORD_LIST_OPTION_DELETE_ALL, IDS_RECORD_LIST_OPTION_DELETE_ALL, 0, 0);
#if defined( FEATURE_RECODER_TEST)
		IMENUCTL_AddItem( pOption, 0, 0, 5000, L"Clear memos", 0);
#endif

		IMENUCTL_SetBottomBarType( pOption, BTBAR_SELECT_BACK);
	}
}


#if defined( FEATURE_RECORDER_RENAME)
static boolean recorder_list_new_file_name_is_valid( ITextCtl* pTextCtl)
{

	AECHAR* pText = ITEXTCTL_GetTextPtr( pTextCtl);
	while( pText[0] != 0)
	{

#if 1
		if( pText[0] == '/'  ||
			pText[0] == '\\' ||
			pText[0] == ':'  ||
			pText[0] == '*'  ||
			pText[0] == '?'  ||
			pText[0] == '"'  ||
			pText[0] == '<'  ||
			pText[0] == '>'  ||
			pText[0] == '|'
		)
#else
		if( !( ( pText[0] >= 'a' && pText[0] <= 'z') ||
			   ( pText[0] >= 'A' && pText[0] <= 'Z') ||
			   ( pText[0] >= '0' && pText[0] <= '9') ||
			   pText[0] == '.'						 ||
			   pText[0] == '_'
			 )
		)
#endif
		{
			return FALSE;
		}

		pText ++;
	}

	return TRUE;
}

static void recorder_list_show_rename_screen( Recorder* pme, IMenuCtl* pMenu, ITextCtl* pTextCtl, AECHAR text[])
{

	AEERect 	rect 		= pme->m_rc;
	CtlAddItem 	menuItem 	= { 0};

	if( IMENUCTL_GetItem( pMenu, IMENUCTL_GetSel( pMenu), &menuItem))
	{
		ITEXTCTL_SetText( pTextCtl, menuItem.pText, WSTRLEN( menuItem.pText));
	}

	ITEXTCTL_SetProperties( pTextCtl, TP_MULTILINE|TP_FRAME|TP_FIXSETRECT|TP_STARKEY_SWITCH|TP_DISPLAY_COUNT|TP_FOCUS_NOSEL|TP_GRAPHIC_BG);
	IDISPLAY_SetColor( pme->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
	ITEXTCTL_SetTitle( pTextCtl, 0, 0, L"");
	ITEXTCTL_SetInputMode( pTextCtl, AEE_TM_CURRENT);
	if( !( text[0] == 0 && text[1] == 0xffff))
	{
		ITEXTCTL_SetText( pTextCtl, text, WSTRLEN( text));
		MEMSET( text, 0, sizeof( text));
	}

#if 0
	IMENUCTL_GetSelItemRect( pMenu, &rect);
	rect.x  = 0;
	rect.dx = pme->m_rc.dx;
	rect.dy += 2;
#else
	rect.dy -= GetBottomBarHeight( pme->m_pDisplay);
#endif

	ITEXTCTL_SetRect( pTextCtl, &rect);
	ITEXTCTL_SetMaxSize( pTextCtl, RECORDER_FILE_NAME_LENGHT);
	ITEXTCTL_SetActive( pTextCtl, TRUE);
	IDIALOG_SetFocus( pme->m_pActiveDialog, IDC_RECORD_LIST_TEXT);
}
#endif

static int recorder_list_menu_sorting_criteria( Recorder* pme, FileInfo* item1, FileInfo* item2)
{

	if( item1 && item2)
	{
		debug( ";-------compare, 0x%x, 0x%x", item1->dwCreationDate, item2->dwCreationDate);
		return ( item2->dwCreationDate - item1->dwCreationDate);
	}

	return 0;
}

static boolean recorder_is_tf_card_exist( void)
{
	extern boolean CMediaGallery_GetTflashStatus(void);
	return CMediaGallery_GetTflashStatus();
}

static int recorder_list_create_menu( Recorder* pme, IMenuCtl* pMenu, char* toSelect)
{

	int select = 0;

	recorder_list_free_menu_data_if( pme, pMenu);

	IMENUCTL_DeleteAll( pMenu);

	{
		RecorderPreference prefs = {0};
		ISHELL_GetPrefs( pme->m_pShell,
				AEECLSID_RECORDER,
				1,
				&prefs,
		    	sizeof( RecorderPreference)
		    );

		//if( prefs.storage == 0 && recorder_is_tf_card_exist())
		//if( prefs.storage == 1 && recorder_is_tf_card_exist())	//Modify by zzg 2012_03_01
		if(recorder_is_tf_card_exist())	//Modify by zzg 2012_03_01
		{
			pme->m_Media.m_pszSaveDir = RECORDER_MEMO_SAVE_DIR_CARD0;
		}
		else
		{
			//pme->m_Media.m_pszSaveDir = RECORDER_MEMO_SAVE_DIR_INTERNAL;
		}
	}

	if( IFILEMGR_EnumInit( pme->m_pFileManager, pme->m_Media.m_pszSaveDir, 0) == SUCCESS)
	{
		FileInfo 	info 	 				= {0};
		int			i 		 				= 0;
		AECHAR		name[AEE_MAX_FILE_NAME] = {0};

		while( IFILEMGR_EnumNext( pme->m_pFileManager, &info))
		{

			if( info.dwSize == 0)
			{
				IFILEMGR_Remove( pme->m_pFileManager, info.szName);
				continue;
			}
#if !defined( FEATURE_RECODER_TEST)
			if( STRENDS( pme->m_Media.m_pszFileExtension, info.szName))
#endif
			{
				if( toSelect && STRENDS( toSelect, info.szName))
				{
					select = i;
				}

#if defined( FEATURE_RECODER_TEST)
				{
					char* x = info.szName + STRLEN( pme->m_Media.m_pszSaveDir);
					debug( ";----------- %s", x);
					while( x[0])
					{
						debug( ";0x%x", x[0]);
						x ++;
					}
				}
#endif

				{
					char* fileName = info.szName + STRLEN( pme->m_Media.m_pszSaveDir);
					int	  len	   = STRLEN( fileName);
					FileInfo* 	data     	= MALLOC( sizeof( FileInfo));
					if( data)
					{

						IFILEMGR_GetInfo( pme->m_pFileManager, info.szName, &info);

						MEMSET( name, 0, sizeof( name));
						MEMCPY( data, &info, sizeof( FileInfo));

						recorder_str_2_wstr( fileName, len - 4, name, sizeof( name));
						debug( ";filename %s", fileName);
						debug( ";date 0x%x, 0x%x", info.dwCreationDate, data->dwCreationDate);

#if defined( FEATURE_RECORDER_RECORD_LIST_WITH_ICON)
						{
							CtlAddItem ai = {0};

							ai.pszResImage	= AEE_RECORDER_IMAGES_RES_FILE;
							ai.wImage		= IDI_RECORDER_RECORDER_LIST_ICON;
							ai.pText		= name;
							ai.dwData		= (uint32)data;
							ai.wItemID		= i ++;

							IMENUCTL_AddItemEx( pMenu, &ai);
						}
#else
						IMENUCTL_AddItem( pMenu, 0, 0, i ++, name, (uint32)data);
#endif
					}
					else
					{
						break;
					}
				}
			}
		}

		if( IMENUCTL_GetItemCount( pMenu) > 0)
		{
			IMENUCTL_SortCustom( pMenu, (PFNMENUSORTCOMPARE)recorder_list_menu_sorting_criteria, pme);
		}
	}

	return select;
}

static void recorder_list_prompt_timer_cb( int* subState)
{

	*subState = 0;
	repaint( TRUE);
}

static void recorder_list_hide_prompt( int* subState)
{

	ISHELL_SetTimer( AEE_GetShell(), 2000, (PFNNOTIFY)recorder_list_prompt_timer_cb, subState);
}

#if !defined( AEE_SIMULATOR)
static void recorder_list_set_as_ringer_cb( Recorder* pme)
{

	IMenuCtl*	pMenu		= (IMenuCtl*)pme->m_ptr[0];
	int*		subState	= (int*)pme->m_ptr[1];

	debug( ";-----------------------------------------------");
	debug( ";recorder_list_set_as_ringtone_cb");

	recorder_list_strip_path_name_from_menu( pme, pMenu);
	if( recorder_list_set_as_call_ringtone( pme))
	{
		*subState = 8;
	}
	else
	{
		*subState = 7;
	}
	recorder_list_hide_prompt( subState);
	repaint( TRUE);
}
#endif

#if defined( FEATURE_RECORDER_RENAME)
static void recorder_list_rename_cb( Recorder* pme)
{

	ITextCtl*	pTextCtl	= (ITextCtl*)pme->m_ptr[0];
	uint16*		selected    = (uint16*)pme->m_ptr[1];
	int*		subState	= (int*)pme->m_ptr[2];
	AECHAR*		text		= (AECHAR*)pme->m_ptr[3];
	IMenuCtl*	pMenu		= (IMenuCtl*)pme->m_ptr[4];
	int			result		= 0;

	DBGPRINTF( ";---------------");
	DBGPRINTF( ";recorder_list_rename_cb");

	if( recorder_list_new_file_name_is_valid( pTextCtl))
	{
		char 	newFileName[256] 	= {0};
		char    newFileNamegb[256]    = {0};
		AECHAR* pText				= ITEXTCTL_GetTextPtr( pTextCtl);
		int		offset				= 0;
		int		len					= sizeof( newFileName);

		debug( ";new file name is valid");

		STRCPY( newFileName, pme->m_Media.m_pszSaveDir);
		offset = STRLEN( newFileName);
		//recorder_wstr_2_str((const AECHAR*)pText, WSTRLEN((AECHAR *)pText), newFileName + offset, len - offset);
		WSTRTOUTF8(pText,sizeof(AECHAR) * (255 + 1),(byte *)newFileNamegb, 256);
		STRCAT(newFileName,newFileNamegb);
		
		if( !STRENDS( pme->m_Media.m_pszFileExtension, newFileName))
		{
			STRCAT( newFileName, pme->m_Media.m_pszFileExtension);
		}
		DBGPRINTF( ";rename to %s", newFileName);
		result = IFILEMGR_Test(pme->m_pFileManager, newFileName);
		DBGPRINTF( ";result to %s", result);
		if( STRCMP( pme->m_FileName, newFileName) == 0)
		{
		}
		
		else if( IFILEMGR_Rename( pme->m_pFileManager, pme->m_FileName, newFileName) == SUCCESS)
		{
			DBGPRINTF("IFILEMGR_Rename sucessssssssssssssssss",0,0,0);
			*selected = recorder_list_create_menu( pme, pMenu, newFileName);
			result = 0;
		}
		else
		{
//			result = 1;
			DBGPRINTF( ";rename file [%s] ==%d ", pme->m_FileName,IFILEMGR_GetLastError(pme->m_pFileManager));
			DBGPRINTF( ";to [%s] failed", newFileName);
		}
	}
	else
	{
		result = 1;
		ITEXTCTL_GetText( pTextCtl, text, RECORDER_FILE_NAME_LENGHT + 8);
	}

	if( result == 1)
	{
		*subState = 11;
		recorder_list_hide_prompt( subState);
	}
	else
	{
		*subState = 0;
	}
	repaint( TRUE);
}
#endif

static void recorder_list_delete_cb( Recorder* pme)
{

	IMenuCtl*	pMenu		= (IMenuCtl*)pme->m_ptr[0];
	int*		subState	= (int*)pme->m_ptr[1];
	int			result		= recorder_list_delete_all( pme, pMenu);

	if( result == 2 || result == 3)
	{
		*subState = 4;
	}
	else if( result == 0)
	{
		*subState = 12;
	}
	recorder_list_hide_prompt( subState);
	repaint( TRUE);
}

static boolean  dialog_handler_of_state_record_list( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam)
{

	static IMenuCtl* pMenu   		= 0;
	static IMenuCtl* pOption 		= 0;
	static IStatic*  pStatic		= 0;
	static ITextCtl* pTextCtl		= 0;
	// 1 option,
	// 2 delete confirm,
	// 3 delete all confirm,
	// 4 delete failed,
	// 5 deleting,
	// 6 setting as ringtone,
	// 7 set as ringtone failed,
	// 8 set is successful,
	// 9 record file number limit reached
	// 10 rename
	// 11 invalid new file name
	// 12 delete ok
	// 13 renaming
	// 14 prompt that the recorde list is empty
	static int   	 subState 		= 0;
	static uint16    selected		= 0;
	static uint16    selectedOption	= 0;
	static boolean	 reserve 		= 0;
#if defined( FEATURE_RECORDER_RENAME)
	static AECHAR text[RECORDER_FILE_NAME_LENGHT + 8] = { 0};
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
		if(evt == EVT_PEN_UP)
		{
			int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
			int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
			AEERect bottomBarRect;
			//int ht;
			int nBarH ;
			AEEDeviceInfo devinfo;
			nBarH = GetBottomBarHeight(pme->m_pDisplay);
			
			MEMSET(&devinfo, 0, sizeof(devinfo));
			ISHELL_GetDeviceInfo(pme->m_pShell, &devinfo);
			SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
			MSG_FATAL("wXPos=====%d,wYPos=========%d",wXPos,wYPos,0);
			
			if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect))
			{
				if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//右
				{						
					//evt = EVT_KEY;
					//wParam = AVK_CLR;
					boolean rt = ISHELL_PostEvent(pme->m_pShell,AEECLSID_RECORDER,EVT_USER,AVK_CLR,0);
						return rt;
				}
				else if((wXPos >= bottomBarRect.x) && (wXPos < bottomBarRect.x + (bottomBarRect.dx/3)))//左
				{						
					
					//evt = EVT_KEY;
					//wParam = AVK_SELECT;
					boolean rt = ISHELL_PostEvent(pme->m_pShell,AEECLSID_RECORDER,EVT_USER,AVK_SELECT,0);
						return rt;
				}
			}
		}
#endif

	switch (evt)
	{

		case EVT_DIALOG_INIT:
		{

			pMenu   	= (IMenuCtl*)IDIALOG_GetControl( pme->m_pActiveDialog, IDC_RECORD_LIST);
			pOption 	= (IMenuCtl*)IDIALOG_GetControl( pme->m_pActiveDialog, IDC_OPTION_MENU);
			pStatic 	= (IStatic*)IDIALOG_GetControl( pme->m_pActiveDialog, IDC_STATIC);
			pTextCtl   	= (ITextCtl*)IDIALOG_GetControl( pme->m_pActiveDialog, IDC_RECORD_LIST_TEXT);
			reserve		= 0;
			if( pme->m_ePreState == STATE_MAIN && !pme->m_bSuspended)
			{
				selected		= 0;
				selectedOption	= 0;
				subState 		= 0;
			}
		}
		return TRUE;

		case EVT_DIALOG_START:
		{

			ITEXTCTL_SetActive( pTextCtl, FALSE);

			recorder_list_create_menu( pme, pMenu, 0);
			
			IMENUCTL_SetOemProperties( pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE|MP_ACTIVE_NO_REDRAW);
			IMENUCTL_SetBottomBarType( pMenu, BTBAR_BACK);
            IMENUCTL_SetProperties( pOption, IMENUCTL_GetProperties( pMenu) | MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);

			if( pme->m_ePreState == STATE_MAIN && !pme->m_bSuspended && IMENUCTL_GetItemCount( pMenu) > 0)
			{
				selected = IMENUCTL_GetItemID( pMenu, 0);
			}
			IMENUCTL_SetSel( pMenu, selected);
#if defined( FEATURE_RECORDER_RENAME)
			if( subState == 10)
			{
				recorder_list_show_rename_screen( pme, pMenu, pTextCtl, text);
			}
#endif

			recorder_list_create_option_menu_if( pOption);
			repaint( TRUE);

			if( subState == 4  ||
				subState == 7  ||
				subState == 8  ||
				subState == 11 ||
				subState == 12
			)
			{
				recorder_list_hide_prompt( &subState);
			}

			IDIALOG_SetProperties( pme->m_pActiveDialog, IDIALOG_GetProperties( pme->m_pActiveDialog)|DLG_NOT_SET_FOCUS_AUTO);
		}
		return TRUE;

		case EVT_DIALOG_END:
		{

			ISHELL_CancelTimer( pme->m_pShell, 0, &subState);
			recorder_list_free_menu_data_if( pme, pMenu);

			if( pme->m_bSuspended)
			{
				MEMSET( text, 0, sizeof( text));
				ITEXTCTL_GetText( pTextCtl, text, ARRAY_SIZE( text));
			}
			else
			{
				subState = 0;
				if( !reserve)
				{
					selected 		= 0;
					selectedOption	= 0;
				}
			}

		}
		return TRUE;

		case EVT_USER_REDRAW:
		{
			#if 0
			IMENUCTL_SetTitle( pMenu, AEE_RECORDER_RES_FILE, IDS_RECORD_LIST_TITLE, 0);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pme->m_pShell,
                        AEE_RECORDER_RES_FILE,                                
                        IDS_RECORD_LIST_TITLE,
                        WTitle,
                        sizeof(WTitle));
                if(pme->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
                }
		    }
			#endif
			if( IMENUCTL_GetItemCount( pMenu) == 0)
			{
				IMENUCTL_SetBottomBarType( pMenu, BTBAR_BACK);
			}
#if defined( FEATURE_RECORDER_RENAME)
			else if( subState == 10)
			{
				#ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
				if( WSTRLEN( ITEXTCTL_GetTextPtr( pTextCtl)) == 0)
				{
					 IMENUCTL_SetBottomBarType( pMenu, BTBAR_BACK);
				}
				else
				{
					IMENUCTL_SetBottomBarType( pMenu, BTBAR_SAVE_DELETE);
				}
				#else
					IMENUCTL_SetBottomBarType( pMenu, BTBAR_BACK);
				#endif
			}
#endif
			else
			{
				IMENUCTL_SetBottomBarType( pMenu, BTBAR_OPTION_BACK);
			}

			if( subState == 0)
			{
				IMENUCTL_SetSel( pMenu, selected);
				IMENUCTL_SetActive( pMenu, TRUE);
				IMENUCTL_SetActive( pOption, FALSE);
				IMENUCTL_Redraw( pMenu);

				if( IMENUCTL_GetItemCount( pMenu) == 0)
				{
					AEERect rect = pme->m_rc;
					rect.y += GetTitleBarHeight( pme->m_pDisplay);
					drawText5( pme,
							FMRADIO_RES_FILE,
							IDS_FMRADIO_CHANNEL_LIST_IS_EMPTY,
							&rect,
							0
						  );
				}
			}
			else if( subState == 1)
			{
				IMENUCTL_SetSel( pOption, selectedOption);
				IMENUCTL_SetActive( pMenu, FALSE);
				IMENUCTL_SetActive( pOption, TRUE);
				IMENUCTL_Redraw( pMenu);
				IMENUCTL_SetPopMenuRect( pOption);
				IMENUCTL_Redraw( pOption);
			}
			else
			{
				IMENUCTL_SetSel( pMenu, selected);
				IMENUCTL_SetActive( pMenu, FALSE);
				IMENUCTL_SetActive( pOption, FALSE);
				IMENUCTL_Redraw( pMenu);

				if( subState == 2 || subState == 3)
				{
					drawModalDialog( pme->m_pDisplay, pStatic, IDS_RECORD_DELETE_CONFIRM + subState - 2, TRUE);
				}
				else if( subState == 4)
				{
					uint16 resId = IDS_RECORD_OPEN_FILE_ERROR_13;
					if( pme->m_nLastOperationError >= FILE_ERROR &&
						pme->m_nLastOperationError <= ENOMEDIA
					)
					{
						resId = IDS_RECORD_OPEN_FILE_ERROR_1 + pme->m_nLastOperationError - FILE_ERROR;
					}
					drawModalDialog( pme->m_pDisplay, pStatic, resId, FALSE);
				}
				else if( subState == 5 || subState == 6 || subState == 13)
				{
#if 0
					drawBgMask( pme, 1);
					drawText3( pme, subState == 5 ? IDS_RECORD_DELETING : IDS_RECORD_SET_AS_RINGER_ING);
#else
					drawWaitingDialog( pme->m_pDisplay, pStatic, subState == 5 ? IDS_RECORD_DELETING : IDS_RECORD_SET_AS_RINGER_ING);
#endif
				}
				else if( subState == 7 || subState == 8 || subState == 9)
				{
					uint16 resId[] = { IDS_RECORD_SET_AS_RINGTONE_FAILED,
									IDS_RECORDER_DONE,
									IDS_RECORD_REACH_FILE_NUMBER_LIMIT
								};
					drawModalDialog( pme->m_pDisplay, pStatic, resId[subState - 7], FALSE);
				}
#if defined( FEATURE_RECORDER_RENAME)
				else if( subState == 10)
				{
					ITEXTCTL_Redraw( pTextCtl);
				}
				else if( subState == 11)
				{
					drawModalDialog( pme->m_pDisplay, pStatic, IDS_RECORD_LIST_RENAME_INVALID_FILE_NAME, FALSE);
				}
#endif
				else if( subState == 12)
				{
					drawModalDialog( pme->m_pDisplay, pStatic, IDS_RECORDER_DONE, FALSE);
				}
			}
		}
		return TRUE;

		case EVT_KEY:
		{

			switch( wParam)
			{
#if defined( AEE_SIMULATOR)
				case AVK_SOFT1:
					if( subState == 0 && IMENUCTL_GetItemCount( pMenu) > 0)
					{
						recorder_list_create_option_menu_if( pOption);
						subState = 1;
						repaint( TRUE);
						break;
					}
#else
				case AVK_SELECT:
#endif
				{
					int result = -1;
					MSG_FATAL("AVK_SELECT...............",0,0,0);
					if( ( subState >= 4 && subState <= 9) ||
						subState == 11 ||
						subState == 12 ||
						subState == 13 ||
						IMENUCTL_GetItemCount( pMenu) == 0
					)
					{
						break;
					}
__dialog_handler_of_state_record_list_confirm_delete__:
					pme->m_bAppIsReady = FALSE;
					if( subState == 2)
					{
						result = recorder_list_delete_if( pme, pMenu);
					}
					else if( subState == 3)
					{
						subState = 5;
						repaint( FALSE);

						pme->m_ptr[0] = (int)pMenu;
						pme->m_ptr[1] = (int)&subState;
						CALLBACK_Init( &pme->m_cb, (PFNNOTIFY)recorder_list_delete_cb, pme);
						ISHELL_Resume( pme->m_pShell, &pme->m_cb);
						break;
					}
#if defined( FEATURE_RECORDER_RENAME)
					else if( subState == 10)
					{
__dialog_handler_of_state_record_list_rename_save__:
						if( WSTRLEN( ITEXTCTL_GetTextPtr( pTextCtl)) == 0)
						{
							break;
						}
						else
						{
							ITEXTCTL_SetActive( pTextCtl, FALSE);
							subState = 13;
							repaint( FALSE);

							pme->m_ptr[0]	= (int)pTextCtl;
							pme->m_ptr[1]	= (int)&selected;
							pme->m_ptr[2]	= (int)&subState;
							pme->m_ptr[3]   = (int)text;
							pme->m_ptr[4]   = (int)pMenu;
							CALLBACK_Init( &pme->m_cb, (PFNNOTIFY)recorder_list_rename_cb, pme);
							ISHELL_Resume( pme->m_pShell, &pme->m_cb);
							break;
						}
					}
#endif
					if( result == 2 || result == 3)
					{
						subState = 4;
					}
					else if( result == 0)
					{
						subState = 12;
						recorder_list_hide_prompt( &subState);
					}
					else
					{
						subState = 0;
					}
					repaint( TRUE);
				}
				break;

				case AVK_SOFT2:
				case AVK_CLR:
				{

					if( subState == 5 || subState == 6 || subState == 13)
					{
						break;
					}
#if defined( FEATURE_RECORDER_RENAME)
					if( subState == 11)
					{
						recorder_list_show_rename_screen( pme, pMenu, pTextCtl, text);
						subState = 10;
						repaint( TRUE);
					}
#endif
					else
					{
						if( subState == 0)
						{
							CLOSE_DIALOG( DLGRET_CANCELED);
						}
#if defined( FEATURE_RECORDER_RENAME)
						else if( subState == 10)
						{
							ITEXTCTL_SetActive( pTextCtl, FALSE);
							repaint( TRUE);
						}
#endif
						else
						{
							if( subState == 4 ||
								subState == 7 ||
								subState == 8 ||
								subState == 9 ||
								subState == 11 ||
								subState == 12 ||
								subState == 14
							)
							{
								ISHELL_CancelTimer( pme->m_pShell, (PFNNOTIFY)recorder_list_prompt_timer_cb, &subState);
							}
							repaint( TRUE);
						}
						subState = 0;
					}

				}
				break;

				case AVK_INFO:
				{
					MSG_FATAL("AVK_INFO....................",0,0,0);
					if( subState == 0 && IMENUCTL_GetItemCount( pMenu) > 0)
					{
						reserve = TRUE;
						recorder_list_play( pme, pMenu);
					}
					else if( subState == 2 || subState == 3)
					{
						goto __dialog_handler_of_state_record_list_confirm_delete__;
					}
#if defined( FEATURE_RECORDER_RENAME)					
					else if( subState == 10)
					{
						goto __dialog_handler_of_state_record_list_rename_save__;
					}	
#endif					
				}
				break;
			}
		}
		return TRUE;

#if defined( FEATURE_RECORDER_RENAME)
		case EVT_KEY_RELEASE:
		{
			if( subState == 10)
			{
				AEERect rect = pme->m_rc;
				rect.dy = GetBottomBarHeight( pme->m_pDisplay);
				rect.y  = pme->m_rc.dy - rect.dy;
				IDISPLAY_SetClipRect( pme->m_pDisplay, &rect);
				#ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
				if( WSTRLEN( ITEXTCTL_GetTextPtr( pTextCtl)) == 0)
				{
					 drawBottomBar( BTBAR_BACK);
				}
				else
				{
					drawBottomBar( BTBAR_SAVE_DELETE);
				}
				#else
					drawBottomBar( BTBAR_BACK);
				#endif
				IDISPLAY_Update( pme->m_pDisplay);
				IDISPLAY_SetClipRect( pme->m_pDisplay, &pme->m_rc);
			}
		}
		return TRUE;
#endif

		case EVT_COMMAND:
		{
			MSG_FATAL("subState===%d",subState,0,0);
			if( subState == 1)
			{

				switch( wParam)
				{
					case IDS_RECORD_LIST_OPTION_PLAY:
					case IDS_RECORD_LIST_OPTION_SET_AS:
					{
						reserve = TRUE;
						if( wParam == IDS_RECORD_LIST_OPTION_PLAY)
						{
							recorder_list_play( pme, pMenu);
						}
						else if( wParam == IDS_RECORD_LIST_OPTION_SET_AS)
						{
							recorder_list_set_as( pme, pMenu);
						}
					}
					break;

					case IDS_RECORD_LIST_OPTION_SET_AS_RINGTONE:
					{
#if defined( AEE_SIMULATOR)
						subState = 8;
						repaint( TRUE);
#else
						subState = 6;
						repaint( FALSE);
						{
							pme->m_ptr[0]    	= (int)pMenu;
							pme->m_ptr[1]		= (int)&subState;
							CALLBACK_Init( &pme->m_cb, (PFNNOTIFY)recorder_list_set_as_ringer_cb, pme);
							ISHELL_Resume( pme->m_pShell, &pme->m_cb);
						}
#endif
					}
					break;

					case IDS_RECORD_LIST_OPTION_DELETE:
					case IDS_RECORD_LIST_OPTION_DELETE_ALL:
					case IDS_RECORD_LIST_OPTION_RENAME:
					{
						pme->m_bAppIsReady 	= FALSE;
						subState 			= wParam -  IDS_RECORD_LIST_OPTION_DELETE + 2;

						if( wParam == IDS_RECORD_LIST_OPTION_RENAME)
						{
							subState = 10;
							text[0] = 0;
							text[1] = 0xffff;
							recorder_list_strip_path_name_from_menu( pme, pMenu);
							recorder_list_show_rename_screen( pme, pMenu, pTextCtl, text);
						}
						repaint( TRUE);
					}
					break;

#if defined( FEATURE_RECODER_TEST)
					case 5000:
					{
						pme->m_bAppIsReady = FALSE;
						recorder_clear_all_memos( pme);
						IMENUCTL_DeleteAll( pMenu);
						subState = 0;
						repaint( TRUE);
						pme->m_bAppIsReady = TRUE;
					}
					break;
#endif
				}
			}
			else if( subState == 0 && IMENUCTL_GetItemCount( pMenu) > 0)
			{

#if defined( AEE_SIMULATOR)
				recorder_list_play( pme, pMenu);
				subState = 0;
#else
				recorder_list_create_option_menu_if( pOption);
				subState = 1;
				repaint( TRUE);
#endif
			}
			#ifdef FEATURE_VERSION_X3
			else if(subState == 10)
			{
				MSG_FATAL("command...............",0,0,0);
				goto __dialog_handler_of_state_record_list_rename_save__;
			}
			#endif
		}
		return TRUE;


		case EVT_CTL_SEL_CHANGED:
		{

			if( !pme->m_bSuspended)
			{
				if( subState == 0)
				{
					selected = IMENUCTL_GetSel( pMenu);
				}
				else if( subState == 1)
				{
					selectedOption = IMENUCTL_GetSel( pOption);
				}
			}
		}
		return TRUE;
	}

	return FALSE;
} // dialog_handler_of_state_record_list

#if defined( FEATURE_RECORDER_SET_AS)

#if !defined( AEE_SIMULATOR)
static void recorder_set_as_prompt_timer_cb( Recorder* pme)
{
	int* subState	= (int*)pme->m_ptr[1];

	*subState = 0;
	CLOSE_DIALOG(DLGRET_CANCELED);
}

static void recorder_set_as_cb( Recorder* pme)
{

	IMenuCtl* 	pMenu 		= (IMenuCtl*)pme->m_ptr[0];
	// 1  setting,
	// 2  set as successful,
	// 4  set as call ringtone failed,
	// 8  set as alarm ringtone failed,
	// 16 set as sms ringtone faile
	int*		subState	= (int*)pme->m_ptr[1];

	MSG_FATAL(";-----------------------------------------------",0,0,0);
	MSG_FATAL( ";recorder_set_as_cb",0,0,0);

	//if( IMENUCTL_EnumSelInit( pMenu))
	{
		
		uint16 	id 		= 0;
		int		result 	= 0;
		MSG_FATAL("IMENUCTL_EnumSelInit..........................",0,0,0);
		//if(( id = IMENUCTL_GetSel( pMenu)) != (uint16)-1)
		{
			MSG_FATAL("IMENUCTL_EnumSelInit......................:%d",id,0,0);
			if(GetCheckBoxVal(pMenu, IDS_SET_AS_RINGER)/*id == IDS_SET_AS_RINGER*/)
			{
				MSG_FATAL("IDS_SET_AS_RINGER..........................",0,0,0);
				if( recorder_list_set_as_call_ringtone( pme))
				{
					result |= 2;
				}
				else
				{
					result |= 4;
				}
				//IMENUCTL_SetFocus(pMenu, IDS_SET_AS_RINGER);
			}
			else
			{
				ringID config[PROFILENUMBER] 	= { 0};
			    byte profile					= 0;
			    if( OEM_GetConfig( CFGI_PROFILE_CUR_NUMBER, &profile, sizeof( byte)) != SUCCESS)
			    {
			        MSG_FATAL( ";get profile setting failed",0,0,0);
			        return ;
			    }

			    if( OEM_GetConfig( CFGI_PROFILE_CALL_RINGER, (void*)config,sizeof( config)) != SUCCESS)
			    {
			        MSG_FATAL( ";get config failed",0,0,0);
			        return ;
			    }
			    //STRCPY(config[profile].szMusicname,pme->m_FileName);
			    config[profile].ringType = OEMNV_MID_RINGER;

			    //config[profile].midID = id;

			    if( OEM_SetConfig( CFGI_PROFILE_CALL_RINGER, (void*)config,sizeof( config)) != SUCCESS)
			    {
			        MSG_FATAL( ";set config failed",0,0,0);
			        return ;
			    }
			}
			 if( GetCheckBoxVal(pMenu, IDS_SET_AS_ALARM_ALERT)/*id == IDS_SET_AS_ALARM_ALERT*/)
			{
				MSG_FATAL("IDS_SET_AS_ALARM_ALERT..........................",0,0,0);
				if( recorder_list_set_as_alarm_ringtone( pme))
				{
					result |= 2;
				}
				else
				{
					result |= 8;
				}
				//IMENUCTL_SetFocus(pMenu, IDS_SET_AS_ALARM_ALERT);
			}
			 else
			 {
			 	ringID config[PROFILENUMBER] 	= { 0};
			    byte profile					= 0;
			    
			    if( OEM_GetConfig( CFGI_PROFILE_CUR_NUMBER, &profile, sizeof( byte)) != SUCCESS)
			    {
			        MSG_FATAL( ";get profile setting failed",0,0,0);
			        return ;
			    }

			    if( OEM_GetConfig( CFGI_PROFILE_ALARM_RINGER, (void*)config,sizeof( config)) != SUCCESS)
			    {
			        MSG_FATAL( ";get config failed",0,0,0);
			        return ;
			    }
			    //STRCPY(config[profile].szMusicname,pme->m_FileName);
			    config[profile].ringType = OEMNV_MID_RINGER;

			    //config[profile].midID = id;

			    if( OEM_SetConfig( CFGI_PROFILE_ALARM_RINGER, (void*)config,sizeof( config)) != SUCCESS)
			    {
			        MSG_FATAL( ";set config failed",0,0,0);
			        return ;
			    }
			 }
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifdef FEATURE_SMSTONETYPE_MID
			if( GetCheckBoxVal(pMenu, IDS_SET_AS_SMS_ALERT)/*id == IDS_SET_AS_SMS_ALERT*/)
			{
				if( recorder_list_set_as_sms_ringtone( pme))
				{
					result |= 2;
				}
				else
				{
					result |= 16;
				}
			}
			else
			{
				ringID config[PROFILENUMBER] 	= { 0};
			    byte profile					= 0;
			    
			    if( OEM_GetConfig( CFGI_PROFILE_CUR_NUMBER, &profile, sizeof( byte)) != SUCCESS)
			    {
			        MSG_FATAL( ";get profile setting failed",0,0,0);
			        return ;
			    }

			    if( OEM_GetConfig( CFGI_PROFILE_SMS_RINGER_ID, (void*)config,sizeof( config)) != SUCCESS)
			    {
			        MSG_FATAL( ";get config failed",0,0,0);
			        return ;
			    }
			    //STRCPY(config[profile].szMusicname,pme->m_FileName);
			    config[profile].ringType = OEMNV_MID_RINGER;

			    //config[profile].midID = id;

			    if( OEM_SetConfig( CFGI_PROFILE_SMS_RINGER_ID, (void*)config,sizeof( config)) != SUCCESS)
			    {
			        MSG_FATAL( ";set config failed",0,0,0);
			        return ;
			    }
			}
#endif
#endif
		}

		MSG_FATAL( ";-----result = %d", result,0,0);
		*subState = result;

		if( result > 0)
		{
			ISHELL_SetTimer( pme->m_pShell, 2000, (PFNNOTIFY)recorder_set_as_prompt_timer_cb, pme);
			repaint( TRUE);
			return;
		}
	}

	*subState = 0;
	CLOSE_DIALOG(DLGRET_CANCELED);
}
#endif

static boolean  dialog_handler_of_state_set_as( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam)
{

	static IMenuCtl* 	pMenu 	 = 0;
	static IStatic*		pStatic  = 0;
	// 1  setting,
	// 2  set as successful,
	// 4  set as call ringtone failed,
	// 8  set as alarm ringtone failed,
	// 16 set as sms ringtone faile
	static int			subState = 0;
	static uint16    	selected = 0;
	
#ifdef FEATURE_LCD_TOUCH_ENABLE
	if(evt == EVT_PEN_UP)
	{
		int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
		int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
		AEERect bottomBarRect;
		//int ht;
		int nBarH ;
		AEEDeviceInfo devinfo;
		nBarH = GetBottomBarHeight(pme->m_pDisplay);
		
		MEMSET(&devinfo, 0, sizeof(devinfo));
		ISHELL_GetDeviceInfo(pme->m_pShell, &devinfo);
		SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
		MSG_FATAL("wXPos=====%d,wYPos=========%d",wXPos,wYPos,0);
		
		if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect))
		{
			if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//右
			{		
				boolean rt = ISHELL_PostEvent(pme->m_pShell,AEECLSID_RECORDER,EVT_USER,AVK_CLR,0);
						return rt;
				//evt = EVT_KEY;
				//wParam = AVK_CLR;
			}
			else if((wXPos >= bottomBarRect.x) && (wXPos < bottomBarRect.x + (bottomBarRect.dx/3)))//左
			{						

				boolean rt = ISHELL_PostEvent(pme->m_pShell,AEECLSID_RECORDER,EVT_USER,AVK_SELECT,0);
						return rt;
				//evt = EVT_KEY;
				//wParam = AVK_SELECT;
				MSG_FATAL("AVK_SELECT",0,0,0);
			}
		}
	}
#endif

	switch (evt)
	{

		case EVT_DIALOG_INIT:
		{
			pMenu   = (IMenuCtl*)IDIALOG_GetControl( pme->m_pActiveDialog, IDC_SET_AS_MENU);
			pStatic = (IStatic*)IDIALOG_GetControl( pme->m_pActiveDialog, IDC_RECORDER_SET_AS_STATIC);
		}
		return TRUE;

		case EVT_DIALOG_START:
		{

			static uint16 resId[]  = {
						IDS_SET_AS_RINGER,
						IDS_SET_AS_ALARM_ALERT
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifdef FEATURE_SMSTONETYPE_MID
						,IDS_SET_AS_SMS_ALERT
#endif
#endif
					};
			int i 		= 0;
			int size 	= ARRAY_SIZE( resId);
			for( i = 0; i < size; i ++)
			{
				IMENUCTL_AddItem( pMenu, AEE_RECORDER_RES_FILE, resId[i], resId[i], 0, 0);
			}

			IMENUCTL_SetProperties( pMenu, MP_MULTI_SEL | MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
			IMENUCTL_SetOemProperties( pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
			#if 0
			IMENUCTL_SetTitle( pMenu, AEE_RECORDER_RES_FILE, IDS_RECORD_LIST_OPTION_SET_AS, 0);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pme->m_pShell,
                        AEE_RECORDER_RES_FILE,                                
                        IDS_RECORD_LIST_OPTION_SET_AS,
                        WTitle,
                        sizeof(WTitle));
                if(pme->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
                }
		    }
			#endif			
			
			IMENUCTL_SetBottomBarType( pMenu, BTBAR_SAVE_BACK);
			pme->m_ptr[0] = (int)pMenu;
			pme->m_ptr[1] = (int)&subState;
			repaint( TRUE);

			if( subState >= 2)
			{
#ifndef WIN32
				ISHELL_SetTimer( pme->m_pShell, 2000, (PFNNOTIFY)recorder_set_as_prompt_timer_cb, pme);
#endif//WIN32
			}

			IDIALOG_SetProperties( pme->m_pActiveDialog, IDIALOG_GetProperties( pme->m_pActiveDialog)|DLG_NOT_SET_FOCUS_AUTO);
		}
		return TRUE;

		case EVT_DIALOG_END:
		{

			ISHELL_CancelTimer( pme->m_pShell, 0, pme);
			if( !pme->m_bSuspended)
			{
				subState = 0;
				selected = 0;
			}
		}
		return TRUE;

		case EVT_USER_REDRAW:
		{
			byte CurProfileNum;
            ringID ConfigRinger[PROFILENUMBER];
			ringID ConfigArmRinger[PROFILENUMBER];
			ringID ConfigSmsRinger[PROFILENUMBER];
            (void) OEM_GetConfig(
                            CFGI_PROFILE_CUR_NUMBER,
                            &CurProfileNum,
                            sizeof(CurProfileNum));
            // 检查是否为来电铃声
            OEM_GetConfig(CFGI_PROFILE_CALL_RINGER,
                          (void*)ConfigRinger,
                          sizeof(ConfigRinger));			
			
			DBGPRINTF("***zzg m_FileName=%s, szMusicname=%s, ringType=%x***", pme->m_FileName,ConfigRinger[CurProfileNum].szMusicname, ConfigRinger[CurProfileNum].ringType);

			
            if(((STRCMP(pme->m_FileName,ConfigRinger[CurProfileNum].szMusicname) == 0) && 
                    (ConfigRinger[CurProfileNum].ringType == OEMNV_MP3_RINGER)))
            {
            	SetCheckBoxItem(pMenu, IDS_SET_AS_RINGER, TRUE);
            }
						
			
            // 检查是否为来闹钟铃声
            OEM_GetConfig(CFGI_PROFILE_ALARM_RINGER,
                          (void*)ConfigArmRinger,
                          sizeof(ConfigArmRinger));

			DBGPRINTF("***zzg m_FileName=%s, szMusicname=%s, ringType=%x***", pme->m_FileName,ConfigArmRinger[CurProfileNum].szMusicname, ConfigArmRinger[CurProfileNum].ringType);
			
            if(((STRCMP(pme->m_FileName,ConfigArmRinger[CurProfileNum].szMusicname) == 0) && 
                    (ConfigArmRinger[CurProfileNum].ringType == OEMNV_MP3_RINGER)))
            {
            	SetCheckBoxItem(pMenu, IDS_SET_AS_ALARM_ALERT, TRUE);       	     
            }
			
			IMENUCTL_SetSel( pMenu, selected);
			IMENUCTL_SetActive( pMenu, subState == 0);
			IMENUCTL_Redraw( pMenu);
			if( subState == 1)
			{
#if 0
				drawBgMask( pme, 1);
				drawText3( pme, IDS_RECORD_SET_AS_RINGER_ING);
#else
				drawWaitingDialog( pme->m_pDisplay, pStatic, IDS_RECORD_SET_AS_RINGER_ING);
#endif
			}
			else if( subState == 2)
			{
				drawModalDialog( pme->m_pDisplay, pStatic, IDS_RECORDER_DONE, FALSE);
			}
			else if( subState > 2)
			{
				AECHAR 	text[128] 	= {0};
				int		len			= 0;
				int		i			= 0;
				uint16  resId[]		= {
										IDS_RECORD_SET_AS_RINGER_FAILED,
										IDS_SET_AS_RINGER,
										IDS_SET_AS_ALARM_ALERT,
										IDS_SET_AS_SMS_ALERT
									};

				len = ISHELL_LoadResString( AEE_GetShell(),
								   AEE_RECORDER_RES_FILE,
								   resId[0],
								   text,
								   256
							   );
				text[len ++] = '\n';

				for( i = 1; i < 4; i ++)
				{
					if( subState & ( 2 << i))
					{
						len += ISHELL_LoadResString( AEE_GetShell(),
										   AEE_RECORDER_RES_FILE,
										   resId[i],
										   text + len,
										   256
									   );
						text[len ++] = ' ';
						text[len ++] = ',';
					}
				}
				len -= 2;
				text[len] = 0;
				drawModalDialog1( pme->m_pDisplay, pStatic, text, FALSE);
			}


			IDISPLAY_UpdateEx( pme->m_pDisplay, FALSE);
		}
		return TRUE;

		case EVT_KEY:
		{
			switch( wParam)
			{
				case AVK_SOFT2:
				case AVK_CLR:
				{
					if( subState != 1)
					{
#ifndef WIN32
						ISHELL_CancelTimer( pme->m_pShell, (PFNNOTIFY)recorder_set_as_prompt_timer_cb, pme);
#endif//WIN32
						subState = 0;
						CLOSE_DIALOG(DLGRET_CANCELED);
					}
				}
				break;
				#if defined( AEE_SIMULATOR)
				case AVK_SOFT1:
#else
				//case AVK_INFO:
				case AVK_SELECT:
#endif
				{			
					MSG_FATAL("subState======%d",subState,0,0);
					if( subState != 0)
					{
						break;
					}

					//Add By zzg 2012_03_15
					if(!GetCheckBoxVal(pMenu, IDS_SET_AS_RINGER)/*id == IDS_SET_AS_RINGER*/)
					{							
						ringID config[PROFILENUMBER] 	= { 0};
					    byte profile					= 0;
						
					    if( OEM_GetConfig( CFGI_PROFILE_CUR_NUMBER, &profile, sizeof( byte)) != SUCCESS)
					    {
					        MSG_FATAL( ";get profile setting failed",0,0,0);					        
					    }

					    if( OEM_GetConfig( CFGI_PROFILE_CALL_RINGER, (void*)config,sizeof( config)) != SUCCESS)
					    {
					        MSG_FATAL( ";get config failed",0,0,0);					        
					    }
						
					    //STRCPY(config[profile].szMusicname,pme->m_FileName);
					    config[profile].ringType = OEMNV_MID_RINGER;

					    //config[profile].midID = id;

					    if( OEM_SetConfig( CFGI_PROFILE_CALL_RINGER, (void*)config,sizeof( config)) != SUCCESS)
					    {
					        MSG_FATAL( ";set config failed",0,0,0);					        
					    }
																
					}
					
					if(!GetCheckBoxVal(pMenu, IDS_SET_AS_ALARM_ALERT)/*id == IDS_SET_AS_ALARM_ALERT*/)
					{
						ringID config[PROFILENUMBER] 	= { 0};
					    byte profile					= 0;
					    
					    if( OEM_GetConfig( CFGI_PROFILE_CUR_NUMBER, &profile, sizeof( byte)) != SUCCESS)
					    {
					        MSG_FATAL( ";get profile setting failed",0,0,0);					        
					    }

					    if( OEM_GetConfig( CFGI_PROFILE_ALARM_RINGER, (void*)config,sizeof( config)) != SUCCESS)
					    {
					        MSG_FATAL( ";get config failed",0,0,0);					        
					    }
					    //STRCPY(config[profile].szMusicname,pme->m_FileName);
					    config[profile].ringType = OEMNV_MID_RINGER;

					    //config[profile].midID = id;

					    if( OEM_SetConfig( CFGI_PROFILE_ALARM_RINGER, (void*)config,sizeof( config)) != SUCCESS)
					    {
					        MSG_FATAL( ";set config failed",0,0,0);					        
					    }													
					}					
					//Add End

					subState = 1;
					repaint( FALSE);
#if defined( AEE_SIMULATOR)
					subState = 0;
					CLOSE_DIALOG(DLGRET_CANCELED);
#else
					{
						MSG_FATAL("dialog_handler_of_state_set_as..........................",0,0,0);
						CALLBACK_Init( &pme->m_cb, (PFNNOTIFY)recorder_set_as_cb, pme);
						ISHELL_Resume( pme->m_pShell, &pme->m_cb);
					}

#endif
				}
				break;
			}
		}
		return TRUE;
		case EVT_COMMAND:
			{
					MSG_FATAL("subState======%d",subState,0,0);
					if( subState != 0)
					{
						break;
					}

					subState = 1;
					repaint( FALSE);
#if defined( AEE_SIMULATOR)
					subState = 0;
					CLOSE_DIALOG(DLGRET_CANCELED);
#else
					{
						MSG_FATAL("dialog_handler_of_state_set_as..........................",0,0,0);
						CALLBACK_Init( &pme->m_cb, (PFNNOTIFY)recorder_set_as_cb, pme);
						ISHELL_Resume( pme->m_pShell, &pme->m_cb);
					}

#endif
				}
				break;
		case EVT_CTL_SEL_CHANGED:
		{
			if( subState == 0)
			{
				selected = IMENUCTL_GetSel( pMenu);
			}
		}
		return TRUE;
	}

	return FALSE;
} // dialog_handler_of_state_set_as
#endif


int  Recorder_ShowMsgBoxDialog( Recorder *pMe, 
                               uint16      msgResId,
                               uint16      dlgResId,                               
                               AECHAR     *msg, 
                               int         msglen)
{
     IStatic* pIStatic = NULL;
     AECHAR szText[MSGBOX_MAXTEXTLEN];
     PromptMsg_Param_type MsgParam={0};

     if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC,
                                         (void **)&pIStatic))
     {        
        return FALSE;
     }

     /*Set the static control text based on the user settings of the
      * m_nMsgBoxTextId parameter.*/
     if(msgResId == IDS_RECORD_OPEN_FILE_ERROR_12)
     {
         ISHELL_LoadResString(pMe->m_pShell,
               AEE_RECORDER_RES_FILE,
               msgResId,
               szText,
               sizeof(szText));     
     }
     else
     {
         ISHELL_LoadResString(pMe->m_pShell,
               AEE_APPSCOMMONRES_LANGFILE,
               msgResId,
               szText,
               sizeof(szText));
     }
     MsgParam.ePMsgType = MESSAGE_CONFIRM;
     MsgParam.pwszMsg = szText;
     if((msgResId == IDS_RECORD_OPEN_FILE_ERROR_12))
     {
        MsgParam.eBBarType = BTBAR_BACK;
     }
     else
     {
        MsgParam.eBBarType = BTBAR_OK_CANCEL;
     }
     /*we do not special ISTATIC control, because of DIALOG will redraw
      * ISTATIC control, and the text color is the default, it may be the
      * same as background, but DrawPromptMessage function will change the
      * text color.*/
     DrawPromptMessage(pMe->m_pDisplay, pIStatic, &MsgParam);
     if(pIStatic)
     ISTATIC_Release(pIStatic);
     IDISPLAY_Update(pMe->m_pDisplay);

     return TRUE;
}


static boolean dialog_handler_of_state_play_msg( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
	int media_scheduler;
    MSG_FATAL("dialog_handler_of_state_play_msg evt=0x%x,wParam=0x%x,dwParam=0x%x",evt,wParam,dwParam);   
#ifdef FEATURE_LCD_TOUCH_ENABLE    
    if(evt == EVT_POINTER_DOWN)
	{
		AEERect bottomBarRect;
		//int ht;
		int nBarH ;
		AEEDeviceInfo devinfo;
        int16 wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
        int16 wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);
        MSG_FATAL("dialog_handler_of_state_play_msg,wXPos=%d,wYPos=%d",wXPos,wYPos,0);        
		nBarH = GetBottomBarHeight(pme->m_pDisplay);
		
		MEMSET(&devinfo, 0, sizeof(devinfo));
		ISHELL_GetDeviceInfo(pme->m_pShell, &devinfo);
		SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
		
		
		if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect))
		{
            MSG_FATAL("TOUCH_PT_IN_RECT bottomBarRect",0,0,0);
			if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//右
			{				
                MSG_FATAL("TOUCH_PT_IN_RECT bottomBarRect 1",0,0,0);
				evt = EVT_KEY_PRESS;
				wParam = AVK_CLR;
			}
			else if((wXPos >= bottomBarRect.x) && (wXPos < bottomBarRect.x + (bottomBarRect.dx/3)))//左
			{						
				MSG_FATAL("TOUCH_PT_IN_RECT bottomBarRect 2",0,0,0);
				evt = EVT_KEY_PRESS;
				wParam = AVK_SELECT;
			}
		}
    }
#endif
    media_scheduler = app_media_scheduler();
	switch (evt)
	{
		
	    case EVT_DIALOG_START:
		{
          repaint( TRUE);
	    }
        return TRUE;
        
        case EVT_USER_REDRAW:
		{
            int hasCard = IFILEMGR_Test(pme->m_pFileManager, AEEFS_CARD0_DIR);
            if(APP_MEDIA_IMPACT_BY_MP3 == app_media_scheduler())
            {
                Recorder_ShowMsgBoxDialog( pme, 
                                        IDS_CONFIRM_OFF_MP,
                                        0,                                      
                                        NULL,
                                        -1);    
            }
            else if(APP_MEDIA_IMPACT_BY_FM == app_media_scheduler())
            {
                Recorder_ShowMsgBoxDialog( pme, 
                                        IDS_CONFIRM_OFF_FM,
                                        0,                                      
                                        NULL,
                                        -1);      
            }
            else if(hasCard != SUCCESS)
            {
                Recorder_ShowMsgBoxDialog( pme, 
                                        IDS_RECORD_OPEN_FILE_ERROR_12,
                                        0,                                      
                                        NULL,
                                        -1);                  
            }
            else
            {
                MSG_FATAL("Recorder state is ERROR for play!", 0, 0, 0);
            }
        }
        return TRUE;    
        
		case EVT_KEY_PRESS:
		{
			switch( wParam)
			{
#if defined( AEE_SIMULATOR)
				case AVK_SOFT1:
#else
				case AVK_INFO:
				case AVK_SELECT:
#endif
                {
                    if(APP_MEDIA_IMPACT_BY_MP3 == media_scheduler)
                    {
                        ISHELL_SendEvent(pme->m_pShell,
                                     AEECLSID_APP_MUSICPLAYER,
                                     EVT_CLOSEAPP,
                                     0,
                                     0);                      
                    }
                    else if(APP_MEDIA_IMPACT_BY_FM == media_scheduler)
                    {
                        ISHELL_SendEvent(pme->m_pShell,
                                     AEECLSID_APP_FMRADIO,
                                     EVT_CLOSEAPP,
                                     0,
                                     0);                         
                    }
                    else
                    {
                        MSG_ERROR("Recorder state2 is ERROR for play!", 0, 0, 0);
                        return TRUE;
                    }
                    CLOSE_DIALOG(DLGRET_OK);

                }
                break;

                case AVK_SOFT2:
			    case AVK_CLR:
				{
                    //MOVE_TO_STATE(STATE_RECORD_LIST);
                    MOVE_TO_STATE(STATE_MAIN);
                    CLOSE_DIALOG(DLGRET_CREATE);
                }   
                break;               
			}
		}
		return TRUE;

	}

	return TRUE;
}
static IFile* recorder_open_file( Media* pme, OpenFileMode openFileMode)
{

	IFile* pFile = IFILEMGR_OpenFile( pme->m_pFileManager, pme->m_FileName, openFileMode);
	if( !pFile)
	{
		char* error[] = { "File exists",
					"File does not exist",
					"Directory not empty",
					"Bad file name",
					"Bad seek position",
					"End of file",
					"File system full",
					"File already open",
					"No INODES (file slots) available",
					"Directory does not exist",
					"Invalid operation",
					"Storage media not present"
				};

		pme->m_nLastOperationError = IFILEMGR_GetLastError( pme->m_pFileManager);
		debug( ";----------");
		debug( ";open file [%s] failed, 0x%x", pme->m_FileName, pme->m_nLastOperationError);
		debug( ";reason, [%s]", error[pme->m_nLastOperationError - FILE_ERROR]);
	}

	return pFile;
}

static void recorder_notify_media_event_listener( Media* pme, AEEMediaCmdNotify* pCmdNotify, MediaEventEnum event)
{
	if( pme->m_pEventNotify != NULL)
	{
		MediaEventNotify notify = { 0};
		notify.eEvent  = event;
		notify.nStatus = pCmdNotify->nStatus;
		notify.pData   = pCmdNotify->pCmdData;
		notify.dwSize  = pCmdNotify->dwSize;
		notify.pMedia  = pme;
		pme->m_pEventNotify( pme->m_pUser, &notify);
	}
}
static void recorder_media_notify_handler( Media* pme, AEEMediaCmdNotify* pCmdNotify)
{
	char* cmd[] = { "SetMediaParm",
				"GetMediaParm",
				"Play",
				"Record",
				"GetTotalTime",
			};

	MSG_FATAL( ";------------------------------",0,0,0);
	MSG_FATAL( ";recorder_media_event_handler",0,0,0);
	MSG_FATAL( ";media object = [%s], 0x%x", pme->m_pszName, pCmdNotify->pIMedia,0);
	MSG_FATAL( ";cmd = %s", cmd[pCmdNotify->nCmd - MM_CMD_BASE - 1],0,0);
	MSG_FATAL( ";subCmd = 0x%x", pCmdNotify->nSubCmd,0,0);
	MSG_FATAL( ";status = 0x%x", pCmdNotify->nStatus,0,0);

	if( pCmdNotify->nStatus == MM_STATUS_ABORT)
	{
		goto __recorder_media_notify_handler_abort__;
	}
	if( pCmdNotify->nCmd == MM_CMD_GETTOTALTIME && pCmdNotify->nStatus == MM_STATUS_DONE)
	{
		pme->m_nDuration = (uint32)pCmdNotify->pCmdData;
		debug( ";totoal time is %d MS", pme->m_nDuration);
		recorder_notify_media_event_listener( pme, pCmdNotify, MEDIA_EVENT_TOTAL_TIME);

		if( pme->m_boardingAppletClsId == ISHELL_ActiveApplet( AEE_GetShell()))
		{
			repaint( TRUE);
		}
	}
	else if( pCmdNotify->nCmd == MM_CMD_GETMEDIAPARM && pCmdNotify->nStatus == MM_STATUS_DONE)
	{
		if( pCmdNotify->nSubCmd == MM_PARM_TICK_TIME)
		{
			pme->m_nTick = (uint32)pCmdNotify->pCmdData;
			debug( ";tick is %d MS", pme->m_nTick);
			if( pme->m_boardingAppletClsId == ISHELL_ActiveApplet( AEE_GetShell()))
			{
				repaint( TRUE);
			}
		}
	}
	else if( pCmdNotify->nCmd == MM_CMD_RECORD || pCmdNotify->nCmd == MM_CMD_PLAY)
	{

		switch( pCmdNotify->nStatus)
		{
			case MM_STATUS_START:	
				debug( ";started");
				recorder_notify_media_event_listener( pme, pCmdNotify, MEDIA_EVENT_STARTED);
				pme->m_nElapsedTime  = 0;	
				
            case MM_STATUS_RESUME:
            	if( pCmdNotify->nCmd == MM_CMD_PLAY)
            	{
            		debug( ";-----to notify fmradio");
            		recorder_notify_fmradio( TRUE);					
					recorder_set_media_device_auto( pme);
            	}

            	if( pCmdNotify->nStatus == MM_STATUS_RESUME && pCmdNotify->nCmd == MM_CMD_PLAY)
            	{
            		pme->m_nElapsedTime = (uint32)pCmdNotify->pCmdData;
            		debug( ";resume, elapsed time is %d MS", pme->m_nElapsedTime);
            	}

            	recorder_notify_media_event_listener( pme, pCmdNotify, MEDIA_EVENT_RESUME);
				pme->m_eState = ( pCmdNotify->nCmd == MM_CMD_RECORD ? MEDIA_STATE_RECORDING : MEDIA_STATE_PLAYING);
				break;
            case MM_STATUS_DONE:
            	debug( ";done");
            	pme->m_eState = MEDIA_STATE_DONE;
            	if( pCmdNotify->nCmd == MM_CMD_PLAY)
            	{
            		debug( ";-----to notify fmradio");
					
					recorder_set_media_device_auto_ex( pme);		//Add By zzg 2010_07_28
					
            		recorder_notify_fmradio( FALSE);
					recorder_notify_media_event_listener( pme, pCmdNotify, MEDIA_EVENT_DONE);
            	}
            	break;
            case MM_STATUS_TICK_UPDATE:
            	pme->m_nElapsedTime += pme->m_nTick;
            	if( pme->m_nDuration > 0 &&
					pme->m_nDuration < RECORDER_TIME_LIMIT_NO_LIMIT &&
					pme->m_nElapsedTime > pme->m_nDuration
            	)
            	{
            		pme->m_nElapsedTime = pme->m_nDuration;
            	}
            	debug( ";elapsed time is %d MS", pme->m_nElapsedTime);
            	recorder_notify_media_event_listener( pme, pCmdNotify, MEDIA_EVENT_TICK_UPDATE);

            	if( pCmdNotify->nCmd == MM_CMD_RECORD && pme->m_nElapsedTime >= pme->m_nTimeLimit)
            	{
            		recorder_stop_if( pme);
					pme->m_nLastOperationError	= MEDIA_ERROR_TIME_LIMIT_REACHED;
					pme->m_bMediaError			= TRUE;
					recorder_notify_media_event_listener( pme, pCmdNotify, MEDIA_EVENT_TIME_LIMIT_REACHED);
            	}
            	break;
            case MM_STATUS_DATA_IO_DELAY:				
				
            	debug( ";io delay");
            	pme->m_eState = MEDIA_STATE_BUFFERING;
            	break;
            case MM_STATUS_PAUSE:
				
            	debug( ";paused");
            	pme->m_eState = MEDIA_STATE_PAUSED;
            	if( pCmdNotify->nCmd == MM_CMD_PLAY)
            	{
            		debug( ";-----to notify fmradio");
            		recorder_notify_fmradio( FALSE);
            	}
            	recorder_notify_media_event_listener( pme, pCmdNotify, MEDIA_EVENT_PAUSED);
            	break;

#if defined( FEATURE_RECORDER_REPORT_SPACE_LOW_WARNING)
            case MM_STATUS_SPACE_WARNING:
#endif
            case MM_STATUS_SPACE_ERROR:				
				
            	MSG_FATAL( ";space warning or error",0,0,0);
            	recorder_stop_if( pme);
				pme->m_nLastOperationError 	= recorder_get_error_2( pCmdNotify->nStatus);
				pme->m_bMediaError			= TRUE;

				recorder_notify_media_event_listener( pme, pCmdNotify, MEDIA_EVENT_SPACE);
            	if( pCmdNotify->nCmd == MM_CMD_PLAY)
            	{
            		debug( ";-----to notify fmradio");
            		recorder_notify_fmradio( FALSE);
            	}
				break;
            case MM_STATUS_ABORT:				
__recorder_media_notify_handler_abort__:
            	debug( ";abort");
            case MM_STATUS_SEEK_FAIL:
            case MM_STATUS_PAUSE_FAIL:
            case MM_STATUS_RESUME_FAIL:
				MSG_FATAL( ";MM_STATUS_PAUSE_FAIL",0,0,0);
            	recorder_stop_if( pme);
            	if( pCmdNotify->nStatus == MM_STATUS_ABORT)
            	{
					pme->m_nLastOperationError 	= recorder_get_error_2( pCmdNotify->nStatus);
					pme->m_bMediaError			= TRUE;
            	}
				recorder_notify_media_event_listener( pme, pCmdNotify, MEDIA_EVENT_ERROR);
            	if( pCmdNotify->nCmd == MM_CMD_PLAY)
            	{
            		debug( ";-----to notify fmradio");
            		recorder_notify_fmradio( FALSE);
            	}
				break;

            case MM_STATUS_MEDIA_SPEC:				
            	debug( ";QCP format: 0x%x, %d", pCmdNotify->pCmdData, pCmdNotify->dwSize);
            	break;
		}
		if( pme->m_boardingAppletClsId == ISHELL_ActiveApplet( AEE_GetShell()))
		{
			repaint( TRUE);
		}
	}
}

static int32 recorder_set_media_device_auto( Media* pme)
{
	int				result		 	= 0;
	int				volumeResult	= 0;
	int32			device		 	= AEE_SOUND_DEVICE_SPEAKER;
	boolean         headsetPresent  = FALSE;

	debug( ";--------------------------------------");
	debug( ";recorder_set_media_device_auto");	

	OEM_GetConfig( CFGI_HEADSET_PRESENT, &headsetPresent, sizeof(boolean));
	
	if( headsetPresent)
	{
		device = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
	}

	result = recorder_set_media_device( pme, device);
	if( ( volumeResult = recorder_set_media_volume( pme, pme->m_nVolume)) != SUCCESS &&
		 volumeResult != MM_PENDING
	)
	{
		debug( ";setVolume failed, 0x%x", volumeResult);
	}
	return result;
}


//Add By zzg 2010_07_28
static int32 recorder_set_media_device_auto_ex( Media* pme)
{
	int				result		 	= 0;
	int				volumeResult	= 0;
	int32			device		 	= AEE_SOUND_DEVICE_SPEAKER;
	boolean         headsetPresent  = FALSE;
	
	OEM_GetConfig( CFGI_HEADSET_PRESENT, &headsetPresent, sizeof(boolean));
	
	if( headsetPresent)
	{
		device = AEE_SOUND_DEVICE_STEREO_HEADSET;   //从SPEACKER到HEADSET
	}
	else
	{
		device = AEE_SOUND_DEVICE_HANDSET;		//此函数是退出时调用的，所以是HANDSET而不是SPEAKER
	}

	result = recorder_set_media_device( pme, device);
	if( ( volumeResult = recorder_set_media_volume( pme, pme->m_nVolume)) != SUCCESS &&
		 volumeResult != MM_PENDING
	)
	{
		debug( ";setVolume failed, 0x%x", volumeResult);
	}
	return result;
}
//Add End


static void recorder_play( Media* pme)
{
	AEEMediaData	mediaData 	 	= {0};
	int				result		 	= 0;
	debug( ";--------------------------------------");
	debug( ";recorder_play, [%s]", pme->m_FileName);

	if( recorder_create_media_if( pme))
	{

		mediaData.clsData	= MMD_FILE_NAME;
		mediaData.pData		= pme->m_FileName;

		pme->m_bRecorder = FALSE;
		pme->m_eTimeDisplayType	= MEDIA_TIME_DISPLAY_TYPE_ELAPSED;//MEDIA_TIME_DISPLAY_TYPE_LEFT;


		if( ( result = IMEDIA_SetMediaData( pme->m_pMedia, &mediaData)) != SUCCESS &&
			result != MM_PENDING
		)
		{
			debug( ";SetMediaData failed, 0x%x", result);
		}
		else if( ( result = IMEDIA_SetMediaParm( pme->m_pMedia, MM_PARM_AUDIO_PATH, MM_APATH_LOCAL, 0)) != SUCCESS &&
				 result != MM_PENDING
		)
		{
			debug( ";setPath failed, 0x%x", result);
		}
		else if( ( result = IMEDIA_RegisterNotify( pme->m_pMedia, (PFNMEDIANOTIFY)recorder_media_notify_handler, pme)) != SUCCESS)
		{
			debug( ";RegisterNotify failed, 0x%x", result);
		}
		else if( ( result = IMEDIA_GetTotalTime( pme->m_pMedia)) != SUCCESS)
		{
			debug( ";getTotalTime failed, 0x%x", result);
		}
		else if( ( result = IMEDIA_Play( pme->m_pMedia)) != SUCCESS)
		{
			debug( ";Record failed, 0x%x", result);
		}
	}
	else
	{
		result = EUNSUPPORTED;
	}

	recorder_process_media_operation_result( pme, result, MEDIA_STATE_READY);
}

static boolean recorder_format_file_name( Media* pme, char* fileNameBuffer, int fileNameBufferLenght)
{

	int offset = STRLEN( pme->m_pszSaveDir);

	if( fileNameBuffer && fileNameBufferLenght >= ( offset + 18))
	{
		JulianType julian = {0};

		GETJULIANDATE( GETTIMESECONDS(), &julian);

		MEMSET( fileNameBuffer, 0, fileNameBufferLenght);
		SPRINTF( fileNameBuffer,      "%s",    pme->m_pszSaveDir);
		SPRINTF( fileNameBuffer      + offset, "%04d", julian.wYear);
		SPRINTF( fileNameBuffer + 4  + offset, "%02d", julian.wMonth);
		SPRINTF( fileNameBuffer + 6  + offset, "%02d", julian.wDay);
		SPRINTF( fileNameBuffer + 8  + offset, "%02d", julian.wHour);
		SPRINTF( fileNameBuffer + 10 + offset, "%02d", julian.wMinute);
	//	#ifdef FEATURE_LANG_THAI
	//	SPRINTF( fileNameBuffer + 12 + offset, "%s",   pme->m_pszFileExtension);
	//	#else
		SPRINTF( fileNameBuffer + 12 + offset, "%02d", julian.wSecond);
		SPRINTF( fileNameBuffer + 14 + offset, "%s",   pme->m_pszFileExtension);
	//	#endif

		debug( ";file name, %s", fileNameBuffer);
		return TRUE;
	}

	return FALSE;
}


static void recorder_record2( Media* pme)
{

	recorder_set_media_path_to_reverse( pme);
	recorder_release_media_if( pme);
	recorder_create_media_if( pme);
	recorder_set_media_volume( pme, AEE_MAX_VOLUME);

	
	recorder_process_media_operation_result( pme, recorder_record( pme), MEDIA_STATE_READY);
}

int recorder_record( Media* pme)
{
    MSG_FATAL("*****recorder_record******",0,0,0);
	return recorder_recordEx(pme, (PFNMEDIANOTIFY)recorder_media_notify_handler, NULL);   //modi by pyuangui 2013-01-10
}

//Add by pyuangui 2013-01-10
#ifdef FEATURE_VERSION_C316
int recorder_call_record( Media* pme,char* path)
{
	return recorder_recordEx( pme, (PFNMEDIANOTIFY)recorder_media_notify_handler,path);
}
#endif
//Add end

#if !defined( AEE_SIMULATOR)
static int recorder_set_format( Media* pme)
{

	int32 format = CMX_QCP_FORMAT_VAR_HALF_13K;
	if( STRCMP( pme->m_pszFileExtension, RECORDER_MEMO_FILE_EXTENSION_QCP) == 0)
	{
		format = CMX_QCP_FORMAT_VAR_HALF_13K;
	}
	else if( STRCMP( pme->m_pszFileExtension, RECORDER_MEMO_FILE_EXTENSION_AMR) == 0)
	{
		format = MM_QCP_FORMAT_FIXED_4_75_AMR;
	}
	else
	{
		pme->m_pszFileExtension = RECORDER_MEMO_FILE_EXTENSION_QCP;
	}


	return IMEDIAQCP_SetFormat( pme->m_pMedia, format);
}
#endif

int recorder_recordEx( Media* pme, PFNMEDIANOTIFY pfnNotify,char* path)
{

	AEEMediaData	mediaData 	 = {0};
	int				result		 = 0;
	IFile*			pFile		 = 0;

	MSG_FATAL( ";--------------------------------------",0,0,0);
	MSG_FATAL( ";recorder_record, [%s]", pme->m_pszName,0,0);
    MSG_FATAL("***pyg****recorder_recordEx=%d---path=%s",STRLEN(path),path,0);
	{
		RecorderPreference prefs = {0};
		ISHELL_GetPrefs( AEE_GetShell(),
				AEECLSID_RECORDER,
				1,
				&prefs,
		    	sizeof( RecorderPreference)
		    );

		//if( prefs.storage == 0 && recorder_is_tf_card_exist())
		//if( prefs.storage == 1 && recorder_is_tf_card_exist())	//Modify by zzg 2012_03_01
		if(recorder_is_tf_card_exist())	//Modify by zzg 2012_03_01
		{
		    if(path)  //Add by pyuangui 2013-01-10
			pme->m_pszSaveDir = path;
			else
			pme->m_pszSaveDir = RECORDER_MEMO_SAVE_DIR_CARD0;
		}
		else
		{
		//	pme->m_pszSaveDir = RECORDER_MEMO_SAVE_DIR_INTERNAL;
		}
	}
	recorder_format_file_name( pme, pme->m_FileName, sizeof( pme->m_FileName));
	if( ( pFile = recorder_open_file( pme, _OFM_CREATE)) == NULL)
	{
		result = pme->m_nLastOperationError;
		goto __recorder_record__result__;
	}
	else
	{
		IFILE_Release( pFile);
	}

	if( !recorder_create_media_if( pme))
	{
		result = EUNSUPPORTED;
		goto __recorder_record__result__;
	}

	mediaData.clsData	= MMD_FILE_NAME;
	mediaData.pData		= pme->m_FileName;

	pme->m_bRecorder = TRUE;
	if( pme->m_nTimeLimit == RECORDER_TIME_LIMIT_NO_LIMIT)
	{
		pme->m_eTimeDisplayType	= MEDIA_TIME_DISPLAY_TYPE_ELAPSED;
	}
	else
	{
		pme->m_eTimeDisplayType	= MEDIA_TIME_DISPLAY_TYPE_ELAPSED_AND_TOTAL;
	}

	if( ( result = IMEDIA_SetMediaData( pme->m_pMedia, &mediaData)) != SUCCESS &&
	    result != MM_PENDING
	)
	{
		debug( ";SetMediaData failed, 0x%x", result);
	}
	else if( ( result = IMEDIA_SetMediaParm( pme->m_pMedia, MM_PARM_AUDIO_PATH,pme->m_bReverse ? MM_APATH_LOCAL : MM_APATH_REMOTE , 0)) != SUCCESS &&
			 result != MM_PENDING
	)
	{
		MSG_FATAL( ";setPath failed, 0x%x", result,0,0);
	}
#if 0 //handfree时候录音声音变大  
	else if( ( result = IMEDIA_SetVolume( pme->m_pMedia, AEE_MAX_VOLUME)) != SUCCESS &&
			 result != MM_PENDING
	)
	{
		debug( ";setVolume failed, 0x%x", result);
	}
#endif
#if !defined( AEE_SIMULATOR)
	else if( ( result = recorder_set_format( pme)) != SUCCESS &&
			 result != MM_PENDING
	)
	{
		debug( ";SetFormat failed, 0x%x", result);
	}
#endif
	else if( ( result = IMEDIA_RegisterNotify( pme->m_pMedia, pfnNotify, pme)) != SUCCESS)
	{
		debug( ";RegisterNotify failed, 0x%x", result);
	}
	else if( ( result = IMEDIA_Record( pme->m_pMedia)) != SUCCESS)
	{
		debug( ";Record failed, 0x%x", result);
	}

__recorder_record__result__:
	return result;
}

int recorder_pause( Media* pme)
{

	int result = 0;

	MSG_FATAL( ";--------------------------------------",0,0,0);
	MSG_FATAL( ";recorder_pause, [%s]", pme->m_pszName,0,0);

	if( !pme->m_pMedia || ( result = IMEDIA_Pause( pme->m_pMedia)) != SUCCESS)
	{
		MSG_FATAL( ";pause failed, 0x%x", result,0,0);
	}

	return result;
}

int recorder_resume( Media* pme)
{

	int result = 0;

	debug( ";--------------------------------------");
	debug( ";recorder_resume, [%s]", pme->m_pszName);

	if( !pme->m_pMedia || ( result = IMEDIA_Resume( pme->m_pMedia)) != SUCCESS)
	{
		debug( ";resume failed, 0x%x", result);
	}

	return result;
}

void recorder_stop_if( Media* pme)
{
	if( pme->m_pMedia)
	{
		int result = 0;

		debug( ";--------------------------------------");
		debug( ";recorder_stop_if, [%s]", pme->m_pszName);		

		recorder_set_media_device_auto_ex( pme);		//Add By zzg 2010_07_28,录音未播放结束时退出，recorder_media_notify_handler不能收到MM_STATUS_DONE,所以在这里加

		if( ( result = IMEDIA_Stop( pme->m_pMedia)) != SUCCESS)
		{		
			
			debug( ";stop failed, 0x%x", result);
		}
		OEMOS_Sleep( 200);		
	}	
	recorder_release_media_if( pme);
	pme->m_eState = MEDIA_STATE_STOPPED;
}

void recorder_cancel_record( Media* pme)
{

	if( IFILEMGR_Remove( pme->m_pFileManager, pme->m_FileName) == SUCCESS)
	{
		debug( ";delete [%s] sucess", pme->m_FileName);
	}
	else
	{
		debug( ";delete [%s] failed: 0x%x", pme->m_FileName, IFILEMGR_GetLastError( pme->m_pFileManager));
	}
}

static void recorder_replay( Media* pme)
{

	int result = 0;	
	
	recorder_set_media_device_auto( pme);
	if( ( result = IMEDIA_Play( pme->m_pMedia)) != SUCCESS)
	{
		debug( ";replay failed, 0x%x", result);
	}


	recorder_process_media_operation_result( pme, result, MEDIA_STATE_PLAYING);
}

#if !defined( AEE_SIMULATOR)
#if defined( FEATURE_RECORDER_SET_AS_RINGTONE) || defined( FEATURE_RECORDER_SET_AS)
static void recorder_delete_memo_ringer_if( IFileMgr* pFileManager, const char* path)
{

	if( IFILEMGR_EnumInit( pFileManager, AEEFS_RINGERS_DIR, 0) == SUCCESS)
	{
		FileInfo info 		= {0};

		debug( ";-------------------------------------");
		debug( ";recorder_delete_memo_ringer_if");
		debug( ";to delete [%s]", path);

		while( IFILEMGR_EnumNext( pFileManager, &info))
		{
			debug( ";file, %s", info.szName);
			if( STRBEGINS( path, info.szName))
			{
				debug( ";found memo ringer file, delete it!");
				if( IFILEMGR_Remove( pFileManager, info.szName) == SUCCESS)
				{
					debug( ";delete %s successful", info.szName);
				}
				else
				{
					debug( ";delete %s failed", info.szName);
				}
				break;
			}
		}
	}

}

static boolean recorder_list_set_as_ringtone( Recorder* pme, AEEConfigItem configItem)
{
#if 1
    ringID config[PROFILENUMBER] 	= { 0};
    byte profile					= 0;
    
    if( OEM_GetConfig( CFGI_PROFILE_CUR_NUMBER, &profile, sizeof( byte)) != SUCCESS)
    {
        MSG_FATAL( ";get profile setting failed",0,0,0);
        return FALSE;
    }

    if( OEM_GetConfig( configItem, (void*)config,sizeof( config)) != SUCCESS)
    {
        MSG_FATAL( ";get config failed",0,0,0);
        return FALSE;
    }
    STRCPY(config[profile].szMusicname,pme->m_FileName);
    config[profile].ringType = OEMNV_MP3_RINGER;

    //config[profile].midID = id;

    if( OEM_SetConfig( configItem, (void*)config,sizeof( config)) != SUCCESS)
    {
        MSG_FATAL( ";set config failed",0,0,0);
        return FALSE;
    }

    MSG_FATAL( ";set memo as ringtone successful %d %d",config[profile].ringType,profile,0);
    return TRUE;

#else
	ringID 			config[PROFILENUMBER] 	= { 0};
	byte			profile					= 0;
	AEERingerID 	id 						= AEE_RINGER_ID_NONE;
	int				result					= 0;
	char 			memo[64] 				= {0};

	char*			ringtone_file_name[]		= {
										RECORDER_MEMO_CALL_RINGTONE_FILE_NAME,
										RECORDER_MEMO_ALARM_RINGTONE_FILE_NAME,
										RECORDER_MEMO_SMS_RINGTONE_FILE_NAME
									};
	char*			ringtone_path_name[]		= {
										RECORDER_MEMO_CALL_RINGTONE_PATH_NAME,
										RECORDER_MEMO_ALARM_RINGTONE_PATH_NAME,
										RECORDER_MEMO_SMS_RINGTONE_PATH_NAME
									};
	AECHAR*			ringtone_wide_file_name[]	= {
										RECORDER_MEMO_CALL_RINGTONE_WIDE_FILE_NAME,
										RECORDER_MEMO_ALARM_RINGTONE_WIDE_FILE_NAME,
										RECORDER_MEMO_SMS_RINGTONE_WIDE_FILE_NAME
									};
	int				ringtone					= 0;

	if( configItem == CFGI_PROFILE_ALARM_RINGER)
	{
		ringtone = 1;
	}
	else if( configItem == CFGI_PROFILE_SMS_RINGER)
	{
		ringtone = 2;
	}

	if( ringtone < 0 || ringtone > 2)
	{
		debug( ";ringtone is overflow, %d", ringtone);
		ringtone = 0;
	}

	debug( ";------------------------------------------------------");
	debug( ";recorder_list_set_as_ringer");
	debug( ";%s", pme->m_FileName);

	if( !STRBEGINS( AEEFS_RINGERS_DIR, pme->m_FileName))
	{
		IFile* 				pFile		= IFILEMGR_OpenFile( pme->m_pFileManager, pme->m_FileName, _OFM_READ);
		AEESoundPlayerFile	fileType	= AEE_SOUNDPLAYER_FILE_QCP;

		debug( ";%s, is not in ringer dir,copy it to ringer dir", pme->m_FileName);
		if( !pFile)
		{
			debug( ";open file [%s] failed", pme->m_FileName);
			return AEE_RINGER_ID_NONE;
		}

		recorder_delete_memo_ringer_if( pme->m_pFileManager, ringtone_path_name[ringtone]);
		if( STRCMP( pme->m_Media.m_pszFileExtension, RECORDER_MEMO_FILE_EXTENSION_AMR) == 0)
		{
			fileType = AEE_SOUNDPLAYER_FILE_AMR;
		}

		debug( ";try to create ringer, %s%s", ringtone_file_name[ringtone], pme->m_Media.m_pszFileExtension);
		debug( ";0x%x, %d, %S", pme->m_pRingerMgr, fileType, ringtone_wide_file_name[ringtone]);
		if( ( result = IRINGERMGR_Create( pme->m_pRingerMgr, ringtone_wide_file_name[ringtone], fileType, (IAStream*)pFile)) != SUCCESS)
		{
			debug( ";create ringer failed, 0x%x", result);
			IFILE_Release( pFile);
			return AEE_RINGER_ID_NONE;
		}
		IFILE_Release( pFile);
	}

	debug( ";create ringtone successful");
	STRCPY( memo, ringtone_file_name[ringtone]);
	STRCPY( memo + STRLEN( memo), pme->m_Media.m_pszFileExtension);
	debug( ";memo ringtone is %s", memo);

	id = IRINGERMGR_GetRingerID( pme->m_pRingerMgr, memo);
	debug( ";ringtone %s, id is %d", id);
	if( id == AEE_RINGER_ID_NONE)
	{
		debug( ";get ringtone id failed, %s", memo);
		return AEE_RINGER_ID_NONE;
	}

    if( OEM_GetConfig( CFGI_PROFILE_CUR_NUMBER, &profile, sizeof( byte)) != SUCCESS)
    {
    	debug( ";get profile setting failed");
    	return AEE_RINGER_ID_NONE;
    }

    if( OEM_GetConfig( configItem, (void*)config,sizeof( config)) != SUCCESS)
    {
    	debug( ";get config failed");
    	return AEE_RINGER_ID_NONE;
    }

	config[profile].midID = id;

    if( OEM_SetConfig( configItem, (void*)config,sizeof( config)) != SUCCESS)
    {
    	debug( ";set config failed");
    	return AEE_RINGER_ID_NONE;
    }

    debug( ";set memo as ringtone successful");
	return id;
#endif
}

static boolean recorder_list_set_as_call_ringtone( Recorder* pme)
{

    return recorder_list_set_as_ringtone( pme, CFGI_PROFILE_CALL_RINGER);
}
#endif

#if defined( FEATURE_RECORDER_SET_AS)
static boolean recorder_list_set_as_alarm_ringtone( Recorder* pme)
{

    return recorder_list_set_as_ringtone( pme, CFGI_PROFILE_ALARM_RINGER);
}

static boolean recorder_list_set_as_sms_ringtone( Recorder* pme)
{

    return recorder_list_set_as_ringtone( pme, CFGI_PROFILE_SMS_RINGER_ID);
}
#endif
#endif
static boolean dialog_handler_of_state_storage_setup( Recorder* pme, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
	static IMenuCtl* 	pMenu 		= 0;
	static uint16		selected	= 0;
	static boolean		reserve		= 0;
	
	switch (evt)
	{
#ifdef FEATURE_LCD_TOUCH_ENABLE
		case  EVT_PEN_UP:
		{
			int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
			int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
			AEERect bottomBarRect;
			//int ht;
			int nBarH ;
			AEEDeviceInfo devinfo;
			nBarH = GetBottomBarHeight(pme->m_pDisplay);
			
			MEMSET(&devinfo, 0, sizeof(devinfo));
			ISHELL_GetDeviceInfo(pme->m_pShell, &devinfo);
			SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
			MSG_FATAL("wXPos=====%d,wYPos=========%d",wXPos,wYPos,0);
			
			if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect))
			{
				if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//右
				{	
					boolean rt = ISHELL_PostEvent(pme->m_pShell,AEECLSID_RECORDER,EVT_USER,AVK_CLR,0);
						return rt;
					//evt = EVT_KEY;
					//wParam = AVK_CLR;
				}
				else if((wXPos >= bottomBarRect.x) && (wXPos < bottomBarRect.x + (bottomBarRect.dx/3)))//左
				{						
					boolean rt = ISHELL_PostEvent(pme->m_pShell,AEECLSID_RECORDER,EVT_USER,AVK_SELECT,0);
						return rt;
					//evt = EVT_KEY;
					//wParam = AVK_SELECT;
				}
			}
		}
#endif

		case EVT_DIALOG_INIT:
		{
			pMenu 		= (IMenuCtl*)IDIALOG_GetControl( pme->m_pActiveDialog, IDC_STORAGE_SETUP);
			reserve		= 0;
			if( pme->m_ePreState == STATE_MAIN && !pme->m_bSuspended)
			{
            	uint16 storage[] = {
            			//IDS_STORAGE_SETUP_PHONE,
            			IDS_STORAGE_SETUP_CARD0
            	};

				//selected = storage[pme->m_prefs.storage];
			}
		}
		return TRUE;

		case EVT_DIALOG_START:
		{
            if(pme->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);
    			#if 0
    			IMENUCTL_SetTitle( pMenu, AEE_RECORDER_RES_FILE, IDS_TITLE, 0);
    			#else
    			{
    				AECHAR wszTitle[16] = {0};
					#ifdef FEATURE_VERSION_C337
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
					
    				IANNUNCIATOR_SetFieldText(pme->m_pIAnn,wszTitle);            
    			}
    			#endif
            }

			//IMENUCTL_AddItem( pMenu, AEE_RECORDER_RES_FILE, IDS_STORAGE_SETUP_PHONE, IDS_STORAGE_SETUP_PHONE, 0, 0);
			IMENUCTL_AddItem( pMenu, AEE_RECORDER_RES_FILE, IDS_STORAGE_SETUP_CARD0, IDS_STORAGE_SETUP_CARD0, 0, 0);

			IMENUCTL_SetBottomBarType( pMenu, BTBAR_SELECT_BACK);

            InitMenuIcons( pMenu);
            {
            	uint16 storage[] = {
            			//IDS_STORAGE_SETUP_PHONE,
            			IDS_STORAGE_SETUP_CARD0
            	};

            	SetMenuIcon( pMenu, storage[pme->m_prefs.storage], TRUE);
            }
			IMENUCTL_SetSel( pMenu, selected);

			repaint( TRUE);
		}
		return TRUE;

		case EVT_DIALOG_END:
		{
			if( !reserve && !pme->m_bSuspended)
			{
				selected = 0;
			}
		}
		return TRUE;

		case EVT_USER_REDRAW:
		{
			IMENUCTL_Redraw( pMenu);
		}
		return TRUE;

		case EVT_KEY:
		{
			switch( wParam)
			{
				case AVK_SOFT2:
				case AVK_CLR:
				{
					CLOSE_DIALOG(DLGRET_CANCELED);
				}
				break;
			}
		}
		return TRUE;

		case EVT_CTL_SEL_CHANGED:
		{
			selected = IMENUCTL_GetSel( pMenu);
		}
		return TRUE;

		case EVT_COMMAND:
		{
			selected = wParam;
			pme->m_prefs.storage = 0;
			if( selected == IDS_STORAGE_SETUP_CARD0)
			{
				pme->m_prefs.storage = 0;//1;
			}
			ISHELL_SetPrefs( pme->m_pShell,
					AEECLSID_RECORDER,
					1,
					&pme->m_prefs,
					sizeof( RecorderPreference)
				);
			CLOSE_DIALOG(DLGRET_OK);
		}
		return TRUE;
	}

	return FALSE;
}



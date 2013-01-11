/** ----------------------------------------------------------------------------
 *
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008-12-1      chunfeng.hou        create
 *
 * -----------------------------------------------------------------------------
 */

#ifndef RECORDER_H_
#define RECORDER_H_

#include "AEEMedia.h"
#include "aeeerror.h"
#include "aeefile.h"
#include "MediaGallery.h"

#if defined(FEATURE_LANG_THAI)||defined(FEATURE_LANG_CHINESE)
#define RECORDER_FILE_NAME_LENGHT	12
#else
#define RECORDER_FILE_NAME_LENGHT	16
#endif
//#define RECORDER_MEMO_SAVE_DIR_INTERNAL 		"fs:/mymemos/"
//#define RECORDER_MEMO_SAVE_DIR_CARD0			(AEEFS_CARD0_DIR "mymemos/")
#define RECORDER_MEMO_SAVE_DIR_INTERNAL MG_PHONECALLMEMO_PATH
#define RECORDER_MEMO_SAVE_DIR_CARD0 MG_MASSCARDVOICEMEMO_PAHT

#define RECORDER_MEMO_FILE_EXTENSION_QCP	".qcp"
#define RECORDER_MEMO_FILE_EXTENSION_AMR	".amr"
#define RECORDER_MEMO_RINGTONE_PATH_NAME_PREFIX		"ringers/__memo_"
#define RECORDER_MEMO_CALL_RINGTONE_WIDE_FILE_NAME  L"__memo_ringer__"
#define RECORDER_MEMO_CALL_RINGTONE_FILE_NAME 		"__memo_ringer__"
#define RECORDER_MEMO_CALL_RINGTONE_PATH_NAME 		(AEEFS_RINGERS_DIR "__memo_ringer__")
#define RECORDER_MEMO_ALARM_RINGTONE_WIDE_FILE_NAME L"__memo_alarm__"
#define RECORDER_MEMO_ALARM_RINGTONE_FILE_NAME 		"__memo_alarm__"
#define RECORDER_MEMO_ALARM_RINGTONE_PATH_NAME 		(AEEFS_RINGERS_DIR "__memo_alarm__")
#define RECORDER_MEMO_SMS_RINGTONE_WIDE_FILE_NAME L"__memo_sms__"
#define RECORDER_MEMO_SMS_RINGTONE_FILE_NAME 		"__memo_sms__"
#define RECORDER_MEMO_SMS_RINGTONE_PATH_NAME 		(AEEFS_RINGERS_DIR "__memo_sms__")
#define  RECORDER_TIME_LIMIT_NO_LIMIT	0xffffffff
#define RECORDER_TIME_LIMIT				RECORDER_TIME_LIMIT_NO_LIMIT
#define RECORDER_FILE_NUMBER_LIMIT 		0xffffffff
#if !defined( repaint)
#define sendEvent( e)       ISHELL_SendEvent( AEE_GetShell(), ISHELL_ActiveApplet( AEE_GetShell()), e, 0, 0)
#define postEvent( e)       ISHELL_PostEvent( AEE_GetShell(), ISHELL_ActiveApplet( AEE_GetShell()), e, 0, 0)
#define repaint( deferred)  (deferred) ? postEvent( EVT_USER_REDRAW) : sendEvent( EVT_USER_REDRAW)
#endif
#if !defined( min)
#define min( a, b)  ( (a) < (b)) ? (a) : (b)
#endif
#if !defined( max)
#define max( a, b)  ( (a) > (b)) ? (a) : (b)
#endif
#define debug(...)


typedef enum
{
	MEDIA_STATE_INIT,
	MEDIA_STATE_READY,
	MEDIA_STATE_PAUSED,
	MEDIA_STATE_DONE,
	MEDIA_STATE_RECORDING,
	MEDIA_STATE_PLAYING,
	MEDIA_STATE_BUFFERING,
	MEDIA_STATE_STOPPED
} RecorderMediaStateEnum;

typedef enum
{
	MEDIA_ERROR_UNKNOWN,
	MEDIA_ERROR_BAD_STATE,
	MEDIA_ERROR_BAD_PARAM,
	MEDIA_ERROR_UNSPPORTED,
	MEDIA_ERROR_ITEM_BUSY,
	MEDIA_ERROR_BAD_MEDIA_DATA,
	MEDIA_ERROR_ABORTED,
	MEDIA_ERROR_SPACE_WARNING,
	MEDIA_ERROR_SPACE_ERROR,
	MEDIA_ERROR_TIME_LIMIT_REACHED

} RecorderMediaErrorEnum;

typedef enum
{
	MEDIA_EVENT_TICK_UPDATE,
	MEDIA_EVENT_TOTAL_TIME,
	MEDIA_EVENT_TIME_LIMIT_REACHED,
	MEDIA_EVENT_SPACE,
	MEDIA_EVENT_ERROR,
	MEDIA_EVENT_STARTED,
	MEDIA_EVENT_RESUME,
	MEDIA_EVENT_PAUSED,
	MEDIA_EVENT_DONE
} MediaEventEnum;

typedef enum
{
	MEDIA_TIME_DISPLAY_TYPE_ELAPSED,
	MEDIA_TIME_DISPLAY_TYPE_LEFT,
	MEDIA_TIME_DISPLAY_TYPE_LEFT_AND_TOTAL,
	MEDIA_TIME_DISPLAY_TYPE_ELAPSED_AND_TOTAL,
} MediaTimeDisplayType;

typedef struct Media Media;

typedef struct
{
	Media*			pMedia;
	MediaEventEnum	eEvent;
	int				nStatus;
    void *      	pData;
    uint32      	dwSize;
} MediaEventNotify;

typedef void(*PFNMEDIAEVENTNOTIFY)( void* pUser, MediaEventNotify* pEventNotify);

struct Media
{

	IMedia*					m_pMedia;
	IFileMgr*				m_pFileManager;
	int						m_nLastOperationError;
	RecorderMediaStateEnum	m_eState;
	char					m_FileName[AEE_MAX_FILE_NAME];
	uint32					m_nDuration;
	uint32					m_nTick;
	uint32					m_nElapsedTime;
	int32					m_nVolume;
	int32					m_nOldDevice;
	int32					m_nOldPauseTime;
	int32					m_nOldPath;
	int32					m_nOldVolume;
	boolean					m_bReverse;
	boolean					m_bMediaError;
	boolean					m_bRecorder;
	char*					m_pszName;
	uint32					m_nTimeLimit;
	PFNMEDIAEVENTNOTIFY		m_pEventNotify;
	void*					m_pUser;
	MediaTimeDisplayType	m_eTimeDisplayType;
	char*					m_pszSaveDir;
	char*					m_pszFileExtension;
	AEECLSID				m_boardingAppletClsId;
};

boolean recorder_create_media_if( Media* pme);
void recorder_release_media_if( Media* pme);
boolean recorder_create_filemgr( Media* pme);
void recorder_release_filemgr_if( Media* pme);
int recorder_get_memo_number( Media* pme);
boolean recorder_init_media( Media* pme);

int32 recorder_set_media_device( Media* pme, int32 device);
void recorder_set_media_path_to_forward( Media* pme);
void recorder_set_media_path_to_reverse( Media* pme);
void recorder_set_media_event_notify( Media* pme, PFNMEDIAEVENTNOTIFY pEventNotify, void* pUser);
void recorder_set_media_time_limit( Media* pme, uint32 limit);
int	recorder_set_media_volume( Media* pme, int32 volume);
void recorder_set_save_dir( Media* pme, char* dir);

int recorder_record( Media* pme);
int recorder_recordEx( Media* pme, PFNMEDIANOTIFY pfnNotify,char* path);   //modi by pyuangui 2013-01-10
int recorder_pause( Media* pme);
int recorder_resume( Media* pme);
void recorder_stop_if( Media* pme);
void recorder_cancel_record( Media* pme);

int recorder_get_error_1( int system_error);
int recorder_get_error_2( int system_error);
void recorder_process_media_operation_result( Media* pme, int result, RecorderMediaStateEnum shallState);
#endif /* RECORDER_H_ */

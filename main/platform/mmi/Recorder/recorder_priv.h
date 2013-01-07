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

#ifndef RECORDER_PRIV_H_
#define RECORDER_PRIV_H_

#include "oemfeatures.h"

#define FEATURE_RECORDER_RENAME
#define FEATURE_AUTO_SAVE_WHEN_TIME_LIMIT_REACHED
#define FEATURE_RECORDER_SET_AS
#if defined( FEATURE_LANG_CHINESE) || defined( FEATURE_LANG_TCHINESE)
#define FEATURE_RECORDER_SUPPORT_CHINESE_FILE_NAME
#endif
#define FEATURE_RECORDER_RECORD_LIST_WITH_ICON
#define FEATURE_RECORDER_PLAY_PAUSE
#define FEATURE_RECORDER_RECORD_PAUSE
#if 0
#define FEATURE_RECODER_TEST
#define FEATURE_RECORDER_REPORT_SPACE_LOW_WARNING
#endif

#include "AEEShell.h"
#include "AEEStdLib.h "
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "aee_oem.h"
#include "appscommon.h"
#include "appscommon.brh"
#include "recorder.brh"
#include "recorder_images.brh"
#include "OEMClassIDs.h"
#include "oemcfgi.h"
#include "aeeringer.h"
#include "recorder.h"
#include "aeesound.h"
#include "AEEAnnunciator.h"

#ifndef WIN32
#include "ui.h"
#else
#include "oemhelperfunctype.h"
#endif//WIN32
#define OEM_IME_DIALOG   (0x7FFF)

#define drawBottomBar( bottomBarType);              \
{                                                   \
	BottomBar_Param_type BarParam;                  \
	MEMSET(&BarParam, 0, sizeof(BarParam));         \
	BarParam.eBBarType = bottomBarType;             \
	DrawBottomBar(pme->m_pDisplay, &BarParam);      \
}
#define drawTitleBar( resFile, resId); 										\
{																			\
	TitleBar_Param_type titleBarParms = {0}; 								\
	AEERect				rect		  = pme->m_rc; 							\
	MEMSET( &titleBarParms, 0, sizeof( TitleBar_Param_type)); 				\
	rect.dy = GetTitleBarHeight( pme->m_pDisplay); 							\
	titleBarParms.dwAlignFlags  = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER; 	\
	titleBarParms.nTitleResID   = resId; 									\
	STRCPY( titleBarParms.strTitleResFile, resFile); 			\
	titleBarParms.prc           = &rect; 									\
	DrawTitleBar( pme->m_pDisplay, &titleBarParms); 						\
}
#define MOVE_TO_STATE(nextState);           \
{                                           \
    RecorderStateEnum state;                \
    state = nextState;                      \
    pme->m_ePreState = pme->m_eCurState;    \
    pme->m_eCurState = state;               \
}
#define CLOSE_DIALOG(DlgRet);                \
{                                            \
	pme->m_bAppIsReady = 0;					 \
	pme->m_eDlgRet = DlgRet;                 \
	(void) ISHELL_EndDialog(pme->m_pShell);  \
}


#if defined( AEE_SIMULATOR)
	#define  AEE_RECORDER_RES_FILE ("fs:/mod/recorder/en/" RECORDER_RES_FILE)
	#define  AEE_RECORDER_IMAGES_RES_FILE ("fs:/mod/recorder/" RECORDER_IMAGES_RES_FILE)
#else
	#define  AEE_RECORDER_RES_FILE (AEE_RES_LANGDIR RECORDER_RES_FILE)
	#define  AEE_RECORDER_IMAGES_RES_FILE (AEE_RES_LANGDIR RECORDER_IMAGES_RES_FILE)
#endif

//--------------------------------------------------------------------------------------------------
typedef enum
{
   DLGRET_CREATE,
   DLGRET_OK,
   DLGRET_CANCELED,
   DLGRET_NEW_RECORD,
   DLGRET_RECORD_LIST,
   DLGRET_PLAY,
   DLGRET_SET_AS
   ,DLGRET_PLAY_MSG
   ,DLGRET_STORAGE_SETUP
} DialogReturnValueEnum;

typedef enum
{
   STATE_NONE,
   STATE_INIT,
   STATE_MAIN,
   STATE_RECORD,
   STATE_RECORD_LIST,
   STATE_SET_AS,
   STATE_PLAY_MSG,
   STATE_STORAGE_SETUP,
   STATE_EXIT
} RecorderStateEnum;

typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMActionEnum;

enum
{
	RECORDER_IMAGE_BG_RECORD,
	RECORDER_IMAGE_BG_PLAY,
	RECORDER_IMAGE_PROGRESSBAR1,
	RECORDER_IMAGE_PROGRESSBAR2,
	RECORDER_IMAGE_VOLUME0,
	RECORDER_IMAGE_VOLUME1,
	RECORDER_IMAGE_VOLUME2,
	RECORDER_IMAGE_VOLUME3,
	RECORDER_IMAGE_VOLUME4,
	RECORDER_IMAGE_VOLUME5
};

typedef struct
{
	byte storage; // 0 phone, 1 TF card
} RecorderPreference;

typedef struct
{
	AEEApplet a;

	IDisplay*	m_pDisplay;
	IShell*		m_pShell;

    AEERect 				m_rc;
	IDialog*				m_pActiveDialog;
	uint16 					m_nActiveDialogId;
	DialogReturnValueEnum 	m_eDlgRet;
	boolean 				m_bNotOverwriteDlgRet;
	RecorderStateEnum 		m_ePreState;
	RecorderStateEnum 		m_eCurState;
	boolean 				m_bInactive;
	boolean					m_bSuspended;
	boolean 				m_bAppIsReady;
	boolean					m_bLockkey;
    #ifdef FEATURE_VERSION_C316
	boolean                 m_bFristStart;   //Add by pyuangui 2013-01-07
	#endif
	IFileMgr*				m_pFileManager;
	Media					m_Media;
	int						m_nLastOperationError;
	char					m_FileName[AEE_MAX_FILE_NAME];
	ISound*					m_pSound;
	IImage*					m_pImage[10];
	AECHAR					m_Title[AEE_MAX_FILE_NAME + 32];
	boolean  				m_bScroll;
	int						m_nTitleX;
	RGBVAL					m_ThemeTextColor;

	int						m_ptr[8];
	AEECallback				m_cb;
    IAnnunciator        *m_pIAnn;
	RecorderPreference		m_prefs;
} Recorder;

// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
#define APPISREADY_TIMER        100
#define EVT_APPISREADY          (EVT_USER+30)


extern int OEM_GetConfig( AEEConfigItem i, void* pBuff, int nSize);
extern int OEM_SetConfig( AEEConfigItem i, void* pBuff, int nSize);


int Recorder_ShowDialog( Recorder* pme,uint16 dlgResId);
boolean Recorder_RouteDialogEvent( Recorder* pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);
NextFSMActionEnum Recorder_ProcessState( Recorder* pme);
#endif /* RECORDER_PRIV_H_ */

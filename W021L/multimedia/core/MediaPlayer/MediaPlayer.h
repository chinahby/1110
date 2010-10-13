#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

/*===========================================================================

FILE: MediaPlayer.h

DESCRIPTION: Header file for MediaPlayer application

       Copyright @2000-2005 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //depot/asic/msmshared/apps/MediaPlayer/MediaPlayer.h#126 $

 when        who          what, where, why
 ---------   ---          -----------------------------------------------------
 1/18/2005   dkrishna     Initial version. Added MediaPlayer header file.
 6/09/2005   jroberts     Added MediaPlayerPlayState to share with IDIAG.
 6/10/2005   jroberts     Added MP_PLAYSTATE_EXITING to MediaPlayerPlayState.
===========================================================================*/
#include "target.h"                    // Target configuration file
#include "customer.h"                  // Customer configuration file
#include "msg.h"
#include "uixsnd.h"                   // The number of sound levels that we have.

#include "MPDefines.h"
#include "AEE.h"
#include "BREWVersion.h"
#include "AEEComdef.h"


#ifdef AEE_STATIC
#include "OEMFeatures.h"
#endif //AEE_STATIC
#include "AEEAppGen.h"                 // AEEApplet structure and
                                       // Applet services
#include "MediaPlayer.bid"
#include "mediaplayer_res.h"
#include "MPDlg.h"

/*
 * AEE Services used by app
 */
#include "AEE_OEM.h"
#include "AEEStdLib.h"                 // AEE Stb Lib Services
#include "AEEMenu.h"                   // AEE Menu Services
#include "AEEFile.h"                   // AEE File Manager Services
#include "AEEMedia.h"                  // AEE Multimedia Services
#include "AEEMediaFormats.h"
#include "AEEImageCtl.h"               // AEE ImageCtl Services
#include "AEEMimeTypes.h"              // AEE MIME definitions
#include "AEEGraphics.h"
#include "AEEMediaUtil.h"              // AEE Media Utility functions
#include "OEMOS.h"
#include "AEETelephone.h"              // AEE ITelephone events - Call accept for incall mixing
#include "AEETelephoneTypes.h"         // AEE Telephone Notify (AEETNotifyInfo)

#define BREW_VERSION_LESS_THAN(maj, min) \
        (BREW_MAJ_VER < maj || (BREW_MAJ_VER == maj && BREW_MIN_VER < min))

#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
  #include "AEEMediaMPEG4.h"
  #ifndef IMEDIA_GetPlaybackPositions
    #include "OEMMediaMPEG4.h"
  #endif //IMEDIA_GetPlaybackPositions
#endif //FEATURE_APP_MEDIA_VIDEO

#ifdef FEATURE_APP_MPEG4

#ifdef FEATURE_QTV_PDP_SELECTION
#include "AEEPDP.h"
#endif //FEATURE_QTV_PDP_SELECTION

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#include "AEEWeb.h"
#endif     // FEATURE_QTV_PSEUDO_STREAM || // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif     // FEATURE_APP_MPEG4

#include "HttpDownloader.h"

#if defined(FEATURE_APP_REAL)
#include "AEEMediaReal.h"
#endif //FEATURE_APP_REAL

#include "event.h"                     // event_report function

#ifdef FEATURE_QTV_DIAG_CMD
#include "AEEClipboard.h"
#endif //FEATURE_QTV_DIAG_CMD

#ifdef FEATURE_MP4_LCD_CONTRAST
#include "disp.h"
#endif // FEATURE_MP4_LCD_CONTRAST

#include <ctype.h>

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif //FEATURE_QTV_MFDRM

#include "MPSettings.h"
#include "MPUtils.h"

#ifdef FEATURE_MP_EQ_UI
#include "MPSetupWin.h"
#endif //FEATURE_MP_EQ_UI

#ifdef FEATURE_MP_SEEK_TIME_DLG
#include "AEEText.h"
#endif //FEATURE_MP_SEEK_TIME_DLG

#ifdef FEATURE_BT_EXTPF_AV
#include "AEEBTExt.h"
#include "AEEBTExtAVRCP.h"
#include "AEEBTExtA2DP.h"
#endif // FEATURE_BT_EXTPF_AV

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#ifdef FEATURE_MMOVERLAY
#include "AEEMMOverlay.h"
#endif // FEATURE_MMOVERLAY

#include "MPProgCtl.h"

/*
 * MediaPlayer playing state is the minor state and is used by both
 * mainline MediaPlayer code and IDIAG (so placed prior to #include
 * of MediaPlayerIDIAG.h).
 *
 * Note:  These enums are very order sensitive!  Only place new play
 *        states at the end of the list.  AST test scripts rely on
 *        the current ordering of these enums.
 */
typedef enum MediaPlayerPlayState
{
  /* Do NOT add any more enums to the start of this list! */
  MP_PLAYSTATE_PLAYING,
  MP_PLAYSTATE_PAUSED,
  MP_PLAYSTATE_STOPPED,
  MP_PLAYSTATE_RECORDING,
  MP_PLAYSTATE_CONNECTING,
  MP_PLAYSTATE_BUFFERING,
  MP_PLAYSTATE_READY_TO_PLAY,
  MP_PLAYSTATE_ERROR,
  MP_PLAYSTATE_EXITING
  /* Add new enum entries here at the end */
} MediaPlayerPlayState;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

/*===========================================================================
                      INTERFACE DEFINITION
===========================================================================*/
/*
 * This is a generic IWindow interface.
 */
typedef struct _IWindow IWindow;

QINTERFACE(IWindow)
{
  /*
   * Enables/Disables the window. Window controls will not process
   * events if the window is disabled.
   */
  void (*Enable)(IWindow *po, boolean bEnable);

  /*
   * Redraws the window if enabled
   */
  void (*Redraw)(IWindow *po);

  /*
   * Handles the events routed to the window
   */
  boolean (*HandleEvent)(IWindow *po, AEEEvent eCode, uint16 wParam,
                         uint32 dwParam);

  /*
   * Releases the window resources
   */
  void (*Delete)(IWindow *po);
};

#define IWINDOW_Enable(p)                 GET_PVTBL(p, IWindow)->Enable(p, TRUE)
#define IWINDOW_Disable(p)                GET_PVTBL(p, IWindow)->Enable(p, FALSE)
#define IWINDOW_Redraw(p)                 GET_PVTBL(p, IWindow)->Redraw(p)
#define IWINDOW_HandleEvent(p, e, w, dw)  GET_PVTBL(p, IWindow)->HandleEvent(p, e, w, dw)
#define IWINDOW_Delete(p)                 GET_PVTBL(p, IWindow)->Delete(p)

/*===========================================================================
                     MACROS
===========================================================================*/
#define MP_IWINDOW_SETVTBL(pVtbl, pfnEn, pfnRd, pfnHE, pfnDel) \
   (pVtbl)->Enable      = (pfnEn); \
   (pVtbl)->Redraw      = (pfnRd); \
   (pVtbl)->HandleEvent = (pfnHE); \
   (pVtbl)->Delete      = (pfnDel)

#define MP_ISEVTKEY(e)                 ((e) == EVT_KEY)
#define MP_ISONEOFKEYEVENT(e)          ((e) == EVT_KEY || (e) == EVT_KEY_PRESS)
#define MP_ISCLR(e)                    (MP_ISEVTKEY(e) && wParam == AVK_CLR)
#define MP_ISEVTCMD(e)                 ((e) == EVT_COMMAND)
#define MP_ISCMD(e, c)                 (MP_ISEVTCMD(e) && (c) == wParam)
#define MP_ISFULLSCRN(p)               ((p)->m_bImage && (p)->m_bFullScreen)


/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/
#ifdef FEATURE_MP_RECORDER_SETTINGS
/*
 * Recording type for MP, should not conflict with MM_QCP_FORMAT_*
 * defined in AEEMediaFormats.h
 */
#define MP_8K_PCM_RECORDING   258
#define MP_AAC_RECORDING      (MP_8K_PCM_RECORDING + 1)
#define MP_EVRCNB_RECORDING   (MP_8K_PCM_RECORDING + 2)
#endif // FEATURE_MP_RECORDER_SETTINGS

#ifdef FEATURE_QTV_DIAG_CMD
#define EVT_MEDIAPLAYER_SELECT_FILE 0x7500      // Select file to play
                                                // on the Media Player
                                                // APP
#define EVT_MEDIAPLAYER_CONTROL     0x7501      // Sends Control Event
                                                // to Media Player APP

#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
/*
 * Event to set default directory for RTSP Stream Recording
 */
#define EVT_MEDIAPLAYER_RTSP_STR_REC_FILE_NAME 0x7502
#endif

#define EVT_MEDIAPLAYER_SEEK 0x7503    // media seek command

#define CMD_DIAG_PLAY        1
#define CMD_DIAG_STOP        2
#define CMD_DIAG_REWIND      3
#define CMD_DIAG_FF          4
#define CMD_DIAG_PAUSE       5
#define CMD_DIAG_FF_STEP     6
#define CMD_DIAG_BK_STEP     7
#define CMD_DIAG_VOL_UP      8
#define CMD_DIAG_VOL_DOWN    9
#define CMD_DIAG_FULLSCRN   10
#define CMD_DIAG_INFO       11
#define CMD_DIAG_CCW_ROTATE 12
#define CMD_DIAG_CW_ROTATE  13
#define CMD_DIAG_ZOOM_IN    14
#define CMD_DIAG_ZOOM_OUT   15
#define CMD_DIAG_CONT_SINGLE  16
#define CMD_DIAG_CONT_ALL   17
#define CMD_DIAG_AUTOPLAY_ON  18
#define CMD_DIAG_AUTOPLAY_OFF 19
#define CMD_DIAG_TERMINATE      20

#if defined(FEATURE_QTV_RANDOM_ACCESS_REPOS) || defined(FEATURE_QTV_PLAYLIST)
#define CMD_DIAG_FF_SKIP             21
#define CMD_DIAG_BK_SKIP             22
#endif     // FEATURE_QTV_RANDOM_ACCESS_REPOS

#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
#define CMD_DIAG_RTSP_STR_REC_ON      23
#define CMD_DIAG_RTSP_STR_REC_OFF     24
#define CMD_DIAG_RTSP_STR_REC_FILE_OW_ON  25
#define CMD_DIAG_RTSP_STR_REC_FILE_OW_OFF 26
#define CMD_DIAG_UI_SETTINGS_RTSP_STR_REC_ON  27
#define CMD_DIAG_UI_SETTINGS_RTSP_STR_REC_OFF 28
#endif     // FEATURE_APP_MPEG4 && FEATURE_QTV_STREAM_RECORD

#define CMD_DIAG_CONT_OFF   29

/*
 * Command from the DIAG interface to modify the point at which
 * QTV streaming starts rebuffering
 */
#define CMD_DIAG_MODIFY_BUFFER_LOWMARK  30

#define DMSSEVT_SELECT_FILE  658
#define DMSSEVT_MEDIAPLAYER_CONTROL 659

#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
#define DMSSEVT_SET_DFLT_RTSP_STR_REC_DIR 660
#endif     // FEATURE_APP_MPEG4 && FEATURE_QTV_STREAM_RECORD

#endif     // FEATURE_QTV_DIAG_CMD

#define MP_SPLASH_TIMER       750
#define MP_MAX_STRLEN         64

// flash media directory
#if MIN_BREW_VERSION(3,0)
  #ifdef PLATFORM_LTK
#error code not present
  #else
    #ifdef FEATURE_COMMON_MM_DIR
      #define MP_MEDIA_DIR   "fs:/shared/media"
    #else
      #define MP_MEDIA_DIR   "fs:/~/media"
    #endif //FEATURE_COMMON_MM_DIR
  #endif // PLATFORM_LTK
#else
  #define MP_MEDIA_DIR          "media"
#endif //MIN_BREW_VERSION(3, 0)

// mmc media directory
#if MIN_BREW_VERSION(3, 0)
  # define MP_MMC_MEDIA_DIR                AEEFS_CARD0_DIR"media"
  # define MP_MMC_ICONTENT_MEDIA_MUSIC_DIR AEEFS_CARD0_DIR"music"
  # define MP_MMC_ICONTENT_MEDIA_VIDEO_DIR AEEFS_CARD0_DIR"video"
#else
  # define MP_MMC_MEDIA_DIR                "mmc1/media"
  # define MP_MMC_ICONTENT_MEDIA_MUSIC_DIR "mmc1/music"
  # define MP_MMC_ICONTENT_MEDIA_VIDEO_DIR "mmc1/video"
#endif // MIN_BREW_VERSION(3, 0)

#ifdef FEATURE_EFS_FTL
  /* This include should define AEEFS_FTL_DIR. */
/*lint -e322*/ /*Suppressing Lint Error: Unable to open include file 'AEECLASSID_FTL.BID'*/
  #include "AEECLASSID_FTL.BID"
/*lint +e322*/
  #ifndef AEEFS_FTL_DIR
    #define AEEFS_FTL_DIR "fs:/~0x0103cd43/"
  #endif
  #define MP_FTL_MEDIA_DIR  AEEFS_FTL_DIR"media"
#endif // FEATURE_EFS_FTL

// Icontent media directory
#ifdef MP_FEATURE_ICONTENTHANDLER
  #define MP_ICONTENT_MEDIA_DIR "fs:/shared/content/installed"
#endif//MP_FEATURE_ICONTENTHANDLER

// max root for explorer
#if (defined(FEATURE_EFS_FTL) && defined(MP_FEATURE_ICONTENTHANDLER))
  #define MP_MAX_ROOTDIRS        6       // maximum number of root dirs
#elif (defined(FEATURE_EFS_FTL) || defined(MP_FEATURE_ICONTENTHANDLER))
  #define MP_MAX_ROOTDIRS        5       // maximum number of root dirs
#else
  #define MP_MAX_ROOTDIRS        4       // maximum number of root dirs
#endif //(defined(FEATURE_EFS_FTL) && defined(MP_FEATURE_ICONTENTHANDLER))
                                 

#ifdef MP_FEATURE_MBMS
#define MP_MBMS_MEDIA_DIR MP_MEDIA_DIR"/mbms"
#endif

#define MEDIAPLAYER_CONFIG_FILE "mediaplayer.cfg"
#define MP_QCP_REC_FILE       "sample.qcp"
#define DOT_CHAR              '.'
#define MP_EXT_SEPARATOR      ", "

#define MP_SEEK_TIME           10000   // 10 seconds
#define MP_PROG_TIMER           2000
#define MP_BK_STEP_TIME         1000   // 1 seconds
#define MP_STEP_BK_FRAME_TIMER  50
#define MP_MAX_RETRY_COUNTS     22
#define MP_SEEK_TIME_DLG_INPUT_LENGTH 8 // allow 8 digits for inputting 
                                        // seek time in dlg

#ifdef FEATURE_QTV_MDP_ASCALE
#define MP_ASCALE_DEFAULT_VALUE    100  // 100% scaling = no scaling
#define MP_ASCALE_MIN_VALUE         25  // 25% scaling = one-quarter scaling
#define MP_ASCALE_MAX_VALUE        400  // 400% scaling = 4X scaling
#define MP_ASCALE_DLG_INPUT_LENGTH   3  // allow 3 digits for input (percentage)
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#define MP_HTTP_STARTUP_TIME    2000
#define MP_HTTP_BUFFERING_TIME  2000
#define MP_HTTP_DLG_INPUT_LENGTH 8     // allow 8 digits for input in
                                       // dlg
#endif     // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD


#if defined(FEATURE_QTV_RANDOM_ACCESS_REPOS) || defined(FEATURE_QTV_PLAYLIST)
#define MP_ACCESS_POINT_SKIP_STEP 1
#endif

#define MP_PROGBAR_DX_DIV     3        // dx is dx/3 of main rectangle
#define MP_PROGBAR_DY_DIV     2        // dy is dy/2 of main rectangle
#define MP_PROGBAR_DX_OFFSET  4        // dx is reduced by offset
#define MP_CLR_PROG_FILL      MAKE_RGB(0, 128, 192)

#ifdef FEATURE_MP4_LCD_CONTRAST
#define MP_CONTRAST_STEP 1
// The following contrast define logic was taken from apps/core/CoreUtils.c
#define CONTRAST_UNDEFINED 0
#if defined(T_MSM6550)
  //For MDDI displays, the driver defaults are correct.
  //TBD need to check this on an EBI-2 display, once it's working.
  #define CONTRAST_MIN  CONTRAST_UNDEFINED
  #define CONTRAST_MAX  CONTRAST_UNDEFINED
#elif defined(T_MSM6500)
#error code not present
#elif defined(T_MSM6280)
#error code not present
#elif defined(T_MSM6275)
#error code not present
#elif defined(T_MSM6250)
#error code not present
#else
#error code not present
#endif
#endif     // FEATURE_MP4_LCD_CONTRAST

#define EVT_CREATEMEDIA       EVT_USER

#define EVT_CREATEMEDIA_QCP   (EVT_USER + 1)
#define EVT_UPDATE_FILELIST   (EVT_USER + 2)

#ifdef FEATURE_APP_MPEG4
#define  EVT_CREATEMEDIA_EX   (EVT_USER + 3)
#endif     // FEATURE_APP_MPEG4

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#define EVT_CREATEPSEUDOMEDIA (EVT_USER + 4)
#endif
#endif

#ifdef FEATURE_MP_RECORDER_SETTINGS
#define EVT_CREATEMEDIA_PCM (EVT_USER + 5)
#endif

#ifdef FEATURE_QTV_FCS
#error code not present
#endif

#define EVT_START_FILELIST    (EVT_USER + 500)

// max. files in file list (MAX_UINT16 - EVT_START_FILELIST)
#define MP_MAX_FILE_COUNT         ((int)MAX_UINT16 - EVT_START_FILELIST)

#ifdef FEATURE_QTV_DIAG_CMD
#define MP_MAX_STRING_LENGTH     256
#define CLIPBOARD_NAME_MAX_LENGTH 15
#endif

#define MP_DEFAULT_NUM_VOL_LEVELS 4


#ifdef FEATURE_MULTISEQUENCER
#define MAX_NUM_SEQUENCES         4
#define PMD_EXTENSION "pmd"
#endif

#ifdef FEATURE_MP_RECORDER_SETTINGS
#define RECORDER_MAX_FILES       1000
#endif
/*
 * key press repeat timer for updating image ctrl 50ms
 */
#define KEY_KEY_REPEAT_TIME_MS   50
/*
 * key press repeat time for menu  200ms
 */
#define KEY_MENU_REPEAT_TIME_MS  200
/*
 * key press repeat time for icon menu  400ms
 */
#define KEY_ICON_MENU_REPEAT_TIME_MS  400

/*
 * max wait time when exiting from player win.  5s
 */
#define MAX_WAIT_TIME 5*1000 

#ifdef FEATURE_MP_HELP_SCREEN
#define MAX_HELP_STRING_LENGTH   512
#endif

#ifdef FEATURE_MMOVERLAY
#define MP_ALPHA_BLEND_MIN_VALUE        0    // Full blending
#define MP_ALPHA_BLEND_MAX_VALUE        256  // Off
#define MP_ALPHA_BLEND_HALF             128  // 50 %
#define MP_ALPHA_BLEND_OFF              MP_ALPHA_BLEND_MAX_VALUE  
#define MP_ALPHA_BLEND_DEFAULT          MP_ALPHA_BLEND_HALF
#define MP_ALPHA_BLEND_DLG_INPUT_LENGTH 3    // allow 3 digits for inputting alpha blend in dlg
                                            
#define MEDIAPLAYER_LAYER1_FILE "fs:/~/QImageOverlay1.bmp"
#define MEDIAPLAYER_LAYER2_FILE "fs:/~/QImageOverlay2.bmp"
#endif //FEATURE_MMOVERLAY

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/
typedef enum CContMode
{
  CONT_MODE_OFF,
  CONT_MODE_SINGLE_FILE,
  CONT_MODE_ALL_FILES
} CContMode;

typedef enum CVolumeLevel
{
  VOLUME_OFF,
  VOLUME_ONE,
  VOLUME_TWO,
  VOLUME_THREE,
  VOLUME_FOUR,
  VOLUME_FIVE,
  VOLUME_SIX,
  VOLUME_SEVEN,
  VOLUME_EIGHT,
  VOLUME_NINE,
  VOLUME_TEN
} CVolumeLevel;

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
typedef enum PSBWLevel
{
  PS_BW_LVL1,
  PS_BW_LVL2,
  PS_BW_LVL3,
  PS_BW_LVL4,
  PS_BW_LVL5,
  PS_BW_RANDOM,
  PS_BW_MAX
} PSBWLevel;
#endif //FEATURE_QTV_PSEUDO_STREAM) || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif //FEATURE_APP_MPEG4

typedef enum MP_RecorderSaveLocationType
{
  SAVELOC_FLASH,
  SAVELOC_MMC,
  SAVELOC_UIM
} MP_RecorderSaveLocationType;

typedef enum MPWindow
{
  MPW_NONE,
  MPW_MAIN,
  MPW_FILELIST,
  MPW_PLAYER,
#if defined(MEDIAPLAYER_SETTINGS)
  MPW_SETTINGS,
#endif     // defined(MEDIAPLAYER_SETTINGS)
#ifdef FEATURE_MP4_LCD_CONTRAST
  MPW_CONTRAST,
#endif     // FEATURE_MP4_LCD_CONTRAST
#if defined(FEATURE_MULTISEQUENCER)
  MPW_MULTISEQUENCER,
  MPW_SEQUENCE_PLAYER,
#endif
#ifdef FEATURE_MP_RECORDER_SETTINGS
  MPW_RECORDER_SETTINGS,
#endif
#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
  MPW_STREAM_RECORDER,
#endif
  MPW_LAST
} MPWindow;

typedef enum MPPlayerWin
{
  MPPW_PLAY,
  MPPW_RECORD,
  MPPW_IMAGE,
  MPPW_STREAM_RECORD
} MPPlayerWin;

#ifdef FEATURE_QTV_DIAG_CMD
typedef struct MPEventData
{
  char      m_inputString[MP_MAX_STRING_LENGTH];
} MPEventData;
#endif

typedef struct CMediaPlayer CMediaPlayer;
typedef struct CWindow CWindow;
typedef struct CMainWin CMainWin;
typedef struct CPlayerWin CPlayerWin;
typedef struct CProgCtl CProgCtl;
typedef struct stContent stContent;

#ifdef FEATURE_MULTISEQUENCER
typedef struct CSequencePlayerWin CSequencePlayerWin;
#endif

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
typedef struct C3GPPTTextCtl C3GPPTTextCtl;
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT

#ifdef FEATURE_MP4_LCD_CONTRAST
typedef struct CContrastWin CContrastWin;
#endif     // FEATURE_MP4_LCD_CONTRAST


#ifdef FEATURE_KEY_HOLD_SCROLL
typedef struct _CKeyStore
{
  IShell   *m_pIShell;
  IControl *m_pICtl;                   // base interface object of the
                                       // ctrl (IMenu, IImage ...)
  AEEEvent  m_eCode;                   // key code
  uint16    m_wParam_Key;              // wParam for key event
  uint32    m_dwParam_Key;             // dwParam for key event
  uint16    m_nTickTime;               // repeat interval
  boolean   m_bListMenu;               // TRUE, when IControl is menu
  boolean   m_bIconMenu;               // TRUE, when icon menu on
                                       // screen
} CKeyStore;
#endif

typedef enum CKeyMapping
{
  MPPW_KEYMAP_NONE = 0,          // Do Nothing
  MPPW_KEYMAP_FULLSCREEN,        // Fullscreen
  MPPW_KEYMAP_BK_FILE,           // Skip Back a File
  MPPW_KEYMAP_FW_FILE,           // Skip Forward a File
  MPPW_KEYMAP_PL_RINGER,         // Play Ringer
  MPPW_KEYMAP_ST_RINGER,         // Stop Ringer
  MPPW_KEYMAP_REWIND,            // Rewind
  MPPW_KEYMAP_FASTFWD,           // Fast Forward
  MPPW_KEYMAP_PLAYSTOP,          // Play/Stop
  MPPW_KEYMAP_PAUSE,             // Pause
  MPPW_KEYMAP_FF_STEP,           // Step Forward
  MPPW_KEYMAP_ZOOM_IN,           // Zoom In
  MPPW_KEYMAP_ZOOM_OUT,          // Zoom Out
  MPPW_KEYMAP_ROTATE,            // Rotate
  MPPW_KEYMAP_RECORD,            // Record Start/Stop
  MPPW_KEYMAP_RECSET_STRST,      // Recorder Settings/Stream Record Stop
  MPPW_KEYMAP_REPLAY_AT_STOP,    // Replay at Stop
  MPPW_KEYMAP_SAVEFILE,          // Save File
  MPPW_KEYMAP_FCS_SWITCH         // FCS Switch
} CKeyMapping;


#define INHERIT_CWindow(iname) \
   DECLARE_VTBL(iname) \
   CMediaPlayer *m_pOwner; \
   IShell *       m_pIShell; \
   IDisplay *     m_pIDisplay; \
   boolean         m_bActive

/*
 * Base class of all IWindow objects.
 */
struct CWindow
{
  INHERIT_CWindow(IWindow);
};

/*
 * Main window: Displays main menu.
 */
struct CMainWin
{
  INHERIT_CWindow(IWindow);

  IImage   *m_pLogo;
  AEERect   m_rectLogo;
  IMenuCtl *m_pMainMenu;  
  boolean   m_bAbout;
#ifdef FEATURE_MP_HELP_SCREEN
  boolean   m_bHelp;
  IStatic  *m_pHelp;
#endif
};

/*
 * Progress control: includes progress bar and the title
 */
struct CProgCtl
{
  IShell   *m_pIShell;
  IDisplay *m_pIDisplay;
  IStatic  *m_pTitle;
  AEERect   m_rectMain;
  AEERect   m_rectBar;
};

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
/*
 * 3GPP Timed Text control: includes timed text
 */
struct C3GPPTTextCtl
{
  IShell   *m_pIShell;
  IDisplay *m_pIDisplay;
  IStatic  *m_pTextCtl;
  AEERect   m_rectMain;
};
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT

#ifdef FEATURE_MP4_LCD_CONTRAST
struct CContrastWin
{
  INHERIT_CWindow(IWindow);
  AEERect   m_rectChart;
  IImage   *m_pChartImage;
  IStatic  *m_pContrastText;
};
#endif     // FEATURE_MP4_LCD_CONTRAST

/*
 * The following bit masks repesent various IMEDIA operations
 * that may be in progress.  Other masks may be added as needed.
 */
typedef enum CMediaBusy
{
  MEDIA_NOT_BUSY = 0x0000,        // Not busy      
  MEDIA_BUSY_PAUSE = 0x0001,      // Busy doing IMEDIA_Pause()
  MEDIA_BUSY_RESUME = 0x0002,     // Busy doing IMEDIA_Resume()
  MEDIA_BUSY_SEEK = 0x0004        // Busy doing IMEDIA_Seek() 
} CMediaBusy;

typedef void (*PlayerWinCB) (CPlayerWin *);

#ifdef FEATURE_MMOVERLAY
typedef struct
{
  AEERect rc;
  IBitmap *pBmp;
  RGBVAL  transparent;
  uint32  alpha;
  uint8   layer;
} OvData;
#endif //FEATURE_MMOVERLAY

typedef struct CPlayerWinCB
{
  /*
   * conditions
   */
  int       m_nCmd;                    // Command code
  int       m_nSubCmd;                 // Sub command code, if any or 0

  AEEMediaCmdNotify m_CmdNotify;       // notify status
  int       m_nCBCount;                // callback counter
  PlayerWinCB m_pFnPtr;                // user defined callback
                                       // function
} CPlayerWinCB;

/*
 * Player window: Plays the media.
 */
struct CPlayerWin
{
  INHERIT_CWindow(IWindow);

  MPPlayerWin m_eWin;                  // Window type

  IStatic  *m_pTitle;                  // Media title text
  AECHAR    m_szText[MP_MAX_STRLEN];
  AECHAR    m_szTimerText[MP_MAX_STRLEN];

  IImage   *m_pIdleImage;              // Image rect placeholder
  AEERect   m_rectIdleImage;           // Rect for the idle image
  AEERect   m_rectImage;               // Current image/video
                                       // rectangle.
                                       // Can change with media type
                                       // and playback mode.
  AEERect   m_dfltRectImage;           // Base image rect, no
                                       // modifiers.
                                       // This may not match rectIdleImage
                                       // if media type forces us to change
                                       // screen layout.  Independent
                                       // of playback mode.
  AEERect   m_rectTitle;               // Title rectangle
  AEERect   m_rectInfo;                // Info rectangle
                                       // Used by CProgCtl
  AEERect   m_rectMenu;                // Menu rectangle
                                       // Used by MenuCtl
  AEERect   m_rectSetups;              // Setups Window rectangle

#ifdef FEATURE_MMOVERLAY
  AEERect   m_rectLayer1;              // Layer1 Overlay rectangle
  AEERect   m_rectLayer2;              // Layer2 Overlay rectangle
  IBitmap  *m_pLayer1Bitmap;           // Layer1 bitmap
  IBitmap  *m_pLayer2Bitmap;           // Layer2 bitmap
#endif // FEATURE_MMOVERLAY

  char     *m_pszFile;                 // Saved file name alloc'd by
                                       // CMediaPlayer

  CMPProgCtl *m_pMPProgCtl;            // MPProgCtl object

  uint16    m_wPct;

  uint32*   m_pAudioSpec;              // audio spec infomation

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  C3GPPTTextCtl m_TTextCtl;            // 3GPPTTextCtl (also use for
                                       // Info display)
  uint32    m_dw3GPRemainTime;         // remaining text display time.
                                       // Used for pause/resume
#endif                                 // FEATURE_MP4_3GPP_TIMED_TEXT

  IStatic  *m_pInfoTextCtl;            // static text control to
                                       // display media information

  IMenuCtl *m_pPlayerMenu;

  AEEMediaData m_md;                   // Media data

  CPlayerWinCB m_CB;                   // callback from
                                       // CPlayerWin_MediaNotify

#ifdef FEATURE_APP_MPEG4
  AEEMediaDataEx *m_pmdExList;         // Extended Media data List
  int32     m_mdListCount;
  boolean   m_bLoopTrack;              // loop the tracks

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  AEEMediaDataBufferChunk m_bufferChunk;        // buffer chunk
  CHttpBlock *psHttpBlock;
#endif                                 // FEATURE_QTV_PSEUDO_STREAM ||
                                       // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  AEEMediaDataSetupTime m_setupTime;
#endif                                 // FEATURE_3GPP_PROGRESSIVE_DNLD
#endif                                 // FEATURE_APP_MPEG4
  IMedia   *m_pMedia;                  // IMedia-based object
  IMedia   *m_pMedia1;                 // another media ptr, used for recorder

  IImageCtl *m_pImageCtl;              // ImageCtl displaying the
                                       // selected image
  IImage   *m_pImage;                  // Image contained in ImageCtl
  boolean   m_bLoadingImage;           // The image should be fully decoded when exiting window.
                                       // If this flag is true, image is still in decoding.
                                       // It is set to false, when CPlayerWin_ImageNotify receives event.
  int       m_nWaitTimerCnt;           // when exiting from window, if image is decoding, this gives the cnt 
                                       // of timer

#ifdef FEATURE_APP_REAL
  AEEMediaRealSpec m_mediaRealInfo;    // informations about current
                                       // Real Media content
#endif
#ifdef FEATURE_APP_MPEG4
  AEEMediaMPEG4Spec m_mediaInfo;       // informations about current
                                       // media
  
  uint32    m_mp4PrevHeight;           // Dimensions of previous frame.  Used to
  uint32    m_mp4PrevWidth;            // determine when to clear screen due to
                                       // transforms
#endif     // FEATURE_APP_MPEG4
  uint32    m_dwDuration;              // Total clip duration (milliseconds)
  uint32    m_dwPlayPos;               // Current playback pos
#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)
  uint32    m_dwPositionAtStop;
#endif //#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  uint32    m_dwDownldPos;             // Current download pos
#ifdef MM_STATUS_DOWNLOAD_PERCENTAGE
  uint32    m_dwDownldPct;              // Current download percentage
#endif                                        
#endif
  uint16    m_wVolume;                 // Volume

  uint32    m_dwTickInterval;          // tick update interval

#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
#ifdef FEATURE_APP_MPEG4
  AEEMediaMPEG4ScalingType m_dwZoomScale;// Zoom Scaling factor 
  AEEMediaMPEG4RotationType m_Rotation;  // rotation
#endif 
  int       m_pendingZoomDirection;      // = 1 if in, -1 if out, 0 if no 
                                         // pending zoom 


#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS ||// FEATURE_QTV_MDP_TRANSFORMATIONS

  boolean   m_bImage;                 // = TRUE, if media contains
                                      // Video/Image component.
  boolean   m_bFullScreen;            // = TRUE, if Video/Image is
                                      // displayed full screen
  boolean   m_bProgTimer;             // = TRUE, if prog timer is
                                      // running. Regular display is
                                      // not updated.
  boolean   m_bStopPressed;
  boolean   m_bStoppedWhileConnecting;// TRUE = player stopped while in connecting state
  boolean   m_bNoTextOrHeader;        // TRUE, if there is no room for 
                                      // either the header and/or
                                      // text when displaying video.

  boolean   m_bMemoryFull;            // used in recorder win, set to
                                      // true when
                                      // MM_STATUS_SPACE_WARNING,
                                      // or MM_STATUS_SPACE_ERROR received.

  boolean   m_bDIsplayInfo;           // Media Info screen is ON.
  boolean   m_bDisplayInfoPending;    // Waiting for Media Info to
                                      // display

  boolean   m_bPlayPending;           // Play back is pending
  boolean   m_bGotMediaInfo;          // TRUE if player has received
                                      // the media info

  boolean   m_bIsCMXFile;             // = TRUE if we are playing a CMX
                                      // MediaFile, FALSE if QTV/Real
                                      // Not set and should not be used for images

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  boolean   m_b3GPPTTextOn;          // TRUE, if 3GPP TimedText is
                                       // ON.
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT
  int       m_seekPending;             // = 1, if reposition is in
                                       // progress
  int       m_pendingSeekOffset;       // time delta to reposition
  int       m_reseek;                  // = 1, if a reposition is
                                       // needed again
                                       // after the current reposition is done.
                                       // This happens when the user does a
                                       // reposition while a reposition is
                                       // already in progress.
  int       m_startSeekPosition;       // Playback position at the
                                       // start of a reposition
  boolean   m_skipSuccess;             // = TRUE when a successful skip happens
  int       m_currentClipIndex;        // Clip index in playlist
  int       m_nextClipOffsetAdj;       // Offset adjustment for the next clip in a playlist
  boolean   m_bAbortStringActive;      // indicates if an Error string is currently displayed

  boolean   m_skipPending;             // = TRUE, if skip reposition is in
                                       // progress
  int       m_skipOffset;              // increment/decrement delta of skip reposition
  boolean   m_reskip;                  // = TRUE, if a skip reposition is
                                       // needed again after the current skip is done.
                                       // This happens when the user does a skip
                                       // while a skip is already in progress.

#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#endif

  boolean    m_bResumeInProcessing;

  CMediaBusy m_MediaBusy;              // <>0 = Busy doing IMEDIA operation

#ifdef FEATURE_MMOVERLAY
  IMMOverlayMgr   *m_pIMMOverlayMgr;
  IMMOverlay      *m_pIMMOverlayTitle;
  IMMOverlay      *m_pIMMOverlayInfo;
  IMMOverlay      *m_pIMMOverlayMenu;
  IMMOverlay      *m_pIMMOverlaySetups;
  IMMOverlay      *m_pIMMOverlayImage;
  IMMOverlay      *m_pIMMOverlayLayer1;
  IMMOverlay      *m_pIMMOverlayLayer2;
#endif // FEATURE_MMOVERLAY

#ifdef MP_FEATURE_FAST_PLAYBACK
  int        m_ReplaySpeed;
  boolean    m_bChangeReplaySpeed;
#endif //MP_FEATURE_FAST_PLAYBACK

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  uint32     m_DualMonoOutput;         // TODO, what is default?
#endif //FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION

  boolean    m_bAutoPlayIsFirstClip;   //Only autoplay first clip 
}; // CPlayerWin


typedef struct CFileArray
{
  char      m_szName[MP_MAX_FILE_NAME]; // full path of the
                                       // file
  boolean   m_bIsFile;                 // file or directory
} CFileArray;

typedef struct CFileExplore
{
  int       m_nCurDepth;               // current directory depth level 
                                       // (from root)
  int       m_NumFiles;                // Number of files read into
                                       // file explorer 
  char      m_szRoots[MP_MAX_ROOTDIRS][MP_MAX_FILE_NAME];
  char      m_nCurDir[MP_MAX_FILE_NAME];       // full current
                                                // directory path
  CFileArray *m_FileArray;             // pointer to array of stucture
} CFileExplore;




/*
 * MediaPlayerState refers to the active window
 */
typedef enum MediaPlayerState
{
  MP_STATE_NULL,
  MP_STATE_UNKNOWN,
  MP_STATE_START_SUSPEND,
  MP_STATE_START_RESUME,
  MP_STATE_BROWSE_FILE,
  MP_STATE_MAIN_WIN,
  MP_STATE_PLAYER_WIN,
  MP_STATE_RECORDER_WIN,
  MP_STATE_IMAGE_WIN,
  MP_STATE_SEQUENCER_WIN,
  MP_STATE_SETTING,
  MP_STATE_HELP_WIN,
  MP_STATE_STREAM_RECORD_WIN
} MediaPlayerState;

#if defined(FEATURE_APP_MPEG4) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))
typedef struct MPNetPolicyInfo
{
  boolean          m_bQoSEnable;      // if Qos Enabled
  boolean          m_bDefaultProfile; // if current PDP is default PDP
  uint8            m_nProfile;        // selected profile number
  int              m_nNumPDP;         // PDP profile count
} MPNetPolicyInfo;
#endif


/*
 * MediaPlayer error state indicates if MediaPlayer is in an error
 * state or not.  Specific types of error states can be added here.
 */
typedef enum MediaPlayerErrorState
{
  MP_ERRORSTATE_NONE,     /* no error */
  MP_ERRORSTATE_ERROR     /* "general" error */
}MediaPlayerErrorState;

#ifdef FEATURE_MULTISEQUENCER
typedef struct
{
  char      m_szSequenceFiles[MAX_NUM_SEQUENCES][MP_MAX_FILE_NAME];
  uint16    m_wNumSequences;
} CMultiSequencerFiles;
#endif //FEATURE_MULTISEQUENCER

/*
 * MediaPlayer app global structure.
 * Note: m_bPlugin == TRUE indicates that MediaPlayer tries to
 * play the file directly in CPlayerWin. Pressing CLR will close
 * the app.
 */
struct CMediaPlayer
{
  AEEApplet a;

  int       m_cxWidth;
  int       m_cyHeight;
  uint16    m_nColorDepth;
  int       m_nNChSize;                // Large char size
  int       m_nLChSize;                // Normal char size

  MPWindow  m_eActiveWin;              // Active window
  MPWindow  m_eSuspendWin;             // Suspended window, when true,
                                       // CPlayerWin_MediaNotify will
                                       // reject all events

  IAnnunciator        *m_pIAnn;         //Add By zzg 2010_08_09
                                       
  IWindow  *m_pWin;
  uint16   m_nPlayerType;              // The MenuID of the player that was selected;

  MediaPlayerState m_State;            // window state (major)
  MediaPlayerPlayState m_playState;    // playing state (minor)
  MediaPlayerErrorState m_ErrorState;   // flag to indicate that an error has been received by MediaNotify
                                       // this has to be cleaned manually

  uint16    m_wMainWin;                // CurSel of CMainWin
  uint16    m_wFileListWin;            // CurSel of CFileListWin

  char     *m_pszAudioExt;             // Registered audio extension
                                       // string: "mid, mp3, ..."
  char     *m_pszVideoExt;             // Registered video extension
                                       // string: "pmd, ..."
  char     *m_pszImageExt;             // Registered image extension
                                       // string: "bmp, png, ..."

  AEECallback m_cbRedraw;
  boolean   m_bRedraw;                 // Processing redraw

  boolean   m_bPlugin;                 // = TRUE, if MediaPlayer is in
                                       // Plugin mode.

  boolean   m_bShowLogo;               // display the logo in PlayerWin only when media is audio only
  boolean   m_bShowPlayerWinSettingUI; // show/hide audio setting screen. 

  boolean   m_bDeleteFile;             // Flag indicates if file selected to be deleted or played
  boolean   m_bAutoPlay;
  int       m_nContMode;

  uint16    m_VolumeStep;              // The amount of change for the volume when the user changes it

  boolean   m_bDisableAutoPlayOnce;    // diable auto play once, this is for suspend/resume
  boolean   m_bResumePending;          // is RESUME pending?
  boolean   m_bNeedReload;             // need to reopen file after
                                       // EVT_APP_RESUME? 
  boolean   m_bResumeAnalyzer;         // Enable spectrum analyzer on resume (incall)?
  boolean   m_bIncallMode;             // TRUE if suspended with incall mode
  boolean   m_bPlayAfterResume;        // state after EVT_APP_RESUME
  char      m_resumeFileName[MP_MAX_FILE_NAME]; // file to play after
                                                 // EVT_APP_RESUME
  int16     m_wSuspendVolume;          // volume when suspending
  int32     m_resumePlayPos;           // resume position after
                                       // EVT_APP_RESUME
  boolean   m_bSuspend;                // TRUE when suspended
  boolean   m_bPlayNextFileOnResume;   // replay next file when resuming
  int       m_bReplaySameFile;         // on resuming, replay same file if cont. mode is on
  /*
   * m_bSavedContMode is used to save continuous mode setting before entering
   * plug-in mode. m_nContMode will be set to m_bSavedContMode just before
   * exiting plug-in mode of operation
   *
   * Note: In plug-in mode of operation, continuous mode is always turned off. 
   */

  int       m_bSavedContMode;
#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
  boolean   m_bRTSPstreamRec;          // TRUE, if RTSP Stream record
                                       // is set to TRUE

  /*
   * True, if it is ok to overwrite an existing file
   */
  boolean   m_bRTSPstreamRecFileOverwrite;

#endif
  uint16    m_wVolumeLevel;
  uint16    m_wNumVolumeLevels;

#ifdef FEATURE_APP_MPEG4
  boolean   m_bQtvSuspendPause;
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  int       m_bPSbwLevel;
  boolean   m_bKddiExt;
  boolean   m_bEnablePSSimulation;
  boolean   m_bFileWriteReadTest;
  boolean   m_bDownloadNotDone;
  boolean   m_bVariableRate;
  // ReconnectWebOnResume may be able to be replaced with some combination of DownloadNotDone and other flags
  boolean   m_bReconnectWebOnResume;
  unsigned int m_nDownloadPendingSize;
  unsigned int m_nDownloadCopiedSize;
  unsigned int m_nDownloadCopyBytes;
  unsigned int m_nChunkSize;
  char      *m_downloadFileBuffer;
  IFile     *m_pPseudoStrIFile;
  IFileMgr  *m_pPSIFileMgr;
  IFileMgr  *m_pIFileMgr2;
#endif
#endif

#if defined(FEATURE_APP_MPEG4) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))
  MPNetPolicyInfo    m_NetPolicy;
#endif

  int       i;
  boolean   m_bIsImage;

  IImage   *m_pIRadioOn;
  IImage   *m_pIRadioOff;

  CFileExplore m_FileExplorer;
  int       m_CurFileIndex;

  int       nSuffix;                   // Suffix for creating voice
                                       // memo in order.

#ifdef FEATURE_KEY_HOLD_SCROLL
  CKeyStore m_KeyStore;
#endif

#ifdef FEATURE_MP4_LCD_CONTRAST
  word      m_dispContrast;
#endif     // FEATURE_MP4_LCD_CONTRAST
#ifdef FEATURE_MULTISEQUENCER
  boolean   m_bIsMultiSequencer;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
#endif
#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
  boolean   m_bIsStreamRecord;      // Is this the stream recording window?
#endif

#ifdef FEATURE_MP_RECORDER_SETTINGS
  uint16    m_AudioFormat;
  uint16    m_SampleRate;
  uint16    m_AudioChannel;
  byte      m_RecordPath;
  uint16    m_SaveLocation;
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

  int32     m_nSeekTime;

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  AEEMediaDataSetupTime m_defaultSetupTime;
  uint32    m_maxDownloadFileSize;
  ITextCtl *m_pActiveCtrl;
#endif 

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
  boolean   m_bPacketVideoStreaming;   // TRUE if packet video, FALSE otherwise
#endif

  boolean   m_bVideoPlaybackReady;     // TRUE after MM_MP4_STATUS_PB_READY or MMV_STATUS_PLAYBACK_READY received

  IGraphics *      m_pIGraphics;
#ifdef FEATURE_MP_EQ_UI
  MPSetting*       m_pSetting;        //  setting object
#endif

  int              m_IconViewCtlDy;  //height of the playermenu icons
  boolean          m_bSmallScreenSize; //true if smaller screen size is in effect
                                       //false otherwrise
#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
  stContent*       m_pContent;
  boolean          m_bIContentMode;    //TRUE, when using icontent DB
  int32            m_IContentResumePos;
  boolean          m_bIContentResumePlay;
  ISource*         m_pContentISource;
#endif //(defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

#ifdef FEATURE_MULTISEQUENCER
  CMultiSequencerFiles m_MSFiles;
#endif

  boolean          m_bPlayFromBuffer;         //If true read the media file into a buffer
                                              //and then play
  boolean          m_bPlayAudioInBackground;  //If True, then continue to play audio files
                                              //in the background if player win suspended/gone
  boolean          m_bBackgroundMode;         //MediaPlayer is in background mode

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  boolean          m_bPvxSavetoFlash;         //If true, the pvx file play location is flash
  boolean          m_bProgDLFileSaveLoc;      //If true, the prog dl file will be saved to flash, else to MMC
#endif

  uint16           m_nAudioPanValue;          //value for CMX audio panning (0 to MM_MAX_PAN)
  boolean          m_bRedrawScreenAtClipEnd;  //If True (default), then redraw the player window
                                              //when the clip is finished playing (MM_STATUS_DONE).

#ifdef FEATURE_QTV_MDP_ASCALE
  int              m_nAScaleXValue;
  int              m_nAScaleYValue;
#endif

  boolean          m_bShowAudioSpec;          // display audio spec info

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
  boolean          m_bCallQtvProgDnld;        //If True (default), then use QTV for HTTP file transfer
  boolean          m_bIsURLFile;              //If true, then clip file ends in ".url"
  CMimicEncryptDnld m_MimicEncryptDnld;      
#endif

#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
  boolean          m_bPlayRingerWithQTV;      //Play Ringer and QTV clip 
  boolean          m_bPlayRingerNoQTV;        //Play Ringer without playing QTV clip
#endif

  boolean   m_bIsHTTP;                        //If true, then playing an HTTP clip
#ifdef FEATURE_QTV_FCS
#error code not present
#endif

  boolean   m_bResumeInProcessing;
  boolean   m_bSuspendPending;

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
  AEEMediaMPEG4ScalingType m_dwLastZoomVal;     // last zoom 
  AEEMediaMPEG4RotationType m_LastRotationVal;  // last rotation
#endif //defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
#endif 

  int       m_cxWidthFull;
  int       m_cyHeightFull;

  boolean m_bInTestAutomation;

  boolean m_bShowExtension;

#ifdef FEATURE_QTV_GENERIC_BCAST
  AEETrackList*    m_pTrackList;     // track list
  int              m_nTrackCount;    // total track count
  uint32           m_TransId;        // Return Id for Reposition and PlaySyncFrame ops
#endif //FEATURE_QTV_GENERIC_BCAST

#ifdef FEATURE_DTV_DVBH
  int32            m_DescramblerKey;
#endif //FEATURE_DTV_DVBH

#ifdef FEATURE_MMOVERLAY
  boolean   m_bOverlays;             // TRUE = overlays are enabled
  boolean   m_bFrameCallbacks;       // TRUE = frame callbacks are enabled
  int       m_nAlphaBlend;           // Alpha blending setting (0-256)
                                     // 0 = off;  256 = 100% blending
#endif // FEATURE_MMOVERLAY

#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)
  boolean m_bReplayAtStop;
  char    m_strReplayFileName[MP_MAX_FILE_NAME]; // last replayed file name
#endif //#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)

  boolean   m_bHighPerformanceMode;  // TRUE = High Performance Mode is enabled
                                     //        Progress bar is not updated while playing 
  IBitmap  *m_pRecIcon;              // Stream Recording ON Icon

  CMPDlg   *m_pDlg;                  // info dlg

  boolean   m_bCallInProgress;       // TRUE = In the middle of call, used for incall mix processing

  boolean   m_bLocalPlayPullMode;    // TRUE = Local Play PULL Mode is ON

  char      m_szFullPath[MP_MAX_FILE_NAME];
  char      m_szBuf[32];
  int       m_nFileSize;
  word      m_fsHandle;
}; // CMediaPlayer

#ifdef FEATURE_MULTISEQUENCER
/*
 * Sequence Player window: Plays Media Sequence
 */

typedef struct
{
  IStatic  *m_pTitle;                  // Media title text
  AECHAR    m_szText[MP_MAX_STRLEN];
  AEERect   m_rectIdleImage;           // Rect for the idle image
  AEERect   m_rectImage;               // Current image/video
                                       // rectangle.
                                       // Can change with media type
                                       // and playback mode.
  AEERect   m_dfltRectImage;           // Base image rect, no
                                       // modifiers.
                                       // This may not match rectIdleImage
                                       // if media type forces us to change
                                       // screen layout.  Independent
                                       // of playback mode.

  CProgCtl  m_ProgCtl;                 // ProgCtl
  uint16    m_wPct;
  uint32    m_dwDuration;              // Total clip duration (milliseconds)
  uint32    m_dwPlayPos;               // Current playback pos

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2                                       
  uint32    m_dwDownldPos;             // Current download pos
#endif                                        
  char     *m_pszFile;                 // File name
  AEEMediaData m_md;                   // Media data
  IMedia   *m_pMedia;                  // Media object
  boolean   m_bPlay;                 // = TRUE, if Playback progress
                                       // flag
  boolean   m_bImage;                // = TRUE, if media contains
                                       // Video/Image component.
  boolean   m_bFullScreen;           // = TRUE, if Video/Image is
                                       // displayed full screen
  boolean   m_bProgTimer;            // = TRUE, if prog timer is
                                       // running. Regular display is
                                       // not updated.
} CSequenceFile;

struct CSequencePlayerWin
{
  INHERIT_CWindow(IWindow);
  IImage   *m_pIdleImage;              // Image rect placeholder
  CSequenceFile m_sSequenceFile[MAX_NUM_SEQUENCES];
  IMenuCtl *m_pPlayerMenu;
  uint16    m_wVolume;                 // Volume
  uint16    m_wCurFile;                // Index of file to control
  uint16    m_wCurMedia;               // Current Media object
  boolean   m_bControllAll;          // Control all sequences
};

#endif

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
boolean MPCreatePlayerWin(CMediaPlayer *pme, void* pObject, MPPlayerWin type, char* pszFile);
boolean MPContentPlayNext(CMediaPlayer *pme);
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

boolean CWindow_ProcessEnable(IWindow *po, boolean bEnable);

IWindow *CWindow_New(int16 nSize, CMediaPlayer *pOwner, VTBL(IWindow) * pvt);
void MP_SetMenuAttr(IMenuCtl *pMenu, AEECLSID clsMenu, uint16 nColorDepth, AEERect *pRect, uint32 dwProps);
void MP_FreeWin(IWindow ** ppif);

uint16 CMediaPlayer_FindHandlerType(CMediaPlayer *pme, const char *pszFile);
boolean CMediaPlayer_GetMIMETypeString(CMediaPlayer *pme, const char *pszFile, int size, char* pMIME);

boolean CMediaPlayer_SetWindow(CMediaPlayer *pme, MPWindow eWin, uint32 dwParam);

boolean MP_AddMenuItem(IMenuCtl *pMenu, uint16 wTextID, AECHAR *pText, uint16 wImageID, uint16 wItemID, uint32 dwData);


void CMediaPlayer_ProcessSuspendEvent(CMediaPlayer *pme);
void CMediaPlayer_ProcessResumeEvent(CMediaPlayer *pme);

char *MP_MediaPlayerStateEnumToString(enum MediaPlayerState mpState);
char *MP_MediaPlayerPlayStateEnumToString(enum MediaPlayerPlayState mpPlayState);

char *MP_GetFileName(const char *psz);
char *MP_GetFileNameNoExtension(const char *psz, char *pszOut, int32 len);
char *MP_GetFileNameExtension(const char *pszFileName);
boolean MP_InsertStringToFileName(char* pSrc, char*pString, char* pDest, int nDestSize);

void MP_ErrorDlg(CMediaPlayer *pme, uint16 wResErrID);
void MPDlgButtonHandler_OK(CMediaPlayer* pme);

uint16 MP_GetClipWidth(CPlayerWin *pwin);
uint16 MP_GetClipHeight(CPlayerWin *pwin);

#ifdef FEATURE_KEY_HOLD_SCROLL
int MP_HandleKeyEventRepeat(IWindow *po, AEEEvent eCode, uint16 wParam, uint32 dwParam, IMenuCtl *pICtl, uint16 wTime);
#endif

#ifdef FEATURE_MULTISEQUENCER
void CMediaPlayer_PlayMultiSequence(CMediaPlayer *pme);
#endif

void    CMediaPlayer_OnSelectFile(CMediaPlayer *pme, const char* szPath);
uint16 CMediaPlayer_IsExtension(CMediaPlayer *pme, const char *pszFile, char *pszInExt, int nExtLen);
boolean CPlayerWin_IsVideoMedia(CPlayerWin *pme);

void MP_SequencePlayerWin_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify);
void MP_FreeIF(IBase ** ppif);
void MP_FreeBitmap(IBitmap ** pp);
void MP_FreeMenu(CMediaPlayer * pme, IBase ** ppif);

boolean CPlayerWin_IsPlaying(CPlayerWin *pme);

#ifdef FEATURE_MMOVERLAY
void MP_FreeOverlay(IMMOverlay ** ppif);
void MP_FreeOverlayMgr(IMMOverlayMgr ** ppif);
#endif // FEATURE_MMOVERLAY

#define MP_RELEASEIF(p)                MP_FreeIF((IBase **)&(p))
#define MP_RELEASEWIN(p)               MP_FreeWin((IWindow **)&(p))
#define MP_RELEASEBITMAP(p)            MP_FreeBitmap((IBitmap **)&(p))
#define MP_RELEASEMENU(m,p)            MP_FreeMenu((m),(IBase **)&(p))
#define MP_WINERR_RETURN(p)            { MP_RELEASEWIN(p); return NULL; }

#ifdef FEATURE_MMOVERLAY
#define MP_RELEASEOVERLAY(p)           MP_FreeOverlay((IMMOverlay **)&(p))
#define MP_RELEASEOVERLAYMGR(p)        MP_FreeOverlayMgr((IMMOverlayMgr **)&(p))
#endif // FEATURE_MMOVERLAY

#define EMIT_MEDIAPLAYER_STATE(x) DBGPRINTF_MED("MP: MediaPlayer state is %s\n", MP_MediaPlayerStateEnumToString(x));
#define EMIT_MEDIAPLAYER_PLAYSTATE(x) DBGPRINTF_MED("MP: MediaPlayer play state is %s\n", MP_MediaPlayerPlayStateEnumToString(x));

#endif /*  MEDIAPLAYER_H */


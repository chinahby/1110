/*===========================================================================

FILE: MediaPlayer.c

SERVICES: Sample Media Player applet showing usage of BREW IMedia interfaces.

DESCRIPTION
  This file contains the implementation of a MediaPlayer applet using the
  AEE IMedia interface services.
  MediaPlayer app
  (1) Plays MIDI, MP3, QCP and PMD media formats
  (2) Records QCP file
  (3) Displays BMP, BCI, PNG and JPEG image formats
  (4) Provides playback/record controls: stop, rewind, ff, pause, resume.
  (5) Allows image panning for images
  (6) Provides volume controls
  (7) Can display images/video in Full Screen mode
  (8) Can be started either in stand-alone mode or as a plugin

  Demonstrates usage of
  (1) IMedia API
  (2) AEEMediaUtil_CreateMedia()
  (3) IImageCtl

       Copyright (c)2000-2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/MediaPlayer/MediaPlayer.c#867 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/05   drl     Changed FEATURE_COMMON_MM_DIR to be consistant with other
                   multi-media apps (media player and qcamcorder).
12/16/04   qb      Look for associated decryption key if MFDRM feature in use
06/10/04   sun     Check the current status before running files in  continuous mode
03/04/04   sun     Play the wave file also at the current volume
09/25/03    lz     Add MultiSequencer support.
09/07/03   igt     Added support for qtv events
07/08/03   ska     previous media info cleared for continuous mode playback
06/17/03   ska     CCW rotation removed for lack of softkey bar space
05/30/03   kar     Mainlined MMC support
05/30/03   kar     Fixed compiler warnings
05/28/03   kar     Fixed compile error for non MP4 targets
05/27/03   san     MP4 changes
02/12/03   NNG     Added function CMediaPlayer_Initialization for gracefully existing
                   the app when bar file is not present.
02/02/03   lz      Fix compile problem due to ARRAY_SIZE macro.
02/02/03   lz      Included only OEMFeatures.h instead of customer.h, target.h and comdef.h
01/20/03   NNG     Added check for bar file existence. Included edit history.
===========================================================================*/


/*===========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/
#include "target.h"                    // Target configuration file
#include "customer.h"                  // Customer configuration file
#include "msg.h"
#include "fs.h"

#include "BREWVersion.h"
#if MIN_BREW_BUILD(4,0,1,5)
#include "AEEConstFile.h"
#else
#include "AEE_OEMFile.h"
#endif


#ifdef FEATURE_APP_MEDIAPLAYER

// if there is any static allocated in header file, create in this module
#define MP_ALLOCATE_HERE
#include "MediaPlayer.h"
#undef MP_ALLOCATE_HERE

#include "MPIO.h"
#include "MPFileListWin.h"

#include "MPEqualizerData.h"
#include "MPConfigFile.h"


#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
#include "MPContent.h"
#include "MPContentHandler.h"
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

#ifdef FEATURE_QTV_BCAST_FLO
#error code not present
#endif //FEATURE_QTV_BCAST_FLO

// Simulation of QoS selection on LTK -- no media player support yet.
#ifdef PLATFORM_LTK
#error code not present
#endif //PLATFORM_LTK

#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#endif //FEATURE_APP_MEDIA_VIDEO

#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
#include "uixsnd.h"
#endif //(defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))

#ifdef FEATURE_QTV_PDP_SELECTION
#include "PDP.BID"
#endif //FEATURE_QTV_PDP_SELECTION

#include "snd.h"

#ifdef FEATURE_MMOVERLAY
#include "AEECLSID_MMOVERLAYMGR.BID"
#endif // FEATURE_MMOVERLAY

#ifndef MP_LTK_OEMEFS_CONDITIONAL
#if MIN_BREW_VERSION(4,0)
#include "OEMefs.h"
#else
#include "OEMFS.h"
#endif //MIN_BREW_VERSION(4,0)

#endif // #ifndef MP_LTK_OEMEFS_CONDITIONAL

/*===========================================================================
                      INTERFACE DEFINITION
===========================================================================*/

/*===========================================================================
                     MACROS
===========================================================================*/
#define MP_ISPLUGIN(p)                 ((p)->m_pOwner->m_bPlugin)


#define CMediaPlayer_CancelRedraw(p)   { CALLBACK_Cancel(&(p)->m_cbRedraw); (p)->m_bRedraw = FALSE; }
#define CMediaPlayer_DisableWin(p)     { IWINDOW_Disable((p)->m_pWin); CMediaPlayer_CancelRedraw(p); }

#define CPlayerWin_IsImage(p)          ((p)->m_eWin == MPPW_IMAGE)
#define CPlayerWin_IsMedia(p)          ((p)->m_eWin == MPPW_PLAY || (p)->m_eWin == MPPW_RECORD || (p)->m_eWin == MPPW_STREAM_RECORD)

// Following is a kludge to provide backwards compatibility with new EVENT_MEDIA_PLAYER_KEYPRESS event
#define MP_KEYPRESS_EVENT_ID           1345  // value of EVENT_MEDIA_PLAYER_KEYPRESS enum in event_defs.h
#define MP_EventReportPayload(id, payload)                                   \
        if ((id) < (int)EVENT_NEXT_UNUSED_EVENT)                             \
        {                                                                    \
          event_report_payload((event_id_enum_type)(id), sizeof((payload)), (void *)&(payload)); \
        }
#define MP_EventReport(id)                                                   \
        if ((id) < (int)EVENT_NEXT_UNUSED_EVENT)                             \
        {                                                                    \
          event_report((event_id_enum_type)(id));                                                \
        }

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
/*
 * This global variable should be set to true by OEM if it wants Qtv to
 * release the DSP with the subsequent call to Qtv pause.
 */
extern boolean qtv_cfg_enable_dsp_release;
#ifdef MP_USE_NEWER_BUILD_ITEMS
extern boolean qtv_cfg_enable_dsp_release_oem;
#endif //MP_USE_NEWER_BUILD_ITEMS

/*
 * The state of the player before the incoming voice call
 */
extern boolean playingBeforeVoiceCall;

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif // FEATURE_QTV_QOS_SELECTION

#endif // FEATURE_QTV_QDSP_RELEASE_RESTORE

#ifdef FEATURE_REAL_QDSP_RELEASE_RESTORE
extern boolean real_cfg_enable_dsp_release;
#endif // FEATURE_REAL_QDSP_RELEASE_RESTORE

AEERect NULL_RECT = { 0, 0, 0, 0 };

#ifdef FEATURE_MMOVERLAY
#define MP_MMOVERLAY_LAYER1  0x1d
#define MP_MMOVERLAY_LAYER2  OV_LAYER_TOP_LAYER //0xff
#endif // FEATURE_MMOVERLAY

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/


/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/
#define MP_ICONTENT_CAT_NAME_MEMO     "memo"
#define MP_ICONTENT_CAT_DESC_MEMO     "mp memo file"

#define MP_ICONTENT_CAT_NAMR_RECORD   "record"
#define MP_ICONTENT_CAT_DESC_RECORD   "mp streaming file"

#define MP_MENU_DEFAULT_SELECTION     -1

#if MIN_BREW_VERSION(4,0)
#define MP_PATH_MEDIA                 "/mod/mediaplayer/media/"
#define MP_PATH_SAVELOC_FLASH         "/mod/mediaplayer/media/pds2.mp4"
#else
#define MP_PATH_MEDIA                 "/brew/mod/mediaplayer/media/"
#define MP_PATH_SAVELOC_FLASH         "/brew/mod/mediaplayer/media/pds2.mp4"
#endif //MIN_BREW_VERSION(4,0)
#define MP_PATH_SAVELOC_MMC           "/mmc1/media/pds2.mp4"

#if MIN_BREW_VERSION(4, 0)
#define MP_PATH_RECORDLOC_FLASH       "mod/mediaplayer/media/"
#else 
#if MIN_BREW_VERSION(3, 0)
#define MP_PATH_RECORDLOC_FLASH       "brew/mod/mediaplayer/media/"
#else
#define MP_PATH_RECORDLOC_FLASH       "brew/media/"
#endif //MIN_BREW_VERSION(3, 0)
#endif //MIN_BREW_VERSION(4, 0)

#define MP_MIME_SIZE                  32

//-------------------------------------------------------------------


/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/

#ifdef FEATURE_MP_RECORDER_SETTINGS
typedef struct _MP_SIZE_16
{
  int16     cx;
  int16     cy;
} MP_Size_16;

MP_Size_16 RecorderAudioSettings[] =
{
  {IDM_13K_FIXED_FULL, MM_QCP_FORMAT_FIXED_FULL_13K},
  {IDM_13K_VARIABLE_FULL, MM_QCP_FORMAT_VAR_FULL_13K},
  {IDM_13K_FIXED_HALF, MM_QCP_FORMAT_FIXED_HALF_13K},
  {IDM_13K_VARIABLE_HALF, MM_QCP_FORMAT_VAR_HALF_13K},
  {IDM_EVRC_FIXED_FULL, MM_QCP_FORMAT_FIXED_FULL_EVRC},
  {IDM_EVRC_VARIABLE_FULL, MM_QCP_FORMAT_EVRC},
#ifdef FEATURE_AUDFMT_EVB
  {IDM_EVRCNB, MP_EVRCNB_RECORDING},
#endif // FEATURE_AUDFMT_EVB
  {IDM_8K_PCM, MP_8K_PCM_RECORDING}
#ifdef FEATURE_AAC_REC
  ,
  {IDM_AAC, MP_AAC_RECORDING}
#endif //FEATURE_AAC_REC
#ifdef FEATURE_AUDFMT_AMR
  ,
  {IDM_AMR_475, MM_AMR_FORMAT_FIXED_4_75_AMR},
  {IDM_AMR_515, MM_AMR_FORMAT_FIXED_5_15_AMR},
  {IDM_AMR_59, MM_AMR_FORMAT_FIXED_5_9_AMR},
  {IDM_AMR_67, MM_AMR_FORMAT_FIXED_6_7_AMR},
  {IDM_AMR_74, MM_AMR_FORMAT_FIXED_7_4_AMR},
  {IDM_AMR_795, MM_AMR_FORMAT_FIXED_7_95_AMR},
  {IDM_AMR_102, MM_AMR_FORMAT_FIXED_10_2_AMR},
  {IDM_AMR_122, MM_AMR_FORMAT_FIXED_12_2_AMR}
#endif // FEATURE_AUDFMT_AMR
};

MP_Size_16 RecorderPathSettings[] =
{
  {IDM_RECORD_UP_LINK, MM_APATH_LOCAL},
  {IDM_RECORD_DOWN_LINK, MM_APATH_REMOTE},
  {IDM_RECORD_BOTH, MM_APATH_BOTH}
};

MP_Size_16 RecorderSampleRateSettings[] =
{
  {IDM_AUDIO_SAMPLE_RATE_8000,    CMX_AF_SAMPLE_RATE_8000}
#if defined(FEATURE_STEREO_DAC)
  ,
  {IDM_AUDIO_SAMPLE_RATE_11025,   CMX_AF_SAMPLE_RATE_11025},
  {IDM_AUDIO_SAMPLE_RATE_12000,   CMX_AF_SAMPLE_RATE_12000},
  {IDM_AUDIO_SAMPLE_RATE_16000,   CMX_AF_SAMPLE_RATE_16000},
  {IDM_AUDIO_SAMPLE_RATE_22050,   CMX_AF_SAMPLE_RATE_22050},
  {IDM_AUDIO_SAMPLE_RATE_24000,   CMX_AF_SAMPLE_RATE_24000},
  {IDM_AUDIO_SAMPLE_RATE_32000,   CMX_AF_SAMPLE_RATE_32000},
  {IDM_AUDIO_SAMPLE_RATE_44100,   CMX_AF_SAMPLE_RATE_44100},
  {IDM_AUDIO_SAMPLE_RATE_48000,   CMX_AF_SAMPLE_RATE_48000}
#ifdef FEATURE_AAC_REC
  ,
  {IDM_AUDIO_SAMPLE_RATE_64000,   CMX_AF_SAMPLE_RATE_64000},
  {IDM_AUDIO_SAMPLE_RATE_88200,   CMX_AF_SAMPLE_RATE_88200},
  {IDM_AUDIO_SAMPLE_RATE_96000,   CMX_AF_SAMPLE_RATE_96000}
#endif //FEATURE_AAC_REC
#endif //FEATURE_STEREO_DAC
};

MP_Size_16 RecorderChannelSettings[] =
{
  {IDM_RECORD_CHANNEL_MONO,       CMX_AF_AAC_CHANNEL_MONO},
  {IDM_RECORD_CHANNEL_DUAL,       CMX_AF_AAC_CHANNEL_DUAL},
  {IDM_RECORD_CHANNEL_TRIPLE,     CMX_AF_AAC_CHANNEL_TRIPLE},
  {IDM_RECORD_CHANNEL_QUAD,       CMX_AF_AAC_CHANNEL_QUAD},
  {IDM_RECORD_CHANNEL_QUINTUPLE,  CMX_AF_AAC_CHANNEL_QUINTUPLE},
  {IDM_RECORD_CHANNEL_SEXTUPLE,   CMX_AF_AAC_CHANNEL_SEXTUPLE},
  {IDM_RECORD_CHANNEL_OCTUPLE,    CMX_AF_AAC_CHANNEL_OCTUPLE}
};

MP_Size_16 RecorderSaveLocation[] =
{
  {IDM_FLASH, SAVELOC_FLASH},
  {IDM_MMC, SAVELOC_MMC},
  {IDM_UIM_STORAGE, SAVELOC_UIM}
};
#endif //FEATURE_MP_RECORDER_SETTINGS

#ifdef FEATURE_MP_RECORDER_SETTINGS
typedef struct _CRecorderSettingWin
{
  INHERIT_CWindow(IWindow);
  IMenuCtl *m_pMenu;
  IMenuCtl *m_pSubMenu;
  int16     m_ParentWin;
} CRecorderSettingWin;
#endif //FEATURE_MP_RECORDER_SETTINGS

/*
 * The allowed multisequencer extensions are only ".mid, .mmf, .spf, .pmd and .xmf".
 * Note: PHR = SMAF phrase files = extension SPF.
*/
#ifdef FEATURE_MULTISEQUENCER
static char *allowedExts[] = { MIDI_EXTENSION,
                               MMF_EXTENSION,
                               PHR_EXTENSION,
                               PMD_EXTENSION,
                               "xmf",  /* no #define found for xmf in AEEMimeTypes.h  */
                               ""      /* empty string marks end of this string table */
                               };
#endif //FEATURE_MULTISEQUENCER

/* 0-9, Star, Pound */
#define MPPW_KEYMAPPING_MAX  12
CKeyMapping MPPW_KeyMappings[MPPW_KEYMAPPING_MAX] = { 
    MPPW_KEYMAP_FULLSCREEN,        // 0
#if defined(FEATURE_DTV_DVBH) || defined(MP_FEATURE_MBMS)
    MPPW_KEYMAP_BK_FILE,           // 1
#else
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
    MPPW_KEYMAP_PL_RINGER,         // 1
#else
    MPPW_KEYMAP_REWIND,            // 1
#endif
#endif
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
    MPPW_KEYMAP_ST_RINGER,         // 2
#else
    MPPW_KEYMAP_PLAYSTOP,          // 2
#endif
#if defined(FEATURE_DTV_DVBH)  || defined(MP_FEATURE_MBMS)
    MPPW_KEYMAP_FW_FILE,           // 3
#else
    MPPW_KEYMAP_FASTFWD,           // 3
#endif
#ifdef FEATURE_QTV_FCS
#error code not present
#else
    MPPW_KEYMAP_NONE,              // 4
#endif
    MPPW_KEYMAP_PAUSE,             // 5
    MPPW_KEYMAP_FF_STEP,           // 6
    MPPW_KEYMAP_ZOOM_IN,           // 7
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    MPPW_KEYMAP_SAVEFILE,          // 8
#else
    MPPW_KEYMAP_ROTATE,            // 8
#endif
    MPPW_KEYMAP_ZOOM_OUT,          // 9
    MPPW_KEYMAP_RECORD,            // *
#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)
    MPPW_KEYMAP_REPLAY_AT_STOP     // #
#else
    MPPW_KEYMAP_RECSET_STRST       // #
#endif
    };

#ifdef MP_USE_NEWER_BUILD_ITEMS
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
extern int OEMMediaMPEG42PV_SetPvxSaveOption(char *dirPath);
#endif //FEATURE_APP_MPEG4 && FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
#endif //MP_USE_NEWER_BUILD_ITEMS

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/

#if !defined(FEATURE_APP_MEDIA_VIDEO)
static boolean CheckForPendingReposition(CPlayerWin *pMe);
#endif

static boolean CMediaPlayer_InitAppData(IApplet *po);
static boolean CMediaPlayer_Initialization(CMediaPlayer *pme);
static boolean CMediaPlayer_HandleEvent(IApplet *pi, AEEEvent eCode,
                                        uint16 wParam, uint32 dwParam);
#ifdef MP_DETERMINE_VOL_LEVELS
static void CMediaPlayer_HandleVolumeLevels(const void*, snd_status_type status, uint16 levels );
#endif
static void CMediaPlayer_DrawSplash(CMediaPlayer *pme);

static void CMediaPlayer_Redraw(CMediaPlayer *pme, boolean bDefer);
static void CMediaPlayer_RedrawNotify(CMediaPlayer *pme);
static boolean CMediaPlayer_PlayFile(CMediaPlayer *pme, const char *pszFile);
static void CPlayerWin_AutoPlayClip(CMediaPlayer *pme);

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/*
 * QTV_PSEUDO_STREAMING_BEGIN
 */
static boolean CMediaPlayer_PseudoPlayFile(CMediaPlayer *pme,
                                           const char *pszFile);
static void CMediaPlayer_KDDIPseudoPlayFile(CMediaPlayer *pme,
                                            const char *pszStream);

extern void PS_Free_web(CHttpBlock *psBlk);
extern void PS_CleanHttpDownload(CHttpBlock* psBlk);
extern void Search_And_Store(char *dst, const char *src, const char *searchStr,
                           unsigned int destLen);
extern boolean http_ps_SetUpWeb(CHttpBlock* psBlk);
extern boolean http_ps_blockInitialize(CHttpBlock *psBlk,
                                       const char *pszStream);
/* extern void http_ps_get_size(CHttpBlock *psHttpBlock); */
extern boolean http_ps_blockUpdate(CHttpBlock *psBlk);
extern void http_ps_get_chunk(CHttpBlock ** ppsBlk);

/*
 * QTV_PSEUDO_STREAMING_END
 */
#endif //FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif //FEATURE_APP_MPEG4

static void CMediaPlayer_RecordQCPFile(CMediaPlayer *pme, const char *pszFile);

#ifdef FEATURE_MP_RECORDER_SETTINGS
static boolean CMediaPlayer_CreateRecorderWindow(CMediaPlayer *pme,
                                                 AEEMediaData *md);
#endif //FEATURE_MP_RECORDER_SETTINGS

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
static boolean CMediaPlayer_CreateStreamRecordWindow(CMediaPlayer *pme, const char *pszFile);
#endif //(MP_FEATURE_VIDEO) && (FEATURE_QTV_STREAM_RECORD)

static boolean CMediaPlayer_PlayImage(CMediaPlayer *pme, const char *pszFile);
static void CMediaPlayer_FillExplorerData(CMediaPlayer *pme,
                                          const char *pszFile);

#ifdef MP_FEATURE_QTV_DIVX_DRM
boolean MediaPlayer_GetDivXRegCode(CMediaPlayer *pme, IMedia *pMedia, char *buf, int len);
boolean CMediaPlayer_DivXDRMSpecCat(AEEMediaMPEG4DivXDRMSpec* pDivXSpec, char *buf, int buf_size);
#endif

/*
 * CWindow abstract window
 */

/*
 * Main window
 */
static IWindow *CMainWin_New(CMediaPlayer *pOwner);
static void CMainWin_Delete(IWindow *po);
static void CMainWin_Enable(IWindow *po, boolean bEnable);
static void CMainWin_Redraw(IWindow *po);
static boolean CMainWin_HandleEvent(IWindow *po, AEEEvent eCode, uint16 wParam,
                                    uint32 dwParam);
static void CMainWin_About(CMainWin *pme);

#ifdef FEATURE_MP_HELP_SCREEN
static void CMainWin_Help(CMainWin *pme);
static AECHAR *FixResourceString(AECHAR *str);
#endif //FEATURE_MP_HELP_SCREEN

/*
 * Player window
 */
static int CPlayerWin_HandleCreateMedia(IWindow *po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static IWindow *CPlayerWin_New(CMediaPlayer *pOwner, MPPlayerWin eWin);
static void CPlayerWin_Delete(IWindow *po);
static void CPlayerWin_UpdateTitle(CPlayerWin *pme);
static void CPlayerWin_SetImageRect(CPlayerWin *pme);
static void CPlayerWin_UpdateImage(CPlayerWin *pme);
static void CPlayerWin_ExitOnImageLoadingFlag(CPlayerWin *pme);
static void CPlayerWin_Enable(IWindow *po, boolean bEnable);
static void CPlayerWin_Redraw(IWindow *po);
static void CPlayerWin_ExitRecorderWin(CPlayerWin *pme);

static void CPlayerWin_SetScreenRects(CPlayerWin *pme);

#ifdef FEATURE_MMOVERLAY
static boolean CPlayerWin_RegisterOvScreen(CPlayerWin *pme);
static boolean CPlayerWin_RegisterOv(IMMOverlayMgr *pIMMOverlayMgr, IMMOverlay **ppIMMOverlay, OvData ov);
static int CPlayerWin_SetOvLayer(IMMOverlay *pIMMOverlay, uint32 ovlayer);
static boolean CPlayerWin_Layer1LoadBitmap(CPlayerWin *pme, IBitmap *pDeviceBitmap);
static boolean CPlayerWin_Layer2LoadBitmap(CPlayerWin *pme, IBitmap *pDeviceBitmap);
static boolean CPlayerWin_LoadBitmapFromFile(CPlayerWin *pme, IBitmap *pDeviceBitmap, IBitmap **ppOutBitmap, char *pFilename);
static void CPlayerWin_SetLayer1Rect(CPlayerWin *pme);
static void CPlayerWin_SetLayer2Rect(CPlayerWin *pme);
static void CPlayerWin_SetOverlayRect(CPlayerWin *pme, IMMOverlay *pOverlay, IBitmap *pBitmap, AEERect *pRect);
static void CPlayerWin_UpdateLayer1(CPlayerWin *pme);
static void CPlayerWin_UpdateLayer2(CPlayerWin *pme);
static void CPlayerWin_UpdateOverlay(CPlayerWin *pme, IMMOverlay *pOverlay);
static void CPlayerWin_UpdateMenuOverlay(CPlayerWin *pme);
#endif // FEATURE_MMOVERLAY

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
static void CPlayerWin_StartStreamRecord(CPlayerWin *pme, boolean overwriteFile);
static void CPlayerWin_StopStreamRecord(CPlayerWin *pme);
#endif //(MP_FEATURE_VIDEO) && (FEATURE_QTV_STREAM_RECORD)

static boolean CPlayerWin_HandleClearOrOK(CPlayerWin *pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);

#ifndef FEATURE_KEY_HOLD_SCROLL
static boolean CPlayerWin_HandleEventKey(CPlayerWin *pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);
#endif //FEATURE_KEY_HOLD_SCROLL

static boolean CPlayerWin_HandleEventCommand(CPlayerWin *pme, uint16 wParam, uint32 dwParam);
static boolean CPlayerWin_HandleImageEventCommand(CPlayerWin *pme, uint16 wParam, uint32 dwParam);
static boolean CPlayerWin_HandleEvent(IWindow *po, AEEEvent eCode, uint16 wParam, uint32 dwParam);

#ifdef FEATURE_KEY_HOLD_SCROLL
static int CPlayerWin_HandleKeyEvent(IWindow *po, AEEEvent eCode,
                                     uint16 wParam, uint32 dwParam);
static void MP_OnKeyRepeatEvent(void *pUser);
#endif //FEATURE_KEY_HOLD_SCROLL

static boolean CPlayerWin_Replay(CPlayerWin* pme);
static boolean CPlayerWin_PlayNext(CPlayerWin* pme, int nMode, boolean bNextFile);
static void CPlayerWin_PlayNextClip(CPlayerWin *pme);
static void CPlayerWin_PlayPreviousClip(CPlayerWin *pme);

#ifdef FEATURE_QTV_GENERIC_BCAST
static void CPlayerWin_Reposition(CPlayerWin *pme);
static boolean CPlayerWin_SyncFrameForward(CPlayerWin *pme);
static boolean CPlayerWin_SyncFrameBack(CPlayerWin *pme);
#endif // FEATURE_QTV_GENERIC_BCAST

static void CMediaPlayer_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify);
static void CPlayerWin_ImageNotify(void *pUser, IImage *pImage, AEEImageInfo *pi, int nErr);                                   
static void CPlayerWin_UpdateProgCtl(CPlayerWin *pme, int nCmd, int nSubCmd, uint16 wResID);
static void CPlayerWin_BuildProgCtlMessage(CPlayerWin *pme, char *szStr);
static int  CPlayerWin_GetMediaPosition(CPlayerWin *pme, uint32 *pos);

#ifdef FEATURE_BT_EXTPF_AV
static boolean CPlayerWin_PausePlay(CPlayerWin *pme);
#endif // FEATURE_BT_EXTPF_AV
static boolean CPlayerWin_IsPause(CPlayerWin *pme);
static boolean CPlayerWin_IsReady(CPlayerWin *pme);
static boolean CPlayerWin_IsRecording(CPlayerWin *pme);
static boolean CPlayerWin_SetMediaData(CPlayerWin *pme, AEEMediaData *pmd);
static boolean CPlayerWin_IsPacketVideo(char *filename);
static boolean CPlayerWin_IsMediaBusy(CPlayerWin *pme);
static void CPlayerWin_SetMediaBusy(CPlayerWin *pme, CMediaBusy mask);
static void CPlayerWin_ClearMediaBusy(CPlayerWin *pme, CMediaBusy mask);

static boolean CPlayerWin_FullScreen(CPlayerWin *pme, boolean bFull,
                                     boolean bDeferRedraw);
static void CPlayerWin_SendMediaNotify(CPlayerWin *pme, int nCmd, int nSubCmd,
                                       int nStatus, void *pData, uint32 dwSize);
static int CPlayerWin_GetTotalTimeRequest(CPlayerWin *pme);
static int CPlayerWin_GetTotalTimeRetry(CPlayerWin *pme);
static int CPlayerWin_GetTotalTime(CPlayerWin *pme);

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif // FEATURE_QTV_MFDRM

#if defined (FEATURE_APP_MPEG4) && defined(FEATURE_QTV_IPL_SCALING )
#error code not present
#endif //#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)

static boolean CPlayerWin_Info(CPlayerWin *pme, boolean bDispUpdate);
#ifdef MP_FEATURE_VIDEO
static void CPlayerWin_Info_Clear(CPlayerWin *pme);
#endif //MP_FEATURE_VIDEO

#ifdef MP_FEATURE_VIDEO
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
static void CPlayerWin_ResetFrameTransformations(CPlayerWin *pme);
static void CPlayerWin_UpdateZoomScale(CPlayerWin *pme);
#endif // FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_MDP_TRANSFORMATIONS
#endif // MP_FEATURE_VIDEO

static void CPlayerWin_DrawRecordMenu(CPlayerWin *pme);
static void CPlayerWin_AdjustRecordMenu(CPlayerWin *pme);

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
static void CPlayerWin_DrawStreamRecordMenu(CPlayerWin *pme);
static void CPlayerWin_AdjustStreamRecordMenu(CPlayerWin *pme);
#endif //(MP_FEATURE_VIDEO) && (FEATURE_QTV_STREAM_RECORD)

static void CPlayerWin_UpdateMenu(CPlayerWin *pme, int nSelectMenuID);
static void CPlayerWin_RestoreDefaultMenu(CPlayerWin *pme);

// MPEG4 media contains video
static boolean CPlayerWin_IsMPEG4Video(CPlayerWin *pme);

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
static void CPlayerWin_3GPPTText(CPlayerWin *pme, AECHAR *pTTxt);
static void CPlayerWin_3GPPTimedTextExpiredNotify(CPlayerWin *pme);
#endif //FEATURE_MP4_3GPP_TIMED_TEXT

#ifdef FEATURE_MP_KEY_MAPPING
static void CPlayerWin_ShowProgCtlText(CPlayerWin *pme, char *strText);
#endif //FEATURE_MP_KEY_MAPPING
static boolean CPlayerWin_PurgeDownloadedFile(CPlayerWin *pme);

#ifdef FEATURE_MP_EQ_UI
static void CPlayerWin_ShowSettingWin(CPlayerWin *pmp);
static void CMediaPlayer_AudioSetupScreen(CMediaPlayer * pme);

#ifdef FEATURE_MP_EQ
static void CMediaPlayer_InitISoundCtl(CMediaPlayer * pme);
static void CMediaPlayer_SoundCtlCBFn(void* pUser, AEESoundCtlNotify* pfnNotify);
static char* CMediaPlayer_GetSoundCtlCBCMDString(int16 cmd, char* pBuf, int size);
static char* CMediaPlayer_GetSoundCtlStatusString(int16 status, char* pBuf, int size);
#endif //FEATURE_MP_EQ
#endif //FEATURE_MP_EQ_UI

boolean CPlayerWin_FCSSwitch(CPlayerWin *pme);
#ifdef FEATURE_QTV_FCS
#error code not present
#endif // FEATURE_QTV_FCS


/*
 * Sequence Player window
 */
#ifdef FEATURE_MULTISEQUENCER
static IWindow *CSequencePlayerWin_New(CMediaPlayer *pOwner);
static void CSequencePlayerWin_Delete(IWindow *po);
static void CSequencePlayerWin_Enable(IWindow *po, boolean bEnable);
static void CSequencePlayerWin_Redraw(IWindow *po);
static boolean CSequencePlayerWin_HandleEvent(IWindow *po, AEEEvent eCode,
                                              uint16 wParam, uint32 dwParam);
static void CSequencePlayerWin_UpdateProgCtl(CSequencePlayerWin *pme, int nCmd,
                                             int nSubCmd, uint16 wResID,
                                             uint16 nIndex);
static void CSequencePlayerWin_Play(CSequencePlayerWin *pme);
static boolean CSequencePlayerWin_IsPlaying(CSequencePlayerWin *pme,
                                            boolean bAll);
#ifdef FEATURE_BT_EXTPF_AV
static boolean CSequencePlayerWin_PausePlay(CSequencePlayerWin *pme);
#endif // FEATURE_BT_EXTPF_AV
static void CSequencePlayerWin_Stop(CSequencePlayerWin *pme, boolean bAll);
static void CSequencePlayerWin_SetVolume(CSequencePlayerWin *pme, uint16 wVol);
static boolean CSequencePlayerWin_FullScreen(CSequencePlayerWin *pme,
                                             boolean bFull,
                                             boolean bDeferRedraw);
static void CSequencePlayerWin_ProgTimerNotify(CSequencePlayerWin *pme);
#endif //FEATURE_MULTISEQUENCER

static int CMediaPlayer_GetPreviousFileIndex(CMediaPlayer *pme, int nCurFileIndex);
static int CMediaPlayer_GetNextFileIndex(CMediaPlayer *pme, int nCurFileIndex);
static int CMediaPlayer_GetFileCount(CMediaPlayer *pme);
static boolean CMediaPlayer_FindFileInFileList(CMediaPlayer *pme,
                                               const char *pszFile);

static void CMediaPlayer_VerifyConfigData(CMediaPlayer *pme);
/*
 * ProgCtl
 */
#ifdef FEATURE_MULTISEQUENCER
static boolean CProgCtl_Init(CProgCtl *pme, AEERect *pRectMain);
static void CProgCtl_SetPos(CProgCtl *pme, AECHAR *psz, uint16 wPct);
static void CProgCtl_Release(CProgCtl *pme);
static void CProgCtl_DrawHist(CProgCtl *pme, uint16 wPct);
#endif //FEATURE_MULTISEQUENCER

/*
 * 3GPPTTextCtl
 */
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
static boolean C3GPPTTextCtl_Init(C3GPPTTextCtl *pme, AEERect *pRectMain);
static void C3GPPTTextCtl_Update(C3GPPTTextCtl *pme, AECHAR *psz);
static void C3GPPTTextCtl_Release(C3GPPTTextCtl *pme);
#endif //FEATURE_MP4_3GPP_TIMED_TEXT

#ifdef FEATURE_MP_RECORDER_SETTINGS
static IWindow *CRecorderSettingWin_New(CMediaPlayer *pOwner);
static void CRecorderSettingsWin_Enable(IWindow *po, boolean bEnable);
static void CRecorderSettingsWin_Redraw(IWindow *po);
static void CRecorderSettingsWin_Delete(IWindow *po);
static boolean CRecorderSettingsWin_HandleEvent(IWindow *po, AEEEvent eCode,
                                                uint16 wParam, uint32 dwParam);
static boolean CRecorderSettingsWin_SettingsKeyHandler(IWindow *po,
                                                       AEEEvent eCode,
                                                       uint16 key);
/*
 * recorder setting window submenus
 */
static void CRecorderSettingsWin_BuildSubMenu(IWindow *po);
static void CRecorderSettingsWin_BuildAudioFormatSubMenu(IWindow *po);
static void CRecorderSettingsWin_BuildAudioSampleRateSubMenu(IWindow *po);
static void CRecorderSettingsWin_BuildAudioChannelSubMenu(IWindow *po);
static void CRecorderSettingsWin_BuildRecordPathSubMenu(IWindow *po);
static void CRecorderSettingsWin_BuildSaveLocationSubMenu(IWindow *po);
static void CRecorderSettingsWin_BuildDeleteFileMenu(IWindow *po);

static boolean CRecorderSettingsWin_MakeList(IWindow *po, CFileExplore *pFileExp);
static boolean IsMemofile(char *szBuf);
static int MP_DeleteFile(CMediaPlayer *pme, IFileMgr *pIFileMgr, const char* pDir, const AECHAR* pszName);
static boolean CRecorderSettingsWin_DeleteSelectedFile(IWindow *po);

static void CRecorderSettingsWin_MarkSelection(IWindow *po);
static char *CRecorderSettingsWin_GetRecordingFileName(CMediaPlayer *pmp);
static boolean GetRecorderFileTypeString(int type, char *pBuf, int16 nSize);
static int CRecorderSettingsWin_GetFileNameSuffix(CMediaPlayer *MP,
                                                  char *pszFileType,
                                                  char *pszDirName, int NumChar,
                                                  char *strFileExt);
static boolean CRecorderSettingsWin_Is13K(int16 audio_format);
static boolean CRecorderSettingsWin_IsEVRC(int16 audio_format);
static boolean CRecorderSettingsWin_IsEVRCNB(int16 audio_format);
static boolean CRecorderSettingsWin_IsAMR(int16 audio_format);
static boolean CRecorderSettingsWin_IsPCM(int16 audio_format);
static boolean CRecorderSettingsWin_IsAAC(int16 audio_format);
#endif //FEATURE_MP_RECORDER_SETTINGS

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

/*
 * Helper Functions
 */
static boolean MP_GetMenuItemIndex(IMenuCtl * pIMenuCtl, uint16 itemID, int* idx);
#ifdef MP_FEATURE_ICONTENT
static void MP_AddFileToIContentDB(CMediaPlayer *pme,
                                           const char* pszCatName, const char* pzeCatDesc,
                                           const char* pszFile, const char* pszDesc,
                                           const char* nMediaType);
//static void MP_DeleteContent(CMediaPlayer *pme, const char* pszFile);
#endif //MP_FEATURE_ICONTENT

static void MP_WaitOnState(CPlayerWin* pme, MediaPlayerPlayState state, uint32 nMaxTime);
static void MP_DrawImage(IImage *pImage, AEERect *pRect, boolean bCenter);

static boolean MP_AddPlayerMenuItem(CMediaPlayer *pme, IMenuCtl *pMenu, uint16 wTextID, AECHAR *pText,
                              uint16 wImageID, uint16 wItemID, uint32 dwData);

#ifdef FEATURE_APP_MPEG4
static boolean MP_PrepareInfoText(CPlayerWin *pme, AECHAR *pBuf, uint16 nSize);
#endif //FEATURE_APP_MPEG4

#if defined(FEATURE_QTV_STREAM_RECORD)

static boolean MP_GenerateRecordFilename(char *recordFile, char *filename, char *path_prefix, char* outExt, boolean addLocal);
static boolean MP_GetRecordFileName_SDP(char *recordFile, char *filename, char *path_prefix);

#if defined(MP_FEATURE_VIDEO)

static boolean MP_GetRecordFileName_Streaming(char *recordFile, char *pURL, char *path_prefix);

#endif // defined(MP_FEATURE_VIDEO)

#endif //defined(FEATURE_QTV_STREAM_RECORD)

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
//static boolean MP_GenerateStreamRecordFilename(char *recordFile, char *pURL, char *path_prefix);
#endif //(MP_FEATURE_VIDEO) && (FEATURE_QTV_STREAM_RECORD)

#if defined (FEATURE_APP_MPEG4) && !defined (FEATURE_APP_MEDIA_VIDEO)
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
static AEEMediaMPEG4ScalingType MP4_GetNextScale(AEEMediaMPEG4ScalingType
                                                 nCurScale, int ZoomType);

static AEEMediaMPEG4RotationType MP4_GetNextRotation(AEEMediaMPEG4RotationType
                                                    curRotation, int type);
#endif //FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_MDP_TRANSFORMATIONS
#endif //FEATURE_APP_MPEG4 && !defined (FEATURE_APP_MEDIA_VIDEO)

static void MP_ScreenSizeCheck(CMediaPlayer *pme);

#ifdef FEATURE_QTV_MDP_ASCALE
static boolean CPlayerWin_SetAScale(CPlayerWin *pwin);
static boolean CPlayerWin_isVerticalDisplay(CPlayerWin *pme);
#endif //FEATURE_QTV_MDP_ASCALE

#ifdef FEATURE_MP4_LCD_CONTRAST
static void UpdateContrast(CContrastWin *pme, int value);

/*
 * Contrast window
 */
static IWindow *CContrastWin_New(CMediaPlayer *pOwner);
static void CContrastWin_Delete(IWindow *po);
static void CContrastWin_Enable(IWindow *po, boolean bEnable);
static void CContrastWin_Redraw(IWindow *po);
static boolean CContrastWin_HandleEvent(IWindow *po, AEEEvent eCode,
                                        uint16 wParam, uint32 dwParam);
#endif //FEATURE_MP4_LCD_CONTRAST

static boolean CMediaPlayer_FindMediaPlayerHandlerType(const char *pszFileExt, int* pIconType);
static boolean MP_ExtractURLFromFile(CMediaPlayer *pme, const char *pszFile, char *pUrl, int szUrl);
boolean CMediaPlayer_FindNextURL(CMediaPlayer *pme, char *url, int url_size);

AEECLSID CMediaPlayer_GetAEECLSID(CMediaPlayer *pme, char *base, char *ext);
/**********************************************************************/

/*********    Data and prototypes needed for Settings Menu      *******/

/**********************************************************************/
static void CMediaPlayer_Settings(CMediaPlayer *pme);
static boolean SettingsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w,
                                       uint32 dw);
static boolean RadioButtonDlgEventHandler(void *pUser, AEEEvent evt, uint16 w,
                                          uint32 dw);
static void UpdateRadioButtons(CMediaPlayer *pme, uint16 wSelId,
                               IMenuCtl *pIMenu);
static uint16 GetCurrentSettings(CMediaPlayer *pme, uint16 wDlgId);
static void SetRadioButtons(CMediaPlayer *pme, IMenuCtl *pICurrentMenu,
                            uint16 wItemId);
static void TranslateResourceIDToSetting(CMediaPlayer *pme, uint16 ResId);
static void EndCurrentDlg(CMediaPlayer *pme);

static boolean CPlayerWin_MappedKeyEventHandler(IWindow *, AEEEvent, uint16, uint32);
uint16 MPKeyMapping_MappingToDlg(CKeyMapping key);
CKeyMapping MPKeyMapping_DlgToMapping(uint16 idm);
#ifdef FEATURE_MP_SEEK_TIME_DLG
static boolean SeekTimeSettingDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
#endif //FEATURE_MP_SEEK_TIME_DLG

#ifdef FEATURE_QTV_MDP_ASCALE
static boolean AScaleSettingDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
#endif //FEATURE_QTV_MDP_ASCALE

#ifdef MP_FEATURE_QTV_DIVX_DRM  
static boolean DivxRegCodeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
#endif //MP_FEATURE_QTV_DIVX_DRM  

static boolean KeyMappingsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
static boolean KeyMappingsDlgSetListKeys(CMediaPlayer *pme, uint16 wItemID, uint16 wTitleID, CKeyMapping curMapping, AEERect *rect);
CKeyMapping KeyMappingsDlgGetMappedKey(CMediaPlayer *pme, uint16 wItemID);
boolean KeyMappingsDlgSetMappedKey(CMediaPlayer *pme, uint16 wItemID, CKeyMapping key);
boolean KeyMappingsSetDefault(CMediaPlayer *pme);

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
static boolean ProgDnldSettingsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

#ifdef FEATURE_MMOVERLAY
static boolean AlphaBlendSettingDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
#endif // FEATURE_MMOVERLAY

#if defined(MP_FEATURE_VIDEO) && (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))
static boolean MPPDPSettingDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
static uint8 MPPDPGetDefaultProfile(CMediaPlayer *pme);
static int MPPDPGetNumProfiles(CMediaPlayer *pme);
static boolean MPPDPGetProfileName(CMediaPlayer *pme, int nProfile, AECHAR* buf, int length);
#endif //(MP_FEATURE_VIDEO) && (FEATURE_QTV_QOS_SELECTION) || FEATURE_QTV_PDP_SELECTION

#ifndef FEATURE_MP_RECORDER_SETTINGS
static int MP_GetFileNameSuffix(CMediaPlayer *MP, char *pszSuffixName, char *pszFileType, char *pszDirName, int NumChar);
#endif //FEATURE_MP_RECORDER_SETTINGS

static void SetSettingsMenuStyle(CMediaPlayer *pMe, IMenuCtl *pIMenu);

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
static void PseudoPlayFile_CleanUp(CMediaPlayer *pme);
static boolean PseudoPlayFile_ProcessURL(CMediaPlayer *pme, const char *pszFile);
static boolean PseudoPlayFile_ExtractURLFromFile(CMediaPlayer *pme, const char *pszFile, char *pURL, boolean *bIsURLfile);
static boolean PseudoPlayFile_ProcessFile(CMediaPlayer *pme, const char *pszFile, boolean *bIsURLfile);
static boolean PseudoPlayFile_StartStream(CMediaPlayer *pme, char *pszBuf);
static boolean PseudoPlayFile_StartTest(CMediaPlayer *pme, char *pszBuf);

static void PseudoStreamSetTimer(CMediaPlayer *pme, unsigned int timeInterval);
static void PSTimerResponse(void *pUser);
#endif //FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif //FEATURE_APP_MPEG4

static boolean CMediaPlayer_IsAudioClassID(AEECLSID classId);
static boolean CMediaPlayer_IsAudioFileExension(char* pExtName);

#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
static boolean CMediaPlayer_IsAudioMedia(CMediaPlayer * pme, char *pExtName);
#endif //#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))

static boolean CMediaPlayer_HandlePhoneEvent(CMediaPlayer *pme, void * pNotifyData);

static boolean CMediaPlayer_IsCreateMediaCode(AEEEvent eCode);

/************************************************
  Routines for handling Local Play PULL Mode
************************************************/
static int  MP_LocalPlayPullModeRegisterCallbacks(CMediaPlayer *pMP, IMedia *pMedia);
void        MP_LocalPlayPullModeFetchBufferedDataSize(void* pUserData, uint32 *bufDataSize, boolean *pbEndOfData);
uint32      MP_LocalPlayPullModeFetchBufferedData(void* pUserData, void *dataBuf, uint32 readSize, uint32 readPos);

/*
 * Dialog Event Handler function pointer structure
 */
typedef   boolean(*PFNDLGHANDLER) (void *pUser, AEEEvent evt, uint16 w,
                                   uint32 dw);

#ifdef MP_FEATURE_FAST_PLAYBACK
static const uint32 REPLAY_SPEED[] =
{
   QTV_PLAYBACK_SPEED_NORMAL
   ,QTV_PLAYBACK_SPEED_1P3X
};

#endif //#ifdef MP_FEATURE_FAST_PLAYBACK

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION

static const uint32 DualMonoOutput[] =
{
  DUAL_MONO_MAIN_TO_L_AND_R
  ,DUAL_MONO_SUB_TO_L_AND_R
  ,DUAL_MONO_MAIN_TO_L_SUB_TO_R
  ,DUAL_MONO_SUB_TO_L_MAIN_TO_R
};

#endif //FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION

typedef struct
{
  uint16    wDialogID;
  uint16    wCtrlID;
} MenuCtlIDTable;

static const PFNDLGHANDLER DlgEventHandlers[] = {
  NULL,
  SettingsDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler,
  RadioButtonDlgEventHandler
};

static const MenuCtlIDTable LookupMenuCtlID[] =
{
  {0, 0},
  {IDD_CONT_MODE, IDC_CONT_MENU},
  {IDD_AUTOPLAY, IDC_AUTOPLAY_MENU},
  {IDD_VOLUME_SETTINGS, IDC_VOLUME_MENU},
#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  {IDD_PSEUDO_STR_SETTINGS, IDC_PS_SETTINGS},
  {IDD_KDDI_EXTENSIONS, IDC_KDDI_EXTENSIONS_MENU},
#endif
#ifdef FEATURE_QTV_PDP_SELECTION
  {IDD_QOS_UMTS_SETTINGS, IDC_QOS},
  {IDD_QOS_CONFIG, IDC_QOS_TOGGLE},
#endif
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  {IDD_RTSP_STR_REC, IDC_RTSP_STR_REC},
#endif
#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
  {IDD_ICONTENT, IDC_ICONTENT_MENU},
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
  {IDD_BUFFER, IDC_BUFFER_MENU},
  {IDD_PLAY_IN_BACKGROUND, IDC_PLAY_IN_BACKGROUND_MENU},
  {IDD_REDRAW_AT_END, IDC_REDRAW_AT_END_MENU},
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  {IDD_PVXSAVELOC, IDC_PVXSAVELOC_MENU},
#endif
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  {IDD_HTTP_DOWNLOAD, IDC_HTTP_DOWNLOAD_MENU},
#endif
#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
  {IDD_HTTP_SOURCE, IDC_HTTP_SOURCE_MENU},
  {IDD_HTTP_MIMIC, IDC_HTTP_MIMIC_MENU},
#endif
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
  {IDD_PLAY_RINGER_AND_QTV, IDC_PLAY_RINGER_AND_QTV_MENU},
  {IDD_PLAY_RINGER_NO_QTV, IDC_PLAY_RINGER_NO_QTV_MENU},
#endif
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  {IDD_PROG_DL_SAVE_LOC, IDC_PROG_DL_SAVE_LOC_MENU},
#endif
#ifdef FEATURE_MMOVERLAY
  {IDD_OVERLAYS, IDC_OVERLAYS_MENU},
#endif // FEATURE_MMOVERLAY
  {IDD_HIGH_PERFORMANCE_MODE, IDC_HIGH_PERFORMANCE_MODE_MENU},
  {IDD_LOCAL_PLAY_PULL_MODE, IDC_LOCAL_PLAY_PULL_MODE_MENU},
#ifdef FEATURE_APP_MPEG4
  {IDD_QTV_SUSPEND_MODE, IDC_QTV_SUSPEND_MODE_MENU}
#endif
};

static unsigned char ConnectStatusStringActive = 0;
//static uint32 BufferedTime = 0;
static uint32 prerollPercentage = 0;

#ifdef MP_FEATURE_VIDEO
static boolean CPlayerWin_SetMediaDataEx(CPlayerWin *pme, char *pVideoFile,
                                         char *pAudioFile);
static boolean MP_PrepMediaDataEx(AEEMediaDataEx *pmd, char *pFile, int nCaps);
static void MP_FreeMediaDataExList(AEEMediaDataEx *pmd, int nCount);
static boolean MP_GetAVFileNames(IShell *po, const char *pFile,
                                 char *pVideoFile, char *pAudioFile,
                                 int fileBufSize, boolean *pbLoop);
#endif //MP_FEATURE_VIDEO
static int ComputeSeekOffset(CPlayerWin *pMe);

//#if !defined(FEATURE_APP_MEDIA_VIDEO)
//static boolean CheckForPendingReposition(CPlayerWin *pMe);
//#endif

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
static unsigned char BufferUpdateToggle = 0;
static uint32 pseudostreamDownloadRate = 0;

/*
 * Qtv Pseudo Streaming simulation
 */

#define PS_BUF_DFLT_SIZE 1000000

#define KBPS_10   1250
#define KBPS_32   4000
#define KBPS_64   8000
#define KBPS_128  16000
#define KBPS_256 32000
#define KBPS_54  6750
#define KBPS_MAX  0xFFFFFFFF   // chunk size will end up being filesize

/* also used in HttpDownloader.c (so _not_ static) */
char     *HttpDownloader_sharedBuffer = NULL;
uint32    HttpDownloader_maxBufferSize = PS_BUF_DFLT_SIZE;
boolean   HttpDownloader_entireClipInBuffer = FALSE;
boolean   HttpDownloader_suspended = FALSE;
boolean   HttpDownloader_resuming = FALSE;

#if defined (MMD_HTTP_PUSH_BUFFER_PLAYBACK)
uint32    HTTPDownloadInterface  = MMD_HTTP_PUSH_BUFFER_PLAYBACK; //default set to push interface
#else
uint32    HTTPDownloadInterface  = MMD_BUFFER_FRAGMENT;
#endif //MMD_HTTP_PUSH_BUFFER_PLAYBACK
#endif //(FEATURE_APP_MPEG4) && (FEATURE_QTV_PSEUDO_STREAM) || (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
/*
 * End Qtv Pseudo Streaming
 */

#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
  #define MP_NO_ZOOM                            MM_MPEG4_NO_SCALING
#endif //FEATURE_APP_MEDIA_VIDEO

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
boolean   bRTSPstreamRecord = FALSE;
static boolean   bRTSPstreamRecFileOverwrite = TRUE;
static char      recFileName[MP_MAX_STRING_LENGTH];
static char      defaultRecDir[MP_MAX_STRING_LENGTH] = { '\0' };
#endif //(MP_FEATURE_VIDEO) && (FEATURE_QTV_STREAM_RECORD)

#if defined (FEATURE_QTV_BCAST_FLO)  && !defined (FEATURE_APP_MEDIA_VIDEO)
#error code not present
#endif //FEATURE_QTV_BCAST_FLO && !defined (FEATURE_APP_MEDIA_VIDEO)


/*
 * -------------------------------------------------------------------
 *          Global Constant Definitions
 * -------------------------------------------------------------------
 */

/*
 * -------------------------------------------------------------------
 *          Global Data Definitions
 * -------------------------------------------------------------------
 */

/*
 * -------------------------------------------------------------------
 *          Static variable Definitions
 * -------------------------------------------------------------------
 */
static boolean mp_ignoreNotices = FALSE;
#ifdef MP_FEATURE_VIDEO
static boolean mp_enableFrameCB = FALSE;
#endif // MP_FEATURE_VIDEO

/*
 * MediaPlayer app can either be statically built into BREW or dynamically
 * linked during run-time.
 * If AEE_STATIC is defined, then MediaPlayer app will be a static app.
 * NOTE: Static apps can be built only by OEMs or Carriers. App developers
 * can build dynamic apps only.
 */
#if defined(AEE_STATIC)

int
CMediaPlayer_CreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po,
                            void **ppObj);
/*
 * static
 */
int
CMediaPlayer_Load(IShell *ps, void *pHelpers, IModule ** pMod);


/*
 * Constant Data...
 */
static const AEEAppInfo gaiMediaPlayerApp =
{
  AEECLSID_MEDIAPLAYER,
  MEDIAPLAYER_RES_FILE,
  IDS_TITLE,
  IDB_TNAIL,
  IDB_TNAIL,
  0,
  0,
  0
};


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

PFNMODENTRY
CMediaPlayer_GetModInfo(IShell *ps, AEECLSID ** ppClasses, AEEAppInfo ** pApps,
                        uint16 * pnApps, uint16 * pwMinPriv)
{
  *pApps = (AEEAppInfo *)& gaiMediaPlayerApp;
  *pnApps = 1;
  return ((PFNMODENTRY)CMediaPlayer_Load);
}

/*
 * static
 */
int
CMediaPlayer_Load(IShell *ps, void *pHelpers, IModule ** pMod)
{
  return (AEEStaticMod_New
          ((int16)(sizeof(AEEMod)), ps, pHelpers, pMod,
           CMediaPlayer_CreateInstance, NULL));
}

#endif     // AEE_STATIC

/*===========================================================================

Function:  CMediaPlayer_InitAppData()

Description:
    This function initializes the app specific data.

Prototype:
  boolean CMediaPlayer_InitAppData(IApplet* po)

Parameters:
   IApplet* po: Pointer to the applet structure. This is of the type defined
  to store applet data. (CMediaPlayer*, in this case).

Return Value:
  TRUE: If successful
  FALSE: IF failed

Comments:  None

Side Effects: None

==============================================================================*/
boolean
CMediaPlayer_InitAppData(IApplet *po)
{
  CMediaPlayer *pme = (CMediaPlayer *)po;
  int       nAscent,
            nDescent;
  AEEDeviceInfo *pdi;

  pme->m_State = MP_STATE_NULL;
  EMIT_MEDIAPLAYER_STATE(pme->m_State);
  pme->m_playState = MP_PLAYSTATE_STOPPED;
  EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_playState);

 /*
   * Get screen pixel count
   */
  pdi = MALLOC(sizeof(AEEDeviceInfo));
  if (!pdi)
    return FALSE;
  ISHELL_GetDeviceInfo(pme->a.m_pIShell, pdi);
  pme->m_cxWidth = pdi->cxScreen;
  pme->m_cyHeight = pdi->cyScreen;
  DBGPRINTF_MED("MP: Our screen size is %d x %d", pme->m_cxWidth, pme->m_cyHeight);
  //Determine based on the screen size if small icons need to be displayed
  MP_ScreenSizeCheck(pme);
  pme->m_nColorDepth = pdi->nColorDepth;
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  pme->m_defaultSetupTime.dwStartupTime = MP_HTTP_STARTUP_TIME;
  pme->m_defaultSetupTime.dwBufferingTime = MP_HTTP_BUFFERING_TIME;
  pme->m_maxDownloadFileSize = PS_BUF_DFLT_SIZE;
  HttpDownloader_maxBufferSize = pme->m_maxDownloadFileSize;
#endif //FEATURE_APP_MPEG4 && FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  pme->m_bIsStreamRecord = FALSE;
#endif //MP_FEATURE_VIDEO && FEATURE_QTV_STREAM_RECORD

  pme->m_bVideoPlaybackReady = FALSE;

#ifdef FEATURE_QTV_MDP_ASCALE
  pme->m_nAScaleXValue = MP_ASCALE_DEFAULT_VALUE;
  pme->m_nAScaleYValue = MP_ASCALE_DEFAULT_VALUE;
#endif //FEATURE_QTV_MDP_ASCALE
  pme->m_bBackgroundMode = FALSE;
#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
#if defined(FEATURE_QTV_HTTP_DOWNLOAD)
  pme->m_bCallQtvProgDnld = TRUE;
#else
  pme->m_bCallQtvProgDnld = FALSE;
  HTTPDownloadInterface = MMD_HTTP_PULL_BUFFER_PLAYBACK;
#endif
  pme->m_MimicEncryptDnld = MIMIC_ENCRYPT_DNLD_NONE;
#endif //FEATURE_APP_MPEG4 && FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  FREEIF(pdi);

  (void)IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_LARGE, &nAscent,
                          &nDescent);
  pme->m_nLChSize = nAscent + nDescent;

  (void)IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_NORMAL, &nAscent,
                          &nDescent);
  pme->m_nNChSize = nAscent + nDescent;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#ifdef FEATURE_BT_EXTPF_AV
  if (ISHELL_RegisterNotify(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                            AEECLSID_BLUETOOTH_NOTIFIER,
                            (NMASK_BT_AVRCP|NMASK_BT_A2DP)|(((uint32)(NOTIFIER_VAL_ANY))<<16))
      != SUCCESS )
  {
    DBGPRINTF_ERROR("MP: failed registration for AVRCP events");
  }
#endif //FEATURE_BT_EXTPF_AV

// Register for ITelephone call events
  if (ISHELL_RegisterNotify(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                            AEECLSID_PHONENOTIFIER, AEET_NMASK_VOICE_CALL | AEET_NMASK_NEW_CALLDESC)
      != SUCCESS )
  {
    DBGPRINTF_ERROR("MP: failed registration for ITelephone events");
  }
  else
  {
    DBGPRINTF_MED("MP: successful registration for ITelephone events");
  }

  pme->m_bInTestAutomation = FALSE;

#ifdef FEATURE_DTV_DVBH
  pme->m_DescramblerKey = -1;
#endif //FEATURE_DTV_DVBH

  return TRUE;
}


/*===========================================================================

Function:  CMediaPlayer_Initialization()

Description:
    This function initializes the app specific data.

Prototype:
  void CMediaPlayer_Initialization(CMediaPlayer *pme)

Parameters:
  (CMediaPlayer*, in this case).

Return Value:
  TRUE: If successful
  FALSE: IF failed

Comments:  None

Side Effects: None

==============================================================================*/
boolean
CMediaPlayer_Initialization(CMediaPlayer *pme)
{
  IFileMgr *pIFileMgr;

  (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR,
                              (void **)&pIFileMgr);
  if (pIFileMgr)
  {
    /*
     * test root directory. Create one if it doesn?t exist
     */
    if (!MP_MakeDir(pIFileMgr, MP_MEDIA_DIR))
    {
      DBGPRINTF_ERROR("MP: failed to create root directory.");
      MP_RELEASEIF(pIFileMgr);
      return FALSE;
    }
  }
  else {
    DBGPRINTF_HIGH("MP: IFileMgr pointer is NULL!");
    return FALSE;
  }

  CALLBACK_Init(&pme->m_cbRedraw, (PFNNOTIFY)CMediaPlayer_RedrawNotify, pme);

  pme->m_pWin = CMainWin_New(pme);
  if (!pme->m_pWin)
  {
    MP_RELEASEIF(pIFileMgr);
    return FALSE;
  }

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
  pme->m_bIContentMode = FALSE;
  pme->m_pContent =  MPContentMake(pme);
  pme->m_IContentResumePos = 0;
  pme->m_bIContentResumePlay = FALSE;

  if(!pme->m_pContent)
  {
  	DBGPRINTF_MED("MP: Calling MP_RELEASEWIN");
    MP_RELEASEWIN(pme->m_pWin);
    MP_RELEASEIF(pIFileMgr);
    return FALSE;
  }
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

   /*
   * Populate Mediaplayer struct with all media files.
   */
  (void)MEMSET(&pme->m_FileExplorer, 0, sizeof(pme->m_FileExplorer));
  CMediaPlayer_InitializeExplorer(pme);

  // Figure out the volume step based off of the number of levels
  // we we have 4 levels we shoudl get 25, if we have 7 we should get 14
  // As we are doing integer division
  pme->m_wNumVolumeLevels = MP_DEFAULT_NUM_VOL_LEVELS;
  pme->m_VolumeStep = AEE_MAX_VOLUME / pme->m_wNumVolumeLevels;
  DBGPRINTF_MED("MP: We have defaulted to %d volume levels with volume step %d", pme->m_wNumVolumeLevels, pme->m_VolumeStep);

#ifdef MP_DETERMINE_VOL_LEVELS
  {
    //uisnd_get_levels(SND_DEVICE_HANDSET, SND_METHOD_VOICE, CMediaPlayer_HandleVolumeLevels, pme);
    snd_get_levels(SND_DEVICE_HANDSET, SND_METHOD_VOICE, CMediaPlayer_HandleVolumeLevels, pme);
  }
#endif

#ifdef MP_FEATURE_MBMS
  // If the mbms dir exists Move to that directory
  // This is a hack for the MBMS Demo
  if (IFILEMGR_Test(pIFileMgr, MP_MBMS_MEDIA_DIR) == SUCCESS)
  {
    STRLCPY(pme->m_FileExplorer.m_nCurDir, MP_MBMS_MEDIA_DIR, MP_MAX_FILE_NAME);
    pme->m_FileExplorer.m_nCurDir[MP_MAX_FILE_NAME - 1] = '\0';
    pme->m_FileExplorer.m_nCurDepth = 2;
  }
#endif //MP_FEATURE_MBMS

  pme->m_bDisableAutoPlayOnce = FALSE;
  pme->m_pIGraphics = NULL;
  pme->m_bShowAudioSpec = FALSE;

#ifdef FEATURE_MP_EQ_UI
  // create and init audio setting object
  pme->m_pSetting = (MPSetting*)MALLOC(sizeof(MPSetting));
  if(pme->m_pSetting)
    MPSettingInit(pme->m_pSetting, (void*)pme);
  else
  {
    pme->m_bShowPlayerWinSettingUI = FALSE;
    DBGPRINTF_ERROR("MP: Unable to create MPSetting object.");
  }
#else
  pme->m_bShowPlayerWinSettingUI = FALSE;
#endif

  // remove old config file
  if (IFILEMGR_Test(pIFileMgr, "mediaplayer.cfg") == SUCCESS)
	{
		if(SUCCESS != IFILEMGR_Remove(pIFileMgr, "mediaplayer.cfg"))
		{
			DBGPRINTF_ERROR("MP: failed to delete %s.", "mediaplayer.cfg");
		}
  }

#ifdef FEATURE_APP_MPEG4
  pme->m_bQtvSuspendPause = TRUE;
#endif
  (void)MPConfigFile_Read(pme);
  CMediaPlayer_VerifyConfigData(pme);

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
#ifndef FEATURE_QTV_HTTP_DOWNLOAD
  if(pme->m_bCallQtvProgDnld == TRUE)
  {
    pme->m_bCallQtvProgDnld = FALSE;
    HTTPDownloadInterface = MMD_HTTP_PULL_BUFFER_PLAYBACK;
  }
#endif
#endif

  pme->m_bIsHTTP = FALSE;
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  pme->m_bEnablePSSimulation = FALSE;
  pme->m_bFileWriteReadTest = FALSE;
  pme->m_bDownloadNotDone = TRUE;
  pme->m_bVariableRate = FALSE;
  pme->m_bReconnectWebOnResume = FALSE;
  pme->m_nDownloadPendingSize = 0;
  pme->m_nDownloadCopiedSize = 0;
  pme->m_nDownloadCopyBytes = 0;
  pme->m_downloadFileBuffer = NULL;
  pme->m_pPseudoStrIFile = NULL;
  pme->m_pPSIFileMgr = NULL;
  pme->m_pIFileMgr2 = NULL;
  pme->m_nChunkSize = KBPS_32;    /* If chunksize is X, download bitrate is X*8 Bps */
#endif //defined (FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#endif // FEATURE_APP_MPEG4

  pme->m_pIRadioOn = ISHELL_LoadResImage(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE,
                                         IDB_RADIO_ON);
  pme->m_pIRadioOff = ISHELL_LoadResImage(pme->a.m_pIShell,
                                          MEDIAPLAYER_RES_FILE, IDB_RADIO_OFF);
  pme->i = 0;
  pme->m_bIsImage = FALSE;
  pme->m_CurFileIndex = 0;
  pme->m_ErrorState = MP_ERRORSTATE_NONE;
  pme->m_bResumePending = FALSE;
  pme->m_bNeedReload = FALSE;
  pme->m_bPlayAfterResume = TRUE;
  pme->m_resumeFileName[0] = '\0';
  pme->m_resumePlayPos = -1;
  pme->m_bSuspend = FALSE;
  pme->m_bIncallMode = FALSE;
  pme->m_bDeleteFile = FALSE;
#ifdef FEATURE_KEY_HOLD_SCROLL
  pme->m_KeyStore.m_wParam_Key = 0;
  pme->m_KeyStore.m_dwParam_Key = 0;
#endif

#ifdef FEATURE_QTV_GENERIC_BCAST
  pme->m_pTrackList = NULL;
  pme->m_nTrackCount = 0;
  pme->m_TransId = 0;
#endif //#ifdef FEATURE_QTV_GENERIC_BCAST

#if defined(MP_FEATURE_VIDEO) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))
  // retrieve default PDP settings
  if(pme->m_NetPolicy.m_bDefaultProfile || pme->m_NetPolicy.m_nProfile == 0)
  {
    // retrieve default PDP
    pme->m_NetPolicy.m_bDefaultProfile = TRUE;
    pme->m_NetPolicy.m_nProfile = MPPDPGetDefaultProfile(pme);
  }
  pme->m_NetPolicy.m_nNumPDP = MPPDPGetNumProfiles(pme);
#endif

  pme->m_bResumeInProcessing = FALSE;
  pme->m_bSuspendPending = FALSE;

  pme->m_cxWidthFull = 0;
  pme->m_cyHeightFull = 0;

  pme->m_bInTestAutomation = FALSE;

#if defined(MP_FEATURE_MBMS)
  pme->m_bShowExtension = FALSE;
#else
  pme->m_bShowExtension = TRUE;
#endif

#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)
  pme->m_bReplayAtStop = 0;
  pme->m_strReplayFileName[0] = '\0';
#endif //#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)

  pme->m_pRecIcon = ISHELL_LoadResBitmap(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDB_RECORDING_ON);
  pme->m_pDlg = NULL;

  MP_RELEASEIF(pIFileMgr);

  pme->m_bCallInProgress = FALSE;

  return TRUE;
}


/*===========================================================================
Function:  CMediaPlayer_FreeAppData()

Description:
  This function frees the app data. This function is registered with the
  applet framework when the applet is created (inside AEEClsCreateInstance()
  function).  This function is called by the app framework when the reference
  count of the applet reaches zero. This function must free all the app data
  that has been allocated by this app. For ex: if their is data that was
  remembered when the app is suspended and resumed, those data must be freed
  here.

Prototype:
  void CMediaPlayer_FreeAppData(IApplet* po)

Parameters:
  IApplet* po: Pointer to the applet structure. This is of the type defined
  to store applet data. (CMediaPlayer*, in this case).

Return Value:
  None

Comments:  None

Side Effects: None
==============================================================================*/
void
CMediaPlayer_FreeAppData(IApplet *po)
{
  CMediaPlayer *pme = (CMediaPlayer *)po;

  DBGPRINTF_HIGH("MP: starting CMediaPlayer_FreeAppData.");

#ifdef FEATURE_MP_EQ_UI
  // release and cleanup.
  if(pme->m_pSetting)
  {
    MPSettingRelease(pme->m_pSetting);
    FREE(pme->m_pSetting);
    pme->m_pSetting = NULL;
  }
#endif

  CMediaPlayer_CancelRedraw(pme);
  DBGPRINTF_MED("MP: Calling MP_RELEASEWIN");
  MP_RELEASEWIN(pme->m_pWin);

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
  MPContentFree(pme->m_pContent);
  pme->m_pContent = NULL;
#endif // #if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

  FREEIF(pme->m_pszAudioExt);
  FREEIF(pme->m_pszVideoExt);
  FREEIF(pme->m_pszImageExt);
  MP_RELEASEIF(pme->m_pIRadioOff);
  MP_RELEASEIF(pme->m_pIRadioOn);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION

#ifdef FEATURE_APP_MPEG4
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  FREEIF(HttpDownloader_sharedBuffer);
#endif
#endif

#ifdef FEATURE_BT_EXTPF_AV
   (void)ISHELL_RegisterNotify( pme->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                          AEECLSID_BLUETOOTH_NOTIFIER,
                          0 );
#endif     // FEATURE_BT_EXTPF_AV

   // Unregister for ITelephone events
   (void)ISHELL_RegisterNotify( pme->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                          AEECLSID_PHONENOTIFIER,
                          0 );
    DBGPRINTF_MED("MP: unregisterred for ITelephone events");

  if(pme->m_pDlg)
  {
    MPDlgDelete(pme->m_pDlg);
    pme->m_pDlg = NULL;
  }

  CMediaPlayer_FreeExplorerData(pme);

  if (pme->m_pRecIcon)
  {
    IBITMAP_Release(pme->m_pRecIcon);
  }

  pme->m_bCallInProgress = FALSE;
}


/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
  This function is invoked while the app is being loaded. All Modules must
  provide this function. Ensure to retain the same name and parameters for
  this function.  In here, the module must verify the ClassID and then
  invoke the AEEApplet_New()function that has been provided in AEEAppGen.c.

  After invoking AEEApplet_New(), this function can do app specific
  initialization. In this example, a generic structure is provided so that
  app developers need not change app specific initialization section every
  time.

PROTOTYPE:
  int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po,
                           void ** ppObj)

PARAMETERS:
  clsID: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell interface.

  pIModule: pin]: Contains pointer to the IModule interface to the current
  module to which this app belongs

  ppObj: [out]: On return, *ppApplet must point to a valid IBase object.
  If the classID  represnts an applet, then ppObj must point to a valid
  AEEApplet structure.Allocation of memory for this structure and initializing
  the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the class/app creation was successful.
  EFAILED: Error occurred while creating the class/app. In this case, the
  app/class will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
int
CMediaPlayer_CreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po,
                            void **ppObj)
#else
int
AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po,
                     void **ppObj)
#endif
{
  *ppObj = NULL;

  /*
   * Here a check is done to see if the ClsID is that of MediaPlayer app.
   * The reason is if this module has more than one applets or classes, then
   * this function is invoked once for each applet or class. Checking here
   * ensures that the correct IApplet or class object is constructed.
   */
  if (ClsId == AEECLSID_MEDIAPLAYER)
  {
    /*
     * Create the applet
     */
    if (AEEApplet_New(sizeof(CMediaPlayer), ClsId, pIShell, po,
                      (IApplet **)ppObj, (AEEHANDLER)CMediaPlayer_HandleEvent,
                      (PFNFREEAPPDATA)CMediaPlayer_FreeAppData))
    {
      /*
       * Initialize applet data
       */
      if (CMediaPlayer_InitAppData((IApplet *)*ppObj))
      {
        /*
         * Data initialized successfully
         */
        return (AEE_SUCCESS);
      }
      else
      {
        /*
         * Release the applet. This will free the memory allocated for
         * the applet when
         */
        (void)IAPPLET_Release(((IApplet *)*ppObj));
        return EFAILED;
      }

    }                                  // AEEApplet_New

  }                                    // ClsId == AEECLSID_MEDIAPLAYER

  return (EFAILED);
}

#if !defined(FEATURE_APP_MEDIA_VIDEO)
/*******************************************************************
// CheckForPendingReposition checks to see if a skip or a seek
// needs to be done, and does it.
// It returns TRUE if a skip/seek was done, or if the underlying media
// was already in the process of skipping/seeking. (so that we know
// not to re-set any of the skip/seek vars
********************************************************************/
static boolean CheckForPendingReposition(CPlayerWin *pme)
{
  boolean didReposition = FALSE;

  // Do not attempt to reposition if we are currently suspended
  if (!pme->m_pOwner->m_bSuspend)
  {
#if defined(FEATURE_QTV_RANDOM_ACCESS_REPOS) || defined(FEATURE_QTV_PLAYLIST)
    /* Skips take precedence over seeks and are processed first. */
    if (pme->m_reskip)
    {
      // set SkipPending to FALSE only if reskip is true.  If we always do it
      // we run into race conditions with built up BUFFERING callbacks
      if (pme->m_skipPending)
      {
        pme->m_skipPending = FALSE;
        DBGPRINTF_MED("MP: CheckForPendingReposition skipPending is TRUE, setting it to false");
      }

      // If we need to reskip but everything has been canceled out,
      // don't do anything
      if (pme->m_skipOffset)
      {
#if MIN_BREW_VERSION(3, 0)
        AEEMediaSeek skipReference = MM_SEEK_MODE_CHAPTER;
#else
        AEEMediaSeek skipReference = MM_SEEK_ACCESS_POINT;
#endif
        int  nRet;
        int  nSkipAmount = pme->m_skipOffset;
#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
        if (MM_MEDIA_WM_SERVER_SIDE_PLAYLIST == pme->m_mediaInfo.streamtype)
        {
          // WM Playlists can only skip one file/chapter at a tim
          // so we need to skip by -1/1 and them move the built
          // up m_skipOffset closer to 0;
          nSkipAmount = (pme->m_skipOffset) ? 1 : -1;
        }
#endif
        pme->m_skipOffset -= nSkipAmount;

        nRet = IMEDIA_Seek(pme->m_pMedia, skipReference  | MM_SEEK_CURRENT,
                          (MP_ACCESS_POINT_SKIP_STEP * nSkipAmount));
        DBGPRINTF_MED("MP: CheckForPendingReposition IMEDIA_Seek(%d) for ReSkip returns %d",
                      (MP_ACCESS_POINT_SKIP_STEP * nSkipAmount), nRet, 0);
        if ((nRet == SUCCESS) || (nRet == EITEMBUSY))
        {
            pme->m_skipPending = TRUE;
            didReposition = TRUE;
        }
        else if (nRet == EALREADY)
        {
          // We are in the process of skiping, revert to the previous offset
          // and see if we can skip later
          DBGPRINTF_HIGH("MP: CheckForPendingReposition IMEDIA_Seek is in the process, reverting our skip attempt!");
          pme->m_skipOffset += nSkipAmount;
          pme->m_skipPending = TRUE;
          didReposition = TRUE;
        }
        else
        {
          DBGPRINTF_HIGH("MP: CheckForPendingReposition IMEDIA_Seek failed!");
        }
      }
      // If we finished off the skipping, set reskip to false
      if (pme->m_skipOffset == 0)
      {
        pme->m_reskip = FALSE;
      }
      if (pme->m_skipPending)
      {
        // Since we skipped we cannot do any re-seeking below. so return and do it later
		    return TRUE;
      }
    }
    else
    {
      // reskip is false, make sure our skip offset is 0.
      pme->m_skipOffset = 0;
    }
#endif  // FEATURE_QTV_RANDOM_ACCESS_REPOS || FEATURE_QTV_PLAYLIST

    if (pme->m_reseek)
    {
      int offset = 0;
      int nRet;
      // set SeekPending to FALSE only if reseek is true.  If we always do it
      // we run into race conditions with built up BUFFERING callbacks
      if (pme->m_seekPending)
      {
        pme->m_seekPending = FALSE;
        DBGPRINTF_MED("MP: CheckForPendingReposition m_seekPending is TRUE, setting it to false");
      }

      DBGPRINTF_MED("MP: Calling ComputeSeekOffset");
      offset = ComputeSeekOffset(pme);


      nRet = IMEDIA_Seek(pme->m_pMedia, MM_SEEK_START, offset);
      DBGPRINTF_MED("MP: CheckForPendingReposition IMEDIA_Seek(%d) for ReSeek returns %d = %s", offset, nRet, MP_ErrorStateToString(nRet));
      if ((nRet == SUCCESS) || (nRet == EITEMBUSY))
      {
        pme->m_seekPending = TRUE;
        pme->m_reseek = 0;
        DBGPRINTF_MED("MP: Setting m_pendingSeekOffset from %d to 0", pme->m_pendingSeekOffset);
        pme->m_pendingSeekOffset = 0;
        // Update the StartSeekPosition to point to the location that we are seeking to
        // That way if they press any more seeks while we are processing this one,
        // they will happen from where we are going to, not where our first seeks were from
        pme->m_startSeekPosition = offset;
        didReposition = TRUE;

      }
      else if (nRet == EALREADY)
      {
        pme->m_seekPending = TRUE;
        DBGPRINTF_HIGH("MP: CheckForPendingReposition IMEDIA_Seek is in the process, will try again later!");
        didReposition = TRUE;
      }
      else
      {
        DBGPRINTF_HIGH("MP: CheckForPendingReposition IMEDIA_Seek failed!");
        // Update everything to 0, like when we get a SEEK_FAIL callback
        pme->m_seekPending = FALSE;
        pme->m_reseek = pme->m_pendingSeekOffset = 0;
      }
      //pme->m_nextClipOffsetAdj += offset;
      pme->m_nextClipOffsetAdj = 0;
    }
  }

  return didReposition;
}
#endif //!defined (FEATURE_APP_MEDIA_VIDEO)

/*===========================================================================

FUNCTION CMediaPlayer_HandleEvent

DESCRIPTION
  This is the EventHandler for this app. All events to this app are handled
  in this function. All APPs must supply an Event Handler.

PROTOTYPE:
  boolean CMediaPlayer_HandleEvent(IApplet *pi, AEEEvent eCode, uint16 wParam,
                                   uint32 dwParam)

PARAMETERS:
  pi: Pointer to the AEEApplet structure. This structure contains information
  specific to this applet. It was initialized during the AppClsCreateInstance()
  function.

  ecode: Specifies the Event sent to this applet

  wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean
CMediaPlayer_HandleEvent(IApplet *pi, AEEEvent eCode, uint16 wParam,
                         uint32 dwParam)
{
  CMediaPlayer *pme = (CMediaPlayer *)pi;

#ifdef FEATURE_QTV_DIAG_CMD
  IClipboard *clBd = NULL;
  void     *input_data;                // get data from diag
  char      clipboardEntryName[CLIPBOARD_NAME_MAX_LENGTH];

  uint32    len;
  IShell   *shell = NULL;

  len = dwParam;
#endif

  switch (eCode)
  {
  case EVT_APP_START:                 // Process Start event
  {
    AEEAppStart *pas = (AEEAppStart *)dwParam;

    if (pme->m_bBackgroundMode)
    {
      // Resume the media
      CMediaPlayer_ProcessResumeEvent(pme);
      pme->m_bBackgroundMode = FALSE;
      return TRUE;
    }

    if (!CMediaPlayer_Initialization(pme))
    {
      AECHAR    szBufTitle[100];
      AECHAR    szBufText[100];

      (void)STRTOWSTR("[ERROR]", szBufTitle, sizeof(szBufTitle));
      (void)STRTOWSTR("Please load mediaplayer.bar on EFS.", szBufText,
                sizeof(szBufText));
      (void)ISHELL_MessageBoxText(pme->a.m_pIShell, szBufTitle, szBufText);
      return TRUE;
    }

    /*
     * An app can be started from app manager or from another app
     * with arguments.  dwParam contains the string argument passed
     * to the applet when it is started.  In case of MediaPlayer app,
     * if the argument is present, then it means that the applet is
     * started as a plugin with argument specifying file name of the
     * media. In this case, the player window is directly invoked
     * and the media starts playing immediately.
     */

    if (pas->pszArgs)
    {
       char* pStrArgs = (char*)pas->pszArgs;
#ifdef FEATURE_DTV_DVBH
       char* pStrLink = NULL;
#endif //#ifdef FEATURE_DTV_DVBH
       /*
       * Set Plugin mode flag to TRUE
       */
      pme->m_bPlugin = TRUE;
      DBGPRINTF_MED("MP: MediaPlayer is in plug-in mode. pszArgs = %s", pStrArgs);

      /*
       * Save current continuos mode settings
       */
      pme->m_bSavedContMode = pme->m_nContMode;

      /*
       * Turn off Continuous mode in plug-in mode
       */
      pme->m_nContMode = CONT_MODE_OFF;

#ifdef FEATURE_DTV_DVBH
      {

        int i;
        char* p0;
        char* p;
        int nLength;

        // parse the input string to extract the file path and descrambler key for DVB-H video
        p0 = pStrArgs;
        p = STRSTR(p0, (const char*)"?");
        if(NULL != p)
        {
          nLength = p - p0;

          p++;
          pme->m_DescramblerKey = ATOI(p);
          DBGPRINTF_HIGH("MP: DVB-H descrambler key = %d.", pme->m_DescramblerKey);

          pStrLink = MALLOC(nLength + 1);
          if(pStrLink)
          {
            STRLCPY(pStrLink, p0,  nLength + 1);
            pStrArgs = pStrLink;

            DBGPRINTF_HIGH("MP: DVB-H link string = %s.", pStrLink);
          }
          else
          {
             DBGPRINTF_ERROR("MP: failed to allocate memory.");
             return TRUE;
          }
        }
        else
        {
           DBGPRINTF_HIGH("MP: found no key in DVB-H link string, %s", pStrArgs);
        }

        // these codes will allow playNext and playPrevious feature to browse the files
        // in fs:/shared/media folder

        // initialize explore
        CMediaPlayer_InitializeExplorer(pme);
        // refresh explore to fill data
        pme->m_FileExplorer.m_nCurDepth = 1;
        CMediaPlayer_RefreshExplorer(pme, "fs:/shared/media");

        pme->m_CurFileIndex = 0;
        // find current file index
        for (i = 0; i < pme->m_FileExplorer.m_NumFiles; i++)
        {
          if(STRCMP(pme->m_FileExplorer.m_FileArray[i].m_szName, pStrArgs) == 0)
          {
            pme->m_CurFileIndex = i;
            break;
          }
        }
      }
#endif //FEATURE_DTV_DVBH


      if (STRCMP(pStrArgs, "PV_VOICE_MEMO") == 0)
        /*
         * launched from PureVoice to record voice memo
         */
      {
        CMediaPlayer_RecordQCPFile(pme, MP_QCP_REC_FILE);
        (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                         IDM_PM_RECORD, 0);
      }
      else
      {
        /*
         * QTV_PSEUDO_STREAMING_BEGIN
         */
        /*
         * play files
         */
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        pme->m_bIsHTTP = FALSE;
        if (NULL != STRSTR(pStrArgs, "URL="))
        {
          pme->m_bIsHTTP = TRUE;
          if (!pme->m_bKddiExt)
          {
            char      destUrl[PS_MAX_URL_LEN] = { '\0' };
            Search_And_Store(destUrl, pStrArgs, "URL=", PS_MAX_URL_LEN);
            (void)CMediaPlayer_PseudoPlayFile(pme, destUrl);
            // "mmc1/media/pstream/ps_ex_f7_v24_f75_amr122-3g2.url");
          }
          else
          {
            (void)CMediaPlayer_KDDIPseudoPlayFile(pme, pStrArgs);
          }
        }
        else if (FALSE == pme->m_bEnablePSSimulation && FALSE == pme->m_bFileWriteReadTest)
        {
          DBGPRINTF_MED("MP: FALSE == pme->m_bEnablePSSimulation && FALSE == pme->m_bFileWriteReadTest");
          (void)CMediaPlayer_PlayFile(pme, pStrArgs);
        }
        else
        {
          DBGPRINTF_MED("MP: CMediaPlayer_PseudoPlayFile(pme, pStrArgs)");
          (void)CMediaPlayer_PseudoPlayFile(pme, pStrArgs);
        }
#else
        (void)CMediaPlayer_PlayFile(pme, pStrArgs);
#endif     // FEATURE_QTV_PSEUDO_STREAM ||
           // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif     // FEATURE_APP_MPEG4

      }


#ifdef FEATURE_DTV_DVBH
      FREEIF(pStrLink);
#endif //#ifdef FEATURE_DTV_DVBH

    }
    else
    {
      CMediaPlayer_DrawSplash(pme);
    }

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif     // FEATURE_QTV_MFDRM

    return TRUE;
  }

  case EVT_DIALOG_INIT:
    pme->m_State = MP_STATE_SETTING;
    EMIT_MEDIAPLAYER_STATE(pme->m_State);
#ifdef FEATURE_MP_SEEK_TIME_DLG
    if (wParam == IDD_SEEK_TIME_SETTTING)
    {
      IDIALOG_SetEventHandler((IDialog *)dwParam,
                              SeekTimeSettingDlgEventHandler, pme);
      IDIALOG_SetProperties((IDialog *)dwParam, DLG_HANDLE_ALL_EVENTS);
      return TRUE;
    }
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    if (wParam == IDD_3GPP_SETTINGS)
    {
      IDIALOG_SetEventHandler((IDialog *)dwParam,
                              ProgDnldSettingsDlgEventHandler, pme);
      IDIALOG_SetProperties((IDialog *)dwParam, DLG_HANDLE_ALL_EVENTS);
      return TRUE;
    }
#endif

#if defined(MP_FEATURE_VIDEO) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))
    if (wParam == IDD_QOS_UMTS_SETTINGS)
    {
      IDIALOG_SetEventHandler((IDialog *)dwParam, MPPDPSettingDlgEventHandler, pme);
      IDIALOG_SetProperties((IDialog *)dwParam, DLG_HANDLE_ALL_EVENTS);
      return TRUE;
    }
#endif

#ifdef FEATURE_QTV_MDP_ASCALE
    if (wParam == IDD_ASCALE_SETTINGS)
    {
      IDIALOG_SetEventHandler((IDialog *)dwParam, AScaleSettingDlgEventHandler, pme);
      IDIALOG_SetProperties((IDialog *)dwParam, DLG_HANDLE_ALL_EVENTS);
      return TRUE;
    }
#endif

#ifdef MP_FEATURE_QTV_DIVX_DRM  
    if (wParam == IDD_DIVX_REG_INFO)
    {
      IDIALOG_SetEventHandler((IDialog *)dwParam, DivxRegCodeDlgEventHandler, pme);
      IDIALOG_SetProperties((IDialog *)dwParam, DLG_HANDLE_ALL_EVENTS);
      return TRUE;
    }
#endif

    if (wParam == IDD_KEYMAPPINGS)
    {
      IDIALOG_SetEventHandler((IDialog *)dwParam, KeyMappingsDlgEventHandler, pme);
      IDIALOG_SetProperties((IDialog *)dwParam, DLG_HANDLE_ALL_EVENTS);
      return TRUE;
    }

#ifdef FEATURE_MMOVERLAY
    if (wParam == IDD_ALPHA_BLEND_SETTING)
    {
      IDIALOG_SetEventHandler((IDialog *)dwParam, AlphaBlendSettingDlgEventHandler, pme);
      IDIALOG_SetProperties((IDialog *)dwParam, DLG_HANDLE_ALL_EVENTS);
      return TRUE;
    }
#endif

    if (DlgEventHandlers[wParam - 6000] != NULL)
    {
      DBGPRINTF_MED("MP: Menu Init for wParam=%d, dwParam=%d, handler=0x%x", wParam, dwParam, DlgEventHandlers[wParam - 6000]);
      IDIALOG_SetEventHandler((IDialog *)dwParam, DlgEventHandlers[wParam - 6000], pme);
      IDIALOG_SetProperties((IDialog *)dwParam, DLG_HANDLE_ALL_EVENTS);
      return TRUE;
    }
    return FALSE;
  case EVT_APP_BROWSE_FILE:
  {
    /*
     * This event is sent when ISHELL_BrowseFile()API finds
     * MediaPlayer app as the registered handler. Shell first
     * sends EVT_APP_START followed by this event. The dwParam
     * contains the file name.
     * Start the MediaPlayer app in plugin mode.
     */
    if (dwParam && !pme->m_bPlugin)
    {
      char     *psz = (char *)dwParam;

      (void)CMediaPlayer_SetWindow(pme, MPW_NONE, 0);
      pme->m_bPlugin = TRUE;
      DBGPRINTF_MED("MP: MediaPlayer is in plug-in mode.");

      /*
       * QTV_PSEUDO_STREAMING_BEGIN
       */
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
      pme->m_bIsHTTP = FALSE;
      if (FALSE == pme->m_bEnablePSSimulation && FALSE == pme->m_bFileWriteReadTest)
      {
        (void)CMediaPlayer_PlayFile(pme, psz);
      }
      else
      {
        (void)CMediaPlayer_PseudoPlayFile(pme, psz);
      }
#else
      (void)CMediaPlayer_PlayFile(pme, psz);
#endif
      /*
       * QTV_PSEUDO_STREAMING_END
       */
    }
    return TRUE;
  }

  case EVT_APP_STOP:                  // process STOP event
    // Go into background mode if background is enabled and if we have a window that is not the MAIN window.
    // This lets us do a full exit if we exit from the MAIN window and cleanup structures, etc.
    if((pme->m_bPlayAudioInBackground) && ((pme->m_eActiveWin != MPW_MAIN) && (pme->m_pWin))) 
    { 
      {
         pme->m_bBackgroundMode = TRUE;
         CMediaPlayer_ProcessSuspendEvent(pme);
         // If background, set dwParam to false to have MP put into background
         DBGPRINTF_MED("MP: EVT_APP_STOP play in background");
         *((boolean *)dwParam) = FALSE;
      }
    } 

    if (TRUE == pme->m_bPlugin)
    {
      /*
       * If operating in plug-in mode
       */

      /*
       * Revert back to the continuous mode settings
       * present before entering plug-in mode
       */
      pme->m_nContMode = pme->m_bSavedContMode;

    }

    (void)MPConfigFile_Write(pme); 
    return (TRUE);

  case EVT_APP_SUSPEND:
   DBGPRINTF_MED("MP: EVT_APP_SUSPEND received.");
   CMediaPlayer_ProcessSuspendEvent(pme);
   return TRUE;

#ifdef FEATURE_QTV_DIAG_CMD
  case EVT_MEDIAPLAYER_SELECT_FILE:

    /*
     * initialize
     */
    input_data = (void *)MALLOC(sizeof(MPEventData));
    if (!input_data)
    {
      /*
       * exit on allocation failure
       */
      return (FALSE);
    }
    shell = AEE_GetShell();
    if (ISHELL_CreateInstance(shell, AEECLSID_CLIPBOARD, (void **)(&clBd)) !=
        SUCCESS)
    {
      /*
       * clear up allocated memory before exiting
       */
      FREE(input_data);
      return (FALSE);
    }
    (void)MEMSET(((MPEventData *)input_data)->m_inputString, 0x00,
           MP_MAX_STRING_LENGTH);
    (void)SNPRINTF(clipboardEntryName, sizeof(clipboardEntryName), "AUTOMATION_%d", wParam);
    /*
     * get data from clipboard
     */
    if (ICLIPBOARD_GetData
        (clBd, clipboardEntryName, (void *)input_data, (int *)&len) != SUCCESS)
    {
      /*
       * clear up allocated memory before exiting
       */
      FREE(input_data);
      return (FALSE);
    }

    CMediaPlayer_FillExplorerData(pme,
                                  ((MPEventData *)input_data)->m_inputString);
    (void)CMediaPlayer_PlayFile(pme, ((MPEventData *)input_data)->m_inputString);

    /*
     * finished task. Cleanup..
     */
    FREE(input_data);
    (void)ICLIPBOARD_SetData(clBd, clipboardEntryName, 0, 0);
    event_report((event_id_enum_type)DMSSEVT_SELECT_FILE);
    return (TRUE);

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  case EVT_MEDIAPLAYER_RTSP_STR_REC_FILE_NAME:
    /*
     * initialize
     */
    input_data = (void *)MALLOC(sizeof(MPEventData));
    if (!input_data)
    {
      /*
       * exit on allocation failure
       */
      return (FALSE);
    }
    shell = AEE_GetShell();
    if (ISHELL_CreateInstance(shell, AEECLSID_CLIPBOARD, (void **)(&clBd)) !=
        SUCCESS)
    {
      /*
       * clean up on exit
       */
      FREE(input_data);
      return (FALSE);
    }
    (void)MEMSET(((MPEventData *)input_data)->m_inputString, 0x00,
           MP_MAX_STRING_LENGTH);
    (void)SNPRINTF(clipboardEntryName, sizeof(clipboardEntryName), "AUTOMATION_%d", wParam);
    /*
     * get data from clipboard
     */
    if (ICLIPBOARD_GetData
        (clBd, clipboardEntryName, (void *)input_data, (int *)&len) != SUCCESS)
    {
      /*
       * clean up on exit
       */
      FREE(input_data);
      return (FALSE);
    }
    (void)STRLCPY(recFileName, ((MPEventData *)input_data)->m_inputString, MP_MAX_STRING_LENGTH);
    DBGPRINTF_MED("MP: Record File Name %s", recFileName);

    /*
     * finished task. Cleanup..
     */
    FREE(input_data);
    ICLIPBOARD_SetData(clBd, clipboardEntryName, 0, 0);
    event_report((event_id_enum_type)DMSSEVT_SET_DFLT_RTSP_STR_REC_DIR);
    return (TRUE);

#endif

  case EVT_MEDIAPLAYER_CONTROL:

    /*
     * initialize to null
     */
    input_data = (void *)MALLOC(sizeof(int));
    if (!input_data)
    {
      /*
       * exit on allocation failure
       */
      return (FALSE);
    }
    shell = AEE_GetShell();
    if (ISHELL_CreateInstance(shell, AEECLSID_CLIPBOARD, (void **)(&clBd)) !=
        SUCCESS)
    {
      /*
       * clean up on exit
       */
      FREE(input_data);
      return (FALSE);
    }

    (void)MEMSET(((int *)input_data), 0x00, sizeof(int));
    (void)SNPRINTF(clipboardEntryName, sizeof(clipboardEntryName), "AUTOMATION_%d", wParam);
    if (ICLIPBOARD_GetData
        (clBd, clipboardEntryName, (void *)input_data, (int *)&len) != SUCCESS)
    {
      /*
       * clean up on exit
       */
      FREE(input_data);
      return (FALSE);
    }

    switch (*((int *)input_data))
    {
    case CMD_DIAG_PLAY:               // Play file is 1
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_PLAY, 0);
      break;

    case CMD_DIAG_STOP:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_STOP, 0);
      break;

    case CMD_DIAG_REWIND:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_REWIND, 0);
      break;

    case CMD_DIAG_FF:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_FF, 0);
      break;

    case CMD_DIAG_PAUSE:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_PAUSE, 0);
      break;

    case CMD_DIAG_FF_STEP:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_FF_STEP, 0);
      break;

    case CMD_DIAG_BK_STEP:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_BK_STEP, 0);
      break;

#if defined(FEATURE_QTV_RANDOM_ACCESS_REPOS) || defined(FEATURE_QTV_PLAYLIST)
    case CMD_DIAG_FF_SKIP:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_FF_SKIP, 0);
      break;

    case CMD_DIAG_BK_SKIP:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_BK_SKIP, 0);
      break;
#endif     // FEATURE_QTV_RANDOM_ACCESS_REPOS

    case CMD_DIAG_VOL_UP:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_UP, 0);
      break;

    case CMD_DIAG_VOL_DOWN:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_DOWN, 0);
      break;

    case CMD_DIAG_FULLSCRN:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_FULLSCRN, 0);
      break;

    case CMD_DIAG_INFO:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_INFO, 0);
      break;

    case CMD_DIAG_CCW_ROTATE:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_CCW_ROTATE, 0);
      break;

    case CMD_DIAG_CW_ROTATE:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_CW_ROTATE, 0);
      break;

    case CMD_DIAG_ZOOM_IN:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_ZOOM_IN, 0);
      break;

    case CMD_DIAG_ZOOM_OUT:
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND,
                       IDM_PM_ZOOM_OUT, 0);
      break;

    case CMD_DIAG_CONT_SINGLE:
      pme->m_nContMode = CONT_MODE_SINGLE_FILE;
      break;

    case CMD_DIAG_CONT_ALL:
      pme->m_nContMode = CONT_MODE_ALL_FILES;
      break;

    case CMD_DIAG_AUTOPLAY_ON:
      pme->m_bAutoPlay = 1;
      break;

    case CMD_DIAG_AUTOPLAY_OFF:
      pme->m_bAutoPlay = 0;
      break;

    case CMD_DIAG_CONT_OFF:
      pme->m_nContMode = CONT_MODE_OFF;
      break;

#ifdef MM_MP4_PARAM_MODIFY_BUFFER_LOWMARK
    case CMD_DIAG_MODIFY_BUFFER_LOWMARK:
      (void)IMEDIA_SetMediaParm(((CPlayerWin *)(pme->m_pWin))->m_pMedia,
                          MM_MP4_PARAM_MODIFY_BUFFER_LOWMARK, (uint32)wParam,
                          (int32)0);
      break;
#endif

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
      /*
       * Start recording RTSP Stream
       * This 'Start' does not care for media player stream
       * recording settings
       * Assume that media already exists
       */
    case CMD_DIAG_RTSP_STR_REC_ON:
      CPlayerWin_StartStreamRecord((CPlayerWin *)(pme->m_pWin), TRUE);
      break;

      /*
       * Turn on RTSP Stream recording in mediaplayer settings
       */
    case CMD_DIAG_UI_SETTINGS_RTSP_STR_REC_ON:
      pme->m_bRTSPstreamRec = TRUE;
      bRTSPstreamRecord = TRUE;
      break;

      /*
       * Turn off RTSP Stream recording in mediaplayer settings
       */
    case CMD_DIAG_UI_SETTINGS_RTSP_STR_REC_OFF:
      pme->m_bRTSPstreamRec = FALSE;
      bRTSPstreamRecord = FALSE;
      break;


      /*
       * Stop recording RTSP Stream. This operation does not care for
       * media player UI settings
       */
    case CMD_DIAG_RTSP_STR_REC_OFF:
      CPlayerWin_StopStreamRecord((CPlayerWin *)(pme->m_pWin));
      break;

    case CMD_DIAG_RTSP_STR_REC_FILE_OW_ON:     // Turn on file
                                                // overwriting
      pme->m_bRTSPstreamRecFileOverwrite = TRUE;
      bRTSPstreamRecFileOverwrite = TRUE;
      break;

    case CMD_DIAG_RTSP_STR_REC_FILE_OW_OFF:    // Turn off file
                                                // overwriting
      pme->m_bRTSPstreamRecFileOverwrite = FALSE;
      bRTSPstreamRecFileOverwrite = FALSE;
      break;

#endif

    case CMD_DIAG_TERMINATE:
      /*
       * ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_TERMINATE, 0);
       */
      pme->m_eActiveWin = MPW_PLAYER;
      DBGPRINTF_MED("MP: Setting m_eActiveWin to MPW_PLAYER");
      (void)CMediaPlayer_SetWindow(pme, MPW_MAIN, 0);
      break;

    default:
      break;
    }
    /*
     * finished task. Cleanup..
     */
    FREE(input_data);
    (void)ICLIPBOARD_SetData(clBd, clipboardEntryName, 0, 0);
    event_report((event_id_enum_type)DMSSEVT_MEDIAPLAYER_CONTROL);
    return (TRUE);
#endif

  case EVT_APP_RESUME:
    CMediaPlayer_ProcessResumeEvent(pme);
      return TRUE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION

    case EVT_NOTIFY:
      {
        AEENotify *pN = (AEENotify*) dwParam;

#ifdef FEATURE_BT_EXTPF_AV

        NotificationData *pData;
        AEEEvent evt;

        if(pN == NULL)
        {
          DBGPRINTF_ERROR("MP: NULL ptr on receiving EVT_NOTIFY");
          return FALSE;
        }

        pData = pN->pData;
        if(pData == NULL)
        {
          DBGPRINTF_ERROR("MP: NULL ptr on receiving EVT_NOTIFY, pN=0x%lx", pN);
          return FALSE;
        }

        evt = GET_NOTIFIER_VAL(pN->dwMask);
        if ( pN->cls == AEECLSID_BLUETOOTH_NOTIFIER
             && GET_NOTIFIER_MASK(pN->dwMask) == NMASK_BT_AVRCP
             && pData->AvrcpOp.opStatus == AEEBT_AVRCP_OP_PRESSED )
        {
          switch( evt )
          {
          case AEEBT_AVRCP_EVT_OP_VOLUME_UP:
            DBGPRINTF_MED("MP: Got AVRCP Volume Down");
            (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_UP, 0);
            break;
          case AEEBT_AVRCP_EVT_OP_VOLUME_DOWN:
            DBGPRINTF_MED("MP: Got AVRCP Volume Up");
            (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_DOWN, 0);
            break;
          case AEEBT_AVRCP_EVT_OP_MUTE:
            DBGPRINTF_MED("MP: Got AVRCP Mute: Ignoring");
            break;
          case AEEBT_AVRCP_EVT_OP_PLAY:
            DBGPRINTF_MED("MP: Got AVRCP Play");
            (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
            break;
          case AEEBT_AVRCP_EVT_OP_STOP:
            DBGPRINTF_MED("MP: Got AVRCP Stop");
            (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_STOP, 0);
            break;
          case AEEBT_AVRCP_EVT_OP_PAUSE:
            DBGPRINTF_MED("MP: Got AVRCP Pause");
            if (pme->m_eActiveWin == MPW_PLAYER)
            {
              CPlayerWin_PausePlay((CPlayerWin*)pme->m_pWin);
            }
#ifdef FEATURE_MULTISEQUENCER
            else if (pme->m_eActiveWin == MPW_SEQUENCE_PLAYER)
            {
              CSequencePlayerWin_PausePlay((CSequencePlayerWin *)pme->m_pWin);
            }
#endif // FEATURE_MULTISEQUENCER
            break;
          case AEEBT_AVRCP_EVT_OP_REWIND:
            DBGPRINTF_MED("MP: Got AVRCP Rewind");
            (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_REWIND, 0);
            break;
          case AEEBT_AVRCP_EVT_OP_FAST_FORWARD:
            DBGPRINTF_MED("MP: Got AVRCP Fast Forward");
            (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_FF, 0);
            break;
          case AEEBT_AVRCP_EVT_OP_FORWARD:
            /* This should go to the next track.
             * Will be implemented once playlists are supported.
             * For now, map to fast forward */
            // Currently this is FF File, as requested by pconcha.  Leaving the comment above in case things might need
            // to change
            DBGPRINTF_MED("MP: Got AVRCP Forward");
            (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_FF_FILE, 0);
            break;
          case AEEBT_AVRCP_EVT_OP_BACKWARD:
            /* This should go to the previous track.
             * Will be implemented once playlists are supported.
             * For now, map to rewind */
            // Currently this is BK File, as requested by pconcha.  Leaving the comment above in case things might need
            // to change
            DBGPRINTF_MED("MP: Got AVRCP Backward");
            (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_BK_FILE, 0);
            break;
          default:
            DBGPRINTF_HIGH("MP: Unhandled AVRCP Command 0x%x",evt);
            return FALSE;
          }
        }
        else if (pN->cls == AEECLSID_BLUETOOTH_NOTIFIER
                 && GET_NOTIFIER_MASK(pN->dwMask) == NMASK_BT_A2DP )
        {
          switch( evt )
          {
            case AEEBT_A2DP_EVT_START:
              if ( pData->A2DPStartInit == AEEBT_A2DP_EVENT_REMOTE_INIT )
              {
                DBGPRINTF_MED("MP: Got A2DP Start");
                (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
              }
              break;
            case AEEBT_A2DP_EVT_SUSPEND:
              if ( pData->A2DPSuspendInit == AEEBT_A2DP_EVENT_REMOTE_INIT )
              {
                DBGPRINTF_MED("MP: Got A2DP Suspend");
                if (pme->m_eActiveWin == MPW_PLAYER)
                {
                  CPlayerWin_PausePlay((CPlayerWin*)pme->m_pWin);
                }
#ifdef FEATURE_MULTISEQUENCER
                else if (pme->m_eActiveWin == MPW_SEQUENCE_PLAYER)
                {
                  CSequencePlayerWin_PausePlay((CSequencePlayerWin *)pme->m_pWin);
                }
#endif // FEATURE_MULTISEQUENCER
              }
              break;
            default:
              DBGPRINTF_HIGH("MP: Unhandled A2DP Command 0x%x",evt);
              return FALSE;
          }
        }
        // Handle audio resume in case of brief playback stop due to ringer interruption
        else if (pN->cls == AEECLSID_PHONENOTIFIER
                 && ( (GET_NOTIFIER_MASK(pN->dwMask) & (AEET_NMASK_VOICE_CALL || AEET_NMASK_NEW_CALLDESC)) != 0) )
#else     // FEATURE_BT_EXTPF_AV

        if(pN == NULL)
        {
          DBGPRINTF_ERROR("MP: NULL ptr on receiving EVT_NOTIFY");
          return FALSE;
        }

        if (pN->cls == AEECLSID_PHONENOTIFIER
                 && ( (GET_NOTIFIER_MASK(pN->dwMask) & (AEET_NMASK_VOICE_CALL || AEET_NMASK_NEW_CALLDESC)) != 0) )
#endif     // FEATURE_BT_EXTPF_AV
        {
          return CMediaPlayer_HandlePhoneEvent(pme, pN->pData);
        }
        else
        {
          return FALSE;
        }
      }


  case EVT_COMMAND:                   // Process menu command event
  case EVT_KEY:                       // Process key event
#ifdef FEATURE_KEY_HOLD_SCROLL
  case EVT_KEY_PRESS:                 // key down event
  case EVT_KEY_RELEASE:               // key release event
#endif
  case EVT_CREATEMEDIA:               // Create media
#ifdef MP_FEATURE_VIDEO
  case EVT_CREATEMEDIA_EX:            // Create Media with extended
                                      // parameters

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  case EVT_CREATEPSEUDOMEDIA:         // Create pseudo stream media
#endif
  case EVT_MEDIAPLAYER_SEEK:          // media seek control
#endif     // MP_FEATURE_VIDEO
  case EVT_CREATEMEDIA_QCP:           // Create IMediaQCP for
#ifdef FEATURE_QTV_FCS
#error code not present
#endif     // FEATURE_QTV_FCS
                                      // recording
  case EVT_COPYRIGHT_END:             // Copyright dialog ended
  case EVT_CTL_TAB:
    if (pme->m_pWin)
      return IWINDOW_HandleEvent(pme->m_pWin, eCode, wParam, dwParam);

  default:
    return FALSE;
  }
}

#ifdef MP_DETERMINE_VOL_LEVELS
// This function handles the volume level determination
static void CMediaPlayer_HandleVolumeLevels(const void* user_data, snd_status_type status, uint16 levels )
{
  CMediaPlayer* pme = (CMediaPlayer*) user_data;
  DBGPRINTF_MED("MP: HandleVolumeLevels called with status %d and levels of %d", status, levels);

  if (levels < MP_DEFAULT_NUM_VOL_LEVELS) 
  {
     levels = MP_DEFAULT_NUM_VOL_LEVELS;
  }

  if (SND_SUCCESS == status)
  {
    pme->m_wNumVolumeLevels = levels;
    pme->m_VolumeStep = AEE_MAX_VOLUME / pme->m_wNumVolumeLevels;
    DBGPRINTF_MED("MP: We have %d volume levels and our volume step is %d", pme->m_wNumVolumeLevels, pme->m_VolumeStep);
  }

  // Make sure that our volume level is never higher than the number of levels
  if (pme->m_wVolumeLevel > pme->m_wNumVolumeLevels)
  {
    pme->m_wVolumeLevel = pme->m_wNumVolumeLevels;
  }
}
#endif //MP_DETERMINE_VOL_LEVELS


// this function is the handler of EVT_APP_SUSPEND
void CMediaPlayer_ProcessSuspendEvent(CMediaPlayer *pme)
{
    int       ret;
#if defined(MP_FEATURE_VIDEO) || defined(FEATURE_APP_REAL)
    // release restore flag is defined, if this feature flag
    // is not defined, we need to delete media object for releasing DSP
    boolean   bReleaseRestoreDefined = FALSE;
    // indicates if PAUSE is issued by QTV
    // when bReleaseRestoreDefined defined, PAUSE will release DSP
    boolean   bDSPReleaseInitiated = FALSE;
#endif


    DBGPRINTF_HIGH("MP: start suspending.");
    if(NULL == pme)
    {
      DBGPRINTF_ERROR("MP: pme is NULL.");
      return;
    }

#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
    // When incall mixing is enabled, we may get suspend notification when we actually want to continue playing.
    if (uisnd_get_incall_mixing() && pme->m_bCallInProgress)
    {
      DBGPRINTF_MED("MP: Aborted suspend event processing due to incall mixing and call in progress.");
      return;
    }
#endif

    if(pme->m_bSuspend == TRUE)
    {
      // not sure if this is an error, warn anyway
      DBGPRINTF_ERROR("MP: received another EVT_APP_SUSPEND, current state: MP_STATE_START_SUSPEND.");
    }

    if(pme->m_bResumeInProcessing || pme->m_bResumePending)
    {
      // a resume request is in process, flag m_bSuspendPending to
      // indicate this SUSPEND
      pme->m_bSuspendPending = TRUE;
      DBGPRINTF_HIGH("MP: ResumeInProcessing = %d, ResumePending = %d.", pme->m_bResumeInProcessing, pme->m_bResumePending);
      DBGPRINTF_HIGH("MP: end suspending.");
      return;
    }

#ifdef FEATURE_KEY_HOLD_SCROLL
    /*
     * Cancel all timers when another application takes control.
     */
    pme->m_KeyStore.m_wParam_Key = 0;
    pme->m_KeyStore.m_dwParam_Key = 0;
    (void)ISHELL_CancelTimer(pme->a.m_pIShell, NULL, (void *)&pme->m_KeyStore);
#endif

    // Cancel all MediaPlayerTimers when another application takes control
    if(pme && pme->m_pWin)
    {
      (void)ISHELL_CancelTimer(pme->a.m_pIShell, NULL, pme->m_pWin);
    }

    pme->m_bSuspend = TRUE;
    // output current play state
    EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_playState);

    if(pme->m_pWin == NULL)
    {
      // something wrong
      pme->m_bNeedReload = FALSE;
      pme->m_bPlayAfterResume = FALSE;
      DBGPRINTF_HIGH("MP: m_pWin ptr is NULL, end suspending.");
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      pme->m_bReconnectWebOnResume = FALSE;
#endif
#endif
      DBGPRINTF_MED("MP: suspending record window.");
      return;
    }

    // save active win type
    pme->m_eSuspendWin = pme->m_eActiveWin;
    pme->m_wSuspendVolume = pme->m_wVolumeLevel;
    DBGPRINTF_HIGH("MP: suspend volumelevel = %d.", pme->m_wSuspendVolume);

    pme->m_bPlayNextFileOnResume = FALSE;
    pme->m_bReplaySameFile = FALSE;
#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
    if ((pme->m_eActiveWin == MPW_PLAYER) || (pme->m_eActiveWin == MPW_STREAM_RECORDER))
#else 
    if (pme->m_eActiveWin == MPW_PLAYER)
#endif // defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
    {
      int32     classId;
      boolean   isPlaying;
      boolean   isPaused;
#if defined(MP_FEATURE_VIDEO) || defined(FEATURE_APP_REAL)
      boolean   isQtvMedia = FALSE;
      boolean   isRealMedia = FALSE;
#endif //defined(MP_FEATURE_VIDEO) || defined(FEATURE_APP_REAL)
      char *pExtName = NULL;

      CPlayerWin *pSuspendWin = (CPlayerWin *)pme->m_pWin;
      if (!pSuspendWin) 
      {
        DBGPRINTF_ERROR("MP: pSuspendWin is NULL.");
        return;
      }

      /*
       * Handle the recorder window seperately.
       */
      if (pme->m_State == MP_STATE_RECORDER_WIN)
      {
        DBGPRINTF_HIGH("MP: suspend in recording win.");
        if (pSuspendWin->m_pMedia == NULL)
        {
          DBGPRINTF_HIGH("MP: m_pMedia is null, no stop issued.");
        }
        else
        {
          /* Stop recording. */
          /* User may have set volume low, set to max when suspending. */
          (void)IMEDIA_SetVolume(pSuspendWin->m_pMedia, AEE_MAX_VOLUME);

          ret = IMEDIA_Stop(pSuspendWin->m_pMedia);
          if (ret != SUCCESS)
            DBGPRINTF_ERROR("MP: failed on IMEDIA_Stop, %d.", ret);

          if(pSuspendWin->m_pMedia1)
          {
            ret = IMEDIA_Stop(pSuspendWin->m_pMedia1);
            if (ret != SUCCESS)
              DBGPRINTF_ERROR("MP: failed on IMEDIA_Stop for m_pMedia1, %d.", ret);
          }
        }

        pme->m_bSuspend = TRUE;
        pme->m_bNeedReload = FALSE;
        pme->m_bPlayAfterResume = FALSE;

        pme->m_State = MP_STATE_START_SUSPEND;
        DBGPRINTF_HIGH("MP: end suspending record window.");
        return;
      }

      // get media file extension
      if(pSuspendWin->m_pszFile)
      {
        // save current file for resuming
        (void)STRLCPY(pme->m_resumeFileName, pSuspendWin->m_pszFile, MP_MAX_FILE_NAME);
        pExtName = MP_GetFileNameExtension(pSuspendWin->m_pszFile);
      }

      if ((pSuspendWin->m_pMedia == NULL))
      {
        // set m_bResumePending flag to indicate that we need to resume later
        pme->m_bResumePending = TRUE;
        pme->m_resumePlayPos = -1;
        pme->m_eSuspendWin = pme->m_eActiveWin;
        pme->m_bNeedReload = FALSE;
        pme->m_bPlayAfterResume = FALSE;
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        pme->m_bReconnectWebOnResume = FALSE;
#endif
#endif

        if (pSuspendWin->m_pImage != NULL)
        {
          /*
           * Image -- don't close the window or else it will be lost.
           */
          pme->m_bNeedReload = FALSE;
          pme->m_bResumePending = FALSE;
          DBGPRINTF_HIGH("MP: suspending image window.");
        }
        else
        {
          // for Pseudo streaming, media is not created until we download the first chunk of data
          DBGPRINTF_HIGH("MP: suspending window with m_pMedia is NULL.");
          if (pExtName && !(STRICMP(pExtName, "url")))
          {
            // this may be in the connecting state of the HTTP streaming

#ifdef FEATURE_APP_MPEG4
  #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
            // cancel streaming timer
            (void)ISHELL_CancelTimer(pme->a.m_pIShell, PSTimerResponse, (void *)pme);
            // stop and clean up streaming
            if (pSuspendWin->psHttpBlock && pSuspendWin->psHttpBlock->bWebPstreaming)
            {
              PS_CleanHttpDownload(pSuspendWin->psHttpBlock);
            }

            FREE(pSuspendWin->psHttpBlock);
            pSuspendWin->psHttpBlock = NULL;
            PseudoPlayFile_CleanUp(pme);
            DBGPRINTF_HIGH("MP: stop and clean up streaming.");
  #else
            DBGPRINTF_HIGH("MP: no action on suspending URL file.");
  #endif
#else
            DBGPRINTF_HIGH("MP: no action on suspending URL file.");
#endif
            // reload on resuming
            pme->m_bNeedReload = TRUE;

          }
          else
          {
            // in continuous playback mode, EVT_APP_SUSPEND may arrive after media deleted
            // and before creating a new one, m_pMedia will be NULL in that case.
            if(pSuspendWin && pSuspendWin->m_pszFile)
            {
              DBGPRINTF_ERROR("MP: not a streaming file %s\n", pSuspendWin->m_pszFile);
            }
            else
            {
              DBGPRINTF_ERROR("MP: file name is NULL.");
            }
            // no reload on resuming
            pme->m_bNeedReload = FALSE;
          }
          pme->m_bPlayAfterResume = FALSE;
        }

        pme->m_State = MP_STATE_START_SUSPEND;

        if(pSuspendWin && pSuspendWin->m_pszFile)
        {
          DBGPRINTF_HIGH("MP: suspending with file %s\n", pSuspendWin->m_pszFile);
        }
        else
        {
          DBGPRINTF_HIGH("MP: suspending with file is NULL");
        }
        DBGPRINTF_HIGH("MP: end suspending.");
        return;
      }

      pme->m_bNeedReload = FALSE;
      pme->m_bPlayAfterResume = FALSE;

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      // Make sure that we at least have a url file before we try to do PS or PD suspend stuff
      if (pExtName && !(STRICMP(pExtName, "url"))) 
      {
        pme->m_bReconnectWebOnResume = FALSE;
        // Tell the HttpDownloader code that we are in the process of suspending
        // so ignore any call backs it may receive
        HttpDownloader_suspended = TRUE;

        // stop and clean up streaming
        // We want to keep the data that we have downloaded so far,
        // so just clean up the Web connection.  This way we know what
        // state the web is in when we resume.  If we don't it might be possible
        // for the web to timeout on us

        if (pSuspendWin->psHttpBlock && pSuspendWin->psHttpBlock->bWebPstreaming)
        {
          PS_Free_web(pSuspendWin->psHttpBlock);
          pme->m_bReconnectWebOnResume = TRUE;
        }
      }
#endif // FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif //FEATURE_APP_MPEG4
      isPlaying = CPlayerWin_IsPlaying(pSuspendWin);
      isPaused = CPlayerWin_IsPause(pSuspendWin);

      ret = IMEDIA_GetClassID(pSuspendWin->m_pMedia, (AEECLSID *)&classId);
      if(SUCCESS != ret && MM_PENDING != ret)
        DBGPRINTF_ERROR("MP: failed on IMEDIA_GetClassID, %d.", ret);

#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
      isQtvMedia = (classId == AEECLSID_MEDIAMPEG4);
#endif //FEATURE_APP_MEDIA_VIDEO

#ifdef FEATURE_APP_REAL
      isRealMedia = (classId == AEECLSID_MEDIAREAL);
#endif     // FEATURE_APP_REAL

#if defined(MP_FEATURE_VIDEO) || defined(FEATURE_APP_REAL)
      DBGPRINTF_HIGH("MP: isQtvMedia = %d, isRealMedia = %d, isPlaying = %d, isPaused = %d.",
                    isQtvMedia, isRealMedia, isPlaying, isPaused);
#else
      DBGPRINTF_HIGH("MP: isPlaying = %d, isPaused = %d.", isPlaying, isPaused);
#endif //defined(MP_FEATURE_VIDEO) || defined(FEATURE_APP_REAL)

      // save replaying status
      pme->m_bPlayAfterResume = isPlaying;

      if(isPlaying)
      {
        // WMA is handled by QTV with same class ID, therefore, we need to compare the extension
        char  *pExtName;
        pExtName = MP_GetFileNameExtension(pSuspendWin->m_pszFile);

        if((pme->m_bBackgroundMode) && !(STRICMP(pExtName, "wma")))

        {
          DBGPRINTF_HIGH("MP: Background mode for WMA format.");
#ifdef FEATURE_MP_EQ
          // disable spectrum analyzer
          pme->m_bResumeAnalyzer = pme->m_pSetting->m_bEnableAnalyzer;
          MPAudioEnableSpectrumAnalyzer(pme->m_pSetting, FALSE);
#endif // FEATURE_MP_EQ

          // this can be in any play state
          DBGPRINTF_HIGH("MP: background play mode enabled for WMA, playing = %d.", isPlaying);
          pme->m_bResumePending = TRUE;
          pme->m_bNeedReload = FALSE;
          pme->m_bPlayAfterResume = FALSE;
          pme->m_State = MP_STATE_START_SUSPEND;
          pme->m_bIncallMode = TRUE;
          pme->m_resumePlayPos = 0;

          DBGPRINTF_HIGH("MP: end suspending.");
          return;
        }
#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
        DBGPRINTF_HIGH("MP: Incall mixing feature IS defined.");
        if ( uisnd_get_incall_mixing() )
        {
          pme->m_bIncallMode = TRUE;
          DBGPRINTF_HIGH("MP: Incall mixing feature is in action.");
        }
#endif //#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
      }

#if defined(MP_FEATURE_VIDEO) || defined(FEATURE_APP_REAL)
      if (isQtvMedia || isRealMedia)
      {

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
      /*
       * IF OEMCM has initiated a DSP release and a PAUSE during an
       * incoming voice call,i.e when qtv_cfg_enable_dsp_release is
       * TRUE, we don't want to PAUSE again on receiving this app
       * suspend event
       */
#ifdef MP_USE_NEWER_BUILD_ITEMS        
        if (qtv_cfg_enable_dsp_release || qtv_cfg_enable_dsp_release_oem)
#else
        if (qtv_cfg_enable_dsp_release)
#endif /* MP_USE_NEWER_BUILD_ITEMS */
        {
          bDSPReleaseInitiated = TRUE;

#ifndef FEATURE_REAL_PLAYER_USE_QTV_API
          if(isRealMedia)
          {
            // if this is real media and the FEATURE_REAL_PLAYER_USE_QTV_API is not been defined
            // MP should handle the pause
            bDSPReleaseInitiated = FALSE;
          }
#endif //FEATURE_REAL_PLAYER_USE_QTV_API
        }

// print out qtv flag values
#ifdef MP_USE_NEWER_BUILD_ITEMS
       DBGPRINTF_HIGH("MP: QTV var status, qtv_cfg_enable_dsp_release = %d, qtv_cfg_enable_dsp_release_oem = %d\n",
                     qtv_cfg_enable_dsp_release, qtv_cfg_enable_dsp_release_oem);
#else
       DBGPRINTF_HIGH("MP: QTV var status, qtv_cfg_enable_dsp_release = %d\n", qtv_cfg_enable_dsp_release);
#endif //MP_USE_NEWER_BUILD_ITEMS

#endif     // FEATURE_QTV_QDSP_RELEASE_RESTORE

        /*
         * Tell QTV/Real to release the DSP upon receiving
         * the pause command.
         */
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE_2
        if (isQtvMedia)
        {
          bReleaseRestoreDefined = TRUE;
        }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE_2 */


#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
        if (isQtvMedia)
        {
          bReleaseRestoreDefined = TRUE;
          DBGPRINTF_HIGH("MP: enabling DSP release, set qtv_cfg_enable_dsp_release_oem\n");
        }
#endif     // FEATURE_QTV_QDSP_RELEASE_RESTORE


#ifdef FEATURE_REAL_QDSP_RELEASE_RESTORE
        if (isRealMedia)
        {
          real_cfg_enable_dsp_release = TRUE;
          bReleaseRestoreDefined = TRUE;
          DBGPRINTF_HIGH("MP: enabling DSP release, set real_cfg_enable_dsp_release\n");
        }
#endif     // FEATURE_REAL_QDSP_RELEASE_RESTORE

#ifdef FEATURE_APP_MPEG4
        if(pme->m_bQtvSuspendPause == FALSE)
        {
          bReleaseRestoreDefined = FALSE;
          DBGPRINTF_HIGH("MP: QTV Suspend Pause False, setting bReleaseRestoreDefine False\n");
        }
#endif // FEATURE_APP_MPEG4

        DBGPRINTF_HIGH("MP: bReleaseRestoreDefined = %d, bDSPReleaseInitiated = %d", bReleaseRestoreDefined, bDSPReleaseInitiated);

#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
        // When incall mixing is enabled, we want to do a full stop and resume instead of pause/resume for all audio files.
        // If not, we do pause/resume
        if ( bReleaseRestoreDefined  && ( !uisnd_get_incall_mixing()  ||
                (uisnd_get_incall_mixing() && !CMediaPlayer_IsAudioMedia(pme, pExtName)) ) 
        )
#else
        if (bReleaseRestoreDefined)
#endif    //#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
        {
          // when bReleaseRestoreDefined is true, we pause, don't care about the replay position

#if defined (FEATURE_QTV_QDSP_RELEASE_RESTORE) && defined FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_REAL_QDSP_RELEASE_RESTORE && FEATURE_QTV_QOS_SELECTION */

          // by now, we know feature release restore is defined in QTV
          if (!bDSPReleaseInitiated)
          {
            DBGPRINTF_HIGH("MP: DSP release not initiated from QTV (not Paused).\n");

            DBGPRINTF_HIGH("MP: pause replay.\n");
            // SetRect to null is a tmp hack to erase the secreen on an MDP
            // system (EraseRect is not supose to work)
            // Only do this for MBMS systems right now.  At some point it should
            // be for all MDP systems 
//#ifdef MP_FEATURE_MBMS
            DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with NULL_RECT");
            IMEDIA_SetRect(pSuspendWin->m_pMedia, &NULL_RECT, NULL);
//#endif // MP_FEATURE_MBMS
            // If we are not paused or we are buffering we need to issue a pause
            if (MP_PLAYSTATE_PAUSED != pme->m_playState)
            {
              boolean   isConnecting;
              boolean   isReady;

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
              if (isQtvMedia)
              {
#ifdef MP_USE_NEWER_BUILD_ITEMS
                qtv_cfg_enable_dsp_release_oem = TRUE;
#endif // MP_USE_NEWER_BUILD_ITEMS
              }
#endif // FEATURE_QTV_QDSP_RELEASE_RESTORE
              isConnecting = (pSuspendWin->m_pOwner->m_playState == MP_PLAYSTATE_CONNECTING);
              isReady = CPlayerWin_IsReady(pSuspendWin);

              // If in CONNECTING state, media cannot be paused, it must be stopped.
              if (isConnecting)
              {
                DBGPRINTF_MED("MP: in CONNECTING state, calling IMEDIA_Stop");
                ret = IMEDIA_Stop(pSuspendWin->m_pMedia);
                if (ret != SUCCESS)
                {
                  DBGPRINTF_ERROR("MP: IMEDIA_Stop failed, %d = %s.", ret, MP_ErrorStateToString(ret));
                }
              }
              else if (isPaused)
              {
                // Media is already paused.  Don't pause again.
                pme->m_playState = MP_PLAYSTATE_PAUSED;
                DBGPRINTF_MED("MP: Media already paused.");
              }
              else if (isReady)
              {
                // Media is in Ready state, can't pause.
                pme->m_playState = MP_PLAYSTATE_PAUSED;
                DBGPRINTF_MED("MP: Media in Ready state, can't pause.");
              }
              else 
              {
                ret = IMEDIA_Pause(pSuspendWin->m_pMedia);
                if (ret == SUCCESS)
                {
                  DBGPRINTF_HIGH("MP: IMEDIA_Pause requested.\n");
                  // clean error state flag
                  pme->m_ErrorState = MP_ERRORSTATE_NONE;
                  // sleep 100ms to give OEM layer some time
                  MP_WaitOnState(pSuspendWin, MP_PLAYSTATE_PAUSED, 100);
                }
                else
                {
                  DBGPRINTF_ERROR("MP: IMEDIA_Pause failed, %d = %s.", ret, MP_ErrorStateToString(ret));
                }
              }
            }

            /* User may have set volume low, set to max when suspending. */
            (void)IMEDIA_SetVolume(pSuspendWin->m_pMedia, AEE_MAX_VOLUME);

#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
            if (pme->m_bPlayRingerWithQTV)
            {
              if (pSuspendWin->m_pMedia1)
              {
                AEECLSID clsId;
                IMEDIA_GetClassID(pSuspendWin->m_pMedia1, &clsId);
                if (clsId == AEECLSID_MEDIAMPEG4)
                {
                  IMEDIA_Pause(pSuspendWin->m_pMedia1);
                }
              }
            }
#endif /* FEATURE_APP_MPEG4 && FEATURE_QTV_AUDIO_RINGER */
          }
          else
          {
            DBGPRINTF_HIGH("MP: DSP release is initiated from QTV(replay Paused).\n");
          }

          pme->m_bResumePending = TRUE;
          pme->m_bNeedReload = FALSE;

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
          // overwrite m_bPlayAfterResume if we can get it from QTV
          /* "playingBeforeVoiceCall" should be used only for QTV and not for REAL */
          if (isQtvMedia )
          {
            pme->m_bPlayAfterResume = (playingBeforeVoiceCall || isPlaying);
            DBGPRINTF_HIGH("MP: m_bPlayAfterResume = playingBeforeVoiceCall(%d) || isPlaying(%d)",
                          playingBeforeVoiceCall, isPlaying);
          }
          else
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
          {
            pme->m_bPlayAfterResume = isPlaying;
            DBGPRINTF_HIGH("MP: m_bPlayAfterResume = isPlaying(%d)", isPlaying);
          }

          DBGPRINTF_MED("MP: Trying to erase current screen which is x = %d, y = %d, dx = %d, dy = %d", pSuspendWin->m_rectImage.x, pSuspendWin->m_rectImage.y,
                        pSuspendWin->m_rectImage.dx, pSuspendWin->m_rectImage.dy);
          IDISPLAY_EraseRect(pSuspendWin->m_pIDisplay, &pSuspendWin->m_rectImage);
          /*
           * TBD: IMMOverlay
           *   Do we need to do IMMOverlay_Update(pSuspendWin->m_pIMMOverlayImage) ?
           */
          IDISPLAY_Update(pSuspendWin->m_pIDisplay);
        }

#ifdef MP_FEATURE_VIDEO
        else // bReleaseRestoreDefined is false
        {
          // bReleaseRestoreDefined is false, release restore is not supported.
          // we need to save current replay pos, and delete media

          // save filename for resuming
          (void)STRLCPY(pme->m_resumeFileName, pSuspendWin->m_pszFile, MP_MAX_FILE_NAME);
          if(pSuspendWin && pSuspendWin->m_pszFile)
          {
            DBGPRINTF_HIGH("MP: MediaPlayer suspending, resume filename is %s\n", pSuspendWin->m_pszFile);
          }
          else
          {
            DBGPRINTF_HIGH("MP: file name is NULL.");
          }
          //playback position
          pme->m_resumePlayPos = -1;

          if (isPlaying || isPaused)
          {
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
            {
              AEEMediaPlaybackPos playPos;
              ret = IMEDIA_GetMediaParm(pSuspendWin->m_pMedia, MM_MP4_PARAM_PLAYBACK_POS, ((int32 *)(&playPos)), 0);
              if(ret != SUCCESS)
                DBGPRINTF_ERROR("MP: failed on IMEDIA_GetMediaParm, %d.", ret);

              if (((pSuspendWin->m_mediaInfo.mpeg4Type == MM_MPEG4_VIDEO_ONLY) ||
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
                   (pSuspendWin->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_VIDEO_TEXT) ||
                   (pSuspendWin->m_mediaInfo.mpeg4Type == MM_MPEG4_VIDEO_TEXT) ||
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT
                   (pSuspendWin->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_VIDEO)) && ret == SUCCESS)
               {
                 pme->m_resumePlayPos = (int32)playPos.dwVideoPos;
                 DBGPRINTF_HIGH("MP: MediaPlayer suspending, video resume position is %d\n", pme->m_resumePlayPos);
               }
               else
               {
                 pme->m_resumePlayPos = (int32)playPos.dwAudioPos;
                 DBGPRINTF_HIGH("MP: MediaPlayer suspending, audio resume position is %d\n", pme->m_resumePlayPos);
               }
             }
#endif //FEATURE_APP_MEDIA_VIDEO
          }

          pme->m_eSuspendWin = pme->m_eActiveWin;
          if(isPlaying)
          {
            // make sure media is fully stopped
            ret = IMEDIA_Stop(pSuspendWin->m_pMedia);
            if(ret == SUCCESS)
            {
              DBGPRINTF_HIGH("MP: success on IMEDIA_Stop");
              // clean error state flag
              pme->m_ErrorState = MP_ERRORSTATE_NONE;
              // sleep 100ms to give OEM layer some time
              MP_WaitOnState(pSuspendWin, MP_PLAYSTATE_STOPPED, 100);
            }
            else
              DBGPRINTF_ERROR("MP: failed on IMEDIA_Stop, %d.", ret);
            pme->m_bPlayAfterResume = TRUE;
          }
          else
            pme->m_bPlayAfterResume = FALSE;

          /* User may have set volume low, set to max when suspending. */
          (void)IMEDIA_SetVolume(pSuspendWin->m_pMedia, AEE_MAX_VOLUME);

          // since we are deleing media, need to be reloaded
          pme->m_bResumePending = TRUE;
          pme->m_bNeedReload = TRUE;
          // delete media and CPlayerWin
          (void)CMediaPlayer_SetWindow(pme, MPW_NONE, 0);

        }     // if (!bReleaseRestoreDefined)
#endif     // MP_FEATURE_VIDEO
      }
      else
#endif     // if defined(MP_FEATURE_VIDEO) || defined(FEATURE_APP_REAL)
      {

        //must be an audio media. Get the play position
        pme->m_resumePlayPos = pSuspendWin->m_dwPlayPos * 1000;
        if(pSuspendWin && pSuspendWin->m_pszFile)
        {
          DBGPRINTF_HIGH("MP: Suspending audio file, filename = %s, pos = %d.", pSuspendWin->m_pszFile, pme->m_resumePlayPos);
        }
        else
        {
          DBGPRINTF_HIGH("MP: Suspending audio file, filename = NULL, pos = %d.", pme->m_resumePlayPos);
        }

        DBGPRINTF_HIGH("MP: playing mode, isPlaying = %d", isPlaying);
        if (isPlaying)
        {

          if(pme->m_bBackgroundMode && CMediaPlayer_IsAudioClassID( classId) )
          {
#ifdef FEATURE_MP_EQ
            // disable spectrum analyzer
            pme->m_bResumeAnalyzer = pme->m_pSetting->m_bEnableAnalyzer;
            MPAudioEnableSpectrumAnalyzer(pme->m_pSetting, FALSE);
#endif // FEATURE_MP_EQ

            // this can be in any play state
            DBGPRINTF_HIGH("MP: background play mode enabled, replaying = %d.", isPlaying);
            pme->m_bResumePending = TRUE;
            pme->m_bNeedReload = FALSE;
            pme->m_bPlayAfterResume = FALSE;
            pme->m_State = MP_STATE_START_SUSPEND;
            pme->m_bIncallMode = TRUE;
            pme->m_resumePlayPos = 0;

            DBGPRINTF_HIGH("MP: end suspending for background playback.");
            return;
          }
          else
          {
#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
            DBGPRINTF_HIGH("MP: Replay will be stopped. Incall mode = %s, background = %s, classId = 0x%X", 
                           uisnd_get_incall_mixing()? "On" : "Off", pme->m_bBackgroundMode? "TRUE" : "FALSE", classId);
#else
            DBGPRINTF_HIGH("MP: Replay will be stopped. background = %s, classId = 0x%X", 
                           pme->m_bBackgroundMode? "TRUE" : "FALSE", classId);
#endif
            DBGPRINTF_HIGH("MP: classType MEDIAMP3 = %d, MEDIAAAC = %d, MEDIAQCP = %d, MEDIAMIDI = %d.",
                           classId == AEECLSID_MEDIAMP3,
                           classId == AEECLSID_MEDIAAAC,
                           classId == AEECLSID_MEDIAQCP,
                           classId == AEECLSID_MEDIAMIDI);
          }

          // stop replay
          DBGPRINTF_HIGH("MP: stop replay.");
          ret = IMEDIA_Stop(pSuspendWin->m_pMedia);
          if(ret == SUCCESS)
          {
            DBGPRINTF_HIGH("MP: success on IMEDIA_Stop");
            // clean error state flag
            pme->m_ErrorState = MP_ERRORSTATE_NONE;
            // sleep 100ms to give OEM layer some time
            MP_WaitOnState(pSuspendWin, MP_PLAYSTATE_STOPPED, 100);
          }
          else
          {
            DBGPRINTF_ERROR("MP: failed on IMEDIA_Stop, %d.", ret);
          }
          pme->m_bPlayAfterResume = TRUE;
        }
        else
        {
          pme->m_bPlayAfterResume = FALSE;
        }
        pme->m_bResumePending = TRUE;
        pme->m_bNeedReload = TRUE;

        // reset player win
        (void)CMediaPlayer_SetWindow(pme, MPW_NONE, 0);
        DBGPRINTF_HIGH("MP: end suspending.");
      }
      pme->m_State = MP_STATE_START_SUSPEND;

      DBGPRINTF_HIGH("MP: end suspending.");
      return;
    } //if (pme->m_eActiveWin == MPW_PLAYER)
#ifdef FEATURE_MULTISEQUENCER
    else if (pme->m_eActiveWin == MPW_SEQUENCE_PLAYER)
    {
      CSequencePlayerWin *pSuspendWin = (CSequencePlayerWin *)pme->m_pWin;

      DBGPRINTF_HIGH("MP: suspending multisequencer window.");
      // this is multi sequencer window
      if(CSequencePlayerWin_IsPlaying(pSuspendWin, TRUE))
      {
        CSequencePlayerWin_Stop(pSuspendWin, TRUE);
      }
      (void)CMediaPlayer_SetWindow(pme, MPW_NONE, 0);
      pme->m_eSuspendWin = MPW_MULTISEQUENCER;
    }
#endif
    else
    {
      // unknown player type
      pme->m_eSuspendWin = pme->m_eActiveWin;
    }

    DBGPRINTF_HIGH("MP: end suspending.");
}

void CMediaPlayer_ProcessResumeEvent(CMediaPlayer *pme)
{
    /*
     * save m_bResumePending before it gets cleared by CPlayerWin_New()
     */
    boolean   resumePending = pme->m_bResumePending;

    if(pme->m_bSuspend == FALSE)
    {
      // not sure if this is an error, warn anyway
      DBGPRINTF_ERROR("MP: received EVT_APP_RESUME, not in suspending mode, state %d.", pme->m_State);

      if (pme->m_eSuspendWin == MPW_NONE)
      {
        DBGPRINTF_ERROR("MP: Resume suspend event received , not in suspending mode with invalid player window state. Aborting...");
        return;
      }
    }
    pme->m_bSuspend = FALSE;
    DBGPRINTF_HIGH("MP: start resuming.");
    pme->m_State = MP_STATE_START_RESUME;
    EMIT_MEDIAPLAYER_STATE(pme->m_State);

#ifdef FEATURE_MP_EQ
    // If we were in incall mode and restore state is different from current state
    if((pme->m_bIncallMode == TRUE) &&
       (pme->m_bResumeAnalyzer != pme->m_pSetting->m_bEnableAnalyzer))
    {
      MPAudioEnableSpectrumAnalyzer(pme->m_pSetting, pme->m_bResumeAnalyzer);
    }
#endif // FEATURE_MP_EQ

    // No longer suspended, reset incall mode
    pme->m_bIncallMode = FALSE;
#if defined(FEATURE_QTV_QOS_SELECTION) && defined(FEATURE_QTV_QDSP_RELEASE_RESTORE)
#error code not present
#endif     // (FEATURE_QTV_QOS_SELECTION) &&
           // defined(FEATURE_QTV_QDSP_RELEASE_RESTORE)

    DBGPRINTF_HIGH("MP: restoring suspended window = %d.", pme->m_eSuspendWin);
    (void)CMediaPlayer_SetWindow(pme, pme->m_eSuspendWin, 0);

    // restore volume setting
    pme->m_wVolumeLevel = pme->m_wSuspendVolume;
    DBGPRINTF_HIGH("MP: resume volume level = %d.", pme->m_wVolumeLevel);

    DBGPRINTF_HIGH("MP: ResumePending = %d, NeedReload = %d, PlayAfterResume = %d, PlayNextFileOnResume = %d\n",
              resumePending, pme->m_bNeedReload, pme->m_bPlayAfterResume, pme->m_bPlayNextFileOnResume);

    if ((pme->m_eSuspendWin == MPW_PLAYER
#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
        || pme->m_eActiveWin == MPW_STREAM_RECORDER
#endif // defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
        ) && (pme->m_State != MP_STATE_RECORDER_WIN))
    {
      /*
       * Restore to state before suspension
       */
      CPlayerWin* pRestoreWin = (CPlayerWin *)pme->m_pWin;
      pme->m_State = MP_STATE_PLAYER_WIN;
      if (NULL != pRestoreWin)
      {
        pRestoreWin->m_bResumeInProcessing = resumePending;
      }
      else
      {
        // We don't have an old win, so just return;
        DBGPRINTF_HIGH("MP: We don't have a player win to resume from. Don't try to restart media");
        pme->m_bPlayNextFileOnResume = FALSE;
        pme->m_bReplaySameFile = FALSE;
        pme->m_bResumePending = FALSE;
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        HttpDownloader_resuming = FALSE;
        HttpDownloader_suspended = FALSE;
        pme->m_bReconnectWebOnResume = FALSE;
#endif //FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif // FEATURE_APP_MPEG4
        DBGPRINTF_HIGH("MP: end resuming.");
        return;
      }
      if (resumePending)
      {
        if(pme->m_bPlayNextFileOnResume)
        {
          DBGPRINTF_HIGH("MP: received STATUS_DONE/ABORT after suspending, ReplaySameFile = %d", pme->m_bReplaySameFile);
          if(pme->m_bReplaySameFile)
          {
            (void)CPlayerWin_PlayNext((CPlayerWin *)pme->m_pWin, CONT_MODE_SINGLE_FILE, TRUE);
          }
          else
          {
            (void)CPlayerWin_PlayNext((CPlayerWin *)pme->m_pWin, pme->m_nContMode, TRUE);
          }
          // do not post PLAY event again
          pme->m_bPlayAfterResume = FALSE;
        }

        if (pme->m_bNeedReload && (pme->m_resumeFileName[0] != '\0'))
        {
          /*
           * resume playback
           */
          DBGPRINTF_HIGH("MP: resuming, resume filename is %s\n", pme->m_resumeFileName);

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
          if (pme->m_bReconnectWebOnResume)
          {
            HttpDownloader_resuming = TRUE;
            HttpDownloader_suspended = FALSE;
          }
#endif //#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))

          // set this flag to disable auto play
          // If a new media is created, it will look for the m_bAutoPlay flag, we want to disable the autoplay.
          pme->m_bDisableAutoPlayOnce = TRUE;

#if defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER)
          if(pme->m_bIContentMode)
          {
            CPlayerWin *pResumeWin = (CPlayerWin *)pme->m_pWin;
            int Err;

            if(pResumeWin && pResumeWin->m_pMedia == NULL)
            {
              if(SUCCESS == (Err = MPContentSelectFile(pme->m_pContent, pme->m_resumeFileName, 0, FALSE)))
              {
                // seek and resume when IContent callback returns
                pme->m_bIContentResumePlay = pme->m_bPlayAfterResume;
                pme->m_IContentResumePos = pme->m_resumePlayPos;
                pme->m_bPlayAfterResume = FALSE;
                pme->m_resumePlayPos = 0;
              }
              else
              {
                DBGPRINTF_ERROR("MP: unable to resume replay, MPContentSelectFile() failed with Err = %d.", Err);
              }
            }
          }
          else
#endif
          {
            DBGPRINTF_MED("MP: resuming, play file %s", pme->m_resumeFileName);
            (void)CMediaPlayer_PlayFile(pme, pme->m_resumeFileName);
          }
          pme->m_resumeFileName[0] = '\0';

#ifdef EVT_MEDIAPLAYER_SEEK
          /*
           * resume playback position
           */
          if (pme->m_resumePlayPos > 0)
          {
            DBGPRINTF_HIGH("MP: MediaPlayer resuming, resume position is %d\n", pme->m_resumePlayPos);
            (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_MEDIAPLAYER_SEEK, 0, pme->m_resumePlayPos);
          }
#endif     // EVT_MEDIAPLAYER_SEEK
        }

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        else if (pme->m_bReconnectWebOnResume)
        {
          CPlayerWin *pResumeWin = (CPlayerWin *)pme->m_pWin;

          HttpDownloader_resuming = TRUE;
          HttpDownloader_suspended = FALSE;

          if (pResumeWin->psHttpBlock != NULL)
          {
            if (http_ps_SetUpWeb(pResumeWin->psHttpBlock))
            {
              http_ps_get_chunk(&pResumeWin->psHttpBlock);
            }
            else
            {
              DBGPRINTF_HIGH("MP: http_ps_SetUpWeb() returns FALSE");
            }
          }
          else
          {
            DBGPRINTF_HIGH("MP: psHttpBlock == NULL");
          }
        }
#endif //FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif // FEATURE_APP_MPEG4

        /*
         * if state before interruption is play, resume play
         */
        if (pme->m_bPlayAfterResume)
        {

          //CPlayerWin *pResumeWin = (CPlayerWin *)pme->m_pWin;

          DBGPRINTF_HIGH("MP: MediaPlayer resuming, starting to play\n");
          pme->m_bResumeInProcessing = TRUE;

          DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
          (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);

#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
            if (pme->m_bPlayRingerWithQTV)
            {
              if (pRestoreWin->m_pMedia1)
              {
                AEECLSID clsId;
                IMEDIA_GetClassID(pRestoreWin->m_pMedia1, &clsId);
                if (clsId == AEECLSID_MEDIAMPEG4)
                {
                  int ret;
                  ret = IMEDIA_Resume(pRestoreWin->m_pMedia1);
                  DBGPRINTF_MED("MP: IMEDIA_Resume returns %d = %s", ret, MP_ErrorStateToString(ret));
                }
              }
            }
#endif /* FEATURE_APP_MPEG4 && FEATURE_QTV_AUDIO_RINGER */
        }
      }
     
      // Ensure that we restore the image rect.
//#ifdef MP_FEATURE_MBMS
      if(pRestoreWin->m_pMedia)
      {
        // Restore the Rect that was set to 0x0 to erase the screen while suspending
        IMEDIA_SetRect(pRestoreWin->m_pMedia, &(pRestoreWin->m_rectImage), NULL);
      }// if (pResumeWin->m_pMedia)
//#endif // MP_FEATURE_MBMS

      // update progress bar to show the current time we are paused at
      CPlayerWin_UpdateProgCtl(pRestoreWin, MM_CMD_PLAY, 0, IDS_PLAY);

      // If the media was suspended in the pause state
      // We need to set the state back to Paused.

      DBGPRINTF_HIGH("MP: MediaPlayer resuming, \n");
      if (CPlayerWin_IsPause(pRestoreWin) && !pme->m_bPlayAfterResume)
      {
        pRestoreWin->m_bResumeInProcessing = FALSE;
        pme->m_playState = MP_PLAYSTATE_PAUSED;
        EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_playState);        
      }
    } // if (pme->m_eSuspendWin == MPW_PLAYER)
 
    pme->m_bPlayNextFileOnResume = FALSE;
    pme->m_bReplaySameFile = FALSE;
    pme->m_bResumePending = FALSE;
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    HttpDownloader_resuming = FALSE;
    HttpDownloader_suspended = FALSE;
    pme->m_bReconnectWebOnResume = FALSE;
#endif //FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif // FEATURE_APP_MPEG4

#ifdef FEATURE_MP_EQ_UI
   if((pme->m_pWin != NULL) &&
      (((CPlayerWin *)pme->m_pWin)->m_eWin == MPPW_PLAY) &&
      (pme->m_bShowPlayerWinSettingUI != FALSE))
   {
     CMediaPlayer_AudioSetupScreen(pme);
     CPlayerWin_Redraw((IWindow *)pme->m_pWin);
   }
#endif //FEATURE_MP_EQ_UI
    DBGPRINTF_HIGH("MP: end resuming.");
}

/*===========================================================================
   This function draws the splash screen and brings up the main window
   after the splash timer runs out.
===========================================================================*/
static void
CMediaPlayer_DrawSplash(CMediaPlayer *pme)
{
  /*
   * The following 'If statement' is entered only after the splash timer
   * runs out...
   */
  if (pme->m_eActiveWin == MPW_MAIN)
  {
    (void)CMediaPlayer_SetWindow(pme, MPW_MAIN, 0);
    return;
  }

  /*
   * Draw the splash screen, set the timer.
   * The timer callback calls this function and redraws the main window.
   */
  {
    IImage   *pi =
      ISHELL_LoadResImage(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDB_LOGO);

    if (pi)
    {
      AEERect   rect;

      IDISPLAY_ClearScreen(pme->a.m_pIDisplay);
      SETAEERECT(&rect, 0, 0, pme->m_cxWidth, pme->m_cyHeight);
      MP_DrawImage(pi, &rect, TRUE);
      IDISPLAY_Update(pme->a.m_pIDisplay);
      MP_RELEASEIF(pi);
    }

    /*
     * Set main window as active and start the timer.
     */
    DBGPRINTF_MED("MP: Setting m_eActiveWin to MPW_MAIN");
    pme->m_eActiveWin = MPW_MAIN;
    (void)ISHELL_SetTimer(pme->a.m_pIShell, MP_SPLASH_TIMER,
                          (PFNNOTIFY)CMediaPlayer_DrawSplash, pme);
  }
}

/*===========================================================================
   This function switches from one window to another:
   (1)Releases all the resources associated with the current window
   (2)Contructs the new window, if any
   (3)Enables and redraws the new window, if any
===========================================================================*/
boolean CMediaPlayer_SetWindow(CMediaPlayer *pme, MPWindow eWin, uint32 dwParam)
{
  /*
   * If same window, then redraw and return.
   */
  if (pme->m_pWin && pme->m_eActiveWin == eWin && eWin != MPW_NONE)
  {
    CMediaPlayer_Redraw(pme, TRUE);
    return TRUE;
  }
  
  DBGPRINTF_MED("MP: Calling MP_RELEASEWIN");
  MP_RELEASEWIN(pme->m_pWin);

  DBGPRINTF_HIGH("MP: Create Win with type = %d.", eWin);
  switch (eWin)
  {
  case MPW_MAIN:
    pme->m_pWin = CMainWin_New(pme);
    break;

  case MPW_FILELIST:
#ifdef FEATURE_MULTISEQUENCER
    pme->m_bIsMultiSequencer = FALSE;
#endif
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
    if (dwParam == (uint32)MPPW_STREAM_RECORD)
      pme->m_bIsStreamRecord = TRUE;
    else
      pme->m_bIsStreamRecord = FALSE;
#endif
    pme->m_pWin = CFileListWin_New(pme);
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    /* invalidate buffer on any file selection */
    HttpDownloader_entireClipInBuffer = FALSE;
#endif
    break;

  case MPW_PLAYER:
    pme->m_pWin = CPlayerWin_New(pme, (MPPlayerWin)dwParam);
		break;

#ifdef FEATURE_MP4_LCD_CONTRAST
  case MPW_CONTRAST:
    pme->m_pWin = CContrastWin_New(pme);
    break;
#endif     // FEATURE_MP4_LCD_CONTRAST

#ifdef FEATURE_MULTISEQUENCER
  case MPW_MULTISEQUENCER:
    pme->m_bIsMultiSequencer = TRUE;
    pme->m_pWin = CFileListWin_New(pme);
    break;

  case MPW_SEQUENCE_PLAYER:
    pme->m_pWin = CSequencePlayerWin_New(pme);
    break;
#endif     // FEATURE_MULTISEQUENCER

#ifdef FEATURE_MP_RECORDER_SETTINGS
  case MPW_RECORDER_SETTINGS:
    pme->m_pWin = CRecorderSettingWin_New(pme);
    break;
#endif

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  case MPW_STREAM_RECORDER:
    pme->m_bIsStreamRecord = TRUE;
    pme->m_pWin = CPlayerWin_New(pme, (MPPlayerWin)dwParam);
    break;
#endif

  case MPW_NONE:
    return TRUE;

  default:
    return FALSE;
  }

  if (!pme->m_pWin)
  {
    DBGPRINTF_ERROR("MP: MediaPlayer: Error on creating new window: %d, %d", eWin, dwParam);
    eWin = MPW_NONE;
    return FALSE;
  }

  DBGPRINTF_MED("MP: Setting m_eActiveWin to %d", eWin);
  pme->m_eActiveWin = eWin;

  CMediaPlayer_Redraw(pme, TRUE);

  return TRUE;
}

/*===========================================================================
   This function schedules an aynchronous redraw if bDefer is TRUE else
   it redras immediately.
===========================================================================*/
static void
CMediaPlayer_Redraw(CMediaPlayer *pme, boolean bDefer)
{
  if (pme->m_bRedraw)
    return;

  pme->m_bRedraw = TRUE;

  if (bDefer)
    ISHELL_Resume(pme->a.m_pIShell, &pme->m_cbRedraw);
  else
    CMediaPlayer_RedrawNotify(pme);
}

/*===========================================================================
   This function redraws the current window.
   This function is called either by IShell in response to the resume scheduled
   in CMediaPlayer_Redraw()or by CMediaPlayer_Redraw()directly.
===========================================================================*/
static void
CMediaPlayer_RedrawNotify(CMediaPlayer *pme)
{
  if (!pme->m_bRedraw)
    return;

  DBGPRINTF_MED("MP: RedrawNotify, Start");

  pme->m_bRedraw = FALSE;

  if (pme->m_pWin)
  {
    IWINDOW_Enable(pme->m_pWin);
    IWINDOW_Redraw(pme->m_pWin);
    DBGPRINTF_MED("MP: RedrawNotify, Done");
  }
  else
  {
    DBGPRINTF_MED("MP: RedrawNotify, No window to redraw");
  }
}

void CMediaPlayer_OnSelectFile(CMediaPlayer *pme, const char* szPath)
{
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  if (pme->m_bIsStreamRecord)
    (void)CMediaPlayer_CreateStreamRecordWindow(pme, szPath);
  else
#endif
#ifdef FEATURE_MULTISEQUENCER
    if (!pme->m_bIsMultiSequencer)
#endif
    /*
      * Play the file...
      */
    {
      pme->m_bIsHTTP = FALSE;
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      if (FALSE == pme->m_bEnablePSSimulation && FALSE == pme->m_bFileWriteReadTest)
        (void)CMediaPlayer_PlayFile(pme, szPath);
      else
        (void)CMediaPlayer_PseudoPlayFile(pme, szPath);
#else
      CMediaPlayer_PlayFile(pme, szPath);
#endif
    }
}
/*===========================================================================
   This function opens the player window with the specified file.
   It also tries to play the file if MediaPlayer is in plugin mode.

   Note: This function allocates memory for the file name and the memory
   is released by player window's destructor.
===========================================================================*/
static boolean CMediaPlayer_PlayFile(CMediaPlayer *pme, const char *pszFile)
{
  char     *pszBuf;
  char     *fiName,
           *extName;
  uint16    fileType;
  boolean   bIsStreamingMedia = FALSE;

  char      tmpPath[MP_MAX_FILE_NAME];
  int pathLen = 0;
  int nRet;

  extern AEE_EXPORTS int AEE_ResolvePath(const char *cpszIn, char *pszOut, int *pnOutLen);

#if defined(MP_FEATURE_VIDEO)
  char      VideoFile[MP_MAX_FILE_NAME];
  char      AudioFile[MP_MAX_FILE_NAME];
  boolean   bLoopTrack = FALSE;
#endif

  if (SUCCESS != AEE_ResolvePath(pszFile, NULL, &pathLen))
  {
	DBGPRINTF_MED("MP: AEE_ResolvePath failed.");
  }

  DBGPRINTF_MED("MP: AEE_ResolvePath pathLen = %d.", pathLen);

  MEMSET(tmpPath, 0, MP_MAX_FILE_NAME);
  if (SUCCESS != AEE_ResolvePath(pszFile, tmpPath, &pathLen))
  {
    DBGPRINTF_MED("MP: AEE_ResolvePath failed.");
  }
  else
  {
    DBGPRINTF_MED("MP: AEE_ResolvePath failed = %s.", tmpPath);
  }

#if MIN_BREW_VERSION(4,0)
   nRet = OEMefs_GetNativePath(tmpPath, NULL, NULL, &pathLen);
#else
   nRet = OEMFS_GetNativePath(tmpPath, NULL, &pathLen);
#endif //MIN_BREW_VERSION(4,0)

#if MIN_BREW_VERSION(4,0)
   nRet = OEMefs_GetNativePath(tmpPath, pme->m_szFullPath, pathLen, 0);
#else
    nRet = OEMFS_GetNativePath(tmpPath, pme->m_szFullPath, &pathLen);
#endif //MIN_BREW_VERSION(4,0)

	if(SUCCESS != nRet)
	{
	  DBGPRINTF_MED("MP: OEMFS_GetNativePath failed.");
	}

#if defined(MP_FEATURE_VIDEO) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
  pme->m_bIsURLFile = FALSE;
#endif
  pme->m_bIsHTTP = FALSE;
#ifdef FEATURE_QTV_FCS
#error code not present
#endif

  /*
   * File name is a special case...
   * pszFile will be released by CFileListWin when CPlayerWin opens.
   * Allocate memory for file name here. It will be used and released
   * by CPlayerWin.
   */
  pszBuf = STRDUP(pszFile);
  if (!pszBuf)
  {
    DBGPRINTF_ERROR("MP: Failed to allocate memory for filename");
    return FALSE;
  }

  DBGPRINTF_MED("MP: PlayFile: %s", pszFile);
  /*
   * If launched as a plug-in then another application may specify
   * the filepath directory, so don't search the default filelist.
   */
  if (!pme->m_bPlugin && !CMediaPlayer_FindFileInFileList(pme, pszBuf))
  {
    DBGPRINTF_MED("MP: CMediaPlayer_FindFileInFileList(%s) returned false", pszBuf);
    FREE(pszBuf);
    return FALSE;
  }

  /*
   * Get the filename.
   */
  fiName = MP_GetFileName(pszBuf);
  /*
   * Check if fiName is null
   */
  if (fiName == NULL)
  {
    FREE(pszBuf);
    return FALSE;
  }

  /*
   * Get the filename extension.
   */
  extName = MP_GetFileNameExtension(fiName);
  /*
   * Check if extName is null
   */
  if (extName == NULL)
  {
    FREE(pszBuf);
    return FALSE;
  }
  DBGPRINTF_MED("MP: MediaPlayer finds %s as filename extension\n", extName);

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  pme->m_bPacketVideoStreaming = FALSE;
  if (!(STRICMP(extName, "pvx")))
  {
    bIsStreamingMedia = TRUE;
    pme->m_bPacketVideoStreaming = TRUE;
    DBGPRINTF_MED("MP: Packet Video Streaming true\n");
  }
  else
#endif
  if (!(STRICMP(extName, "url")))
  {
    IFileMgr       *pIFileMgr;
    IFile          *pIFile = NULL;
    AEEFileInfoEx  fi;
    unsigned int   size = 0;
    char           pUrl[PS_MAX_URL_LEN];

    bIsStreamingMedia = TRUE;
    (void)MEMSET(pUrl, 0, PS_MAX_URL_LEN);

    DBGPRINTF_MED("MP: MediaPlayer will try to play a URL file\n");
    (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR,
                                (void **)&pIFileMgr);

    if (NULL == pIFileMgr)
    {
      FREE(pszBuf);
      return FALSE;
    }

    if ((pIFile = IFILEMGR_OpenFile(pIFileMgr, pszBuf, _OFM_READ)) == NULL)
    {
      FREE(pszBuf);
      MP_RELEASEIF(pIFileMgr);
      return FALSE;
    }

    (void)MEMSET(&fi,0,sizeof(fi));
    fi.nStructSize = sizeof(fi);
    if (IFILEMGR_GetInfoEx(pIFileMgr, pszBuf, &fi) == SUCCESS)
    {
      int       rSize = 0;

      size = fi.dwSize;
      if (size <= PS_MAX_URL_LEN)
      {
        rSize = IFILE_Read(pIFile, pUrl, size);
        pUrl[rSize] = '\0';
        DBGPRINTF_MED("MP: MediaPlayer gets URL=%s\n", pUrl);
      }

      (void)IFILE_Release(pIFile);
      pIFile = NULL;
      MP_RELEASEIF(pIFileMgr);

#ifdef FEATURE_QTV_FCS
#error code not present
#endif
      /*
       * case insensitive search for string "http"
       */
      if (!(STRNICMP(pUrl, "http://", 7)))
      {                                // http was found
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
        boolean   retval = TRUE;
        DBGPRINTF_MED("MP: MediaPlayer found http prefix in %s\n", pUrl);
        pme->m_bIsHTTP = TRUE;

#ifdef FEATURE_MP_EQ_UI
        pme->m_bShowPlayerWinSettingUI = TRUE;
        pme->m_bShowLogo = FALSE;
#endif //FEATURE_MP_EQ_UI

        retval = CMediaPlayer_PseudoPlayFile(pme, pszFile);

        FREE(pszBuf);
        return retval;
#endif     // FEATURE_QTV_PSEUDO_STREAM ||
           // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      }

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
      else if (!(STRNICMP(pUrl, "rtsp://", 7)))
      {
        DBGPRINTF_MED("MP: MediaPlayer found rtsp prefix in %s\n", pUrl);
        if (!MP_GetRecordFileName_Streaming(recFileName, pUrl, defaultRecDir))
        {
          DBGPRINTF_ERROR("MP: Failed to get StreamRecord filename");
          FREE(pszBuf);
          return FALSE;
        }
      }
#endif     // defined(MP_FEATURE_VIDEO) &&
           // defined(FEATURE_QTV_STREAM_RECORD)
    }
    else
    {
      (void)IFILE_Release(pIFile);
      pIFile = NULL;
      MP_RELEASEIF(pIFileMgr);
    }
  }

  fileType = CMediaPlayer_FindHandlerType(pme, pszFile);
  DBGPRINTF_MED("MP: MediaPlayer finds %d as fileType", fileType);
  if (IDB_IMAGE_ICON == fileType)
  {
    /*
     * return TRUE, if the file can be shown as a image
     */
    DBGPRINTF_MED("MP: MediaPlayer is about to play an image");
    if (CMediaPlayer_PlayImage(pme, pszBuf))
    {
      return TRUE;
    }
    else
    {
      FREE(pszBuf);
      return FALSE;
    }
  }

#ifdef FEATURE_MP_EQ_UI
  pme->m_bShowPlayerWinSettingUI = TRUE;
  pme->m_bShowLogo = FALSE;
#endif //FEATURE_MP_EQ_UI

  /*
   * This is not image...it must be media.
   */
  if (!CMediaPlayer_SetWindow(pme, MPW_PLAYER, MPPW_PLAY))
  {
    DBGPRINTF_ERROR("MP: MediaPlayer is Unable to SetWindow");
    FREE(pszBuf);
    return FALSE;
  }

#ifdef MP_FEATURE_VIDEO
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
  CPlayerWin_ResetFrameTransformations((CPlayerWin *)pme->m_pWin);
#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS ||
           // FEATURE_QTV_MDP_TRANSFORMATIONS
#endif     // MP_FEATURE_VIDEO

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  if (!(STRICMP(extName, "sdp")))
  {
    if (!MP_GetRecordFileName_SDP(recFileName, fiName, defaultRecDir))
    {
      FREE(pszBuf);
      return FALSE;
    }
  }
#endif

#if defined(MP_FEATURE_VIDEO)
  if (!STRICMP(extName, "MFPT") &&
      MP_GetAVFileNames(pme->a.m_pIShell, pszFile, VideoFile, AudioFile,
                        MP_MAX_FILE_NAME, &bLoopTrack))
  {
    ((CPlayerWin *)pme->m_pWin)->m_bLoopTrack = bLoopTrack;
    if (CPlayerWin_SetMediaDataEx
        ((CPlayerWin *)pme->m_pWin, VideoFile, AudioFile))
    {
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_CREATEMEDIA_EX, 0, 0);
      if (pme->m_bPlugin)
      {
        DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
        (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
      }
    }
    FREE(pszBuf);
  }
  else
#endif
  {
    AEEMediaData md;

    md.clsData = 0;
    md.dwSize = 0;
    md.pData = NULL;

    //Read the file into a buffer and then start playing
    if (pme->m_bPlayFromBuffer && !bIsStreamingMedia)
    {
      IFileMgr       *pIFileMgr;
      IFile          *pIFile = NULL;
      AEEFileInfoEx  fi;
      unsigned int   size = 0;
      char           *playBuf = NULL;

      DBGPRINTF_MED("MP: MediaPlayer is about to play a file from a buffer");

      // Free the memory we previously allocated for media data.
      // This is only needed if it is MMD_BUFFER
      if (((CPlayerWin *)pme->m_pWin)->m_md.clsData == MMD_BUFFER)
      {
        FREEIF(((CPlayerWin *)pme->m_pWin)->m_md.pData);
      }

      (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);

      if (NULL == pIFileMgr)
      {
        DBGPRINTF_ERROR("MP: MediaPlayer was unable to create AEECLSID_FILEMGR");
        FREE(pszBuf);
        return FALSE;
      }

      if ((pIFile = IFILEMGR_OpenFile(pIFileMgr, pszBuf, _OFM_READ)) == NULL)
      {
        DBGPRINTF_ERROR("MP: MediaPlayer was unable to open file %s", pszBuf);
        FREE(pszBuf);
        MP_RELEASEIF(pIFileMgr);
        return FALSE;
      }

      (void)MEMSET(&fi,0,sizeof(fi));
      fi.nStructSize = sizeof(fi);
      if (IFILEMGR_GetInfoEx(pIFileMgr, pszBuf, &fi) == SUCCESS)
      {
        size = fi.dwSize;
        playBuf = MALLOC(size);
        if (playBuf != NULL)
        {
          DBGPRINTF_MED("MP: Reading file into a buffer");
          if (IFILE_Read(pIFile, playBuf, size))
          {
            md.clsData = MMD_BUFFER;
            md.pData = (void *)playBuf;
            md.dwSize = size;
          }
        }
        else
        {
          MP_ErrorDlg(pme, IDS_NOT_ENOUGH_MEMORY);
          DBGPRINTF_ERROR("MP: MediaPlayer was unable to malloc memory for file");
        }
      }

      // Free File resources 
      (void)IFILE_Release(pIFile);
      pIFile = NULL;
      MP_RELEASEIF(pIFileMgr);

      if (((CPlayerWin *)pme->m_pWin)->m_pszFile)
      {
        FREEIF(((CPlayerWin *)pme->m_pWin)->m_pszFile);
      }

      //Set the media data
      (void)MEMCPY(&((CPlayerWin *)pme->m_pWin)->m_md, &md, sizeof(AEEMediaData));

      // If file successfully read into memory set the file name.
      if (playBuf != NULL)
      {
        ((CPlayerWin *)pme->m_pWin)->m_pszFile = (char *) pszBuf;
      }
      else
      {
        FREE(pszBuf);
        return FALSE;
      }
    }
    else if (pme->m_bLocalPlayPullMode && MP_IsMediaVideo(pme->a.m_pIShell, fiName))
    {
      // For Local Play PULL Mode, get and save file info

      IFileMgr       *pIFileMgr;
     // IFile          *pIFile = NULL;
      AEEFileInfoEx  fi;

      DBGPRINTF_MED("MP: MediaPlayer is getting file size for Local Play Pull Mode");

      (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);

      if (NULL == pIFileMgr)
      {
        DBGPRINTF_ERROR("MP: MediaPlayer was unable to create AEECLSID_FILEMGR");
        return FALSE;
      }

      //if ((pIFile = IFILEMGR_OpenFile(pIFileMgr, pszBuf, _OFM_READ)) == NULL)
      //{
      //  DBGPRINTF_ERROR("MP: MediaPlayer was unable to open file %s", pszBuf);
      //  MP_RELEASEIF(pIFileMgr);
      //  FREE(pszBuf);
      //  return FALSE;
      //}


      if (((CPlayerWin *)pme->m_pWin)->m_pszFile)
      {
        FREEIF(((CPlayerWin *)pme->m_pWin)->m_pszFile);
      }

      //Set the media data
      //(void)MEMCPY(&((CPlayerWin *)pme->m_pWin)->m_md, &md, sizeof(AEEMediaData));

      // save file name.
      ((CPlayerWin *)pme->m_pWin)->m_pszFile = (char *) pszBuf;

      //MP_LocalPlayPullModeFetchBufferedData(pme, pme->m_szBuf, 64, 0);
      //***********************************************************
   
	    if(pme->m_fsHandle)
	    {
        fs_rsp_msg_type fsResult;
    
        fs_close( pme->m_fsHandle, NULL, &fsResult );
        if ( fsResult.close.status != FS_OKAY_S )
        {
             ;        
        }  
    
        pme->m_fsHandle = 0;
      }

      (void)MEMSET(pme->m_szBuf,0,sizeof(pme->m_szBuf));
      (void)MEMSET(&fi,0,sizeof(fi));

      fi.nStructSize = sizeof(fi);
      if (IFILEMGR_GetInfoEx(pIFileMgr, pszBuf, &fi) == SUCCESS)
      {
         pme->m_nFileSize = fi.dwSize;
         md.clsData = MMD_HTTP_PULL_BUFFER_PLAYBACK;
         md.pData = (void *)pme->m_szBuf;
         md.dwSize = sizeof(pme->m_szBuf);
         //Set the media data
        
         (void)MEMCPY(&((CPlayerWin *)pme->m_pWin)->m_md, &md, sizeof(AEEMediaData));
      }

      // Free File resources 
     // (void)IFILE_Release(pIFile);
     // pIFile = NULL;
      MP_RELEASEIF(pIFileMgr);   
    }
    else
    {
      md.clsData = MMD_FILE_NAME;
      md.pData = (void *)pszBuf;
      (void)CPlayerWin_SetMediaData((CPlayerWin *)pme->m_pWin, &md);
    }

    //if (md.pData != NULL)
    {
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
      {
        DBGPRINTF_MED("MP: Posting EVT_CREATEMEDIA event");
        (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_CREATEMEDIA, 0, 0);
      }
      if (pme->m_bPlugin)
      {
        DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
        (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
      }
    }
  }
  return TRUE;
}

// Auto play the current clip.  Ensuring that MediaPlayer is in a state
// that will allow play to happen
static void CPlayerWin_AutoPlayClip(CMediaPlayer *pme)
{
  boolean   bChg = 0;
  int       nState = 0;
  CPlayerWin* pWin = NULL;

  DBGPRINTF_MED("MP: In CPlayerWin_AutoPlayClip");
  
  if (pme->m_bSuspend)
  {
    // We are currently suspended, ignore this callback and don't do anything
    DBGPRINTF_MED("MP: Ignoring AutoPlayClip because we are suspended");
    return;
  }

  pWin = (CPlayerWin*) pme->m_pWin;

  if (NULL == pWin)
  {
    // We have no window, just exit
    DBGPRINTF_ERROR("MP: AutoPlayClip Called with Null Player Window");
    return;
  }

  if (!pWin->m_bAutoPlayIsFirstClip)
  {
    DBGPRINTF_MED("MP: Not AutoPlaying, not first clip");
    return;
  }

  /*
   * make sure IMedia is in the ready state and we are in the ready state
   */
  if (pWin->m_pMedia)
  {
    DBGPRINTF_MED("MP: Our m_pMedia pointer is %d", pWin->m_pMedia);
    nState = IMEDIA_GetState(pWin->m_pMedia, &bChg);
  }
  /*
   * If already playing, don't need to autostart
   */
  if (MP_PLAYSTATE_PLAYING == pme->m_playState)
  {
    DBGPRINTF_MED("MP: AutoPlayClip Exit, already playing");
    return;
  }

  /* If we are currently displaying a dialog, don't auto start while it is active */
  if((pWin->m_bDIsplayInfo) || (pme->m_pDlg != NULL))
  {
    DBGPRINTF_MED("MP: Dialog active wait until completed.");
     // wait for 500 ms and try it again
    (void)ISHELL_SetTimer(pWin->m_pIShell, 500, (PFNNOTIFY)CPlayerWin_AutoPlayClip, pme);
    return;
  }
 
  /*
   * try max. 1 second as long as the IMediaState is changing or its state is not READ and our internal state
   * is not ready
   */
  if (bChg || MM_STATE_READY != nState || (!pWin->m_bIsCMXFile && !pme->m_bVideoPlaybackReady))
  {
    DBGPRINTF_MED("MP: IMedia is still not in the ready state, start timer and try again in 50ms.");
    /*
     * wait for 50 ms and try it again
     */
    (void)ISHELL_SetTimer(pWin->m_pIShell, 50, (PFNNOTIFY)CPlayerWin_AutoPlayClip, pme);
    return;
  }

  /*
   * When in continuous play mode, make sure that only the first clip is auto-started
   */
  if (pme->m_nContMode)
  {
    pWin->m_bAutoPlayIsFirstClip = FALSE;
  }

  DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
  (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
}

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
// this function creates PlayerWin on a given media
// pObject, IMedia for media or IImage for image type
// type indicates PlayerWin type to be created.
// pszFile, physical file location or URI if called from IContent object
boolean MPCreatePlayerWin(CMediaPlayer *pme, void* pObject, MPPlayerWin type, char* pszFile)
{
  AEEMediaData md;
  CPlayerWin *ppw;
  boolean bRet = FALSE;
  char* pszBuf = NULL;

  if(pObject == NULL)
    return bRet;

  if(pszFile)
  {
    pszBuf = STRDUP(pszFile);
    if (!pszBuf)
      return FALSE;
  }

  switch(type)
  {
    case MPPW_PLAY:
      // need to verify type here...
      if(CMediaPlayer_SetWindow(pme, MPW_PLAYER, MPPW_PLAY))
      {
        ((CPlayerWin *)pme->m_pWin)->m_pMedia = pObject;

#ifdef MP_FEATURE_VIDEO
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
         CPlayerWin_ResetFrameTransformations((CPlayerWin *)pme->m_pWin);
#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS ||
           // FEATURE_QTV_MDP_TRANSFORMATIONS
#endif     // MP_FEATURE_VIDEO

        if(NULL == pme->m_pContentISource)
        {
          md.clsData = MMD_FILE_NAME;
          md.pData = (void *)pszBuf;
          md.dwSize = 0;
          (void)CPlayerWin_SetMediaData((CPlayerWin *)pme->m_pWin, &md);
        }
        else
        {
          md.clsData = MMD_ISOURCE;
          md.pData = (void *)pme->m_pContentISource;
          md.dwSize = 0;
          // free any previous memory. This is valid in continuous mode
          if((((CPlayerWin *)(pme->m_pWin)))->m_pszFile)
            FREEIF((((CPlayerWin *)(pme->m_pWin)))->m_pszFile);

          (((CPlayerWin *)(pme->m_pWin)))->m_pszFile = (char *)(pszBuf);
          MEMCPY(&(((CPlayerWin *)(pme->m_pWin))->m_md), &md, sizeof(AEEMediaData));
        }
        // we still need to post this event, even we have had the media object already.
        // the handler function will perform setup and retrieve some information
        (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_CREATEMEDIA, 0, 0);

        if (pme->m_IContentResumePos > 0)
        {
          // save the position
          pme->m_resumePlayPos = pme->m_IContentResumePos;
          (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_MEDIAPLAYER_SEEK, 0, pme->m_IContentResumePos);
          DBGPRINTF_HIGH("MP: MediaPlayer IContent resuming, resume position is %d\n", pme->m_IContentResumePos);
        }
        pme->m_IContentResumePos = 0;

        // if we are in plug-in mode, or resume from MP interrupt
        if (pme->m_bPlugin || pme->m_bIContentResumePlay)
        {
          DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
          (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
        }
        pme->m_bIContentResumePlay = FALSE;

        bRet = TRUE;
      }
      break;
    case MPPW_IMAGE:
      if(pme->m_nContMode != CONT_MODE_ALL_FILES)
      {
        if(CMediaPlayer_SetWindow(pme, MPW_PLAYER, MPPW_IMAGE))
        {
          pme->m_bIsImage = TRUE;
          ppw = (CPlayerWin *)pme->m_pWin;
          ppw->m_bImage = TRUE;                // This is an image
          ppw->m_pszFile = (char *)pszBuf;     // Save the pointer
          ppw->m_bLoadingImage = FALSE;
          ppw->m_nWaitTimerCnt  = 0;

          if (SUCCESS != ISHELL_CreateInstance(ppw->m_pIShell, AEECLSID_IMAGECTL, (void **)&ppw->m_pImageCtl))
          {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
            MP_ErrorDlg(ppw->m_pOwner, IDS_ERR_CREATEMEDIA);
          }
          else
          {
            ppw->m_pImage = pObject;
            ppw->m_bLoadingImage = TRUE;
            DBGPRINTF_MED("MP: Calling IIMAGECTL_SetRect, with %d,%d -> %d,%d", ppw->m_rectImage.x, ppw->m_rectImage.y,
              ppw->m_rectImage.dx, ppw->m_rectImage.dy);
            IIMAGECTL_SetRect(ppw->m_pImageCtl, &ppw->m_rectImage);
            IIMAGECTL_SetImage(ppw->m_pImageCtl, ppw->m_pImage);
            pme->m_bRedraw = FALSE;
            ppw->m_bLoadingImage = FALSE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION

            CMediaPlayer_Redraw(pme, FALSE);
            bRet = TRUE;
          }
        }
      }

      break;
    default:
      break;
  }
  // We did an strdup above and if we fail, we need to clean it up
  if((bRet != TRUE) && pszBuf)
     FREE(pszBuf);
  return bRet;
}

boolean MPContentPlayNext(CMediaPlayer *pme)
{
  boolean bRet = FALSE;
  CPlayerWin *ppw;
 
  if(pme == NULL)
     return bRet;

  if((pme->m_nContMode != CONT_MODE_ALL_FILES) && (pme->m_nContMode != CONT_MODE_SINGLE_FILE))
     return bRet;

  if(pme->m_pWin && pme->m_eActiveWin != MPW_PLAYER)
     return bRet;

  ppw = (CPlayerWin *)(pme->m_pWin);

  if(CPlayerWin_PlayNext(ppw, pme->m_nContMode, TRUE))
  {
    bRet = TRUE;
    DBGPRINTF_LOW("MP: Content continuous mode on, continuing...");
  }
  else
  {
    DBGPRINTF_ERROR("MP: Content continuous mode replay failed.");
  }
 
  return bRet;
}
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

#ifdef FEATURE_MULTISEQUENCER

/*===========================================================================
   This function opens the player window to play a sequence of files.
===========================================================================*/
void CMediaPlayer_PlayMultiSequence(CMediaPlayer *pme)
{
  uint16    nIndex;
  int       nRet;
  CSequencePlayerWin *pWin;

  if (pme->m_MSFiles.m_wNumSequences == 0)
  {
    /*
     * No files were selected in the sequence
     */
    MP_ErrorDlg(pme, IDS_ERR_NOSEQUENCE);
    return;
  }

  if (!CMediaPlayer_SetWindow(pme, MPW_SEQUENCE_PLAYER, 0))
  {
    return;
  }

  pWin = (CSequencePlayerWin *)pme->m_pWin;

  for (nIndex = 0; nIndex < pme->m_MSFiles.m_wNumSequences; nIndex++)
  {
    AEEMediaData md;

    md.clsData = MMD_FILE_NAME;
    md.pData = (void *)pme->m_MSFiles.m_szSequenceFiles[nIndex];
    md.dwSize = 0;
    pWin->m_sSequenceFile[nIndex].m_pszFile = pme->m_MSFiles.m_szSequenceFiles[nIndex];
    (void)MEMCPY(&(pWin->m_sSequenceFile[nIndex].m_md), &md, sizeof(AEEMediaData));
    nRet =
      AEEMediaUtil_CreateMedia(pWin->m_pIShell,
                               &pWin->m_sSequenceFile[nIndex].m_md,
                               &(pWin->m_sSequenceFile[nIndex].m_pMedia));
    if (SUCCESS != nRet)
      MP_ErrorDlg(pWin->m_pOwner, IDS_ERR_CREATEMEDIA);
    else
    {
      uint32    dwCaps;

      (void)IMEDIA_GetMediaParm(pWin->m_sSequenceFile[nIndex].m_pMedia, MM_PARM_CAPS,
                          (int32 *) & dwCaps, 0);
      pWin->m_sSequenceFile[nIndex].m_bImage =
        (dwCaps & MM_CAPS_VIDEO) ? TRUE : FALSE;

      DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with x = %d, y = %d, cx = %d, cy = %d", pWin->m_sSequenceFile[nIndex].m_rectImage.x,
        pWin->m_sSequenceFile[nIndex].m_rectImage.y, pWin->m_sSequenceFile[nIndex].m_rectImage.dx, pWin->m_sSequenceFile[nIndex].m_rectImage.dy);
      (void)IMEDIA_SetRect(pWin->m_sSequenceFile[nIndex].m_pMedia,
                     &pWin->m_sSequenceFile[nIndex].m_rectImage, NULL);
      (void)IMEDIA_EnableChannelShare(pWin->m_sSequenceFile[nIndex].m_pMedia, TRUE);
      if (SUCCESS !=
          IMEDIA_RegisterNotify(pWin->m_sSequenceFile[nIndex].m_pMedia,
                                MP_SequencePlayerWin_MediaNotify, pWin->m_pOwner))
        MP_ErrorDlg(pWin->m_pOwner, IDS_ERR_MEDIA);
    }
  }
  CMediaPlayer_Redraw(pWin->m_pOwner, TRUE);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
}
#endif     // FEATURE_MULTISEQUENCER

/*===========================================================================
   This function opens the record window with the specified file.
===========================================================================*/
static void
CMediaPlayer_RecordQCPFile(CMediaPlayer *pme, const char *pszFile)
{
  IFileMgr *pIFileMgr;
  AEEMediaData md;
  char     *pszBuf;
  char      sz[2] = { DIRECTORY_CHAR, 0 };
  boolean   bMMC = FALSE;
  boolean   bUIM = FALSE;
#if MIN_BREW_VERSION(4,0)
  char      pszUIMStoragePath[MP_MAX_FILE_NAME] = {0};
#endif // #if MIN_BREW_VERSION(4,0)

  (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR,
                              (void **)&pIFileMgr);
  if (pIFileMgr)
  {
#ifdef FEATURE_MP_RECORDER_SETTINGS
    if (pme->m_SaveLocation == SAVELOC_FLASH)
    {
      /*
       * record on flash mem
       */
      if (!MP_MakeDir(pIFileMgr, MP_MEDIA_DIR))
      {
        DBGPRINTF_ERROR("MP: Recorder failed to create root directory on flash.");
        MP_RELEASEIF(pIFileMgr);
        return;
      }
    }
    else if (pme->m_SaveLocation == SAVELOC_MMC)
    {
      if (MP_MakeDir(pIFileMgr, MP_MMC_MEDIA_DIR))
        bMMC = TRUE;
    }
    else if (pme->m_SaveLocation == SAVELOC_UIM)
    {
#if MIN_BREW_VERSION(4,0)
      (void)MEMSET(pszUIMStoragePath, 0, sizeof (pszUIMStoragePath) );
      if (MP_GetUIMMediaPlayerRoot(pIFileMgr, pszUIMStoragePath, sizeof (pszUIMStoragePath)))
        bUIM = TRUE;
#endif //#if MIN_BREW_VERSION(4,0)
    }
#else
    /*
     * record on flash mem
     */
    if (!MP_MakeDir(pIFileMgr, MP_MEDIA_DIR))
    {
      DBGPRINTF_ERROR("MP: Recorder failed to create root directory.");
      MP_RELEASEIF(pIFileMgr);
      return;
    }
#endif
    MP_RELEASEIF(pIFileMgr);
  }

  /*
   * File name is a special case...
   * pszFile will be released by CFileListWin when CPlayerWin opens.
   * Allocate memory for file name here. It will be used and released
   * by CPlayerWin.
   */
  pszBuf = MALLOC(MP_MAX_FILE_NAME);
  if (!pszBuf)
    return;
#ifdef FEATURE_MP_RECORDER_SETTINGS
  if (pme->m_SaveLocation == SAVELOC_MMC && bMMC)
    (void)STRLCPY(pszBuf, MP_MMC_MEDIA_DIR, MP_MAX_FILE_NAME);
  else if (pme->m_SaveLocation == SAVELOC_UIM && bUIM)
  {
#if MIN_BREW_VERSION(4,0)
    (void)STRLCPY(pszBuf, pszUIMStoragePath, MP_MAX_FILE_NAME);
#endif //#if MIN_BREW_VERSION(4,0)
  }
  else
#endif
    (void)STRLCPY(pszBuf, MP_MEDIA_DIR, MP_MAX_FILE_NAME);
  (void)STRLCAT(pszBuf, sz, MP_MAX_FILE_NAME);
  (void)STRLCAT(pszBuf, pszFile, MP_MAX_FILE_NAME);

  md.clsData = MMD_FILE_NAME;
  md.pData = (void *)pszBuf;
  md.dwSize = 0;

  if (!CMediaPlayer_SetWindow(pme, MPW_PLAYER, (uint32)MPPW_RECORD))
  {
    FREE(pszBuf);
    return;
  }

  if (CPlayerWin_SetMediaData((CPlayerWin *)pme->m_pWin, &md))
    (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                     EVT_CREATEMEDIA_QCP, 0, 0);
}

#ifdef FEATURE_MP_RECORDER_SETTINGS
static boolean
CMediaPlayer_CreateRecorderWindow(CMediaPlayer *pme, AEEMediaData *md)
{
  if (!CMediaPlayer_SetWindow(pme, MPW_PLAYER, (uint32)MPPW_RECORD))
    return FALSE;

  if (CPlayerWin_SetMediaData((CPlayerWin *)pme->m_pWin, md))
  {
    (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                     EVT_CREATEMEDIA_QCP, 0, 0);
    return TRUE;
  }
  return FALSE;
}
#endif

/*===========================================================================
   This function opens the streaming record window.
===========================================================================*/
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
static boolean
CMediaPlayer_CreateStreamRecordWindow(CMediaPlayer *pme, const char *pszFile)
{
  AEEMediaData md;
  char     *pszBuf;
  char     *fiName,
           *extName;
  uint16   fileType;

#if defined(MP_FEATURE_VIDEO)
  char      VideoFile[MP_MAX_FILE_NAME];
  char      AudioFile[MP_MAX_FILE_NAME];
  boolean   bLoopTrack = FALSE;
#endif

   if (pszFile == NULL)
  {
    DBGPRINTF_ERROR("MP: pszFile is NULL!");
    return FALSE;
  }
  if (pszFile[0] == 0)
  {
    DBGPRINTF_ERROR("MP: pszFile is empty string!");
    return FALSE;
  }

  /*
   * File name is a special case...
   * pszFile will be released by CFileListWin when CPlayerWin opens.
   * Allocate memory for file name here. It will be used and released
   * by CPlayerWin.
   */
  pszBuf = STRDUP(pszFile);
  if (pszBuf == NULL)
  {
    DBGPRINTF_ERROR("MP: STRDUP failed for %s", pszFile);
    return FALSE;
  }

  DBGPRINTF_MED("MP: PlayFile: %s", pszFile);

  /*
   * If launched as a plug-in then another application may specify
   * the filepath directory, so don't search the default filelist.
   */
  if (!pme->m_bPlugin && !CMediaPlayer_FindFileInFileList(pme, pszBuf))
  {
    FREE(pszBuf);
    return FALSE;
  }

  /*
   * Get the filename.
   */
  fiName = MP_GetFileName(pszBuf);
  /*
   * Check if fiName is null
   */
  if (fiName == NULL)
  {
    FREE(pszBuf);
    DBGPRINTF_ERROR("MP: filename missing or not found!");
    return FALSE;
  }

  /*
   * Get the filename extension.
   */
  extName = MP_GetFileNameExtension(fiName);
  /*
   * Check if extName is null
   */
  if (extName == NULL)
  {
    FREE(pszBuf);
    DBGPRINTF_ERROR("MP: filename extension missing or not found!");
    return FALSE;
  }
  DBGPRINTF_MED("MP: MediaPlayer finds %s as filename extension\n", extName);

  if (!(STRICMP(extName, "url")))
  {
    IFileMgr       *pIFileMgr;
    IFile          *pIFile = NULL;
    AEEFileInfoEx  fi;
    unsigned int   size = 0;
    char           pUrl[PS_MAX_URL_LEN];

    DBGPRINTF_MED("MP: MediaPlayer will try to play a URL file\n");
    (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR,
                                (void **)&pIFileMgr);

    if (NULL == pIFileMgr)
    {
      FREE(pszBuf);
      return FALSE;
    }

    if ((pIFile = IFILEMGR_OpenFile(pIFileMgr, pszBuf, _OFM_READ)) == NULL)
    {
      FREE(pszBuf);
      MP_RELEASEIF(pIFileMgr);
      pIFileMgr = NULL;
      return FALSE;
    }

    (void)MEMSET(&fi,0,sizeof(fi));
    fi.nStructSize = sizeof(fi);
    if (IFILEMGR_GetInfoEx(pIFileMgr, pszBuf, &fi) == SUCCESS)
    {
      int       rSize = 0;

      size = fi.dwSize;
      if (size <= PS_MAX_URL_LEN)
      {
        rSize = IFILE_Read(pIFile, pUrl, size);
        pUrl[rSize] = '\0';
      }

      (void)IFILE_Release(pIFile);
      pIFile = NULL;
      MP_RELEASEIF(pIFileMgr);
      pIFileMgr = NULL;

      /*
       * case insensitive search for string "http"
       */
      if (!(STRNICMP(pUrl, "http://", 7)))
      {                                // http was found
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
        boolean   retval = TRUE;
        pme->m_bIsHTTP = TRUE;
        retval = CMediaPlayer_PseudoPlayFile(pme, pszFile);
        FREE(pszBuf);
        return retval;
#endif     // FEATURE_QTV_PSEUDO_STREAM ||
           // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      }
      else if (!(STRNICMP(pUrl, "rtsp://", 7)))
      {
        if (!MP_GetRecordFileName_Streaming(recFileName, pUrl, defaultRecDir))
          return FALSE;
      }
    }
    else
    {
      (void)IFILE_Release(pIFile);
      pIFile = NULL;
      MP_RELEASEIF(pIFileMgr);
      pIFileMgr = NULL;
    }
  }

#ifdef FEATURE_MP_EQ
    pme->m_bShowPlayerWinSettingUI = TRUE;
    pme->m_bShowLogo = FALSE;
#endif

  fileType = CMediaPlayer_FindHandlerType(pme, pszFile);
  if (IDB_IMAGE_ICON == fileType)  /* Don't handle images. */
    return FALSE;

  /*
   * This is not image...it must be media.
   */
  if (!CMediaPlayer_SetWindow(pme, MPW_STREAM_RECORDER, (uint32)MPPW_STREAM_RECORD))
  {
    FREE(pszBuf);
    return FALSE;
  }

#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
  CPlayerWin_ResetFrameTransformations((CPlayerWin *)pme->m_pWin);
#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS ||
           // FEATURE_QTV_MDP_TRANSFORMATIONS

#if defined(FEATURE_QTV_STREAM_RECORD)
  if (!(STRICMP(extName, "sdp")))
  {
    if (!MP_GetRecordFileName_SDP(recFileName, fiName, defaultRecDir))
      return FALSE;

  }
#endif
  if (!STRICMP(extName, "MFPT") &&
      MP_GetAVFileNames(pme->a.m_pIShell, pszFile, VideoFile, AudioFile,
                        MP_MAX_FILE_NAME, &bLoopTrack))
  {
    ((CPlayerWin *)pme->m_pWin)->m_bLoopTrack = bLoopTrack;
    if (CPlayerWin_SetMediaDataEx
        ((CPlayerWin *)pme->m_pWin, VideoFile, AudioFile))
    {
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_CREATEMEDIA_EX, 0, 0);
      if (pme->m_bPlugin)
      {
        DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
        (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
      }
    }
    FREE(pszBuf);
  }
  else
  {
    md.clsData = MMD_FILE_NAME;
    md.pData = (void *)pszBuf;
    md.dwSize = 0;

    if (CPlayerWin_SetMediaData((CPlayerWin *)pme->m_pWin, &md))
    {
      (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_CREATEMEDIA,
                       0, 0);
      if (pme->m_bPlugin)
      {
        DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
        (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
      }
    }
  }
  return TRUE;
}
#endif  //end defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)

/*=====================================================================
FUNCTION: CMediaPlayer_PlayImage

DESCRIPTION: 
  This function displays the image in the player window.

PARAMETERS:
  pme     - Ptr to CMediaPlayer object.
  pszFile - Ptr to Image file name.  Saved in pme->m_pWin->m_pszFile.

RETURN VALUE:
  TRUE = Image successfully loaded and displayed
======================================================================*/
static boolean
CMediaPlayer_PlayImage(CMediaPlayer *pme, const char *pszFile)
{
  CPlayerWin *ppw;
  IImage   *pImage = ISHELL_LoadImage(pme->a.m_pIShell, pszFile);
  if (!pImage)
  {
    DBGPRINTF_ERROR("MP: Failed to Load Image");
    return FALSE;
  }

  if (!CMediaPlayer_SetWindow(pme, MPW_PLAYER, MPPW_IMAGE) &&
      pme->m_nContMode != CONT_MODE_ALL_FILES)
  {
    DBGPRINTF_ERROR("MP: Failed to Set Window for Image");
    (void)IIMAGE_Release(pImage);
    return FALSE;
  }

  if ((pszFile != NULL) && (pszFile[0] != 0))
  {
    DBGPRINTF_MED("MP: MediaPlayer is playing image file %s\n", pszFile);
  }

  pme->m_bIsImage = TRUE;
  ppw = (CPlayerWin *)pme->m_pWin;
  ppw->m_bImage = TRUE;                // This is an image
  ppw->m_pszFile = (char *)pszFile;    // Save the pointer
  ppw->m_bLoadingImage = FALSE;
  ppw->m_nWaitTimerCnt  = 0;

  if (ISHELL_CreateInstance (ppw->m_pIShell, AEECLSID_IMAGECTL, (void **)&ppw->m_pImageCtl))
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    MP_ErrorDlg(ppw->m_pOwner, IDS_ERR_CREATEMEDIA);
  }
  else
  {
    DBGPRINTF_MED("MP: Setting Image rect to x = %d, y = %d, dx = %d, dy = %d",
      ppw->m_rectImage.x, ppw->m_rectImage.y, ppw->m_rectImage.dx, ppw->m_rectImage.dy);
    IIMAGECTL_SetRect(ppw->m_pImageCtl, &ppw->m_rectImage);
    IIMAGECTL_SetImage(ppw->m_pImageCtl, pImage);
    ppw->m_pImage = pImage;
    ppw->m_bLoadingImage = TRUE;

    IIMAGE_Notify(pImage, CPlayerWin_ImageNotify, ppw);
    pme->m_bRedraw = FALSE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
  }

  return TRUE;
}

/*===========================================================================
   This functions sets the image used for the idle screen.  This can be
   used to diplay a format specific splash screen
===========================================================================*/
static boolean
CMediaPlayer_SetIdleImage(CPlayerWin *pme, unsigned long resourceID)
{
  IImage   *image = ISHELL_LoadResImage(pme->m_pIShell, MEDIAPLAYER_RES_FILE,
                                        resourceID);
  if (image)
  {
    if (pme->m_pIdleImage)
    {
      (void)IIMAGE_Release(pme->m_pIdleImage);
    }
    pme->m_pIdleImage = image;
  }
  return image != NULL;
}

/*===========================================================================
   This function returns the image/audio/default icon resource id if
   extension is present in the internal list.
===========================================================================*/
uint16 CMediaPlayer_IsExtension(CMediaPlayer *pme, const char *pszFile,
                                char *pszInExt, int nExtLen)
{
  char     *psz;
  uint16    wRet = 0;
  char     *szExt;
  int       extLen = 0;

  if(pszFile == NULL || pszInExt == NULL)
  {
    DBGPRINTF_ERROR("MP: NULL ptr.");
    return 0;
  }

  psz = STRRCHR(pszFile, '.');
  if (!psz)
    return 0;

  psz++;
  if (!*psz)
    return 0;

#ifdef FEATURE_MULTISEQUENCER
  if (pme->m_bIsMultiSequencer)
  {
    int i = 0;
    for (i = 0 ; allowedExts[i][0] != 0 ; i++)
    {
      /* If match an allowed extension, quit looking. */
      if (!STRNCMP(psz, allowedExts[i], STRLEN(allowedExts[i])))
        break;
    }

    /* If got to end without matching an allowed extension, then return/exit. */
    if (allowedExts[i][0] == 0)
      return 0;
  }
#endif     // FEATURE_MULTISEQUNCER

  extLen = STRLEN(psz);
  szExt = MALLOC(extLen + STRLEN(MP_EXT_SEPARATOR) + 1);
  if (!szExt)
    return 0;

  /*
   * Form a string like "mid, "
   */
  (void)STRLCPY(szExt, psz, (extLen + STRLEN(MP_EXT_SEPARATOR) + 1));
  (void)STRLCAT(szExt, MP_EXT_SEPARATOR, (extLen + STRLEN(MP_EXT_SEPARATOR) + 1));

  if (pszInExt && nExtLen > (int)STRLEN(psz))
    (void)STRLCPY(pszInExt, psz, nExtLen);

  if (pme->m_pszAudioExt && STRSTR(pme->m_pszAudioExt, szExt))
    wRet = IDB_AUDIO_ICON;
  else if (pme->m_pszVideoExt && STRSTR(pme->m_pszVideoExt, szExt))
    wRet = IDB_VIDEO_ICON;
  else if (pme->m_pszImageExt && STRSTR(pme->m_pszImageExt, szExt))
    wRet = IDB_IMAGE_ICON;

  FREE(szExt);
  return wRet;
}

/*===========================================================================
   This function tries to find the handler for an extension as follows:
   (1)Search the internal lists for audio, video and image extensions
   (2)If not found, then query Shell Registry for audio/video/image based
       handler
   (3)If found, add the extension to internal list
===========================================================================*/
uint16 CMediaPlayer_FindHandlerType(CMediaPlayer *pme, const char *pszFile)
{
  AEECLSID  cls;
  char      szMIME[32];
  char      szExt[10] = "";
  uint16    wExtType;
  int       IconType = 0;

  if(pszFile == NULL)
  {
    DBGPRINTF_ERROR("MP: NULL ptr.");
    return IconType;
  }

  wExtType =   CMediaPlayer_IsExtension(pme, pszFile, szExt, sizeof(szExt));
  if (wExtType)
    return wExtType;

  if (!STRCMP(szExt, ""))
  {
    DBGPRINTF_MED("MP: no extension was found for file %s", pszFile);
    return IconType;
  }

  /*
   * Now query the Shell Registry...
   */

  /*
   * Look for audio based handler
   */
  (void)STRLCPY(szMIME, AUDIO_MIME_BASE, sizeof(szMIME));
  (void)STRLCAT(szMIME, szExt, sizeof(szMIME));
  cls = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_MEDIA, szMIME);
  if (cls)
  {
    if (MP_AddExtension(&pme->m_pszAudioExt, szExt))
    {
      return IDB_AUDIO_ICON;
    }
    DBGPRINTF_HIGH("MP: Could not AddExtension for %s", pszFile);
  }

  /*
   * Look for video based handler
   */
  (void)STRLCPY(szMIME, VIDEO_MIME_BASE, sizeof(szMIME));
  (void)STRLCAT(szMIME, szExt, sizeof(szMIME));
  cls = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_MEDIA, szMIME);
  if (cls)
  {
    if  (MP_AddExtension(&pme->m_pszVideoExt, szExt))
    {
      return IDB_VIDEO_ICON;
    }
    DBGPRINTF_HIGH("MP: Could not AddExtension for %s", pszFile);
  }

  /*
   * Look for image based handler
   */
  (void)STRLCPY(szMIME, IMAGE_MIME_BASE, sizeof(szMIME));
  (void)STRLCAT(szMIME, szExt, sizeof(szMIME));
  cls = ISHELL_GetHandler(pme->a.m_pIShell, HTYPE_VIEWER, szMIME);
  if (cls)
  {
    if (MP_AddExtension(&pme->m_pszImageExt, szExt))
    {
      return IDB_IMAGE_ICON;
    }
    DBGPRINTF_HIGH("MP: Could not AddExtension for %s", pszFile);
  }

  /*
   * Look for MP handled media
   */
  if(CMediaPlayer_FindMediaPlayerHandlerType(szExt, &IconType))
  {
    return IconType;
  }

  DBGPRINTF_HIGH("MP: Could not find Association for %s", pszFile);
  return IconType;
}

// find mp supported media type
static boolean CMediaPlayer_FindMediaPlayerHandlerType(const char *pszFileExt, int* pIconType)
{
  if(!pszFileExt)
    return FALSE;

  *pIconType = 0;
#ifdef MP_FEATURE_ICONTENTHANDLER
  // icontent protected type
  if (STRCMP(pszFileExt, "dcf") == 0 ||
      STRCMP(pszFileExt, "odf") == 0 ||
      STRCMP(pszFileExt, "o4a") == 0 ||
      STRCMP(pszFileExt, "o4v") == 0)
  {
    *pIconType = IDB_FILE_ICON;
    return TRUE;
  }
#endif //MP_FEATURE_ICONTENTHANDLER

  return FALSE;
}

boolean CMediaPlayer_GetMIMETypeString(CMediaPlayer *pme, const char *pszFile, int size, char* pMIME)
{
  AEECLSID  cls;
  char      szMIME[32];
 // char     *pExt;

  char      pExt[10] = "";

  (void)CMediaPlayer_IsExtension(pme, pszFile, pExt, sizeof(pExt));

  // ISHELL_DetectType should work

 // pExt = MP_GetFileNameExtension(pszFile);
  /*
   * Look for audio based handler
   */
  (void)STRLCPY(szMIME, AUDIO_MIME_BASE, sizeof(szMIME));
  (void)STRLCAT(szMIME, pExt, sizeof(szMIME));
  cls = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_MEDIA, szMIME);
  if (cls)
  {
    (void)STRLCPY(pMIME, szMIME, size);
    return TRUE;
  }

  /*
   * Look for video based handler
   */
  (void)STRLCPY(szMIME, VIDEO_MIME_BASE, sizeof(szMIME));
  (void)STRLCAT(szMIME, pExt, sizeof(szMIME));
  cls = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_MEDIA, szMIME);
  if (cls)
  {
    (void)STRLCPY(pMIME, szMIME, size);
    return TRUE;
  }

  /*
   * Look for image based handler
   */
  (void)STRLCPY(szMIME, IMAGE_MIME_BASE, sizeof(szMIME));
  (void)STRLCAT(szMIME, pExt, sizeof(szMIME));
  cls = ISHELL_GetHandler(pme->a.m_pIShell, HTYPE_VIEWER, szMIME);
  if (cls)
  {
    (void)STRLCPY(pMIME, szMIME, size);
    return TRUE;
  }

  return FALSE;

}
/*===========================================================================

                     CWindow Functions

===========================================================================*/

/*===========================================================================
   This function is the base class constructor of an IWindow object.
   It allocates memory for the interface and sets the vtbl.
===========================================================================*/
IWindow * CWindow_New(int16 nSize, CMediaPlayer *pOwner, VTBL(IWindow) * pvt)
{
  CWindow  *pme;

  VTBL(IWindow) *pVtbl;

  pme = MALLOC(nSize + sizeof(VTBL(IWindow)));
  if (!pme)
    return NULL;

  (void)MEMSET(pme, 0, nSize + sizeof(VTBL(IWindow)));

  pme->m_pOwner = pOwner;
  pme->m_pIShell = pme->m_pOwner->a.m_pIShell;
  pme->m_pIDisplay = pme->m_pOwner->a.m_pIDisplay;
#ifdef FEATURE_KEY_HOLD_SCROLL
  pme->m_pOwner->m_KeyStore.m_dwParam_Key = 0;
#endif
  pVtbl = (VTBL(IWindow) *) ((byte *)pme + nSize);
  (void)MEMCPY(pVtbl, pvt, sizeof(VTBL(IWindow)));
  INIT_VTBL(pme, IWindow, *pVtbl);

  return (IWindow *)pme;
}

/*===========================================================================
   This function indicates if IWINDOW_Enable()/IWINDOW_Disable()needs to be
   processed and also sets the bActive flag appropriately.
   It is called by derived class IWINDOW_Enable()function before processing
   the command.
===========================================================================*/
boolean CWindow_ProcessEnable(IWindow *po, boolean bEnable)
{
  CWindow  *pme = (CWindow *)po;
  boolean   bProc = TRUE;

  if (!bEnable)
  {
    if (pme->m_bActive)
    {
      pme->m_bActive = FALSE;
      CMediaPlayer_CancelRedraw(pme->m_pOwner);
    }
    else
      bProc = FALSE;
  }
  else
  {
    if (pme->m_bActive)
    {
      bProc = FALSE;
    }
    else
    {
      pme->m_bActive = TRUE;
    }
  }

  return bProc;
}

/*===========================================================================
   This function returns the index of previous file in that directory.
===========================================================================*/
static int CMediaPlayer_GetPreviousFileIndex(CMediaPlayer *pme, int nCurFileIndex)
{
  int       i;

  if (nCurFileIndex == 0)
    nCurFileIndex = pme->m_FileExplorer.m_NumFiles - 1;
  else
    nCurFileIndex--;

  for (i = 0; i < pme->m_FileExplorer.m_NumFiles; i++)
  {
    // current one is a valid file, return the idx
    if (pme->m_FileExplorer.m_FileArray[nCurFileIndex].m_bIsFile)
      return nCurFileIndex;

    // move to previous idx
    if (nCurFileIndex == 0)
      nCurFileIndex = pme->m_FileExplorer.m_NumFiles - 1;
    else
      nCurFileIndex--;
  }
  return 0;
}
/*===========================================================================
   This function returns the index of next file in that directory.
===========================================================================*/
static int CMediaPlayer_GetNextFileIndex(CMediaPlayer *pme, int nCurFileIndex)
{
  int       i;

  if (nCurFileIndex < (pme->m_FileExplorer.m_NumFiles - 1))
    nCurFileIndex++;
  else
    nCurFileIndex = 0;

  for (i = 0; i < pme->m_FileExplorer.m_NumFiles; i++)
  {
    if (pme->m_FileExplorer.m_FileArray[nCurFileIndex].m_bIsFile)
      return nCurFileIndex;

    if (nCurFileIndex < (pme->m_FileExplorer.m_NumFiles - 1))
      nCurFileIndex++;
    else
      nCurFileIndex = 0;
  }
  return 0;
}

// this function returns playable file count under current directory
static int CMediaPlayer_GetFileCount(CMediaPlayer *pme)
{
  int i;
  int nCount = 0;
  for (i = 0; i < pme->m_FileExplorer.m_NumFiles; i++)
  {
    if (pme->m_FileExplorer.m_FileArray[i].m_bIsFile)
      nCount++;
  }
  return nCount;
}

/*===========================================================================
   This function fills the file explorer data when a file is selected
   via a BREW event.  Without it, Continuous play won't work via BREW events.
   (because the necessary explorer data will be uninitialized)
===========================================================================*/
static void
CMediaPlayer_FillExplorerData(CMediaPlayer *pme, const char *pszFile)
{
  char      szParentDir[MP_MAX_FILE_NAME];

  (void)MP_GetParentDir(pme, pszFile, szParentDir, MP_MAX_FILE_NAME);
  pme->m_FileExplorer.m_nCurDepth = MP_GetCurrentDepth(szParentDir);
  pme->m_CurFileIndex = MP_GetCurrentIndex(pme, szParentDir, pszFile);
  CMediaPlayer_RefreshExplorer(pme, szParentDir);
}


/*============================================================================
  This function determines whether a given file name exists in the file list.
=============================================================================*/
static boolean
CMediaPlayer_FindFileInFileList(CMediaPlayer *pme, const char *szName)
{
  IFileMgr  *pIFileMgr;
  int nRet;

  if(SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr))
  {
    DBGPRINTF_ERROR("MP: failed to create file manager.");
    return FALSE;
  }
  
  nRet = IFILEMGR_Test(pIFileMgr, szName);

  MP_RELEASEIF(pIFileMgr);

  return (SUCCESS == nRet);
}

/*===========================================================================

                     CMainWin Functions

===========================================================================*/

/*===========================================================================
   This function constucts the main window.
===========================================================================*/
static IWindow *
CMainWin_New(CMediaPlayer *pOwner)
{
  CMainWin *pme;

  VTBL(IWindow)vtbl;

#ifdef _MPDEBUG
  DBGPRINTF_MED("MP: MediaPlayer: creating new main window pOwner:0x%08X)", pOwner);
#endif

  MP_IWINDOW_SETVTBL(&vtbl, CMainWin_Enable, CMainWin_Redraw, CMainWin_HandleEvent, CMainWin_Delete);

  pme = (CMainWin *)CWindow_New(sizeof(CMainWin), pOwner, &vtbl);
  if (!pme)
  {
    return NULL;
  }

  {
    int       cx = pme->m_pOwner->m_cxWidth;
    int       cy = pme->m_pOwner->m_cyHeight;
    int       y,
              dy;
    AEERect   rect;

    pme->m_pOwner->m_State = MP_STATE_MAIN_WIN;
    EMIT_MEDIAPLAYER_STATE(pme->m_pOwner->m_State);
    /*
     * Initialize logo below the header
     */
    pme->m_pLogo = ISHELL_LoadResImage(pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDB_LOGO);
    if (!pme->m_pLogo)
    {
    	DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
      MP_WINERR_RETURN(pme)
    }

    y = 1;
    dy = cy / 2 - y;
    SETAEERECT(&pme->m_rectLogo, 0, y, cx, dy);
    IIMAGE_SetFrameCount(pme->m_pLogo, 2);
    IIMAGE_SetAnimationRate(pme->m_pLogo, 500);

    if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MENUCTL, (void **)&pme->m_pMainMenu))
    {
    	DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
      MP_WINERR_RETURN(pme)
    }

    SETAEERECT(&rect, 0, cy / 2 + 1, cx, cy / 2 - 1);
    MP_SetMenuAttr(pme->m_pMainMenu, AEECLSID_MENUCTL, pme->m_pOwner->m_nColorDepth, &rect, 0);

    (void)IMENUCTL_DeleteAll(pme->m_pMainMenu);

#if defined(MP_FEATURE_MBMS)
    (void)MP_AddMenuItem(pme->m_pMainMenu, IDM_MAIN_PLAYCHANNEL, NULL, IDB_PLAY, IDM_MAIN_PLAYFILE, 0);
#else
    (void)MP_AddMenuItem(pme->m_pMainMenu, IDM_MAIN_PLAYFILE, NULL, IDB_PLAY, IDM_MAIN_PLAYFILE, 0);
#endif

#if defined(FEATURE_MULTISEQUENCER) && !defined(MP_FEATURE_MBMS)
    (void)MP_AddMenuItem(pme->m_pMainMenu, IDM_MAIN_PLAYSEQUENCE, NULL, IDB_PLAY, IDM_MAIN_PLAYSEQUENCE, 0);
#endif

#ifndef MP_FEATURE_MBMS
    (void)MP_AddMenuItem(pme->m_pMainMenu, IDM_MAIN_RECORD, NULL, IDB_RECORD, IDM_MAIN_RECORD, 0);
#endif

#ifdef FEATURE_APP_MPEG4
#if (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)) && !defined(MP_FEATURE_MBMS)
    (void)MP_AddMenuItem(pme->m_pMainMenu, PSEUDO_STREAM, NULL, IDB_PLAY, PSEUDO_STREAM, 0);
    /*
     * MP_AddMenuItem(pme->m_pMainMenu, PSEUDO_FILE, NULL, IDB_PLAY, PSEUDO_FILE, 0);
     */
#endif
#endif
#if defined (MP_FEATURE_VIDEO) && defined (FEATURE_QTV_STREAM_RECORD) && !defined(MP_FEATURE_MBMS)
    (void)MP_AddMenuItem(pme->m_pMainMenu, IDM_MAIN_STREAM_RECORD, NULL, IDB_RECORD, IDM_MAIN_STREAM_RECORD, 0);
#endif
    (void)MP_AddMenuItem(pme->m_pMainMenu, IDM_MAIN_SETTINGS, NULL, IDB_SETTINGS, IDM_MAIN_SETTINGS, 0);
    (void)MP_AddMenuItem(pme->m_pMainMenu, IDM_MAIN_DELETEFILE, NULL, IDB_PLAY, IDM_MAIN_DELETEFILE, 0);

#ifdef FEATURE_MP_HELP_SCREEN
    {
      char      str_chr_Length[12];
      AECHAR    str_achr_Length[12];
      int       nStringLength;

      if (ISHELL_LoadResString
          (pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDM_HELP_DISPLAY_STRING_LENGTH,
           str_achr_Length, 16 * sizeof(AECHAR)))
      {
        (void)WSTRTOSTR(str_achr_Length, str_chr_Length, sizeof(str_chr_Length));
        nStringLength = ATOI(str_chr_Length);
        if (nStringLength)
          (void)MP_AddMenuItem(pme->m_pMainMenu, IDM_HELP, NULL, IDB_HELP, IDM_HELP, 0);
      }

      pme->m_pHelp = NULL;
      pme->m_bHelp = FALSE;
    }
#endif

    (void)MP_AddMenuItem(pme->m_pMainMenu, IDM_MAIN_ABOUT, NULL, IDB_ABOUT, IDM_MAIN_ABOUT, 0);
  }

  return (IWindow *)pme;
}

/*===========================================================================
   This function deletes the main window.
===========================================================================*/
static void
CMainWin_Delete(IWindow *po)
{
  CMainWin *pme = (CMainWin *)po;
  CMediaPlayer *pMP = pme->m_pOwner;

  MP_RELEASEIF(pme->m_pLogo);
#ifdef FEATURE_MP_HELP_SCREEN
  MP_RELEASEIF(pme->m_pHelp);
#endif

  if (pme->m_pMainMenu)
    pme->m_pOwner->m_wMainWin = IMENUCTL_GetSel(pme->m_pMainMenu);
  MP_RELEASEMENU(pMP, pme->m_pMainMenu);

  FREE(pme);

  if(pMP)
  {
    pMP->m_pWin = NULL;
    pMP->m_eActiveWin = MPW_NONE;
  }
}

/*===========================================================================
   This function enables/disables the main window.
===========================================================================*/
static void
CMainWin_Enable(IWindow *po, boolean bEnable)
{
  CMainWin *pme = (CMainWin *)po;

  if (!CWindow_ProcessEnable(po, bEnable))
    return;

  if (!pme->m_bActive)
  {
    IMENUCTL_SetActive(pme->m_pMainMenu, FALSE);
    IIMAGE_Stop(pme->m_pLogo);
    return;
  }

  IMENUCTL_SetActive(pme->m_pMainMenu, TRUE);
  IMENUCTL_SetSel(pme->m_pMainMenu, pme->m_pOwner->m_wMainWin);
  MP_DrawImage(pme->m_pLogo, &pme->m_rectLogo, TRUE);
}

/*===========================================================================
   This function redraws the main window.
===========================================================================*/
static void
CMainWin_Redraw(IWindow *po)
{
  CMainWin *pme = (CMainWin *)po;

  if (!pme->m_bActive)
    return;

  IDISPLAY_ClearScreen(pme->m_pIDisplay);

  MP_DrawImage(pme->m_pLogo, &pme->m_rectLogo, TRUE);
  (void)IMENUCTL_Redraw(pme->m_pMainMenu);

  IDISPLAY_Update(pme->m_pIDisplay);
}

/*===========================================================================
   This function processes events routed to main window.
===========================================================================*/
static boolean
CMainWin_HandleEvent(IWindow *po, AEEEvent eCode, uint16 wParam,
                     uint32 dwParam)
{
  CMainWin *pme = (CMainWin *)po;
  boolean   bRet = TRUE;

#ifdef FEATURE_KEY_HOLD_SCROLL
  int       ret;

  if (MP_ISONEOFKEYEVENT(eCode) && (wParam == AVK_SELECT || wParam == AVK_CLR))
  {
    /*
     * if timer is enabled for auto-scrolling, do nothing here
     */
    if (pme->m_pOwner->m_KeyStore.m_wParam_Key)
      return TRUE;
  }
#endif

  if (eCode == EVT_COPYRIGHT_END && pme->m_bAbout)
  {
    pme->m_bAbout = FALSE;
    CMediaPlayer_Redraw(pme->m_pOwner, TRUE);
    return TRUE;
  }
#ifdef FEATURE_MP_HELP_SCREEN
  else if (MP_ISCLR(eCode) && pme->m_bHelp)
  {
    pme->m_bHelp = FALSE;
    MP_RELEASEIF(pme->m_pHelp);
    pme->m_pHelp = NULL;
    CMediaPlayer_Redraw(pme->m_pOwner, TRUE);
    return TRUE;
  }
  else if (pme->m_bHelp && pme->m_pHelp)
  {
    return ISTATIC_HandleEvent(pme->m_pHelp, eCode, wParam, dwParam);
  }
#endif


#ifdef FEATURE_KEY_HOLD_SCROLL
  ret = MP_HandleKeyEventRepeat(po, eCode, wParam, dwParam, pme->m_pMainMenu,
                             KEY_MENU_REPEAT_TIME_MS);
  if (ret >= 0)
    return ret;
#else
  if (MP_ISEVTKEY(eCode))
    return IMENUCTL_HandleEvent(pme->m_pMainMenu, eCode, wParam, dwParam);
#endif


  if (!MP_ISEVTCMD(eCode))
    return FALSE;

  pme->m_pOwner->m_nPlayerType = wParam;
  switch (wParam)
  {
  case IDM_MAIN_PLAYFILE:
      pme->m_pOwner->m_bDeleteFile = FALSE;

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    pme->m_pOwner->m_bFileWriteReadTest = FALSE;
    pme->m_pOwner->m_bEnablePSSimulation = FALSE;
#endif
    if (pme->m_pOwner->m_nContMode == CONT_MODE_OFF)    // Test Mode
                                                        // Off
      (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, 0);
    else if (pme->m_pOwner->m_nContMode == CONT_MODE_SINGLE_FILE)       // Single
                                                                        // file
                                                                        // Continuously
      (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, 0);
    else if (pme->m_pOwner->m_nContMode == CONT_MODE_ALL_FILES) // All
                                                                // files
                                                                // continuously
      (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, 0);
    /*
     * CMediaPlayer_PlayFile(pme->m_pOwner, pme->m_pOwner->m_FileExplorer.m_FileArray[pme->m_pOwner->m_FileExplorer.m_NumFilesPlayed].m_szName);
     */

    break;

  case IDM_MAIN_DELETEFILE:
      pme->m_pOwner->m_bDeleteFile = TRUE;
      (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, 0);
    break;

#ifdef FEATURE_MULTISEQUENCER
  case IDM_MAIN_PLAYSEQUENCE:
    (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_MULTISEQUENCER, 0);
    break;
#endif     // FEATURE_MULTISEQUENCER

  case IDM_MAIN_RECORD:
    /*
     * This will create voice memo files in order of memo1.qcp, memo2.qcp
     * and so on
     */
#ifdef FEATURE_MP_RECORDER_SETTINGS
  {
    char     *strNewFile =
      CRecorderSettingsWin_GetRecordingFileName(pme->m_pOwner);
    if (!strNewFile)
      return FALSE;

    CMediaPlayer_RecordQCPFile(pme->m_pOwner, strNewFile);
    FREEIF(strNewFile);
  }
#else
  {
    int       suffix1 = 0;
    char     *pszFinalAudioFileName;
    char      dot_char = '.';

    suffix1 =
      MP_GetFileNameSuffix(pme->m_pOwner, "qcp", "memo", MP_MEDIA_DIR, 4);
    pszFinalAudioFileName = MALLOC(MP_MAX_FILE_NAME);

    if (!pszFinalAudioFileName)
    {
      return FALSE;
    }

    SNPRINTF(pszFinalAudioFileName, MP_MAX_FILE_NAME, "%s%d%c%s", "memo", suffix1, dot_char,
            "qcp");


    /*
     * Call for QCP recording window here.
     */
    CMediaPlayer_RecordQCPFile(pme->m_pOwner, pszFinalAudioFileName);
    FREEIF(pszFinalAudioFileName);
  }
#endif
    break;

  case IDM_MAIN_SETTINGS:
    CMediaPlayer_Settings(pme->m_pOwner);
    break;

  case IDM_MAIN_ABOUT:
    CMainWin_About(pme);
    break;
#ifdef FEATURE_MP_HELP_SCREEN
  case IDM_HELP:
    CMainWin_Help(pme);
    break;
#endif

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  case PSEUDO_STREAM:
    pme->m_pOwner->m_bFileWriteReadTest = FALSE;
    pme->m_pOwner->m_bEnablePSSimulation = TRUE;
    (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, 0);
    break;

  case PSEUDO_FILE:
    pme->m_pOwner->m_bFileWriteReadTest = TRUE;
    pme->m_pOwner->m_bEnablePSSimulation = FALSE;
    (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, 0);
    break;
#endif

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  case IDM_MAIN_STREAM_RECORD:
    /*
     * This will record a streaming session.
     */
    (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, (uint32)MPPW_STREAM_RECORD);
    break;
#endif

  default:
    bRet = FALSE;
    break;
  }

  return bRet;
}

/*
 * QTV_PSEUDO_STREAMING_BEGIN
 */

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
static void
CMediaPlayer_KDDIPseudoPlayFile(CMediaPlayer *pme, const char *pszStream)
{
  CPlayerWin *pwin = NULL;
  if (!STRNCMP(pszStream, "URL=", 4))
  {
    AEEMediaData md;
    char     *pszBuf = (char *)MALLOC(PS_MAX_URL_LEN);

    if (!pszBuf)
    {
      return;
    }

    Search_And_Store(pszBuf, pszStream, "URL=", PS_MAX_URL_LEN);
    md.clsData = MMD_BUFFER;
    md.pData = (void *)pszBuf;
    md.dwSize = 0;

    if (!CMediaPlayer_SetWindow(pme, MPW_PLAYER, MPPW_PLAY))
    {
      FREE(pszBuf);
      return;
    }

#ifdef MP_FEATURE_VIDEO
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
    CPlayerWin_ResetFrameTransformations((CPlayerWin *)pme->m_pWin);
#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS ||
           // FEATURE_QTV_MDP_TRANSFORMATIONS
#endif     // MP_FEATURE_VIDEO

    pwin = (CPlayerWin *)pme->m_pWin;
    if ((pme->m_bEnablePSSimulation == TRUE) ||   // Pseudo streaming is turned ON
        (pme->m_bIsHTTP == TRUE))
    {
      if (md.pData != NULL)
      {
        (void)CPlayerWin_SetMediaData((CPlayerWin *)pme->m_pWin, &md);
      }

      if (pwin->psHttpBlock && HttpDownloader_sharedBuffer)
      {
        /*
         * Continuous-Mode streaming
         * HttpDownloader_sharedBuffer is cleaned inside HttpDownloader.c
         * and in CPlayerWin_Delete()
         */
        if (pwin->psHttpBlock->bWebPstreaming)
        {
          PS_CleanHttpDownload(pwin->psHttpBlock);
        }
        FREE(pwin->psHttpBlock);
        pwin->psHttpBlock = NULL;
      }

      pwin->psHttpBlock = (CHttpBlock *)MALLOC(sizeof(CHttpBlock));
      if (pwin->psHttpBlock != NULL)
      {
        pwin->psHttpBlock->psHttpIShell = pme->a.m_pIShell;
        if (http_ps_blockInitialize(pwin->psHttpBlock, pszStream))
        {
          /*
           * if(!pwin->psHttpBlock->totalPSBufSize)
           *      {
           *      Issue the HEAD request :[NOTE= This might not be needed as
           *      size is alwasy computed from the first GET response
           *      http_ps_get_size(pwin->psHttpBlock);
           *      }
           */
          http_ps_get_chunk(&pwin->psHttpBlock);
        }
        else
        {
          if (pwin->psHttpBlock->bWebPstreaming)
          {
            PS_CleanHttpDownload(pwin->psHttpBlock);
          }
          FREE(pwin->psHttpBlock);
          pwin->psHttpBlock = NULL;
        }
      }
    }
  }
  return;
}

/*===========================================================================

   This function is called for
   1)Pseudo streaming a HTTP URL
   2)Starting pseudo stream download simulator

===========================================================================*/
static boolean
CMediaPlayer_PseudoPlayFile(CMediaPlayer *pme, const char *pszFile)
{
  boolean   bReturn = TRUE;
  AEEMediaData md;
  char     *pszBuf;
  boolean   bIsURLfile = FALSE;
  CPlayerWin *pwin = NULL;

#if defined(MP_FEATURE_VIDEO) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
  pme->m_bIsURLFile = FALSE;
#endif
  /*
   * File name is a special case...
   * pszFile will be released by CFileListWin when CPlayerWin opens.
   * Allocate memory for file name here. It will be used and released
   * by CPlayerWin.
   */
  pszBuf = STRDUP(pszFile);
  if (!pszBuf)
    return FALSE;

  DBGPRINTF_HIGH("MP: PseudoPlayFile: %s", pszFile);

  if (IDB_IMAGE_ICON == CMediaPlayer_FindHandlerType(pme, pszFile))
  {
    /*
     * return TRUE, if the file can be shown as a image
     */
    if (CMediaPlayer_PlayImage(pme, pszBuf))
      return TRUE;
  }

  if (HttpDownloader_entireClipInBuffer)
  {
    md.pData = (void *)HttpDownloader_sharedBuffer;
    md.clsData = MMD_BUFFER;
    md.dwSize = HttpDownloader_maxBufferSize;
  }
  else
  {
    md.pData = (void *)pszBuf;
    md.clsData = MMD_BUFFER;
    md.dwSize = 0;
    DBGPRINTF_MED("MP: HttpDownloader_sharedBuffer is NULL.");
  }

  if (!CMediaPlayer_SetWindow(pme, MPW_PLAYER, MPPW_PLAY))
  {
    FREE(pszBuf);
    return FALSE;
  }


#ifdef MP_FEATURE_VIDEO
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
  CPlayerWin_ResetFrameTransformations((CPlayerWin *)pme->m_pWin);
#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS ||
           // FEATURE_QTV_MDP_TRANSFORMATIONS
#endif     // MP_FEATURE_VIDEO

  pwin = (CPlayerWin *)pme->m_pWin;
  DBGPRINTF_MED("MP: HttpDownloader_entireClipInBuffer = %d", HttpDownloader_entireClipInBuffer);

#if defined(MP_FEATURE_VIDEO) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
  if (pme->m_bCallQtvProgDnld)
  {
     char pUrl[PS_MAX_URL_LEN];

     md.pData = pszBuf;
     if (pszBuf)
     {
       DBGPRINTF_MED("MP: PseudoPlayFile: pszBuf=%s", pszBuf);
     }
     else
     {
       DBGPRINTF_HIGH("MP: PseudoPlayFile: pszBuf is NULL!");
     }
     md.clsData = MMD_FILE_NAME;
     md.dwSize = 0;

     if (md.pData != NULL)
      (void)CPlayerWin_SetMediaData((CPlayerWin *)pme->m_pWin, &md);

      if (!STRNICMP(pszFile, "http://", 7))
      {
        (void)STRLCPY(pUrl, pszFile, PS_MAX_URL_LEN);
        bReturn = TRUE;
        pme->m_bIsHTTP = TRUE;
      }
      else
      {
        /* Not a URL, see if a filename that ends in URL and extract the URL from the file. */
        bReturn = PseudoPlayFile_ExtractURLFromFile(pme, pszFile, pUrl, &bIsURLfile);
        pme->m_bIsURLFile = bIsURLfile;
        pme->m_bIsHTTP = bReturn;
      }

      // URL file must be closed so QTV can open it.
      PseudoPlayFile_CleanUp(pme);

      if (bReturn)
      {
        boolean bEventPosted;

        /* pUrl contains the URL to download - call QTV code here by creating IMEDIA object. */
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
          bEventPosted = ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_CREATEMEDIA, 0, 0);
        if (bEventPosted)
        {
          DBGPRINTF_MED("MP: event posted to create media");
          bReturn = TRUE;
        }
        else
        {
          DBGPRINTF_HIGH("MP: event NOT posted to create media");
          bReturn = FALSE;
        }
      }
      else
      {
        DBGPRINTF_ERROR("MP: bReturn is FALSE.");
      }

    return bReturn;
  }
#endif  // MP_FEATURE_VIDEO && (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)

  // If QTV is not performing HTTP download, FCS should not be enabled
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
  if ((pme->m_bEnablePSSimulation == TRUE) ||     // Pseudo streaming is turned ON
      (pme->m_bIsHTTP == TRUE))
  {
    if (md.pData != NULL)
    {
      (void)CPlayerWin_SetMediaData((CPlayerWin *)pme->m_pWin, &md);
    }

    if (HttpDownloader_entireClipInBuffer)
    {
      boolean bEventPosted;

      DBGPRINTF_MED("MP: Entire clip already in buffer, posting event to create media...");
      bEventPosted= ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_CREATEPSEUDOMEDIA, 0, 0);
      if (bEventPosted == TRUE)
      {
        DBGPRINTF_MED("MP: event posted to create pseudo media");
      }
      else
      {
        DBGPRINTF_HIGH("MP: event NOT posted to create pseudo media");
      }
      return bEventPosted;
    }

    if (pwin->psHttpBlock && HttpDownloader_sharedBuffer)
    {
      /*
       * Continuous-Mode streaming
       * HttpDownloader_sharedBuffer is cleaned inside HttpDownloader.c and in CPlayerWin_Delete()
       */
      if (pwin->psHttpBlock->bWebPstreaming)
      {
        PS_CleanHttpDownload(pwin->psHttpBlock);
      }
      FREE(pwin->psHttpBlock);
      pwin->psHttpBlock = NULL;
    }
    else
      DBGPRINTF_HIGH("MP: psHttpBlock and HttpDownloader_sharedBuffer are NULL.");

    if (!STRNICMP(pszFile, "http://", 7))
    {
      bReturn = PseudoPlayFile_ProcessURL(pme, pszFile);
      pme->m_bIsHTTP = TRUE;
    }
    else
    {
      bReturn = PseudoPlayFile_ProcessFile(pme, pszFile, &bIsURLfile);
#if defined(MP_FEATURE_VIDEO) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
      pme->m_bIsURLFile = bIsURLfile;
#endif
      pme->m_bIsHTTP = bReturn;
      /*
       * Kick the pseudo streaming simulator
       */
      if (bReturn && !bIsURLfile)
      {
        bReturn = PseudoPlayFile_StartStream(pme, pszBuf);
        DBGPRINTF_HIGH("MP: PseudoPlayFile_StartStream() return %d.", bReturn);
      }
    }                                  // if(!STRNICMP(pszFile,"http://",7))
  }                                    // if(pme->m_bEnablePSSimulation ==
                                       // TRUE)
  else if (pme->m_bFileWriteReadTest == TRUE)
  {
    bReturn = PseudoPlayFile_StartTest(pme, pszBuf);
  }

  /*
   * cleanup will be called in PSTimerResponse()
   * when PseudoPlayFile_StartTest()or
   * PseudoPlayFile_StartStream()succeeds
   */
  if (!bReturn || bIsURLfile)
  {
    PseudoPlayFile_CleanUp(pme);
  }
  /*
   * pszBuf will be freed in CPlayerWin_Delete()
   */

  return bReturn;
}


static void
PseudoPlayFile_CleanUp(CMediaPlayer *pme)
{
  /*
   * If a buffer was allocated to read the actual file, free it here.
   */
  if (pme->m_downloadFileBuffer)
  {
    FREE(pme->m_downloadFileBuffer);
    pme->m_downloadFileBuffer = NULL;
  }

  /*
   * Free any file resources that were used.
   */
  if (pme->m_pPseudoStrIFile != NULL)
  {
    (void)IFILE_Release(pme->m_pPseudoStrIFile);
    pme->m_pPseudoStrIFile = NULL;
  }

  MP_RELEASEIF(pme->m_pPSIFileMgr);
  pme->m_pPSIFileMgr = NULL;

  MP_RELEASEIF(pme->m_pIFileMgr2);
  pme->m_pIFileMgr2 = NULL;
}


/*===========================================================================
   These PseudoPlayFile_XYZ()functions are helpers to
   CMediaPlayer_PseudoPlayFile()
   They all return FALSE if an error is encountered and TRUE if no errors
   occur.
===========================================================================*/
static boolean
PseudoPlayFile_ProcessURL(CMediaPlayer *pme, const char *pszFile)
{
  CPlayerWin *pwin = (CPlayerWin *)pme->m_pWin;
  DBGPRINTF_MED("MP: PseudoPlayFile_ProcessURL for %s", pszFile);
  pwin->psHttpBlock = (CHttpBlock *)MALLOC(sizeof(CHttpBlock));
  if (pwin->psHttpBlock != NULL)
  {
    pwin->psHttpBlock->psHttpIShell = pme->a.m_pIShell;

    if (http_ps_blockInitialize(pwin->psHttpBlock, NULL))
    {
      (void)STRLCPY(pwin->psHttpBlock->m_pUrl, pszFile, PS_MAX_URL_LEN);
      http_ps_get_chunk(&pwin->psHttpBlock);
    }
    else
    {
      DBGPRINTF_HIGH("MP: http_ps_blockInitialize() returns FALSE");
      FREE(pwin->psHttpBlock);
      pwin->psHttpBlock = NULL;
    }
  }

  return TRUE;
}

static boolean PseudoPlayFile_ExtractURLFromFile(CMediaPlayer *pme, const char *pszFile,
                                                 char *pUrl, boolean *bIsURLfile)
{
  char           *fiName, *extName;
  unsigned int   size = 0;
  AEEFileInfoEx  fi;

  *bIsURLfile = FALSE;

  if (pszFile != NULL)
  {
    DBGPRINTF_MED("MP: PseudoPlayFile_ExtractURLFromFile() pszFile=%s", pszFile);
  }
  else
  {
    DBGPRINTF_HIGH("MP: PseudoPlayFile_ExtractURLFromFile() pszFile = NULL");
    return FALSE;
  }

  /* Get the filename. */
  fiName = MP_GetFileName(pszFile);
  if (fiName == NULL)
  {
    DBGPRINTF_HIGH("MP: PseudoPlayFile_ExtractURLFromFile() fiName = NULL");
    return FALSE;
  }

  /* Get the filename extension. */
  extName = MP_GetFileNameExtension(fiName);
  if (extName == NULL)
  {
    DBGPRINTF_HIGH("MP: PseudoPlayFile_ExtractURLFromFile() extName = NULL");
    return FALSE;
  }
  DBGPRINTF_MED("MP: PseudoPlayFile_ExtractURLFromFile() fiName=%s,extName=%s", fiName, extName);

  /* Make sure this is a "URL" file. */
  if (!STRICMP(extName, "url"))
  {
    *bIsURLfile = TRUE;
    (void)STRLCPY(pUrl, "", PS_MAX_URL_LEN);
    if(SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pme->m_pPSIFileMgr))
    {
      DBGPRINTF_ERROR("MP: failed to create file manager.");
      return FALSE;
    }

    if ((pme->m_pPseudoStrIFile = IFILEMGR_OpenFile(pme->m_pPSIFileMgr, pszFile, _OFM_READ)) == NULL)
    {
      DBGPRINTF_ERROR("MP: error on opening file.");
      return FALSE;
    }

    (void)MEMSET(&fi,0,sizeof(fi));
    fi.nStructSize = sizeof(fi);
    if (IFILEMGR_GetInfoEx(pme->m_pPSIFileMgr, pszFile, &fi) == SUCCESS)
    {
      int       rSize = 0;

      size = fi.dwSize;
      if (size <= PS_MAX_URL_LEN)
      {
        rSize = IFILE_Read(pme->m_pPseudoStrIFile, pUrl, size);
        pUrl[rSize] = '\0';
      }

      /*
       * case insensitive search for string "http"
       */
      if (STRNICMP(pUrl, "http://", 7) != 0)
      {                                // http was not found?
        DBGPRINTF_ERROR("MP: http prefix not found");
        return FALSE;
      }

      if (rSize != size)
      {
        (void)STRLCPY(pUrl, "", PS_MAX_URL_LEN);
        DBGPRINTF_ERROR("MP: rSize(%d) != size(%d)", rSize, size);
        return FALSE;
      }
    }
    else
    {
      DBGPRINTF_ERROR("MP: IFILEMGR_GetInfoEx() != SUCCESS");
      return FALSE;
    }
  }
  else
  {
    *bIsURLfile = FALSE;
    DBGPRINTF_ERROR("MP:  File does not end in URL!");
    return FALSE;
  }

  /* URL successfully extracted info pUrl. */
  DBGPRINTF_HIGH("MP: PseudoPlayFile_ExtractURLFromFile() returning URL = %s", pUrl);
  return TRUE;
}


static boolean PseudoPlayFile_ProcessFile(CMediaPlayer *pme, const char *pszFile,
                                          boolean *bIsURLfile)
{
  char      pUrl[PS_MAX_URL_LEN];
  CPlayerWin *pwin = (CPlayerWin *)pme->m_pWin;

  if (PseudoPlayFile_ExtractURLFromFile(pme, pszFile, pUrl, bIsURLfile))
  {
    pwin->psHttpBlock = (CHttpBlock *)MALLOC(sizeof(CHttpBlock));
    if (pwin->psHttpBlock != NULL)
    {
      pwin->psHttpBlock->psHttpIShell = pme->a.m_pIShell;
      if (http_ps_blockInitialize(pwin->psHttpBlock, NULL))
      {
        (void)STRLCPY(pwin->psHttpBlock->m_pUrl, pUrl, PS_MAX_URL_LEN);
        http_ps_get_chunk(&pwin->psHttpBlock);
      }
      else
      {
        DBGPRINTF_ERROR("MP: http_ps_blockInitialize() returns FALSE");
        FREE(pwin->psHttpBlock);
        pwin->psHttpBlock = NULL;
      }
    }

    /*
     * End of handling a .url file for pseudo streaming
     */
    DBGPRINTF_MED("MP: End of handling a .url file for pseudo streaming");
    return TRUE;
  }
  else
  {
    if(FALSE == *bIsURLfile)
    {
      DBGPRINTF_MED("MP: pseudo streaming with local file.");
      return TRUE;
    }
  }
  return FALSE;
}


static boolean PseudoPlayFile_StartStream(CMediaPlayer *pme, char *pszBuf)
{
  AEEFileInfoEx  fi;
  CPlayerWin *pwin;

  if (pme == NULL)
  {
    DBGPRINTF_HIGH("MP: PseudoPlayFile_StartStream given NULL CMediaPlayer pointer!");
    return FALSE;
  }
  DBGPRINTF_MED("MP: PseudoPlayFile_StartStream()");
  pwin = (CPlayerWin *)pme->m_pWin;

  if(SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR,(void **)&pme->m_pPSIFileMgr))
  {
    DBGPRINTF_ERROR("MP: failed to create file manager.");
    return FALSE;
  }

  if ((pme->m_pPseudoStrIFile = IFILEMGR_OpenFile(pme->m_pPSIFileMgr, pszBuf, _OFM_READ)) == NULL)
  {
    /*
     * Failed to open the input file
     */
    DBGPRINTF_ERROR("MP: failed to open the input file ");
    return FALSE;
  }

  (void)MEMSET(&fi,0,sizeof(fi));
  fi.nStructSize = sizeof(fi);
  if (IFILEMGR_GetInfoEx(pme->m_pPSIFileMgr, pszBuf, &fi) == SUCCESS)
  {
    /*
     * Obtain information about file size
     */
    int       size = fi.dwSize;

    pme->m_nDownloadPendingSize = size;
    pme->m_nDownloadCopiedSize = 0;

    /*
     * Compute the chunk size
     */
    pme->m_nDownloadCopyBytes = MIN(pme->m_nChunkSize, pme->m_nDownloadPendingSize);

    /*
     * Allocate memory for pseudo streaming shared buffer
     */
    if (HttpDownloader_sharedBuffer != NULL)
    {
      FREE(HttpDownloader_sharedBuffer);
      HttpDownloader_sharedBuffer = NULL;
      HttpDownloader_entireClipInBuffer = FALSE;
    }
    HttpDownloader_sharedBuffer = MALLOC(size);

    if (HttpDownloader_sharedBuffer != NULL)
    {
      (void)IFILE_Read(pme->m_pPseudoStrIFile, HttpDownloader_sharedBuffer, pme->m_nDownloadCopyBytes);
      /*
       * MEMCPY(HttpDownloader_sharedBuffer,pme->m_downloadFileBuffer,pme->m_nDownloadCopyBytes);
       */
      pwin->m_md.pData = HttpDownloader_sharedBuffer;
      pwin->m_md.dwSize = size;
      pwin->m_md.clsData = HTTPDownloadInterface ;
      pme->m_nDownloadCopiedSize = pme->m_nDownloadCopyBytes;
      pme->m_nDownloadPendingSize = size - pme->m_nDownloadCopyBytes;
      PseudoStreamSetTimer(pme, 1);
    }
    else
    {                                  // Failed to allocate memory for
                                       // shared buffer
      /*
       * DBGPRINTF()sends a message to diag port.
       * Legacy HIGH messages should be turned on for this
       * message to appear on terminal (QXDM)connected to
       * receive data from diag port.
       */
      DBGPRINTF_ERROR("MP: Not enough memory for simulating pseudo streaming");
      return FALSE;
    }
  }
  else                                 // Failed to obtain information
                                       // about file size
  {
    /*
     * DBGPRINTF()sends a message to diag port.
     * Legacy HIGH messages should be turned on for this
     * message to appear on terminal (QXDM)connected to
     * receive data from diag port.
     */
    DBGPRINTF_ERROR("MP: failed to obtain file info");
    return FALSE;
  }

  /*
   * Post an event to self
   */
  (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_CREATEPSEUDOMEDIA, 0, 0);
  DBGPRINTF_MED("MP: PseudoPlayFile_StartStream() returns TRUE");
  return TRUE;
}


static boolean PseudoPlayFile_StartTest(CMediaPlayer *pme, char *pszBuf)
{
  AEEFileInfoEx  fi;
  int            size = 0;
  CPlayerWin *pwin;

  if (pme == NULL)
  {
    DBGPRINTF_HIGH("MP: PseudoPlayFile_StartStream given NULL CMediaPlayer pointer!");
    return FALSE;
  }
  DBGPRINTF_HIGH("MP: PseudoPlayFile_StartTest()");
  pwin = (CPlayerWin *)pme->m_pWin;

  if(SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pme->m_pPSIFileMgr))
  {
    DBGPRINTF_ERROR("MP: failed to create file manager.");
    return FALSE;
  }

  if ((pme->m_pPseudoStrIFile = IFILEMGR_OpenFile(pme->m_pPSIFileMgr, pszBuf, _OFM_READ)) == NULL)
  {
    DBGPRINTF_ERROR("MP: failed to open the input file ");
    return FALSE;
  }

  (void)MEMSET(&fi,0,sizeof(fi));
  fi.nStructSize = sizeof(fi);
  if (IFILEMGR_GetInfoEx(pme->m_pPSIFileMgr, pszBuf, &fi) == SUCCESS)
  {
    size = fi.dwSize;
    pme->m_nDownloadPendingSize = size;
    pme->m_nDownloadCopiedSize = 0;
    pme->m_nDownloadCopyBytes = MIN(pme->m_nChunkSize, pme->m_nDownloadPendingSize);

    if ((pme->m_downloadFileBuffer = MALLOC(size)) != NULL)
    {
      if (IFILE_Read(pme->m_pPseudoStrIFile, pme->m_downloadFileBuffer, size) == size)
      {
        if (HttpDownloader_sharedBuffer != NULL)
        {
          FREE(HttpDownloader_sharedBuffer);
          HttpDownloader_sharedBuffer = NULL;
          HttpDownloader_entireClipInBuffer = FALSE;
        }

        HttpDownloader_sharedBuffer = MALLOC(size);
        if (!HttpDownloader_sharedBuffer)
        {
          DBGPRINTF_ERROR("MP: cannot allocate memory.");
          return FALSE;
        }

        if(SUCCESS == ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pme->m_pIFileMgr2))
        {
          (void)IFILE_Release(pme->m_pPseudoStrIFile);
          if ((pme->m_pPseudoStrIFile = IFILEMGR_OpenFile(pme->m_pIFileMgr2, "media/test.mp4",
                                                 _OFM_CREATE)) != NULL)
          {
            (void)IFILE_Write(pme->m_pPseudoStrIFile, pme->m_downloadFileBuffer, pme->m_nDownloadCopyBytes);
            pwin->m_md.pData = (void *)"media/test.mp4";
            pwin->m_md.dwSize = 0;
            pwin->m_md.clsData = MMD_FILE_NAME;
            pme->m_nDownloadCopiedSize += pme->m_nDownloadCopyBytes;
            pme->m_nDownloadPendingSize -= pme->m_nDownloadCopyBytes;
            PseudoStreamSetTimer(pme, 1);
          }

          (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_CREATEMEDIA, 0, 0);
          if (pme->m_bPlugin)
          {
            DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
            (void)ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
          }
          DBGPRINTF_MED("MP: PseudoPlayFile_StartTest() returns TRUE");
          return TRUE;
        }
        else
        {
          DBGPRINTF_ERROR("MP: failed on creating file mgr for reading file.");
        }
      }
    }
  }
  else
    DBGPRINTF_ERROR("MP: IFILEMGR_GetInfoEx failed.");

  DBGPRINTF_HIGH("MP: PseudoPlayFile_StartTest() returns FALSE");
  return FALSE;
}


static void
PseudoStreamSetTimer(CMediaPlayer *pme, unsigned int timeInterval)
{
  if (pme == NULL)
  {
    DBGPRINTF_HIGH("MP: PseudoStreamSetTimer passed NULL CMediaPlayer pointer!");
    return;
  }

  if (pme->m_nDownloadPendingSize == 0)
  {
    pme->m_bDownloadNotDone = FALSE;
  }
  else
  {
    pme->m_bDownloadNotDone = TRUE;
  }

  if (pme->m_bDownloadNotDone)
  {
    (void)ISHELL_SetTimer(pme->a.m_pIShell, 1000, PSTimerResponse, (void *)pme);
  }
}

static void
PSTimerResponse(void *pUser)
{
  int       nRet = 0;
  static unsigned int cnt = 0;
  CMediaPlayer *pme;
  CPlayerWin *pwin;

  if (pUser == NULL)
  {
    DBGPRINTF_HIGH("MP: PSTimerResponse given NULL argument!");
    return;
  }
  pme = (CMediaPlayer *)pUser;
  pwin = (CPlayerWin *)pme->m_pWin;
 /*
   * write some data to PS buffer. If there is more to write set the
   * timer again
   */

  pme->m_nDownloadCopyBytes = MIN(pme->m_nChunkSize, pme->m_nDownloadPendingSize);

  if (pme->m_bEnablePSSimulation == TRUE)
  {
    (void)IFILE_Read(pme->m_pPseudoStrIFile, HttpDownloader_sharedBuffer + pme->m_nDownloadCopiedSize, pme->m_nDownloadCopyBytes);
  }
  else if (pme->m_bFileWriteReadTest == TRUE)
  {
    (void)IFILE_Write(pme->m_pPseudoStrIFile, pme->m_downloadFileBuffer + pme->m_nDownloadCopiedSize, pme->m_nDownloadCopyBytes);
  }

  pme->m_nDownloadCopiedSize += pme->m_nDownloadCopyBytes;

  if (pme->m_nDownloadPendingSize <= pme->m_nDownloadCopyBytes)
  {
    pme->m_nDownloadPendingSize = 0;
  }
  else
  {
    pme->m_nDownloadPendingSize -= pme->m_nDownloadCopyBytes;
  }
  pwin->m_bufferChunk.dwSize = pme->m_nDownloadCopyBytes;
  pwin->m_bufferChunk.pData  = NULL; /* Ignore this field */

  DBGPRINTF_MED("MP: PSTimerResponse chunk %d, copied %d pending %d",
      pme->m_nDownloadCopyBytes, pme->m_nDownloadCopiedSize, pme->m_nDownloadPendingSize);

  if (NULL != pwin->m_pMedia)
  {
    nRet = IMEDIA_SetPseudoStreamMediaData(pwin->m_pMedia, &pwin->m_bufferChunk);
    DBGPRINTF_MED("MP: IMEDIA_SetPseudoStreamMediaData with buffer size of %d, returns %d = %s",
                  pwin->m_bufferChunk.dwSize, nRet, MP_ErrorStateToString(nRet));

    if (pme->m_nDownloadPendingSize == 0)
      pme->m_bDownloadNotDone = FALSE;
  }
  else
  {
    pme->m_bDownloadNotDone = FALSE;
  }

  if (pme->m_bDownloadNotDone)               // There is more data to copy!
  {
    ++cnt;
    if (!pme->m_bVariableRate)
    {
      (void)ISHELL_SetTimer(pme->a.m_pIShell, 1000, PSTimerResponse, (void *)pme);
    }
    else if ((cnt % 3) == 0)
    {
      (void)ISHELL_SetTimer(pme->a.m_pIShell, 4000, PSTimerResponse, (void *)pme);
      pme->m_nChunkSize = KBPS_10;
    }
    else
    {
      (void)ISHELL_SetTimer(pme->a.m_pIShell, 8000, PSTimerResponse, (void *)pme);
      pme->m_nChunkSize = KBPS_256;
    }

  }
  else
  {
    (void)ISHELL_CancelTimer(pme->a.m_pIShell, PSTimerResponse, (void *)pme);
    PseudoPlayFile_CleanUp(pme);
  }
}

#endif     // FEATURE_APP_MPEG4 && // FEATURE_QTV_PSEUDO_STREAM || // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

/*===========================================================================
   This function displays the About dialog of the app.
===========================================================================*/
static void
CMainWin_About(CMainWin *pme)
{
  CMediaPlayer_DisableWin(pme->m_pOwner);

  pme->m_bAbout = TRUE;

  IDISPLAY_ClearScreen(pme->m_pIDisplay);
  (void)ISHELL_ShowCopyright(pme->m_pIShell);
}

#ifdef FEATURE_MP_HELP_SCREEN

/*===========================================================================
   This function displays the Help Screen.
   please note. the texts on the screen are from resource file.
   id = IDM_HELP_DISPLAY_STRING
===========================================================================*/
static void
CMainWin_Help(CMainWin *pme)
{
  AECHAR   *pBuf;
  AEERect   rc;
  uint32    charHight;
  char      str_chr_Length[12];
  AECHAR    str_achr_Length[12];
  int       nStringLength;

  pme->m_pOwner->m_State = MP_STATE_HELP_WIN;
  EMIT_MEDIAPLAYER_STATE(pme->m_pOwner->m_State);

  if (ISHELL_LoadResString
      (pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDM_HELP_DISPLAY_STRING_LENGTH,
       str_achr_Length, 16 * sizeof(AECHAR)))
  {
    (void)WSTRTOSTR(str_achr_Length, str_chr_Length, sizeof(str_chr_Length));
    nStringLength = ATOI(str_chr_Length);
    if (!nStringLength)
      return;
  }
  else
    return;

  /*
   * create static text ctrl
   */
  if (ISHELL_CreateInstance
      (pme->m_pIShell, AEECLSID_STATIC, (void **)&pme->m_pHelp) != SUCCESS)
    return;
  pBuf = MALLOC((nStringLength + 2) * sizeof(AECHAR));
  if (pBuf == NULL)
  {
    MP_RELEASEIF(pme->m_pHelp);
    return;
  }

  if (ISHELL_LoadResString
      (pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDM_HELP_DISPLAY_STRING, pBuf,
       MAX_HELP_STRING_LENGTH *sizeof(AECHAR)))
  {
    charHight =
      IDISPLAY_GetFontMetrics(pme->m_pOwner->a.m_pIDisplay, AEE_FONT_NORMAL,
                              NULL, NULL);
    DBGPRINTF_MED("MP: IDISPLAY_GetFontMetrics returns %d", charHight);
    rc.x = 1;
    rc.y = 1;
    rc.dx = pme->m_pOwner->m_cxWidth - 1;
    rc.dy = (pme->m_pOwner->m_cyHeight - rc.y) - 1;

    /*
     * set size and location of ctrl
     */
    ISTATIC_SetRect(pme->m_pHelp, &rc);
    /*
     * clear old screen texts
     */
    CMediaPlayer_DisableWin(pme->m_pOwner);
    IDISPLAY_ClearScreen(pme->m_pIDisplay);

    pme->m_bHelp = TRUE;
    /*
     * set bk color
     */
    IDISPLAY_FillRect(pme->m_pOwner->a.m_pIDisplay, &rc, MENU8_BACKGROUND);
    (void)IDISPLAY_SetColor(pme->m_pOwner->a.m_pIDisplay, CLR_USER_BACKGROUND,
                            MENU8_BACKGROUND);

    /*
     * replace '\' and 'n' in resource string with '\n'
     */
    pBuf = FixResourceString(pBuf);

    (void)ISTATIC_SetText(pme->m_pHelp, NULL, pBuf, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
    (void)ISTATIC_Redraw(pme->m_pHelp);
  }

  FREE(pBuf);
}

/*===========================================================================
   For given input string, replace '\' and 'n' into '\n'
===========================================================================*/
static AECHAR *
FixResourceString(AECHAR *str)
{
  char      chRT[6];
  AECHAR    achRT[6];
  AECHAR   *pRead;
  AECHAR   *pWrite;
  int       i;
  int       nLength;

  chRT[0] = '\\';
  chRT[1] = 'n';
  chRT[2] = '\n';
  chRT[3] = '\0';
  (void)STR_TO_WSTR(chRT, achRT, sizeof(achRT));

  pWrite = str;
  pRead = str;
  nLength = WSTRLEN(str);
  for (i = 0; i < nLength; i++)
  {
    if (*pRead == achRT[0] && *(pRead + 1) == achRT[1])
    {
      *pWrite = achRT[2];
      pRead++;
    }
    else
    {
      *pWrite = *pRead;
    }
    pRead++;
    pWrite++;
  }
  *pWrite = achRT[3];
  return str;
}
#endif


/*===========================================================================

                     CPlayerWin Functions

===========================================================================*/

/*===========================================================================
   This function constructs player window.
   Player window type can be
   (1)MPPW_PLAY: Plays media files
   (2)MPPW_RECORD: Records QCP file
   (3)MPPW_IMAGE: Displays image (of IImage type)
   (4)MPPW_STREAM_RECORD: Records a streaming session
===========================================================================*/
static IWindow *
CPlayerWin_New(CMediaPlayer *pOwner, MPPlayerWin eWin)
{
  CPlayerWin *pme;

  VTBL(IWindow)vtbl;

  DBGPRINTF_MED("MP: start creating player win %d.", (int) eWin);
  mp_ignoreNotices = FALSE;

  MP_IWINDOW_SETVTBL(&vtbl, CPlayerWin_Enable, CPlayerWin_Redraw,
                     CPlayerWin_HandleEvent, CPlayerWin_Delete);
  pme = (CPlayerWin *)CWindow_New(sizeof(CPlayerWin), pOwner, &vtbl);
  if (!pme)
    return NULL;

  switch (eWin)
  {
  case MPPW_PLAY:
    pme->m_pOwner->m_State = MP_STATE_PLAYER_WIN;
    break;
  case MPPW_RECORD:
    pme->m_pOwner->m_State = MP_STATE_RECORDER_WIN;
    break;
  case MPPW_IMAGE:
    pme->m_pOwner->m_State = MP_STATE_IMAGE_WIN;
    break;
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  case MPPW_STREAM_RECORD:
    pme->m_pOwner->m_State = MP_STATE_STREAM_RECORD_WIN;
    break;
#endif
  default:
    pme->m_pOwner->m_State = MP_STATE_UNKNOWN;
    break;
  }

  EMIT_MEDIAPLAYER_STATE(pme->m_pOwner->m_State);
  pme->m_pOwner->m_playState = MP_PLAYSTATE_STOPPED;
  pme->m_pOwner->m_ErrorState = MP_ERRORSTATE_NONE;
  EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);

  pme->m_pMedia1 = NULL;

  pme->m_CB.m_pFnPtr = NULL;
  pme->m_CB.m_nCBCount = 0;
  pme->m_CB.m_nCmd = -1;
  pme->m_CB.m_nSubCmd = 0;

  pme->m_pAudioSpec = NULL;
  pme->m_pInfoTextCtl = NULL;

  DBGPRINTF_MED("MP: Setting m_eWin to %d", eWin);
  pme->m_eWin = eWin;
  pme->m_dwDuration = 0;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2                                       
  pme->m_dwDownldPos = 0;
#ifdef MM_STATUS_DOWNLOAD_PERCENTAGE
  pme->m_dwDownldPct = 0;
#endif //MM_STATUS_DOWNLOAD_PERCENTAGE
#endif //FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

  pme->m_pOwner->m_bVideoPlaybackReady = FALSE;

  pOwner->m_bResumePending = FALSE;
  pme->m_nWaitTimerCnt = 0;
  pme->m_MediaBusy = MEDIA_NOT_BUSY;
  pme->m_bStoppedWhileConnecting = FALSE;

#ifdef FEATURE_QTV_GENERIC_BCAST

  FREEIF(pme->m_pOwner->m_pTrackList);
  pme->m_pOwner->m_nTrackCount = 0;

#endif //#ifdef FEATURE_QTV_GENERIC_BCAST

  CPlayerWin_SetScreenRects(pme);

  /*
   * Create IStatic for file name
   */
  if (SUCCESS != ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_STATIC, (void **)&pme->m_pTitle))
  {
    DBGPRINTF_HIGH("MP: m_pTitle Creation Failed");
    MP_WINERR_RETURN(pme)
  }
  ISTATIC_SetRect(pme->m_pTitle, &pme->m_rectTitle);
  ISTATIC_SetProperties(pme->m_pTitle, ST_CENTERTEXT | ST_NOSCROLL);

  /*
   * Create Menu control for soft keys
   */
  if (SUCCESS != ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pme->m_pPlayerMenu))
  {
    DBGPRINTF_HIGH("MP: m_pPlayerMenu Creation Failed");
    MP_WINERR_RETURN(pme)
  }
  MP_SetMenuAttr(pme->m_pPlayerMenu, AEECLSID_ICONVIEWCTL,
                 pme->m_pOwner->m_nColorDepth, &pme->m_rectMenu, MP_MAXSOFTKEYITEMS);

#ifdef FEATURE_MMOVERLAY
  pme->m_pIMMOverlayMgr = NULL;
  pme->m_pIMMOverlayTitle = NULL;
  pme->m_pIMMOverlayInfo = NULL;
  pme->m_pIMMOverlayMenu = NULL;
  pme->m_pIMMOverlaySetups = NULL;
  pme->m_pIMMOverlayImage = NULL;
  pme->m_pIMMOverlayLayer1 = NULL;
  pme->m_pIMMOverlayLayer2 = NULL;
  pme->m_pLayer1Bitmap = NULL;
  pme->m_pLayer2Bitmap = NULL;

  if (pme->m_pOwner->m_bOverlays)
  {
    DBGPRINTF_MED("MP: CPlayerWin_New: MMOVERLAY enabled");
    //Create instance for pme->m_pIOverlayMgr and pme->m_pIMMOverlayMgr
    if (SUCCESS == ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MMOVERLAYMGR,(void **)&pme->m_pIMMOverlayMgr))
    {
      if (!CPlayerWin_RegisterOvScreen(pme))
      {
        DBGPRINTF_HIGH("MP: Failed to register overlays");
        MP_WINERR_RETURN(pme)
      }
    }
    else
    {
      DBGPRINTF_HIGH("MP: m_pIMMOverlayMgr Creation Failed");
      MP_WINERR_RETURN(pme)
    }
  }
#endif // FEATURE_MMOVERLAY

  if (CPlayerWin_IsImage(pme))
  {
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FULLSCRN, IDM_PM_FULLSCRN, 0);
  }
  else if (CPlayerWin_IsMedia(pme))
  {
    /*
     * Initialize logo below the header
     */
    if (!CMediaPlayer_SetIdleImage(pme, IDB_LOGO))
    {
      DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
      MP_WINERR_RETURN(pme)
    }

    /*
     * Initialize ProgCtl
     */
#ifdef FEATURE_MMOVERLAY
    pme->m_pMPProgCtl = MPProgCtl_Create(pme->m_pIShell, pme->m_pIDisplay, (void *)pme->m_pIMMOverlayInfo, pme->m_rectInfo);
#else
    pme->m_pMPProgCtl = MPProgCtl_Create(pme->m_pIShell, pme->m_pIDisplay, NULL, pme->m_rectInfo);
#endif // FEATURE_MMOVERLAY

    if (pme->m_pMPProgCtl == NULL)
    {
      DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
      MP_WINERR_RETURN(pme)
    }

    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_PLAY, IDM_PM_PLAY, 0);

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
    if (pme->m_eWin == MPPW_STREAM_RECORD)
    {
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_STOP, 
        IDM_PM_STOP_STREAM_RECORD, 0);
    }
    else
#endif
    {
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_STOP, IDM_PM_STOP, 0);
    }
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_REWIND, IDM_PM_REWIND, 0);

    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_PAUSE, IDM_PM_PAUSE, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FF, IDM_PM_FF, 0);
    
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
    if (pme->m_eWin == MPPW_STREAM_RECORD)
    {
      if(!CPlayerWin_PurgeDownloadedFile(pme))
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_RECORD, IDM_PM_START_STREAM_RECORD, 0);
    }
#endif
    if (pme->m_eWin == MPPW_RECORD)
    {
      if(!CPlayerWin_PurgeDownloadedFile(pme))
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_RECORD, IDM_PM_RECORD, 0);
    }
//#ifdef MP_FEATURE_VIDEO
    else if (pme->m_eWin != MPPW_STREAM_RECORD)
    {
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_INFO, IDM_PM_INFO, 0);
    }
//#endif     // MP_FEATURE_VIDEO

    if (pme->m_eWin != MPPW_RECORD)
    {
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_BK_FILE, IDM_PM_BK_FILE, 0);
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FF_FILE, IDM_PM_FF_FILE, 0);
    }

#ifdef MP_FEATURE_FAST_PLAYBACK
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FAST_REPLAY, IDM_PM_FAST_PLAYBACK, 0);
#endif //MP_FEATURE_FAST_PLAYBACK

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_DUAL_MONO_CHANNEL, IDS_AUDIO_CHANNEL, 0);
#endif //FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION

    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_UP, IDM_PM_UP, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_DOWN, IDM_PM_DOWN, 0);

#ifdef FEATURE_MP_RECORDER_SETTINGS
    if (pme->m_eWin == MPPW_RECORD)
    {
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_SETTINGS, 
        IDM_PM_RECORDER_SETTINGS, 0);
    }
#endif

#ifdef FEATURE_MP_EQ_UI
    //no support for continue mode now
    if(pme->m_eWin == MPPW_PLAY && pme->m_pOwner->m_bShowPlayerWinSettingUI)
    {
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_SETTINGS, 
                                 IDM_PM_PLAYERWIN_SETTINGS, 0);
    }
#endif //FEATURE_MP_EQ_UI
  }

  pme->m_seekPending = pme->m_pendingSeekOffset = pme->m_reseek = 0;
  pme->m_nextClipOffsetAdj = 0;
  pme->m_startSeekPosition = 0;
  pme->m_skipSuccess = FALSE;
  pme->m_skipPending = FALSE;
  pme->m_skipOffset = 0;
  pme->m_reskip = FALSE;
  pme->m_currentClipIndex = 0;
  pme->m_bMemoryFull = FALSE;
  // reset error display flag
  pme->m_bAbortStringActive = 0;

  pme->m_bIsCMXFile = TRUE;
  pme->m_bResumeInProcessing = FALSE;

  pme->m_pOwner->m_bResumeInProcessing = FALSE;
  pme->m_pOwner->m_bSuspendPending = FALSE;  
  pme->m_pOwner->m_bPlayNextFileOnResume = FALSE;
  pme->m_pOwner->m_bReplaySameFile = FALSE;

  pme->m_bAutoPlayIsFirstClip = pme->m_pOwner->m_bAutoPlay;

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
  pme->m_dwZoomScale = MM_MPEG4_NO_SCALING;    
  pme->m_Rotation = MM_MPEG4_NO_ROTATION;
#endif // FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_MDP_TRANSFORMATIONS
#endif //FEATURE_APP_MPEG4

#ifdef MP_FEATURE_FAST_PLAYBACK
  pme->m_ReplaySpeed = QTV_PLAYBACK_SPEED_NORMAL;
  pme->m_bChangeReplaySpeed = FALSE;
#endif //#ifdef MP_FEATURE_FAST_PLAYBACK

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  pme->m_DualMonoOutput = DUAL_MONO_MAIN_TO_L_AND_R;
#endif //#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION

  pme->m_dwPlayPos = 0;
#ifdef FEATURE_MP_EQ_UI
  if(pme->m_eWin == MPPW_PLAY && pme->m_pOwner->m_bShowPlayerWinSettingUI)
  {
    CMediaPlayer_AudioSetupScreen(pme->m_pOwner);
    CPlayerWin_Redraw((IWindow *)pme);
  }
#endif //FEATURE_MP_EQ_UI

  DBGPRINTF_MED("MP: end creating player win..");
  return (IWindow *)pme;
}

/*=====================================================================
FUNCTION: CPlayerWin_SetScreenRects

  DESCRIPTION: This function sets the rectangles for each of the PlayerWin
    screen objects.

  PARAMETERS:
    pme - Pointer to CPlayerWin struct

  RETURN VALUE:
    void
======================================================================*/
static void CPlayerWin_SetScreenRects(CPlayerWin *pme)
{  
  /*
   * Set title and image rects. Create the static control.
   */
  int       y;
  int       dy;
  int       cx = pme->m_pOwner->m_cxWidth;
  int       cy = pme->m_pOwner->m_cyHeight;
  int       nLChSize = pme->m_pOwner->m_nLChSize;
  int       cyProgMain = pme->m_pOwner->m_nNChSize + 1;

  /*
   * File name text rect
   */
  y = 1;
  dy = nLChSize;
  SETAEERECT(&pme->m_rectTitle, 0, y, cx, dy);

  /*
   * Base image rect - the space between the player menu at the bottom and
   * the title/header boxes on the top.
   */
  y += pme->m_rectTitle.dy;
  dy = cy - y - cyProgMain - pme->m_pOwner->m_IconViewCtlDy - 2;
  SETAEERECT(&pme->m_rectImage, 0, y, cx, dy);

  /*
   * Info rect used by ProgMain
   */
  y = cy - cyProgMain - pme->m_pOwner->m_IconViewCtlDy - 2;
  dy = cyProgMain;   
  SETAEERECT(&pme->m_rectInfo, 0, y, cx, dy);  

  /*
   * Menu rect used by IconViewCtl
   */
  y = cy - pme->m_pOwner->m_IconViewCtlDy;
  dy = pme->m_pOwner->m_IconViewCtlDy;
  SETAEERECT(&pme->m_rectMenu, 0, y, cx, dy);
  DBGPRINTF_MED("MP: Setting PlayerMenu to x = %d, y = %d, dx = %d, dy = %d", 
                 pme->m_rectMenu.x, pme->m_rectMenu.y, pme->m_rectMenu.dx, pme->m_rectMenu.dy);

  if (CPlayerWin_IsImage(pme))
  {
    // If it is an image we don't have an info bar so give that space to the image
    // and set the info bar to a 0x0 rectangle
    pme->m_rectImage.dy += pme->m_rectInfo.dy;
    SETAEERECT(&pme->m_rectInfo, 0, 0, 0, 0);
  }
  /*
   * Set default rects to same area as Image
   */
  (void)MEMCPY(&pme->m_rectIdleImage, &pme->m_rectImage, sizeof(AEERect));
  (void)MEMCPY(&pme->m_dfltRectImage, &pme->m_rectImage, sizeof(AEERect));
  /*
   * Set Layer1, Layer2 rects to same area as Image
   */
#ifdef FEATURE_MMOVERLAY
  (void)MEMCPY(&pme->m_rectLayer1, &pme->m_rectImage, sizeof(AEERect));
  (void)MEMCPY(&pme->m_rectLayer2, &pme->m_rectImage, sizeof(AEERect));
#endif // FEATURE_MMOVERLAY
}

#ifdef FEATURE_MMOVERLAY
/*=====================================================================
FUNCTION CPlayerWin_RegisterOvScreen

  DESCRIPTION: This function registers the IMMOverlay display area for UI layer.

  PARAMETERS:
    pme - Pointer to CPlayerWin struct
    bFullScreen - flag to tell if it is registered for full screen or normal
    bTransparent - TBD

  RETURN VALUE:
    TRUE if success. Otherwise FALSE.
======================================================================*/
static boolean CPlayerWin_RegisterOvScreen(CPlayerWin *pme)
{
  IBitmap       *pBmp = NULL;
  OvData        ov;
  int           nRet = FALSE;

  if (!pme)
    return FALSE;

  // Clear old objects

  MP_RELEASEOVERLAY(pme->m_pIMMOverlayTitle);
  MP_RELEASEOVERLAY(pme->m_pIMMOverlayInfo);
  MP_RELEASEOVERLAY(pme->m_pIMMOverlayMenu);
  MP_RELEASEOVERLAY(pme->m_pIMMOverlaySetups);
  MP_RELEASEOVERLAY(pme->m_pIMMOverlayImage);
  MP_RELEASEOVERLAY(pme->m_pIMMOverlayLayer1);
  MP_RELEASEOVERLAY(pme->m_pIMMOverlayLayer2);

  //
  // Get device bitmap
  //
  if (SUCCESS == IDISPLAY_GetDeviceBitmap(pme->m_pIDisplay, &pBmp))
  {
    // Register the video image area first.  
    ov.pBmp = pBmp;
    ov.alpha = MP_ALPHA_BLEND_OFF;        // blending off
    ov.transparent = RGB_NONE;            // transparency off
    ov.layer = OV_LAYER_BASE_LAYER;       // base layer

    (void)MEMCPY(&ov.rc, &pme->m_rectImage, sizeof(AEERect));
    if (!CPlayerWin_RegisterOv(pme->m_pIMMOverlayMgr, &pme->m_pIMMOverlayImage, ov))
    {
      DBGPRINTF_HIGH("MP: Failed to register m_pIMMOverlayImage");
      goto exit;
    }
    nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_DEST_REGION, (uint32)&ov.rc, 
                              OV_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER | OV_FLAG_DEST_REGION_ALIGN_VERT_CENTER);
    if (SUCCESS != nRet)
    {
      DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_DEST_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
      goto exit;
    }

    ov.alpha = pme->m_pOwner->m_nAlphaBlend;
    ov.transparent = RGB_MASK_COLOR;
    ov.layer = OV_LAYER_TOP_LAYER;

    // Register for the title
    (void)MEMCPY(&ov.rc, &pme->m_rectTitle, sizeof(AEERect));
    if (!CPlayerWin_RegisterOv(pme->m_pIMMOverlayMgr, &pme->m_pIMMOverlayTitle, ov))
    {
      DBGPRINTF_HIGH("MP: Failed to register m_pIMMOverlayTitle");
      goto exit;
    }

    // Register for the info/prog bar
    (void)MEMCPY(&ov.rc, &pme->m_rectInfo, sizeof(AEERect));
    if (!CPlayerWin_RegisterOv(pme->m_pIMMOverlayMgr, &pme->m_pIMMOverlayInfo, ov))
    {
      DBGPRINTF_HIGH("MP: Failed to register m_pIMMOverlayInfo");
      goto exit;
    }     
          
    // Register for the icon menu area
    (void)MEMCPY(&ov.rc, &pme->m_rectMenu, sizeof(AEERect));
    if (!CPlayerWin_RegisterOv(pme->m_pIMMOverlayMgr, &pme->m_pIMMOverlayMenu, ov))
    {
      DBGPRINTF_HIGH("MP: Failed to register m_pIMMOverlayMenu");
      goto exit;
    }

    // Register for the setups window
    (void)MEMCPY(&ov.rc, &pme->m_rectSetups, sizeof(AEERect));
    if (!CPlayerWin_RegisterOv(pme->m_pIMMOverlayMgr, &pme->m_pIMMOverlaySetups, ov))
    {
      DBGPRINTF_HIGH("MP: Failed to register m_pIMMOverlaySetups");
      goto exit;
    }

    //If Layer1 overlay is available, get bitmap and register window
    if (CPlayerWin_Layer1LoadBitmap(pme, pBmp))
    {
      DBGPRINTF_MED("MP: Registering Layer1 Overlay");
      ov.layer = MP_MMOVERLAY_LAYER1;
      (void)MEMCPY(&ov.rc, &pme->m_rectLayer1, sizeof(AEERect));
      if (!CPlayerWin_RegisterOv(pme->m_pIMMOverlayMgr, &pme->m_pIMMOverlayLayer1, ov))
      {
        DBGPRINTF_HIGH("MP: Failed to register m_pIMMOverlayLayer1");
        goto exit;
      }
      else
      {
        DBGPRINTF_MED("MP: RegisterOvScreen: m_pIMMOverlayLayer1 = 0x%x", pme->m_pIMMOverlayLayer1);
      }
    }

    //If Layer2 overlay is available, get bitmap and register window
    if (CPlayerWin_Layer2LoadBitmap(pme, pBmp))
    {
      DBGPRINTF_MED("MP: Registering Layer2 Overlay");
      ov.layer = MP_MMOVERLAY_LAYER2;
      (void)MEMCPY(&ov.rc, &pme->m_rectLayer2, sizeof(AEERect));
      if (!CPlayerWin_RegisterOv(pme->m_pIMMOverlayMgr, &pme->m_pIMMOverlayLayer2, ov))
      {
        DBGPRINTF_HIGH("MP: Failed to register m_pIMMOverlayLayer2");
        goto exit;
      }
      else
      {
        DBGPRINTF_MED("MP: RegisterOvScreen: m_pIMMOverlayLayer2 = 0x%x", pme->m_pIMMOverlayLayer2);
      }
    }

    nRet = TRUE;

  }
  else
  {
    DBGPRINTF_HIGH("MP: IDISPLAY_GetDeviceBitmap Failed");
  }

exit:
  MP_RELEASEBITMAP(pBmp);
  return nRet;
}

/*===========================================================================
  FUNCTION: CPlayerWin_RegisterOv

  DESCRIPTION:
    This function registers the specified foreground area for MDP overlay.
  
  PARAMETERS:
    pIMMOverlayMgr - Pointer to IMMOverlayMgr interface
    pIMMOverlay    - Pointer to the IMMOverlay pointer that will be created
    ov             - Overlay data structure 

  RETURN VALUE:
    TRUE if success. Otherwise FALSE.
===========================================================================*/
static boolean CPlayerWin_RegisterOv(IMMOverlayMgr *pIMMOverlayMgr, IMMOverlay **ppIMMOverlay, OvData ov)
{
  AEEPoint pt;
  int      nRet;

  if (!pIMMOverlayMgr || !ppIMMOverlay)
    return FALSE;

  pt.x = ov.rc.x;
  pt.y = ov.rc.y;

  //Create overlay object in AEECLSID_DISPLAY1 with provided ov data
  if (SUCCESS == IMMOVERLAYMGR_CreateOverlay(pIMMOverlayMgr, ov.pBmp, AEECLSID_DISPLAY1, pt, ppIMMOverlay))
  {
    nRet = CPlayerWin_SetOvLayer(*ppIMMOverlay, (uint32)ov.layer);
    if (SUCCESS != nRet)
    {
      DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_LAYER, %d, %s", nRet, MP_ErrorStateToString(nRet));
      return FALSE;
    }
 
    nRet = IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_DEST_REGION, (uint32)&ov.rc, 0);
    if (SUCCESS != nRet)
    {
      DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_DEST_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
      return FALSE;
    }

    nRet = IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_SRC_REGION, (uint32)&ov.rc, 0);
    if (SUCCESS != nRet)
    {
      DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_SRC_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
      return FALSE;
    }

    if (ov.transparent == RGB_NONE)
    {
      //turn off transparency
      nRet = IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_TRANSPARENCY, 0, 0);
      if (SUCCESS != nRet)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_TRANSPARENCY, %d, %s", nRet, MP_ErrorStateToString(nRet));
        return FALSE;
      }
    }
    else
    {
      //turn on transparency
      nRet = IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_TRANSPARENCY, 1, ov.transparent);
      if (SUCCESS != nRet)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_TRANSPARENCY, %d, %s", nRet, MP_ErrorStateToString(nRet));
        return FALSE;
      }
    }

    nRet = IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_ALPHA_BLEND_PCT, ov.alpha, 0);
    if (SUCCESS != nRet)
    {
      DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_ALPHA_BLEND_PCT, %d, %s", nRet, MP_ErrorStateToString(nRet));
      return FALSE;
    }

    if (OV_LAYER_BASE_LAYER == ov.layer) // Just set OV_PARM_SYSTEM_OVERLAY for Base Layer
    {
      nRet = IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_SYSTEM_OVERLAY, 1, 0);
      if (SUCCESS != nRet)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_DEST_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
        return FALSE;
      }
    }
  }
  else
  {
    DBGPRINTF_ERROR( "MP: Failed to Create Overlay");
    return FALSE;
  }

  return TRUE;

}

/*===========================================================================
  FUNCTION: CPlayerWin_SetOvLayer

  DESCRIPTION:
    This function sets the Overlay layer for the specified overlay.  If the 
    requested layer is not OV_LAYER_BASE_LAYER or OV_LAYER_TOP_LAYER, an attempt
    is made to set the overlay layer to the requested value.  If this attempt
    fails, an attempt is then made to set the overlay layer to OV_LAYER_TOP_LAYER. 
    Note, some platforms (6260) do not support layers other than OV_LAYER_BASE_LAYER
    and OV_LAYER_TOP_LAYER.
  
  PARAMETERS:
    pIMMOverlay    - Pointer to the IMMOverlay object
    ovlayer        - Overlay layer 

  RETURN VALUE:
    SUCCESS        - If overlay successfully set.
===========================================================================*/
static int CPlayerWin_SetOvLayer(IMMOverlay *pIMMOverlay, uint32 ovlayer)
{
  int nRet = EFAILED;

  if ((OV_LAYER_BASE_LAYER != ovlayer) && (OV_LAYER_TOP_LAYER != ovlayer))
  {
    nRet = IMMOVERLAY_SetParam(pIMMOverlay, OV_PARM_LAYER, ovlayer, 0);
    if (SUCCESS != nRet)
    {
      DBGPRINTF_MED("MP: Defaulting OV_PARM_LAYER to OV_LAYER_TOP_LAYER");
      nRet = IMMOVERLAY_SetParam(pIMMOverlay, OV_PARM_LAYER, (uint32)OV_LAYER_TOP_LAYER, 0);
    }
  }
  else
  {
    nRet = IMMOVERLAY_SetParam(pIMMOverlay, OV_PARM_LAYER, ovlayer, 0);
  }

  if (SUCCESS != nRet)
  {
    DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_LAYER, %d, %s", nRet, MP_ErrorStateToString(nRet));
  }

  return nRet;
}

/*===========================================================================
  FUNCTION: CPlayerWin_Layer1LoadBitmap

  DESCRIPTION:
    This function loads the bitmap for the Layer1 overlay.
  
  PARAMETERS:
    pme - ptr to PlaywerWin instance
    pDeviceBitmap - ptr to device bitmap

  OUTPUT:
    m_pLayer1Bitmap - IBitmap object created.  Must be released by client.

  RETURN VALUE:
    TRUE if success. Otherwise FALSE.
===========================================================================*/
static boolean CPlayerWin_Layer1LoadBitmap(CPlayerWin *pme, IBitmap *pDeviceBitmap)
{
  return (CPlayerWin_LoadBitmapFromFile(pme, pDeviceBitmap, &(pme->m_pLayer1Bitmap), MEDIAPLAYER_LAYER1_FILE));
}

/*===========================================================================
FUNCTION: CPlayerWin_Layer2LoadBitmap

  DESCRIPTION:
    This function loads the bitmap for the Layer2 overlay.
  
  PARAMETERS:
    pme - ptr to PlaywerWin instance
    pDeviceBitmap - ptr to device bitmap

  OUTPUT:
    m_pLayer2Bitmap - IBitmap object created.  Must be released by client.

  RETURN VALUE:
    TRUE if success. Otherwise FALSE.
===========================================================================*/
static boolean CPlayerWin_Layer2LoadBitmap(CPlayerWin *pme, IBitmap *pDeviceBitmap)
{
  return (CPlayerWin_LoadBitmapFromFile(pme, pDeviceBitmap, &(pme->m_pLayer2Bitmap), MEDIAPLAYER_LAYER2_FILE));
}

/*===========================================================================
FUNCTION: CPlayerWin_LoadBitmapFromFile

  DESCRIPTION:
    This function loads bitmap from file, if file is found.
  
  PARAMETERS:
    pme           - ptr to CPlayerWin instance
    pDeviceBitmap - ptr to device bitmap
    ppOutBitmap   - ptr to ptr to bitmap to load
    pFilename     - name of file to load bitmap from

  OUTPUT:
    ppOutBitmap   - IBitmap object created.  Must be released by client.

  RETURN VALUE:
    TRUE if success. Otherwise FALSE.
===========================================================================*/
static boolean CPlayerWin_LoadBitmapFromFile(CPlayerWin *pme, IBitmap *pDeviceBitmap, IBitmap **ppOutBitmap, char *pFilename)
{
  IBitmap      *pInBitmap;
  AEEBitmapInfo bi;
  int           nRet;

  if (!pme || !pDeviceBitmap || !ppOutBitmap || !pFilename)
  {
    DBGPRINTF_HIGH("MP: Bad Param, Could not load bitmap from file");
    return FALSE;
  }

  // Make sure bitmap does not already exits
  if (*ppOutBitmap)
  {
    MP_RELEASEBITMAP(*ppOutBitmap);
  }

  // Load bitmap from file 
  pInBitmap = ISHELL_LoadBitmap(pme->m_pIShell, pFilename);
  if (NULL == pInBitmap)
  {
    DBGPRINTF_HIGH("MP: File %s NOT loaded.", pFilename);
    return FALSE;
  }
  else
  {
    DBGPRINTF_MED("MP: File %s loaded.", pFilename);
    (void)IBITMAP_GetInfo(pInBitmap, &bi, sizeof(bi));
    DBGPRINTF_MED("MP: pInBitmap Info: %d x %d x %d", bi.cx, bi.cy, bi.nDepth);
  }

  // Create device compatible bitmap, sized properly for overlay
  nRet = IBITMAP_CreateCompatibleBitmap(pDeviceBitmap, ppOutBitmap, bi.cx, bi.cy);
  if (nRet != SUCCESS)
  {
    DBGPRINTF_HIGH("MP: Failed to create compatible bitmap");
    MP_RELEASEBITMAP(pInBitmap);
    return FALSE;
  }
  else
  {
    (void)IBITMAP_GetInfo(*ppOutBitmap, &bi, sizeof(bi));
    DBGPRINTF_MED("MP: OutBitmap Info: [0x%x] %d x %d x %d", *ppOutBitmap, bi.cx, bi.cy, bi.nDepth);
  }

  // Convert bmp pixels into pixels compatible with device
  nRet = IBITMAP_BltIn(*ppOutBitmap, 0, 0, bi.cx, bi.cy, pInBitmap, 0, 0, AEE_RO_COPY);
  if (nRet != SUCCESS)
  {
    DBGPRINTF_HIGH("MP: Failed to create OutBitmap");
    MP_RELEASEBITMAP(*ppOutBitmap);
    MP_RELEASEBITMAP(pInBitmap);
    return FALSE;
  } 
  else
  {
    DBGPRINTF_MED("MP: OutBitmap Info: [0x%x] %d x %d x %d", *ppOutBitmap, bi.cx, bi.cy, bi.nDepth);
  }

  MP_RELEASEBITMAP(pInBitmap);
  return TRUE;
}
#endif // FEATURE_MMOVERLAY

/*===========================================================================
   This function deletes the player window.
===========================================================================*/
static void
CPlayerWin_Delete(IWindow *po)
{
  CPlayerWin *pme = (CPlayerWin *)po;
  CMediaPlayer *pMP = pme->m_pOwner;
  boolean   bChg = 0;
  int       nState = 0;
  int       nRet;

  DBGPRINTF_MED("MP: start deleting player win.");
  if(pMP)
  {
    pMP->m_playState = MP_PLAYSTATE_EXITING;
    EMIT_MEDIAPLAYER_PLAYSTATE(pMP->m_playState);
  }
  else
    DBGPRINTF_ERROR("MP: NULL pointer encountered.");

  // Cancel all the applicable timers before cleaning the house.
  // This is to prevent any timer event callbacks from happening when the callback context may be questionable.
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (pme->m_b3GPPTTextOn)
  {
    (void)ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)CPlayerWin_3GPPTimedTextExpiredNotify, pme);
    CPlayerWin_3GPPTText(pme, NULL);
  }
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT

  if (CPlayerWin_IsMedia(pme))
  {
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    if ((TRUE == pme->m_pOwner->m_bEnablePSSimulation) ||
        (pme->m_pOwner->m_bIsHTTP == TRUE))
    {
      if (pMP->a.m_pIShell)
        (void)ISHELL_CancelTimer(pMP->a.m_pIShell, PSTimerResponse, (void *)(pme->m_pOwner));
    }
#endif //defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))

    /*
     * Release ProgCtl object and cancel it's timer
     */
    MPProgCtl_Release(&pme->m_pMPProgCtl);
    
    (void)ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)CPlayerWin_AutoPlayClip, pMP);

    // Kill all of our timers, in case we missed some above
    (void)ISHELL_CancelTimer(pme->m_pIShell, NULL, pme);
  }
  // put it back in default mode, enable annunicators
  DBGPRINTF_MED("MP: enable annunciators.");
  nRet = IDISPLAY_SetPrefs(pme->m_pIDisplay, "a:1", strlen("a:1"));
  if(nRet != SUCCESS)
    DBGPRINTF_ERROR("MP: failed on IDISPLAY_SetPrefs, nRet = %d", nRet);

  MP_RELEASEIF(pme->m_pTitle);
  MP_RELEASEIF(pme->m_pIdleImage);
  MP_RELEASEMENU(pMP, pme->m_pPlayerMenu);
#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
  MP_RELEASEIF(pMP->m_pContentISource);
#endif //(defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))


#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
  {
    extern void MP_ArchiveCleanup(void);
    MP_ArchiveCleanup();
  }
#endif

  /*
  * this will free the Info resources
  */
  pme->m_bDIsplayInfo = FALSE;
  MP_RELEASEIF(pme->m_pInfoTextCtl);
  pme->m_pInfoTextCtl = NULL;

#if defined(MP_FEATURE_VIDEO)
  /*
   * free if media data list is there
   */
  if (pme->m_pmdExList)
  {
    DBGPRINTF_MED("MP: deleting media data list.");
    MP_FreeMediaDataExList(pme->m_pmdExList, pme->m_mdListCount);
    pme->m_pmdExList = NULL;
    pme->m_mdListCount = 0;
  }
#endif     // MP_FEATURE_VIDEO

  if (CPlayerWin_IsMedia(pme))
  {

#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)
    if(pme->m_pszFile)
    {
      // make a copy of current file name
      (void)STRLCPY(pme->m_pOwner->m_strReplayFileName, pme->m_pszFile, MP_MAX_FILE_NAME);
    }
#endif //#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)

    pme->m_bIsCMXFile = TRUE;

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    if ((TRUE == pme->m_pOwner->m_bEnablePSSimulation) ||
        (pme->m_pOwner->m_bIsHTTP == TRUE))
    {
      if (pMP->m_downloadFileBuffer)
      {
        FREE(pMP->m_downloadFileBuffer);
        pMP->m_downloadFileBuffer = NULL;
      }

      if (pMP->m_pPseudoStrIFile)
      {
        (void)IFILE_Release(pMP->m_pPseudoStrIFile);
        pMP->m_pPseudoStrIFile = NULL;
      }
      if (pme->psHttpBlock)
      {
        if (pme->psHttpBlock->bWebPstreaming)
        {
          PS_CleanHttpDownload(pme->psHttpBlock);
        }
        FREE(pme->psHttpBlock);
        pme->psHttpBlock = NULL;
      }
    }

    if (PSEUDO_STREAM != pme->m_pOwner->m_nPlayerType)
    {
      pme->m_pOwner->m_bFileWriteReadTest = FALSE;
      pme->m_pOwner->m_bEnablePSSimulation = FALSE;
    }
#endif     // FEATURE_QTV_PSEUDO_STREAM ||
           // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

    if (pme->m_pMedia)
    {
      nState = IMEDIA_GetState(pme->m_pMedia, &bChg);
      if (bChg)
      {
        DBGPRINTF_MED("MP: media is in transition state (%d = %s).",
                      nState,  MP_MediaStateToString(nState));
      }
      else if(nState == MM_STATE_PLAY)
      {
        DBGPRINTF_MED("MP: media is in playing state (%d = %s).",
                      nState,  MP_MediaStateToString(nState));
      }

      // Stop play before releasing media
      if (CPlayerWin_IsPlaying(pme))
      {
        DBGPRINTF_MED("MP: Stopping play");
        (void)IMEDIA_Stop(pme->m_pMedia);
      }
      /* Delete (release) the IMEDIA object. */
      DBGPRINTF_MED("MP: deleting m_pMedia.");
      MP_RELEASEIF(pme->m_pMedia);
      MP_RELEASEIF(pme->m_pMedia1);
    }
    else
    {
      DBGPRINTF_MED("MP: Can't delete m_pMedia.");
    }
  }
  else if (CPlayerWin_IsImage(pme))
  {
    DBGPRINTF_MED("MP: Deleting image memory");
    MP_RELEASEIF(pme->m_pImage);
    MP_RELEASEMENU(pMP, pme->m_pImageCtl);
  }

  /*
   * Release memory for file name allocated in CMediaPlayer
   */
  if (!MP_ISPLUGIN(pme))
  {
    boolean bDeleteBuffer = TRUE;

    if((char*)pme->m_pszFile == (char*)pme->m_md.pData)
    {
      // Both point to  same memory, set bDeleteBuffer to prevent double free
      bDeleteBuffer = FALSE;
    }

    FREEIF(pme->m_pszFile);

    // Free the memory we previously allocated.
    // This is only needed if it is MMD_BUFFER. if it is MMD_FILE_NAME then m_pszFile
    // Also points to the same memory and is freed by it
    if (pme->m_md.pData && pme->m_md.clsData == MMD_BUFFER && bDeleteBuffer)
    {
      FREEIF(pme->m_md.pData);
    }
  }

#if defined (FEATURE_QTV_BCAST_FLO) && !defined (FEATURE_APP_MEDIA_VIDEO)
#error code not present
#endif

#ifdef FEATURE_MP_EQ_UI
  // this function only release resources,
  // we need to keep pme->m_pOwner->m_pSetting ptr
  // to save information into config file when exit from MP
  MPSettingRelease(pme->m_pOwner->m_pSetting);

#endif //FEATURE_MP_EQ_UI

  // release IGraphics interface
  if(pme->m_pOwner->m_pIGraphics)
  {
    (void)IGRAPHICS_Release(pme->m_pOwner->m_pIGraphics);
  }
  pme->m_pOwner->m_pIGraphics = NULL;

  pme->m_pOwner->m_bShowPlayerWinSettingUI = FALSE;
  pme->m_pOwner->m_bShowLogo = FALSE;

  FREEIF(pme->m_pAudioSpec)

#ifdef FEATURE_MMOVERLAY
  if (pme->m_pOwner->m_bOverlays)
  {
    DBGPRINTF_MED("MP: release IMMOverlays");
    MP_RELEASEOVERLAY(pme->m_pIMMOverlayTitle);
    MP_RELEASEOVERLAY(pme->m_pIMMOverlayImage);
    MP_RELEASEOVERLAY(pme->m_pIMMOverlayInfo);
    MP_RELEASEOVERLAY(pme->m_pIMMOverlayMenu);
    MP_RELEASEOVERLAY(pme->m_pIMMOverlaySetups);
    MP_RELEASEOVERLAY(pme->m_pIMMOverlayLayer1);
    MP_RELEASEOVERLAY(pme->m_pIMMOverlayLayer2);
    MP_RELEASEOVERLAYMGR(pme->m_pIMMOverlayMgr);
    MP_RELEASEBITMAP(pme->m_pLayer1Bitmap);
    MP_RELEASEBITMAP(pme->m_pLayer2Bitmap);
  }
#endif // FEATURE_MMOVERLAY


  if(pMP->m_fsHandle)
  {
    fs_rsp_msg_type fsResult;
    fs_close( pMP->m_fsHandle, NULL, &fsResult );
    if ( fsResult.close.status != FS_OKAY_S )
    {
      DBGPRINTF_ERROR("MP: failed to close file handle, status = %d", fsResult.close.status);       
    }  
    pMP->m_fsHandle = 0;
  }


  DBGPRINTF_MED("MP: deleting player win.");
  FREE(pme);
  if (pMP)
  {
    pMP->m_pWin = NULL;
    pMP->m_eActiveWin = MPW_NONE;
    pMP->m_State = MP_STATE_NULL;
    pMP->m_bResumeInProcessing = FALSE;
    pMP->m_bSuspendPending = FALSE;
    EMIT_MEDIAPLAYER_STATE(pMP->m_State);
  }
  DBGPRINTF_MED("MP: end deleting player win.");
}

/*===========================================================================
   This function enables/disables the player window.
===========================================================================*/
static void
CPlayerWin_Enable(IWindow *po, boolean bEnable)
{
  CPlayerWin *pme = (CPlayerWin *)po;
  boolean   bActive;

  if (!CWindow_ProcessEnable(po, bEnable))
    return;

  bActive = pme->m_bActive;

  ISTATIC_SetActive(pme->m_pTitle, bActive);
  IMENUCTL_SetActive(pme->m_pPlayerMenu, bActive);

  if (CPlayerWin_IsImage(pme))
  {
    if(pme->m_pImageCtl)
      IIMAGECTL_SetActive(pme->m_pImageCtl, bActive);
  }

  if (!pme->m_bActive)
  {
    if(pme->m_pIdleImage)
    {
      IIMAGE_Stop(pme->m_pIdleImage);
    }
    if (CPlayerWin_IsMedia(pme) && pme->m_pMedia)
    {
      (void)IMEDIA_Stop(pme->m_pMedia);
    }
  }
  else
  {
    if (!pme->m_bFullScreen)
    {
      uint16 menuSel = IDM_PM_PLAY;
   
      if(pme->m_eWin == MPPW_PLAY)
      {
        menuSel = IDM_PM_PLAY;
        // Set pause option as current if resumePlayPos set
        if(pme->m_pOwner->m_resumePlayPos > 0)
          menuSel = IDM_PM_PAUSE;
      }
      else
      {
        menuSel = IDM_PM_RECORD;
      }
      IMENUCTL_SetSel(pme->m_pPlayerMenu, menuSel);
    }
  }
}

/*===========================================================================
   This function redraws player window.
   If the full screen mode is on, then
   (1)if playback is not ongoing, then it paints the screen with black.
   (2)if image is being displayed, then redraws the image.
===========================================================================*/
static void
CPlayerWin_Redraw(IWindow *po)
{
  CPlayerWin *pme = (CPlayerWin *)po;
  char     *pszFile = pme->m_pszFile;
  AEERect   rect;

  if (!pme->m_bActive)
  {
    return;
  }

  if (MP_ISFULLSCRN(pme))
  {
    DBGPRINTF_MED("MP: CPlayerWin Redraw - Fullscreen started ...");
    if (!CPlayerWin_IsPlaying(pme) && !CPlayerWin_IsRecording(pme) && !CPlayerWin_IsImage(pme))
    {
      DBGPRINTF_MED("MP: CPlayerWin Redraw - Fullscreen not playing ...");
      // If not playing, recording, or displaying an image,
      // prepare screen for next image.
      IDISPLAY_EraseRgn(pme->m_pIDisplay, pme->m_rectImage.x ,pme->m_rectImage.y, pme->m_rectImage.dx, pme->m_rectImage.dy);
      IDISPLAY_ClearScreen(pme->m_pIDisplay);
      CPlayerWin_UpdateImage(pme);
    }
    else if (CPlayerWin_IsImage(pme))
    {
      DBGPRINTF_MED("MP: CPlayerWin Redraw - Fullscreen image...");
      // Redraw image
      IDISPLAY_ClearScreen(pme->m_pIDisplay);
      (void)IIMAGECTL_Redraw(pme->m_pImageCtl);
      CPlayerWin_UpdateImage(pme);
    }
    else
    {
      // We need to clear screen and force an update as toggling between regular and fullscreen may leave
      // annunicators, menu, title and progress bar around in all but the first time fullscreen mode.
      IDISPLAY_ClearScreen(pme->m_pIDisplay);
      IDISPLAY_Update(pme->m_pIDisplay);
    }

    return;
  }

  /*
   * Not in FullScreen mode
   */
  IDISPLAY_ClearScreen(pme->m_pIDisplay);
  CPlayerWin_SetImageRect(pme); 

  if (CPlayerWin_IsMedia(pme))
  {
#ifdef MP_FEATURE_VIDEO
    if (!(pme->m_bNoTextOrHeader))
    {
#endif     // MP_FEATURE_VIDEO
      /*
       * Header
       */
      //MP_DRAWHEADER(pme);

      /*
       * File name (title)text
       */
      if (pszFile)
      {
        // If we are showing the extension, just copy over the file name
        // otherwise remove the extention from the filename
        if (pme->m_pOwner->m_bShowExtension)
        {
          (void)STRTOWSTR(MP_GetFileName(pszFile), pme->m_szText, sizeof(pme->m_szText));
        }
        else
        {
          char fileTitle[MP_MAX_FILE_NAME + 1];
          (void)MEMSET(fileTitle, '\0', MP_MAX_FILE_NAME + 1);
          (void)STRTOWSTR(MP_GetFileNameNoExtension(pszFile, fileTitle, MP_MAX_FILE_NAME), pme->m_szText, sizeof(pme->m_szText));
        }
        MP_FitStaticText(pme->m_pIDisplay, pme->m_pTitle, AEE_FONT_NORMAL, pme->m_szText);

        CPlayerWin_UpdateTitle(pme);
      }

      /*
       * Draw the line below text.
       */
      if(pme->m_pTitle)
      {
        ISTATIC_GetRect(pme->m_pTitle, &rect);
        rect.y += rect.dy - 1;
        rect.dy = 1;
        MP_FrameRect(pme->m_pIDisplay, &rect);
      }
      if (pme->m_pOwner->m_bShowLogo && !pme->m_pOwner->m_bShowPlayerWinSettingUI)
      {
        /*
         * Initialize logo below the header
         */
        (void)CMediaPlayer_SetIdleImage(pme, IDB_LOGO);
        /*
         * Video/Image
         */
        MP_DrawImage(pme->m_pIdleImage, &pme->m_rectIdleImage, TRUE);
      }
#ifdef MP_FEATURE_VIDEO
    }
#endif


    if(pme->m_bDIsplayInfo && pme->m_pInfoTextCtl)
    {
      (void)ISTATIC_Redraw(pme->m_pInfoTextCtl);
      MP_FrameRect(pme->m_pIDisplay, &pme->m_rectImage);
    }

    /*
     * Tell the progress bar to update itself.
     */
    CPlayerWin_UpdateProgCtl(pme, MP_EVENT_REDRAW_CTRL, 0, 0);
  }
  else if (CPlayerWin_IsImage(pme))
  {
    DBGPRINTF_MED("MP: CPlayerWin_Redraw: IsImage");

    if(pme->m_pImageCtl)
    {
      (void)IIMAGECTL_Redraw(pme->m_pImageCtl);
    }
    CPlayerWin_UpdateImage(pme);
  }
  else
  {
    DBGPRINTF_MED("MP: Neither Media or Image");
  }
  /*
   * SoftKey
   */
  if(pme->m_pPlayerMenu)
  {
    (void)IMENUCTL_Redraw(pme->m_pPlayerMenu);
#ifdef FEATURE_MMOVERLAY
    CPlayerWin_UpdateMenuOverlay(pme);
#endif // FEATURE_MMOVERLAY
  }

#ifdef FEATURE_MMOVERLAY
  if (pme->m_pOwner->m_bOverlays)
  {
    if (pme->m_pLayer1Bitmap)
    {
      /*
       * Center Layer1 overlay over video image
       */
      CPlayerWin_SetLayer1Rect(pme);
      CPlayerWin_UpdateLayer1(pme);
    }
    if (pme->m_pLayer2Bitmap)
    {
      /*
       * Center Layer2 overlay over video image
       */
      CPlayerWin_SetLayer2Rect(pme);
      CPlayerWin_UpdateLayer2(pme);
    }
  }
#endif // FEATURE_MMOVERLAY

#ifdef FEATURE_MP_EQ_UI
  {
    boolean bHasVideo;
    bHasVideo = CPlayerWin_IsVideoMedia(pme);
    (void)MPUpdateSetupWin(pme->m_pOwner->m_pSetting, pme->m_pOwner->m_pIGraphics, bHasVideo);
  }
#endif //FEATURE_MP_EQ_UI

  if(pme->m_pOwner->m_pDlg)
  {
    (void)MPDlgUpdate(pme->m_pOwner->m_pDlg, pme->m_pOwner->m_pIGraphics);
  }
}

/*===========================================================================
   This function Updates Title display.
===========================================================================*/
static void CPlayerWin_UpdateTitle(CPlayerWin *pme)
{
#ifdef FEATURE_MMOVERLAY

  /*
   * When overlay mode is enabled,
   * use IMMOverlay_Update to update display.
   */
  if (pme->m_pOwner->m_bOverlays)
  {
    uint32    nRet;

    (void)ISTATIC_Redraw(pme->m_pTitle);
    nRet = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayTitle, NULL, 0);
    if (nRet == MP_ERR_IMMOVERLAY)
    {
      DBGPRINTF_ERROR("MP: IMMOVERLAY_UpdateAsync Failed, nRet=%d", nRet);
    }
  }
  else
#endif // FEATURE_MMOVERLAY
  {
    (void)ISTATIC_Redraw(pme->m_pTitle);
  }
}

/*===========================================================================
   This function Updates Image rectangle.  Sets to default region.
===========================================================================*/
static void CPlayerWin_SetImageRect(CPlayerWin *pme)
{
  // Make sure Image Rectangle is set properly
    
  if (!MPSetupWinIsShow(pme->m_pOwner->m_pSetting))
  {
    (void)MEMCPY(&pme->m_rectImage, &pme->m_dfltRectImage, sizeof(AEERect));

#ifdef FEATURE_MMOVERLAY
    if (pme->m_pOwner->m_bOverlays)
    {
      int nRet;
      /*
       * If overlays are enabled, use IMMOVERLAY methods
       * to define Image region.
       */
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_DEST_REGION, (uint32)&pme->m_rectImage, 
               OV_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER | OV_FLAG_DEST_REGION_ALIGN_VERT_CENTER);
      if (SUCCESS != nRet)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_DEST_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
      }
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_SRC_REGION, (uint32)&pme->m_rectImage, 0);
      if (SUCCESS != nRet)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_SRC_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
      }
    }
#endif // FEATURE_MMOVERLAY
  }
  else
  {
    DBGPRINTF_MED("MP: Can't restore ImageRect, SetupWin is Showing.");
  }
}

/*===========================================================================
   This function Updates Image display.
===========================================================================*/
static void
CPlayerWin_UpdateImage(CPlayerWin *pme)
{
#ifdef FEATURE_MMOVERLAY
  uint32 nRet;

  if (pme->m_pOwner->m_bOverlays)
  {
    nRet = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayImage, NULL, 0);
    if (nRet == MP_ERR_IMMOVERLAY)
    {
      DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
    }
  }
  else
#endif // FEATURE_MMOVERLAY
  {
    IDISPLAY_Update(pme->m_pIDisplay);
  }
}

#ifdef FEATURE_MMOVERLAY
/*===========================================================================
   This function Updates the Menu Overlay.
===========================================================================*/
static void
CPlayerWin_UpdateMenuOverlay(CPlayerWin *pme)
{
  uint32 nRet;

  if (pme->m_pOwner->m_bOverlays)
  {
    nRet = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayMenu, NULL, 0);
    if (nRet == MP_ERR_IMMOVERLAY)
    {
      DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
    }
  }
}

/*===========================================================================
   This function sets up Layer1 rectangle.  
   Centers over Image rectangle and associates bitmap with destination region.
===========================================================================*/
static void CPlayerWin_SetLayer1Rect(CPlayerWin *pme)
{
  CPlayerWin_SetOverlayRect(pme, pme->m_pIMMOverlayLayer1, pme->m_pLayer1Bitmap, &(pme->m_rectLayer1));
  DBGPRINTF_MED("MP: m_rectLayer1 = %d,%d,%d,%d", pme->m_rectLayer1.x, pme->m_rectLayer1.y, pme->m_rectLayer1.dx, pme->m_rectLayer1.dy);
}

/*===========================================================================
   This function sets up Layer2 rectangle.  
   Centers over Image rectangle and associates bitmap with destination region.
===========================================================================*/
static void CPlayerWin_SetLayer2Rect(CPlayerWin *pme)
{
  CPlayerWin_SetOverlayRect(pme, pme->m_pIMMOverlayLayer2, pme->m_pLayer2Bitmap, &(pme->m_rectLayer2));
  DBGPRINTF_MED("MP: m_rectLayer2 = %d,%d,%d,%d", pme->m_rectLayer2.x, pme->m_rectLayer2.y, pme->m_rectLayer2.dx, pme->m_rectLayer2.dy);
}

/*===========================================================================
FUNCTION: CPlayerWin_SetOverlayRect

  DESCRIPTION:
    Centers bitmap over Image region.

  PARAMETERS:
    pme       - ptr to CPlayerWin instance
    pOverlay  - Ptr to IMMOverlay to associate bitmap with
    pBitmap   - IBitmap object
    pRect     - ptr to AEERect structure for overlay

  OUTPUTS:
    pRect     - Set to same dimensions as Image rectangle

  RETURN VALUE:
    None.
===========================================================================*/
static void CPlayerWin_SetOverlayRect(CPlayerWin *pme, IMMOverlay *pOverlay, IBitmap *pBitmap, AEERect *pRect)
{
  AEEBitmapInfo bi;
  AEERect       srcRect;
  int           nRet;

  if (pme && (pme->m_pOwner->m_bOverlays) && pBitmap && pOverlay)
  { 
    // Center bitmap in overlay
    (void)MEMCPY(pRect, &pme->m_rectImage, sizeof(AEERect));
    nRet = IMMOVERLAY_SetParam(pOverlay, OV_PARM_DEST_REGION, (uint32)pRect, 
        OV_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER | OV_FLAG_DEST_REGION_ALIGN_VERT_CENTER);
    if (SUCCESS != nRet)
    {
      DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_DEST_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
    }

    // Set source to bitmap
    (void)IBITMAP_GetInfo(pBitmap, &bi, sizeof(bi));
    srcRect.x = srcRect.y = 0;
    srcRect.dx = bi.cx;
    srcRect.dy = bi.cy;
    nRet = IMMOVERLAY_SetParam(pOverlay, OV_PARM_SRC_REGION, (uint32)&srcRect, 0);
    if (SUCCESS != nRet)
    {
      DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_SRC_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
    }

    // Do not clear overlay
    nRet = IMMOVERLAY_SetParam(pOverlay, OV_PARM_NO_AUTOCLEAR, 1, 0);
    if (SUCCESS != nRet)
    {
      DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_NO_AUTOCLEAR, %d, %s", nRet, MP_ErrorStateToString(nRet));
    }

    // Associate bitmap with overlay
    nRet = IMMOVERLAY_SetParam(pOverlay, OV_PARM_BITMAP, (uint32)pBitmap, 0);
    if (SUCCESS != nRet)
    {
      DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_BITMAP, %d, %s", nRet, MP_ErrorStateToString(nRet));
    }
  }
}

/*===========================================================================
   Update the Layer1 overlay.
===========================================================================*/
static void CPlayerWin_UpdateLayer1(CPlayerWin *pme)
{
  CPlayerWin_UpdateOverlay(pme, pme->m_pIMMOverlayLayer1);
}

/*===========================================================================
   Update the Layer2 overlay.
===========================================================================*/
static void CPlayerWin_UpdateLayer2(CPlayerWin *pme)
{
  CPlayerWin_UpdateOverlay(pme, pme->m_pIMMOverlayLayer2);
}

/*===========================================================================
   This function Updates the specified overlay.
===========================================================================*/
static void CPlayerWin_UpdateOverlay(CPlayerWin *pme, IMMOverlay *pOverlay)
{
  uint32 nRet;

  if (pme && (pme->m_pOwner->m_bOverlays) && pOverlay)
  {
    nRet = IMMOVERLAY_UpdateAsync(pOverlay, NULL, 0);
    if (nRet == MP_ERR_IMMOVERLAY)
    {
      DBGPRINTF_ERROR("MP: IMMOVERLAY_UpdateAsync %s Failed, nRet=%d", nRet);
    }
  }
}
#endif // FEATURE_MMOVERLAY



/*===========================================================================
   This function redraws player window without the header and text
===========================================================================*/
static void
CPlayerWin_EraseHeaderAndText(CPlayerWin *pme)
{
  IDISPLAY_ClearScreen(pme->m_pIDisplay);
  CPlayerWin_UpdateProgCtl(pme, 0, 0, 0);
  (void)IMENUCTL_Redraw(pme->m_pPlayerMenu);
}


/*===========================================================================
   This function handles presses of the OK or CLR key from an error dialog.
   (a)If app is in plugin mode, it is closed.
   (b)If media playback is on, then file list window is opened
   (c)If recording is on, then main window is opened
===========================================================================*/
static boolean
CPlayerWin_HandleClearOrOK(CPlayerWin *pme, AEEEvent eCode, uint16 wParam,
                           uint32 dwParam)
{

  if(MP_ISCLR(eCode))
  {
    DBGPRINTF_HIGH("MP: CLR key pressed.");
    EMIT_MEDIAPLAYER_STATE(pme->m_pOwner->m_State);
    EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
  }

  if(pme->m_pOwner->m_pDlg)
  {
    if(MPDlgIsDismiss(pme->m_pOwner->m_pDlg))
    {
      MPDlgDelete(pme->m_pOwner->m_pDlg);
      pme->m_pOwner->m_pDlg = NULL;

      CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
      return TRUE;
    }
  }

  /*
   * remove Info screen for any other button
   */
  if ((IDM_PM_INFO != wParam) && (pme->m_bDIsplayInfo))
  {
    (void)CPlayerWin_Info(pme, TRUE);
  }

#ifdef FEATURE_MP_EQ_UI
  if(pme->m_pOwner->m_pSetting->m_bEditMode)
  {
    return TRUE;
  }
  // if setup screen has focus, turn off and return, otherwise
  // close PlayerWin
  if(MPSetupFocus(pme->m_pOwner->m_pSetting, FALSE))
  {
    AEERect rcScreen;
    (void)MEMCPY(&rcScreen, &pme->m_rectImage, sizeof(AEERect));
    // set display rect back
    DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with x = %d, y = %d, cx = %d, cy = %d", rcScreen.x, rcScreen.y, rcScreen.dx, rcScreen.dy);
    (void)IMEDIA_SetRect(pme->m_pMedia, &rcScreen, NULL);

    CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
    return TRUE;
  }

#endif //FEATURE_MP_EQ_UI

  /*
   * Stop the app if it is in plugin mode.
   */
  if (MP_ISPLUGIN(pme))
    (void)ISHELL_CloseApplet(pme->m_pIShell, FALSE);
  else
  {
    if (pme->m_eWin == MPPW_PLAY || pme->m_eWin == MPPW_IMAGE)
    {
      if((CPlayerWin_IsPlaying(pme)) || CPlayerWin_IsPause(pme))
        (void)IMEDIA_Stop(pme->m_pMedia);

      if(CPlayerWin_IsImage(pme) && pme->m_bLoadingImage )
      {
        // make sure image is fully decoded before exiting
        pme->m_nWaitTimerCnt = 0;
        CPlayerWin_ExitOnImageLoadingFlag(pme);
        return TRUE;
      }
      else
        // we can exit from the window
        (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, 0);
    }
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
    else if ((pme->m_eWin == MPPW_RECORD) || (pme->m_eWin == MPPW_STREAM_RECORD))
#else
    else if (pme->m_eWin == MPPW_RECORD)
#endif
    {
      /*
       * reset the callback counter
       */
      pme->m_CB.m_nCBCount = 0;
      CPlayerWin_ExitRecorderWin(pme);
    }
  }

  return TRUE;
}
/*===========================================================================
   This function will terminate current Player window when
   1. m_bLoadingImage flag becomes to FALSE.
   2. Max. wait time is reached
===========================================================================*/
static void CPlayerWin_ExitOnImageLoadingFlag(CPlayerWin *pme)
{
  int stoptime = MAX_WAIT_TIME/100;
  pme->m_nWaitTimerCnt++;
  if((pme->m_nWaitTimerCnt*(stoptime) <= MAX_WAIT_TIME) && pme->m_bLoadingImage)
  {
    // try again
    (void)ISHELL_SetTimer(pme->m_pIShell, stoptime,(PFNNOTIFY)CPlayerWin_ExitOnImageLoadingFlag, pme);
  }
  else
  {
    // exit playerwin now
    pme->m_bLoadingImage = FALSE;
    (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, 0);
  }
}
/*===========================================================================
   This function will make sure that the recorder is fully stopped
   before exiting.
   if IMedia is in recording state, the function posts IDM_PM_STOP event
   and waits the callback from event handler.
===========================================================================*/
void
CPlayerWin_ExitRecorderWin(CPlayerWin *pme)
{
  if (pme->m_CB.m_nCBCount == 0)
  {
    if (CPlayerWin_IsRecording(pme) || CPlayerWin_IsPause(pme))
    {
      pme->m_CB.m_pFnPtr = &CPlayerWin_ExitRecorderWin;
      pme->m_CB.m_nCBCount++;
      pme->m_CB.m_nCmd = MM_CMD_RECORD;
      pme->m_CB.m_nSubCmd = 0;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_COMMAND, IDM_PM_STOP, 0);
    }
    else
    {
      (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_MAIN, 0);
    }
  }
  else
  {
    boolean   bChg = 0;
    int       nState = 0;

    if (pme->m_CB.m_nCmd == MM_CMD_RECORD && pme->m_CB.m_nSubCmd == 0)
    {
      /*
       * make sure IMedia is fully stopped
       */
      if (pme->m_pMedia)
        nState = IMEDIA_GetState(pme->m_pMedia, &bChg);

      /*
       * try max. 1 second
       */
      if ((bChg || MM_STATE_RECORD == nState) &&
          pme->m_CB.m_nCBCount < MP_MAX_RETRY_COUNTS)
      {
        pme->m_CB.m_nCBCount++;
#ifdef _MPDEBUG
        DBGPRINTF_HIGH
          ("MP: IMedia is still in recording state, start timer and try again in 50ms.");
#endif
        /*
         * wait for 50 ms and try it again
         */
        (void)ISHELL_SetTimer(pme->m_pIShell, 50, (PFNNOTIFY)CPlayerWin_ExitRecorderWin, pme);
        return;
      }

      pme->m_CB.m_pFnPtr = NULL;
      pme->m_CB.m_nCBCount = 0;
      pme->m_CB.m_nCmd = -1;
      pme->m_CB.m_nSubCmd = 0;

      (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_MAIN, 0);
    }
  }
}

/*===========================================================================
   These functions start and stop stream recording.
===========================================================================*/
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
static void
CPlayerWin_StartStreamRecord(CPlayerWin *pme, boolean overwriteFile)
{
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
  AEEStreamRecordInfo recInfo;
#endif //FEATURE_APP_MEDIA_VIDEO

  int retVal;

  if (pme == NULL)
  {
    DBGPRINTF_HIGH("MP:  pme is NULL!");
    return;
  }
  if (pme->m_pMedia == NULL)
  {
    DBGPRINTF_HIGH("MP:  m_pMedia is NULL!");
    return;
  }
  if (recFileName[0] == 0)
  {
    DBGPRINTF_HIGH("MP:  recFileName is empty!");
    return;
  }

  if(CPlayerWin_PurgeDownloadedFile(pme))
    return;
 
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
  recInfo.filename = STRDUP(recFileName);
  if (!recInfo.filename)
  {
    DBGPRINTF_HIGH("MP:  could not dup string = %s", recFileName);
    return;
  }
  recInfo.overwrite = overwriteFile;    // File overwrite flag
  recInfo.mode = 0xFF;           // Record all Tracks
  recInfo.duration = 0;          // No limit on recording duration
  DBGPRINTF_MED("MP: Start Stream Recording");
  DBGPRINTF_MED("MP: stream file = %s", recFileName);
  retVal = IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_RTSP_STREAM_CONVERT,
                               (int32) &recInfo, 0);
  FREE(recInfo.filename);
  if((retVal != SUCCESS) && (retVal != MM_PENDING))
  {
    DBGPRINTF_HIGH("MP: IMEDIA_SetMediaParm() returns %d = %s",
                  retVal, MP_ErrorStateToString(retVal));
    return;
  }
#endif //FEATURE_APP_MEDIA_VIDEO

  CPlayerWin_AdjustStreamRecordMenu(pme);
  return;
}

static void
CPlayerWin_StopStreamRecord(CPlayerWin *pme)
{
  int retVal;

  if (pme == NULL)
  {
    DBGPRINTF_HIGH("MP:  pme is NULL!");
    return;
  }
  if (pme->m_pMedia == NULL)
  {
    DBGPRINTF_HIGH("MP:  m_pMedia is NULL!");
    return;
  }

  DBGPRINTF_MED("MP: Stop Stream Recording");
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
  retVal = IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_RTSP_STOP_STR_CONVERT,
                               (int32) 0, (int32) 0);
#endif //FEATURE_APP_MEDIA_VIDEO

  if((retVal != SUCCESS) && (retVal != MM_PENDING))
  {
    DBGPRINTF_HIGH("MP: IMEDIA_SetMediaParm() returns %d = %s",
                  retVal, MP_ErrorStateToString(retVal));
  }

#ifdef MP_FEATURE_ICONTENT
  // save recorded file into IContent DB
  if(pme->m_pOwner->m_bIContentMode && recFileName)
  {
    MP_AddFileToIContentDB(pme->m_pOwner,
                      MP_ICONTENT_CAT_NAMR_RECORD, MP_ICONTENT_CAT_DESC_RECORD,
                      recFileName, MP_ICONTENT_CAT_DESC_RECORD,
                      MT_VIDEO_MPEG4);
  }
#endif //MP_FEATURE_ICONTENT
}
#endif

/*===========================================================================
   This function processes key events (event code EVT_KEY).
===========================================================================*/
#ifndef FEATURE_KEY_HOLD_SCROLL
static boolean
CPlayerWin_HandleEventKey(CPlayerWin *pme, AEEEvent eCode, uint16 wParam,
                          uint32 dwParam)
{
  /*
   * Divide this into 2 parts...one for media and one for image
   */
  if (wParam == AVK_0)
  {
      // Check if Fullscreen is available
      CtlAddItem pai;
      if ( IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_FULLSCRN, &pai) )
      {
          (void)CPlayerWin_FullScreen(pme, (boolean) ! pme->m_bFullScreen, FALSE);
          IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_FULLSCRN);
      }
  }

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  else if (wParam == AVK_STAR)
  {
    /*
     * Start recording
     */
    DBGPRINTF_MED("MP: Key *: Start Recording");
    CPlayerWin_StartStreamRecord(pme, TRUE);
  }
  else if (wParam == AVK_POUND)
  {
    /*
     * Stop recording
     */
    DBGPRINTF_MED("MP: Key #: Stop Recording");
    CPlayerWin_StopStreamRecord(pme);
  }
#endif     // FEATURE_QTV_STREAM_RECORD

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    else if (wParam == AVK_8)
    {
    if (pme->m_pMedia)
    {
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
      {
        AEECLSID  ClsId;
        char* pszFileName;

        if (IMEDIA_GetClassID(pme->m_pMedia, &ClsId) == SUCCESS && ClsId == AEECLSID_MEDIAMPEG4)
        {
          if (pme->m_pOwner->m_bProgDLFileSaveLoc)
          {
            pszFileName = MP_PATH_SAVELOC_FLASH;
          }
          else
          {
            pszFileName = MP_PATH_SAVELOC_MMC;
          }
          (void)IMEDIA_SetMediaParm(pme->m_pMedia,
                       MM_MP4_PARAM_COPY_BUFFER_TO_FILE,
                       (int32)pszFileName, 1);
          DBGPRINTF_MED("MP:  Key 8 pressed, Save to file %s",
                        fileSaveInfo.pszFileName);
        }
      }
#endif //FEATURE_APP_MEDIA_VIDEO


#ifdef MP_FEATURE_ICONTENT
      if(pme->m_pOwner->m_bIContentMode)
      {
        // save recorded file into IContent DB
        MP_AddFileToIContentDB(pme->m_pOwner,
                          MP_ICONTENT_CAT_NAMR_RECORD, MP_ICONTENT_CAT_DESC_RECORD,
                          "media/pds2.mp4", MP_ICONTENT_CAT_DESC_RECORD
                          MT_VIDEO_MPEG4);
      }
#endif //MP_FEATURE_ICONTENT

      }
    }
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

  else if (CPlayerWin_IsImage(pme))
  {
    if (wParam != AVK_SELECT)
      return IIMAGECTL_HandleEvent(pme->m_pImageCtl, eCode, wParam, dwParam);
    else
      return IMENUCTL_HandleEvent(pme->m_pPlayerMenu, eCode, wParam, dwParam);
  }
  else if (CPlayerWin_IsMedia(pme))
  {
    if (wParam == AVK_UP)
    {
      int     nRet;
      uint16  nNextLevel = pme->m_pOwner->m_wVolumeLevel + 1;
      uint16  wVol = (pme->m_pOwner->m_wNumVolumeLevels <= nNextLevel)? AEE_MAX_VOLUME : (nNextLevel * pme->m_pOwner->m_VolumeStep) + 2;

      wVol = (wVol > AEE_MAX_VOLUME) ? AEE_MAX_VOLUME : wVol;
      nRet  = IMEDIA_SetVolume(pme->m_pMedia, (uint16)wVol);
      DBGPRINTF_MED("MP: IMEDIA_SetVolume, vol = %d, nRet = %d[%s]", wVol, nRet, MP_ErrorStateToString(nRet));
      if (SUCCESS == nRet)
      {
        CPlayerWin_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME,
                                 MM_STATUS_DONE, NULL, 0);
      }
      return TRUE;
    }
    else if (wParam == AVK_DOWN)
    {
      int    nRet;
      uint16 nNextLevel = 0;
      int16  wVol;

      if(pme->m_pOwner->m_wVolumeLevel > 0)
      {
        nNextLevel = pme->m_pOwner->m_wVolumeLevel - 1;
      }
      wVol = (nNextLevel * pme->m_pOwner->m_VolumeStep) + 2;

      nVol = (nVol < 0) ? 0 : nVol;
      nRet  = IMEDIA_SetVolume(pme->m_pMedia, (uint16)nVol);
      DBGPRINTF_MED("MP: IMEDIA_SetVolume, vol = %d, nRet = %d[%s]", nVol, nRet, MP_ErrorStateToString(nRet));
      if (SUCCESS == nRet)
      {
        CPlayerWin_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME,
                                 MM_STATUS_DONE, NULL, 0);
      }
      return TRUE;
    }
    return IMENUCTL_HandleEvent(pme->m_pPlayerMenu, eCode, wParam, dwParam);
  }
  return TRUE;
}
#endif


/*===========================================================================
   This function processes the menu commands (event code EVT_COMMAND).
===========================================================================*/

static boolean
CPlayerWin_HandleEventCommand(CPlayerWin *pme, uint16 wParam, uint32 dwParam)
{
  int ret = SUCCESS;

  if (CPlayerWin_IsMedia(pme) && !pme->m_pMedia)
  {
    DBGPRINTF_ERROR("MP: m_pMedia is NULL.");
    return TRUE;
  }

  /*
   * If image, handle the image event commands.
   */
  if (CPlayerWin_IsImage(pme))
  {
    return CPlayerWin_HandleImageEventCommand(pme, wParam, dwParam);
  }

  /*
   * remove Info screen for any other button
   */
  if ((IDM_PM_INFO != wParam) && (pme->m_bDIsplayInfo))
  {
    if(!pme->m_pAudioSpec)
      (void)CPlayerWin_Info(pme, TRUE);
  }
  /*
   * Process EVT_COMMAND...
   */
  switch (wParam)
  {
  case ID_EVENT_SUSPEND:
  {
    int32     classId;
    boolean   isQtvMedia = FALSE;
    boolean   isRealMedia = FALSE;

    ret = IMEDIA_GetClassID(pme->m_pMedia, (AEECLSID *)&classId);
    if(SUCCESS == ret && MM_PENDING != ret)
    {

#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
      isQtvMedia = (classId == AEECLSID_MEDIAMPEG4);
#endif //FEATURE_APP_MEDIA_VIDEO

#ifdef FEATURE_APP_REAL
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else
      isRealMedia = (classId == AEECLSID_MEDIAREAL);
#endif //FEATURE_APP_MEDIA_VIDEO
#endif     // FEATURE_APP_REAL
    }
    else
      DBGPRINTF_ERROR("MP: failed on IMEDIA_GetClassID, %d.", ret);

    if(isQtvMedia ||isRealMedia)
    {
      DBGPRINTF_MED("MP: Suspending QTV media or Real media.");
      CMediaPlayer_ProcessSuspendEvent(pme->m_pOwner);
    }
    else
    {
      DBGPRINTF_ERROR("MP: feature is not supported for ClassID, %d.", ret);
      (void)ISHELL_Beep(pme->m_pOwner->a.m_pIShell, BEEP_ERROR, FALSE);
      (void)ISHELL_Beep(pme->m_pOwner->a.m_pIShell, BEEP_ERROR, FALSE);
    }
    break;
  }
  case ID_EVENT_RESUME:
  {
    int32     classId;
    boolean   isQtvMedia = FALSE;
    boolean   isRealMedia = FALSE;

    ret = IMEDIA_GetClassID(pme->m_pMedia, (AEECLSID *)&classId);
    if(SUCCESS == ret && MM_PENDING != ret)
    {

#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
      isQtvMedia = (classId == AEECLSID_MEDIAMPEG4);
#endif //FEATURE_APP_MEDIA_VIDEO

#ifdef FEATURE_APP_REAL
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else
      isRealMedia = (classId == AEECLSID_MEDIAREAL);
#endif //FEATURE_APP_MEDIA_VIDEO
#endif     // FEATURE_APP_REAL
    }
    else
      DBGPRINTF_ERROR("MP: failed on IMEDIA_GetClassID, %d.", ret);

    if(isQtvMedia ||isRealMedia)
      CMediaPlayer_ProcessResumeEvent(pme->m_pOwner);
    else
    {
      DBGPRINTF_ERROR("MP: feature is not supported for ClassID, %d.", ret);
      (void)ISHELL_Beep(pme->m_pOwner->a.m_pIShell, BEEP_ERROR, FALSE);
      (void)ISHELL_Beep(pme->m_pOwner->a.m_pIShell, BEEP_ERROR, FALSE);
    }
    break;
  }
  case ID_EVENT_CONTINUOUS:
    DBGPRINTF_MED("MP: ID_EVENT_CONTINUOUS event received.");

    if(CPlayerWin_PlayNext(pme, pme->m_pOwner->m_nContMode, (boolean)dwParam))
    {
      DBGPRINTF_LOW("MP: Continuous mode on, continuing...");
    }
    else
    {
      DBGPRINTF_ERROR("MP: Continuous mode replay failed.");
    }
    break;
  case IDM_PM_PLAY:
    {
#if (defined(FEATURE_APP_MPEG4) && defined(MP_FEATURE_PLAY_LIVE))
    boolean bResumeInProcessing;
    bResumeInProcessing = pme->m_pOwner->m_bResumeInProcessing;
#endif // (defined(FEATURE_APP_MPEG4) && defined(MP_FEATURE_PLAY_LIVE))

    DBGPRINTF_MED("MP: IDM_PM_PLAY event received.");

    if(pme->m_pOwner->m_bResumeInProcessing && pme->m_pOwner->m_bSuspendPending)
    {
      // we have a SUSPEND request during the resuming, so abort this
      // replay request
      pme->m_pOwner->m_bResumeInProcessing = FALSE;
      pme->m_pOwner->m_bSuspendPending = FALSE;

      // If playing make sure state is checked and updated so suspend will work correctly.
      if (CPlayerWin_IsPlaying(pme))
      {
        pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
        EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      }

      // set resume pending flag to TRUE, so we can resume again
      pme->m_pOwner->m_bResumePending = TRUE;
      DBGPRINTF_MED("MP: SUSPEND pending, abort play request.");

      break;
    }
    // Don't clear the resume/suspend flags until resume status from play is received
    // Set the bResumeInProcessing flag to true.  This will force any suspend events
    // that arrive to wait until the PLAY event has completed processing before running.
    pme->m_pOwner->m_bResumeInProcessing = TRUE;
    pme->m_bStopPressed = FALSE;

    if ((pme->m_pOwner->m_playState == MP_PLAYSTATE_STOPPED) && pme->m_bStoppedWhileConnecting)
    {
      // This play request follows a stop while connecting.
      // Rerun previous play request.  
      pme->m_bStoppedWhileConnecting = FALSE;
      CPlayerWin_Replay(pme);
      break;
    }

    if (CPlayerWin_IsPause(pme))
    {
#if (defined(FEATURE_APP_MPEG4) && defined(MP_FEATURE_PLAY_LIVE))
      if(pme->m_bGotMediaInfo)
      {
        if((MM_MEDIA_LIVE_STREAMING == pme->m_mediaInfo.streamtype) && !bResumeInProcessing)
        {
          /* 
           * This is the case where live streaming is paused or interrupted and then a Play request is made.
           * In this case, the stream is restarted at its current point in time.  
           * i.e. if interrupted at time t and Play is requested 5 seconds later, streaming resumes at t+5.
           */
          DBGPRINTF_HIGH("MP: streaming resume live.");
          ret = IMEDIA_Play(pme->m_pMedia);
          if(ret != SUCCESS)
          {
            // Imedia may be busy
            if(ret != EITEMBUSY)
            {
              // display error
              CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
              DBGPRINTF_ERROR("MP: failed to Resume live, returns %d = %s.",
                              ret, MP_ErrorStateToString(ret));
            }
            else
            {
              DBGPRINTF_HIGH("MP: failed to Resume live, IMedia is busy, %d = EITEMBUSY.", ret);
            }
          }
          else
          {
            DBGPRINTF_MED("MP: IMEDIA_Play returns %d = %s", ret, MP_ErrorStateToString(ret));
            pme->m_pOwner->m_bPlayAfterResume = FALSE;
          }
        }
        else
        {
          if(pme->m_pMedia1)
          {
            ret = IMEDIA_Resume(pme->m_pMedia1);
            if(ret != SUCCESS)
            {
              // Imedia may be busy
              if(ret != EITEMBUSY)
              {
                // display error
                CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
                DBGPRINTF_ERROR("MP: failed to Resume dl, returns %d = %s.",
                                ret, MP_ErrorStateToString(ret));
              }
              else
                DBGPRINTF_HIGH("MP: failed to Resume dl, IMedia is busy, %d = EITEMBUSY.", ret);
            }
            else
            {
              DBGPRINTF_MED("MP: IMEDIA_Resume on dl returns %d = %s", ret, MP_ErrorStateToString(ret));
            }
          }

          ret = IMEDIA_Resume(pme->m_pMedia);
          if(ret != SUCCESS)
          {
            // Imedia may be busy
            if(ret != EITEMBUSY)
            {
              // display error
              CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
              DBGPRINTF_ERROR("MP: failed to Resume, returns %d = %s.",
                              ret, MP_ErrorStateToString(ret));
            }
            else
            {
              DBGPRINTF_HIGH("MP: failed to Resume, IMedia is busy, %d = EITEMBUSY.", ret);
            }
          }
          else
          {
            DBGPRINTF_MED("MP: IMEDIA_Resume returns %d = %s", ret, MP_ErrorStateToString(ret));
            pme->m_pOwner->m_bPlayAfterResume = FALSE;
          }
        }

      }
      else
#endif //#if defined(FEATURE_APP_MPEG4) && defined(MP_FEATURE_PLAY_LIVE)
      {
        if(pme->m_pMedia1)
        {
          ret = IMEDIA_Resume(pme->m_pMedia1);
          if(ret != SUCCESS)
          {
            // Imedia may be busy
            if(ret != EITEMBUSY)
            {
              // display error
              CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
              DBGPRINTF_ERROR("MP: failed to Resume dl, returns %d = %s.",
                              ret, MP_ErrorStateToString(ret));
            }
            else
              DBGPRINTF_HIGH("MP: failed to Resume dl, IMedia is busy, %d = EITEMBUSY.", ret);
          }
          else
          {
            DBGPRINTF_MED("MP: IMEDIA_Resume on dl returns %d = %s", ret, MP_ErrorStateToString(ret));
          }
        }

        ret = IMEDIA_Resume(pme->m_pMedia);
        if(ret != SUCCESS)
        {
          // Imedia may be busy
          if(ret != EITEMBUSY)
          {
            // display error
            CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
            DBGPRINTF_ERROR("MP: failed to Resume, returns %d = %s.",
                            ret, MP_ErrorStateToString(ret));
          }
          else
          {
            DBGPRINTF_HIGH("MP: failed to Resume, IMedia is busy, %d = EITEMBUSY.", ret);
          }
        }
        else
        {
          DBGPRINTF_MED("MP: IMEDIA_Resume returns %d = %s", ret, MP_ErrorStateToString(ret));
          pme->m_pOwner->m_bPlayAfterResume = FALSE;
        }
      }
    }
    else
    {
      if (pme->m_eWin == MPPW_RECORD)
      {
        /*
         * check if file exists
         */
        if (!MP_IsExist(pme->m_pOwner, NULL, pme->m_pszFile))
        {
          char buf[MP_MAX_FILE_NAME + 1];
          if(!MP_InsertStringToFileName(pme->m_pszFile, "_ul", buf, MP_MAX_FILE_NAME + 1) &&
             !MP_InsertStringToFileName(pme->m_pszFile, "_dl", buf, MP_MAX_FILE_NAME + 1)
            )
          {
            /*
             * Display and announce error
             */
            (void)ISHELL_Beep(pme->m_pIShell, BEEP_ERROR, FALSE);
            CPlayerWin_UpdateProgCtl(pme, MM_CMD_RECORD, MM_STATUS_SPACE_ERROR, IDS_FILE_NOT_EXIST);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION
            break;
          }
        }
      }

      // cannot be set to play when it is in MP_PLAYSTATE_CONNECTING
      if(pme->m_pOwner->m_playState == MP_PLAYSTATE_CONNECTING)
      {
        DBGPRINTF_HIGH("MP: cannot play when MP is in CONNECTING state.");
        pme->m_pOwner->m_bResumeInProcessing = FALSE;
        break;
      }

      // Ignore play key presses if already playing or displaying an image.
      if(CPlayerWin_IsPlaying(pme) || CPlayerWin_IsRecording(pme) || CPlayerWin_IsImage(pme))
      {
        DBGPRINTF_MED("MP: Ignoring PLAY key press since already playing or recording");
        pme->m_pOwner->m_bResumeInProcessing = FALSE;
        break;
      }

      ret = CPlayerWin_GetTotalTimeRequest(pme);
      if ((ret != SUCCESS) && (ret != EITEMBUSY))
      {
        CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
      }

      DBGPRINTF_MED("MP: (SUCCESS != ret && MM_PENDING != ret, ret = %d) || CPlayerWin_IsPacketVideo(pme->m_pszFile) %d",
                    ret, CPlayerWin_IsPacketVideo(pme->m_pszFile));

      if ((SUCCESS != ret && MM_PENDING != ret) || CPlayerWin_IsPacketVideo(pme->m_pszFile))
      {
        if (pme->m_bNoTextOrHeader)
        {
          DBGPRINTF_MED("MP: m_bNoTextOrHeader = TRUE");
          CPlayerWin_EraseHeaderAndText(pme);
        }

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)  && \
    defined(FEATURE_QTV_DIAG_CMD)
        /*
         * For the stream recording window, play will not start the record.
         */
        if (pme->m_pOwner->m_bIsStreamRecord == TRUE)
          bRTSPstreamRecord = FALSE;

        DBGPRINTF_MED("MP: pme->m_mediaInfo.streamtype = %d", pme->m_mediaInfo.streamtype);

        if ((bRTSPstreamRecord) &&     // Stream record flag is set to
                                       // TRUE
            (pme->m_mediaInfo.streamtype == MM_MEDIA_LIVE_STREAMING ||
             pme->m_mediaInfo.streamtype == MM_MEDIA_STREAMING))
        {
          /*
           * Start stream recording
           */
          CPlayerWin_StartStreamRecord(pme, bRTSPstreamRecFileOverwrite);
          (void)STRLCPY(recFileName, "", sizeof(recFileName));
        }
#endif

        if(pme->m_pMedia1)
        {
          ret = IMEDIA_Play(pme->m_pMedia1);
          if(ret != SUCCESS)
          {
            if(ret == EITEMBUSY)
            {
              DBGPRINTF_HIGH("MP: failed on IMEDIA_Play for dl, IMedia is busy,  returns %d = %s.",
                              ret, MP_ErrorStateToString(ret));
            }
            else
            {
              DBGPRINTF_ERROR("MP: failed on IMEDIA_Play for dl, returns %d = %s.",
                              ret, MP_ErrorStateToString(ret));
              CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
            }
          }
          else
          {
            DBGPRINTF_MED("MP: IMEDIA_Play on d1 returns %d = %s", ret, MP_ErrorStateToString(ret));
          }

        }
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        if (pme->psHttpBlock  && !pme->psHttpBlock->bWebPstreaming && PS_STATE_COMPLETE != pme->psHttpBlock->web_download_state)
        {
          // We are trying to play a Web file, but we have not completed the download and we are currently not Streaming
          // so re-create the web.
          DBGPRINTF_MED("MP: Reconnecting to the web");

          if (http_ps_SetUpWeb(pme->psHttpBlock))
          {
            http_ps_get_chunk(&pme->psHttpBlock);
          }
          else
          {
            DBGPRINTF_HIGH("MP: http_ps_SetUpWeb() returns FALSE");
          }
        }
#endif //#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#endif //#ifdef FEATURE_APP_MPEG4


        // Because of a hack to clear the last frame of video when
        // a clip stops (by settings the display rect to 0,0,0,0 for the media)
        // we need to re-set the ImageRect so if they re-hit play, it will display
        // This can also happen if a streaming clip is attempted to be played 
        // While we are in a call
        DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with x = %d, y = %d, cx = %d, cy = %d", pme->m_rectImage.x, pme->m_rectImage.y, pme->m_rectImage.dx, pme->m_rectImage.dy);
        IMEDIA_SetRect(pme->m_pMedia, &(pme->m_rectImage), NULL);

        ret = IMEDIA_Play(pme->m_pMedia);
        if (AEE_SUCCESS == ret)
        {
          DBGPRINTF_MED("MP: IMEDIA_Play returns %d = %s", ret, MP_ErrorStateToString(ret));
        }
        else
        {
          if(ret == EITEMBUSY)
          {
            DBGPRINTF_HIGH("MP: failed on IMEDIA_Play, IMedia is busy,  returns %d = %s.",
                            ret, MP_ErrorStateToString(ret));
          }
          else
          {
            DBGPRINTF_ERROR("MP: failed on IMEDIA_Play, returns %d = %s.",
                            ret, MP_ErrorStateToString(ret));
            CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
          }
        }
      }
      else
      {
        pme->m_bPlayPending = TRUE;
        DBGPRINTF_MED("MP: setting m_bPlayPending = TRUE");
        pme->m_pOwner->m_bPlayAfterResume = FALSE;
      }

      DBGPRINTF_MED("MP: Resetting seek values");
      pme->m_seekPending = pme->m_pendingSeekOffset = pme->m_reseek = 0;
      pme->m_startSeekPosition = 0;
      pme->m_nextClipOffsetAdj = 0;
      pme->m_skipPending = FALSE;
      pme->m_skipOffset = 0;
      pme->m_reskip = FALSE;

      // Set Rect based on player
      if (CPlayerWin_IsMedia(pme))
      {
        DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with x = %d, y = %d, cx = %d, cy = %d", pme->m_rectImage.x, pme->m_rectImage.y, pme->m_rectImage.dx, pme->m_rectImage.dy);
        (void)IMEDIA_SetRect(pme->m_pMedia, &pme->m_rectImage, NULL);

#ifdef FEATURE_QTV_MDP_ASCALE //
      // Do AScaling if the media file is not an image.
      if (!pme->m_pOwner->m_bIsImage)
      {
        (void)CPlayerWin_SetAScale( pme );
      }
#endif  // FEATURE_QTV_MDP_ASCALE

        // Force redraw for overlays.
        CPlayerWin_Redraw((IWindow *)pme);
      }
    }

    // If our play/resume failed then make sure these flags are cleared.
    if(ret != SUCCESS)
    {
      pme->m_pOwner->m_bResumeInProcessing = FALSE;
      pme->m_pOwner->m_bSuspendPending = FALSE;
    }

    // set volume for both play and resume
    pme->m_wVolume = (pme->m_pOwner->m_wNumVolumeLevels == pme->m_pOwner->m_wVolumeLevel)? AEE_MAX_VOLUME : (pme->m_pOwner->m_wVolumeLevel * pme->m_pOwner->m_VolumeStep) + 2;
    ret = IMEDIA_SetVolume(pme->m_pMedia, pme->m_wVolume);
    DBGPRINTF_MED("MP: IMEDIA_SetVolume = %d, returns %d = %s", pme->m_wVolume, ret, MP_ErrorStateToString(ret));
    if(SUCCESS != ret && MM_PENDING != ret)
      DBGPRINTF_ERROR("MP: failed on IMEDIA_SetVolume, returns %d = %s.", ret, MP_ErrorStateToString(ret));
    else
      CPlayerWin_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME, MM_STATUS_DONE, NULL, 0);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    }
    break;

  case IDM_PM_RECORD:
  {
    int       ret = EFAILED;

    DBGPRINTF_MED("MP: IDM_PM_RECORD event received.");
    pme->m_bMemoryFull = FALSE;
    if (CPlayerWin_IsPause(pme))
    {
      if(pme->m_pMedia1)
      {
        ret = IMEDIA_Resume(pme->m_pMedia1);
        if(ret != SUCCESS)
        {
          switch (ret)
          {
            /*
             * case MM_EBADSTREAM:
             *   DBGPRINTF_ERROR("MP: IMEDIA_Record error, Bad stream.");
             *   break;
             */
          case EBADPARM:
            DBGPRINTF_ERROR("MP: IMEDIA_Record dl resume error, Bad parm in stream.");
            break;
          case EBADSTATE:
            DBGPRINTF_ERROR("MP: IMEDIA_Record dl resume error, Cannot play in the current state .");
            break;
          case EITEMBUSY:
            DBGPRINTF_ERROR("MP: IMEDIA_Record dl resume error, IMedia is currently busy. Try after some time.");
            break;
          case EUNSUPPORTED:
            DBGPRINTF_ERROR("MP: IMEDIA_Record dl resume error, API not supported by the class.");
            break;
          default:
            DBGPRINTF_ERROR("MP: IMEDIA_Record dl resume error, Unknown reason, error code %d.", ret);
            break;
          }
        }
        else
        {
          DBGPRINTF_MED("MP: IMEDIA_Resume on d1 returns %d = %s", ret, MP_ErrorStateToString(ret));
        }
      }
      ret = IMEDIA_Resume(pme->m_pMedia);
      if(ret != SUCCESS)
      {
        switch (ret)
        {
          /*
           * case MM_EBADSTREAM:
           *   DBGPRINTF_ERROR("MP: IMEDIA_Record error, Bad stream.");
           *   break;
           */
        case EBADPARM:
          DBGPRINTF_ERROR("MP: IMEDIA_Record resume error, Bad parm in stream.");
          break;
        case EBADSTATE:
          DBGPRINTF_ERROR("MP: IMEDIA_Record resume error, Cannot play in the current state .");
          break;
        case EITEMBUSY:
          DBGPRINTF_ERROR("MP: IMEDIA_Record resume error, IMedia is currently busy. Try after some time.");
          break;
        case EUNSUPPORTED:
          DBGPRINTF_ERROR("MP: IMEDIA_Record resume error, API not supported by the class.");
          break;
        default:
          DBGPRINTF_ERROR("MP: IMEDIA_Record resume error, Unknown reason, error code %d.", ret);
          break;
        }
      }
      else
      {
        DBGPRINTF_MED("MP: IMEDIA_Resume returns %d = %s", ret, MP_ErrorStateToString(ret));
        pme->m_pOwner->m_bPlayAfterResume = FALSE;
      }
    }
    else
    {
      if(pme->m_pMedia1)
      {
        if(!CPlayerWin_PurgeDownloadedFile(pme))
        {
          ret = IMEDIA_Record(pme->m_pMedia1);
          if(ret != SUCCESS)
          {
            switch (ret)
            {
              /*
               * case MM_EBADSTREAM:
               *   DBGPRINTF_ERROR("MP: IMEDIA_Record error, Bad stream.");
               *   break;
               */
            case EBADPARM:
              DBGPRINTF_ERROR("MP: IMEDIA_Record dl record error, Bad parm in stream.");
              break;
            case EBADSTATE:
              DBGPRINTF_ERROR("MP: IMEDIA_Record dl record error, Cannot play in the current state .");
              break;
            case EITEMBUSY:
              DBGPRINTF_ERROR("MP: IMEDIA_Record dl record error, IMedia is currently busy. Try after some time.");
              break;
            case EUNSUPPORTED:
              DBGPRINTF_ERROR("MP: IMEDIA_Record dl record error, API not supported by the class.");
              break;
            default:
              DBGPRINTF_ERROR("MP: IMEDIA_Record dl record error, Unknown reason, error code %d.", ret);
              break;
            }
          }
          else
          {
            DBGPRINTF_MED("MP: IMEDIA_Resume returns %d = %s", ret, MP_ErrorStateToString(ret));
          }
        }
      }
      if(!CPlayerWin_PurgeDownloadedFile(pme))
      {
        ret = IMEDIA_Record(pme->m_pMedia);
        if (ret == SUCCESS)
        {
          CPlayerWin_AdjustRecordMenu(pme);
        }
      }
      else
      {
          CPlayerWin_AdjustRecordMenu(pme);
      }
    }

    if (ret != SUCCESS)
    {
      CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
      switch (ret)
      {
        /*
         * case MM_EBADSTREAM:
         *   DBGPRINTF_ERROR("MP: IMEDIA_Record error, Bad stream.");
         *   break;
         */
      case EBADPARM:
        DBGPRINTF_ERROR("MP: IMEDIA_Record error, Bad parm in stream.");
        break;
      case EBADSTATE:
        DBGPRINTF_ERROR
          ("MP: IMEDIA_Record error, Cannot play in the current state .");
        break;
      case EITEMBUSY:
        DBGPRINTF_ERROR
          ("MP: IMEDIA_Record error, IMedia is currently busy. Try after some time.");
        break;
      case EUNSUPPORTED:
        DBGPRINTF_ERROR
          ("MP: IMEDIA_Record error, API not supported by the class.");
        break;
      default:
        DBGPRINTF_ERROR
          ("MP: IMEDIA_Record error, Unknown reason, error code %d.",
           ret);
        break;
      }
    }
    else
    {
      DBGPRINTF_MED("MP: IMEDIA_Record returns %d = %s", ret, MP_ErrorStateToString(ret));
    }
    break;
  }
#ifdef FEATURE_MP_RECORDER_SETTINGS
  case IDM_PM_RECORDER_SETTINGS:
  {
    AEEMediaData md;
    char     *pFile = MALLOC(MP_MAX_FILE_NAME);

    DBGPRINTF_MED("MP: IDM_PM_RECORDER_SETTINGS event received.");
    if (!pFile)
      return FALSE;
    (void)STRLCPY(pFile, pme->m_pszFile, MP_MAX_FILE_NAME);
    if (!CMediaPlayer_SetWindow(pme->m_pOwner, MPW_RECORDER_SETTINGS, 0))
    {
      DBGPRINTF_ERROR("MP: Error on creating recorder setting window.");
      /*
       * back to recorder window
       */

      md.clsData = MMD_FILE_NAME;
      md.pData = (void *)pFile;
      md.dwSize = 0;
      (void)CMediaPlayer_CreateRecorderWindow(pme->m_pOwner, &md);
      return FALSE;
    }
  }
    break;
#endif
#ifdef FEATURE_MP_EQ_UI
  case IDM_PM_PLAYERWIN_SETTINGS:
    {
      DBGPRINTF_MED("MP: IDM_PM_PLAYERWIN_SETTINGS event received.");
      CPlayerWin_ShowSettingWin(pme);
    }
    break;
#endif //FEATURE_MP_EQ_UI
  case IDM_PM_STOP:
    DBGPRINTF_MED("MP: IDM_PM_STOP event received.");
    pme->m_bStopPressed = TRUE;

    if(pme->m_pMedia1)
    {
      ret = IMEDIA_Stop(pme->m_pMedia1);
      if (ret != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: failed on IMEDIA_Stop Media1, returns %d = %s.",
                      ret, MP_ErrorStateToString(ret));
      }
    }

    ret = IMEDIA_Stop(pme->m_pMedia);
    if (ret == SUCCESS)
    {
      if(pme->m_pOwner->m_playState == MP_PLAYSTATE_CONNECTING)
      {
        // If stopped while connecting, will need to release media and create again
        // before restarting.
        pme->m_bStoppedWhileConnecting = TRUE;
      }
      pme->m_pOwner->m_playState = MP_PLAYSTATE_STOPPED;
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
    }
    else
    {
      if ((ret == EBADSTATE) && (pme->m_pOwner->m_playState == MP_PLAYSTATE_CONNECTING))
      {
        // If in connecting state, may not be able to pause/stop media 
        // If stop fails, terminate playback
        DBGPRINTF_MED("MP: In MP_PLAYSTATE_CONNECTING state, IMEDIA_Stop failed, %d", ret);
        // Clear current connecting status
        ConnectStatusStringActive = 0;
        // Save info needed to resume media later
        pme->m_pOwner->m_bResumePending = FALSE;
        pme->m_pOwner->m_resumePlayPos = -1;
        pme->m_pOwner->m_eSuspendWin = pme->m_pOwner->m_eActiveWin;
        pme->m_pOwner->m_wSuspendVolume = pme->m_pOwner->m_wVolumeLevel;
        pme->m_pOwner->m_bNeedReload = FALSE;
        pme->m_pOwner->m_bPlayAfterResume = FALSE;

        (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_NONE, 0);
        break;
      }
      else 
      {
        DBGPRINTF_ERROR("MP: failed on IMEDIA_Stop, returns %d = %s.", ret, MP_ErrorStateToString(ret));
      }
    }
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    // Stop any Web downloading that might be going on in the background
    if (pme->psHttpBlock && pme->psHttpBlock->bWebPstreaming)
    {
      // Or do I want PS_CleanHttpDownload?
      PS_Free_web(pme->psHttpBlock);
      pme->m_pOwner->m_bReconnectWebOnResume = TRUE;
    }
#endif // FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif //FEATURE_APP_MPEG4

    if (pme->m_eWin == MPPW_RECORD)
    {
      CPlayerWin_DrawRecordMenu(pme);

#ifdef MP_FEATURE_ICONTENT
      // save recorded file into IContent DB
      if(pme->m_pOwner->m_bIContentMode && pme->m_pszFile)
      {
        MP_AddFileToIContentDB(pme->m_pOwner,
                          MP_ICONTENT_CAT_NAME_MEMO, MP_ICONTENT_CAT_DESC_MEMO,
                          pme->m_pszFile, MP_ICONTENT_CAT_DESC_MEMO,
                          MT_AUDIO_QCP);
      }
#endif //MP_FEATURE_ICONTENT

    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    break;

  case IDM_PM_REWIND:
  case IDM_PM_FF:
    if (wParam == IDM_PM_REWIND)
    {
      DBGPRINTF_MED("MP: IDM_PM_REWIND event received.");
  	}
    else
    {
	    DBGPRINTF_MED("MP: IDM_PM_FF event received.");
  	}

    if (CPlayerWin_IsPause(pme) && pme->m_skipPending)
    {
      DBGPRINTF_ERROR("MP: Cannot Seek when paused and a pending skip");
      (void)ISHELL_Beep(pme->m_pOwner->a.m_pIShell, BEEP_ERROR, FALSE);
      (void)ISHELL_Beep(pme->m_pOwner->a.m_pIShell, BEEP_ERROR, FALSE);
      break;
    }
   
    if (CPlayerWin_IsRecording(pme))
    {
      DBGPRINTF_HIGH("MP: Cannot Seek when Recording");
      break;
    }

    if (CPlayerWin_IsReady(pme))
    {
      DBGPRINTF_HIGH("MP: Cannot Seek when Media is in the Ready state");
      break;
    }

    {
      int       direction = (wParam == IDM_PM_FF) ? 1 : -1;

#ifdef MP_FEATURE_VIDEO
      int       nRet = SUCCESS;
      boolean   allowSeek = TRUE;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      /* FastTrack clip information is available after receiving an MM_MP4_STATUS_PB_READY.   */
      /* Don't allow seeking until then and check the clip info to see if seeking is allowed. */
      if (pme->m_pOwner->m_bPacketVideoStreaming == FALSE)
        allowSeek = TRUE;
      else if (pme->m_pOwner->m_bVideoPlaybackReady == TRUE)
      {
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined (MP_USE_NEWER_BUILD_ITEMS) && defined (FEATURE_APP_MPEG4)
        AEEMediaMPEG4Spec *pMediaInfo = &pme->m_mediaInfo;
        DBGPRINTF_MED("MP:  FastTrack bAllowUserPlayback = %d", pMediaInfo->ExtendedInfo.PvxSpec.bAllowUserPlayback);
        if(pMediaInfo->eFormatType == MM_PVX_FORMAT_TYPE)
        {
          if (pMediaInfo->ExtendedInfo.PvxSpec.bAllowUserPlayback == TRUE)
            allowSeek = TRUE;
          else
            allowSeek = FALSE;
        }
#else
        DBGPRINTF_MED("MP:  Not checking FastTrack flags, allowing reposition");
        allowSeek = TRUE;
#endif
      }
      else
          allowSeek = FALSE;

      DBGPRINTF_MED("MP:  FastTrack setting allowSeek = %d", allowSeek);
#endif
      if (allowSeek)
      {
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
      int       ClsId;
      if ((IMEDIA_GetClassID(pme->m_pMedia, (AEECLSID *)&ClsId) == SUCCESS)
          && ClsId == AEECLSID_MEDIAMPEG4)
#endif //FEATURE_APP_MPEG4
      {
        if (pme->m_skipPending)
        {
          // We are in the process of skiping files, build up and FF/REW that
          // occure during this process.
          int newOffset = pme->m_pendingSeekOffset + (direction * pme->m_pOwner->m_nSeekTime);
          DBGPRINTF_MED("MP: Skip pending, update the seekoffset from %d to %d", pme->m_pendingSeekOffset, newOffset);
          pme->m_pendingSeekOffset += direction * pme->m_pOwner->m_nSeekTime;
          pme->m_reseek = 1;
        }
        else if (pme->m_seekPending)
        {
          int newOffset = pme->m_pendingSeekOffset + (direction * pme->m_pOwner->m_nSeekTime);
          DBGPRINTF_MED("MP: Seek pending, update the seekoffset from %d to %d", pme->m_pendingSeekOffset, newOffset);
          pme->m_pendingSeekOffset += direction * pme->m_pOwner->m_nSeekTime;
          pme->m_reseek = 1;
        }
        else
        {
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
          AEEMediaPlaybackPos PlayPos;
#endif //FEATURE_APP_MEDIA_VIDEO
          if (CPlayerWin_IsPause(pme))
          {
            ret = IMEDIA_Seek(pme->m_pMedia, MM_SEEK_CURRENT, direction * pme->m_pOwner->m_nSeekTime);
            if(SUCCESS != ret)
            {
              if(ret == EITEMBUSY)
                DBGPRINTF_HIGH("MP: failed on IMEDIA_Seek, IMedia is busy, %d.", ret);
              else
                DBGPRINTF_ERROR("MP: failed on IMEDIA_Seek, returns %d = %s.",
                ret, MP_ErrorStateToString(ret));
            }
          }
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
          else if (IMEDIA_GetMediaParm(pme->m_pMedia, MM_MP4_PARAM_PLAYBACK_POS, (int32*)&PlayPos, NULL) == SUCCESS)
          {
            int seekAmount = 0;
            int currentPlayPos = 0;

            DBGPRINTF_MED("MP: dwPlaybackPos = %d", PlayPos.dwPlaybackPos);
            currentPlayPos = PlayPos.dwPlaybackPos;
            pme->m_seekPending = 1;

            seekAmount = direction * pme->m_pOwner->m_nSeekTime;
            

            // Don't try to rewind past start of clip, just seek to start.
            if ((currentPlayPos + seekAmount) < 0)
              nRet = IMEDIA_Seek(pme->m_pMedia, MM_SEEK_START, 0);
            else
              nRet = IMEDIA_Seek(pme->m_pMedia, MM_SEEK_CURRENT, seekAmount);
            DBGPRINTF_MED("MP: IMEDIA_Seek returns %d = %s", nRet, MP_ErrorStateToString(nRet));
            if (nRet != SUCCESS && nRet != EITEMBUSY)
            {
              if (EALREADY == nRet)
              {
                int newOffset = pme->m_pendingSeekOffset + (direction * pme->m_pOwner->m_nSeekTime);
                DBGPRINTF_MED("MP: Seek pending, update the seekoffset from %d to %d", pme->m_pendingSeekOffset, newOffset);
                pme->m_pendingSeekOffset += direction * pme->m_pOwner->m_nSeekTime;
                pme->m_reseek = 1;
              }
              else
              {
                pme->m_seekPending = pme->m_pendingSeekOffset = pme->m_nextClipOffsetAdj = 0;
                pme->m_startSeekPosition = 0;
                DBGPRINTF_ERROR("MP: failed on IMEDIA_Seek, reset position (%s).",  MP_ErrorStateToString(nRet));
              }
            }
            else
            {
              DBGPRINTF_MED("MP: setting m_pendingSeekOffset to %d", seekAmount);
              pme->m_pendingSeekOffset = seekAmount;
              pme->m_nextClipOffsetAdj = seekAmount;
              pme->m_startSeekPosition = currentPlayPos;
            }
          }
#endif //FEATURE_APP_MEDIA_VIDEO
        }
      }
      else
#endif     // MP_FEATURE_VIDEO
      {
        if(pme->m_pMedia1)
        {
          ret = IMEDIA_Seek(pme->m_pMedia1, MM_SEEK_CURRENT, direction * pme->m_pOwner->m_nSeekTime);
          if(SUCCESS != ret)
          {
            if(ret == EITEMBUSY)
              DBGPRINTF_HIGH("MP: failed on Media1 IMEDIA_Seek, Media1 is busy, %d = EITEMBUSY.", ret);
            else
              DBGPRINTF_ERROR("MP: failed on Media1 IMEDIA_Seek, %d = %s.",
              ret, MP_ErrorStateToString(ret));
          }
        }

        ret = IMEDIA_Seek(pme->m_pMedia, MM_SEEK_CURRENT, direction * pme->m_pOwner->m_nSeekTime);
        if(SUCCESS != ret)
        {
          if(ret == EITEMBUSY)
            DBGPRINTF_HIGH("MP: failed on IMEDIA_Seek, IMedia is busy, %d = EITEMBUSY.", ret);
          else
            DBGPRINTF_ERROR("MP: failed on IMEDIA_Seek, %d = %s.",
            ret, MP_ErrorStateToString(ret));
        }
      }
#ifdef MP_FEATURE_VIDEO
      }
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    }
    break;

  case IDM_PM_PAUSE:
    {
    boolean allowPause = TRUE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

    /*
     * Verify that BREW and MediaPlayer both agree on the playing state,
     * or else the state is in transition.  If in transition, ignore pause request.
     */
    DBGPRINTF_MED("MP: IDM_PM_PAUSE event received.");
    EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);

#ifdef MP_FEATURE_VIDEO
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    /* FastTrack clip information is available after receiving an MM_MP4_STATUS_PB_READY. */
    /* Don't allow pause until then and check the clip info to see if pause is allowed.   */
    if (pme->m_pOwner->m_bPacketVideoStreaming == FALSE)
      allowPause = TRUE;
    else if (pme->m_pOwner->m_bVideoPlaybackReady == TRUE)
    {
#if defined (MP_USE_NEWER_BUILD_ITEMS) && defined (FEATURE_APP_MPEG4)
      AEEMediaMPEG4Spec *pMediaInfo = &pme->m_mediaInfo;

      DBGPRINTF_MED("MP:  FastTrack bAllowUserPlayback = %d", pMediaInfo->ExtendedInfo.PvxSpec.bAllowUserPlayback);
      if(pMediaInfo->eFormatType == MM_PVX_FORMAT_TYPE)
      {
        if (pMediaInfo->ExtendedInfo.PvxSpec.bAllowUserPlayback == TRUE)
          allowPause = TRUE;
        else
          allowPause = FALSE;
      }
#else
      DBGPRINTF_MED("MP:  Not checking FastTrack flags, allowing PAUSE");
      allowPause = TRUE;
#endif
    }
    else
        allowPause = FALSE;

    DBGPRINTF_MED("MP:  FastTrack setting allowPause = %d", allowPause);
#endif  /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
#endif  /* MP_FEATURE_VIDEO */

   if (allowPause && CPlayerWin_IsPause(pme) && !CPlayerWin_IsMediaBusy(pme) &&
       ((pme->m_pOwner->m_playState == MP_PLAYSTATE_PAUSED) ||
        (pme->m_pOwner->m_playState == MP_PLAYSTATE_BUFFERING)))
    {
      int ret;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      DBGPRINTF_MED("MP: resuming, calling IMEDIA_Resume...");
      ret = IMEDIA_Resume(pme->m_pMedia);
      if (ret == SUCCESS)
      {
         CPlayerWin_SetMediaBusy(pme, MEDIA_BUSY_RESUME);
         pme->m_pOwner->m_bPlayAfterResume = FALSE;
      }
      else
      {
        DBGPRINTF_ERROR("MP: IMEDIA_Resume failed, returns %d = %s", ret, MP_ErrorStateToString(ret));
      }

	    //Removed Redraw after pause to be consistent with repositioning
      //CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      if (pme->m_b3GPPTTextOn && pme->m_dw3GPRemainTime)
      {
        (void)ISHELL_SetTimer(pme->m_pIShell, pme->m_dw3GPRemainTime,
                              (PFNNOTIFY)CPlayerWin_3GPPTimedTextExpiredNotify, pme);
      }
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT
    }
    else if (allowPause && CPlayerWin_IsPlaying(pme) && !CPlayerWin_IsMediaBusy(pme) &&
             (pme->m_pOwner->m_playState == MP_PLAYSTATE_PLAYING ||
              pme->m_pOwner->m_playState == MP_PLAYSTATE_BUFFERING))
    {
      int ret;
      DBGPRINTF_MED("MP: pausing, calling IMEDIA_Pause...");
      ret = IMEDIA_Pause(pme->m_pMedia);
      // pause mp when it is in either playing or buffering state
      if (ret == SUCCESS)
      {
         CPlayerWin_SetMediaBusy(pme, MEDIA_BUSY_PAUSE);
      }
      else
      {
        DBGPRINTF_ERROR("MP: IMEDIA_Pause failed, %d = %s",
                        ret, MP_ErrorStateToString(ret));
      }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      if (pme->m_b3GPPTTextOn)
      {
        pme->m_dw3GPRemainTime = ISHELL_GetTimerExpiration(pme->m_pIShell,
                                 (PFNNOTIFY)CPlayerWin_3GPPTimedTextExpiredNotify, pme);
        (void)ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)CPlayerWin_3GPPTimedTextExpiredNotify, pme);
      }
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT
    }
    else if (CPlayerWin_IsRecording(pme) && pme->m_pOwner->m_playState == MP_PLAYSTATE_RECORDING)
    {

      if ((pme->m_pMedia) && !CPlayerWin_IsMediaBusy(pme))
      {
        int ret;

        DBGPRINTF_MED("MP: pause pMedia recording, calling IMEDIA_Pause...");
        ret = IMEDIA_Pause(pme->m_pMedia);
        if (ret == SUCCESS)
        {
          CPlayerWin_SetMediaBusy(pme, MEDIA_BUSY_PAUSE);
        }
        else
        {
         DBGPRINTF_ERROR("MP: IMEDIA_Pause on pMedia failed, %d = %s", ret, MP_ErrorStateToString(ret));
        }
      }

      if (pme->m_pMedia1)
      {
        int ret;

        DBGPRINTF_MED("MP: pause pMedia1 recording, calling IMEDIA_Pause...");
        ret = IMEDIA_Pause(pme->m_pMedia1);
        if (ret != SUCCESS)
        {
           DBGPRINTF_ERROR("MP: IMEDIA_Pause on pMedia1 failed, %d = %s", ret, MP_ErrorStateToString(ret));
        }
      }
    }
    else
    {
      // If we have a resumePlayPos > 0 then assume we were paused and restarted
      // process a pause command as a play.
      if (pme->m_pOwner->m_resumePlayPos > 0)
      {
          DBGPRINTF_MED("MP: PAUSE command, with resumePlayPos set, posting IDM_PM_PLAY event");
          (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
      }
      else
      {
        DBGPRINTF_MED("MP: Ignore PAUSE command, current state: %d[%s]", pme->m_pOwner->m_playState,
                     MP_MediaPlayerPlayStateEnumToString(pme->m_pOwner->m_playState));
      }
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    }

    break;

  case IDM_PM_FF_STEP:
  {
    DBGPRINTF_MED("MP: IDM_PM_FF_STEP event received.");

    if (CPlayerWin_IsPlaying(pme) && !CPlayerWin_IsMediaBusy(pme))
    {
      if (SUCCESS == IMEDIA_Pause(pme->m_pMedia))
      {
        CPlayerWin_SetMediaBusy(pme, MEDIA_BUSY_PAUSE);
      }
    }
#ifdef FEATURE_QTV_GENERIC_BCAST
    else if (pme->m_pOwner->m_pSetting->m_bEnableBcastSettings && CPlayerWin_IsPause(pme) && !CPlayerWin_IsMediaBusy(pme))
    {
      CPlayerWin_Reposition(pme);
    }
#endif // FEATURE_QTV_GENERIC_BCAST
    else if (CPlayerWin_IsPause(pme)  && !CPlayerWin_IsMediaBusy(pme))
    {
      if (SUCCESS == IMEDIA_SeekFrame(pme->m_pMedia, MM_SEEK_CURRENT, 1))
      {
        CPlayerWin_SetMediaBusy(pme, MEDIA_BUSY_SEEK);
      }
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    break;
  }

  case IDM_PM_BK_STEP:
  {
    DBGPRINTF_MED("MP: IDM_PM_BK_STEP event received.");

    if (CPlayerWin_IsPlaying(pme) && !CPlayerWin_IsMediaBusy(pme))
    {
      if (SUCCESS == IMEDIA_Pause(pme->m_pMedia))
      {
        CPlayerWin_SetMediaBusy(pme, MEDIA_BUSY_PAUSE);
      }
    }
#ifdef FEATURE_QTV_GENERIC_BCAST
    else if (pme->m_pOwner->m_pSetting->m_bEnableBcastSettings && CPlayerWin_IsPause(pme) && !CPlayerWin_IsMediaBusy(pme))
    {
      CPlayerWin_Reposition(pme);
    }
#endif // FEATURE_QTV_GENERIC_BCAST
    else if (CPlayerWin_IsPause(pme)  && !CPlayerWin_IsMediaBusy(pme))
    {
      if (SUCCESS == IMEDIA_SeekFrame(pme->m_pMedia, MM_SEEK_CURRENT, -1))
      {
        CPlayerWin_SetMediaBusy(pme, MEDIA_BUSY_SEEK);
      }
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    break;
  }

#if defined(FEATURE_QTV_RANDOM_ACCESS_REPOS) || defined(FEATURE_QTV_PLAYLIST)
  case IDM_PM_FF_SKIP:
  case IDM_PM_BK_SKIP:
    {
#if MIN_BREW_VERSION(3,0)
      AEEMediaSeek skipReference = MM_SEEK_MODE_CHAPTER;
#else
      AEEMediaSeek skipReference = MM_SEEK_ACCESS_POINT;
#endif
      int direction = (wParam == IDM_PM_FF_SKIP) ? 1 : -1;
      int nRet;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */


      if (CPlayerWin_IsPause(pme) && pme->m_skipPending)
      {
        DBGPRINTF_ERROR("MP: Cannot Skip when paused and a pending skip");
        (void)ISHELL_Beep(pme->m_pOwner->a.m_pIShell, BEEP_ERROR, FALSE);
        (void)ISHELL_Beep(pme->m_pOwner->a.m_pIShell, BEEP_ERROR, FALSE);
        break;
      }


      if (direction < 0)
      {
        DBGPRINTF_MED("MP: IDM_PM_BK_SKIP event received.");
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */
      }
      else
      {
        DBGPRINTF_MED("MP: IDM_PM_FF_SKIP event received.");
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */
      }

      /* Performing a skip overrides any seeks waiting to be done. */
      if (pme->m_reseek)
      {
        DBGPRINTF_MED("MP: Resetting seek values");
        pme->m_pendingSeekOffset = 0;
        pme->m_nextClipOffsetAdj = 0;
        pme->m_startSeekPosition = 0;
        pme->m_reseek = FALSE;
      }

      /* If waiting for prior skip to complete, then accumulate skips and */
      /* "reskip", i.e. send the seek later when current skip is done.    */
      if (pme->m_skipPending)
      {
        DBGPRINTF_MED("MP: Skip pending, update the seekoffset from %d to %d", pme->m_skipOffset, pme->m_skipOffset + direction);
        pme->m_skipOffset += direction;
        pme->m_reskip = TRUE;
      }
      else
      {
        // This skip will be pending until we get a skip complete
        // message
        if (CPlayerWin_IsPlaying(pme) || CPlayerWin_IsPause(pme))
        {
          pme->m_skipPending = TRUE;
        }

        nRet = IMEDIA_Seek(pme->m_pMedia, skipReference | MM_SEEK_CURRENT,
                           (MP_ACCESS_POINT_SKIP_STEP * direction));
        DBGPRINTF_MED("MP: IMEDIA_Seek(%d) returns %d = %s",
          (MP_ACCESS_POINT_SKIP_STEP * direction), nRet, MP_ErrorStateToString(nRet));
        if (nRet != SUCCESS && nRet != EITEMBUSY)
        {
          pme->m_skipPending = FALSE;
          pme->m_skipOffset = 0;
          DBGPRINTF_ERROR("MP: IMEDIA_Seek failed!");
        }
      }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    }
    break;
#endif     // FEATURE_QTV_RANDOM_ACCESS_REPOS || FEATURE_QTV_PLAYLIST

  case IDM_PM_UP:
  {
    int     nRet;
    uint16  nNextLevel = pme->m_pOwner->m_wVolumeLevel + 1;
    uint16  wVol = (pme->m_pOwner->m_wNumVolumeLevels <= nNextLevel)? AEE_MAX_VOLUME : (nNextLevel * pme->m_pOwner->m_VolumeStep) + 2;

    DBGPRINTF_MED("MP: IDM_PM_UP event received.");
    wVol = (wVol > AEE_MAX_VOLUME) ? AEE_MAX_VOLUME : wVol;

    nRet  = IMEDIA_SetVolume(pme->m_pMedia, (uint16)wVol);
    DBGPRINTF_MED("MP: IMEDIA_SetVolume, vol = %d, nRet = %d[%s]", wVol, nRet, MP_ErrorStateToString(nRet));
    if (SUCCESS == nRet)
    {
      CPlayerWin_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME,
                                 MM_STATUS_DONE, NULL, 0);
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    break;
  }

  case IDM_PM_DOWN:
  {
    int    nRet;
    uint16 nNextLevel = 0;
    int16  nVol;

    if(pme->m_pOwner->m_wVolumeLevel > 0)
    {
      nNextLevel = pme->m_pOwner->m_wVolumeLevel - 1;
    }
    nVol = (nNextLevel * pme->m_pOwner->m_VolumeStep) + 2;

    DBGPRINTF_MED("MP: IDM_PM_DOWN event received.");
    nVol = (nVol < 0) ? 0 : nVol;

    nRet  = IMEDIA_SetVolume(pme->m_pMedia, (uint16)nVol);
    DBGPRINTF_MED("MP: IMEDIA_SetVolume, vol = %d, nRet = %d[%s]", nVol, nRet, MP_ErrorStateToString(nRet));
    if (SUCCESS == nRet)
    {
      CPlayerWin_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME,
                                 MM_STATUS_DONE, NULL, 0);
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    break;
  }

  case IDM_PM_BK_FILE:
    DBGPRINTF_MED("MP: IDM_PM_BK_FILE event received.");
    {
#ifdef FEATURE_QTV_GENERIC_BCAST
      boolean     bBcastEnable;
      int         iRet;

      bBcastEnable = pme->m_pOwner->m_pSetting->m_bEnableBcastSettings;
      if (bBcastEnable)
      {
        if (CPlayerWin_IsPlaying(pme))
        {
          (void)IMEDIA_Pause(pme->m_pMedia);
        }
        else
        {
          iRet = CPlayerWin_SyncFrameBack(pme);
          if (iRet == SUCCESS)
          {
            DBGPRINTF_MED("MP: call IMEDIA_Resume after SyncFrameBack");
            iRet = IMEDIA_Resume(pme->m_pMedia);
            if(iRet != SUCCESS)
            {
              if(iRet == EITEMBUSY)
              {
                DBGPRINTF_HIGH("MP: failed on IMEDIA_Resume after SyncFrameBack, IMedia is busy,  returns %d = %s.",
                              iRet, MP_ErrorStateToString(iRet));
              }
              else
              {
                DBGPRINTF_ERROR("MP: failed on IMEDIA_Resume after SyncFrameBack, returns %d = %s.",
                                iRet, MP_ErrorStateToString(iRet));
                CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
              }
            }
            else
            {
              DBGPRINTF_MED("MP: IMEDIA_Resume returns %d = %s", ret, MP_ErrorStateToString(ret));
              pme->m_pOwner->m_bPlayAfterResume = FALSE;
            }
          }
        }
      }
      else
#endif // FEATURE_QTV_GENERIC_BCAST
      {
        pme->m_CB.m_nCBCount = 0;
        CPlayerWin_PlayPreviousClip(pme);
      }

      // need to return from here since pme may have been updated, or even set to NULL
      return TRUE;
    }
  case IDM_PM_FF_FILE:
    DBGPRINTF_MED("MP: IDM_PM_FF_FILE event received.");
    {
#ifdef FEATURE_QTV_GENERIC_BCAST
      boolean     bBcastEnable;
      int         iRet;

      bBcastEnable = pme->m_pOwner->m_pSetting->m_bEnableBcastSettings;
      if (bBcastEnable)
      {
        if (CPlayerWin_IsPlaying(pme))
        {
          (void)IMEDIA_Pause(pme->m_pMedia);
        }
        else
        {
          iRet = CPlayerWin_SyncFrameForward(pme);
          if (iRet == SUCCESS)
          {
            DBGPRINTF_MED("MP: call IMEDIA_Resume after SyncFrameForward");
            iRet = IMEDIA_Resume(pme->m_pMedia);
            if(iRet != SUCCESS)
            {
              if(iRet == EITEMBUSY)
              {
                DBGPRINTF_HIGH("MP: failed on IMEDIA_Resume after SyncFrameForward, IMedia is busy,  returns %d = %s.",
                              iRet, MP_ErrorStateToString(iRet));
              }
              else
              {
                DBGPRINTF_ERROR("MP: failed on IMEDIA_Resume after SyncFrameForward, returns %d = %s.",
                                iRet, MP_ErrorStateToString(iRet));
                CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
              }
            }
            else
            {
              pme->m_pOwner->m_bPlayAfterResume = FALSE;
            }
          }
        }
      }
      else
#endif // FEATURE_QTV_GENERIC_BCAST
      {
        pme->m_CB.m_nCBCount = 0;
        CPlayerWin_PlayNextClip(pme);
      }
      // need to return from here since pme may have been updated, or even set to NULL
      return TRUE;
    }

  case IDM_PM_FULLSCRN:
    DBGPRINTF_MED("MP: IDM_PM_FULLSCRN event received.");
    (void)CPlayerWin_FullScreen(pme, (boolean) ! pme->m_bFullScreen, FALSE);
    break;

#ifdef MP_FEATURE_VIDEO
  case IDM_PM_INFO:
    {
#ifdef MP_FEATURE_QTV_DIVX_DRM  
      AEECLSID  ClsId;
      char regCode[128];
      char buf[128];
      DBGPRINTF_MED("MP: IDM_PM_INFO event received.");

      if (IMEDIA_GetClassID(pme->m_pMedia, &ClsId) == SUCCESS)
      {
        if((ClsId == AEECLSID_MEDIAMPEG4) && (pme->m_mediaInfo.eFormatType == MM_DIVX_DRM_FORMAT_TYPE))
        {
          if(MediaPlayer_GetDivXRegCode(pme->m_pOwner, pme->m_pMedia, regCode, 128))
          {
            CMediaPlayer* pMP = pme->m_pOwner;

            // reg code is in 8 digit length
            (void)SNPRINTF(buf, sizeof(buf), "\nDevice Registration Code: %s",regCode);

            if(pMP->m_pDlg == NULL)
            {
              pMP->m_pDlg = (CMPDlg*)MPDlgCreate(pMP, pMP->a.m_pIShell, pme->m_pIDisplay, &pme->m_rectImage);
              if(pMP->m_pDlg)
              {
                DBGPRINTF_MED("MP: display DivX device registration code.");
                (void)MPDlgSetText(pMP->m_pDlg, "DivX registration info", buf);
                (void)MPDlgAddButton(pMP->m_pDlg, MP_DLG_OK, (ButtonHandlerT)&MPDlgButtonHandler_OK);
                (void)MPDlgSetButtonFocus(pMP->m_pDlg, MP_DLG_OK);
                (void)MPDlgUpdate(pMP->m_pDlg, pMP->m_pIGraphics);
              }
              else
              {
                DBGPRINTF_ERROR("MP: failed to create dlg.");
              }
            }
            else
            {
              DBGPRINTF_ERROR("MP: incorrect Media SPEC.");
            }
          }
        }
        else
        {
#endif //#ifdef MP_FEATURE_QTV_DIVX_DRM  
          // toggle info display
          if(!pme->m_bDIsplayInfo)
            (void)CPlayerWin_Info(pme, TRUE);
          else
            CPlayerWin_Info_Clear(pme);
#ifdef MP_FEATURE_QTV_DIVX_DRM  
        }
      }
#endif //#ifdef MP_FEATURE_QTV_DIVX_DRM  
    }
    break;

#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
  case IDM_PM_CCW_ROTATE:
    /*
     * Not supported -- callback assumes rotation is done clockwise only.
     */
    DBGPRINTF_MED("MP: IDM_PM_CCW_ROTATE event received.");
    return FALSE;

#ifdef MP_FEATURE_FAST_PLAYBACK
  case IDM_PM_FAST_PLAYBACK:
    {
      if( CPlayerWin_IsPlaying(pme) && !CPlayerWin_IsMediaBusy(pme) &&
             (pme->m_pOwner->m_playState == MP_PLAYSTATE_PLAYING ||
              pme->m_pOwner->m_playState == MP_PLAYSTATE_BUFFERING))
      {
        ret = IMEDIA_Pause(pme->m_pMedia);
        if(ret == SUCCESS)
        {
          DBGPRINTF_HIGH("MP: IMEDIA_Pause requested.\n");
          pme->m_bChangeReplaySpeed = TRUE;
        }
        else
        {
          DBGPRINTF_ERROR("MP: IMEDIA_Pause failed, %d", ret);
        }
      }
      else
      {
        int i;
        int size;
        int newSpeed = pme->m_ReplaySpeed;

        // get next speed
        size = sizeof(REPLAY_SPEED) / sizeof(uint32);
        for(i=0; i < size; i++)
        {
          if(REPLAY_SPEED[i] == pme->m_ReplaySpeed)
          {
            if(i < (size -1))
            {
              newSpeed = REPLAY_SPEED[i+1];
            }
            else
            {
              newSpeed = REPLAY_SPEED[0];
            }
          }
        }

        // set new speed
        if(SUCCESS == IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_PB_SPEED, newSpeed, 0))
        {
          pme->m_ReplaySpeed = newSpeed;
          DBGPRINTF_HIGH("MP: Set replay speed %d.", newSpeed);
          CPlayerWin_UpdateProgCtl(pme, MM_CMD_PLAY, MM_MP4_PARM_PB_SPEED, 0);
        }
        else
        {
          DBGPRINTF_ERROR("MP: failed to set new replay speed, %d.", newSpeed);
        }
      }
    }

    break;
#endif //MP_FEATURE_FAST_PLAYBACK


#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  case IDS_AUDIO_CHANNEL:
     {
     //DualMonoOutput
        int i;
        int size;
        int ret;
        int DualMono = pme->m_DualMonoOutput;

        // get next speed
        size = sizeof(DualMonoOutput) / sizeof(uint32);
        for(i=0; i < size; i++)
        {
          if(DualMonoOutput[i] == pme->m_DualMonoOutput)
          {
            if(i < (size -1))
            {
              DualMono = DualMonoOutput[i+1];
            }
            else
            {
              DualMono = DualMonoOutput[0];
            }
          }
        }

        if(SUCCESS == (ret = IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_DUAL_MONO, DualMono, 0)))
        {
          pme->m_DualMonoOutput = DualMono;
          DBGPRINTF_HIGH("MP: MM_MP4_PARM_DUAL_MONO, set to %d.", DualMono);
        }
        else
        {
          DBGPRINTF_ERROR("MP: failed to set MM_MP4_PARM_DUAL_MONO, %d, ret = %d", DualMono, ret);
        }

     }
     break;

#endif //FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION

  case IDM_PM_CW_ROTATE:
    /*
     * The player might be buffering but still considered playing,
     * in which case rotation will fail. So don't update the
     * stored orientation until the callback is notified of a
     * successful rotation, or SetMediaParm()returns SUCCESS
     * immediately (which means it was done synchronously).
     */
    DBGPRINTF_MED("MP: IDM_PM_CW_ROTATE event received.");
    if (CPlayerWin_IsPlaying(pme))
    {
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
      AEEMediaMPEG4RotationType rotationType = MP4_GetNextRotation(pme->m_Rotation, IDM_PM_CW_ROTATE);

      /*
      * If the rotation is done synchronously, update the orientation.
      * Otherwise, let the callback do it when / if it is notified.
      */
      if (SUCCESS == IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_ROTATION, (int32)rotationType, 0))
      {
        pme->m_Rotation = rotationType;
      }
#endif // defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
#endif //FEATURE_APP_MEDIA_VIDEO

#ifdef FEATURE_QTV_MDP_ASCALE //
      // Adjust the dimensions of the video space when rotating
      if (!pme->m_pOwner->m_bIsImage)
      {
        (void)CPlayerWin_SetAScale( pme );
      }
#endif
      
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
      if ( !pme->m_bFullScreen ) 
      {
          (void)IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_SCALING, pme->m_dwZoomScale, 0);
      }
#endif // defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    }
    break;

  case IDM_PM_ZOOM_IN:
  case IDM_PM_ZOOM_OUT:
    /*
     * Don't zoom in or out if there's a pending zoom.
     */
    DBGPRINTF_MED("MP: IDM_PM_ZOOM_IN/OUT event received.");
    if ((0 == pme->m_pendingZoomDirection) && CPlayerWin_IsPlaying(pme))
    {
      if (pme->m_bFullScreen)
      {
        DBGPRINTF_HIGH("MP: Ignoring Zoom request as display is currently in FullScreen mode");
      }
      else
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined (FEATURE_APP_MPEG4) && defined(MP_FEATURE_VIDEO)
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
      {
        int retval = EFAILED;
        AEEMediaMPEG4ScalingType dwZoomScale = MP4_GetNextScale(pme->m_dwZoomScale, wParam);
        /*
        * Check that we haven't reached the limit of our zoom capability.
        */
        if (pme->m_dwZoomScale != dwZoomScale)
        {
          pme->m_pendingZoomDirection = ((wParam == IDM_PM_ZOOM_IN) ? 1 : -1);

          retval =
            IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_SCALING,
                                (int32)dwZoomScale, 0);
          if (SUCCESS == retval)
          {
            /*
            * If the zoom is done synchronously, update the stored
            * zoom scalefactor.  Otherwise, let the callback do it
            * when / if it is called.
            */
            CPlayerWin_UpdateZoomScale(pme);
          }
          else if (MM_PENDING != retval)
          {
            /*
            * Clear pending direction in case of error, so we can
            * try again later.
            */
            pme->m_pendingZoomDirection = 0;
          }
        }
      }
#endif //defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
#endif //FEATURE_APP_MEDIA_VIDEO

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    }

    break;
#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS ||
           // FEATURE_QTV_MDP_TRANSFORMATIONS

#ifdef FEATURE_QTV_STREAM_RECORD
  case IDM_PM_START_STREAM_RECORD:
  {
    int       ret;

    DBGPRINTF_MED("MP: IDM_PM_START_STREAM_RECORD event received.");
    bRTSPstreamRecord = TRUE;
    pme->m_bStopPressed = FALSE;

    if (CPlayerWin_IsPause(pme))
    {
      // If paused, first resume before starting Stream Record
      ret = IMEDIA_Resume(pme->m_pMedia);
      DBGPRINTF_MED("MP: IMEDIA_Resume returns %d = %s", ret, MP_ErrorStateToString(ret));
      if(ret != SUCCESS)
      {
        // Imedia may be busy
        if(ret != EITEMBUSY)
        {
          // display error
          CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
          DBGPRINTF_ERROR("MP: failed to Resume, %d = %s.", ret, MP_ErrorStateToString(ret));
        }
        else
        {
          DBGPRINTF_HIGH("MP: failed to Resume, IMedia is busy, %d = EITEMBUSY.", ret);
        }
        break;
      }
      pme->m_pOwner->m_bPlayAfterResume = FALSE;
    }
    else if (pme->m_pOwner->m_playState == MP_PLAYSTATE_CONNECTING)
    {
      // Cannot start stream recording while in Connecting state
      // Show dialog if not running from diagnostics, otherwise, log message.
      DBGPRINTF_HIGH("MP: Cannot start Stream Record, MP is Connecting");
      break;
    }
    else if (CPlayerWin_IsReady(pme))
    {
      // Playback is ready but not yet started, so request play before
      // starting Stream Record
      ret = IMEDIA_Play(pme->m_pMedia);
      if (SUCCESS != ret)
      {
        if(ret == EITEMBUSY)
        {
          DBGPRINTF_HIGH("MP: failed on IMEDIA_Play, IMedia is busy,  %d = EITEMBUSY.", ret);
        }
        else
        {
          DBGPRINTF_ERROR("MP: failed on IMEDIA_Play, %d = %s.", ret, MP_ErrorStateToString(ret));
          CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
        }
        break;
      }
      else
      {
        DBGPRINTF_MED("MP: IMEDIA_Play returns %d = %s", ret, MP_ErrorStateToString(ret));
        pme->m_pOwner->m_bPlayAfterResume = FALSE;
      }
    }

    // Start recording the clip
    DBGPRINTF_MED("MP: Starting to record clip.");
    CPlayerWin_StartStreamRecord(pme, TRUE);
    break;
  }

  case IDM_PM_STOP_STREAM_RECORD:
    DBGPRINTF_MED("MP: IDM_PM_STOP_STREAM_RECORD event received.");
    pme->m_bStopPressed = TRUE;
    pme->m_pOwner->m_playState = MP_PLAYSTATE_STOPPED;
    EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
    ret = IMEDIA_Stop(pme->m_pMedia);
    if(ret != SUCCESS)
      DBGPRINTF_ERROR("MP: failed on IMEDIA_Stop, returns %d = %s.",
                      ret, MP_ErrorStateToString(ret));
    CPlayerWin_StopStreamRecord(pme);

    if (pme->m_eWin == MPPW_STREAM_RECORD)
    {
      CPlayerWin_DrawStreamRecordMenu(pme);
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    break;

#endif     // FEATURE_QTV_STREAM_RECORD

#endif     // MP_FEATURE_VIDEO
  case IDM_PM_TERMINATE:
    DBGPRINTF_MED("MP: IDM_PM_TERMINATE event received.");
    MP_RELEASEIF(pme->m_pMedia);
    break;

  case IDM_PM_FCS_SWITCH:
  {
    DBGPRINTF_MED("MP: IDM_PM_FCS_SWITCH event received.");
    CPlayerWin_FCSSwitch(pme);
    break;
  }

  default:
    return FALSE;
  }
  return TRUE;
}

/*==========================================================================
   This function processes the image related menu commands
   (event code EVT_COMMAND).
==========================================================================*/
static boolean
CPlayerWin_HandleImageEventCommand(CPlayerWin *pme, uint16 wParam,
                                   uint32 dwParam)
{
  if (CPlayerWin_IsImage(pme) && !pme->m_pImageCtl)
  {
    return TRUE;
  }
  switch (wParam)
  {
  case IDM_PM_FULLSCRN:
    (void)CPlayerWin_FullScreen(pme, (boolean) ! pme->m_bFullScreen, FALSE);
    break;
  default:
    return FALSE;
  }
  return TRUE;
}

/*==========================================================================
 * This function performs a Fast Content Switch (FCS).
 * This is a QTV feature for switching between content on a server (HTTP/RTSP)
 *==========================================================================
 */
boolean CPlayerWin_FCSSwitch(CPlayerWin *pme)
{
#ifdef FEATURE_QTV_FCS
#error code not present
#else
  DBGPRINTF_ERROR("MP: CPlayerWin_FCSSwitch - FCS unsupported feature.");
  return(FALSE);
#endif
}

#if defined (FEATURE_QTV_BCAST_FLO) && !defined (FEATURE_APP_MEDIA_VIDEO)
#error code not present
#endif     // #if defined (FEATURE_QTV_BCAST_FLO) && !defined (FEATURE_APP_MEDIA_VIDEO)


/*===========================================================================
   This function handles the events routed to the player window.
   If the event is
   (1)CLR or OK from error dialog:
       (a)If app is in plugin mode, it is closed.
       (b)If media playback is on, then file list window is opened
       (c)If recording is on, then main window is opened

   (2)AVK_0 or EVT_COMMAND(IDM_PM_FULLSCRN): Full screen is toggled
   (3)EVT_CREATEMEDIA: Media is created using AEEMediaUtil_CreateMedia()
   (4)EVT_CREATEMEDIA_QCP: IMediaQCP is created and its media data is set
       to put it in Ready state.
   (5)EVT_COMMAND: processes the menu commands.
   (6)EVT_KEY: routed appropriately to the controls.
===========================================================================*/
static boolean
CPlayerWin_HandleEvent(IWindow *po, AEEEvent eCode, uint16 wParam,
                       uint32 dwParam)
{
  CPlayerWin *pme = (CPlayerWin *)po;
  boolean   bRet = TRUE;
  int       ret;

  if(pme == NULL || pme->m_pOwner == NULL)
  {
    DBGPRINTF_ERROR("MP: player win, NULL ptr encountered.");
    return FALSE;
  }

  if(pme->m_pOwner->m_pDlg && MPDlgHandleEvent(pme->m_pOwner->m_pDlg, eCode, wParam, dwParam))
  {
    CPlayerWin_Redraw(po);
    return TRUE;
  }

#ifdef FEATURE_MP_EQ_UI
  // return TRUE if event is handled by setup win
  if(MPSetupHandleEvent(pme->m_pOwner->m_pSetting, eCode, wParam, dwParam))
  {
    CPlayerWin_Redraw(po);
    return TRUE;
  }
#endif //FEATURE_MP_EQ_UI

#ifdef FEATURE_KEY_HOLD_SCROLL
  if (MP_ISONEOFKEYEVENT(eCode) && (wParam == AVK_SELECT || wParam == AVK_CLR))
  {
    /*
     * if timer is enabled for auto-scrolling, do nothing here
     */
    if (pme->m_pOwner->m_KeyStore.m_wParam_Key)
      return TRUE;
  }
#endif

  /*
   * Process CLR or OK from error dlg
   */
  if (MP_ISCLR(eCode) || MP_ISCMD(eCode, IDS_OK))
  {
    return CPlayerWin_HandleClearOrOK(pme, eCode, wParam, dwParam);
  }

#ifdef FEATURE_KEY_HOLD_SCROLL
  /*
   * if this is a key event, process and return.
   */
  ret = CPlayerWin_HandleKeyEvent(po, eCode, wParam, dwParam);
  if (ret >= 0)
    return ret;
#else
  /*
   * Process EVT_KEY...
   */
  if (MP_ISEVTKEY(eCode))
  {
    return CPlayerWin_HandleEventKey(pme, eCode, wParam, dwParam);
  }
#endif

  ret = CPlayerWin_HandleCreateMedia(po, eCode, wParam, dwParam);
  if(ret >= 0)
  {
    return (boolean)ret;
  }

#ifdef MP_FEATURE_VIDEO
  /*
   * EVT_MEDIAPLAYER_SEEK command
   */
  if (eCode == EVT_MEDIAPLAYER_SEEK)
  {
    if (pme->m_pMedia != NULL)
    {
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
      int32     classId;
      (void)IMEDIA_GetClassID(pme->m_pMedia, (AEECLSID *)&classId);
      if (classId == AEECLSID_MEDIAMPEG4)
#endif
      {
        /*
         * Reset the start position to the resume play position
         */
        DBGPRINTF_MED("MP: Rest the start position to the resume play position");
        (void)IMEDIA_SetMediaParm(pme->m_pMedia, MM_PARM_POS, 0, dwParam);
        /*
         * Reset the resume play position
         */
        pme->m_pOwner->m_resumePlayPos = 0;
        return TRUE;
      }
    }
    return FALSE;
  }
#endif     // MP_FEATURE_VIDEO

  /*
   * If this event is not EVT_COMMAND, then return.
   * Otherwise, handle the command.
   */
  if (!MP_ISEVTCMD(eCode) ||
      !CPlayerWin_HandleEventCommand(pme, wParam, dwParam))
  {
    DBGPRINTF_MED("MP: CPlayerWin_HandleEvent returning False for eCode %x wParam %x dwParam %x", eCode, wParam, dwParam);
    return FALSE;
  }
  return bRet;
}
/*===========================================================================
  This function handles the create media event.
  return -1 if event is not handled
  return 1, if SUCCESS, and event is handled
  return 0, if failed to create media object, but the event is handled (not used now)
===========================================================================*/
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else //FEATURE_APP_MEDIA_VIDEO
static int CPlayerWin_HandleCreateMedia(IWindow *po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  CPlayerWin *pme = (CPlayerWin *)po;
  int nReturn = -1;

  //DBGPRINTF_LOW("MP: CPlayerWin_HandleCreateMedia eCode = %d, wParam = %d, dwParam = %d", eCode, wParam, dwParam);

  /*
   * Process create media events...
   */
  if(TRUE == CMediaPlayer_IsCreateMediaCode(eCode))
  {
    int       nRet = EFAILED;

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
    if(pme->m_pOwner->m_bIContentMode && pme->m_pMedia)
      nRet = SUCCESS;
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

    if (eCode == EVT_CREATEMEDIA)
    {
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
      if (pme->m_pOwner->m_bPlayRingerNoQTV)
      {
        if (!pme->m_pMedia)
        {
          nRet = AEEMediaUtil_CreateMedia(pme->m_pIShell,&pme->m_md,&pme->m_pMedia);
          DBGPRINTF_HIGH("MP: AEEMediaUtil_CreateMedia() returns %d", nRet);
        }
      }
      else
      {
#endif //(FEATURE_APP_MPEG4) && (FEATURE_QTV_AUDIO_RINGER)
#if defined(FEATURE_QTV_BCAST_FLO) && defined(FEATURE_APP_MPEG4)
#error code not present
#endif     // FEATURE_QTV_BCAST_FLO || FEATURE_APP_MPEG4
        {
          /*
           * -----------------------------------------------------------------
           *  No extension or extension does not match FLO.  Proceed with
           *  standard QTV processing
           *  ----------------------------------------------------------------
           */
#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))

          AEEMediaDataWithQoS mediaQos;
          AEENetPolicyInfo    mediapolicy;

          mediaQos.clsData = pme->m_md.clsData;
          mediaQos.dwSize = pme->m_md.dwSize;
          mediaQos.pData = pme->m_md.pData;
          mediapolicy.size = sizeof(AEENetPolicyInfo);
          mediapolicy.isSet = TRUE;
          mediapolicy.isQoSEnabled = pme->m_pOwner->m_NetPolicy.m_bQoSEnable;
          mediapolicy.primaryPDPProfileNo = pme->m_pOwner->m_NetPolicy.m_nProfile;
          mediaQos.qosInfo = &mediapolicy;

          DBGPRINTF_HIGH("MP: create media with Qos: %s, PDP = %d",
                         (pme->m_pOwner->m_NetPolicy.m_bQoSEnable)?"On":"Off",
                          pme->m_pOwner->m_NetPolicy.m_nProfile);
          if(!pme->m_pMedia)
          {
            nRet = AEEMediaUtil_CreateQoSMedia(pme->m_pIShell, &mediaQos, &pme->m_pMedia);
            DBGPRINTF_HIGH("MP: CreateQoSMedia() returns %d = %s", nRet, MP_ErrorStateToString(nRet));
          }
#endif  // FEATURE_APP_MPEG4 && (FEATURE_QTV_QOS_SELECTION || FEATURE_QTV_PDP_SELECTION)

          if(!pme->m_pMedia)
          {
            if(pme->m_pOwner->m_bLocalPlayPullMode && (pme->m_md.clsData == MMD_HTTP_PULL_BUFFER_PLAYBACK) )
            {
              nRet = MP_CreateMedia(pme->m_pIShell, &pme->m_md, &pme->m_pMedia);

              if(nRet == SUCCESS && pme->m_pMedia)
              {
                // Register callbacks for Local Play PUll Mode. These has to be done before calling on IMEDIA_SetMediaData
                if (pme->m_pOwner->m_bLocalPlayPullMode)
                {
                  DBGPRINTF_MED("MP: Register callbacks for Local Play PULL Mode");
                  nRet = MP_LocalPlayPullModeRegisterCallbacks(pme->m_pOwner, pme->m_pMedia);
                }
			   
                if(nRet == SUCCESS)
                {
                  // Set the media data and put IMedia in Ready state. The function will also create QTV object
                  nRet = IMEDIA_SetMediaData(pme->m_pMedia, &pme->m_md);
                }
              }
            }
            else
            {
              nRet = AEEMediaUtil_CreateMedia(pme->m_pIShell, &pme->m_md, &pme->m_pMedia);
            }
            DBGPRINTF_HIGH("MP: CreateMedia() returns %d = %s", nRet, MP_ErrorStateToString(nRet));
          }
          if(!pme->m_pMedia)
          {
            DBGPRINTF_HIGH("MP:  m_pMedia is NULL after AEEMediaUtil_CreateMedia!");
          }
        }
        if (pme->m_pMedia)
        {
          DBGPRINTF_HIGH("MP:  m_pMedia created");
          // For audio clips, we can obtain the duration immediately after creating the media.
          if (CMediaPlayer_FindHandlerType(pme->m_pOwner, pme->m_pszFile) == IDB_AUDIO_ICON)
          {
            int nState;
            boolean bChg;
            // for debugging, print state information
            nState = IMEDIA_GetState(pme->m_pMedia, &bChg);
            DBGPRINTF_HIGH("MP:  media state = %d = %s, state changing = %d", nState, MP_MediaStateToString(nState), bChg);
            // Disabled the following lines to avoid GetTotalTime cb not being executed before next Play request,
            // which causes an abort.  See CR 99459 and CR 101179 for analysis.
            // int nErr;
            // nErr = IMEDIA_GetTotalTime(pme->m_pMedia);
            // DBGPRINTF_HIGH("MP:  IMEDIA_GetTotalTime() returns %d = %s", nErr, MP_ErrorStateToString(nErr));
          }

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
          if (pme->m_pOwner->m_bCallQtvProgDnld && pme->m_pOwner->m_bIsURLFile)
          {
            extern void MP_RegisterCallbacksForQTVDownload(IMedia *, CMimicEncryptDnld);
            MP_RegisterCallbacksForQTVDownload(pme->m_pMedia, pme->m_pOwner->m_MimicEncryptDnld);
          }
#endif  // FEATURE_APP_MPEG4 && (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)


#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) && defined(MM_MP4_PARAM_HTTP_FILE_SAVE_OPTION)
          /*Set File Save Option */
          if (pme->m_pOwner->m_bPvxSavetoFlash)
          {
            char efsName[MP_MAX_FILE_NAME];
            int Ret;

            STRLCPY(efsName, MP_PATH_MEDIA, MP_MAX_FILE_NAME);
            Ret = IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARAM_HTTP_FILE_SAVE_OPTION, (int32)(efsName), 0);
            DBGPRINTF_MED("MP: MM_MP4_PARAM_HTTP_FILE_SAVE_OPTION returns %d = %s", Ret, Ret?"True":"False");
          }
#endif  // defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)&& defined(MM_MP4_PARAM_HTTP_FILE_SAVE_OPTION)
        }

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif    // FEATURE_QTV_MFDRM
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
      }
#endif //(FEATURE_APP_MPEG4) && (FEATURE_QTV_AUDIO_RINGER)
    }

#if defined(FEATURE_APP_MPEG4)
    else if (eCode == EVT_CREATEMEDIA_EX)
    {
      if(!pme->m_pMedia)
      {
        nRet = AEEMediaUtil_CreateMediaEx(pme->m_pIShell, pme->m_pmdExList, pme->m_mdListCount, &pme->m_pMedia);
        DBGPRINTF_HIGH("MP: CreateMediaEx() returns %d", nRet);
      }
    }
#endif     // FEATURE_APP_MPEG4
#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    else if (eCode == EVT_CREATEPSEUDOMEDIA)
    {
      if (wParam == MM_STATUS_ABORT)
      {
        CPlayerWin_SendMediaNotify(pme, MM_CMD_PLAY, 0, MM_STATUS_ABORT, NULL, 0);
        // if this has been enabled disable it
        pme->m_pOwner->m_bDisableAutoPlayOnce = FALSE;
        nReturn = 1;
        return nReturn;
      }

      if (pme->psHttpBlock)
      {
#ifdef MP_USE_CREATEPSEUDOMEDIA
        DBGPRINTF_HIGH("MP: CreatePseudoMedia() psHttpBlock = 0x%lx", pme->psHttpBlock);
        if(!pme->m_pMedia)
        {
          nRet = AEEMediaUtil_CreatePseudoMedia(pme->m_pIShell,
                                             &pme->psHttpBlock->m_md,
                                             &pme->m_pMedia);
          DBGPRINTF_HIGH("MP: CreatePseudoMedia() returns %d = %s", nRet, MP_ErrorStateToString(nRet));
        }
#else
        // This has not been tested yet.  Once we get an update from QTV-OEM we can test this
        DBGPRINTF_HIGH("MP: CreatePseudoMediaEx() psHttpBlock = 0x%lx", pme->psHttpBlock);
        if(!pme->m_pMedia)
        {
          AEEMediaDataEx mdx;

          (void)MEMSET(&mdx, 0, sizeof(AEEMediaDataEx));
          mdx.clsData = pme->psHttpBlock->m_md.clsData;
          mdx.pData = pme->psHttpBlock->m_md.pData;
          mdx.dwSize = pme->psHttpBlock->m_md.dwSize;
          if (HttpDownloader_entireClipInBuffer)
            mdx.dwBufferSize = pme->psHttpBlock->m_md.dwSize;
          else
            mdx.dwBufferSize = pme->psHttpBlock->m_bufferChunk.dwSize;
          mdx.dwStructSize = sizeof(AEEMediaDataEx);
         
          nRet = AEEMediaUtil_CreatePseudoMediaEx(pme->m_pIShell, &mdx, &pme->m_pMedia);
          DBGPRINTF_HIGH("MP: CreatePseudoMediaEx() with %d/%d of buffer filled returns %d = %s", 
                         mdx.dwBufferSize, mdx.dwSize, nRet, MP_ErrorStateToString(nRet));
        }
#endif //MP_USE_CREATEPSEUDOMEDIA
        else
        {
          // nRet is only set if we don't have a m_pMedia object.  So lets set it
          // to success.
          // This may not be necessary once we properly handle Suspend and Resume
          // of HTTPDownload files
          nRet = SUCCESS;
        }
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
        pme->m_setupTime.dwStartupTime = ((CMediaPlayer *) (pme->m_pOwner))->m_defaultSetupTime.dwStartupTime;
        pme->m_setupTime.dwBufferingTime = ((CMediaPlayer *) (pme->m_pOwner))->m_defaultSetupTime.dwBufferingTime;
        if (pme->m_pMedia)
        {
          (void)IMEDIA_SetHTTPStreamMediaData(pme->m_pMedia, &pme->m_setupTime);
          HttpDownloader_maxBufferSize = ((CMediaPlayer *) (pme->m_pOwner))->m_maxDownloadFileSize;
          DBGPRINTF_HIGH("MP: 3GPP startup time   = %d", pme->m_setupTime.dwStartupTime);
          DBGPRINTF_HIGH("MP: 3GPP buffering time = %d", pme->m_setupTime.dwBufferingTime);
          DBGPRINTF_HIGH("MP: 3GPP max file size  = %d", ((CMediaPlayer*)(pme->m_pOwner))->m_maxDownloadFileSize);
        }
#endif     // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      }
      else
      {
#ifdef MP_USE_CREATEPSEUDOMEDIA
        DBGPRINTF_HIGH("MP: CreatePseudoMedia() psHttpBlock = NULL");
        if(!pme->m_pMedia)
        {
          nRet = AEEMediaUtil_CreatePseudoMedia(pme->m_pIShell, &pme->m_md, &pme->m_pMedia);
          DBGPRINTF_HIGH("MP: CreatePseudoMedia() returns %d = %s", nRet, MP_ErrorStateToString(nRet));
        }
#else
        // This has not been tested yet.  Once we get an update from QTV-OEM we can test this
        DBGPRINTF_HIGH("MP: CreatePseudoMediaEx() psHttpBlock = NULL");
        if(!pme->m_pMedia)
        {
          AEEMediaDataEx mdx;

          (void)MEMSET(&mdx, 0, sizeof(AEEMediaDataEx));
          mdx.clsData = pme->m_md.clsData;
          mdx.pData = pme->m_md.pData;
          mdx.dwSize = pme->m_md.dwSize;
          mdx.dwBufferSize = ((CMediaPlayer *) (pme->m_pOwner))->m_nDownloadCopyBytes;
          mdx.dwStructSize = sizeof(AEEMediaDataEx);
         
          nRet = AEEMediaUtil_CreatePseudoMediaEx(pme->m_pIShell, &mdx, &pme->m_pMedia);
          DBGPRINTF_HIGH("MP: CreatePseudoMediaEx() with %d/%d of buffer filled returns %d = %s", 
                         mdx.dwBufferSize, mdx.dwSize, nRet, MP_ErrorStateToString(nRet));
        }
#endif //MP_USE_CREATEPSEUDOMEDIA

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
        pme->m_setupTime.dwStartupTime = ((CMediaPlayer *) (pme->m_pOwner))->m_defaultSetupTime.dwStartupTime;
        pme->m_setupTime.dwBufferingTime = ((CMediaPlayer *) (pme->m_pOwner))->m_defaultSetupTime.dwBufferingTime;
        if (pme->m_pMedia)
        {
          (void)IMEDIA_SetHTTPStreamMediaData(pme->m_pMedia, &pme->m_setupTime);
          HttpDownloader_maxBufferSize = ((CMediaPlayer *) (pme->m_pOwner))->m_maxDownloadFileSize;
          DBGPRINTF_HIGH("MP: 3GPP startup time   = %d", pme->m_setupTime.dwStartupTime);
          DBGPRINTF_HIGH("MP: 3GPP buffering time = %d", pme->m_setupTime.dwBufferingTime);
          DBGPRINTF_HIGH("MP: 3GPP max file size  = %d", ((CMediaPlayer*)(pme->m_pOwner))->m_maxDownloadFileSize);
        }
#endif     // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      }

      if (SUCCESS == nRet)
      {
        if (pme->m_pMedia)
        {
          if (pme->psHttpBlock && pme->psHttpBlock->bWebPstreaming)
          {
            pme->psHttpBlock->m_pMedia = pme->m_pMedia;
#ifdef MP_USE_CREATEPSEUDOMEDIA // If we create Ex media, first offset was done during create.
            DBGPRINTF_HIGH("MP: SetPseudoStreamMediaData block = %d", pme->psHttpBlock->m_bufferChunk.dwSize);
            (void)IMEDIA_SetPseudoStreamMediaData(pme->m_pMedia, &pme->psHttpBlock->m_bufferChunk);
#else
            if (HttpDownloader_entireClipInBuffer)
            {
              pme->psHttpBlock->m_bufferChunk.dwSize = 0;
              DBGPRINTF_HIGH("MP: SetPseudoStreamMediaData block = %d", pme->psHttpBlock->m_bufferChunk.dwSize);
              (void)IMEDIA_SetPseudoStreamMediaData(pme->m_pMedia, &pme->psHttpBlock->m_bufferChunk);
            }
#endif
          }
          else
          {
            pme->m_bufferChunk.dwSize = pme->m_pOwner->m_nDownloadCopiedSize;
            pme->m_bufferChunk.pData = (void *)HttpDownloader_sharedBuffer;
#ifdef MP_USE_CREATEPSEUDOMEDIA // If we create Ex media, first offset was done during create.
            DBGPRINTF_HIGH("MP: SetPseudoStreamMediaData = %d", pme->m_bufferChunk.dwSize);
            (void)IMEDIA_SetPseudoStreamMediaData(pme->m_pMedia, &pme->m_bufferChunk);
#else 
            /* If we filled buffer on first read (max kbps), we need to call SetPseudoStreamMediaData with 0 size */
            if(pme->m_pOwner->m_nDownloadCopiedSize == pme->m_pOwner->m_nDownloadCopyBytes)
            {
              pme->m_bufferChunk.dwSize = 0;
              DBGPRINTF_HIGH("MP: SetPseudoStreamMediaData = %d", pme->m_bufferChunk.dwSize);
              (void)IMEDIA_SetPseudoStreamMediaData(pme->m_pMedia, &pme->m_bufferChunk);
              pme->m_bufferChunk.dwSize = pme->m_pOwner->m_nDownloadCopiedSize;
            }
#endif
          }
        }

        if (pme->m_pOwner->m_bPlugin || pme->m_pOwner->m_nContMode != CONT_MODE_OFF)
        {
          DBGPRINTF("MP: Calling Play since m_bPlugin = %d || m_nContMode (%d) != CONT_MODE_OFF",
            pme->m_pOwner->m_bPlugin, pme->m_pOwner->m_nContMode);
          CPlayerWin_AutoPlayClip(pme->m_pOwner);
          //(void)ISHELL_PostEvent(((CMediaPlayer *)pme->m_pOwner)->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
        }
        if (pme->psHttpBlock && pme->psHttpBlock->bWebPstreaming)
        {
          if (http_ps_blockUpdate(pme->psHttpBlock))
          {
            /*
             * Request the next data chunk
             */
            http_ps_get_chunk(&pme->psHttpBlock);
          }
          else
          {                            // Download is over
            if (pme->psHttpBlock->bKDDIExt)
            {
              /*
               * Initiate Normal signal completion
               */
              http_ps_get_chunk(&pme->psHttpBlock);
            }
          }
        }
      }
      else
      {                                // Create Pseudo media failed
                                       // !!!
        /*
         * DBGPRINTF()sends a message to diag port.
         * Legacy HIGH messages should be turned on for this
         * message to appear on terminal (QXDM)connected to
         * receive data from diag port.
         */
        DBGPRINTF_ERROR("MP: Not enough memory for creating (HTTP) pseudo stream media (%d)", nRet);
        if (pme->psHttpBlock && pme->psHttpBlock->bWebPstreaming)
        {
          PS_CleanHttpDownload(pme->psHttpBlock);
          FREE(pme->psHttpBlock);
          pme->psHttpBlock = NULL;
        }
      }
    }
#if defined(FEATURE_QTV_FCS)
    else if (eCode == EVT_CREATEMEDIA_FCS)
    {
      if(!pme->m_pMedia)
      {
        // for creating an FCS media object
        nRet = CPlayerWin_CreateMediaFCS(pme, &pme->m_md, &pme->m_pMedia);
      }
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: failed to create FCS media.");
      }
    }
#endif // FEATURE_QTV_FCS

#endif  // FEATURE_APP_MPEG4 && (FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#ifdef FEATURE_MP_RECORDER_SETTINGS
    else
    {
#if BREW_VERSION_LESS_THAN(4, 0)
      // this is the hack for brew 3x to record both up and down links using two pMedia object.
      // brew 4.0 should support MM_APATH_BOTH mode.
      if(pme->m_pOwner->m_RecordPath == MM_APATH_BOTH)
      {
        AEEMediaData md;
        int i=0;
        char buf[MP_MAX_FILE_NAME + 1];
        char* pData;
        IMedia* pMedia = NULL;

        pData = pme->m_md.pData;
        md.clsData = pme->m_md.clsData;
        md.dwSize = pme->m_md.dwSize;
        md.pData = buf;

        DBGPRINTF_HIGH("MP: Recording mode is set to MM_APATH_BOTH, generating _up and _dl files.");
        
        // append _ul to current file name
        if(MP_InsertStringToFileName(pData, "_ul", buf, MP_MAX_FILE_NAME + 1))
        {
          for(i=0; i < 2; i++)
          {
            if (eCode == EVT_CREATEMEDIA_QCP && pme->m_pOwner->m_AudioFormat == MP_8K_PCM_RECORDING)
            {
              // for recording PCM, we have to use PCM classID
              nRet = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MEDIAADPCM, (void **)&pMedia);
              if (nRet == SUCCESS)
              {
                DBGPRINTF_HIGH("MP: PCM record, %s", (char*)md.pData);
                nRet = IMEDIA_SetMediaData(pMedia, &md);
                if(SUCCESS != nRet)
                {
                  DBGPRINTF_ERROR("MP: IMEDIA_SetMediaData failed, nRet = %d, md.clsData = %d, md.dwSize = %d, md.pData = %s",
                                  nRet, md.clsData, md.dwSize, (char*)md.pData);
                }
              }
              else
              {
                DBGPRINTF_ERROR("MP: failed to create media for PCM recording, %s.", (char*)md.pData);
              }
            }
#ifdef FEATURE_AAC_REC
            else if(eCode == EVT_CREATEMEDIA_QCP && pme->m_pOwner->m_AudioFormat == MP_AAC_RECORDING)
            {
              // for recording AAC, use AEECLSID_MEDIAAAC
              nRet = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MEDIAAAC, (void **)&pMedia);
              if (nRet == SUCCESS)
              {
                DBGPRINTF_HIGH("MP: AAC record, %s", (char*)md.pData);
                nRet = IMEDIA_SetMediaData(pMedia, &md);
                if(SUCCESS != nRet)
                {
                  DBGPRINTF_ERROR("MP: IMEDIA_SetMediaData faild, nRet = %d, md.clsData = %d, md.dwSize = %d, md.pData = %s",
                                  nRet, md.clsData, md.dwSize, (char*)md.pData);
                }
              }
              else
              {
                DBGPRINTF_ERROR("MP: failed to create media for AAC recording %s.", (char*)md.pData);
              }
            }
#endif    // FEATURE_AAC_REC
#ifdef FEATURE_AUDFMT_EVB
            else if(eCode == EVT_CREATEMEDIA_QCP && pme->m_pOwner->m_AudioFormat == MP_EVRCNB_RECORDING)
            {
              AEECLSID cls;
              // for recording EVRC-NB, use AEECLSID_MediaEVRCNB
              cls = CMediaPlayer_GetAEECLSID(pme->m_pOwner, AUDIO_MIME_BASE, "evb");
              nRet = ISHELL_CreateInstance(pme->m_pIShell, cls, (void **)&pMedia);
              if (nRet == SUCCESS)
              {
                DBGPRINTF_HIGH("MP: EVRC-NB record, %s", (char*)md.pData);
                nRet = IMEDIA_SetMediaData(pMedia, &md);
                if(SUCCESS != nRet)
                {
                  DBGPRINTF_ERROR("MP: IMEDIA_SetMediaData faild, nRet = %d, md.clsData = %d, md.dwSize = %d, md.pData = %s",
                                  nRet, md.clsData, md.dwSize, (char*)md.pData);
                }
              }
              else
              {
                DBGPRINTF_ERROR("MP: failed to create media for EVRC-NB recording %s.", (char*)md.pData);
              }
            }
#endif    // FEATURE_AUDFMT_EVB
            else if (eCode == EVT_CREATEMEDIA_QCP)
            {
              nRet = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MEDIAQCP, (void **)&pMedia);
              if (nRet == SUCCESS)
              {
                DBGPRINTF_HIGH("MP: QCP record, %s", (char*)md.pData);
                nRet = IMEDIA_SetMediaData(pMedia, &md);
                if(SUCCESS != nRet)
                {
                  DBGPRINTF_ERROR("MP: IMEDIA_SetMediaData faild, nRet = %d, md.clsData = %d, md.dwSize = %d, md.pData = %s",
                                  nRet, md.clsData, md.dwSize, (char*)md.pData);
                }
              }
              else
              {
                DBGPRINTF_ERROR("MP: failed to create media for QCP recoding, %s.", (char*)md.pData);
              }
            }
            else
            {
              DBGPRINTF_ERROR("MP: Unsupported create media event type: eCode = %d.", eCode);
            }

            if(i == 0)
            {
              pme->m_pMedia = pMedia;
              // append _dl_link to current file name
              if(!MP_InsertStringToFileName(pData, "_dl", buf, MP_MAX_FILE_NAME + 1))
              {
                DBGPRINTF_ERROR("MP: error on createing _dl record file name.");
                break;
              }
            }
            else
            {
              pme->m_pMedia1 = pMedia;
            }
          }
        }
        else
        {
          DBGPRINTF_ERROR("MP: error on creating _ul record file name.");
        }
      }
      else
#endif //BREW_VERSION_LESS_THAN(4, 0)
      {
        if (eCode == EVT_CREATEMEDIA_QCP && pme->m_pOwner->m_AudioFormat == MP_8K_PCM_RECORDING)
        {
          if(NULL == pme->m_pMedia && NULL != pme->m_md.pData)
          {
            DBGPRINTF_HIGH("MP: PCM record, %s", (char*)pme->m_md.pData);
            // for recording PCM, we have to use PCM classID
            nRet = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MEDIAADPCM, (void **)&pme->m_pMedia);
            if (nRet == SUCCESS)
            {
              nRet = IMEDIA_SetMediaData(pme->m_pMedia, &pme->m_md);
              if(SUCCESS != nRet)
              {
                DBGPRINTF_ERROR("MP: IMEDIA_SetMediaData faild, nRet = %d, md.clsData = %d, md.dwSize = %d, md.pData = %s",
                                nRet, pme->m_md.clsData, pme->m_md.dwSize, (char*)pme->m_md.pData);
              }
            }
            else
            {
              DBGPRINTF_ERROR("MP: failed to create media.");
            }
          }
          else
          {
            DBGPRINTF_ERROR("MP: pme->m_pMedia or pme->m_md.pData error.");
          }
        }
#ifdef FEATURE_AAC_REC
        else if(eCode == EVT_CREATEMEDIA_QCP && pme->m_pOwner->m_AudioFormat == MP_AAC_RECORDING)
        {
          if(NULL == pme->m_pMedia && NULL != pme->m_md.pData)
          {
            DBGPRINTF_HIGH("MP: AAC record, %s", (char*)pme->m_md.pData);
            // for recording AAC, use AEECLSID_MEDIAAAC
            nRet = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MEDIAAAC, (void **)&pme->m_pMedia);
            if (nRet == SUCCESS)
            {
              DBGPRINTF_HIGH("MP: AAC recording, %s.", (char*)pme->m_md.pData);
              nRet = IMEDIA_SetMediaData(pme->m_pMedia, &pme->m_md);
              if(SUCCESS != nRet)
              {
                DBGPRINTF_ERROR("MP: IMEDIA_SetMediaData faild, nRet = %d, md.clsData = %d, md.dwSize = %d, md.pData = %s",
                                nRet, pme->m_md.clsData, pme->m_md.dwSize, (char*)pme->m_md.pData);
              }
            }
            else
            {
              DBGPRINTF_ERROR("MP: failed to create media.");
            }
          }
          else
          {
            DBGPRINTF_ERROR("MP: pme->m_pMedia or pme->m_md.pData error.");
          }
        }
#endif  // FEATURE_AAC_REC
#ifdef FEATURE_AUDFMT_EVB
        else if(eCode == EVT_CREATEMEDIA_QCP && pme->m_pOwner->m_AudioFormat == MP_EVRCNB_RECORDING)
        {
          if(NULL == pme->m_pMedia && NULL != pme->m_md.pData)
          {
            AEECLSID cls;
            DBGPRINTF_HIGH("MP: EVRCNB record, %s", (char*)pme->m_md.pData);
            // for recording EVRC-NB, use AEECLSID_MediaEVRCNB
            cls = CMediaPlayer_GetAEECLSID(pme->m_pOwner, AUDIO_MIME_BASE, "evb");
            nRet = ISHELL_CreateInstance(pme->m_pIShell, cls, (void **)&pme->m_pMedia);
            if (nRet == SUCCESS)
            {
              DBGPRINTF_HIGH("MP: EVRCNB recording, %s.", (char*)pme->m_md.pData);
              nRet = IMEDIA_SetMediaData(pme->m_pMedia, &pme->m_md);
              if(SUCCESS != nRet)
              {
                DBGPRINTF_ERROR("MP: IMEDIA_SetMediaData faild, nRet = %d, md.clsData = %d, md.dwSize = %d, md.pData = %s",
                                nRet, pme->m_md.clsData, pme->m_md.dwSize, (char*)pme->m_md.pData);
              }
            }
            else
            {
              DBGPRINTF_ERROR("MP: failed to create media.");
            }
          }
          else
          {
            DBGPRINTF_ERROR("MP: pme->m_pMedia or pme->m_md.pData error.");
          }
        }
#endif  // FEATURE_EVRCNB_REC
        else if (eCode == EVT_CREATEMEDIA_QCP)
        {
          if(NULL == pme->m_pMedia && NULL != pme->m_md.pData)
          {
            DBGPRINTF_HIGH("MP: QCP record, %s", (char*)pme->m_md.pData);
            nRet = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MEDIAQCP, (void **)&pme->m_pMedia);
            if (nRet == SUCCESS)
            {
              nRet = IMEDIA_SetMediaData(pme->m_pMedia, &pme->m_md);
              if(SUCCESS != nRet)
              {
                DBGPRINTF_ERROR("MP: IMEDIA_SetMediaData faild, nRet = %d, md.clsData = %d, md.dwSize = %d, md.pData = %s",
                                nRet, pme->m_md.clsData, pme->m_md.dwSize, (char*)pme->m_md.pData);
              }
            }
            else
            {
              DBGPRINTF_ERROR("MP: failed to create media.");
            }
          }
          else
          {
            DBGPRINTF_ERROR("MP: pme->m_pMedia or pme->m_md.pData error.");
          }
        }
        else
        {
          DBGPRINTF_ERROR("MP: Unsupported create media event type: eCode = %d.", eCode);
        }
      }
    }
#else
    else
    {
      if(!pme->m_pMedia)
      {
        nRet = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MEDIAQCP,
                                    (void **)&pme->m_pMedia);
        if(nRet != SUCCESS)
        {
          DBGPRINTF_ERROR("MP: failed to create media, ret = %d.", nRet);
        }
      }
      if (nRet == SUCCESS)
      {
        nRet = IMEDIA_SetMediaData(pme->m_pMedia, &pme->m_md);
        if(SUCCESS != nRet)
        {
          DBGPRINTF_ERROR("MP: IMEDIA_SetMediaData faild, nRet = %d", nRet);
        }
      }
      else
      {
        DBGPRINTF_ERROR("MP: failed to set media data, ret = %d.", nRet);
      }
    }
#endif // FEATURE_MP_RECORDER_SETTINGS

    if (SUCCESS != nRet)
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION

      DBGPRINTF_ERROR("MP: nRet is not SUCCESS, nRet = %d.", nRet);
      MP_ErrorDlg(pme->m_pOwner, IDS_ERR_CREATEMEDIA);
    }
    else if (!pme->m_pMedia || (pme->m_pMedia && (SUCCESS != (nRet = IMEDIA_RegisterNotify(pme->m_pMedia, CMediaPlayer_MediaNotify, pme->m_pOwner)))))
    {
      if(!pme->m_pMedia)
      {
        DBGPRINTF_ERROR("MP: NULL media ptr.");
      }
      else if(nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: faied on IMEDIA_RegisterNotify, nRet = %d.", nRet);
      }
      MP_ErrorDlg(pme->m_pOwner, IDS_ERR_MEDIA);
    }
    else
    {
      uint32    dwCaps;

      if (pme->m_pOwner->m_nContMode == CONT_MODE_ALL_FILES && pme->m_eWin != MPPW_RECORD)
      {
        CPlayerWin_RestoreDefaultMenu(pme);
      }

      (void)IMEDIA_GetMediaParm(pme->m_pMedia, MM_PARM_CAPS, (int32 *) & dwCaps, 0);
      pme->m_bImage = (dwCaps & MM_CAPS_VIDEO) ? TRUE : FALSE;
      if (!pme->m_bImage)
      {
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FULLSCRN);
      }
      else
      {
#if defined(FEATURE_APP_MPEG4) || defined(FEATURE_APP_REAL)
        AEECLSID  ClsId;
#endif //defined(FEATURE_APP_MPEG4) || defined(FEATURE_APP_REAL)
        AEERect   rectImage;

        (void)MEMCPY(&rectImage, &pme->m_rectImage, sizeof(AEERect));

#ifdef FEATURE_APP_MPEG4
        if ((IMEDIA_GetClassID(pme->m_pMedia, &ClsId) == SUCCESS) && (
#if defined(FEATURE_APP_REAL)
                                                                       (ClsId ==
                                                                        AEECLSID_MEDIAREAL)
                                                                       ||
#endif  // FEATURE_APP_REAL
                                                                       (ClsId ==
                                                                        AEECLSID_MEDIAMPEG4)))
        {
          AEEMediaMPEG4Spec *pSpec;
          int32     dwSize;

#if (defined(FEATURE_DTV_DVBH) && defined(MM_MP4_PARM_DESCRAMBLER_HANDLER))
          int       ret;
          if(pme->m_pOwner->m_DescramblerKey >= 0)
          {
            if(SUCCESS == (ret = IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_DESCRAMBLER_HANDLER, pme->m_pOwner->m_DescramblerKey, 0)))
            {
               DBGPRINTF_HIGH("MP: set DVB-H key = %d",pme->m_pOwner->m_DescramblerKey);
            }
            else
            {
               DBGPRINTF_HIGH("MP: failed to set DVB-H key = %d, ret = %d",pme->m_pOwner->m_DescramblerKey, ret);
            }
          }

#endif //#if (defined(FEATURE_DTV_DVBH) && defined(MM_MP4_PARM_DESCRAMBLER_HANDLER))


#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)

          DBGPRINTF_HIGH("MP: Replay at Stop Mode, %s.", (pme->m_pOwner->m_bReplayAtStop)?"On":"Off");
          if(pme->m_pOwner->m_bReplayAtStop)
          {
            DBGPRINTF_HIGH("MP: Previous file = %s", (pme->m_pOwner->m_strReplayFileName)?pme->m_pOwner->m_strReplayFileName:"NULL");
            DBGPRINTF_HIGH("MP: Current file  = %s", (pme->m_pszFile)?pme->m_pszFile:"NULL");

            if (pme->m_pOwner->m_strReplayFileName && pme->m_pszFile && 
                STRCMP(pme->m_pOwner->m_strReplayFileName , pme->m_pszFile) == 0)
            {
              // enable replay form stop mode only we are replaying same file
              if(SUCCESS == IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_START_FROM_POS, (int32)pme->m_dwPositionAtStop, 0))
              {
                DBGPRINTF_HIGH("MP: replay will be started at %d.", pme->m_dwPositionAtStop);
              }
              else
              {
                DBGPRINTF_ERROR("MP: failed to set MM_MP4_PARM_START_FROM_POS with %d.", pme->m_dwPositionAtStop);
              }
            }
          }
#endif //#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)


          // We are not in a CMXFile
          pme->m_bIsCMXFile = FALSE;

          /*
           * Get the media specification (i.e. clip meta data).
           */
          nRet = IMEDIA_GetMediaSpec(pme->m_pMedia, &pSpec, &dwSize);
          /*
           * If SUCCESS, copy the data.  Otherwise, either MM_PENDING
           * and it'll arrive via MM_CMD_GETMEDIAPARM callback
           */
          if (SUCCESS == nRet)
          {
            if (dwSize == sizeof(AEEMediaMPEG4Spec))
            {
              /*
               * Copying assumes that there are no pointers in
               * info structure
               */
              (void)MEMMOVE(&pme->m_mediaInfo, pSpec, sizeof(AEEMediaMPEG4Spec));
              pme->m_dwDuration = pme->m_mediaInfo.dwDuration;
              pme->m_bGotMediaInfo = TRUE;
            }
#if defined(FEATURE_APP_REAL)
            else if (dwSize == sizeof(AEEMediaRealSpec))
            {
              /*
               * Copying assumes that there are no pointers in
               * info structure
               */
              (void)MEMMOVE(&pme->m_mediaRealInfo, pSpec, sizeof(AEEMediaRealSpec));
              pme->m_dwDuration = pme->m_mediaRealInfo.dwDuration;
              pme->m_bGotMediaInfo = TRUE;
            }
#endif  // FEATURE_APP_REAL
          }

#ifdef FEATURE_MMOVERLAY
          if (pme->m_pOwner->m_bOverlays)
          {
            mp_enableFrameCB = pme->m_pOwner->m_bFrameCallbacks = TRUE;
            DBGPRINTF_MED("MP: Enabling FrameCallbacks");
          }
          else if (pme->m_pOwner->m_bFrameCallbacks)
          {
            mp_enableFrameCB = TRUE;
          }
          else
#endif // FEATURE_MMOVERLAY
          {
            mp_enableFrameCB = FALSE;
          }
          /*
           * Enable frame callbacks
           */
          nRet = IMEDIA_EnableFrameCallback(pme->m_pMedia, mp_enableFrameCB);
          /*
           * Initialize the dimensions of the frame
           */
          pme->m_mp4PrevHeight = pme->m_mp4PrevWidth = 0;

          (void)IMEDIA_SetMediaParm(pme->m_pMedia,
                                    MM_MP4_PARM_LOOP_TRACK,
                                    (int32)pme->m_bLoopTrack,
                                    0);

#if defined(FEATURE_APP_REAL)
          if (ClsId == AEECLSID_MEDIAREAL)
          {
            /*
             * Add the RealOne Splash Screen
             */
            (void)CMediaPlayer_SetIdleImage(pme, IDB_REAL_SPLASH);

            /*
             * Delete the full screen icon, and add the RealOne Logo Icon
             */
#ifndef FEATURE_QTV_MDP_TRANSFORMATIONS
            (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FULLSCRN);
#endif
            (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_INFO);
            if(pme->m_bDIsplayInfo)
              CPlayerWin_Info_Clear(pme);
            (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_REAL_LOGO,
                           IDM_PM_INFO, 0);
          }
#endif  // FEATURE_APP_REAL
        }

        /*
         * Enable RTSP status code notification
         */
        (void)IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_RTSP_STATUS_NOTIFY, TRUE, 0);
#endif     // MP_FEATURE_VIDEO
        pme->m_dwTickInterval = 1000;  // in milliseconds
        nRet = IMEDIA_SetTickTime(pme->m_pMedia, pme->m_dwTickInterval);
        DBGPRINTF_MED("MP: IMEDIA_SetTickTime(%d) returned %d", pme->m_dwTickInterval, nRet);
        DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with x = %d, y = %d, cx = %d, cy = %d",
                      rectImage.x, rectImage.y, rectImage.dx, rectImage.dy);
        (void)IMEDIA_SetRect(pme->m_pMedia, &rectImage, NULL);
      }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
      CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
    }

    // when started from EVT_APP_RESUME, we don't want auto start
    if (pme->m_pOwner->m_bAutoPlay && eCode != EVT_CREATEMEDIA_QCP && !pme->m_pOwner->m_bDisableAutoPlayOnce)
    {
      DBGPRINTF_MED("MP: Calling CPlayerWin_AutoPlayClip because m_bAutoPlay = %d && eCode (%d) != EVT_CREATEMEDIA_QCP && !disableAutplay (%d)",
                pme->m_pOwner->m_bAutoPlay, eCode, pme->m_pOwner->m_bDisableAutoPlayOnce);
      CPlayerWin_AutoPlayClip(pme->m_pOwner);
      //(void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
    }

    if(pme->m_eWin == MPPW_RECORD)
    {
    
#ifdef FEATURE_MP_RECORDER_SETTINGS
      if (eCode == EVT_CREATEMEDIA_QCP)
      {
        /*
         * we don't set format type for PCM recorder
         */
        if (pme->m_pOwner->m_AudioFormat != MP_8K_PCM_RECORDING
#ifdef FEATURE_AAC_REC
          && pme->m_pOwner->m_AudioFormat != MP_AAC_RECORDING
#endif //FEATURE_AAC_REC
#ifdef FEATURE_AUDFMT_EVB
          && pme->m_pOwner->m_AudioFormat != MP_EVRCNB_RECORDING
#endif //FEATURE_EVRCNB_REC
           )

        {
          if(pme->m_pMedia)
          {
            nRet = IMEDIA_SetMediaParm(pme->m_pMedia, MM_PARM_QCP_FORMAT, (int32)pme->m_pOwner->m_AudioFormat, 0);
            if(nRet == SUCCESS || nRet == MM_PENDING)
            {
              DBGPRINTF_HIGH("MP: set recording format %d, ret %d", (int32)pme->m_pOwner->m_AudioFormat, nRet);
            }
            else
            {
              DBGPRINTF_ERROR("MP: Failed to set recording format %d, ret %d", (int32)pme->m_pOwner->m_AudioFormat, nRet);
            }
          }
          if(pme->m_pMedia1)
          {
            nRet = IMEDIA_SetMediaParm(pme->m_pMedia1, MM_PARM_QCP_FORMAT, (int32)pme->m_pOwner->m_AudioFormat, 0);
            if(nRet == SUCCESS || nRet == MM_PENDING)
            {
              DBGPRINTF_HIGH("MP: set recording format %d, ret %d", (int32)pme->m_pOwner->m_AudioFormat, nRet);
            }
            else
            {
              DBGPRINTF_ERROR("MP: Failed to set recording format %d, ret %d", (int32)pme->m_pOwner->m_AudioFormat, nRet);
            }
          }
        }
#if BREW_VERSION_LESS_THAN(4, 0)    
        if(pme->m_pMedia1)
        {
          if(pme->m_pMedia)
          {
            // set media to record uplink
            nRet = IMEDIA_SetMediaParm(pme->m_pMedia, MM_PARM_AUDIO_PATH, (int32)MM_APATH_LOCAL, 0);
            if(nRet == SUCCESS || nRet == MM_PENDING)
            {
              DBGPRINTF_HIGH("MP: Set Media audio path:%d[MM_APATH_LOCAL], ret %d", (int32)MM_APATH_LOCAL, nRet);
            }
            else
            {
              DBGPRINTF_ERROR("MP: Failed to set Media audio path: %d[MM_APATH_LOCAL], ret %d = %s", (int32)MM_APATH_LOCAL, nRet,
                              MP_ErrorStateToString(nRet));
            }
          }
    
          // set media1 to record downlink
          nRet = IMEDIA_SetMediaParm(pme->m_pMedia1, MM_PARM_AUDIO_PATH, (int32)MM_APATH_REMOTE, 0);
          if(nRet == SUCCESS || nRet == MM_PENDING)
          {
            DBGPRINTF_HIGH("MP: Set media1 audio path:%d[MM_APATH_LOCAL], ret %d", (int32)MM_APATH_REMOTE, nRet);
          }
          else
          {
            DBGPRINTF_ERROR("MP: Failed to set media1 audio path: %d[MM_APATH_LOCAL], ret %d = %s", (int32)MM_APATH_REMOTE, nRet,
                             MP_ErrorStateToString(nRet));
          }

          nRet = IMEDIA_SetMediaParm(pme->m_pMedia1, MM_PARM_AUDIO_PATH, (int32)MM_APATH_BOTH, 0);
          if(nRet == SUCCESS || nRet == MM_PENDING)
          {
            DBGPRINTF_HIGH("MP: Set media1 audio path:%d[MM_APATH_LOCAL], ret %d", (int32)MM_APATH_BOTH, nRet);
          }
          else
          {
            DBGPRINTF_ERROR("MP: Failed to set media1 audio path: %d[MM_APATH_LOCAL], ret %d = %s", (int32)MM_APATH_BOTH, nRet,
                             MP_ErrorStateToString(nRet));
          }
        }
        else
#endif //BREW_VERSION_LESS_THAN(4, 0)
        {
          if(pme->m_pMedia)
          {
            nRet = IMEDIA_SetMediaParm(pme->m_pMedia, MM_PARM_AUDIO_PATH, (int32)pme->m_pOwner->m_RecordPath, 0);
            if(nRet == SUCCESS || nRet == MM_PENDING)
            {
              DBGPRINTF_HIGH("MP: Set audio path: %d, ret %d", (int32)pme->m_pOwner->m_RecordPath, nRet);
            }
            else
            {
              DBGPRINTF_ERROR("MP: Failed to set audio path: %d, ret %d = %s", (int32)pme->m_pOwner->m_RecordPath, nRet,
                              MP_ErrorStateToString(nRet));
            }
          }
        }
      }
#endif  // FEATURE_MP_RECORDER_SETTINGS
    }

    // if this has been enabled disable it
    pme->m_pOwner->m_bDisableAutoPlayOnce = FALSE;
    nReturn = 1;
    return nReturn;
  }
  // not EVT_CREATEMEDIA event
  return nReturn;
}
#endif //FEATURE_APP_MEDIA_VIDEO

/*===========================================================================
   This function will insert string into file name for record
   e.g
     pSrc = memo_amr_000.qcp
     pString = _dl
     pDest = memo_arm_dl_000.qcp
===========================================================================*/
boolean MP_InsertStringToFileName(char* pSrc, char*pString, char* pDest, int nDestSize)
{
  if(pSrc && pString && pDest)
  {
    char* pstr;
    char* pstr1;
    int nLength;

    (void)MP_GetFileNameExtension(pSrc);
    pstr = STRSTR( pSrc, (const char*)"." );
    pstr1 = STRRCHR(pSrc, '_');
    if(pstr == NULL || pstr1 == NULL)
      return FALSE;

    nLength = (pstr1 - pSrc);
    nLength = MIN(nLength + 1, nDestSize);

    (void)STRLCPY(pDest, pSrc, nLength);
    (void)STRLCAT(pDest, pString, nDestSize);
    (void)STRLCAT(pDest, pstr1, nDestSize);

    return TRUE;
  }
  return FALSE;
}


/*===========================================================================
   This function indicates IMedia is in playing state.
===========================================================================*/
boolean CPlayerWin_IsPlaying(CPlayerWin *pme)
{
  boolean   bChg = 0;
  int       nState = 0;

  if (pme->m_pMedia)
    nState = IMEDIA_GetState(pme->m_pMedia, &bChg);

  DBGPRINTF_MED("MP: CPlayerWin_IsPlaying() AEE MM_STATE=%d[%s], bChg=%d", 
                 nState, MP_MediaStateToString(nState), bChg);
  if (bChg)
    return FALSE;

  return (nState == MM_STATE_PLAY);
}

#ifdef FEATURE_BT_EXTPF_AV
/*===========================================================================
   This function checks to see if the playback is still playing and, if so,
   requests that it go to the PAUSE state.
===========================================================================*/
static boolean
CPlayerWin_PausePlay(CPlayerWin *pWin)
{
  boolean bPause = FALSE;

  if (pWin)
  {
    if (CPlayerWin_IsPlaying(pWin) &&
      (pWin->m_pOwner->m_playState == MP_PLAYSTATE_PLAYING ||
       pWin->m_pOwner->m_playState == MP_PLAYSTATE_BUFFERING))
    {
      (void)ISHELL_PostEvent(pWin->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                             EVT_COMMAND, IDM_PM_PAUSE, 0);
      bPause = TRUE;
    }
  }

  if (bPause)
  {
    DBGPRINTF_MED("MP: CPlayerWin - Pause Request");
  }
  else
  {
    DBGPRINTF_MED("MP: CPlayerWin - No Pause Request");
  }
  return bPause;
}
#endif // FEATURE_BT_EXTPF_AV

/*===========================================================================
   This function indicates IMedia is in paused state.
===========================================================================*/
static boolean
CPlayerWin_IsPause(CPlayerWin *pme)
{
  boolean   bChg = 0;
  int       nState = 0;

  if (pme->m_pMedia)
    nState = IMEDIA_GetState(pme->m_pMedia, &bChg);

  DBGPRINTF_MED("MP: CPlayerWin_IsPause() AEE MM_STATE = %d = %s", nState, MP_MediaStateToString(nState));
  if (bChg)
    return FALSE;

  return (nState == MM_STATE_PLAY_PAUSE || nState == MM_STATE_RECORD_PAUSE);
}

/*===========================================================================
   This function indicates IMedia is in the ready state.
===========================================================================*/
static boolean
CPlayerWin_IsReady(CPlayerWin *pme)
{
  boolean   bChg = 0;
  int       nState = 0;

  if (pme->m_pMedia)
    nState = IMEDIA_GetState(pme->m_pMedia, &bChg);

  DBGPRINTF_MED("MP: CPlayerWin_IsReady() AEE MM_STATE = %d = %s", nState, MP_MediaStateToString(nState));
  if (bChg)
    return FALSE;

  return (nState == MM_STATE_READY);
}

/*===========================================================================
   This function indicates IMedia is recording.
===========================================================================*/
static boolean
CPlayerWin_IsRecording(CPlayerWin *pme)
{
  boolean   bChg = 0;
  int       nState = 0;

  if (pme->m_pMedia)
    nState = IMEDIA_GetState(pme->m_pMedia, &bChg);

  DBGPRINTF_MED("MP: AEE MM_STATE = %d = %s", nState, MP_MediaStateToString(nState));
  if (bChg)
    return FALSE;

  return ((nState == MM_STATE_RECORD));
}

/*===========================================================================
   This function saves the file name and sets the media data locally.
   Assumption: pmd is not NULL.
===========================================================================*/
static boolean
CPlayerWin_SetMediaData(CPlayerWin *pme, AEEMediaData *pmd)
{
  boolean bDeleteBuffer = TRUE;

  if (!pmd->pData)
    return FALSE;

  if((char*)pme->m_pszFile == (char*)pme->m_md.pData)
  {
    // Both point to same memory, set bDeleteBuffer to prevent double free
    bDeleteBuffer = FALSE;
  }

  /*
   * free any previous memory. This is valid in continuous mode
   */
  if (pme->m_pszFile)
    FREEIF(pme->m_pszFile);

  // Free the memory we previously allocated.
  // This is only needed if it is MMD_BUFFER. if it is MMD_FILE_NAME then m_pszFile
  // Also points to the same memory and is freed by it
  if (pme->m_md.pData && pme->m_md.clsData == MMD_BUFFER && bDeleteBuffer)
  {
    FREEIF(pme->m_md.pData);
  }

  pme->m_pszFile = (char *)pmd->pData;
  (void)MEMCPY(&pme->m_md, pmd, sizeof(AEEMediaData));

  return TRUE;
}


/*==========================================================================
   If the filename ends in "pvx", then it is FastTrack Packet Video.
==========================================================================*/
static boolean
CPlayerWin_IsPacketVideo(char *filename)
{
  char     *extName;

  if (!filename)
  {
    return FALSE;
  }

  /*
   * Get the filename extension.
   */
  extName = MP_GetFileNameExtension(filename);
  if (!extName)
  {
    return FALSE;
  }

  if (!STRICMP(extName, "pvx"))        // Case insensitive search for
                                       // "pvx"
    return TRUE;

  return FALSE;
}

/*===========================================================================
   This function indicates if IMedia is busy
===========================================================================*/
static boolean
CPlayerWin_IsMediaBusy(CPlayerWin *pme)
{
  boolean bRet;

  if (pme->m_MediaBusy == MEDIA_NOT_BUSY)
  {
    bRet = FALSE;
  }
  else
  {
    DBGPRINTF_MED("MP: Media is Busy, 0x%x", pme->m_MediaBusy);
    bRet = TRUE;
  }

  return bRet;
}

/*===========================================================================
   This function  sets busy status for the Media
===========================================================================*/
static void
CPlayerWin_SetMediaBusy(CPlayerWin *pme, CMediaBusy mask)
{
  pme->m_MediaBusy |= mask;
  DBGPRINTF_MED("MP: Setting MediaBusy, 0x%x, 0x%x", mask, pme->m_MediaBusy);
}

/*===========================================================================
   This function clears busy status for the Media
===========================================================================*/
static void
CPlayerWin_ClearMediaBusy(CPlayerWin *pme, CMediaBusy mask)
{
  pme->m_MediaBusy &= ~mask;
  DBGPRINTF_MED("MP: Clearing MediaBusy, 0x%x, 0x%x", mask, pme->m_MediaBusy);
}


#ifdef FEATURE_MP_EQ_UI
/*===========================================================================
   This function will show CPlayerWin setting screen
===========================================================================*/
static void CPlayerWin_ShowSettingWin(CPlayerWin *pme)
{
  AEERect rcScreen;
  AEERect rcPanel;  

  //make sure m_pSetting is created
  CMediaPlayer_AudioSetupScreen(pme->m_pOwner);
  if(pme && pme->m_pOwner && MPShowSetupWin(pme->m_pOwner->m_pSetting, TRUE))
  {
    DBGPRINTF_MED("MP: CPlayerWin_ShowSettingWin, get Rect");
    // Since we are going to show setting win, resize display rect to 
    // avoid overlay, which may cause flickering
    if(MPSetupWinGetSettingPanelRect(pme->m_pOwner->m_pSetting->m_pSetupWin, &rcPanel))
    {
      (void)MEMCPY(&rcScreen, &pme->m_rectImage, sizeof(AEERect));
      rcScreen.dy = (rcScreen.dy - rcPanel.dy);

#ifdef FEATURE_MMOVERLAY
      /*
       * If overlays are enabled, use IMMOVERLAY methods
       * to define Image and Setups regions.
       */
      if (pme->m_pOwner->m_bOverlays)
      {
        uint32     nRet;

        nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_DEST_REGION, (uint32)&rcScreen, 
                OV_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER | OV_FLAG_DEST_REGION_ALIGN_VERT_CENTER);
        if (SUCCESS != nRet)
        {
          DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_DEST_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
        }
        nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_SRC_REGION, (uint32)&rcScreen, 0);
        if (SUCCESS != nRet)
        {
          DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_SRC_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
        }
        nRet = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayImage, NULL, 0);
        if (nRet == MP_ERR_IMMOVERLAY)
        {
          DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
        }
        nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlaySetups, OV_PARM_DEST_REGION, (uint32)&rcPanel, 0);
        if (SUCCESS != nRet)
        {
          DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_DEST_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
        }
        nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlaySetups, OV_PARM_SRC_REGION, (uint32)&rcPanel, 0);
        if (SUCCESS != nRet)
        {
          DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_SRC_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
        }
        nRet = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlaySetups, NULL, 0);
        if (nRet == MP_ERR_IMMOVERLAY)
        {
          DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
        }
      }
      else 
#endif // FEATURE_MMOVERLAY
      {
        DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with x = %d, y = %d, cx = %d, cy = %d", rcScreen.x, rcScreen.y, rcScreen.dx, rcScreen.dy);
        (void)IMEDIA_SetRect(pme->m_pMedia, &rcScreen, NULL);
      }
    }
    DBGPRINTF_MED("MP: CPlayerWin_ShowSettingWin, redraw");
    CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
  }
}
#endif //FEATURE_MP_EQ_UI

/*===========================================================================
   This function will do followings:
     Initialize ISoundCtl if it is not
     Create IGraphics in CMediaPlayer
     if m_bShowPlayerWinSettingUI create EqalizerUI, otherwise delete
===========================================================================*/
#ifdef FEATURE_MP_EQ_UI
static void CMediaPlayer_AudioSetupScreen(CMediaPlayer * pme)
{
  AEERect rcPanel;
  int nEqMax = MP_EQUALIZER_GAIN_MAX;
  int nEqMin = MP_EQUALIZER_GAIN_MIN;

  // calculate setting win rect
  rcPanel.x = 0;
  rcPanel.dx = pme->m_cxWidth;
  rcPanel.y = pme->m_nLChSize + 1;
  rcPanel.dy = pme->m_cyHeight - rcPanel.y - pme->m_nNChSize - pme->m_IconViewCtlDy - 1;

  // create IGraphics
  if(NULL == pme->m_pIGraphics)
  {
    if(ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_GRAPHICS,(void**)&pme->m_pIGraphics) != SUCCESS)
    {
      DBGPRINTF_ERROR("MP: error on creating IGraphics.");
      return;
    }
    (void)IGRAPHICS_EnableDoubleBuffer(pme->m_pIGraphics, TRUE);
  }

#ifdef FEATURE_MP_EQ
  // init ISoundCtl
  CMediaPlayer_InitISoundCtl(pme);
#endif //FEATURE_MP_EQ

  if(pme->m_bShowPlayerWinSettingUI)
  {
    if(pme->m_pSetting && NULL == pme->m_pSetting->m_pSetupWin)
    {
      pme->m_pSetting->m_pSetupWin = (void*)MPSetupWinCreate(pme->a.m_pIShell, pme->a.m_pIDisplay, pme->m_pSetting, rcPanel, nEqMin, nEqMax);
      if(pme->m_pSetting->m_pSetupWin == NULL)
      {
        (void)IGRAPHICS_Release(pme->m_pIGraphics);
        pme->m_pIGraphics = NULL;
        DBGPRINTF_ERROR("MP: error on creating SettingWin.");
        return;
      }
      // enable setting win, but set focus off
      MPSetupWinSetEnable(pme->m_pSetting->m_pSetupWin, TRUE);
      MPSetupWinSetFocus(pme->m_pSetting->m_pSetupWin, FALSE);

      // load default settings
      MPSetupWinRefreshAudioSettings(pme->m_pSetting->m_pSetupWin);
      CMediaPlayer_Redraw(pme, FALSE);
    }
  }
}
#endif //FEATURE_MP_EQ_UI
/*===========================================================================
   ISoundCtl callback function
===========================================================================*/
#if (defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))
static void CMediaPlayer_SoundCtlCBFn(void* pUser, AEESoundCtlNotify* pfnNotify)
{
  CMediaPlayer *   pme = (CMediaPlayer *)pUser;
  if (pme == NULL || pfnNotify == NULL)
  {
    DBGPRINTF_ERROR("MP: SoundCtlCBFn received NULL ptr, pUser %d, pNotify %d", pUser, pfnNotify);
    return;
  }

  if(AEE_SOUNDCTL_SUCCESS != pfnNotify->nStatus)
  {
    char   szCMD[64] = {'\0'};
    char   szStatus[64] = {'\0'};
    (void)CMediaPlayer_GetSoundCtlCBCMDString(pfnNotify->nCmd, szCMD, 64);
    (void)CMediaPlayer_GetSoundCtlStatusString(pfnNotify->nStatus, szStatus, 64);
    // report error information
    DBGPRINTF_ERROR( "MP: SoundCtlCBFn return with error, cmd=%d[%s]; callback status=%d[%s]",
                    pfnNotify->nCmd, szCMD, pfnNotify->nStatus, szStatus);
    return;
  }

  if(pfnNotify->nCmd == AEE_SOUNDCTL_CMD_SPECTRUM_ANALYZER_DATA)
  {
    if( pme->m_eActiveWin == MPW_PLAYER )
    {
      if(CPlayerWin_IsVideoMedia((CPlayerWin*)pme->m_pWin))
      {
        // media has video, do not draw spectrum
        return;
      }
    }
    // spectrum data
    if(pme->m_pSetting->m_pSetupWin && pfnNotify->dwDataSize)
    {
      if(pme->m_pIGraphics)
      {
        // draw and update data graph
        MPSetupWinSetSpectrumData(pme->m_pSetting->m_pSetupWin, pfnNotify->pData, pfnNotify->dwDataSize);
        (void)MPSetupWinDrawAnalyzer(pme->m_pSetting->m_pSetupWin, pme->m_pIGraphics);
        IGRAPHICS_Update(pme->m_pIGraphics);
      }
    }
    else if(pfnNotify->dwDataSize == 0)
    {
      DBGPRINTF_ERROR("MP: SoundCtlCBFn return 0 size of spectrum data");
    }
  }
  else
  {
    char   szCMD[64] = {'\0'};
    char   szStatus[64] = {'\0'};
    (void)CMediaPlayer_GetSoundCtlCBCMDString(pfnNotify->nCmd, szCMD, 64);
    (void)CMediaPlayer_GetSoundCtlStatusString(pfnNotify->nStatus, szStatus, 64);
    //print cmd and status information
    DBGPRINTF_LOW( "MP: SoundCtlCBFn, cmd=%d[%s]; callback status=%d[%s]",
                    pfnNotify->nCmd, szCMD, pfnNotify->nStatus, szStatus);
  }
}
#endif // defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ)
/*===========================================================================
   This function converts cmd status string from cmd id.
===========================================================================*/
#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))
static char* CMediaPlayer_GetSoundCtlCBCMDString(int16 cmd, char* pBuf, int size)
{
   if(pBuf == NULL)
     return pBuf;

   switch(cmd)
   {
   case AEE_SOUNDCTL_CMD_ENABLE_DISABLE_EQUALIZER:
     (void)STRLCPY(pBuf, "ENABLE_DISABLE_EQUALIZER", size);
     break;
   case AEE_SOUNDCTL_CMD_SET_EQUALIZER:
     (void)STRLCPY(pBuf, "SET_EQUALIZER", size);
     break;
   case AEE_SOUNDCTL_CMD_SET_EQUALIZER_GAIN:
     (void)STRLCPY(pBuf, "SET_EQUALIZER_GAIN", size);
     break;
   case AEE_SOUNDCTL_CMD_ENABLE_DISABLE_AUDIO_EXPANDER:
     (void)STRLCPY(pBuf, "ENABLE_DISABLE_AUDIO_EXPANDER", size);
     break;
   case AEE_SOUNDCTL_CMD_SET_AUDIO_EXPANDER:
     (void)STRLCPY(pBuf, "SET_AUDIO_EXPANDER", size);
     break;
   case AEE_SOUNDCTL_CMD_ENABLE_DISABLE_SPECTRUM_ANALYZER:
     (void)STRLCPY(pBuf, "ENABLE_DISABLE_SPECTRUM_ANALYZER", size);
     break;
   case AEE_SOUNDCTL_CMD_SET_SPECTRUM_ANALYZER:
     (void)STRLCPY(pBuf, "SET_SPECTRUM_ANALYZER", size);
     break;
   case AEE_SOUNDCTL_CMD_SPECTRUM_ANALYZER_DATA:
     (void)STRLCPY(pBuf, "SPECTRUM_ANALYZER_DATA", size);
     break;
   default:
     (void)STRLCPY(pBuf, "unknown", size);
     break;
   }
   return pBuf;
}
#endif //defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ)
/*===========================================================================
   This function converts SoundCtl status string from id.
===========================================================================*/
#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))
static char* CMediaPlayer_GetSoundCtlStatusString(int16 status, char* pBuf, int size)
{
  if(pBuf == NULL)
    return pBuf;

  switch(status)
  {
  case AEE_SOUNDCTL_SUCCESS:
     (void)STRLCPY(pBuf, "SUCCESS", size);
    break;
  case AEE_SOUNDCTL_FAILURE:
     (void)STRLCPY(pBuf, "FAILURE", size);
    break;
  case AEE_SOUNDCTL_UNKNOWN:
     (void)STRLCPY(pBuf, "UNKNOWN", size);
    break;
  case AEE_SOUNDCTL_UNSUPPORTED:
     (void)STRLCPY(pBuf, "UNSUPPORTED", size);
    break;
  default:
     (void)STRLCPY(pBuf, "unknown", size);
    break;
  }
  return pBuf;
}
#endif //defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ)
/*===========================================================================
   This function initialize ISoundCtl.
===========================================================================*/
#if(defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ))
static void CMediaPlayer_InitISoundCtl(CMediaPlayer * pme)
{
  char   szStatus[64] = {'\0'};
  int    ret;

  // return if it the interface object has been created already
  if(pme->m_pSetting->m_pSoundCtl)
    return;

  if(SUCCESS == ISHELL_CreateInstance(pme->a.m_pIShell,
                                      AEECLSID_SOUNDCTL,
                                      (void **)&pme->m_pSetting->m_pSoundCtl))
  {
    // register ISouncdCtl callback
    ret = ISOUNDCTL_RegisterNotify(pme->m_pSetting->m_pSoundCtl,
                                   (PFNSOUNDCTLNOTIFY)&CMediaPlayer_SoundCtlCBFn,
                                   (const void *)pme );
    if(SUCCESS != ret)
      DBGPRINTF_ERROR("MP: ISOUNDCTL_RegisterNotify return error: %s",
                      CMediaPlayer_GetSoundCtlStatusString(ret, szStatus, 64));
  }
  else
  {
    // failed to create ISoundCtl instance, make sure ptr is NULL
    pme->m_pSetting->m_pSoundCtl = NULL;
    DBGPRINTF_ERROR("MP: Error on creating ISoundCtl");
  }
}
#endif //defined( FEATURE_MP_EQ_UI) && defined( FEATURE_MP_EQ)

#if defined(MP_FEATURE_VIDEO)

#define END_CHAR_COUNT 5
const char *
MP_GetEndLocation(const char *startLoc)
{
  char      endChar[END_CHAR_COUNT] = {
    ' ',                               // Space
    '\n',                              // Carriage return
    '\r',                              // Line feed
    '\t',                              // Tab
    '>'                                // HTML end comment character
  };
  const char *endLoc[END_CHAR_COUNT];
  int       i;

  for (i = 0; i < END_CHAR_COUNT; i++)
  {
    endLoc[i] = STRCHR(startLoc, endChar[i]);
  }

  /*
   * figure out which one is first
   */
  for (i = 1; i < END_CHAR_COUNT; i++)
  {
    if (endLoc[0] == NULL || (endLoc[i] != NULL && endLoc[i] < endLoc[0]))
    {
      endLoc[0] = endLoc[i];
    }
  }
  return endLoc[0];
}

static boolean
MP_ParseValue(const char *input, const char *keyWord, char *value,
              int valueLength)
{
  const char *inputStr = input;
  const char *startLoc;
  const char *endLoc;
  boolean   result = FALSE;

  value[0] = '\0';

  startLoc = STRSTR(inputStr, keyWord);
  if (startLoc != NULL)
  {
    startLoc = STRCHR(startLoc, '=');
    if (startLoc != NULL)
    {
      result = TRUE;
      /*
       * skip the '='
       */
      startLoc++;

      /*
       * skip white spaces
       */
      while (isspace(*startLoc))
        startLoc++;

      /*
       * Account for quoted strings
       */
      if (*startLoc == '\"')
      {
        /*
         * skip the '"'
         */
        startLoc++;
        endLoc = STRCHR(startLoc, '\"');
      }
      else
      {
        endLoc = MP_GetEndLocation(startLoc);
      }


      if (endLoc != NULL)
      {
        (void)STRLCPY(value, startLoc, MIN(endLoc - startLoc + 1, valueLength));
      }
      else
      {
        (void)STRLCPY(value, startLoc, valueLength);
      }
    }
  }
  return result;
}

/*
 * ===========================================================================
 * FUNCTION MP_GetAVFileNames DESCRIPTION Extracts the audio/video file paths
 * from the given file. It also extracts the track loop value. returns FALSE if
 * file is not MFPT. The format of file is: video_path=brew/media/...
 * audio_path=brew/media/... track_loop=TRUE
 * ==============================================================================
 */
static boolean
MP_GetAVFileNames(IShell *ps, const char *pFile, char *pVideoFile,
                  char *pAudioFile, int fileBufSize, boolean *pbLoop)
{
  const char *videoTag = "video_path";
  const char *audioTag = "audio_path";
  const char *loopTag = "track_loop";
  static char buf[AEE_MAX_FILE_NAME * 6];
  char      loop[16];
  int       nRet;

  IFileMgr *pIFileMgr;
  IFile    *pf;

  (void)ISHELL_CreateInstance(ps, AEECLSID_FILEMGR, (void **)&pIFileMgr);

  if (!pIFileMgr)
    return FALSE;

  pf = IFILEMGR_OpenFile(pIFileMgr, pFile, _OFM_READ);
  if (!pf)
  {
    (void)IFILEMGR_Release(pIFileMgr);
    return FALSE;
  }

  (void)MEMSET(buf, 0, sizeof(buf));
  nRet = IFILE_Read(pf, buf, sizeof(buf));
  buf[sizeof(buf) - 1] = '\0';
  (void)IFILE_Release(pf);
  (void)IFILEMGR_Release(pIFileMgr);

  if (nRet == 0)
  {
    return FALSE;
  }

  if (!MP_ParseValue(buf, videoTag, pVideoFile, fileBufSize))
  {
    pVideoFile[0] = '\0';
  }

  if (!MP_ParseValue(buf, audioTag, pAudioFile, fileBufSize))
  {
    pAudioFile[0] = '\0';
  }

  if (!MP_ParseValue(buf, loopTag, loop, sizeof(loop)))
  {
    loop[0] = '\0';
  }

  if (STRICMP(loop, "true") == 0)
    *pbLoop = TRUE;
  else
    *pbLoop = FALSE;

  return TRUE;
}

static boolean
MP_PrepMediaDataEx(AEEMediaDataEx *pmd, char *pFile, int nCaps)
{
  pmd->clsData = MMD_FILE_NAME;
  pmd->pData = MALLOC(STRLEN(pFile) + 1);
  if (!pmd->pData)
    return FALSE;
  (void)STRLCPY((char *)pmd->pData, pFile, (STRLEN(pFile) + 1));
  pmd->dwSize = STRLEN(pFile);
  pmd->dwStructSize = sizeof(AEEMediaDataEx);
  pmd->dwCaps = nCaps;
  return TRUE;
}

static void
MP_FreeMediaDataExList(AEEMediaDataEx *pmd, int nCount)
{
  int       i;

  if (pmd)
  {
    for (i = 0; i < nCount; i++)
    {
      if (pmd[i].pData)
        FREE(pmd[i].pData);
    }
    FREE(pmd);
  }
}

/*===========================================================================
   This function allocate memory for media data and saves the file name and
   sets the media data. If file is MFPT type, it creates the list.
   Assumption: pmd is not NULL.
===========================================================================*/
static boolean
CPlayerWin_SetMediaDataEx(CPlayerWin *pme, char *pVideoFile, char *pAudioFile)
{
  /*
   * free if any old list is there
   */
  if (pme->m_pmdExList)
  {
    MP_FreeMediaDataExList(pme->m_pmdExList, pme->m_mdListCount);
    pme->m_pmdExList = NULL;
    pme->m_mdListCount = 0;
  }

  if (STRLEN(pVideoFile) && STRLEN(pAudioFile))
  {
    pme->m_mdListCount = 2;
    pme->m_pmdExList =
      (AEEMediaDataEx *)MALLOC(sizeof(AEEMediaDataEx) * pme->m_mdListCount);
    if (!pme->m_pmdExList)
      return FALSE;

    (void)MP_PrepMediaDataEx(&pme->m_pmdExList[0], pVideoFile, MM_CAPS_VIDEO);
    (void)MP_PrepMediaDataEx(&pme->m_pmdExList[1], pAudioFile, MM_CAPS_AUDIO);
  }
  else if (STRLEN(pAudioFile))
  {
    pme->m_mdListCount = 1;
    pme->m_pmdExList = (AEEMediaDataEx *)MALLOC(sizeof(AEEMediaDataEx));
    if (!pme->m_pmdExList)
      return FALSE;

    (void)MP_PrepMediaDataEx(&pme->m_pmdExList[0], pAudioFile, MM_CAPS_AUDIO);
  }
  else if (STRLEN(pVideoFile))
  {
    pme->m_mdListCount = 1;
    pme->m_pmdExList = (AEEMediaDataEx *)MALLOC(sizeof(AEEMediaDataEx));
    if (!pme->m_pmdExList)
      return FALSE;

    (void)MP_PrepMediaDataEx(&pme->m_pmdExList[0], pVideoFile, MM_CAPS_VIDEO);
  }
  return TRUE;
}

#endif     // MP_FEATURE_VIDEO

/*===========================================================================
   This function toggles full screen mode and does a redraw.
===========================================================================*/
static boolean CPlayerWin_FullScreen(CPlayerWin *pme, boolean bFull, boolean bDeferRedraw)
{
  /*
   * Support needs to be added for MDP implementation of full screen mode
   */
  int nRet;

  DBGPRINTF_MED("MP: CPlayerWin_FullScreen");

  if (!pme->m_bImage)
  {
    DBGPRINTF_HIGH("MP: Full Screen mode not supported for this type of file.");
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    return FALSE;
  }

  //If we are in paused state then no need to do anything
  if (CPlayerWin_IsPause(pme))
  {
      return FALSE;
  }

  // Check if action is needed
  if (pme->m_bFullScreen == bFull)
  {
    return TRUE;            // No need to do anything, we're in the right mode
  }

  // Set the Fullscreen flag
  DBGPRINTF_MED("MP: Setting FullScreen to %d", bFull);
  pme->m_bFullScreen = bFull;

  // Perform appropriate action 
  if ( pme->m_bFullScreen )     
  {
    //Going to fullscreen.
    AEEDeviceInfo *pdi;

#ifdef FEATURE_MMOVERLAY
    AEERect   srcRect;
#endif // FEATURE_MMOVERLAY

    // disable annunciators
    DBGPRINTF_MED("MP: Going to FullScreen - disable annunciators.");
    nRet = IDISPLAY_SetPrefs(pme->m_pIDisplay, "a:0", strlen("a:0"));
    if(nRet != SUCCESS)
    {
      DBGPRINTF_ERROR("MP: failed on IDISPLAY_SetPrefs, nRet = %d", nRet);
    }

    // Cancel progress message timer as it may try to draw progress message within a small window
    DBGPRINTF_MED("MP: Going to FullScreen - Cancel message timer.");
    MPProgCtl_MessageTimerCancel(pme->m_pMPProgCtl);

    if((pme->m_pOwner->m_cxWidthFull == 0) && (pme->m_pOwner->m_cyHeightFull == 0))
    {
      pdi = MALLOC(sizeof(AEEDeviceInfo));
      if(pdi)
      {
        // retrieve new screen size after disabling annunicators
        ISHELL_GetDeviceInfo(pme->m_pOwner->a.m_pIShell, pdi);
        pme->m_pOwner->m_cxWidthFull = pdi->cxScreen;
        pme->m_pOwner->m_cyHeightFull = pdi->cyScreen;
        DBGPRINTF_MED("MP: device info cxScreen = %d, cyScreen = %d", pdi->cxScreen, pdi->cyScreen);
        FREEIF(pdi);
      }
      else
      {
        DBGPRINTF_ERROR("MP: failed to allocate memory.");
      }
    }

#if defined(FEATURE_MMOVERLAY) && defined(FEATURE_APP_MPEG4)
    SETAEERECT(&srcRect, 0, 0, pme->m_mp4PrevWidth, pme->m_mp4PrevHeight);
    DBGPRINTF_MED("MP: setting srcRect to %d,%d x %d,%d", srcRect.x, srcRect.y, srcRect.dx, srcRect.dy);
#endif //#if defined(FEATURE_MMOVERLAY) && defined(FEATURE_APP_MPEG4)

    DBGPRINTF_MED("MP: setting ImageRect to %d x %d", pme->m_pOwner->m_cxWidthFull, pme->m_pOwner->m_cyHeightFull);
    SETAEERECT(&pme->m_rectImage, 0, 0, pme->m_pOwner->m_cxWidthFull, pme->m_pOwner->m_cyHeightFull);
       
    // Set the MenuRect, in case we run into a audio only file
    //IMENUCTL_GetRect(pme->m_pPlayerMenu, &rectMenu);
    //DBGPRINTF_MED("MP: PlayerMenu Rec was to x = %d, y = %d, dx = %d, dy = %d", rectMenu.x, rectMenu.y, rectMenu.dx, rectMenu.dy);
    //rectMenu.y = pme->m_pOwner->m_cyHeightFull - rectMenu.dy;
    //DBGPRINTF_MED("MP: Setting PlayerMenu to x = %d, y = %d, dx = %d, dy = %d", rectMenu.x, rectMenu.y, rectMenu.dx, rectMenu.dy);
    //(void)MEMCPY(&pme->m_rectMenu, &rectMenu, sizeof(AEERect));
    //IMENUCTL_SetRect(pme->m_pPlayerMenu, &rectMenu);
    //(void)IMENUCTL_Redraw(pme->m_pPlayerMenu);
     
    //Do AScaling if the media file is not an image.
#ifdef FEATURE_QTV_MDP_ASCALE 
#ifdef FEATURE_MMOVERLAY
    if (!pme->m_pOwner->m_bIsImage && !pme->m_pOwner->m_bOverlays)
#else
    if (!pme->m_pOwner->m_bIsImage)
#endif // FEATURE_MMOVERLAY
    {
      (void)CPlayerWin_SetAScale( pme );
    }
#endif // FEATURE_QTV_MDP_ASCALE 

#ifdef FEATURE_MMOVERLAY
    /*
     * If overlays are enabled, use IMMOVERLAY methods
     * to define and scale Image region.
     */
    if (pme->m_pOwner->m_bOverlays)
    {
      uint32 nRetVal;
      uint32 scaleFlag = OV_FLAG_SCALE_TO_FIT;
      DBGPRINTF_MED("MP: define FullScreen image");

      // Setup Video Image overlay
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_DEST_REGION, (uint32)&pme->m_rectImage, 0);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_SetParam(OV_PARM_DEST_REGION) Failed, nRet=%d", nRet);
      }
      // need to specify 256 as p2 for OV_PARM_SCALE_X/Y as that is the value that maps to
      // 1.0 for scaling MMOVERLAY stuff.  Documentation hints that this is ignored, but I
      // think that the doc is being updated. -GSB
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_SCALE_X, scaleFlag, 256);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_SetParam(OV_PARM_SCALE_X, %d, 256) Failed, nRet=%d", scaleFlag, nRet);
      }      
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_SCALE_Y, scaleFlag, 256);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_SetParam(OV_PARM_SCALE_Y, %d, 256) Failed, nRet=%d", scaleFlag, nRet);
      }

      // Update Alpha Blending layers
      CPlayerWin_SetLayer1Rect(pme);
      CPlayerWin_UpdateLayer1(pme);
      CPlayerWin_SetLayer2Rect(pme);
      CPlayerWin_UpdateLayer2(pme);

      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_SRC_REGION, (uint32)&srcRect, 0);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_SetParam(OV_PARM_SRC_REGION) Failed, nRet=%d", nRet);
      }    
      // Disable other overlays for now
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayTitle, OV_PARM_ENABLE, FALSE, 0);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_SetParam(OV_PARM_ENABLE) Failed, nRet=%d", nRet);
      }
      nRetVal = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayTitle, NULL, 0);
      if (nRetVal == MP_ERR_IMMOVERLAY)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
      }
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayInfo, OV_PARM_ENABLE, FALSE, 0);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_SetParam(OV_PARM_ENABLE) Failed, nRet=%d", nRet);
      }
      nRetVal = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayInfo, NULL, 0);
      if (nRetVal == MP_ERR_IMMOVERLAY)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
      }

      // For now, disable the Menu, in the future we want to blend the menu with
      // the image
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayMenu, OV_PARM_ENABLE, FALSE, 0);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_SetParam(OV_PARM_ENABLE) Failed, nRet=%d", nRet);
      }
      nRetVal = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayMenu, NULL, 0);
      if (nRetVal == MP_ERR_IMMOVERLAY)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
      }
    }
#endif // FEATURE_MMOVERLAY
  }
  else  // Coming out of fullscreen.
  {
    AEERect   rectMenu = {0};

    // Force a screen clearing immediately by calling UpdateEx with no delay before re-enabling annunciators
    DBGPRINTF_MED("MP: Turning off FullScreen: forcefully clearing display...");
    IDISPLAY_ClearScreen(pme->m_pIDisplay);
    IDISPLAY_SetClipRect(pme->m_pIDisplay, NULL);
    IDISPLAY_UpdateEx(pme->m_pIDisplay, FALSE);
 
    // enable annunciators
    DBGPRINTF_MED("MP: Turning off FullScreen: enabling annunciators.");
    nRet = IDISPLAY_SetPrefs(pme->m_pIDisplay, "a:1", strlen("a:1"));
    if(nRet != SUCCESS)
    {
      DBGPRINTF_ERROR("MP: failed on IDISPLAY_SetPrefs, nRet = %d", nRet);
    }
    
    // Restore the default image rect.
    (void)MEMCPY(&pme->m_rectImage, &pme->m_dfltRectImage, sizeof(AEERect));

    // Restore the player menu

    IMENUCTL_GetRect(pme->m_pPlayerMenu, &rectMenu);
    DBGPRINTF_MED("MP: PlayerMenu Rec was to x = %d, y = %d, dx = %d, dy = %d", rectMenu.x, rectMenu.y, rectMenu.dx, rectMenu.dy);
    rectMenu.y = pme->m_pOwner->m_cyHeight - pme->m_pOwner->m_IconViewCtlDy;
    rectMenu.dy = pme->m_pOwner->m_IconViewCtlDy;
    (void)MEMCPY(&pme->m_rectMenu, &rectMenu, sizeof(AEERect));

    DBGPRINTF_MED("MP: Setting PlayerMenu to x = %d, y = %d, dx = %d, dy = %d", rectMenu.x, rectMenu.y, rectMenu.dx, rectMenu.dy);
    IMENUCTL_SetRect(pme->m_pPlayerMenu, &rectMenu);
    (void)IMENUCTL_Redraw(pme->m_pPlayerMenu);

#ifdef FEATURE_MMOVERLAY
    /*
     * When overlay mode is enabled,
     * use IMMOverlay_Update to update display.
     */
    if (pme->m_pOwner->m_bOverlays)
    {
      uint32    nRetVal;

      // Restore Image parameters
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_DEST_REGION, (uint32)&pme->m_rectImage, 
                              OV_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER | OV_FLAG_DEST_REGION_ALIGN_VERT_CENTER);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_DEST_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
      }
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_SCALE_X, OV_FLAG_SCALE_KEEP_ASPECT | OV_FLAG_SCALE_NO_CROP | OV_FLAG_SCALE_TO_FIT, 256);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_SCALE_X, %d, %s", nRet, MP_ErrorStateToString(nRet));
      }      
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_SCALE_Y, OV_FLAG_SCALE_KEEP_ASPECT | OV_FLAG_SCALE_NO_CROP | OV_FLAG_SCALE_TO_FIT, 256);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_SCALE_Y, %d, %s", nRet, MP_ErrorStateToString(nRet));
      }

      // Update Alpha Blending layers
      CPlayerWin_SetLayer1Rect(pme);
      CPlayerWin_UpdateLayer1(pme);
      CPlayerWin_SetLayer2Rect(pme);
      CPlayerWin_UpdateLayer2(pme);

      // Restore Menu, Info, and Title displays
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayTitle, OV_PARM_ENABLE, TRUE, 0);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_ENABLE, %d, %s", nRet, MP_ErrorStateToString(nRet));
      }
      nRetVal = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayTitle, NULL, 0);
      if (nRetVal == MP_ERR_IMMOVERLAY)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
      }
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayInfo, OV_PARM_ENABLE, TRUE, 0);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_ENABLE, %d, %s", nRet, MP_ErrorStateToString(nRet));
      }
      nRetVal = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayInfo, NULL, 0);
      if (nRetVal == MP_ERR_IMMOVERLAY)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
      }
      nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayMenu, OV_PARM_ENABLE, TRUE, 0);
      if (nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: Error on IMMOVERLAY_SetParam OV_PARM_ENABLE, %d, %s", nRet, MP_ErrorStateToString(nRet));
      }
      nRetVal = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayMenu, NULL, 0);
      if (nRetVal == MP_ERR_IMMOVERLAY)
      {
        DBGPRINTF_ERROR("MP: IMMOVERLAY_Update Failed, nRet=%d", nRet);
      }
    }
#endif // FEATURE_MMOVERLAY

#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
    if (pme->m_dwZoomScale != MM_MPEG4_NO_SCALING)
    {
      (void)IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_SCALING, pme->m_dwZoomScale, 0);
    }
    else
#endif // defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
    {
#ifdef FEATURE_QTV_MDP_ASCALE //
      //Do AScaling if the media file is not an image.
      if (!pme->m_pOwner->m_bIsImage)
      {
        (void)CPlayerWin_SetAScale( pme );
      }
#endif  // FEATURE_QTV_MDP_ASCALE
    }
  }

  if (CPlayerWin_IsMedia(pme) || CPlayerWin_IsImage(pme) || pme->m_bFullScreen == TRUE)
  {
	  // Set Clipping Rect
	  if (pme->m_bFullScreen)
	  {
  		DBGPRINTF_MED("MP: Setting Clipping Rec to x = %d, y = %d, dx = %d, dy = %d",
  		  pme->m_rectImage.x, pme->m_rectImage.y, pme->m_rectImage.dx, pme->m_rectImage.dy);
  		IDISPLAY_SetClipRect(pme->m_pIDisplay, &pme->m_rectImage);
	  }
	  else
	  {
  		AEERect tmp;
  		tmp.x = 0;
  		tmp.y = 0;
  		tmp.dx = pme->m_pOwner->m_cxWidth;
  		tmp.dy = pme->m_pOwner->m_cyHeight;
  		DBGPRINTF_MED("MP: Setting Clipping Rec to x = %d, y = %d, dx = %d, dy = %d", tmp.x, tmp.y, tmp.dx, tmp.dy);
  		IDISPLAY_SetClipRect(pme->m_pIDisplay, &tmp);
	  }     
	  if (CPlayerWin_IsMedia(pme))
	  {
  		DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with x = %d, y = %d, cx = %d, cy = %d", pme->m_rectImage.x, pme->m_rectImage.y, pme->m_rectImage.dx, pme->m_rectImage.dy);
  		(void)IMEDIA_SetRect(pme->m_pMedia, &pme->m_rectImage, NULL);
	  }
	  else if (CPlayerWin_IsImage(pme))
	  {      
  		DBGPRINTF_MED("MP: Calling IIMAGECTL_SetRect with x = %d, y = %d, cx = %d, cy = %d", pme->m_rectImage.x, pme->m_rectImage.y, pme->m_rectImage.dx, pme->m_rectImage.dy);
  		IIMAGECTL_SetActive(pme->m_pImageCtl, TRUE);
  		IIMAGECTL_SetRect(pme->m_pImageCtl, &pme->m_rectImage);
  		(void)IIMAGECTL_Redraw(pme->m_pImageCtl);
	  }

    CPlayerWin_Redraw((IWindow *)pme);

    if (pme->m_bFullScreen == TRUE)
    {
      event_report(EVENT_QTV_CLIP_FULLSCREEN);
    }
  }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
  return TRUE;
}


#ifdef FEATURE_MULTISEQUENCER

/*===========================================================================

                     CSequencePlayerWin Functions

===========================================================================*/

/*===========================================================================
   This function constructs Sequence player window.
===========================================================================*/
static IWindow* CSequencePlayerWin_New(CMediaPlayer *pOwner)
{
  CSequencePlayerWin *pme;

  VTBL(IWindow)vtbl;
  int       y;
  int       dy;

  mp_ignoreNotices = FALSE;

  MP_IWINDOW_SETVTBL(&vtbl, CSequencePlayerWin_Enable,
                     CSequencePlayerWin_Redraw, CSequencePlayerWin_HandleEvent,
                     CSequencePlayerWin_Delete);
  pme =
    (CSequencePlayerWin *)CWindow_New(sizeof(CSequencePlayerWin), pOwner,
                                       &vtbl);
  if (!pme)
    return NULL;

  /*
   * Set title and image rects. Create the static control.
   */
  {
    int       cx = pme->m_pOwner->m_cxWidth;
    int       cy = pme->m_pOwner->m_cyHeight;
    int       nLChSize = pme->m_pOwner->m_nLChSize;
    int       cyProgMain = pme->m_pOwner->m_nNChSize + 1;
    AEERect   rectTitle;
    AEERect   rectMenu;
    AEERect   rectProg;
    uint16    nIndex;

    pme->m_pOwner->m_State = MP_STATE_SEQUENCER_WIN;
    EMIT_MEDIAPLAYER_STATE(pme->m_pOwner->m_State);
    pme->m_pOwner->m_playState = MP_PLAYSTATE_STOPPED;
    EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
   /*
     * File name text rect
     */
    y = 1;
    dy = nLChSize;
    SETAEERECT(&rectTitle, 0, y, cx, dy);


    /*
     * Base image rect - the space between the player menu at the bottom and
     * the title/header boxes on the top. 
     */
    y += rectTitle.dy;
    dy = cy - y - cyProgMain - pme->m_pOwner->m_IconViewCtlDy - 2;

    /*
     * Create IStatic for file name
     */
    for (nIndex = 0; nIndex < pOwner->m_MSFiles.m_wNumSequences; nIndex++)
    {
      if (ISHELL_CreateInstance
          (pme->m_pIShell, AEECLSID_STATIC,
           (void **)&pme->m_sSequenceFile[nIndex].m_pTitle))
      {
      	DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
        MP_WINERR_RETURN(pme)
      }
      ISTATIC_SetRect(pme->m_sSequenceFile[nIndex].m_pTitle, &rectTitle);
      ISTATIC_SetProperties(pme->m_sSequenceFile[nIndex].m_pTitle,
                            ST_CENTERTEXT | ST_NOSCROLL);
      SETAEERECT(&pme->m_sSequenceFile[nIndex].m_rectImage, 0, y, cx, dy);
      (void)MEMCPY(&pme->m_sSequenceFile[nIndex].m_rectIdleImage,
             &pme->m_sSequenceFile[nIndex].m_rectImage, sizeof(AEERect));
      (void)MEMCPY(&pme->m_sSequenceFile[nIndex].m_dfltRectImage,
             &pme->m_sSequenceFile[nIndex].m_rectImage, sizeof(AEERect));
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      pme->m_sSequenceFile[nIndex].m_dwDownldPos = 0;
#endif
    }

    /*
     * IconViewCtl rect
     */
    y = cy - pme->m_pOwner->m_IconViewCtlDy;
    dy = pme->m_pOwner->m_IconViewCtlDy;
    SETAEERECT(&rectMenu, 0, y, cx, dy);
    if (ISHELL_CreateInstance
        (pme->m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pme->m_pPlayerMenu))
    {
    	DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
      MP_WINERR_RETURN(pme)
    }

    DBGPRINTF_MED("MP: Setting PlayerMenu to x = %d, y = %d, dx = %d, dy = %d", rectMenu.x, rectMenu.y, rectMenu.dx, rectMenu.dy);
    MP_SetMenuAttr(pme->m_pPlayerMenu, AEECLSID_ICONVIEWCTL,
                   pme->m_pOwner->m_nColorDepth, &rectMenu, MP_MAXSOFTKEYITEMS);

    /*
     * ProgMain rect
     */
    y = cy - cyProgMain - pme->m_pOwner->m_IconViewCtlDy - 2;
    dy = cyProgMain;
    SETAEERECT(&rectProg, 0, y, cx, dy);


    /*
     * Initialize logo below the header
     */
    pme->m_pIdleImage =
      ISHELL_LoadResImage(pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDB_LOGO);
    if (!pme->m_pIdleImage)
    {
    	DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
      MP_WINERR_RETURN(pme)
    }

    for (nIndex = 0; nIndex < pOwner->m_MSFiles.m_wNumSequences; nIndex++)
    {
      boolean bRet;
      /*
       * Initialize ProgCtl
       */
      pme->m_sSequenceFile[nIndex].m_ProgCtl.m_pIShell = pme->m_pIShell;
      pme->m_sSequenceFile[nIndex].m_ProgCtl.m_pIDisplay = pme->m_pIDisplay;

      bRet = CProgCtl_Init(&pme->m_sSequenceFile[nIndex].m_ProgCtl, &rectProg);
      if (!bRet)
      {
      	DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
        MP_WINERR_RETURN(pme)
      }

      /*
       * Initialize default flags
       */
      DBGPRINTF_MED("MP: Setting FullScreen to %d", FALSE);
      pme->m_sSequenceFile[nIndex].m_bImage = FALSE;
      pme->m_sSequenceFile[nIndex].m_bPlay = FALSE;
      pme->m_sSequenceFile[nIndex].m_bFullScreen = FALSE;
      pme->m_sSequenceFile[nIndex].m_bProgTimer = FALSE;
    }

    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_PLAY, IDM_PM_PLAY, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_STOP, IDM_PM_STOP, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_REWIND, IDM_PM_REWIND, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_PAUSE, IDM_PM_PAUSE, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FF, IDM_PM_FF, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_UP, IDM_PM_UP, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_DOWN, IDM_PM_DOWN, 0);

#ifdef FEATURE_MP_EQ_UI
    pme->m_pOwner->m_bShowPlayerWinSettingUI = TRUE;
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_SETTINGS,
                               IDM_PM_PLAYERWIN_SETTINGS, 0);
    CMediaPlayer_AudioSetupScreen(pme->m_pOwner);
    CSequencePlayerWin_Redraw((IWindow *)pme);
#endif //FEATURE_MP_EQ_UI

    /*
     * Set default flags
     */
    pme->m_bControllAll = TRUE;
    pme->m_wCurFile = 0;
    pme->m_wCurMedia = 0;
  }
  return (IWindow *)pme;
}

/*===========================================================================
   This function deletes the Sequence player window.
===========================================================================*/
static void
CSequencePlayerWin_Delete(IWindow *po)
{
  uint16    nIndex;
  CSequencePlayerWin *pme; 
  CMediaPlayer *pMP;

  pme = (CSequencePlayerWin *)po;
  if (!pme) 
  {
      return;
  }

  pMP = pme->m_pOwner;
  if (!pMP) 
  {
      return;
  }

  pMP->m_playState = MP_PLAYSTATE_EXITING;
  EMIT_MEDIAPLAYER_PLAYSTATE(pMP->m_playState);

#ifdef FEATURE_MP_EQ_UI
  // this function only release resources,
  // we need to keep pme->m_pOwner->m_pSetting ptr
  // to save information into config file when exit from MP
  MPSettingRelease(pme->m_pOwner->m_pSetting);

#endif //FEATURE_MP_EQ_UI

  for (nIndex = 0; nIndex < pMP->m_MSFiles.m_wNumSequences; nIndex++)
  {
    MP_RELEASEIF(pme->m_sSequenceFile[nIndex].m_pTitle);
    /* Delete (release) the IMEDIA object. */
    DBGPRINTF_MED("MP: nIndex=%d releasing IMEDIA object", nIndex);
    MP_RELEASEIF(pme->m_sSequenceFile[nIndex].m_pMedia);
    CProgCtl_Release(&pme->m_sSequenceFile[nIndex].m_ProgCtl);
    if (pme->m_sSequenceFile[nIndex].m_bProgTimer)
    {
      pme->m_wCurMedia = nIndex;
      (void)ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)CSequencePlayerWin_ProgTimerNotify, pme);
    }
  }
  MP_RELEASEIF(pme->m_pIdleImage);
  MP_RELEASEIF(pme->m_pPlayerMenu);

  if (pMP)
  {
    pMP->m_pWin = NULL;
    pMP->m_eActiveWin = MPW_NONE;
    pMP->m_State = MP_STATE_NULL;
    EMIT_MEDIAPLAYER_STATE(pMP->m_State);
  }
  FREE(pme);
}

/*===========================================================================
   This function enables/disables the Sequence player window.
===========================================================================*/
static void
CSequencePlayerWin_Enable(IWindow *po, boolean bEnable)
{
  CSequencePlayerWin *pme = (CSequencePlayerWin *)po;
  boolean   bActive;

  if (!CWindow_ProcessEnable(po, bEnable))
    return;

  bActive = pme->m_bActive;

  ISTATIC_SetActive(pme->m_sSequenceFile[pme->m_wCurFile].m_pTitle, bActive);
  IMENUCTL_SetActive(pme->m_pPlayerMenu, bActive);

  if (!pme->m_bActive)
  {
    IIMAGE_Stop(pme->m_pIdleImage);

    /*
     * Stop the entire sequence
     */
    CSequencePlayerWin_Stop(pme, TRUE);
  }
  else
  {
    IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_PLAY); 
  }
}

/*===========================================================================
   This function redraws Sequence player window.
===========================================================================*/
static void
CSequencePlayerWin_Redraw(IWindow *po)
{
  CSequencePlayerWin *pme = (CSequencePlayerWin *)po;
  AEERect   rect;
  CSequenceFile *psf = &(pme->m_sSequenceFile[pme->m_wCurFile]);

  if (!pme->m_bActive)
    return;

  if (psf->m_bImage && psf->m_bFullScreen)
  {
    if (!psf->m_bPlay)
    {
      IDISPLAY_ClearScreen(pme->m_pIDisplay);
      IDISPLAY_Update(pme->m_pIDisplay);
    }
    else
    {
      IDISPLAY_ClearScreen(pme->m_pIDisplay);
    }
    return;
  }

  IDISPLAY_ClearScreen(pme->m_pIDisplay);

  /*
   * (title)text
   */
  (void)STRTOWSTR(psf->m_pszFile, psf->m_szText, sizeof(psf->m_szText));
  MP_FitStaticText(pme->m_pIDisplay, psf->m_pTitle, AEE_FONT_NORMAL,
                   psf->m_szText);
  (void)ISTATIC_Redraw(psf->m_pTitle);

  /*
   * Draw the line below text.
   */
  ISTATIC_GetRect(psf->m_pTitle, &rect);
  rect.y += rect.dy - 1;
  rect.dy = 1;
  MP_FrameRect(pme->m_pIDisplay, &rect);

  /*
   * SoftKey
   */
  (void)IMENUCTL_Redraw(pme->m_pPlayerMenu);

  /*
   * Image
   */
  if (!psf->m_bPlay)
  {
    MP_DrawImage(pme->m_pIdleImage, &psf->m_rectIdleImage, TRUE);
  }

#ifdef FEATURE_MP_EQ_UI
  (void)MPUpdateAnalyzerGraph(pme->m_pOwner->m_pSetting, pme->m_pOwner->m_pIGraphics);
#endif //FEATURE_MP_EQ_UI

  /*
   * Progress bar...
   */
  CSequencePlayerWin_UpdateProgCtl(pme, 0, 0, 0, pme->m_wCurFile);
}


/*===========================================================================
   This function handles the events routed to the Sequence player window.
   If the event is
   (1)CLR or OK from error dialog:
      Sequence file list window is opened
   (2)EVT_COMMAND: processes the menu commands.
   (3)EVT_KEY: routed appropriately to the controls.
===========================================================================*/
static boolean
CSequencePlayerWin_HandleEvent(IWindow *po, AEEEvent eCode, uint16 wParam,
                               uint32 dwParam)
{
  CSequencePlayerWin *pme = (CSequencePlayerWin *)po;
  boolean   bRet = TRUE;

  if (pme == NULL)
  {
    DBGPRINTF_ERROR("MP: pme is NULL.");
    return TRUE;
  }

#ifdef FEATURE_MP_EQ_UI
  if (pme->m_pOwner == NULL)
  {
      DBGPRINTF_ERROR("MP: pme->m_pOwner is NULL.");
      return TRUE;
  }

  // return TURE if event is handled by setup win
  if(MPSetupHandleEvent(pme->m_pOwner->m_pSetting, eCode, wParam, dwParam))
  {
    CSequencePlayerWin_Redraw(po);
    return TRUE;
  }
#endif //FEATURE_MP_EQ_UI

#ifdef FEATURE_KEY_HOLD_SCROLL
  if (MP_ISONEOFKEYEVENT(eCode) && (wParam == AVK_SELECT || wParam == AVK_CLR))
  {
    /*
     * if timer is enabled for auto-scrolling, do nothing here
     */
    if (pme->m_pOwner->m_KeyStore.m_wParam_Key)
      return TRUE;
  }
#endif

  /*
   * Process CLR or OK from error dlg
   */
  if (MP_ISCLR(eCode) || MP_ISCMD(eCode, IDS_OK))
  {
#ifdef FEATURE_MP_EQ_UI
    // if setup screen has focus, turn off and return, otherwise
    // close PlayerWin
    if(MPSetupFocus(pme->m_pOwner->m_pSetting, FALSE))
    {
      CSequencePlayerWin_Redraw(po);
      return TRUE;
    }

#endif //FEATURE_MP_EQ_UI

    CSequencePlayerWin_Stop(pme, TRUE);
    (void)CMediaPlayer_SetWindow(pme->m_pOwner, MPW_MULTISEQUENCER, 0);
    return TRUE;
  }

  /*
   * Process EVT_KEY...
   */
  // Match to settings window and CPlayerWin
  if(eCode == EVT_KEY_PRESS)
  {
    switch (wParam)
    {
    case AVK_UP:
    {
      uint16  nNextLevel = pme->m_pOwner->m_wVolumeLevel + 1;
      uint16  wVol = (pme->m_pOwner->m_wNumVolumeLevels <= nNextLevel)? AEE_MAX_VOLUME : (nNextLevel * pme->m_pOwner->m_VolumeStep) + 2;

      wVol = (wVol > AEE_MAX_VOLUME) ? AEE_MAX_VOLUME : wVol;
      CSequencePlayerWin_SetVolume(pme, wVol);
    }
      break;
    case AVK_DOWN:
    {
      uint16 nNextLevel = 0;
      int16  nVol;

      if(pme->m_pOwner->m_wVolumeLevel > 0)
      {
        nNextLevel = pme->m_pOwner->m_wVolumeLevel - 1;
      }
      nVol = (nNextLevel * pme->m_pOwner->m_VolumeStep) + 2;

      nVol = (nVol < 0) ? 0 : nVol;
      CSequencePlayerWin_SetVolume(pme, (uint16)nVol);
    }
      break;

    case AVK_0:
    {
        // Check if Fullscreen is available
        CtlAddItem pai;
        if ( IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_FULLSCRN, &pai) )
        {
            (void)CSequencePlayerWin_FullScreen(pme,
                (boolean) ! (pme->m_sSequenceFile[pme->m_wCurFile].m_bFullScreen), FALSE);
            IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_FULLSCRN);
        }
    }
      break;

    case AVK_1:
      pme->m_wCurFile = 0;
      pme->m_bControllAll = FALSE;
      CMediaPlayer_Redraw(pme->m_pOwner, TRUE);
      break;

    case AVK_2:
      if (pme->m_pOwner->m_MSFiles.m_wNumSequences > 1)
      {
        pme->m_wCurFile = 1;
        pme->m_bControllAll = FALSE;
        CMediaPlayer_Redraw(pme->m_pOwner, TRUE);
      }
      break;

    case AVK_3:
      if (pme->m_pOwner->m_MSFiles.m_wNumSequences > 2)
      {
        pme->m_wCurFile = 2;
        pme->m_bControllAll = FALSE;
        CMediaPlayer_Redraw(pme->m_pOwner, TRUE);
      }
      break;

    case AVK_4:
      if (pme->m_pOwner->m_MSFiles.m_wNumSequences > 3)
      {
        pme->m_wCurFile = 3;
        pme->m_bControllAll = FALSE;
        CMediaPlayer_Redraw(pme->m_pOwner, TRUE);
      }
      break;

    case AVK_STAR:
      pme->m_bControllAll = TRUE;
      break;
    }
    // Menu needs the EVT_KEY code to process, but we are doing KEY_PRESS
    return IMENUCTL_HandleEvent(pme->m_pPlayerMenu, EVT_KEY, wParam, dwParam);
  }

  /*
   * If this event is not EVT_COMMAND, then return...
   */
  if (!MP_ISEVTCMD(eCode))
    return FALSE;

  /*
   * Process EVT_COMMAND...
   */
  switch (wParam)
  {
  case IDM_PM_PLAY:
    CSequencePlayerWin_Play(pme);
    break;

  case IDM_PM_STOP:
    CSequencePlayerWin_Stop(pme, pme->m_bControllAll);
    break;

#ifdef FEATURE_MP_EQ_UI
  case IDM_PM_PLAYERWIN_SETTINGS:
    {
      DBGPRINTF_MED("MP: IDM_PM_PLAYERWIN_SETTINGS event received.");
      if(pme && pme->m_pOwner && MPShowSetupWin(pme->m_pOwner->m_pSetting, TRUE))
      {
        CSequencePlayerWin_Redraw(po);
      }
    }
    break;
#endif //FEATURE_MP_EQ_UI

  case IDM_PM_REWIND:
    {
      uint16    nIndex;
      int nRet;

      if (pme->m_bControllAll)
      {
        for (nIndex = 0; nIndex < pme->m_pOwner->m_MSFiles.m_wNumSequences; nIndex++)
        {
          DBGPRINTF_HIGH ("MP: play sequence, nIndex, %d, pMedia, 0x%X, ", nIndex, pme->m_sSequenceFile[nIndex].m_pMedia);
          if(pme->m_sSequenceFile[nIndex].m_pMedia)
          {
            nRet = IMEDIA_Rewind(pme->m_sSequenceFile[nIndex].m_pMedia, pme->m_pOwner->m_nSeekTime);
            if(SUCCESS != nRet)
            {
              DBGPRINTF_ERROR("MP: IMEDIA_Rewind failed, idx = %d, nRet = %d[%s]", nIndex, nRet, MP_ErrorStateToString(nRet));
            }
          }
          else
          {
            DBGPRINTF_ERROR("MP: NULL ptr, idx = %d", nIndex);
          }
        }
      }
      else
      {
        nRet = IMEDIA_Rewind(pme->m_sSequenceFile[pme->m_wCurFile].m_pMedia, pme->m_pOwner->m_nSeekTime);
        if(SUCCESS != nRet)
        {
          DBGPRINTF_ERROR("MP: IMEDIA_Rewind failed, currentFile = %d, nRet = %d[%s]", pme->m_wCurFile, nRet, MP_ErrorStateToString(nRet));
        }
      }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    }
    break;

  case IDM_PM_FF:
    {
      uint16    nIndex;
      int nRet;

      if (pme->m_bControllAll)
      {
        for (nIndex = 0; nIndex < pme->m_pOwner->m_MSFiles.m_wNumSequences; nIndex++)
        {
          DBGPRINTF_HIGH ("MP: play sequence, nIndex, %d, pMedia, 0x%X, ", nIndex, pme->m_sSequenceFile[nIndex].m_pMedia);
          if(pme->m_sSequenceFile[nIndex].m_pMedia)
          {
            nRet = IMEDIA_FastForward(pme->m_sSequenceFile[nIndex].m_pMedia, pme->m_pOwner->m_nSeekTime);
            if(SUCCESS != nRet)
            {
              DBGPRINTF_ERROR("MP: IMEDIA_FastForward failed, idx = %d, nRet = %d[%s]", nIndex, nRet, MP_ErrorStateToString(nRet));
            }
          }
          else
          {
            DBGPRINTF_ERROR("MP: NULL ptr, idx = %d", nIndex);
          }
        }
      }
      else
      {
        nRet = IMEDIA_FastForward(pme->m_sSequenceFile[pme->m_wCurFile].m_pMedia, pme->m_pOwner->m_nSeekTime);
        if(SUCCESS != nRet)
        {
          DBGPRINTF_ERROR("MP: IMEDIA_FastForward failed, currentFile = %d, nRet = %d[%s]", pme->m_wCurFile, nRet, MP_ErrorStateToString(nRet));
        }
      }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    }
    break;

  case IDM_PM_PAUSE:
    {
      uint16    nIndex;
      boolean  bPlaying;
      int nRet;

      // look current file for playing status
      bPlaying = CSequencePlayerWin_IsPlaying(pme, pme->m_bControllAll);
        // pause/resume on all files
      if (pme->m_bControllAll)
      {
        for (nIndex = 0; nIndex < pme->m_pOwner->m_MSFiles.m_wNumSequences; nIndex++)
        {
          DBGPRINTF_HIGH ("MP: play sequence, nIndex, %d, pMedia, 0x%X, ", nIndex, pme->m_sSequenceFile[nIndex].m_pMedia);
          if(pme->m_sSequenceFile[nIndex].m_pMedia)
          {
            if(bPlaying)
            {
              nRet = IMEDIA_Pause(pme->m_sSequenceFile[nIndex].m_pMedia);
              if(SUCCESS != nRet)
              {
                DBGPRINTF_ERROR("MP: IMEDIA_Pause failed, idx = %d, nRet = %d[%s]", nIndex, nRet, MP_ErrorStateToString(nRet));
              }
              else
              {
                DBGPRINTF_MED("MP: IMEDIA_Pause returns %d = %s", nRet, MP_ErrorStateToString(nRet));
              }
            }
            else
            {
              nRet = IMEDIA_Resume(pme->m_sSequenceFile[nIndex].m_pMedia);
              if(SUCCESS != nRet)
              {
                DBGPRINTF_ERROR("MP: IMEDIA_Resume failed, idx = %d, nRet = %d[%s]", nIndex, nRet, MP_ErrorStateToString(nRet));
              }
              else
              {
                DBGPRINTF_MED("MP: IMEDIA_Pause returns %d = %s", nRet, MP_ErrorStateToString(nRet));
                pme->m_pOwner->m_bPlayAfterResume = FALSE;
              }
            }
          }
          else
          {
            DBGPRINTF_ERROR("MP: NULL ptr, idx = %d", nIndex);
          }
        }
      }
      else
      {
        if(bPlaying)
        {
          nRet = IMEDIA_Pause(pme->m_sSequenceFile[pme->m_wCurFile].m_pMedia);
          if(SUCCESS != nRet)
          {
            DBGPRINTF_ERROR("MP: IMEDIA_Pause failed, currentFile = %d, nRet = %d[%s]", pme->m_wCurFile, nRet, MP_ErrorStateToString(nRet));
          }
          else
          {
            DBGPRINTF_MED("MP: IMEDIA_Pause returns %d = %s", nRet, MP_ErrorStateToString(nRet));
          }
        }
        else
        {
          nRet = IMEDIA_Resume(pme->m_sSequenceFile[pme->m_wCurFile].m_pMedia);
          if(SUCCESS != nRet)
          {
            DBGPRINTF_ERROR("MP: IMEDIA_Resume failed, currentFile = %d, nRet = %d[%s]", pme->m_wCurFile, nRet, MP_ErrorStateToString(nRet));
          }
          else
          {
            DBGPRINTF_MED("MP: IMEDIA_Pause returns %d = %s", nRet, MP_ErrorStateToString(nRet));
            pme->m_pOwner->m_bPlayAfterResume = FALSE;
          }
        }
      }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    }
    break;

  case IDM_PM_UP:
  {
    uint16  nNextLevel = pme->m_pOwner->m_wVolumeLevel + 1;
    uint16  wVol = (pme->m_pOwner->m_wNumVolumeLevels <= nNextLevel)? AEE_MAX_VOLUME : (nNextLevel * pme->m_pOwner->m_VolumeStep) + 2;

    wVol = (wVol > AEE_MAX_VOLUME) ? AEE_MAX_VOLUME : wVol;
    CSequencePlayerWin_SetVolume(pme, wVol);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    break;
  }

  case IDM_PM_DOWN:
  {
    uint16 nNextLevel = 0;
    int16  nVol;

    if(pme->m_pOwner->m_wVolumeLevel > 0)
    {
      nNextLevel = pme->m_pOwner->m_wVolumeLevel - 1;
    }
    nVol = (nNextLevel * pme->m_pOwner->m_VolumeStep) + 2;

    nVol = (nVol < 0) ? 0 : nVol;
    CSequencePlayerWin_SetVolume(pme, (uint16)nVol);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
    break;
  }

  default:
    bRet = FALSE;
    break;
  }

  return bRet;
}


/*===========================================================================
   This function plays a sequence
===========================================================================*/
static void
CSequencePlayerWin_Play(CSequencePlayerWin *pme)
{
  uint16    nIndex;
  int       ret;
  for (nIndex = 0; nIndex < pme->m_pOwner->m_MSFiles.m_wNumSequences; nIndex++)
  {
    DBGPRINTF_HIGH ("MP: play sequence, nIndex, %d, pMedia, 0x%X, ", nIndex, pme->m_sSequenceFile[nIndex].m_pMedia);
    if(pme->m_sSequenceFile[nIndex].m_pMedia)
    {
      (void)IMEDIA_EnableChannelShare(pme->m_sSequenceFile[nIndex].m_pMedia, TRUE);
      ret = IMEDIA_Play(pme->m_sSequenceFile[nIndex].m_pMedia);
      DBGPRINTF_MED("MP: IMEDIA_Play returns %d = %s", ret, MP_ErrorStateToString(ret));

      pme->m_sSequenceFile[nIndex].m_bPlay = TRUE;
    }
    else
    {
      DBGPRINTF_ERROR("MP: NULL ptr, idx = %d", nIndex);
    }
  }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
}

/*===========================================================================
   This function indicates if Sequence is in playing state.
===========================================================================*/
static boolean
CSequencePlayerWin_IsPlaying(CSequencePlayerWin *pme, boolean bAll)
{
  boolean   bChg = 0;
  int       nState = 0;

  /*
   * if need to check entire sequence or just current file
   */
  if (bAll)
  {
    uint16    nIndex;

    for (nIndex = 0; nIndex < pme->m_pOwner->m_MSFiles.m_wNumSequences; nIndex++)
    {
      if (pme->m_sSequenceFile[nIndex].m_pMedia)
      {
        nState = IMEDIA_GetState(pme->m_sSequenceFile[nIndex].m_pMedia, &bChg);
        if (nState == MM_STATE_PLAY)
          return TRUE;
      }
      else
      {
        DBGPRINTF_ERROR("MP: NULL ptr, idx = %d", nIndex);
      }
    }
    return FALSE;
  }
  else
  {
    if (pme->m_sSequenceFile[pme->m_wCurFile].m_pMedia)
      nState =
        IMEDIA_GetState(pme->m_sSequenceFile[pme->m_wCurFile].m_pMedia, &bChg);

    return (nState == MM_STATE_PLAY);
  }
}

#ifdef FEATURE_BT_EXTPF_AV
/*===========================================================================
   This function checks to see if the sequencer playback is still playing and,
   if so, requests that it go to the PAUSE state.
===========================================================================*/
static boolean
CSequencePlayerWin_PausePlay(CSequencePlayerWin *pWin)
{
  boolean bPause = FALSE;

  if (pWin)
  {
    if (pWin->m_bControllAll)
    {
      (void)IMEDIA_EnableChannelShare(pWin->m_sSequenceFile[0].m_pMedia, FALSE);
      if (CSequencePlayerWin_IsPlaying(pWin, TRUE))
      {
        (void)ISHELL_PostEvent(pWin->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                             EVT_COMMAND, IDM_PM_PAUSE, 0);
        bPause = TRUE;
      }
    }
    else
    {
      if (CSequencePlayerWin_IsPlaying(pWin, FALSE))
      {
       (void)ISHELL_PostEvent(pWin->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                             EVT_COMMAND, IDM_PM_PAUSE, 0);
        bPause = TRUE;
      }
    }
  }

  if (bPause)
  {
    DBGPRINTF_MED("MP: CSequencePlayerWin - Pause Request");
  }
  else
  {
    DBGPRINTF_MED("MP: CSequencePlayerWin - No Pause Request");
  }
  return bPause;
}
#endif // FEATURE_BT_EXTPF_AV

/*===========================================================================
   This function stops the entire sequence.
===========================================================================*/
static void
CSequencePlayerWin_Stop(CSequencePlayerWin *pme, boolean bAll)
{
  uint16    nIndex;
  int       nRet;
  for (nIndex = 0; nIndex < pme->m_pOwner->m_MSFiles.m_wNumSequences; nIndex++)
  {
    if(pme->m_sSequenceFile[nIndex].m_pMedia)
    {
      nRet = IMEDIA_Stop(pme->m_sSequenceFile[nIndex].m_pMedia);
      if(nRet != SUCCESS)
      {
        DBGPRINTF_ERROR("MP: SequencePlayerWin failed to stop, i = %d, nRet = %d[%s].",
                        nIndex, nRet, MP_ErrorStateToString(nRet));
      }
    }
    else
    {
      DBGPRINTF_ERROR("MP: NULL ptr, idx = %d", nIndex);
    }
  }
  if (bAll)
  {
    if(pme->m_sSequenceFile[0].m_pMedia)
    {
      (void)IMEDIA_EnableChannelShare(pme->m_sSequenceFile[0].m_pMedia, FALSE);
    }
    else
    {
      DBGPRINTF_ERROR("MP: NULL ptr, idx = %d", 0);
    }
  }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
}

/*===========================================================================
   This function sets the volume of the entire sequence.
===========================================================================*/
static void
CSequencePlayerWin_SetVolume(CSequencePlayerWin *pme, uint16 wVol)
{
  uint16    nIndex;

  for (nIndex = 0; nIndex < pme->m_pOwner->m_MSFiles.m_wNumSequences; nIndex++)
  {
    if(pme->m_sSequenceFile[nIndex].m_pMedia)
    {
      if (MM_PENDING == IMEDIA_SetVolume(pme->m_sSequenceFile[nIndex].m_pMedia, wVol))
      {
        AEEMediaCmdNotify mcn;
        AEECLSID  cls;

        (void)IMEDIA_GetClassID(pme->m_sSequenceFile[nIndex].m_pMedia, &cls);
        mcn.clsMedia = cls;
        mcn.pIMedia = pme->m_sSequenceFile[nIndex].m_pMedia;
        mcn.nCmd = MM_CMD_SETMEDIAPARM;
        mcn.nSubCmd = MM_PARM_VOLUME;
        mcn.nStatus = MM_STATUS_DONE;
        mcn.pCmdData = NULL;
        mcn.dwSize = 0;

        MP_SequencePlayerWin_MediaNotify(pme->m_pOwner, &mcn);
      }
      else
      {
        DBGPRINTF_ERROR("MP: NULL ptr, idx = %d", nIndex);
      }
    }
  }
}

/*===========================================================================
   This is the main callback notification function for the IMedia object
   within the Sequence player window.
   (1)Handles Play, Record, GetTotalTime, GetMediaParm and
       SetMediaParm commands and corresponding status for those commands.
   (2)Updates the ProgCtl, if required.
===========================================================================*/
void MP_SequencePlayerWin_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
{
  CMediaPlayer* pMP = (CMediaPlayer*)pUser;
  CSequencePlayerWin *pme = (CSequencePlayerWin *)pMP->m_pWin;
  uint16    nTextID = 0;
  uint16    i = 0;
  uint16    nIndex = 0;
  uint16    iMax = 0;
  boolean   CallUpdateProgCtl = FALSE;

  if(pme == NULL)
  {
    DBGPRINTF_ERROR("MP: MediaNotify, pme is NULL");
    return;
  }

  if(pMP->m_eActiveWin != MPW_SEQUENCE_PLAYER)
  {
    DBGPRINTF_ERROR("MP: MediaNotify, active win not MPW_SEQUENCE_PLAYER (%d).", pme->m_pOwner->m_eActiveWin);
    return;
  }
 
  /*
   * do not process any event when exiting
   */
  EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
  if (pme->m_pOwner->m_playState == MP_PLAYSTATE_EXITING)
    return;

  iMax = pme->m_pOwner->m_MSFiles.m_wNumSequences;
  if (iMax > MAX_NUM_SEQUENCES)
  {
    iMax = MAX_NUM_SEQUENCES;
  }
  for (i = 0; i < iMax; i++)
  {
    if (pme->m_sSequenceFile[i].m_pMedia == pCmdNotify->pIMedia)
    {
      nIndex = i;
      break;
    }
  }

  switch (pCmdNotify->nStatus)
  {
  case MM_STATUS_SEEK_FAIL:
  case MM_STATUS_PAUSE_FAIL:
  case MM_STATUS_RESUME_FAIL:
    nTextID = IDS_ERR_PLAYCTL_CMD;
    break;
  }

  if (pCmdNotify->nCmd == MM_CMD_PLAY) // IMEDIA_Play
  {
    uint32    dwPlayPos = pme->m_sSequenceFile[nIndex].m_dwPlayPos;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    uint32    dwDownldPos = pme->m_sSequenceFile[nIndex].m_dwDownldPos;
#endif

    nTextID = IDS_PLAY;
    switch (pCmdNotify->nStatus)
    {
    case MM_STATUS_START:
      DBGPRINTF_MED("MP: Setting the seclected Menu Item to STOP");
      IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_STOP);
      pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      break;

#if defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) && defined(MM_STATUS_DOWNLOAD_TICK_UPDATE)
    case MM_STATUS_DOWNLOAD_TICK_UPDATE:
      dwDownldPos = (uint32)pCmdNotify->pCmdData;
      break;
#endif

    case MM_STATUS_TICK_UPDATE:       // Typcally, one-second update
      /*
       * If pCmdData is nonzero, then use the value in pCmdData as the time
       * to update the progress bar.
       */
      if ((uint32)pCmdNotify->pCmdData == 0)
      {
        dwPlayPos++;
      }
      else
      {
        dwPlayPos = MPClipTimeInSeconds((uint32)pCmdNotify->pCmdData);
      }

      // If no total time, increase by 20!
      if ((pme->m_sSequenceFile[nIndex].m_dwDuration / 1000) == 0)
        pme->m_sSequenceFile[nIndex].m_wPct = (pme->m_sSequenceFile[nIndex].m_wPct + 20) % 100;
      break;

    case MM_STATUS_SEEK:
      nTextID = IDS_SEEK;
      if (pCmdNotify->dwSize)
      {
        dwPlayPos = MPClipTimeInSeconds((uint32)pCmdNotify->pCmdData);
      }
      break;

    case MM_STATUS_PAUSE:
      nTextID = IDS_PAUSE;
      pme->m_pOwner->m_playState = MP_PLAYSTATE_PAUSED;
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      if (pCmdNotify->dwSize)
      {
        dwPlayPos = MPClipTimeInSeconds((uint32)pCmdNotify->pCmdData);
      }
      break;

    case MM_STATUS_RESUME:
      if (pme->m_bControllAll)
      {
        if (CSequencePlayerWin_IsPlaying(pme, TRUE))
          pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
      }
      else
      {
        if (CSequencePlayerWin_IsPlaying(pme, FALSE))
          pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
      }
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      if (pCmdNotify->dwSize)
      {
        dwPlayPos = MPClipTimeInSeconds((uint32)pCmdNotify->pCmdData);
      }
      break;

    case MM_STATUS_ABORT:             // playback aborted
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      dwDownldPos = 0;
#endif
    case MM_STATUS_DONE:              // playback done
    {
      if (pCmdNotify->nStatus == MM_STATUS_DONE)
      {
        nTextID = IDS_STOP;
        pme->m_pOwner->m_playState = MP_PLAYSTATE_STOPPED;
      }
      else
      {
        nTextID = IDS_ABORT;
        pme->m_pOwner->m_playState = MP_PLAYSTATE_ERROR;
        pme->m_pOwner->m_ErrorState = MP_ERRORSTATE_ERROR;
      }
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      pme->m_sSequenceFile[nIndex].m_bPlay = FALSE;
      dwPlayPos = 0;
      pme->m_sSequenceFile[nIndex].m_wPct = 0;

      IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_PLAY);
      CMediaPlayer_Redraw(pme->m_pOwner, TRUE);
      break;
    }

    }                                  // end of switch(nStatus)

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    if (dwDownldPos != pme->m_sSequenceFile[nIndex].m_dwDownldPos)
    {
      pme->m_sSequenceFile[nIndex].m_dwDownldPos = dwDownldPos;
      CallUpdateProgCtl = TRUE;
    }
#endif
    if (dwPlayPos != pme->m_sSequenceFile[nIndex].m_dwPlayPos)
    {
      pme->m_sSequenceFile[nIndex].m_dwPlayPos = dwPlayPos;
      CallUpdateProgCtl = TRUE;
    }
    if (CallUpdateProgCtl)
    {
      CSequencePlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd,
                                       pCmdNotify->nSubCmd, nTextID, nIndex);
    }

  }                                    // MM_CMD_PLAY
  else if (pCmdNotify->nCmd == MM_CMD_GETTOTALTIME)
  {
    if (pCmdNotify->nStatus == MM_STATUS_DONE)
    {
     pme->m_sSequenceFile[nIndex].m_dwDuration =
        ((uint32)pCmdNotify->pCmdData);
    }
  }                                    // MM_CMD_GETTOTALTIME
  else if (pCmdNotify->nCmd == MM_CMD_SETMEDIAPARM)
  {
    if (pCmdNotify->nSubCmd == MM_PARM_VOLUME &&
        pCmdNotify->nStatus == MM_STATUS_DONE)
    {
      int nRet;
      nRet = IMEDIA_GetMediaParm(pCmdNotify->pIMedia, MM_PARM_VOLUME, NULL, NULL);
      DBGPRINTF_MED("MP: Retrieve VOLUME, nRet = %d[%s]", nRet, MP_ErrorStateToString(nRet));
      return;
    }
  }                                    // MM_CMD_SETMEDIAPARM
  else if (pCmdNotify->nCmd == MM_CMD_GETMEDIAPARM)     // IMEDIA_GetMediaParm()
                                                        // events
  {
    if (pCmdNotify->nSubCmd == MM_PARM_VOLUME &&
        pCmdNotify->nStatus == MM_STATUS_DONE)
    {
      pme->m_wVolume = (uint16) (uint32)pCmdNotify->pCmdData;
      DBGPRINTF_MED("MP: Volume set, vol = %d", pme->m_wVolume);
      /*
       * save volume level
       */
      ((CMediaPlayer *)pme->m_pOwner)->m_wVolumeLevel = pme->m_wVolume / pme->m_pOwner->m_VolumeStep;
      pme->m_wCurMedia = nIndex;
      // save current timer status
      pme->m_sSequenceFile[nIndex].m_bProgTimer = FALSE;
      CSequencePlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd,
                                       pCmdNotify->nSubCmd, IDS_VOLUME, nIndex);
      /*
       * Set the prog timer so that the display stays for few seconds.
       */
      pme->m_sSequenceFile[nIndex].m_bProgTimer = TRUE;
      (void)ISHELL_SetTimer(pme->m_pIShell, MP_PROG_TIMER,
                            (PFNNOTIFY)CSequencePlayerWin_ProgTimerNotify, pme);
    }
  }                                    // MM_CMD_GETMEDIAPARM
}

/*===========================================================================
   This function updates the ProgCtl if progress timer is not running.
===========================================================================*/
static void
CSequencePlayerWin_UpdateProgCtl(CSequencePlayerWin *pme, int nCmd,
                                 int nSubCmd, uint16 wResID, uint16 nIndex)
{
  char      sz[32];
  AECHAR    szBuf[32];
  CSequenceFile *psf = &(pme->m_sSequenceFile[nIndex]);

  /*
   * We only update the current selection
   */
  if (nIndex != pme->m_wCurFile)
    return;

  /*
   * No progress timer in full screen mode
   */
  if (psf->m_bImage && psf->m_bFullScreen)
    return;

  if (psf->m_bProgTimer)
    return;

  if (!ISHELL_LoadResString
      (pme->m_pIShell, MEDIAPLAYER_RES_FILE, wResID, psf->m_szText,
       sizeof(psf->m_szText)))
    psf->m_szText[0] = (AECHAR) 0;

  if (nCmd == MM_CMD_PLAY)
  {
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    if (psf->m_dwDownldPos > 0)
    {
      uint32 dwDuration = MPClipTimeInSeconds(psf->m_dwDuration);
      uint16 wPctDownld = (uint16)((100 * psf->m_dwDownldPos) / psf->m_dwDuration);

      (void)SNPRINTF(sz, sizeof(sz)," %02d:%02d/%02d:%02d DL %d%%", psf->m_dwPlayPos / 60,
              psf->m_dwPlayPos % 60, dwDuration / 60,
              dwDuration % 60, wPctDownld);
    }
    else
#endif
    {
      uint32 dwDuration = MPClipTimeInSeconds(psf->m_dwDuration);

      (void)SNPRINTF(sz, sizeof(sz)," %02d:%02d/%02d:%02d", psf->m_dwPlayPos / 60,
              psf->m_dwPlayPos % 60, dwDuration / 60,
              dwDuration % 60);
    }
    (void)STRTOWSTR(sz, szBuf, sizeof(szBuf));
    (void)WSTRLCAT(psf->m_szText, szBuf, sizeof(psf->m_szText));

    if ((psf->m_dwDuration / 1000) > 0)
      psf->m_wPct = (uint16) ((100 * psf->m_dwPlayPos) / (psf->m_dwDuration / 1000));

  }
  else if (nCmd == MM_CMD_GETTOTALTIME)
  {
    uint32 dwDuration = MPClipTimeInSeconds(psf->m_dwDuration);

    (void)SNPRINTF(sz, sizeof(sz)," %02d:%02d/%02d:%02d", psf->m_dwPlayPos / 60,
            psf->m_dwPlayPos % 60, dwDuration / 60,
            dwDuration % 60);
    (void)STRTOWSTR(sz, szBuf, sizeof(szBuf));
    (void)WSTRLCAT(psf->m_szText, szBuf, sizeof(psf->m_szText));
  }
  else if (nCmd == MM_CMD_GETMEDIAPARM && nSubCmd == MM_PARM_VOLUME)
  {
    (void)SNPRINTF(sz, sizeof(sz),"[%u]", pme->m_wVolume);
    (void)STRTOWSTR(sz, szBuf, sizeof(szBuf));
    (void)WSTRLCAT(psf->m_szText, szBuf, sizeof(psf->m_szText));
    psf->m_wPct = (100 * pme->m_wVolume) / AEE_MAX_VOLUME;
  }
  else
  {
    psf->m_wPct = 0;
  }

  CProgCtl_SetPos(&(psf->m_ProgCtl), psf->m_szText, psf->m_wPct);

  IDISPLAY_Update(pme->m_pIDisplay);
}

/*===========================================================================
   This clears the progress timer.
===========================================================================*/
static void
CSequencePlayerWin_ProgTimerNotify(CSequencePlayerWin *pme)
{
  pme->m_sSequenceFile[pme->m_wCurMedia].m_bProgTimer = FALSE;
  if ((pme->m_wCurFile == pme->m_wCurMedia) &&
      (!pme->m_sSequenceFile[pme->m_wCurFile].m_bPlay))
    CMediaPlayer_Redraw(pme->m_pOwner, TRUE);
}

/*===========================================================================
   This function toggles full screen mode and does a redraw.
===========================================================================*/
static boolean
CSequencePlayerWin_FullScreen(CSequencePlayerWin *pme, boolean bFull,
                              boolean bDeferRedraw)
{
  CSequenceFile *psf = &(pme->m_sSequenceFile[pme->m_wCurFile]);

  if (!psf->m_bImage)
    return FALSE;

  if (psf->m_bFullScreen == bFull)
  {
    return TRUE;                       // No need to do anything, we're
                                       // in the right mode
  }

  if (!psf->m_bFullScreen)
  {
    /*
     * Going to fullscreen.
     */
    SETAEERECT(&psf->m_rectImage, 0, 0, pme->m_pOwner->m_cxWidth,
               pme->m_pOwner->m_cyHeight);
  }
  else
  {
    /*
     * Restore the default image rect.
     */
    (void)MEMCPY(&psf->m_rectImage, &psf->m_dfltRectImage, sizeof(AEERect));
  }

  DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with x = %d, y = %d, cx = %d, cy = %d", psf->m_rectImage.x, psf->m_rectImage.y, psf->m_rectImage.dx, psf->m_rectImage.dy);
  (void)IMEDIA_SetRect(psf->m_pMedia, &psf->m_rectImage, NULL);

  DBGPRINTF_MED("MP: Setting FullScreen to %d", bFull);
  psf->m_bFullScreen = bFull;

  CMediaPlayer_Redraw(pme->m_pOwner, bDeferRedraw);

  return TRUE;
}

#endif     // FEATURE_MULTISEQUENCER

#ifdef FEATURE_MP4_LCD_CONTRAST

/*===========================================================================

                     CContrastWin Functions

========================================================================== */
static IWindow *
CContrastWin_New(CMediaPlayer *pOwner)
{
  CContrastWin *pme;

  VTBL(IWindow)vtbl;

  MP_IWINDOW_SETVTBL(&vtbl, CContrastWin_Enable, CContrastWin_Redraw,
                     CContrastWin_HandleEvent, CContrastWin_Delete);
  pme = (CContrastWin *)CWindow_New(sizeof(CContrastWin), pOwner, &vtbl);
  if (!pme)
    return NULL;

  {
    int       cx = pme->m_pOwner->m_cxWidth;
    int       cy = pme->m_pOwner->m_cyHeight;
    AEERect   rect;

    /* The SC1X requires a smaller color grid for contrast changes. */
    if (pOwner->m_bSmallScreenSize)
    {
      pme->m_pChartImage =
        ISHELL_LoadResImage(pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDB_MCB_CHART_SM);
    }
    else
    {
      pme->m_pChartImage =
        ISHELL_LoadResImage(pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDB_MCB_CHART);
    }
    if (!pme->m_pChartImage)
    {
    	DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
      MP_WINERR_RETURN(pme)
    }

    if (ISHELL_CreateInstance
        (pme->m_pIShell, AEECLSID_STATIC, (void **)&pme->m_pContrastText))
    {
    	DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");    
      MP_WINERR_RETURN(pme)
    }

    SETAEERECT(&pme->m_rectChart, 0, 0, cx, cy);
    SETAEERECT(&rect, 0, (cy - 20), cx, 20);
    ISTATIC_SetRect(pme->m_pContrastText, &rect);
    UpdateContrast(pme, 0);
  }

  return (IWindow *)pme;
}


/*===========================================================================
   This function deletes the Contrast window.
===========================================================================*/
static void
CContrastWin_Delete(IWindow *po)
{
  CContrastWin *pme = (CContrastWin *)po;
  CMediaPlayer *pMP = pme->m_pOwner;

  MP_RELEASEIF(pme->m_pChartImage);
  MP_RELEASEIF(pme->m_pContrastText);

  FREE(pme);

  if(pMP)
  {
    pMP->m_pWin = NULL;
    pMP->m_eActiveWin = MPW_NONE;
  }
}

/*===========================================================================
   This function enables/disables the Contrast window.
===========================================================================*/
static void
CContrastWin_Enable(IWindow *po, boolean bEnable)
{
  CContrastWin *pme = (CContrastWin *)po;

  if (!CWindow_ProcessEnable(po, bEnable))
    return;

  if (!pme->m_bActive)
  {
    IIMAGE_Stop(pme->m_pChartImage);
    return;
  }

  MP_DrawImage(pme->m_pChartImage, &pme->m_rectChart, TRUE);
  UpdateContrast(pme, 0);
}

/*===========================================================================
   This function redraws the Contrast window.
===========================================================================*/
static void
CContrastWin_Redraw(IWindow *po)
{
  CContrastWin *pme = (CContrastWin *)po;

  if (!pme->m_bActive)
    return;

  IDISPLAY_ClearScreen(pme->m_pIDisplay);

  MP_DrawImage(pme->m_pChartImage, &pme->m_rectChart, TRUE);
  UpdateContrast(pme, 0);

  IDISPLAY_Update(pme->m_pIDisplay);
}

/*===========================================================================
   This function processes events routed to Contrast window.
===========================================================================*/
static boolean
CContrastWin_HandleEvent(IWindow *po, AEEEvent eCode, uint16 wParam,
                         uint32 dwParam)
{
  CContrastWin *pme = (CContrastWin *)po;

  if (MP_ISCLR(eCode) || MP_ISCMD(eCode, IDS_OK))
  {
    CMediaPlayer_Settings(pme->m_pOwner);
  }

  switch (eCode)
  {
  case EVT_KEY:
    switch (wParam)
    {
    case AVK_UP:
      UpdateContrast(pme, MP_CONTRAST_STEP);
      return TRUE;

    case AVK_DOWN:
      UpdateContrast(pme, -MP_CONTRAST_STEP);
      return TRUE;
    }
    return TRUE;
  }
  return FALSE;
}
#endif     // FEATURE_MP4_LCD_CONTRAST

/*===========================================================================

                     ProgCtl Functions

========================================================================== */

#ifdef FEATURE_MULTISEQUENCER

/*===========================================================================
   This funtion initializes the ProgCtl.
   ProgCtl contains two rectangles: main rectangle and within it the progress
   bar rectangle. The title is displayed on left of the progress bar within
   the main rectangle.
===========================================================================*/

static boolean
CProgCtl_Init(CProgCtl *pme, AEERect *pRectMain)
{
  AEERect   rect;
  int       x,
            y;
  int       dxProg,
            dyProg;

  (void)MEMCPY(&pme->m_rectMain, pRectMain, sizeof(AEERect));

  if (ISHELL_CreateInstance
      (pme->m_pIShell, AEECLSID_STATIC, (void **)&pme->m_pTitle))
    return FALSE;

  /*
   * ProgBar rect
   */
  dxProg = pRectMain->dx / MP_PROGBAR_DX_DIV;
  dyProg = pRectMain->dy / MP_PROGBAR_DY_DIV;
  x = pRectMain->x + pRectMain->dx - dxProg + 1;
  y = pRectMain->y + pRectMain->dy / 2 - dyProg / 2;

  if (dxProg > MP_PROGBAR_DX_OFFSET)
    dxProg -= MP_PROGBAR_DX_OFFSET;
  SETAEERECT(&pme->m_rectBar, x, y, dxProg, dyProg);

  SETAEERECT(&rect, pRectMain->x + 1, pRectMain->y + 1, pRectMain->dx - dxProg,
             pRectMain->dy);
  ISTATIC_SetRect(pme->m_pTitle, &rect);
  ISTATIC_SetProperties(pme->m_pTitle, ST_CENTERTEXT | ST_NOSCROLL);

  return TRUE;
}

/*===========================================================================
   This function updates the progress bar and the title.
===========================================================================*/
static void
CProgCtl_SetPos(CProgCtl *pme, AECHAR *psz, uint16 wPct)
{
  if (!pme)
  {
    DBGPRINTF_ERROR("MP:  pme is NULL in CProgCtl_SetPos!");
    return;
  }
  if (!pme->m_pIDisplay)
  {
    DBGPRINTF_ERROR("MP:  pme->m_pIDisplay is NULL in CProgCtl_SetPos!");
    return;
  }
  if (!pme->m_pTitle)
  {
    DBGPRINTF_ERROR("MP: pme->m_pTitle is NULL in CProgCtl_SetPos!");
    return;
  }
//#if defined(MP_FEATURE_MBMS)
//  MP_FitStaticText(pme->m_pIDisplay, pme->m_pTitle, AEE_FONT_LARGE, psz);
//#else
  MP_FitStaticText(pme->m_pIDisplay, pme->m_pTitle, AEE_FONT_NORMAL, psz);
//#endif
  (void)ISTATIC_Redraw(pme->m_pTitle);
  CProgCtl_DrawHist(pme, wPct);
  MP_FrameRect(pme->m_pIDisplay, &pme->m_rectMain);
}


/*===========================================================================
   This function deletes the ProgCtl
===========================================================================*/
static void
CProgCtl_Release(CProgCtl *pme)
{
  MP_RELEASEIF(pme->m_pTitle);
}

/*===========================================================================
   This function fills wPct of  progress bar rectangle.
===========================================================================*/
static void
CProgCtl_DrawHist(CProgCtl *pme, uint16 wPct)
{
  AEERect   rc;

  (void)MEMCPY(&rc, &pme->m_rectBar, sizeof(AEERect));

  if (wPct > 100)
    wPct = 100;

  (void)IDISPLAY_DrawFrame(pme->m_pIDisplay, &rc, AEE_FT_RAISED, CLR_SYS_SCROLLBAR);
  rc.dx = (rc.dx *wPct) / 100;
  IDISPLAY_FillRect(pme->m_pIDisplay, &rc, MP_CLR_PROG_FILL);
}
#endif // FEATURE_MULTISEQUENCER

/*===========================================================================

                     3GPPTTextCtl Functions

========================================================================== */
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT

/*===========================================================================
   This funtion initializes the 3GPPTTextCtl.
   3GPPTTextCtl contains one rectangle in which it will display any timed text.
===========================================================================*/
static boolean
C3GPPTTextCtl_Init(C3GPPTTextCtl *pme, AEERect *pRectMain)
{
  (void)MEMCPY(&pme->m_rectMain, pRectMain, sizeof(AEERect));

  if (ISHELL_CreateInstance
      (pme->m_pIShell, AEECLSID_STATIC, (void **)&pme->m_pTextCtl))
    return FALSE;

  ISTATIC_SetRect(pme->m_pTextCtl, pRectMain);
  ISTATIC_SetProperties(pme->m_pTextCtl, ST_CENTERTEXT);
  return TRUE;
}

/*===========================================================================
   This function updates the Timed Text.
===========================================================================*/
static void
C3GPPTTextCtl_Update(C3GPPTTextCtl *pme, AECHAR *psz)
{
  AECHAR    sz[1] = { '\0' };
  
  if(psz)
    (void)ISTATIC_SetText(pme->m_pTextCtl, NULL, psz, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
  else
    (void)ISTATIC_SetText(pme->m_pTextCtl, NULL, sz, AEE_FONT_NORMAL, AEE_FONT_NORMAL);

  (void)ISTATIC_Redraw(pme->m_pTextCtl);
  MP_FrameRect(pme->m_pIDisplay, &pme->m_rectMain);
}

/*===========================================================================
   This function deletes the 3GPPTTextCtl
===========================================================================*/
static void
C3GPPTTextCtl_Release(C3GPPTTextCtl *pme)
{
  MP_RELEASEIF(pme->m_pTextCtl);
  pme->m_pTextCtl = NULL;
}

#endif     // FEATURE_MP4_3GPP_TIMED_TEXT

/*===========================================================================

                     Local Functions

========================================================================== */
/*===========================================================================
  This function will block current thread until
  the playstate becomes TRUE or reach to MAX time
  nMaxTime must be multiple of 20

  Note, it always timeout since the callback from OEM is in same UI thread
===========================================================================*/
static void MP_WaitOnState(CPlayerWin* pme, MediaPlayerPlayState state, uint32 nMaxTime)
{
  int i;
  int cnt;
  int nSleepTime = 20; // 20 ms

  if(nMaxTime >= nSleepTime && nMaxTime < MAX_WAIT_TIME)
    cnt = nMaxTime/nSleepTime;
  else
    cnt = MAX_WAIT_TIME/nSleepTime;

  for(i=0; (i < cnt) &&
      (pme->m_pOwner->m_playState != state && pme->m_pOwner->m_ErrorState != MP_ERRORSTATE_ERROR); i++)
  {
    DBGPRINTF_HIGH("MP: sleep on waiting  i = %d, time = %d.", i, nSleepTime);
    MSLEEP(nSleepTime);
  }

  if(pme->m_pOwner->m_playState == state)
  {
    DBGPRINTF_HIGH("MP: WaitOnState received: %d[%s]",
                   state,
                   MP_MediaPlayerPlayStateEnumToString(state));
  }
  else if(pme->m_pOwner->m_ErrorState == MP_ERRORSTATE_ERROR)
  {
    DBGPRINTF_HIGH("MP: received FAIL statue on waiting state: %d[%s]",
                   state,
                   MP_MediaPlayerPlayStateEnumToString(state));
  }
  else
  {
    DBGPRINTF_HIGH("MP: time out on waiting state: %d[%s]",
                   state,
                   MP_MediaPlayerPlayStateEnumToString(state));
  }
}
/*===========================================================================
   This function draws the image and centers it within the specified
   rectangle if bCenter is TRUE.
===========================================================================*/
static void
MP_DrawImage(IImage *pImage, AEERect *pRect, boolean bCenter)
{
  AEEImageInfo ii;
  int       x;
  int       y;

  IIMAGE_GetInfo(pImage, &ii);

  /*
   * Do not display if image does not fit in the allocated rectangle.
   */
  if (ii.cx > pRect->dx || ii.cy > pRect->dy)
    return;

  if (bCenter)
  {
    x = pRect->x + (pRect->dx / 2) - (ii.cxFrame / 2);
    y = pRect->y + (pRect->dy / 2) - (ii.cy / 2);
  }
  else
  {
    x = pRect->x;
    y = pRect->y;
  }

  IIMAGE_Start(pImage, x, y);
}

/*===========================================================================
   This function adds one item to the specified IMenuCtl.
===========================================================================*/
boolean MP_AddMenuItem(IMenuCtl *pMenu, uint16 wTextID, AECHAR *pText,
               uint16 wImageID, uint16 wItemID, uint32 dwData)
{
  CtlAddItem ai;

  (void)MEMSET(&ai, '\0', sizeof(ai));

  /*
   * Fill in the CtlAddItem structure values
   */
  ai.pText = pText;
  ai.pImage = NULL;
  ai.pszResImage = MEDIAPLAYER_RES_FILE;
  ai.pszResText = MEDIAPLAYER_RES_FILE;
  ai.wText = wTextID;
#if defined(MP_FEATURE_MBMS)
  ai.wFont = AEE_FONT_LARGE;
#else
  ai.wFont = AEE_FONT_NORMAL;
#endif
  ai.wImage = wImageID;
  ai.wItemID = wItemID;
  ai.dwData = dwData;

  /*
   * Add the item to the menu control
   */
  return IMENUCTL_AddItemEx(pMenu, &ai);
}

/*===========================================================================
   This function adds one item to the PlayerMenuCtl.
===========================================================================*/
static boolean
MP_AddPlayerMenuItem(CMediaPlayer *pme, IMenuCtl *pMenu, uint16 wTextID, AECHAR *pText,
               uint16 wImageID, uint16 wItemID, uint32 dwData)
{
  CtlAddItem ai;

  /*
   * Fill in the CtlAddItem structure values
   */
  ai.pText = pText;
  ai.pImage = NULL;
  ai.pszResImage = MEDIAPLAYER_RES_FILE;
  ai.pszResText = MEDIAPLAYER_RES_FILE;
  ai.wText = wTextID;
  ai.wFont = AEE_FONT_NORMAL;
  //Please Note: The small player menu icons (8x8) resource ids start from
  //1000s. For example IDB_PLAY (12x12) resource ID is 4 and the
  //corresponding small icon resource ID is 1004 (IDB_PLAY_SM).
  //So while new icons are added this order should be followed.
  ai.wImage = (pme->m_bSmallScreenSize)?wImageID+1000:wImageID;
  ai.wItemID = wItemID;
  ai.dwData = dwData;

  /*
   * Add the item to the menu control
   */
  return IMENUCTL_AddItemEx(pMenu, &ai);
}

/*===========================================================================
   This function displays the specified error message and disables the
   current window.
===========================================================================*/
void MP_ErrorDlg(CMediaPlayer *pme, uint16 wResErrID)
{
  AEEPromptInfo pi;
  uint16    wButtonIDs[] = { IDS_OK, 0 };

  if (pme->m_pWin)
    CMediaPlayer_DisableWin(pme);

  pi.pszRes = MEDIAPLAYER_RES_FILE;
  pi.pTitle = NULL;
  pi.pText = NULL;
  pi.wTitleID = 0;                     // IDS_ERR_BASE;
  pi.wTextID = wResErrID;
  pi.wDefBtn = IDS_OK;
  pi.pBtnIDs = wButtonIDs;
  pi.dwProps = ST_MIDDLETEXT | ST_CENTERTITLE;
  pi.fntTitle = AEE_FONT_BOLD;
  pi.fntText = AEE_FONT_NORMAL;
  pi.dwTimeout = 10000;
  if (ISHELL_Prompt(pme->a.m_pIShell, &pi) == FALSE)
    return;

  IDISPLAY_Update(pme->a.m_pIDisplay);
}

/*===========================================================================
   This function sets the menu attributes based on BREW Style Sheet for
   menu control.
===========================================================================*/
void MP_SetMenuAttr(IMenuCtl *pMenu, AEECLSID clsMenu, uint16 nColorDepth,
               AEERect *pRect, uint32 dwProps)
{
  AEEItemStyle sel,
            normal;
  AEEMenuColors col;

  /*
   * Menu Style
   */
  normal.ft = MENU8_FT;
  normal.xOffset = 0;
  normal.yOffset = 0;
  normal.roImage = MENU8_RO;

  sel.ft = MENU8_SELECT_FT;
  sel.xOffset = 0;
  sel.yOffset = 0;
  sel.roImage = MENU8_SELECT_RO;

  /*
   * Menu Colors
   */
  col.cSelText = MENU8_SELECT_TEXT;
  col.wMask = MENU8_COLOR_MASK;

  if (clsMenu == AEECLSID_MENUCTL)
  {
    col.cBack = MENU8_BACKGROUND;
    col.cSelBack = MENU8_SELECT_BACKGROUND;

    dwProps |= IMENUCTL_GetProperties(pMenu);
  }
  else if (clsMenu == AEECLSID_SOFTKEYCTL || clsMenu == AEECLSID_ICONVIEWCTL)
  {
    col.cBack = TB8_BACKGROUND;
    col.cSelBack = TB8_SELECT_BACKGROUND;

    dwProps |= MP_ICON_TEXT_TOP | MP_NO_ARROWS;
  }

  if (clsMenu == AEECLSID_MENUCTL || clsMenu == AEECLSID_SOFTKEYCTL ||
      clsMenu == AEECLSID_ICONVIEWCTL)
  {
    IMENUCTL_SetStyle(pMenu, &normal, &sel);
    IMENUCTL_SetColors(pMenu, &col);
    IMENUCTL_SetProperties(pMenu, dwProps);
    if (pRect)
    {
      DBGPRINTF_MED("MP: Setting IMENUCTL_SetRect to x = %d, y = %d, dx = %d, dy = %d", pRect->x, pRect->y, pRect->dx, pRect->dy);
      IMENUCTL_SetRect(pMenu, pRect);
    }
  }
}

/*===========================================================================
   This function sends a media command notification.
===========================================================================*/
static void
CPlayerWin_SendMediaNotify(CPlayerWin *pme, int nCmd, int nSubCmd, int nStatus,
                           void *pData, uint32 dwSize)
{
  AEEMediaCmdNotify mcn;
  AEECLSID  cls;

  if (pme->m_pMedia)
  {
    (void)IMEDIA_GetClassID(pme->m_pMedia, &cls);
    mcn.clsMedia = cls;
  }
  else
  {
    mcn.clsMedia = 0;
  }
  mcn.pIMedia = pme->m_pMedia;
  mcn.nCmd = nCmd;
  mcn.nSubCmd = nSubCmd;
  mcn.nStatus = nStatus;
  mcn.pCmdData = pData;
  mcn.dwSize = dwSize;

  CMediaPlayer_MediaNotify(pme->m_pOwner, &mcn);
}

/*===========================================================================
   This function makes an IMEDIA_GetTotalTime() request and resets the
   retry count.
===========================================================================*/
static int  nGetTotalTimeRetryCount = 0;
static int
CPlayerWin_GetTotalTimeRequest(CPlayerWin *pme)
{
  int nRet;

  nGetTotalTimeRetryCount = 0;
  //nRet = IMEDIA_GetTotalTime(pme->m_pMedia);
  nRet = CPlayerWin_GetTotalTime(pme);
  return nRet;
}
/*===========================================================================
   This function makes an IMEDIA_GetTotalTime() if the retry counter has not 
   been exceeded.
===========================================================================*/
static int
CPlayerWin_GetTotalTimeRetry(CPlayerWin *pme)
{
  int nRet;

  nGetTotalTimeRetryCount++;
  if (nGetTotalTimeRetryCount > MP_GETTOTALTIME_RETRY_MAX)
  {
    DBGPRINTF_HIGH("MP: CPlayerWin_GetTotalTimeRetry: Retry Count Exceeded");
    return EFAILED;
  }

  //nRet = IMEDIA_GetTotalTime(pme->m_pMedia);
  nRet = CPlayerWin_GetTotalTime(pme);
  return nRet;
}
/*===========================================================================
   This function makes the IMEDIA_GetTotalTime() request after checking parameters.
===========================================================================*/
static int
CPlayerWin_GetTotalTime(CPlayerWin *pme)
{
  int nRet;

  if (pme == NULL)
  {
    DBGPRINTF_ERROR("MP: CPlayerWin_GetTotalTime: pme=NULL");
    return EFAILED;
  }

  if (pme->m_pMedia == NULL)
  {
    DBGPRINTF_ERROR("MP: CPlayerWin_GetTotalTime: pme->m_pMedia=NULL");
    return EFAILED;
  }

  nRet = IMEDIA_GetTotalTime(pme->m_pMedia);
  DBGPRINTF_HIGH("MP: IMEDIA_GetTotalTime() returns %d = %s", nRet, MP_ErrorStateToString(nRet));

  return nRet;
}
/*===========================================================================
   This function determines if a given media contains video
===========================================================================*/
boolean CPlayerWin_IsVideoMedia(CPlayerWin *pme)
{
  boolean bRet = FALSE;
  char  *pExtName = NULL;

  if(pme && pme->m_pMedia)
  {
    bRet = CPlayerWin_IsMPEG4Video(pme);
    if(!bRet)
    {
      uint32 dwCaps;
      if(SUCCESS == IMEDIA_GetMediaParm(pme->m_pMedia, MM_PARM_CAPS, (int32 *)&dwCaps, 0))
      {
        bRet = (dwCaps & MM_CAPS_VIDEO);
      }
    }
    
    if (bRet && pme->m_pszFile)
    {
      pExtName = MP_GetFileNameExtension(pme->m_pszFile);
      if (pExtName && CMediaPlayer_IsAudioFileExension(pExtName))
        bRet = FALSE;
    }
  }
  return bRet;
}

/*===========================================================================

                     IMedia Callback Function

========================================================================== */

/*===========================================================================
   This is the main callback notification function for the IMedia object
   within the player window.
   (1)Handles Play, Record, GetTotalTime, GetMediaParm and
       SetMediaParm commands and corresponding status for those commands.
   (2)Updates the ProgCtl, if required.
===========================================================================*/
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else //FEATURE_APP_MEDIA_VIDEO
static void CMediaPlayer_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
{
  CMediaPlayer* pMP;
  CPlayerWin*   pme;
  uint16        nTextID = 0;
#ifdef FEATURE_APP_MPEG4
  boolean       CallUpdateProgCtl = FALSE;
#endif //FEATURE_APP_MPEG4
  /*
   * do not process any event when exiting or NULL ptr
   */
  pMP = (CMediaPlayer*)pUser;
  if(pMP == NULL)
  {
    DBGPRINTF_ERROR("MP: MediaNotify, m_pOwner is NULL.");
    return;
  }
  pme = (CPlayerWin *)pMP->m_pWin;
  if(pme == NULL)
  {
    DBGPRINTF_ERROR("MP: MediaNotify, pme is NULL.");
    return;
  }

  /*
   * If playing in the background, then we MUST ignore notices since
   * CPlayerWin will have been deleted.
   */
  if (mp_ignoreNotices) {
    DBGPRINTF_HIGH("MP: CMediaPlayer_MediaNotify() ignoring notice");
    return;
  }
   
  DBGPRINTF_MED("MP: MediaNotify, pUser=0%X, pme=0x%X, nCmd=%d, nStatus=%d, nSubCmd=%d, dwSize=%d, pCmdData=0x%X",
                 pUser, pme, pCmdNotify->nCmd, pCmdNotify->nStatus, pCmdNotify->nSubCmd, pCmdNotify->dwSize, pCmdNotify->pCmdData);

#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
  if((pMP->m_eActiveWin != MPW_PLAYER) && (pMP->m_eActiveWin != MPW_STREAM_RECORDER))
  {
    DBGPRINTF_ERROR("MP: MediaNotify, active win is neither MPW_PLAYER nor MPW_STREAM_RECORDER (%d).", pme->m_pOwner->m_eActiveWin);
    return;
  }
#else
  if(pMP->m_eActiveWin != MPW_PLAYER)
  {
    DBGPRINTF_ERROR("MP: MediaNotify, active win is not MPW_PLAYER (%d).", pme->m_pOwner->m_eActiveWin);
    return;
  }
#endif

  if (pme->m_pOwner != pMP)
  {
    DBGPRINTF_ERROR("MP: Player window owner %p not equal to MediaPlayer object %p.", pme->m_pOwner, pMP);
    return;
  }

#ifdef MP_FEATURE_ICONTENTHANDLER
  if((pme->m_pMedia != pCmdNotify->pIMedia) && (!pMP->m_bIContentMode))
#else
  if(pme->m_pMedia != pCmdNotify->pIMedia)
#endif
  {
    DBGPRINTF_ERROR("MP: MediaNotify, notify for media not in current MPW_PLAYER window %p %p.", pme->m_pMedia, pCmdNotify->pIMedia);
    return;
  }
  if ((pMP->m_playState == MP_PLAYSTATE_EXITING) && (pme->m_CB.m_pFnPtr == NULL))
  {
    DBGPRINTF_LOW("MP: exiting from player win.");
    EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
    return;
  }
  if(pme->m_pMedia == NULL)
  {
    DBGPRINTF_ERROR("MP: MediaNotify, m_pMedia is NULL.");
    return;
  }

  if (!pCmdNotify)
  {
    DBGPRINTF_ERROR("MP:  pCmdNotify is NULL in CMediaPlayer_MediaNotify!");
    return;
  }


 switch (pCmdNotify->nStatus)
  {
#ifdef MP_FEATURE_QTV_DIVX_DRM
  case MM_MP4_STATUS_DRM_AUTHORIZATION_ERROR:
    if(pMP->m_pDlg == NULL)
    {
      //create Dlg
      pMP->m_pDlg = (CMPDlg*)MPDlgCreate(pMP, pMP->a.m_pIShell, pme->m_pIDisplay, &pme->m_rectImage);
      if(pMP->m_pDlg)
      {
        (void)MPDlgSetText(pMP->m_pDlg, "Device Authorization Error:", "\nThe user is not authorized to play this video.");
        (void)MPDlgAddButton(pMP->m_pDlg, MP_DLG_OK, (ButtonHandlerT)&MPDlgButtonHandler_OK);
        (void)MPDlgSetButtonFocus(pMP->m_pDlg, MP_DLG_OK);
        (void)MPDlgUpdate(pMP->m_pDlg, pMP->m_pIGraphics);
      }
      else
      {
        DBGPRINTF_ERROR("MP: failed to create dlg.");
      }
    }
    else
    {
      DBGPRINTF_ERROR("MP: unable to create dlg, dlg exists.");
    }
    break;
  case MM_MP4_STATUS_DRM_DEVICE_REGISTRATION_ERROR:
    if(pMP->m_pDlg == NULL)
    {
      //create Dlg
      pMP->m_pDlg = (CMPDlg*)MPDlgCreate(pMP, pMP->a.m_pIShell, pme->m_pIDisplay, &pme->m_rectImage);
      if(pMP->m_pDlg)
      {
        (void)MPDlgSetText(pMP->m_pDlg, "Device Registration Error:", "\nThe device is not registered to play this video.");
        (void)MPDlgAddButton(pMP->m_pDlg, MP_DLG_OK, (ButtonHandlerT)&MPDlgButtonHandler_OK);
        (void)MPDlgSetButtonFocus(pMP->m_pDlg, MP_DLG_OK);
        (void)MPDlgUpdate(pMP->m_pDlg, pMP->m_pIGraphics);
      }
      else
      {
        DBGPRINTF_ERROR("MP: failed to create dlg.");
      }
    }
    else
    {
      DBGPRINTF_ERROR("MP: unable to create dlg, dlg exists.");
    }
    break;
  case MM_MP4_STATUS_DRM_RENTAL_COUNT_EXPIRED:
    if(pMP->m_pDlg == NULL)
    {
      //create Dlg
      pMP->m_pDlg = (CMPDlg*)MPDlgCreate(pMP, pMP->a.m_pIShell, pme->m_pIDisplay, &pme->m_rectImage);
      if(pMP->m_pDlg)
      {
        (void)MPDlgSetText(pMP->m_pDlg, "Rental Expired:", "\nThis rental has 0 views left.");
        (void)MPDlgAddButton(pMP->m_pDlg, MP_DLG_OK, (ButtonHandlerT)&MPDlgButtonHandler_OK);
        (void)MPDlgSetButtonFocus(pMP->m_pDlg, MP_DLG_OK);
        (void)MPDlgUpdate(pMP->m_pDlg, pMP->m_pIGraphics);
      }
      else
      {
        DBGPRINTF_ERROR("MP: failed to create dlg.");
      }
    }
    else
    {
      DBGPRINTF_ERROR("MP: unable to create dlg, dlg exists.");
    }
    break;
  case MM_MP4_STATUS_DRM_PLAYBACK_GENERAL_ERROR:
    if(pMP->m_pDlg == NULL)
    {
      //create Dlg
      pMP->m_pDlg = (CMPDlg*)MPDlgCreate(pMP, pMP->a.m_pIShell, pme->m_pIDisplay, &pme->m_rectImage);
      if(pMP->m_pDlg)
      {
        (void)MPDlgSetText(pMP->m_pDlg, "Error:", "\nDRM playback error.");
        (void)MPDlgAddButton(pMP->m_pDlg, MP_DLG_OK, (ButtonHandlerT)&MPDlgButtonHandler_OK);
        (void)MPDlgSetButtonFocus(pMP->m_pDlg, MP_DLG_OK);
        (void)MPDlgUpdate(pMP->m_pDlg, pMP->m_pIGraphics);
      }
      else
      {
        DBGPRINTF_ERROR("MP: failed to create dlg.");
      }
    }
    else
    {
      DBGPRINTF_ERROR("MP: unable to create dlg, dlg exists.");
    }
    break;
  case MM_MP4_STATUS_CONSUME_RENTAL_VIEW_CONFIRMATION:
    {
      int32 size;
      AEEMediaMPEG4Spec* pSpec; 

      if(SUCCESS == IMEDIA_GetMediaSpec(pme->m_pMedia, &pSpec, &size))
      {
        if (size == sizeof(AEEMediaMPEG4Spec))
        {
          if(pSpec->eFormatType == MM_DIVX_DRM_FORMAT_TYPE)
          {
            AEEMediaMPEG4DivXDRMSpec* pDivXSpec;
            char buf[256];
   
            pDivXSpec = (AEEMediaMPEG4DivXDRMSpec*)&pSpec->ExtendedInfo;
     
            (void)SNPRINTF(buf, sizeof(buf), 
              "\nMax. # of view allowed = %d \ncurrent view cout = %d \nCgmsa, Acptb and DigitalProtection Signal = ", 
               pDivXSpec->nUseLimit, pDivXSpec->nUseCounter); 
            CMediaPlayer_DivXDRMSpecCat(pDivXSpec, buf, 256);

            if(pMP->m_pDlg == NULL)
            {
              pMP->m_pDlg = (CMPDlg*)MPDlgCreate(pMP, pMP->a.m_pIShell, pme->m_pIDisplay, &pme->m_rectImage);
              if(pMP->m_pDlg)
              {
                (void)MPDlgSetText(pMP->m_pDlg, "DivX info", buf);
                (void)MPDlgAddButton(pMP->m_pDlg, MP_DLG_OK, (ButtonHandlerT)&MPDlgButtonHandler_OK);
                (void)MPDlgSetButtonFocus(pMP->m_pDlg, MP_DLG_OK);
                (void)MPDlgUpdate(pMP->m_pDlg, pMP->m_pIGraphics);
              }
              else
              {
                DBGPRINTF_ERROR("MP: failed to create dlg.");
              }
            }
            else
            {
              DBGPRINTF_ERROR("MP: unable to create dlg, dlg exists.");
            }
          }
          else
          {
            DBGPRINTF_ERROR("MP: incorrect SPEC format type %d.", pSpec->eFormatType);
          }
        }
        else
        {
          DBGPRINTF_ERROR("MP: invalid SPEC size.");
        }
      }
      else
      {
        DBGPRINTF_ERROR("MP: IMEDIA_GetMediaSpec failed.");
      }
    }
    break;
#endif //MP_FEATURE_QTV_DIVX_DRM
#if defined(MM_MP4_STATUS_SKIP_SEEK_FAILED)
  case MM_MP4_STATUS_SKIP_SEEK_FAILED:
    DBGPRINTF_MED("MP: MM_MP4_STATUS_SKIP_SEEK_FAILED");
#endif
  case MM_STATUS_SEEK_FAIL:
    CPlayerWin_ClearMediaBusy(pme, MEDIA_BUSY_SEEK);
    DBGPRINTF_MED("MP: Resetting seek values");
    pme->m_seekPending = pme->m_pendingSeekOffset = pme->m_reseek = 0;
    pme->m_nextClipOffsetAdj = 0;
    pme->m_startSeekPosition = 0;
    pme->m_skipPending = FALSE;
    pme->m_reskip = FALSE;
    pme->m_skipOffset = 0;
    if ((uint32) (pCmdNotify->pCmdData) == MM_SEEK_MODE_FRAME)
    {
      /*
       * no more previous frames available to step-back
       */
      break;
    }
    pme->m_pOwner->m_ErrorState = MP_ERRORSTATE_ERROR;
    nTextID = IDS_ERR_PLAYCTL_CMD;
    DBGPRINTF_ERROR("MP: received MM_STATUS_SEEK_FAIL/MM_MP4_STATUS_SKIP_SEEK_FAIL events");
    break;
  case MM_STATUS_PAUSE_FAIL:
    CPlayerWin_ClearMediaBusy(pme, MEDIA_BUSY_PAUSE);
    pme->m_pOwner->m_ErrorState = MP_ERRORSTATE_ERROR;
    nTextID = IDS_ERR_PLAYCTL_CMD;
    DBGPRINTF_ERROR("MP: received MM_STATUS_PAUSE_FAIL events");
    break;
  case MM_STATUS_RESUME_FAIL:
    CPlayerWin_ClearMediaBusy(pme, MEDIA_BUSY_RESUME);
    pme->m_pOwner->m_ErrorState = MP_ERRORSTATE_ERROR;
    nTextID = IDS_ERR_PLAYCTL_CMD;
    if(pme->m_pOwner->m_bResumeInProcessing && pme->m_pOwner->m_bSuspendPending)
    {
        // we have a SUSPEND pending request during the resuming, we need to suspend again
      pme->m_pOwner->m_bResumeInProcessing = FALSE;
      pme->m_pOwner->m_bSuspendPending = FALSE;

      DBGPRINTF_MED("MP: MM_STATUS_RESUME_FAIL, SUSPEND pending, setback to suspend mode.");
      CMediaPlayer_ProcessSuspendEvent(pme->m_pOwner);
      return;
    }
    pme->m_pOwner->m_bResumeInProcessing = FALSE;
    pme->m_pOwner->m_bSuspendPending = FALSE;

    DBGPRINTF_ERROR("MP: received MM_STATUS_RESUME_FAIL events");
    break;

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif     // FEATURE_QTV_MFDRM

#if defined(FEATURE_APP_MPEG4) || defined(FEATURE_APP_REAL)
#if defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)
#ifdef MM_MP4_STATUS_DL_DATA_AVAILABLE
    case MM_MP4_STATUS_DL_DATA_AVAILABLE:
      {
        int nRet;
        char *buffer = NULL;
        int32 bufferSize = 0;
        extern void MP_WriteIntoArchiveBuffer(char *buffer, int size);

        /* Request the encrypted data here. */
        nRet = IMEDIA_GetMediaParm(pme->m_pMedia, MM_MP4_PARAM_HTTP_GET_ENCRYPTED_DATA, (int32 *)(&buffer), (int32 *)(&bufferSize));
        DBGPRINTF_MED("MP: MM_MP4_PARAM_HTTP_GET_ENCRYPTED_DATA returns %d = %s",
                  nRet, MP_ErrorStateToString(nRet));
        MP_WriteIntoArchiveBuffer(buffer, bufferSize);
        nRet = IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARAM_HTTP_FREE_DOWNLOAD_BUFFER, (int32)(buffer), 0);
        DBGPRINTF_MED("MP: MM_MP4_PARAM_HTTP_FREE_DOWNLOAD_BUFFER returns %d = %s",
                  nRet, MP_ErrorStateToString(nRet));

        if (pme->m_pOwner->m_bPacketVideoStreaming == TRUE)
        {
          /* clip will start playing after first chunk is downloaded */
          event_report(EVENT_MEDIA_PLAYER_START);
          CPlayerWin_UpdateMenu(pme, MP_MENU_DEFAULT_SELECTION);
        }
      }
      break;
#endif       // MM_MP4_STATUS_DL_DATA_AVAILABLE
#endif      // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
#endif     // MP_FEATURE_VIDEO || FEATURE_APP_REAL
  }

  if (pCmdNotify->nCmd == MM_CMD_PLAY || pCmdNotify->nCmd == MM_CMD_RECORD)     // IMEDIA_Play/IMEDIA_Record
                                                                                // events
  {
    uint32    dwPlayPos = pme->m_dwPlayPos;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    uint32    dwDownldPos = pme->m_dwDownldPos;
#ifdef MM_STATUS_DOWNLOAD_PERCENTAGE
    uint32    dwDownldPct  = pme->m_dwDownldPct;
#endif // MM_STATUS_DOWNLOAD_PERCENTAGE
#endif // FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

    if (pCmdNotify->nStatus != MM_STATUS_DONE &&
        pCmdNotify->nStatus != MM_STATUS_ABORT)
    {
      pme->m_bAbortStringActive = 0;
    }

    nTextID = pCmdNotify->nCmd == MM_CMD_PLAY ? IDS_PLAY : IDS_RECORD;
    switch (pCmdNotify->nStatus)
    {
    case MM_STATUS_SPACE_WARNING:
    case MM_STATUS_SPACE_ERROR:
      if (pCmdNotify->nCmd == MM_CMD_RECORD)
      {
        pme->m_bMemoryFull = TRUE;
        pme->m_bStopPressed = TRUE;
        (void)IMEDIA_Stop(pme->m_pMedia);
        CPlayerWin_DrawRecordMenu(pme);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION

        /*
         * no more space for recording
         */
        pme->m_pOwner->m_ErrorState = MP_ERRORSTATE_ERROR;
        DBGPRINTF_HIGH("MP: Received IMedia MM_STATUS_SPACE_ERROR event");
      }
      break;

#if defined(MP_FEATURE_VIDEO) || defined(FEATURE_APP_REAL)
    case MM_MP4_STATUS_CONNECTION_UPDATE:

      if (MM_MP4_STREAM_AUTHENTICATE == pCmdNotify->nSubCmd)
      {
        /*
         * Client/user needs to provide authentication
         *
         * Call QtvPlayer::GetServerChallenge(char *challenge), to obtain
         * the the server challenge as is. i.e., value field of the RTSP
         * header "WWW-Authenticate"
         *
         * Display the UI (BREW IDialog??)for authentication.
         * Get the username & password. Compose the value field of the
         * outgoing RTSP message header "Authorization:"
         *
         * Call QtvPlayer::SetQtvUserAuthInfoandRetry(char *authInfo),
         * pass the above composed value field as an argument
         *
         * The App must call
         * QtvPlayer::SetQtvUserAuthInfoandRetry(char *authInfo)
         *
         * App decides to cancel the authorization or If user enters no
         * authentication information, set authInfo to NULL
         *
         * Note: App MUST respond to this event by calling
         * QtvPlayer::SetQtvUserAuthInfoandRetry(char *authInfo)
         *
         * To be eligible to receive this event, App must register with
         * QtvPlayer by calling QtvPlayer::QtvAppCanAuthenticate(TRUE);
         *
         * By default, Qtv Player assumes that App has no capability to
         * provide authentication information, and never sends this event to
         * App.
         */

      }
      else
      {
        pCmdNotify->nSubCmd = MM_MP4_PARM_CONNECT_UPDATE;
//        BufferedTime = 0;
        CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd,
                                 nTextID);
        pme->m_pOwner->m_playState = MP_PLAYSTATE_CONNECTING;
        EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      }
      break;

    case MM_MP4_STATUS_PB_READY:
      {
#if defined(FEATURE_QTV_MDP_ASCALE) || defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
        boolean setAscalingValue = FALSE;
#endif //#if defined(FEATURE_QTV_MDP_ASCALE || defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)

		// This callback is meaningless if we're already playing.
		if (MP_PLAYSTATE_PLAYING == pme->m_pOwner->m_playState) {
			break;
		}

        pCmdNotify->nSubCmd = MM_MP4_PARM_PB_READY;
        pme->m_pOwner->m_bVideoPlaybackReady = TRUE;
        DBGPRINTF_MED("MP: Qtv player is in playback ready state");
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif     // FEATURE_QTV_MFDRM

        // For now with FEATURE_QTV_LGT_STREAMING_EXTENSIONS_OVERRIDE, do not hide the PAUSE button.
        // Later the MediaPlayer can read a configuration item to determine if it can issue PAUSE
        // commands to IMedia, which QTV will just perform as usual, except for actually sending
        // the PAUSE command to the server. This is what will happen by default.
#ifdef FEATURE_QTV_LGT_LIVESTREAMING
#error code not present
#endif /*#ifdef FEATURE_QTV_LGT_LIVESTREAMING*/

#ifdef FEATURE_QTV_MDP_ASCALE //
        (void)CPlayerWin_SetAScale( pme );
#endif    // FEATURE_QTV_MDP_ASCALE

        CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, nTextID);
        pme->m_pOwner->m_playState = MP_PLAYSTATE_READY_TO_PLAY;
        EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);

        CPlayerWin_UpdateMenu(pme, MP_MENU_DEFAULT_SELECTION);
        if (!pme->m_bFullScreen)
          IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_PLAY);

#if defined (FEATURE_APP_MPEG4) ||defined (FEATURE_APP_REAL)
        if(pme->m_pMedia)
        {
          AEECLSID  ClsId;
          (void)IMEDIA_GetClassID(pme->m_pMedia, &ClsId);
          if(ClsId == AEECLSID_MEDIAMPEG4)
          {
            //Restore the last zoom and rotation settings
            DBGPRINTF_MED("MP: Restoring the last zoom and rotation settings, IsImage = %d, isFullScreen = %d", pme->m_pOwner->m_bIsImage, pme->m_bFullScreen);
            if (!pme->m_pOwner->m_bIsImage && pme->m_bFullScreen)
		        {
              DBGPRINTF_MED("MP: Showing as Full Screen");

#ifdef FEATURE_QTV_MDP_ASCALE //
              (void) CPlayerWin_SetAScale( pme );
#endif //FEATURE_QTV_MDP_ASCALE

            }
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
            else
            {
              // Only set the scaling/zoom value if we have not already set the AscalingValue or
              // they have scaled the image, thus we still keep their last zoom
              if (MM_MPEG4_NO_SCALING != pme->m_pOwner->m_dwLastZoomVal || !setAscalingValue)
              {
                (void)IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_SCALING, (int32)pme->m_pOwner->m_dwLastZoomVal, 0);
              }
            }
            (void)IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_ROTATION, (int32)pme->m_pOwner->m_LastRotationVal, 0);
            pme->m_dwZoomScale = pme->m_pOwner->m_dwLastZoomVal;
            pme->m_Rotation = pme->m_pOwner->m_LastRotationVal;
#endif // defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
          }
        }
#endif // defined (FEATURE_APP_MPEG4) ||defined (FEATURE_APP_REAL)
      }
      break;

#endif     // MP_FEATURE_VIDEO

    case MM_STATUS_START:
    {
      AEECLSID  ClsId;

      if(pme->m_pOwner->m_bResumeInProcessing && pme->m_pOwner->m_bSuspendPending)
      {
        // we have a SUSPEND pending request during the play starting, we need to suspend
        pme->m_pOwner->m_bResumeInProcessing = FALSE;
        pme->m_pOwner->m_bSuspendPending = FALSE;

        // If playing make sure state is checked and updated so suspend will work correctly.
        if (CPlayerWin_IsPlaying(pme))
        {
          pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
          EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
        }

        DBGPRINTF_MED("MP: play started, SUSPEND pending, set to suspend mode.");
        CMediaPlayer_ProcessSuspendEvent(pme->m_pOwner);
        return;
      }
      // If we are going to be seeking don't clear these flags until seek done
      if (pme->m_pOwner->m_resumePlayPos <= 0)
      {
        pme->m_pOwner->m_bResumeInProcessing = FALSE;
        pme->m_pOwner->m_bSuspendPending = FALSE;
      }

      (void)IMEDIA_GetClassID(pme->m_pMedia, &ClsId);
      ConnectStatusStringActive = 0;
      event_report(EVENT_MEDIA_PLAYER_START);
      CPlayerWin_UpdateMenu(pme, IDM_PM_STOP);

      if (pCmdNotify->nCmd == MM_CMD_PLAY)
      {
        pme->m_skipSuccess = FALSE;
        pme->m_currentClipIndex = 0;
        pme->m_nextClipOffsetAdj = 0;
        pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
        EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      }
      else if (pCmdNotify->nCmd == MM_CMD_RECORD)
      {
        pme->m_pOwner->m_playState = MP_PLAYSTATE_RECORDING;
        EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      }

     /*
       * This is a workaround. Since cmx audio files have not
       * implemented the MM_PARM_POS, we need to wait for the
       * media to start playing and seek to the resume play
       * position. Ideally this code should reside in the
       * EVT_MEDIAPLAYER_SEEK section.
       */
      if (pme->m_pOwner->m_resumePlayPos > 0)
      {
          int nRet = SUCCESS;
          nRet = IMEDIA_Seek(pme->m_pMedia, MM_SEEK_CURRENT, pme->m_pOwner->m_resumePlayPos);
          DBGPRINTF_MED("MP: MM_STATUS_START IMEDIA_Seek returns %d = %s", nRet, MP_ErrorStateToString(nRet));

        /*
         * reset the resume play position.
         */
        pme->m_pOwner->m_resumePlayPos = 0;
      }
    }
      break;

    case MM_STATUS_MEDIA_SPEC:
      /*
       * pCmdNotify->pCmdData may point to media specs structure
       * defined in AEEMediaFormats.h. Save this info.
       */
      if (pCmdNotify->pCmdData)
      {
        AEECLSID  ClsId;
        int       result = IMEDIA_GetClassID(pme->m_pMedia, &ClsId);

        if(result == SUCCESS && ClsId == AEECLSID_MEDIAMP3)
        {
          AEEMediaMP3Spec* pSpecMP3;
          AEEMediaMP3Spec* pSpec;
          pSpec = (AEEMediaMP3Spec*)MALLOC(sizeof(AEEMediaMP3Spec));
          if(pSpec)
          {
            pSpecMP3 = (AEEMediaMP3Spec *)pCmdNotify->pCmdData;
            // save media spec info
            (void)MEMMOVE(pSpec, pSpecMP3, sizeof(AEEMediaMP3Spec));
            pme->m_bGotMediaInfo = TRUE;
            if(pme->m_pOwner->m_bShowAudioSpec)
            {
              pme->m_bDisplayInfoPending = TRUE;
            }

            FREEIF(pme->m_pAudioSpec);
            pme->m_pAudioSpec = (uint32*)pSpec;
          }
        }

#ifdef FEATURE_APP_MPEG4
        if ((result == SUCCESS) && (ClsId == AEECLSID_MEDIAMPEG4) &&
            (pCmdNotify->dwSize == sizeof(AEEMediaMPEG4Spec)))
        {                              // Verify that it's an MP4 spec
          AEEMediaMPEG4Spec *pSpec = (AEEMediaMPEG4Spec *)pCmdNotify->pCmdData;

          /*
           * this copying assumes that there is no pointers in
           * info structure
           */
          (void)MEMMOVE(&pme->m_mediaInfo, pSpec, sizeof(AEEMediaMPEG4Spec));
          pme->m_dwDuration = pme->m_mediaInfo.dwDuration;      // milliseconds
          pme->m_bGotMediaInfo = TRUE;
        }
#ifdef FEATURE_APP_REAL
        else if ((result == SUCCESS) && (ClsId == AEECLSID_MEDIAMPEG4) &&
                 (pCmdNotify->dwSize == sizeof(AEEMediaMPEG4Spec)))
        {                              // Verify that it's a Real Media
                                       // spec
          AEEMediaRealSpec *pSpec = (AEEMediaRealSpec *)pCmdNotify->pCmdData;

          /*
           * this copying assumes that there is no pointers in
           * info structure
           */
          (void)MEMMOVE(&pme->m_mediaRealInfo, pSpec, sizeof(AEEMediaRealSpec));
          pme->m_dwDuration = pme->m_mediaRealInfo.dwDuration;      // milliseconds
          pme->m_bGotMediaInfo = TRUE;
        }
#endif  // FEATURE_APP_REAL
#endif  // FEATURE_APP_MPEG4

      }
      if (pme->m_bDisplayInfoPending)
      {
        if (pme->m_bGotMediaInfo)
          (void)CPlayerWin_Info(pme, TRUE);
        pme->m_bDisplayInfoPending = FALSE;
        return;
      }
      break;

    case MM_STATUS_FRAME:
    {
      AEEBitmapInfo bi;
      int       status;
      IBitmap  *pFrame = NULL;

      /*
       * IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame)after
       * you're done with pFrame.
       */
      status = IMEDIA_GetFrame(pme->m_pMedia, &pFrame);
      if (status != SUCCESS || pFrame == NULL)
      {
        DBGPRINTF_ERROR("MP: Failed IMEDIA_GetFrame, status=%d, pFrame=0x%x", status, pFrame);
        if (pFrame) 
        {
          MP_RELEASEBITMAP(pFrame);
        }
        return;
      }
      /*
        * Get the bitmap info
        */
      (void)IBITMAP_GetInfo(pFrame, &bi, sizeof(bi));
      DBGPRINTF_MED("MP: pFrame is %d x %d x %d", bi.cx, bi.cy, bi.nDepth);

#ifdef FEATURE_MMOVERLAY
      /*
       * If overlays are enabled, use IMMOVERLAY methods
       * to update screen.
       */
      if (pme->m_pOwner->m_bOverlays)
      {
        uint32    nRet;
        AEERect   srcRect;

        if (pme->m_pIMMOverlayImage)
        {
          nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_BITMAP, (uint32)pFrame, 0);
          if (SUCCESS != nRet)
          {
            DBGPRINTF_HIGH("MP: Error on IMMOVERLAY_SetParam OV_PARM_BITMAP, %d, %s", status, MP_ErrorStateToString(status));
          }
          srcRect.x = srcRect.y = 0;
          srcRect.dx = bi.cx;
          srcRect.dy = bi.cy;
          nRet = IMMOVERLAY_SetParam(pme->m_pIMMOverlayImage, OV_PARM_SRC_REGION, (uint32)&srcRect, 0);
          if (SUCCESS != nRet)
          {
            DBGPRINTF_HIGH("MP: Error on IMMOVERLAY_SetParam OV_PARM_SRC_REGION, %d, %s", nRet, MP_ErrorStateToString(nRet));
          }
          nRet = IMMOVERLAY_UpdateAsync(pme->m_pIMMOverlayImage, NULL, 0);
          if (nRet == MP_ERR_IMMOVERLAY)
          {
            DBGPRINTF_ERROR("MP: IMMOVERLAY_UpdateAsync Failed");
          }
        }
      }
      else 
#endif // FEATURE_MMOVERLAY
      {
        AEERect       rcPanel;
        uint16        x, y;
        uint16        dx, dy;

        // Currently we do not enable frame callback for CMX/.pmd files
        // If we do, we may need to move PrevHeight to be outside of MPEG4 protection
#ifdef FEATURE_APP_MPEG4
        if (pme->m_mp4PrevHeight != bi.cy || pme->m_mp4PrevWidth != bi.cx)
        {
          /*
           * Frame dimensions have changed, clear the screen
           */
          IDISPLAY_EraseRgn(pme->m_pIDisplay, pme->m_rectImage.x,
                            pme->m_rectImage.y, pme->m_rectImage.dx,
                            pme->m_rectImage.dy);

          pme->m_mp4PrevHeight = bi.cy;
          pme->m_mp4PrevWidth = bi.cx;
        }
#endif // FEATURE_APP_MPEG4
        dx = MIN(pme->m_rectImage.dx, bi.cx);
        dy = MIN(pme->m_rectImage.dy, bi.cy);

        x = (uint16) ((pme->m_rectImage.dx - dx) / 2) + pme->m_rectImage.x;
        y = (uint16) ((pme->m_rectImage.dy - dy) / 2) + pme->m_rectImage.y;

#ifdef FEATURE_MP_EQ_UI
        // if setting win is shown, resize the display rect
        if(MPSetupWinIsShow(pme->m_pOwner->m_pSetting))
        {
          if(MPSetupWinGetSettingPanelRect(pme->m_pOwner->m_pSetting->m_pSetupWin, &rcPanel))
          {
            dy = (dy - rcPanel.dy);
          }
        }
#endif
        /*
         * Display the frame centered on the screen
         */
        IDISPLAY_BitBlt(pme->m_pIDisplay, x, y, dx, dy, pFrame, 0, 0, AEE_RO_COPY);
        IDISPLAY_Update(pme->m_pIDisplay);
      }
      MP_RELEASEBITMAP(pFrame);
    }
    break;

#if defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) && defined(MM_STATUS_DOWNLOAD_TICK_UPDATE)
    case MM_STATUS_DOWNLOAD_TICK_UPDATE:
      DBGPRINTF_MED("MP: MM_STATUS_DOWNLOAD_TICK_UPDATE");
      dwDownldPos = (uint32)pCmdNotify->pCmdData;
      break;
#endif

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
#ifdef MM_STATUS_DOWNLOAD_PERCENTAGE
    case MM_STATUS_DOWNLOAD_PERCENTAGE:
      DBGPRINTF_MED("MP: MM_STATUS_DOWNLOAD_PERCENTAGE");
      dwDownldPct = (uint32)pCmdNotify->pCmdData;
      break;
#endif // MM_STATUS_DOWNLOAD_PERCENTAGE
#endif // FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

    case MM_STATUS_TICK_UPDATE:       // Typically, one-second update
      { 
        uint32  playPos;

        if (SUCCESS == CPlayerWin_GetMediaPosition(pme, &playPos))
        {
          dwPlayPos = playPos;
        }
        else
        {
          dwPlayPos++;
        }
      }
      // If no total time, increase by 20!
      if ((pme->m_dwDuration / 1000) == 0)
        pme->m_wPct = (pme->m_wPct + 20) % 100;
      break;


#if defined(FEATURE_APP_MPEG4) || defined(FEATURE_APP_REAL)
    case MM_MP4_STATUS_BUFFER_UPDATE:
      CPlayerWin_ClearMediaBusy(pme, MEDIA_BUSY_RESUME);
      pCmdNotify->nSubCmd = MM_MP4_PARM_BUFFER_UPDATE;
      if (pCmdNotify->pCmdData)
      {
        prerollPercentage = *((uint32 *)pCmdNotify->pCmdData);
      }

      // a Buffer_Update can can come in before we finish process the skip/seek finished
      // callback.  But by the time we get here we will have already called
      // seek/skip. so don't try again.
#if defined(FEATURE_QTV_PLAYLIST)
      {
        boolean havePlaylist = FALSE;
#if defined(FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST)
        if (pme->m_mediaInfo.streamtype == MM_MEDIA_PV_SERVER_SIDE_PLAYLIST)
        {
          DBGPRINTF_MED("MP: stream type is MM_MEDIA_PV_SERVER_SIDE_PLAYLIST");
          havePlaylist = TRUE;
        }
#endif /* FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST */
#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
        if (pme->m_mediaInfo.streamtype == MM_MEDIA_WM_SERVER_SIDE_PLAYLIST)
        {
          DBGPRINTF_MED("MP: stream type is MM_MEDIA_WM_SERVER_SIDE_PLAYLIST");
          havePlaylist = TRUE;
        }
#endif /* FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST */
        if ((!havePlaylist) || (!pme->m_skipPending && !pme->m_seekPending))
        {
          DBGPRINTF_MED("MP: calling CheckForPending Reposition");
          (void)CheckForPendingReposition(pme);
        }
        else
        {
          if (pme->m_reskip || pme->m_reseek)
          {
            DBGPRINTF_MED("MP: A skip or seek is pending, so we are not trying to skip(%d) or seek(%d)", pme->m_skipOffset, pme->m_pendingSeekOffset);
          }
        }
      }
#else
      // If we don't need to worry about playlists, just see if we need to re-position
      DBGPRINTF_MED("MP: calling CheckForPending Reposition");
      (void)CheckForPendingReposition(pme);
#endif // FEATURE_QTV_PLAYLIST
      /*
       * update progress bar
       */
      {
        pme->m_dwPlayPos = dwPlayPos;
        CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, nTextID);
      }
      pme->m_pOwner->m_playState = MP_PLAYSTATE_BUFFERING;
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      break;

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    case MM_MP4_STATUS_PS_BUFFER_UPDATE:
      pCmdNotify->nSubCmd = MM_MP4_PARM_PS_BUFFER_UPDATE;

      if (pCmdNotify->pCmdData)
      {
        pseudostreamDownloadRate = *((uint32 *)pCmdNotify->pCmdData);
      }
      pme->m_pOwner->m_playState = MP_PLAYSTATE_BUFFERING;
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      break;

    case MM_MP4_STATUS_PSEUDO_PAUSE:
    case MM_MP4_STATUS_HTTP_PAUSE:
      break;

#endif     // FEATURE_QTV_PSEUDO_STREAM ||
           // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif     // FEATURE_APP_MPEG4

    case MM_STATUS_SEEK:
      event_report(EVENT_MEDIA_PLAYER_SEEK);
      nTextID = IDS_SEEK;
      if (pCmdNotify->dwSize)
      {
        dwPlayPos = MPClipTimeInSeconds((uint32)pCmdNotify->pCmdData);
      }
      if (CPlayerWin_IsPlaying(pme))
        pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
      else if (CPlayerWin_IsPause(pme))
        pme->m_pOwner->m_playState = MP_PLAYSTATE_PAUSED;
      else {
        DBGPRINTF_MED("MP: m_playState is neither play nor pause on MM_STATUS_SEEK");
      }
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      CPlayerWin_ClearMediaBusy(pme, MEDIA_BUSY_SEEK);
      pme->m_skipPending = FALSE;
      break;

    case MM_STATUS_PAUSE:
      nTextID = IDS_PAUSE;
      if (pCmdNotify->dwSize)
      {
        dwPlayPos = MPClipTimeInSeconds((uint32)pCmdNotify->pCmdData);
      }
      pme->m_pOwner->m_playState = MP_PLAYSTATE_PAUSED;
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      CPlayerWin_ClearMediaBusy(pme, MEDIA_BUSY_PAUSE);

#ifdef MP_FEATURE_FAST_PLAYBACK
      if(pme->m_bChangeReplaySpeed)
      {
        int i;
        int size;
        int ret;
        int newSpeed = pme->m_ReplaySpeed;

        // get next speed
        size = sizeof(REPLAY_SPEED) / sizeof(uint32);
        for(i=0; i < size; i++)
        {
          if(REPLAY_SPEED[i] == pme->m_ReplaySpeed)
          {
            if(i < (size -1))
            {
              newSpeed = REPLAY_SPEED[i+1];
            }
            else
            {
              newSpeed = REPLAY_SPEED[0];
            }
          }
        }

        // set new speed
        if(SUCCESS == IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_PB_SPEED, newSpeed, 0))
        {
          pme->m_ReplaySpeed = newSpeed;
          DBGPRINTF_HIGH("MP: Set replay speed %d.", newSpeed);
          CPlayerWin_UpdateProgCtl(pme, MM_CMD_PLAY, MM_MP4_PARM_PB_SPEED, 0);
        }
        else
        {
          DBGPRINTF_ERROR("MP: failed to set new replay speed, %d.", newSpeed);
        }

        // resume on play
        if(SUCCESS != (ret = IMEDIA_Resume(pme->m_pMedia)))
        {
          // Imedia may be busy
          if(ret != EITEMBUSY)
          {
            // display error
            CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
            DBGPRINTF_ERROR("MP: failed to Resume, returns %d = %s.",
                            ret, MP_ErrorStateToString(ret));
          }
          else
          {
            DBGPRINTF_HIGH("MP: failed to Resume, IMedia is busy, %d = EITEMBUSY.", ret);
          }
        }
        else
        {
          DBGPRINTF_MED("MP: IMEDIA_Resume returns %d = %s", ret, MP_ErrorStateToString(ret));
          pme->m_pOwner->m_bPlayAfterResume = FALSE;
        }

        pme->m_bChangeReplaySpeed = FALSE;
      }
#endif //MP_FEATURE_FAST_PLAYBACK
      if((pme->m_pOwner->m_bPlayAfterResume == TRUE) && (pme->m_pOwner->m_bSuspend != TRUE))
      {
          DBGPRINTF_MED("MP: MM_STATUS_PAUSE bPlayAfterResume TRUE, Posting IDM_PM_PLAY event");
          (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
      }

      break;

    case MM_STATUS_RESUME:
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
        if (pCmdNotify->dwSize)
        {
          dwPlayPos = MPClipTimeInSeconds((uint32)pCmdNotify->pCmdData);
          // Update pme position in case we suspend
          pme->m_dwPlayPos = dwPlayPos;
        }

        if(pme->m_pOwner->m_bResumeInProcessing && pme->m_pOwner->m_bSuspendPending)
        {
          // we have a SUSPEND pending request during the resuming, we need to suspend again
          pme->m_pOwner->m_bResumeInProcessing = FALSE;
          pme->m_pOwner->m_bSuspendPending = FALSE;

          // If playing make sure state is checked and updated so suspend will work correctly.
          if (CPlayerWin_IsPlaying(pme))
          {
            pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
            EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
          }

          DBGPRINTF_MED("MP: replay resumed, SUSPEND pending, setback to suspend mode.");
          CMediaPlayer_ProcessSuspendEvent(pme->m_pOwner);
          return;
        }
        pme->m_pOwner->m_bResumeInProcessing = FALSE;
        pme->m_pOwner->m_bSuspendPending = FALSE;

        // Check to make sure that we don't have any more seeking to do.
        if (!CheckForPendingReposition(pme))
        {
          DBGPRINTF_MED("MP: Setting m_pendingSeekOffset from %d to 0", pme->m_pendingSeekOffset);
          pme->m_seekPending = pme->m_pendingSeekOffset = pme->m_nextClipOffsetAdj = 0;
          pme->m_skipPending = FALSE;
          pme->m_skipOffset = 0;
          if (CPlayerWin_IsPlaying(pme))
          {
            pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
            EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
          }
        }
        
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      }
      CPlayerWin_ClearMediaBusy(pme, MEDIA_BUSY_RESUME);
      break;

    case MM_STATUS_ABORT:             // playback aborted
      DBGPRINTF_ERROR("MP: Received MM_STATUS_ABORT, pme = 0x%X, nCmd = %d, nStatus = %d, nSubCmd = %d, dwSize = %d, pCmdData = 0x%X",
                      pme, pCmdNotify->nCmd, pCmdNotify->nStatus, pCmdNotify->nSubCmd, pCmdNotify->dwSize, pCmdNotify->pCmdData );

      pme->m_pOwner->m_playState = MP_PLAYSTATE_ERROR;
      pme->m_pOwner->m_ErrorState = MP_ERRORSTATE_ERROR;
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      dwDownldPos = 0;
#endif
      pme->m_MediaBusy = MEDIA_NOT_BUSY;

      if(pme->m_pOwner->m_bResumeInProcessing && pme->m_pOwner->m_bSuspendPending)
      {
        // we have a SUSPEND pending request during the playing, we need to suspend
        pme->m_pOwner->m_bResumeInProcessing = FALSE;
        pme->m_pOwner->m_bSuspendPending = FALSE;
  
        DBGPRINTF_MED("MP: MM_STATUS_ABORT, SUSPEND pending, set to suspend mode.");
        CMediaPlayer_ProcessSuspendEvent(pme->m_pOwner);
        return;
      }
      pme->m_pOwner->m_bResumeInProcessing = FALSE;
      pme->m_pOwner->m_bSuspendPending = FALSE;

    case MM_STATUS_DONE:              // playback done
    {
      boolean bPlayNextFile = TRUE;
      if(pCmdNotify->nStatus == MM_STATUS_DONE)
      {
        DBGPRINTF_MED("MP: Received MM_STATUS_DONE, pme = 0x%X, nCmd = %d, nStatus = %d, nSubCmd = %d, dwSize = %d, pCmdData = 0x%X",
                       pme, pCmdNotify->nCmd, pCmdNotify->nStatus, pCmdNotify->nSubCmd, pCmdNotify->dwSize, pCmdNotify->pCmdData );
      }

#ifdef MP_FEATURE_ICONTENTHANDLER
      DBGPRINTF_MED("MP: call IMEDIA_Stop to stop IContentPlayer");
      (void) IMEDIA_Stop(pme->m_pMedia);
#endif //MP_FEATURE_ICONTENTHANDLER

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      if ((TRUE == pme->m_pOwner->m_bEnablePSSimulation) ||
          (pme->m_pOwner->m_bIsHTTP == TRUE))
      {
        (void)ISHELL_CancelTimer(pme->m_pOwner->a.m_pIShell, PSTimerResponse, (void *)(pme->m_pOwner));

        /*
         * We don't need the file resources anymore. Free them here
         */
        if (pme->m_pOwner->m_pPseudoStrIFile)
        {
          (void)IFILE_Release(pme->m_pOwner->m_pPseudoStrIFile);
          pme->m_pOwner->m_pPseudoStrIFile = NULL;
        }
      }
      else if (pme->m_pOwner->m_bFileWriteReadTest == TRUE)
      {
        pme->m_pOwner->m_pIFileMgr2 = NULL;
        pme->m_pOwner->m_bFileWriteReadTest = FALSE;
      }
#endif     // FEATURE_QTV_PSEUDO_STREAM ||
           // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif

//#ifdef MP_FEATURE_MBMS
      // SetRect to null is a tmp hack to erase the secreen on an MDP
      // system (EraseRect is not supose to work).
      // With this happing here, if you re-play a video without
      // going back to the file selection menu
      // The video portion will not display
      DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with NULL_RECT");
      IMEDIA_SetRect(pme->m_pMedia, &NULL_RECT, NULL);
//#endif

      /*
       * Never let fullscreen mode persist once a clip has ended.
       * ...well, we think we want it to persist
       */
      //(void)CPlayerWin_FullScreen(pme, FALSE, TRUE);

      event_report(EVENT_MEDIA_PLAYER_STOP);
      nTextID = (pCmdNotify->nStatus == MM_STATUS_DONE) ? IDS_STOP : IDS_ABORT;
      if (pCmdNotify->nStatus == MM_STATUS_DONE)
        pme->m_pOwner->m_playState = MP_PLAYSTATE_STOPPED;
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);

#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)
      pme->m_dwPositionAtStop = dwPlayPos * 1000;
#endif //#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)

      dwPlayPos = 0;
      pme->m_wPct = 0;
      ConnectStatusStringActive = 0;
      DBGPRINTF_MED("MP: Setting m_pendingSeekOffset from %d to 0", pme->m_pendingSeekOffset);
      pme->m_seekPending = pme->m_pendingSeekOffset = pme->m_reseek = 0;
      pme->m_startSeekPosition = 0;
      pme->m_skipPending = FALSE;
      pme->m_skipOffset = 0;
      pme->m_reskip = FALSE;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      dwDownldPos = 0;
#endif

#ifdef FEATURE_KEY_HOLD_SCROLL
      /*
       * If Key repeat is currently turned on, turn it off
       */
      if (pme->m_pOwner->m_KeyStore.m_wParam_Key != 0)
      {
        DBGPRINTF_MED("MP: Cancelling MP_OnKeyRepeatEvent Timer");
        (void)ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)MP_OnKeyRepeatEvent,
                                (void *)&pme->m_pOwner->m_KeyStore);
        pme->m_pOwner->m_KeyStore.m_wParam_Key = 0;
        pme->m_pOwner->m_KeyStore.m_dwParam_Key = 0;
      }
#endif // FEATURE_KEY_HOLD_SCROLL

#ifdef FEATURE_APP_MPEG4
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      if (pme->m_b3GPPTTextOn)
      {
        (void)ISHELL_CancelTimer(pme->m_pIShell,
                                 (PFNNOTIFY)CPlayerWin_3GPPTimedTextExpiredNotify,
                                 pme);
        CPlayerWin_3GPPTText(pme, NULL);
      }
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT
#endif     // FEATURE_APP_MPEG4

      if(pme->m_eWin != MPPW_RECORD)
      {
        CMediaPlayer* pMP = pme->m_pOwner;

        if(pMP->m_nContMode != CONT_MODE_OFF && (pMP->m_bResumeInProcessing || pMP->m_bResumePending) &&
           (pMP->m_bIncallMode == FALSE))
        {
          // we are in suspending/resuming mode, play next file should be disabled.
          if(pCmdNotify->nStatus == MM_STATUS_ABORT)
          {
            //play same file when resuming
            pMP->m_bReplaySameFile = TRUE;
          }
          else
          {
            // when resuming, ReplayNextFile depends on the m_nContMode
            pMP->m_bReplaySameFile = FALSE;
          }
          pMP->m_bPlayNextFileOnResume = TRUE;
          // don't continue on next file
          bPlayNextFile = FALSE;
        }

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        //For pseudo streaming, WebAction_GotResp sends the EVT_CREATEPSEUDOMEDIA event with status set to
        //MM_STATUS_ABORT, in order to abort the media creation if it could not get the first chunk.
        if (bPlayNextFile && pme->m_pOwner->m_nContMode == CONT_MODE_SINGLE_FILE && !pme->m_bStopPressed)
#else
        if (bPlayNextFile && pme->m_pOwner->m_nContMode == CONT_MODE_SINGLE_FILE && !pme->m_bStopPressed &&
            pCmdNotify->nStatus != MM_STATUS_ABORT)
#endif
        {
          DBGPRINTF_HIGH("MP: Continuous mode on, single file");
          DBGPRINTF_HIGH("MP: CPlayerWin_PlayNext(), pme = 0x%X, nCmd = %d, nStatus = %d, nSubCmd = %d",
                         pme, pCmdNotify->nCmd, pCmdNotify->nStatus, pCmdNotify->nSubCmd);

          (void)ISHELL_PostEvent(pMP->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, ID_EVENT_CONTINUOUS, TRUE);

          break;
        }
        else if (bPlayNextFile && pme->m_pOwner->m_nContMode == CONT_MODE_ALL_FILES && !pme->m_bStopPressed)
        {
          int nFileCnt = CMediaPlayer_GetFileCount(pme->m_pOwner);
          // will not play again if there is only one file and MM_STATUS_ABORT received
          // To improve: we need to track all files, stop playing if all files give MM_STATUS_ABORT
          if(!(nFileCnt == 1 && pCmdNotify->nStatus == MM_STATUS_ABORT))
          {
            DBGPRINTF_HIGH("MP: Continuous  mode on, all files.");
            DBGPRINTF_HIGH("MP: pme = 0x%X, nCmd = %d, nStatus = %d, nSubCmd = %d",
                           pme, pCmdNotify->nCmd, pCmdNotify->nStatus, pCmdNotify->nSubCmd);

            (void)ISHELL_PostEvent(pMP->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, ID_EVENT_CONTINUOUS, TRUE);

            break;
          }
          else
          {
            DBGPRINTF_ERROR("MP: Continuous  mode on, abort with file count: %d, pCmdNotify->Status %d.",
                             nFileCnt, pCmdNotify->nStatus);
          }
        }
      }

#ifdef FEATURE_APP_MPEG4
      if (MM_STATUS_ABORT == pCmdNotify->nStatus && !pme->m_bAbortStringActive)
      {
        pCmdNotify->nSubCmd = MM_MP4_PARM_PB_ABORT;
        pme->m_bAbortStringActive = 1;
        CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, 0);
        pme->m_pOwner->m_playState = MP_PLAYSTATE_ERROR;
        EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      }
#endif

      /*
       * If test mode is off, continue normally
       */
      if (pme->m_pPlayerMenu != NULL)
      {
        if (!pme->m_bFullScreen)
          IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_PLAY);
      }

      /*
       * if recording stopped with memory full, display it
       */
      if (pme->m_eWin == MPPW_RECORD && pme->m_bMemoryFull)
      {
        /*
         * reset pos
         */
        pme->m_dwPlayPos = 0;
        /*
         * clear out this flag
         */
        pme->m_bMemoryFull = FALSE;
        pme->m_bAbortStringActive = 1;
        (void)ISHELL_Beep(pme->m_pIShell, BEEP_ERROR, FALSE);
        CPlayerWin_UpdateProgCtl(pme, MM_CMD_RECORD, MM_STATUS_SPACE_ERROR, IDS_MEMORY_FULL);
        DBGPRINTF_HIGH("MP: memory full.");
        return;
      }

      if ((MM_STATUS_DONE == pCmdNotify->nStatus) && !pme->m_bAbortStringActive)
      {
        // Update status display
        CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, nTextID);
        if (pme->m_pOwner->m_bRedrawScreenAtClipEnd)
        {
          /*
           * Redraw screen. But if we just display "Error", then leave as it is
           */
          CMediaPlayer_Redraw(pme->m_pOwner, TRUE);
        }
      }
      break;
    }

    case MM_STATUS_REPOSITIONING:
    {
      if (!(uint32)pCmdNotify->pCmdData)
      {
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_REWIND);
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FF);
      }
      break;
    }

#ifdef FEATURE_APP_MPEG4

    case MM_MP4_STATUS_NO_SYNC_FRAME:
    {
      break;
    }

    case MM_MP4_STATUS_RTSP:
      /*
       * OEMMediaMPEG42PV_SetRTSPStatusNotify(true)was called
       * earlier, and an rtsp response of other than 200 has been
       * received.  It is passed here for your processing.  You
       * get the status code and a corresponding string.
       * Processing is just stubbed out here.  Fill in with what
       * you want to do with unhandled lines.
       */
      /*
       * int32 statusCode = ((AEERTSPStatus *)pCmdNotify->pCmdData)->statusCode;
       * *statusString = ((AEERTSPStatus *)pCmdNotify->pCmdData)->statusString;
       * (statusCode != 200 && statusString != NULL)
       */
      break;

      // unhandled RTSP and SDP lines
    case MM_MP4_STATUS_UNHANDLED_SDP:
    case MM_MP4_STATUS_UNHANDLED_RTSP:
      /*
       * OEMMediaMPEG42PV_SetUnhandledRTSPNotify(true)or
       * OEMMediaMPEG42PV_SetUnhandledSDPNotify(true)was called
       * earlier, and an unhandled sdp or rtsp header line has
       * been encountered.  It is passed here for your processing.
       * Processing is just stubbed out here.  Fill in with what
       * you want to do with unhandled lines.
       */
      /*
       * char *unhandledLine = ((AEEUnhandledSDP_RTSP_Status *)pCmdNotify->pCmdData)->statusString;
       */
      break;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    case MM_MP4_STATUS_3GPP_TTEXT:
    {
      AEE3GPPTimedText *pTTxt = (AEE3GPPTimedText *) (pCmdNotify->pCmdData);

      if (pme->m_bFullScreen)
      {
        // If we are in FullScreen mode, ignore TimedText
        break;
      }

      if (pTTxt && pTTxt->pszText)
      {
        CPlayerWin_3GPPTText(pme, pTTxt->pszText);
        (void)ISHELL_SetTimer(pme->m_pIShell, pTTxt->dwDuration,
                              (PFNNOTIFY)CPlayerWin_3GPPTimedTextExpiredNotify, pme);
      }
      else
      {
        (void)ISHELL_CancelTimer(pme->m_pIShell,
                                 (PFNNOTIFY)CPlayerWin_3GPPTimedTextExpiredNotify,
                                 pme);
        CPlayerWin_3GPPTText(pme, NULL);
      }
      break;
    }
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT


#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
    case MM_MP4_STATUS_TELOP_TEXT:
    {
      char     *pTxtStr = NULL;
      AECHAR   *pWTxtStr = NULL;
      int       i,
                len;
      AEETelopTextSubString *pTxtSubStr;
      AEETelopTextElement *pTxtElement =
        (AEETelopTextElement *) (pCmdNotify->pCmdData);

      if (pme->m_bFullScreen)
      {
        // If we are in FullScreen mode, ignore TimedText
        break;
      }

      pTxtStr = (char *)MALLOC(pTxtElement->dwTelopSize);
      if (pTxtStr)
      {
        (void)MEMSET(pTxtStr, 0, pTxtElement->dwTelopSize);
        for (i = 0; i < pTxtElement->dwNumberOfSubStrings; i++)
        {
          (void)IMEDIA_GetMediaParm(pme->m_pMedia, MM_MP4_PARM_TELOP_SUB_STR,
                              (int32 *) & pTxtSubStr, (int32 *) & i);
          if (pTxtSubStr && pTxtSubStr->pTextSubString)
            (void)STRLCAT(pTxtStr, pTxtSubStr->pTextSubString, (pTxtElement->dwTelopSize));
        }
        len = STRLEN(pTxtStr);
        if (len)
        {
          pWTxtStr = (AECHAR *)MALLOC((len + 1) * sizeof(AECHAR));
          if (pWTxtStr)
          {
            (void)MEMSET(pWTxtStr, 0, (len + 1) * sizeof(AECHAR));
            (void)UTF8TOWSTR((byte *)pTxtStr, len, pWTxtStr,
                       (len + 1) * sizeof(AECHAR));
            CPlayerWin_3GPPTText(pme, pWTxtStr);
            (void)ISHELL_SetTimer(pme->m_pIShell, pTxtElement->dwDuration,
                                  (PFNNOTIFY)CPlayerWin_3GPPTimedTextExpiredNotify,
                                  pme);
            FREE(pWTxtStr);
          }
        }
        else
        {
          (void)ISHELL_CancelTimer(pme->m_pIShell,
                                   (PFNNOTIFY)CPlayerWin_3GPPTimedTextExpiredNotify,
                                   pme);
          CPlayerWin_3GPPTText(pme, NULL);
        }
        FREE(pTxtStr);
      }
      break;
    }
#endif     // FEATURE_MP4_KDDI_TELOP_TEXT

    case MM_MP4_STATUS_RESTARTING_VIDEO:
      break;

    case MM_MP4_STATUS_RESTARTING_AUDIO:
      break;

    case MM_MP4_STATUS_RESTARTING_TEXT:
      break;

    // We get this at the start of playing or after a repositioning is
    // finished (after buffering)
    case MM_MP4_STATUS_PLAYING:
      DBGPRINTF_MED("MP: calling CheckForPending Reposition");
      if (!CheckForPendingReposition(pme))
      {
        pme->m_nextClipOffsetAdj = 0;
        DBGPRINTF_MED("MP: Setting m_pendingSeekOffset from %d to 0", pme->m_pendingSeekOffset);
        pme->m_seekPending = pme->m_pendingSeekOffset = pme->m_reseek = 0;
      }
      pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
      EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
      break;

#if defined(FEATURE_QTV_STREAM_RECORD) && defined(MP_FEATURE_VIDEO)
    case MM_MP4_STATUS_FILE_SYS_LIMIT:
      DBGPRINTF_HIGH("MP: File system limit approaching. Close the file");

      /*
       * Stop stream recording
       */
      CPlayerWin_StopStreamRecord(pme);
      break;

    case MM_MP4_STATUS_RECORD_COMPLETED:
      /*
       * Clip recording is completed
       */
      DBGPRINTF_MED("MP: Record completed");
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION
      break;

#ifdef MM_MP4_STATUS_RECORDED_CLIP_DURATION
    case MM_MP4_STATUS_RECORDED_CLIP_DURATION:
      if (pCmdNotify->pCmdData)
      {
        uint32    record_duration;

        record_duration = *((uint32 *)pCmdNotify->pCmdData);
        DBGPRINTF_MED("MP: Recorded duration: %ld", record_duration);
      }
      break;
#endif     // MM_MP4_STATUS_RECORDED_CLIP_DURATION
#endif     // FEATURE_QTV_STREAM_RECORD && MP_FEATURE_VIDEO

#ifdef FEATURE_QTV_PLAYLIST
    case MM_MP4_STATUS_PLAYLIST_SKIP_FAILED:
      DBGPRINTF_HIGH("MP:  PLAYLIST_SKIP_FAILED");
      pme->m_skipPending = FALSE;
      pme->m_skipOffset = 0;
      pme->m_reskip = FALSE;

#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
      if (pme->m_mediaInfo.streamtype == MM_MEDIA_WM_SERVER_SIDE_PLAYLIST)
      {
        DBGPRINTF_MED("MP: Stopping after PLAYLIST_SKIP_FAILED");
        (void) IMEDIA_Stop(pme->m_pMedia);
      }
#endif /* FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST */
      break;

    case MM_MP4_STATUS_PLAYLIST_SKIP_COMPLETE:
      DBGPRINTF_HIGH("MP:  PLAYLIST_SKIP_COMPLETE");
      if (*((uint32 *)pCmdNotify->pCmdData) != pme->m_currentClipIndex &&
          !pme->m_skipPending)
      {
        pme->m_skipSuccess = TRUE;
      }

      pme->m_seekPending = FALSE;
      pme->m_skipPending = FALSE;
      pme->m_currentClipIndex = *((uint32 *)pCmdNotify->pCmdData);
      DBGPRINTF_MED("MP: calling CheckForPending Reposition");
      (void)CheckForPendingReposition(pme);
      break;

    case MM_MP4_STATUS_PLAYLIST_CLIP_TRANSITION:
      DBGPRINTF_MED("MP: Moving to next clip");
      // Clear out the display so that if the next clip is audio
      // the last frame from the previous video does not hang around
      IDISPLAY_EraseRect(pme->m_pIDisplay, &pme->m_rectImage);
      pme->m_skipSuccess = FALSE;
      break;
#endif /* FEATURE_QTV_PLAYLIST */

#endif     // FEATURE_APP_MPEG4

    }                                  // end of switch(nStatus)

#ifdef FEATURE_APP_MPEG4

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    {
      boolean bEnableFF = FALSE;

#ifdef  MM_STATUS_DOWNLOAD_PERCENTAGE
      // If the MM_STATUS_DOWNLOAD_PERCENTAGE callback is available, fast track and
      // progressive downloads report percentage of download completed
      if ((dwDownldPct != pme->m_dwDownldPct) || (dwDownldPos != pme->m_dwDownldPos))
      {
        pme->m_dwDownldPos = dwDownldPos;
        pme->m_dwDownldPct = dwDownldPct;
        DBGPRINTF_MED("MP: Download positon = %d, Download percent = %d", dwDownldPos, dwDownldPct);
        if ((pme->m_dwDownldPct >= 100) || (pme->m_dwDownldPos >= pme->m_dwDuration))
        {
          CallUpdateProgCtl = TRUE;
          if ((pme->m_mediaInfo.eFormatType == MM_PVX_FORMAT_TYPE) ||
            (pme->m_mediaInfo.eFormatType == MM_PD_FORMAT_TYPE))
          {
            bEnableFF = TRUE;
          }
        }
      }
#else
      if (dwDownldPos != pme->m_dwDownldPos)
      {
        pme->m_dwDownldPos = dwDownldPos;
        DBGPRINTF_MED("MP: Download position = %d", dwDownldPos);
        if (pme->m_dwDownldPos >= pme->m_dwDuration)
        {
          CallUpdateProgCtl = TRUE;
          if (pme->m_mediaInfo.eFormatType == MM_PVX_FORMAT_TYPE)
          {
            bEnableFF = TRUE;
          }
        }
      }
#endif //MM_STATUS_DOWNLOAD_PERCENTAGE
      if (bEnableFF)
      {
        CtlAddItem pai;

        // If We don't already have the FF or Rewind button on our menu, add it.
        // We do this now since we have the whole clip, so the user
        // is now allowed to fast-forward
        if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_FF, &pai) ||
            !IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_REWIND, &pai))
        {
          CPlayerWin_RestoreDefaultMenu(pme);
          CPlayerWin_UpdateMenu(pme, IDM_PM_STOP);
        }
      }
    }
#endif //FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

   /*
    * Do not update position on Status Frame callbacks or on tick updates if in Hi Perf Mode
    */
   if (pme && ((pCmdNotify->nStatus != MM_MP4_STATUS_BUFFER_UPDATE) && 
                (pCmdNotify->nStatus != MM_STATUS_FRAME) &&
                !(pme->m_pOwner->m_bHighPerformanceMode && (pCmdNotify->nStatus == MM_STATUS_TICK_UPDATE)) &&
               ((dwPlayPos != pme->m_dwPlayPos) || (CPlayerWin_IsPlaying(pme)))))
    {
      pme->m_dwPlayPos = dwPlayPos;
      CallUpdateProgCtl = TRUE;
    }
    if (CallUpdateProgCtl)
    {
      CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, nTextID);
    }
#endif     // FEATURE_APP_MPEG4

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    else if (MM_MP4_STATUS_PS_BUFFER_UPDATE == pCmdNotify->nStatus &&
             ((TRUE == pme->m_pOwner->m_bEnablePSSimulation) || (pme->m_pOwner->m_bIsHTTP == TRUE)))
    {
      CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, nTextID);
    }
#endif     // FEATURE_QTV_PSEUDO_STREAM ||
           // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

  }                                    // MM_CMD_PLAY
  else if (pCmdNotify->nCmd == MM_CMD_GETTOTALTIME)
  {
    if (pCmdNotify->nStatus == MM_STATUS_DONE)
    {
      pme->m_dwDuration = ((uint32)pCmdNotify->pCmdData);
      CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, nTextID);
      if (pme->m_dwDuration == 0) 
      {
        // Try again, may be in Connecting state
        // TBD: Is this really a good idea?  May there be a problem with getting stuck
        // in Connecting state and never getting a real total time value?
        int nRet;

        DBGPRINTF_MED("MP: MM_CMD_GETTOTALTIME, m_dwDuration = 0");

        nRet = CPlayerWin_GetTotalTimeRetry(pme);
        if (nRet != SUCCESS)
        {
          DBGPRINTF_HIGH("MP: GetTotalTime Retry Failed");
        }
      }
    }
    if (pme->m_bNoTextOrHeader)
    {
      CPlayerWin_EraseHeaderAndText(pme);
    }
    if (pme->m_bPlayPending)
    {
      int nRet;
      pme->m_bPlayPending = FALSE;

      if(pme->m_pMedia1)
      {
        nRet = IMEDIA_Play(pme->m_pMedia1);
        if(nRet != SUCCESS)
        {
          if(nRet == EITEMBUSY)
          {
            DBGPRINTF_HIGH("MP: failed on IMEDIA_Play for dl, IMedia is busy,  returns %d = %s.",
                              nRet, MP_ErrorStateToString(nRet));
          }
          else
          {
             DBGPRINTF_ERROR("MP: failed on IMEDIA_Play for dl, returns %d = %s.",
                              nRet, MP_ErrorStateToString(nRet));
             CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
          }
        }
        else
        {
          DBGPRINTF_MED("MP: IMEDIA_Play returns %d = %s", nRet, MP_ErrorStateToString(nRet));
        }
      }

      if (AEE_SUCCESS == (nRet = IMEDIA_Play(pme->m_pMedia)))
      {
        DBGPRINTF_MED("MP: IMEDIA_Play returns %d = %s", nRet, MP_ErrorStateToString(nRet));
        pme->m_pOwner->m_bPlayAfterResume = FALSE;
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)  && \
    defined(FEATURE_QTV_DIAG_CMD)

        if ((bRTSPstreamRecord))
#if 0
          &&                           /* Stream record flag is set to
                                        * TRUE */
            (pme->m_mediaInfo.streamtype == MM_MEDIA_LIVE_STREAMING ||
             pme->m_mediaInfo.streamtype == MM_MEDIA_STREAMING))
#endif
        {
          CPlayerWin_StartStreamRecord(pme, TRUE);
          (void)STRLCPY(recFileName, "", sizeof(recFileName));
        }
#endif
      }
      else
      {
        if(nRet == EITEMBUSY)
        {
          DBGPRINTF_HIGH("MP: failed on IMEDIA_Play, IMedia is busy,  returns %d = %s.",
                            nRet, MP_ErrorStateToString(nRet));
        }
        else
        {
           DBGPRINTF_ERROR("MP: failed on IMEDIA_Play, returns %d = %s.",
                            nRet, MP_ErrorStateToString(nRet));
           CPlayerWin_UpdateProgCtl(pme, MP_EVENT, MP_EVENT_ERROR, 0);
        }
      }
      CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, nTextID);
    }
  }                                    // MM_CMD_GETTOTALTIME
  else if (pCmdNotify->nCmd == MM_CMD_SETMEDIAPARM)
  {
    if (pCmdNotify->nSubCmd == MM_PARM_VOLUME &&
        pCmdNotify->nStatus == MM_STATUS_DONE)
    {
      int nRet;
      nRet = IMEDIA_GetMediaParm(pme->m_pMedia, MM_PARM_VOLUME, NULL, NULL);
      DBGPRINTF_MED("MP: Retrieve VOLUME, nRet = %d[%s]", nRet, MP_ErrorStateToString(nRet));
      return;
    }
    else if (pCmdNotify->nSubCmd == MM_PARM_PAN && pCmdNotify->nStatus == MM_STATUS_DONE)
    {
      int nRet;
      int32 value;
      nRet = IMEDIA_GetMediaParm(pme->m_pMedia, MM_PARM_PAN, &value, NULL);
	    if (SUCCESS == nRet)
	    {
	      ((CMediaPlayer *)pme->m_pOwner)->m_nAudioPanValue = value;
	    }
      DBGPRINTF_MED("MP: Retrieve PAN, nRet = %d[%s]", nRet, MP_ErrorStateToString(nRet));
      return;
    }

#if defined(FEATURE_APP_MPEG4) && defined(MP_FEATURE_VIDEO)
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
    else if (pCmdNotify->nSubCmd == MM_MP4_PARM_ROTATION &&
             pCmdNotify->nStatus == MM_STATUS_DONE)
      /*
       * Now that the rotation was done successfully, we can update
       * internal orientation.
       * Note! This assumes that a clockwise rotation was performed!
       */
    {
      pme->m_Rotation = MP4_GetNextRotation(pme->m_Rotation, IDM_PM_CW_ROTATE);
      return;
    }
    else if (pCmdNotify->nSubCmd == MM_MP4_PARM_SCALING)
    {
      /*
       * Update the zoom state if the scaling completed successfully.
       */
      if ((pCmdNotify->nStatus == MM_STATUS_DONE) &&
          (pme->m_pendingZoomDirection != 0))
      {
        CPlayerWin_UpdateZoomScale(pme);
      }
      /*
       * Reset regardless of whether or not the zoom completed successfully.
       */
      pme->m_pendingZoomDirection = 0;
      return;
    }
#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_MDP_TRANSFORMATIONS

#endif     // FEATURE_APP_MPEG4

  }                                    // MM_CMD_SETMEDIAPARM
  else if (pCmdNotify->nCmd == MM_CMD_GETMEDIAPARM)     // IMEDIA_GetMediaParm()
                                                        // events
  {
    if (pCmdNotify->nSubCmd == MM_PARM_VOLUME &&
        pCmdNotify->nStatus == MM_STATUS_DONE)
    {
      pme->m_wVolume = (uint16) (uint32)pCmdNotify->pCmdData;
      DBGPRINTF_MED("MP: Volume set, vol = %d", pme->m_wVolume);

      /*
       * save volume level
       */
      ((CMediaPlayer *)pme->m_pOwner)->m_wVolumeLevel = pme->m_wVolume / pme->m_pOwner->m_VolumeStep;

      CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, IDS_VOLUME);
    }
#if defined FEATURE_APP_MPEG4
    else if (pCmdNotify->nSubCmd == MM_MP4_PARM_MEDIA_SPEC &&
             pCmdNotify->nStatus == MM_STATUS_DONE)
    {
      if (pCmdNotify->pCmdData != NULL)
      {
        AEECLSID  ClsId;
        int       result = IMEDIA_GetClassID(pme->m_pMedia, &ClsId);

        if ((result == SUCCESS) && (ClsId == AEECLSID_MEDIAMPEG4) &&
            (pCmdNotify->dwSize == sizeof(AEEMediaMPEG4Spec)))
        {                              // Verify that it's an MP4 spec
          AEEMediaMPEG4Spec *pSpec = (AEEMediaMPEG4Spec *)pCmdNotify->pCmdData;

          /*
           * this copying assumes that there is no pointers in
           * info structure
           */
          (void)MEMMOVE(&pme->m_mediaInfo, pSpec, sizeof(AEEMediaMPEG4Spec));
          pme->m_dwDuration = pme->m_mediaInfo.dwDuration;      // milliseconds

#ifdef FEATURE_QTV_PLAYLIST
          DBGPRINTF_MED("MP: MM_MP4_PARM_MEDIA_SPEC type = %d",
            pSpec->mpeg4Type);
          /* If the clip has no supported codecs, go to the next clip. */
          if (pSpec->mpeg4Type == MM_MPEG4_INVALID)
          {
            switch (pSpec->streamtype)
            {
#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
              case MM_MEDIA_WM_SERVER_SIDE_PLAYLIST:
              {
#if MIN_BREW_VERSION(3, 0)
                AEEMediaSeek skipReference = MM_SEEK_MODE_CHAPTER;
#else
                AEEMediaSeek skipReference = MM_SEEK_ACCESS_POINT;
#endif /* MIN_BREW_VERSION(3, 0) */

                int nRet = 0;

                DBGPRINTF_MED("MP: MM_MP4_PARM_MEDIA_SPEC skipping, pending = %d",
                  pme->m_skipPending);

                /* Performing a skip overrides any seeks waiting to be done. */
                if (pme->m_reseek)
                {
                  DBGPRINTF_MED("MP: Setting m_pendingSeekOffset from %d to 0", pme->m_pendingSeekOffset);
                  pme->m_pendingSeekOffset = 0;
                  pme->m_reseek = FALSE;
                }

                /* If waiting for prior skip to complete, don't do anything. */
                if (!pme->m_skipPending)
                {
                  // This skip will be pending until we get a skip complete
                  // message
                  if (CPlayerWin_IsPlaying(pme) || CPlayerWin_IsPause(pme))
                  {
                    pme->m_skipPending = TRUE;
                  }

                  nRet = IMEDIA_Seek(pme->m_pMedia, skipReference | MM_SEEK_CURRENT, 1);
                  DBGPRINTF_MED("MP: IMEDIA_Seek returns %d = %s", nRet,
                    MP_ErrorStateToString(nRet));

                  if ((nRet != SUCCESS) && (nRet != EITEMBUSY))
                  {
                    pme->m_skipPending = FALSE;
                    pme->m_skipOffset = 0;
                    DBGPRINTF_ERROR("MP: IMEDIA_Seek failed!");
                  }
                }
              }
              break;
#endif /* FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST */
#if defined(FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST)
              case MM_MEDIA_PV_SERVER_SIDE_PLAYLIST:
#endif //FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST
              default:
              break;
            } // switch (streamType)
          } /* mpeg4Type == INVALID */
#endif /* FEATURE_QTV_PLAYLIST */

          pme->m_bGotMediaInfo = TRUE;
        }
#ifdef FEATURE_APP_REAL
        else if ((result == SUCCESS) && (ClsId == AEECLSID_MEDIAREAL) &&
                 (pCmdNotify->dwSize == sizeof(AEEMediaRealSpec)))
        {                              // Verify that it's an Real
                                       // Media spec
          AEEMediaRealSpec *pSpec = (AEEMediaRealSpec *)pCmdNotify->pCmdData;

          /*
           * this copying assumes that there is no pointers in info structure
           */
          (void)MEMMOVE(&pme->m_mediaRealInfo, pSpec, sizeof(AEEMediaRealSpec));
          pme->m_dwDuration = pme->m_mediaRealInfo.dwDuration;      // milliseconds
          pme->m_bGotMediaInfo = TRUE;
        }
#endif
      }
      if (pme->m_bDisplayInfoPending)
      {
        if (pme->m_bGotMediaInfo)
          (void)CPlayerWin_Info(pme, TRUE);
        pme->m_bDisplayInfoPending = FALSE;
        return;
      }
    }
#endif     // FEATURE_APP_MPEG4
  }                                    // MM_CMD_GETMEDIAPARM

  /*
   * if callback needed, do it.
   */
  if (pme && (pme->m_CB.m_pFnPtr != NULL && pCmdNotify->nCmd == pme->m_CB.m_nCmd &&
      pCmdNotify->nSubCmd == pme->m_CB.m_nSubCmd))
  {
    (void)MEMCPY(&pme->m_CB.m_CmdNotify, pCmdNotify, sizeof(AEEMediaCmdNotify));
    (*pme->m_CB.m_pFnPtr)((CPlayerWin *)pme);
  }
}
#endif //FEATURE_APP_MEDIA_VIDEO

static int CPlayerWin_GetMediaPosition(CPlayerWin *pme, uint32 *pos)
{
  int    nRet = EFAILED;
  uint32 dwPos = 0;

#if defined(FEATURE_APP_MPEG4)
  uint32 dwVideoPos = 0;
  uint32 dwAudioPos = 0;
  uint32 dwTextPos = 0;
  uint32 dwPlaybackPos = 0;

  {
    AEEMediaPlaybackPos PlayPos;

    nRet = IMEDIA_GetPlaybackPositions(pme->m_pMedia, &PlayPos);
    if (SUCCESS == nRet)
    {
      // dwPlaybackPos contains the greatest of the video/audio/test
      // positions.  If it contains a value, use it.
      dwPlaybackPos = MPClipTimeInSeconds(PlayPos.dwPlaybackPos);
      if (dwPlaybackPos == 0)
      {
        if ((pme->m_mediaInfo.mpeg4Type == MM_MPEG4_VIDEO_ONLY) ||
          (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_VIDEO_TEXT) ||
          (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_VIDEO_TEXT) ||
          (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_VIDEO))
        {
          // Get current video position
          dwVideoPos = MPClipTimeInSeconds(PlayPos.dwVideoPos);
          dwPos = dwVideoPos;
        }
        if ((pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_ONLY) ||
          (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_TEXT) ||
          (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_VIDEO_TEXT) ||
          (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_VIDEO))
        {
          // Get current audio position
          dwAudioPos = MPClipTimeInSeconds(PlayPos.dwAudioPos);
          if (dwAudioPos > dwPos)
            dwPos = dwAudioPos;
        }
#ifdef MP_FEATURE_MPEG4_TEXT
        if ((pme->m_mediaInfo.mpeg4Type == MM_MPEG4_TEXT) ||
          (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_VIDEO_TEXT) ||
          (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_TEXT) ||
          (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_VIDEO_TEXT))
        {
          // Get current text  position
          dwTextPos = MPClipTimeInSeconds(PlayPos.dwTextPos);
          if (dwTextPos > dwPos)
            dwPos = dwTextPos;
        }
#endif // MP_FEATURE_MPEG4_TEXT
      }
      else
      {
        dwPos = dwPlaybackPos;
      }

      DBGPRINTF_MED("MP: VideoPos=%d, AudioPos=%d, TextPos=%d, dwPlaybackPos=%d, dwPos=%d", 
         PlayPos.dwVideoPos, PlayPos.dwAudioPos, PlayPos.dwTextPos, PlayPos.dwPlaybackPos, dwPos);
    }
    else
    {
      DBGPRINTF_ERROR("MP: Failed, IMEDIA_GetPlaybackPositions, nRet=%d", nRet);
    }
  }
#endif     // FEATURE_APP_MPEG4

  *pos = dwPos;
  return nRet;
}

/*==========================================================================
FUNCTION: CPlayerWin_Replay

DESCRIPTION:
  Replays previously played file. Current media is released and new media
  created.

RETURN VALUE:
  TRUE  = Play started successfully.
===========================================================================*/
static boolean CPlayerWin_Replay(CPlayerWin* pWin)
{
  DBGPRINTF_MED("MP: Replay");
  // Play the currently selected file.
  return CPlayerWin_PlayNext(pWin, CONT_MODE_SINGLE_FILE, FALSE);
}


// return TRUE, if SUCCESS, other return FALSE
// nMode == CONT_MODE_OFF (not imp)
//   select next file in the list and play if the index is <= the last one
// nMode == CONT_MODE_SINGLE_FILE
//   replay the same file
// nMode == CONT_MODE_ALL_FILES
//   play next file from list, if last one is reached, select and play the first one
//   bNextFile is TRUE, previous file, otherwise next file

static boolean CPlayerWin_PlayNext(CPlayerWin* pwin, int nMode, boolean bNextFile)
{
  int nRet = FALSE;
  CMediaPlayer* pMP;
  int nNextIndex = 0;

  if(!pwin || !pwin->m_pOwner)
  {
    DBGPRINTF_ERROR("MP: NULL ptr when calling CPlayerWin_PlayNext(). pwin = 0x%X, pOwner = 0x%X",
                    pwin, (pwin)?pwin->m_pOwner:0);
    return nRet;
  }

  pMP = pwin->m_pOwner;
  if(nMode == CONT_MODE_OFF)
  {
    return nRet;
  }

  // reset state,
  pMP->m_playState = MP_PLAYSTATE_STOPPED;
  pMP->m_ErrorState = MP_ERRORSTATE_NONE;

  // reset position for next file
  pwin->m_dwPlayPos = 0;
#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)
  pwin->m_dwPositionAtStop = 0;
  pMP->m_strReplayFileName[0] = '\0';
#endif //#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)

  // We have not pressed Stop
  pwin->m_bStopPressed = FALSE;
  pwin->m_bStoppedWhileConnecting = FALSE;

#if defined (FEATURE_APP_MPEG4) ||defined (FEATURE_APP_REAL)
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
  if (pwin->m_pMedia)
  {
    AEECLSID  ClsId;
    (void)IMEDIA_GetClassID(pwin->m_pMedia, &ClsId);
    if(ClsId == AEECLSID_MEDIAMPEG4)
    {
      DBGPRINTF_MED("MP: Get current zoom and rotation values");
      pMP->m_dwLastZoomVal = pwin->m_dwZoomScale;
      pMP->m_LastRotationVal = pwin->m_Rotation;
    }
  }
#endif // defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
#endif // defined (FEATURE_APP_MPEG4) ||defined (FEATURE_APP_REAL)

  // select next file in the list and play if the index is < the last one
  if (nMode == CONT_MODE_SINGLE_FILE)
  {
    DBGPRINTF_MED("MP: releasing current media.");
    (void)ISHELL_CancelTimer(pwin->m_pIShell, (PFNNOTIFY)CPlayerWin_AutoPlayClip, pMP);
    MP_RELEASEIF(pwin->m_pMedia);
    /*
     * even though it is same file, we will get this info
     * again from OEM layer
     */
    pwin->m_bGotMediaInfo = FALSE;
#ifdef MP_FEATURE_ICONTENTHANDLER
    if (pMP->m_bIContentMode)
    {
      char* pFileName;
      pFileName = pMP->m_FileExplorer.m_FileArray[pMP->m_CurFileIndex].m_szName;
      // select and play this content now
      if(SUCCESS != MPContentSelectFile(pMP->m_pContent, pFileName, 0, TRUE))
      {
        DBGPRINTF_ERROR("MP: call to MPContentSelectFile failed.");
        return nRet;
      }
      nRet = TRUE;
    }
    else
#elif defined(MP_FEATURE_ICONTENT)
    if (pMP->m_bIContentMode)
    {
      char* pCurrentFileURI;
      pCurrentFileURI = pMP->m_FileExplorer.m_FileArray[pMP->m_CurFileIndex].m_szName;
      // select and play this content now
      if(SUCCESS != MPContentSelectFile(pMP->m_pContent, pCurrentFileURI, 0, TRUE))
      {
        DBGPRINTF_ERROR("MP: call to MPContentSelectFile failed.");
        return nRet;
      }
      nRet = TRUE;
    }
    else
#endif //MP_FEATURE_ICONTENTHANDLER

    {
      /*
       * MPEG4-specific:
       *
       * We had been restoring defaults for m_bNoTextOrHeader here, but
       * we're replaying the same file so there's no point.
       */

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      if (FALSE == pMP->m_bEnablePSSimulation && FALSE == pMP->m_bFileWriteReadTest)
      {
        DBGPRINTF_MED("MP: FALSE == pwin->m_bEnablePSSimulation && FALSE == pwin->m_bFileWriteReadTest");
#endif
#endif
        if(CMediaPlayer_PlayFile(pMP, pMP->m_FileExplorer.m_FileArray[pMP->m_CurFileIndex].m_szName))
        {
          DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
          (void)ISHELL_PostEvent(pMP->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
          nRet = TRUE;
        }
        else
        {
          DBGPRINTF_ERROR("MP: call to CMediaPlayer_PlayFile failed.");
        }
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      }
      else
      {
        if(CMediaPlayer_PseudoPlayFile(pMP, pMP->m_FileExplorer.m_FileArray[pMP->m_CurFileIndex].m_szName))
        {
          DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
          (void)ISHELL_PostEvent(pMP->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
          nRet = TRUE;
        }
        else
        {
          DBGPRINTF_ERROR("MP: call to CMediaPlayer_PseudoPlayFile failed.");
        }
      }
#endif
#endif
    }
  }
  /*
   * If test mode is All Files, play all the files until stop is pressed
   */
  else if (nMode == CONT_MODE_ALL_FILES)
  {
    uint16    iconId = 0;
    int       nCount = 0;

    DBGPRINTF_MED("MP: releasing current media.");
    (void)ISHELL_CancelTimer(pwin->m_pIShell, (PFNNOTIFY)CPlayerWin_AutoPlayClip, pMP);
    MP_RELEASEIF(pwin->m_pMedia);

#ifdef FEATURE_QTV_GENERIC_BCAST

  FREEIF(pMP->m_pTrackList);
  pMP->m_nTrackCount = 0;

#endif //#ifdef FEATURE_QTV_GENERIC_BCAST

    // get next file index
    if(bNextFile)
      nNextIndex = CMediaPlayer_GetNextFileIndex(pMP, pMP->m_CurFileIndex);
    else
      nNextIndex = CMediaPlayer_GetPreviousFileIndex(pMP, pMP->m_CurFileIndex);

#ifdef MP_FEATURE_ICONTENTHANDLER
    if (pMP->m_bIContentMode)
    {
        char* pFileName;

        pFileName = pMP->m_FileExplorer.m_FileArray[nNextIndex].m_szName;
        iconId = 0;

        /*
         * Get the file handler type, if the media is not an audio or
         * video file that is supported,
         * skip to the next file in the list.
         */
        iconId = CMediaPlayer_FindHandlerType(pMP, pFileName);
        nCount = 0;
        // find next if this one is not what we wanted.
        while ((iconId == 0) || (iconId == IDB_IMAGE_ICON) ||
               ((pMP->m_bIncallMode == TRUE) && (iconId != IDB_AUDIO_ICON)))
        {
          if(bNextFile)
            nNextIndex = CMediaPlayer_GetNextFileIndex(pMP, nNextIndex);
          else
            nNextIndex = CMediaPlayer_GetPreviousFileIndex(pMP, nNextIndex);

          pFileName = pMP->m_FileExplorer.m_FileArray[nNextIndex].m_szName;
          iconId = CMediaPlayer_FindHandlerType(pMP, pFileName);
          if(nCount > (pMP->m_FileExplorer.m_NumFiles + 1))
          {
            // we have search all files, can't find any valid content file, break out now
            DBGPRINTF_ERROR("MP: cannot find any valid file to play.");
            return nRet;
          }

          DBGPRINTF_MED("MP: find next content file, nNextIndex = %d, iconId = %d, cnt %d.",
                        nNextIndex, iconId, nCount);
          nCount++;
        }

        /*
         * discard old media info, as we will play new media
         */
        pwin->m_bGotMediaInfo = FALSE;
        pMP->m_bShowLogo = TRUE;
        pMP->m_bShowPlayerWinSettingUI = FALSE;

#ifdef MP_FEATURE_VIDEO
        /*
         * Restore default geometry settings.  We don't know what
         * the new media's dimensions will be.
         */
        pwin->m_bNoTextOrHeader = FALSE;
        if (pwin->m_bFullScreen)
        {
          DBGPRINTF_MED("MP: Ignoring setting to m_rectIdleImage");
        }
        else
        {
          (void)MEMCPY(&pwin->m_rectImage, &pwin->m_rectIdleImage, sizeof(AEERect));  //TAV
          (void)MEMCPY(&pwin->m_dfltRectImage, &pwin->m_rectImage, sizeof(AEERect));
        }
#endif     // MP_FEATURE_VIDEO

        /* invalidate http buffer on any new file selection */
        if(pMP->m_CurFileIndex != nNextIndex)
          HttpDownloader_entireClipInBuffer = FALSE;

        pFileName = pMP->m_FileExplorer.m_FileArray[nNextIndex].m_szName;
        // select and play this content now
        if(SUCCESS != MPContentSelectFile(pMP->m_pContent, pFileName, 0, TRUE))
        {
          DBGPRINTF_ERROR("MP: call to MPContentSelectFile failed.");
          return nRet;
        }

        pMP->m_CurFileIndex = nNextIndex;
        nRet = TRUE;
    }
    else
#elif defined(MP_FEATURE_ICONTENT)
    if (pMP->m_bIContentMode)
    {
        char szFileName[MP_MAX_FILE_NAME];
        char* pCurrentFileURI;
        pCurrentFileURI = pMP->m_FileExplorer.m_FileArray[nNextIndex].m_szName;
        szFileName[0] = '\0';
        iconId = 0;
        if(SUCCESS != MPContentGetLocalURLfromURI(pMP->m_pContent, pCurrentFileURI, szFileName, MP_MAX_FILE_NAME))
        {
          /*
           * Get the file handler type, if the media is not an audio or
           * video file that is supported,
           * skip to the next file in the list.
           */
          iconId = CMediaPlayer_FindHandlerType(pMP, szFileName);
        }

        nCount = 0;
        // find next if this one is not what we wanted.
        while ((iconId == 0) || (iconId == IDB_IMAGE_ICON) ||
               ((pMP->m_bIncallMode == TRUE) && (iconId != IDB_AUDIO_ICON)))
        {
          if(bNextFile)
            nNextIndex = CMediaPlayer_GetNextFileIndex(pMP, nNextIndex);
          else
            nNextIndex = CMediaPlayer_GetPreviousFileIndex(pMP, nNextIndex);

          pCurrentFileURI = pMP->m_FileExplorer.m_FileArray[nNextIndex].m_szName;
          iconId = 0;
          if(SUCCESS == MPContentGetLocalURLfromURI(pMP->m_pContent, pCurrentFileURI, szFileName, MP_MAX_FILE_NAME))
            iconId = CMediaPlayer_FindHandlerType(pMP, szFileName);

          if(nCount > (pMP->m_FileExplorer.m_NumFiles + 1))
          {
            // we have search all files, can?t find any valid content file, break out now
            DBGPRINTF_ERROR("MP: cannot find any valid file to play.");
            return nRet;
          }
          DBGPRINTF_MED("MP: find next content file, nNextIndex = %d, iconId = %d, cnt %d.",
                        nNextIndex, iconId, nCount);
          nCount++;
        }

        /*
         * discard old media info, as we will play new media
         */
        pwin->m_bGotMediaInfo = FALSE;
        pMP->m_bShowLogo = TRUE;
        pMP->m_bShowPlayerWinSettingUI = FALSE;

#ifdef MP_FEATURE_VIDEO
        /*
         * Restore default geometry settings.  We don't know what
         * the new media's dimensions will be.
         */
        pwin->m_bNoTextOrHeader = FALSE;
        if (pwin->m_bFullScreen)
        {
          DBGPRINTF_MED("MP: Ignoring setting to m_rectIdleImage");
        }
        else
        {
          (void)MEMCPY(&pwin->m_rectImage, &pwin->m_rectIdleImage, sizeof(AEERect));  //TAV
          (void)MEMCPY(&pwin->m_dfltRectImage, &pwin->m_rectImage, sizeof(AEERect));
        }
#endif     // MP_FEATURE_VIDEO

        /* invalidate http buffer on any new file selection */
        if(pMP->m_CurFileIndex != nNextIndex)
          HttpDownloader_entireClipInBuffer = FALSE;

        pCurrentFileURI = pMP->m_FileExplorer.m_FileArray[nNextIndex].m_szName;
        // select and play this content now
        if(SUCCESS != MPContentSelectFile(pMP->m_pContent, pCurrentFileURI, 0, TRUE))
        {
          DBGPRINTF_ERROR("MP: call to MPContentSelectFile failed.");
          return nRet;
        }

        pMP->m_CurFileIndex = nNextIndex;
        nRet = TRUE;
    }
    else
#endif //#ifdef MP_FEATURE_ICONTENTHANDLER
    {
        /*
         * Get the file handler type, if the media is not an audio or
         * video file that is supported,
         * skip to the next file in the list.
         */
        iconId = CMediaPlayer_FindHandlerType(pMP, pMP->m_FileExplorer.m_FileArray[nNextIndex].m_szName);
        nCount = 0;
        while ((iconId == 0) || (iconId == IDB_IMAGE_ICON) ||
               ((pMP->m_bIncallMode == TRUE) && (iconId != IDB_AUDIO_ICON)))
        {
          if(bNextFile)
            nNextIndex = CMediaPlayer_GetNextFileIndex(pMP, nNextIndex);
          else
            nNextIndex = CMediaPlayer_GetPreviousFileIndex(pMP, nNextIndex);

          iconId = CMediaPlayer_FindHandlerType(pMP, pMP->m_FileExplorer.m_FileArray[nNextIndex].m_szName);
          if(nCount > (pMP->m_FileExplorer.m_NumFiles + 1))
          {
            // we have search all files, can?t find any valid content file, break out now
            DBGPRINTF_ERROR("MP: cannot find any valid file to play.");
            return nRet;
          }
          DBGPRINTF_MED("MP: find next content file, nNextIndex = %d, iconId = %d, cnt %d.",
                        nNextIndex, iconId, nCount);
          nCount++;
        }
        /*
         * discard old media info, as we will play new media
         */
        pwin->m_bGotMediaInfo = FALSE;
        pMP->m_bShowLogo = TRUE;
        pMP->m_bShowPlayerWinSettingUI = FALSE;

        // Erase the Current Screen
        DBGPRINTF_MED("MP: Erasing Current Screen which is x = %d, y = %d, dx = %d, dy = %d", pwin->m_rectImage.x, pwin->m_rectImage.y,
          pwin->m_rectImage.dx, pwin->m_rectImage.dy);
        IDISPLAY_EraseRect(pwin->m_pIDisplay, &pwin->m_rectImage);
        //IDISPLAY_Update(pwin->m_pIDisplay);

        CPlayerWin_UpdateImage(pwin);
     
#ifdef MP_FEATURE_VIDEO
        /*
         * Restore default geometry settings.  We don't know what
         * the new media's dimensions will be.
         */
        pwin->m_bNoTextOrHeader = FALSE;
        if (pwin->m_bFullScreen)
        {
          DBGPRINTF_MED("MP: Ignoring setting to m_rectIdleImage");

        }
        else
        {
          (void)MEMCPY(&pwin->m_rectImage, &pwin->m_rectIdleImage, sizeof(AEERect));  //TAV
          (void)MEMCPY(&pwin->m_dfltRectImage, &pwin->m_rectImage, sizeof(AEERect));
        }
#endif     // MP_FEATURE_VIDEO

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        /* invalidate http buffer on any new file selection */
        if(pMP->m_CurFileIndex != nNextIndex)
          HttpDownloader_entireClipInBuffer = FALSE;

        if (FALSE == pMP->m_bEnablePSSimulation && FALSE == pMP->m_bFileWriteReadTest)
        {
#endif
#endif
          if(CMediaPlayer_PlayFile(pMP, pMP->m_FileExplorer.m_FileArray[nNextIndex].m_szName))
          {
            pMP->m_CurFileIndex = nNextIndex;
            DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
            (void)ISHELL_PostEvent(pMP->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
            nRet = TRUE;
          }
          else
            DBGPRINTF_ERROR("MP: call to CMediaPlayer_PlayFile failed.");
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        }
        else
        {
          if(CMediaPlayer_PseudoPlayFile(pMP, pMP->m_FileExplorer.m_FileArray[nNextIndex].m_szName))
          {
            pMP->m_CurFileIndex = nNextIndex;
            DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
            (void)ISHELL_PostEvent(pMP->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
            nRet = TRUE;
          }
          else
            DBGPRINTF_ERROR("MP: call to CMediaPlayer_PseudoPlayFile failed.");
        }
#endif
#endif
    }
  }
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  // If we failed to play next, make sure Http is cleaned up
  if((nRet != TRUE) && (pwin->psHttpBlock))
  {
    if (pwin->psHttpBlock->bWebPstreaming)
    {
      PS_CleanHttpDownload(pwin->psHttpBlock);
    }
    FREE(pwin->psHttpBlock);
    pwin->psHttpBlock = NULL;
  }
#endif
  return nRet;
}

static void CPlayerWin_PlayNextClip(CPlayerWin *pme)
{
  if (pme->m_CB.m_nCBCount == 0)
  {
    if(CPlayerWin_IsPlaying(pme))
    {
      pme->m_CB.m_pFnPtr = &CPlayerWin_PlayNextClip;
      pme->m_CB.m_nCBCount++;
      pme->m_CB.m_nCmd = MM_CMD_PLAY;
      pme->m_CB.m_nSubCmd = 0;

      // We are "hitting the stop button"  If continuous mode is
      // on we don't want to jump two clips
      pme->m_bStopPressed = TRUE;
      (void)IMEDIA_Stop(pme->m_pMedia);

      DBGPRINTF_HIGH("MP: IMedia is still in playing state, wait for fully stop.");
    }
    else
    {
      DBGPRINTF_HIGH("MP: Play Next media clip");
      (void)CPlayerWin_PlayNext(pme, CONT_MODE_ALL_FILES, TRUE);
    }
  }
  else
  {
    boolean   bChg = 0;
    int       nState = 0;

    if (pme->m_CB.m_nCmd == MM_CMD_PLAY && pme->m_CB.m_nSubCmd == 0 &&
          (pme->m_CB.m_CmdNotify.nStatus == MM_STATUS_DONE || pme->m_CB.m_CmdNotify.nStatus == MM_STATUS_ABORT))
    {
      /*
       * make sure IMedia is fully stopped
       */
      if (pme->m_pMedia)
      {
        nState = IMEDIA_GetState(pme->m_pMedia, &bChg);
      }
      /*
       * try max. 1 second
       */
      if ((bChg || MM_STATE_PLAY == nState) && pme->m_CB.m_nCBCount < MP_MAX_RETRY_COUNTS)
      {
        pme->m_CB.m_nCBCount++;
        DBGPRINTF_HIGH("MP: IMedia is still in playing state, start timer and try again in 50ms.");
        /*
         * wait for 50 ms and try it again
         */
        (void)ISHELL_SetTimer(pme->m_pIShell, 50, (PFNNOTIFY)CPlayerWin_PlayNextClip, pme);
        return;
      }

      pme->m_CB.m_pFnPtr = NULL;
      pme->m_CB.m_nCBCount = 0;
      pme->m_CB.m_nCmd = -1;
      pme->m_CB.m_nSubCmd = 0;

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      // Make sure we have cleaned up any HTTP streams before going to next
      HttpDownloader_entireClipInBuffer = FALSE;

      if(pme->psHttpBlock != NULL)
      {
        if (pme->psHttpBlock->bWebPstreaming)
        {
          PS_CleanHttpDownload(pme->psHttpBlock);
        }
        FREE(pme->psHttpBlock);
      }
      pme->psHttpBlock = NULL;
#endif // FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif //FEATURE_APP_MPEG4

      DBGPRINTF_HIGH("MP: Play Next media clip");
      (void)CPlayerWin_PlayNext(pme, CONT_MODE_ALL_FILES, TRUE);
    }
  }
}

static void CPlayerWin_PlayPreviousClip(CPlayerWin *pme)
{
  if (pme->m_CB.m_nCBCount == 0)
  {
    if(CPlayerWin_IsPlaying(pme))
    {
      pme->m_CB.m_pFnPtr = &CPlayerWin_PlayPreviousClip;
      pme->m_CB.m_nCBCount++;
      pme->m_CB.m_nCmd = MM_CMD_PLAY;
      pme->m_CB.m_nSubCmd = 0;

      // We are "hitting the stop button"  If continuous mode is
      // on we don't want to just re-start this clip
      pme->m_bStopPressed = TRUE;
      (void)IMEDIA_Stop(pme->m_pMedia);
    }
    else
    {
      DBGPRINTF_HIGH("MP: Play previous media clip");
      (void)CPlayerWin_PlayNext(pme, CONT_MODE_ALL_FILES, FALSE);
    }
  }
  else
  {
    boolean   bChg = 0;
    int       nState = 0;

    if (pme->m_CB.m_nCmd == MM_CMD_PLAY && pme->m_CB.m_nSubCmd == 0 &&
          (pme->m_CB.m_CmdNotify.nStatus == MM_STATUS_DONE || pme->m_CB.m_CmdNotify.nStatus == MM_STATUS_ABORT))
    {
      /*
       * make sure IMedia is fully stopped
       */
      if (pme->m_pMedia)
      {
        nState = IMEDIA_GetState(pme->m_pMedia, &bChg);
      }
      /*
       * try max. 1 second
       */
      if ((bChg || MM_STATE_PLAY == nState) && pme->m_CB.m_nCBCount < MP_MAX_RETRY_COUNTS)
      {
        pme->m_CB.m_nCBCount++;
        DBGPRINTF_HIGH("MP: IMedia is still in playing state, start timer and try again in 50ms.");
        /*
         * wait for 50 ms and try it again
         */
        (void)ISHELL_SetTimer(pme->m_pIShell, 50, (PFNNOTIFY)CPlayerWin_PlayPreviousClip, pme);
        return;
      }

      pme->m_CB.m_pFnPtr = NULL;
      pme->m_CB.m_nCBCount = 0;
      pme->m_CB.m_nCmd = -1;
      pme->m_CB.m_nSubCmd = 0;

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      // Make sure we have cleaned up any HTTP streams before going to next
      HttpDownloader_entireClipInBuffer = FALSE;

      if(pme->psHttpBlock != NULL)
      {
        if (pme->psHttpBlock->bWebPstreaming)
        {
          PS_CleanHttpDownload(pme->psHttpBlock);
        }
        FREE(pme->psHttpBlock);
      }
      pme->psHttpBlock = NULL;
#endif // FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#endif //FEATURE_APP_MPEG4

      DBGPRINTF_HIGH("MP: Play previous media clip");
      (void)CPlayerWin_PlayNext(pme, CONT_MODE_ALL_FILES, FALSE);
    }
  }
}

#ifdef FEATURE_QTV_GENERIC_BCAST
static void CPlayerWin_Reposition(CPlayerWin *pme)
{
  int     retVal;
  int32   nTimestamp;

  nTimestamp = pme->m_pOwner->m_pSetting->m_BcastData.nTimestamp;

  DBGPRINTF_MED("MP: CPlayerWin_Reposition to time %d", nTimestamp);

  retVal = IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_REPOSITION,
                               nTimestamp, (int32)&pme->m_pOwner->m_TransId);
  if((retVal != SUCCESS) && (retVal != MM_PENDING))
  {
    DBGPRINTF_HIGH("MP: IMEDIA_SetMediaParm() returns %d = %s",
                  retVal, MP_ErrorStateToString(retVal));
  }
}
#endif // FEATURE_QTV_GENERIC_BCAST

#ifdef FEATURE_QTV_GENERIC_BCAST
static boolean CPlayerWin_SyncFrameForward(CPlayerWin *pme)
{
  int     retVal;
  int32   nOffset;

  nOffset = pme->m_pOwner->m_pSetting->m_BcastData.nOffset;
  DBGPRINTF_MED("MP: CPlayerWin_SyncFrameForward Offset is %d", nOffset);

  retVal = IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_PLAY_IFRAME_MODE,
                               nOffset, (int32)&pme->m_pOwner->m_TransId);
  if((retVal != SUCCESS) && (retVal != MM_PENDING))
  {
    DBGPRINTF_HIGH("MP: IMEDIA_SetMediaParm() returns %d = %s",
                  retVal, MP_ErrorStateToString(retVal));
  }
  return retVal;
}
#endif // FEATURE_QTV_GENERIC_BCAST

#ifdef FEATURE_QTV_GENERIC_BCAST
static boolean CPlayerWin_SyncFrameBack(CPlayerWin *pme)
{
  int     retVal;
  int32   nOffset;

  nOffset = pme->m_pOwner->m_pSetting->m_BcastData.nOffset;
  DBGPRINTF_MED("MP: CPlayerWin_SyncFrameBack Offset is %d", -nOffset);

  retVal = IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_PLAY_IFRAME_MODE,
                               -nOffset, (int32)&pme->m_pOwner->m_TransId);
  if((retVal != SUCCESS) && (retVal != MM_PENDING))
  {
    DBGPRINTF_HIGH("MP: IMEDIA_SetMediaParm() returns %d = %s",
                  retVal, MP_ErrorStateToString(retVal));
  }
  return retVal;
}
#endif // FEATURE_QTV_GENERIC_BCAST



#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif     // FEATURE_QTV_MFDRM


/*===========================================================================
  This function is used to help emit state status to debug output.
===========================================================================*/
char *MP_MediaPlayerStateEnumToString(enum MediaPlayerState mpState)
{
  switch (mpState)
  {
  case MP_STATE_NULL:
    return("NULL");
  case MP_STATE_UNKNOWN:
    return("UNKNOWN");
  case MP_STATE_START_SUSPEND:
    return("START_SUSPEND");
  case MP_STATE_START_RESUME:
    return("START_RESUME");
  case MP_STATE_BROWSE_FILE:
    return("BROWSE_FILE");
  case MP_STATE_MAIN_WIN:
    return("MAIN_WIN");
  case MP_STATE_PLAYER_WIN:
    return("PLAYER_WIN");
  case MP_STATE_RECORDER_WIN:
    return("RECORDER_WIN");
  case MP_STATE_IMAGE_WIN:
    return("IMAGE_WIN");
  case MP_STATE_SEQUENCER_WIN:
    return("SEQUENCER_WIN");
  case MP_STATE_SETTING:
    return("SETTING");
  case MP_STATE_HELP_WIN:
    return("HELP_WIN");
  case MP_STATE_STREAM_RECORD_WIN:
    return("STREAM_RECORD_WIN");
  default:
    return("Undefined");
  }
}

/*===========================================================================
  This function is used to help emit play state status to debug output.
===========================================================================*/
char *MP_MediaPlayerPlayStateEnumToString(enum MediaPlayerPlayState mpPlayState)
{
  switch (mpPlayState)
  {
  case MP_PLAYSTATE_PLAYING:
    return("PLAYING");
  case MP_PLAYSTATE_PAUSED:
    return("PAUSED");
  case MP_PLAYSTATE_STOPPED:
    return("STOPPED");
  case MP_PLAYSTATE_RECORDING:
    return("RECORDING");
  case MP_PLAYSTATE_CONNECTING:
    return("CONNECTING");
  case MP_PLAYSTATE_BUFFERING:
    return("BUFFERING");
  case MP_PLAYSTATE_READY_TO_PLAY:
    return("READY_TO_PLAY");
  case MP_PLAYSTATE_ERROR:
    return("ERROR");
  case MP_PLAYSTATE_EXITING:
    return("EXITING");
  default:
    return("Undefined");
  }
}


#if defined (FEATURE_APP_MPEG4) && defined(FEATURE_QTV_IPL_SCALING )
#error code not present
#endif //#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)


#ifdef MP_FEATURE_VIDEO
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)

/*===========================================================================
   This function resets the frame transformation state (rotation and
   scaling).
===========================================================================*/
static void CPlayerWin_ResetFrameTransformations(CPlayerWin *pme)
{
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
  pme->m_dwZoomScale = MM_MPEG4_NO_SCALING;
  pme->m_Rotation = MM_MPEG4_NO_ROTATION;
#endif // defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
#endif //FEATURE_APP_MPEG4
  pme->m_pendingZoomDirection = 0;
}

/*===========================================================================
   This function advances the stored zoom scalefactor according to the
   pending zoom direction. It then clears the pending direction so that
   another zoom command can be issued. If applicable, the display is updated
   to reflect the new zoom scalefactor.
===========================================================================*/
static void
CPlayerWin_UpdateZoomScale(CPlayerWin *pme)
{
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
  pme->m_dwZoomScale =
    MP4_GetNextScale(pme->m_dwZoomScale,
                         (pme->m_pendingZoomDirection ==
                      1) ? IDM_PM_ZOOM_IN : IDM_PM_ZOOM_OUT);
#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#endif     // FEATURE_QTV_IPL_SCALING
#endif // defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
#endif //FEATURE_APP_MEDIA_VIDEO

  /*
   * Clear so we can issue another zoom command.
   */
  pme->m_pendingZoomDirection = 0;


}
#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS ||
           // FEATURE_QTV_MDP_TRANSFORMATIONS
#endif     // MP_FEATURE_VIDEO


/*===========================================================================
   This function updates the ProgCtl if full screen mode is not active
   and the progress timer is not running.
===========================================================================*/
static void 
CPlayerWin_UpdateProgCtl(CPlayerWin *pme, int nCmd, int nSubCmd, uint16 wResID)
{
  boolean   bShowRecIcon;
  boolean   bIsTimedMessage = FALSE;
  char      sz[32] = { '\0' };

  if (!pme)
  {
    DBGPRINTF_ERROR("MP: pme is NULL in CPlayerWin_UpdateProgCtl!");
    return;
  }

  if ((MP_EVENT_REDRAW_CTRL != nCmd) && ((wResID == 0) ||
      !ISHELL_LoadResString(pme->m_pIShell, MEDIAPLAYER_RES_FILE, wResID,
                            pme->m_szTimerText, sizeof(pme->m_szTimerText))))
  {
    pme->m_szTimerText[0] = (AECHAR) 0;
  }

  /*
   * This must be the queued message from PVPlayer indicating Connection Setup
   */

  switch (nCmd)
  {
  case MM_CMD_PLAY:
    switch (nSubCmd)
    {
#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
    case MM_MP4_PARM_PB_ABORT:
#endif
      MPProgCtl_MessageTimerCancel(pme->m_pMPProgCtl);
      CPlayerWin_BuildProgCtlMessage(pme, " Error");
      break;

#if defined (FEATURE_APP_MEDIA_VIDEO)
#error code not present
#elif defined(FEATURE_APP_MPEG4) || defined(FEATURE_APP_REAL)
    case MM_MP4_PARM_CONNECT_UPDATE:
#endif
      if (!ConnectStatusStringActive)
      {
        CPlayerWin_BuildProgCtlMessage(pme, " Connecting..");
        ConnectStatusStringActive = 1;
      }
      break;

#if defined (FEATURE_APP_MEDIA_VIDEO)
#error code not present
#elif defined(FEATURE_APP_MPEG4) || defined(FEATURE_APP_REAL)
    case MM_MP4_PARM_BUFFER_UPDATE:
#endif
      (void)SNPRINTF(sz, sizeof(sz), " %s %d%s", "Buffering ", (int)prerollPercentage, "%");
      CPlayerWin_BuildProgCtlMessage(pme, sz);
      pme->m_wPct = prerollPercentage;
      break;

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    case MM_MP4_PARM_PS_BUFFER_UPDATE:
    {
      char *str = (BufferUpdateToggle) ? "Kbps." : "Kbps...";
      (void)SNPRINTF(sz, sizeof(sz), " %s %d %s", "Buffering@", pseudostreamDownloadRate, str);
      CPlayerWin_BuildProgCtlMessage(pme, sz);
      BufferUpdateToggle = !BufferUpdateToggle;
      break;
    }
#endif // (FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))

#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined(FEATURE_APP_MPEG4) || defined(FEATURE_APP_REAL)
    case MM_MP4_PARM_PB_READY:
#endif
      /*
       * Received an update having moved into PB Ready State
       */
      CPlayerWin_BuildProgCtlMessage(pme, " Ready to Play..");
      ConnectStatusStringActive = 0;
      break;

#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_IPL_SCALING)
#error code not present
#endif // #if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_IPL_SCALING)

#ifdef MP_FEATURE_FAST_PLAYBACK
      case MM_MP4_PARM_PB_SPEED:
        if(pme->m_ReplaySpeed == QTV_PLAYBACK_SPEED_NORMAL)
        {
          CPlayerWin_BuildProgCtlMessage(pme, " 1x.");
          bIsTimedMessage = TRUE;
        }
        else if(pme->m_ReplaySpeed == QTV_PLAYBACK_SPEED_1P3X)
        {
          CPlayerWin_BuildProgCtlMessage(pme, " 1.3x.");
          bIsTimedMessage = TRUE;
        }
        else
        {
          DBGPRINTF_ERROR("MP: unknown speed setting, %d", wResID);
        }
      break;
#endif //MP_FEATURE_FAST_PLAYBACK

    default:
      {
        char      temp[32] = { '\0' };

        (void)WSTRTOSTR(pme->m_szTimerText, temp, sizeof(temp));

        // TBD:
        // Not sure why we do this here.  The proper way is to do this
        // is to set the state to playing when the state changes, not here
        // in the update handler
        if ((STRCMP(temp, "PL") == 0) && !pme->m_bResumeInProcessing && !CPlayerWin_IsPause(pme))
        {
          pme->m_pOwner->m_playState = MP_PLAYSTATE_PLAYING;
          EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
        }

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
#ifdef MM_STATUS_DOWNLOAD_PERCENTAGE
        //If download percentage is available, use it
        //Otherwise, calculate percentage from position
        if (pme->m_dwDownldPct > 0)
        {
          //Use percentage
          uint32 dwPctDownld;

          dwPctDownld = pme->m_dwDownldPct;
          if (dwPctDownld > 100) 
          {
            dwPctDownld = 100;
          }
          if (pme->m_dwDuration == 0)
          {
            // Don't show pos/duration values if not known
            (void)SNPRINTF(sz, sizeof(sz), " DL %d%%", dwPctDownld);
          }
          else
          {
            uint32 dwDuration = MPClipTimeInSeconds(pme->m_dwDuration);

            (void)SNPRINTF(sz, sizeof(sz), " %02d:%02d/%02d:%02d DL %d%%", pme->m_dwPlayPos / 60,
                    pme->m_dwPlayPos % 60, dwDuration / 60,
                    dwDuration % 60, dwPctDownld);
          }
        }
        else if (pme->m_dwDownldPos > 0)
        {
          uint32 dwDuration = MPClipTimeInSeconds(pme->m_dwDuration);
          //Use position
          uint32 dwPctDownld;

          dwPctDownld = ((100 * pme->m_dwDownldPos) / pme->m_dwDuration);
          if (dwPctDownld > 100) 
          {
            dwPctDownld = 100;
          }
          (void)SNPRINTF(sz, sizeof(sz), " %02d:%02d/%02d:%02d DL %d%%", pme->m_dwPlayPos / 60,
                  pme->m_dwPlayPos % 60, dwDuration / 60,
                  dwDuration % 60, dwPctDownld);
        }
        else
#else
        if (pme->m_dwDownldPos > 0)
        {
          uint32 dwDuration = MPClipTimeInSeconds(pme->m_dwDuration);
          int16 dwPctDownld;

          dwPctDownld = (uint16)((100 * pme->m_dwDownldPos) / pme->m_dwDuration);
          if (dwPctDownld > 100)
          {
            dwPctDownld = 100;
          }
          (void)SNPRINTF(sz, sizeof(sz), " %02d:%02d/%02d:%02d DL %d%%", pme->m_dwPlayPos / 60,
                  pme->m_dwPlayPos % 60, dwDuration / 60,
                  dwDuration % 60, dwPctDownld);
        }
        else
#endif //MM_STATUS_DOWNLOAD_PERCENTAGE
#endif //FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
        {
          if (pme->m_dwDuration > 0)
          {
            uint32 dwDuration = MPClipTimeInSeconds(pme->m_dwDuration);

            (void)SNPRINTF(sz, sizeof(sz), " %02d:%02d/%02d:%02d", pme->m_dwPlayPos / 60,
                    pme->m_dwPlayPos % 60, dwDuration / 60, dwDuration % 60);
          }
          else
          {
            (void)SNPRINTF(sz, sizeof(sz), " %02d:%02d", pme->m_dwPlayPos / 60, pme->m_dwPlayPos % 60);
          }
        }

        CPlayerWin_BuildProgCtlMessage(pme, sz);
        if ((pme->m_dwDuration / 1000) > 0)
        {
          pme->m_wPct = (uint16) ((100 * pme->m_dwPlayPos) / (pme->m_dwDuration / 1000));
        }
        break;
      }
    }
    break;

  case MM_CMD_RECORD:
    if ((nSubCmd == MM_STATUS_SPACE_ERROR) || (nSubCmd == MM_STATUS_SPACE_WARNING))
    {
        CPlayerWin_BuildProgCtlMessage(pme, "");
        bIsTimedMessage = TRUE;
        pme->m_pOwner->m_playState = MP_PLAYSTATE_ERROR;
        pme->m_pOwner->m_ErrorState = MP_ERRORSTATE_ERROR;
    }
    else 
    {
      uint32 dwDuration = MPClipTimeInSeconds(pme->m_dwDuration);

      (void)SNPRINTF(sz, sizeof(sz), " %02d:%02d/%02d:%02d", pme->m_dwPlayPos / 60,
              pme->m_dwPlayPos % 60, dwDuration / 60, dwDuration % 60);

      CPlayerWin_BuildProgCtlMessage(pme, sz);

      if ((pme->m_dwDuration / 1000) > 0)
      {
        pme->m_wPct = (uint16) ((100 * pme->m_dwPlayPos) / (pme->m_dwDuration / 1000));
      }
      // TBD: This needs to be fixed.
      // DO NOT set play state in this routine!  
      pme->m_pOwner->m_playState = MP_PLAYSTATE_RECORDING;
    }
    EMIT_MEDIAPLAYER_PLAYSTATE(pme->m_pOwner->m_playState);
    break;

  case MM_CMD_GETTOTALTIME:
    {
      uint32 dwDuration = MPClipTimeInSeconds(pme->m_dwDuration);

      if ((pme->m_dwDuration / 1000) > 0)
      {
        pme->m_wPct = (uint16) ((100 * pme->m_dwPlayPos) / pme->m_dwDuration);
      }
      (void)SNPRINTF(sz, sizeof(sz), " %02d:%02d/%02d:%02d", pme->m_dwPlayPos / 60,
              pme->m_dwPlayPos % 60, dwDuration / 60, dwDuration % 60);
      CPlayerWin_BuildProgCtlMessage(pme, sz);
    }
    break;

  case MM_CMD_GETMEDIAPARM:
    if (nSubCmd == MM_PARM_VOLUME)
    {
      (void)SNPRINTF(sz, sizeof(sz), "[%u]", pme->m_wVolume);
      CPlayerWin_BuildProgCtlMessage(pme, sz);
      bIsTimedMessage = TRUE;
      pme->m_wPct = (100 * pme->m_wVolume) / AEE_MAX_VOLUME;
    }
    else
    {
      pme->m_wPct = 0;
    }
    break;

  case MP_EVENT:
    if(nSubCmd == MP_EVENT_ERROR)
    {
      CPlayerWin_BuildProgCtlMessage(pme, " Error");
    }
    break;

  case MP_EVENT_REDRAW_CTRL:
    if ((pme->m_dwDuration / 1000) > 0)
    {
      pme->m_wPct = (uint16) ((100 * pme->m_dwPlayPos) / (pme->m_dwDuration / 1000));
    }
    break;

  default:
    pme->m_wPct = 0;
    break;
  }

#if defined(FEATURE_APP_MPEG4) && defined(FEATURE_QTV_STREAM_RECORD)
  bShowRecIcon = pme->m_pOwner->m_bRTSPstreamRec;
#else
  bShowRecIcon = NULL;
#endif 

  // CR123032 Don't update the progress control if in full screen mode
  if (pme->m_bFullScreen) 
  {
    return;
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (pme->m_b3GPPTTextOn)
  {
    return;
  }
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT

  /*
   * Update Progress Control object
   */
  MPProgCtl_Update(pme->m_pMPProgCtl, pme->m_pOwner->m_pRecIcon, bShowRecIcon, pme->m_szTimerText, bIsTimedMessage, pme->m_wPct);
}

/*===========================================================================
   Builds message string to be displayed in progress bar. Input string szStr
   is converted to AECHAR type and appended to existing ProgCtl string szTimerText.
===========================================================================*/
static void
CPlayerWin_BuildProgCtlMessage(CPlayerWin *pme, char *szStr)
{
  AECHAR    szBuf[32] = { '\0' };

  (void)STRTOWSTR(szStr, szBuf, sizeof(szBuf));
  (void)WSTRLCAT(pme->m_szTimerText, szBuf, sizeof(szBuf));
}

/*===========================================================================
   This function redraws when the image is fully decoded.
===========================================================================*/
static void
CPlayerWin_ImageNotify(void *pUser, IImage *pImage, AEEImageInfo *pi,
                       int nErr)
{
  CPlayerWin *pme = (CPlayerWin *)pUser;
  boolean     bError = TRUE;

  DBGPRINTF_MED("MP: In CPlayerWin_ImageNotify pUser = 0x%08X, pImage = 0x%08X, pi = 0x%08X, nErr = %d",
    pUser, pImage, pi, nErr);


  if(pme)
  {
    pme->m_bLoadingImage = FALSE;
    if (pme->m_pImage == pImage && !nErr)
    {
      CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
      bError = FALSE;
    }
  }

  if(bError)
  {
    DBGPRINTF_ERROR("MP: ImageNotify handler receives error, pUser 0x%08X, pImage 0x%08X, error code %d",
                    pUser, pImage, nErr);
    if(pme && pme->m_pOwner)
      MP_ErrorDlg(pme->m_pOwner, IDS_ERR_DECODING_IMAGE);

  }
}

/*===========================================================================
   This function restores to default PlayerWin menu.
===========================================================================*/
static void
CPlayerWin_RestoreDefaultMenu(CPlayerWin *pme)
{
  IMenuCtl *pMenu;

  if (pme && pme->m_pPlayerMenu)
  {
    pMenu = pme->m_pPlayerMenu;

    /*
     * remove all current menu items
     */
    (void)IMENUCTL_DeleteAll(pMenu);

    if (CPlayerWin_IsImage(pme))
    {
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FULLSCRN, IDM_PM_FULLSCRN, 0);
    }
    else if (CPlayerWin_IsMedia(pme))
    {
      /*
       * Initialize logo below the header
       */
      (void)CMediaPlayer_SetIdleImage(pme, IDB_LOGO);
      /*
       * Initialize MenuCtl
       */
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_PLAY, IDM_PM_PLAY, 0);
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_STOP, IDM_PM_STOP, 0);
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_REWIND, IDM_PM_REWIND, 0);
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_PAUSE, IDM_PM_PAUSE, 0);
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FF, IDM_PM_FF, 0);
      if (pme->m_eWin == MPPW_RECORD)
      {
        if(!CPlayerWin_PurgeDownloadedFile(pme))
          (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_RECORD, IDM_PM_RECORD, 0);
      }
//#ifdef MP_FEATURE_VIDEO
      else
      {
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_INFO, IDM_PM_INFO, 0);
      }
//#endif     // MP_FEATURE_VIDEO
      if (pme->m_eWin != MPPW_RECORD)
      {
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_BK_FILE, IDM_PM_BK_FILE, 0);
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FF_FILE, IDM_PM_FF_FILE, 0);
      }

#ifdef MP_FEATURE_FAST_PLAYBACK
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_FAST_REPLAY, IDM_PM_FAST_PLAYBACK, 0);
#endif //MP_FEATURE_FAST_PLAYBACK

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_DUAL_MONO_CHANNEL, IDS_AUDIO_CHANNEL, 0);
#endif //FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION

      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_UP, IDM_PM_UP, 0);
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_DOWN, IDM_PM_DOWN, 0);

#ifdef FEATURE_MP_RECORDER_SETTINGS
      if (pme->m_eWin == MPPW_RECORD)
      {
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_SETTINGS, IDM_PM_RECORDER_SETTINGS, 0);
      }
#endif

#ifdef FEATURE_MP_EQ_UI
    if(pme->m_eWin == MPPW_PLAY && pme->m_pOwner->m_bShowPlayerWinSettingUI)
    {
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_SETTINGS, IDM_PM_PLAYERWIN_SETTINGS, 0);
    }

#endif //FEATURE_MP_EQ_UI
    }
  }
}

/*===========================================================================
   This function draws the record menu.
===========================================================================*/
static void
CPlayerWin_DrawRecordMenu(CPlayerWin *pme)
{
  (void)IMENUCTL_DeleteAll(pme->m_pPlayerMenu);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_PLAY, IDM_PM_PLAY, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_STOP, IDM_PM_STOP, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_REWIND, IDM_PM_REWIND, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_PAUSE, IDM_PM_PAUSE, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FF, IDM_PM_FF, 0);
  if(!CPlayerWin_PurgeDownloadedFile(pme))
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_RECORD, IDM_PM_RECORD, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_UP, IDM_PM_UP, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_DOWN, IDM_PM_DOWN, 0);


#ifdef FEATURE_MP_RECORDER_SETTINGS
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_SETTINGS,
      IDM_PM_RECORDER_SETTINGS, 0);
#endif
  if ((!pme->m_bFullScreen) && (!CPlayerWin_PurgeDownloadedFile(pme)))
    IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_RECORD);
  CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
}

 /*===========================================================================
   This function removes items not useable from the record menu
   after recording has started.
===========================================================================*/
static void
CPlayerWin_AdjustRecordMenu(CPlayerWin *pme)
{
  (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_REWIND);
  (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FF);
#ifdef FEATURE_MP_RECORDER_SETTINGS
  (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_RECORDER_SETTINGS);
#endif
  if (!pme->m_bFullScreen)
  {
    DBGPRINTF_MED("MP: Setting the seclected Menu Item to STOP");
    IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_STOP);
  }
  if(CPlayerWin_PurgeDownloadedFile(pme))
    (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_RECORD);
  CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
}

/*===========================================================================
   This function draws the stream recorder menu.
===========================================================================*/
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
static void
CPlayerWin_DrawStreamRecordMenu(CPlayerWin *pme)
{
  IMENUCTL_DeleteAll(pme->m_pPlayerMenu);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_PLAY, IDM_PM_PLAY, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_STOP, IDM_PM_STOP_STREAM_RECORD, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_REWIND, IDM_PM_REWIND, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_PAUSE, IDM_PM_PAUSE, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FF, IDM_PM_FF, 0);
  if(!CPlayerWin_PurgeDownloadedFile(pme))
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_RECORD, IDM_PM_START_STREAM_RECORD, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_UP, IDM_PM_UP, 0);
  (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_DOWN, IDM_PM_DOWN, 0);
  if ((!pme->m_bFullScreen) && (!CPlayerWin_PurgeDownloadedFile(pme)))
    IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_START_STREAM_RECORD);
  CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
}

 /*===========================================================================
   This function removes items not useable from the stream record menu
   after recording has started.
===========================================================================*/
static void
CPlayerWin_AdjustStreamRecordMenu(CPlayerWin *pme)
{
  (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_REWIND);
  (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FF);
  if (!pme->m_bFullScreen)
  {
    DBGPRINTF_MED("MP: Setting the seclected Menu Item to STREAM_STOP");
    IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_STOP_STREAM_RECORD);
  }
  CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
  if(CPlayerWin_PurgeDownloadedFile(pme))
    (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_START_STREAM_RECORD);
}
#endif

/*===========================================================================
   This function starts to play a clip and returns error code.
===========================================================================*/
static void
CPlayerWin_UpdateMenu(CPlayerWin *pme, int nSelectMenuID)
{
  IMenuCtl *pMenu = pme->m_pPlayerMenu;
  uint16    FileHandlerType;
  AEECLSID  ClsId;
  CtlAddItem pai;

#if defined(MP_FEATURE_VIDEO)
  if (pme->m_pmdExList)
  {
    /*
     * all are same type and at least one is present in list
     */
    FileHandlerType =
      CMediaPlayer_FindHandlerType(pme->m_pOwner,
                                   (char *)pme->m_pmdExList[0].pData);
  }
  else
  {
    FileHandlerType =
      CMediaPlayer_FindHandlerType(pme->m_pOwner, (char *)pme->m_pszFile);
  }
#else      // MP_FEATURE_VIDEO
  FileHandlerType =
    CMediaPlayer_FindHandlerType(pme->m_pOwner, (char *)pme->m_md.pData);
#endif     // MP_FEATURE_VIDEO

  (void)IMEDIA_GetClassID(pme->m_pMedia, &ClsId);

#ifdef MP_FEATURE_QTV_DIVX_DRM 
  if((ClsId != AEECLSID_MEDIAMP3) && (ClsId != AEECLSID_MEDIAMPEG4))
  {
    (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_INFO);
    if(pme->m_bDIsplayInfo)
      CPlayerWin_Info_Clear(pme);
  }
#else
  if((ClsId != AEECLSID_MEDIAMP3) && (ClsId != AEECLSID_MEDIAMPEG4))
  {
    (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_INFO);
#ifdef MP_FEATURE_VIDEO
    if(pme->m_bDIsplayInfo)
      CPlayerWin_Info_Clear(pme);
#endif
  }
#endif //MP_FEATURE_QTV_DIVX_DRM

  // We identify DRM content as IDB_FILE_ICON, we cant determine type by extension
  // so set the handler type by checking the media capabilities
#ifdef MP_FEATURE_ICONTENTHANDLER
  if((FileHandlerType == IDB_FILE_ICON) && (pme->m_pOwner->m_bIContentMode))
  {
    int32 capType;

    (void)IMEDIA_GetMediaParm(pme->m_pMedia, MM_PARM_CAPS, (&capType),0);
    if (capType & MM_CAPS_VIDEO)
    {
      DBGPRINTF("MP: DRM file IDB_FILE_ICON, set handler type to VIDEO");
      FileHandlerType = IDB_VIDEO_ICON;
    }
    else if (capType & MM_CAPS_AUDIO)
    {
      DBGPRINTF("MP: DRM file IDB_FILE_ICON, set handler type to AUDIO");
      FileHandlerType = IDB_AUDIO_ICON;
    }
  }
#endif // MP_FEATURE_ICONTENTHANDLER

  if (FileHandlerType == IDB_VIDEO_ICON)
  {
    DBGPRINTF_MED("MP: FileHandlerType = IDB_VIDEO_ICON");
    /*
     * Remove the logo for video clips.
     */
    pme->m_pOwner->m_bShowLogo = FALSE;
    pme->m_pOwner->m_bShowPlayerWinSettingUI = FALSE;

    //Add Full Screen icon if the media has video
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_FULLSCRN,
      IDM_PM_FULLSCRN, 0);

#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
    if (ClsId == AEECLSID_MEDIAMPEG4)
    {
      DBGPRINTF_MED("MP: ClsId = AEECLSID_MEDIAMPEG4");
#ifdef FEATURE_QTV_RANDOM_ACCESS_REPOS
      /*
       * Add menu items for repositioning using the random
       * access atom (MFRA)
       */
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_BK_SKIP, IDM_PM_BK_SKIP, 0);
      (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_FF_SKIP, IDM_PM_FF_SKIP, 0);
#endif     // FEATURE_QTV_RANDOM_ACCESS_REPOS

#ifdef MP_FEATURE_FAST_PLAYBACK
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_FAST_REPLAY, IDM_PM_FAST_PLAYBACK, 0);
#endif //MP_FEATURE_FAST_PLAYBACK

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_DUAL_MONO_CHANNEL, IDS_AUDIO_CHANNEL, 0);
#endif //FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION

      /*
       * Special case: Remove fullscreen button when there is no video
       */

#ifdef FEATURE_MP_EQ
    pme->m_pOwner->m_bShowPlayerWinSettingUI = TRUE;
    pme->m_pOwner->m_bShowLogo = FALSE;
#endif

      if (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_ONLY)
      {
        DBGPRINTF_MED("MP: mpeg4Type = MM_MPEG4_AUDIO_ONLY");
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FULLSCRN);
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_CW_ROTATE);
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_ZOOM_IN);
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_ZOOM_OUT);
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FF_STEP);
      }
      else
      {
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
        /*
         * removed for lack of space
         * MP_AddMenuItem pMenu,,    0, NULL, IDB_CCW_ROTATE, IDM_PM_CCW_ROTATE,   0);
         */
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_CW_ROTATE, IDM_PM_CW_ROTATE, 0);
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_ZOOM_IN, IDM_PM_ZOOM_IN, 0);
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_ZOOM_OUT, IDM_PM_ZOOM_OUT, 0);
#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS ||
           // FEATURE_QTV_MDP_TRANSFORMATIONS

        DBGPRINTF_MED("MP: stream type is %d", pme->m_mediaInfo.streamtype);
        /*
         * If it is not a streaming media, add the buttons for single stepping.
         * For streaming media, these buttons should not appear on the display
         */
        if (pme->m_mediaInfo.streamtype == MM_MEDIA_NON_STREAMING  || pme->m_pOwner->m_bLocalPlayPullMode)
        {
          DBGPRINTF_MED("MP: stream type is MM_MEDIA_NON_STREAMING");
          /*
           * Back Step is not currently supported, so it is disabled for now.
           */
          //MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_BK_STEP, IDM_PM_BK_STEP, 0);
          (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_FF_STEP, IDM_PM_FF_STEP, 0);
        }
      }

      /*
        * If it's live streaming, then remove the ff and rewind buttons.
        */
      if (pme->m_mediaInfo.streamtype == MM_MEDIA_LIVE_STREAMING)
      {
        DBGPRINTF_MED("MP: stream type is MM_MEDIA_LIVE_STREAMING");
        (void)IMENUCTL_DeleteItem(pMenu, IDM_PM_REWIND);
        (void)IMENUCTL_DeleteItem(pMenu, IDM_PM_FF);
      }
      else if (pme->m_mediaInfo.streamtype == MM_MEDIA_STREAMING)
      {
        DBGPRINTF_MED("MP: stream type is MM_MEDIA_STREAMING");
      }
#if defined(FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST)
      else if (pme->m_mediaInfo.streamtype == MM_MEDIA_PV_SERVER_SIDE_PLAYLIST)
      {
        DBGPRINTF_MED("MP: stream type is MM_MEDIA_PV_SERVER_SIDE_PLAYLIST");
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_BK_SKIP, IDM_PM_BK_SKIP, 0);
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_FF_SKIP, IDM_PM_FF_SKIP, 0);
      }
#endif /* FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST */
#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
      else if (pme->m_mediaInfo.streamtype == MM_MEDIA_WM_SERVER_SIDE_PLAYLIST)
      {
        DBGPRINTF_MED("MP: stream type is MM_MEDIA_WM_SERVER_SIDE_PLAYLIST");
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_BK_SKIP, IDM_PM_BK_SKIP, 0);
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_FF_SKIP, IDM_PM_FF_SKIP, 0);
      }
#endif /* FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST */

      /*
        * Special case: Remove volume up and down buttons when there
        * is no audio
        */
      if (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_VIDEO_ONLY)
      {
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_UP);
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_DOWN);
      }
    }                                  // ClsId == AEECLSID_MEDIAMPEG4
#endif //FEATURE_APP_MEDIA_VIDEO

#ifdef MP_FEATURE_FAST_PLAYBACK
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_FAST_REPLAY, IDM_PM_FAST_PLAYBACK, 0);
#endif //MP_FEATURE_FAST_PLAYBACK

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pPlayerMenu, 0, NULL, IDB_DUAL_MONO_CHANNEL, IDS_AUDIO_CHANNEL, 0);
#endif //FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION

#if defined FEATURE_APP_REAL && defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
    if (ClsId == AEECLSID_MEDIAREAL)
    {
      DBGPRINTF_MED("MP: ClsId = AEECLSID_MEDIAREAL");
      if (pme->m_mediaRealInfo.videoType == MM_REAL_VIDEO_NO_VIDEO )
      {
        //Remove fullscreen button when there is no video
        DBGPRINTF_MED("MP: realmedia = MM_REAL_VIDEO_NO_VIDEO");
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FULLSCRN);
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_CW_ROTATE);
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_ZOOM_IN);
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_ZOOM_OUT);
        (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FF_STEP);
      }
      else
      {
        //If the real media has video, then add the rotate, zoom in and zoom out icons.
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_CW_ROTATE, IDM_PM_CW_ROTATE, 0);
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_ZOOM_IN, IDM_PM_ZOOM_IN, 0);
        (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_ZOOM_OUT, IDM_PM_ZOOM_OUT, 0);
      }
    }

#endif
  }                                    // if (FileHandlerType ==
                                       // IDB_VIDEO_ICON)
  else if (FileHandlerType == IDB_AUDIO_ICON)
  {
    DBGPRINTF_MED("MP: FileHandlerType = IDB_AUDIO_ICON");
    (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FULLSCRN);
    (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_CW_ROTATE);
    (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_ZOOM_IN);
    (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_ZOOM_OUT);
    (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FF_STEP);
  }

#ifdef FEATURE_MP_EQ
  pme->m_pOwner->m_bShowPlayerWinSettingUI = TRUE;
  pme->m_pOwner->m_bShowLogo = FALSE;
#endif

  DBGPRINTF_MED("MP: Check Purge eWin %d record %s", pme->m_eWin, 
     ((IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_RECORD, &pai))?"TRUE":"FALSE"));
  if((pme->m_eWin == MPPW_RECORD) && (IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_RECORD, &pai)))
  {
    if(CPlayerWin_PurgeDownloadedFile(pme))
      (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_RECORD);
  }
#ifdef FEATURE_MP_EQ_UI
  // no support for continue mode now
  if(pme->m_eWin == MPPW_PLAY && pme->m_pOwner->m_bShowPlayerWinSettingUI)
  {
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pMenu, 0, NULL, IDB_SETTINGS, IDM_PM_PLAYERWIN_SETTINGS, 0);
  }
#endif //FEATURE_MP_EQ_UI
  DBGPRINTF_MED("MP: Check Purge eWin %d record %s", pme->m_eWin, 
     ((IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_START_STREAM_RECORD, &pai))?"TRUE":"FALSE"));
  if((pme->m_eWin == MPPW_STREAM_RECORD) && (IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_START_STREAM_RECORD, &pai)))
  {
    if(CPlayerWin_PurgeDownloadedFile(pme))
      (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_START_STREAM_RECORD);
  }

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  //Remove the FFWD button if pvx or pd and file not completely downloaded
#ifdef  MM_STATUS_DOWNLOAD_PERCENTAGE
  if (((pme->m_mediaInfo.eFormatType == MM_PVX_FORMAT_TYPE) || (pme->m_mediaInfo.eFormatType == MM_PD_FORMAT_TYPE))
      && ((pme->m_dwDownldPos < pme->m_dwDuration) || (pme->m_dwDownldPct < 100)))
#else
  if ((pme->m_mediaInfo.eFormatType == MM_PVX_FORMAT_TYPE) && (pme->m_dwDownldPos < pme->m_dwDuration))
#endif // MM_STATUS_DOWNLOAD_PERCENTAGE
  {
      (void)IMENUCTL_DeleteItem(pMenu, IDM_PM_FF);
  }
#endif // FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

  if (!pme->m_bFullScreen && (MP_MENU_DEFAULT_SELECTION != nSelectMenuID))
  {
    DBGPRINTF_MED("MP: Setting the seclected Menu Item to %d", nSelectMenuID);
    IMENUCTL_SetSel(pme->m_pPlayerMenu, nSelectMenuID);
  }
#ifdef FEATURE_MMOVERLAY
  CPlayerWin_UpdateMenuOverlay(pme);
#endif // FEATURE_MMOVERLAY
  return;
}

/*===========================================================================
   This function determines if a given MPEG4 media contains video
===========================================================================*/
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#else
static boolean CPlayerWin_IsMPEG4Video(CPlayerWin *pme)

{
  AEECLSID  ClsId;

  if (IMEDIA_GetClassID(pme->m_pMedia, &ClsId) == SUCCESS)
  {
#ifdef FEATURE_APP_MPEG4
    if (ClsId == AEECLSID_MEDIAMPEG4)
    {
      if (pme->m_mediaInfo.mpeg4Type == MM_MPEG4_VIDEO_ONLY ||
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
          pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_VIDEO_TEXT ||
          pme->m_mediaInfo.mpeg4Type == MM_MPEG4_VIDEO_TEXT ||
#endif
          pme->m_mediaInfo.mpeg4Type == MM_MPEG4_AUDIO_VIDEO)
      {
        return TRUE;
      }
    }
#endif //FEATURE_APP_MPEG4
  }
  return FALSE;
}
#endif // FEATURE_APP_MEDIA_VIDEO

/*===========================================================================
   This function displays the Information about current loaded media file.
===========================================================================*/
static boolean
CPlayerWin_Info(CPlayerWin *pme, boolean bDispUpdate)
{
  AECHAR   *pwszInfo = NULL;
  uint16    nSize;

  if (!pme->m_bGotMediaInfo)

  {
    (void)CPlayerWin_GetTotalTimeRequest(pme);
    pme->m_bDisplayInfoPending = TRUE;
    return FALSE;
  }

  if (!pme->m_bDIsplayInfo)
  {
    AEECLSID  ClsId;
    int       result;

    result = IMEDIA_GetClassID(pme->m_pMedia, &ClsId);


    if(result == SUCCESS && ClsId == AEECLSID_MEDIAMP3)
    {
      if(pme->m_pInfoTextCtl == NULL)
      {
        AEEMediaMP3Spec* pSpec;
        char *pszbuf = NULL;

        if (SUCCESS !=
            ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_STATIC, (void **)&pme->m_pInfoTextCtl))
          return FALSE;

        // total string length in AEEMediaMP3Spec + "\r" and end of string
        nSize = (MM_MP3_ID3_TAG_LENGTH*5) + 32 + 
                sizeof("Title: ") + sizeof("Artist: ") + sizeof("Album: ") + sizeof("Year: ") + sizeof("Comment: ");
        
       
        pSpec = (AEEMediaMP3Spec*)pme->m_pAudioSpec;
        if(pSpec)
        {
          char buf[MM_MP3_ID3_TAG_LENGTH + 1];
          int nStrlen;

          nStrlen = MM_MP3_ID3_TAG_LENGTH;
          
          pszbuf = (char*)MALLOC(nSize);
          pwszInfo = (AECHAR *)MALLOC(nSize* sizeof(AECHAR));

          if(pszbuf && pwszInfo)
          {

            (void)MEMSET(pszbuf, 0, nSize);
            (void)MEMSET(pwszInfo, 0, nSize* sizeof(AECHAR));

            (void)STRLCPY(buf, "Title: ", (MM_MP3_ID3_TAG_LENGTH+1));
            (void)STRLCAT(pszbuf, buf, nSize);
            // buf will always have end of string (NULL terminated)
            (void)STRLCPY(buf, pSpec->szTitle, nStrlen); 
            (void)STRLCAT(pszbuf, buf, nSize);
            (void)STRLCPY(buf, "\n", (MM_MP3_ID3_TAG_LENGTH+1));
            (void)STRLCAT(pszbuf, buf, nSize);


            (void)STRLCPY(buf, "Artist: ", (MM_MP3_ID3_TAG_LENGTH+1));
            (void)STRLCAT(pszbuf, buf, nSize);
            (void)STRLCPY(buf, pSpec->szArtist, nStrlen);
            (void)STRLCAT(pszbuf, buf, nSize);            
            (void)STRLCPY(buf, "\n", (MM_MP3_ID3_TAG_LENGTH+1));
            (void)STRLCAT(pszbuf, buf, nSize);


            (void)STRLCPY(buf, "Album: ", (MM_MP3_ID3_TAG_LENGTH+1));
            (void)STRLCAT(pszbuf, buf, nSize);
            (void)STRLCPY(buf, pSpec->szAlbum, nStrlen);
            (void)STRLCAT(pszbuf, buf, nSize);                
            (void)STRLCPY(buf, "\n", (MM_MP3_ID3_TAG_LENGTH+1));
            (void)STRLCAT(pszbuf, buf, nSize);


            (void)STRLCPY(buf, "Year: ", (MM_MP3_ID3_TAG_LENGTH+1));
            (void)STRLCAT(pszbuf, buf, nSize);
            (void)STRLCPY(buf, pSpec->szYear, nStrlen);
            (void)STRLCAT(pszbuf, buf, nSize);   
            (void)STRLCPY(buf, "\n", (MM_MP3_ID3_TAG_LENGTH+1));
            (void)STRLCAT(pszbuf, buf, nSize);


            (void)STRLCPY(buf, "Comment: ", (MM_MP3_ID3_TAG_LENGTH+1));
            (void)STRLCAT(pszbuf, buf, nSize);
            (void)STRLCPY(buf, pSpec->szComment, nStrlen);
            (void)STRLCAT(pszbuf, buf, nSize); 
            (void)STRLCPY(buf, "\n", (MM_MP3_ID3_TAG_LENGTH+1));
            (void)STRLCAT(pszbuf, buf, nSize);

            (void)STRTOWSTR(pszbuf, pwszInfo, nSize*sizeof(AECHAR));
          }
          else
          {
            FREEIF(pszbuf);
            FREEIF(pwszInfo);
            MP_RELEASEIF(pme->m_pInfoTextCtl);
            return FALSE;
          }

          FREEIF(pszbuf);
        }
        else
        {
          MP_RELEASEIF(pme->m_pInfoTextCtl);
          return FALSE;
        }

        ISTATIC_SetRect(pme->m_pInfoTextCtl, &pme->m_rectImage);
        ISTATIC_SetProperties(pme->m_pInfoTextCtl, ST_NOSCROLL);
        (void)ISTATIC_SetText(pme->m_pInfoTextCtl, NULL, pwszInfo, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
        (void)ISTATIC_Redraw(pme->m_pInfoTextCtl);
        MP_FrameRect(pme->m_pIDisplay, &pme->m_rectImage);
        //MP_DRAWHEADER(pme);
        pme->m_bDIsplayInfo = TRUE;
        FREEIF(pwszInfo);
      }
    }
#ifdef FEATURE_APP_MPEG4
    else
    {

      if (CPlayerWin_IsPlaying(pme) || CPlayerWin_IsRecording(pme))
        return FALSE;

      if(!pme->m_pInfoTextCtl)
      {
        if (SUCCESS != ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_STATIC, (void **)&pme->m_pInfoTextCtl))
        return FALSE;

        nSize =
          MM_MPEG4_MAX_AUTHOR_BYTES + MM_MPEG4_MAX_COPYRIGHT_BYTES +
          MM_MPEG4_MAX_RATING_BYTES + MM_MPEG4_MAX_DESCRIPTION_BYTES +
          MM_MPEG4_MAX_VERSION_BYTES + 75;

        pwszInfo = (AECHAR *)MALLOC(nSize);
        if (!pwszInfo)
        {
          MP_RELEASEIF(pme->m_pInfoTextCtl);
          return FALSE;
        }

        (void)MP_PrepareInfoText(pme, pwszInfo, nSize);
      }

      ISTATIC_SetRect(pme->m_pInfoTextCtl, &pme->m_rectImage);
      ISTATIC_SetProperties(pme->m_pInfoTextCtl, ST_NOSCROLL);
      (void)ISTATIC_SetText(pme->m_pInfoTextCtl, NULL, pwszInfo, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
      (void)ISTATIC_Redraw(pme->m_pInfoTextCtl);
      MP_FrameRect(pme->m_pIDisplay, &pme->m_rectImage);
      //MP_DRAWHEADER(pme);
      pme->m_bDIsplayInfo = TRUE;
      FREE(pwszInfo);
    }
#endif //FEATURE_APP_MPEG4
  }
  else
  {
    if (bDispUpdate)
      CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
  }
  return TRUE;
}
#ifdef MP_FEATURE_VIDEO
static void CPlayerWin_Info_Clear(CPlayerWin *pme)
{
  if(pme->m_pInfoTextCtl)
  {
    pme->m_bDIsplayInfo = FALSE;
    MP_RELEASEIF(pme->m_pInfoTextCtl);
    pme->m_pInfoTextCtl = NULL;
    CMediaPlayer_Redraw(pme->m_pOwner, FALSE);
  }
}
#endif //MP_FEATURE_VIDEO

#ifdef FEATURE_APP_MPEG4
static boolean
MP_PrepareInfoText(CPlayerWin *pme, AECHAR *pBuf, uint16 nSize)
{
  AECHAR    szFormat[100];
  AEEMediaMPEG4Spec *pMediaInfo;

  pMediaInfo = &pme->m_mediaInfo;
  if (pMediaInfo->eFormatType == MM_MPEG4_STD_FORMAT_TYPE)
  {
    if (nSize <
        (WSTRLEN(pMediaInfo->ExtendedInfo.StdSpec.szAuthor) +
         WSTRLEN(pMediaInfo->ExtendedInfo.StdSpec.szCopyright) +
         WSTRLEN(pMediaInfo->ExtendedInfo.StdSpec.szDescription) +
         WSTRLEN(pMediaInfo->ExtendedInfo.StdSpec.szVersion) +
         WSTRLEN(pMediaInfo->ExtendedInfo.StdSpec.szRating) + 75))
    {
      return FALSE;
    }
    if (!ISHELL_LoadResString
        (pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDS_PV_INFO_FORMAT, szFormat,
         sizeof(szFormat)))
    {
      return FALSE;
    }

    WSPRINTF(pBuf, nSize, szFormat, pMediaInfo->ExtendedInfo.StdSpec.szAuthor,
             0x0A, pMediaInfo->ExtendedInfo.StdSpec.szVersion, 0x0A,
             pMediaInfo->ExtendedInfo.StdSpec.szRating, 0x0A,
             pMediaInfo->ExtendedInfo.StdSpec.szDescription, 0x0A,
             pMediaInfo->ExtendedInfo.StdSpec.szCopyright);
    return TRUE;
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  else if (pMediaInfo->eFormatType == MM_MPEG4_KDDI_FORMAT_TYPE)
  {
    if (nSize <
        (WSTRLEN(pMediaInfo->ExtendedInfo.KDDISpec.szAuthor) +
         WSTRLEN(pMediaInfo->ExtendedInfo.KDDISpec.szTitle) +
         WSTRLEN(pMediaInfo->ExtendedInfo.KDDISpec.szDeviceName) +
         WSTRLEN(pMediaInfo->ExtendedInfo.KDDISpec.szMemo) +
         WSTRLEN(pMediaInfo->ExtendedInfo.KDDISpec.szCopyright) + 75))
    {
      return FALSE;
    }
    if (!ISHELL_LoadResString
        (pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDS_KDDI_INFO_FORMAT, szFormat,
         sizeof(szFormat)))
    {
      return FALSE;
    }

    WSPRINTF(pBuf, nSize, szFormat, pMediaInfo->ExtendedInfo.KDDISpec.szAuthor,
             0x0A, pMediaInfo->ExtendedInfo.KDDISpec.szTitle, 0x0A,
             pMediaInfo->ExtendedInfo.KDDISpec.szDeviceName, 0x0A,
             pMediaInfo->ExtendedInfo.KDDISpec.szMemo, 0x0A,
             pMediaInfo->ExtendedInfo.KDDISpec.szCopyright);
  }
#endif     // FEATURE_MP4_3GPP_TIMED_TEXT
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  if (pMediaInfo->eFormatType == MM_PVX_FORMAT_TYPE)
  {
    if (nSize <
        (WSTRLEN(pMediaInfo->ExtendedInfo.PvxSpec.StdSpec.szAuthor) +
         WSTRLEN(pMediaInfo->ExtendedInfo.PvxSpec.StdSpec.szCopyright) +
         WSTRLEN(pMediaInfo->ExtendedInfo.PvxSpec.StdSpec.szDescription) +
         WSTRLEN(pMediaInfo->ExtendedInfo.PvxSpec.StdSpec.szVersion) +
         WSTRLEN(pMediaInfo->ExtendedInfo.PvxSpec.StdSpec.szRating) + 75))
    {
      return FALSE;
    }
    if (!ISHELL_LoadResString
        (pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDS_PV_INFO_FORMAT, szFormat,
         sizeof(szFormat)))
    {
      return FALSE;
    }

    WSPRINTF(pBuf, nSize, szFormat, pMediaInfo->ExtendedInfo.PvxSpec.StdSpec.szAuthor,
             0x0A, pMediaInfo->ExtendedInfo.PvxSpec.StdSpec.szVersion, 0x0A,
             pMediaInfo->ExtendedInfo.PvxSpec.StdSpec.szRating, 0x0A,
             pMediaInfo->ExtendedInfo.PvxSpec.StdSpec.szDescription, 0x0A,
             pMediaInfo->ExtendedInfo.PvxSpec.StdSpec.szCopyright);
    return TRUE;
  }
#endif  //FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

  return FALSE;
}
#endif     // FEATURE_APP_MPEG4


#ifdef FEATURE_MP4_3GPP_TIMED_TEXT

/*===========================================================================
   This function create or updates the 3GPPTTextCtl
===========================================================================*/
static void
CPlayerWin_3GPPTText(CPlayerWin *pme, AECHAR *pTTxt)
{
  if (pTTxt)
  {
    int       y,
              dy;
    int       cx = pme->m_pOwner->m_cxWidth;
    int       cy = pme->m_pOwner->m_cyHeight;
    int       cyTTextMain = pme->m_pOwner->m_nNChSize + 2;
    AEERect   rectTText;

    if (!pme->m_b3GPPTTextOn)
    {
      /*
       * TTextMain rect
       */
      y = cy - cyTTextMain - pme->m_pOwner->m_IconViewCtlDy - 2;
      dy = cyTTextMain;
      SETAEERECT(&rectTText, 0, y, cx, dy);

      /*
       * Initialize TTextCtl
       */
      pme->m_TTextCtl.m_pIShell = pme->m_pIShell;
      pme->m_TTextCtl.m_pIDisplay = pme->m_pIDisplay;
      if (!C3GPPTTextCtl_Init(&pme->m_TTextCtl, &rectTText))
      {
      	DBGPRINTF_MED("MP: Calling MP_RELEASEWIN");
        MP_RELEASEWIN(pme);
        return;
      }
      pme->m_b3GPPTTextOn = TRUE;
    }
    C3GPPTTextCtl_Update(&pme->m_TTextCtl, pTTxt);
    // Cancel any pending MPProgCtl timer as it will overwrite
    MPProgCtl_MessageTimerCancel(pme->m_pMPProgCtl);
  }
  else
  {
    if (pme->m_b3GPPTTextOn)
    {
      pme->m_b3GPPTTextOn = FALSE;
      /*
       * clear any pending text
       */
      C3GPPTTextCtl_Update(&pme->m_TTextCtl, NULL);
      C3GPPTTextCtl_Release(&pme->m_TTextCtl);
      /*
       * now we can display progress bar
       */
      CPlayerWin_UpdateProgCtl(pme, 0, 0, 0);
    }
  }
  return;
}


/*===========================================================================
   3GPP timed text has expired, so remove the text.
===========================================================================*/
static void
CPlayerWin_3GPPTimedTextExpiredNotify(CPlayerWin *pme)
{
  CPlayerWin_3GPPTText(pme, NULL);
}

#endif     // FEATURE_MP4_3GPP_TIMED_TEXT

/*===========================================================================
   This function releases IBase.
===========================================================================*/
void MP_FreeIF(IBase ** ppif)
{
  if (ppif && *ppif)
  {
    (void)IBASE_Release(*ppif);
    *ppif = NULL;
  }
}

/*===========================================================================
   This function releases IBase.
===========================================================================*/

void MP_FreeMenu(CMediaPlayer * pme, IBase ** ppif)
{
  if (NULL == pme)
    return;
  if (ppif && *ppif)
  {
    // if the menu associated with the current key repeat timer is being released,
    // then disassociate the timer event from the menu.
    if (((IBase *)pme->m_KeyStore.m_pICtl) == *ppif)
    {
      DBGPRINTF_MED("MP: Releasing menu with timer still active. Cancelling MP_OnKeyRepeatEvent Timer");
      (void)ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)MP_OnKeyRepeatEvent,
                                               (void *)&pme->m_KeyStore);
      pme->m_KeyStore.m_wParam_Key = 0;
      pme->m_KeyStore.m_dwParam_Key = 0;
      pme->m_KeyStore.m_pICtl = NULL;
    }
    MP_FreeIF(ppif);
  }
}

/*===========================================================================
   This function releases IWindow.
===========================================================================*/
void MP_FreeWin(IWindow ** ppif)
{
  if (ppif && *ppif)
  {
  	DBGPRINTF_MED("MP: Calling IWINDOW_Delete");
    IWINDOW_Delete(*ppif);
    *ppif = NULL;
  }
}

/*===========================================================================
   This function releases IBitmap.
===========================================================================*/
void MP_FreeBitmap(IBitmap ** ppif)
{
  if (ppif && *ppif)
  {
  	DBGPRINTF_MED("MP: Calling IBITMAP_Release");
    IBITMAP_Release(*ppif);
    *ppif = NULL;
  }
}

#ifdef FEATURE_MMOVERLAY
/*===========================================================================
   This function releases IMMOverlay.
===========================================================================*/
void MP_FreeOverlay(IMMOverlay ** ppif)
{
  if (ppif && *ppif)
  {
  	DBGPRINTF_MED("MP: Calling IMMOVERLAY_Release");
    IMMOVERLAY_Release(*ppif);
    *ppif = NULL;
  }
}

/*===========================================================================
   This function releases IMMOverlayMgr.
===========================================================================*/
void MP_FreeOverlayMgr(IMMOverlayMgr ** ppif)
{
  if (ppif && *ppif)
  {
  	DBGPRINTF_MED("MP: Calling IMMOVERLAYMGR_Release");
    IMMOVERLAYMGR_Release(*ppif);
    *ppif = NULL;
  }
}
#endif // FEATURE_MMOVERLAY

/******************************************************************************/

/*******                       Settings Menu                            *******/

/******************************************************************************/
static void
CMediaPlayer_Settings(CMediaPlayer *pme)
{
  int       returnStatus;

	DBGPRINTF_MED("MP: Calling MP_RELEASEWIN");
  MP_RELEASEWIN(pme->m_pWin);
  returnStatus =
    ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_SETTINGS,
                        NULL);
  DBGPRINTF_MED("MP: ISHELL_CreateDialog() returns %d = %s",
    returnStatus, MP_ErrorStateToString(returnStatus));
}


static boolean CMediaPlayer_SettingsUpdateVolMenu(CMediaPlayer *pme)
{
  //CtlAddItem pai;
  //AEERect rect;
  IMenuCtl *pICurrentMenu = NULL;
  IDialog  *pIActiveDialog = NULL;
  uint16    wDialogId = 0;
  uint16    wItemId = 0;

  DBGPRINTF_MED("MP: Trying to create dynamic Volume settings Menu");

  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  if (NULL == pIActiveDialog)
  {
    DBGPRINTF_ERROR("MP: Unable to get Active Dialog");
    return FALSE;
  }
  wDialogId = IDIALOG_GetID(pIActiveDialog);


  if (IDD_VOLUME_SETTINGS != wDialogId)
  {
    // We got here when whe should not have
    DBGPRINTF_ERROR("MP: Got a dialog ID of %d, expecting %d", wDialogId, IDD_VOLUME_SETTINGS);
    return FALSE;
  }
  pICurrentMenu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, IDC_VOLUME_MENU);
  if (NULL == pICurrentMenu)
  {
    DBGPRINTF_ERROR("MP: Unable to get Active Dialog");
    return FALSE;
  }

  //clean up old menu
  IMENUCTL_Reset(pICurrentMenu);
  (void)IMENUCTL_DeleteAll(pICurrentMenu);

  //Set the Rectangle for the Menu
  //SETAEERECT(&rect, 0, 0, pme->m_cxWidth, pme->m_cyHeight);
  //MP_SetMenuAttr(pICurrentMenu, AEECLSID_MENUCTL, pme->m_nColorDepth, &rect, 0);

  // Set Title and init data structure
  (void)IMENUCTL_SetTitle(pICurrentMenu, MEDIAPLAYER_RES_FILE, IDS_VOLUME_SETTINGS, NULL);

  (void)MP_AddMenuItem(pICurrentMenu, IDS_SDAC_OFF, NULL, IDB_RADIO_OFF, IDDS_VOLUME_OFF, 0);
  (void)MP_AddMenuItem(pICurrentMenu, IDS_SINGLE_LEVEL, NULL, IDB_RADIO_OFF, IDDS_VOLUME_ONE, 0);
  (void)MP_AddMenuItem(pICurrentMenu, IDS_SECOND_LEVEL, NULL, IDB_RADIO_OFF, IDDS_VOLUME_TWO, 0);
  (void)MP_AddMenuItem(pICurrentMenu, IDS_THIRD_LEVEL, NULL, IDB_RADIO_OFF, IDDS_VOLUME_THREE, 0);
  (void)MP_AddMenuItem(pICurrentMenu, IDS_VOLUME_FOUR, NULL, IDB_RADIO_OFF, IDDS_VOLUME_FOUR, 0);
  if (pme->m_wNumVolumeLevels > 4)
  {
    (void)MP_AddMenuItem(pICurrentMenu, IDS_VOLUME_FIVE, NULL, IDB_RADIO_OFF, IDDS_VOLUME_FIVE, 0);
  }
  if (pme->m_wNumVolumeLevels > 5)
  {
    (void)MP_AddMenuItem(pICurrentMenu, IDS_VOLUME_SIX, NULL, IDB_RADIO_OFF, IDDS_VOLUME_SIX, 0);
  }
  if (pme->m_wNumVolumeLevels > 6)
  {
    (void)MP_AddMenuItem(pICurrentMenu, IDS_VOLUME_SEVEN, NULL, IDB_RADIO_OFF, IDDS_VOLUME_SEVEN, 0);
  }
  if (pme->m_wNumVolumeLevels > 7)
  {
    (void)MP_AddMenuItem(pICurrentMenu, IDS_VOLUME_EIGHT, NULL, IDB_RADIO_OFF, IDDS_VOLUME_EIGHT, 0);
  }
  if (pme->m_wNumVolumeLevels > 8)
  {
    (void)MP_AddMenuItem(pICurrentMenu, IDS_VOLUME_NINE, NULL, IDB_RADIO_OFF, IDDS_VOLUME_NINE, 0);
  }
  if (pme->m_wNumVolumeLevels > 9)
  {
    (void)MP_AddMenuItem(pICurrentMenu, IDS_VOLUME_TEN, NULL, IDB_RADIO_OFF, IDDS_VOLUME_TEN, 0);
  }

  // Make sure that our volume level is never higher than the number of levels
  if (pme->m_wVolumeLevel > pme->m_wNumVolumeLevels)
  {
    pme->m_wVolumeLevel = pme->m_wNumVolumeLevels;
  }
  // Set the current volume level as the selected option
  wItemId = GetCurrentSettings(pme, wDialogId);
  SetRadioButtons(pme, pICurrentMenu, wItemId);

  (void)IMENUCTL_Redraw(pICurrentMenu);
  IMENUCTL_SetActive(pICurrentMenu, TRUE);

  return TRUE;
}

/******************************************************************************/

/*******              Settings Dialog Event Handler                     *******/

/******************************************************************************/
static boolean
SettingsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CMediaPlayer *pme = (CMediaPlayer *)pUser;
  IMenuCtl *pICurrentMenu;

  switch (evt)
  {
  case EVT_DIALOG_START:
    pICurrentMenu =
      (IMenuCtl *)IDIALOG_GetControl((IDialog *)dw, IDC_MENU_SETTINGS);
    SetSettingsMenuStyle(pme, pICurrentMenu);
#ifdef FEATURE_MP_RESOURCE_MERGE
    (void)IMENUCTL_DeleteAll(pICurrentMenu);

    (void)MP_AddMenuItem(pICurrentMenu, IDS_VOLUME_SETTINGS, NULL, IDB_PLAY,
                   IDS_VOLUME_SETTINGS, 0);
#ifdef FEATURE_MP_SEEK_TIME_DLG
    (void)MP_AddMenuItem(pICurrentMenu, IDM_SEEK_TIME_SETTINGS, NULL, IDB_PLAY,
                   IDM_SEEK_TIME_SETTINGS, 0);
#endif

#ifdef FEATURE_MP4_LCD_CONTRAST
    (void)MP_AddMenuItem(pICurrentMenu, IDS_CONTRAST_SETTINGS, NULL, IDB_PLAY,
                   IDS_CONTRAST_SETTINGS, 0);
#endif     // FEATURE_MP4_LCD_CONTRAST
    (void)MP_AddMenuItem(pICurrentMenu, IDS_CMODE, NULL, IDB_PLAY, IDS_CMODE, 0);
    (void)MP_AddMenuItem(pICurrentMenu, IDS_AUTOPLAY, NULL, IDB_PLAY, IDS_AUTOPLAY, 0);

#if defined(FEATURE_MP_RECORDER_SETTINGS)
    (void)MP_AddMenuItem(pICurrentMenu, IDM_PM_RECORDER_SETTINGS, NULL, IDB_PLAY,
                   IDM_PM_RECORDER_SETTINGS, 0);
#endif

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    (void)MP_AddMenuItem(pICurrentMenu, IDS_PS_BW_TITLE, NULL, IDB_PLAY,
                   IDS_PS_BW_TITLE, 0);
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    (void)MP_AddMenuItem(pICurrentMenu, IDM_3GPP_SETTINGS, NULL, IDB_PLAY,
                   IDM_3GPP_SETTINGS, 0);
#endif

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
    (void)MP_AddMenuItem(pICurrentMenu, IDS_RTSP_STR_REC, NULL, IDB_PLAY,
                   IDS_RTSP_STR_REC, 0);
#endif

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    (void)MP_AddMenuItem(pICurrentMenu, IDS_KDDI_EXTENSIONS, NULL, IDB_PLAY,
                   IDS_KDDI_EXTENSIONS, 0);
#endif

#if defined(MP_FEATURE_VIDEO) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))
    (void)MP_AddMenuItem(pICurrentMenu, IDS_QOS_UMTS, NULL, IDB_PLAY,
      IDS_QOS_UMTS, 0);
    (void)MP_AddMenuItem(pICurrentMenu, IDS_CHOOSE_PROFILE, NULL, IDB_PLAY,
      IDS_CHOOSE_PROFILE, 0);
#endif

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
    (void)MP_AddMenuItem(pICurrentMenu, IDS_ICONTENT, NULL, IDB_PLAY, IDS_ICONTENT, 0);
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

    (void)MP_AddMenuItem(pICurrentMenu, IDS_BUFFER, NULL, IDB_PLAY, IDS_BUFFER, 0);
    (void)MP_AddMenuItem(pICurrentMenu, IDS_PLAY_IN_BACKGROUND, NULL, IDB_PLAY, IDS_PLAY_IN_BACKGROUND, 0);
    (void)MP_AddMenuItem(pICurrentMenu, IDS_REDRAW_AT_END, NULL, IDB_PLAY, IDS_REDRAW_AT_END, 0);

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    (void)MP_AddMenuItem(pICurrentMenu, IDS_PVXSAVELOC, NULL, IDB_PLAY, IDS_PVXSAVELOC, 0);
#endif

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    (void)MP_AddMenuItem(pICurrentMenu, IDS_HTTP_DOWNLOAD, NULL, IDB_PLAY, IDS_HTTP_DOWNLOAD, 0);
#endif

#ifdef FEATURE_QTV_MDP_ASCALE
    (void)MP_AddMenuItem(pICurrentMenu, IDM_ASCALE_SETTINGS, NULL, IDB_PLAY, IDM_ASCALE_SETTINGS, 0);
#endif

#ifdef MP_FEATURE_QTV_DIVX_DRM  
    (void)MP_AddMenuItem(pICurrentMenu, IDM_DIVX_REG_INFO, NULL, IDB_PLAY, IDM_DIVX_REG_INFO, 0);
#endif

#ifdef FEATURE_MP_KEY_MAPPING
    (void)MP_AddMenuItem(pICurrentMenu, IDM_KEYMAPPINGS, NULL, IDB_PLAY, IDM_KEYMAPPINGS, 0);
#endif

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
#if defined(FEATURE_QTV_HTTP_DOWNLOAD)
    (void)MP_AddMenuItem(pICurrentMenu, IDS_HTTP_SOURCE, NULL, IDB_PLAY, IDS_HTTP_SOURCE, 0);
#endif
    (void)MP_AddMenuItem(pICurrentMenu, IDS_HTTP_MIMIC, NULL, IDB_PLAY, IDS_HTTP_MIMIC, 0);
#endif

#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
    (void)MP_AddMenuItem(pICurrentMenu, IDS_PLAY_RINGER_AND_QTV, NULL, IDB_PLAY, IDS_PLAY_RINGER_AND_QTV, 0);
    (void)MP_AddMenuItem(pICurrentMenu, IDS_PLAY_RINGER_NO_QTV, NULL, IDB_PLAY, IDS_PLAY_RINGER_NO_QTV, 0);
#endif

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    (void)MP_AddMenuItem(pICurrentMenu, IDS_PROG_DOWNLOAD_LOC, NULL, IDB_PLAY, IDS_PROG_DOWNLOAD_LOC, 0);
#endif

#ifdef FEATURE_MMOVERLAY
    (void)MP_AddMenuItem(pICurrentMenu, IDS_OVERLAYS, NULL, IDB_PLAY, IDS_OVERLAYS, 0);
#endif // FEATURE_MMOVERLAY

    (void)MP_AddMenuItem(pICurrentMenu, IDS_HIGH_PERFORMANCE_MODE, NULL, IDB_PLAY, IDS_HIGH_PERFORMANCE_MODE, 0);

#ifdef FEATURE_APP_MPEG4
    (void)MP_AddMenuItem(pICurrentMenu, IDS_QTV_SUSPEND_MODE, NULL, IDB_PLAY, IDS_QTV_SUSPEND_MODE, 0);
#endif

#ifdef FEATURE_MMOVERLAY
    (void)MP_AddMenuItem(pICurrentMenu, IDM_ALPHA_BLEND_SETTINGS, NULL, IDB_PLAY, IDM_ALPHA_BLEND_SETTINGS, 0);
#endif // FEATURE_MMOVERLAY

    (void)MP_AddMenuItem(pICurrentMenu, IDS_LOCAL_PLAY_PULL_MODE, NULL, IDB_PLAY, IDS_LOCAL_PLAY_PULL_MODE, 0);

    (void)IMENUCTL_Redraw(pICurrentMenu);
#endif     // FEATURE_MP_RESOURCE_MERGE
    return TRUE;

  case EVT_KEY:
    switch (w)
    {
    case AVK_CLR:

      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      (void)CMediaPlayer_SetWindow(pme, MPW_MAIN, 0);
      return TRUE;
    }

    return TRUE;

  case EVT_COMMAND:
    switch (w)
    {
#ifdef FEATURE_MP_RESOURCE_MERGE
    case IDS_CMODE:
#else
    case IDDS_CONT:
#endif     // FEATURE_MP_RESOURCE_MERGE
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_CONT_MODE, NULL);
      return TRUE;

#ifdef FEATURE_MP_RESOURCE_MERGE
    case IDS_AUTOPLAY:
#else
    case IDDS_AUTOPLAY:
#endif     // FEATURE_MP_RESOURCE_MERGE
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_AUTOPLAY, NULL);
      return TRUE;

#ifdef FEATURE_MP_RESOURCE_MERGE
    case IDS_VOLUME_SETTINGS:
#else
    case IDDS_VOLUME:
#endif     // FEATURE_MP_RESOURCE_MERGE
      DBGPRINTF_MED("MP: Creating Volume Settings menu/dialog");
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_VOLUME_SETTINGS, NULL);
      (void)CMediaPlayer_SettingsUpdateVolMenu(pme);
      return TRUE;

#ifdef FEATURE_MP_SEEK_TIME_DLG
    case IDM_SEEK_TIME_SETTINGS:
      if (SUCCESS != ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_SEEK_TIME_SETTTING, NULL))
        DBGPRINTF_ERROR("MP: cannot create seek time setting dlg.");
      return TRUE;
#endif

#ifdef FEATURE_QTV_MDP_ASCALE
    case IDM_ASCALE_SETTINGS:
      if (SUCCESS != ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_ASCALE_SETTINGS, NULL))
        DBGPRINTF_ERROR("MP: cannot create AScale Settings dlg.");
      return TRUE;
#endif

#ifdef MP_FEATURE_QTV_DIVX_DRM  
    case IDM_DIVX_REG_INFO:
      if (SUCCESS != ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_DIVX_REG_INFO, NULL))
        DBGPRINTF_ERROR("MP: cannot create Divx Registration dlg.");
      return TRUE;
#endif

    case IDM_KEYMAPPINGS:
      if (SUCCESS != ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_KEYMAPPINGS, NULL))
        DBGPRINTF_ERROR("MP: cannot create Key Mappings dlg.");
      return TRUE;

#ifdef FEATURE_MP_RECORDER_SETTINGS

#ifdef FEATURE_MP_RESOURCE_MERGE
    case IDM_PM_RECORDER_SETTINGS:
#else
    case IDDS_RECORDER_SETTINGS:
#endif     // FEATURE_MP_RESOURCE_MERGE
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      if (CMediaPlayer_SetWindow(pme, MPW_RECORDER_SETTINGS, 0))
        ((CRecorderSettingWin *)pme->m_pWin)->m_ParentWin = IDM_MAIN_SETTINGS;
      else
      {
        DBGPRINTF_ERROR("MP: Error on creating recorder setting window.");
        /*
         * recreate main setting window
         */
        CMediaPlayer_Settings(pme);
        return TRUE;
      }
      break;
#endif

#ifdef FEATURE_MP4_LCD_CONTRAST

#ifdef FEATURE_MP_RESOURCE_MERGE
    case IDS_CONTRAST_SETTINGS:
#else
    case IDDS_CONTRAST:
#endif     // FEATURE_MP_RESOURCE_MERGE
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      (void)CMediaPlayer_SetWindow(pme, MPW_CONTRAST, 0);
      return TRUE;
#endif     // FEATURE_MP4_LCD_CONTRAST

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
#ifdef FEATURE_MP_RESOURCE_MERGE
    case IDS_PS_BW_TITLE:
#else
    case IDDS_PS:
#endif     // FEATURE_MP_RESOURCE_MERGE
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE,
                          IDD_PSEUDO_STR_SETTINGS, NULL);
      return TRUE;

#ifdef FEATURE_MP_RESOURCE_MERGE
    case IDS_KDDI_EXTENSIONS:
#else
    case IDDS_KDDI_EXTENSIONS:
#endif     // FEATURE_MP_RESOURCE_MERGE
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE,
                          IDD_KDDI_EXTENSIONS, NULL);
      return TRUE;
#endif //FEATURE_APP_MPEG4 && FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    case IDM_3GPP_SETTINGS:
      if (SUCCESS !=
          ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE,
                              IDD_3GPP_SETTINGS, NULL))
        DBGPRINTF_ERROR("MP: Cannot create 3GPP settings dialog!");
      return TRUE;
#endif

#if defined(MP_FEATURE_VIDEO) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))

#ifdef FEATURE_MP_RESOURCE_MERGE
    case IDS_QOS_UMTS:
#else
    case IDDS_QOS:
#endif
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE,
                          IDD_QOS_CONFIG, NULL);
      return TRUE;

#ifdef FEATURE_MP_RESOURCE_MERGE
    case IDS_CHOOSE_PROFILE:
#else
    case IDDS_QOS_CTRL:
#endif

      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE,
                          IDD_QOS_UMTS_SETTINGS, NULL);
      return TRUE;
#endif

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)

#ifdef FEATURE_MP_RESOURCE_MERGE
    case IDS_RTSP_STR_REC:
#else
    case IDDS_RTSP_STR_REC:
#endif
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE,
                          IDD_RTSP_STR_REC, NULL);
      return TRUE;
#endif

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
    case IDS_ICONTENT:
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_ICONTENT,
                          NULL);
      return TRUE;
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

    case IDS_BUFFER:
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_BUFFER,
                          NULL);
      return TRUE;

    case IDS_PLAY_IN_BACKGROUND:
      {
        int nRet = ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_PLAY_IN_BACKGROUND,
                          NULL);
        DBGPRINTF_MED("MP: CreateDialog of Play in Background returns %d", nRet);
      }
      return TRUE;

    case IDS_REDRAW_AT_END:
      {
        int nRet = ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_REDRAW_AT_END,
                          NULL);
        DBGPRINTF_MED("MP: CreateDialog of Clear Window after Clip Ends returns %d", nRet);
      }
      return TRUE;

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    case IDS_PVXSAVELOC:
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_PVXSAVELOC,
                          NULL);
      return TRUE;
#endif

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    case IDS_HTTP_DOWNLOAD:
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_HTTP_DOWNLOAD,
                          NULL);
      return TRUE;
#endif

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
    case IDS_HTTP_SOURCE:
      {
        int nRet = ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_HTTP_SOURCE,
                          NULL);
        DBGPRINTF_MED("MP: CreateDialog of HTTP Source returns %d", nRet);
      }
      return TRUE;

    case IDS_HTTP_MIMIC:
      {
        int nRet = ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_HTTP_MIMIC,
                          NULL);
        DBGPRINTF_MED("MP: CreateDialog of Mimic encrypted download returns %d", nRet);
      }
      return TRUE;
#endif  // FEATURE_APP_MPEG4 && (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)

#ifdef FEATURE_APP_MPEG4
    case IDS_QTV_SUSPEND_MODE:
      {
        int nRet = ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_QTV_SUSPEND_MODE, NULL);
        DBGPRINTF_MED("MP: CreateDialog of QTV Suspend Mode returns %d", nRet);
      }
      return TRUE;
#endif

#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
    case IDS_PLAY_RINGER_AND_QTV:
      {
        int nRet = ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_PLAY_RINGER_AND_QTV, NULL);
        DBGPRINTF_MED("MP: CreateDialog of Play Ringer and QTV returns %d", nRet);
      }
      return TRUE;

    case IDS_PLAY_RINGER_NO_QTV:
      {
        int nRet = ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_PLAY_RINGER_NO_QTV, NULL);
        DBGPRINTF_MED("MP: CreateDialog of Play Ringer with no QTV returns %d", nRet);
      }
      return TRUE;

#endif //(FEATURE_APP_MPEG4) && (FEATURE_QTV_AUDIO_RINGER)

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    case IDS_PROG_DOWNLOAD_LOC:
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_PROG_DL_SAVE_LOC, NULL);
      return TRUE;
#endif

#ifdef FEATURE_MMOVERLAY
    case IDS_OVERLAYS:
      DBGPRINTF_MED("MP: Creating Overlays Dialog");
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_OVERLAYS, NULL);
      return TRUE;
#endif // FEATURE_MMOVERLAY

    case IDS_HIGH_PERFORMANCE_MODE:
      DBGPRINTF_MED("MP: Creating High Performance Mode Dialog");
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_HIGH_PERFORMANCE_MODE, NULL);
      return TRUE;

#ifdef FEATURE_MMOVERLAY
    case IDM_ALPHA_BLEND_SETTINGS:
      if (SUCCESS != ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_ALPHA_BLEND_SETTING, NULL))
        DBGPRINTF_ERROR("MP: cannot create Alpha Blend setting dlg.");
      return TRUE;
#endif // FEATURE_MMOVERLAY

    case IDS_LOCAL_PLAY_PULL_MODE:
      DBGPRINTF_MED("MP: Creating Local Play PULL Mode Dialog");
      (void)ISHELL_CreateDialog(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDD_LOCAL_PLAY_PULL_MODE, NULL);
      return TRUE;
   }
   return TRUE;

  case EVT_DIALOG_END:

    return TRUE;
  }
  return FALSE;                        // no menu control ----> error

}

/******************************************************************************/

/*******            Radio Button Dialog Event Handler                   *******/

/******************************************************************************/
static boolean
RadioButtonDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CMediaPlayer *pme = (CMediaPlayer *)pUser;
  static IMenuCtl *pICurrentMenu;
  IDialog  *pIActiveDialog;
  uint16    wDialogId,
            wSelId,
            wItemId;
  int       i = 0;

  switch (evt)
  {
  case EVT_DIALOG_START:
    pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
    wDialogId = IDIALOG_GetID(pIActiveDialog);

    for (i = 0; i < ARR_SIZE(LookupMenuCtlID); i++)
    {
      if (LookupMenuCtlID[i].wDialogID == wDialogId)
        pICurrentMenu =
          (IMenuCtl *)IDIALOG_GetControl((IDialog *)dw,
                                          LookupMenuCtlID[i].wCtrlID);
    }
    if (pICurrentMenu)
    {
      SetSettingsMenuStyle(pme, pICurrentMenu);
      wItemId = GetCurrentSettings(pme, wDialogId);
      SetRadioButtons(pme, pICurrentMenu, wItemId);
    }
    return TRUE;
  case EVT_KEY:
    switch (w)
    {
    case AVK_CLR:
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    }
    return TRUE;
  case EVT_COMMAND:
    wSelId = IMENUCTL_GetSel(pICurrentMenu);
    UpdateRadioButtons(pme, wSelId, pICurrentMenu);
    TranslateResourceIDToSetting(pme, wSelId);
    (void)ISHELL_SetTimer(pme->a.m_pIShell, 250, (PFNNOTIFY)EndCurrentDlg,
                    (uint32 *)pme);
    return TRUE;
  case EVT_DIALOG_END:
    return TRUE;
  }
  return FALSE;                        // no menu control ----> error
}

#ifdef FEATURE_MP_SEEK_TIME_DLG

/******************************************************************************/

/*******            Seek Time Setting Dialog Event Handler              *******/

/******************************************************************************/
static boolean
SeekTimeSettingDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CMediaPlayer *pme = (CMediaPlayer *)pUser;
  IDialog  *pIActiveDialog;
  uint16    wDialogId;
  IMenuCtl *menu;
  ITextCtl *ctrl;
  /* SetDefaultSoftkeyLook() is from apps/core/AppComFunc.c */
  extern void SetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm);

  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  wDialogId = IDIALOG_GetID(pIActiveDialog);
  DBGPRINTF_MED("MP: IDIALOG_GetID returns %d", wDialogId);
  ctrl = (ITextCtl *)IDIALOG_GetControl(pIActiveDialog, IDD_SEEK_TIME_INPUT);
  if (NULL == ctrl)
    return FALSE;
  menu =
    (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog,
                                    IDD_SEEK_TIME_SOFTKEY_BUTTON);
  if (menu == NULL)
    return FALSE;

  switch (evt)
  {
  case EVT_DIALOG_START:
  {
    AEERect   textRect;
    char      buf[32];
    AECHAR    seekTime[32];
    uint32    charHight;

    (void)IMENUCTL_DeleteAll(menu);
    (void)IMENUCTL_Redraw(menu);

    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_DEFAULT,
                     IDD_SEEK_TIME_SOFTKEY_DEFAULT, NULL, 0);
    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_OK,
                     IDD_SEEK_TIME_SOFTKEY_OK, NULL, 0);
    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_CANCEL,
                     IDD_SEEK_TIME_SOFTKEY_CANCEL, NULL, 0);

    /*
     * Remove the multiline property
     */
    ITEXTCTL_SetProperties(ctrl,
                           ITEXTCTL_GetProperties(ctrl) & ~TP_MULTILINE |
                           TP_FRAME);
    SetDefaultSoftkeyLook(pme->a.m_pIShell, menu);

    /*
     * set seek time to display
     */
    (void)SNPRINTF(buf, sizeof(buf), "%d", pme->m_nSeekTime);
    (void)STRTOWSTR(buf, seekTime, sizeof(seekTime));
    (void)ITEXTCTL_SetText(ctrl, seekTime, -1);

    /*
     * set pos
     */
    ITEXTCTL_GetRect(ctrl, &textRect);
    charHight =
      IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);
    textRect.x = 4;
    textRect.dx -= 8;
    textRect.y = 1;
    textRect.dy = charHight;
    ITEXTCTL_SetRect(ctrl, &textRect);

    ITEXTCTL_SetMaxSize(ctrl, MP_SEEK_TIME_DLG_INPUT_LENGTH);
    (void)ITEXTCTL_SetInputMode(ctrl, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(ctrl, TRUE);
    return TRUE;
  }
  case EVT_KEY:
    switch (w)
    {
    case AVK_CLR:
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    default:
      return FALSE;
    }
  case EVT_DIALOG_END:
    return TRUE;
  case EVT_COMMAND:
    switch (w)
    {
    case IDD_SEEK_TIME_SOFTKEY_DEFAULT:
    {
      char      buf[32];
      AECHAR    seekTime[32];

      /*
       * set back to default seek time
       */
      (void)SNPRINTF(buf, sizeof(buf), "%d", MP_SEEK_TIME);
      (void)STRTOWSTR(buf, seekTime, sizeof(seekTime));
      (void)ITEXTCTL_SetText(ctrl, seekTime, -1);
      return TRUE;
    }
    case IDD_SEEK_TIME_SOFTKEY_OK:
    {
      char      buf[32];
      int32     nNum;
      AECHAR   *pText = ITEXTCTL_GetTextPtr(ctrl);

      /*
       * no more than MP_SEEK_TIME_DLG_INPUT_LENGTH digits
       */
      if (pText && WSTRLEN(pText) <= MP_SEEK_TIME_DLG_INPUT_LENGTH)
      {
        (void)WSTRTOSTR(pText, buf, sizeof(buf));
        nNum = ATOI(buf);
        if (nNum > 0)
        {
          pme->m_nSeekTime = nNum;
          (void)ISHELL_EndDialog(pme->a.m_pIShell);
        }
      }
      return TRUE;
    }
    case IDD_SEEK_TIME_SOFTKEY_CANCEL:
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    }
  }
  return FALSE;
}
#endif     // FEATURE_MP_SEEK_TIME_DLG

#ifdef FEATURE_QTV_MDP_ASCALE
/******************************************************************************/

/*******            AScale Settings Dialog Event Handler                *******/

/******************************************************************************/
static boolean
AScaleSettingDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CMediaPlayer *pme = (CMediaPlayer *)pUser;
  IDialog  *pIActiveDialog;
  uint16    wDialogId;
  IMenuCtl *menu;
  ITextCtl *xCtrl;
  ITextCtl *yCtrl;

  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  wDialogId = IDIALOG_GetID(pIActiveDialog);
  DBGPRINTF_MED("MP: IDIALOG_GetID returns %d", wDialogId);

  xCtrl = (ITextCtl *)IDIALOG_GetControl(pIActiveDialog, IDD_ASCALE_X_VALUE_INPUT);
  if (NULL == xCtrl)
  {
    return FALSE;
  }

  yCtrl = (ITextCtl *)IDIALOG_GetControl(pIActiveDialog, IDD_ASCALE_Y_VALUE_INPUT);
  if (NULL == yCtrl)
  {
    return FALSE;
  }

  menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, IDD_ASCALE_SOFTKEY_BUTTON);
  if (menu == NULL)
  {
    return FALSE;
  }

  switch (evt)
  {
  case EVT_DIALOG_START:
  {
    AEERect   textRect;
    char      buf[32];
    AECHAR    ascaleXValue[32];
    AECHAR    ascaleYValue[32];
    uint32    charHeight;
    /* SetDefaultSoftkeyLook() is from apps/core/AppComFunc.c */
    extern void SetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm);

    (void)IMENUCTL_DeleteAll(menu);
    (void)IMENUCTL_Redraw(menu);

    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_DEFAULT,
                     IDD_ASCALE_SOFTKEY_DEFAULT, NULL, 0);
    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_OK,
                     IDD_ASCALE_SOFTKEY_OK, NULL, 0);
    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_CANCEL,
                     IDD_ASCALE_SOFTKEY_CANCEL, NULL, 0);

    /*
     * Remove the multiline property
     */
    ITEXTCTL_SetProperties(xCtrl, ITEXTCTL_GetProperties(xCtrl) & ~TP_MULTILINE | TP_FRAME);
    ITEXTCTL_SetProperties(yCtrl, ITEXTCTL_GetProperties(yCtrl) & ~TP_MULTILINE | TP_FRAME);
    SetDefaultSoftkeyLook(pme->a.m_pIShell, menu);

    /*
     * Set XScale and YScale values to be displayed
     */
    (void)SNPRINTF(buf, sizeof(buf), "%d", pme->m_nAScaleXValue);
    (void)STRTOWSTR(buf, ascaleXValue, sizeof(ascaleXValue));
    (void)ITEXTCTL_SetText(xCtrl, ascaleXValue, -1);

    (void)SNPRINTF(buf, sizeof(buf), "%d", pme->m_nAScaleYValue);
    (void)STRTOWSTR(buf, ascaleYValue, sizeof(ascaleYValue));
    (void)ITEXTCTL_SetText(yCtrl, ascaleYValue, -1);

    /*
     * set position
     */
    charHeight =
      IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);

    ITEXTCTL_GetRect(xCtrl, &textRect);
    textRect.x = 4;
    textRect.dx -= 8;
    textRect.y = 1;
    textRect.dy = charHeight;
    ITEXTCTL_SetRect(xCtrl, &textRect);

    ITEXTCTL_GetRect(yCtrl, &textRect);
    textRect.x = 4;
    textRect.dx -= 8;
    textRect.y = 1 + 4 * charHeight;
    textRect.dy = charHeight;
    ITEXTCTL_SetRect(yCtrl, &textRect);

    ITEXTCTL_SetMaxSize(xCtrl, MP_ASCALE_DLG_INPUT_LENGTH);
    (void)ITEXTCTL_SetInputMode(xCtrl, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(xCtrl, TRUE);

    ITEXTCTL_SetMaxSize(yCtrl, MP_ASCALE_DLG_INPUT_LENGTH);
    (void)ITEXTCTL_SetInputMode(yCtrl, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(yCtrl, FALSE);
    return TRUE;
  }
  case EVT_KEY:
    switch (w)
    {
    case AVK_CLR:
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    default:
      return FALSE;
    }
  case EVT_DIALOG_END:
    return TRUE;
  case EVT_COMMAND:
    switch (w)
    {
    case IDD_ASCALE_SOFTKEY_DEFAULT:
    {
      char      buf[32];
      AECHAR    ascaleValue[32];

      /*
       * set back to default scale factors
       */
      (void)SNPRINTF(buf, sizeof(buf), "%d", MP_ASCALE_DEFAULT_VALUE);
      (void)STRTOWSTR(buf, ascaleValue, sizeof(ascaleValue));
      (void)ITEXTCTL_SetText(xCtrl, ascaleValue, -1);
      (void)ITEXTCTL_SetText(yCtrl, ascaleValue, -1);
      return TRUE;
    }
    case IDD_ASCALE_SOFTKEY_OK:
    {
      char      buf[32];
      int32     nNum;
      AECHAR   *pTextX = ITEXTCTL_GetTextPtr(xCtrl);
      AECHAR   *pTextY = ITEXTCTL_GetTextPtr(yCtrl);

      /*
       * no more than MP_ASCALE_DLG_INPUT_LENGTH digits
       */
      if (pTextX && WSTRLEN(pTextX) <= MP_ASCALE_DLG_INPUT_LENGTH)
      {
        (void)WSTRTOSTR(pTextX, buf, sizeof(buf));
        nNum = ATOI(buf);
        if (nNum >= MP_ASCALE_MIN_VALUE && nNum <= MP_ASCALE_MAX_VALUE)
        {
          pme->m_nAScaleXValue = nNum;
          DBGPRINTF_MED("MP: Set AScale X percentage = %d", nNum);
        }
        else
        {
          DBGPRINTF_HIGH("MP: Illegal AScale X percentage = %d", nNum);
          DBGPRINTF_HIGH("MP: AScale X percentage remains = %d", pme->m_nAScaleXValue);
        }
      }

      if (pTextY && WSTRLEN(pTextY) <= MP_ASCALE_DLG_INPUT_LENGTH)
      {
        (void)WSTRTOSTR(pTextY, buf, sizeof(buf));
        nNum = ATOI(buf);
        if (nNum >= MP_ASCALE_MIN_VALUE && nNum <= MP_ASCALE_MAX_VALUE)
        {
          pme->m_nAScaleYValue = nNum;
          DBGPRINTF_MED("MP: Set AScale Y percentage = %d", nNum);
        }
        else
        {
          DBGPRINTF_HIGH("MP: Illegal AScale Y percentage = %d", nNum);
          DBGPRINTF_HIGH("MP: AScale Y percentage remains = %d", pme->m_nAScaleYValue);
        }
      }
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    }
    case IDD_ASCALE_SOFTKEY_CANCEL:
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    }
  }
  return FALSE;
}
#endif     // FEATURE_QTV_MDP_ASCALE

#ifdef MP_FEATURE_QTV_DIVX_DRM  
/******************************************************************************/

/*******         DivX Registration Code Dialog Event Handler            *******/

/******************************************************************************/
static boolean
DivxRegCodeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CMediaPlayer *pme = (CMediaPlayer *)pUser;
  IDialog  *pIActiveDialog;
  uint16    wDialogId;
  IMenuCtl *menu;
  IStatic  *staticCode;
  char      regCode[64];

  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  wDialogId = IDIALOG_GetID(pIActiveDialog);
  DBGPRINTF_MED("MP: IDIALOG_GetID returns %d", wDialogId);

  staticCode = (IStatic *)IDIALOG_GetControl(pIActiveDialog, IDC_DIVX_REG_CODE);
  if (NULL == staticCode)
  {
    DBGPRINTF_ERROR("MP: Divx Reg Code failed to get static control");
    return FALSE;
  }

  menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, IDM_DIVX_REG_SOFTKEY);
  if (menu == NULL)
  {
    DBGPRINTF_ERROR("MP: Divx Reg Code failed to get softkey control");
    return FALSE;
  }

  switch (evt)
  {
  case EVT_DIALOG_START:
  {
    AECHAR    AEregCode[32];
    AECHAR    AEtitleCode[64];
    /* SetDefaultSoftkeyLook() is from apps/core/AppComFunc.c */
    extern void SetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm);

    (void)IMENUCTL_DeleteAll(menu);
    (void)IMENUCTL_Redraw(menu);

    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_OK,
                     IDC_DIVX_REG_SOFTKEY_OK, NULL, 0);

    SetDefaultSoftkeyLook(pme->a.m_pIShell, menu);
    MediaPlayer_GetDivXRegCode(pme, NULL, regCode, 128);

    (void)ISHELL_LoadResString(pme->a.m_pIShell, MEDIAPLAYER_RES_FILE, IDS_DIVX_REG_CODE,
                       AEtitleCode, sizeof(AEtitleCode));
    /*
     * Set registration code text
     */
    (void)STRTOWSTR(regCode, AEregCode, sizeof(AEregCode));
    (void)ISTATIC_SetText(staticCode, AEtitleCode, AEregCode, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
    return TRUE;
  }
  case EVT_KEY:
    (void)ISHELL_EndDialog(pme->a.m_pIShell);
    return TRUE;
  case EVT_DIALOG_END:
    return TRUE;
  case EVT_COMMAND:
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
  }
  return FALSE;
}
#endif     // MP_FEATURE_QTV_DIVX_DRM  

/******************************************************************************/

/*******              Key Mappings Dialog Event Handler                 *******/

/******************************************************************************/
static boolean
KeyMappingsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CMediaPlayer *pme = (CMediaPlayer *)pUser;
  IDialog  *pIActiveDialog;
  uint16    wDialogId;
  int       keyOffset = 0;
  IMenuCtl *menu;
  AEEMenuColors col;
  static uint16 activeCtl = IDC_KEYMAP_0;
  
  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  wDialogId = IDIALOG_GetID(pIActiveDialog);
  DBGPRINTF_MED("MP: IDIALOG_GetID returns %d (%04x %hu %u)", wDialogId, evt, w, dw);

  menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, IDM_KEYMAP_SOFTKEY);
  if (menu == NULL)
  {
    DBGPRINTF_ERROR("MP: Key Mappings failed to get softkey control");
    return FALSE;
  }

  switch (evt)
  {
  case EVT_DIALOG_START:
  {
    AEERect rect = {0};

    /* SetDefaultSoftkeyLook() is from apps/core/AppComFunc.c */
    extern void SetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm);

    (void)IMENUCTL_DeleteAll(menu);
    (void)IMENUCTL_Redraw(menu);

    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_DEFAULT,
                     IDC_KEYMAP_SOFTKEY_OK, NULL, 0);

    SetDefaultSoftkeyLook(pme->a.m_pIShell, menu);

    keyOffset = 0;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_0, IDM_KEY_0, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_1, IDM_KEY_1, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_2, IDM_KEY_2, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_3, IDM_KEY_3, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_4, IDM_KEY_4, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_5, IDM_KEY_5, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_6, IDM_KEY_6, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_7, IDM_KEY_7, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_8, IDM_KEY_8, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_9, IDM_KEY_9, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_STAR, IDM_KEY_STAR, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;
    KeyMappingsDlgSetListKeys(pme, IDC_KEYMAP_POUND, IDM_KEY_POUND, MPPW_KeyMappings[keyOffset++], &rect);
    rect.y += rect.dy;

    IMENUCTL_SetActive(menu, FALSE);
    activeCtl = IDC_KEYMAP_0;
    return TRUE;
  }
  case EVT_KEY:
    return TRUE;
  case EVT_KEY_PRESS:
    return TRUE;
  case EVT_KEY_RELEASE:
    menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, activeCtl);
    IMENUCTL_SetActive(menu, FALSE);
    if (menu == NULL)
    {
      DBGPRINTF_ERROR("MP: Key Mappings failed to get menu control for %hu", activeCtl);
      return FALSE;
    }
    if (w == AVK_DOWN)
    {
      activeCtl++;
      if(activeCtl > IDM_KEYMAP_SOFTKEY)
        activeCtl = IDC_KEYMAP_0;

      // Set colors of prev menu
      col.cSelText = RGB_BLACK;
      col.wMask = MC_BACK | MC_SEL_BACK | MC_SEL_TEXT;
      col.cBack = MAKE_RGB(255, 255, 204);
      col.cSelBack = MAKE_RGB(153, 204, 204);
      IMENUCTL_SetColors(menu, &col);
      (void)IMENUCTL_Redraw(menu);

      menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, activeCtl);
      IMENUCTL_SetActive(menu, FALSE);
      if (menu == NULL)
      {
        DBGPRINTF_ERROR("MP: Key Mappings failed to get menu control for %hu", activeCtl);
        return TRUE;
      }

      // Set colors of current menu
      col.cSelText = RGB_BLACK;
      col.wMask = MC_BACK | MC_SEL_BACK | MC_SEL_TEXT;
      col.cBack = MAKE_RGB(200, 200, 200);
      col.cSelBack = MAKE_RGB(200, 200, 200);
      IMENUCTL_SetColors(menu, &col);
      (void)IMENUCTL_Redraw(menu);
    }
    else if (w == AVK_UP)
    {
      IMENUCTL_SetActive(menu, FALSE);
      activeCtl--;
      if(activeCtl < IDC_KEYMAP_0)
        activeCtl = IDM_KEYMAP_SOFTKEY;

      // Set colors of prev menu
      col.cSelText = RGB_BLACK;
      col.wMask = MC_BACK | MC_SEL_BACK | MC_SEL_TEXT;
      col.cBack = MAKE_RGB(255, 255, 204);
      col.cSelBack = MAKE_RGB(153, 204, 204);
      IMENUCTL_SetColors(menu, &col);
      (void)IMENUCTL_Redraw(menu);

      menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, activeCtl);
      IMENUCTL_SetActive(menu, FALSE);
      if (menu == NULL)
      {
        DBGPRINTF_ERROR("MP: Key Mappings failed to get menu control for %hu", activeCtl);
        return TRUE;
      }

      // Set colors of current menu
      col.cSelText = RGB_BLACK;
      col.wMask = MC_BACK | MC_SEL_BACK | MC_SEL_TEXT;
      col.cBack = MAKE_RGB(200, 200, 200);
      col.cSelBack = MAKE_RGB(200, 200, 200);
      IMENUCTL_SetColors(menu, &col);
      (void)IMENUCTL_Redraw(menu);
    }
    else if ((w == AVK_LEFT) && (activeCtl != IDM_KEYMAP_SOFTKEY))
    {
      boolean ret;
      IMENUCTL_SetActive(menu, TRUE);
      ret = IMENUCTL_HandleEvent(menu, EVT_KEY, AVK_DOWN, dw);
      IMENUCTL_SetActive(menu, FALSE);
      return(ret);
    }
    else if ((w == AVK_RIGHT) && (activeCtl != IDM_KEYMAP_SOFTKEY))
    {
      boolean ret;
      IMENUCTL_SetActive(menu, TRUE);
      ret = IMENUCTL_HandleEvent(menu, EVT_KEY, AVK_UP, dw);
      IMENUCTL_SetActive(menu, FALSE);
      return(ret);
    }
    else if ((w == AVK_SELECT) && (activeCtl == IDM_KEYMAP_SOFTKEY))
    {
      KeyMappingsSetDefault(pme);
      return TRUE;
    }
    else if ((w == AVK_CLR))// || (w == AVK_END))
    {
      keyOffset = 0;
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_0);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_1);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_2);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_3);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_4);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_5);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_6);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_7);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_8);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_9);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_STAR);
      MPPW_KeyMappings[keyOffset++] = KeyMappingsDlgGetMappedKey(pme, IDC_KEYMAP_POUND);

      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    }
    return TRUE;
  case EVT_DIALOG_END:
    return TRUE;
  case EVT_COMMAND:
    switch (w)
    {
    case IDC_KEYMAP_SOFTKEY_OK:
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    default:
      return TRUE;
    }
  }
  return FALSE;
}

static boolean
KeyMappingsDlgSetListKeys(CMediaPlayer *pme, uint16 wItemID, uint16 wTitleID, CKeyMapping curMapping, AEERect *rect)
{
  IDialog  *pIActiveDialog;
  IMenuCtl *menu;
  AEEMenuColors col;

  if((pme == NULL) || (rect == NULL))
    return FALSE;

  DBGPRINTF_MED("MP: Set keys options for %hu, curMapping %d", wItemID, curMapping);
  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  if(pIActiveDialog == NULL)
    return FALSE;

  menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, wItemID);
  if (menu == NULL)
  {
    DBGPRINTF_ERROR("MP: Set key options failed to get menu control %hu", wItemID);
    return FALSE;
  }

  (void)IMENUCTL_DeleteAll(menu);
  (void)IMENUCTL_Redraw(menu);

  (void)IMENUCTL_SetTitle(menu, MEDIAPLAYER_RES_FILE, wTitleID, NULL);

  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_NONE, IDM_KEYMAP_NONE, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_FULLSCREEN, IDM_KEYMAP_FULLSCREEN, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_BK_FILE, IDM_KEYMAP_BK_FILE, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_FW_FILE, IDM_KEYMAP_FW_FILE, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_PL_RINGER, IDM_KEYMAP_PL_RINGER, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_ST_RINGER, IDM_KEYMAP_ST_RINGER, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_REWIND, IDM_KEYMAP_REWIND, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_FASTFWD, IDM_KEYMAP_FASTFWD, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_PLAYSTOP, IDM_KEYMAP_PLAYSTOP, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_PAUSE, IDM_KEYMAP_PAUSE, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_FF_STEP, IDM_KEYMAP_FF_STEP, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_ZOOM_IN, IDM_KEYMAP_ZOOM_IN, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_ZOOM_OUT, IDM_KEYMAP_ZOOM_OUT, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_ROTATE, IDM_KEYMAP_ROTATE, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_RECORD, IDM_KEYMAP_RECORD, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_RECSET_STRST, IDM_KEYMAP_RECSET_STRST, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_REPLAY_AT_STOP, IDM_KEYMAP_REPLAY_AT_STOP, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_SAVEFILE, IDM_KEYMAP_SAVEFILE, NULL, 0);
  (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_KEYMAP_FCS_SWITCH, IDM_KEYMAP_FCS_SWITCH, NULL, 0);

  switch(curMapping)
  {
    case MPPW_KEYMAP_NONE:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_NONE);
      break;
    case MPPW_KEYMAP_FULLSCREEN:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_FULLSCREEN);
      break;
    case MPPW_KEYMAP_BK_FILE:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_BK_FILE);
      break;
    case MPPW_KEYMAP_FW_FILE:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_FW_FILE);
      break;
    case MPPW_KEYMAP_PL_RINGER:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_PL_RINGER);
      break;
    case MPPW_KEYMAP_ST_RINGER:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_ST_RINGER);
      break;
    case MPPW_KEYMAP_REWIND:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_REWIND);
      break;
    case MPPW_KEYMAP_FASTFWD:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_FASTFWD);
      break;
    case MPPW_KEYMAP_PLAYSTOP:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_PLAYSTOP);
      break;
    case MPPW_KEYMAP_PAUSE:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_PAUSE);
      break;
    case MPPW_KEYMAP_FF_STEP:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_FF_STEP);
      break;
    case MPPW_KEYMAP_ZOOM_IN:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_ZOOM_IN);
      break;
    case MPPW_KEYMAP_ZOOM_OUT:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_ZOOM_OUT);
      break;
    case MPPW_KEYMAP_ROTATE:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_ROTATE);
      break;
    case MPPW_KEYMAP_RECORD:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_RECORD);
      break;
    case MPPW_KEYMAP_RECSET_STRST:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_RECSET_STRST);
      break;
    case MPPW_KEYMAP_REPLAY_AT_STOP:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_REPLAY_AT_STOP);
      break;
    case MPPW_KEYMAP_SAVEFILE:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_SAVEFILE);
      break;
    case MPPW_KEYMAP_FCS_SWITCH:
      IMENUCTL_SetSel(menu, IDM_KEYMAP_FCS_SWITCH);
      break;
    default:
      break;
  }

  // Set Colors
  col.cSelText = RGB_BLACK;
  col.wMask = MC_BACK | MC_SEL_BACK | MC_SEL_TEXT;
  col.cBack = MAKE_RGB(255, 255, 204);
  col.cSelBack = MAKE_RGB(153, 204, 204);
  IMENUCTL_SetColors(menu, &col);

  if(rect->dy == 0)
  {
    rect->x = 1;
    rect->y = 1;
    rect->dx= pme->m_cxWidth - 1;
#if 0
    rect->dy= 13;
#else
    rect->dy= (pme->m_cyHeight / 14);
#endif

    // Set colors of current menu
    col.cSelText = RGB_BLACK;
    col.wMask = MC_BACK | MC_SEL_BACK | MC_SEL_TEXT;
    col.cBack = MAKE_RGB(200, 200, 200);
    col.cSelBack = MAKE_RGB(200, 200, 200);
    IMENUCTL_SetColors(menu, &col);
    (void)IMENUCTL_Redraw(menu);
  }
  IMENUCTL_SetRect(menu, rect);
  (void)IMENUCTL_Redraw(menu);
  IMENUCTL_SetActive(menu, FALSE);
  return TRUE;
}

CKeyMapping KeyMappingsDlgGetMappedKey(CMediaPlayer *pme, uint16 wItemID)
{
  IDialog  *pIActiveDialog;
  IMenuCtl *menu;
  uint16 sel;

  if(pme == NULL)
    return MPPW_KEYMAP_NONE;

  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  if(pIActiveDialog == NULL)
    return MPPW_KEYMAP_NONE;

  menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, wItemID);
  if (menu == NULL)
  {
    DBGPRINTF_ERROR("MP: Get key failed to get menu control %hu", wItemID);
    return MPPW_KEYMAP_NONE;
  }

  sel = IMENUCTL_GetSel(menu);
  return MPKeyMapping_DlgToMapping(sel);
}

boolean KeyMappingsDlgSetMappedKey(CMediaPlayer *pme, uint16 wItemID, CKeyMapping key)
{
  IDialog  *pIActiveDialog;
  IMenuCtl *menu;
  uint16 sel;

  DBGPRINTF_ERROR("MP: Set key %hu with %d", wItemID, key);
  if(pme == NULL)
  {
    DBGPRINTF_ERROR("MP: Set key failed pme NULL");
    return FALSE;
  }

  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  if(pIActiveDialog == NULL)
  {
    DBGPRINTF_ERROR("MP: Set key failed get active dialog failed");
    return FALSE;
  }

  menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, wItemID);
  if (menu == NULL)
  {
    DBGPRINTF_ERROR("MP: Get key failed to get menu control %hu", wItemID);
    return MPPW_KEYMAP_NONE;
  }

  sel = MPKeyMapping_MappingToDlg(key);
  (void)IMENUCTL_SetSel(menu, sel);
  return TRUE;
}

boolean KeyMappingsSetDefault(CMediaPlayer *pme)
{
  int keyOffset;

  DBGPRINTF_MED("MP: KeyMappingSetDefault");
  if(pme == NULL)
    return FALSE;

  MPPW_KeyMappings[0] = MPPW_KEYMAP_FULLSCREEN;
#if defined(FEATURE_DTV_DVBH) || defined(MP_FEATURE_MBMS)
  MPPW_KeyMappings[1] = MPPW_KEYMAP_BK_FILE;
#else
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
  MPPW_KeyMappings[1] = MPPW_KEYMAP_PL_RINGER;
#else
  MPPW_KeyMappings[1] = MPPW_KEYMAP_REWIND;
#endif
#endif
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
  MPPW_KeyMappings[2] = MPPW_KEYMAP_ST_RINGER;
#else
  MPPW_KeyMappings[2] = MPPW_KEYMAP_PLAYSTOP;
#endif
#if defined(FEATURE_DTV_DVBH)  || defined(MP_FEATURE_MBMS)
  MPPW_KeyMappings[3] = MPPW_KEYMAP_FW_FILE;
#else
  MPPW_KeyMappings[3] = MPPW_KEYMAP_FASTFWD;
#endif
#ifdef FEATURE_QTV_FCS
#error code not present
#else
  MPPW_KeyMappings[4] = MPPW_KEYMAP_NONE;
#endif
  MPPW_KeyMappings[5] = MPPW_KEYMAP_PAUSE;
  MPPW_KeyMappings[6] = MPPW_KEYMAP_FF_STEP;
  MPPW_KeyMappings[7] = MPPW_KEYMAP_ZOOM_IN;
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  MPPW_KeyMappings[8] = MPPW_KEYMAP_SAVEFILE;
#else
  MPPW_KeyMappings[8] = MPPW_KEYMAP_ROTATE;
#endif
  MPPW_KeyMappings[9] = MPPW_KEYMAP_ZOOM_OUT;
  MPPW_KeyMappings[10] = MPPW_KEYMAP_RECORD;
#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)
  MPPW_KeyMappings[11] = MPPW_KEYMAP_REPLAY_AT_STOP;
#else
  MPPW_KeyMappings[11] = MPPW_KEYMAP_RECSET_STRST;
#endif

  keyOffset = 0;
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_0, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_1, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_2, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_3, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_4, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_5, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_6, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_7, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_8, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_9, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_STAR, MPPW_KeyMappings[keyOffset++]);
  KeyMappingsDlgSetMappedKey(pme, IDC_KEYMAP_POUND, MPPW_KeyMappings[keyOffset++]); 

  return TRUE;
}

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
static boolean
ProgDnldSettingsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CMediaPlayer *pme = (CMediaPlayer *)pUser;
  IDialog  *pIActiveDialog;
  uint16    wDialogId;
  IMenuCtl *menu;
  ITextCtl *startupCtrl;
  ITextCtl *bufferingCtrl;
  ITextCtl *maxsizeCtrl;
  boolean   menuState;
  boolean   startupState;
  boolean   bufferingState;
  boolean   maxsizeState;
  /* SetDefaultSoftkeyLook() is from apps/core/AppComFunc.c */
  extern void SetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm);

  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  wDialogId = IDIALOG_GetID(pIActiveDialog);

  startupCtrl =
    (ITextCtl *)IDIALOG_GetControl(pIActiveDialog,
                                    IDD_3GPP_STARTUP_TIME_INPUT);
  if (NULL == startupCtrl)
    return FALSE;
  bufferingCtrl =
    (ITextCtl *)IDIALOG_GetControl(pIActiveDialog,
                                    IDD_3GPP_BUFFERING_TIME_INPUT);
  if (NULL == bufferingCtrl)
    return FALSE;
  maxsizeCtrl =
    (ITextCtl *)IDIALOG_GetControl(pIActiveDialog,
                                    IDD_3GPP_MAX_FILESIZE_INPUT);
  if (NULL == maxsizeCtrl)
    return FALSE;
  menu =
    (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog,
                                    IDD_3GPP_SETTINGS_SOFTKEY_BUTTON);
  if (menu == NULL)
    return FALSE;

  switch (evt)
  {
  case EVT_DIALOG_START:
  {
    AEERect   textRect;
    char      buf[32];
    AECHAR    startupTime[32];
    AECHAR    bufferingTime[32];
    AECHAR    maxFileSize[32];
    uint32    charHeight;

    (void)IMENUCTL_DeleteAll(menu);
    (void)IMENUCTL_Redraw(menu);

    IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_DEFAULT,
                     IDD_3GPP_SETTINGS_SOFTKEY_DEFAULT, NULL, 0);
    IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_OK,
                     IDD_3GPP_SETTINGS_SOFTKEY_OK, NULL, 0);
    IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_CANCEL,
                     IDD_3GPP_SETTINGS_SOFTKEY_CANCEL, NULL, 0);

    /*
     * Remove the multiline property
     */
    ITEXTCTL_SetProperties(startupCtrl,
                           ITEXTCTL_GetProperties(startupCtrl) & ~TP_MULTILINE |
                           TP_FRAME);
    ITEXTCTL_SetProperties(bufferingCtrl,
                           ITEXTCTL_GetProperties(bufferingCtrl) & ~TP_MULTILINE
                           | TP_FRAME);
    ITEXTCTL_SetProperties(maxsizeCtrl,
                           ITEXTCTL_GetProperties(maxsizeCtrl) & ~TP_MULTILINE |
                           TP_FRAME);
    SetDefaultSoftkeyLook(pme->a.m_pIShell, menu);

    /*
     * set values to display in text fields
     */
    (void)SNPRINTF(buf, sizeof(buf), "%d", pme->m_defaultSetupTime.dwStartupTime);
    (void)STRTOWSTR(buf, startupTime, sizeof(startupTime));
    (void)ITEXTCTL_SetText(startupCtrl, startupTime, -1);
    (void)SNPRINTF(buf, sizeof(buf), "%d", pme->m_defaultSetupTime.dwBufferingTime);
    (void)STRTOWSTR(buf, bufferingTime, sizeof(bufferingTime));
    (void)ITEXTCTL_SetText(bufferingCtrl, bufferingTime, -1);
    (void)SNPRINTF(buf, sizeof(buf), "%d", pme->m_maxDownloadFileSize);
    (void)STRTOWSTR(buf, maxFileSize, sizeof(maxFileSize));
    (void)ITEXTCTL_SetText(maxsizeCtrl, maxFileSize, -1);

    /*
     * set pos
     */
    charHeight =
      IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);

    ITEXTCTL_GetRect(startupCtrl, &textRect);
    textRect.x = 4;
    textRect.dx -= 8;
    textRect.y = 1;
    textRect.dy = charHeight;
    ITEXTCTL_SetRect(startupCtrl, &textRect);

    ITEXTCTL_GetRect(bufferingCtrl, &textRect);
    textRect.x = 4;
    textRect.dx -= 8;
    textRect.y = 1 + 4 * charHeight;
    textRect.dy = charHeight;
    ITEXTCTL_SetRect(bufferingCtrl, &textRect);

    ITEXTCTL_GetRect(maxsizeCtrl, &textRect);
    textRect.x = 4;
    textRect.dx -= 8;
    textRect.y = 1 + 8 * charHeight;
    textRect.dy = charHeight;
    ITEXTCTL_SetRect(maxsizeCtrl, &textRect);

    ITEXTCTL_SetMaxSize(startupCtrl, MP_HTTP_DLG_INPUT_LENGTH);
    ITEXTCTL_SetInputMode(startupCtrl, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(startupCtrl, TRUE);
    ITEXTCTL_SetMaxSize(bufferingCtrl, MP_HTTP_DLG_INPUT_LENGTH);
    ITEXTCTL_SetInputMode(bufferingCtrl, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(bufferingCtrl, FALSE);
    ITEXTCTL_SetMaxSize(maxsizeCtrl, MP_HTTP_DLG_INPUT_LENGTH);
    ITEXTCTL_SetInputMode(maxsizeCtrl, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(maxsizeCtrl, FALSE);

    ITEXTCTL_SetSoftKeyMenu(startupCtrl, NULL);
    ITEXTCTL_SetSoftKeyMenu(bufferingCtrl, NULL);
    ITEXTCTL_SetSoftKeyMenu(maxsizeCtrl, NULL);

    IMENUCTL_SetActive(menu, FALSE);
    pme->m_pActiveCtrl = startupCtrl;

    return TRUE;
  }
  case EVT_KEY_PRESS:
    if (w == AVK_UP || w == AVK_DOWN)
    {
      return TRUE;
    }
    break;

  case EVT_KEY_RELEASE:
    startupState = FALSE;
    bufferingState = FALSE;
    maxsizeState = FALSE;
    menuState = FALSE;
    switch (w)
    {
    case AVK_UP:
      if (pme->m_pActiveCtrl == startupCtrl)
      {
        menuState = TRUE;
        pme->m_pActiveCtrl = (ITextCtl *)NULL;
      }
      else if (pme->m_pActiveCtrl == bufferingCtrl)
      {
        startupState = TRUE;
        pme->m_pActiveCtrl = startupCtrl;
      }
      else if (pme->m_pActiveCtrl == maxsizeCtrl)
      {
        bufferingState = TRUE;
        pme->m_pActiveCtrl = bufferingCtrl;
      }
      else if (pme->m_pActiveCtrl == (ITextCtl *)NULL)
      {
        maxsizeState = TRUE;
        pme->m_pActiveCtrl = maxsizeCtrl;
      }

      IMENUCTL_SetActive(menu, menuState);
      ITEXTCTL_SetActive(startupCtrl, startupState);
      ITEXTCTL_SetActive(bufferingCtrl, bufferingState);
      ITEXTCTL_SetActive(maxsizeCtrl, maxsizeState);
      return TRUE;

    case AVK_DOWN:
      if (pme->m_pActiveCtrl == startupCtrl)
      {
        bufferingState = TRUE;
        pme->m_pActiveCtrl = bufferingCtrl;
      }
      else if (pme->m_pActiveCtrl == bufferingCtrl)
      {
        maxsizeState = TRUE;
        pme->m_pActiveCtrl = maxsizeCtrl;
      }
      else if (pme->m_pActiveCtrl == maxsizeCtrl)
      {
        menuState = TRUE;
        pme->m_pActiveCtrl = (ITextCtl *)NULL;
      }
      else if (pme->m_pActiveCtrl == (ITextCtl *)NULL)
      {
        startupState = TRUE;
        pme->m_pActiveCtrl = startupCtrl;
      }
      IMENUCTL_SetActive(menu, menuState);
      ITEXTCTL_SetActive(startupCtrl, startupState);
      ITEXTCTL_SetActive(bufferingCtrl, bufferingState);
      ITEXTCTL_SetActive(maxsizeCtrl, maxsizeState);
      return TRUE;

    default:
      return FALSE;
    }
    return FALSE;
    break;

  case EVT_KEY:
    switch (w)
    {
    case AVK_UP:
    case AVK_DOWN:
      return TRUE;

    case AVK_CLR:
      ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    default:
      return FALSE;
    }
    return FALSE;

  case EVT_DIALOG_END:
    return TRUE;

  case EVT_COMMAND:
    switch (w)
    {
    case IDD_3GPP_SETTINGS_SOFTKEY_DEFAULT:
    {
      char      buf[32];
      AECHAR    startupTime[32];
      AECHAR    bufferingTime[32];
      AECHAR    maxFileSize[32];

      /*
       * set back to default startup time
       */
      (void)SNPRINTF(buf, sizeof(buf), "%d", MP_HTTP_STARTUP_TIME);
      (void)STRTOWSTR(buf, startupTime, sizeof(startupTime));
      (void)ITEXTCTL_SetText(startupCtrl, startupTime, -1);
      /*
       * set back to default buffering time
       */
      (void)SNPRINTF(buf, sizeof(buf), "%d", MP_HTTP_BUFFERING_TIME);
      (void)STRTOWSTR(buf, bufferingTime, sizeof(bufferingTime));
      (void)ITEXTCTL_SetText(bufferingCtrl, bufferingTime, -1);
      /*
       * set back to default file size
       */
      (void)SNPRINTF(buf, sizeof(buf), "%d", PS_BUF_DFLT_SIZE);
      (void)STRTOWSTR(buf, maxFileSize, sizeof(maxFileSize));
      (void)ITEXTCTL_SetText(maxsizeCtrl, maxFileSize, -1);
      return TRUE;
    }
    case IDD_3GPP_SETTINGS_SOFTKEY_OK:
    {
      char      buf[32];
      int32     nNum;
      AECHAR   *pText = ITEXTCTL_GetTextPtr(startupCtrl);

      /*
       * no more than MP_SEEK_TIME_DLG_INPUT_LENGTH digits
       */
      if (pText && WSTRLEN(pText) <= MP_HTTP_DLG_INPUT_LENGTH)
      {
        (void)WSTRTOSTR(pText, buf, sizeof(buf));
        nNum = ATOI(buf);
        if (nNum > 0)
        {
          pme->m_defaultSetupTime.dwStartupTime = nNum;
        }
      }
      pText = ITEXTCTL_GetTextPtr(bufferingCtrl);
      if (pText && WSTRLEN(pText) <= MP_HTTP_DLG_INPUT_LENGTH)
      {
        (void)WSTRTOSTR(pText, buf, sizeof(buf));
        nNum = ATOI(buf);
        if (nNum > 0)
        {
          pme->m_defaultSetupTime.dwBufferingTime = nNum;
        }
      }
      pText = ITEXTCTL_GetTextPtr(maxsizeCtrl);
      if (pText && WSTRLEN(pText) <= MP_HTTP_DLG_INPUT_LENGTH)
      {
        (void)WSTRTOSTR(pText, buf, sizeof(buf));
        nNum = ATOI(buf);
        if (nNum > 0)
        {
          pme->m_maxDownloadFileSize = nNum;
          HttpDownloader_maxBufferSize = pme->m_maxDownloadFileSize;
        }
      }
      ISHELL_EndDialog(pme->a.m_pIShell);
      DBGPRINTF_HIGH("MP: set 3GPP startup time   = %d", pme->m_defaultSetupTime.dwStartupTime);
      DBGPRINTF_HIGH("MP: set 3GPP buffering time = %d", pme->m_defaultSetupTime.dwBufferingTime);
      DBGPRINTF_HIGH("MP: set 3GPP max file size  = %d", pme->m_maxDownloadFileSize);
      return TRUE;
    }
    case IDD_3GPP_SETTINGS_SOFTKEY_CANCEL:
      ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    }
  }
  return FALSE;
}
#endif     // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#ifdef FEATURE_MMOVERLAY

/******************************************************************************/

/*******         Alpha Blending Setting Dialog Event Handler            *******/

/******************************************************************************/
static boolean
AlphaBlendSettingDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CMediaPlayer *pme = (CMediaPlayer *)pUser;
  IDialog  *pIActiveDialog;
  uint16    wDialogId;
  IMenuCtl *menu;
  ITextCtl *ctrl;
  /* SetDefaultSoftkeyLook() is from apps/core/AppComFunc.c */
  extern void SetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm);

  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  wDialogId = IDIALOG_GetID(pIActiveDialog);
  DBGPRINTF_MED("MP: IDIALOG_GetID returns %d", wDialogId);
  ctrl = (ITextCtl *)IDIALOG_GetControl(pIActiveDialog, IDD_ALPHA_BLEND_INPUT);

  if (NULL == ctrl)
    return FALSE;

  menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, IDD_ALPHA_BLEND_SOFTKEY_BUTTON);

  if (menu == NULL)
    return FALSE;

  switch (evt)
  {
  case EVT_DIALOG_START:
  {
    AEERect   textRect;
    char      buf[32];
    AECHAR    alphaBlend[32];
    uint32    charHight;

    (void)IMENUCTL_DeleteAll(menu);
    (void)IMENUCTL_Redraw(menu);

    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_DEFAULT,
                     IDD_ALPHA_BLEND_SOFTKEY_DEFAULT, NULL, 0);
    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_OK,
                     IDD_ALPHA_BLEND_SOFTKEY_OK, NULL, 0);
    (void)IMENUCTL_AddItem(menu, MEDIAPLAYER_RES_FILE, IDM_CANCEL,
                     IDD_ALPHA_BLEND_SOFTKEY_CANCEL, NULL, 0);

    /*
     * Remove the multiline property
     */
    ITEXTCTL_SetProperties(ctrl, ITEXTCTL_GetProperties(ctrl) & ~TP_MULTILINE | TP_FRAME);
    SetDefaultSoftkeyLook(pme->a.m_pIShell, menu);

    /*
     * set alpha blending to display
     */
    (void)SNPRINTF(buf, sizeof(buf), "%d", pme->m_nAlphaBlend);
    (void)STRTOWSTR(buf, alphaBlend, sizeof(alphaBlend));
    (void)ITEXTCTL_SetText(ctrl, alphaBlend, -1);

    /*
     * set pos
     */
    ITEXTCTL_GetRect(ctrl, &textRect);
    charHight = IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);
    textRect.x = 4;
    textRect.dx -= 8;
    textRect.y = 1;
    textRect.dy = charHight;
    ITEXTCTL_SetRect(ctrl, &textRect);

    ITEXTCTL_SetMaxSize(ctrl, MP_ALPHA_BLEND_DLG_INPUT_LENGTH);
    (void)ITEXTCTL_SetInputMode(ctrl, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(ctrl, TRUE);
    return TRUE;
  }
  case EVT_KEY:
    switch (w)
    {
    case AVK_CLR:
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    default:
      return FALSE;
    }
  case EVT_DIALOG_END:
    return TRUE;
  case EVT_COMMAND:
    switch (w)
    {
    case IDD_ALPHA_BLEND_SOFTKEY_DEFAULT:
    {
      char      buf[32];
      AECHAR    alphaBlend[32];

      /*
       * set back to default seek time
       */
      (void)SNPRINTF(buf, sizeof(buf), "%d", MP_ALPHA_BLEND_DEFAULT);
      (void)STRTOWSTR(buf, alphaBlend, sizeof(alphaBlend));
      (void)ITEXTCTL_SetText(ctrl, alphaBlend, -1);
      return TRUE;
    }
    case IDD_ALPHA_BLEND_SOFTKEY_OK:
    {
      char      buf[32];
      int32     nNum;
      AECHAR   *pText = ITEXTCTL_GetTextPtr(ctrl);

      /*
       * no more than MP_ALPHA_BLEND_DLG_INPUT_LENGTH digits
       */
      if (pText && WSTRLEN(pText) <= MP_ALPHA_BLEND_DLG_INPUT_LENGTH)
      {
        (void)WSTRTOSTR(pText, buf, sizeof(buf));
        nNum = ATOI(buf);
        if (nNum >= MP_ALPHA_BLEND_MIN_VALUE && nNum <= MP_ALPHA_BLEND_MAX_VALUE)
        {
          pme->m_nAlphaBlend = nNum;
          (void)ISHELL_EndDialog(pme->a.m_pIShell);
          DBGPRINTF_MED("MP: Set Alpha Blend = %d", nNum);
        }
        else
        {
          DBGPRINTF_HIGH("MP: Illegal Alpha Blend value = %d", nNum);
        }
      }
      return TRUE;
    }
    case IDD_ALPHA_BLEND_SOFTKEY_CANCEL:
      (void)ISHELL_EndDialog(pme->a.m_pIShell);
      return TRUE;
    }
  }
  return FALSE;
}
#endif     // FEATURE_MMOVERLAY

#if defined(MP_FEATURE_VIDEO) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))

/******************************************************************************/

/*******            Profile Setting Dialog Event Handler              *******/

/******************************************************************************/
static boolean MPPDPSettingDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  CMediaPlayer *pme = (CMediaPlayer *)pUser;
  IDialog  *pIActiveDialog;
  IMenuCtl *menu;

  pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  menu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog,  IDC_QOS);
  if (menu == NULL)
    return FALSE;

  switch (evt)
  {
    case EVT_DIALOG_START:
    {
      char   buf[32];
      AECHAR aeBuf[32];
      CtlAddItem ai;
      int i;
      uint8 currentPDP;

      // delete current menu
      (void)IMENUCTL_DeleteAll(menu);
      (void)IMENUCTL_Redraw(menu);
      SetSettingsMenuStyle(pme, menu);

      // Add Items in the Menu
      ai.wText = 0;
      ai.pImage = NULL;
      ai.pszResImage = ai.pszResText = MEDIAPLAYER_RES_FILE;
      ai.wFont = AEE_FONT_NORMAL;
      ai.dwData = 0;

      pme->m_NetPolicy.m_nNumPDP = MPPDPGetNumProfiles(pme);

			if(pme->m_NetPolicy.m_bDefaultProfile)
			  currentPDP = MPPDPGetDefaultProfile(pme);
      else
        currentPDP = pme->m_NetPolicy.m_nProfile;

      // PDP number start from 1, 0 is for default
      for (i = 0; i <= pme->m_NetPolicy.m_nNumPDP; i++)
      {
        if(i==0) //idx 0 is used for default PDP
        {
          ai.dwData = MPPDPGetDefaultProfile(pme);
          (void)STRLCPY(buf, "use default", sizeof(buf));
          (void)STRTOWSTR(buf, aeBuf, sizeof(aeBuf));
        }
        else
        {
          ai.dwData = i;
          if(!MPPDPGetProfileName(pme, i, aeBuf, 32))
          {
            // if we can't get name fomr IPDP...
            (void)SNPRINTF(buf, sizeof(buf), "profile %d", i);
            (void)STRTOWSTR(buf, aeBuf, sizeof(aeBuf));
          }
        }

        //Add individual entries to the Menu
        ai.pText    = aeBuf;
        ai.wItemID  = i;
				if(pme->m_NetPolicy.m_bDefaultProfile && i == 0)
          ai.wImage   = (uint16)IDB_RADIO_ON;
				else if(pme->m_NetPolicy.m_bDefaultProfile)
          ai.wImage   = (uint16)IDB_RADIO_OFF;
				else
          ai.wImage   = (uint16)(i == currentPDP ? IDB_RADIO_ON : IDB_RADIO_OFF);
        IMENUCTL_AddItemEx(menu, &ai);
      }

      return TRUE;
    }
    case EVT_KEY_PRESS:
      if (w == AVK_SELECT)
      {
        CtlAddItem pai;
        int16 i;
        int16 nSel;
        nSel = IMENUCTL_GetSel(menu);

        // PDP number start from 1, 0 is for default
        for(i=0; i <= pme->m_NetPolicy.m_nNumPDP; i++)
        {
          if (IMENUCTL_GetItem(menu, i, &pai))
          {
            pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
            pai.pImage = NULL;
            pai.wImage = (uint16)(i == nSel ? IDB_RADIO_ON : IDB_RADIO_OFF);
            IMENUCTL_SetItem(menu, i, MSIF_IMAGE, &pai);

            if(i == nSel)
            {
              // save selection info
              pme->m_NetPolicy.m_bDefaultProfile = (nSel == 0)? TRUE : FALSE;
              pme->m_NetPolicy.m_nProfile = (uint8)pai.dwData;
              DBGPRINTF_HIGH("MP: Set PDP %d", (int)pme->m_NetPolicy.m_nProfile);
            }
          }
        }
        IMENUCTL_Redraw(menu);

        return TRUE;
      }
    case EVT_KEY:
      switch (w)
      {
        case AVK_CLR:
          ISHELL_EndDialog(pme->a.m_pIShell);
          return TRUE;
        default:
          return FALSE;
      }
      return FALSE;
    case EVT_DIALOG_END:
     return TRUE;
  }
  return FALSE;
}
/******************************************************************************
  This function returns system default profile. If failed to create IPDP,
  return 1 as default
******************************************************************************/
static uint8 MPPDPGetDefaultProfile(CMediaPlayer *pme)
{
  IPDP  *pIPDP = NULL;
  byte  num;
  uint8 defaultPDP = 1;

  if(SUCCESS == ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_PDP,(void **)&pIPDP))
  {
    if(AEE_SUCCESS == IPDP_GetDefaultProfileNum(pIPDP, &num))
    {
      DBGPRINTF_MED("MP: Default PDP %d", (int)num);
      defaultPDP = (int)num;
    }
    else
      DBGPRINTF_ERROR("MP: failed to get default profile.");

    IPDP_Release(pIPDP);
  }
  else
    DBGPRINTF_ERROR("MP: failed to create PDP object.");

  return defaultPDP;
}
/******************************************************************************
  This function returns profile name
******************************************************************************/
static boolean MPPDPGetProfileName(CMediaPlayer *pme, int nProfile, AECHAR* buf, int length)
{
  IPDP  *pIPDP = NULL;
  AEEPDP PDPInfo;
  boolean ret = FALSE;

  if(SUCCESS == ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_PDP,(void **)&pIPDP))
  {

// AEEPDP_MAX_PROFILE_NAME_LEN is only defined in old AEEPDP.h file.
// To make backward compatible, we need to check for AEEPDP_MAX_PROFILE_NAME_LEN

#ifndef AEEPDP_MAX_PROFILE_NAME_LEN
    PDPInfo.profile_name = buf;
    PDPInfo.profile_name_len = length;
#endif //AEEPDP_MAX_PROFILE_NAME_LEN

    if(AEE_SUCCESS == IPDP_GetParm(pIPDP, nProfile, AEEPDP_PROFILE_NAME, &PDPInfo))
    {

#ifdef AEEPDP_MAX_PROFILE_NAME_LEN
      //only for old AEEPDP interface
      (void)WSTRLCPY(buf, PDPInfo.profile_name, MIN(length, AEEPDP_MAX_PROFILE_NAME_LEN));
#endif //AEEPDP_MAX_PROFILE_NAME_LEN

      ret = TRUE;
    }
    else
    {
      DBGPRINTF_ERROR("MP: failed to get profile name.");
    }

    IPDP_Release(pIPDP);
  }
  else
  {
    DBGPRINTF_ERROR("MP: failed to create PDP object.");
  }

  return ret;
}
/******************************************************************************
  This function returns system default profile. If failed to create IPDP,
  return 1 as default
******************************************************************************/
static int MPPDPGetNumProfiles(CMediaPlayer *pme)
{
  IPDP  *pIPDP = NULL;
  int   numPDP = 16;

  if(SUCCESS == ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_PDP,(void **)&pIPDP))
  {
    numPDP =  (int)IPDP_GetNumProfiles(pIPDP);
    IPDP_Release(pIPDP);
    DBGPRINTF_MED("MP: Total PDP %d", (int)numPDP);
  }
  else
    DBGPRINTF_ERROR("MP: failed to create pdp object.");

  return numPDP;
}

#endif //FEATURE_QTV_PDP_SELECTION

/******************************************************************************/

/*******        This function is callback for ISHELL_SetTimer.          *******/

/******************************************************************************/
static void
EndCurrentDlg(CMediaPlayer *pme)
{
  (void)ISHELL_EndDialog(pme->a.m_pIShell);
}

/******************************************************************************/

/*******                    Update Radio Buttons                        *******/

/******************************************************************************/
static void
UpdateRadioButtons(CMediaPlayer *pme, uint16 wSelId, IMenuCtl *pIMenu)
{
  int       count,
            i;
  uint16    wItemID;
  CtlAddItem pai;

  count = IMENUCTL_GetItemCount(pIMenu);

  /*
   * uncheck previously checked item
   */
  for (i = 0; i < count; i++)
  {
    (void)MEMSET(&pai, 0, sizeof(pai));
    pai.pszResImage = MEDIAPLAYER_RES_FILE;
    wItemID = IMENUCTL_GetItemID(pIMenu, i);

    if (IMENUCTL_GetItem(pIMenu, wItemID, &pai))
    {
      if (pai.pImage == pme->m_pIRadioOn)
      {
        pai.pszResImage = MEDIAPLAYER_RES_FILE;
        pai.pImage = pme->m_pIRadioOff;
        (void)IMENUCTL_SetItem(pIMenu, wItemID, MSIF_IMAGE, &pai);
        break;
      }
    }
  }

  /*
   * check newly selected item
   */
  (void)MEMSET(&pai, 0, sizeof(pai));
  pai.pszResImage = MEDIAPLAYER_RES_FILE;
  pai.pImage = pme->m_pIRadioOn;
  (void)IMENUCTL_SetItem(pIMenu, wSelId, MSIF_IMAGE, &pai);
  (void)IMENUCTL_Redraw(pIMenu);


}

#ifdef FEATURE_MP4_LCD_CONTRAST

/******************************************************************************/

/*******                    Update Contrast                             *******/

/******************************************************************************/
static void
UpdateContrast(CContrastWin *pme, int value)
{
  char      temp[20];
  AECHAR    szBuf[MP_MAX_STRLEN] = { '0' };
  int       contrastValue = pme->m_pOwner->m_dispContrast + value;
  int       minContrast = CONTRAST_MIN;
  int       maxContrast = CONTRAST_MAX;
  disp_info_type di;

  /* disp_get_info() is in drivers/lcd/disp_api_default.c and returns LCD info */
  /* by using a drv_ioctl() call.  Contrast values are set as part of the      */
  /* display device initialization. */
  di = disp_get_info();

  DBGPRINTF_MED("MP: UpdateContrast(%d) contrastValue = %d", value, contrastValue);
  DBGPRINTF_MED("MP: UpdateContrast(%d) di min %d, max %d, default %d",
    value, di.contrast_min, di.contrast_max, di.contrast_default);
  DBGPRINTF_MED("MP: UpdateContrast(%d) minContrast %d, maxContrast %d",
    value, minContrast, maxContrast);

  /* This is the case if no one filled in min/max contrast for this platform. */
  /* Use driver defaults. */
  if ((minContrast == CONTRAST_UNDEFINED) &&
      (maxContrast == CONTRAST_UNDEFINED)) {
    minContrast = di.contrast_min;
    maxContrast = di.contrast_max;
  }

  if (contrastValue > maxContrast)
  {
    contrastValue = maxContrast;
  }
  if (contrastValue < minContrast)
  {
    contrastValue = minContrast;
  }

  DBGPRINTF_MED("MP: calling disp_set_contrast(%d)", contrastValue);
  disp_set_contrast(contrastValue);
  pme->m_pOwner->m_dispContrast = contrastValue;

  (void)ISHELL_LoadResString(pme->m_pIShell, MEDIAPLAYER_RES_FILE, IDS_CONTRAST_VALUE,
                       szBuf, sizeof(szBuf));
  (void)SNPRINTF(temp, sizeof(temp), " %d/%d", pme->m_pOwner->m_dispContrast, maxContrast);
  (void)STRTOWSTR(temp, &szBuf[WSTRLEN(szBuf)], sizeof(szBuf) - WSTRLEN(szBuf));
  MP_FitStaticText(pme->m_pIDisplay, pme->m_pContrastText, AEE_FONT_BOLD,
                   szBuf);
  (void)ISTATIC_Redraw(pme->m_pContrastText);

}
#endif     // FEATURE_MP4_LCD_CONTRAST

/******************************************************************************/

/*******    This function returns current selected menu settings.       *******/

/******************************************************************************/
static uint16 GetCurrentSettings(CMediaPlayer *pme, uint16 wDlgId)
{
  switch (wDlgId)
  {
  case IDD_AUTOPLAY:
    if (pme->m_bAutoPlay)
      return IDDS_AUTOPLAY_ON;
    else
      return IDDS_AUTOPLAY_OFF;

  case IDD_CONT_MODE:
    if (pme->m_nContMode == CONT_MODE_OFF)
      return IDDS_CONT_OFF;
    else if (pme->m_nContMode == CONT_MODE_SINGLE_FILE)
      return IDDS_SINGLE_FILE;
    else if (pme->m_nContMode == CONT_MODE_ALL_FILES)
      return IDDS_ALL_FILES;

  case IDD_VOLUME_SETTINGS:
    if (pme->m_wVolumeLevel == VOLUME_OFF)
    {
      return IDDS_VOLUME_OFF;
    }
    else if (pme->m_wVolumeLevel == VOLUME_ONE)
    {
      return IDDS_VOLUME_ONE;
    }
    else if (pme->m_wVolumeLevel == VOLUME_TWO)
    {
      return IDDS_VOLUME_TWO;
    }
    else if (pme->m_wVolumeLevel == VOLUME_THREE)
    {
      return IDDS_VOLUME_THREE;
    }
    else if (pme->m_wVolumeLevel == VOLUME_FOUR)
    {
      return IDDS_VOLUME_FOUR;
    }
    else if (pme->m_wVolumeLevel == VOLUME_FIVE)
    {
      return IDDS_VOLUME_FIVE;
    }
    else if (pme->m_wVolumeLevel == VOLUME_SIX)
    {
      return IDDS_VOLUME_SIX;
    }
    else if (pme->m_wVolumeLevel == VOLUME_SEVEN)
    {
      return IDDS_VOLUME_SEVEN;
    }
    else if (pme->m_wVolumeLevel == VOLUME_EIGHT)
    {
      return IDDS_VOLUME_EIGHT;
    }
    else if (pme->m_wVolumeLevel == VOLUME_NINE)
    {
      return IDDS_VOLUME_NINE;
    }
    else if (pme->m_wVolumeLevel == VOLUME_TEN)
    {
      return IDDS_VOLUME_TEN;
    }

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  case IDD_PSEUDO_STR_SETTINGS:

    if (pme->m_bPSbwLevel == PS_BW_LVL1)
    {
      return IDDS_PS_BW_LVL1;
    }
    if (pme->m_bPSbwLevel == PS_BW_LVL2)
    {
      return IDDS_PS_BW_LVL2;
    }
    else if (pme->m_bPSbwLevel == PS_BW_LVL3)
    {
      return IDDS_PS_BW_LVL3;
    }
    else if (pme->m_bPSbwLevel == PS_BW_LVL4)
    {
      return IDDS_PS_BW_LVL4;
    }
    else if (pme->m_bPSbwLevel == PS_BW_LVL5)
    {
      return IDDS_PS_BW_LVL5;
    }
    else if (pme->m_bPSbwLevel == PS_BW_RANDOM)
    {
      return IDDS_PS_BW_RANDOM;
    }
    else if (pme->m_bPSbwLevel == PS_BW_MAX)
    {
      return IDDS_PS_BW_MAX;
    }

  case IDD_KDDI_EXTENSIONS:
    if (pme->m_bKddiExt)
      return IDDS_KDDI_EXTENSIONS_ON;
    else
      return IDDS_KDDI_EXTENSIONS_OFF;
#endif
#endif

#if defined(MP_FEATURE_VIDEO) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))
  case IDD_QOS_CONFIG:
    if (TRUE == pme->m_NetPolicy.m_bQoSEnable)
    {
      return IDDS_QOS_ENABLE;
    }

    if (FALSE == pme->m_NetPolicy.m_bQoSEnable)
    {
      return IDDS_QOS_DISABLE;
    }
#endif

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  case IDD_RTSP_STR_REC:
    if (pme->m_bRTSPstreamRec)
      return IDDS_RTSP_STR_REC_ON;
    else
      return IDDS_RTSP_STR_REC_OFF;

#endif

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
    case IDD_ICONTENT:
    if (pme->m_bIContentMode)
      return IDDS_ICONTENT_ON;
    else
      return IDDS_ICONTENT_OFF;
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

    case IDD_BUFFER:
    if (pme->m_bPlayFromBuffer)
      return IDDS_BUFFER_ON;
    else
      return IDDS_BUFFER_OFF;

    case IDD_PLAY_IN_BACKGROUND:
    if (pme->m_bPlayAudioInBackground)
      return IDDS_PLAY_IN_BACKGROUND_ON;
    else
      return IDDS_PLAY_IN_BACKGROUND_OFF;

    case IDD_REDRAW_AT_END:
    if (pme->m_bRedrawScreenAtClipEnd)
      return IDDS_REDRAW_AT_END_ON;
    else
      return IDDS_REDRAW_AT_END_OFF;

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    case IDD_PVXSAVELOC:
    if (pme->m_bPvxSavetoFlash)
      return IDDS_PVXSAVELOC_FLASH;
    else
      return IDDS_PVXSAVELOC_NONE;
#endif

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
#if defined (MMD_HTTP_PULL_BUFFER_PLAYBACK) && defined (MMD_HTTP_PUSH_BUFFER_PLAYBACK)
    case IDD_HTTP_DOWNLOAD:
      if (HTTPDownloadInterface == MMD_HTTP_PULL_BUFFER_PLAYBACK)
        return IDDS_HTTP_PULL;
      else if (HTTPDownloadInterface == MMD_HTTP_PUSH_BUFFER_PLAYBACK)
        return IDDS_HTTP_PUSH;
#endif //FEATURE_APP_MPEG4
#endif

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
    case IDD_HTTP_SOURCE:
      if (pme->m_bCallQtvProgDnld)
        return IDDS_HTTP_SOURCE_QTV;
      else
        return IDDS_HTTP_SOURCE_MEDIAPLAYER;

    case IDD_HTTP_MIMIC:
      if (pme->m_MimicEncryptDnld == MIMIC_ENCRYPT_DNLD_ENABLE)
        return IDDS_HTTP_MIMIC_ON;
      else if (pme->m_MimicEncryptDnld == MIMIC_ENCRYPT_DNLD_STOP)
        return IDDS_HTTP_MIMIC_STOP;
      else
        return IDDS_HTTP_MIMIC_OFF;
#endif  // FEATURE_APP_MPEG4 && (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)

#ifdef FEATURE_APP_MPEG4
    case IDD_QTV_SUSPEND_MODE:
      if (pme->m_bQtvSuspendPause)
        return IDDS_QTV_SUSPEND_MODE_PAUSE;
      else
        return IDDS_QTV_SUSPEND_MODE_STOP;
#endif

#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
    case IDD_PLAY_RINGER_AND_QTV:
    if (pme->m_bPlayRingerWithQTV)
      return IDDS_PLAY_RINGER_AND_QTV_ON;
    else
      return IDDS_PLAY_RINGER_AND_QTV_OFF;

    case IDD_PLAY_RINGER_NO_QTV:
    if (pme->m_bPlayRingerNoQTV)
      return IDDS_PLAY_RINGER_NO_QTV_ON;
    else
      return IDDS_PLAY_RINGER_NO_QTV_OFF;
#endif //(FEATURE_APP_MPEG4) && (FEATURE_QTV_AUDIO_RINGER)

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
    case IDD_PROG_DL_SAVE_LOC:
    if (pme->m_bProgDLFileSaveLoc)
      return IDDS_PROG_DL_SAVE_LOC_FLASH;
    else
      return IDDS_PROG_DL_SAVE_LOC_MMC;
#endif

#ifdef FEATURE_MMOVERLAY
  case IDD_OVERLAYS:
    if ((pme->m_bOverlays) && (pme->m_bFrameCallbacks))
      return IDDS_FRAMES_AND_OVERLAYS_ON;
    else if ((!pme->m_bOverlays) && (pme->m_bFrameCallbacks))
      return IDDS_FRAMES_ONLY;
    else
      return IDDS_FRAMES_AND_OVERLAYS_OFF;
#endif // FEATURE_MMOVERLAY

  case IDD_HIGH_PERFORMANCE_MODE:
    if (pme->m_bHighPerformanceMode)
      return IDDS_HIGH_PERFORMANCE_MODE_ON;
    else
      return IDDS_HIGH_PERFORMANCE_MODE_OFF;

  case IDD_LOCAL_PLAY_PULL_MODE:
    if (pme->m_bLocalPlayPullMode)
      return IDDS_LOCAL_PLAY_PULL_MODE_ON;
    else
      return IDDS_LOCAL_PLAY_PULL_MODE_OFF;

  default:
    DBGPRINTF_HIGH("MP:  Unknown wDlgId = %d", wDlgId);
    break;
  }
  return 0;
}

// this function verify some values read from configure file
// Wrong HTTPDownloadInterface can crash the target.
static void CMediaPlayer_VerifyConfigData(CMediaPlayer *pme)
{
#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  #if defined (MMD_HTTP_PULL_BUFFER_PLAYBACK) && defined (MMD_HTTP_PUSH_BUFFER_PLAYBACK)
    if(HTTPDownloadInterface < MMD_HTTP_PUSH_BUFFER_PLAYBACK ||
     HTTPDownloadInterface > MMD_HTTP_PULL_BUFFER_PLAYBACK)
    {
      HTTPDownloadInterface = MMD_HTTP_PUSH_BUFFER_PLAYBACK;
    }
  #else
    HTTPDownloadInterface  = MMD_BUFFER_FRAGMENT;
  #endif //defined (MMD_HTTP_PULL_BUFFER_PLAYBACK) && defined (MMD_HTTP_PUSH_BUFFER_PLAYBACK)
#endif //defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
}

/******************************************************************************/

/*******       This function sets and initializes radio buttons.        *******/

/******************************************************************************/
static void
SetRadioButtons(CMediaPlayer *pme, IMenuCtl *pICurrentMenu, uint16 wItemId)
{
  CtlAddItem pai;

  (void)MEMSET(&pai, 0, sizeof(pai));
  pai.pszResImage = MEDIAPLAYER_RES_FILE;
  pai.pImage = pme->m_pIRadioOn;
  (void)IMENUCTL_SetItem(pICurrentMenu, wItemId, MSIF_IMAGE, &pai);
  IMENUCTL_SetSel(pICurrentMenu, wItemId);

}

/******************************************************************************/

/*******       This function sets menu style for settings menu.         *******/

/******************************************************************************/
static void
SetSettingsMenuStyle(CMediaPlayer *pMe, IMenuCtl *pIMenu)
{
  AEEItemStyle sstyle;
  AEEItemStyle ustyle;
  AEEMenuColors col;
  uint32    dwProperties = 0;

  /*
   * selection style
   */
  sstyle.ft = AEE_FT_RAISED;
  sstyle.xOffset = 0;
  sstyle.yOffset = 0;
  sstyle.roImage = AEE_RO_TRANSPARENT;
  ustyle.ft = AEE_FT_NONE;
  ustyle.xOffset = 2;
  ustyle.yOffset = 0;
  ustyle.roImage = AEE_RO_TRANSPARENT;
  IMENUCTL_SetStyle(pIMenu, &ustyle, &sstyle);

  /*
   * colors
   */
  col.cSelText = RGB_BLACK;
  col.wMask = MC_BACK | MC_SEL_BACK | MC_SEL_TEXT;
  col.cBack = MAKE_RGB(255, 255, 204);
  col.cSelBack = MAKE_RGB(153, 204, 204);

  dwProperties |= IMENUCTL_GetProperties(pIMenu);

  IMENUCTL_SetColors(pIMenu, &col);
}                                      // End SetGeneralMenuStyle

/******************************************************************************/

/*******       This function translates resource ID to settings.        *******/

/******************************************************************************/
static void TranslateResourceIDToSetting(CMediaPlayer *pme, uint16 ResId)
{
  switch (ResId)
  {
  case IDDS_CONT_OFF:
    pme->m_nContMode = CONT_MODE_OFF;
    break;

  case IDDS_SINGLE_FILE:
    pme->m_nContMode = CONT_MODE_SINGLE_FILE;
    break;

  case IDDS_ALL_FILES:
    pme->m_nContMode = CONT_MODE_ALL_FILES;
    break;

  case IDDS_AUTOPLAY_ON:
    pme->m_bAutoPlay = TRUE;
    break;

  case IDDS_AUTOPLAY_OFF:
    pme->m_bAutoPlay = FALSE;
    break;

  case IDDS_VOLUME_OFF:
    pme->m_wVolumeLevel = VOLUME_OFF;
    break;

  case IDDS_VOLUME_ONE:
    pme->m_wVolumeLevel = VOLUME_ONE;
    break;

  case IDDS_VOLUME_TWO:
    pme->m_wVolumeLevel = VOLUME_TWO;
    break;

  case IDDS_VOLUME_THREE:
    pme->m_wVolumeLevel = VOLUME_THREE;
    break;

  case IDDS_VOLUME_FOUR:
    pme->m_wVolumeLevel = VOLUME_FOUR;
    break;

  case IDDS_VOLUME_FIVE:
    pme->m_wVolumeLevel = VOLUME_FIVE;
    break;

  case IDDS_VOLUME_SIX:
    pme->m_wVolumeLevel = VOLUME_SIX;
    break;

  case IDDS_VOLUME_SEVEN:
    pme->m_wVolumeLevel = VOLUME_SEVEN;
    break;

  case IDDS_VOLUME_EIGHT:
    pme->m_wVolumeLevel = VOLUME_EIGHT;
    break;

  case IDDS_VOLUME_NINE:
    pme->m_wVolumeLevel = VOLUME_NINE;
    break;

  case IDDS_VOLUME_TEN:
    pme->m_wVolumeLevel = VOLUME_TEN;
    break;

#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  case IDDS_PS_BW_LVL1:
    pme->m_bPSbwLevel = PS_BW_LVL1;    // 10 Kbps
    pme->m_nChunkSize = KBPS_10;
    pme->m_bVariableRate = FALSE;
    break;

  case IDDS_PS_BW_LVL2:
    pme->m_bPSbwLevel = PS_BW_LVL2;    // 32 Kbps
    pme->m_nChunkSize = KBPS_32;
    pme->m_bVariableRate = FALSE;
    break;

  case IDDS_PS_BW_LVL3:
    pme->m_bPSbwLevel = PS_BW_LVL3;    // 64 Kbps
    pme->m_nChunkSize = KBPS_64;
    pme->m_bVariableRate = FALSE;
    break;

  case IDDS_PS_BW_LVL4:
    pme->m_bPSbwLevel = PS_BW_LVL4;    // 128 Kbps
    pme->m_nChunkSize = KBPS_128;
    pme->m_bVariableRate = FALSE;
    break;

  case IDDS_PS_BW_LVL5:
    pme->m_bPSbwLevel = PS_BW_LVL5;    // 256 Kbps
    pme->m_nChunkSize = KBPS_256;
    pme->m_bVariableRate = FALSE;
    break;

  case IDDS_PS_BW_MAX:
    pme->m_bPSbwLevel = PS_BW_MAX;     // MAX Kbps
    pme->m_nChunkSize = KBPS_MAX;
    pme->m_bVariableRate = FALSE;
    break;

  case IDDS_PS_BW_RANDOM:
    /*
     * current implementation steadily drops from 54 Kbps
     */
    pme->m_bPSbwLevel = PS_BW_RANDOM;
    pme->m_bVariableRate = TRUE;
    pme->m_nChunkSize = KBPS_54;
    break;

  case IDDS_KDDI_EXTENSIONS_ON:
    pme->m_bKddiExt = TRUE;
    break;

  case IDDS_KDDI_EXTENSIONS_OFF:
    pme->m_bKddiExt = FALSE;
    break;

#endif
#endif

#if defined(MP_FEATURE_VIDEO) && \
 (defined(FEATURE_QTV_QOS_SELECTION)||defined(FEATURE_QTV_PDP_SELECTION))
  case IDDS_QOS_ENABLE:
    pme->m_NetPolicy.m_bQoSEnable = TRUE;
    break;

  case IDDS_QOS_DISABLE:
    pme->m_NetPolicy.m_bQoSEnable = FALSE;
    break;

#endif

#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
  case IDDS_RTSP_STR_REC_ON:
    pme->m_bRTSPstreamRec = bRTSPstreamRecord = TRUE;
    break;

  case IDDS_RTSP_STR_REC_OFF:
    pme->m_bRTSPstreamRec = bRTSPstreamRecord = FALSE;
    break;
#endif

#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))
    case IDDS_ICONTENT_ON:
    pme->m_bIContentMode = TRUE;
    break;

  case IDDS_ICONTENT_OFF:
    pme->m_bIContentMode = FALSE;
    break;
#endif //#if (defined(MP_FEATURE_ICONTENT) || defined(MP_FEATURE_ICONTENTHANDLER))

  case IDDS_BUFFER_ON:
    pme->m_bPlayFromBuffer = TRUE;
    break;

  case IDDS_BUFFER_OFF:
    pme->m_bPlayFromBuffer = FALSE;
    break;

  case IDDS_PLAY_IN_BACKGROUND_ON:
    pme->m_bPlayAudioInBackground = TRUE;
    break;

  case IDDS_PLAY_IN_BACKGROUND_OFF:
    pme->m_bPlayAudioInBackground = FALSE;
    break;

  case IDDS_REDRAW_AT_END_ON:
    pme->m_bRedrawScreenAtClipEnd = TRUE;
    break;

  case IDDS_REDRAW_AT_END_OFF:
    pme->m_bRedrawScreenAtClipEnd = FALSE;
    break;

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  case IDDS_PVXSAVELOC_FLASH:
    pme->m_bPvxSavetoFlash = TRUE;
    break;

  case IDDS_PVXSAVELOC_NONE:
    pme->m_bPvxSavetoFlash = FALSE;
    break;
#endif

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
#if defined(MMD_HTTP_PUSH_BUFFER_PLAYBACK) && defined (MMD_HTTP_PULL_BUFFER_PLAYBACK)
  case IDDS_HTTP_PULL:
    HTTPDownloadInterface = MMD_HTTP_PULL_BUFFER_PLAYBACK;
    pme->m_bLocalPlayPullMode = TRUE;
    break;

  case IDDS_HTTP_PUSH:
    HTTPDownloadInterface = MMD_HTTP_PUSH_BUFFER_PLAYBACK;
    pme->m_bLocalPlayPullMode = FALSE;
    break;
#endif //defined(MMD_HTTP_PUSH_BUFFER_PLAYBACK) && defined (MMD_HTTP_PULL_BUFFER_PLAYBACK)
#endif

#if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2))
  case IDDS_HTTP_SOURCE_QTV:
#if defined(FEATURE_QTV_HTTP_DOWNLOAD)
    pme->m_bCallQtvProgDnld = TRUE;
#else
    pme->m_bCallQtvProgDnld = FALSE;
#endif
    break;

  case IDDS_HTTP_SOURCE_MEDIAPLAYER:
    pme->m_bCallQtvProgDnld = FALSE;
    break;

  case IDDS_HTTP_MIMIC_ON:
    pme->m_MimicEncryptDnld = MIMIC_ENCRYPT_DNLD_ENABLE;
    break;

  case IDDS_HTTP_MIMIC_OFF:
    pme->m_MimicEncryptDnld = MIMIC_ENCRYPT_DNLD_NONE;
    break;

  case IDDS_HTTP_MIMIC_STOP:
    pme->m_MimicEncryptDnld = MIMIC_ENCRYPT_DNLD_STOP;
    break;
#endif  // FEATURE_APP_MPEG4 && (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)

#ifdef FEATURE_APP_MPEG4
  case IDDS_QTV_SUSPEND_MODE_PAUSE:
    pme->m_bQtvSuspendPause = TRUE;
    break;

  case IDDS_QTV_SUSPEND_MODE_STOP:
    pme->m_bQtvSuspendPause = FALSE;
    break;
#endif

#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
  case IDDS_PLAY_RINGER_AND_QTV_ON:
    pme->m_bPlayRingerWithQTV = TRUE;
    break;

  case IDDS_PLAY_RINGER_AND_QTV_OFF:
    pme->m_bPlayRingerWithQTV = FALSE;
    break;

  case IDDS_PLAY_RINGER_NO_QTV_ON:
    pme->m_bPlayRingerNoQTV = TRUE;
    break;

  case IDDS_PLAY_RINGER_NO_QTV_OFF:
    pme->m_bPlayRingerNoQTV = FALSE;
    break;
#endif //(FEATURE_APP_MPEG4) && (FEATURE_QTV_AUDIO_RINGER)

#if defined(FEATURE_APP_MPEG4) && (defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  case IDDS_PROG_DL_SAVE_LOC_FLASH:
    pme->m_bProgDLFileSaveLoc = TRUE;
    break;

  case IDDS_PROG_DL_SAVE_LOC_MMC:
    pme->m_bProgDLFileSaveLoc = FALSE;
    break;
#endif
 
#ifdef FEATURE_MMOVERLAY    
  case IDDS_FRAMES_AND_OVERLAYS_ON:
    DBGPRINTF_MED("MP: Turn Frame Callbacks and MMOverlays ON");
    pme->m_bOverlays = TRUE;
    pme->m_bFrameCallbacks = TRUE;
    break;

  case IDDS_FRAMES_AND_OVERLAYS_OFF:
    DBGPRINTF_MED("MP: Turn Frame Callbacks and MMOverlays OFF");
    pme->m_bOverlays = FALSE;
    pme->m_bFrameCallbacks = FALSE;
    break;

  case IDDS_FRAMES_ONLY:
    DBGPRINTF_MED("MP: Turn Frame Callbacks and MMOverlays OFF");
    pme->m_bOverlays = FALSE;
    pme->m_bFrameCallbacks = TRUE;
    break;
#endif // FEATURE_MMOVERLAY    

  case IDDS_HIGH_PERFORMANCE_MODE_ON:
    DBGPRINTF_MED("MP: Turn High Performance Mode ON");
    pme->m_bHighPerformanceMode = TRUE;
    break;

  case IDDS_HIGH_PERFORMANCE_MODE_OFF:
    DBGPRINTF_MED("MP: Turn High Performance Mode OFF");
    pme->m_bHighPerformanceMode = FALSE;
    break;

  case IDDS_LOCAL_PLAY_PULL_MODE_ON:
    DBGPRINTF_MED("MP: Turn Local Play PULL Mode ON");
    pme->m_bLocalPlayPullMode = TRUE;
    break;

  case IDDS_LOCAL_PLAY_PULL_MODE_OFF:
    DBGPRINTF_MED("MP: Turn Local Play PULL Mode OFF");
    pme->m_bLocalPlayPullMode = FALSE;
    break;

  default:
    DBGPRINTF_HIGH("MP:  Unknown resource ID, ResId = %d", ResId);
    break;
  }
}

/*************************************************************************/

/*****   This function returns count of number of file types        ******/

/*****   with suffix name in DirName.                               ******/

/*************************************************************************/
#ifndef FEATURE_MP_RECORDER_SETTINGS
static int MP_GetFileNameSuffix(CMediaPlayer *MP, char *pszSuffixName, char *pszFileType,
                                char *pszDirName, int NumChar)
{
  stMPIO* pIO;

  pIO= MPMakeIO(MP->a.m_pIShell);
  if(!pIO)
    return 0;

  pIO->m_EnumeType = IO_ENUME_FILE;
  if(!MPIOInit(pIO, pszDirName))
  {
    DBGPRINTF_HIGH("MP: Unable to determine number of files with suffix %s in %s", pszSuffixName, pszDirName);
    MPIO_FREEIF(pIO);
    return 0;
  }

  MP->nSuffix = 1;
  while (MPIOEnumNext(pIO))
  {
    char  *fiName, *extName;

    /*
     * Get the filename.
     */
    fiName = MP_GetFileName(pIO->m_FileInfo.pszFile);

    /*
     * Get the filename extension.
     */
    extName = MP_GetFileNameExtension(fiName);

    /*
     * Compare filename extension with pszSuffixName...
     */
    if (STRCMP(extName, pszSuffixName) == 0)
    {
      if (STRNCMP(pszFileType, fiName, NumChar) == 0)
      {
        MP->nSuffix++;
      }
    }

  }

  MPIO_FREEIF(pIO);
  return MP->nSuffix;
}
#endif


#if defined(FEATURE_QTV_STREAM_RECORD)
/*===========================================================================
   This function generates the record filename given a base filename and an
   extension. If outExt is Not NULL it will be used instead of the existing
   extension (if there is one).  If addLocal is true the string '_local' is
   added before the extension

   Assumption:  Storage has already been allocated for passed-in parameters
   and that the string length of recordFile is MP_MAX_STRING_LENGTH.
===========================================================================*/
static boolean MP_GenerateRecordFilename(char *recordFile, char *filename, char *path_prefix, char* outExt, boolean addLocal)
{
  int       i = 0, j = 0, k = 0;
  char      c;

  if (strcmp(path_prefix, "") == 0)
  {
    (void)STRLCPY(path_prefix, MP_PATH_RECORDLOC_FLASH, MP_MAX_STRING_LENGTH);
  }

  while ((c = filename[i++]) != '\0')
  {
    if (c == '/')
      j = i;  /* find last path delimiter */

    if (c == '.')
      k = i;  /* find last extension delimiter */
   }

  if (k > j)
  {
    int len = STRLEN(path_prefix);
    int curLen = 0;
    int resultStrLen = 0;


    // Figure out the length of the resulting string, to make sure that
    // it will fit in the buffer

    // The path and base file name
    resultStrLen = len + k - j - 1;

    if (addLocal)
    {
      // We are adding the string '_local.' as part of the file name
      resultStrLen += 7;
    }
    else
    {
      // Just adding a '.'
      resultStrLen += 1;
    }

    // The length of the extension
    if (outExt && STRLEN(outExt) > 0)
      resultStrLen += STRLEN(outExt);
    else
      resultStrLen += 3;


    // The trailing '\0'
    resultStrLen++;

    if (MP_MAX_STRING_LENGTH <= resultStrLen)
    {
      // We don't have enough space in the char* for the full path of the file to record to
      // and the ending \0
      return FALSE;
    }

    (void)STRLCPY(recordFile, path_prefix, MP_MAX_STRING_LENGTH);
    (void)MEMCPY(recordFile + len, filename + j, k - j - 1);

    curLen = len + k - j - 1;
    recordFile[curLen] = '\0';
    if (addLocal)
    {
      (void)STRLCAT(recordFile, "_local.", MP_MAX_STRING_LENGTH);
      curLen += 7;
    }
    else
    {
      (void)STRLCAT(recordFile, ".", MP_MAX_STRING_LENGTH);
      curLen += 1;
    }
    if (outExt && STRLEN(outExt) > 0)
    {
      (void)STRLCAT(recordFile, outExt, MP_MAX_STRING_LENGTH);
      curLen += STRLEN(outExt);
    }
    else
    {
      (void)MEMCPY(recordFile + curLen, filename + k , 3);
      curLen += 3;
    }
    recordFile[curLen] = '\0';
  }
  else
  {
    /* No extension. The . if any came before the last slash. */
    int path_len = STRLEN(path_prefix);
    int file_len = STRLEN(filename);
    int curLen = 0;
    int resultStrLen = 0;



    // Figure out the length of the resulting string, to make sure that
    // it will fit in the buffer

    // The path and base file name
    resultStrLen = path_len + file_len - j;

    // are we adding the string '_local. as part of the file name
    if (addLocal)
      resultStrLen += 6;

    // Are we adding an extension, and a '.'
    if (outExt && STRLEN(outExt) > 0)
      resultStrLen += STRLEN(outExt) + 1;

    // The trailing '\0'
    resultStrLen++;

    if (MP_MAX_STRING_LENGTH <= resultStrLen)
    {
      // We don't have enough space in the char* for the full path of the file to record to
      // and the ending \0
      return FALSE;
    }

    (void)MEMSET(recordFile, '\0', MP_MAX_STRING_LENGTH);

    (void)STRLCPY(recordFile, path_prefix, MP_MAX_STRING_LENGTH);
    (void)MEMCPY(recordFile + path_len, filename + j, file_len - j);

    curLen = path_len + file_len - j;

    recordFile[path_len + curLen] = '\0';
    if (addLocal)
    {
      (void)STRLCAT(recordFile, "_local", MP_MAX_STRING_LENGTH);
      curLen += 6;
    }

    if (outExt && STRLEN(outExt) > 0)
    {
      (void)STRLCAT(recordFile, ".", MP_MAX_STRING_LENGTH);
      (void)MEMCPY(recordFile + curLen + 1, outExt, STRLEN(outExt));
      curLen += STRLEN(outExt) + 1;
    }

    recordFile[path_len + curLen] = '\0';
  }

  DBGPRINTF_MED("MP: Record:File Name--> %s", recordFile);
  return TRUE;  /* success */
}

/*===========================================================================
   This function generates the record filename given a base file name of an SDP
   file. The resulting name is 'filename_local.3gp'

   Assumption:  Storage has already been allocated for passed-in parameters
   and that the string length of recordFile is MP_MAX_STRING_LENGTH.
===========================================================================*/
static boolean MP_GetRecordFileName_SDP(char *recordFile, char *filename, char *path_prefix)
{
  return MP_GenerateRecordFilename(recordFile, filename, path_prefix, "3gp", TRUE);
}

#ifdef MP_FEATURE_VIDEO
/*===========================================================================
   This function generates the record filename given a url.  The resulting
   name is the file name on the remote server

   Assumption:  Storage has already been allocated for passed-in parameters
   and that the string length of recordFile is MP_MAX_STRING_LENGTH.
===========================================================================*/
static boolean MP_GetRecordFileName_Streaming(char *recordFile, char *pURL, char *path_prefix)
{
  return MP_GenerateRecordFilename(recordFile, pURL, path_prefix, NULL, FALSE);
}

#endif // ifdef MP_FEATURE_VIDEO

#endif //defined(FEATURE_QTV_STREAM_RECORD)

#if defined (FEATURE_APP_MPEG4) && !defined (FEATURE_APP_MEDIA_VIDEO)
#if (defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS))

/*************************************************************************/

/***** This function returns the next scaling type in a given        *****/

/***** direction. If the limit of scaling has been reached, return   *****/

/***** the same type as passed in.                                   *****/

/*************************************************************************/
static AEEMediaMPEG4ScalingType
MP4_GetNextScale(AEEMediaMPEG4ScalingType nCurScale, int ZoomType)
{
  AEEMediaMPEG4ScalingType Scaling;

  /*
   * currently DSP supports 2X up/down scaling
   */
  if (ZoomType == IDM_PM_ZOOM_IN)
  {
    event_report(EVENT_QTV_CLIP_ZOOM_IN);
    switch (nCurScale)
    {
#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#elif defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
    case MM_MPEG4_2X_ZOOM:
      /*
       * Saturate -- can't zoom in anymore
       */
      Scaling = MM_MPEG4_2X_ZOOM;
      break;

    case MM_MPEG4_NO_SCALING:
      Scaling = MM_MPEG4_2X_ZOOM;
      break;

    case MM_MPEG4_0P75X_SHRINK:
      Scaling = MM_MPEG4_NO_SCALING;
      break;

    case MM_MPEG4_2X_SHRINK:
      Scaling = MM_MPEG4_0P75X_SHRINK;
      break;

    case MM_MPEG4_0P25X_SHRINK:
      Scaling = MM_MPEG4_2X_SHRINK;
      break;

#else      // !FEATURE_QTV_IPL_SCALING &&
           // !FEATURE_QTV_MDP_TRANSFORMATIONS
    case MM_MPEG4_2X_ZOOM:
      /*
       * Saturate -- can't zoom in anymore
       */
      Scaling = MM_MPEG4_2X_ZOOM;
      break;

    case MM_MPEG4_NO_SCALING:
      Scaling = MM_MPEG4_2X_ZOOM;
      break;

    case MM_MPEG4_2X_SHRINK:
      Scaling = MM_MPEG4_NO_SCALING;
      break;

#endif     // FEATURE_QTV_IPL_SCALING
    default:
      Scaling = MM_MPEG4_NO_SCALING;
      break;

    }
  }
  else
  {
    event_report(EVENT_QTV_CLIP_ZOOM_OUT);
    switch (nCurScale)
    {
#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#elif defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
    case MM_MPEG4_2X_ZOOM:
      Scaling = MM_MPEG4_NO_SCALING;
      break;

    case MM_MPEG4_NO_SCALING:
      Scaling = MM_MPEG4_0P75X_SHRINK;
      break;

    case MM_MPEG4_0P75X_SHRINK:
      Scaling = MM_MPEG4_2X_SHRINK;
      break;

    case MM_MPEG4_2X_SHRINK:
      Scaling = MM_MPEG4_0P25X_SHRINK;
      break;

    case MM_MPEG4_0P25X_SHRINK:
      /*
       * Saturate -- can't zoom out anymore
       */
      Scaling = MM_MPEG4_0P25X_SHRINK;
      break;

#else      // !FEATURE_QTV_IPL_SCALING &&
           // !FEATURE_QTV_MDP_TRANSFORMATIONS
    case MM_MPEG4_2X_ZOOM:
      Scaling = MM_MPEG4_NO_SCALING;
      break;

    case MM_MPEG4_NO_SCALING:
      Scaling = MM_MPEG4_2X_SHRINK;
      break;

      /*
       * Saturate -- can't zoom out anymore
       */
    case MM_MPEG4_2X_SHRINK:
      Scaling = MM_MPEG4_2X_SHRINK;
      break;

#endif     // FEATURE_QTV_IPL_SCALING
    default:
      Scaling = MM_MPEG4_NO_SCALING;
      break;

    }
  }
  return Scaling;
}

/*************************************************************************/

/***** This function returns the next orientation after rotating     *****/

/***** a given orientation in a given direction.                     *****/

/*************************************************************************/
static AEEMediaMPEG4RotationType
MP4_GetNextRotation(AEEMediaMPEG4RotationType curRotation, int type)
{
  AEEMediaMPEG4RotationType Rotation;

  if (type == IDM_PM_CCW_ROTATE)
  {
    switch (curRotation)
    {
    case MM_MPEG4_90_CW_ROTATION:
      Rotation = MM_MPEG4_NO_ROTATION;
      break;

    case MM_MPEG4_90_CCW_ROTATION:
      Rotation = MM_MPEG4_180_ROTATION;
      break;

    case MM_MPEG4_180_ROTATION:
      Rotation = MM_MPEG4_90_CW_ROTATION;
      break;

    case MM_MPEG4_NO_ROTATION:
    default:
      Rotation = MM_MPEG4_90_CCW_ROTATION;
      break;
    };
  }
  else
  {
    switch (curRotation)
    {
    case MM_MPEG4_90_CW_ROTATION:
      Rotation = MM_MPEG4_180_ROTATION;
      break;

    case MM_MPEG4_90_CCW_ROTATION:
      Rotation = MM_MPEG4_NO_ROTATION;
      break;

    case MM_MPEG4_180_ROTATION:
      Rotation = MM_MPEG4_90_CCW_ROTATION;
      break;

    case MM_MPEG4_NO_ROTATION:
    default:
      Rotation = MM_MPEG4_90_CW_ROTATION;
      break;
    };
  }
  return Rotation;
}

#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS ||
            //FEATURE_QTV_MDP_TRANSFORMATIONS
#endif     // FEATURE_APP_MPEG4 && !defined (FEATURE_APP_MEDIA_VIDEO)


/*
 * ComputeSeekOffset computes the new offset (from the begining of the media)
 * that needs to be seeked too.  This is used for catching up on FF and REW
 * that happen while a Seek or Skip is in progress.
 * The return is the offset to seek to from MM_SEEK_START.
 */
static int ComputeSeekOffset(CPlayerWin *pMe)
{
  int       TargetPosition;

  DBGPRINTF_MED("MP: pMe->m_startSeekPosition = %d", pMe->m_startSeekPosition);
  DBGPRINTF_MED("MP: pMe->m_pendingSeekOffset = %d", pMe->m_pendingSeekOffset);
  TargetPosition = MAX(0, pMe->m_startSeekPosition + pMe->m_pendingSeekOffset);

  DBGPRINTF_MED("MP: TargetPosition = %d", TargetPosition);

  // If the clip duration is known, adjust the target position.
  if (pMe->m_dwDuration != 0)
  {
    DBGPRINTF_MED("MP: We know the duration, calculating new seek offset with it.");
    if (TargetPosition >= pMe->m_dwDuration)
    {
      // If seeking to end-of-clip, set to one second before end-of-clip or
      // current position if closer to end.
      TargetPosition = MAX((pMe->m_dwDuration - 1000), pMe->m_startSeekPosition);
    }
  }

  DBGPRINTF_MED("MP: TargetPosition = %d", TargetPosition);

  return TargetPosition;
}


/*==========================================================================
FUNCTION: MP_GetClipWidth() and MP_GetClipHeight()

DESCRIPTION:
Function to find clip width and height.

DEPENDENCIES
  None

RETURN VALUE
 None.

SIDE EFFECTS
  None
===========================================================================*/
uint16 MP_GetClipWidth(CPlayerWin *pwin)
{
  if (!pwin)
  {
    DBGPRINTF_HIGH("MP: MP_GetClipWidth pwin is NULL!");
    return 0;
  }
  if (!pwin->m_pMedia)
  {
    DBGPRINTF_HIGH("MP: MP_GetClipWidth m_pMedia is NULL!");
    return 0;
  }


#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
  {
	  AEECLSID ClsId;
	  int nRet;

	  nRet = IMEDIA_GetClassID(pwin->m_pMedia, &ClsId);
	  DBGPRINTF_MED("MP: IMEDIA_GetClassID() gets %d, returns %d = %s",
		  ClsId, nRet, MP_ErrorStateToString(nRet));

	  if (nRet == SUCCESS)
	  {
#ifdef FEATURE_APP_REAL
		  if (ClsId == AEECLSID_MEDIAREAL)
			  return (uint16)(pwin->m_mediaRealInfo.dwVideoWidth);
#endif
		  if (ClsId == AEECLSID_MEDIAMPEG4)
			  return (uint16)(pwin->m_mediaInfo.dwWidth);
	  }
  }
#endif //FEATURE_APP_MEDIA_VIDEO
  return 0;
}

uint16 MP_GetClipHeight(CPlayerWin *pwin)
{
  if (!pwin)
  {
    DBGPRINTF_HIGH("MP: MP_GetClipHeight pwin is NULL!");
    return 0;
  }
  if (!pwin->m_pMedia)
  {
    DBGPRINTF_HIGH("MP: MP_GetClipHeight m_pMedia is NULL!");
    return 0;
  }

#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
  {
	  AEECLSID ClsId;
	  int nRet;

	  nRet = IMEDIA_GetClassID(pwin->m_pMedia, &ClsId);
	  DBGPRINTF_MED("MP: IMEDIA_GetClassID() gets %d, returns %d = %s",
		  ClsId, nRet, MP_ErrorStateToString(nRet));
	  if (nRet == SUCCESS)
	  {
#ifdef FEATURE_APP_REAL
		  if (ClsId == AEECLSID_MEDIAREAL)
			  return (uint16)(pwin->m_mediaRealInfo.dwVideoHeight);
#endif
		  if (ClsId == AEECLSID_MEDIAMPEG4)
			  return (uint16)(pwin->m_mediaInfo.dwHeight);
	  }
  }
#endif //FEATURE_APP_MEDIA_VIDEO
  return 0;
}

/*==========================================================================
FUNCTION: CPlayerWin_SetAScale

DESCRIPTION:
Scales image based on current scaling and rotation parameters. If in fullscreen
mode, ignores ascaling parameters and scales image, maintaining aspect ratio without
cropping.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE  = Display scaled successfully.
  FALSE = Scaling failed.

SIDE EFFECTS:
  None
===========================================================================*/
#ifdef FEATURE_QTV_MDP_ASCALE 
static boolean CPlayerWin_SetAScale(CPlayerWin * pwin)
{
  int     nRet = 0;

  boolean bVerticalDisplay = FALSE;
  int     scaledX, scaledY, usTemp;
  int     scaleXPercentage, scaleYPercentage;
  int     cxWidth, cyHeight;
  int     clipWidth, clipHeight;

  // Scale the image according to the user's settings using MDP AScaling.
  scaleXPercentage = (pwin->m_pOwner->m_nAScaleXValue);
  scaleYPercentage = (pwin->m_pOwner->m_nAScaleYValue);
  DBGPRINTF_MED("MP: scaleXPercentage, scaleYPercentage = %d,%d", scaleXPercentage, scaleYPercentage);

  // Get Screen Dimensions
  cxWidth  = (pwin->m_pOwner->m_cxWidthFull); 
  cyHeight = (pwin->m_pOwner->m_cyHeightFull);
  DBGPRINTF_MED("MP: cxWidth, cyHeight = %d,%d", cxWidth, cyHeight);

  // Get the video dimensions
  clipWidth  = (int)MP_GetClipWidth(pwin);
  clipHeight = (int)MP_GetClipHeight(pwin);
  DBGPRINTF_MED("MP: clipWidth, clipHeight = %d,%d", clipWidth, clipHeight);

  // Return if there are not clip dimensions
  if ((clipWidth == 0) || (clipHeight == 0) || (scaleXPercentage == 0) || (scaleYPercentage == 0))
  {
    return FALSE;
  }

  // Determine whether we are displaying Vertically or Horizontally
  bVerticalDisplay = CPlayerWin_isVerticalDisplay(pwin);
  DBGPRINTF_MED("MP: bVerticalDisplay = %s", (bVerticalDisplay ? "TRUE" : "FALSE"));

  DBGPRINTF_MED("MP: bFullScreen = %s", (pwin->m_bFullScreen ? "TRUE" : "FALSE"));
  // Get the scaling dimensions - Handle Fullscreen mode
  if (pwin->m_bFullScreen)
  {
    // If Vertical, swap Height and Width values
    if (bVerticalDisplay) 
    {
      usTemp = clipWidth;
      clipWidth = clipHeight;
      clipHeight = usTemp;
    }

    // Determine whether Height or Width bounded (Assume Height bound)
    scaledX = (clipWidth * cyHeight) / clipHeight; 
    scaledY = cyHeight;

    // Check if width bound
    if ( scaledX > cxWidth ) // width bound 
    {
      scaledX = cxWidth;
      scaledY = (clipHeight * cxWidth) / clipWidth; 
    }
  }
  else    // Handle normal mode
  {
    // If Vertical, swap all Height and Width Variables
    if (bVerticalDisplay) 
    {
      usTemp = clipWidth;
      clipWidth = clipHeight;
      clipHeight = usTemp;

      usTemp = scaleXPercentage;
      scaleXPercentage = scaleYPercentage;
      scaleYPercentage = usTemp;
    }

    scaledX = clipWidth;
    scaledY = clipHeight;

    // Return if scaling unnecessary
    if ((scaleXPercentage != 100) || (scaleYPercentage != 100))
    {
      scaledX = (clipWidth * scaleXPercentage) / 100;
      scaledY = (clipHeight * scaleYPercentage) / 100;
    }
  }

  // Set the scaled parameters appropriately
  // Make sure neither dimension is being scaled > 4x (AScale limit)
  if (scaledX > clipWidth*4)
  {
    scaledX = clipWidth*4;
    DBGPRINTF("MP: adjusted scaledX to %d", scaledX);
  }
  if (scaledY > clipHeight*4)
  {
    scaledY = clipHeight*4;
    DBGPRINTF("MP: adjusted scaledY to %d", scaledY);
  }

  DBGPRINTF_MED("MP: scaledX,scaledY = %d,%d", scaledX, scaledY);
  nRet = IMEDIA_SetMediaParm(pwin->m_pMedia, MM_MP4_PARM_ASCALING, scaledX, scaledY);
  if (SUCCESS != nRet)
  {
    DBGPRINTF_MED("MP: IMEDIA_SetMediaParm(MM_MP4_PARM_ASCALING,%d,%d) returns %d = %s",
              scaledX, scaledY, nRet, MP_ErrorStateToString(nRet));
    return FALSE;
  }

  return TRUE;
}

/*==========================================================================
FUNCTION: CPlayerWin_isVerticalDisplay

DESCRIPTION:
Function checks to see if the image is rotated vertically in the display.  
It is vertical if it has been rotated either clockwise or counter-closckwise 
90 degrees.

DEPENDENCIES:
  None

RETURN VALUE:
 TRUE = display is vertical.

SIDE EFFECTS:
  None
===========================================================================*/
static boolean CPlayerWin_isVerticalDisplay(CPlayerWin *pme)
{ 
  boolean bVerticalDisplay = FALSE;

#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
  if( pme->m_Rotation == MM_MPEG4_90_CW_ROTATION || pme->m_Rotation == MM_MPEG4_90_CCW_ROTATION ) 
  {
    bVerticalDisplay = TRUE;
  }
#endif  // FEATURE_APP_MEDIA_VIDEO, FEATURE_APP_MPEG4

  return bVerticalDisplay;
}
#endif    // FEATURE_QTV_MDP_ASCALE

/*==========================================================================
FUNCTION: MP_ScreenSizeCheck

DESCRIPTION:
Function to determine if we have smaller screen size. If so, then we will
set the m_bSmallScreenSize flag to true. This is used to determine if we need to
display smaller player menu icons.

DEPENDENCIES:
  None

RETURN VALUE:
 None.

SIDE EFFECTS:
  None
===========================================================================*/
static void MP_ScreenSizeCheck(CMediaPlayer *pme)
{
  int sc1xScreenSize = 128;

  //Currently just checking for SC1X screen size.
  if ((pme->m_cyHeight <= sc1xScreenSize) &&  (pme->m_cxWidth <= sc1xScreenSize))
  {
    pme->m_bSmallScreenSize = TRUE;
    pme->m_IconViewCtlDy = 12;
  }
  else
  {
    pme->m_bSmallScreenSize = FALSE;
    pme->m_IconViewCtlDy = 18;
  }
}

#ifdef FEATURE_KEY_HOLD_SCROLL
static int
CPlayerWin_HandleKeyEvent(IWindow *po, AEEEvent eCode, uint16 wParam,
                          uint32 dwParam)
{
  CPlayerWin *pme = (CPlayerWin *)po;
  boolean   bHandled = FALSE;

  // We are processing EVT_KEY_PRESS and EVT_KEY_RELEASE events, however we need
  // to ensure that EVT_KEY events process correctly and don't return FALSE. If 
  // we end up returning false then some other app may try and process an event
  // intended for us with unexpected results, basically just return TRUE.
  if (MP_ISEVTKEY(eCode))
    return TRUE;

  if (eCode != EVT_KEY_PRESS && eCode != EVT_KEY_RELEASE)
    return -1;

  if (pme == NULL)
  {
    DBGPRINTF_ERROR("MP: Error: pme is NULL!");
    return FALSE;
  }
  if (CPlayerWin_IsMedia(pme) && pme->m_pMedia == NULL)
  {
    DBGPRINTF_ERROR("MP: Error: m_pMedia is NULL!");
    return FALSE;
  }

  switch (eCode)
  {
  case EVT_KEY_PRESS:
    /*
     * Generate a Mediaplayer Keypress event
     */
    MP_EventReportPayload(MP_KEYPRESS_EVENT_ID, wParam);

    /*
     * Divide this into 2 parts...one for media and one for image
     */
    if (CPlayerWin_IsImage(pme))
    {
      if (wParam == AVK_0)
      {
        // Check if Fullscreen is available
        CtlAddItem pai;
        if ( IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_FULLSCRN, &pai) )
        {
            (void)CPlayerWin_FullScreen(pme, (boolean) ! pme->m_bFullScreen, FALSE);
            IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_FULLSCRN);
        }
      }
      else
      {
        /*
         * display image in imagectrl mode
         */
        switch (wParam)
        {
        case AVK_SELECT:
          return IMENUCTL_HandleEvent(pme->m_pPlayerMenu, EVT_KEY, wParam,
                                      dwParam);
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
          /*
           * we need to prevent multi key press
           */
          if (pme->m_pOwner->m_KeyStore.m_wParam_Key == 0)
          {
            pme->m_pOwner->m_KeyStore.m_pIShell = pme->m_pIShell;
            pme->m_pOwner->m_KeyStore.m_pICtl = (IControl *)pme->m_pImageCtl;
            pme->m_pOwner->m_KeyStore.m_eCode = eCode;
            pme->m_pOwner->m_KeyStore.m_wParam_Key = wParam;
            pme->m_pOwner->m_KeyStore.m_dwParam_Key = dwParam;
            pme->m_pOwner->m_KeyStore.m_nTickTime = KEY_KEY_REPEAT_TIME_MS;
            pme->m_pOwner->m_KeyStore.m_bListMenu = FALSE;
            pme->m_pOwner->m_KeyStore.m_bIconMenu = FALSE;
            MP_OnKeyRepeatEvent(&pme->m_pOwner->m_KeyStore);
          }
          bHandled = TRUE;
          break;
        default:
          bHandled = TRUE;
          break;
        }
      }
    }
    else if (CPlayerWin_IsMedia(pme))
    {
      if (wParam == AVK_UP)
      {
        int     nRet;
        uint16  nNextLevel = pme->m_pOwner->m_wVolumeLevel + 1;
        uint16  wVol = (pme->m_pOwner->m_wNumVolumeLevels <= nNextLevel)? AEE_MAX_VOLUME : (nNextLevel * pme->m_pOwner->m_VolumeStep) + 2;

        DBGPRINTF_MED("MP: Our current volLevel=%d, next volLevel=%d, new vol=%d", pme->m_pOwner->m_wVolumeLevel, nNextLevel, wVol);

        wVol = (wVol > AEE_MAX_VOLUME) ? AEE_MAX_VOLUME : wVol;
        nRet  = IMEDIA_SetVolume(pme->m_pMedia, (uint16)wVol);
        DBGPRINTF_MED("MP: IMEDIA_SetVolume, vol=%d, nRet=%d[%s], m_pMedia=0x%x", wVol, nRet, MP_ErrorStateToString(nRet), pme->m_pMedia);
        if (SUCCESS == nRet)
        {
          CPlayerWin_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME, MM_STATUS_DONE, NULL, 0);
        }
        return TRUE;
      }
      else if (wParam == AVK_DOWN)
      {
        int    nRet;
        uint16 nNextLevel = 0;
        int16  nVol;

        if(pme->m_pOwner->m_wVolumeLevel > 0)
        {
           nNextLevel = pme->m_pOwner->m_wVolumeLevel - 1;
        }
        nVol = (nNextLevel * pme->m_pOwner->m_VolumeStep) + 2;

        nVol = (nVol < 0) ? 0 : nVol;
        nRet  = IMEDIA_SetVolume(pme->m_pMedia, (uint16)nVol);
        DBGPRINTF_MED("MP: IMEDIA_SetVolume, vol=%d, nRet=%d[%s]", nVol, nRet, MP_ErrorStateToString(nRet));
        if (SUCCESS == nRet)
        {
          CPlayerWin_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME,
                                 MM_STATUS_DONE, NULL, 0);
        }
        return TRUE;
      }

#ifdef FEATURE_MP_KEY_MAPPING
      if (CPlayerWin_MappedKeyEventHandler(po, eCode, wParam, dwParam))
        return TRUE;
#endif     // FEATURE_MP_KEY_MAPPING

#ifdef MP_FEATURE_VIDEO
#if (defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) && !defined (FEATURE_QTV_MDP_TRANSFORMATIONS))
      /*
       * If full screen and magnified, then update the display region so
       * the icon menu is not drawn over by the next frame in media clip.
       */
      if (pme->m_bFullScreen)
      {
#ifdef FEATURE_APP_MPEG4
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
        if (pme->m_dwZoomScale > 0)
        {
#endif // defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined(FEATURE_QTV_MDP_TRANSFORMATIONS)
#endif //FEATURE_APP_MPEG4
          DBGPRINTF_MED("MP: Ignoring setting to newRect"); // TAV
/*
          AEERect   iconRect;
          AEERect   newRect;

          IMENUCTL_GetRect(pme->m_pPlayerMenu, &iconRect);

          //
          // Use full screen coordinates, except for height.
          //
          newRect.x = 0;
          newRect.dx = pme->m_pOwner->m_cxWidth;
          newRect.y = 0;
          newRect.dy = iconRect.y;

          //
          // Only set a new clip rectangle if we need to.
          //
          if ((pme->m_rectImage.x != newRect.x) ||
              (pme->m_rectImage.y != newRect.y) ||
              (pme->m_rectImage.dx != newRect.dx) ||
              (pme->m_rectImage.dy != newRect.dy))
          {
            DBGPRINTF_MED("MP: Calling IMEDIA_SetRect with x = %d, y = %d, cx = %d, cy = %d", newRect.x, newRect.y, newRect.dx, newRect.dy);
            (void)IMEDIA_SetRect(pme->m_pMedia, &newRect, NULL);
            (void)MEMCPY(&pme->m_rectImage, &newRect, sizeof(AEERect));
          }
*/
#ifdef FEATURE_APP_MPEG4
        }
#endif //FEATURE_APP_MPEG4
      }
#endif     // FEATURE_MP4_FRAME_TRANSFORMATIONS && !FEATURE_QTV_MDP_TRANSFORMATIONS
#endif     // MP_FEATURE_VIDEO

      /*
       * If REWIND or FF being held down, then repeat the event.
       */
      if ((pme->m_pPlayerMenu != NULL) && (wParam == AVK_SELECT))
      {
        uint16    currentButton = IMENUCTL_GetSel(pme->m_pPlayerMenu);

        if ((pme->m_pOwner->m_KeyStore.m_wParam_Key == 0) &&
            ((currentButton == IDM_PM_REWIND) || (currentButton == IDM_PM_FF)))
        {
          pme->m_pOwner->m_KeyStore.m_pIShell = pme->m_pIShell;
          pme->m_pOwner->m_KeyStore.m_pICtl = (IControl *)pme->m_pPlayerMenu;
          pme->m_pOwner->m_KeyStore.m_eCode = eCode;
          pme->m_pOwner->m_KeyStore.m_wParam_Key = wParam;
          pme->m_pOwner->m_KeyStore.m_dwParam_Key = dwParam;
          pme->m_pOwner->m_KeyStore.m_nTickTime = KEY_ICON_MENU_REPEAT_TIME_MS;
          pme->m_pOwner->m_KeyStore.m_bListMenu = FALSE;
          pme->m_pOwner->m_KeyStore.m_bIconMenu = TRUE;
          MP_OnKeyRepeatEvent(&pme->m_pOwner->m_KeyStore);
          return TRUE;
        }
      }

      if(pme->m_pPlayerMenu != NULL)
        return IMENUCTL_HandleEvent(pme->m_pPlayerMenu, EVT_KEY, wParam, dwParam);
    }
    bHandled = TRUE;
    break;
  case EVT_KEY_RELEASE:
    (void)ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)MP_OnKeyRepeatEvent,
                             (void *)&pme->m_pOwner->m_KeyStore);
    pme->m_pOwner->m_KeyStore.m_wParam_Key = 0;
    pme->m_pOwner->m_KeyStore.m_dwParam_Key = 0;
    bHandled = TRUE;
    break;
  default:
    bHandled = FALSE;
    break;
  }
  return bHandled;
}

/*==========================================================================
FUNCTION MP_HandleKeyEventRepeat
DESCRIPTION
  handles EVT_KEY_PRESS and EVT_KEY_RELEASE, a timer will be invoked to
  send EVT_KEY event to specified ctrl repeatedly.

DEPENDENCIES
  None

RETURN VALUE
  return -1 for unhandled, 1 if event is handled

SIDE EFFECTS
  None
===========================================================================*/
int MP_HandleKeyEventRepeat(IWindow *po, AEEEvent eCode, uint16 wParam,
                     uint32 dwParam, IMenuCtl *pICtl, uint16 wTime)
{
  CWindow  *pme = (CWindow *)po;
  boolean   bHandled = FALSE;

  if (eCode != EVT_KEY_PRESS && eCode != EVT_KEY_RELEASE)
    return -1;

  if (pme == NULL || pICtl == NULL)
    return -1;

  switch (eCode)
  {
  case EVT_KEY_PRESS:
    switch (wParam)
    {
    case AVK_UP:
    case AVK_DOWN:
      /*
       * we need to prevent muti key press
       */
      if (pme->m_pOwner->m_KeyStore.m_wParam_Key == 0)
      {
        pme->m_pOwner->m_KeyStore.m_pIShell = pme->m_pIShell;
        pme->m_pOwner->m_KeyStore.m_pICtl = (IControl *)pICtl;
        pme->m_pOwner->m_KeyStore.m_eCode = eCode;
        pme->m_pOwner->m_KeyStore.m_wParam_Key = wParam;
        pme->m_pOwner->m_KeyStore.m_dwParam_Key = dwParam;
        pme->m_pOwner->m_KeyStore.m_nTickTime = wTime;
        pme->m_pOwner->m_KeyStore.m_bListMenu = TRUE;
        pme->m_pOwner->m_KeyStore.m_bIconMenu = FALSE;
        MP_OnKeyRepeatEvent(&pme->m_pOwner->m_KeyStore);
      }
      bHandled = TRUE;
      break;
    default:
      (void)ICONTROL_HandleEvent((IControl *)pICtl, EVT_KEY, wParam, dwParam);
      bHandled = TRUE;
      break;
    }
    break;
  case EVT_KEY_RELEASE:
    (void)ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)MP_OnKeyRepeatEvent,
                             (void *)&pme->m_pOwner->m_KeyStore);
    pme->m_pOwner->m_KeyStore.m_wParam_Key = 0;
    pme->m_pOwner->m_KeyStore.m_dwParam_Key = 0;
    bHandled = TRUE;
    break;
  default:
    bHandled = FALSE;                  // it is unlikely to reach here.
    break;
  }
  return bHandled;
}

/*==========================================================================
FUNCTION MP_OnKeyRepeatEvent
DESCRIPTION
  callback from shell timer and start timer again.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void MP_OnKeyRepeatEvent(void *pUser)
{
  int16     nItemCount = 0;
  int16     nFirstID = 0;
  int16     nLastID = 0;
  int16     selectID = 0;

  CKeyStore *pk = (CKeyStore *)pUser;

  if (pUser == 0)
    return;

  if (pk->m_wParam_Key == 0 || pk->m_pICtl == NULL)
    return;

  if (pk->m_bListMenu)
  {
    nItemCount = IMENUCTL_GetItemCount((IMenuCtl *)pk->m_pICtl);
    if (nItemCount)
    {
      nFirstID = IMENUCTL_GetItemID((IMenuCtl *)pk->m_pICtl, 0);
      nLastID = IMENUCTL_GetItemID((IMenuCtl *)pk->m_pICtl, nItemCount - 1);
      selectID = IMENUCTL_GetSel((IMenuCtl *)pk->m_pICtl);
    }

    /*
     * if last menu item is currently selected and DOWN key pressed,
     * set selection on the first one
     * if first menu item is current selected and the UP key pressed,
     * set selection on the last one
     */
    if (nItemCount && pk->m_wParam_Key == AVK_DOWN && selectID == nLastID)
    {
      IMENUCTL_SetSel((IMenuCtl *)pk->m_pICtl, nFirstID);
    }
    else if (nItemCount && pk->m_wParam_Key == AVK_UP && selectID == nFirstID)
    {
      IMENUCTL_SetSel((IMenuCtl *)pk->m_pICtl, nLastID);
    }
    else
    {
      /*
       * send event to menu control
       */
      (void)ICONTROL_HandleEvent((IControl *)pk->m_pICtl, EVT_KEY, pk->m_wParam_Key,
                           pk->m_dwParam_Key);
    }
  }
  else if (pk->m_bIconMenu)
  {
    (void)IMENUCTL_HandleEvent((IMenuCtl *)pk->m_pICtl, EVT_KEY, pk->m_wParam_Key,
                         pk->m_dwParam_Key);
  }
  else                                 // image control
  {
    (void)ICONTROL_HandleEvent((IControl *)pk->m_pICtl, EVT_KEY, pk->m_wParam_Key,
                         pk->m_dwParam_Key);
  }

  /*
   * start the timer again. if failed, reset m_wParam_Key
   */
  if (SUCCESS !=
      ISHELL_SetTimer(pk->m_pIShell, (int32)pk->m_nTickTime,
                      (PFNNOTIFY)MP_OnKeyRepeatEvent, (void *)pk))
  {
    pk->m_wParam_Key = 0;
  }
}

#endif     // FEATURE_KEY_HOLD_SCROLL

#ifdef FEATURE_MP_KEY_MAPPING
static void
CPlayerWin_ShowProgCtlText(CPlayerWin *pme, char *strText)
{
  AECHAR    szBuf[MP_MAX_STRLEN] = { '\0' };

  (void)STRTOWSTR(strText, szBuf, sizeof(szBuf));
  DBGPRINTF_MED("MP: %s", strText);

  /*
   * these will block some messages, we can't do it until we have
   * some kind of event message queue implemented
   */
#if 0
  pme->m_bProgTimer = FALSE;
  CProgCtl_SetPos(&pme->m_ProgCtl, szBuf, pme->m_wPct);
  IDISPLAY_Update(pme->m_pIDisplay);
  pme->m_bProgTimer = TRUE;
  /*
   * Set the prog timer so that the display stays for few seconds.
   */
  ISHELL_SetTimer(pme->m_pIShell, 1, (PFNNOTIFY)CPlayerWin_ProgTimerNotify, pme);
#else
  // Previously used for debugging.
  // Uncomment following line if timed messages needed. 
  // MPProgCtl_TimedMessage(pme->m_pMPProgCtl, szBuf);
#endif
}

#endif     // FEATURE_MP_KEY_MAPPING

#ifdef FEATURE_MP_RECORDER_SETTINGS
/*===========================================================================

                     CRecorderSettingWin_New Functions

===========================================================================*/

/*===========================================================================
   This function constructs memo recorder setting window.
===========================================================================*/
static IWindow *
CRecorderSettingWin_New(CMediaPlayer *pOwner)
{
  CRecorderSettingWin *pme = NULL;

  VTBL(IWindow)vtbl;

  MP_IWINDOW_SETVTBL(&vtbl, CRecorderSettingsWin_Enable,
                     CRecorderSettingsWin_Redraw,
                     CRecorderSettingsWin_HandleEvent,
                     CRecorderSettingsWin_Delete);
  pme =
    (CRecorderSettingWin *)CWindow_New(sizeof(CRecorderSettingWin), pOwner,
                                        &vtbl);
  if (pme)
  {
    int       cx;
    int       cy;
    int       y,
              dy;
    AEERect   rc;

    pme->m_pMenu = NULL;
    pme->m_pSubMenu = NULL;
    pme->m_pOwner->m_State = MP_STATE_SETTING;
    EMIT_MEDIAPLAYER_STATE(pme->m_pOwner->m_State);

    if (ISHELL_CreateInstance
        (pme->m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pme->m_pMenu))
    {
    	DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
      MP_WINERR_RETURN(pme)
    }
    if (ISHELL_CreateInstance
        (pme->m_pIShell, AEECLSID_MENUCTL, (void **)&pme->m_pSubMenu))
    {
    	DBGPRINTF_MED("MP: calling MP_WINERR_RETURN");
      MP_WINERR_RETURN(pme)
    }


    cx = pme->m_pOwner->m_cxWidth;
    cy = pme->m_pOwner->m_cyHeight;
    dy = pme->m_pOwner->m_IconViewCtlDy;
    y = cy - dy;
    SETAEERECT(&rc, 0, y, cx, dy);
    /*
     * set up button bar menu
     */
    MP_SetMenuAttr(pme->m_pMenu, AEECLSID_SOFTKEYCTL,
                   pme->m_pOwner->m_nColorDepth, &rc, MP_WRAPSCROLL);

    cy = pme->m_pOwner->m_cyHeight;
    y = 0;
    dy = cy - pme->m_pOwner->m_IconViewCtlDy;
    SETAEERECT(&rc, 0, y, cx, dy);
    /*
     * set up sub menu
     */
    MP_SetMenuAttr(pme->m_pSubMenu, AEECLSID_MENUCTL,
                   pme->m_pOwner->m_nColorDepth, &rc, MP_WRAPSCROLL);


    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pMenu, 0, NULL, IDB_AUDIO_FORMAT, IDS_AUDIO_FORMAT, 0);
#ifdef FEATURE_AAC_REC
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pMenu, 0, NULL, IDB_SAMPLE_RATE, IDS_AUDIO_SAMPLE_RATE, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pMenu, 0, NULL, IDB_CHANNEL, IDS_AUDIO_CHANNEL, 0);
#endif //FEATURE_AAC_REC
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pMenu, 0, NULL, IDB_RECORD_PATH, IDS_RECORD_PATH, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pMenu, 0, NULL, IDB_DELETE_FILE, IDS_DELETE_FILE, 0);
    (void)MP_AddPlayerMenuItem(pme->m_pOwner, pme->m_pMenu, 0, NULL, IDB_SAVE_LOCATION, IDS_SAVE_LOCATION, 0);
  }

  return (IWindow *)pme;
}

/*===========================================================================
   This function enables/disables the Recorder Settings window.
===========================================================================*/
static void
CRecorderSettingsWin_Enable(IWindow *po, boolean bEnable)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  boolean   bActive;

  if (!CWindow_ProcessEnable(po, bEnable))
    return;

  bActive = pme->m_bActive;
  IMENUCTL_SetActive(pme->m_pMenu, bActive);
  IMENUCTL_SetActive(pme->m_pSubMenu, bActive);
  if (pme->m_bActive)
  {
    IMENUCTL_SetSel(pme->m_pMenu, (uint16)IDS_AUDIO_FORMAT);
    CRecorderSettingsWin_BuildSubMenu(po);
  }
}

/*===========================================================================
   This function deletes object in recorder setting window.
===========================================================================*/
static void
CRecorderSettingsWin_Delete(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  CMediaPlayer *pMP = pme->m_pOwner;

  if (pme->m_pMenu)
    MP_RELEASEIF(pme->m_pMenu);
  if (pme->m_pSubMenu)
    MP_RELEASEIF(pme->m_pSubMenu);
  pme->m_pMenu = NULL;
  pme->m_pSubMenu = NULL;
  FREE(pme);

  if(pMP)
  {
    pMP->m_pWin = NULL;
    pMP->m_eActiveWin = MPW_NONE;
  }
}

/*===========================================================================
   This function redraws the Recorder Settings Window.
===========================================================================*/
static void
CRecorderSettingsWin_Redraw(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;

  if (!pme->m_bActive)
    return;

  IDISPLAY_ClearScreen(pme->m_pIDisplay);

  (void)IMENUCTL_Redraw(pme->m_pMenu);
  (void)IMENUCTL_Redraw(pme->m_pSubMenu);

  IDISPLAY_Update(pme->m_pIDisplay);
}

/*===========================================================================
   This function processes events routed to the recorder setting window.
===========================================================================*/
static boolean
CRecorderSettingsWin_HandleEvent(IWindow *po, AEEEvent eCode, uint16 wParam,
                                 uint32 dwParam)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;

  switch (eCode)
  {
  case EVT_KEY:
    (void)IMENUCTL_HandleEvent(pme->m_pMenu, eCode, wParam, 0);
    (void)IMENUCTL_HandleEvent(pme->m_pSubMenu, eCode, wParam, 0);
    return CRecorderSettingsWin_SettingsKeyHandler(po, eCode, wParam);
  case EVT_COMMAND:
    switch (wParam)
    {
    case IDS_AUDIO_FORMAT:
    case IDS_AUDIO_SAMPLE_RATE:
    case IDS_AUDIO_CHANNEL:
    case IDS_RECORD_PATH:
    case IDB_SAVE_LOCATION:
    case IDS_DELETE_FILE:
      CRecorderSettingsWin_BuildSubMenu(po);
      return TRUE;
    default:
      break;
    }
    break;

  default:
    break;
  }
  return FALSE;
}

/*===========================================================================
   This function processes handle key events for recorder setting window.
===========================================================================*/
static boolean
CRecorderSettingsWin_SettingsKeyHandler(IWindow *po, AEEEvent eCode,
                                        uint16 wParam)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  boolean   result = FALSE;

  switch (wParam)
  {
  case AVK_SELECT:
    CRecorderSettingsWin_MarkSelection(po);
    (void)CRecorderSettingsWin_DeleteSelectedFile(po);
    result = TRUE;
    break;
  case AVK_LEFT:
  case AVK_RIGHT:
    CRecorderSettingsWin_BuildSubMenu(po);
    CRecorderSettingsWin_MarkSelection(po);
    result = TRUE;
    break;
  case AVK_UP:
  case AVK_DOWN:
    result = TRUE;
    break;
  case AVK_CLR:
  {
    if (pme->m_ParentWin == IDM_MAIN_SETTINGS)
    {
      /*
       * we were entering from main setting window.
       */
      CMediaPlayer_Settings(pme->m_pOwner);
      result = TRUE;
    }
    else
    {
      /*
       * entered from recorder window
       */
      char     *strNewFile =
        CRecorderSettingsWin_GetRecordingFileName(pme->m_pOwner);
      if (strNewFile)
      {
        CMediaPlayer_RecordQCPFile(pme->m_pOwner, strNewFile);
        FREEIF(strNewFile);
        result = TRUE;
      }
    }
    break;
  }
  default:
    break;
  }
  return result;
}

/*===========================================================================
   call this function to build sub menu
===========================================================================*/
static void
CRecorderSettingsWin_BuildSubMenu(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  int       nSubMenuID = IMENUCTL_GetSel(pme->m_pMenu);

  switch (nSubMenuID)
  {
  case IDS_AUDIO_FORMAT:
    CRecorderSettingsWin_BuildAudioFormatSubMenu(po);
    break;
  case IDS_AUDIO_SAMPLE_RATE:
    CRecorderSettingsWin_BuildAudioSampleRateSubMenu(po);
    break;
  case IDS_AUDIO_CHANNEL:
    CRecorderSettingsWin_BuildAudioChannelSubMenu(po);
    break;
  case IDS_RECORD_PATH:
    CRecorderSettingsWin_BuildRecordPathSubMenu(po);
    break;
  case IDS_SAVE_LOCATION:
    CRecorderSettingsWin_BuildSaveLocationSubMenu(po);
    break;
  case IDS_DELETE_FILE:
    CRecorderSettingsWin_BuildDeleteFileMenu(po);
    break;
  default:
    DBGPRINTF_HIGH("MP: asked to build unknown submenu ID = %d!", nSubMenuID);
    break;
  }
}

/*===========================================================================
   This function build AudioFormat sub menu
===========================================================================*/
static void
CRecorderSettingsWin_BuildAudioFormatSubMenu(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  CMediaPlayer *pMp;
  uint16    i = 0;
  uint16    selecteditem = MM_QCP_FORMAT_FIXED_FULL_13K;
  uint16    menuitems;
  MP_Size_16 item;
  CtlAddItem pai;
  boolean   bFind = FALSE;

  /*
   * Delete any previous sub_menu
   */
  (void)IMENUCTL_DeleteAll(pme->m_pSubMenu);
  /*
   * Set Title
   */
  (void)IMENUCTL_SetTitle(pme->m_pSubMenu, MEDIAPLAYER_RES_FILE, IDS_AUDIO_FORMAT, NULL);

  pMp = pme->m_pOwner;
  menuitems = sizeof(RecorderAudioSettings) / sizeof(MP_Size_16);
  for (i = 0; i < menuitems; i++)
  {
    item = RecorderAudioSettings[i];
    if (pMp->m_AudioFormat == item.cy)
    {
      (void)MP_AddMenuItem(pme->m_pSubMenu, item.cx, NULL, IDB_RADIO_ON, item.cx, 0);
      selecteditem = item.cx;
      bFind = TRUE;
    }
    else
      (void)MP_AddMenuItem(pme->m_pSubMenu, item.cx, NULL, IDB_RADIO_OFF, item.cx, 0);
  }

  if (!bFind)
  {
    /*
     * select the first menu item and set image
     */
    item = RecorderAudioSettings[0];
    if (IMENUCTL_GetItem(pme->m_pSubMenu, item.cx, &pai))
    {
      pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
      pai.pImage = NULL;
      pai.wImage = (uint16)IDB_RADIO_ON;
      selecteditem = item.cx;
      pMp->m_AudioFormat = item.cy;
      (void)IMENUCTL_SetItem(pme->m_pSubMenu, item.cx, MSIF_IMAGE, &pai);
    }
  }
  IMENUCTL_SetActive(pme->m_pSubMenu, TRUE);
  IMENUCTL_SetSel(pme->m_pSubMenu, selecteditem);
  (void)IMENUCTL_Redraw(pme->m_pSubMenu);
}
/*===========================================================================
   This function build Record sample rate sub menu
===========================================================================*/
static void CRecorderSettingsWin_BuildAudioSampleRateSubMenu(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  CMediaPlayer *pMp;
  uint16    i = 0;
  uint16    selecteditem = CMX_AF_SAMPLE_RATE_8000;
  uint16    menuitems;
  MP_Size_16 item;
  CtlAddItem pai;
  boolean   bFind = FALSE;

  /*
   * Delete any previous sub_menu
   */
  (void)IMENUCTL_DeleteAll(pme->m_pSubMenu);
  /*
   * Set Title
   */
  (void)IMENUCTL_SetTitle(pme->m_pSubMenu, MEDIAPLAYER_RES_FILE, IDS_AUDIO_SAMPLE_RATE, NULL);

  pMp = pme->m_pOwner;
  menuitems = sizeof(RecorderSampleRateSettings) / sizeof(MP_Size_16);
  for (i = 0; i < menuitems; i++)
  {
    item = RecorderSampleRateSettings[i];
    if (pMp->m_SampleRate == item.cy)
    {
      (void)MP_AddMenuItem(pme->m_pSubMenu, item.cx, NULL, IDB_RADIO_ON, item.cx, 0);
      selecteditem = item.cx;
      bFind = TRUE;
    }
    else
      (void)MP_AddMenuItem(pme->m_pSubMenu, item.cx, NULL, IDB_RADIO_OFF, item.cx, 0);
  }

  if (!bFind)
  {
    /*
     * select the first menu item and set image
     */
    item = RecorderSampleRateSettings[0];
    if (IMENUCTL_GetItem(pme->m_pSubMenu, item.cx, &pai))
    {
      pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
      pai.pImage = NULL;
      pai.wImage = (uint16)IDB_RADIO_ON;
      selecteditem = item.cx;
      pMp->m_RecordPath = item.cy;

      (void)IMENUCTL_SetItem(pme->m_pSubMenu, item.cx, MSIF_IMAGE, &pai);
    }
  }
  IMENUCTL_SetActive(pme->m_pSubMenu, TRUE);
  IMENUCTL_SetSel(pme->m_pSubMenu, selecteditem);
  (void)IMENUCTL_Redraw(pme->m_pSubMenu);
}
/*===========================================================================
   This function build Record channel sub menu
===========================================================================*/
static void CRecorderSettingsWin_BuildAudioChannelSubMenu(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  CMediaPlayer *pMp;
  uint16    i = 0;
  uint16    selecteditem = CMX_AF_AAC_CHANNEL_MONO;
  uint16    menuitems;
  MP_Size_16 item;
  CtlAddItem pai;
  boolean   bFind = FALSE;

  /*
   * Delete any previous sub_menu
   */
  (void)IMENUCTL_DeleteAll(pme->m_pSubMenu);
  /*
   * Set Title
   */
  (void)IMENUCTL_SetTitle(pme->m_pSubMenu, MEDIAPLAYER_RES_FILE, IDS_AUDIO_CHANNEL, NULL);

  pMp = pme->m_pOwner;
  menuitems = sizeof(RecorderChannelSettings) / sizeof(MP_Size_16);
  for (i = 0; i < menuitems; i++)
  {
    item = RecorderChannelSettings[i];
    if (pMp->m_AudioChannel == item.cy)
    {
      (void)MP_AddMenuItem(pme->m_pSubMenu, item.cx, NULL, IDB_RADIO_ON, item.cx, 0);
      selecteditem = item.cx;
      bFind = TRUE;
    }
    else
      (void)MP_AddMenuItem(pme->m_pSubMenu, item.cx, NULL, IDB_RADIO_OFF, item.cx, 0);
  }

  if (!bFind)
  {
    /*
     * select the first menu item and set image
     */
    item = RecorderChannelSettings[0];
    if (IMENUCTL_GetItem(pme->m_pSubMenu, item.cx, &pai))
    {
      pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
      pai.pImage = NULL;
      pai.wImage = (uint16)IDB_RADIO_ON;
      selecteditem = item.cx;
      pMp->m_RecordPath = item.cy;

      (void)IMENUCTL_SetItem(pme->m_pSubMenu, item.cx, MSIF_IMAGE, &pai);
    }
  }
  IMENUCTL_SetActive(pme->m_pSubMenu, TRUE);
  IMENUCTL_SetSel(pme->m_pSubMenu, selecteditem);
  (void)IMENUCTL_Redraw(pme->m_pSubMenu);
}

/*===========================================================================
   This function build Record Path sub menu
===========================================================================*/
static void
CRecorderSettingsWin_BuildRecordPathSubMenu(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  CMediaPlayer *pMp;
  uint16    i = 0;
  uint16    selecteditem = MM_APATH_LOCAL;
  uint16    menuitems;
  MP_Size_16 item;
  CtlAddItem pai;
  boolean   bFind = FALSE;

  /*
   * Delete any previous sub_menu
   */
  (void)IMENUCTL_DeleteAll(pme->m_pSubMenu);
  /*
   * Set Title
   */
  (void)IMENUCTL_SetTitle(pme->m_pSubMenu, MEDIAPLAYER_RES_FILE, IDS_RECORD_PATH, NULL);

  pMp = pme->m_pOwner;
  menuitems = sizeof(RecorderPathSettings) / sizeof(MP_Size_16);
  for (i = 0; i < menuitems; i++)
  {
    item = RecorderPathSettings[i];
    if (pMp->m_RecordPath == item.cy)
    {
      (void)MP_AddMenuItem(pme->m_pSubMenu, item.cx, NULL, IDB_RADIO_ON, item.cx, 0);
      selecteditem = item.cx;
      bFind = TRUE;
    }
    else
      (void)MP_AddMenuItem(pme->m_pSubMenu, item.cx, NULL, IDB_RADIO_OFF, item.cx, 0);
  }

  if (!bFind)
  {
    /*
     * select the first menu item and set image
     */
    item = RecorderPathSettings[0];
    if (IMENUCTL_GetItem(pme->m_pSubMenu, item.cx, &pai))
    {
      pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
      pai.pImage = NULL;
      pai.wImage = (uint16)IDB_RADIO_ON;
      selecteditem = item.cx;
      pMp->m_RecordPath = item.cy;

      (void)IMENUCTL_SetItem(pme->m_pSubMenu, item.cx, MSIF_IMAGE, &pai);
    }
  }
  IMENUCTL_SetActive(pme->m_pSubMenu, TRUE);
  IMENUCTL_SetSel(pme->m_pSubMenu, selecteditem);
  (void)IMENUCTL_Redraw(pme->m_pSubMenu);
}

/*===========================================================================
   This function build save location sub menu
===========================================================================*/
static void
CRecorderSettingsWin_BuildSaveLocationSubMenu(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  CMediaPlayer *pMp;
  uint16    i = 0;
  uint16    selecteditem = SAVELOC_FLASH;
  uint16    menuitems;
  MP_Size_16 item;
  CtlAddItem pai;
  IFileMgr *pIFileMgr;
  boolean   bFind = FALSE;
  boolean   bMMC = FALSE;
  boolean   bUIM = FALSE;
#if MIN_BREW_VERSION(4,0)
  char pszUIMStoragePath[MP_MAX_FILE_NAME]= {0};
#endif //#if MIN_BREW_VERSION(4,0)

  /*
   * Delete any previous sub_menu
   */
  (void)IMENUCTL_DeleteAll(pme->m_pSubMenu);
  /*
   * Set Title
   */
  (void)IMENUCTL_SetTitle(pme->m_pSubMenu, MEDIAPLAYER_RES_FILE, IDS_SAVE_LOCATION, NULL);

  pMp = pme->m_pOwner;
  menuitems = sizeof(RecorderSaveLocation) / sizeof(MP_Size_16);

  (void)ISHELL_CreateInstance(pme->m_pOwner->a.m_pIShell, AEECLSID_FILEMGR,
                              (void **)&pIFileMgr);
  if (pIFileMgr)
  {
    /*
     * test root on MMC directory. Create one if it doesn?t exist
     */
    bMMC = MP_MakeDir(pIFileMgr, MP_MMC_MEDIA_DIR);

    // test root on UIM directory.  Create one if it doesn't exist
#if MIN_BREW_VERSION(4,0)
  (void)MEMSET(pszUIMStoragePath, 0, sizeof (pszUIMStoragePath) );
  if (MP_GetUIMMediaPlayerRoot(pIFileMgr, pszUIMStoragePath, sizeof (pszUIMStoragePath)))
  {
    bUIM = TRUE;
  }
#endif //#if MIN_BREW_VERSION(4,0)

    MP_RELEASEIF(pIFileMgr);
  }

  for (i = 0; i < menuitems; i++)
  {
    item = RecorderSaveLocation[i];
    // if no MMC, skip
    if ( !bMMC && item.cy == SAVELOC_MMC)
      continue;      
    // if no UIM, skip
    if ( !bUIM && item.cy == SAVELOC_UIM)
      continue;                     
    if (pMp->m_SaveLocation == item.cy)
    {
      (void)MP_AddMenuItem(pme->m_pSubMenu, item.cx, NULL, IDB_RADIO_ON, item.cx, 0);
      selecteditem = item.cx;
      bFind = TRUE;
    }
    else
      (void)MP_AddMenuItem(pme->m_pSubMenu, item.cx, NULL, IDB_RADIO_OFF, item.cx, 0);
  }

  if (!bFind)
  {
    /*
     * select the first menu item and set image
     */
    item = RecorderSaveLocation[0];
    if (IMENUCTL_GetItem(pme->m_pSubMenu, item.cx, &pai))
    {
      pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
      pai.pImage = NULL;
      pai.wImage = (uint16)IDB_RADIO_ON;
      selecteditem = item.cx;
      if (pMp->m_SaveLocation != item.cy)
      {
        pMp->m_SaveLocation = item.cy;
      }
      (void)IMENUCTL_SetItem(pme->m_pSubMenu, item.cx, MSIF_IMAGE, &pai);
    }
  }
  IMENUCTL_SetActive(pme->m_pSubMenu, TRUE);
  IMENUCTL_SetSel(pme->m_pSubMenu, selecteditem);
  (void)IMENUCTL_Redraw(pme->m_pSubMenu);
}
// this function build delete file menu
static void CRecorderSettingsWin_BuildDeleteFileMenu(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  (void)CRecorderSettingsWin_MakeList(po, &pme->m_pOwner->m_FileExplorer);
}
// delete given file from memory
static int MP_DeleteFile(CMediaPlayer *pme, IFileMgr *pIFileMgr, const char* pDir, const AECHAR* pszName)
{
  char*          pFileName;
  AEEFileInfoEx  fi;
  int            nRet = EFAILED;
  AECHAR         *szBuf;

  if(pIFileMgr== NULL || pDir == NULL || pszName == NULL)
  {
    DBGPRINTF_ERROR("MP: MP_DeleteFile(), called with wrong arg.");
    return nRet;
  }

  pFileName = MALLOC(MP_MAX_FILE_NAME);
  if(!pFileName)
  {
    DBGPRINTF_ERROR("MP: cannot allocate memory.");
    return nRet;
  }

  // initialize fi for enumeration
  (void)MEMSET (pFileName, 0, MP_MAX_FILE_NAME);
  (void)MEMSET(&fi, 0, sizeof(fi));
  fi.nStructSize = sizeof(fi);
  fi.pszFile = pFileName;
  fi.nMaxFile = MP_MAX_FILE_NAME;

  szBuf = MALLOC(MP_MAX_FILE_NAME *sizeof(AECHAR));
  if (!szBuf)
  {
    DBGPRINTF_ERROR("MP: cannot allocate memory.");
    FREE(pFileName);
    return nRet;
  }

  // list file in media dir
  if (IFILEMGR_EnumInit(pIFileMgr, pDir, FALSE) == SUCCESS)
  {
    while (IFILEMGR_EnumNextEx(pIFileMgr, &fi))
    {
      char     *fiName;
      //  Get the filename.
      fiName = MP_GetFileName(fi.pszFile);
      (void)STRTOWSTR(fiName, szBuf, MP_MAX_FILE_NAME);

      if (WSTRCMP(pszName, szBuf) == 0)
      {
        DBGPRINTF_HIGH("MP: deleting file: %s.", fi.pszFile);
//#ifdef MP_FEATURE_ICONTENT
//        if(pme->m_bIContentMode)
//        {
//          MP_DeleteContent(pme, fi.pszFile);
//        }
//        else
//#endif //MP_FEATURE_ICONTENT
        {
          if(SUCCESS == (nRet = IFILEMGR_Remove(pIFileMgr, fi.pszFile)))
            DBGPRINTF_MED("MP: successfully delete file: %s.", fi.pszFile);
          else
            DBGPRINTF_ERROR("MP: failed to delete file: %s.", fi.pszFile);

        }
      }
    }
  }

  FREE(szBuf);
  FREE(pFileName);
  return nRet;
}

/*===========================================================================
 Prepares the menu with list of files and directory in explorer
===========================================================================*/
static boolean CRecorderSettingsWin_MakeList(IWindow *po, CFileExplore *pFileExp)
{
  char*          pFileName;
  IFileMgr      *pIFileMgr;
  AEEFileInfoEx  fi;
  char           *pBuf;
  int16          i = 0;
  AECHAR         *pwszBuf;

  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  if(pme == NULL)
  {
    DBGPRINTF_ERROR("MP: NULL ptr.");
    return FALSE;
  }

  (void)IMENUCTL_SetTitle(pme->m_pSubMenu, MEDIAPLAYER_RES_FILE, IDS_DELETE_MENU_TITLE, NULL);

  if (ISHELL_CreateInstance(pme->m_pOwner->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr) != SUCCESS)
  {
    DBGPRINTF_ERROR("MP: failed to create file manager.");
    return FALSE;
  }

  pBuf = (char *)MALLOC((RECORDER_MAX_FILES + 1) * sizeof(char));
  if (!pBuf)
  {
    MP_RELEASEIF(pIFileMgr);
    DBGPRINTF_ERROR("MP: cannot allocate memory.");
    return FALSE;
  }

  pFileName = MALLOC(MP_MAX_FILE_NAME);
  if(!pFileName)
  {
    FREE(pBuf);
    MP_RELEASEIF(pIFileMgr);
    DBGPRINTF_ERROR("MP: cannot allocate memory.");
    return FALSE;
  }

  // initialize fi for enumeration
  (void)MEMSET(pBuf, 0, sizeof(pBuf));
  (void)MEMSET (pFileName, 0, MP_MAX_FILE_NAME);
  (void)MEMSET(&fi, 0, sizeof(fi));
  fi.nStructSize = sizeof(fi);
  fi.pszFile = pFileName;
  fi.nMaxFile = MP_MAX_FILE_NAME;

  pwszBuf = MALLOC(MP_MAX_FILE_NAME *sizeof(AECHAR));
  if (!pwszBuf)
  {
    FREE(pBuf);
    FREE(pFileName);
    MP_RELEASEIF(pIFileMgr);
    DBGPRINTF_ERROR("MP: cannot allocate memory.");
    return FALSE;
  }

  (void)IMENUCTL_DeleteAll(pme->m_pSubMenu);
  // list file in media dir
  if (IFILEMGR_EnumInit(pIFileMgr, MP_MEDIA_DIR, FALSE) == SUCCESS)
  {
    while (IFILEMGR_EnumNextEx(pIFileMgr, &fi))
    {
      char     *fiName;
      uint16    wIconID;
      /*
       * Get the filename.
       */
      fiName = MP_GetFileName(fi.pszFile);
      if(IsMemofile(fiName))
      {
        // add this file into list
        (void)STRTOWSTR(fiName, pwszBuf, MP_MAX_FILE_NAME);
        wIconID = CMediaPlayer_FindHandlerType(pme->m_pOwner, fiName);
        (void)MP_AddMenuItem(pme->m_pSubMenu, 0, pwszBuf, wIconID, (uint16)(EVT_START_FILELIST + i), 0);
        i++;
      }
    }
  }

  FREE(pwszBuf);
  FREE(pBuf);
  FREE(pFileName);
  MP_RELEASEIF(pIFileMgr);

  IMENUCTL_SetActive(pme->m_pSubMenu, TRUE);
  IMENUCTL_SetSel(pme->m_pSubMenu, 0);
  (void)IMENUCTL_Redraw(pme->m_pSubMenu);
  return TRUE;
}

// this function verify if the file is mediaplyer recorded memo file
static boolean IsMemofile(char *szBuf)
{
  if(NULL != STRSTR(szBuf, "memo_"))
  {
    if (NULL != STRSTR(szBuf, "memo_13k_"))
    {
      return TRUE;
    }
    else if(NULL != STRSTR(szBuf, "memo_evrc_"))
    {
      return TRUE;
    }
    else if(NULL != STRSTR(szBuf, "memo_evrcb_"))
    {
      return TRUE;
    }
    else if(NULL != STRSTR(szBuf, "memo_amr_"))
    {
      return TRUE;
    }
    else if(NULL != STRSTR(szBuf, "memo_aac_"))
    {
      return TRUE;
    }
    else if(NULL != STRSTR(szBuf, "memo_pcm_"))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================
   This function show filled dot image on current selected menu
===========================================================================*/
static void
CRecorderSettingsWin_MarkSelection(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  CMediaPlayer *pMp;
  CtlAddItem pai;
  int16     nMenuId;
  int16     nSubMenuID;
  int16     menuitems;
  MP_Size_16 item;
  int16     i;
//  boolean   bFind = FALSE;

  pMp = pme->m_pOwner;
  nMenuId = IMENUCTL_GetSel(pme->m_pMenu);
  nSubMenuID = IMENUCTL_GetSel(pme->m_pSubMenu);

  if (IDS_AUDIO_FORMAT == nMenuId)
  {
    menuitems = sizeof(RecorderAudioSettings) / sizeof(MP_Size_16);
    /*
     * Update the bullets
     */
    for (i = 0; i < menuitems; ++i)
    {
      item = RecorderAudioSettings[i];
      if (IMENUCTL_GetItem(pme->m_pSubMenu, item.cx, &pai))
      {
        pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
        pai.pImage = NULL;
        if (item.cx == nSubMenuID)
        {
          pai.wImage = (uint16)IDB_RADIO_ON;
          pMp->m_AudioFormat = item.cy;
//          bFind = TRUE;
        }
        else
          pai.wImage = (uint16)IDB_RADIO_OFF;
        (void)IMENUCTL_SetItem(pme->m_pSubMenu, item.cx, MSIF_IMAGE, &pai);
      }
    }
  }
  else if (IDS_AUDIO_SAMPLE_RATE == nMenuId)
  {
//    bFind = FALSE;
    menuitems = sizeof(RecorderSampleRateSettings) / sizeof(MP_Size_16);
    /*
     * Update the bullets
     */
    for (i = 0; i < menuitems; ++i)
    {
      item = RecorderSampleRateSettings[i];
      if (IMENUCTL_GetItem(pme->m_pSubMenu, item.cx, &pai))
      {
        pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
        pai.pImage = NULL;
        if (item.cx == nSubMenuID)
        {
          pai.wImage = (uint16)IDB_RADIO_ON;
          pMp->m_SampleRate = item.cy;
        }
        else
          pai.wImage = (uint16)IDB_RADIO_OFF;
        (void)IMENUCTL_SetItem(pme->m_pSubMenu, item.cx, MSIF_IMAGE, &pai);
      }
    }
  }
  else if (IDS_AUDIO_CHANNEL == nMenuId)
  {
//    bFind = FALSE;
    menuitems = sizeof(RecorderChannelSettings) / sizeof(MP_Size_16);
    /*
     * Update the bullets
     */
    for (i = 0; i < menuitems; ++i)
    {
      item = RecorderChannelSettings[i];
      if (IMENUCTL_GetItem(pme->m_pSubMenu, item.cx, &pai))
      {
        pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
        pai.pImage = NULL;
        if (item.cx == nSubMenuID)
        {
          pai.wImage = (uint16)IDB_RADIO_ON;
          pMp->m_AudioChannel = item.cy;
        }
        else
          pai.wImage = (uint16)IDB_RADIO_OFF;
        (void)IMENUCTL_SetItem(pme->m_pSubMenu, item.cx, MSIF_IMAGE, &pai);
      }
    }
  }
  else if (IDS_RECORD_PATH == nMenuId)
  {
//    bFind = FALSE;
    menuitems = sizeof(RecorderPathSettings) / sizeof(MP_Size_16);
    /*
     * Update the bullets
     */
    for (i = 0; i < menuitems; ++i)
    {
      item = RecorderPathSettings[i];
      if (IMENUCTL_GetItem(pme->m_pSubMenu, item.cx, &pai))
      {
        pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
        pai.pImage = NULL;
        if (item.cx == nSubMenuID)
        {
          pai.wImage = (uint16)IDB_RADIO_ON;
          pMp->m_RecordPath = item.cy;
        }
        else
          pai.wImage = (uint16)IDB_RADIO_OFF;
        (void)IMENUCTL_SetItem(pme->m_pSubMenu, item.cx, MSIF_IMAGE, &pai);
      }
    }
  }
  else if (IDS_SAVE_LOCATION == nMenuId)
  {
//    bFind = FALSE;
    menuitems = sizeof(RecorderSaveLocation) / sizeof(MP_Size_16);
    /*
     * Update the bullets
     */
    for (i = 0; i < menuitems; ++i)
    {
      item = RecorderSaveLocation[i];
      if (IMENUCTL_GetItem(pme->m_pSubMenu, item.cx, &pai))
      {
        pai.pszResImage = pai.pszResText = MEDIAPLAYER_RES_FILE;
        pai.pImage = NULL;
        if (item.cx == nSubMenuID)
        {
          pai.wImage = (uint16)IDB_RADIO_ON;
          if (pMp->m_SaveLocation != item.cy)
          {
            pMp->m_SaveLocation = item.cy;
          }
        }
        else
          pai.wImage = (uint16)IDB_RADIO_OFF;
        (void)IMENUCTL_SetItem(pme->m_pSubMenu, item.cx, MSIF_IMAGE, &pai);
      }
    }
  }

  (void)IMENUCTL_Redraw(pme->m_pSubMenu);
}

static boolean CRecorderSettingsWin_DeleteSelectedFile(IWindow *po)
{
  CRecorderSettingWin *pme = (CRecorderSettingWin *)po;
  CMediaPlayer *pMp;
  CtlAddItem pai;
  int16     nMenuId;
  int16     nSubMenuID;


  if(pme == NULL)
  {
    DBGPRINTF_ERROR("MP: NULL ptr.");
    return FALSE;
  }

  pMp = pme->m_pOwner;
  nMenuId = IMENUCTL_GetSel(pme->m_pMenu);

  if( IDS_DELETE_FILE == nMenuId)
  {
    nSubMenuID = IMENUCTL_GetSel(pme->m_pSubMenu);
    if (IMENUCTL_GetItem(pme->m_pSubMenu, nSubMenuID, &pai))
    {
      IFileMgr *pIFileMgr;

      if(ISHELL_CreateInstance(pme->m_pOwner->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr) == SUCCESS)
      {
        int nMenuCount;
        int idx = -1;
        int nNextItemId = -1;

        nMenuCount = IMENUCTL_GetItemCount(pme->m_pSubMenu);
        if(MP_GetMenuItemIndex(pme->m_pSubMenu, nSubMenuID, &idx))
        {
          if(idx < (nMenuCount - 1))
            nNextItemId = (int)IMENUCTL_GetItemID(pme->m_pSubMenu, ++idx);
          else
            nNextItemId = (int)IMENUCTL_GetItemID(pme->m_pSubMenu, 0);
        }

        if(MP_DeleteFile(pMp, pIFileMgr, MP_MEDIA_DIR, pai.pText) == SUCCESS)
          (void)IMENUCTL_DeleteItem(pme->m_pSubMenu, nSubMenuID);
        else
          DBGPRINTF_ERROR("MP: unable to delete selected file.");

        MP_RELEASEIF(pIFileMgr);

        if(nNextItemId >= 0)
          IMENUCTL_SetSel(pme->m_pSubMenu, (uint16)nNextItemId);

        (void)IMENUCTL_Redraw(pme->m_pSubMenu);
      }
      else
        DBGPRINTF_ERROR("MP: unable to create file manager.");

    }
    else
      DBGPRINTF_HIGH("MP: unable to find selected item.");
  }

  return TRUE;
}

static boolean MP_GetMenuItemIndex(IMenuCtl * pIMenuCtl, uint16 itemID, int* idx)
{
  int i;
  int nMenuCount;

  nMenuCount = IMENUCTL_GetItemCount(pIMenuCtl);
  for(i=0; i < nMenuCount; i++)
  {
    if(itemID == IMENUCTL_GetItemID(pIMenuCtl, i))
    {
      *idx = i;
      return TRUE;
    }
  }
  return FALSE;
}
/*
 * this function returns a file name for memo recorder in following format
 *  memo_ABC_XXX.qcp
 *   ABC is the type name and XXX is the sequence number
 *   e.g. memo_13k_000.qcp, memo_amr_001.qcp
 */
static char *
CRecorderSettingsWin_GetRecordingFileName(CMediaPlayer *pmp)
{
  IFileMgr *pIFileMgr;
  int       suffix1 = 0;
  char     *pszFinalAudioFileName;
  char      strFiletype[16];
  char      strFileExt[8];

  if (!GetRecorderFileTypeString(pmp->m_AudioFormat, strFiletype, sizeof(strFiletype)))
    return NULL;

  (void)ISHELL_CreateInstance(pmp->a.m_pIShell, AEECLSID_FILEMGR,
                              (void **)&pIFileMgr);
  if (!pIFileMgr)
    return NULL;

  if(CRecorderSettingsWin_IsAAC(pmp->m_AudioFormat))
    (void)STRLCPY(strFileExt, "aac", sizeof(strFileExt));
  else if(CRecorderSettingsWin_IsEVRCNB(pmp->m_AudioFormat))
    (void)STRLCPY(strFileExt, "evb", sizeof(strFileExt));
  else
    (void)STRLCPY(strFileExt, "qcp", sizeof(strFileExt));

  /*
   * get the suffix number for the recorder file
   */
  if (pmp->m_SaveLocation == SAVELOC_MMC)
  {
    /*
     * mmc card
     */
    if (!MP_MakeDir(pIFileMgr, MP_MMC_MEDIA_DIR))
    {
      /*
       * mmc may not exist, record on internal flash
       */
      suffix1 =
        CRecorderSettingsWin_GetFileNameSuffix(pmp, "memo", MP_MEDIA_DIR, 4,
                                               strFileExt);
    }
    else
      suffix1 =
        CRecorderSettingsWin_GetFileNameSuffix(pmp, "memo", MP_MMC_MEDIA_DIR, 4,
                                               strFileExt);
  }
  else if (pmp->m_SaveLocation == SAVELOC_UIM)
  {
    /*
     * UIM as storage card
     */
#if MIN_BREW_VERSION(4,0)
    char pszUIMStoragePath[MP_MAX_FILE_NAME];
    (void)MEMSET(pszUIMStoragePath, 0, sizeof (pszUIMStoragePath) );
    if (MP_GetUIMMediaPlayerRoot(pIFileMgr, pszUIMStoragePath, sizeof (pszUIMStoragePath)))
    {
      suffix1 =
          CRecorderSettingsWin_GetFileNameSuffix(pmp, "memo", pszUIMStoragePath, 4,
                                                 strFileExt);
    }
    else
    {
       suffix1 =
          CRecorderSettingsWin_GetFileNameSuffix(pmp, "memo", MP_MEDIA_DIR, 4,
                                                 strFileExt);
    }
#endif //#if MIN_BREW_VERSION(4,0)
  }
  else
  {
    /*
     * internal flash
     * test root directory. Create one if it doesn?t exist
     */
    if (!MP_MakeDir(pIFileMgr, MP_MEDIA_DIR))
    {
      DBGPRINTF_ERROR("MP: failed to create root directory.");
      MP_RELEASEIF(pIFileMgr);
      return NULL;
    }
    suffix1 =
      CRecorderSettingsWin_GetFileNameSuffix(pmp, "memo", MP_MEDIA_DIR, 4,
                                             strFileExt);
  }

  pszFinalAudioFileName = MALLOC(MP_MAX_FILE_NAME);
  if (!pszFinalAudioFileName)
  {
    DBGPRINTF_ERROR("MP: failed to allocate memory.");
    MP_RELEASEIF(pIFileMgr);
    return NULL;
  }

  /*
   * make file name
   */
  (void)SNPRINTF(pszFinalAudioFileName, MP_MAX_FILE_NAME, "%s%s%03d%c%s", "memo", strFiletype, suffix1,
          DOT_CHAR, strFileExt);

  MP_RELEASEIF(pIFileMgr);
  DBGPRINTF_MED("MP: memo recorder filename = %s", pszFinalAudioFileName);
  return pszFinalAudioFileName;
}

/*
 * This function write format string in to given buffer
 * return TRUE is successful, otherwise FASLE
 */
static boolean
GetRecorderFileTypeString(int AudioFormat, char *pBuf, int16 nSize)
{
  if (pBuf == NULL)
    return FALSE;

  if (CRecorderSettingsWin_Is13K(AudioFormat))
  {
    (void)STRLCPY(pBuf, "_13k_", nSize);
    return TRUE;
  }

  if (CRecorderSettingsWin_IsEVRC(AudioFormat))
  {
    (void)STRLCPY(pBuf, "_evrc_", nSize);
    return TRUE;
  }

  if (CRecorderSettingsWin_IsEVRCNB(AudioFormat))
  {
    (void)STRLCPY(pBuf, "_evrcnb_", nSize);
    return TRUE;
  }

  if (CRecorderSettingsWin_IsAMR(AudioFormat))
  {
    (void)STRLCPY(pBuf, "_amr_", nSize);
    return TRUE;
  }

  if (CRecorderSettingsWin_IsPCM(AudioFormat))
  {
    (void)STRLCPY(pBuf, "_pcm_", nSize);
    return TRUE;
  }
  if (CRecorderSettingsWin_IsAAC(AudioFormat))
  {
    (void)STRLCPY(pBuf, "_aac_", nSize);
    return TRUE;
  }

  DBGPRINTF_HIGH("MP: unknown audio format %d!", AudioFormat);
  return FALSE;
}

/*
 * This function returns the first available number of file types with
 * suffix in given file ext type
 * pszFileType: file type (i.e. memo)
 * pszDirName: directory
 * NumChar: number of chars to compare while browsing
 * strFileExt: file ext (i.e. "qcp")
 */
static int CRecorderSettingsWin_GetFileNameSuffix(CMediaPlayer *MP, char *pszFileType,
                                                  char *pszDirName, int NumChar,
                                                  char *strFileExt)
{
  char*          pFileName;
  IFileMgr      *pIFileMgr;
  AEEFileInfoEx  fi;
  char           *pStr;
  char           buf[6];
  int16          nFileNum;
  char           *pBuf;
  int16          i;

  MP->nSuffix = 0;
  if (ISHELL_CreateInstance(MP->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr) != SUCCESS)
  {
    DBGPRINTF_ERROR("MP: failed to create file manager.");
    return 0;
  }

  pBuf = (char *)MALLOC((RECORDER_MAX_FILES + 1) * sizeof(char));
  if (!pBuf)
  {
    MP_RELEASEIF(pIFileMgr);
    DBGPRINTF_ERROR("MP: cannot allocate memory.");
    return 0;
  }

  pFileName = MALLOC(MP_MAX_FILE_NAME);
  if(!pFileName)
  {
    FREE(pBuf);
    MP_RELEASEIF(pIFileMgr);
    DBGPRINTF_ERROR("MP: cannot allocate memory.");
    return 0;
  }

  // initialize fi for enumeration
  (void)MEMSET(pBuf, 0, sizeof(pBuf));
  (void)MEMSET (pFileName, 0, MP_MAX_FILE_NAME);
  (void)MEMSET(&fi, 0, sizeof(fi));
  fi.nStructSize = sizeof(fi);
  fi.pszFile = pFileName;
  fi.nMaxFile = MP_MAX_FILE_NAME;

  if (IFILEMGR_EnumInit(pIFileMgr, pszDirName, FALSE) == SUCCESS)
  {
    while (IFILEMGR_EnumNextEx(pIFileMgr, &fi))
    {
      char     *fiName,
               *extName;

      /*
       * Get the filename.
       */
      fiName = MP_GetFileName(fi.pszFile);
      /*
       * Get the filename extension.
       */
      extName = MP_GetFileNameExtension(fiName);
      /*
       * Compare filename extension with pszSuffixName...
       */
      if (STRCMP(extName, strFileExt) == 0)
      {
        if (STRNCMP(pszFileType, fiName, NumChar) == 0)
        {
          pStr = STRRCHR(fiName, '_');
          if (pStr)
          {
            pStr++;
            (void)MEMCPY(buf, pStr, 3);
            nFileNum = ATOI(buf);
            if (nFileNum < RECORDER_MAX_FILES)
              pBuf[nFileNum] = 1;
          }
        }
      }
    }
    /*
     * find first available num
     */
    for (i = 0; i < RECORDER_MAX_FILES; i++)
    {
      if (!pBuf[i])
      {
        MP->nSuffix = i;
        break;
      }
    }
  }

  FREE(pBuf);
  FREE(pFileName);
  MP_RELEASEIF(pIFileMgr);
  return MP->nSuffix;
}

/*
 * return TRUE if audio format is in 13k group
 */
static boolean
CRecorderSettingsWin_Is13K(int16 audio_format)
{
  switch (audio_format)
  {
  case MM_QCP_FORMAT_FIXED_FULL_13K:
  case MM_QCP_FORMAT_VAR_FULL_13K:
  case MM_QCP_FORMAT_VAR_HALF_13K:
  case MM_QCP_FORMAT_FIXED_HALF_13K:
    return TRUE;
  default:
    return FALSE;
  }
}

/*
 * return TRUE if audio format is in evrc group
 */
static boolean
CRecorderSettingsWin_IsEVRC(int16 audio_format)
{
  switch (audio_format)
  {
  case MM_QCP_FORMAT_FIXED_FULL_EVRC:
  case MM_QCP_FORMAT_VAR_FULL_EVRC:
    return TRUE;
  default:
    return FALSE;
  }
}

/*
 * return TRUE if audio format is in evrc-b group
 */
static boolean
CRecorderSettingsWin_IsEVRCNB(int16 audio_format)
{
  switch (audio_format)
  {
#ifdef FEATURE_AUDFMT_EVB
  case MP_EVRCNB_RECORDING:
    return TRUE;
#endif
  default:
    return FALSE;
  }
}


/*
 * return TRUE if audio format is in amr group
 */
static boolean
CRecorderSettingsWin_IsAMR(int16 audio_format)
{
  switch (audio_format)
  {
  case MM_AMR_FORMAT_FIXED_4_75_AMR:
  case MM_AMR_FORMAT_FIXED_5_15_AMR:
  case MM_AMR_FORMAT_FIXED_5_9_AMR:
  case MM_AMR_FORMAT_FIXED_6_7_AMR:
  case MM_AMR_FORMAT_FIXED_7_4_AMR:
  case MM_AMR_FORMAT_FIXED_7_95_AMR:
  case MM_AMR_FORMAT_FIXED_10_2_AMR:
  case MM_AMR_FORMAT_FIXED_12_2_AMR:
    return TRUE;
  default:
    return FALSE;
  }
}

/*
 * return TRUE if audio format is in pcm group
 */
static boolean
CRecorderSettingsWin_IsPCM(int16 audio_format)
{
  switch (audio_format)
  {
  case MP_8K_PCM_RECORDING:
    return TRUE;
  default:
    return FALSE;
  }
}

static boolean CRecorderSettingsWin_IsAAC(int16 audio_format)
{
  switch (audio_format)
  {
  case MP_AAC_RECORDING:
    return TRUE;
  default:
    return FALSE;
  }
}

#endif     // FEATURE_MP_RECORDER_SETTINGS

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif     // FEATURE_APP_TEST_AUTOMATION


#ifdef MP_FEATURE_ICONTENT
static void MP_AddFileToIContentDB(CMediaPlayer *pme,
                                           const char* pszCatName, const char* pzeCatDesc,
                                           const char* pszFile, const char* pszFileDesc,
                                           const char* nMediaType)
{
  IFileMgr       *pIFileMgr = NULL;
  char str[MP_MAX_FILE_NAME + 64];
  int length;

  if(SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr))
  {
    DBGPRINTF_ERROR("MP: filed to create file mgr.");
    return;
  }

  if(SUCCESS == IFILEMGR_ResolvePath(pIFileMgr, pszFile, str, &length))
    MPContentAdd(pme->m_pContent, pszCatName, pzeCatDesc, str, pszFileDesc, nMediaType);
  else
    DBGPRINTF_ERROR("MP: cannot resolve path.");

  MP_RELEASEIF(pIFileMgr);
}
/*
static void MP_DeleteContent(CMediaPlayer *pme, const char* pszFile)
{
  IFileMgr       *pIFileMgr = NULL;
  char str[MP_MAX_FILE_NAME + 64];
  char szURI[MP_MAX_FILE_NAME];
  int length;

  if(SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr))
  {
    DBGPRINTF_ERROR("MP: filed to create file mgr.");
    return;
  }

  if(SUCCESS == IFILEMGR_ResolvePath(pIFileMgr, pszFile, str, &length))
  {
    if(SUCCESS == MPContentGetURIfromLocalURL(pme->m_pContent, str, szURI, MP_MAX_FILE_NAME))
    {
      (void)MPContentDelete(pme->m_pContent, szURI);
    }
    else
      DBGPRINTF_ERROR("MP: unable to get URI for %s.", str);

  }
  else
    DBGPRINTF_ERROR("MP: cannot resolve path.");

  MP_RELEASEIF(pIFileMgr);
}
*/
#endif //MP_FEATURE_ICONTENT

#ifdef MP_FEATURE_QTV_DIVX_DRM  
boolean MediaPlayer_GetDivXRegCode(CMediaPlayer *pme, IMedia *pMedia, char *buf, int len)
{
  int32 nRegCodeLength = 0;
  boolean bRet = FALSE;  
  boolean relMedia = FALSE;  

  if(buf == NULL)
    return bRet;
  buf[0] = '\0';

  if((pMedia == NULL) ||
     (SUCCESS != IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_DRM_REG_CODE, NULL, &nRegCodeLength)))
  {
    if(SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_MEDIAMPEG4, (void **)&pMedia))
    {
      DBGPRINTF_ERROR("MP: Failed to create DivX Media for registration code.");
      return bRet;
    }
    DBGPRINTF_MED("MP: Created DivX Media for registration code.");
    if(SUCCESS != IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_DRM_REG_CODE, NULL, &nRegCodeLength))
    {
      MP_RELEASEIF(pMedia);
      DBGPRINTF_ERROR("MP: Failed to get DivX Registration Code Length.");
      return bRet;
    }
    relMedia = TRUE;  
  }
  else
  {
    relMedia = FALSE;  
  }

  if((nRegCodeLength <= len) && (nRegCodeLength != 0))
  {
    if(SUCCESS == IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_DRM_REG_CODE, (int32 *)buf, &nRegCodeLength))
    {
      DBGPRINTF_MED("MP: DivX device registration code (%d) %s.", nRegCodeLength, buf);
      bRet = TRUE;
    } 
    else
    {
      DBGPRINTF_ERROR("MP: failed to retrieve DivX registration code.");  
    }
  }
  else
  {
    DBGPRINTF_ERROR("MP: reg code length invalid %d.", nRegCodeLength);  
  }

  if(relMedia == TRUE)
  {
    DBGPRINTF_MED("MP: Releasing created DivX Media for registration code.");
    MP_RELEASEIF(pMedia);
  }
  return bRet;
}

/* Function to cat the text to the end of the buf from pDivXSpec */
/* The format catted is (CGMSA, ACPTB, DP) */
boolean CMediaPlayer_DivXDRMSpecCat(AEEMediaMPEG4DivXDRMSpec* pDivXSpec, char *buf, int buf_size)
{
   if((pDivXSpec == NULL) || (buf == NULL))
      return FALSE;

   // Cat the CGMSA text onto buffer */
   switch(pDivXSpec->nCgmsaSignal)
   {
      case 0:
         STRLCAT(buf, "(CopyFreely, ", buf_size);
         break;

      case 1:
         STRLCAT(buf, "(CopyNoMore, ", buf_size);
         break;

      case 2:
         STRLCAT(buf, "(CopyOnce, ", buf_size);
         break;

      case 3:
         STRLCAT(buf, "(CopyNever, ", buf_size);
         break;

      default:
         STRLCAT(buf, "(UNKNOWN, ", buf_size);
         break;
   }

   // Cat the ACPTB text onto buffer */
   switch(pDivXSpec->nAcptbSignal)
   {
      case 0:
         STRLCAT(buf, "Off, ", buf_size);
         break;

      case 1:
         STRLCAT(buf, "Automatic Gain Control, ", buf_size);
         break;

      case 2:
         STRLCAT(buf, "Two Line Color Burst, ", buf_size);
         break;

      case 3:
         STRLCAT(buf, "Four Line Color Burst, ", buf_size);
         break;

      default:
         STRLCAT(buf, "UNKNOWN, ", buf_size);
         break;
   }

   // Cat the Digital Protection text onto buffer */
   switch(pDivXSpec->nDigitalProtectionSignal)
   {
      case 0:
         STRLCAT(buf, "Off)", buf_size);
         break;

      case 1:
         STRLCAT(buf, "On)", buf_size);
         break;

      default:
         STRLCAT(buf, "UNKNOWN)", buf_size);
         break;
   }

   return TRUE;
}
#endif //#ifdef MP_FEATURE_QTV_DIVX_DRM

void MPDlgButtonHandler_OK(CMediaPlayer* pme)
{
  if(pme->m_pDlg)
  {
    MPDlgDelete(pme->m_pDlg);
    pme->m_pDlg = NULL;
  }
}

static boolean
CPlayerWin_MappedKeyEventHandler(IWindow *po, AEEEvent eCode, uint16 wParam,
                                   uint32 dwParam)
{
  CtlAddItem pai;
  CPlayerWin *pme = (CPlayerWin *)po;
  uint16 keyOffset = 0;

  /* 0-9, *, # */
  keyOffset = wParam - (uint16)AVK_0;
  /* Validate our offset */
  if(keyOffset >= MPPW_KEYMAPPING_MAX)
    return FALSE;

  DBGPRINTF_MED("MP: MappedKeyEventHandler wParam %hu Mapping %d", wParam, MPPW_KeyMappings[keyOffset]);
  if (CPlayerWin_IsMedia(pme))
  {
    switch (MPPW_KeyMappings[keyOffset])
    {
    case MPPW_KEYMAP_NONE:
      CPlayerWin_ShowProgCtlText(pme, "None.");
      return TRUE;

    case MPPW_KEYMAP_FULLSCREEN:
      if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_FULLSCRN, &pai))
        return TRUE;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_COMMAND, IDM_PM_FULLSCRN, 0);
      IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_FULLSCRN);
      CPlayerWin_ShowProgCtlText(pme, "Toggle Full.");
      return TRUE;

    case MPPW_KEYMAP_BK_FILE:
      if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_BK_FILE, &pai))
        return TRUE;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_BK_FILE, 0);
      if(!pme->m_bFullScreen)
      {
        if (!pme->m_bFullScreen)
          IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_BK_FILE);
        CPlayerWin_ShowProgCtlText(pme, "BK_F.");
      }
      return TRUE;

    case MPPW_KEYMAP_FW_FILE:
      if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_FF_FILE, &pai))
        return TRUE;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_FF_FILE, 0);
      if(!pme->m_bFullScreen)
      {
        if (!pme->m_bFullScreen)
          IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_FF_FILE);
        CPlayerWin_ShowProgCtlText(pme, "FF_F.");
      }
      return TRUE;

    case MPPW_KEYMAP_PL_RINGER:
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
      if ( pme->m_pOwner->m_bPlayRingerWithQTV)
      {
        int ret;
        DBGPRINTF("MP: play ringer while QTV play is on");
        if(!pme->m_pMedia1)
        {
          AEEMediaData md;
          AEECLSID cls;
          md.clsData = MMD_FILE_NAME;
          md.pData = (void *)"media/ringer/ringer.3gp";
          md.dwSize = 0;
          ret= AEEMediaUtil_FindClass(pme->m_pIShell, &md, &cls);
          if (ret != SUCCESS)
          {
            return ret;
          }

          if (SUCCESS != (ret = ISHELL_CreateInstance(pme->m_pIShell, cls, (void **)&pme->m_pMedia1) || !pme->m_pMedia1))
          {
            DBGPRINTF_HIGH("MP: ISHELL_CreateInstance failed,ret = %d", ret);
            return TRUE;
          }

          if(pme->m_pMedia1)
          {
            /* Set priority for the concurrent audio instance */
            IMEDIA_SetMediaParm(pme->m_pMedia1,
                                MM_MP4_PARM_PRIORITY,
                                MM_QTV_PRIORITY_2,
                                0);

            ret = IMEDIA_SetMediaData(pme->m_pMedia1, &md);
            if (ret != SUCCESS)
            {
              IMEDIA_Release(pme->m_pMedia1);
              DBGPRINTF_HIGH("MP: IMEDIA_SetMediaData failed,ret = %d", ret);
              return TRUE;
            }

            ret = IMEDIA_Play(pme->m_pMedia1);
            DBGPRINTF_HIGH("MP: IMEDIA_Play with ringer media returns %d", ret);
          }
        }
        else
        {
          /* If the instance alreay exists, issue play */
          ret = IMEDIA_Play(pme->m_pMedia1);
          DBGPRINTF_HIGH("MP: IMEDIA_Play with ringer media returns %d", ret);
        }
        return TRUE;
      }
#endif //#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
      return TRUE;

    case MPPW_KEYMAP_ST_RINGER:
#if defined (FEATURE_APP_MPEG4) && defined (FEATURE_QTV_AUDIO_RINGER)
      if ( pme->m_pOwner->m_bPlayRingerWithQTV)
      {
        if(pme->m_pMedia1)
        {
          int ret;
          /* Stop first followed by release. */
          ret = IMEDIA_Stop(pme->m_pMedia1);
          DBGPRINTF_HIGH("MP: IMEDIA_Stop with ringer media returns %d", ret);
          MP_RELEASEIF(pme->m_pMedia1);
        }
      }
#endif
      return TRUE;

    case MPPW_KEYMAP_REWIND:
      if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_REWIND, &pai))
        return TRUE;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_COMMAND, IDM_PM_REWIND, 0);
      if (!pme->m_bFullScreen)
        IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_REWIND);
      CPlayerWin_ShowProgCtlText(pme, "RW.");
      return TRUE;

    case MPPW_KEYMAP_FASTFWD:
      if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_FF, &pai))
        return TRUE;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_COMMAND, IDM_PM_FF, 0);
      if (!pme->m_bFullScreen)
        IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_FF);
      CPlayerWin_ShowProgCtlText(pme, "FF.");
      return TRUE;

    case MPPW_KEYMAP_PLAYSTOP:
      if ((pme->m_eWin == MPPW_PLAY && CPlayerWin_IsPlaying(pme)) ||
          (pme->m_eWin == MPPW_RECORD && CPlayerWin_IsRecording(pme)))
      {
        (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_STOP, 0);
      }
      else
      {
        DBGPRINTF_MED("MP: Posting IDM_PM_PLAY event");
        (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
      }

      CPlayerWin_ShowProgCtlText(pme, "Play/Stop.");
      return TRUE;

    case MPPW_KEYMAP_PAUSE:
      if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_PAUSE, &pai))
        return TRUE;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_COMMAND, IDM_PM_PAUSE, 0);
      if (!pme->m_bFullScreen)
        IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_PAUSE);
      CPlayerWin_ShowProgCtlText(pme, "Pause.");
      return TRUE;

    case MPPW_KEYMAP_FF_STEP:
      if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_FF_STEP, &pai))
        return TRUE;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_COMMAND, IDM_PM_FF_STEP, 0);
      if (!pme->m_bFullScreen)
        IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_FF_STEP);
      CPlayerWin_ShowProgCtlText(pme, "FF Step.");
      return TRUE;

    case MPPW_KEYMAP_ZOOM_IN:
      if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_ZOOM_IN, &pai))
        return TRUE;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_COMMAND, IDM_PM_ZOOM_IN, 0);
      if (!pme->m_bFullScreen)
        IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_ZOOM_IN);
      CPlayerWin_ShowProgCtlText(pme, "Zoom In.");
      return TRUE;

    case MPPW_KEYMAP_ZOOM_OUT:
      if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_ZOOM_OUT, &pai))
        return TRUE;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_COMMAND, IDM_PM_ZOOM_OUT, 0);
      if (!pme->m_bFullScreen)
        IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_ZOOM_OUT);
      CPlayerWin_ShowProgCtlText(pme, "Zoom Out.");
      return TRUE;

    case MPPW_KEYMAP_ROTATE:
      if (!IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_CW_ROTATE, &pai))
        return TRUE;

      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_COMMAND, IDM_PM_CW_ROTATE, 0);
      if (!pme->m_bFullScreen)
        IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_CW_ROTATE);
      CPlayerWin_ShowProgCtlText(pme, "Rotate.");
      return TRUE;

    case MPPW_KEYMAP_RECORD:
      if(CPlayerWin_PurgeDownloadedFile(pme))
        return TRUE;
      if (pme->m_eWin == MPPW_RECORD && IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_RECORD, &pai))
      {
        // in recording window now
        if (!CPlayerWin_IsRecording(pme))
        {
          (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                         EVT_COMMAND, IDM_PM_RECORD, 0);
          if (!pme->m_bFullScreen)
            IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_RECORD);
          CPlayerWin_ShowProgCtlText(pme, "Record.");
        }
        else
        {
          (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                         EVT_COMMAND, IDM_PM_STOP, 0);
          if (!pme->m_bFullScreen)
          {
            IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_STOP);
          }
          CPlayerWin_ShowProgCtlText(pme, "Stop.");
        }
        return TRUE;
      }
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
      else if(pme->m_eWin == MPPW_PLAY || pme->m_eWin == MPPW_STREAM_RECORD)
      {
        // in either player win or stream recorder win
        DBGPRINTF_MED("MP: Start Recording");
        CPlayerWin_StartStreamRecord(pme, TRUE);
        return TRUE;
      }
#endif // defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
      return TRUE;

    case MPPW_KEYMAP_RECSET_STRST:
      if (pme->m_eWin == MPPW_RECORD && IMENUCTL_GetItem(pme->m_pPlayerMenu, IDM_PM_RECORDER_SETTINGS, &pai))
      {
        (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,


                       EVT_COMMAND, IDM_PM_RECORDER_SETTINGS, 0);
        CPlayerWin_ShowProgCtlText(pme, "Recorder Setting.");
        return TRUE;
      }
#if defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
      else if(pme->m_eWin == MPPW_PLAY || pme->m_eWin == MPPW_STREAM_RECORD)
      {
        // in either player win or stream recorder win
        DBGPRINTF_MED("MP: Stop Recording");
        CPlayerWin_StopStreamRecord(pme);
        return TRUE;
      }
#endif // defined(MP_FEATURE_VIDEO) && defined(FEATURE_QTV_STREAM_RECORD)
      return TRUE;

    case MPPW_KEYMAP_REPLAY_AT_STOP:
#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)
      // toggle replay at stop mode
      pme->m_pOwner->m_bReplayAtStop = !pme->m_pOwner->m_bReplayAtStop; 
      DBGPRINTF_HIGH("MP: Replay at Stop Mode, %s.", (pme->m_pOwner->m_bReplayAtStop)?"On":"Off");

      if(pme->m_pOwner->m_bReplayAtStop && pme->m_pMedia)
      {
        int ret;
        ret = IMEDIA_SetMediaParm(pme->m_pMedia, MM_MP4_PARM_START_FROM_POS, (int32)pme->m_dwPositionAtStop, 0);
        if(SUCCESS == ret)
        {
          DBGPRINTF_HIGH("MP: replay will be started at %d.", pme->m_dwPositionAtStop);
        }
        else
        {
          DBGPRINTF_ERROR("MP: failed to set MM_MP4_PARM_START_FROM_POS with %d.", pme->m_dwPositionAtStop);
        }
        CPlayerWin_ShowProgCtlText(pme, "MP: Replay at Stop: On.");
      }
      else
      {
        CPlayerWin_ShowProgCtlText(pme, "MP: Replay at Stop: Off.");
      }
#endif //#if defined(FEATURE_QTV_GENERIC_BCAST_ISDB) && defined(MM_MP4_PARM_START_FROM_POS)
      return TRUE;

    case MPPW_KEYMAP_SAVEFILE:
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      {
        if (pme->m_pMedia)
        {
#if defined FEATURE_APP_MEDIA_VIDEO
#error code not present
#elif defined FEATURE_APP_MPEG4
          if(CPlayerWin_PurgeDownloadedFile(pme))
            return TRUE;
          DBGPRINTF_MED("MP: KEYMAP SAVEFILE streamtype %d", pme->m_mediaInfo.streamtype);
          if (pme->m_mediaInfo.streamtype != MM_MEDIA_NON_STREAMING ) 
          {
            AEECLSID ClsId;
            char*    pszFileName;

            if (IMEDIA_GetClassID(pme->m_pMedia, &ClsId) == SUCCESS &&
                ClsId == AEECLSID_MEDIAMPEG4)
            {
              if(pme->m_pOwner->m_bProgDLFileSaveLoc)
              {
                pszFileName = MP_PATH_SAVELOC_FLASH;
              }
              else
              {
                pszFileName = MP_PATH_SAVELOC_MMC;
              }
              (void)IMEDIA_SetMediaParm(pme->m_pMedia,
                           MM_MP4_PARAM_COPY_BUFFER_TO_FILE,
                           (int32)pszFileName, 1);
              DBGPRINTF_MED("MP: save to file %s", pszFileName);
            }
          }
#endif //FEATURE_APP_MEDIA_VIDEO

#ifdef MP_FEATURE_ICONTENT
          if(pme->m_pOwner->m_bIContentMode)
          {
            // save recorded file into IContent DB
            MP_AddFileToIContentDB(pme->m_pOwner,
                          MP_ICONTENT_CAT_NAMR_RECORD, MP_ICONTENT_CAT_DESC_RECORD,
                          "media/pds2.mp4", MP_ICONTENT_CAT_DESC_RECORD,
                          MT_VIDEO_MPEG4);
          }
#endif //MP_FEATURE_ICONTENT
        }
        // do not return from here. otherwise it will block rotation feature
      }
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
      return TRUE;

    case MPPW_KEYMAP_FCS_SWITCH:
      (void)ISHELL_PostEvent(pme->m_pOwner->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                       EVT_COMMAND, IDM_PM_FCS_SWITCH, 0);
      CPlayerWin_ShowProgCtlText(pme, "FCS Switch.");
      return TRUE;

    default:
      return FALSE;
    }
  }
  return FALSE;
}

CKeyMapping MPKeyMapping_DlgToMapping(uint16 idm)
{
  switch(idm)
  {
    case IDM_KEYMAP_NONE:
      return MPPW_KEYMAP_NONE;
    case IDM_KEYMAP_FULLSCREEN:
      return MPPW_KEYMAP_FULLSCREEN;
    case IDM_KEYMAP_BK_FILE:
      return MPPW_KEYMAP_BK_FILE;
    case IDM_KEYMAP_FW_FILE:
      return MPPW_KEYMAP_FW_FILE;
    case IDM_KEYMAP_PL_RINGER:
      return MPPW_KEYMAP_PL_RINGER;
    case IDM_KEYMAP_ST_RINGER:
      return MPPW_KEYMAP_ST_RINGER;
    case IDM_KEYMAP_REWIND:
      return MPPW_KEYMAP_REWIND;
    case IDM_KEYMAP_FASTFWD:
      return MPPW_KEYMAP_FASTFWD;
    case IDM_KEYMAP_PLAYSTOP:
      return MPPW_KEYMAP_PLAYSTOP;
    case IDM_KEYMAP_PAUSE:
      return MPPW_KEYMAP_PAUSE;
    case IDM_KEYMAP_FF_STEP:
      return MPPW_KEYMAP_FF_STEP;
    case IDM_KEYMAP_ZOOM_IN:
      return MPPW_KEYMAP_ZOOM_IN;
    case IDM_KEYMAP_ZOOM_OUT:
      return MPPW_KEYMAP_ZOOM_OUT;
    case IDM_KEYMAP_ROTATE:
      return MPPW_KEYMAP_ROTATE;
    case IDM_KEYMAP_RECORD:
      return MPPW_KEYMAP_RECORD;
    case IDM_KEYMAP_RECSET_STRST:
      return MPPW_KEYMAP_RECSET_STRST;
    case IDM_KEYMAP_REPLAY_AT_STOP:
      return MPPW_KEYMAP_REPLAY_AT_STOP;
    case IDM_KEYMAP_SAVEFILE:
      return MPPW_KEYMAP_SAVEFILE;
    case IDM_KEYMAP_FCS_SWITCH:
      return MPPW_KEYMAP_FCS_SWITCH;
    default:
      return MPPW_KEYMAP_NONE;
  }
}

uint16 MPKeyMapping_MappingToDlg(CKeyMapping key)
{
  switch(key)
  {
    case MPPW_KEYMAP_NONE:
      return IDM_KEYMAP_NONE;
    case MPPW_KEYMAP_FULLSCREEN:
      return IDM_KEYMAP_FULLSCREEN;
    case MPPW_KEYMAP_BK_FILE:
      return IDM_KEYMAP_BK_FILE;
    case MPPW_KEYMAP_FW_FILE:
      return IDM_KEYMAP_FW_FILE;
    case MPPW_KEYMAP_PL_RINGER:
      return IDM_KEYMAP_PL_RINGER;
    case MPPW_KEYMAP_ST_RINGER:
      return IDM_KEYMAP_ST_RINGER;
    case MPPW_KEYMAP_REWIND:
      return IDM_KEYMAP_REWIND;
    case MPPW_KEYMAP_FASTFWD:
      return IDM_KEYMAP_FASTFWD;
    case MPPW_KEYMAP_PLAYSTOP:
      return IDM_KEYMAP_PLAYSTOP;
    case MPPW_KEYMAP_PAUSE:
      return IDM_KEYMAP_PAUSE;
    case MPPW_KEYMAP_FF_STEP:
      return IDM_KEYMAP_FF_STEP;
    case MPPW_KEYMAP_ZOOM_IN:
      return IDM_KEYMAP_ZOOM_IN;
    case MPPW_KEYMAP_ZOOM_OUT:
      return IDM_KEYMAP_ZOOM_OUT;
    case MPPW_KEYMAP_ROTATE:
      return IDM_KEYMAP_ROTATE;
    case MPPW_KEYMAP_RECORD:
      return IDM_KEYMAP_RECORD;
    case MPPW_KEYMAP_RECSET_STRST:
      return IDM_KEYMAP_RECSET_STRST;
    case MPPW_KEYMAP_REPLAY_AT_STOP:
      return IDM_KEYMAP_REPLAY_AT_STOP;
    case MPPW_KEYMAP_SAVEFILE:
      return IDM_KEYMAP_SAVEFILE;
    case MPPW_KEYMAP_FCS_SWITCH:
      return IDM_KEYMAP_FCS_SWITCH;
    default:
      return IDM_KEYMAP_NONE;
  }
}

static boolean CPlayerWin_PurgeDownloadedFile(CPlayerWin *pme)
{
#if defined FEATURE_APP_MPEG4
  AEEMediaMPEG4Spec *pSpec;
  int32     dwSize;
  int       nRet;

  if(pme == NULL)
    return FALSE;

  DBGPRINTF_MED("MP: PurgeDownload Check %d", pme->m_mediaInfo.eFormatType); 

  if((pme->m_pMedia != NULL) && (pme->m_mediaInfo.eFormatType == MM_MPEG4_UNKNOWN_FORMAT_TYPE))
  {
    nRet = IMEDIA_GetMediaSpec(pme->m_pMedia, &pSpec, &dwSize);
    DBGPRINTF_ERROR("MP: MediaSpec nRet %d dwSize %d (%d).", nRet, dwSize, sizeof(AEEMediaMPEG4Spec));
    if (SUCCESS == nRet)
    {
      if (dwSize == sizeof(AEEMediaMPEG4Spec))
      {
        (void)MEMMOVE(&pme->m_mediaInfo, pSpec, sizeof(AEEMediaMPEG4Spec));
        pme->m_dwDuration = pme->m_mediaInfo.dwDuration;
        pme->m_bGotMediaInfo = TRUE;
      }
#if defined(FEATURE_APP_REAL)
      else if (dwSize == sizeof(AEEMediaRealSpec))
      {
        (void)MEMMOVE(&pme->m_mediaRealInfo, pSpec, sizeof(AEEMediaRealSpec));
        pme->m_dwDuration = pme->m_mediaRealInfo.dwDuration;
        pme->m_bGotMediaInfo = TRUE;
      }
#endif  // FEATURE_APP_REAL
    }
  }

#if defined (MM_PD_FORMAT_TYPE)
  DBGPRINTF_MED("MP: PurgeDownload Check PD"); 
  if(pme->m_mediaInfo.eFormatType == MM_PD_FORMAT_TYPE)
  {
    // If save not allowed just log and return
    DBGPRINTF_MED("MP: PurgeDownload set for PD file, %s", 
      (pme->m_mediaInfo.ExtendedInfo.PDSpec.bPurgeDowloadedFile)?"TRUE":"FALSE");
    if(pme->m_mediaInfo.ExtendedInfo.PDSpec.bPurgeDowloadedFile == TRUE)
      return TRUE;
  }
#endif
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)
  DBGPRINTF_MED("MP: PurgeDownload Check PVX"); 
  if(pme->m_mediaInfo.eFormatType == MM_PVX_FORMAT_TYPE)
  {
    // If save not allowed just log and return
    DBGPRINTF_MED("MP: PurgeDownload set for PVX file, %s", 
      (pme->m_mediaInfo.ExtendedInfo.PvxSpec.bPurgeDowloadedFile)?"TRUE":"FALSE");
    if(pme->m_mediaInfo.ExtendedInfo.PvxSpec.bPurgeDowloadedFile == TRUE)
      return TRUE;
  }
#endif
#endif // FEATURE_APP_MPEG4
  return FALSE;
}


AEECLSID CMediaPlayer_GetAEECLSID(CMediaPlayer *pme, char *base, char *ext)
{
  char szMIME[MP_MIME_SIZE];
  AEECLSID cls;

  (void)STRLCPY(szMIME, base, sizeof(szMIME));
  (void)STRLCAT(szMIME, ext, sizeof(szMIME));
  cls = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_MEDIA, szMIME);
  DBGPRINTF_MED("MP: GetAEECLSID %s%s %d\n", base, ext, cls);
  if (cls)
  {
     return(cls);
  }
  return(0);
} 

/*---------------------------------------------------------------------
FUNCTION: CMediaPlayer_HandlePhoneEvent

DESCRIPTION: This function handle events of "phone state" changes, and may suspend or resume MediaPlayer
             depending on the internal state.

PARAMETERS:
    pme           pointer to the Applet member structure (CMediaPlayer*)
    pNotifyData   pointer to void nofication data, which is actually a pointer to AEETNotifyInfo

RETURN:
    TRUE, if event is of interest
    FALSE, otherwise

    The function may attempt to suspend or resume CMediaPlayer

SIDE EFFECTS:
    The function, when appropriate, sets the attribute of m_bCallInProgress in CMediaPlayer

NOTES:  The phone state change appears to be in the sequence of 
  AEET_EVENT_CALL_INCOM
  AEET_EVENT_CALL_ANSWER
  AEET_EVENT_CALL_CONNECT
  AEET_EVENT_CALL_END

MediaPlayer may get external suspend event when the phone transits to call connect.
-----------------------------------------------------------------------*/

static boolean CMediaPlayer_HandlePhoneEvent(CMediaPlayer *pme, void * pNotifyData)
{
  int retVal = FALSE;
  AEETNotifyInfo * pNotifyInfo = NULL;

  DBGPRINTF_HIGH("MP: Processing PHONENOTIFIER");
  pNotifyInfo = (AEETNotifyInfo * )pNotifyData;
  
#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
  {
    CPlayerWin *pWin;
    char * pExtName = NULL;

    // When incall mixing is disabled, we just bypass the whole call event processing.
    if (!uisnd_get_incall_mixing() )
    {
      DBGPRINTF_MED("MP: Incall mixing is disabled.  No speical processing for phone events.");
      return TRUE;
    }
  
    pWin = NULL;
    if((pme->m_eActiveWin == MPW_PLAYER) && (pme->m_pWin != NULL)) 
      pWin = (CPlayerWin *)pme->m_pWin;
    
    if ( pme->m_bCallInProgress || pme->m_bSuspend  )
    {
      pExtName = MP_GetFileNameExtension(pme->m_resumeFileName);
    }
    else if (pWin && pWin->m_pszFile)
    {
      pExtName = MP_GetFileNameExtension( pWin->m_pszFile);
    }
  
    // The class Id or media file extension is of no interest. 
    if ( !CMediaPlayer_IsAudioMedia(pme, pExtName))
    {
      DBGPRINTF_MED("MP: Media file is of no interest.  No speical processing for phone events.");
      return TRUE;
    }
  }
#else
  {
    return TRUE;
  }
#endif

  if(pNotifyInfo == NULL)
  {
    DBGPRINTF_ERROR("MP: Encountered NULL AEETNotifyInfo pointer while processing PHONENOTIFIER");
    return retVal;
  }

  // Process the call events
  switch (pNotifyInfo->event)
  {
  case AEET_EVENT_CALL_INCOM:
    {
      DBGPRINTF_MED("MP: Processing AEET_EVENT_CALL_INCOM");
      (void)CMediaPlayer_ProcessSuspendEvent(pme);
      pme->m_bCallInProgress = TRUE;      
      retVal = TRUE;
      break;
    }
  case AEET_EVENT_CALL_OPS:
    {
      // Looks like this is the call event when the soft reject button is pressed.
      // We need to reset the call in progress flag.
      DBGPRINTF_MED("MP: Received AEET_EVENT_CALL_OPS");
      pme->m_bCallInProgress = FALSE;      
      retVal = TRUE;
      break;
    }
    break;
  case AEET_EVENT_CALL_ANSWER:
    {
      DBGPRINTF_MED("MP: Processing AEET_EVENT_CALL_ANSWER");
      pme->m_bCallInProgress = TRUE;
      (void)CMediaPlayer_ProcessResumeEvent(pme);
      retVal = TRUE;
      break;
    }
  case AEET_EVENT_CALL_CONNECT:
    {
      //DBGPRINTF_MED("MP: Processing AEET_EVENT_CALL_CONNECT");
      break;
    }
  case AEET_EVENT_CALL_END:
    {
      DBGPRINTF_MED("MP: Processing AEET_EVENT_CALL_END");
      pme->m_bCallInProgress = FALSE;
      retVal = TRUE;
      break;
    }
  default:
    //DBGPRINTF_MED("MP: Received call event %d.  No handling required", pNotifyInfo->event);
    break;
  }

  return retVal;
}

boolean CMediaPlayer_FindNextURL(CMediaPlayer *pme, char *url, int url_size)
{
  char *extName;
  char *fileName;
  int  nRet = FALSE;
  int  nNextIndex = 0;
  int  nCount;

  nNextIndex = CMediaPlayer_GetNextFileIndex(pme, pme->m_CurFileIndex);
  extName = MP_GetFileNameExtension(pme->m_FileExplorer.m_FileArray[nNextIndex].m_szName);
  fileName = pme->m_FileExplorer.m_FileArray[nNextIndex].m_szName;
  nCount = 0;

  // find the next URL file (note returns 0 if match)
  while(STRICMP(extName, "url"))
  {
    nNextIndex = CMediaPlayer_GetNextFileIndex(pme, nNextIndex);
    extName = MP_GetFileNameExtension(pme->m_FileExplorer.m_FileArray[nNextIndex].m_szName);
    fileName = pme->m_FileExplorer.m_FileArray[nNextIndex].m_szName;
    if(nCount > (pme->m_FileExplorer.m_NumFiles + 1))
    {
      // we have search all files, can?t find any valid content file, break out now
      DBGPRINTF_ERROR("MP: cannot find any URL to play.");
      return nRet;
    }

    DBGPRINTF_MED("MP: find next URL file, nNextIndex = %d nCount = %d.", nNextIndex, nCount);
    nCount++;
  }
  pme->m_CurFileIndex = nNextIndex;

  // Found URL file - extract URL and return
  return(MP_ExtractURLFromFile(pme, fileName, url, url_size));
}

static boolean MP_ExtractURLFromFile(CMediaPlayer *pme, const char *pszFile, char *pUrl, int szUrl)
{
  IFileMgr       *pIFileMgr;
  IFile          *pIFile = NULL;
  AEEFileInfoEx  fi;
  unsigned int   size = 0;
  boolean        nRet = FALSE;
 
  (void)MEMSET(pUrl, 0, szUrl);
  (void)ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);

  if (NULL == pIFileMgr)
    return nRet;

  if ((pIFile = IFILEMGR_OpenFile(pIFileMgr, pszFile, _OFM_READ)) == NULL)
  {
    MP_RELEASEIF(pIFileMgr);
    return nRet;
  }

  (void)MEMSET(&fi,0,sizeof(fi));
  fi.nStructSize = sizeof(fi);
  if(IFILEMGR_GetInfoEx(pIFileMgr, pszFile, &fi) == SUCCESS)
  {
    int       rSize = 0;

    size = fi.dwSize;
    if (size <= szUrl)
    {
      rSize = IFILE_Read(pIFile, pUrl, size);
      pUrl[rSize] = '\0';
      DBGPRINTF_MED("MP: MP_ExtractURLFromFile URL=%s\n", pUrl);
      nRet = TRUE;
    }
  } 
  (void)IFILE_Release(pIFile);
  pIFile = NULL;
  MP_RELEASEIF(pIFileMgr);
  return nRet;
}

static boolean CMediaPlayer_IsAudioClassID(AEECLSID classId)
{
  // Note: This does not test for .wma files
  if (classId == AEECLSID_MEDIAMP3 || classId == AEECLSID_MEDIAAAC ||
      classId == AEECLSID_MEDIAQCP || classId == AEECLSID_MEDIAMIDI)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
static boolean CMediaPlayer_IsAudioMedia(CMediaPlayer * pme, char *pExtName)
{
  CPlayerWin *pWin;
  char* pMediaExtName;
  int ret = FALSE;
  AEECLSID classId = 0;

  // We may have to deal with a valid CPlayerWin or just a file extension
  pWin = NULL;
  if((pme->m_eActiveWin == MPW_PLAYER) && (pme->m_pWin != NULL)) 
    pWin = (CPlayerWin *)pme->m_pWin;

  if (pWin && (pWin->m_pMedia))
  {
    pMediaExtName = MP_GetFileNameExtension(pWin->m_pszFile);
    if (pMediaExtName && CMediaPlayer_IsAudioFileExension(pMediaExtName))
    {
      DBGPRINTF_MED("MP: Media file extension is %s.", pMediaExtName);
      return TRUE;
    }
    if (SUCCESS == IMEDIA_GetClassID(pWin->m_pMedia, (AEECLSID *)&classId))
    {
      if (CMediaPlayer_IsAudioClassID(classId))
      {
        DBGPRINTF_MED("MP: Media class id = (%d).", classId);
        return TRUE;
      }
    }
  }

  if (pExtName && CMediaPlayer_IsAudioFileExension(pExtName))
  {
    DBGPRINTF_MED("MP: Media file extension is %s.", pExtName);
    ret = TRUE;
  }
  return ret;
}
#endif // #if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))

static boolean CMediaPlayer_IsAudioFileExension(char* pExtName)
{
  // We currently only care about .mp3, .aac, .wma, .qcp, .mid and .wav files
  if (pExtName && 
        ( !(STRICMP(pExtName, "mp3")) ||  !(STRICMP(pExtName, "aac")) || 
          !(STRICMP(pExtName, "qcp")) ||  !(STRICMP(pExtName, "mid")) || 
          !(STRICMP(pExtName, "wma")) ||  !(STRICMP(pExtName, "wav"))
        )
  )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

static boolean CMediaPlayer_IsCreateMediaCode(AEEEvent eCode)
{
  if((eCode == EVT_CREATEMEDIA) || (eCode == EVT_CREATEMEDIA_QCP))
  {
    DBGPRINTF_MED("MP: IsCreateMediaCode EVT_CREATEMEDIA || EVT_CREATEMEDIA_QCP");
    return TRUE;
  }

#if defined(FEATURE_QTV_FCS)
  if(eCode == EVT_CREATEMEDIA_FCS)
  {
    DBGPRINTF_MED("MP: IsCreateMediaCode EVT_CREATEMEDIA_FCS");
    return TRUE;
  }
#endif

#ifdef FEATURE_APP_MEDIA_VIDEO
#error code not present
#else // not FEATURE_APP_MEDIA_VIDEO
  #if defined(FEATURE_APP_MPEG4)
  if(eCode == EVT_CREATEMEDIA_EX)
  {
    DBGPRINTF_MED("MP: IsCreateMediaCode EVT_CREATEMEDIA_EX");
    return TRUE;
  }
  #endif
  #if defined(FEATURE_APP_MPEG4) && (defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))
  if(eCode == EVT_CREATEPSEUDOMEDIA)
  {
    DBGPRINTF_MED("MP: IsCreateMediaCode EVT_CREATEPSEUDOMEDIA");
    return TRUE;
  }
  #endif
#endif
  DBGPRINTF_MED("MP: IsCreateMediaCode Code not found %d.", eCode);
  return FALSE;
}

#ifdef FEATURE_QTV_FCS
#error code not present
#endif // FEATURE_QTV_FCS


static int MP_LocalPlayPullModeRegisterCallbacks(CMediaPlayer *pMP, IMedia *pMedia)
{
  int nRet = EFAILED;

  DBGPRINTF_MED("MP: Registering callbacks for Local Play PULL Mode: pMP=0x%x, pMedia=0x%x", pMP, pMedia);
#if defined(MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA_SIZE) && defined(MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA)
  if (!pMP)
  {
    DBGPRINTF_ERROR("MP: Invalid MediaPlayer instance 0x%x", pMP);
    return (EFAILED);
  }

  if (!pMedia)
  {
    DBGPRINTF_ERROR("MP: Invalid media instance 0x%x", pMedia);
    return (EFAILED);
  }
    
  nRet = IMEDIA_SetMediaParm(pMedia, MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA_SIZE,
                                 (int32)&MP_LocalPlayPullModeFetchBufferedDataSize, (int32)pMP);
  if (nRet != SUCCESS)
  {
    DBGPRINTF_ERROR("MP: Failed to register cb for MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA_SIZE");
    return (nRet);
  }
 
  nRet = IMEDIA_SetMediaParm(pMedia, MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA,
                                 (int32)&MP_LocalPlayPullModeFetchBufferedData, (int32)pMP);
  if (nRet != SUCCESS)
  {
    DBGPRINTF_ERROR("MP: Failed to register cb for MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA");
    return  (nRet);
  }
#endif //#if defined(MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA_SIZE) && defined(MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA)

  return  (nRet);
}

void MP_LocalPlayPullModeFetchBufferedDataSize(void* pUserData, uint32 *bufDataSize, boolean *pbEndOfData)
{
  uint32       fileSize = 0;
  CMediaPlayer *pMP;

  pMP = (CMediaPlayer*)pUserData;
  
  if (pMP)
  {
    //fileSize = pWin->m_md.dwSize;
    fileSize = pMP->m_nFileSize;
  }

  if (bufDataSize)
  {
    *bufDataSize = fileSize;
  }

  if (pbEndOfData)
  {
    *pbEndOfData = TRUE;
  }
}

uint32 MP_LocalPlayPullModeFetchBufferedData(void* pUserData, void *dataBuf, uint32 readSize, uint32 readPos)
{
  // Verify valid data buffer ptr, and that Mediaplayer app is active, and that the media instance is valid.

  CMediaPlayer *pMP;
  uint32        count = 0;
  fs_rsp_msg_type fsResult;

  //DBGPRINTF_MED("MP: pUserData=0x%x, dataBuf=0x%x, readSize=0x%x, readPos=0x%x, trackId=%d, handle=%d", pUserData, dataBuf, readSize, readPos);

  pMP = (CMediaPlayer*)pUserData;
  if (pMP == NULL)
  {
    return 0;
  }

  if (dataBuf == NULL)
  {
   // DBGPRINTF_ERROR("MP: Bad parm, dataBuf is NULL");
    return 0;
  }

  if(pMP->m_fsHandle == 0)
  {
    //open file
    fs_open( pMP->m_szFullPath, FS_OA_READONLY, 0, 0, &fsResult );
    if ( fsResult.open.status == FS_OKAY_S )
    {
      pMP->m_fsHandle = fsResult.open.handle;
    }
    else
    {
      return 0;
    }
  }
  
  if(pMP->m_fsHandle)
  {
    fs_seek( pMP->m_fsHandle, FS_SEEK_SET, readPos, 0, &fsResult );
    if ( fsResult.seek.status != FS_OKAY_S )
    {
      return 0;
    }

    fs_read( pMP->m_fsHandle, dataBuf, readSize, 0, &fsResult );
    if(fsResult.read.status == FS_OKAY_S)
    {
      if(fsResult.read.count == 0)
	    {
        // end of file? 
        // this should be re. 
        fs_close( pMP->m_fsHandle, NULL, &fsResult );
        pMP->m_fsHandle = 0;
 	    }
	    count = fsResult.read.count;
    }
  }

  //DBGPRINTF_ERROR("MP: read data count %d", count);
  return (count);
}

#endif     // FEATURE_APP_MEDIAPLAYER

/*========================  End of File  =================================*/

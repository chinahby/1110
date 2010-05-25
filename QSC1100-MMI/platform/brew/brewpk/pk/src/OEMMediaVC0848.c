/*===========================================================================

FILE: OEMMediaVC0848.c

SERVICES: IMEDIA interfaces

DESCRIPTION
   This file implements IMedia-based classes on multimedia platform.
   VC0848

          
// 文件:
//        OEMCameraVC0848.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        OEMMedia 处理函数相关文件
//                 
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdlib.h>
#include "OEMFeatures.h"

#if defined(FEATURE_BREW_MULTIMEDIA)

#include "cmx.h"
#include "AEEMedia.h"
#include "OEMClassIDs.h"
#include "OEMMediaFormats.h"
#include "OEMObjectMgr.h"
#include "AEECriticalSection.h"

#include "OEMMedia.h"
#include "AEE_OEMDispatch.h"
#include "AEEStdLib.h"
#include "AEEMediaFormats.h"
#include "AEEMimeTypes.h"
#include "AEEFile.h"
#include "AEE_OEM.h"
#include "AEE_OEMFile.h"
#include "OEMFS.h"
#include "AEEBitmap.h"
#include "mmd_ply_driver.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#ifndef FEATURE_OEMUI_TASK
#include "ui.h"
#else
#include "oemui.h"
#endif
#include "task.h"

#if defined (FEATURE_QAUDIOFX)
#include "OEMMediaAudio.h"
#include "OEMMediaCMX.h"
#include "MEDIAAUDIO3D.BID"
#endif // defined (FEATURE_QAUDIOFX)

#include "AEESource.h"

#include "AEECLSID_MEDIAAMRWB.bid"

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
#include "AEECLSID_SYNTHQUALITY.BID"
#include "OEMSynthQuality.h"
#endif

#if defined (FEATURE_ACM)
#include "OEMACM.h"
#endif // FEATURE_ACM

typedef struct CMediaBg          CMediaBg;
typedef struct CMediaMIDIOutMsg  CMediaMIDIOutMsg;
typedef struct CMediaMIDIOutQCP  CMediaMIDIOutQCP;

//MPEG4 
#include "BREWVersion.h"
#include "msg.h"
#include "OEMMedia.h"
#include "OEMObjectMgr.h"
#include "OEMMediaMPEG4.h"
#include "AEEMimeTypes.h"
#include "AEEMediaFormats.h"
#include "AEEMediaMPEG4.h"
#include "AEEFile.h"
#include "fs.h"
#include "OEMCommonDisp.h"    /* For BREW_OFFSET_X/Y */
#include "OEMFeatures.h"
#include "rex.h"
#include "AEEMediaUtil.h"
#include "AEE_OEMFile.h"
#include "OEMFS.h"
#if MIN_BREW_VERSION(3,0)
# include "OEMHeap.h"
# include "OEMCriticalSection.h"
# include "AEE_OEMDispatch.h"
#endif /* MIN_BREW_VERSION(3,0) */

#ifdef FEATURE_APP_MEDIA_MULTIPLE_FILE
#include "OEMMediaMultipleFile.h"
#endif

#include "vc0848.h"
#include "OEMFSPath_priv.h"

// Define pass thru routine as AEE_SetAppContext is defined only in BREW 2.x
#if MIN_BREW_VERSION(3,0)
//static OEMCriticalSection gcmCriticalSection;
# define MUTEX_LOCK() OEMCriticalSection_Enter(&gcmCriticalSection)
# define MUTEX_UNLOCK() OEMCriticalSection_Leave(&gcmCriticalSection)
# define AEE_SetAppContext AEE_EnterAppContext
#else /* MIN_BREW_VERSION(3,0) */
# define MUTEX_LOCK() OEMOS_MutexStart(); {
# define MUTEX_UNLOCK() } OEMOS_MutexStop();
#endif /* MIN_BREW_VERSION(3,0) */

#define AEEMedia_SetState(pme, nState) \
   {pme->m_nState = nState; \
    pme->m_bStateChanging = FALSE;}

void * pMediaCB = NULL;
AEECLSID  OEMMediaCLSID;
struct CMediaFile
{
	PMEDIA_FILE   plyMediaFile;
	MMD_PLYFILE  pPlayFileHandle;
} MediaFile;
extern unsigned int media_work_mode_initialize(unsigned int work_mode);
/*===========================================================================

                      DEFINES

===========================================================================*/
#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
#define FARF_ALWAYS  1
#else
#define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }

#define FARF_QUEUE   1
#define FARF_SOURCE  1
#define FARF_MEDIA   1
#define OEMMM_RINGER_SILENCE_TIME      1000  // 1 sec (default)

#define OEMMM_TICK_PERIOD              1000  // 1 sec (default)
#define OEMMM_MAX_CB_PLAY              3     // simultaneous play/record cbs
#define OEMMM_DEF_WIPE_COLOR           MAKE_RGB(0xFF, 0, 0)
#define OEMMM_DEF_TEXT_COLOR           MAKE_RGB(0, 0, 0)

#define OEMMM_CLIENT_BUF_INIT_COUNT    3     // 1 each for image, animation, text

//$r$
// NOTE: We need to use AEECriticalSection as OEMMediaCMX.c resides in
// user space and only callback functions get executed in CMX (system)
// context.
static AEECriticalSection gMediaCMXCriticalSection;

#define OEMMM_ENTER_CRITICAL_SECTION()     AEECriticalSection_Enter(&gMediaCMXCriticalSection); {
#define OEMMM_LEAVE_CRITICAL_SECTION()  ;} AEECriticalSection_Leave(&gMediaCMXCriticalSection);

//MPEG4
#define OEMMPEG4_MAX_CB     8
/* How long to poll the UI thread to release the callbacks */
#define UI_TRANSITION_TO_RUN_TIMEOUT        500 /* in ms */
static uint32        guMPEG4RefCnt = 0;

static AEECallback  g_MediaStopCB; // define Callback struct for vc0848 callback VC_PlayCB

/*===============================================================================

      OEMMedia DECLARATIONS
      These are used by all IMedia classes implemented in this file.

===============================================================================*/

typedef struct CMXInfo
{
   int16             nType;   // OEMCMX_TYPE_XXX
   cmx_handle_type * ph;      // Set for Start()/TotalTime command
   uint32            dwCB;
   uint32            dwUserData;
} CMXInfo;

typedef struct CMXDataRequest
{
   uint32   dwPos;
   uint32   dwMaxLen;
   cmx_client_req_cb_func_ptr_type pfnDataReq;
   void *   pServerUserData;
} CMXDataRequest;

#if !defined(FEATURE_BREW_CMX_V3)
/* Structures for QCP recording configuration parameters */
typedef struct {
  cmx_qcp_format_type   format;       /* QCP recording mode            */
  uint32                report_ms;    /* Time callback interval in ms  */
  uint32                auto_stop_ms; /* Silence time to end recording */
} cmx_qcp_rec_para_type;
#endif // !defined(FEATURE_BREW_CMX_V3)

typedef struct OEMMedia
{
   AEECLSID       m_clsSelf;

   CMXInfo        m_ciStart;
   CMXInfo        m_ciStatus;

   // Play/Ringer
   int16          m_eType; // CMX_AUDFMT_START_XXX
   uint32         m_dwPauseMS;     // Pause between ringers

#if defined(FEATURE_QCP)
   // QCPRecord
   cmx_qcp_rec_para_type * m_pQCPRecParm;
#endif // defined(FEATURE_QCP)

#if defined(FEATURE_CMX_RAW_PLAYBACK)
   cmx_codec_spec_type *   m_pCodecSpec;
   uint32                  m_dwAudioSync;
#endif

   // MIDIOutQCP
   uint16         m_wVolume;
   uint16         m_wPan;
   uint32         m_dwChannel;
#if !defined (FEATURE_CMX_MM_API)
#if defined(FEATURE_MIDI_OUT_QCP)
   uint32         m_dwRepeatCount;
#endif // defined(FEATURE_MIDI_OUT_QCP)
#endif // defined (FEATURE_CMX_MM_API)

#ifdef FEATURE_MULTISEQUENCER
   // Multisequencer
   cmx_audfmt_multi_param_type * m_pMultiParam;
   void *                        m_pSequence;
   cmx_audfmt_seq_param_type *   m_pSeqParam;

#endif  //FEATURE_MULTISEQUENCER

#ifdef FEATURE_PCM_REC
   // PCM Record
   cmx_pcm_rec_para_type * m_pPCMRecParm;
   cmx_pcm_rec_link_type   m_Link;
#endif //FEATURE_PCM_REC

#ifdef FEATURE_AAC_REC
   cmx_mm_rec_param_type * m_pAACRecParm;
#endif // FEATURE_AAC_REC

#if defined (FEATURE_CMX_MM_API)
   cmx_mm_session_handle_type hsession;
   uint32                     nResult;
#endif // defined (FEATURE_CMX_MM_API)

#if defined(FEATURE_QCP) || defined(FEATURE_AAC_REC)
   flg            m_bReverse:1;  // Record locally (ReverseLink)
#endif // defined(FEATURE_QCP) || defined(FEATURE_AAC_REC)
} OEMMedia;

#define OEMCMX_ID_TYPE              1  // p1 = CMXInfo *, p2/p3 = see comments in OEMCMX_TYPE_XXX
#define OEMCMX_ID_STATUS_CB         2  // p1 = CMXInfo *
#define OEMCMX_ID_TOTALTIME         3  // p1 = CMXInfo *
#define OEMCMX_ID_VOLUME            4  // p1 = Volume
#define OEMCMX_ID_PAN               5  // p1 = Pan
#define OEMCMX_ID_TEMPO             6  // p1 = Tempo
#define OEMCMX_ID_TUNE              7  // p1 = Tune
#define OEMCMX_ID_MIDI_DEVICE       8  // p1 = TRUE/FALSE => Open/Close, p2 = ADECType, p3 = CMXInfo *
#define OEMCMX_ID_CHANNEL           9  // p1 = Channel #
#define OEMCMX_ID_DATA_RESPONSE     10 // p1 = CMXDataRequest *, p2 = uint8 * (pBuf), p3 = sizeof buffer in p2
#define OEMCMX_ID_SEQUENCE         11 // p1 = Sequence, p2 = cmx_audfmt_seq_param_type *
#define OEMCMX_ID_LINK             12 // p1 = link => forward or reverse link for PCM recording operations

#if !defined (FEATURE_CMX_MM_API)
#if defined(FEATURE_MIDI_OUT_QCP)
#define OEMCMX_ID_REPEAT             13 // p1 repeat count
#endif // defined(FEATURE_MIDI_OUT_QCP)
#endif // defined (FEATURE_CMX_MM_API)

#define OEMCMX_TYPE_AUDFMT_PLAY     1  // p2 = CMX_AUDFMT_START_XXX
#define OEMCMX_TYPE_AUDFMT_RINGER   2  // p2 = CMX_AUDFMT_START_XXX, p3 = Pause time
#define OEMCMX_TYPE_QCP_RECORD      3  // p2 = cmx_qcp_rec_para_type *, p3 = TRUE/FALSE => ReverseLink(Local)/ForwardLink
#define OEMCMX_TYPE_MIDI_OUT_MSG    4  //
#define OEMCMX_TYPE_MIDI_OUT_QCP    5  //
#define OEMCMX_TYPE_AUDFMT_PLAY_CODEC  6  // p2 = cmx_codec_spec_type *
#define OEMCMX_TYPE_AUDFMT_PLAY_MULTI  7  // p2 = cmx_audfmt_multi_param_type *
#define OEMCMX_TYPE_PCM_RECORD         8  // p2 = cmx_pcm_rec_para_type *
#define OEMCMX_TYPE_AAC_RECORD		   10 // p2 = cmx_mm_rec_param_type *

static int  OEMMedia_New(AEECLSID cls, OEMMedia ** ppme);
static void OEMMedia_Delete(OEMMedia * pme);
static int  OEMMedia_Start(OEMMedia * pme);
static int  OEMMedia_Stop(OEMMedia * pme);
static int  OEMMedia_PauseResume(OEMMedia * pme, boolean bPause);
#define     OEMMedia_Pause(pme)   OEMMedia_PauseResume(pme, TRUE)
#define     OEMMedia_Resume(pme) OEMMedia_PauseResume(pme, FALSE)
static int  OEMMedia_Seek(OEMMedia * pme, AEEMediaSeek eSeek, int32 lTimeMS);
static int  OEMMedia_SetParm(OEMMedia * pme, int16 nParmID, int32 p1, int32 p2, int32 p3, boolean bLocal);

/*===============================================================================

      CALLBACK FUNCTIONS.
      Called in CMX context.

===============================================================================*/
static void CMediaMMLayer_PlayCB(cmx_status_type status, const void * pClientData, cmx_af_server_data_type * psd);
static void CMediaMMLayer_RingerCB(cmx_status_type status, const void * pClientData);
static void CMediaMMLayer_StatusCB(cmx_status_type status, const void * pClientData);
static void CMediaMMLayer_TimeCB(cmx_status_type status, const void * pClientData, uint32 dwTimeMS);
#if defined(FEATURE_QCP) || defined(FEATURE_PCM_REC) || defined(FEATURE_AAC_REC)
static void CMediaMMLayer_RecordCB(cmx_status_type status, const void * pClientData);
#endif // FEATURE_QCP || FEATURE_PCM_REC

#if defined(FEATURE_MIDI_OUT)
static void CMIDIOut_MIDIDeviceCB(cmx_status_type status, const void * pClientData);
static void CMediaMMLayer_MIDIOutMsgCB(cmx_status_type status, const void * pClientData);

#if defined(FEATURE_MIDI_OUT_QCP)
static void CMediaMMLayer_MIDIOutQCPStatusCB(cmx_status_type status, const void * pClientData);
static void CMediaMMLayer_MIDIOutQCPPlayCB(cmx_status_type status, const void * midi_out_qcp_id, const void * pClientData);

#if defined (FEATURE_QAUDIOFX)
#if !defined (FEATURE_CMX_MM_API)
static void CMediaMIDIOutQCP_QCPStartCB(void * pUser);
#endif // !defined (FEATURE_CMX_MM_API)
static void CMediaMIDIOutQCP_FlushParamCB (cmx_status_type status, const void *client_data);
#endif // defined (FEATURE_QAUDIOFX)
#endif // defined(FEATURE_MIDI_OUT_QCP)
#endif // defined(FEATURE_MIDI_OUT)
static void CMediaMMLayer_DataRequestCB(uint32 position, uint32 max_len, cmx_client_req_cb_func_ptr_type data_req_cb,
                                           const void *server_client_data, const void *client_data);
#if defined(FEATURE_CMX_AV_SYNC)
static void CMediaMMLayer_CMXAudioSyncCB(qword timestamp, qword num_of_samples, qword num_of_bytes, const void  *client_data);
#endif

/*===============================================================================

      CMediaCMX DECLARATIONS
      These are used by all IMedia classes implemented in this file.

===============================================================================*/
#ifdef FEATURE_ACM
#define INHERIT_CMediaCMX(iname) \
   INHERIT_AEEMedia(iname); \
   AEEObjectHandle   m_hObject;  \
   PFNNOTIFY         m_pfnDelete; \
   OEMMedia*         m_pMediaCMX; \
   \
   uint32            m_dwCaps; \
   uint32            m_dwEnabledCaps; \
   int               m_nAPath; \
   boolean           m_bMute; \
   uint16            m_wVolume; \
   uint16            m_wPan; \
   uint16            m_wTempo; \
   uint16            m_wTune; \
   uint16            m_wSilenceTimeMS; \
   uint32            m_dwTickTimeMS; \
   uint32            m_dwRepeatCount; \
   flg               m_bStopping:1; \
   flg               m_bMIDIOut:1; \
   flg               m_bVolumeScaled:1; \
   int               m_nPlayType
#else // ! FEATURE-ACM
#define INHERIT_CMediaCMX(iname) \
   INHERIT_AEEMedia(iname); \
   AEEObjectHandle   m_hObject;  \
   PFNNOTIFY         m_pfnDelete; \
   OEMMedia*         m_pMediaCMX; \
   \
   uint32            m_dwCaps; \
   uint32            m_dwEnabledCaps; \
   int               m_nAPath; \
   boolean           m_bMute; \
   uint16            m_wVolume; \
   uint16            m_wPan; \
   uint16            m_wTempo; \
   uint16            m_wTune; \
   uint16            m_wSilenceTimeMS; \
   uint32            m_dwTickTimeMS; \
   uint32            m_dwRepeatCount; \
   flg               m_bStopping:1; \
   flg               m_bMIDIOut:1; \
   flg               m_bVolumeScaled:1
#endif // FEATURE_ACM


OBJECT(CMediaCMX)
{
   INHERIT_CMediaCMX(IMediaMIDI);
};

// Constructor and destructor
static IBase * CMediaCMX_New(IShell * ps, AEECLSID cls, const void * pVtbl, uint32 dwSize, PFNNOTIFY pfnDelete);
static void    CMediaCMX_Delete(CMediaCMX * po);

// Interface functions
static uint32  CMediaCMX_Release(IMedia * po);
static int     CMediaCMX_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int     CMediaCMX_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);

static int     CMediaCMX_RecordUnsupported(IMedia * po);
static int     CMediaCMX_StopUnsupported(IMedia * po);
static int     CMediaCMX_SeekUnsupported(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS);
static int     CMediaCMX_PauseUnsupported(IMedia * po);
static int     CMediaCMX_ResumeUnsupported(IMedia * po);
static int     CMediaCMX_GetTotalTimeUnsupported(IMedia * po);

// Callback from due to resume in ASIC MM Layer callback.
static void    CMediaCMX_CallbackNotify(void * pUser);

// MMLayer global static objects...
typedef struct CMediaInitFlags
{
   flg   gbCMXInit:1;
   flg   gbMediaBgInit:1;
   flg   gbMediaMIDIOutQCPInit:1;
} CMediaInitFlags;

static CMediaInitFlags  gInitFlags;

static AEECallback   gcbAEEExit;

#if defined(FEATURE_MIDI_OUT)

#define OEMMM_ADEC_MODE_DEFAULT  0  // Default type of QSynth ADEC
#define OEMMM_ADEC_MODE_ADPCM    1  // QSynth ADPCM audio decoder
#define OEMMM_ADEC_MODE_QCP      2  // QSynth QCELP audio decoder
#define OEMMM_ADEC_MODE_PCM      3  // QSynth PCM audio decoder

typedef void (*PFNMIDIOUTNOTIFY)(CMediaCMX * pme, boolean bSuccess);

typedef struct CMIDIOut
{
   int               m_nRef;
   byte              m_nADECType;   // For CMX 2.x, zero. For CMX 3.x, OEMMM_ADEC_MODE_XXX
   boolean           m_bOpenPending;
   PFNMIDIOUTNOTIFY  m_pfnNotify;
   CMediaCMX *       m_pMM;
   AEECallback       m_cb;
} CMIDIOut;

static int CMIDIOut_AddRef(void);
static int CMIDIOut_Release(void);
static int CMIDIOut_Open(byte nADECType, PFNMIDIOUTNOTIFY pfn, CMediaCMX * pme);
static void CMIDIOut_OpenNotify(void * pUser);

static CMIDIOut   gsMIDIOut = { 0 };

#define CMIDIOut_IsOpen()        (gsMIDIOut.m_nRef > 0)
#define CMIDIOut_IsDefault()     (gsMIDIOut.m_nADECType == OEMMM_ADEC_MODE_DEFAULT)
#define CMIDIOut_IsQCP()         (gsMIDIOut.m_nADECType == OEMMM_ADEC_MODE_QCP || CMIDIOut_IsDefault())
#define CMIDIOut_IsADPCM()       (gsMIDIOut.m_nADECType == OEMMM_ADEC_MODE_ADPCM || CMIDIOut_IsDefault())
#define CMIDIOut_IsPCM()         (gsMIDIOut.m_nADECType == OEMMM_ADEC_MODE_PCM || CMIDIOut_IsDefault())

#endif // defined(FEATURE_MIDI_OUT)

//MPEG4
// MPEG4 IMedia objects must contain a reference to this object.
// Note: This class is a friend of IMedia and/or IMedia-based classes.
//
typedef struct OEMMediaMPEG4Layer
{
   AEEMedia *        m_pMedia;           // Owner IMedia object
   void *            m_pAppContext;      // App context
#ifdef FEATURE_MP4_MSM6050
   OBJECTCONTEXT     m_cxtMedia;         // Context returned by ObjectMgr
#else /* FEATURE_MP4_MSM6050 */
   AEEObjectHandle   m_hObject;          // Context returned by ObjectMgr
#endif /* FEATURE_MP4_MSM6050 */
   PFNNOTIFY         m_pfnNotify;        // IMedia notification function

   AEEMediaCallback  m_cb[OEMMPEG4_MAX_CB];
   uint32            m_dwCaps;           // Capabilities of IMedia
   ISound *          m_pISound;
   AEEMediaCallback  m_cbCommand;
   boolean           m_bFrameCBEnabled;  // Frame callbacks enabled?
   boolean           m_bClearScreen;     // Clear screen on the next frame callback?
   IDisplay *        m_pIDisplay;        // Pointer to display interface
   uint32            m_iPrevHeight;      // Dimensions of previous frame.  Used
   uint32            m_iPrevWidth;       // to know when to clear screen
   boolean bTerminating;				 //Set to true before calling OEMMediaMPEG42PV_Terminate
} OEMMediaMPEG4Layer;

OBJECT(OEMMediaMPEG4)
{
   INHERIT_AEEMedia(IMediaMPEG4);
   AEERect  m_rectImage;
   boolean  m_bRelMark;
   OEMMediaMPEG4Layer *m_pMPEG4;
#ifdef FEATURE_SMALL_DISPLAY_SCREEN
   boolean  m_bscalevideo;                //if this flag is true scale video to fit in available screen dimensions
#endif /* FEATURE_SMALL_DISPLAY_SCREEN */
};

/*===========================================================================
      MMLayer functions
===========================================================================*/

static void CMediaMMLayer_Init(IShell * ps);
static void CMediaMMLayer_AEEExitNotify(void * pUser);
static void CMediaMMLayer_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eStatus, uint32 dwParam);
static void CMediaMMLayer_InitCallback(IMedia * po, int nCmd, AEEMediaCallback * pcb, int nMax, PFNNOTIFY pfn);
static AEEMediaCallback * CMediaMMLayer_AllocCallback(AEEMediaCallback * pcb, int nMax, int nCmd, int nSubCmd);
static void CMediaMMLayer_FreeCallback(AEEMediaCallback * pcb);
static void CMediaMMLayer_CancelCallback(AEEMediaCallback * pcb, int nMax);

#ifdef FEATURE_MULTISEQUENCER
/* List of all class ids that are supported in the MultiSequence API */

static AEECLSID multiSequencerClasses[] = {AEECLSID_MEDIAMIDI,
                                           AEECLSID_MEDIAMMF,
                                           AEECLSID_MEDIAPHR,
                                           AEECLSID_MEDIAXMF};

static boolean CMediaMMLayer_IsMultiSequence(CMediaBg *pme);
#endif /* FEATURE_MULTISEQUENCER */

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
static int CMediaMMLayer_SetSynthQuality(CMediaBg *pme);
#endif

/*---------------------------------------------------------------------------
      Sound functions
-----------------------------------------------------------------------------*/
static void CMediaMMLayer_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eSPStatus, uint32 dwParam);
static int CMediaMMLayer_SetVolume(CMediaBg * pme, uint16 wVolume);
static int CMediaMMLayer_SetDevice(CMediaBg * pme, AEESoundDevice eDevice);
static int CMediaMMLayer_SetPath(CMediaBg * pme, int nAPath);
static int CMediaMMLayer_SetMute(CMediaBg * pme, boolean bMute);

/*---------------------------------------------------------------------------
      Media play/record
-----------------------------------------------------------------------------*/
static int CMediaMMLayer_Play(CMediaBg * pme);
static int CMediaMMLayer_PlayCodec(CMediaBg * pme, cmx_handle_type * ph, AEEMediaCallback * pcb);
static int CMediaMMLayer_Stop(CMediaBg * pme);
static int CMediaMMLayer_Seek(CMediaBg * pme, AEEMediaSeek eSeek, int32 lTimeMS);
static int CMediaMMLayer_Pause(CMediaBg * pme);
static int CMediaMMLayer_Resume(CMediaBg * pme);
static int CMediaMMLayer_GetTotalTime(CMediaBg * pme);

static int CMediaMMLayer_SetTempo(CMediaBg * pme, uint32 dwTempo);
static int CMediaMMLayer_SetTune(CMediaBg * pme, uint32 dwTune);
static int CMediaMMLayer_SetPan(CMediaBg * pme, uint32 dwTune);

static int CMediaMMLayer_QCPRecord(CMediaBg * pme, int nFormat);
static int CMediaMMLayer_PCMRecord(CMediaBg * pme);
static int CMediaMMLayer_AMRRecord(CMediaBg * pme);

#ifdef FEATURE_AAC_REC
static int CMediaMMLayer_AACRecord(CMediaBg * pme, int nFormat);
#endif //FEATURE_AAC_REC

//$r$
static void CMediaMMLayer_FlushStreamData(CMediaBg * pme);
#if defined(FEATURE_QCP)
static int CMediaMMLayer_GetQCPFormat(int nFormat, int * pCodec);
#endif // FEATURE_QCP
/*---------------------------------------------------------------------------
      Media play thru MIDIOut device
-----------------------------------------------------------------------------*/
static int CMediaMMLayer_MIDIOutMsg(CMediaMIDIOutMsg * pme);

static int CMediaMMLayer_MIDIOutQCPPlay(CMediaMIDIOutQCP * pme);
static int CMediaMMLayer_MIDIOutQCPStop(CMediaMIDIOutQCP * pme, uint32 dwChannel);
static int CMediaMMLayer_MIDIOutQCPPause(CMediaMIDIOutQCP * pme);
static int CMediaMMLayer_MIDIOutQCPResume(CMediaMIDIOutQCP * pme);
static int CMediaMMLayer_MIDIOutQCPSetVolume(CMediaMIDIOutQCP * pme, uint32 dwChannel, uint16 wVolume);
static int CMediaMMLayer_MIDIOutQCPSetPan(CMediaMIDIOutQCP * pme, uint32 dwChannel, uint16 wPan);

#if defined (FEATURE_ACM)
static int CMediaBg_AcmPlayCB(ACM_Callback_data_type * po);
static int CMediaBg_AcmRecordCB(ACM_Callback_data_type *po);

static int CMediaMIDIOutQCP_AcmPlayCB(ACM_Callback_data_type *po);
static int CMediaMIDIOutMsg_AcmPlayCB(ACM_Callback_data_type *po);

void CMediaBg_GetResourceCntlData(AEEMedia * po, int * nPlayType, flg * channelShare);
void CMediaBg_SetResourceCntlCBData(uint32 * acmCB, flg isPlay);
void CMediaMIDIOutQCP_GetResourceCntlData(AEEMedia *po, int * nPlayType, flg * channelShare, uint32 * acmCB);
void CMediaMIDIOutMsg_GetResourceCntlData(AEEMedia *po, int * nPlayType, uint32 * acmCB);

extern void AEEMedia_CleanUpResource(AEEMedia * po);
#endif // FEATURE_ACM

/*===============================================================================

      CMediaBg: Background Audio (MIDI, MP3, QCP. Includes PMD) related.

===============================================================================*/
typedef struct CQueue
{
   uint8 *  pBuffer;    // Size of the queue
   int32    nMaxSize;   // Max size (bytes) of the queue
   int32    nWrite;     // Write pointer
   int32    nRead;      // Read pointer
   int32    nFill;      // Size (bytes) of data filled in the queue
   boolean  bMutex;     // Specifies if mutex lock is required for operations
} CQueue;

static int     CQueue_New(int32 nSize, boolean bMutex, CQueue ** ppQ);
static void    CQueue_Delete(CQueue * pme);
static int32   CQueue_Put(CQueue * pme, uint8 * pData, int32 nSize);
static int32   CQueue_Get(CQueue * pme, uint8 * pData, int32 nSize);
#define        CQueue_MaxSize(pme)           ((pme)->nMaxSize)
#define        CQueue_Size(pme)              ((pme)->nFill)
#define        CQueue_IsEmpty(pme)           (0 == (pme)->nFill)
#define        CQueue_IsFull(pme)            ((pme)->nFill >= (pme)->nMaxSize)
#define        CQueue_IsPutOk(pme, nSize)    ((pme)->nFill + (nSize) <= (pme)->nMaxSize)
#define        CQueue_Flush(pme)             ((pme)->nWrite = (pme)->nRead = (pme)->nFill = 0)
#define        CQueue_GetBuffer(pme)         ((pme)->pBuffer)

#define OEMMM_CMX_REQ_SIZE_MAX         (4 * sizeof(CMXDataRequest))  // CMX request buffers
#define OEMMM_BUFFER_UNIT_SIZE         (8820)                        // IMedia data fetch unit size
#define OEMMM_BUFFER_SIZE_MAX          (4 * OEMMM_BUFFER_UNIT_SIZE)  // IMedia data buffer size (default)
#define OEMMM_ISSTREAMING(pme)         ((pme)->m_bStreaming)

typedef struct CMediaMP3
{
   AEEMediaMP3Spec   m_spec; // MP3
} CMediaMP3;

typedef struct CMediaQCP
{
   byte  m_nFormat;     // QCP
} CMediaQCP;

typedef struct CMediaADPCM
{
   AEEMediaWaveSpec  m_spec; // IMA-ADPCM
} CMediaADPCM;

#ifdef FEATURE_AAC_REC
typedef struct CMediaAAC
{
   byte  m_nFormat; // AAC FORMATS (ADTS / ADIF)
} CMediaAAC;
#endif //FEATURE_AAC_REC


struct CMediaBg
{
   INHERIT_CMediaCMX(IMediaMIDI);

   union u
   {
      CMediaMP3 *    m_pMP3;
      CMediaQCP *    m_pQCP;
      CMediaADPCM *  m_pADPCM;
	  
#ifdef FEATURE_AAC_REC
	  CMediaAAC *    m_pAAC;
#endif // FEATURE_AAC_REC
   } u;

   AEEMediaCallback  m_cbPlay[OEMMM_MAX_CB_PLAY];
   AEEMediaCallback  m_cbCommand;
   AEEMediaCallback  m_cbGetTotalTime;

   ISound *          m_pISound;

   CQueue *          m_pqCMXDataReq;
   AEECallback *     m_pcbCMXDataReq;
   CQueue *          m_pqMediaData;
   AEECallback *     m_pcbMediaData;
   uint8 *           m_pSrcBuffer;
   uint32            m_dwSrcBufferSize;

#if defined(FEATURE_CMX_AV_SYNC)
   uint32            m_dwAudioSyncType;   // Time based or sample based
   uint32            m_dwAudioSyncData;   // Time in ms or samples
   AEEMediaAudioSync *m_pAudioSyncInfo;   // CB info
   uint64            m_qwAudioSyncBytes;
#endif //defined(FEATURE_CMX_AV_SYNC)

   flg               m_bStreaming:1;
   flg               m_bEOS:1;
   void   *          m_pSequence;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
   uint32            m_dwNotes;
   boolean           m_bISynthQualityRef;
#endif
};

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
#define OEMMM_MAX_NOTES    72 // Accommodate as many notes as possible (HP_ALL mode)
static uint32              gdwNotes = OEMMM_MAX_NOTES;
static boolean             gbNotesSet = FALSE;
static ISynthQuality      *gpISynthQuality = NULL;
#endif

void IMediaVC0848Bg_Init(IShell * ps);
int  IMediaVC0848Bg_New(IShell * ps, AEECLSID cls, void **ppif);

static int     CMediaBg_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int     CMediaBg_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);
static int     CMediaBg_Play(IMedia * po);
static int     CMediaBg_Record(IMedia * po);
static int     CMediaBg_Stop(IMedia * po);
static int     CMediaBg_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS);
static int     CMediaBg_Pause(IMedia * po);
static int     CMediaBg_Resume(IMedia * po);
static int     CMediaBg_GetTotalTime(IMedia * po);

void    CMediaBg_CallbackNotify(void * pUser);
static void    CMediaBg_Delete(CMediaBg * pme);
char *         get_pExtName(char* fullname, char tag);
void CMediaMMLayer_CallbackTimer(AEEMediaCallback *   pcb);
extern MMD_PLYFILE pFileHandle;

static const VTBL(IMediaMIDI) gMediaBgFuncs =
{
   AEEMedia_AddRef,
   CMediaCMX_Release,
   AEEMedia_QueryInterface,
   AEEMedia_RegisterNotify,
   CMediaBg_SetMediaParm,
   CMediaBg_GetMediaParm,
   CMediaBg_Play,
   CMediaBg_Record,
   CMediaBg_Stop,
   CMediaBg_Seek,
   CMediaBg_Pause,
   CMediaBg_Resume,
   CMediaBg_GetTotalTime,
   AEEMedia_GetState
};

/*===========================================================================

                      IMediaMIDIOutMsg DECLARATIONS

===========================================================================*/
struct CMediaMIDIOutMsg
{
   INHERIT_CMediaCMX(IMediaMIDIOutMsg);

   uint8 *           m_pMsg;
   uint32            m_dwSize;

   AEEMediaCallback  m_cbPlay[OEMMM_MAX_CB_PLAY];
   AEEMediaCallback  m_cbCommand;
};

void IMediaVC0848MIDIOutMsg_Init(IShell * ps);
int  IMediaVC0848MIDIOutMsg_New(IShell * ps, AEECLSID cls, void **ppif);

static int CMediaMIDIOutMsg_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int CMediaMIDIOutMsg_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);
static int CMediaMIDIOutMsg_Play(IMedia * po);

static void CMediaMIDIOutMsg_Delete(CMediaMIDIOutMsg * pme);

static const VTBL(IMediaMIDIOutMsg) gMediaMIDIOutMsgFuncs =
{
   AEEMedia_AddRef,
   CMediaCMX_Release,
   AEEMedia_QueryInterface,
   AEEMedia_RegisterNotify,
   CMediaMIDIOutMsg_SetMediaParm,
   CMediaMIDIOutMsg_GetMediaParm,
   CMediaMIDIOutMsg_Play,
   CMediaCMX_RecordUnsupported,
   CMediaCMX_StopUnsupported,
   CMediaCMX_SeekUnsupported,
   CMediaCMX_PauseUnsupported,
   CMediaCMX_ResumeUnsupported,
   CMediaCMX_GetTotalTimeUnsupported,
   AEEMedia_GetState
};

/*===========================================================================

                      IMediaMIDIOutQCP DECLARATIONS

===========================================================================*/
#if defined (FEATURE_QAUDIOFX)

typedef struct paramCache paramCache;
typedef struct paramCache
{
  cmx_3d_pos_cmd_enum_type  cmd;
  cmx_3d_pos_param_type     cmxParam;
  paramCache               *pNext;
} paramCache;
#endif // defined (FEATURE_QAUDIOFX)

struct CMediaMIDIOutQCP
{
   INHERIT_CMediaCMX(IMediaMIDIOutQCP);

   uint32            m_dwChannel;
   flg               m_bChannelSet:1;  //true if channel has been set

   AEEMediaCallback  m_cbPlay[OEMMM_MAX_CB_PLAY];
   AEEMediaCallback  m_cbCommand;
#if defined (FEATURE_QAUDIOFX)
#if !defined (FEATURE_CMX_MM_API)
   AEEMediaCallback  m_cb3DStart;
#endif // !defined (FEATURE_CMX_MM_API)
   cmx_3d_pos_get_param_type   m_cmxGetParam;
   volatile paramCache       * m_pCache;
   paramCache                * m_pLast;
#if !defined (FEATURE_CMX_MM_API)
   flg                         m_bPlaying:1;
   flg                         m_bStarting:1;
   flg                         m_bIgnore3D:1;
#endif // !defined (FEATURE_CMX_MM_API)
#endif // defined (FEATURE_QAUDIOFX)
};

void IMediaVC0848MIDIOutQCP_Init(IShell * ps);
int  IMediaVC0848MIDIOutQCP_New(IShell * ps, AEECLSID cls, void **ppif);

static int CMediaMIDIOutQCP_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int CMediaMIDIOutQCP_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);
static int CMediaMIDIOutQCP_Play(IMedia * po);
static int CMediaMIDIOutQCP_Stop(IMedia * po);
static int CMediaMIDIOutQCP_Pause(IMedia * po);
static int CMediaMIDIOutQCP_Resume(IMedia * po);

static void CMediaMIDIOutQCP_Delete(CMediaMIDIOutQCP * pme);

static const VTBL(IMediaMIDIOutQCP) gMediaMIDIOutQCPFuncs =
{
   AEEMedia_AddRef,
   CMediaCMX_Release,
   AEEMedia_QueryInterface,
   AEEMedia_RegisterNotify,
   CMediaMIDIOutQCP_SetMediaParm,
   CMediaMIDIOutQCP_GetMediaParm,
   CMediaMIDIOutQCP_Play,
   CMediaCMX_RecordUnsupported,
   CMediaMIDIOutQCP_Stop,
   CMediaCMX_SeekUnsupported,
   CMediaMIDIOutQCP_Pause,
   CMediaMIDIOutQCP_Resume,
   CMediaCMX_GetTotalTimeUnsupported,
   AEEMedia_GetState
};

/*===========================================================================

                      IMediaMPEG4 DECLARATIONS

===========================================================================*/
static uint32 OEMMediaMPEG4_Release(IMedia * po);
static int    OEMMediaMPEG4_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int    OEMMediaMPEG4_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 *  pP2);
static int    OEMMediaMPEG4_Play(IMedia * po);
static int    OEMMediaMPEG4_RecordUnsupported(IMedia * po);
static int    OEMMediaMPEG4_Stop(IMedia * po);
static int    OEMMediaMPEG4_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS);
static int    OEMMediaMPEG4_Pause(IMedia * po);
static int    OEMMediaMPEG4_Resume(IMedia * po);
static int    OEMMediaMPEG4_GetTotalTime(IMedia * po);
static IBase *OEMMediaMPEG4_New(IShell * ps, AEECLSID cls);
static void   OEMMediaMPEG4_Delete(IMedia * po, boolean bFree);
static OEMMediaMPEG4Layer * OEMMediaMPEG4Layer_New(IMedia * po, PFNNOTIFY pfn);
static void   OEMMediaMPEG4Layer_Delete(OEMMediaMPEG4Layer * pMPEG4);
static int OEMMediaMPEG4_OpenURN(AEEMediaDataEx *pmdExList, int nCount);
void OEMMediaMPEG4_CallbackNotify(AEEMediaCallback * pcb);

#if MIN_BREW_VERSION(3,0)
static int OEMMediaMpeg4_FilePathEx(AEEMediaDataEx * pmdList, int nListCount);
#endif

static const VTBL(IMediaMPEG4) gMediaMPEG4Funcs =
{
   AEEMedia_AddRef,
   OEMMediaMPEG4_Release,
   AEEMedia_QueryInterface,
   AEEMedia_RegisterNotify,
   OEMMediaMPEG4_SetMediaParm,
   OEMMediaMPEG4_GetMediaParm,
   OEMMediaMPEG4_Play,
   OEMMediaMPEG4_RecordUnsupported,
   OEMMediaMPEG4_Stop,
   OEMMediaMPEG4_Seek,
   OEMMediaMPEG4_Pause,
   OEMMediaMPEG4_Resume,
   OEMMediaMPEG4_GetTotalTime,
   AEEMedia_GetState
};

/*===============================================================================

      COMMON FUNCTION DEFINITIONS
      Can be used by all IMedia classes implemented in this file

===============================================================================*/
#include "musicplayer.h"
#include "OEMCFGI.h"
//  1: working, 0:idle
static boolean oem_musicplayer_status = 0;

int oem_media_scheduler(void)
{
    boolean b_FMBackground = FALSE;
    
    OEM_GetConfig(CFGI_FM_BACKGROUND,&b_FMBackground, sizeof(b_FMBackground));
    if((TRUE == b_FMBackground) && (AEECLSID_APP_FMRADIO != ISHELL_ActiveApplet(AEE_GetShell())))
    {
       return EITEMBUSY;      
    }
    else if(1 == oem_musicplayer_status)
    {
       return EITEMBUSY; 
    }
    else if(1 == Appscommon_bluetooth_used_sd(-1))
    {
        return EITEMBUSY; 
    }
    return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMedia_New(AEECLSID cls, OEMMedia ** ppme)
{

   if (!ppme)
      return EBADPARM;

   *ppme = (OEMMedia *)MALLOC(sizeof(OEMMedia));
   if (!*ppme)
      return ENOMEMORY;

   (*ppme)->m_clsSelf = cls;
#if !defined (FEATURE_CMX_MM_API)
#if defined(FEATURE_MIDI_OUT_QCP)
   (*ppme)->m_dwRepeatCount = 1;
#endif // defined(FEATURE_MIDI_OUT_QCP)
#endif//  defined (FEATURE_CMX_MM_API)


   return SUCCESS;
}

/*==================================================================

==================================================================*/
static void OEMMedia_Delete(OEMMedia * pme)
{
#if defined (FEATURE_CMX_MM_API)
   if (pme->hsession)
   {
      cmx_mm_playback_param_type param;

      param.session_handle = pme->hsession;
      param.cb_func = NULL;
      param.client_data = NULL;

      cmx_mm_terminate_session(&params);
   }
#endif // defined (FEATURE_CMX_MM_API)
   FREE(pme);
}

/*==================================================================

==================================================================*/
static int  OEMMedia_Start(OEMMedia * pme)
{
   void *    pUser = (void *)pme->m_ciStart.dwUserData;

   if (!pme->m_ciStart.ph)
      return EBADPARM;

   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY:
#if defined (FEATURE_CMX_MM_API)
         OEMMedia_StartPlaySession(pme);
#else // !defined (FEATURE_CMX_MM_API) 
#if 0 // debug for mp3 play
      pExtName = get_pExtName(pme->m_ciStart.ph->efs.filename, '.');
      if((strcmp(pExtName, "mid") == 0) || (strcmp(pExtName, "MID") == 0)
      || (strcmp(pExtName, "midi") == 0) || (strcmp(pExtName, "MIDI") == 0))
      {
          cmx_audfmt_play(pme->m_ciStart.ph, CMX_AF_CALLBACK_VALUE, (cmx_audfmt_start_enum_type)pme->m_eType,
                          (cmx_audfmt_play_cb_func_ptr_type)pme->m_ciStart.dwCB,
                          pUser);
       
      }
      else
      {
#endif // if 0 debug for mp3 play
      {
         vc_union_type vc_data;
         char * pFile = NULL;
         vc_data.play_info.play_mode = VC_PLAY_M;
         if(CMX_SOURCE_MEM == pme->m_ciStart.ph->source_type)
         {
              vc_data.play_info.pFileaddr = pme->m_ciStart.ph->mem.buf;
              vc_data.play_info.fileLen = pme->m_ciStart.ph->mem.len;	  
         }
         else if(CMX_SOURCE_EFS == pme->m_ciStart.ph->source_type)
         {
              pFile = SPLITPATH((char *)&pme->m_ciStart.ph->efs.filename, OEMFS_CARD0_DIR);
              vc_data.play_info.file_scr = VC_IN_CORE_FILE;
              if(!pFile)
              {
                  pFile = SPLITPATH((char *)&pme->m_ciStart.ph->efs.filename, OEMFS_BREW_DIR);
                  vc_data.play_info.file_scr = VC_IN_BASEBAND_RAM;
              }
              if(!pFile) //for mp3 IRinger PEK, if dir is not fs:/card0 or fs:/brew, she efs.filename to vc_task directly in BASEBAND_RAM mode
              {
                 //pFile = SPLITPATH((char *)&pme->m_ciStart.ph->efs.filename, OEMFS_RINGERS_DIR);
                 pFile = (char *)&pme->m_ciStart.ph->efs.filename;
                 vc_data.play_info.file_scr = VC_IN_BASEBAND_RAM;
              }
              STRCPY((char *)&vc_data.play_info.szFileName, pFile);
              vc_data.play_info.pFileaddr =NULL;	 		  
         }

         VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);
	  if(VC_GetDeviceState() != VC_STAT_DONE)
           {
               return EFAILED;
           }
      }
#endif // !defined (FEATURE_CMX_MM_API)
	    break;

      case OEMCMX_TYPE_AUDFMT_RINGER:
#if defined (FEATURE_CMX_MM_API)
         OEMMedia_StartPlaySession(pme);
#else // !defined (FEATURE_CMX_MM_API)
         //cmx_audfmt_ringer(pme->m_ciStart.ph, (uint16)pme->m_dwPauseMS, (cmx_audfmt_start_enum_type)pme->m_eType,
         //                    (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB,
         //                    pUser);
#endif   // !defined (FEATURE_CMX_MM_API)
         break;

#if defined(FEATURE_QCP)
      case OEMCMX_TYPE_QCP_RECORD:
         if (pme->m_bReverse)
         {
#if defined(FEATURE_BREW_CMX_V3)
            cmx_qcp_record_reverse(pme->m_ciStart.ph, pme->m_pQCPRecParm,
                                   (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB,
                                   pUser);
#else
            cmx_qcp_record_reverse(pme->m_ciStart.ph, pme->m_pQCPRecParm->format, pme->m_pQCPRecParm->report_ms,
                                   (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB,
                                   pUser);
#endif // defined(FEATURE_BREW_CMX_V3)
         }
         else
         {
#if defined(FEATURE_BREW_CMX_V3)
            cmx_qcp_record_forward(pme->m_ciStart.ph, pme->m_pQCPRecParm,
                                   (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB,
                                   pUser);
#else
            cmx_qcp_record_forward(pme->m_ciStart.ph, pme->m_pQCPRecParm->format, pme->m_pQCPRecParm->report_ms,
                                   (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB,
                                   pUser);
#endif // defined(FEATURE_BREW_CMX_V3)
         }
         break;
#endif // defined(FEATURE_QCP)

#if defined(FEATURE_MIDI_OUT)
      case OEMCMX_TYPE_MIDI_OUT_MSG:
         cmx_midi_out_msg(pme->m_ciStart.ph,
                          (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB,
                          pUser);
         break;

#if defined(FEATURE_MIDI_OUT_QCP)
      case OEMCMX_TYPE_MIDI_OUT_QCP:
#if defined (FEATURE_QAUDIOFX)
#if defined (FEATURE_CMX_MM_API)
            OEMMedia_StartPlaySession(pme);
#else // !defined (FEATURE_CMX_MM_API)
         {
            cmx_midi_out_adec_param_type param;

            param.volume      = pme->m_wVolume;
            param.pan         = (sint15)(pme->m_wPan);
            param.start_mode  = CMX_ADEC_START_MODE_DELAYED;
            param.repeat_mode = (1 == pme->m_dwRepeatCount) ? CMX_ADEC_REPEAT_MODE_DISABLED : CMX_ADEC_REPEAT_MODE_ENABLED;
            param.repeat_num  = pme->m_dwRepeatCount;

            cmx_midi_out_adec(pme->m_ciStart.ph, &param,
                              (cmx_midi_out_cb_func_ptr_type)pme->m_ciStart.dwCB,
                              pUser);
         }
#endif //  !defined (FEATURE_CMX_MM_API)
#else // !defined (FEATURE_QAUDIOFX)
         cmx_midi_out_qcp(pme->m_ciStart.ph, pme->m_wVolume, (sint15)(pme->m_wPan),
                          (cmx_midi_out_cb_func_ptr_type)pme->m_ciStart.dwCB,
                          pUser);
#endif // !defined (FEATURE_QAUDIOFX)
         break;
#endif // defined(FEATURE_MIDI_OUT_QCP)
#endif // defined(FEATURE_MIDI_OUT)

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_play_multi(pme->m_ciStart.ph, pme->m_pMultiParam,
                               (cmx_audfmt_play_cb_func_ptr_type)pme->m_ciStart.dwCB,
                               pUser);
         break;
#endif //FEATURE MULTISEQUENCER

#if defined(FEATURE_PCM_REC)
      case OEMCMX_TYPE_PCM_RECORD:
            cmx_pcm_record(pme->m_ciStart.ph, pme->m_pPCMRecParm,
                           (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB,
                            pUser);
         break;
#endif // defined(FEATURE_PCM_REC)

#if defined(FEATURE_AAC_REC)
      case OEMCMX_TYPE_AAC_RECORD:
/*            memcpy(pme->m_ciStart.ph->efs.filename,aac_rec_file[pme->m_pAACRecParm->mm_rec_param.cmx_mm_rec_aac_para.chan]
                     [pme->m_pAACRecParm->mm_rec_param.cmx_mm_rec_aac_para.sample_rate],
                  sizeof(pme->m_ciStart.ph->efs.filename));*/
            cmx_mm_record(pme->m_ciStart.ph, CMX_MM_REC_OPERATION_NORMAL, pme->m_pAACRecParm,
                           (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB,
                           pUser);
 		  break;
#endif // defined(FEATURE_AAC_REC)

#if defined(FEATURE_CMX_RAW_PLAYBACK)
      case OEMCMX_TYPE_AUDFMT_PLAY_CODEC:
      {
         if (!pme->m_dwAudioSync)
         {
#if defined (FEATURE_CMX_MM_API)
            OEMMedia_StartPlaySession(pme);
#else // !defined (FEATURE_CMX_MM_API)
            cmx_audfmt_play_codec(pme->m_ciStart.ph, pme->m_pCodecSpec,
                                 (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB,
                                 pUser);
#endif // !defined (FEATURE_CMX_MM_API)
         }
         else
         {
#if defined(FEATURE_CMX_AV_SYNC)
            cmx_audfmt_play_codec_av_sync(pme->m_ciStart.ph, pme->m_pCodecSpec,
                                          (cmx_av_sync_type *)pme->m_dwAudioSync,
                                          (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB,
                                          pUser);
#else // defined(FEATURE_CMX_AV_SYNC)
            return EFAILED;
#endif // defined(FEATURE_CMX_AV_SYNC)

         }

      }

      break;
#endif //defined(FEATURE_CMX_RAW_PLAYBACK)

      default:
         return EFAILED;
   }

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMedia_Stop(OEMMedia * pme)
{
   cmx_cb_func_ptr_type pfn = (cmx_cb_func_ptr_type)pme->m_ciStatus.dwCB;
   void *               pUser = (void *)pme->m_ciStatus.dwUserData;
   
   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY: // Fall thru...
#if 0      
       MMD_Player_File_Stop( MediaFile.pPlayFileHandle);
       MMD_Player_File_Close( MediaFile.pPlayFileHandle,NULL,NULL);
       MMD_SYS_PMU_SWITCH(PMU_MODE_BYPASS);

       VC_GPIO_CTRL(46, LO); // disable Speaker PA   //VC_GPIO_CTRL(49, LO); //关耳机
       VC_GPIO_CTRL(44, LO); // disable Speaker_Sel
#endif // if 0
      {
         vc_union_type vc_data;
         vc_data.play_info.play_mode = VC_STOP_M;
         VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);
      }
        break;
      case OEMCMX_TYPE_AUDFMT_RINGER: // Fall thru...
      case OEMCMX_TYPE_AUDFMT_PLAY_CODEC:
#if defined (FEATURE_CMX_MM_API)
         OEMMedia_SessionCmd(pme, CMX_MM_PLAY_CONTROL_CMD_STOP, 0);
#else // !define (FEATURE_CMX_MM_API)
         cmx_audfmt_stop(pfn, pUser);
#endif // !defined (FEATURE_CMX_MM_API)
         break;

#if defined(FEATURE_QCP)
      case OEMCMX_TYPE_QCP_RECORD:
         cmx_qcp_record_stop(pfn, pUser);
         break;
#endif // defined(FEATURE_QCP)

      case OEMCMX_TYPE_MIDI_OUT_MSG:
         // No effect.
         break;

#if defined(FEATURE_MIDI_OUT_QCP)
      case OEMCMX_TYPE_MIDI_OUT_QCP:
#if defined (FEATURE_CMX_MM_API)
         OEMMedia_SessionCmd(pme, CMX_MM_PLAY_CONTROL_CMD_STOP, 0);
#else // !defined (FEATURE_CMX_MM_API)
         cmx_midi_out_qcp_stop((void *)pme->m_dwChannel, pfn, pUser);
#endif // !defined (FEATURE_CMX_MM_API);
         break;
#endif // defined(FEATURE_MIDI_OUT_QCP)

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam,
                                 pfn, pUser);
         break;
#endif // FEATURE_MULTISEQUENCER

#ifdef FEATURE_PCM_REC
      case OEMCMX_TYPE_PCM_RECORD:
         cmx_pcm_record_stop(pme->m_Link, pfn, pUser);
         break;
#endif // FEATURE_PCM_REC

#ifdef FEATURE_AAC_REC
      case OEMCMX_TYPE_AAC_RECORD:
         cmx_mm_record_stop(CMX_MM_REC_LINK_REVERSE, pfn, pUser);
         break;
#endif // FEATURE_AAC_REC

      default:
         return EFAILED;
   }

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMedia_PauseResume(OEMMedia * pme, boolean bPause)
{
   cmx_cb_func_ptr_type pfn = (cmx_cb_func_ptr_type)pme->m_ciStatus.dwCB;
   void *               pUser = (void *)pme->m_ciStatus.dwUserData;

   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY: // Fall thru...
       if (bPause)
       {
#if 0         
            //            cmx_audfmt_pause(pfn, pUser);

            nRet = MMD_Player_File_Pause( MediaFile.pPlayFileHandle);
            if(nRet)
            {
               nRet = MMD_Player_File_Close( MediaFile.pPlayFileHandle,NULL,NULL);
               return nRet;
            }
#endif // if 0    
            vc_union_type vc_data;
            vc_data.play_info.play_mode = VC_PAUSE_M;
            VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);
            
        }
        else
        {
#if 0         
            //            cmx_audfmt_resume(pfn, pUser);
            nRet = MMD_Player_File_Resume( MediaFile.pPlayFileHandle);
            if(nRet)
            {
               nRet = MMD_Player_File_Close( MediaFile.pPlayFileHandle,NULL,NULL);
               return nRet;
            }
#endif // if 0 debug for mp3 andmp4 play func
            vc_union_type vc_data;
            vc_data.play_info.play_mode = VC_RESUME_M;
            VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);
        }
        break;
      case OEMCMX_TYPE_AUDFMT_RINGER:
      case OEMCMX_TYPE_AUDFMT_PLAY_CODEC:
#if defined (FEATURE_CMX_MM_API)
         OEMMedia_SessionCmd(pme, bPause ? CMX_MM_PLAY_CONTROL_CMD_PAUSE :
                                           CMX_MM_PLAY_CONTROL_CMD_RESUME, 0);
#else // !defined (FEATURE_CMX_MM_API)
         if (bPause)
            cmx_audfmt_pause(pfn, pUser);
#if 0 // debug for mp3 and mp4 play func         
         nRet = MMD_Player_File_Pause( MediaFile.pPlayFileHandle);
         if(nRet)
         {
            nRet = MMD_Player_File_Close( MediaFile.pPlayFileHandle,NULL,NULL);
            return nRet;
         }
               else
         //            cmx_audfmt_resume(pfn, pUser);
         nRet = MMD_Player_File_Resume( MediaFile.pPlayFileHandle);
         if(nRet)
         {
            nRet = MMD_Player_File_Close( MediaFile.pPlayFileHandle,NULL,NULL);
            return nRet;
         }
#endif // if 0 debug for mp3 and mp4 play func

#endif // !defined (FEATURE_CMX_MM_API)
         break;

#if defined(FEATURE_QCP)
      case OEMCMX_TYPE_QCP_RECORD:
         if (bPause)
            cmx_qcp_record_pause(pfn, pUser);
         else
            cmx_qcp_record_resume(pfn, pUser);
         break;
#endif // defined(FEATURE_QCP)

      case OEMCMX_TYPE_MIDI_OUT_MSG:
         // No effect.
         break;

#if defined(FEATURE_MIDI_OUT_QCP)
      case OEMCMX_TYPE_MIDI_OUT_QCP:
#if defined (FEATURE_CMX_MM_API)
         OEMMedia_SessionCmd(pme, bPause ? CMX_MM_PLAY_CONTROL_CMD_PAUSE :
                                           CMX_MM_PLAY_CONTROL_CMD_RESUME, 0);
#else // !defined (FEATURE_CMX_MM_API)
         if (bPause)
            cmx_midi_out_qcp_pause(pfn, pUser);
         else
            cmx_midi_out_qcp_resume(pfn, pUser);
#endif // !defined (FEATURE_CMX_MM_API)
         break;
#endif // defined(FEATURE_MIDI_OUT_QCP)

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam,
                                 pfn, pUser);
#if 0 // debug for mp3 play         
         if (bPause)
         nRet = MMD_Player_File_Pause( MediaFile.pPlayFileHandle);
         if(nRet)
         {
         nRet = MMD_Player_File_Close( MediaFile.pPlayFileHandle,NULL,NULL);
         return nRet;
         }
            else
         nRet = MMD_Player_File_Resume( MediaFile.pPlayFileHandle);
         if(nRet)
         {
         nRet = MMD_Player_File_Close( MediaFile.pPlayFileHandle,NULL,NULL);
         return nRet;
         }
#endif // if 0 debug for mp3 play         
         break;

#endif // FEATURE_MULTISEQUENCER

#ifdef FEATURE_PCM_REC
      case OEMCMX_TYPE_PCM_RECORD:
         if (bPause)
            cmx_pcm_record_pause(pme->m_Link, pfn, pUser);
         else
            cmx_pcm_record_resume(pme->m_Link, pfn, pUser);
         break;
#endif // FEATURE_PCM_REC

#ifdef FEATURE_AAC_REC
      case OEMCMX_TYPE_AAC_RECORD:
         if (bPause)
            cmx_mm_record_pause(CMX_MM_REC_LINK_REVERSE, pfn, pUser);
         else
            cmx_mm_record_resume(CMX_MM_REC_LINK_REVERSE, pfn, pUser);
         break;
#endif // FEATURE_AAC_REC

      default:
         return EFAILED;
   }

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMedia_Seek(OEMMedia * pme, AEEMediaSeek eSeek, int32 lTimeMS)
{
   //void *               pUser = (void *)pme->m_ciStatus.dwUserData;
   MMD_U32    curTime,totTime;
   vc_union_type vc_data;
   int nRet;

#if 0
   if (eSeek != MM_SEEK_CURRENT)
      return EUNSUPPORTED;
#endif
   if (VC_GetPlayMode() == VC_NONE_M)                  /*MP3 功能加入SEEK_START*/
   	{
          MMD_Player_File_GetTotalTime(pFileHandle,&totTime);
          if(lTimeMS >(int32)totTime)
           {
              lTimeMS = totTime;
	    }
         vc_data.parm.p1 = lTimeMS;
         vc_data.play_info.play_mode = VC_SEEK_M;
         nRet = VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);

	  if(nRet != VC_STAT_DONE)
          {
             return EFAILED;
          }
	  
	   return SUCCESS;
   	}
   if (eSeek == MM_SEEK_CURRENT)
   	{
   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY: // Fall thru...
      case OEMCMX_TYPE_AUDFMT_RINGER:
#if defined (FEATURE_CMX_MM_API)
         OEMMedia_SessionCmd(pme, lTimeMS < 0 ? CMX_MM_PLAY_CONTROL_CMD_REWIND :
                                                CMX_MM_PLAY_CONTROL_CMD_FAST_FORWARD, (uint32)lTimeMS);
#else // !defined (FEATURE_CMX_MM_API)
         if (lTimeMS < 0)
             //cmx_audfmt_rewind(-1*lTimeMS, pfn, pUser);
          {
            if(VC_GetPlayMode() == VC_NONE_M)
            {
                return FAILED;
            }
           else
            {
              MMD_Player_File_GetPlayTime(pFileHandle,&curTime);
              if(curTime >( -lTimeMS))
               {
                 curTime += lTimeMS;
               }
              else
               {
                  curTime = 0;
               }
              vc_data.parm.p1 = curTime;
              vc_data.play_info.play_mode = VC_SEEK_M;
              nRet = VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);
			  
		if(nRet != VC_STAT_DONE)
               {
                  return EFAILED;
               }
            }
          }
         else
         //cmx_audfmt_fforward(lTimeMS, pfn, pUser);
          {
            if(VC_GetPlayMode() == VC_NONE_M)
            {
                return FAILED;
            }
            else
             {
                MMD_Player_File_GetPlayTime(pFileHandle,&curTime);
                MMD_Player_File_GetTotalTime(pFileHandle,&totTime);
                curTime += lTimeMS;
                if(curTime >totTime)
                	{
                    curTime = totTime;
                	}
   		
                vc_data.parm.p1 = curTime;
                vc_data.play_info.play_mode = VC_SEEK_M;
                nRet = VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);

		  if(nRet != VC_STAT_DONE)
                 {
                    return EFAILED;
                 }
             }
          }
                   
#endif // !defined (FEATURE_CMX_MM_API)
         break;

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam,
                                 pfn, pUser);
         break;
#endif // FEATURE_MULTISEQUENCER

      default:
         return EFAILED;
                 }
   }

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMedia_SetParm(OEMMedia * pme, int16 nParmID, int32 p1, int32 p2, int32 p3, boolean bLocal)
{
   cmx_cb_func_ptr_type pfn;
   void *               pUser;
   int                  nErr = SUCCESS;
   AEEMediaCallback *   pcb = (AEEMediaCallback *)pMediaCB;

   // Process MIDI_DEVICE open/close which may not require object instance.
#if defined(FEATURE_MIDI_OUT)
   if (OEMCMX_ID_MIDI_DEVICE == nParmID)
   {
      boolean     bOpen = (boolean)p1;
      int         eADEC = (int)p2;
      CMXInfo *   pci = (CMXInfo *)p3;

      if (bLocal)
         return SUCCESS;

      pfn = pci ? (cmx_cb_func_ptr_type)pci->dwCB : NULL;
      pUser = pci ? (void *)pci->dwUserData : NULL;

      if (bOpen)
#if defined(FEATURE_BREW_CMX_V3)
         cmx_midi_out_open_adec_format((cmx_adec_mode_enum_type)eADEC, pfn, pUser);
#else
         cmx_midi_out_open(pfn, pUser);
#endif // defined(FEATURE_BREW_CMX_V3)
      else
         cmx_midi_out_close(pfn, pUser);

      return SUCCESS;
   }
#endif // defined(FEATURE_MIDI_OUT_QCP)

   if (!pme)
      return SUCCESS;

   pfn = (cmx_cb_func_ptr_type)pme->m_ciStatus.dwCB;
   pUser = (void *)pme->m_ciStatus.dwUserData;

   switch (nParmID)
   {
      case OEMCMX_ID_TYPE:
         if (!bLocal)
            break;

         if (p1)
            MEMCPY(&pme->m_ciStart, (CMXInfo *)p1, sizeof(CMXInfo));

         if (OEMCMX_TYPE_AUDFMT_PLAY == pme->m_ciStart.nType)
               pme->m_eType = (int16)p2;
         else if (OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
         {
            pme->m_eType = (int16)p2;
            pme->m_dwPauseMS = (uint32)p3;
         }
#if defined(FEATURE_QCP)
         else if (OEMCMX_TYPE_QCP_RECORD == pme->m_ciStart.nType)
         {
            pme->m_pQCPRecParm = (cmx_qcp_rec_para_type *)p2;
            pme->m_bReverse = (boolean)p3;
         }
#endif // defined(FEATURE_QCP)

#ifdef FEATURE_MULTISEQUENCER
         else if (OEMCMX_TYPE_AUDFMT_PLAY_MULTI == pme->m_ciStart.nType)
         {
            pme->m_pMultiParam = (cmx_audfmt_multi_param_type *)p2;
         }
#endif // FEATURE_MULTISEQUENCER

#ifdef FEATURE_PCM_REC
         else if (OEMCMX_TYPE_PCM_RECORD == pme->m_ciStart.nType)
         {
            pme->m_pPCMRecParm = (cmx_pcm_rec_para_type *)p2;
         }
#endif // FEATURE_PCM_REC

#ifdef FEATURE_AAC_REC
         else if (OEMCMX_TYPE_AAC_RECORD == pme->m_ciStart.nType)
         {
			pme->m_pAACRecParm= (cmx_mm_rec_param_type*)p2;
            pme->m_bReverse = (boolean)p3;
         }
#endif // FEATURE_AAC_REC
#if defined(FEATURE_CMX_RAW_PLAYBACK)
         else if (OEMCMX_TYPE_AUDFMT_PLAY_CODEC == pme->m_ciStart.nType)
         {
            pme->m_pCodecSpec = (cmx_codec_spec_type *)p2;
            pme->m_dwAudioSync = (uint32)p3;
         }
#endif // FEATURE_CMX_RAW_PLAYBACK

#ifdef FEATURE_MULTISEQUENCER
         else if (OEMCMX_TYPE_AUDFMT_PLAY_MULTI == pme->m_ciStart.nType)
         {
            pme->m_pMultiParam = (cmx_audfmt_multi_param_type *)p2;
         }
#endif // FEATURE_MULTISEQUENCER

#ifdef FEATURE_PCM_REC
         else if (OEMCMX_TYPE_PCM_RECORD == pme->m_ciStart.nType)
         {
            pme->m_pPCMRecParm = (cmx_pcm_rec_para_type *)p2;
         }
#endif // FEATURE_PCM_REC

         else
            nErr = EBADPARM;
         break;

      case OEMCMX_ID_STATUS_CB:
         if (!bLocal)
            break;
         if (p1)
            MEMCPY(&pme->m_ciStatus, (CMXInfo *)p1, sizeof(CMXInfo));
         break;


      case OEMCMX_ID_TOTALTIME:
      {
         CMXInfo *   pci = (CMXInfo *)p1;
         char * pFile = NULL;
         static vc_union_type vc_data;

         if (bLocal)
            break;

         if (pci && OEMCMX_TYPE_AUDFMT_PLAY == pci->nType)
         {
#if defined (FEATURE_CMX_MM_API)
            cmx_mm_play_get_param_type param;

            param.total_time.get_cmd = CMX_MM_PLAY_GET_TOTAL_TIME;
            OEMMedia_GetSessionParam(pme, &param, (void *)pci->dwCB, (void *)pci->dwUserData);
#else // !defined (FEATURE_CMX_MM_API)
            vc_data.play_info.play_mode = VC_PLAY_M;
            pFile = SPLITPATH((char *)&pci->ph->efs.filename, OEMFS_CARD0_DIR);
            vc_data.play_info.file_scr = VC_IN_CORE_FILE;
            if(!pFile)
            {
                pFile = SPLITPATH((char *)&pci->ph->efs.filename, OEMFS_BREW_DIR);
                vc_data.play_info.file_scr = VC_IN_BASEBAND_RAM;
            }
            STRCPY((char *)&vc_data.play_info.szFileName, pFile);

            VC_DeviceControl(VC_ITM_GET_PLAY_TOTALTIME_I, VC_FUNC_GET_PARM, &vc_data);
            pcb->cmdNotify.pCmdData = (void*)vc_data.parm.p1;
            pcb->cmdNotify.nStatus = MM_STATUS_DONE;
            pcb->cmdNotify.nCmd = MM_CMD_GETTOTALTIME;
	     pcb->cmdNotify.pCmdData = (void*)vc_data.parm.p1;
            CMediaBg_CallbackNotify(pMediaCB);
	    #if 0
            cmx_audfmt_get_time(pci->ph,
                                (cmx_audfmt_time_cb_func_ptr_type)pci->dwCB,
                                (void *)pci->dwUserData);
	   #endif
#endif // defined (FEATURE_CMX_MM_API)
         }
         else
            nErr = EBADPARM;

         break;
      }

      case OEMCMX_ID_VOLUME:
         pme->m_wVolume = (uint16)p1;
         if (bLocal)
            break;

#if defined(FEATURE_MIDI_OUT_QCP)
         if (OEMCMX_TYPE_MIDI_OUT_QCP == pme->m_ciStart.nType)
            cmx_midi_out_qcp_volume((void *)pme->m_dwChannel, pme->m_wVolume, pfn, pUser);
         else
#endif // defined(FEATURE_MIDI_OUT_QCP)
            nErr = EUNSUPPORTED;
         break;

      case OEMCMX_ID_PAN:
         pme->m_wPan = (uint16)p1;
         if (bLocal)
            break;

         if (OEMCMX_TYPE_AUDFMT_PLAY == pme->m_ciStart.nType || OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
            cmx_audfmt_pan((uint32)pme->m_wPan, pfn, pUser);
#if defined(FEATURE_MIDI_OUT_QCP)
         else if (OEMCMX_TYPE_MIDI_OUT_QCP == pme->m_ciStart.nType)
            cmx_midi_out_qcp_pan((void *)pme->m_dwChannel, (sint15)pme->m_wPan, pfn, pUser);
#endif // defined(FEATURE_MIDI_OUT_QCP)
         else
            nErr = EUNSUPPORTED;
         break;

      case OEMCMX_ID_TEMPO:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_AUDFMT_PLAY == pme->m_ciStart.nType || OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
#if defined (FEATURE_CMX_MM_API)
            OEMMedia_SessionCmd(pme, CMX_MM_PLAY_CONTROL_CMD_TEMPO, (int32)p1);
#else // !defined (FEATURE_CMX_MM_API)
            cmx_audfmt_tempo((uint32)p1, pfn, pUser);
#endif // !defined (FEATURE_CMX_MM_API)
         else
            nErr = EUNSUPPORTED;
         break;

      case OEMCMX_ID_TUNE:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_AUDFMT_PLAY == pme->m_ciStart.nType || OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
            cmx_audfmt_tune((uint32)p1, pfn, pUser);
         else
            nErr = EUNSUPPORTED;
         break;

      case OEMCMX_ID_CHANNEL:
         if (!bLocal)
            break;

         pme->m_dwChannel = (uint32)p1;
         break;

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_ID_SEQUENCE:
         pme->m_pSequence = (void*)p1;
         pme->m_pSeqParam = (cmx_audfmt_seq_param_type *)p2;
         if (bLocal)
            break;

         cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam,
                                 pfn, pUser);
         break;
#endif // FEATURE_MULTISEQUENCER

#ifdef FEATURE_PCM_REC
      case OEMCMX_ID_LINK:
         pme->m_Link = (cmx_pcm_rec_link_type)p1;
	 break;
#endif // FEATURE_PCM_REC
      case OEMCMX_ID_DATA_RESPONSE:
         if (bLocal)
            break;
         else
         {
            CMXDataRequest *  pReq = (CMXDataRequest *)p1;
            uint8 *           pBuffer = (uint8 *)p2;
            uint32            dwBufSize = (uint32)p3;

            pReq->pfnDataReq(CMX_SUCCESS, pBuffer, dwBufSize, pReq->pServerUserData);
         }

         break;
      default:
         nErr = EFAILED;
   }

   return nErr;
}

/*==================================================================
   Constructor
==================================================================*/
static IBase * CMediaCMX_New(IShell * ps, AEECLSID cls, const void * pVtbl, uint32 dwSize, PFNNOTIFY pfnDelete)
{
   IMedia *       po;
   CMediaCMX *    pme;

   // Alloc memory for the object
   po = (IMedia *)AEE_NewClass((IBaseVtbl *)pVtbl, dwSize);
   if (!po)
      return NULL;

   pme = (CMediaCMX *)po;

   // Call base class constructor
   if (SUCCESS != AEEMedia_New(po, ps, cls))
      goto Done;

   if (SUCCESS != OEMMedia_New(cls, &pme->m_pMediaCMX))
      goto Done;

   // Initialize MM Layer
   CMediaMMLayer_Init(ps);

   pme->m_pfnDelete = pfnDelete;

   // Register the object with ObjectMgr...
   if (AEEObjectMgr_Register(pme, &pme->m_hObject))
      goto Done;

   pme->m_dwCaps |= MM_CAPS_AUDIO;
   pme->m_dwEnabledCaps = pme->m_dwCaps;

   pme->m_wVolume = (3 * AEE_MAX_VOLUME) / 4;
   pme->m_nAPath = MM_APATH_LOCAL;
   pme->m_wPan = 64;
   pme->m_bVolumeScaled = FALSE;
   pme->m_dwRepeatCount = 1;

   return (IBase *)po;

Done:
   CMediaCMX_Delete(pme);
//   FREE(pme);
   return NULL;
}

/*==================================================================
   Destructor
==================================================================*/
static void CMediaCMX_Delete(CMediaCMX * pme)
{
   if (pme->m_pfnDelete) {
      pme->m_pfnDelete(pme);
      pme->m_pfnDelete = NULL;
   }

   AEEObjectMgr_Unregister(pme->m_hObject);

   // Call base class destructor
   AEEMedia_Delete((IMedia *)pme);

   OEMMedia_Delete(pme->m_pMediaCMX);

   FREE(pme);
}

/*==================================================================

==================================================================*/
static uint32 CMediaCMX_Release(IMedia * po)
{
   int32     nRef = ((CMediaCMX *)po)->m_nRefs;
   AEEMedia * pme = (AEEMedia*) po;

   if (nRef)
   {
      nRef = (int32)AEEMedia_Release(po);

      if (!nRef)
      {
         CMediaCMX_Delete((CMediaCMX *)po);
         // while release mpeg4 interface, stop the dvice here and enter BYPASS mode 
         {
           vc_union_type vc_data;
	    switch(OEMMediaCLSID)
	    {
	    	  case AEECLSID_MEDIAMP3:
			 vc_data.dev_run.curr_dev = VC_DEV_PLY_MP3;
			 break;
	    	  case AEECLSID_MEDIAAAC:
			 vc_data.dev_run.curr_dev = VC_DEV_PLY_AAC;
			 break;		  	 
		  case AEECLSID_MEDIAAMR:
		  	 vc_data.dev_run.curr_dev = VC_DEV_PLY_AMRAUDIO;
			 break;
	    }
           VC_DeviceControl(VC_ITM_DEV_STOP_I, VC_FUNC_CONTROL, &vc_data);
         }
         oem_musicplayer_status = 0;
      }
   }
   
   return nRef;
}

/*==================================================================

==================================================================*/
static int CMediaCMX_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   CMediaCMX * pme = (CMediaCMX *)po;
   int         nRet = SUCCESS;


   if (nParmID == MM_PARM_MEDIA_DATA)
      return AEEMedia_SetMediaParm(po, nParmID, p1, p2);

   // No need to alloc a command callback for ISound operations since the callback
   // from ISound uniquely identifies this operation and also
   // gets called in BREW context.
   // These are MM_PARM_VOLUME, MM_PARM_PATH, MM_PARM_MUTE.

   switch (nParmID)
   {
      case MM_PARM_ENABLE:
         if (p1)
         {
            uint32   dw = pme->m_dwCaps & p1;
            if (dw)
               pme->m_dwEnabledCaps = dw;
            else
               nRet = EBADPARM;
         }
         break;

      case MM_PARM_PLAY_REPEAT:
#if defined (FEATURE_CMX_MM_API)
         pme->m_dwRepeatCount = (uint32)p1;
#else
         {
#if defined(FEATURE_MULTISEQUENCER) || defined(FEATURE_MIDI_OUT_QCP)
            boolean multi = FALSE;
#endif
            if (
#if defined(FEATURE_MIDI_OUT_QCP)
                (AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf) ||
                (AEECLSID_MEDIAADPCM == pme->m_clsSelf) ||
                (AEECLSID_MEDIAPCM == pme->m_clsSelf) ||
                (AEECLSID_MEDIAQCP == pme->m_clsSelf) ||
#endif // defined(FEATURE_MIDI_OUT_QCP)
#if defined(FEATURE_MULTISEQUENCER)
                (multi = CMediaMMLayer_IsMultiSequence((CMediaBg *)pme)) ||
#endif // defined(FEATURE_MULTISEQUENCER)
                FALSE)
            {
               pme->m_dwRepeatCount = (uint32)p1;
#if defined(FEATURE_MIDI_OUT_QCP)
               if (!multi)
               {
                  OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_REPEAT, (int32)pme->m_dwRepeatCount, 0, 0, TRUE);
               }
#endif // defined(FEATURE_MIDI_OUT_QCP)
            }
            else
            {
               nRet = EUNSUPPORTED;
            }
         }
#endif // defined (FEATURE_CMX_MM_API)
         break;

      default:
         nRet = EUNSUPPORTED;
         break;
   }

   return nRet;
}

/*==================================================================

==================================================================*/
static int CMediaCMX_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2)
{
   CMediaCMX * pme = (CMediaCMX *)po;
   int         nRet = SUCCESS;

   if (nParmID == MM_PARM_MEDIA_DATA || nParmID == MM_PARM_CLSID)
      return AEEMedia_GetMediaParm(po, nParmID, pP1, pP2);

   switch (nParmID)
   {
      case MM_PARM_CAPS:
         if (pP1)
            *pP1 = (int32)pme->m_dwCaps;
         if (pP2)
            *pP2 = 0;
         break;

      case MM_PARM_ENABLE:
         if (pP1)
            *pP1 = (int32)pme->m_dwEnabledCaps;
         if (pP2)
            *pP2 = 0;
         break;

      case MM_PARM_CHANNEL_SHARE:
         if (pP1)
            *(boolean *)pP1 = FALSE;
         break;

      case MM_PARM_PLAY_REPEAT:
#if defined (FEATURE_CMX_MM_API)
         *pP1 = (int32)pme->m_dwRepeatCount;
#else
         if (
#if defined(FEATURE_MIDI_OUT_QCP)
            (AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf) ||
             (AEECLSID_MEDIAADPCM == pme->m_clsSelf) ||
             (AEECLSID_MEDIAPCM == pme->m_clsSelf) ||
             (AEECLSID_MEDIAQCP == pme->m_clsSelf) ||
#endif // defined(FEATURE_MIDI_OUT_QCP)
#if defined(FEATURE_MULTISEQUENCER)
             CMediaMMLayer_IsMultiSequence((CMediaBg *)pme) ||
#endif //defined(FEATURE_MULTISEQUENCER)
            FALSE )
         {
            *pP1 = (int32)pme->m_dwRepeatCount;
         }
         else
         {
            nRet = EUNSUPPORTED;
         }
#endif // defined (FEATURE_CMX_MM_API)
         break;

      default:
         nRet = EUNSUPPORTED;
         break;
   }

   return nRet;
}

/*==================================================================
   Unsupported functions -- used in vtbl of IMedia classes
==================================================================*/
static int CMediaCMX_RecordUnsupported(IMedia * po)
{
   return EUNSUPPORTED;
}

static int CMediaCMX_StopUnsupported(IMedia * po)
{
   return EUNSUPPORTED;
}

static int CMediaCMX_SeekUnsupported(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   return EUNSUPPORTED;
}

static int CMediaCMX_PauseUnsupported(IMedia * po)
{
   return EUNSUPPORTED;
}

static int CMediaCMX_ResumeUnsupported(IMedia * po)
{
   return EUNSUPPORTED;
}

static int CMediaCMX_GetTotalTimeUnsupported(IMedia * po)
{
   return EUNSUPPORTED;
}

/*==================================================================

   Callback Notification Functions: called in BREW context after
   AEE_ResumeCallback()

==================================================================*/
/*==================================================================
   Media callback notify -- called in BREW context.
==================================================================*/
void CMediaCMX_CallbackNotify(void * pUser)
{
   AEEMediaCallback *   pcb = (AEEMediaCallback *)pUser;
   CMediaCMX *          pme;
   AEEMediaCallback     mcb;

   if (!pcb)
      return;

   MEMCPY(&mcb, pcb, sizeof(AEEMediaCallback));
   CMediaMMLayer_FreeCallback(pcb);

   pme = (CMediaCMX *)mcb.cmdNotify.pIMedia;
   if (!pme)
      return;

   AEEMedia_CallbackNotify((AEEMedia *)pme, &mcb);
}

/*===============================================================================

                     CMediaBg PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==================================================================
   Called only once during AEE initialization
==================================================================*/
void IMediaVC0848Bg_Init(IShell * ps)
{
   if (gInitFlags.gbMediaBgInit)
      return;

   gInitFlags.gbMediaBgInit = TRUE;

   // If found in the mod table, then register the formats...

   if (CMediaFormat_InModTable(AEECLSID_MEDIAAMR, (void *)IMediaVC0848Bg_Init, NULL))
      AEEMedia_Init(ps, MT_AUDIO_AMR, AEECLSID_MEDIAAMR);

   if (CMediaFormat_InModTable(AEECLSID_MEDIAMP3, (void *)IMediaVC0848Bg_Init, NULL))
      AEEMedia_Init(ps, MT_AUDIO_MP3, AEECLSID_MEDIAMP3);

   if (CMediaFormat_InModTable(AEECLSID_MEDIAAAC, (void *)IMediaVC0848Bg_Init, NULL))
   {
      AEEMedia_Init(ps, MT_AUDIO_AAC, AEECLSID_MEDIAAAC);
      AEEMedia_Init(ps, "audio/apl", AEECLSID_MEDIAAAC);
   }
   
#ifndef FEATURE_SUPPORT_VC0848 
   if (CMediaFormat_InModTable(AEECLSID_MEDIAMIDI, (void *)IMediaBg_Init, NULL))
   {
      AEEMedia_Init(ps, MT_AUDIO_MIDI, AEECLSID_MEDIAMIDI);
      AEEMedia_Init(ps, "video/mld", AEECLSID_MEDIAMIDI);
   }
   if (CMediaFormat_InModTable(AEECLSID_MEDIAIMELODY, (void *)IMediaBg_Init, (void *)NULL))
         AEEMedia_Init(ps, MT_AUDIO_IMELODY, AEECLSID_MEDIAIMELODY);

   if (CMediaFormat_InModTable(AEECLSID_MEDIAAMRWB, (void *)IMediaVC0848Bg_Init, (void *)NULL))
         AEEMedia_Init(ps, "audio/awb", AEECLSID_MEDIAAMRWB);

   if (CMediaFormat_InModTable(AEECLSID_MEDIAMMF, (void *)IMediaBg_Init, (void *)NULL))
         AEEMedia_Init(ps, MT_AUDIO_MMF, AEECLSID_MEDIAMMF);

   if (CMediaFormat_InModTable(AEECLSID_MEDIAPHR, (void *)IMediaBg_Init, (void *)NULL))
         AEEMedia_Init(ps, MT_AUDIO_PHR, AEECLSID_MEDIAPHR);

   if (CMediaFormat_InModTable(AEECLSID_MEDIAXMF, (void *)IMediaBg_Init, (void *)NULL))
   {
         AEEMedia_Init(ps, MT_AUDIO_XMF, AEECLSID_MEDIAXMF);
         AEEMedia_Init(ps, MT_AUDIO_MXMF, AEECLSID_MEDIAXMF);
         AEEMedia_Init(ps, MT_AUDIO_XMF0, AEECLSID_MEDIAXMF);
         AEEMedia_Init(ps, MT_AUDIO_XMF1, AEECLSID_MEDIAXMF);
   }
#endif /* FEATURE_SUPPORT_VC0848 */
//   if (CMediaFormat_InModTable(AEECLSID_MEDIADLS, (void *)IMediaVC0848Bg_Init, (void *)NULL))
//         AEEMedia_Init(ps, MT_AUDIO_DLS, AEECLSID_MEDIADLS);
   // Initialize ASIC MM Layer here...
   CMediaMMLayer_Init(ps);
}

/*==================================================================
   Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaVC0848Bg_New(IShell * ps, AEECLSID cls, void **ppif)
{
   int         nRet;
   CMediaBg *  pme;
    nRet = oem_media_scheduler();
    if(SUCCESS != nRet)
    {
        return nRet;    
    }
    oem_musicplayer_status = 1;

   // Create the object
   if (ppif)
   {
      pme = *ppif = CMediaCMX_New(ps, cls, &gMediaBgFuncs, sizeof(CMediaBg), (PFNNOTIFY)CMediaBg_Delete);
      nRet = pme ? SUCCESS : ENOMEMORY;
   }
   else {
      nRet = EBADPARM;
      pme = 0;
   }

   if (nRet == SUCCESS)
   {
      AEESoundMethod eMethod;
      AEESoundInfo   si;
      // Create ISound, register callback notify, and set method based on media.
      pme->m_pISound = NULL;
      if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_SOUND, (void **)&pme->m_pISound))
         goto Done;
      ISOUND_RegisterNotify(pme->m_pISound, CMediaMMLayer_SoundCB, (void *)pme->m_hObject);
      if (AEECLSID_MEDIAQCP == cls || AEECLSID_MEDIAAMR == cls)
         eMethod = AEE_SOUND_METHOD_VOICE;
      else
         eMethod = AEE_SOUND_METHOD_MIDI;
      ISOUND_Get(pme->m_pISound, &si);
      si.eMethod = eMethod;
      ISOUND_Set(pme->m_pISound, &si);
      OEMMediaCLSID = cls;
      switch (cls)
      {
         case AEECLSID_MEDIAMP3:
            pme->u.m_pMP3 = MALLOC(sizeof(CMediaMP3));
            if (!pme->u.m_pMP3)
               goto Done;
            {
               vc_union_type vc_data;
               vc_data.dev_run.curr_dev = VC_DEV_PLY_MP3; // MP3
               VC_DeviceControl(VC_ITM_DEV_OPEN_I, VC_FUNC_CONTROL, &vc_data); // init hardware for mp3 player
            }
            break;

         case AEECLSID_MEDIAAAC:
            pme->u.m_pMP3 = MALLOC(sizeof(CMediaMP3));
            if (!pme->u.m_pMP3)
               goto Done;
            {
               vc_union_type vc_data;
               vc_data.dev_run.curr_dev = VC_DEV_PLY_AAC; // AAC
               VC_DeviceControl(VC_ITM_DEV_OPEN_I, VC_FUNC_CONTROL, &vc_data); // init hardware for AAC player
            }
            break;

	case AEECLSID_MEDIAAMR:
            pme->u.m_pMP3 = MALLOC(sizeof(CMediaMP3));
            if (!pme->u.m_pMP3)
               goto Done;
            {
               vc_union_type vc_data;
               vc_data.dev_run.curr_dev = VC_DEV_PLY_AMRAUDIO; // AMR
               VC_DeviceControl(VC_ITM_DEV_OPEN_I, VC_FUNC_CONTROL, &vc_data); // init hardware for AMR player
            }
            break;

         case AEECLSID_MEDIAQCP:
            pme->u.m_pQCP = MALLOC(sizeof(CMediaQCP));
            if (!pme->u.m_pQCP)
               goto Done;
            pme->u.m_pQCP->m_nFormat = MM_QCP_FORMAT_FIXED_FULL_13K;
            break;

#ifdef FEATURE_AAC_REC
         case AEECLSID_MEDIAAAC:
            pme->u.m_pAAC = MALLOC(sizeof(CMediaAAC));
            if (!pme->u.m_pAAC)
               goto Done;
			pme->u.m_pAAC->m_nFormat = CMX_AF_AAC_DATA_FORMAT_ADTS;
            break;
#endif // FEATURE_AAC_REC

		 case AEECLSID_MEDIAADPCM:
            pme->u.m_pADPCM = MALLOC(sizeof(CMediaADPCM));
            if (!pme->u.m_pADPCM)
               goto Done;
            break;
      } // switch

      pme->m_wTempo = 100;
      pme->m_wTune = 0x40;
      pme->m_wSilenceTimeMS = OEMMM_RINGER_SILENCE_TIME; // one second wait between rings.
      pme->m_dwTickTimeMS = OEMMM_TICK_PERIOD;

      // Initialize callbacks.
      CMediaMMLayer_InitCallback((IMedia *)pme, MM_CMD_PLAY, pme->m_cbPlay, OEMMM_MAX_CB_PLAY, CMediaBg_CallbackNotify);
      CMediaMMLayer_InitCallback((IMedia *)pme, 0, &pme->m_cbCommand, 1, CMediaBg_CallbackNotify);
      CMediaMMLayer_InitCallback((IMedia *)pme, MM_CMD_GETTOTALTIME, &pme->m_cbGetTotalTime, 1, CMediaBg_CallbackNotify);

      {
         CMXInfo   ci;


         ci.nType = OEMCMX_TYPE_AUDFMT_PLAY;
         ci.ph = NULL;
         ci.dwCB = (uint32)CMediaMMLayer_StatusCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_STATUS_CB, (int32)&ci, 0, 0, TRUE);
      }

   } // if

   return nRet;

Done:
   // Free anything that could have been successfully allocated before "goto Done"
   if (pme->m_pISound)
   {
      ISOUND_RegisterNotify(pme->m_pISound, NULL, (void *)pme->m_hObject);
      ISOUND_Release(pme->m_pISound);
   }

   FREE(pme);
   return ENOMEMORY;
}

/*==================================================================
   Destructor
==================================================================*/
static void CMediaBg_Delete(CMediaBg * pme)
{
   CMediaMMLayer_CancelCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY);
   CMediaMMLayer_CancelCallback(&pme->m_cbCommand, 1);
   CMediaMMLayer_CancelCallback(&pme->m_cbGetTotalTime, 1);

   MM_RELEASEIF(pme->m_pISound);

   FREEIF(pme->u.m_pMP3); // Note: This is a union member!
/*
   switch (pme->m_clsSelf)
   {
      case AEECLSID_MEDIAMP3:
         FREEIF(pme->u.m_pMP3);
         break;

      case AEECLSID_MEDIAQCP:
         FREEIF(pme->u.m_pQCP);
         break;

      case AEECLSID_MEDIAPMD:
         break;
   } // switch
*/

   pme->m_bStreaming = FALSE;

   if (pme->m_pqCMXDataReq)
   {
      CQueue_Delete(pme->m_pqCMXDataReq);
      pme->m_pqCMXDataReq = NULL;
   }

   if (pme->m_pcbCMXDataReq)
   {
      CALLBACK_Cancel(pme->m_pcbCMXDataReq);
      FREE(pme->m_pcbCMXDataReq);
      pme->m_pcbCMXDataReq = NULL;
   }

   if (pme->m_pqMediaData)
   {
      CQueue_Delete(pme->m_pqMediaData);
      pme->m_pqMediaData = NULL;
   }

   if (pme->m_pcbMediaData)
   {
      CALLBACK_Cancel(pme->m_pcbMediaData);
      FREE(pme->m_pcbMediaData);
      pme->m_pcbMediaData = NULL;
   }

   FREEIF(pme->m_pSrcBuffer);

#if defined(FEATURE_CMX_AV_SYNC)
   FREEIF(pme->m_pAudioSyncInfo);
#endif

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
   if (pme->m_bISynthQualityRef)
   {
      gdwNotes -= pme->m_dwNotes;
      if (gpISynthQuality)
      {
         int32 nRef = ISYNTHQUALITY_Release(gpISynthQuality);
         if (!nRef)
         {
            gpISynthQuality = NULL;
            gdwNotes = OEMMM_MAX_NOTES;
            gbNotesSet = FALSE;
         }
      } // if (gpISynthQuality)
      pme->m_bISynthQualityRef = FALSE;
   }
#endif
}

/*==================================================================

==================================================================*/
static int CMediaBg_Play(IMedia * po)
{
  CMediaBg *  pme = (CMediaBg *)po;
   int         nRet;

   nRet = AEEMedia_Play(po);

#if defined (FEATURE_ACM)
   if (nRet == MM_PENDING)
   {
     // ACM Resource Acquire Pending
     return SUCCESS;
   }
#endif // FEATURE_ACM
   if (nRet == SUCCESS)
   {
      nRet = CMediaMMLayer_Play(pme);

#if defined (FEATURE_ACM)
   if (nRet != SUCCESS)
      {
         AEEMedia_CleanUpResource((AEEMedia *) po);
      }
#endif // FEATURE_ACM
   }
   return nRet;
}

/*==================================================================

==================================================================*/
static int CMediaBg_Stop(IMedia * po)
{
   CMediaBg * pme = (CMediaBg *)po;
   int            nRet;

   nRet = AEEMedia_Stop(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaMMLayer_Stop(pme);
}

/*==================================================================

==================================================================*/
static int CMediaBg_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   CMediaBg * pme = (CMediaBg *)po;
   int            nRet;
   IShell * pIShell = AEE_GetShell();

   nRet = AEEMedia_Seek(po, eSeek, lTimeMS);
   if (nRet != SUCCESS)
      return nRet;

   if (AEEMedia_IsSeekFrame(eSeek))
      return EUNSUPPORTED;

   ISHELL_CancelTimer(pIShell,(PFNNOTIFY)(CMediaMMLayer_CallbackTimer),pMediaCB);

   return CMediaMMLayer_Seek(pme, eSeek, lTimeMS);
}

/*==================================================================

==================================================================*/
static int CMediaBg_Pause(IMedia * po)
{
   CMediaBg * pme = (CMediaBg *)po;
   int            nRet;
   IShell * pIShell = AEE_GetShell();
   AEEMediaCallback *   pcb = (AEEMediaCallback *)pMediaCB;

   nRet = AEEMedia_Pause(po);
   if (nRet != SUCCESS)
      return nRet;

   ISHELL_CancelTimer(pIShell,(PFNNOTIFY)(CMediaMMLayer_CallbackTimer),pcb);
   return CMediaMMLayer_Pause(pme);
}

/*==================================================================

==================================================================*/
static int CMediaBg_Resume(IMedia * po)
{
   CMediaBg * pme = (CMediaBg *)po;
   int            nRet;

   nRet = AEEMedia_Resume(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaMMLayer_Resume(pme);
}

/*==================================================================

==================================================================*/
static int CMediaBg_GetTotalTime(IMedia * po)
{
   CMediaBg * pme = (CMediaBg *)po;
   int            nRet;
   IShell * pIShell = AEE_GetShell();

   nRet = AEEMedia_GetTotalTime(po);
   if (nRet != SUCCESS)
      return nRet;
   ISHELL_CancelTimer(pIShell,(PFNNOTIFY)(CMediaMMLayer_CallbackTimer),pMediaCB);
   return CMediaMMLayer_GetTotalTime(pme);
}

/*==================================================================

   Callback Notification Functions: called in BREW context after
   ISHELL_Resume()

==================================================================*/
/*==================================================================
   Media callback notify -- called in BREW context
==================================================================*/
void CMediaBg_CallbackNotify(void * pUser)
{
   AEEMediaCallback *   pcb = (AEEMediaCallback *)pUser;
   CMediaBg *           pme;
   
   if (!pcb)
      return;

   pme = (CMediaBg *)pcb->cmdNotify.pIMedia;
   if (!pme)
      return;

   if (pcb->cmdNotify.nStatus == MM_STATUS_MEDIA_SPEC)
   {
      // If it is media spec, then send m_spec to user.
      if (pme->m_clsSelf == AEECLSID_MEDIAMP3 && pme->u.m_pMP3)
      {
         pcb->cmdNotify.pCmdData = (void *)&pme->u.m_pMP3->m_spec;
         pcb->cmdNotify.dwSize = sizeof(AEEMediaMP3Spec);
      }
      else if (pme->m_clsSelf == AEECLSID_MEDIAADPCM && pme->u.m_pADPCM)
      {
         pcb->cmdNotify.pCmdData = (void *)&pme->u.m_pADPCM->m_spec;
         pcb->cmdNotify.dwSize = sizeof(AEEMediaWaveSpec);
      }
   }
   else if (pcb->cmdNotify.nStatus == MM_STATUS_DONE || pcb->cmdNotify.nStatus == MM_STATUS_ABORT)
   {
      if (OEMMM_ISSTREAMING(pme))
         CMediaMMLayer_FlushStreamData(pme);
   }

   CMediaCMX_CallbackNotify(pcb);
}


void OEMMedia_VC_PlayCB(unsigned int command)
{
  AEEMediaCallback * pcb = (AEEMediaCallback *)pMediaCB;
  AEEMedia * DpUser = (AEEMedia *)pMediaCB;
  IShell * pShell = AEE_GetShell();
  
  if(!pMediaCB || !pShell)
  {
     return;
  }
  MSG_ERROR("OEMMedia_VC_PlayCB get command:0x%04x", command, 0, 0);
  switch(command)
  {
  case MMD_PLY_END: // MMD_D2H_PLY_FILE_END
     if(VC_GetCurrentDevice() == VC_DEV_PLY_AMR)
     {
        ISHELL_CancelTimer(pShell,(PFNNOTIFY)(CMediaMMLayer_CallbackTimer),pMediaCB);
        Lcd_Rotate(VIM_HAPI_ROTATE_0);
        VIM_SetRT(0);
	 DpUser->m_nState = MM_STATE_PLAY;		
        pcb->cmdNotify.nStatus = MM_STATUS_DONE;
        pcb->cmdNotify.nCmd = MM_CMD_PLAY;
        CALLBACK_Cancel(&g_MediaStopCB);
        CALLBACK_Init(&g_MediaStopCB, OEMMediaMPEG4_CallbackNotify, pcb);
        ISHELL_Resume(pShell, &g_MediaStopCB);
     }
     else if(VC_GetCurrentDevice() == VC_DEV_PLY_MP3 || VC_GetCurrentDevice() == VC_DEV_PLY_AMRAUDIO
	 	   || VC_GetCurrentDevice() == VC_DEV_PLY_AAC)
     {
        ISHELL_CancelTimer(pShell,(PFNNOTIFY)(CMediaMMLayer_CallbackTimer),pMediaCB);
	 DpUser->m_nState = MM_STATE_PLAY;		
        pcb->cmdNotify.nStatus = MM_STATUS_DONE;
        pcb->cmdNotify.nCmd = MM_CMD_PLAY;
        CALLBACK_Cancel(&g_MediaStopCB);
        CALLBACK_Init(&g_MediaStopCB, CMediaBg_CallbackNotify, pcb);
        ISHELL_Resume(pShell, &g_MediaStopCB);
     }
     break;

  default: 
     break;     
  }
}

/*==================================================================

==================================================================*/
static int CMediaBg_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   CMediaBg *  pme = (CMediaBg *)po;
   int         nRet = EUNSUPPORTED;
   boolean     bProc = TRUE;

   if (pme->m_clsSelf == AEECLSID_MEDIAQCP && pme->u.m_pQCP)
   {
      switch (nParmID)
      {
         case MM_PARM_TICK_TIME:
            pme->m_dwTickTimeMS = (uint32)p1;
               nRet = SUCCESS;
            break;

         case MM_PARM_QCP_FORMAT:
               pme->u.m_pQCP->m_nFormat = (byte)p1;
               nRet = SUCCESS;
            break;

         default:
            bProc = FALSE;
            break;
      }

      if (bProc)
         return nRet;
   }

/*#if defined(FEATURE_AAC_REC)
   if (pme->m_clsSelf == AEECLSID_MEDIAAAC && pme->u.m_pAAC)
   {
      switch (nParmID)
	  {
         case MM_PARM_AAC_FORMAT:
               pme->u.m_pAAC->m_nFormat = (byte)p1;
               nRet = SUCCESS;
            break;

         default:
            bProc = FALSE;
            break;
	  }
      if (bProc)
         return nRet;
   }
#endif //FEATURE_AAC_REC*/

   bProc = TRUE;
   switch (nParmID)
   {
      case MM_PARM_VOLUME:
      {
         nRet = CMediaMMLayer_SetVolume(pme, (uint16)p1);
         if (!nRet || nRet == MM_PENDING)
            pme->m_wVolume = (uint16)p1;
         break;
      }

      case MM_PARM_AUDIO_DEVICE:
         nRet = CMediaMMLayer_SetDevice(pme, (AEESoundDevice)p1);
         break;

      case MM_PARM_AUDIO_PATH:
         nRet = CMediaMMLayer_SetPath(pme, (int)p1);
         break;

      case MM_PARM_MUTE:
         nRet = CMediaMMLayer_SetMute(pme, (boolean)p1);
         break;

      case MM_PARM_TEMPO:
         if (AEEMedia_IsPlayOn(pme))
            nRet = CMediaMMLayer_SetTempo(pme, (uint32)p1);
         else
            nRet = EBADSTATE;
         break;

      case MM_PARM_TUNE:
         if (AEEMedia_IsPlayOn(pme))
            nRet = CMediaMMLayer_SetTune(pme, (uint32)p1);
         else
            nRet = EBADSTATE;
         break;

      case MM_PARM_PAN:
         if (AEEMedia_IsPlayOn(pme))
            nRet = CMediaMMLayer_SetPan(pme, (uint32)p1);
         else
            nRet = EBADSTATE;
         break;

      case MM_PARM_POS:
         break;

      case MM_PARM_TICK_TIME:
         if (pme->m_clsSelf == AEECLSID_MEDIAPCM)
         {
            pme->m_dwTickTimeMS = (int)p1;
            nRet = SUCCESS;
         }
         break;

      case MM_PARM_CHANNEL_SHARE:
      {
           boolean  bEnable = (boolean)p1;
           boolean bChanging;
           int nState = IMEDIA_GetState(po, &bChanging);
           if (nState == MM_STATE_READY || nState == MM_STATE_IDLE)
           {
              if (pme->m_bChannelShare != bEnable)
              {
                pme->m_bChannelShare = bEnable;
              }
              nRet = SUCCESS;
           }
           else
              nRet = EBADSTATE;
      }
      break;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
      case MM_PARM_NOTES:
         if (CMediaMMLayer_IsMultiSequence(pme))
         {
            if (AEEMedia_IsReadyState(pme))
            {
               // Create ISynthQuality instance if not already done.
               if (!pme->m_bISynthQualityRef)
               {
                  pme->m_bISynthQualityRef = TRUE;
                  if (!gpISynthQuality)
                     ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_SYNTHQUALITY, (void **)&gpISynthQuality);
                  else
                     ISYNTHQUALITY_AddRef(gpISynthQuality);
               }

               // Update the number of notes.
               if (!gbNotesSet) // Uninitialized and default
               {
                  gdwNotes = pme->m_dwNotes = (uint32)p1;
                  gbNotesSet = TRUE;
               }
               else
               {
                  // Notes already set. So, set the new number of notes.
                  gdwNotes -= pme->m_dwNotes;
                  pme->m_dwNotes = (uint32)p1;
                  gdwNotes += pme->m_dwNotes;
               }
               nRet = SUCCESS;
            } // if (AEEMedia_IsReadyState(pme))
            else
               nRet = EBADSTATE;
         } // if (CMediaMMLayer_IsMultiSequence(pme))
         break;
#endif

#if defined(FEATURE_CMX_AV_SYNC)
      case MM_PARM_AUDIOSYNC:
      {
         if (p1 && (MM_AUDIOSYNC_MODE_TIME == p1 || MM_AUDIOSYNC_MODE_SAMPLE == p1))
         {
            pme->m_dwAudioSyncType = (uint32)p1;
            pme->m_dwAudioSyncData = (uint32)p2;
            nRet = SUCCESS;
         }
         else
            nRet = EBADPARM;
      }
      break;
#endif //defined(FEATURE_CMX_AV_SYNC)

      default:
         bProc = FALSE;
         break;
   }

   if (bProc)
      return nRet;

   return CMediaCMX_SetMediaParm(po, nParmID, p1, p2);
}

/*==================================================================

==================================================================*/
static int CMediaBg_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2)
{
   CMediaBg *  pme = (CMediaBg *)po;
   int         nRet = SUCCESS;
   boolean     bProc = TRUE;

   if (pme->m_clsSelf == AEECLSID_MEDIAQCP && pme->u.m_pQCP)
   {
      switch (nParmID)
      {
         case MM_PARM_TICK_TIME:
            if (pP1)
               *pP1 = pme->m_dwTickTimeMS;
            break;

         case MM_PARM_QCP_FORMAT:
            if (pP1)
               *pP1 = pme->u.m_pQCP->m_nFormat;
            break;

         default:
            bProc = FALSE;
            break;
      }

      if (bProc)
         return nRet;
   }

/*#if defined(FEATURE_AAC_REC)
   if (pme->m_clsSelf == AEECLSID_MEDIAAAC && pme->u.m_pAAC)
   {
      switch (nParmID)
      {
		 case MM_PARM_AAC_FORMAT:
            if (pP1)
               *pP1 = pme->u.m_pAAC->m_nFormat;
            break;
		 default:
            bProc = FALSE;
            break;
      }

      if (bProc)
         return nRet;
   }
#endif // FEATURE_AAC_REC*/

   bProc = TRUE;
   switch (nParmID)
   {
      case MM_PARM_VOLUME:
         // No need to alloc a command callback since the callback from
         // ISound uniquely identifies this operation and also
         // gets called in BREW context.
         ISOUND_Get(pme->m_pISound, NULL);	// Update the current device settings
         ISOUND_GetVolume(pme->m_pISound);
         nRet = MM_PENDING;
         break;

      case MM_PARM_AUDIO_DEVICE:
         nRet = EUNSUPPORTED;
         break;

      case MM_PARM_AUDIO_PATH:
         if (pP1)
            *pP1 = (int32)pme->m_nAPath;
         break;

      case MM_PARM_MUTE:
         if (pP1)
            *pP1 = (int32)pme->m_bMute;
         break;

      case MM_PARM_TEMPO:
         if (pP1)
            *pP1 = (int32)pme->m_wTempo;
         break;

      case MM_PARM_TUNE:
         if (pP1)
            *pP1 = (int32)pme->m_wTune;
         break;

      case MM_PARM_PAN:
         if (pP1)
            *pP1 = (int32)pme->m_wPan;
         break;

      case MM_PARM_POS:
         nRet = EUNSUPPORTED;
         break;

      case MM_PARM_TICK_TIME:
         if (pme->m_clsSelf == AEECLSID_MEDIAPCM)
         {
            if (pP1)
               *pP1 = pme->m_dwTickTimeMS;
         }
         else
            nRet = EUNSUPPORTED;
         break;

      case MM_PARM_RECT:
            nRet = EUNSUPPORTED;
         break;

      case MM_PARM_FRAME:
            nRet = EUNSUPPORTED;
         break;

      case MM_PARM_CHANNEL_SHARE:
         if (pP1)
            *(boolean *)pP1 = pme->m_bChannelShare;
         break;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
      case MM_PARM_NOTES:
         if (pP1 && CMediaMMLayer_IsMultiSequence(pme))
            *(uint32 *)pP1 = pme->m_dwNotes;
         else
            nRet = EUNSUPPORTED;
         break;
#endif

#if defined(FEATURE_CMX_AV_SYNC)
      case MM_PARM_AUDIOSYNC:
      {
         if (pP1)
            *(uint32 *)pP1 = pme->m_dwAudioSyncType;
         if (pP2)
            *(uint32 *)pP2 = pme->m_dwAudioSyncData;
      }
      break;
#endif //defined(FEATURE_CMX_AV_SYNC)

      default:
         bProc = FALSE;
         break;
   }

   if (bProc)
      return nRet;

   return CMediaCMX_GetMediaParm(po, nParmID, pP1, pP2);
}

/*==================================================================

==================================================================*/
static int CMediaBg_Record(IMedia * po)
{
   CMediaBg * pme = (CMediaBg *)po;
   int         nRet;

   if (pme->m_clsSelf == AEECLSID_MEDIAQCP && pme->u.m_pQCP)
   {
      nRet = AEEMedia_Record(po);

#if defined (FEATURE_ACM)
      if (nRet == MM_PENDING)
      {
         return SUCCESS; /* Delayed Acquire */
      }
#endif // FEATURE_ACM

      if (nRet == SUCCESS)
      {
         nRet = CMediaMMLayer_QCPRecord(pme, pme->u.m_pQCP->m_nFormat);

#if defined (FEATURE_ACM)
         if(nRet != SUCCESS)
         {
            AEEMedia_CleanUpResource((AEEMedia *)po);
         }
#endif // FEATURE_ACM
      }
   }
   else if (pme->m_clsSelf == AEECLSID_MEDIAPCM || (pme->m_clsSelf == AEECLSID_MEDIAADPCM && pme->u.m_pADPCM))
   {
      nRet = AEEMedia_Record(po);
#if defined (FEATURE_ACM)
      if (nRet == MM_PENDING)
      {
         return SUCCESS; /* Delayed Acquire */
      }
#endif // FEATURE_ACM

      if (nRet == SUCCESS)
      {
         nRet = CMediaMMLayer_PCMRecord(pme);
#if defined (FEATURE_ACM)
         if(nRet != SUCCESS)
         {
            AEEMedia_CleanUpResource((AEEMedia *)po);
         }
#endif // FEATURE_ACM
      }
   }
#if defined(FEATURE_AAC_REC)
   else if (pme->m_clsSelf == AEECLSID_MEDIAAAC && pme->u.m_pAAC)
   {
      nRet = AEEMedia_Record(po);
#if defined (FEATURE_ACM)
      if (nRet == MM_PENDING)
      {
         return SUCCESS; /* Delayed Acquire */
      }
#endif // FEATURE_ACM

      if (nRet == SUCCESS)
      {
         nRet = CMediaMMLayer_AACRecord(pme, pme->u.m_pAAC->m_nFormat);
#if defined (FEATURE_ACM)
         if(nRet != SUCCESS)
         {
            AEEMedia_CleanUpResource((AEEMedia *)po);
         }
#endif // FEATURE_ACM
      }
   }
#endif // FEATURE_AAC_REC
   else if (pme->m_clsSelf == AEECLSID_MEDIAAMR)
   {
      nRet = AEEMedia_Record(po);

#if defined (FEATURE_ACM)
      if (nRet == MM_PENDING)
      {
         return SUCCESS; /* Delayed Acquire */
      }
#endif // FEATURE_ACM

      if (nRet == SUCCESS)
      {
         nRet = CMediaMMLayer_AMRRecord(pme);
      }

#if defined (FEATURE_ACM)
         if(nRet != SUCCESS)
         {
            AEEMedia_CleanUpResource((AEEMedia *)po);
         }
#endif // FEATURE_ACM
      }
   else nRet = EUNSUPPORTED;
   return nRet;
}
/*===============================================================================

                     IMediaMIDIOutMsg PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==================================================================
   Called only once during AEE initialization
==================================================================*/
void IMediaVC0848MIDIOutMsg_Init(IShell * ps)
{
   // Initialize ASIC MM Layer here...
   CMediaMMLayer_Init(ps);
}

/*==================================================================
   IMediaMIDIOutMsg constructor
   Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaVC0848MIDIOutMsg_New(IShell * ps, AEECLSID cls, void **ppif)
{
   IBase * pobj = CMediaCMX_New(ps, cls, &gMediaMIDIOutMsgFuncs, sizeof(CMediaMIDIOutMsg), (PFNNOTIFY)CMediaMIDIOutMsg_Delete);

   if (pobj)
   {
      CMediaMIDIOutMsg *  pme = (CMediaMIDIOutMsg *)pobj;
      CMediaMMLayer_InitCallback((IMedia *)pme, MM_CMD_PLAY, pme->m_cbPlay, OEMMM_MAX_CB_PLAY, CMediaCMX_CallbackNotify);
      CMediaMMLayer_InitCallback((IMedia *)pme, 0, &pme->m_cbCommand, 1, CMediaCMX_CallbackNotify);
   }
   {
      vc_union_type vc_data;
      vc_data.dev_run.curr_dev = VC_DEV_PLY_AMR;
      VC_DeviceControl(VC_ITM_DEV_OPEN_I, VC_FUNC_CONTROL, &vc_data); // init hardware for mp3 player
   }

   *ppif = pobj;
   return pobj ? SUCCESS : ENOMEMORY;
}


/*==================================================================
   Destructor
==================================================================*/
static void CMediaMIDIOutMsg_Delete(CMediaMIDIOutMsg * pme)
{
   CMediaMMLayer_CancelCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY);
   CMediaMMLayer_CancelCallback(&pme->m_cbCommand, 1);

#if defined(FEATURE_MIDI_OUT)
   if (pme->m_bMIDIOut)
   {
      CMIDIOut_Release();
      pme->m_bMIDIOut = FALSE;
   }
#endif // defined(FEATURE_MIDI_OUT)
}

/*==================================================================

==================================================================*/
static int CMediaMIDIOutMsg_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   CMediaMIDIOutMsg *   pme = (CMediaMIDIOutMsg *)po;
   int                  nRet = EUNSUPPORTED;
   boolean              bProc = TRUE;

   // Volume/Path/Mute/Pan is handled.
   switch (nParmID)
   {
      case MM_PARM_VOLUME:
      case MM_PARM_PAN:
      case MM_PARM_TEMPO:
      case MM_PARM_TUNE:
         break;

      case MM_PARM_MIDI_MSG:
         if (!p1 || !p2)
            nRet = EBADPARM;
         else
         {
            pme->m_pMsg = (uint8 *)p1;
            pme->m_dwSize = (uint32)p2;
            nRet = SUCCESS;
         }
         break;

      default:
         bProc = FALSE;
         break;
   }

   if (bProc)
      return nRet;

   return CMediaCMX_SetMediaParm(po, nParmID, p1, p2);
}

/*==================================================================

==================================================================*/
static int CMediaMIDIOutMsg_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2)
{
   CMediaMIDIOutMsg *   pme = (CMediaMIDIOutMsg *)po;
   int                  nRet = SUCCESS;
   boolean              bProc = TRUE;

   // Volume/Path/Mute/Pan is handled.
   switch (nParmID)
   {
      case MM_PARM_VOLUME:
      case MM_PARM_PAN:
      case MM_PARM_TEMPO:
      case MM_PARM_TUNE:
         nRet = EUNSUPPORTED;
         break;

      case MM_PARM_MIDI_MSG:
         if (pP1)
            *pP1 = (int32)pme->m_pMsg;
         if (pP2)
            *pP2 = (int32)pme->m_dwSize;
         break;

      default:
         bProc = FALSE;
         break;
   }

   if (bProc)
      return nRet;

   return CMediaCMX_GetMediaParm(po, nParmID, pP1, pP2);
}

/*==================================================================

==================================================================*/
static int CMediaMIDIOutMsg_Play(IMedia * po)
{
   CMediaMIDIOutMsg *  pme = (CMediaMIDIOutMsg *)po;
   int            nRet;

   nRet = AEEMedia_Play(po);

#if defined (FEATURE_ACM)
   if (nRet == MM_PENDING)
   {
      // ACM Resource Acquire Pending
      return SUCCESS;
   }
#endif // FEATURE_ACM

   if (nRet == SUCCESS)
   {
      nRet = CMediaMMLayer_MIDIOutMsg(pme);

#if defined (FEATURE_ACM)
   if (nRet != SUCCESS)
   {
      AEEMedia_CleanUpResource((AEEMedia *) po);
   }
#endif // FEATURE_ACM
   }

   return nRet;
}

/*===============================================================================

                     IMediaMIDIOutQCP PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==================================================================
   Called only once during AEE initialization
==================================================================*/
void IMediaVC0848MIDIOutQCP_Init(IShell * ps)
{
   if (gInitFlags.gbMediaMIDIOutQCPInit)
      return;

   gInitFlags.gbMediaMIDIOutQCPInit = TRUE;

   // If found in the mod table, then register the formats...

   if (CMediaFormat_InModTable(AEECLSID_MEDIAMIDIOUTQCP, (void *)IMediaVC0848MIDIOutQCP_Init, NULL))
      AEEMedia_Init(ps, MT_AUDIO_QCF, AEECLSID_MEDIAMIDIOUTQCP);

   // Initialize ASIC MM Layer here...
   CMediaMMLayer_Init(ps);
}

/*==================================================================
   IMediaMIDIOutQCP constructor
   Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaVC0848MIDIOutQCP_New(IShell * ps, AEECLSID cls, void **ppif)
{
   IBase * pobj = CMediaCMX_New(ps, cls, &gMediaMIDIOutQCPFuncs, sizeof(CMediaMIDIOutQCP), (PFNNOTIFY)CMediaMIDIOutQCP_Delete);

   if (pobj)
   {

      CMediaMIDIOutQCP *  pme = (CMediaMIDIOutQCP *)pobj;
      CMediaMMLayer_InitCallback((IMedia *)pme, MM_CMD_PLAY, pme->m_cbPlay, OEMMM_MAX_CB_PLAY, CMediaCMX_CallbackNotify);
      CMediaMMLayer_InitCallback((IMedia *)pme, 0, &pme->m_cbCommand, 1, CMediaCMX_CallbackNotify);
#if defined(FEATURE_QAUDIOFX)
#if !defined (FEATURE_CMX_MM_API)
      CMediaMMLayer_InitCallback((IMedia *)pme, 0, &pme->m_cb3DStart, 1, CMediaMIDIOutQCP_QCPStartCB);
#endif // !defined (FEATURE_CMX_MM_API)
#endif // defined(FEATURE_QAUDIOFX)

#if defined(FEATURE_MIDI_OUT_QCP)
      {
         CMXInfo  ci;
         ci.nType = OEMCMX_TYPE_MIDI_OUT_QCP;
         ci.ph = NULL;
         ci.dwCB = (uint32)CMediaMMLayer_MIDIOutQCPStatusCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_STATUS_CB, (int32)&ci, 0, 0, TRUE);
      }
#endif // defined(FEATURE_MIDI_OUT_QCP)
      pme->m_pIShell = ps;
   }

   *ppif = pobj;
   return pobj ? SUCCESS : ENOMEMORY;
}

/*==================================================================
   Destructor
==================================================================*/
static void CMediaMIDIOutQCP_Delete(CMediaMIDIOutQCP * pme)
{
   CMediaMMLayer_CancelCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY);
   CMediaMMLayer_CancelCallback(&pme->m_cbCommand, 1);
#if defined(FEATURE_QAUDIOFX)
#if !defined (FEATURE_CMX_MM_API)
   CMediaMMLayer_CancelCallback(&pme->m_cb3DStart, 1);
#endif // !defined (FEATURE_CMX_MM_API)
   {
      paramCache * pCache;

      OEMMM_ENTER_CRITICAL_SECTION()

      while (NULL != (pCache = (paramCache *)pme->m_pCache))
      {
         pme->m_pCache = pCache->pNext;
         FREE(pCache);
      }
      pme->m_pLast = NULL;

      OEMMM_LEAVE_CRITICAL_SECTION()
   }
#endif // defined(FEATURE_QAUDIOFX)

#if defined(FEATURE_MIDI_OUT)
   if (pme->m_bMIDIOut)
   {
      CMIDIOut_Release();
      pme->m_bMIDIOut = FALSE;
   }
#endif // defined(FEATURE_MIDI_OUT)
}

/*==================================================================

==================================================================*/
static int CMediaMIDIOutQCP_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   CMediaMIDIOutQCP * pme = (CMediaMIDIOutQCP *)po;
   int         nRet = EUNSUPPORTED;
   boolean     bProc = TRUE;

   // Volume/Path/Mute/Pan is handled.
   switch (nParmID)
   {
      case MM_PARM_VOLUME:
         nRet = CMediaMMLayer_MIDIOutQCPSetVolume(pme, pme->m_dwChannel, (uint16)p1);
         break;

      case MM_PARM_PAN:
         nRet = CMediaMMLayer_MIDIOutQCPSetPan(pme, pme->m_dwChannel, (uint16)p1);
         break;

      case MM_PARM_TEMPO:
      case MM_PARM_TUNE:
         break;

      default:
         bProc = FALSE;
         break;
   }

   if (bProc)
      return nRet;

   return CMediaCMX_SetMediaParm(po, nParmID, p1, p2);
}

/*==================================================================

==================================================================*/
static int CMediaMIDIOutQCP_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2)
{
   int                  nRet = EUNSUPPORTED;
   boolean              bProc = TRUE;

   // Tempo/Mute/Pan is handled.
   switch (nParmID)
   {
      case MM_PARM_TEMPO:
      case MM_PARM_TUNE:
      case MM_PARM_PAN:
         break;

      default:
         bProc = FALSE;
         break;
   }

   if (bProc)
      return nRet;

   return CMediaCMX_GetMediaParm(po, nParmID, pP1, pP2);
}

/*==================================================================

==================================================================*/
static int CMediaMIDIOutQCP_Play(IMedia * po)
{
   CMediaMIDIOutQCP *   pme = (CMediaMIDIOutQCP *)po;
   int                  nRet;

   nRet = AEEMedia_Play(po);

#if defined (FEATURE_ACM)
   if (nRet == MM_PENDING)
   {
      // ACM Resource Acquire Pending
      return SUCCESS;
   }
#endif // FEATURE_ACM

   if (nRet == SUCCESS)
   {
      nRet = CMediaMMLayer_MIDIOutQCPPlay(pme);

#if defined (FEATURE_ACM)
      if (nRet != SUCCESS)
      {
         AEEMedia_CleanUpResource((AEEMedia *) po);
      }
#endif // FEATURE_ACM
}

   return nRet;
}

/*==================================================================

==================================================================*/
static int CMediaMIDIOutQCP_Stop(IMedia * po)
{
   CMediaMIDIOutQCP *   pme = (CMediaMIDIOutQCP *)po;
   int                  nRet;

   nRet = AEEMedia_Stop(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaMMLayer_MIDIOutQCPStop(pme, pme->m_dwChannel);
}

/*==================================================================

==================================================================*/
static int CMediaMIDIOutQCP_Pause(IMedia * po)
{
   CMediaMIDIOutQCP *   pme = (CMediaMIDIOutQCP *)po;
   int                  nRet;

   nRet = AEEMedia_Pause(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaMMLayer_MIDIOutQCPPause(pme);
}

/*==================================================================

==================================================================*/
static int CMediaMIDIOutQCP_Resume(IMedia * po)
{
   CMediaMIDIOutQCP *   pme = (CMediaMIDIOutQCP *)po;
   int                  nRet;

   nRet = AEEMedia_Resume(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaMMLayer_MIDIOutQCPResume(pme);
}

/*==================================================================

   CMediaBg Local Functions

==================================================================*/

/*==================================================================

   CMediaCMX: MMLayer

==================================================================*/
/*===========================================================================

                      MACRO DEFINITIONS

===========================================================================*/
#define CMediaMMLayer_ISCOMMAND(pcb)     ((pcb)->bInUse)
#define CMediaMMLayer_ISCMDPLAYREC(pcb)  ((pcb)->cmdNotify.nCmd == MM_CMD_PLAY \
                                          || (pcb)->cmdNotify.nCmd == MM_CMD_RECORD)
#define CMediaMMLayer_ISPLAYCTL(pcb)     (CMediaMMLayer_ISCOMMAND(pcb) && CMediaMMLayer_ISCMDPLAYREC(pcb) \
                                          && (pcb)->cmdNotify.nSubCmd > OEMMM_CTLCMD_NONE)
#define CMediaMMLayer_ISSEEK(pcb)        (CMediaMMLayer_ISCOMMAND(pcb) && CMediaMMLayer_ISCMDPLAYREC(pcb) \
                                          && (pcb)->cmdNotify.nSubCmd == OEMMM_CTLCMD_SEEK)
#define CMediaMMLayer_ISPAUSE(pcb)       (CMediaMMLayer_ISCOMMAND(pcb) && CMediaMMLayer_ISCMDPLAYREC(pcb) \
                                          && (pcb)->cmdNotify.nSubCmd == OEMMM_CTLCMD_PAUSE)
#define CMediaMMLayer_ISRESUME(pcb)      (CMediaMMLayer_ISCOMMAND(pcb) && CMediaMMLayer_ISCMDPLAYREC(pcb) \
                                          && (pcb)->cmdNotify.nSubCmd == OEMMM_CTLCMD_RESUME)
#define CMediaMMLayer_ISSTOP(pcb)        (CMediaMMLayer_ISCOMMAND(pcb) && CMediaMMLayer_ISCMDPLAYREC(pcb) \
                                          && (pcb)->cmdNotify.nSubCmd == OEMMM_CTLCMD_STOP)

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef enum OEMMMStatus
{
   OEMMM_STATUS_PLAY = 0,
   OEMMM_STATUS_PLAYCTL,
   OEMMM_STATUS_SETPARM
} OEMMMStatus;

typedef enum OEMMMCtlCmd
{
   OEMMM_CTLCMD_NONE = 0,
   OEMMM_CTLCMD_SEEK,
   OEMMM_CTLCMD_PAUSE,
   OEMMM_CTLCMD_RESUME,
   OEMMM_CTLCMD_STOP
} OEMMMCtlCmd;

static int CMediaMMLayer_SetSource(CMediaCMX * pme, cmx_handle_type * ph);

static void CMediaMMLayer_RequestSourceData(CMediaBg * pme);
static void CMediaMMLayer_ReadSource(CMediaBg * pme);

/*==================================================================
   Called only once during AEE initialization.
==================================================================*/
static void CMediaMMLayer_Init(IShell * ps)
{
   if (gInitFlags.gbCMXInit)
      return;

   gInitFlags.gbCMXInit = TRUE;

   CALLBACK_Init(&gcbAEEExit, CMediaMMLayer_AEEExitNotify, NULL);
   ISHELL_OnExit(ps, &gcbAEEExit);
}

/*==================================================================
   Called only once during AEE Exit
==================================================================*/
static void CMediaMMLayer_AEEExitNotify(void * pUser)
{
   gInitFlags.gbCMXInit = FALSE;
   gInitFlags.gbMediaBgInit = FALSE;
   gInitFlags.gbMediaMIDIOutQCPInit = FALSE;
}

/*==================================================================

      Sound functions

==================================================================*/
/*==================================================================

==================================================================*/
static int CMediaMMLayer_SetVolume(CMediaBg * pme, uint16 wVolume)
{
   vc_union_type vc_data;
   
   if (!pme->m_pISound)
      return EUNSUPPORTED;

   if (wVolume > 100)
      return EBADPARM;

   ISOUND_Get(pme->m_pISound, NULL);	// Update the current device settings
   ISOUND_SetVolume(pme->m_pISound, wVolume);

   vc_data.parm.p1 = wVolume;
   VC_DeviceControl(VC_ITM_SET_MEDIA_VOL_I, VC_FUNC_SET_PARM, &vc_data);
   return MM_PENDING;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_SetDevice(CMediaBg * pme, AEESoundDevice eDevice)
{
   AEESoundInfo   si;
   ISound *       pISound = pme->m_pISound   ;

   if (!pISound)
      return EUNSUPPORTED;

   ISOUND_Get(pISound, &si);
   si.eDevice = eDevice;
   ISOUND_Set(pISound, &si);

   ISOUND_SetDevice(pISound);

   return MM_PENDING;
}

/*==================================================================
   For playback
==================================================================*/
static int CMediaMMLayer_SetPath(CMediaBg * pme, int nAPath)
{
   if (pme->m_clsSelf != AEECLSID_MEDIAQCP &&
       pme->m_clsSelf != AEECLSID_MEDIAPCM &&
       pme->m_clsSelf != AEECLSID_MEDIAADPCM)
      return EUNSUPPORTED;

   if (nAPath > MM_APATH_BOTH)
      return EBADPARM;

   pme->m_nAPath = nAPath;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_SetMute(CMediaBg * pme, boolean bMute)
{
   pme->m_bMute = bMute;

   return CMediaMMLayer_SetVolume(pme, (uint16)(bMute ? 0 : pme->m_wVolume));
}


/*==================================================================

   Callback Notification Functions: called in BREW context after
   AEE_ResumeCallback()

==================================================================*/
/*==================================================================

==================================================================*/
static void CMediaMMLayer_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eStatus, uint32 dwParam)
{
   CMediaBg *      pme = (CMediaBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pUser);
   AEESoundCmdData *    pData = (AEESoundCmdData *)dwParam;
   AEEMediaCallback     mcb;

   if (!pme)
      return;

   MEMCPY(&mcb, &pme->m_cbCommand, sizeof(AEEMediaCallback));

   mcb.bInUse = TRUE;
   mcb.cmdNotify.nSubCmd = MM_PARM_VOLUME;
   mcb.cmdNotify.nCmd = (eCBType == AEE_SOUND_VOLUME_CB) ? MM_CMD_GETMEDIAPARM : MM_CMD_SETMEDIAPARM;
   mcb.cmdNotify.nStatus = (eStatus == AEE_SOUND_SUCCESS) ? MM_STATUS_DONE : MM_STATUS_ABORT;
   mcb.cmdNotify.pCmdData = NULL;
   mcb.cmdNotify.dwSize = 0;

   if (pData && eCBType == AEE_SOUND_VOLUME_CB && eStatus == AEE_SOUND_SUCCESS)
   {
      mcb.cmdNotify.pCmdData = (void *)(uint32)pData->wVolume;
      mcb.cmdNotify.dwSize = sizeof(pData->wVolume);
   }

   AEEMedia_CallbackNotify((AEEMedia *)pme, &mcb);
}

/*==================================================================

            LOCAL FUNCTIONS

==================================================================*/
static size_t strlcpy(char * dst, const char * src, size_t nSize)
{
   size_t  ret;
   char   *pcDst;

   if( !nSize )
      return STRLEN(src);

   for( ret = 0, pcDst = dst; (ret < nSize) && (*pcDst++ = *src); ret++, src++ ) {}

   if( ret == nSize ) dst[nSize-1] = '\0';

   while( *src++ ) ret++;

   return ret;
}

//$r$
/*==================================================================
   CQueue
==================================================================*/
static int  CQueue_New(int32 nMaxSize, boolean bMutex, CQueue ** ppQ)
{
   CQueue * pme;
   uint8 *  pBuf = MALLOC(sizeof(CQueue) + nMaxSize);
   if (!pBuf)
      return ENOMEMORY;

   pme = (CQueue *)pBuf;
   pme->pBuffer = pBuf + sizeof(CQueue);
   pme->nMaxSize = nMaxSize;
   pme->bMutex = bMutex;

   *ppQ = pme;

   return SUCCESS;
}

static void CQueue_Delete(CQueue * pme)
{
   FREE(pme);
}

static int32 CQueue_Put(CQueue * pme, uint8 * pData, int32 nSize)
{
   int32 nRet;

   if (pme->bMutex)
      AEECriticalSection_Enter(&gMediaCMXCriticalSection);

   if (CQueue_IsPutOk(pme, nSize))
   {
      int32   nTillEnd, nWrap;

      if (pme->nWrite + nSize > pme->nMaxSize)
      {
         nTillEnd = pme->nMaxSize - pme->nWrite;
         nWrap = nSize - nTillEnd;
      }
      else
      {
         nTillEnd = nSize;
         nWrap = 0;
      }

      MEMCPY(pme->pBuffer + pme->nWrite, pData, nTillEnd);
      if (nWrap)
      {
         MEMCPY(pme->pBuffer, pData + nTillEnd, nWrap);
         pme->nWrite = nWrap;
      }
      else
      {
         pme->nWrite += nTillEnd;
         if (pme->nWrite >= pme->nMaxSize)
            pme->nWrite = 0;
      }

      pme->nFill += nSize;

      nRet = nSize;
   }
   else
      nRet = 0;

   if (pme->bMutex)
      AEECriticalSection_Leave(&gMediaCMXCriticalSection);

   FARF(QUEUE, ("===Put:F:%ul W:%ul R:%ul S:%ul===", pme->nFill, pme->nWrite, pme->nRead, nRet));
   return nRet;
}

static int32 CQueue_Get(CQueue * pme, uint8 * pData, int32 nSize)
{
   int32   nRet;

   if (pme->bMutex)
      AEECriticalSection_Enter(&gMediaCMXCriticalSection);

   if (pme->nFill)// && pme->nWrite != pme->m_nRead)
   {
      int32   nTillEnd, nWrap;
      int32   nActualSize = MIN(nSize, pme->nFill);

      if (pme->nRead + nActualSize > pme->nMaxSize)
      {
         nTillEnd = pme->nMaxSize - pme->nRead;
         nWrap = nActualSize - nTillEnd;
      }
      else
      {
         nTillEnd = nActualSize;
         nWrap = 0;
      }

      MEMCPY(pData, pme->pBuffer + pme->nRead, nTillEnd);
      if (nWrap)
      {
         MEMCPY(pData + nTillEnd, pme->pBuffer, nWrap);
         pme->nRead = nWrap;
      }
      else
      {
         pme->nRead += nTillEnd;
         if (pme->nRead >= pme->nMaxSize)
            pme->nRead = 0;
      }

      pme->nFill -= nActualSize;

      nRet = nActualSize;
   }
   else
      nRet = 0;

   if (pme->bMutex)
      AEECriticalSection_Leave(&gMediaCMXCriticalSection);

   FARF(QUEUE, ("===Get:F:%ul W:%ul R:%ul S:%ul===", pme->nFill, pme->nWrite, pme->nRead, nRet));
   return nRet;
}

/*==================================================================
   Called in CMX context.
==================================================================*/

void CMediaMMLayer_DataRequestCB(
  uint32 position,
  uint32 max_len,
  cmx_client_req_cb_func_ptr_type data_req_cb,
  const void *server_client_data,
  const void *client_data
)
{
   CMXDataRequest req;
   CMediaBg *  pme = (CMediaBg *)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);

   FARF(SOURCE, ("===CMXCB: pos=%ul len=%ul", position, max_len));

   if (!pme)
   {
      data_req_cb(CMX_SUCCESS, NULL, 0, server_client_data);
      return;
   }

   if (!max_len || !data_req_cb)
      return;

   // Save the request in queue...
   req.dwPos = position;
   req.dwMaxLen = max_len;
   req.pfnDataReq = data_req_cb;
   req.pServerUserData = (void *)server_client_data;
//   req.pClientUserData = (void *)client_data;

   if (CQueue_Put(pme->m_pqCMXDataReq, (uint8 *)&req, sizeof(req)))
      AEE_ResumeCallback(pme->m_pcbCMXDataReq, pme->m_pac);
}

/*==================================================================

==================================================================*/
static void CMediaMMLayer_CMXDataReqNotify(CMediaBg * pme)
{
   int            nRead;
   int            nLen;
   CMXDataRequest req;

   if (!pme)
      return;

   while (1)
   {
      if (CQueue_IsEmpty(pme->m_pqCMXDataReq))
         break;

      if (CQueue_IsEmpty(pme->m_pqMediaData))
      {
         FARF(SOURCE, ("===CMXNotify: No media data"));
         CMediaMMLayer_RequestSourceData(pme);
         break;
      }

      if (!CQueue_Get(pme->m_pqCMXDataReq, (uint8 *)&req, sizeof(req)))
      {
         FARF(SOURCE, ("===CMXNotify: Fatal mdq error"));
         break;
      }

      CMediaMMLayer_RequestSourceData(pme);

      nLen = MIN(pme->m_dwSrcBufferSize, req.dwMaxLen);
      nRead = CQueue_Get(pme->m_pqMediaData, pme->m_pSrcBuffer, nLen);

      // This check is not required as it is done when callback arrives.
      //   if (!req.pfnDataReq || !req.dwMaxLen)
      //      return;
      FARF(SOURCE, ("===CMXNotify: nReq=%ul nLen=%d", req.dwMaxLen, nLen));

      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_DATA_RESPONSE, (int32)&req, (int32)pme->m_pSrcBuffer, (int32)nRead, FALSE);
//      req.pfnDataReq(CMX_SUCCESS, pme->m_pSrcBuffer, (uint32)nRead, req.pServerUserData);
   }
}

/*==================================================================

==================================================================*/
static void CMediaMMLayer_RequestSourceData(CMediaBg * pme)
{
   if (pme->m_bEOS || CALLBACK_IsQueued(pme->m_pcbMediaData))
      return;

   if (CQueue_Size(pme->m_pqMediaData) <= (CQueue_MaxSize(pme->m_pqMediaData) / 2))
   {
      FARF(SOURCE, ("===ReqSrc: MORE data"));
      AEE_ResumeCallback(pme->m_pcbMediaData, pme->m_pac);
   }
}

/*==================================================================

==================================================================*/
static void CMediaMMLayer_ReadSource(CMediaBg * pme)
{
   ISource *   pSource = (ISource *)pme->m_md.pData;
   int32       nRead;
   boolean     bNotify = FALSE;

   while (1)
   {
      if (CQueue_IsFull(pme->m_pqMediaData) || !CQueue_IsPutOk(pme->m_pqMediaData, (int32)pme->m_dwSrcBufferSize))
         break;

      nRead = ISOURCE_Read(pSource, (char *)pme->m_pSrcBuffer, (int32)pme->m_dwSrcBufferSize);

      if (ISOURCE_WAIT == nRead)
      {
         ISOURCE_Readable(pSource, pme->m_pcbMediaData);
         break;
      }

      if (ISOURCE_END == nRead || ISOURCE_Exhausted(nRead))
      {
         FARF(SOURCE, ("===ReadSrc: EOS %d", nRead));
         pme->m_bEOS = TRUE;
         break;
      }

      if (!CQueue_Put(pme->m_pqMediaData, pme->m_pSrcBuffer, nRead))
      {
         FARF(SOURCE, ("===ReadSrc: Put error nRead=%l", nRead));
         break;
      }

      bNotify = TRUE;
   }

   if (bNotify)
   {
      FARF(SOURCE, ("===ReadSrc: Proc CMXRq"));
      CMediaMMLayer_CMXDataReqNotify(pme);
   }
}

/*==================================================================
   Assumption: pme and ph are not NULL.
==================================================================*/
int CMediaMMLayer_SetSource(CMediaCMX * pme, cmx_handle_type * ph)
{
   AEEMedia *     pMedia = (AEEMedia *)pme;
   AEEMediaData * pmd = &pMedia->m_md;
   int            nRet = SUCCESS;

   if (pmd->clsData == MMD_FILE_NAME)
   {
      ph->efs.source_type = CMX_SOURCE_EFS;
      if (pmd->pData)
         strlcpy(ph->efs.filename, pMedia->m_pszFile, sizeof(ph->efs.filename));
      else
         nRet = EBADPARM;
   }
   else if (pmd->clsData == MMD_BUFFER)
   {
      ph->mem.source_type = CMX_SOURCE_MEM;
      if ( pmd->pData )
      {
         ph->mem.buf = (byte *)pmd->pData;
         /*
            OEM NOTE: Backward Compatibility
            ISOUNDPLAYER_Set with SDT_BUFFER workaround
            if the size was set to 16, then ISOUNDPLAYER
            Placed 16 as a key to calculate the size of the media
            Please determine the type of media, then the size of the
            media and fill hType.dwLength with the new value
         */
         /*
           if( pmd->dwSize == 16 ){
             uint32 nType;
             nType = GetMediaFormat(pmd->pData); // Write this function
             switch( nType ){
             case 0x444D4D4D:
               //calculate SMAF MMF data size, inline or in function
               pmd->dwSize = GetMMFSMAFSize(pmd->pData);// Write this function
             case 0x6468544D:
               // calculate MIDI data size, inline or in function
               pmd->dwSize = GetMidiSize(pmd->pData);// Write this function
             case ...: // Other OEM known formats
               pmd->dwSize = GetOtherSize(pmd->pData);// Write these functions
             ...
             }
          }
         */
         ph->mem.len = pmd->dwSize;
      }
      else
         nRet = EBADPARM;
   }

   if (pmd->clsData >= MMD_ISOURCE)
   {
      // Perform streaming....
      CMediaBg *  pBg = (CMediaBg *)pme;

      if (AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf ||
          AEECLSID_MEDIAMIDIOUTMSG == pme->m_clsSelf)
      {
          return EUNSUPPORTED;
      }

      if (OEMMM_ISSTREAMING(pBg))
         CMediaMMLayer_FlushStreamData(pBg);
      else
      {
         uint32   dwBufSize = pme->m_dwBufferSize ? pme->m_dwBufferSize : OEMMM_BUFFER_SIZE_MAX;
         FARF(SOURCE, ("===Src setup: Start"));
         if (CQueue_New(OEMMM_CMX_REQ_SIZE_MAX, TRUE, &pBg->m_pqCMXDataReq))
            return ENOMEMORY;

         if (CQueue_New(dwBufSize, FALSE, &pBg->m_pqMediaData))
            return ENOMEMORY;

         pBg->m_pcbCMXDataReq = MALLOC(sizeof(AEECallback));
         if (!pBg->m_pcbCMXDataReq)
            return ENOMEMORY;
         CALLBACK_Init(pBg->m_pcbCMXDataReq, (PFNNOTIFY)CMediaMMLayer_CMXDataReqNotify, (void *)pBg);

         pBg->m_pcbMediaData = MALLOC(sizeof(AEECallback));
         if (!pBg->m_pcbMediaData)
            return ENOMEMORY;
         CALLBACK_Init(pBg->m_pcbMediaData, (PFNNOTIFY)CMediaMMLayer_ReadSource, (void *)pBg);

         pBg->m_dwSrcBufferSize = MIN(OEMMM_BUFFER_UNIT_SIZE, dwBufSize);
         pBg->m_pSrcBuffer = MALLOC(pBg->m_dwSrcBufferSize);
         if (!pBg->m_pSrcBuffer)
            return ENOMEMORY;

         FARF(SOURCE, ("===Src setup: Done"));
         pBg->m_bStreaming = TRUE;
      }

      ph->client.source_type = ph->source_type = CMX_SOURCE_CLIENT;
      ph->client.data_config = CMX_CLIENT_DATA_REAL_TIME;
      ph->client.data_len = 0xFFFFffff;   // Set it to max.
      ph->client.data_req_ptr = CMediaMMLayer_DataRequestCB;

      CMediaMMLayer_ReadSource(pBg);
   }

#if defined (FEATURE_CMX_MM_API)
   if (SUCCESS == nRet)
   {
      nRet = OEMMedia_CreateSession(pme, ph);
   }
#endif // defined (FEATURE_CMX_MM_API)

   return nRet;
}

/*==================================================================

==================================================================*/
static void CMediaMMLayer_FlushStreamData(CMediaBg * pme)
{
   FARF(SOURCE, ("===Flushing stream data..."));

//   cmx_audio_playback_control(CMX_AUDIO_PB_CMD_FLUSH, NULL, NULL);

   // Cancel pending callbacks...
   CALLBACK_Cancel(pme->m_pcbMediaData);
   CALLBACK_Cancel(pme->m_pcbCMXDataReq);

   // Flush source data queue.
   CQueue_Flush(pme->m_pqMediaData);

   // Flush CMX buffers...
   while (!CQueue_IsEmpty(pme->m_pqCMXDataReq))
   {
      CMXDataRequest req;

      if (!CQueue_Get(pme->m_pqCMXDataReq, (uint8 *)&req, sizeof(req)))
         break;

      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_DATA_RESPONSE, (int32)&req, (int32)NULL, (int32)0, FALSE);
//      req.pfnDataReq(CMX_SUCCESS, NULL, 0, req.pServerUserData);
   }

   if (pme->m_bEOS)
      pme->m_bEOS = FALSE;
}

/*==================================================================

==================================================================*/
static void CMediaMMLayer_InitCallback(IMedia * po, int nCmd, AEEMediaCallback * pcb, int nMax, PFNNOTIFY pfn)
{
   AEEMedia *  pMedia = (AEEMedia *)po;
   int         i;

   for (i = 0; i < nMax; i++)
   {
      pcb[i].bInUse = FALSE;
      CALLBACK_Init(&pcb[i].cb, pfn, &pcb[i]);
      pcb[i].cmdNotify.clsMedia = pMedia->m_clsSelf;
      pcb[i].cmdNotify.pIMedia = po;
      pcb[i].cmdNotify.nCmd = nCmd;
   }
}

/*==================================================================

==================================================================*/
static AEEMediaCallback * CMediaMMLayer_AllocCallback(AEEMediaCallback * pcb, int nMax, int nCmd, int nSubCmd)
{
   AEEMediaCallback *   pcbRet = NULL;

   OEMMM_ENTER_CRITICAL_SECTION()

   int   i;

   for (i = 0; i < nMax; i++)
   {
      if (!pcb[i].bInUse)
      {
         pcb[i].bInUse = TRUE;
         pcb[i].cmdNotify.nCmd = nCmd;
         pcb[i].cmdNotify.nSubCmd = nSubCmd;
         pcb[i].cmdNotify.pCmdData = NULL;
         pcb[i].cmdNotify.dwSize = 0;
         pcbRet = &pcb[i];
         goto Done;
      }
   }

Done:
   OEMMM_LEAVE_CRITICAL_SECTION()

   pMediaCB = (void *)pcbRet;    //为手动调用回调CMediaBg_CallbackNotify 的参数赋值

   return pcbRet;
}

/*==================================================================

==================================================================*/
static void CMediaMMLayer_FreeCallback(AEEMediaCallback * pcb)
{
   OEMMM_ENTER_CRITICAL_SECTION()

   pcb->bInUse = FALSE;
   pcb->cmdNotify.nSubCmd = 0;
   pcb->cmdNotify.pCmdData = NULL;
   pcb->cmdNotify.dwSize = 0;

   OEMMM_LEAVE_CRITICAL_SECTION()
}

static void CMediaMMLayer_CancelCallback(AEEMediaCallback * pcb, int nMax)
{
   OEMMM_ENTER_CRITICAL_SECTION()

   int   i;

   for (i = 0; i < nMax; i++)
   {
      CALLBACK_Cancel(&pcb[i].cb);
   }

   OEMMM_LEAVE_CRITICAL_SECTION()
}

#if defined(FEATURE_AUDIO_FORMAT)

/*===========================================================================

                      CMX: MIDI, MP3, QCP

===========================================================================*/
static int CMediaMMLayer_GetMMStatus(CMediaCMX * pme, cmx_status_type status, cmx_af_server_data_type * psd, OEMMMStatus * peStatus);
static int CMediaMMLayer_GetMMStatus2(CMediaCMX * pme, cmx_status_type status, void* data, uint32 num_bytes, OEMMMStatus * peStatus);

static void CMediaMMLayer_ProcessPlayServerData(CMediaCMX * pme, cmx_status_type status, cmx_af_server_data_type * psd, AEEMediaCallback * pcb);

static void CMediaMMLayer_SetMediaSpec(CMediaBg * pme, cmx_af_audio_spec_type * psd, AEEMediaCallback * pcb);

/*==================================================================

==================================================================*/
static int CMediaMMLayer_Play(CMediaBg * pme)
{
   cmx_handle_type      h;
   AEEMediaCallback *   pcb;
   int nRet = SUCCESS;
   CMXInfo              ci;
   IShell * pIShell = AEE_GetShell();

   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;
#if 0   
   if(h.efs.filename != NULL)
   {
      VC_GPIO_CTRL(46, HI); // enable Speaker PA     //VC_GPIO_CTRL(49, HI); //  开耳机
      VC_GPIO_CTRL(44, HI); // enable speaker_sel for VC0848
      disppriv_set_window(0,0,176,220);
      MMD_SYS_PMU_SWITCH(PMU_MODE_NORMAL);
   }
#endif // if 0 debug for mp3 and mp4 play func   
   pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_PLAY, 0);
   if (!pcb)
      return EITEMBUSY;

   // If it is raw data, then select codec and start the playback...
   if (pme->m_bRaw)
      return CMediaMMLayer_PlayCodec(pme, &h, pcb);

   if (pme->m_nAPath != MM_APATH_LOCAL_RINGER)
   {

      cmx_audfmt_start_enum_type eType;
      pcb->cmdNotify.nStatus = MM_STATUS_START;

      AEE_ResumeCallback(&pcb->cb, pme->m_pac);

#ifdef FEATURE_MULTISEQUENCER
      /* See if we need to play a multisequence */
      if (CMediaMMLayer_IsMultiSequence(pme))
      {
         cmx_audfmt_multi_param_type sParam;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
         CMediaMMLayer_SetSynthQuality(pme);
#endif

         sParam.repeat_state = (1 == pme->m_dwRepeatCount) ? CMX_AUDFMT_REPEAT_DISABLED : CMX_AUDFMT_REPEAT_ENABLED;
         sParam.repeat_count = pme->m_dwRepeatCount;
         sParam.repeat_ms = 0;
         sParam.cb_type = CMX_AF_CALLBACK_VALUE;

         ci.nType = OEMCMX_TYPE_AUDFMT_PLAY_MULTI;
         ci.ph = &h;
         ci.dwCB = (uint32)CMediaMMLayer_PlayCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&sParam, 0, TRUE);
         nRet = OEMMedia_Start(pme->m_pMediaCMX);
      }
	  
      else
#endif  // FEATURE_MULTISEQUENCER
      {
         if (pme->m_clsSelf != AEECLSID_MEDIAQCP)
            eType = CMX_AUDFMT_START_NORMAL;
         else
         {
            if (pme->m_nAPath == MM_APATH_LOCAL)
               eType = CMX_AUDFMT_START_PATH_FORWARD;
            else if (pme->m_nAPath == MM_APATH_REMOTE)
               eType = CMX_AUDFMT_START_PATH_REVERSE;
            else if (pme->m_nAPath == MM_APATH_BOTH)
               eType = CMX_AUDFMT_START_PATH_BOTH;
            else
               eType = CMX_AUDFMT_START_NORMAL;
         }

         pcb->cmdNotify.nStatus = MM_STATUS_START;
         AEE_ResumeCallback(&pcb->cb, pme->m_pac);

         ci.nType = OEMCMX_TYPE_AUDFMT_PLAY;
         ci.ph = &h;
         ci.dwCB = (uint32)CMediaMMLayer_PlayCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)eType, 0, TRUE);
         nRet = OEMMedia_Start(pme->m_pMediaCMX);
      }
   }
   else
   {
      cmx_audfmt_start_enum_type eType = (pme->m_clsSelf == AEECLSID_MEDIAQCP) ? CMX_AUDFMT_START_PATH_FORWARD : CMX_AUDFMT_START_NORMAL;

      pcb->cmdNotify.nStatus = MM_STATUS_START;
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);

      ci.nType = OEMCMX_TYPE_AUDFMT_RINGER;
      ci.ph = &h;
      ci.dwCB = (uint32)CMediaMMLayer_RingerCB;
      ci.dwUserData = (uint32)pme->m_hObject;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)eType, (int32)pme->m_wSilenceTimeMS, TRUE);
      nRet = OEMMedia_Start(pme->m_pMediaCMX);
   }
   

   
   if(nRet != 0)
   	{
   	    pcb->cmdNotify.nStatus = MM_STATUS_ABORT;
	    return nRet;
   	}
   ISHELL_SetTimer(pIShell, 1000,(PFNNOTIFY)CMediaMMLayer_CallbackTimer,pcb);
   return nRet;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_PlayCodec(CMediaBg * pme, cmx_handle_type * ph, AEEMediaCallback * pcb)
{
#if defined(FEATURE_CMX_RAW_PLAYBACK)
   cmx_codec_spec_type  cs;
   int                  nErr = SUCCESS;

   FARF(MEDIA, ("===Play codec"));
   switch (pme->m_clsSelf)
   {
#if defined(FEATURE_QCP)
      case AEECLSID_MEDIAQCP:
      {
         int   nCodec;

         FARF(MEDIA, ("===Select QCP codec"));
         CMediaMMLayer_GetQCPFormat(pme->u.m_pQCP->m_nFormat, (int *)&nCodec);
         cs.qcp_codec.file_type = (cmx_af_file_enum_type)nCodec;
         if (-1 == cs.qcp_codec.file_type)
            nErr = EINVALIDFORMAT;
         break;
      }
#endif //defined(FEATURE_QCP)

#if defined(FEATURE_MP3)
      case AEECLSID_MEDIAMP3:
      {
         FARF(MEDIA, ("===Select MP3 codec"));
         cs.qcp_codec.file_type = CMX_AF_FILE_MP3;
         break;
      }
#endif //defined(FEATURE_MP3)

#if defined(FEATURE_CMX_LINEAR_PCM)
      case AEECLSID_MEDIAPCM: // Fall thru...
      case AEECLSID_MEDIAADPCM:
      {
         AEEMediaWaveSpec *   pws = (AEEMediaWaveSpec *)pme->m_pSpec;

         FARF(MEDIA, ("===Select Wave codec"));
         if (!pws || !pme->m_dwSpecSize)
            nErr = EBADPARM;
         else
         {
            cs.wave_codec.file_type       = CMX_AF_FILE_WAVE;
            cs.wave_codec.format          = CMX_AF_WAVE_FORMAT_LINEAR_PCM;
            cs.wave_codec.sample_format   = pws->bUnsigned ? CMX_AF_WAVE_SAMPLE_FMT_UNSIGNED : CMX_AF_WAVE_SAMPLE_FMT_SIGNED;
            cs.wave_codec.sample_rate     = pws->dwSamplesPerSec;
            cs.wave_codec.bits_per_sample = pws->wBitsPerSample;
            cs.wave_codec.channels        = pws->wChannels;
         }
         break;
      }
#endif

      default:
         nErr = EFAILED;
   }

   if (!nErr)
   {
      CMXInfo  ci;

      FARF(MEDIA, ("===Play codec"));

      ci.nType = OEMCMX_TYPE_AUDFMT_PLAY_CODEC;
      ci.ph = ph;
      ci.dwCB = (uint32)CMediaMMLayer_RingerCB;
      ci.dwUserData = (uint32)pme->m_hObject;

#if defined(FEATURE_CMX_AV_SYNC)
      if (pme->m_dwAudioSyncType)
      {
         cmx_av_sync_type  ast;

         if (!pme->m_pAudioSyncInfo)
         {
            pme->m_pAudioSyncInfo = MALLOC(sizeof(AEEMediaAudioSync));
            if (!pme->m_pAudioSyncInfo)
            {
               CMediaMMLayer_FreeCallback(pcb);
               return ENOMEMORY;
            }
         }

         ast.av_sync_interval = pme->m_dwAudioSyncData;
         ast.av_sync_mode = (MM_AUDIOSYNC_MODE_TIME == pme->m_dwAudioSyncType) ? CMX_AV_SYNC_TIME_MS : CMX_AV_SYNC_SAMPLES;
         ast.av_sync_cb_func = CMediaMMLayer_CMXAudioSyncCB;
         ast.client_data = (const void *)pme->m_hObject;

         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&cs, (int32)&ast, TRUE);
         OEMMedia_Start(pme->m_pMediaCMX);
      }
      else
#endif // defined(FEATURE_CMX_AV_SYNC)
      {
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&cs, 0, TRUE);
         OEMMedia_Start(pme->m_pMediaCMX);
      }

      pcb->cmdNotify.nStatus = MM_STATUS_START;
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
   }
   else
      CMediaMMLayer_FreeCallback(pcb);

   return nErr;

#else

   return EUNSUPPORTED;

#endif // defined(FEATURE_CMX_RAW_PLAYBACK)
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_Stop(CMediaBg * pme)
{
   AEEMediaCallback *   pcb = &pme->m_cbCommand;
//   int                  nCmd;

   // Callback must always be available for stop command.
   CMediaMMLayer_FreeCallback(pcb);
//   nCmd = AEEMedia_IsPlayOn(pme) ? MM_CMD_PLAY : MM_CMD_RECORD;
//   CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, nCmd, OEMMM_CTLCMD_STOP);

   // Media is stopping...do not process callback data.
   pme->m_bStopping = TRUE;
   if (pme->m_clsSelf == AEECLSID_MEDIAQCP && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_QCP)
      OEMMedia_Stop(pme->m_pMediaCMX);
#else
      return EFAILED;
#endif // defined(FEATURE_QCP)
   }
   else if (((pme->m_clsSelf == AEECLSID_MEDIAPCM) || (pme->m_clsSelf == AEECLSID_MEDIAADPCM)) && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_PCM_REC)
      cmx_pcm_rec_link_type link;

      if (pme->m_nAPath == MM_APATH_LOCAL)
         link = CMX_PCM_REC_LINK_REVERSE;
      else if (pme->m_nAPath == MM_APATH_REMOTE)
         link = CMX_PCM_REC_LINK_FORWARD;
      else
         link = CMX_PCM_REC_LINK_INVALID;

      if (link != CMX_PCM_REC_LINK_INVALID)
      {
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_LINK, (int32)link, 0, 0, TRUE);
      }
   else
   {
         return EBADPARM;
      }

      OEMMedia_Stop(pme->m_pMediaCMX);
#else
      return EFAILED;
#endif // defined(FEATURE_PCM_REC)
   }
   else if (pme->m_clsSelf == AEECLSID_MEDIAAAC && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_AAC_REC)
	OEMMedia_Stop(pme->m_pMediaCMX);
#else
      return EFAILED;
#endif // defined(FEATURE_AAC_REC)
   }
   else
   {
#ifdef FEATURE_MULTISEQUENCER
      /* Check if it is a multisequence */
      if (CMediaMMLayer_IsMultiSequence(pme))
      {
          cmx_audfmt_seq_param_type   seqParam;
          seqParam.cmd = CMX_AUDFMT_SEQ_CMD_STOP;
          seqParam.num = 0;
          OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, TRUE);
      }
#endif  // FEATURE_MULTISEQUENCER

      OEMMedia_Stop(pme->m_pMediaCMX);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_Seek(CMediaBg * pme, AEEMediaSeek eSeek, int32 lTimeMS)
{
   AEEMediaCallback *   pcb;
   AEEMedia * DpUser;
   IShell * pIShell = AEE_GetShell();
   int nRet;
   
   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_PLAY, OEMMM_CTLCMD_SEEK);
   if (!pcb)
      return EITEMBUSY;

   switch (eSeek)
   {
      case MM_SEEK_CURRENT:
#ifdef FEATURE_MULTISEQUENCER
          /* See if it is a multisequence */
          if (CMediaMMLayer_IsMultiSequence(pme))
          {
             cmx_audfmt_seq_param_type   seqParam;
             seqParam.cmd = lTimeMS < 0 ? CMX_AUDFMT_SEQ_CMD_REWIND : CMX_AUDFMT_SEQ_CMD_FFORWARD;
             seqParam.num = lTimeMS < 0 ? -lTimeMS : lTimeMS;
             OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, TRUE);
          }
#endif  // FEATURE_MULTISEQUENCER

         nRet = OEMMedia_Seek(pme->m_pMediaCMX, MM_SEEK_CURRENT, lTimeMS);
         break;

      case MM_SEEK_START:
      case MM_SEEK_END:
      {
         CMediaMMLayer_FreeCallback(pcb);
	  DpUser = (AEEMedia *)pme;
	  DpUser->m_nState = MM_STATE_IDLE;
         return EBADSTATE;
      }
   }

   if(nRet != 0) 
    {
       return nRet;
    }
   pcb->cmdNotify.nCmd = MM_CMD_PLAY;
   pcb->cmdNotify.nStatus = MM_STATUS_SEEK;
   CMediaBg_CallbackNotify(pcb);
   
   pcb->cmdNotify.nCmd = MM_CMD_PLAY;
   pcb->cmdNotify.nStatus = MM_STATUS_RESUME;
   CMediaBg_CallbackNotify(pcb);
   
   ISHELL_SetTimer(pIShell, 1000,(PFNNOTIFY)CMediaMMLayer_CallbackTimer,pcb);   

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_Pause(CMediaBg * pme)
{
   AEEMediaCallback *   pcb;
   int                  nCmd;
   AEEMedia * DpUser = (AEEMedia *)pMediaCB;
   static vc_union_type vc_data;

   nCmd = AEEMedia_IsPlayOn(pme) ? MM_CMD_PLAY : MM_CMD_RECORD;
   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, nCmd, OEMMM_CTLCMD_PAUSE);
   if (!pcb)
      return EITEMBUSY;

   if (pme->m_clsSelf == AEECLSID_MEDIAQCP && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_QCP)
      OEMMedia_Pause(pme->m_pMediaCMX);
#else
      return EFAILED;
#endif // defined(FEATURE_QCP)
   }
   else if (((pme->m_clsSelf == AEECLSID_MEDIAPCM) || (pme->m_clsSelf == AEECLSID_MEDIAADPCM)) && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_PCM_REC)
      cmx_pcm_rec_link_type link;

      if (pme->m_nAPath == MM_APATH_LOCAL)
         link = CMX_PCM_REC_LINK_REVERSE;
      else if (pme->m_nAPath == MM_APATH_REMOTE)
         link = CMX_PCM_REC_LINK_FORWARD;
   else
         link = CMX_PCM_REC_LINK_INVALID;

      if (link != CMX_PCM_REC_LINK_INVALID)
      {
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_LINK, (int32)link, 0, 0, TRUE);
      }
      else
      {
         return EBADPARM;
      }

      OEMMedia_Pause(pme->m_pMediaCMX);
      
      /*
      nRet = MMD_Player_File_Pause(mpFile);
      if(nRet)
      {
         nRet = MMD_Player_File_Close(mpFile,NULL,NULL);
         return nRet;
      }
      */
#else
      return EFAILED;
#endif // defined(FEATURE_PCM_REC)
   }
   else if (pme->m_clsSelf == AEECLSID_MEDIAAAC && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_AAC_REC)
      OEMMedia_Pause(pme->m_pMediaCMX);

      /*
      nRet = MMD_Player_File_Pause(mpFile);
      if(nRet)
      {
         nRet = MMD_Player_File_Close(mpFile,NULL,NULL);
         return nRet;
      }
      */
#else
      return EFAILED;
#endif // defined(FEATURE_AAC_REC)
   }
   else
   {
#ifdef FEATURE_MULTISEQUENCER
      /* See if it is a multisequence */
      if (CMediaMMLayer_IsMultiSequence(pme))
   {
         cmx_audfmt_seq_param_type   seqParam;
         seqParam.cmd = CMX_AUDFMT_SEQ_CMD_PAUSE;
         seqParam.num = 0;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, TRUE);
      }
#endif
      pcb->cmdNotify.nStatus = MM_STATUS_START;
      OEMMedia_Pause(pme->m_pMediaCMX);
      /*
      nRet = MMD_Player_File_Pause(mpFile);
      if(nRet)
      {
         nRet = MMD_Player_File_Close(mpFile,NULL,NULL);
         return nRet;
      }
      */
      }
      VC_DeviceControl(VC_ITM_GET_PLAY_TIME_I, VC_FUNC_GET_PARM, &vc_data);
      pcb->cmdNotify.pCmdData = (void*)vc_data.parm.p1;
      // Add CallBack   手动回调
     DpUser->m_nState = MM_STATE_PLAY_PAUSE;
     pcb->cmdNotify.nStatus = MM_STATUS_PAUSE;
     CMediaBg_CallbackNotify(pMediaCB);
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_Resume(CMediaBg * pme)
{
   AEEMediaCallback *   pcb;
   int                  nCmd;
   IShell * pIShell = AEE_GetShell();

   nCmd = AEEMedia_IsPlayOn(pme) ? MM_CMD_PLAY : MM_CMD_RECORD;
   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, nCmd, OEMMM_CTLCMD_RESUME);
   if (!pcb)
      return EITEMBUSY;

   if (pme->m_clsSelf == AEECLSID_MEDIAQCP && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_QCP)
     OEMMedia_Resume(pme->m_pMediaCMX);
      /*
      nRet = MMD_Player_File_Resume(mpFile);
      if(nRet)
      {
         nRet = MMD_Player_File_Close(mpFile,NULL,NULL);
         return nRet;
      }
      */
#else
      return EFAILED;
#endif // defined(FEATURE_QCP)
   }
   else if (((pme->m_clsSelf == AEECLSID_MEDIAPCM) || (pme->m_clsSelf == AEECLSID_MEDIAADPCM)) && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_PCM_REC)
      cmx_pcm_rec_link_type link;

      if (pme->m_nAPath == MM_APATH_LOCAL)
         link = CMX_PCM_REC_LINK_REVERSE;
      else if (pme->m_nAPath == MM_APATH_REMOTE)
         link = CMX_PCM_REC_LINK_FORWARD;
      else
         link = CMX_PCM_REC_LINK_INVALID;

      if (link != CMX_PCM_REC_LINK_INVALID)
      {
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_LINK, (int32)link, 0, 0, TRUE);
      }
   else
   {
         return EBADPARM;
      }

      OEMMedia_Resume(pme->m_pMediaCMX);
      /*
      nRet = MMD_Player_File_Resume(mpFile);
      if(nRet)
      {
         nRet = MMD_Player_File_Close(mpFile,NULL,NULL);
         return nRet;
      }
      */
#else
      return EFAILED;
#endif // defined(FEATURE_PCM_REC)
   }
   else if (pme->m_clsSelf == AEECLSID_MEDIAAAC && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_AAC_REC)
      OEMMedia_Resume(pme->m_pMediaCMX);
      /*
      nRet = MMD_Player_File_Resume(mpFile);
      if(nRet)
      {
         nRet = MMD_Player_File_Close(mpFile,NULL,NULL);
         return nRet;
      }
      */
#else
      return EFAILED;
#endif // defined(FEATURE_AAC_REC)
   }
   else
   {
#ifdef FEATURE_MULTISEQUENCER
      if (CMediaMMLayer_IsMultiSequence(pme))
      {
         cmx_audfmt_seq_param_type   seqParam;
         seqParam.cmd = CMX_AUDFMT_SEQ_CMD_RESUME;
         seqParam.num = 0;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, TRUE);
      }
#endif  // FEATURE_MULTISEQUENCER
      OEMMedia_Resume(pme->m_pMediaCMX);
      /*
      nRet = MMD_Player_File_Resume(mpFile);
      if(nRet)
      {
         nRet = MMD_Player_File_Close(mpFile,NULL,NULL);
         return nRet;
      }
      */
   }
   // Add CallBack   手动回调
   pcb->cmdNotify.nCmd = MM_CMD_PLAY;
   pcb->cmdNotify.nStatus = MM_STATUS_RESUME;
   ISHELL_SetTimer(pIShell, 50,(PFNNOTIFY)CMediaBg_CallbackNotify,pcb);   
   ISHELL_SetTimer(pIShell, 1000,(PFNNOTIFY)CMediaMMLayer_CallbackTimer,pcb);
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_GetTotalTime(CMediaBg * pme)
{
   cmx_handle_type      h;
   AEEMediaCallback *   pcb;
   int                  nRet = SUCCESS;
   CMXInfo              ci;

#if defined (FEATURE_CMX_MM_API)
   if (!pme->m_pMediaCMX->hsession)
#endif // defined (FEATURE_CMX_MM_API)
   {
   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;
   }

   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbGetTotalTime, 1, MM_CMD_GETTOTALTIME, 0);
   if (!pcb)
      return EITEMBUSY;

   ci.nType = OEMCMX_TYPE_AUDFMT_PLAY;
   ci.ph = &h;
   ci.dwCB = (uint32)CMediaMMLayer_TimeCB;
   ci.dwUserData = (uint32)pme->m_hObject;
   OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TOTALTIME, (int32)&ci, 0, 0, FALSE);

   return nRet;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_SetTempo(CMediaBg * pme, uint32 dwTempo)
{
   AEEMediaCallback *   pcb;

   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_TEMPO);
   if (!pcb)
      return EITEMBUSY;

   pme->m_wTempo = (uint16)dwTempo;

#ifdef FEATURE_MULTISEQUENCER
   if (CMediaMMLayer_IsMultiSequence(pme))
   {
      cmx_audfmt_seq_param_type seqParam;
      seqParam.cmd = CMX_AUDFMT_SEQ_CMD_TEMPO;
      seqParam.num = dwTempo;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, FALSE);
   }
   else
#endif  // FEATURE_MULTISEQUENCER

   OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TEMPO, (int32)dwTempo, 0, 0, FALSE);

   return MM_PENDING;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_SetTune(CMediaBg * pme, uint32 dwTune)
{
   AEEMediaCallback *   pcb;

   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_TUNE);
   if (!pcb)
      return EITEMBUSY;

   pme->m_wTune = (uint16)dwTune;

#ifdef FEATURE_MULTISEQUENCER
   if (CMediaMMLayer_IsMultiSequence(pme))
   {
      cmx_audfmt_seq_param_type seqParam;
      seqParam.cmd = CMX_AUDFMT_SEQ_CMD_TUNE;
      seqParam.num = dwTune;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, FALSE);
   }
   else
#endif  // FEATURE_MULTISEQUENCER
   OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TUNE, (int32)dwTune, 0, 0, FALSE);

   return MM_PENDING;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_SetPan(CMediaBg * pme, uint32 dwPan)
{
   AEEMediaCallback *   pcb;

#ifdef FEATURE_MULTISEQUENCER    // Pan is currently unsupported
   if (CMediaMMLayer_IsMultiSequence(pme))
   {
      return EUNSUPPORTED;
   }
#endif // FEATURE_MULTISEQUENCER

   if (dwPan > MM_MAX_PAN)
      return EBADPARM;

   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_PAN);
   if (!pcb)
      return EITEMBUSY;

#if !defined(AEE_SIMULATOR)
   pme->m_wPan = (uint16)dwPan;

   OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_PAN, (int32)dwPan, 0, 0, FALSE);

   return MM_PENDING;
#else
   CMediaMMLayer_FreeCallback(pcb);
   return EFAILED;
#endif //!defined(AEE_SIMULATOR)
}

static int CMediaMMLayer_QCPRecord(CMediaBg * pme, int nFormat)
{
#if !defined(FEATURE_QCP)
   return EFAILED;
#else
   cmx_handle_type      h;
   AEEMediaCallback *   pcb;
   int                  nRet = SUCCESS;
   cmx_qcp_format_type  fmt = (cmx_qcp_format_type)CMediaMMLayer_GetQCPFormat(nFormat, NULL);

   if (fmt >= (int)CMX_QCP_FORMAT_INVALID)
      return EBADPARM;

   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

   pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_RECORD, 0);
   if (!pcb)
      return EITEMBUSY;

#if defined(FEATURE_BREW_CMX_V3)
   {
      cmx_qcp_rec_para_type       qcprecParam;
      qcprecParam.auto_stop_ms = 0;
      qcprecParam.report_ms = pme->m_dwTickTimeMS;
      qcprecParam.format = fmt;

      if (pme->m_nAPath == MM_APATH_LOCAL || pme->m_nAPath == MM_APATH_REMOTE)
      {
         CMXInfo  ci;

         ci.nType = OEMCMX_TYPE_QCP_RECORD;
         ci.ph = &h;
         ci.dwCB = (uint32)CMediaMMLayer_RecordCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&qcprecParam, (pme->m_nAPath == MM_APATH_LOCAL), TRUE);
         OEMMedia_Start(pme->m_pMediaCMX);
      }
      else
      {
         CMediaMMLayer_FreeCallback(pcb);
         return EBADPARM;
      }
   }
#else // CMX 2.x
   if (pme->m_nAPath == MM_APATH_LOCAL)
      cmx_qcp_record_reverse(&h, fmt, pme->m_dwTickTimeMS, CMediaMMLayer_RecordCB, (void *)pme->m_hObject);
   else if (pme->m_nAPath == MM_APATH_REMOTE)
      cmx_qcp_record_forward(&h, fmt, pme->m_dwTickTimeMS, CMediaMMLayer_RecordCB, (void *)pme->m_hObject);
   else
   {
      CMediaMMLayer_FreeCallback(pcb);
      return EBADPARM;
   }
#endif // defined(FEATURE_BREW_CMX_V3)

   pcb->cmdNotify.nStatus = MM_STATUS_START;
   AEE_ResumeCallback(&pcb->cb, pme->m_pac);

   return SUCCESS;
#endif // !defined(FEATURE_QCP)
}

//$r$
#if defined(FEATURE_QCP)
static int CMediaMMLayer_GetQCPFormat(int nFormat, int * pCodec)
{
   int   fmt;
   int   nCodec = -1;

#if defined(FEATURE_BREW_CMX_V3)
   switch (nFormat)
   {
      case MM_QCP_FORMAT_FIXED_FULL_13K:  fmt = CMX_QCP_FORMAT_FIXED_FULL_13K;   nCodec = CMX_AF_FILE_QCP_13K; break;
      case MM_QCP_FORMAT_VAR_FULL_13K:    fmt = CMX_QCP_FORMAT_VAR_FULL_13K;     nCodec = CMX_AF_FILE_QCP_13K; break;
      case MM_QCP_FORMAT_VAR_HALF_13K:    fmt = CMX_QCP_FORMAT_VAR_HALF_13K;     nCodec = CMX_AF_FILE_QCP_13K; break;
      case MM_QCP_FORMAT_FIXED_HALF_13K:  fmt = CMX_QCP_FORMAT_FIXED_HALF_13K;   nCodec = CMX_AF_FILE_QCP_13K; break;
      case MM_QCP_FORMAT_FIXED_FULL_EVRC: fmt = CMX_QCP_FORMAT_FIXED_FULL_EVRC;  nCodec = CMX_AF_FILE_QCP_EVRC; break;
      case MM_QCP_FORMAT_VAR_FULL_EVRC:   fmt = CMX_QCP_FORMAT_VAR_FULL_EVRC;    nCodec = CMX_AF_FILE_QCP_EVRC; break;

      case MM_QCP_FORMAT_FIXED_4_75_AMR:  fmt = CMX_QCP_FORMAT_FIXED_4_75_AMR;   nCodec = CMX_AF_FILE_QCP_AMR; break;
      case MM_QCP_FORMAT_FIXED_5_15_AMR:  fmt = CMX_QCP_FORMAT_FIXED_5_15_AMR;   nCodec = CMX_AF_FILE_QCP_AMR; break;
      case MM_QCP_FORMAT_FIXED_5_9_AMR :  fmt = CMX_QCP_FORMAT_FIXED_5_9_AMR ;   nCodec = CMX_AF_FILE_QCP_AMR; break;
      case MM_QCP_FORMAT_FIXED_6_7_AMR :  fmt = CMX_QCP_FORMAT_FIXED_6_7_AMR ;   nCodec = CMX_AF_FILE_QCP_AMR; break;
      case MM_QCP_FORMAT_FIXED_7_4_AMR :  fmt = CMX_QCP_FORMAT_FIXED_7_4_AMR ;   nCodec = CMX_AF_FILE_QCP_AMR; break;
      case MM_QCP_FORMAT_FIXED_7_95_AMR:  fmt = CMX_QCP_FORMAT_FIXED_7_95_AMR;   nCodec = CMX_AF_FILE_QCP_AMR; break;
      case MM_QCP_FORMAT_FIXED_10_2_AMR:  fmt = CMX_QCP_FORMAT_FIXED_10_2_AMR;   nCodec = CMX_AF_FILE_QCP_AMR; break;
      case MM_QCP_FORMAT_FIXED_12_2_AMR:  fmt = CMX_QCP_FORMAT_FIXED_12_2_AMR;   nCodec = CMX_AF_FILE_QCP_AMR; break;
#ifdef FEATURE_AUDFMT_AMR
      case MM_AMR_FORMAT_FIXED_4_75_AMR:  fmt = CMX_AMR_FORMAT_FIXED_4_75_AMR;   break;
      case MM_AMR_FORMAT_FIXED_5_15_AMR:  fmt = CMX_AMR_FORMAT_FIXED_5_15_AMR;   break;
      case MM_AMR_FORMAT_FIXED_5_9_AMR :  fmt = CMX_AMR_FORMAT_FIXED_5_9_AMR ;   break;
      case MM_AMR_FORMAT_FIXED_6_7_AMR :  fmt = CMX_AMR_FORMAT_FIXED_6_7_AMR ;   break;
      case MM_AMR_FORMAT_FIXED_7_4_AMR :  fmt = CMX_AMR_FORMAT_FIXED_7_4_AMR ;   break;
      case MM_AMR_FORMAT_FIXED_7_95_AMR:  fmt = CMX_AMR_FORMAT_FIXED_7_95_AMR;   break;
      case MM_AMR_FORMAT_FIXED_10_2_AMR:  fmt = CMX_AMR_FORMAT_FIXED_10_2_AMR;   break;
      case MM_AMR_FORMAT_FIXED_12_2_AMR:  fmt = CMX_AMR_FORMAT_FIXED_12_2_AMR;   break;
#endif
      default:                            fmt = CMX_QCP_FORMAT_INVALID;          break;
   }
#else // CMX 2.x
   fmt = nFormat;
#endif // defined(FEATURE_BREW_CMX_V3)

   if (pCodec)
      *pCodec = nCodec;

   return fmt;
}
#endif // defined(FEATURE_QCP)

static int CMediaMMLayer_PCMRecord(CMediaBg * pme)
{
#if !defined(FEATURE_PCM_REC)
   return EFAILED;
#else
   cmx_handle_type      h;
   AEEMediaCallback *   pcb;
   int                  nRet = SUCCESS;
   cmx_pcm_rec_para_type       pcmrecParam;

   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

   pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_RECORD, 0);
   if (!pcb)
      return EITEMBUSY;

   if (pme->m_nAPath == MM_APATH_LOCAL)
      pcmrecParam.link = CMX_PCM_REC_LINK_REVERSE;
   else if (pme->m_nAPath == MM_APATH_REMOTE)
      pcmrecParam.link = CMX_PCM_REC_LINK_FORWARD;
   else
      pcmrecParam.link = CMX_PCM_REC_LINK_INVALID;

   if (pcmrecParam.link != CMX_PCM_REC_LINK_INVALID)
   {
      CMXInfo  ci;

      ci.nType = OEMCMX_TYPE_PCM_RECORD;
      ci.ph = &h;
      ci.dwCB = (uint32)CMediaMMLayer_RecordCB;
      ci.dwUserData = (uint32)pme->m_hObject;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&pcmrecParam, 0, TRUE);
      OEMMedia_Start(pme->m_pMediaCMX);
   }
   else
   {
      CMediaMMLayer_FreeCallback(pcb);
      return EBADPARM;
   }

   pcb->cmdNotify.nStatus = MM_STATUS_START;
   AEE_ResumeCallback(&pcb->cb, pme->m_pac);

   return SUCCESS;
#endif // !defined(FEATURE_PCM_REC)
}

#if defined(FEATURE_AAC_REC)
//
// CMX AAC Recording functions.
//
/*==================================================================

==================================================================*/
static int CMediaMMLayer_AACRecord(CMediaBg * pme, int nFormat)
{
   cmx_handle_type      h;
   AEEMediaCallback *   pcb;
   int                  nRet = SUCCESS;
   cmx_mm_rec_param_type       aacrecParam;

   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

   pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_RECORD, 0);
   if (!pcb)
      return EITEMBUSY;

   if (pme->m_nAPath == MM_APATH_LOCAL)
      aacrecParam.mm_rec_link  = CMX_MM_REC_LINK_REVERSE;
   else if (pme->m_nAPath == MM_APATH_REMOTE)
      aacrecParam.mm_rec_link = CMX_MM_REC_LINK_FORWARD;
   else
      aacrecParam.mm_rec_link = CMX_MM_REC_LINK_INVALID;

   aacrecParam.mm_rec_param.mm_rec_format = CMX_MM_FORMAT_AAC;
   aacrecParam.mm_rec_param.cmx_mm_rec_aac_para.mm_rec_format = CMX_MM_FORMAT_AAC;
   aacrecParam.mm_rec_param.cmx_mm_rec_aac_para.aac_file_format = nFormat;
   aacrecParam.mm_rec_param.cmx_mm_rec_aac_para.sample_rate = CMX_AF_SAMPLE_RATE_8000;
   aacrecParam.mm_rec_param.cmx_mm_rec_aac_para.sync_cb_func = NULL;
   aacrecParam.mm_rec_param.cmx_mm_rec_aac_para.bit_per_spl=0x1C00;
   aacrecParam.mm_rec_param.cmx_mm_rec_aac_para.chan = CMX_AF_AAC_CHANNEL_MONO;

   if(aacrecParam.mm_rec_link != CMX_MM_REC_LINK_INVALID)
   {
      CMXInfo  ci;

      ci.nType = OEMCMX_TYPE_AAC_RECORD;
      ci.ph = &h;
      ci.dwCB = (uint32)CMediaMMLayer_RecordCB;
      ci.dwUserData = (uint32)pme->m_hObject;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&aacrecParam, (int32)(pme->m_nAPath == MM_APATH_LOCAL), TRUE);
      OEMMedia_Start(pme->m_pMediaCMX);
   }
   else
   {
      CMediaMMLayer_FreeCallback(pcb);
      return EBADPARM;
   }

   pcb->cmdNotify.nStatus = MM_STATUS_START;
   AEE_ResumeCallback(&pcb->cb, pme->m_pac);

   return SUCCESS;
}
#endif // defined(FEATURE_AAC_REC)

static int CMediaMMLayer_AMRRecord(CMediaBg * pme)
{
/*
#if !defined(FEATURE_AUDFMT_AMR)
   return EFAILED;
#else
   cmx_handle_type      h;
   AEEMediaCallback *   pcb;
   int                  nRet = SUCCESS;
   cmx_pcm_rec_para_type       amrrecParam;
   RMEDIA_FILE  rfileinfor;
   MMD_U32  mrFile;

   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

   pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_RECORD, 0);
   if (!pcb)
      return EITEMBUSY;

//  if (pme->m_nAPath == MM_APATH_LOCAL)
//      amrrecParam.link = CMX_PCM_REC_LINK_REVERSE;
//   else if (pme->m_nAPath == MM_APATH_REMOTE)
//      amrrecParam.link = CMX_PCM_REC_LINK_FORWARD;
//   else
//      amrrecParam.link = CMX_PCM_REC_LINK_INVALID;

   if (amrrecParam.link != CMX_PCM_REC_LINK_INVALID)
   {
      CMXInfo  ci;

      ci.nType = OEMCMX_TYPE_PCM_RECORD;
      ci.ph = &h;
      ci.dwCB = (uint32)CMediaMMLayer_RecordCB;
      ci.dwUserData = (uint32)pme->m_hObject;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&amrrecParam, 0, TRUE);
//      OEMMedia_Start(pme->m_pMediaCMX);
      mrFile = *((MMD_U32*)pme->m_md.pData);
//      mrFile = 
      rfileinfor.a_recchn = 1;
      rfileinfor.a_recbits = 16;
      rfileinfor.a_recsmplrate = 8000;
      rfileinfor.v_recwidth = 320;
      rfileinfor.v_recheight = 240;
      rfileinfor.v_recfps = 15;
      rfileinfor.a_codec = MEDIA_AUDIO_TYPE_AMR;
      rfileinfor.fs_filetype = MMD_MEDIA_FILE_TYPE_AMR;
      nRet = MMD_Recorder_File_Creat(NULL,&rfileinfor,&mrFile);
	  if(nRet)
	  	return nRet;
     nRet = MMD_Recorder_File_Start(mrFile, NULL);
	  if(nRet)
	  	{
	  	    nRet = MMD_Recorder_File_Close(NULL,mrFile);
	  	}
   else
   {
      CMediaMMLayer_FreeCallback(pcb);
      return EBADPARM;
   }

   pcb->cmdNotify.nStatus = MM_STATUS_START;
   AEE_ResumeCallback(&pcb->cb, pme->m_pac);

   return SUCCESS;
#endif // !defined(FEATURE_PCM_REC)
*/
return EUNSUPPORTED;
}

//
// CMX callback functions. These functions are called in CMX context.
//
/*==================================================================

==================================================================*/
void CMediaMMLayer_PlayCB(cmx_status_type status, const void * pClientData, cmx_af_server_data_type * psd)
{
   CMediaBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   OEMMMStatus          eStatus;
   int                  nStatus = MM_STATUS_UNKNOWN;

   pme = (CMediaBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   nStatus = CMediaMMLayer_GetMMStatus((CMediaCMX *)pme, status, psd, &eStatus);
   if (nStatus == MM_STATUS_UNKNOWN)
      goto OEMMM_CBExit;

   // For playctl command, cbCommand was reserved...free it since
   // SEEK/PAUSE/RESUME/STOP status is sent thru play cb.
   if (eStatus == OEMMM_STATUS_PLAYCTL && CMediaMMLayer_ISPLAYCTL(&pme->m_cbCommand))
   {
      CMediaMMLayer_FreeCallback(&pme->m_cbCommand);
   }

   // For set parm event (tempo, tune, pan), cbCommand is sent in AEE_ResumeCallback().
   // Else for other playback events, one of cbPlay is sent in AEE_ResumeCallback().
   if (eStatus == OEMMM_STATUS_SETPARM) // || eStatus == OEMMM_STATUS_PLAYCTL)
      pcb = &pme->m_cbCommand;
   else
      pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_PLAY, 0);

   if (pcb)
   {
      pcb->cmdNotify.nStatus = nStatus;
      CMediaMMLayer_ProcessPlayServerData((CMediaCMX *)pme, status, psd, pcb);

      bResume = TRUE;
   }

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
}

/*==================================================================

==================================================================*/
void CMediaMMLayer_RingerCB(cmx_status_type status, const void * pClientData)
{
   CMediaMMLayer_PlayCB(status, pClientData, NULL);
}

#if defined(FEATURE_CMX_AV_SYNC)
/*==================================================================

==================================================================*/
static void CMediaMMLayer_CMXAudioSyncCB(qword timestamp, qword num_of_samples, qword num_of_bytes, const void  *client_data)
{
   CMediaBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = (CMediaBg *)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);
   if (!pme)
      goto OEMMM_CBExit;

   if (pme->m_pAudioSyncInfo)
   {
      pme->m_pAudioSyncInfo->ullTimeStampMS = QW_CVT_Q2N(timestamp);
      pme->m_pAudioSyncInfo->ullSamples = QW_CVT_Q2N(num_of_samples);
   }
   pme->m_qwAudioSyncBytes = QW_CVT_Q2N(num_of_bytes);

   pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_PLAY, 0);
   if (pcb)
   {
      pcb->cmdNotify.nStatus = MM_STATUS_AUDIOSYNC;
      pcb->cmdNotify.pCmdData = pme->m_pAudioSyncInfo;
      pcb->cmdNotify.dwSize = sizeof(AEEMediaAudioSync);
      bResume = TRUE;
   }

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
}
#endif // defined(FEATURE_CMX_AV_SYNC)

/*==================================================================

==================================================================*/
void CMediaMMLayer_StatusCB(cmx_status_type status, const void * pClientData)
{
   CMediaBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   int   nStatus = MM_STATUS_UNKNOWN;

   pme = (CMediaBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   pcb = &pme->m_cbCommand;
   if (!pcb->bInUse)
      goto OEMMM_CBExit;

   nStatus = CMediaMMLayer_GetMMStatus((CMediaCMX *)pme, status, NULL, NULL);
   if (nStatus == MM_STATUS_UNKNOWN)
      goto OEMMM_CBExit;

   // Check if it is playctl cmd event. If it is and if status is MM_STATUS_ABORT,
   // the status must be mapped to one of SEEK_FAIL/PAUSE_FAIL/RESUME_FAIL
   // bases on nSubCmd.
   if (nStatus == MM_STATUS_ABORT && CMediaMMLayer_ISPLAYCTL(pcb))
   {
      int   nSubCmd = pcb->cmdNotify.nSubCmd;
      pcb->cmdNotify.nSubCmd = 0;   // Do not send this subcmd to user.

      if (nSubCmd == OEMMM_CTLCMD_SEEK)
         nStatus = MM_STATUS_SEEK_FAIL;
      else if (nSubCmd == OEMMM_CTLCMD_PAUSE)
         nStatus = MM_STATUS_PAUSE_FAIL;
      else if (nSubCmd == OEMMM_CTLCMD_RESUME)
         nStatus = MM_STATUS_RESUME_FAIL;
   }

   pcb->cmdNotify.nStatus = nStatus;

   bResume = TRUE;

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
}

/*==================================================================

==================================================================*/
void CMediaMMLayer_TimeCB(cmx_status_type status, const void * pClientData, uint32 dwTimeMS)
{
   CMediaBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   int                  nStatus = MM_STATUS_UNKNOWN;
   pme = (CMediaBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   pcb = &pme->m_cbGetTotalTime;

   nStatus = CMediaMMLayer_GetMMStatus((CMediaCMX *)pme, status, NULL, NULL);
   if (nStatus == MM_STATUS_UNKNOWN)
      goto OEMMM_CBExit;

   pcb->cmdNotify.nStatus = nStatus;

   if (nStatus == MM_STATUS_DONE)
   {
      pcb->cmdNotify.pCmdData = (void *)dwTimeMS;
      pcb->cmdNotify.dwSize = sizeof(uint32);
   }

   bResume = TRUE;

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);

}

/*==================================================================

==================================================================*/
#if defined(FEATURE_QCP) || defined(FEATURE_PCM_REC)|| defined(FEATURE_AAC_REC)
void CMediaMMLayer_RecordCB(cmx_status_type status, const void * pClientData)
{
   CMediaBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   int                  nStatus = MM_STATUS_UNKNOWN;
   OEMMMStatus          eStatus;

   pme = (CMediaBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   nStatus = CMediaMMLayer_GetMMStatus((CMediaCMX *)pme, status, NULL, &eStatus);
   if (nStatus == MM_STATUS_UNKNOWN)
      goto OEMMM_CBExit;

   if (eStatus == OEMMM_STATUS_PLAYCTL)
      pcb = &pme->m_cbCommand;
   else
      pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_RECORD, 0);

   if (pcb)
   {
      pcb->cmdNotify.nStatus = nStatus;
      bResume = TRUE;
   }

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
}
#endif // defined(FEATURE_QCP) || defined(FEATURE_PCM_REC)

/*==================================================================

==================================================================*/
#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
static int CMediaMMLayer_SetSynthQuality(CMediaBg *pme)
{
   int   nErr;
   int   nCurrentMode;
   int   nNewMode;

   if (!gpISynthQuality)
      return EFAILED;

   // Get the current mode.
   nErr = ISYNTHQUALITY_GetSynthMode(gpISynthQuality, &nCurrentMode);
   if (nErr)
      goto Done;

   // Compute the new mode.
   if (!gbNotesSet || gdwNotes >= OEMMM_MAX_NOTES)
      nNewMode = AEE_SYNTH_MODE_HP_ALL;
   else if (0 == gdwNotes)
      nNewMode = AEE_SYNTH_MODE_HQ_ALL;
   else // gdwNotes is between (both including) 1 and 71
   {
      if (gdwNotes > 56)
         nNewMode = AEE_SYNTH_MODE_HP_ALL;
      else if (gdwNotes > 46)
         nNewMode = AEE_SYNTH_MODE_HP_HI;
      else if (gdwNotes > 38)
         nNewMode = AEE_SYNTH_MODE_HP_HI_MED;
      else if (gdwNotes > 30)
         nNewMode = AEE_SYNTH_MODE_HP_MED;
      else if (gdwNotes > 20)
         nNewMode = AEE_SYNTH_MODE_HQ_MED;
      else if (gdwNotes > 12)
         nNewMode = AEE_SYNTH_MODE_HQ_HI_MED;
      else
         nNewMode = AEE_SYNTH_MODE_HQ_HI;
   }

   // Set new mode
   if (nCurrentMode != nNewMode)
      nErr = ISYNTHQUALITY_SetSynthMode(gpISynthQuality, nNewMode);

Done:
   return nErr;
}
#endif


/*==================================================================

==================================================================*/
#ifdef FEATURE_MULTISEQUENCER
static boolean CMediaMMLayer_IsMultiSequence(CMediaBg *pme)
{
   int i;
   int numClassesSupported = sizeof(multiSequencerClasses)/sizeof(AEECLSID);

   if (pme->m_bChannelShare)
   {
      for (i = 0; i < numClassesSupported; i++)
      {
         if (multiSequencerClasses[i] == pme->m_clsSelf)
         {
            return TRUE;
         }
      }
   }
   return FALSE;
}
#endif  // FEATURE_MULTISEQUENCER

/*==================================================================

==================================================================*/
static int CMediaMMLayer_GetMMStatus(CMediaCMX * pme, cmx_status_type status, cmx_af_server_data_type * psd, OEMMMStatus * peStatus)
{
   int   nStatus = MM_STATUS_UNKNOWN;

   if (!pme)
      return nStatus;

   if (peStatus)
      *peStatus = OEMMM_STATUS_PLAY;

   switch (status)
   {
      case CMX_SUCCESS:
      case CMX_DONE:
      case CMX_ABORTED:
      case CMX_PAUSE:
      case CMX_REC_PAUSED:
      case CMX_RESUME:
      case CMX_REC_RESUMED:
      case CMX_ERROR:
      case CMX_FAILURE:
      case CMX_ENABLED:
      case CMX_DISABLED:
      case CMX_TICK_UPDATE:
      case CMX_REPEAT:
      case CMX_DATA_ACCESS_DELAY:
      case CMX_FLUSHED:
      case CMX_INVALID_DATA_SOURCE:
      case CMX_REWIND:
      case CMX_FFORWARD:
         nStatus = CMediaMMLayer_GetMMStatus2(pme, status, NULL, 0, peStatus);
         break;

      case CMX_TEMPO: // fall thru...
      case CMX_TUNE: // fall thru...
      case CMX_PAN:
         nStatus = MM_STATUS_DONE;
         if (peStatus)
            *peStatus = OEMMM_STATUS_SETPARM;
         break;

      case CMX_AUDIO_SPEC:
         nStatus = MM_STATUS_MEDIA_SPEC;
         break;

#ifdef FEATURE_QCP
      case CMX_QCP_SPACE_WARNING:
         nStatus = MM_STATUS_SPACE_WARNING;
         break;

      case CMX_QCP_SPACE_DONE:
         nStatus = MM_STATUS_SPACE_ERROR;
         break;
#endif // FEATURE_QCP
   }


   return nStatus;
}

/*==================================================================
  This version has a signature used by some stand-alone decoders
==================================================================*/
static int CMediaMMLayer_GetMMStatus2(CMediaCMX * pme, cmx_status_type status, void* data, uint32 num_bytes, OEMMMStatus * peStatus)
{
   int   nStatus = MM_STATUS_UNKNOWN;

   if (peStatus)
      *peStatus = OEMMM_STATUS_PLAY;

   switch (status)
   {
      case CMX_SUCCESS:
         break;

      case CMX_DONE:
         nStatus = MM_STATUS_DONE;
         pme->m_bStopping = FALSE;
         break;

      case CMX_ABORTED:
         // Differentiate between error and user stop
         if (pme->m_bStopping)
            nStatus = MM_STATUS_DONE;
         else
            nStatus = MM_STATUS_ABORT;
         pme->m_bStopping = FALSE;
         break;

      case CMX_PAUSE:
      case CMX_REC_PAUSED:
         nStatus = MM_STATUS_PAUSE;
         if (peStatus)
            *peStatus = OEMMM_STATUS_PLAYCTL;
         break;

      case CMX_RESUME:
      case CMX_REC_RESUMED:
         nStatus = MM_STATUS_RESUME;
         if (peStatus)
            *peStatus = OEMMM_STATUS_PLAYCTL;
         break;

      case CMX_ERROR: // fall thru...
      case CMX_FAILURE:
         if (pme->m_bStopping)
            pme->m_bStopping = FALSE;
         nStatus = MM_STATUS_ABORT;
         break;

      case CMX_CLIENT_BUF_FREE:
      case CMX_CLIENT_BUF_REQ:
         break;

      case CMX_ENABLED: // fall thru...
      case CMX_DISABLED:
         break;

      case CMX_TICK_UPDATE:
         if (pme->m_dwTickTimeMS)
            nStatus = MM_STATUS_TICK_UPDATE;
         break;

      case CMX_REPEAT:
         nStatus = MM_STATUS_REPEAT;
         break;

      case CMX_DATA_ACCESS_DELAY:
         nStatus = MM_STATUS_DATA_IO_DELAY;
         break;

      case CMX_FLUSHED:  // fall thru...
      case CMX_INVALID_DATA_SOURCE:
         break;

      case CMX_REWIND: // fall thru...
      case CMX_FFORWARD:
         nStatus = MM_STATUS_SEEK;
         if (peStatus)
            *peStatus = OEMMM_STATUS_PLAYCTL;
         break;
   }

   return nStatus;
}

/*==================================================================

==================================================================*/
static void CMediaMMLayer_ProcessPlayServerData(CMediaCMX * pme, cmx_status_type status, cmx_af_server_data_type * psd, AEEMediaCallback * pcb)
{
   if (!pcb || !psd)
      return;

   switch (status)
   {
      case CMX_PAUSE:
      case CMX_REC_PAUSED:
         pcb->cmdNotify.nSubCmd = OEMMM_CTLCMD_PAUSE;
         pcb->cmdNotify.pCmdData = (void *)psd->elapsed_time_ms;
         pcb->cmdNotify.dwSize = sizeof(psd->elapsed_time_ms);
         break;

      case CMX_RESUME:
      case CMX_REC_RESUMED:
         pcb->cmdNotify.nSubCmd = OEMMM_CTLCMD_RESUME;
         pcb->cmdNotify.pCmdData = (void *)psd->elapsed_time_ms;
         pcb->cmdNotify.dwSize = sizeof(psd->elapsed_time_ms);
         break;

      case CMX_REWIND:
      case CMX_FFORWARD:
         pcb->cmdNotify.nSubCmd = OEMMM_CTLCMD_SEEK;
         pcb->cmdNotify.pCmdData = (void *)psd->elapsed_time_ms;
         pcb->cmdNotify.dwSize = sizeof(psd->elapsed_time_ms);
         break;

      case CMX_TEMPO:
         pme->m_wTempo = (uint16)psd->tempo;
         pcb->cmdNotify.dwSize = sizeof(psd->tempo);
         break;

      case CMX_TUNE:
         pme->m_wTune = (uint16)psd->tune;
         pcb->cmdNotify.dwSize = sizeof(psd->tune);
         break;

      case CMX_PAN:
         pme->m_wPan = (uint16)psd->pan;
         pcb->cmdNotify.dwSize = sizeof(psd->pan);
         break;

      case CMX_AUDIO_SPEC:
         CMediaMMLayer_SetMediaSpec((CMediaBg *)pme, (cmx_af_audio_spec_type *)psd, pcb);
         break;

      default:
         break;
   } // switch
}

/*==================================================================

==================================================================*/
static void CMediaMMLayer_SetMediaSpec(CMediaBg * pme, cmx_af_audio_spec_type * psd, AEEMediaCallback * pcb)
{
#if defined(FEATURE_MP3)
   if (AEECLSID_MEDIAMP3 == pme->m_clsSelf && CMX_AF_FILE_MP3 == psd->file_type)
   {
      AEEMediaMP3Spec *       pm = &pme->u.m_pMP3->m_spec;
      cmx_af_mp3_spec_type *  pc = (cmx_af_mp3_spec_type *)psd;

      pm->nVersion = (int)pc->version;
      pm->nLayer = (byte)pc->layer;
      pm->bCRCFlag = pc->crc_f;
      pm->wBitRate = (uint16)pc->bitrate;

      pm->nChannel = (byte)pc->channel;
      pm->nExtension = (byte)pc->extension;
      pm->bCopyrightFlag = pc->copyright_f;
      pm->bOriginalFlag = pc->original_f;
      pm->nEmphasis = (byte)pc->emphasis;
      MEMCPY(pm->szTitle, pc->title, MIN(sizeof(pm->szTitle), sizeof(pc->title)));
      MEMCPY(pm->szArtist, pc->artist, MIN(sizeof(pm->szArtist), sizeof(pc->artist)));
      MEMCPY(pm->szAlbum, pc->album, MIN(sizeof(pm->szAlbum), sizeof(pc->album)));
      MEMCPY(pm->szYear, pc->year, MIN(sizeof(pm->szYear), sizeof(pc->year)));
      MEMCPY(pm->szComment, pc->comment, MIN(sizeof(pm->szComment), sizeof(pc->comment)));
      pm->nGenre = pc->genre;

      // Convert the sample rate...
      switch (pc->sample_rate)
      {
         case CMX_AF_MP3_SAMPLE_RATE_8000: pm->dwSampleRate = 8000; break;

#if defined(FEATURE_STEREO_DAC)
         case CMX_AF_MP3_SAMPLE_RATE_11025: pm->dwSampleRate = 11025; break;
         case CMX_AF_MP3_SAMPLE_RATE_12000: pm->dwSampleRate = 12000; break;

         case CMX_AF_MP3_SAMPLE_RATE_16000: pm->dwSampleRate = 16000; break;
         case CMX_AF_MP3_SAMPLE_RATE_22050: pm->dwSampleRate = 22050; break;
         case CMX_AF_MP3_SAMPLE_RATE_24000: pm->dwSampleRate = 24000; break;

         case CMX_AF_MP3_SAMPLE_RATE_32000: pm->dwSampleRate = 32000; break;
         case CMX_AF_MP3_SAMPLE_RATE_44100: pm->dwSampleRate = 44100; break;
         case CMX_AF_MP3_SAMPLE_RATE_48000: pm->dwSampleRate = 48000; break;
#endif // defined(FEATURE_STEREO_DAC)

         case CMX_AF_MP3_SAMPLE_RATE_UNKNOWN: // fall thru...
         default:
            pm->dwSampleRate = MM_MP3_SAMPLE_RATE_UNKNOWN; break;
      } // switch
   } // if
#endif // defined(FEATURE_MP3)

#if defined(FEATURE_BREW_CMX_V3)

#if defined(FEATURE_ADPCM)
   if (AEECLSID_MEDIAADPCM == pme->m_clsSelf && CMX_AF_FILE_ADPCM == psd->file_type)
   {
      AEEMediaWaveSpec *         pws = &pme->u.m_pADPCM->m_spec;
      cmx_af_adpcm_spec_type *   pc = (cmx_af_adpcm_spec_type *)psd;


      pws->wSize = sizeof(AEEMediaWaveSpec);
      pws->clsMedia = AEECLSID_MEDIAADPCM;
      pws->wChannels = pc->channels;
      pws->dwSamplesPerSec = pc->sample_rate;
      pws->dwBytesPerSec = pc->bytes_rate;
      pws->wBlockSize = pc->frame_size;
      pws->wBitsPerSample = pc->bits_per_sample;
      pws->dwExtra = 0;
   }
#endif // defined(FEATURE_ADPCM)

#if defined(FEATURE_AAC)
   if (AEECLSID_MEDIAAAC == pme->m_clsSelf)
   {
      // AAC media spec
   }
#endif // defined(FEATURE_AAC)
#if defined(FEATURE_IMELODY)
   if (AEECLSID_MEDIAAAC == pme->m_clsSelf)
   {
      // IMelody media spec
   }
#endif // defined(FEATURE_IMELODY)

   if (pme->m_bChannelShare)
   {
#ifdef FEATURE_MIDI
      if (AEECLSID_MEDIAMIDI == pme->m_clsSelf)
    {
      pme->m_pSequence = psd->midi_spec.sequence;
    }
#endif
#ifdef FEATURE_SMAF
    if (AEECLSID_MEDIAMMF == pme->m_clsSelf || AEECLSID_MEDIAPHR == pme->m_clsSelf)
    {
      pme->m_pSequence = psd->smaf_spec.sequence;
    }
#endif
#ifdef FEATURE_XMF
    if (AEECLSID_MEDIAXMF == pme->m_clsSelf)
    {
      // XMF media spec
    }
#endif
   }
#endif // defined(FEATURE_BREW_CMX_V3)
}

#endif // defined(FEATURE_AUDIO_FORMAT)

#if defined(FEATURE_MIDI_OUT)

/*===========================================================================

                      CMX: MIDIOutMsg and MIDIOutQCP

===========================================================================*/
static void CMediaMMLayer_MIDIOutNotify(CMediaCMX * pme, boolean bSuccess);

/*==================================================================

   CMX MIDI Device Functions

==================================================================*/
static int CMIDIOut_AddRef(void)
{
   return ++gsMIDIOut.m_nRef;
}

static int CMIDIOut_Release(void)
{
   if (!--gsMIDIOut.m_nRef)
   {
      OEMMedia_SetParm(NULL, OEMCMX_ID_MIDI_DEVICE, (int32)FALSE, 0, 0, FALSE);
      MEMSET(&gsMIDIOut, (int)0, sizeof(CMIDIOut));
   }

   return gsMIDIOut.m_nRef;
}

static int CMIDIOut_Open(byte nADECType, PFNMIDIOUTNOTIFY pfn, CMediaCMX * pme)
{
   int   nRet = SUCCESS;

   if (!pfn)
      return EBADPARM;

   if (!CMIDIOut_IsOpen())
   {
      if (!gsMIDIOut.m_bOpenPending)
      {
         gsMIDIOut.m_nADECType = nADECType;
         gsMIDIOut.m_bOpenPending = TRUE;
         gsMIDIOut.m_pfnNotify = pfn;
         gsMIDIOut.m_pMM = pme;
         CALLBACK_Init(&gsMIDIOut.m_cb, CMIDIOut_OpenNotify, NULL);
#if defined(FEATURE_BREW_CMX_V3)
         {
            CMXInfo  ci;
            ci.nType = OEMCMX_TYPE_MIDI_OUT_QCP;
            ci.ph = NULL;
            ci.dwCB = (uint32)CMIDIOut_MIDIDeviceCB;
            ci.dwUserData = (uint32)pme->m_hObject;
            OEMMedia_SetParm(NULL, OEMCMX_ID_MIDI_DEVICE, (int32)TRUE, (int32)nADECType, (int32)&ci, FALSE);
         }
#else // CMX 2.x
         cmx_midi_out_open(CMIDIOut_MIDIDeviceCB, (void *)pme->m_hObject);
#endif // defined(FEATURE_BREW_CMX_V3)
      }
      else
         nRet = MM_EMIDIOUTNOTOPEN;
   }
   else
   {
      CMIDIOut_AddRef();
      pfn(pme, TRUE);
   }

   return nRet;
}

void CMIDIOut_MIDIDeviceCB(cmx_status_type status, const void * pClientData)
{
   CMediaCMX * pMedia ;
   boolean     bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   pMedia = (CMediaCMX *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);


   if (!pMedia)
   {
      gsMIDIOut.m_bOpenPending = FALSE;
      goto OEMMM_CBExit;
   }

   switch (status)
   {
      case CMX_ENABLED: // open succeeded.
      case CMX_FAILURE: // for open() failure or prev midi cmd failure
         if (!CMIDIOut_IsOpen())
         {
            gsMIDIOut.m_cb.pNotifyData = (void *)status;
            bResume = TRUE;
         }
         break;

      case CMX_DISABLED: // when close() happens
         break;

      case CMX_SUCCESS: // for open() queueing, prev midi cmd success
         break;

      case CMX_DONE: // never sent??
         break;
   }

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&gsMIDIOut.m_cb,0); // Sys resume.
}

static void CMIDIOut_OpenNotify(void * pUser)
{
   CMediaCMX * pme = (CMediaCMX *)AEEObjectMgr_GetObject(gsMIDIOut.m_pMM->m_hObject);
   boolean     bOpen = ((cmx_status_type)(uint32)pUser) == CMX_ENABLED;

   gsMIDIOut.m_bOpenPending = FALSE;

   if (!pme)
      return;

   if (bOpen)
      CMIDIOut_AddRef();

   gsMIDIOut.m_pfnNotify(gsMIDIOut.m_pMM, bOpen);
}

static void CMediaMMLayer_MIDIOutNotify(CMediaCMX * pme, boolean bSuccess)
{
   pme->m_bMIDIOut = bSuccess;

   if (pme->m_clsSelf == AEECLSID_MEDIAMIDIOUTMSG)
   {
      if (bSuccess)
         CMediaMMLayer_MIDIOutMsg((CMediaMIDIOutMsg *)pme);
   }
   else if (pme->m_clsSelf == AEECLSID_MEDIAMIDIOUTQCP || pme->m_clsSelf == AEECLSID_MEDIAADPCM || pme->m_clsSelf == AEECLSID_MEDIAPCM)
   {
#if defined(FEATURE_MIDI_OUT_QCP)
      if (bSuccess)
         CMediaMMLayer_MIDIOutQCPPlay((CMediaMIDIOutQCP *)pme);
      else
         CMediaMMLayer_MIDIOutQCPStatusCB(CMX_FAILURE, (void *)pme->m_hObject);
#endif // defined(FEATURE_MIDI_OUT_QCP)
   }
}

static int CMediaMMLayer_MIDIOutMsg(CMediaMIDIOutMsg * pme)
{
   // If MIDIOut device is not open, then issue an open command and return.
   // The callback of the MIDI open will increment MIDI device ref and
   // calls the registered callback which calls this function to play the media

   if (!pme->m_bMIDIOut)
      return CMIDIOut_Open((byte)OEMMM_ADEC_MODE_DEFAULT, CMediaMMLayer_MIDIOutNotify, (CMediaCMX *)pme);

   {
      CMediaMIDIOutMsg *   p = (CMediaMIDIOutMsg *)pme;
      cmx_handle_type      h;
      CMXInfo              ci;

      h.source_type = h.mem.source_type = CMX_SOURCE_MEM;
      h.mem.buf = p->m_pMsg;
      h.mem.len = p->m_dwSize;

      ci.nType = OEMCMX_TYPE_MIDI_OUT_MSG;
      ci.ph = &h;
      ci.dwCB = (uint32)CMediaMMLayer_MIDIOutMsgCB;
      ci.dwUserData = (uint32)pme->m_hObject;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, 0, 0, TRUE);
      OEMMedia_Start(pme->m_pMediaCMX);
   }

   return SUCCESS;
}

void CMediaMMLayer_MIDIOutMsgCB(cmx_status_type status, const void * pClientData)
{
   CMediaMIDIOutMsg *   pme;
   AEEMediaCallback *   pcb1 = NULL;
   AEEMediaCallback *   pcb2 = NULL;
   boolean              bResume1 = FALSE;
   boolean              bResume2 = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = (CMediaMIDIOutMsg *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   // In this callback, send MM_STATUS_START followed by
   // MM_STATUS_DONE/MM_STATUS_ABORT based on CMX status.

   pcb1 = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_PLAY, 0);
   if (!pcb1)
      goto OEMMM_CBExit;

   pcb1->cmdNotify.nStatus = MM_STATUS_START;
   bResume1 = TRUE;

   pcb2 = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_PLAY, 0);
   if (!pcb2)
   {
      pcb1->cmdNotify.nStatus = MM_STATUS_ABORT;
      goto OEMMM_CBExit;
   }

   switch (status)
   {
      case CMX_SUCCESS:
         pcb2->cmdNotify.nStatus = MM_STATUS_DONE;
         bResume2 = TRUE;
         break;

      case CMX_FAILURE:
         pcb2->cmdNotify.nStatus = MM_STATUS_ABORT;
         bResume2 = TRUE;
         break;

      default:
         CMediaMMLayer_FreeCallback(pcb2);
         break;
   }

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume1)
      AEE_ResumeCallback(&pcb1->cb, pme->m_pac);
   if(bResume2)
      AEE_ResumeCallback(&pcb2->cb, pme->m_pac);
}

#if defined(FEATURE_MIDI_OUT_QCP)
#if defined(FEATURE_QAUDIOFX)
#if !defined (FEATURE_CMX_MM_API)
static void CMediaMIDIOutQCP_QCPStartCB(void * pUser)
{
   AEEMediaCallback * pcb = (AEEMediaCallback *)pUser;
   CMediaMIDIOutQCP * pme;
   AEEMediaCallback   mcb;
   CMXInfo            ci;

   if (!pcb)
      return;

   MEMCPY(&mcb, pcb, sizeof(AEEMediaCallback));
   CMediaMMLayer_FreeCallback(pcb);

   if (MM_CMD_PLAY == mcb.cmdNotify.nCmd && MM_STATUS_START == mcb.cmdNotify.nStatus)
   {
      pme = (CMediaMIDIOutQCP *)mcb.cmdNotify.pIMedia;
      CMediaMIDIOutQCP_FlushParamCache((IMedia *)pme);
      pme->m_bStarting = TRUE;

      ci.nType = OEMCMX_TYPE_MIDI_OUT_QCP;
      ci.ph = NULL;
      ci.dwCB = (uint32)CMediaMMLayer_MIDIOutQCPStatusCB;
      ci.dwUserData = (uint32)pme->m_hObject;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_STATUS_CB, (int32)&ci, 0, 0, TRUE);

      cmx_midi_out_adec_start_chan ((void *)pme->m_dwChannel, (cmx_cb_func_ptr_type)pme->m_pMediaCMX->m_ciStatus.dwCB, (void *)pme->m_pMediaCMX->m_ciStatus.dwUserData);
   }
}
#endif // !defined (FEATURE_CMX_MM_API)
#endif // defined(FEATURE_QAUDIOFX)

/*==================================================================

   CMX MIDIOutQCP Functions

==================================================================*/
static int CMediaMMLayer_MIDIOutQCPPlay(CMediaMIDIOutQCP * pme)
{
   cmx_handle_type      h;
   int                  nRet = SUCCESS;
   CMXInfo              ci;

   // If MIDIOut device is not open, then issue an open command and return.
   // The callback of the MIDI open will increment MIDI device ref and
   // calls the registered callback which calls this function to play the media
   // Make sure that QCP is played via ADEC_QCP and ADPCM is played via ADEC_ADPCM...
   if (!pme->m_bMIDIOut)
   {
      int   nADECType = -1;

      if (AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf)
      {
         if (CMIDIOut_IsQCP())
            nADECType = OEMMM_ADEC_MODE_QCP;
      }
      else if (AEECLSID_MEDIAADPCM == pme->m_clsSelf)
      {
         if (CMIDIOut_IsADPCM())
            nADECType = OEMMM_ADEC_MODE_ADPCM;
      }
      else if (AEECLSID_MEDIAPCM == pme->m_clsSelf)
      {
         if (CMIDIOut_IsPCM())
            nADECType = OEMMM_ADEC_MODE_PCM;
      }

#if defined(FEATURE_GASYNTH_INTERFACE)
      // Any mode will work with any media, and they can be mixed
      nADECType = OEMMM_ADEC_MODE_ADPCM;
#endif //defined(FEATURE_GASYNTH_INTERFACE)

      if (nADECType > -1)
         return CMIDIOut_Open((byte)nADECType, CMediaMMLayer_MIDIOutNotify, (CMediaCMX *)pme);
      else
         return EFAILED;
   }

   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

   if (!pme->m_bVolumeScaled)
   {
      // Scale the volume here...
      pme->m_wVolume = (uint16)((CMX_MIDI_OUT_QCP_DEFAULT_VOLUME / AEE_MAX_VOLUME) * pme->m_wVolume);
      pme->m_bVolumeScaled = TRUE;
   }

   ci.nType = OEMCMX_TYPE_MIDI_OUT_QCP;
   ci.ph = &h;
   ci.dwCB = (uint32)CMediaMMLayer_MIDIOutQCPPlayCB;
   ci.dwUserData = (uint32)pme->m_hObject;
   OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, 0, 0, TRUE);
   OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_VOLUME, (int32)pme->m_wVolume, 0, 0, TRUE);
   OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_PAN, (int32)(pme->m_wPan - 64), 0, 0, TRUE);
   OEMMedia_Start(pme->m_pMediaCMX);

   // MM_STATUS_START is sent in the CMX_SUCCESS of CMediaMMLayer_MIDIOutQCPPlayCB()

   return SUCCESS;
}

static int CMediaMMLayer_MIDIOutQCPStop(CMediaMIDIOutQCP * pme, uint32 dwChannel)
{
   AEEMediaCallback *   pcb = &pme->m_cbCommand;

   // Callback must always be available for stop command.
   CMediaMMLayer_FreeCallback(pcb);
   CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_PLAY, OEMMM_CTLCMD_STOP);

   OEMMedia_Stop(pme->m_pMediaCMX);
   return SUCCESS;
}

static int CMediaMMLayer_MIDIOutQCPPause(CMediaMIDIOutQCP * pme)
{
   AEEMediaCallback *   pcb;

   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_PLAY, OEMMM_CTLCMD_PAUSE);
   if (!pcb)
      return EITEMBUSY;

   OEMMedia_Pause(pme->m_pMediaCMX);

   return SUCCESS;
}

static int CMediaMMLayer_MIDIOutQCPResume(CMediaMIDIOutQCP * pme)
{
   AEEMediaCallback *   pcb;

   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_PLAY, OEMMM_CTLCMD_RESUME);
   if (!pcb)
      return EITEMBUSY;

   OEMMedia_Resume(pme->m_pMediaCMX);

   return SUCCESS;
}

static int CMediaMMLayer_MIDIOutQCPSetVolume(CMediaMIDIOutQCP * pme, uint32 dwChannel, uint16 wVolume)
{
   AEEMediaCallback *   pcb;

   if (wVolume > AEE_MAX_VOLUME)
      wVolume = AEE_MAX_VOLUME;

   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME);
   if (!pcb)
      return EITEMBUSY;

   // Scale the volume here...
   pme->m_wVolume = (uint16)((CMX_MIDI_OUT_QCP_DEFAULT_VOLUME / AEE_MAX_VOLUME) * wVolume);
   pme->m_bVolumeScaled = TRUE;

   OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_VOLUME, (int32)pme->m_wVolume, 0, 0, FALSE);

   return SUCCESS;
}

static int CMediaMMLayer_MIDIOutQCPSetPan(CMediaMIDIOutQCP * pme, uint32 dwChannel, uint16 wPan)
{
   AEEMediaCallback *   pcb;
   sint15               nScaledPan;

   if (wPan > MM_MAX_PAN)
      return EBADPARM;

   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_PAN);
   if (!pcb)
      return EITEMBUSY;

   pme->m_wPan = wPan;
   // Scale the pan here...
   nScaledPan = (sint15)(wPan - 64);

   OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_PAN, (int32)nScaledPan, 0, 0, FALSE);

   return SUCCESS;
}

void CMediaMMLayer_MIDIOutQCPStatusCB(cmx_status_type status, const void * pClientData)
{
   AEEMediaCallback *   pcb = NULL;
   CMediaMIDIOutQCP *   pme;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = (CMediaMIDIOutQCP *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

#if defined(FEATURE_QAUDIOFX)
#if !defined (FEATURE_CMX_MM_API)
   if (pme->m_bStarting)
   {
      if (CMX_SUCCESS == status) // everything is ok, nothing to do
         goto OEMMM_CBExit;

      // notify app about this error
      pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_PLAY, 0);
      if (!pcb)
         goto OEMMM_CBExit;
   }
   else
#endif // !defined (FEATURE_CMX_MM_API)
#endif // defined(FEATURE_QAUDIOFX
   {
      pcb = &pme->m_cbCommand;
      if (!pcb->bInUse)
         goto OEMMM_CBExit;
   }

   switch (status)
   {
      case CMX_SUCCESS:
         if (CMediaMMLayer_ISPAUSE(pcb))
            pcb->cmdNotify.nStatus = MM_STATUS_PAUSE;
         else if (CMediaMMLayer_ISRESUME(pcb))
            pcb->cmdNotify.nStatus = MM_STATUS_RESUME;
         else if (!CMediaMMLayer_ISSTOP(pcb))  // do not send stop done as play done is sent.
            pcb->cmdNotify.nStatus = MM_STATUS_DONE;

         pcb->cmdNotify.nSubCmd = 0;
         break;

      case CMX_FAILURE:
         if (CMediaMMLayer_ISPAUSE(pcb))
            pcb->cmdNotify.nStatus = MM_STATUS_PAUSE_FAIL;
         else if (CMediaMMLayer_ISRESUME(pcb))
            pcb->cmdNotify.nStatus = MM_STATUS_RESUME_FAIL;
         else if (!CMediaMMLayer_ISSTOP(pcb))  // do not send stop done as play done is sent.
            pcb->cmdNotify.nStatus = MM_STATUS_ABORT;

         pcb->cmdNotify.nSubCmd = 0;
         break;
   }

#if defined (FEATURE_QAUDIOFX)
#if !defined (FEATURE_CMX_MM_API)
   if (MM_CMD_PLAY == pcb->cmdNotify.nCmd &&
       (MM_STATUS_DONE == pcb->cmdNotify.nStatus || MM_STATUS_ABORT == pcb->cmdNotify.nStatus))
   {
      pme->m_bStarting = FALSE;
      pme->m_bPlaying = FALSE;
   }
#endif // !defined (FEATURE_CMX_MM_API)
#endif // defined(FEATURE_QAUDIOFX)

   bResume = TRUE;

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
}

#if defined (FEATURE_QAUDIOFX)
static void  CMediaMIDIOutQCP_QCPPlayDoneCB (cmx_status_type status, cmx_3d_pos_param_enum_type param, cmx_3d_pos_param_setting_type *setting, const void *pClientData)
{
   CMediaMIDIOutQCP * pme = (CMediaMIDIOutQCP *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);

   if (pme && CMX_SUCCESS == status &&  CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS == param)
   {
      OEMMM_ENTER_CRITICAL_SECTION()

      CMediaMIDIOutQCP_SetParamCache(pme, setting);
      pme->m_bPlaying = FALSE;

      OEMMM_LEAVE_CRITICAL_SECTION()
   }
}
#endif // defined(FEATURE_QAUDIOFX)

void CMediaMMLayer_MIDIOutQCPPlayCB(cmx_status_type status, const void * midi_out_qcp_id, const void * pClientData)
{
   CMediaMIDIOutQCP *   pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = (CMediaMIDIOutQCP *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_PLAY, 0);
   if (!pcb)
      goto OEMMM_CBExit;

   switch (status)
   {
#if defined(FEATURE_QAUDIOFX)
#if !defined (FEATURE_CMX_MM_API)
      case CMX_STARTED:
         if (pme->m_bStarting)
         {
            cmx_3d_pos_get_param_type  cmxGetParam;

            pcb->cmdNotify.nStatus = MM_STATUS_START;
            pme->m_bStarting = FALSE;
            pme->m_bPlaying = TRUE;

            cmxGetParam.param           = CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS;
            cmxGetParam.src_param.param = CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS;
            cmxGetParam.src_param.id    = (void *)pme->m_dwChannel;
            cmx_3d_pos_get_param (&cmxGetParam, CMediaMIDIOutQCP_QCPPlayDoneCB, (void *)pme->m_hObject);
         }
         else
         {
            pcb->cmdNotify.nStatus = MM_STATUS_ABORT;
         }
         break;
#endif // !defined (FEATURE_CMX_MM_API)
#endif // defined(FEATURE_QAUDIOFX)

      case CMX_SUCCESS:
      {
         pme->m_dwChannel = (uint32)midi_out_qcp_id;
         pme->m_bChannelSet = TRUE;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_CHANNEL, pme->m_dwChannel, 0, 0, TRUE);

#if defined(FEATURE_QAUDIOFX)
#if defined (FEATURE_CMX_MM_API)
         if (OEMCMX_TYPE_MIDI_OUT_QCP == pme->m_ciStart.nType)
         {
            cmx_3d_pos_get_param_type  cmxGetParam;

            pme->m_bPlaying = TRUE;

            cmxGetParam.param           = CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS;
            cmxGetParam.src_param.param = CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS;
            cmxGetParam.src_param.id    = (void *)pme->m_dwChannel;
            cmx_3d_pos_get_param (&cmxGetParam, CMediaMIDIOutQCP_QCPPlayDoneCB, (void *)pme->m_hObject);
         }
#else // !defined (FEATURE_CMX_MM_API)
            CMediaMMLayer_FreeCallback(pcb);
            pcb = CMediaMMLayer_AllocCallback(&pme->m_cb3DStart, 1, MM_CMD_PLAY, 0);
            if (!pcb)
               goto OEMMM_CBExit;
#endif // !defined (FEATURE_CMX_MM_API)
#endif // defined(FEATURE_QAUDIOFX)

         pcb->cmdNotify.nStatus = MM_STATUS_START;
         break;
      }

      case CMX_FAILURE:
         pcb->cmdNotify.nStatus = MM_STATUS_ABORT;
         pme->m_bChannelSet = FALSE;
         break;

      //rr required??
      case CMX_DONE:
         pcb->cmdNotify.nStatus = MM_STATUS_DONE;
         pme->m_bChannelSet = FALSE;
         break;
   }

#if defined (FEATURE_QAUDIOFX)
#if !defined (FEATURE_CMX_MM_API)
   if (MM_CMD_PLAY == pcb->cmdNotify.nCmd &&
       (MM_STATUS_DONE == pcb->cmdNotify.nStatus || MM_STATUS_ABORT == pcb->cmdNotify.nStatus))
   {
      pme->m_bStarting = FALSE;
      pme->m_bPlaying = FALSE;
   }
#endif // !defined (FEATURE_CMX_MM_API)
#endif // defined (FEATURE_QAUDIOFX)

   bResume = TRUE;

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
}

#endif // defined(FEATURE_MIDI_OUT_QCP)
#endif // defined(FEATURE_MIDI_OUT)

/*===========================================================================

  If FEATURE_AUDIO_FORMAT is not defined, then
  define the following stubs.

===========================================================================*/
#if !defined(FEATURE_AUDIO_FORMAT)

static int CMediaMMLayer_Play(CMediaBg * pme)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_Stop(CMediaBg * pme)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_Seek(CMediaBg * pme, AEEMediaSeek eSeek, int32 lTimeMS)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_Pause(CMediaBg * pme)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_Resume(CMediaBg * pme)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_GetTotalTime(CMediaBg * pme)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_SetTempo(CMediaBg * pme, uint32 dwTempo)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_SetTune(CMediaBg * pme, uint32 dwTune)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_SetPan(CMediaBg * pme, uint32 dwTune)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_QCPRecord(CMediaBg * pme, int nFormat)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_PCMRecord(CMediaBg * pme)
{
   return EUNSUPPORTED;
}
static int CMediaMMLayer_AMRRecord(CMediaBg * pme)
{
   return EUNSUPPORTED;
}
#endif // !defined(FEATURE_AUDIO_FORMAT)

/*===========================================================================

  If FEATURE_MIDI_OUT is not defined, then
  define the following stubs.

===========================================================================*/
#if !defined(FEATURE_MIDI_OUT)

static int CMediaMMLayer_MIDIOutMsg(CMediaMIDIOutMsg * pme)
{
   return EUNSUPPORTED;
}

#endif // !defined(FEATURE_MIDI_OUT)

#if !defined(FEATURE_MIDI_OUT_QCP)

static int CMediaMMLayer_MIDIOutQCPPlay(CMediaMIDIOutQCP * pme)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_MIDIOutQCPStop(CMediaMIDIOutQCP * pme, uint32 dwChannel)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_MIDIOutQCPPause(CMediaMIDIOutQCP * pme)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_MIDIOutQCPResume(CMediaMIDIOutQCP * pme)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_MIDIOutQCPSetVolume(CMediaMIDIOutQCP * pme, uint32 dwChannel, uint16 wVolume)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_MIDIOutQCPSetPan(CMediaMIDIOutQCP * pme, uint32 dwChannel, uint16 wPan)
{
   return EUNSUPPORTED;
}

#endif // !defined(FEATURE_MIDI_OUT_QCP)

/*===============================================================================

      CMediaMain DECLARATIONS
      This is a wrapper IMedia class that routes the calls to currently
      active IMedia object contained in the wrapper class.

===============================================================================*/

OBJECT(CMediaMain)
{
   INHERIT_AEEMedia(IMediaADPCM);

   IMedia *          m_pMedia;
};

void IMediaVC0848Main_Init(IShell * ps);
int  IMediaVC0848Main_New(IShell * ps, AEECLSID cls, void **ppif);

static uint32  CMediaMain_Release(IMedia * po);
static int     CMediaMain_RegisterNotify(IMedia * po, PFNMEDIANOTIFY pfnNotify, void * pUser);
static int     CMediaMain_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int     CMediaMain_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);
static int     CMediaMain_Play(IMedia * po);
static int     CMediaMain_Record(IMedia * po);
static int     CMediaMain_Stop(IMedia * po);
static int     CMediaMain_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS);
static int     CMediaMain_Pause(IMedia * po);
static int     CMediaMain_Resume(IMedia * po);
static int     CMediaMain_GetTotalTime(IMedia * po);
static int     CMediaMain_GetState(IMedia * po, boolean * pbStateChanging);
static int     CMediaMain_QueryInterface(IMedia * po, AEECLSID idReq, void ** ppo);

static void    CMediaMain_Delete(CMediaMain * pme);
static int     CMediaMain_CreateMedia(CMediaMain * pme);
static void    CMediaMain_MediaNotify(CMediaMain * pme, AEEMediaCmdNotify * pcn);

static const VTBL(IMediaADPCM) gMediaMainFuncs =
{
   AEEMedia_AddRef,
   CMediaMain_Release,
   CMediaMain_QueryInterface,
   CMediaMain_RegisterNotify,
   CMediaMain_SetMediaParm,
   CMediaMain_GetMediaParm,
   CMediaMain_Play,
   CMediaMain_Record,
   CMediaMain_Stop,
   CMediaMain_Seek,
   CMediaMain_Pause,
   CMediaMain_Resume,
   CMediaMain_GetTotalTime,
   CMediaMain_GetState
};

/*==================================================================

==================================================================*/
void IMediaVC0848Main_Init(IShell * ps)
{
   if (CMediaFormat_InModTable(AEECLSID_MEDIAQCP, (void *)IMediaVC0848Main_Init, NULL))
   {
      AEEMedia_Init(ps, MT_AUDIO_QCP, AEECLSID_MEDIAQCP);
      AEEMedia_Init(ps, MT_AUDIO_VND_QCELP, AEECLSID_MEDIAQCP);
   }

   if (CMediaFormat_InModTable(AEECLSID_MEDIAADPCM, (void *)IMediaVC0848Main_Init, NULL))
      AEEMedia_Init(ps, MT_AUDIO_ADPCM, AEECLSID_MEDIAADPCM);

   if (CMediaFormat_InModTable(AEECLSID_MEDIAAMR, (void *)IMediaVC0848Main_Init, (void *)NULL))
   {
      AEEMedia_Init(ps, MT_AUDIO_AMR, AEECLSID_MEDIAAMR);
   }

}

/*==================================================================

==================================================================*/
int IMediaVC0848Main_New(IShell * ps, AEECLSID cls, void **ppif)
{
   int            nErr;
   CMediaMain *   pme;

   if (!ppif)
      return EFAILED;

   pme = (CMediaMain *)AEE_NewClass((IBaseVtbl *)&gMediaMainFuncs, sizeof(CMediaMain));
   if (!pme)
      return ENOMEMORY;

   if (SUCCESS != AEEMedia_New((IMedia *)pme, ps, cls))
   {
      FREE(pme);
      return ENOMEMORY;
   }

   // Based on class ID, decide the value of bChannelShare...
   if ((cls == AEECLSID_MEDIAQCP) || (cls == AEECLSID_MEDIAADPCM) ||
       (cls == AEECLSID_MEDIAPCM) || (cls == AEECLSID_MEDIAAMR))
   {
      pme->m_bChannelShare = FALSE;
   }
   else // by default, if allowed by device, set to TRUE.
   {
      pme->m_bChannelShare = TRUE;
   }

   nErr = CMediaMain_CreateMedia(pme);
   if (nErr)
   {
      FREE(pme);
      return nErr;
   }

   *ppif = pme;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int CMediaMain_CreateMedia(CMediaMain * pme)
{
   int            nErr;
   int            nState;

   if (pme->m_pMedia)
   {
      nState = IMEDIA_GetState(pme->m_pMedia, NULL);
      MM_RELEASEIF(pme->m_pMedia);
   }
   else
      nState = MM_STATE_IDLE;

   if (pme->m_bChannelShare)
   {
      AEECLSID cls = (pme->m_clsSelf == AEECLSID_MEDIAQCP)
                     ? AEECLSID_MEDIAMIDIOUTQCP : pme->m_clsSelf;

      nErr = IMediaVC0848MIDIOutQCP_New(pme->m_pIShell, cls, (void **)&pme->m_pMedia);

      // set the channel shared flag in m_pMedia, which will default to 0
      ((CMediaMIDIOutQCP *) pme->m_pMedia)->m_bChannelShare = 1;
   }
   else
      nErr = IMediaVC0848Bg_New(pme->m_pIShell, pme->m_clsSelf, (void **)&pme->m_pMedia);

   if (!nErr && nState != MM_STATE_IDLE)
   {
      nErr = IMEDIA_SetMediaData(pme->m_pMedia, &pme->m_md);
      if (!nErr)
         nErr = IMEDIA_RegisterNotify(pme->m_pMedia, (PFNMEDIANOTIFY)CMediaMain_MediaNotify, pme);
   }

   return nErr;
}

/*==================================================================

==================================================================*/
static uint32 CMediaMain_Release(IMedia * po)
{
   CMediaMain *   pme = (CMediaMain *)po;
   int32          nRef = pme->m_nRefs;

   if (nRef)
   {
      nRef = (int32)AEEMedia_Release(po);

      if (!nRef)
      {
         CMediaMain_Delete(pme);
      }
   }

   return nRef;
}

/*==================================================================

==================================================================*/
static int CMediaMain_RegisterNotify(IMedia * po, PFNMEDIANOTIFY pfnNotify, void * pUser)
{
   CMediaMain *   pme = (CMediaMain *)po;
   int            nErr;

   if (!pme->m_pMedia)
      return EFAILED;

   nErr = AEEMedia_RegisterNotify(po, pfnNotify, pUser);
   if (nErr)
      return nErr;

   return IMEDIA_RegisterNotify(pme->m_pMedia, (PFNMEDIANOTIFY)CMediaMain_MediaNotify, pme);
}

/*==================================================================

==================================================================*/
static int CMediaMain_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   CMediaMain *   pme = (CMediaMain *)po;
   int            nErr;

   if (!pme->m_pMedia)
      return EFAILED;

   if (nParmID == MM_PARM_MEDIA_DATA)
   {
      nErr = AEEMedia_SetMediaParm(po, nParmID, p1, p2);
      if (!nErr)
         return IMEDIA_SetMediaParm(pme->m_pMedia, nParmID, p1, p2);
      else
         return nErr;
   }
   else if (nParmID == MM_PARM_CHANNEL_SHARE)
   {
      boolean  bEnable = (boolean)p1;
      boolean bChanging;
      int nState = IMEDIA_GetState(po, &bChanging);
      if (nState == MM_STATE_READY || nState == MM_STATE_IDLE)
      {
         if (pme->m_bChannelShare != bEnable)
         {
            pme->m_bChannelShare = bEnable;
            return CMediaMain_CreateMedia(pme);
         }
         return SUCCESS;
      }
      else
         return EBADSTATE;
   }

   return IMEDIA_SetMediaParm(pme->m_pMedia, nParmID, p1, p2);
}

/*==================================================================

==================================================================*/
static int CMediaMain_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2)
{
   CMediaMain *   pme = (CMediaMain *)po;

   if (!pme->m_pMedia)
      return EFAILED;

   if (nParmID == MM_PARM_MEDIA_DATA || nParmID == MM_PARM_CLSID)
      return AEEMedia_GetMediaParm(po, nParmID, pP1, pP2);
   else if (nParmID == MM_PARM_CHANNEL_SHARE)
   {
      if (pP1)
         *(boolean *)pP1 = pme->m_bChannelShare;
      return SUCCESS;
   }

   return IMEDIA_GetMediaParm(pme->m_pMedia, nParmID, pP1, pP2);
}

/*==================================================================

==================================================================*/
static int CMediaMain_Play(IMedia * po)
{
   CMediaMain *   pme = (CMediaMain *)po;

   if (!pme->m_pMedia)
      return EFAILED;

   return IMEDIA_Play(pme->m_pMedia);
}

/*==================================================================

==================================================================*/
static int CMediaMain_Record(IMedia * po)
{
   CMediaMain *   pme = (CMediaMain *)po;

   if (!pme->m_pMedia)
      return EFAILED;

   return IMEDIA_Record(pme->m_pMedia);
}

/*==================================================================

==================================================================*/
static int CMediaMain_Stop(IMedia * po)
{
   CMediaMain *   pme = (CMediaMain *)po;

   if (!pme->m_pMedia)
      return EFAILED;

   return IMEDIA_Stop(pme->m_pMedia);
}

/*==================================================================

==================================================================*/
static int CMediaMain_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   CMediaMain *   pme = (CMediaMain *)po;

   if (!pme->m_pMedia)
      return EFAILED;

   return IMEDIA_Seek(pme->m_pMedia, eSeek, lTimeMS);
}

/*==================================================================

==================================================================*/
static int CMediaMain_Pause(IMedia * po)
{
   CMediaMain *   pme = (CMediaMain *)po;

   if (!pme->m_pMedia)
      return EFAILED;

   return IMEDIA_Pause(pme->m_pMedia);
}

/*==================================================================

==================================================================*/
static int CMediaMain_Resume(IMedia * po)
{
   CMediaMain *   pme = (CMediaMain *)po;

   if (!pme->m_pMedia)
      return EFAILED;

   return IMEDIA_Resume(pme->m_pMedia);
}

/*==================================================================

==================================================================*/
static int CMediaMain_GetTotalTime(IMedia * po)
{
   CMediaMain *   pme = (CMediaMain *)po;

   if (!pme->m_pMedia)
      return EFAILED;

   return IMEDIA_GetTotalTime(pme->m_pMedia);
}

/*==================================================================

==================================================================*/
static int CMediaMain_GetState(IMedia * po, boolean * pbStateChanging)
{
   CMediaMain *   pme = (CMediaMain *)po;

   if (pme->m_pMedia)
      pme->m_nState = IMEDIA_GetState(pme->m_pMedia, &pme->m_bStateChanging);

   if (pbStateChanging)
      *pbStateChanging = pme->m_bStateChanging;

   return pme->m_nState;
}

/*==================================================================

==================================================================*/
static void CMediaMain_Delete(CMediaMain * pme)
{
   MM_RELEASEIF(pme->m_pMedia);

   // Call base class destructor
   AEEMedia_Delete((IMedia *)pme);

   FREE(pme);
}

/*==================================================================

==================================================================*/
static void CMediaMain_MediaNotify(CMediaMain * pme, AEEMediaCmdNotify * pcn)
{
   // Update the state...
   CMediaMain_GetState((IMedia *)pme, NULL);

   // Update the notify info...
   if (pcn && pme->m_pfnNotify)
   {
      pcn->pIMedia = (IMedia *)pme;
      pcn->clsMedia = pme->m_clsSelf;
      pme->m_pfnNotify(pme->m_pUser, pcn);
   }
}

/*==================================================================

==================================================================*/
static int CMediaMain_QueryInterface(IMedia * po, AEECLSID idReq, void ** ppo)
{
   CMediaMain *pme = (CMediaMain*)po;

   if (!ppo)
      return EBADPARM;

   if (!pme->m_pMedia)
      return EBADSTATE;

#if defined (FEATURE_QAUDIOFX)
   if (((pme->m_clsSelf == AEECLSID_MEDIAQCP) || (pme->m_clsSelf == AEECLSID_MEDIAADPCM) || (pme->m_clsSelf == AEECLSID_MEDIAPCM)) &&
       (idReq == AEECLSID_MEDIAAUDIO3D))
   {
      int nRet;
      boolean bOldChannelShare;
      nRet = IMEDIA_IsChannelShare(po, &bOldChannelShare);
      if (SUCCESS != nRet)
         return nRet;

      if (!bOldChannelShare)
      {
         nRet = IMEDIA_EnableChannelShare(po, TRUE);
         if (SUCCESS != nRet)
            return nRet;
      }

      return IMediaAudio3D_New(pme->m_pIShell, idReq, ppo, pme->m_pMedia);
   }
#if defined (FEATURE_ACM) && defined (SIRIUS_PLATFORM)
   else if (idReq == AEECLSID_APPRESCTL)
   {
      // The AppResCtl registered with ACM is the one associated with play/record
      return AEEMedia_QueryInterface(pme->m_pMedia, idReq, ppo);
   }
#endif // FEATURE_ACM && SIRIUS_PLATFORM
   else
#endif //defined (FEATURE_QAUDIOFX)
   {
      return AEEMedia_QueryInterface(po, idReq, ppo);
   }
}

#if defined (FEATURE_ACM)
void CMediaBg_GetResourceCntlData(AEEMedia * po, int * nPlayType, flg * channelShare)
{
  CMediaBg* pCMediaBg = (CMediaBg *) po;

  *nPlayType = pCMediaBg->m_nPlayType;

  *channelShare = pCMediaBg->m_bChannelShare;

}
void CMediaBg_SetResourceCntlCBData(uint32 * acmCB, flg isPlay)
{

  if(isPlay)
    *acmCB = (uint32)(int32)CMediaBg_AcmPlayCB;
  else
    *acmCB = (uint32)(int32)CMediaBg_AcmRecordCB;
}

static int CMediaBg_AcmRecordCB(ACM_Callback_data_type *po)
{
  int32           nRet = SUCCESS;
  AEEMedia *      pAEEMedia = (AEEMedia *)(po->pdata);
  CMediaBg *      pCMediaBg = (CMediaBg *) (po->pdata);
  IMedia *        pIMedia = (IMedia *)(po->pdata);

  if (po->state == ACM_RESOURCE_ACQUIRED)
  {
    pAEEMedia->m_bResOwner = TRUE;
    if (pCMediaBg->m_clsSelf == AEECLSID_MEDIAQCP && pCMediaBg->u.m_pQCP)
    {
      nRet = CMediaMMLayer_QCPRecord(pCMediaBg, pCMediaBg->u.m_pQCP->m_nFormat);
    }
    else if (pCMediaBg->m_clsSelf == AEECLSID_MEDIAPCM || (pCMediaBg->m_clsSelf == AEECLSID_MEDIAADPCM && pCMediaBg->u.m_pADPCM))
    {
      nRet = CMediaMMLayer_PCMRecord(pCMediaBg);
    }
    else
    {
     AEEMedia_CleanUpResource(pAEEMedia);
     return EUNSUPPORTED;
    }
    if(nRet != SUCCESS)
    {
      AEEMedia_CleanUpResource(pAEEMedia);
    }
  }
  else
  {
    // FORCE RELEASE from ACM
    // update AEEMedia state for ACM resource owner and stop
    if(pAEEMedia->m_bResOwner == TRUE)
    {
      // Protect against getting another force release while stopping
      pAEEMedia->m_bResOwner = FALSE;
      pAEEMedia->m_bForceRelease = TRUE;
      nRet = CMediaBg_Stop(pIMedia);
    }
  }

  return nRet;
}

static int CMediaBg_AcmPlayCB(ACM_Callback_data_type *po)
{
  AEEMedia *      pAEEMedia = (AEEMedia *)(po->pdata);
  CMediaBg * pCMediaBg = (CMediaBg *) (po->pdata);
  int32 nRet = SUCCESS;

  if (po->state == ACM_RESOURCE_ACQUIRED)
  {
    pAEEMedia->m_bResOwner = TRUE;
    nRet =  CMediaMMLayer_Play(pCMediaBg);

    if (nRet != SUCCESS)
    {
      AEEMedia_CleanUpResource(pAEEMedia);
    }
  }
  else
  {
    // FORCE RELEASE from ACM
    // update AEEMedia state for ACM resource owner and stop
    if(pAEEMedia->m_bResOwner == TRUE)
    {
      // Protect against getting another force release while stopping
      pAEEMedia->m_bResOwner = FALSE;
      pAEEMedia->m_bForceRelease = TRUE;
      nRet = CMediaBg_Stop((IMedia *)pAEEMedia);
    }
  }
  return nRet;
}

void CMediaMIDIOutMsg_GetResourceCntlData(AEEMedia * po, int * nPlayType, uint32 * acmCB)
{
  CMediaMIDIOutMsg* pCMediaMIDIOutMsg = (CMediaMIDIOutMsg*) po;

  *nPlayType = pCMediaMIDIOutMsg->m_nPlayType;

  *acmCB = (uint32)(int32)CMediaMIDIOutMsg_AcmPlayCB;
}

static int CMediaMIDIOutMsg_AcmPlayCB(ACM_Callback_data_type *po)
{
  AEEMedia *      pAEEMedia = (AEEMedia *)(po->pdata);
  CMediaMIDIOutMsg * pCMediaMIDIOutMsg = (CMediaMIDIOutMsg *) (po->pdata);
  int32 nRet = SUCCESS;

  if (po->state == ACM_RESOURCE_ACQUIRED)
  {
    pAEEMedia->m_bResOwner = TRUE;
    nRet = CMediaMMLayer_MIDIOutMsg(pCMediaMIDIOutMsg);

    if (nRet != SUCCESS)
    {
      AEEMedia_CleanUpResource(pAEEMedia);
    }
  }
  else
  {
    // FORCE RELEASE from ACM
    // update AEEMedia state for ACM resource owner and stop
    if(pAEEMedia->m_bResOwner == TRUE)
    {
      // Protect against getting another force release while stopping
      pAEEMedia->m_bResOwner = FALSE;
      pAEEMedia->m_bForceRelease = TRUE;
      // Per disucssion with Jadine Yee and Ed Morris, we should not
      // call IMEDIA_Release from OEM layer without the
      // application awareness. Otherwise,if the application then tries
      // to access IMedia interface, there may be a data abort.
      //
      // Acording to Alex Wong, CMX driver allow cmx_midi_out_msg()
      // and cmx_midi_out_qcp() to run at the same time and they will
      // be stopped by calling cmx_midi_out_close. However, in the ACM
      // forced release case, if the midi out msg conflicts with the
      // high priority operation, then the midp out qcp should also
      // conflict with the high priority operation. So, even
      // though calling cmx_midi_out_close can close simultaneously playing,
      // it should not be an issue.
      //
      // So, we decide to call cmx_midi_out_close instead of IMEDIA_Release.
      //
      // IMEDIA_Release((IMedia *)pAEEMedia);
      cmx_midi_out_close ((cmx_cb_func_ptr_type) CMIDIOut_MIDIDeviceCB,
                          (void *)(((CMediaCMX *)pAEEMedia)->m_hObject));

      AEEMedia_CleanUpResource(pAEEMedia);
    }
  }
  return nRet;
}

void CMediaMIDIOutQCP_GetResourceCntlData(AEEMedia * po, int * nPlayType, flg *channelShare, uint32 * acmCB)
{
  CMediaMIDIOutQCP* pCMediaMIDIOutQCP = (CMediaMIDIOutQCP*) po;

  *nPlayType = pCMediaMIDIOutQCP->m_nPlayType;
  *channelShare = pCMediaMIDIOutQCP->m_bChannelShare;
  *acmCB = (uint32)(int32)CMediaMIDIOutQCP_AcmPlayCB;
}

static int CMediaMIDIOutQCP_AcmPlayCB(ACM_Callback_data_type *po)
{
  AEEMedia *      pAEEMedia = (AEEMedia *)(po->pdata);
  CMediaMIDIOutQCP * pCMediaMIDIOutQCP = (CMediaMIDIOutQCP *) (po->pdata);
  int32 nRet = SUCCESS;

  if (po->state == ACM_RESOURCE_ACQUIRED)
  {
    pAEEMedia->m_bResOwner = TRUE;
    nRet = CMediaMMLayer_MIDIOutQCPPlay(pCMediaMIDIOutQCP);

    if (nRet != SUCCESS)
    {
      AEEMedia_CleanUpResource(pAEEMedia);
    }
  }
  else
  {
    // FORCE RELEASE from ACM
    // update AEEMedia state for ACM resource owner and stop
    if(pAEEMedia->m_bResOwner == TRUE)
    {
      // Protect against getting another force release while stopping
      pAEEMedia->m_bResOwner = FALSE;
      pAEEMedia->m_bForceRelease = TRUE;
      nRet = CMediaMIDIOutQCP_Stop((IMedia *)pAEEMedia);
    }
  }
  return nRet;
}
#endif // FEATURE_ACM

#endif // defined(FEATURE_BREW_MULTIMEDIA)

/*===============================================================================

      MPEG4 FUNCTION DEFINITIONS

===============================================================================*/

/*==================================================================
   Destructor
==================================================================*/
void OEMMediaMPEG4_Delete(IMedia * po, boolean bFree)
{
   OEMMediaMPEG4      * pme = (OEMMediaMPEG4 *)po;
   if (!pme->m_bRelMark)
   {
      pme->m_bRelMark = TRUE;

      OEMMediaMPEG4Layer_Delete(pme->m_pMPEG4);

      // Call base class destructor
      AEEMedia_Delete(po);
   }

   // Free object memory
   if (bFree)
      FREE(po);
}

/*==================================================================
   Destructor
==================================================================*/
void OEMMediaMPEG4Layer_Delete(OEMMediaMPEG4Layer * pMPEG4)
{
  int i;
  
  pMPEG4->bTerminating = TRUE;
//  OEMMediaMPEG42PV_Terminate();
  pMPEG4->bTerminating = FALSE;

#ifdef FEATURE_MP4_MSM6050
  CObjectMgr_Unregister(gpObjectMgr, pMPEG4->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
  AEEObjectMgr_Unregister(pMPEG4->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */

  /* Cancel any pending callbacks */
  for (i = 0; i < OEMMPEG4_MAX_CB; i++)
  {
	pMPEG4->m_cb[i].bInUse = FALSE;
    CALLBACK_Cancel(&pMPEG4->m_cb[i].cb);
  }

  MM_RELEASEIF(pMPEG4->m_pISound);
  MM_RELEASEIF(pMPEG4->m_pIDisplay);
  FREE(pMPEG4);
}

/*==============================================================================

==============================================================================*/
static uint32 OEMMediaMPEG4_Release(IMedia * po)
{
   int32 nRef = AEEMedia_Release(po);

   if (!nRef)
   {
      OEMMediaMPEG4_Delete(po, TRUE);
      guMPEG4RefCnt = 0;
      /* Set guMPEG4RefCnt to 0 since we're allowing only one instance of
      ** MPEG4 media object.
      */
      
      // while release mpeg4 interface, stop the dvice here and enter BYPASS mode 
      {
        vc_union_type vc_data;
        vc_data.dev_run.curr_dev = VC_DEV_PLY_AMR;
        VC_DeviceControl(VC_ITM_DEV_STOP_I, VC_FUNC_CONTROL, &vc_data);
      }
   }

   return nRef;
}
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif

#if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
#error code not present
#endif

/*==============================================================================

==============================================================================*/
static int OEMMediaMPEG4_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   OEMMediaMPEG4 * pme = (OEMMediaMPEG4 *)po;
   AEEMedia * pMedia = pme->m_pMPEG4->m_pMedia;
   int nRet = SUCCESS;
   vc_union_type vc_data;
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif

#if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
#error code not present
#endif


   switch (nParmID)
   {
     #if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
#error code not present
     #endif

     break;

      case MM_PARM_VOLUME:
      {
        if (!pme->m_pMPEG4->m_pISound)
        {
          nRet = EUNSUPPORTED;
        }
        else if ((uint16)p1 > AEE_MAX_VOLUME)
        {
          nRet =  EBADPARM;
        }
        else
        {
//          nRet = OEMMediaMPEG42PV_SetVolume(p1);
             vc_data.parm.p1 = p1;
             VC_DeviceControl(VC_ITM_SET_MEDIA_VOL_I, VC_FUNC_SET_PARM, &vc_data);
        }
        break;
      }
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif
      case MM_PARM_MEDIA_DATA:
        if(p2 > 0)
        {
          /* this is extended data */
          nRet = OEMMediaMPEG4_OpenURN((AEEMediaDataEx *)p1, (int)p2);
          if (nRet == SUCCESS)
          {
            pme->m_nState = MM_STATE_READY;
          }
        }
        else
        {
          nRet = AEEMedia_SetMediaParm(po, nParmID, p1, p2);
          if (nRet == SUCCESS)
          {
            switch( pMedia->m_md.clsData )
            {
              case MMD_BUFFER:  /* playback from buffer */

#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif /* FEATURE_QTV_PSEUDO_STREAM */
  				        /* here video and audio buffers are same */
//                  nRet = OEMMediaMPEG42PV_OpenBufferURN(
//                                    (unsigned char *)pMedia->m_md.pData,
//                                    pMedia->m_md.dwSize,
//                                    (unsigned char *)pMedia->m_md.pData,
//                                    pMedia->m_md.dwSize);

                break;


#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif /* FEARTURE_QTV_PSEUDO_STREAM */
            case MMD_FILE_NAME:

//                nRet = OEMMediaMPEG42PV_OpenURN(pMedia->m_pszFile, pMedia->m_pszFile);
                break;


              default:
                nRet = EUNSUPPORTED;
            } /* switch( pMedia->m_md.clsData ) */
          } /* end of if(AEEMedia_SetMediaParm == SUCCESS) */
        }
        break;

      case MM_MP4_AUDIO_VIDEO_MEDIA_DATA:
//         nRet = OEMMediaMPEG42PV_OpenURN((char *)p1, (char *)p2);
         if (nRet == SUCCESS)
         {
            pme->m_nState = MM_STATE_READY;
         }
         break;

      case MM_PARM_RECT:
         MEMCPY(&pme->m_rectImage, (void *)p1, sizeof(AEERect));
         break;

      case MM_PARM_FRAME:
         pme->m_pMPEG4->m_bFrameCBEnabled = p1;
         break;

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
      case MM_MP4_PARM_ROTATION:
         OEMMediaMPEG42PV_RotateVideo((AEEMediaMPEG4RotationType)p1);
         pme->m_pMPEG4->m_bClearScreen = !pme->m_pMPEG4->m_bFrameCBEnabled;
         break;

      case MM_MP4_PARM_SCALING:
         OEMMediaMPEG42PV_ScaleVideo((AEEMediaMPEG4ScalingType)p1);
         pme->m_pMPEG4->m_bClearScreen = !pme->m_pMPEG4->m_bFrameCBEnabled;
         break;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

      case MM_MP4_PARAM_AUDIO_OVERRIDE:
//         OEMMediaMPEG42PV_SetAudOverride((int32)p1);
         break;

#ifdef FEATURE_QTV_STREAMING
#error code not present
#endif /* FEATURE_QTV_STREAMING */

      case MM_PARM_TICK_TIME:
//         OEMMediaMPEG42PV_SetTickUpdateInterval((uint32)p1);
         break;

      case MM_PARM_POS:
//         nRet = OEMMediaMPEG42PV_SetStartPos((uint32)p1);
         break;

      case MM_MP4_PARM_LOOP_TRACK:
//         nRet = OEMMediaMPEG42PV_SetLoopTrack((boolean)p1);
         break;

#ifdef FEATURE_QTV_MFDRM
      case MM_MP4_PARM_DECRYPT_KEY:
//         nRet = OEMMediaMPEG42PV_SetDRMKey((void*)p1, p2);
         break;
#endif


#ifdef FEATURE_QTV_STREAM_RECORD
#error code not present
#endif

      default:
         nRet = EUNSUPPORTED;
   }

   return nRet;
}


/*==============================================================================

==============================================================================*/
static int OEMMediaMPEG4_GetMediaParm(IMedia * po, int nParmID, int32 * p1, int32 * p2)
{
   OEMMediaMPEG4 * pme = (OEMMediaMPEG4 *)po;
   OEMMediaMPEG4Layer * pMPEG4 = pme->m_pMPEG4;
   int nRet = SUCCESS;

   if (nParmID == MM_PARM_MEDIA_DATA)
      return AEEMedia_GetMediaParm(po, nParmID, p1, p2);

   switch (nParmID)
   {
      case MM_PARM_VOLUME:
//        nRet = OEMMediaMPEG42PV_GetVolume();
        break;

      case MM_PARM_CLSID:
         if (p1 == NULL)
            return EFAILED;

         *p1 = AEECLSID_MEDIAMPEG4;
         break;

      case MM_PARM_CAPS:
         if (p1 == NULL)
            return EFAILED;

         *p1 = (int32)pMPEG4->m_dwCaps;
         if (p2)
            *p2 = 0;
         break;

      case MM_PARM_RECT:
         if ( !p1 | !pme )
         {
           return EFAILED;
         }

         MEMCPY( (void *)p1, &pme->m_rectImage, sizeof(AEERect));
         break;

#ifndef FEATURE_QTV_VIDEO_DISABLE
      case MM_PARM_FRAME:
         if ((p1 == NULL && p2 == NULL) ||
             (p1 != NULL && p2 != NULL))
         { /* Invalid parameters */
            return EFAILED;
         }

         if (p1 == NULL)
         { /* App is calling IMEDIA_IsFrameCallback */
            *p2 = pme->m_pMPEG4->m_bFrameCBEnabled;
         }
         else
         { /* App is calling IMEDIA_GetFrame */
            nRet = OEMMediaMPEG42PV_GetFrame((IBitmap**)p1);
         }
         break;
#endif /* FEATURE_QTV_VIDEO_DISABLE */

      case MM_MP4_PARM_TELOP_SUB_STR:
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
//         OEMMediaMPEG42PV_GetTelopSubString((AEETelopTextSubString **)p1, *p2);
         break;
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

      case MM_MP4_PARM_MEDIA_SPEC:
//         nRet = OEMMediaMPEG42PV_GetMediaSpec((AEEMediaMPEG4Spec **)p1, p2);
         break;

      case MM_MP4_PARM_TRACK_TIME_SCALE:
//         nRet = OEMMediaMPEG42PV_GetTrackTimeScale((AEEMediaMPEG4TrackType)(int32)p1, (uint32 *)p2);
         break;

      case MM_MP4_PARAM_PLAYBACK_POS:
//         nRet = OEMMediaMPEG42PV_GetPlaybackPos((AEEMediaPlaybackPos *)p1);
         break;

#ifdef FEATURE_QTV_SKT_MOD_MIDI
      case MM_MP4_PARM_MIDI_SIZE:
        nRet = OEMMediaMPEG42PV_GetMidiDataSize((uint32 *)p1);
        break;

      case MM_MP4_PARM_MIDI_DATA:
        nRet = OEMMediaMPEG42PV_GetMidiData((uint8 *)p1, (uint32 *)p2);
        break;

      case MM_MP4_PARM_LINK_SIZE:
        nRet = OEMMediaMPEG42PV_GetLinkDataSize((uint32 *)p1);
        break;

      case MM_MP4_PARM_LINK_DATA:
        nRet = OEMMediaMPEG42PV_GetLinkData((uint8 *)p1, (uint32 *)p2);
        break;

#endif /* FEATURE_QTV_SKT_MOD_MIDI */

      default:
         nRet = EUNSUPPORTED;
   }
   /* TODO
   return OEMMediaMPEG4Layer_GetMediaParm(pme->m_pMPEG4, nParmID, p1, p2);
   */
   return nRet;
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_Play(IMedia * po)
{
    int 	ret = 0;
//#if defined(FEATURE_SMALL_DISPLAY_SCREEN)
   OEMMediaMPEG4 *    pme = (OEMMediaMPEG4 *)po;
   AEEMediaCallback *   pcb = pme->m_pMPEG4->m_cb;
   AEEMedia *  pMedia = pme->m_pMPEG4->m_pMedia;
   IShell * pIShell = AEE_GetShell();
    db_items_value_type db_item;
//#endif

   {
          db_item.nCamMode = 0;
          db_put(DB_CAMERA_MODE, &db_item);
   }

   pMediaCB = pme->m_pMPEG4->m_cb;
   ret = AEEMedia_Play(po);
   if (ret != SUCCESS)
      return ret;
#ifdef FEATURE_SMALL_DISPLAY_SCREEN
  //Turn on the default scaling option. 
  pme->m_bscalevideo = TRUE;
#endif 

if(MMD_FILE_NAME == pme->m_pMPEG4->m_pMedia->m_md.clsData)
   {
      char * pFile = NULL;
      vc_union_type vc_data;
      vc_data.play_info.play_mode = VC_PLAY_M;
      pFile = SPLITPATH(pme->m_pMPEG4->m_pMedia->m_pszFile, OEMFS_CARD0_DIR);
      vc_data.play_info.file_scr = VC_IN_CORE_FILE;
      vc_data.play_info.pFileaddr = NULL;
      if(!pFile)
      {
        pFile = SPLITPATH(pme->m_pMPEG4->m_pMedia->m_pszFile, OEMFS_BREW_DIR);
        vc_data.play_info.file_scr = VC_IN_BASEBAND_RAM;
      }
      STRCPY((char *)&vc_data.play_info.szFileName, pFile);
      VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);
      if(VC_GetDeviceState() != VC_STAT_DONE)
        {
            db_item.nCamMode = 1;
            db_put(DB_CAMERA_MODE, &db_item);
			
            pcb->cmdNotify.nStatus = MM_STATUS_ABORT;
            pcb->cmdNotify.nCmd = MM_CMD_PLAY;
            OEMMediaMPEG4_CallbackNotify(pcb);
            return EFAILED;
        }
	else
	{
	     pcb->cmdNotify.nStatus = MM_STATUS_START;
	     pcb->cmdNotify.nCmd = MM_CMD_PLAY;
	     OEMMediaMPEG4_CallbackNotify(pcb);
	     ISHELL_SetTimer(pIShell, 1000,(PFNNOTIFY)CMediaMMLayer_CallbackTimer,pcb);
	     AEEMedia_SetState(pMedia, MM_STATE_PLAY);
	}
      return SUCCESS;
    }  
   return EFAILED;
}

/*==================================================================
   Unsupported functions -- used in vtbl of IMedia classes
==================================================================*/
static int OEMMediaMPEG4_RecordUnsupported(IMedia * po)
{
   return EUNSUPPORTED;
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_Stop(IMedia * po)
{
   int            nRet;
   AEEMedia *  pme = (AEEMedia *)po;
   vc_union_type vc_data;

   {
          db_items_value_type db_item;
          db_item.nCamMode = 1;
          db_put(DB_CAMERA_MODE, &db_item);
   }
   
   nRet = AEEMedia_Stop(po);
   if (nRet != SUCCESS)
      return nRet;

   vc_data.play_info.play_mode = VC_STOP_M;
   VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);
   AEEMedia_SetState(pme, MM_STATE_READY);

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   int            nRet;
   MMD_U32    curTime,totTime;
   vc_union_type vc_data;

   nRet = AEEMedia_Seek(po, eSeek, lTimeMS);
   if (nRet != SUCCESS)
      return nRet;

//   return OEMMediaMPEG42PV_Seek(eSeek, lTimeMS);
     if (lTimeMS < 0)
             if(VC_GetPlayMode() == VC_NONE_M)
              {
                 return FAILED;
		}
	     else
	     	{
	     	   MMD_Player_File_GetPlayTime(pFileHandle,&curTime);
                 if((int32)curTime > (-lTimeMS))
                 	{
                       curTime += lTimeMS;
                       vc_data.parm.p1 = curTime;
                       vc_data.play_info.play_mode = VC_SEEK_M;
                       nRet = VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);

                      if(nRet != VC_STAT_DONE)
                        {
                           return EFAILED;
                        }
                 	}
                 else
                 	{
                        MMD_Player_File_Stop(pFileHandle);
			            MMD_Player_File_Play(pFileHandle,(PLYCallback)OEMMedia_VC_PlayCB);
                 	}
	     	}		
         else
            //cmx_audfmt_fforward(lTimeMS, pfn, pUser);
            if(VC_GetPlayMode() == VC_NONE_M)
              {
                 return FAILED;
		}
	     else
	     	{
	     	   MMD_Player_File_GetPlayTime(pFileHandle,&curTime);
		   MMD_Player_File_GetTotalTime(pFileHandle,&totTime);
		   curTime += lTimeMS;
                 if(curTime >totTime)
                 	{
                       curTime = totTime;
                 	}
                vc_data.parm.p1 = curTime;
                vc_data.play_info.play_mode = VC_SEEK_M;
                nRet = VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);
		  if(nRet != VC_STAT_DONE)
                 {
                    return EFAILED;
                 }
	     	}
	return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_Pause(IMedia * po)
{
   int            nRet;
   vc_union_type vc_data;
   IShell * pIShell = AEE_GetShell();
   AEEMediaCallback *   pcb = (AEEMediaCallback *)pMediaCB;
   
   nRet = AEEMedia_Pause(po);
   if (nRet != SUCCESS)
      return nRet;
#if 0   
   nRet = MMD_Player_File_Pause( MediaFile.pPlayFileHandle);
   if(nRet)
   {
      nRet = MMD_Player_File_Close( MediaFile.pPlayFileHandle,NULL,NULL);
      return nRet;
   }
#endif
    ISHELL_CancelTimer(pIShell,(PFNNOTIFY)(CMediaMMLayer_CallbackTimer),pMediaCB);
   vc_data.play_info.play_mode = VC_PAUSE_M;
   VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);

   VC_DeviceControl(VC_ITM_GET_PLAY_TIME_I, VC_FUNC_GET_PARM, &vc_data);
   pcb->cmdNotify.pCmdData = (void*)vc_data.parm.p1;
   pcb->cmdNotify.nCmd = MM_CMD_PLAY;
   pcb->cmdNotify.nStatus = MM_STATUS_PAUSE;
   OEMMediaMPEG4_CallbackNotify(pcb);

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_Resume(IMedia * po)
{
   int            nRet;
   vc_union_type vc_data;
   IShell * pIShell = AEE_GetShell();
   AEEMediaCallback *   pcb = (AEEMediaCallback *)pMediaCB;
   
   nRet = AEEMedia_Resume(po);
   if (nRet != SUCCESS)
      return nRet;
#if 0
   nRet = MMD_Player_File_Resume( MediaFile.pPlayFileHandle);
   if(nRet)
   {
      nRet = MMD_Player_File_Close( MediaFile.pPlayFileHandle,NULL,NULL);
      return nRet;
   }
#endif
   vc_data.play_info.play_mode = VC_RESUME_M;
   VC_DeviceControl(VC_ITM_PLAY_MODE_I, VC_FUNC_PLAY_ON, &vc_data);

   VC_DeviceControl(VC_ITM_GET_PLAY_TIME_I, VC_FUNC_GET_PARM, &vc_data);
   pcb->cmdNotify.pCmdData = (void*)vc_data.parm.p1;
   pcb->cmdNotify.nCmd = MM_CMD_PLAY;
   pcb->cmdNotify.nStatus = MM_STATUS_RESUME;
   OEMMediaMPEG4_CallbackNotify(pcb);

   ISHELL_SetTimer(pIShell, 1000,(PFNNOTIFY)CMediaMMLayer_CallbackTimer,pMediaCB);

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_GetTotalTime(IMedia * po)
{
   int            nRet;
   char * pFile = NULL;
   static vc_union_type vc_data;

    OEMMediaMPEG4 *    pme = (OEMMediaMPEG4 *)po;
    AEEMediaCallback *   pcb = pme->m_pMPEG4->m_cb;

   nRet = AEEMedia_GetTotalTime(po);
   if (nRet != SUCCESS)
      return nRet;

   vc_data.play_info.play_mode = VC_PLAY_M;
   pFile = SPLITPATH(pme->m_pMPEG4->m_pMedia->m_pszFile, OEMFS_CARD0_DIR);
   vc_data.play_info.file_scr = VC_IN_CORE_FILE;
   if(!pFile)
   {
      pFile = SPLITPATH(pme->m_pMPEG4->m_pMedia->m_pszFile, OEMFS_BREW_DIR);
      vc_data.play_info.file_scr = VC_IN_BASEBAND_RAM;
   }
   STRCPY((char *)&vc_data.play_info.szFileName, pFile);

   VC_DeviceControl(VC_ITM_GET_PLAY_TOTALTIME_I, VC_FUNC_GET_PARM, &vc_data);
   if(VC_GetDeviceState() == VC_STAT_DONE)
   {
      pcb->cmdNotify.pCmdData = (void*)vc_data.parm.p1;
   }
   else
   {
      pcb->cmdNotify.pCmdData = NULL;
   }
   pcb->cmdNotify.nStatus = MM_STATUS_DONE;
   pcb->cmdNotify.nCmd = MM_CMD_GETTOTALTIME;
   pcb->cmdNotify.pCmdData = (void*)vc_data.parm.p1;

   OEMMediaMPEG4_CallbackNotify(pcb);
//   return OEMMediaMPEG42PV_GetClipInfo();
   return SUCCESS;
}

/*==================================================================

   Callback Notification Functions: called in BREW context after
   ISHELL_Resume()

==================================================================*/
/*==================================================================
   Media callback notify -- called in BREW context.
==================================================================*/
void OEMMediaMPEG4_CallbackNotify(AEEMediaCallback * pcb)
{
  OEMMediaMPEG4 *    pme;

#ifdef FEATURE_SMALL_DISPLAY_SCREEN
  uint32 scr_width=0, scr_height=0;
#endif

  if (!pcb)
    return;

  pme = (OEMMediaMPEG4 *)pcb->cmdNotify.pIMedia;
#if 0
  if (  pcb->cmdNotify.nCmd    == MM_CMD_PLAY &&
        pcb->cmdNotify.nStatus == MM_STATUS_FRAME &&
        !pme->m_pMPEG4->m_bFrameCBEnabled )
  {
    /* We're about to send an MM_STATUS_FRAME to the Brew app.  However, they
    * haven't enabled them, so handle it ourselves and return.
    */
    IBitmap      * pFrame;
    AEEBitmapInfo  bi;
    uint16 x,  y;
    uint16 dx, dy;

    /*
    * IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame) after you're done with pFrame.
    */
    IMEDIA_GetFrame((IMedia *)pme, &pFrame);
    if (pFrame != NULL)
    {
      // Get the bitmap size
      IBITMAP_GetInfo(pFrame, &bi, sizeof(bi));

      /* Should clear the screen if we've indicated and the dimensions of the
      * current frame have changed from the previous one.
      *
      * 'and' is not sufficient here.  There's a race condition where a button masher
      * pushing 'rotate' at full speed can cause two rotations between processings of
      * this loop, resulting in an improper screen erase between rotations.  We need
      * to clear the screen whenever the flag is high OR the image dimensions have changed.
      */
      if( pme->m_pMPEG4->m_bClearScreen ||
          (pme->m_pMPEG4->m_iPrevWidth  != bi.cx ||
          pme->m_pMPEG4->m_iPrevHeight != bi.cy) )
      {
        /* Frame callbacks are not enabled, so clear the screen */
#ifndef FEATURE_QTV_MDP
        IDISPLAY_EraseRgn(  pme->m_pMPEG4->m_pIDisplay,
                            pme->m_rectImage.x,
                            pme->m_rectImage.y,
                            pme->m_rectImage.dx,
                            pme->m_rectImage.dy );
#endif /* FEATURE_QTV_MDP */
        pme->m_pMPEG4->m_bClearScreen = FALSE;
      }

#ifdef FEATURE_SMALL_DISPLAY_SCREEN
      //For 6030 we will turn on m_bscalevideo at the start when we play a clip.
      //We will scale the video only if the available screen size is less
      //than the video size. For scaling down we have 3 available options
      //MM_MPEG4_0P25X_SHRINK,MM_MPEG4_0P75X_SHRINK & MM_MPEG4_2X_SHRINK.
      if(pme->m_bscalevideo)
      {
         //Calculate the screen width available for display.
	 scr_width = pme->m_rectImage.dx - pme->m_rectImage.x;
	 scr_height = pme->m_rectImage.dy - pme->m_rectImage.y;

         if((bi.cx > scr_width) || (bi.cy > scr_height))
	 {
            //If the video size is no bigger than 30% of screen size
	    if((bi.cx <= 1.3*scr_width) && (bi.cy <= 1.3*scr_height))
            {
	       OEMMediaMPEG42PV_ScaleVideo((AEEMediaMPEG4ScalingType)MM_MPEG4_0P25X_SHRINK);
	    }
	    else //If the video size is between 30% - 60% more than screen size
	    if((bi.cx <= 1.6*scr_width) && (bi.cy <= 1.6*scr_height))
	    {
               OEMMediaMPEG42PV_ScaleVideo((AEEMediaMPEG4ScalingType)MM_MPEG4_0P75X_SHRINK); 
	    }
	    else //If the video size is bigger than 60% of the screen size
	    {
	       OEMMediaMPEG42PV_ScaleVideo((AEEMediaMPEG4ScalingType)MM_MPEG4_2X_SHRINK); 
	    }
	    pme->m_pMPEG4->m_bClearScreen = !pme->m_pMPEG4->m_bFrameCBEnabled;	
         }
         //After we have scaled the video we should turn this flag off since
         //we want the video to be scaled only once at the start of play.
         pme->m_bscalevideo = FALSE;
      }
#endif /* FEATURE_SMALL_DISPLAY_SCREEN */

      /* Save the dimensions of the current frame */
      pme->m_pMPEG4->m_iPrevWidth  = bi.cx;
      pme->m_pMPEG4->m_iPrevHeight = bi.cy;

      dx = MIN( pme->m_rectImage.dx, bi.cx );
      dy = MIN( pme->m_rectImage.dy, bi.cy );
      x = (uint16)((pme->m_rectImage.dx - dx) / 2 + pme->m_rectImage.x);
      y = (uint16)((pme->m_rectImage.dy - dy) / 2 + pme->m_rectImage.y);

#ifdef FEATURE_QTV_MDP
      {
        /* Use MDP for post processing and display */
        MDPIMG mdpImg;
        IDIB *pYCbCr;

        /* Obtain IDIB pointer from an IBitmap */
        if (IBITMAP_QueryInterface(pFrame, AEECLSID_DIB, (void**)&pYCbCr) != SUCCESS)
        {
          MSG_ERROR("Invalid interface ID",0,0,0);
          mp4_free_frame_buffer(NULL);
          IBITMAP_Release(pFrame);
          return;
        }
        if (!pYCbCr)
        {
          MSG_ERROR("Could not get IDIB",0,0,0);
          mp4_free_frame_buffer(NULL);
          IBITMAP_Release(pFrame);
          return;
        }

        mdpImg.dest = PRIMARY_LCD_TYPE;
        mdpImg.imgType = MDP_YCBCR420;
        mdpImg.bmy_addr = (uint32 *)pYCbCr->pBmp;
        mdpImg.cbcr_addr = (uint32 *)&pYCbCr->pBmp[bi.cx * bi.cy];

        mdpImg.width = bi.cx;
        mdpImg.cwin.x1 = 0;
        mdpImg.cwin.x2 = dx-1;
        mdpImg.cwin.y1 = 0;
        mdpImg.cwin.y2 = dy-1;
        mdpImg.lcd_x = x;
        mdpImg.lcd_y = y;
        mdpImg.mdpOp = MDPOP_NOP;

        /* Display the specified image on the LCD screen */
        if (1 != mdp_disp_update(&mdpImg))
        {
          MSG_ERROR("MDP display update failed",0,0,0);
        }

        /* The IDIB pointer must be released once we are finished using it */
        IDIB_Release(pYCbCr);

        /* Signal the codec to tell it that the MDP has finished accessing the
        ** frame buffer. The call to mp4_free_frame_buffer() is tentative;
        ** a cleaner buffer management mechanism will need to be implemented to
        ** support future enhancements.
        */
        mp4_free_frame_buffer(NULL);
      }
#else
      /* Display the frame at centered on the screen */
      IDISPLAY_BitBlt(pme->m_pMPEG4->m_pIDisplay, x, y, dx, dy, pFrame, 0, 0, AEE_RO_COPY);
      IDISPLAY_Update(pme->m_pMPEG4->m_pIDisplay);
#endif /* FEATURE_QTV_MDP */
      IBITMAP_Release(pFrame);
    }
#ifdef FEATURE_QTV_MDP
    else
    {
      MSG_ERROR("pFrame is NULL",0,0,0);
      mp4_free_frame_buffer(NULL);
    }
#endif /* FEATURE_QTV_MDP */
    /* Do not send this callback to the app as they are not registered for it */
    return;
  }
 #endif
  AEEMedia_CallbackNotify((AEEMedia *)pme, pcb);
}

#ifdef FEATURE_MP4_MSM6050
static void OEMMediaMPEG4_ObjMgrNotify(void * pUser, IBase * pObject)
{
   OEMMediaMPEG4 * pme = (OEMMediaMPEG4 *)pUser;

   if (!pUser || !pObject)
      return;

   // Stop the media, if playback/recording is in progress. This also releases
   // most of device multimedia resources.
   if (AEEMedia_IsPlayRecOn(pme))
      IMEDIA_Stop((IMedia *)pme);

   // Release the allocated resources but do not delete the object itself.
   OEMMediaMPEG4_Delete((IMedia *)pUser, FALSE);
}
#endif /* FEATURE_MP4_MSM6050 */

/*==================================================================

==================================================================*/
static void OEMMediaMPEG4_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eStatus, uint32 dwParam)
{
   OEMMediaMPEG4 *      pme = (OEMMediaMPEG4 *)pUser;
   AEESoundCmdData *    pData = (AEESoundCmdData *)dwParam;
   AEEMediaCallback     mcb;
   void *               paLast;

   MEMCPY(&mcb, &pme->m_pMPEG4->m_cbCommand, sizeof(AEEMediaCallback));

   mcb.bInUse = TRUE;
   mcb.cmdNotify.nSubCmd = MM_PARM_VOLUME;
   mcb.cmdNotify.nCmd = (eCBType == AEE_SOUND_VOLUME_CB) ? MM_CMD_GETMEDIAPARM : MM_CMD_SETMEDIAPARM;
   mcb.cmdNotify.nStatus = (eStatus == AEE_SOUND_SUCCESS) ? MM_STATUS_DONE : MM_STATUS_ABORT;
   mcb.cmdNotify.pCmdData = NULL;
   mcb.cmdNotify.dwSize = 0;

   if (pData && eCBType == AEE_SOUND_VOLUME_CB && eStatus == AEE_SOUND_SUCCESS)
   {
      mcb.cmdNotify.pCmdData = (void *)(uint32)pData->wVolume;
      mcb.cmdNotify.dwSize = sizeof(pData->wVolume);
   }

   paLast = AEE_SetAppContext(pme->m_pMPEG4->m_pAppContext);
   AEEMedia_CallbackNotify((AEEMedia *)pme, &mcb);
   AEE_SetAppContext(paLast);
}

/*==================================================================
FUNCTION:  OEMMediaMPEG4_Notify()
DESCRIPTION:
   Callback function queued by the PV MPEG4 engine to run in BREW's
   context.
==================================================================*/
static void OEMMediaMPEG4_Notify(AEEMediaCallback * pcb)
{
   OEMMediaMPEG4 *      pMedia;
   OEMMediaMPEG4Layer * pme;
   AEEMediaCallback   mcb;

   if (!pcb)
      return;

   pMedia = (OEMMediaMPEG4 *)pcb->cmdNotify.pIMedia;
   if (!pMedia)
      return;

   pme = pMedia->m_pMPEG4;
   if (!pme)
      return;

   MEMCPY(&mcb, pcb, sizeof(AEEMediaCallback));

   /* Free the callback structure */
   pcb->bInUse = FALSE;

   if (pme->m_pfnNotify)
      pme->m_pfnNotify(&mcb);
}

/*==================================================================
   IMediaMPEG4 constructor functions
==================================================================*/
OEMMediaMPEG4Layer * OEMMediaMPEG4Layer_New(IMedia * po, PFNNOTIFY pfn)
{
  OEMMediaMPEG4Layer *   pme;
  int           i;
  int           nRetVal;
  AEEDeviceInfo di;

  /* Only allow one instantiation of the MPEG4 IMedia object */
  if( !po || (guMPEG4RefCnt != 0) )
    return NULL;

  pme = MALLOC(sizeof(OEMMediaMPEG4Layer));

  if (!pme)
    return NULL;

  MEMSET(pme,0,sizeof(OEMMediaMPEG4Layer));
  pme->m_pMedia = (AEEMedia *)po;
  pme->m_pAppContext = AEE_GetAppContext();

#ifdef FEATURE_MP4_MSM6050
  gpObjectMgr = CObjectMgr_New(pme->m_pMedia->m_pIShell);
  if (!gpObjectMgr)
  {
    FREE(pme);
    return NULL;
  }

  pme->m_cxtMedia = CObjectMgr_Register(gpObjectMgr, (IBase*)pme->m_pMedia,
  OEMMediaMPEG4_ObjMgrNotify, pme->m_pMedia);
  if (!pme->m_cxtMedia)
  {
    FREE(pme);
    return NULL;
  }
#else /* FEATURE_MP4_MSM6050 */
  if (AEEObjectMgr_Register(pme->m_pMedia, &pme->m_hObject) != SUCCESS)
  {
    FREE(pme);
    return NULL;
  }
#endif /* FEATURE_MP4_MSM6050 */

  pme->m_pfnNotify = pfn;  // pfn is OEMMediaMPEG4_CallbackNotify
  pme->m_dwCaps = MM_CAPS_AUDIO | MM_CAPS_VIDEO;

  // Initialize callbacks.
  for (i = 0; i < OEMMPEG4_MAX_CB; i++)
  {
    pme->m_cb[i].bInUse = FALSE;
    pme->m_cb[i].cmdNotify.pIMedia = po;
    pme->m_cb[i].cmdNotify.clsMedia = ((AEEMedia *)po)->m_clsSelf;
    pme->m_cb[i].cmdNotify.nStatus = 0;
    CALLBACK_Init(&pme->m_cb[i].cb, OEMMediaMPEG4_Notify, &pme->m_cb[i]);
  }

  ISHELL_GetDeviceInfo(pme->m_pMedia->m_pIShell, &di);

  /* Get a pointer to the IDisplay */
  nRetVal = ISHELL_CreateInstance(pme->m_pMedia->m_pIShell, AEECLSID_DISPLAY, (void **)&pme->m_pIDisplay);
  if( nRetVal != SUCCESS )
  {
#ifdef FEATURE_MP4_MSM6050
    CObjectMgr_Unregister(gpObjectMgr, pme->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
    AEEObjectMgr_Unregister(pme->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */
    FREE(pme);
    return NULL;
  }

  nRetVal = ISHELL_CreateInstance(pme->m_pMedia->m_pIShell, AEECLSID_SOUND, (void **)&pme->m_pISound);
  if(SUCCESS != nRetVal)
  {
    MM_RELEASEIF(pme->m_pIDisplay);
#ifdef FEATURE_MP4_MSM6050
    CObjectMgr_Unregister(gpObjectMgr, pme->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
    AEEObjectMgr_Unregister(pme->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */
    FREE(pme);
    return NULL;
  }

  ISOUND_RegisterNotify(pme->m_pISound, OEMMediaMPEG4_SoundCB, pme);

  /* Initialize other variables */
  pme->m_bFrameCBEnabled = FALSE;
  pme->m_iPrevHeight = pme->m_iPrevWidth = 0;

  // At this point, the media player has not made a call informing the OEM
  // layer the size of the display area.  For now, we'll call
  // PVPlayer::Init() with a 0 x 0 display area.  When the media player sets
  // the available display area for the video, we'll forward that information
  // to the PV MPEG4 engine via PVPlayer::SetDisplayArea().
//  nRetVal = OEMMediaMPEG42PV_Init();
  if( nRetVal != SUCCESS )
  {
    MM_RELEASEIF(pme->m_pIDisplay);
    MM_RELEASEIF(pme->m_pISound);
#ifdef FEATURE_MP4_MSM6050
    CObjectMgr_Unregister(gpObjectMgr, pme->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
    AEEObjectMgr_Unregister(pme->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */
    FREE(pme);
    return NULL;
  }

#ifdef FEATURE_MP4_MSM6050
//  nRetVal = OEMMediaMPEG42PV_RegisterForCallback(pme->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
//  nRetVal = OEMMediaMPEG42PV_RegisterForCallback((void*)pme->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */

  if (nRetVal != SUCCESS)
  {
    MM_RELEASEIF(pme->m_pIDisplay);
    MM_RELEASEIF(pme->m_pISound);
#ifdef FEATURE_MP4_MSM6050
    CObjectMgr_Unregister(gpObjectMgr, pme->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
    AEEObjectMgr_Unregister(pme->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */
    pme->bTerminating=TRUE;
//    OEMMediaMPEG42PV_Terminate();
	pme->bTerminating=FALSE;
    FREE(pme);
    pme = NULL;
  }

  return pme;
}

static IBase * OEMMediaMPEG4_New(IShell * ps, AEECLSID cls)
{
  IMedia *      po;
  OEMMediaMPEG4 * pme;

  // Alloc memory for the object
  po = (IMedia *)AEE_NewClass((IBaseVtbl *)&gMediaMPEG4Funcs, sizeof(OEMMediaMPEG4));
  if (!po)
    return NULL;

  // Call base class constructor
  if (SUCCESS != AEEMedia_New(po, ps, cls))
  {
    FREE(po);
    return NULL;
  }

  // Call MPEG4 Layer constructor
  pme = (OEMMediaMPEG4 *)po;
  pme->m_pMPEG4 = OEMMediaMPEG4Layer_New(po, (PFNNOTIFY)OEMMediaMPEG4_CallbackNotify);

  if ( !pme->m_pMPEG4 )
  {
    AEEMedia_Delete(po);
    FREE(po);
    return NULL;
  }
  return (IBase *)po;
}

/*===============================================================================

                     IMediaMPEG4 PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==================================================================
   Called only once during AEE initialization
==================================================================*/
void IMediaVC0848MPEG4_Init(IShell * ps)
{
   AEEMedia_Init(ps, MT_VIDEO_MPEG4, AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/url", AEECLSID_MEDIAMPEG4);
#ifndef FEATURE_APP_SDP
   AEEMedia_Init(ps, "video/sdp", AEECLSID_MEDIAMPEG4);
#endif
   AEEMedia_Init(ps, "video/pvx", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/3gp", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/amc", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/3g2", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/k3g", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/skm", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/mfpt", AEECLSID_MEDIAMPEG4);
}

/*==================================================================
   Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaVC0848MPEG4_New(IShell * ps, AEECLSID cls, void **ppif)
{
   IBase * pobj;
   if(SUCCESS != oem_media_scheduler())
   {
        return EITEMBUSY;
   }

   if (guMPEG4RefCnt == 0)
   {
      pobj = OEMMediaMPEG4_New(ps, cls);
	  if(!pobj) return  EFAILED;	  
      guMPEG4RefCnt = (pobj) ? 1 : 0;
   }
   else
   {
      pobj = NULL;
   }

   *ppif = pobj;
   {
      vc_union_type vc_data;
      vc_data.dev_run.curr_dev = VC_DEV_PLY_AMR;
      VC_DeviceControl(VC_ITM_DEV_OPEN_I, VC_FUNC_CONTROL, &vc_data); // init hardware for mp3 player
   }

   return pobj ? SUCCESS : ENOMEMORY;
}

/*==================================================================
   Called by the PV MPEG4 Engine callback function to queue a
   callback to execute in the BREW context
==================================================================*/
#if 0
int OEMMediaMPEG4_QueueCallback(void *pClientData, int nStatus, int nCmd, int nSubCmd, void *pData, uint32 dwSize)
{
   int i,j;
   OEMMediaMPEG4      * pMedia;
   OEMMediaMPEG4Layer * pme;
   AEEMediaCallback   * pcb = NULL;

#ifdef FEATURE_MP4_MSM6050
   pMedia = (OEMMediaMPEG4 *)CObjectMgr_GetObject(gpObjectMgr, (OBJECTCONTEXT)pClientData);
#else /* FEATURE_MP4_MSM6050 */
   pMedia = (OEMMediaMPEG4 *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
#endif /* FEATURE_MP4_MSM6050 */
   pme = pMedia->m_pMPEG4;

   /* Find a free callback structure */
   MUTEX_LOCK();
   for (i = 0; i < OEMMPEG4_MAX_CB; i++)
   {
      if (pme->m_cb[i].bInUse == FALSE)
      {
         pme->m_cb[i].bInUse = TRUE;
         pcb = &pme->m_cb[i];
         break;
      }
   }
   MUTEX_UNLOCK();

   if (i == OEMMPEG4_MAX_CB)
   {
	  if(pme->bTerminating)
	  {
	      return -1;
	  }
      else	{
		  unsigned int totalSleepDuration = 0;
		  do{

              /* sleep in chuncks of 16ms, until a callback is free*/
			  rex_sleep(16);
			  totalSleepDuration += 16;
			  MUTEX_LOCK();
			  for (j = 0; j < OEMMPEG4_MAX_CB; j++)
			   {
				  if (pme->m_cb[j].bInUse == FALSE)
   				  {
					 pme->m_cb[j].bInUse = TRUE;
					 pcb = &pme->m_cb[j];
					 break;
				  }
			   }
			  MUTEX_UNLOCK();
		  }while(j== OEMMPEG4_MAX_CB && totalSleepDuration < UI_TRANSITION_TO_RUN_TIMEOUT);


		  if(j== OEMMPEG4_MAX_CB){
      		MSG_FATAL("Out of callback structures", 0, 0, 0);
      		return -1;
   		  }
	  }
   }

   pcb->cmdNotify.nStatus = nStatus;
   pcb->cmdNotify.nCmd = nCmd;
   pcb->cmdNotify.nSubCmd = nSubCmd;
   pcb->cmdNotify.pCmdData = pData;
   pcb->cmdNotify.dwSize = dwSize;
#if MIN_BREW_VERSION(3,0)
   AEE_ResumeCallback(&pcb->cb, pme->m_pAppContext);
#else
   AEE_ResumeEx(&pcb->cb, 0, pme->m_pAppContext);
#endif /* MIN_BREW_VERSION(3,0) */
   return 0;
}
#endif
/*==================================================================
  This function extracts audio/video file names from extended media
  data structure and then calls QTV openURN function.
==================================================================*/
int OEMMediaMPEG4_OpenURN(AEEMediaDataEx *pmdExList, int nCount)
{
  AEECLSID  clsData;
  int       i;

  if( !pmdExList || !nCount )
    return EBADPARM;

  /* currently QTV only support one type of input (file(s) or buffer(s)) */
  clsData = pmdExList[0].clsData;
  for(i=1; i<nCount; i++)
  {
    if(clsData != pmdExList[i].clsData)
      return EUNSUPPORTED;
  }

  if(clsData ==  MMD_FILE_NAME)   /* all members are file name */
  {
    char     *pVideoFile=NULL;
    char     *pAudioFile=NULL;

    /* this will accept first audio or video file in the list */
    for(i=0; i<nCount; i++)
    {
      if( !pVideoFile && (pmdExList[i].dwCaps & MM_CAPS_VIDEO) )
      {
        pVideoFile = (char *)pmdExList[i].pData;
      }
      if( !pAudioFile && (pmdExList[i].dwCaps & MM_CAPS_AUDIO) )
      {
        pAudioFile = (char *)pmdExList[i].pData;
      }
    }
    if(pVideoFile || pAudioFile)
    {
//      return OEMMediaMPEG42PV_OpenURN(pVideoFile, pAudioFile);
      return EUNSUPPORTED;
    }
  }
  else if(clsData ==  MMD_BUFFER) /* all members are buffer pointers */
  {
    unsigned char *pVideoBuf=NULL;
    unsigned char *pAudioBuf=NULL;
    uint32         nVideoBufSize=0;
    uint32         nAudioBufSize=0;

    /* this will accept first audio or video file in the list */
    for(i=0; i<nCount; i++)
    {
      if( !nVideoBufSize && (pmdExList[i].dwCaps & MM_CAPS_VIDEO) )
      {
        pVideoBuf = (unsigned char *)pmdExList[i].pData;
        nVideoBufSize = pmdExList[i].dwSize;
      }
      if( !nAudioBufSize && (pmdExList[i].dwCaps & MM_CAPS_AUDIO) )
      {
        pAudioBuf = (unsigned char *)pmdExList[i].pData;
        nAudioBufSize = pmdExList[i].dwSize;
      }
    }
    if(nVideoBufSize || nAudioBufSize)
    {
//      return OEMMediaMPEG42PV_OpenBufferURN(pVideoBuf, nVideoBufSize, pAudioBuf, nAudioBufSize);
      return EUNSUPPORTED;
    }
  }
  return EBADPARM;
}

/*=========================================================================
Function: CREATEMEDIAEX

Description:
   Given AEEMediaData, this function analyzes media data and creates
   IMedia-based object (IMedia object).

Prototype:

   int AEEMediaUtil_CreateMediaEx( IShell * ps,
                                   AEEMediaData * pmdList,
                                   int * pnCapsList,
                                   int nListCount,
                                   IMedia ** ppm );

Parameters:
   ps  [in]:        Shell pointer
   pmdList [in]:    Media data info list
   pnCapsList [in]: Media data capability list
   nListCount [in]: number of media data elements in the list
   ppm [out]:       IMedia object pointer returned to caller

Return Value:
   SUCCESS:          IMedia object successfully created
   EBADPARM:         Input parameter(s) is wrong
   EUNSUPPORTED:     Class not found
   MM_EBADMEDIADATA: Media data is not correct
   MM_ENOMEDIAMEMORY:No memory to create IMedia object
   ENEEDMORE:        Need more data to create IMedia object.
                     For MMD_ISOURCE, call IPEEK_Peekable(). (See Comments).
   MM_EPEEK:         Error while peeking for the data. (See Comments)
   MM_ENOTENOUGHDATA Aborted due to insufficient data
   EFAILED:          General failure

Comments:
  repeate following tests for all elements in the list
    If pmdList[i]->clsData is MMD_FILE_NAME, then
    (1) file extension is checked to see if any IMedia class is registered
        for the extension.
    (2) If not, the file is opened and file contents are read. Using
        ISHELL_DetectType(), the MIME of the media data is determined and IMedia class
        is found from Shell registry.

    If pmd->clsData is MMD_BUFFER, then
    (1) The buffer contents are analyzed using ISHELL_DetectType() to determine the MIME
        of the media data is determined and IMedia class is found from Shell registry.

    If pmd->clsData is MMD_ISOURCE, then
    (1) The caller needs to set pmd->pData to IPeek *.
    (2) This function peeks for the data to see if enough data is available. The buffer
        contents are analyzed using ISHELL_DetectType(), the MIME of the media data is determined
        and IMedia class is found from Shell registry.
    (3) If enough data is not there, then it calls IPEEK_Peek() requesting required
        number of bytes. If IPEEK_Peek() returns IPEEK_WAIT, then it returns
        ENEEDMORE. In response to this, caller needs to call IPEEK_Peekable().
        After the data is becomes available, caller should call this function again to
        create IMedia object.

   If IMedia class is found and all the elements point to same class, then IMedia object is
   created and media data is set. This puts IMedia object in Ready state.

Side Effects:
   None.

See Also:
   ISHELL_DetectType()

========================================================================= */
int AEEMedia848Util_CreateMediaEx( IShell * ps,
                                AEEMediaDataEx * pmdList,
                                int nListCount,
                                IMedia ** ppm )
{
  AEECLSID        Cls;
  AEECLSID        tempCls;
  IMedia *        pMedia;
  AEEMediaData    mediaData;
  int             nRet;
  int             i;

  if (!ps || !pmdList || !nListCount || !ppm)
    return EBADPARM;

  for(i=0; i<nListCount; i++)
  {
    if( !(pmdList[i].pData) )
      return EBADPARM;
  }

  *ppm = NULL;

  // Find the class ID. This function uses ISHELL_DetectType() API to get the IMedia
  // MIME and subsequently class ID. All the media data elements should point to the
  // same class ID.
  // currently find class function only works for AEEMediaData structure
  mediaData.clsData = pmdList[0].clsData;
  mediaData.pData   = pmdList[0].pData;
  mediaData.dwSize  = pmdList[0].dwSize;
  nRet = AEEMediaUtil_FindClass(ps, &mediaData, &Cls);
  if (nRet != SUCCESS)
    return nRet;

  for(i=1; i<nListCount; i++)
  {
    mediaData.clsData = pmdList[i].clsData;
    mediaData.pData   = pmdList[i].pData;
    mediaData.dwSize  = pmdList[i].dwSize;
    nRet = AEEMediaUtil_FindClass(ps, &mediaData, &tempCls);

    if (nRet != SUCCESS)
      return nRet;

    if(Cls != tempCls)
      return MM_EBADMEDIADATA;
  }

  // Create IMedia-based object
  if (ISHELL_CreateInstance(ps, Cls, (void **)&pMedia) || !pMedia)
    return MM_ENOMEDIAMEMORY;

#if MIN_BREW_VERSION(3,0)
   /* this will resolve the relative file path to full file path */
  if(OEMMediaMpeg4_FilePathEx(pmdList, nListCount) != SUCCESS)
    return MM_EBADMEDIADATA;
#endif

  // Set the media data and put IMedia in Ready state.
  nRet = IMEDIA_SetMediaDataEx(pMedia, pmdList, nListCount);
  if (nRet != SUCCESS)
  {
    IMEDIA_Release(pMedia);
    return nRet;
  }

  *ppm = pMedia;

  return SUCCESS;
}

#if MIN_BREW_VERSION(3,0)
/*==================================================================
This function resolves the file path for AEEMediaDataEx, which is
not done by BREW.
==================================================================*/
static int OEMMediaMpeg4_FilePathEx(AEEMediaDataEx * pmdList, int nListCount)
{
  int i, nLen;
  char *pszFS = NULL;
  int nRet = EFAILED;

  //EFS and MMC start tags to check if the path specified 
  //in .mfpt file is a full path or relative path.
  
  char* EfsFullPathStartTag = "brew/";
  char* MmcFullPathStartTag = "mmc1/";

  if (!pmdList || !nListCount) 
    goto Done;

  for(i=0, nLen=0; i<nListCount; i++)
  {

   //If the path starts with "brew/" or "mmc1/" then its a full path and 
   //AEE_ResolvePath should be skipped. For playing from EFS, relative or 
   //full path will work, however, when playing from memocr card,
   //user needs to specify full path as 
   //AEE_ResolvePath doesn't work properly for memory card.

    if(!strncmp(EfsFullPathStartTag,(char *)(pmdList[i].pData),strlen(EfsFullPathStartTag)) ||
       !strncmp(MmcFullPathStartTag,(char *)(pmdList[i].pData),strlen(MmcFullPathStartTag)))
    {
      continue;
    }


   // Get resolved fs:/ path length
    if (SUCCESS != AEE_ResolvePath((char *)(pmdList[i].pData), NULL, &nLen))
      goto Done;

    pszFS = MALLOC(nLen);
    if (!pszFS)
      goto Done;

    // Resolve path to fs:/
    if (SUCCESS != AEE_ResolvePath((char *)(pmdList[i].pData), pszFS, &nLen))
      goto Done;

    // Check for access
    if (SUCCESS != AEE_CheckPathAccess((char *)(pmdList[i].pData), AEEFP_READ,0)) 
      goto Done;

    /* measure native path */
    if (SUCCESS != OEMFS_GetNativePath(pszFS,0, &nLen))
      goto Done;

    FREEIF((char *)(pmdList[i].pData));
    pmdList[i].pData = MALLOC(nLen);
    if (!pmdList[i].pData) 
      goto Done;

    pmdList[i].dwSize = nLen;

    /* get native path */   
    if (SUCCESS != OEMFS_GetNativePath(pszFS,pmdList[i].pData,&nLen))
      goto Done;
  }

  nRet = SUCCESS;

Done:
  FREEIF(pszFS);
  return nRet;
}
#endif  // MIN_BREW_VERSION(3,0)


//#endif  // FEATURE_APP_MPEG4

//	Recursive method get extname from filename with multiple '.' tags.
char* get_pExtName(char* fullname, char tag)
{
	char* result;
	result = strchr((const char *)fullname, tag);
	if(result == NULL)
	{
		return fullname-1;
	}
	else
	{
		result++;
		return get_pExtName(result, tag);
	}
}
extern vc_play_mode     g_play_mode;
void CMediaMMLayer_CallbackTimer(AEEMediaCallback *   pcb)
{
    IShell * pIShell = AEE_GetShell();//pme->m_pIShell;
    static vc_union_type vc_data;
    AEEMedia * DpUser = (AEEMedia *)pMediaCB;
    
    VC_DeviceControl(VC_ITM_GET_PLAY_TIME_I, VC_FUNC_GET_PARM, &vc_data);
    pcb->cmdNotify.pCmdData = (void*)vc_data.parm.p1;
    pcb->cmdNotify.nCmd = MM_CMD_PLAY;
    pcb->cmdNotify.nStatus = MM_STATUS_TICK_UPDATE;
	
    if(VC_PLAY_M == g_play_mode)
    	{
           if(VC_GetCurrentDevice() == VC_DEV_PLY_MP3 || VC_GetCurrentDevice() == VC_DEV_PLY_AMRAUDIO
		 || VC_GetCurrentDevice() == VC_DEV_PLY_AAC)
           {
              CMediaBg_CallbackNotify(pMediaCB);
           }
           else if(VC_GetCurrentDevice() == VC_DEV_PLY_AMR)
           {
              OEMMediaMPEG4_CallbackNotify(pMediaCB);
           }
           ISHELL_SetTimer(pIShell, 1000,(PFNNOTIFY)(CMediaMMLayer_CallbackTimer),pcb);
    	}
    else if(VC_NONE_M == g_play_mode)
    	{  
    	    DpUser->m_nState = MM_STATE_READY;
    	    if(VC_GetCurrentDevice() == VC_DEV_PLY_MP3 || VC_GetCurrentDevice() == VC_DEV_PLY_AMRAUDIO
		 || VC_GetCurrentDevice() == VC_DEV_PLY_AAC)
           {
              CMediaBg_CallbackNotify(pMediaCB);
           }
           else if(VC_GetCurrentDevice() == VC_DEV_PLY_AMR)
           {
              OEMMediaMPEG4_CallbackNotify(pMediaCB);
           }
    	}
}

/*===========================================================================

FILE: OEMMediaCMX.c

SERVICES: IMEDIA interfaces

DESCRIPTION
   This file implements IMedia-based classes on CMX multimedia platform.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/oem/BREW3.1.5/main/latest/src/OEMMediaCMX.c#18 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/10   rp/rg  NULL pointer check in OEMMedia_Stop function.
08/11/09   bk      Changes for returning failure in case unsupported OATMedia 
                   test of Linear PCM is run.
06/25/09   kk      Added the missed code.
06/25/09   sk      Modified the code to initialise audfmt_spec as static and
                   added the missed break in Brew Detect AAC and MP3 Callbacks.
04/29/09   sk      Modifed the Debug messages from FARF to MSG.
03/18/09   Laks    Moving a stmt causing crash into the NULL check -CR166384
01/19/09    BK     Found and fixed the compilation bug.
01/15/09   antti   Modified the code to fix the dropped data request 
                   acknowlegements while flushing the CMX buffers,CR(167593).
09/24/08    ry     Removed the use of STRLEN in OEMMedia_DetectTypeMP3Ex func as
                   it's functionaly will fail if buffer contains 0x00.
09/19/08  gs/ss    Moved Critical Section to end in PlayCB and gave Critical
                   Sections in CMediaCMX_Delete() and added some debug 
                   messages. Fix for CR_151261.
09/17/08  ss/gs    Added pcb allocation fixes that was originated from 
                   CMX_STARTED. Fixed CR_146672.
22/07/08   vsud    Added AAC class id for in call AAC feature.
04/24/08    ss     Added support for Set_Pan which fails in case of midi,mmf
                   and xmf files (Origin from CR_134818).
03/31/08  ant/ss   Added ixstream_length in CMediaMMLayer_SetSource() for
                   get_total_time fix. Fixed CR_141269.
03/15/08    sk     Fixed CR 124531 - uiOne: multimedia ringer continues 
                   playback after MT call is answered   
03/10/08    sk     Modified code the remove the MM_STATUS_START in
                   CMediaMMLayer_Play() CR 120853.  
03/07/08    ss     Added ixstream_delete_stream in CMediaCMX_Delete and
                   CMediaCMX_SetMediaParm for a fix related to OMA DRM.
02/12/08    pl     Fixed CR# 120108 - create QAudioFX command cache when
                   3D or QAFX interface query is successful. Add new function,
                   CMediaMIDIOutQCP_RestoreParm, to restore AMSS layer to previous
                   3D & QAFX state if 3D or QAFX commands time out waiting for 
                   response from CMX.
11/05/07    sk     Updated the new CMX status message CMX_STARTED to set the
                   MM_STATUS_START in CMediaMMLayer_GetMMStatus2().
                   Remove the Status MM_STATUS_START in CMediaMMLayer_Play() 
                   CR 120253,124336,124337,120853. 
01/08/06    cah    Integrated with 3.1.5.17 version.
12/22/05    jg     Added calls to ISOUND_Get() to update device before calling
                   other ISound functions.  CR# 45762, 71437, 80162, 84704.

12/21/05    jg     Fixes/additions to SAF offset calculations.

12/02/05    jg     Reset m_bChannelSet variable for MIDIOUTQCP when playback
                   is done or aborted
                   
12/01/05    jg     Created a separate rect for JPEG data to prevent corruption
                   between setting and reading by other incoming data 
                   (CR #78717)
                   
11/22/05    jg     Added support for MM_STATUS_FRAME for image and text updates

11/14/05    jg     Changes to allow PCM to be channel shared (played via 
                   cmx_midi_out_qcp).

11/04/05    jg     QIS changes to PNGCopyBuffer to correct PNG data handling

11/01/06    jg     For FEATURE_GASYNTH_INTERFACE bypassed checks to ensure ADPCM/QCP
                   are not mixed.
                   
10/12/05    jg     Added code necessary for alpha version of QAudioFX.

09/01/05    jg     cmx_midi_out_qcp pan value was being converted twice: in
                   CMediaMMLayer_MIDIOutQCPPlay and in OEMMedia_Start. Removed
                   conversion from OEMMedia_Start.

08/22/05    pk     Added unicode conversions for Korean, Japanese, Trad., Chinese, and 
                   Hindi(Devanagari) character set encoding for pmd text

07/26/05    jg     Modified IMediaBg_Init to treat MLD as audio only, even
                   though its mime type says "video".  CR70788.

07/08/05    jg     Augmented the change from 6/1/05 to include the callbacks
                   for client buffer allocation.

06/27/05	   pk     Added AAC Recording Feature

06/17/05    anb    Added conversion interfaces for Chinese(GB2312)toUnicode in
				   ProcessPMDText.

06/10/05    jg     Added code to restrict number of buffer copies created 
                   based on size of each buffer.

06/01/05    jg     Created an array of buffer copies and callback structures
                   to prevent data loss from flooding of CMX callbacks  
                   faster than UI callbacks can handle them and consume the
                   data. This is to fix graphics corruption reported in 
                   CR #50792.

04/15/05    jg     Added Srinivas' change for concurrent ADPCM and removed my
                   change from 3/16/05.

03/28/05    jg     Enabled tick time parm for PCM.

03/16/05    jg     Fixed problem playing multiple ADPCM. CR#57599.

03/11/05   sjn     Fixed compiler warnings on 6550 & 6250 based on BREW 3.1.3.10

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

#include "ui.h"
#include "task.h"
#include "msg.h"

#include "OEMMediaCMX.h"

#if defined (FEATURE_QAUDIOFX)
#include "AEEMediaAudio.h"
#include "OEMMediaAudio.h"
#include "MEDIAAUDIO3D.BID"
#include "MEDIAAUDIOFX.BID"
#endif // defined (FEATURE_QAUDIOFX)

#include "AEESource.h"

#include "AEECLSID_MEDIAAMRWB.bid"
#include "AEEMedia.bid"

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
#include "AEECLSID_SYNTHQUALITY.BID"
#include "OEMSynthQuality.h"
#endif

#if defined (FEATURE_ACM)
#include "OEMACM.h"
#endif // FEATURE_ACM

#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
#include "uixsnd.h"
#endif // (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))

#if defined(FEATURE_QTV_DRM_DCF)
#undef __cplusplus
#include "IxStreamUtils.h"
#endif // defined(FEATURE_QTV_DRM_DCF)

typedef struct CMediaBg          CMediaBg;
typedef struct CMediaMIDIOutMsg  CMediaMIDIOutMsg;
typedef struct CMediaMIDIOutQCP  CMediaMIDIOutQCP;

/*===========================================================================

                      DEFINES

===========================================================================*/
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

#ifdef FEATURE_BREW_EVW
#define OEMMM_IsVocType(cls)           (AEECLSID_MEDIAQCP == (cls) || AEECLSID_MediaEVRCNB == (cls) || AEECLSID_MediaEVRCWB== (cls))
#else
#define OEMMM_IsVocType(cls)           (AEECLSID_MEDIAQCP == (cls) || AEECLSID_MediaEVRCNB == (cls))
#endif


#if defined(FEATURE_AUDIO_FORMAT)

/*===========================================================================

                      CMX: MIDI, MP3, PMD, QCP

===========================================================================*/

#ifdef FEATURE_MULTISEQUENCER
/* List of all class ids that are supported in the MultiSequence API */

static AEECLSID multiSequencerClasses[] = { AEECLSID_MEDIAMIDI,
                                            AEECLSID_MEDIAMMF,
                                            AEECLSID_MEDIAPMD,
                                            AEECLSID_MEDIAPHR,
                                            AEECLSID_MEDIAXMF };

static boolean CMediaMMLayer_IsMultiSequenceType ( CMediaBg * pme );
static boolean CMediaMMLayer_IsMultiSequence ( CMediaBg * pme );
#endif /* FEATURE_MULTISEQUENCER */
#endif // defined(FEATURE_AUDIO_FORMAT)

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
   flg      bReqPending;
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

#if defined(FEATURE_CMX_RAW_PLAYBACK)
   cmx_codec_spec_type *   m_pCodecSpec;
   uint32                  m_dwAudioSync;
#endif

   // MIDIOutQCP
   uint16         m_wVolume;
   uint16         m_wPan;
   uint32         m_dwChannel;
#if defined(FEATURE_MIDI_OUT_QCP)
   uint32         m_dwRepeatCount;
#endif // defined(FEATURE_MIDI_OUT_QCP)
   
#ifdef FEATURE_MULTISEQUENCER
   // Multisequencer
   cmx_audfmt_multi_param_type * m_pMultiParam;
   void *                        m_pSequence;
   cmx_audfmt_seq_param_type *   m_pSeqParam;
   
#endif  //FEATURE_MULTISEQUENCER

#if defined(FEATURE_QCP)
   // QCPRecord
   cmx_qcp_rec_para_type * m_pQCPRecParm;
#endif // defined(FEATURE_QCP)

#ifdef FEATURE_PCM_REC
   // PCM Record
   cmx_pcm_rec_para_type * m_pPCMRecParm;
   cmx_pcm_rec_link_type   m_Link;
#endif //FEATURE_PCM_REC

#ifdef FEATURE_AAC_REC
   cmx_mm_rec_param_type * m_pAACRecParm;
#endif // FEATURE_AAC_REC

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

#if defined(FEATURE_MIDI_OUT_QCP)
#define OEMCMX_ID_REPEAT             13 // p1 repeat count
#endif // defined(FEATURE_MIDI_OUT_QCP)

#define OEMCMX_TYPE_AUDFMT_PLAY     1  // p2 = CMX_AUDFMT_START_XXX
#define OEMCMX_TYPE_AUDFMT_RINGER   2  // p2 = CMX_AUDFMT_START_XXX, p3 = Pause time
#define OEMCMX_TYPE_QCP_RECORD      3  // p2 = cmx_qcp_rec_para_type *, p3 = TRUE/FALSE => ReverseLink(Local)/ForwardLink
#define OEMCMX_TYPE_MIDI_OUT_MSG    4  //
#define OEMCMX_TYPE_MIDI_OUT_QCP    5  // 
#define OEMCMX_TYPE_AUDFMT_PLAY_CODEC  6  // p2 = cmx_codec_spec_type *
#define OEMCMX_TYPE_AUDFMT_PLAY_MULTI  7  // p2 = cmx_audfmt_multi_param_type *
#define OEMCMX_TYPE_PCM_RECORD         8  // p2 = cmx_pcm_rec_para_type *
#define OEMCMX_TYPE_AAC_RECORD		   9 // p2 = cmx_mm_rec_param_type *

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
static void CMediaMIDIOutQCP_QCPStartCB(void * pUser);
#endif // defined (FEATURE_QAUDIOFX)
#endif // defined(FEATURE_MIDI_OUT_QCP)
#endif // defined(FEATURE_MIDI_OUT)
void CMediaMMLayer_CMXDataRequestCB(uint32 position, uint32 max_len, cmx_client_req_cb_func_ptr_type data_req_cb, 
                                           const void *server_client_data, const void *client_data);
#if defined(FEATURE_CMX_AV_SYNC)
static void CMediaMMLayer_CMXAudioSyncCB(qword timestamp, qword num_of_samples, qword num_of_bytes, const void  *client_data);
#endif // defined(FEATURE_CMX_AV_SYNC)

#if defined(FEATURE_BREW_DETECT_MP3)
int     OEMMedia_DetectTypeMP3Ex(const void * cpBuf, uint32 * pdwSize);
static void    CMediaCMX_DetectMP3CB(cmx_status_type status, const void *client_data, cmx_af_audio_spec_type *audio_spec);
#endif // defined(FEATURE_BREW_DETECT_MP3)
#if defined(FEATURE_BREW_DETECT_AAC)
int     OEMMedia_DetectTypeAACEx(const void * cpBuf, uint32 * pdwSize);
static void    CMediaCMX_DetectAACCB(cmx_status_type status, const void *client_data, cmx_af_audio_spec_type *audio_spec);
#endif // defined(FEATURE_BREW_DETECT_AAC)


/*===============================================================================

      CMediaCMX DECLARATIONS
      These are used by all IMedia classes implemented in this file.

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

#if defined(FEATURE_QTV_DRM_DCF) && defined(FEATURE_ACM)
#define INHERIT_CMediaCMX(iname) \
   INHERIT_AEEMedia(iname); \
   AEEObjectHandle           m_hObject;  \
   PFNNOTIFY                 m_pfnDelete; \
   AEEMediaCallback          m_cbPlay[OEMMM_MAX_CB_PLAY]; \
   AEEMediaCallback          m_cbCommand; \
   OEMMedia*                 m_pMediaCMX; \
   CQueue *                  m_pqCMXDataReq; \
   AEECallback *             m_pcbCMXDataReq; \
   CQueue *                  m_pqMediaData; \
   AEECallback *             m_pcbMediaData; \
   uint8 *                   m_pSrcBuffer; \
   uint32                    m_dwSrcBufferSize; \
   cmx_client_data_enum_type m_eStreamingType; \
   CMXDataRequest            m_sReq; \
   IxStreamReadRspT          m_pReadRsp; \
   \
   uint32                    m_dwCaps; \
   uint32                    m_dwEnabledCaps; \
   int                       m_nAPath; \
   int                       m_nPlayType; \
   uint16                    m_wVolume; \
   uint16                    m_wPan; \
   uint16                    m_wTempo; \
   uint16                    m_wTune; \
   uint16                    m_wSilenceTimeMS; \
   uint32                    m_dwTickTimeMS; \
   uint32                    m_dwRepeatCount; \
   boolean                   m_bMute; \
   flg                       m_bStreaming:1; \
   flg                       m_bReadPending : 1; \
   flg                       m_bCancelRead : 1; \
   flg                       m_bEOS:1; \
   flg                       m_bStarting:1; \
   flg                       m_bStopping:1; \
   flg                       m_bMIDIOut:1; \
   flg                       m_bVolumeScaled:1
#elif defined(FEATURE_QTV_DRM_DCF)
#define INHERIT_CMediaCMX(iname) \
   INHERIT_AEEMedia(iname); \
   AEEObjectHandle           m_hObject;  \
   PFNNOTIFY                 m_pfnDelete; \
   AEEMediaCallback          m_cbPlay[OEMMM_MAX_CB_PLAY]; \
   AEEMediaCallback          m_cbCommand; \
   OEMMedia*                 m_pMediaCMX; \
   CQueue *                  m_pqCMXDataReq; \
   AEECallback *             m_pcbCMXDataReq; \
   CQueue *                  m_pqMediaData; \
   AEECallback *             m_pcbMediaData; \
   uint8 *                   m_pSrcBuffer; \
   uint32                    m_dwSrcBufferSize; \
   cmx_client_data_enum_type m_eStreamingType; \
   CMXDataRequest            m_sReq; \
   IxStreamReadRspT          m_pReadRsp; \
   \
   uint32                    m_dwCaps; \
   uint32                    m_dwEnabledCaps; \
   int                       m_nAPath; \
   boolean                   m_bMute; \
   uint16                    m_wVolume; \
   uint16                    m_wPan; \
   uint16                    m_wTempo; \
   uint16                    m_wTune; \
   uint16                    m_wSilenceTimeMS; \
   uint32                    m_dwTickTimeMS; \
   uint32                    m_dwRepeatCount; \
   flg                       m_bStreaming:1; \
   flg                       m_bReadPending : 1; \
   flg                       m_bCancelRead : 1; \
   flg                       m_bEOS:1; \
   flg                       m_bStarting:1; \
   flg                       m_bStopping:1; \
   flg                       m_bMIDIOut:1; \
   flg                       m_bVolumeScaled:1
#elif defined(FEATURE_ACM)
#define INHERIT_CMediaCMX(iname) \
   INHERIT_AEEMedia(iname); \
   AEEObjectHandle           m_hObject;  \
   PFNNOTIFY                 m_pfnDelete; \
   AEEMediaCallback          m_cbPlay[OEMMM_MAX_CB_PLAY]; \
   AEEMediaCallback          m_cbCommand; \
   OEMMedia*                 m_pMediaCMX; \
   CQueue *                  m_pqCMXDataReq; \
   AEECallback *             m_pcbCMXDataReq; \
   CQueue *                  m_pqMediaData; \
   AEECallback *             m_pcbMediaData; \
   uint8 *                   m_pSrcBuffer; \
   uint32                    m_dwSrcBufferSize; \
   cmx_client_data_enum_type m_eStreamingType; \
   CMXDataRequest            m_sReq; \
   \
   uint32                    m_dwCaps; \
   uint32                    m_dwEnabledCaps; \
   int                       m_nAPath; \
   int                       m_nPlayType; \
   uint16                    m_wVolume; \
   uint16                    m_wPan; \
   uint16                    m_wTempo; \
   uint16                    m_wTune; \
   uint16                    m_wSilenceTimeMS; \
   uint32                    m_dwTickTimeMS; \
   uint32                    m_dwRepeatCount; \
   boolean                   m_bMute; \
   flg                       m_bStreaming:1; \
   flg                       m_bEOS:1; \
   flg                       m_bStarting:1; \
   flg                       m_bStopping:1; \
   flg                       m_bMIDIOut:1; \
   flg                       m_bVolumeScaled:1
#else // ! FEATURE-ACM
#define INHERIT_CMediaCMX(iname) \
   INHERIT_AEEMedia(iname); \
   AEEObjectHandle           m_hObject;  \
   PFNNOTIFY                 m_pfnDelete; \
   AEEMediaCallback          m_cbPlay[OEMMM_MAX_CB_PLAY]; \
   AEEMediaCallback          m_cbCommand; \
   OEMMedia*                 m_pMediaCMX; \
   CQueue *                  m_pqCMXDataReq; \
   AEECallback *             m_pcbCMXDataReq; \
   CQueue *                  m_pqMediaData; \
   AEECallback *             m_pcbMediaData; \
   uint8 *                   m_pSrcBuffer; \
   uint32                    m_dwSrcBufferSize; \
   cmx_client_data_enum_type m_eStreamingType; \
   CMXDataRequest            m_sReq; \
   \
   uint32                    m_dwCaps; \
   uint32                    m_dwEnabledCaps; \
   int                       m_nAPath; \
   boolean                   m_bMute; \
   uint16                    m_wVolume; \
   uint16                    m_wPan; \
   uint16                    m_wTempo; \
   uint16                    m_wTune; \
   uint16                    m_wSilenceTimeMS; \
   uint32                    m_dwTickTimeMS; \
   uint32                    m_dwRepeatCount; \
   flg                       m_bStreaming:1; \
   flg                       m_bEOS:1; \
   flg                       m_bStarting:1; \
   flg                       m_bStopping:1; \
   flg                       m_bMIDIOut:1; \
   flg                       m_bVolumeScaled:1
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

typedef struct CMIDIOut
{
   int               m_nRef;
   byte              m_nADECType;   // For CMX 2.x, zero. For CMX 3.x, CMX_ADEC_MODE_XXX
   boolean           m_bOpenPending;
   PFNMIDIOUTNOTIFY  m_pfnNotify;
   void *            m_pMM;         // object that opened MIDI_OUT
   uint32            m_pHandle;     // handle for the object
   boolean           m_bSync;       // perform synchronous open
   AEECallback       m_cb;
} CMIDIOut;

static int CMIDIOut_AddRef(void);
static void CMIDIOut_OpenNotify(void * pUser);

static CMIDIOut   gsMIDIOut = { 0 };

#define CMIDIOut_IsOpen()        (gsMIDIOut.m_nRef > 0)
#define CMIDIOut_IsDefault()     (gsMIDIOut.m_nADECType == CMX_ADEC_MODE_DEFAULT)
#define CMIDIOut_IsQCP()         (gsMIDIOut.m_nADECType == CMX_ADEC_MODE_QCP || CMIDIOut_IsDefault())
#define CMIDIOut_IsADPCM()       (gsMIDIOut.m_nADECType == CMX_ADEC_MODE_ADPCM || CMIDIOut_IsDefault())
#define CMIDIOut_IsPCM()         (gsMIDIOut.m_nADECType == CMX_ADEC_MODE_PCM || CMIDIOut_IsDefault())

#endif // defined(FEATURE_MIDI_OUT)

/*===========================================================================
      MMLayer functions
===========================================================================*/

static void CMediaMMLayer_Init(IShell * ps);
static void CMediaMMLayer_AEEExitNotify(void * pUser);
static void CMediaMMLayer_InitCallback(IMedia * po, int nCmd, AEEMediaCallback * pcb, int nMax, PFNNOTIFY pfn);
static AEEMediaCallback * CMediaMMLayer_AllocCallback(AEEMediaCallback * pcb, int nMax, int nCmd, int nSubCmd);
static void CMediaMMLayer_FreeCallback(AEEMediaCallback * pcb);
static void CMediaMMLayer_CancelCallback(AEEMediaCallback * pcb, int nMax);

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
static int CMediaMMLayer_PlayCodec(CMediaBg * pme, cmx_handle_type * ph);
static int CMediaMMLayer_Stop(CMediaBg * pme);
static int CMediaMMLayer_Seek(CMediaBg * pme, AEEMediaSeek eSeek, int32 lTimeMS);
static int CMediaMMLayer_Pause(CMediaBg * pme);
static int CMediaMMLayer_Resume(CMediaBg * pme);
static int CMediaMMLayer_GetTotalTime(CMediaBg * pme);

static int CMediaMMLayer_SetTempo(CMediaBg * pme, uint32 dwTempo);
static int CMediaMMLayer_SetTune(CMediaBg * pme, uint32 dwTune);
static int CMediaMMLayer_SetPan(CMediaBg * pme, uint32 dwPan);

static int CMediaMMLayer_QCPRecord(CMediaBg * pme, int nFormat);
static int CMediaMMLayer_EVRCBRecord(CMediaBg * pme);
static int CMediaMMLayer_PCMRecord(CMediaBg * pme);
#ifdef FEATURE_BREW_EVW   
static int CMediaMMLayer_EWBRecord(CMediaBg * pme);
#endif

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
static int CMediaMMLayer_MIDIOutQCPStop(CMediaMIDIOutQCP * pme);
static int CMediaMMLayer_MIDIOutQCPPause(CMediaMIDIOutQCP * pme);
static int CMediaMMLayer_MIDIOutQCPResume(CMediaMIDIOutQCP * pme);
static int CMediaMMLayer_MIDIOutQCPSetVolume(CMediaMIDIOutQCP * pme, uint16 wVolume);
static int CMediaMMLayer_MIDIOutQCPSetPan(CMediaMIDIOutQCP * pme, uint16 wPan);
#ifdef FEATURE_JAVA
#error code not present
#endif

#if defined (FEATURE_ACM)
static int CMediaBg_AcmPlayCB(ACM_Callback_data_type * po);
static int CMediaBg_AcmRecordCB(ACM_Callback_data_type *po);

static int CMediaMIDIOutQCP_AcmPlayCB(ACM_Callback_data_type *po);
static int CMediaMIDIOutMsg_AcmPlayCB(ACM_Callback_data_type *po);

void CMediaBg_GetResourceCntlData(AEEMedia * po, int * nPlayType, flg * channelShare);
void CMediaBg_SetResourceCntlCBData(uint32 * acmCB, flg isPlay);
void CMediaMIDIOutQCP_GetResourceCntlData(AEEMedia *po, int * nPlayType, flg * channelShare, uint32 * acmCB);
void CMediaMIDIOutQCP_CleanupResource (AEEMedia *pme);
void CMediaMIDIOutMsg_GetResourceCntlData(AEEMedia *po, int * nPlayType, uint32 * acmCB);

extern void AEEMedia_CleanUpResource(AEEMedia * po);
#endif // FEATURE_ACM

/*===============================================================================

      CMediaBg: Background Audio (MIDI, MP3, QCP. Includes PMD) related.

===============================================================================*/

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

   AEEMediaCallback  m_cbGetTotalTime;

   ISound *          m_pISound;

#if defined(FEATURE_CMX_AV_SYNC)
   uint32            m_dwAudioSyncType;   // Time based or sample based
   uint32            m_dwAudioSyncData;   // Time in ms or samples
   AEEMediaAudioSync *m_pAudioSyncInfo;   // CB info
   uint64            m_qwAudioSyncBytes;
#endif //defined(FEATURE_CMX_AV_SYNC)

   void   *          m_pSequence;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
   uint32            m_dwNotes;
   boolean           m_bISynthQualityRef;
#endif

   int32 m_iStartTime;   // Time playback or recording will
                         //   start if MM_PARM_POS is set
   int32 m_iStopTime;    // Time playback or recording will 
                         //   stop if MM_PARM_POS is set
   int32 m_iCurrentTime; // Current time in playback or recording                         
};

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
#define OEMMM_MAX_NOTES    72 // Accommodate as many notes as possible (HP_ALL mode)
static uint32              gdwNotes = OEMMM_MAX_NOTES;
static boolean             gbNotesSet = FALSE;
static ISynthQuality      *gpISynthQuality = NULL;
#endif

void IMediaBg_Init(IShell * ps);
int  IMediaBg_New(IShell * ps, AEECLSID cls, void **ppif);

static int     CMediaBg_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int     CMediaBg_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);
static int     CMediaBg_Play(IMedia * po);
static int     CMediaBg_Record(IMedia * po);
static int     CMediaBg_Stop(IMedia * po);
static int     CMediaBg_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS);
static int     CMediaBg_Pause(IMedia * po);
static int     CMediaBg_Resume(IMedia * po);
static int     CMediaBg_GetTotalTime(IMedia * po);

static void    CMediaBg_CallbackNotify(void * pUser);
static void    CMediaBg_Delete(CMediaBg * pme);

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
};

void IMediaMIDIOutMsg_Init(IShell * ps);
int  IMediaMIDIOutMsg_New(IShell * ps, AEECLSID cls, void **ppif);

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

   uint32                      m_dwChannel;
   flg                         m_bChannelSet:1;  //true if channel has been set

#if defined (FEATURE_QAUDIOFX)
   AEEMediaCallback            m_cb3DStart;
   paramCache                  m_savedCmd;
   paramCache                * m_pCache;
   paramCache                * m_pLast;
   uint32                      m_dwEnable;
   flg                         m_bPlaying:1;   
   flg                         m_bInEnv:1;
   flg                         m_bSavedCmdValid:1;
   flg                         m_bCacheIntialized:1;
#endif // defined (FEATURE_QAUDIOFX)
};

void IMediaMIDIOutQCP_Init(IShell * ps);
int  IMediaMIDIOutQCP_New(IShell * ps, AEECLSID cls, void **ppif);

static int CMediaMIDIOutQCP_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int CMediaMIDIOutQCP_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);
static int CMediaMIDIOutQCP_Play(IMedia * po);
static int CMediaMIDIOutQCP_Stop(IMedia * po);
static int CMediaMIDIOutQCP_Pause(IMedia * po);
static int CMediaMIDIOutQCP_Resume(IMedia * po);

static void CMediaMIDIOutQCP_Delete(CMediaMIDIOutQCP * pme);

static int CMediaMain_QueryInterface(IMedia * po, AEECLSID idReq, void ** ppo);

static const VTBL(IMediaMIDIOutQCP) gMediaMIDIOutQCPFuncs =
{
   AEEMedia_AddRef,
   CMediaCMX_Release,
   CMediaMain_QueryInterface,
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

#if defined (FEATURE_QAUDIOFX)
/*===============================================================================
      3D Audio parameter caching functions
===============================================================================*/

// add a 3D parameter to the cache. 
// This will be called from CMediaMIDIOutQCP_SetParamCache() after the media has finished playing
static int CMediaMIDIOutQCP_AddCachedParamInt(CMediaMIDIOutQCP * pme, int nParmID, void * p1)
{
   cmx_3d_pos_cmd_enum_type  cmd = (cmx_3d_pos_cmd_enum_type)nParmID;
   cmx_3d_pos_param_type    *cmxParam = (cmx_3d_pos_param_type *)p1;
   paramCache               *pNew, *pCache;

   switch(cmd)
   {
      case CMX_3D_POS_CMD_SET_SRC_POS:
      case CMX_3D_POS_CMD_SET_SRC_VOL:
      case CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN:
      case CMX_3D_POS_CMD_SET_SRC_VEL:
      case CMX_3D_POS_CMD_SRC_ENABLE:
      case CMX_3D_POS_CMD_SET_SRC_REVERB_VOL:
         break;

      default:
         return EFAILED;
   }

   pCache = pme->m_pCache;

   while (pCache)
   {
      if (pCache->cmd == cmd)
      {
         // replace existing record
         MEMCPY((uint8 *)&pCache->cmxParam, (uint8 *)cmxParam, sizeof(cmx_3d_pos_param_type));
         return SUCCESS;
      }
      pCache = pCache->pNext;
   }

   // create a new parameter setting record
   pNew = (paramCache *)MALLOC(sizeof(paramCache));

   if (!pNew)
   {
      return ENOMEMORY;
   }

   MEMCPY((uint8 *)&pNew->cmxParam, (uint8 *)cmxParam, sizeof(cmx_3d_pos_param_type));
   pNew->cmd = cmd;

   OEMMM_ENTER_CRITICAL_SECTION()

   if (pme->m_pLast)
   {
      pme->m_pLast->pNext = pNew;
   }
   else
   {
      pme->m_pCache = pNew;
   }
   pme->m_pLast = pNew;

   OEMMM_LEAVE_CRITICAL_SECTION()

   return SUCCESS;
}

static void CMediaMIDIOutQCP_SetDefault3DParam(IMedia * po) 
{
  CMediaMIDIOutQCP    *pme = (CMediaMIDIOutQCP *)po;
  cmx_3d_pos_param_type cmx_param;

  if (pme->m_bCacheIntialized == 1) {
    MSG_HIGH("Cache already initialized", 0, 0, 0);
    return;
  }
  cmx_param.src_pos_cmd.duration_ms = 0;
  cmx_param.src_pos_cmd.x_pos = 0;
  cmx_param.src_pos_cmd.y_pos = 0;
  cmx_param.src_pos_cmd.z_pos = 0;
  (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, 
                                           CMX_3D_POS_CMD_SET_SRC_POS, 
                                           (void*) &cmx_param);

  cmx_param.src_volume_cmd.duration_ms = 0;
  cmx_param.src_volume_cmd.volume = 0;
  (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, 
                                           CMX_3D_POS_CMD_SET_SRC_VOL, 
                                           (void*) &cmx_param);

  cmx_param.src_vol_atten_cmd.max_distance = 0x7FFFFFFF;
  cmx_param.src_vol_atten_cmd.min_distance = 1000;
  cmx_param.src_vol_atten_cmd.scale = 1000;
  cmx_param.src_vol_atten_cmd.mute_after_max = FALSE;
  (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, 
                                           CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN, 
                                           (void*) &cmx_param);

  cmx_param.src_enable_cmd.enable_mode = CMX_3D_POS_MODE_DISABLE;
  (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, 
                                           CMX_3D_POS_CMD_SRC_ENABLE, 
                                           (void*) &cmx_param);
  cmx_param.src_vel_cmd.duration_ms = 0;
  cmx_param.src_vel_cmd.x_vel = 0;
  cmx_param.src_vel_cmd.y_vel = 0;
  cmx_param.src_vel_cmd.z_vel = 0;
  (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, 
                                           CMX_3D_POS_CMD_SET_SRC_VEL, 
                                           (void*) &cmx_param);

  cmx_param.src_reverb_vol_cmd.volume = -9600;
  (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, 
                                           CMX_3D_POS_CMD_SET_SRC_REVERB_VOL, 
                                           (void*) &cmx_param);

  pme->m_bCacheIntialized = 1;
} 

// save parameter for later use. In rare cases CMX reports that a set param was success
// but DSP never processed the request. If the duration of the request == 0
// we'll override final parameters with the last saved request in order for
// GetParam and replay to have expected behavior
void CMediaMIDIOutQCP_SaveParam(IMedia * po, int nParmID, void * p1)
{
   CMediaMIDIOutQCP         *pme = (CMediaMIDIOutQCP *)po;
   cmx_3d_pos_cmd_enum_type  cmd = (cmx_3d_pos_cmd_enum_type)nParmID;
   cmx_3d_pos_param_type    *cmxParam = (cmx_3d_pos_param_type *)p1;
   int                       Status = SUCCESS;

   switch(cmd)
   {
   case CMX_3D_POS_CMD_SET_SRC_POS:
      if (0 != cmxParam->src_pos_cmd.duration_ms)
      {
         Status = EFAILED;
      }
      break;

   case CMX_3D_POS_CMD_SET_SRC_VOL:
      if (0 != cmxParam->src_volume_cmd.duration_ms)
      {
         Status = EFAILED;
      }
      break;

   case CMX_3D_POS_CMD_SET_SRC_VEL:
      if (0 != cmxParam->src_vel_cmd.duration_ms)
      {
         Status = EFAILED;
      }
      break;

   case CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN:
   case CMX_3D_POS_CMD_SRC_ENABLE:
   case CMX_3D_POS_CMD_SET_SRC_REVERB_VOL:
      break;

   default:
      Status = EUNSUPPORTED;
      break;
   }
   if (SUCCESS == Status)
   {
      (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, nParmID, p1);

      MEMCPY((byte *)&pme->m_savedCmd.cmxParam, (byte *)cmxParam, sizeof(cmx_3d_pos_param_type));
      pme->m_savedCmd.cmd = cmd;
      pme->m_bSavedCmdValid = TRUE;
   }
   else if (EFAILED == Status)
   {
      pme->m_bSavedCmdValid = FALSE;
   }
}

// add a 3D parameter to the cache. 
// This will be called from CMediaMIDIOutQCP_SetMediaParm() before the media plays
int CMediaMIDIOutQCP_AddCachedParam(IMedia * po, int nParmID, void * p1)
{
   CMediaMIDIOutQCP         *pme = (CMediaMIDIOutQCP *)po;
   cmx_3d_pos_cmd_enum_type  cmd = (cmx_3d_pos_cmd_enum_type)nParmID;
   int                       nRet;

   switch(cmd)
   {
   case CMX_3D_POS_CMD_SET_SRC_POS:
   case CMX_3D_POS_CMD_SET_SRC_VOL:
   case CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN:
   case CMX_3D_POS_CMD_SRC_ENABLE:
   case CMX_3D_POS_CMD_SET_SRC_VEL:
      if (pme->m_bPlaying && pme->m_bInEnv && (pme->m_dwEnable & MM_AENV_ENABLE_3D))
      {
         nRet = EBADSTATE; // send to cmx
      }
      else
      {
         nRet = CMediaMIDIOutQCP_AddCachedParamInt(pme, nParmID, p1);
      }
      break;

   case CMX_3D_POS_CMD_SET_SRC_REVERB_VOL:
      if (pme->m_bPlaying && pme->m_bInEnv && (pme->m_dwEnable & MM_AENV_ENABLE_REVERB))
      {
         nRet = EBADSTATE; // send to cmx
      }
      else
      {
         nRet = CMediaMIDIOutQCP_AddCachedParamInt(pme, nParmID, p1);
      }
      break;

   default:
      nRet = pme->m_bPlaying  ? EBADSTATE : EFAILED;
      break;
   }

   return nRet;
}

// check if a 3D parameter is in the cache.
// This will be called from CMediaMIDIOutQCP_GetMediaParm()
int CMediaMIDIOutQCP_CheckCachedParam(IMedia * po, int nParmID, void * p1)
{
   CMediaMIDIOutQCP              *pme = (CMediaMIDIOutQCP *)po;
   cmx_3d_pos_param_enum_type     cmd = (cmx_3d_pos_param_enum_type)nParmID;
   cmx_3d_pos_param_setting_type *setting = (cmx_3d_pos_param_setting_type *)p1;
   paramCache                    *pCache;
   cmx_3d_pos_param_enum_type     param;

   switch(cmd)
   {
      case CMX_3D_POS_PARAM_SRC_POS:
      case CMX_3D_POS_PARAM_SRC_VOLUME:
      case CMX_3D_POS_PARAM_SRC_VOL_ATTEN:
      case CMX_3D_POS_PARAM_SRC_VEL:
      case CMX_3D_POS_PARAM_SRC_ENABLED:
         if (pme->m_bPlaying && pme->m_bInEnv && (pme->m_dwEnable & MM_AENV_ENABLE_3D))
         {
            return EBADSTATE; // get from cmx
         }
         break;

      case CMX_3D_POS_PARAM_SRC_REVERB_VOL:
         if (pme->m_bPlaying && pme->m_bInEnv && (pme->m_dwEnable & MM_AENV_ENABLE_REVERB))
         {
            return EBADSTATE; // get from cmx
         }
         break;

      default:
         return (pme->m_bPlaying ? EBADSTATE : EFAILED);
   }

   pCache = pme->m_pCache;

   while (pCache)
   {
      switch(pCache->cmd)
      {
      case CMX_3D_POS_CMD_SET_SRC_POS:
         param = CMX_3D_POS_PARAM_SRC_POS;
         break;

      case CMX_3D_POS_CMD_SET_SRC_VOL:
         param = CMX_3D_POS_PARAM_SRC_VOLUME;
         break;

      case CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN:
         param = CMX_3D_POS_PARAM_SRC_VOL_ATTEN;
         break;

      case CMX_3D_POS_CMD_SET_SRC_VEL:
         param = CMX_3D_POS_PARAM_SRC_VEL;
         break;

      case CMX_3D_POS_CMD_SRC_ENABLE:
         param = CMX_3D_POS_PARAM_SRC_ENABLED;
         break;

      case CMX_3D_POS_CMD_SET_SRC_REVERB_VOL:
         param = CMX_3D_POS_PARAM_SRC_REVERB_VOL;
         break;

      default:
         param = CMX_3D_POS_PARAM_INVALID;
         break;
      }

      if (param == cmd)
      {
         switch(cmd)
         {
         case CMX_3D_POS_PARAM_SRC_POS:
            setting->position.x_pos = pCache->cmxParam.src_pos_cmd.x_pos;
            setting->position.y_pos = pCache->cmxParam.src_pos_cmd.y_pos;
            setting->position.z_pos = pCache->cmxParam.src_pos_cmd.z_pos;
            break;

         case CMX_3D_POS_PARAM_SRC_VOLUME:
            setting->src_volume.volume = pCache->cmxParam.src_volume_cmd.volume;
            break;

         case CMX_3D_POS_PARAM_SRC_VOL_ATTEN:
            setting->src_vol_atten.min_distance = pCache->cmxParam.src_vol_atten_cmd.min_distance;
            setting->src_vol_atten.max_distance = pCache->cmxParam.src_vol_atten_cmd.max_distance;
            setting->src_vol_atten.scale = pCache->cmxParam.src_vol_atten_cmd.scale;
            break;

         case CMX_3D_POS_PARAM_SRC_VEL:
            setting->src_vel.x_vel = pCache->cmxParam.src_vel_cmd.x_vel;
            setting->src_vel.y_vel = pCache->cmxParam.src_vel_cmd.y_vel;
            setting->src_vel.z_vel = pCache->cmxParam.src_vel_cmd.z_vel;
            break;

         case CMX_3D_POS_PARAM_SRC_ENABLED:
            setting->src_enable = pCache->cmxParam.src_enable_cmd.enable_mode != CMX_3D_POS_MODE_DISABLE;
            break;

        case CMX_3D_POS_PARAM_SRC_REVERB_VOL:
            setting->src_reverb_vol.volume = pCache->cmxParam.src_reverb_vol_cmd.volume;
            break;

         default:
            return EFAILED;
         }
         return SUCCESS;
      }
      pCache = pCache->pNext;
   }

   return EFAILED;
}

// save current cmx 3D settings. 
// This will be called when media play ends or 3D/FX processing gets disabled.
static void CMediaMIDIOutQCP_SetParamCache(CMediaMIDIOutQCP * pme, cmx_3d_pos_param_setting_type * settings)
{
   cmx_3d_pos_param_type cmxParam;

   if (pme->m_bInEnv)
   {
      cmxParam.src_pos_cmd.x_pos = settings->src_all.position.x_pos;
      cmxParam.src_pos_cmd.y_pos = settings->src_all.position.y_pos;
      cmxParam.src_pos_cmd.z_pos = settings->src_all.position.z_pos;
      cmxParam.src_pos_cmd.duration_ms = 0;
      (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, CMX_3D_POS_CMD_SET_SRC_POS, (void *)&cmxParam);

      cmxParam.src_volume_cmd.volume = settings->src_all.volume.volume;
      cmxParam.src_volume_cmd.duration_ms = 0;
      (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, CMX_3D_POS_CMD_SET_SRC_VOL, (void *)&cmxParam);

      cmxParam.src_vol_atten_cmd.min_distance = settings->src_all.vol_atten.min_distance;
      cmxParam.src_vol_atten_cmd.max_distance = settings->src_all.vol_atten.max_distance;
      cmxParam.src_vol_atten_cmd.scale = settings->src_all.vol_atten.scale;
      (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN, (void *)&cmxParam);     

      cmxParam.src_vel_cmd.x_vel = settings->src_all.src_vel_cmd.x_vel;
      cmxParam.src_vel_cmd.y_vel = settings->src_all.src_vel_cmd.y_vel;
      cmxParam.src_vel_cmd.z_vel = settings->src_all.src_vel_cmd.z_vel;
      cmxParam.src_vel_cmd.duration_ms = 0;
      (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, CMX_3D_POS_CMD_SET_SRC_VEL, (void *)&cmxParam);
 
      cmxParam.src_enable_cmd.enable_mode = settings->src_all.src_enable ? CMX_3D_POS_MODE_ENABLE : CMX_3D_POS_MODE_DISABLE;
      (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, CMX_3D_POS_CMD_SRC_ENABLE, (void *)&cmxParam);

      cmxParam.src_reverb_vol_cmd.volume = settings->src_all.reverb_vol.volume;
      (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, CMX_3D_POS_CMD_SET_SRC_REVERB_VOL, (void *)&cmxParam);
      
      if (pme->m_bSavedCmdValid)
      {
         (void)CMediaMIDIOutQCP_AddCachedParamInt(pme, pme->m_savedCmd.cmd, (void *)&pme->m_savedCmd.cmxParam);
         pme->m_bSavedCmdValid = FALSE;
      }
   }
}

// set 3D parameters in cmx
// This will be called when the media play begins
static void CMediaMIDIOutQCP_FlushParamCacheInt(CMediaMIDIOutQCP * pme)
{
   paramCache            *pCache;
   paramCache            *pPrev;

   if (!pme->m_bPlaying && !pme->m_bStarting)
   {
      return;
   }

   if (!pme->m_bInEnv)
   {
      cmx_3d_pos_param_type  cmxParam;

      cmxParam.src_enable_cmd.id = (void *)pme->m_dwChannel;
      cmxParam.src_enable_cmd.enable_mode = CMX_3D_POS_MODE_DISABLE;

      cmx_3d_pos_cmd (CMX_3D_POS_CMD_SRC_ENABLE, (cmx_3d_pos_param_type *)&cmxParam, NULL, NULL);
 
      cmxParam.src_reverb_vol_cmd.id = (void *)pme->m_dwChannel;
      cmxParam.src_reverb_vol_cmd.volume = -9600;

      cmx_3d_pos_cmd (CMX_3D_POS_CMD_SET_SRC_REVERB_VOL, (cmx_3d_pos_param_type *)&cmxParam, NULL, NULL);
   }
   else
   {
      if (pme->m_dwEnable & MM_AENV_ENABLE_3D)
      {
         int duration;

         pPrev = pCache = pme->m_pCache;

         while (pCache)
         {
            duration = 0;

            switch (pCache->cmd)
            {
            case CMX_3D_POS_CMD_SET_SRC_POS:
               pCache->cmxParam.src_pos_cmd.id = (void *)pme->m_dwChannel;
               duration = pCache->cmxParam.src_pos_cmd.duration_ms;
               break;

            case CMX_3D_POS_CMD_SET_SRC_VOL:
               pCache->cmxParam.src_volume_cmd.id = (void *)pme->m_dwChannel;
               duration = pCache->cmxParam.src_volume_cmd.duration_ms;
               break;

            case CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN:
               pCache->cmxParam.src_vol_atten_cmd.id = (void *)pme->m_dwChannel;
               break;

            case CMX_3D_POS_CMD_SET_SRC_VEL:
               pCache->cmxParam.src_vel_cmd.id = (void *)pme->m_dwChannel;
               duration = pCache->cmxParam.src_vel_cmd.duration_ms;
               break;

            case CMX_3D_POS_CMD_SRC_ENABLE:
               pCache->cmxParam.src_enable_cmd.id = (void *)pme->m_dwChannel;
               break;
            case CMX_3D_POS_CMD_SET_SRC_REVERB_VOL:
               pCache->cmxParam.src_reverb_vol_cmd.id = (void *)pme->m_dwChannel;
               break;
            default:
               pCache = pCache->pNext;
               continue;
            }

            cmx_3d_pos_cmd (pCache->cmd, (cmx_3d_pos_param_type *)&pCache->cmxParam, NULL, NULL);
            
            // remove cached parameter with duration value != 0
            if ( 0 == duration )
            {
               pPrev = pCache;
               pCache = pCache->pNext;
            }
            else if ( pPrev == pCache ) // removing first element
            {
               pme->m_pCache = pCache->pNext;

               // removing last element
               if ( pCache == pme->m_pLast )
               {
                  pme->m_pLast = NULL;
               }

               FREE( pCache );
               pPrev = pCache = pme->m_pCache;
            }
            else // removing other element
            {
               pPrev->pNext = pCache->pNext;

               // removing last element
               if ( pCache == pme->m_pLast )
               {
                  pme->m_pLast = pPrev;
               }

               FREE( pCache );
               pCache = pPrev->pNext;
            }
         }
      }

      if (pme->m_dwEnable & MM_AENV_ENABLE_REVERB)
      {
         pCache = pme->m_pCache;

         while (pCache)
         {
            if (CMX_3D_POS_CMD_SET_SRC_REVERB_VOL == pCache->cmd)
            {
               pCache->cmxParam.src_reverb_vol_cmd.id = (void *)pme->m_dwChannel;
               cmx_3d_pos_cmd (CMX_3D_POS_CMD_SET_SRC_REVERB_VOL, (cmx_3d_pos_param_type *)&pCache->cmxParam, NULL, NULL);
               break;
            }
            pCache = pCache->pNext;
         }
      }
   }
}
#endif // defined (FEATURE_QAUDIOFX)

#if defined(FEATURE_CMX_RAW_PLAYBACK)
/*==================================================================
   Get media codec if any
==================================================================*/
static int CMediaCMX_GetCodec(CMediaBg *pme,  cmx_codec_spec_type *pcs)
{  
   int nErr = SUCCESS;

   switch (pme->m_clsSelf)
   {
#if defined(FEATURE_QCP)
   case AEECLSID_MEDIAQCP:
   {
      int   nCodec;

      MSG_MED("===Select QCP codec===",0,0,0);
      CMediaMMLayer_GetQCPFormat(pme->u.m_pQCP->m_nFormat, (int *)&nCodec);
      pcs->qcp_codec.file_type = (cmx_af_file_enum_type)nCodec;
      if (-1 == nCodec)
         nErr = EINVALIDFORMAT;
      break;
   }
#endif //defined(FEATURE_QCP)

#if defined(FEATURE_MP3)
   case AEECLSID_MEDIAMP3:
   {
      MSG_MED("===Select MP3 codec===",0,0,0);
      pcs->qcp_codec.file_type = CMX_AF_FILE_MP3;
      break;
   }
#endif //defined(FEATURE_MP3)
#ifdef FEATURE_AAC_RAW
   case AEECLSID_MEDIAAAC:
   {
      AEEMediaAACSpec *pAAC = (AEEMediaAACSpec *)pme->m_pSpec;
      MSG_MED("===Select AAC codec===",0,0,0);
      if (!pAAC || !pme->m_dwSpecSize)
         nErr = EBADPARM;
      else
      {
         pcs->aac_codec.codec_type.file_type                 = CMX_AF_FILE_AAC;
         pcs->aac_codec.format                               = (cmx_af_aac_data_format_enum_type) pAAC->nFormat;
         pcs->aac_codec.audio_object                         = (cmx_af_mp4_audio_object_type) pAAC->nAudioObj;
         pcs->aac_codec.sample_rate                          = (cmx_af_sample_rate_enum_type)pAAC->nSampleRate;
         pcs->aac_codec.num_channels                         = (cmx_af_aac_channel_enum_type) pAAC->nNumChannels;
         pcs->aac_codec.bit_rate                             = pAAC->nBitRate;       
         pcs->aac_codec.ep_config                            = (byte) pAAC->nEPConfig;
         pcs->aac_codec.aac_section_data_resilience_flag     = (byte) pAAC->nSectionDataResilience;
         pcs->aac_codec.aac_scalefactor_data_resilience_flag = (byte) pAAC->nScalefactorDataResilience;
         pcs->aac_codec.aac_spectral_data_resilience_flag    = (byte) pAAC->nSpectralDataResilience;
      }
      break;
   }
#endif //FEATURE_AAC_RAW

#if defined(FEATURE_CMX_LINEAR_PCM)
   case AEECLSID_MEDIAPCM: 
#ifdef FEATURE_STD_MIDI
   {
               MSG_MED("Unsupported Format ",0,0,0);
	       nErr = EINVALIDFORMAT;
	       break;
   }
#endif  //FEATURE_STD_MIDI
   case AEECLSID_MEDIAADPCM:
   {
      AEEMediaWaveSpec *pws = (AEEMediaWaveSpec *)pme->m_pSpec;

      MSG_MED("===Select Wave codec===",0,0,0);
      if (!pws || !pme->m_dwSpecSize)
         nErr = EBADPARM;
      else
      {
         pcs->wave_codec.file_type       = CMX_AF_FILE_WAVE;
         pcs->wave_codec.format          = CMX_AF_WAVE_FORMAT_LINEAR_PCM;
         pcs->wave_codec.sample_format   = pws->bUnsigned ? CMX_AF_WAVE_SAMPLE_FMT_UNSIGNED : CMX_AF_WAVE_SAMPLE_FMT_SIGNED;
         pcs->wave_codec.sample_rate     = pws->dwSamplesPerSec;
         pcs->wave_codec.bits_per_sample = pws->wBitsPerSample;
         pcs->wave_codec.channels        = pws->wChannels;
      }
      break;
   }
#endif // defined(FEATURE_CMX_LINEAR_PCM)   
   default:
      nErr = EFAILED;
   }


   return nErr;
}
#endif // defined(FEATURE_CMX_RAW_PLAYBACK)

/*===============================================================================

      COMMON FUNCTION DEFINITIONS
      Can be used by all IMedia classes implemented in this file

===============================================================================*/
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
#if defined(FEATURE_MIDI_OUT_QCP)
   (*ppme)->m_dwRepeatCount = 1;
#endif // defined(FEATURE_MIDI_OUT_QCP)

   return SUCCESS; 
}

/*==================================================================

==================================================================*/
static void OEMMedia_Delete(OEMMedia * pme)
{ 
   FREE(pme);
}

/*==================================================================

==================================================================*/
static int  OEMMedia_Start(OEMMedia * pme)
{
   void *      pUser = (void *)pme->m_ciStart.dwUserData;

   if (!pme->m_ciStart.ph)
      return EBADPARM;

   MSG_HIGH("===OEMMedia_Start:cmx_handle=0x%x,pme->m_ciStart.dwUserData = %d",
            (uint32)pme->m_ciStart.ph,(uint32)pme->m_ciStart.dwUserData,0);
   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY:
         cmx_audfmt_play(pme->m_ciStart.ph, CMX_AF_CALLBACK_VALUE, (cmx_audfmt_start_enum_type)pme->m_eType, 
                         (cmx_audfmt_play_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                         pUser);
         break;

      case OEMCMX_TYPE_AUDFMT_RINGER:
         cmx_audfmt_ringer(pme->m_ciStart.ph, (uint16)pme->m_dwPauseMS, (cmx_audfmt_start_enum_type)pme->m_eType,
                           (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                           pUser);
         break;

#if defined(FEATURE_MIDI_OUT)
      case OEMCMX_TYPE_MIDI_OUT_MSG:
         cmx_midi_out_msg(pme->m_ciStart.ph,
                          (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                          pUser);
         break;

#if defined(FEATURE_MIDI_OUT_QCP)
      case OEMCMX_TYPE_MIDI_OUT_QCP:
#if defined (FEATURE_QAUDIOFX)
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

#if defined(FEATURE_CMX_RAW_PLAYBACK)
      case OEMCMX_TYPE_AUDFMT_PLAY_CODEC:
      {
         if (!pme->m_dwAudioSync)
         {
            cmx_audfmt_play_codec(pme->m_ciStart.ph, pme->m_pCodecSpec, 
                                 (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                                 pUser);
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

      default:
         return EFAILED;
   }

   return SUCCESS; 
}

/*==================================================================

==================================================================*/
static int OEMMedia_Stop(OEMMedia * pme)
{
  cmx_cb_func_ptr_type pfn;
  void *               pUser;

  if(pme != NULL)
  {
    pfn = (cmx_cb_func_ptr_type)pme->m_ciStatus.dwCB;
    pUser = (void *)pme->m_ciStatus.dwUserData;

    switch (pme->m_ciStart.nType)
    {
      case OEMCMX_TYPE_AUDFMT_PLAY: // Fall thru...
      case OEMCMX_TYPE_AUDFMT_RINGER: // Fall thru...
      case OEMCMX_TYPE_AUDFMT_PLAY_CODEC:
        cmx_audfmt_stop(pfn, pUser);
        break;

      case OEMCMX_TYPE_MIDI_OUT_MSG:
        // No effect.
        break;

#if defined(FEATURE_MIDI_OUT_QCP)
      case OEMCMX_TYPE_MIDI_OUT_QCP:
        cmx_midi_out_qcp_stop((void *)pme->m_dwChannel, pfn, pUser);
        break;
#endif // defined(FEATURE_MIDI_OUT_QCP)

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
        cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam, 
                                 pfn, pUser);
        break;
#endif // FEATURE_MULTISEQUENCER

#if defined(FEATURE_QCP)
      case OEMCMX_TYPE_QCP_RECORD:
        cmx_qcp_record_stop(pfn, pUser);
        break;
#endif // defined(FEATURE_QCP)

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
  else
  {
    return EFAILED;
  }
}

/*==================================================================

==================================================================*/
static int OEMMedia_PauseResume(OEMMedia * pme, boolean bPause)
{
   cmx_cb_func_ptr_type pfn = (cmx_cb_func_ptr_type)pme->m_ciStatus.dwCB;
   void *               pUser = (void *)pme->m_ciStatus.dwUserData;

   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY:
      case OEMCMX_TYPE_AUDFMT_RINGER:
      case OEMCMX_TYPE_AUDFMT_PLAY_CODEC:
         if (bPause)
            cmx_audfmt_pause(pfn, pUser);
         else
            cmx_audfmt_resume(pfn, pUser);
         break;

      case OEMCMX_TYPE_MIDI_OUT_MSG:
         // No effect.
         break;

#if defined(FEATURE_MIDI_OUT_QCP)
      case OEMCMX_TYPE_MIDI_OUT_QCP:
         if (bPause)
            cmx_midi_out_qcp_pause_chan((void *)pme->m_dwChannel, pfn, pUser);
         else
            cmx_midi_out_qcp_resume_chan((void *)pme->m_dwChannel, pfn, pUser);
         break;
#endif // defined(FEATURE_MIDI_OUT_QCP)

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam, 
                                 pfn, pUser);
         break;
#endif // FEATURE_MULTISEQUENCER

#if defined(FEATURE_QCP)
      case OEMCMX_TYPE_QCP_RECORD:
         if (bPause)
            cmx_qcp_record_pause(pfn, pUser);
         else
            cmx_qcp_record_resume(pfn, pUser);
         break;
#endif // defined(FEATURE_QCP)

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
   cmx_cb_func_ptr_type pfn = (cmx_cb_func_ptr_type)pme->m_ciStatus.dwCB;
   void *               pUser = (void *)pme->m_ciStatus.dwUserData;

   if (eSeek != MM_SEEK_CURRENT)
      return EUNSUPPORTED;

   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY:
      case OEMCMX_TYPE_AUDFMT_RINGER:
         if (lTimeMS < 0)
            cmx_audfmt_rewind(-1*lTimeMS, pfn, pUser);
         else
            cmx_audfmt_fforward(lTimeMS, pfn, pUser);
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

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMedia_SetParm(OEMMedia * pme, int16 nParmID, int32 p1, int32 p2, int32 p3, boolean bLocal)
{ 
   cmx_cb_func_ptr_type pfn;
   void *               pUser;
   int                  nErr = SUCCESS;

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
#endif // defined(FEATURE_MIDI_OUT)

   if (!pme)
      return EFAILED;

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
         {
            pme->m_eType = (int16)p2;
         }
         else if (OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
         {
            pme->m_eType = (int16)p2;
            pme->m_dwPauseMS = (uint32)p3;
         }
#ifdef FEATURE_MULTISEQUENCER
         else if (OEMCMX_TYPE_AUDFMT_PLAY_MULTI == pme->m_ciStart.nType)
         {
            pme->m_pMultiParam = (cmx_audfmt_multi_param_type *)p2;
         }
#endif // FEATURE_MULTISEQUENCER 
 
#if defined(FEATURE_CMX_RAW_PLAYBACK)
         else if (OEMCMX_TYPE_AUDFMT_PLAY_CODEC == pme->m_ciStart.nType)
         {
            pme->m_pCodecSpec = (cmx_codec_spec_type *)p2;
            pme->m_dwAudioSync = (uint32)p3;
         }
#endif // FEATURE_CMX_RAW_PLAYBACK   

#if defined(FEATURE_QCP)
         else if (OEMCMX_TYPE_QCP_RECORD == pme->m_ciStart.nType)
         {
            pme->m_pQCPRecParm = (cmx_qcp_rec_para_type *)p2;
            pme->m_bReverse = (boolean)p3;
         }
#endif // defined(FEATURE_QCP)       

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

         if (bLocal)
            break;

         if (pci && OEMCMX_TYPE_AUDFMT_PLAY == pci->nType)
         {
            cmx_audfmt_get_time(pci->ph, 
                                (cmx_audfmt_time_cb_func_ptr_type)pci->dwCB, 
                                (void *)pci->dwUserData);
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
            cmx_audfmt_tempo((uint32)p1, pfn, pUser);
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
         if (!bLocal)
         {
            CMXDataRequest *  pReq = (CMXDataRequest *)p1;
            uint8 *           pBuffer = (uint8 *)p2;
            uint32            dwBufSize = (uint32)p3;

            pReq->pfnDataReq(CMX_SUCCESS, pBuffer, dwBufSize, pReq->pServerUserData);
         }       
         break;

#if defined(FEATURE_MIDI_OUT_QCP)
      case OEMCMX_ID_REPEAT:
         pme->m_dwRepeatCount = (uint32)p1;
         break;
#endif // defined(FEATURE_MIDI_OUT_QCP)

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
   if (pme->m_clsSelf == AEECLSID_MEDIAPMD)
   {
      pme->m_dwCaps |= (MM_CAPS_VIDEO | MM_CAPS_TEXT);
   }

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

   OEMMM_ENTER_CRITICAL_SECTION()
   AEEObjectMgr_Unregister(pme->m_hObject);
   OEMMM_LEAVE_CRITICAL_SECTION()

   // Call base class destructor
   AEEMedia_Delete((IMedia *)pme);

   OEMMedia_Delete(pme->m_pMediaCMX);

   pme->m_bStreaming = FALSE;

#if defined(FEATURE_QTV_DRM_DCF)
   if ( pme->m_pReadRsp.pStream != NULL )
   {
      (void)ixstream_close_sync ( pme->m_pReadRsp.pStream );
      ixstream_delete_stream( pme->m_pReadRsp.pStream );
      pme->m_pReadRsp.pStream = NULL;
   }
#endif // defined(FEATURE_QTV_DRM_DCF)

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

   FREE(pme);
}

/*==================================================================

==================================================================*/
static uint32 CMediaCMX_Release(IMedia * po)
{
   int32     nRef = ((CMediaCMX *)po)->m_nRefs;

   if (nRef)
   {
      nRef = (int32)AEEMedia_Release(po);

      if (!nRef)
      {
         CMediaCMX_Delete((CMediaCMX *)po);
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
   {
#if defined(FEATURE_QTV_DRM_DCF)
   if ( pme->m_pReadRsp.pStream != NULL )
   {
      (void)ixstream_close_sync ( pme->m_pReadRsp.pStream );
      ixstream_delete_stream( pme->m_pReadRsp.pStream );
      pme->m_pReadRsp.pStream = NULL;
   }
#endif // defined(FEATURE_QTV_DRM_DCF)
      return AEEMedia_SetMediaParm(po, nParmID, p1, p2);
   }

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
         {
#if defined(FEATURE_MULTISEQUENCER) || defined(FEATURE_MIDI_OUT_QCP)
            boolean multi = FALSE;

#if defined(FEATURE_MULTISEQUENCER)
                multi = CMediaMMLayer_IsMultiSequenceType((CMediaBg *)pme);
#endif // defined(FEATURE_MULTISEQUENCER)

#endif // defined(FEATURE_MULTISEQUENCER) || defined(FEATURE_MIDI_OUT_QCP)
            if (
#if defined(FEATURE_MIDI_OUT_QCP)
                (AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf) ||
                (AEECLSID_MEDIAADPCM == pme->m_clsSelf) ||
                (AEECLSID_MEDIAPCM == pme->m_clsSelf) ||
                (AEECLSID_MEDIAQCP == pme->m_clsSelf) ||
                (AEECLSID_MEDIAAMR == pme->m_clsSelf) ||
#endif // defined(FEATURE_MIDI_OUT_QCP)
#if defined(FEATURE_MULTISEQUENCER)
                multi ||
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
         {
            if (
#if defined(FEATURE_MIDI_OUT_QCP)
                (AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf) ||
                (AEECLSID_MEDIAADPCM == pme->m_clsSelf) ||
                (AEECLSID_MEDIAPCM == pme->m_clsSelf) ||
                (AEECLSID_MEDIAQCP == pme->m_clsSelf) ||
                (AEECLSID_MEDIAAMR == pme->m_clsSelf) ||
#endif // defined(FEATURE_MIDI_OUT_QCP)
#if defined(FEATURE_MULTISEQUENCER)
                CMediaMMLayer_IsMultiSequenceType((CMediaBg *)pme) ||
#endif // defined(FEATURE_MULTISEQUENCER)
                FALSE)
            {
               *(boolean *)pP1 = pme->m_bChannelShare;
            }
            else
            {
               nRet = EUNSUPPORTED;
            }
         }
         break;

      case MM_PARM_PLAY_REPEAT:
         if (
#if defined(FEATURE_MIDI_OUT_QCP)
            (AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf) ||
             (AEECLSID_MEDIAADPCM == pme->m_clsSelf) ||
             (AEECLSID_MEDIAPCM == pme->m_clsSelf) ||
             (AEECLSID_MEDIAQCP == pme->m_clsSelf) ||
             (AEECLSID_MEDIAAMR == pme->m_clsSelf) ||
#endif // defined(FEATURE_MIDI_OUT_QCP)
#if defined(FEATURE_MULTISEQUENCER)
             CMediaMMLayer_IsMultiSequenceType((CMediaBg *)pme) ||
#endif //defined(FEATURE_MULTISEQUENCER)
            FALSE )
         {
            *pP1 = (int32)pme->m_dwRepeatCount;
         }
         else
         {
            nRet = EUNSUPPORTED;
         }
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
   (void)po; /* unused parameter */
   return EUNSUPPORTED;
}

static int CMediaCMX_StopUnsupported(IMedia * po)
{
   (void)po; /* unused parameter */
   return EUNSUPPORTED;
}

static int CMediaCMX_SeekUnsupported(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   (void)po; /* unused parameter */
   return EUNSUPPORTED;
}

static int CMediaCMX_PauseUnsupported(IMedia * po)
{
   (void)po; /* unused parameter */
   return EUNSUPPORTED;
}

static int CMediaCMX_ResumeUnsupported(IMedia * po)
{
   (void)po; /* unused parameter */
   return EUNSUPPORTED;
}

static int CMediaCMX_GetTotalTimeUnsupported(IMedia * po)
{
   (void)po; /* unused parameter */
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

   if (pcb->cmdNotify.nStatus == MM_STATUS_DONE || pcb->cmdNotify.nStatus == MM_STATUS_ABORT)
   {
      if (OEMMM_ISSTREAMING(pme))
         CMediaMMLayer_FlushStreamData((CMediaBg *)pme);
   }

   AEEMedia_CallbackNotify((AEEMedia *)pme, &mcb);
}

/*===============================================================================

                     CMediaBg PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==================================================================
   Called only once during AEE initialization
==================================================================*/
void IMediaBg_Init(IShell * ps)
{
   if (gInitFlags.gbMediaBgInit)
      return;

   gInitFlags.gbMediaBgInit = TRUE;

   // If found in the mod table, then register the formats...

   if (CMediaFormat_InModTable(AEECLSID_MEDIAMIDI, (void *)IMediaBg_Init, NULL))
   {
      AEEMedia_Init(ps, MT_AUDIO_MIDI, AEECLSID_MEDIAMIDI);
      AEEMedia_Init(ps, "video/mld", AEECLSID_MEDIAMIDI);
      AEEMedia_Init(ps, "audio/x-midi", AEECLSID_MEDIAMIDI);
   }

   if (CMediaFormat_InModTable(AEECLSID_MEDIAMP3, (void *)IMediaBg_Init, NULL))
      AEEMedia_Init(ps, MT_AUDIO_MP3, AEECLSID_MEDIAMP3);

   if (CMediaFormat_InModTable(AEECLSID_MEDIAAAC, (void *)IMediaBg_Init, NULL))
   {
      AEEMedia_Init(ps, MT_AUDIO_AAC, AEECLSID_MEDIAAAC);
      AEEMedia_Init(ps, "audio/apl", AEECLSID_MEDIAAAC);
   }

   if (CMediaFormat_InModTable(AEECLSID_MEDIAIMELODY, (void *)IMediaBg_Init, (void *)NULL))
         AEEMedia_Init(ps, MT_AUDIO_IMELODY, AEECLSID_MEDIAIMELODY);

   if (CMediaFormat_InModTable(AEECLSID_MEDIAAMRWB, (void *)IMediaBg_Init, (void *)NULL))
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

#if defined(FEATURE_AUDFMT_EVB)
   if (CMediaFormat_InModTable(AEECLSID_MediaEVRCNB, (void *)IMediaBg_Init, (void *)NULL))
         AEEMedia_Init(ps, "audio/evb", AEECLSID_MediaEVRCNB);
#endif

#if defined(FEATURE_BREW_EVW)
   if (CMediaFormat_InModTable(AEECLSID_MediaEVRCWB, (void *)IMediaBg_Init, (void *)NULL))
         AEEMedia_Init(ps, "audio/evw", AEECLSID_MediaEVRCWB);
#endif 
   // Initialize ASIC MM Layer here...
   CMediaMMLayer_Init(ps);
}

/*==================================================================
   Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaBg_New(IShell * ps, AEECLSID cls, void **ppif)
{
   int         nRet;
   CMediaBg *  pme;

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
       if (AEECLSID_MEDIAQCP == cls || AEECLSID_MEDIAAMR == cls || AEECLSID_MediaEVRCNB == cls 
		#ifdef FEATURE_BREW_EVW
         || AEECLSID_MediaEVRCWB == cls
		#endif 
         )
         eMethod = AEE_SOUND_METHOD_VOICE;
      else
         eMethod = AEE_SOUND_METHOD_MIDI;

      ISOUND_Get(pme->m_pISound, &si);
      si.eMethod = eMethod;
      ISOUND_Set(pme->m_pISound, &si);

      switch (cls)
      {
         case AEECLSID_MEDIAMP3:
            pme->u.m_pMP3 = MALLOC(sizeof(CMediaMP3));
            if (!pme->u.m_pMP3)
               goto Done;
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

      pme->m_iStartTime = 0; 
      pme->m_iStopTime = -1; 
      pme->m_iCurrentTime = -1; 

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

   pme->m_iCurrentTime = 0; 

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

   pme->m_iCurrentTime = -1; 

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

   nRet = AEEMedia_Seek(po, eSeek, lTimeMS);
   if (nRet != SUCCESS)
      return nRet;

   if (AEEMedia_IsSeekFrame(eSeek))
      return EUNSUPPORTED;

   nRet = CMediaMMLayer_Seek(pme, eSeek, lTimeMS);

   if (nRet == SUCCESS)
   {
      if ((pme->m_iCurrentTime + lTimeMS) < 0)
         pme->m_iCurrentTime = 0; 	  
	  else 	  
         pme->m_iCurrentTime += lTimeMS; 	  
   }

   return nRet; 
}

/*==================================================================

==================================================================*/
static int CMediaBg_Pause(IMedia * po)
{
   CMediaBg * pme = (CMediaBg *)po;
   int            nRet;

   nRet = AEEMedia_Pause(po);
   if (nRet != SUCCESS)
      return nRet;

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

   nRet = AEEMedia_GetTotalTime(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaMMLayer_GetTotalTime(pme);
}

/*==================================================================

   Callback Notification Functions: called in BREW context after
   ISHELL_Resume()

==================================================================*/
/*==================================================================
   Media callback notify -- called in BREW context
==================================================================*/
static void CMediaBg_CallbackNotify(void * pUser)
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

	  // Fast-forward to the start time. 
	  if (pme->m_iStartTime > 0)
	  {
		  CMediaBg_Seek((IMedia *)pme, MM_SEEK_CURRENT, pme->m_iStartTime);
	  }
	  else if (pme->m_iStartTime == -1)
	  {
		  // Start time is last possible time.
		  // Since reverse play is unsupported, don't play at all. 
		  CMediaBg_Stop((IMedia *)pme); 
	  }
   }
   else if (pcb->cmdNotify.nStatus == MM_STATUS_TICK_UPDATE)
   {
      // Increment current time by one tick.
      pme->m_iCurrentTime += pme->m_dwTickTimeMS; 
	  
      // If the stop time has been specified and exceeded, stop.
      if (pme->m_iStopTime != -1 && pme->m_iCurrentTime > pme->m_iStopTime)
	  {
         CMediaMMLayer_Stop(pme); 
	  }	  
   }

   CMediaCMX_CallbackNotify(pcb);
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

   bProc = TRUE;
   switch (nParmID)
   {
      case MM_PARM_VOLUME:
         nRet = CMediaMMLayer_SetVolume(pme, (uint16)p1);
         if (!nRet || nRet == MM_PENDING)
            pme->m_wVolume = (uint16)p1;
         break;

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
	  {
         
         int32 nIsStop = (p1 & ((int32)(0x0000FFFF)));  
         int32 nSeekMode = ((p1 & ((int32)(0xFFFF0000))) >> 16); 				

         if (nSeekMode == MM_SEEK_MODE_TIME)
		 {					 
            // Ensure that the specified time is valid.
            if (p2 >= 0 || p2 == -1)
			{												
			   if (nIsStop)			   
                  pme->m_iStopTime = p2; 				   
               else 						
                  pme->m_iStartTime = p2;  						
               
               if (pme->m_iStopTime != -1 && pme->m_iStartTime > pme->m_iStopTime)			   
                  nRet = EUNSUPPORTED; // Reverse play is unsupported 
               else
                  nRet = SUCCESS; 
			}
            else 			
               nRet = EBADPARM; // Invalid time	(p2)
		 }
         else		 
            nRet = EUNSUPPORTED; //Unsupported seek mode
         break; 
	  }
         
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
           int nState;
              
           if (
#if defined(FEATURE_MULTISEQUENCER)
                CMediaMMLayer_IsMultiSequenceType(pme) ||
#endif // defined(FEATURE_MULTISEQUENCER)
                FALSE)
           {
              nState = IMEDIA_GetState(po, &bChanging);
              if (nState == MM_STATE_READY || nState == MM_STATE_IDLE)
              {
                 if (pme->m_bChannelShare != bEnable)
                 {
                   pme->m_bChannelShare = bEnable;
                 }
                 nRet = SUCCESS;
              }
              else
              {
                 nRet = EBADSTATE;
              }
           }
           else
           {
              nRet = EUNSUPPORTED;
           }
      }
      break;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
      case MM_PARM_NOTES:
         if (CMediaMMLayer_IsMultiSequenceType(pme))
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
         if (!pP1 || !pP2)		 
            nRet = EBADPARM; 
         else if ((*pP1) != MM_SEEK_MODE_TIME) 
            nRet = EUNSUPPORTED; 
         else if (pme->m_iCurrentTime == -1)			
            nRet = EBADSTATE; // Not playing - no current time
         else 
		 {
            *pP1 = pme->m_iCurrentTime; 
            *pP2 = pme->m_iCurrentTime; 
		 }
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
         {
            if (
#if defined(FEATURE_MULTISEQUENCER)
                CMediaMMLayer_IsMultiSequenceType(pme) ||
#endif // defined(FEATURE_MULTISEQUENCER)
                FALSE)
            {
               *(boolean *)pP1 = pme->m_bChannelShare;
            }
            else
            {
               nRet = EUNSUPPORTED;
            }
         }
         break;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
      case MM_PARM_NOTES:
         if (pP1 && CMediaMMLayer_IsMultiSequenceType(pme))
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

   pme->m_bStarting = FALSE;  /*Set false for the bStarting flag */ 
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
   else if (AEECLSID_MediaEVRCNB == pme->m_clsSelf)
   {
      nRet = AEEMedia_Record(po);
      if (nRet == SUCCESS)
         nRet = CMediaMMLayer_EVRCBRecord(pme);
   }
#ifdef FEATURE_BREW_EVW   
   else if (AEECLSID_MediaEVRCWB == pme->m_clsSelf)
   {
      nRet = AEEMedia_Record(po);
      if (nRet == SUCCESS)
         nRet = CMediaMMLayer_EWBRecord(pme);
   }
#endif   
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
   else
      nRet = EUNSUPPORTED;

   return nRet;
}

/*===============================================================================

                     IMediaMIDIOutMsg PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==================================================================
   Called only once during AEE initialization
==================================================================*/
void IMediaMIDIOutMsg_Init(IShell * ps)
{
   // Initialize ASIC MM Layer here...
   CMediaMMLayer_Init(ps);
}

/*==================================================================
   IMediaMIDIOutMsg constructor
   Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaMIDIOutMsg_New(IShell * ps, AEECLSID cls, void **ppif)
{
   IBase * pobj = CMediaCMX_New(ps, cls, &gMediaMIDIOutMsgFuncs, sizeof(CMediaMIDIOutMsg), (PFNNOTIFY)CMediaMIDIOutMsg_Delete);

   if (pobj)
   {
      CMediaMIDIOutMsg *  pme = (CMediaMIDIOutMsg *)pobj;
      CMediaMMLayer_InitCallback((IMedia *)pme, MM_CMD_PLAY, pme->m_cbPlay, OEMMM_MAX_CB_PLAY, CMediaCMX_CallbackNotify);
      CMediaMMLayer_InitCallback((IMedia *)pme, 0, &pme->m_cbCommand, 1, CMediaCMX_CallbackNotify);
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

   FREEIF( pme->m_pMsg );

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
         if (( 0 == p1 ) || ( 0 == p2 ))
         {
            nRet = EBADPARM;
         }
         else
         {
            uint32 nSize = ( uint32 ) p2;

            if ( pme->m_dwSize < nSize )
            {
               uint8 * pnBuf = ( uint8 * ) REALLOC( pme->m_pMsg, nSize );

               if ( NULL == pnBuf )
               {
                  FREEIF( pme->m_pMsg );
               }
               pme->m_pMsg = pnBuf;
            }

            if ( NULL != pme->m_pMsg )
            {
               MEMCPY( pme->m_pMsg, ( uint8 * ) p1, nSize );
               pme->m_dwSize = nSize;
               nRet = SUCCESS;
            }
            else
            {
               nRet = ENOMEMORY;
            }
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
void IMediaMIDIOutQCP_Init(IShell * ps)
{
   if (gInitFlags.gbMediaMIDIOutQCPInit)
      return;

   gInitFlags.gbMediaMIDIOutQCPInit = TRUE;

   // If found in the mod table, then register the formats...

   if (CMediaFormat_InModTable(AEECLSID_MEDIAMIDIOUTQCP, (void *)IMediaMIDIOutQCP_Init, NULL))
      AEEMedia_Init(ps, MT_AUDIO_QCF, AEECLSID_MEDIAMIDIOUTQCP);

   // Initialize ASIC MM Layer here...
   CMediaMMLayer_Init(ps);
}

/*==================================================================
   IMediaMIDIOutQCP constructor
   Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaMIDIOutQCP_New(IShell * ps, AEECLSID cls, void **ppif)
{
   IBase * pobj = CMediaCMX_New(ps, cls, &gMediaMIDIOutQCPFuncs, sizeof(CMediaMIDIOutQCP), (PFNNOTIFY)CMediaMIDIOutQCP_Delete);

   if (pobj)
   {

      CMediaMIDIOutQCP *  pme = (CMediaMIDIOutQCP *)pobj;
#ifdef FEATURE_JAVA
#error code not present
#endif
      CMediaMMLayer_InitCallback((IMedia *)pme, MM_CMD_PLAY, pme->m_cbPlay, OEMMM_MAX_CB_PLAY, CMediaCMX_CallbackNotify);
      CMediaMMLayer_InitCallback((IMedia *)pme, 0, &pme->m_cbCommand, 1, CMediaCMX_CallbackNotify);
#if defined(FEATURE_QAUDIOFX)
      CMediaMMLayer_InitCallback((IMedia *)pme, 0, &pme->m_cb3DStart, 1, CMediaMIDIOutQCP_QCPStartCB);
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
   CMediaMMLayer_CancelCallback(&pme->m_cb3DStart, 1);
   {
      paramCache * pCache;

      OEMMM_ENTER_CRITICAL_SECTION()

      while (NULL != (pCache = pme->m_pCache))
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
         nRet = CMediaMMLayer_MIDIOutQCPSetVolume(pme, (uint16)p1);
         break;

      case MM_PARM_PAN:
         nRet = CMediaMMLayer_MIDIOutQCPSetPan(pme, (uint16)p1);
         break;

      case MM_PARM_TEMPO:
      case MM_PARM_TUNE:
         break;

#ifdef FEATURE_JAVA
#error code not present
 #endif

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
#ifdef FEATURE_JAVA
#error code not present
#endif

   // Tempo/Mute/Pan is handled.
   switch (nParmID)
   {
      case MM_PARM_TEMPO:
      case MM_PARM_TUNE:
      case MM_PARM_PAN:
         break;
#ifdef FEATURE_JAVA
#error code not present
#endif

      default:
         bProc = FALSE;
         break;
   }

   if (bProc)
      return nRet;

   return CMediaCMX_GetMediaParm(po, nParmID, pP1, pP2);
}

#if defined (FEATURE_QAUDIOFX)
/*==================================================================
 This function gets media parms that are for OEM use only
==================================================================*/
int CMediaMIDIOutQCP_GetMediaParmOEM(IMedia * po, int nParmID, int32 * pP1, int32 * pP2)
{
   int                  nRet = SUCCESS;
   CMediaMIDIOutQCP * pme = (CMediaMIDIOutQCP *)po;
   
   switch (nParmID)
   {
      case MM_PARM_OEM_CHANNEL:
         if (pme->m_bChannelSet)
         {
            if (pP1)
            {
               *pP1 = pme->m_dwChannel;
            }
         }
         else
         {
            nRet = EBADSTATE;
         }
         break;
         
         default:
            nRet = EUNSUPPORTED;
            break;
   }

   return nRet;
}
#endif // defined (FEATURE_QAUDIOFX)

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

   return CMediaMMLayer_MIDIOutQCPStop(pme);
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
   if (!pme->m_pISound)
      return EUNSUPPORTED;

   if (wVolume > AEE_MAX_VOLUME)
      return EBADPARM;

   ISOUND_Get(pme->m_pISound, NULL);	// Update the current device settings
   ISOUND_SetVolume(pme->m_pISound, wVolume);

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
#if defined(FEATURE_AAC)
   if (pme->m_clsSelf == AEECLSID_MEDIAAAC && MM_APATH_LOCAL == nAPath)
   {
      ; // ok
   }
   else
#endif // defined(FEATURE_AAC)
   if (!OEMMM_IsVocType(pme->m_clsSelf) && 
       pme->m_clsSelf != AEECLSID_MEDIAPCM &&
       pme->m_clsSelf != AEECLSID_MEDIAADPCM)
   {
      return EUNSUPPORTED;
   }

   if (nAPath > MM_APATH_BOTH)
   {
      return EBADPARM;
   }

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

   MSG_HIGH("===Put:F:%ul S:%ul",pme->nFill,nRet,0);
   MSG_HIGH("===Put:R:%ul W:%ul",pme->nWrite, pme->nRead,0);
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

   MSG_HIGH("===Get:F:%ul S:%ul",pme->nFill,nRet,0);
   MSG_HIGH("===Get:R:%ul W:%ul",pme->nWrite, pme->nRead,0);
   return nRet;
}

/*==================================================================
   Called in CMX context.
==================================================================*/
void CMediaMMLayer_CMXDataRequestCB(
  uint32 position,
  uint32 max_len,
  cmx_client_req_cb_func_ptr_type data_req_cb,
  const void *server_client_data,
  const void *client_data
)
{
   CMXDataRequest req;
   CMediaBg *  pme = (CMediaBg *)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);

   MSG_HIGH("===CMXCB: pos=%ul len=%ul",position, max_len,0);

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
static void CMediaMMLayer_DeliverCMXData ( CMediaBg * pme )
{
   int nRead = -1;

   while ( 1 )
   {
      // called only internally, so pme != NULL always
      int nLen = MIN(pme->m_dwSrcBufferSize, pme->m_sReq.dwMaxLen);

#if defined(FEATURE_QTV_DRM_DCF)
      if ( pme->m_bReadPending )
      {
         break;
      }
#endif // defined(FEATURE_QTV_DRM_DCF)

      nRead = CQueue_Get(pme->m_pqMediaData, pme->m_pSrcBuffer, nLen);

      MSG_HIGH("===CMXNotify: nReq=%ul nLen=%d",pme->m_sReq.dwMaxLen, nLen,0);

      if ( CMX_CLIENT_DATA_RANDOM != pme->m_eStreamingType )
      {
         CMediaMMLayer_RequestSourceData(pme);
      }

      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_DATA_RESPONSE, (int32)&pme->m_sReq, (int32)pme->m_pSrcBuffer, (int32)nRead, FALSE);
      pme->m_sReq.bReqPending = FALSE;

      if ( !CQueue_Get ( pme->m_pqCMXDataReq, ( uint8 * ) & pme->m_sReq, sizeof( pme->m_sReq )))
      {
         break;
      }

      pme->m_sReq.bReqPending = TRUE;

      if ( !pme->m_bEOS && CQueue_IsEmpty ( pme->m_pqMediaData ))
      {
         MSG_MED("===CMXNotify: No media data",0,0,0);
         CMediaMMLayer_RequestSourceData ( pme );
         break;
      }
   }

   if ( pme->m_bEOS && ( 0 == nRead ) && !pme->m_bStopping)
   {
      IMEDIA_Stop(( IMedia * )pme );
   }
}

/*==================================================================

==================================================================*/
static void CMediaMMLayer_CMXDataReqNotify ( CMediaBg * pme )
{
   if ( NULL == pme )
   {
      return;
   }

   if ( !pme->m_sReq.bReqPending )
   {
      if ( !CQueue_Get ( pme->m_pqCMXDataReq, ( uint8 * ) & pme->m_sReq, sizeof( pme->m_sReq )))
      {
         MSG_MED("===CMXNotify: Fatal mdq error",0,0,0);
         return;
      }

      pme->m_sReq.bReqPending = TRUE;

      if ( !pme->m_bEOS && CQueue_IsEmpty ( pme->m_pqMediaData ))
      {
         MSG_MED("===CMXNotify: No media data",0,0,0);
         CMediaMMLayer_RequestSourceData ( pme );
      }
      else
      {
         CMediaMMLayer_DeliverCMXData ( pme );
      }
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
      MSG_MED("===ReqSrc: MORE data",0,0,0);
      AEE_ResumeCallback(pme->m_pcbMediaData, pme->m_pac);
   }
}

#if defined(FEATURE_QTV_DRM_DCF)
/*==================================================================

==================================================================*/
static void CMediaMMLayer_IxStreamReadAsyncCB ( void * pUser )
{
   IxStreamReadRspT * pReadResp = ( IxStreamReadRspT * ) pUser;
   CMediaCMX        * pme = ( CMediaCMX * ) AEEObjectMgr_GetObject (( AEEObjectHandle ) pReadResp->pUserData );

   if ( NULL == pme )
   {
      return;
   }

   if ( pme->m_bCancelRead )
   {
      pme->m_bReadPending = FALSE;
      pme->m_bCancelRead = FALSE;

      // kick off a new read if needed
      if ( !CQueue_IsEmpty ( pme->m_pqCMXDataReq ))
      {
         AEE_ResumeCallback(pme->m_pcbCMXDataReq, pme->m_pac);
      }
   }
   else if ( pme->m_bReadPending )
   {
      pme->m_bReadPending = FALSE;
      if ( E_SUCCESS != pReadResp->error )
      {
         pme->m_bEOS = TRUE;
      }
      else if ( !CQueue_Put ( pme->m_pqMediaData, pme->m_pSrcBuffer, pReadResp->nBytesRead ))
      {
         MSG_HIGH("===ReadSrc: Put error nRead=%l",pReadResp->nBytesRead,0,0);
         return;
      }
      MSG_MED("===ReadSrc: Proc CMXRq",0,0,0);

      if ( pme->m_sReq.bReqPending )
      {
         CMediaMMLayer_DeliverCMXData (( CMediaBg * ) pme );
      }
      else if ( !pme->m_bEOS && ( CMX_CLIENT_DATA_RANDOM != pme->m_eStreamingType ))
      {
         CMediaMMLayer_RequestSourceData (( CMediaBg * ) pme );
      }
   }
}
#endif // defined(FEATURE_QTV_DRM_DCF)

/*==================================================================

==================================================================*/
static void CMediaMMLayer_ReadSource(CMediaBg * pme)
{
   ISource *   pSource = (ISource *)pme->m_md.pData;
   int32       nRead;
   boolean     bNotify = FALSE;

   while (1)
   {
      if (CQueue_IsFull(pme->m_pqMediaData) || !CQueue_IsPutOk(pme->m_pqMediaData, (int32)pme->m_dwSrcBufferSize)
#if defined(FEATURE_QTV_DRM_DCF)
         || pme->m_bReadPending || (!pme->m_sReq.bReqPending && (CMX_CLIENT_DATA_RANDOM == pme->m_eStreamingType))
#endif // defined(FEATURE_QTV_DRM_DCF)
         )
      {
         break;
      }

      nRead = 0;
      if (CMX_CLIENT_DATA_RANDOM != pme->m_eStreamingType)
      {
          // sequential streaming
#if defined(FEATURE_QTV_DRM_DCF) 
         if ( pme->m_pReadRsp.pStream != NULL )
         {
            if ( E_AGAIN == ixstream_read_async ( pme->m_pReadRsp.pStream, (byte *) pme->m_pSrcBuffer, pme->m_dwSrcBufferSize, (uint32 *) &nRead, (IxStreamCbT) CMediaMMLayer_IxStreamReadAsyncCB, & pme->m_pReadRsp ))
            {
               pme->m_bReadPending = TRUE;
               break;
            }
            if (0 == nRead)
            {
               MSG_HIGH("===ReadSrc: EOS %d",nRead,0,0);
               pme->m_bEOS = TRUE;
               break;
            }
         }
         else
#endif // defined(FEATURE_QTV_DRM_DCF)
         {
            nRead = ISOURCE_Read(pSource, (char *)pme->m_pSrcBuffer, (int32)pme->m_dwSrcBufferSize);

            if (ISOURCE_WAIT == nRead)
            {
               ISOURCE_Readable(pSource, pme->m_pcbMediaData);
               break;
            }

            if (ISOURCE_END == nRead || ISOURCE_Exhausted(nRead))
            {
               MSG_HIGH("===ReadSrc: EOS %d",nRead,0,0);
               pme->m_bEOS = TRUE;
               break;
            }
         }
      }
      else
      {
#if defined(FEATURE_QTV_DRM_DCF) 
         if ( E_SUCCESS == ixstream_seek ( pme->m_pReadRsp.pStream, (const int32) pme->m_sReq.dwPos, IX_STRM_SEEK_START ))
         {
            if ( pme->m_sReq.dwMaxLen > pme->m_dwSrcBufferSize )
            {
               pme->m_sReq.dwMaxLen = pme->m_dwSrcBufferSize;
            }

            if ( E_AGAIN == ixstream_read_async ( pme->m_pReadRsp.pStream, (byte *) pme->m_pSrcBuffer, pme->m_sReq.dwMaxLen, (uint32 *) &nRead, (IxStreamCbT) CMediaMMLayer_IxStreamReadAsyncCB, & pme->m_pReadRsp ))
            {
               pme->m_bReadPending = TRUE;
               break;
            }
         }
         if (0 == nRead)
         {
            MSG_HIGH("===ReadSrc: EOS %d",nRead,0,0);
            pme->m_bEOS = TRUE;
            break;
         }
#else
         MSG_MED("===ReadSrc: Random access ISource not supported",0,0,0);
         break; // not supported
#endif // defined(FEATURE_QTV_DRM_DCF)
      }

      if ( !CQueue_Put ( pme->m_pqMediaData, pme->m_pSrcBuffer, nRead ))
      {
         MSG_HIGH("===ReadSrc: Put error nRead=%l",nRead,0,0);
         break;
      }

      bNotify = TRUE;

#if defined(FEATURE_QTV_DRM_DCF) 
      if ( pme->m_pReadRsp.pStream != NULL )
      {
         break; // something is wrong or we read to the EOF
      }
#endif // defined(FEATURE_QTV_DRM_DCF) 
   }

   if ( bNotify && pme->m_sReq.bReqPending )
   {
      MSG_MED("===ReadSrc: Proc CMXRq",0,0,0);
      CMediaMMLayer_DeliverCMXData(pme);
   }
}

/*==================================================================
   Assumption: cpBuf is not NULL
==================================================================*/

#if defined(FEATURE_BREW_DETECT_MP3)
int OEMMedia_DetectTypeMP3Ex(const void * cpBuf, uint32 * pdwSize)
{
    cmx_handle_type    ph;
    int nRet = EFAILED;
    static int audfmt_spec = CMX_AF_FILE_UNKNOWN;
    byte *   cbBuf = (byte *)cpBuf; 
    
    ph.mem.source_type = CMX_SOURCE_MEM;
    ph.mem.buf = cbBuf;
    ph.mem.len = *pdwSize;

    if(ph.mem.len){
       ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
        cmx_audfmt_get_specs(&ph,(cmx_audfmt_spec_cb_func_ptr_type)  CMediaCMX_DetectMP3CB, &audfmt_spec);
       ( void ) rex_wait ( AEE_MEDIA_SIG );
         
        if(audfmt_spec == CMX_AF_FILE_MP3){
            nRet = SUCCESS;
        }

     }
     return nRet;
}
/*==================================================================

==================================================================*/
 void CMediaCMX_DetectMP3CB( cmx_status_type        status,       /* Status of event                     */
                             const void             *client_data, /* Client data passed in to the command*/
                             cmx_af_audio_spec_type *audio_spec   /* Audio spec of requested file        */
                             )
{   
    switch(status){
         case CMX_DONE:
            switch (audio_spec->file_type) {
                case CMX_AF_FILE_MP3:
                    *(int *)client_data = (int)CMX_AF_FILE_MP3;
                    rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );
                    break;
                default:
                    *(int *)client_data = (int)CMX_AF_FILE_UNKNOWN;
                    rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );
                    break;
        }
            break; 
        default:
            *(int *)client_data = (int)CMX_AF_FILE_UNKNOWN;
            rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );
            break;
    }
}
#endif

/*==================================================================
   Assumption: cpBuf is not NULL
==================================================================*/


#if defined(FEATURE_BREW_DETECT_AAC)
int OEMMedia_DetectTypeAACEx(const void * cpBuf, uint32 * pdwSize)
{
    cmx_handle_type    ph;
    int nRet = EFAILED;
    static int audfmt_spec = CMX_AF_FILE_UNKNOWN;
    byte *   cbBuf = (byte *)cpBuf; 
    
    ph.mem.source_type = CMX_SOURCE_MEM;
    ph.mem.buf = cbBuf;
    ph.mem.len = *pdwSize;

    if(ph.mem.len){
       ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
        cmx_audfmt_get_specs(&ph,(cmx_audfmt_spec_cb_func_ptr_type)  CMediaCMX_DetectAACCB, &audfmt_spec);
       ( void ) rex_wait ( AEE_MEDIA_SIG );
         
        if(audfmt_spec == CMX_AF_FILE_AAC){
            nRet = SUCCESS;
        }

     }
     return nRet;
}
/*==================================================================

==================================================================*/
 void CMediaCMX_DetectAACCB( cmx_status_type        status,       /* Status of event                     */
                             const void             *client_data, /* Client data passed in to the command*/
                             cmx_af_audio_spec_type *audio_spec   /* Audio spec of requested file        */
                             )
{   
    switch(status){
         case CMX_DONE:
            switch (audio_spec->file_type) {
                case CMX_AF_FILE_AAC:
                    *(int *)client_data = (int)CMX_AF_FILE_AAC;
                    rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );
                    break;
                default:
                    *(int *)client_data = (int)CMX_AF_FILE_UNKNOWN;
                    rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );
                    break;
        }
            break;
        default:
            *(int *)client_data = (int)CMX_AF_FILE_UNKNOWN;
            rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );
            break;
    }
}
#endif

/*==================================================================
   Assumption: pme and ph are not NULL.
==================================================================*/
int CMediaMMLayer_SetSource(CMediaCMX * pme, cmx_handle_type * ph)
{
   AEEMedia *     pMedia = (AEEMedia *)pme;
   AEEMediaData * pmd = &pMedia->m_md;
   int            nRet = SUCCESS;
   uint32         nLength = 0xFFFFffff;   // Set it to max.

   if (pmd->clsData == MMD_FILE_NAME)
   {
      ph->efs.source_type = CMX_SOURCE_EFS;
      if (pmd->pData)
         STRLCPY(ph->efs.filename, pMedia->m_pszFile, sizeof(ph->efs.filename));
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

#if defined(FEATURE_QTV_DRM_DCF)
      if ( pme->m_pReadRsp.pStream != NULL )
      {
         // the IxStream is already active; seek to the beginning

         if ( E_SUCCESS != ixstream_seek ( pme->m_pReadRsp.pStream, 0, IX_STRM_SEEK_START ))
         {
            return EBADPARM;
         }
         if ( E_SUCCESS != ixstream_length ( pme->m_pReadRsp.pStream, &nLength, NULL ))
         {
            nLength = 0xFFFFffff;
         }
      }
      else if (( E_SUCCESS == ix_stream_recreate_stream(( ISource * )pmd->pData, & pme->m_pReadRsp.pStream )) &&
               ( pme->m_pReadRsp.pStream != NULL ))
      {
         // this is IxStream with DRM content

#ifdef FEATURE_MULTISEQUENCER
         if ( CMediaMMLayer_IsMultiSequenceType ( pBg ))
         {
            pme->m_eStreamingType = CMX_CLIENT_DATA_RANDOM;
         }
         else
#endif // FEATURE_MULTISEQUENCER
         {
            pme->m_eStreamingType = CMX_CLIENT_DATA_REAL_TIME;
         }
         pme->m_pReadRsp.pUserData = ( void * ) pme->m_hObject;
         if ( E_SUCCESS != ixstream_length ( pme->m_pReadRsp.pStream, &nLength, NULL ))
         {
            nLength = 0xFFFFffff;
         }
      }
      else  // assume it's a regular ISource
#endif // defined(FEATURE_QTV_DRM_DCF)
      if (pme->m_dwRepeatCount != 1 ||
#ifdef FEATURE_MULTISEQUENCER
          CMediaMMLayer_IsMultiSequenceType(pBg) ||
#endif // FEATURE_MULTISEQUENCER
          AEECLSID_MEDIAMIDIOUTMSG == pme->m_clsSelf)
      {
         return EBADPARM;
      }
      else
      {
         pme->m_eStreamingType = CMX_CLIENT_DATA_REAL_TIME;
      }

      if (OEMMM_ISSTREAMING(pBg))
      {
         CMediaMMLayer_FlushStreamData(pBg);
      }
      else
      {
         uint32   dwBufSize = pme->m_dwBufferSize ? pme->m_dwBufferSize : OEMMM_BUFFER_SIZE_MAX;
         MSG_MED("===Src setup: Start",0,0,0);
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

        MSG_MED("===Src setup: Done",0,0,0);
         pBg->m_bStreaming = TRUE;
      }

      ph->client.source_type = CMX_SOURCE_CLIENT;
      ph->client.data_config = pme->m_eStreamingType;
      ph->client.data_len = nLength;
      ph->client.data_req_ptr = CMediaMMLayer_CMXDataRequestCB;

      CMediaMMLayer_ReadSource(pBg);
   }

   return nRet;
}

/*==================================================================

==================================================================*/
static void CMediaMMLayer_FlushStreamData(CMediaBg * pme)
{
    MSG_MED("===Flushing stream data",0,0,0);

//   cmx_audio_playback_control(CMX_AUDIO_PB_CMD_FLUSH, NULL, NULL);

   // Cancel pending callbacks...
   CALLBACK_Cancel(pme->m_pcbMediaData);
   CALLBACK_Cancel(pme->m_pcbCMXDataReq);

   // Flush source data queue.
   CQueue_Flush(pme->m_pqMediaData);

   if (pme->m_sReq.bReqPending)
   {   
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_DATA_RESPONSE, (int32)&pme->m_sReq, (int32)NULL, (int32)0, FALSE);
   }
   // Flush CMX buffers...
   while (!CQueue_IsEmpty(pme->m_pqCMXDataReq))
   {
      if (!CQueue_Get(pme->m_pqCMXDataReq, (uint8 *)&pme->m_sReq, sizeof(pme->m_sReq)))
      {
         break;
      }

      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_DATA_RESPONSE, (int32)&pme->m_sReq, (int32)NULL, (int32)0, FALSE);
   }

   pme->m_sReq.bReqPending = FALSE;
   pme->m_bEOS = FALSE;
#if defined(FEATURE_QTV_DRM_DCF)
   if ( pme->m_bReadPending )
   {
      pme->m_bCancelRead = TRUE;
   }
#endif // defined(FEATURE_QTV_DRM_DCF)
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
         pcb[i].cmdNotify.nStatus =  MM_STATUS_UNKNOWN;
         pcb[i].cmdNotify.dwSize = 0;
         pcbRet = &pcb[i];
         goto Done;
      }
   }

Done:
   OEMMM_LEAVE_CRITICAL_SECTION()

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
   int                  nRet = SUCCESS;
   CMXInfo              ci;

   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

   // If it is raw data, then select codec and start the playback...
   if (pme->m_bRaw)
      return CMediaMMLayer_PlayCodec(pme, &h);

   MSG_HIGH("===CMediaMMLayer_Play: cmx_handle = 0x%x,pme->m_hObject = %d",
           (uint32)(&h), (uint32)(pme->m_hObject),0);


   if (pme->m_nAPath != MM_APATH_LOCAL_RINGER)
   {
      cmx_audfmt_start_enum_type eType;
      pme->m_bStarting = TRUE; 


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
      }
      else
#endif  // FEATURE_MULTISEQUENCER
      {
         if (!OEMMM_IsVocType(pme->m_clsSelf))
         {
            eType = CMX_AUDFMT_START_NORMAL;
         }
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
#if (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
         if (uisnd_get_incall_mixing() &&
             ((pme->m_clsSelf == AEECLSID_MEDIAMP3) ||
              (pme->m_clsSelf == AEECLSID_MEDIAQCP)||
              (pme->m_clsSelf == AEECLSID_MEDIAAAC)))
         {
            eType = CMX_AUDFMT_START_PATH_FORWARD_INCALL;
         }
#endif // (defined(FEATURE_AVS_INCALL_ADEC_PB) || defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))


         ci.nType = OEMCMX_TYPE_AUDFMT_PLAY;
         ci.ph = &h;
         ci.dwCB = (uint32)CMediaMMLayer_PlayCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)eType, 0, TRUE);
      }
   }
   else
   {
      cmx_audfmt_start_enum_type eType = (OEMMM_IsVocType(pme->m_clsSelf)) ? CMX_AUDFMT_START_PATH_FORWARD : CMX_AUDFMT_START_NORMAL;

      pme->m_bStarting = TRUE; 

      ci.nType = OEMCMX_TYPE_AUDFMT_RINGER;
      ci.ph = &h;
      ci.dwCB = (uint32)CMediaMMLayer_RingerCB;
      ci.dwUserData = (uint32)pme->m_hObject;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)eType, (int32)pme->m_wSilenceTimeMS, TRUE);
   }
   OEMMedia_Start(pme->m_pMediaCMX);

   return nRet;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_PlayCodec(CMediaBg * pme, cmx_handle_type * ph)
{
#if defined(FEATURE_CMX_RAW_PLAYBACK)
   int                  nErr = SUCCESS;
   cmx_codec_spec_type  cs;

   nErr = CMediaCMX_GetCodec(pme, &cs);

   MSG_MED("===Play codec",0,0,0);

   if (!nErr)
   {
      CMXInfo  ci;

      MSG_MED("===Play codec",0,0,0);

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
      pme->m_bStarting = TRUE;

   }

   return nErr;

#else

   return EUNSUPPORTED;

#endif // defined(FEATURE_CMX_RAW_PLAYBACK)
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_Stop(CMediaBg * pme)
{
   AEEMediaCallback * pcb = &pme->m_cbCommand;
//   int                nCmd;
   boolean            bChanging;
   int                nState = IMEDIA_GetState((IMedia *)pme, &bChanging);

   if (((nState == MM_STATE_READY || nState == MM_STATE_IDLE) &&
	!(pme->m_bStarting == TRUE)))
   {
      return SUCCESS;
   }

   // Callback must always be available for stop command.
   CMediaMMLayer_FreeCallback(pcb);
//   nCmd = AEEMedia_IsPlayOn(pme) ? MM_CMD_PLAY : MM_CMD_RECORD;
//   CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, nCmd, OEMMM_CTLCMD_STOP);

   // Media is stopping...do not process callback data.
   pme->m_bStopping = TRUE;

   if (OEMMM_IsVocType(pme->m_clsSelf) && AEEMedia_IsRecOn(pme))
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
      else if (pme->m_nAPath == MM_APATH_BOTH)
         link = CMX_PCM_REC_LINK_BOTH;
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

         OEMMedia_Seek(pme->m_pMediaCMX, MM_SEEK_CURRENT, lTimeMS);
         break;

      case MM_SEEK_START:
      case MM_SEEK_END:
      {
         CMediaMMLayer_FreeCallback(pcb);
         return EUNSUPPORTED;
      }
   }

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_Pause(CMediaBg * pme)
{
   AEEMediaCallback *   pcb;
   int                  nCmd;

   nCmd = AEEMedia_IsPlayOn(pme) ? MM_CMD_PLAY : MM_CMD_RECORD;
   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, nCmd, OEMMM_CTLCMD_PAUSE);
   if (!pcb)
      return EITEMBUSY;

   if (OEMMM_IsVocType(pme->m_clsSelf) && AEEMedia_IsRecOn(pme))
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
      else if (pme->m_nAPath == MM_APATH_BOTH)
         link = CMX_PCM_REC_LINK_BOTH;
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
#else
      return EFAILED;
#endif // defined(FEATURE_PCM_REC)
   }
   else if (pme->m_clsSelf == AEECLSID_MEDIAAAC && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_AAC_REC)
      OEMMedia_Pause(pme->m_pMediaCMX);
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

      OEMMedia_Pause(pme->m_pMediaCMX);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int CMediaMMLayer_Resume(CMediaBg * pme)
{
   AEEMediaCallback *   pcb;
   int                  nCmd;

   nCmd = AEEMedia_IsPlayOn(pme) ? MM_CMD_PLAY : MM_CMD_RECORD;
   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, nCmd, OEMMM_CTLCMD_RESUME);
   if (!pcb)
      return EITEMBUSY;

   if (OEMMM_IsVocType(pme->m_clsSelf) && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_QCP)
     OEMMedia_Resume(pme->m_pMediaCMX);
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
      else if (pme->m_nAPath == MM_APATH_BOTH)
         link = CMX_PCM_REC_LINK_BOTH;
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
#else
      return EFAILED;
#endif // defined(FEATURE_PCM_REC)
   }
   else if (pme->m_clsSelf == AEECLSID_MEDIAAAC && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_AAC_REC)
      OEMMedia_Resume(pme->m_pMediaCMX);
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
   }
   
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

   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

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


   if (dwPan > MM_MAX_PAN)
      return EBADPARM;

   pcb = CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_PAN);
   if (!pcb)
      return EITEMBUSY;

#if !defined(AEE_SIMULATOR)
   pme->m_wPan = (uint16)dwPan;

#ifdef FEATURE_MULTISEQUENCER
   if (CMediaMMLayer_IsMultiSequence(pme))
   {
      cmx_audfmt_seq_param_type seqParam;
      seqParam.cmd = CMX_AUDFMT_SEQ_CMD_PAN;
      seqParam.num = dwPan;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, FALSE);      
   }
   else
#endif // FEATURE_MULTISEQUENCER
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

      if (pme->m_nAPath == MM_APATH_LOCAL || pme->m_nAPath == MM_APATH_REMOTE || pme->m_nAPath == MM_APATH_BOTH)
      {
         CMXInfo  ci;

         ci.nType = OEMCMX_TYPE_QCP_RECORD;
         ci.ph = &h;
         ci.dwCB = (uint32)CMediaMMLayer_RecordCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&qcprecParam, (pme->m_nAPath == MM_APATH_REMOTE), TRUE); // MM_APATH_LOCAL
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
   else if (pme->m_nAPath == MM_APATH_BOTH)
      cmx_qcp_record_both(&h, fmt, pme->m_dwTickTimeMS, CMediaMMLayer_RecordCB, (void *)pme->m_hObject);
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
#if defined(FEATURE_AUDFMT_EVB)
      case MM_QCP_FORMAT_VAR_FULL_4GV_NB:       fmt = CMX_QCP_FORMAT_VAR_FULL_4GV_NB; break;
      case MM_QCP_FORMAT_FIXED_FULL_4GV_NB:     fmt = CMX_QCP_FORMAT_FIXED_FULL_4GV_NB; break;
#endif // defined(FEATURE_AUDFMT_EVB)

      case MM_QCP_FORMAT_FIXED_FULL_EFR:        fmt = CMX_QCP_FORMAT_FIXED_FULL_EFR; break;
      case MM_QCP_FORMAT_FIXED_FULL_FR:         fmt = CMX_QCP_FORMAT_FIXED_FULL_FR; break;
      case MM_QCP_FORMAT_FIXED_FULL_HR:         fmt = CMX_QCP_FORMAT_FIXED_FULL_HR; break;

	  #if defined(FEATURE_BREW_EVW)
      case MM_QCP_FORMAT_VAR_FULL_4GV_WB:       fmt = CMX_QCP_FORMAT_VAR_FULL_4GV_WB; break;
      case MM_QCP_FORMAT_FIXED_FULL_4GV_WB:     fmt = CMX_QCP_FORMAT_FIXED_FULL_4GV_WB; break;
      #endif // defined(FEATURE_BREW_EVW)
      
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
   else if (pme->m_nAPath == MM_APATH_BOTH)
      pcmrecParam.link = CMX_PCM_REC_LINK_BOTH;
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
#ifdef FEATURE_BREW_EVW   
static int CMediaMMLayer_EWBRecord(CMediaBg * pme)
{
#if !defined(FEATURE_BREW_EVW)
   return EFAILED;
#else
   cmx_handle_type      h;
   AEEMediaCallback *   pcb;
   int                  nRet = SUCCESS;

   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

   pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_RECORD, 0);
   if (!pcb)
      return EITEMBUSY;

   {
      cmx_qcp_rec_para_type qcp_rec_param;
      qcp_rec_param.auto_stop_ms = 0;
      qcp_rec_param.report_ms = pme->m_dwTickTimeMS;
      qcp_rec_param.format = CMX_EVW_FORMAT_VAR_FULL_4GV_WB;

      if (pme->m_nAPath == MM_APATH_LOCAL || pme->m_nAPath == MM_APATH_REMOTE || pme->m_nAPath == MM_APATH_BOTH)
      {
         CMXInfo  ci;

         ci.nType = OEMCMX_TYPE_QCP_RECORD;
         ci.ph = &h;
         ci.dwCB = (uint32)CMediaMMLayer_RecordCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&qcp_rec_param, (pme->m_nAPath == MM_APATH_REMOTE), TRUE);
         OEMMedia_Start(pme->m_pMediaCMX);
      }
      else
      {
         CMediaMMLayer_FreeCallback(pcb);
         return EBADPARM;
      }
   }

   pcb->cmdNotify.nStatus = MM_STATUS_START;
   AEE_ResumeCallback(&pcb->cb, pme->m_pac);

   return SUCCESS;
#endif // !defined(FEATURE_BREW_EVW)
}
#endif
static int CMediaMMLayer_EVRCBRecord(CMediaBg * pme)
{
#if !defined(FEATURE_AUDFMT_EVB)
   return EFAILED;
#else
   cmx_handle_type      h;
   AEEMediaCallback *   pcb;
   int                  nRet = SUCCESS;

   nRet = CMediaMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

   pcb = CMediaMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_RECORD, 0);
   if (!pcb)
      return EITEMBUSY;

   {
      cmx_qcp_rec_para_type qcp_rec_param;
      qcp_rec_param.auto_stop_ms = 0;
      qcp_rec_param.report_ms = pme->m_dwTickTimeMS;
      qcp_rec_param.format = CMX_EVB_FORMAT_VAR_FULL_4GV_NB;

      if (pme->m_nAPath == MM_APATH_LOCAL || pme->m_nAPath == MM_APATH_REMOTE || pme->m_nAPath == MM_APATH_BOTH)
      {
         CMXInfo  ci;

         ci.nType = OEMCMX_TYPE_QCP_RECORD;
         ci.ph = &h;
         ci.dwCB = (uint32)CMediaMMLayer_RecordCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&qcp_rec_param, (pme->m_nAPath == MM_APATH_REMOTE), TRUE);
         OEMMedia_Start(pme->m_pMediaCMX);
      }
      else
      {
         CMediaMMLayer_FreeCallback(pcb);
         return EBADPARM;
      }
   }

   pcb->cmdNotify.nStatus = MM_STATUS_START;
   AEE_ResumeCallback(&pcb->cb, pme->m_pac);

   return SUCCESS;
#endif // !defined(FEATURE_AUDFMT_EVB)
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
   else if (pme->m_nAPath == MM_APATH_BOTH)
      aacrecParam.mm_rec_link = CMX_MM_REC_LINK_BOTH;
   else
      aacrecParam.mm_rec_link = CMX_MM_REC_LINK_INVALID;

   aacrecParam.mm_rec_param.mm_rec_format = CMX_MM_FORMAT_AAC;
   aacrecParam.mm_rec_param.cmx_mm_rec_aac_para.mm_rec_format = CMX_MM_FORMAT_AAC;
   aacrecParam.mm_rec_param.cmx_mm_rec_aac_para.aac_file_format = (cmx_af_aac_data_format_enum_type)nFormat;
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
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&aacrecParam, (int32)(pme->m_nAPath == MM_APATH_REMOTE), TRUE);
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
   MSG_HIGH("===CMediaMMLayer_PlayCB: nStatus = %d, eStatus = %d,status = %d",
            (int)nStatus, (uint32)eStatus, (uint32)status);
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
      MSG_HIGH("===CMediaMMLayer_PlayCB: &pcb->cb = 0x%x, pcb->cb.pfnNotify = 0x%x pcb->cb.pNotifyData = 0x%x",
              (uint32)&pcb->cb,(uint32)pcb->cb.pfnNotify,(uint32)pcb->cb.pNotifyData);

      pcb->cmdNotify.nStatus = nStatus;
      CMediaMMLayer_ProcessPlayServerData((CMediaCMX *)pme, status, psd, pcb);

      bResume = TRUE;
   }

OEMMM_CBExit:


   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);

   OEMMM_LEAVE_CRITICAL_SECTION()

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
static int CMediaMMLayer_MapStatus ( AEEMediaCallback * pcb, int nStatus )
{
   if (CMediaMMLayer_ISPLAYCTL(pcb))
   {
      int nSubCmd = pcb->cmdNotify.nSubCmd;

      // The status must be mapped to one of SEEK_FAIL/PAUSE_FAIL/RESUME_FAIL

      if (nSubCmd == OEMMM_CTLCMD_SEEK)
      {
         return MM_STATUS_SEEK_FAIL;
      }
      else if (nSubCmd == OEMMM_CTLCMD_PAUSE)
      {
         return MM_STATUS_PAUSE_FAIL;
      }
      else if (nSubCmd == OEMMM_CTLCMD_RESUME)
      {
         return MM_STATUS_RESUME_FAIL;
      }
   }

   return nStatus;
}

/*==================================================================

==================================================================*/
void CMediaMMLayer_StatusCB(cmx_status_type status, const void * pClientData)
{
   CMediaBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   int   nStatus = MM_STATUS_UNKNOWN;

   MSG_HIGH("=CMediaMMLayer_StatusCB:pClientData= %d",(uint32)pClientData,0,0);
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
      nStatus = CMediaMMLayer_MapStatus ( pcb, nStatus );
      pcb->cmdNotify.nSubCmd = 0;   // Do not send this subcmd to user.
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
   {
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);

      // If this is a memory full error, send out an additional abort signal. 
      if (status == CMX_PCM_INSUFF_SPACE || status == CMX_QCP_SPACE_DONE || 
          status == CMX_PCM_SPACE_WARNING || status == CMX_MM_INSUFF_SPACE)    
      {
         pcb->cmdNotify.nStatus = MM_STATUS_ABORT; 
         AEE_ResumeCallback(&pcb->cb, pme->m_pac); 
      }
   }
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
static boolean CMediaMMLayer_IsMultiSequenceType ( CMediaBg * pme )
{
   int i;
   int numClassesSupported = sizeof(multiSequencerClasses)/sizeof(AEECLSID);

   for (i = 0; i < numClassesSupported; i++)
   {
      if (multiSequencerClasses[i] == pme->m_clsSelf)
      {
         return TRUE;
      }
   }
   return FALSE;
}

static boolean CMediaMMLayer_IsMultiSequence ( CMediaBg * pme )
{
   if ( pme->m_bChannelShare )
   {
      return CMediaMMLayer_IsMultiSequenceType ( pme );
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
      case CMX_STARTED:	  	
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

#if defined(FEATURE_PCM_REC) || defined(FEATURE_AAC_REC)

      // Warning for PCM and AAC recordings - space running low 
      case CMX_PCM_SPACE_WARNING: 
         nStatus = MM_STATUS_SPACE_WARNING; 
         break; 

#endif // FEATURE_PCM_REC || FEATURE_AAC_REC

#ifdef FEATURE_PCM_REC

      // Error for PCM recording - not enough space to start
      case CMX_PCM_INSUFF_SPACE: 
         nStatus = MM_STATUS_SPACE_ERROR; 
         break; 

#endif // FEATURE_PCM_REC

#ifdef FEATURE_AAC_REC

      // Error for AAC recording - not enough space to start
	   case CMX_MM_INSUFF_SPACE: 
         nStatus = MM_STATUS_SPACE_ERROR; 
         break; 

#endif // FEATURE_AAC_REC

#ifdef FEATURE_QCP

      // Warning for AMR recording - space running low
      case CMX_QCP_SPACE_WARNING:
         nStatus = MM_STATUS_SPACE_WARNING;
         break;

      // Error for AMR recording - not enough space to start
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
      default:
      case CMX_SUCCESS:             
         break;

      case CMX_STARTED:
	  nStatus = MM_STATUS_START;
          pme->m_bStarting = FALSE;            
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
static void CMediaMMLayer_MIDIOutNotify(void * pObj, boolean bSuccess);

/*==================================================================

   CMX MIDI Device Functions

==================================================================*/
static int CMIDIOut_AddRef(void)
{
   return ++gsMIDIOut.m_nRef;
}

int CMIDIOut_Release(void)
{
   if ( gsMIDIOut.m_nRef > 0 )
   {
      if ( --gsMIDIOut.m_nRef )
      {
         return( gsMIDIOut.m_nRef );
      }

      OEMMedia_SetParm(NULL, OEMCMX_ID_MIDI_DEVICE, (int32)FALSE, 0, 0, FALSE);
      MEMSET(&gsMIDIOut, (int)0, sizeof(CMIDIOut));
   }

   return 0;
}

int CMIDIOut_Open(byte nADECType, PFNMIDIOUTNOTIFY pfn, void * pme, uint32 handle, boolean synchronous)
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
         gsMIDIOut.m_pHandle = handle;
         gsMIDIOut.m_bSync = synchronous;
         CALLBACK_Init(&gsMIDIOut.m_cb, CMIDIOut_OpenNotify, NULL);

         if (synchronous)
         {
            (void) rex_clr_sigs ( &ui_tcb, AEE_MEDIA_SIG );
         }
#if defined(FEATURE_BREW_CMX_V3)
         {
            CMXInfo  ci;
            ci.nType = OEMCMX_TYPE_MIDI_OUT_QCP;
            ci.ph = NULL;
            ci.dwCB = (uint32)CMIDIOut_MIDIDeviceCB;
            ci.dwUserData = handle;
            OEMMedia_SetParm(NULL, OEMCMX_ID_MIDI_DEVICE, (int32)TRUE, (int32)nADECType, (int32)&ci, FALSE);
         }
#else // CMX 2.x
         cmx_midi_out_open(CMIDIOut_MIDIDeviceCB, (void *)handle);
#endif // defined(FEATURE_BREW_CMX_V3)

         if (synchronous)
         {
            (void) rex_wait( AEE_MEDIA_SIG );
         }
      }
      else
         nRet = EITEMBUSY;
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

            if (gsMIDIOut.m_bSync)
            {
               gsMIDIOut.m_cb.pfnNotify(gsMIDIOut.m_cb.pNotifyData);
            }
            else
            {
            bResume = TRUE;
         }
         }
         break;

      case CMX_DISABLED: // when close() happens
#ifdef FEATURE_ACM
         AEEMedia_CleanUpResource((AEEMedia *)pMedia);
#endif
         break;

      case CMX_SUCCESS: // for open() queueing, prev midi cmd success
         break;

      case CMX_DONE: // never sent??
         break;

      default:
         break;
   }

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&gsMIDIOut.m_cb,0); // Sys resume.
}

static void CMIDIOut_OpenNotify(void * pUser)
{
   CMediaCMX * pme = (CMediaCMX *)AEEObjectMgr_GetObject(gsMIDIOut.m_pHandle);
   boolean     bOpen = ((cmx_status_type)(uint32)pUser) == CMX_ENABLED;

   gsMIDIOut.m_bOpenPending = FALSE;

   if (!pme)
      return;

   if (bOpen)
      CMIDIOut_AddRef();

   gsMIDIOut.m_pfnNotify(gsMIDIOut.m_pMM, bOpen);

   if (gsMIDIOut.m_bSync)
   {
      rex_set_sigs( &ui_tcb, AEE_MEDIA_SIG);
   }
}

static void CMediaMMLayer_MIDIOutNotify(void *pObj, boolean bSuccess)
{
   CMediaCMX *pme = (CMediaCMX *)pObj;
   pme->m_bMIDIOut = bSuccess;

   if (pme->m_clsSelf == AEECLSID_MEDIAMIDIOUTMSG)
   {
      if (bSuccess)
         CMediaMMLayer_MIDIOutMsg((CMediaMIDIOutMsg *)pme);
   }
   else if (pme->m_clsSelf == AEECLSID_MEDIAMIDIOUTQCP ||
            pme->m_clsSelf == AEECLSID_MEDIAQCP        ||
            pme->m_clsSelf == AEECLSID_MEDIAADPCM      ||
            pme->m_clsSelf == AEECLSID_MEDIAPCM        ||
            pme->m_clsSelf == AEECLSID_MEDIAAMR)
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
      return CMIDIOut_Open((byte)CMX_ADEC_MODE_DEFAULT, CMediaMMLayer_MIDIOutNotify, (void *)pme, pme->m_hObject, FALSE);

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
      pme->m_bStarting = TRUE;
      CMediaMIDIOutQCP_FlushParamCacheInt(pme);
      
      ci.nType = OEMCMX_TYPE_MIDI_OUT_QCP;
      ci.ph = NULL;
      ci.dwCB = (uint32)CMediaMMLayer_MIDIOutQCPStatusCB;
      ci.dwUserData = (uint32)pme->m_hObject;
      OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_STATUS_CB, (int32)&ci, 0, 0, TRUE);

      cmx_midi_out_adec_start_chan ((void *)pme->m_dwChannel, (cmx_cb_func_ptr_type)pme->m_pMediaCMX->m_ciStatus.dwCB, (void *)pme->m_pMediaCMX->m_ciStatus.dwUserData);
   }
}
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
      byte   nADECType = CMX_ADEC_MODE_DEFAULT;
      
      if ((AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf) ||
          (AEECLSID_MEDIAAMR == pme->m_clsSelf))
      {
         if (CMIDIOut_IsQCP())
            nADECType = CMX_ADEC_MODE_QCP;
      }
      else if (AEECLSID_MEDIAADPCM == pme->m_clsSelf)
      {
         if (CMIDIOut_IsADPCM())
            nADECType = CMX_ADEC_MODE_ADPCM;
      }
      else if (AEECLSID_MEDIAPCM == pme->m_clsSelf)
      {
         if (CMIDIOut_IsPCM())
            nADECType = CMX_ADEC_MODE_PCM;
      }

#if defined(FEATURE_GASYNTH_INTERFACE)
      // Any mode will work with any media, and they can be mixed
      nADECType = CMX_ADEC_MODE_ADPCM;
#endif //defined(FEATURE_GASYNTH_INTERFACE)
      
      if (nADECType != CMX_ADEC_MODE_DEFAULT)
         return CMIDIOut_Open(nADECType, CMediaMMLayer_MIDIOutNotify, (void *)pme, pme->m_hObject, TRUE);
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

static int CMediaMMLayer_MIDIOutQCPStop(CMediaMIDIOutQCP * pme)
{
   AEEMediaCallback * pcb = &pme->m_cbCommand;
   boolean            bChanging;
   int                nState = IMEDIA_GetState((IMedia *)pme, &bChanging);

   if (nState != MM_STATE_READY && nState != MM_STATE_IDLE)
   {
      pme->m_bStopping = TRUE;

      // Callback must always be available for stop command.
      CMediaMMLayer_FreeCallback(pcb);
      CMediaMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_PLAY, OEMMM_CTLCMD_STOP);

      OEMMedia_Stop(pme->m_pMediaCMX);
   }

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

static int CMediaMMLayer_MIDIOutQCPSetVolume(CMediaMIDIOutQCP * pme, uint16 wVolume)
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

static int CMediaMMLayer_MIDIOutQCPSetPan(CMediaMIDIOutQCP * pme, uint16 wPan)
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
         else if (CMediaMMLayer_ISSTOP(pcb))  // do not send stop done as play done is sent.
         {
            pcb->cmdNotify.nSubCmd = 0;
            goto OEMMM_CBExit;
         }
         else
            pcb->cmdNotify.nStatus = MM_STATUS_DONE;

         pcb->cmdNotify.nSubCmd = 0;
         break;

      case CMX_FAILURE:
         if (CMediaMMLayer_ISPAUSE(pcb))
            pcb->cmdNotify.nStatus = MM_STATUS_PAUSE_FAIL;
         else if (CMediaMMLayer_ISRESUME(pcb))
            pcb->cmdNotify.nStatus = MM_STATUS_RESUME_FAIL;
         else if (CMediaMMLayer_ISSTOP(pcb))  // do not send stop done as play done is sent.
         {
            pcb->cmdNotify.nSubCmd = 0;
            goto OEMMM_CBExit;
         }
         else
            pcb->cmdNotify.nStatus = MM_STATUS_ABORT;

         pcb->cmdNotify.nSubCmd = 0;
         break;

      default: /* shouldn't land here... */
         break;
   }
#if defined (FEATURE_QAUDIOFX)
   if (MM_CMD_PLAY == pcb->cmdNotify.nCmd &&
       (MM_STATUS_DONE == pcb->cmdNotify.nStatus || MM_STATUS_ABORT == pcb->cmdNotify.nStatus))
   {
      pme->m_bStarting = FALSE;
      pme->m_bPlaying = FALSE;
   }
#endif // defined(FEATURE_QAUDIOFX)

   bResume = TRUE;

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
}

#ifdef FEATURE_JAVA
#error code not present
#endif

#if defined (FEATURE_QAUDIOFX)
static void  CMediaMIDIOutQCP_QCPPlayDoneCB (cmx_status_type status, cmx_3d_pos_param_enum_type param, cmx_3d_pos_param_setting_type *setting, const void *pClientData)
{
   CMediaMIDIOutQCP * pme = (CMediaMIDIOutQCP *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);

   if (pme && CMX_SUCCESS == status &&  CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS == param)
   {
      pme->m_bPlaying = FALSE;
      CMediaMIDIOutQCP_SetParamCache(pme, setting);
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
#ifdef FEATURE_JAVA
#error code not present
#endif      
         }
         else
         {
            pcb->cmdNotify.nStatus = MM_STATUS_ABORT; 
#ifdef FEATURE_JAVA
#error code not present
#endif
         }
         break;
#endif // defined(FEATURE_QAUDIOFX)

      case CMX_SUCCESS:
      {
         pme->m_dwChannel = (uint32)midi_out_qcp_id;
         pme->m_bChannelSet = TRUE;
         OEMMedia_SetParm(pme->m_pMediaCMX, OEMCMX_ID_CHANNEL, pme->m_dwChannel, 0, 0, TRUE);

#if defined(FEATURE_QAUDIOFX)
            CMediaMMLayer_FreeCallback(pcb);
            pcb = CMediaMMLayer_AllocCallback(&pme->m_cb3DStart, 1, MM_CMD_PLAY, 0);
            if (!pcb)
               goto OEMMM_CBExit;
#endif // defined(FEATURE_QAUDIOFX)

         pcb->cmdNotify.nStatus = MM_STATUS_START;
#ifdef FEATURE_JAVA
#error code not present
#endif
         break;
      }

      case CMX_FAILURE:
         pcb->cmdNotify.nStatus = MM_STATUS_ABORT;
         pme->m_bChannelSet = FALSE;
#ifdef FEATURE_JAVA
#error code not present
#endif
         break;

      case CMX_DONE:
         pcb->cmdNotify.nStatus = MM_STATUS_DONE;
         pme->m_bChannelSet = FALSE;
#ifdef FEATURE_JAVA
#error code not present
#endif
         break;

      case CMX_ABORTED:
         // Differentiate between error and user stop
         if (pme->m_bStopping)
            pcb->cmdNotify.nStatus = MM_STATUS_DONE;
         else
            pcb->cmdNotify.nStatus = MM_STATUS_ABORT;

         pme->m_bStopping = FALSE;
         pme->m_bChannelSet = FALSE;
#ifdef FEATURE_JAVA
#error code not present
#endif
         break;

      default:
         goto OEMMM_CBExit;
   }

#if defined (FEATURE_QAUDIOFX)
   if (MM_CMD_PLAY == pcb->cmdNotify.nCmd &&
       (MM_STATUS_DONE == pcb->cmdNotify.nStatus || MM_STATUS_ABORT == pcb->cmdNotify.nStatus))
   {
      pme->m_bStarting = FALSE;
      pme->m_bPlaying = FALSE;
   }
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

static int CMediaMMLayer_SetPan(CMediaBg * pme, uint32 dwPan)
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

static int CMediaMMLayer_MIDIOutQCPStop(CMediaMIDIOutQCP * pme)
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

static int CMediaMMLayer_MIDIOutQCPSetVolume(CMediaMIDIOutQCP * pme, uint16 wVolume)
{
   return EUNSUPPORTED;
}

static int CMediaMMLayer_MIDIOutQCPSetPan(CMediaMIDIOutQCP * pme, uint16 wPan)
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

void IMediaMain_Init(IShell * ps);
int  IMediaMain_New(IShell * ps, AEECLSID cls, void **ppif);

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
void IMediaMain_Init(IShell * ps)
{
   if (CMediaFormat_InModTable(AEECLSID_MEDIAQCP, (void *)IMediaMain_Init, NULL))
   {
      AEEMedia_Init(ps, MT_AUDIO_QCP, AEECLSID_MEDIAQCP);
      AEEMedia_Init(ps, MT_AUDIO_VND_QCELP, AEECLSID_MEDIAQCP);
   }

   if (CMediaFormat_InModTable(AEECLSID_MEDIAADPCM, (void *)IMediaMain_Init, NULL))
   {
      AEEMedia_Init(ps, "audio/x-wav", AEECLSID_MEDIAADPCM);
      AEEMedia_Init(ps, MT_AUDIO_ADPCM, AEECLSID_MEDIAADPCM);
   }

   if (CMediaFormat_InModTable(AEECLSID_MEDIAAMR, (void *)IMediaMain_Init, (void *)NULL))
   {
      AEEMedia_Init(ps, MT_AUDIO_AMR, AEECLSID_MEDIAAMR);
   }

}

/*==================================================================

==================================================================*/
int IMediaMain_New(IShell * ps, AEECLSID cls, void **ppif)
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

      nErr = IMediaMIDIOutQCP_New(pme->m_pIShell, cls, (void **)&pme->m_pMedia);

      // set the channel shared flag in m_pMedia, which will default to 0
      ((CMediaMIDIOutQCP *) pme->m_pMedia)->m_bChannelShare = 1;
   }
   else
      nErr = IMediaBg_New(pme->m_pIShell, pme->m_clsSelf, (void **)&pme->m_pMedia);

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
      int nState;
      
      if (
#if defined(FEATURE_MIDI_OUT_QCP)
          (AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf) ||
          (AEECLSID_MEDIAADPCM == pme->m_clsSelf) ||
          (AEECLSID_MEDIAPCM == pme->m_clsSelf) ||
          (AEECLSID_MEDIAQCP == pme->m_clsSelf) ||
          (AEECLSID_MEDIAAMR == pme->m_clsSelf) ||
#endif // defined(FEATURE_MIDI_OUT_QCP)
          FALSE)
      {
         nState = IMEDIA_GetState(po, &bChanging);
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
         {
            return EBADSTATE;
         }
      }
      else
      {
         return EUNSUPPORTED;
      }
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
      if (
#if defined(FEATURE_MIDI_OUT_QCP)
          (AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf) ||
          (AEECLSID_MEDIAADPCM == pme->m_clsSelf) ||
          (AEECLSID_MEDIAPCM == pme->m_clsSelf) ||
          (AEECLSID_MEDIAQCP == pme->m_clsSelf) ||
          (AEECLSID_MEDIAAMR == pme->m_clsSelf) ||
#endif // defined(FEATURE_MIDI_OUT_QCP)
          FALSE)
      {
         if (pP1)
         {
            *(boolean *)pP1 = pme->m_bChannelShare;
         }
         return SUCCESS;
      }
      else
      {
         return EUNSUPPORTED;
      }
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
   if (((pme->m_clsSelf == AEECLSID_MEDIAQCP)         ||
        (pme->m_clsSelf == AEECLSID_MEDIAADPCM)       || 
        (pme->m_clsSelf == AEECLSID_MEDIAPCM)         ||
        (pme->m_clsSelf == AEECLSID_MEDIAAMR)         ||
        (pme->m_clsSelf == AEECLSID_MEDIAMIDIOUTQCP)) &&
       ((idReq == AEECLSID_MEDIAAUDIO3D)              ||
        (idReq == AEEIID_MEDIAAUDIO3D)                ||
        (idReq == AEECLSID_MEDIAAUDIOFX)              ||
        (idReq == AEEIID_MEDIAAUDIOFX)))
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

      if ((idReq == AEECLSID_MEDIAAUDIO3D) || (idReq == AEEIID_MEDIAAUDIO3D))
      {
         if ((nRet = IMediaAudio3D_New(pme->m_pIShell, 
                                       idReq, 
                                       ppo, 
                                       pme->m_pMedia)) == SUCCESS) 
         {
           /* Create default cache */
           CMediaMIDIOutQCP_SetDefault3DParam(pme->m_pMedia);
         }
         return nRet;
   }
      else // idReq == AEECLSID_MEDIAAUDIOFX
      {
        if ((nRet = IMediaAudioMediaFX_New(pme->m_pIShell, 
                                       idReq, 
                                       ppo, 
                                       pme->m_pMedia)) == SUCCESS) {
           /* Create default cache */
          CMediaMIDIOutQCP_SetDefault3DParam(pme->m_pMedia);
         }
         return nRet;
      }
   }
#if defined (FEATURE_ACM) && defined (FEATURE_ODM_UI) 
   else if (idReq == AEECLSID_APPRESCTL)
   {
      // The AppResCtl registered with ACM is the one associated with play/record
      return AEEMedia_QueryInterface(pme->m_pMedia, idReq, ppo);
   } 
#endif // FEATURE_ACM && FEATURE_ODM_UI
   else
#endif //defined (FEATURE_QAUDIOFX)   
   {
      return AEEMedia_QueryInterface(po, idReq, ppo);
   } 
}

#if defined (FEATURE_QAUDIOFX)
void CMediaMIDIOutQCP_SetMediaEnv(IMedia * po, uint32 dwEnable, flg inEnv)
{
   CMediaMain       *pme = (CMediaMain *)po;
   CMediaMIDIOutQCP *pQCP;

   if ((pme->m_clsSelf != AEECLSID_MEDIAMIDIOUTQCP) &&
       (pme->m_clsSelf != AEECLSID_MEDIAQCP)        &&
       (pme->m_clsSelf != AEECLSID_MEDIAADPCM)      &&
       (pme->m_clsSelf != AEECLSID_MEDIAAMR)        &&
       (pme->m_clsSelf != AEECLSID_MEDIAPCM))
      return; // wrong media type

   pQCP = (CMediaMIDIOutQCP *)(pme->m_pMedia);
   pQCP->m_dwEnable = dwEnable;
   pQCP->m_bInEnv = inEnv;
   CMediaMIDIOutQCP_FlushParamCacheInt( pQCP );
}

void CMediaMIDIOutQCP_FlushParamCache(IMedia * po)
{
   CMediaMain *pme = (CMediaMain *)po;

   if ((pme->m_clsSelf != AEECLSID_MEDIAMIDIOUTQCP) &&
       (pme->m_clsSelf != AEECLSID_MEDIAQCP)        &&
       (pme->m_clsSelf != AEECLSID_MEDIAADPCM)      &&
       (pme->m_clsSelf != AEECLSID_MEDIAAMR)        &&
       (pme->m_clsSelf != AEECLSID_MEDIAPCM))
      return; // wrong media type

   CMediaMIDIOutQCP_FlushParamCacheInt((CMediaMIDIOutQCP *)(pme->m_pMedia));
}

void CMediaMIDIOutQCP_RestoreParm(IMedia * po, int nParmID)  
{
  CMediaMIDIOutQCP         *pme = (CMediaMIDIOutQCP *)po;
  cmx_3d_pos_cmd_enum_type  cmx_cmd = (cmx_3d_pos_cmd_enum_type)nParmID;
  paramCache               *pCache = NULL;
  
  pCache = pme->m_pCache;

  while (pCache != NULL) {
    if (pCache->cmd == cmx_cmd) {      
      MSG_MED("Found cached QAFX command", 0, 0, 0);
      cmx_3d_pos_cmd(cmx_cmd, &pCache->cmxParam, NULL, NULL);
    }
    pCache = pCache->pNext;
  }
}
#endif // defined (FEATURE_QAUDIOFX)

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
    // Protect against getting another force release while stopping 
    if(pAEEMedia->m_bForceRelease == FALSE)
    {
      pAEEMedia->m_bForceRelease = TRUE;
      if (pAEEMedia->m_bResOwner == TRUE)
      {
        nRet = CMediaBg_Stop(pIMedia);
        pAEEMedia->m_bResOwner = FALSE;
      }
      else
      {
        AEEMedia_CleanUpResource(pAEEMedia);
      }
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
    // Protect against getting another force release while stopping 
    if(pAEEMedia->m_bForceRelease == FALSE)
    {
      pAEEMedia->m_bForceRelease = TRUE;
      if (pAEEMedia->m_bResOwner == TRUE)
      {
        nRet = CMediaBg_Stop((IMedia *)pAEEMedia);
        pAEEMedia->m_bResOwner = FALSE;
      }
      else
      {
        AEEMedia_CleanUpResource(pAEEMedia);
      }
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
    // Protect against getting another force release while stopping 
    if(pAEEMedia->m_bResOwner == TRUE && pAEEMedia->m_bForceRelease == FALSE)
    {
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

void CMediaMIDIOutQCP_CleanupResource (AEEMedia *pme)
{
   CMediaMIDIOutQCP * pCMediaMIDIOutQCP = (CMediaMIDIOutQCP *) pme;
            
   // close the midi out resource
   if (pCMediaMIDIOutQCP->m_bMIDIOut)
   {
      if (!--gsMIDIOut.m_nRef)
      {
         CMXInfo  ci;
      
         ci.nType = OEMCMX_TYPE_MIDI_OUT_QCP;
         ci.ph = NULL;
         ci.dwCB = (uint32)CMIDIOut_MIDIDeviceCB;
         ci.dwUserData = pCMediaMIDIOutQCP->m_hObject;
         OEMMedia_SetParm(NULL, OEMCMX_ID_MIDI_DEVICE, (int32)FALSE, 0, (int32)&ci, FALSE);
      
         MEMSET(&gsMIDIOut, (int)0, sizeof(CMIDIOut));
      }

      pCMediaMIDIOutQCP->m_bMIDIOut = FALSE;

      // We can free the entry from ACM if the midi out resource 
      // is still considered open
      // The last one that calls CMediaMIDIOutQCP_CleanupResource
      // will need to wait for the callback from CMX regarding 
      // the status of cmx_midi_out_close before calling 
      // AEEMedia_CleanupResource
      if (CMIDIOut_IsOpen())
      {
         AEEMedia_CleanUpResource(pme);
      }     
   }
   // The else part should not happen, add just to be safe
   else
   {
      DBGPRINTF ("midi out not open, received cmx done or abort event");
      AEEMedia_CleanUpResource(pme);
   }
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
    // Protect against getting another force release while stopping 
    if(pAEEMedia->m_bForceRelease == FALSE)
    {
      pAEEMedia->m_bForceRelease = TRUE;
      pAEEMedia->m_bResOwner = FALSE;

      // file is playing
      if (pCMediaMIDIOutQCP->m_bChannelSet == TRUE)
      {
        nRet = CMediaMIDIOutQCP_Stop((IMedia *)pAEEMedia);
      }
      else
      {
        CMediaMIDIOutQCP_CleanupResource (pAEEMedia);
      }
    }
  }
  return nRet;
}
#endif // FEATURE_ACM

#endif // defined(FEATURE_BREW_MULTIMEDIA)


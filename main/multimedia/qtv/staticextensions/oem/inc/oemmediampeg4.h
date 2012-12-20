/*============================================================================*/
/*!
  @file   OEMMediaMPEG4.h

  @brief  Internal OEMMediaMPEG4 functions

  This file contains function and structure declarations that are shared between
  OEMMediaMPEG4.c and OEMMediaMPEG42PV.cpp.

  @note   This file cannot include QtvPlayer.h since it is a C++ header.

*/
/*============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

 ============================================================================
  
                             PERFORCE HEADER

$Header: //source/qcom/qct/multimedia/qtv/staticextensions/oem/main/latest/inc/oemmediampeg4.h#19 $
$Author: spandiri $
$DateTime: 2010/11/01 06:28:10 $
$Change: 1499827 $

 ============================================================================*/

#ifndef OEMMPEG4FORMAT_H
#define OEMMPEG4FORMAT_H

#include "customer.h"
#include "OEMFeatures.h"

#include "AEEMediaMPEG4.h"

#ifdef   FEATURE_APP_MPEG4

#include "AEE_OEM.h"
#include "OEMMedia.h"
#include "AEEMediaFormats.h"
#include "OEMObjectMgr.h"

#if MIN_BREW_VERSION(3,0)
#include "OEMCriticalSection.h"
#endif

#ifdef FEATURE_QTV_OEM_BUFFER_MGR
#include "queue.h"
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */
#ifdef FEATURE_QTV_MDP
#include "IYCbCr.h"
#endif /* FEATURE_QTV_MDP */
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
#include "mdp_drv_api.h"
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

#include"vdecoder_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/*-------------------------------------------------------------------
      Macros Declarations
-------------------------------------------------------------------*/
#define OEMMPEG4_MAX_CB     40  /*!< Static number of callback structures
                                  provided for use in the BREW/UI context */
#define MAX_NUM_OEM_PLAYBACK_INST 3 /*!< Max possible number of OEM Layer 
                                     instances; must match MAX_NUM_QTV_PLAYBACK_INST */ 
#define VGA_SIZE 480


extern OEMCriticalSection TimedText3gppCS;
#define MUTEX_LOCK_CS(a)     OEMCriticalSection_Enter(&a)
#define MUTEX_UNLOCK_CS(a)   OEMCriticalSection_Leave(&a)


/* The following macros allow a common code base on MDP and non-MDP targets. 
 * For MDP targets, we use IYCbCr objects to hold Qtv video frames. For non-MDP targets, 
 * IDIB's are used. These are both cast to the IBITMAP base class for use by the application.
 */
#ifdef FEATURE_QTV_MDP
# define QTVBITMAP_SUBTYPE               IYCbCr
# define QTVBITMAP_IID                   AEEIID_YCBCR
# define QTVBITMAP_SUBTYPE_TO_IBITMAP(p) IYCBCR_TO_IBITMAP(p)
# define QTVBITMAP_IS_BUFFER_SET(p)      ((p)->pLuma != NULL && (p)->pChroma != NULL)
# define QTVBITMAP_CLEAR_BUFFER(p)       ((p)->pLuma = (p)->pChroma = NULL)
# define QTVBITMAP_FRAMEBUFFER(p)        ((p)->pLuma)
#else
# define QTVBITMAP_SUBTYPE               IDIB
# define QTVBITMAP_IID                   AEECLSID_DIB
# define QTVBITMAP_SUBTYPE_TO_IBITMAP(p) IDIB_TO_IBITMAP(p)
# define QTVBITMAP_IS_BUFFER_SET(p)      ((p)->pBmp != NULL)
# define QTVBITMAP_CLEAR_BUFFER(p)       ((p)->pBmp = NULL)
# define QTVBITMAP_FRAMEBUFFER(p)        ((p)->pBmp)
#endif /* FEATURE_QTV_MDP */

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

typedef enum {
  SOUNDCMD_SET_VOLUME,
  SOUNDCMD_GET_VOLUME,
  SOUNDCMD_SET_MUTE,
  SOUNDCMD_GET_MUTE,
  SOUNDCMD_SET_DEVICE,
  SOUNDCMD_GET_DEVICE,
  SOUNDCMD_MAX
}OEMSoundCmdEnumType;

typedef union SoundCmdParamT {
  AEESoundDevice  eDevice;
  uint16          volume;
  boolean         bMute;
} SoundCmdParamType;

typedef struct OEMSoundCmdT{
  OEMSoundCmdEnumType cmd;  
  SoundCmdParamType   param;
  int                 aee_cmd;     // store the MM_CMD_, such as MM_CMD_GETMEDIAPARM
  int                 aee_param;   // store the MM_PARM_, such as MM_PARM_VOLUME
}OEMSoundCmdType;

typedef struct OEMSoundT{
  uint16          current_volume;
  boolean         current_mute;
  boolean         cmd_pending;
  boolean         cb_pending;
  boolean         method_set;
  OEMSoundCmdType sound_cmd;
}OEMSoundType;

typedef enum OEMScalingType {
  OEM_ASCALE       = MM_MPEG4_ASCALE,
  OEM_0P75X_SHRINK = MM_MPEG4_0P75X_SHRINK,     //!< 0.75X scaling
  OEM_0P5X_SHRINK  = MM_MPEG4_2X_SHRINK,        //!< 0.5X scaling
  OEM_0P25X_SHRINK = MM_MPEG4_0P25X_SHRINK,     //!< 0.25X scaling
  OEM_NO_SCALING   = MM_MPEG4_NO_SCALING,       //!< 1X scaling
  OEM_1P25X_ZOOM   = MM_MPEG4_1P25X_ZOOM,       //!< 1.25X scaling
  OEM_2X_ZOOM      = MM_MPEG4_2X_ZOOM,          //!< 2X scaling
  OEM_2P5X_ZOOM    = MM_MPEG4_2P5X_ZOOM,        //!< 2.5X scaling
  OEM_XSCALE 
} OEMScalingType;

typedef enum OEMRotationType {
  OEM_NO_ROTATION     = MM_MPEG4_NO_ROTATION,      //!< video is not rotated
  OEM_90_CW_ROTATION  = MM_MPEG4_90_CW_ROTATION,   //!< video rotated 90 degrees clockwise
  OEM_90_CCW_ROTATION = MM_MPEG4_90_CCW_ROTATION,  //!< video rotated 90 degrees counter clockwise
  OEM_180_ROTATION    = MM_MPEG4_180_ROTATION      //!< video rotated 180 degrees counter clockwise
} OEMRotationType;


typedef struct AEEDataFrameInfo
{
  uint32 FrameNum;      /*!< sample number (first sample is zero)               */
  uint32 FrameSize;     /*!< size of sample in bytes                            */
  uint32 FrameOffset;   /*!< offset of sample in stream, in bytes               */
  uint32 FrameTime;     /*!< composition time of sample, in the msec            */
  uint32 Framedelta;    /*!< difference between composition time of this sample
                         *   and the next sample, in msec                       */
  uint32 IsSyncFrame;   /*!< Indication if sample is a random access point
                         *   (I-Frame)(non-zero) or not (zero)                  */
} AEEDataFrameInfo;

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
typedef struct MDPVideo
{
  /*! MDP Image structures for the player regions */
  MDPIMG image;
  /*! MDP Image IDs */
  uint32 id;

  /*! For applications requesting frame callback */
  IBitmap* pBitmap;
  IDIB* pDIB;
  
  IBitmap* prev_pFrame;
} MDPVideo;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

#ifdef FEATURE_QTV_OEM_BUFFER_MGR
typedef struct
{
  q_link_type  qLink;
  void*        pOutputBuffer;
  int32        nRefCnt;
} OEMOutputBufferType;
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */
/*!
 * MPEG4 IMedia objects must contain a reference to this object.
 * Note: This class is a friend of IMedia and/or IMedia-based classes.
 */
typedef struct OEMMediaMPEG4Layer
{
  AEEMedia *        m_pMedia;           /*!< Owner IMedia object */
  void *            m_pAppContext;      /*!< App context */
#ifdef FEATURE_MP4_MSM6050
  OBJECTCONTEXT     m_cxtMedia;         /*!< Context returned by ObjectMgr */
#else /* FEATURE_MP4_MSM6050 */
  AEEObjectHandle   m_hObject;          /*!< Context returned by ObjectMgr */
#endif /* FEATURE_MP4_MSM6050 */
  PFNNOTIFY         m_pfnNotify;        /*!< IMedia notification function */

  AEEMediaCallback  m_cb[OEMMPEG4_MAX_CB];
  uint32            m_dwCaps;           /*!< Capabilities of IMedia */
  uint32            m_dwEnabledCaps;    /*!< Capabilities enabled by caller of IMedia */
  ISound *          m_pISound;
  OEMSoundType      m_Sound;
  AEEMediaCallback  m_cbCommand;
  boolean           m_bFrameCBEnabled;  /*!< Frame callbacks enabled? */
  boolean           m_bXscaleVideo;     /*!< XScaleVideo enabled? */
  boolean           m_bMDPScale;        /*!< MDP Scaling enabled? */
  IDisplay *        m_pIDisplay;        /*!< Pointer to display interface */
  AEERect           m_rectClip;         /*!< rectClip is the clipping region set
                                         * by the application.  We can only draw
                                         * in this area.
                                         */
  boolean           m_bRectClipChanged; /*!< Indicates m_rectClip has changed */
  AEERect           m_rectImage;        /*!< rectImage contains the x,y screen
                                         * coordinates for displaying the image.
                                         * It also contains the clipped
                                         * height,width.  By definition,
                                         * rectImage.dx,dy is <= frameInfo.cx,cy
                                         */
  AEEPoint          m_dimPrev;          /*!< Dimensions of previous frame. */
  void *            m_pHandle;          /*!< handle to the specific QtvPlayer instance */
  uint32            m_nPriority;        /*!< QtvPlayerObj & this object's priority */
  boolean           m_bTerminating;     /*!< Set to true before calling OEMMediaMPEG42PV_Terminate */

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  AEECallback *     m_gpcbISourceMediaData;     
#endif /* FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  uint8*            m_gpISourceStream;
  uint32            m_sourceBufferWritePtrOffset;
  boolean           m_bStreamOpened;
  int               m_nStreamOpenReturn;
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
  MDPVideo          m_Video;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

  /*! Current rotation/scaling transformation */
  OEMRotationType           m_QtvCurrentRotation;
  OEMScalingType            m_QtvCurrentScaling;
#if defined(FEATURE_QTV_MDP_ASCALE) || defined(FEATURE_QTV_XSCALE_VIDEO)
  AEEPoint                  m_QtvCurrentAScale;
#endif /* FEATURE_QTV_MDP_ASCALE || FEATURE_QTV_XSCALE_VIDEO*/
#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#endif /* FEATURE_QTV_IPL_SCALING */
#if (defined (FEATURE_MP4_AAC_PLUS) || defined (PLATFORM_LTK) )
  /*! This variable is used to pass the rotation factor to the QTV IPL rotation function */
  AEEMediaMPEG4RotationType m_IPLRotationFactor; 
#endif /* FEATURE_MP4_AAC_PLUS || PLATFORM_LTK */
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  AEE3GPPTimedText          m_TimedText3GPP;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  AEETelopTextElement       m_TelopTextElement;
  AEETelopTextSubString     m_TelopTextSubString;
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
#ifdef FEATURE_QTV_ISDB_SUBTITLES
#error code not present
#endif /* FEATURE_QTV_ISDB_SUBTITLES */
#if defined(FEATURE_ACM) || defined (FEATURE_BMP_ACM)
  //! For delayed acquiring case when IMedia_SetMediaDataEx is called, we need to save 
  //! the parameters. We will use the saved parameters to put the IMedia in READY state
  //! when the resource is acquired. The parameter will be freed later on.
  AEEMediaDataEx   *m_pmdExList;     
  int               m_nCount;
#endif // FEATURE_ACM || FEATURE_BMP_ACM
#if defined (FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  uint32            m_nPsdownloadrate;
  uint32            m_nPseudoBufferWritePtrOffset;
#endif
  void *            m_pCallbackData;
  uint32            m_nPrerollPercentage;
  int32             m_nStartTime;
  int32             m_nAccumulatedClipOffset;
  int32             m_nCurrentClipIndex;
  uint32            m_nTickInterval;

  void *            m_pClipInfo;
  AEEMediaMPEG4Spec m_MPEG4Spec;
  boolean           m_bClipInfoAvail;


#ifdef FEATURE_QTV_GENERIC_BCAST
  uint32            m_nBcastMutePending;
#endif /* FEATURE_QTV_GENERIC_BCAST */
  int32             m_nPlayFrameMode;
  boolean           m_cbGenerateTotalTime;
  boolean           m_bPBReadyPending;
  boolean           m_bSendTotalTime;               /*!<  This is to handle the case when STOP/PAUSE while a streaming 
                                                          clip is in connecting state */
  boolean           m_cbGenerateMediaSpec;
  boolean           m_bStreamingError;
  boolean           m_bRepositionInPause;
  boolean           m_bRepositionInPauseUpdateTime; /*!< Update time display  */
  boolean           m_bSkipInPause; /*!< Stores need to reposition upon resume */
  boolean           m_bPaused;      /*!< OEMLayer state variable */
  boolean           m_bPausePending; /*!< Asynchronous Pause cmd sent, not yet returned */
  boolean           m_bResumePending; /*!< Asynchronous Play cmd sent, not yet returned */
  boolean           m_bPlayStarted; /*!< TRUE from MM_STATUS_START to MM_STATUS_DONE */
  boolean           m_bSeekPending; /*!< Asynchronous FF/RW cmd sent, not yet returned */
  boolean           m_bSuspend; /*!< TRUE while the QtvPlayer is internally suspended.
                                     Release, Pause & Stop are the only external cmds
                                     processed while this flag is set. */
  boolean           m_bClkrgmIsReg;  /*!< Keep track of registering the clkrgm */
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  boolean           m_bFrameTransformPending;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
  uint32            m_nMPEG4EngineState;

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  AEEMediaDualMonoOutputType m_dualMonoOutput;
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */
  uint32            m_positionAtStop;
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
  q_type            m_qOutputBuffer;
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */
  boolean           m_bPauseInProcess;
  AEERTSPStatus     m_RTSPStatus;
  AEEUnhandledSDP_RTSP_Status m_SDPStatus;

  boolean                    m_bPULLDataMode;
  void*                      m_pClientData;
  AEEFetchBufferedDataSizeT  m_FetchBufferSizeCB;
  AEEFetchBufferedDataT      m_FetchBufferCB;

} OEMMediaMPEG4Layer;

typedef OEMMediaMPEG4Layer* OEMHandle;


/*===========================================================================

                      IMediaMPEG4 DECLARATIONS

===========================================================================*/

// Called by AEE_Init() during BREW initialization
void  IMediaMPEG4_Init(IShell * ps);
// Called by ISHELL_CreateInstance() during object instantiation
int   IMediaMPEG4_New(IShell * ps, AEECLSID cls, void **ppif);

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
int OEMMediaMPEG4_QueueCallback( void *pClientData,
                                 int nStatus,
                                 int nCmd,
                                 int nSubCmd,
                                 void *pData,
                                 uint32 dwSize );


void OEMMediaMPEG4_Conc_ACM_Stop(AEEMedia *pOEM);


#ifdef FEATURE_QTV_DRM_DCF
#include "AEESource.h"
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int OEMMediaMPEG42PV_OpenIxStreamFromISource(ISource*, OEMHandle pMPEG4);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* FEATURE_QTV_DRM_DCF */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
void UpdateProgressBar(void *pData);
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
void UpdateDownloadProgressBar(void *pData);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */


int OEMMediaMPEG42PV_GetClipInfo(OEMHandle pOEM);
int OEMMediaMPEG42PV_Init(OEMHandle pOEM);
int OEMMediaMPEG42PV_OpenURN(char *videoFileName, char *audioFileName,
                             char *textFileName, OEMHandle pOEM);
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
int OEMMediaMPEG42PV_OpenBufferURN(  unsigned char *pVideoBuf, uint32 dwVideoBufSize,
                                     unsigned char *pAudioBuf, uint32 dwAudioBufSize,
                                     OEMHandle pOEM );
int OEMMediaMPEG42PV_Pause(OEMHandle pOEM);
int OEMMediaMPEG42PV_Play(int, OEMHandle);
int OEMMediaMPEG42PV_RegisterForCallback(void *, OEMHandle);
int OEMMediaMPEG42PV_Seek(AEEMediaSeek eSeek, int32 TimeOrFrame, OEMHandle pOEM);
int OEMMediaMPEG42PV_Stop(OEMHandle);
int OEMMediaMPEG42PV_Terminate(OEMHandle);
int OEMMediaMPEG42PV_SetDevice(OEMHandle pOEM, void *pDevice);
int OEMMediaMPEG42PV_SetVolume(OEMHandle pOEM, void *pVolume);
int OEMMediaMPEG42PV_SetMuteCtl(OEMHandle pOEM, void *pbMute); 
int OEMMediaMPEG42PV_GetMuteCtl(OEMHandle pOEM, void *pbMute); 
int OEMMediaMPEG42PV_GetVolume(OEMHandle pOEM, void *);
int OEMMediaMPEG42PV_SetSoundMethod(OEMHandle pOEM);
int OEMMediaMPEG42PV_GetMediaSpec(AEEMediaMPEG4Spec **pSpec, int32 *pSize, OEMHandle pOEM);
int OEMMediaMPEG42PV_GetFrame(IBitmap** pb, AEEMediaExtFrameInfoHeader** pe,
                              OEMHandle pOEM);
int OEMMediaMPEG42PV_RotateVideo(AEEMediaMPEG4RotationType, OEMHandle);
int OEMMediaMPEG42PV_ScaleVideo(AEEMediaMPEG4ScalingType, uint16, uint16, OEMHandle);
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
int OEMMediaMPEG42PV_GetTelopSubString(AEETelopTextSubString **ppTextSubStr, int32 nIndex,
                                       OEMHandle pOEM);
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
int OEMMediaMPEG42PV_GetTrackTimeScale(AEEMediaMPEG4TrackType trackType, uint32 *pTimeScale);
int OEMMediaMPEG42PV_SetAudOverride(int32 audioFormat);
int OEMMediaMPEG42PV_SetStartPos(uint32 lTimeMS, OEMHandle pOEM);

int OEMMediaMPEG42PV_GetPlaybackPos(AEEMediaPlaybackPos *pPlayPos, OEMHandle pOEM);
int OEMMediaMPEG42PV_SetLoopTrack(boolean bLoop, OEMHandle pOEM);

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
int OEMMediaMPEG4Qtv_OpenPseudoStream(unsigned char *pBuf,uint32 bufSize, uint32 wPtrOffset,
                                      uint32 interfaceId, OEMHandle pOEM);
int OEMMediaMPEG4Qtv_UpdatePSBufferOffset(unsigned int offset);
#endif /* FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
int OEMMediaMPEG4Qtv_SetStartAndBufferingTime(uint32 startupTime, uint32 bufferingResumeTime, OEMHandle pOEM);
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

int OEMMediaMPEG42PV_SetMediaInfo(void *pMediaInfo, uint32 size, OEMHandle pOEM);

#ifdef FEATURE_QTV_STREAM_RECORD
int OEMMediaMPEG42PV_RecordStream(AEEStreamRecordInfo* recInfo, OEMHandle pOEM);
int OEMMediaMPEG42PV_StopStreamRecording(void);
#endif

#if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
int OEMMediaMPEG42Qtv_SetNetPolicyInfo(AEENetPolicyInfo*, OEMHandle pOEM);
#endif

#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
int OEMMediaMPEG42Qtv_SetDescramblerHandler(void *descrHandler, OEMHandle pOEM);
#endif

int OEMMediaMPEG42Qtv_SetUserAgent(char*);
int OEMMediaMPEG42Qtv_ISettings_Set(char *userString, const char *value, OEMHandle pOEM);


int OEMMediaMPEG42PV_SetRTSPStatusNotify(boolean notifyEnable, OEMHandle pOEM);

#ifdef FEATURE_QTV_SKT_MOD_MIDI
int OEMMediaMPEG42PV_GetMidiDataSize(uint32 *pSize);
int OEMMediaMPEG42PV_GetMidiData(uint8 *pBuf, uint32 *pSize);
int OEMMediaMPEG42PV_GetLinkDataSize(uint32 *pSize);
int OEMMediaMPEG42PV_GetLinkData(uint8 *pBuf, uint32 *pSize);
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

uint8 OEMMediaMPEG42PV_GetDataSize(int32 aType, uint32 *pSize);
uint8 OEMMediaMPEG42PV_GetData(int32 aType, uint8 *pBuf, uint32 *pSize);

int OEMMediaMPEG42PV_ModifyBufferLowMark(int32 modified_buffer_lowmark);

int  OEMMediaMPEG42PV_SetDataPortRange(int beginPort, int endPort);

void OEMMediaMPEG42PV_ReleaseCurrentVideoFrameBuffer(void *pBuffer);
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
void ReleaseOutputBuffer(const VDEC_STREAM_ID streamID, void *pCbData, void* pBuf);
#endif /* FEATUER_QTV_OEM_BUFFER_MGR */
int OEMMediaMPEG42PV_GetVideoCodec(AEEMediaMP4CodecType *pCodec, OEMHandle pOEM);
int OEMMediaMPEG42PV_GetAudioCodec(AEEMediaMP4CodecType *pCodec, OEMHandle pOEM);
int OEMMediaMPEG42PV_GetVideoFrameRate(float *pFrameRate, OEMHandle pOEM);
int OEMMediaMPEG42PV_GetVideoAverageBitRate(unsigned long *pBitRate, OEMHandle pOEM);
int OEMMediaMPEG42PV_GetAudioAverageBitRate(unsigned long *pBitRate, OEMHandle pOEM);
int OEMMediaMPEG42PV_GetTotalRxDataRate(uint32 *TotalRxDataRate);

#ifdef FEATURE_QTV_DIVX_DRM
int OEMMediaMPEG42PV_GetRegistrationCode(char *code,int* max_len, OEMHandle pOEM);
#endif

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
int OEMMediaMPEG42PV_CopyBufferToFile(char *fname, boolean ow);
int OEMMediaMPEG42PV_SetPvxSaveOption(char *dirPath);
int OEMMediaMPEG42PV_SetPvxPreroll(int preRoll);
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
int OEMMediaMPEG42PV_RegisterFetchDataSizeCallback(uint32 *);
int OEMMediaMPEG42PV_RegisterFetchDataCallback(uint32 *);
int OEMMediaMPEG42PV_RegisterIsMimeSupportedCallback(uint32 *);
int OEMMediaMPEG42PV_SetFileSaveOption(char *fileName);
int OEMMediaMPEG42PV_GetEncryptedData(uint32 *buffer, uint32 *dataSize);
int OEMMediaMPEG42PV_FreeDownloadBuffer(char *buffer);
OEMHandle OEMMediaMPEG42PV_GetOEMLayer(OEMHandle pOEM);
#ifdef FEATURE_QTV_GENERIC_BCAST
int OEMMediaMPEG42PV_MutePlaybackTracks(uint32, uint32, OEMHandle);
int OEMMediaMPEG42PV_SelectPlaybackTracks(uint32, AEETrackID *, OEMHandle);
int OEMMediaMPEG42PV_ReadPlaybackTracks(uint32 *, AEETrackList *, OEMHandle);
int OEMMediaMPEG42PV_PausedReposition(uint32, uint32 *, OEMHandle);
int OEMMediaMPEG42PV_FrameSeek(sint31, uint32 *, boolean, OEMHandle);
#endif /* FEATURE_QTV_GENERIC_BCAST */

int OEMMediaMPEG42PV_SetPlaybackSpeed(AEEMediaPlaybackSpeedType pbSpeed, OEMHandle pOEM);

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
int OEMMediaMPEG42PV_SetDualMonoOutput(OEMHandle pOEM);
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

void OEMMediaMPEG42PV_ClkrgmRelease(OEMHandle pOEM);
void OEMMediaMPEG42PV_ClkrgmIncrease(OEMHandle pOEM);
int OEMMediaMPEG42PV_RegisterDRMDecryptMethod(AEEMediaDRMDecryptMethodT pFunction, void *pData, OEMHandle pOEM);
int OEMMediaMPEG4Qtv_GetFrameRate(double* pFrameRate, OEMHandle pOEM);

void *AllocateOutputBuffer(const VDEC_STREAM_ID streamID,void *pCbData, 
                           uint32 bytes );
void  FreeOutputBuffer(const VDEC_STREAM_ID streamID,void *pCbData, void *pBuf);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif  // FEATURE_APP_MPEG4

#endif /* #ifndef OEMMPEG4FORMAT_H*/

#ifndef AEEMEDIAVIDEO_H
#define AEEMEDIAVIDEO_H
/*============================================================================
FILE:   AEEMediaVideo.h

SERVICES:  BREW MediaVideo Services

DESCRIPTION:
   This file defines the interface and specs for IMediaVideo which is used
   in conjunction with IMedia interface.

PUBLIC CLASSES:  IMediaVideo

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright ?2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/* =======================================================================
                             Edit History
$Header: //depot/asic/msmshared/services/Qtv/targets/6260/2.4/DynamicExtensions/IMediaVideo/AEEMediaVideo.h#1 $
$DateTime: 2007/02/05 12:49:38 $
$Change: 437403 $

========================================================================== */

/*------------------------------------------------------------------------
            Include Files
--------------------------------------------------------------------------*/
#include "AEEMedia.h"

#define AEEIID_MEDIAVIDEO      0x01038655

/*-------------------------------------------------------------------
      STATUS and PARM type Declarations
-------------------------------------------------------------------*/

typedef uint32 AEEMMVStatusType;
typedef uint32 AEEMMVSubStatusType;
typedef uint32 AEEMMVParmType;
typedef uint32 AEEMMVSubParamType;

// MMV_STATUS_XXX code is returned via registered callback function to
// indicate cmd status.
// AEEMediaNotify::nStatus may contain following status codes.
#define MMV_BASE_OFFSET                        (0x1000)
#define MMV_STATUS_BASE                        (MM_STATUS_USER_BASE + MMV_BASE_OFFSET)
// status base for derived class
#define MMV_STATUS_USER_BASE                   (MMV_STATUS_BASE + MMV_BASE_OFFSET)

#define MMV_STATUS_TIMED_TEXT                  (MMV_STATUS_BASE + 1)
#define MMV_STATUS_TELOP_TEXT                  (MMV_STATUS_BASE + 2)
#define MMV_STATUS_BUFFER_UPDATE               (MMV_STATUS_BASE + 3)
#define MMV_STATUS_CONNECTION_UPDATE           (MMV_STATUS_BASE + 4)
#define MMV_STATUS_DOWNLOAD_TICK_UPDATE        (MMV_STATUS_BASE + 5)
#define MMV_STATUS_PLAYBACK_READY              (MMV_STATUS_BASE + 6)
#define MMV_STATUS_OPEN_COMPLETE               (MMV_STATUS_BASE + 7)
#define MMV_STATUS_NO_SYNC_FRAME               (MMV_STATUS_BASE + 8)
#define MMV_STATUS_RESTARTING_VIDEO            (MMV_STATUS_BASE + 9)
#define MMV_STATUS_RESTARTING_AUDIO            (MMV_STATUS_BASE +10)
#define MMV_STATUS_RESTARTING_TEXT             (MMV_STATUS_BASE +11)
#define MMV_STATUS_VIDEO_END                   (MMV_STATUS_BASE +12)
#define MMV_STATUS_AUDIO_END                   (MMV_STATUS_BASE +13)
#define MMV_STATUS_TEXT_END                    (MMV_STATUS_BASE +14)
#define MMV_STATUS_RTSP                        (MMV_STATUS_BASE +15)
#define MMV_STATUS_UNHANDLED_SDP               (MMV_STATUS_BASE +16)
#define MMV_STATUS_UNHANDLED_RTSP              (MMV_STATUS_BASE +17)
#define MMV_STATUS_TS_LAYER_PRESENT            (MMV_STATUS_BASE +18)
#define MMV_STATUS_ERROR                       (MMV_STATUS_BASE +19)
#define MMV_STATUS_FILE_SAVING                 (MMV_STATUS_BASE +20)


// sub status type
#define MMV_SUB_STATUS_BASE                      (0)
// sub status type base for derived class
#define MMV_SUB_STATUS_USER_BASE                 (MMV_ERROR_STATUS_BASE + MMV_BASE_OFFSET)

// error type for MMV_STATUS_ERROR
#define MMV_ERROR_AUDIO                          (MMV_SUB_STATUS_BASE + 1)
#define MMV_ERROR_VIDEO                          (MMV_SUB_STATUS_BASE + 2)
#define MMV_ERROR_VIDEO_RESOLUTION_NOT_SUPPORTED (MMV_SUB_STATUS_BASE + 3)
#define MMV_ERROR_TEXT                           (MMV_SUB_STATUS_BASE + 4)

// sub status type of MMV_STATUS_FILE_SAVING
#define MMV_FILE_SAVING_COMPLETED                (MMV_SUB_STATUS_BASE + 1)
#define MMV_FILE_SAVING_DURATION                 (MMV_SUB_STATUS_BASE + 2)
#define MMV_FILE_SAVING_ALREADY_EXISTS           (MMV_SUB_STATUS_BASE + 3)
#define MMV_FILE_SAVING_OW_FAILURE               (MMV_SUB_STATUS_BASE + 4)
#define MMV_FILE_SAVING_RECORD_FAILED            (MMV_SUB_STATUS_BASE + 5)


// SubCmd codes
#define MMV_SUB_CMD_BASE                       (0)
// SubCmd base for derived class
#define MMV_SUB_CMD_USER_BASE                  (MMV_SUB_CMD_BASE + MMV_BASE_OFFSET)


// this section defines the additional IMedia MM_PARM_XXX types. If requested action/data
// is not currently available, the IMEDIA_GetMediaParm/IMEDIA_SetMediaParm
// will return MM_PENDING and the result will be sent back via callback.
#define MMV_PARM_BASE                          (MM_PARM_USER_BASE + MMV_BASE_OFFSET)
// parm base for derived class
#define MMV_PARM_USER_BASE                     (MMV_PARM_BASE + MMV_BASE_OFFSET)
                                              
#define MMV_PARM_ROTATION                      (MMV_PARM_BASE +  1)
#define MMV_PARM_SCALING                       (MMV_PARM_BASE +  2)
#define MMV_PARM_TRACK_TIME_SCALE              (MMV_PARM_BASE +  3)
#define MMV_PARM_PLAYBACK_VIDEO_POS            (MMV_PARM_BASE +  4)
#define MMV_PARM_PLAYBACK_AUDIO_POS            (MMV_PARM_BASE +  5)
#define MMV_PARM_PLAYBACK_TEXT_POS             (MMV_PARM_BASE +  6)
#define MMV_PARM_FILE_SAVE_START               (MMV_PARM_BASE +  7)
#define MMV_PARM_FILE_SAVE_STOP                (MMV_PARM_BASE +  8)
#define MMV_PARM_PREROLL                       (MMV_PARM_BASE +  9)
#define MMV_PARM_ATOM_SIZE                     (MMV_PARM_BASE + 10)
#define MMV_PARM_ATOM                          (MMV_PARM_BASE + 11)
#define MMV_PARM_SPEC_SIZE                     (MMV_PARM_BASE + 12)
#define MMV_PARM_SPEC                          (MMV_PARM_BASE + 13)
#define MMV_PARM_TRANSPORT                     (MMV_PARM_BASE + 14)
#define MMV_PARM_FRAME_DATA                    (MMV_PARM_BASE + 15)
#define MMV_PARM_LOOP_TRACK                    (MMV_PARM_BASE + 16)


// following section defines sub types of MMV_PARM_XXX
// sub type of MMV_PARM_PREROLL
#define MMV_PREROLL_BASE                       (0)
// preload base for derived class             
#define MMV_PREROLL_USER_BASE                  (MMV_PREROLL_BASE + MMV_BASE_OFFSET)
                                              
#define MMV_PREROLL_TIME                       (MMV_PREROLL_BASE + 1)
#define MMV_PREROLL_SIZE                       (MMV_PREROLL_BASE + 2)
#define MMV_PREROLL_REBUFFER_PREROLL_TIME      (MMV_PREROLL_BASE + 3)
#define MMV_PREROLL_REBUFFER_PREROLL_SIZE      (MMV_PREROLL_BASE + 4)
                                              
// sub type of MMV_PARM_ATOM                  
#define MMV_ATOM_BASE                          (0)
// atom base for derived class                
#define MMV_ATOM_USER_BASE                     (MMV_ATOM_BASE + MMV_BASE_OFFSET)
                                              
#define MMV_ATOM_MIDI                          (MMV_ATOM_BASE + 1)
#define MMV_ATOM_LINK                          (MMV_ATOM_BASE + 2)
#define MMV_ATOM_FTYP                          (MMV_ATOM_BASE + 3)
#define MMV_ATOM_DCMD                          (MMV_ATOM_BASE + 4)
#define MMV_ATOM_UDTA_CPRT                     (MMV_ATOM_BASE + 5)
#define MMV_ATOM_UDTA_AUTH                     (MMV_ATOM_BASE + 6)
#define MMV_ATOM_UDTA_TITL                     (MMV_ATOM_BASE + 7)
#define MMV_ATOM_UDTA_DSCP                     (MMV_ATOM_BASE + 8)
#define MMV_ATOM_UDTA_RTNG                     (MMV_ATOM_BASE + 9)
#define MMV_ATOM_UDTA_PERF                     (MMV_ATOM_BASE +10)
#define MMV_ATOM_UDTA_CLSF                     (MMV_ATOM_BASE +11)
#define MMV_ATOM_UDTA_GNRE                     (MMV_ATOM_BASE +12)
#define MMV_ATOM_UDTA_KYWD                     (MMV_ATOM_BASE +13)
#define MMV_ATOM_UDTA_LOCI                     (MMV_ATOM_BASE +14)
                                              
// sub type of MMV_PARM_SPEC                  
#define MMV_SPEC_BASE                          (0)
// spec base for derived class                
#define MMV_SPEC_USER_BASE                     (MMV_SPEC_BASE + MMV_BASE_OFFSET)
                                              
#define MMV_SPEC_DURATION                      (MMV_SPEC_BASE +  1)
#define MMV_SPEC_FILE_SIZE                     (MMV_SPEC_BASE +  2)
#define MMV_SPEC_STREAM_TYPE                   (MMV_SPEC_BASE +  3)
#define MMV_SPEC_AUDIO_CODEC_TYPE              (MMV_SPEC_BASE +  4)
#define MMV_SPEC_VIDEO_CODEC_TYPE              (MMV_SPEC_BASE +  5)
#define MMV_SPEC_AUDIO_BITRATE                 (MMV_SPEC_BASE +  6)
#define MMV_SPEC_VIDEO_BITRATE                 (MMV_SPEC_BASE +  7)
#define MMV_SPEC_VIDEO_FRAMERATE               (MMV_SPEC_BASE +  8)
#define MMV_SPEC_AUDIO_ONLY_FLAG               (MMV_SPEC_BASE +  9)
#define MMV_SPEC_VIDEO_ONLY_FLAG               (MMV_SPEC_BASE + 10)
#define MMV_SPEC_REPOSITION_FLAG               (MMV_SPEC_BASE + 11)
#define MMV_SPEC_VIDEO_FRAME_SIZE_WIDTH        (MMV_SPEC_BASE + 12)
#define MMV_SPEC_VIDEO_FRAME_SIZE_HEIGHT       (MMV_SPEC_BASE + 13)
#define MMV_SPEC_TEXT_TKHD_ORIGIN_X            (MMV_SPEC_BASE + 14)
#define MMV_SPEC_TEXT_TKHD_ORIGIN_Y            (MMV_SPEC_BASE + 15)
#define MMV_SPEC_TEXT_TKHD_WIDTH               (MMV_SPEC_BASE + 16)
#define MMV_SPEC_TEXT_TKHD_HEIGHT              (MMV_SPEC_BASE + 17)
#define MMV_SPEC_PURGE_DOWNLOAD_FILE           (MMV_SPEC_BASE + 18)
#define MMV_SPEC_ALLOW_USER_PLAYBACK           (MMV_SPEC_BASE + 19)
#define MMV_SPEC_DOWNLOAD_PLAYBACK_CONTROL     (MMV_SPEC_BASE + 20)
                                              
#define MMV_SPEC_TITLE_STRING                  (MMV_SPEC_BASE + 21)
#define MMV_SPEC_AUTHOR_STRING                 (MMV_SPEC_BASE + 22)
#define MMV_SPEC_COPYRIGHT_STRING              (MMV_SPEC_BASE + 23)
#define MMV_SPEC_RATING_STRING                 (MMV_SPEC_BASE + 24)
#define MMV_SPEC_DESCRIPTION_STRING            (MMV_SPEC_BASE + 25)
#define MMV_SPEC_OWNER_STRING                  (MMV_SPEC_BASE + 26)
#define MMV_SPEC_VERSION_STRING                (MMV_SPEC_BASE + 27)
#define MMV_SPEC_CREATIONDATE_STRING           (MMV_SPEC_BASE + 28)
#define MMV_SPEC_ARTIST_STRING                 (MMV_SPEC_BASE + 29)
#define MMV_SPEC_ALBUM_STRING                  (MMV_SPEC_BASE + 30)
#define MMV_SPEC_INFO_STRING                   (MMV_SPEC_BASE + 31)
#define MMV_SPEC_KEYWORD_STRING                (MMV_SPEC_BASE + 32)
                                              
#define MMV_SPEC_COPYPROHIBITION_FLAG          (MMV_SPEC_BASE + 33)
#define MMV_SPEC_VALIDITY_EFFECTIVE_DATE       (MMV_SPEC_BASE + 34)
#define MMV_SPEC_VALIDITY_PERIOD               (MMV_SPEC_BASE + 35)
#define MMV_SPEC_ALLOWED_PLAYBACKS             (MMV_SPEC_BASE + 36)
#define MMV_SPEC_GENERATION_DLL_VERSION        (MMV_SPEC_BASE + 37)
                                              
#define MMV_SPEC_EDIT_FLAG                     (MMV_SPEC_BASE + 38)
#define MMV_SPEC_RECORDING_MODE                (MMV_SPEC_BASE + 39)
#define MMV_SPEC_RECORDING_DATE                (MMV_SPEC_BASE + 40)
                                              
#define MMV_SPEC_DEVICE_NAME_STRING            (MMV_SPEC_BASE + 41)
#define MMV_SPEC_MODEL_NAME_STRING             (MMV_SPEC_BASE + 42)
#define MMV_SPEC_ENCODER_INFORMATION_STRING    (MMV_SPEC_BASE + 43)
#define MMV_SPEC_MUX_INFORMATION_STRING        (MMV_SPEC_BASE + 44)
                                              
#define MMV_SPEC_GPS_BYTE_ORDER                (MMV_SPEC_BASE + 45)
#define MMV_SPEC_GPS_VERSION_ID                (MMV_SPEC_BASE + 46)
#define MMV_SPEC_GPS_LATITUDE_REF              (MMV_SPEC_BASE + 47)
#define MMV_SPEC_GPS_LONGITUDE_REF             (MMV_SPEC_BASE + 48)
#define MMV_SPEC_GPS_ALTITUDE_REF              (MMV_SPEC_BASE + 49)
#define MMV_SPEC_GPS_LATITUDE_ARRAY            (MMV_SPEC_BASE + 50)
#define MMV_SPEC_GPS_LONGITUDE_ARRAY           (MMV_SPEC_BASE + 51)
#define MMV_SPEC_GPS_ALTITUDE                  (MMV_SPEC_BASE + 52)
#define MMV_SPEC_GPS_TIME_ARRAY                (MMV_SPEC_BASE + 53)
#define MMV_SPEC_GPS_MAP_SCALE_INFO            (MMV_SPEC_BASE + 54)
#define MMV_SPEC_GPS_SURVEY_DATA               (MMV_SPEC_BASE + 55)
#define MMV_SPEC_GPS_POSITIONING_METHOD        (MMV_SPEC_BASE + 56)
#define MMV_SPEC_GPS_POSITIONING_NAME          (MMV_SPEC_BASE + 57)
#define MMV_SPEC_GPS_DATE                      (MMV_SPEC_BASE + 58)

// sub type of MMV_PARM_TRANSPORT
#define MMV_TRANSPORT_BASE                     (0)
// sub type of MMV_PARM_TRANSPORT for derived class
#define MMV_TRANSPORT_USER_BASE                (MMV_TRANSPORT_BASE + MMV_BASE_OFFSET)

#define MMV_TRANSPORT_DATA_PORT_RANGE          (MMV_TRANSPORT_BASE + 1)
#define MMV_TRANSPORT_NET_POLICY               (MMV_TRANSPORT_BASE + 2)



/*-------------------------------------------------------------------
      type Declarations
-------------------------------------------------------------------*/

typedef uint32 MediaCAPType;
typedef uint32 MediaStreamType;
typedef uint32 MediaCodecType;
typedef uint32 MediaDownloadPlaybackCtlType;

#define MMV_MAX_3GPP_TEXT_COLOR                  4
#define MMV_MAX_3GPP_FONT_LEN                    32

// Additional MM_PARM_CAPS defines (MediaCodecType)
#define MMV_CAPS_INVALID                         0x00000100
#define MMV_CAPS_STILL_IMAGE                     0x00000200
#define MMV_CAPS_VIDEO_BCAST_FLO                 0x00000400

// Download Playback control defines (MediaDownloadPlaybackCtlType)
#define MMV_DPC_BASE                             (0)
#define MMV_DPC_USER_BASE                        (MMV_DPC_BASE + MMV_BASE_OFFSET)
#define MMV_DPC_NO_PLAY                          (MMV_DPC_BASE)
#define MMV_DPC_PLAY_AFTER_DOWNLOAD              (MMV_DPC_BASE + 1)
#define MMV_DPC_PLAY_ASAP                        (MMV_DPC_BASE + 2)

// media stream type defines (MediaStreamType)
#define MMV_STREAMING_TYPE_BASE                  (0)
// media stream type base id for derived class
#define MMV_STREAMING_TYPE_USER_BASE             (MMV_STREAMING_TYPE_BASE + MMV_BASE_OFFSET)

#define MMV_STREAMING_UNKNOWN                    (MMV_STREAMING_TYPE_BASE)
#define MMV_STREAMING_LOCAL                      (MMV_STREAMING_TYPE_BASE +  1)
#define MMV_STREAMING_HTTP                       (MMV_STREAMING_TYPE_BASE +  2)
#define MMV_STREAMING_RTSP                       (MMV_STREAMING_TYPE_BASE +  3)
#define MMV_STREAMING_LIVE                       (MMV_STREAMING_TYPE_BASE +  4)
#define MMV_STREAMING_MCAST                      (MMV_STREAMING_TYPE_BASE +  5)
#define MMV_STREAMING_BCAST_FLO                  (MMV_STREAMING_TYPE_BASE +  6)
#define MMV_STREAMING_BCAST_DMB                  (MMV_STREAMING_TYPE_BASE +  7)
#define MMV_STREAMING_PVX                        (MMV_STREAMING_TYPE_BASE +  8)

// media codec type defines (MediaCodecType)
#define MMV_CODEC_BASE                           (0)
// codec base id for derived class
#define MMV_CODEC_USER_BASE                      (MMV_CODEC_BASE + MMV_BASE_OFFSET)

#define MMV_CODEC_UNKNOWN                        (MMV_CODEC_BASE)
#define MMV_CODEC_NONE                           (MMV_CODEC_BASE + 1)

// media audio codec types
#define MMV_CODEC_AUDIO_BASE                     (MMV_CODEC_NONE)
#define MMV_CODEC_AUDIO_EVRC                     (MMV_CODEC_AUDIO_BASE +  1)
#define MMV_CODEC_AUDIO_QCELP                    (MMV_CODEC_AUDIO_BASE +  2)
#define MMV_CODEC_AUDIO_AAC                      (MMV_CODEC_AUDIO_BASE +  3)
#define MMV_CODEC_AUDIO_AAC_PLUS                 (MMV_CODEC_AUDIO_BASE +  4)
#define MMV_CODEC_AUDIO_ENH_AAC_PLUS             (MMV_CODEC_AUDIO_BASE +  5)
#define MMV_CODEC_AUDIO_GSM_AMR                  (MMV_CODEC_AUDIO_BASE +  6)
#define MMV_CODEC_AUDIO_MP3                      (MMV_CODEC_AUDIO_BASE +  7)
#define MMV_CODEC_AUDIO_WMA                      (MMV_CODEC_AUDIO_BASE +  8)
#define MMV_CODEC_AUDIO_REAL_COOK_G2_MONO        (MMV_CODEC_AUDIO_BASE +  9)
#define MMV_CODEC_AUDIO_REAL_COOK_G2_DUAL_MONO   (MMV_CODEC_AUDIO_BASE + 10)
#define MMV_CODEC_AUDIO_REAL_COOK_8_STEREO       (MMV_CODEC_AUDIO_BASE + 11)
#define MMV_CODEC_AUDIO_REAL_SIPRO               (MMV_CODEC_AUDIO_BASE + 12)

// media video codec types
#define MMV_CODEC_VIDEO_BASE                     (MMV_CODEC_NONE + 200)
#define MMV_CODEC_VIDEO_MPEG4                    (MMV_CODEC_VIDEO_BASE +  1)
#define MMV_CODEC_VIDEO_H263                     (MMV_CODEC_VIDEO_BASE +  2)
#define MMV_CODEC_VIDEO_H264                     (MMV_CODEC_VIDEO_BASE +  3)
#define MMV_CODEC_VIDEO_STILL_IMAGE              (MMV_CODEC_VIDEO_BASE +  4)
#define MMV_CODEC_VIDEO_TIMED_TEXT               (MMV_CODEC_VIDEO_BASE +  5)
#define MMV_CODEC_VIDEO_WMV1                     (MMV_CODEC_VIDEO_BASE +  6)
#define MMV_CODEC_VIDEO_WMV2                     (MMV_CODEC_VIDEO_BASE +  7)
#define MMV_CODEC_VIDEO_WMV3                     (MMV_CODEC_VIDEO_BASE +  8)
#define MMV_CODEC_VIDEO_REAL_VIDEO_8             (MMV_CODEC_VIDEO_BASE +  9)
#define MMV_CODEC_VIDEO_REAL_VIDEO_9             (MMV_CODEC_VIDEO_BASE + 10)
#define MMV_CODEC_VIDEO_FLO_OSCAR                (MMV_CODEC_VIDEO_BASE + 11)


/*-------------------------------------------------------------------
            Structure Type Declarations
-------------------------------------------------------------------*/
// This structure is used to send notifications data. It is pointed by
// pCmdData field of AEEMediaCmdNotify
typedef struct AEEMediaVideoData
{
  uint32      dwType;
  uint32      dwSize;
  void*       pData;
} AEEMediaVideoData;

//This structure is used to set the range of UDP ports
typedef struct AEEMediaVideoDataPort
{
  uint32      dwBeginPort;
  uint32      dwEndPort;
} AEEMediaVideoDataPort;

// this structure provides setup information for MMV_PARM_FILE_SAVE_START
typedef struct AEEMediaVideoFileSave
{
  char*    pszFileName;
  boolean  bOverWrite;
  boolean  bCopyBuffer;
  uint8    wMode;
  int32    nDuration;
} AEEMediaVideoFileSave;

// 3GPP timed text font info
typedef struct AEEMediaVideoTTFontInfo
{
  uint16    wID;
  char      Name[MMV_MAX_3GPP_FONT_LEN];
} AEEMediaVideoTTFontInfo;

// 3GPP timed text
typedef struct AEEMediaVideo3GPPTimedText
{
  uint32     dwTextSize;
  AECHAR    *pwszText;
  uint32     dwDuration;
  uint32     dwDisplayFlags;
  int32      nHorzJustification;
  int32      nVertJustification;
  int32      nBoxTop;
  int32      nBoxLeft;
  int32      nBoxBottom;
  int32      nBoxRight;
  uint32     dwStartChar;
  uint32     dwEndChar;
  uint32     dwFontID;
  uint32     dwFontStyleFlags;
  uint32     dwFontSize;
  uint8      BackgroundColorRGBA[MMV_MAX_3GPP_TEXT_COLOR];
  uint8      TextColorRGBA[MMV_MAX_3GPP_TEXT_COLOR];
  uint32     dwFontListSize;
  AEEMediaVideoTTFontInfo   *pFontList;
  uint32     dwSampleModifiersSize;
  uint8      *pSampleModifiersBuffer;
} AEEMediaVideo3GPPTimedText;

//This structure defines telop text substring
typedef struct AEEMediaVideoTelopTextSubString
{
  boolean     bLineFeed;
  boolean     bUnderline;
  boolean     bReversal;
  boolean     bLinking;
  int32       nFontColor;
  uint32      dwTextSampleSize;
  uint32      dwLinkSize;
  AECHAR     *pwszTextSubString;
  AECHAR     *pwszLinkValue;
} AEEMediaVideoTelopTextSubString;

// telop element structure
typedef struct AEEMediaVideoTelopTextElement
{
  uint32      dwTelopSize;
  int32       nBeginTime;
  int32       nDuration;
  boolean     bWrapFlag;
  int32       nBackgroundColor;
  int32       nDefaultFontColor;
  int32       nNumberOfSubStrings;
  AEEMediaVideoTelopTextSubString* pSubString;
} AEEMediaVideoTelopTextElement;

typedef struct AEEMediaVideoNetPolicyInfo
{
  uint32   dwSize;
  boolean  bQoSEnable;
  uint8    wPrimaryPDPProfileNo;
}AEEMediaVideoNetPolicyInfo;

/*===========================================================================
                    CLASS DEFINITIONS
===========================================================================*/
typedef struct _IMediaVideo         IMediaVideo;
QINTERFACE(IMediaVideo)
{
   INHERIT_IMedia(IMedia);
};

/*-------------------------------------------------------------------
      API Definitions
-------------------------------------------------------------------*/
// get current rotation value
static __inline int IMEDIAVIDEO_GetRotation(IMedia *pme, int32* pnValue)
{
  return AEEGETPVTBL(pme,IMedia)->GetMediaParm((IMedia*)pme, MMV_PARM_ROTATION, pnValue, 0);
}

// Rotate to the closest supported degree
static __inline int IMEDIAVIDEO_SetRotation(IMedia *pme, int32 nValue)
{
  return AEEGETPVTBL(pme,IMedia)->SetMediaParm((IMedia*)pme, MMV_PARM_ROTATION, nValue, (int32)0);
}

// Rotate to the closest supported degree
static __inline int IMEDIAVIDEO_SetRotationEx(IMedia *pme, int32 nValue, int32* pnValue)
{
  int nRet;
  if(SUCCESS == (nRet = IMEDIAVIDEO_SetRotation(pme, nValue))) 
  {
    if(pnValue)
      return IMEDIAVIDEO_GetRotation(pme, pnValue);
    return nRet;
  }
  else
    return nRet;
}

// Gets current media scalling values
static __inline int IMEDIAVIDEO_GetScaling(IMedia *pme, int32* x, int32* y)
{
  return AEEGETPVTBL(pme,IMedia)->GetMediaParm((IMedia*)pme, MMV_PARM_SCALING, x, y);
}

//set to the closet supported scalling values
static __inline int IMEDIAVIDEO_SetScaling(IMedia *pme, int32 x, int32 y)
{
  return AEEGETPVTBL(pme,IMedia)->SetMediaParm((IMedia*)pme, MMV_PARM_SCALING, x, y);
}

static __inline int IMEDIAVIDEO_SetScalingEx(IMedia *pme, int32 x, int32 y, int32* x1, int32* y1)
{
  int nRet;
  if(SUCCESS == (nRet = IMEDIAVIDEO_SetScaling(pme, x, y)))
  {
    if(x1 && y1)
      return IMEDIAVIDEO_GetScaling(pme, x1, y1);
    return nRet;
  }
  else
    return nRet;
}

// get clip time scale
static __inline int IMEDIAVIDEO_GetTimeScale(IMedia *pme, int32 nType, int32* pTimeScale)
{
  return AEEGETPVTBL(pme,IMedia)->GetMediaParm((IMedia*)pme, MMV_PARM_TRACK_TIME_SCALE, &nType, pTimeScale);
}

// get current video position
static __inline int IMEDIAVIDEO_GetVideoPos(IMedia *pme, int32* pPos)
{
  return AEEGETPVTBL(pme,IMedia)->GetMediaParm((IMedia*)pme, MMV_PARM_PLAYBACK_VIDEO_POS, pPos, 0);
}

// get current audio position
static __inline int IMEDIAVIDEO_GetAudioPos(IMedia *pme, int32* pPos)
{
  return AEEGETPVTBL(pme,IMedia)->GetMediaParm((IMedia*)pme, MMV_PARM_PLAYBACK_AUDIO_POS, pPos, 0);
}

// get current text position
static __inline int IMEDIAVIDEO_GetTextPos(IMedia *pme, int32* pPos)
{
  return AEEGETPVTBL(pme,IMedia)->GetMediaParm((IMedia*)pme, MMV_PARM_PLAYBACK_TEXT_POS, pPos, 0);
}

// start saving
static __inline int IMEDIAVIDEO_FileSaveStart(IMedia *pme, AEEMediaVideoFileSave* pData)
{
  return AEEGETPVTBL(pme,IMedia)->SetMediaParm((IMedia*)pme, MMV_PARM_FILE_SAVE_START, (int32)pData, 0);
}

// stop saving
static __inline int IMEDIAVIDEO_FileSaveStop(IMedia *pme)
{
  return AEEGETPVTBL(pme,IMedia)->SetMediaParm((IMedia*)pme, MMV_PARM_FILE_SAVE_STOP, 0, 0);
}

// set preroll settings
static __inline int IMEDIAVIDEO_SetPrerollData(IMedia *pme, int32 type, int32* pData)
{
  AEEMediaVideoData data;
  data.dwType = type;
  data.dwSize = 0;
  data.pData  = pData;

  return AEEGETPVTBL(pme,IMedia)->SetMediaParm((IMedia*)pme, MMV_PARM_PREROLL, (int32)&data, 0);
}

// get atom size
static __inline int IMEDIAVIDEO_GetAtomSize(IMedia *pme, int32 nType, int32* size)
{
  int nRet;
  AEEMediaVideoData data;
  data.dwType = nType;
  data.dwSize = 0;
  data.pData  = NULL;
  
  if(SUCCESS == (nRet = AEEGETPVTBL(pme,IMedia)->GetMediaParm(pme, MMV_PARM_ATOM, (int32*)&data, 0)))
  {
    *size = data.dwSize;
  }
  return nRet;
}

// get atom setting, need buffer and buffer size information
// the return of the size is the actual size copied
static __inline int IMEDIAVIDEO_GetAtom(IMedia *pme, int32 type, int32* pData, int32* size)
{
  int nRet;
  AEEMediaVideoData data;
  data.dwType = type;
  data.dwSize = *size;
  data.pData  = pData;
  
  if(SUCCESS == (nRet = AEEGETPVTBL(pme,IMedia)->GetMediaParm((IMedia*)pme, MMV_PARM_ATOM, (int32*)&data, 0)))
  {
    *size = data.dwSize;
  }
  return nRet;
}

// get media spec size
static __inline int IMEDIAVIDEO_GetSpecSize(IMedia *pme, int32 nType, int32* size)
{
  int nRet;
  AEEMediaVideoData data;
  data.dwType = nType;
  data.dwSize = 0;
  data.pData  = NULL;
  
  if(SUCCESS == (nRet = AEEGETPVTBL(pme,IMedia)->GetMediaParm(pme, MMV_PARM_SPEC, (int32*)&data, 0)))
  {
    *size = data.dwSize;
  }
  return nRet;
}

// get SPEC info,  need buffer and buffer size information
// the return of the size is the actual size copied
static __inline int IMEDIAVIDEO_GetSPEC(IMedia *pme, int32 type, int32* pData, int32* size)
{
  int nRet;
  AEEMediaVideoData data;
  data.dwType = type;
  data.dwSize = *size;
  data.pData  = pData;

  if(SUCCESS == (nRet = AEEGETPVTBL(pme,IMedia)->GetMediaParm((IMedia*)pme, MMV_PARM_SPEC, (int32*)&data, 0)))
  {
    *size = data.dwSize;
  }
  return nRet;
}

// get transport settings
static __inline int IMEDIAVIDEO_GetTransport(IMedia *pme, int32 type, int32* pData)
{
  int nRet;
  AEEMediaVideoData data;
  data.dwType = type;
  data.dwSize = 0;
  data.pData  = pData;

  nRet = AEEGETPVTBL(pme,IMedia)->GetMediaParm((IMedia*)pme, MMV_PARM_TRANSPORT, (int32*)&data, 0);
  return nRet;
}

// set transport data
static __inline int IMEDIAVIDEO_SetTransport(IMedia *pme, int32 type, int32* pData)
{
  int nRet;
  AEEMediaVideoData data;
  data.dwType = type;
  data.dwSize = 0;
  data.pData  = pData;
  
  nRet = AEEGETPVTBL(pme,IMedia)->SetMediaParm((IMedia*)pme, MMV_PARM_TRANSPORT, (int32)&data, 0);
  return nRet;
}

// get frame data
static __inline int IMEDIAVIDEO_GetFrame(IMedia *pme, int32 type, int32* pData, int32* size)
{
  int nRet;
  AEEMediaVideoData data;
  data.dwType = type;
  data.dwSize = *size;
  data.pData  = pData;

  if(SUCCESS == (nRet = AEEGETPVTBL(pme,IMedia)->GetMediaParm((IMedia*)pme, MMV_PARM_FRAME_DATA, (int32*)&data, 0)))
  {
    *size = data.dwSize;
  }
  return nRet;
}


    
/*=====================================================================
  INTERFACE DOCUMENTATION
=====================================================================
Interface Name: IMediaVideo

Description:
The interface is inherited from IMedia. It defines video-specific parameters 
to provide video playback and streaming features. 

The dynamic module of MediaVideo registers a MIME type with handler class IDs 
in the MIF file. To create an instance of IMediaVideo, AEEMediaUtil_CreateMedia() 
can be enhanced to return IMediaVideo-based interface by using AEEIID_MEDIAVIDEO 
as a base class for ISHELL_GetHandler().

An instance of IMediaVideo can also be created using the ISHELL_CreateInstance 
method with the abstract media video interface ID AEEIID_MEDIAVIDEO or the handler 
class IDs.

For details on IMedia interface, refer to IMedia interface description section.

     
=====================================================================
AEEMMVStatusType

Description:
  The following MMV_STATUS_xxx instances indicate event status:

  MMV_STATUS_TIMED_TEXT : Timed text is ready to display
  MMV_STATUS_TELOP_TEXT : Telop Text is ready to display
  MMV_STATUS_BUFFER_UPDATE : Data is buffering
  MMV_STATUS_CONNECTION_UPDATE : Streaming setup is in progress
  MMV_STATUS_DOWNLOAD_TICK_UPDATE : Download progress update
  MMV_STATUS_PLAYBACK_READY : Indicates moving to playback ready state
  MMV_STATUS_OPEN_COMPLETE : Open file complete
  MMV_STATUS_NO_SYNC_FRAME : No sync frame presents in media to complete re-positioning
  MMV_STATUS_RESTARTING_VIDEO : In loop track mode, video track is restarting
  MMV_STATUS_RESTARTING_AUDIO : In loop track mode, audio track is restarting
  MMV_STATUS_RESTARTING_TEXT : In loop track mode, restarting text in loop mode
  MMV_STATUS_VIDEO_END : Video track ends
  MMV_STATUS_AUDIO_END : Audio track ends
  MMV_STATUS_TEXT_END : Text track ends
  MMV_STATUS_RTSP : RTSP status, pCmdData contains RTSP status string
  MMV_STATUS_UNHANDLED_SDP : Unhandled SDP header line has been encountered
  MMV_STATUS_UNHANDLED_RTSP : Unhandled RTSP header line has been encountered
  MMV_STATUS_TS_LAYER_PRESENT : Temporal scalability layer present in clip
  MMV_STATUS_ERROR : Error encountered when replaying
  MMV_STATUS_FILE_SAVING : status for saving file

Comments:
 none

See Also:
  AEEMediaCmdNotify
  AEEMMVSubStatusType
  AEEMediaVideoData

=============================================================================
AEEMMVSubStatusType

Description:
  The sub status type expresses the detail information on a returned event status message,

MMV_STATUS_ERROR
  MMV_ERROR_AUDIO : Error in audio track
  MMV_ERROR_VIDEO : Error in video track
  MMV_ERROR_VIDEO_RESOLUTION_NOT_SUPPORTED : Video resolution not supported
  MMV_ERROR_TEXT: Error in text track
  
MMV_STATUS_FILE_SAVING
  MMV_FILE_SAVING_COMPLETED : Stream record completed
  MMV_FILE_SAVING_DURATION : Recorded clip duration
  MMV_FILE_SAVING_ALREADY_EXISTS :  File already exists, Cannot overwrite
  MMV_FILE_SAVING_OW_FAILURE : Failed to overwrite a file
  MMV_FILE_SAVING_RECORD_FAILED : Stream recorder failed


Comments:

The following table lists the possible events that contain commands and sub-commands,
status, and context-sensitive data. Data field should be evaluated before using.

===pre>
  nCmd               nSubCmd       nStatus                       pData [optional], comments
-----------------------------------------------------------------------------------------------
MM_CMD_SETMEDIAPARM  MM_PARM_xxx   MM_STATUS_DONE                <Depends on parm>
                                   MM_STATUS_ABORT

MM_CMD_GETMEDIAPARM  MM_PARM_xxx   MM_STATUS_DONE                <Depends on parm>
                                   MM_STATUS_ABORT

MM_CMD_PLAY          0             MM_STATUS_START               NULL, start playing
                                   MM_STATUS_DONE                NULL, stop playing
                                   MM_STATUS_ABORT               NULL or abort type
                                   MM_STATUS_MEDIA_SPEC          TRUE, if media spec is availiable. otherwise, FALSE
                                   MM_STATUS_TICK_UPDATE         Time in MS
                                   MM_STATUS_DATA_IO_DELAY       Elapsed Time in MS, (e.g. stream buffering)
                                   MM_STATUS_SEEK                
                                   MM_STATUS_SEEK_FAIL
                                   MM_STATUS_PAUSE               Elapsed Time in MS
                                   MM_STATUS_PAUSE_FAIL
                                   MM_STATUS_RESUME              Elapsed Time in MS
                                   MM_STATUS_RESUME_FAIL
                                   MM_STATUS_REPEAT              Elapsed Time in MS
                                   MM_STATUS_FRAME

                                   MMV_STATUS_TIMED_TEXT         AEEMediaVideo3GPPTimedText
                                   MMV_STATUS_TELOP_TEXT         AEEMediaVideoTelopTextElement
                                   MMV_STATUS_BUFFER_UPDATE      preroll %
                                   MMV_STATUS_CONNECTION_UPDATE  NULL
                                   MMV_STATUS_DOWNLOAD_TICK_UPDATE downloaded time in MS
                                   MMV_STATUS_PLAYBACK_READY     NULL
                                   MMV_STATUS_PB_READY           NULL
                                   MMV_STATUS_NO_SYNC_FRAME      NULL
                                   MMV_STATUS_RESTARTING_VIDEO   NULL
                                   MMV_STATUS_RESTARTING_AUDIO   NULL
                                   MMV_STATUS_RESTARTING_TEXT    NULL
                                   MMV_STATUS_VIDEO_END          NULL
                                   MMV_STATUS_AUDIO_END          NULL
                                   MMV_STATUS_TEXT_END           NULL
                                   MMV_STATUS_RTSP               AEEMediaVideoData
                                   MMV_STATUS_UNHANDLED_SDP      AEEMediaVideoData
                                   MMV_STATUS_UNHANDLED_RTSP     AEEMediaVideoData
                                   MMV_STATUS_TS_LAYER_PRESENT   NULL
                                   MMV_STATUS_FILE_SAVING        AEEMediaVideoData
                                  
                                   MMV_STATUS_ERROR              MMV_ERROR_AUDIO
                                   MMV_STATUS_ERROR              MMV_ERROR_VIDEO
                                   MMV_STATUS_ERROR              MMV_ERROR_VIDEO_RESOLUTION_NOT_SUPPORTED
                                   MMV_STATUS_ERROR              MMV_ERROR_TEXT

                                 
===/pre>

See Also:
  AEEMediaCmdNotify
  AEEMediaVideoData
  AEEMMVStatusType

=============================================================================
AEEMMVParmType

Description:
This section defines the additional IMedia parameters. These allow getting/setting
of media data. Some parameters have sub param types, which will be carried by
AEEMediaVideoData. When requested data is not currently available, MM_PENDING
will be returned by IMEDIA_GetMediaParm(), and the result will be sent back
via callback with following parameters

    nstatus = MM_STATUS_DONE/MM_STATUS_ABORT
    nCmd    = MM_CMD_SETMEDIAPARM/MM_CMD_GETMEDIAPARM
    nSubCmd = param type


MMV_PARM_ROTATION
Set:  Rotate clip clockwise
      p1 = int32,  requested rotation value in degrees
Get:  Gets the current rotation value
      p1 = int32*, current rotation in degrees

Comments:
If the requested value is not supported, the next available one will be used.

MMV_PARM_SCALING
Set: set scaling factors for width and height
     p1 = int32,  requested scaling value for width in %, original size = 100
     p2 = int32,  requested scaling value for height in %, original size = 100
Get: get current width and height scaling factors
     p1 = int32*, width value in %
     p2 = int32*, height value in %

Comments:
If the requested value is not supported, the next available one will be used.
When zooming in, the next value will be the one larger, otherwise, it will be the one smaller.

MMV_PARM_TRACK_TIME_SCALE
Get: Gets the timescale for the specified media track
     p1 = int32, media track type
     p2 = int32*, (time scale)

Comments:
 The media track type can be one of the media MM_CAPS_xxx.

MMV_PARM_PLAYBACK_VIDEO_POS
Get : get video position
     p1 = int32*, position in ms

MMV_PARM_PLAYBACK_AUDIO_POS
Get : get audio position
     p1 = int32*, position in ms

MMV_PARM_PLAYBACK_TEXT_POS
Get : get text position
     p1 = int32* , position in ms

MMV_PARM_FILE_SAVE_START
Set: start saving/recording file
     p1 = int32*, point to AEEMediaVideoFileSave

MMV_PARM_FILE_SAVE_STOP
Set: stop saving file

MMV_PARM_PREROLL
Set: start up time and buffering resume time. This should be set before streaming.
    p1 = int32*, point to AEEMediaVideoData

MMV_PARM_ATOM_SIZE
Get: get atom size.
    p1 = int32*, point to AEEMediaVideoData

MMV_PARM_ATOM
Get: get atom information. User needs to allocate buffer for string type of atom data
    p1 = int32*, point to AEEMediaVideoData

MMV_PARM_SPEC_SIZE
Get: get spec data size.
    p1 = int32*, point to AEEMediaVideoData
    
MMV_PARM_SPEC
Get: media spec information. User needs to allocate buffer for string type of data. 
     A status callback will be generated if the spec is not currently avaliable.
    p1 = int32*, point to AEEMediaVideoData

MMV_PARM_TRANSPORT
Get/Set transport layer data
    p1 = int32*, point to AEEMediaVideoData

MMV_PARM_FRAME_DATA
Get: get frame data
    p1 = int32*, point to AEEMediaVideoData


See Also:
  IMEDIA_SetMediaParm
  IMEDIA_GetMediaParm
  AEEMediaVideoFileSave
  AEEMediaVideoDataPort
  AEEMediaVideoData
  AEEMMVStatusType

===========================================================================================
MediaCAPType

Description:
 This section defines additional MM_PARM_CAPS.

  MMV_CAPS_INVALID
  MMV_CAPS_STILL_IMAGE
  MMV_CAPS_VIDEO_BCAST_FLO

Comments:
  The MediaCAPType types are also media track types.

See Also:
  IMEDIA_GetMediaParm()
  AEEMediaVideoData

===========================================================================================
MediaDownloadPlaybackCtlType

Description:
  Download playback control type indicates the playback action for a given clip (e.g. FastTrack PVX).
  It can be one of the following types:

  MMV_DPC_NO_PLAY, No playback after downloading
  MMV_DPC_PLAY_AFTER_DOWNLOAD, Start playback after finishing download
  MMV_DPC_PLAY_ASAP, Start playback as soon as preroll data of the file has been downloaded
  MMV_DPC_PLAY_RESERVE, Reserved

Comments:
  Download Playback control type can be retrieved by calling IMEDIA_GetMediaParm() with MMV_PARM_SPEC
and MMV_SPEC_DOWNLOAD_PLAYBACK_CONTROL.

See Also:
  IMEDIA_GetMediaParm()
  IMEDIAVIDEO_GetSPEC()

===========================================================================================
MediaStreamType

Description:
 The following values define media stream types:

  MMV_STREAMING_UNKNOWN
  MMV_STREAMING_LOCAL
  MMV_STREAMING_HTTP
  MMV_STREAMING_RTSP
  MMV_STREAMING_LIVE
  MMV_STREAMING_MCAST
  MMV_STREAMING_BCAST_FLO
  MMV_STREAMING_BCAST_DMB
  MMV_STREAMING_PVX

Comments:
  The media stream type can be retrieved by calling IMEDIA_GetMediaParm() with MMV_PARM_SPEC
and MMV_SPEC_STREAM_TYPE.

See Also:
  IMEDIA_GetMediaParm()
  AEEMediaVideoData

===========================================================================================
MediaCodecType

Description:
 The following values define media audio and video codec types

 unknown and none codec:
  MMV_CODEC_UNKNOWN
  MMV_CODEC_NONE

 audio codec:
  MMV_CODEC_AUDIO_EVRC
  MMV_CODEC_AUDIO_QCELP
  MMV_CODEC_AUDIO_AAC
  MMV_CODEC_AUDIO_AAC_PLUS
  MMV_CODEC_AUDIO_ENH_AAC_PLUS
  MMV_CODEC_AUDIO_GSM_AMR
  MMV_CODEC_AUDIO_MP3
  MMV_CODEC_AUDIO_WMA
  MMV_CODEC_AUDIO_REAL_COOK_G2_MONO
  MMV_CODEC_AUDIO_REAL_COOK_G2_DUAL_MONO
  MMV_CODEC_AUDIO_REAL_COOK_8_STEREO
  MMV_CODEC_AUDIO_REAL_SIPRO

 video codec:
  MMV_CODEC_VIDEO_MPEG4
  MMV_CODEC_VIDEO_H263
  MMV_CODEC_VIDEO_H264
  MMV_CODEC_VIDEO_STILL_IMAGE
  MMV_CODEC_VIDEO_TIMED_TEXT
  MMV_CODEC_VIDEO_WMV1
  MMV_CODEC_VIDEO_WMV2
  MMV_CODEC_VIDEO_WMV3
  MMV_CODEC_VIDEO_REAL_VIDEO_8
  MMV_CODEC_VIDEO_REAL_VIDEO_9
  MMV_CODEC_VIDEO_FLO_OSCAR


Comments:
  The media codec type can be retrieved by calling IMEDIA_GetMediaParm() with MMV_PARM_SPEC
and MMV_SPEC_AUDIO_CODEC_TYPE/MMV_SPEC_VIDEO_CODEC_TYPE.

See Also:
  IMEDIA_SetMediaParm()
  IMEDIA_GetMediaParm()
  AEEMediaVideoData

=============================================================================================
AEEMediaVideoData

Description:
This structure contains media data.

Definition:
typedef struct AEEMediaVideoData
{
  uint32      dwType;
  uint32      dwSize;
  void*       pData;
} AEEMediaVideoData;

Members:
  dwType:  Media data type
  dwSize:  Data size in bytes
  pData:   Var length data

Comments:
  This struct is used to send notifications data. It is pointed by the pCmdData field of
  AEEMediaCmdNotify; dwType gives the status info type. This struct can also be used as a data field
  when calling to IMEDIA_SetMediaParm/IMEDIA_GetMediaParm; dwType indicates the sub parameter type.

  The following table lists other possible parameters for IMEDIA_SetMediaParm() of IMedia object
===pre>
  MMV_STATUS_xxx            dwType                             dwSize           pData
-------------------------------------------------------------------------------------------
  MMV_STATUS_RTSP           rtsp status code                                    string buffer
  MMV_STATUS_UNHANDLED_RTSP NULL                                                string buffer
  MMV_STATUS_UNHANDLED_SDP  NULL                                                string buffer
  MMV_STATUS_FILE_SAVING    MMV_FILE_SAVING_ALREADY_EXISTS                      NULL
                            MMV_FILE_SAVING_OW_FAILURE                          NULL
                            MMV_FILE_SAVING_RECORD_FAILED                       NULL
                            MMV_FILE_SAVING_COMPLETED                           NULL
                            MMV_FILE_SAVING_DURATION                            time in ms
                                                                             
                                                                             
  MMV_PARM_xxx               dwType                            dwSize           pData
--------------------------------------------------------------------------------------------
  MMV_PARM_PREROLL         MMV_PREROLL_TIME                                     time in ms
                           MMV_PREROLL_SIZE                                     size in byte
                           MMV_PREROLL_REBUFFER_PREROLL_TIME                    time in ms
                           MMV_PREROLL_REBUFFER_PREROLL_SIZE                    size in byte
                                                                            
  MMV_PARM_ATOM_SIZE       MMV_ATOM_MIDI                      atom size         NULL
                           MMV_ATOM_LINK                      atom size         NULL
                           MMV_ATOM_FTYP                      atom size         NULL
                           MMV_ATOM_DCMD                      atom size         NULL
                           MMV_ATOM_UDTA_CPRT                 atom size         NULL
                           MMV_ATOM_UDTA_AUTH                 atom size         NULL
                           MMV_ATOM_UDTA_TITL                 atom size         NULL
                           MMV_ATOM_UDTA_DSCP                 atom size         NULL
                           MMV_ATOM_UDTA_RTNG                 atom size         NULL
                           MMV_ATOM_UDTA_PERF                 atom size         NULL
                           MMV_ATOM_UDTA_CLSF                 atom size         NULL
                           MMV_ATOM_UDTA_GNRE                 atom size         NULL
                           MMV_ATOM_UDTA_KYWD                 atom size         NULL
                           MMV_ATOM_UDTA_LOCI                 atom size         NULL
                                                                             
  MMV_PARM_ATOM            MMV_ATOM_MIDI                      atom size         data buffer
                           MMV_ATOM_LINK                      atom size         data buffer
                           MMV_ATOM_FTYP                      atom size         data buffer
                           MMV_ATOM_DCMD                      atom size         data buffer
                           MMV_ATOM_UDTA_CPRT                 atom size         data buffer
                           MMV_ATOM_UDTA_AUTH                 atom size         data buffer
                           MMV_ATOM_UDTA_TITL                 atom size         data buffer
                           MMV_ATOM_UDTA_DSCP                 atom size         data buffer
                           MMV_ATOM_UDTA_RTNG                 atom size         data buffer
                           MMV_ATOM_UDTA_PERF                 atom size         data buffer
                           MMV_ATOM_UDTA_CLSF                 atom size         data buffer
                           MMV_ATOM_UDTA_GNRE                 atom size         data buffer
                           MMV_ATOM_UDTA_KYWD                 atom size         data buffer
                           MMV_ATOM_UDTA_LOCI                 atom size         data buffer
                                                                            
  MMV_PARM_SPEC            MMV_SPEC_DURATION                                    time in ms
                           MMV_SPEC_FILE_SIZE                                   size in byte
                           MMV_SPEC_STREAM_TYPE                                 media stream type
                           MMV_SPEC_AUDIO_CODEC_TYPE                            media audio codec type
                           MMV_SPEC_VIDEO_CODEC_TYPE                            media audio video type
                           MMV_SPEC_AUDIO_BITRATE                               audio bitrate
                           MMV_SPEC_VIDEO_BITRATE                               video bitrate
                           MMV_SPEC_VIDEO_FRAMERATE                             frame rate; pointer to double 
                                                                                type data
                           MMV_SPEC_AUDIO_ONLY_FLAG                             TRUE; audio-only playback is
                                                                                allowed; otherwise, FALSE
                           MMV_SPEC_VIDEO_ONLY_FLAG                             TRUE; video-only playback is
                                                                                allowed; otherwise, FALSE
                           MMV_SPEC_REPOSITION_FLAG                             TRUE; reposition is allowed,
                                                                                otherwise, FALSE
                           MMV_SPEC_VIDEO_FRAME_SIZE_WIDTH                      width in pixels
                           MMV_SPEC_VIDEO_FRAME_SIZE_HEIGHT                     height in pixels
                           MMV_SPEC_TEXT_TKHD_ORIGIN_X                          x position
                           MMV_SPEC_TEXT_TKHD_ORIGIN_Y                          y position
                           MMV_SPEC_TEXT_TKHD_WIDTH                             width
                           MMV_SPEC_TEXT_TKHD_HEIGHT                            height
                           MMV_SPEC_PURGE_DOWNLOAD_FILE                         TRUE or FALSE
                           MMV_SPEC_ALLOW_USER_PLAYBACK                         TRUE or FALSE
                           MMV_SPEC_DOWNLOAD_PLAYBACK_CONTROL                   MediaDownloadPlaybackCtlType
                                                                            
                           MMV_SPEC_TITLE_STRING              number of AECHAR  AECHAR string buffer
                           MMV_SPEC_AUTHOR_STRING             number of AECHAR  AECHAR string buffer
                           MMV_SPEC_COPYRIGHT_STRING          number of AECHAR  AECHAR string buffer
                           MMV_SPEC_RATING_STRING             number of AECHAR  AECHAR string buffer
                           MMV_SPEC_DESCRIPTION_STRING        number of AECHAR  AECHAR string buffer
                           MMV_SPEC_OWNER_STRING              number of AECHAR  AECHAR string buffer
                           MMV_SPEC_VERSION_STRING            number of AECHAR  AECHAR string buffer
                           MMV_SPEC_CREATIONDATE_STRING       number of AECHAR  AECHAR string buffer
                           MMV_SPEC_ARTIST_STRING             number of AECHAR  AECHAR string buffer
                           MMV_SPEC_ALBUM_STRING              number of AECHAR  AECHAR string buffer
                           MMV_SPEC_INFO_STRING               number of AECHAR  AECHAR string buffer
                           MMV_SPEC_KEYWORD_STRING            number of AECHAR  AECHAR string buffer
               
                           MMV_SPEC_COPYPROHIBITION_FLAG                        TRUE; copy is allowed;
                                                                                otherwise, FALSE
                           MMV_SPEC_VALIDITY_EFFECTIVE_DATE                     start date value
                           MMV_SPEC_VALIDITY_PERIOD                             validity period value
                           MMV_SPEC_ALLOWED_PLAYBACKS                           number of allowed playbacks
                           MMV_SPEC_GENERATION_DLL_VERSION                      DLL version value
                                                                         
                           MMV_SPEC_EDIT_FLAG                                   uint32, media edit flag
                           MMV_SPEC_RECORDING_MODE                              uint8, recording mode
                           MMV_SPEC_RECORDING_DATE                              uint32, recording date
                                                                         
                           MMV_SPEC_DEVICE_NAME_STRING         number of AECHAR AECHAR string buffer
                           MMV_SPEC_MODEL_NAME_STRING          number of AECHAR AECHAR string buffer
                           MMV_SPEC_ENCODER_INFORMATION_STRING number of AECHAR AECHAR string buffer
                           MMV_SPEC_MUX_INFORMATION_STRING     number of AECHAR AECHAR string buffer
                                                                         
                           MMV_SPEC_GPS_BYTE_ORDER                              GPS byte order
                           MMV_SPEC_GPS_VERSION_ID                              version ID
                           MMV_SPEC_GPS_LATITUDE_REF                            latitude value
                           MMV_SPEC_GPS_LONGITUDE_REF                           longitude value
                           MMV_SPEC_GPS_ALTITUDE_REF                            altitude value
                           MMV_SPEC_GPS_LATITUDE_ARRAY         number of uint64 point to uint64 data buffer
                           MMV_SPEC_GPS_LONGITUDE_ARRAY        number of uint64 point to uint64 data buffer
                           MMV_SPEC_GPS_ALTITUDE                                point to uint64 data
                                                                           
                           MMV_SPEC_GPS_TIME_ARRAY             number of uint64 point to uint64 data buffer
                           MMV_SPEC_GPS_MAP_SCALE_INFO                          point to uint64 data
                           MMV_SPEC_GPS_SURVEY_DATA                             GPS survey data string buffer
                                                                           
                           MMV_SPEC_GPS_POSITIONING_METHOD    number of AECHAR  GPS positioning method string 
                                                                                buffer
                           MMV_SPEC_GPS_POSITIONING_NAME      number of AECHAR  GPS positioning name string 
                                                                                buffer
                           MMV_SPEC_GPS_DATE                  number of AECHAR  GPS date string
                                                                           
  MMV_PARM_TRANSPORT       MMV_TRANSPORT_DATA_PORT_RANGE                        point to AEEMediaVideoDataPort
                           MMV_TRANSPORT_NET_POLICY                             point to AEEMediaVideoNetPolicyInfo
                                                                           
  MMV_PARM_FRAME_DATA      frame type                        size of buffer     data buffer ptr

===pre>



See Also:
  AEEMediaCmdNotify
  IMEDIA_SetMediaParm
  IMEDIA_GetMediaParm
  AEEMMVStatusType;
  AEEMMVSubStatusType;
  AEEMMVParmType;
  AEEMMVSubParamType;

=============================================================================================
AEEMediaVideoFileSave

Description:
This structure is passed as p1 when calling MMV_PARM_FILE_SAVE_START.

typedef struct AEEMediaVideoFileSave
{
  char*    pszFileName;
  boolean  bOverWrite;
  boolean  bCopyBuffer;
  uint8    wMode;
  int32    nDuration;
} AEEMediaVideoFileSave;

Members:
  pszFileName: Pointer to recorded file name
  bOverWrite:  TRUE if allow overwrite, otherwise FALSE
  bCopyBuffer: If this is set, nMode will be ignored. Recorder will perform buffer copy operation
  wMode:       Record mode, 0xFF to ignore.
  nDuration:   Record duration

Comments:

See Also:
  IMEDIA_SetMediaParm
  MMV_PARM_FILE_SAVE_START
  MMV_PARM_FILE_SAVE_STOP

=============================================================================================
AEEMediaVideoTTFontInfo

Description:
This is the font information structure that stores the name of the font and associated ID
used for storing font information in an array

Definition:
  typedef struct AEEMediaVideoTTFontInfo
  {
    uint16    wID;
    char      Name[MMV_MAX_FONT_LEN];

  } AEEMediaVideoTTFontInfo ;

Members:
  wID:   ID of the font for the given name
  Name:  A char array representing the name of the font

Comments:

See Also:
  AEEMediaVideo3GPPTimedText
  AEEMMVStatusType

=============================================================================================
AEEMediaVideo3GPPTimedText

Description:
This structure is used to hold all the information needed for displaying and keeping track of a
timed text area. The timed text is sent back via the pCmdData field of the notification on
MMV_STATUS_TIMED_TEXT event.

Definition:
typedef struct AEEMediaVideo3GPPTimedText
{
  uint32     dwTextSize;
  AECHAR    *pwszText;
  uint32     dwDuration;
  uint32     dwDisplayFlags;
  int32      nHorzJustification;
  int32      nVertJustification;
  int32      nBoxTop;
  int32      nBoxLeft;
  int32      nBoxBottom;
  int32      nBoxRight;
  uint32     dwStartChar;
  uint32     dwEndChar;
  uint32     dwFontID;
  uint32     dwFontStyleFlags;
  uint32     dwFontSize;
  uint8      BackgroundColorRGBA[MMV_MAX_3GPP_TEXT_COLOR];
  uint8      TextColorRGBA[MMV_MAX_3GPP_TEXT_COLOR];
  uint32     dwFontListSize;
  AEEMediaVideoTTFontInfo   *pFontList;
  uint32     dwSampleModifiersSize;
  uint8      *pSampleModifiersBuffer;
} AEEMediaVideo3GPPTimedText;

Members:
  dwTextSize:             Size of text string buffer
  pwszText:               Pointer to a text string that is displayed
  dwDuration:             The duration which the text is displayed for in ms
  dwDisplayFlags:         Flags to modify how text displayed
  nHorzJustification:     Horizontal justification
  nVertJustification:     Vertical Justification
  nBoxTop:                Top edge of text box
  nBoxLeft:               Left edge of text box
  nBoxBottom:             Bottom edge of text box
  nBoxRight:              Right edge of text box
  dwStartChar:            First character index from szText
  dwEndChar:              Last character index from szText
  dwFontID:               ID of the font to use for text
  dwFontStyleFlags:       Style of font, bold, italics, etc.
  dwFontSize:             Size of the font to use
  BackgroundColorRGBA:    Array containing the background color information
  TextColorRGBA:          Array containing the text color information
  dwFontListSize:         Number of elements in the font list, indexed from 0 to dwFontListSize-1
  pFontList:              A pointer to an array of font elements
  dwSampleModifiersSize:  Size of the sample modifiers buffer
  pSampleModifiersBuffer: Pointer to the sample modifiers buffer data

Comments:

See Also:
  AEEMediaVideoTTFontInfo
  AEEMMVStatusType

=============================================================================================
Description:
This structure is used to hold telop string information.

Definition:
typedef struct AEEMediaVideoTelopTextSubString
{
  boolean     bLineFeed;
  boolean     bUnderline;
  boolean     bReversal;
  boolean     bLinking;
  int32       nFontColor;
  uint32      dwTextSampleSize;
  uint32      dwLinkSize;
  AECHAR     *pTextSubString;
  AECHAR     *pLinkValue;
} AEEMediaVideoTelopTextSubString;

Members:

  bLineFeed:        Specifies a line feed
  bUnderline:       Specifies underlined text status
  bReversal:        Specifies if foreground color and background color are reversed 
  bLinking:         Validates following link string
  nFontColor:       Font color
  dwTextSampleSize: Size of Text string in bytes
  dwLinkSize:       Size of link string in bytes
  pTextSubString:   point to Text string
  pLinkValue:       point to Link string

Comments:

See Also:
  AEEMediaVideoTelopTextElement
  AEEMMVStatusType 

=============================================================================================
Description:
This structure is used to hold information for the telop text element.

Definition:
typedef struct AEEMediaVideoTelopTextElement
{
  uint32      dwTelopSize;
  int32       nBeginTime;
  int32       nDuration;
  boolean     bWrapFlag;
  int32       nBackgroundColor;
  int32       nDefaultFontColor;
  int32       nNumberOfSubStrings;
  AEEMediaVideoTelopTextSubString* pSubString
} AEEMediaVideoTelopTextElement;

Members:
  dwTelopSize:         Max. possible Telop element size
  nBeginTime:          Start time of the element in ms
  nDuration:           The duration which the text is displayed in ms
  bWrapFlag:           Specifies the word wrap status, TRUE for wrap, otherwise FALSE
  nBackgroundColor:    Background color
  nDefaultFontColor:   Default font color
  nNumberOfSubStrings: Numberof substring objects

Comments:

See Also:
  AEEMediaVideoTelopTextSubString
  AEEMMVStatusType
  
=============================================================================================
Description:
This structure is used to enable Qos and Set PDP profile.

Definition:
typedef struct AEEMediaVideoNetPolicyInfo
{
  uint32   dwSize;
  boolean  bQoSEnabled;
  uint8    wPrimaryPDPProfileNo;
}AEEMediaVideoNetPolicyInfo

Members:
  dwSize:                Size of this total structure in bytes
  bQoSEnabled:           TRUE, if QoS is enabled
  wPrimaryPDPProfileNo:  PDP profile Number

Comments:

See Also:
  AEEMMVStatusType

=============================================================================================
*/

#endif // AEEMEDIAVIDEO_H


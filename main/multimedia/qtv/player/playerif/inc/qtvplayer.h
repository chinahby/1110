#ifndef _QTVPLAYERAPI_H_
#define _QTVPLAYERAPI_H_
/* =======================================================================
                               qtvplayer.h
DESCRIPTION
  Native DMSS Qtv content player API

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/playerif/main/latest/inc/qtvplayer.h#53 $
$DateTime: 2010/11/09 04:48:52 $
$Change: 1509879 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "QtvConfig.h"
#include "mp4buf.h"
#include "qtv_log.h"
#include "../../../utils/url/inc/url.h" /*to fix high lint errors*/
#include "vdecoder_types.h"
#include "qcplayer_misc.h"
typedef void* dcf_ixstream_type;
#define IS_IPC_MSG_READY(sigType)  ((rex_get_sigs(rex_self())) & (sigType))

#define QTV_PLAYER_MULTIPLE_INST_1_0

class Mpeg4Player;
class QtvPlayerDispatch;
class DispatchThread;
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/*--------------------------------------------------
  Maximum string lengths, including null-terminator
  -------------------------------------------------- */

// MAX Media Information size
#define QTV_MAX_MEDIA_INFO_SIZE                     2048

/* Max length of an SDP request or response */
#define MAX_BUFFER_LEN                              4096
/* Max length of filename of a clip to download */
#define QTV_MAX_FILENAME_BYTES                       256
/* Clip info (c.i.) strings */
   /* Max length of c.i. title */
   #define QTV_MAX_TITLE_BYTES                        50
   /* Max length of c.i. author */
   #define QTV_MAX_AUTHOR_BYTES                       50
   /* Max length of c.i. copyright */
   #define QTV_MAX_COPYRIGHT_BYTES                   128
   /* Max length of c.i. rating string */
   #define QTV_MAX_RATING_BYTES                      128
   /* Max length of c.i. description */
   #define QTV_MAX_DESCRIPTION_BYTES                 256
   /* Max length of c.i. owner */
   #define QTV_MAX_OWNER_BYTES                       128
   /* Max length of c.i. creation date */
   #define QTV_MAX_CREATIONDATE_BYTES                128
   /* Max length of c.i. version string */
   #define QTV_MAX_VERSION_BYTES                     128
   /* Max length of c.i. album id string */
   #define QTV_MAX_ALBUM_BYTES                        50
   /* Max length of c.i. general info string */
   #define QTV_MAX_INFO_BYTES                        256
   /* Max length of c.i. performance string */
   #define QTV_MAX_PERF_BYTES                        128
   /* Max length of c.i. Genre string */
   #define QTV_MAX_GENRE_BYTES                       128
   /* Max length of c.i. Classification string */
   #define QTV_MAX_CLSF_BYTES                        128
   /* Max length of c.i. Keyword string */
   #define QTV_MAX_KYWD_BYTES                        128
   /* Max length of c.i. Location string */
   #define QTV_MAX_LOCI_BYTES                        256

   /*This is the max length of the YRRC string. 
   Not sure of the exact value, have to lookup in the 
   standard document*/
   #define QTV_MAX_YRRC_BYTES                        128


/* Maximum number of a/v tracks in a single playback */
#define QTV_MAX_MEDIA_TRACKS                           12

   /* length of device name string */
   #define QTV_MAX_DEVICE_NAME_BYTES                   8
   /* length of device model string */
   #define QTV_MAX_MODEL_NAME_BYTES                    8
   /* encoder information string length */
   #define QTV_MAX_ENCODER_INFO_BYTES                  8
   /* MUX information string length */
   #define QTV_MAX_MUX_INFO_BYTES                      8
   /* length of content title string */
   #define QTV_MAX_CONTENT_PROPERTY_TITLE_BYTES      128
   /* length of content copyright string */
   #define QTV_MAX_CONTENT_PROPERTY_COPYRIGHT_BYTES  128
   /* length of content author string */
   #define QTV_MAX_CONTENT_PROPERTY_AUTHOR_BYTES     128
   /* length of content memo field */
   #define QTV_MAX_CONTENT_PROPERTY_MEMO_BYTES       128
   /* 3 byte GPS latitude */
   #define QTV_MAX_GPS_LATITUDE                        3
   /* 3 byte GPS longitude */
   #define QTV_MAX_GPS_LONGITUDE                       3
   /* 3 byte GPS time */
   #define QTV_MAX_GPS_TIME                            3
   /* GPS survey data length */
   #define QTV_MAX_GPS_SURVEY_DATA_BYTES               8
   /* GPS Positioning method length */
   #define QTV_MAX_GPS_POSITIONING_METHOD_BYTES       64
   /* GPS Positioning name length */
   #define QTV_MAX_GPS_POSITIONING_NAME_BYTES         64
   /* GPS date string length */
   #define QTV_MAX_GPS_DATE_BYTES                     11
   /* Track language string length 
      This includes the NULL termination string 
   */
   #define QTV_MAX_LANGUAGE_BYTES 4

#define QTV_PLAYER_MIME_TYPE  "video/mp4"
#define REAL_MEDIA_MIME_TYPE  "video/rm"
#define RAM_FILE_MIME_TYPE    "video/ram"
#define AUDIO_ONLY_MIME_TYPE  "audio/generic"
#define REAL_MEDIA_EXT        "rm"
#define REAL_AUDIO_EXT        "ra"
#define REAL_VIDEO_EXT        "rv"
#define SDP_FILE_EXT          "sdp"
#define RAM_FILE_EXT          "ram"
#define URL_FILE_EXT          "url"
#define QTV_MAX_CHALLENGE_BYTES                   512

#define QTV_UNKNOWN_MIME_TYPE   "unknown/unknown"

#define QTV_NO_TRACK_SELECTED             0x00
#define QTV_VIDEO_TRACK_SELECTED          0x01
#define QTV_AUDIO_TRACK_SELECTED          0x02
#define QTV_TEXT_TRACK_SELECTED           0x04


//QTV Track Mute action type  
#define QTV_TRACK_MUTE                    0x01
#define QTV_TRACK_UNMUTE                  0x02

// Selected Track state. Either Selected or Not. 
// Also tells whether the selected track is muted or not. 
#define QTV_TRACK_STATE_SELECTED          0x01 
#define QTV_TRACK_STATE_MUTED             0x02 
#define QTV_TRACK_STATE_READY             0x04
#define QTV_TRACK_STATE_DISABLED          0x08

#define MP4_MIME_BUFFER_LENGTH            8
#define WM_MIME_BUFFER_LENGTH             16
#define REAL_MIME_BUFFER_LENGTH           4
#define MAXIMUM_MIME_BUFFER_LENGTH        16
#define AMR_MIME_BUFFER_LENGTH            5
#define MP3_MIME_BUFFER_LENGTH            3
#define AAC_MIME_BUFFER_LENGTH            4
#define QCP_MIME_BUFFER_LENGTH            12
#define RIFF_AVI_MIME_BUFFER_LENGTH       11
#define AVI_MIME_BUFFER_LENGTH            3
#define RIFF_MIME_BUFFER_LENGTH           4

#define QTV_REAL_CODEC_TYPES_V2

/* This define is for backward compatibility between Real and QTV */
#define QTV_PLAYER_FRAMES_USE_CWIN

/* all the ASF files starts with Header Object GUID, so if first 16 bytes of file matches the
** array below (which is GUID for ASF Header Object), we can safely assume that file/buffer is
** an ASF object */
extern unsigned char asfFileIdentifier[WM_MIME_BUFFER_LENGTH];

#define FMT_RM_MARK ".RMF"
#define FMT_MP4_MARK "ftyp"
#define FMT_MP4_MARK_1 "mdat"
#define FMT_WM_MARK   asfFileIdentifier
#define FMT_AMR_BEGIN "#!AMR"
#define FMT_QCP_QLCM  "QLCM"
#define FMT_RIFF_MARK  "RIFF"
#define FMT_AVI_MARK   "AVI"

#ifdef FEATURE_QTV_AVI
#define DIVX_AVI_FILE_SIGNATURE_BYTES 4
#define AVI_FILE_SIGNATURE_BYTES 3
#define DIVX_AVI_FileIdentifier = "RIFF"
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
  enum streaming_codec_select_criteria
  {
     DEFAULT_CRITERIA = -1,
     BITRATE_BASED_STREAMING_CODEC_CRITERIA = 0,
     PERFORMANCE_BASED_STREAMING_CODEC_CRITERIA = 1
  };

#ifdef FEATURE_QTV_TEXT_RECORD
  enum streaming_text_record_fromat
  {
     DEFAULT_TEXT_RECORD_FORMAT = -1,
     SKT_TEXT_RECORD_FORMAT = 0,
     TX3G_TEXT_RECORD_FORMAT = 1
  };
#endif

 enum UserAgentParamsType
  {
    QTV_STREAMING_EXTENSIONS_NONE,
    QTV_LGT_STREAMING_EXTENSIONS,
    QTV_SKT_STREAMING_EXTENSIONS
  };

  enum qtv_download_action
  {
    APP_TO_DECRYPT = 0,
    CONTENT_NOT_ENCRYPTED = 1,
    QTV_STOP = 2
  } ;

  enum EndOfClipActionT
  {
     NO_END_OF_CLIP_ACTION = 0
    ,END_OF_CLIP_CLOSE_APP
    ,END_OF_CLIP_PLAY_NEXT_URL
    ,END_OF_CLIP_LAUNCH_URL
  };

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* Flag used to switch between MPEG4-AAC & MPEG4-AAC+ DSP images. If
** TRUE: MPEG4-AAC+ image is loaded and post-processing is done by ARM
** FALSE: MPEG4-AAC image is loaded and post-processing is done by DSP
*/
extern "C" boolean qtv_cfg_enable_aacplus;

/* Flag used to enable or disable H263 Profile 3 on 6250. If
** TRUE: Profile 3 is enabled and the H263 DSP image is used to decode H263
** FALSE: Profile 3 is disabled and the MPEG-AAC+ DSP image is used to decode H263
*/
extern "C" boolean qtv_cfg_enable_h263_profile3;
/* Flag used to enable playing a clip with unsupported Profile Level */
extern boolean qtv_cfg_play_unsupported_profile_level;

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
CLASS
  QtvPlayer

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
class QtvPlayer
{
public:

  typedef void * InstanceHandleT;
  enum InstancePriorityT
  {
    QTV_PRIORITY_INVALID,
    QTV_PRIORITY_1,
    QTV_PRIORITY_HIGHEST = QTV_PRIORITY_1,
    QTV_PRIORITY_2,
    QTV_PRIORITY_3,
    QTV_PRIORITY_LOWEST  = QTV_PRIORITY_3,
    QTV_PRIORITY_DEFAULT = QTV_PRIORITY_LOWEST,
    QTV_PRIORITY_COUNT
  };

  enum OpenURNTypeT
  {
    OPEN_URN_NORMAL,
    FCS_PROBE
  };


  /* Return codes for all API methods. */
  enum ReturnT
  {
    QTV_RETURN_OK=0
    /* Command accepted.  If the command is asynchronous, completion
       status will be reported later in a callback. */

    ,QTV_RETURN_ERROR
    /* Command error, command cannot be processed */

   ,QTV_RETURN_UNSUPPORTED
   /*Unsupported MIME type*/

    ,QTV_RETURN_FEATURE_NOT_AVAILABLE
    /* Command rejected, feature is not available. */

    ,QTV_RETURN_DATA_NOT_AVAILABLE
    /* Command complete, requested data is not available. */

    ,QTV_RETURN_BUFFER_UNDERRUN
    /* Command rejected, supplied buffer is not enough. */

    ,QTV_RETURN_PRIORITY_IN_USE
    /* The requested priority is already in use by another QtvPlayer instance. */

    ,QTV_RETURN_MULTIPLE_INSTANCE_RUNNING
    /* More than one QtvPlayer instances are running. If API returns this error code, 
    ** then the API is not supported when multiple instances of QtvPlayer
    ** are running. */
    ,QTV_RETURN_INVALID_STATE
    /* Operation attempted in incorrect player state */

    ,QTV_RETURN_INVALID_PARAMETER
    /* Invalid Parameter */

    ,QTV_RETURN_NO_INSTANCE_AVAILABLE
    /*No Qtv instances available*/

  };

   /**
    * When using Brew, OEM can pass in IxStream..
    * Arguments:
    * pstream: This is actually IxStream*, associated with conetnt to be played.
    * dwTrackSelected: specifies which combination of tracks to play (audio/video/text)
    *
    * Following defines specify the mask to determine which tracks to be played.
    *
    *  #define QTV_NO_TRACK_SELECTED             0x00
    *  #define QTV_VIDEO_TRACK_SELECTED          0x01
    *  #define QTV_AUDIO_TRACK_SELECTED          0x02
    *  #define QTV_TEXT_TRACK_SELECTED           0x04
    *
    * OpenIxStream will fail if No tracks are selected,(dwTrackSelected == 0)
    * or pstream is NULL.
    */
    static QtvPlayer::ReturnT OpenIxStream(dcf_ixstream_type pstream,
                                           uint32 dwTrackSelected,
                                           InstanceHandleT handle = NULL,
                                           InstancePriorityT priority = QTV_PRIORITY_DEFAULT);
    /*
    * Retrieves the DIVX DRM registration code from the device.
    * Pass NULL as the first parameter to retrieve the length.
    * Registration code is NULL terminated string and
    * Valid registration code has 8 characters.
    * Thus, the length returned will be 8+1.
    */
    static QtvPlayer::ReturnT GetRegistrationCode(char*,int*,
                                                  InstanceHandleT handle = NULL,
                                                  InstancePriorityT priority = QTV_PRIORITY_DEFAULT);

  /* URN Types */
  enum URNTypeT
  {
    URN_NONE=0                /* NO URN defined */
    ,URN_RTSP                 /* URN is local EFS file containing rtsp: spec */
    ,URN_LOCAL_FILE           /* URN is local EFS media file */
    ,URN_LOCAL_BUFFER         /* URN is media in a memory buffer */
    ,URN_INPUTSTREAM          /* URN is an input stream */
    , URN_PSEUDO_STREAM
    , URN_HTTP_STREAM
    ,URN_PVX_FILE             /* URN is local EFS PVX file (proprietary) */
    ,URN_SDP_FILE             /* URN is local EFS file in SDP text format */
    ,URN_SDP_BUFFER
    ,URN_BCAST_ISDB           /* URN is ISDB broadcast media */
    ,URN_BCAST_FLO            /* URN is media broadcasted over the FLO network */
    ,URN_QTV_HTTP_STREAM      /* UNR is for Progressive dowload URN handled by QTV */
    ,URN_BCAST_TDMB           /* URN is T-DMB broadcast media */
    ,URN_MAPI_MEDIA_MASK      /* Media mask URN */
    ,URN_MAPI_REPLAY          /* Media mask log URN */
#ifdef FEATURE_QTV_GENERIC_BCAST_CMMB
#error code not present
#endif
  } ;

  /* URL Types */
  enum URLTypeT
  {
    URL_NONE=0                /* No URL defined */
    ,URL_STREAM=1              /* URL references media to be streamed */
    ,URL_DOWNLOAD
  } ;

  // Application parameters from the PVX file.

  enum DownloadPlaybackControlT
  {
      NO_PLAYBACK = 0
      ,PLAYBACK_AFTER_DOWNLOAD
      ,PLAYBACK_ASAP
      ,PLAYBACK_RESERVE
      ,NONE
  };


  class AppParametersT
  {
    public:
      bool   Secured;
      bool   UserPlaybackAllowed;
      bool   RepositioningAllowed;

      DownloadPlaybackControlT DownloadPlaybackControl;
      EndOfClipActionT EndOfClipAction;

      URL * NextURL;
  } ;


    /* Media types */
  enum MediaTypeT
  {
    MEDIA_AUDIO_ONLY=0
    ,MEDIA_VIDEO_ONLY
    ,MEDIA_AUDIO_VIDEO
    ,MEDIA_VIDEO_STILL_IMAGE
    ,MEDIA_AUDIO_VIDEO_STILL_IMAGE
    ,MEDIA_VIDEO_TEXT
    ,MEDIA_AUDIO_TEXT
    ,MEDIA_AUDIO_VIDEO_TEXT
    ,MEDIA_VIDEO_STILL_IMAGE_TEXT
    ,MEDIA_AUDIO_VIDEO_STILL_IMAGE_TEXT
    ,MEDIA_TEXT
    ,MEDIA_INVALID
  } ;

  enum TrackTypeT
  {
    /* Track Types */
    TRACK_UNKNOWN,          /* Unknown track type  */
    TRACK_AUDIO_EVRC,       /* EVRC audio          */
    TRACK_AUDIO_QCELP,      /* QCELP audio         */
    TRACK_AUDIO_AMR,        /* AMR audio           */
    TRACK_AUDIO_MP4A_LATM,  /* AAC audio           */
    TRACK_VIDEO_MP4V_ES,    /* MPEG4 video         */
    TRACK_VIDEO_H263_2000,  /* H.263 video         */
    TRACK_VIDEO_H264,       /* H.264 video         */
    TRACK_3GPP_TIMED_TEXT,  /* 3GPP Timed Text     */
    TRACK_GENERIC_TEXT,
    TRACK_WM_AUDIO,         /* Windows Media Audio */
    TRACK_WM1_VIDEO,        /* Windows Media Video 7 */
    TRACK_WM2_VIDEO,        /* Windows Media Video 8 */
    TRACK_WM3_VIDEO,        /* Windows Media Video 9 */
    TRACK_VIDEO_OSCAR,      /* OSCAR video         */
    /* Real Media Supported Track Types */
    TRACK_REAL_AUDIO_G2_MONO,
    TRACK_REAL_AUDIO_G2_DUAL_MONO,
    TRACK_REAL_AUDIO_8_STEREO,
    TRACK_REAL_VIDEO_8,
    TRACK_REAL_VIDEO_9,
    TRACK_VIDEO_DIVX311,
    TRACK_AUDIO_AMR_WB,     /* AMR-WB audio        */
    TRACK_AUDIO_AMR_WB_PLUS, /* AMR-WB+ audio       */
    TRACK_WM_PRO_AUDIO ,        /* Windows Media PRO */
    TRACK_WM_PRO_PLUS_AUDIO,     /* Windows Media PRO PLUS*/
    TRACK_AUDIO_EVRC_NB,     /* EVRC-NB audio        */
    TRACK_AUDIO_EVRC_WB, /* EVRC-WB audio       */
    TRACK_AUDIO_AC3,
    TRACK_AUDIO_PCM
  } ;

  typedef enum SysModeT
  {
    QTV_SYS_MODE_NONE = -1,

    QTV_SYS_MODE_NO_SRV=0,
        /* No service = NV_MODE_INACTIVE                      */

    QTV_SYS_MODE_AMPS=1,
        /* Analog Mobile Phone System (AMPS) mode             */

    QTV_SYS_MODE_CDMA=2,
        /* Code Division Multiple Access (CDMA) mode          */

    QTV_SYS_MODE_GSM=3,
        /* Global System for Mobile communications (GSM) mode */

    QTV_SYS_MODE_HDR=4,
        /* High Data Rate (HDR) mode                          */

    QTV_SYS_MODE_WCDMA=5,
        /* Wideband CDMA (WCDMA) mode                         */

    QTV_SYS_MODE_GPS=6,
        /* Global Positioning System (GPS) mode               */

    /* Value 7 reserved for the "GW" (GSM/WCDMA) mode in CM */

    QTV_SYS_MODE_WLAN=8,
        /* WLAN mode                                          */

    QTV_SYS_MODE_BIG = 0x10000000
        /* Force 32-bit enums */

  } SysModeT;

  enum DataT
  {
    /* Atom Types */
    DATA_ATOM_NONE = 0,
    DATA_ATOM_FTYP = 1,             /* File Type Atom*/
    DATA_ATOM_DCMD = 2,             /* Dcmd DRM Atom*/
    DATA_ATOM_UDTA_CPRT = 3,        /* Copyright Atom */
    DATA_ATOM_UDTA_AUTH = 4,        /* Author Atom*/
    DATA_ATOM_UDTA_TITL = 5,        /* Title Atom*/
    DATA_ATOM_UDTA_DSCP = 6,        /* Description Atom*/
    DATA_ATOM_UDTA_MIDI = 7,        /* Midi Atom*/
    DATA_ATOM_UDTA_LINK = 8,        /* Link Atom*/
    DATA_TEXT_TKHD_ORIGIN_X =9,     /* Text track origin_x*/
    DATA_TEXT_TKHD_ORIGIN_Y =10,    /* Text track origin_y*/
    DATA_TEXT_TKHD_WIDTH =11,       /* Text track width*/
    DATA_TEXT_TKHD_HEIGHT =12,      /* Text track height*/
    DATA_MEDIA_INFO=13,             /* Media information*/
    DATA_ATOM_UDTA_RTNG = 14,       /*Rating Atom*/
    DATA_ATOM_UDTA_PERF = 15,       /*Performance Atom*/
    DATA_ATOM_UDTA_CLSF = 16,       /*Classification Atom*/
    DATA_ATOM_UDTA_KYWD = 17,       /*Keyword Atom*/
    DATA_ATOM_UDTA_LOCI = 18,       /*Location Atom*/
    DATA_ATOM_UDTA_GNRE = 19,        /*Genre Atom*/
    DATA_ATOM_UDTA_META = 20,        /*Meta Atom*/
    DATA_ATOM_UUID = 21,            /*UUID atom*/
    DATA_ATOM_TX3G = 22,             /*Text Atom*/
    DATA_ATOM_SCHM = 23,
    DATA_ATOM_FRMA = 24,
    DATA_ATOM_OHDR = 25,
    DATA_ATOM_ODAF = 26,
    DATA_ATOM_MDRI = 27,
/* Here we are returning the offset of the atom including the header. */
    DATA_ATOM_SCHM_OFFSET = 28,	
    DATA_ATOM_FRMA_OFFSET = 29,	
    DATA_ATOM_OHDR_OFFSET = 30,	
    DATA_ATOM_ODAF_OFFSET = 31,
    DATA_ATOM_MDRI_OFFSET = 32,	
    DATA_ATOM_UDTA_ALBM = 33,        /* Album Atom*/
    DATA_ATOM_UDTA_YRRC = 34        /* recording year */		   
  } ;
  
  enum EncryptionTypeT
  {
    // type of encryption
    ENCRYPT_NONE = 0,
    ENCRYPT_OMA_DRM_V2,
    ENCRYPT_WM_DRM,
    ENCRYPT_UNSUPPORTED
  } ;

  typedef enum StreamTypeT
  {
    MEDIA_NON_STREAMING   = 0,      /* Local clip       */
    MEDIA_STREAMING       = 1,      /* non-live stream  */
    MEDIA_LIVE_STREAMING  = 2,      /* Live stream      */
    MEDIA_MCAST_STREAMING = 3,      /* Multicast stream */
    MEDIA_PV_SERVER_SIDE_PLAYLIST = 4, /* PV Server-side playlist (stream) */
    MEDIA_WM_SERVER_SIDE_PLAYLIST = 5, /* WM Server-side playlist (stream) */
    MEDIA_BUFFERED_LIVE_STREAMING = 6  /* Live stream via Time-Shift Buffer (DVR) */
  } StreamTypeT;

  /* Which type of extended ClipInfo is available */
  enum ClipExtInfoKindT
  {
     CLIP_EXT_INFO_NONE=0           /* does not have valid extended information */
    ,CLIP_EXT_INFO_STANDARD         /* has standard extended informations */
    ,CLIP_EXT_INFO_KDDI             /* has KDDI specific extended informations */
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    ,CLIP_EXT_INFO_PVX             /* has PVX specific extended information */
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    ,CLIP_EXT_INFO_PD             /* has PD specific extended information */
#endif
#ifdef FEATURE_QTV_DIVX_DRM
    ,CLIP_EXT_DIVX_DRM_INFO       /* has DivX DRM specific information*/
#endif
  } ;

  /* extended STANDARD clip information */
  struct ClipInfoStandardExt
  {
    /*Available after STATUS_PLAYBACK_READY or STATUS_DOWNLOAD_READY. */
    char      Title[QTV_MAX_TITLE_BYTES];
    char      Author[QTV_MAX_AUTHOR_BYTES];
    char      Copyright[QTV_MAX_COPYRIGHT_BYTES];
    char      Rating[QTV_MAX_RATING_BYTES];
    char      Description[QTV_MAX_DESCRIPTION_BYTES];
	char      Performance[QTV_MAX_PERF_BYTES];
	char      Genre[QTV_MAX_GENRE_BYTES];
	char      Classification[QTV_MAX_CLSF_BYTES];
	char      Keyword[QTV_MAX_KYWD_BYTES];
	char	  Location[QTV_MAX_LOCI_BYTES];

    /*Available after STATUS_PLAYBACK_READY. */
    char      Owner[QTV_MAX_OWNER_BYTES];
    char      Version[QTV_MAX_VERSION_BYTES];
    char      CreationDate[QTV_MAX_CREATIONDATE_BYTES];
    char      Artist[QTV_MAX_AUTHOR_BYTES];
    char      sTitle[QTV_MAX_TITLE_BYTES];
    char      Album[QTV_MAX_ALBUM_BYTES];
    char      Info[QTV_MAX_INFO_BYTES];
    char      LaunchURL[QTV_MAX_URN_BYTES];
    char      Yrrc[QTV_MAX_YRRC_BYTES];
    // 0 means no playlist. -1 means an unknown number of clips.
    int32  NumClipsInServerSidePlaylist;

    // -1 means unknown, 0 means not available, 1 means available
    int IsPrevClipInServerSidePlaylistAvail;

    // -1 means unknown, 0 means not available, 1 means available
    int IsNextClipInServerSidePlaylistAvail;
    EndOfClipActionT EndofClipAction;
  };

  /* KDDI specific extended clip information.
     For details, please refer to KDDI specification documents */
  struct ClipInfoKDDIExt
  {
    /*Available after successful OPEN_URN. */
    /* From CopyGaurdAtom */
    uint32      CopyProhibitionFlag;
    uint32      ValidityEffectiveDate;
    uint32      ValidityPeriod;
    uint32      NumberOfAllowedPlayBacks;
    /* From Content Property Atom */
    char        ContentPropertyTitle[QTV_MAX_CONTENT_PROPERTY_TITLE_BYTES];
    char        ContentPropertyCopyRight[QTV_MAX_CONTENT_PROPERTY_COPYRIGHT_BYTES];
    char        ContentPropertyAuthor[QTV_MAX_CONTENT_PROPERTY_AUTHOR_BYTES];
    char        ContentPropertyMemo[QTV_MAX_CONTENT_PROPERTY_MEMO_BYTES];
    uint32      AuthorDLLVersion;
    /* From Movie Mail Atom */
    uint32      EditFlags;
    uint32      RecordingMode;
    uint32      RecordingDate;
    /* From Encoder Information Atom */
    char        DeviceName[QTV_MAX_DEVICE_NAME_BYTES];
    char        ModelName[QTV_MAX_MODEL_NAME_BYTES];
    char        EncoderInformation[QTV_MAX_ENCODER_INFO_BYTES];
    char        MuxInformation[QTV_MAX_MUX_INFO_BYTES];
    /* From GPS Atom */
    uint16      GPSByteOrder;
    uint32      GPSVersionID;
    uint32      GPSLatitudeRef;
    uint32      GPSLongitudeRef;
    uint32      GPSAltitudeRef;
    uint64      GPSLatitudeArray[QTV_MAX_GPS_LATITUDE];
    uint64      GPSLongitudeArray[QTV_MAX_GPS_LONGITUDE];
    uint64      GPSAltitude;
    uint64      GPSTimeArray[QTV_MAX_GPS_TIME];
    char        GPSSurveyData[QTV_MAX_GPS_SURVEY_DATA_BYTES];
    char        GPSPositioningMethod[QTV_MAX_GPS_POSITIONING_METHOD_BYTES];
    char        GPSPositioningName[QTV_MAX_GPS_POSITIONING_NAME_BYTES];
    char        GPSDate[QTV_MAX_GPS_DATE_BYTES];
    uint64      GPSExtensionMapScaleInfo;
  } ;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  /* PVX Clip information */
  struct ClipInfoPvxExt
  {
    ClipInfoStandardExt      StdInfo;
    boolean                  PurgeDowloadedFile;
    boolean                  AllowUserPlayback;
    DownloadPlaybackControlT DownloadPlaybackControl;
  };
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  /* PD Clip information */
  struct ClipInfoPDExt
  {
    ClipInfoStandardExt      StdInfo;
    boolean                  PurgeDowloadedFile;
  };
#endif

#ifdef FEATURE_QTV_DIVX_DRM  
  /**
  Description:
  Following structure defines DivX Clip specific DRM information.

  Members:
  
  nUseLimit	               - The counter to specify maximum number of times this clip can be played.
  nUseCounter              - Current playback count for the given clip.
                             Clip can be played only 'nUseLimit' times. User has to renew the count once it expires.
  nCgmsaSignal             - External signal protection. Copy protection mechanism for analog television signals.
                         
  nAcptbSignal             - External signal protection. If set and if device does not have MacroVision,
                             TvOut should be disabled.
  nDigitalProtectionSignal - External signal protection for digital output. When set,
                             content can only be output over secure digital outputs approved by DivX or
                             analog video outputs that are constrained to maximum 480p resolution.

  Comments:
  Clip specific information,ClipDrmInfoT, will be available once QTV reports PB_READY.  
  */
  typedef struct DrmInfoT
  {
    bool     isRental;
    uint16   useLimit;
    uint16   useCounter;
    uint8    cgmsaSignal;
    uint8    acptbSignal;
    uint8    digitalProtectionSignal;
  }ClipDrmInfoT;     
#endif//#ifdef FEATURE_QTV_DIVX_DRM

  /* holds one of the following extended clip information */
  union ClipExtInfoT
  {
    ClipInfoStandardExt StdInfo;
    ClipInfoKDDIExt     KDDIInfo;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    ClipInfoPvxExt     PvxInfo;
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    ClipInfoPDExt     PDInfo;
#endif
#ifdef FEATURE_QTV_DIVX_DRM
    ClipDrmInfoT       DrmInfo;
#endif
  };

  /* Clip information */
  struct ClipInfoT
  {
    /* Available after STATUS_PLAYBACK_READY.
       Only applicable to streaming clips. */
    StreamTypeT  streamtype;
#ifdef FEATURE_QTV_PDCF
    EncryptionTypeT encryptionType;
#endif /* FEATURE_QTV_PDCF */
    /* Available after STATUS_PLAYBACK_READY. */
    boolean      RepositioningAllowed;
    boolean      AudioOnlyPlaybackAllowed;
    boolean      VideoOnlyPlaybackAllowed;

    /* Available after STATUS_PLAYBACK_READY or STATUS_DOWNLOAD_READY */
    MediaTypeT   MediaType;

    /* MIME and track types
       Available after STATUS_PLAYBACK_READY or STATUS_DOWNLOAD_READY. */
    int32       TrackCount;
    TrackTypeT  TrackTypes[QTV_MAX_MEDIA_TRACKS];

    /*Available after STATUS_PLAYING. */
    int32        Width;
    int32        Height;

    /*Available after STATUS_PLAYBACK_READY. */
    int32        Duration;

    /*Available after STATUS_PLAYBACK_READY or STATUS_DOWNLOADING.
      Not applicable to streaming clips. */
    int32        Size;

    long                AudioSamplingFreq;
    float               VideoFrameRate;
    unsigned long       NumAudioChannels;
    unsigned long       VideoEncodedBitRate;
    unsigned long       AudioEncodedBitRate;

    /* type of extended information */
    ClipExtInfoKindT ExtInfoType;
    /* extended information */
    ClipExtInfoT     ExtInfo;
  };

  /* Status information passed up to the application layer.
     This is the data that accompanies QTV_PLAYER_STATUS_INFO. */
  struct StatusInfoT
  {
    uint32 Size;
    void * Data;
  };

  /* Describes a logical rectangular window */
  struct WindowT
  {
    int32 x1; //(x1,y1) -> top_left coordinate
    int32 y1;
    int32 x2; //(x2,y2) -> bottom_right coordinate
    int32 y2;
  };

  struct FrameInfoT
  {
    /* Pointer to the frame buffer */
    void    *RGBBuffer;
    void    *YUVBuffer;
    int32    Height;
    int32    Width;
    //The display/crop area in the decoded frame buffer
    WindowT  CropWindow;
    uint32   Timestamp;
    uint32   NumConcealedMB;
    uint8    bI_VOP;

    VDEC_POST_FILTER_MB_INFO *pPostFilterMbInfo;

    unsigned long NumIntraMBBuffer;
  };

  /* KDDI Telop Element (for details, please refer to KDDI specification doc) */
  struct TelopElementT
  {
    int32       TelopSize;
    int32       BeginTime;
    int32       Duration;
    int32       NumberOfSubStrings;
    boolean     WrapFlag;
    int32       BackgroundColor;
    int32       DefaultFontColor;
  } ;

  /* KDDI Telop Element Substring struct (for details, please refer to KDDI specification doc) */
  struct TelopSubStringT
  {
    boolean   LineFeed;
    boolean   Underline;
    boolean   Reversal;
    boolean   Linking;
    int32     FontColor;
    int32     TextSampleSize;  /* in bytes */
    uint32    LinkSize;
    char     *TextSubString;
    char     *LinkValue;
  } ;

   // Download information
    struct DownloadInfoT
    {
        int    InterruptedTime;    //Playback time in milliseconds.
        char Filename[QTV_MAX_FILENAME_BYTES];
        bool   DownloadComplete;
    } ;


  /*--------------------------------------------------

     PLAYER status callback definitions

  --------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Callback status codes. Be extra carefull when adding to this enumeration.
    The code assignments are now independent of the FEATURE_ that are
    defined in a particular build. This is done through initializers placed
    in the enumeration. If there are new codes then all the initializers
    need to be updated. The simplest thing to do is just add to this enum
    at the very bottom of it. Also when there are new codes the
    PlayerStatusString[] stucture needs to be updated along with this one.
    In addition to PlayerStatusString[] if you have added a new code that
    can be classified as an "error" code, the IsPlayerStatusErrorCode()
    should be updated. That method is used to decide whether to assign a
    high priority to an outgoing QXDM message.
  -------------------------------------------------------------------------*/
  enum PlayerStatusT
  {
     QTV_PLAYER_STATUS_FIRST                            = 0

     /*--------------------------------------------------
       Completion codes for asynchronous commands
       --------------------------------------------------*/
     ,QTV_COMMAND_OPEN_URN_COMPLETE                      = 0
     /* A command to open a URN was successful */

     ,QTV_COMMAND_OPEN_URN_FAILED                        = 1
     /* A command to open a URN failed */

     ,QTV_COMMAND_REGISTER_FOR_CALLBACK_COMPLETE         = 2
     /* Registering a callback function successful */

     ,QTV_COMMAND_REGISTER_FOR_CALLBACK_FAILED           = 3
     /* Registering a callback function failed */

     ,QTV_COMMAND_PLAY_CLIP_COMPLETE                     = 4
     /* A command to play the clip was successful */

     ,QTV_COMMAND_PLAY_CLIP_FAILED                       = 5
     /* A command to play the clip failed */

     ,QTV_COMMAND_STOP_COMPLETE                          = 6
     /* A command to stop the clip was successful */

     ,QTV_COMMAND_STOP_FAILED                            = 7
     /* A command to stop the clip failed */

     ,QTV_COMMAND_PAUSE_COMPLETE                         = 8
     /* A command to pause the clip was successful */

     ,QTV_COMMAND_PAUSE_FAILED                           = 9
     /* A command to pause the clip failed */

     ,QTV_COMMAND_NEXT_FRAME_COMPLETE                    = 10
     ,QTV_COMMAND_NEXT_FRAME_FAILED                      = 11
     ,QTV_COMMAND_PREV_FRAME_COMPLETE                    = 12
     ,QTV_COMMAND_PREV_FRAME_FAILED                      = 13

     /*--------------------------------------------------
       Decoded frame notification
       --------------------------------------------------*/
     ,QTV_PLAYER_STATUS_DECODE_FRAME                     = 14
     /* Pass decoded frame back to OEM layer */

     /*--------------------------------------------------
       Playback completion notifications.
       --------------------------------------------------*/

     ,QTV_PLAYER_STATUS_PLAYBACK_COMPLETE                = 15
     /* Playback completion notice: normal termination. */

     ,QTV_PLAYER_STATUS_PLAYBACK_ERROR_ABORT             = 16
     /* Playback completion notice: error termination. */

     ,QTV_PLAYER_STATUS_PLAYBACK_STOPPED                 = 17
     /* Playback completion notice:
        Playback stopped by an API command. */

     /*--------------------------------------------------
       Player state notifications.
       --------------------------------------------------*/

     ,QTV_PLAYER_STATUS_CONNECTING                       = 18
     /* Playback status update: connecting to server. */

     ,QTV_PLAYER_STATUS_PLAYBACK_READY                   = 19
     /* Playback status update: ready to play clip. */

     ,QTV_PLAYER_STATUS_BUFFERING                        = 20
     /* Playback status update: data is buffering. */

     ,QTV_PLAYER_STATUS_OPENING                          = 21
     /* Playback status update: opening and processing file. */

     ,QTV_PLAYER_STATUS_PLAYING                          = 22
     /* Playback status update: data is playing. */

     ,QTV_PLAYER_STATUS_PAUSED                           = 23
     // Playback status update: paused.

     , QTV_PLAYER_STREAM_AUTH_REQUIRED                   = 24
     // Stream requires the user to provide the authorization

     ,QTV_PLAYER_STATUS_REPOSITIONING                    = 25
     /* Playback status update: suspended while repositioning. */

     ,QTV_PLAYER_STATUS_IDLE                             = 26
     /* Overall player state update: playback and download
        are both idle. */

	  ,QTV_PLAYER_STATUS_NEXT_TEXT_READY                  = 27
     /* Playback status update: next 3GPP timed-text sample
        ready for rendering. */

     ,QTV_PLAYER_STATUS_NEXT_TELOP_READY                 = 28
     /* Playback status update: next telop timed-text sample
        ready for rendering */

     /*--------------------------------------------------
       Playback error notifications.
       --------------------------------------------------*/

     ,QTV_PLAYER_STATUS_AUDIO_ERROR                      = 29
     /* An audio playback error occurred--audio will terminate. */

     ,QTV_PLAYER_STATUS_VIDEO_ERROR                      = 30
     /* A video playback error occurred--video will terminate. */

     ,QTV_PLAYER_STATUS_TEXT_ERROR                       = 31
     /* A timed text playback error occurred--timed text processing will terminate. */

     ,QTV_PLAYER_STATUS_STREAM_ERROR                     = 32
     /* Streaming server error. */

     ,QTV_PLAYER_STATUS_AUDIO_ERROR_FORMAT_NOT_SUPPORTED = 33
     /* Audio format of the clip is not supported.  Audio will not play. */

     ,QTV_PLAYER_STATUS_VIDEO_ERROR_FORMAT_NOT_SUPPORTED = 34
     /* Video format of the clip is not supported.  Video will not play. */

     ,QTV_PLAYER_STATUS_VIDEO_ONLY_PLAYBACK_NOT_ALLOWED  = 35
     /* Audio format of the clip is not supported and
        video-only playback is not allowed.
        Playback will terminate */

     ,QTV_PLAYER_STATUS_AUDIO_ONLY_PLAYBACK_NOT_ALLOWED  = 36
     /* Video format of the clip is not supported and
        audio-only playback is not allowed.
        Playback will terminate */

     ,QTV_PLAYER_STATUS_PLAY_TIME_ERROR                  = 37
     /* Start time is beyond the end of clip
        or stop time is before start time.
        Playback will terminate. */

     ,QTV_PLAYER_STATUS_NO_SYNC_FRAME_ERROR              = 38
     /* No Sync (I_Frame) frame present in the
        media to complete re-positioning. So
        repositioning failed. */

     ,QTV_PLAYER_STATUS_NEXT_FRAME_DONE                  = 39
     /* Step Next Frame operation completed successfully */

     ,QTV_PLAYER_STATUS_NEXT_FRAME_FAILED                = 40
     /* Step Next Frame operation failed to complete */

     ,QTV_PLAYER_STATUS_PREV_FRAME_DONE                  = 41
     /* Step back to previous Frame completed successfully */

     ,QTV_PLAYER_STATUS_PREV_FRAME_FAILED                = 42
     /* Step back to previous Frame Failed. This may happen,
        if no previous decoded frames are in buffer */

	 ,QTV_PLAYER_STATUS_ROTATION_DONE                    = 43
     /* Frame Rotation completed successfully */

     ,QTV_PLAYER_STATUS_ROTATION_FAILED                  = 44
     /* Frame Rotation failed, this may be due to
        unsupported value */

     ,QTV_PLAYER_STATUS_SCALING_DONE                     = 45
     /* Frame Scaling completed successfully */

     ,QTV_PLAYER_STATUS_SCALING_FAILED                   = 46
     /* Frame Scaling failed, this may be due to
        unsupported value */

    /* Qtv paused as enough data is not available
       in pseudo stream buffer.
    */

    ,QTV_PLAYER_STATUS_PSEUDO_PAUSE                     = 47

     /* Qtv resumed from PSEUDO_PAUSE state (above)  */
    ,QTV_PLAYER_STATUS_PSEUDO_RESUME                    = 48

	,QTV_PS_COMMAND_OPEN_STREAM_COMPLETE                = 49
	,QTV_PS_COMMAND_OPEN_STREAM_FAILED                  = 50

    ,QTV_PS_PARSER_STATUS_PAUSE                         = 51
	,QTV_PS_PARSER_STATUS_RESUME                        = 52
	,QTV_PS_PARSER_STATUS_READY                         = 53
	,QTV_PS_PARSER_STATUS_FAILED                        = 54

	,QTV_PSEUDO_STREAM_ALREADY_ACTIVE                   = 55
	,QTV_PS_PLAYER_BUFFERING                            = 56

     ,QTV_COMMAND_ROTATE_COMPLETE                       = 57
     /* The command to rotate the clip (in any direction) was successful */

     ,QTV_COMMAND_ROTATE_FAILED                         = 58
    /* The command to rotate the clip failed */

      , QTV_PLAYER_STATUS_VIDEO_RESTARTING_TRACK        = 59
      , QTV_PLAYER_STATUS_AUDIO_RESTARTING_TRACK        = 60
      , QTV_PLAYER_STATUS_TEXT_RESTARTING_TRACK         = 61
      , QTV_PLAYER_LOOP_TRACK_NOT_SUPPORTED             = 62

      , QTV_COMMAND_PAUSE_VIDEO_COMPLETE                = 63
      , QTV_COMMAND_PAUSE_VIDEO_FAILED                  = 64
      , QTV_COMMAND_PAUSE_AUDIO_COMPLETE                = 65
      , QTV_COMMAND_PAUSE_AUDIO_FAILED                  = 66
      , QTV_COMMAND_PAUSE_TEXT_COMPLETE                 = 67
      , QTV_COMMAND_PAUSE_TEXT_FAILED                   = 68

      , QTV_COMMAND_RECORD_CLIP_COMPLETE                = 69
       /* A command to RECORD the clip was successful */

      ,QTV_COMMAND_RECORD_CLIP_FAILED                   = 70
      /* A command to RECORD the clip failed */

      ,QTV_COMMAND_RECORD_STOP_COMPLETE                 = 71
       /* A command to STOP RECORDING was successful  */

      ,QTV_COMMAND_RECORD_STOP_FAILED                   = 72

       /*  A command to stop clip recording has failed */

      ,QTV_REC_FILE_SYS_LIMIT_APPROACHING               = 73
       /* Stream recording: File system limit is approaching*/

      ,QTV_REC_FILE_SYS_FULL                            = 74
       /* Stream recording: File system is full */

      ,QTV_RECORDER_FILE_ALREADY_EXISTS                 = 75
       /* Given file name already exists */

      ,QTV_RECORDER_FILE_WRITE_ERROR                    = 76
       /* Error occured while writing to file system */

      ,QTV_RECORDER_FILE_OW_FAILURE                     = 77
       /* Failed to overwite a given file name*/

      ,QTV_RECORDER_FAILED                              = 78
       /* Recorder Failed */

      ,QTV_PLAYER_STATUS_REDIRECT                       = 79

      , QTV_COMMAND_RESUME_VIDEO_COMPLETE               = 80
      , QTV_COMMAND_RESUME_VIDEO_FAILED                 = 81
      , QTV_COMMAND_RESUME_AUDIO_COMPLETE               = 82
      , QTV_COMMAND_RESUME_AUDIO_FAILED                 = 83
      , QTV_COMMAND_RESUME_TEXT_COMPLETE                = 84
      , QTV_COMMAND_RESUME_TEXT_FAILED                  = 85

      , QTV_MIDI_DATA_PRESENT                           = 86

     ,QTV_COMMAND_SKIP_CLIP_COMPLETE                     = 87
     /* A command to skip the clip was successful */

     ,QTV_COMMAND_SKIP_CLIP_FAILED                       = 88
     /* A command to skip the clip failed */

     ,QTV_PLAYER_STATUS_NO_RANDOM_ACCESS_POINT_ERROR     = 89
     /* No Random Access frame present in the
        media to complete re-positioning. So
        repositioning failed. */

    ,QTV_PLAYER_STATUS_SEEK_FAILED                      = 90
    /* Reposition attempt failed */

     ,QTV_COMMAND_PARSE_FRAGMENT_COMPLETE               = 91
     /* A command to open a URN was successful */

     ,QTV_COMMAND_PARSE_FRAGMENT_FAILED                 = 92
     /* A command to open a URN failed */

    ,QTV_PLAYER_RECORDING_COMPLETED                     = 93
    /* File recorded to file system */

    , QTV_PLAYER_STATUS_VIDEO_MEDIA_END                 = 94
    /* End of video track */

    , QTV_PLAYER_STATUS_AUDIO_MEDIA_END                 = 95
    /* End of audio track */

    , QTV_PLAYER_STATUS_TEXT_MEDIA_END                  = 96
    /* End of text track */

    ,QTV_PLAYER_STATUS_DOWNLOAD_COMPLETE                = 97
    ,QTV_PLAYER_STATUS_DOWNLOAD_ERROR_ABORT             = 98
    ,QTV_PLAYER_STATUS_DOWNLOAD_STOPPED                 = 99
    ,QTV_COMMAND_DELETE_DOWNLOAD_SESSION_COMPLETE        = 100
    ,QTV_COMMAND_DELETE_DOWNLOAD_SESSION_FAILED          = 101
    ,QTV_COMMAND_DOWNLOAD_CLIP_COMPLETE                  = 102
    ,QTV_COMMAND_DOWNLOAD_CLIP_FAILED                    = 103
    ,QTV_COMMAND_RESUME_DOWNLOAD_COMPLETE                = 104
    ,QTV_COMMAND_RESUME_DOWNLOAD_FAILED                  = 105
    ,QTV_COMMAND_STOP_DOWNLOAD_COMPLETE                  = 106
    ,QTV_COMMAND_STOP_DOWNLOAD_FAILED                    = 107
    ,PV_PLAYER_STATUS_DOWNLOAD_TEMPFILE_ERROR           = 108
    ,QTV_HTTP_COMMAND_OPEN_STREAM_COMPLETE              = 109
	,QTV_HTTP_COMMAND_OPEN_STREAM_FAILED                = 110
    ,QTV_HTTP_PLAYER_BUFFERING                          = 111
    ,QTV_PLAYER_STATUS_HTTP_PAUSE                       = 112
    ,QTV_PLAYER_STATUS_HTTP_RESUME                      = 113
    ,QTV_HTTP_PARSER_STATUS_PAUSE                       = 114
	,QTV_HTTP_PARSER_STATUS_RESUME                      = 115
	,QTV_HTTP_PARSER_STATUS_READY                       = 116
	,QTV_HTTP_PARSER_STATUS_FAILED                      = 117

    ,QTV_PLAYER_STATUS_VIDEO_RELEASE_DONE               = 118
    /* Command to release video DSP successful */

    ,QTV_PLAYER_STATUS_VIDEO_RELEASE_FAILED             = 119
    /* Command to release video DSP failed */

    ,QTV_UPDATE_HTTP_STREAM_COMPLETE                    = 120

    ,QTV_UPDATE_HTTP_STREAM_FAILED                      = 121

    ,QTV_PLAYER_STATUS_STREAM_SERVER_CLOSED             = 122
    /* Streaming server closed connection */

    ,QTV_PLAYER_TS_LAYER_PRESENT                        = 123
     /* Temporal scalability layer present in clip. */

    ,QTV_PLAYER_STATUS_RELEASE_QOS_INPROGRESS           = 124
     /* QoS Release in Progress */

    ,QTV_PLAYER_STATUS_RELEASE_QOS_DONE                 = 125
    /* QoS Release Complete */

    ,QTV_PLAYER_STATUS_VIDEO_RELEASE_INPROGRESS          = 126
    /* Video DSP Release in Progress */

    ,QTV_STREAM_UNSUPPORTED_MEDIA_TYPE			             = 127
    /*media file unsupported*/

    ,QTV_STREAM_RATE_ADAPTATION_SUPPORTED                = 128
    /*rate adaptation supported*/

    ,QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED    = 129
    /* video dimensions not supported */

    ,QTV_BUFFER_TO_FILE_COPY_DISALLOWED                 = 130
    ,QTV_BUFFER_TO_FILE_COPY_FILE_ALREADY_EXISTS        = 131
    ,QTV_BUFFER_TO_FILE_COPY_COMPLETE                   = 132
    ,QTV_BUFFER_TO_FILE_COPY_FAILED                     = 133
    ,QTV_BUFFER_TO_FILE_COPY_NOT_ENOUGH_SPACE           = 134

    ,QTV_PLAYER_STATUS_SUSPENDED                        = 135
    /* Playback status update: suspended */

    ,QTV_PLAYER_STATUS_SUSPEND_FAILED                   = 136
    /* Playback status update: suspend failed */

    ,QTV_PLAYER_STATUS_SUSPENDING                       = 137
    /* Playback status update: internal suspend, ignore key press */


    ,QTV_PLAYER_STATUS_DOWNLOAD_READY                   = 138
    ,QTV_PLAYER_STATUS_DOWNLOADING                      = 139
    ,QTV_PLAYER_STATUS_DOWNLOADED_DATA_AVAIL            = 140
    ,QTV_PLAYER_STATUS_DOWNLOAD_ABORT_MEM_FAIL          = 141

    ,QTV_PLAYER_STATUS_INFO                             = 142

    ,QTV_COMMAND_REGISTER_DRM_DECRYPT_METHOD_COMPLETE   = 143
    ,QTV_COMMAND_REGISTER_DRM_DECRYPT_METHOD_FAILED     = 144

    ,QTV_COMMAND_PLAYLIST_PLAY_COMPLETE                 = 145
    /* A command to skip to a clip in the playlist was successful */

    ,QTV_COMMAND_PLAYLIST_PLAY_FAILED                   = 146
    /* A command to skip to a clip in the playlist was successful */

    ,QTV_PLAYER_STATUS_CLIP_TRANSITION                  = 147
    /* Playback on the next clip in a playlist has started */

    ,QTV_PLAYER_STATUS_PLAYLIST_PLAY_COMPLETE           = 148
    /* A user-initiated skip in a playlist succeeded */

    ,QTV_PLAYER_STATUS_PLAYLIST_PLAY_FAILED             = 149
    /* A user-initiated skip in a playlist failed */

    ,QTV_PLAYER_AUDIO_UNSUPPORTED_BIT_RATE              = 150
    ,QTV_PLAYER_VIDEO_UNSUPPORTED_BIT_RATE              = 151
    ,QTV_PLAYER_VIDEO_UNSUPPORTED_PROFILE               = 152

    ,QTV_PLAYER_STATUS_UNSUPPORTED_CODEC                = 153
    /* Unknown Codec status*/

    ,QTV_UPDATE_PSEUDO_STREAM_COMPLETE                  = 154
    ,QTV_UPDATE_PSEUDO_STREAM_FAILED                    = 155
    /* For pseudo stream buffer update status*/

    /*IxStream creation for Video track failed*/
    ,QTV_PLAYER_VIDEO_IXSTREAM_FAILED                   = 156

    /*IxStream creation for audio track failed*/
    ,QTV_PLAYER_AUDIO_IXSTREAM_FAILED                   = 157

    /*IxStream creation for text track failed*/
    ,QTV_PLAYER_TEXT_IXSTREAM_FAILED                    = 158

    ,QTV_PLAYER_STATUS_PLAY_NOT_SUPPORTED               = 159
    /* When Mpeg4Player is suspended and In-call QTV is not supported and if
       an attempt is made to playback. */

    ,QTV_PLAYER_STATUS_PAUSED_SUSPENDED                 = 160
    /* When Mpeg4Player is suspended both internally (due to incoming call)
       and externally (MP is suspended or has issued a pause). */


    ,QTV_PLAYER_COMMAND_SEEK_SYNC_DONE                  = 161
    /* QTV_FEATURE_BCAST_GENERIC : SEEK SYNC Executed */

    ,QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED                = 162
    /* QTV_FEATURE_BCAST_GENERIC : SEEK SYNC Failed   */ 
 
   ,QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_DONE             = 163
    /* QTV_FEATURE_BCAST_GENERIC : PLAY SYNC Executed */

    ,QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED          = 164
    /* QTV_FEATURE_BCAST_GENERIC : PLAY SYNC Failed */ 

   ,QTV_PLAYER_COMMAND_REPOSITION_DONE                  = 165
    /* QTV_FEATURE_BCAST_GENERIC : SEEK SYNC Executed */

    ,QTV_PLAYER_COMMAND_REPOSITION_FAILED               = 166
    /* QTV_FEATURE_BCAST_GENERIC : SEEK SYNC Failed     */                    

    ,QTV_PLAYER_STATUS_TRACK_LIST_UPDATE                = 167
    /* QTV_FEATURE_BCAST_GENERIC : Track List Update */

    ,QTV_PLAYER_COMMAND_MUTE_COMPLETE                   = 168
    /* QTV_FEATURE_BCAST_GENERIC : PLAY SYNC Failed */ 

   ,QTV_PLAYER_COMMAND_MUTE_FAILED                      = 169
    /* QTV_FEATURE_BCAST_GENERIC : SEEK SYNC Executed */

    ,QTV_PLAYER_COMMAND_UNMUTE_COMPLETE                 = 170
    /* QTV_FEATURE_BCAST_GENERIC : PLAY SYNC Failed */ 

   ,QTV_PLAYER_COMMAND_UNMUTE_FAILED                    = 171
    /* QTV_FEATURE_BCAST_GENERIC : SEEK SYNC Executed */

    ,QTV_PLAYER_COMMAND_MUTE_ALL_FAILED                 = 172
     /* QTV_FEATURE_BCAST_GENERIC : SEEK SYNC Executed */


    ,QTV_PLAYER_COMMAND_SELECT_PB_TRACK_COMPLETE        = 173
    /* QTV_FEATURE_BCAST_GENERIC : PLAY SYNC Failed */ 

   ,QTV_PLAYER_COMMAND_SELECT_PB_TRACK_FAILED           = 174
    /* QTV_FEATURE_BCAST_GENERIC : SEEK SYNC Executed */

    ,QTV_PLAYER_STATUS_NEXT_GENERIC_TEXT_READY          = 175
    // QTV_PLAYER_STATUS_NEXT_GENERIC_TEXT_READY
    /* Playback status update: next simple text sample
       ready for rendering. */

    ,QTV_PLAYER_STATUS_END_OF_TSB                       = 176
    /* End of time shift buffer reached */

    ,QTV_PLAYER_STATUS_TSB_OVERFLOW                     = 177
    /* Time shift buffer overflow */

    ,QTV_PLAYER_STATUS_TSB_DATA_LOSS                    = 178
    /* Time shift buffer data loss */

    ,QTV_PLAYER_STATUS_PLAYING_1P3X                     = 179
    /* Playback status update: data is playing at 1.3X speed. */

    ,QTV_PLAYER_STATUS_TRACK_STATE_CHANGE               = 180
    /* Track information has changed. */

    ,QTV_PLAYER_DRM_AUTHORIZATION_ERROR                 = 181
    /* User Authorization Error */

    ,QTV_PLAYER_DRM_DEVICE_REGISTRATION_ERROR           = 182
    /* Device is not registered for DRM playback */

    ,QTV_PLAYER_DRM_RENTAL_COUNT_EXPIRED                = 183
    /* Rental Count has expired. */

    ,QTV_PLAYER_DRM_PLAYBACK_GENERAL_ERROR              = 184
    /* General DRM error occured. */

    ,QTV_PLAYER_CONSUME_RENTAL_VIEW_CONFIRMATION        = 185
    /*  Confirmation needed from user before consuming view count */

    ,QTV_PLAYER_STATUS_SERVER_TIMEOUT                   = 186
    /*when real player gets timeout alert from server*/

    ,QTV_PLAYER_STATUS_SWITCHING_IN_PROGRESS           = 187
     /* Switch Stream success for switch */

    /* Switch partial status for a switch request*/

    ,QTV_PLAYER_STATUS_SWITCH_STREAM_FAILED             = 188
    /* Switch Stream failed status for a switch request*/

    ,QTV_PLAYER_STATUS_FCS_PLAYBACK_COMPLETE            = 189
    /* current clip playback somplete status for FCS session*/

    ,QTV_COMMAND_PROBE_COMPLETE                         = 190
      /* Probe URN command complete */

    ,QTV_COMMAND_PROBE_FAILED                           = 191
      /* Probe URN command complete */

    ,QTV_PLAYER_STATUS_SERVER_NOT_ENOUGH_BW             = 192
    /* SERVER not having enough BW */

   ,QTV_PLAYER_STATUS_VALID_CLIPINFO_DIMENSIONS        = 193
    /* to notify validation of clip*/

   ,QTV_PLAYER_STATUS_DATA_INACTIVITY_ERROR		= 194
    /* Inactivity timer expired */

   ,QTV_STREAM_TRACKLIST_UNKNOWN_CODEC                  = 195
      /* stream track mime not supported/unknown codec */	
   
    ,QTV_PLAYER_STATUS_RECONNECT_SUCCESS                = 196
    /* Reconnect to the server is successfull*/

    ,QTV_PLAYER_STATUS_RECONNECT_FAIL                   = 197
    /* Reconnect to server failed*/

    ,QTV_PLAYER_STATUS_RECONNECT_IN_PROGRESS            = 198
    /* Reconnection to server is in progress*/
   
   ,QTV_PLAYER_STATUS_FCS_SWITCH_SUPPORTED             = 199
    /*session supports switch */

   ,QTV_PLAYER_STATUS_RECONNECTING_USING_TCP_INTERLEAVE = 200
    /* Reconnecting using tcp interleaving*/

   ,QTV_PLAYER_STATUS_LAST                             = 200
    /* This is the last and has value equal to the one before */
  };


  /*---------------------------------------------------------------------------
    This is an array of strings that correspond to the enum codes in the
    PlayerStatusT. The exact same FEATURE-ization does not apply here. The
    reason is that the enum codes will get the same values regardless of the
    FEATUREs and they are used to address this array of strings. So in this
    array we must always have all the strings defined in the correct locations.
  ---------------------------------------------------------------------------*/
  static const char * PlayerStatusString[];

  enum QosStatusT
  {
     QOS_SPDP_NOT_INUSE       = 0,  /* SPDP is not in use                */
     QOS_SPDP_BEING_REQUESTED = 1,  /* Qtv is requesting SPDP            */
     QOS_SPDP_ESTABLISHED     = 2,  /* Qtv is granted SPDP               */
     QOS_SPDP_DEACTIVATED     = 3,  /* SPDP is deactivated (dormant)     */
     QOS_SPDP_BEING_RELEASED  = 4   /* Qtv is releasing SPDP             */
  };

  //QTV Track Mute action type  
  typedef uint32   MuteActionT; 

  /* AAC channel information */
  enum ChannelConfigT
  {
    AUDIO_CHANNEL_UNKNOWN = 0, 
    AUDIO_CHANNEL_MONO,         /* Single channel */
    AUDIO_CHANNEL_DUAL,         /* Stereo */ 
    AUDIO_CHANNEL_TRIPLE,       /* 3 channels (UNSUPPORTED) */  
    AUDIO_CHANNEL_QUAD,         /* 4 channels (UNSUPPORTED) */ 
    AUDIO_CHANNEL_QUINTUPLE,    /* 5 channels (UNSUPPORTED */ 
    AUDIO_CHANNEL_SEXTUPLE,     /* 5+1 channels (UNSUPPORTED */ 
    AUDIO_CHANNEL_OCTUPLE,      /* 7+1 channels (UNSUPPORTED) */ 
    AUDIO_CHANNEL_DUAL_MONO,    /* Dual mono */ 
    AUDIO_CHANNEL_UNSUPPORTED
  };

  /* AAC codec information */
  struct AACInfoT
  {
    uint32          nSamplingFreq;   /* Sampling frequency */
    ChannelConfigT  eChannelConfig;  /* Channel configuration */
  };

  /* Codec specific information */
  union CodecInfoT
  {
    AACInfoT aacInfo;  /* AAC codec specific information */
  };

  // Track Information structure. 
  // Track list is an array of track information structures. 
  struct TrackListT
  {
    uint32          nTrackID;     /* track identifier */
    TrackTypeT     trackType;    /* track codec type */
    MediaTypeT     mediaType;    /* track media type */
    uint32      bmTrackState;    /* track state */
    CodecInfoT     codecInfo;    /* codec information */
    EncryptionTypeT encryptionType;
    char             language[QTV_MAX_LANGUAGE_BYTES];
  };


  /* Callback state record. */
  struct PlayerStateRecordT
  {
    int32   videoFrames;       /* video processing position (frame count). */
    int32   audioFrames;       /* audio processing position (frame count). */
    uint32  playbackMsec;      /* playback position (milliseconds).        */

    uint32 videoPlaybackMsec;   /* video track play position */
    uint32 audioPlaybackMsec;   /* audio track play position */
    uint32 textPlaybackMsec;    /* text track play position */

 /* Used in pseudo streaming to notify current playback offset.
    It is expressed in no of bytes from the beginning of
    pseudo streaming buffer provided by application
 */
    uint32  playbackOffsetinBytes;

    uint32 downloadProgress;  // download progress (percent complete).
    uint32 downloadTime;    /* playback time available from the downloading file */

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif
  } ;


  /*-------------------------------------------------------------------------
    Callback Function Type for the PlayerStatusT.
  -------------------------------------------------------------------------*/
  typedef void (*PlayerStatusCallbackT)
    (
      PlayerStatusT   status,  /* player status code  */
      const char *    strptr   /* pointer to a string */
    );

  typedef void (*PlayerStatusInstanceCallbackT)
    (
      PlayerStatusT   status,  /* player status code  */
      const char *    strptr,  /* pointer to a string */
      InstanceHandleT  handle,
      void *          pUserData
    );

  /* Callback Function Type. */
  typedef void (*CallbackFuncT)(

    PlayerStatusT status,    /* player status code                                   */
    void *      pClientData,
    PlayerStateRecordT state /* current player state.                                */
    );

  typedef void (*CallbackInstanceFuncT)(

    PlayerStatusT status,     /* player status code                                   */
    void *      pClientData,
    PlayerStateRecordT state, /* current player state.                                */
    InstanceHandleT  handle,
    void *      pUserData
    );


  typedef void (*SuspendStatusCallbackT)(

    bool status,    /* suspend status code                                           */
    void *      pClientData
    );

  /*-------------------------------------------------------------------------
    Callback Function Type for Pull interface for Buffered playback.
  -------------------------------------------------------------------------*/
  /*Callback used to check the amount of data OEM can supply. The value returned by
  this callback must not decrease on successive periodic invocations to this callback.
  OEM is responsible for the persistent management of the buffered data.
  */
  typedef void (*FetchBufferedDataSizeT)
    (
      uint32       pUserData,       /* In case of OOP, the function can be implemented in a class which has C interface to map correctly.*/
      uint32 *     bufDataSize,     /* Amount of data so far valid for playback */
      boolean *    pbEndOfData,     /* OEM will mark this as TRUE if entire data available in the buffer */ 
      InstanceHandleT  handle       /* Qtv instance handle */ 
    );

  /*
  The following callback can be used to fetch the actual valid data from the OEM.
  It returns the amount of bytes actually read into the supplied buffer 'dataBuf'.
  It no data is available to copy into dataBuf then it returns 0.
  Arguments:
  dataBuf         - Destination buffer to copy the data
  readSize        - Amount of data to be read
  readPos         - Data read offset from base 0
  'trackId'       - Media stream trackId. It is just an additional information provided
                    to help OEM implement this function to support concurrency in their code.
                    Its up to the OEM to use it.
  */
  typedef uint32 (*FetchBufferedDataT)
    (
      void *      dataBuf,         /* Destination buffer to copy the data */
      uint32      readSize,        /* Amount of data to be read */
      uint32      readPos,         /* Data read offset from base 0 */
      uint32      trackId,         /* Media stream trackId */
      InstanceHandleT  handle      /* Qtv instance handle */ 
    );

  /*Callback used by QTV to query OEM if the MIME type is supported
  Argument
  Sring containing the MIME type */
  typedef qtv_download_action  (*IsMimeTypeSupportedT)
  (
      char *MIMEString /* String which contains the MIME type */
  );

  /*-------------------------------------------------------------------------
    Callback Function Type for RTSP Status codes
  -------------------------------------------------------------------------*/
  typedef void (*RTSPStatusCallbackT)
    (
      int32         status,         /* integer RTSP status, per rfc2326 */
      const char *  statusStringPtr /* text equiv of status */
    );

  typedef void (*RTSPStatusInstanceCallbackT)
    (
      int32         status,          /* integer RTSP status, per rfc2326 */
      const char *  statusStringPtr, /* text equiv of status */
      InstanceHandleT  handle,
      void *      pUserData
    );

    struct NetPolicyInfo
    {
      boolean isSet;
      boolean isQoSEnabled;
      int     primaryPDPProfileNo;
    };

    /*-------------------------------------------------------------------------
      Callback function type for unhandled SDP and RTSP headers
    -------------------------------------------------------------------------*/
    typedef void (*UnhandledStreamingItemCallbackT)
    (
      const char *unhandledItem
    );

  /*-------------------------------------------------------------------------
    Callback Function Type for the allocation of output buffers
  -------------------------------------------------------------------------*/
typedef void * (*MallocOutputBufferT)( const VDEC_STREAM_ID  streamID, 
                                       void *        pCbData,
                                       uint32        bytes );

typedef void     (*FreeOutputBufferT)( const VDEC_STREAM_ID  streamID,
                                       void *        pCbData, 
                                       void *        pointer );

  static InstanceHandleT QtvInstances[QTV_PRIORITY_COUNT];
  InstancePriorityT InstancePriority;
  
  /* Set to true if this QtvPlayer instance gets successfully 
  ** registered within internal function RegisterInst*/
  bool m_registered;
  
  // Local non-static class members
  // Player object
  Mpeg4Player         *pMpeg4Player;

  const char          *CurrentMIMEType;
  CallbackFuncT        CallbackFunction;
  void                *CallbackClientData;
  CallbackInstanceFuncT CallbackInstanceFunction;
  void                *CallbackUserData;
  MallocOutputBufferT  MallocBufferFunction;
  FreeOutputBufferT    FreeBufferFunction;

  NetPolicyInfo NetPolicy;
  void                *MediaInfo;
  uint32               MediaInfoSize;
  UnhandledStreamingItemCallbackT unprocessedRtspHeaderCallback;
  UnhandledStreamingItemCallbackT unprocessedSdpHeaderCallback;

  void *descramblerHandler;
  char MediaPlayerRootPath[MAX_MEDIAPLAYER_ROOT_PATH_BYTES];

  #ifdef FEATURE_QTV_DRM_DCF
  QtvPlayerDispatch* pDispatchInstance;
  DispatchThread* pDispatchThreadHandle;
  boolean g_always_through_ixstream;
  #endif

   /*---------------------------------------------------------------------------
   The callback function to obtain RTSP result status codes and text
   equivalents.  When the RTSP engine receives a status code of other than
   "Ok", this information is passed to any interested software via the
   function specified in this pointer.  The pointer is set via
   SetRTSPStatusCallback(), and accessed via GetRTSPStatusCallback().  It
   is declared static, so is only available via the access function.
   ---------------------------------------------------------------------------*/
  RTSPStatusCallbackT RTSPStatusT_CB;
  RTSPStatusInstanceCallbackT RTSPStatusInstanceT_CB;
  void *RTSPStatusT_CB_UserData;

   /*---------------------------------------------------------------------------
   This is a callback definition for the PlayerStatusT enum type. It is
   initialized to NULL here. It is STATIC to this file and accessed through
   its own access method provided by the class. The purpose is to retain
   its value even after a destruction of the instance of the class.
   ---------------------------------------------------------------------------*/
  PlayerStatusCallbackT PlayerStatusT_CB;
  PlayerStatusInstanceCallbackT PlayerStatusInstanceT_CB;
  void *PlayerStatusT_CB_UserData;

  /*--------------------------------------------------

     Placeholder for RAM File Player data

  --------------------------------------------------*/
  void *RAMPlayer;

  /*--------------------------------------------------

     Player Control APIs.

  --------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Method that returns an appropriate explanatory string for the code
  -------------------------------------------------------------------------*/
  static const char *MapPlayerStatusCodetoString(PlayerStatusT code);

  /*-------------------------------------------------------------------------
    Method that returns True/False based on whether the PlayerStatusT
    code is classified as an "error" code.
  -------------------------------------------------------------------------*/
  static boolean     IsPlayerStatusErrorCode(PlayerStatusT code);

  /*-------------------------------------------------------------------------
    Method that returns True/False based on whether the PlayerStatusT
    code is classified as an "trivial" code. Helps avoid too many messages
  -------------------------------------------------------------------------*/
  static boolean     IsPlayerStatusTrivialCode(PlayerStatusT code);

  /*-------------------------------------------------------------------------
    Method that registers the callback for the PlayerStatusT codes and
    their corresponding explanatory strings.
  -------------------------------------------------------------------------*/
  static boolean SetQTVErrorCallback(PlayerStatusCallbackT PlayerStatusCB);
  static boolean SetQTVErrorCallback(PlayerStatusInstanceCallbackT PlayerStatusCB, InstanceHandleT handle, void *pUserData);

  /*-------------------------------------------------------------------------
    Method that returns the callback of type PlayerStatusCallbackT. This is
    useful checking whether is defined (not NULL) and so it should be called
  -------------------------------------------------------------------------*/
  static PlayerStatusCallbackT GetQTVErrorCallback(void);
  static PlayerStatusInstanceCallbackT GetQTVErrorCallback(InstanceHandleT handle);

  /*-------------------------------------------------------------------------
    Method that registers the callback function to obtain RTSP result
    status codes and text equivalents.  When the RTSP engine receives a
    status code of other than "Ok", this information is passed to any
    interested software via the function specified in this call.
  -------------------------------------------------------------------------*/
  static void SetRTSPStatusCallback(RTSPStatusCallbackT RTSPStatusCB);
  static void SetRTSPStatusCallback(RTSPStatusInstanceCallbackT RTSPStatusCB, InstanceHandleT handle, void *pUserData);

  /*-------------------------------------------------------------------------
    Method to access the callback function currently registered for
    obtaining RTSP result status codes and text equivalents.  If no callback
    has been registered, 0 is returned.
  -------------------------------------------------------------------------*/
  static RTSPStatusCallbackT GetRTSPStatusCallback(void);
  static RTSPStatusInstanceCallbackT GetRTSPStatusCallback(InstanceHandleT handle);
  static void* GetRTSPStatusUserData(InstanceHandleT handle);

  /* Method to create & initialize the QTVPlayer. */
  /*-------------------------------------------------------------------------
  In you pass in a pointer to a QtvPlayer Instance handle in the parameter
  "instance", Init will create a unique instance of the QtvPlayer.  If you
  pass in NULL for the parameter "instance", a default instance of the
  QtvPlayer is created for you automatically to allow the QtvPlayer API to be
  backwards compatible for users that need only one instance of the
  QtvPlayer.  It is necessary to pass in a pointer to a QtvPlayer instance
  handle only when you need to run more than one instance of the QtvPlayer
  simultaniously.  For example if you want to play a video ringer while you
  are already playing a video clip.  If you pass in a pointer to a QtvPlayer
  instance handle, then you must also pass that QtvPlayer instance handle
  into each QtvPlayer static class member function that takes an
  InstanceHandleT object.
  -------------------------------------------------------------------------*/
  static ReturnT Init(MallocOutputBufferT, FreeOutputBufferT, InstanceHandleT *handle = NULL);

  /* Method to select the URN. Video URN will only play video and audio
     URN will only play audio. Pass same URN in both, if you want to
     play video and audio from the same file. */
  static ReturnT OpenURN(const char *VideoURN, const char *AudioURN);

  /* Method to select the buffer. Video buffer will only play video and audio
     buffer will only play audio. Pass same buffer in both, if you want to
     play video and audio from the same buffer. */
  static ReturnT OpenURN(unsigned char *VideoBuf, uint32 videoBufSize, unsigned char *AudioBuf, uint32 audioBufSize);

  /* Method to select the URN. Video URN will only play video, audio
     URN will only play audio and Text URN can only play text. Pass same URN in all,
     if you want to play video, audio and text from the same file. */
  static ReturnT OpenURN(const char *VideoURN, const char *AudioURN, const char *TextURN,
                         InstanceHandleT handle = NULL,
                         InstancePriorityT priority = QTV_PRIORITY_DEFAULT,
                         OpenURNTypeT opentype = OPEN_URN_NORMAL );

  /* Method to select the buffer. Video buffer will only play video, audio
     buffer will only play audio and text buffer will only play text. Pass same buffer in all,
     if you want to play video, audio and text from the same buffer. */
  static ReturnT OpenURN(unsigned char *VideoBuf, uint32 videoBufSize,
                         unsigned char *AudioBuf, uint32 audioBufSize,
                         unsigned char *TextBuf, uint32 textBufSize,
                         InstanceHandleT handle = NULL,
                         InstancePriorityT priority = QTV_PRIORITY_DEFAULT);
#ifdef FEATURE_QTV_FCS
#error code not present
#endif

  /* Methods to get information about the URN, URL, and/or PVX file. */
  static ReturnT GetURNType(URNTypeT &URNType, bool bVideoURN=true, InstanceHandleT handle = NULL);
  static ReturnT GetURLType(URLTypeT &URLType, InstanceHandleT handle = NULL);

  /* Method to register for status updates via a callback function. */
  static ReturnT RegisterForCallback(CallbackFuncT callback, void *pClientData);
  static ReturnT RegisterForCallback(CallbackInstanceFuncT callback, void *pClientData, InstanceHandleT handle, void *pUserData);

  ReturnT RegisterForCallbackInternal(CallbackFuncT callback,  CallbackInstanceFuncT callbackInstance, 
                                      void *pClientData, void *pUserData);

  /* Methods to control playback. */
  static ReturnT PlayClip(int32 startTime, int32 stopTime, InstanceHandleT handle = NULL);

  /* Overloaded PlayClip to support content switching */
  static ReturnT PlayClip(int32 startTime, int32 stopTime,const char *switchURN,InstanceHandleT handle = NULL);

  static ReturnT RecordClip(char* filename,
                             bool overwrite,
                             uint8 mode,
                             uint32 duration, 
                             InstanceHandleT handle = NULL);

  static ReturnT StopClipRecording(InstanceHandleT handle = NULL);
  static ReturnT SetStrRecEFSReservedSpace(uint32 resSpaceinBytes, InstanceHandleT handle = NULL);
  static ReturnT getRecordedClipDuration(uint32 &duration, InstanceHandleT handle = NULL);

  static ReturnT SuspendForIncomingCall(SuspendStatusCallbackT suspendCbFn, void *suspendClientData, InstanceHandleT handle = NULL);
  static ReturnT RestoreAfterIncomingCallEnd(InstanceHandleT handle = NULL);

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif

  static ReturnT Pause(InstanceHandleT handle = NULL);
  static ReturnT Stop(InstanceHandleT handle = NULL);

  enum AudioSourceFormatT
  {
    AUDIO_SOURCE_AUTO,      /* Qtv decides between CLIP and CONC_CALL based on
                               call state (default) */
    AUDIO_SOURCE_CLIP,      /* Force audio from clip */
    AUDIO_SOURCE_CONC_CALL, /* Audio from voice call */
    AUDIO_SOURCE_MAX
  };

  static ReturnT SetAudioSource(AudioSourceFormatT audiosource,
                             InstanceHandleT handle = NULL);

  /* displays the next video frame (video must be in Pause mode) */
  static ReturnT PlayNextFrame(InstanceHandleT handle = NULL);
  /* displays the prev video frame if it is available in buffer (video must be in Pause mode) */
  static ReturnT PlayPrevFrame(InstanceHandleT handle = NULL);

  /* Retrieve information about the currently decoded frame */
  static ReturnT GetFrameInfo(FrameInfoT &frameInfo, InstanceHandleT handle = NULL);

  /* Extended version of the GetFrameInfo function that retrieves extended
     metadata information in addition to the the decoded frame. */
  static ReturnT GetExtFrameInfo(FrameInfoT &frameInfo, void **ppExtFrmInfo, InstanceHandleT handle = NULL);

  /* Methods to get information about the clip */
  static ReturnT GetClipInfo(ClipInfoT &clipInfo, InstanceHandleT handle = NULL);

  /* ====================================================================== */
  /*  Function : QtvPlayer::GetServerChallenge()                            */
  /*  Author   : Harikishan Desineni                                    */
  /*  Date   : 09/16/2003                                         */
  /*  Purpose  : Method to obtain the value field of WWW-Authenticate header*/
  /*            received from the server                                    */
  /*  In/out : challenge* Pointer to write the challenge rcvd from server   */
  /*                                                                        */
  /* Return : QTV_RETURN_ERROR/QTV_RETURN_OK                              */
  /* Note     : size of the challenge can be atmost QTV_MAX_CHALLENGE_BYTES */
  /*            defined in this header file. Qtv App must provide a buffer  */
  /*            of minimum size QTV_MAX_CHALLENGE_BYTES                     */
  /*                                                                        */
  /* ====================================================================== */
  static ReturnT GetServerChallenge(char *challenge, InstanceHandleT handle = NULL);

  /* ====================================================================== */
  /*  Function : QtvPlayer::SetQtvUserAuthInfoandRetry()                    */
  /*  Author   : Harikishan Desineni                                  */
  /*  Date   : 09/16/2003                                         */
  /*  Purpose  : Provide user authorization info to QtvPlayer and Retry     */
  /*             the session establishment                                  */
  /*  In/out   : Authorization info                                         */
  /*  Return   : QTV_RETURN_ERROR if it fails, else QTV_RETURN_OK           */
  /*  Note     : size of the string pointed by authInfo must not exceed     */
  /*             QTV_MAX_CHALLENGE_BYTES defiend in this header file.If it  */
  /*             exceeds,Qtv Player will truncate the string to             */
  /*             QTV_MAX_CHALLENGE_BYTES                                    */
  /*                                                                        */
  /*            authinfo is passed as is in the value field of outgoing RTSP*/
  /*            message header "Authorization: "                            */
  /*            App is responsible for any kind of encoding specific to     */
  /*            authentication scheme.                                      */
  /*                                                                        */
  /*            If App has informed it's ability to provide auth info, it   */
  /*            must callback this method. Pass NULL argument for authinfo  */
  /*            to inform Qtv player close/cancel the session               */
  /* ====================================================================== */
  static ReturnT SetQtvUserAuthInfoandRetry(char *authInfo, InstanceHandleT handle = NULL);

  /* ====================================================================== */
  /*  Function : QtvPlayer::QtvAppCanAuthenticate()                       */
  /*  Author   : Harikishan Desineni                                    */
  /*  Date      : 09/16/2003                                      */
  /*  Purpose  : Method to record the ability of the QtvApp to provide      */
  /*            Authorization information, if required                      */
  /*  In/out   : auth:                                                      */
  /*             TRUE, if Qtv APP can provide authorization info            */
  /*            FALSE, if Qtv App cannot provide authorization info         */
  /*                                                                        */
  /* Return : Always QTV_RETURN_OK                                          */
  /* Note  : By default, Qtv player assumes that Qtv App has no capability  */
  /*         to provide authentication information                          */
  /* ====================================================================== */
  static ReturnT QtvAppCanAuthenticate(bool auth, InstanceHandleT handle = NULL);


  /*------------------------------------------------------------------------
    Method to open a pseudo stream.
    Arguments:
    pBuf: pointer to the beginning of pseudo stream buffer
    dwBufSize: size of pseudo stream buffer
    wPtrOffset: current pseudo stream buffer write pointer offset.
    It should be 0 if nothing has been written to the buffer yet.

    Notes: OpenPseudoStream( ) will fail if a pseudo stream is already active.

  -------------------------------------------------------------------------*/

  static ReturnT OpenPseudoStream(unsigned char *pBuf,
                                  uint32 dwBufSize,
                                  uint32 wPtrOffset,
                                  InstanceHandleT handle = NULL,
                                  InstancePriorityT priority = QTV_PRIORITY_DEFAULT);

  static ReturnT OpenPseudoStream(unsigned char *pBuf,
                                  uint32 dwBufSize,
                                  uint32 wPtrOffset,
                                  uint32 dwTrackSelected,
                                  InstanceHandleT handle = NULL,
                                  InstancePriorityT priority = QTV_PRIORITY_DEFAULT);

  /*------------------------------------------------------------------------
   Method to update current pseudo stream buffer write pointer offset.
   Application should call this method and update Qtv after every write
   operation to pseudo stream buffer.

   Arguments:

   wPtrOffset: Write pointer offset from the beginning of pseudo stream buffer
           This value is expressed in bytes.

   wPtrOffset MUST always increment. Application should never pass an offset
   value which is less than the value issued in past updates. Such an update
   will be treated as fatal error in Qtv.

  -------------------------------------------------------------------------*/

  static ReturnT UpdatePseudoStreamBufferWriteOffset(uint32 wPtrOffset, InstanceHandleT handle = NULL);

  /*------------------------------------------------------------------------
    Method to open an HTTP Stream.
    Arguments:
    pBuf: pointer to the beginning of http stream buffer
    dwBufSize: size of http stream buffer
    wPtrOffset: current http stream buffer write pointer offset.
    It should be 0 if nothing has been written to the buffer yet.

    Notes: OpenHTTPStream( ) will fail if an HTTP stream is already active.

  -------------------------------------------------------------------------*/

  static ReturnT OpenHTTPStream(unsigned char *pBuf,
                                uint32 dwBufSize,
                          		  uint32 wPtrOffset,
                                InstanceHandleT handle = NULL,
                                InstancePriorityT priority = QTV_PRIORITY_DEFAULT);

  static ReturnT OpenHTTPStream(unsigned char *pBuf,
                                uint32 dwBufSize,
                                uint32 wPtrOffset,
                                uint32 dwTrackSelected,
                                InstanceHandleT handle = NULL,
                                InstancePriorityT priority = QTV_PRIORITY_DEFAULT);

  /*------------------------------------------------------------------------
    Method to open an Buffered Stream with Push Interface.
    Arguments:
    pBuf: pointer to the beginning of Buffered stream buffer
    dwBufSize: size of Buffered stream buffer
    writePtrOffset: current Buffered stream buffer write pointer offset.
    It should be 0 if nothing has been written to the buffer yet.
    dwTrackSelected: specifies which combination of tracks to play (audio/video/text)
                     #define QTV_NO_TRACK_SELECTED             0x00
                     #define QTV_VIDEO_TRACK_SELECTED          0x01
                     #define QTV_AUDIO_TRACK_SELECTED          0x02
                     #define QTV_TEXT_TRACK_SELECTED           0x04
    Notes: OpenBufferedStream( ) will fail if
            a) No tracks are selected.
            b) pBuf is null OR dwBufsize is 0.
  -------------------------------------------------------------------------*/

  static ReturnT OpenPushBufferedStream(unsigned char *pBuf,
                                        uint32 dwBufSize,
                                        uint32 writePtrOffset,
                                        uint32 dwTrackSelected,
                                        InstanceHandleT handle = NULL,
                                        InstancePriorityT priority = QTV_PRIORITY_DEFAULT);

  /*------------------------------------------------------------------------
    Method to open an Buffered Stream with Pull Interface.
    Arguments:
    dwTrackSelected: specifies which combination of tracks to play (audio/video/text)
                     #define QTV_NO_TRACK_SELECTED             0x00
                     #define QTV_VIDEO_TRACK_SELECTED          0x01
                     #define QTV_AUDIO_TRACK_SELECTED          0x02
                     #define QTV_TEXT_TRACK_SELECTED           0x04
    FetchBufferedDataSize: OEM callback to pull the size of the data currently buffered.
    FetchBufferedData: OEM callback to the pull the buffered data.
    Notes: OpenPullBufferedStream( ) will fail if
            a) No tracks are selected.
            b) FetchBufferedDataSize is null OR FetchBufferedData is null.
  -------------------------------------------------------------------------*/
  static ReturnT OpenPullBufferedStream(FetchBufferedDataSizeT FetchBufferedDataSize,
                                        FetchBufferedDataT FetchBufferedData,
                                        uint32 dwTrackSelected,
                                        InstanceHandleT handle = NULL,
                                        InstancePriorityT priority = QTV_PRIORITY_DEFAULT);


  /*------------------------------------------------------------------------
   Method to update current buffered stream buffer write pointer offset.
   Application should call this method and update Qtv after every write
   operation to stream buffer.

   Arguments:

   writePtrOffset: Write pointer offset from the beginning of stream buffer
           This value is expressed in bytes and it MUST always increment. It it
           decreases then the player might enter rebuffering depending on the
           value of the writePtrOffset.

  -------------------------------------------------------------------------*/
  static ReturnT UpdatePushBufferedDataOffset(uint32 writePtrOffset, InstanceHandleT handle = NULL);

  /*------------------------------------------------------------------------
   Method to update current HTTP stream buffer write pointer offset.
   Application should call this method and update Qtv after every write
   operation to HTTP stream buffer.

   Arguments:

   wPtrOffset: Write pointer offset from the beginning of HTTP stream buffer
           This value is expressed in bytes.

   wPtrOffset MUST always increment. Application should never pass an offset
   value which is less than the value issued in past updates. Such an update
   will be treated as fatal error in Qtv.

  -------------------------------------------------------------------------*/

  static ReturnT UpdateHTTPStreamBufferWriteOffset(uint32 wPtrOffset, InstanceHandleT handle = NULL);



 /*------------------------------------------------------------------------
   Method to set the desired start up time and buffering resume time in ms
   Application should call this method and set the optimal desired time
   Qtv will start playback after there is enough data to play upto the
   start up time and wil buffer data until there is enough data to play
   up to the buffer update time

   Arguments:

   startupTime : playback time required before playback can start
                 This value is expressed in ms.
   bufferingResumeTime: While buffering the playback time required before
                       playback can restart. This value is expressed in ms.

  -------------------------------------------------------------------------*/

  static ReturnT SetStartAndBufferingTime(uint32 startupTime, uint32 bufferingResumeTime, InstanceHandleT handle = NULL);

  /*------------------------------------------------------------------------
   Method to skip the clip to seek the next Random Access Point

   Arguments: int32 skipNumber gives the number of points to be skipped
              rewind or Fast Forward. If skipping 3 points forward set
              skipNumber to 3, if skipping 3 points Rewind set skipNumber to -3
  -------------------------------------------------------------------------*/
  static ReturnT SkipClip(int32 skipNumber, InstanceHandleT handle = NULL);

  /*
   the OEM registers for the Callbacks with QTV after doing the INIT

   Input Arguments:
   OEM callback to send Encypted Data from QTV to OEM
   OEM callback to Fetch the size of the data currently buffered.
   OEM callback to Fetch the buffered data
   OEM callback to query if MIME type is supported.
   Maxiumum Size of that data buffer QTV can use during WriteEncryptedDataCB.

   Return Type:
   QTV_RETURN_OK indicating success, any other value indicates failure. */

  static QtvPlayer::ReturnT RegisterCallbacksForDownload(QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize,
                                   QtvPlayer::FetchBufferedDataT FetchBufferedData,
                                   QtvPlayer::IsMimeTypeSupportedT IsMimeTypeSupported, InstanceHandleT handle = NULL);
  // Qtv Player API used my OEM to free the buffer it received during Write CB
  static char* GetEncryptedData(uint32 &size, InstanceHandleT handle = NULL);
  static QtvPlayer::ReturnT  FreeDownloadBuffer(char *buffer, InstanceHandleT handle = NULL);

  // Callback data for QTV_PLAYER_STATUS_PLAYLIST_PLAY_COMPLETE and
  // QTV_PLAYER_STATUS_CLIP_TRANSITION callbacks.
  typedef struct _PlaylistClipTransitionInfo
  {
    uint32 clipIndex;
    uint32 clipTransitionErrorReasons;
    uint32 transitionTime;
    char playlistName[QTV_MAX_URN_BYTES];
  } PlaylistClipTransitionInfo;

#ifdef FEATURE_QTV_GENERIC_BCAST
  // Callback data for following status 
  // QTV_PLAYER_COMMAND_REPOSITION_DONE
  // QTV_PLAYER_COMMAND_SEEK_SYNC_DONE 
  // QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_DONE
  // QTV_PLAYER_COMMAND_REPOSITION_FAILED
  // QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED 
  // QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED
  typedef struct _MapiCallbackInfo
  {
    uint32 uTransID;
    uint32 uTimestamp;
  } MapiCallbackInfo;
#endif 

  /*------------------------------------------------------------------------
   Method to seek to a clip in a playlist

   Arguments:

   clipIndex: Either the absolute value of the clip to seek to if
              isClipIndexRelative is false, or a relative value (with respect
              to the currently playing clip), if isClipIndexRelative is false.
   preferClientPlaylistSkip: If we are inside of a server-side playlist
                             which is inside a client-side playlist, and this
                             flag is true, we will interpret the skip as within
                             the client-side playlist. Otherwise we will
                             interpret the seek as within the server-side
                             playlist.
  -------------------------------------------------------------------------*/
  static ReturnT PlaylistPlayClip(int32 clipIndex,
                                  bool isClipIndexRelative = true,
                                  bool preferClientPlaylistSkip = true, 
                                  InstanceHandleT handle = NULL);

  /*------------------------------------------------------------------------
   Method to seek to a clip in a playlist

   Arguments:

   playlistName: For server-side playlists, some server may allow the playlist
                 to be changed. If so, this is relative or absolute playlist
                 filename.
   clipIndex: Either the absolute value of the clip to seek to if
              isClipIndexRelative is false, or a relative value (with respect
              to the currently playing clip), if isClipIndexRelative is false.
   time:      The absolute value of the time within the requested clip.
   when:      When during the duration of the currently playing clip to
              transition to the next clip. -1 means now, -2 means when the
              currently playing clip has finished, -3 means when the playlist
              is finished.
  -------------------------------------------------------------------------*/
  static ReturnT PlaylistPlayClip(const char *playlistName,
                                  int32 clipIndex,
                                  bool isClipIndexRelative = false,
                                  int32 time = 0,
                                  int32 when = -1, 
                                  InstanceHandleT handle = NULL);

  /* The following methods are for timed text only: */
  typedef struct _ThreeGPPTimedTextInfo
  {
    uint32 dwTextDataSize;  /* size of text type data */
    uint32 dwDisplayFlags;
    uint32 dwHorzJustification;
    uint32 dwVertJustification;
    uint32 dwBoxTop;
    uint32 dwBoxLeft;
    uint32 dwBoxBottom;
    uint32 dwBoxRight;
    uint32 dwStartChar;
    uint32 dwEndChar;
    uint32 dwFontID;
    uint32 dwFontStyleFlags;
    uint32 dwFontSize;
    uint32 dwFontListSize;  /* call get GetFontInfoAt to get the font list */
    uint32 dwDuration;
    uint32 dwBeginTime;
    uint32 dwSampleModifiersSize;
  } ThreeGPPTimedTextInfo;
  static boolean Get3GPPTimedTextInfo(ThreeGPPTimedTextInfo *pInfo, InstanceHandleT handle = NULL);
  static boolean Get3GPPFontInfoAt(uint16 * pFontID, char * pNameBuf, int nBufSize, uint16 Idx, InstanceHandleT handle = NULL);
  static boolean Get3GPPTimedTextData(uint8 * pDataBuf, int nBufSize, InstanceHandleT handle = NULL);
  static boolean Get3GPPTimedTextBGRGB(uint8 * pRGBBuf, int nBufSize, InstanceHandleT handle = NULL);
  static boolean Get3GPPTimedTextTxtRGB(uint8 * pRGBBuf, int nBufSize, InstanceHandleT handle = NULL);

  /*------------------------------------------------------------------------
   Method to get text data, textformat.
   Should be called when "QTV_PLAYER_STATUS_NEXT_GENERIC_TEXT_READY" is recieved 

   Arguments:

   pTextFormat: Output text format from text media.
   pDataBuf   : Output text buffer, if this is NULL then the text data size available with Qtv
                will be copied to *(pBufSize).
   pBufSize   : OEM/App has to pass "pDataBuf" size when "pDataBuf" is not NULL.
                Qtv will copy the text data size to *(pBufSize).                  

  -------------------------------------------------------------------------*/
  static ReturnT GetGenericTextData(uint8** pTextFormat, uint8* pDataBuf, uint32* pBufSize, InstanceHandleT handle = NULL);

  enum RotationType
  {
    ROTATE_NO,
    ROTATE_90_CW,
    ROTATE_90_CCW,
    ROTATE_180
  };
  /* rotates the video to the desired direction
    (compared to the original orientation) */
  static ReturnT RotateVideo(RotationType direction, InstanceHandleT handle = NULL);

  enum ScalingType
  {
    SCALE_4X_ZOOM=4,
    SCALE_2X_ZOOM=2,
    SCALE_NONE=0,
    SCALE_2X_SHRINK=(-2),
    SCALE_4X_SHRINK=(-4)
  };
  /* scales(zoom/shrink) the video to the desired factor
    (compared to the original size) */
  static ReturnT ScaleVideo(ScalingType ScaleFactor, InstanceHandleT handle = NULL);

  /* To arbitrarily scale video from 0.3x - 1,9x with DSP xScalar module*/
  static ReturnT ScaleVideo(uint32 outWidth, uint32 outHeight, InstanceHandleT handle = NULL);

  /* Methods to get information about the current playback and/or download */
  static ReturnT GetPlayerState(PlayerStateRecordT &state, InstanceHandleT handle = NULL);

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  static ReturnT GetDownloadTime(PlayerStateRecordT &state, InstanceHandleT handle = NULL);
  static DownloadPlaybackControlT  getDownloadClipType(InstanceHandleT handle = NULL);
#endif

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  static bool SetPvxPreroll(int pvxPreroll, InstanceHandleT handle = NULL);
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
  static bool SetFileSaveOption(char *filePath, InstanceHandleT handle = NULL);

  /* Method to shutdown and destroy the player. */
  static ReturnT Terminate(InstanceHandleT handle = NULL, bool destroyInst = true);

  enum AudOverrideType
  {
    AUD_OVER_NONE=0,
    AUD_OVER_ENC_QCELP13K,
    AUD_OVER_ENC_EVRC,
    AUD_OVER_ENC_AMR,
    AUD_OVER_DEC_AAC,
    AUD_OVER_DEC_MP3,
    AUD_OVER_DEC_AMR,
    AUD_OVER_DEC_QCP,
    AUD_OVER_LAST
  };

  static ReturnT SetAudOverride(AudOverrideType audformat, InstanceHandleT handle = NULL);

  enum CodecType
  {
    QTV_EVRC_CODEC,
    QTV_EVRC_NB_CODEC,
    QTV_EVRC_WB_CODEC,
    QTV_QCELP_CODEC,
    QTV_AAC_CODEC,
    QTV_BSAC_CODEC,
    QTV_GSM_AMR_CODEC,
    QTV_MPEG4_CODEC,
    QTV_DIVX311_CODEC,
    QTV_H263_CODEC,
    QTV_STILL_IMAGE_CODEC,
    QTV_TIMED_TEXT_CODEC,
    QTV_MP3_CODEC,
    QTV_H264_CODEC,
    QTV_WMA_CODEC,
    QTV_WMV1_CODEC,
    QTV_WMV2_CODEC,
    QTV_WMV3_CODEC,
    QTV_WMA_PRO_CODEC,
    QTV_WMA_PRO_PLUS_CODEC,
    QTV_COOK_CODEC,
    QTV_SIPR_CODEC,
    QTV_RV30_CODEC,
    QTV_RV40_CODEC,
    QTV_CONCURRENT_CODEC,
    QTV_AMR_WB_CODEC,
    QTV_AMR_WB_PLUS_CODEC,
    QTV_NONMP4_MP3_CODEC,
    QTV_QCP_CODEC,
    QTV_NONMP4_AMR_CODEC,
    QTV_NONMP4_AAC_CODEC,
    QTV_MIDI_CODEC,
    QTV_AC3_CODEC,
    QTV_PCM_CODEC,
    QTV_UNKNOWN_CODEC
  };

  static CodecType GetAudioCodecType(InstanceHandleT handle = NULL);

  static CodecType GetVideoCodecType(InstanceHandleT handle = NULL);

  /* The following methods are for KDDI Telop Timed Text only:
   * Returns the next telop element (not including substrings) */
  static ReturnT GetNextTelopElement(TelopElementT &telopElement, InstanceHandleT handle = NULL);
  /* Returns a pointer to the specified SubString of the current Telop Element */
  static bool GetTelopSubString(TelopSubStringT *pUserSubStr, int32 index, InstanceHandleT handle = NULL);

  /* The following methods are for LGT */
  /* LGT requires that the User-Agent header sent in RTSP messages from the
     client to the server contain additional fields describing the capability
     of the agent.  Most of these fields are known by Qtv and can be filled
     in automatically.  However, the manufacturer code (man) and  handset
     CTN (ctn) must be provided once (only) to the software after a reboot,
     before the first RTSP messages are generated. */
  static ReturnT SetUserAgentParameters(const char * man, const char * ctn, InstanceHandleT handle = NULL);

  /* The following methods are for SKT */
  /* SkT requires that the User-Agent header sent in RTSP messages from the
     client to the server contain additional fields describing the capability
     of the agent. params should end with NULL.*/
  static ReturnT SetUserAgentParameters(const char* params, InstanceHandleT handle = NULL);

/* Override the default value of 'QtvPlayer' which is sent in all RTSP
     messages to the streaming server for the value of the User-Agent header:
       User-Agent: QtvPlayer */
  static ReturnT SetUserAgent(const char * userAgentName, InstanceHandleT handle = NULL);

  static ReturnT RegisterQTVInstance( InstanceHandleT handle,InstancePriorityT priority );

  /*--------------------------------------------------
     Audio/Video statistics
    --------------------------------------------------*/

  struct AudioVideoStatisticsT
  {
    /* Video stats */
    struct
    {
      uint32 format;

      /* encoded video frame rate */
      uint32 encodedFrameRate;

      /* encoded image bitrate */
      uint32 encodedBitrate;

      /* total video frames processed from the clip. */
      int32 frames;

      /* total video frames interpolated for the clip */
      int32 framesInterpolated;

      /* total video frames that were neither decoded nor displayed due to cpu
       * time constraints or command interruptions. */
      int32 skippedDecode;

      /* total video frames that were decoded but not displayed due to cpu time
       * constraints or command interruptions. */
      int32 skippedDisplay;

      /* total video frames that could not be decoded, probably due to errors in
       * the data. */
      int32 failedDecode;

      /* max number of consecutive non-displayed video frames. */
      int32 maxDrop;

      /* decoded frame rate: (frames - skippedDecode)/time */
      uint32 decodedFrameRate;

      /* displayed frame rate: (frames - skippedDecode - skippedDisplay)/time */
      uint32 displayedFrameRate;

      /* frames dropped due to AV sync */   
      int32 avSyncDrops;      
      
      /* total video frames that were decoded but not displayed due to heavy macroblock errors */
      int32 skippedDisplayDueToErrs;

      /*video data lead*/
      int32 maxDataLead;
      int32 minDataLead;


      /*Count of different types of frames*/
      uint32 nIFrameTally; // Count of I Frames 
      uint32 nPFrameTally; // Count of P Frames
      uint32 nBFrameTally; // Count of B Frames

      /*Total B-Frames Received*/
      int32 nBFramesReceived;      
    } Video;

    /* Audio stats */
    struct
    {
      uint32 format;

      /* encoded audio bitrate */
      uint32 encodedBitrate;

      /* total audio samples processed. */
      int32 frames;

      /* total number of null-filled audio samples, due to lost data. */
      int32 nullSamples;

      /* max number of consecutive null audio samples. */
      int32 maxDrop;

      /* max audio data lead information */     	
	 int32 maxAudioDataLead;

      /* min audio data lead information */     	
	 int32 minAudioDataLead;
	  
    } Audio;

    /* Text stats*/
    struct TimedTextStatisticsT
    {
      int32 iMaxTextDataLeadStat;
      int32 iMinTextDataLeadStat;
    } Text;

    /* Audio / Video Sync stats */
    struct
    {
      /* true if the rest of the AVSync data is valid */
      boolean avSyncValid;

      /* amount the video was ahead of the audio at the end of playback, or zero
       * if info not available. */
      uint32 videoAheadMsec;

      /* amount the video was behind of the audio at the end of playback, or
       * zero if info not available. */
      uint32 videoBehindMsec;
    } AVSync;
  };


  /*--------------------------------------------------
     RTP statistics
    --------------------------------------------------*/

  enum MediaTrackT
  {
    UNUSED_TRACK = 0,
    VIDEO_BASE_TRACK,
    VIDEO_ENHANCEMENT_TRACK,
    AUDIO_TRACK,
    TEXT_TRACK,
    UNKNOWN_TRACK
  };

  /* RTP info maintained per-track */
  struct TrackDataT
  {
    MediaTrackT TrackType;
    /* ms (nominal) separation between I-Frames (key) in video source */
    int32       VideoIFrameInterval;
    /* number of lost RTP packets thus far in stream */
    int32       LostPackets;
    /* number of RTP packets currently buffered */
    int32       PacketsInBuffer;
    /* total number of RTP packets received thus far */
    int32       PacketsReceived;
    /* average value of the delta between time of receipt of packets.
       Time is in ms, averaged over all packets received thus far */
    int32       NetworkJitter;
    /* how many frames per second */
    double      VideoFrameRate;
    /* the bps of the media */
    int32       MediaDataRate;
  } ;

  /* RTP streaming statistics */
  struct RTPStatisticsT
  {
    int32       NumTracks;
    TrackDataT  TrackData[QTV_MAX_MEDIA_TRACKS];

  } ;

  /*--------------------------------------------------
     API Methods.
    --------------------------------------------------*/

  /* Methods to get information about the current playback */
  static ReturnT GetAudioVideoStatistics(AudioVideoStatisticsT &statistics, InstanceHandleT handle = NULL);
  static ReturnT GetRTPStatistics(RTPStatisticsT &statistics, InstanceHandleT handle = NULL);

  /* Set the range of UDP ports that Qtv can attempt to use when setting up
     an RTP connection in order to stream audio or video from a server.  The
     default range is set in QtvStream.h  The function enforces that the
     range start with an even port number and end with an odd port number to
     meet the requirements of RTP, RFC1889. Changes to the range take effect
     with the next attempt to make an RTP connection. */
  static void SetDataPortRange(int beginPort, int endPort);


  /* MIME Type
     Returns the MIME type QTV_PLAYER_MIME_TYPE ("video/mp4")
     if the file is a media type that can be played by
     the Qtv Player, or NULL, if it is an unknown type. This
     function does not work on streaming files (.URL files) */
  static const char *GetMIMEType(const char *file);
  static const char *GetMIMEType(unsigned char *buffer, int length);

  /* Returns the MIME type based buffer content */
  static const char *GetMIMEType(unsigned char *pBuf, uint32 dwBufSize, uint32 wPtrOffset);

  /* Returns the MIME type based upon the file extension */
  static const char *GetMediaType(const char *file);

  /*------------------------------------------------------------------------
   Returns a pointer to a QtvPlayer object from and instance handle.
  ------------------------------------------------------------------------*/
   static QtvPlayer *GetPlayer(InstanceHandleT handle);

  enum TrackType
  {
    VideoTrack,
    AudioTrack,
    TextTrack
  };

  /* gets the time scale of the given track */
  static ReturnT GetTrackTimeScale(TrackType trackType, uint32 *pTimeScale, InstanceHandleT handle = NULL);

  /* sets/resets the loop track flag */
  static ReturnT SetLoopTrack(bool bLoop, InstanceHandleT handle = NULL);

  struct DRMKey
  {
    void   *pKey;            // DRM Key
    uint32  dwSize;          // size of key
  };
  static ReturnT SetDRMKey(void *pKey, uint32 keySize, InstanceHandleT handle = NULL);

 /*-------------------------------------------------------------------------
    DRM decryption function type (Implimented by OEM).
  -------------------------------------------------------------------------*/
  typedef boolean (*DRMDecryptMethodT)
    (
      uint32     TrackId,
      void      *pEncryptedDataBuf,   /* pointer to encrypted data buffer, which has to be decrypted */
      void      *pDecryptedDataBuf,   /* pointer to destination buffer to copy decrypted data,
                                         OEM is resposible for copying the decrypted data  */
      uint32    wEncryptedDataSize,   /* encrypted data buffer size */
      uint32    *pDecryptedDataSize,   /* pointer to decrypted data buffer size,
                                         OEM is resposible for copying the decrypted data size  */
      void      *pClientData          /* client data provided by OEM when registering callback */
    );

  /* API to register DRM decryption method, OEM has to impliment the decryption method */
  static ReturnT RegisterDRMDecryptMethod( DRMDecryptMethodT pDecryptFunction, void *pClientData, InstanceHandleT handle = NULL );

  /* Generic interface: Media types, including network */
  typedef enum {
    MEDIA_INFO_TYPE_BCAST_FLO = 0,
    MEDIA_INFO_TYPE_NUM       = 1
  } Mpeg4MediaType;

  /* Generic interface: Media info struct */
  typedef struct {
    Mpeg4MediaType type;     /* Identifies the contents of of info below */
    uint32             size; /* # of bytes pointed to by info, for versioning */
    byte               info[1]; /* Var length media info, identified by type */
  } Mpeg4MediaInfo;

  static ReturnT SetMediaInfo(void *pInfo, 
                              uint32 nSize, 
                              InstanceHandleT handle = NULL);

  struct RawFrameInfo
  {
    uint32 FrameNum;      /* sample number (first sample is zero)               */
    uint32 FrameSize;     /* size of sample in bytes                            */
    uint32 FrameOffset;   /* offset of sample in stream, in bytes               */
    uint32 FrameTime;     /* composition time of sample, in msec                */
    uint32 Framedelta;    /* difference between composition time of this sample */
                          /* and the next sample, in msec                       */
    uint32 IsSyncFrame;   /* Indication if sample is a random access point      */
                          /* (I-Frame)(non-zero) or not (zero)                  */
  };

  /* gets information of the closest frame of the given time (in MSec) */
  static ReturnT GetRawFrameInfo(TrackType trackType, uint32 time, RawFrameInfo *pFrameInfo, InstanceHandleT handle = NULL);

  /* gets Sync Frame (I-Frame) information closest to the given frame number
     in reverse or forward direction */
  static ReturnT GetNextRawSyncFrameInfo(uint32 FrameNum, bool reverse, RawFrameInfo *pFrameInfo, InstanceHandleT handle = NULL);

  /* gets size of the largest frame of the given track.
     May return QTV_RETURN_ERROR, if track not found. */
  static ReturnT GetLargestRawFrameSize(TrackType trackType, uint32 *pFrameSize, InstanceHandleT handle = NULL);

  /* gets the header of the track. User should pass "pBufSize" with the size of
    supplied buffer. If size of supplied buffer is not sufficient, it will return
    QTV_RETURN_BUFFER_UNDERRUN with "pBufSize" having header size. In case of any
    other error, this will return QTV_RETURN_ERROR with pointers untouched.
    In case of success, this will return QTV_RETURN_OK and "pBufSize" will have
    header length. */
  static ReturnT GetRawTrackHeader(TrackType trackType, byte *pBuf, uint32 *pBufSize, InstanceHandleT handle = NULL);

  /* gets number of frames in the given track.
     May return QTV_RETURN_ERROR, if track not found. */
  static ReturnT GetNumFrames(TrackType trackType, uint32 *pNumFrames, InstanceHandleT handle = NULL);

  /* gets the frame of a given track by frame number and also gets frame info */
  static ReturnT GetRawFrame(TrackType trackType, uint32 FrameNum, RawFrameInfo *pFrameInfo, byte *pBuf, uint32 bufSize, InstanceHandleT handle = NULL);

  /* if telop is present in the opened clip, this will initialize the text player, so
     that it can process telops */
  static ReturnT InitializeTelopElements(InstanceHandleT handle = NULL);
  /* prepares the next telop elements, so other telop functions can be use to access
     the telop element. This function should be used only for video editing. Calling
     this function during playback will cause wrong telop display */
  static ReturnT SetupNextTelopElement(InstanceHandleT handle = NULL);


   /*
        When oem-defined RTSP headers are defined using EditOemRtspHeaders(),
      this enumeration controls to which method the headers will be appended.
      The list is powers of two, so that multiple methods can be specified
      by ORing together values from the enumeration.
        If values outside of these bits are passed into the Edit function,
      they will be ignored.
        NOTE: if change the values in this table, update corresponding
      function comment in OEMMediaMPEG42PV.cpp, OEMEditOemRtspHeaders()
   */
   enum AffectedRTSPMethod {
       RTSP_METHOD_NONE             =  0,
       RTSP_METHOD_DESCRIBE         =  1,
       RTSP_METHOD_SETUP            =  2,
       RTSP_METHOD_PLAY             =  4,
       RTSP_METHOD_PAUSE            =  8,
       RTSP_METHOD_TEARDOWN         = 16,
       RTSP_METHOD_OPTIONS_RESPONSE = 32,
       // all of the above
       RTSP_METHOD_ALL              =
         RTSP_METHOD_DESCRIBE |
         RTSP_METHOD_SETUP    |
         RTSP_METHOD_PLAY     |
         RTSP_METHOD_PAUSE    |
         RTSP_METHOD_TEARDOWN |
         RTSP_METHOD_OPTIONS_RESPONSE
    };

    /*
      What operation should be performed on the Oem Rtsp headers?
    */
    enum OemRtspHeaderCommand {
       OEM_RTSP_HEADER_DELETE_ALL = 0,
       OEM_RTSP_HEADER_DELETE     = 1,
       OEM_RTSP_HEADER_ADD        = 2,
       OEM_RTSP_HEADER_REPLACE    = 3,
       // limit entries
       OEM_RTSP_HEADER_FIRST_CMD  = 0,
       OEM_RTSP_HEADER_LAST_CMD   = OEM_RTSP_HEADER_REPLACE
    };

    /*
      What is the result of editing?
    */
    enum OemRtspHeaderResult {
       OEM_RTSP_RESULT_OK          = 0, // no problem
       OEM_RTSP_RESULT_TOO_MANY    = 1, // all headers already used
       OEM_RTSP_RESULT_NOT_FOUND   = 2, // request to replace non-existent
       OEM_RTSP_RESULT_BAD_DATA    = 3, // methods, name or value is empty
       OEM_RTSP_RESULT_NO_MEMORY   = 4, // unable to alloc memory for item
       OEM_RTSP_RESULT_BAD_COMMAND = 5  // command not OemRtspHeaderCommand
    };

    enum OemHttpHeaderResult {
       OEM_HTTP_RESULT_OK          = 0, // no problem
       OEM_HTTP_RESULT_TOO_MANY    = 1, // all headers already used
       OEM_HTTP_RESULT_NOT_FOUND   = 2, // request to replace non-existent
       OEM_HTTP_RESULT_BAD_DATA    = 3, // methods, name or value is empty
       OEM_HTTP_RESULT_NO_MEMORY   = 4, // unable to alloc memory for item
       OEM_HTTP_RESULT_BAD_COMMAND = 5  // command not OemRtspHeaderCommand
    };

    /*
      Request to edit the list of oem-defined rtsp headers
      Example:
      EditOemRtspHeaders(
        OEM_RTSP_HEADER_ADD, RTSP_DESCRIBE|RTSP_PLAY, "what", "gives");
      This will result in an rtsp header line of:
        what: gives<cr><lf>
    */
    static OemRtspHeaderResult EditOemRtspHeaders(
       OemRtspHeaderCommand whatCommand,
       // remainder are don't care for delete, delete all
       uint32 whatMethods, // entries from AffectedRTSPMethod
       const char *headerName,
       const char *headerValue,
       InstanceHandleT handle = NULL
    );

    static ReturnT AddOemHttpHeaders(
       const char *headerName,
       const char *headerValue,
       OemHttpHeaderResult &err,
       InstanceHandleT handle = NULL
    );

    // setting unhandled header callbacks
    static void SetUnhandledRTSPHeaderCallback(UnhandledStreamingItemCallbackT, InstanceHandleT handle = NULL);
    static void SetUnhandledSDPCallback(UnhandledStreamingItemCallbackT, InstanceHandleT handle = NULL);
    // accessing unhandled header callbacks
    static UnhandledStreamingItemCallbackT GetUnhandledRTSPHeaderCallback(InstanceHandleT handle = NULL);
    static UnhandledStreamingItemCallbackT GetUnhandledSDPCallback(InstanceHandleT handle = NULL);

    static ReturnT GetNetPolicyInfo(NetPolicyInfo* netPolicyInfo, InstanceHandleT handle = NULL);
    static ReturnT SetNetPolicyInfo(NetPolicyInfo* netPolicyInfo, InstanceHandleT handle = NULL);

  static ReturnT SetDescramblerHandler(void *descrambler,
                                        InstanceHandleT handle = NULL);

  static uint32 GetMidiDataSize(InstanceHandleT handle = NULL);
  static uint32 GetMidiData(uint8 *pBuf, uint32 size, uint32 offset, InstanceHandleT handle = NULL);

  static uint32 GetLinkDataSize(InstanceHandleT handle = NULL);
  static uint32 GetLinkData(uint8 *pBuf, uint32 size, InstanceHandleT handle = NULL);

  /*
    Returns the specified data-type data size OR
    0 if the specified data type is not present.
  */
  static uint32 GetDataSize(DataT dType, uint32 offset=0, InstanceHandleT handle = NULL);
  /*
     Copies the specified data-type data into supplied buffer (pBuf),
     starting from the specified 'offset' (offset value of 0 copies the whole atom
     into the supplied buffer). Returns the acutal bytes copied into the buffer OR
     0 if the specified atom is not present.
  */
  static uint32 GetData(DataT dType, uint8 *pBuf, uint32 size, uint32 offset=0, InstanceHandleT handle = NULL);
  static ReturnT ModifyBufferLowMark(int32 modified_buffer_lowmark, InstanceHandleT handle = NULL);

  /***************************************************************************
  ** Frame-buffer deallocation                                          GRB **
  **                                                                        **
  ** When the OEM layer has completed rendering a video frame buffer,       **
  ** it calls this member to notify the player that it can release          **
  ** (or reuse) the buffer.                                                 **
  **                                                                        */

  static void ReleaseCurrentVideoFrameBuffer( void *pBuffer, InstanceHandleT handle = NULL);

  /*                                                                    GRB **
  ***************************************************************************/
  /* for logging QTV commands */
  static void LogQtvCmd(const char *cmd ,...);
  static bool CopyBufferToFile(char * fullpathtoFilename = NULL, bool ow = true, InstanceHandleT handle = NULL);

  /* ======================================================================
  FUNCTION:
    SelectPlaybackTracks

  DESCRIPTION:
    It selects the playble tracks from a opened local file or progressive
    download / pseudo stream session. Not applicable for regular streaming.
    It will only succeed if QTV is in READY/IDLE state (not playing).

  INPUT/OUTPUT PARAMETERS:
    dwTrackFlag - bitwise flags for tracks to be selected
    #define QTV_NO_TRACK_SELECTED             0x00
    #define QTV_VIDEO_TRACK_SELECTED          0x01
    #define QTV_AUDIO_TRACK_SELECTED          0x02
    #define QTV_TEXT_TRACK_SELECTED           0x04

  RETURN VALUE:
    QtvPlayer ReturnT type

  SIDE EFFECTS:
    a call to OpenURN will reset these values and flags passed in OpenURN
    will be valid.
  ======================================================================*/
  static ReturnT SelectPlaybackTracks(uint32 dwTrackFlag, InstanceHandleT handle = NULL);

  /*WRAPPER FUNCTIONS TO BE INVOKED TO ACCESS THE QTVCONFIGURATION ITEMS*/
  static bool SetQTVConfigItem( QtvConfig::QtvConfigItemId item_id, int* value, InstanceHandleT handle = NULL );
  static bool GetQTVConfigItem( QtvConfig::QtvConfigItemId item_id, int* value, InstanceHandleT handle = NULL );

  static uint32 GetTotalRxDataRate(InstanceHandleT handle = NULL);

/* ======================================================================
FUNCTION:
  ReadPlaybackTracks

DESCRIPTION:
  This Synchronous method can be used to read the audio, video and text track 
  information. Track information returned contains the track identifier, 
  media type and codec type. Client has to enquire for the 
  track count first. This is done by passing null pointer in the second argument. 
  Subsequently client has to allocate the memory for the whole track list and 
  passed valid pointer in the second argument to obtain the entrie track list 
  information. Only base layers are reported. i.e enhanced layers are not reported. 


INPUT/OUTPUT PARAMETERS:
  handle     (I)      - Qtv Player Instance Handle
  nTrackCount(O) - Track count. 
  pTrackList (O)    - Valid Track List Information pointer. 

RETURN VALUE:
  QTV_RETURN_OK    - Successful 
  QTV_RETURN_MULTIPLE_INSTANCE_RUNNING - This primitive is only supported for default player instance. 
  QTV_RETURN_INVALID_PARAMETER - Null Track count pointer passed 
  QTV_RETURN_DATA_NOT_AVAILABLE - Track List information not yet available. Media is not yet initialized.
  QTV_RETURN_ERROR - Other General Error. 

SIDE EFFECTS:
  None. 
======================================================================*/
static ReturnT ReadPlaybackTracks(uint32 *nTrackCount, 
                                       TrackListT *pTrackList,
                                       InstanceHandleT handle = NULL);

/* ======================================================================
FUNCTION:
  MutePlaybackTracks

DESCRIPTION:
  This asynchronous method is used to mute/un-mute the selected audio/video/text 
  tracks of the player.  QTV will stop pulling data for muted tracks. Unmute will resume 
  pulling data from the media. 

  For Live Broadcast streams all the tracks can be muted Whereas for recorded streams
  this can not be done. This is indicated through special callback staus. 

INPUT/OUTPUT PARAMETERS:
  handle(I)   - Qtv Player Instance Handle

  bmTrackSelected(I) - Track selection bit mask
  #define QTV_VIDEO_TRACK_SELECTED  0x01
  #define QTV_AUDIO_TRACK_SELECTED  0x02
  #define QTV_TEXT_TRACK_SELECTED   0x04

  eAction(I)   - Whether to Mute the track or not. 
  #define QTV_TRACK_MUTE            0x01
  #define QTV_TRACK_UNMUTE          0x02

RETURN VALUE:
  QTV_RETURN_OK - Command Successful
  QTV_RETURN_MULTIPLE_INSTANCE_RUNNING - Operation supported only for default instance.
  QTV_RETURN_INVALID_PARAMETER - Invalid Parameter
  QTV_RETURN_ERROR - Other general errors. 

SIDE EFFECTS:
  Following ASynchronous events shall be posted by the QTV through 
  the status callbacks registered: 
  + QTV_PLAYER_COMMAND_MUTE_COMPLETE
  + QTV_PLAYER_COMMAND_MUTE_FAILED
  + QTV_PLAYER_COMMAND_UNMUTE_COMPLETE 
  + QTV_PLAYER_COMMAND_UNMUTE_FAILED
  + QTV_PLAYER_COMMAND_MUTE_ALL_FAILED

======================================================================*/
static ReturnT MutePlaybackTracks(uint32  bmTrackSelected, 
                                 MuteActionT eAction,
                                 InstanceHandleT handle = NULL);

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION:
  SelectPlaybackTracks

DESCRIPTION:
  Asynchronous method used to select the new set of tracks from which to pull 
  data. First parameter denotes the media combination and the remaining
  are the track identifiers which are to be selected. 

  Note:- This method is valid before the player started running. Once the 
  player started running, this method can not be executed. In IDLE state 
  you can not execute this method since the media is not yet initialized. 
  Once the player is initialized it moves from opening to PB_READY without 
  user intervention. Hence this method can be called only when the player 
  state is PB_READY.  It is better to call this from the state change 
  callback. 

INPUT/OUTPUT PARAMETERS:
  handle         (I) - Qtv Player Instance Handle
	
  bmTrackSelected(I) - Track selection bit mask
  #define QTV_VIDEO_TRACK_SELECTED          0x01
  #define QTV_AUDIO_TRACK_SELECTED          0x02
  #define QTV_TEXT_TRACK_SELECTED           0x04
	
  nAudioTrackID  (I) - Audio Track Identifier 
  nVideoTrackID  (I) - Video Track Identifier 
  nTextTrackID   (I) - Text Track Identifier 	

RETURN VALUE:
  QTV_RETURN_OK - Successful. 
  QTV_RETURN_MULTIPLE_INSTANCE_RUNNING - Operation supported only for default player instance.
  QTV_RETURN_INVALID_PARAMETER  - Invalid Parameter. 
  QTV_RETURN_ERROR - General Error 

SIDE EFFECTS:
  Following Asynchronous events shall be posted to denote command execution result: 
  + QTV_PLAYER_COMMAND_SELECT_PB_TRACK_COMPLETE
  + QTV_PLAYER_COMMAND_SELECT_PB_TRACK_FAILED
======================================================================*/
static ReturnT SelectPlaybackTracks(uint32 bmTrackSelected, 
                                    uint32 nAudioTrackID,
                                    uint32 nVideoTrackID, 
                                    uint32 nTextTrackID,
                                    InstanceHandleT handle=NULL);

/* ======================================================================
FUNCTION:
  Reposition

DESCRIPTION:
  This is an Asynchronous method. This is used to move the media cursor
  to the absolute time offset specified while the player is paused. 
	
INPUT/OUTPUT PARAMETERS:
  handle(I)          - Qtv Player Instance Handle
  nTimestamp(I)  - Absolute time stamp
  uTransID (O)    - Transaction identifier returned to the caller

RETURN VALUE:
  QTV_RETURN_OK - Primitive is successfully queued.  
  QTV_RETURN_INVALID_PARAMETER - Invalid Parameter
  QTV_RETURN_MULTIPLE_INSTANCE_RUNNING - Operation supported only on default instance.
  QTV_RETURN_ERROR - Error occured while queuing the primitive. 

SIDE EFFECTS:
  Following ASynchronous events shall be posted by the QTV through 
  the status callbacks registered: 
  + QTV_PLAYER_COMMAND_REPOSITION_DONE 
    This event carries the new media cursor time offset. 
  + QTV_PLAYER_COMMAND_REPOSITION_FAILED
    
======================================================================*/
static ReturnT Reposition(uint32 nTimestamp,
                          uint32 *uTransID,
                          InstanceHandleT handle=NULL);

/* ======================================================================
FUNCTION:
  SeekToSync

DESCRIPTION:
  This is an Asynchronous method. This is used to move the media cursor
  in a relative manner by specified number of sync frames. This movement 
  could be either in the forward or in the backward direction.  This method is 
  not applicable to live broadcast streams.
  Pre-condition for the successful execution of this method is that the 
  player must be paused.  If the first I-frame is to the right of the 
  media cursor , then negative offsets position the cursor at the first 
  I-frame. If the last I-frame is to the left of the media cursor then 
  positive offsets move the cursor to the last I-frame. If the cursor 
  is already positioned at the first/Last sync frame , then Prev/Next
  Sync operations results in error respectively. If the offset is zero 
  and cursor is not on a I-frame then it will move to the previous
  sync frame if present. Otherwise it moves to the first I-frame. 
  If the cursor is already on a sync frame then zero offset is equivalent 
  to a No-OP. 
	
INPUT/OUTPUT PARAMETERS:
  handle(I)         - Qtv Player Instance Handle
  nSyncOffset(I) - Sync Offset 
  uTransID(O)    - Transaction Identifier returned to the caller. 

RETURN VALUE:
  QTV_RETURN_OK - Primitive is successfully queued. 
  QTV_RETURN_MULTIPLE_INSTANCE_RUNNING - Operation supported only on default instance.
  QTV_RETURN_INVALID_PARAMTER - Invalid Parameter
  QTV_RETURN_ERROR - Error occured while queuing the primitive. 

SIDE EFFECTS:
  Following ASynchronous events shall be posted by the QTV through 
  the status callbacks registered: 
  + QTV_PLAYER_COMMAND_SEEK_SYNC_DONE 
    This event carries the new media cursor time offset 
  + QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED
    
======================================================================*/
static ReturnT SeekToSync(sint31 nSyncOffset,
                          uint32   *uTransID,
                          InstanceHandleT handle=NULL);

/* ======================================================================
FUNCTION:
  PlaySyncFrame

DESCRIPTION:
  ASynchronous method used to move the video track by specified number 
  of Sync frames in both directions while the player is paused. It is different
  from the seekSync method in a sense that this method shall display the current
  I-frame also. This method is not applicable to live broadcast streams.  
  This method is equivalent  the seekSync followed by playNextFrame while the 
  player is paused. 
	
INPUT/OUTPUT PARAMETERS:
  handle(I)         - Qtv Player Instance Handle
  nSyncOffset(I) -  Sync Offset
  uTransID(O)    - Transaction Identifier returned to the caller

RETURN VALUE:
  QTV_RETURN_OK    - Successfully queued. 
  QTV_RETURN_MULTIPLE_INSTANCE_RUNNING - Operation supported on default instance only.
  QTV_RETURN_INVALID_PARAMETER - Invalid Parameter
  QTV_RETURN_ERROR - Couldnt queue the primitive 

SIDE EFFECTS:
  Following ASynchronous events shall be posted by the QTV through 
  the status callbacks registered: 
  + QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_DONE 
    This event carries the new media cursor time offset 
  + QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED
  
======================================================================*/
static ReturnT PlaySyncFrame(sint31 nSyncOffset,
                             uint32 *uTransID,
                             InstanceHandleT handle=NULL);

// Get Media Info returns the user information returned by the media source 
// back to the application. This takes in the output parameters such as 
// pInfo & size. Memory for the media information shall be allocated in the
// OEM space. 
static ReturnT GetMediaInfo(void **ppInfo,
                            unsigned int *pnSize, 
                            InstanceHandleT handle=NULL);
#endif /* FEATURE_QTV_GENERIC_BCAST */

  /* Playback speed settings */
  enum PlaybackSpeedT
  {
    PLAYBACK_SPEED_NORMAL  /* Normal speed (default) */
    ,PLAYBACK_SPEED_1P3X   /* 1.3x speed */
  };

  /* ======================================================================
  FUNCTION
    QtvPlayer::SetPlaybackSpeed

  DESCRIPTION
    Sets the playback speed. This setting is applicable only to the next
    PlayClip command.

  PARAMETERS
    PlaybackSpeedT pbSpeed
      The desired playback speed.
    InstanceHandleT handle
      Handle to the instance to which this change is applicable

  RETURN VALUE
    QtvPlayer::ReturnT
      Enum of the Qtv return type.
  ========================================================================== */
  static ReturnT SetPlaybackSpeed(PlaybackSpeedT pbSpeed, 
                                  InstanceHandleT handle = NULL);

  enum DualMonoOutputT
  {
    DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R = 0, /* main channel to left and right */
    DUAL_MONO_OUTPUT_SUB_TO_L_AND_R,      /* sub channel to left and right */ 
    DUAL_MONO_OUTPUT_MAIN_TO_L_SUB_TO_R,  /* main channel to left, sub channel to right */
    DUAL_MONO_OUTPUT_SUB_TO_L_MAIN_TO_R   /* sub channel to left, main channel to right */
  };

  /* ======================================================================
  FUNCTION
    QtvPlayer::SetDualMonoOutput

  DESCRIPTION
    Sets the dual mono output configuration.

  PARAMETERS
    DualMonoOutputT dualMonoOutput
      The desired dual mono output configuration.
    InstanceHandleT handle
      Handle to the instance to which this change is applicable

  RETURN VALUE
    QtvPlayer::ReturnT
      Enum of the Qtv return type.
  ========================================================================== */
  static ReturnT SetDualMonoOutput(DualMonoOutputT dualMonoOutput, 
                                   InstanceHandleT handle = NULL);

typedef union
{
  VDEC_FRE_ENABLE  freEnable;
} QtvVDECParameterDataT;

enum QtvVDECParameterIdT
{
  QTV_VDEC_PARM_FRE_ENABLE_ID = VDEC_PARM_FRE_ENABLE
}; 

/* ======================================================================
FUNCTION
  QtvPlayer::SetVDECParameter

DESCRIPTION
  Sets the VDEC parameter

PARAMETERS
  QtvVDECParameterIdT inputParamId
    input VDEC parameter id
  QtvVDECParameterDataT* pInputParam
    pointer to input VDEC parameter value

RETURN VALUE
  QtvPlayer::ReturnT
    Enum of the Qtv return type.
========================================================================== */
static ReturnT SetVDECParameter( QtvVDECParameterIdT inputParamId,  QtvVDECParameterDataT* pInputParam, InstanceHandleT handle = NULL);

#ifdef FEATURE_WINCE
#error code not present
#endif
static void setSpeed(float speed,InstanceHandleT handle = NULL);
static void setAccDuration(int time,InstanceHandleT handle = NULL);
 void register_qtv_with_callmgr();
private:

  friend class QtvPlayerDispatch;

  // Private Init function.   Called internally when FEATURE_REAL_PLAYER_USE_QTV_API is defined
  ReturnT Init(MallocOutputBufferT, FreeOutputBufferT, const char *mediaType);

  /*------------------------------------------------------------------------
   Private QtvPlayer constructor.  To create a QtvPlayer
   instance, please call Init instead.
  ------------------------------------------------------------------------*/
  QtvPlayer(void);

  /*------------------------------------------------------------------------
   Private QtvPlayer destructor.  This is called internally from
   QtvPlayer::Terminate.
  ------------------------------------------------------------------------*/
   ~QtvPlayer(void);

  /*------------------------------------------------------------------------
   Method to seek to a clip in a playlist

   Arguments:

   playlistName: For server-side playlists, some server may allow the playlist
                 to be changed. If so, this is relative or absolute playlist
                 filename.
   clipIndex: Either the absolute value of the clip to seek to if
              isClipIndexRelative is false, or a relative value (with respect
              to the currently playing clip), if isClipIndexRelative is false.
   time:      The absolute value of the time within the requested clip.
   when:      When during the duration of the currently playing clip to
              transition to the next clip. -1 means now, -2 means when the
              currently playing clip has finished.
   preferClientPlaylistSkip: If we are inside of a server-side playlist
                             which is inside a client-side playlist, and this
                             flag is true, we will interpret the skip as within
                             the client-side playlist. Otherwise we will
                             interpret the seek as within the server-side
                             playlist.
  -------------------------------------------------------------------------*/
  static ReturnT PlaylistPlayClip(const char *playlistName,
                                  int32 clipIndex, bool isClipIndexRelative,
                                  int32 time, int32 when,
                                  bool preferClientPlaylistSkip,
                              InstanceHandleT handle = NULL);

  /* Function used internally to set the priority of the instance*/
  static ReturnT SetPriority(InstanceHandleT handle, InstancePriorityT priority);

  /* Function used internally to check if any other instance exists whose priority 
     is higher than the current instance priority */
  static ReturnT CheckInstances(InstanceHandleT handle);

  /* Function used internally to suspend all the low priority instances 
  ** via the concurrency manager */
  static ReturnT SuspendLowPriorityInstance(InstanceHandleT handle);

  /*Function used internally to register a QtvPlayer inst within the 
  ** QtvInstances[] lookup array. The instance gets registered within OpenURN,  
  ** if its the only instance running at the specified "prioriy" value (Qtv does  
  ** not allow two inst to run at the same priority) AND there are no other 
  ** higher priority instances previously registered. This instance of Qtv also 
  ** gets registered with the concurrency manager, if its not the highest priority 
  ** instance ("priority" set to QTV_PRIORITY_HIGHEST when calling OpenURN) */
  static ReturnT RegisterInst( InstanceHandleT handle,
                               InstancePriorityT priority );


  /*Function used internally to rollback Init() effect within open like routines
  ** such as open pseudo stream , open push buffered stream or pull buffered 
  ** stream.
  */
  void InitRollback( );

  /*This variable is for internal usage, to reference count the IMEM/Heap resource*/
  static uint32 guQtvPlayerResourceRefCnt;

  /* This variable is for internal usage, to track the total number of
  ** created instances */
  static uint32 numQtvPlayers;

  /* This variable is for internal usage, to track the total number of
  ** registered instances */
  static uint32 numRegisteredQtvPlayers;
#ifdef FEATURE_QTV_GENERIC_BCAST
  /* Transaction identifier withing the qtvPlayer object */
  static uint32 uTransID;
#endif

  /* SetDRMKey will save the DRM key and key size in these local variables.
  ** When OpenURN is called, Mpeg4Player is created and these values are 
  ** given to the Mpeg4Player before PV_OPEN_CONTENT_URN is posted to it.
  */
  void * m_DRMKey;
  uint32 m_DRMKeySize;

  PlaybackSpeedT m_playbackSpeed;

  // Storage for user agent
  char m_UserAgentStr[QTV_MAX_USER_AGENT_STR_BYTES + 1];
  char m_UserAgentParam_man[QTV_MAX_USER_AGENT_MAN_BYTES + 1];
  char m_UserAgentParam_ctn[QTV_MAX_USER_AGENT_CTN_BYTES + 1];
  char m_UserAgentParam_params[QTV_MAX_USER_AGENT_STR_BYTES + 1];
};

#endif /* _QTVPLAYERAPI_H_ */


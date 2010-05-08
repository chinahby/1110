#ifndef AEEMEDIAMPEG4_H
#define AEEMEDIAMPEG4_H

/*============================================================================
FILE:   AEEMediaMPEG4.h

SERVICES:  BREW Movie File Services

DESCRIPTION:
   This file defines the IMediaMPEG4 interface that is used to read and write
   audio and video stream data in a movie file.
   
PUBLIC CLASSES:  IMediaMPEG4

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

/* =======================================================================
                             Edit History

$Header: //depot/asic/msmshared/services/Qtv/QTV.04.04/StaticExtensions/Inc/AEEMediaMPEG4.h#4 $
$DateTime: 2005/01/05 16:53:08 $
$Change: 153485 $


========================================================================== */

/*-------------------------------------------------------------------
            Include Files
-------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEMedia.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
//
// Status codes
//
// Status code is returned via registered callback function to 
// indicate event status and to return data to client.
// AEEMediaNotify::nStatus sent via callback function contains 
// the following status codes.
//

// MP4 only status
#define MM_MP4_STATUS_3GPP_TTEXT        (MM_STATUS_USER_BASE + 1)
#define MM_MP4_STATUS_TELOP_TEXT        (MM_STATUS_USER_BASE + 2)

// Some presently MP4, BUT POTENTIALLY MEDIA COMMON STATUS
#define MM_MP4_STATUS_BUFFER_UPDATE     (MM_STATUS_USER_BASE + 3)   // indicate Buffering
#define MM_MP4_STATUS_CONNECTION_UPDATE (MM_STATUS_USER_BASE + 4)   // streaming Setup in progress indication 
#define MM_MP4_STATUS_PB_READY          (MM_STATUS_USER_BASE + 5)   // indicate moving to Playback Ready State 
#define MM_MP4_STATUS_OPEN_COMPLETE     (MM_STATUS_USER_BASE + 6)   // indicate open file complete
#define MM_MP4_STATUS_NO_SYNC_FRAME     (MM_STATUS_USER_BASE + 7)   // no sync frame error (seek may fail)
#define MM_MP4_STATUS_RTSP              (MM_STATUS_USER_BASE + 8)   // RTSP status code
#define MM_MP4_STATUS_RESTARTING_VIDEO  (MM_STATUS_USER_BASE + 9)   // restarting video in loop mode
#define MM_MP4_STATUS_RESTARTING_AUDIO  (MM_STATUS_USER_BASE +10)   // restarting audio in loop mode
#define MM_MP4_STATUS_RESTARTING_TEXT   (MM_STATUS_USER_BASE +11)   // restarting text in loop mode
#define MM_MP4_STATUS_PS_BUFFER_UPDATE  (MM_STATUS_USER_BASE +12)   // pseudo stream is in buffering state
#define MM_MP4_STATUS_UNHANDLED_SDP     (MM_STATUS_USER_BASE +13)   // unhandled sdp line
#define MM_MP4_STATUS_UNHANDLED_RTSP    (MM_STATUS_USER_BASE +14)   // unhandled rtsp header
#define MM_MP4_STATUS_AUDIO_ABORT       (MM_STATUS_USER_BASE +15)   // Error in audio track
#define MM_MP4_STATUS_VIDEO_ABORT       (MM_STATUS_USER_BASE +16)   // Error in video track
#define MM_MP4_STATUS_MIDI_AVAILABLE    (MM_STATUS_USER_BASE +17)   // MIDI data is available for user
#define MM_MP4_STATUS_PLAYING           (MM_STATUS_USER_BASE +18)   // Qtv is playing after reposition or 
                                                                    // start of play
#define MM_MP4_STATUS_FILE_SYS_LIMIT    (MM_STATUS_USER_BASE+19)    // File sys limit is approaching
                                                                    // during stream recording
#define MM_MP4_STATUS_FILE_SYS_FULL       (MM_STATUS_USER_BASE+20)  // File sys is FULL
#define MM_MP4_STATUS_FILE_ALREADY_EXISTS (MM_STATUS_USER_BASE+21)  // File already Exists, Cannot overwrite
#define MM_MP4_STATUS_FILE_OW_FAILURE     (MM_STATUS_USER_BASE+22)  // Failed to overwrite a file
#define MM_MP4_STATUS_RECORDER_FAILED     (MM_STATUS_USER_BASE+23)  // Stream recorder failed
#define MM_MP4_STATUS_RECORD_COMPLETED    (MM_STATUS_USER_BASE+24)  // Stream record completed

//
// MP4 only parameters 
//
#define MM_MP4_PARM_ROTATION            (MM_PARM_USER_BASE + 1)		// Rotation parameter
#define MM_MP4_PARM_SCALING             (MM_PARM_USER_BASE + 2)		// Scaling parameter
#define MM_MP4_PARM_TELOP_SUB_STR       (MM_PARM_USER_BASE + 3)

//
// Some presently MP4, BUT POTENTIALLY MEDIA COMMON parameters
//
#define MM_MP4_PARM_BUFFER_UPDATE       (MM_PARM_USER_BASE + 4)  // Indicate Buffering
#define MM_MP4_PARM_CONNECT_UPDATE      (MM_PARM_USER_BASE + 5)  // Indicate Connection Setup to user
#define MM_MP4_PARM_PB_READY            (MM_PARM_USER_BASE + 6)  // Indicate PB Ready
#define MM_MP4_PARM_MEDIA_SPEC          (MM_PARM_USER_BASE + 7)  // Retrieves the MediaSpec

//
// Sub status to indicate that stream requires authentication 
//
#define MM_MP4_STREAM_AUTHENTICATE      (MM_PARM_USER_BASE + 8) 

// Set separate audio and video files 
//   Use this instead of MM_PARM_MEDIA_DATA to use different audio and video 
//   file names.  Param 1 == video, Param 2 == audio, 
//   To disable video, set Param 1 to NULL
//   To disable audio, set Param 2 to NULL 
#define MM_MP4_AUDIO_VIDEO_MEDIA_DATA   (MM_PARM_USER_BASE + 9) 

// ex: IMEDIA_GetMediaParm( pme->m_pMedia,
//                          MM_MP4_PARM_TRACK_TIME_SCALE,
//                          (int32 *)VIDEO_TRACK,
//                          (int32*)&timescale ) 
#define MM_MP4_PARM_TRACK_TIME_SCALE    (MM_PARM_USER_BASE + 10)

// Sets the override value of audio format. If this is set, Audio
//   format for DSP is chosen by this value and audio format of
//   MP4 file is ignored.
//   Param 1 == audio type class ID
#define MM_MP4_PARAM_AUDIO_OVERRIDE     (MM_PARM_USER_BASE + 11)

// Enable/disable RTSP status notification to the BREW App
#define MM_MP4_PARM_RTSP_STATUS_NOTIFY  (MM_PARM_USER_BASE + 12)

// p1 has value true/false
#define MM_MP4_PARM_LOOP_TRACK          (MM_PARM_USER_BASE + 13)

// p1 = *playback time structure
#define MM_MP4_PARAM_PLAYBACK_POS       (MM_PARM_USER_BASE + 14)

#define MMD_BUFFER_FRAGMENT             (MMD_BASE + 2)
#define MM_MP4_PARAM_PSBUF_WRITE_OFFSET (MM_PARM_USER_BASE + 15)
#define MM_MP4_PARM_PS_BUFFER_UPDATE    (MM_PARM_USER_BASE + 16) 

#define MM_MP4_SCALING_UPDATE           (MM_PARM_USER_BASE + 17)

// p1= pointer to AEEMFDecryptKey struct, p2=size
// see below for this definition
//#define MM_MP4_PARM_DECRYPT_KEY         (MM_PARM_USER_BASE + 18)

// Start RTSP stream recording
//p1 = pointer to AEEStreamRecordInfo structure. 
#define MM_MP4_PARM_RTSP_STREAM_CONVERT  (MM_PARM_USER_BASE + 19)

// get midi data size, p1 = pointer to uint32 for size
#define MM_MP4_PARM_MIDI_SIZE           (MM_PARM_USER_BASE + 20)

// get midi data, p1 = buf pointer,
// p2 = pointer to uint32 has size, will have bytes copied
#define MM_MP4_PARM_MIDI_DATA           (MM_PARM_USER_BASE + 21)

// Indicate PB Abort
#define MM_MP4_PARM_PB_ABORT            (MM_PARM_USER_BASE + 22)

//Stop RTSP stream recording
#define MM_MP4_PARM_RTSP_STOP_STR_CONVERT  (MM_PARM_USER_BASE + 23)

// Set Network QoS Policy.
// p1 = pointer to AEENetPolicyInfo structure
// NetPolicy info will take effect only when it is set before 
// moving Mpeg4 IMEDIA object to "Ready" state.
//Ex:IMEDIA_SetMediaParm(pMedia,
//                      MM_MP4_PARM_NET_QOS_POLICY,
//                      (int32)(pmd->qosInfo),
//                      0);
//
// Move Mpeg4 IMEDIA object to "Ready" state
// IMEDIA_SetMediaData((IMedia *)(p),(int32)(pmd))
//
#define MM_MP4_PARM_NET_QOS_POLICY      (MM_PARM_USER_BASE + 24)

// get link data size, p1 = pointer to uint32 for size
#define MM_MP4_PARM_LINK_SIZE           (MM_PARM_USER_BASE + 25)

// get link data, p1 = buf pointer,
// p2 = pointer to uint32 has size, will have bytes copied
#define MM_MP4_PARM_LINK_DATA           (MM_PARM_USER_BASE + 26)

#define MM_MP4_PARM_RECORDED_CLIP_DURATION ( MM_PARM_USER_BASE + 27)
// p1= pointer to AEEMFDecryptKey struct (QTV threats it as void pointer), p2=size
// SKA - special case, please do not change
#define MM_MP4_PARM_DECRYPT_KEY         (MM_PARM_USER_BASE + 200)

/*-------------------------------------------------------------------
            Macros
-------------------------------------------------------------------*/
#define IMEDIA_GetMediaSpec(p, ppSpec, pSize)   \
  IMEDIA_GetMediaParm((IMedia *)(p), MM_MP4_PARM_MEDIA_SPEC, (int32 *)(ppSpec), (int32 *)(pSize))

/*-------------------------------------------------------------------
            Structure Type Declarations
-------------------------------------------------------------------*/

#define MM_MPEG4_VOLUME_STEP                       25

#define MM_MPEG4_MAX_TITLE_BYTES                   50
#define MM_MPEG4_MAX_AUTHOR_BYTES                  50
#define MM_MPEG4_MAX_COPYRIGHT_BYTES              128
#define MM_MPEG4_MAX_RATING_BYTES                 128
#define MM_MPEG4_MAX_DESCRIPTION_BYTES            256
#define MM_MPEG4_MAX_OWNER_BYTES                  128
#define MM_MPEG4_MAX_VERSION_BYTES                128
#define MM_MPEG4_MAX_CREATIONDATE_BYTES           128
#define MM_MPEG4_MAX_DEVICE_NAME_BYTES              9
#define MM_MPEG4_MAX_MODEL_NAME_BYTES               9
#define MM_MPEG4_MAX_ENCODER_INFO_BYTES             9
#define MM_MPEG4_MAX_MUX_INFO_BYTES                 9
#define MM_MPEG4_MAX_GPS_LATITUDE                   3
#define MM_MPEG4_MAX_GPS_LONGITUDE                  3
#define MM_MPEG4_MAX_GPS_TIME                       3
#define MM_MPEG4_MAX_GPS_SURVEY_DATA_BYTES          9
#define MM_MPEG4_MAX_GPS_POSITIONING_METHOD_BYTES  64
#define MM_MPEG4_MAX_GPS_POSITIONING_NAME_BYTES    64
#define MM_MPEG4_MAX_GPS_DATE_BYTES                12
#define MM_MPEG4_MAX_ALBUM_BYTES                   50

#define MM_MPEG4_KDDI_MAX_TEXT_COLORS 4
#define MM_MPEG4_3GPP_MAX_FONT_LEN    32

#define MM_MPEG4_MAX_RTSP_STATUS_STRING_BYTES      64
#define MM_MPEG4_MAX_UNHANDLED_SDP_RTSP_STRING_BYTES 256
#define MM_DEFAULT_TICK_INTERVAL                  500 /* in milliseconds */

// added to compile with BREW 3.0 will have to remove it after change
// is added to AEEMedia.h

#define MM_SEEK_ACCESS_POINT 3

// 
// MPEG4RotationType Enumeration
//
typedef enum AEEMediaMPEG4RotationType
{
  MM_MPEG4_NO_ROTATION=0,		// MPEG4 is not rotated
  MM_MPEG4_90_CW_ROTATION,		// MPEG4 rotated 90 degrees clockwise
  MM_MPEG4_90_CCW_ROTATION,		// MPEG4 rotated 90 degrees counter clockwise
  MM_MPEG4_180_ROTATION			// MPEG4 rotated 180 degrees counter clockwise
} AEEMediaMPEG4RotationType;

//
// MPEG4TrakType Enumeration
// 
typedef enum AEEMediaMPEG4TrackType
{
  VIDEO_TRACK,
  AUDIO_TRACK,
  TEXT_TRACK
} AEEMediaMPEG4TrackType;

//
// MPEG4FileFormatType Enumeration
// 
typedef enum AEEMediaMPEG4FileFormatType
{
  MM_MPEG4_UNKNOWN_FORMAT_TYPE=0,
  MM_MPEG4_STD_FORMAT_TYPE,
  MM_MPEG4_KDDI_FORMAT_TYPE
} AEEMediaMPEG4FileFormatType;

//
// StreamType Enumeration
// 
typedef enum AEEStreamType
{
  MM_MEDIA_NON_STREAMING = 0, 	// local clip
  MM_MEDIA_STREAMING,    		//  streaming
  MM_MEDIA_LIVE_STREAMING 		// Live streaming
} AEEStreamType;

//
// MPEG4ScalingType Enumeration
// 
typedef enum AEEMediaMPEG4ScalingType
{
  MM_MPEG4_2P5X_ZOOM=3,			// 2.5X scaling
  MM_MPEG4_2X_ZOOM=2,			// 2X scaling
  MM_MPEG4_1P25X_ZOOM=1,		// 1.25X scaling
  MM_MPEG4_NO_SCALING=0,		// 1X scaling
  MM_MPEG4_0P25X_SHRINK=(-1),	// 0.25X scaling
  MM_MPEG4_0P75X_SHRINK=(-2),	// 0.75X scaling
  MM_MPEG4_2X_SHRINK=(-3)		// 0.5X scaling
} AEEMediaMPEG4ScalingType;

typedef enum AEEMediaMPEG4Type
{
   MM_MPEG4_AUDIO_ONLY=0
  ,MM_MPEG4_VIDEO_ONLY
  ,MM_MPEG4_AUDIO_VIDEO
  ,MM_MPEG4_VIDEO_STILL_IMAGE
  ,MM_MPEG4_AUDIO_VIDEO_STILL_IMAGE
  ,MM_MPEG4_VIDEO_TEXT
  ,MM_MPEG4_AUDIO_TEXT
  ,MM_MPEG4_AUDIO_VIDEO_TEXT
  ,MM_MPEG4_VIDEO_STILL_IMAGE_TEXT
  ,MM_MPEG4_AUDIO_VIDEO_STILL_IMAGE_TEXT
  ,MM_MPEG4_INVALID
} AEEMediaMPEG4Type;

typedef enum AEEMediaMP4CodecType
{
   MM_MPEG4_EVRC_CODEC=0
  ,MM_MPEG4_QCELP_CODEC
  ,MM_MPEG4_AAC_CODEC
  ,MM_MPEG4_GSM_AMR_CODEC
  ,MM_MPEG4_MPEG4_CODEC
  ,MM_MPEG4_H263_CODEC 
  ,MM_MPEG4_STILL_IMAGE_CODEC
  ,MM_MPEG4_TIMED_TEXT_CODEC
  ,MM_MPEG4_MP3_CODEC
  ,MM_MPEG4_H264_CODEC
  ,MM_MPEG4_UNKNOWN_CODEC 
} AEEMediaMP4CodecType;


typedef struct AEEMediaMPEG4StdSpec
{
  AECHAR              szTitle[MM_MPEG4_MAX_TITLE_BYTES];
  AECHAR              szAuthor[MM_MPEG4_MAX_AUTHOR_BYTES];
  AECHAR              szCopyright[MM_MPEG4_MAX_COPYRIGHT_BYTES];
  AECHAR              szRating[MM_MPEG4_MAX_RATING_BYTES];
  AECHAR              szDescription[MM_MPEG4_MAX_DESCRIPTION_BYTES];
  AECHAR              szOwner[MM_MPEG4_MAX_OWNER_BYTES];
  AECHAR              szVersion[MM_MPEG4_MAX_VERSION_BYTES];
  AECHAR              szCreationDate[MM_MPEG4_MAX_CREATIONDATE_BYTES];
  AECHAR              szArtist[MM_MPEG4_MAX_AUTHOR_BYTES];
  AECHAR              szSTitle[MM_MPEG4_MAX_TITLE_BYTES];
  AECHAR              szAlbum[MM_MPEG4_MAX_ALBUM_BYTES];
  AECHAR              szInfo[MM_MPEG4_MAX_DESCRIPTION_BYTES];
} AEEMediaMPEG4StdSpec;

typedef struct AEEMediaMPEG4KDDISpec
{
  uint32              dwCopyProhibitionFlag;
  uint32              dwValidityEffectiveDate; 
  uint32              dwValidityPeriod; 
  uint32              dwNumberOfAllowedPlayBacks; 

  AECHAR              szTitle[MM_MPEG4_MAX_TITLE_BYTES];
  AECHAR              szCopyright[MM_MPEG4_MAX_COPYRIGHT_BYTES];
  AECHAR              szAuthor[MM_MPEG4_MAX_AUTHOR_BYTES];
  AECHAR              szMemo[MM_MPEG4_MAX_DESCRIPTION_BYTES];
  uint32              dwGenerationDLLVersion;

  uint32              dwEditFlags; 
  uint32              dwRecordingMode; 
  uint32              dwRecordingDate;

  AECHAR              szDeviceName[MM_MPEG4_MAX_DEVICE_NAME_BYTES]; 
  AECHAR              szModelName[MM_MPEG4_MAX_MODEL_NAME_BYTES]; 
  AECHAR              szEncoderInformation[MM_MPEG4_MAX_ENCODER_INFO_BYTES]; 
  AECHAR              szMuxInformation[MM_MPEG4_MAX_MUX_INFO_BYTES]; 

  uint16              wGPSByteOrder;
  uint32              dwGPSVersionID;
  uint32              dwGPSLatitudeRef;
  uint32              dwGPSLongitudeRef;
  uint32              dwGPSAltitudeRef;
  uint64              GPSLatitudeArray[MM_MPEG4_MAX_GPS_LATITUDE];
  uint64              GPSLongitudeArray[MM_MPEG4_MAX_GPS_LONGITUDE];
  uint64              dwGPSAltitude;
  uint64              GPSTimeArray[MM_MPEG4_MAX_GPS_TIME];
  uint64              ddwGPSExtensionMapScaleInfo;
  AECHAR              szGPSSurveyData[MM_MPEG4_MAX_GPS_SURVEY_DATA_BYTES];
  AECHAR              szGPSPositioningMethod[MM_MPEG4_MAX_GPS_POSITIONING_METHOD_BYTES];
  AECHAR              szGPSPositioningName[MM_MPEG4_MAX_GPS_POSITIONING_NAME_BYTES];
  AECHAR              szGPSDate[MM_MPEG4_MAX_GPS_DATE_BYTES];

} AEEMediaMPEG4KDDISpec;

typedef union AEEMediaMPEG4SpecEx
{
  AEEMediaMPEG4StdSpec        StdSpec;
  AEEMediaMPEG4KDDISpec       KDDISpec;
}AEEMediaMPEG4SpecEx;

//
// Clip information
//
typedef struct AEEMediaMPEG4Spec
{
  uint32                      dwDuration; 					// clip duration in ms
  uint32                      dwSize;     					// file size
  AEEStreamType               streamtype; 					// Only applicable to streaming clips
  AEEMediaMPEG4Type           mpeg4Type;  					// MP4 content type
  AEEMediaMP4CodecType        audioType;  					// MP4 Audio Codec type
  boolean                     bRepositioningAllowed;
  boolean                     bAudioOnlyPlaybackAllowed;
  boolean                     bVideoOnlyPlaybackAllowed;
  uint32                      dwHeight; 					// height of the video frame
  uint32                      dwWidth;  					// width of video frame

  AEEMediaMPEG4FileFormatType eFormatType;
  AEEMediaMPEG4SpecEx         ExtendedInfo;
} AEEMediaMPEG4Spec;

//
// 3GPP timed text font info
//
typedef struct AEETTFontInfo 
{
  uint16    nID;
  char      Name[MM_MPEG4_3GPP_MAX_FONT_LEN];
} AEETTFontInfo;

typedef struct AEE3GPPTimedText
{
  AECHAR      * pszText;
  uint32        dwDuration;
  uint32        dwDisplayFlags;
  int32         dwHorzJustification;
  int32         dwVertJustification;
  int32         dwBoxTop;
  int32         dwBoxLeft;
  int32         dwBoxBottom;
  int32         dwBoxRight;
  uint32        dwStartChar;
  uint32        dwEndChar;
  uint32        dwFontID;
  uint32        dwFontStyleFlags;
  uint32        dwFontSize;
  uint8         BackgroundColorRGBA[MM_MPEG4_KDDI_MAX_TEXT_COLORS];
  uint8         TextColorRGBA[MM_MPEG4_KDDI_MAX_TEXT_COLORS];
  uint32        dwFontListSize;
  AEETTFontInfo *pFontList; /* list indexed from 0 to dwFontListSize-1 */
  uint32        dwSampleModifiersSize;
  uint8         *pSampleModifiersBuffer;

} AEE3GPPTimedText;

//
// RTSPStatus structure
// 
typedef struct AEERTSPStatus
{
  int32 statusCode;
  char  statusString[MM_MPEG4_MAX_RTSP_STATUS_STRING_BYTES];
} AEERTSPStatus;

//
// Unhandled status string
// 
typedef struct AEEUnhandledSDP_RTSP_Status
{
   char  statusString[MM_MPEG4_MAX_UNHANDLED_SDP_RTSP_STRING_BYTES];
} AEEUnhandledSDP_RTSP_Status;

/* QoS structure for choosing PDP context */
typedef struct AEENetPolicyInfo
{
    uint32 size; /* Size of this total structure in bytes */
    boolean isSet;    /* TRUE, if the information in this structure is valid */
    boolean isQoSEnabled; /* TRUE, if QoS is enabled */
    uint8   primaryPDPProfileNo; /* PDP profile Number */
}AEENetPolicyInfo;

/* Stream recording parameters*/
typedef struct AEEStreamRecordInfo
{
  char * filename; /* File name along with path */
  boolean overwrite; /* TRUE, if it is OK to overwrite an existing file */
  uint8 mode; /*Bit mask to select tracks to be recoreded */
  uint32 duration; /* 0, If no limit is enforced */
  /*
    mode and duration may be ignored by the Qtv player. 
    Refer to Qtv player API for more details.
  */
} AEEStreamRecordInfo;

/*=========================================================================================
  DATA STRUCTURE DOCUMENTATION
===========================================================================================
AEEMediaMPEG4StdSpec  
  
Description:
This structure contains information for the standard specifications for a MPEG4 clip.

Definition:
	typedef struct AEEMediaMPEG4StdSpec
	{
	  AECHAR              szTitle[MM_MPEG4_MAX_TITLE_BYTES];
	  AECHAR              szAuthor[MM_MPEG4_MAX_AUTHOR_BYTES];
	  AECHAR              szCopyright[MM_MPEG4_MAX_COPYRIGHT_BYTES];
	  AECHAR              szRating[MM_MPEG4_MAX_RATING_BYTES];
	  AECHAR              szDescription[MM_MPEG4_MAX_DESCRIPTION_BYTES];
	  AECHAR              szOwner[MM_MPEG4_MAX_OWNER_BYTES];
	  AECHAR              szVersion[MM_MPEG4_MAX_VERSION_BYTES];
	  AECHAR              szCreationDate[MM_MPEG4_MAX_CREATIONDATE_BYTES];
	  AECHAR              szArtist[MM_MPEG4_MAX_AUTHOR_BYTES];
	  AECHAR              szSTitle[MM_MPEG4_MAX_TITLE_BYTES];
	  AECHAR              szAlbum[MM_MPEG4_MAX_ALBUM_BYTES];
	  AECHAR              szInfo[MM_MPEG4_MAX_DESCRIPTION_BYTES];
	} AEEMediaMPEG4StdSpec;

Members:
	szTitle, szAuthor, szCopyright, szRating, szDescription, szOwner, szVersion, szCreationDate,
	szArtist, szSTitle, szAlbum, szInfo are all char arrays contain strings representing given
	infomation for a clip about the title, author, copyright, etc.

Comments:
	Every MPEG4 must have these fields in it.  Even though they exist some of them may still be
	NULL.

See Also:
	AEEMediaMPEG4SpecEx
	AEEMediaMPEG4KDDISpec

=============================================================================================
AEEMediaMPEG4KDDISpec

Description:
This structure is an extension to the basic AEEMediaMPEG4StdSpec that contains extra information
given for all KDDI clips including GPS information.

Definition:
	typedef struct AEEMediaMPEG4KDDISpec
	{
	  uint32              dwCopyProhibitionFlag;
	  uint32              dwValidityEffectiveDate; 
	  uint32              dwValidityPeriod; 
	  uint32              dwNumberOfAllowedPlayBacks; 
	
	  AECHAR              szTitle[MM_MPEG4_MAX_TITLE_BYTES];
	  AECHAR              szCopyright[MM_MPEG4_MAX_COPYRIGHT_BYTES];
	  AECHAR              szAuthor[MM_MPEG4_MAX_AUTHOR_BYTES];
	  AECHAR              szMemo[MM_MPEG4_MAX_DESCRIPTION_BYTES];
	  uint32              dwGenerationDLLVersion;
	
	  uint32              dwEditFlags; 
	  uint32              dwRecordingMode; 
	  uint32              dwRecordingDate;

	  AECHAR              szDeviceName[MM_MPEG4_MAX_DEVICE_NAME_BYTES]; 
	  AECHAR              szModelName[MM_MPEG4_MAX_MODEL_NAME_BYTES]; 
	  AECHAR              szEncoderInformation[MM_MPEG4_MAX_ENCODER_INFO_BYTES]; 
	  AECHAR              szMuxInformation[MM_MPEG4_MAX_MUX_INFO_BYTES]; 

	  uint16              wGPSByteOrder;
	  uint32              dwGPSVersionID;
	  uint32              dwGPSLatitudeRef;
	  uint32              dwGPSLongitudeRef;
	  uint32              dwGPSAltitudeRef;
	  uint64              GPSLatitudeArray[MM_MPEG4_MAX_GPS_LATITUDE];
	  uint64              GPSLongitudeArray[MM_MPEG4_MAX_GPS_LONGITUDE];
	  uint64              dwGPSAltitude;
	  uint64              GPSTimeArray[MM_MPEG4_MAX_GPS_TIME];
	  uint64              ddwGPSExtensionMapScaleInfo;
	  AECHAR              szGPSSurveyData[MM_MPEG4_MAX_GPS_SURVEY_DATA_BYTES];
	  AECHAR              szGPSPositioningMethod[MM_MPEG4_MAX_GPS_POSITIONING_METHOD_BYTES];
	  AECHAR              szGPSPositioningName[MM_MPEG4_MAX_GPS_POSITIONING_NAME_BYTES];
	  AECHAR              szGPSDate[MM_MPEG4_MAX_GPS_DATE_BYTES];
	} AEEMediaMPEG4KDDISpec;

Members:
	dwCopyProhibitionFlag:			Any bit in this field is set if clip is copy protected
	dwValidityEffectiveDate:		Validity Start date from CopyGaurdAtom
	dwValidityPeriod:				Validity period from the Start Date from CopyGuardAtom
	dwNumberOfAllowedPlayBacks:		Number of times clip is allowed to be played back
	szTitle:						Char array with title of clip
	szCopyright:					Char array with copyright string
	szAuthor:						Char array with author string
	szMemo:							Char array with any extra notes or info
	dwGenerationDLLVersion:			Version of the DLL used
	dwEditFlags:					Edit flags from Movie Mail Atom
	dwRecordingMode:				Recording mode of Movie Mail Atom
	dwRecordingDate:				Recording date for Movie Mail Atom
	szDeviceName:					Char array with encoding device name from Encoder Information Atom
	szModelName:					Char array with encoder model name from Encoder Information Atom
	szEncoderInformation:			Char array with encoder information from Encoder Information Atom
	szMuxInformation:				Char array with encoding multiplexer information
	wGPSByteOrder:					GPS byte order from GPS Atom
	dwGPSVersionID:					Version ID from GPS Atom
	dwGPSLatitudeRef:				GPS Latitude reference from GPS Atom
	dwGPSLongitudeRef:				GPS Longitude reference from GPS Atom
	dwGPSAltitudeRef:				GPS Altitude reference from GPS Atom
	GPSLatitudeArray:				64-bit array for GPS latitude from GPS Atom
	GPSLongitudeArray:				64-bit array for GPS longitude from GPS Atom
	dwGPSAltitude:					64-bit GPS Altitude from GPS Atom
	GPSTimeArray:					64-bit time array from GPS Atom
	ddwGPSExtensionMapScaleInfo:	Extension Map Scale information from GPS Atom
	szGPSSurveyData:				Char array with GPS Survey data from GPS Atom
	szGPSPositioningMethod:			Char array with GPS positioning method from GPS Atom
	szGPSPositioningName:			Char array with GPS positioning name from GPS Atom
	szGPSDate:						Char array with GPS date from GPS Atom

Comments:

See Also:
	AEEMediaMPEG4SpecEx

=============================================================================================
AEEMediaMPEG4SpecEx

Description:
This is the extended spec which contains bot the standard and KDDI spec's described above.

Definition:
	typedef union AEEMediaMPEG4SpecEx
	{
	  AEEMediaMPEG4StdSpec        StdSpec;
	  AEEMediaMPEG4KDDISpec       KDDISpec;
	} AEEMediaMPEG4SpecEx;

Members:
	StdSpec:		Standard specification, see above
	KDDISpec:		KDDI specification, see above
	
Comments:

See Also:

=============================================================================================
AEEMediaMPEG4Spec

Description:
This is the information that pertains only to the video and auto information for a clip.

Definition:
	typedef struct AEEMediaMPEG4Spec
	{
	  uint32                      dwDuration; 					
	  uint32                      dwSize;     				
	  AEEStreamType               streamtype; 			
	  AEEMediaMPEG4Type           mpeg4Type;  		
	  AEEMediaMP4CodecType        audioType;
	  boolean                     bRepositioningAllowed;
	  boolean                     bAudioOnlyPlaybackAllowed;
	  boolean                     bVideoOnlyPlaybackAllowed;
	  uint32                      dwHeight;
	  uint32                      dwWidth;
	  AEEMediaMPEG4FileFormatType eFormatType;
	  AEEMediaMPEG4SpecEx         ExtendedInfo;
	} AEEMediaMPEG4Spec;

Members:
	dwDuration:						Duration of the clip in ms
	dwSize:							Size of the file in bytes
	streamtype:						Stream type only used for streaming clips
	mpeg4Type:						MP4 content type
	audioType:						MP4 Audio codec type
	bRepositioningAllowed:			True if reposition in clip is allowed
	bAudioOnlyPlaybackAllowed:		True if only audio and no video
	bVideoOnlyPlaybackAllowed:		True if only video and no audio
	dwHeight:						Height of video frame
	dwWidth:						Width of video frame
	eFormatType:					UNKNOWN, STD, or KDDI format type
	ExtendedInfo:					Extra STD and KDDI info structure, see above

Comments:
	This is the basic video and audio information that is needed, if other details are needed
	dig into the STD or KDDI structures
	
See Also:
	AEEMediaMPEG4SpecEx

=============================================================================================
AEETTFontInfo 

Description:
This is the font information structure that stores the name of the font and associated ID 
used for storing font information in an array

Definition:
	typedef struct AEETTFontInfo 
	{
	  uint16    nID;
	  char      Name[MM_MPEG4_3GPP_MAX_FONT_LEN];
	} AEETTFontInfo;

Members:
	nID:		ID of the font for the given name
	Name:		A char array representing the name of the font

Comments:

See Also:
	AEE3GPPTimedText

=============================================================================================
AEE3GPPTimedText

Description:
This structure is used to hold all the information need for displaying and keeping track of a
timed text area.

Definition:
	typedef struct AEE3GPPTimedText
	{
	  AECHAR      * pszText;
	  uint32        dwDuration;
	  uint32        dwDisplayFlags;
	  int32         dwHorzJustification;
	  int32         dwVertJustification;
	  int32         dwBoxTop;
	  int32         dwBoxLeft;
	  int32         dwBoxBottom;
	  int32         dwBoxRight;
	  uint32        dwStartChar;
	  uint32        dwEndChar;
	  uint32        dwFontID;
	  uint32        dwFontStyleFlags;
	  uint32        dwFontSize;
	  uint8         BackgroundColorRGBA[MM_MPEG4_KDDI_MAX_TEXT_COLORS];
	  uint8         TextColorRGBA[MM_MPEG4_KDDI_MAX_TEXT_COLORS];
	  uint32        dwFontListSize;
	  AEETTFontInfo *pFontList;  		list indexed from 0 to dwFontListSize-1
	  uint32        dwSampleModifiersSize;
	  uint8         *pSampleModifiersBuffer;
	} AEE3GPPTimedText;

Members:
	pszText:					Pointer to an text string that is displayed
	dwDuration:					The duration which the text is displayed for in ms
	dwDisplayFlags:				Flags to modify how text displayed
	dwHorzJustification:		Horizontal justification
	dwVertJustification:		Vertical Justification
	dwBoxTop:					Top edge of text box
	dwBoxLeft:					Left edge of text box
    dwBoxBottom:				Bottom edge of text box
	dwBoxRight:					Right edge of text box
	dwStartChar:				First character index from szText
	dwEndChar:					Last character index from szText	
	dwFontID:					ID of the font to use for text
	dwFontStyleFlags:			Style of font, bold, italics, etc
	dwFontSize:					Size of the font to use
	BackgroundColorRGBA:		Array containing the background color information
	TextColorRGBA:				Array containing the text color information
	dwFontListSize:				Number of elements in the font list
	pFontList:					A pointer to an array of font elements
	dwSampleModifiersSize:		Size of the sample modifiers buffer
	pSampleModifiersBuffer:		Pointer to the sample modifiers buffer data

Comments:

See Also:
	AEETTFontInfo 

=============================================================================================
AEERTSPStatus

Description:
Status structure contains a code and a string

Definition:
	typedef struct AEERTSPStatus
	{
	  int32 statusCode;
	  char  statusString[MM_MPEG4_MAX_RTSP_STATUS_STRING_BYTES];
	} AEERTSPStatus;
	
Members:
	statusCode:		Code number of the status
	statusString:	String describing the status

Comments:

See Also:

=============================================================================================
AEEUnhandledSDP_RTSP_Status

Description:
Unhandled status has no code, only a string.

Definition:
	typedef struct AEEUnhandledSDP_RTSP_Status
	{
	   char  statusString[MM_MPEG4_MAX_UNHANDLED_SDP_RTSP_STRING_BYTES];
	} AEEUnhandledSDP_RTSP_Status;
	
Members:
	statusString:	String describing the unhandled status
	
Comments:

See Also:

=============================================================================================

MP4 Control Parameters (MM_MP4_PARM_XXX)

Description:
These parameters allow setting/getting of MP4 media parameters.  Use IMEDIA_GetMediaData and 
IMEDIA_SetMediaData to get and set media parameters.

MM_MP4_PARM_ROTATION
Set:  Sets the rotational value of current clip
      p1 = AEEMediaData *
      p2 = int32  (rotational value)
Get:  Gets the current media data's rotation value.
      p1 = AEEMediaData *
      p2 = int32  * (rotational value)

MM_MP4_PARM_SCALING
Set:  Sets the scaling value of current clip
      p1 = AEEMediaData *
      p2 = int32  (scaling value)
Get:  Gets the current media data's scaling value.
      p1 = AEEMediaData *
      p2 = int32  * (scaling value)

MM_MP4_PARM_TELOP_SUB_STR
Get:  Gets the current media data's telop sub string.
      p1 = AEEMediaData *
      p2 = int32  * (pointer to substring)
	  p3 = telop substring index

MM_MP4_PARM_BUFFER_UPDATE       - Indicate Buffering
MM_MP4_PARM_CONNECT_UPDATE      - Indicate Connection Setup to user
MM_MP4_PARM_PB_READY            - Indicate PB Ready
MM_MP4_PARM_MEDIA_SPEC          - Retrieves the MediaSpec
MM_MP4_STREAM_AUTHENTICATE      - Authenticate the stream when enabled 
MM_MP4_AUDIO_VIDEO_MEDIA_DATA   - all audio video media data
MM_MP4_PARM_TRACK_TIME_SCALE    - Track time scale amount
MM_MP4_PARAM_AUDIO_OVERRIDE     - Overide audio parameters
MM_MP4_PARM_RTSP_STATUS_NOTIFY  - RTSP status notify event
MM_MP4_PARM_LOOP_TRACK          - p1 has value true/false
MM_MP4_PARAM_PLAYBACK_POS       - p1 = *playback time structure
MM_MP4_PARAM_PSBUF_WRITE_OFFSET - Set/Get PSBUF offset for writing
MM_MP4_PARM_PS_BUFFER_UPDATE    - Update the PS buffer
MM_MP4_PARM_DECRYPT_KEY         - p1= pointer to AEEMFDecryptKey struct, p2 = size
MM_MP4_SCALING_UPDATE           - Indicate to update after scaling
MM_MP4_PARM_RTSP_STREAM_RECORD  - enable stream recording
MM_MP4_PARM_MIDI_SIZE           - get midi data size, p1(output) = * uint32
MM_MP4_PARM_MIDI_DATA           - get midi data, p1 = buf pointer, p2(input/output) = * uint32 size
MM_MP4_PARM_LINK_SIZE           - get link data size, p1(output) = * uint32
MM_MP4_PARM_LINK_DATA           - get link data, p1 = buf pointer, p2(input/output) = * uint32 size
	  
===================================================================== 
  INTERFACES   DOCUMENTATION
===================================================================== 

IMEDIA_GetMediaSpec()

Description:
   
   This macro is used to get a pointer to a specific media specification.

Prototype:
   int IMEDIA_GetMediaSpec(IMedia * p, int32 * ppSpec, int32 * pSize);
  
Parameters:
     p [in]:        Pointer to the IMedia Interface object
     ppSpec [in]:   Pointer to an integer to store spec info
     pSize [in]:    Pointer to an integer to store the size in

Return Value:

   SUCCESS:    Successful.
   EBADSTATE:  Error - IMedia is not in Ready state.

Comments: 
   None.

Side Effects: 
   None

See Also:

======================================================================
*/
#endif // AEEMEDIAMPEG4_H

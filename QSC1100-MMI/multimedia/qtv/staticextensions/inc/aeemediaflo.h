#ifndef AEEMEDIAFLO_H
#define AEEMEDIAFLO_H

/*============================================================================
FILE:   AEEMediaFLO.h

SERVICES:  BREW Movie File Services for MediaFLO.

DESCRIPTION:
   This file defines specific structures cast from generic ones in 
   AEEMediaMpeg4.h
   
PUBLIC CLASSES:  IMediaMPEG4

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/staticextensions/main/latest/inc/aeemediaflo.h#3 $
$DateTime: 2008/06/05 22:19:31 $
$Change: 672160 $


========================================================================== */

/*===========================================================================
  Includes
===========================================================================*/
#include "AEE.h"
#include "AEEMediaMPEG4.h"

/*===========================================================================
  Defines
===========================================================================*/

/*---------------------------------------------------------------------------
  The largest permitted debug UI string.
---------------------------------------------------------------------------*/
#define AEEMEDIAFLO_MAX_DEBUG_STRING_CHARS  255

/*---------------------------------------------------------------------------
  Max filename size, not including the NULL char
---------------------------------------------------------------------------*/
#define AEEMFLO_MAX_FILE_CHARS  60

/*===========================================================================
  Enums
===========================================================================*/

/*---------------------------------------------------------------------------
  Used when sending media specific events to clients.  Lets client know the
  media type associated with the event.
---------------------------------------------------------------------------*/
typedef enum {
  AEEMFLO_MEDIA_NONE  = 0,
  AEEMFLO_MEDIA_VIDEO = 1,
  AEEMFLO_MEDIA_AUDIO = 2,

  AEEMFLO_MEDIA_NUM   = 3
} AEEMediaFLOMediaType;

/*---------------------------------------------------------------------------
  Specifies the type of event being forwarded from MediaFLO.
---------------------------------------------------------------------------*/
typedef enum
{
  NOTIFY_MFLO_BLACKOUT            = 0,
  NOTIFY_MFLO_SYSTEM_LOST         = 1,
  NOTIFY_MFLO_UNDERRUN_RECOVERED  = 2,
  NOTIFY_MFLO_ERR_DECRYPT_FAIL    = 3,
  NOTIFY_MFLO_STREAM_ACTIVATED    = 4,
  NOTIFY_MFLO_STREAM_DEACTIVATED  = 5,
  NOTIFY_MFLO_DEBUG_STRING        = 6,
  NOTIFY_MFLO_REACQUIRING_SYSTEM  = 7,
  NOTIFY_MFLO_QOS_STATS           = 8
} AEEMediaFLOMsgType;

/*---------------------------------------------------------------------------
  MediaFLO SetMediaInfo data
---------------------------------------------------------------------------*/
typedef enum {
  AEEM_FLO_VIDEO_H264  = 0,
  AEEM_FLO_VIDEO_OSCAR = 1,

  AEEM_FLO_VIDEO_NUM   = 2
} AEEMediaFLOVideoDecoder;

/*---------------------------------------------------------------------------
  MediaFLO available resolutions.
---------------------------------------------------------------------------*/
typedef enum {
  AEEM_FLO_VIDEO_RES_QCIF  = 0,
  AEEM_FLO_VIDEO_RES_QVGA  = 1,
  AEEM_FLO_VIDEO_RES_VGA   = 2,

  AEEM_FLO_VIDEO_RES_NUM   = 3
} AEEMediaFLOVideoResolution;

/*---------------------------------------------------------------------------
  Supported audio formats.
---------------------------------------------------------------------------*/
typedef enum {
  AEEM_FLO_AUDIO_AAC          = 0,
  AEEM_FLO_AUDIO_AAC_PLUS     = 1,
  AEEM_FLO_AUDIO_ENH_AAC_PLUS = 2,

  AEEM_FLO_AUDIO_NUM          = 3
} AEEMediaFLOAudioDecoder;

/*---------------------------------------------------------------------------
  MediaFLO stream types.
---------------------------------------------------------------------------*/
typedef enum {
  AEEMFLO_STRM_VIDEO_BASE = 0,
  AEEMFLO_STRM_VIDEO_ENH  = 1,
  AEEMFLO_STRM_AUDIO      = 2,
  AEEMFLO_STRM_NUM        = 3
} AEEMediaFLOStreamType;

/*---------------------------------------------------------------------------
  The various ways that FRUC can be forced.  The first eight (0 through 7)
  enums write a specific value to each FRUC table entry.  The second-last
  enum writes a random value to each FRUC table entry.  The final enum is
  for bounds checking.
---------------------------------------------------------------------------*/
typedef enum {
  AEEMFLO_FRUC_VERSION_OFF     = 0,
  AEEMFLO_FRUC_VERSION_1_DOT_0 = 1,
  AEEMFLO_FRUC_VERSION_2_DOT_0 = 2,
  AEEMFLO_FRUC_VERSION_3_DOT_0 = 3,
  AEEMFLO_FRUC_VERSION_4_DOT_0 = 4,
  AEEMFLO_FRUC_VERSION_5_DOT_0 = 5,
  AEEMFLO_FRUC_VERSION_6_DOT_0 = 6,
  AEEMFLO_FRUC_VERSION_7_DOT_0 = 7,
  AEEMFLO_FRUC_VERSION_RANDOM  = 8,
  AEEMFLO_FRUC_VERSION_NUM     = 9,
  AEEMFLO_FRUC_FORCE_8BITS     = 0xff /* Guarantees 8 bit quantity. */
} AEEMediaFLOFRUCVersionType;

/*---------------------------------------------------------------------------
  MediaFLO GetMediaInfo data.  Enumeration of mflo subitems requested 
  through MM_MP4_PARM_MEDIA_INFO
---------------------------------------------------------------------------*/
typedef enum {
  /*-------------------------------------------------------------------------
    Version of this interface
  -------------------------------------------------------------------------*/
  AEEMFLO_GETDATA_VERSION   		              = 0,

  /*-------------------------------------------------------------------------
    UI sends us this not to get data, but to delivery us a debug keypress
    from the user.
  -------------------------------------------------------------------------*/
  AEEMFLO_GETDATA_CMD_DEBUG_INCOMING_KEYPRESS = 1
} AEEMediaFLOParmType;

/*---------------------------------------------------------------------------
  Available video and audio layers.
---------------------------------------------------------------------------*/
typedef enum {
  AEEMFLO_VIDEO_LYR_BASE = 0,
  AEEMFLO_VIDEO_LYR_ENH  = 1,
  AEEMFLO_VIDEO_LYR_NUM  = 2,

  AEEMFLO_AUDIO_LYR_BASE = 0,
  AEEMFLO_AUDIO_LYR_NUM  = 1,

  AEEMFLO_MAX_LYR_NUM    = 2
} AEEMediaFLOLayerType;

/*===========================================================================
  Structures
===========================================================================*/

/*---------------------------------------------------------------------------
  Blackout information.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 start_time;
  uint32 end_time;
} AEEMediaFLOBlackoutInfo;

/*---------------------------------------------------------------------------
  Holds Quality of Service information from the previous superframe.
---------------------------------------------------------------------------*/
typedef struct {
  /*-------------------------------------------------------------------------
    Rx'd frame rate.  Measured in frames per second.
  -------------------------------------------------------------------------*/
  uint32 sourceFPS;

  /*-------------------------------------------------------------------------
    Output frame rate to display/speaker.  Measured in frames per second.
  -------------------------------------------------------------------------*/
  uint32 outputFPS;

  /*-------------------------------------------------------------------------
    Packet error rate, as a percentage of total packets in the superframe.
  -------------------------------------------------------------------------*/
  uint16 packetErrorRate[AEEMFLO_MAX_LYR_NUM];

  /*-------------------------------------------------------------------------
    Media's incoming date rate.  Measured in bytes per second.
  -------------------------------------------------------------------------*/
  uint16 sourceDataRate[AEEMFLO_MAX_LYR_NUM];
} AEEMediaFLOQualityStats;

/*---------------------------------------------------------------------------
  Represents the data for messages sent to the UI.
---------------------------------------------------------------------------*/
typedef union 
{
  /*-------------------------------------------------------------------------
    Blackout information sent to UI.
  -------------------------------------------------------------------------*/
  AEEMediaFLOBlackoutInfo blackout;

  /*-------------------------------------------------------------------------
    Debug string sent to UI for display.  +1 for NULL
  -------------------------------------------------------------------------*/
  char                    debug_string[AEEMEDIAFLO_MAX_DEBUG_STRING_CHARS+1];

  /*-------------------------------------------------------------------------
    QOS statistics sent to the UI for usage tracking purposes.
  -------------------------------------------------------------------------*/
  AEEMediaFLOQualityStats qosStats;
} AEEMediaFLOMsgData;

/*---------------------------------------------------------------------------
  Provides data to accompany MediaFLO notification messages.  Please add
  new members to the end of the structure to preserve old interfaces.
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    Data being sent for this event (if any).
  -------------------------------------------------------------------------*/
  AEEMediaFLOMsgData data;

  /*-------------------------------------------------------------------------
    The media type that this message refers to (if any).
  -------------------------------------------------------------------------*/
  AEEMediaFLOMediaType media;
} AEEMediaFLOMsgInfo;

/*---------------------------------------------------------------------------
  Data structure that represents a media status message sent from BCAST FLO  
  through   MM_MP4_PARM_MEDIA_INFO
---------------------------------------------------------------------------*/
typedef struct AEEMediaFLOMessage
{
  /*-------------------------------------------------------------------------
    Param supplied by caller indicating which flo data to fetch
  -------------------------------------------------------------------------*/
  AEEMediaFLOMsgType    type;
  
  /*-------------------------------------------------------------------------
    Data that is returned/filled-in by the callee (Media: BCAST FLO)
  -------------------------------------------------------------------------*/
  AEEMediaFLOMsgInfo    info;
} AEEMediaFLOMessage;

/*---------------------------------------------------------------------------
  FLO media info type for video
---------------------------------------------------------------------------*/
typedef struct {
  /*-------------------------------------------------------------------------
    A video stream exists.
  -------------------------------------------------------------------------*/
  boolean                     exists;

  /*-------------------------------------------------------------------------
    Drops the current superframe if the renderer drops this many late 
    frames.
  -------------------------------------------------------------------------*/
  uint32                      drop_sf_after_late_vid_frames;

  /*-------------------------------------------------------------------------
    Video decoder to use.
  -------------------------------------------------------------------------*/
  AEEMediaFLOVideoDecoder     decoder;

  /*-------------------------------------------------------------------------
    Resolution
  -------------------------------------------------------------------------*/
  AEEMediaFLOVideoResolution  res;
  // bitrate

  /* Filename to play, if any */
  char                        file[AEEMFLO_MAX_FILE_CHARS+1]; 
} AEEMediaFLOVideoInfo;

/* FLO media info type for audio */
typedef struct {
  boolean                 exists;  /* An audio stream exists */
  AEEMediaFLOAudioDecoder decoder; /* Audio decoder to use */
  // bitrate

  // debug... perhaps take these out later
  char                    file[AEEMFLO_MAX_FILE_CHARS+1]; 
                                   /* Filename to play, if any */

} AEEMediaFLOAudioInfo;

/* Debug info */
typedef struct {
  /* Flag to start files off at random positions. */
  boolean               random_start; 
  
  /* Error generation flags */
  boolean               gen_plp_errs[AEEMFLO_STRM_NUM];
  boolean               gend_plp_errs_are_random_bits[AEEMFLO_STRM_NUM];
  boolean               gend_plp_errs_marked_bad[AEEMFLO_STRM_NUM];

  /* Fade generation flags */
  uint32                gen_fades_after_x_sfs[AEEMFLO_STRM_NUM];
  uint32                gend_fades_last_x_sfs[AEEMFLO_STRM_NUM];

  /* For recording audio and/or video superframes to a file on the EFS 
     Note that the video file will contain both base & enh layers. */
  boolean               record_video;
  boolean               record_audio;

  /* Increases the delay between generated SFs > 1 second.  Used for YUV
     dumping, since YUV frames must squeeze through the USB port. */
  boolean               slow_sf_generation;

  /* Allows corrupt frames to pass through the decoder.  When disabled, 
     the media layer will discard frames that fail CRC. */
  boolean               allow_frame_errors;

  /* Forced FRUC on or off.  Combine with parameter below. */
  boolean               force_FRUC;

  /* Minimum version to write in FRUC directory, when FRUC is forced on. */
  AEEMediaFLOFRUCVersionType min_FRUC_version;

} AEEMediaFLODebugInfo;

/* FLO media info type, includes both audio & video.  Users of this header
   should cast the media info void * to this if the enum describing it is
   MM_MEDIA_BCAST_FLO */
typedef struct {
  boolean               use_file; /* Flag to use file playback or not, as
                                     opposed to the live OTA feed */
  AEEMediaFLOVideoInfo  video;
  AEEMediaFLOAudioInfo  audio;
  AEEMediaFLODebugInfo  debug;

  /*-------------------------------------------------------------------------
    Yes, this field should be in the video struct.  However, for backwards
    compatibility with previous version of the UI, we must always add new
    fields to the end of the structure.

    Essentially, B frames indicates that this clip's frames could predict
    into the future.  This allows the media to set a minimum pre-render
    latency to support such out of order rendering.

    If unsure, the caller should default this to FALSE.  It should only be
    set to TRUE when the caller is absolutely sure that B frames (future
    prediction) will *not* be used in this clip/stream.
  -------------------------------------------------------------------------*/
  boolean               video_has_no_b_frames;

} AEEMediaFLOInfo;

/*---------------------------------------------------------------------------
  Data Structures for enums listed above (Needed only for complex items)
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Union of data representing each member of AEEMediaFLOParmType
---------------------------------------------------------------------------*/
typedef union AEEMediaFLOParmData
{
  /*-------------------------------------------------------------------------
    Which interface version does the AMSS support
  -------------------------------------------------------------------------*/
  uint32                version;

  /*-------------------------------------------------------------------------
    Which key was pressed.  This is the ASCII character for the key that was
    pressed (eg. '0', '1', '#') 
  -------------------------------------------------------------------------*/
  char                  debug_key;
} AEEMediaFLOParmData;

/*---------------------------------------------------------------------------
  Data structure that represents a media flow parm item requested through
  MM_MP4_PARM_MEDIA_INFO
---------------------------------------------------------------------------*/
typedef struct AEEMediaFLOParm
{
  /* Param supplied by caller indicating which flo data to fetch */
  AEEMediaFLOParmType   type;
  
  /* Param supplied by caller indicating the length of data that the caller 
     expects to be returned in the 'data' param. Must be set to the size of 
     the specific structure within the union, that corresponds to the 'type'
     being requested */
  uint32                data_length; 

  /* Data that is returned/filled-in by the callee (Media: BCAST FLO) */
  AEEMediaFLOParmData   data;
} AEEMediaFLOParm;
/*************************************************************************/


/*---------------------------------------------------------------------------
  MFLO_OSCAR version of the extended frame info. The initial part of the 
  structure is the common header. The elements that follow are specific to 
  MediaFlo
---------------------------------------------------------------------------*/
typedef struct
{
  /*-----------------------------------------------------------------------
    The constant header part that is common to any codec type that implements
    extended frame info
  -----------------------------------------------------------------------*/
  AEEMediaExtFrameInfoHeader header;

  /*---------------------------------------------------------------------
    This member is used to identify the current version of this structure, 
    so that the end user can check for backward compatibility
  ---------------------------------------------------------------------*/
  uint32 version;               /* The version of this user data structure */

  /*-----------------------------------------------------------------------
    This is an opaque ID that is passed along with each frame. This ID is
    an indicator of the current channel that is being decoded. When a 
    channel switch occurs, the opaque ID  would change.  Internally the 
    opaque ID maps to the FLO-ID for the current channel. The intent of this 
    member is to assist the end user (UI) identify a channel switch. 
  -----------------------------------------------------------------------*/
  uint32 id;

} AEEMediaExtFrameMFLOInfo;


#endif // AEEMEDIAFLO_H

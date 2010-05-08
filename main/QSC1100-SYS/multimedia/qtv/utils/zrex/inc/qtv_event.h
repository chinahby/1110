#ifndef QTV_EVENT_H
#define QTV_EVENT_H
/* =======================================================================
                               qtv_event.h
DESCRIPTION
  All the data structures that represent payloads and related definitions
  to support the QTV Diagnostoc events reside in this file.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/zrex/main/latest/inc/qtv_event.h#7 $
$DateTime: 2008/05/08 14:44:36 $
$Change: 656482 $

when       who          what, where, why
--------   -----       ----------------------------------------------------------
10/30/03   igt         Added PACKED designation to the buffering event structure
09/05/03   ioakimt     Initial check-in for the qtv_event.h file
10/07/03   igt         Added event payload for EVENT_QTV_BUFFERING_STARTED and
                       EVENT_QTV_BUFFERING_ENDED
===========================================================================*/

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
#include "event.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_QTV_IPV6 
#error code not present
#endif
// for EVENT_QTV_USING_PROXY_SERVER
typedef struct 
{
  uint16 rtsp_port;
#ifdef FEATURE_QTV_IPV6 
#error code not present
#endif
  uint8  ip_address[4];
#ifdef FEATURE_QTV_IPV6 
#error code not present
#endif

} qtv_event_using_proxy_server_type;

// for EVENT_QTV_SERVER_PORTS_USED
typedef struct 
{
  uint16 rtp_port;
  uint16 rtcp_port;
} qtv_event_server_ports_used_type;

// for EVENT_QTV_STREAMING_URL_OPEN  
typedef PACKED struct
{
  uint32 status;
} qtv_event_url_open_type;

//for EVENT_QTV_ROTATE
typedef struct
{
	long direction;
} qtv_event_rotate_type;

// for EVENT_QTV_CLIP_REPOSITION_RESUME
typedef struct 
{
  long resume_time;
} qtv_event_clip_reposition_resume_type;

// for EVENT_QTV_CLIP_PAUSE_RESUME
typedef struct 
{
  long resume_time;
} qtv_event_clip_pause_resume_type;

// for EVENT_QTV_CLIP_REPOSITIONING
typedef struct 
{
  unsigned long current_time;
} qtv_event_clip_repositioning_type;

// for EVENT_QTV_CLIP_PAUSE
typedef struct 
{
  unsigned long current_time;
} qtv_event_clip_pause_type;
// for EVENT_QTV_CLIP_ROTATE
typedef struct
{
  unsigned long direction;
} qtv_event_clip_rotate_type;

//for EVENT_QTV_AUDIO_CHANNEL_SWITCH_FRAME
typedef struct
{
  uint32 ChannelSwitchedTS;
  uint32 ApproxDelay;
}qtv_event_Audiochannelswitch_frame;

// for EVENT_QTV_SDP_PARSER_REJECT
#define QTV_EVENT_UNKNOWN_MEDIA_TYPE 0
#define QTV_EVENT_AUDIO              1
#define QTV_EVENT_VIDEO              2
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#define QTV_EVENT_TEXT               3
#endif

// for EVENT_QTV_SILENCE_INSERTION_ENDED
typedef struct
{
  int32 numSilenceFramesInserted;
  int32 durationofSilenceFramesInserted;
} qtv_event_silence_insertion_type;

// for EVENT_QTV_SILENCE_INSERTION_DURATION
typedef struct
{
  uint32 silenceDuration;
} qtv_event_silence_insertion_duration;

typedef struct 
{
  uint8 audio_video; // 0 - unknown media type
                     // 1 - audio, 
                     // 2 - video,
                     // 3 - text,
  uint8 reject_type; // 0 - SDP_BAD_FORMAT
                     // 1 - SDP_BAD_MEDIA_FORMAT
                     // 2 - SDP_BAD_SESSION_FORMAT 
                     // 3 - SDP_UNSUPPORTED_MEDIA_TYPE
                     // 4 - SDP_AUDIO_ERROR_FORMAT_NOT_SUPPORTED
                     // 5 - SDP_VIDEO_ERROR_FORMAT_NOT_SUPPORTED
} qtv_event_sdp_parser_reject_type;


// for EVENT_QTV_CLIP_ENDED
#define QTV_EVENT_CLIP_ENDED_SUCCESS     0
#define QTV_EVENT_CLIP_ENDED_STOPPED     1
#define QTV_EVENT_CLIP_ENDED_ERROR_ABORT 2

typedef PACKED struct 
{
  uint8  end_type;      // success, failure_type
  uint32 NumFrameErrors;
} qtv_event_clip_ended_type;

typedef PACKED struct 
{
  uint8  end_type;      // success, failure_type
  uint32 total_frames;  // total number of video frames
  uint32 decode_error_frames;// decode error video frames
  uint32 dropped_frames;// dropped video frames
} qtv_event_clip_ended_type_ver2;


typedef struct 
{
  uint8  video_codec; // Media::CodecType
  uint8  audio_codec; // Media::CodecType
  uint8  num_layers ; // 1 or 2 if temporal scalability
  long   start_time ; // starting (frame/time)
  uint32 duration;    // clip duration
} qtv_event_clip_started_type;

// for EVENT_QTV_BUFFERING_STARTED & EVENT_QTV_BUFFERING_ENDED
typedef PACKED struct 
{
  uint8  audio_video;        // audio or video, uses QTV_EVENT_AUDIO/VIDEO
  int    remaining_duration; // amount of data remaining in the buffer
} qtv_event_buffering_type;


/* Not flagged with FEATURE_QTV_PSEUDO_STREAM */
typedef struct
{
  uint32 totalBufSize; /* Total size of pseudo stream buffer provided by App */
  uint32 unfilledBufSize;  /* totalBufSize - filled buffer size */
} qtv_event_ps_dl_start_type;


typedef struct
{
 uint32 pseudo_pause_duration; /* milliseconds*/
}qtv_event_pp_duration_type;


typedef struct
{
  uint16 fragment_number; /*sequence number of the fragment*/
  uint32 unfilled_buffer_size; /*size of the buffer allocated left without data*/
  uint32 buffer_fill_rate; /*rate at which the buffer is filling data rate*/
}qtv_event_parser_ready_type;

typedef struct
{
  uint16 fragment_number; /*sequence number of the fragment*/
}qtv_event_fragment_pb_info_type;

typedef struct
{  
  uint8 bPlayAudio;/* non zero indicates fragment diag event will be fired when parsing audio*/
  uint8 bPlayVideo;/* non zero indicates fragment diag event will be fired when parsing video*/
}qtv_fragment_event_fire_info_type;

/*Generic event used by diag to report Qtv status as a diag event*/
typedef struct
{
  uint32 qtv_status;
  uint32 handle;
  uint32 pClientData;

}qtv_status_diag_event_payload_type;
/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

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
FUNCTION 
  SAMPLE_FUNC

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

#endif /* QTV_EVENT_H */


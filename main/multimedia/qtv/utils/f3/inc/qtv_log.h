#ifndef QTV_LOG_H
#define QTV_LOG_H
/* =======================================================================
                               qtv_log.h
DESCRIPTION
   definitions for Qtv Log Codes

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/f3/main/latest/inc/qtv_log.h#5 $
$DateTime: 2009/05/09 01:17:43 $
$Change: 908545 $


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
#include "log.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define LOG_QTV_SLEEP_TIMEOUT                           20    /* msec. */
#define LOG_QTV_CMX_AUDIO_DATA_C_RETRY                  10    /* 10 attempts at the max. */

#define LOG_QTV_STREAMING_RX_STATS_C                    (0x164 + LOG_1X_BASE_C)
#define LOG_QTV_RTSP_DESCRIBE_C                         (0x165 + LOG_1X_BASE_C)
#define LOG_QTV_RTSP_SETUP_C                            (0x166 + LOG_1X_BASE_C)
#define LOG_QTV_RTSP_PLAY_C                             (0x167 + LOG_1X_BASE_C)
#define LOG_QTV_RTSP_PAUSE_C                            (0x168 + LOG_1X_BASE_C)
#define LOG_QTV_RTSP_TEARDOWN_C                         (0x169 + LOG_1X_BASE_C)
#define LOG_QTV_RTSP_SERVERRESPONSE_C                   (0x16A + LOG_1X_BASE_C)
#define LOG_QTV_RTCP_RR_C                               (0x16B + LOG_1X_BASE_C)
#define LOG_QTV_RTCP_SR_C                               (0x16C + LOG_1X_BASE_C)
#define LOG_QTV_RTP_STATS_C                             (0x16D + LOG_1X_BASE_C)
#define LOG_QTV_SDP_FILE_C                              (0x16E + LOG_1X_BASE_C)
#define LOG_QTV_RTSP_CLIENTRESPONSE_C                   (0x16F + LOG_1X_BASE_C)
#define LOG_QTV_RTP_HDR_C                               (0x170 + LOG_1X_BASE_C)
#define LOG_QTV_RTP_PKT_C                               (0x171 + LOG_1X_BASE_C)
#define LOG_QTV_RTP_AUDIO_SAMPLE_C                      (0x172 + LOG_1X_BASE_C)
#define LOG_QTV_RTP_VIDEO_SAMPLE_C                      (0x173 + LOG_1X_BASE_C)

/*(0x173+LOG_1X_BASE_C) is the last reserved log code for Qtv log code space1*/

/* The following is the allocated second log code space for Qtv */

/* 432-480 Qtv reserved logs
 #define LOG_QTV2_RESERVED_CODES_BASE_C                 (0x1B0 + LOG_1X_BASE_C)
 #define LOG_QTV2_LAST_C                  (48 + LOG_QTV2_RESERVED_CODES_BASE_C)
*/

#define LOG_QTV_RTP_TEXT_SAMPLE_C                      (0x1B0 + LOG_1X_BASE_C)
#define LOG_QTV_PLAYER_TIMED_TEXT_C                    (0x1B1 + LOG_1X_BASE_C)
#define LOG_QTV_FRAME_DECODE_C                         (0x1B2 + LOG_1X_BASE_C)
#define LOG_QTV_FRAME_RENDER_C                         (0x1B3 + LOG_1X_BASE_C)
#define LOG_QTV_AV_SYNC_C                              (0x1B4 + LOG_1X_BASE_C)
#define LOG_QTV_STREAMING_SDP_PARAMS                   (0x1B5 + LOG_1X_BASE_C)
#define LOG_QTV_PDS2_STATS                             (0x1B6 + LOG_1X_BASE_C)  
#define LOG_QTV_PDS2_GET_REQUEST                       (0x1B7 + LOG_1X_BASE_C)
#define LOG_QTV_PDS2_GET_RESP_HEADER                   (0x1B8 + LOG_1X_BASE_C)  

#define LOG_QTV_CMX_AUDIO_DATA_C                       (0x1BA + LOG_1X_BASE_C)   
#define LOG_QTV_RTSP_OPTIONS_C                         (0x1BB + LOG_1X_BASE_C)
#define LOG_QTV_RTSP_GET_PARAMETER_C                   (0x1BC + LOG_1X_BASE_C)
#define LOG_QTV_RTSP_SET_PARAMETER_C                   (0x1BD + LOG_1X_BASE_C)
#define LOG_ARM_VIDEO_DECODE_STATS                     (0x1BF + LOG_1X_BASE_C)
#define LOG_QTV_CMD_LOGGING_C                          (0x1C1 + LOG_1X_BASE_C)  

#define LOG_QTV_AUDIO_FRAME_PTS_INFO_C                 (0x1C2 + LOG_1X_BASE_C)
#define LOG_QTV_VIDEO_FRAME_DECODE_INFO_C              (0x1C3 + LOG_1X_BASE_C)
#define LOG_QTV_RTCP_COMPOUND_RR_C                     (0x1C4 + LOG_1X_BASE_C)
#define LOG_QTV_FRAME_BUFFER_RELEASE_REASON_C          (0x1C5 + LOG_1X_BASE_C)
#define LOG_QTV_AUDIO_CHANNEL_SWITCH_FRAME_C           (0x1C6 + LOG_1X_BASE_C)
#define LOG_QTV_PCR_DRIFT_RATE_C                       (0x1C8 + LOG_1X_BASE_C)
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

#ifndef PLATFORM_LTK

/* QTV Frame-render event */
LOG_RECORD_DEFINE(LOG_QTV_FRAME_RENDER_C)
  /* display timestamp for the frame */
  uint32 timestamp;
  /* frame size, in bytes */
  uint32 nBytes;
  /* Frame dimensions */
  uint32 height;
  uint32 width;
LOG_RECORD_END

/* QTV Frame-decode event */
LOG_RECORD_DEFINE(LOG_QTV_FRAME_DECODE_C)
  /* display timestamp for the frame */
  uint32 timestamp;
  /* non-zero if it's an I-frame */
  uint8  isIntraFrame;
  /* number of bytes in decode buffer */
  uint32 nBytes;
  /* # of bytes processed in decoding frame */
  uint32 nBytesProcessed;
  /* # of MBs in error in a frame */
  uint32 percentConcealedMBs;
LOG_RECORD_END

/* QTV A/V sync event */
LOG_RECORD_DEFINE(LOG_QTV_AV_SYNC_C)
  /* display timestamp for the frame */
  uint32 timestamp;
  /* ms delay: negative means video is late, positive ahead */
  int32  delay;
  /* Framed dropped information*/
  uint8 FrameDropped;
LOG_RECORD_END

/* QTV Peak and average statistic */
LOG_RECORD_DEFINE(LOG_ARM_VIDEO_DECODE_STATS)
  /* Average ARM decode time */
  uint32 average_dec_time_ms; 
  /* Average ARM decode time */
  uint32 peak_dec_time_ms;
  /* Type of video played*/
  uint8 video_type_played;
LOG_RECORD_END
typedef LOG_ARM_VIDEO_DECODE_STATS_type log_arm_decode_stats_type;

/* QTV frame buffer release reason */
LOG_RECORD_DEFINE(LOG_QTV_FRAME_BUFFER_RELEASE_REASON_C)
  /* Release reason */
  uint8 ReleaseReason; 
  /* Timestamp */
  uint32 TimeStamp;
LOG_RECORD_END
typedef LOG_QTV_FRAME_BUFFER_RELEASE_REASON_C_type log_qtv_frame_release_reason_type;

/* Clip statistics */
LOG_RECORD_DEFINE(LOG_MPEG4_CLIP_STATS_VER3_C)
  /* (PV) Actual playback rate (frames decoded only) */
  uint32  DecodedFrameRate;
  /* (PV) Actual playback rate (frames displayed) */
  uint32  DisplayedFrameRate;
  /* (Codec) Number of frames in the clip */
  uint32  NumberFrames;
  /* (Codec) Number of frames dropped by the codec */
  uint32  NumberFramesDroppedUnrecoverable;
  /* (PV) Number of frames dropped due to insufficient decode time */
  uint32  NumberFramesDroppedDecodeTime;
  /* (PV) Number of frames dropped due to insufficient rendering time */
  uint32  NumberFramesDroppedRenderTime;
  /* (PV) Max time(ms) that video was ahead of audio */
  uint32  MaxMsecVideoAhead;
  /* (PV) Max time(ms) that video was behind audio */
  uint32  MaxMsecVideoBehind;
  /* (PV) Encoded image bitrate */
  uint32  VideoEncodedBitrate;
  /* (PV) Encoded video frame rate */
  uint32  VideoEncodedFrameRate;
  /* (PV) Audio format */
  uint32  AudioFormat;
  /* (PV) Video format */
  uint32  VideoFormat;
  /* (PV) Count of I Frames */
  uint32 nIFrameTally;
  /* (PV) Count of P Frames */
  uint32 nPFrameTally;
  /* (PV) Count of B Frames */
  uint32 nBFrameTally;
  /* (PV) Encoded audio bitrate */
  uint32  AudioEncodedBitrate;
  /*Drops due to AV sync failure*/
  uint32 videoAVSyncDrops;
  /*Number of frames dropped due to MB errors*/
  uint32 FramesDroppedofMacroBlockErrors;
  /*Max consecutive Framedrops*/
  uint32 MaxConsecVideoDrops;
  /* Num frames interpolated*/
  uint32 VideoFramesInterpolated;
  /*max Video Lead */
  int32  maxVideoLead;
  /* min Video Lead */
  int32  minVideoLead;
  /*number of Audio Frames*/
  uint32 numAudioFrames;
  /*max Audio Data Lead*/
  int32 maxAudioDataLead;
  /* min Audio Lead*/
  int32 minAudioDataLead;
  /*max consecutive Audio Drops*/
  uint32 maxAudioDrop;
  /*null Audio Samples*/
  uint32 nullAudioSamples;

LOG_RECORD_END
typedef LOG_MPEG4_CLIP_STATS_VER3_C_type  log_mpeg4_clip_stats_type;

/* Audio Frame TS */
LOG_RECORD_DEFINE(LOG_QTV_AUDIO_FRAME_PTS_INFO_C)
  /* PTS of the frame */
  uint32 AudioFramePTS;
  /* Interpolated/Silence flag */
  uint8 isSilenceInserted;
LOG_RECORD_END
typedef LOG_QTV_AUDIO_FRAME_PTS_INFO_C_type  log_qtv_AudioFramePTS_type;

/* Video Frame Stats */
LOG_RECORD_DEFINE(LOG_QTV_VIDEO_FRAME_DECODE_INFO_C)
  /* Frame Type*/
  uint8 FrameType;
  /* SuperFrame ID */
  uint32 SuperFrameID;
  /* Layer decoded 0-Base,1-Enhanced,2-both*/
  uint8 Layer_Decoded;
  /* Intra MB's in base layer of the frame */
  uint16 NumIntraMBinBase; 
  /* Intra MB's in Enh layer of the frame */
  uint16 NumIntraMBinEnh; 
  /* Inter MB's in base layer of the frame */
  uint16 NumInterMBinBase; 
  /* Inter MB's in Enh layer of the frame */
  uint16 NumInterMBinEnh; 
  /* MacroBlocks in Error in base layer */
  uint16 NumErrorMBinBase; 
  /* MacroBlocks in Error in Enh layer */
  uint16 NumErrorMBinEnh; 
  /* Frame PTS */
  uint32 VideoFramePTS;
  /* IsInterpolated*/
  uint8 isInterpolated;
LOG_RECORD_END
typedef LOG_QTV_VIDEO_FRAME_DECODE_INFO_C_type  log_QTV_VideoFrame_DecodeStats_type;

LOG_RECORD_DEFINE(LOG_QTV_AUDIO_CHANNEL_SWITCH_FRAME_C)
  /*  timestamp of the channel switched frame */
  uint32 ChannelSwitchedTS;
  /* Approximate delay between actual time and reported time */
  uint32 ApproxDelay;
LOG_RECORD_END
typedef LOG_QTV_AUDIO_CHANNEL_SWITCH_FRAME_C_type log_qtv_audio_channel_type;

#define LOG_DSP_MAX_SLICE_BUFFER_SIZE 512
LOG_RECORD_DEFINE(LOG_QTV_DSP_SLICE_BUFFER_C)
  uint16 ChunkNumber;
  uint16 DataSize;
  uint8  SliceBuffer[LOG_DSP_MAX_SLICE_BUFFER_SIZE];
LOG_RECORD_END
typedef LOG_QTV_DSP_SLICE_BUFFER_C_type log_dsp_slice_buffer_type;

/* QTV PCR drift rate event */
LOG_RECORD_DEFINE(LOG_QTV_PCR_DRIFT_RATE_C)
  /* display timestamp for the frame */
  uint32 timestamp;
  /* SR drift rate:  */
  int  srDriftRate;
    /* RD drift rate */
  int  rdDriftRate;
LOG_RECORD_END

/* For video bitstream logging */
#define LOG_BITSTREAM_MAX_SIZE 512
LOG_RECORD_DEFINE(LOG_QTV_VIDEO_BITSTREAM)
  uint16 ChunkNum;
  uint32 NBytes;
  uint8  Data[LOG_BITSTREAM_MAX_SIZE];
LOG_RECORD_END
typedef LOG_QTV_VIDEO_BITSTREAM_type log_video_bitstream_type;
#endif /* ! PLATFORM_LTK */

/* =========================================================================
** RTCP log packet definition
** =========================================================================*/
/* APP_COMMON_LOG for RTCP_PVSS_APP_LOG */
typedef PACKED struct 
{
  uint32 sendTime;
  uint32 recvRate;   // bits/sec
  uint32 recvRateInterval;
  uint32 playbackBufDepth;
  uint32 highestCtrlMediaSeqNum;
  uint32 cumulativeBytes;
} APP_COMMON_LOG;

/* APP_EXTRA_DRC_LOG for RTCP_PVSS_APP_LOG */
typedef PACKED struct
{
  uint16 rebufCount;
  uint16 missingPackets;
  uint32 cumulativePacketsReceived;
  uint32 totalProcessedFrames;
  uint32 totalSkippedFrames;
  uint32 cumulativePacketsLost;
} APP_EXTRA_DRC_LOG;

/* APP_EXTRA_BUFLOW_LOG for RTCP_PVSS_APP_LOG */
typedef PACKED struct 
{
  uint16 depletionRateInteger;
  uint16 depletionRateFraction;
} APP_EXTRA_BUFLOW_LOG;
  
/* APP_EXTRA_BUFHIGH_LOG for RTCP_PVSS_APP_LOG */
typedef PACKED struct
{
  uint16 fillRateInteger;
  uint16 fillRateFraction;
} APP_EXTRA_BUFHIGH_LOG;

/* PVSS App specific report types */
typedef PACKED union
{
  APP_EXTRA_DRC_LOG extraDRC;
  APP_EXTRA_BUFLOW_LOG extraBufLow;
  APP_EXTRA_BUFHIGH_LOG extraBufHigh;
} PVSS_APP_EXTRA;


/* RTCP_PVSS_APP_LOG  for RTCP_APP  types */
typedef PACKED struct
{
  uint8 subtype;
  APP_COMMON_LOG common;
  PVSS_APP_EXTRA pvss_app_extra;
} RTCP_PVSS_APP_LOG;

/* RTCP_NADU_APP_LOG for RTCP_APP  types */
typedef PACKED struct
{
  uint32 ssrc;
  uint16 playout_delay;
  uint16 nsn;
  uint16 rsv_nun;
  uint16 fbs;
} RTCP_NADU_APP_LOG;

/* LGT_BILLING_INFO_LOG for RTCP_APP  types*/
typedef struct
{
  uint32 len;
  char ptr[1];
} LGT_BILLING_INFO_LOG;

/* App types for RTCP */
typedef union
{
  RTCP_PVSS_APP_LOG pvss_app_data;
  RTCP_NADU_APP_LOG nadu_app_data;
  LGT_BILLING_INFO_LOG lgt_billing_app_data; //this for APPs other than "PVSS"
} RTCP_APP_TYPE;

// RTCP_APP_LOG extensions for RTCP_RR
typedef struct
{
  uint8 subtype;
  uint32 ssrc;
  char type[4];
  RTCP_APP_TYPE app_type;
} RTCP_APP_LOG;

// RTCP_SDES_LOG
typedef struct
{
  uint8 type;
  uint8 length;
  char des[1]; //text not null terminated
} RTCP_SDES_LOG;

// RTCP_PROFILE_EXT_LOG
typedef struct
{
  uint32 length;
  char data[1]; 
} RTCP_PROFILE_EXT_LOG;

typedef struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  uint8  channelNo;
  uint32 sourceSSRC;
  uint8  fractionLost;
  uint32 cumulativeNumberOfPacketsLost;
  uint32 highestSequenceNumberReceived;
  uint32 interarrivalJitter;
  uint32 lastSR;
  uint32 delaySinceLastSR;
  uint32 cumulativeBytesReceived;   /* This field is not present in actual
                                       RTCP RR protocol packet. We use this
                                       here as an aid for more debug info */
  RTCP_PROFILE_EXT_LOG rtcpProfileExtLog;
  RTCP_SDES_LOG rtcpSdesLog;
  RTCP_APP_LOG rtcpAppLog;
} RTCP_COMPOUND_RR_LOG;

typedef PACKED struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  uint8  channelNo;
  uint32 sourceSSRC;
  uint8  fractionLost;
  uint32 cumulativeNumberOfPacketsLost;
  uint32 highestSequenceNumberReceived;
  uint32 interarrivalJitter;
  uint32 lastSR;
  uint32 delaySinceLastSR;
  uint32 cumulativeBytesReceived;   /* This field is not present in actual
                                       RTCP RR protocol packet. We use this 
                                       here as an aid for more debug info */
} RTCP_RR_LOG;

typedef PACKED struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  uint32 senderSSRC;
  uint32 NTP_timestamp_low;
  uint32 NTP_timestamp_high;
  uint32 RTP_timestamp;
  uint32 packet_count;
  uint32 octet_count;
  /* At present, support for RTCP RR blocks is not necessary */
} RTCP_SR_LOG;

/* =========================================================================
** RTP log packet definition
** =========================================================================*/

typedef PACKED struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  uint8  channelNo;

  /* Size of this RTP packet. Current code produces this log packet for every
  five RTP packets received on this channel */
  uint32 sizeOfThisRTPPacket;

  /* Cumulative no of RTP packets rcvd on this channel's lifetime */
  uint32 totalNoOfPacketsRcvdOnThisChannel;

  /* Currnet no of packets residing on this channel */
  uint32 currentNoOfPacketsBufferedOnThisChannel;

  /*  Presentation duration for currentNoOfPacketsBufferedOnThisChannel */
  uint32 durationOfBufferedDataInMsec;

  /* Flag to note that end of the stream is detected */
  uint8  endOfStreamDetected;

  /* Network jitter (in timestamp units) */
  int32 jitter;
  
  /* Channel Data rate is BPS*/
  uint32 dataRate;
} RTP_STATS_LOG;

typedef PACKED struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  uint8  channelNo;

  /* Computed for this channel,over the most recent RTCP Interval */
  uint32 chDataRxRateInBPS;

  /* Computed over the most recent RTCP Interval, across all RTP channels*/
  uint32 totalDataRxRateInBPS;

  /* Amount of presentation data currently residing on RTP channel */
  uint32 duration_in_buffer;

  /* 1/4 th of the maximum buffer duration */
  uint32 buf_low_mark_duration;

  /* maximum buffer duration - buf_low_mark_duration */
  uint32 buf_high_mark_duration;

  /* Default: 0. Set to 1, if buffered duration is below low WM */
  uint8  durationIsBelowLowMark;

  /* Default: 0 .Set to 1, if buffered duration is above high WM */
  uint8  durationIsAboveHighMark;

} STREAMING_STATS_LOG;

/* =========================================================================
** RTSP log packet definition
** =========================================================================*/

void log_rtsp_msg(int, char *, uint16);

typedef struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  uint32 cnt;
  char rtsp_byte[1];
} RTSP_LOG;

/* =========================================================================
** HTTP log packet definition for FASTTRACK
** =========================================================================*/

void log_pds2_http_msg(int, char *, uint16);

typedef struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  uint32 cnt;
  char http_msg_byte[1];
} HTTP_PDS2_LOG;


/* =========================================================================
** SDP log packet definition
** =========================================================================*/

typedef struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  uint32 cnt;
  char sdp_byte[1];
} SDP_LOG;

/* =========================================================================
** Timed-Text log packet definition
** =========================================================================*/

typedef struct
{
  uint32 render_time;  /* text sample rendered time (NPT), in milli-second */
  uint32 duration;     /* duration of this text sample, in milli-second */
  uint32 timestamp;    /* presentation timestamp of this text sample */
} TIMED_TEXT_LOG_INFO;

typedef struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  TIMED_TEXT_LOG_INFO timed_text_info;
  uint16 size;
  char content[1];     /* content of this text sample */
} TIMED_TEXT_LOG;

/* Structure definitions for RTP Sample logs */
typedef PACKED struct
{
    uint8 trackno; /* Media track number */
    uint32 rtp_timestamp; /* RTP Time Stamp units */
    uint32 display_timestamp;  /* RTP TS in milliseconds scale */

    uint8 enh_video_layer_no; /*  Set non-zero for samples from enhanced
                       video layer else set to 0 */
    uint32 rawts; /* sample raw timestamp */

}RTP_SAMPLE_LOG_INFO;

typedef PACKED struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  RTP_SAMPLE_LOG_INFO  sample_info;
  uint32 size;  /* size of content */
  char content[1];    /* content of this sample (audio or video).
                       * Multiple video layers may be present */
}RTP_SAMPLE_LOG;

/* Structure definition for RTP header/packet logging */
typedef struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  uint32 size;  /* RTP header (usually 12 bytes.But,it is variable)
                 * or RTP packet size */
  char rtp_content[1]; /* RTP header or full packet data */
} RTP_LOG;

typedef PACKED struct
{
  log_hdr_type hdr; 
  int audio_samplingrate;
  int audio_payloadtype;
  int audio_channelnum;
  int video_samplingrate;
  int video_payloadtype;
  int video_channelnum;
  int enhancedvideo_samplingrate;
  int enhancedvideo_payloadtype;
  int enhancedvideo_channelnum;
} QTV_STREAMING_SDP_PARAM;

typedef PACKED struct
{
  log_hdr_type hdr;    /* Log header (length, code, timestamp) */
  
  /* Computed for this channel,over the most recent RTCP Interval */
  int DataRateInBPS;

  /* Total Playback time of the clip*/
  int ClipStopTime;

  /* Current Download time*/
  int DownloadTime;
  
  /* Amount of presentation data Buffer can hold */
  int total_bytes_buffer_can_hold;

  /* Amount of presentation data Buffer present in the Buffer */
  int total_bytes_in_buffer;
  
} PDS2_STATS_LOG;

/* -----------------------------------------------------------------------*
* Function Declarations*
* ----------------------------------------------------------------------- */
void log_sdp(int cnt, const char *buffer);
#ifdef __cplusplus
void log_timed_text_msg(int, char *, TIMED_TEXT_LOG_INFO&, uint16);
#endif //cpluscplus
void log_rtp_header(uint32 cnt, char *buffer);
void log_rtp_packet(uint32 cnt, char *buffer);

/* Refer to StreaMedia class(StreamMedia.cpp) for methods which log audio,video
** and text samples
*/
/*
 * Type definition for Double Argument. 
 * Description:- 
 * Sign, integer part followed by fractional part. 
 */
typedef PACKED struct
  {
    byte sign; /* bool 1-positive, o-negative */
    uint32 arg_double_int_part;
    uint16 arg_double_frac_part;
  } QTV_ARG_DOUBLE; 

/*
 * Type definition for String Argument. 
 * Description:- 
 * Length followed by variable length string. 
 */
typedef PACKED struct
  {
    uint16 arg_str_len;
    char arg_str[1];
  } QTV_ARG_STR ;

/*
 * Type definition for Argument value structure. 
 * Description:- 
 * This is a union of integer/double/string
 */
typedef PACKED union
{
  int32 arg_int;
  uint32 arg_uint;
  QTV_ARG_DOUBLE qtv_arg_double;                     
  QTV_ARG_STR qtv_arg_str;
} QTV_CMD_ARG_TYPE;
/*
 * Type definition for Argument specific structure. 
 * Description:- 
 * This is a pair arg type followed by the specifc argument details. 
 */
typedef PACKED struct 
{
  char arg_d_type;
  QTV_CMD_ARG_TYPE arg_type;
} QTV_CMD_ARG;

/*
 * Type definition for Log Command packet prefix or fixed block. 
 * Description:- 
 * Immediately after the fixed portion variable portion begins. 
 * Variable portion has the command name followed by number of arguemnts. 
 * After that each of the arguments is logged as (argType,arg) pair. . 
 * 
 * Diagram representation of fixed portion is given below: 
 * +----------+-------------+----------------+
 * | hdr (12) | timestamp(4)| cmd_name_len(2)|   
 * +----------+-------------+----------------+
 * 
 * Diagram represention of the variable portion following the fixed portion is given below: 
 * +---------------------------+------------+-------------+----------------------+
 * | cmd_name(variable length) | num_args(2)| arg_type(1) | arg (arg_str/arg_int)|
 * +---------------------------+------------+-------------+----------------------+ 
 */
typedef PACKED struct
{
  log_hdr_type hdr; /* Log header (length, code, timestamp) */
  uint32 timeStamp;
  uint16 cmd_name_len;
} QTV_LOG_CMD_TYPE;

void log_rtsp_msg(int cnt, char *buffer, uint16 log_code);

#endif  /* QTV_LOG_H */

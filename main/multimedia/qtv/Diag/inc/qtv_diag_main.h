#ifndef QTV_DIAG_H
#define QTV_DIAG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          QTV Diagnostic Packet Service Client Routines

General Description
  This file contains declaration for QTV Diag test commands via the 
  DMSS diagnostics packet service.

Copyright (c) 2002-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

$Header: //source/qcom/qct/multimedia/qtv/diag/main/latest/inc/qtv_diag_main.h#9 $
$DateTime: 2010/06/10 03:10:55 $
$Change: 1329416 $
                           Edit History

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/04   anandr  Initial Revision
===========================================================================*/

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"

#include "diag.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "vdecoder_types.h"

#include "rex.h"
#include "task.h"

typedef rex_crit_sect_type CRITICAL_SECTION;

#include "QtvPlayer.h"
#include "oscl_file_io.h"
#include "qtv_conc_mgr.h"

extern bool audioBuf_log_flag;

/***************************************************************
***********QTVDIAG TASK AND PARAMETER DEFINITIONS END************
***************************************************************/
#define QTVDIAG_REPLAY_MAX_API_ARGS             10
#define QTVDIAG_REPLAY_KEEPLIVE_TIMEOUT         5000

#define QTVDIAG_CPU_LOADING_TIME_RESOLUTION     100     // 100 Msec

#define FEATURE_QTVDIAG_REPLAY                 /*  for Qtv API replay */
//#define FEATURE_QTVDIAG_REPLAY_DEBUG         /* for Qtv API replay debugging */

#define FEATURE_QTV_DEBUG_MEMORY_FAILURE
#define FEATURE_QTV_DEBUG_CPU_LOADING

/* specific cmd to run one of the QTV APIs*/ 
typedef PACKED struct 
{
  diagpkt_subsys_header_type header;

  // An error code indicating any problems QTV Diag had issuing the 
  // command. Values are from qtvdiag_cmd_error_code_e_type.
  uint8  diag_error_code;

  // An error code returned by the QTV player method that the command 
  // invoked. Values are from QTVPlayer::ReturnT.
  uint32  qtv_return_code; 
} qtvdiag_command_response_s_type;

typedef PACKED struct 
{
  diagpkt_subsys_header_type header;
  char  value[7]; 
} qtvdiag_command_test_response_s_type;

// Response packet structure for parrot message 
typedef PACKED struct 
{
  diagpkt_subsys_header_type header;
  uint32 buffersize; 
} qtvdiag_command_parrot_response_s_type;

typedef PACKED struct
{
  diagpkt_subsys_header_type header;

  uint16  api_id;
} qtvdiag_packet;

//Define play modes for ISDBT - TSB filter
typedef enum
{
   QTV_DIAG_ISDBT_NOP = 0,
   QTV_DIAG_ISDBT_RESUME = 1, //Resume playback from TSB from Paused State
   QTV_DIAG_ISDBT_PLAY_LIVE = 2 //Play live from current buffering state
}BCastResumeModeFromPauseT;

typedef struct CallInfoT
{
    cm_mm_call_event_f_type * call_event_func;
    cm_call_event_e_type call_event;
    cm_mm_call_info_s_type *call_info;
}CallInfoT;

typedef  struct QtvDiagAppT
{

  /* CPU loading members */
  uint8 nPercentageOfCPULoading;
  uint32 nCPULoadingStartTime;
  int32 nDurationOfCPULoading;
  int32 nPendingDurationOfCPULoading;
  rex_timer_type* pQtvDiagCPULoadingTimer;
  /*Frame info structure  */
  QtvPlayer::FrameInfoT  nFrameInfoData;
  BCastResumeModeFromPauseT BCastResumeModeFromPause;

  /*Structure to hold call related data. Used for diag testing of in-call qtv*/
  CallInfoT *call_info_ptr;
} QtvDiagAppT;

typedef struct MessageStructT
{
    char   *api_arr_ptr;
    char   *response_pkt_payload_ptr;
    uint32 curr_api_len;
    bool   exec_api_return;
}QtvDiagMessageStructT;

typedef enum
{
  QTV_DIAG_REPLAY_INIT,
  QTV_DIAG_REPLAY_PROCESS_NEXT_API,
  QTV_DIAG_REPLAY_WAIT_ON_TIME_DELTA,
  QTV_DIAG_REPLAY_WAIT_ON_EVENT,
  QTV_DIAG_REPLAY_DONE

} QtvDiagReplayStateT;

typedef  struct QtvReplayDiagAppT
{
  typedef  struct QtvReplayDiagAPIArgsT
  {
    uint8* pArgBuffer;
    uint32 nArgSize;
  } QtvReplayDiagAPIArgsT;

  typedef PACKED struct QtvReplayDiagAPIT
  {
    uint32 nAPISeqNumber;
    uint32 nTimeDelta;
    int32  nEventId;
    uint32 nEventTimeOut;
    uint32 nAPIId;
    uint32 nNumberOfAPIArgs;
  } QtvReplayDiagAPIT;

  QtvReplayDiagAPIT currentAPICmdInfo;
  QtvReplayDiagAPIArgsT currentAPIArgsInfo[QTVDIAG_REPLAY_MAX_API_ARGS];
  uint32 currentAPIOffset;
  boolean bIsCurrentAPIInfoValid;
  boolean bCheckAPIEventSensitivity;
  uint32 numQtvInstances;
  OSCL_FILE * pQtvDiagReplayInputFilePtr;
#ifdef FEATURE_QTVDIAG_REPLAY_DEBUG 
  OSCL_FILE * pQtvDiagReplayDebugFilePtr;
#endif  /*#ifdef FEATURE_QTVDIAG_REPLAY_DEBUG  */

  uint32 nReplayStartTime;
  QtvDiagReplayStateT nState;
  uint32 nLastKeepLiveMessageTime;
  boolean bCheckKeepLiveMessages;

  rex_timer_type* pQtvDiagReplayTimer;
  rex_timer_type* pQtvDiagReplayKeepLiveTimer;

} QtvReplayDiagAppT;

typedef enum
{
  QTVDIAG_REPLAY_SUCCESS,
  QTVDIAG_REPLAY_INVALID_ARGS,
  QTVDIAG_REPLAY_READ_ERROR,
  QTVDIAG_REPLAY_MEMORY_ERROR,
  QTVDIAG_REPLAY_ERROR  
} QtvDiagReplayErrorType;

// Values for the diag_error_code field of 
// qtvdiag_event_command_response_s_type 
typedef enum 
{
  // Indicates the command was issued successfully
  QTVDIAG_ERROR_CODE_COMMAND_ISSUED,

  // Indicates an error occured issuing the command -- check the qtv_return_code
  // field.
  QTVDIAG_ERROR_CODE_GENERIC_ERROR,

  // Indicates the command could not be issued due to an illegal state
  QTVDIAG_ERROR_CODE_ILLEGAL_STATE,

  // Indicates the command is known, but not supported in the current 
  // build of QTV
  QTVDIAG_ERROR_CODE_COMMAND_UNSUPPORTED,

  // Indicates the command is unknown
  QTVDIAG_ERROR_CODE_COMMAND_UNKNOWN,

  // Indicates a memory allocation error occured
  QTVDIAG_ERROR_CODE_OUT_OF_MEMORY
} qtvdiag_cmd_error_code_e_type;

   
/* The identifiers below are values for fields of request packets, and are
 * used by QTV Diag to determine which command to execute as well as 
 * parameters for the command. Each field value is documented with a remainder
 * packet format, which determines the contents of the request packet after
 * the value appears. When fields are more than 1 byte long, they are in 
 * network byte order.
 */

/* Identifiers for QTV commands. Note: Do not make any insertions, or else
 * the values afterwards will change, which will cause the QXDM database to
 * be invalid. Additional commands, just before QTVDIAG_COMMAND_CARDINALITY,
 * are allowed though.
 */
typedef enum
{ 
  // Start the QTV Diag task.
  // Remainder packet format: None
  QTVDIAG_COMMAND_START_TASK = 0,

  // End the QTV Diag task.
  // Remainder packet format: None
  QTVDIAG_COMMAND_END_TASK = 1,

  // Register the QtvDiag callback function which allows frames to be rendered
  // and notifies the host of command completion status.
  // Remainder packet format: None
  QTVDIAG_COMMAND_REGISTER_FOR_CALLBACK = 2,

  // Allocates memory on the heap to simulate environments with less memory. 
  // If there is existing allocated memory due to a previous command, the 
  // previously allocated memory will be freed first.
  // Remainder packet format:
  //   Bytes 0 ~ 3: 32 unsigned bits indicating the number of bytes to
  //                allocate. May be 0.  
  QTVDIAG_COMMAND_USE_HEAP_MEMORY = 3,

  // Initialize the QTV player.
  // Remainder packet format: None
  QTVDIAG_COMMAND_INIT = 4,
  
  // Open a pair of URN's as the video and audio URN's, respectively.
  // Remainder packet format:
  //         0 ~ video URN string length: 
  //                The characters making up the the video URN, including 
  //                the terminating null character. If the string does not 
  //                begin with a '/', it will be interpreted as a URN relative
  //                to the normal location  of the clips for the media player.
  //   Bytes 1 + video URN string length ~ 
  //         1 + video URN string length + audio URN string length:
  //                The characters making up the the audio URN, including 
  //                the terminating null character. If the string does not 
  //                begin with a '/', it will be interpreted as a URN relative
  //                to the normal location of the clips for the media player.
  QTVDIAG_COMMAND_OPEN_URN = 5,

  // Set the output buffers needed for decoding video clips. This command
  // must be issued before video clips are played.
  // Remainder packet format: 
  //   Bytes 0 ~ 3: 32 unsigned bits indicating width of frame.
  //   Bytes 4 ~ 7: 32 signed bits indicating height of frame.
  QTVDIAG_COMMAND_SET_OUTPUT_BUFFERS = 6,

  // Play the currently open clip. 
  // Remainder packet format:
  //   Bytes 0 ~ 3: 32 signed bits indicating the start
  //                position. If -1, resume from the last position.
  //   Bytes 4 ~ 7: 32 signed bits indicating the end
  //                position. If -1, play until the end.  
  QTVDIAG_COMMAND_PLAY = 7,    

  // Play the next frame. 
  // Remainder packet format: None.
  QTVDIAG_COMMAND_PLAY_NEXT_FRAME = 8,

  // Play the next frame. 
  // Remainder packet format: None.
  QTVDIAG_COMMAND_PLAY_PREVIOUS_FRAME = 9,

  // Pause the player.
  // Remainder packet format: None.
  QTVDIAG_COMMAND_PAUSE = 10,

  // Stop the player.
  // Remainder packet format: None.
  QTVDIAG_COMMAND_STOP = 11,

  // Seek to a position. This causes the player to first pause, then play
  // starting from the specified position, until the end.
  // Remainder packet format:
  //   Bytes 0 ~ 3: 32 signed bits indicating the position
  //                to seek to.
  QTVDIAG_COMMAND_SEEK = 12,

  // Skip to a random access point.
  // Remainder packet format:
  //   Bytes 0 ~ 3: 32 signed bits indicating the number of
  //                random access points to skip. Negative values indicate 
  //                skipping backwards.
  QTVDIAG_COMMAND_SKIP = 13,

  // Terminate the player.
  // Remainder packet format: None.
  QTVDIAG_COMMAND_TERMINATE = 14,

  // Rotate the decoded video as displayed on the screen.
  // Remainder packet format: 
  //   Byte 0: The value of one of the RotationType enum members.  
  QTVDIAG_COMMAND_ROTATE_VIDEO = 15,

  // Scale the decoded video as displayed on the screen.
  // Remainder packet format: 
  //   Byte 0: The value of one of the ScalingType enum members.  
  QTVDIAG_COMMAND_SCALE_VIDEO = 16,
  
  // Start recording the current clip.
  // Remainder packet format:
  //   Byte 0:      0 to disallow overwriting an existing file with the same 
  //                name, 1 to allow
  //   Byte 1:      Indicates the recording mode.
  //   Bytes 2 ~ 5: 32 unsigned bits indicating the duration, 
  //                in milliseconds to record.
  //   Bytes 6 ~ 9: 32 unsigned bits indicating the length of the output 
  //                filename, not including the terminating null character.
  //   Bytes 7 ~ 7 + output filename length - 1:
  //                The output filename, not including the 
  //                terminating null character.
  QTVDIAG_COMMAND_START_RECORDING = 17, 

  // Stop recording the current clip.
  // Remainder packet format: None
  QTVDIAG_COMMAND_STOP_RECORDING = 18, 

  // Reset the RTP statistics.
  // Remainder packet format: 
  //   Byte 0: Bit mask of the track numbers to be reset. To reset track n,
  //           turn on bit n (where bit 0 is the LSB). Since there are only
  //           a maximum of 3 tracks, 8 bits is more than enough.
  QTVDIAG_COMMAND_RESET_RTP_STATISTICS = 19,

  // Simulates an error decoding some type of media. 
  // Remainder packet format:
  //   Byte 0:    The value of one of the qtvdiag_decoding_error_e_types enum
  //              members indicating what kind of decoding error to simulate.               
  //   Bytes >=1: Determined by the decoding error type, usually not present,
  //              but may be used to further specify the type of error.  
  QTVDIAG_COMMAND_SIMULATE_DECODING_ERROR = 20,

  // Simulates a network condition at the RTP layer
  // Remainder packet format:
  //   Byte 0:    The value of one of the 
  //              qtvdiag_rtp_network_condition_e_type enum members indicating
  //              which network condition to simulate.
  //   Byte >=1:  Determined by the network condition type.
  QTVDIAG_COMMAND_SIMULATE_RTP_NETWORK_CONDITION = 21,

  // Start and stop QTV rapidly.
  QTVDIAG_COMMAND_START_STOP = 22,

  QTVDIAG_COMMAND_SET_DFLT_RECORDING_DIR = 23,

  // Set an item in the StreamerConfig database.
  // Remainder packet format:
  //   Bytes 0 ~ 4:   The value of one of the Qtvplayer::StreamingConfigItem enum 
  //                  members.
  //   Byte  5 ~ 8:   The corresponding value to set.
  QTVDIAG_COMMAND_SET_STREAM_CONFIG_ITEM = 24,

  // Rewind playback.
  // Remainder packet format:
  //   Bytes 0 ~ 4:  The number of milliseconds to rewind by.
  QTVDIAG_COMMAND_REWIND = 25,

  // Fast forward playback.
  // Remainder packet format:
  //   Bytes 0 ~ 4:  The number of milliseconds to fast forward by.
  QTVDIAG_COMMAND_FFWD = 26,

  //
  QTVDIAG_COMMAND_VDEC_CREATE = 27,

  //
  QTVDIAG_COMMAND_VDEC_DECODE = 28,

  //
  QTVDIAG_COMMAND_VDEC_DESTROY = 29,

  //
  QTVDIAG_COMMAND_VDEC_DUMP = 30,

  //
  QTVDIAG_COMMAND_VDEC_FLUSH = 31,

  //
  QTVDIAG_COMMAND_VDEC_FREE_BLOB = 32,

  //
  QTVDIAG_COMMAND_VDEC_PARSE_MEDIA_HEADER = 33,

  //
  QTVDIAG_COMMAND_VDEC_QUEUE = 34,

  //
  QTVDIAG_COMMAND_VDEC_REUSE_FRAME_BUFFER = 35,

  //
  QTVDIAG_COMMAND_VDEC_SET_DIAG_VARS = 36,

  //
  QTVDIAG_COMMAND_VDEC_CLEAR_DIAG_VARS = 37,

  // Relinquish onwership of the QDSP image so that another entity with lower 
  // priority than Qtv can use QDSP on a temporary basis.
  // Remainder packet format: None.
  QTVDIAG_COMMAND_SUSPEND = 38,
 
#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */
   
  // Skips to a clip in a playlist using 
  // Skip(int32 clipIndex, bool isClipIndexRelative, 
  //      bool preferClientPlaylistSkip)
  // Remainder packet format:
  //   Bytes 0 ~ 3: The clip index
  //   Byte  4: 0 if the clip index is relative, otherwise non-zero
  //   Byte  5: 0 if the skip is for the client playlist instead of the 
  //            server-side playlist if both are present, otherwise non-zero
  QTVDIAG_COMMAND_PLAYLIST_SKIP1 = 40,

  // Skips to a clip in a playlist using 
  // Skip(const char *playlistName, int32 clipIndex, bool isClipIndexRelative, 
  //      int32 time, int32 when)
  // Remainder packet format (strange order so that 32-bit quantities align
  // correctly):
  //         0 ~ playlist string length: 
  //                The characters making up the the playlist name, including 
  //                the terminating null character.
  //   Bytes 0 ~ 3  The clip index
  //   Bytes 4 ~ 7  The time after the start of the clip to begin playback.
  //   Bytes 8 ~ 11 When to make the clip transition. 
  //                -1 == now, -2 == at end of current clip, 
  //                -3 at end of playlist
  //   Byte 12      0 if the clip index is relative, otherwise non-zero
  //   Bytes 13 ~ 14 + playlist string length
  //                The new playlist to transition to.
  QTVDIAG_COMMAND_PLAYLIST_SKIP2 = 41,

  QTVDIAG_COMMAND_VDEC_INITIALIZE = 42,

  QTVDIAG_COMMAND_CARDINALITY
} qtvdiag_command_e_type; 

/* A constant to add to info types to use as a subsystem response code for
 * sending info as diag packets.
 */
#define QTVDIAG_INFO_OFFSET 128

/* Identifiers for information sent back to the host. For use for the API
 * command QTVDIAG_GET_INFO.
 */
typedef enum 
{
  QTVDIAG_INFO_PLAYER_STATE = QTVDIAG_INFO_OFFSET,
  QTVDIAG_INFO_PLAYER_STATUS,
  QTVDIAG_INFO_FRAME_INFO,
  QTVDIAG_INFO_CLIP_INFO,
  QTVDIAG_INFO_AV_STATISTICS,
  QTVDIAG_INFO_TIMED_TEXT_STATISTICS,
  QTVDIAG_INFO_RTP_STATISTICS,
  QTVDIAG_INFO_MAX
} qtvdiag_info_e_type;

/* Identifiers for the decoding errors that can be simulated with the API
 * command QTVDIAG_SIMULATE_DECODING_ERROR.
 */
typedef enum 
{
  QTVDIAG_DECODING_ERROR_GENERIC_VIDEO,
  QTVDIAG_DECODING_ERROR_GENERIC_AUDIO, 
  QTVDIAG_DECODING_ERROR_GENERIC_TIMED_TEXT,
  QTVDIAG_DECODING_ERROR_CARDINALITY
} qtvdiag_decoding_error_e_type;

// A "base class" for information about the info to send.
class QtvDiagInfoData {  

public:

  QtvDiagInfoData() { isSticky = false; }

  // Initialize the data from the part of the request packet after the
  // information type.
  QtvDiagInfoData(PACKED char *packet) { 
    isSticky = ((packet != NULL) && (*packet != 0));   
  }
  
  virtual ~QtvDiagInfoData() { }

  // If true, always send the info. Otherwise, send once only.
  bool isSticky; 

  // Protects infoArray
  static CRITICAL_SECTION cs;

  // Array of instances, shared among all instances.
  static QtvDiagInfoData *infoArray[QTVDIAG_INFO_MAX - QTVDIAG_INFO_OFFSET];

  static void Reset(bool init);

  static int  PayloadSizeForInfo(qtvdiag_info_e_type info_type, void *pInfoSource);

  static bool FillInfoPacketForDiag(qtvdiag_info_e_type info_type, PACKED char *info_ptr,
    QtvPlayer::PlayerStatusT status, void *pInfoSource);

  static bool FillPlayerStateInfoPacket(PACKED char *info_ptr);

  static void FillClipInfoInfoPacket(PACKED char *info_ptr, void *pInfoSource);

  static bool FillRTPStatisticsInfoPacket(PACKED char *info_ptr);

  static bool ShouldSend(qtvdiag_info_e_type info_type);

  static void SetSendInfo(qtvdiag_info_e_type info_type, PACKED char *packet);
  
  static void SendAll(int status);  

  static bool Send(qtvdiag_info_e_type info_type, QtvDiagInfoData *infoData, 
                   QtvPlayer::PlayerStatusT status);
};

/*===========================================================================

FUNCTION SystemTimeInMsec

DESCRIPTION
   Used to get the systemtime.

DEPENDENCIES
  None.

RETURN VALUE
  Returns system time in millisecs.

SIDE EFFECTS
  None.

===========================================================================*/

unsigned long SystemTimeInMsec(void);

/*===========================================================================

FUNCTION QTVDIAG_ALLOC_RESPONSE_PACKET

DESCRIPTION
  Allocate a subsystem response packet for an ordinary QTV command.
  Set the diag error code to QTVDIAG_ERROR_CODE_COMMAND_UNKNOWN and
  the QTV return code to QTV_RETURN_ERROR.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet, NULL if allocation fails.

SIDE EFFECTS
  None.

===========================================================================*/

qtvdiag_command_response_s_type *qtvdiag_alloc_response_packet( uint16 command_id); 

/* =========================================================================
 * The following section is done for Qtv replay and API tesing frame work
 * =========================================================================
 */
//#define QTV_DIAG_REPLAY
#define QTVDIAG_FRAMEWORK_START_CMD_ID          255
#define QTVDIAG_FRAMEWORK_STOP_CMD_ID           254
#define QTVDIAG_FRAMEWORK_SEND_TESTAPI_CMD_ID   253
#define QTVDIAG_FRAMEWORK_REPLAYAPI_CMD_ID      252
#define QTVDIAG_FRAMEWORK_STOP_REPLAY_CMD_ID    251

/* For Replay Framework */
#define QTVDIAG_REPLAY_START_CMD_ID             250
#define QTVDIAG_REPLAY_KEEP_LIVE_CMD_ID         249
#define QTVDIAG_REPLAY_STOP_CMD_ID              248

/* For Qtv Debug instructions */
#define QTVDIAG_MEMORY_FAILURE_CMD_ID           240
#define QTVDIAG_CPU_LOADING_CMD_ID              239

#define TEMP_BUFFER_MAX_SIZE          2000
#define DIAG_PAYLOAD_SIZE             247
#define APITEST_RESPONSE_PAYLOAD_SIZE 251 //256-4-1 (4-header 1-defragment status)
#define APIREPLAY_RESPONSE_PAYLOAD_SIZE 251 //256-4-4 (4-header 4-free buffersize)
#define API_HEADER_SIZE  8
#define MIN_URN_LEN 2
#define SIZE_OF_URN_LEN 4
#define RESPONSE_PKT_DEFAULT_PARAMS_SIZE 5
#define MIME_TYPE_MAX_LEN 242
#define MEDIA_TYPE_MAX_LEN 242

#define TARGET_PROTOCOL_VERSION 100001
#define TARGET_API_VERSION 100000

typedef enum
{ 
  //Init
  QTVDIAG_FRAMEWORK_INIT_API_ID = 1,

  //Register for call back
  QTVDIAG_FRAMEWORK_REGISTER_FOR_CALLBACK_API_ID = 2,

  //OpenURN
  QTVDIAG_FRAMEWORK_OPEN_URN_API_ID = 3,

  QTVDIAG_FRAMEWORK_OPEN_URN2_API_ID = 4,

  QTVDIAG_FRAMEWORK_OPEN_URN3_API_ID = 5,

  QTVDIAG_FRAMEWORK_OPEN_URN4_API_ID = 6,

  //Play clip
  QTVDIAG_FRAMEWORK_PLAY_API_ID = 7,

  //Pause
  QTVDIAG_FRAMEWORK_PAUSE_API_ID = 8,

  //Pause resume
  QTVDIAG_FRAMEWORK_PAUSE_RESUME_API_ID = 9,

  //Stop
  QTVDIAG_FRAMEWORK_STOP_API_ID = 10,

  //Terminate
  QTVDIAG_FRAMEWORK_TERMINATE_API_ID = 11,

  //Play next frame
  QTVDIAG_FRAMEWORK_PLAY_NEXT_FRAME_API_ID = 12,

  //Play previous frame
  QTVDIAG_FRAMEWORK_PLAY_PREVIOUS_FRAME_API_ID = 13,

  //Rewind
  QTVDIAG_FRAMEWORK_REWIND_API_ID = 14,

  //Fastforward
  QTVDIAG_FRAMEWORK_FFWD_API_ID = 15,

  //Rotate
  QTVDIAG_FRAMEWORK_ROTATE_VIDEO_API_ID = 16,

  //Scale
  QTVDIAG_FRAMEWORK_SCALE_VIDEO_API_ID = 17,

  //Seek
  QTVDIAG_FRAMEWORK_SEEK_API_ID = 18,

  //Skip
  QTVDIAG_FRAMEWORK_SKIP_API_ID = 19,

  //Record clip
  QTVDIAG_FRAMEWORK_RECORD_CLIP_API_ID = 20,

  //Stop clip recording
  QTVDIAG_FRAMEWORK_STOP_CLIP_RECORDING_API_ID = 21,

  //Start and stop
  QTVDIAG_FRAMEWORK_START_STOP_API_ID = 22,

  //Get frame info
  QTVDIAG_FRAMEWORK_GET_FRAME_INFO_API_ID = 23,

  //Get video codec type
  QTVDIAG_FRAMEWORK_GET_VIDEO_CODEC_TYPE_API_ID = 24,

  //Get audio codec type
  QTVDIAG_FRAMEWORK_GET_AUDIO_CODEC_TYPE_API_ID = 25,

  //Get audio video statistics
  QTVDIAG_FRAMEWORK_GET_AUDIO_VIDEO_STATS_API_ID = 26,

  //Get MIME type
  QTVDIAG_FRAMEWORK_GET_MIME_TYPE_API_ID = 27,

  //Get media type
  QTVDIAG_FRAMEWORK_GET_MEDIA_TYPE_API_ID = 28,

  //Get URN type
  QTVDIAG_FRAMEWORK_GET_URN_TYPE_API_ID = 29,

  //Get clip info
  QTVDIAG_FRAMEWORK_GET_CLIP_INFO_API_ID = 30,

  //Suspend
  QTVDIAG_FRAMEWORK_SUSPEND = 31,
   
  //SetAudioSource
  QTVDIAG_FRAMEWORK_SET_AUDIO_SOURCE= 32,
  
  //OpenPullBufferedStream
  QTVDIAG_FRAMEWORK_OPEN_PULL_BUFFERED_STREAM = 33,

   //GetRawFrameInfo
  QTVDIAG_FRAMEWORK_GET_RAW_FRAME_INFO = 34,

  //SetQtvConfigItem
  QTVDIAG_FRAMEWORK_SET_QTV_CONFIG_ITEM = 35,

  //GetLargetstFrameSize
  QTVDIAG_FRAMEWORK_GET_LARGEST_RAW_FRAME_SIZE = 36,
  
  //GetRawTrackHeader
  QTVDIAG_FRAMEWORK_GET_RAW_TRACK_HEADER = 37,

  //GetNumFrames
  QTVDIAG_FRAMEWORK_GET_NUM_FRAMES = 38,

  //GetRawFrame
  QTVDIAG_FRAMEWORK_GET_RAW_FRAME = 39,

  QTVDIAG_FRAMEWORK_GET_3GPP_TIMEDTEXT_INFO = 40,

  QTVDIAG_FRAMEWORK_GET_PLAYER_STATE = 41,

  QTVDIAG_FRAMEWORK_GET_DOWNLOAD_TIME = 42,

  QTVDIAG_FRAMEWORK_SCALE_VIDEO_API_ID_NEW = 43,

  //setplaybackspeed
  QTVDIAG_FRAMEWORK_SET_PLAYBACK_SPEED = 44,
  
  QTVDIAG_FRAMEWORK_MUTE_UNMUTE_PLAYBACK_TRACKS_ID = 45,

  QTVDIAG_FRAMEWORK_READ_PLAYBACK_TRACKS_ID = 46,

  QTVDIAG_FRAMEWORK_SELECT_PLAYBACK_TRACKS_ID = 47,
  
//PlaySyncFrame
  QTVDIAG_FRAMEWORK_PLAY_SYNC_FRAME = 48,
  
//SetDualMonoOutput  
  QTVDIAG_FRAMEWORK_SET_DUAL_MONO_OUTPUT = 49,
  
 //SetISDBTStatus
  QTVDIAG_FRAMEWORK_SET_ISDBT_STATUS = 50,

  QTVDIAG_CALLBACK_FOR_CM = 51,

 //OpenPseudoStream  
  QTVDIAG_FRAMEWORK_OPEN_PSEUDO_STREAM = 52,

//OpenPseudoStream 2
 QTVDIAG_FRAMEWORK_OPEN_PSEUDO_STREAM_3_ARGS = 53,
  
 //SetUserAgent
 QTVDIAG_FRAMEWORK_SET_USER_AGENT = 54,
  
 //SetVDECParameter
 QTVDIAG_FRAMEWORK_SET_VDEC_PARAMETER = 55,

 //Reset testapp underrun flag
 QTVDIAG_FRAMEWORK_RESET_TESTAPP_UNDERRUN_FLAG = 56
} qtvdiag_framework_api_id_e_type;

// Unpacking to a BYTE(8Bits) value
#define UNPACK_UINT8(var, pkt_ptr) \
{                                  \
  var = *((uint8 *)(pkt_ptr++));   \
}                                  \
// Unpacking to a WORD(16 Bits) value
#define UNPACK_UINT16(var, pkt_ptr)\
{                                  \
  uint8 msb,lsb;                   \
  lsb = *((uint8 *)(pkt_ptr++));   \
  msb = *((uint8 *)(pkt_ptr++));   \
  var = ((msb<<8)|lsb);            \
}                                  \
// Unpacking to a DOUBLE WORD(32Bits) value
#define UNPACK_UINT32(var, pkt_ptr)\
{                                  \
  uint8 msb,nmsb,nlsb,lsb;         \
  lsb = *((uint8 *)(pkt_ptr++));   \
  nlsb = *((uint8 *)(pkt_ptr++));  \
  nmsb = *((uint8 *)(pkt_ptr++));  \
  msb = *((uint8 *)(pkt_ptr++));   \
  var = ((msb << 24)|(nmsb << 16)|(nlsb << 8)|(lsb));   \
}                                  \
// The SKIP_UINT16 macro skips the byte stream "pkt_ptr" by 16 bytes
#define SKIP_UINT16(pkt_ptr) \
{                            \
  pkt_ptr += sizeof(uint16); \
}                            \
// The SKIP_UINT32 macro skips the byte stream "pkt_ptr" by 32 bytes
#define SKIP_UINT32(pkt_ptr) \
{                            \
  pkt_ptr += sizeof(uint32); \
}                            \

//Macro for filling the response packet
#define FILL_RESPONSE_PACKET_UINT8(response_ptr, var) \
{                                                         \
  memcpy(response_ptr,(uint8 *)&(var),sizeof(uint8));  \
  response_ptr += sizeof(uint8);                       \
}                                                         \

#define FILL_RESPONSE_PACKET_UINT16(response_ptr, var) \
{                                                         \
  memcpy(response_ptr,(uint8 *)&(var),sizeof(uint16));  \
  response_ptr += sizeof(uint16);                       \
}                                                         \

#define FILL_RESPONSE_PACKET_UINT32(response_ptr, var) \
{                                                         \
  memcpy(response_ptr,(uint8 *)&(var),sizeof(uint32));  \
  response_ptr += sizeof(uint32);                       \
}                                                         \

#define FILL_RESPONSE_PACKET_FLOAT(response_ptr, var) \
{                                                         \
  memcpy(response_ptr,(uint8 *)&(var),sizeof(float));  \
  response_ptr += sizeof(float);                       \
}                                                         \

#define FILL_RESPONSE_PACKET_INT32(response_ptr, var) \
{                                                         \
  memcpy(response_ptr,(uint8 *)&(var),sizeof(int32));  \
  response_ptr += sizeof(int32);                       \
}                                                         \

#define FILL_RESPONSE_PACKET_BOOL(response_ptr, var) \
{                                                         \
  memcpy(response_ptr,(uint8 *)&(var),sizeof(boolean));  \
  response_ptr += sizeof(boolean);                       \
}                                                         \

// Following enum describes the status of diag packet de fragmentation
typedef enum
{
  QTVDIAG_DEFRAG_ERROR,
  QTVDIAG_DEFRAG_BADFRAGMENT,
  QTVDIAG_DEFRAG_WAITINGFORMORE,
  QTVDIAG_DEFRAGMENT_DONE
} qtvdiag_defrag_messg_e_type;

//Following enum describes the whether response packet is already filled or not
typedef enum
{
  QTVDIAG_RESPONSE_CREATED,
  QTVDIAG_RESPONSE_NOT_CREATED
} qtvdiag_response_packet_e_type;

// Following is the structure for response packet of the qtvdiag_start_task command
typedef struct
{
  diagpkt_subsys_header_type header;
  char response_payload[APITEST_RESPONSE_PAYLOAD_SIZE];
} qtvdiag_starttask_response_s_type;

// Following is the structure for response packet of qtvdiag_testapi command
typedef struct
{
  diagpkt_subsys_header_type header;
  uint8 defragment_status;
  char response_payload[APITEST_RESPONSE_PAYLOAD_SIZE];
} qtvdiag_testapi_response_s_type;

typedef struct
{
  diagpkt_subsys_header_type header;
  uint8 defragment_status;
  char response_payload[APIREPLAY_RESPONSE_PAYLOAD_SIZE];
} qtvdiag_Replayapi_response_s_type;

/* enumeration used to identify each type of command */
typedef enum
{
   QTVDIAG_CMD_RENDER_FRAME
  ,QTVDIAG_CMD_HANDLE_PLAY
  ,QTVDIAG_REPLAY_CMD_EXEC_CURRENT_API
  ,QTVDIAG_CMD_CPU_LOADING
  ,QTVDIAG_CMD_CARDINALITY
  ,QTVDIAG_CMD_EXEC_API_CALL //Exexute qtvdiag_exec_api function in Qtv Diag context
}qtvdiag_cmd_e_type;

/* command structure used by qtvdiag */
typedef struct
{
  q_link_type          link;
  qtvdiag_cmd_e_type   cmd_id;
  uint32               cmd_data;
}qtvdiag_cmd_s_type;

/* end Cmd interface for qtvdiag */

/*===========================================================================

FUNCTION 
  QTVDIAG_DEFRAG_DIAGPACKET
  
DESCRIPTION
  This function gets the diag packet as input and checks whether
  it is a  fragment or a full API packet. In case of diag packet 
  containing  fragmentof API  information it waits for more diag 
  packets to form a full API packet.

DEPENDENCIES
  None.

RETURN VALUE
  Defragment status

SIDE EFFECTS
  None.

===========================================================================*/
qtvdiag_defrag_messg_e_type qtvdiag_defrag_diagpacket(const char *diag_pkt_ptr, 
                                                      uint32* api_length);

/*===========================================================================

FUNCTION qtvdiag_create_starttask_response_packet

DESCRIPTION
 Allocate a subsystem response packet for qtvdiag_start_task command.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet, NULL if allocation fails.

SIDE EFFECTS
  None.

===========================================================================*/

qtvdiag_starttask_response_s_type *qtvdiag_create_starttask_response_packet(uint16 command_id);

/*===========================================================================

FUNCTION 
  QTVDIAG_CREATE_TESTAPI_RESPONSE_PACKET
  
DESCRIPTION
  Allocate a subsystem response packet for qtvdiag_testapi command.
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet, NULL if allocation fails.

SIDE EFFECTS
  None.

===========================================================================*/

qtvdiag_testapi_response_s_type *qtvdiag_create_testapi_response_packet(uint16 command_id);

/*===========================================================================

FUNCTION 
  qtvdiag_exec_api
  
DESCRIPTION
  Function called in the Diag context used to execute the Qtv APIs.
  
DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/

bool qtvdiag_exec_api(char *request_packet_ptr, char *response_pkt_ptr,uint32 curr_api_len);

/*===========================================================================

FUNCTION 
  getAPIheaderSize

DESCRIPTION
  Function returns the header size which includes size of timedelta,apiid and
  param length field.
  
DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 getAPIheaderSize();

/*===========================================================================

FUNCTION 
  parseHeader

DESCRIPTION
  Function fills the time delta and total api size values into
  *timeDelta_ptr and *totAPIsize_ptr respectively.
  param length field.
  
DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
bool parseHeader(char* api_hdr, uint32* timeDelta, uint32* totAPIsize);

/*===========================================================================

FUNCTION 
  qtvdiag_get_defrag_data_ptr

DESCRIPTION
  Function called in the Diag context used to get the pointer to defragmented data

DEPENDENCIES
  None.

RETURN VALUE
 Pointer to the defragmented buffer

SIDE EFFECTS
  None.

===========================================================================*/
void* qtvdiag_get_defrag_data_ptr(uint16* defrag_pkt_len);
/*===========================================================================

FUNCTION 
  is_duplicate_diag_packet

DESCRIPTION
  This will return whether the diag packet received by the command handle
  is duplicate packet or received first time
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/
bool is_duplicate_diag_packet(char* diag_pkt);
/*===========================================================================

FUNCTION FetchDnldDataSize

DESCRIPTION
     Offset value is written to the pointer provided by the caller.
  
DEPENDENCIES
  None.

RETURN VALUE
   None
SIDE EFFECTS
  None
===========================================================================*/

void FetchDnldDataSize (uint32 pUserData, uint32 * dnldDataSize, boolean * pbEndOfData, QtvPlayer::InstanceHandleT handle);
/*===========================================================================

FUNCTION HttpSimulatorStart

DESCRIPTION
   Initializes the values for Http Simulation
  
DEPENDENCIES
  None.

RETURN VALUE
   None
   
SIDE EFFECTS
  None
===========================================================================*/

void HttpSimulatorStart( uint32 downld_rate, uint32 priority);
/*=========================================================================== 

FUNCTION HttpSimulatorStop

DESCRIPTION
      Terminates Http Simulation and deletes the timer.
  
DEPENDENCIES
  None.

RETURN VALUE
  None
   
SIDE EFFECTS
  None
===========================================================================*/

void HttpSimulatorStop(QtvPlayer::InstanceHandleT handle);
/*===========================================================================

FUNCTION FetchDnldData

DESCRIPTION
     Updates the destination buffer with the data as per the updated offset
  
DEPENDENCIES
  None.

RETURN VALUE
      number of bytes read
      
SIDE EFFECTS
  None
===========================================================================*/

uint32 FetchDnldData (
      void *      dataBuf,         /* Destination buffer to copy the data */
      uint32      readSize,        /* Amount of data to be read */
      uint32      readPos,         /* Data read offset */
      uint32,                /* Media stream trackId */
      QtvPlayer::InstanceHandleT handle
    );
/*===========================================================================

FUNCTION HttpSimulatorUpdateBufferOffset

DESCRIPTION
   Updates the Offset of the buffer for every second so that Qtv can read the available data.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void HttpSimulatorUpdateBufferOffset(unsigned long bufferOffsetData);

/*===========================================================================

FUNCTION GetHandleIndex

DESCRIPTION
   Get the handle index from QtvInstances priority array.
   
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 GetHandleIndex(QtvPlayer::InstanceHandleT handle);

/*===========================================================================
FUNCTION qtvdiag_maollc

DESCRIPTION
  Function for qtvdiag malloc 
  
DEPENDENCIES
  None.

RETURN VALUE
  void * 

SIDE EFFECTS
  None
===========================================================================*/
void * qtvdiag_maollc(const VDEC_STREAM_ID streamID,
                      void  *pCbData,
                      uint32 nBytes);

/*===========================================================================
FUNCTION qtvdiag_free

DESCRIPTION
  Function for qtvdiag free
  
DEPENDENCIES
  None.

RETURN VALUE
  void 

SIDE EFFECTS
  None
===========================================================================*/
void qtvdiag_free(const VDEC_STREAM_ID streamID,void *pCbData,void* pBuf);

#ifdef FEATURE_QTVDIAG_REPLAY
/*===========================================================================

FUNCTION QtvDiagReplayStartCmdHandler

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to QTVDIAG_REPLAY_START_CMD_ID and results in the qtvdiag replay
  being started. 

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * QtvDiagReplayStartCmdHandler ( PACKED void * req_pkt_ptr, word pkt_len );

/*===========================================================================

FUNCTION QtvDiagReplayInit

DESCRIPTION
  Function called in the Diag context used to initialize replay APP

DEPENDENCIES
  None.

RETURN VALUE 
  QtvDiagReplayErrorType
SIDE EFFECTS
  None.

===========================================================================*/
QtvDiagReplayErrorType QtvDiagReplayInit ();

/*===========================================================================

FUNCTION QtvDiagReplayExecCurrentAPICallback

DESCRIPTION
  Callback function to post cmd "QTVDIAG_REPLAY_CMD_EXEC_CURRENT_API"

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagReplayExecCurrentAPICallback(unsigned long);

/*===========================================================================

FUNCTION QtvDiagReplayExecCurrentAPIHandler

DESCRIPTION
  Function is called to handle various Qtv APIs

DEPENDENCIES
  None.

RETURN VALUE
  QtvDiagReplayErrorType
SIDE EFFECTS
  None.

===========================================================================*/
QtvDiagReplayErrorType QtvDiagReplayExecCurrentAPIHandler(void);

/*===========================================================================

FUNCTION QtvDiagReplayPostCmd

DESCRIPTION
  Function to post Repaly cmd

DEPENDENCIES
  None.

RETURN VALUE
  QtvDiagReplayErrorType
SIDE EFFECTS
  None.

===========================================================================*/
QtvDiagReplayErrorType QtvDiagReplayPostCmd(qtvdiag_cmd_e_type cmd);

/*===========================================================================

FUNCTION QtvDiagReplayParseCurrentAPICmd

DESCRIPTION
  Function to parse current API cmd

DEPENDENCIES
  None.

RETURN VALUE
  QtvDiagReplayErrorType
SIDE EFFECTS
  None.

===========================================================================*/
QtvDiagReplayErrorType QtvDiagReplayParseCurrentAPICmd(void);

/*===========================================================================

FUNCTION QtvDiagReplayClose

DESCRIPTION
  Function to destroy/delete replay App

DEPENDENCIES
  None.

RETURN VALUE
  QtvDiagReplayErrorType
SIDE EFFECTS
  None.

===========================================================================*/
QtvDiagReplayErrorType QtvDiagReplayClose ();

/*===========================================================================

FUNCTION QtvDiagReplayKeepLiveCmdHandler

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to QTVDIAG_REPLAY_KEEP_LIVE_CMD_ID.

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled
SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * QtvDiagReplayKeepLiveCmdHandler ( PACKED void * req_pkt_ptr, word pkt_len );

/*===========================================================================

FUNCTION QtvDiagReplayStopCmdHandler

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to QTVDIAG_REPLAY_STOP_CMD_ID.
DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled
SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * QtvDiagReplayStopCmdHandler ( PACKED void * req_pkt_ptr, word pkt_len );

/*===========================================================================

FUNCTION QtvDiagReplayKeepLiveCallback

DESCRIPTION
  Callback function to to check whether client is alive or not

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagReplayKeepLiveCallback(unsigned long);

/*===========================================================================

FUNCTION QtvDiagReplayProcessNextAPIHandler

DESCRIPTION
  Function to to handle "QTVDIAG_REPLAY_PROCESS_NEXT_API"

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagReplayProcessNextAPIHandler ();

/*===========================================================================

FUNCTION QtvDiagReplayExecuteCurrentAPICmdHandler

DESCRIPTION
  Function to to handle "QTVDIAG_REPLAY_CMD_EXEC_CURRENT_API"

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagReplayExecuteCurrentAPICmdHandler ();

#endif /* #ifdef FEATURE_QTVDIAG_REPLAY */

#ifdef FEATURE_QTV_DEBUG_MEMORY_FAILURE
/*===========================================================================

FUNCTION QtvDiagMemoryFailureCmdHandler

DESCRIPTION
  Function to to handle "QTVDIAG_MEMORY_FAILURE_CMD_ID"

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * QtvDiagMemoryFailureCmdHandler( PACKED void * req_pkt_ptr, word pkt_len );
#endif // #ifdef FEATURE_QTV_DEBUG_MEMORY_FAILURE

#ifdef FEATURE_QTV_DEBUG_CPU_LOADING
/*===========================================================================

FUNCTION QtvDiagCPULoadingCmdHandler

DESCRIPTION
  Function to to handle "QTVDIAG_CPU_LOADING_CMD_ID"

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * QtvDiagCPULoadingCmdHandler( PACKED void * req_pkt_ptr, word pkt_len );

/*===========================================================================

FUNCTION QtvDiagCPULoadingInit

DESCRIPTION
  Function to init CPU loading parameters

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagCPULoadingInit( uint8 nPercentageOfCPULoading, uint32 nDurationOfCPULoading );

/*===========================================================================

FUNCTION QtvDiagCPULoadingTimerCallback

DESCRIPTION
  Function to post CPU loading cmd

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagCPULoadingTimerCallback( unsigned long);

/*===========================================================================

FUNCTION QtvDiagCPULoadingHandler

DESCRIPTION
  Function to handle CPU loading 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagCPULoadingHandler();

/*===========================================================================

FUNCTION QtvDiagCPULoadingStop

DESCRIPTION
  Function to Stop CPU loading 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagCPULoadingStop();

#endif // #ifdef FEATURE_QTV_DEBUG_CPU_LOADING

/*===========================================================================

FUNCTION QtvDiagPostCmd

DESCRIPTION
  Function to Post a Cmd to QtvDiag task

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagPostCmd(qtvdiag_cmd_e_type cmd);
/*===========================================================================

FUNCTION cm_mm_client_diag_reg

DESCRIPTION
  This func registers the conc mgr call back with diag

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if registration succeeds

SIDE EFFECTS
  None

===========================================================================*/
bool cm_mm_client_diag_reg(cm_mm_call_event_f_type * event_func);

#endif /*QTV_DIAG_H*/


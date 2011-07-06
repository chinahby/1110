
#ifndef _QTVPLAYER_CONFIG_H_
#define _QTVPLAYER_CONFIG_H_
/* =======================================================================
                               QtvConfig.h
DESCRIPTION
  Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/config/main/latest/inc/qtvconfig.h#27 $
$DateTime: 2010/11/09 04:48:52 $
$Change: 1509879 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_log.h"
//#ifndef _QTVPLAYERAPI_H_
//#include "QtvPlayer.h"
//#endif

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define MAX_SESSIONID_LEN 16
#define DEFAULT_MEDIAPLAYER_ROOT_PATH                         "/brew/mod/mediaplayer/media/"
#define DEFAULT_HTTP_PD_ROOT_PATH                                 "/mod/mediaplayer/media/"
#define MAX_MEDIAPLAYER_ROOT_PATH_BYTES                       256 
#define QTV_MAX_PROBE_URN_LEN                                       2048

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef struct {
uint8 length;
uint8 sID[MAX_SESSIONID_LEN];
}SessionIDType;

typedef struct {
  uint16 length;
  uint8 URL[QTV_MAX_PROBE_URN_LEN];
}ProbeURLType;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
  
/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
// Filename from which to read configuration items.
#ifndef FEATURE_WINCE
#define DEFAULT_QTVCONFIG_FILENAME "/mod/mediaplayer/qtv_config.cfg"
#define DEFAULT_QTVCONFIG_INI_FILENAME "/mod/QTVISettings/qtv_config.inix"

#else
#error code not present
#endif


/* =======================================================================
**                        Function Declarations
** ======================================================================= */
class QtvConfig
{
public:
  /*Qtv configuration support*/
  enum QtvConfigItemId
  {
    QTVCONFIG_USER_ITEM_FIRST = 0,
    QTVCONFIG_REBUFFERING_THRESHOLD = QTVCONFIG_USER_ITEM_FIRST,
    QTVCONFIG_CODECSELECT_CRITERIA = 1,  //0 if selection based on BW and 1 if on Performance
    QTVCONFIG_ISREPOS_IN_LIVESTREAM_ALLOWED = 2, // 0 to disallow and non 0 to allow
    QTVCONFIG_ISPAUSE_IN_LIVESTREAM_ALLOWED = 3, // 0 to disallow and non 0 to allow
    QTVCONFIG_RTSPDESCRIBE_TIMEOUT = 4,
    QTVCONFIG_RTSPSETUP_TIMEOUT = 5,
    QTVCONFIG_RTSPPLAY_TIMEOUT = 6,
    QTVCONFIG_RTSPPAUSE_TIMEOUT = 7,
    QTVCONFIG_RTSPTEARDOWN_TIMEOUT = 8,
    QTVCONFIG_RTSPKEEPALIVE_OPTION_TIMEOUT = 9,
    QTVCONFIG_RTSPOPTIONS_KEEPALIVE_INTERVAL = 10,
    QTVCONFIG_PAUSE_SUPERVISION_TIME = 11,
    QTVCONFIG_STREAMING_AV_SYNC_WAIT_VIDEO = 12, // 0 to disallow and non 0 to wait for Video
    // Equivalent to T_SM in TD878 spec
    QTVCONFIG_QOS_REQUEST_TIMEOUT = 13, 
    QTVCONFIG_QOS_RELEASE_TIMEOUT = 14, 
    // Timeout before reseting TCP connection during close
    QTVCONFIG_TCP_CLOSE_TIMEOUT = 15, 
    // Delay before setting NET_ABORT signal when stopping streaming. Once 
    // the signal is set, most BSD network functions should abort immediately,
    // so we cannot get any more data from the network.
    // Should be greater than the RTSP TEARDOWN timeout + TCP close timeout.
    // At some point, may also include QoS Release timeout.
    QTVCONFIG_NETWORK_ABORT_DELAY = 16, 
    // If set to a non-zero value, missing OPTIONS responses will cause QTV
    // to abort streaming as per TD878.
    QTVCONFIG_ABORT_ON_MISSING_OPTIONS_RESPONSE = 17,
    // If set to true, try to stream Windows Media using interleaved TCP.
    QTVCONFIG_USE_INTERLEAVED_TCP_FOR_WM = 18,
    // % of maximum bandwidth to use while fast starting Windows Media
    QTVCONFIG_WM_FAST_START_MAX_BW_PERCENT = 19,
    //constant for how long it will wait for data to arrive from the server
    //before declaring the connection lost, followed by session termination
    QTVCONFIG_DATA_INACTIVITY_TIME_OUT = 20,		 			/*QTVCONFIG_SERVER_TIMEOUT is the same as this*/
    // config item to enable RTSP OPTIONS KEEPALIVE METHOD
    QTVCONFIG_RTSP_OPTIONS_KEEPALIVE_ENABLED = 21,
    // config item to enable RTCP RR KEEPALIVE METHOD
    QTVCONFIG_RTCP_KEEP_ALIVE_ENABLED = 22,
    QTVCONFIG_RTCP_RR_INTERVAL = 23,
    QTVCONFIG_TEXT_RECORD_FORMAT = 24,
    //maximum url size allowed
    QTVCONFIG_MAX_URL_SIZE = 25,
    QTVCONFIG_MCAST_INTERFACE = 26,     
    QTVCONFIG_MAX_BUFFER_DURATION = 27,
    QTVCONFIG_MAX_WM_BITRATE = 28,
    QTVCONFIG_MAX_PROXY_THROTTLE_THRESHOLD = 29,
    QTVCONFIG_SESSION_ID = 30,
    QTVCONFIG_DECRYPT_EXEC_MODE = 31,
    QTVCONFIG_DECRYPT_EXEC_PLATFORM = 32,
    //Config item used to check the AMR palyload validation status
    QTVCONFIG_ENABLE_AMR_PAYLOAD_VALIDATATION = 33,
    QTVCONFIG_MAX_RTCP_BW_PERCENTAGE = 34,
    QTVCONFIG_MAX_SECOND_PDP_BANDWIDTH = 35,
    QTVCONFIG_RECONFIGURE_PORT = 36,
    QTVCONFIG_STREAMING_PREROLL = 37,
    QTVCONFIG_STREAMING_REBUFFER_PREROLL = 38,
    QTVCONFIG_STREAMING_MIN_BUFFER_DURATION = 39,
    QTVCONFIG_STREAMING_AUDIO_CMX_THRESHOLD = 40,
    QTVCONFIG_TREAT_RESUME_AS_SEEK_FOR_MCAST_OR_LIVESTREAM= 41,
    QTVCONFIG_WM_PREDSTREAM_ENABLED = 42,
    QTVCONFIG_PLAY_DURING_SKIP = 43,
    QTVCONFIG_PAUSE_IS_SUSPEND = 44,
    QTVCONFIG_STREAMING_VIDEO_MIN_PREROLL = 45,
    QTVCONFIG_DEFAULT_RTSP_MESSAGE_TIMEOUT = 46,
    QTVCONFIG_FIREWALL_PROBE_ENABLED = 47,
    QTVCONFIG_TCP_NODELAY_FOR_RTSP = 48,
    QTVCONFIG_USER_PAUSE_TIMEOUT = 49,
    QTVCONFIG_BLOCK_NON_MFLO_H264 = 50,
    QTVCONFIG_ENABLED_RATE_ADAPTATION =51,  
    QTVCONFIG_QTV_RTSP_OPTIONS_FIRST =52,
    QTVCONFIG_PCR_DISCONTINUITY_THRESHOLD_CONST = 53,
    QTVCONFIG_PCR_DISCONTINUITY_THRESHOLD_PPM = 54,
    QTVCONFIG_PCR_DISCONTINUITY_DETECTION_ENABLED = 55,
    QTVCONFIG_MAX_TS_OFFSET_OVER_PTS = 56,
    QTVCONFIG_MAX_TS_OFFSET_UNDER_PTS = 57,
    QTVCONFIG_MAX_SILENCE_DURATION = 58,
    QTVCONFIG_STREAM_SILENCE_INSERTION_ENABLED =59 ,
    QTVCONFIG_STREAM_RECORD_USE_PREALLOC_BUFFER = 60,
    QTVCONFIG_WMPL_USER_INITIATED_SETUP_DELAY = 61,
    QTVCONFIG_WMPL_NATURAL_TRANSITION_SETUP_DELAY = 62,
    QTVCONFIG_GENERIC_TEXT_DEFAULT_SAMPLE_DURATION = 63,
    QTVCONFIG_IGNORE_TIAS_BANDWITH = 64, 
    QTVCONFIG_QOS_EVDO_DELTA_ABOVE_BANDWIDTH = 65,
    QTVCONFIG_QOS_EVDO_FWD_LINK_PROFILE_ID = 66,
    QTVCONFIG_QOS_EVDO_REV_LINK_ENABLE = 67,
    QTVCONFIG_QOS_EVDO_REV_LINK_PROFILE_ID = 68,
    QTVCONFIG_MEDIAPLAYER_ROOT_PATH = 69,
    QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY_ENABLE = 70,
    QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY = 71,
    QTVCONFIG_AUDIO_DISCONTINUITY_THRESHOLD = 72,
    QTVCONFIG_ENABLE_QFRE = 73,
    QTVCONFIG_ENABLE_SEI_FOR_SINGLE_NAL_DROP = 74,
    QTVCONFIG_MAX_FRAME_DROPS_TO_IFRAME = 75,
    QTVCONFIG_PCR_SYNC_USE_SW_TIMESTAMPS = 76,
    QTVCONFIG_ENABLE_BRAZIL_SPECIFIC_H264_VUI_PARAM_TYPE = 77,
    QTVCONFIG_WRITE_STATS_TO_FILE = 78,
    QTVCONFIG_BSD_IFACE_SELECTION = 79,
    QTVCONFIG_ENABLE_WM_FAST_RECONNECT = 80,
    QTVCONFIG_WM_FAST_RECONNECT_MAX_ATTEMPTS = 81,
    QTVCONFIG_PROBE_URN = 82,
    QTVCONFIG_FCS_KEEPALIVE_SESSION_TIMEOUT = 83,
    QTVCONFIG_RTSP_LINK_CHAR_ENABLE = 84,
    QTVCONFIG_STREAM_RECORD_DEC_BUF_DURATION = 85,
    QTVCONFIG_RTP_MIN_BUFF_TIME= 86,
    QTVCONFIG_RTP_MAX_BUFF_TIME= 87,
    QTVCONFIG_ENABLEVIDEO_AT_KEY_FRAME = 88,
    QTVCONFIG_USE_INTERLEAVED_TCP_FOR_3GP =89,
    QTVCONFIG_VIDEO_ADUIO_VOICE_MIXING = 90,
    QTVCONFIG_ACCEPT_SERVER_ANNOUNCED_CLIENT_PORTS = 91,
    QTVCONFIG_DISABLE_PIPELINING_FOR_FIREWALL_PROBE_ENABLED = 92,
    QTVCONFIG_ENABLE_PIPELINING_FOR_SDP_PLAYBACK = 93,
    QTVCONFIG_ENABLE_PIPELINING_FOR_STREAMING = 94,
    QTVCONFIG_HTTP_FILE_SAVE_TO_EFS_PATH = 95,
    QTVCONFIG_AUTO_FALLBACK_OVER_TCP = 96,
    QTVCONFIG_USER_ITEM_LAST = QTVCONFIG_AUTO_FALLBACK_OVER_TCP,

    // Configuration items after this one should not be set by OEMs.
    // They are for debugging purposes only.
    QTVCONFIG_DEBUG_ITEM_FIRST = QTVCONFIG_USER_ITEM_LAST + 1,
    // Pattern of packets to drop, e.g. "[ [3-29:5;7-26], [366-;84] ]"
    QTVCONFIG_DROP_PACKET_PATTERN = QTVCONFIG_DEBUG_ITEM_FIRST,
    QTVCONFIG_STREAMING_UDP_DROPRATE = QTVCONFIG_DEBUG_ITEM_FIRST + 1,
    QTVCONFIG_SIMULATE_OLD_WM_SERVER = QTVCONFIG_DEBUG_ITEM_FIRST + 2,
    QTVCONFIG_STA_IFACE_ENABLED = QTVCONFIG_DEBUG_ITEM_FIRST + 3,
    QTVCONFIG_PCR_DRIFT_CORRECTION_ENABLED = QTVCONFIG_DEBUG_ITEM_FIRST + 4,
    QTVCONFIG_SR_AVG_DRIFT_FILTER_ZERO_PADDING = QTVCONFIG_DEBUG_ITEM_FIRST + 5,
    QTVCONFIG_RD_AVG_DRIFT_FILTER_ZERO_PADDING = QTVCONFIG_DEBUG_ITEM_FIRST + 6,
    QTVCONFIG_SR_DRIFT_FILTER_UPDATE_DELAY = QTVCONFIG_DEBUG_ITEM_FIRST + 7,
    QTVCONFIG_RD_DRIFT_FILTER_UPDATE_DELAY = QTVCONFIG_DEBUG_ITEM_FIRST + 8,
    QTVCONFIG_DRIFT_CORRECTION_DELAY = QTVCONFIG_DEBUG_ITEM_FIRST + 9,
    QTVCONFIG_SR_DRIFT_FILTER_TYPE = QTVCONFIG_DEBUG_ITEM_FIRST + 10,
    QTVCONFIG_RD_DRIFT_FILTER_TYPE = QTVCONFIG_DEBUG_ITEM_FIRST + 11,
    QTVCONFIG_SR_DRIFT_FILTER_UPDATE_INTERVAL = QTVCONFIG_DEBUG_ITEM_FIRST + 12,
    QTVCONFIG_RD_DRIFT_FILTER_UPDATE_INTERVAL = QTVCONFIG_DEBUG_ITEM_FIRST + 13,
    QTVCONFIG_SR_DRIFT_FILTER_TIME_CONSTANT = QTVCONFIG_DEBUG_ITEM_FIRST + 14,
    QTVCONFIG_RD_DRIFT_FILTER_TIME_CONSTANT = QTVCONFIG_DEBUG_ITEM_FIRST + 15,
    QTVCONFIG_SR_DRIFT_FILTER_MAX_ITEMS_IN_MEMORY = QTVCONFIG_DEBUG_ITEM_FIRST + 16,
    QTVCONFIG_RD_DRIFT_FILTER_MAX_ITEMS_IN_MEMORY = QTVCONFIG_DEBUG_ITEM_FIRST + 17,
    QTVCONFIG_SR_DRIFT_FILTER_MEASUREMENT_INTERVAL = QTVCONFIG_DEBUG_ITEM_FIRST + 18,
    QTVCONFIG_RD_DRIFT_FILTER_MEASUREMENT_INTERVAL = QTVCONFIG_DEBUG_ITEM_FIRST + 19,
    QTVCONFIG_DEBUG_ITEM_LAST = QTVCONFIG_RD_DRIFT_FILTER_MEASUREMENT_INTERVAL,
    QTVCONFIG_ITEM_COUNT
  };

  // constructor 
  QtvConfig(const char *configFile = DEFAULT_QTVCONFIG_FILENAME);  	

  //Destructor
  ~QtvConfig();
  
  // config methods
  bool SetQTVConfigItem(QtvConfigItemId item_id, int32 value);
  int32 GetQTVConfigItem(QtvConfigItemId item_id) const;
  void Reset(const char *configFile = DEFAULT_QTVCONFIG_FILENAME);
  void PrintValues() const;   

private:
 
  void ResetHardCodedValues();
  int  ReadItemsFromNV();
  int  ReadItemsFromConfigFile(const char *configFile);
  
  // elements
  int32 config_items[QTVCONFIG_ITEM_COUNT];  
  char mediaplayer_root_path[MAX_MEDIAPLAYER_ROOT_PATH_BYTES];
  char http_pd_root_path[MAX_MEDIAPLAYER_ROOT_PATH_BYTES];
  bool SetMediaPlayerRootPath(char* value);
#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
  SessionIDType sessionID;
  bool SetSessionID(SessionIDType* value);
#endif

  ProbeURLType probeURL;
  bool SetProbeURL(ProbeURLType* value);
  bool SetHTTPPDRootPath(char *value);
};

#endif /* _QTV_CONFIG_H_ */





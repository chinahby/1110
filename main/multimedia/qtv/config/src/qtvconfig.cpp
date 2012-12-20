
/* =======================================================================
                               QtvConfig.cpp
DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright 2004 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/config/main/latest/src/qtvconfig.cpp#32 $
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
#include "qtv_msg.h"
#include "qtvsystem.h"
#include "qtvstreambase.h"
#include "ConfigFileReader.h"
#include "QtvConfig.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define MAX_SILENCE_DURATION 1000  /* 1000 Msec*/
#define MAX_RTSP_TIMEOUT_VALUE 600000
#define DEFAULT_RTSP_TEARDOWN_TIMEOUT 2000

#ifdef FEATURE_QTV_QOS_SELECTION_EVDO
#define DEFAULT_MAX_SECOND_PDP_BANDWIDTH 3000000
#else
#define DEFAULT_MAX_SECOND_PDP_BANDWIDTH 256000
#endif /* FEATURE_QTV_QOS_SELECTION_EVDO */
#define DEFAULT_MAX_RTCP_BW_PERCENTAGE 250

// Interval between last keepalive OPTIONS response and next request.
#define DEFAULT_RTSP_OPTIONS_KEEPALIVE_INTERVAL 50000

#define DEFAULT_TCP_CLOSE_TIMEOUT 1000

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif

/* Should be greater than DEFAULT_RTSP_TEARDOWN_TIMEOUT + 
 * DEFAULT_TCP_CLOSE_TIMEOUT.
 */ 
#define DEFAULT_NETWORK_ABORT_DELAY (DEFAULT_RTSP_TEARDOWN_TIMEOUT + DEFAULT_TCP_CLOSE_TIMEOUT + 400)

#ifdef FEATURE_QTV_WINDOWS_MEDIA
#define DEFAULT_MAX_WM_BITRATE 384000

#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
#define DEFAULT_WMPL_USER_INITIATED_SETUP_DELAY 200
#define DEFAULT_WMPL_NATURAL_TRANSITION_SETUP_DELAY 11000;
#endif /* FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST */

#endif /* FEATURE_QTV_WINDOWS_MEDIA */

#ifdef FEATURE_QTV_ENCRYPTED_STREAMS 
#define MAX_PROXY_RTP_INFO_ARRAY 100
#endif /* FEATURE_QTV_ENCRYPTED_STREAMS */

#define DEFAULT_RTCP_RR_INTERVAL  -1 
#define DEFAULT_STREAMING_VIDEO_MIN_PREROLL 1000

#define DEFAULT_WM_FAST_RECONNECT_ATTEMPS  2

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* -----------------------------------------------------------------------
** Static/private  Data Declarations
** ----------------------------------------------------------------------- */
//Raghu
//QtvConfig *QtvConfig::instance = NULL;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/* ======================================================================
FUNCTION
  QtvConfig::GetQTVConfigItem

DESCRIPTION
  This provides the QTV configuration parameter

DEPENDENCIES
  None

RETURN VALUE
  Value of Config Item

SIDE EFFECTS
========================================================================== */

int32 QtvConfig::GetQTVConfigItem(QtvConfigItemId itemID) const
{
   if ( (int)itemID >= 0 && itemID < QtvConfig::QTVCONFIG_ITEM_COUNT )  /*lint !e568*/
   {
     return config_items[itemID];
   }
   else
   {
     return -1;
   }
} /*lint !e1762*/



/* ======================================================================
FUNCTION
  QtvConfig::SetQTVConfigItem

DESCRIPTION
  Set the qtv config

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

========================================================================== */

bool QtvConfig::SetQTVConfigItem(QtvConfigItemId item_id, int32 value)
{
  switch (item_id)
  {
    case QTVCONFIG_REBUFFERING_THRESHOLD:
    case QTVCONFIG_ISREPOS_IN_LIVESTREAM_ALLOWED: 
    case QTVCONFIG_ISPAUSE_IN_LIVESTREAM_ALLOWED:
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
    case QTVCONFIG_TCP_CLOSE_TIMEOUT:
    case QTVCONFIG_NETWORK_ABORT_DELAY:
    case QTVCONFIG_ABORT_ON_MISSING_OPTIONS_RESPONSE:
#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case QTVCONFIG_USE_INTERLEAVED_TCP_FOR_WM:
    case QTVCONFIG_WM_FAST_START_MAX_BW_PERCENT:
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
    case QTVCONFIG_RTSP_OPTIONS_KEEPALIVE_ENABLED :
#ifdef FEATURE_QTV_RTCP_KEEP_ALIVE  
    case QTVCONFIG_RTCP_KEEP_ALIVE_ENABLED:
#endif /* FEATURE_QTV_RTCP_KEEP_ALIVE   */
    case QTVCONFIG_RTCP_RR_INTERVAL:
    case QTVCONFIG_MAX_URL_SIZE:
    case QTVCONFIG_STREAMING_UDP_DROPRATE:
    case QTVCONFIG_MAX_BUFFER_DURATION:
#ifdef FEATURE_QTV_MCAST
    case QTVCONFIG_MCAST_INTERFACE:
#endif /* FEATURE_QTV_MCAST */
    case QTVCONFIG_BSD_IFACE_SELECTION:
#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case QTVCONFIG_MAX_WM_BITRATE:
    case QTVCONFIG_WM_PREDSTREAM_ENABLED:
    case QTVCONFIG_SIMULATE_OLD_WM_SERVER:
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
    case QTVCONFIG_ENABLE_AMR_PAYLOAD_VALIDATATION:
#ifdef FEATURE_QTV_PLAYLIST
    case QTVCONFIG_PLAY_DURING_SKIP:
#endif /* FEATURE_QTV_PLAYLIST */
#ifdef FEATURE_ENABLE_PAUSE_IS_SUSPEND
    case QTVCONFIG_PAUSE_IS_SUSPEND:
    case QTVCONFIG_IGNORE_TIAS_BANDWITH:
#endif /* FEATURE_ENABLE_PAUSE_IS_SUSPEND */
    case QTVCONFIG_MAX_SILENCE_DURATION: 
    case QTVCONFIG_STREAM_SILENCE_INSERTION_ENABLED:
    case QTVCONFIG_ENABLED_RATE_ADAPTATION:
    case QTVCONFIG_BLOCK_NON_MFLO_H264:
    case QTVCONFIG_FIREWALL_PROBE_ENABLED:
    case QTVCONFIG_STA_IFACE_ENABLED: 
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
    case QTVCONFIG_GENERIC_TEXT_DEFAULT_SAMPLE_DURATION:
    case QTVCONFIG_QTV_RTSP_OPTIONS_FIRST: 
#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
    case QTVCONFIG_MAX_FRAME_DROPS_TO_IFRAME: 
    case QTVCONFIG_WRITE_STATS_TO_FILE:
    case QTVCONFIG_RTSP_LINK_CHAR_ENABLE:
    case QTVCONFIG_AUTO_FALLBACK_OVER_TCP:
    case QTVCONFIG_ENABLE_WM_FAST_RECONNECT:
    case QTVCONFIG_WM_FAST_RECONNECT_MAX_ATTEMPTS:
    case QTVCONFIG_RTP_MIN_BUFF_TIME:
    case QTVCONFIG_RTP_MAX_BUFF_TIME:
    case QTVCONFIG_USE_INTERLEAVED_TCP_FOR_3GP:    
    case QTVCONFIG_VIDEO_ADUIO_VOICE_MIXING:
    case QTVCONFIG_ACCEPT_SERVER_ANNOUNCED_CLIENT_PORTS:    
    case QTVCONFIG_DISABLE_PIPELINING_FOR_FIREWALL_PROBE_ENABLED:
    case QTVCONFIG_ENABLE_PIPELINING_FOR_SDP_PLAYBACK:
    case QTVCONFIG_ENABLE_PIPELINING_FOR_STREAMING:
      if (value < 0)
      {
         return false;
      }
      break;

    case QTVCONFIG_RTSPDESCRIBE_TIMEOUT:
    case QTVCONFIG_RTSPSETUP_TIMEOUT:
    case QTVCONFIG_RTSPPLAY_TIMEOUT:
    case QTVCONFIG_RTSPPAUSE_TIMEOUT:
    case QTVCONFIG_RTSPTEARDOWN_TIMEOUT:
    case QTVCONFIG_PAUSE_SUPERVISION_TIME:
    case QTVCONFIG_RTSPOPTIONS_KEEPALIVE_INTERVAL:
    case QTVCONFIG_RTSPKEEPALIVE_OPTION_TIMEOUT:         
       if (value < 0 || value > MAX_RTSP_TIMEOUT_VALUE)     /* seconds */
       {
         return false;
       }
       break;

    case QTVCONFIG_CODECSELECT_CRITERIA:
       if (value != BITRATE_BASED_STREAMING_CODEC_CRITERIA &&      /*lint !e641*/
           value != PERFORMANCE_BASED_STREAMING_CODEC_CRITERIA) /*lint !e641*/
       {
         return false;
       }
       break;

#ifdef FEATURE_QTV_TEXT_RECORD
    case QTVCONFIG_TEXT_RECORD_FORMAT:
       if (value != SKT_TEXT_RECORD_FORMAT && 
           value != TX3G_TEXT_RECORD_FORMAT)
       {
         return false;
       }
       break;
#endif

#ifdef FEATURE_FRE
    case QTVCONFIG_ENABLE_QFRE:
       if (value < 0 || value > 1)
       {
         return false;
       }
       break;
#endif

    case QTVCONFIG_ENABLEVIDEO_AT_KEY_FRAME:
       if (value < 0 || value > 1)
       {
         return false;
       }
       break;

    case QTVCONFIG_DROP_PACKET_PATTERN:
    // Can be NULL or a pointer value.
    case QTVCONFIG_MAX_RTCP_BW_PERCENTAGE:
    case  QTVCONFIG_MAX_SECOND_PDP_BANDWIDTH:
    case QTVCONFIG_RECONFIGURE_PORT:
    case QTVCONFIG_DEFAULT_RTSP_MESSAGE_TIMEOUT:
    case QTVCONFIG_STREAMING_PREROLL:
    case QTVCONFIG_STREAMING_REBUFFER_PREROLL:
    case QTVCONFIG_STREAMING_VIDEO_MIN_PREROLL:
    case QTVCONFIG_STREAMING_MIN_BUFFER_DURATION:
#ifdef FEATURE_QTV_STREAM_RECORD
    case QTVCONFIG_STREAM_RECORD_DEC_BUF_DURATION:
#endif
    case QTVCONFIG_STREAMING_AUDIO_CMX_THRESHOLD:
    case QTVCONFIG_TREAT_RESUME_AS_SEEK_FOR_MCAST_OR_LIVESTREAM :
    case QTVCONFIG_TCP_NODELAY_FOR_RTSP:
#ifdef FEATURE_QTV_ENCRYPTED_STREAMS 
    case QTVCONFIG_MAX_PROXY_THROTTLE_THRESHOLD:
    case QTVCONFIG_DECRYPT_EXEC_MODE:
    case QTVCONFIG_DECRYPT_EXEC_PLATFORM:
#endif /*FEATURE_QTV_ENCRYPTED_STREAMS*/
    case QTVCONFIG_USER_PAUSE_TIMEOUT:
#ifdef FEATURE_QTV_QOS_SELECTION_EVDO
    case QTVCONFIG_QOS_EVDO_DELTA_ABOVE_BANDWIDTH:
    case QTVCONFIG_QOS_EVDO_FWD_LINK_PROFILE_ID:
    case QTVCONFIG_QOS_EVDO_REV_LINK_ENABLE:
    case QTVCONFIG_QOS_EVDO_REV_LINK_PROFILE_ID:
#endif /* FEATURE_QTV_QOS_SELECTION_EVDO */
    case QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY_ENABLE:
    case QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY:
    case QTVCONFIG_ENABLE_SEI_FOR_SINGLE_NAL_DROP:
    case QTVCONFIG_ENABLE_BRAZIL_SPECIFIC_H264_VUI_PARAM_TYPE:
#ifdef FEATURE_QTV_FCS
#error code not present
#endif 
      break;

#ifdef FEATURE_QTV_ENCRYPTED_STREAMS 
    case QTVCONFIG_SESSION_ID:
      return SetSessionID((SessionIDType*)value);
#endif /*FEATURE_QTV_ENCRYPTED_STREAMS*/
#ifdef FEATURE_QTV_STREAM_RECORD
    case QTVCONFIG_STREAM_RECORD_USE_PREALLOC_BUFFER:
      break;
#endif
    case QTVCONFIG_MEDIAPLAYER_ROOT_PATH:
      return SetMediaPlayerRootPath((char *)value);
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
    case QTVCONFIG_HTTP_FILE_SAVE_TO_EFS_PATH:
      return SetHTTPPDRootPath((char *)value);
    case QTVCONFIG_DATA_INACTIVITY_TIME_OUT: 
    if (value <= 0)
    {
      return false;
    } 
    break;
    default:
    return false;
  }

  config_items[item_id] = value;
  return true;
}


 /* ======================================================================
FUNCTION
  QtvStreamerConfig::QtvStreamerConfig

DESCRIPTION
   Class constructor.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
========================================================================== */

QtvConfig::QtvConfig(const char *configFile)
{
   memset(http_pd_root_path,0,sizeof(http_pd_root_path));
   Reset(configFile);
   PrintValues();
}
  
/* ======================================================================
FUNCTION
  QtvConfig::Reset

DESCRIPTION
   Resets all values to their defaults. Steps are:
   1) Use hard-coded defaults.
   2) Read NV items (not implemented yet)
   3) Read from configuration file
   Each step overrides the settings of the previous step.
   
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
========================================================================== */

void QtvConfig::Reset(const char *configFile)
{
   ResetHardCodedValues();
   ReadItemsFromNV(); /*lint !e534*/
   ReadItemsFromConfigFile(configFile); /*lint !e534*/
   ReadItemsFromConfigFile(DEFAULT_QTVCONFIG_INI_FILENAME); /*read data from qtv_config.ini*/
}


/* ======================================================================
FUNCTION
  QtvConfig::PrintValues

DESCRIPTION
  Print all mappings using F3 messages.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
========================================================================== */
void QtvConfig::PrintValues() const
{
   for (int i = 0; i < (int) QTVCONFIG_ITEM_COUNT; i++)
   {
      int32 config_value = 0;
      config_value = GetQTVConfigItem((QtvConfigItemId) i);    	 
      QTV_MSG_SPRINTF_2( QTVDIAG_CONFIGURATION, "SCPV: value for %s is %d",           
         QtvConfigFileReader::upperCaseNameForConfigItem[i], config_value);
   }
}


/* ======================================================================
FUNCTION
  QtvConfig::ReadHardCodedValues

DESCRIPTION
   Overrides configuration items with hard-coded defaults.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
========================================================================== */
void QtvConfig::ResetHardCodedValues()
{  
    for (int i = 0; i < (int) QTVCONFIG_ITEM_COUNT; i++)
    {
       config_items[i] =  -1;
    }

    config_items[QTVCONFIG_ISPAUSE_IN_LIVESTREAM_ALLOWED] = -1;
    config_items[QTVCONFIG_ISREPOS_IN_LIVESTREAM_ALLOWED] = 0;
    config_items[QTVCONFIG_CODECSELECT_CRITERIA] =
         DEFAULT_CRITERIA; /*lint !e641*/
    config_items[QTVCONFIG_RTSPDESCRIBE_TIMEOUT] =  DEFAULT_TIMEOUT;
    config_items[QTVCONFIG_RTSPSETUP_TIMEOUT] =  DEFAULT_TIMEOUT; 
    config_items[QTVCONFIG_RTSPPLAY_TIMEOUT] =  DEFAULT_TIMEOUT;
    config_items[QTVCONFIG_RTSPPAUSE_TIMEOUT] =  DEFAULT_TIMEOUT;
    config_items[QTVCONFIG_RTSPTEARDOWN_TIMEOUT] = 
         DEFAULT_RTSP_TEARDOWN_TIMEOUT;
    // Setting it to a large value so that the change does not affect all OEM's. OEM  
    // can configure using the API in QTVplayer.h
    config_items[QTVCONFIG_PAUSE_SUPERVISION_TIME] =  1200000000; 
    config_items[QTVCONFIG_RTSPKEEPALIVE_OPTION_TIMEOUT] = 
       DEFAULT_OPTIONS_TIMEOUT;
    config_items[QTVCONFIG_RTSPOPTIONS_KEEPALIVE_INTERVAL] = 
       DEFAULT_RTSP_OPTIONS_KEEPALIVE_INTERVAL;

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */

    config_items[QTVCONFIG_TCP_CLOSE_TIMEOUT] = 
       DEFAULT_TCP_CLOSE_TIMEOUT;
    config_items[QTVCONFIG_NETWORK_ABORT_DELAY] =
       DEFAULT_NETWORK_ABORT_DELAY;
    config_items[QTVCONFIG_ABORT_ON_MISSING_OPTIONS_RESPONSE] =  0;
	config_items[QTVCONFIG_WRITE_STATS_TO_FILE] = 0;

#ifdef FEATURE_QTV_WINDOWS_MEDIA
    config_items[QTVCONFIG_USE_INTERLEAVED_TCP_FOR_WM] =  0;
    config_items[QTVCONFIG_WM_FAST_START_MAX_BW_PERCENT] = 
       DEFAULT_FAST_START_MAX_BANDWIDTH_PERCENT;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

    config_items[QTVCONFIG_DATA_INACTIVITY_TIME_OUT] =  
       DEFAULT_DATA_INACTIVITY_TIMEOUT;
#ifdef FEATURE_QTV_RTCP_KEEP_ALIVE  
    config_items[QTVCONFIG_RTCP_KEEP_ALIVE_ENABLED] =  1;
#endif /* FEATURE_QTV_RTCP_KEEP_ALIVE   */
#ifdef FEATURE_QTV_RTSP_OPTIONS_KEEPALIVE
    config_items[QTVCONFIG_RTSP_OPTIONS_KEEPALIVE_ENABLED] =  1;
#else
    config_items[QTVCONFIG_RTSP_OPTIONS_KEEPALIVE_ENABLED] =  0;
#endif /* FEATURE_QTV_RTSP_OPTIONS_KEEPALIVE */
    config_items[QTVCONFIG_RTCP_RR_INTERVAL] = 
       DEFAULT_RTCP_RR_INTERVAL;
#ifdef FEATURE_QTV_MCAST
    config_items[QTVCONFIG_MCAST_INTERFACE] = 
       DEFAULT_BCAST_IFACE;
#endif /*FEATURE_QTV_MCAST*/
    config_items[QTVCONFIG_BSD_IFACE_SELECTION] = -1;
#ifdef FEATURE_QTV_TEXT_RECORD
    config_items[QTVCONFIG_TEXT_RECORD_FORMAT] = 
       DEFAULT_TEXT_RECORD_FORMAT;
#endif

    config_items[QTVCONFIG_DROP_PACKET_PATTERN] = NULL;

    config_items[QTVCONFIG_MAX_URL_SIZE] = 
       QTV_MAX_URN_BYTES;
    config_items[QTVCONFIG_STREAMING_UDP_DROPRATE] = 0;
    config_items[QTVCONFIG_MAX_BUFFER_DURATION] = -1;
#ifdef FEATURE_QTV_WINDOWS_MEDIA
    config_items[QTVCONFIG_MAX_WM_BITRATE] = DEFAULT_MAX_WM_BITRATE;
    config_items[QTVCONFIG_WM_PREDSTREAM_ENABLED] = 1;
    config_items[QTVCONFIG_SIMULATE_OLD_WM_SERVER] = 0;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */	
    config_items[QTVCONFIG_MAX_SECOND_PDP_BANDWIDTH] = DEFAULT_MAX_SECOND_PDP_BANDWIDTH;
    config_items[QTVCONFIG_MAX_RTCP_BW_PERCENTAGE] = DEFAULT_MAX_RTCP_BW_PERCENTAGE;
#ifdef FEATURE_QTV_ENCRYPTED_STREAMS 
    config_items[QTVCONFIG_MAX_PROXY_THROTTLE_THRESHOLD]= MAX_PROXY_RTP_INFO_ARRAY;
    sessionID.length = 0;
    config_items[QTVCONFIG_SESSION_ID] = (int32)&sessionID;
    config_items[QTVCONFIG_DECRYPT_EXEC_MODE]= (int32)SECAPI_SYNC_EXEC;
    config_items[QTVCONFIG_DECRYPT_EXEC_PLATFORM]= (int32)SECAPI_EXEC_PLATFORM_ARM;
#endif
    config_items[QTVCONFIG_RECONFIGURE_PORT] = 0;
    config_items[QTVCONFIG_STREAMING_PREROLL] = -1;
    config_items[QTVCONFIG_STREAMING_REBUFFER_PREROLL] = -1;
    config_items[QTVCONFIG_STREAMING_VIDEO_MIN_PREROLL] = 
                                         DEFAULT_STREAMING_VIDEO_MIN_PREROLL;
#ifdef FEATURE_QTV_STREAM_RECORD
    config_items[QTVCONFIG_STREAM_RECORD_DEC_BUF_DURATION] = -1;
#endif
    config_items[QTVCONFIG_DEFAULT_RTSP_MESSAGE_TIMEOUT] = DEFAULT_TIMEOUT;
    config_items[QTVCONFIG_STREAMING_MIN_BUFFER_DURATION] = -1;
    config_items[QTVCONFIG_STREAMING_AUDIO_CMX_THRESHOLD] = 0;  
    config_items[QTVCONFIG_TREAT_RESUME_AS_SEEK_FOR_MCAST_OR_LIVESTREAM] = 1;
    config_items[QTVCONFIG_MAX_SILENCE_DURATION] = MAX_SILENCE_DURATION ;
    config_items[QTVCONFIG_STREAM_SILENCE_INSERTION_ENABLED] = true;
    config_items[QTVCONFIG_IGNORE_TIAS_BANDWITH] = false;
    config_items[QTVCONFIG_ENABLED_RATE_ADAPTATION] =true; 
    config_items[QTVCONFIG_QTV_RTSP_OPTIONS_FIRST] = true;
#ifdef FEATURE_QTV_PLAYLIST
    config_items[QTVCONFIG_PLAY_DURING_SKIP] = 0;
#endif /* FEATURE_QTV_PLAYLIST */

#ifdef FEATURE_ENABLE_PAUSE_IS_SUSPEND
    config_items[QTVCONFIG_PAUSE_IS_SUSPEND] = 1;
#else 
    config_items[QTVCONFIG_PAUSE_IS_SUSPEND] = 0;
#endif /* FEATURE_ENABLE_PAUSE_IS_SUSPEND */
    config_items[QTVCONFIG_FIREWALL_PROBE_ENABLED] = 1;
    config_items[QTVCONFIG_TCP_NODELAY_FOR_RTSP] = 1;
    config_items[QTVCONFIG_USER_PAUSE_TIMEOUT] = -1;

    config_items[QTVCONFIG_BLOCK_NON_MFLO_H264] = 0;

    config_items[QTVCONFIG_STA_IFACE_ENABLED] = 0;

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

#ifdef FEATURE_QTV_STREAM_RECORD
  config_items[QTVCONFIG_STREAM_RECORD_USE_PREALLOC_BUFFER] = true;
#endif /* FEATURE_QTV_STREAM_RECORD */

#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
    config_items[QTVCONFIG_WMPL_USER_INITIATED_SETUP_DELAY] = 
      DEFAULT_WMPL_USER_INITIATED_SETUP_DELAY;

    config_items[QTVCONFIG_WMPL_NATURAL_TRANSITION_SETUP_DELAY] = 
      DEFAULT_WMPL_NATURAL_TRANSITION_SETUP_DELAY;
#endif /* FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST */

    config_items[QTVCONFIG_GENERIC_TEXT_DEFAULT_SAMPLE_DURATION] = 1000;
    std_strlcpy(mediaplayer_root_path,DEFAULT_MEDIAPLAYER_ROOT_PATH, sizeof(mediaplayer_root_path));
    config_items[QTVCONFIG_MEDIAPLAYER_ROOT_PATH] = (int32)&mediaplayer_root_path;
    config_items[QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY_ENABLE] = false;
    config_items[QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY] = 50;
    config_items[QTVCONFIG_ENABLE_SEI_FOR_SINGLE_NAL_DROP] = true;
    config_items[QTVCONFIG_ENABLE_BRAZIL_SPECIFIC_H264_VUI_PARAM_TYPE] = false;
#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

#ifdef FEATURE_FRE
    config_items[QTVCONFIG_ENABLE_QFRE] = true;
#else
    config_items[QTVCONFIG_ENABLE_QFRE] = false;
#endif

    config_items[QTVCONFIG_ENABLEVIDEO_AT_KEY_FRAME] = false;

    config_items[QTVCONFIG_MAX_FRAME_DROPS_TO_IFRAME] = 4;
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
    config_items[QTVCONFIG_RTSP_LINK_CHAR_ENABLE] = false;
    config_items[QTVCONFIG_ENABLE_WM_FAST_RECONNECT] = false;
    config_items[QTVCONFIG_WM_FAST_RECONNECT_MAX_ATTEMPTS] = DEFAULT_WM_FAST_RECONNECT_ATTEMPS; 
    config_items[QTVCONFIG_USE_INTERLEAVED_TCP_FOR_3GP] = false;
    config_items[QTVCONFIG_RTP_MIN_BUFF_TIME] = DEFAULT_RTP_MIN_BUFFER_DURATION;
    config_items[QTVCONFIG_RTP_MAX_BUFF_TIME] = DEFAULT_RTP_MAX_BUFFER_DURATION;
    config_items[QTVCONFIG_VIDEO_ADUIO_VOICE_MIXING] = 0;   
    config_items[QTVCONFIG_ACCEPT_SERVER_ANNOUNCED_CLIENT_PORTS] = false;
    config_items[QTVCONFIG_DISABLE_PIPELINING_FOR_FIREWALL_PROBE_ENABLED] = false;
    config_items[QTVCONFIG_ENABLE_PIPELINING_FOR_STREAMING] = false;
    config_items[QTVCONFIG_ENABLE_PIPELINING_FOR_SDP_PLAYBACK] = -1;
    config_items[QTVCONFIG_HTTP_FILE_SAVE_TO_EFS_PATH] = (int32)&http_pd_root_path;
    config_items[QTVCONFIG_AUTO_FALLBACK_OVER_TCP] = false; 
}

/* ======================================================================
FUNCTION
  QtvStreamerConfig::ReadItemsFromNV

DESCRIPTION
  Overrides configuration items by reading NV items. Currently not 
  implemented.
   
DEPENDENCIES
  None

RETURN VALUE
  Return the number of items read from NV.

SIDE EFFECTS
========================================================================== */
int QtvConfig::ReadItemsFromNV()
{
   return 0;
} /*lint !e1762*/

/* ======================================================================
FUNCTION
  QtvConfig::ReadItemsFromConfigFile

DESCRIPTION
   Overrides configuration items by reading items from a configuration file.
   
DEPENDENCIES
  None

RETURN VALUE
  The number of items read from the configuration file.

SIDE EFFECTS
  None
========================================================================== */
int QtvConfig::ReadItemsFromConfigFile(const char *configFile)
{
   QtvConfigFileReader reader;

   if (NULL == configFile)
   {
      QTV_MSG_PRIO( QTVDIAG_CONFIGURATION, QTVDIAG_PRIO_ERROR, 
                    "configFile is NULL!");
      return 0;
   }

   if (!reader.open(configFile))
   {
      QTV_MSG_PRIO1( QTVDIAG_CONFIGURATION, QTVDIAG_PRIO_ERROR, 
        "Couldn't open streaming config file %s", configFile);
      return 0;
   }

   return reader.readItems(this);   
}

/* ======================================================================
FUNCTION
  QtvStreamerConfig::~QtvStreamerConfig

DESCRIPTION
   Class destructor
   Because this is a singleton that is never freed, the destructor should
 never be invoked.  It's here for completeness only.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
========================================================================== */
QtvConfig::~QtvConfig()
{
   QTV_MSG_PRIO( QTVDIAG_CONFIGURATION, QTVDIAG_PRIO_LOW,
     "QtvConfig::~QtvConfig" );
}

#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
/* ======================================================================
FUNCTION
  QtvStreamerConfig::SetSessionID

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
========================================================================== */
bool QtvConfig::SetSessionID(SessionIDType* value)
{
    //First element of the value is length
    //Second Element of the value is the address of the byte array
    bool result = true;

    if(!value || value->length == 0)
    {
     return false;
    }

    if (value->length <= MAX_SESSIONID_LEN)
    {
      sessionID.length = value->length;
      memcpy(sessionID.sID, value->sID, value->length);
    }
    else
    {
      result = false;
    }

    return result;
}
#endif


/* ======================================================================
FUNCTION
  QtvStreamerConfig::SetMediaPlayerRootPath

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
========================================================================== */
bool QtvConfig::SetMediaPlayerRootPath(char* value)
{ 
   if(NULL == value)
   {
     value = (char *)DEFAULT_MEDIAPLAYER_ROOT_PATH;
   }
   std_strlcpy(mediaplayer_root_path,(char*)value, sizeof(mediaplayer_root_path));

   return true;
}


bool QtvConfig::SetProbeURL(ProbeURLType* value)
{
    //First element of the value is length
    //Second Element of the value is the address of the byte array
    bool result = true;

    if(!value || value->length == 0)
    {
     return false;
    }

    if (value->length <= QTV_MAX_PROBE_URN_LEN)
    {
      probeURL.length = value->length;
      memcpy(probeURL.URL, value->URL, value->length);
    }
    else
    {
      result = false;
    }

    return result;
}


bool QtvConfig::SetHTTPPDRootPath(char* value)
{ 
   if(NULL != value)
   {
     std_strlcpy(http_pd_root_path,(char*)value, sizeof(http_pd_root_path));
     return true;
   }
   return false;
  
}


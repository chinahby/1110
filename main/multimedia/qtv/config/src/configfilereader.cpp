/* =======================================================================
                               ConfigFileReader.cpp
DESCRIPTION
  Class implementation for configuration file reader.

  Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/config/main/latest/src/configfilereader.cpp#34 $
$DateTime: 2010/11/09 04:48:52 $
$Change: 1509879 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_file_io.h"
#include "oscl_string_utils.h"
#include "qcplayer_oscl_utils.h"
#include "QtvConfig.h"
#include "qtv_msg.h"
#include "ConfigFileReader.h"
#include "oscl_string_utils.h"
#include "qtvstreambase.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define MAX_CONFIG_STRING_LENGTH 128

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
  
/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
const char *QtvConfigFileReader::upperCaseNameForConfigItem[] =
{
   "REBUFFERING_THRESHOLD",                        // 0
   "CODECSELECT_CRITERIA",                         // 1
   "ISREPOS_IN_LIVESTREAM_ALLOWED",                // 2
   "ISPAUSE_IN_LIVESTREAM_ALLOWED",                // 3
   "RTSPDESCRIBE_TIMEOUT",                         // 4
   "RTSPSETUP_TIMEOUT",                            // 5
   "RTSPPLAY_TIMEOUT",                             // 6
   "RTSPPAUSE_TIMEOUT",                            // 7
   "RTSPTEARDOWN_TIMEOUT",                         // 8
   "RTSPKEEPALIVE_OPTION_TIMEOUT",                 // 9
   "RTSPKEEPALIVE_OPTION_INTERVAL",                // 10
   "PAUSE_SUPERVISION_TIME",                       // 11
   "STREAMING_AV_SYNC_WAIT_VIDEO",                 // 12
   "QOS_REQUEST_TIMEOUT",                          // 13
   "QOS_RELEASE_TIMEOUT",                          // 14
   "TCP_CLOSE_TIMEOUT",                            // 15
   "NETWORK_ABORT_DELAY",                          // 16
   "ABORT_ON_MISSING_OPTIONS_RESPONSE",            // 17
   "USE_INTERLEAVED_TCP_FOR_WM",                   // 18
   "WM_FAST_START_MAX_BW_PERCENT",                 // 19
   "DATA_INACTIVITY_TIMEOUT",                      // 20
   "RTSP_OPTIONS_KEEPALIVE_ENABLED",               // 21
   "RTCP_KEEP_ALIVE_ENABLED",                      // 22
   "RTCP_RR_INTERVAL",                             // 23   
   "TEXT_RECORD_FORMAT",                           // 24
   "MAX_URL_SIZE",                                 // 25
   "MCAST_INTERFACE",                              // 26
   "MAX_BUFFER_DURATION",                          // 27
   "MAX_WM_BITRATE",                               // 28
   "MAX_PROXY_THROTTLE_THRESHOLD",                 // 29
   "SESSION_ID",                                   // 30
   "DECRYPT_EXEC_MODE",                            // 31
   "DECRYPT_EXEC_PLATFORM",                        // 32
   "ENABLE_AMR_PAYLOAD_VALIDATATION",              // 33
   "MAX_RTCP_BW_PERCENTAGE",                       // 34
   "MAX_SECOND_PDP_BANDWIDTH",                     // 35
   "RECONFIGURE_PORT",                             // 36
   "STREAMING_PREROLL",                            // 37
   "STREAMING_REBUFFER_PREROLL",                   // 38
   "STREAMING_MIN_BUFFER_DURATION",                // 39  
   "STREAMING_AUDIO_CMX_THRESHOLD",                // 40
   "TREAT_RESUME_AS_SEEK_FOR_MCAST_OR_LIVESTREAM", // 41
   "WM_PREDSTREAM_ENABLED",                        // 42
   "PLAY_DURING_SKIP",                             // 43
   "PAUSE_IS_SUSPEND",                             // 44
   "STREAMING_VIDEO_MIN_PREROLL",                  // 45
   "DEFAULT_RTSP_MESSAGE_TIMEOUT",                 // 46
   "FIREWALL_PROBE_ENABLED",                       // 47
   "TCP_NODELAY_FOR_RTSP",                         // 48
   "USER_PAUSE_TIMEOUT",                           // 49
   "BLOCK_NON_MFLO_H264",                          //50
   "ENABLED_RATE_ADAPTATION",                     // 51
   "QTV_RTSP_OPTIONS_FIRST" ,                      //52
   "PCR_DISCONTINUITY_THRESHOLD_CONST",            // 53
   "PCR_DISCONTINUITY_THRESHOLD_PPM",              // 54
   "PCR_DISCONTINUITY_DETECTION_ENABLED",          // 55
   "MAX_TS_OFFSET_OVER_PTS",                       // 56
   "MAX_TS_OFFSET_UNDER_PTS",                      // 57
   "MAX_SILENCE_DURATION",                         //58
   "STREAM_SILENCE_INSERTION_ENABLED",             //59
   "STREAM_RECORD_USE_PREALLOC_BUFFER",            // 60
   "WMPL_USER_INITIATED_SETUP_DELAY",              // 61
   "WMPL_NATURAL_TRANSITION_SETUP_DELAY",          // 62
   "GENERIC_TEXT_DEFAULT_SAMPLE_DURATION",         // 63
    "IGNORE_TIAS_BANDWITH",                          //64 
   "QOS_EVDO_DELTA_ABOVE_BANDWIDTH",               // 65
   "QOS_EVDO_FWD_LINK_PROFILE_ID",                 // 66
   "QOS_EVDO_REV_LINK_QOS_ENABLE",                 // 67
   "QOS_EVDO_REV_LINK_PROFILE_ID",                 // 68
   "MEDIAPLAYER_ROOT_PATH" ,                       // 69
   "QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY_ENABLE", //70
   "QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY",    // 71
   "AUDIO_DISCONTINUITY_THRESHOLD",                 // 72
   "ENABLE_QFRE",                                  // 73
   "ENABLE_SEI_FOR_SINGLE_NAL_DROP",               // 74
   "MAX_FRAME_DROPS_TO_IFRAME",                    // 75
   "PCR_SYNC_USE_SW_TIMESTAMPS",                   // 76
   "ENABLE_BRAZIL_SPECIFIC_H264_VUI_PARAM_TYPE",   // 77
   "WRITE_STATS_TO_FILE",                          // 78
   "BSD_IFACE_SELECTION",                          // 79 
   "ENABLE_WM_FAST_RECONNECT",                     // 80
   "WM_FAST_RECONNECT_MAX_ATTEMPTS",               // 81   
   "PROBE_URN",                                    // 82
   "FCS_KEEPALIVE_SESSION_TIMEOUT",                // 83
   "ENABLE_LINK_CHAR",                             // 84   
   "STREAM_RECORD_DEC_BUF_DURATION",               // 85   
   "RTP_MIN_BUFF_TIME",                            // 86
   "RTP_MAX_BUFF_TIME",                            // 87 
   "ENABLEVIDEO_AT_KEY_FRAME",                     // 88         
   "USE_INTERLEAVED_TCP_FOR_3GP",                  // 89  
   "VIDEO_ADUIO_VOICE_MIXING",                     // 90
   "ACCEPT_SERVER_ANNOUNCED_CLIENT_PORTS",         // 91 
   "DISABLE_PIPELINING_FOR_FIREWALL_PROBE_ENABLED", // 92
   "ENABLE_PIPELINING_FOR_SDP_PLAYBACK",           // 93
   "ENABLE_PIPELINING_FOR_STREAMING",              // 94  
   "HTTP_FILE_SAVE_TO_EFS_PATH",		    //95
   "AUTO_FALLBACK_OVER_TCP",                        //96
   "DROP_PACKET_PATTERN",                          // 97  
   "STREAMING_UDP_DROPRATE",                       // 98   
   "SIMULATE_OLD_WM_SERVER",                       // 99
   "STA_IFACE_ENABLED",                            // 100
   "PCR_DRIFT_CORRECTION_ENABLED",                 // 101
   "SR_AVG_DRIFT_FILTER_ZERO_PADDING",             // 102
   "RD_AVG_DRIFT_FILTER_ZERO_PADDING",             // 103
   "SR_DRIFT_FILTER_UPDATE_DELAY",                 // 104
   "RD_DRIFT_FILTER_UPDATE_DELAY",                 // 105
   "DRIFT_CORRECTION_DELAY",                       // 106
   "SR_DRIFT_FILTER_TYPE",                         // 107
   "RD_DRIFT_FILTER_TYPE",                         // 108
   "SR_DRIFT_FILTER_UPDATE_INTERVAL",              // 109
   "RD_DRIFT_FILTER_UPDATE_INTERVAL",              // 110
   "SR_DRIFT_FILTER_TIME_CONSTANT",                // 111
   "RD_DRIFT_FILTER_TIME_CONSTANT",                // 112
   "SR_DRIFT_FILTER_MAX_ITEMS_IN_MEMORY",          // 113
   "RD_DRIFT_FILTER_MAX_ITEMS_IN_MEMORY",          // 114
   "SR_DRIFT_FILTER_MEASUREMENT_INTERVAL",         // 115
   "RD_DRIFT_FILTER_MEASUREMENT_INTERVAL",         // 116
};

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
  
/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */
QtvConfigFileReader::QtvConfigFileReader()
{
   fp = NULL;
}

QtvConfigFileReader::~QtvConfigFileReader()
{
   if ((NULL != fp) && OSCL_IsFileOpen(fp))
   {
      OSCL_FileClose(fp);
   }
   fp = NULL;
}

bool QtvConfigFileReader::open(const char *filename) 
{
   if (NULL == filename)
   {
      return false;
   }

   fp = OSCL_FileOpen(OSCL_STRING(filename), "r");

   return (fp != NULL);
}

int QtvConfigFileReader::readItems(QtvConfig *config)
{
   if (NULL == config)
   {
      return 0;
   }

   int length = 0;
   char *configFileString = getText(&length);

   if (NULL == configFileString)
   {
      return 0;
   }

   int numItems = parseItemsFromConfigString(configFileString, length, config);
   
   QTV_Delete_Array(configFileString);

   return numItems;
}

int QtvConfigFileReader::parseItemsFromConfigString(
   const char *s, int length, QtvConfig *config)
{
   const char *end_ptr = s + length;
   // skip any leading whitespace including line terminators
   const char *section_start_ptr = skip_whitespace_and_line_term(s, end_ptr);
   const char *line_start_ptr = NULL;
   const char *line_end_ptr = NULL;
   int numItems = 0;
   int numLines = 0;   
   char temp_str[MAX_CONFIG_STRING_LENGTH];
   void *value = NULL;
   int value_str_len;
   
   while ((section_start_ptr - s) < length) 
   {
      // Declarations needed here so we can GOTO below.
      const char *equals_ptr = NULL;
      const char *name_end = NULL;
      int name_length = 0;
      QtvConfig::QtvConfigItemId item_id = QtvConfig::QTVCONFIG_ITEM_COUNT;
      const char *setting_value_str = NULL;

      bool convertRsl = false;

#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
      SessionIDType sIDvalue;
#endif /* FEATURE_QTV_ENCRYPTED_STREAMS */

#ifdef FEATURE_QTV_FCS
#error code not present
#endif 

      if (!get_next_line(section_start_ptr, end_ptr, 
          line_start_ptr, line_end_ptr) || 
          (NULL == line_start_ptr) || (NULL == line_end_ptr))
      {
         return numItems;
      }

      numLines++;

      // For next iteration, start at end of current line.
      section_start_ptr = skip_whitespace_and_line_term(line_end_ptr, end_ptr);

      // Now we have at least one character
      int line_length = line_end_ptr - line_start_ptr;

      // Skip all-whitespace line
      if (line_length <= 0)
      {         
         goto ParseItemsDoneLine;
      } 
      else if (line_length > MAX_CONFIG_STRING_LENGTH) 
      {
         QTV_MSG1( QTVDIAG_CONFIGURATION, "Line %d is too long, skipping!",  numLines );
         goto ParseItemsDoneLine;
      }

      // Check for '#' or '//' prefixed line comment
      if ((line_start_ptr[0] == '#') ||
          (strncmp(line_start_ptr, "//", 2) == 0))      
      {
         goto ParseItemsDoneLine;
      }
      
      // Check for block comment
      if (strncmp(line_start_ptr, "/*", 2) == 0)
      {
         // Just after the "/*"
         section_start_ptr = line_start_ptr + 2;

         // Finish reading block comment -- do in a loop so we can maintain an
         // accurate line count.
         while ((section_start_ptr - s) < length)          
         {
            if (!get_next_line(section_start_ptr, end_ptr, 
               line_start_ptr, line_end_ptr) || 
               (NULL == line_start_ptr) || (NULL == line_end_ptr))
            {
               QTV_MSG(QTVDIAG_CONFIGURATION, "Unclosed block comment in config file");
               return numItems;
            }                     

            numLines++;

            line_length = line_end_ptr - line_start_ptr;

            if (line_length < 0)
            {
               // What the ???
               return numItems;
            }

            const char *comment_end_ptr = oscl_strnstr(line_start_ptr, "*/", line_length);

            if (comment_end_ptr != NULL)
            {
               // Point to just after the comment -- any characters on the same line will be
               // treated as starting a new line.
               section_start_ptr = comment_end_ptr  + 2;
               goto ParseItemsDoneLine;
            }

            section_start_ptr = line_end_ptr;
         } // while not at the end of the string

         QTV_MSG(QTVDIAG_CONFIGURATION, "Unclosed block comment in config file");
         return numItems;
      } // if start of block comment

      if (line_length < 3)
      {
         std_strlcpy(temp_str, line_start_ptr, 4);
         QTV_MSG2( QTVDIAG_CONFIGURATION, "Illegal line in config file: %s on line %d", 
                   temp_str, numLines);
      }

      equals_ptr = oscl_strnstr(line_start_ptr, "=", line_length);

      if ((equals_ptr == NULL) || (equals_ptr <= line_start_ptr) || 
          (equals_ptr >= line_end_ptr - 1))
      {
         QTV_MSG1( QTVDIAG_CONFIGURATION, 
            "Invalid line (no = or starts with = or ends with =) on line %d", 
            numLines );
         goto ParseItemsDoneLine;
      }

      name_end = equals_ptr - 1;

      // Strip trailing spaces from setting name
      while ((name_end > line_start_ptr) && 
             ((*name_end == ' ') || (*name_end == '\t')))              
      {
         name_end--;
      }

      name_length = MIN(sizeof(temp_str) - 1, name_end - line_start_ptr + 1);

      if (name_length <= 0)
      {
          QTV_MSG1( QTVDIAG_CONFIGURATION, "Empty name on line %d",  numLines );
          goto ParseItemsDoneLine;
      }

      strncpy_upper(temp_str, line_start_ptr, name_length);         

      temp_str[name_length] = '\0';

      item_id = getItemIDForName(temp_str);

      if (item_id >= QtvConfig::QTVCONFIG_ITEM_COUNT)
      {
         QTV_MSG1( QTVDIAG_CONFIGURATION, "Unknown setting name: %s", temp_str);
         goto ParseItemsDoneLine;
      }

      // Get the string version of the value
      setting_value_str = skip_whitespace(equals_ptr + 1, line_end_ptr);      
      
      if ((NULL == setting_value_str) || (setting_value_str >= line_end_ptr))
      {
        QTV_MSG1( QTVDIAG_CONFIGURATION, "Blank value for: %s", temp_str);
        goto ParseItemsDoneLine;
      }

      value_str_len = MIN(line_end_ptr - setting_value_str, 
                          MAX_CONFIG_STRING_LENGTH - 1);
      ZUtils::StrcpyN(temp_str, setting_value_str, value_str_len + 1);         

      switch (item_id)
      {
#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
         case QtvConfig::QTVCONFIG_SESSION_ID:
           convertRsl = convertSessionID(temp_str, value_str_len, &sIDvalue);
           //sIDValue's lifespan should cover following SetQTVConfigItem()
           value = &sIDvalue;
           break;
#endif /* FEATURE_QTV_ENCRYPTED_STREAMS */
         case QtvConfig::QTVCONFIG_MEDIAPLAYER_ROOT_PATH:
           value = (void*)&temp_str;
           convertRsl = true;
           break;
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
         case QtvConfig::QTVCONFIG_HTTP_FILE_SAVE_TO_EFS_PATH:
           value = (void*)&temp_str;
           convertRsl = true;
           break;
         default:
           convertRsl = convertValue(item_id, temp_str, value_str_len, &value);
           break;
      }

      if (convertRsl)
      {
        // Success!!!
        if (config->SetQTVConfigItem(item_id, (int32)value))
        {
          numItems++;
        }
        else
        {
          QTV_MSG_PRIO3( QTVDIAG_CONFIGURATION, QTVDIAG_PRIO_ERROR,
            "Could not set value for item %d to %d on line %d", 
            item_id, value, numLines);    
        }
      }
      else
      {
        QTV_MSG_SPRINTF_PRIO_2( QTVDIAG_CONFIGURATION, QTVDIAG_PRIO_ERROR,
            "Could not parse value %s on line %d", 
            temp_str, numLines);
      }

ParseItemsDoneLine:    
      ;
   }

   return numItems;
}

char *QtvConfigFileReader::getText(int *pLength)
{  
   // Declarations needed here so we can GOTO below.
   int fileLength = 0;
   char *text = NULL;   
   
   if (NULL == fp)
   {
      goto GetTextCleanupNoFile;
   }

   // Get length of file   
   if (OSCL_FileSeek(fp, 0, SEEK_END) != 0)
   {
      goto GetTextCleanup;
   }

   fileLength = OSCL_FileTell(fp);
   if (fileLength <= 0)
   {
      goto GetTextCleanup;
   }

   if (pLength != NULL)
   {
      *pLength = fileLength;
   }

   if (OSCL_FileSeek(fp, 0, SEEK_SET) != 0)
   {
      return NULL;
   }

   text = (char *) QTV_New_Array(char, (fileLength + 1));

   if (text == NULL)
   {
      // out of memory
      goto GetTextCleanup;
   }
  
   if (OSCL_FileRead((void *) text, sizeof(char), fileLength, fp) !=
       fileLength)
   {
      goto GetTextCleanup;
   }

   // Success
   OSCL_FileClose(fp);
   fp = NULL;

   *pLength = fileLength;
   return text;

GetTextCleanup:
   // Handle error from above

   // Close file   
   OSCL_FileClose(fp);
   fp = NULL;
 
GetTextCleanupNoFile:
   if (NULL != pLength)
   {
      *pLength = 0;
   }
    
   return NULL;
}

QtvConfig::QtvConfigItemId 
QtvConfigFileReader::getItemIDForName(const char *name)
{
   if (name == NULL)
   {
      return QtvConfig::QTVCONFIG_ITEM_COUNT;
   }

   for (int i = 0; i < (int) QtvConfig::QTVCONFIG_ITEM_COUNT; i++)
   {
      if (strcmp(upperCaseNameForConfigItem[i], name) == 0)
      {
         return (QtvConfig::QtvConfigItemId) i;
      }
   }

   // Couldn't find ID
   return QtvConfig::QTVCONFIG_ITEM_COUNT;
}

bool QtvConfigFileReader::convertValue(QtvConfig::QtvConfigItemId item_id, 
  const char *setting_value_str, int value_str_length, void *pValue)
{
    QTV_USE_ARG1(value_str_length);

   if ((setting_value_str == NULL) || (setting_value_str[0] == '\0'))
   {
      return false;
   }

   switch (item_id)
   {
     default:
          return convertInt32Value(setting_value_str,pValue);
   }
}


bool QtvConfigFileReader::stripBrackets(const char *& str, const char *& end_str)
{
  if (end_str - str < 2) 
  {
    QTV_MSG_PRIO(QTVDIAG_CONFIGURATION, QTVDIAG_PRIO_ERROR,
       "Invalid packet pattern 0");
    return false;
  }

  // Skip initial whitespace in case we haven't already.
  str = skip_whitespace(str, end_str);
  
  // Minimum string = "[]"
  if ((end_str - str < 2) || (str[0] != '['))
  {
    QTV_MSG_PRIO(QTVDIAG_CONFIGURATION, QTVDIAG_PRIO_ERROR,
      "Invalid packet pattern 2");
    return false;
  }
  
  // Strip first [
  str++;

  // Skip spaces after [
  str = skip_whitespace(str, end_str);

  // Strip last ], but ignore trailing whitespace
  bool found = false;
  for (; (end_str > str) && !found; end_str--) 
  {
    char c = end_str[-1];

    if (c == ']') 
    {
      found = true;
    }
    else if (oscl_iswhite(c)) 
    {
      ; 
    }
    else
    {
      QTV_MSG_PRIO1(QTVDIAG_CONFIGURATION, QTVDIAG_PRIO_ERROR,
        "Found junk character %c at end of packet pattern", c);
      return false;
    }
  }

  if (!found)
  {
    QTV_MSG_PRIO(QTVDIAG_CONFIGURATION, QTVDIAG_PRIO_ERROR,
      "Did not find trailing ] for packet pattern");
    return false;
  }
  
  // Truncate the trailing spaces
  while ((end_str > str) && oscl_iswhite(end_str[-1])) {    
    end_str--;
  }

  return true;
}


bool QtvConfigFileReader::convertInt32Value(const char *setting_value_str, 
                                            void *pValue)
{
  int32 value;
  int32 *piValue = (int32 *) pValue;

  // Use a dummy pointer below in case pValue is NULL.
  if (NULL == piValue) {
    piValue = &value;
  }
  *piValue = -1;

  char c = setting_value_str[0];
  
  if (strncmp(setting_value_str, "0x", 2) == 0)           
  {
    // Hex number
    uint32 uvalue;

    if (PV_atoi(setting_value_str + 2, 'x', uvalue))
    {
        *piValue = (int32) uvalue;
        return true;
    }
    return false;                   
  }
  else if ((c == '-') || (('0' <= c) && (c <= '9')))
  {
    // Decimal number
    bool isNegative = (c == '-');

    uint32 uvalue;

    if (PV_atoi(setting_value_str + (isNegative ? 1 : 0), 'd', uvalue))
    {
      *piValue = (isNegative ? -(int32) uvalue : (int32) uvalue);
      return true;
    }
    return false;                   
  }
  else if ((strcmpi(setting_value_str, "y") == 0) ||
      (strcmpi(setting_value_str, "yes") == 0) ||
      (strcmpi(setting_value_str, "t") == 0) ||
      (strcmpi(setting_value_str, "true") == 0))
  {
    *piValue = 1;
    return true;
  }
  else if ((strcmpi(setting_value_str, "n") == 0) ||
          (strcmpi(setting_value_str, "no") == 0) ||
          (strcmpi(setting_value_str, "f") == 0) ||
          (strcmpi(setting_value_str, "false") == 0))
  {
    *piValue = 0;
    return true;
  }
  return false;
}


#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
bool QtvConfigFileReader::convertSessionID(const char *setting_value_str, 
                                           int value_str_length,
                                           SessionIDType* pValue)
{
  bool returnCode = false;

  if (!pValue || !setting_value_str || value_str_length == 0)
  {
    return false;
  }

  memset( pValue->sID, 0x0, MAX_SESSIONID_LEN);

  //Todo: skip whitespace if needed;
  if (strncmp(setting_value_str, "0x", 2) == 0)
  {
    setting_value_str += 2;
    value_str_length -= 2;

    returnCode = true;
    uint32 uvalue = 0;
    uint8 len = ((value_str_length + 1) / 2);
    for(int i = 0; i < len; i++)
    {
      if (PV_atoi(setting_value_str + i*2, 'x',2, uvalue))
      {
          pValue->sID[i] = (uint8) uvalue;
      }
      else
      {
        returnCode = false;
        break;
      }
    }
    pValue->length = len;
  }

  return returnCode;
}
#endif

bool QtvConfigFileReader::convertProbeURL(const char *setting_value_str, 
                                           int value_str_length,
                                           ProbeURLType* pValue)
{
  bool returnCode = false;

  if (!pValue || !setting_value_str || value_str_length == 0)
  {
    return false;
  }

  memset( pValue->URL, 0x00, QTV_MAX_PROBE_URN_LEN);

  if(value_str_length < (QTV_MAX_PROBE_URN_LEN -1))
  {
    memcpy(pValue->URL,setting_value_str,value_str_length);
    pValue->URL[value_str_length+1] = '\0';
    pValue->length = value_str_length+1;
    returnCode = true;
  }

  return returnCode;
}

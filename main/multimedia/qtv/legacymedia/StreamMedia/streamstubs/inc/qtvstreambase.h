/* =======================================================================
         qtvstreambase.h
DESCRIPTION
   This module defines the qtvstreambase class.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/inc/qtvstreambase.h#3 $
$DateTime: 2011/02/02 22:29:39 $
$Change: 1605117 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

#ifndef QTVSTREAMBASE_H_
#define QTVSTREAMBASE_H_

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtvconfig.h"
#include "sdpfile.h"

#include "dsbsd.h"
#include "Media.h"
#include "rtsp_par_com_basic_ds.h"

const int DEFAULT_TIMEOUT               = 30000; /* 30 secs */
const int DEFAULT_OPTIONS_TIMEOUT       = 60000; /* 60 secs */
const int DEFAULT_DATA_INACTIVITY_TIMEOUT = 60000; /*60 secs*/
const int DEFAULT_RTP_MIN_BUFFER_DURATION   = 2500; /* ms */
const int DEFAULT_RTP_MAX_BUFFER_DURATION   = 5000; /* ms */
#ifdef FEATURE_QTV_MCAST
/*default broadcast interface*/
const uint8 DEFAULT_BCAST_IFACE = (uint8) BSD_IFACE_CDMA_BCAST;
#endif

enum QtvStreamCode
{
  PV_STREAM_FAIL                    = 0,
  PV_STREAM_SUCCESS                 = 1,
  PV_STREAM_INVALID_URL             = 2,
  PV_STREAM_INVALID_SDP             = 3,
  PV_STREAM_INVALID_STATE           = 4,
  PV_STREAM_INVALID_SESSION         = 5,
  PV_STREAM_INVALID_CHANNEL         = 6,
  PV_STREAM_INVALID_TRACK           = 7,
  PV_STREAM_INVALID_DATA_PACKET     = 8,
  PV_STREAM_UNKNOWN_PAYLOAD         = 9,
  PV_STREAM_TOO_MANY_SESSIONS       = 10,
  PV_STREAM_SAMPLE_TRUNCATED        = 11,
  PV_STREAM_NO_DATA                 = 12,
  PV_STREAM_BUFFER_TOO_SMALL        = 13,
  PV_STREAM_NETWORK_ERROR           = 14,
  PV_STREAM_SOURCE_TIMEOUT          = 15,
  PV_STREAM_CLIP_NOT_AVAILABLE      = 16,
  PV_STREAM_PLAYER_TOO_OLD          = 17,
  PV_STREAM_OUT_OF_MEMORY           = 18,
  PV_STREAM_PACKET_LOSS             = 19,
  PV_STREAM_BAD_PACKET              = 20,
  PV_STREAM_WAITING                 = 21,
  PV_STREAM_NET_WAITING             = 22,
  PV_STREAM_EOS                     = 23 ,
  PV_STREAM_FRAME_READY             = 24,
  PV_STREAM_PARTIAL_FRAME           = 25,
  PV_STREAM_TRYAGAIN                = 26,
  PV_STREAM_REDIRECT                = 27,
  PV_STREAM_DELAYED_REDIRECT        = 28,
  PV_STREAM_RTSP_REQ_DISCARDED      = 29,
  PV_STREAM_QOS_AVAILABLE_MODIFIED  = 30,
  PV_STREAM_QOS_UNAVAILABLE         = 31,
  PV_STREAM_SERVER_CLOSED           = 32,
  PV_STREAM_RELEASE_QOS_FAIL        = 33,
  PV_STREAM_RELEASE_QOS_TIMEOUT     = 34,
  PV_STREAM_SERVER_SIDE_PLAYLIST_SKIP_SUCCESS = 35,
  PV_STREAM_SERVER_SIDE_PLAYLIST_SKIP_FAILED  = 36,
  PV_STREAM_SERVER_SIDE_PLAYLIST_TRANSITION   = 37,
  PV_STREAM_DESCRAMBLER_PROXY_FAIL  = 38
};

enum ServerSidePlaylistType
{
  SERVER_SIDE_PLAYLIST_TYPE_NONE,
  SERVER_SIDE_PLAYLIST_TYPE_PV,
#ifdef FEATURE_QTV_WINDOWS_MEDIA
  SERVER_SIDE_PLAYLIST_TYPE_WM
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
};

/* real-time millisecond clock function */
uint32 pvGetTickCount();
uint64 pvCvt2Systime(uint32 time);

bool get_next_line(const char *start_ptr, const char * end_ptr, 
                   const char *& line_start, 
                   const char *& line_end);

// QtvStream functions used in Mpeg4Player
class QtvStream
{
public:

  QtvStream(QtvConfig *configObject, bool *status = NULL, 
            void (*truncationCallback)(void) = NULL, 
            SDPInfo *sdpInfo = NULL, bool log = false);

  ~QtvStream();

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif

};

#endif

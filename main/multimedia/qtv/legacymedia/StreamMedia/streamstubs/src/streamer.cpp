/* =======================================================================
         streamer.h
DESCRIPTION
   This module defines the streamer class.

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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/src/streamer.cpp#4 $
$DateTime: 2011/02/02 22:29:39 $
$Change: 1605117 $

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
#include "streamer.h"
#include "qtvstreambase.h"
#include "streammedia.h"
#include "QtvConfig.h" 
#include "Events.h"

// Source
EVENT_POOL( STREAMER_TEMP_POOL, 10 )
//to player
EVENT_SOURCE( STREAMER_STATUS, STREAMER_TEMP_POOL )


bool Streamer::GetRTPStatistics(QtvPlayer::RTPStatisticsT &statistics)
{
  QTV_USE_ARG1(statistics.NumTracks);
  return FALSE;
}
bool Streamer::RetrySessionWithAuthorization(char *)
{
  return FALSE; 
}
void Streamer::Destroy()
{
};
void Streamer::Stop(bool &bError)
{
  QTV_USE_ARG1(bError);
};
bool Streamer::Pause(bool &bError)
{
  QTV_USE_ARG1(bError);
  return FALSE;
}
bool Streamer::Resume(bool &bError)
{
  QTV_USE_ARG1(bError);
  return FALSE;
}
bool Streamer::Create(void * pContext, QtvStream *pQtvStream, QtvConfig *configObject)
{
  QTV_USE_ARG2(pContext,pQtvStream);
  QTV_USE_ARG1(configObject);
  return FALSE;
}

bool Streamer::IsPaused() 
{
  return FALSE;
}

bool Streamer::StartStreaming( bool &bError,
                               long nStart,
                               long nStop,
                               bool bReposition)
{
  QTV_USE_ARG2(bError,nStart);
  QTV_USE_ARG2(nStop,bReposition);
  return FALSE;
}
void Streamer::SetStreamBufferingParameters(bool bIsRebuffering)
{
  QTV_USE_ARG1(bIsRebuffering);
};


#ifdef FEATURE_QTV_WINDOWS_MEDIA
void Streamer::SendConnectTimeLog(void)
{
};

#endif

//#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
//    void Streamer::SetSescramblerHandler(void *Instance)
//    {
//    }
//#endif

#ifdef FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST
bool Streamer::AdjustTransitionTimeForServerSidePlaylist(int64 &transitionTime)
{
  return FALSE;
}
bool Streamer::AdjustPlaybackTimesForServerSidePlaylist(int64 &audioPlaybackTime, 
                                                        int64 &videoPlaybackTime, int64 &textPlaybackTime)
{
  return FALSE;
}
#endif
#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
bool Streamer::ShouldUseSkipForReposition(int32 startTime)
{
  return FALSE;
}
bool Streamer::SwitchToNextSDPInServerSidePlaylist()
{
  return FALSE;
}
bool Streamer::UpdateAfterSwitchingSDPForServerSidePlaylist( bool bUserInitated)
{
  return FALSE;
}
int32 Streamer::GetPlayingServerSidePlaylistClipIndex()
{
  return FALSE;
}

bool Streamer::SkipToClipInServerSidePlaylist(const char *playlistName, 
                                              int32 clipIndex,  bool isClipIndexRelative, int32 time, int32 when,
                                              bool keepStreaming, bool shouldPlayDuringSkip)
{
  return false;
}
bool Streamer::IsNextClipInServerSidePlaylistReady()
{
  return false;
}
#endif
bool Streamer::StartConnecting( const int playbackID, 
                                StreamMedia *pStreamMedia, 
                                const URL &url,
                                const KEY_DEF *pKey,
                                bool &bError )
{
  QTV_USE_ARG2(playbackID,pStreamMedia);
  QTV_USE_ARG2(url.MaxUrlSize,pKey);
  QTV_USE_ARG1(bError);
  return FALSE;
}

#ifdef  FEATURE_QTV_QOS_SELECTION
#error code not present
#endif


bool Streamer::InitiateStop()
{
  return true;
}
uint32 Streamer::getTotalRxDataRate()
{
  return 0;
}

void Streamer::setSpeed(float speed)
{
}

void Streamer::setAccDuration(int time)
{
}

bool Streamer::Reconnect
(
  bool &bError
)
{
  return FALSE;
}

ReconnectionStatus Streamer::getReconnectionStatus()
{
 return RECONNECT_STATUS_INVALID;
}

bool Streamer::PrepareForReconnect(bool isReconnectForFirewall)
{ 
  return FALSE;
}

bool Streamer::isReconnectionInProgress()
{
  return FALSE;
}

bool Streamer::TeardownForReconnect()
{
  return FALSE;
}

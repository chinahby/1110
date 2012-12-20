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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/inc/streamer.h#4 $
$DateTime: 2011/02/02 22:29:39 $
$Change: 1605117 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#ifndef STREAMER_H_
#define STREAMER_H_
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"

#include "qtvstreambase.h"

//Forward declaration
class QtvStream;
class QtvConfig;
class StreamMedia;

enum ReconnectionStatus
{
  RECONNECT_STATUS_INVALID,
  RECONNECT_STATUS_REQUIRED,
  RECONNECT_STATUS_IN_PROGRESS,
  RECONNECT_STATUS_COMPLETE,
  RECONNECT_STATUS_EOS_DETECTED
};



// Streamer functions used in Mpeg4Player
class Streamer
{

public:
  bool GetRTPStatistics(QtvPlayer::RTPStatisticsT &statistics);
  bool RetrySessionWithAuthorization(char *);
  void Destroy();
  void Stop(bool &bError);
  bool Pause(bool &bError);
  bool Resume(bool &bError);
  void SetStreamBufferingParameters(bool bIsRebuffering);
  bool Create(void * pContext, QtvStream * pQtvStream, QtvConfig *configObject);

  bool IsPaused() ;

 void setSpeed(float speed);
 void setAccDuration(int time);
 bool Reconnect(bool &bError );
 
  bool StartStreaming( bool &bError,
                       long nStart,
                       long nStop,
                       bool bReposition);

#ifdef FEATURE_QTV_WINDOWS_MEDIA
  void SendConnectTimeLog(void);
#endif

#ifdef FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST
  bool AdjustTransitionTimeForServerSidePlaylist(int64 &transitionTime);
  bool AdjustPlaybackTimesForServerSidePlaylist(int64 &audioPlaybackTime, 
                                                int64 &videoPlaybackTime, int64 &textPlaybackTime);

#endif
#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
  bool ShouldUseSkipForReposition(int32 startTime);
  bool SwitchToNextSDPInServerSidePlaylist();
  bool UpdateAfterSwitchingSDPForServerSidePlaylist( bool bUserInitated);
  int32 GetPlayingServerSidePlaylistClipIndex();
  bool IsNextClipInServerSidePlaylistReady();   
  bool SkipToClipInServerSidePlaylist(const char *playlistName, 
                                      int32 clipIndex,  bool isClipIndexRelative, int32 time, int32 when,
                                      bool keepStreaming, bool shouldPlayDuringSkip);

#endif
#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
  void SetSescramblerHandler(void *Instance)
  {
  };
#endif

ReconnectionStatus getReconnectionStatus();
bool PrepareForReconnect(bool isReconnectForFirewall = false);
bool isReconnectionInProgress();
bool TeardownForReconnect();

  bool InitiateStop();
  uint32 getTotalRxDataRate();
#ifdef  FEATURE_QTV_QOS_SELECTION
#error code not present
#endif

  bool StartConnecting( const int playbackID, 
                        StreamMedia *pStreamMedia, 
                        const URL &url,
                        const KEY_DEF *pKey,
                        bool &bError );

};
#endif


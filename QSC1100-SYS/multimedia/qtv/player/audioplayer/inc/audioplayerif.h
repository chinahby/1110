#ifndef _AudioPlayerIf_H_
#define _AudioPlayerIf_H_
/* =======================================================================

                          audioPlayerIf.h

DESCRIPTION
  Definition of the public interface to the audio sybsystem. Clients of the 
  audio player can only communicate to the player via this interface.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Init() should be called before making further calls on the subsystem.

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/inc/audioplayerif.h#10 $
$DateTime: 2008/10/10 11:52:23 $
$Change: 760476 $

========================================================================== */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Callback function used to return post an event to the client.
**
** Parameters:
**
**   pEvent:  The event to pe posted onto the client on the client
**            task context.
**   pCbData: the client data the application passed in when it created
**            the audio player in the first place.
*/
typedef void (*PLAYER_NOTIFY_CB_FN)( QCMessageType * const   pEvent,
                                     void * const            pCbData );


typedef uint32 AUDIO_PLAYER_ID;

class AudioPlayerIf
{

public:

  /* Should be the clients first call into the audio subsystem.*/
  static void Init();

  /* Used to create a new AudioPlayer object. The new audioPlayer will be
  ** managed by this interface layer. The client will get the audio player id,
  ** which it should use to make further request. */
  static bool CreateAudioPlayer(AUDIO_PLAYER_ID&      apInst, 
                                PLAYER_NOTIFY_CB_FN   playerCB,
                                void*                 pClientData);

  /* Used to inform the audio player that it should use the specified
  ** obj ptr to set AV sync update information */
  static void SetAVSyncObj(AUDIO_PLAYER_ID const recipientID, void* pAVSync);

  /* Used to initialize the audio player */
  static bool initializeAudioplayer(AUDIO_PLAYER_ID const recipientID, 
                                    const char *audio_type,
                                    Media *pMedia = NULL);

  /* Used to cleanup the resources used by the audio subsystem.*/
  static void Destroy(AUDIO_PLAYER_ID const recipientID);


  /* Start the audio player*/
  static bool Start(AUDIO_PLAYER_ID const recipientID, bool bRestart);

  /* Pause the audio player*/
  static bool Pause(AUDIO_PLAYER_ID const recipientID);

  /* Resume the audio player*/
  static bool Resume(AUDIO_PLAYER_ID const recipientID, long stop);

  /* Suspend the audio player*/
  static bool Suspend(AUDIO_PLAYER_ID const recipientID);

  /* Stop the audio player*/
  static bool Stop(AUDIO_PLAYER_ID const recipientID);

  /* Used to setup the audio manager*/
  static bool Prep(AUDIO_PLAYER_ID const recipientID, int playbackID, 
                   bool bRestart, void* pMedia, bool bHasVideo, Media::CodecType audioCodec);

  /* Used to start audio after buffering is over.*/
  static bool RestartAudioOutput(AUDIO_PLAYER_ID const recipientID);

  /* It Stops->Preps->Starts the audio player*/
  static bool RestartAudio( AUDIO_PLAYER_ID const recipientID );

  /* Fetch the audio track elapsed time in msec*/
  static long GetElapsedTime(AUDIO_PLAYER_ID const recipientID);

  /* See how far data processing was ahead of audio playback at the current clock time*/
  static long GetAmtBufferedInCmx(AUDIO_PLAYER_ID const recipientID);

  static bool EnableTimeCheck( AUDIO_PLAYER_ID const recipientID, unsigned long timeValue );

  static bool GetPosition( AUDIO_PLAYER_ID const recipientID, int32& pos );

  /* Fetch the audio stats during playback. Ususally used at the end of playback*/
  static bool GetStatistics( AUDIO_PLAYER_ID const recipientID, 
                             QtvPlayer::AudioVideoStatisticsT &stats, 
                             Common::LastFrame &lastFrame);

  /* See if audio player is in IDLE state or done playing*/
  static bool IsDone( AUDIO_PLAYER_ID const recipientID );

  static bool IsPlaying( AUDIO_PLAYER_ID const recipientID );
  
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

  static bool IsPaused( AUDIO_PLAYER_ID const recipientID );

  static bool IsPausedDisabled( AUDIO_PLAYER_ID const recipientID );

  static bool IsIdle( AUDIO_PLAYER_ID const recipientID );

  static bool TrackHasEnded( AUDIO_PLAYER_ID const recipientID );

  static void ResetData( AUDIO_PLAYER_ID const recipientID );

  /* Used during repositioning to set the new start/stop time for the audio track*/
  static bool SetPlayTimes( AUDIO_PLAYER_ID const recipientID, long start, long stop);
  static void SetTSfromBytes(AUDIO_PLAYER_ID const recipientID, int32 TSfromBytes);

  /* Used to drain the audio buffer during release-restore*/
  static bool DrainAudioBuffer(AUDIO_PLAYER_ID const recipientID, uint32 timestamp);
  static bool Disable(AUDIO_PLAYER_ID const recipientID);

  static bool SetAudioChannelSwitchTS(AUDIO_PLAYER_ID const recipientID, uint32 reqData);

  /*Used to show statistical information about the audio manager, used by the 
  ** specified audio player*/
  static bool ShowInfo(AUDIO_PLAYER_ID const recipientID);

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

#ifdef FEATURE_QTV_FAST_PLAYBACK_AUDIO
  /* Sets the playback speed */
  static void SetPlaybackSpeed(AUDIO_PLAYER_ID const recipientID,
                               Common::PlaybackSpeedType const pbSpeed);
#endif /* FEATURE_QTV_FAST_PLAYBACK_AUDIO */

#ifdef FEATURE_QTV_GENERIC_BCAST
  /* Returns the audio track information */
  static bool GetAudioTrackSpec(AUDIO_PLAYER_ID const recipientID,
                                uint32 &bmTrackState,
                                Common::ChannelConfigType &eChannelConfig,
                                int &nSamplingFreq);
#endif /* FEATURE_QTV_GENERIC_BCAST */

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  /* Sets the dual mono output configuration */
  static bool SetDualMonoOutput(AUDIO_PLAYER_ID const recipientID, 
                                Common::DualMonoOutputType const dualMonoOutput);
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */
#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

};

#endif //_AudioPlayerIf_H_ 


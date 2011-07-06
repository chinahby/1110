/// ===========================================================================
/// 
/// @file
/// @brief Definition of Mpeg4Player::PlayClip.
/// 
/// Copyright 2007 QUALCOMM Incorporated, All Rights Reserved.
/// 
/// ===========================================================================

// ============================================================================
//                              Edit History
// 
// $Header: //source/qcom/qct/multimedia/qtv/player/playertask/main/latest/src/mpeg4player__playclip.cpp#26 $
// $DateTime: 2010/07/08 00:58:18 $
// $Change: 1359722 $
// 
// ============================================================================

// ============================================================================
//               Includes and Public Data Declarations
// ============================================================================

// ============================================================================
//
//                   INCLUDE FILES FOR MODULE
//
// ============================================================================
#include "qtvinternaldefs.h" // must be first so features are defined correctly
#ifdef FEATURE_QTV_GENERIC_BCAST
#include "gbm_ui.h"
#include "genericbcastmedia.h"
#endif
#include "mpeg4player.h"
#include "qtv_conc_mgr.h"

extern "C"
{
#include "clkregim.h"
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif

#ifdef FEATURE_CMI_MM
#error code not present
#endif
}

#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
#include "qtv_mapi_params.h"
#include "Streamer_v2.h"
#else
#include "Streamer.h"
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */

// ============================================================================
//
//                      DATA DECLARATIONS
//
// ============================================================================

// ----------------------------------------------------------------------------
// Constant / Define Declarations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Type Declarations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Global Constant Data Declarations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Global Data Declarations
// ----------------------------------------------------------------------------

// ============================================================================
//                        Macro Definitions
// ============================================================================

// ============================================================================
//                        Class & Function Definitions
// ============================================================================

/// ---------------------------------------------------------------------------
/// 
/// Begins clip playback, both from scratch and in 'restart' cases
/// like pause or suspend/resume.
/// 
/// Start and stop times have some special values:
///   -1 for the stop time means 'until end of media'.
///   -1 for start means...what does it mean?
///    0 for start means 'beginning of media;.
/// 
/// @param[in] startTime The timestamp at which to begin playback
/// @param[in] stopTime The timestamp at which to end playback
/// @param[in] pbSpeed The speed at which playback is to progress
/// @return true for success, false otherwise.
/// 
/// ---------------------------------------------------------------------------
bool Mpeg4Player::PlayClip( int32 startTime, 
                            int32 stopTime,
                            const URL* urlToSwitch,
                            Common::PlaybackSpeedType pbSpeed )
{
   // get the start time
   qtv_event_clip_started_payload.start_time = startTime;

   QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                 "Mpeg4Player::PlayClip %d %d %d", 
                 startTime, stopTime, pbSpeed);

#if (defined(FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY) || \
     defined(FEATURE_QTV_FAST_PLAYBACK_AUDIO))
   /* Set the playback speed */
   SetPlaybackSpeed(pbSpeed);
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY ||
          FEATURE_QTV_FAST_PLAYBACK_AUDIO */

   //make sure there's a valid urn
   if ( !videoURN.bValid && !audioURN.bValid && !textURN.bValid )
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Error: PlayClip with no URN");
      return (false);
   }

#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
   bool bInitialPlay = false;

   if ( (startTime == 0) &&
        (playerState != BUFFERING) && (playerState != PLAYING) &&
        (playerState != PAUSED) )
   {
      bInitialPlay = true;
   }

   if ( !bInitialPlay && clip.bStreaming && streamer &&      
      streamer->ShouldUseSkipForReposition(startTime))
   {
      return (Skip(NULL, 0, true, startTime));
   }
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */

   reposControl.reposType = 0;

   /*Allow Video to set AV Offset when Audio is underrun..Used for MFLO Only */
   bAllowVidToSetOffSetOnAudUnderrun = true;

#ifndef FEATURE_QTV_DISABLE_CONC_MGR
   if ( qtv_conc_mgr::can_process_command(this, QTV_PLAYER_MIME_TYPE) )
   {
      UpdateAudioConcState();
      if ( playerAudioConcState == AUDIO_CONCURRENCY_NOT_SUPPORTED )
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                      "PlayClip not supported");
         Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_NOT_SUPPORTED);
         return (true);
      }
   }
   else
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                   "PlayClip not supported");
      Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_NOT_SUPPORTED);
      return (false);
   }
#endif

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
   if ( clip.bDownload )
   {
      if ( (startTime != -1) &&
           ((unsigned long)startTime > GetElapsedTime()) &&
           !bDownloadDone )
      {
         Notify(QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED);
         QTV_MSG( MSG_LEGACY_MED,
                  "PDS2: FF not allowed as Download is not complete");
         return (false);
      }
      //make sure there's a valid download filename
      //For a user command, make sure user playback is allowed
      //by the PVX.
      if ( appData.bValid )
      {
         if ( !appData.data.UserPlaybackAllowed )
         {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                         "PDS2: User playback not allowed");
            return (false);
         }
      }

   }
#endif
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
   if ( clip.bQtvHttpStreaming )
   {
      if ( (startTime != -1) &&
           ((unsigned long)startTime > GetElapsedTime()) &&
           !bDownloadDone )
      {
         Notify(QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED);
         QTV_MSG( MSG_LEGACY_MED,
                  " FF not allowed as HTTP Download is not complete");
         return (false);
      }
   }
#endif
   //See what action to take.
   bool bDoRestart=false;
   bool bDoStartNow=false;
   bool bDoStartAsap=false;
   bool bDoResume=false;
   bool bDoOpenClip=false;

// register for CPU resources if not localfile playback
#ifdef FEATURE_QTV_DYNAMIC_CPU_SWITCHING
   if ( clip.bStreaming
        || clip.bBcastISDB
#ifdef FEATURE_QTV_BCAST_FLO
#error code not present
#endif /* FEATURE_QTV_BCAST_FLO */
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
        || clip.bDownload
#endif
#ifdef FEATURE_QTV_PSEUDO_STREAM
        || clip.bPseudoStreaming
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
        || clip.bHTTPStreaming
#endif
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
        || clip.bQtvHttpStreaming
#endif
      )
   {
      clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QTV_PROTOCOLS_HIGH);
   }
#elif defined(FEATURE_QTV_STATIC_CPU_HIGH)
   // No dynamic clock, so set high clock during all media playback
   clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QTV_PROTOCOLS_HIGH);
#endif

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

   if ( clip.bDownload )
   {
      // Pseudo-streaming
      switch ( playerState )
      {
         case IDLE:
            if ( bDownloadDone )
            {
               bDoStartNow=true;      //download is complete.
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                            "PDS2: PlayClip: bDoStartNow");
            }
            else
            {
               bDoOpenClip=true;
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                            "PDS2: PlayClip: bDoOpenClip");
            }


            break;

         case DOWNLOADING:
            //normal pseudo-stream start.
            bDoStartNow=true;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "PDS2: PlayClip: bDoStartNow");

            break;

         case CONNECTING:
            //download is pending, waiting on connection.
            bDoStartAsap=true;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "PDS2: PlayClip: bDoStartAsap");

            break;

         case OPENING:
            //downloading & waiting on MP4 file header.
            bDoStartAsap=true;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "PDS2: PlayClip: bDoStartAsap");

            break;

         case BUFFERING:
         case PLAYING:
            bDoRestart=true;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "PDS2: PlayClip: bDoRestart");

            break;

         case PAUSED:
         case SUSPENDED:
            bDoResume=true;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "PDS2: PlayClip: bDoResume");

            break;

         case PB_READY:
            // server is connected & ready to play.
            bDoStartNow=true;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "PDS2: PlayClip: bDoStartNow");

            break;


         default:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                         "Weird state in PlayClip for pseudostream");
            return (false);
      }
   }
   else
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
      if ( clip.bStreaming )
   {
     if (streamer->isReconnectionInProgress() && (startTime > 0) && 
         (playerState != PB_READY ))
     {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                    "Ignoring this seek request, reconnection in progress");
       return false;
     }
 
     if(streamer->isReconnectionInProgress() && ( startTime == -1 ) &&
        ((playerState != PLAYING) && (playerState != PAUSED) && 
        (playerState != SUSPENDED) && ( playerState != BUFFERING )))
     {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                    "Ignoring this resume request (not compatible player state)\
                     reconnection in progress");
       return false;
     }

    if(streamer->isReconnectionInProgress() && 
       (playerState == PB_READY))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                   "Ignoring this play request (in pb_ready)\
                   reconnection in progress, this should \
                   never be the case");
      return false;
     }
#ifdef FEATURE_QTV_FCS   
#error code not present
#endif
    {
#ifdef FEATURE_QTV_PIPELINING	
#error code not present
#endif	  
	  // Stream PlayClip
      switch ( playerState )
      {
         case IDLE:
            // previous playback is complete, need to reconnect.
            bDoOpenClip=true;
            break;

         case CONNECTING:
            // waiting on connection.
#ifdef FEATURE_QTV_FCS
#error code not present
#elif FEATURE_QTV_PIPELINING 
#error code not present
#endif

            bDoStartAsap=true;
            break;

         case PB_READY:
            // server is connected & ready to play.
            bDoStartNow=true;
            break;

         case REPOSITIONING:
            /*don't process resume when in the repositioning state*/
            if ( startTime < 0 )
            {
               return (true);
            }

            bDoResume=true;
            break;

         case BUFFERING:
         case PLAYING:
            // If we were already playing, don't do anything.
            if ( startTime < 0 )
            {
               return (true);
            }
            bDoRestart=true;
            break;

         case PAUSED:
         case SUSPENDED:
            bDoResume=true;
            break;

         default:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Weird state in PlayClip for stream");
            return (false);
      }
    }
   }
#ifndef FEATURE_QTV_GENERIC_BCAST
   else if ( clip.bBcastFLO || clip.bBcastISDB )
#else
   else if ( clip.bBcastStream )
#endif
   {
      switch ( playerState )
      {
         case IDLE:
            // previous playback is complete, need to reconnect.
            bDoOpenClip=true;
            break;

         case PB_READY:
            {
#ifdef FEATURE_QTV_GENERIC_BCAST
               /* To enable play from a predetermined point in the TSB */
               if ( startTime > 0 && 
                    clipInfo.info.streamtype == QtvPlayer::MEDIA_BUFFERED_LIVE_STREAMING )
               {
                  /* uTransID is provided by QtvPlayer for every Reposition or
                  ** SeekToSync call made, the ID 0 is unused, so using it here to 
                  ** distinguish that this seek to sync is from Mpeg4Player.
                  */
                  SeekToSyncFromTime(-1, false, 0, startTime);
                  return (true);
               }
#endif /* FEATURE_QTV_GENERIC_BCAST */

               // server is connected & ready to play.
               bDoStartNow=true;
               break;
            }

         case BUFFERING:
         case PLAYING:
#ifdef FEATURE_QTV_GENERIC_BCAST
            // If we are already playing then ignore the play command with 
            // default start time. 
            if ( startTime < 0 )
            {
               // If we are already playing then return true. 
               return (true); 
            }
#endif
            bDoRestart = true;
            break;
#ifdef FEATURE_QTV_GENERIC_BCAST
         case PAUSED:
         case SUSPENDED:
            if ( (startTime == 0) &&
                 (clipInfo.bValid && 
                  clipInfo.info.streamtype == QtvPlayer::MEDIA_BUFFERED_LIVE_STREAMING) )
            {
               /* A live media was paused earlier (implies TSB), and now is being 
               ** resumed from the live end of the buffer (PlayClip(0, -1)).
               ** Set bLiveBcast to true (it was set to false in Pause/Suspend).
               */
               clip.bLiveBcast = true;

               /* Do a restart instead of a resume */
               bDoRestart = true;
            }

            // For file broadcast streams allow resume operation from the 
            // paused/suspended state. 
            if ( !clip.bLiveBcast )
            {
               // For file like streams allow resume. 
               bDoResume=true;
            }
            break;
         case REPOSITIONING:
            /*Allow resume when in the repositioning state*/
            if ( !clip.bLiveBcast )
            {
               if ( startTime < 0 )
               {
                  // Return without doing anything for default resume
                  return (true);
               }

               bDoResume=true;
            }
            break;
	  case OPENING:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: Bcast is OPENING");
            bDoStartAsap=true;
            break;
#endif

         default:
#ifdef FEATURE_QTV_GENERIC_BCAST
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                          "Weird state in PlayClip for Generic Broadcast %s",
                          ConvertGenericBcastSourceToStr(clip.eGenericBcastMediaID));
#else /* FEATURE_QTV_GENERIC_BCAST */
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Weird state in PlayClip for FLO");
#endif /* FEATURE_QTV_GENERIC_BCAST */
            return (false);
      }
   }

#ifdef FEATURE_QTV_PSEUDO_STREAM
   else if ( clip.bPseudoStreaming )
   {
      switch ( playerState )
      {
         case IDLE:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: Pseudo Stream is IDLE");
            bDoOpenClip=true;
            break;

         case OPENING:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: Pseudo Stream is OPENING");
            bDoStartAsap=true;
            break;

         case PB_READY:
            QTV_MSG( QTVDIAG_MP4_PLAYER, "PlayClip: Pseudo Stream is Ready");
            bDoStartNow=true;
            break;

         case REPOSITIONING:
         case PLAYING:
            QTV_MSG( QTVDIAG_MP4_PLAYER, "PlayClip: Pseudo Stream is PLAYING");
            bDoRestart=true;
            break;

         case PAUSED:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: Pseudo Stream is PAUSED");
            bDoResume=true;
            break;

         case PLAYER_PSEUDO_PAUSE:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: Pseudo Stream is PSEUDO PAUSED");
            bDoResume = true;
            Notify(PLAYER_PSEUDO_PAUSE);
            break;

         case SUSPENDED:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: Pseudo Stream is SUSPENDED");
            bDoResume=true;
            break;

         default:
            QTV_MSG(QTVDIAG_MP4_PLAYER,"Play Cmd received in weird state of pseudo stream session");
            return (false);
      }
   }
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
   else if ( clip.bHTTPStreaming )
   {
      switch ( playerState )
      {
         case IDLE:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: HTTP Stream is IDLE");
            bDoOpenClip=true;
            break;

         case OPENING:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: HTTP Stream is OPENING");
            bDoStartAsap=true;
            break;

         case PB_READY:
            QTV_MSG( QTVDIAG_MP4_PLAYER, "PlayClip: HTTP Stream is Ready");
            bDoStartNow=true;
            break;

         case REPOSITIONING:
         case PLAYING:
            QTV_MSG( QTVDIAG_MP4_PLAYER, "PlayClip: HTTP Stream is PLAYING");
            bDoRestart=true;
            break;

         case PAUSED:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: HTTP Stream is PAUSED");
            bDoResume=true;
            break;

         case PLAYER_HTTP_PAUSE:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: HTTP Stream is PSEUDO PAUSED");
            bDoResume = true;
            Notify(PLAYER_HTTP_PAUSE);
            break;

         case SUSPENDED:
            QTV_MSG(QTVDIAG_MP4_PLAYER, "PlayClip: HTTP Stream is SUSPENDED");
            bDoResume=true;
            break;

         default:
            QTV_MSG(QTVDIAG_MP4_PLAYER,
                    "Play Cmd received in weird state of HTTP stream session");
            return (false);
      }
   }
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
   else if ( clip.bQtvHttpStreaming && !clip.bHTTPStreaming )
   {
      switch ( playerState )
      {
         case IDLE:
            // previous playback is complete, need to reconnect.
            bDoOpenClip=true;
            break;

         case CONNECTING:
            // waiting on connection.
            bDoStartAsap=true;
            break;


         default:
            QTV_MSG(QTVDIAG_MP4_PLAYER,
                    "Play Cmd received in weird state of HTTP stream session");
            return (false);
      }
   }
#endif /*FEATURE_QTV_HTTP_DOWNLOAD*/

   else
   {
      // Local file PlayClip
      switch ( playerState )
      {
         case IDLE:
            //previous playback is complete, need to re-open the clip.
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PlayClip: Idle");
            bDoOpenClip=true;
            break;

         case OPENING:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PlayClip: Opening");
            bDoStartAsap=true;
            break;

         case PB_READY:
            //file is parsed and ready to play.
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PlayClip: Ready");
            bDoStartNow=true;
            break;

         case BUFFERING:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PlayClip: Buffering");
            bDoRestart=true;
            break;

         case PLAYING:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PlayClip: Playing");
            bDoRestart=true;
            break;

         case PAUSED:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PlayClip: Paused");
            bDoResume=true;
            break;

         case SUSPENDED:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PlayClip: Suspended");
            bDoResume=true;
            break;

         default:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PlayClip: Unknown State");
            return (false);
      }
   }

   //Take actions.
   long start = 0, stop = 0;
   if ( bDoOpenClip )
   {
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

      //save initial playback times.
      SetPlayTimes(startTime,stopTime,false);

      if ( !GetPlayTimes(start,stop) )
      {
         Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
         return (false);
      }

      //Open clip and start playing.
      appControl.bStartPlaying=true;
      if ( !OpenClip() )
      {
         ErrorAbort(OPEN_CLIP_ERROR);
         return (false);
      }
#ifndef FEATURE_WINCE   
      event_report_payload(EVENT_QTV_CLIP_STARTED,
                           sizeof(qtv_event_clip_started_type),
                           &qtv_event_clip_started_payload);
#endif
      return (true);
   }

   if ( bDoStartAsap )
   {
      //set initial start time.
      //clip is not started, but this might not be the first
      //playclip command.  use previous start time if necessary.
      SetPlayTimes((startTime<0) ? reposControl.nStart : startTime, stopTime, false);

      if ( !GetPlayTimes(start,stop) )
      {
         Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
         return (false);
      }

      if ( appControl.bStartPlaying )
      {
         // If appControl.bStartPlaying = true means that the Mpeg4Player was
         // suspended previously in the CONNECTING state for some reason 
         // (ex. incoming call).
         // When Suspended in the CONNECTING state we stop the streamer, so we 
         // need to OpenClip to get things started off once again.
         if ( !OpenClip() )
         {
            ErrorAbort(OPEN_CLIP_ERROR);
            return (false);
         }
#ifndef FEATURE_WINCE
         event_report_payload(EVENT_QTV_CLIP_STARTED,
                              sizeof(qtv_event_clip_started_type),
                              &qtv_event_clip_started_payload);
#endif
      }
      else
      {
         // Set flag to playback ASAP whenever the player is ready to play
         appControl.bStartPlaying=true;
      }
      return (true);
   }

   if ( bDoStartNow )
   {
      //set initial playback times.
      SetPlayTimes(startTime,stopTime,false);
      if ( !GetPlayTimes(start,stop) )
      {
         Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
         return (false);
      }

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      //Start playing.
      if ( clip.bDownload )
      {
         //pseudo-stream start.
         bool bOK = false;
         if ( playerState == IDLE )
         {
            appControl.bStartPlaying=true;
            if ( !StartPseudoStream() )
            {
               ErrorAbort(START_PSEUDO_STREAM_ERROR);
               return (false);
            }
            else
            {
               bOK=true;            
            }
         }
         else
         {
            if ( StartPlaying() )
            {
               bOK=true;
            }
         }
         return (bOK);
      }
      else
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

         if ( clip.bStreaming && streamer )
      {
         /* Normal case: PlayClip received when player is in PB_READY state. 
         ** Streamer has not been started, so it is not in the paused state.
         */
        if ( !streamer->IsPaused())
         {
          if(acceleratedDuration > -1)
          {
            streamer->setAccDuration (acceleratedDuration);
          }
          if(speed > -1)
          {
            streamer->setSpeed(speed);
          }
 
            //server is connected, start streaming now.
            if ( !StartStreaming(urlToSwitch) )
            {
               ErrorAbort(STREAM_START_ERROR);
            }
#ifdef FEATURE_QTV_STREAM_RECORD
            else if ( bRecPending )
            {
               if ( TRUE == initStreamRecording(recFileName) )
               {
                  /* Disallow repositioning during stream recording */
                  clipInfo.info.RepositioningAllowed =  FALSE;
               }
               else
               {
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                               "initStreamRecording() returned FALSE");

               }
            }
#endif /* FEATURE_QTV_STREAM_RECORD */
         }
         /* Corner case: In the case of streaming, when a PlayClip is received,
         ** StartPlaying is called only after receiving a STREAM_START from the
         ** streamer. In case a Pause/Suspend is issued to the player while
         ** waiting for the STREAM_START, the player pauses the streamer, but 
         ** continues to remain in the PB_READY state. Since the player has
         ** been paused the received STREAM_START is ignored. Hence on resuming 
         ** from this Pause/Suspend, the player should resume the streamer and 
         ** perform all actions it performs on receiving a STREAM_START.
         */
         else
         {
#ifdef FEATURE_QTV_FCS
#error code not present
#endif /* FEATURE_QTV_FCS */
          {
            bool bError = false;
          if (streamer && streamer->Resume(bError))
            {
               if ( bError )
               {
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                               "Stream error from Resume");
                  StreamError();
               }
               else
               {
                  if ( !StartPlaying() )
                  {
                     ErrorAbort(START_PLAYING_ERROR);
                  }
               }
            }
          }
        }
#ifndef FEATURE_WINCE
         event_report_payload(EVENT_QTV_CLIP_STARTED,
                              sizeof(qtv_event_clip_started_type),
                              &qtv_event_clip_started_payload);
#endif

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
         playingBeforeVoiceCall = TRUE;
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

         return (true);
      }
      else
      {
#ifdef FEATURE_QTV_GENERIC_BCAST
         // PB ready to Play Transition. Indicate PlayModePlay to Media source
         if ( clip.bBcastGeneric )
         {
            // By default pcr is de-activated to begin with when open complete processed 
            // Since playmode is set to live here activating it explicitly
            #ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
            #endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
            bool bRet;
            bRet = GENERIC_BCAST_MEDIA_PLAY(clip.pVideoMpeg4Playback,
                                            UID_QTV_MEDIA_PARAM_PLAY_MODE_LIVE);
            if ( !bRet )
            {
               QTV_MSG_PRIO(QTVDIAG_GENERAL, 
                            QTVDIAG_PRIO_ERROR, 
                            "Generic Bcast Media Play failed");
            }
         }
#endif
         //local file or stream start.
         if ( !StartPlaying() )
         {
            ErrorAbort(START_PLAYING_ERROR);
            return (false);
         }

#ifdef FEATURE_QTV_PSEUDO_STREAM
#ifndef FEATURE_WINCE
/* if pseudo streaming, generate EVENT_QTV_PSEUDO_STREAM_STARTED */
         if ( clip.bPseudoStreaming )
            event_report(EVENT_QTV_PSEUDO_STREAM_STARTED);
#endif
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
/* if Progressive Download , generate EVENT_QTV_HTTP_STREAM_STARTED */
         //if(clip.bHTTPStreaming)
         //   event_report(EVENT_QTV_HTTP_STREAM_STARTED);
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

#ifndef FEATURE_WINCE
         event_report_payload(EVENT_QTV_CLIP_STARTED,
                              sizeof(qtv_event_clip_started_type),
                              &qtv_event_clip_started_payload);
#endif

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
         playingBeforeVoiceCall = TRUE;
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

         return (true);
      }
   }

   if ( bDoResume )
   {
#ifdef FEATURE_QTV_GENERIC_BCAST
      /* Generic bcast has the TSB solution, so it is possible to resume, i.e. 
      ** PlayClip(-1, -1). So set startTime to 0 only if it is not Generic bcast.
      */
      if ( !clip.bBcastGeneric )
#endif /* FEATURE_QTV_GENERIC_BCAST */
      {
         if ( qtvConfigObject.GetQTVConfigItem(QtvConfig::QTVCONFIG_TREAT_RESUME_AS_SEEK_FOR_MCAST_OR_LIVESTREAM) 
              && ( is_live_broadcast( clipInfo.info.streamtype ) ||
                   (clipInfo.info.streamtype == QtvPlayer::MEDIA_MCAST_STREAMING) )
              && ((playerState == PAUSED)  || (playerState == SUSPENDED) )
            )
         {
            startTime = 0;
            stopTime = -1;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "SetPlayTimes set to 0,-1");
         }
      }

      //Set resume/restart time.
      SetPlayTimes(startTime,stopTime,true);

      if ( !GetPlayTimes(start,stop) )
      {
         Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
         return (false);
      }

      //if start time is changed, or stream stop time is changed, then
      //we must restart instead of resuming.
      if ( start>=0 || (clip.bStreaming && stop!=reposControl.nStreamStop) )
      {
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
         if ( qtv_cfg_enable_dsp_release )
         {
            if ( clip.bHasVideo )
            {
               if ( qtv_cfg_enable_dsp_release )
               {
#ifndef FEATURE_WINCE
                  if (pActiveVideoPlayer)
                  {
                     pActiveVideoPlayer->RestoreVideo();
                  }
#endif   /*    FEATURE_WINCE   */
               }
            }
         }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
         bDoRestart=true;
      }
      else if ( playerState == SUSPENDED && lastPlayerState == REPOSITIONING )
      {
         /* In the case of streaming the player is in the REPOSITIONING state
         ** until we receive a STREAM_START from the streamer which may take a 
         ** while. After receiving the STREAM_START, we issue StartPlaying in
         ** which the old buffers are flushed and the audio and video players 
         ** move to the new position. If we receive a suspend while waiting 
         ** for STREAM_START, the player moves to the SUSPENDED state. The 
         ** STREAM_START message received subsequently is ignored. On resuming
         ** we need to flush the old buffers and hence need to do a restart 
         ** and not a ResumePlay.
         */
         bDoRestart = true;
      }
      else
      {
#ifdef FEATURE_QTV_GENERIC_BCAST
         // Pause to Play Transition. Indicate PlayModePlay to Media source
         if ( clip.bBcastGeneric && !clip.bLiveBcast )
         {
            bool bRet;
            bRet = GENERIC_BCAST_MEDIA_PLAY( clip.pVideoMpeg4Playback, UID_QTV_MEDIA_PARAM_PLAY_MODE_PLAY );
            if ( !bRet )
            {
               QTV_MSG_PRIO(QTVDIAG_GENERAL, 
                            QTVDIAG_PRIO_ERROR, 
                            "Generic Bcast Media Play failed");
            }
         }
#endif
         //resume from pause.
         if ( !ResumePlay() )
         {
            ErrorAbort(RESUME_PLAY_ERROR);
            return (false);
         }

         qtv_event_clip_pause_resume_type qtv_event_clip_pause_resume_payload;
         qtv_event_clip_pause_resume_payload.resume_time = start;
#ifndef FEATURE_WINCE
         event_report_payload(EVENT_QTV_CLIP_PAUSE_RESUME,
                              sizeof(qtv_event_clip_pause_resume_type),
                              &qtv_event_clip_pause_resume_payload   );
#endif

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
         playingBeforeVoiceCall = TRUE;
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

         return (true);
      }
   }

   if ( bDoRestart )
   {
      if ( clipInfo.info.RepositioningAllowed && !bLoopTrackFlag ||
           ( clip.pVideoMpeg4Playback &&
             is_live_broadcast( clipInfo.info.streamtype ) &&
             start == 0) )
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Replaying Same Clip");

#ifdef FEATURE_QTV_GENERIC_BCAST
		//Start the Media Mode to Play or Live based on the mode
		if ( clip.bBcastGeneric && !clip.bLiveBcast )
		{
			bool bRet;
			bRet = GENERIC_BCAST_MEDIA_PLAY( clip.pVideoMpeg4Playback, UID_QTV_MEDIA_PARAM_PLAY_MODE_PLAY );
			if ( !bRet )
			{
				QTV_MSG_PRIO(QTVDIAG_GENERAL, 
							 QTVDIAG_PRIO_ERROR, 
							 "Generic Bcast Media Play failed");
			}
		}
#endif

      }
      else
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Repositioning NOT ALLOWED" );
         Notify(QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED);
         return (false);
      }
#ifndef FEATURE_QTV_GENERIC_BCAST
      if ( clip.bBcastFLO || clip.bBcastISDB )
      {
         return (false);
      }
#else
      if ( clip.bBcastGeneric && clip.bLiveBcast )
      {
        // Since playmode is set to live here activating it explicitly
        #ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
        #endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
         bool bRet;
         bRet = GENERIC_BCAST_MEDIA_PLAY(clip.pVideoMpeg4Playback,
                                         UID_QTV_MEDIA_PARAM_PLAY_MODE_LIVE);
         if ( !bRet )
         {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                         "Generic Bcast Media Play failed");
         }
      }
#endif /* FEATURE_QTV_GENERIC_BCAST */

      //Cannot restart a bitstream
      if ( clip.bM4vFile )
      {
         return (false);
      }

      qtv_event_clip_repositioning_type qtv_event_clip_repositioning_payload;
      qtv_event_clip_repositioning_payload.current_time = 0;
      //get elapsed time.
      unsigned long temp_var=0;
      if (clip.bHasVideo
#ifndef FEATURE_WINCE
          && pActiveVideoPlayer
#endif   /*    FEATURE_WINCE   */
)
      {
#ifndef FEATURE_WINCE
         temp_var=(unsigned long) pActiveVideoPlayer->GetElapsedTime();
#endif   /*    FEATURE_WINCE   */
         qtv_event_clip_repositioning_payload.current_time =
         ZMAX(qtv_event_clip_repositioning_payload.current_time,
              temp_var);
      }

      if ( clip.bHasAudio )
      {
#ifndef FEATURE_WINCE
         temp_var=(unsigned long)AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
         qtv_event_clip_repositioning_payload.current_time =
         ZMAX(qtv_event_clip_repositioning_payload.current_time,
              temp_var);
      }
#ifndef FEATURE_WINCE
      event_report_payload(EVENT_QTV_CLIP_REPOSITIONING,
                           sizeof(qtv_event_clip_repositioning_type),
                           &qtv_event_clip_repositioning_payload);
#endif   /*    FEATURE_WINCE   */

      //Set restart time.
#ifdef FEATURE_QTV_PSEUDO_STREAM
      if ( clip.bPseudoStreaming )
      {
         if ( startTime > (int)qtv_event_clip_repositioning_payload.current_time )
         {/* This is a FF operation */
            if ( PLAYER_PSEUDO_PAUSE == playerState )
            { /* FF, when pseudo stream is in pseudo pause state */
               QTV_MSG( QTVDIAG_MP4_PLAYER,
                        "PlayClip: FF, when Pseudo Stream is in pseudo pause state."
                        "Remain in state PLAYER_PSEUDO_PAUSE");
               Notify(PLAYER_PSEUDO_PAUSE);
               return (false);
            }
         }
      }
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      if ( clip.bHTTPStreaming )
      {
         if ( (unsigned long ) startTime > qtv_event_clip_repositioning_payload.current_time )
         {/* This is a FF operation */
            if ( PLAYER_HTTP_PAUSE == playerState )
            { /* FF, when HTTP stream is in pseudo pause state */
#ifndef FEATURE_WINCE  
               QTV_MSG( QTVDIAG_MP4_PLAYER,
                        "PlayClip: FF, when HTTP Stream is in pseudo pause state."
                        "Remain in state PLAYER_HTTP_PAUSE");
#endif						
               Notify(PLAYER_HTTP_PAUSE);
               return (false);
            }
         }
      }
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
      SetPlayTimes(startTime,stopTime,true);

      if ( !GetPlayTimes(start,stop) )
      {
         Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
         return (false);
      }

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
      playingBeforeVoiceCall = TRUE;
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

      //Restart active clip.
      if ( !clip.bStreaming &&
#ifndef FEATURE_QTV_GENERIC_BCAST
           !clip.bBcastISDB &&
           !clip.bBcastFLO &&
#else
           !clip.bBcastStream &&
#endif
           start<0 &&
           (playerState==PLAYING || playerState==BUFFERING) )
      {
         //Special case for local file or pseudo-stream stop time update,
         //there's no real need to restart.
         UpdateStopTime(stop);
         return (true);
      }

      //Normal restart.
      if ( !RestartActiveClip() )
      {
         if ( IsDone() )
         {
            /* Player has already done playing while processing reposition,
            we can not restart in this condition, let the player be in IDLE state */

            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::Player has done playing while procesing reposition");    
         }
         else
         {
            ErrorAbort(RESTART_ACTIVE_CLIP_ERROR);
         }
         return (false);
      }
      return (true);
   }

   return (false);
}

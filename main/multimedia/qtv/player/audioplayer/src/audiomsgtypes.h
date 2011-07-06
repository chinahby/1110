#ifndef _AudioMsgTypes_H_
#define _AudioMsgTypes_H_
/* =======================================================================

                          audioMsgTypes.h

DESCRIPTION
  Definition of message types which are used within the audio subsystem. 
  AudioPlayerIf interface constructs one of these messages in order to 
  communicate with a specific AudioPlayer. The client of the audio subsystem
  is unaware of these messages. Its for internal purpose only..

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/audiomsgtypes.h#8 $
$DateTime: 2009/05/12 14:25:55 $
$Change: 911031 $

========================================================================== */

class AudioMsgTypes
{
public:
    typedef enum audioMsgTypesT
    {
      AUDIO_PLAYER_STOP = 0,
      AUDIO_PLAYER_SUSPEND = 1,
      AUDIO_PLAYER_START = 2,
      AUDIO_PLAYER_PAUSE = 3,
      AUDIO_PLAYER_RESUME = 4,
      AUDIO_PLAYER_RESTART_AUDIO_OUTPUT = 5,
      AUDIO_PLAYER_PREP = 6,
      AUDIO_PLAYER_PREP_AND_RESTART_AUDIO_OUTPUT = 7,
      AUDIO_PLAYER_ENABLE_TIME_CHECK = 8,
      AUDIO_PLAYER_GET_STATS = 9,
      AUDIO_PLAYER_GET_ISIDLE = 10,
      AUDIO_PLAYER_GET_ISPAUSED = 11,
      AUDIO_PLAYER_GET_TRACK_ENDED = 12,
      AUDIO_PLAYER_SET_PLAY_TIME = 13,
      AUDIO_PLAYER_SELF_DISPATCH = 14,
      AUDIO_PLAYER_GET_ISPLAYING = 15,
      AUDIO_PLAYER_DRAIN_AUDIO_BUFFER = 16,
      AUDIO_PLAYER_DISABLE = 17,
      AUDIO_PLAYER_SET_AUDIO_CHANNELSWITCH_TS = 18,
      AUDIO_PLAYER_SHOW_INFO = 19,
      AUDIO_PLAYER_GET_ISPAUSED_DISABLED = 20,
      AUDIO_PLAYER_PCR_RESET = 21,
      AUDIO_PLAYER_CMX_FEEDBACK = 22,
      AUDIO_PLAYER_CLOCK_ADJUST = 23
      #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
      #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
    } audioMsgTypes;

    /* Generic audio msg header used to identify a audio msg.
    ** All the audio messages will have this as its first member
    ** variable.
    */
    typedef struct audio_msg_hdr_tag
    {
      //Will be set of one of the audioMsgTypes.
      uint16 msgId;
    } audio_msg_hdr;

    typedef struct audio_cmd_start_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool bRestart;
      bool* pbStart;
    } audio_cmd_start_msg;

    typedef struct audio_cmd_pause_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool* pbPause;
    } audio_cmd_pause_msg;

    typedef struct audio_cmd_resume_msg_tag
    {
      audio_msg_hdr msgHdr;
      long stop;
      bool* pbResume;
    } audio_cmd_resume_msg;
        
    typedef struct audio_cmd_suspend_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool* pbSuspend;
    } audio_cmd_suspend_msg;
    
    typedef struct audio_cmd_stop_msg_tag
    {
      audio_msg_hdr msgHdr;
    } audio_cmd_stop_msg;

    typedef struct audio_prep_msg_tag
    {
      audio_msg_hdr msgHdr;
      int playbackID;
      bool bRestart;
      void* pMedia;
      bool bHasVideo;
      bool* pbPrep;
      Media::CodecType audioCodec;
    } audio_prep_msg;

    typedef struct audio_cmd_restart_output_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool* pbRestartAudioOutput;
    } audio_cmd_restart_output_msg;

    typedef struct audio_cmd_prep_and_restart_output_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool* pbPrepRestartAudioOutput;
    } audio_cmd_prep_and_restart_output_msg;

    typedef struct audio_enable_time_check_msg_tag
    {
      audio_msg_hdr msgHdr;
      unsigned long timeValue;
    } audio_enable_time_check_msg;

    typedef struct audio_get_stats_msg_tag
    {
      audio_msg_hdr msgHdr;
      QtvPlayer::AudioVideoStatisticsT *pStats;
      Common::LastFrame *pLastFrame;
    } audio_get_stats_msg;

    typedef struct audio_get_isidle_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool *pIsIdle;
    } audio_get_isidle_msg;

    typedef struct audio_get_ispaused_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool *pIsPaused;
    } audio_get_ispaused_msg;

    typedef struct audio_get_ispaused_disabled_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool *pIsPausedDisabled;
    } audio_get_ispaused_disabled_msg;

    typedef struct audio_get_isplaying_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool *pIsPlaying;
    } audio_get_isplaying_msg;
    
    #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
    #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

    typedef struct audio_get_track_ended_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool *pIsTrackEnded;
    } audio_get_track_ended_msg;

    typedef struct audio_set_play_times_msg_tag
    {
      audio_msg_hdr msgHdr;
      long start;
      long stop;
    } audio_set_play_times_msg;

    typedef struct audio_cmd_self_dispatch_msg_tag
    {
      audio_msg_hdr msgHdr;
      uint32 delay_ms;
    } audio_cmd_self_dispatch_msg;

    typedef struct audio_cmd_drain_audio_buffer_msg_tag
    {
      audio_msg_hdr msgHdr;
      uint32 timestamp;
    } audio_cmd_drain_audio_buffer_msg;

    typedef struct audio_cmd_disable_msg_tag
    {
      audio_msg_hdr msgHdr;
      bool* pbDisable;
    } audio_cmd_disable_msg;

    typedef struct audio_cmd_set_audio_channelswitch_msg_tag
    {
      audio_msg_hdr msgHdr;
      uint32 reqData;
    } audio_cmd_set_audio_channelswitch_msg;

    typedef struct audio_cmd_show_info_msg_tag
    {
      audio_msg_hdr msgHdr;
    } audio_cmd_show_info_msg;

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

    typedef struct audio_clock_adjust_msg_tag
    {
      audio_msg_hdr msgHdr;
    } audio_clock_adjust_msg;

};

#endif //_AudioMsgTypes_H_ 


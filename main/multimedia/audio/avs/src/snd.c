/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Sound Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     S O U N D   S E R V I C E S

GENERAL DESCRIPTION
  This task controls generation of sound heard by the user, including both
  transmitted sounds and tone generation.

EXTERNALIZED FUNCTIONS
  snd_task
    This is the entrance procedure for the Sound Task.  It contains the
    main processing loop for the task.
  snd_phone_awake
    Signals sound task that phone is awake.
  snd_register_aux_ctl
    This function determines how Sound will activate the auxiliary generator
  snd_set_device
    Connects a specific audio device. Also controls muting.
  snd_set_volume
    Set the volume for a specific device and method.
  snd_get_volume
    Get the volume for a specific device and method.
  snd_get_levels
    get the number of volume levels for a specific device and method.
  snd_comp_ctl
    Compander control for IS-55 tests.  Turn the compressor/expander off/on.
  snd_rxfm_mute_ctl
    Temporary mute/unmute of the audio input from the FM receive path.
  snd_txfm_mute_ctl
    Temporary mute/unmute of the audio output to the FM transmit path.
  snd_dtmf_tx_mute_ctl
    Temporary mute/unmute of the DTMF output to the FM transmit path.
  snd_stereo_mute_ctl
    Sets stereo mute on HFK device.
  snd_ext_connect
    Enables/disables external communications mode.
  snd_ext_protocol
    Sets the protocol mode being used by IDS.
  snd_filters
    Selects filter for voice or for data calls.
  snd_tone_start
    Start playing a tone using the device, method, and duration specified.
  snd_freq_tone_start
    Start playing a tone with a specified pair of frequencies.
  snd_sample_tone_start
    Start playing a tone using device, method, duration and volume specified.
  snd_tone_stop
    Stop playing the current tone.
  snd_sound_id_start
    Start playing a sound from the database using device and method specified.
  snd_sample_sound_id_start
    Start playing a sound from the database with a specified volume.
  snd_sound_start
    Start playing a sound using a list of tones specified.
  snd_sample_sound_start
    Start playing a sound using a list of tones specified at a specific volume.
  snd_sound_stop
    Stop playing the sound determined by the priority.
  snd_vp_start
    Start playing a voice prompt at a particular device.
  snd_vp_stop
    Stop playing voice prompt
  snd_vp_pause
    Pauses voice prompt playback.
  snd_vp_resume
    Resumes voice prompt playback.
  snd_mm_rec_new_buffer
    Receives new buffer from CMX for multimedia recording 
  snd_mm_record
    Starts a MulteMedia recording session.
  snd_mm_record_stop 
    Ends a MultiMedia recording session.
  snd_mm_record_pause
    Pauses a MultiMedia recording session.
  snd_mm_record_resume
    Resumes a MultiMedia recording session.
  snd_mm_rec_status
    This function sends the indicated status back to the client.
  snd_sidetone_ctl
    This function queues a sidetone control command packet to Sound Task.
  snd_voc_adec_start
    This function starts QCP or AMR decoder.
  snd_aux_pcm_clk_enable
    This function starts external AuxPCM clock.
  snd_aux_pcm_clk_disable
    This function stop external AuxPCM clock.
  snd_set_aux_line_in
    This function set or reset the Aux PGA Line In loop back path.
  snd_is_task_up
    This function returns the snd task init status.
INITIALIZATION AND SEQUENCING REQUIREMENTS
  snd_task() must be started up before any other sound function calls
  will be processed.  Snd_task() must have been started in order to
  initialize the hardware state sufficiently for snd_*_mute_ctl().

  Copyright(c) 1992 - 2009 by QUALCOMM, Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/snd.c#37 $ $DateTime: 2009/11/20 04:39:57 $ $Author: avangala $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/20/09    av     Fixed error shown on ADS120 compiler
11/20/09  vsud     Fixed compiler warning. 
11/16/09   kkm     Added the function snd_clr_bt_cb_func to deregister the
                   callback function registered bt BT for informing the play type.
11/11/09   knm     Added new sound devices for Bi-I2S
10/06/09    sj     Added debug messages in APIs snd_set_device() and
                   snd_set_volume().
09/23/09    kkm    sndhw_audfmt_cleanup is added in snd_expire to clean up for
                   voice prompt playback.
09/07/09   vsud    Changes for resampler image imeplementation.
07/31/09   bk      Fix for a crash due to NULL pinter access.
07/07/09   rk      Notifying SND that PAs are turned on, wakes up sound task.
05/27/09    kk     Fixed CR 182954 - CMX_ERROR occurs when record video 
                   clips after cmx_audfmt_get_specs. 
05/25/09    vm     Added check to ensure the sound file is in pause state
                   for resume to be called.
05/23/09   sud     Enabled more debug messages.
04/20/09    rm     CR 168030, handling voc_state_get_high_module
04/06/09   sud     Modified code to not zero out volume for SND_TYPE_SILENCE.
                   CR 176951.
		   Updated Copyright.
03/26/09    sj     Added debug messages in snd_set_device() and 
                   snd_set_volume() functions
01/19/09   anb     Added snd_set_qtv_cb_func so that QTV can be informed about
                   mod/image swap event.Fixes Cr 163807(SR-1133583)
03/10/09   sj      Added slowtalk feature support
01/19/09   bk      Featurized properly to fix compilation errors arised
                   after trying to remove FEATURE_WEBAUDIO from the build.
01/19/09   dp      CR 168251 Removed feturization for FEATURE_BT_QSOC
12/26/08   rm      Added RVE API changes
12/10/08   knm     Added snd_is_task_up() API to check init status of sound task.
11/26/08   skr     Fixed compiler warning
11/24/08   skr     Added code for Audio + camera Concurrency
11/18/08   dp      Featurization fix for lowtier build
10/22/08   dp      FM over BT AG changes
08/24/08   skr     Moved the resetting of Qensemble interrupt flags to 
                   sndhw_audfmt_cleanup().
09/15/08   knm     Added support for Aux Line In(FM) volume control
08/26/08   anb     Added support for DSP based WMAPRO
07/16/08   skr     Reset qensemble interrupt request flags after snd_expire.
07/11/08   skr     Changes to release VOCODER & ADEC separately during Incall.
06/20/08   knm     Created Sound API to set or reset the AuxPGA Line in
                   path configuration.
03/11/08    kk     Added support for sending mp3 configuration 
                   information for DSP. 
02/20/08    hs     Adding audmain_reset_parms before get time and get spec
02/07/08   mas     Initializing ATP protocol from snd_init()
                   Updated Copyright
02/07/08    kk     Added support for Qensemble phase 2 interface changes.
01/30/08    hs     Fixed Klockworks warnings
12/19/07    hs     Added support ISDB-T dual mono output control
12/08/07    sr     buffer_underrun_flag moved to sndi.h . Fix for CR-127448
11/05/07    sk     Set audmain_set_init_done in snd_setup_audfmt() when the file playback is 
                   wave CR 120253,124336,124337,120853
11/28/07   wfd     Test that snd_bt_cb_func_ptr isn't null before dereferencing, now that 
                   this pointer is deregistered when BT device is not used.
11/14/07    hs     Added QEnsemble support
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
08/06/07  anb/wfd  Added aux_pcm_clk to be enabled/disabled from other tasks like BT (CR 122106)
07/27/07    pl     Merge in fix for CR  124279 - A SMAF file playback with 
                   cmx_audfmt_play()/cmx_audfmt_play_multi() is failed
07/10/07    st     Modified code to repeat processing in snd_age_adpcm()
                   if a new data buffer is already available.
07/10/07    kk     Fixed CR 118785 - CMX: AbError observed during Rapid key Press 
                   of FF Key is done during play back of AMR and QCLEP clips.
05/28/07    pl     Modified to use new SLEEP API
04/07/07    pl     Addeded support for starting qsynth AMR-NB
                   decoder.
03/02/07    at     Added AUDMAIN_AMR_WB_AUDIO_TYPE case to various functions
                   for 3GP AWB file support.
02/13/07    ay     Fixed CR#96487, AMR file loops continuously.
01/19/07    sp     Fixed a BT AV Sync issues which causes video to be frozen
                   when the device switches from A2DP to wired HS and back to
                   A2DP.
12/04/06   suk     Fixed 6260 ThinUI build compilation error.
11/28/06    sp     Cleaned up AV Sync related APIs.
                   Added support for OEMSBL Audio.
11/21/06    at     Fixed CR 103188.
11/17/06    sk     Merged Critical Lintwarning issues fixed by Andy
11/09/06    az     Merged the code for MM API changes.
10/20/06    pl     Put a check for NULL data pointer in snd_pcm_rec_status().
10/18/06   suk     Corrected the Wrong Featurization.
10/11/06    az     Fixed CR: 101988 elapsed time error in MP3.
09/08/06    lg     Fixed a bug in snd_audfadec_int for PMD playback
09/05/06    ay     Added AMR-WB+ support.
08/29/06    hz     Added support for incall MP3 on 65nm platforms.
08/01/06    hz     Fixed a mis-merging in snd_expire().
08/01/06    pl     Added SRC_ENABLED case in snd_3d_pos_query_src_req(). 
07/31/06    sp     Fixed some merge issues in AV Sync feature and
                   some issues in SCO rework feature 
07/28/06    hz     Added support for ID3V2 and AAC ADTS/VBR song length.
07/27/06   anb     Added the FEATURE_AVS_BT_SCO_REWORK which enables the AVS to send
                   messages to the BT driver
07/26/06    sp     Added support for A2DP AV Sync
07/25/06   sk/lg   Fixed Pan settings for PMD  has AAC content.(CR 98041)
06/27/06    pl     Added environment & source 3D enable/disable and reset support.
06/26/06    pl     Added support for doppler, reverb and ping-pong buffer
                   data writing scheme.
06/07/06    pl     Modified code to set hybrid mode for PCM playback thru MIDI_OUT 
                   interface.
05/31/06    sg     Fixed featurization so that Animated AAC can be disabled.
04/14/06    lg     Modified code to check if AAC image is configured for 
                   Animated AAC Ringers. (CR#89914)
03/27/06    ak     Added support for sidetone control. Functions 
                   snd_sidetone_ctl and snd_sidetone_ctl_req queue the command
                   and sets the sidetone control, respectively.
03/16/06   anb     Modified to support sbc re-configuration given the encoded params
                   CR 78581,87334 
03/13/06    st     Added support for delayed start for playback of audio
                   decoder files to MIDI device; added support for repeat
                   playback mode for audio decoder files to MIDI device;
                   added support for 3D positional final source parameter
                   settings query command.
01/31/06    hz     Added support for in-call mp3 feature.
01/30/06    sp     Added support for I2S SBC interface.
01/30/06    pl     Added support to provide a way to select hybrid mode 
                   for multisequencer mode and MIDI device.
01/13/06   act     Fixed compile warning with low-tier featureset.
11/23/05    lg     Modified code to write adec data to DSP even when there is 
                   no interrupt from DSP to avoid race conditions, CR78752
11/18/05    st     Modified code to handle decreasing return values from
                   clk_uptime_ms(), CR72249.
11/16/05    sp     Changes to support SBC encoding in lower priority task.
11/14/05    hz     Clarified compilation warning.
11/04/05    sp     Added a check for NULL pointer in snd_sound_start_req.
10/19/05    pl     Added PCM AVSync support.
09/27/05    sp     Fixed a compilation error that occurs on UI builds.
09/18/05    sp     Added support for DSP based SBC encoder.
09/18/05    st     Added support for QAudioFX: 3D positional commands and query
                   interface; added snd_adec_qcp_start(); modified code to 
                   NULL callback pointer for MIDI_OUT_OPEN and MIDI_OUT_CLOSE
                   commands so a callback is not sent back twice.
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is 
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
09/12/05    hz     Added AV sync support for windows media.
08/31/05    sm     Added VOX support.
08/29/05    sp     Fixed bugs resulting in a streaming file not being played 
                   on BT A2DP headset after fw/rewind, added support for 
                   changing SBC parameters for testing purposes.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
08/10/05    sm     Added pause support for A/Ulaw wav files.
07/22/05    sm     Added Wav A/ULaw support.
07/19/05    pl     Added support for relaying the categoried abort status back 
                   to the upper layer and fixed snd_age_qcp() not to report abort
                   error for normal termination of qcp playback. 
07/13/05    sp     Added code so that audiosbc_audio_flush gets called for both
                   SND_DEVICE_BT_A2DP_HEADSET and SND_DEVICE_BT_A2DP_SCO_HEADSET
06/15/05    pl     Modified snd_audfadec_int to check if the pause mask is 
                   being set when audio decoder type is AAC.
06/03/05    st     Modified code to support simultaneous AUX generator
                   and sound; modified code to support SND_CMX_AF_TIME_TICK
                   for PCM recording.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/24/05    hz     Added support for MultiMedia/AAC record functionality.
05/18/05    ac     Modified code to support AV sync functionality with MP3,
                   AAC, and QCP playback.
05/17/05    st     CR52408: Modified to not send TIME_TICK callbacks if there
                   is no data available for playback (QCP, MP3, AAC, RA, WMA).
04/28/05   ymc     Added 4GV NB support.
04/25/05    lg     Modified age fns to send SND_ABORT callback when xxx_parse  
                   returns AUDMAIN_STATUS_ERROR, except audfmt_parse.
04/22/05    hz     Removed a junk character during checked in.
04/22/05    aw     Modified code to fix Coverity Error. Modified code to not
                   return SND_SUCCESS twice on AudioPP functions.
04/21/05    sg     Added missed PCM sample report for Windows Media playback.
04/21/05    hz     Added VirtualPacketLength support for Windows Media 
                   playback.
03/25/05   ac/sg   Added support for A/V synchronization.
03/02/05    aw     Added support for Qconcert, API change for configuring
                   EQ bands.
02/14/05    hz     Added support for WindowsMedia Audio decode;
                   Modified snd_age_adec_stream() to accomodate Windows
                   Media Audio decoder too.
02/06/05    st     Modified code in snd_sound_start_req() to only expire
                   an active sound_ptr.
02/05/05    st     Modified code to support various playback features starting
                   from an offset into file; modified code to support HQ DLS;
                   added support for DLS file processing.
12/01/04    aw     Modified code to return SND_CMX_QCP_REC_ERROR if the rec 
                   failed.
11/19/04    sp     Added support for Arm Post Processing.
10/14/04    aw     Modified code to update the elapsed time when playback is
                   paused.
09/22/04    aw     Modified code to update the QCP elapsed time when it is 
                   paused.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
            sm     Only set tone mute when voice is unmuted.
08/11/04    st     Modified to calculate the next TIME_TICK duration correctly
                   for MIDI and MP3; modified code to add command to verify
                   reset status of MIDI; removed code to reset command buffer
                   after sndhw_audfmt_cmd(), this is now handled in lower layer.
06/17/04    aw     Modified code to do the checking in snd content. 
                   Modified code to buffer up the client equalizer table.
06/08/04    aw     Added support for Equalizer and Spectrum analyzer.
05/19/04    lg     Modified code to reset snd_array's audfmt_buf in snd_expire.
05/11/04    sg     Fixed to return a no-change indication for tempo commands
                   while playing QCP files.
04/27/04    lg     Modified code to fix Animated-AAC data not playing if vocoder
                   is in sleep state.
04/22/04    lg     Added support for Animated AAC Ringers.
04/16/04    st     Modified code to return tune and tempo status to client.
03/22/04    st     Modified code to not mute AUDMAIN_MIDI_TYPE, pause/mute
                   processing is done by the audfmt layer; updated
                   snd_audfmt_pause_seq() and snd_audfmt_set_vol_seq to return
                   proper status; added support for PCM playback and recording;
                   updated get_spec command processing; added
                   snd_audfmt_adec_chan_flush() function.
                   Redesigned MIDI_OUT_QCP/MIDI pause/resume mechanism to be
                   independent of one another; updated snd_expire() to unpause
                   ADEC properly and issue any final MIDI commands; updated
                   snd_get_midi_out_enabled_flag(); updated code to use new
                   audmain_play_cb() function; added snd_audfmt_pause_seq(),
                   snd_audfmt_set_vol_seq(); modified snd_age_adpcm() to run
                   even when paused in case MIDI_OUT_QCP channels are active;
                   minor multisequencer, general code and featurization updates.
03/04/04    sg     Modified code to reset calc time in get time abort;
02/27/04    sg     Fixed feature-ization in snd_get_spec_req().
01/26/04    lg     Modified code to support tune, tempo & pan adjustments and
                   volume, pause & stop of each sequence in multi_sequencer
01/26/04    sg     Restored old behavior for tune, pan and tempo - for formats
                   that don't support it, the clients will get a success
                   callback indicating no change.
            aw     Added support to get file specs.
01/22/04    lg     Added support for get_time functionality for IMY,QCP,ADPCM & PCM
01/21/04    aw     Added support for qsynth volume mapping.
12/15/03    aw     Modified code to expire the current sound only once CR#37956.
12/08/03    sg     Modified to check for NULL pointer before attempting to
                   expire sound in snd_sound_start_req().
                   Modified to not clear snd_af_ff_ctl if ff/rewind is pending
                   in snd_audfmt_do_seek().
12/02/03    sg     Added support for linear PCM playback.
11/24/02    aw     Modified code to support 32Khz sample rate for qsynth
                   hybrid mode. Modified code to do the callback for midi out 
                   after QSYNTH image is acquired.
11/22/03    sg     Added support for RealAudio codec.
10/16/03  aw/st    Modified code to support YADPCM and audfmt multi-sequences.
10/10/03    sg     Modified to support additional AAC params reqd by the DSP.
09/25/03    sm     Added FEATURE_LOCAL_DTMF_TX_VOICE so TX voice is unmuted
                   during local tone playback.
05/28/03    st     Updated reference in ERR_FATAL().
05/06/03    sg     Modified snd_age_mpeg() so that data is not transferred to
                   the adec if the sound is currently on hold.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface.
03/10/03    st     Modified code to support pause/resume for ADPCM.
02/28/03    st     Modified code to support setting EC and NS modes.
02/19/03  aw/st    Added support for playback of ADPCM file format.
01/25/03    aw     Modified code to add proper featurization.
01/05/03    sg     Modified to initialize the adec data strutures in
                   snd_setup_tone() when iMelody used MIDI.
11/19/02    st     Added snd_audfmt_synth_mvol() and snd_audfmt_adec_mvol()
                   functions.
                   Modified code to return FA_DELAY status in snd_age_qcp().
11/18/02    st     Fixed iMelody featurization.
11/06/02    sm     Added support for QCP AMR.
11/05/02    sg     Added support for iMelody format.
10/28/02  aw/st    Modified code to not issue resume callback when is doing
                   fforward or rewind; Modified code to service adec interrupt
                   faster.
09/10/02   ymc     Changed FEATURE_MP3 into FEATURE_QCP for AUDMAIN_QCP_TYPE.
09/06/02    st     Added support for multimode architecture.
08/13/02  aw/st    Modified code to support ADPCM audio decoder.
          sg/st    Modified so that the adec buffers are not flushed when
                   fforward/rewind requests fail. Playing is resumed from
                   where it was paused.
                   Modified so that the AAC/MP3 client is informed of data
                   underflow when there is insufficient data in the
                   ADEC buffers.
                   Added support for MP4 and AAC.
                   Added support for flush command.
                   Added support for sounds where format is specified by client.
07/11/02    yl     Added the code to handle the VOC_REC_STAT_AUTO_STOP. 
                   Changed the parameters of snd_qcp_record_req(), 
                   snd_qcp_record().
06/11/02    st     Modified code to kick watchdog in snd_audfmt_wait(); added
                   code to reset audmain module at the end of a
                   snd_audfmt_do_time().
03/12/02    st     Modified code to mute Tx DTMFs when sounds are finished.
03/04/02    st     Modified code to account for seeking past a repeat timer
                   for audio format playback.
12/12/01    aw     Merged with MSM_AVS.01.05.17.
11/10/01    st     Modified code to only send buffer underflow status once
                   until the status is cleared.
10/08/01    st     Modified code to call MIDI/WebAudio callback with pause/
                   resume status for MIDI device pause/resume commands;
                   modified code to call snd_midi_out_open() callback for
                   MIDI device functions; added snd_midi_out_qcp_vol_req() and
                   snd_midi_out_qcp_pan_req() functions; modified code to
                   check pause flag in fast-forward and rewind before unpausing
                   synth.
10/01/01    st     Modified code to support multiple ADEC channels for MIDI
                   device; added support for pause, resume, volume and panning
                   commands for the MIDI device audio decoder; modified
                   snd_age_audfmt() to set the init done flag even when
                   a command buffer error is returned from audmain_parse().
09/19/01    st     Modified code to abort QCP playback if not able to enqueue
                   prompt to the vocoder.
09/19/01    st     Modified snd_midi_out_msg_req() to add range checking for
                   MIDI commands, CR18372.
09/16/01    st     Added support for setting cnfg chunk in a QCP file.     
09/03/01  st/aw    Added support for QCP recording and support functions 
                   (set/get information); modified code to always set muting
                   in snd_txfm_mute_ctl(); modified code to support ff/rewind
                   functions for QCP playback; modified code to use audfadec
                   module for QSynth audio decoder functions; modified code to
                   not do MIDI audio decoder processing if repeat timer is
                   active.
06/18/01   jkl     Added FEATURE_ACP.
05/09/01    st     Modified code to set the proper change_mask value in
                   snd_sound_stop_req().
04/26/01    st     Modified code to use new sndhw_audfmt_adec_data()
                   interface, it now indicates the type of audio decoder data;
                   modified code to use repeat count in SND_RPT_NOCALLBACK
                   and SND_RPT control tones; modified code in snd_expire()
                   to clear pause flag on audio decoder.
04/26/01    sm     Removed mp3_speed_factor
04/04/01    st     Removed unnecessary defines; modified code to only do
                   snd_audfmt_adec_int_req() after init has been done.
                   Modified code to set init done flag in snd_age_audfmt() 
                   after first non-CMD_BUFF_ERR preparse, this will enable 
                   the sound to be aged properly.
                   Modified code to queue command for VP signals instead of
                   setting REX signal; added support for direct MIDI device
                   output interface; modified code to send an all notes off
                   command when expiring a MIDI sound; redesigned QCP playback
                   code; redesigned stereo panning code; updated code to use
                   any interfaces which have been modified; lint updates.
02/28/01    st     Modified code in snd_change() such that the snd_tone_timer
                   is always set to a minimum of 5 milliseconds for each
                   iteration; added support for preparsing of audio format
                   files; modified parameter in and_audfmt_adec_int() to be
                   a void pointer type; lint updates.
12/10/00    st     Added snd_setup_audfmt() to be called from 
                   snd_sound_start_req() snd snd_sound_id_start_req().
12/06/00    st     Check audmain_get_repeat() when aging to see if we're in
                   the repeat silence state.
12/04/00  st/sm    Setup MP3 tone with song config.  snd_audfmt_cmd() returns
                   snd_status_type.  Added adec_chan_vol(), adec_chan_pan().
                   Load song config with audfmt song.  Added snd_sound_pan().
          st/sm    Merged audmain support.  Added FEATURE_QCP, MIDI, ADEC.
                   Removed reset type from SND_CMX_AF_RESET calls.  Store
                   stereo DAC sampling rate in sound.  Moved many MIDI calls
                   into audfmt.  Added SND_CMX_GET_FILE_LEN support.
            st     Modified code to not age audio format sounds until the sound 
                   is no longer on hold.
09/25/00    sm     Add call to sndhw_bt_ag_init().
09/05/00    st     Added support for remote sounds.
08/07/00    st     Remerged change from 07/27.
08/07/00    st     Modified code in snd_audfmt_wait() to verify if signal has
                   been set before doing a rex_timed_wait().
07/27/00   spf     Increased SND_NUM_PACKETS to 30 to accomodate new TTY
                   volume setttings.
07/25/00    st     Modified code to support a starting position indicator for
                   audio format sounds.
07/22/00    st     Modified user-controlled tuning mechanism such that command
                   is now issued through audfmt module.
07/15/00    st     Added snd_audfmt_wait() and snd_audfmt_continue() functions
                   to be used by audfmt module to wait for data requests;
                   updated SND_CMX_AF_RESET audio format callbacks with 
                   reset parameter; added support for tempo and tune functions;
                   lint updates.
07/14/00   spf     Replaced memory.h with string.h.
06/29/00    st     Modified code such that snd_priority_type is no longer used 
                   to index into snd_array[].
06/29/00    st     Changed code to use the correct SND_AUDIO_FORMAT_SOUND enum.
06/22/00    st     Reset change_mask in snd_change() function after first 
                   aging iteration, this allows sounds with zero time to be 
                   aged properly.
06/13/00    st     Added snd_audfmt_new_data() function.
06/11/00   spf     Fixed compiler directive in snd_age_tone().
05/18/00    st     Added support for audio format sound playback; added sound
                   flow functions (currently only audio format sounds support
                   the flow functions); modified code to use sound age function
                   pointer instead of snd_age(); added support for another
                   level of sound playback.
05/01/00    st     Modified code to return ABORT status if voice playback has
                   been aborted by vocoder driver; added code to handle 
                   SND_DEVICE_CURRENT properly in snd_set_device_req().
03/06/00    st     Modified sound index to 16 bit value for larger sounds;
                   removed FEATURE_ENHANCED_RINGS from scaling; reset scaling
                   factor to zero after tones are done playing.
02/10/00    st     Removed sleep logic, sound no longer has to wait for phone
                   to wake up before starting the vocoder DTMF generator.
11/15/99    st     Fixed bug in snd_expire() where a NULL pointer was
                   possibly being accessed.
09/09/99  jkl/st   Modifications after code review; added AGC/AVC control 
                   functions.
08/24/99   st      Added code for click state machine; temporary fix for 
                   snd_age() (when a loop loops back to a control tone).
08/17/99   jkl     Added new function call according to the new API
07/30/99   st/vl   Moved SND_VP_SIGNAL inside #ifdef FEATURE_VOICE_PLAYBACK
                   in snd_change().
07/19/99   st/vl   Added status parameter to voice play callback functions.
                   Removed 'near '. Always do snd_age() when we receive signal
                   from vocoder regarding voice prompts. Added
                   snd_get_voice_path_out() function to return mute state of
                   current voice path.
06/30/99  sm/RPW   Integrated AGC feature with VR/VM features; removed double-
                   fault on voc_acquire ERR_FATAL (st).
03/22/99    st     Removed SND_ANALOG command; moved snd_rxfm_mute_ctl()
                   functionality to snd_rx_mute_ctl() function in sndhw; put
                   mechanism to start tones if entering TGP w/VR protocol; HFK
                   AEC commands are now done in sndhw.
03/04/99    st     Modifications to support tones which are specified by 
                   the calling task in the form of the frequency values in hz;
                   added support for voice memo.
01/13/99    st     Added support for playing voice prompts; added casts, etc...
                   to remove compiler warnings and errors.
11/09/98    st     Added timeout for request to play tones on the HFK for VR;
                   moved target dependant #pragma to after "target.h" include.
11/04/98    sm     Merged ARM/MSM3000 support to mainline.
10/16/98    st     Included auxrom.h header file.
10/08/98    st     Added DOTG to list of recognized device IDs.
10/08/98    st     HFK-VR mainlining: Made changes in snd_change() to to set 
                   tone timer to maximum period if tone is on hold; fixed bug
                   in use of snd_delay_index variable; modified calls to 
                   sndhw_hfk_ctl() to use sndhw_hfk_aec() and sndhw_hfk_phs(); 
                   added SND_EXT_PROTOCOL command, and modified SND_EXT_CONNECT
                   command; added SND_IDS_SIG in REX signal wait list; added 
                   support for playing tones on the HFK while in VR mode.
10/08/98    st     Make call to sndhw_voc_change_path_set() when 
                   SND_VOC_CHNG_SIG is received.
08/28/98    st     Modified code to 'unhold' sounds in snd_expire(), added
                   code to expire SND_TONE after 500ms timeout: CR6033.
                   Also changed global variables to be in 'near' in attempt 
                   to save code space.
08/24/98    st     Modified code to no longer change AEC to CDMA mode on HFK
                   while tone is played during an analog call, found that this
                   caused HFK filters to reset. CR5947.
08/17/98    st     Modified code to retrieve predefined sounds out of AUXROM
                   instead of from ROM.
06/22/98    st     Added SND_ADSP_TONEGEN case in snd_change_active_tone() to 
                   handle playing of tones on ADSP.
04/21/98    st     Modified PHS override setting for coil-cord HFKs, corrects
                   power down problem when disconnecting phone from HFK, in
                   function snd_change_active_tone(), CR5438
04/08/98    st     Changed code to set HFK to CDMA AEC mode when playing tones
                   in FM mode, corrects distorted rings and tones on the HFK
                   while in FM mode. CR5357
04/01/98    ha     Changed SND_NUM_PACKETS to 20 - CR4511.
03/20/98    ha     Changed SND_MAX_KEYS to 3 and SND_KEY_SPACE to 50 ms 
                   duration. Also replaced #include "ui.h" with 
                   #include "mc.h" - CR4340.
02/13/98    ha     Added support for HFK volume scaling.
02/06/98    ro     Fix initial unmute in FM - CR3292.
                   No calls to snd_comp_ctl (they were obsolete).
02/02/98    ro     Removed #ifdef NotYetEnabled, and added debug message for
                   SND_FILTERS command.
01/30/98    ha     Removed conditional compile from headset path volume change
                   in snd_req(). Updated comments.
01/21/98    ha     Added support for SND_VOL_SCALE ctrl tone. 
01/16/98    ro     Added support for external communications and device ID.
                   Added SND_EXT_CONNECT and SND_EXT_ID commands.
01/12/98    ro     Use new sndhw_stereo_mute interface.
                   Update copyright.
12/08/97    ha     Added support for click sound. 
                   Removed snd_array default initialization.
08/18/97    ha     Added a 30msec time delay in between ringer tone generation
                   and unmuting the microphone. This is to avoid transmitting
                   local ringer sounds over the air that produce a 'click' sound
                   at land side.
07/23/97    ro     Lint fix for T_I2.
06/20/97    ro     Changed initial value of snd_txfm_muted in
                   snd_txfm_mute_ctl() to fix muted TX in FM bug.
06/06/97    ro     Set volume to zero when path override is on with no tone.
                   Control the HFK+ AEC mode bit, was in hw layer.
                   Eliminate handling of SND_VOX_CHNG_TIMER_SIG, the
                   background noise init is moved to the vocoder driver.
05/23/97    ro     Add path override function to keep the vocoder on for
                   a period of time after a sound is played.
                   Complete support for "HFK Plus" carkit.
                   No longer include mc.h.
                   Snd_voice_muted no longer LOCAL.
04/29/97    ro     Support TGP carkit - "NGP Plus".
04/20/96    ro     Fix problem with setting volume = 0 in SND_VOLUME command.
04/17/97    ro     In snd_task(), allow sleeping after task startup.
                   Previously, SND would prevent the phone from sleeping
                   until at least one command was received.
04/10/97    ro     Some cleanup and comment adjustments.
                   Use sndhw_rx_mute_ctl() instead of calling
                   voc_rx_mute_cmd() directly.
                   Move definition of snd_voc_dtmf_volume to hardware layer.
                   Make snd_dtmf_tx_muted LOCAL.
                   Remove #if's that specify all supported targets:
                   (TG==T_MD) || (TG==T_G) || (TG==T_T) || (TG==T_I2))
03/28/97    ro     Support for TGP. Uses SND_FIRST_CONTROL_TONE to detect
                   control tones. Added support for additional loop-back
                   control tones. Update multi-tone termination check.
03/12/97   jjn     Added (TG==T_MD) to conditional compile statements
02/12/97    ro     Add support for SND_GEN_AUX.
01/20/97    ro     Added key delay to support car kit delayed-on time.
                   Added SND_RPT1 in support of key delay function.
                   Include ui.h to get timing for key delay.
                   Don't check snd_class in SND_RING_VOLUME.
01/07/97    ro     Support for Car-kit and Earpiece DTMF's using the vocoder.
                   Interface changes:
                   Added SND_SET_PATH and SND_VOLUME commands.
                   Removed SND_APATH_IN, SND_APATH_OUT, SND_APATH_DTMF
                   SND_SPKR_VOLUME, and SND_PHONE_VOLUME commands.
                   Removed support for these targets:
                   TG=T_M, T_B2, T_S, T_P, T_I1
                   Added support for generator selection - sclass field to
                   SND_*_START commands.
                   SND_MULTI_START - take rpt_sigs from command.
                   In snd_get_packet, clear entire packet. Include <memory.h>.
10/14/96    ro     Added (TG==T_T) cases to all #if's that have (TG==T_G)
                   for initial support of TGP build.
08/23/96    ro     Added support for SND_SIGNAL_SILENCE and SND_RPT_NOSIGNAL
                   to snd_age().
07/18/96   fkm     Minor Lint Nit on T_G Targets
07/08/96   fkm     Fixed Problem if Constant DTMF running while going in/out
                   of Car Kit.
03/19/96   fkm     Updates Per Code Review
02/16/96   fkm     Tweeks to comments and documentation
02/12/96   fkm     Lint and Documentation Update.  Some Apex Code (disabled).
01/17/96   fkm     Added T_C1 Stuff and Stereo Mute now only on certain
                   supported targets
01/03/96   fkm     Lint Nits
12/21/95   fkm     Removed (TG<T_G)s and Moved some Consistency checking
                   under SND_DEBUG.
12/13/95   fkm     Lint Nits
12/06/95   fkm     Fixed some #ifdef T_MSM2P and SND_MUTE_SNDV Problem
11/30/95   fkm     Removed MSG_ERROR() left as a trace, snd_init() now checks
                   size of snd_sound_ptrs array.
11/02/95   fkm     Minor Name Change to Vocoder Gain Adjust Stuff.
11/02/95   fkm     Slight Change to Stereo Mute, Support for declaring
                   Vocoder Gain Type and Packet Pool Provided.
10/11/95   fkm     Wait for Phone Awake before voc_acquire()
09/28/95   fkm     Fixed loop_cnt initialization problems and Beta2 Builds
09/21/95   fkm     Fixed "Volume Jump" on Key Press when in Car Kit
09/06/95   fkm     snd_stereo_mute now an extern (allocated elsewhere)
08/22/95   fkm     Added Car Stereo Support via SND_[UN]MUTE_CAR_STEREO
08/15/95   fkm     Fix DTMF mute problem for SND_DTMF_APATH_BOTH
07/21/95   fkm     VOC_UNITY_GAIN now only set going to Analog Mode
07/06/95   fkm     More Minor Documentation Updates
06/19/95   fkm     Minor Documentation Updates
06/14/95   fkm     Fixed Gemini Muting Problems and ISS2 Compile Problems
06/08/95   fkm     Added Compander Support for Gemini
05/16/95   fkm     Fixed #ifdef (TG==T_G) to also do ISS2.
04/10/95   fkm     Gemini Changes
01/13/95   fkm     Code Reorginization in Preparation of Gemini Hardware
12/09/94   jah     Added vocoder status monitoring to mobile-specific code.
11/09/94   jah     Changes to use voc_acquire(), fixed pbx_ss alert.
05/05/94   jah     Changed standard alerting to match IS-95 section 7.7.5.5
                   Fixed a bug in the ringer volume update.
01/06/94   jah     Changed watchdog iface to use dog_report().
11/08/93   jah     Added snd_init_1271_from_nv() to init the 1271 using values
                   stored in NV.  Added T_W1 values for the WLL #1 ringer.
08/10/93   jah     Have the global ring volume change update active ring volume
07/30/93   jah     Added snd_fm_tx_pwr() calls to turn-on/vote-off the 1271.
                   Replaced snd_in_a_call with snd_voc_is_on.
07/15/93   jah     Changed T_P normal alert to be 2700/2350 Hz, and changed
                   the ISDN style ring tones to follow this scheme.
06/04/93   jah     Added support for SND_VOC_CHNG_SIG for 1271 on/off control.
05/27/93   jah     Change voc_pwr_on() to voc_pwr() to match iface change.
05/25/93   jah     Added Beta II clock programming, and new Vocoder init for
                   Mobile and Beta II.
05/04/93   jah     Changed T_P error/warn tone to 2 KHz Sony tone from Ringer.
05/03/93   jah     Added snd_wbd_mode() to fix FM DC-bias problem.
04/26/93   jah     Added changes for Beta II mobile.
04/14/93   jah     Added Portable muting during ATX switching (on <-> off).
04/09/93   jah     Added SND_SLEEP_ALLOW/FORBID() ref's for CDMA sleep.
03/18/93   jah     Changed Attenuators based on input fron Paul Peterzell.
03/16/93   jah     Changed STD to follow FM_CLK.  Added clk_divide() init.
02/25/93   jah     Change tone audio path switching to not change the path
                   for tones which have no association to the audio path. 
02/23/93   jah     Change no-qwack to set volume after the path change.
02/19/93   jah     FM_CLK can't really be off and have the audio BPF work
                   right.  Added a snd_in_call variable, which is currently
                   always TRUE.  Added vox change timer for resetting vocoder
                   background noise estimate after a vox and path change.
02/17/93   jah     Fix the ring volume to get set from the current volume.
02/09/93   jah     Added no-qwack changes, added DTMF path change to no-qwack.
                   Added FM_CLK control for Portable power savings.
01/21/93   jah     Changed A3 to 3.2 dB, and A4 to -1.2 dB after a tuning
                   excercise.  Vocoder ERR() messages changed to MSG_MED()
                   for vocoder failures that could be a result of the vocoder
                   being powered off in the portable.
01/13/93   jah     Put back in portable hooks lost somehow in the last rev.
01/08/93   jah     #ifdef SND_NO_QWACK non-qwacking voice/tone path/vol change
                   code.  This code needs to be integrated with UI.
                   Added SND_MUTE_SNDV, for voice muting separate from
                   path-change muting.
01/05/93   jah     Changed filters to 0 db for non-mobile targets.
                   Changed snd_voc_tone() to allow for tone overlap
                   so that the 'clip' noise is not generated.
10/22/92   jah     Moved Voice muting from tone generators into snd_change(),
                   this lessens the voc_rx_mute() command load on the vocoder
                   and mutes voice for the duration of the sound.
                   Improved some comments.
10/21/92   jah     Portable ringer + Diag mute overrides for test-set.
10/16/92   jah     Broke heavily mobile-specific routines off into sndm.c.
                   Made ringback/audible-ring correspond to ISDN layer-3
                   specification instead of brassboard.  Added comments and
                   some portable-specific code (inside of #if (TG==...)).
10/13/92   jah     Using BYPS during mute to avoid Expander noise has the
                   unfortunate side effect of disabling the compressor on
                   the transmit path.  BYPS during mute is no longer used.
10/08/92   jah     Changed VOX to only change from the VOX mode if either of
                   the in/out audio path is to the handset.  Regression test.
10/01/92   jah     Route muted audio away from the speaker/external speaker.
                   Allow speaker to be installed/de-installed during phone
                   operation.  Changed "mute input" to send the vocoder a
                   voc_tx_mute().
09/23/92   jah     Allow tones to transmit when the output volume==0.
                   Reordered sound path routines above tone generation
                   routines.  Fix un-mute of audio after a 1270 tone.
09/22/92   jah     Streamlined setting of audio-out to not re-send a handset
                   command that was just sent.  Also fixed a state-change bug
                   in the DTMF path setting.
09/18/92   jah     Added voice muting during tone changes.
09/16/92   jah     Added db_put() and voc_vox_cmd() to snd_voc_mode() to set
                   the vocoder into VOX mode, and let RXC know about VOX too.
09/15/92   jah     Changed snd_1271_ser_out() to be mobile/portable
09/11/92   jah     Changed service area change tone to 2 beeps.  Changed
                   expiration of a tone by a new tone command, to not signal
                   back to the calling task (convenience for UI).
09/10/92   jah     Upgraded and improved all of the 'ringing' tones.
09/03/92   jah     Fix SND_ANALOG to reset VOX for CDMA/Analog at handoff.
                   Fixed tone restart after mute.  Fixed off by 1 count in
                   SND_LOOP_BACK2 (do while) loop in predefined sounds.
09/02/92   jah     Fix audio input muting, bug in 9/1 change.  Added Horn
                   alert.  Added snd_set_volume().  Added snd_comp_ctl().
09/01/92   jah     Added snd_set_vox_mode() to set VOX mode.  Added message
                   threshold.
08/28/92   jah     Changed SND_SVC_CHANGE and SND_FADE_TONE to have greater
                   mass appeal, tightened audio path synchronization.
                   Removed 'erasure' noise from analog by setting vocoder
                   voice volume to 0.
08/27/92   jah     Added snd_rxfm_mute_ctl()
08/26/92   jah     Added control of switch 6 to Rx audio muting, tone mute bug.
08/25/92   jah     Removed audio switches 4 & 5 from muting, they are noisy.
08/25/92   jah     Added snd_age(), ordered tones to simplify snd_change.
08/24/92   jah     Removed partial T_REX support, SND_2ND -> 770 Hz, added
                   volume change tracking messages, improved mute control.
08/21/92   jah     Fixed DTMF muting to mute and fixed bug in Unmuting,
                   removed incompatible T_REX support.
08/20/92   jah     Upgraded and improved all of the 'ringing' tones, added
                   SND_LOOP_BACK2 to make them non-huge.  Put voc_init()
                   after audio path muting, to avoid 'popping'.
08/14/92   jah     Cleaned-up audio-out path synchronization,
                   added "answer tone", and new RING, fade + svc change sounds
08/09/92   jah     Fixed ANALOG/CDMA to mute TXFM in CDMA/Idle mode
08/07/92   jah     Fixed TxFM audio un-muting
08/06/92   jah     Fixed RxFM audio un-muting
08/06/92   jah     Changed Attenuator #3 and #4 to -1.2 dB
08/05/92   jah     Changed SND_MED_PITCH_TONE to 941 Hz
08/05/92   jah/gb  Fixed snd_init() to have the codec clocks disabled during
                   voc_init() and enabled later.  Fixes unstable voc inits.
06/03/92   jah/tdw Created

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "snddata.h"            /* Sound Calibration database             */
#include "sdb.h"                /* Sound Database prototypes              */
#include "sndi.h"               /* Sound internal typedefs                */
#include "sndihw.h"             /* Sound Hardware Generic Interface       */
#include "sndtest.h"            /* Tests for Sound functions              */
#include "rex.h"                /* definition of REX data types           */
#include "task.h"               /* Task definitions and prototypes        */
#include "msg.h"                /* Message logging/reporting services     */
#include "err.h"                /* Error logging/reporting services       */
#include "dog.h"                /* Watchdog signal and time definitions   */
#include "mc.h"                 /* Need for MC signals and prototypes     */
#include <string.h>             /* Interface to string library            */
#include "snd.h"                /* Sound typedefs and prototypes          */
#include "tdb.h"                /* Tone Database typedefs and prototypes  */
#include "vocdata.h"
#ifdef FEATURE_AVS_AUDIO_TRANSPORT_PROTOCOL
#include "audtp.h"           /*acm diag dispatcher*/
#endif /*FEATURE_AVS_PROTOCOL_OVER_DIAG*/
#ifdef FEATURE_AUDIO_FORMAT
#include "clk.h"                /* Clock typedefs and prototypes          */
#include "qw.h"                 /* Quadword typedefs and prototypes       */
#endif

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#include "audiosbc.h"        /* BT SBC Codec internal definitions      */
#endif /* FEATURE_SBC_CODEC */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

There are definitions for LOCAL data.  There is no global data.

===========================================================================*/
/* These constants support the key-queuing functions.
*/
#define SND_MAX_KEYS    3
#define SND_KEY_TIME    95
#define SND_KEY_SPACE   50

/* Set local message level [Regional Data].
*/
word snd_msg_level = MSG_LVL_LOW;

#undef  MSG_THRESHOLD
#define MSG_THRESHOLD   snd_msg_level

/* Watchdog and Tone Timers
*/
rex_timer_type snd_rpt_timer;    /* Watch Dog Timer               */
rex_timer_type snd_tone_timer;   /* Tone Duration Timer           */
#ifdef FEATURE_ANIMATED_AAC
rex_timer_type snd_aac_timer;   /* Tone Duration Timer           */
#define SND_AAC_RETRY_TIMER  20  /*Timer for next retry         */
#endif

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#ifndef FEATURE_AVS_SBCENC_TASK
rex_timer_type snd_sbc_codec_timer;  /* Timer for encoding and sending frames */
#define SND_SBC_CODEC_TIMER 10

#if defined (FEATURE_AVS_BT_SCO_REWORK)
snd_bt_cb_func_ptr_type snd_bt_cb_func_ptr = NULL;
#endif /*FEATURE_AVS_BT_SCO_REWORK*/

#endif
#endif

/*-------------------------------------------------------------------------
** Tone Buffers
**-----------------------------------------------------------------------*/
/* The following variables and constants are used for buffering tones
** and supporting the key-delay function.  The key delay is required
** to support a delay in turning on the car kit.
*/
#define SND_STI_TONE    0       /* Single Tone Index for "tone" entry   */
#define SND_STI_TYPE    1       /* Single Tone Index for stop/rpt entry */

/* The size of the tone array depends on how many tones can be delayed.
** After the maximum number of keys are delayed, the last tone
** will be over-written.
*/
#define SND_MAX_DELAY_INDEX  (2*SND_MAX_KEYS)
#define SND_TONE_ARRAY_SIZE  (SND_MAX_DELAY_INDEX+1)

/* Array which includes status information for each type of sound.
** This Array is indexed by snd_array_idx_type.
**
** Note: last entry is highest priority.
*/
snd_sound_status_type snd_array[SND_ARRAY_SIZE]; 

/*-------------------------------------------------------------------------
** State variables
**-----------------------------------------------------------------------*/

/* Output paths (Rx)
*/
snd_pstate_type snd_voice_path_out;
snd_pstate_type snd_tone_path_out;

/* Input path muting (Tx)
*/
snd_mute_control_type snd_voice_path_in_mute  = SND_MUTE_MUTED;
#ifdef FEATURE_LOCAL_DTMF_TX_VOICE
snd_mute_control_type snd_tone_path_in_mute   = SND_MUTE_UNMUTED;
#else
snd_mute_control_type snd_tone_path_in_mute   = SND_MUTE_MUTED;
#endif /* FEATURE_LOCAL_DTMF_TX_VOICE */

/* Audio Mute Flag - audio will be unmuted at initialization.
*/ 
boolean snd_voice_muted = FALSE;

/*-------------------------------------------------------------------------
** Sound Command Queue, Free Queue and Available Packets.
**-----------------------------------------------------------------------*/

/* SND_NUM_PACKETS controls the number of Free Packets that are allocated
** and made available on snd_free_q. The number of packets has to be chosen
** accordingly to the worst case for setting sound volume, path info PLUS
** any other events that UI may  require sound to act on. 
** The premature initialization of UI with respect to Sound requires sound
** to have enough room to queue commands from UI before snd can process them.
** 30 packets is slightly over the required number of packets before snd 
** gets a chance to clear up the queue.
** This ensures that no volume settings (sent by UI) are lost.
*/ 

#define SND_NUM_PACKETS 30        /* # of Free Packets Allocated  */
q_type           snd_cmd_q;       /* Request Packets Queue        */
q_type           snd_free_q;      /* Available Packets Queue      */
snd_packets_type snd_free_packet[SND_NUM_PACKETS];

#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif

#ifdef FEATURE_VOICE_PLAYBACK
/* Callback function pointer for voice prompts */
snd_cb_func_ptr_type  snd_vp_callback_func = NULL;

/* This variable is used to store the vocoder VP playback statuses.    */
#define SND_NUM_VP_SIGS 5
voc_end_vpentry_return_type snd_vp_status[SND_NUM_VP_SIGS];

/* This is an array of pointers to the voice prompt client data that 
** have been played by the vocoder. It is used for client callback purposes.
*/
const void *snd_vp_client_data[SND_NUM_VP_SIGS];

/* This parameter is used to indicate the number of VP callbacks
** that are to be processed.
*/
uint32 snd_vp_sig_num   = 0;
#endif

#ifdef FEATURE_AUDIO_FORMAT
/* Total actual elapsed time, in milliseconds, of the current audio 
** format playback and the elapsed time adjusted for tempo variation.
*/
uint32 snd_audfmt_elapsed_ms;
uint32 snd_audfmt_adjusted_ms;

/* Time of the next tick update for audio format playback */
#define SND_AUDFMT_TICK_INTERVAL 1000
uint32 snd_audfmt_next_update;

/* Debug parameter that keeps track of the unaccounted for time */
uint32 snd_audfmt_diff_out;

/* Parameter to help keep accurate timing for audio format playback */
qword  snd_audfmt_interval = {0, 0};

/* The following parameters are used to get the playback time of an audio 
** file.
*/
void                       *snd_af_get_time_ctl      = NULL;
snd_time_cb_func_ptr_type  snd_get_time_cb_ptr       = NULL;
const void                 *snd_get_time_client_data = NULL;

/* The following parameters are used to do fast-forward and rewind during
** audio format playback.
*/
void                   *snd_af_ff_ctl      = NULL;
snd_priority_type      snd_af_ff_pri;

/* This is a pointer to an audio format command buffer. This is used in the 
** cases where the audio format sound is not the highest priority sound.
*/
audmain_cmd_buf_type *snd_audfmt_buf;

/* This timer is used as a bailout in case an audfmt data read does not
** callback withing 3 seconds.
*/
#define SND_AUDFMT_DATA_TIMEOUT 3000    /* 3 second bailout timer          */
rex_timer_type snd_audfmt_data_timer;   /* Audio Format data bailout timer */

#ifdef FEATURE_QCP
/* This flag is used to indicate if there is a current QCP recording
** session active.
*/
boolean snd_qcp_record_active = FALSE;

/* This flag is used to indicate when there is an error enqueuing a QCP
** prompt to the vocoder driver.
*/
boolean snd_vp_err_status = FALSE;
#endif /* FEATURE_QCP */
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_MIDI_OUT
/* This enum is used to indicate the state of the MIDI out connection.
*/
typedef enum {
  SND_MIDI_OUT_STATE_DISABLED,
  SND_MIDI_OUT_STATE_PENDING,
  SND_MIDI_OUT_STATE_ENABLED
} snd_midi_out_state_enum_type;

/* Callback and client data pointers for MIDI_OUT commands */
snd_midi_out_state_enum_type snd_midi_out_enabled = SND_MIDI_OUT_STATE_DISABLED;
snd_cb_func_ptr_type         snd_midi_out_cb           = NULL;
const void                   *snd_midi_out_client_data = NULL;

/* Buffer to hold commands to be issued to MIDI output device.
** There is a limit of 32 commands per function call, which means
** 32*4 = 128 words; 32 standard MIDI commands would be 32*3 = 96.
*/
#define SND_MIDI_OUT_MSG_MAX  128
#define SND_MIDI_OUT_STD_MAX  96
uint16 snd_midi_out_buffer[SND_MIDI_OUT_MSG_MAX];

#ifdef FEATURE_MIDI_OUT_QCP
/* Handle of QCP playback to MIDI output device */
void    *snd_midi_out_qcp_ctl   = NULL;
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

#ifdef FEATURE_QAUDIOFX
/* The following stucture and variable are used to keep track of 3D
** positional final source parameter settings query commands.
*/
typedef struct {
  snd_3d_pos_get_cb_ptr_type cb_func;      /* Source query callback function */
  const void                 *client_data; /* Source query client data       */
} snd_3d_pos_query_src_final_struct_type;

snd_3d_pos_query_src_final_struct_type snd_3d_pos_src_final[4] = {
                                                           { NULL, NULL },
                                                           { NULL, NULL },
                                                           { NULL, NULL },
                                                           { NULL, NULL }};
#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_AUDIO_FORMAT
#ifdef FEATURE_AUDFMT_EQUALIZER
/* Macro used to check supported Quality factor
*/
#define SND_EQUALIZER_CHECK_QFACTOR(x) (((x) >= 128) && ((x) <= 1792))
/* Macro used to check supported gain
*/
#define SND_EQUALIZER_CHECK_GAIN(x)    (((x) >= -12) && ((x) <= 12)) 
/* Macro used to check cut-off freq range, it is depend on the filter boost
** type
*/
#define SND_EQUALIZER_CHECK_CUTOFFFQ(x,y)                                   \
                 ((((y) == SND_EQUALIZER_BASE_BOOST) && ((x) >= 50))     || \
                  (((y) == SND_EQUALIZER_TREBLE_BOOST) && ((x) >= 1000)) || \
                  (((y) == SND_EQUALIZER_BAND_BOOST) && ((x) >= 100)))

/* Local equalizer table
*/
#if (VOC_EQ_BANDS_NUM > 0) 
LOCAL snd_equalizer_filter_type snd_equalizer_buffer[VOC_EQ_BANDS_NUM];
#else
#define snd_equalizer_buffer NULL
#endif

LOCAL boolean snd_equalizer_buffer_inuse = FALSE;
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/* Callback to send data to client 
*/
LOCAL snd_spectrum_analyzer_cb_type snd_spectrum_analyzer_client_cb = NULL;
LOCAL void *snd_spectrum_analyzer_client_data = NULL;

/* Buffer used to store SPA data
*/
LOCAL uint16 snd_spa_buffer[SND_SPECTRUM_ANALYZER_NUM_COEFFS];

LOCAL boolean snd_spa_data_available = FALSE;
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

#ifdef FEATURE_MM_REC
/*
 * MultiMedia control block at sound layer
 */
snd_mm_rec_control_type snd_mm_rec_ctrl;

#endif /* FEATURE_MM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)

/* Callback function to send data to client 
*/

LOCAL snd_pp_cb_type snd_pp_client_cb = NULL;

#endif /* FEATURE_ARM_POST_PROCESSING ||(FEATURE_AVS_WB_PCM_RX_INTERFACE)  */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
LOCAL snd_pp_cb_type snd_sbc_client_cb = NULL;
#endif /* FEATURE_SBC_CODEC  || FEATURE_SBC_DSP_CODEC */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
snd_audfmt_poly_mode_type snd_audfmt_poly_mode = SND_AUDFMT_POLY_MODE_0;
#endif /* FEATURE_AUDFMT_HYBRID_MODE */

#if (defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)) \
    && defined(FEATURE_NEW_SLEEP_API)
    const char *const snd_sleep_str = "SND_SLEEP";
    sleep_okts_handle snd_sleep_handle;
#endif /* (T_SLEEP || FEATURE_SLEEP) && FEATURE_SLEEP_API */

/* default init status of snd task */
static boolean snd_task_init_status = FALSE;

/* <EJECT> */
/*===========================================================================

MACRO SND_SLEEP_ALLOW

DESCRIPTION
  Tell the sleep task that we are in a state where we can sleep.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The sleep task may take us up on our offer.

===========================================================================*/
#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#ifdef FEATURE_NEW_SLEEP_API
  #define SND_SLEEP_ALLOW() \
    sleep_assert_okts(snd_sleep_handle)
#else
  #define SND_SLEEP_ALLOW() \
    SLEEP_ASSERT_OKTS( SLEEP_SND_OKTS_SIG )
#endif /* FEATURE_NEW_SLEEP_API */
#else
#define SND_SLEEP_ALLOW()
#endif
/* <EJECT> */
/*===========================================================================

MACRO SND_SLEEP_FORBID

DESCRIPTION
  Tell the sleep task that we are NOT in a state where we can sleep.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#ifdef FEATURE_NEW_SLEEP_API
  #define SND_SLEEP_FORBID() \
    sleep_negate_okts(snd_sleep_handle)
#else
  #define SND_SLEEP_FORBID() \
    SLEEP_NEGATE_OKTS( SLEEP_SND_OKTS_SIG )
#endif /* FEATURE_NEW_SLEEP_API */
#else
#define SND_SLEEP_FORBID()
#endif

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_dtmf_tx_mute_ctl

DESCRIPTION
  Temporary mute/unmute of the DTMF output to the FM transmit path.
  The parameter allows multiple requesters to request muting and for the
  audio path to unmute only when all requesters have un-muted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_dtmf_tx_mute_ctl
(
  snd_mute_type mask,
    /* mask of audio mute request associated with caller */

  boolean mute_dtmf
    /* True = mute DTMF, False = un-mute DTMF. */
)
{
  static snd_mute_type   dtmf_tx_muted         = SND_MUTE_NONE;
    /* Transmit DTMF mute mask. */
  static boolean         dtmf_tx_mute_override = FALSE;
    /* Mute override controls for DM - once DM requests mute override, we will
    ** never get back to non-override until the phone powers off.
    */
  boolean                snd_was_muted;
    /* Used to Remember if we were already muted */

  /* Diagnostic override allows the Diag Task to run its tests with
  ** full control over muting.  Once we have a Diagnostic override,
  ** we have it forever.
  */
  if ( (mask & SND_MUTE_DIAG) != 0 ) {
    dtmf_tx_mute_override = TRUE;
    mask = SND_ALL_MUTE;                      /* Vote all bits */
  } else if ( dtmf_tx_mute_override ) {
    return;
  }

  INTLOCK();                                  /* Block interrupts */

  /* Capture "Prior State"
  */
  snd_was_muted = (boolean) (dtmf_tx_muted != SND_MUTE_NONE);

  /* Turn on/off the associated mask bit
  */
  if ( mute_dtmf ) {
    /* Set the bit */
    dtmf_tx_muted = (snd_mute_type)(dtmf_tx_muted | mask);
  } else {
    /* mask the bit */
    dtmf_tx_muted = (snd_mute_type)(dtmf_tx_muted ^ (dtmf_tx_muted & mask));
    /* Note: you can't legally ones-compliment an enum */
  }
  INTFREE();                            /* Un-block interrupts         */

  if (dtmf_tx_muted) {
    if ( !snd_was_muted ) {             /* Want Mute, but not already  */
      sndhw_dtmf_tx_mute_ctl(TRUE);     /* Mute Hardware               */
      /* Trace this Sound Request (if SND_DEBUG Enabled)
      */
      SND_DEBUG_MSG("snd_dtmf_tx_mute_ctl(0x%x,%d)",mask,mute_dtmf,0);
    }
  } else {
    if ( snd_was_muted ) {              /* Want Unmute,but not already */
      sndhw_dtmf_tx_mute_ctl(FALSE);    /* Unmute Hardware             */
      /* Trace this Sound Request (if SND_DEBUG Enabled)
      */
      SND_DEBUG_MSG("snd_dtmf_tx_mute_ctl(0x%x,%d)",mask,mute_dtmf,0);
    }
  }

} /* end of snd_dtmf_tx_mute_ctl */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_get_packet

DESCRIPTION
  Get a Sound Command Packet.  Use by callers of snd_cmd().
  
DEPENDENCIES
  snd_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available snd_packets_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  snd_free_q (and the packet fetched off snd_free_q)

===========================================================================*/
LOCAL snd_packets_type *snd_get_packet( void )
{
  snd_packets_type *packet;

  /* Get a packet and then either report an error, or clear the packet out
  ** so the previous user's garbage doesn't do something unexpected.
  */
  packet = (snd_packets_type *) q_get( &snd_free_q );
  if( packet == NULL ) {
    ERR( "Ran Out of Sound Packets!", 0, 0, 0 );
  } else {    
#ifdef SND_DEBUG
    /* Fill the entire sound packet with invalid values to 
    ** catch any un-initialized fields.
    */
    (void) memset(packet, 0xff, sizeof(snd_packets_type));
#else
    /* Fill the entire sound packet with 0's to get
    ** default behavior on un-initialized fields.
    */
    (void) memset(packet, 0, sizeof(snd_packets_type));
#endif /* SND_DEBUG */
    packet->hdr.cmd        = SND_INVALID;       /* Invalid Command Value */
  }

  return( packet );

} /* end of snd_get_packet */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_cmd

DESCRIPTION
  Queue a command for processing by the Sound Task.

DEPENDENCIES
  This routine must not be called until the Sound Task has been started.

RETURN VALUE
  None

SIDE EFFECTS
  snd_cmd_q
  snd_tcb

===========================================================================*/
LOCAL void snd_cmd
(
  snd_packets_type *snd_cmd_ptr
    /* pointer to sound command */
)
{
  (void) q_link(snd_cmd_ptr, &snd_cmd_ptr->hdr.link );  /* Init link field */

  q_put( &snd_cmd_q, &snd_cmd_ptr->hdr.link );    /* Put on command queue  */
  (void) rex_set_sigs( &snd_tcb, SND_CMD_Q_SIG ); /* Signal a queue event  */

} /* end of snd_cmd */

/* <EJECT> */
#ifdef  FEATURE_AUDIO_FORMAT 
/*===========================================================================

FUNCTION snd_av_sync_int_cb

DESCRIPTION
  This function queues an AV Sync call back request command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_av_sync_int_cb ( 
  qword timestamp,
  qword num_of_samples,
  qword num_of_bytes
)
{
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */
 
  /* Get a Sound Packet and queue the Request */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL){
    packet->hdr.cmd = SND_AUDFMT_AV_SYNC;

    qw_set(packet->av_sync.timestamp,
           qw_hi(timestamp), qw_lo(timestamp));

    qw_set(packet->av_sync.num_of_samples,
           qw_hi(num_of_samples), qw_lo(num_of_samples));

    qw_set(packet->av_sync.num_of_bytes,
           qw_hi(num_of_bytes), qw_lo(num_of_bytes));

    snd_cmd(packet);
  }
}/* end of snd_av_sync_int_cb */

/*===========================================================================

FUNCTION snd_av_sync_int_cb_req

DESCRIPTION
  Invokes the A/V sync callback registered by client that initiated playback
  in SND task context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void snd_av_sync_int_cb_req (
  qword timestamp, 
  qword num_of_samples,
  qword num_of_bytes
)
{
  snd_sound_status_type       *snd_ptr;
  snd_array_idx_type          snd_idx;
  const snd_af_av_sync_type   *av_sync;

  for(snd_idx = SND_SND_LO; snd_idx <= SND_SND_HI; snd_idx++) {
    if (snd_array[snd_idx].active) {
      snd_ptr = &snd_array[snd_idx];
      av_sync = &snd_ptr->sound_ptr->format_sound.av_sync;
      if ((snd_ptr->sound_ptr->type == SND_FORMAT_SOUND) &&
          (snd_ptr->hold == FALSE) &&
          ((av_sync->av_sync_cb_func != NULL)
           || (av_sync->av_sync_ex_cb_func!= NULL))
         ) {
        /* 6k doesn't support sample rate changing, we can use same av sync
        callback but should provide av_sync_ex parameter. It's in 
        snd_av_sync_int_cb_req()
        */
        if ( av_sync->av_sync_cb_func != NULL ) {
          av_sync->av_sync_cb_func(timestamp, 
                                   num_of_samples,
                                   num_of_bytes,
                                   av_sync->client_data);
        } else if ( av_sync->av_sync_ex_cb_func!= NULL ) {
          snd_af_av_sync_info_type av_sync_ex;
          memcpy( &av_sync_ex.timestamp,      timestamp, sizeof(qword));
          memcpy( &av_sync_ex.num_of_samples, num_of_samples, sizeof(qword));
          memcpy( &av_sync_ex.num_of_bytes,   num_of_bytes, sizeof(qword));
          #ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
          /* provide same sample rate */
          av_sync_ex.sample_rate = 
              snd_ptr->sound_ptr->format_sound.input_spec.aac_spec.sample_rate;
          MSG_LOW("AVSync - sample rate [%d]", av_sync_ex.sample_rate,0,0);
          #endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */
          av_sync->av_sync_ex_cb_func( av_sync_ex,
                                       av_sync->client_data);
        }
        break;      
      }
    }
  }
} /* end of snd_av_sync_int_cb_req */
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION snd_vp_callback

DESCRIPTION
  This function calls the function pointed to by snd_vp_callback_func,
  if one exists, to free the specified voice prompt buffer.

DEPENDENCIES
  snd_vp_callback_func

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_vp_callback (
  const void         *client_data, /* ptr to client data supplied w\command */
  snd_status_type    status        /* status of voice playback */
) {
  if(snd_vp_callback_func != NULL) {
    snd_vp_callback_func(client_data, status);
  } 
#ifdef FEATURE_QCP
  else {
    snd_vp_err_status = TRUE;
  }
#endif /* FEATURE_QCP */
} /* end of snd_vp_callback */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_vp_signal

DESCRIPTION
  Callback function for vocoder voice prompt driver, used by vocoder driver
  to send sound task status information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  snd_vp_status
  snd_vp_client_data

===========================================================================*/
void snd_vp_signal (
  voc_end_vpentry_return_type vp_status, /* status of voice playback */
  voc_pb_header_type          *vp,       /* pointer to voice prompt concerned */
    /*lint -esym(715,vp) Tell Lint we know vp parameter is not used */
  const void                  *client_data 
    /* pointer to data client supplied with this voice prompt */

) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

#ifdef FEATURE_QCP
  /* For QCP file playback, all status callbacks from the vocoder should
  ** be ignored while seeking.
  */
  if(snd_af_ff_ctl == NULL) 
#endif /* FEATURE_QCP */
  {
    /* Get a Sound Packet and queue the Request.
    */
    packet = snd_get_packet();    /* Already Logs Error if Fails */
    if (packet != NULL) {
      packet -> vp_sig.hdr.cmd          = SND_VOICE_PROMPT_SIG;
      packet -> vp_sig.vp_status        = vp_status;
      packet -> vp_sig.client_data      = client_data;
      snd_cmd(packet);
    }
  }

} /* end of snd_vp_signal */
#endif /* FEATURE_VOICE_PLAYBACK */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_init

DESCRIPTION
  This procedure performs sound task initialization processing including
  hardware initialization.

DEPENDENCIES
  snd_rpt_timer
  snd_tone_timer
  snd_cmd_q
  snd_free_q
  snd_voice_path_out
  snd_tone_path_out
  snd_vp_signal

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
LOCAL void snd_init( void )
{
  int i;    /* Temporary for() counter for array looping */

  /* Initialize timers
  */
  rex_def_timer( &snd_rpt_timer, &snd_tcb, SND_RPT_TIMER_SIG );
  rex_def_timer( &snd_tone_timer, &snd_tcb, SND_TONE_TIMER_SIG );

#ifdef FEATURE_ANIMATED_AAC
  rex_def_timer( &snd_aac_timer, &snd_tcb, SND_AAC_RETRY_SIG );
#endif

#ifdef FEATURE_AUDIO_FORMAT
  rex_def_timer( &snd_audfmt_data_timer, &snd_tcb, SND_AUDIO_FORMAT_SIG );
#endif

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#ifndef FEATURE_AVS_SBCENC_TASK 
  rex_def_timer(&snd_sbc_codec_timer, &snd_tcb, SND_SBC_CODEC_TIMER_SIG);
#endif
#endif
  
  /* Initialize the queues.
  */
  (void) q_init( &snd_cmd_q );            /* Initialize the command queue  */
  (void) q_init( &snd_free_q );           /* Initialize the free queue     */

  for(i=0;i<SND_NUM_PACKETS;i++) {        /* Add items to snd_free_q       */
    q_link_type *link_item;
    link_item = q_link( &snd_free_packet[i], &snd_free_packet[i].hdr.link );
    q_put( &snd_free_q, link_item );
  }

  /* All snd_array entries are initialized with an inactive status. 
  */
  for (i=0; i < SND_ARRAY_SIZE; i++) {
     snd_array[i].active = FALSE;
  }

#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif

  /* Initialize paths
  */
  snd_setup_voice_path(SND_DEVICE_HANDSET,SND_MUTE_MUTED,&snd_voice_path_out);
  snd_setup_voice_path(SND_DEVICE_HANDSET,SND_MUTE_MUTED,&snd_tone_path_out);

  /* Mute Transmit DTMF's
  */
  snd_dtmf_tx_mute_ctl( SND_MUTE_SND, TRUE );

#ifdef FEATURE_VOICE_PLAYBACK
  /* Register callback function used by vocoder driver to indicate
  ** progress/status of voice prompt playback.
  */
  voc_pb_register( (void (*)(voc_end_vpentry_return_type, voc_pb_header_type*, 
                             const void * )) snd_vp_signal);
#endif

  /* initialize the tone database */
  tdb_init();

  /* Initialize the hardware as needed.  */
  sndhw_init();

#if (defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)) \
    && defined(FEATURE_NEW_SLEEP_API)
  snd_sleep_handle = sleep_register(snd_sleep_str,TRUE);
#endif /* (T_SLEEP || FEATURE_SLEEP) && FEATURE_SLEEP_API */

  MSG_LOW("Sound task initialized",0,0,0);

  /* set snd task init status */
  snd_task_init_status = TRUE;

#ifdef FEATURE_AVS_AUDIO_TRANSPORT_PROTOCOL
  MSG_LOW("Going to register ACM diag dispatcher",0,0,0);
  atp_diag_init();
  MSG_LOW("Completed registering ACM diag dispatcher",0,0,0);
#endif /*FEATURE_AVS_PROTOCOL_OVER_DIAG*/

} /* end of snd_init */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_offline

DESCRIPTION
  This procedure performs the transition to "offline" for the sound task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void snd_offline( void )
{

  /* Acknowledge the offline signal
  */
#ifdef FEATURE_MULTIMODE_ARCH
  task_offline();
#else
  (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif /*FEATURE_MULTIMODE_ARCH*/

  /* Clear the offline signal, and return
  */
  (void) rex_clr_sigs( &snd_tcb, TASK_OFFLINE_SIG );

} /* end of snd_offline */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_shutdown

DESCRIPTION
  This procedure performs shutdown processing for the sound task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void snd_shutdown( void )
{

  /* Acknowledge the stop signal
  */
#ifdef FEATURE_MULTIMODE_ARCH
  task_stop();
#else
  (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif /*FEATURE_MULTIMODE_ARCH*/

  /* Clear the stop signal, and return
  */
  (void) rex_clr_sigs( &snd_tcb, TASK_STOP_SIG );

} /* end of snd_shutdown */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_finish_cmd

DESCRIPTION
  Dispose of a command which is done being processed.

DEPENDENCIES
  snd_free_q

RETURN VALUE
  None

SIDE EFFECTS
  Can call the callback function with SND_SUCCESS that was in the command.

===========================================================================*/
LOCAL void snd_finish_cmd
(
  snd_packets_type *cmd_ptr
    /* Queue item link pointer for the command */
)
{
  snd_cb_func_ptr_type  call_back;      /* Pointer to call back function */
  const void            *client_data;   /* Pointer to client data        */
  
  call_back   = cmd_ptr->hdr.callback_ptr;
  client_data = cmd_ptr->hdr.client_data;

  /* Return the buffer to the free queue before calling the callback 
  ** function that indicates the operation has been completed.
  */
  q_put( &snd_free_q, (q_link_type *) cmd_ptr );

  /* If call back is non-NULL, execute this function indicating that the
  ** command was accepted. End of sounds will be another message.
  */
  if ( call_back != NULL ) {
    call_back( client_data, SND_SUCCESS );
  }
} /* end of snd_finish_cmd */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_get_sound_info

DESCRIPTION
  Looks into the sound structure and passes back the tone and duration
  of the corresponding index.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void snd_get_sound_info(
  const snd_sound_type *sound,   /* sound structure to crack */
  word                 index,    /* Index of tone into sound struct to return */
  snd_tone_type        *tone,    /* tone returned */
  snd_tone_data_type   *tone_data/* data returned */
) {
  snd_compact_tone_type  compact_tone;   /* temp compact tone structure  */
  snd_flexible_tone_type flexible_tone;  /* temp flexible tone structure */

  switch( sound->type ) {
    case SND_COMPACT_SOUND:
      compact_tone = (((snd_compact_sound_type *)sound)->tone_array)[index];
      *tone        = compact_tone.tone;
      *tone_data   = compact_tone.param;
      break;

#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif

    case SND_FLEXIBLE_SOUND:
      flexible_tone = (((snd_flexible_sound_type *)sound)->tone_array)[index];
      *tone         = flexible_tone.tone;
      *tone_data    = flexible_tone.param;
      break;

#ifdef FEATURE_VOICE_PLAYBACK
    case SND_TONE_AND_VOICE:
      compact_tone = (((snd_tone_and_voice_sound_type *)sound)->tone_array)
                                                                        [index];
      *tone        = compact_tone.tone;
      *tone_data   = compact_tone.param;
      break;
#endif

#ifdef FEATURE_AUDIO_FORMAT
    case SND_AUDIO_FORMAT_SOUND:
    case SND_FORMAT_SOUND:
      *tone                           = 0;
      tone_data->duration_ms          = 0xFFFF;
      break;
#endif
    /*lint -esym(787, SND_INVALID_SOUND) Tell Lint we know that
    ** this value for the enum is not in this switch statement
    */
  }
} /* end of snd_get_sound_info */
/* <EJECT> */
/*===========================================================================
FUNCTION snd_setup_tone

DESCRIPTION
  Use the Sound Status to setup the tone for playing. Figure out what
  device, method, and volume to use and write the information into the
  Tone Status.

DEPENDENCIES
  snd_voice_path_out

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void snd_setup_tone
(
  tone_status_type       *tone_status_ptr, /* Pointer to tone status          */
  snd_sound_status_type  *snd_ptr,         /* Pointer to sound's status table */
  snd_sound_status_type  *aux_ptr          /* Pointer to sound's status table */
)
{
  snd_compact_tone_type  compact_tone;        /* temp compact tone structure */
  snd_flexible_tone_type flexible_tone = {0,};/* temp freq tone structure    */
  snd_tone_type          tone = SND_SILENCE;  /* temp tone                   */
  tdb_dtmf_type          dtmf;                /* temp dtmf structure         */

#ifdef FEATURE_AUDIO_FORMAT
  audmain_file_type             parse_type;
#ifdef FEATURE_MPEG_AUDIO
  audmpeg_song_config_type      *mpeg_aud_cfg;
  snd_audfmt_mpeg_header_config *mpeg_hdr;
#endif /* FEATURE_MPEG_AUDIO */

#ifdef FEATURE_REAL_AUDIO
  snd_audfmt_ra_dsp_config      *ra_cfg;
  snd_ra_format_spec_type       *ra_spec;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
  snd_audfmt_wma_dsp_config_type *wma_cfg;
  snd_wma_format_spec_type       *wma_spec;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
  snd_audfmt_amr_wbp_dsp_config_type *amrwbp_cfg;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

#if defined(FEATURE_MPEG_AUDIO) || defined(FEATURE_REAL_AUDIO) || \
    defined(FEATURE_QCP)
  const snd_af_av_sync_type *av_sync;
#endif /* FEATURE_MPEG_AUDIO || FEATURE_REAL_AUDIO || FEATURE_QCP */
#endif /* FEATURE_AUDIO_FORMAT */

  /* Copy info */
  tone_status_ptr->method          = snd_ptr->method;
  tone_status_ptr->dtmf_path       = snd_ptr->dtmf_path;
  tone_status_ptr->time_remain_ms  = snd_ptr->time_remain_ms;
  tone_status_ptr->vol             = snd_ptr->vol;
#ifdef FEATURE_AUDIO_FORMAT
  tone_status_ptr->sample_rate     = snd_ptr->sample_rate;
  tone_status_ptr->av_sync_cb      = NULL;
#endif
#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
  tone_status_ptr->type            = SND_TYPE_SILENCE;

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
  tone_status_ptr->audfmt_codec_update_cb = NULL; /*initialize cb function*/
#endif

  /* get current tone */
  switch( snd_ptr->sound_ptr->type) {
    case SND_COMPACT_SOUND:
      compact_tone = (((snd_compact_sound_type *)
                       (snd_ptr->sound_ptr))->tone_array)[snd_ptr->tone_index];
      tone         = compact_tone.tone;
      break;

#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif

    case SND_FLEXIBLE_SOUND:
      flexible_tone = (((snd_flexible_sound_type *)
                        (snd_ptr->sound_ptr))->tone_array)[snd_ptr->tone_index];
      tone          = flexible_tone.tone;
      break;

#ifdef FEATURE_VOICE_PLAYBACK
    case SND_TONE_AND_VOICE:
      compact_tone = (((snd_tone_and_voice_sound_type *)
                       (snd_ptr->sound_ptr))->tone_array)[snd_ptr->tone_index];
      tone         = compact_tone.tone;
      break;
#endif

#ifdef FEATURE_AUDIO_FORMAT
    case SND_AUDIO_FORMAT_SOUND:
    case SND_FORMAT_SOUND:
#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
      if (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND) {
        /* SG: No mixing options on streamed interface */
        tone_status_ptr->audfmt_path = SND_AUDFMT_START_PATH_FORWARD;
      }
      else {
        tone_status_ptr->audfmt_path =
          snd_ptr->sound_ptr->audfmt_sound.start_position;
      }
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */

      parse_type = audmain_get_parse_type(snd_ptr->audfmt_ctl);
      switch (parse_type) {
#ifdef FEATURE_MIDI
        case AUDMAIN_MIDI_TYPE:
          tone_status_ptr->audfmt_buf = snd_ptr->audfmt_buf;
          tone_status_ptr->type       = SND_TYPE_AUDFMT;

#ifdef FEATURE_ANIMATED_AAC
        if( audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC){
          tone_status_ptr->type = SND_TYPE_AAC;
          tone_status_ptr->sample_rate = audmain_get_sample_rate( snd_ptr->audfmt_ctl);
          audmain_get_song_config( snd_ptr->audfmt_ctl, &snd_ptr->audfmt_song_cfg);
          tone_status_ptr->audfmt_dsp_config.aac_config.params = 
            snd_ptr->audfmt_song_cfg->header_config.aac_config;

          memcpy((void*)&tone_status_ptr->audfmt_dsp_config.mpeg_hdr_config.mpeg_mask,
            (void*)&snd_ptr->audfmt_song_cfg->header_config.mask, sizeof(uint8)*4);
          tone_status_ptr->audfmt_dsp_config.mpeg_hdr_config.mpeg_mask_size =
            snd_ptr->audfmt_song_cfg->header_config.mask_size;
          memcpy((void*)&tone_status_ptr->audfmt_dsp_config.mpeg_hdr_config.mpeg_header,
            (void*)&snd_ptr->audfmt_song_cfg->header_config.header, sizeof(uint8)*4); 

          if( audmain_get_init_done( snd_ptr->audfmt_ctl) != TRUE){
            sndhw_audfmt_cleanup();
          }
        }
#endif

#ifdef FEATURE_WEBAUDIO
          tone_status_ptr->audfmt_adec_cb = 
                                (voc_adec_cb_func_struct) snd_audfmt_adec_int;
          tone_status_ptr->audfmt_adec    = (void *) snd_ptr;
#endif /* FEATURE_WEBAUDIO */
          break;
#endif /* FEATURE_MIDI */

#ifdef FEATURE_IMELODY
        case AUDMAIN_IMY_TYPE:
          tone_status_ptr->audfmt_buf = snd_ptr->audfmt_buf;
          tone_status_ptr->type   = SND_TYPE_IMY;
#ifndef FEATURE_MIDI
          /* Use the Ringer otherwise */
          tone_status_ptr->method = SND_METHOD_RING;
#endif
#ifdef FEATURE_WEBAUDIO
          tone_status_ptr->audfmt_adec_cb =
                                (voc_adec_cb_func_struct) snd_audfmt_adec_int;
          tone_status_ptr->audfmt_adec    = (void *) snd_ptr;
#endif /* FEATURE_WEBAUDIO */
          break;
#endif /* FEATURE_IMELODY */

#ifdef FEATURE_MPEG_AUDIO
#ifdef FEATURE_MP3
        case AUDMAIN_MP3_TYPE:
          /* Fall through */
#endif
#ifdef FEATURE_AAC
        case AUDMAIN_AAC_TYPE:
#endif
          tone_status_ptr->audfmt_buf     = NULL;
          tone_status_ptr->audfmt_adec_cb =
                                (voc_adec_cb_func_struct) snd_audfmt_adec_int;
          tone_status_ptr->audfmt_adec    = (void *) snd_ptr;

          mpeg_aud_cfg = &snd_ptr->audfmt_song_cfg->header_config;

          mpeg_hdr = (snd_audfmt_mpeg_header_config *)
                                      &tone_status_ptr->audfmt_dsp_config;
          mpeg_hdr->mpeg_mask_size = mpeg_aud_cfg->mask_size;

          memcpy(mpeg_hdr->mpeg_mask, mpeg_aud_cfg->mask,
                 mpeg_hdr->mpeg_mask_size); 
          memcpy(mpeg_hdr->mpeg_header, mpeg_aud_cfg->header,
                 mpeg_hdr->mpeg_mask_size); 

#ifdef FEATURE_AAC
          if (parse_type == AUDMAIN_AAC_TYPE) {
            tone_status_ptr->type       = SND_TYPE_AAC;
            tone_status_ptr->audfmt_dsp_config.aac_config.params =
                                                     mpeg_aud_cfg->aac_config;
          }
#endif /* FEATURE_AAC */

#ifdef FEATURE_MP3
          if (parse_type == AUDMAIN_MP3_TYPE) {
            tone_status_ptr->type       = SND_TYPE_MP3;
            tone_status_ptr->audfmt_dsp_config.mp3_config.params =
                                                     mpeg_aud_cfg->mp3_config;
          }
#endif /* FEATURE_MP3 */

          if(snd_ptr->sound_ptr->type == SND_FORMAT_SOUND) {
            av_sync = &snd_ptr->sound_ptr->format_sound.av_sync;
            if((av_sync->av_sync_interval != 0) &&
               ((av_sync->av_sync_cb_func != NULL)
                || (av_sync->av_sync_ex_cb_func != NULL))
                && (av_sync->av_sync_mode < SND_AV_SYNC_INVALID)) {
                /* 6k doesn't support sample rate changing, we can use same av 
                sync callback but should provide av_sync_ex parameter. It's in
                snd_av_sync_int_cb_req()
                */
              tone_status_ptr->av_sync_mode     = av_sync->av_sync_mode;
              tone_status_ptr->av_sync_interval = av_sync->av_sync_interval;
              tone_status_ptr->av_sync_cb       = snd_av_sync_int_cb;
            }
          }
          #ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
          MSG_LOW("set audfmt_codec_update_cb %p", snd_audfmt_codec_update_int,0,0);
          tone_status_ptr->audfmt_codec_update_cb = snd_audfmt_codec_update_int;
          #else
          tone_status_ptr->audfmt_codec_update_cb = NULL;
          #endif
          break;
#endif /* FEATURE_MPEG_AUDIO */

#ifdef FEATURE_REAL_AUDIO
        case AUDMAIN_RA_TYPE:
          tone_status_ptr->type           = SND_TYPE_RA;
          tone_status_ptr->audfmt_buf     = NULL;
          tone_status_ptr->audfmt_adec_cb =
                               (voc_adec_cb_func_struct) snd_audfmt_adec_int;
          tone_status_ptr->audfmt_adec    = (void *) snd_ptr;

          ra_spec = (snd_ra_format_spec_type*)
                       &snd_ptr->sound_ptr->format_sound.input_spec.ra_spec;

          ra_cfg = (snd_audfmt_ra_dsp_config*)
                        &tone_status_ptr->audfmt_dsp_config.ra_config;
          ra_cfg->bits_per_frame = ra_spec->bits_per_frame;
          ra_cfg->coupling_quant = ra_spec->coupling_quant;
          ra_cfg->coupling_start = ra_spec->coupling_start;
          ra_cfg->decoder_mode   = ra_spec->decoder_mode;
          ra_cfg->frame_erasure  = ra_spec->frame_erasure;
          ra_cfg->num_channels   = ra_spec->num_channels;
          ra_cfg->num_regions    = ra_spec->num_regions;
          ra_cfg->sample_rate    = ra_spec->sample_rate;
          ra_cfg->samples_per_frame = ra_spec->samples_per_frame;

          if(snd_ptr->sound_ptr->type == SND_FORMAT_SOUND) {
            av_sync = &snd_ptr->sound_ptr->format_sound.av_sync;
            if((av_sync->av_sync_interval != 0) &&
               (av_sync->av_sync_cb_func != NULL) &&
               (av_sync->av_sync_mode < SND_AV_SYNC_INVALID)) {
              tone_status_ptr->av_sync_mode     = av_sync->av_sync_mode;
              tone_status_ptr->av_sync_interval = av_sync->av_sync_interval;
              tone_status_ptr->av_sync_cb       = snd_av_sync_int_cb;
            }
          }
          break;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_QCP
        case AUDMAIN_QCP_TYPE:
          tone_status_ptr->type       = SND_TYPE_VOICE_PROMPT;
          tone_status_ptr->audfmt_buf = NULL;

          if(snd_ptr->sound_ptr->type == SND_FORMAT_SOUND) {
            av_sync = &snd_ptr->sound_ptr->format_sound.av_sync;
            if((av_sync->av_sync_interval != 0) &&
               (av_sync->av_sync_cb_func != NULL) &&
               (av_sync->av_sync_mode < SND_AV_SYNC_INVALID)) {
              tone_status_ptr->av_sync_mode     = av_sync->av_sync_mode;
              tone_status_ptr->av_sync_interval = av_sync->av_sync_interval;
              tone_status_ptr->av_sync_cb       = snd_av_sync_int_cb;
            }
          }
          break;
#endif /* FEATURE_QCP */

#ifdef FEATURE_ADPCM
        case AUDMAIN_ADPCM_TYPE:
          tone_status_ptr->type           = SND_TYPE_ADPCM;
          tone_status_ptr->audfmt_buf     = NULL;
          tone_status_ptr->audfmt_adec_cb = snd_audfmt_adec_int;
          tone_status_ptr->audfmt_adec    = (void *) snd_ptr;
          break;
#endif /* FEATURE_ADPCM */

#ifdef FEATURE_PCM
        case AUDMAIN_PCM_TYPE:
          tone_status_ptr->type           = SND_TYPE_PCM;
          tone_status_ptr->audfmt_buf     = NULL;
          tone_status_ptr->audfmt_adec_cb = snd_audfmt_adec_int;
          tone_status_ptr->audfmt_adec    = (void *) snd_ptr;
          if(snd_ptr->sound_ptr->type == SND_FORMAT_SOUND) {
            av_sync = &snd_ptr->sound_ptr->format_sound.av_sync;
            if((av_sync->av_sync_interval != 0) &&
               (av_sync->av_sync_cb_func != NULL) &&
               (av_sync->av_sync_mode < SND_AV_SYNC_INVALID)) {
              tone_status_ptr->av_sync_mode     = av_sync->av_sync_mode;
              tone_status_ptr->av_sync_interval = av_sync->av_sync_interval;
              tone_status_ptr->av_sync_cb       = snd_av_sync_int_cb;
            }
          }
          break;
#endif /* FEATURE_PCM */

#ifdef FEATURE_VOC_G711
        case AUDMAIN_WAV_MULAW_TYPE:
          tone_status_ptr->type           = SND_TYPE_WAV_MULAW;
          tone_status_ptr->audfmt_buf     = NULL;
          tone_status_ptr->audfmt_adec_cb = snd_audfmt_adec_int;
          tone_status_ptr->audfmt_adec    = (void *) snd_ptr;
          break;

        case AUDMAIN_WAV_ALAW_TYPE:
          tone_status_ptr->type           = SND_TYPE_WAV_ALAW;
          tone_status_ptr->audfmt_buf     = NULL;
          tone_status_ptr->audfmt_adec_cb = snd_audfmt_adec_int;
          tone_status_ptr->audfmt_adec    = (void *) snd_ptr;
          break;
#endif /* FEATURE_PCM */

#ifdef FEATURE_WMA
        case AUDMAIN_WMA_TYPE:
        case AUDMAIN_WMA_PRO_TYPE:
          tone_status_ptr->type           = SND_TYPE_WMA;
          tone_status_ptr->audfmt_buf     = NULL;
          tone_status_ptr->audfmt_adec_cb = snd_audfmt_adec_int;
          tone_status_ptr->audfmt_adec    = (void *) snd_ptr;

          if(snd_ptr->sound_ptr->type == SND_FORMAT_SOUND) {
            av_sync = &snd_ptr->sound_ptr->format_sound.av_sync;
            if((av_sync->av_sync_interval != 0) &&
               (av_sync->av_sync_cb_func != NULL) &&
               (av_sync->av_sync_mode < SND_AV_SYNC_INVALID)) {
              tone_status_ptr->av_sync_mode     = av_sync->av_sync_mode;
              tone_status_ptr->av_sync_interval = av_sync->av_sync_interval;
              tone_status_ptr->av_sync_cb       = snd_av_sync_int_cb;
            }
          }

          wma_spec = (snd_wma_format_spec_type *)
                  &snd_ptr->sound_ptr->format_sound.input_spec.wma_spec;
          /*check if current played type is WMA_PRO*/
          if(wma_spec->version == 3)
           tone_status_ptr->type = SND_TYPE_WMA_PRO;

          wma_cfg  = (snd_audfmt_wma_dsp_config_type *)
                         &tone_status_ptr->audfmt_dsp_config.wma_config;
          wma_cfg->channels         = wma_spec->channels;
          wma_cfg->version          = wma_spec->version;
          wma_cfg->sample_rate      = wma_spec->sample_rate;
          wma_cfg->encoder_options  = wma_spec->encoder_options;
          wma_cfg->bytes_per_second = wma_spec->bytes_per_second;
          wma_cfg->virtual_pkt_len  = wma_spec->virtual_pkt_len;
          wma_cfg->bits_per_sample  = wma_spec->bits_per_sample;
          wma_cfg->advanced_encoder_opt1  = wma_spec->advanced_encoder_opt1;
          wma_cfg->channel_mask  = wma_spec->channel_mask;
          wma_cfg->format_tag  = wma_spec->format_tag;
          wma_cfg->advanced_encoder_opt2  = wma_spec->advanced_encoder_opt2;
          break;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
#ifdef FEATURE_AMR_WB_AUDIO_DEC
        case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC*/
        case AUDMAIN_AMR_WBPLUS_TYPE:
          /* Setup basic fields relevant to AMR-WB+ */
          tone_status_ptr->type           = SND_TYPE_AMR_WBPLUS;
          tone_status_ptr->audfmt_buf     = NULL;
          tone_status_ptr->audfmt_adec_cb = snd_audfmt_adec_int;
          tone_status_ptr->audfmt_adec    = (void *) snd_ptr;

          /* Setup DSP song specific configuration */
          amrwbp_cfg = &tone_status_ptr->audfmt_dsp_config.amr_wbp_config;

          amrwbp_cfg->sample_rate = snd_ptr->audfmt_song_cfg->amr_wb_config.
                                                    amr_wb_config.sample_rate;
          amrwbp_cfg->num_channels = snd_ptr->audfmt_song_cfg->amr_wb_config.
                                                  amr_wb_config.dec_chan_mode;

          /* Setup A/V sync when requested by the client */
          if(snd_ptr->sound_ptr->type == SND_FORMAT_SOUND) {
            av_sync = &snd_ptr->sound_ptr->format_sound.av_sync;
            if((av_sync->av_sync_interval != 0) &&
               (av_sync->av_sync_cb_func != NULL) &&
               (av_sync->av_sync_mode < SND_AV_SYNC_INVALID)) {
              tone_status_ptr->av_sync_mode     = av_sync->av_sync_mode;
              tone_status_ptr->av_sync_interval = av_sync->av_sync_interval;
              tone_status_ptr->av_sync_cb       = snd_av_sync_int_cb;
            }
          }
          break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

        default:
          break;
      }

      break;
#endif /* FEATURE_AUDIO_FORMAT */

    case SND_INVALID_SOUND:
    default:
      ERR_FATAL("Sound type not supported!!!",0,0,0);
  }

#ifdef FEATURE_AUDIO_FORMAT
  if((snd_ptr->sound_ptr->type != SND_AUDIO_FORMAT_SOUND) &&
     (snd_ptr->sound_ptr->type != SND_FORMAT_SOUND))
  {
    /* Pick up any audio format command buffer and send it to the DSP
    ** since audio format sound is not highest priority but we still
    ** want the DSP to get the proper commands.
    */
    tone_status_ptr->audfmt_buf = snd_audfmt_buf;
#endif /* FEATURE_AUDIO_FORMAT */
    if(tone == SND_FREQ) {
      /*lint -esym(644,flexible_tone) Tell Lint we know flexible_tone is not 
      ** initialized    
      */
      /* It is only possible to come through this point if flexible_tone */
      /* has been initilaized.                                           */
      /* If SND_FREQ, use frequencies from freq tone structure           */
      tone_status_ptr->freq_pair.freq_hi = flexible_tone.freq_hi;
      tone_status_ptr->freq_pair.freq_lo = flexible_tone.freq_lo;
      tone_status_ptr->type              = SND_TYPE_TONE;
    } else 
#ifdef FEATURE_VOICE_PLAYBACK
    if (tone == SND_VOICE_PROMPT) {
      tone_status_ptr->type = SND_TYPE_VOICE_PROMPT;
    } else
#endif
    if (tone < SND_LAST_CONTROL_TONE) {
      /* If current tone is a control tone, then set type to silence
         Since SND_FIRST_CONTROL_TONE is equal to ZERO, the unsigned
         comparison tone >= SND_FIRST_CONTROL_TONE is unnecessary 
         because it is always true.
         The actual comparison should be: 
         If (tone >= SND_FIRST_CONTROL_TONE && tone < SND_LAST_CONTROL_TONE) 
         However removing the first comparison also removes compiler warnings
      */
      tone_status_ptr->type = SND_TYPE_SILENCE;
    } else {
      /* Look up the tone in the tone database */
      (void) tdb_get_voc_tone_freq(tone, &dtmf);
      tone_status_ptr->freq_pair.freq_hi = dtmf.hi;
      tone_status_ptr->freq_pair.freq_lo = dtmf.lo;
      tone_status_ptr->type              = SND_TYPE_TONE;
    }

    MSG_MED("TONE: %x, %x", tone_status_ptr->freq_pair.freq_hi, 
                            tone_status_ptr->freq_pair.freq_lo, 0);

#ifdef FEATURE_AUDIO_FORMAT
  }
#endif

  tone_status_ptr->aux_type = SND_TYPE_SILENCE;
  if(aux_ptr != NULL) {
    tone = SND_SILENCE;
    switch(aux_ptr->sound_ptr->type) {
      case SND_COMPACT_SOUND:
        compact_tone = (((snd_compact_sound_type *)
                        (aux_ptr->sound_ptr))->tone_array)[aux_ptr->tone_index];
        tone         = compact_tone.tone;
        break;

#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif

      case SND_FLEXIBLE_SOUND:
        flexible_tone = (((snd_flexible_sound_type *)
                        (aux_ptr->sound_ptr))->tone_array)[aux_ptr->tone_index];
        tone          = flexible_tone.tone;
        break;

#ifdef FEATURE_VOICE_PLAYBACK
      case SND_TONE_AND_VOICE:
        compact_tone = (((snd_tone_and_voice_sound_type *)
                        (aux_ptr->sound_ptr))->tone_array)[aux_ptr->tone_index];
        tone         = compact_tone.tone;
        break;
#endif
    }

    if((tone > SND_LAST_CONTROL_TONE)
#ifdef FEATURE_VOICE_PLAYBACK
       && (tone != SND_VOICE_PROMPT)
#endif /* FEATURE_VOICE_PLAYBACK */
      ) {
      tone_status_ptr->aux_type = SND_TYPE_TONE;
    }
  }

  /* Setup the tone's path. If the path is not specified,
  ** take the current voice path. This is a temporary value
  ** because the path may change again if the
  ** sound class specifies a generator on a different path.
  */
  if (snd_ptr->device == SND_DEVICE_CURRENT) {
    tone_status_ptr->device = snd_voice_path_out.device;
  } else {
    tone_status_ptr->device = snd_ptr->device;
  }
} /* end of snd_setup_tone */

/* <EJECT> */
#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif /* FEATURE_REMOTE_SOUND */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_expire

DESCRIPTION
  Free up and dispose of a sound which is done being processed.  If the
  task that started the sound wanted a signal when done, send the signal.

DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
LOCAL void snd_expire
(
  snd_array_idx_type snd_idx
    /* Index of sound to expire */
)
{
  snd_sound_status_type  *snd_array_ptr;
    /* Shorthand pointer for snd_array[snd_idx] */
#ifdef FEATURE_AUDIO_FORMAT
  audmain_file_type      file_type;
#endif

  /* Get a pointer to the indicated snd_array entry and set it "not active"
  */
  snd_array_ptr         = &(snd_array[ snd_idx ]);
  snd_array_ptr->hold   = FALSE;

  if(snd_array_ptr->active) {
    snd_array_ptr->active = FALSE;
    snd_array_ptr->age_func_ptr = NULL;
    
#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif
  }

#ifdef FEATURE_AUDIO_FORMAT
  if((snd_array[snd_idx].sound_ptr != NULL) &&
     ((snd_array[snd_idx].sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_array[snd_idx].sound_ptr->type == SND_FORMAT_SOUND))) {

    file_type = audmain_get_parse_type(snd_array[snd_idx].audfmt_ctl);
#ifdef FEATURE_QCP
    if(file_type == AUDMAIN_QCP_TYPE) {
      sndhw_cleanup_vp();    /* Call voc function to end voice prompt playback */
      sndhw_abort_vp();
      sndhw_ppq_free_all();  /* Clear local queue */
      sndhw_pq_free_all();   /* Clear local queue */
    }
#endif

    /* Reset any fast-forward/rewind mechanisms now that sound is expired. */
    snd_af_ff_ctl = NULL;
    audmain_reset();

    switch(file_type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        (void) audmain_get_final_cmd(snd_array[snd_idx].audfmt_buf);
        sndhw_audfmt_cmd(snd_array[snd_idx].audfmt_buf);
        break;
#endif /* FEATURE_MIDI */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        audmain_cleanup_vbr(AUDMAIN_AAC_TYPE);
#endif
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
        sndhw_audfmt_adec_pause(FALSE);
        break;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */
    }

   if(snd_array[snd_idx].wave_resampler == TRUE){
     snd_array[snd_idx].wave_resampler = FALSE;	
    }
    sndhw_audfmt_cleanup();
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* Before expiring this snd, lets tell the SBC codec that the
    playback is done. This will enable the encoder to flush the remaining
    buffers. */
    if ((snd_voice_path_out.device == SND_DEVICE_BT_A2DP_HEADSET)||
        (snd_voice_path_out.device == SND_DEVICE_BT_A2DP_SCO_HEADSET))
    {
      audiosbc_audio_flush(TRUE);
    }
#endif /* FEATURE_SBC_CODEC */

    /* Zero out audio format timer. */
    qw_set(snd_audfmt_interval, 0, 0);
    snd_array[snd_idx].sound_ptr = NULL;
  }
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_VOICE_PLAYBACK
  /* Need to clean up if expiring voice prompt */
  if(((snd_array[snd_idx].sound_ptr != NULL) &&
      (snd_array[snd_idx].sound_ptr->type == SND_TONE_AND_VOICE)) ||
     (snd_idx == SND_PLAYBACK)) {
    sndhw_cleanup_vp();    /* Call voc function to end voice prompt playback */
    sndhw_ppq_free_all();  /* Clear local queue */
    sndhw_pq_free_all();   /* Clear local queue */
    snd_array[snd_idx].sound_ptr = NULL;
    sndhw_audfmt_cleanup();
  }
#endif
#if defined (FEATURE_AVS_BT_SCO_REWORK)
  if((snd_array[snd_idx].sound_ptr != NULL) 
#ifdef FEATURE_VOICE_PLAYBACK
     && (snd_array[snd_idx].sound_ptr->type != SND_TONE_AND_VOICE) 
#endif
#ifdef FEATURE_AUDIO_FORMAT
     && (snd_array[snd_idx].sound_ptr->type != SND_AUDIO_FORMAT_SOUND) 
     && (snd_array[snd_idx].sound_ptr->type != SND_FORMAT_SOUND) 
#endif
     )
  {
    sndhw_cleanup_tone(); 
  }
#endif  /*AVS_BT_SCO_REWORK*/

  /* If the call back is non-NULL, signal the task that the tone is done
  */
  if ( snd_array_ptr->callback_ptr != NULL ) {
    MSG_MED("Expire %d , status %d", snd_idx, SND_PLAY_DONE, 0);
    snd_array_ptr->callback_ptr(snd_array_ptr->client_data, SND_PLAY_DONE);
    snd_array_ptr->callback_ptr = NULL;
  }

  /* Clear the buffer underrun flag */
  snd_array_ptr->buffer_underrun_flag = FALSE;

} /* end of snd_expire */
/* <EJECT> */
#if (defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_VOICE_PLAYBACK))
/*===========================================================================

FUNCTION snd_highest_priority

DESCRIPTION
  This function determines if the sound_index is the highest priority sound 
  with voice prompts and/or midi and expires any lower priority conflicting
  sounds.

DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean snd_highest_priority (
  snd_array_idx_type sound_index
) {
  int                counter;
  boolean            ret_val = TRUE;

#ifdef FEATURE_VOICE_PLAYBACK
  /* Expire SND_PLAYBACK because this sound has a VP */
  if(snd_array[SND_PLAYBACK].active) {
    snd_expire( SND_PLAYBACK );
  } 
  else 
#endif
  {
    for(counter = SND_SND_LO; counter <= SND_SND_HI; counter++) {
      if((counter != sound_index) && (snd_array[counter].active) &&
         (
#if (defined(FEATURE_AUDIO_FORMAT) && defined(FEATURE_VOICE_PLAYBACK))
          (snd_array[counter].sound_ptr->type == SND_TONE_AND_VOICE) ||
          (snd_array[counter].sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
          (snd_array[counter].sound_ptr->type == SND_FORMAT_SOUND)
#elif defined(FEATURE_VOICE_PLAYBACK)
          (snd_array[counter].sound_ptr->type == SND_TONE_AND_VOICE)
#elif defined(FEATURE_AUDIO_FORMAT)
          (snd_array[counter].sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
          (snd_array[counter].sound_ptr->type == SND_FORMAT_SOUND)
#endif
         ))
      {

        if(counter < sound_index) {
          /* if lower priority sound with voice prompt playing, then expire */
          snd_expire( (snd_array_idx_type) counter );
        } else {
          ret_val = FALSE;
        }
        break;
      }
    }
  }
  return(ret_val);
}
#endif /* FEATURE_AUDIO_FORMAT || FEATURE_VOICE_PLAYBACK */
/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION snd_enqueue_vp

DESCRIPTION
  Queues voice prompts in local queues for later processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_ppq_pq_return_type snd_enqueue_vp (
  voc_pb_header_type    *vp,
  const void            *client_data,
  snd_cb_func_ptr_type  cb_func
) {
  snd_ppq_pq_return_type ret_val = SND_PPQ_PQ_SUCCESS;
  word                   vp_num;

  if(vp != NULL) {
    if( (ret_val = sndhw_ppq_enqueue( vp, client_data, &vp_num )) == 
        SND_PPQ_PQ_FAIL ) {
      MSG_ERROR("Snd prompt ptr queue is full!", 0, 0, 0);
      if(cb_func != NULL) {
        cb_func (client_data, SND_PROMPT_BUSY);
      }
    }

    /* Enqueue index onto intermediate queue */
    if((ret_val == SND_PPQ_PQ_SUCCESS) &&
       (ret_val = sndhw_pq_enqueue(vp_num)) == SND_PPQ_PQ_FAIL) {
      /* should never reach this point */
      ERR("Snd prompt queue full!!!",0,0,0);
      if ( cb_func != NULL ) {
        cb_func (client_data, SND_FAILURE);
      }
    }
  }
  return(ret_val);
}
#endif /* FEATURE_VOICE_PLAYBACK */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION snd_get_midi_out_enabled_flag 

DESCRIPTION
  This function returns the state of the MIDI OUT device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean snd_get_midi_out_enabled_flag (void)
{
  return (snd_midi_out_enabled != SND_MIDI_OUT_STATE_DISABLED);
}
#endif /* FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION snd_audfadec_int 

DESCRIPTION
  This function parses and sends QCP files for output to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_GASYNTH_INTERFACE
void snd_audfadec_int (void *ctl)
{
  audmain_adec_cmd_type adec_cmd[AUDMAIN_ADEC_MAX_CHANNELS];
  voc_adec_cmd_type     voc_adec_cmd[AUDMAIN_ADEC_MAX_CHANNELS];
  uint32                adec_buf1_used[AUDMAIN_ADEC_MAX_CHANNELS];
  uint32                adec_buf2_used[AUDMAIN_ADEC_MAX_CHANNELS];
  uint8                 buf_usage_mask[AUDMAIN_ADEC_MAX_CHANNELS];
  audmain_status_type   status;
  boolean               do_write = FALSE;
  uint32                counter;
  static uint32         adec_reset = 0;

  snd_adec_mode_type    adec_type = SND_ADEC_MODE_MAX;
  
  uint32                num_written =0;     
  uint8                 iteration = 0;
  
  memset((uint8*) adec_cmd, 0,
         sizeof(audmain_adec_cmd_type) * AUDMAIN_ADEC_MAX_CHANNELS);

#ifdef FEATURE_GASYNTH_INTERFACE
 iteration = 0;
#else
 iteration = 1;
#endif

#ifdef FEATURE_ANIMATED_AAC
  (void) rex_clr_timer( &snd_aac_timer );

  if( audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC){
    if( audmain_get_pause(ctl) == TRUE ){
      return;
    }
    iteration = 0;
  }

#endif

do{
  iteration++;
  status = audmain_adec_parse(ctl, adec_cmd);

  if(status == AUDMAIN_STATUS_SUCCESS) {
    counter = AUDMAIN_ADEC_MAX_CHANNELS;
#ifdef FEATURE_ANIMATED_AAC
    if( audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC){
      adec_type = SND_ADEC_MODE_QTUNES;
    } else
#endif
    {
      adec_type = SND_ADEC_MODE_QSYNTH;
    }
    while(counter--) {
      if(adec_cmd[counter].adec_reset) {
        adec_reset |= (1<<counter);
      }
      adec_buf1_used[counter] = adec_cmd[counter].adec_num_buf1;
      adec_buf2_used[counter] = adec_cmd[counter].adec_num_buf2;
      buf_usage_mask[counter] = adec_cmd[counter].buf_usage_mask;
      if((adec_buf1_used[counter] + adec_buf2_used[counter]) > 0) {
        do_write = TRUE;
      } 
      voc_adec_cmd[counter].buf1    = adec_cmd[counter].adec_buf1;
      voc_adec_cmd[counter].length1 = adec_cmd[counter].adec_num_buf1;
      voc_adec_cmd[counter].buf2    = adec_cmd[counter].adec_buf2;
      voc_adec_cmd[counter].length2 = adec_cmd[counter].adec_num_buf2;
      voc_adec_cmd[counter].buf_usage_mask = &buf_usage_mask[counter];
    }

    if((audmain_get_adec_mode() != AUDMAIN_ADEC_MODE_AAC) ||
       ((audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC) && 
        sndhw_audfmt_adec_int_pending())) {
      if(do_write) {
        MSG_MED("snd_audfadec_int: Confirm to write", 0, 0, 0);
        if(adec_reset != 0) {
          sndhw_audfmt_adec_reset(adec_reset);
          adec_reset = 0;
        }

        (void)sndhw_audfmt_adec_data(adec_type, voc_adec_cmd);

        counter = AUDMAIN_ADEC_MAX_CHANNELS;
        num_written = 0;
        while(counter--) {
          if((adec_buf1_used[counter] + adec_buf2_used[counter]) > 0) {
            adec_buf1_used[counter] -= voc_adec_cmd[counter].length1;
            adec_buf2_used[counter] -= voc_adec_cmd[counter].length2;
            num_written += (adec_buf1_used[counter] + adec_buf2_used[counter]);
          }

        }

        (void) audmain_adec_ppbuf_adjust(ctl, adec_buf1_used, 
                                         adec_buf2_used, buf_usage_mask);
      }
    } 
#ifdef FEATURE_ANIMATED_AAC
    if( (do_write != 0) && (num_written == 0) &&
        (audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC) ){
      rex_set_timer( &snd_aac_timer, 20);
    }
#endif
  }
  } while( iteration < 2 );
}
#else /* FEATURE_GASYNTH_INTERFACE */
void snd_audfadec_int (void *ctl)
{
  audmain_adec_cmd_type adec_cmd[AUDMAIN_ADEC_MAX_CHANNELS];
  voc_adec_cmd_type     voc_adec_cmd[AUDMAIN_ADEC_MAX_CHANNELS];
  audmain_status_type   status;
  boolean               do_write = FALSE;
  uint32                num[AUDMAIN_ADEC_MAX_CHANNELS], counter;
  static uint32         adec_reset = 0;

  snd_adec_mode_type    adec_type = SND_ADEC_MODE_MAX;
  
  uint32                num_written =0;     
  
  memset((uint8*) adec_cmd, 0,
         sizeof(audmain_adec_cmd_type) * AUDMAIN_ADEC_MAX_CHANNELS);

#ifdef FEATURE_ANIMATED_AAC
  (void) rex_clr_timer( &snd_aac_timer );

  if( audmain_get_pause(ctl) == TRUE && 
      audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC){
    return;
  }
#endif

  status = audmain_adec_parse(ctl, adec_cmd);

  if(status == AUDMAIN_STATUS_SUCCESS) {
    counter = AUDMAIN_ADEC_MAX_CHANNELS;
#ifdef FEATURE_ANIMATED_AAC
    if( audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC){
      adec_type = SND_ADEC_MODE_QTUNES;
    } else
#endif
    {
      adec_type = SND_ADEC_MODE_QSYNTH;
    }
    while(counter--) {
      if(adec_cmd[counter].adec_reset) {
        adec_reset |= (1<<counter);
      }
      num[counter] = adec_cmd[counter].adec_num_buf1 +
                     adec_cmd[counter].adec_num_buf2;
      if(num[counter] != 0) {
        do_write = TRUE;
      }
      voc_adec_cmd[counter].buf1    = adec_cmd[counter].adec_buf1;
      voc_adec_cmd[counter].length1 = adec_cmd[counter].adec_num_buf1;
      voc_adec_cmd[counter].buf2    = adec_cmd[counter].adec_buf2;
      voc_adec_cmd[counter].length2 = adec_cmd[counter].adec_num_buf2;
    }

    if(sndhw_audfmt_adec_int_pending()) {
      if(do_write) {
        if(adec_reset != 0) {
          sndhw_audfmt_adec_reset(adec_reset);
          adec_reset = 0;
        }

        (void)sndhw_audfmt_adec_data(adec_type, voc_adec_cmd);

        counter = AUDMAIN_ADEC_MAX_CHANNELS;
        num_written = 0;
        while(counter--) {
          if(num[counter] != 0) {
            num_written += num[counter] - (voc_adec_cmd[counter].length1 +
                                           voc_adec_cmd[counter].length2);
            num[counter] = num[counter] - (voc_adec_cmd[counter].length1 +
                                           voc_adec_cmd[counter].length2);
          }
        }


        (void) audmain_adec_adjust(ctl, num);
      }
    } 

#ifdef FEATURE_ANIMATED_AAC
    if( (do_write != 0) && (num_written == 0) &&
        (audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC) ){
      rex_set_timer( &snd_aac_timer, 20);
    }
#endif
  }
}
#endif /* FEATURE_GASYNTH_INTERFACE */
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION snd_check_wa_audfadec_int 

DESCRIPTION
  This function parses and sends QCP/ADPCM contents in CMX files for output 
  to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean snd_check_wa_audfadec_int(void) {
  int i;
  for (i=0; i<SND_ARRAY_SIZE; i++) {
    if ((snd_array[i].active) &&
        ((snd_array[i].sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
         (snd_array[i].sound_ptr->type == SND_FORMAT_SOUND)) &&
        (audmain_get_parse_type(snd_array[i].audfmt_ctl) == AUDMAIN_MIDI_TYPE))
    {
      snd_audfadec_int ( snd_array[i].audfmt_ctl );
      return (TRUE);
    }
  }
  return (FALSE);
}
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
#ifdef FEATURE_ADEC
/*===========================================================================

FUNCTION snd_audfmt_adec_int_req

DESCRIPTION


DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
snd_status_type snd_audfmt_adec_int_req (
  snd_sound_status_type *snd_ptr
) {
  snd_audfmt_sound_type *audfmt_sound_ptr;
  snd_adec_mode_type    adec_type = SND_ADEC_MODE_MAX;
  audmain_adec_cmd_type *adec_cmd;
  voc_adec_cmd_type     voc_adec_cmd[AUDMAIN_ADEC_MAX_CHANNELS];
  audmain_file_type     file_type;
  uint32                num;
  snd_status_type       ret_val = SND_SUCCESS;

  if((snd_ptr->active) &&
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {

    if(sndhw_audfmt_adec_int_pending()) {
      adec_cmd = &(snd_ptr->audfmt_buf->adec_cmd);
      if((adec_cmd->adec_num_buf1 != 0) ||
         (adec_cmd->adec_num_buf2 != 0)) {

        num = adec_cmd->adec_num_buf1 + adec_cmd->adec_num_buf2;

        memset(voc_adec_cmd, 0, sizeof(voc_adec_cmd));

        voc_adec_cmd[0].buf1    = adec_cmd->adec_buf1;
        voc_adec_cmd[0].length1 = adec_cmd->adec_num_buf1;
        voc_adec_cmd[0].buf2    = adec_cmd->adec_buf2;
        voc_adec_cmd[0].length2 = adec_cmd->adec_num_buf2;

        file_type = audmain_get_parse_type(snd_ptr->audfmt_ctl);

        switch (file_type) {
#ifdef FEATURE_QTUNES_COMMON
#ifdef FEATURE_MP3
          case AUDMAIN_MP3_TYPE:
#endif /* FEATURE_MP3 */
#ifdef FEATURE_AAC
          case AUDMAIN_AAC_TYPE:
#endif /* FEATURE_AAC */
#ifdef FEATURE_REAL_AUDIO
          case AUDMAIN_RA_TYPE:
#endif /* FEATURE_REAL_AUDIO */
#ifdef FEATURE_WMA
          case AUDMAIN_WMA_TYPE:
          case AUDMAIN_WMA_PRO_TYPE:
#endif 
#ifdef FEATURE_AMR_WB_AUDIO_DEC
          case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
          case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
            adec_type = SND_ADEC_MODE_QTUNES;
            break;
#endif /* FEATURE_QTUNES_COMMON */

          default:
            adec_type = SND_ADEC_MODE_MAX;
            break;
        }

        if((ret_val = sndhw_audfmt_adec_data(adec_type, voc_adec_cmd))
           != SND_SUCCESS){
          return ret_val;
        }

        num = num - (voc_adec_cmd[0].length1 + voc_adec_cmd[0].length2);

        (void) audmain_adec_adjust(snd_ptr->audfmt_ctl, &num);
      } else {
        if(sndhw_audfmt_adec_pcm_stopped()) {
          /* Data buffer underflow, need more data to continue. */
          MSG_HIGH("Adec Buffer underflow",0,0,0);
          audfmt_sound_ptr = (snd_audfmt_sound_type *) snd_ptr->sound_ptr;
          audmain_play_cb(snd_ptr->audfmt_ctl,
                          &(audfmt_sound_ptr->audfmt_handle), 
                          SND_CMX_AF_FA_DELAY, snd_audfmt_adjusted_ms, NULL);
        }
      }
    }
  }

  return ret_val;
}
#endif /* FEATURE_ADEC */

#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION snd_audfmt_vp_queue

DESCRIPTION
  Queues voice prompts.

DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
void snd_audfmt_vp_queue ( snd_sound_status_type *snd_ptr )
{

  if(audmain_get_parse_type(snd_ptr->audfmt_ctl) == AUDMAIN_QCP_TYPE) {
    if(snd_ptr->audfmt_buf->prompt1 != NULL) {
      /* Enqueue voice prompt pointer */
      if( snd_enqueue_vp( snd_ptr->audfmt_buf->prompt1,
                          snd_ptr->audfmt_buf->prompt1, NULL) == 
          SND_PPQ_PQ_FAIL ) {
        ERR("Snd prompt ptr queue is full!", 0, 0, 0);
      } else {
        snd_ptr->audfmt_buf->prompt1 = NULL;
      }
    }
    if(snd_ptr->audfmt_buf->prompt2 != NULL) {
      /* Enqueue voice prompt pointer */
      if( snd_enqueue_vp( snd_ptr->audfmt_buf->prompt2,
                          snd_ptr->audfmt_buf->prompt2, NULL) == 
          SND_PPQ_PQ_FAIL ) {
        ERR("Snd prompt ptr queue is full!", 0, 0, 0);
      } else {
        snd_ptr->audfmt_buf->prompt2 = NULL;
      }
    }
    if(snd_ptr->audfmt_buf->prompt3 != NULL) {
      /* Enqueue voice prompt pointer */
      if( snd_enqueue_vp( snd_ptr->audfmt_buf->prompt3,
                          snd_ptr->audfmt_buf->prompt3, NULL) ==
          SND_PPQ_PQ_FAIL ) {
        ERR("Snd prompt ptr queue is full!", 0, 0, 0);
      } else {
        snd_ptr->audfmt_buf->prompt3 = NULL;
      }
    }
    if(snd_ptr->audfmt_buf->prompt4 != NULL) {
      /* Enqueue voice prompt pointer */
      if( snd_enqueue_vp( snd_ptr->audfmt_buf->prompt4,
                          snd_ptr->audfmt_buf->prompt4, NULL) ==
          SND_PPQ_PQ_FAIL ) {
        ERR("Snd prompt ptr queue is full!", 0, 0, 0);
      } else {
        snd_ptr->audfmt_buf->prompt4 = NULL;
      }
    }
  }
}

/*===========================================================================

FUNCTION snd_age_qcp

DESCRIPTION
  Updates/ages the status of the QCP file sound specified by which_snd.  
  This includes expiring the sound after the last tone has been played.

DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
LOCAL void snd_age_qcp
(
  snd_array_idx_type which_snd,
    /* Which sound is being processed (voice prompt, sound, or tone) */

  uint32 ms_elapsed_time,
    /* Number of milliseconds which have elapsed since the last update */

  word change_mask
    /*lint -esym(715,change_mask) Tell Lint we know change_mask parameter 
    ** is not used */
    /* Mask of Changes found by snd_change() */
)
{
  snd_sound_status_type *snd_ptr;          /* Pointer to snd_array[which_snd] */
  snd_audfmt_sound_type *audfmt_sound_ptr;
  audmain_status_type   audfmt_status;
  uint32                index;
  snd_status_type       ret_status;

  /* Get a pointer to the sound status for the sound being aged.  */
  snd_ptr = &(snd_array[ which_snd ]);
  if(((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND)) &&
      !audmain_get_pause(snd_ptr->audfmt_ctl) ) {

    if(audmain_get_parse_type(snd_ptr->audfmt_ctl) == AUDMAIN_QCP_TYPE) {

      if(snd_ptr->buffer_underrun_flag) {
        ms_elapsed_time = 0;
        if((snd_ptr->audfmt_buf->prompt1 != NULL) ||
           (snd_ptr->audfmt_buf->prompt2 != NULL) ||
           (snd_ptr->audfmt_buf->prompt3 != NULL) ||
           (snd_ptr->audfmt_buf->prompt4 != NULL)) {
          snd_ptr->buffer_underrun_flag = FALSE;
        }
      }

      snd_audfmt_vp_queue(snd_ptr);

      for(index = 0; index < snd_vp_sig_num; index++) {
        snd_ptr->audfmt_buf->prompt1 = 
                              (voc_pb_header_type *) snd_vp_client_data[index];

        audfmt_status = audmain_parse( snd_ptr->audfmt_ctl, 
                                       snd_ptr->audfmt_buf,
                                       &(snd_ptr->time_remain_ms) );

        if(audfmt_status == AUDMAIN_STATUS_BUFF_ERR) {
          snd_ptr->buffer_underrun_flag = TRUE;

          audfmt_sound_ptr = (snd_audfmt_sound_type *) snd_ptr->sound_ptr;
          audmain_play_cb(snd_ptr->audfmt_ctl,
                          &(audfmt_sound_ptr->audfmt_handle),
                          SND_CMX_AF_FA_DELAY, snd_audfmt_adjusted_ms, NULL);
        } else if (((audfmt_status == AUDMAIN_STATUS_ERROR)           ||
                    (audfmt_status == AUDMAIN_STATUS_DATA_ERR)        ||
                    (audfmt_status == AUDMAIN_STATUS_DATA_ACCESS_ERR) ||
                    (audfmt_status == AUDMAIN_STATUS_RESOURCE_ERR)    ||
                    (audfmt_status == AUDMAIN_STATUS_FAILURE)) ||
                   ((audfmt_status != AUDMAIN_STATUS_DONE) && 
	            (snd_vp_err_status == TRUE))) {

          /* Wait until all queued voice prompts complete before expiring the
          ** playback, otherwise playback could terminate before the output
          ** device has warmed-up
          */
          if ((snd_vp_status[index] != VOC_END_Q_NOT_EMPTY) &&
              (sndhw_pq_empty() == TRUE)) {
            
            /* Notify the client that playback is abnormally terminating */
            if (snd_ptr->callback_ptr != NULL) {
          
              switch (audfmt_status) {
                case AUDMAIN_STATUS_DATA_ERR:
                  ret_status = SND_ABORT_DATA_ERR;
                  break;
                case AUDMAIN_STATUS_DATA_ACCESS_ERR:
                  ret_status = SND_ABORT_DATA_ACCESS_ERR;
                  break;
                case AUDMAIN_STATUS_RESOURCE_ERR:
                  ret_status = SND_ABORT_RESOURCE_ERR;
                  break;
                case AUDMAIN_STATUS_ERROR:
                case AUDMAIN_STATUS_FAILURE:
                  ret_status = SND_ABORT;
                  break;

                case AUDMAIN_STATUS_MAX:
                default:
                  /* Problem in the code if this status is encountered */
                  MSG_ERROR("Should not run into this status", 0,0,0);
                  ret_status = SND_ABORT;
                  break;
              }

              MSG_MED("Expire %d , status %d", which_snd, ret_status, 0);
              snd_ptr->callback_ptr(snd_ptr->client_data, ret_status);
              snd_ptr->callback_ptr = NULL;
            }

            snd_expire(which_snd); /* Expire the sound */
            return;
          } else {
            /* Prevent new prompts from playing in the error state */
            snd_ptr->audfmt_buf->prompt1 = NULL;
          }
        } else if ((snd_vp_status[index] == VOC_END_PB) &&
                   (sndhw_pq_empty() == TRUE) &&
                   (audfmt_status == AUDMAIN_STATUS_DONE)) {

          snd_expire(which_snd); /* Expire the sound */
          return;
        }
      }

      snd_audfmt_elapsed_ms  += ms_elapsed_time;
      snd_audfmt_adjusted_ms  = snd_audfmt_elapsed_ms;
      if(snd_audfmt_elapsed_ms >= snd_audfmt_next_update) {
        snd_audfmt_next_update += SND_AUDFMT_TICK_INTERVAL;

        if(snd_ptr->callback_ptr != NULL) {
          snd_ptr->callback_ptr(snd_ptr->client_data, SND_AF_TIME_TICK);
        }
      }

      if(!snd_ptr->buffer_underrun_flag) {
        snd_ptr->time_remain_ms = snd_audfmt_next_update -
                                                  snd_audfmt_elapsed_ms;
      } else {
        snd_ptr->time_remain_ms = SND_MAX_PERIOD;
      }
    }
  }
}
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_QSYNTH_ADPCM 
/*===========================================================================

FUNCTION snd_adec_adpcm_stop

DESCRIPTION
  This function stops playback on the specified ADPCM channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_adec_adpcm_stop (
  uint32                     channel
) {
  voc_adec_adpcm_stop(channel);
}
#endif /* FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#ifdef FEATURE_GASYNTH_INTERFACE 
/*===========================================================================

FUNCTION snd_voc_adec_start

DESCRIPTION
  This function starts the audio decoder for QCP or AMR-NB playback
  
DEPENDENCIES
  NONE
  
RETURN VALUE
  A value of snd_status_type to indiciate whether DSP is properly
  configured to accept vocoder data. 

SIDE EFFECTS
  NONE
===========================================================================*/
snd_status_type snd_voc_adec_start (
  uint32                        channel,
  snd_voc_adec_enum_type        voc_type,
  snd_adec_start_mode_enum_type start_mode
) {
  snd_status_type               retval = SND_FAILURE;
  voc_status_type               adec_result;
  voc_adec_start_mode_enum_type voc_mode = VOC_ADEC_START_MODE_NORMAL;
  voc_adec_voc_enum_type        adec_type = VOC_ADEC_VOC_QCP;
  
  if(start_mode == SND_ADEC_START_MODE_DELAYED) {
    voc_mode = VOC_ADEC_START_MODE_DELAYED;
  }

  if (voc_type == SND_VOC_ADEC_AMR) {
    adec_type = VOC_ADEC_VOC_AMR;
  }

  adec_result = voc_adec_voc_start(channel, adec_type, voc_mode);
  if (adec_result == VOC_DONE_S) {
    retval = SND_SUCCESS;
  } else if (adec_result == VOC_CMD_BUSY_S) {
    retval = SND_PROMPT_BUSY;
  }

  return (retval);
}

/*===========================================================================

FUNCTION snd_adec_data_consumed

DESCRIPTION
  This function detects decoder has finished consuming data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - data_is_needed flag is set
  FALSE - data_is_needed flag is not set 

SIDE EFFECTS
  NONE

===========================================================================*/
boolean snd_adec_data_consumed(
  uint8 channel
) {
  return (voc_adec_data_consumed(channel));
}
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION snd_adec_adpcm_start

DESCRIPTION
  This function initializes the specified ADPCM channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type snd_adec_adpcm_start (
  uint32                        channel,
  snd_adec_adpcm_play_enum_type adpcm_play_type,
  snd_adec_adpcm_format_type    *format,
  snd_adec_start_mode_enum_type start_mode
) {
  snd_status_type               ret_val = SND_FAILURE;
  voc_status_type               voc_status;
  voc_adpcm_spec_type           spec;
  voc_adec_start_mode_enum_type voc_mode = VOC_ADEC_START_MODE_NORMAL;

  if(start_mode == SND_ADEC_START_MODE_DELAYED) {
    voc_mode = VOC_ADEC_START_MODE_DELAYED;
  }

  spec.bits_per_sample = format->bits_per_sample;
  spec.block_length    = format->frame_size;
  spec.sampling_rate   = format->sample_rate;
  spec.num_channels    = format->channels;

  switch(format->mode) {
    case SND_ADPCM_SIGNED_PCM_MODE:
      spec.type          = VOC_ADPCM_SIGNED_PCM_MODE;
      break;
    case SND_ADPCM_UNSIGNED_PCM_MODE:
      spec.type          = VOC_ADPCM_UNSIGNED_PCM_MODE;
      break;
    case SND_ADPCM_YADPCM_MODE:
      spec.type          = VOC_ADPCM_YADPCM_MODE;
      break;
    case SND_ADPCM_ADPCM_MODE:
      spec.type          = VOC_ADPCM_ADPCM_MODE;
      break;
#ifdef FEATURE_VOC_G711
    case SND_ADPCM_WAV_ALAW_MODE:
      spec.type          = VOC_ADPCM_WAV_ALAW_MODE;
      break;
    case SND_ADPCM_WAV_MULAW_MODE:
      spec.type          = VOC_ADPCM_WAV_MULAW_MODE;
      break;
#endif /* FEATURE_VOC_G711 */
  }

  voc_status = voc_set_adpcm_params(channel,(voc_adpcm_play_enum_type)adpcm_play_type,
  	                              &spec,voc_mode);
  if ( voc_status == VOC_DONE_S){
  	ret_val = SND_SUCCESS;
  }
  
  voc_status = voc_adec_adpcm_start(channel,
                   (voc_adpcm_play_enum_type) adpcm_play_type, &spec, voc_mode);

  if(voc_status == VOC_DONE_S) {
    ret_val = SND_SUCCESS;
  } else if(voc_status == VOC_CMD_BUSY_S) {
    ret_val = SND_PROMPT_BUSY;
  }
  return(ret_val);
}
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION snd_audfmt_adec_mvol

DESCRIPTION
  This function sets the ADEC master volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type snd_audfmt_adec_mvol (
  uint16 vol
) {
  snd_status_type ret_val = SND_FAILURE;

  if(sndhw_audfmt_adec_mvol(vol)) {
    ret_val = SND_SUCCESS;
  }

  return(ret_val);
}
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION snd_audfmt_adec_chan_vol

DESCRIPTION
  Transfers the MIDI command buffer to the QDSP for processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type snd_audfmt_adec_chan_vol (
  uint32 channel,
  uint16 vol
) {
  snd_status_type ret_val = SND_FAILURE;

  if(sndhw_audfmt_adec_chan_vol(channel, vol)) {
    ret_val = SND_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_adec_chan_pan

DESCRIPTION
  Transfers the MIDI command buffer to the QDSP for processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type snd_audfmt_adec_chan_pan (
  uint32 channel,
  sint15 pan
) {
  snd_status_type ret_val = SND_FAILURE;

  if(sndhw_audfmt_adec_chan_pan(channel, pan)) {
    ret_val = SND_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_adec_chan_start

DESCRIPTION
  Starts the adec channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type snd_audfmt_adec_chan_start (
  uint32  channel
) {
  snd_status_type ret_val = SND_FAILURE;

  if(voc_adec_chan_start(channel) == VOC_DONE_S) {
    ret_val = SND_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_adec_chan_pause

DESCRIPTION
  Sets the pause flag for adec channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type snd_audfmt_adec_chan_pause (
  uint32  channel,
  boolean pause_flag
) {
  snd_status_type ret_val = SND_FAILURE;

  if(sndhw_audfmt_adec_chan_pause(channel, pause_flag)) {
    ret_val = SND_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_adec_chan_flush

DESCRIPTION
  Sends a flush command to the adec channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_audfmt_adec_chan_flush (
  uint32  channel
) {
  sndhw_audfmt_adec_flush(1 << channel);
}
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION snd_set_adec_mode

DESCRIPTION
  This function sets the audio decoder mode for the QSynth image.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void snd_set_adec_mode (
  snd_set_adec_mode_enum_type mode
) {
  sndhw_set_adec_mode(mode);
}
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#ifdef FEATURE_ANIMATED_AAC
/*===========================================================================

FUNCTION snd_set_adec_song_config

DESCRIPTION
  This function sets the song config for Animate-AAC and Codec Clocks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
boolean snd_set_adec_song_config(
  audmain_song_config_type *config, 
  snd_sample_rate_type    samplerate
){
  boolean ret_val = FALSE;

  /*Set AAC config only if the DSP stopped playing the previous frame */
  if( sndhw_audfmt_adec_pcm_stopped() == TRUE){
    sndhw_audfmt_adec_flush( 0);
    if( sndhw_set_sample_rate( samplerate) ){
      if( sndhw_aac_set_song_config( (voc_aac_config_type*)
                                 &config->header_config.aac_config) ){
        sndhw_aac_set_mpeg_header( 
          (snd_audfmt_mpeg_header_config*) &config->header_config);
        ret_val = TRUE;
      }
    }
  }else{
    (void) rex_clr_sigs(&snd_tcb, SND_AAC_RETRY_SIG);
     rex_set_timer( &snd_aac_timer, SND_AAC_RETRY_TIMER);
  }

  return ret_val;
}
#endif

/* <EJECT> */
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION snd_set_dls_config

DESCRIPTION
  This function sets the DLS configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_set_dls_config (
  snd_dls_config_type *dls_config
) {
  voc_set_dls_config((voc_dls_config_type *) dls_config); 
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_qsynth_vol_mode

DESCRIPTION
  This function sets the qysnth volume mapping configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_set_qsynth_vol_mode(
  snd_qsynth_vol_mode_enum_type mode
) {
#ifdef FEATURE_MIDI_OUT
  if (snd_midi_out_enabled == SND_MIDI_OUT_STATE_DISABLED)
#endif /* FEATURE_MIDI_OUT */
  {
    voc_set_qsynth_vol_mode((voc_qsynth_vol_mode_enum_type) mode);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_hybrid_mode

DESCRIPTION
  This function sets the hybrid mode configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_set_hybrid_mode (
  snd_audfmt_poly_mode_type hybrid_mode
) {
  uint8 trans_poly_mode = 0;               
  boolean set_poly_mode = TRUE;
#ifdef FEATURE_MIDI_OUT
  if (snd_midi_out_enabled == SND_MIDI_OUT_STATE_DISABLED)
#endif /* FEATURE_MIDI_OUT */
  {
    switch(hybrid_mode) {
      case SND_AUDFMT_POLY_MODE_0:
        trans_poly_mode = 0;
        break;
      case SND_AUDFMT_POLY_MODE_1:
        trans_poly_mode = 1;
        break;
      case SND_AUDFMT_POLY_MODE_2:
        trans_poly_mode = 2;
        break;
      case SND_AUDFMT_POLY_MODE_3:
        trans_poly_mode = 3;
        break;
      case SND_AUDFMT_POLY_MODE_4:
        trans_poly_mode = 4;
        break;
      case SND_AUDFMT_POLY_MODE_5:
        trans_poly_mode = 5;
        break;
      case SND_AUDFMT_POLY_MODE_6:
        trans_poly_mode = 6;
        break;
      case SND_AUDFMT_POLY_MODE_7:
        trans_poly_mode = 7;
        break;
      default:
        set_poly_mode = FALSE;
        break;
    }

    if (set_poly_mode == TRUE) {
      voc_set_hybrid_mode(trans_poly_mode);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_pause_seq

DESCRIPTION
  This function issues a pause command to the indictaed sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean snd_audfmt_pause_seq (
  uint32  sequence,
  boolean pause_flag
) {
  return(voc_pause_midi_seq(sequence, pause_flag) == VOC_DONE_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_set_vol_seq

DESCRIPTION
  This function issues a set volume command to the indictaed sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean snd_audfmt_set_vol_seq (
  uint32 sequence,
  uint16 vol
) {
  return(voc_set_vol_midi_seq(sequence, vol) == VOC_DONE_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_age_audfmt

DESCRIPTION
  Updates/ages the status of the midi/WebAudio sound specified by which_snd.  
  This includes expiring the sound after the last tone has been played.

DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
LOCAL void snd_age_audfmt
(
  snd_array_idx_type which_snd,
    /* Which sound is being processed (voice prompt, sound, or tone) */

  uint32 ms_elapsed_time,
    /* Number of milliseconds which have elapsed since the last update */

  word change_mask
    /*lint -esym(715,change_mask) Tell Lint we know change_mask parameter 
    ** is not used */
    /* Mask of Changes found by snd_change() */
)
{
  snd_sound_status_type *snd_ptr;          /* Pointer to snd_array[which_snd] */
  snd_audfmt_sound_type *audfmt_sound_ptr;
  audmain_status_type   audfmt_status = AUDMAIN_STATUS_SUCCESS;
  uint32                aged_ms, time_ms_adjust;

  /* Get a pointer to the sound status for the sound being aged.  */
  snd_ptr = &(snd_array[ which_snd ]);
  if((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
     (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND)) {

    audfmt_sound_ptr = (snd_audfmt_sound_type *) snd_ptr->sound_ptr;

    if(audmain_get_parse_type(snd_array[which_snd].audfmt_ctl) == 
       AUDMAIN_MIDI_TYPE) {

      aged_ms = ms_elapsed_time;
      /* Age audio format sound */
      audmain_age( snd_ptr->audfmt_ctl, &aged_ms, &time_ms_adjust );

      /* Increment elapsed time */
      snd_audfmt_elapsed_ms  += aged_ms;
      snd_audfmt_adjusted_ms += time_ms_adjust;
      if(snd_audfmt_adjusted_ms >= snd_audfmt_next_update) {
        snd_audfmt_next_update += SND_AUDFMT_TICK_INTERVAL;

        audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle), 
                        SND_CMX_AF_TIME_TICK, 0, NULL);
      }

      /* Get one time event of audio format sound */
      audfmt_status = audmain_parse( snd_ptr->audfmt_ctl, snd_ptr->audfmt_buf,
                                     &(snd_ptr->time_remain_ms) );

      if((ms_elapsed_time == 0) || (time_ms_adjust != 0)) {
        if(snd_audfmt_next_update > snd_audfmt_adjusted_ms) {
          snd_ptr->time_remain_ms = (uint16) MIN(snd_ptr->time_remain_ms, 
                              snd_audfmt_next_update - snd_audfmt_adjusted_ms);
        } else {
          snd_ptr->time_remain_ms = 0;
        }
      }

      if(audfmt_status == AUDMAIN_STATUS_DONE) {
        snd_expire( which_snd );           /* expire the sound         */
      }
    }
  }
}
#endif /* FEATURE_MIDI */

/* <EJECT> */
#if defined(FEATURE_MPEG_AUDIO) || defined(FEATURE_REAL_AUDIO) || \
    defined(FEATURE_WMA)
/*===========================================================================

FUNCTION snd_age_adec_stream

DESCRIPTION
  Updates/ages the status of the MP3/AAC/RA sound specified by which_snd.  
  This includes expiring the sound after the DSPs PCM buffers have been emptied.

DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
LOCAL void snd_age_adec_stream
(
  snd_array_idx_type which_snd,
    /* Which sound is being processed (voice prompt, sound, or tone) */

  uint32 ms_elapsed_time,
    /* Number of milliseconds which have elapsed since the last update */

  word change_mask
    /*lint -esym(715,change_mask) Tell Lint we know change_mask parameter 
    ** is not used */
    /* Mask of Changes found by snd_change() */
)
{
  snd_sound_status_type *snd_ptr;          /* Pointer to snd_array[which_snd] */
  audmain_status_type   audfmt_status = AUDMAIN_STATUS_SUCCESS;
  uint32                time_ms_adjust;
  snd_audfmt_sound_type *audfmt_sound_ptr;
  snd_status_type       ret_status;
  boolean               do_abort = FALSE; 

  /* Get a pointer to the sound status for the sound being aged.  */
  snd_ptr = &(snd_array[ which_snd ]);
  if((snd_ptr->hold == FALSE) &&
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND)) &&
      !audmain_get_pause(snd_ptr->audfmt_ctl) ) {

    switch (audmain_get_parse_type(snd_array[which_snd].audfmt_ctl)) {
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
        break;

      default:
        return;
    }

    if(snd_ptr->buffer_underrun_flag) {
      ms_elapsed_time = 0;
    }

    /* Age audio format sound */
    audmain_age( snd_ptr->audfmt_ctl, &ms_elapsed_time, &time_ms_adjust );

    /* Increment elapsed time */
    /* Manipulate the time only when the ms_elapsed_time is greater than Zero*/
    if((int32) ms_elapsed_time > 0) {
    snd_audfmt_elapsed_ms  += ms_elapsed_time;
    snd_audfmt_adjusted_ms += time_ms_adjust;
    }

    if(snd_audfmt_adjusted_ms >= snd_audfmt_next_update) {
      snd_audfmt_next_update += SND_AUDFMT_TICK_INTERVAL;
      if(snd_ptr->callback_ptr != NULL) {
        snd_ptr->callback_ptr(snd_ptr->client_data, SND_AF_TIME_TICK);
      }
    }

    if(snd_audfmt_next_update > snd_audfmt_adjusted_ms) {
      snd_ptr->time_remain_ms = (uint16) MIN(SND_AUDFMT_TICK_INTERVAL,
                            snd_audfmt_next_update - snd_audfmt_adjusted_ms);
    }

    /* Get one time event of audio format sound */
    audfmt_status = audmain_parse( snd_ptr->audfmt_ctl, snd_ptr->audfmt_buf,
                                   &(snd_ptr->time_remain_ms) );

    if(audfmt_status == AUDMAIN_STATUS_SUCCESS ||
       audfmt_status == AUDMAIN_STATUS_DONE ||
       audfmt_status == AUDMAIN_STATUS_BUFF_ERR) { 
      ret_status = snd_audfmt_adec_int_req ( snd_ptr );
      switch(ret_status){
      case SND_SUCCESS:
        break;
      case SND_ABORT_DATA_ERR:
        audfmt_status = AUDMAIN_STATUS_DATA_ERR;
        break;
      case SND_ABORT:
      default:
      audfmt_status = AUDMAIN_STATUS_ERROR;
        break;
      }
    }

    if(audfmt_status != AUDMAIN_STATUS_BUFF_ERR) {
      snd_ptr->buffer_underrun_flag = FALSE;
    }

    switch(audfmt_status) {
      case AUDMAIN_STATUS_DONE:
        if(!sndhw_audfmt_adec_pcm_stopped()) {
          /* Poll the DSP until the PCM stops */
          snd_ptr->time_remain_ms = (uint16) MIN(snd_ptr->time_remain_ms, 20);
          /* Wait until the DSP is done */
          break;
        }
        /* else expire */
        snd_expire( which_snd);
        break;

      case AUDMAIN_STATUS_MAX:
      /* Problem in the code if this status is encountered */
        MSG_ERROR("Unexpected error status", 0,0,0);
      case AUDMAIN_STATUS_ERROR:
      case AUDMAIN_STATUS_FAILURE:
        ret_status = SND_ABORT;
        do_abort = TRUE;
        break;
      case AUDMAIN_STATUS_DATA_ERR:
        ret_status = SND_ABORT_DATA_ERR;
        do_abort = TRUE;
        break;
      case AUDMAIN_STATUS_DATA_ACCESS_ERR:
        ret_status = SND_ABORT_DATA_ACCESS_ERR;
        do_abort = TRUE;
        break;
      case AUDMAIN_STATUS_RESOURCE_ERR:
        ret_status = SND_ABORT_RESOURCE_ERR;
        do_abort = TRUE;
        break;

        case AUDMAIN_STATUS_BUFF_ERR:
          if((!snd_ptr->buffer_underrun_flag) && sndhw_audfmt_adec_pcm_stopped()) {
            snd_ptr->buffer_underrun_flag = TRUE;
            MSG_HIGH("Buffer underflow",0,0,0);
            audfmt_sound_ptr = (snd_audfmt_sound_type *) snd_ptr->sound_ptr;
            audmain_play_cb(snd_ptr->audfmt_ctl,
                            &(audfmt_sound_ptr->audfmt_handle), 
                            SND_CMX_AF_FA_DELAY, snd_audfmt_adjusted_ms, NULL);
            break;
          }
        /* else fall thru and allow the data to be aged. */

      case AUDMAIN_STATUS_SUCCESS:
#ifdef FEATURE_WMA
        if((audmain_get_parse_type(snd_array[which_snd].audfmt_ctl) ==
              AUDMAIN_WMA_TYPE ) || 
            (audmain_get_parse_type(snd_array[which_snd].audfmt_ctl) ==
              AUDMAIN_WMA_PRO_TYPE)
           ){

          audmain_set_init_done(snd_ptr->audfmt_ctl);

          if (sndhw_audfmt_adec_pcm_stopped()) {

            MSG_HIGH("No PCM output",0,0,0);

            audfmt_sound_ptr = (snd_audfmt_sound_type *) snd_ptr->sound_ptr;

            audmain_play_cb(snd_ptr->audfmt_ctl,
                            &(audfmt_sound_ptr->audfmt_handle), 
                            SND_CMX_AF_FA_DELAY, snd_audfmt_adjusted_ms, NULL);
          }
          break;
        }
#endif /* FEATURE_WMA */
      default:
        /* Set init flag to done, sound can proceed to be aged. */
        audmain_set_init_done(snd_ptr->audfmt_ctl);
        break;
    }

    if(do_abort == TRUE){
      if ( snd_ptr->callback_ptr != NULL ) {
        MSG_MED("Expire %d , status %d", which_snd, ret_status, 0);
        snd_ptr->callback_ptr(snd_ptr->client_data, ret_status);
        snd_ptr->callback_ptr = NULL;
      }
      snd_expire( which_snd );
    }

    if(snd_ptr->buffer_underrun_flag) {
      snd_ptr->time_remain_ms = SND_MAX_PERIOD;
    }
  }
}
#endif /* FEATURE_MPEG_AUDIO || FEATURE_REAL_AUDIO || FEATURE_WMA */

/* <EJECT> */
#ifdef FEATURE_IMELODY
/*===========================================================================

FUNCTION snd_age_imy

DESCRIPTION
  Updates/ages the status of the iMelody sound specified by which_snd.  

DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
LOCAL void snd_age_imy
(
  snd_array_idx_type which_snd,
    /* Which sound is being processed (voice prompt, sound, or tone) */

  uint32 ms_elapsed_time,
    /* Number of milliseconds which have elapsed since the last update */

  word change_mask
    /*lint -esym(715,change_mask) Tell Lint we know change_mask parameter 
    ** is not used */
    /* Mask of Changes found by snd_change() */
)
{
  snd_sound_status_type *snd_ptr;          /* Pointer to snd_array[which_snd] */
  audmain_status_type   audfmt_status = AUDMAIN_STATUS_SUCCESS;
  uint32                time_ms_adjust;
  snd_status_type       ret_status;
  boolean               do_abort = FALSE;
  /* Get a pointer to the sound status for the sound being aged.  */
  snd_ptr = &snd_array[ which_snd ];
  if( snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND &&
      !audmain_get_pause(snd_ptr->audfmt_ctl) ) {

    if (audmain_get_parse_type(snd_array[which_snd].audfmt_ctl) ==
        AUDMAIN_IMY_TYPE) {

      /* Age audio format sound */
      audmain_age( snd_ptr->audfmt_ctl, &ms_elapsed_time, &time_ms_adjust );

      /* Increment elapsed time */
      snd_audfmt_elapsed_ms  += ms_elapsed_time;
      snd_audfmt_adjusted_ms += time_ms_adjust;
      if(snd_audfmt_adjusted_ms >= snd_audfmt_next_update) {
        snd_audfmt_next_update += SND_AUDFMT_TICK_INTERVAL;
        if(snd_ptr->callback_ptr != NULL) {
          snd_ptr->callback_ptr(snd_ptr->client_data, SND_AF_TIME_TICK);
        }
      }

      /* Get one time event of audio format sound */
      audfmt_status = audmain_parse( snd_ptr->audfmt_ctl, snd_ptr->audfmt_buf,
                                     &(snd_ptr->time_remain_ms) );

      if(snd_audfmt_next_update > snd_audfmt_adjusted_ms) {
        snd_ptr->time_remain_ms = (uint16) MIN(snd_ptr->time_remain_ms, 
                              snd_audfmt_next_update - snd_audfmt_adjusted_ms);
      }

      switch(audfmt_status){
      case AUDMAIN_STATUS_MAX:
        /* Problem in the code if this status is encountered */
        MSG_ERROR("Unexpected error status", 0,0,0);
      case AUDMAIN_STATUS_ERROR:
      case AUDMAIN_STATUS_FAILURE:
        ret_status = SND_ABORT;
        do_abort = TRUE;
        break;
      case AUDMAIN_STATUS_DATA_ERR:
        ret_status = SND_ABORT_DATA_ERR;
        do_abort = TRUE;
        break;
      case AUDMAIN_STATUS_DATA_ACCESS_ERR:
        ret_status = SND_ABORT_DATA_ACCESS_ERR;
        do_abort = TRUE;
        break;
      case AUDMAIN_STATUS_RESOURCE_ERR:
        ret_status = SND_ABORT_RESOURCE_ERR;
        do_abort = TRUE;
        break;
        
      case AUDMAIN_STATUS_DONE:
          /* expire the sound         */
          snd_expire( which_snd );
        break;
     
      default:
        /* Set init flag to done, sound can proceed to be aged. */
        audmain_set_init_done(snd_ptr->audfmt_ctl);
        break;
      }
      if(do_abort == TRUE){
        if ( snd_ptr->callback_ptr != NULL ) {
          MSG_MED("Expire %d , status %d", which_snd, ret_status, 0);
          snd_ptr->callback_ptr(snd_ptr->client_data, ret_status);
          snd_ptr->callback_ptr = NULL;
        }
        snd_expire( which_snd );
      }
    }
  }
}
#endif /* FEATURE_IMELODY */

/* <EJECT> */
#ifdef FEATURE_WAVE_PB
/*===========================================================================

FUNCTION snd_age_adpcm

DESCRIPTION
  Updates/ages the status of the adpcm sound specified by which_snd.

DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
LOCAL void snd_age_adpcm
(
  snd_array_idx_type which_snd,
    /* Which sound is being processed (voice prompt, sound, or tone) */

  uint32 ms_elapsed_time,
    /* Number of milliseconds which have elapsed since the last update */

  word change_mask
    /*lint -esym(715,change_mask) Tell Lint we know change_mask parameter
    ** is not used */
    /* Mask of Changes found by snd_change() */
)
{
  snd_sound_status_type *snd_ptr;          /* Pointer to snd_array[which_snd] */
  snd_audfmt_sound_type *audfmt_sound_ptr;
  audmain_status_type   status = AUDMAIN_STATUS_SUCCESS;
  uint32                time_ms_adjust;
  audmain_adec_cmd_type adec_cmd[AUDMAIN_ADEC_MAX_CHANNELS];
  voc_adec_cmd_type     voc_adec_cmd[AUDMAIN_ADEC_MAX_CHANNELS];
  boolean               do_write = FALSE, paused;
#ifdef FEATURE_GASYNTH_INTERFACE
  uint32                adec_buf1_used[AUDMAIN_ADEC_MAX_CHANNELS];
  uint32                adec_buf2_used[AUDMAIN_ADEC_MAX_CHANNELS];
  uint8                 buf_usage_mask[AUDMAIN_ADEC_MAX_CHANNELS];
#else
  uint32                num[AUDMAIN_ADEC_MAX_CHANNELS]; 
#endif /* FEATURE_GASYNTH_INTERFACE */
  uint32                counter, num_iterations = 0;
  static uint32         adec_reset = 0;
  snd_adec_mode_type    adec_mode;
  audmain_file_type     file_type;
  snd_status_type       ret_status = SND_SUCCESS;
  boolean               do_abort = FALSE, do_repeat;

  /* Get a pointer to the sound status for the sound being aged.  */
  snd_ptr = &snd_array[ which_snd ];
  if((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
     (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND)) {
    file_type = audmain_get_parse_type(snd_ptr->audfmt_ctl);
    switch(audmain_get_parse_type(snd_ptr->audfmt_ctl)) {
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
        adec_mode = SND_ADEC_MODE_ADPCM;
        break;
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
        adec_mode = SND_ADEC_MODE_PCM;
        break;
#endif
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_ALAW_TYPE:
        adec_mode = SND_ADEC_MODE_WAV_ALAW;
        break;
      case AUDMAIN_WAV_MULAW_TYPE:
        adec_mode = SND_ADEC_MODE_WAV_MULAW;
        break;
#endif /* FEATURE_VOC_G711 */

      default:
        return;
    }

    do {
      num_iterations++;
      do_repeat = FALSE;

      paused = audmain_get_pause(snd_ptr->audfmt_ctl);
      if(!paused) {
        /* Age audio format sound */
        audmain_age(snd_ptr->audfmt_ctl, &ms_elapsed_time, &time_ms_adjust);

        /* Increment elapsed time */
        snd_audfmt_elapsed_ms  += ms_elapsed_time;
        snd_audfmt_adjusted_ms += time_ms_adjust;
        if(snd_audfmt_adjusted_ms >= snd_audfmt_next_update) {
          snd_audfmt_next_update += SND_AUDFMT_TICK_INTERVAL;
          if(snd_ptr->callback_ptr != NULL) {
            snd_ptr->callback_ptr(snd_ptr->client_data, SND_AF_TIME_TICK);
          }
        }

        /* Get one time event of audio format sound */
        status = audmain_parse(snd_ptr->audfmt_ctl, snd_ptr->audfmt_buf,
                               &(snd_ptr->time_remain_ms));

        if(snd_audfmt_next_update > snd_audfmt_adjusted_ms) {
          snd_ptr->time_remain_ms = snd_audfmt_next_update -
                                                 snd_audfmt_adjusted_ms;
        } else {
          snd_ptr->time_remain_ms = 0;
        }
      } else {
        snd_ptr->time_remain_ms = SND_MAX_PERIOD;
      }

      switch(status) {
        case AUDMAIN_STATUS_BUFF_ERR:
          audfmt_sound_ptr = (snd_audfmt_sound_type *) snd_ptr->sound_ptr;
          audmain_play_cb(snd_ptr->audfmt_ctl,
                          &(audfmt_sound_ptr->audfmt_handle),
                          SND_CMX_AF_FA_DELAY, snd_audfmt_adjusted_ms, NULL);
          /* Fall thru */

        case AUDMAIN_STATUS_SUCCESS:
          memset((uint8*) adec_cmd, 0,
                 sizeof(audmain_adec_cmd_type) * AUDMAIN_ADEC_MAX_CHANNELS);

#ifdef FEATURE_MIDI_OUT_QCP
          (void) audmain_adec_parse(snd_ptr->audfmt_ctl, adec_cmd);
#endif /* FEATURE_MIDI_OUT_QCP */

          if(!paused) {
            adec_cmd[0] = snd_ptr->audfmt_buf->adec_cmd;
          }

          counter = AUDMAIN_ADEC_MAX_CHANNELS;
          while(counter--) {
            if(adec_cmd[counter].adec_reset) {
              adec_reset |= (1<<counter);
            }
#ifdef FEATURE_GASYNTH_INTERFACE
            adec_buf1_used[counter] = adec_cmd[counter].adec_num_buf1;
            adec_buf2_used[counter] = adec_cmd[counter].adec_num_buf2;
            buf_usage_mask[counter] = adec_cmd[counter].buf_usage_mask;
            voc_adec_cmd[counter].buf_usage_mask = &buf_usage_mask[counter];
            if ((adec_buf1_used[counter] + adec_buf2_used[counter]) > 0) {
              do_write = TRUE;
            }          
#else
            num[counter] = adec_cmd[counter].adec_num_buf1 +
                           adec_cmd[counter].adec_num_buf2;
            if(num[counter] != 0) {
              do_write = TRUE;
            }
#endif /* FEATURE_GASYNTH_INTERFACE */
            voc_adec_cmd[counter].buf1    = adec_cmd[counter].adec_buf1;
            voc_adec_cmd[counter].length1 = adec_cmd[counter].adec_num_buf1;
            voc_adec_cmd[counter].buf2    = adec_cmd[counter].adec_buf2;
            voc_adec_cmd[counter].length2 = adec_cmd[counter].adec_num_buf2;
          }

          if(sndhw_audfmt_adec_int_pending()
#ifdef FEATURE_GASYNTH_INTERFACE
#ifdef FEATURE_PCM
             || (adec_mode == SND_ADEC_MODE_PCM) 
#endif /* FEATURE_PCM */
#ifdef FEATURE_ADPCM
             || (adec_mode == SND_ADEC_MODE_ADPCM) 
#endif /* FEATURE_ADPCM */
#endif /* FEATURE_GASYNTH_INTERFACE */
            ) {
            if(do_write) {
              if(adec_reset != 0) {
                sndhw_audfmt_adec_reset(adec_reset);
                adec_reset = 0;
              }

              if((ret_status = sndhw_audfmt_adec_data(adec_mode,
                                                      voc_adec_cmd)) !=
                 SND_SUCCESS) {

                if(snd_ptr->callback_ptr != NULL) {
                  MSG_MED("Expire %d , status %d", which_snd, ret_status, 0);
                  snd_ptr->callback_ptr(snd_ptr->client_data, ret_status);
                  snd_ptr->callback_ptr = NULL;
                }
                snd_expire( which_snd );        /* expire the sound       */
#ifdef FEATURE_MIDI_OUT_QCP
                snd_audfmt_adec_int(NULL);
#endif /* FEATURE_MIDI_OUT_QCP */
                break;
              }

              counter = AUDMAIN_ADEC_MAX_CHANNELS;
#ifdef FEATURE_GASYNTH_INTERFACE
              while(counter--) {
                if(adec_buf1_used[counter] != 0) {
                  adec_buf1_used[counter] -= voc_adec_cmd[counter].length1;
                }
                if (adec_buf2_used[counter] != 0) {
                  adec_buf2_used[counter] -= voc_adec_cmd[counter].length2;
                }
              }
              if(audmain_adec_ppbuf_adjust(snd_ptr->audfmt_ctl, 
                                           adec_buf1_used, 
                                           adec_buf2_used,
                                           buf_usage_mask) ==
                 AUDMAIN_STATUS_PENDING) {
                do_repeat = TRUE;
              }
#else
              while(counter--) {
                if(num[counter] != 0) {
                  num[counter] = num[counter] -
                                 (voc_adec_cmd[counter].length1 +
                                  voc_adec_cmd[counter].length2);
                }
              }

              (void) audmain_adec_adjust(snd_ptr->audfmt_ctl, num);
#endif /* FEATURE_GASYNTH_INTERFACE */
            } 
          }
          break;

        case AUDMAIN_STATUS_DONE:
          /* Make sure the last chunk of data has been processed */
          if (!sndhw_audfmt_adec_int_pending()) {
            break;
          }
          snd_expire( which_snd );        /* expire the sound       */
#ifdef FEATURE_MIDI_OUT_QCP
          snd_audfmt_adec_int(NULL);
#endif /* FEATURE_MIDI_OUT_QCP */
          break;

      
        case AUDMAIN_STATUS_DATA_ERR:
          ret_status = SND_ABORT_DATA_ERR;
          do_abort = TRUE;
          break;
        case AUDMAIN_STATUS_DATA_ACCESS_ERR:
          ret_status = SND_ABORT_DATA_ACCESS_ERR;
          do_abort = TRUE;
          break;
        case AUDMAIN_STATUS_RESOURCE_ERR:
          ret_status = SND_ABORT_RESOURCE_ERR;
          do_abort = TRUE;
          break;
        case AUDMAIN_STATUS_MAX:
        /* Problem in the code if this status is encountered */
          MSG_ERROR("Unexpected error status", 0,0,0);
        case AUDMAIN_STATUS_ERROR:
        case AUDMAIN_STATUS_FAILURE:
        default:
          ret_status = SND_ABORT;
          do_abort = TRUE;
          break;
      }
    } while((do_repeat) && (num_iterations < 2));

    if(do_abort == TRUE) {
      if(snd_ptr->callback_ptr != NULL) {
        MSG_MED("Expire %d , status %d", which_snd, ret_status, 0);
        snd_ptr->callback_ptr(snd_ptr->client_data, ret_status);
        snd_ptr->callback_ptr = NULL;
      }

      snd_expire( which_snd );        /* expire the sound       */
#ifdef FEATURE_MIDI_OUT_QCP
      snd_audfmt_adec_int(NULL);
#endif /* FEATURE_MIDI_OUT_QCP */
    }
  }
}
#endif /* FEATURE_WAVE_PB */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_setup_audfmt

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type snd_setup_audfmt (
  snd_sound_status_type *snd_ptr,
  snd_cb_func_ptr_type  cb_func,
  const void            *client_data
) {
  snd_audfmt_sound_type   *audfmt_sound_ptr;
  snd_format_sound_type   *format_sound_ptr;
  snd_af_access_type      *audfmt_handle;
  audmain_play_param_type param;
  audfmt_sound_ptr = NULL;

  if (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND) {
    format_sound_ptr = (snd_format_sound_type *) snd_ptr->sound_ptr;
    audfmt_handle    = &(format_sound_ptr->audfmt_handle);

    if ( audmain_format_play_init ( audfmt_handle,
                                    &(format_sound_ptr->input_spec),
                                    &(snd_ptr->audfmt_ctl),
                                    &(snd_ptr->audfmt_buf)
                                  ) != AUDMAIN_STATUS_SUCCESS
       )
    {
      if ( cb_func != NULL ) {
        cb_func(client_data, SND_FAILURE);
      }
      return(SND_FAILURE);
    }
  } else {
    audfmt_sound_ptr = (snd_audfmt_sound_type *) snd_ptr->sound_ptr;
    audfmt_handle    = &(audfmt_sound_ptr->audfmt_handle);

    param.calc_time      = FALSE;
    param.get_specs      = FALSE;
    param.start_position = audfmt_sound_ptr->start_position;
    param.multiseq_flag  = audfmt_sound_ptr->param.multiseq_flag;
    param.cuep_enable    = audfmt_sound_ptr->param.cuepoint_enable;
    param.byte_offset    = audfmt_sound_ptr->param.byte_offset;
    if(param.cuep_enable) {
      param.repeat_enable  = FALSE;
      param.repeat_cnt     = audfmt_sound_ptr->param.repeat_cnt;
      param.repeat_ms      = 0;
    } else if(audfmt_sound_ptr->repeat_ms != 0) {
      param.repeat_enable  = TRUE;
      param.repeat_cnt     = 0;
      param.repeat_ms      = audfmt_sound_ptr->repeat_ms;
    } else {
      param.repeat_enable  = audfmt_sound_ptr->param.repeat_enable;
      param.repeat_cnt     = audfmt_sound_ptr->param.repeat_cnt;
      param.repeat_ms      = audfmt_sound_ptr->param.repeat_ms;
    }

    /* Parse header of audio format sound */
    if(audmain_parse_head(audfmt_handle, &(snd_ptr->audfmt_ctl),
                          &(snd_ptr->audfmt_buf), &param) !=
       AUDMAIN_STATUS_SUCCESS) {
      if(!snd_ptr->active) {
        if ( cb_func != NULL ) {
          cb_func(client_data, SND_FAILURE);
        }
      }
      return(SND_FAILURE);
    }
  }

  if(!snd_ptr->active) {
    /* Initialize audio format sound variables */
    snd_audfmt_elapsed_ms  = 0;
    snd_audfmt_adjusted_ms = 0;
    snd_audfmt_diff_out    = 0;
    snd_audfmt_next_update = SND_AUDFMT_TICK_INTERVAL;
    clk_uptime_ms(snd_audfmt_interval);
  }

  snd_ptr->sample_rate = audmain_get_sample_rate(snd_ptr->audfmt_ctl);

  /* Load the playback-specific song configuration */
  (void) audmain_get_song_config(snd_ptr->audfmt_ctl,
                                 &(snd_ptr->audfmt_song_cfg));

  switch ( audmain_get_parse_type(snd_ptr->audfmt_ctl) ) {
#ifdef FEATURE_MIDI
    case AUDMAIN_MIDI_TYPE:
      /* Force it to use MIDI generator. */
      snd_ptr->method       = SND_METHOD_MIDI;

      /* Set aging function */
      snd_ptr->age_func_ptr = (snd_age_func_ptr_type) snd_age_audfmt;
      break;
#endif /* FEATURE_MIDI */

#ifdef FEATURE_IMELODY
  case AUDMAIN_IMY_TYPE:
    /* Set aging function */
    snd_ptr->age_func_ptr = (snd_age_func_ptr_type) snd_age_imy;
    break;
#endif /* FEATURE_IMELODY */

#if defined(FEATURE_MPEG_AUDIO) || defined(FEATURE_REAL_AUDIO) || \
    defined(FEATURE_WMA)
#ifdef FEATURE_ADEC
#ifdef FEATURE_MP3
    case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
    case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
    case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_WMA
    case AUDMAIN_WMA_TYPE:
    case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_AMR_WB_AUDIO_DEC
    case AUDMAIN_AMR_WB_TYPE:
    case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
    case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

#ifdef FEATURE_MIDI_OUT
      if(snd_midi_out_enabled == SND_MIDI_OUT_STATE_ENABLED) {
        audmain_reset();
        if ( cb_func != NULL ) {
          cb_func(client_data, SND_FAILURE);
        }
        return(SND_FAILURE);
      } else 
#endif /* FEATURE_MIDI_OUT */
      {
        snd_ptr->method       = SND_METHOD_MIDI;

        /* Set aging function */
        snd_ptr->age_func_ptr = (snd_age_func_ptr_type) snd_age_adec_stream;
      }
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
      /* write daul mono/sbr mode, to save data in vocdata */
        sndhw_audfmt_dual_mono(
          snd_ptr->sound_ptr->format_sound.input_spec.aac_spec.dual_mono_mode);
        sndhw_audfmt_sbr_mode(
          snd_ptr->sound_ptr->format_sound.input_spec.aac_spec.sbr_mode);
#endif
      break;
#endif /* FEATURE_ADEC */
#endif /* FEATURE_MPEG_AUDIO || FEATURE_REAL_AUDIO || FEATURE_WMA */

#ifdef FEATURE_QCP
    case AUDMAIN_QCP_TYPE:
#ifdef FEATURE_MIDI_OUT
      if(snd_midi_out_enabled == SND_MIDI_OUT_STATE_ENABLED) {
        audmain_reset();
        if ( cb_func != NULL ) {
          cb_func(client_data, SND_FAILURE);
        }
        return(SND_FAILURE);
      } else
#endif /* FEATURE_MIDI_OUT */
      {
        /* Initialize variables */
        snd_vp_callback_func  = NULL;
        snd_vp_err_status     = FALSE;
        snd_ptr->method       = SND_METHOD_VOICE;
        /* Set aging function */
        snd_ptr->age_func_ptr = (snd_age_func_ptr_type) snd_age_qcp;

        /* Enqueue voice prompt pointer */
        if( snd_enqueue_vp( snd_ptr->audfmt_buf->prompt1,
                            snd_ptr->audfmt_buf->prompt1, cb_func ) == 
            SND_PPQ_PQ_FAIL ) {
          return(SND_FAILURE);
        } else {
          snd_ptr->audfmt_buf->prompt1 = NULL;
        }

        audmain_set_init_done(snd_ptr->audfmt_ctl);
      }
      break;
#endif /* FEATURE_QCP */

#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
    case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
    case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_VOC_G711
    case AUDMAIN_WAV_ALAW_TYPE:
    case AUDMAIN_WAV_MULAW_TYPE:
#endif /* FEATURE_VOC_G711 */
      /* Force it to use MIDI generator. */
      snd_ptr->method       = SND_METHOD_MIDI;

      audmain_set_init_done(snd_ptr->audfmt_ctl);
      /* Set aging function */
      snd_ptr->age_func_ptr = snd_age_adpcm;
   if(!(param.calc_time)&&!(param.get_specs) &&
   	      audfmt_sound_ptr!=NULL){
   	
      if((audfmt_sound_ptr->param.multiseq_flag!=TRUE)
#ifdef FEATURE_MIDI_OUT_QCP
		 &&(snd_midi_out_enabled == SND_MIDI_OUT_STATE_DISABLED)
#endif
		 ){
			snd_ptr->wave_resampler = TRUE;  
      }else{
            snd_ptr->wave_resampler = FALSE;
	  }	  
    }	  

      break;   
#endif /* FEATURE_WAVE_PB */

  }
  return(SND_SUCCESS);
}
#endif /* FEATURE_AUDIO_FORMAT */


/* <EJECT> */
/*===========================================================================

FUNCTION snd_use_resampler_wave

DESCRIPTION
  This function tells whether to use resampler module or regular module.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean snd_use_resampler_wave(void)
{
  return(snd_array[SND_SND_LO].wave_resampler);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_age_tone

DESCRIPTION
  Updates/ages the status of the sound specified by which_snd.  This includes
  expiring the sound after the last tone has been played.

DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
LOCAL void snd_age_tone
(
  snd_array_idx_type which_snd,
    /* Which sound is being processed (voice prompt, sound, or tone) */

  uint32 ms_elapsed_time,
    /* Number of milliseconds which have elapsed since the last update */

  word change_mask
    /* Mask of Changes found by snd_change() */
)
{
  snd_sound_status_type *snd_ptr;
    /* Pointer to snd_array[which_snd] */
  uint32            time_remain_ms;
    /* Number of Milliseconds left in the tone currently being scanned */
  word              tone_index;
    /* Index of current tone in snd_array entry */

  snd_tone_type          tone;        /* temp tone      */
  snd_tone_data_type     tone_data;   /* temp tone data */
#ifdef FEATURE_VOICE_PLAYBACK
  uint32 counter;
#endif

  if( !SND_NEW_TONE(which_snd, change_mask) ) {
    /* Get a pointer to the sound status for the sound being aged.  */
    snd_ptr     = &(snd_array[ which_snd ]);

#ifdef FEATURE_VOICE_PLAYBACK
    /* We want to do snd_age for voice prompts if we get a signal from vocoder
    ** regardless if the tone is on hold.
    */
    if ((snd_ptr->hold == FALSE) || (change_mask&SND_VP_SIGNAL))
#else
    if (snd_ptr->hold == FALSE)
#endif
    {

      tone_index  = snd_ptr->tone_index;    /* Get tone index in sound  */

      /* get the current tone and duration from the sound structure */
      snd_get_sound_info(snd_ptr->sound_ptr, tone_index, &tone, &tone_data);

#ifdef FEATURE_VOICE_PLAYBACK
      if(tone == SND_VOICE_PROMPT) {
        if(snd_ptr->vp_playing) {
          /* If we got a signal from the vocoder that its VP queue is empty,
          ** the voice prompt is done playing.
          */
          if ((change_mask&SND_VP_SIGNAL)!=0) {
            for(counter = 0; counter < snd_vp_sig_num; counter++) {
              if(snd_vp_status[counter] == VOC_END_PB) {
                MSG_HIGH( "VP:Q Empty Ack",0,0,0);
                snd_ptr->vp_playing     = FALSE;
                snd_ptr->time_remain_ms = 0;
                ms_elapsed_time         = 0;
                if((which_snd == SND_SND_HI) || (which_snd == SND_SND_MED) ||
                   (which_snd == SND_SND_LO)) 
                {
                  /* Only need to cleanup for sound because if it is of type 
                  ** SND_PLAYBACK, it will be expired in snd_expire(). We have
                  ** to do the cleanup here for sound because the VP portion 
                  ** of the sound is finished but the sound itself may still
                  ** not be done (i.e. there could be some tones to play now).
                  */
                  sndhw_cleanup_vp();
                }
              } else if (snd_vp_status[counter] == VOC_END_ABORT_PB) {
                if(snd_ptr->callback_ptr != NULL) {
                  snd_ptr->callback_ptr(snd_ptr->client_data, SND_ABORT);
                }
                snd_expire(which_snd);
                return;
              }
            }
          }
        }
      }
#endif

      /* Subtract elapsed time, if we have elapsed more than just this
      ** one tone index, take the time from the next one(s) until the
      ** sound is expired -or- we've found the tone that should be
      ** generated at this point in time.
      */
      /* Count of ms left on this tone */
      time_remain_ms = snd_ptr->time_remain_ms;

#ifdef FEATURE_VOICE_PLAYBACK
      /* Only enter here if a voice prompt is not active, tones are finished
      ** when their duration is done but VPs are finished only when the vocoder
      ** driver tells us they are done.
      */
      if ((!snd_ptr->vp_playing)&&(time_remain_ms <= ms_elapsed_time)) {
#else
      if (time_remain_ms <= ms_elapsed_time) {
#endif
        do {

          ms_elapsed_time -= time_remain_ms;     /* Account for some time  */

          tone_index++;                          /* Next tone in sequence  */

#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
          /* get the current tone and duration from the sound structure */
          snd_get_sound_info(snd_ptr->sound_ptr, tone_index, &tone, &tone_data);

          /* Process special tones (stop, repeat, loop-back-2) */
          /*lint -e788 Tell Lint we know not every enum is in the switch */
          switch ( tone ) {
            /* Stop playing
            */
            case SND_STOP:
              snd_expire( which_snd );           /* expire the tone         */
              return;                            /* discontinue this tone   */

            /* Callback then silence
            */
            case SND_CALLBACK_SILENCE:

              /* If there is a callback when 'repeat' is reached,
              ** call the callback.
              ** Note: for certain sounds, it is desirable to send the
              ** repeat callback at the start of the silence before the
              ** repeat. This control feature should be use in conjunction
              ** with SND_RPT_NOCALLBACK.
              */
              if ( snd_ptr->callback_ptr != NULL )  {
                snd_ptr->callback_ptr( snd_ptr->client_data, SND_REPEAT );
              }
              break;

            /* Call Callback and repeat from start
            */
            case SND_RPT:
              /* If there is a callback when 'repeat' is reached, call the
              ** function. Indicates that sequence is starting over.
              */
              if ( snd_ptr->callback_ptr != NULL )  {
                snd_ptr->callback_ptr( snd_ptr->client_data, SND_REPEAT );
              }
              /* Fall through to next case */

            /* Repeat from start
            */
            case SND_RPT_NOCALLBACK:
              if ( snd_ptr->repeat_cnt != 0) {
                snd_ptr->repeat_cnt--;             /* decrement count */
                /* Check loop count, if it goes to 0, the loop is done.
                */
                if (snd_ptr->repeat_cnt == 0) {   /* stop this tone        */
                  snd_expire( which_snd );        /* expire the tone       */
                  return;                         /* discontinue this tone */
                }
              } else if (tone_data.data != 0) {
                snd_ptr->repeat_cnt = tone_data.data;
              }

              tone_index = 0;                    /* Loop back to first tone */
              snd_get_sound_info(snd_ptr->sound_ptr, tone_index, 
                                 &tone, &tone_data);
              break;

            /* Loop back 2 tones
            */
            case SND_LOOP_BACK2:

              /* If the loop count is not initialized, initialize it.  Note
              ** that for SND_LOOP_BACK2, tone_data contains the number of
              ** times to go back and repeat the loop (NOT Milliseconds).
              */
              if ( snd_ptr->loop_cnt == 0) {
                snd_ptr->loop_cnt = tone_data.data;
              } else {      /* Else, the count is initialized, decrement it */
                --snd_ptr->loop_cnt;
              }

              /* Check loop count, if it goes to 0, the loop is done.
              */
              if (snd_ptr->loop_cnt == 0) {      /* Continue to next tone   */

                time_remain_ms = 0;              /* No time in this 'tone'  */
                continue;                        /* Continue from the top   */

              } else {                           /* Loop back 2 tones       */

                tone_index -= 2;                 /* Adjust the tone index   */
                snd_get_sound_info(snd_ptr->sound_ptr, tone_index, 
                                   &tone, &tone_data);

              }
              break;

            /* Record the position of the label.
            */
            case SND_LABEL:
              snd_ptr->label_index = tone_index;
              break;
  
            /* Loop back to the label
            */
            case SND_BACK_TO_LABEL:
              if (snd_ptr->label_index == 0xff) {
                ERR_FATAL("No label to loop to",0,0,0);
              }

              /* If the loop count is not initialized, initialize it.  Note
              ** that tone_data contains the number of
              ** times to go back and repeat the loop (NOT Milliseconds).
              */
              if ( snd_ptr->label_cnt == 0) {
                snd_ptr->label_cnt = tone_data.data;
              } else {      /* Else, the count is initialized, decrement it */
                --snd_ptr->label_cnt;
              }

              /* Check loop count, if it goes to 0, the loop is done.
              */
              if (snd_ptr->label_cnt == 0) {     /* Continue to next tone   */
                snd_ptr->label_index = 0xff;     /* Done with this loop     */
                time_remain_ms       = 0;        /* No time in this 'tone'  */
              } else {                           /* Loop back to the label */
                tone_index     = snd_ptr->label_index;
                time_remain_ms = 0;              /* No time in this 'tone'  */
              }
              continue;                          /* Continue from the top   */
  
            /* Repeat the last tone forever
            */
            case SND_RPT1:

              tone_index -= 1;                   /* Adjust the tone index   */
              snd_get_sound_info(snd_ptr->sound_ptr, tone_index, 
                                 &tone, &tone_data);
              break;

            case SND_VOL_SCALE:
          
              snd_ptr->scaling = tone_data.data;   /* Update vol scale       */
              time_remain_ms   = 0;                /* No time in this 'tone' */
              continue;  /* continue to prevent tone_data from being counted */ 
  
            default:                               /* No special action      */
              break;
          }
          /*lint +e788 turn message back on for other switch() statements */

#ifdef FEATURE_VOICE_PLAYBACK
          if(tone == SND_VOICE_PROMPT) {
            time_remain_ms = 0;
          } else {
            /* Set tone count to the duration for this tone.
            */
            time_remain_ms = tone_data.duration_ms;
          }
        } while((time_remain_ms <= ms_elapsed_time) &&  /* cont loop only if */
                (tone != SND_VOICE_PROMPT));            /* VP not next tone  */
                                                 /* or time has not elapsed  */
#else /* FEATURE_VOICE_PLAYBACK */
          time_remain_ms = tone_data.duration_ms;
        } while(time_remain_ms <= ms_elapsed_time);
#endif /* FEATURE_VOICE_PLAYBACK */

        snd_ptr->tone_index = tone_index;        /* Update tone index       */
      }

#ifdef FEATURE_VOICE_PLAYBACK
      if(tone != SND_VOICE_PROMPT) 
#endif
      {
        /* Tone_cnt is greater than the remaining segment of ms_elapsed_time.
        ** Count down the time which has elapsed on the tone which is the
        ** currently active tone.
        */
        snd_ptr->time_remain_ms = time_remain_ms - ms_elapsed_time;
      }
    }
  }
} /* end of snd_age_tone */
/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION snd_voice_prompt_queue

DESCRIPTION
  Will enqueue voice prompts in local intermediate queue to be enqueued onto 
  the vocoder queue later on. Will also free VP buffers if they need to be 
  freed.

DEPENDENCIES
  snd_array

RETURN VALUE
  None

SIDE EFFECTS
  snd_array

===========================================================================*/
void snd_voice_prompt_queue (
  snd_array_idx_type which_snd,
    /* Which sound is being processed (voice prompt, sound, or tone) */

  word change_mask
    /* Mask of Changes found by snd_change() */
)
{
  snd_sound_status_type *snd_ptr;
    /* Pointer to snd_array[which_snd] */
  word              tone_index;
    /* Index of current tone in snd_array entry */

  snd_tone_type         tone;        /* temp tone */
  snd_tone_data_type    tone_data;   /* temp tone data */

  uint32 counter;

  /* Get a pointer to the sound status for the sound being aged.  */
  snd_ptr     = &(snd_array[ which_snd ]);
  tone_index  = snd_ptr->tone_index;    /* Get tone index in sound */

  /* get the current tone and duration from the sound structure */
  snd_get_sound_info(snd_ptr->sound_ptr, tone_index, &tone, &tone_data);

  if ((which_snd == SND_PLAYBACK) && ((change_mask&SND_VP_SIGNAL)!=0)) {
    for(counter = 0; counter < snd_vp_sig_num; counter++) {
      if(snd_vp_status[counter] == VOC_END_Q_NOT_EMPTY) {
        /* Free buffer if vocoder queue not empty and is of type SND_PLAYBACK */
        snd_vp_callback(snd_vp_client_data[counter], SND_PROMPT_DONE);
      }
    }
  }

  if(tone == SND_VOICE_PROMPT) {
    if(which_snd != SND_PLAYBACK) {
      /* SND_PLAYBACK voice prompts are enqueued when the command is 
      ** processed. Multi tones have to be done here due to their dynamic
      ** nature.
      */
      if(!snd_ptr->vp_playing) {
        /* If VP not currently playing then the sound_ptr is pointing to
        ** a VP that has not yet been enqueued, if there is a VP playing
        ** then sound_ptr points to the last VP that has been queued.
        */
        if(sndhw_pq_enqueue(tone_data.data) == SND_PPQ_PQ_FAIL) {
          /* should never reach this point */
          ERR("Queue is full even though no VP playing!",0,0,0);
        }
      }
      /* Check the following tone, if it is a voice prompt then try to enqueue
      ** it, if the queue is full the tone_index will not be advanced. This
      ** way, tone_index will always point to the last VP enqueued.
      */
      snd_get_sound_info(snd_ptr->sound_ptr, tone_index+1, &tone, &tone_data);
      while(tone == SND_VOICE_PROMPT) {
        if(sndhw_pq_enqueue(tone_data.data) == SND_PPQ_PQ_FAIL) {
          /* intermediate queue full, move on */
          break;
        }
        tone_index++;
        snd_get_sound_info(snd_ptr->sound_ptr, tone_index+1, &tone, &tone_data);
        MSG_MED( "VP:Queuing, %x", tone_data.data, 0, 0);
      }
    }
    snd_ptr->vp_playing     = TRUE;           /* set vp_playing flag to TRUE */
    snd_ptr->tone_index     = tone_index;     /* Update tone index       */
    snd_ptr->time_remain_ms = SND_MAX_PERIOD;
  }
} /* end of snd_voice_prompt_queue */
#endif /* FEATURE_VOICE_PLAYBACK */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_change_active_tone

DESCRIPTION
  This routine is called once it has been determined that there is an active
  tone to generate.  This code sets up the generation of that tone, and
  sets the Audio Path(s) and timers.

DEPENDENCIES
  snd_voice_path_out

RETURN VALUE
  TRUE  - if tone is on hold
  FALSE - if tone is being played

SIDE EFFECTS
  SND_SLEEP_FORBID()
  snd_voice_muted
  snd_tone_path_out

===========================================================================*/

LOCAL boolean snd_change_active_tone
(
  tone_status_type *tone_status_ptr,
    /* Pointer to DTMF information */
  word change_mask
    /* Mask of Changes found by snd_change() */
    /*lint -esym(715,change_mask) Suppress "unused" Message (Some Targets) */
)

{
  boolean        if_muted;
    /* Are the DTMF's Currently Muted?  (Target Specific) */
  snd_apath_type dtmf_path;
    /* Local shortcut for current dtmf path */
  byte           tone_vol;
    /* Local shortcut for current tone volume */

  /* While we are generating a Tone (with ANY generator) we will prevent
  ** the Phone from Sleeping.  Note that there are several reason we might
  ** want the phone to stay awake.
  */
  SND_SLEEP_FORBID();

  /* Setup local shortcuts
  */
  dtmf_path = tone_status_ptr->dtmf_path;
  tone_vol  = tone_status_ptr->vol;

  MSG_LOW("Sound: dev: %d, dp: %d, vol: %d",
           tone_status_ptr->device, dtmf_path, tone_vol);

#ifdef FEATURE_PHONE_VR
  /* Inform vocoder driver that Sound is playing something. */
  voc_snd_active_status (VOC_SND_STATUS_ACTIVE);
#endif /* FEATURE_PHONE_VR */

  /* If DTMF's are muted 'at this moment', play silence.
  */
  if_muted = (dtmf_path == SND_APATH_MUTE) ||
               /* Muted because we are on the Mute Path */
             ((tone_vol==0) && (dtmf_path==SND_APATH_LOCAL));
               /* Muted because the Volume is Zero or the tone is only being
               ** played for the user locally */

  if ( if_muted ) {  /* DTMF is Silence  */
    tone_status_ptr->type      = SND_TYPE_SILENCE;

  /* Else we are not muted "at this moment"
  */
  } else {

    /* Set DTMF output path
    */
    switch (dtmf_path) {

      case SND_APATH_LOCAL:      /* DTMF's on local-audio-only        */

        snd_dtmf_tx_mute_ctl( SND_MUTE_SND, TRUE );     /* DTMF Tx mute    */
        break;

      case SND_APATH_TX:         /* Transmit-only DTMFs               */
      case SND_APATH_BOTH:       /* Tx and sound DTMF's locally       */

        snd_dtmf_tx_mute_ctl( SND_MUTE_SND, FALSE );    /* DTMF Tx unmute  */
        break;

      /*lint -esym(788,SND_APATH_MUTE) Tell Lint we know that
      ** this value for the enum is not in this switch statement
      */

      default:
        ERR_FATAL("Illegal dtmf path %x", dtmf_path, 0, 0);
        /* Does not return */
    }
  }

  /* Mute voice on Rx and Tx.
  ** Muting the input path takes care of Tx voice.
  */
  if ( !snd_voice_muted ) {
    snd_voice_muted = TRUE;

    /* Mute audio-in during tone generation
    */
    if(snd_voice_path_in_mute == SND_MUTE_UNMUTED) {
      sndhw_set_apath_in( snd_tone_path_in_mute );
    }

    /* Mute voice
    */
    snd_rx_mute_ctl( SND_MUTE_SNDV, TRUE );
  }

#ifdef SND_HFK_PLUS
#error code not present
#endif

  /* Make changes to the audio path if necessary.
  ** This will also take care of any volume changes.
  */
  snd_tone_path_out.device  = tone_status_ptr->device;
  snd_tone_path_out.method  = tone_status_ptr->method;
  snd_tone_path_out.vol     = tone_status_ptr->vol;
  snd_tone_path_out.mute    = SND_MUTE_UNMUTED;

  /* setup tone path */
  sndhw_set_tone_path_out( &snd_tone_path_out, &snd_voice_path_out );

  return(sndhw_gen_tone(tone_status_ptr));

} /* end snd_change_active_tone() */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_change_inactive_tone

DESCRIPTION
  This procedure is called by snd_change() when it has determined there is
  no DTMF tone to play at this time.  This routine will re-establish the
  voice and audio path(s), clears timers and enables sleep mode.

DEPENDENCIES
  snd_voice_muted
  snd_voice_path_out
  snd_voice_path_in_mute

RETURN VALUE
  None

SIDE EFFECTS
  SND_SLEEP_ALLOW()
  snd_voice_muted

===========================================================================*/
LOCAL void snd_change_inactive_tone ( void )
{
  MSG_HIGH("Sound change, no tone", 0, 0, 0);

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)

  /*Indicates a HW ringer tone got stopped*/
  if(sndhw_get_bt_playback_other_start())
  {
    if (snd_bt_cb_func_ptr)
    {
      MSG_MED("Calling BT callback with playback OTHER stop", 0,0,0);
      snd_bt_cb_func_ptr(VOC_PLAYBACK_OTHER_STOP);
      sndhw_set_bt_playback_other_start(FALSE);
    }
  }

  /*Indicates a dtmf tone got stopped in FFA*/
  if(sndhw_get_bt_playback_dtmf_start())
  {
    if (snd_bt_cb_func_ptr)
    {
      MSG_MED("Calling BT callback with playback DTMF stop", 0,0,0);
      snd_bt_cb_func_ptr(VOC_PLAYBACK_DTMF_STOP);
      sndhw_set_bt_playback_dtmf_start(FALSE);
    }
  }
#endif /*FEATURE_AVS_BT_SCO_REWORK*/



#ifdef FEATURE_PHONE_VR
  /* Inform Vocoder driver that Sound is done. */
  voc_snd_active_status (VOC_SND_STATUS_INACTIVE);
#endif /* FEATURE_PHONE_VR */

#ifdef SND_HFK_PLUS
#error code not present
#endif

  /* Mute Transmit DTMF's
  */
  snd_dtmf_tx_mute_ctl( SND_MUTE_SND, TRUE );

  /* Re-connect the Vocoder and FM voice (only fiddle FM in analog mode)
  */
  if ( snd_voice_muted ) {
    snd_voice_muted = FALSE;

    /* Un-mute Vocoded voice, if in CDMA.  The newer Vocoder software can
    ** accept the Mute Change at any time.
    ** Un-mute Analog voice, always do this here (regardless of mode) to
    ** avoid mode switch problems with tones.
    */
    snd_rx_mute_ctl( SND_MUTE_SNDV, FALSE );

    /* After tones are done playing, reset scaling factor to zero */
    snd_data_set_scale (0);
  }

  /* Restore audio path and volume for voice. 
  */
  sndhw_restore_voice_path(&snd_voice_path_out, snd_voice_path_in_mute);

  /* I no longer need the phone to stay awake for Tone Generation.  So,
  ** I'll drop the flag that says this and allow Sleeping.
  */
  SND_SLEEP_ALLOW();
} /* end snd_change_inactive_tone() */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_change

DESCRIPTION
  This procedure changes the audio hardware to reflect the current state.
  Expire old tones, turn off unused tone generators, set the audio path and
  volume, generate/continue-generating the highest priority tone.

DEPENDENCIES
  snd_array
  snd_tone_timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void snd_change
(
  word  change_mask
    /* Mask of state changes */
)
{
  static uint32        last_period = 0;     /* Last period (Msecs) between  */
                                            /* tone changes                 */
  uint32               elapsed_time;        /* Time since last call (msecs) */
  uint32               next_period = 0;     /* Msecs before we come back    */

  snd_sound_status_type *snd_ptr, *aux_ptr; /* Pointer to snd_array[i]      */
  tone_status_type      crnt_tone;          /* All info to play a tone      */
  snd_tone_type         tone;               /* Current tone playing         */
  snd_tone_data_type    tone_data;          /* Data of current tone         */
  snd_array_idx_type    highest;            /* Index of Highest Pri Tone    */
  snd_array_idx_type    aux_snd;            /* Index of AUX sound, if any   */
  snd_gen_type          gen;                /* Generator of sound           */
  int                   i;                  /* Loop Array Index Temporary   */

#ifdef FEATURE_AUDIO_FORMAT
  void    *audfmt_ctl = NULL;
  uint32  next_audfmt_period;         /* Next period for audio format sounds */
  qword   tqword2, tqword1 = {0,0};   /* used to calculate elapsed time      */
#endif

  /* If there was a "last period" of tone generation, find out
  ** how far into it we are now.  Get elapsed time by subtracting
  ** the timer value from the last timer setting.
  */
  if (last_period != 0) {
    elapsed_time = last_period - rex_get_timer( &snd_tone_timer );
  } else {
    elapsed_time = 0;
  }

#ifdef FEATURE_AUDIO_FORMAT
  if(qw_cmp(snd_audfmt_interval, tqword1) != 0) {
    /* Calculate the exact elapsed time since the last service for
    ** audio format sounds.
    */
    clk_uptime_ms(tqword1);

    if(qw_cmp(tqword1, snd_audfmt_interval) < 0) {
      qw_set(tqword2, 0, 0);
    } else {
      qw_sub(tqword2, tqword1, snd_audfmt_interval);
      snd_audfmt_diff_out += (qw_lo(tqword2) - elapsed_time);
    }

    if(last_period != 0) {
      elapsed_time = qw_lo(tqword2);
    } else {
      elapsed_time = 0;
    }

    qw_equ(snd_audfmt_interval, tqword1);
  }
#endif

  /* Determine which sound to play.  At the end of this loop 'highest' is
  ** the index of the snd_array entry with the highest priority active sound.
  */
  highest = SND_ARRAY_SIZE;
  aux_snd = SND_ARRAY_SIZE;

  /* Set to Max we're willing to ever wait
  */
  next_period        = SND_MAX_PERIOD;

#ifdef FEATURE_AUDIO_FORMAT
  /* Initialize parameters */
  next_audfmt_period = SND_MAX_PERIOD;
  snd_audfmt_buf     = NULL;
#endif

  /* Subtract elapsed time from each active "old" tone,
  ** passing through the sound indices if necessary [i..N].
  ** Completed tones are purged.
  */
  for (i = 0, snd_ptr = snd_array; i < SND_ARRAY_SIZE; i++, snd_ptr++) {

    /* Age sounds.  */
    if ( snd_ptr->active ) {
      if (snd_ptr->age_func_ptr != NULL) {
        snd_ptr->age_func_ptr( (snd_array_idx_type) i, elapsed_time, 
                               change_mask );
      } else{
        ERR_FATAL("No age function for active sound: %d!!!",i,0,0);
      }
    }

    /* If tone has not expired, check to see if it is the shortest or 
    ** highest priority.
    */
    if ( snd_ptr->active ) {

#ifdef FEATURE_AUDIO_FORMAT
      if((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
         (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND)) {
        /* Save the information for audio format sound and determine at a
        ** later time whether to validate the numbers based on events.
        */
        snd_audfmt_buf     = snd_ptr->audfmt_buf;
        audfmt_ctl         = snd_ptr->audfmt_ctl;
        next_audfmt_period = snd_ptr->time_remain_ms;
      }
      else
#endif /* FEATURE_AUDIO_FORMAT */
      /* Get least duration for the next period to delay
      */
      if ( snd_ptr->time_remain_ms < next_period ) {
        next_period = snd_ptr->time_remain_ms;
      }

      if(snd_ptr->device == SND_DEVICE_CURRENT) {
        gen = snd_data_get_generator(snd_voice_path_out.device,
                                     snd_ptr->method);
      } else {
        gen = snd_data_get_generator(snd_ptr->device, snd_ptr->method);
      }
      if(gen == SND_GEN_AUX) {
        aux_snd = (snd_array_idx_type) i;
      } else {
        /* Highest priority sound has the highest index, note highest active.
        */
        highest = (snd_array_idx_type) i;
      }

#ifdef FEATURE_VOICE_PLAYBACK
      /* Check to see if current tone is a voice prompt to enqueue ptrs */
      snd_voice_prompt_queue( (snd_array_idx_type) i, change_mask );
#endif
    }
  }
  elapsed_time = 0;
  change_mask  = 0;
#ifdef FEATURE_VOICE_PLAYBACK
  snd_vp_sig_num   = 0;
#endif /* FEATURE_VOICE_PLAYBACK */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If no active tones, the DTMF is silence.  The NULL pointer allows us
  ** to differentiate this from an interval of silence, for which we would
  ** have to set a timer. Otherwise there is an active tone.
  */
  if ( highest == SND_ARRAY_SIZE ) {
    highest = aux_snd;
    aux_snd = SND_ARRAY_SIZE;
  }
  if ( highest == SND_ARRAY_SIZE ) {
    snd_ptr = NULL;
    tone    = SND_SILENCE;
  } else {
    snd_ptr = &(snd_array[ highest ]);
    if(aux_snd != SND_ARRAY_SIZE) {
      aux_ptr = &(snd_array[aux_snd]);
    } else {
      aux_ptr = NULL;
    }
    /* Get current tone and duration */
    snd_get_sound_info(snd_ptr->sound_ptr, snd_ptr->tone_index, 
                       &tone, &tone_data);
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( snd_ptr != NULL ) {

    /* There is an active tone.
    */
    /* Setup tone pointer */
    snd_setup_tone(&crnt_tone, snd_ptr, aux_ptr);

    /* Set scaling all the time since we don't know whether a tone is
    ** currently overiding a sound. Do not want sound scaling to be done for
    ** tone and vice versa.
    */
    snd_data_set_scale(snd_ptr->scaling);

    /* The tone might have to be delayed to wait for the vocoder
    */
    snd_ptr->hold = snd_change_active_tone( &crnt_tone, change_mask );

#ifdef FEATURE_ANIMATED_AAC
    if( snd_ptr->hold != FALSE &&
        (audmain_get_parse_type(snd_ptr->audfmt_ctl) == AUDMAIN_MIDI_TYPE &&
         audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC)
      ){
      audmain_set_init_done( snd_ptr->audfmt_ctl);
    }
#endif /* FEATURE_ANIMATED_AAC */

#ifdef FEATURE_AUDIO_FORMAT
    if((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
       (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND)) {
      next_audfmt_period = crnt_tone.time_remain_ms;
    }
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
    /* Set new tone timer.  Note: this clears SND_TONE_TIMER_SIG */
    /* If tone is on hold then set timer for maximum period. */
    if((snd_ptr->hold) 
#ifdef FEATURE_VOICE_PLAYBACK
       || (tone == SND_VOICE_PROMPT)
#endif
#ifdef FEATURE_AUDIO_FORMAT
       /* If we are an audio format sound and we are paused, set tone
       ** timer to max.
       */
       || (((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
            (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND)) &&
           audmain_get_pause(snd_ptr->audfmt_ctl))
#endif
      ) 
    {
      /* Set timer to max if playing a voice prompt, need to wait for the
      ** vocoder driver signal to wake us up but do not want to lose timing
      ** for other sounds.
      */
      last_period = 0;      /* Save the new 'last' period  */
      next_period = SND_MAX_PERIOD;
      (void) rex_set_timer( &snd_tone_timer, next_period );
    } else {
#ifdef FEATURE_AUDIO_FORMAT
      if((audmain_get_valid_time(audfmt_ctl) ||
          (snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
          (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND)) &&
         (next_period > next_audfmt_period)) {
        /* If audio format time is valid and less than next_period, set
        ** to next period.
        */
        next_period = next_audfmt_period;
      }
#endif
      next_period = MAX(5,next_period);  /* Release processor for a minimum */
                                         /* of 5 milliseconds               */
      last_period = next_period;         /* Save the new 'last' period  */
      (void) rex_set_timer( &snd_tone_timer, next_period );
    }
    MSG_MED("Next period = %d",last_period,0,0);
  } else {

    snd_change_inactive_tone();

    /* Turn off the tone timer, clear SND_TONE_TIMER_SIG, and set 'no' new
    ** 'last' period.
    */
    last_period = 0;
    (void) rex_clr_timer( &snd_tone_timer );
    (void) rex_clr_sigs( &snd_tcb, SND_TONE_TIMER_SIG );
  }
} /* end of snd_change */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_aux_line_in

DESCRIPTION
  Set the aux line in on/off

DEPENDENCIES
  
  
RETURN VALUE
  none

SIDE EFFECTS
  Command Packet queued to Sound Task.
  
===========================================================================*/
void snd_set_aux_line_in (

  snd_cb_func_ptr_type           callback_ptr,
  /* Pointer to cb func for receving status about this command */

  const void                     *client_data,
  /* Pointer to Client provided data */

  snd_aux_line_in_mode_type aux_mode
  /* Aux line In mode */
)
{
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> aux_linein.hdr.cmd          = SND_SET_AUX_LINE_IN;
    packet -> aux_linein.hdr.callback_ptr = callback_ptr;
    packet -> aux_linein.hdr.client_data  = client_data;
    packet -> aux_linein.aux_mode         = aux_mode;

    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_device_req

DESCRIPTION
  Set the path to the chosen device

DEPENDENCIES
  snd_voice_path_out
  snd_voice_path_in_mute

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  snd_voice_path_in_mute

===========================================================================*/
word snd_set_device_req ( snd_packets_type *cmd_ptr )
{
  word change_mask = SND_NO_CHANGE_M;

  MSG_MED("SND CMD: SND_SET_DEVICE=%d, O=%d, I=%d",
                cmd_ptr->dev.device,
                cmd_ptr->dev.out_mute_s,
                cmd_ptr->dev.in_mute_s);
 
  /* Set current device if requested */
  if(cmd_ptr->dev.device == SND_DEVICE_CURRENT) {
    cmd_ptr->dev.device = snd_voice_path_out.device;
  }

  /* If there is a change in Input Path Muting */
  if ( snd_voice_path_in_mute != cmd_ptr->dev.in_mute_s ) {
    snd_voice_path_in_mute = cmd_ptr->dev.in_mute_s;
    change_mask           |= SND_AUD_IN_M;
  }

#ifdef FEATURE_BT_AG  
  if (sndhw_check_aux_line_in() == TRUE){
    /* FM Is turned on */
    MSG_MED("FM is ON and device set to = %d (BT_HEADSET = %d)",
                  cmd_ptr->dev.device,
                  SND_DEVICE_BT_HEADSET, 0);
    
    if(cmd_ptr->dev.device == SND_DEVICE_BT_HEADSET){

      /* Unmute the device so vocoder remains acquired */
      cmd_ptr->dev.out_mute_s = SND_MUTE_UNMUTED;
    }
  }
#endif

  //to disable PP
  switch(cmd_ptr->dev.device)
  {
    case SND_DEVICE_BI_I2S:
    case SND_DEVICE_BI_I2S_TTY_FULL:
    case SND_DEVICE_BI_I2S_TTY_VCO:
    case SND_DEVICE_BI_I2S_TTY_HCO:
      snd_smartsnd_ctl(0,NULL, NULL);  
      break;
    default: 
      //do nothing
      break; 
  }

  /* If there is a change in Audio path or muting */
  if (( snd_voice_path_out.device != cmd_ptr->dev.device      ) ||
      ( snd_voice_path_out.mute   != cmd_ptr->dev.out_mute_s  )) {
 
    snd_setup_voice_path( cmd_ptr->dev.device,
                          cmd_ptr->dev.out_mute_s,
                          &snd_voice_path_out );

    change_mask |= SND_AUD_OUT_M;
  }
  return (change_mask);
} /* end of snd_set_device_req */
/*===========================================================================

FUNCTION snd_set_volume_req

DESCRIPTION
  Set the volume according to the device and method

DEPENDENCIES
  snd_voice_path_out

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  None

===========================================================================*/
word snd_set_volume_req ( snd_packets_type *cmd_ptr )
{
  word change_mask = SND_NO_CHANGE_M;

  MSG_MED("SND CMD:SND_SET_VOLUME D=%d, M=%d, V=%d",
                cmd_ptr->vol.device, 
                cmd_ptr->vol.method,
                cmd_ptr->vol.volume );

  /* Take care of phone volume changes.
  */
  if(cmd_ptr->vol.device == SND_DEVICE_CURRENT) {
    cmd_ptr->vol.device = snd_voice_path_out.device;
  }
  
  /* Set the volume for Line In */
  if(cmd_ptr->vol.method == SND_METHOD_AUX_LINE_IN)
  {
    /* control aux line in volume */
    sndhw_set_aux_line_in_vol(cmd_ptr->vol.volume);
  }
  else
  {
    if( sndhw_set_int_volume(cmd_ptr->vol.device, 
                           cmd_ptr->vol.method, cmd_ptr->vol.volume) ) {
      /* if volume has changed, make sure the change takes effect  */
      snd_setup_voice_path( snd_voice_path_out.device, 
                            snd_voice_path_out.mute,
                           &snd_voice_path_out );

      sndhw_set_volume();

      change_mask |= SND_VOLUME_M;
    }
  }
  return (change_mask);
} /* end of snd_set_volume_req */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_get_volume_req

DESCRIPTION
  Get the volume according to the device and method

DEPENDENCIES
  snd_voice_path_out

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_get_volume_req ( snd_packets_type *cmd_ptr )
{
  uint16 volume;

  /* Get volume from the volume array
  */
  if( cmd_ptr->get_vol.device == SND_DEVICE_CURRENT ) {
    cmd_ptr->get_vol.device = snd_voice_path_out.device;
  }
  volume = snd_data_get_volume(cmd_ptr->get_vol.device,
                               cmd_ptr->get_vol.method);

  MSG_MED("SND CMD: SND_GET_VOLUME= %d, %d, %d",
                cmd_ptr->get_vol.device, cmd_ptr->get_vol.method, volume );

  /* If call back is non-NULL, execute this function indicating that the
  ** command was accepted and returning the volume.
  */
  if ( cmd_ptr->get_vol.callback_ptr != NULL ) {
    cmd_ptr->get_vol.callback_ptr(cmd_ptr->hdr.client_data,SND_SUCCESS,volume);
    cmd_ptr->get_vol.callback_ptr = NULL;
  }
} /* end of snd_get_volume_req */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_get_levels_req

DESCRIPTION
  Get the number of volume level according to the device and method

DEPENDENCIES
  snd_voice_path_out

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_get_levels_req ( snd_packets_type *cmd_ptr )
{
  uint16 levels;

  if( cmd_ptr->level.device == SND_DEVICE_CURRENT ) {
    cmd_ptr->level.device = snd_voice_path_out.device;
  }

  /* Get maximum volume level from the calibration data base
  */
  levels = snd_data_get_num_levels(cmd_ptr->level.device,
                                   cmd_ptr->level.method);

  MSG_MED("SND CMD: SND_GET_LEVELS= %d, %d, %d",
                cmd_ptr->level.device, cmd_ptr->level.method, levels );
  
  /* If call back is non-NULL, execute this function indicating that the
  ** command was accepted and returning the number of volume levels.
  */
  if ( cmd_ptr->level.callback_ptr != NULL ) {
    cmd_ptr->level.callback_ptr(cmd_ptr->hdr.client_data, SND_SUCCESS, levels);
    cmd_ptr->level.callback_ptr = NULL;
  }
} /* end of snd_get_levels_req */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_tone_req

DESCRIPTION
  Start/stop playing a tone

DEPENDENCIES
  None

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  snd_array

===========================================================================*/
word snd_tone_req ( snd_packets_type *cmd_ptr )
{
  word change_mask = SND_NO_CHANGE_M;
  /* This buffer is used to make a single tone look like a sound
  ** when being processed, to simplify the tone processing algorithms.
  ** It is made large enough to support the key delay functions.
  ** Two tones are needed for each delayed key (1 for the inter-digit)
  ** silence. One addional tone is needed at the end.
  */
  static snd_flexible_tone_type  single_tone_buf[ SND_TONE_ARRAY_SIZE ];
  static snd_flexible_sound_type single_tone = { SND_FLEXIBLE_SOUND, 
                                                 single_tone_buf };

#ifdef SND_HFK
  static boolean snd_tone_delayed = FALSE;
    /* Flag for controlling delay function */
  static int     snd_delay_index;
    /* The next location in the tone array for inserting a tone */
#endif

  if (cmd_ptr->hdr.cmd == SND_TONE_START){
    MSG_MED("SND CMD: SND_TONE_START=%d, D=%d, M=%d",
                cmd_ptr->tone.tone_id,
                cmd_ptr->tone.device,
                cmd_ptr->tone.method);

    /* Range check the tone against the beginning/end range markers
       Since SND_FIRST_CONTROL_TONE is equal to ZERO, the unsigned
       comparison tone >= SND_FIRST_CONTROL_TONE is unnecessary 
       because it is always true.
       The actual comparison should be: 
       if (!( ( (cmd_ptr->tone.tone_id >= SND_FIRST_TONE) && 
             (cmd_ptr->tone.tone_id <  SND_LAST_TONE ) )            ||
           ( (cmd_ptr->tone.tone_id >= SND_FIRST_CONTROL_TONE) &&
             (cmd_ptr->tone.tone_id <  SND_LAST_CONTROL_TONE ) )    ||
           ( (cmd_ptr->tone.tone_id >= SND_FIRST_AUDIO_TONE) &&
             (cmd_ptr->tone.tone_id <  SND_LAST_AUDIO_TONE ) ) ) ) {
       However removing the mentioned comparison also removes compiler warnings
    */
    if (!( ( (cmd_ptr->tone.tone_id >= SND_FIRST_TONE) && 
             (cmd_ptr->tone.tone_id <  SND_LAST_TONE ) )            ||
             (cmd_ptr->tone.tone_id <  SND_LAST_CONTROL_TONE )      ||
           ( (cmd_ptr->tone.tone_id >= SND_FIRST_AUDIO_TONE) &&
             (cmd_ptr->tone.tone_id <  SND_LAST_AUDIO_TONE ) ) ) ) {
      /* Does not return */
      ERR_FATAL("Illegal Single Tone %x", (long) cmd_ptr->tone.tone_id,0,0);
    }

    /* Accept volume changes
    */
    snd_array[ SND_TONE ].vol = cmd_ptr->tone.volume; 
   
#ifdef SND_HFK
    /* If the device or method change, stop delaying the current tone.
    ** or if new tone.
    */
    if (!snd_array[ SND_TONE ].active                          ||
        (snd_array[ SND_TONE ].device != cmd_ptr->tone.device) ||
        (snd_array[ SND_TONE ].method != cmd_ptr->tone.method)) {
      snd_array[ SND_TONE ].hold = FALSE;
      snd_tone_delayed           = FALSE;
    }
    
    /* If the current tone is on hold, and key delay is not
    ** active, begin delaying tones. Convert the tone to a 
    ** "short", and add an inter-digit silence.
    */
    if (snd_array[ SND_TONE ].hold == TRUE) {
      if (snd_tone_delayed == FALSE) {
        snd_tone_delayed = TRUE;
        snd_delay_index = 0;
        snd_array[ SND_TONE ].time_remain_ms                  = SND_KEY_TIME;
        single_tone_buf[snd_delay_index++].param.duration_ms  = SND_KEY_TIME;
        single_tone_buf[snd_delay_index].tone                 = SND_SILENCE;
        single_tone_buf[snd_delay_index++].param.duration_ms  = SND_KEY_SPACE;
        single_tone_buf[snd_delay_index].tone                 = SND_STOP;
      }
    }
   
    /* When key delay is active: If the key is a "short" tone
    ** add the new tone plus an inter-digit silence to the end of
    ** the list. If the key is a "long" tone change the duration
    ** to the standard short tone duration and add a "repeat 1"
    ** control tone to the end of the list.
    */
    if (snd_tone_delayed == TRUE) {
      /* If the tone buffer is full, over-write the last tone.
      */
      if (snd_delay_index > SND_MAX_DELAY_INDEX - 2) {
        snd_delay_index = SND_MAX_DELAY_INDEX - 2;
      }
      single_tone_buf[snd_delay_index].tone        = cmd_ptr->tone.tone_id;
      single_tone_buf[snd_delay_index].freq_hi     = cmd_ptr->tone.freq_hi;
      single_tone_buf[snd_delay_index].freq_lo     = cmd_ptr->tone.freq_lo;
      single_tone_buf[snd_delay_index++].param.duration_ms   = SND_KEY_TIME;
      if (cmd_ptr->tone.duration_ms == 0) {
        single_tone_buf[snd_delay_index].tone                = SND_RPT1;
      } else {
        single_tone_buf[snd_delay_index].tone                = SND_SILENCE;
        single_tone_buf[snd_delay_index++].param.duration_ms = SND_KEY_SPACE;
        single_tone_buf[snd_delay_index].tone                = SND_STOP;
      }
    } else
#endif /* SND_HFK */
    {
      /* Do not callback when a tone is stopped by another tone
      ** being started.  This is a convenience for the caller (e.g. UI)
      */
      snd_array[ SND_TONE ].callback_ptr = NULL;
      snd_expire( SND_TONE );
   
      /* Copy to sound buf */
      single_tone_buf[SND_STI_TONE].tone    = cmd_ptr->tone.tone_id;
      single_tone_buf[SND_STI_TONE].freq_hi = cmd_ptr->tone.freq_hi;
      single_tone_buf[SND_STI_TONE].freq_lo = cmd_ptr->tone.freq_lo;
      if ( cmd_ptr->tone.duration_ms == 0 ) {
        single_tone_buf[SND_STI_TONE].param.duration_ms = SND_MAX_PERIOD;
      } else {
        single_tone_buf[SND_STI_TONE].param.duration_ms = 
                                          cmd_ptr->tone.duration_ms;
      }

      /* If the duration is 0, this note is set up to play forever,
      ** so set next tone to SND_RPT and the tone duration to the maximum.
      */
      if ( cmd_ptr->tone.duration_ms == 0 ) {
        single_tone_buf[SND_STI_TYPE].tone = SND_RPT;

      /* Else, This tone will stop eventually, set next tone to SND_STOP */
      } else {
        single_tone_buf[SND_STI_TYPE].tone = SND_STOP;
      }

      /* Set the sound array, first duration, buffer index, callback,
      ** client_data and 'active'
      */
      snd_array[ SND_TONE ].sound_ptr   = (const snd_sound_type *) &single_tone;
      snd_array[ SND_TONE ].time_remain_ms = 
                           single_tone_buf[SND_STI_TONE].param.duration_ms;
      snd_array[ SND_TONE ].tone_index  = 0;
      snd_array[ SND_TONE ].callback_ptr= cmd_ptr->tone.hdr.callback_ptr;
      snd_array[ SND_TONE ].client_data = cmd_ptr->tone.hdr.client_data;
      snd_array[ SND_TONE ].device      = cmd_ptr->tone.device;
      snd_array[ SND_TONE ].method      = cmd_ptr->tone.method;
      snd_array[ SND_TONE ].dtmf_path   = cmd_ptr->tone.dtmf_path;
      snd_array[ SND_TONE ].active      = TRUE;
      snd_array[ SND_TONE ].hold        = FALSE;
      snd_array[ SND_TONE ].loop_cnt    = 0;
      snd_array[ SND_TONE ].label_index = 0xff;
      snd_array[ SND_TONE ].label_cnt   = 0;
      snd_array[ SND_TONE ].repeat_cnt  = 0;
      snd_array[ SND_TONE ].scaling     = 0;
      snd_array[ SND_TONE ].vol         = cmd_ptr->tone.volume;
#ifdef FEATURE_AUDIO_FORMAT
      rex_clr_sigs( &snd_tcb, SND_VOC_MODULE_STATUS );
#endif
#ifdef FEATURE_VOICE_PLAYBACK
      snd_array[ SND_TONE ].vp_playing  = FALSE;
#endif
#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
      snd_array[ SND_TONE ].age_func_ptr = (snd_age_func_ptr_type) snd_age_tone;

      /* Set tone change mask so the tone will be started but not
      ** aged the first time through.
      */
      change_mask |= SND_TONE_M;
    }
  } else { /* SND_TONE_STOP */
    MSG_MED("SND CMD: SND_TONE_STOP",0,0,0);
#ifdef SND_HFK
    /* If key delay is not active, and the current tone is on
    ** hold, convert the current tone to a "short", add an 
    ** inter-digit silence, and activate key delay. If the current
    ** tone is not on hold, then stop it normally.
    ** If key delay is active, add an inter-digit silence and
    ** change the last tone in the list to a stop, so that the tone
    ** will play for a minimum time and stop.
    */
    if (snd_tone_delayed == FALSE) {
      if (snd_array[ SND_TONE ].hold == TRUE) {
        snd_tone_delayed = TRUE;
        snd_delay_index = 0;
        snd_array[ SND_TONE ].time_remain_ms                 = SND_KEY_TIME;
        single_tone_buf[snd_delay_index++].param.duration_ms = SND_KEY_TIME;
        single_tone_buf[snd_delay_index].tone                = SND_SILENCE;
        single_tone_buf[snd_delay_index++].param.duration_ms = SND_KEY_SPACE;
        single_tone_buf[snd_delay_index].tone                = SND_STOP;
      } else {
        snd_expire( SND_TONE );
        change_mask |= SND_TONE_M;
      }
    } else {
      if (snd_delay_index == SND_MAX_DELAY_INDEX) {
        snd_delay_index = SND_MAX_DELAY_INDEX - 1;
      }
      single_tone_buf[snd_delay_index].tone                = SND_SILENCE;
      single_tone_buf[snd_delay_index++].param.duration_ms = SND_KEY_SPACE;
      single_tone_buf[snd_delay_index].tone                = SND_STOP;
    }
#else
    snd_expire( SND_TONE );
    change_mask |= SND_TONE_M;
#endif /* SND_HFK */
  }
  return (change_mask);
} /* end of snd_tone_req */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_id_start_req

DESCRIPTION
  Start to play a sound from the database

DEPENDENCIES
  None

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  snd_array

===========================================================================*/
word snd_sound_id_start_req ( snd_packets_type *cmd_ptr )
{
  word               change_mask = SND_NO_CHANGE_M;
  snd_array_idx_type sound_index;
  snd_tone_type      tone;           /* temporary tone indicator */
  snd_tone_data_type tone_data;      /* temporary tone data  */

  MSG_MED("SND CMD:SND_SOUND_ID_START=%d,D=%d,M=%d",
                 cmd_ptr->sound_id.sound_id,
                 cmd_ptr->sound_id.device,
                 cmd_ptr->sound_id.method);

  MSG_MED("Volume=%d, priority=%d", cmd_ptr->sound_id.volume,
                                          cmd_ptr->sound_id.priority, 0);

  if(cmd_ptr->sound_id.priority == SND_PRIO_LOW) {
    sound_index = SND_SND_LO;
    /* Set low-pri sound change mask so the low-pri sound will be started
    ** but not aged the first time through.
    */
    change_mask |= SND_SND_LO_M;
  } else if(cmd_ptr->sound_id.priority == SND_PRIO_MED) {
    sound_index = SND_SND_MED;
    /* Set med-pri sound change mask so the med-pri sound will be started 
    ** but not aged the first time through.
    */
    change_mask |= SND_SND_MED_M;
  } else {
    sound_index = SND_SND_HI;
    /* Set high-pri sound change mask so the high-pri sound will be started 
    ** but not aged the first time through.
    */
    change_mask |= SND_SND_HI_M;
  }

  snd_expire( sound_index );

  if (sdb_get_sound(cmd_ptr->sound_id.sound_id,
                    &snd_array[ sound_index ].sound_ptr) == SDB_FAIL) {
    ERR("Sound Id not in Database id=%d", cmd_ptr->sound_id.sound_id,0,0);
    if ( cmd_ptr->sound_id.hdr.callback_ptr != NULL ) {
      cmd_ptr->sound_id.hdr.callback_ptr(cmd_ptr->hdr.client_data, SND_FAILURE);
      cmd_ptr->sound_id.hdr.callback_ptr = NULL;
    }
    return (change_mask);
  }

#ifdef FEATURE_AUDIO_FORMAT
  if((snd_array[sound_index].sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
     (snd_array[sound_index].sound_ptr->type == SND_FORMAT_SOUND)) {
    if(!snd_highest_priority(sound_index)) {
      /* if higher priority sound with VP playing then 
      ** return status of PROMPT BUSY 
      */
      if ( cmd_ptr->sound.hdr.callback_ptr != NULL ) {
        cmd_ptr->sound_id.hdr.callback_ptr (cmd_ptr->sound_id.hdr.client_data,
                                            SND_PROMPT_BUSY);
        cmd_ptr->sound_id.hdr.callback_ptr = NULL;
      }
      return(change_mask);
    } else {
      if(snd_setup_audfmt(&(snd_array[sound_index]), 
                          cmd_ptr->sound_id.hdr.callback_ptr, 
                          cmd_ptr->sound_id.hdr.client_data) == SND_FAILURE) {
        cmd_ptr->sound_id.hdr.callback_ptr = NULL;
        return(change_mask);
      }
    }
  }
  else
#endif /* FEATURE_AUDIO_FORMAT */
  {
    /* Get first tone and duration in sound */
    snd_get_sound_info(snd_array[sound_index].sound_ptr, 0, &tone, &tone_data);
    snd_array[sound_index].age_func_ptr = (snd_age_func_ptr_type) snd_age_tone;
    snd_array[sound_index].time_remain_ms = tone_data.duration_ms;
    snd_array[sound_index].method         = cmd_ptr->sound_id.method;
  }

  snd_array[ sound_index ].tone_index     = 0;
  snd_array[ sound_index ].callback_ptr   = cmd_ptr->sound_id.hdr.callback_ptr;
  snd_array[ sound_index ].client_data    = cmd_ptr->sound_id.hdr.client_data;
  snd_array[ sound_index ].device         = cmd_ptr->sound_id.device;
  snd_array[ sound_index ].vol            = cmd_ptr->sound_id.volume;
  snd_array[ sound_index ].dtmf_path      = cmd_ptr->sound_id.dtmf_path;
  snd_array[ sound_index ].active         = TRUE;
  snd_array[ sound_index ].hold           = FALSE;
  snd_array[ sound_index ].loop_cnt       = 0;
  snd_array[ sound_index ].label_index    = 0xff;
  snd_array[ sound_index ].label_cnt      = 0;
  snd_array[ sound_index ].repeat_cnt     = cmd_ptr->sound_id.repeat_cnt;
  snd_array[ sound_index ].scaling        = 0; 
#ifdef FEATURE_VOICE_PLAYBACK
  snd_array[ sound_index ].vp_playing     = FALSE;
#endif
#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
  MSG_MED("Start sound, %d, pri, %d", cmd_ptr->sound_id.sound_id, 
                                      cmd_ptr->sound_id.priority, 0);

  /* Initialize buffer underrun flag */
  snd_array[ sound_index ].buffer_underrun_flag = FALSE;
#ifdef FEATURE_AUDIO_FORMAT
      rex_clr_sigs( &snd_tcb, SND_VOC_MODULE_STATUS );
#endif
  return (change_mask);
} /* end of snd_sound_id_start_req */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_start_req

DESCRIPTION
  Start to play a sound from a list of tones

DEPENDENCIES
  None

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  snd_array
  snd_vp_callback_func

===========================================================================*/
word snd_sound_start_req ( snd_packets_type *cmd_ptr )
{
  word               change_mask = SND_NO_CHANGE_M;
  snd_array_idx_type sound_index;
  snd_tone_type      tone = SND_LAST_TONE; /* temporary tone indicator */
  snd_tone_data_type tone_data;            /* temporary tone data      */

#ifdef FEATURE_AUDIO_FORMAT
  snd_audfmt_sound_type *current_af_ptr;
  snd_audfmt_sound_type *new_af_ptr;
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_VOICE_PLAYBACK
  voc_pb_header_type **prompt_list;
    /* pointer to array of voice prompts */
#endif

  MSG_MED("SND CMD: SND_SOUND_START D=%d, M=%d, P=%d",
                          cmd_ptr->sound.device,
                          cmd_ptr->sound.method,
                          cmd_ptr->sound.priority);

// verify if last tone is STOP,..., return error if not
//        snd_get_sound_info(cmd_ptr->sound.sound_ptr, 0, &tone, &tone_data);
//        if((tone != SND_RPT)  && (tone != SND_STOP) &&
//           (tone != SND_RPT1) && (tone != SND_RPT_NOSIGNAL)) {
//          // return error.
//        }

  if(cmd_ptr->sound.priority == SND_PRIO_LOW) {
    sound_index       = SND_SND_LO;
    /* Set low-pri sound change mask so the low-pri sound will be started 
    ** but not aged the first time through.
    */
    change_mask    |= SND_SND_LO_M;
  } else if(cmd_ptr->sound.priority == SND_PRIO_MED) {
    sound_index       = SND_SND_MED;
    /* Set med-pri sound change mask so the med-pri sound will be started 
    ** but not aged the first time through.
    */
    change_mask    |= SND_SND_MED_M;
  } else {
    sound_index       = SND_SND_HI;
    /* Set high-pri sound change mask so the high-pri sound will be started 
    ** but not aged the first time through.
    */
    change_mask    |= SND_SND_HI_M;
  }


#ifdef FEATURE_AUDIO_FORMAT
  if((cmd_ptr->sound.sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
     (cmd_ptr->sound.sound_ptr->type == SND_FORMAT_SOUND)) {
    if(!snd_highest_priority(sound_index)) {
      snd_expire( sound_index );

      /* if higher priority sound with VP playing then 
      ** return status of PROMPT BUSY 
      */
      if ( cmd_ptr->sound.hdr.callback_ptr != NULL ) {
        cmd_ptr->sound.hdr.callback_ptr (cmd_ptr->sound.hdr.client_data,
                                         SND_PROMPT_BUSY);
        cmd_ptr->sound.hdr.callback_ptr = NULL;
      }
      return(change_mask);
    } else {
      if(((snd_array[ sound_index ].sound_ptr != NULL) &&
          (snd_array[ sound_index ].sound_ptr->type != SND_AUDIO_FORMAT_SOUND))
           || (cmd_ptr->sound.sound_ptr->type != SND_AUDIO_FORMAT_SOUND)) {
        snd_expire( sound_index );
      } else {
        current_af_ptr = (snd_audfmt_sound_type *)
                                   snd_array[sound_index].sound_ptr;
        new_af_ptr     = (snd_audfmt_sound_type *)
                                   cmd_ptr->sound.sound_ptr;

        if(((current_af_ptr != NULL) && (!current_af_ptr->param.multiseq_flag)) ||
           (((new_af_ptr != NULL) && !new_af_ptr->param.multiseq_flag))) {
          snd_expire( sound_index );
        }
      }
      snd_array[sound_index].sound_ptr = cmd_ptr->sound.sound_ptr;
      snd_array[sound_index].method    = cmd_ptr->sound.method;

      if(snd_setup_audfmt(&(snd_array[sound_index]), 
                          cmd_ptr->sound.hdr.callback_ptr, 
                          cmd_ptr->sound.hdr.client_data) == SND_FAILURE) {
        cmd_ptr->sound.hdr.callback_ptr = NULL;
        return(change_mask);
      }
    }
  }
  else
#endif /* FEATURE_AUDIO_FORMAT */

  {
    snd_expire( sound_index );
    snd_array[ sound_index ].sound_ptr = cmd_ptr->sound.sound_ptr;

#ifdef FEATURE_VOICE_PLAYBACK
    if(cmd_ptr->sound.sound_ptr->type == SND_TONE_AND_VOICE) {

      if(!snd_highest_priority(sound_index) 
#ifdef FEATURE_MIDI_OUT
         || (snd_midi_out_enabled != SND_MIDI_OUT_STATE_DISABLED) 
#endif /* FEATURE_MIDI_OUT */
        ) {
        /* if higher priority sound with VP playing then 
        ** return status of PROMPT BUSY 
        */
        if ( cmd_ptr->sound.hdr.callback_ptr != NULL ) {
          cmd_ptr->sound.hdr.callback_ptr (cmd_ptr->sound.hdr.client_data,
                                           SND_PROMPT_BUSY);
          cmd_ptr->sound.hdr.callback_ptr = NULL;
        }
        return(change_mask);
      }

      /* Enqueue all the voice prompt pointers. */
      prompt_list = ((snd_tone_and_voice_sound_type *) 
                     (cmd_ptr->sound.sound_ptr))->prompt_list;

      if(prompt_list != NULL) {
        (void) sndhw_ppq_set_list(prompt_list);
      } else {
        /* invalid prompt list entered */
        if ( cmd_ptr->sound.hdr.callback_ptr != NULL ) {
          cmd_ptr->sound.hdr.callback_ptr (cmd_ptr->sound.hdr.client_data,
                                           SND_FAILURE);
          cmd_ptr->sound.hdr.callback_ptr = NULL;
        }
        return(change_mask);
      }

      snd_vp_callback_func = cmd_ptr->sound.hdr.callback_ptr;
    }
#endif

    /* get first tone and duration of sound */
    snd_get_sound_info(cmd_ptr->sound.sound_ptr, 0, &tone, &tone_data);
    snd_array[sound_index].time_remain_ms = tone_data.duration_ms;
    snd_array[sound_index].age_func_ptr = (snd_age_func_ptr_type) snd_age_tone;
    snd_array[sound_index].method       = cmd_ptr->sound.method;
  }

#ifdef FEATURE_VOICE_PLAYBACK
  if(tone == SND_VOICE_PROMPT) {
    snd_array[ sound_index ].time_remain_ms = 0;
  } 
#endif
  snd_array[ sound_index ].tone_index       = 0;
  snd_array[ sound_index ].callback_ptr     = cmd_ptr->sound.hdr.callback_ptr;
  snd_array[ sound_index ].client_data      = cmd_ptr->sound.hdr.client_data;
  snd_array[ sound_index ].device           = cmd_ptr->sound.device;
  snd_array[ sound_index ].vol              = cmd_ptr->sound.volume;
  snd_array[ sound_index ].dtmf_path        = cmd_ptr->sound.dtmf_path;
  snd_array[ sound_index ].active           = TRUE;
  snd_array[ sound_index ].hold             = FALSE;
  snd_array[ sound_index ].loop_cnt         = 0;
  snd_array[ sound_index ].label_index      = 0xff;
  snd_array[ sound_index ].label_cnt        = 0;
  snd_array[ sound_index ].repeat_cnt       = cmd_ptr->sound.repeat_cnt;
  snd_array[ sound_index ].scaling          = 0; 
#ifdef FEATURE_VOICE_PLAYBACK
  snd_array[ sound_index ].vp_playing       = FALSE;
#endif
#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
  MSG_MED("Start sound ptr, pri, %d", cmd_ptr->sound.priority, 0, 0);

    /* Initialize buffer underrun flag */
  snd_array[ sound_index ].buffer_underrun_flag = FALSE;
#ifdef FEATURE_AUDIO_FORMAT
  rex_clr_sigs( &snd_tcb, SND_VOC_MODULE_STATUS );
#endif
  return (change_mask);
} /* end of snd_sound_start_req */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_stop_req

DESCRIPTION
  Stop playing a sound

DEPENDENCIES
  snd_array

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  snd_array

===========================================================================*/
word snd_sound_stop_req ( snd_packets_type *cmd_ptr )
{
  word change_mask = SND_NO_CHANGE_M;
#ifdef FEATURE_VOICE_PLAYBACK
  /* Structures used to stop sounds with voice prompts */
  static snd_compact_tone_type         sound_stop_buf[2];
  static snd_tone_and_voice_sound_type sound_stop = 
                                                 {SND_TONE_AND_VOICE, 
                                                  sound_stop_buf, 
                                                  NULL};
  snd_compact_tone_type *tone_compact_ptr;
    /* pointer used to traverse tone array */
#endif
  int sound_index, sound_index_max;
  
  MSG_MED("SND CMD: SND_STOP_SOUND pri=%d", 
                cmd_ptr->snd_stop.priority,0,0);

  if(cmd_ptr->snd_stop.priority == SND_PRIO_LOW) {
    sound_index      = SND_SND_LO;
    sound_index_max  = SND_SND_LO;
  } else if(cmd_ptr->snd_stop.priority == SND_PRIO_MED) {
    sound_index      = SND_SND_MED;
    sound_index_max  = SND_SND_MED;
  } else if(cmd_ptr->snd_stop.priority == SND_PRIO_HIGH) {
    sound_index      = SND_SND_HI;
    sound_index_max  = SND_SND_HI;
  } else {
    sound_index      = SND_SND_LO;
    sound_index_max  = SND_SND_HI;
  }
  change_mask     |= SND_CHANGE_M;

  for(; sound_index <= sound_index_max; sound_index++) {
#ifdef FEATURE_VOICE_PLAYBACK
    if(snd_array[sound_index].vp_playing)
    {
      tone_compact_ptr = &((((snd_tone_and_voice_sound_type *)
                           (snd_array[sound_index].sound_ptr))->tone_array)
                           [snd_array[sound_index].tone_index]);
      if(tone_compact_ptr->tone == SND_VOICE_PROMPT) {
      /* Change the next tone to SND_STOP so that when the vocoder is
      ** done ending the VPs, the snd task will know to expire this
      ** sound.
      */
        sound_stop_buf[0].tone          = SND_VOICE_PROMPT;
        sound_stop_buf[1].tone          = SND_STOP;

        snd_array[ sound_index ].sound_ptr      =
                               (const snd_sound_type *) &sound_stop;
        snd_array[ sound_index ].time_remain_ms = 0;
        snd_array[ sound_index ].tone_index     = 0;

      } else {
      /* Should never reach this point, if there is a VP playing then
      ** the tone pointer should always be pointing to a 
      ** SND_VOICE_PROMPT tone type.
      */
        ERR("Tone ptr pointing to VP but VP not playing!",0,0,0);
      }
      sndhw_abort_vp();
    }
    else
#endif /* FEATURE_VOICE_PLAYBACK */
    {
      snd_expire( (snd_array_idx_type) sound_index );
#ifdef FEATURE_AUDIO_QENSEMBLE		
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
	  
    }
  }
  return (change_mask);
} /* end of snd_sound_stop_req */
/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION snd_vp_play_req

DESCRIPTION
  Start to play a voice prompt

DEPENDENCIES
  None

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  snd_array
  snd_vp_callback_func

===========================================================================*/
word snd_vp_play_req ( snd_packets_type *cmd_ptr )
{
  word change_mask = SND_NO_CHANGE_M;
    /* pointer to array of voice prompts */

  /* sound structure used to play voice prompt */
  static snd_compact_tone_type  single_voice_prompt_buf[2];
  static snd_compact_sound_type single_voice_prompt = {SND_COMPACT_SOUND, 
                                                     single_voice_prompt_buf};
#ifdef FEATURE_AUDIO_FORMAT
  int counter;

  for(counter = SND_SND_LO; counter <= SND_SND_HI; counter++) {
    /* Look for audio format sounds of higher priority. If any, return
    ** prompt busy.
    */
    if((snd_array[counter].active) && 
       ((snd_array[counter].sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
        (snd_array[counter].sound_ptr->type == SND_FORMAT_SOUND))) {
      MSG_HIGH("Sound is currently playing voice prompts.",0,0,0);
      if ( cmd_ptr->vp.hdr.callback_ptr != NULL ) {
        cmd_ptr->vp.hdr.callback_ptr (cmd_ptr->vp.hdr.client_data, 
                                      SND_PROMPT_BUSY);
        /* If no sounds with voice prompts are playing */
        if ( sndhw_ppq_internal_q() ) {
          cmd_ptr->vp.hdr.callback_ptr(NULL, SND_PROMPT_NONE);
        }
        cmd_ptr->vp.hdr.callback_ptr = NULL;
        return(change_mask);
      }
    }
  }
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_MIDI_OUT
  if(snd_midi_out_enabled != SND_MIDI_OUT_STATE_DISABLED) {
    if ( cmd_ptr->vp.hdr.callback_ptr != NULL ) {
      cmd_ptr->vp.hdr.callback_ptr (cmd_ptr->vp.hdr.client_data,
                                    SND_PROMPT_BUSY);
      cmd_ptr->vp.hdr.callback_ptr = NULL;
      return(change_mask);
    }
  }
#endif /* FEATURE_MIDI_OUT */

  /* If no sounds with voice prompts are playing */
  if(sndhw_ppq_internal_q()) {
    SND_DEBUG_MSG("SND CMD: SND_VOICE_PROMPT_PLAY=%d, D=%d",
                      cmd_ptr->vp.vp,
                      cmd_ptr->vp.device, 0);

    /* Enqueue voice prompt pointer */
    if( snd_enqueue_vp( cmd_ptr->vp.vp, cmd_ptr->vp.hdr.client_data,
                        cmd_ptr->vp.hdr.callback_ptr ) == SND_PPQ_PQ_FAIL )
    {
      cmd_ptr->vp.hdr.callback_ptr = NULL;
      return(change_mask);
    }

    single_voice_prompt_buf[0].tone          = SND_VOICE_PROMPT;
    single_voice_prompt_buf[1].tone          = SND_STOP;

    snd_vp_callback_func = cmd_ptr->vp.hdr.callback_ptr;

    /* Set the sound array, first duration, buffer index, callback,
    ** client_data and 'active'
    */
    snd_array[ SND_PLAYBACK ].sound_ptr      = 
                                 (const snd_sound_type *) &single_voice_prompt;
    snd_array[ SND_PLAYBACK ].time_remain_ms = 0;
    snd_array[ SND_PLAYBACK ].tone_index     = 0;
    snd_array[ SND_PLAYBACK ].callback_ptr   = cmd_ptr->vp.hdr.callback_ptr;
    snd_array[ SND_PLAYBACK ].client_data    = cmd_ptr->vp.hdr.client_data;
    snd_array[ SND_PLAYBACK ].device         = cmd_ptr->vp.device;
    snd_array[ SND_PLAYBACK ].method         = SND_METHOD_VOICE;
    snd_array[ SND_PLAYBACK ].vol            = SND_VOL_USE_DEFAULT;
    snd_array[ SND_PLAYBACK ].dtmf_path      = SND_APATH_LOCAL;
    snd_array[ SND_PLAYBACK ].active         = TRUE;
    snd_array[ SND_PLAYBACK ].hold           = FALSE;
    snd_array[ SND_PLAYBACK ].loop_cnt       = 0;
    snd_array[ SND_PLAYBACK ].label_index    = 0xff;
    snd_array[ SND_PLAYBACK ].label_cnt      = 0;
    snd_array[ SND_PLAYBACK ].repeat_cnt     = 0;
    snd_array[ SND_PLAYBACK ].scaling        = 0;
    snd_array[ SND_PLAYBACK ].vp_playing     = FALSE;
    snd_array[ SND_PLAYBACK ].age_func_ptr   = 
                                          (snd_age_func_ptr_type) snd_age_tone;

    /* Set tone change mask so the tone will be started but not
    ** aged the first time through.
    */
    change_mask |= SND_PLAYBACK_M;
#ifdef FEATURE_AUDIO_FORMAT
      rex_clr_sigs( &snd_tcb, SND_VOC_MODULE_STATUS );
#endif
  } else {
    MSG_HIGH("Sound is currently playing voice prompts.",0,0,0);
    if ( cmd_ptr->vp.hdr.callback_ptr != NULL ) {
      cmd_ptr->vp.hdr.callback_ptr (cmd_ptr->vp.hdr.client_data, SND_PROMPT_BUSY);
      cmd_ptr->vp.hdr.callback_ptr = NULL;
    }
  }
  return (change_mask);
} /* end of snd_vp_play_req */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_vp_stop_req

DESCRIPTION
  Stop playing a voice prompt

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_vp_stop_req ( 
  snd_packets_type *cmd_ptr 
  /*lint -esym(715,cmd_ptr) Tell Lint we know cmd_ptr parameter is not used */
) {
  SND_DEBUG_MSG("SND CMD: SND_VOICE_PROMPT_STOP",0,0,0);
  sndhw_abort_vp();
} /* end of snd_vp_stop_req */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_vp_sig_req

DESCRIPTION
  Process vocoder voice prompt playback signal.

DEPENDENCIES
  snd_vp_sig_num

RETURN VALUE
  None

SIDE EFFECTS
  snd_vp_status
  snd_vp_client_data
  snd_vp_sig_num

===========================================================================*/
void snd_vp_sig_req ( snd_packets_type *cmd_ptr )
{
  if(snd_vp_sig_num < SND_NUM_VP_SIGS) {
    /* save status of voice playback into snd_vp_status */
    snd_vp_status[snd_vp_sig_num]      = cmd_ptr->vp_sig.vp_status;

    /* save client data of voice prompt into snd_vp_client_data */
    snd_vp_client_data[snd_vp_sig_num] = cmd_ptr->vp_sig.client_data;

    snd_vp_sig_num++;

  } else {
    MSG_ERROR("Overflow!",0,0,0);
  }
} /* end of snd_vp_sig_req */
#endif /* FEATURE_VOICE_PLAYBACK */
/* <EJECT> */
#ifdef FEATURE_VOICE_MEMO
/*===========================================================================

FUNCTION snd_vp_pause_resume_req

DESCRIPTION
  Pause the current voice prompt

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_vp_pause_resume_req ( snd_packets_type *cmd_ptr )
{
  switch(cmd_ptr->vp_control.play_record) {
    case SND_VP_PLAY_PAUSE:
      voc_pb_pause();
      break;
  
    case SND_VP_PLAY_RESUME:
      voc_pb_resume(FALSE);
      break;

    default:
      ERR_FATAL("Command not supported: %d",
                                       cmd_ptr->vp_control.play_record,0,0);
      break;
  }
} /* end of snd_vp_pause_resume_req */
#endif /* FEATURE_VOICE_MEMO */
/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION snd_audfmt_wait

DESCRIPTION
  This function waits a maximum of SND_AUDFMT_DATA_TIMEOUT until 
  SND_AUDIO_FORMAT_SIG is set.

DEPENDENCIES
  snd_audfmt_data_timer

RETURN VALUE
  FALSE if wait timed out.
  TRUE if signal was set properly.

SIDE EFFECTS
  snd_audfmt_data_timer

===========================================================================*/
boolean snd_audfmt_wait()
{
  rex_sigs_type sigs;
  boolean       ret_val = TRUE;

  sigs = rex_get_sigs(&snd_tcb);

  if((sigs & SND_AUDIO_FORMAT_SIG) != SND_AUDIO_FORMAT_SIG) {
    (void) rex_set_timer( &snd_audfmt_data_timer, SND_AUDFMT_DATA_TIMEOUT );

    while((sigs & SND_AUDIO_FORMAT_SIG) != SND_AUDIO_FORMAT_SIG) {
      dog_report(DOG_SND_RPT);
      (void) rex_set_timer( &snd_rpt_timer, DOG_SND_RPT_TIME );
      sigs = rex_wait(SND_AUDIO_FORMAT_SIG|SND_RPT_TIMER_SIG);
    }

    if(rex_get_timer(&snd_audfmt_data_timer) == 0) {
      ret_val = FALSE;
    }
  }

  (void) rex_clr_sigs(&snd_tcb, SND_AUDIO_FORMAT_SIG);
  (void) rex_clr_timer(&snd_audfmt_data_timer);

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_continue

DESCRIPTION
  This function sets the SND_AUDIO_FORMAT_SIG signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_audfmt_continue()
{
  (void) rex_set_sigs(&snd_tcb, SND_AUDIO_FORMAT_SIG);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_do_seek

DESCRIPTION
  This function does the actual work in fast forwarding a midi/WebAudio file.

DEPENDENCIES
  snd_af_ff_ctl

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  snd_af_ff_ctl

===========================================================================*/
word snd_audfmt_do_seek ( void )
{
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr;
  audmain_status_type   aud_status;

  word ret_val = 0;

  if(snd_af_ff_ctl != NULL) {
    aud_status = audmain_do_seek(snd_af_ff_ctl, &snd_audfmt_elapsed_ms, 
                                 &snd_audfmt_adjusted_ms);

    switch(aud_status) {
      case AUDMAIN_STATUS_SUCCESS:
      case AUDMAIN_STATUS_DONE:
        if(!audmain_get_pause(snd_af_ff_ctl)) {
          if(snd_af_ff_pri == SND_PRIO_LOW) {
            snd_ptr = &(snd_array[SND_SND_LO]);
          } else if(snd_af_ff_pri == SND_PRIO_MED) {
            snd_ptr = &(snd_array[SND_SND_MED]);
          } else {
            snd_ptr = &(snd_array[SND_SND_HI]);
          }

          audfmt_sound_ptr = (snd_audfmt_sound_type *) (snd_ptr->sound_ptr);

          /* Send resume status to client with elapsed time */
          audmain_play_cb(snd_ptr->audfmt_ctl,
                          &(audfmt_sound_ptr->audfmt_handle), 
                          SND_CMX_AF_RESUME, snd_audfmt_adjusted_ms, NULL);
        }
        /* Fall thru */

      case AUDMAIN_STATUS_ERROR:
      case AUDMAIN_STATUS_FAILURE:
        ret_val = (SND_SND_LO_M << snd_af_ff_pri);

        snd_audfmt_next_update = 
                          snd_audfmt_adjusted_ms + SND_AUDFMT_TICK_INTERVAL;
        snd_af_ff_ctl          = NULL;
        clk_uptime_ms(snd_audfmt_interval);
        break;

      case AUDMAIN_STATUS_PENDING:
        switch (audmain_get_parse_type(snd_af_get_time_ctl)) {
#if defined(FEATURE_MIDI) || defined(FEATURE_AAC)
#ifdef FEATURE_MIDI
          case AUDMAIN_MIDI_TYPE: 
#endif 
#ifdef FEATURE_AAC
          case AUDMAIN_AAC_TYPE: 
#endif
            snd_af_ff_ctl = NULL;
            break;
#endif /* FEATURE_MIDI || FEATURE_AAC */
         
          default:
            break;
        }
        break;

      case AUDMAIN_STATUS_BUFF_ERR:
      default:
        break;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_reset_buffers

DESCRIPTION
  This function is used to flush the buffers before performing an actual
  seek. Must be called before and_audfmt_do_seek() when performing a
  fast-forward or rewind action.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void snd_audfmt_reset_buffers (
  void *audfmt_ctl
) {
  switch ( audmain_get_parse_type(audfmt_ctl) ) {
#ifdef FEATURE_QCP
    case AUDMAIN_QCP_TYPE:
      sndhw_abort_vp();
      sndhw_ppq_free_all();
      sndhw_pq_free_all();
      break;
#endif /* FEATURE_QCP */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
#ifdef FEATURE_MP3
    case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
    case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
    case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_PCM
    case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_WMA
    case AUDMAIN_WMA_TYPE:
    case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_AMR_WB_AUDIO_DEC
    case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif  /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
    case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
#ifdef FEATURE_VOC_G711
    case AUDMAIN_WAV_MULAW_TYPE:
    case AUDMAIN_WAV_ALAW_TYPE:
#endif /* FEATURE_VOC_G711 */
      sndhw_audfmt_adec_flush(1);
      if(!audmain_get_pause_flag(audfmt_ctl)) {
        sndhw_audfmt_adec_pause(FALSE);
      }
      break;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */
    default:
      break;
  }
}

/*===========================================================================

FUNCTION snd_vbr_check_ffwd

DESCRIPTION
  This function re-check the VBR fforward when the target frame is found

DEPENDENCIES
  audmain_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_vbr_check_ffwd (
  uint32 adjusted_ms,
  uint32 elapsed_ms,
  uint32 ms_forward
)
{

  snd_audfmt_adjusted_ms =
  snd_audfmt_elapsed_ms  = elapsed_ms  + ms_forward;

  MSG_MED("ADTS FFWD done, elapsed_ms = %d, ms_forward = %d",
          elapsed_ms,ms_forward,0);

  MSG_MED("ADTS FFWD done, snd_audfmt_elapsed_ms = %d, ms_forward = %d",
          snd_audfmt_elapsed_ms,ms_forward,0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_ff

DESCRIPTION
  This function initiates the midi/WebAudio fast forward mechanism.

DEPENDENCIES
  snd_array

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  snd_af_ff_ctl
  snd_af_ff_pri

===========================================================================*/
word snd_audfmt_ff (
  snd_priority_type priority,
  uint32            time_ms
) {
  word                  ret_val = 0;
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr = NULL;

  if(priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if(priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  /* Set audio format fast-forward parameters */
  snd_af_ff_ctl        = snd_ptr->audfmt_ctl;
  snd_af_ff_pri        = priority;

  if(audmain_fforward(snd_af_ff_ctl, time_ms, &snd_audfmt_elapsed_ms, 
                      &snd_audfmt_adjusted_ms) == AUDMAIN_STATUS_SUCCESS) {
    snd_audfmt_reset_buffers(snd_af_ff_ctl);
    ret_val = snd_audfmt_do_seek();
  } else {

    switch(audmain_get_parse_type(snd_ptr->audfmt_ctl)) {
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_MULAW_TYPE:
      case AUDMAIN_WAV_ALAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        if(!audmain_get_pause_flag(snd_ptr->audfmt_ctl)) {
          sndhw_audfmt_adec_pause(FALSE);
        }
        break;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

      default:
        break;
    }

    if(!audmain_get_pause_flag(snd_ptr->audfmt_ctl)) {
      audfmt_sound_ptr = (snd_audfmt_sound_type *) snd_ptr->sound_ptr;
      audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle), 
                      SND_CMX_AF_RESUME, snd_audfmt_adjusted_ms, NULL);
    }

    ret_val = (SND_SND_LO_M << snd_af_ff_pri);
    snd_af_ff_ctl = NULL;
  }

  return( ret_val );
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_flush_req

DESCRIPTION
  This function flushes buffers and restarts playing

DEPENDENCIES
  snd_array

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  None

===========================================================================*/
word snd_sound_flush_req (
  snd_priority_type     priority,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  word                  ret_val = 0;
  snd_status_type       status  = SND_FAILURE;
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr;

  if(priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if(priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  if((snd_ptr->active == TRUE) && 
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {

    switch ( audmain_get_parse_type(snd_ptr->audfmt_ctl) ) {
#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
#endif
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_MULAW_TYPE:
      case AUDMAIN_WAV_ALAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        audfmt_sound_ptr = (snd_audfmt_sound_type *) (snd_ptr->sound_ptr);
        audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle),
                        SND_CMX_AF_FLUSHED, 0, NULL);

        if (audmain_flush(snd_ptr->audfmt_ctl) == AUDMAIN_STATUS_SUCCESS) {
          snd_audfmt_reset_buffers(snd_ptr->audfmt_ctl);
          status = SND_SUCCESS;
        }
        break;

      default:
        break;
    }
  }

  if(callback_ptr != NULL) {
    callback_ptr ( client_data, status );
  }
  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_ff_req

DESCRIPTION
  This function initiates the fast forward mechanism if possible.

DEPENDENCIES
  snd_array
  snd_audfmt_elapsed_ms

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  None

===========================================================================*/
word snd_sound_ff_req (
  snd_priority_type     priority,
  uint32                time_ms,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  word                  ret_val = 0;
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr;

  if(priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if(priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  if((snd_ptr->active) && 
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {
    /* Fast-forward is currently only supported for audio format sounds */

    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_SUCCESS );
    }

    audfmt_sound_ptr = (snd_audfmt_sound_type *) (snd_ptr->sound_ptr);
    audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle),
                    SND_CMX_AF_FFORWARD, snd_audfmt_adjusted_ms, NULL);

    /* Initiate fast-forward sequence */
    ret_val = snd_audfmt_ff(priority, time_ms);

  } else if(callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_rewind

DESCRIPTION
  This function initiates the rewind mechanism for an audio format sound.

DEPENDENCIES
  snd_array
  snd_audfmt_elapsed_ms

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  snd_array
  snd_audfmt_elapsed_ms

===========================================================================*/
word snd_audfmt_rewind (
  snd_priority_type     priority,
  uint32                rewind_ms
) {
  word                  ret_val = 0;
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr = NULL;

  if(priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if(priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  /* Set audio format fast-forward parameters */
  snd_af_ff_ctl        = snd_ptr->audfmt_ctl;
  snd_af_ff_pri        = priority;

  if(audmain_rewind(snd_af_ff_ctl, rewind_ms, &snd_audfmt_elapsed_ms,
                    &snd_audfmt_adjusted_ms) == AUDMAIN_STATUS_SUCCESS) {
    snd_audfmt_reset_buffers(snd_af_ff_ctl);
    ret_val = snd_audfmt_do_seek();
  } else {

    switch(audmain_get_parse_type(snd_ptr->audfmt_ctl)) {
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_MULAW_TYPE:
      case AUDMAIN_WAV_ALAW_TYPE:
#endif /* FEATURE_VOC_G711 */
      if(!audmain_get_pause_flag(snd_ptr->audfmt_ctl)) {
        sndhw_audfmt_adec_pause(FALSE);
      }
      break;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

      default:
        break;
    }

    if(!audmain_get_pause_flag(snd_ptr->audfmt_ctl)) {
      audfmt_sound_ptr = (snd_audfmt_sound_type *) snd_ptr->sound_ptr;
      audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle), 
                      SND_CMX_AF_RESUME, snd_audfmt_adjusted_ms, NULL);
    }

    ret_val = (SND_SND_LO_M << snd_af_ff_pri);
    snd_af_ff_ctl = NULL;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_rewind_req

DESCRIPTION
  This function initiates the rewind mechanism, if possible.

DEPENDENCIES
  snd_array
  snd_audfmt_elapsed_ms

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  None

===========================================================================*/
word snd_sound_rewind_req (
  snd_priority_type     priority,
  uint32                rewind_ms,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  word                  ret_val = 0;
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr;

  if(priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if(priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  if((snd_ptr->active) && 
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {

    /* Rewind is currently only supported for audio format sounds */
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_SUCCESS );
    }
  
    audfmt_sound_ptr = (snd_audfmt_sound_type *) (snd_ptr->sound_ptr);
  
    audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle),
                    SND_CMX_AF_REWIND, snd_audfmt_adjusted_ms, NULL);

    /* Initiate rewind sequence */
    ret_val = snd_audfmt_rewind( priority, rewind_ms );

  } else if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }

  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_pause_req

DESCRIPTION
  This function pause a midi/WebAudio playback.

DEPENDENCIES
  snd_array

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  snd_array

===========================================================================*/
word snd_sound_pause_req (
  snd_priority_type     priority,
  boolean               pause,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  word                  ret_val = 0;
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr;
  snd_af_data_cmd_enum  cmx_pause;
  audmain_file_type     parse_type;
  qword   tqword2, tqword1 = {0,0};   /* used to calculate elapsed time      */

  if(priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if(priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  if((snd_ptr->active) && 
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {

    /* Pause\resume is currently only supported for audio format sounds */
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_SUCCESS );
    }
  
    /* Set pause\resume flag for audio format sound */
    audmain_set_pause(snd_ptr->audfmt_ctl, pause);

    audfmt_sound_ptr = (snd_audfmt_sound_type *) (snd_ptr->sound_ptr);

    parse_type = audmain_get_parse_type(snd_ptr->audfmt_ctl);

    if (pause) {
      if(qw_cmp(snd_audfmt_interval, tqword1) != 0) {
        /* Calculate the exact elapsed time since the last service for
        ** audio format sounds.
        */
        clk_uptime_ms(tqword1);

        if(qw_cmp(tqword1, snd_audfmt_interval) > 0) {
          qw_sub(tqword2, tqword1, snd_audfmt_interval);
          snd_audfmt_elapsed_ms  += qw_lo(tqword2);
          snd_audfmt_adjusted_ms  = snd_audfmt_elapsed_ms;
        }
        qw_set(snd_audfmt_interval, 0, 0);
      }
    }

    switch (parse_type) {
#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        if (pause) {
          voc_pb_pause();
        } else {
          voc_pb_resume(FALSE);
        }
        break;
#endif /* FEATURE_QCP */

#ifdef FEATURE_ANIMATED_AAC
    case AUDMAIN_MIDI_TYPE:
      if( audmain_get_adec_mode() != AUDMAIN_ADEC_MODE_AAC){
        break;
      }
#endif

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_MULAW_TYPE:
      case AUDMAIN_WAV_ALAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        sndhw_audfmt_adec_pause(pause);
        break;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */
    }

    if (pause) {
      cmx_pause = SND_CMX_AF_PAUSE;
    } else {
      cmx_pause = SND_CMX_AF_RESUME;
    }

    if((snd_af_ff_ctl == NULL) || (cmx_pause != SND_CMX_AF_RESUME)) {
      audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle),
                      cmx_pause, snd_audfmt_adjusted_ms, NULL);
    }

    ret_val = (SND_SND_LO_M << priority);

  } else if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_seek_pause_req

DESCRIPTION
  This function prepares to receive a rewind/fast-forward command by
  halting playback and muting the output.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
word snd_sound_seek_pause_req (
  snd_priority_type     priority,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  word                  ret_val = 0;
  snd_sound_status_type *snd_ptr;

  if(priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if(priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  if((snd_ptr->active) && 
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {

    /* This command is only supported for audio format sounds */
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_SUCCESS );
    }

    /* Set pause flag for audio format sound */
    audmain_set_ff_start(snd_ptr->audfmt_ctl);

    switch ( audmain_get_parse_type(snd_ptr->audfmt_ctl) ) {
#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        voc_pb_pause();
        break;
#endif /* FEATURE_QCP */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_MULAW_TYPE:
      case AUDMAIN_WAV_ALAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        sndhw_audfmt_adec_pause(TRUE);
        break;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */
    }

    ret_val = (SND_SND_LO_M << priority);

  } else if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
  return (ret_val);
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_tempo_req

DESCRIPTION
  This function modifies the playback tempo, if possible.

DEPENDENCIES
  snd_array
  snd_audfmt_elapsed_ms

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  None

===========================================================================*/
void snd_sound_tempo_req (
  snd_priority_type     priority,
  uint32                tempo_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr;

  if(priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if(priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  if((snd_ptr->active) && 
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {

    /* Tempo command is currently only supported for audio format sounds */
    audfmt_sound_ptr = (snd_audfmt_sound_type *) (snd_ptr->sound_ptr);

    /* Set tempo */
    if(audmain_set_tempo(snd_ptr->audfmt_ctl, &tempo_factor) ==
       AUDMAIN_STATUS_SUCCESS) {
      if (callback_ptr != NULL) {
        callback_ptr ( client_data, SND_SUCCESS );
      }

      audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle),
                      SND_CMX_AF_TEMPO, tempo_factor, NULL);
      return;
    }
  }

  if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_tune_req

DESCRIPTION
  This function modifies the playback tune, if possible.

DEPENDENCIES
  snd_array
  snd_audfmt_elapsed_ms

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  None

===========================================================================*/
void snd_sound_tune_req (
  snd_priority_type     priority,
  uint8                 tune_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr;

  if(priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if(priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  if((snd_ptr->active) && 
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {

    /* Tune command is currently only supported for audio format sounds */
    audfmt_sound_ptr = (snd_audfmt_sound_type *) (snd_ptr->sound_ptr);

    /* Set tune */
    if(audmain_set_tune(snd_ptr->audfmt_ctl, &tune_factor) ==
       AUDMAIN_STATUS_SUCCESS) {
      if (callback_ptr != NULL) {
        callback_ptr ( client_data, SND_SUCCESS );
      }

      audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle),
                      SND_CMX_AF_TUNE, tune_factor, NULL);
      return;
    }
  }

  if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_pan_req

DESCRIPTION
  This function modifies the playback stereo pan, if possible.

DEPENDENCIES
  snd_array
  snd_audfmt_elapsed_ms

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  None

===========================================================================*/
void snd_sound_pan_req (
  snd_priority_type     priority,
  uint8                 pan_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr;
  audmain_file_type     parse_type;
  boolean               status = FALSE;

  if(priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if(priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  if((snd_ptr->active) && 
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {
    /* Pan command is currently only supported for audio format sounds */
    /* Set pan */
    audfmt_sound_ptr = (snd_audfmt_sound_type *) (snd_ptr->sound_ptr);

    parse_type = audmain_get_parse_type(snd_ptr->audfmt_ctl);
    switch (parse_type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
#if defined( FEATURE_WEBAUDIO ) || defined (FEATURE_MIDI_OUT_QCP)
       if(audmain_get_adec_mode() != AUDMAIN_ADEC_MODE_AAC) {
#endif 
        status = (audmain_set_pan(snd_ptr->audfmt_ctl, &pan_factor) ==
                  AUDMAIN_STATUS_SUCCESS);
        break;
#if defined( FEATURE_WEBAUDIO ) || defined (FEATURE_MIDI_OUT_QCP)
       }  //else fall thru
#endif       
#endif /* FEATURE_MIDI */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
        status = sndhw_audfmt_stereo_pan(pan_factor);
        break;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */
#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        /* Indicate no change, since it is unsupported */
        pan_factor = 100;
        status = TRUE;
        break;
#endif
    }

    if(status) {
      if (callback_ptr != NULL) {
        callback_ptr ( client_data, SND_SUCCESS );
      }

      audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle),
                      SND_CMX_AF_PAN, pan_factor, NULL);
      return;
    }

  }

  if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_do_time

DESCRIPTION
  This function does the actual work in calculating the playback time of
  a midi/WebAudio file.

DEPENDENCIES
  snd_audfmt_elapsed_ms
  snd_af_get_time_ctl
  snd_get_time_cb_ptr
  snd_get_time_client_data

RETURN VALUE
  None

SIDE EFFECTS
  snd_audfmt_elapsed_ms
  snd_af_get_time_ctl
  snd_get_time_cb_ptr

===========================================================================*/
word snd_audfmt_do_time ( void )
{
  audmain_status_type   aud_status;

  word ret_val = 0;

  if(snd_af_get_time_ctl != NULL) {
    aud_status = audmain_do_time(snd_af_get_time_ctl, &snd_audfmt_elapsed_ms);

    if(aud_status == AUDMAIN_STATUS_PENDING) {
    
      switch (audmain_get_parse_type(snd_af_get_time_ctl)) { 
#ifdef FEATURE_MIDI
        case AUDMAIN_MIDI_TYPE:
          snd_af_get_time_ctl = NULL;
          snd_get_time_cb_ptr = NULL;
          break;
#endif /* FEATURE_MIDI */

#ifdef FEATURE_MP3
        case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
        case AUDMAIN_AAC_TYPE:
#endif
        default:
          break;
      }
    } else
    if(aud_status == AUDMAIN_STATUS_DONE) {
      snd_af_get_time_ctl = NULL;
      if(snd_get_time_cb_ptr != NULL) {
        snd_get_time_cb_ptr( snd_get_time_client_data, SND_PLAY_DONE,
                             snd_audfmt_elapsed_ms );
        snd_get_time_cb_ptr = NULL;
      }
      audmain_reset();
    } else if(aud_status == AUDMAIN_STATUS_ERROR) {
      snd_af_get_time_ctl = NULL;
      if(snd_get_time_cb_ptr != NULL) {
        snd_get_time_cb_ptr( snd_get_time_client_data, SND_FAILURE, 0 );
        snd_get_time_cb_ptr = NULL;
      }
      audmain_reset();
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_get_time

DESCRIPTION
  This function initiates the mechanism to calculate the playback time
  of a midi/WebAudio file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  snd_af_get_time_ctl

===========================================================================*/
void snd_audfmt_get_time (
  snd_af_access_type         *audfmt_handle,
  uint32                     byte_offset,
  snd_time_cb_func_ptr_type  callback_ptr,
  const void                 *client_data
) {
  /* Initialize audio format sound */
  audmain_reset_parms();
  if(audmain_get_time(audfmt_handle, byte_offset, &snd_af_get_time_ctl) !=
     AUDMAIN_STATUS_SUCCESS) {
    snd_af_get_time_ctl = NULL;
    if(callback_ptr != NULL) {
      callback_ptr( client_data, SND_FAILURE, 0 );
    }
  } else {
    if(snd_get_time_cb_ptr != NULL) {
      snd_get_time_cb_ptr( snd_get_time_client_data, SND_ABORT, 0 );
    }
    if(callback_ptr != NULL) {
      callback_ptr( client_data, SND_SUCCESS, 0 );
    }

    /* Initialize audio format get time parameters */
    snd_audfmt_elapsed_ms    = 0;
    snd_get_time_cb_ptr      = callback_ptr;
    snd_get_time_client_data = client_data;

    /* Start get time sequence */
    (void) snd_audfmt_do_time();
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_gt_abort_req

DESCRIPTION
  This function aborts the midi/WebAudio get time function.

DEPENDENCIES
  snd_get_time_cb_ptr
  snd_get_time_client_data

RETURN VALUE
  None

SIDE EFFECTS
  snd_af_get_time_ctl
  snd_audfmt_elapsed_ms

===========================================================================*/
void snd_audfmt_gt_abort_req (
  snd_cb_func_ptr_type       callback_ptr,
  const void                 *client_data
) {
  audmain_reset_calc_time();

  snd_af_get_time_ctl   = NULL;
  snd_audfmt_elapsed_ms = 0;

  if(snd_get_time_cb_ptr != NULL) {
    if(callback_ptr != NULL) {
      callback_ptr(client_data, SND_SUCCESS);
    }

    snd_get_time_cb_ptr( snd_get_time_client_data, SND_ABORT, 0 );
    snd_get_time_cb_ptr = NULL;
  } else if(callback_ptr != NULL) {
    callback_ptr(client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_stop_req

DESCRIPTION
  This function stops the indicated sequence playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_sequence_stop_req (
  void                  *sequence,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {

  if((audmain_reset_sequence(sequence) == AUDMAIN_STATUS_SUCCESS) &&
     (callback_ptr != NULL)) {
    callback_ptr ( client_data, SND_SUCCESS );
  } else if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_pause_req

DESCRIPTION
  This function pauses/resumes indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_sequence_pause_req (
  void                  *sequence,
  boolean               pause_flag,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  if((audmain_pause_sequence(sequence, pause_flag) == AUDMAIN_STATUS_SUCCESS) &&
     (callback_ptr != NULL)) {
    callback_ptr ( client_data, SND_SUCCESS );
  } else if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_fforward_req

DESCRIPTION
  This function fast-forwards indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_sequence_fforward_req (
  void                  *sequence,
  uint32                ms_forward,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {

  if((audmain_fforward_sequence(sequence, ms_forward) ==
      AUDMAIN_STATUS_SUCCESS) && (callback_ptr != NULL)) {
    callback_ptr ( client_data, SND_SUCCESS );
  } else if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_rewind_req

DESCRIPTION
  This function rewinds indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_sequence_rewind_req (
  void                  *sequence,
  uint32                ms_rewind,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {

  if((audmain_rewind_sequence(sequence, ms_rewind) == AUDMAIN_STATUS_SUCCESS) &&
     (callback_ptr != NULL)) {
    callback_ptr ( client_data, SND_SUCCESS );
  } else if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_set_tempo_req

DESCRIPTION
  This function sets the tempo of the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_sequence_set_tempo_req (
  void                  *sequence,
  uint32                tempo_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  if((audmain_set_tempo_sequence(sequence, tempo_factor) ==
      AUDMAIN_STATUS_SUCCESS) && (callback_ptr != NULL)) {
    callback_ptr ( client_data, SND_SUCCESS );
  } else if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_set_tune_req

DESCRIPTION
  This function sets the tune of the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_sequence_set_tune_req (
  void                  *sequence,
  uint32                tune_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  if((audmain_set_tune_sequence(sequence, tune_factor) ==
      AUDMAIN_STATUS_SUCCESS) && (callback_ptr != NULL)) {
    callback_ptr ( client_data, SND_SUCCESS );
  } else if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_set_pan_req

DESCRIPTION
  This function sets the pan of the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_sequence_set_pan_req (
  void                  *sequence,
  uint16                pan_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  if((audmain_set_pan_sequence(sequence, pan_factor) ==
      AUDMAIN_STATUS_SUCCESS) && (callback_ptr != NULL)) {
    callback_ptr ( client_data, SND_SUCCESS );
  } else if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_set_vol_req

DESCRIPTION
  This function sets the vol of the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_sequence_set_vol_req (
  void                  *sequence,
  uint16                vol_level,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {

  if(audmain_set_vol_sequence(sequence, vol_level) == AUDMAIN_STATUS_SUCCESS) {
    if(callback_ptr != NULL) {
      callback_ptr(client_data, SND_SUCCESS);
    }
  } else if(callback_ptr != NULL) {
    callback_ptr(client_data, SND_FAILURE);
  }
}

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION snd_qcp_rec_voc_cb_req

DESCRIPTION
  This function handles the QCP recording vocoder callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_qcp_rec_voc_cb_req (
  snd_packets_type *cmd_ptr
) {
  if((cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_DONE) ||
     (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_ERROR) ||
     (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_AUTO_STOP) ||
     (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_BUFFER_ERROR) ||
     (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_INVALID_PARAM) ||
     (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_ANALOG_ERROR) ||
     (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_NEW_CONFIG_ERROR)) {
    if(cmd_ptr->qcp_rec_cb.tx_buffer != NULL) {
      audmain_rec_voc_cb(AUDMAIN_REC_LAST_TX_BUFFER, 
                         cmd_ptr->qcp_rec_cb.tx_buffer,
                         cmd_ptr->qcp_rec_cb.tx_num_bytes,
                         cmd_ptr->qcp_rec_cb.tx_num_frames);
    }
    if(cmd_ptr->qcp_rec_cb.rx_buffer != NULL) {
      audmain_rec_voc_cb(AUDMAIN_REC_LAST_RX_BUFFER, 
                         cmd_ptr->qcp_rec_cb.rx_buffer,
                         cmd_ptr->qcp_rec_cb.rx_num_bytes,
                         cmd_ptr->qcp_rec_cb.rx_num_frames);
    }
    if((cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_ERROR) ||
       (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_INVALID_PARAM) ||
       (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_ANALOG_ERROR) ||
       (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_NEW_CONFIG_ERROR)) {
      (void) audmain_record_stop(AUDMAIN_REC_ERROR);
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_BUFFER_ERROR) {
      (void) audmain_record_stop(AUDMAIN_REC_BUFF_ERROR);
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_AUTO_STOP) {
      (void) audmain_record_stop(AUDMAIN_REC_AUTO_STOP);
    } else {
      (void) audmain_record_stop(AUDMAIN_REC_DONE);
    }
  } else {
    if(cmd_ptr->qcp_rec_cb.tx_buffer != NULL) {
      audmain_rec_voc_cb(AUDMAIN_REC_TX_BUFFER, cmd_ptr->qcp_rec_cb.tx_buffer,
                         cmd_ptr->qcp_rec_cb.tx_num_bytes,
                         cmd_ptr->qcp_rec_cb.tx_num_frames);
    }
    if(cmd_ptr->qcp_rec_cb.rx_buffer != NULL) {
      audmain_rec_voc_cb(AUDMAIN_REC_RX_BUFFER, cmd_ptr->qcp_rec_cb.rx_buffer,
                         cmd_ptr->qcp_rec_cb.rx_num_bytes,
                         cmd_ptr->qcp_rec_cb.rx_num_frames);
    }

    if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_INFORM_EVRC) {
      audmain_rec_voc_cb(AUDMAIN_REC_INFORM_EVRC, NULL, 0, 0);
#ifdef MSMAUD_VOC_4GV
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_INFORM_4GV_NB) {
      audmain_rec_voc_cb(AUDMAIN_REC_INFORM_4GV_NB, NULL, 0, 0);
#endif
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_INFORM_13K) {
      audmain_rec_voc_cb(AUDMAIN_REC_INFORM_13K, NULL, 0, 0);
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_INFORM_AMR) {
      audmain_rec_voc_cb(AUDMAIN_REC_INFORM_AMR, NULL, 0, 0);
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_INFORM_EFR) {
      audmain_rec_voc_cb(AUDMAIN_REC_INFORM_EFR, NULL, 0, 0);
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_INFORM_FR) {
      audmain_rec_voc_cb(AUDMAIN_REC_INFORM_FR, NULL, 0, 0);
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_INFORM_HR) {
      audmain_rec_voc_cb(AUDMAIN_REC_INFORM_HR, NULL, 0, 0);
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_PAUSED) {
      audmain_rec_voc_cb(AUDMAIN_REC_PAUSED, NULL, 0, 0);
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_RESUMED) {
      audmain_rec_voc_cb(AUDMAIN_REC_RESUMED, NULL, 0, 0);
    } else if (cmd_ptr->qcp_rec_cb.status == VOC_REC_STAT_INT_TIME) {
      audmain_rec_voc_cb(AUDMAIN_REC_INT_TIME, NULL, 0, 0);
    }
  } 
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_rec_voc_cb

DESCRIPTION
  This function is used as the callback for QCP recording for the 
  vocoder driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_qcp_rec_voc_cb (
  voc_rec_result_type *result
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  if((result->status == VOC_REC_STAT_DONE) ||
     (result->status == VOC_REC_STAT_ERROR) ||
     (result->status == VOC_REC_STAT_AUTO_STOP) ||
     (result->status == VOC_REC_STAT_BUFFER_ERROR) ||
     (result->status == VOC_REC_STAT_INVALID_PARAM) ||
     (result->status == VOC_REC_STAT_ANALOG_ERROR) ||
     (result->status == VOC_REC_STAT_NEW_CONFIG_ERROR)) {
    snd_qcp_record_active = FALSE;
  }
  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->qcp_rec_cb.hdr.cmd    = SND_QCP_REC_VOC_CB;
    packet->qcp_rec_cb.status     = result->status;
    if(result->rev_ptr != NULL) {
      packet->qcp_rec_cb.tx_buffer     = result->rev_ptr->rec_data_ptr;
      packet->qcp_rec_cb.tx_num_bytes  = result->rev_ptr->rec_length;
      packet->qcp_rec_cb.tx_num_frames = result->rev_ptr->rec_num_frames;
    } else {
      packet->qcp_rec_cb.tx_buffer     = NULL;
      packet->qcp_rec_cb.tx_num_bytes  = 0;
      packet->qcp_rec_cb.tx_num_frames = 0;
    }
    if(result->fwd_ptr != NULL) {
      packet->qcp_rec_cb.rx_buffer     = result->fwd_ptr->rec_data_ptr;
      packet->qcp_rec_cb.rx_num_bytes  = result->fwd_ptr->rec_length;
      packet->qcp_rec_cb.rx_num_frames = result->fwd_ptr->rec_num_frames;
    } else {
      packet->qcp_rec_cb.rx_buffer     = NULL;
      packet->qcp_rec_cb.rx_num_bytes  = 0;
      packet->qcp_rec_cb.rx_num_frames = 0;
    }
    snd_cmd(packet);
  } else {
    MSG_ERROR("Lost recording status",0,0,0);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_rec_new_buffer_req

DESCRIPTION
  This function receives new buffer for QCP recording and delivers
  it to the vocoder driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void snd_qcp_rec_new_buffer_req (
  uint8  *buf,
  uint32 num_bytes
) {
  if(snd_qcp_record_active == TRUE) {
    voc_rec_put_buf(buf, num_bytes);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_get_func_req

DESCRIPTION
  Gets the indicated information from a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_qcp_get_func_req (
  snd_qcp_func_enum_type func,
  snd_af_access_type     *handle
) {
  switch(func) {
    case SND_QCP_FUNC_GET_DATA_SIZE:
      audmain_qcp_get_data_size(handle);
      break;

    case SND_QCP_FUNC_GET_FRAMES:
      audmain_qcp_get_frames(handle);
      break;

    case SND_QCP_FUNC_CNFG:
      audmain_qcp_get_cnfg(handle);
      break;

    case SND_QCP_FUNC_LABL:
      audmain_qcp_get_labl(handle);
      break;

    case SND_QCP_FUNC_TEXT:
      audmain_qcp_get_text(handle);
      break;

    case SND_QCP_FUNC_INVALID:
    default:
      if((handle != NULL) && (handle->data_func != NULL)) {
        handle->data_func(SND_CMX_QCP_FAILURE, handle->audfmt_ptr, 0, NULL);
      }
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_set_func_req

DESCRIPTION
  Sets the indicated information in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_qcp_set_func_req (
  snd_qcp_func_enum_type       func,
  uint8                        *buffer,
  uint32                       num_bytes,
  snd_af_access_type           *handle,
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  switch(func) {
    case SND_QCP_FUNC_CNFG:
      audmain_qcp_set_cnfg ( num_bytes, handle, cb_func, client_data );
      break;

    case SND_QCP_FUNC_LABL:
      audmain_qcp_set_labl ( buffer, num_bytes, handle, cb_func, client_data );
      break;

    case SND_QCP_FUNC_TEXT:
      audmain_qcp_set_text ( buffer, num_bytes, handle, cb_func, client_data );
      break;

    case SND_QCP_FUNC_INVALID:
    default:
      if(cb_func != NULL) {
        cb_func(SND_CMX_QCP_FAILURE, NULL, client_data);
      }
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_record_req

DESCRIPTION
  Starts QCP recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_qcp_record_req (
  snd_qcp_format_type          format,
  snd_qcp_dir_type             link,
  snd_mm_qcp_rec_para_type     rec_param,
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {

  /* sync time  should be in times of 20ms */

  if ((rec_param.data_req_ms != 0)     &&
#ifdef FEATURE_MM_REC
      (rec_param.sync_cb_func != NULL) &&
#endif /* FEATURE_MM_REC */
      (rec_param.data_req_ms < 20 ||
      (rec_param.data_req_ms - (rec_param.data_req_ms/20)*20))) {

    if (cb_func != NULL) {
      snd_qcp_record_active = FALSE;
      cb_func(SND_CMX_QCP_REC_ERROR, NULL, client_data);
    }
    return;
  }
  
  /*resets all the audmain module parameters*/
  audmain_reset_parms();

  snd_qcp_record_active = TRUE;
  if (audmain_record(format, 
                     link, 
                     cb_func, 
                     client_data) == AUDMAIN_STATUS_SUCCESS) {
   
    sndhw_qcp_record(format, 
                     link, 
                     &rec_param,
                     (voc_rec_cb_type) snd_qcp_rec_voc_cb,
                     client_data);

  } else if(cb_func != NULL) {
    snd_qcp_record_active = FALSE;
    cb_func(SND_CMX_QCP_REC_ERROR, NULL, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_record_stop_req

DESCRIPTION
  Stops a QCP recording session, if one is active.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_qcp_record_stop_req (
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  if(snd_qcp_record_active) {
    sndhw_qcp_record_stop();
    if(cb_func != NULL) {
      cb_func(client_data, SND_SUCCESS);
    }
  } else if (cb_func != NULL) {
    cb_func ( client_data, SND_FAILURE );   
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_record_pause

DESCRIPTION
  Pauses a QCP recording session, if one is active.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_qcp_record_pause_req (
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  if(snd_qcp_record_active) {
    sndhw_qcp_record_pause();
    if(cb_func != NULL) {
      cb_func(client_data, SND_SUCCESS);
    }
  } else if (cb_func != NULL) {
    cb_func ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_record_resume

DESCRIPTION
  Resumes a QCP recording session, if one is active.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_qcp_record_resume_req (
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  if(snd_qcp_record_active) {
    sndhw_qcp_record_resume();
    if(cb_func != NULL) {
      cb_func(client_data, SND_SUCCESS);
    }
  } else if (cb_func != NULL) {
    cb_func ( client_data, SND_FAILURE );
  }
}
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION snd_pcm_record_req

DESCRIPTION
  Starts PCM recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_pcm_record_req (
  snd_pcm_rec_dir_type         link,
  uint8                        *buf1,
  uint32                       num_buf1,
  uint8                        *buf2,
  uint32                       num_buf2,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  voc_pcm_client_output_fn_type  voc_cb_func;

  voc_cb_func = audmain_pcm_record(link, buf1, num_buf1, buf2, num_buf2,
                                   cb_func, client_data);

  if(voc_cb_func != NULL) {
    sndhw_pcm_record(link, voc_cb_func);
  } else if(cb_func != NULL) {
    cb_func(SND_CMX_AF_FAILURE, NULL, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pcm_record_stop_req

DESCRIPTION
  Stops PCM recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_pcm_record_stop_req (
  snd_pcm_rec_dir_type link,
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  if(audmain_pcm_record_stop(link) == AUDMAIN_STATUS_SUCCESS) {
    sndhw_pcm_record_stop(link);
    if(cb_func != NULL) {
      cb_func(client_data, SND_SUCCESS);
    }
  } else if (cb_func != NULL) {
    cb_func ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pcm_record_pause_req

DESCRIPTION
  Pauses/resumes PCM recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_pcm_record_pause_req (
  boolean              pause,
  snd_pcm_rec_dir_type link,
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  if(audmain_pcm_record_pause(pause, link) == AUDMAIN_STATUS_SUCCESS) {
    if(cb_func != NULL) {
      cb_func(client_data, SND_SUCCESS);
    }
  } else if (cb_func != NULL) {
    cb_func ( client_data, SND_FAILURE );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pcm_record_status_req

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_pcm_record_status_req (
  snd_af_data_cmd_enum         status,
  snd_pcm_rec_data_struct_type *data,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  switch(status) {
    case SND_CMX_AF_TIME_TICK:
    case SND_CMX_PCM_REC_DROP_FRAMES:
    case SND_CMX_PCM_REC_DROP_RESUME:
      if(cb_func != NULL) {
        cb_func(status, NULL, client_data);
      }
      break;

    case SND_CMX_PCM_REC_DATA:
      if(cb_func != NULL) {
        data->buf_cb_func = snd_pcm_record_buf_cb;
        cb_func(status, data, client_data);
      }
      break;

    case SND_CMX_PCM_REC_INTERRUPTED:
    case SND_CMX_PCM_REC_INT_RESUME:
     audmain_pcm_rec_send_status(status);
     break;
  }
}
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION snd_mm_record_buf_cb

DESCRIPTION
  This function is used as the callback function for MultiMedia recording
  buffer requests.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_mm_record_buf_cb (
  uint8                  *buffer,
  uint32                 num_bytes,
  const void             *client_data
) 
{
  audmain_mm_record_buf(buffer,
                        num_bytes,
                        client_data, 
                        snd_mm_rec_ctrl.format);
} /* snd_mm_record_buf_cb */

/*===========================================================================

FUNCTION snd_mm_isr_cb_func

DESCRIPTION
  MultiMedia recording interrupt service at sound layer.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.
===========================================================================*/
void snd_mm_isr_cb_func (void)
{
  snd_packets_type *packet;

  packet = snd_get_packet();

  if (packet != NULL) {

    packet->mm_rec_isr.hdr.cmd = SND_MM_RECORD_ISR;
    snd_cmd(packet);
  }
}

/*===========================================================================

FUNCTION snd_mm_record_req

DESCRIPTION
  MultiMedia command handler which starts a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_mm_record_req (
  snd_mm_rec_dir_type         dir,          /* MultiMedia recording direction */
  snd_mm_rec_aud_type         *rec_param,   /* Record parameters              */
  snd_mm_rec_cmd_struct_type  *buf,         /* MM record buffer info          */
  snd_mm_rec_cb_func_ptr_type cb_func,      /* Callback function to CMX       */
  const void                  *client_data  /* Client data pointer            */
)
{
  snd_mm_rec_ctrl.format      = rec_param->format;
  snd_mm_rec_ctrl.cb_func     = cb_func;
  snd_mm_rec_ctrl.client_data = client_data;

  if ((audmain_mm_record_start(rec_param, 
                               buf,
                               cb_func,
                               client_data)) == TRUE) {

    sndhw_mm_record(rec_param,
                    snd_mm_isr_cb_func);
  } else {

    if (cb_func != NULL) {

      cb_func(SND_CMX_MM_REC_FAILURE,
              NULL, 
              client_data);
    }
  }
}/* end of snd_mm_record_req */

/*===========================================================================

FUNCTION snd_mm_record_stop_req

DESCRIPTION
  Stops MultiMedia recording.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_mm_record_stop_req (
  snd_mm_rec_dir_type    link,               /* Record direction           */
  snd_cb_func_ptr_type   cb_func,
  const void             *client_data
) 
{
  snd_status_type cb_type;

  cb_type = SND_FAILURE;

  if(snd_qcp_record_active) {

    snd_qcp_record_stop_req(cb_func, client_data);
  } else if (audmain_mm_record_stop(snd_mm_rec_ctrl.format) == 
      AUDMAIN_STATUS_SUCCESS) {
  
    sndhw_mm_record_stop();

    cb_type = SND_SUCCESS;

  }

  if (cb_func != NULL) {
    cb_func (client_data, cb_type);
  }

  return;
}

/*===========================================================================

FUNCTION snd_mm_record_pause_req

DESCRIPTION
  Pauses/resumes a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_mm_record_pause_req (
  boolean                pause,
  snd_mm_rec_dir_type    link,               /* Record direction           */
  snd_cb_func_ptr_type   cb_func,
  const void             *client_data
) 
{
  snd_status_type cb_type;

  cb_type = SND_FAILURE;

  if(snd_qcp_record_active) {

    (pause == TRUE) ? snd_qcp_record_pause_req(cb_func, client_data) : 
                      snd_qcp_record_resume_req(cb_func, client_data);  
  } else if (audmain_mm_record_pause(pause, snd_mm_rec_ctrl.format) == 
      AUDMAIN_STATUS_SUCCESS) {
  
    cb_type = SND_SUCCESS;
  }

  if (cb_func != NULL) {
    cb_func (client_data, cb_type);
  }
}

/*===========================================================================

FUNCTION snd_mm_record_status_req

DESCRIPTION
  This function sends the indicated MultiMedia recording status back to the 
  client.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_mm_record_status_req (
  snd_af_data_cmd_enum         status,
  snd_mm_rec_data_struct_type  *data,
  snd_mm_rec_cb_func_ptr_type  cb_func,
  const void                   *client_data,
  snd_mm_rec_format_type       format
)
{
  switch (status) {

    case SND_CMX_MM_REC_HEADER_DATA:
    case SND_CMX_MM_REC_DATA: {

        if ((cb_func != NULL) &&
            (data    != NULL)) {
          data->buf_cb_func = (snd_mm_buf_cb_func_type) snd_mm_record_buf_cb;
          cb_func(status, data, client_data);
        }
      }
      break;

    case SND_CMX_MM_REC_INTERRUPTED:
    case SND_CMX_MM_REC_INT_RESUME: 
    case SND_CMX_MM_REC_ENABLED:  {
        audmain_mm_rec_send_status(format, status);
      }
      break;

    default:
      break;
  }
} /* end of snd_mm_record_status_req */

/*===========================================================================

FUNCTION snd_mm_record_data_int

DESCRIPTION
  This function handles the MultiMedia recording interrupt command signal, it
  will get the recorded data and invokes corresponding routine to process.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_mm_record_data_int (void)
{
#ifdef FEATURE_AAC_REC
  audmain_mm_record_buf_type  buf;
  audmain_mm_record_data_type frame_info;
#endif /* FEATURE_AAC_REC */

  switch (snd_mm_rec_ctrl.format) {

#ifdef FEATURE_AAC_REC
  case  SND_MM_FORMAT_AAC: {
    
      if (audmain_mm_record_get_buf(SND_MM_FORMAT_AAC,
                                    &buf) == FALSE) {
        return;
      }
      
      if ((sndhw_mm_record_data(SND_MM_FORMAT_AAC, 
                                &buf,
                                &frame_info)) != TRUE) {
        return;
      }
      
      audmain_mm_record_frame(SND_MM_FORMAT_AAC,
                              &frame_info);
    }
    break;
#endif /* FEATURE_AAC_REC */

  default:
    break;
  }
}
#endif /* FEATURE_MM_REC */
/* <EJECT> */

/*===========================================================================

FUNCTION snd_aux_pcm_clk_enable

DESCRIPTION
  This function will enable (external)auxpcm clock.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
void snd_aux_pcm_clk_enable_req
(
  snd_cb_func_ptr_type     callback_ptr
    /* Call back function                     */
)
{
    /*enable the aux_pcm_clock*/
	(void)sndhw_enable_eclock();

  if (callback_ptr !=  NULL) {
    callback_ptr( NULL, SND_SUCCESS);
  }
}
/*===========================================================================

FUNCTION snd_aux_pcm_clk_disable_req

DESCRIPTION
  This function will disable (external)auxpcm clock.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
void snd_aux_pcm_clk_disable_req
(
  snd_cb_func_ptr_type     callback_ptr
    /* Call back function                     */
)
{
  /*disable the aux_pcm_clock*/
  (void)sndhw_disable_eclock();

  if (callback_ptr != NULL) {
    callback_ptr(NULL, SND_SUCCESS);
  }
}


/* <EJECT> */
#ifdef FEATURE_AUDFMT_EQUALIZER
/*===========================================================================

FUNCTION snd_get_equalizer_buffer

DESCRIPTION
  This function will get the internal equalizer buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
snd_equalizer_filter_type* snd_get_equalizer_buffer( void )
{
  snd_equalizer_filter_type *eq_ptr = NULL;

  INTLOCK();
  if (snd_equalizer_buffer_inuse == FALSE) {
    snd_equalizer_buffer_inuse = TRUE;
    eq_ptr                     = snd_equalizer_buffer;
  }
  INTFREE();
  return (eq_ptr);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_equalizer_ctl_req

DESCRIPTION
  This function does the actual work in enable/disable equalizer.
  specified band.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void snd_equalizer_ctl_req
( 
  boolean                         enable,
  snd_cb_func_ptr_type            callback_ptr,
  const void                      *client_data
) {
  snd_status_type status = SND_FAILURE;

  if (voc_enable_equalizer(enable) == VOC_DONE_S) {
    status = SND_SUCCESS;
  }

  if (callback_ptr !=  NULL) {
    callback_ptr( client_data, status);
  }
}  

/* <EJECT> */
/*===========================================================================

FUNCTION snd_equalizer_set_req

DESCRIPTION
  This function does the actual work in setting the equalizer filters

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void snd_equalizer_set_req
(
  const snd_equalizer_filter_type *filter,
  uint16                          num,
  snd_cb_func_ptr_type            callback_ptr,
  const void                      *client_data
)
{
  snd_status_type status = SND_SUCCESS;
  uint16          i;

  if (num <= VOC_EQ_BANDS_NUM) {
    for (i=0;i<num;i++) {
      if ((!SND_EQUALIZER_CHECK_GAIN(filter[i].gain)) ||
          (!SND_EQUALIZER_CHECK_QFACTOR(filter[i].qf)) ||
          (!SND_EQUALIZER_CHECK_CUTOFFFQ(filter[i].freq, filter[i].type))) {
        status = SND_FAILURE;
        break;
      }    
    }

    if (status == SND_SUCCESS) {
      if (voc_set_equalizer((voc_equalizer_filter_type *)filter, 
                             num) != VOC_DONE_S) {
        status = SND_FAILURE;
      }
    }
  } else {
    status = SND_FAILURE;
  }

  snd_equalizer_buffer_inuse = FALSE;

  if (callback_ptr !=  NULL) {
    callback_ptr( client_data, status);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_equalizer_gain_req

DESCRIPTION
  This function does the actual work in setting the new equalizer gain on the 
  specified band.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void snd_equalizer_gain_req( 
  uint16                band,
  int16                 gain,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_status_type status = SND_FAILURE;

  if ((gain >= SND_EQUALIZER_MIN_GAIN) &&
      (gain <= SND_EQUALIZER_MAX_GAIN)) {
    if (voc_set_equalizer_gain(band, gain) == VOC_DONE_S) {
      status = SND_SUCCESS;
    }
  }

  if (callback_ptr !=  NULL) {
    callback_ptr( client_data, status);
  }
}
#endif /* FEATURE_AUDFMT_EQUALIZER */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/*===========================================================================

FUNCTION snd_spectrum_analyzer_callback

DESCRIPTION
  Callback which receives spectrum analyzer data from the DSP in interrupt
  context and enqueues it to the sound task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_spectrum_analyzer_callback 
( 
  const uint16 *buffer 
)
{
  snd_packets_type *packet;

  /* Don't enqueue if there's an update already pending */
  if (snd_spa_data_available == FALSE) {
    snd_spa_data_available = TRUE;

    memcpy(snd_spa_buffer, buffer, sizeof(snd_spa_buffer));

    /* Get a Sound Packet and queue the Request.
    */
    packet = snd_get_packet();    /* Already Logs Error if Fails */
    if (packet != NULL) {
      packet->hdr.cmd = SND_SPECTRUM_ANALYZER_DATA;
      snd_cmd(packet);
    }
  } else {
    MSG_MED("Dropped spectrum analyzer data",0,0,0);
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_spectrum_analyzer_pass_data_to_client

DESCRIPTION
  Passes the spa data to client in sound task context, manages the ping-pong
  buffers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_spectrum_analyzer_data_int ( void )
{
  if (snd_spectrum_analyzer_client_cb != NULL) {
    /* Pass the new data to the client */
    snd_spectrum_analyzer_client_cb(snd_spa_buffer,
                                    snd_spectrum_analyzer_client_data);
  }
  snd_spa_data_available = FALSE;
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_spectrum_analyzer_ctl_req

DESCRIPTION
  Registers the spectrum analyzer callback with the vocoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_spectrum_analyzer_ctl_req (
  uint16                        update_period,
  snd_spectrum_analyzer_cb_type sp_cb,
  snd_cb_func_ptr_type          callback_ptr,
  const void                    *client_data
) {

  /* Verify if the parameters are valid */
  if ((sp_cb != NULL) &&
      ((update_period < SND_SPECTRUM_ANALYZER_MIN_PERIOD) ||
       (update_period > SND_SPECTRUM_ANALYZER_MAX_PERIOD))) {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
    return;
  }

  snd_spectrum_analyzer_client_cb   = sp_cb;
  snd_spectrum_analyzer_client_data = (void *)client_data;

  if (sp_cb == NULL) {
    voc_spectrum_analyzer_ctl(0, NULL);
  } else {
    voc_spectrum_analyzer_ctl(update_period, 
                              snd_spectrum_analyzer_callback);
  }

  if (callback_ptr != NULL) {
    callback_ptr(client_data, SND_SUCCESS);
  }
}
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

/* <EJECT> */
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_QCONCERT
/*===========================================================================

FUNCTION snd_qconcert_ctl_req

DESCRIPTION
  This function enables/disables the Qconcert 3D audio expander feature.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_qconcert_ctl_req
(
  boolean enable,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
) {
  
  snd_status_type status = SND_SUCCESS;

  if (voc_enable_qconcert(enable) != VOC_DONE_S) {
    status = SND_FAILURE;
  }

  if (callback_ptr != NULL) {
    callback_ptr(client_data, status);
  }  

}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_qconcert_params_req

DESCRIPTION
  This function configures the audio 3D expander's parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_set_qconcert_params_req
(
  snd_qconcert_params_type *params,
  snd_cb_func_ptr_type     callback_ptr,
  const void               *client_data
)
{
  snd_status_type status = SND_SUCCESS;
  voc_qconcert_params_type voc_params;

  voc_params.mode     = params->mode;
  voc_params.gain     = params->gain;
  voc_params.spread   = params->spread;
  voc_params.geometry = params->geometry;

  if (voc_set_qconcert_params(&voc_params) != VOC_DONE_S) {
    status = SND_FAILURE;
  }

  if (callback_ptr != NULL) {
    callback_ptr(client_data, status);
  }
}
#endif /* FEATURE_AUDFMT_QCONCERT */
#endif /* FEATURE_AUDIO_FORMAT */

/*===========================================================================

FUNCTION snd_slowtalk_ctl_req

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_slowtalk_ctl_req
(
  boolean              enable,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
) {
  
  snd_status_type status = SND_SUCCESS;

  if (voc_enable_slowtalk(enable) != VOC_DONE_S) {
    status = SND_FAILURE;
  }

  if (callback_ptr != NULL) {
    callback_ptr(client_data, status);
  }  

}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_slowtalk_params_req

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_set_slowtalk_params_req
(
  snd_slowtalk_params_type *params,
  snd_cb_func_ptr_type     callback_ptr,
  const void               *client_data
)
{
  snd_status_type          status = SND_SUCCESS;
  voc_slowtalk_params_type voc_slowtalk_params;

  voc_slowtalk_params.expansion_ratio_level = params->expansion_ratio_level;
  
  if (voc_set_slowtalk_params(&voc_slowtalk_params) != VOC_DONE_S) {
    status = SND_FAILURE;
  }

  if (callback_ptr != NULL) {
    callback_ptr(client_data, status);
  }
}

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_bt_cb 

DESCRIPTION
  This function is used to inform the audio drivers of the BT callback fucntion
  to be called by audio drivers to inform BT about the type of sound (whether
  the sound is supported over BT SCO, BT A2DP etc.). 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern void snd_set_bt_cb_func(snd_bt_cb_func_ptr_type cb_func)
{
  snd_bt_cb_func_ptr = cb_func;
  sndhw_set_bt_cb_func((voc_bt_cb_func_ptr_type)cb_func);
}

/*===========================================================================

FUNCTION snd_clr_bt_cb_func 

DESCRIPTION
  This function is used to unregister the BT callback function which is registered
  to audio drivers for informing the type of sound.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern void snd_clr_bt_cb_func(snd_bt_cb_func_ptr_type cb_func)
{
  if (cb_func == snd_bt_cb_func_ptr)
  {
    snd_bt_cb_func_ptr = NULL;
    sndhw_set_bt_cb_func((voc_bt_cb_func_ptr_type)NULL);
  }
  else
  {
    MSG_MED("snd_clr_bt_cb_func is called with unregistered function",0,0,0);
  }
}

#endif /*FEATURE_AVS_BT_SCO_REWORK*/
/*===========================================================================

FUNCTION snd_set_qtv_cb_func 

DESCRIPTION
  This function is used to inform the audio drivers about the QTV
  callback to be called by audio drivers to inform QTV about the
  QDSP_MOD_DISABLE evt when the image swap is happening from audio
  only image to audio/video image.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern void snd_set_qtv_cb_func(snd_notify_qtv_cb_func_ptr_type cb_func)
{
  sndhw_set_qtv_cb_func((voc_notify_qtv_cb_func_ptr_type)cb_func);
}


#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_callback

DESCRIPTION
  Callback which receives PCM data for SBC codec from the DSP in interrupt
  context and enqueues it to the sound task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_sbc_callback 
( 
  uint16  buf_size,
  uint16  *buffer,
  uint16  pp_id
)
{
#if defined (FEATURE_AVS_SBCENC_TASK) 
  snd_pp_buffer_type pp_buffer;
  /* We are in ISR context.
     Lets not queue it to snd task, instead send it directly to SBC encoder
     task 
   */
    if (snd_sbc_client_cb != NULL) 
    {
      pp_buffer.pp_id             = pp_id;
      pp_buffer.pp_buf_length     = buf_size;
      pp_buffer.pp_data           = buffer;
      /* pp_num_channels is dont care */
      snd_sbc_client_cb (pp_buffer);
    }
    else
    {
      sndhw_free_wb_pcm_rx_interface_buffer(pp_id);
      MSG_ERROR("snd sbc data client callback is null \n", 0,0,0);
    }
#else
  snd_packets_type *packet;

  /* Get a Sound Packet and queue the Request.
   */

  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd = SND_SBCCODEC_DATA;
    packet->pp_resp_ctl.pp_id  = pp_id;
    packet->pp_resp_ctl.buffer = buffer;
    packet->pp_resp_ctl.buf_size = buf_size; 
    snd_cmd(packet);
  } else {
    MSG_MED("Dropped PP data",0,0,0);
  }
#endif
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_ctl_req

DESCRIPTION
  Registers the  BT SBC codec callback with the vocoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_sbc_ctl_req 
( 
  snd_pp_cb_type                sbc_cb, 
  snd_cb_func_ptr_type          callback_ptr,
  const void                    *client_data
) {

  snd_sbc_client_cb = sbc_cb;

  voc_sbc_set_cb(snd_sbc_callback);

  if (callback_ptr != NULL) {
    callback_ptr(client_data, SND_SUCCESS);
  }
}
#ifndef FEATURE_AVS_SBCENC_TASK
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_set_bitrate_req

DESCRIPTION
 Sets the SBC codec bitrate 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_sbc_set_bitrate_req 
( 
  snd_sbc_bitrate_type          bitrate_hint,
  uint32                        bitrate, 
  snd_cb_func_ptr_type          callback_ptr,
  const void                    *client_data
) {
  boolean retval         = FALSE;
  snd_status_type status = SND_FAILURE;

  retval = audiosbc_set_bitrate(bitrate_hint, bitrate);
  if (retval)
  {
    status = SND_SUCCESS;
  }
  if (callback_ptr != NULL) {
    callback_ptr(client_data, status);
  }
}
#endif
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_data

DESCRIPTION
  This is the function called by snd task to pass the PCM data from 
  SND task to the client. This function will copy the arguments into 
  snd_buffer_type and pass it to the callback function registered by the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_sbc_data 
(
  uint16    buf_size, 
  uint16    *buffer, 
  uint16    pp_id, 
  uint16    num_channels
) {
  snd_pp_buffer_type pp_buffer;
  if (snd_sbc_client_cb != NULL) 
  {
    pp_buffer.pp_id             = pp_id;
    pp_buffer.pp_buf_length     = buf_size;
    pp_buffer.pp_data           = buffer;
    pp_buffer.pp_num_channels   = num_channels;
    snd_sbc_client_cb (pp_buffer);
  }
  else
  {
    sndhw_free_wb_pcm_rx_interface_buffer(pp_id);
    MSG_ERROR("snd sbc data client callback is null \n", 0,0,0);
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_init_req

DESCRIPTION
 Initializes the SBC codec.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_sbc_init_req 
( 
  snd_sbc_frame_cb_func_ptr_type  sbc_cb,
  snd_cb_func_ptr_type            callback_ptr,
  const void                      *client_data
) {

  audiosbc_init(sbc_cb);
  if (callback_ptr != NULL) {
    callback_ptr(client_data, SND_SUCCESS);
  }
}


#ifndef FEATURE_AVS_SBCENC_TASK 
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_start_timer

DESCRIPTION
  This function sets the SBC timer. This function is called from
  SBC module.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_sbc_start_timer(void)
{
  rex_set_timer( &snd_sbc_codec_timer, SND_SBC_CODEC_TIMER);
}
#endif
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_config_req

DESCRIPTION
  This function configures the audio SBC encoder's parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_sbc_config_req
(
  audiosbcenc_data_type    *params,
  snd_cb_func_ptr_type     callback_ptr,
  const void               *client_data
)
{
  snd_status_type status = SND_SUCCESS;

  audiosbc_set_enc_params(params);/*Call to modify the SBC configuration*/

  if (callback_ptr != NULL) {
    callback_ptr(client_data, status);
  }
}
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_AVS_I2SSBC
/* <EJECT> */
/*===========================================================================

FUNCTION snd_i2ssbc_enable

DESCRIPTION
  This function enables the I2S SBC interface 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_i2ssbc_enable_req
(
  snd_sample_rate_type        sample_rate,      /* SADC interface sample rate */
  snd_i2ssbc_cb_func_ptr_type i2ssbc_cb_ptr,    /* I2SSBC status cb func      */
  snd_cb_func_ptr_type        callback_ptr,
  const void                  *client_data
)
{
  snd_status_type status;

  status = sndhw_i2ssbc_enable(sample_rate, i2ssbc_cb_ptr);

  if (callback_ptr != NULL) {
    callback_ptr(client_data, status);
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_i2ssbc_disable

DESCRIPTION
  This function disables the I2S SBC interface 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_i2ssbc_disable_req
(
  snd_cb_func_ptr_type        callback_ptr,
  const void                  *client_data
)
{
  sndhw_i2ssbc_disable();
  if (callback_ptr != NULL) {
    callback_ptr(client_data, SND_SUCCESS);
  }

}
#endif /* FEATURE_AVS_I2SSBC */
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pp_callback

DESCRIPTION
  Callback which receives Post Processing data from the DSP in interrupt
  context and enqueues it to the sound task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_pp_callback 
( 
  uint16  buf_size,
  uint16  *buffer,
  uint16  pp_id
)
{
  snd_packets_type *packet;

  /* Get a Sound Packet and queue the Request.
   */

  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd = SND_PP_DSP_DATA;
    packet->pp_resp_ctl.pp_id  = pp_id;
    packet->pp_resp_ctl.buffer = buffer;
    packet->pp_resp_ctl.buf_size = buf_size; 
    if (voc_pp_stereo_codec()) 
    {
      packet->pp_resp_ctl.num_channels = 2;
    }
    else
    {
      packet->pp_resp_ctl.num_channels = 1;
    }
    snd_cmd(packet);
  } else {
    MSG_MED("Dropped PP data",0,0,0);
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_pp_ctl_req

DESCRIPTION
  Registers the  Post processing callback with the vocoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_pp_ctl_req 
( 
  snd_pp_cb_type                pp_cb, 
  snd_cb_func_ptr_type          callback_ptr,
  const void                    *client_data
) {

  snd_pp_client_cb = pp_cb;

  if (pp_cb == NULL) {
    voc_pp_ctl(NULL, VOC_PP_TYPE_GENERIC);
  } else {
    voc_pp_ctl(snd_pp_callback, VOC_PP_TYPE_GENERIC);
  }

  if (callback_ptr != NULL) {
    callback_ptr(client_data, SND_SUCCESS);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pp_dsp_data

DESCRIPTION
  This is the function called by snd task to pass the post processing data from 
  SND task to the client. This function will copy the arguments into 
  snd_buffer_type and pass it to the callback function registered by the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void snd_pp_dsp_data 
(
  uint16    buf_size, 
  uint16    *buffer, 
  uint16    pp_id, 
  uint16    num_channels
) {
  snd_pp_buffer_type pp_buffer;

  if (snd_pp_client_cb != NULL) 
  {
    pp_buffer.pp_id             = pp_id;
    pp_buffer.pp_buf_length     = buf_size;
    pp_buffer.pp_data           = buffer;
    pp_buffer.pp_num_channels   = num_channels;
    snd_pp_client_cb (pp_buffer);
  }
}
#endif  /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */ 
/* <EJECT> */
/*===========================================================================

FUNCTION snd_get_time_req

DESCRIPTION
  This function initiates the mechanism to calculate the playback time
  of a midi/WebAudio file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  snd_af_get_time_ctl

===========================================================================*/
void snd_get_time_req (
  snd_sound_type             *sound_ptr,
  snd_time_cb_func_ptr_type  callback_ptr,
  const void                 *client_data
) {

  switch(sound_ptr->type) {
    case SND_COMPACT_SOUND:
#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif
    case SND_FLEXIBLE_SOUND:
      /* Not yet supported */
      if(callback_ptr != NULL) {
        callback_ptr( client_data, SND_FAILURE, 0 );
      }
      break;

#ifdef FEATURE_AUDIO_FORMAT
    case SND_AUDIO_FORMAT_SOUND:
    case SND_FORMAT_SOUND:
      snd_audfmt_get_time(
                  &(((snd_audfmt_sound_type *) sound_ptr)->audfmt_handle),
                  ((snd_audfmt_sound_type *) sound_ptr)->param.byte_offset,
                  callback_ptr, client_data );
      break;
#endif

#ifdef FEATURE_VOICE_PLAYBACK
    case SND_TONE_AND_VOICE:
      /* Currently not possible to calculate time for sounds with 
      ** voice prompts
      */
      if(callback_ptr != NULL) {
        callback_ptr( client_data, SND_FAILURE, 0 );
      }
      break;
#endif
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_get_spec_req

DESCRIPTION
  This function initiates the mechanism to get the audio format type of a sound.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void snd_get_spec_req (
  snd_sound_type            *sound_ptr,
  snd_spec_cb_func_ptr_type callback_ptr,
  const void                *client_data
) {
  switch(sound_ptr->type) {
    case SND_COMPACT_SOUND:
#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif
    case SND_FLEXIBLE_SOUND:
      /* Not yet supported */
      if(callback_ptr != NULL) {
        callback_ptr(client_data, SND_FAILURE);
      }
      break;

#ifdef FEATURE_AUDIO_FORMAT
    case SND_AUDIO_FORMAT_SOUND:
    case SND_FORMAT_SOUND:
      if(callback_ptr != NULL) {
        callback_ptr(client_data, SND_SUCCESS);
      }
      audmain_reset_parms();
      if(audmain_get_specs(
              &(((snd_audfmt_sound_type *) sound_ptr)->audfmt_handle),
              ((snd_audfmt_sound_type *) sound_ptr)->param.byte_offset) !=
         AUDMAIN_STATUS_SUCCESS) {
        if(callback_ptr != NULL) {
          callback_ptr(client_data, SND_FAILURE);
        }
      }
      break;
#endif

#ifdef FEATURE_VOICE_PLAYBACK
    case SND_TONE_AND_VOICE:
      /* Currently not possible to calculate time for sounds with 
      ** voice prompts
      */
      if(callback_ptr != NULL) {
        callback_ptr(client_data, SND_FAILURE);
      }
      break;
#endif
  }
}

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT
/*===========================================================================

FUNCTION snd_midi_out_req

DESCRIPTION
  This function enables/disables midi device.

DEPENDENCIES
  snd_midi_out_qcp_ctl
  snd_midi_out_enabled

RETURN VALUE
  None

SIDE EFFECTS
  snd_midi_out_cb
  snd_midi_out_client_data
  snd_midi_out_qcp_ctl
  snd_midi_out_enabled

===========================================================================*/
void snd_midi_out_req (
  snd_midi_out_type              enable,
  snd_cb_func_ptr_type           callback_ptr,
  snd_qsynth_adec_mode_enum_type adec_mode,
  const void                     *client_data
) {
#if defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_WEBAUDIO)
  audmain_adec_mode_enum_type audmain_adec_mode;

  switch (adec_mode) {
    case SND_QSYNTH_ADEC_MODE_PCM:
      audmain_adec_mode = AUDMAIN_ADEC_MODE_PCM;
      break;
    case SND_QSYNTH_ADEC_MODE_ADPCM:
      audmain_adec_mode = AUDMAIN_ADEC_MODE_ADPCM;
      break;
    case SND_QSYNTH_ADEC_MODE_QCP:
      audmain_adec_mode = AUDMAIN_ADEC_MODE_QCP;
      break;
    case SND_QSYNTH_ADEC_MODE_DEFAULT:
    default:
      audmain_adec_mode = AUDMAIN_ADEC_MODE_DEFAULT;
      break;
  }
#endif /* FEATURE_MIDI_OUT_QCP || FEATURE_WEBAUDIO */

  if (enable == SND_MIDI_OUT_ENABLE) {
    if(
#if defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_WEBAUDIO)
       (audmain_set_adec_mode(audmain_adec_mode) == AUDMAIN_STATUS_SUCCESS) &&
#endif /* FEATURE_MIDI_OUT_QCP || FEATURE_WEBAUDIO */
       (sndhw_midi_out(
#ifdef FEATURE_MIDI_OUT_QCP
                      snd_audfmt_adec_int, NULL,
#endif /* FEATURE_MIDI_OUT_QCP */
                      SND_MIDI_OUT_ENABLE) == SND_SUCCESS)) {
      if((snd_midi_out_enabled == SND_MIDI_OUT_STATE_ENABLED) ||
         sndhw_midi_out_path_ready()) {
        snd_midi_out_enabled = SND_MIDI_OUT_STATE_ENABLED;
        if (callback_ptr != NULL) {
          callback_ptr(client_data, SND_ENABLED);
        }
      } else {
#ifndef FEATURE_GASYNTH_INTERFACE
        if (audmain_adec_mode == AUDMAIN_ADEC_MODE_PCM) {
#ifdef FEATURE_AUDFMT_HYBRID_MODE 
          if (snd_audfmt_poly_mode != SND_AUDFMT_POLY_MODE_0) {
            snd_set_hybrid_mode(snd_audfmt_poly_mode);
          } else          
#endif /* FEATURE_AUDFMT_HYBRID_MODE */
          {
            snd_set_hybrid_mode(SND_AUDFMT_POLY_MODE_1);
          }
        } else 
#endif /* FEATURE_GASYNTH_INTERFACE */
        {
#ifdef FEATURE_AUDFMT_HYBRID_MODE
        snd_set_hybrid_mode(snd_audfmt_poly_mode);
#else
        snd_set_hybrid_mode(SND_AUDFMT_POLY_MODE_0);
#endif /* FEATURE_AUDFMT_HYBRID_MODE */
        }
        snd_midi_out_enabled = SND_MIDI_OUT_STATE_PENDING;
      }
      snd_midi_out_cb          = callback_ptr;
      snd_midi_out_client_data = client_data;
    } else if(callback_ptr != NULL) {
      callback_ptr(client_data, SND_FAILURE);
    }
  } else if ((enable == SND_MIDI_OUT_DISABLE) &&
             (snd_midi_out_enabled != SND_MIDI_OUT_STATE_DISABLED)) {
    if(sndhw_midi_out(
#ifdef FEATURE_MIDI_OUT_QCP
                      NULL, NULL,
#endif /* FEATURE_MIDI_OUT_QCP */
                      SND_MIDI_OUT_DISABLE) == SND_SUCCESS) {
#ifdef FEATURE_MIDI_OUT_QCP
      (void) audmain_midi_out_qcp_reset(snd_midi_out_qcp_ctl);
      snd_midi_out_qcp_ctl = NULL;
#endif /* FEATURE_MIDI_OUT_QCP */
      if(callback_ptr != NULL) {
        callback_ptr(client_data, SND_DISABLED);
      }
      snd_midi_out_cb         = NULL;
      snd_midi_out_enabled    = SND_MIDI_OUT_STATE_DISABLED;
#if defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_WEBAUDIO)
      audmain_set_adec_mode(AUDMAIN_ADEC_MODE_NONE);
#endif /* FEATURE_MIDI_OUT_QCP || FEATURE_WEBAUDIO */

#ifdef FEATURE_QAUDIOFX
      voc_3d_reverb_reset();
      voc_3d_pos_enable(VOC_3D_POS_MODE_DISABLE);
#endif /* FEATURE_QAUDIOFX */

    } else if(callback_ptr != NULL) {
      callback_ptr(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_msg_req

DESCRIPTION
  This function issues midi commands.

DEPENDENCIES
  snd_midi_out_cb
  snd_midi_out_enabled
  snd_midi_out_client_data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_midi_out_msg_req (
  uint8        *msg_buffer,
  uint32       length
) {
  uint16  num_words    = 0;
  uint16  num_commands = 0;
  uint16  *midi_buffer = snd_midi_out_buffer;
  uint16  tword;
  uint8   tbyte1, tbyte2;
  boolean error = FALSE;

  if(snd_midi_out_enabled == SND_MIDI_OUT_STATE_ENABLED) {
    if((length > 0) && (length <= SND_MIDI_OUT_STD_MAX) && (msg_buffer != NULL))
    {
      while(length) {
        tword = *msg_buffer & 0xF0;
        switch(tword) {
          case SND_MIDI_NOTE_OFF:
          case SND_MIDI_NOTE_ON:
          case SND_MIDI_AFTER_TOUCH:
          case SND_MIDI_CONTROL_CHANGE:
            if(length >= 3) {
              *midi_buffer++ = tword;
              *midi_buffer++ = *msg_buffer++ & 0x0F;
              tbyte1         = *msg_buffer++;
              tbyte2         = *msg_buffer++;
              if((tbyte1 > SND_MIDI_MAX_VALUE) || 
                 (tbyte2 > SND_MIDI_MAX_VALUE)) {
                error = TRUE;
                break;
              }
              *midi_buffer++ = tbyte1;
              *midi_buffer++ = tbyte2;
              num_commands++;
              num_words += 4;
              length    -= 3;
            } else {
              error = TRUE;
            }
            break;

          case SND_MIDI_PITCH_WHEEL:
            if(length >= 3) {
              *midi_buffer++ = tword;
              *midi_buffer++ = *msg_buffer++ & 0x0F;
              *midi_buffer++ = *msg_buffer++;
              *midi_buffer++ = *msg_buffer++;
              num_commands++;
              num_words += 4;
              length    -= 3;
            } else {
              error = TRUE;
            }
            break;
  
          case SND_MIDI_PROGRAM_CHANGE:
          case SND_MIDI_CHANNEL_PRESSURE:
            if(length >= 2) {
              *midi_buffer++ = tword;
              *midi_buffer++ = *msg_buffer++ & 0x0F;
              tbyte1         = *msg_buffer++;
              if(tbyte1 > SND_MIDI_MAX_VALUE) {
                error = TRUE;
                break;
              }
              *midi_buffer++ = tbyte1;
              num_commands++;
              num_words += 3;
              length    -= 2;
            } else {
              error = TRUE;
            }
            break;
  
          default:
            if(*msg_buffer == SND_MIDI_RESET) {
              *midi_buffer++ = *msg_buffer++;
              num_commands++;
              num_words++;
              length--;
            } else {
              error = TRUE;
            }
            break;
        }
        if(error) {
          if(snd_midi_out_cb != NULL) {
            snd_midi_out_cb(snd_midi_out_client_data, SND_FAILURE);
          }
          return;
        }
      }
    }

    if(sndhw_midi_out_msg(num_commands, num_words, snd_midi_out_buffer) == 
       SND_SUCCESS) {
      if(snd_midi_out_cb != NULL) {
        snd_midi_out_cb(snd_midi_out_client_data, SND_SUCCESS);
      }
    } else if(snd_midi_out_cb != NULL) {
      snd_midi_out_cb(snd_midi_out_client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION snd_midi_out_qcp_req

DESCRIPTION
  This function plays a QCP file to the MIDI device.

DEPENDENCIES
  snd_midi_out_cb
  snd_midi_out_enabled
  snd_midi_out_client_data

RETURN VALUE
  None

SIDE EFFECTS
  snd_midi_out_qcp_ctl

===========================================================================*/
void snd_midi_out_qcp_req (
  uint32                       channel,
  snd_af_access_type           *audfmt_handle,
  snd_midi_out_adec_param_type *param
) {
  audmain_midi_out_adec_param_type audmain_param;

  if(snd_midi_out_enabled == SND_MIDI_OUT_STATE_ENABLED) {
    audmain_param.repeat_num = param->repeat_num;

    switch (param->start_mode) {
      case SND_ADEC_START_MODE_DELAYED:
        audmain_param.start_mode = AUDMAIN_ADEC_START_MODE_DELAYED;
        break;

      case SND_ADEC_START_MODE_NORMAL:
      default:
        audmain_param.start_mode = AUDMAIN_ADEC_START_MODE_NORMAL;
        break;
    }

    switch (param->repeat_mode) {
      case SND_ADEC_REPEAT_MODE_ENABLED:
        audmain_param.repeat_mode = AUDMAIN_ADEC_REPEAT_MODE_ENABLED;
        break;

      case SND_ADEC_REPEAT_MODE_DISABLED:
      default:
        audmain_param.repeat_mode = AUDMAIN_ADEC_REPEAT_MODE_DISABLED;
        break;
    }

    if(audmain_midi_out_qcp(&snd_midi_out_qcp_ctl, channel, audfmt_handle,
                            &audmain_param) == AUDMAIN_STATUS_SUCCESS) {
      sndhw_audfmt_adec_flush(1<<channel);
      snd_audfmt_adec_chan_vol(channel, param->volume);
      snd_audfmt_adec_chan_pan(channel, param->pan);
      snd_audfmt_adec_int(NULL);
      if(snd_midi_out_cb != NULL) {
        snd_midi_out_cb(snd_midi_out_client_data, SND_SUCCESS);
      }
      return;
    }
  }

  if(snd_midi_out_cb != NULL) {
    snd_midi_out_cb(snd_midi_out_client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_adec_start_chan_req

DESCRIPTION
  This function starts adec file playing to the MIDI device.

DEPENDENCIES
  snd_midi_out_cb
  snd_midi_out_client_data

RETURN VALUE
  None

SIDE EFFECTS
  snd_midi_out_qcp_ctl

===========================================================================*/
void snd_midi_out_adec_start_chan_req ( uint32 channel )
{
  if((snd_midi_out_qcp_ctl != NULL) &&
     (audmain_midi_out_adec_start_channel(snd_midi_out_qcp_ctl, channel) ==
      AUDMAIN_STATUS_SUCCESS)) {
    if(snd_midi_out_cb != NULL) {
      snd_midi_out_cb(snd_midi_out_client_data, SND_SUCCESS);
    }
  } else if(snd_midi_out_cb != NULL) {
    snd_midi_out_cb(snd_midi_out_client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_stop_req

DESCRIPTION
  This function stops a QCP file playing to the MIDI device.

DEPENDENCIES
  snd_midi_out_cb
  snd_midi_out_client_data

RETURN VALUE
  None

SIDE EFFECTS
  snd_midi_out_qcp_ctl

===========================================================================*/
void snd_midi_out_qcp_stop_req ( uint32 channel )
{
  audmain_status_type   status;

  if((snd_midi_out_qcp_ctl != NULL) &&
     audmain_check_midi_out_qcp_channel(snd_midi_out_qcp_ctl, channel)) {
    sndhw_audfmt_adec_flush(1<<channel);
    status = audmain_adec_reset_channel(snd_midi_out_qcp_ctl, channel);

    if(status == AUDMAIN_STATUS_DONE) {
      snd_midi_out_qcp_ctl = NULL;
    }
    if(snd_midi_out_cb != NULL) {
      snd_midi_out_cb(snd_midi_out_client_data, SND_SUCCESS);
    }
  } else if(snd_midi_out_cb != NULL) {
    snd_midi_out_cb(snd_midi_out_client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_pause_req

DESCRIPTION
  This function pauses/resumes QCP files playing to the MIDI device.

DEPENDENCIES
  snd_midi_out_cb
  snd_midi_out_client_data

RETURN VALUE
  None

SIDE EFFECTS
  snd_midi_out_qcp_ctl

===========================================================================*/
void snd_midi_out_qcp_pause_req ( boolean pause )
{
  if((snd_midi_out_qcp_ctl != NULL) &&
     (audmain_midi_out_qcp_pause(snd_midi_out_qcp_ctl, pause) ==
      AUDMAIN_STATUS_SUCCESS)) {
    if(snd_midi_out_cb != NULL) {
      snd_midi_out_cb(snd_midi_out_client_data, SND_SUCCESS);
    }
  } else if(snd_midi_out_cb != NULL) {
    snd_midi_out_cb(snd_midi_out_client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_pause_chan_req

DESCRIPTION
  This function pauses/resumes QCP file playing to the MIDI device.

DEPENDENCIES
  snd_midi_out_cb
  snd_midi_out_client_data

RETURN VALUE
  None

SIDE EFFECTS
  snd_midi_out_qcp_ctl

===========================================================================*/
void snd_midi_out_qcp_pause_chan_req ( boolean pause, uint32 channel )
{
  if((snd_midi_out_qcp_ctl != NULL) &&
     audmain_check_midi_out_qcp_channel(snd_midi_out_qcp_ctl, channel)) {
    snd_audfmt_adec_chan_pause(channel, pause);
    if(snd_midi_out_cb != NULL) {
      snd_midi_out_cb(snd_midi_out_client_data, SND_SUCCESS);
    }
  } else if(snd_midi_out_cb != NULL) {
    snd_midi_out_cb(snd_midi_out_client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_vol_req

DESCRIPTION
  This function pauses/resumes QCP files playing to the MIDI device.

DEPENDENCIES
  snd_midi_out_cb
  snd_midi_out_client_data

RETURN VALUE
  None

SIDE EFFECTS
  snd_midi_out_qcp_ctl

===========================================================================*/
void snd_midi_out_qcp_vol_req ( uint32 channel, uint16 vol )
{
  if((snd_midi_out_qcp_ctl != NULL) &&
     audmain_check_midi_out_qcp_channel(snd_midi_out_qcp_ctl, channel)) {
    snd_audfmt_adec_chan_vol(channel, vol);
    if(snd_midi_out_cb != NULL) {
      snd_midi_out_cb(snd_midi_out_client_data, SND_SUCCESS);
    }
  } else if(snd_midi_out_cb != NULL) {
    snd_midi_out_cb(snd_midi_out_client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_pan_req

DESCRIPTION
  This function pauses/resumes QCP files playing to the MIDI device.

DEPENDENCIES
  snd_midi_out_cb
  snd_midi_out_client_data

RETURN VALUE
  None

SIDE EFFECTS
  snd_midi_out_qcp_ctl

===========================================================================*/
void snd_midi_out_qcp_pan_req ( uint32 channel, sint15 pan )
{
  if((snd_midi_out_qcp_ctl != NULL) &&
     audmain_check_midi_out_qcp_channel(snd_midi_out_qcp_ctl, channel)) {
    snd_audfmt_adec_chan_pan(channel, pan);
    if(snd_midi_out_cb != NULL) {
      snd_midi_out_cb(snd_midi_out_client_data, SND_SUCCESS);
    }
  } else if(snd_midi_out_cb != NULL) {
    snd_midi_out_cb(snd_midi_out_client_data, SND_FAILURE);
  }
}
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_ec_ctl_req

DESCRIPTION
  Sets EC to force disabled or normal mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_ec_ctl_req ( snd_packets_type *cmd_ptr )
{
  sndhw_ec_ctl(cmd_ptr->ec_ns_mode.mode);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_ns_ctl_req

DESCRIPTION
  Sets NS to force disabled or normal mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_ns_ctl_req ( snd_packets_type *cmd_ptr )
{
  sndhw_ns_ctl(cmd_ptr->ec_ns_mode.mode);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_vox_ctl_req

DESCRIPTION
  Sets VOX to force enabled or normal mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_vox_ctl_req ( snd_packets_type *cmd_ptr )
{
  sndhw_vox_ctl(cmd_ptr->vox_mode.mode);
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sidetone_ctl_req

DESCRIPTION
  Sets sidetone to force disabled or normal mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_sidetone_ctl_req ( snd_packets_type *cmd_ptr )
{
  sndhw_sidetone_ctl(cmd_ptr->sidetone_mode.mode);
}
/* <EJECT> */

#ifdef FEATURE_AUDIO_AGC
/*===========================================================================

FUNCTION snd_avc_ctl_req

DESCRIPTION
  Enables\disables AVC (automatic volume control)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_avc_ctl_req ( snd_packets_type *cmd_ptr )
{
  SND_DEBUG_MSG("SND CMD: SND_AVC_CTL=%d",
                cmd_ptr->avc_ctl.on,0,0);
  sndhw_avc_ctl(cmd_ptr->avc_ctl.on);
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_agc_ctl_req

DESCRIPTION
  Enables\disables AGC (automatic gain control)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_agc_ctl_req ( snd_packets_type *cmd_ptr )
{
  SND_DEBUG_MSG("SND CMD: SND_AGC_CTL=%d",
                cmd_ptr->agc_ctl.on,0,0);
  sndhw_agc_ctl(cmd_ptr->agc_ctl.on);
}
#endif /* FEATURE_AUDIO_AGC */


/* <EJECT> */
/*==========================================================================

 FUNCTION snd_smartsnd_ctl_req

 DESCRIPTION
   Controls AGC/AVC/RVE.

 DEPENDENCIES
   None
   
 RETURN VALUE
   None

 SIDE EFFECTS
   None   
 ==========================================================================*/

void snd_smartsnd_ctl_req( snd_packets_type *cmd_ptr )
{
  SND_DEBUG_MSG("SND CMD: SND_SMARTSND_CTL=%d",
                cmd_ptr->smartsnd_ctl.smartsnd,0,0);
  sndhw_smartsnd_ctl(cmd_ptr->smartsnd_ctl.smartsnd);
}

/* <EJECT> */
#ifdef SND_HFK
/*===========================================================================

FUNCTION snd_mute_car_stereo_req

DESCRIPTION
  Mutes car stereo

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_mute_car_stereo_req ( snd_packets_type *cmd_ptr )
{
  SND_DEBUG_MSG("SND CMD: SND_MUTE_CAR_STEREO=%d",
                cmd_ptr->stereo.mute_stereo,0,0);
  sndhw_stereo_mute(cmd_ptr->stereo.mute_stereo);
} /* end of snd_mute_car_stereo */
#endif /* SND_HFK */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_filters_req

DESCRIPTION
  Select filters for voice or data

DEPENDENCIES
  None

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  None

===========================================================================*/
word snd_filters_req ( snd_packets_type *cmd_ptr )
{
  word change_mask = SND_NO_CHANGE_M;

  SND_DEBUG_MSG("SND CMD: SND_FILTERS=%d", cmd_ptr->filters.filters,0,0);

  sndhw_filters_ctl(cmd_ptr->filters.filters);

  change_mask |= SND_VOLUME_M;

  return(change_mask);
} /* end of snd_filters_req */
/* <EJECT> */
#ifdef SND_EXTCOM
#error code not present
#endif /* SND_EXTCOM */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
/*==========================================================================
 FUNCTION snd_multi_set_poly_mode

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void snd_multi_set_poly_mode(
  snd_audfmt_poly_mode_type mode,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd          = SND_MULTI_CMD_SET_POLY_MODE;
    packet->hdr.callback_ptr = cb_func;
    packet->hdr.client_data  = client_data;
    packet->pmode_set.mode   = mode;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/*==========================================================================
 FUNCTION snd_multi_get_poly_mode

 DESCRPTION
   This function gets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void snd_multi_get_poly_mode(
  snd_poly_mode_cb_func_ptr_type cb_func,
  const void                     *client_data
) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd           = SND_MULTI_CMD_GET_POLY_MODE;
    packet->hdr.client_data   = client_data;
    packet->pmode_get.pm_callback_ptr = cb_func;
    snd_cmd(packet);

  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE, SND_AUDFMT_POLY_MODE_0);
  }
}

/*==========================================================================
 FUNCTION snd_multi_set_poly_mode_req

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void snd_multi_set_poly_mode_req(
  snd_audfmt_poly_mode_type mode,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
) {
  snd_status_type               ret_status = SND_FAILURE;
  boolean                       set_poly = TRUE;
  audmain_audfmt_poly_mode_type audmode = AUDMAIN_AUDFMT_POLY_MODE_0;

  switch(mode) {
    case SND_AUDFMT_POLY_MODE_0:
      audmode = AUDMAIN_AUDFMT_POLY_MODE_0;
      break;
    case SND_AUDFMT_POLY_MODE_1:
      audmode = AUDMAIN_AUDFMT_POLY_MODE_1;
      break;
    case SND_AUDFMT_POLY_MODE_2:
      audmode = AUDMAIN_AUDFMT_POLY_MODE_2;
      break;
    case SND_AUDFMT_POLY_MODE_3:
      audmode = AUDMAIN_AUDFMT_POLY_MODE_3;
      break;
    case SND_AUDFMT_POLY_MODE_4:
      audmode = AUDMAIN_AUDFMT_POLY_MODE_4;
      break;
    case SND_AUDFMT_POLY_MODE_5:
      audmode = AUDMAIN_AUDFMT_POLY_MODE_5;
     break;
    case SND_AUDFMT_POLY_MODE_6:
      audmode = AUDMAIN_AUDFMT_POLY_MODE_6;
      break;
    case SND_AUDFMT_POLY_MODE_7:
      audmode = AUDMAIN_AUDFMT_POLY_MODE_7;
      break;
    default:
      set_poly = FALSE;
  }

#ifdef FEATURE_MIDI_OUT
  if (set_poly == TRUE && 
      snd_midi_out_enabled != SND_MIDI_OUT_STATE_DISABLED) {
    set_poly = FALSE;
  }
#endif /* FEATURE_MIDI_OUT */

#ifdef FEATURE_MIDI
  if (set_poly == TRUE &&
      audmain_multi_set_poly_mode(audmode) != AUDMAIN_STATUS_SUCCESS) {
    set_poly = FALSE;
  }
#endif /* FEATURE_MIDI */

  if (set_poly == TRUE) {
    snd_audfmt_poly_mode = mode;
    ret_status = SND_SUCCESS;
  } 

  if (cb_func != NULL) {
    cb_func(client_data, ret_status);
  }

}

/*==========================================================================
 FUNCTION snd_multi_get_poly_mode_req

 DESCRPTION
   This function gets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void snd_multi_get_poly_mode_req(
  snd_poly_mode_cb_func_ptr_type cb_func,
  const void                    *client_data
) {

  if (cb_func != NULL) {
    cb_func(client_data, SND_SUCCESS, snd_audfmt_poly_mode);
  }

}
#endif /* FEATURE_AUDFMT_HYBRID_MODE */

#ifdef FEATURE_AUDIO_PCR_SUPPORT
/*===========================================================================

FUNCTION snd_audfmt_adjust_sample_req

DESCRIPTION
 This function is used to add/drop one audio sample.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
void snd_audfmt_adjust_sample_req(
  snd_priority_type              priority,
  boolean                        add_sample,
  snd_cb_func_ptr_type           callback_ptr,
  const void                     *client_data
)
{
  snd_sound_status_type   *snd_ptr;
  snd_status_type         ret_val     = SND_FAILURE;
  snd_array_idx_type      sound_index = SND_SND_LO; /* fixed Warning */

  switch (priority) {
    case SND_PRIO_LOW:
      sound_index       = SND_SND_LO;
      break;

    case SND_PRIO_MED:
      sound_index       = SND_SND_MED;
     break;

    case SND_PRIO_HIGH:
      sound_index       = SND_SND_HI;
      break;
  }

  snd_ptr = &(snd_array[ sound_index ]);

  
  if((snd_ptr->active) &&
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {
    
    if (sndhw_audfmt_adjust_sample(add_sample) == TRUE) {
      ret_val = SND_SUCCESS;
    }
  } 

  if (callback_ptr != NULL) {
    callback_ptr ( client_data, ret_val );
  }
}
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

/*===========================================================================

FUNCTION snd_set_aux_line_in_req

DESCRIPTION
  Process the aux line in on/off setting

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
 None

==========================================================================*/
void snd_set_aux_line_in_req (
  snd_aux_line_in_mode_type aux_mode
) 
{

  /* Set the aux line in */
  sndhw_set_aux_line_in(aux_mode);
                                            
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_req

DESCRIPTION
  This procedure processes requests received by snd_cmd().  See sndi.h for
  a description of the actual sound packets.

DEPENDENCIES
  snd_init() must have been called to init the command queue.

RETURN VALUE
  A mask of changes which have occurred.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL word snd_req( void )
{
  snd_packets_type *cmd_ptr;
    /* pointer to sound command packet */
  word change_mask = SND_NO_CHANGE_M;
    /* mask of audio state changes */

  change_mask = SND_NO_CHANGE_M;        /* Assume to 'no changes' */

  /* Clear the 'queued commands' signal prior to emptying the queue.
  ** This is done in order to avoid race conditions where an item is
  ** added to the queue just as we are done servicing the queue.
  */
  (void) rex_clr_sigs( &snd_tcb, SND_CMD_Q_SIG );

  /* While there are commands to process, process each command
  ** and set the corresponding change bit for the call to snd_change()
  ** when we're done processing the entire queue.
  */
  while ( (cmd_ptr = (snd_packets_type *) q_get(&snd_cmd_q)) != NULL ) {

    /* Process each command type
    */
    switch (cmd_ptr->hdr.cmd ) {

      case SND_SET_DEVICE:
        change_mask |= snd_set_device_req(cmd_ptr);
        break;

      case SND_SET_VOLUME:
        change_mask |= snd_set_volume_req(cmd_ptr);
        break;
      
      case SND_GET_VOLUME:
        snd_get_volume_req(cmd_ptr);
        break;
      
      case SND_GET_LEVELS:
        snd_get_levels_req(cmd_ptr);
        break;

      case SND_TONE_START:
      case SND_TONE_STOP:
        change_mask |= snd_tone_req(cmd_ptr);
        break;

      case SND_SOUND_ID_START:
        change_mask |= snd_sound_id_start_req(cmd_ptr);
        break;

      case SND_SOUND_START:
        change_mask |= snd_sound_start_req(cmd_ptr);
        break;

      case SND_SOUND_STOP:
        change_mask |= snd_sound_stop_req(cmd_ptr);
        break;

#ifdef FEATURE_VOICE_PLAYBACK
      case SND_VOICE_PROMPT_PLAY:
        change_mask |= snd_vp_play_req(cmd_ptr);
        break;

      case SND_VOICE_PROMPT_STOP:
        snd_vp_stop_req(cmd_ptr);
        break;

      case SND_VOICE_PROMPT_SIG:
        snd_vp_sig_req(cmd_ptr);
        change_mask |= SND_VP_SIGNAL;
        break;
#endif /* FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_VOICE_MEMO
      case SND_VOICE_PAUSE_RESUME:
        snd_vp_pause_resume_req(cmd_ptr);
        break;
#endif

      case SND_EC_MODE_CTL:
        snd_ec_ctl_req(cmd_ptr);
        break;

      case SND_NS_MODE_CTL:
        snd_ns_ctl_req(cmd_ptr);
        break;

      case SND_VOX_MODE_CTL:
        snd_vox_ctl_req(cmd_ptr);
        break;

      case SND_SIDETONE_MODE_CTL:
        snd_sidetone_ctl_req(cmd_ptr);
        break;

#ifdef FEATURE_AUDIO_AGC
      case SND_AVC_CTL:
        snd_avc_ctl_req(cmd_ptr);
        break;

      case SND_AGC_CTL:
        snd_agc_ctl_req(cmd_ptr);
        break;
#endif


      case SND_SMARTSND_CTL:
        snd_smartsnd_ctl_req(cmd_ptr);
        break;
     
        
#ifdef SND_HFK
      case SND_MUTE_CAR_STEREO:
        snd_mute_car_stereo_req(cmd_ptr);
        break;
#endif /* SND_HFK */

      /* Select Hardware Filtering     */
      case SND_FILTERS:
        change_mask |= snd_filters_req(cmd_ptr);
        break;

#ifdef SND_EXTCOM
#error code not present
#endif /* SND_EXTCOM */
        
      case SND_SOUND_GET_TIME:
        snd_get_time_req ( cmd_ptr->sound_get_time.sound_ptr,
                           cmd_ptr->sound_get_time.cb_func,
                           cmd_ptr->sound_get_time.hdr.client_data );
        break;

      case SND_SOUND_GET_SPEC:
        snd_get_spec_req ( cmd_ptr->sound_get_spec.sound_ptr,
                           cmd_ptr->sound_get_spec.cb_func,
                           cmd_ptr->sound_get_spec.hdr.client_data );
        break;

#ifdef FEATURE_AUDIO_FORMAT
      case SND_SOUND_FLUSH:
        change_mask |= snd_sound_flush_req(cmd_ptr->sound_flow.priority,
                                           cmd_ptr->sound_flow.hdr.callback_ptr,
                                           cmd_ptr->sound_flow.hdr.client_data);
        break;

      case SND_SOUND_REWIND:
        change_mask |= snd_sound_rewind_req(cmd_ptr->sound_flow.priority,
                                           cmd_ptr->sound_flow.time_ms,
                                           cmd_ptr->sound_flow.hdr.callback_ptr,
                                           cmd_ptr->sound_flow.hdr.client_data);
        break;

      case SND_SOUND_FFORWARD:
        change_mask |= snd_sound_ff_req(cmd_ptr->sound_flow.priority,
                                        cmd_ptr->sound_flow.time_ms,
                                        cmd_ptr->sound_flow.hdr.callback_ptr,
                                        cmd_ptr->sound_flow.hdr.client_data);
        break;

      case SND_SOUND_PAUSE:
        change_mask |= snd_sound_pause_req(cmd_ptr->sound_flow.priority, TRUE,
                                           cmd_ptr->sound_flow.hdr.callback_ptr,
                                           cmd_ptr->sound_flow.hdr.client_data);
        break;

      case SND_SOUND_SEEK_PAUSE:
        change_mask |= snd_sound_seek_pause_req(
                                           cmd_ptr->sound_flow.priority,
                                           cmd_ptr->sound_flow.hdr.callback_ptr,
                                           cmd_ptr->sound_flow.hdr.client_data);
        break;

      case SND_SOUND_RESUME:
        change_mask |= snd_sound_pause_req(cmd_ptr->sound_flow.priority, FALSE,
                                           cmd_ptr->sound_flow.hdr.callback_ptr,
                                           cmd_ptr->sound_flow.hdr.client_data);
        clk_uptime_ms(snd_audfmt_interval);
        break;

      case SND_SOUND_TEMPO:
        snd_sound_tempo_req(cmd_ptr->sound_flow.priority,
                            cmd_ptr->sound_flow.time_ms,
                            cmd_ptr->sound_flow.hdr.callback_ptr,
                            cmd_ptr->sound_flow.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_SOUND_TUNE:
        snd_sound_tune_req(cmd_ptr->sound_flow.priority,
                           (uint8) cmd_ptr->sound_flow.time_ms,
                           cmd_ptr->sound_flow.hdr.callback_ptr,
                           cmd_ptr->sound_flow.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_SOUND_PAN:
        snd_sound_pan_req(cmd_ptr->sound_flow.priority,
                          (uint8) cmd_ptr->sound_flow.time_ms,
                          cmd_ptr->sound_flow.hdr.callback_ptr,
                          cmd_ptr->sound_flow.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_SEQ_STOP:
        snd_sequence_stop_req(cmd_ptr->seq_cmd.sequence,
                              cmd_ptr->seq_cmd.hdr.callback_ptr,
                              cmd_ptr->seq_cmd.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_SEQ_PAUSE:
        snd_sequence_pause_req(cmd_ptr->seq_cmd.sequence, TRUE,
                               cmd_ptr->seq_cmd.hdr.callback_ptr,
                               cmd_ptr->seq_cmd.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_SEQ_RESUME:
        snd_sequence_pause_req(cmd_ptr->seq_cmd.sequence, FALSE,
                               cmd_ptr->seq_cmd.hdr.callback_ptr,
                               cmd_ptr->seq_cmd.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_SEQ_FFORWARD:
        snd_sequence_fforward_req(cmd_ptr->seq_cmd.sequence,
                                  cmd_ptr->seq_cmd.num,
                                  cmd_ptr->seq_cmd.hdr.callback_ptr,
                                  cmd_ptr->seq_cmd.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_SEQ_REWIND:
        snd_sequence_rewind_req(cmd_ptr->seq_cmd.sequence, cmd_ptr->seq_cmd.num,
                                cmd_ptr->seq_cmd.hdr.callback_ptr,
                                cmd_ptr->seq_cmd.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_SEQ_SET_TEMPO:
        snd_sequence_set_tempo_req(cmd_ptr->seq_cmd.sequence,
                                   cmd_ptr->seq_cmd.num,
                                   cmd_ptr->seq_cmd.hdr.callback_ptr,
                                   cmd_ptr->seq_cmd.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_SEQ_SET_TUNE:
        snd_sequence_set_tune_req(cmd_ptr->seq_cmd.sequence,
                                  cmd_ptr->seq_cmd.num,
                                  cmd_ptr->seq_cmd.hdr.callback_ptr,
                                  cmd_ptr->seq_cmd.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_SEQ_SET_PAN:
        snd_sequence_set_pan_req(cmd_ptr->seq_cmd.sequence,
                                 cmd_ptr->seq_cmd.num,
                                 cmd_ptr->seq_cmd.hdr.callback_ptr,
                                 cmd_ptr->seq_cmd.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_SEQ_SET_VOL:
        snd_sequence_set_vol_req(cmd_ptr->seq_cmd.sequence,
                                 cmd_ptr->seq_cmd.num,
                                 cmd_ptr->seq_cmd.hdr.callback_ptr,
                                 cmd_ptr->seq_cmd.hdr.client_data);
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_GET_TIME_ABORT:
        snd_audfmt_gt_abort_req ( cmd_ptr->hdr.callback_ptr,
                                  cmd_ptr->hdr.client_data );
        cmd_ptr->hdr.callback_ptr = NULL;
        break;

      case SND_AUDFMT_READ_CALLBACK:
        if(audmain_get_data_cb_req ( cmd_ptr->cmx_af_read_cb.data,
                                     cmd_ptr->cmx_af_read_cb.buf,
                                     cmd_ptr->cmx_af_read_cb.length,
                                     cmd_ptr->cmx_af_read_cb.pos )) {
#ifdef FEATURE_MIDI_OUT_QCP
          if(audmain_check_midi_out_qcp(snd_midi_out_qcp_ctl)) {
            snd_audfadec_int(snd_midi_out_qcp_ctl);
          } 
#endif /* FEATURE_MIDI_OUT_QCP */
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_MIDI_OUT_QCP
          else
#endif /* FEATURE_MIDI_OUT_QCP */
          {
            (void )snd_check_wa_audfadec_int();
          }
#endif /* FEATURE_WEBAUDIO */
          
          /* If return variable is not zero, start get time and fast-forward
          ** sequences as they may have been waiting for a data buffer.
          */
          (void) snd_audfmt_do_time();
          (void) snd_audfmt_do_seek();
          change_mask |= SND_CHANGE_M;
        }
        break;

      case SND_AUDFMT_PREPARSE_DONE:
        change_mask |= SND_CHANGE_M;
        break;

      case SND_AUDFMT_HEADER_CALLBACK:
        audmain_header_data_cb_req ( cmd_ptr->cmx_af_read_cb.data,
                                     cmd_ptr->cmx_af_read_cb.buf,
                                     cmd_ptr->cmx_af_read_cb.length,
                                     cmd_ptr->cmx_af_read_cb.pos );
        break;

      case SND_AUDFMT_DLS_CALLBACK:
        audmain_dls_data_cb_req ( cmd_ptr->cmx_af_dls_cb.data,
                                  cmd_ptr->cmx_af_dls_cb.buf,
                                  cmd_ptr->cmx_af_dls_cb.length,
                                  cmd_ptr->cmx_af_dls_cb.client_data );
        break;

#ifdef FEATURE_DLS
      case SND_AUDFMT_DLS_LOAD:
        audmain_dls_load(cmd_ptr->audfmt_dls.audfmt_handle,
                         cmd_ptr->audfmt_dls.byte_offset);
        break;

      case SND_AUDFMT_DLS_UNLOAD:
        audmain_dls_unload(cmd_ptr->audfmt_dls.audfmt_handle);
        break;

      case SND_AUDFMT_DLS_READ_CALLBACK:
        audmain_dls_get_data_cb_req ( cmd_ptr->cmx_af_read_cb.client_data,
                                      cmd_ptr->cmx_af_read_cb.buf,
                                      cmd_ptr->cmx_af_read_cb.length,
                                      cmd_ptr->cmx_af_read_cb.pos );
        break;
#endif /* FEATURE_DLS */

#ifdef FEATURE_MIDI
      case SND_AUDFMT_CHECK_RESET:
        audmain_audfmt_check_reset_req();
        break;
#endif /* FEATURE_MIDI */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
      case SND_AUDFMT_ADEC_INT:
#ifdef FEATURE_MIDI_OUT_QCP
        if(audmain_check_midi_out_qcp(snd_midi_out_qcp_ctl)) {
          snd_audfadec_int(snd_midi_out_qcp_ctl);
        } else
#endif /* FEATURE_MIDI_OUT_QCP */
#ifdef FEATURE_WEBAUDIO
        if (!snd_check_wa_audfadec_int())
#endif /* FEATURE_WEBAUDIO */
        {
          change_mask |= SND_CHANGE_M;
        }
        break;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

#ifdef FEATURE_QCP
      case SND_QCP_GET_FUNC:
        snd_qcp_get_func_req (cmd_ptr->qcp_get_func.func,
                              cmd_ptr->qcp_get_func.qcp_handle);
        break;

      case SND_QCP_SET_FUNC:
        snd_qcp_set_func_req (cmd_ptr->qcp_set_func.func,
                              cmd_ptr->qcp_set_func.buffer,
                              cmd_ptr->qcp_set_func.num_bytes,
                              cmd_ptr->qcp_set_func.handle,
                              cmd_ptr->qcp_set_func.cb_func,
                              cmd_ptr->qcp_set_func.client_data);
        break;

      case SND_QCP_RECORD:
        snd_qcp_record_req ( cmd_ptr->qcp_rec.format,
                             cmd_ptr->qcp_rec.link,
                             cmd_ptr->qcp_rec.rec_qcp_param,
                             cmd_ptr->qcp_rec.cb_func,
                             cmd_ptr->qcp_rec.client_data );
        break;

      case SND_QCP_RECORD_STOP:
        snd_qcp_record_stop_req ( cmd_ptr->hdr.callback_ptr,
                                  cmd_ptr->hdr.client_data );
        break;

      case SND_QCP_RECORD_PAUSE:
        snd_qcp_record_pause_req ( cmd_ptr->hdr.callback_ptr,
                                   cmd_ptr->hdr.client_data );
        break;

      case SND_QCP_RECORD_RESUME:
        snd_qcp_record_resume_req ( cmd_ptr->hdr.callback_ptr,
                                    cmd_ptr->hdr.client_data );
        break;

      case SND_QCP_REC_VOC_CB:
        snd_qcp_rec_voc_cb_req ( cmd_ptr );
        break;

      case SND_QCP_RECORD_BUF:
        snd_qcp_rec_new_buffer_req ( cmd_ptr->qcp_buffer.buffer, 
                                     cmd_ptr->qcp_buffer.num_bytes );
        break;

#endif /* FEATURE_QCP */

#ifdef FEATURE_PCM_REC
      case SND_PCM_RECORD:
        snd_pcm_record_req ( cmd_ptr->pcm_rec.link,
                             cmd_ptr->pcm_rec.buf1,
                             cmd_ptr->pcm_rec.num_buf1,
                             cmd_ptr->pcm_rec.buf2,
                             cmd_ptr->pcm_rec.num_buf2,
                             cmd_ptr->pcm_rec.cb_func,
                             cmd_ptr->pcm_rec.client_data );
        break;

      case SND_PCM_RECORD_BUF:
        audmain_pcm_record_buf( cmd_ptr->pcm_rec_buf.buffer,
                                cmd_ptr->pcm_rec_buf.num_bytes,
                                cmd_ptr->pcm_rec_buf.client_data );
        break;

      case SND_PCM_RECORD_STOP:
        snd_pcm_record_stop_req ( cmd_ptr->pcm_rec_cmd.link,
                                  cmd_ptr->hdr.callback_ptr,
                                  cmd_ptr->hdr.client_data );
        break;

      case SND_PCM_RECORD_PAUSE:
        snd_pcm_record_pause_req ( TRUE, cmd_ptr->pcm_rec_cmd.link,
                                   cmd_ptr->hdr.callback_ptr,
                                   cmd_ptr->hdr.client_data );
        break;

      case SND_PCM_RECORD_RESUME:
        snd_pcm_record_pause_req ( FALSE, cmd_ptr->pcm_rec_cmd.link,
                                   cmd_ptr->hdr.callback_ptr,
                                   cmd_ptr->hdr.client_data );
        break;

      case SND_PCM_RECORD_STATUS:
        snd_pcm_record_status_req ( cmd_ptr->pcm_rec_status.status,
                                    &cmd_ptr->pcm_rec_status.data,
                                    cmd_ptr->pcm_rec_status.cb_func,
                                    cmd_ptr->pcm_rec_status.client_data );
        break;
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
      case SND_MM_RECORD:
        snd_mm_record_req (cmd_ptr->mm_rec.dir,
                           &(cmd_ptr->mm_rec.rec_param),
                           &(cmd_ptr->mm_rec.buf),
                           cmd_ptr->mm_rec.cb_func,
                           cmd_ptr->mm_rec.client_data);
        break;

      case SND_MM_RECORD_BUF:
        audmain_mm_record_buf(cmd_ptr->mm_rec_buf.buffer,
                              cmd_ptr->mm_rec_buf.num_bytes,
                              cmd_ptr->mm_rec_buf.client_data, 
                              snd_mm_rec_ctrl.format);
        break;

      case SND_MM_RECORD_STOP:
        snd_mm_record_stop_req (cmd_ptr->mm_rec_cmd.link,
                                cmd_ptr->mm_rec_cmd.hdr.callback_ptr,
                                cmd_ptr->mm_rec_cmd.hdr.client_data);
        break;

      case SND_MM_RECORD_PAUSE:
        snd_mm_record_pause_req (TRUE, 
                                 cmd_ptr->mm_rec_cmd.link,
                                 cmd_ptr->mm_rec_cmd.hdr.callback_ptr,
                                 cmd_ptr->mm_rec_cmd.hdr.client_data);
        break;

      case SND_MM_RECORD_RESUME:
        snd_mm_record_pause_req (FALSE, 
                                 cmd_ptr->mm_rec_cmd.link,
                                 cmd_ptr->mm_rec_cmd.hdr.callback_ptr,
                                 cmd_ptr->mm_rec_cmd.hdr.client_data);
        break;

      case SND_MM_RECORD_STATUS:
        snd_mm_record_status_req (cmd_ptr->mm_rec_status.status,
                                  &cmd_ptr->mm_rec_status.data,
                                  cmd_ptr->mm_rec_status.cb_func,
                                  cmd_ptr->mm_rec_status.client_data,
                                  cmd_ptr->mm_rec_status.format);
        break;

      case SND_MM_RECORD_ISR:
        snd_mm_record_data_int();
        break;
#endif /* FEATURE_MM_REC */

      case SND_AUX_PCM_CLK_ENABLE:
        snd_aux_pcm_clk_enable_req(cmd_ptr->aux_pcm_clk_ctl.callback_ptr);
		break;
      case SND_AUX_PCM_CLK_DISABLE:
        snd_aux_pcm_clk_disable_req(cmd_ptr->aux_pcm_clk_ctl.callback_ptr);
		break;

#ifdef FEATURE_AUDFMT_EQUALIZER
      case SND_EQUALIZER_CTL:
        snd_equalizer_ctl_req(cmd_ptr->eq_ctl.enable,
                              cmd_ptr->eq_ctl.callback_ptr,
                              cmd_ptr->eq_ctl.client_data);
        break;

      case SND_EQUALIZER_SET_FUNC:
        snd_equalizer_set_req(cmd_ptr->eq_set.filter,
                              cmd_ptr->eq_set.num,
                              cmd_ptr->eq_set.callback_ptr,
                              cmd_ptr->eq_set.client_data);
        break;
 
      case SND_EQUALIZER_GAIN:
        snd_equalizer_gain_req(cmd_ptr->eq_gain.band,
                               cmd_ptr->eq_gain.gain,
                               cmd_ptr->eq_gain.callback_ptr,
                               cmd_ptr->eq_gain.client_data);
        break;
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
      case SND_SPECTRUM_ANALYZER_CTL:
        snd_spectrum_analyzer_ctl_req(cmd_ptr->sp_ctl.update_period,
                                      cmd_ptr->sp_ctl.cb_func,
                                      cmd_ptr->sp_ctl.callback_ptr,
                                      cmd_ptr->sp_ctl.client_data);
        break;

      case SND_SPECTRUM_ANALYZER_DATA:
        snd_spectrum_analyzer_data_int();
        break;
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
        
#ifdef FEATURE_AUDFMT_QCONCERT
      case SND_QCONCERT_CTL:
        snd_qconcert_ctl_req(cmd_ptr->qconcert_ctl.enable,
                             cmd_ptr->qconcert_ctl.callback_ptr,
                             cmd_ptr->qconcert_ctl.client_data);
        break;
 
      case SND_QCONCERT_PARAMS:
        snd_set_qconcert_params_req(&cmd_ptr->qconcert_params.params,
                                    cmd_ptr->qconcert_params.callback_ptr,
                                    cmd_ptr->qconcert_params.client_data);
        break;
#endif /* FEATURE_AUDFMT_QCONCERT */
      case SND_AUDFMT_AV_SYNC:
#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)

      audiosbc_process_avsync( cmd_ptr->av_sync.timestamp,
                               cmd_ptr->av_sync.num_of_samples,
                               cmd_ptr->av_sync.num_of_bytes);
        break;

#else
    
      snd_av_sync_int_cb_req(cmd_ptr->av_sync.timestamp,
                               cmd_ptr->av_sync.num_of_samples,
                               cmd_ptr->av_sync.num_of_bytes);
    
        break;
#endif /* defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)*/
#endif /* FEATURE_AUDIO_FORMAT */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
      case SND_SBCCODEC_CTL: 
        snd_sbc_ctl_req(cmd_ptr->pp_ctl.pp_cb_func, 
                     cmd_ptr->pp_ctl.hdr.callback_ptr,
                     cmd_ptr->pp_ctl.hdr.client_data);
        break;

      case SND_SBCCODEC_DATA:
        snd_sbc_data(cmd_ptr->pp_resp_ctl.buf_size,
                      cmd_ptr->pp_resp_ctl.buffer,
                      cmd_ptr->pp_resp_ctl.pp_id,
                      cmd_ptr->pp_resp_ctl.num_channels);
        break;
#ifndef FEATURE_AVS_SBCENC_TASK
      case SND_SBCCODEC_BITRATE:
        snd_sbc_set_bitrate_req(cmd_ptr->sbc_bitrate_ctl.bitrate_hint,
                              cmd_ptr->sbc_bitrate_ctl.sbc_bitrate,
                              cmd_ptr->sbc_bitrate_ctl.hdr.callback_ptr,
                              cmd_ptr->sbc_bitrate_ctl.hdr.client_data);
        break;
#endif

      case SND_SBCCODEC_INIT:
        snd_sbc_init_req(cmd_ptr->sbc_ctl.sbc_cb,
                        cmd_ptr->sbc_ctl.hdr.callback_ptr,
                        cmd_ptr->sbc_ctl.hdr.client_data);
      break;
    case SND_SBCCODEC_PARAMS:
      snd_sbc_config_req(&cmd_ptr->sbc_params.params,
                              cmd_ptr->sbc_params.callback_ptr,
                              cmd_ptr->sbc_params.client_data);
      break;
#endif /* FEATURE_SBC_CODEC */
#ifdef FEATURE_AVS_I2SSBC
      case SND_I2SSBC_ENABLE:
        snd_i2ssbc_enable_req (cmd_ptr->i2ssbc_ctl.sample_rate,
                               cmd_ptr->i2ssbc_ctl.i2ssbc_cb,
                               cmd_ptr->hdr.callback_ptr,
                               cmd_ptr->hdr.client_data);
      break;
      case SND_I2SSBC_DISABLE:
        snd_i2ssbc_disable_req (cmd_ptr->hdr.callback_ptr,
                               cmd_ptr->hdr.client_data);
      break;
#endif /* FEATURE_AVS_I2SBC */

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)

      case SND_PP_CTL:
        snd_pp_ctl_req(cmd_ptr->pp_ctl.pp_cb_func, 
                       cmd_ptr->pp_ctl.hdr.callback_ptr,
                       cmd_ptr->pp_ctl.hdr.client_data);
        break;

      case SND_PP_DSP_DATA:
        snd_pp_dsp_data(cmd_ptr->pp_resp_ctl.buf_size,
                        cmd_ptr->pp_resp_ctl.buffer,
                        cmd_ptr->pp_resp_ctl.pp_id,
                        cmd_ptr->pp_resp_ctl.num_channels);
        break;

#endif  /*(FEATURE_ARM_POST_PROCESSING) ||(FEATURE_AVS_WB_PCM_RX_INTERFACE) */ 

#ifdef FEATURE_MIDI_OUT
      case SND_MIDI_OUT_OPEN:
        snd_midi_out_req(SND_MIDI_OUT_ENABLE,
                         cmd_ptr->midi_out_open.hdr.callback_ptr, 
                         cmd_ptr->midi_out_open.adec_mode,
                         cmd_ptr->midi_out_open.hdr.client_data );
        cmd_ptr->midi_out_open.hdr.callback_ptr = NULL;
        break;

      case SND_MIDI_OUT_CLOSE:
        snd_midi_out_req(SND_MIDI_OUT_DISABLE,
                         cmd_ptr->midi_out_open.hdr.callback_ptr, 
                         cmd_ptr->midi_out_open.adec_mode,
                         cmd_ptr->midi_out_open.hdr.client_data );
        cmd_ptr->midi_out_open.hdr.callback_ptr = NULL;
        break;

      case SND_MIDI_OUT_MSG:
        snd_midi_out_msg_req(cmd_ptr->midi_out.msg_buffer, 
                             cmd_ptr->midi_out.length );
        break;

#ifdef FEATURE_MIDI_OUT_QCP
      case SND_MIDI_OUT_QCP:
        snd_midi_out_qcp_req(cmd_ptr->midi_out_qcp.channel,
                             cmd_ptr->midi_out_qcp.audfmt_handle,
                             &cmd_ptr->midi_out_qcp.param);
        break;

      case SND_MIDI_OUT_ADEC_START_CHAN:
        snd_midi_out_adec_start_chan_req(cmd_ptr->midi_out_qcp.channel);
        break;

      case SND_MIDI_OUT_QCP_STOP:
        snd_midi_out_qcp_stop_req(cmd_ptr->midi_out_qcp.channel);
        break;

      case SND_MIDI_OUT_QCP_PAUSE:
        snd_midi_out_qcp_pause_req(TRUE);
        break;

      case SND_MIDI_OUT_QCP_RESUME:
        snd_midi_out_qcp_pause_req(FALSE);
        break;

      case SND_MIDI_OUT_QCP_PAUSE_CHAN:
        snd_midi_out_qcp_pause_chan_req(TRUE, cmd_ptr->midi_out_qcp.channel);
        break;

      case SND_MIDI_OUT_QCP_RESUME_CHAN:
        snd_midi_out_qcp_pause_chan_req(FALSE, cmd_ptr->midi_out_qcp.channel);
        break;

      case SND_MIDI_OUT_QCP_PAN:
        snd_midi_out_qcp_pan_req(cmd_ptr->midi_out_qcp.channel,
                                 cmd_ptr->midi_out_qcp.param.pan);
        break;

      case SND_MIDI_OUT_QCP_VOL:
        snd_midi_out_qcp_vol_req(cmd_ptr->midi_out_qcp.channel,
                                 cmd_ptr->midi_out_qcp.param.volume);
        break;
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

#ifdef FEATURE_QAUDIOFX
      case SND_3D_POS_CMD_ENABLE:
        snd_3d_pos_enable_req(cmd_ptr->enable_3d_pos.mode,
                              cmd_ptr->hdr.callback_ptr,
                              cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_RESET:
        snd_3d_pos_reset_req(cmd_ptr->hdr.callback_ptr,
                             cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_SET_LSNR_POS:
        snd_3d_pos_lsnr_pos_req(&cmd_ptr->position_3d_pos.position,
                                cmd_ptr->hdr.callback_ptr,
                                cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_SET_LSNR_ORIENT:
        snd_3d_pos_lsnr_orient_req(&cmd_ptr->orient_3d_pos.orient,
                                   cmd_ptr->hdr.callback_ptr,
                                   cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_SET_LSNR_VEL:
        snd_3d_pos_lsnr_vel_req(&cmd_ptr->velocity_3d_pos.velocity,
                                cmd_ptr->hdr.callback_ptr,
                                cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_SET_REVERB:
        snd_3d_reverb_set_req(cmd_ptr->reverb_3d_pos.reverb_type,
                              &cmd_ptr->reverb_3d_pos.reverb_param,
                              cmd_ptr->hdr.callback_ptr,
                              cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_REVERB_ENABLE:
        snd_3d_reverb_enable_req( cmd_ptr->rev_flag.reverb_enable,
                                  cmd_ptr->hdr.callback_ptr,
                                        cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_SRC_ENABLE:
        snd_3d_pos_src_enable_req(cmd_ptr->src_enable_3d.channel,
                                  cmd_ptr->src_enable_3d.mode,
                                  cmd_ptr->hdr.callback_ptr,
                                  cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_SRC_RESET:
        snd_3d_pos_src_reset_req(cmd_ptr->src_reset_3d.channel,
                                 cmd_ptr->hdr.callback_ptr,
                                 cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_SET_SRC_VOL:
        snd_3d_pos_src_vol_req(cmd_ptr->src_vol_3d_pos.channel,
                               &cmd_ptr->src_vol_3d_pos.src_vol,
                               cmd_ptr->hdr.callback_ptr,
                               cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_SET_SRC_VOL_ATTEN:
        snd_3d_pos_src_vol_atten_req(cmd_ptr->vol_atten_3d_pos.channel,
                                     &cmd_ptr->vol_atten_3d_pos.vol_atten,
                                     cmd_ptr->hdr.callback_ptr,
                                     cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_SET_SRC_POS:
        snd_3d_pos_src_pos_req(cmd_ptr->position_3d_pos.channel,
                               &cmd_ptr->position_3d_pos.position,
                               cmd_ptr->hdr.callback_ptr,
                               cmd_ptr->hdr.client_data);
        break;
      
      case SND_3D_POS_CMD_SET_SRC_VEL:
        snd_3d_pos_src_vel_req(cmd_ptr->velocity_3d_pos.channel,
                               &cmd_ptr->velocity_3d_pos.velocity,
                               cmd_ptr->hdr.callback_ptr,
                               cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_SET_REVERB_VOL:
        snd_3d_reverb_src_vol_req( cmd_ptr->reverb_vol_3d_pos.channel,
                                   cmd_ptr->reverb_vol_3d_pos.reverb_vol.volume,
                                   cmd_ptr->hdr.callback_ptr,
                                   cmd_ptr->hdr.client_data); 
        break;

      case SND_3D_POS_CMD_QUERY_ENV:
        snd_3d_pos_query_env_req(cmd_ptr->query_3d_pos.param,
                                 cmd_ptr->query_3d_pos.cb_func,
                                 cmd_ptr->hdr.client_data);
        break;

      case SND_3D_POS_CMD_QUERY_SRC:
        snd_3d_pos_query_src_req(cmd_ptr->query_3d_pos.param,
                                 cmd_ptr->query_3d_pos.channel,
                                 cmd_ptr->query_3d_pos.cb_func,
                                 cmd_ptr->hdr.client_data);
        break;

#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
      case SND_MULTI_CMD_SET_POLY_MODE:
        snd_multi_set_poly_mode_req(cmd_ptr->pmode_set.mode,
                                      cmd_ptr->pmode_set.hdr.callback_ptr,
                                      cmd_ptr->pmode_set.hdr.client_data);
        cmd_ptr->midi_out_open.hdr.callback_ptr = NULL;
        break;
      case SND_MULTI_CMD_GET_POLY_MODE:
        snd_multi_get_poly_mode_req(cmd_ptr->pmode_get.pm_callback_ptr,
                                    cmd_ptr->pmode_get.hdr.client_data);
        break;
#endif /* FEATURE_AUDFMT_HYBRID_MODE */

#ifdef FEATURE_AUDIO_PCR_SUPPORT
      case SND_AUDFMT_ADJUST_SAMPLE:
        snd_audfmt_adjust_sample_req(cmd_ptr->adjust_sample.priority,
                                     cmd_ptr->adjust_sample.add_sample,
                                     cmd_ptr->hdr.callback_ptr,
                                     cmd_ptr->hdr.client_data);
        break;
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
	    case SND_OP_DUAL_MONO_MODE:
        snd_audfmt_dual_mono_mode_req(cmd_ptr->dual_mono_mode.priority,
                                      cmd_ptr->dual_mono_mode.mode,
                                      cmd_ptr->hdr.callback_ptr,
                                      cmd_ptr->hdr.client_data);
        break;
      case SND_OP_SBR_MODE:
        snd_audfmt_sbr_mode_req(cmd_ptr->sbr_mode.priority,
                                cmd_ptr->sbr_mode.mode,
                                cmd_ptr->hdr.callback_ptr,
                                cmd_ptr->hdr.client_data);
        break;
      case SND_OP_CODEC_INFO_UPDATE:
        snd_audfmt_codec_update_req(&cmd_ptr->codec_update.codec_info, 
                                     cmd_ptr->codec_update.client_data);
        break;
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

      case SND_SET_AUX_LINE_IN:
        snd_set_aux_line_in_req(cmd_ptr->aux_linein.aux_mode);

        break;

      case SND_SLOWTALK_CTL:
        snd_slowtalk_ctl_req(cmd_ptr->slowtalk_ctl.enable,
                             cmd_ptr->slowtalk_ctl.callback_ptr,
                             cmd_ptr->slowtalk_ctl.client_data);
        break;
 
      case SND_SLOWTALK_PARAMS:
        snd_set_slowtalk_params_req(&cmd_ptr->slowtalk_params.params,
                                    cmd_ptr->slowtalk_params.callback_ptr,
                                    cmd_ptr->slowtalk_params.client_data);
        break;

        /* Callback function for PA wakeup which posts the command to SND queue 
       */
      case SND_AUDFMT_PA_ON_CB:
        change_mask |= SND_CHANGE_M;
        break;

        /* INVALID COMMAND !!!           */
      case SND_INVALID:
         /* Now Lint can see we have not forgotten this enum value.
         ** Fall Thru to "default".
         */
      /* ILLEGAL COMMAND !!!           */
      default:
        ERR_FATAL("Illegal command %x", cmd_ptr->hdr.cmd, 0, 0);
        /* Does not return */
    }

    snd_finish_cmd( cmd_ptr );

  } /* end of while "commands to process" */

  /* Return a mask of changes which have occurred.
  */
  return( change_mask );

} /* end of snd_req */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_register_aux_ctl

DESCRIPTION
  Register a function to be called by sound to control an auxiliary
  generator such as a vibrator alert.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_register_aux_ctl
(
  snd_aux_func_ptr_type aux_ctl_fn_ptr
    /* Pointer to the function to control an auxiliary generator.*/
)
{
  sndhw_register_aux_ctl(aux_ctl_fn_ptr);
} /* end of snd_register_aux_ctl */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_device

DESCRIPTION
  Set device and muting for the voice path, or control muting for the
  current device

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_set_device
(
  snd_device_type       device,
    /* The New device Type the caller wants */
  snd_mute_control_type ear_mute,
    /* Mute, unmute output                  */
  snd_mute_control_type mic_mute,
    /* Mute, unmute microphone              */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> dev.hdr.cmd          = SND_SET_DEVICE;
    packet -> dev.hdr.callback_ptr = callback_ptr;
    packet -> dev.hdr.client_data  = client_data;
    packet -> dev.device           = device;
    packet -> dev.in_mute_s        = mic_mute;
    packet -> dev.out_mute_s       = ear_mute;
    snd_cmd(packet);
    MSG_HIGH("snd_set_device: device=%x, ear_mute=%x, mic_mute=%x", 
              device, ear_mute, mic_mute);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }

} /* end of snd_set_device */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_volume

DESCRIPTION
  Set volume for a device x method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_set_volume
(
  snd_device_type       device,
    /* The New device Type the caller wants */
  snd_method_type       method,
    /* method                               */
  uint8                 volume,
    /* volume                               */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> vol.hdr.cmd          = SND_SET_VOLUME;
    packet -> vol.hdr.callback_ptr = callback_ptr;
    packet -> vol.hdr.client_data  = client_data;
    packet -> vol.device           = device;
    packet -> vol.method           = method;
    packet -> vol.volume           = volume;
    snd_cmd(packet);
    MSG_HIGH("snd_set_volume: device=%x, method=%x, vol=%x", 
              device, method, volume);    
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_set_volume */
/* <EJECT> */

/*===========================================================================

FUNCTION snd_get_volume

DESCRIPTION
  Get volume for a device x method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_get_volume
(
  snd_device_type          device,
    /* The New device Type the caller wants */
  snd_method_type          method,
    /* method                               */
  snd_vol_cb_func_ptr_type callback_ptr,
    /* Call back function for volume        */
  const void               *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */
  
  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> get_vol.hdr.cmd          = SND_GET_VOLUME;
    packet -> get_vol.hdr.client_data  = client_data;
    packet -> get_vol.device           = device;
    packet -> get_vol.method           = method;
    packet -> get_vol.callback_ptr     = callback_ptr;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE, 0 );
    }
  }
} /* end of snd_get_volume */
/* <EJECT> */

/*===========================================================================

FUNCTION snd_get_levels

DESCRIPTION
  Get volume level for a device x method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_get_levels
(
  snd_device_type          device,
    /* The New device Type the caller wants */
  snd_method_type          method,
    /* method                               */
  snd_lev_cb_func_ptr_type callback_ptr,
    /* Call back function                   */
  const void               *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> level.hdr.cmd          = SND_GET_LEVELS;
    packet -> level.hdr.client_data  = client_data;
    packet -> level.device           = device;
    packet -> level.method           = method;
    packet -> level.callback_ptr     = callback_ptr;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE, 0 );
    }
  }
} /* end of snd_get_levels */
/* <EJECT> */

/*===========================================================================

FUNCTION snd_tone_start

DESCRIPTION
  Start playing a tone using a particular device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_tone_start
(
  snd_device_type       device,
    /* The device chosen for this command   */
  snd_method_type       method,
    /* method                               */
  snd_tone_type         tone_id,
    /* Tone id                              */
  uint16                duration,
    /* Duration of the tone in ms           */
  snd_apath_type        dtmf_path,
    /* Play this tone OTA when in DFM call  */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> tone.hdr.cmd          = SND_TONE_START;
    packet -> tone.hdr.callback_ptr = callback_ptr;
    packet -> tone.hdr.client_data  = client_data;
    packet -> tone.device           = device;
    packet -> tone.method           = method;
    packet -> tone.tone_id          = tone_id;
    packet -> tone.duration_ms      = duration;
    packet -> tone.dtmf_path        = dtmf_path;
    packet -> tone.volume           = SND_VOL_USE_DEFAULT;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_tone_start */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_freq_tone_start

DESCRIPTION
  Start playing a tone specified by a pair of frequencies
  using a particular device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_freq_tone_start
(
  snd_device_type       device,
    /* The device chosen for this command   */
  snd_method_type       method,
    /* method                               */
  uint16                f1_hz,
    /* The higher freq of the DTMF pair     */
  uint16                f2_hz,
    /* The lower freq of the DTMF pair      */
  uint16                duration,
    /* Duration of the tone in ms           */
  snd_apath_type        dtmf_path,
    /* Play this tone OTA when in DFM call  */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> tone.hdr.cmd          = SND_TONE_START;
    packet -> tone.hdr.callback_ptr = callback_ptr;
    packet -> tone.hdr.client_data  = client_data;
    packet -> tone.device           = device;
    packet -> tone.method           = method;
    packet -> tone.tone_id          = SND_FREQ;
    packet -> tone.freq_hi          = f1_hz;
    packet -> tone.freq_lo          = f2_hz;
    packet -> tone.duration_ms      = duration;
    packet -> tone.dtmf_path        = dtmf_path;
    packet -> tone.volume           = SND_VOL_USE_DEFAULT;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_freq_tone_start */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sample_tone_start

DESCRIPTION
  Start playing a tone with the specified volume in the command
  using a particular device and method

DEPENDENCIES
  snd_voice_path_out

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sample_tone_start
(
  snd_device_type       device,
    /* The device chosen for this command   */
  snd_method_type       method,
    /* method                               */
  snd_tone_type         tone_id,
    /* Tone id                              */
  uint16                duration,
    /* Duration of the tone in ms           */
  uint8                 vol_level,
    /* Volume level                         */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> tone.hdr.cmd          = SND_TONE_START;
    packet -> tone.hdr.callback_ptr = callback_ptr;
    packet -> tone.hdr.client_data  = client_data;
    if (device == SND_DEVICE_CURRENT) {
      packet -> tone.device         = snd_voice_path_out.device;
    } else {
      packet -> tone.device         = device;
    }
    packet -> tone.method           = method;
    packet -> tone.tone_id          = tone_id;
    packet -> tone.duration_ms      = duration;
    packet -> tone.dtmf_path        = SND_APATH_LOCAL;
#ifdef SND_OLD_VOLUME_UNITS
    if(vol_level >=  0x1f) {
      vol_level = SND_UI_VOL_TO_INDEX(vol_level);
      vol_level++;
    }
#endif
    packet -> tone.volume           = vol_level;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_sample_tone_start */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_tone_stop

DESCRIPTION
  Stop playing the current tone

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_tone_stop
(
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> tone.hdr.cmd          = SND_TONE_STOP;
    packet -> tone.hdr.callback_ptr = callback_ptr;
    packet -> tone.hdr.client_data  = client_data;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_tone_stop */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_id_start

DESCRIPTION
  Start playing a sound from the database using a particular device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_id_start
(
  snd_device_type       device,
   /* The device chosen for this command    */
  snd_method_type       method,
    /* method                               */
  snd_sound_id_type     sound_id,
    /* Sound id                             */
  snd_priority_type     priority,
    /* Priority of the tone                 */
  snd_apath_type        dtmf_path,
    /* Play this tone OTA when in DFM call  */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_id.hdr.cmd          = SND_SOUND_ID_START;
    packet -> sound_id.hdr.callback_ptr = callback_ptr;
    packet -> sound_id.hdr.client_data  = client_data;
    packet -> sound_id.device           = device;
    packet -> sound_id.method           = method;
    packet -> sound_id.sound_id         = sound_id;
    packet -> sound_id.priority         = priority;
    packet -> sound_id.dtmf_path        = dtmf_path;
    packet -> sound_id.volume           = SND_VOL_USE_DEFAULT;
    packet -> sound_id.repeat_cnt       = 0;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_sound_id_start */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sample_id_sound_start

DESCRIPTION
  Start playing a sound from the database with the specified volume 
  in the command using a particular device and method

DEPENDENCIES
  snd_voice_path_out

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sample_sound_id_start
(
  snd_device_type       device,
   /* The device chosen for this command    */
  snd_method_type       method,
    /* method                               */
  snd_sound_id_type     sound_id,
    /* Sound id                             */
  uint8                 volume,
    /* Volume to play this sound            */
  uint16                repeat_cnt,
    /* Nbr of times to repeat before stop   */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_id.hdr.cmd          = SND_SOUND_ID_START;
    packet -> sound_id.hdr.callback_ptr = callback_ptr;
    packet -> sound_id.hdr.client_data  = client_data;
    if (device == SND_DEVICE_CURRENT) {
      packet -> sound_id.device         = snd_voice_path_out.device;
    } else {
      packet -> sound_id.device         = device;
    }
    packet -> sound_id.method           = method;
    packet -> sound_id.sound_id         = sound_id;
    packet -> sound_id.priority         = SND_PRIO_LOW;
    packet -> sound_id.dtmf_path        = SND_APATH_LOCAL;
#ifdef SND_OLD_VOLUME_UNITS
    if(volume >=  0x1f) {
      volume = SND_UI_VOL_TO_INDEX(volume);
      volume++;
    }
#endif
    packet -> sound_id.volume           = volume;
    packet -> sound_id.repeat_cnt       = repeat_cnt;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_sample_sound_id_start */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_start

DESCRIPTION
  Start playing a sound from the list of tones using a particular 
  device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_start
(
  snd_device_type       device,
   /* The device chosen for this command    */
  snd_method_type       method,
    /* method                               */
  const snd_sound_type  *sound_ptr,
    /* list of tones                        */
  snd_priority_type     priority,
    /* Priority of the tone                 */
  snd_apath_type        dtmf_path,
    /* Play this tone OTA when in DFM call  */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */
  snd_sound_type   *sound_buffer;


  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
#ifdef FEATURE_REMOTE_SOUND
#error code not present
#else
      sound_buffer                     = (snd_sound_type *) sound_ptr;
#endif
      packet -> sound.hdr.cmd          = SND_SOUND_START;
      packet -> sound.hdr.callback_ptr = callback_ptr;
      packet -> sound.hdr.client_data  = client_data;
      packet -> sound.device           = device;
      packet -> sound.method           = method;
      packet -> sound.sound_ptr        = sound_buffer;
      packet -> sound.priority         = priority;
      packet -> sound.dtmf_path        = dtmf_path;
      packet -> sound.volume           = SND_VOL_USE_DEFAULT;
      packet -> sound.repeat_cnt       = 0;
      snd_cmd(packet);
#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_sound_start */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sample_sound_start

DESCRIPTION
  Start playing a sound from the list of tones with the specified volume 
  in the command using a particular device and method

DEPENDENCIES
  snd_voice_path_out

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sample_sound_start
(
  snd_device_type       device,
   /* The device chosen for this command    */
  snd_method_type       method,
    /* method                               */
  const snd_sound_type  *sound_ptr,
    /* Tone id                              */
  uint8                 volume,
    /* Volume to play this sound            */
  uint16                repeat_cnt,
    /* Nbr of times to repeat before stop   */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */
  snd_sound_type   *sound_buffer;

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
#ifdef FEATURE_REMOTE_SOUND
#error code not present
#else
      sound_buffer                     = (snd_sound_type *) sound_ptr;
#endif
      packet -> sound.hdr.cmd          = SND_SOUND_START;
      packet -> sound.hdr.callback_ptr = callback_ptr;
      packet -> sound.hdr.client_data  = client_data;
      if (device == SND_DEVICE_CURRENT) {
        packet -> sound.device         = snd_voice_path_out.device;
      } else {
        packet -> sound.device         = device;
      }
      packet -> sound.method           = method;
      packet -> sound.sound_ptr        = sound_buffer;
      packet -> sound.priority         = SND_PRIO_LOW;
      packet -> sound.dtmf_path        = SND_APATH_LOCAL;
#ifdef SND_OLD_VOLUME_UNITS
      if(volume >=  0x1f) {
        volume = SND_UI_VOL_TO_INDEX(volume);
        volume++;
      }
#endif
      packet -> sound.volume           = volume;
      packet -> sound.repeat_cnt       = repeat_cnt;
      snd_cmd(packet);
#ifdef FEATURE_REMOTE_SOUND
#error code not present
#endif
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_sample_sound_start */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_stop

DESCRIPTION
  Stop playing the current sound

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_stop
(
  snd_priority_type     priority,
    /* Priority of the tone                 */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> snd_stop.hdr.cmd          = SND_SOUND_STOP;
    packet -> snd_stop.hdr.callback_ptr = callback_ptr;
    packet -> snd_stop.hdr.client_data  = client_data;
    packet -> snd_stop.priority         = priority;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_sound_stop */

/* <EJECT> */
#ifdef FEATURE_OEMSBL_AUDIBLE_TONE
/*===========================================================================

FUNCTION snd_set_oemsbl_cb

DESCRIPTION
  This function is called by OEMSBL code to set a call back. This callback
  will be used by vocoder drivers to let OEMSBL know when the DSP is shutdown. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_set_oemsbl_cb(snd_oemsbl_cb_func_ptr_type func_ptr)
{
  sndhw_set_oemsbl_cb(func_ptr);
}
#endif
/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION snd_vp_start

DESCRIPTION
  Start playing a voice prompt using a particular device

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_vp_start
(
  snd_device_type       device,
    /* The device chosen for this command   */
  voc_pb_header_type    *vp_ptr,
    /* Voice prompt to be played            */ 
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)

{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> vp.hdr.cmd          = SND_VOICE_PROMPT_PLAY;
    packet -> vp.hdr.callback_ptr = callback_ptr;
    packet -> vp.hdr.client_data  = client_data;
    packet -> vp.device           = device;
    packet -> vp.vp               = vp_ptr;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_vp_start */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_vp_stop

DESCRIPTION
  Stop playing the current voice prompt

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_vp_stop
(
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> vp.hdr.cmd          = SND_VOICE_PROMPT_STOP;
    packet -> vp.hdr.callback_ptr = callback_ptr;
    packet -> vp.hdr.client_data  = client_data;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_vp_stop */
#endif /* FEATURE_VOICE_PLAYBACK */
/* <EJECT> */
#ifdef FEATURE_VOICE_MEMO
/*===========================================================================

FUNCTION snd_vp_pause

DESCRIPTION
  Pause in-progress playback

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_vp_pause
(
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> vp_control.hdr.cmd          = SND_VOICE_PAUSE_RESUME;
    packet -> vp_control.hdr.callback_ptr = callback_ptr;
    packet -> vp_control.hdr.client_data  = client_data;
    packet -> vp_control.play_record      = SND_VP_PLAY_PAUSE;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_vp_pause */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_vp_resume

DESCRIPTION
  Resume paused playback

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_vp_resume
(
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> vp_control.hdr.cmd          = SND_VOICE_PAUSE_RESUME;
    packet -> vp_control.hdr.callback_ptr = callback_ptr;
    packet -> vp_control.hdr.client_data  = client_data;
    packet -> vp_control.play_record      = SND_VP_PLAY_RESUME;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_vp_resume */
#endif /* FEATURE_VOICE_MEMO */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_get_time

DESCRIPTION
  Calculate the playback time of a sound if possible.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_get_time (
  snd_sound_type             *sound_ptr,
  snd_time_cb_func_ptr_type  callback_ptr,
  const void                 *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_get_time.hdr.cmd          = SND_SOUND_GET_TIME;
    packet -> sound_get_time.sound_ptr        = sound_ptr;
    packet -> sound_get_time.cb_func          = callback_ptr;
    packet -> sound_get_time.hdr.client_data  = client_data;
    packet -> sound_get_time.hdr.callback_ptr = NULL;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE, 0 );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_get_specs

DESCRIPTION
  Get the audio format type of a sound if possible.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_get_specs (
  snd_sound_type            *sound_ptr,
  snd_spec_cb_func_ptr_type callback_ptr,
  const void                *client_data
) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_get_spec.hdr.cmd          = SND_SOUND_GET_SPEC;
    packet -> sound_get_spec.sound_ptr        = sound_ptr;
    packet -> sound_get_spec.cb_func          = callback_ptr;
    packet -> sound_get_spec.hdr.client_data  = client_data;
    packet -> sound_get_spec.hdr.callback_ptr = NULL;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION snd_sound_flush

DESCRIPTION
  Flush buffers and restart playing

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_flush (
  snd_priority_type     priority,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_packets_type *packet;

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_flow.hdr.cmd          = SND_SOUND_FLUSH;
    packet -> sound_flow.hdr.callback_ptr = callback_ptr;
    packet -> sound_flow.hdr.client_data  = client_data;
    packet -> sound_flow.priority         = priority;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_fforward

DESCRIPTION
  Fast forward sound playback if possible

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_fforward (
  snd_priority_type     priority,
  uint32                fforward_ms,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_flow.hdr.cmd          = SND_SOUND_FFORWARD;
    packet -> sound_flow.hdr.callback_ptr = callback_ptr;
    packet -> sound_flow.hdr.client_data  = client_data;
    packet -> sound_flow.priority         = priority;
    packet -> sound_flow.time_ms          = fforward_ms;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_rewind

DESCRIPTION
  Rewind sound playback if possible.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_rewind (
  snd_priority_type     priority,
  uint32                rewind_ms,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_flow.hdr.cmd          = SND_SOUND_REWIND;
    packet -> sound_flow.hdr.callback_ptr = callback_ptr;
    packet -> sound_flow.hdr.client_data  = client_data;
    packet -> sound_flow.priority         = priority;
    packet -> sound_flow.time_ms          = rewind_ms;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_pause

DESCRIPTION
  Pause/resume playback of sound if possible.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_pause (
  snd_priority_type     priority,
  boolean               pause,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_flow.hdr.callback_ptr = callback_ptr;
    packet -> sound_flow.hdr.client_data  = client_data;
    packet -> sound_flow.priority         = priority;
    if(pause) {
      packet -> sound_flow.hdr.cmd        = SND_SOUND_PAUSE;
    } else {
      packet -> sound_flow.hdr.cmd        = SND_SOUND_RESUME;
    }
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_seek_pause

DESCRIPTION
  This function prepares to receive a rewind/fast-forward command by
  halting playback and muting the output.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_seek_pause (
  snd_priority_type     priority,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_flow.hdr.callback_ptr = callback_ptr;
    packet -> sound_flow.hdr.client_data  = client_data;
    packet -> sound_flow.hdr.cmd          = SND_SOUND_SEEK_PAUSE;
    packet -> sound_flow.priority         = priority;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_tempo

DESCRIPTION
  Modify sound playback tempo if possible.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_tempo (
  snd_priority_type     priority,
  uint32                tempo_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_flow.hdr.cmd          = SND_SOUND_TEMPO;
    packet -> sound_flow.hdr.callback_ptr = callback_ptr;
    packet -> sound_flow.hdr.client_data  = client_data;
    packet -> sound_flow.priority         = priority;
    packet -> sound_flow.time_ms          = tempo_factor;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_tune

DESCRIPTION
  Modify sound playback tune if possible.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_tune (
  snd_priority_type     priority,
  uint8                 tune_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_flow.hdr.cmd          = SND_SOUND_TUNE;
    packet -> sound_flow.hdr.callback_ptr = callback_ptr;
    packet -> sound_flow.hdr.client_data  = client_data;
    packet -> sound_flow.priority         = priority;
    packet -> sound_flow.time_ms          = tune_factor;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sound_pan

DESCRIPTION
  Modify sound playback stereo pan if possible.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sound_pan (
  snd_priority_type     priority,
  uint8                 pan_factor,
  snd_cb_func_ptr_type  callback_ptr,
  const void            *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sound_flow.hdr.cmd          = SND_SOUND_PAN;
    packet -> sound_flow.hdr.callback_ptr = callback_ptr;
    packet -> sound_flow.hdr.client_data  = client_data;
    packet -> sound_flow.priority         = priority;
    packet -> sound_flow.time_ms          = (uint32)pan_factor;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_stop

DESCRIPTION
  Stop indicated sequence playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sequence_stop (
  void                 *sequence,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> seq_cmd.hdr.cmd          = SND_AUDFMT_SEQ_STOP;
    packet -> seq_cmd.hdr.callback_ptr = callback_ptr;
    packet -> seq_cmd.hdr.client_data  = client_data;
    packet -> seq_cmd.sequence         = sequence;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_pause

DESCRIPTION
  Pause/resume indicated sequence playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sequence_pause (
  void                 *sequence,
  boolean              pause_flag,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    if(pause_flag) {
      packet -> seq_cmd.hdr.cmd        = SND_AUDFMT_SEQ_PAUSE;
    } else {
      packet -> seq_cmd.hdr.cmd        = SND_AUDFMT_SEQ_RESUME;
    }
    packet -> seq_cmd.hdr.callback_ptr = callback_ptr;
    packet -> seq_cmd.hdr.client_data  = client_data;
    packet -> seq_cmd.sequence         = sequence;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_fforward

DESCRIPTION
  Fast-forward indicated sequence playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sequence_fforward (
  void                 *sequence,
  uint32               ms_forward,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> seq_cmd.hdr.cmd          = SND_AUDFMT_SEQ_FFORWARD;
    packet -> seq_cmd.hdr.callback_ptr = callback_ptr;
    packet -> seq_cmd.hdr.client_data  = client_data;
    packet -> seq_cmd.sequence         = sequence;
    packet -> seq_cmd.num              = ms_forward;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_rewind

DESCRIPTION
  Rewind indicated sequence playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sequence_rewind (
  void                 *sequence,
  uint32               ms_rewind,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> seq_cmd.hdr.cmd          = SND_AUDFMT_SEQ_REWIND;
    packet -> seq_cmd.hdr.callback_ptr = callback_ptr;
    packet -> seq_cmd.hdr.client_data  = client_data;
    packet -> seq_cmd.sequence         = sequence;
    packet -> seq_cmd.num              = ms_rewind;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_set_tempo

DESCRIPTION
  Set tempo for indicated sequence playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sequence_set_tempo (
  void                 *sequence,
  uint32               tempo_factor,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> seq_cmd.hdr.cmd          = SND_AUDFMT_SEQ_SET_TEMPO;
    packet -> seq_cmd.hdr.callback_ptr = callback_ptr;
    packet -> seq_cmd.hdr.client_data  = client_data;
    packet -> seq_cmd.sequence         = sequence;
    packet -> seq_cmd.num              = tempo_factor;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_set_tune

DESCRIPTION
  Set tune for indicated sequence playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sequence_set_tune (
  void                 *sequence,
  uint32               tune_factor,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> seq_cmd.hdr.cmd          = SND_AUDFMT_SEQ_SET_TUNE;
    packet -> seq_cmd.hdr.callback_ptr = callback_ptr;
    packet -> seq_cmd.hdr.client_data  = client_data;
    packet -> seq_cmd.sequence         = sequence;
    packet -> seq_cmd.num              = tune_factor;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_set_pan

DESCRIPTION
  Set pan for indicated sequence playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sequence_set_pan (
  void                 *sequence,
  uint16               pan_factor,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> seq_cmd.hdr.cmd          = SND_AUDFMT_SEQ_SET_PAN;
    packet -> seq_cmd.hdr.callback_ptr = callback_ptr;
    packet -> seq_cmd.hdr.client_data  = client_data;
    packet -> seq_cmd.sequence         = sequence;
    packet -> seq_cmd.num              = pan_factor;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sequence_set_vol

DESCRIPTION
  Set vol for indicated sequence playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sequence_set_vol (
  void                 *sequence,
  uint16               vol_factor,
  snd_cb_func_ptr_type callback_ptr,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> seq_cmd.hdr.cmd          = SND_AUDFMT_SEQ_SET_VOL;
    packet -> seq_cmd.hdr.callback_ptr = callback_ptr;
    packet -> seq_cmd.hdr.client_data  = client_data;
    packet -> seq_cmd.sequence         = sequence;
    packet -> seq_cmd.num              = vol_factor;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_get_time_abort

DESCRIPTION
  Abort a midi/WebAudio get time command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_audfmt_get_time_abort (
  snd_cb_func_ptr_type         callback_ptr,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd          = SND_AUDFMT_GET_TIME_ABORT;
    packet -> hdr.callback_ptr = callback_ptr;
    packet -> hdr.client_data  = client_data;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_new_data

DESCRIPTION
  This function receives new data for the audio format parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_audfmt_new_data (
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 position
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->cmx_af_read_cb.hdr.cmd  = SND_AUDFMT_READ_CALLBACK;
    packet->cmx_af_read_cb.data     = data;
    packet->cmx_af_read_cb.buf      = buf;
    packet->cmx_af_read_cb.length   = length;
    packet->cmx_af_read_cb.pos      = position;
    snd_cmd(packet);
  }
}

/* <EJECT> */
#ifdef FEATURE_DLS
/*===========================================================================

FUNCTION snd_audfmt_new_dls_data

DESCRIPTION
  This function receives new data for the DLS parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_audfmt_new_dls_data (
  void   *data,
  uint8  *buf,
  uint32 length,
  uint32 position
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->cmx_af_read_cb.hdr.cmd     = SND_AUDFMT_DLS_READ_CALLBACK;
    packet->cmx_af_read_cb.client_data = data;
    packet->cmx_af_read_cb.buf         = buf;
    packet->cmx_af_read_cb.length      = length;
    packet->cmx_af_read_cb.pos         = position;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_load_dls

DESCRIPTION
  This function loads a DLS file to be used by all MIDI-based sequences.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_audfmt_load_dls (
  snd_af_access_type *handle,
  uint32             byte_offset
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->audfmt_dls.hdr.cmd       = SND_AUDFMT_DLS_LOAD;
    packet->audfmt_dls.audfmt_handle = handle;
    packet->audfmt_dls.byte_offset   = byte_offset;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_unload_dls

DESCRIPTION
  This function unloads all DLS resources allocated by snd_audfmt_load_dls().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_audfmt_unload_dls (
  snd_af_access_type *handle
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->audfmt_dls.hdr.cmd       = SND_AUDFMT_DLS_UNLOAD;
    packet->audfmt_dls.audfmt_handle = handle;
    snd_cmd(packet);
  }
}
#endif /* FEATURE_DLS */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_preparse_done

DESCRIPTION
  This function issues a command to sound to indicate that the audio format
  file has been parsed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_audfmt_preparse_done ( void *client_data ) 
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd               = SND_AUDFMT_PREPARSE_DONE;
    packet->preparse.client_data  = client_data;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_header_data

DESCRIPTION
  This function receives new header data for the audio format parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_audfmt_header_data (
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 position
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->cmx_af_read_cb.hdr.cmd  = SND_AUDFMT_HEADER_CALLBACK;
    packet->cmx_af_read_cb.data     = data;
    packet->cmx_af_read_cb.buf      = buf;
    packet->cmx_af_read_cb.length   = length;
    packet->cmx_af_read_cb.pos      = position;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_audfmt_dls_data

DESCRIPTION
  This function receives DLS data for the audio format parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_audfmt_dls_data (
  void   *client_data,
  uint8  *data,
  uint8  *buf,
  uint32 length
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->cmx_af_dls_cb.hdr.cmd     = SND_AUDFMT_DLS_CALLBACK;
    packet->cmx_af_dls_cb.data        = data;
    packet->cmx_af_dls_cb.buf         = buf;
    packet->cmx_af_dls_cb.length      = length;
    packet->cmx_af_dls_cb.client_data = client_data;
    snd_cmd(packet);
  }
}

/* <EJECT> */
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION snd_audfmt_check_reset

DESCRIPTION
  This function issues a check reset command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_audfmt_check_reset ( void )
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd = SND_AUDFMT_CHECK_RESET;
    snd_cmd(packet);
  }
}
#endif /* FEATURE_MIDI */

/* <EJECT> */
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/*===========================================================================

FUNCTION snd_audfmt_adec_int

DESCRIPTION
  This function requests new data for the audio format parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_audfmt_adec_int (
  void *data_ptr
) {
  /* Pointer to Request Packet from Packet Pool */
  snd_packets_type *packet;
  
  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->af_adec_int.hdr.cmd          = SND_AUDFMT_ADEC_INT;
    packet->af_adec_int.data_ptr         = data_ptr;
    snd_cmd(packet);
  }
}
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION snd_qcp_get_func

DESCRIPTION
  Gets the indicated information from a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_qcp_get_func (
  snd_qcp_func_enum_type func,
  snd_af_access_type     *handle
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->qcp_get_func.hdr.cmd     = SND_QCP_GET_FUNC;
    packet->qcp_get_func.func        = func;
    packet->qcp_get_func.qcp_handle  = handle;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_set_func

DESCRIPTION
  Sets the indicated information in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_qcp_set_func (
  snd_qcp_func_enum_type       func,
  uint8                        *buffer,
  uint32                       num_bytes,
  snd_af_access_type           *handle,
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->qcp_set_func.hdr.cmd     = SND_QCP_SET_FUNC;
    packet->qcp_set_func.func        = func;
    packet->qcp_set_func.buffer      = buffer;
    packet->qcp_set_func.num_bytes   = num_bytes;
    packet->qcp_set_func.handle      = handle;
    packet->qcp_set_func.cb_func     = cb_func;
    packet->qcp_set_func.client_data = client_data;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_record

DESCRIPTION
  Starts a QCP recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_qcp_record (
  snd_qcp_format_type          format,
  snd_qcp_dir_type             link,
  snd_mm_qcp_rec_para_type     *snd_rec_param,
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->qcp_rec.hdr.cmd          = SND_QCP_RECORD;
    packet->qcp_rec.format           = format;
    packet->qcp_rec.link             = link;
    packet->qcp_rec.rec_qcp_param    = *snd_rec_param;
    packet->qcp_rec.cb_func          = cb_func;
    packet->qcp_rec.client_data      = client_data;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_record_stop

DESCRIPTION
  Ends a QCP recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_qcp_record_stop (
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd          = SND_QCP_RECORD_STOP;
    packet->hdr.callback_ptr = cb_func;
    packet->hdr.client_data  = client_data;
    snd_cmd(packet);
  } else {
    if (cb_func != NULL) {
      cb_func ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_record_pause

DESCRIPTION
  Pauses a QCP recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_qcp_record_pause (
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd          = SND_QCP_RECORD_PAUSE;
    packet->hdr.callback_ptr = cb_func;
    packet->hdr.client_data  = client_data;
    snd_cmd(packet);
  } else {
    if (cb_func != NULL) {
      cb_func ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_record_resume

DESCRIPTION
  Resumes a QCP recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_qcp_record_resume (
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd          = SND_QCP_RECORD_RESUME;
    packet->hdr.callback_ptr = cb_func;
    packet->hdr.client_data  = client_data;
    snd_cmd(packet);
  } else {
    if (cb_func != NULL) {
      cb_func ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_qcp_rec_new_buffer

DESCRIPTION
  This function receives new buffer for QCP recording in command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_qcp_rec_new_buffer (
  uint8  *buf,
  uint32 num_bytes
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->qcp_buffer.hdr.cmd   = SND_QCP_RECORD_BUF;
    packet->qcp_buffer.buffer    = buf;
    packet->qcp_buffer.num_bytes = num_bytes;
    snd_cmd(packet);
  } else {
    MSG_ERROR("Lost recording buffer",0,0,0);
  }
}
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION snd_pcm_record

DESCRIPTION
  Starts a PCM recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_pcm_record (
  snd_pcm_rec_dir_type         link,
  snd_pcm_rec_cmd_struct_type  *spec,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->pcm_rec.hdr.cmd       = SND_PCM_RECORD;
    packet->pcm_rec.link          = link;
    packet->pcm_rec.buf1          = spec->buffer1;
    packet->pcm_rec.num_buf1      = spec->num_buf1;
    packet->pcm_rec.buf2          = spec->buffer2;
    packet->pcm_rec.num_buf2      = spec->num_buf2;
    packet->pcm_rec.cb_func       = cb_func;
    packet->pcm_rec.client_data   = client_data;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pcm_record_stop

DESCRIPTION
  Ends a PCM recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_pcm_record_stop (
  snd_pcm_rec_dir_type link,
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd          = SND_PCM_RECORD_STOP;
    packet->hdr.callback_ptr = cb_func;
    packet->hdr.client_data  = client_data;
    packet->pcm_rec_cmd.link = link;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pcm_record_pause

DESCRIPTION
  Pauses a PCM recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_pcm_record_pause (
  snd_pcm_rec_dir_type link,
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd          = SND_PCM_RECORD_PAUSE;
    packet->hdr.callback_ptr = cb_func;
    packet->hdr.client_data  = client_data;
    packet->pcm_rec_cmd.link = link;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pcm_record_resume

DESCRIPTION
  Resumes a PCM recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_pcm_record_resume (
  snd_pcm_rec_dir_type link,
  snd_cb_func_ptr_type cb_func,
  const void           *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd          = SND_PCM_RECORD_RESUME;
    packet->hdr.callback_ptr = cb_func;
    packet->hdr.client_data  = client_data;
    packet->pcm_rec_cmd.link = link;
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pcm_rec_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_pcm_rec_status (
  snd_af_data_cmd_enum         status,
  snd_pcm_rec_data_struct_type *data,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd                    = SND_PCM_RECORD_STATUS;
    packet->pcm_rec_status.status      = status;
    packet->pcm_rec_status.cb_func     = cb_func;
    packet->pcm_rec_status.client_data = client_data;
    if (data == NULL) {
      packet->pcm_rec_status.data.buf_cb_func = NULL;
      packet->pcm_rec_status.data.client_data = NULL;
      packet->pcm_rec_status.data.rec_data_buf = NULL;
      packet->pcm_rec_status.data.rec_data_num = 0;
      packet->pcm_rec_status.data.rec_data_pos = 0;
    } else {
      packet->pcm_rec_status.data        = *data;
    }
    snd_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_pcm_record_buf_cb

DESCRIPTION
  This function is used as the callback function for PCM recording
  buffer requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_pcm_record_buf_cb (
  uint8            *buffer,
  uint32           num_bytes,
  const void       *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->hdr.cmd                 = SND_PCM_RECORD_BUF;
    packet->pcm_rec_buf.buffer      = buffer;
    packet->pcm_rec_buf.num_bytes   = num_bytes;
    packet->pcm_rec_buf.client_data = client_data;
    snd_cmd(packet);
  }
}
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION snd_mm_rec_new_buffer

DESCRIPTION
  This function receives new buffer from CMX for multimedia recording 
  in command queue.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_mm_rec_new_buffer (
  uint8                  *buf,
  uint32                 num_bytes
) 
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */

  if (packet != NULL) {

    packet->mm_rec_buf.hdr.cmd    = SND_MM_RECORD_BUF;
    packet->mm_rec_buf.buffer     = buf;
    packet->mm_rec_buf.num_bytes  = num_bytes;
    packet->mm_rec_buf.rec_format = snd_mm_rec_ctrl.format;

    snd_cmd(packet);

  } else {
    MSG_ERROR("Lost recording buffer",0,0,0);
  }
} /* void snd_mm_rec_new_buffer */

/*===========================================================================

FUNCTION snd_mm_record

DESCRIPTION
  Starts a MulteMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_mm_record (
  snd_mm_rec_dir_type          dir,
  snd_mm_rec_aud_type          *rec_param,
  snd_mm_rec_cmd_struct_type   *mm_spec,
  snd_mm_rec_cb_func_ptr_type  cb_func,
  const void                   *client_data
)
{
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */

  if (packet != NULL) {

    packet->mm_rec.hdr.cmd     = SND_MM_RECORD;
    packet->mm_rec.dir         = dir;
    packet->mm_rec.rec_param   = *rec_param;
    packet->mm_rec.buf         = *mm_spec;
    packet->mm_rec.cb_func     = cb_func;
    packet->mm_rec.client_data = client_data;

    snd_cmd(packet);
  }
} /* snd_mm_record */

/*===========================================================================

FUNCTION snd_mm_record_stop

DESCRIPTION
  Ends a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_mm_record_stop (
  snd_mm_rec_dir_type    link, 
  snd_cb_func_ptr_type   cb_func,
  const void             *client_data
) 
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */

  if (packet != NULL) {

    packet->hdr.cmd           = SND_MM_RECORD_STOP;
    packet->hdr.callback_ptr  = cb_func;
    packet->hdr.client_data   = client_data;
    packet->mm_rec_cmd.link   = link;

    snd_cmd(packet);
  }
} /* snd_mm_record_stop */

/*===========================================================================

FUNCTION snd_mm_record_pause

DESCRIPTION
  Pauses a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_mm_record_pause (
  snd_mm_rec_dir_type    link, 
  snd_cb_func_ptr_type   cb_func,
  const void             *client_data
) 
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */

  if (packet != NULL) {

    packet->hdr.cmd           = SND_MM_RECORD_PAUSE;

    packet->hdr.callback_ptr  = cb_func;
    packet->hdr.client_data   = client_data;
    packet->mm_rec_cmd.link   = link;

    snd_cmd(packet);
  }
} /* snd_mm_record_pause */

/*===========================================================================

FUNCTION snd_mm_record_resume

DESCRIPTION
  Resumes a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_mm_record_resume (
  snd_mm_rec_dir_type    link, 
  snd_cb_func_ptr_type   cb_func,
  const void             *client_data
) 
{
  snd_packets_type *packet;

    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */

  if (packet != NULL) {

    packet->hdr.cmd           = SND_MM_RECORD_RESUME;
    packet->hdr.callback_ptr  = cb_func;
    packet->hdr.client_data   = client_data;
    packet->mm_rec_cmd.link   = link;

    snd_cmd(packet);
  }
} /* snd_mm_record_resume */

/*===========================================================================

FUNCTION snd_mm_rec_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_mm_rec_status (
  snd_af_data_cmd_enum         status,
  snd_mm_rec_data_struct_type  *data,
  snd_mm_rec_cb_func_ptr_type  cb_func,
  const void                   *client_data
) 
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */
	
  snd_mm_rec_data_struct_type dummy_data = {0};
  /* A dummy variable to assign packet->mm_rec_status.data
     to zero bitwise only if parameter data is passed as
     NULL (which is a valid scenario). This eliminates a 
     crash due to NULL pointer access.
  */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */

  if (packet != NULL) {

    packet->mm_rec_status.hdr.cmd     = SND_MM_RECORD_STATUS;
    packet->mm_rec_status.status      = status;
    packet->mm_rec_status.cb_func     = cb_func;
    packet->mm_rec_status.client_data = client_data;
    if(data != NULL)	
    	{
               packet->mm_rec_status.data        =  *data;
    	}
    else
	{
	       packet->mm_rec_status.data  = dummy_data;
	}
    packet->mm_rec_status.format      = snd_mm_rec_ctrl.format;

    snd_cmd(packet);
  }
} /* snd_mm_rec_status */
#endif /* FEATURE_MM_REC */

/*===========================================================================

FUNCTION snd_aux_pcm_clk_enable

DESCRIPTION
  This function will enable (external)auxpcm clock.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
extern void snd_aux_pcm_clk_enable
(
  snd_cb_func_ptr_type     callback_ptr
    /* Call back function                     */
)
{
	 snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> aux_pcm_clk_ctl.hdr.cmd      = SND_AUX_PCM_CLK_ENABLE;
    packet -> aux_pcm_clk_ctl.callback_ptr = callback_ptr;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( NULL, SND_FAILURE );
    }
  }
}
/*===========================================================================

FUNCTION snd_aux_pcm_clk_disable

DESCRIPTION
  This function will disable (external)auxpcm clock.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
extern void snd_aux_pcm_clk_disable
(
  snd_cb_func_ptr_type     callback_ptr
    /* Call back function                     */
)
{
	 snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> aux_pcm_clk_ctl.hdr.cmd      = SND_AUX_PCM_CLK_DISABLE;
    packet -> aux_pcm_clk_ctl.callback_ptr = callback_ptr;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( NULL, SND_FAILURE );
    }
  }
}

/* <EJECT> */
#ifdef FEATURE_AUDFMT_EQUALIZER
/*===========================================================================

FUNCTION snd_equalizer_ctl

DESCRIPTION
  This function will enable/disable equalizer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

===========================================================================*/
extern void snd_equalizer_ctl
(
  boolean                  enable,
    /* Enable/Disable equalizer               */
  snd_cb_func_ptr_type     callback_ptr,
    /* Call back function                     */
  const void               *client_data
    /* Pointer to Client data                 */
)
{
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> eq_ctl.hdr.cmd      = SND_EQUALIZER_CTL;
    packet -> eq_ctl.callback_ptr = callback_ptr;
    packet -> eq_ctl.client_data  = client_data;
    packet -> eq_ctl.enable       = enable;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }  
}
  
/*===========================================================================

FUNCTION snd_set_equalizer

DESCRIPTION
  This function will setup the equalizer filters table. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

===========================================================================*/
extern void snd_set_equalizer
(
  const snd_equalizer_filter_type *equalizer,
    /* Equalizer filter                       */
  uint32                          bands,
    /* Number of bands                        */
  snd_cb_func_ptr_type            callback_ptr,
    /* Call back function                     */
  const void                      *client_data
    /* Pointer to Client data                 */
)
{
  snd_packets_type          *packet;
  /* Pointer to Request Packet from Packet Pool */
  snd_equalizer_filter_type *eq_ptr;
  /* Pointer to internal equalizer buffer */

  /* Check the number of supported bands 
  */
  if (bands <= VOC_EQ_BANDS_NUM) {

    /* Get a Sound Packet and queue the Request.
    */
    packet = snd_get_packet();    /* Already Logs Error if Fails */

    if (packet != NULL) {
      eq_ptr = snd_get_equalizer_buffer();

      if (eq_ptr != NULL) {
        memcpy( eq_ptr, equalizer, 
                sizeof(snd_equalizer_filter_type) * bands);
        packet -> eq_set.hdr.cmd      = SND_EQUALIZER_SET_FUNC;
        packet -> eq_set.callback_ptr = callback_ptr;
        packet -> eq_set.client_data  = client_data;
        packet -> eq_set.filter       = eq_ptr;
        packet -> eq_set.num          = bands;
        snd_cmd(packet);
        return;
      } else {
        q_put( &snd_free_q, (q_link_type *) packet );
      } 
    }
  }

  if (callback_ptr != NULL) {
    callback_ptr ( client_data, SND_FAILURE );
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_equalizer_gain_ctl

DESCRIPTION
  This function will setup the gain on the specified band.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

===========================================================================*/
extern void snd_equalizer_gain_ctl 
(
  uint16                band,
    /* The band number                      */
  int16                 gain,
    /* The new gain value                   */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> eq_gain.hdr.cmd      = SND_EQUALIZER_GAIN;
    packet -> eq_gain.callback_ptr = callback_ptr;
    packet -> eq_gain.client_data  = client_data;
    packet -> eq_gain.band         = band;
    packet -> eq_gain.gain         = gain;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/*===========================================================================

FUNCTION snd_equalizer_get_bands

DESCRIPTION
  This function will get the available equalizer bands in the system.

DEPENDENCIES
  None

RETURN VALUE
  The total number of equalizer bands.

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
extern uint32 snd_equalizer_get_bands ( void )
{
  return (VOC_EQ_BANDS_NUM);
}
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/*===========================================================================

FUNCTION snd_spectrum_analyzer_ctl

DESCRIPTION
  Allows the user to register a callback to receive spectrum analyzer data.
  If the callback is set to NULL, then the spectrum analyzer is disabled.

DEPENDENCIES
  Even if a non-NULL callback is registered, the callback will not be invoked
  if the currently running DSP firmware image does not support the spectrum
  analyzer function.

  If a non-NULL callback is registered, then a valid buffer must also provided
  in the client_buffer variable

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
void snd_spectrum_analyzer_ctl (
  uint16                         update_period,
    /* How often should the data be analyzed (10ms multiples) */

  snd_spectrum_analyzer_cb_type  sp_cb,
    /* Callback to receive the data */

  snd_cb_func_ptr_type           callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                     *client_data
    /* Pointer to Client provided data */
) {

  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sp_ctl.hdr.cmd       = SND_SPECTRUM_ANALYZER_CTL;
    packet -> sp_ctl.callback_ptr  = callback_ptr;
    packet -> sp_ctl.client_data   = client_data;
    packet -> sp_ctl.update_period = update_period;
    packet -> sp_ctl.cb_func       = sp_cb;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

/* <EJECT> */
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_QCONCERT
/*===========================================================================

FUNCTION snd_qconcert_ctl

DESCRIPTION
  This function will enable/disable the Qconcert 3D audio expander.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_qconcert_ctl (
  boolean               enable,
    /* Flag to enable/disable qconcert */
  snd_cb_func_ptr_type  callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void            *client_data
    /* Pointer to Client provided data */
) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> qconcert_ctl.hdr.cmd      = SND_QCONCERT_CTL;
    packet -> qconcert_ctl.callback_ptr = callback_ptr;
    packet -> qconcert_ctl.client_data  = client_data;
    packet -> qconcert_ctl.enable       = enable;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_qconcert_params

DESCRIPTION
  Configures the parameters for the Qconcert 3D audio expander.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_set_qconcert_params (
  snd_qconcert_params_type *params,
    /* Parameters value for Qconcert effect */
  snd_cb_func_ptr_type     callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */
  const void               *client_data
    /* Pointer to Client provided data */
) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> qconcert_params.hdr.cmd      = SND_QCONCERT_PARAMS;
    packet -> qconcert_params.callback_ptr = callback_ptr;
    packet -> qconcert_params.client_data  = client_data;
    packet -> qconcert_params.params       = *params;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}
#endif /* FEATURE_AUDFMT_QCONCERT */
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_slowtalk_ctl

DESCRIPTION
  Enables/Disable the slowtalk operation
  To enable slowtalk, enable = TRUE (1)
  To disable slowtalk, enable = FALSE (0)

  Arguments callback_ptr and client_data can be NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
void snd_slowtalk_ctl (
  boolean  enable,
    /* Flag to enable/disable slowtalk */
  snd_cb_func_ptr_type  callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */
  const void            *client_data
    /* Pointer to Client provided data */
)
{
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> slowtalk_ctl.hdr.cmd      = SND_SLOWTALK_CTL;
    packet -> slowtalk_ctl.callback_ptr = callback_ptr;
    packet -> slowtalk_ctl.client_data  = client_data;
    packet -> slowtalk_ctl.enable       = enable;
    snd_cmd(packet);
    MSG_HIGH("snd_slowtalk_ctl() enable=0x%x,cb=0x%x,cd=0x%x", enable, 
                                 (uint32)callback_ptr, (uint32)client_data);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_set_slowtalk_params 

DESCRIPTION
  Configures the slowtalk parameters on firmware
  Pass the slowtalk parameters as argument to this function (params).
  Arguments callback_ptr and client_data can be NULL.

DEPENDENCIES
  expansion ratio range should be between 0 to 5

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_set_slowtalk_params (
  snd_slowtalk_params_type *params,
    /* slowtalk control paramters */
  snd_cb_func_ptr_type     callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */
  const void               *client_data
    /* Pointer to Client provided data */
)
{
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> slowtalk_params.hdr.cmd      = SND_SLOWTALK_PARAMS;
    packet -> slowtalk_params.callback_ptr = callback_ptr;
    packet -> slowtalk_params.client_data  = client_data;
    packet -> slowtalk_params.params       = *params;
    snd_cmd(packet);
    MSG_HIGH("snd_set_slowtalk_params() exp_rat=0x%x,cb=0x%x,cd=0x%x", 
                      params->expansion_ratio_level, (uint32)callback_ptr, 
                      (uint32)client_data);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_config 

DESCRIPTION
  This function is used to configure the sbc encoder.

  NOTE: This function is to be used only for testing purposes to evaluate
  SBC encoder functionality with different SBC parameters. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_sbc_config (
  audiosbcenc_data_type *params,
    /* SBC encoded paramters */
  snd_cb_func_ptr_type     cb_func,
    /* Callback to indicate whether this ctl command was accepted */
  const void               *client_data
    /* Pointer to Client provided data */
) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sbc_params.hdr.cmd      = SND_SBCCODEC_PARAMS;
    packet -> sbc_params.callback_ptr = cb_func;
    packet -> sbc_params.client_data  = client_data;
    packet -> sbc_params.params       = *params;
    snd_cmd(packet);
  } else {
    if (cb_func != NULL) {
      cb_func ( client_data, SND_FAILURE );
    }
  }
}

/*===========================================================================

FUNCTION snd_sbc_set_cb

DESCRIPTION
  This function allows the SBC codec to register a callback function to
  receive PCM data from DSP.
  
DEPENDENCIES
  Even if a non-NULL callback is registered, the callback will not be invoked
  if the currently running DSP firmware image does not support the post
  processing function and ARM based SBC codec.

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_sbc_set_cb (
  snd_pp_cb_type                 sbc_cb,
    /* Callback to receive the data */

  snd_cb_func_ptr_type           callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                     *client_data
    /* Pointer to Client provided data */
) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> pp_ctl.hdr.cmd          = SND_SBCCODEC_CTL;
    packet -> pp_ctl.hdr.callback_ptr = callback_ptr;
    packet -> pp_ctl.hdr.client_data  = client_data;
    packet -> pp_ctl.pp_cb_func       = sbc_cb;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_set_bitrate

DESCRIPTION
 This function is called by BT driver to change the bitrate of the SBC
 codec 
DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_sbc_set_bitrate (
  snd_sbc_bitrate_type           bitrate_hint,
  /* Hint for the new bitrate */

  uint32                         bitrate,
  /* New bitrate if the bitrate_hint is SND_SBC_BITRATE_CUSTOM */
  
  snd_cb_func_ptr_type           callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                     *client_data
    /* Pointer to Client provided data */
) {

#ifdef FEATURE_AVS_SBCENC_TASK
  /* If SBC encoder is on lower priority task then we can directly call this
  function as this function will queue the request onto SBCENC task */
  audiosbc_set_bitrate(bitrate_hint, bitrate);

#else
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sbc_bitrate_ctl.hdr.cmd          = SND_SBCCODEC_BITRATE;
    packet -> sbc_bitrate_ctl.hdr.callback_ptr = NULL; 
    packet -> sbc_bitrate_ctl.hdr.client_data  = NULL; 
    packet -> sbc_bitrate_ctl.sbc_bitrate      = bitrate;
    packet -> sbc_bitrate_ctl.bitrate_hint     = bitrate_hint;
    snd_cmd(packet);
  } 
#endif
}

#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)

/* <EJECT> */
/*===========================================================================
FUNCTION snd_sbc_avsync

DESCRIPTION
 This function is called by BT driver to let SND know that a particular  
 frame has been sent by BT to the HS.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
void snd_sbc_avsync (qword frame_num)
{
  audiosbc_avsync(frame_num);
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_avsync_final

DESCRIPTION
 This function is called by Audiosbc to let SND know that some number of
 samples have been sent by BT to HS. This information has to be sent to 
 QTV. 

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/

void snd_sbc_avsync_final (qword num_of_bytes,
                           qword num_of_samples,
                           qword time_stamp)
{
  MSG_LOW("A2DP AV SYNC sending %d %d num of bytes to QTV", \
          qw_hi(num_of_bytes), qw_lo(num_of_bytes),0);
  MSG_LOW("A2DP AV SYNC sending %d %d num of samples to QTV", \
          qw_hi(num_of_samples), qw_lo(num_of_samples),0);
  MSG_LOW("A2DP AV SYNC sending %d %d time stamp to QTV", \
          qw_hi(time_stamp), qw_lo(time_stamp),0);

  snd_av_sync_int_cb_req (time_stamp, num_of_samples, num_of_bytes);
}
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sbc_init

DESCRIPTION
 This function is called by BT driver to initialize the SBC codec 

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_sbc_init (
  snd_sbc_frame_cb_func_ptr_type sbc_cb,
  /* Callback function to be called to send one SBC frame to BT */

  snd_cb_func_ptr_type           callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                     *client_data
    /* Pointer to Client provided data */
) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd              = SND_SBCCODEC_INIT;
    packet -> hdr.callback_ptr     = callback_ptr;
    packet -> hdr.client_data      = client_data;
    packet -> sbc_ctl.sbc_cb       = sbc_cb;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}
#endif /* FEATURE_SBC_CODEC */
#ifdef FEATURE_AVS_I2SSBC
/* <EJECT> */
/*===========================================================================

FUNCTION snd_i2ssbc_enable 

DESCRIPTION
  This function is used to enable I2S SBC interface

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_i2ssbc_enable (

  snd_i2ssbc_config_params_type  *config_params,
  /* I2SSBC interface configuration parameters */

  snd_cb_func_ptr_type           callback_ptr,
  /* Pointer to cb func for receving status about this command */

  const void                     *client_data
  /* Pointer to Client provided data */

) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd                = SND_I2SSBC_ENABLE;
    packet -> hdr.callback_ptr       = callback_ptr;
    packet -> hdr.client_data        = client_data;
    packet -> i2ssbc_ctl.i2ssbc_cb   = config_params->cb_func;
    packet -> i2ssbc_ctl.sample_rate = config_params->sample_rate;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_i2ssbc_disable 

DESCRIPTION
  This function is used to Disable I2S SBC interface

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_i2ssbc_disable (

  snd_cb_func_ptr_type         callback_ptr,
  /* Pointer to cb func for receving status about this command */

  const void                   *client_data
  /* Pointer to Client provided data */

) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd                = SND_I2SSBC_DISABLE;
    packet -> hdr.callback_ptr       = callback_ptr;
    packet -> hdr.client_data        = client_data;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}
#endif /* FEATURE_AVS_I2SSBC */

/* <EJECT> */
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
 /*===========================================================================

FUNCTION snd_pp_ctl

DESCRIPTION
  Allows the user to register a callback to receive post processing data.
  If the callback is set to NULL, then arm post processing is disabled.

DEPENDENCIES
  Even if a non-NULL callback is registered, the callback will not be invoked
  if the currently running DSP firmware image does not support the post
  processing function

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_pp_ctl (
  snd_pp_cb_type  pp_cb,
    /* Callback to receive the data */

  snd_cb_func_ptr_type           callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                     *client_data
    /* Pointer to Client provided data */
) {

  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> pp_ctl.hdr.cmd          = SND_PP_CTL;
    packet -> pp_ctl.hdr.callback_ptr = callback_ptr;
    packet -> pp_ctl.hdr.client_data  = client_data;
    packet -> pp_ctl.pp_cb_func       = pp_cb;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}
#endif /* (FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

#ifdef FEATURE_ARM_POST_PROCESSING 
/*===========================================================================

FUNCTION snd_pp_done

DESCRIPTION
  This is the function called by the client to pass the buffer to the
  DSP after post processing is done. This function will be executed 
  in the context of the caller's task.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_pp_done ( 
  snd_pp_buffer_type             pp_buffer,

  snd_cb_func_ptr_type           callback_ptr,
    /* Callback to indicate whether this ctl command was accepted */

  const void                     *client_data
    /* Pointer to Client provided data */
) {

  snd_status_type snd_status;

  voc_status_type voc_status;

  voc_status = voc_pp_client_data(pp_buffer.pp_buf_length,
                                  pp_buffer.pp_data,
                                  pp_buffer.pp_id);
  if (voc_status == VOC_DONE_S) 
  {
    snd_status = SND_SUCCESS;
  }
  else
  {
    snd_status = SND_FAILURE;
  }

  if (callback_ptr != NULL) 
  {
    callback_ptr(client_data, snd_status);
  }
}
#endif /* FEATURE_ARM_POST_PROCESSING */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT
/*===========================================================================

FUNCTION snd_midi_out_enable

DESCRIPTION
  Enable midi device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_open (
  snd_cb_func_ptr_type           callback_ptr,
    /* Call back function                   */
  snd_qsynth_adec_mode_enum_type adec_mode,
    /* Adec decoder type                    */
  const void                     *client_data
    /* Pointer to Client data               */
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> midi_out_open.hdr.cmd           = SND_MIDI_OUT_OPEN;
    packet -> midi_out_open.hdr.callback_ptr  = callback_ptr;
    packet -> midi_out_open.hdr.client_data   = client_data;
    packet -> midi_out_open.adec_mode         = adec_mode;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_disable

DESCRIPTION
  Disable midi device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_close (
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> midi_out_open.hdr.cmd          = SND_MIDI_OUT_CLOSE;
    packet -> midi_out_open.hdr.callback_ptr = callback_ptr;
    packet -> midi_out_open.hdr.client_data  = client_data;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_msg

DESCRIPTION
  Commands to send to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_msg (
  uint8  *msg_buffer,
  uint32 length
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd             = SND_MIDI_OUT_MSG;
    packet -> midi_out.msg_buffer = msg_buffer;
    packet -> midi_out.length     = length;
    snd_cmd(packet);
  } else {
    if (snd_midi_out_cb != NULL) {
      snd_midi_out_cb ( snd_midi_out_client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION snd_midi_out_qcp

DESCRIPTION
  Playback QCP file to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_qcp (
  uint32                       channel,
  snd_af_access_type           *audfmt_handle,
  snd_midi_out_adec_param_type *param
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd                    = SND_MIDI_OUT_QCP;
    packet -> midi_out_qcp.channel       = channel;
    packet -> midi_out_qcp.audfmt_handle = audfmt_handle;
    packet -> midi_out_qcp.param         = *param;
    snd_cmd(packet);
  } else {
    if (snd_midi_out_cb != NULL) {
      snd_midi_out_cb ( snd_midi_out_client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_adec_start_chan

DESCRIPTION
  Starts adec file to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_adec_start_chan ( uint32 channel )
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd              = SND_MIDI_OUT_ADEC_START_CHAN;
    packet -> midi_out_qcp.channel = channel;
    snd_cmd(packet);
  } else {
    if (snd_midi_out_cb != NULL) {
      snd_midi_out_cb ( snd_midi_out_client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_stop

DESCRIPTION
  Stop QCP file to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_qcp_stop ( uint32 channel )
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd              = SND_MIDI_OUT_QCP_STOP;
    packet -> midi_out_qcp.channel = channel;
    snd_cmd(packet);
  } else {
    if (snd_midi_out_cb != NULL) {
      snd_midi_out_cb ( snd_midi_out_client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_pause

DESCRIPTION
  Pauses QCP files to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_qcp_pause ( void )
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd = SND_MIDI_OUT_QCP_PAUSE;
    snd_cmd(packet);
  } else {
    if (snd_midi_out_cb != NULL) {
      snd_midi_out_cb ( snd_midi_out_client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_resume

DESCRIPTION
  Resumes QCP files to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_qcp_resume ( void )
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd = SND_MIDI_OUT_QCP_RESUME;
    snd_cmd(packet);
  } else {
    if (snd_midi_out_cb != NULL) {
      snd_midi_out_cb ( snd_midi_out_client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_pause_chan

DESCRIPTION
  Pauses QCP file to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_qcp_pause_chan ( uint32 channel )
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd              = SND_MIDI_OUT_QCP_PAUSE_CHAN;
    packet -> midi_out_qcp.channel = channel;
    snd_cmd(packet);
  } else {
    if (snd_midi_out_cb != NULL) {
      snd_midi_out_cb ( snd_midi_out_client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_resume_chan

DESCRIPTION
  Resumes QCP file to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_qcp_resume_chan ( uint32 channel )
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd              = SND_MIDI_OUT_QCP_RESUME_CHAN;
    packet -> midi_out_qcp.channel = channel;
    snd_cmd(packet);
  } else {
    if (snd_midi_out_cb != NULL) {
      snd_midi_out_cb ( snd_midi_out_client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_pan

DESCRIPTION
  Modifies panning of QCP file to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_qcp_pan ( uint32 channel, sint15 pan )
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd                = SND_MIDI_OUT_QCP_PAN;
    packet -> midi_out_qcp.channel   = channel;
    packet -> midi_out_qcp.param.pan = pan;
    snd_cmd(packet);
  } else {
    if (snd_midi_out_cb != NULL) {
      snd_midi_out_cb ( snd_midi_out_client_data, SND_FAILURE );
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_midi_out_qcp_volume

DESCRIPTION
  Modifies volume of a QCP file to MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_midi_out_qcp_volume ( uint32 channel, uint16 volume )
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd                    = SND_MIDI_OUT_QCP_VOL;
    packet -> midi_out_qcp.channel       = channel;
    packet -> midi_out_qcp.param.volume  = volume;
    snd_cmd(packet);
  } else {
    if (snd_midi_out_cb != NULL) {
      snd_midi_out_cb ( snd_midi_out_client_data, SND_FAILURE );
    }
  }
}

#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
/*===========================================================================

FUNCTION snd_3d_pos_enable_req

DESCRIPTION
  This function processes a 3D positional enable command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_enable_req (
  snd_3d_pos_mode_enum_type mode,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
) {
  voc_3d_pos_mode_enum_type voc_mode = VOC_3D_POS_MODE_DISABLE;
  voc_status_type           voc_status;

  if(mode == SND_3D_POS_MODE_ENABLE) {
    voc_mode = VOC_3D_POS_MODE_ENABLE;
  }
  voc_status = voc_3d_pos_enable(voc_mode);

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/*===========================================================================

FUNCTION snd_3d_pos_reset_req

DESCRIPTION
  This function processes an environment 3D reset command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_reset_req (
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
) {
  voc_status_type           voc_status;

  voc_status = voc_3d_pos_reset_env();

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_lsnr_pos_req

DESCRIPTION
  This function processes a 3D positional listener position command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_lsnr_pos_req (
  snd_3d_pos_position_cmd_type *pos,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
) {
  voc_3d_pos_coord_type voc_pos;
  voc_status_type       voc_status = VOC_CMD_FAILED_S;

  if(pos != NULL) {
    voc_pos.x_pos = pos->x_pos;
    voc_pos.y_pos = pos->y_pos;
    voc_pos.z_pos = pos->z_pos;

    voc_status = voc_3d_pos_lsnr_pos(&voc_pos, pos->duration_ms);
  }

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_lsnr_orient_req

DESCRIPTION
  This function processes a 3D positional listener orientation command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_lsnr_orient_req (
  snd_3d_pos_lsnr_orient_cmd_type *orient,
  snd_cb_func_ptr_type            cb_func,
  const void                      *client_data
) {
  voc_3d_pos_orient_type voc_orient;
  voc_status_type        voc_status = VOC_CMD_FAILED_S;

  if(orient != NULL) {
    voc_orient.forward.x_pos = orient->x_forward;
    voc_orient.forward.y_pos = orient->y_forward;
    voc_orient.forward.z_pos = orient->z_forward;
    voc_orient.up.x_pos      = orient->x_up;
    voc_orient.up.y_pos      = orient->y_up;
    voc_orient.up.z_pos      = orient->z_up;

    voc_status = voc_3d_pos_lsnr_orient(&voc_orient, orient->duration_ms);
  }

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION snd_3d_pos_lsnr_vel_req

DESCRIPTION
  This function processes a 3D positional listener velocity command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_lsnr_vel_req (
  snd_3d_pos_velocity_cmd_type *vel,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
){
  voc_3d_pos_coord_type voc_vel;
  voc_status_type       voc_status = VOC_CMD_FAILED_S;

  if(vel != NULL) {
    voc_vel.x_pos = vel->x_vel;
    voc_vel.y_pos = vel->y_vel;
    voc_vel.z_pos = vel->z_vel;

    voc_status = voc_3d_pos_lsnr_vel(&voc_vel, vel->duration_ms);
  }

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_set_req

DESCRIPTION
  This function processes a 3D Reverb set command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_reverb_set_req (
  snd_3d_reverb_enum_type   reverb_type,    /* Reverb Parameter type */
  snd_3d_reverb_cmd_type    *reverb_param,   /* Reverb Parameters     */
  snd_cb_func_ptr_type          cb_func,
  const void                   *client_data
){
  voc_3d_reverb_type       reverb;
  voc_3d_reverb_enum_type  type;
  voc_status_type       voc_status = VOC_CMD_FAILED_S;

  if(reverb_param != NULL) {

    switch( reverb_type){
      case SND_3D_POS_REVERB_SET_PRESET:
        type = VOC_3D_POS_REVERB_SET_PRESET;
        reverb.env_type   = reverb_param->env_type;
        reverb.decay_ms   = reverb_param->decay_ms;
        reverb.decay_ratio= reverb_param->decay_ratio;
        reverb.ref_delay  = reverb_param->ref_delay;
        reverb.reverb_delay = reverb_param->reverb_delay;
        reverb.reverb_gain  = reverb_param->reverb_gain;
        reverb.room_gain    = reverb_param->room_gain;
        break;

      case SND_3D_POS_REVERB_SET_LEVEL:
        type = VOC_3D_POS_REVERB_SET_LEVEL;
        reverb.reverb_gain   = reverb_param->reverb_gain;
        break;

      case SND_3D_POS_REVERB_SET_TIME:
        type = VOC_3D_POS_REVERB_SET_TIME;
        reverb.decay_ms   = reverb_param->decay_ms;
        break;

      case SND_3D_POS_REVERB_SET_RATIO:
        type = VOC_3D_POS_REVERB_SET_RATIO;
        reverb.decay_ratio= reverb_param->decay_ratio;
        break;

      default:
        cb_func( client_data, SND_FAILURE);
        return;
    }

    voc_status = voc_3d_reverb_set(type, &reverb);
  }

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_enable_req

DESCRIPTION
  This function enables/disables 3D Reverb effects.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_reverb_enable_req (
  boolean                      enable,
  snd_cb_func_ptr_type          cb_func,
  const void                   *client_data
){
  voc_status_type  voc_status = voc_3d_reverb_enable( enable);

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_src_enable_req

DESCRIPTION
  This function processes a source 3D positional enable command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_enable_req (
  uint32                    channel,
  snd_3d_pos_mode_enum_type mode,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
) {
  voc_3d_pos_mode_enum_type voc_mode = VOC_3D_POS_MODE_DISABLE;
  voc_status_type           voc_status;

  if(mode == SND_3D_POS_MODE_ENABLE) {
    voc_mode = VOC_3D_POS_MODE_ENABLE;
  }
  voc_status = voc_3d_pos_src_enable(channel, voc_mode);

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/*===========================================================================

FUNCTION snd_3d_pos_src_reset_req

DESCRIPTION
  This function processes a source 3D reset command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_reset_req (
  uint32                    channel,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
) {
  voc_status_type           voc_status;

  voc_status = voc_3d_pos_reset_src(channel);

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/*===========================================================================

FUNCTION snd_3d_pos_src_vol_req

DESCRIPTION
  This function processes a 3D positional source volume command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_vol_req (
  uint32                         channel,
  snd_3d_pos_src_volume_cmd_type *vol,
  snd_cb_func_ptr_type           cb_func,
  const void                     *client_data
) {
  voc_status_type voc_status = VOC_CMD_FAILED_S;

  if(vol != NULL) {
    voc_status = voc_3d_pos_src_vol(channel, vol->volume, vol->duration_ms);
  }

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_src_vol_atten_req

DESCRIPTION
  This function processes a 3D positional source volume attenuation command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_vol_atten_req (
  uint32                            channel,
  snd_3d_pos_src_vol_atten_cmd_type *vol_atten,
  snd_cb_func_ptr_type              cb_func,
  const void                        *client_data
) {
  voc_3d_pos_vol_atten_type voc_vol_atten;
  voc_status_type           voc_status = VOC_CMD_FAILED_S;

  if(vol_atten != NULL) {
    voc_vol_atten.max_distance = vol_atten->max_distance;
    voc_vol_atten.min_distance = vol_atten->min_distance;
    voc_vol_atten.scale        = vol_atten->scale;
    voc_vol_atten.mute         = vol_atten->mute ;

    voc_status = voc_3d_pos_src_vol_atten(channel, &voc_vol_atten);
  }

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_src_pos_req

DESCRIPTION
  This function processes a 3D positional source position command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_pos_req (
  uint32                       channel,
  snd_3d_pos_position_cmd_type *pos,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
) {
  voc_3d_pos_coord_type voc_pos;
  voc_status_type       voc_status = VOC_CMD_FAILED_S;

  if(pos != NULL) {
    voc_pos.x_pos = pos->x_pos;
    voc_pos.y_pos = pos->y_pos;
    voc_pos.z_pos = pos->z_pos;

    voc_status = voc_3d_pos_src_pos(channel, &voc_pos, pos->duration_ms);
  }

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION snd_3d_pos_src_vel_req

DESCRIPTION
  This function processes a 3D positional source velocity command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_src_vel_req (
  uint32                       channel,
  snd_3d_pos_velocity_cmd_type *vel,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
) {
  voc_3d_pos_coord_type voc_vel;
  voc_status_type       voc_status = VOC_CMD_FAILED_S;

  if(vel != NULL) {
    voc_vel.x_pos = vel->x_vel;
    voc_vel.y_pos = vel->y_vel;
    voc_vel.z_pos = vel->z_vel;

    voc_status = voc_3d_pos_src_vel(channel, &voc_vel, vel->duration_ms);
  }

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_src_vol_req

DESCRIPTION
  This function processes a 3D positional source position command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_reverb_src_vol_req (
  uint32                       channel,
  sint31                       volume,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
){
  voc_status_type       voc_status = VOC_CMD_FAILED_S;

  voc_status = voc_3d_reverb_src_vol(channel, volume);

  if(cb_func != NULL) {
    if(voc_status == VOC_DONE_S) {
      cb_func(client_data, SND_SUCCESS);
    } else {
      cb_func(client_data, SND_FAILURE);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_query_env_req

DESCRIPTION
  This function processes a 3D positional environment parameter
  query command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_query_env_req (
  snd_3d_pos_param_enum_type param,
  snd_3d_pos_get_cb_ptr_type cb_func,
  const void                 *client_data
) {
  voc_3d_pos_coord_type         coord;
  voc_3d_pos_orient_type        orient;
  voc_3d_pos_vector_type        vector;
  boolean                       flag;
  voc_3d_reverb_type            reverb_setting;
  snd_3d_pos_param_setting_type setting;
  voc_status_type               voc_status = VOC_CMD_FAILED_S;

  if(cb_func != NULL) {
    switch(param) {
      case SND_3D_POS_PARAM_ENV_ENABLED:
        voc_status = voc_3d_pos_query_env_enable(&flag);
        if (voc_status == VOC_DONE_S) {
          setting.env_enabled_flag = flag;
        }
        break;

      case SND_3D_POS_PARAM_LSNR_POS:
        voc_status = voc_3d_pos_query_lsnr_pos(&coord);
        if(voc_status == VOC_DONE_S) {
          setting.position.x_pos = coord.x_pos;
          setting.position.y_pos = coord.y_pos;
          setting.position.z_pos = coord.z_pos;
        }
        break;

      case SND_3D_POS_PARAM_LSNR_ORIENT:
        voc_status = voc_3d_pos_query_lsnr_orient(&orient);
        if(voc_status == VOC_DONE_S) {
          setting.lsnr_orient.x_forward = orient.forward.x_pos;
          setting.lsnr_orient.y_forward = orient.forward.y_pos;
          setting.lsnr_orient.z_forward = orient.forward.z_pos;
          setting.lsnr_orient.x_up      = orient.up.x_pos;
          setting.lsnr_orient.y_up      = orient.up.y_pos;
          setting.lsnr_orient.z_up      = orient.up.z_pos;
        }
        break;

      case SND_3D_POS_PARAM_LSNR_VEL:
        voc_status = voc_3d_pos_query_lsnr_vel(&vector);
        if(voc_status == VOC_DONE_S) {
          setting.lsnr_vel.x_vel = vector.x_pos;
          setting.lsnr_vel.y_vel = vector.y_pos;
          setting.lsnr_vel.z_vel = vector.z_pos;
        }
        break;
      case SND_3D_POS_PARAM_REVERB_ENABLED:
        voc_status = voc_3d_pos_query_reverb_enabled(&flag);
        if (voc_status == VOC_DONE_S) {
          setting.reverb_flag = flag;
        }
        break;
      case SND_3D_POS_PARAM_REVERB:
        voc_status = voc_3d_pos_query_reverb_setting(&reverb_setting);
        if (voc_status == VOC_DONE_S) {
          setting.reverb.env_type = reverb_setting.env_type;
          setting.reverb.decay_ms = reverb_setting.decay_ms;
          setting.reverb.decay_ratio = reverb_setting.decay_ratio;
          setting.reverb.ref_delay = reverb_setting.ref_delay;
          setting.reverb.reverb_gain = reverb_setting.reverb_gain;
          setting.reverb.room_gain = reverb_setting.room_gain;
          setting.reverb.reverb_delay = reverb_setting.reverb_delay;
        }
        break;
    }

    if(voc_status == VOC_DONE_S) {
      cb_func(SND_SUCCESS, param, &setting, client_data);
    } else {
      cb_func(SND_FAILURE, param, NULL, client_data);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_query_src_final_cb

DESCRIPTION
  This function is used as the callback for a 3D positional final source
  parameter settings query. It will call back the client with the information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_query_src_final_cb (
  uint32                          channel,
  voc_3d_pos_src_all_setting_type *src_all
) {
  snd_3d_pos_param_setting_type setting;
  
  if(snd_3d_pos_src_final[channel].cb_func != NULL) {
    if(src_all != NULL) {
      setting.src_all.src_enable             = src_all->src_enable;
      setting.src_all.position.x_pos         = src_all->position.x_pos;
      setting.src_all.position.y_pos         = src_all->position.y_pos;
      setting.src_all.position.z_pos         = src_all->position.z_pos;
      setting.src_all.volume.volume          = src_all->volume;
      setting.src_all.vol_atten.min_distance = src_all->vol_atten.min_distance;
      setting.src_all.vol_atten.max_distance = src_all->vol_atten.max_distance;
      setting.src_all.vol_atten.scale        = src_all->vol_atten.scale;
      setting.src_all.vol_atten.mute         = src_all->vol_atten.mute;
      setting.src_all.vel_vector.x_vel       = src_all->vel_vector.x_pos;
      setting.src_all.vel_vector.y_vel       = src_all->vel_vector.y_pos;
      setting.src_all.vel_vector.z_vel       = src_all->vel_vector.z_pos;
      setting.src_all.rev_vol.volume         = src_all->reverb_vol;
      snd_3d_pos_src_final[channel].cb_func(SND_SUCCESS,
                                  SND_3D_POS_PARAM_SRC_FINAL_SETTINGS, &setting,
                                  snd_3d_pos_src_final[channel].client_data);
    } else {
      snd_3d_pos_src_final[channel].cb_func(SND_FAILURE,
                                  SND_3D_POS_PARAM_SRC_FINAL_SETTINGS, NULL,
                                  snd_3d_pos_src_final[channel].client_data);
    }
    snd_3d_pos_src_final[channel].cb_func = NULL;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_query_src_req

DESCRIPTION
  This function processes a 3D positional source parameter query command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_3d_pos_query_src_req (
  snd_3d_pos_param_enum_type param,
  uint32                     channel,
  snd_3d_pos_get_cb_ptr_type cb_func,
  const void                 *client_data
) {
  voc_3d_pos_coord_type         coord;
  voc_3d_pos_vol_atten_type     vol_atten;
  snd_3d_pos_param_setting_type setting;
  sint31                        volume;
  voc_3d_pos_vector_type        vector;
  boolean                       flag;
  voc_status_type               voc_status = VOC_CMD_FAILED_S;

  if(cb_func != NULL) {
    switch(param) {
      case SND_3D_POS_PARAM_SRC_FINAL_SETTINGS:
        if((channel < 4) && (cb_func != NULL)) {
          if(snd_3d_pos_src_final[channel].cb_func != NULL) {
            snd_3d_pos_src_final[channel].cb_func(SND_ABORT,
                       SND_3D_POS_PARAM_SRC_FINAL_SETTINGS, NULL,
                       snd_3d_pos_src_final[channel].client_data);
            snd_3d_pos_src_final[channel].cb_func = NULL;
          }
          voc_status = voc_3d_pos_query_src_final(channel,
                                              snd_3d_pos_query_src_final_cb);
          if(voc_status == VOC_DONE_S) {
            snd_3d_pos_src_final[channel].cb_func     = cb_func;
            snd_3d_pos_src_final[channel].client_data = client_data;
            return;
          }
        }
        break;

      case SND_3D_POS_PARAM_SRC_ENABLED:
        voc_status = voc_3d_pos_query_src_enable(channel, &flag);
        if (voc_status == VOC_DONE_S) {
          setting.src_enable_flag = flag;
        }
        break;

      case SND_3D_POS_PARAM_SRC_POS:
        voc_status = voc_3d_pos_query_src_pos(channel, &coord);
        if(voc_status == VOC_DONE_S) {
          setting.position.x_pos = coord.x_pos;
          setting.position.y_pos = coord.y_pos;
          setting.position.z_pos = coord.z_pos;
        }
        break;

      case SND_3D_POS_PARAM_SRC_VOLUME:
        voc_status = voc_3d_pos_query_src_vol(channel, &volume);
        if(voc_status == VOC_DONE_S) {
          setting.src_volume.volume = volume;
        }
        break;

      case SND_3D_POS_PARAM_SRC_VOL_ATTEN:
        voc_status = voc_3d_pos_query_src_vol_atten(channel, &vol_atten);
        if(voc_status == VOC_DONE_S) {
          setting.src_vol_atten.min_distance = vol_atten.min_distance;
          setting.src_vol_atten.max_distance = vol_atten.max_distance;
          setting.src_vol_atten.scale        = vol_atten.scale;
          setting.src_vol_atten.mute         = vol_atten.mute ;
        }
        break;

      case SND_3D_POS_PARAM_SRC_VEL:
        voc_status = voc_3d_pos_query_src_vel(channel, &vector);
        if(voc_status == VOC_DONE_S) {
          setting.src_vel.x_vel = vector.x_pos;
          setting.src_vel.y_vel = vector.y_pos;
          setting.src_vel.z_vel = vector.z_pos;
        }
        break;

      case SND_3D_POS_PARAM_SRC_REV_VOL:
        voc_status = voc_3d_pos_query_src_revol(channel, &volume);
        if(voc_status == VOC_DONE_S) {
          setting.rev_vol.volume = volume;
        }
        break;
    }

    if(voc_status == VOC_DONE_S) {
      cb_func(SND_SUCCESS, param, &setting, client_data);
    } else {
      cb_func(SND_FAILURE, param, NULL, client_data);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_enable

DESCRIPTION
  This function sends a 3D positional enable/disable command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_enable (
  snd_3d_pos_mode_enum_type mode,
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_ENABLE;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;
    packet->enable_3d_pos.mode   = mode;
    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/*===========================================================================

FUNCTION snd_3d_pos_reset

DESCRIPTION
  This function sends a 3D environment reset command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_reset (
  snd_cb_func_ptr_type      cb_func,
  const void                *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_RESET;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;
    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_lsnr_pos

DESCRIPTION
  This function sends a 3D positional listener position command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_lsnr_pos (
  snd_3d_pos_position_cmd_type *pos_cmd,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SET_LSNR_POS;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;

    packet->position_3d_pos.position = *pos_cmd;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_lsnr_orient

DESCRIPTION
  This function sends a 3D positional listener orientation command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_lsnr_orient (
  snd_3d_pos_lsnr_orient_cmd_type *orient,
  snd_cb_func_ptr_type            cb_func,
  const void                      *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SET_LSNR_ORIENT;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;

    packet->orient_3d_pos.orient = *orient;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION snd_3d_pos_lsnr_vel

DESCRIPTION
  This function sends a 3D positional listener velocity command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_lsnr_vel (
  snd_3d_pos_velocity_cmd_type *vel_cmd,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SET_LSNR_VEL;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;

    packet->velocity_3d_pos.velocity = *vel_cmd;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_set

DESCRIPTION
  This function sends a 3D positional listener velocity command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_reverb_set (
  snd_3d_reverb_enum_type   reverb_type,
  snd_3d_reverb_cmd_type    *reverb_cmd,
  snd_cb_func_ptr_type          cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SET_REVERB;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;

    packet->reverb_3d_pos.reverb_type = reverb_type;
    packet->reverb_3d_pos.reverb_param = *reverb_cmd;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_enable

DESCRIPTION
  This function enables/disables 3D reverb effect

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_reverb_enable (
  boolean                      enable,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->rev_flag.reverb_enable = enable;
    packet->hdr.cmd                = SND_3D_POS_CMD_REVERB_ENABLE;  
    packet->hdr.callback_ptr       = cb_func;
    packet->hdr.client_data        = client_data;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/*===========================================================================

FUNCTION snd_3d_pos_src_enable

DESCRIPTION
  This function sends a source 3D enable command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_src_enable (
  uint32                       channel,
  snd_3d_pos_mode_enum_type    mode,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SRC_ENABLE;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;

    
    packet->src_enable_3d.channel = channel;
    packet->src_enable_3d.mode    = mode;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/*===========================================================================

FUNCTION snd_3d_pos_src_reset

DESCRIPTION
  This function sends a source 3D reset command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_src_reset (
  uint32                       channel,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SRC_RESET;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;
    
    packet->src_reset_3d.channel = channel;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_src_pos

DESCRIPTION
  This function sends a 3D positional source position command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_src_pos (
  uint32                       channel,
  snd_3d_pos_position_cmd_type *pos_cmd,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SET_SRC_POS;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;

    packet->position_3d_pos.position = *pos_cmd;
    packet->position_3d_pos.channel  = channel;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_src_vol

DESCRIPTION
  This function sends a 3D positional source volume command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_src_vol (
  uint32                         channel,
  snd_3d_pos_src_volume_cmd_type *src_vol,
  snd_cb_func_ptr_type           cb_func,
  const void                     *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SET_SRC_VOL;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;

    packet->src_vol_3d_pos.src_vol  = *src_vol;
    packet->src_vol_3d_pos.channel  = channel;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_src_vol_atten

DESCRIPTION
  This function sends a 3D positional source volume attenuation command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_src_vol_atten (
  uint32                            channel,
  snd_3d_pos_src_vol_atten_cmd_type *vol_atten,
  snd_cb_func_ptr_type              cb_func,
  const void                        *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SET_SRC_VOL_ATTEN;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;

    packet->vol_atten_3d_pos.vol_atten  = *vol_atten;
    packet->vol_atten_3d_pos.channel    = channel;
    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION snd_3d_pos_src_vel

DESCRIPTION
  This function sends a 3D positional source velocity command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_src_vel (
  uint32                       channel,
  snd_3d_pos_velocity_cmd_type *vel_cmd,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SET_SRC_VEL;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;

    packet->velocity_3d_pos.velocity = *vel_cmd;
    packet->position_3d_pos.channel  = channel;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_reverb_src_vol

DESCRIPTION
  This function sends a 3D positional source reverb volume command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_reverb_src_vol (
  uint32                       channel,
  snd_3d_reverb_src_volume_cmd_type *reverb_vol_cmd,
  snd_cb_func_ptr_type         cb_func,
  const void                   *client_data
){
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request. */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_SET_REVERB_VOL;
    packet->hdr.callback_ptr     = cb_func;
    packet->hdr.client_data      = client_data;

    packet->reverb_vol_3d_pos.reverb_vol = *reverb_vol_cmd;
    packet->reverb_vol_3d_pos.channel    = channel;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(client_data, SND_FAILURE);
  }
}


/* <EJECT> */
/*===========================================================================


FUNCTION snd_3d_pos_query_env

DESCRIPTION
  This function sends a 3D positional environment parameter query command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_query_env (
  snd_3d_pos_param_enum_type param,
  snd_3d_pos_get_cb_ptr_type cb_func,
  const void                 *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_QUERY_ENV;
    packet->hdr.client_data      = client_data;
    packet->query_3d_pos.cb_func = cb_func;
    packet->query_3d_pos.param   = param;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(SND_FAILURE, param, NULL, client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_3d_pos_query_src

DESCRIPTION
  This function sends a 3D positional source parameter query command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_3d_pos_query_src (
  snd_3d_pos_param_enum_type param,
  uint32                     channel,
  snd_3d_pos_get_cb_ptr_type cb_func,
  const void                 *client_data
) {
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if(packet != NULL) {
    packet->hdr.cmd              = SND_3D_POS_CMD_QUERY_SRC;
    packet->hdr.client_data      = client_data;
    packet->query_3d_pos.cb_func = cb_func;
    packet->query_3d_pos.param   = param;
    packet->query_3d_pos.channel = channel;

    snd_cmd(packet);
  } else if(cb_func != NULL) {
    cb_func(SND_FAILURE, param, NULL, client_data);
  }
}
#endif /* FEATURE_QAUDIOFX */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_filters

DESCRIPTION
  Filter selection for voice or data calls.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_filters
(
  snd_filters_type      filters,
    /* Data or voice filters                */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> filters.hdr.cmd          = SND_FILTERS;
    packet -> filters.hdr.callback_ptr = callback_ptr;
    packet -> filters.hdr.client_data  = client_data;
    packet -> filters.filters          = filters;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_filters */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_ec_ctl

DESCRIPTION
  Controls EC (Echo Canceller).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_ec_ctl
(
  snd_ec_ns_mode_type   ec_mode,
    /* EC control                           */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> ec_ns_mode.hdr.cmd          = SND_EC_MODE_CTL;
    packet -> ec_ns_mode.hdr.callback_ptr = callback_ptr;
    packet -> ec_ns_mode.hdr.client_data  = client_data;
    packet -> ec_ns_mode.mode             = ec_mode;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_ec_ctl */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_ns_ctl

DESCRIPTION
  Controls NS (Noise Suppressor).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_ns_ctl
(
  snd_ec_ns_mode_type   ns_mode,
    /* NS control                           */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> ec_ns_mode.hdr.cmd          = SND_NS_MODE_CTL;
    packet -> ec_ns_mode.hdr.callback_ptr = callback_ptr;
    packet -> ec_ns_mode.hdr.client_data  = client_data;
    packet -> ec_ns_mode.mode             = ns_mode;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_ns_ctl */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_vox_ctl

DESCRIPTION
  Controls VOX (half/full duplex voice mode).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_vox_ctl
(
  snd_vox_mode_type     vox_mode,
    /* VOX control                           */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> vox_mode.hdr.cmd          = SND_VOX_MODE_CTL;
    packet -> vox_mode.hdr.callback_ptr = callback_ptr;
    packet -> vox_mode.hdr.client_data  = client_data;
    packet -> vox_mode.mode             = vox_mode;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_vox_ctl */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_sidetone_ctl

DESCRIPTION
  Controls sidetone.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_sidetone_ctl
(
  snd_sidetone_mode_type     sidetone_mode,
    /* Sidetone control                           */
  snd_cb_func_ptr_type       callback_ptr,
    /* Call back function                   */
  const void                 *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> sidetone_mode.hdr.cmd          = SND_SIDETONE_MODE_CTL;
    packet -> sidetone_mode.hdr.callback_ptr = callback_ptr;
    packet -> sidetone_mode.hdr.client_data  = client_data;
    packet -> sidetone_mode.mode             = sidetone_mode;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_sidetone_ctl */

/* <EJECT> */

#ifdef FEATURE_AUDIO_AGC
/*===========================================================================

FUNCTION snd_avc_ctl

DESCRIPTION
  Controls AVC (Automatic Volume Control).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_avc_ctl
(
  boolean               avc_ctl,
    /* AVC control                          */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> avc_ctl.hdr.cmd          = SND_AVC_CTL;
    packet -> avc_ctl.hdr.callback_ptr = callback_ptr;
    packet -> avc_ctl.hdr.client_data  = client_data;
    packet -> avc_ctl.on               = avc_ctl;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_avc_ctl */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_agc_ctl

DESCRIPTION
  Controls AGC (Automatic Gain Control).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_agc_ctl
(
  boolean               agc_ctl,
    /* AGC Control                          */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> agc_ctl.hdr.cmd          = SND_AGC_CTL;
    packet -> agc_ctl.hdr.callback_ptr = callback_ptr;
    packet -> agc_ctl.hdr.client_data  = client_data;
    packet -> agc_ctl.on               = agc_ctl;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_agc_ctl */
#endif /* FEATURE_AUDIO_AGC */



/* <EJECT> */
/*===========================================================================
 FUNCTION snd_smartsnd_ctl

 DESCRIPTION Controls AGC/AVC/RVE

 DEPENDENCIES
  None

SIDE EFFECTS
  Command Packet queued to Sound Task. 
 
===========================================================================*/

void snd_smartsnd_ctl
(
  snd_smartsnd_mode_type   smartsnd,	      /* smartsound control */
  snd_cb_func_ptr_type     callback_ptr,      /* Call back function */
  const void               *client_data       /* Pointer to Client Data */
)
{
   snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */

  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> smartsnd_ctl.hdr.cmd          = SND_SMARTSND_CTL;
    packet -> smartsnd_ctl.hdr.callback_ptr = callback_ptr;
    packet -> smartsnd_ctl.hdr.client_data  = client_data;
    packet -> smartsnd_ctl.smartsnd         = smartsnd;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION snd_stereo_mute_ctl

DESCRIPTION
  Request to the HFK to mute stereo

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void snd_stereo_mute_ctl
(
  boolean               mute_stereo,
    /* Mute Stereo                          */
  snd_cb_func_ptr_type  callback_ptr,
    /* Call back function                   */
  const void            *client_data
    /* Pointer to Client data               */
)
{
  snd_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> stereo.hdr.cmd          = SND_MUTE_CAR_STEREO;
    packet -> stereo.hdr.callback_ptr = callback_ptr;
    packet -> stereo.hdr.client_data  = client_data;
    packet -> stereo.mute_stereo      = mute_stereo;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
} /* end of snd_stereo_mute_ctl */
/* <EJECT> */
#ifdef SND_EXTCOM
#error code not present
#endif /* SND_EXTCOM */

#ifdef FEATURE_AUDIO_PCR_SUPPORT
/*===========================================================================

FUNCTION snd_audfmt_adjust_sample

DESCRIPTION
 This function is used to add/drop one audio sample.

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void snd_audfmt_adjust_sample 
(
  snd_priority_type             priority,
  boolean                       add_sample,
  snd_cb_func_ptr_type          callback_ptr,
  const void                    *client_data
)
{
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd                = SND_AUDFMT_ADJUST_SAMPLE;
    packet -> hdr.callback_ptr       = callback_ptr;
    packet -> hdr.client_data        = client_data;
    packet -> adjust_sample.priority = priority;
    packet -> adjust_sample.add_sample   = add_sample;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr ( client_data, SND_FAILURE );
    }
  }
}
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION snd_audfmt_codec_update_int

DESCRIPTION
  Codec udpate interrupt function.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
void snd_audfmt_codec_update_int (
  uint32 dualmono,
  void *data_ptr
) {
  /* Pointer to Request Packet from Packet Pool */
  snd_packets_type *packet;
  snd_aac_channel_enum_type ch_mode;

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet->codec_update.hdr.cmd  = SND_OP_CODEC_INFO_UPDATE;
    packet->codec_update.codec_info.codec_type = SND_CMX_AF_FILE_AAC;
    packet->codec_update.codec_info.aac_info.codec_type = SND_CMX_AF_FILE_AAC;

    switch( dualmono ) {
      case 0xffff:
        ch_mode = SND_AAC_CHANNEL_DUAL_MONO;
        break;
      default:
        ch_mode = SND_AAC_CHANNEL_DUAL; /* for now set stereo */
        break;
    }
    packet->codec_update.codec_info.aac_info.channel_mode = ch_mode;
    packet->codec_update.client_data = data_ptr;
    snd_cmd(packet);
  }
}


/*===========================================================================

FUNCTION snd_audfmt_dual_mono_mode

DESCRIPTION
  Set dual mono mode.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
void snd_audfmt_dual_mono_mode(
  snd_priority_type                   priority,
  snd_af_dual_mono_mode_enum_type  dual_mono_mode,
  snd_cb_func_ptr_type                callback_ptr,
  const void                          *client_data
) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd                = SND_OP_DUAL_MONO_MODE;
    packet -> hdr.callback_ptr       = callback_ptr;
    packet -> hdr.client_data        = client_data;
    packet -> dual_mono_mode.priority = priority;
    packet -> dual_mono_mode.mode     = dual_mono_mode;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr( client_data, SND_FAILURE );
    }
  }
}

/*===========================================================================

FUNCTION snd_audfmt_sbr_mode

DESCRIPTION
  This function sets sbr enable/disable flag of decoder.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
void snd_audfmt_sbr_mode(
  snd_priority_type              priority,
  snd_af_sbr_mode_enum_type   sbr_mode,
  snd_cb_func_ptr_type           callback_ptr,
  const void                     *client_data
) {
  snd_packets_type *packet;
  /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = snd_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> hdr.cmd                = SND_OP_SBR_MODE;
    packet -> hdr.callback_ptr       = callback_ptr;
    packet -> hdr.client_data        = client_data;
    packet -> sbr_mode.priority      = priority;
    packet -> sbr_mode.mode          = sbr_mode;
    snd_cmd(packet);
  } else {
    if (callback_ptr != NULL) {
      callback_ptr( client_data, SND_FAILURE );
    }
  }
}

/*===========================================================================

FUNCTION snd_audfmt_dual_mono_mode_req

DESCRIPTION
 dual mono mode req fuction.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
void snd_audfmt_dual_mono_mode_req(
  snd_priority_type                   priority,
  snd_af_dual_mono_mode_enum_type  mode,
  snd_cb_func_ptr_type                callback_ptr,
  const void                          *client_data
) {
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr;
  audmain_file_type     parse_type;
  boolean               status = FALSE;

  if (priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if (priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  if ((snd_ptr->active) && 
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {
     audfmt_sound_ptr = (snd_audfmt_sound_type *) (snd_ptr->sound_ptr);

    parse_type = audmain_get_parse_type(snd_ptr->audfmt_ctl);
    switch (parse_type) {

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
        status = sndhw_audfmt_dual_mono(mode);
#endif
        break;
      default:
		break;
    }

    if (status) {
      if (callback_ptr != NULL) {
        callback_ptr( client_data, SND_SUCCESS );
      }
      audmain_play_cb( snd_ptr->audfmt_ctl,
                        &(audfmt_sound_ptr->audfmt_handle),
                        SND_CMX_AF_DUAL_MONO_MODE,
                        mode,
                        NULL );
      return;
    }
  }

  if (callback_ptr != NULL) {
    callback_ptr( client_data, SND_FAILURE );
  }
}


/*===========================================================================

FUNCTION snd_audfmt_sbr_mode_req

DESCRIPTION
  sbr mode req function.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
void snd_audfmt_sbr_mode_req(
  snd_priority_type              priority,
  snd_af_sbr_mode_enum_type   mode,
  snd_cb_func_ptr_type           callback_ptr,
  const void                     *client_data
) {
  snd_sound_status_type *snd_ptr;
  snd_audfmt_sound_type *audfmt_sound_ptr;
  audmain_file_type     parse_type;
  boolean               status = FALSE;

  if (priority == SND_PRIO_LOW) {
    snd_ptr = &(snd_array[SND_SND_LO]);
  } else if (priority == SND_PRIO_MED) {
    snd_ptr = &(snd_array[SND_SND_MED]);
  } else {
    snd_ptr = &(snd_array[SND_SND_HI]);
  }

  if ((snd_ptr->active) && 
    ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
    (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND))) {
    
    audfmt_sound_ptr = (snd_audfmt_sound_type *) (snd_ptr->sound_ptr);

    parse_type = audmain_get_parse_type(snd_ptr->audfmt_ctl);
    switch (parse_type) {

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
        status = sndhw_audfmt_sbr_mode(mode);
#endif
        break;

	  default:
		break;
    }
    if (status) {
      if(callback_ptr != NULL) {
        callback_ptr( client_data, SND_SUCCESS );
      }
      audmain_play_cb(snd_ptr->audfmt_ctl, &(audfmt_sound_ptr->audfmt_handle),
                      SND_CMX_AF_SBR_MODE, 
                      mode, 
                      NULL);
      return;
    }
  }
    if (callback_ptr != NULL) {
      callback_ptr( client_data, SND_FAILURE );
  }
}


/*===========================================================================

FUNCTION snd_audfmt_codec_update_req

DESCRIPTION
  This function will call callback fucntion.

DEPENDENCIES
 None

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
void snd_audfmt_codec_update_req (
  snd_cmx_af_info_type                *codec_info, 
  const void                          *client_data
) {
  snd_sound_status_type     *sound_status_ptr = NULL;

  sound_status_ptr = (snd_sound_status_type*) client_data;

  if ((sound_status_ptr != NULL) &&
      (sound_status_ptr->sound_ptr != NULL) &&
      (sound_status_ptr->sound_ptr->format_sound.audfmt_handle.data_func)) {    
      MSG_LOW("snd_audfmt_codec_update_req is calling next cb func",0,0,0);
      sound_status_ptr->sound_ptr->format_sound.audfmt_handle.data_func(
        SND_CMX_AF_CODEC_UPDATE,
        sound_status_ptr->sound_ptr->format_sound.audfmt_handle.audfmt_ptr,
        0, (void*) codec_info);                                          
  }

}
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */


/* <EJECT> */
/*===========================================================================

FUNCTION snd_phone_awake

DESCRIPTION
  Tell Sound that the phone is awake

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
void snd_phone_awake ( void )
{
  (void) rex_set_sigs(&snd_tcb, SND_PHONE_AWAKE_SIG);
} /* end of snd_phone_awake */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_is_task_up

DESCRIPTION
  Return init status of snd task

DEPENDENCIES
  None

RETURN VALUE
  TRUE   - If Snd Task is initialized.
  FALSE  - If Snd Task is uninitialized

SIDE EFFECTS
  None

===========================================================================*/
boolean snd_is_task_up ( void )
{
  return(snd_task_init_status);
} /* end of snd_is_task_up */

/* <EJECT> */
/*===========================================================================

FUNCTION snd_voice_prompt_playing

DESCRIPTION
  checks the status of the Sound.

DEPENDENCIES
  None

RETURN VALUE
  True:if paused False:playing state

SIDE EFFECTS
  None

===========================================================================*/

boolean snd_voice_prompt_playing( void )
{
  snd_sound_status_type *snd_ptr;
  uint32 index;
  boolean pause = FALSE;

#ifdef FEATURE_AUDIO_FORMAT
  for(index = SND_SND_LO; index <SND_ARRAY_SIZE ; index++ )
  {
    snd_ptr = &(snd_array[index]);
    if((snd_ptr->active) && 
     ((snd_ptr->sound_ptr->type == SND_AUDIO_FORMAT_SOUND) ||
      (snd_ptr->sound_ptr->type == SND_FORMAT_SOUND)))
    {
      if(audmain_get_pause(snd_ptr->audfmt_ctl))
      {
        pause = TRUE;
      }
      else
      {
        pause =  FALSE;
      }
    }
  }
#endif
  return pause;
}
/*===========================================================================

FUNCTION snd_pa_on_cb

DESCRIPTION
  This function is the callback function when PA is on, which posts a 
  command in the sound queue and wakes up sound task.

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
 None
==========================================================================*/
void snd_pa_on_cb(void)
{
  
  /* Post a command to sound queue that PA is on*/
  snd_packets_type *packet;
  packet = snd_get_packet();
  if (packet != NULL)  
  {
    packet->hdr.cmd = SND_AUDFMT_PA_ON_CB;    
    snd_cmd(packet);
  }
}
/*===========================================================================

FUNCTION snd_task

DESCRIPTION
  This procedure is the entrance procedure for the Sound Task.  It
  contains the main processing loop for the Sound Task which controls
  queue processing and sound generation.

DEPENDENCIES
  snd_array

RETURN VALUE
  Does not return

SIDE EFFECTS
  snd_tcb

===========================================================================*/
void snd_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type   sigs;
  word            change_mask;

  /* Initialize SND Task
  */
  snd_init();

  /* Wait for start signal from Main Control task
  */
#ifdef FEATURE_MULTIMODE_ARCH
  task_start( SND_RPT_TIMER_SIG, DOG_SND_RPT, &snd_rpt_timer );
#else
  mc_task_start( SND_RPT_TIMER_SIG, DOG_SND_RPT, &snd_rpt_timer );
#endif /*FEATURE_MULTIMODE_ARCH*/

  /* Clear start signal
  */
  (void) rex_clr_sigs( &snd_tcb, TASK_START_SIG );

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs( &snd_tcb, SND_RPT_TIMER_SIG );

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
  /* Start the bluetooth AG connection
  */
  sndhw_bt_init();

#endif /* FEATURE_BT_AG || FEATURE_SBC_CODEC */

  /* Send a signal to the sleep task to indicate that SND does not
  ** need the phone to stay awake.
  */
  SND_SLEEP_ALLOW();

  /* Init the change mask to 'no change'
  */
  change_mask = SND_NO_CHANGE_M;

  /* Infinite loop -- process events forever
  */
  for (;;) {

    /* Wait for something to happen
    */
    sigs = rex_wait( SND_RPT_TIMER_SIG      |   /* Watchdog report timer */
                     SND_CMD_Q_SIG          |   /* Command Queue signal  */
                     TASK_STOP_SIG          |   /* Task stop from MC     */
                     TASK_OFFLINE_SIG       |   /* Task offline from MC  */
                     SND_VOC_CHNG_SIG       |   /* Vocoder state change  */
                     SND_PHONE_AWAKE_SIG    |   /* The phone Woke Up!    */
                     SND_PATH_TIMER_SIG     |   /* Path timer expired    */
                     SND_TONE_TIMER_SIG     |   /* Tone 'done' signal    */
                     SND_UNMUTE_SIG         |   /* Audio can be unmuted  */ 
#ifdef FEATURE_AUDIO_FORMAT
                     SND_VOC_MODULE_STATUS  |   /* Module support on current image*/
#endif
#ifdef SND_EXTCOM
#error code not present
#endif
#ifdef FEATURE_ANIMATED_AAC
                     SND_AAC_RETRY_SIG  |
#endif
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#ifndef FEATURE_AVS_SBCENC_TASK 
                     SND_SBC_CODEC_TIMER_SIG |
#endif
#endif
                     SND_OVERRIDE_TIMER_SIG     /* Override done signal  */
                   );

    /* If watchdog report timer expired, report and restart the timer
    */
    if ( (sigs & SND_RPT_TIMER_SIG) != 0 ) {
      dog_report(DOG_SND_RPT);
      (void) rex_set_timer( &snd_rpt_timer, DOG_SND_RPT_TIME );
    }

    /* If we're supposed to shutdown, let's do it
    */
    if ( (sigs & TASK_STOP_SIG) != 0 ) {
      snd_shutdown();
    }

    /* If we're supposed to go offline, let's do it
    */
    if ( (sigs & TASK_OFFLINE_SIG) != 0 ) {
      snd_offline();
    }

    /* The phone woke up.
    */
    if ( (sigs & SND_PHONE_AWAKE_SIG) != 0 ) {
      (void) rex_clr_sigs( &snd_tcb, SND_PHONE_AWAKE_SIG );
    }

#ifdef FEATURE_AUDIO_FORMAT
    if ( (sigs & SND_VOC_MODULE_STATUS ) != 0)
    {
    	(void) rex_clr_sigs( &snd_tcb, SND_VOC_MODULE_STATUS );
    	sndhw_voc_module_fetched();
    }

    if ( (sigs & SND_AUDIO_FORMAT_SIG) != 0 ) {
      /* If this signal is set, clear it. We should never get to this
      ** point in the code because this signal should never be set unless
      ** the sound task is doing a rex_ait for this signal elsewhere. It is
      ** being cleared here as a precaution.
      */
      (void) rex_clr_sigs( &snd_tcb, SND_AUDIO_FORMAT_SIG );
    }
#endif /* FEATURE_AUDIO_FORMAT */

    /* Let the hardware routine know when the path timer expires.
    */
    if ( (sigs & SND_PATH_TIMER_SIG) != 0 ) {
      if(sndhw_is_path_ready() == SND_PATH_NOT_READY) {
        /* Tones are being expired because a timeout has occured, meaning
        ** that it has been 500ms since sound has requested the vocoder
        ** and hasn't gotten a response. It is bad to play tones when they 
        ** are not expected. Sounds and Multi-tones don't get expired as 
        ** they are longer term sounds, expected to go on.
        */
        snd_expire(SND_TONE);
        change_mask |= SND_TONE_M;
      } else {
        sndhw_path_timer_ctl();
        sigs |= SND_TONE_TIMER_SIG;     /* start any active tones       */
      }
      (void) rex_clr_sigs( &snd_tcb, SND_PATH_TIMER_SIG );
    }

    /* Clear path override. Set signal so path will be returned to
    ** default.
    */
    if ( (sigs & SND_OVERRIDE_TIMER_SIG) != 0 ) {
      MSG_HIGH("Clear path override - timer expired",0,0,0);
      (void) rex_clr_sigs( &snd_tcb, SND_OVERRIDE_TIMER_SIG );
      sndhw_override_done();
      sigs |= SND_TONE_TIMER_SIG;
    }

    /* Unmute the mic, the delay timer has expired. 
    */

    if ( (sigs & SND_UNMUTE_SIG) != 0) {
      (void) rex_clr_sigs( &snd_tcb, SND_UNMUTE_SIG );
      sndhw_mute_done();
      sigs |= SND_TONE_TIMER_SIG;
    }

    /* Deal with a vocoder state change
    */
    if ( (sigs & SND_VOC_CHNG_SIG) != 0 ) {
      (void) rex_clr_sigs( &snd_tcb, SND_VOC_CHNG_SIG);
      /* set path after vocoder has been acquired */
      sndhw_voc_change_path_set();
      /* If the path setting finished, otherwise wait for
      ** snd_path_timer to expire.
      */
      if (sndhw_is_path_ready() == SND_PATH_READY) {
        sigs |= SND_TONE_TIMER_SIG;     /* start any active tones       */
      }
#ifdef FEATURE_VOICE_PLAYBACK
      /* Resume voice prompt play back if necessary.
      */

    voc_pb_resume (snd_voice_prompt_playing());

#endif /* FEATURE_VOICE_PLAYBACK */
#ifdef FEATURE_AUDIO_FORMAT
      /* Start audio format fast-forward sequence as it may have been
      ** waiting for the DSP to get to QSYNTH state.
      */
      (void) snd_audfmt_do_seek();
#endif /* FEATURE_AUDIO_FORMAT */
#ifdef FEATURE_MIDI_OUT
      if ((snd_midi_out_enabled == SND_MIDI_OUT_STATE_PENDING) &&
          sndhw_midi_out_path_ready()) {
        snd_midi_out_enabled = SND_MIDI_OUT_STATE_ENABLED;
        if (snd_midi_out_cb != NULL) {
          snd_midi_out_cb(snd_midi_out_client_data, SND_ENABLED);
        }
      }
#endif /* FEATURE_MIDI_OUT */

#ifdef FEATURE_WEBAUDIO
      snd_check_wa_audfadec_int();
#endif /* FEATURE_WEBAUDIO */
    }

#ifdef SND_EXTCOM
#error code not present
#endif

    /* Process the queued commands, return a mask of configuration changes
    ** and "and" them in with any we might already have.
    */
    if ( (sigs & SND_CMD_Q_SIG) != 0 ) {
      change_mask |= snd_req();
    }


#ifdef FEATURE_ANIMATED_AAC
    if( (sigs & SND_AAC_RETRY_SIG) != 0){
      (void) rex_clr_sigs( &snd_tcb, SND_AAC_RETRY_SIG);
      snd_check_wa_audfadec_int();
    }
#endif

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#ifndef FEATURE_AVS_SBCENC_TASK 
    if( (sigs & SND_SBC_CODEC_TIMER_SIG) != 0) {
      (void) rex_clr_sigs( &snd_tcb, SND_SBC_CODEC_TIMER_SIG);
      audiosbc_clear_backlog();
    }
#endif
#endif


    /* Process changes to the audio state.  This is either because time
    ** has passed, or snd_req() changed the configuration while processing
    ** sound commands.
    */
    if ( (change_mask != SND_NO_CHANGE_M)
    || (sigs & SND_TONE_TIMER_SIG) != 0 ) {

      snd_change( change_mask );        /* make changes to sound hardware */
      change_mask = SND_NO_CHANGE_M;    /* changes have now been done     */
    }

#ifdef SND_TEST_CODE
    /* Check to make sure nobody is keeping and/or losing packets.  Note
    ** that this can occasionally appear if we happen to look while somebody
    ** really is using a packet.
    */
    {
      static word last_bad_count = SND_NUM_PACKETS;
      word        current_count;
      current_count = q_cnt ( &snd_free_q );
      if ( current_count == SND_NUM_PACKETS ) {
        /* If all the Packets are Present, Clear the "last" counter.
        */
        last_bad_count = SND_NUM_PACKETS;
      } else {
        /* If Packets are missing, don't print an error until the same
        ** number of packets are missing twice in a row.  This suppresses
        ** messages when packets really are in use doing something.
        */
        if (current_count != last_bad_count) {
          last_bad_count = current_count;
        } else {
          MSG_ERROR( "Sound Packet Loss (%d/%d)", q_cnt( &snd_free_q ),
                                                  SND_NUM_PACKETS, 0 );
        }
      }
    }
    /*
    ** Call Hardware Test Block (if compiled into system)
    */
    snd_test_code(&change_mask);
#endif
  } /* end of infinite loop */
} /* end of snd_task */

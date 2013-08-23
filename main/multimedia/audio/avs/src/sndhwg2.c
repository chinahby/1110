/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Sound Hardware Routines (2nd/3rd Generation)")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

S O U N D   H A R D W A R E   R O U T I N E S   ( 2 N D   G E N E R A T I O N )

GENERAL DESCRIPTION
  These routines provide the low level hardware routines for the Sound Task.
  Any hardware related information related to the 2nd and 3rd Generation
  Phones is to be hidden here.

REFERENCES
  None

EXTERNALIZED FUNCTIONS
  sndhw_register_aux_ctl
    This function registers a function to be used as the auxilliary generator.
  snd_setup_voice_path
    This function sets up the state of an audio output path.
  sndhw_voc_change_path_set
    This function does a path set after the vocoder has been acquired.
  sndhw_voc_acquire_needed
    Returns whether a voc acquire was requested and if so sets the process in
    motion.
  sndhw_is_path_ready
    Returns the path status.
  sndhw_path_timer_ctl
    Puts the path in a ready state.
  sndhw_stereo_mute
    Set Hardware to Mute/Unmute Car Stereo as requested.
  sndhw_hfk_aec
    Sets AEC mode on HFK.
  sndhw_hfk_phs
    Sets PHS mode on HFK.
  sndhw_hfk_ctl
    Sets pad value according to AEC, PHS, stereo mute and volume.
  sndhw_extcom_ctl
    Sets up external communications.
  sndhw_extcom_protocol
    Sets the protocol being used with the HFK.
  sndhw_hfk_tone_timer_expired
    Returns whether the HFK tone timer has expired.
  sndhw_hfk_vr_tone
    Sends command to indicate that tone is being played.
  snd_rx_mute_ctl
    Temporary mute/unmute of the audio input from the CDMA/FM receive path.
  sndhw_txfm_mute_ctl
    Temporary mute/unmute of the audio output to the FM transmit path.
  sndhw_dtmf_tx_mute_ctl
    Temporary mute/unmute of the DTMF output to the FM transmit path.
  sndhw_gen_tone
    Generate a new tone
  sndhw_init
    Generic Low Level Hardware Initialization for Sound Functions and chips.
  sndhw_filters_ctl
    Control type of filtering for Call (Data or Voice)
  sndhw_comp_ctl
    Compander control for IS-55 tests.  Turn the compressor/expander off/on.
  sndhw_set_int_volume
    Sets the database volume for a given device and method.
  sndhw_set_volume
    Set volume per current Path settings.
  sndhw_set_apath_in
    Set Input Audio Path per current Path Requests.
  sndhw_set_apath_out
    Set Output Audio Path per current Path Requests.
  sndhw_set_tone_path_out
    Sets up the path to play a tone.
  sndhw_override_done
    Signals the end of the override timer.
  sndhw_unmute_done
    Signals the end of the unmute timer.
  sndhw_restore_voice_path
    Function restores voice path after a tone has finished playing.
  sndhw_init_from_nv
    Hardware init including reading any configurable values from NV.
  sndhw_cleanup_vp
    Function cleans up when voice prompts are finished.
  sndhw_abort_vp
    Function aborts currently playing voice prompts.
  sndhw_ppq_free_all
    Function clears voice prompt pointer queue.
  sndhw_ppq_set_list
    Function sets the internal voice prompt list to be used.
  sndhw_ppq_enqueue
    Function enqueues a voice prompt onto the voice prompt pointer queue.
  sndhw_ppq_internal_q
    Returns whether the current voice prompt list is the internal list.
  sndhw_pq_free_all
    Function clears prompt queue.
  sndhw_pq_enqueue
    Function enqueues index onto prompt queue.
  sndhw_mm_record
    Function sends a command to the vocoder driver to start recording
    MultiMedia data.
  sndhw_mm_record_stop
    Function sends a command to the vocoder driver to stop recording
    MultiMedia data.
  sndhw_mm_record_data
    Get the recorded data from QDSP to snd layer.
  sndhw_enable_eclock
    Enable (external)auxpcm clock.
  sndhw_disable_eclock
    Disable (external)auxpcm clock.
  sndhw_set_aux_line_in
    Configure the Aux PGA Line In loopback path.
  sndhw_set_fm_over_bt
    This function do the processing related to FM over BT
  sndhw_check_aux_line_in
    Return aux line in status
  sndhw_set_aux_line_in_vol
    Set the aux line in volume

Copyright(c) 1990 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/sndhwg2.c#37 $ $DateTime: 2009/10/21 01:52:26 $ $Author: kkmajeti $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/09   kkm     Added function sndhw_get_audfmt_need_config to provide audfmt 
                   configuration status.
10/06/09   sj      Added SCMM specific changes
09/03/09   kkm     Modified to call bt_cmd_ag_set_ad_spkr_gain is called to set 
                   gain when BT device AG is connected.
07/31/09   rm      snd_expire gets called soon whenever present image does not
                   support its module in dsp. relaxing sound expire by 100ms.
                   [CR 189921 - Sound mute issue (VOC_CMD_BUSY_S)]
07/07/09   rk      Notifying SND that PAs are turned on, wakes up sound task.
06/10/09   kkm     Modifed to RX Mute is called only when sound tone is playing. If sound
                   tone is not playing then there is no need to mute rx. Fixes CR #181973.
                   When mediaplayer is ended the mute command is proceesing and unmute command
                   is saved and not processed for some time. So audio break is observed.
05/08/09    vm     Added Support for Device switch from external to internal.
04/20/09    rm     CR 168030, handling voc_state_get_high_module
01/19/09   anb     Added sndhw_set_qtv_cb_func so that QTV can be informed
                   about mod/image swap event.Fixes Cr 163807(SR-1133583)
02/09/09   bk      Featurized for removing compilation warnings.
01/23/09   skr     Fix for In call mp3 Audio issue with respect to clk regime
01/19/09   bk      Fixed the compilation error arised after trying to
                   featurize for FEATURE_STD_MIDI.
01/19/09   dp      CR 166184 Removed unnecessary calls of
                   bt_cmd_ag_ring_audio_device and removed bt state dependancy
                   on sound device selection
01/19/09   dp      CR 168251 Removed feturization for FEATURE_BT_QSOC
12/26/08   rm      Added RVE API changes
02/12/08   dp      correcting sco link setup for MPEG+AMR content
11/24/08   skr     Added code for Audio + camera Concurrency
11/20/08   knm     Added code to support FM on HS-USB.
11/20/08   dp      Fixed sample rate initialization issue for QTVE AMR-NB clips
11/18/08   dp      Featurization fix for lowtier build
10/22/08   dp      FM over BT AG changes
10/14/08   anb     Delay sending bitstream/midi commands to the DSP till BT
                   AG connection is up when FEATURE_AVS_BT_SCO_REWORK enabled.
                   Changes complete fix for CR 140315
10/14/08   skr     Fixed CR 156254 Reset the sample rate and configuration
09/24/08   vsud    Added feature guard for Ext Ringer.
09/24/08   skr     Reset the Qensemble interrupt request flags in
                   sndhw_audfmt_cleanup().
09/15/08   knm     Added support for Aux Line In(FM) volume control
08/26/08   anb     Added support for DSP based WMAPRO
08/11/08   skr     Added fix for CR 145385 and CR 138959
08/01/08   knm     Added fix to remove compiler warnings
07/03/08   skr     Fixed issue of midi commands not being sent when FFWD/RWND
                   midi clips during In call CMX scenario
06/20/08   knm     Added sndhw_set_aux_line_in API to configure AuxPGA
                   Line In loopback path.
05/12/08   skr     Fixed CR 145175. changes in sndhw_gen_tone
05/09/08   anb     Fixed a compilation issue(CR 145920) on SC2X nor builds.
05/09/08   skr     Fixes for Thin UI compilation issues for 6246
04/11/08   skr     Fixes for CR 141860 and 142714.
03/16/08    sj     Added Support for OEMSBL
03/11/08    kk     Added support for sending mp3 configuration
                   information for DSP.
02/28/08    az     Fixed the issue with the in call midi.
01/15/08    hs     Added support ISDB-T dual mono output control
01/07/08    az     Included the in call midi and the new in call architecture.
12/19/07   wfd     Wrap use of VOC_INFORM_GRAPH_AUDIO w/ FEATURE_GRAPH_AUDIO.
11/21/07   anb     Fixed compilation errors for SC2X prepaid build
11/15/07    az     Featurized the code for In call to avoid compilation for
                   other targets.
11/02/07    az     Included the support for the in call wma.
10/26/07    hs     Added EVW file format support
10/26/07    az     Included the support for the in call AAC.
09/14/07   anb     Fixed CR 122104
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
08/08/07  anb/wfd  Added aux_pcm_clk to be enabled/disabled from other tasks like BT (CR 122106).
04/20/07    at     Added .EVB file format recording support.
12/04/06   suk     Fixed 6260 ThinUI build compilation error.
11/29/06    ay     Fixed G.711 audio muting. Codecs which are used for both
                   voice+audio should check and apply unmute settings; voice
                   and audio muting go through different paths.
11/20/06    sp     Added support for OEMSBL Audio.
11/09/06    az     Merged the code for MM API changes.
10/31/06  sp/anb   Serialized the commands sent to BT, Fixes 97271
09/08/06    pl     Added 44K clk case in sndhw_set_sample_rate().
09/05/06    ay     Added AMR-WB+ support.
                   Updated copyright text.
09/01/06    hz     Fixed CR#100139 -- No volume control for ADPCM in camera
                   preview mode.
08/29/06    hz     Added support for incall MP3 on 65nm platforms.
07/31/06    sp     Fixed some merge issues in AV Sync feature and
                   some issues in SCO rework feature
07/27/06   anb     Added the FEATURE_AVS_BT_SCO_REWORK which enables the AVS to send
                   messages to the BT driver
06/14/06    sp     Delay sending bitstream/midi commands to the DSP till BT
                   A2DP connection is up.
03/27/06    ak     Added funtion sndhw_sidetone_ctl to support sidetone control
03/14/06    pl     Fixed compilation warning
03/13/06   act     Rewired DTMF mixing support in sndhw_rx_mute_ctl() for the
                   sake of 1x targets.
03/11/06    hz     Added support for evrc in-call MP3 support.
03/10/06    sm     Fixed a featurization issue with SDAC
02/21/06   act     Modified sndhw_rx_mute_ctl() to handle new parameter for
                   DTMF mixing support.
01/31/06    hz     Added support for in-call mp3 feature.
01/30/06    sp     Added support for I2S SBC interface.
11/16/05    sp     Made changes to support BT CCAP requirements. BT APP can
                   now initialize SCO connection at any time it is required.
                   Audio driver will issue codec config and setup the hardware
                   link only when appropriate image gets loaded onto ADSP.
11/04/05    pl     Modified the Wave AVSync support in sndhw_gen_tone() to be
                   guarded by FEATURE_QSYNTH_ADPCM and FEATURE_QSYNTH_PCM.
10/19/05    pl     Added PCM AVSync support.
09/23/05    sp     Fixed a compiler error that occurs when FEATURE_SBC_CODEC is
                   not defined.
09/19/05    hz     Added fixes in sndhw_voc_vp_enq for CR76675.
09/18/05    sp     Added support for DSP based SBC encoder.
09/16/05    sp     Fixed a compiler error.
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
09/12/05    hz     Added AV sync support for windows media.
08/31/05    sm     VOX support
            sm     Enhanced aux codec support to pass codec mode.
08/29/05    sp     Made changes so that keybeeps and messages
                   are played on BT HS when AG/A2DP connection is enabled, added
                   debug messages when A2DP/AG connections are enabled/disabled,
                   Fixed a bug so that streaming playback resumes on BT HS after
                   fwd/rewind, made changes so that A2DP disable happens at
                   vocfunc.c
08/11/05    hz     Added support for DTX and data requrest time for AMR
                   recording.
07/28/05   act     Corrected a typo in sndhw_set_apath_out with respect to
                   SND_DEVICE_TTY_HCO.
07/26/05    sm     Added stereo USB support.
07/22/05    sm     Added Wav A/ULaw support.
07/19/05    pl     Modified sndhw_audfmt_adec_data() to handle new vocoder error
                   status.
07/13/05    sp     Added code to honor bitpool values preferred by BT A2DP
                   headset.
07/06/05   act     Added voc_codec_type parameter to functions registered with
                   voc_register_codec_func().
06/23/05    sm     TTY updates for auto-bypass and VCO/HCO
06/15/05    st     Modified code to disable sndhw_vp_playing flag only when
                   sndhw_abort_vp() is called to prevent AV sync
                   reinitialization when VP playback is pre-empted.
06/03/05    st     Modified code to add support for simultaneous AUX generator
                   and sound.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/24/05    hz     Added Support for MultiMedia/AAC record functionality.
05/18/05    ac     Modified code to support AV sync functionality with QCP,
                   MP3 and AAC.
05/16/05    hz     Added support for Vocoder image+ADPCM+VFE.
04/28/05   ymc     Added 4GV NB QCP recording support.
04/21/05   act     Reverted name of voc_codec_id_type to voc_codec_type.
04/01/05   act     Changed use of specific codec IDs to generic codec types
                   where applicable.
03/25/05   ac/sg   Added support for A/V synchronization.
03/21/05    aw     Modified snd_crnt_path_out to use SND_METHOD_MIDI if midi_out
                   is enabled and tone is not playing CR#57599.
02/14/05    hz     Added Support for WindowsMediaAudio decode under
                   FEATURE_WMA.
12/03/04    sm     Added FEATURE_USB_CARKIT support.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
08/19/04    sm     Added FEATURE_AUDIO_CAL_MED_HIGH_FFA.
08/11/04    st     Modified code to handle multiple MIDI command buffers.
07/30/04    st     Modified code to set start_aging flag in sndhw_audfmt_tone;
                   modified code to call ADEC CB at initialization for PCM
                   playback.
05/19/04    lg     Cleaned up code to not send voc_config in sndhw_audfmt_cmd.
05/13/04    sm     Removed test code from PMIC_SPEAKER_ON_OFF_DELAY.
05/04/04    sm     Added FEATURE_PMIC_SPEAKER_ON_OFF_DELAY.
04/22/04    lg     Added support for Animated AAC Ringers.
03/22/04    st     Added support for PCM recording.
                   Updated sndhw_audfmt_adec_pause() to remove sequence
                   parameter; removed sndhw_set_seq_vol() function; updated
                   sndhw_set_volume() to set METHOD_MIDI volume correctly.
03/05/04    sg     Modified to clear time_remain_ms in qtunes_gen_tone only
01/26/04    lg     Modified code to support volume adjustment and audioPause as
                   an array for multi_sequencer
01/26/04    sg     Added 1ms delay after DTMF generation ends, to allow the tone
                   to end more gracefully - this helps in reducing clipping
                   if the codec type is being switched after the DTMF ends.
                   Modified to wait for codec clock switch to complete before
                   a sound is taken out of the hold state.
12/02/03    sg     Added support for linear PCM playback.
11/24/03    aw     Modified code to not update qsynth hybrid mode value if
                   midi out path is set. Added sndhw_midi_out_path_ready func.
11/22/03    sg     Added support for RealAudio codec. Reset time_remain_ms
                   if the vocoder is not ready for MP3/AAC/RA.
11/07/03  aw/st    Modified code to support YADPCM and audfmt Multi-sequences.
10/30/03    sm     Fixed compiler warnings.
10/10/03    sg     Modified to support additional AAC params reqd by the DSP.
08/21/03   ymc     Checked sndhw_bt_hs_ringing in sndhw_gen_tone.
           ymc     Removed PVCSPath.
05/13/03   ymc     MSM6200 MIDI is now using 32KHz codec clock.
05/06/03    sg     Modified so that AAC specific configuration is written to the
                   DSP only after the AAC image is downloaded.
04/18/03    aw     Added audio support for Q3D image.
04/17/03    sm     Added AMR file format support.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface.
03/10/03    st     Added sndhw_audfmt_adec_pause() for FEATURE_ADPCM.
02/28/03    st     Added sndhw_ec_ctl() and sndhw_ns_ctl() functions.
02/19/03  aw/st    Added support for playback of ADPCM file format.
01/20/03    st     Modified code to issue ADEC interrupt after setting callback
                   function in case interrupt was already pending; modified
                   code to play rings on ringer for BT headset ring.
01/05/03    sg     Updated sndhw_midi_to_dtmf() to use the correct index to
                   lookup the DTMF values.
12/03/02    st     Modified code to add support for graph audio states;
                   modified BT code to use updated interface.
11/19/02    st     Added sndhw_audfmt_synth_mvol() and sndhw_audfmt_adec_mvol()
                   functions.
                   Fixed audfmt race condition in sndhw_gen_tone.
11/18/02    st     Modified code to perform iMelody volume adjustment only
                   if tone_status_ptr is not NULL; Added FEATURE_CLICK support
                   for PMIC ringer.
11/06/02    sm     Added QCP AMR support.
11/05/02    sg     Added support for iMelody format.
10/29/02    st     Added support SPEAKER_PHONE device.
09/06/02    st     Modified code to register for BT AG events; modified code
                   to inform vocoder driver when BT audio connected event
                   occurs.
08/13/02    st     Added sndhw_set_adec_mode() function.
            st     Added check for VOC_INFORM_MP4 in sndhw_voc_acquire().
           sg/st   Added support for MP4 and AAC; modified code to not reset
                   path timer when INFORM_RESET is received, path timer is
                   required to timeout tones.
07/11/02    yl     Modified the function parameters for sndhw_qcp_record().
07/09/02    sm     T_MSM6200 uses 22kHz MIDI image.
06/13/02    st     Modified code to support 32kHz MIDI image (below comment
                   from 03/21 should have read 22kHz).
06/11/02    st     Modified code to support fixed fullrate EVRC and fixed
                   halfrate 13k QCP recording options.
03/21/02    st     Modified code to support 32kHz MIDI image.
03/12/02    st     Modified code to revert back to only setting master_volume
                   if method is MIDI.
03/12/02    st     Modified code to support PMIC speaker; unfeaturized sample
                   rate code because it now applies going forward; obsoleted
                   FEATURE_NEW_SYNTH; replaced voc_codec_gain() calls with
                   voc_codec_pad(); merged sndhw_set_codec_gain() and
                   sndhw_set_volume().
02/21/02    st     Modified code to support PMIC ringer.
01/15/02   ymc     Deleted VOC_API_VERSION usage.  VOC_API_VERSION_1 is no
                   longer supported.
           ymc     Updated copyright.
10/04/01    sg     Added VCO and HCO modes for TTY.
10/01/01    st     Modified code to support multiple ADEC channels: modified
                   sndhw_audfmt_adec_chan_vol(), sndhw_audfmt_adec_chan_pan(),
                   sndhw_audftm_adec_data(), sndhw_audfmt_adec_flush() and
                   sndhw_audfmt_adec_reset().
10/01/01    sm     Added FEATURE_BT_AG_LOCAL_AUDIO.
09/03/01    st     Added support for QCP recording; modified code to always
                   set muting in snd_rx_mute_ctl().
07/24/01   ymc     Put in SND_HFK dependency inside sndhw_stereo_mute.
                   Removed code to re-mute voice in sndhw_voc_tone.
06/18/01    ro     Always set muting in sndhw_set_apath_in.
06/18/01  jkl/ym   Added FEATURE_ACP.
04/26/01    st     Modified code to add audio decoder type indicator in
                   sndhw_audfmt_adec_data().
04/26/01    sm     Removed speed_factor from voc_mp3_set_song_config().
04/04/01    st     Added support for direct MIDI device output interface;
                   modified code to add support for new featurization; added
                   sndhw_audfmt_adec_reset(); added sndhw_audfmt_stereo_pan()
                   and reworked stereo pan code; lint updates.
03/02/01    sm     Moved BT audio_connect/disconnect to vocoder task.
02/28/01    st     Lint updates.
01/26/01    st     Modified code to only set master volume if the current
                   method is SND_METHOD_MIDI in sndhw_set_codec_volume();
                   modified code to use SND_SAMPLE_RATE_8000 as the default
                   sampling rate; removed call to reset sampling rate in
                   sndhw_audfmt_cleanup(); added call to flush adec in
                   sndhw_audfmt_cleanup().
12/10/00    st     Modified code to use sndhw_set_sample_rate() instead of
                   calling voc_sample_rate() directly.
12/04/00    sm     Add adec_chan_vol(), adec_chan_pan(). Update pan when needed.
                   Write voc_master_volume(). Chk status fm voc_adec_move_data()
            sm     Added AUDMAIN, MP3, MIDI, QCP, ADEC support.
09/25/00    sm     Added sndhw_bt_ag_init().
09/21/00    sm     Removed change to rename bt_cmd_ag_ring_headset().
09/18/00    sm     Renamed bt_cmd_ag_ring_headset() to bt_cmd_ag_ring_hs().
                   Add logic to shut off the BT_AG ringer.
09/12/00    sm     Check last generator before change on midi note.
09/08/00    st     Bluetooth support.
07/22/00    st     Featurized the vibrator support in sndhw_audfmt_tone().
07/15/00    st     Lint updates; added support for vibrator control for audio
                   format sounds.
07/14/00   spf     Added support for TTY headset and HFK devices.
05/30/00    st     Added sndhw_set_codec_volume() function.
05/05/00    st     Added support to send midi messages to DSP; added code to
                   handle VOC_INFORM_QSYNTH vocoder informs; added call to
                   voc_config() to unconfig QSYNTH when not playing midi,
                   CR13462.
05/02/00   spf     Updated sndhw_voc_change() to ignore VOC_INFORM_NO_SAT.
            sm     aud_process_ctl() now calls aud_process_override w/ BIO_SW10
03/06/00    st     Added SND_HFK_PLUS around call to sndhw_hfk_ctl().
02/10/00    st     Removed sleep logic, sound no longer has to wait for phone
                   to wake up before acquiring vocoder.
02/03/00    st     Use SND_BAD_VP instead of SND_FAILURE to indicate a bad
                   voice prompt header/buffer.
11/15/99    st     Rearranged functions such that functions are declared before
                   being used.
11/11/99   jkl     Fixed volume level that was not in dbm values.
10/27/99   spf     Updated to use new VOC_API_VERSION definitions.
10/18/99    st     Enabled AGC commands to the vocoder for all targets that
                   have FEATURE_AUDIO_AGC defined.
10/14/99   jkl     Replaced FEATURE_QDSP2 for VOC_API_VERSION.
10/04/99   jkl     Reviewed codec volumes and pcm pad functionality.
09/23/99    st     Fixed bug in sndhw_set_apath_in() where muting was not
                   properly done coming out of analog mode.
09/22/99    st     Fixed bug in sndhw_ring() where a NULL pointer was possibly
                   being accessed.
09/09/99  jkl/st   Modifications after code review. Added
                   AGC/AVC control functions.
08/24/99    st     Added sndhw_click(); added code to handle click state
                   machine; added capability to release vocoder in cases where
                   a ringer tone interrupts a vocoder tone; ignore SAT DCC
                   status in sndhw_voc_change();
08/17/99  st/jkl   Modified for new API and Sound Calibration files
07/19/99   st/vl   Added status to voice prompt callback function.
                   Removed 'near '. Modified the manner in which muting is
                   done for voice prompts (done to control muting of voice
                   prompts in the vocoder driver in certain situations).
                   Fixed references to AGC static gain.
06/30/99  sm/RPW   Integrated AGC feature with VR/VM features; added Q2 support;
                   removed snd_voc_acquire_timer, we will wait for phone to
                   wake up when it is ready; use VOC_OFF_BOARD when doing ext.
                   communications, mu-law with padding now supported by vocoder;
                   general code changes to remove compiler errors (st).
03/22/99    st     Merged Odie changes into mainline.
03/22/99    st     Added snd_rx_mute_ctl(); modified code to allow vocoder to
                   tell us if it is in CDMA or FM mode.
03/04/99    st     Incorporated ha's frequency calculation routines for vocoder:
                   added sndhw_compute_vocoder_freq(). Added supporting table
                   snd_voc_values for data interpolation (these have since
                   been moved to vocsup).  Moved all ringer and vibrator
                   references to ring.c and ring.h (ringer driver), renamed
                   remaining sndrng_ functions with sndhw_ prefix, added an
                   ADSP freq calculation routine.
03/03/99    st     Reset mode in sndhw_hfk_vr_tone() if not in VR mode, CR8093.
01/15/99    st     Added DTMF_CONST and VOL_CONST to volume and DTMF tables
                   to compile tables into ROM for ARM targets; added support
                   for voice prompting; added casts, etc.. to remove compiler
                   warnings and errors; also modified earpiece volume levels
                   and VBAP initialization for 5GP targets as per
                   electro-acoustics team's suggestions, CR8263.
01/14/99    st     Modified the initialization of snd_ctl_hset_ring to enable
                   volume scaling, CR7723.
01/14/99    st     Do not send HFK setup if going from TGP-VR mode to TGP mode.
12/17/98    st     Corrected volume mapping of keybeep levels for Q target;
                   now only send AEC command to IDS if different from previous
                   setting; added code to properly handle the cases where the
                   HFK is either simply in TGP protocol or in TGP-VR mode.
12/04/98    sm     Support selectable ROM keyword for backward compatibility.
11/09/98    st     Added timeout for request to play tones on HFK-VR; changed
                   index of vol_map_ext_beep and vol_map_ext_voice_beep for
                   Odie target to match the settings for Q target, CR6909; also
                   entered Odie and Q calibrated levels from electro-acoustics
                   group; modified vol_map_ext_beep and vol_map_ext_voice_beep
                   for Q target; modified the initial VBAP register(0x01) value
                   for TGP; fixed bug in sndhw_extcom_ctl(); don't call
                   voc_codec_pad() in sndhw_which_codec() anymore when in
                   ext_com mode; modified sndhw_send_hfk_setup() to send proper
                   volume to HFK.
11/04/98    sm     Merged ARM/MSM3000 support to mainline.  Support test click.
10/16/98    st     Added support for seven earpiece and HFK speaker volume
                   levels for Q targets, also created separate gain table for
                   Q headset. CR6671.
10/12/98    st     Fixed bug: inserted #ifdef SND_OLD_VOLUME_UNITS around
                   SND_UI_VOL_TO_INDEX macro call.
10/08/98    st     HFK-VR mainlining: Implemented new deviceID sequence; HFK
                   volume, AEC mode, PHS override, and stereo mute now issued
                   via IDS command if HFK is in TGP protocol; added support for
                   playing tones on the HFK while in VR mode; added general
                   support for HFK voice recognition.
10/08/98    st     Added filter setting in test_block; added initialization for
                   ctl_flags field of snd_vol_ctl_type structures for
                   DATA_FILTER volume override, added DATA_FILTER volume
                   override check in function sndhw_get_voc_setting().
                   Fixes CR 6485. Also, moved path setting from
                   sndhw_voc_change() to its own function to be called from
                   snd when it responds to the SND_VOC_CHNG_SIG, fixes CR6626.
10/02/98    st     Modified snd_ext_voice_gains for Odie target; implemented
                   use of different voice volume tables for headset and HFK
                   for Odie target.
10/01/98    st     Added AVBAP initialization table for TGP; added code to
                   support choosing preemphasis filters based on which codec
                   is being used in targets that support more than one codec.
08/28/98    st     Modified code to set timer that will expire SND_TONE if
                   the vocoder is not ready after 500ms: CR6033. Also,
                   changed global variables to be in 'near' and added '_cdecl'
                   to sndhw_click_isr() in attempt to save code space.
08/24/98    st     Made changes in sndhw_init() to initialize EAR_MUTE.
08/18/98    st     Merged modifications from 08/10 that were lost in the
                   last checkin.
08/17/98    st     Added AVBAP initialization table (snd_vbap_init_table)
                   and function (sndhw_vbap_init_reg). Modified code so that
                   either new vbap or old codec can be used on TGP.
08/10/98    st     Replaced usage of the SinBaD interface with the BORSCHT
                   interface by request from the WLL group.
07/16/98    st     Backed out RO's changes from 03/25/98, also fixes CR5390.
                   Also changed snd_phone_gains and AVBAP register settings
                   for 5GP target by request from the electro-acoustics group.
07/10/98    st     Increased the number of volume levels to 4 (0..4) for ADSP
                   tone generation for the SSS, modified snd_adsp_beep_gains
                   and vol_map_adsp_beep.
06/22/98    st     Added code to support the playing of tones using the ADSP
                   on the SSS.
06/18/98    ha     Added snd_ext_ring_gains[] table for T_T HFK volume
                   scaling support. Modified SND_RING_D tone to be 2750 Hz.
05/29/98    ha     Restored original tone mappings in sndrng_dtmf and added
                   SND_RING_D tone (2600Hz).
05/28/98    st     Modified the volume levels for DTMF tones for SSS
                   by request from the electro-acoustic group. CR5468.
05/13/98    ro     Remove timed waits in sndhw_which_codec.  They were no
                   longer needed, and were causing a problem with the DOTG
                   card. Fixes CR5343.
05/05/98    ha     Modified snd_ctl_ext_beep to use SND_GEN_RING_VOCTX
                   instead of SND_GEN_VOC if FEATURE_CAR_KIT_DTMF is off.
04/29/98    ha     Added separate snd_ctl_ext_beep and snd_ctl_ext_ring tables
                   for when FEATURE_CAR_KIT_DTMF is off. Fixes CR 5298.
04/23/98    st     Added initialization for ctl_flags field of snd_vol_ctl_type
                   structures, added BIO_SW10 volume override check in
                   function sndhw_set_volume. Fixes CR 4888.
04/21/98    ha     Changed sndrng_dtmf[] from ROM to DTMF_MEMORY to support
                   Gemini "Old ringer hardware".
04/02/98    ha     Added Support for T_O and AVBAP.
                   Test block provides vbap read/write of any vbap register
                   if SND_AVBAP_REQUIRED is defined.
03/25/98    ro     Put tones on hold when the vocoder is in reset.
                   Use voc_get_status to get initial vocoder state.
03/09/98    ha     Changed SND_EARPIECE_ON_TIME from 50 msec to 80 msec.
02/23/98    ro     In a DOTG call, mute if volume = 0.  Fixes CR 3774.
02/13/98    ha     Added HFK volume scaling support.
                   Created sndhw_get_table_index for volume setting table index
                   calculation.
                   SND_UI_VOL_TO_INDEX is now controlled by SND_OLD_VOLUME_UNITS
                   vol_ctl uses snd_ctl_phone_aux instead of snd_ctl_ext_beep
                   in snd_get_tone_volume when using SND_CLASS_AUX.
02/06/98    ro     No call to sndhw_comp_ctl in sndhw_init (was obsolete).
01/30/98    ha     Added SND_DT_DTACO_TONE in support of DTACO roaming dial tone
                   sndrng_click() now uses Timer1 in MSM2300+ and Timer2 for
                   all earlier MSM versions.
01/22/98    ha     Adjusted Q's beep_gains to produce equivalent output to TGP's
                   beep_gains.
01/21/98    ha     Added support for volume scaling separated dtmf[] and
                   frequency[] tables for targets for conditional compile.
                   Added additional piano notes.
01/16/98    ro     Added support for external communications and device ID.
                   Added support for Headset Adapters.
01/12/98    ro     Added mute parameter to sndhw_stereo_mute.
                   Added default case in sndhw_voc_change.
                   Update copyright.
12/23/97    ha     Added SND_CLASS_RING support.
                   Removed SND_APATH_PRIVACY support.
12/08/97    ha     Added sndrng_click() -- function produces a 'click' sound.
                   Removed obsolete comments.
11/21/97    ha     Modified SND_RING_A to use DS6 tone. (2489.1 Hz)
10/31/97    ro     Separate T_Q external path table, updated gains for Q HFK.
                   Change Q HFK FM mic. gains, fixing CR 4211.
                   Change carkit turn-on time for T_Q to 100ms from 500ms.
09/18/97    ha     Separated (TG==T_MD) Tables for conditional compile.
08/25/97    ro     Change DTMF gain in "data" volume tables to match voice
                   DTMF gain - CR3745.
08/23/97    ro     In sndhw_set_apath_out, fix voice mute problem going in
                   and out of HFK - CR3756.
                   For TGP, changed all FM mic. gains to unity.
08/15/97    ro     Calibration - by ear - of TGP carkit keybeep and DTMF
                   levels.  Closing CR3539 and CR3286.
08/08/97    ro     Fix problem with vol_map_phone_voice_beep and
                   vol_map_ext_beep. HIGH volume tone now plays at max vol.
07/30/97    ro     In sndhw_voc_tone, limit dtmf duration to max.
07/23/97    ro     TGP Ringer tuning. Using sndring.h.
                   Fix calculation of maximum volume levels - MAPSIZE.
                   Enter calibrated ringer and earpiece tone levels.
                   Lint fixes for T_I2.
07/17/97    ro     Fixed conditional compile on last version.
07/17/97    ro     Q Ringer tuning. Ringer "Jam High" under #if.
06/17/97    ro     Support for T_Q, replaces FEATURE_SND_FLIP_PHONE.
                   Don't initialize snd_voc_dtmf_volume in definition.
06/17/97    ro     Add support for 425Hz vocoder tone - SND_DT_TONE_BRAZIL.
06/06/97    ro     Move control of HFK+ AEC mode to upper layer.
                   Update sndhw_stereo_mute() for HFK+.
                   Don't use Ear-Mute if volume is 0, only if path is muted.
                   Remove references to snd_vox_chng_timer.
                   Added additional (total 6) volume levels for TGP ringer.
                   Revert to previous voc_change interface.
06/05/97    ro     Update to ISS2 calibration.
06/04/97    ro     Enter calibrated levels for ISS2 earpiece, 3.0 dB steps.
                   Level 2 is nominal.
05/23/97    ro     Enter the ringer volume in % rather than "shift" to
                   enable higher resolution.
                   Complete support for "HFK Plus" carkit.  Disable external
                   control of stereo muting when "HFK Plus" is enabled.
                   Change TGP  feedback tone to 852Hz.
04/29/97    ro     Fix stray end-comment build problem.
04/29/97    ro     Support TGP carkit - "NGP Plus". Added sndhw_hfk_ctl().
                   Volume tables include level 0 (mute), volume functions
                   reflect change.
04/25/97    ro     Change #if SND_DEBUG to #ifdef SND_DEBUG
04/24/97    ro     Enter calibrated levels for TGP Earpiece, 3.0 dB steps.
04/22/97    ro     Reset path timer in sndhw_voc_release(), to fix missing
                   first tone problem.
                   Change codec pad value in snd_muted_hf_gains.
04/11/97    ro     Change sndhw_rxfm_mute_ctl() to sndhw_rx_mute_ctl() and
                   use instead of calling voc_rx_mute_cmd() directly.
                   Disable init_from_nv() for "Q" phone.
                   Change definition of snd_voc_dtmf_volume and snd_tx_muted
                   to LOCAL. Update dependencies.
03/28/97    ro     Added volume tables to support TGP.
                   Separated ringer and vocoder tables to support independent
                   tuning. Changed some sndhw prefixes to sndrng.
                   Make sndhw_voc_acquire local since it is no longer used
                   externally. Use FEATURE_VIBRATOR.
03/12/97   jjn     Added (TG==T_MD) for Module in conditional compile statements
03/10/97    ro     Fix build problem for T_T.
02/12/97    ro     Added support for aux generators - SND_CLASS_AUX
                   Using sound features SND_USE_RINGER_ENABLE, and
                   SND_USE_RINGER_INT. Using FEATURE_SND_FLIP_PHONE.
                   Using FEATURE_SINBAD.
                   In sndhw_set_apath_out, fix problem with volume setting
                   when new volume is the same as but vol_ctl pointer is
                   different from current.
02/06/97    ro     Enter calibrated levels for earpiece DTMF's, 4.5 dB steps.
01/28/97    ro     In set_apath_out, set volume before muting path. Transfer
                   control of AUX_SYNC_VAL to vocoder. Change codec pad
                   value in muted_hf_gains.
01/20/97    ro     Added SND_RPT1 in support of key delay function.
                   Use FEATURE_CAR_KIT_DTMF to enable DTMF's at the speaker.
                   Changed vol_ctl_* to snd_ctl_*
                   Release ear mute only in phone path.
01/17/97    ro     In sndhw_voc_tone() - fix problem with FM "ringing" due to
                   missing dtmf overwrite mode in MSM2P.
01/10/97    ro     Added (TG==T_I2) for definition of SND_UI_VOL_TO_INDEX.
01/07/97    ro     Support for Car-kit and Earpiece DTMF's using the vocoder.
                   Rework volume and path setting. Update test functions.
                   Added definitions for SND_SIGNAL_SILENCE and
                   SND_RPT_NOSIGNAL to sndhw_dtmf[].
10/14/96    ro     Added (TG==T_T) cases to all #if's that have (TG==T_G)
                   for initial support of TGP build.
09/18/96   fkm     Roll Rel 7 "branch" changes into baseline.
08/14/96   rah     Changed snd_ext_gains to provide even step response in
                   current HFK.
07/18/96   fkm     Removed sndhw_ext_pcm() routine (had become a NOP).
05/29/96   fkm     Change Car Kit Volume to not Use SND_CODEC_0DB.
04/30/96   dak     Added T_I2 to some #ifdefs to support new ringer circuit
04/18/96   fkm     Changed so no longer need uisnd.h to compile
04/03/96   fkm     Control of MANY signals moved to VOC Routines
03/27/96   rah     Inverted sense of AUX_CLK_VAL in phone path of
                   sndhw_which_codec.
03/21/96   fkm     Remove Debug Code that Didn't Belong.
03/19/96   fkm     Updates Per Code Review
02/22/96   fkm     Changed index_a/index_b to index_hi/index_lo and updated
                   sndhw_dtmf table so High/Low tones appear correctly.
02/16/96   fkm     Tweeks to comments and documentation
02/12/96   fkm     Lint and Documentation Updates.  Some Apex Code (disabled).
01/17/96   fkm     Added T_C1 Stuff.
01/03/96   fkm     Fix TX DTMF Mutes during Handoffs
12/21/95   fkm     Moved Consistancy Checks under SND_DEBUG and added
                   SND_DEBUG_MSG() in some places.
12/20/95   fkm     Fixed ISS2 sndhw_phone_gains[] table
12/13/95   fkm     ISS2 Ringer Update, Menu-7-4 (mute) fix, Lint Nits
12/06/95   fkm     Most SND_HZI_UNUSED values removed from sndhw_dtmf[].
11/30/95   fkm     New Gemini Ringer Tones, Better External PCM Control,
                   Old/New Ringer Detection/Tone Selection and PIP/India Tones
11/17/95   fkm     Fixed sndhw_set_volume() for Car Kit RX Muting.
11/10/95   fkm     RNG_M_START bit now added when Ringer Started (no longer
                   kept in table).
11/02/95   fkm     Minor Name Change to Vocoder Gain Adjust Stuff.
11/02/95   fkm     Support for Vocoder Filter's and Gain Control Added.
10/26/95   fkm     Fix ISS2 Race Condition for Flash hook Processing
10/11/95   fkm     Wait for Phone Awake before voc_acquire()
09/28/95   fkm     Move voc_codec_select() call to cover Hardware Bug.
09/21/95   fkm     Wait for Phone to "unsleep" before doing voc_acquire()
09/06/95   fkm     voc_acquire() on Stereo Mute and ISS2 rex_def_timer fix.
08/22/95   fkm     Car Stereo Mute Support, Set Minimum Codec Volume on path
                   change, fixed "first call" mute Problem.
08/15/95   fkm     ISS2 Support, voc_acquire(), 8K/13K Car Kit
07/21/95   fkm     Delete Audio Patch (see below), VOC_UNITY_GAIN now only
                   set going to Analog Mode, Car Kit gets CLK/SYNC at startup.
07/11/95   fkm     #undef'ed rdh Temporary Patch (see 06/19/95 below)
07/06/95   fkm     First Cut at "real" Car Kit Support
06/19/95   rdh     Temporary workaround to keep transient garbage out of audio.
06/19/95   fkm     Factory Patch: Force Car Kit Enable from DM "poke"
06/14/95   fkm     Fixed RX FM Muting, Ear Muting and ISS2 #if's
06/08/95   fkm     Gemini Ringer Turn Off Code, plus Compander Update
05/16/95   fkm     Single Ringer Tone now uses both sides of Ringer, some
                   ISS2 Preparation, REORDER now IS-99 Compliant, Problem
                   with VOC_UNIT_GAIN fixed, old "junk code" deleted.
01/18/95   fkm     Created from code from in SND.C

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions          */
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "voc.h"                /* Vocoder typedefs and prototypes      */
#include "sndi.h"               /* Sound internal typedefs              */
#include "snddev.h"             /* Devices and Method                   */
#include "snddata.h"            /* Sound Calibration Database functions */
#include "ring.h"               /* Ringer definitions                   */
#include "sndihw.h"             /* Generic Sound Hardware Interface     */
#include "rex.h"                /* definition of REX data types         */
#include "task.h"               /* Task definitions and prototypes      */
#include "msg.h"                /* Message logging/reporting services   */
#include "err.h"                /* Error logging/reporting services     */
#include "snd.h"                /* Sound typedefs and prototypes        */
#include "hw.h"                 /* Prototype for hw_capability()        */
#include "voci.h"
#include "vocstate.h"
#ifdef SND_EXTCOM
#error code not present
#endif
#ifdef FEATURE_BORSCHT
#error code not present
#endif
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
#include "bt.h"
#endif /* #if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#include "btpf.h"
#endif
#if defined(FEATURE_PMIC_RINGER) || defined(FEATURE_PMIC_SPEAKER)
#include "pm.h"                 /* PMIC driver prototypes               */
#endif /* FEATURE_PMIC_RINGER || FEATURE_PMIC_SPEAKER */
#ifdef FEATURE_USB_CARKIT
#include "usbcc.h"
#endif /* FEATURE_USB_CARKIT */
#ifdef FEATURE_HS_USB_ANALOG_AUDIO
#include "hsu_analog_audio.h"
#include "hsu_al_task.h"
#endif /* FEATURE_HS_USB_ANALOG_AUDIO */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_VOICE_PLAYBACK
/* Structure that contains the queue of voice prompts that are to be played */
snd_prompts_q_type sndhw_prompt_q;

/* Structure that contains a list of pointers to voice prompts */
snd_prompt_ptr_q_type sndhw_prompt_ptr_q;

/* Pointer to current list of voice prompts */
voc_pb_header_type **sndhw_prompt_ptr_q_list = sndhw_prompt_ptr_q.prompts;

/* Flag to indicate whether voice prompt is currently playing */
boolean sndhw_vp_playing = FALSE;
#endif

/* Below there is LOCAL data and regional data.  There is no global     */
/* data.  Since C does not directly support regional data, regional     */
/* data is labelled "[Regional Data]".                                  */

/* Set local message level [Regional Data] and change defintion from    */
/* msg.h to use by "poke-able" message level for Sound Sub-system.      */

extern word snd_msg_level;

#undef  MSG_THRESHOLD
#define MSG_THRESHOLD   snd_msg_level

/*-------------------------------------------------------------------------
** State variables
**-----------------------------------------------------------------------*/
/* Keep track of which hardware is running                               */

boolean snd_voc_is_ok        = FALSE; /* TRUE If Vocoder Currently Working */
boolean snd_voc_acquired     = FALSE; /* TRUE If Vocoder Acquired          */

boolean sndhw_aux_busy             = FALSE; /* Auxiliary generator is on?   */
void (*sndhw_aux_ctl_fn) (boolean) = NULL;  /* Function pointer for aux gen */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* Audio paths                                                              */
/* Current Audio output path */
snd_pstate_type       snd_crnt_path_out;
/* Current input mute state */
snd_mute_control_type sndhw_crnt_path_in_mute   = SND_MUTE_MUTED;

/* Current input mute state for voice path */
snd_mute_control_type sndhw_voice_path_out_mute = SND_MUTE_MUTED;

/* Indicator that a tone is currently playing */
boolean sndhw_tone_playing = FALSE;

/* Path timer */
rex_timer_type      snd_path_timer;
/* Path status */
snd_path_ready_type snd_path_is_ready = SND_PATH_NOT_READY;

#define SND_UNMUTE_TIME 30               /* 30 msec unmute timer       */
rex_timer_type snd_unmute_timer;         /* unmute timer               */
boolean        sndhw_unmute_flag = TRUE; /* flag to delay mic unmuting */

/* Flags to control the path override function.  The reason for this
** function is to keep the vocoder on for a period of time once it
** has been turned on to play a tone.  Keeping the vocoder on avoids
** repeated delays and clicks caused by switching on and off.
*/
#ifndef FEATURE_OEMSBL_AUDIBLE_TONE
#define SND_OVERRIDE_TIME  3000        /* 3 second path override */
#else
#define SND_OVERRIDE_TIME  0           /* 0 second path override */
#endif
rex_timer_type snd_override_timer;     /* override timer         */
boolean        sndhw_keep_voc_acquired = FALSE;

#if (defined(SND_HFK) && !defined(SND_HFK_PLUS))
/* Flag that indicates if UI wants the Car Stereo Muted. */
boolean sndhw_stereo_mute_s = FALSE;   /* Assume Car Stereo Not Muted  */
#endif /* SND_HFK && !SND_HFK_PLUS */

#ifdef SND_EXTCOM
#error code not present
#endif

#ifdef FEATURE_AUDIO_AGC
boolean sndhw_agc_on = FALSE;
boolean sndhw_avc_on = FALSE;
#endif /* FEATURE_AUDIO_AGC */


snd_smartsnd_mode_type sndhw_smartsnd_status = SND_SMARTSND_NONE;


#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
bt_app_id_type sndhw_bt_handle     = 0;  /* client id */
boolean        sndhw_bt_hs_ringing = FALSE;
#endif

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
boolean sndhw_bt_a2dp_connected = FALSE;
boolean sndhw_codec_supports_a2dp = FALSE;
#endif
#if defined (FEATURE_BT_AG)
boolean        sndhw_bt_ag_connected   = FALSE;
#if defined (FEATURE_AVS_BT_SCO_REWORK)
boolean        sndhw_bt_sco_connect_evt_rcvd=FALSE;
boolean        sndhw_codec_supports_ag = FALSE;
#endif/*AVS_BT_SCO_REWORK*/
#endif
/* Variable to remember what type of "filtering" has been               */
/* requested.  Set initially to SND_FILTERS_VOICE so targets that don't */
/* support this option will do the right thing anyway.                  */

snd_filters_type snd_current_filters = SND_FILTERS_VOICE;

#ifdef FEATURE_AUDIO_FORMAT
boolean sndhw_audfmt_need_config = TRUE;
rex_timer_type snd_voc_module_timer;     /* voc module config timer     */
#define SND_VOC_MODULE_TIME  100         /*voc module trial timer 100ms */
#endif

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
void *sndhw_adec_client_data = NULL;
#endif

#ifdef FEATURE_PCM_REC
/* These variables indicate the status of PCM recording for reverse and
** forward links.
*/
boolean sndhw_pcm_rec_reverse_enabled = FALSE;
boolean sndhw_pcm_rec_forward_enabled = FALSE;
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*
 * These variables indicate the status of MultiMedia recording for reverse and
 * forward links.
 */
typedef enum {
  SNDHW_MM_REC_STATUS_PENDING,
  SNDHW_MM_REC_STATUS_ENABLED,
  SNDHW_MM_REC_STATUS_DISABLED,
  SNDHW_MM_REC_STATUS_MAX
} sndhw_mm_rec_status_type;

sndhw_mm_rec_status_type sndhw_mm_rec_status = SNDHW_MM_REC_STATUS_MAX;
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_MIDI
#ifdef FEATURE_AUDFMT_MULTISEQUENCER
#define SNDHW_MIDI_NUM_SEQ          4
#else
#define SNDHW_MIDI_NUM_SEQ          1
#endif /* FEATURE_AUDFMT_MULTISEQUENCER */
#endif /* FEATURE_MIDI */

#ifdef FEATURE_MIDI_OUT
/* Flag to indicate whether MIDI output device is enabled */
boolean sndhw_midi_out_enabled = FALSE;
#endif /* FEATURE_MIDI_OUT */

#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
/* This enum is used to indicate the source/reason for a command
*/
typedef enum {
#ifdef FEATURE_AUDIO_FORMAT
  SND_AUDFMT_SOURCE_NORMAL,      /* Normal source for command            */
#endif
#ifdef FEATURE_MIDI_OUT
  SND_AUDFMT_SOURCE_MIDI_OUT,    /* Source of command is MIDI out device */
#endif
  SND_AUDFMT_SOURCE_MAX
} snd_audfmt_source_type;
#endif /* defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT) */

#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
snd_array_idx_type 			sndhw_current_index  		= SND_ARRAY_SIZE;
snd_audfmt_source_type 		sndhw_current_source 		= SND_AUDFMT_SOURCE_MAX;
voc_capability_type			sndhw_current_capability 	= VOC_CAP_NONE;
extern snd_sound_status_type snd_array[SND_ARRAY_SIZE];
extern void snd_expire( snd_array_idx_type snd_idx);
#endif

#ifdef FEATURE_IMELODY
/* Table used to convert a MIDI note to a frequency
*/
const uint16 snd_midi_note_to_freq[] = {
 55,    58,    62,    65,    69,    73,
 78,    82,    87,    92,    98,    104,
 110,   117,   123,   131,   139,   147,
 156,   165,   175,   185,   196,   208,
 220,   233,   247,   262,   277,   294,
 311,   330,   349,   370,   392,   415,
 440,   466,   494,   523,   554,   587,
 622,   659,   698,   740,   784,   831,
 880,   932,   988,   1047,  1109,  1175,
 1245,  1319,  1397,  1480,  1568,  1661,
 1760,  1865,  1976,  2093,  2217,  2349,
 2489,  2637,  2794,  2960,  3136,  3322,
 3520,  3729,  3951,  4186,  4435,  4699,
 4978,  5274,  5588,  5920,  6272,  6645,
 7040,  7459,  7902,  8372,  8870,  9397,
 9956,  10548, 11175, 11840, 12544, 13290,
 14080, 14917, 15804, 16744, 17740, 18795,
 19912, 21096, 22351, 23680, 25088, 26580
};
#endif

#ifdef FEATURE_AVS_I2SSBC
boolean                sndhw_i2ssbc_voc_acquired = FALSE;
snd_sample_rate_type   sndhw_i2ssbc_sample_rate  = SND_SAMPLE_RATE_NONE;

snd_i2ssbc_cb_func_ptr_type sndhw_i2ssbc_cb_func_ptr
                                                 = NULL;
snd_i2ssbc_status_type  sndhw_i2ssbc_status      = SND_I2SSBC_STATUS_DISABLED;
#endif /* FEATURE_AVS_I2SSBC */

#ifdef FEATURE_PMIC_RINGER
/* Flag to indicate if PMIC ringer is enabled */
boolean sndhw_pmic_ring_enabled = FALSE;
#endif /* FEATURE_PMIC_RINGER */

#ifdef FEATURE_PMIC_SPEAKER_ON_OFF_DELAY
boolean sndhw_pmic_speaker_delay = FALSE;
boolean sndhw_pmic_speaker_start = FALSE;

#define SNDHW_PMIC_COOLDOWN_TIME 45        /* 45 millisecond cooldown */

#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
voc_bt_cb_func_ptr_type sndhw_bt_cb_func_ptr = NULL;
boolean sndhw_bt_playback_other_started      = FALSE;
boolean sndhw_bt_playback_dtmf_ring_started  = FALSE;
boolean sndhw_bt_playback_dtmf_started       = FALSE;

#endif /*AVS_BT_SCO_REWORK*/

/* <EJECT> */
#ifdef FEATURE_K_AMPLIFIER
#ifdef FEATURE_KAMP_ON_OFF_DELAY
boolean sndhw_kamp_delay = FALSE;
boolean sndhw_kamp_start = FALSE;

#define SNDHW_KAMP_COOLDOWN_TIME 45        /* 45 millisecond cooldown */

#endif /* FEATURE_KAMP_ON_OFF_DELAY */

void sndhw_kamp_cmd(boolean on)
{
    gpio_tlmm_config(GPIO_OUTPUT_10);
    if(on)
    {
        gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
        clk_busy_wait(1);
        gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
        clk_busy_wait(1);
        gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
        clk_busy_wait(1);
        gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
        clk_busy_wait(1);
        gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
        //clk_busy_wait(1);
        //gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
        //clk_busy_wait(1);
        //gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_HIGH_VALUE);
    }
    else
    {
        gpio_out(GPIO_OUTPUT_10,(GPIO_ValueType)GPIO_LOW_VALUE);
    }
}

#ifdef FEATURE_KAMP_ON_OFF_DELAY
/*===========================================================================

FUNCTION sndhw_restore_pmic_speaker

DESCRIPTION
  This function restores the PMIC speakerphone state.

DEPENDENCIES
  It is executed in the vocoder's task context, so it is dependent on VOC_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_restore_kamp()
{
  sndhw_kamp_delay = FALSE;
  if(sndhw_kamp_start == TRUE) {
    sndhw_kamp_start = FALSE;
    sndhw_kamp_cmd(TRUE);
  }
}
#endif /* FEATURE_KAMP_ON_OFF_DELAY */
/*===========================================================================

FUNCTION sndhw_pmic_speaker_ctl

DESCRIPTION
  This function commands the PMIC speakerphone on or off.

DEPENDENCIES
  It is executed in the vocoder's task context, so it is dependent on VOC_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -esym(715,codec) codec parameter is not used*/
void sndhw_kamp_ctl(
  voc_codec_type       codec,
  voc_codec_mode_type  mode,
  boolean              on
)
{
  if(on) {
#ifdef FEATURE_KAMP_ON_OFF_DELAY
    if(sndhw_kamp_delay == TRUE) {
      sndhw_kamp_start = TRUE;
    } else
#endif /* FEATURE_KAMP_ON_OFF_DELAY */
    {
       sndhw_kamp_cmd(TRUE);
    }
  } else {
#ifdef FEATURE_KAMP_ON_OFF_DELAY
    if(sndhw_kamp_delay == TRUE) {
      sndhw_kamp_start = FALSE;
    } else {
      sndhw_kamp_cmd(FALSE);

      /* Wait before allowing to turn on again */
      sndhw_kamp_delay = TRUE;
      voc_pmic_speaker_delay(SNDHW_KAMP_COOLDOWN_TIME,
                             sndhw_restore_kamp);
    }
#else
    sndhw_kamp_cmd(FALSE);
#endif /* FEATURE_KAMP_ON_OFF_DELAY */
  }
}
/*lint +esym(715,codec)*/
#endif /* FEATURE_PMIC_SPEAKER */

#ifdef FEATURE_PMIC_SPEAKER
#ifdef FEATURE_PMIC_SPEAKER_ON_OFF_DELAY
/*===========================================================================

FUNCTION sndhw_restore_pmic_speaker

DESCRIPTION
  This function restores the PMIC speakerphone state.

DEPENDENCIES
  It is executed in the vocoder's task context, so it is dependent on VOC_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_restore_pmic_speaker()
{
  #ifdef MSMAUD_SCMM
#error code not present
  #endif /* MSMAUD_SCMM */

  sndhw_pmic_speaker_delay = FALSE;
  if(sndhw_pmic_speaker_start == TRUE) {
    sndhw_pmic_speaker_start = FALSE;
    #ifndef MSMAUD_SCMM
    pm_speaker_cmd(PM_SPKR_ON);
    #else
#error code not present
    #endif /* MSMAUD_SCMM */
  }
}
#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */
/*===========================================================================

FUNCTION sndhw_pmic_speaker_ctl

DESCRIPTION
  This function commands the PMIC speakerphone on or off.

DEPENDENCIES
  It is executed in the vocoder's task context, so it is dependent on VOC_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -esym(715,codec) codec parameter is not used*/
void sndhw_pmic_speaker_ctl(
  voc_codec_type       codec,
  voc_codec_mode_type  mode,
  boolean              on
)
{
  #ifdef MSMAUD_SCMM
#error code not present
  #endif /* MSMAUD_SCMM */

  if(on) {
#ifdef FEATURE_PMIC_SPEAKER_ON_OFF_DELAY
    if(sndhw_pmic_speaker_delay == TRUE) {
      sndhw_pmic_speaker_start = TRUE;
    } else
#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */
    {
      #ifndef MSMAUD_SCMM
      pm_speaker_cmd(PM_SPKR_ON);
      #else
#error code not present
      #endif /* MSMAUD_SCMM */
    }
  } else {
#ifdef FEATURE_PMIC_SPEAKER_ON_OFF_DELAY
    if(sndhw_pmic_speaker_delay == TRUE) {
      sndhw_pmic_speaker_start = FALSE;
    } else {
      #ifndef MSMAUD_SCMM
      pm_speaker_cmd(PM_SPKR_OFF);
      #else
#error code not present
      #endif /* MSMAUD_SCMM */

      /* Wait before allowing to turn on again */
      sndhw_pmic_speaker_delay = TRUE;
      voc_pmic_speaker_delay(SNDHW_PMIC_COOLDOWN_TIME,
                             sndhw_restore_pmic_speaker);
    }
#else
    #ifndef MSMAUD_SCMM
    pm_speaker_cmd(PM_SPKR_OFF);
    #else
#error code not present
    #endif /* MSMAUD_SCMM */

#endif /* FEATURE_PMIC_SPEAKER_ON_OFF_DELAY */
  }
}
/*lint +esym(715,codec)*/
#endif /* FEATURE_PMIC_SPEAKER */

#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
/*===========================================================================

FUNCTION sndhw_usb_audio_config_cb

DESCRIPTION
  This function receives a result from the USB audio config call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_usb_audio_config_cb(
  boolean status
)
{
  MSG_MED("USB config cb: %d",status,0,0);
}

/*===========================================================================

FUNCTION sndhw_usb_audio_reset_cb

DESCRIPTION
  This function receives a result from the USB audio reset call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_usb_audio_reset_cb(
  boolean status
)
{
  MSG_MED("USB reset cb: %d",status,0,0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_stereo_usb_audio_config_cb

DESCRIPTION
  This function receives a result from the stereo USB audio config call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_stereo_usb_audio_config_cb(
  boolean status
)
{
  MSG_MED("USB config cb: %d",status,0,0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_stereo_usb_audio_reset_cb

DESCRIPTION
  This function receives a result from the stereo USB audio reset call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_stereo_usb_audio_reset_cb(
  boolean status
)
{
  MSG_MED("USB reset cb: %d",status,0,0);
}

#ifdef FEATURE_USB_CARKIT

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_usb_ctl

DESCRIPTION
  This function commands the USB Carkit on or off.

DEPENDENCIES
  It is executed in the vocoder's task context, so it is dependent on VOC_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_usb_ctl( voc_codec_type codec, voc_codec_mode_type mode, boolean on )
{
  boolean result;
  static sndhw_usb_active = FALSE;

  if(on) {
    if(sndhw_usb_active == FALSE) {
      sndhw_usb_active = TRUE;

#ifdef FEATURE_INTERNAL_SDAC
      if(codec == VOC_CODEC_STEREO_USB) {
#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON)
        if(mode == VOC_CODEC_MODE_MULTIMEDIA) {
          result = usb_otg_cc_config_audio(USB_OTG_CC_AUDIO_MODE__STEREO,
                                           sndhw_stereo_usb_audio_config_cb);
        } else
#endif /* defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON) */
        {
          result = usb_otg_cc_config_audio(USB_OTG_CC_AUDIO_MODE__MONO,
                                           sndhw_usb_audio_config_cb);
        }
      } else
#endif /* FEATURE_INTERNAL_SDAC */
      {
        /* Mono USB carkit */
        result = usb_otg_cc_config_audio(USB_OTG_CC_AUDIO_MODE__MONO,
                                         sndhw_usb_audio_config_cb);
      }
    }
  } else {
    if(sndhw_usb_active == TRUE) {
      sndhw_usb_active = FALSE;
      result = usb_otg_cc_reset_audio(sndhw_usb_audio_reset_cb);
    }
  }
}

#else /* FEATURE_HS_USB_ANALOG_AUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_usb_ctl

DESCRIPTION
  This function commands the USB Carkit on or off.

DEPENDENCIES
  It is executed in the vocoder's task context, so it is dependent on VOC_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_usb_ctl( voc_codec_type codec, voc_codec_mode_type mode, boolean on )
{
  boolean status;
  static sndhw_usb_active = FALSE;
  static hsu_analog_audio_conf_param_type audio_config_param;

  audio_config_param.completion_cb = NULL ;

  if(on) {
    if(sndhw_usb_active == FALSE) {
      sndhw_usb_active = TRUE;

#ifdef FEATURE_INTERNAL_SDAC
      if(codec == VOC_CODEC_STEREO_USB) {
        switch (mode)
        {
#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON)
          case VOC_CODEC_MODE_MULTIMEDIA:
            audio_config_param.new_mode = HSU_AUDIO_MODE_STEREO_SPK;
            break;
#endif /* defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON) */
#ifdef FEATURE_MM_REC
          case VOC_CODEC_MODE_MM_REC:
            audio_config_param.new_mode = HSU_AUDIO_MODE_STEREO_SPK;
            break;
#endif /* FEATURE_MM_REC */
          default:
            /* Check aux line in status */
            status = voc_check_aux_line_in();
            if(status == TRUE)
            {
              audio_config_param.new_mode = HSU_AUDIO_MODE_STEREO_SPK;
            }
            else
            {
              audio_config_param.new_mode = HSU_AUDIO_MODE_MONO_SPK_MIC;
            }
            break;
        }
        hsu_al_task_enqueue_cmd(hsu_analog_audio_config, (void*)(&audio_config_param));
      } else
#endif /* FEATURE_INTERNAL_SDAC */
      {
        switch (mode)
        {
#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON)
          case VOC_CODEC_MODE_MULTIMEDIA:
            audio_config_param.new_mode = HSU_AUDIO_MODE_MONO_SPK;
            break;
#endif /* defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON) */
#ifdef FEATURE_MM_REC
          case VOC_CODEC_MODE_MM_REC:
            audio_config_param.new_mode = HSU_AUDIO_MODE_MONO_SPK;
            break;
#endif /* FEATURE_MM_REC */
          default:
            audio_config_param.new_mode = HSU_AUDIO_MODE_MONO_SPK_MIC;
            break;
        }
        hsu_al_task_enqueue_cmd(hsu_analog_audio_config, (void*)(&audio_config_param));
      }
    }
    else
    {
      /* Check aux line in status */
      status = voc_check_aux_line_in();
      if(status == TRUE)
      {
        if(codec == VOC_CODEC_STEREO_USB)
        {
          audio_config_param.new_mode = HSU_AUDIO_MODE_STEREO_SPK;
        }
        else
        {
          audio_config_param.new_mode = HSU_AUDIO_MODE_MONO_SPK;
        }
        hsu_al_task_enqueue_cmd(hsu_analog_audio_config, (void*)(&audio_config_param));
      }
    }
  } else {
    if(sndhw_usb_active == TRUE) {
      sndhw_usb_active = FALSE;
      audio_config_param.new_mode = HSU_AUDIO_MODE_USB;
      hsu_al_task_enqueue_cmd(hsu_analog_audio_config, (void*)(&audio_config_param));
    }
  }

}

#endif /* FEATURE_USB_CARKIT*/

#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_register_aux_ctl

DESCRIPTION
  This function will register the function provided by the client
  to use in the external generator action

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  sndhw_aux_ctl_fn

===========================================================================*/
/* Define the function pointer for controlling the auxilliary generator  */
/* In this case, turn the vibrator on and off                            */
void sndhw_register_aux_ctl( snd_aux_func_ptr_type aux_ctl_fn_ptr)
{
  sndhw_aux_ctl_fn = aux_ctl_fn_ptr;
}
/*===========================================================================
FUNCTION snd_setup_voice_path

DESCRIPTION
  Setup the state of an audio output path.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void snd_setup_voice_path
(
  snd_device_type       device,      /* Setting for physical path */
  snd_mute_control_type mute,        /* Mute setting              */
  snd_pstate_type       *path_state  /* Output: path state        */
)
{
  /* setup path */
  path_state->device  = device;
  path_state->method  = SND_METHOD_VOICE;
  path_state->mute    = mute;

  if(mute == SND_MUTE_MUTED) {
    /* If path is muted, set volume to zero */
    path_state->vol = 0;
  } else {
    /* If path is unmuted, set volume to database value */
    path_state->vol = snd_data_get_volume(device, SND_METHOD_VOICE);
  }
} /* end of snd_setup_voice_path */

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_rx_mute_ctl

DESCRIPTION
  Temporary mute/unmute of the audio input from the CDMA/FM receive path.
  This is the low level hardware interface that actually directs the
  hardware to mute.

DEPENDENCIES
  See snd_rx_mute_ctl().  Logic exists at that level to allow multiple
  mute requestors.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_rx_mute_ctl
(
  boolean mute_audio
    /* True = mute audio, False = un-mute.                              */
)
{
  voc_status_type voc_status;
    /* Temporary for checking voc_call() return status.                 */

  /* The Audio Switch that controls this muting is in the Vocoder.      */
  MSG_MED("Mute RX audio: %d", mute_audio, 0, 0);

  voc_status = voc_rx_mute_cmd(mute_audio);
  if (voc_status != VOC_DONE_S) {
    ERR("voc_rx_mute_cmd(%d) Error=%d",mute_audio,voc_status,0);
  }

} /* end of sndhw_rx_mute_ctl */

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_rx_dtmf_override_ctl

DESCRIPTION
  Temporary mute/unmute of the audio input from the CDMA/FM receive path
  during DTMF playback.

DEPENDENCIES
  See snd_rx_mute_ctl().  Logic exists at that level to allow multiple
  mute requestors.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_rx_dtmf_override_ctl
(
  boolean mute_audio
    /* True = mute audio, False = un-mute.                              */
)
{
  voc_status_type voc_status;
    /* Temporary for checking voc_call() return status.                 */

  /* The Audio Switch that controls this muting is in the Vocoder.      */
  MSG_MED("Mute RX audio for DTMF: %d", mute_audio, 0, 0);

  voc_status = voc_rx_dtmf_override_cmd(mute_audio);
  if (voc_status != VOC_DONE_S) {
    ERR("voc_rx_dtmf_override_cmd(%d) Error=%d",mute_audio,voc_status,0);
  }

} /* end of sndhw_rx_dtmf_override_ctl */

/* <EJECT>*/
/*===========================================================================

REGIONAL FUNCTION sndhw_aud_process_ctl

DESCRIPTION
  Controls whether ESEC (Ear-Seal Echo Cancellation) is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If aud-in or aud-out is muted, the associated switches are set to mute.
  Sets snd_vox_mode.

===========================================================================*/
void sndhw_aud_process_ctl( void )
{
  	if(!sndhw_agc_on){
		sndhw_smartsnd_ctl(SND_SMARTSND_RVE_AGC_AIG_ENABLED);
	}
	else {
		if(sndhw_avc_on)
		{
			sndhw_smartsnd_ctl(SND_SMARTSND_AVC_AGC_AIG_ENABLED);
		}
		else
		{
			sndhw_smartsnd_ctl(SND_SMARTSND_AGC_ENABLED_AIG_ENABLED);
		}
	}
} /* end of sndhw_aud_process_ctl */


/* <EJECT>*/
#ifdef FEATURE_AUDIO_AGC
/*==========================================================================
 FUNCTION sndhw_smartsnd_aud_agc_process_ctl

 DESCRIPTION
   Controls AGC,AVC enabling.

 DEPENDENCIES
   sndhw_smartsnd_status

 RETURN VALUE
   none

 SIDE EFFECTS
   none

 ==========================================================================*/

void sndhw_smartsnd_aud_agc_process_ctl( void )
{
  voc_status_type  voc_status;
  static voc_smartsnd_type 		current_agc_cmd = VOC_SMARTSND_INVALID;
  static snd_device_type        current_device  = SND_DEVICE_MAX;
  voc_smartsnd_type 			agc_cmd;

  switch(sndhw_smartsnd_status)
  {
   case SND_SMARTSND_NONE:
       agc_cmd = VOC_SMARTSND_NONE;
       break;

   case   SND_SMARTSND_AGC_ENABLED_AIG_ENABLED:
       agc_cmd = VOC_SMARTSND_AGC_AIG;
       break;

   case  SND_SMARTSND_AGC_ENABLED_AIG_DISABLED:
       agc_cmd = VOC_SMARTSND_AGC;
       break;

   case SND_SMARTSND_AVC_AGC_AIG_ENABLED:
       agc_cmd = VOC_SMARTSND_AGC_AVC_AIG;
       break;

   case SND_SMARTSND_AVC_AGC_ENABLED_AIG_DISABLED:
       agc_cmd = VOC_SMARTSND_AGC_AVC;
       break;

   case SND_SMARTSND_AVC_ENABLED_AGC_AIG_DISABLED:
       agc_cmd = VOC_SMARTSND_AVC;
       break;

   case SND_SMARTSND_RVE_AGC_AIG_ENABLED:
       agc_cmd = VOC_SMARTSND_AGC_AIG;
       break;

   case SND_SMARTSND_RVE_AGC_ENABLED_AIG_DISABLED:
       agc_cmd = VOC_SMARTSND_AGC;
       break;

   case SND_SMARTSND_RVE_ENABLED_AGC_AIG_DISABLED:
       agc_cmd = VOC_SMARTSND_NONE;
       break;

   default:
       MSG_HIGH("sndhw_smartsnd_aud_agc_process_ctl(void), wrong smartsnd =%d", sndhw_smartsnd_status,0,0);
       return;
  }

  if((agc_cmd        != current_agc_cmd) ||
     (current_device != snd_crnt_path_out.device)) {
    MSG_MED("Calling AGC fn: %d, device: %d.",
               agc_cmd, snd_crnt_path_out.device, 0);
    current_agc_cmd = agc_cmd;
    current_device  = snd_crnt_path_out.device;

    voc_status = voc_smartsnd_rx_agc_cmd(current_agc_cmd);

    if (voc_status != VOC_DONE_S) {
      ERR("voc_agc_cmd(%d) Error=%d",current_agc_cmd,voc_status,0);
   }
  }
}

#endif /*FEATURE_AUDIO_AGC*/



/* <EJECT>*/
/*==========================================================================
 FUNCTION  sndhw_smartsnd_aud_rve_process_ctl

 DESCRIPTION
   Controls RVE enabling.

 DEPENDENCIES
   sndhw_smartsnd_status

 RETURN VALUE
   none

 SIDE EFFECTS
   none

 ==========================================================================*/

 void sndhw_smartsnd_aud_rve_process_ctl( void )
 {
  static voc_smartsnd_type 	current_rve_cmd = VOC_SMARTSND_INVALID;
  voc_smartsnd_type 	rve_cmd;

   switch(sndhw_smartsnd_status)
   {
   case SND_SMARTSND_NONE:
   case   SND_SMARTSND_AGC_ENABLED_AIG_ENABLED:
   case  SND_SMARTSND_AGC_ENABLED_AIG_DISABLED:
   case SND_SMARTSND_AVC_AGC_AIG_ENABLED:
   case SND_SMARTSND_AVC_AGC_ENABLED_AIG_DISABLED:
   case SND_SMARTSND_AVC_ENABLED_AGC_AIG_DISABLED:
       rve_cmd = VOC_SMARTSND_RVE_NONE;
       break;
   case SND_SMARTSND_RVE_AGC_AIG_ENABLED:
   case SND_SMARTSND_RVE_AGC_ENABLED_AIG_DISABLED:
   case SND_SMARTSND_RVE_ENABLED_AGC_AIG_DISABLED:
       rve_cmd = VOC_SMARTSND_RVE_AVAIL;
       break;
   default:
       MSG_HIGH("sndhw_smartsnd_aud_rve_process_ctl(void), wrong smartsnd =%d", sndhw_smartsnd_status,0,0);
       return;
  }
   if(rve_cmd != current_rve_cmd)
   {
	 current_rve_cmd = rve_cmd;
     MSG_MED("Calling RVE fn: %d, device: %d.", rve_cmd, 0, 0);
     voc_smartsnd_set_rve_mode(rve_cmd);
   }
 }



/* <EJECT>*/
/*==========================================================================

 FUNCTION sndhw_smartsnd_aud_process_ctl


 DESCRIPTION
   Controls whether ESEC (Ear-Seal Echo Cancellation) is enabled or not.
   Also controls RVE and AGC.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   If aud-in or aud-out is muted, the associated switches are set to mute.
   Sets snd_vox_mode.

 ===========================================================================*/

void sndhw_smartsnd_aud_process_ctl( void )
{
  voc_status_type               voc_status;
    /* Save voc_xxx return value for Error Processing */
    /* Set vocoder VOX override if BIO_SW10 is ACTIVE.                  */
  boolean aud_process_override_input = (BIO_GET_SW(BIO_SW10_M)==BIO_SW10_M);
  sndhw_smartsnd_aud_rve_process_ctl();

  voc_status = voc_aud_process_override(aud_process_override_input);
  if (voc_status != VOC_DONE_S) {
    ERR("voc_aud_process_override(%d) Error=%d",
        aud_process_override_input,voc_status,0);
  }

#ifdef FEATURE_AUDIO_AGC
  sndhw_smartsnd_aud_agc_process_ctl();
#endif /*FEATURE_AUDIO_AGC*/
}



/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_voc_release

DESCRIPTION
  This routine is called when conditions have changed to see if it would be
  appropriate to release the Vocoder.  If so, the Vocoder is released,
  otherwise no action is taken.

DEPENDENCIES
  snd_crnt_path_in
  snd_crnt_path_out
  snd_voc_is_acquired

RETURN VALUE
  None

SIDE EFFECTS
  snd_voc_is_acquired
  snd_voc_is_ok
  snd_path_is_ready

===========================================================================*/
LOCAL void sndhw_voc_release( void )
{

  /* We want to keep the vocoder as long as we are doing ANY of the     */
  /* following:                                                         */
  /* 1. Generating a tone                                               */
  /* 2. Have any Unmuted Paths                                          */
  /* 3. Are using Codec Clocks to Mute the Car Stereo.                  */

  if (sndhw_keep_voc_acquired) {
    return;
  }
#ifdef FEATURE_MIDI_OUT
  if (sndhw_midi_out_enabled) {
    return;
  }
#endif /* FEATURE_MIDI_OUT */
#ifdef FEATURE_PCM_REC
  if(sndhw_pcm_rec_reverse_enabled || sndhw_pcm_rec_forward_enabled) {
    return;
  }
#endif /* FEATURE_PCM_REC */
  if (snd_crnt_path_out.mute == SND_MUTE_UNMUTED) {
    return;
  }
  if (sndhw_crnt_path_in_mute == SND_MUTE_UNMUTED ) {
    return;
  }

#if (defined(SND_HFK) && !defined(SND_HFK_PLUS))
  if (sndhw_stereo_mute_s) {
    return;
  }
#endif /* SND_HFK && !SND_HFK_PLUS */

#ifdef SND_EXTCOM
#error code not present
#endif

  /* If we have already released, then don't do it again.               */
  if (!snd_voc_acquired) {
    return;
  }

  /* Looks like we don't need the vocoder for now.
  */
  MSG_HIGH("Releasing vocoder",0,0,0);

  /* Turn off the path timer and clear SND_PATH_TIMER_SIG. Indicate
  ** That no audio paths are ready.
  */
  (void) rex_clr_timer( &snd_path_timer );
  (void) rex_clr_sigs( &snd_tcb, SND_PATH_TIMER_SIG );
  snd_path_is_ready = SND_PATH_NOT_READY;

  snd_voc_is_ok     = FALSE;            /* Declare Vocoder "not okay"   */
  snd_voc_acquired  = FALSE;            /* Remember I don't have it     */

  voc_release(VOC_ACQ_SND);             /* Give Up the Vocoder          */

} /* end of sndhw_voc_release() */

/* <EJECT> */
#ifdef FEATURE_IMELODY
/*===========================================================================

FUNCTION snd_data_get_imy_ring_volume

DESCRIPTION
  Get the ringer volume for an iMelody tone.

DEPENDENCIES
  None.

RETURN VALUE
  The ringer volume in duty cycles.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 sndhw_adjust_imy_ring_volume (
  uint16            ring_vol,  /* Ringer volume level   */
  byte              imy_vol    /* iMelody volume level  */
)
{
  /* Step down by 4dB, for 15 IMY volume levels */
  return ring_vol + (15 - imy_vol) * 40;
}

/* <EJECT> */
/*===========================================================================

FUNCTION snd_data_get_imy_dtmf_volume

DESCRIPTION
  Get the dtmf volume for an iMelody tone.

DEPENDENCIES
  None.

RETURN VALUE
  The dtmf volume in dbm

SIDE EFFECTS
  None.

===========================================================================*/
sint15 sndhw_adjust_imy_dtmf_volume (
  sint15            dtmf_vol,  /* DTMF master volume level */
  byte              imy_vol    /* iMelody volume level     */
)
{
  /* Step down by 4dB, for 15 IMY volume levels */
  return dtmf_vol + (imy_vol - 15)* 40;
}

/*===========================================================================

FUNCTION sndhw_midi_to_dtmf

DESCRIPTION
  Converts midi commands to a form that is suitable for use with the
  ringer/vocoder.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void sndhw_midi_to_dtmf
(
  audmain_cmd_buf_type *audfmt_cmd_buf,
  uint16               *freq_hi_ptr,
  uint16               *freq_lo_ptr,
  uint16               *duration_ptr
)
{
  byte    index  = 0;
  uint16* cmd_buf = audfmt_cmd_buf->midi_cmd[0].command_buf;
  boolean sound_on = FALSE;

  if(audfmt_cmd_buf->midi_cmd[0].num_commands) {
    while(index < audfmt_cmd_buf->midi_cmd[0].num_words) {

      switch(cmd_buf[index]) {
      case 0x90: /* Note ON */
        *freq_hi_ptr = snd_midi_note_to_freq[cmd_buf[index+2] - 21];
        *freq_lo_ptr = *freq_hi_ptr;
        index += 4;
        sound_on = TRUE;
        break;

      case 0x80: /* Note OFF */
      case 0xb0: /* Volume */
        index += 4;
        break;

      case 0xff: /* Reset */
        index++;
        break;

      case 0xc0: /* Program change */
        index += 3;
        break;

      default:
        MSG_ERROR("Unknown cmd type %d at %d", cmd_buf[index], (int)index, 0);
        return;
      }
    }

    *duration_ptr = (sound_on == TRUE)? SND_MAX_PERIOD : 0;
  }
}
#endif /* FEATURE_IMELODY */

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_voc_tone

DESCRIPTION
  Send a command to the vocoder to have it start generating a tone.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_voc_tone
(
  tone_status_type *tone_status_ptr
)
{
  static boolean voc_tone_playing = FALSE;
    /* Vocoder is generating a tone      */
  uint16 tone_hi;
    /* Holds the Vocoder's "Hi" DTMF Value */
  uint16 tone_lo;
    /* Holds the Vocoder's "Lo" DTMF Value */
  uint16 duration;
    /* Duration of the tone, 0 = stop tone */
  uint8  vol_index;
  sint15 volume;
    /* Local copy of the DTMF volume       */
  voc_status_type voc_status;
    /* Save voc_xxx() return status in case Error Processing is needed */

  if (tone_status_ptr != NULL) {
#ifdef FEATURE_IMELODY
    if(tone_status_ptr->type == SND_TYPE_IMY) {
      sndhw_midi_to_dtmf(tone_status_ptr->audfmt_buf,
                         &tone_hi, &tone_lo, &duration);
    } else
#endif
    {
      tone_hi  = tone_status_ptr->freq_pair.freq_hi;
      tone_lo  = tone_status_ptr->freq_pair.freq_lo;
      duration = (uint16) tone_status_ptr->time_remain_ms;
    }

    if (duration > SND_MAX_PERIOD) {
      duration = SND_MAX_PERIOD;
    }
    /* If not playing tone locally or tone is playing from ringer, mute
    ** vocoder tone generation.
    */
    if ((tone_status_ptr->dtmf_path == SND_APATH_TX) ||
        (snd_data_get_generator(tone_status_ptr->device,
                                tone_status_ptr->method) != SND_GEN_VOC)) {
      volume = VOC_VOL_SILENCE;
    } else {
      vol_index   = snd_crnt_path_out.vol;
      if(vol_index == SND_VOL_USE_DEFAULT) {
        /* If flag indicates to use database volume setting, retrieve from DB */
        vol_index = snd_data_get_volume(snd_crnt_path_out.device,
                                        snd_crnt_path_out.method);
      }
      volume   = snd_data_get_dtmf_volume(snd_crnt_path_out.device,
                                          snd_crnt_path_out.method,
                                          vol_index);
    }
#ifdef FEATURE_IMELODY
    if(tone_status_ptr->type == SND_TYPE_IMY) {
      volume = sndhw_adjust_imy_dtmf_volume(volume,
                                       tone_status_ptr->audfmt_buf->imy_volume);
    }
#endif

  } else {
    /* If no tone is currently playing then no need to shut off vocoder
    ** tone generator.
    */
    if(!voc_tone_playing) {
      return;
    }
    tone_hi  = 0;
    tone_lo  = 0;
    duration = 0;
    volume   = VOC_VOL_SILENCE;
  }
  /* If this is not a "stop" request, then try to have the tone extend  */
  /* for overlap, if we can't, we can't.  Overlap is needed for         */
  /* continuing tones, so that the tone does not get stopped by the     */
  /* vocoder and then quickly started again.  This can cause a 'clip'   */
  /* sound.  In any case, we also need to "acquire" to Vocoder (if we   */
  /* haven't already).                                                  */

  /* In analog mode, we need to handle a limitation of the early MSM2P. */
  /* There is no working DTMF over-write mode. Once a tone is started,  */
  /* it will complete. So, if we are already playing a tone and it      */
  /* isn't the same as the current tone, do not add any overlap.        */
  /* To avoid drop-outs, add one overlap  when switching to busy from   */
  /* not busy.                                                          */

  {
    if (duration != 0) {
      if ( duration <= ((word) 0xffff - SND_VOC_GRAIN) ) {
        duration += SND_VOC_GRAIN;
      } else {
        MSG_MED("Voc Tone duration too long for extension",0,0,0);
      }
    }
  }

  /* Ask Vocoder to Make a Vocoder tone                                 */

  voc_status = voc_dtmf_cmd
  (
    tone_hi,                    /* first tone to generate  */
    tone_lo,                    /* second tone to generate */
    volume,                     /* volume to use           */
    duration                    /* duration of tones       */
  );

  /* Check the Vocoder's response.                                      */

  if (voc_status != VOC_DONE_S) {
    ERR("voc_dtmf_cmd(...) Error=(%x)", voc_status, 0, 0 );
  }

  /* By looking at the duration, determine whether we just turned 'on'  */
  /* or 'off' the vocoder's DTMF generator.  If we are turning if off   */
  /* then "release" the vocoder.                                        */

  if (duration != 0) {
    voc_tone_playing = TRUE;
  } else {
    MSG_MED("Zero duration tone",0,0,0);
    voc_tone_playing  = FALSE;
    /* Allow some time for the DTMF waveform to gracefully reach a
     * quiescent state before we release the DSP - helps reduce clicks
     * at the end of the tone. */
    rex_sleep(1);
    sndhw_voc_release();                /* Release it (maybe)           */
  }

} /* end of sndhw_voc_tone */
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_ring

DESCRIPTION
  Start a DTMF emitting from the ringer.

DEPENDENCIES
  sndrng_dtmf

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void sndhw_ring
(
  tone_status_type *tone_status_ptr
)
{
  static boolean ring_tone_playing = FALSE;
    /* Ringer is generating a tone?     */
  word freq_a;
    /* Freq to Generate in side "A" */
  word freq_b;
    /* Freq to Generate in side "B" */
  int  ring_vol;
    /* The table value for ringer volume level */
  uint16 duration;
    /* Duration of the tone, 0 = stop tone */
  byte volume = 0;

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)

  if (sndhw_bt_cb_func_ptr != NULL)
  {
    if (tone_status_ptr != NULL)
    {
      if((!ring_tone_playing)&& (!sndhw_bt_playback_other_started))
      {
        MSG_MED("Calling BT callback with playback OTHER START", 0,0,0);
        sndhw_bt_cb_func_ptr(VOC_PLAYBACK_OTHER);
        sndhw_bt_playback_other_started=TRUE;
      }
    }
  }
#endif/*AVS_BT_SCO_REWORK*/

  if ( tone_status_ptr == NULL ) {
    if ( !ring_tone_playing ) {
      return;
    }
  } else {
    volume = tone_status_ptr->vol;
    if(volume == SND_VOL_USE_DEFAULT) {
      /* If flag indicates to use database volume setting, retrieve from DB */
      volume = snd_data_get_volume(tone_status_ptr->device,
                                   tone_status_ptr->method);
    }
  }

  /* If we are muted or tone_status_ptr == NULL, turn off ringer and return.
  */
  if (( tone_status_ptr == NULL ) || ( volume == 0 )) {

#ifndef FEATURE_NO_EXT_RINGER
    ring_tone(0,0,0,0);
#endif

    ring_tone_playing = FALSE;      /*  The ringer has now been switched off  */
    return;
  }

#ifdef FEATURE_IMELODY
  if(tone_status_ptr->type == SND_TYPE_IMY) {
    sndhw_midi_to_dtmf(tone_status_ptr->audfmt_buf, &freq_a, &freq_b,
                       &duration);
  } else
#endif
  {
    freq_a  = tone_status_ptr->freq_pair.freq_hi;
    freq_b  = tone_status_ptr->freq_pair.freq_lo;
    duration = (uint16) tone_status_ptr->time_remain_ms;
  }

  /* Shift factors (or duty cycles) for the ringer duty cycle are
  ** stored in a database. Use the volume level to get the database
  ** value.
  */
  ring_vol = snd_data_get_ringer_volume(tone_status_ptr->device,
                                        tone_status_ptr->method, volume );

#ifdef FEATURE_IMELODY
  if(tone_status_ptr->type == SND_TYPE_IMY) {
    ring_vol = sndhw_adjust_imy_ring_volume(ring_vol,
                                       tone_status_ptr->audfmt_buf->imy_volume);
  }
#endif

#ifndef FEATURE_NO_EXT_RINGER
  ring_tone(freq_a, freq_b, ring_vol, duration);
  /* Note that the ringer circuit is busy now.                          */
#endif
  ring_tone_playing = TRUE;

} /* end of sndhw_ring */
/* <EJECT>*/
#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
/* <EJECT>*/
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION sndhw_get_audfmt_need_config

DESCRIPTION
  Get the status whether audfmt configuration is needed or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - Audfmt configuration is not done
  FALSE - Audfmt configuration is done

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_get_audfmt_need_config()
{
  /* return whether audfmt config is needed or not*/
  return (sndhw_audfmt_need_config);
}
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT>*/
#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
/*===========================================================================

FUNCTION sndhw_set_sample_rate

DESCRIPTION
  Send sample rate command to the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_set_sample_rate (
  snd_sample_rate_type sample_rate
) {
  voc_sample_rate_type v_sample_rate = VOC_SAMPLE_RATE_8000;

  switch(sample_rate) {
    case SND_SAMPLE_RATE_8000:
      v_sample_rate = VOC_SAMPLE_RATE_8000;
      break;

    case SND_SAMPLE_RATE_11025:
      v_sample_rate = VOC_SAMPLE_RATE_11025;
      break;

    case SND_SAMPLE_RATE_12000:
      v_sample_rate = VOC_SAMPLE_RATE_12000;
      break;

    case SND_SAMPLE_RATE_16000:
      v_sample_rate = VOC_SAMPLE_RATE_16000;
      break;

    case SND_SAMPLE_RATE_22050:
      v_sample_rate = VOC_SAMPLE_RATE_22050;
      break;

    case SND_SAMPLE_RATE_24000:
      v_sample_rate = VOC_SAMPLE_RATE_24000;
      break;

    case SND_SAMPLE_RATE_32000:
      v_sample_rate = VOC_SAMPLE_RATE_32000;
      break;
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
    case SND_SAMPLE_RATE_44000:
      v_sample_rate = VOC_SAMPLE_RATE_44000;
      break;
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
    case SND_SAMPLE_RATE_44100:
      v_sample_rate = VOC_SAMPLE_RATE_44100;
      break;

    case SND_SAMPLE_RATE_48000:
      v_sample_rate = VOC_SAMPLE_RATE_48000;
      break;

#ifdef FEATURE_AAC
    case SND_SAMPLE_RATE_64000:
    case SND_SAMPLE_RATE_88200:
    case SND_SAMPLE_RATE_96000:
#endif /* FEATURE_AAC        */

    default:
      break;
  }

  return voc_sample_rate(v_sample_rate);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_voc_config

DESCRIPTION
  Send voc_config() command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_voc_config (
  snd_audfmt_source_type source,
  voc_capability_type    config
) {
#if defined(FEATURE_AUDIO_FORMAT) && defined(FEATURE_MIDI_OUT)
  static voc_capability_type current_config[SND_AUDFMT_SOURCE_MAX] =
                                  {VOC_CAP_NONE, VOC_CAP_NONE};

  for(sndhw_current_index=0; (sndhw_current_index != SND_ARRAY_SIZE) &&
							  !snd_array[sndhw_current_index].active; ++sndhw_current_index);
  sndhw_current_source = source;

  current_config[source] = config;



  if(current_config[SND_AUDFMT_SOURCE_NORMAL] != VOC_CAP_NONE) {
	  sndhw_current_capability = current_config[SND_AUDFMT_SOURCE_NORMAL];
    (void) voc_config(VOC_ACQ_SND, current_config[SND_AUDFMT_SOURCE_NORMAL]);
  } else if(current_config[SND_AUDFMT_SOURCE_MIDI_OUT] != VOC_CAP_NONE) {
	  sndhw_current_capability = current_config[SND_AUDFMT_SOURCE_MIDI_OUT];
    (void) voc_config(VOC_ACQ_SND, current_config[SND_AUDFMT_SOURCE_MIDI_OUT]);
#else
    sndhw_current_capability = config;
  if(config != VOC_CAP_NONE) {
    (void) voc_config(VOC_ACQ_SND, config);
#endif /* defined(FEATURE_AUDIO_FORMAT) && defined(FEATURE_MIDI_OUT) */
  } else {
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
    (void) voc_adec_init(NULL, NULL, NULL);
    sndhw_adec_client_data = NULL;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */
#ifdef FEATURE_ADEC
    sndhw_audfmt_adec_flush(0xFFFF);
#endif /* FEATURE_ADEC */
    sndhw_current_capability = config;
    (void) voc_config(VOC_ACQ_SND, VOC_CAP_NONE);
  }
  (void) rex_clr_timer( &snd_voc_module_timer );
}
#endif /* defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT) */

/* <EJECT>*/
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION sndhw_set_av_sync

DESCRIPTION
  Set av sync related parameters

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_set_av_sync (
  voc_av_sync_dec_enum_type    decoder_type,
  snd_av_sync_int_cb_ptr_type  av_sync_cb,
  snd_av_sync_interval_type    av_sync_mode,
  uint32                       av_sync_interval
) {
 if((decoder_type >= VOC_AV_SYNC_DEC_INVALID) || (av_sync_cb ==  NULL) ||
    (av_sync_interval == 0) || (av_sync_mode >= SND_AV_SYNC_INVALID)) {
   voc_set_av_sync(VOC_AV_SYNC_DEC_INVALID, 0, VOC_AV_SYNC_INVALID, NULL);
 } else {
   voc_set_av_sync(decoder_type, av_sync_interval,
                   (voc_av_sync_interval_type) av_sync_mode,
                   (voc_av_sync_cb_type) av_sync_cb);
 }
}

/*===========================================================================

FUNCTION sndhw_audfmt_cleanup

DESCRIPTION
  Unconfigures vocoder after midi is finished.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_audfmt_cleanup ()
{
  sndhw_voc_config(SND_AUDFMT_SOURCE_NORMAL, VOC_CAP_NONE); /* Unconfig DSP */
#ifdef FEATURE_WAVE_PB
  voc_set_adpcm_mode(FALSE);
#ifdef FEATURE_VOC_G711
  voc_set_wav_mode(VOC_ADPCM_INVALID_MODE);
#endif /* FEATURE_VOC_G711 */
#endif /* FEATURE_WAVE_PB */
  sndhw_set_av_sync(VOC_AV_SYNC_DEC_INVALID, NULL, SND_AV_SYNC_INVALID,
                    NULL);
#if defined(FEATURE_AUDIO_FORMAT) && \
    (defined(FEATURE_AVS_INCALL_ADEC_PB) || \
     defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))

  voc_set_audfmt_mix_mode(VOC_ADEC_MIX_MODE_NONE);
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
#endif /* FEATURE_AUDIO_FORMAT &&
          (FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB) */

  sndhw_audfmt_need_config = TRUE;

#ifdef FEATURE_AAC
  voc_aac_reset_song_config();
#endif /* FEATURE_AAC */

#ifdef FEATURE_WMA
  voc_wma_reset_song_config();
#endif /* FEATURE_WMA */

#ifdef FEATURE_REAL_AUDIO
  voc_ra_reset_song_config();
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
  voc_amr_wbp_reset_song_config();
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

}

/* <EJECT>*/
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION sndhw_audfmt_tone

DESCRIPTION
  Send midi commands to the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_tone (
  tone_status_type *tone_status_ptr
) {
  voc_midi_cmd_type cmd_buf[SNDHW_MIDI_NUM_SEQ];
  voc_status_type   voc_status;
  snd_gen_type      gen;
  uint8             volume;
  sint15            volume_db;
  uint32            counter;

  audmain_midi_cmd_buf_type *src_cmd;
  voc_midi_cmd_type         *dest_cmd;

  if(tone_status_ptr != NULL) {
    volume   = tone_status_ptr->vol;
    if(volume == SND_VOL_USE_DEFAULT) {
      /* If flag indicates to use database volume setting, retrieve from DB */
      volume = snd_data_get_volume(tone_status_ptr->device,
                                      tone_status_ptr->method);
    }

    if(volume != 0) {
      /* Get the current generator, if not GEN_MIDI then set volume to zero
      ** because audio format is not highest priority sound
      */
      gen = snd_data_get_generator(tone_status_ptr->device,
                                   tone_status_ptr->method);
      if(gen != SND_GEN_MIDI) {
        volume = 0;
      }
    }

    if(volume != 0) {
      volume_db = snd_data_get_midi_volume(tone_status_ptr->device,
                                           tone_status_ptr->method,
                                           volume);

#ifdef FEATURE_WEBAUDIO
      if(sndhw_aux_ctl_fn != NULL) {
        if(tone_status_ptr->audfmt_buf->vib) {
          sndhw_aux_ctl_fn(TRUE);  /* Turn on the auxiliary generator        */
        } else if(!sndhw_aux_busy) {
          sndhw_aux_ctl_fn(FALSE); /* Turn off the auxiliary generator       */
        }
      }
#endif
    } else {
      volume_db = VOC_VOL_SILENCE;

#ifdef FEATURE_WEBAUDIO
      if((!sndhw_aux_busy) && (sndhw_aux_ctl_fn != NULL)) {
        sndhw_aux_ctl_fn(FALSE); /* Turn off the auxiliary generator       */
      }
#endif
    }

    if(tone_status_ptr->audfmt_buf != NULL) {
      dest_cmd = cmd_buf;
      src_cmd  = tone_status_ptr->audfmt_buf->midi_cmd;
      for(counter = 0; counter < SNDHW_MIDI_NUM_SEQ; counter++) {
        dest_cmd->num_words    = src_cmd->num_words;
        dest_cmd->num_commands = src_cmd->num_commands;
        dest_cmd->command_buf  = src_cmd->command_buf;
        dest_cmd++;
        src_cmd++;
      }

      /* Send midi commands */
      voc_status = voc_midi_cmd(cmd_buf, SNDHW_MIDI_NUM_SEQ, volume_db);

      switch(voc_status) {
        case VOC_DONE_S:
          tone_status_ptr->audfmt_buf->start_aging  = TRUE;
          /* fall thru */

        case VOC_CMD_FAILED_S:
          /* Clear command buffer */
          dest_cmd = cmd_buf;
          src_cmd  = tone_status_ptr->audfmt_buf->midi_cmd;
          for(counter = 0; counter < SNDHW_MIDI_NUM_SEQ; counter++) {
            if(dest_cmd->num_words == 0) {
              src_cmd->num_words    = 0;
              src_cmd->num_commands = 0;
            }
            dest_cmd++;
            src_cmd++;
          }
          break;

        case VOC_BADFMT_S:
          return(TRUE);
      }
    }
  } else {
    /* Mute the midi generator */
    voc_status = voc_midi_cmd(NULL, 0, VOC_VOL_SILENCE);

#ifdef FEATURE_WEBAUDIO
    if((!sndhw_aux_busy) && (sndhw_aux_ctl_fn != NULL)) {
      sndhw_aux_ctl_fn(FALSE); /* Turn off the auxiliary generator       */
    }
#endif
  }

  return(FALSE);

}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_audfmt_cmd

DESCRIPTION
  Send midi commands to the vocoder with zero volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_audfmt_cmd (
  audmain_cmd_buf_type *cmd_buf
) {
  voc_midi_cmd_type dest_buf;

  dest_buf.num_words    = cmd_buf->midi_cmd[0].num_words;
  dest_buf.num_commands = cmd_buf->midi_cmd[0].num_commands;
  dest_buf.command_buf  = cmd_buf->midi_cmd[0].command_buf;
  (void) voc_midi_cmd(&dest_buf, 1, VOC_VOL_SILENCE);

  /* Clear command buffer */
  cmd_buf->midi_cmd[0].num_words    = 0;
  cmd_buf->midi_cmd[0].num_commands = 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_audfmt_synth_mvol

DESCRIPTION
  This function sets the synth master volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_synth_mvol (
  uint16 vol
)
{
  boolean ret_val = FALSE;

  if(voc_synth_mvol(vol) == VOC_DONE_S) {
    ret_val = TRUE;
  }

  return(ret_val);
}
#endif /* FEATURE_MIDI */

/* <EJECT>*/
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION sndhw_audfmt_adec_mvol

DESCRIPTION
  This function sets the ADEC master volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_adec_mvol (
  uint16 vol
)
{
  boolean ret_val = FALSE;

  if(voc_adec_mvol(vol) == VOC_DONE_S) {
    ret_val = TRUE;
  }

  return(ret_val);
}
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION sndhw_audfmt_adec_chan_vol

DESCRIPTION
  Transfers the MIDI command buffer to the QDSP for processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_adec_chan_vol (
  uint32 channel,
  uint16 vol
)
{
  boolean ret_val = FALSE;

  if(voc_adec_chan_vol(channel, vol) == VOC_DONE_S) {
    ret_val = TRUE;
  }

  return(ret_val);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_audfmt_adec_chan_pan

DESCRIPTION
  Transfers the MIDI command buffer to the QDSP for processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_adec_chan_pan (
  uint32 channel,
  sint15 pan
) {
  boolean ret_val = FALSE;

  if(voc_adec_chan_pan(channel, pan) == VOC_DONE_S) {
    ret_val = TRUE;
  }
  return(ret_val);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_audfmt_adec_chan_pause

DESCRIPTION
  Transfers the MIDI command buffer to the QDSP for processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_adec_chan_pause (
  uint32  channel,
  boolean pause_flag
) {
  boolean ret_val = FALSE;

  if(voc_adec_chan_pause(channel, pause_flag) == VOC_DONE_S) {
    ret_val = TRUE;
  }
  return(ret_val);
}
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT>*/
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION sndhw_set_adec_mode

DESCRIPTION
  This function setup the QSynth audio decoder mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_set_adec_mode(
  snd_set_adec_mode_enum_type mode
) {
  if(mode == SND_SET_ADEC_MODE_ADPCM) {
    voc_set_adec_mode(VOC_QSYNTH_ADEC_MODE_ADPCM);
  } else if(mode == SND_SET_ADEC_MODE_YADPCM) {
    voc_set_adec_mode(VOC_QSYNTH_ADEC_MODE_YADPCM);
#ifdef FEATURE_QSYNTH_PCM
  } else if(mode == SND_SET_ADEC_MODE_PCM) {
    voc_set_adec_mode(VOC_QSYNTH_ADEC_MODE_PCM);
#endif /* FEATURE_QSYNTH_PCM */
  } else {
    voc_set_adec_mode(VOC_QSYNTH_ADEC_MODE_QCP);
  }
}
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

/* <EJECT>*/
#ifdef FEATURE_ADEC
/*===========================================================================

FUNCTION sndhw_audfmt_adec_flush

DESCRIPTION
  Resets Audio decoder playback buffers and internal states in the QDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_audfmt_adec_flush ( uint16 flush_mask )
{
  (void) voc_adec_flush( flush_mask );
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_audfmt_adec_pcm_stopped

DESCRIPTION
  This function checks if PCM output has stopped.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if PCM output has stopped.

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_adec_pcm_stopped()
{
  return(voc_adec_pcm_stopped());
}
#endif /* FEATURE_ADEC */

/* <EJECT>*/
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/*===========================================================================

FUNCTION sndhw_audfmt_adec_reset

DESCRIPTION
  Resets Audio decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_audfmt_adec_reset ( uint32 reset_mask )
{
  voc_adec_reset( reset_mask );
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_audfmt_adec_pause

DESCRIPTION
  Pauses Audio decoder playback and interrupts in the qdsp.  Updates the
  QDSP_audioPause variable in DSP which is specific to each sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_audfmt_adec_pause (
  boolean pause
) {
  (void) voc_adec_pause(pause);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_audfmt_adec_data

DESCRIPTION
  Moves data from Audfmt/Sound task to Audio decoder playback buffers in QDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type sndhw_audfmt_adec_data(
  snd_adec_mode_type adec_mode,
  voc_adec_cmd_type  *adec_cmd
) {
  voc_adec_mode_type adec_type = VOC_ADEC_MODE_MAX;
  voc_status_type    voc_status;
  snd_status_type    ret_status = SND_SUCCESS;
#ifdef FEATURE_QSYNTH_COMMON
  if(adec_mode == SND_ADEC_MODE_QSYNTH) {
    adec_type = VOC_ADEC_MODE_QSYNTH;
  }
#endif

#ifdef FEATURE_QTUNES_COMMON
  if(adec_mode == SND_ADEC_MODE_QTUNES) {
    adec_type = VOC_ADEC_MODE_QTUNES;
  }
#endif

#ifdef FEATURE_ADPCM
  if(adec_mode == SND_ADEC_MODE_ADPCM) {
    adec_type = VOC_ADEC_MODE_ADPCM;
  }
#endif

#ifdef FEATURE_PCM
  if(adec_mode == SND_ADEC_MODE_PCM) {
    adec_type = VOC_ADEC_MODE_PCM;
  }
#endif

#ifdef FEATURE_VOC_G711
  if(adec_mode == SND_ADEC_MODE_WAV_MULAW) {
    adec_type = VOC_ADEC_MODE_WAV_MULAW;
  }
  if(adec_mode == SND_ADEC_MODE_WAV_ALAW) {
    adec_type = VOC_ADEC_MODE_WAV_ALAW;
  }
#endif /* FEATURE_VOC_G711 */

  voc_status = voc_adec_move_data(adec_type, adec_cmd);

  switch(voc_status){
    case VOC_DONE_S:
      break;
    case VOC_DSP_DATA_ERR_S:
      ret_status = SND_ABORT_DATA_ERR;
      break;
    case VOC_CMD_FAILED_S:
    default:
      ret_status = SND_ABORT;
      break;
  }

  return(ret_status);

}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_audfmt_adec_int_pending

DESCRIPTION
  This function checks for a pending ADEC interrupt.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if an interrupt is pending.

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_adec_int_pending()
{
  return(voc_adec_int_pending());
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_audfmt_stereo_pan

DESCRIPTION
  Issues a stereo pan command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_stereo_pan (
  uint8 pan_factor
) {
  boolean ret_val = FALSE;

  if(voc_master_pan(pan_factor) == VOC_DONE_S) {
    ret_val = TRUE;
  }

  return(ret_val);
}
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

/* <EJECT>*/
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION sndhw_qcp_record

DESCRIPTION
  This function sends a command to the vocoder driver to start
  recording QCP data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_qcp_record (
  snd_qcp_format_type      format,
  snd_qcp_dir_type         link,
  snd_mm_qcp_rec_para_type *rec_param,
  voc_rec_cb_type          cb_func,
  const void               *client_data
) {
  voc_rec_parm_type rec_parm;

  rec_parm.rec_interval_ms       = rec_param->report_int;
  rec_parm.auto_stop_ms          = rec_param->auto_stop_ms;
#ifdef FEATURE_MM_REC
  rec_parm.data_req_ms           = rec_param->data_req_ms;

  rec_parm.sync_cb_func          =
    (voc_rec_av_sync_cb_ptr) rec_param->sync_cb_func;

  rec_parm.client_data           = client_data;
#endif  /* FEATURE_MM_REC */
  rec_parm.settings.frame_format = VOC_PB_NATIVE_QCP;

  switch (format) {
#ifdef MSMAUD_VOC_4GV
    case SND_QCP_FORMAT_VAR_FULL_4GV_NB:
      rec_parm.settings.capability = VOC_CAP_4GV_NB;
      rec_parm.settings.max_rate   = VOC_1_RATE;
      rec_parm.settings.min_rate   = VOC_8_RATE;
      break;

    case SND_QCP_FORMAT_FIXED_FULL_4GV_NB:
      rec_parm.settings.capability = VOC_CAP_4GV_NB;
      rec_parm.settings.max_rate   = VOC_1_RATE;
      rec_parm.settings.min_rate   = VOC_1_RATE;
      break;
#endif  /* MSMAUD_VOC_4GV */

    case SND_QCP_FORMAT_VAR_FULL_EVRC:
      rec_parm.settings.capability = VOC_CAP_IS127;
      rec_parm.settings.max_rate   = VOC_1_RATE;
      rec_parm.settings.min_rate   = VOC_8_RATE;
      break;

    case SND_QCP_FORMAT_FIXED_FULL_EVRC:
      rec_parm.settings.capability = VOC_CAP_IS127;
      rec_parm.settings.max_rate   = VOC_1_RATE;
      rec_parm.settings.min_rate   = VOC_1_RATE;
      break;

    case SND_QCP_FORMAT_FIXED_12_2_AMR:
    case SND_QCP_FORMAT_FIXED_12_2_AMR_DTX:
      rec_parm.settings.capability = VOC_CAP_AMR;
      rec_parm.settings.max_rate   = VOC_AMR_RATE_1220;
      rec_parm.settings.min_rate   = VOC_AMR_RATE_1220;
      break;

    case SND_QCP_FORMAT_FIXED_10_2_AMR:
    case SND_QCP_FORMAT_FIXED_10_2_AMR_DTX:
      rec_parm.settings.capability = VOC_CAP_AMR;
      rec_parm.settings.max_rate   = VOC_AMR_RATE_1020;
      rec_parm.settings.min_rate   = VOC_AMR_RATE_1020;
      break;

    case SND_QCP_FORMAT_FIXED_7_95_AMR:
    case SND_QCP_FORMAT_FIXED_7_95_AMR_DTX:
      rec_parm.settings.capability = VOC_CAP_AMR;
      rec_parm.settings.max_rate   = VOC_AMR_RATE_795;
      rec_parm.settings.min_rate   = VOC_AMR_RATE_795;
      break;

    case SND_QCP_FORMAT_FIXED_7_4_AMR:
    case SND_QCP_FORMAT_FIXED_7_4_AMR_DTX:
      rec_parm.settings.capability = VOC_CAP_AMR;
      rec_parm.settings.max_rate   = VOC_AMR_RATE_740;
      rec_parm.settings.min_rate   = VOC_AMR_RATE_740;
      break;

    case SND_QCP_FORMAT_FIXED_6_7_AMR:
    case SND_QCP_FORMAT_FIXED_6_7_AMR_DTX:
      rec_parm.settings.capability = VOC_CAP_AMR;
      rec_parm.settings.max_rate   = VOC_AMR_RATE_670;
      rec_parm.settings.min_rate   = VOC_AMR_RATE_670;
      break;

    case SND_QCP_FORMAT_FIXED_5_9_AMR:
    case SND_QCP_FORMAT_FIXED_5_9_AMR_DTX:
      rec_parm.settings.capability = VOC_CAP_AMR;
      rec_parm.settings.max_rate   = VOC_AMR_RATE_590;
      rec_parm.settings.min_rate   = VOC_AMR_RATE_590;
      break;

    case SND_QCP_FORMAT_FIXED_5_15_AMR:
    case SND_QCP_FORMAT_FIXED_5_15_AMR_DTX:
      rec_parm.settings.capability = VOC_CAP_AMR;
      rec_parm.settings.max_rate   = VOC_AMR_RATE_515;
      rec_parm.settings.min_rate   = VOC_AMR_RATE_515;
      break;

    case SND_QCP_FORMAT_FIXED_4_75_AMR:
    case SND_QCP_FORMAT_FIXED_4_75_AMR_DTX:
      rec_parm.settings.capability = VOC_CAP_AMR;
      rec_parm.settings.max_rate   = VOC_AMR_RATE_475;
      rec_parm.settings.min_rate   = VOC_AMR_RATE_475;
      break;

    case SND_QCP_FORMAT_FIXED_FULL_13K:
      rec_parm.settings.capability = VOC_CAP_IS733;
      rec_parm.settings.max_rate = VOC_1_RATE;
      rec_parm.settings.min_rate = VOC_1_RATE;
      break;

    case SND_QCP_FORMAT_VAR_FULL_13K:
      rec_parm.settings.capability = VOC_CAP_IS733;
      rec_parm.settings.max_rate = VOC_1_RATE;
      rec_parm.settings.min_rate = VOC_8_RATE;
      break;

    case SND_QCP_FORMAT_FIXED_HALF_13K:
      rec_parm.settings.capability = VOC_CAP_IS733;
      rec_parm.settings.max_rate = VOC_2_RATE;
      rec_parm.settings.min_rate = VOC_2_RATE;
      break;

    case SND_QCP_FORMAT_VAR_HALF_13K:
      rec_parm.settings.capability = VOC_CAP_IS733;
      rec_parm.settings.max_rate = VOC_2_RATE;
      rec_parm.settings.min_rate = VOC_8_RATE;
      break;

#ifdef FEATURE_AUDFMT_AMR
    case SND_AMR_FORMAT_FIXED_12_2_AMR:
    case SND_AMR_FORMAT_FIXED_12_2_AMR_DTX:
      rec_parm.settings.capability   = VOC_CAP_AMR;
      rec_parm.settings.max_rate     = VOC_AMR_RATE_1220;
      rec_parm.settings.min_rate     = VOC_AMR_RATE_1220;
      rec_parm.settings.frame_format = VOC_PB_AMR;
      break;

    case SND_AMR_FORMAT_FIXED_10_2_AMR:
    case SND_AMR_FORMAT_FIXED_10_2_AMR_DTX:
      rec_parm.settings.capability   = VOC_CAP_AMR;
      rec_parm.settings.max_rate     = VOC_AMR_RATE_1020;
      rec_parm.settings.min_rate     = VOC_AMR_RATE_1020;
      rec_parm.settings.frame_format = VOC_PB_AMR;
      break;

    case SND_AMR_FORMAT_FIXED_7_95_AMR:
    case SND_AMR_FORMAT_FIXED_7_95_AMR_DTX:
      rec_parm.settings.capability   = VOC_CAP_AMR;
      rec_parm.settings.max_rate     = VOC_AMR_RATE_795;
      rec_parm.settings.min_rate     = VOC_AMR_RATE_795;
      rec_parm.settings.frame_format = VOC_PB_AMR;
      break;

    case SND_AMR_FORMAT_FIXED_7_4_AMR:
    case SND_AMR_FORMAT_FIXED_7_4_AMR_DTX:
      rec_parm.settings.capability   = VOC_CAP_AMR;
      rec_parm.settings.max_rate     = VOC_AMR_RATE_740;
      rec_parm.settings.min_rate     = VOC_AMR_RATE_740;
      rec_parm.settings.frame_format = VOC_PB_AMR;
      break;

    case SND_AMR_FORMAT_FIXED_6_7_AMR:
    case SND_AMR_FORMAT_FIXED_6_7_AMR_DTX:
      rec_parm.settings.capability   = VOC_CAP_AMR;
      rec_parm.settings.max_rate     = VOC_AMR_RATE_670;
      rec_parm.settings.min_rate     = VOC_AMR_RATE_670;
      rec_parm.settings.frame_format = VOC_PB_AMR;
      break;

    case SND_AMR_FORMAT_FIXED_5_9_AMR:
    case SND_AMR_FORMAT_FIXED_5_9_AMR_DTX:
      rec_parm.settings.capability   = VOC_CAP_AMR;
      rec_parm.settings.max_rate     = VOC_AMR_RATE_590;
      rec_parm.settings.min_rate     = VOC_AMR_RATE_590;
      rec_parm.settings.frame_format = VOC_PB_AMR;
      break;

    case SND_AMR_FORMAT_FIXED_5_15_AMR:
    case SND_AMR_FORMAT_FIXED_5_15_AMR_DTX:
      rec_parm.settings.capability   = VOC_CAP_AMR;
      rec_parm.settings.max_rate     = VOC_AMR_RATE_515;
      rec_parm.settings.min_rate     = VOC_AMR_RATE_515;
      rec_parm.settings.frame_format = VOC_PB_AMR;
      break;

    case SND_AMR_FORMAT_FIXED_4_75_AMR:
    case SND_AMR_FORMAT_FIXED_4_75_AMR_DTX:
      rec_parm.settings.capability   = VOC_CAP_AMR;
      rec_parm.settings.max_rate     = VOC_AMR_RATE_475;
      rec_parm.settings.min_rate     = VOC_AMR_RATE_475;
      rec_parm.settings.frame_format = VOC_PB_AMR;
      break;
#endif /* FEATURE_AUDFMT_AMR */

#ifdef FEATURE_AUDFMT_EVB
    case SND_EVB_FORMAT_VAR_FULL_4GV_NB:
      rec_parm.settings.capability   = VOC_CAP_4GV_NB;
      rec_parm.settings.max_rate     = VOC_1_RATE;
      rec_parm.settings.min_rate     = VOC_8_RATE;
      rec_parm.settings.frame_format = VOC_PB_EVB;
      break;
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
    case SND_EVW_FORMAT_VAR_FULL_4GV_WB:
      rec_parm.settings.capability   = VOC_CAP_4GV_WB;
      rec_parm.settings.max_rate     = VOC_1_RATE;
      rec_parm.settings.min_rate     = VOC_8_RATE;
      rec_parm.settings.frame_format = VOC_PB_EVW;
      break;
#endif /* FEATURE_AUDFMT_EVB */

    case SND_QCP_FORMAT_FIXED_FULL_EFR:
      rec_parm.settings.capability = VOC_CAP_GSM_EFR;
      rec_parm.settings.max_rate   = VOC_GSM_ENH_FULL_RATE;
      rec_parm.settings.min_rate   = VOC_GSM_ENH_FULL_RATE;
      break;

    case SND_QCP_FORMAT_FIXED_FULL_FR:
      rec_parm.settings.capability = VOC_CAP_GSM_FR;
      rec_parm.settings.max_rate   = VOC_GSM_FULL_RATE;
      rec_parm.settings.min_rate   = VOC_GSM_FULL_RATE;
      break;

    case SND_QCP_FORMAT_FIXED_FULL_HR:
      rec_parm.settings.capability = VOC_CAP_GSM_HR;
      rec_parm.settings.max_rate   = VOC_GSM_HALF_RATE;
      rec_parm.settings.min_rate   = VOC_GSM_HALF_RATE;
      break;

    default:
      break;
  } /* switch (format) */

  if(link == SND_QCP_DIR_REVERSE) {
    rec_parm.rec_type            = VOC_REC_REVERSE;
  } else if(link == SND_QCP_DIR_FORWARD) {
    rec_parm.rec_type            = VOC_REC_FORWARD;
  } else {
    rec_parm.rec_type            = VOC_REC_BOTH;
  }

  if (((format >= SND_QCP_FORMAT_FIXED_4_75_AMR_DTX) &&
      (format <= SND_QCP_FORMAT_FIXED_12_2_AMR_DTX))
#ifdef FEATURE_AUDFMT_AMR
      || ((format >= SND_AMR_FORMAT_FIXED_4_75_AMR_DTX) &&
          (format <= SND_AMR_FORMAT_FIXED_12_2_AMR_DTX))
#endif /* FEATURE_AUDFMT_AMR */
     ) {
    rec_parm.dtx_enable = TRUE;
  } else {

    rec_parm.dtx_enable = FALSE;
  }

  voc_rec_start(cb_func, &rec_parm);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_qcp_record_stop

DESCRIPTION
  This function sends a command to the vocoder driver to stop
  recording QCP data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_qcp_record_stop ( void )
{
  voc_rec_stop();
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_qcp_record_pause

DESCRIPTION
  This function sends a command to the vocoder driver to pause
  recording QCP data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_qcp_record_pause ( void )
{
  voc_rec_pause(VOC_REC_PAUSE_RET_DATA);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_qcp_record_resume

DESCRIPTION
  This function sends a command to the vocoder driver to resume
  recording QCP data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_qcp_record_resume ( void )
{
  voc_rec_resume();
}
#endif /* FEATURE_QCP */

/* <EJECT>*/
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION sndhw_pcm_record

DESCRIPTION
  This function sends a command to the vocoder driver to start
  recording PCM data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_pcm_record (
  snd_pcm_rec_dir_type           link,
  voc_pcm_client_output_fn_type  cb_func
) {
  if(link == SND_PCM_REC_DIR_REVERSE) {
    sndhw_pcm_rec_reverse_enabled = (cb_func != NULL);
    voc_register_pcm_output_client(VOC_PCM_INTERFACE_TX_OUTPUT, cb_func);
  } else if(link == SND_PCM_REC_DIR_FORWARD) {
    sndhw_pcm_rec_forward_enabled = (cb_func != NULL);
    voc_register_pcm_output_client(VOC_PCM_INTERFACE_RX_OUTPUT, cb_func);
  }
  sndhw_set_apath_out(&snd_crnt_path_out);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_pcm_record_stop

DESCRIPTION
  This function sends a command to the vocoder driver to stop
  recording PCM data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_pcm_record_stop (
  snd_pcm_rec_dir_type link
) {
  if(link == SND_PCM_REC_DIR_REVERSE) {
    sndhw_pcm_rec_reverse_enabled = FALSE;
    voc_register_pcm_output_client(VOC_PCM_INTERFACE_TX_OUTPUT, NULL);
  } else if(link == SND_PCM_REC_DIR_FORWARD) {
    sndhw_pcm_rec_forward_enabled = FALSE;
    voc_register_pcm_output_client(VOC_PCM_INTERFACE_RX_OUTPUT, NULL);
  }
  sndhw_set_apath_out(&snd_crnt_path_out);
}
#endif /* FEATURE_PCM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT>*/
#ifdef FEATURE_MIDI_OUT
/*===========================================================================

FUNCTION sndhw_midi_out_path_ready

DESCRIPTION
  This function return the status of midi out path.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - The path is ready.
  FLASE - The path is not ready.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_midi_out_path_ready(void) {
  return ((sndhw_midi_out_enabled) && (voc_check_cap_ready(VOC_CAP_QSYNTH)==TRUE));
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_midi_out

DESCRIPTION
  This function enables the MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type sndhw_midi_out(
#ifdef FEATURE_MIDI_OUT_QCP
  voc_adec_cb_func_struct     adec_cb,
  void                        *adec_client_data,
#endif /* FEATURE_MIDI_OUT_QCP */
  snd_midi_out_type           enable
) {
  if(enable == SND_MIDI_OUT_ENABLE) {
    sndhw_midi_out_enabled = TRUE;
    sndhw_set_apath_out(&snd_crnt_path_out);
    (void)sndhw_set_sample_rate(SND_SAMPLE_RATE_32000);
#ifdef FEATURE_MIDI_OUT_QCP
    (void) voc_adec_init(adec_cb, adec_client_data, NULL);
#endif /* FEATURE_MIDI_OUT_QCP */
    sndhw_voc_config(SND_AUDFMT_SOURCE_MIDI_OUT, VOC_CAP_QSYNTH); /* Cnfg DSP */
  } else {
    sndhw_midi_out_enabled = FALSE;
    sndhw_set_apath_out(&snd_crnt_path_out);
    sndhw_voc_config(SND_AUDFMT_SOURCE_MIDI_OUT, VOC_CAP_NONE); /* Uncnfg DSP */
  }
  return(SND_SUCCESS);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_midi_out_msg

DESCRIPTION
  This function sends midi commands to the midi device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type sndhw_midi_out_msg (
  uint16 num_commands,
  uint16 num_words,
  uint16 *buf
) {
  snd_status_type   ret_val = SND_FAILURE;
  voc_midi_cmd_type cmd_buf;
  uint8             volume;
  sint15            db_volume;

  volume    = snd_data_get_volume(snd_crnt_path_out.device, SND_METHOD_MIDI);
  db_volume = snd_data_get_midi_volume(snd_crnt_path_out.device,
                                       SND_METHOD_MIDI, volume);
  (void) voc_master_volume(db_volume);

  if(voc_get_status() == VOC_INFORM_QSYNTH) {
    cmd_buf.num_words    = num_words;
    cmd_buf.num_commands = num_commands;
    cmd_buf.command_buf  = buf;
    if(voc_midi_cmd(&cmd_buf, 1, db_volume) == VOC_DONE_S) {
      ret_val = SND_SUCCESS;
    }
  }

  return(ret_val);
}
#endif /* FEATURE_MIDI_OUT */

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_change_gen

DESCRIPTION
  Turn off Tone Generators, either because they are no longer needed or
  because they have been preempted by a higher priority sound.

DEPENDENCIES
  sndhw_aux_busy
  sndhw_adsp_busy

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_change_gen
(
  snd_gen_type new_gen
    /* Type of Generator that is about to start */
)
{
  static snd_gen_type last_gen = SND_GEN_NONE;

#ifdef FEATURE_VOICE_PLAYBACK
  /* Mute earpiece if not GEN_VOC. */
  if (new_gen != SND_GEN_VOC) {
    sndhw_voc_vp(NULL);
  }
#endif

  /* If the Vocoder is busy and the new generator is not a Vocoder      */
  /* Sound, then shut the Vocoder Sound Off.                            */
  if (((new_gen != SND_GEN_VOC) && (new_gen != SND_GEN_RING)) ||
      (new_gen != last_gen)) {
    sndhw_voc_tone(NULL);
  }

  /* If the Ringer is busy and the new generator is not a Ring Sound,   */
  /* then shut the Ringer off.                                          */

  if (new_gen != SND_GEN_RING) {
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
    if(sndhw_bt_hs_ringing) {
      sndhw_bt_hs_ringing = FALSE;
    }
#endif /* defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) */
    sndhw_ring(NULL);
  }

  /* If the auxiliary generator is busy,                                */
  /* then shut it off.                                                  */

  if (sndhw_aux_busy) {
    if (new_gen == SND_GEN_NONE) {
      sndhw_aux_busy = FALSE;
      sndhw_aux_ctl_fn(FALSE);
    }
  }

#ifdef FEATURE_MIDI
  /* Mute midi generator */
  if ((new_gen != SND_GEN_MIDI) && (last_gen == SND_GEN_MIDI)) {
    (void) sndhw_audfmt_tone(NULL);
  }
#endif

  last_gen = new_gen;
} /* end of sndhw_change_gen */
/* <EJECT>*/
/*===========================================================================
 FUNCTION sndhw_voc_module_fetched

 DESCRIPTION
	 Inform upper layer that the Vocoder is unable to support current requested
	 module. Expires snd_index for which voc_config
	 was requested.

 DEPENDENCIES
	 sndhw_curr_index
	 sndhw_curr_source
	 sndhw_curr_config

 RETURN VALUE
	 None

 SIDE_EFFECTS
 	 sndhw_current_index
	 sndhw_current_source
	 sndhw_current_config

 ===========================================================================*/

void sndhw_voc_module_fetched(void)
{

#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
  snd_sound_status_type  		*snd_array_ptr = NULL;
  snd_array_idx_type		cur_idx = sndhw_current_index;
  snd_audfmt_source_type 		cur_src = sndhw_current_source;
  voc_capability_type		cur_cap	= sndhw_current_capability;

  /*while the Vocoder informed about module availability,
    sound settings changed*/
  if((cur_idx == SND_ARRAY_SIZE) ||
     (cur_src == SND_AUDFMT_SOURCE_MAX ) ||
     (cur_cap == VOC_CAP_NONE)
     )
    return;

  snd_array_ptr = &( snd_array[ sndhw_current_index ] );
  if( snd_array_ptr->active == FALSE )
    return;
  snd_expire(sndhw_current_index);
  (void) rex_clr_timer( &snd_voc_module_timer );

#endif
}

/* end of sndhw_voc_module_fetched */
/* <EJECT>*/

/*==============================================================

FUNCTION sndhw_voc_change

DESCRIPTION
  Tell the Sound Task that the vocoder has changed state.

DEPENDENCIES
  This routine may be called from an interrupt, don't use DM messages.

RETURN VALUE
  None

SIDE EFFECTS
  snd_path_is_ready
  snd_voc_is_ok
  snd_analog_mode

===========================================================================*/
LOCAL void sndhw_voc_change
(
  voc_inform_type status
    /* New Status of Vocoder */
)
{

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

  /* Is the Vocoder "usable" at this time or not?            */
  snd_voc_is_ok = (boolean) ((status == VOC_INFORM_VOCODER)       ||
#ifdef FEATURE_ACP
#error code not present
#endif
#ifdef FEATURE_QSYNTH_COMMON
                             (status == VOC_INFORM_QSYNTH)        ||
#endif
#ifdef FEATURE_QTUNES_COMMON
                             (status == VOC_INFORM_QTUNES)        ||
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
                             (status == VOC_INFORM_CAM_AUDIO_ACTIVE) ||
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
#endif
#ifdef FEATURE_GRAPH_AUDIO
                             (status == VOC_INFORM_GRAPH_IDLE)    ||
                             (status == VOC_INFORM_GRAPH_AUDIO)   ||
#endif
#ifdef FEATURE_AVS_INCALL_ADEC_PB
                             (status == VOC_INFORM_VOCODER_ADEC)  ||
#endif
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
                             (status == VOC_INFORM_MIXER_ACTIVE)  ||
#endif
#ifdef FEATURE_VOC_ADPCM
                             (status == VOC_INFORM_VOCODER_ADPCM) ||
#endif
                             (status == VOC_INFORM_IDLE) );

#ifdef FEATURE_PCM_REC
  if(sndhw_pcm_rec_reverse_enabled || sndhw_pcm_rec_forward_enabled) {
    if((status == VOC_INFORM_IDLE) || (status == VOC_INFORM_VOCODER)
#ifdef FEATURE_ACP
#error code not present
#endif
      ) {
      snd_pcm_rec_status (SND_CMX_PCM_REC_INT_RESUME, NULL, NULL, NULL);
    } else {
      snd_pcm_rec_status (SND_CMX_PCM_REC_INTERRUPTED, NULL, NULL, NULL);
    }
  }
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_AUDIO_FORMAT
  if(status != VOC_INFORM_MODULE_UNFETCHED)
  {
    (void) rex_clr_timer( &snd_voc_module_timer );
  }
#endif

  switch (status) {

    /* The vocoder is about to be reset. The vocoder module will take care
    ** of everything.
    */
    case VOC_INFORM_RESET:
      /* Clear SND_PATH_TIMER_SIG. Indicate that no audio paths are ready.
      */
      snd_path_is_ready = SND_PATH_NOT_READY;
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
      sndhw_adec_client_data = NULL;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */
      break;

    case VOC_INFORM_MODULE_UNFETCHED:

#ifdef FEATURE_AUDIO_FORMAT
      if (rex_get_timer(&snd_voc_module_timer) == 0)
       {
         /*set the timer to expire snd client, in case the corresponding
           module is not available on dsp*/
    	 (void) rex_set_timer( &snd_voc_module_timer, SND_VOC_MODULE_TIME );
       }
#endif
    	break;

    /* The firmware completed reset, finish setting the path. Start any
    ** active tones.
    */
    case VOC_INFORM_VOCODER:
#ifdef FEATURE_ACP
#error code not present
#endif
#ifdef FEATURE_QSYNTH_COMMON
    case VOC_INFORM_QSYNTH:
#endif
#ifdef FEATURE_QTUNES_COMMON
    case VOC_INFORM_QTUNES:
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
    case VOC_INFORM_CAM_AUDIO_ACTIVE:
#endif/* FEATURE_AUDIO_CAMERA_CONCURRENCY */

#endif
#ifdef FEATURE_AVS_INCALL_ADEC_PB
    case VOC_INFORM_VOCODER_ADEC:
#endif
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
    case VOC_INFORM_MIXER_ACTIVE:
        case VOC_INFORM_MIXER_IDLE:
    case VOC_INFORM_MIXER_VOC_IDLE:
    case VOC_INFORM_MIXER_VOCODER:
    case VOC_INFORM_MIXER_ADEC:
#endif
#ifdef FEATURE_GRAPH_AUDIO
    case VOC_INFORM_GRAPH_IDLE:
    case VOC_INFORM_GRAPH_AUDIO:
#endif
#ifdef FEATURE_VOC_ADPCM
		case VOC_INFORM_VOCODER_ADPCM:
#endif
    case VOC_INFORM_IDLE:
#ifdef FEATURE_ACP
#error code not present
#endif
      /* Make paths ready */
      sndhw_path_timer_ctl();

      /* The vocoder state changed, update the tones/mute state. */
      (void) rex_set_sigs( &snd_tcb, SND_VOC_CHNG_SIG );
      break;

    /* If the MSM mode is going to change, stop
    ** any vocoder tones.
    */
    case VOC_INFORM_CHANGE:
      sndhw_change_gen(SND_GEN_RING);
      break;

    default:
      ERR_FATAL("switch(%d) Unknown in sndhw_voc_change()",status,0,0);
  }
} /* end of sndhw_voc_change */
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_voc_change_path_set

DESCRIPTION
  This routine does a path set and possibly signals the IDS task after the
  vocoder state has changed to VOC_ACQ_RE_INIT_S.

DEPENDENCIES
  extcom_cb_ptr
  extcom_client_data
  sndhw_ext_com_state
  snd_analog_mode

RETURN VALUE
  None

SIDE EFFECTS
  extcom_cb_ptr
  sndhw_ext_com_state

===========================================================================*/
void sndhw_voc_change_path_set()
{
#ifdef SND_HFK_PLUS
#error code not present
#endif
  if (sndhw_tone_playing == TRUE)
  {
    snd_rx_mute_ctl(SND_MUTE_NONE, TRUE);
  }

  sndhw_smartsnd_aud_process_ctl();
  sndhw_set_volume();

#ifdef SND_EXTCOM
#error code not present
#endif
}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_voc_acquire

DESCRIPTION
  Acquire the Vocoder on behalf of the Sound Task (if it has not already
  been acquired).

DEPENDENCIES
  snd_voc_acquired
  snd_voc_is_acquired

RETURN VALUE
  None

SIDE EFFECTS
  snd_voc_is_acquired
  snd_voc_acquired

===========================================================================*/
LOCAL void sndhw_voc_acquire( void )
{
  voc_inform_type voc_status;

  /* If the Vocoder was already acquired, then there is nothing to do.  */

  if (!snd_voc_acquired) {
    /* Okay, lets set the flag that assumes it is running and then        */
    /* acquire the Vocoder.  Note that if in fact the Vocoder is not      */
    /* available, the sndhw_voc_change() routine will be immediately      */
    /* called and the flag will drop.                                     */

    voc_acquire(VOC_ACQ_SND,sndhw_voc_change);
  }

  /* Is the Vocoder "usable" at this time or not?            */
  voc_status = voc_get_status();

  snd_voc_is_ok = ((voc_status == VOC_INFORM_VOCODER)       ||
#ifdef FEATURE_ACP
#error code not present
#endif
#ifdef FEATURE_QSYNTH_COMMON
                   (voc_status == VOC_INFORM_QSYNTH)        ||
#endif
#ifdef FEATURE_QTUNES_COMMON
                   (voc_status == VOC_INFORM_QTUNES)        ||
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
                  (voc_status == VOC_INFORM_CAM_AUDIO_ACTIVE) ||
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
#endif
#ifdef FEATURE_GRAPH_AUDIO
                   (voc_status == VOC_INFORM_GRAPH_IDLE)    ||
                   (voc_status == VOC_INFORM_GRAPH_AUDIO)   ||
#endif
#ifdef FEATURE_AVS_INCALL_ADEC_PB
                   (voc_status == VOC_INFORM_VOCODER_ADEC)  ||
#endif
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
                   (voc_status == VOC_INFORM_MIXER_ACTIVE)  ||
#endif
#ifdef FEATURE_VOC_ADPCM
                   (voc_status == VOC_INFORM_VOCODER_ADPCM) ||
#endif
                   (voc_status == VOC_INFORM_IDLE));

  if (snd_voc_is_ok) {
#ifdef FEATURE_ACP
#error code not present
#endif
    if (!snd_voc_acquired) {
      sndhw_path_timer_ctl();
    }
  } else {
    /* The path timer is being set here, if the vocoder is not available
    ** at the end of this timeout then tones should be expired rather than
    ** be played after such a long delay.
    */
    (void) rex_set_timer( &snd_path_timer, SND_TONE_VOC_TIMEOUT );
  }
  snd_voc_acquired = TRUE;
  MSG_HIGH("Vocoder acquired",0,0,0);

} /* end of sndhw_voc_acquire() */
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_is_path_ready

DESCRIPTION
  Returns the path status.

DEPENDENCIES
  snd_path_is_ready

RETURN VALUE
  SND_PATH_NOT_READY  - if path is not ready
  SND_PATH_VOC_READY  - if vocoder is acquired but path is not yet ready
  SND_PATH_READY      - if path is ready

SIDE EFFECTS
  None.

===========================================================================*/
snd_path_ready_type sndhw_is_path_ready( void )
{
  return(snd_path_is_ready);
}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_path_timer_ctl

DESCRIPTION
  Mark paths ready after a fixed delay period.

DEPENDENCIES
  snd_path_timer

RETURN VALUE
  None

SIDE EFFECTS
  snd_path_is_ready

===========================================================================*/
void sndhw_path_timer_ctl( void )
{
  uint16 warmup_time;
  if(snd_path_is_ready == SND_PATH_NOT_READY) {
    warmup_time = snd_data_get_warmup_time(snd_crnt_path_out.device,
                                           snd_crnt_path_out.method);
    if(warmup_time == 0) {
      /* If no need for warmup delay, set path to ready */
      snd_path_is_ready = SND_PATH_READY;
    } else {
      /* If need warmup delay, set path to ready but vocoder needs to warm up */
      (void) rex_set_timer( &snd_path_timer, warmup_time );
      snd_path_is_ready = SND_PATH_VOC_READY;
    }
  } else {
    /* Set path ready */
    snd_path_is_ready = SND_PATH_READY;
  }
} /* end of sndhw_path_timer_ctl() */
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_which_codec

DESCRIPTION
  Setup the Phone based on the output path.  Hardware will use the Internal
  or External Codec as required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Volume is muted, and needs to be reset. The volume is not set in this
  routine because the vocoder may be resetting.

===========================================================================*/
LOCAL void sndhw_which_codec
(
  snd_pstate_type *path
    /* Which Audio Path to Setup */
)
{
  static voc_codec_type previous_codec = VOC_CODEC_NONE;
    /* Remember Previous apath_out setting to suppress redundant calls. */
  voc_codec_type tcodec;
    /* Temporary for current codec setting */
  voc_status_type voc_status;
    /* Temporary for Checking voc_call() return Values. */

  /* Enables the caller to force a format change */
  if (path == NULL) {
    previous_codec = VOC_CODEC_NONE;
    return;
  } else {
#ifdef SND_EXTCOM
#error code not present
#endif
    {
      switch (previous_codec) {
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
        case VOC_CODEC_BT_A2DP:
        case VOC_CODEC_BT_A2DP_SCO:
#ifdef FEATURE_AVS_I2SSBC
          if ((sndhw_i2ssbc_status != SND_I2SSBC_STATUS_DISABLED) &&
              (path->method != SND_METHOD_MIDI))
          {
            tcodec = previous_codec;
            break;
          } else
#endif
          if (sndhw_bt_a2dp_connected &&
              ((path->method == SND_METHOD_KEY_BEEP) ||
                (path->method == SND_METHOD_MESSAGE))) {
              /* We dont want to switch the codec as we want the tone
              to be played on the BT headset */
            tcodec = previous_codec;
            break;
          }
#endif
#ifdef FEATURE_BT_AG
        case VOC_CODEC_BT_AG:
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
          if (sndhw_bt_ag_connected &&
              ((path->method == SND_METHOD_KEY_BEEP) ||
                (path->method == SND_METHOD_MESSAGE))) {
              /* We dont want to switch the codec as we want the tone
              to be played on the BT headset */
            tcodec = previous_codec;
            break;
          }
#endif
        default:
      /* Retrieve codec to use with device/method pair */
      tcodec = snd_data_get_pcm_path(path->device, path->method);
          break;
      }
    }
  }

  /* If this call sets the hardware to the same Output Path as the last */
  /* call, then we ignore the call.  This prevents upsetting the        */
  /* Vocoder(s) and Codec(s) unnecessarily.                             */
  if (previous_codec == tcodec) {
    return;
  }

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  if ((tcodec == VOC_CODEC_BT_A2DP) || (tcodec == VOC_CODEC_BT_A2DP_SCO)) {
    /* Current codec is BT_A2DP or BT_A2DP_SCO */

    sndhw_codec_supports_a2dp = TRUE;
  } else {
    sndhw_codec_supports_a2dp = FALSE;
#ifdef FEATURE_AVS_I2SSBC
    /* Current codec is not BT_A2DP or BT_A2DP_SCO */
    if (((previous_codec == VOC_CODEC_BT_A2DP) ||
         (previous_codec == VOC_CODEC_BT_A2DP_SCO)) &&
        (sndhw_i2ssbc_status != SND_I2SSBC_STATUS_DISABLED)) {
      /* Codec has changed, the new codec does not support BT A2DP so we
      need to release the vocoder acquired with key VOC_ACQ_I2SSBC */
      sndhw_i2ssbc_disable();
    }
#endif /* FEATURE_AVS_I2SSBC */
  }
#endif

#if defined(FEATURE_BT_AG) && defined (FEATURE_AVS_BT_SCO_REWORK)
  if (tcodec == VOC_CODEC_BT_AG) {
    /* Current codec is BT_AG */
      sndhw_codec_supports_ag = TRUE;
      MSG_MED("sndhw_which_codec:codec supports Ag",0,0,0);
  }else {
      sndhw_codec_supports_ag = FALSE;
  }
#endif /*AVS_BT_SCO_REWORK*/

  voc_check_codec_clk_change(previous_codec , tcodec );

  /* Save current codec config */
  previous_codec = tcodec;

  MSG_MED("Selecting Codec = %d",tcodec,0,0);

#ifdef SND_EXTCOM
#error code not present
#endif
  {
    voc_codec_pad(SND_CODEC_MIN_VOL,
                  snd_data_get_pad_mask(path->device, path->method));
  }

  voc_status = voc_vol_cmd(VOC_VOL_SILENCE);
  if (voc_status != VOC_DONE_S) {
    ERR("voc_vol_cmd(VOC_VOL_SILENCE) Error=%d",voc_status,0,0);
  }

  snd_rx_mute_ctl( SND_MUTE_SND, TRUE );

  voc_codec_select(tcodec);
} /* end of sndhw_which_codec() */
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_stereo_mute

DESCRIPTION
  Attempt to mute/unmute the Car Stereo as requested by Regional Flags.
  This assumes of course that we are connected to a Car Kit that supports
  stereo mute.

DEPENDENCIES
  sndhw_ext_com_state
  sndhw_hfk_protocol

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_stereo_mute_s
  extcom_stereo_mute_s

===========================================================================*/
void sndhw_stereo_mute( boolean mute )
{
#ifdef SND_HFK
#ifdef SND_EXTCOM
#error code not present
#endif
#ifndef SND_HFK_PLUS
  sndhw_stereo_mute_s = mute;
  /* Path override is done in the path setting function */
  sndhw_set_apath_out(&snd_crnt_path_out);
#else
#error code not present
#endif /* SND_HFK_PLUS */
#endif /* SND_HFK */
} /* end of sndhw_stereo_mute */
/* <EJECT>*/
#ifdef SND_EXTCOM
#error code not present
#endif /* SND_EXTCOM */
/* <EJECT>*/
#ifdef SND_HFK_PLUS
#error code not present
#endif /* SND_HFK_PLUS */
/* <EJECT> */
#ifdef SND_EXTCOM
#error code not present
#endif /* SND_EXTCOM */
/* <EJECT>*/
/*===========================================================================

FUNCTION snd_rx_mute_ctl

DESCRIPTION
  Temporary mute/unmute of the audio input from the FM receive path.
  The parameter allows multiple requesters to request muting and for the
  audio path to unmute only when all requesters have un-muted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void snd_rx_mute_ctl
(
  snd_mute_type mask,
    /* mask of audio mute request associated with caller */

  boolean mute_audio
    /* True = mute audio, False = un-mute.               */
)
{
  static snd_mute_type snd_rxfm_muted = SND_MUTE_SND;
    /* Static to Remember current requesters between calls */
  static boolean snd_rxfm_mute_override = FALSE;

  /* Diagnostic override allows the Diag Task to run its tests with
  ** full control over muting.  Once we have a Diagnostic override,
  ** we have it forever.
  */
  if ( (mask & SND_MUTE_DIAG) != 0 ) {
    snd_rxfm_mute_override = TRUE;
    mask = SND_ALL_MUTE;                        /* Vote all bits */
  } else if ( snd_rxfm_mute_override ) {
    return;
  }

  INTLOCK();                                    /* Block interrupts */

  /* Turn on/off the associated mask bit
  */
  if ( mute_audio ) {
    snd_rxfm_muted = (snd_mute_type)(snd_rxfm_muted | mask); /* Set the bit */
  } else {
    /* mask the bit */
    snd_rxfm_muted = (snd_mute_type)(snd_rxfm_muted^(snd_rxfm_muted & mask));
    /* Note: you can't legally ones-compliment an enum */
  }
  INTFREE();                            /* Un-block interrupts          */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  {
    if((mask&SND_SND_MUTE) || (mask == SND_MUTE_NONE)) {
      MSG_MED("Mute RX (digital), mask = %d, mute:%d", mask, mute_audio, 0);
      if( mask & SND_MUTE_SNDV ) {
        sndhw_rx_dtmf_override_ctl(mute_audio);
      } else {
        sndhw_rx_mute_ctl(mute_audio);
      }
    }
  }

} /* end of snd_rx_mute_ctl */

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_txfm_mute_ctl

DESCRIPTION
  Temporary mute/unmute of the audio output to the FM transmit path.

  Note:  this does not mute DTMF output to the FM transmit path, use
  snd_dtmf_tx_mute_ctl() to mute DTMF output.

DEPENDENCIES
  Multiple Mute Sources is supported at snd_txfm_mute_ctl() level.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_txfm_mute_ctl
(
  boolean mute_audio
    /* True = mute audio, False = un-mute.                              */
)
{
  voc_status_type voc_status;
    /* Temporary for checking voc_xxx() return value.                   */

  /* The Audio Switch that controls this is in the Vocoder.             */

  voc_status = voc_tx_mute_cmd(mute_audio);
  if (voc_status != VOC_DONE_S)
    {
    ERR("voc_tx_mute_cmd(%d) Error=%d",mute_audio,voc_status,0);
    }
} /* end of sndhw_txfm_mute_ctl */
/* <EJECT>*/
/*========================================================================

FUNCTION sndhw_dtmf_tx_mute_ctl

DESCRIPTION
  Control mute/unmute of the DTMF output to the FM transmit path.

DEPENDENCIES
  snd_dtmf_tx_mute_ctl() has additional code to allow multiple source
  muting.

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================*/
void sndhw_dtmf_tx_mute_ctl
  (
  boolean mute_audio
    /* True = mute audio, False = un-mute. */
  )
{
  (void) voc_dtmf_tx_mute_cmd(mute_audio);
} /* end of sndhw_dtmf_tx_mute_ctl */

/* <EJECT> */
#ifdef FEATURE_QTUNES_COMMON
/*===========================================================================

FUNCTION sndhw_qtunes_gen_tone

DESCRIPTION
  This routine is called by sndhw_gen_tone() to actually begin
  the generation of a tone QTUNES mode.

DEPENDENCIES
  sndhw_aux_busy
  sndhw_adsp_busy
  sndhw_hfk_protocol
  sndhw_hfk_vr_tone_ready

RETURN VALUE
  TRUE  - if tone is on hold
  FALSE - if tone is playing

SIDE EFFECTS
  None.

===========================================================================*/
boolean sndhw_qtunes_gen_tone
(
  tone_status_type     *tone_status_ptr,
  voc_capability_type  voc_capability
)
{
  boolean hold = FALSE;

#ifdef FEATURE_MPEG_AUDIO
  snd_audfmt_mpeg_header_config *mpeg_hdr;
#endif
#ifdef FEATURE_REAL_AUDIO
  /* Allocate space on heap instead of stack */
  static voc_ra_dsp_config_type   ra_config;
  snd_audfmt_ra_dsp_config *ra_cfg = NULL;
#endif

  if(sndhw_audfmt_need_config) {
    hold = !sndhw_set_sample_rate(tone_status_ptr->sample_rate);

#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
    /* This check is made for the in call wma scenario where in there is no
       change required from the client to set the start postion as
       PATH_FORWARD_INCALL as we are doing it here*/
#ifdef FEATURE_WMA
    if(voc_capability == VOC_CAP_QTUNES_WMA) {
      tone_status_ptr->audfmt_path = SND_AUDFMT_START_PATH_FORWARD_INCALL;
    }
#endif
    if ((voc_capability == VOC_CAP_QTUNES)
#ifdef FEATURE_QTUNES_AAC
        ||(voc_capability == VOC_CAP_QTUNES_AAC)
#endif
#ifdef FEATURE_WMA
        ||(voc_capability == VOC_CAP_QTUNES_WMA)
#endif
        ) {
      if (tone_status_ptr->audfmt_path == SND_AUDFMT_START_PATH_FORWARD_INCALL){
        voc_set_audfmt_mix_mode(VOC_ADEC_MIX_MODE_RX);
      }
      else {
        voc_set_audfmt_mix_mode(VOC_ADEC_MIX_MODE_NONE);
      }
    } else {
      voc_set_audfmt_mix_mode(VOC_ADEC_MIX_MODE_NONE);
    }
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */

    /* Config DSP */
    sndhw_voc_config(SND_AUDFMT_SOURCE_NORMAL, voc_capability);
    sndhw_audfmt_need_config = hold;
  }

#ifdef FEATURE_REAL_AUDIO
  if (voc_capability == VOC_CAP_QTUNES_RA) {
    ra_cfg = (snd_audfmt_ra_dsp_config*)
                &tone_status_ptr->audfmt_dsp_config.ra_config;
    ra_config.decoder_mode      = ra_cfg->decoder_mode;
    ra_config.sample_rate       = ra_cfg->sample_rate;
    ra_config.num_channels      = ra_cfg->num_channels;
    ra_config.samples_per_frame = ra_cfg->samples_per_frame;
    ra_config.num_regions       = ra_cfg->num_regions;
    ra_config.bits_per_frame    = ra_cfg->bits_per_frame;
    ra_config.coupling_start    = ra_cfg->coupling_start;
    ra_config.coupling_quant    = ra_cfg->coupling_quant;
    ra_config.frame_erasure     = ra_cfg->frame_erasure;

    (void)voc_ra_set_song_config(&ra_config);
  }
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
  if((voc_capability == VOC_CAP_QTUNES_WMA) ||
      (voc_capability == VOC_CAP_QTUNES_WMA_PRO))
  {
    voc_wma_set_song_config((const voc_wma_config_type *)
                    &tone_status_ptr->audfmt_dsp_config.wma_config);
  }
#endif /* FEATURE_WMA */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  if (sndhw_codec_supports_a2dp && voc_is_bt_a2dp_pending())
  {
    hold = TRUE;
  } else
#endif
#if defined(FEATURE_BT_AG) && defined (FEATURE_AVS_BT_SCO_REWORK)
  if (sndhw_codec_supports_ag && voc_is_bt_ag_pending())
  {
    hold = TRUE;
    MSG_MED("holding tunes play - awaiting sco_connect_evt ...",0,0,0);
  }else
#endif /*AVS_BT_SCO_REWORK*/
  if ((voc_get_status()    == VOC_INFORM_QTUNES)
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
    ||(voc_get_status() == VOC_INFORM_CAM_AUDIO_ACTIVE)
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
#if defined (FEATURE_MP3) || defined (FEATURE_AAC) || defined (FEATURE_WMA)
      || (
#ifdef FEATURE_AVS_INCALL_ADEC_PB
          (voc_get_status() == VOC_INFORM_VOCODER_ADEC) &&
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
          ((voc_get_status() == VOC_INFORM_MIXER_IDLE) ||
           (voc_get_status() == VOC_INFORM_MIXER_ACTIVE)) &&
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
          ((voc_capability   == VOC_CAP_QTUNES) 
#ifdef FEATURE_QTUNES_AAC // Gemsea ADD
          || (voc_capability   == VOC_CAP_QTUNES_AAC)
#endif
#ifdef FEATURE_WMA
           ||(voc_capability   == VOC_CAP_QTUNES_WMA)
           ||(voc_capability   == VOC_CAP_QTUNES_WMA_PRO)
#endif
           ))
#endif /* FEATURE_MP3 || FEATURE_AAC || FEATURE_WMA */
     ) {
#ifdef FEATURE_WEBAUDIO
    if((tone_status_ptr != NULL) && (tone_status_ptr->audfmt_buf != NULL)) {
      tone_status_ptr->audfmt_buf->start_aging  = TRUE;
    }
#endif
    if(sndhw_adec_client_data == NULL) {
      tone_status_ptr->time_remain_ms = 0;
      switch (voc_capability) {
#ifdef FEATURE_MPEG_AUDIO
#ifdef FEATURE_AAC
        case VOC_CAP_QTUNES_AAC:
          if (voc_aac_set_song_config((const voc_aac_config_type*)
              &tone_status_ptr->audfmt_dsp_config.aac_config.params) == FALSE) {
            sndhw_adec_client_data = NULL;
            return hold;
          }
          mpeg_hdr = &tone_status_ptr->audfmt_dsp_config.mpeg_hdr_config;
          if (voc_mpeg_set_song_config(
                  mpeg_hdr->mpeg_mask,
                  mpeg_hdr->mpeg_header,
                  mpeg_hdr->mpeg_mask_size,
                  voc_capability) == FALSE) {
            sndhw_adec_client_data = NULL;
            return hold;
          }
          sndhw_set_av_sync(VOC_AV_SYNC_DEC_AAC,
                            tone_status_ptr->av_sync_cb,
                            tone_status_ptr->av_sync_mode,
                            tone_status_ptr->av_sync_interval);
          break;
#endif /* FEATURE_AAC */

#ifdef FEATURE_MP3
        case VOC_CAP_QTUNES:
          if (voc_mp3_set_song_config((const voc_mp3_config_type*)
              &tone_status_ptr->audfmt_dsp_config.mp3_config.params) == FALSE) {
            sndhw_adec_client_data = NULL;
            return hold;
          }
          mpeg_hdr = &tone_status_ptr->audfmt_dsp_config.mpeg_hdr_config;
          if (voc_mpeg_set_song_config(
                  mpeg_hdr->mpeg_mask,
                  mpeg_hdr->mpeg_header,
                  mpeg_hdr->mpeg_mask_size,
                  voc_capability) == FALSE) {
            sndhw_adec_client_data = NULL;
            return hold;
          }
          sndhw_set_av_sync(VOC_AV_SYNC_DEC_MP3,
                            tone_status_ptr->av_sync_cb,
                            tone_status_ptr->av_sync_mode,
                            tone_status_ptr->av_sync_interval);
          break;
#endif /* FEATURE_MP3 */
#endif /* FEATURE_MPEG_AUDIO */

#ifdef FEATURE_REAL_AUDIO
        case VOC_CAP_QTUNES_RA:
          if (voc_ra_set_song_config(&ra_config) == FALSE) {
            sndhw_adec_client_data = NULL;
            return hold;
          }
          sndhw_set_av_sync(VOC_AV_SYNC_DEC_RA,
                            tone_status_ptr->av_sync_cb,
                            tone_status_ptr->av_sync_mode,
                            tone_status_ptr->av_sync_interval);
          break;
#endif /* FEATURE_REAL_AUDIO */


#ifdef FEATURE_WMA
        case VOC_CAP_QTUNES_WMA:
        case VOC_CAP_QTUNES_WMA_PRO:
          if (voc_wma_set_song_config((const voc_wma_config_type *)
                    &tone_status_ptr->audfmt_dsp_config.wma_config) == FALSE)
          {
            /* Decoder not ready yet */
            sndhw_adec_client_data = NULL;
            return hold;
          }
          sndhw_set_av_sync(VOC_AV_SYNC_DEC_WMA,
                            tone_status_ptr->av_sync_cb,
                            tone_status_ptr->av_sync_mode,
                            tone_status_ptr->av_sync_interval);
          break;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
        case VOC_CAP_QTUNES_AMR_WBPLUS:
          if (voc_amr_wbp_set_song_config((const voc_amr_wbp_config_type*)
              &tone_status_ptr->audfmt_dsp_config.amr_wbp_config) == FALSE) {

            /* Decoder not ready yet */
            sndhw_adec_client_data = NULL;
            return hold;
          }
          sndhw_set_av_sync(VOC_AV_SYNC_DEC_AMR_WBPLUS,
                            tone_status_ptr->av_sync_cb,
                            tone_status_ptr->av_sync_mode,
                            tone_status_ptr->av_sync_interval);
          break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

        default:
          return hold;
      }

      sndhw_adec_client_data = tone_status_ptr->audfmt_adec;
      (void) voc_adec_init(tone_status_ptr->audfmt_adec_cb,
                           sndhw_adec_client_data,
                           tone_status_ptr->audfmt_codec_update_cb);
      MSG_HIGH("Decoder ready for playback", 0, 0, 0);
    }
  }
  else {
    hold = TRUE;
  }
  return hold;
}
#endif /* FEATURE_QTUNES_COMMON */

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_gen_tone

DESCRIPTION
  This routine is called by the Generic Sound Task Level to actually begin
  the generation of a tone.

DEPENDENCIES
  sndhw_aux_busy
  sndhw_adsp_busy
  sndhw_hfk_protocol
  sndhw_hfk_vr_tone_ready

RETURN VALUE
  TRUE  - if tone is on hold
  FALSE - if tone is playing

SIDE EFFECTS
  None.

===========================================================================*/
boolean sndhw_gen_tone
(
  tone_status_type *tone_status_ptr
)
{
  boolean         hold = FALSE;
  snd_gen_type    generator;
#ifdef FEATURE_WAVE_PB
  voc_inform_type voc_status;
#endif /* FEATURE_WAVE_PB */

#if defined(FEATURE_QSYNTH_ADPCM) || defined(FEATURE_QSYNTH_PCM)
  uint32                         wave_sample_rate;
  uint32                         adjusted_avsync_interval;
#endif /* FEATURE_QSYNTH_ADPCM || FEATURE_QSYNTH_PCM */

#if defined(FEATURE_AVS_INCALL_MIXER_CMX) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
 voc_state_control_type*  state;
#endif

  /* Generate/continue tone                                             */
  if(tone_status_ptr->type == SND_TYPE_SILENCE) {
    if((sndhw_aux_ctl_fn == NULL) ||
       (tone_status_ptr->aux_type == SND_TYPE_SILENCE)) {
      sndhw_change_gen(SND_GEN_NONE);
    } else {
      sndhw_change_gen(SND_GEN_AUX);
    }
#ifdef FEATURE_MIDI
    if((tone_status_ptr->audfmt_buf != NULL) && (!sndhw_audfmt_need_config)) {
      if(voc_get_status() == VOC_INFORM_QSYNTH) {
        (void) sndhw_audfmt_tone(tone_status_ptr);
      }
    }
#endif /* FEATURE_MIDI */
    if(sndhw_aux_ctl_fn != NULL) {
      if(tone_status_ptr->aux_type == SND_TYPE_SILENCE) {
        if(sndhw_aux_busy) {
          sndhw_aux_busy = FALSE;
          sndhw_aux_ctl_fn(FALSE);
        }
      } else if(!sndhw_aux_busy) {
        sndhw_aux_busy = TRUE;
        sndhw_aux_ctl_fn(TRUE);
      }
    }
  } else {
#ifdef FEATURE_VOICE_PLAYBACK
    if((tone_status_ptr->type != SND_TYPE_VOICE_PROMPT)&&(sndhw_vp_playing))
    {
      /* Queue up voice prompts to be played even if voice prompt is not
      ** the current highest priority sound. Voice prompt timing will begin
      ** so that when the higher priority sound is done, the voice prompt
      ** will be played from that point (same relationship between TONEs and
      ** SOUNDs, etc...)
      */
      sndhw_voc_vp_enq();
    }
#endif

#ifdef FEATURE_MIDI
    if((tone_status_ptr->type != SND_TYPE_AUDFMT)
#ifdef FEATURE_IMELODY
        && (tone_status_ptr->type != SND_TYPE_IMY)
#endif
      ) {
      /* If audio format is not the highest priority sound, send
      ** command buffer if not NULL. The command should automatically mute.
      */
      if((tone_status_ptr->audfmt_buf != NULL) && (!sndhw_audfmt_need_config)) {
        /* We should send MIDI commands during FFWD/RWND of cmx clips in
         * Normal playback and In call CMX scenario */
        if (voc_check_cap_ready(VOC_CAP_QSYNTH) == TRUE) {
          (void) sndhw_audfmt_tone(tone_status_ptr);
        }
      }
    }
#endif

    generator = snd_data_get_generator(tone_status_ptr->device,
                                       tone_status_ptr->method);

    if((generator != SND_GEN_AUX) && (sndhw_aux_ctl_fn != NULL)) {
      if(tone_status_ptr->aux_type == SND_TYPE_SILENCE) {
        if(sndhw_aux_busy) {
          sndhw_aux_busy = FALSE;
          sndhw_aux_ctl_fn(FALSE);
        }
      } else if(!sndhw_aux_busy) {
        sndhw_aux_busy = TRUE;
        sndhw_aux_ctl_fn(TRUE);
      }
    }

    switch (generator) {
      case SND_GEN_VOC:         /* The Vocoder generates these tones      */
        if (snd_path_is_ready != SND_PATH_READY) {
          MSG_MED("Waiting for voc_acquire or reset",0,0,0);
          hold = TRUE;
#ifdef SND_EXTCOM
#error code not present
#endif
        } else {
#ifdef FEATURE_VOICE_PLAYBACK
          if(tone_status_ptr->type == SND_TYPE_VOICE_PROMPT) {
            /* Turn off vocoder tones here, we couldn't turn them off in
            ** sndhw_change_gen() because VP and vocoder tones have the same
            ** generator. Vocoder will not be released because this tone
            ** (i.e. VP) is still active.
            */
            sndhw_voc_tone(NULL);
            /* The vocoder will play a voice prompt */
#ifdef FEATURE_AUDIO_FORMAT
            if(!sndhw_vp_playing) {
              if(sndhw_audfmt_need_config) {
                sndhw_audfmt_need_config =
                          !sndhw_set_sample_rate(tone_status_ptr->sample_rate);
              }

              sndhw_set_av_sync(VOC_AV_SYNC_DEC_VOCODER,
                                tone_status_ptr->av_sync_cb,
                                tone_status_ptr->av_sync_mode,
                                tone_status_ptr->av_sync_interval);
              MSG_HIGH("Ready for QCP playback", 0, 0, 0);
            }
#endif /* FEATURE_AUDIO_FORMAT */
            sndhw_voc_vp(tone_status_ptr);
          } else {
            /* Mute voice prompts because the current tone has higher priority */
            sndhw_voc_vp(NULL);
#if defined(FEATURE_AVS_BT_SCO_REWORK)
            if (tone_status_ptr->method == SND_METHOD_RING) {
              if (!sndhw_bt_playback_dtmf_ring_started &&
                  (sndhw_bt_cb_func_ptr != NULL))
              {
                sndhw_bt_playback_dtmf_ring_started = TRUE;
                sndhw_bt_cb_func_ptr(VOC_PLAYBACK_DTMF_RINGER_START);
              }
            } else {
              if (!sndhw_bt_playback_dtmf_started &&
                  (sndhw_bt_cb_func_ptr != NULL))
              {
                sndhw_bt_playback_dtmf_started = TRUE;
                sndhw_bt_cb_func_ptr(VOC_PLAYBACK_DTMF_START);
              }
            }
#endif /*AVS_BT_SCO_REWORK*/
            sndhw_voc_tone(tone_status_ptr);
          }
#else
#if defined(FEATURE_AVS_BT_SCO_REWORK)
          if (tone_status_ptr->method == SND_METHOD_RING) {
            if (!sndhw_bt_playback_dtmf_ring_started &&
                (sndhw_bt_cb_func_ptr != NULL))
            {
              sndhw_bt_playback_dtmf_ring_started = TRUE;
              sndhw_bt_cb_func_ptr(VOC_PLAYBACK_DTMF_RINGER_START);
            }
          } else {
            if (!sndhw_bt_playback_dtmf_started &&
                (sndhw_bt_cb_func_ptr != NULL))
            {
              sndhw_bt_playback_dtmf_started = TRUE;
              sndhw_bt_cb_func_ptr(VOC_PLAYBACK_DTMF_START);
            }
          }
#endif /*AVS_BT_SCO_REWORK*/
          sndhw_voc_tone(tone_status_ptr);
#endif /* FEATURE_VOICE_PLAYBACK */
        }
        break;

#ifdef FEATURE_AUDIO_FORMAT
      case SND_GEN_MIDI:         /* The Vocoder generates these midi tones    */
        switch (tone_status_ptr->type) {
#ifdef FEATURE_MIDI
          case SND_TYPE_AUDFMT:
#ifdef FEATURE_IMELODY
          case SND_TYPE_IMY:
#endif /* FEATURE_IMELODY */
            if(sndhw_audfmt_need_config) {
#ifdef FEATURE_MIDI_OUT
              if (sndhw_midi_out_enabled) {
                hold = !sndhw_set_sample_rate(VOC_SAMPLE_RATE_32000);
              } else
#endif /* FEATURE_MIDI_OUT */
              {
                hold = !sndhw_set_sample_rate(tone_status_ptr->sample_rate);
              }
              sndhw_audfmt_need_config = hold;
              /* Config DSP */
              sndhw_voc_config(SND_AUDFMT_SOURCE_NORMAL, VOC_CAP_QSYNTH);
            }

            if (!hold) {
              if((voc_check_cap_ready(VOC_CAP_QSYNTH) == FALSE)
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
                || (sndhw_codec_supports_a2dp && voc_is_bt_a2dp_pending())
#endif
#if defined(FEATURE_BT_AG) && defined (FEATURE_AVS_BT_SCO_REWORK)
                || (sndhw_codec_supports_ag && voc_is_bt_ag_pending())
#endif /*AVS_BT_SCO_REWORK*/
                 ){
                hold = TRUE;
#if defined(FEATURE_AVS_INCALL_MIXER_CMX) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
                state = voc_state_get_state_data();
                if(check_interlock(VOC_UNLOCK_VOCODER) == TRUE){
                   voc_set_audfmt_mix_mode(VOC_ADEC_MIX_MODE_RX);
                }
#endif
              } else {
                hold = sndhw_audfmt_tone(tone_status_ptr);/*Send midi commands*/
#ifdef FEATURE_WEBAUDIO
                if(sndhw_adec_client_data == NULL) {
                  sndhw_adec_client_data = tone_status_ptr->audfmt_adec;
                  (void) voc_adec_init(tone_status_ptr->audfmt_adec_cb,
                                       sndhw_adec_client_data,
                                       tone_status_ptr->audfmt_codec_update_cb);
                  if(tone_status_ptr->audfmt_adec_cb != NULL) {
                    tone_status_ptr->audfmt_adec_cb(sndhw_adec_client_data);
                  }
                }
#endif /* FEATURE_WEBAUDIO */
              }
            }
            break;
#endif /* FEATURE_MIDI */

#ifdef FEATURE_MP3
          case SND_TYPE_MP3:
            hold = sndhw_qtunes_gen_tone(tone_status_ptr, VOC_CAP_QTUNES);
            break;
#endif /* FEATURE_MP3 */

#ifdef FEATURE_AAC
          case SND_TYPE_AAC:
            hold = sndhw_qtunes_gen_tone(tone_status_ptr, VOC_CAP_QTUNES_AAC);
            break;
#endif /* FEATURE_AAC */

#ifdef FEATURE_REAL_AUDIO
          case SND_TYPE_RA:
            hold = sndhw_qtunes_gen_tone(tone_status_ptr, VOC_CAP_QTUNES_RA);
            break;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
          case SND_TYPE_WMA:
            hold = sndhw_qtunes_gen_tone(tone_status_ptr, VOC_CAP_QTUNES_WMA);
            break;
          case SND_TYPE_WMA_PRO:
            hold = sndhw_qtunes_gen_tone(tone_status_ptr, VOC_CAP_QTUNES_WMA_PRO);
            break;

#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
          case SND_TYPE_AMR_WBPLUS:
            hold = sndhw_qtunes_gen_tone(tone_status_ptr,
                                         VOC_CAP_QTUNES_AMR_WBPLUS);
            break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
          case SND_TYPE_ADPCM:
#endif /* FEATURE_ADPCM */
#ifdef FEATURE_PCM
          case SND_TYPE_PCM:
#endif /* FEATURE_PCM */
#ifdef FEATURE_VOC_G711
          case SND_TYPE_WAV_MULAW:
          case SND_TYPE_WAV_ALAW:
#endif /* FEATURE_VOC_G711 */
            if(sndhw_audfmt_need_config) {
              sndhw_audfmt_need_config =
                        !sndhw_set_sample_rate(tone_status_ptr->sample_rate);
              /* Config DSP */
#ifdef FEATURE_VOC_G711
              if(tone_status_ptr->type == SND_TYPE_WAV_MULAW) {
                voc_set_wav_mode(VOC_ADPCM_WAV_MULAW_MODE);
              } else
              if(tone_status_ptr->type == SND_TYPE_WAV_ALAW) {
                voc_set_wav_mode(VOC_ADPCM_WAV_ALAW_MODE);
              } else
#endif /* FEATURE_VOC_G711 */
              {
                voc_set_adpcm_mode(TRUE);
              }
            }

            voc_status = voc_get_status();
            switch(voc_status) {
#ifdef FEATURE_GRAPH_AUDIO
              case VOC_INFORM_GRAPH_IDLE:
              case VOC_INFORM_GRAPH_AUDIO:
#ifdef FEATURE_PCM
                if (tone_status_ptr->type == SND_TYPE_PCM) {
                  hold = TRUE;
                  break;
                }
                /* Fall thru */
#endif /* FEATURE_PCM */
#endif /* FEATURE_GRAPH_AUDIO */

              case VOC_INFORM_VOCODER:
#ifdef FEATURE_VOC_ADPCM
              case VOC_INFORM_IDLE:
              case VOC_INFORM_VOCODER_ADPCM:
#endif /* FEATURE_VOC_ADPCM */
                /* The default sample rate will be used. Force success */
                sndhw_audfmt_need_config = FALSE;
                hold = FALSE;

                if (snd_crnt_path_out.mute == SND_MUTE_UNMUTED) {
                  sndhw_rx_mute_ctl(FALSE);
                }

#ifdef FEATURE_QSYNTH_COMMON
              case VOC_INFORM_QSYNTH:
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
              case VOC_INFORM_CAM_AUDIO_ACTIVE:
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
                if (sndhw_codec_supports_a2dp && voc_is_bt_a2dp_pending())
                {
                  hold = TRUE;
                  break;
                }
#endif
#if defined(FEATURE_BT_AG) && defined (FEATURE_AVS_BT_SCO_REWORK)
                if (sndhw_codec_supports_ag && voc_is_bt_ag_pending())
                {
                  hold = TRUE;
                  break;
                }
#endif/*AVS_BT_SCO_REWORK*/
                if ( (voc_status == VOC_INFORM_QSYNTH)
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
            ||(voc_status == VOC_INFORM_CAM_AUDIO_ACTIVE)
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
            ){
               hold = sndhw_audfmt_need_config;
             }
#endif /* FEATURE_QSYNTH_COMMON */
                if(sndhw_adec_client_data == NULL) {
                  sndhw_adec_client_data = tone_status_ptr->audfmt_adec;
                  (void) voc_adec_init(tone_status_ptr->audfmt_adec_cb,
                                       sndhw_adec_client_data,
                                       tone_status_ptr->audfmt_codec_update_cb);
#if defined(FEATURE_QSYNTH_ADPCM) || defined(FEATURE_QSYNTH_PCM)
                  if (tone_status_ptr->av_sync_mode == SND_AV_SYNC_TIME_MS) {
                    wave_sample_rate = ((snd_sound_status_type*) tone_status_ptr
                                       -> audfmt_adec)->sound_ptr
                                       -> format_sound.input_spec. \
                                       wave_codec.sample_rate;
                    adjusted_avsync_interval = tone_status_ptr->
                                               av_sync_interval *
                                               (wave_sample_rate / 1000);
                  } else {
                    adjusted_avsync_interval = tone_status_ptr->av_sync_interval;
                  }
                  sndhw_set_av_sync(VOC_AV_SYNC_DEC_WAVE,
                                tone_status_ptr->av_sync_cb,
                                SND_AV_SYNC_SAMPLES,
                                adjusted_avsync_interval);
                  MSG_HIGH("Qsynth ADEC ready for playback", 0, 0, 0);
#endif /* FEATURE_QSYNTH_ADPCM || FEATURE_QSYNTH_PCM */
                  if(tone_status_ptr->audfmt_adec_cb != NULL) {
                    tone_status_ptr->audfmt_adec_cb(sndhw_adec_client_data);
                  }
                }
                break;

              default:
                hold = TRUE;
            }
            break;
#endif /* FEATURE_WAVE_PB */

          default:
            break;
        }      /* switch(tone_status_ptr->type) */

        break; /* case SND_GEN_MIDI:            */
#endif /* FEATURE_AUDIO_FORMAT */

      case SND_GEN_RING:
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
        switch (tone_status_ptr->device) {
#ifdef FEATURE_BT_AG
          case SND_DEVICE_BT_HEADSET:
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
          case SND_DEVICE_BT_A2DP_SCO_HEADSET:
#endif /* FEATURE_SBC_CODEC */
            if(tone_status_ptr->method == SND_METHOD_RING) {
             if ( !sndhw_bt_hs_ringing )
             {
               sndhw_bt_hs_ringing = TRUE;
             }
            }
          break;
        }
#if defined (FEATURE_BT_AG) && (defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC))
        if ((sndhw_bt_ag_connected || sndhw_bt_a2dp_connected ) &&
#elif defined (FEATURE_BT_AG)
        if ((sndhw_bt_ag_connected) &&
#elif defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
        if ((sndhw_bt_a2dp_connected) &&
#endif
            ((tone_status_ptr->method == SND_METHOD_KEY_BEEP) ||
              (tone_status_ptr->method == SND_METHOD_MESSAGE))) {
          /* If BT is connected we should be playing on the BT connection so
          send it to vocoder */
          sndhw_voc_tone(tone_status_ptr);

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
          if((sndhw_bt_cb_func_ptr != NULL)&&
             (!sndhw_get_bt_playback_dtmf_start())
            )
          {
            MSG_MED("Calling BT callback with playback DTMF start",0,0,0);
            sndhw_bt_cb_func_ptr(VOC_PLAYBACK_DTMF_START);
            sndhw_set_bt_playback_dtmf_start(TRUE);
          }
#endif/*AVS_BT_SCO_REWORK*/
          break;
        }
#endif /* FEATURE_BT_AG || FEATURE_SBC_CODEC */
        /* If dtmf_path == TX or Both then vocoder needs to send dtmfs    */
        if (tone_status_ptr->dtmf_path != SND_APATH_LOCAL) {
          sndhw_voc_tone(tone_status_ptr);
        }

        /* If we require only TX then don't need to play sound locally     */

        if (tone_status_ptr->dtmf_path == SND_APATH_TX) {
          sndhw_ring(NULL);            /* Stop tone                    */
          break;
        }

        sndhw_ring(tone_status_ptr);   /* Generate tone                 */
        break;

#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */

      case SND_GEN_AUX:          /* The auxiliary alert generator turns on */
        if(sndhw_aux_ctl_fn != NULL) {
          sndhw_aux_busy = TRUE;
          sndhw_aux_ctl_fn(TRUE); /* Turn on the auxiliary generator       */
        }
        break;

      case SND_GEN_NONE:        /* No sound generator (silence)            */
        sndhw_change_gen(SND_GEN_NONE);
        break;

      default:                  /* Illegal generator selection             */
        ERR_FATAL("Illegal tone gen D=%x M=%x ",tone_status_ptr->device,
                                                tone_status_ptr->method, 0);
        /* Does not return */
    }
  }
  return(hold);
} /* sndhw_gen_tone */

/* <EJECT>*/
#ifdef FEATURE_ANIMATED_AAC
/*===========================================================================

FUNCTION sndhw_aac_set_song_config

DESCRIPTION
  Sets the DSP configuration for AAC

DEPENDENCIES
  The Bluetooth task must have been initialized first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_aac_set_song_config( voc_aac_config_type* config){

  return voc_aac_set_song_config(config);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_aac_mpeg_header_config

DESCRIPTION
  Sets the MPEG header configuration for AAC

DEPENDENCIES
  AAC DSP Image must be loaded already.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_aac_set_mpeg_header( snd_audfmt_mpeg_header_config* config){

  return voc_mpeg_set_song_config( config->mpeg_mask, config->mpeg_header,
                                  config->mpeg_mask_size, VOC_CAP_QTUNES_AAC);
}


#endif

/* <EJECT>*/
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
/*===========================================================================

FUNCTION sndhw_bt_ev_cb_func

DESCRIPTION
  Function to receive responses issued through app_id registration.
  Please note: The Sound Server does not currently act on any BT events,
               this callback function is provided to the BT driver in order
               to get a valid app ID. This callback function can be modified
               to act on BT events in the future if needed.

DEPENDENCIES
  The Bluetooth task must have been initialized first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_bt_ev_cb_func (
  struct bt_ev_msg_struct* bt_ev_msg_ptr
) {
  /*lint -e788 Tell Lint we know not every enum is in the switch */
  switch(bt_ev_msg_ptr->ev_hdr.ev_type) {
    /* Response to bt_cmd_ag_audio_disconnect() */
    case BT_EV_AG_AUDIO_DISCONNECTED:
      sndhw_bt_ag_connected = FALSE;
#if defined (FEATURE_AVS_BT_SCO_REWORK)
      sndhw_bt_sco_connect_evt_rcvd = FALSE;
      MSG_HIGH("Recvd AG Disconnect without AVS initiating it", 0,0,0);
#endif
      /*update the state variable that Sco got disconnected.*/
      voc_set_sco_disconnected();
      MSG_MED("BT_EV_AG_AUDIO_DISCONNECTED event received", 0,0,0);
      break;

    /* General command done event */
    case BT_EV_GN_CMD_DONE:
      break;

    /* Response to bt_cmd_ag_audio_connect() */
    case BT_EV_AG_AUDIO_CONNECTED:
      /* Inform vocoder driver of BT connected */
      MSG_MED("BT_EV_AG_AUDIO_CONNECTED event received", 0,0,0);
#if defined (FEATURE_AVS_BT_SCO_REWORK)
      sndhw_bt_sco_connect_evt_rcvd = TRUE;
      voc_set_sco_connected();
#endif
      voc_complete_bt_ag();
      break;

    /* Response to bt_cmd_ag_enable() */
    case BT_EV_AG_ENABLED:
      break;
    /* Response to bt_cmd_ag_disable() */
    case BT_EV_AG_DISABLED:
    MSG_MED("BT_EV_AG_DISABLED event received", 0,0,0);
      sndhw_bt_ag_connected = FALSE;
#if defined (FEATURE_AVS_BT_SCO_REWORK)
      voc_set_sco_disconnected();
      sndhw_bt_sco_connect_evt_rcvd = FALSE;
#endif
      break;
    /* Report of HS button pressed */
    case BT_EV_AG_BUTTON_PRESSED:
    /* Response to bt_cmd_ag_set_hs_spkr_gain() */
    case BT_EV_AG_SPKR_GAIN_REPORT:
    /* Response to bt_cmd_ag_set_hs_mic_gain() */
    case BT_EV_AG_MIC_GAIN_REPORT:
    break;

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    case BT_EV_PF_A2DP_START:
      /* Inform the vocoder driver of BT A2DP connection */
      voc_complete_bt_a2dp();
      MSG_MED("BT_EV_PF_A2DP_START event received", 0,0,0);
      break;

    case BT_EV_PF_A2DP_DISCON:
      sndhw_bt_a2dp_connected = FALSE;
      MSG_MED("BT_EV_PF_A2DP_DISCON event received ", 0,0,0);
      break;

    case BT_EV_PF_A2DP_SUSPEND:
      sndhw_bt_a2dp_connected = FALSE;
      voc_set_a2dp_disconnected();
      MSG_MED("BT_EV_PF_A2DP_SUSPEND event received ", 0,0,0);
    /* Confirm for Suspend comamnd sent by Audio */
      break;

    case BT_EV_PF_A2DP_CON:
      MSG_MED("BT_EV_PF_A2DP_CON event received ", 0,0,0);
      voc_set_bt_a2dp_remote_params(
           bt_ev_msg_ptr->ev_msg.ev_a2dp_con.sbc_config);
      break;
#endif

    default:
      break;
  }
  /*lint +e788 turn message back on for other switch() statements */
}

#ifdef FEATURE_BT_AG
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_ag_enabled_cb

DESCRIPTION
  This callback function gets called when AG has been succesfully enabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_ag_enabled_cb(void)
{
#if defined (FEATURE_AVS_BT_SCO_REWORK)
 if (sndhw_codec_supports_ag && sndhw_bt_sco_connect_evt_rcvd)
 {

  sndhw_bt_ag_connected = TRUE;
    switch (voc_get_status()) {
#if defined (FEATURE_QSYNTH_COMMON) || defined (FEATURE_QTUNES_COMMON)
#ifdef FEATURE_QSYNTH_COMMON
      case VOC_INFORM_QSYNTH:
#endif
#ifdef FEATURE_QTUNES_COMMON
      case VOC_INFORM_QTUNES:
#endif
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
      case VOC_INFORM_CAM_AUDIO_ACTIVE:
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
#ifdef FEATURE_GRAPH_AUDIO
      case VOC_INFORM_GRAPH_AUDIO:
#endif /* For QTV amr NB on SCO link */

        /* Trigger SND to start playing tones */
        (void) rex_set_sigs( &snd_tcb, SND_TONE_TIMER_SIG );
        break;
#endif
      default:
        MSG_ERROR("ag_enabled_cb BT conn not up yet",0,0,0);
        break;
    }
  }
#else
  sndhw_bt_ag_connected = TRUE;
#endif

}
#endif
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_a2dp_enabled_cb

DESCRIPTION
  This callback function gets called when A2DP has been succesfully enabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sndhw_a2dp_enabled_cb(void)
{
  sndhw_bt_a2dp_connected = TRUE;
  if (sndhw_codec_supports_a2dp) {
    switch (voc_get_status()) {
#if defined (FEATURE_QSYNTH_COMMON) || defined (FEATURE_QTUNES_COMMON)
#ifdef FEATURE_QSYNTH_COMMON
      case VOC_INFORM_QSYNTH:
#endif
#ifdef FEATURE_QTUNES_COMMON
      case VOC_INFORM_QTUNES:
#endif
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
      case VOC_INFORM_CAM_AUDIO_ACTIVE:
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */

        /* Trigger SND to start playing tones */
        (void) rex_set_sigs( &snd_tcb, SND_TONE_TIMER_SIG );
        break;
#endif
      default:
        break;
    }
  }
}
#endif

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_bt_init

DESCRIPTION
  Initialization for Bluetooth Audio Gateway communication.

DEPENDENCIES
  The Bluetooth task must have been initialized first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_bt_init( void ) {
  sndhw_bt_handle = bt_cmd_ec_get_application_id();

  /* Register for events */
#ifdef FEATURE_BT_AG
  bt_cmd_ec_reg_event_set_any_app_id(sndhw_bt_handle, sndhw_bt_ev_cb_func,
                                     BT_EC_ES_CUSTOM, BT_EV_AG_AUDIO_CONNECTED,
                                     BT_EV_AG_AUDIO_CONNECT_FAILED);
  voc_register_ag_enabled_cb (sndhw_ag_enabled_cb);
#endif /* FEATURE_BT_AG */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  bt_cmd_ec_reg_event_set_any_app_id(sndhw_bt_handle, sndhw_bt_ev_cb_func,
                                    BT_EC_ES_CUSTOM, BT_EV_PF_A2DP_CON,
                                    BT_EV_PF_A2DP_SUSPEND);
  voc_register_a2dp_enabled_cb (sndhw_a2dp_enabled_cb);
#endif /* FEATURE_SBC_CODEC */

  /* Give BT app Id to vocoder driver */
  voc_set_bt_app_id(sndhw_bt_handle);

  if (sndhw_bt_handle == BT_APP_ID_NULL) {
    MSG_ERROR ("SND BT: error initializing client-%d", sndhw_bt_handle, 0, 0);
  }
}

#endif /* defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) */

/*===========================================================================

FUNCTION sndhw_set_qtv_cb_func

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
void sndhw_set_qtv_cb_func(voc_notify_qtv_cb_func_ptr_type cb_func)
{
  voc_set_qtv_cb_func((voc_notify_qtv_cb_func_ptr_type)cb_func);
}


#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
/*===========================================================================

FUNCTION sndhw_set_bt_cb

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
void sndhw_set_bt_cb_func(voc_bt_cb_func_ptr_type cb_func)
{
  sndhw_bt_cb_func_ptr = cb_func;
  voc_set_bt_cb_func((voc_bt_cb_func_ptr_type)cb_func);
}

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_cleanup_tone

DESCRIPTION
  Calls the BT functions to let BT know that we have finished playing a tone

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_cleanup_tone(void)
{

  if (sndhw_bt_playback_dtmf_ring_started && (sndhw_bt_cb_func_ptr != NULL))
  {
    sndhw_bt_playback_dtmf_ring_started = FALSE;
    sndhw_bt_cb_func_ptr(VOC_PLAYBACK_DTMF_RINGER_STOP);
  }
  else if (sndhw_bt_playback_dtmf_started && (sndhw_bt_cb_func_ptr != NULL))
  {
    sndhw_bt_playback_dtmf_started = FALSE;
    sndhw_bt_cb_func_ptr(VOC_PLAYBACK_DTMF_STOP);
  }
  else if (sndhw_bt_playback_other_started && (sndhw_bt_cb_func_ptr != NULL))
  {
    sndhw_bt_playback_other_started = FALSE;
    sndhw_bt_cb_func_ptr(VOC_PLAYBACK_OTHER_STOP);
  }
}
/*===========================================================================

FUNCTION sndhw_get_bt_playback_other_start

DESCRIPTION
  This function is used to get the status of the other playback event
  that is sent to BT driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
boolean sndhw_get_bt_playback_other_start(void)
{
  return sndhw_bt_playback_other_started;
}
/*===========================================================================

FUNCTION sndhw_set_bt_playback_other_start

DESCRIPTION
  This function is used to set the status of the other playback event
  that is sent to BT driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void sndhw_set_bt_playback_other_start(boolean bt_other_play_evt_status)
{
  sndhw_bt_playback_other_started = bt_other_play_evt_status;
}
/*===========================================================================

FUNCTION sndhw_get_bt_playback_dtmf_start

DESCRIPTION
  This function is used to get the status of the dtmf playback event
  that is sent to BT driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
boolean sndhw_get_bt_playback_dtmf_start(void)
{
  return voc_get_bt_playback_dtmf_start();
}
/*===========================================================================
FUNCTION sndhw_set_bt_playback_dtmf_start

DESCRIPTION
  This function is used to set the status of the dtmf playback event
  that is sent to BT driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void sndhw_set_bt_playback_dtmf_start(boolean bt_dtmf_play_evt_status)
{
  voc_set_bt_playback_dtmf_start(bt_dtmf_play_evt_status);
}

#endif /* FEATURE_BT_AG|FEATURE_SBC_CODEC|FEATURE_SBC_DSP_CODEC&AVS_BT_SCO_REWORK*/



#if defined FEATURE_SBC_CODEC || defined FEATURE_SBC_DSP_CODEC || \
    defined FEATURE_AVS_WB_PCM_RX_INTERFACE
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_free_wb_pcm_rx_interface_buffer

DESCRIPTION
  Frees the wb_pcm_rx_interface_buffer

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_free_wb_pcm_rx_interface_buffer( uint16 index )
{
  voc_free_wb_pcm_rx_interface_buffer(index);
}
#endif /* defined (FEATURE_SBC_CODEC) ||  FEATURE_AVS_WB_PCM_RX_INTERFACE*/
/* <EJECT>*/

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/*===========================================================================

FUNCTION sndhw_bt_a2dp_suspend

DESCRIPTION
  Suspend the BT A2DP connection

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_bt_a2dp_suspend(void)
{
  sndhw_bt_a2dp_connected = FALSE;
  voc_bt_a2dp_suspend();
}

#endif /* defined (FEATURE_SBC_CODEC) */

/* <EJECT>*/
#ifdef FEATURE_SBC_DSP_CODEC
/*===========================================================================

FUNCTION sndhw_sbc_set_bitrate

DESCRIPTION
  Sets SBC bitrate to the DSP

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_sbc_set_bitrate(uint32 bitrate)
{
  voc_sbc_set_bitrate(bitrate);
}
/*===========================================================================

FUNCTION sndhw_sbc_configure
  This function sets the SBC configuration information with the audio drivers.
  When a DSP image that supports SBC gets loaded onto ADSP audio driver will
  configure the SBC encoder in DSP with these parameters.

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_sbc_configure(uint32 blocks, uint32 mode, uint32 alloc)
{
  voc_sbc_config_params_type config;
  config.alloc  = alloc;
  config.blocks = blocks;
  config.mode   = mode;
  voc_sbc_configure(&config);
}
#endif /* FEATURE_SBC_DSP_CODEC */

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_init

DESCRIPTION
  Generic Low Level Hardware Initialization for Sound Functions and chips.

DEPENDENCIES
  The Timers and Queues in snd_init() must have been initialized first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_init( void )
{

  /* Initialize ringer */
#ifndef FEATURE_NO_EXT_RINGER
  ring_init();
#endif

  rex_def_timer( &snd_override_timer, &snd_tcb, SND_OVERRIDE_TIMER_SIG );
  rex_def_timer( &snd_unmute_timer, &snd_tcb, SND_UNMUTE_SIG);
  rex_def_timer( &snd_path_timer, &snd_tcb, SND_PATH_TIMER_SIG );

#ifdef FEATURE_AUDIO_FORMAT
  rex_def_timer( &snd_voc_module_timer, &snd_tcb, SND_VOC_MODULE_STATUS);
#endif

#ifdef SND_EXTCOM
#error code not present
#endif
  /* Regestering CB function when PAs are turned on, in the VOC context, this
     registerd callback function is called, which posts a command in SND queue
     and wakes up the sound task */
  voc_register_pa_on(snd_pa_on_cb);
#ifdef FEATURE_PMIC_SPEAKER
#ifdef FEATURE_K_AMPLIFIER
  (void) voc_register_codec_func( VOC_CODEC_ON_CHIP_AUX,
                                  sndhw_kamp_ctl);
#ifdef FEATURE_SPEAKER_PHONE
  (void) voc_register_codec_func( VOC_CODEC_SPEAKER, sndhw_kamp_ctl);
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM
  (void) voc_register_codec_func( VOC_CODEC_SPEAKER_FM, sndhw_kamp_ctl);
#endif
#else
  (void) voc_register_codec_func( VOC_CODEC_ON_CHIP_AUX,
                                  sndhw_pmic_speaker_ctl);
#ifdef FEATURE_SPEAKER_PHONE
  (void) voc_register_codec_func( VOC_CODEC_SPEAKER, sndhw_pmic_speaker_ctl);
#endif /* FEATURE_SPEAKER_PHONE */
#ifdef FEATURE_FM_OEM
  (void) voc_register_codec_func( VOC_CODEC_SPEAKER_FM, sndhw_pmic_speaker_ctl);
#endif
#endif
#if defined(FEATURE_AUDIO_CAL_MED_HIGH_FFA)
#error code not present
#else
  #ifndef MSMAUD_SCMM
  pm_set_speaker_gain(PM_SPKR_GAIN_PLUS12DB);
  #else
#error code not present
  #endif /* MSMAUD_SCMM */
#endif /* defined(FEATURE_AUDIO_CAL_MED_HIGH_FFA) */
#endif /* FEATURE_PMIC_SPEAKER */

#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
  (void) voc_register_codec_func( VOC_CODEC_USB, sndhw_usb_ctl );
#ifdef FEATURE_INTERNAL_SDAC
  (void) voc_register_codec_func( VOC_CODEC_STEREO_USB, sndhw_usb_ctl );
#endif /* FEATURE_INTERNAL_SDAC */
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */

  snd_setup_voice_path(SND_DEVICE_HANDSET,SND_MUTE_MUTED,&snd_crnt_path_out);

  /* While it is true that we generally come up "muted", we have to     */
  /* initialize the Codec Muxes to something known and "phone" is the   */
  /* logical choice.                                                    */

  sndhw_which_codec(&snd_crnt_path_out);

#ifdef SND_HFK_PLUS
#error code not present
#endif

  /* All the timers are ready now, so I can call the routine to turn    */
  /* off the ringer and bring it to a known state.                      */

  sndhw_ring(NULL);

  /* Mute the RX Path (Vocoder).                                        */
  /* Note: changed from TRUE to FALSE on 08/22/95                       */
  sndhw_rx_mute_ctl(FALSE);

} /* end of sndhw_init */
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_filters_ctl

DESCRIPTION
  Some targets allow different types of "Filtering" to be done by Sound.
  For instance, most data calls need VOXing turned off.

DEPENDENCIES
  snd_current_filters

RETURN VALUE
  None

SIDE EFFECTS
  snd_current_filters

===========================================================================*/
void sndhw_filters_ctl
(
  snd_filters_type filters
    /* Type of Filtering Current Desired */
)
{

  /* I set the global variable to indicate what kind of filtering we    */
  /* are being requested to do and then call any routines that might    */
  /* need to look at this change.                                       */

  if (snd_current_filters != filters)
    {
    SND_DEBUG_MSG("Sound Filters Now = %d",filters,0,0);
    snd_current_filters = filters;
    sndhw_set_volume();
    }

}

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT>*/
/*===========================================================================

REGIONAL FUNCTION sndhw_set_int_volume

DESCRIPTION
  Set the volume for a given device and method in the database.

DEPENDENCIES
  none

RETURN VALUE
  TRUE  - if a new volume setting is made.
  FALSE - if no volume setting was necessary.

SIDE EFFECTS
  none

===========================================================================*/
boolean sndhw_set_int_volume (
  snd_device_type device,
  snd_method_type method,
  byte            volume
) {
  boolean retval = FALSE;
  byte    tvol;

  tvol = snd_data_get_volume( device, method );
  if(tvol != volume) {
    snd_data_set_volume( device, method, volume );

#if defined(FEATURE_BT_DEVICE_VOLUME_CONTROL)
#error code not present
#endif /* FEATURE_BT_DEVICE_VOLUME_CONTROL */

    retval = TRUE;
  }
  return(retval);
}

/* <EJECT>*/
/*===========================================================================

REGIONAL FUNCTION sndhw_set_volume

DESCRIPTION
  Set the output volume of the vocoder according to the current output path.
  This includes muting if the output is muted.

DEPENDENCIES
  snd_crnt_path_out

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void sndhw_set_volume( void )
{
  sint15  rx_volume;
  uint8   pad_value;
  uint8   pad_mask;
#if defined(FEATURE_QTUNES_COMMON) || defined(FEATURE_MIDI) || \
    defined(FEATURE_QSYNTH_ADPCM)
  sint15  db_volume;
#endif

  snd_device_type device;
  snd_method_type method;
  byte            volume;

  /* Stored volume last set on HFK while in TGP protocol so that we don't
  ** have to set it every time.
  */
#ifdef SND_EXTCOM
#error code not present
#endif

  voc_status_type voc_status;    /* Temporary for voc_vol_cmd() call      */

#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  sint15  incall_rx_volume;
  byte    incall_volume;
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */

  if ((snd_voc_acquired) && (!snd_voc_is_ok)) {
    MSG_HIGH("Delaying volume setting",0,0,0);
    return;
  }

  device = snd_crnt_path_out.device;
  method = snd_crnt_path_out.method;
  volume = snd_crnt_path_out.vol;

  if((volume == SND_VOL_USE_DEFAULT)
#ifdef FEATURE_MIDI_OUT
     || ((volume == 0) && (sndhw_midi_out_enabled))
#endif /* FEATURE_MIDI_OUT */
    ) {
    volume = snd_data_get_volume(device, method);
  }

  /* If BIO_SW10_M is set (available only via a DM) then the Vocoder  */
  /* guys want NO FILTERS and VOC_UNITY_GAIN only.  In this case,     */
  /* override the database volume and do it the way they want.        */
  /* Also if Data filters is set.                                     */
  if(BIO_GET_SW(BIO_SW10_M) || (snd_current_filters == SND_FILTERS_DATA)) {
    snd_data_set_volume_override( TRUE );
  } else {
    snd_data_set_volume_override( FALSE );
  }

  if (snd_voc_is_ok) {
    rx_volume = snd_data_get_rx_volume(device, method, volume);
    pad_value = snd_data_get_pad_value(device, method, volume);
    pad_mask  = snd_data_get_pad_mask(device, method);
#if defined(FEATURE_QTUNES_COMMON) || defined(FEATURE_MIDI) || \
    defined(FEATURE_QSYNTH_ADPCM)
    db_volume = snd_data_get_midi_volume(device, SND_METHOD_MIDI, volume);
#endif

#ifdef SND_EXTCOM
#error code not present
#else
    voc_codec_pad(pad_value, pad_mask);
#endif

#if defined(FEATURE_QTUNES_COMMON) || defined(FEATURE_MIDI) || \
    defined(FEATURE_QSYNTH_ADPCM) || defined(FEATURE_MIDI_OUT)
    if((method == SND_METHOD_MIDI)
#ifdef FEATURE_MIDI_OUT
       || (sndhw_midi_out_enabled)
#endif /* FEATURE_MIDI_OUT */
      ) {
      (void) voc_master_volume(db_volume);
    }
#endif

    voc_status = voc_vol_cmd(rx_volume);
    if (voc_status != VOC_DONE_S) {
      ERR("voc_vol_cmd(%d) Error=%d",rx_volume,voc_status,0);
    }

    /* Now unmute the rx voice path if no sound is playing.
    */
    if ( !sndhw_tone_playing ) {
      snd_rx_mute_ctl( SND_MUTE_SND, FALSE );
    }
#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)

    else  {
      switch (voc_get_status()) {
#ifdef FEATURE_AVS_INCALL_ADEC_PB
        case VOC_INFORM_VOCODER_ADEC:
#endif
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
        case VOC_INFORM_MIXER_ACTIVE:
#endif
          /* un-mute tx path */
          sndhw_set_apath_in(SND_MUTE_UNMUTED);

          /* un-mute rx path */
          snd_rx_mute_ctl(SND_MUTE_SNDV, FALSE);

          incall_volume = snd_data_get_volume(device, SND_METHOD_VOICE);

          incall_rx_volume = snd_data_get_rx_volume(device, SND_METHOD_VOICE,
                                                    incall_volume);

          voc_status = voc_vol_cmd(incall_rx_volume);
          if (voc_status != VOC_DONE_S) {
            ERR("INCALL voc_vol_cmd(%d) Error=%d",
                incall_rx_volume,voc_status,0);
          }
          break;

        default:
          break;
      } /* switch */
    }
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */
  } /* if (snd_voc_is_ok) */
} /* end of sndhw_set_volume */

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_ec_ctl

DESCRIPTION
  Controls EC (Echo Canceller).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_ec_ctl (
  snd_ec_ns_mode_type   ec_mode
) {
  voc_ec_ns_mode_type voc_cmd;

  if(ec_mode == SND_EC_NS_FORCE_DISABLED) {
    voc_cmd = VOC_EC_NS_FORCE_DISABLED;
  } else {
    voc_cmd = VOC_EC_NS_NORMAL_OPERATION;
  }
  voc_set_ec_override_mode(voc_cmd);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_ns_ctl

DESCRIPTION
  Controls NS (Noise Suppressor).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_ns_ctl (
  snd_ec_ns_mode_type   ns_mode
) {
  voc_ec_ns_mode_type voc_cmd;

  if(ns_mode == SND_EC_NS_FORCE_DISABLED) {
    voc_cmd = VOC_EC_NS_FORCE_DISABLED;
  } else {
    voc_cmd = VOC_EC_NS_NORMAL_OPERATION;
  }
  voc_set_ns_override_mode(voc_cmd);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_vox_ctl

DESCRIPTION
  Controls VOX (half/full duplex voice mode).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_vox_ctl (
  snd_vox_mode_type   vox_mode
) {
  voc_vox_mode_type   voc_cmd;

  if(vox_mode == SND_VOX_FORCE_ENABLED) {
    voc_cmd = VOC_VOX_FORCE_ENABLED;
  } else {
    voc_cmd = VOC_VOX_NORMAL_OPERATION;
  }
  voc_set_vox_override_mode(voc_cmd);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_sidetone_ctl

DESCRIPTION
  Controls sidetone.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_sidetone_ctl (
  snd_sidetone_mode_type   sidetone_mode
) {
  voc_sidetone_mode_type   sidetone_cmd;

  if(sidetone_mode == SND_SIDETONE_FORCE_DISABLED) {
    sidetone_cmd = VOC_SIDETONE_FORCE_DISABLED;
  } else {
    sidetone_cmd = VOC_SIDETONE_NORMAL_OPERATION;
  }
  voc_set_sidetone_override_mode(sidetone_cmd);
}

/* <EJECT>*/

#ifdef FEATURE_AUDIO_AGC
/*===========================================================================

REGIONAL FUNCTION sndhw_avc_ctl

DESCRIPTION
  Controls whether AVC (automatic volume control) is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_avc_on

===========================================================================*/
void sndhw_avc_ctl(boolean avc_on)
{
  sndhw_avc_on = avc_on;
  sndhw_aud_process_ctl();
}

/*===========================================================================

REGIONAL FUNCTION sndhw_agc_ctl

DESCRIPTION
  Controls whether AGC (automatic gain control) is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_agc_on

===========================================================================*/
void sndhw_agc_ctl(boolean agc_on)
{
  sndhw_agc_on = agc_on;
  sndhw_aud_process_ctl();
}

#endif

/* <EJECT>*/

/*==========================================================================

REGIONAL FUNCTION sndhw_smartsnd_ctl

DESCRIPTION
  Controls AGC/AVC/RVE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_smartsnd_ctl


==========================================================================*/

void sndhw_smartsnd_ctl(snd_smartsnd_mode_type smartsnd)
{
  sndhw_smartsnd_status = smartsnd;
  sndhw_smartsnd_aud_process_ctl();
}


/* <EJECT>*/
/*===========================================================================

REGIONAL FUNCTION sndhw_set_apath_in

DESCRIPTION
  Mute or Unmute the input path.

DEPENDENCIES
  The Sound Task must have been started in order to initialize the hardware.
  The physical input path is the same as the output path, so this routine
  controls input path muting only.
  snd_analog_mode

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_set_apath_in
(
  snd_mute_control_type   mute
    /* Mute State */
)
{
  boolean voc_tx_mute;
    /* Are we to Mute or Unmute for this call? */

  /*lint -e568 */
  if (mute >= SND_MUTE_MAX) {
  /*lint +e568 */
    ERR_FATAL("Illegal muting control",0,0,0);
  }

  MSG_MED("Setting input path muting = %d", mute, 0, 0);

  sndhw_crnt_path_in_mute = mute;

  if (mute == SND_MUTE_MUTED) {
    voc_tx_mute = TRUE;         /* Mute the Vocoder transmit output     */
  } else {
    sndhw_voc_acquire();
    voc_tx_mute = FALSE;
  }

  /* Set VOX mode to the appropriate state                              */

  sndhw_smartsnd_aud_process_ctl();

  /* In Analog mode, we need to use the "Generic Level" TX mute         */
  /* command since there are several possible "muters".                 */
#ifdef FEATURE_ACP
#error code not present
#endif
  {
    (void) voc_tx_mute_cmd(voc_tx_mute);
  }

  /* Release the Vocoder (maybe) if we are going to Mute.               */

  if (mute == SND_MUTE_MUTED) {
    sndhw_voc_release();
  }

} /* end of sndhw_set_apath_in */
/* <EJECT>*/
/*===========================================================================

REGIONAL FUNCTION sndhw_set_apath_out

DESCRIPTION
  Change output paths. Set the volume.

DEPENDENCIES
  The Sound Task must have been started in order to initialize the hardware.
  Cannot be called from interrupt context, cannot be called from any task
  other than the sound task.

RETURN VALUE
  None

SIDE EFFECTS
  snd_crnt_path_out
  snd_voc_acquired

===========================================================================*/
void sndhw_set_apath_out
(
    snd_pstate_type *apath_out
    /* State of the audio output path to set. If NULL, continue
    ** use current path state.
    */
)
{
  boolean         unmute_path;
  /* Shortcut for Current Path */
  boolean         path_set = FALSE;
  /* Flag indicating whether the path will be set */
  static boolean  tone_playing      = FALSE;
  static boolean  keep_voc_acquired = FALSE;
#ifdef FEATURE_MIDI_OUT
  static boolean  midi_out_enabled  = FALSE;
#endif /* FEATURE_MIDI_OUT */
#ifdef FEATURE_PCM_REC
  static boolean pcm_rec_reverse_enabled = FALSE;
  static boolean pcm_rec_forward_enabled = FALSE;
#endif /* FEATURE_PCM_REC */

#if (defined(SND_HFK) && !defined(SND_HFK_PLUS))
  static boolean  stereo_mute = FALSE;
  /* Local flag indicating whether stereo mute is on */
#endif

#ifdef SND_EXTCOM
#error code not present
#endif

  if (apath_out != NULL) {

#if defined(SND_EXTCOM)
#error code not present
#elif (defined(SND_HFK) && !defined(SND_HFK_PLUS))
    /* If the stereo mute changes, and results in a muting change,
    ** go through the path change.
    */
    if (stereo_mute != sndhw_stereo_mute_s) {
      stereo_mute = sndhw_stereo_mute_s;
      if (snd_crnt_path_out.mute == SND_MUTE_MUTED) {
        path_set = TRUE;
      }
    }
#endif
    /* If the new path or device or muting status or volume or
    ** volume control information is different than the current path
    ** go through path change.
    */
    if(( apath_out->device   != snd_crnt_path_out.device ) ||
       (( apath_out->method  != snd_crnt_path_out.method)
#ifdef FEATURE_MIDI_OUT
          /* The following check will only accept the method
          ** change if we are not in the special SND_METHOD_MIDI
          ** override condition
          */
         && ((snd_crnt_path_out.method != SND_METHOD_MIDI) ||
             (!sndhw_midi_out_enabled) ||
             (sndhw_tone_playing))
#endif /* FEATURE_MIDI_OUT */
       ) ||
       ( apath_out->mute     != snd_crnt_path_out.mute   ) ||
       ( apath_out->vol      != snd_crnt_path_out.vol    ) ||
       ( tone_playing        != sndhw_tone_playing       ) ||
#ifdef FEATURE_MIDI_OUT
       ( midi_out_enabled    != sndhw_midi_out_enabled   ) ||
#endif /* FEATURE_MIDI_OUT */
#ifdef FEATURE_PCM_REC
       ( pcm_rec_reverse_enabled != sndhw_pcm_rec_reverse_enabled ) ||
       ( pcm_rec_forward_enabled != sndhw_pcm_rec_forward_enabled ) ||
#endif /* FEATURE_PCM_REC */
       ( keep_voc_acquired   != sndhw_keep_voc_acquired  )) {

      /* Save current setting of output path (structure copy) */
      snd_crnt_path_out = *apath_out;
      tone_playing      = sndhw_tone_playing;
      keep_voc_acquired = sndhw_keep_voc_acquired;
#ifdef FEATURE_MIDI_OUT
      midi_out_enabled  = sndhw_midi_out_enabled;
      if(sndhw_midi_out_enabled && !sndhw_tone_playing) {
        snd_crnt_path_out.method = SND_METHOD_MIDI;
      }
#endif /* FEATURE_MIDI_OUT */
#ifdef FEATURE_PCM_REC
      pcm_rec_reverse_enabled = sndhw_pcm_rec_reverse_enabled;
      pcm_rec_forward_enabled = sndhw_pcm_rec_forward_enabled;
#endif /* FEATURE_PCM_REC */
      path_set          = TRUE;

      MSG_HIGH("Setting Device: Dev = %d, Mute = %d",
                apath_out->device, apath_out->mute, 0 );
    }
  } else {
    /* Go through the path change because this is a continuation of
    ** a previous, incomplete path setting.
    */
    path_set = TRUE;
  }

  if (path_set == TRUE) {

    /* Setup shortcut to path. Combine the physical path, muting, and
    ** stereo mute override.
    */
#if defined(SND_EXTCOM)
#error code not present
#elif (defined(SND_HFK) && !defined(SND_HFK_PLUS))
    unmute_path = ((snd_crnt_path_out.mute != SND_MUTE_MUTED) || (stereo_mute));
#else
    unmute_path = (snd_crnt_path_out.mute != SND_MUTE_MUTED);
#endif
#ifdef FEATURE_MIDI_OUT
    unmute_path = unmute_path || midi_out_enabled;
#endif /* FEATURE_MIDI_OUT */
#ifdef FEATURE_PCM_REC
    unmute_path = (unmute_path || pcm_rec_reverse_enabled ||
                   sndhw_pcm_rec_forward_enabled);
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_TTY
    /* Inform the vocoder driver that a TTY device is being used so that
    ** the proper firmware can be loaded.
    */
    if ((snd_crnt_path_out.device == SND_DEVICE_TTY_HFK ) ||
        (snd_crnt_path_out.device == SND_DEVICE_TTY_HEADSET ) ||
        (snd_crnt_path_out.device == SND_DEVICE_TTY_VCO ) ||
        (snd_crnt_path_out.device == SND_DEVICE_TTY_HCO ) ) {

      if(snd_crnt_path_out.device == SND_DEVICE_TTY_VCO) {
        (void) voc_set_tty_mode(VOC_TTY_MODE_VCO);
      } else if(snd_crnt_path_out.device == SND_DEVICE_TTY_HCO) {
        (void) voc_set_tty_mode(VOC_TTY_MODE_HCO);
      } else {
        (void) voc_set_tty_mode(VOC_TTY_MODE_ON);
      }
    } else {
      (void) voc_set_tty_mode(VOC_TTY_MODE_OFF);
    }
#endif /* FEATURE_TTY */

    /* Select the codec based on the physical path.
    ** Select the right codec before acquiring the vocoder to minimize
    ** the number of vocoder resets.
    */
    sndhw_which_codec(&snd_crnt_path_out);

    if(unmute_path) {
      sndhw_voc_acquire();      /* Acquire the Vocoder (if not already) */
      if (snd_voc_acquired) {
        sndhw_smartsnd_aud_process_ctl(); /* Leave VOX mode (if needed)          */
        (void)snd_data_get_generator(snd_crnt_path_out.device,
                                     snd_crnt_path_out.method);
        sndhw_set_volume();     /* Set the volume for this path         */
#ifdef SND_EXTCOM
#error code not present
#endif
      }
    } else {
      MSG_MED("Muting output path",0,0,0);
      sndhw_set_volume();       /* Set the volume for this path (mute)  */
      sndhw_voc_release();      /* Release the Vocoder (maybe)          */
    }
  }
} /* end of sndhw_set_apath_out */
/* <EJECT>*/
/*===========================================================================

REGIONAL FUNCTION sndhw_set_tone_apath_out

DESCRIPTION
  Change output paths depending on the generator.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_tone_playing
  sndhw_keep_voc_acquired
  snd_override_timer
  sndhw_unmute_flag
  snd_unmute_timer

===========================================================================*/
void sndhw_set_tone_path_out (
  snd_pstate_type *tone_path_out,
  snd_pstate_type *voice_path_out
) {
  snd_gen_type generator;

#ifdef SND_EXTCOM
#error code not present
#endif

  sndhw_tone_playing = TRUE;
  sndhw_voice_path_out_mute = voice_path_out->mute;

  generator = snd_data_get_generator(tone_path_out->device,
                                     tone_path_out->method);
  sndhw_change_gen(generator);

#ifdef FEATURE_PMIC_RINGER
  if(((generator == SND_GEN_RING)
#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
     ) && (!sndhw_pmic_ring_enabled)) {
    pm_set_ringer (PM_ON_CMD);
    sndhw_pmic_ring_enabled = TRUE;
  } else if(((generator != SND_GEN_RING)
#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */
            ) && (sndhw_pmic_ring_enabled)) {
    pm_set_ringer (PM_OFF_CMD);
    sndhw_pmic_ring_enabled = FALSE;
  }
#endif /* FEATURE_PMIC_RINGER */

  switch (generator) {
    case SND_GEN_VOC:
#ifdef FEATURE_AUDIO_FORMAT
    case SND_GEN_MIDI:
#endif
      /* If generating a vocoder tone, set override flag to keep
      ** vocoder acquired for 3 seconds after tones are done playing
      */
      sndhw_keep_voc_acquired = TRUE;
      (void) rex_clr_timer(&snd_override_timer);
      sndhw_set_apath_out(tone_path_out);
      break;

    case SND_GEN_AUX:
      if (sndhw_keep_voc_acquired) {
        /* If override timer is not set yet, set it for 3 seconds */
        if (rex_get_timer(&snd_override_timer) == 0) {
          (void) rex_set_timer( &snd_override_timer, SND_OVERRIDE_TIME );
        }
      }
      sndhw_set_apath_out(voice_path_out);
      break;

    case SND_GEN_RING:
#ifdef FEATURE_CLICK
#error code not present
#endif /* FEATURE_CLICK */

      /* Set unmute flag here to not unmute mic until a delay after
      ** tone is finished playing in order to not pick up sound from ringer
      */
      sndhw_unmute_flag = FALSE;
      (void) rex_clr_timer(&snd_unmute_timer);
      sndhw_set_apath_out(voice_path_out);

      if (sndhw_keep_voc_acquired) {
        /* If override timer is not set yet, set it for 3 seconds */
        if (rex_get_timer(&snd_override_timer) == 0) {
          (void) rex_set_timer( &snd_override_timer, SND_OVERRIDE_TIME );
        }
      }
      break;

    case SND_GEN_NONE:
      break;
  }
}
/* <EJECT>*/
/*===========================================================================

REGIONAL FUNCTION sndhw_overrride_done

DESCRIPTION
  This function signals the end of the override timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_keep_voc_acquired
  snd_override_timer

===========================================================================*/
void sndhw_override_done() {
  /* reset flag that keeps vocoder acquired */
  sndhw_keep_voc_acquired = FALSE;
  (void) rex_clr_timer( &snd_override_timer );
}
/* <EJECT>*/
/*===========================================================================

REGIONAL FUNCTION sndhw_unmute_done

DESCRIPTION
  This function signals the end of the unmute timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_unmute_flag
  snd_unmute_timer

===========================================================================*/
void sndhw_mute_done() {
  /* reset flag that allows the mic to be unmuted */
  sndhw_unmute_flag = TRUE;
  (void) rex_clr_timer( &snd_unmute_timer );
}
/* <EJECT>*/
/*===========================================================================

REGIONAL FUNCTION sndhw_restore_voice_path

DESCRIPTION
  This function restores the voice path after tones are finished playing.

DEPENDENCIES
  sndhw_keep_voc_acquired
  snd_override_timer
  sndhw_unmute_flag
  snd_unmute_timer

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_tone_playing
  sndhw_unmute_flag

===========================================================================*/
void sndhw_restore_voice_path (
  snd_pstate_type      *path_out,
  snd_mute_control_type path_in_mute
) {

#ifdef SND_EXTCOM
#error code not present
#endif

  sndhw_tone_playing = FALSE;
  sndhw_change_gen(SND_GEN_NONE);

#ifdef FEATURE_PMIC_RINGER
  if(sndhw_pmic_ring_enabled) {
    pm_set_ringer (PM_OFF_CMD);
    sndhw_pmic_ring_enabled = FALSE;
  }
#endif /* FEATURE_PMIC_RINGER */

  if (sndhw_keep_voc_acquired) {
    /* If override timer is not set yet, set it for 3 seconds */
    if (rex_get_timer(&snd_override_timer) == 0) {
      (void) rex_set_timer( &snd_override_timer, SND_OVERRIDE_TIME );
    }
  }

  /* Restore audio path and volume for voice.
  ** If muting input, do that first. Otherwise unmute output first.
  ** Setting the output path will take care of setting the volume.
  **
  ** snd_unmute_flag is used to control when to unmute.
  ** a sufficient delay as defined by SND_UNMUTE_TIME is used so as to
  ** prevent ringer sounds from being sent through the mic.
  */
  if((sndhw_unmute_flag)||(path_in_mute == SND_MUTE_MUTED)||
     (path_out->mute == SND_MUTE_MUTED)) {
    sndhw_unmute_flag = TRUE;
    if (path_in_mute == SND_MUTE_MUTED) {
      sndhw_set_apath_in( path_in_mute );
      sndhw_set_apath_out( path_out );
    } else {
      sndhw_set_apath_out( path_out );
      if (snd_path_is_ready == SND_PATH_READY) {
        /* Restore input muting to voice setting  */
        sndhw_set_apath_in( path_in_mute );
      }
    }
  } else {
    /* Set unmute timer here if it hasn't been set yet */
    if (rex_get_timer(&snd_unmute_timer) == 0) {
      (void) rex_set_timer( &snd_unmute_timer, SND_UNMUTE_TIME );
    }
  }
}

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION sndhw_mm_rec_voc_change

DESCRIPTION
  Tell the Sound Task that the vocoder has changed state.

DEPENDENCIES
  This routine may be called from an interrupt, don't use DM messages.

RETURN VALUE
  None

SIDE EFFECTS
  snd_path_is_ready
  snd_voc_is_ok
  snd_analog_mode

===========================================================================*/
LOCAL void sndhw_mm_rec_voc_change
(
  voc_inform_type status
    /* New Status of Vocoder */
)
{
  /* This part takes care of MultiMedia recording */
  if ((sndhw_mm_rec_status == SNDHW_MM_REC_STATUS_PENDING)
#ifdef FEATURE_GRAPH_AUDIO
       &&(status == VOC_INFORM_GRAPH_AUDIO)
#endif
     ) {

      if (voc_mm_rec_check_config() != TRUE) {
       /* Nothing changed, enable recording */
       sndhw_mm_rec_status          = SNDHW_MM_REC_STATUS_ENABLED;
       snd_mm_rec_status(SND_CMX_MM_REC_ENABLED, NULL, NULL, NULL);
     }

  } else if (sndhw_mm_rec_status == SNDHW_MM_REC_STATUS_ENABLED) {
#ifdef FEATURE_GRAPH_AUDIO
    if (status == VOC_INFORM_GRAPH_AUDIO) {
      snd_mm_rec_status (SND_CMX_MM_REC_INT_RESUME, NULL, NULL, NULL);
    } else
#endif
    {
      snd_mm_rec_status (SND_CMX_MM_REC_INTERRUPTED, NULL, NULL, NULL);
    }
  }
}

/*===========================================================================

FUNCTION sndhw_mm_voc_acquire

DESCRIPTION
  This function acquire and config vocoder for multimedia recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void sndhw_mm_voc_acquire (
  voc_capability_type    capability
)
{
  /* Acquire the vocoder at the start of recording
  */
  voc_acquire(VOC_ACQ_MMREC, sndhw_mm_rec_voc_change);

  /* Now configure the vocoder
  */
  if (voc_config(VOC_ACQ_MMREC, capability) != VOC_DONE_S) {

    MSG_HIGH ("Start MultiMedia Recording, Voc Config failed", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION sndhw_mm_record

DESCRIPTION
  This function sends a command to the vocoder driver to start
  recording MultiMedia data.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_mm_record (
  snd_mm_rec_aud_type       *rec_param,   /* Record parameters              */
  voc_mm_rec_cb_fn_type     snd_isr_cb_func
)
{
#ifdef FEATURE_AAC_REC
  snd_mm_rec_aac_para_type aac_param;
#endif /* FEATURE_AAC_REC */

#ifdef FEATURE_AAC_REC
  if (rec_param->format == SND_MM_FORMAT_AAC) {

    aac_param              = *(&(rec_param->aac_rec_param));
  }

  voc_set_mm_param((voc_sample_rate_type)(aac_param.sample_rate),
                   (voc_mm_rec_chan_type)(aac_param.chan -
                                          SND_AAC_CHANNEL_MONO),
                   aac_param.bit_per_spl,
                   snd_isr_cb_func);

  sndhw_mm_voc_acquire(VOC_CAP_AAC_REC);

  sndhw_mm_rec_status = SNDHW_MM_REC_STATUS_PENDING;
#endif /* FEATURE_AAC_REC */
}

/*===========================================================================

FUNCTION sndhw_mm_record_stop

DESCRIPTION
  This function sends a command to the vocoder driver to stop
  recording MultiMedia data.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_mm_record_stop (void)
{
  sndhw_mm_rec_status = SNDHW_MM_REC_STATUS_DISABLED;

  voc_release(VOC_ACQ_MMREC);

  sndhw_set_apath_out(&snd_crnt_path_out);
}

/*===========================================================================

FUNCTION sndhw_mm_record_data

DESCRIPTION
  Get the recorded data from QDSP to snd layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_mm_record_data(
  snd_mm_rec_format_type      mm_type,
  audmain_mm_record_buf_type  *buf,
  audmain_mm_record_data_type *frame_info
)
{
  voc_mm_record_format_type voc_mm_format;
  boolean                   status;

  voc_mm_format = VOC_MM_FORMAT_MAX;

#ifdef FEATURE_AAC_REC
  if (mm_type == SND_MM_FORMAT_AAC) {
    voc_mm_format = VOC_MM_FORMAT_AAC;
  }
#endif /* FEATURE_AAC_REC */

  if (voc_mm_format != VOC_MM_FORMAT_MAX) {
    status = voc_mm_rec_get_data(voc_mm_format,
                                 (voc_mm_record_buf_type  *)buf,
                                 (voc_mm_record_data_type *)frame_info);
  } else {
    status = FALSE;
  }

  return (status);
}
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_AVS_I2SSBC
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_i2ssbc_change

DESCRIPTION
  Tell the Sound Task that the vocoder has changed state.

DEPENDENCIES
  This routine may be called from an interrupt, don't use DM messages.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
LOCAL void sndhw_i2ssbc_change
(
  voc_inform_type status
    /* New Status of Vocoder */
)
{
  if((sndhw_i2ssbc_status == SND_I2SSBC_STATUS_PENDING) &&
     (voc_image_supports_i2ssbc())
#ifdef FEATURE_GRAPH_AUDIO
      &&(status == VOC_INFORM_GRAPH_AUDIO)
#endif
    ){

      sndhw_i2ssbc_status = SND_I2SSBC_STATUS_ENABLED;

      if (sndhw_i2ssbc_cb_func_ptr != NULL) {
       sndhw_i2ssbc_cb_func_ptr(SND_I2SSBC_STATUS_ENABLED);
    }

  } else if ((sndhw_i2ssbc_status != SND_I2SSBC_STATUS_DISABLED)
#ifdef FEATURE_GRAPH_AUDIO
             && (status != VOC_INFORM_GRAPH_AUDIO)
#endif
            ){

    /* Vocoder changed state, but we did not release the vocoder and
    so we will move to pending state and inform the client so */
    sndhw_i2ssbc_status = SND_I2SSBC_STATUS_PENDING;

    if (sndhw_i2ssbc_cb_func_ptr != NULL) {
      sndhw_i2ssbc_cb_func_ptr(SND_I2SSBC_STATUS_PENDING);
    }
  }
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_i2ssbc_enable

DESCRIPTION
  This function acquires the vocoder to start I2S SBC encoding.

DEPENDENCIES
  The current codec must be able to support BT A2DP
  i.e. VOC_CODEC_BT_A2DP or VOC_BT_A2DP_SCO

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type sndhw_i2ssbc_enable(
  snd_sample_rate_type               sample_rate,
  snd_i2ssbc_cb_func_ptr_type        cb_func
  )
{
  snd_status_type    status = SND_FAILURE;
  // Yangdecai for BT
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  if (sndhw_codec_supports_a2dp) 
#endif  	
  {

    sndhw_i2ssbc_sample_rate = sample_rate;

    sndhw_i2ssbc_cb_func_ptr = cb_func;

    switch (sndhw_i2ssbc_status) {
      case SND_I2SSBC_STATUS_DISABLED:
        /* Lets go ahead and acquire the vocoder */
        voc_acquire(VOC_ACQ_I2SSBC, sndhw_i2ssbc_change);
        sndhw_i2ssbc_status  = SND_I2SSBC_STATUS_PENDING;

      case SND_I2SSBC_STATUS_PENDING:

      case SND_I2SSBC_STATUS_ENABLED:
        sndhw_set_sample_rate(sample_rate);
        status               = SND_SUCCESS;
        break;
    }
  }
  return status;
}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_i2ssbc_disable

DESCRIPTION
  This function releases the vocoder

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_i2ssbc_disable(void)
{
  switch (sndhw_i2ssbc_status) {
    case SND_I2SSBC_STATUS_PENDING:
    case SND_I2SSBC_STATUS_ENABLED:
      voc_release(VOC_ACQ_I2SSBC);
      sndhw_i2ssbc_status  = SND_I2SSBC_STATUS_DISABLED;
      /* Lets tell the client that I2S SBC has been disabled */
      if (sndhw_i2ssbc_cb_func_ptr != NULL) {
        sndhw_i2ssbc_cb_func_ptr(SND_I2SSBC_STATUS_DISABLED);
      }
      break;
    default:
      break;
  }
}
#endif /* FEATURE_AVS_I2SSBC */

/* <EJECT>*/
#ifdef FEATURE_VOICE_PLAYBACK
/*************************** Voice Prompt Functions *************************/
/*===========================================================================

FUNCTION sndhw_voc_vp_enq

DESCRIPTION
  Enqueue as many voice prompts onto the vocoder VP queue.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Modifies snd_prompt_q and snd_prompt_ptr_q.

===========================================================================*/
void sndhw_voc_vp_enq()
{
  voc_add_pbq_return_type voc_pbq_stat;
  voc_pb_header_type      *vp_ptr;
  const void              *client_data;
  int                     vp_index;

  while(!sndhw_pq_empty()) { /* while there are voice prompts to enqueue */
    (void) sndhw_pq_peek(&vp_index);         /* get the top voice prompt */

    /* get its associated voice prompt pointer */
    (void) sndhw_ppq_get_ptr(vp_index, &vp_ptr, &client_data);

    INTLOCK();
    /* Add the voice prompt to the vocoder queue */
    voc_pbq_stat = voc_pb_add_q(vp_ptr, client_data);
    INTFREE();

    if((voc_pbq_stat == VOC_PB_FULL) || (voc_pbq_stat == VOC_PB_NO_CDMA)) {
      /* If vocoder queue if full, stop enqueuing voice prompts */
      /* If vocoder is in ananlog mode, stop enqueuing          */
      break;
    } else if(voc_pbq_stat == VOC_PB_DIFF_SO) {
      (void) sndhw_ppq_pop_single();
      snd_vp_callback( client_data, SND_VP_DIFF_SO );
    } else if(voc_pbq_stat != VOC_PB_OK) {
      /* If voice prompt is different mode than others on vocoder queue
      ** it cannot be played, so free the voice prompt. Also if this voice
      ** prompt has a bad header pointer, it cannot be played, free it
      */
      if( sndhw_ppq_pop_single() == SND_PPQ_PQ_SUCCESS ) {
        snd_vp_callback( client_data, SND_BAD_VP );
      }
      MSG_ERROR("VOC_PBQ ERROR: %x", voc_pbq_stat, 0, 0);
    } else {
      /* Able to enqueue voice prompt, free VP pointer from local queue */
      (void) sndhw_ppq_pop_single();
      MSG_HIGH("Voc_tone queueing: %x", vp_index, 0, 0);
    }
    /* Free VP from queue */
    (void) sndhw_pq_dequeue();
  }
}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_voc_vp

DESCRIPTION
  Send a command to the vocoder to have it start playing a voice prompt.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void sndhw_voc_vp
(
  tone_status_type *tone_status_ptr
)
{
  static boolean vp_playing = FALSE;

  if(tone_status_ptr != NULL) {
    sndhw_voc_vp_enq();  /* enqueue voice prompts onto vocoder queue */
    if(sndhw_voice_path_out_mute == SND_MUTE_MUTED) {
      /* If voice path out is muted, allow vocoder driver to decide whether
      ** the earpiece should be muted.  It will mute the earpiece if there is
      ** no voice prompts to play locally
      */
      voc_rx_vp_mute_ctl();
    } else {
      /* If voice path is unmuted, unmute */
      sndhw_rx_mute_ctl(FALSE);
    }
    vp_playing       = TRUE;
    sndhw_vp_playing = TRUE;
  } else if (vp_playing) {
    sndhw_rx_mute_ctl(TRUE);
    vp_playing = FALSE;
  }
}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_cleanup_vp

DESCRIPTION
  Calls vocoder driver voice prompt clean up function and mutes rx path.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_cleanup_vp(
) {
  voc_pb_end();
}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_abort_vp

DESCRIPTION
  Sends vocoder voice prompt driver signal to end voice playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_abort_vp(
) {
  sndhw_vp_playing = FALSE;
  voc_pb_abort();
}
/* <EJECT>*/
/**********   P R O M P T   P T R   Q U E U E   F U N C T I O N S   ********/
/*===========================================================================

FUNCTION sndhw_ppq_free_all

DESCRIPTION
  This function frees all entries in the internal prompt ptr queue and resets
  the current prompt ptr queue to the internal one.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_prompt_ptr_q
  sndhw_prompt_ptr_q_list

===========================================================================*/
void sndhw_ppq_free_all()
{
  sndhw_prompt_ptr_q.number = 0; /* reset number of VP pointers to zero */
  sndhw_prompt_ptr_q_list   = sndhw_prompt_ptr_q.prompts;
}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_ppq_pop_single

DESCRIPTION
  This function will free a single entry in the internal prompt ptr queue.

DEPENDENCIES
  sndhw_prompt_ptr_q

RETURN VALUE
  SND_PPQ_PQ_FAIL if queue is empty or not internal prompt queue being used.
  SND_PPQ_PQ_SUCCESS if enqueue is not empty.

SIDE EFFECTS
  sndhw_prompt_ptr_q

===========================================================================*/
snd_ppq_pq_return_type sndhw_ppq_pop_single()
{
  snd_ppq_pq_return_type ret_val = SND_PPQ_PQ_SUCCESS;

  if((sndhw_prompt_ptr_q_list == sndhw_prompt_ptr_q.prompts)&&
     (sndhw_prompt_ptr_q.number > 0)) {
    sndhw_prompt_ptr_q.number--;
  } else {
    ret_val = SND_PPQ_PQ_FAIL;
  }

  return( ret_val );
}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_ppq_set_list

DESCRIPTION
  This function sets the current list of voice prompts to use.

DEPENDENCIES
  sndhw_prompt_ptr_q

RETURN VALUE
  SND_PPQ_PQ_SUCCESS

SIDE EFFECTS
  sndhw_prompt_ptr_q_list

===========================================================================*/
snd_ppq_pq_return_type sndhw_ppq_set_list (
  voc_pb_header_type **prompt_list
) {
  if(prompt_list != NULL) {
    sndhw_prompt_ptr_q_list = prompt_list;
  } else {
    /* If NULL then set to internal list */
    sndhw_prompt_ptr_q_list = sndhw_prompt_ptr_q.prompts;
  }
  return(SND_PPQ_PQ_SUCCESS);
}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_ppq_enqueue

DESCRIPTION
  This function enqueues a single pointer onto the internal prompt ptr queue.

DEPENDENCIES
  sndhw_prompt_ptr_q

RETURN VALUE
  SND_PPQ_PQ_FAIL if queue is full.
  SND_PPQ_PQ_SUCCESS if able to queue entry.

SIDE EFFECTS
  sndhw_prompt_ptr_q

===========================================================================*/
snd_ppq_pq_return_type sndhw_ppq_enqueue (
  voc_pb_header_type *vp,     /* Pointer to voice prompt to enqueue        */
  const void         *client_data,
  word               *idx     /* Returns index into queue that VP was NQed */
) {
  static word index = 0;
  snd_ppq_pq_return_type ret_val = SND_PPQ_PQ_SUCCESS;

  if(sndhw_prompt_ptr_q.number < SND_MAX_VP) {
    sndhw_prompt_ptr_q.prompts[index]     = vp;
    sndhw_prompt_ptr_q.client_data[index] = client_data;
    *idx = index++;
    if(index == SND_MAX_VP) {
      /* reset index */
      index = 0;
    }
    sndhw_prompt_ptr_q.number++;
  } else {
    ret_val = SND_PPQ_PQ_FAIL;
  }
  return( ret_val );

}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_ppq_get_ptr

DESCRIPTION
  This function returns the specified entry from the current prompt ptr queue.

DEPENDENCIES
  sndhw_prompt_ptr_q

RETURN VALUE
  Entry in prompt ptr queue that was specified with associated client data.

SIDE EFFECTS
  None

===========================================================================*/
snd_ppq_pq_return_type sndhw_ppq_get_ptr (
  int                index,          /* Index into queue to get VP pointer */
  voc_pb_header_type **vp,
  const void         **client_data
) {
  *vp = sndhw_prompt_ptr_q_list[index];
  if(sndhw_prompt_ptr_q_list == sndhw_prompt_ptr_q.prompts) {
    *client_data = sndhw_prompt_ptr_q.client_data[index];
  } else {
    *client_data = NULL;
  }
  return(SND_PPQ_PQ_SUCCESS);
}
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_ppq_internal_q

DESCRIPTION
  This function checks if the internal prompt ptr queue is current.

DEPENDENCIES
  sndhw_prompt_ptr_q_list

RETURN VALUE
  TRUE if the internal prompt ptr queue is current
  FALSE if the internal prompt ptr queue is not current

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_ppq_internal_q()
{
  return(sndhw_prompt_ptr_q_list == sndhw_prompt_ptr_q.prompts);
}
/* <EJECT>*/
/**************   P R O M P T   Q U E U E   F U N C T I O N S   ************/
/*===========================================================================

FUNCTION sndhw_pq_free_all

DESCRIPTION
  This function frees all the entries in the intermediate prompt queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_prompt_q

===========================================================================*/
void sndhw_pq_free_all()
{
  sndhw_prompt_q.num_pending = 0;
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_pq_enqueue

DESCRIPTION
  This function enqueues an entry onto the intermediate prompt queue.

DEPENDENCIES
  sndhw_prompt_q

RETURN VALUE
  SND_PPQ_PQ_FAIL if queue is full.
  SND_PPQ_PQ_SUCCESS if enqueued successfully.

SIDE EFFECTS
  sndhw_prompt_q

===========================================================================*/
snd_ppq_pq_return_type sndhw_pq_enqueue(
  word index              /* Index to enqueue onto queue */
) {
  int temp_index;
  snd_ppq_pq_return_type ret_val = SND_PPQ_PQ_SUCCESS;

  if(sndhw_prompt_q.num_pending < SND_MAX_VP) {
    MSG_HIGH( "VP:Queuing, %x",index,0,0);
    temp_index = sndhw_prompt_q.index + sndhw_prompt_q.num_pending;
    if(temp_index >= SND_MAX_VP) {
      temp_index -= SND_MAX_VP;
    }
    sndhw_prompt_q.prompts[temp_index] = index;
    sndhw_prompt_q.num_pending++;
  } else {
    /* Should NEVER reach this point */
    ret_val = SND_PPQ_PQ_FAIL;
  }
  return( ret_val );
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_pq_empty

DESCRIPTION
  This function checks if the intermediate prompt queue is empty.

DEPENDENCIES
  sndhw_prompt_q

RETURN VALUE
  TRUE if the intermediate prompt queue is empty.
  FALSE if the intermediate prompt queue is not empty.

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_pq_empty()
{
  return(sndhw_prompt_q.num_pending == 0);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_pq_peek

DESCRIPTION
  This function returns the top entry in the intermediate prompt queue.

DEPENDENCIES
  sndhw_prompt_q

RETURN VALUE
  SND_PPQ_PQ_FAIL if queue is empty.
  SND_PPQ_PQ_SUCCESS if queue is not empty.
  Top entry in intermediate prompt queue.

SIDE EFFECTS
  None

===========================================================================*/
snd_ppq_pq_return_type sndhw_pq_peek(
  int *index  /* Returns top entry of queue */
) {
  snd_ppq_pq_return_type ret_val = SND_PPQ_PQ_SUCCESS;

  if (sndhw_prompt_q.num_pending > 0) {
    *index = sndhw_prompt_q.prompts[sndhw_prompt_q.index];
  } else {
    ret_val = SND_PPQ_PQ_FAIL;
  }
  return( ret_val );
}

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_pq_dequeue

DESCRIPTION
  This function dequeues the top entry in the intermediate prompt queue.

DEPENDENCIES
  sndhw_prompt_q

RETURN VALUE
  SND_PPQ_PQ_FAIL if queue is empty.
  SND_PPQ_PQ_SUCCESS if queue is not empty.

SIDE EFFECTS
  sndhw_prompt_q

===========================================================================*/
snd_ppq_pq_return_type sndhw_pq_dequeue()
{
  snd_ppq_pq_return_type ret_val = SND_PPQ_PQ_SUCCESS;

  if (sndhw_prompt_q.num_pending > 0) {
    if(++sndhw_prompt_q.index == SND_MAX_VP) {
      sndhw_prompt_q.index = 0;
    }
    sndhw_prompt_q.num_pending--;
  } else {
    ret_val = SND_PPQ_PQ_FAIL;
  }

  return( ret_val );
}
#endif /* FEATURE_VOICE_PLAYBACK */

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_enable_eclock

DESCRIPTION
  This function will enable (external)auxpcm clock.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
void sndhw_enable_eclock(void)
{
  voc_ecodec_clock_ctl(TRUE);
}
/*===========================================================================

FUNCTION sndhw_disable_eclock

DESCRIPTION
  This function will disable (external)auxpcm clock.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
void sndhw_disable_eclock(void)
{
#ifdef FEATURE_QTUNES_COMMON
  if (voc_get_status() != VOC_INFORM_QTUNES)
#endif
  {
    voc_ecodec_clock_ctl(FALSE);
  }
}



#ifdef FEATURE_AUDIO_PCR_SUPPORT
/*===========================================================================

FUNCTION sndhw_audfmt_adjust_sample

DESCRIPTION
  Add/Drop one audio sample.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_adjust_sample(boolean add_sample)
{
  boolean ret_val = FALSE;
  if (voc_audfmt_adjust_sample(add_sample) == VOC_DONE_S) {
      ret_val = TRUE;
  }
  return( ret_val );
}
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION sndhw_audfmt_dual_mono

DESCRIPTION
  Set dual mono channel configuration.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_dual_mono (
  snd_af_dual_mono_mode_enum_type  mode
) {
  boolean ret_val = FALSE;

  if(voc_dual_mono_mode((uint16)mode) == VOC_DONE_S) {
    ret_val = TRUE;
  }

  return(ret_val);
}

/*===========================================================================

FUNCTION sndhw_audfmt_sbr_mode

DESCRIPTION
  Set SBR mode

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_sbr_mode (
  snd_af_sbr_mode_enum_type   sbr_mode
) {
  boolean ret_val = FALSE;

  if(voc_sbr_mode((uint16)sbr_mode) == VOC_DONE_S) {
    ret_val = TRUE;
  }

  return(ret_val);
}

#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

#ifdef FEATURE_OEMSBL_AUDIBLE_TONE
/*===========================================================================

FUNCTION sndhw_set_oemsbl_cb

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
void sndhw_set_oemsbl_cb(snd_oemsbl_cb_func_ptr_type func_ptr)
{
  voc_set_oemsbl_cb((voc_oemsbl_cb_func_ptr_type)func_ptr);
}
#endif

/* FM over BT AG */
/*===========================================================================

FUNCTION sndhw_set_fm_over_bt

DESCRIPTION
  This function do the processing related to FM over BT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_set_fm_over_bt (
  snd_aux_line_in_mode_type aux_mode
)
{
  /* FM over BT related processing */
  voc_process_fm_over_bt(aux_mode);
}

/*===========================================================================

FUNCTION sndhw_set_aux_line_in

DESCRIPTION
  Set the aux line in on/off

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_set_aux_line_in (
  snd_aux_line_in_mode_type aux_mode
)
{
#ifdef FEATURE_BT_AG
  extern snd_pstate_type snd_voice_path_out;
#endif

  /* process the aux line in configuration */
  voc_process_aux_line_in(aux_mode);

#ifdef FEATURE_BT_AG
  MSG_MED("Current Device for FM Playback is set to %d \n",
                                    snd_voice_path_out.device, 0, 0);
#endif

#ifdef FEATURE_BT_AG
  if (snd_voice_path_out.device == SND_DEVICE_BT_HEADSET)
  {
    /* FM over BT AG */
    MSG_MED("Selecting codec VOC_CODEC_FM_OVER_BT_AG \n", 0, 0, 0);
    voc_codec_select(VOC_CODEC_FM_OVER_BT_AG);
  }
#endif /* FEATURE_BT_AG */
}

/*===========================================================================

FUNCTION sndhw_check_aux_line_in

DESCRIPTION
  Return aux line in status.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Aux Line In path is active.
  FALSE - Aux Line In path is not active.

SIDE EFFECTS
  None.

===========================================================================*/
boolean sndhw_check_aux_line_in(
  void
)
{
  return voc_check_aux_line_in();

}

/*===========================================================================

FUNCTION sndhw_set_aux_line_in_vol

DESCRIPTION
  Set the aux line in volume

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_set_aux_line_in_vol (uint8 volume)
{
  /* process the aux line in volume */
  voc_process_aux_line_in_vol(volume);
}

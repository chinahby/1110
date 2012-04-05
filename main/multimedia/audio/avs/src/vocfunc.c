/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("QDSP2 Device Driver")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Q D S P 2   V O C O D E R   D R I V E R
                          A P I   F U N C T I O N S

GENERAL DESCRIPTION
  This module contains API functions for the QDSP2 vocoder.

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS
  voc_read_ones_detect
    This function reads ones detect
  voc_reset_ones_detect
    This function resets ones detect.
  voc_codec_gain
    Set the gain values for the Codec.
  voc_codec_select
    Select which CODEC the vocoder will use.
  voc_dtmf_cmd
    Generate DTMF tones.
  voc_vol_cmd
    Set the internal vocoder gain.
  voc_tx_cmd
    Get the current vocoder frame and sets the rate for the next frame.
  voc_rx_cmd
    Give the current vocoder frame to the vocoder.
  voc_frame_cmd
    Set the rate of the next frame for the vocoder.
  voc_peek_cmd
    Get the results of a peek from the vocoder.
  voc_poke_cmd
    Poke the bytes to the vocoder.
  voc_rx_mute_cmd
    Mute or un-mute the vocoder receive audio path.
  voc_tx_mute_cmd
    Mute or un-mute the vocoder transmit audio path.
  voc_dtmf_tx_mute_cmd
    Control mute/un-mute of the DTMF output to the FM transmit path.
  voc_compander_ctl
    Compander control for IS-55 tests.  Turn the compressor/expander off/on.
  voc_tx_rate_limit
    Limit the vocoder transmit rate to ( 2s+1 ) / ( 2s+2 ).
  voc_pkt_int_loop_cmd
    Set the Packet internal loopback state of the vocoder receive audio path.
  voc_dfm_loop_cmd
    Set the DFM loopback state of the vocoder receive audio path.
  voc_pkt_loop_cmd
    Set the Packet loopback state of the vocoder receive audio path.
  voc_audio_loop_cmd
    Sets the Audio loopback state of the vocoder receive audio path.
  voc_pcm_loop_cmd
    Set the PCM loopback state of the vocoder receive audio path.
  voc_version
    Report the Vocoder version.
  voc_get_pcmpad
    Return the value of the incoming (TX) pad byte
  voc_put_pcmpad
    Set the value of the outgoing (RX) pad byte
  voc_dfm_sat_ctrl
    Enables or disables SAT transpond.
  voc_dfm_sat_set_ecc
    Configures the SAT Expected Color Code (ECC) to be used.
  voc_dfm_sat_get_dcc
    Retrieves the SAT Detected Color Code (DCC).
  voc_dfm_sat_dcc_cb
    Callback function registered with the QDSP command interface.
  voc_rx_agc_cmd
    Set the Rx AGC/AVC mode.
  voc_tx_agc_cmd
    Set the Tx AGC mode.
  voc_aud_process_override
    Set audio process override mode.
  voc_disable_hfk_pins()
    Disable MSM shared pins for external CODEC. (UART2 is enabled.)
  voc_enable_hfk_pins()
    Enable MSM shared pins for external CODEC.
  voc_set_equalizer_gain()
    Set equalizer gain.
  voc_wma_set_song_config
    Set configuration params for Windows Media Audio song playback.
  voc_mm_parm
    Select a new set of paramters for multimedia recording
  voc_mm_rec_update_read_count
    Reset the read counter to DSP for MultiMedia recording.
  voc_mm_rec_get_data
    Get the encoded data frame from the qdsp multimedia encode buffers.
  voc_mm_rec_check_config
    Check wether multi media recording configurations have been changed.

  voc_adec_data_consumed
    This function detects decoder has finished consuming data.

  voc_3d_reverb_set
    This function issues a 3D Reverb Set command.

  voc_3d_pos_lsnr_vel
    This function issues a 3D positional listener velocity command.

  voc_3d_reverb_enable
    This function issues a 3D Reverb Enable/Disable command.

  voc_3d_reverb_src_vol
    This function issues a 3D reverb source volume command.

  voc_3d_pos_src_vel
    This function issues a 3D positional source velocity command.

  voc_3d_pos_query_src_revol
    This function queries the source reverb volume.

  voc_3d_pos_query_src_vel
    This function queries the source velocity.

  voc_3d_pos_query_lsnr_vel
    This function queries the listener velocity.

  voc_3d_pos_query_reverb_enabled
    This function queries reverb enabled flag.

  voc_3d_pos_query_reverb_setting
    This function queries reverb setting.

  voc_3d_pos_set_output_mode
    This function sets QAFX output mode.

  voc_3d_pos_reverb_reset
    This function clears reverb preset and set the reverb enable flag to
    FALSE.

  voc_3d_pos_src_enable
    This function issues sound source 3D enable/disable command

  voc_3d_pos_reset_env
    This function issues source 3D reset command

  voc_3d_pos_query_env_enable
    This function queries 3D environment enable flag

  voc_3d_pos_query_src_enable
    This function queries source 3D enable flag

  voc_adec_voc_start
     This function start QCP or AMR-NB decoder

  voc_ecodec_clock_ctl
     This function start or stop external AuxPCM clock

  voc_process_fm_over_bt
       This function enables/disables FM over BT

  voc_process_aux_line_in
     This function processes the AuxPGA Line In configuration
  voc_check_aux_line_in
     Return aux line in status.
  voc_process_aux_line_in_vol
     This function process the aux line in volume set request

INITIALIZATION AND SEQUENCING REQUIREMENTS
  voc_task() must be started up before calling any of the vocoder API functions.

Copyright(c) 1999 - 2008 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/multimedia/audio/avs/src/vocfunc.c#4 $ $DateTime: 2010/09/26 23:15:28 $ $Author: asifmd $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
09/27/10   aim     Modified code so that A2DP link is enabled only if 
                   FW supports A2DP over current playback format.  
04/15/10    dp     Fixed the issue of ADPCM playback not starting when GRAPH image
                   is loaded.
03/23/10   skr     Modified code to send ADPCM decoder start command when ADPCM 
                   playback other than 32Khz, 4bits/sample, 512 blocksize,
                   1channel is initiated when voc_state is in CAM_AUDIO_ACTIVE . 
10/23/09    sj     As clk_regime_enable_bt_pcm_clk() API is absolute for SCMM,
                   don't call this API for SCMM.
10/20/09   kkm     Setting up equalizer is modified to call after audfmt 
                   configuration is done.
19/10/09   kkm     Added codec config retry timer for A2DP. If voc_codec_select is called 
                   pa_on timer is getting reset and calling lately. So complete_bt_a2dp
                   function is called
10/08/09   knm     Integrated new marimba codec driver for SCMM
10/01/09    rm     Support for removing assertion in voc_fl_packet_processor.
10/07/09    rm     Added condition to handle rate factor for VoIP
10/06/09    sj     Added SCMM specific changes
09/16/09    gs     Added support for FM playback over Lineout, Handset
09/07/09   vsud    Changes for resampler image imeplementation.
08/28/09    rm     slowtalk: removed check for voc_check_in_call
                   while checking for slowtalk configurations
07/13/09   rm      Adding support for 4GV_WB_VoIP.
07/07/09   rk      Notifying SND that PAs are turned on, wakes up sound task.
07/07/09   rm      Added changes for QCamCorder Turbo image 
06/30/09   kk      Added more F3 messages.
06/03/09   kkm     Reverted back "modified to select Qsynth image for ADPCM Media Playback.
                   Support added for ADPCM to play on BT headset" because Camera shutter sound is not stopping
                   and preview is freezing
01/06/09  vm/sk    Modified the code to return voc_pa_on_status as true
                   for external codec.
05/21/09    kk     Added support for H264 QCIF playback support 
                   for low power mode(TURBO).
05/15/09    rm     QCamCorder FW's Vocoder State Paths has changed
                   to Voc Combo's state path. The SW chnages are done
                   to adapt to those changes.
05/13/09   kkm     0x301 command fails as the command register is not reset
                   by dsp. Increased the number of tries to check the command
                   register. Cr:177617
05/08/09    vm     Added Support for Device switch from external to internal.
05/05/09   knm     Added APIs to set & check headphone connection status on ULC
05/05/09    kk     Added support for H264 clip playback.
05/05/09   kkm     modified to select Qsynth image for ADPCM Media Playback.
                   Support added for ADPCM to play on BT headset
04/07/09   knm     Added voc_master_vol() API to directly set the master volume
04/06/09   skr     Do not send 0x320 comand from SND task for ADPCM playback
                   as shutter sound when in CAM_AUDIO_ACTIVE state .
                   Fixed CR-176952.
19/02/09   kd/knm  Fixed CR-167490,sending midi commands before setting
                   the rx and tx gains.
01/19/09   anb     Added voc_set_qtv_cb_func so that QTV can be informed about
                   mod/image swap event.Fixes Cr 163807(SR-1133583)
03/12/09   knm     Removed voc_disable_codec_func() from FM disable function.
16/02/09    sj     Added slowtalk feature support
03/05/09    vm     Fixed issue: FM audio playback over Bluetooth headset
                   is not happening as expected. Fixed CR 173666
21/02/09    sj     Fixed this issue: For Qcamcorder Image vocoder interface
                   changed from legacy to combo vocoder interface, so DMTF
                   was not working while Qcamcorder recording. Fixed CR 171955.
02/17/09    dp     Fixing CR 171476 - Phone Crash: "Undef Instr 0x 0"
                   seen during 1x-FM concurrency
02/09/09    bk     Properly featurized for removing compilation warnings.
01/19/09    dp     CR 168251 Removed feturization for FEATURE_BT_QSOC
01/07/08   laks    Fixed compilation issue with low tier ULC build
01/07/08    sj     Fixed compilation issues with low tier ULC builds.
12/26/08    rm     Added RVE API changes
12/31/08   laks    Fixed CR 163940: code in voc_adec_chan_vol() and
                   voc_adec_chan_pan()is modified to call check_interlock()twice
                   once with VOC_UNLOCK_QSYNTH once with VOC_UNLOCK_QTUNES
12/26/08   skr     Moved Macros (VOC_GRAPH_ADPCM_VALID_CHANNELS ..etc) to
                   vocdata.h to fix compilation issues due to CL 807920
12/23/08   vm      Added test code for AUXPCM testing support
12/01/08    sj     Fixed CR 165592 - no output from speaker
                   incae of --> fast switching b/w dual-mic to speaker
11/14/08   sj      Don't enable ECNS when we don't require it. Fixed CR 160542
11/24/08   skr     Added code for Audio + camera Concurrency
11/18/08   knm     Added code to support FM on HS-USB.
11/18/08   dp      Added support for playing NB-AMR on A2DP
11/18/08   dp      Featurization fixes for Prepaid build
11/12/08   dp      Fixed CR 154999 - BT IOT: A2DP: Uplink downlink audio is not
                   available for call plus music interaction
10/30/08   vm      Removed featurisation for sco rework
10/22/08   dp      FM over BT AG changes
10/17/08   vm      added idlemetring support for qtv image
10/14/08   anb     Delay sending bitstream/midi commands to the DSP till BT
                   AG connection is up when FEATURE_AVS_BT_SCO_REWORK enabled.
                   Changes complete fix for CR 140315.
10/14/08   skr     Fixed CR 156254 Reset the sample rate and configuration
09/30/08   knm     Fixed compilation errors by featurizing the code.
09/24/08   skr     changes done in voc_update_qensemble_config()
09/15/08   knm     Added support for Aux Line In(FM) volume control
09/11/08   dp      Correction to check bt a2dp functionality to send codec config in added scenario
                   CR 156726
08/29/08   av      Modified voc_codec_select for QSC60X5_AUDIO_HW specific code.
08/26/08   anb     Added support for DSP based WMAPRO
08/26/08  ss/sud   Function qdsp_cmd_adec_flush() is modified such that
                   qdsp_cmd_adpcm_adec_info is cleared for VOC ADPCM.
                   CR_154303 (SR_1099290)
08/12/08    sj     Fixed klokwork errors
08/07/08    av     Added support for CrystalSpeech using EXTERNAL_SADC
07/23/08    dp     Fixed CR: 149857
07/11/08   skr     Changes to release VOCODER & ADEC separately during Incall.
06/20/08   knm     Added voc_process_aux_line_in API to configure
                   AuxPGA Line In path.
05/14/08    sr     Modified qdsp_cmd_codec_cofig() to include default_frame_
                   rate.Fix for CR 134407 - 16khz PCM sampling. Updated Copyright.
04/15/08    sj     Added code for codec overriding in voc_codec_select()
                   This code will be helpful in order to override the
                   current codec with other codec. One use case is overriding
                   on_chip_0_codec with dual_mic_codec
03/30/08    hs     Modified voc_adec_move_data() to check CAP instead of module
                   for AAC bitsream failure
03/17/08  ymc/anb  Modified codec gain ctrl api to control gain changes.
                   Fix for SC2X codec pop issue through MSMAUD_PCM_DC_OFFSET feature.
03/11/08    kk     Added support for sending mp3 configuration
                   information for DSP.
02/08/08    az     Added post processing support for the in call audio.
02/08/08    vs     Fixed compile error when in-call midi not defined.
02/07/08    kk     Added support for Qensemble phase 2 interface changes.
02/05/08    kk     Added support to send DTMF cmds to mixerCommandReg
                   for Incall images.
01/30/08    az     Include the support for the In call Qtv.
01/15/08    hs     Added support ISDB-T dual mono output control
01/09/08    az     Included the in call midi and the new in call architecture.
01/08/08   wfd     CR 135189: In voc_check_bt_a2dp(), allow call to
                   voc_bt_a2dp_suspend() if voc_bt_state ==
                   VOC_BT_STATE_A2DP_LINK_PENDING.
12/18/07    sj	   Added function call to voc_state_write_codec_gain() in
		   function voc_codec_select() to write the codec gains
		   to ADSP when device is changed while playback is active.
		   (CR resolved 131975)
11/21/07   anb     Fixed compilation errors for SC2X prepaid build
11/14/07    hs     Added QEnsemble support
11/02/07    az     Included the support for the in call wma.
10/26/07    az     Included the support for the in call AAC.
10/15/07    az     Included support for the crystal speech for in call audio.
09/27/07   wfd     Fix CR 124200: In voc_check_bt_a2dp() do not suspend A2DP if
                   voc_bt_state is VOC_BT_STATE_A2DP_LINK_PENDING or _ENABLE_PENDING.
09/14/07   anb     Fixed CR 122104
08/28/07   ymc     Added Crystal Speech support.
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
08/08/07  anb/wfd  Added aux_pcm_clk to be enabled/disabled from other tasks like BT (CR 122106).
06/14/07   anb     In voc_check_bt_a2dp(), A2DP is suspended only if
                   voc_bt_mode is VOC_BT_MODE_A2DP. Fix for CR 117288.
06/13/07    sg     Added support for selection of the AAC module based on MIPs
                   requirements.
06/08/07    sg     Added support for selection of the MP3 module based on
                   the post-proc features that are currently enabled.
05/10/07    sp     Added support for an external BT SOC interfacing through
                   AUX PCM interface.
05/07/07    sp     Added code so that SBC encoder is reneabled if the DSP
                   state changes to from QTUNES -> QTUNES_INIT ->...-> QTUNES.
                   Fix for CR 117660.
05/02/07   ss      Fixed Error Checking in voc_3d_pos_enable function.
04/20/07  kan/anb  Added a compiler switch for VOC_UNLOCK_GRAPH
04/14/07   sud     Added support for independent control of Tx and Rx codec
                   clock for Saturn codec.
04/07/07    pl     Addeded support for starting qsynth AMR-NB
                   decoder.
03/15/07    kan    Added 4GV-WB Code
03/05/07    az     Fixed the SR: 905693 after multiple pause resume audio
                   disappear
02/26/07   kan     Added AMR-WB changes
02/26/07   sp/sk   Fixed CR 103945.PMD files with AAC not playing over BT.
02/09/07   act     Modified code to enforce no AGC/AVC during UMTS TTY.
01/26/07    az     Fixed the CR: 110341 Changed (Severity: HIGH) audio
                   track can not be played well of this clip
01/14/07    sg     In voc_midi_cmd(), removed unnecessary check for
                   VOC_STATE_QSYNTH_DISABLE_PENDING since check_interlock()
                   will perform the same check.
01/09/07   suk     Added QDSP_IMAGE_VOC_COMBO_VOIP support.
                   Updated Copyright text.
12/02/06   suk     Added support for VOIP Combo Vocoder, and Combo vocoder
                   with G711 and 4GV.
11/28/06    sp     Fixed SCO rework issues for QCP files.
11/20/06    sp     Added support for OEMSBL Audio.
11/16/06    sp     Made changes to update BT state properly after a disconnect.
11/14/06   act     Resolving compiler warnings.
10/31/06  sp/anb   Serialized the commands sent to BT, Fixes 97271
09/08/06    pl     Set 44K CLK to QDSP_CMD_I2SSBC_SAMPLE_RATE_44100 in
                   voc_sbc_enable().
09/05/06    ay     Added AMR-WB+ support.
09/01/06    hz     Fixed CR#100139 -- No volume control for ADPCM in camera
                   preview mode.
08/29/06    hz     Added support for incall MP3 on 65nm platforms.
08/04/06    st     Updated voc_adec_flush() to support QDSP_IMAGE_G711 image.
07/31/06    sp     Fixed some merge issues in AV Sync feature and
                   some issues in SCO rework feature
07/27/06   anb     Added the FEATURE_AVS_BT_SCO_REWORK which enables the AVS to send
                   messages to the BT driver
07/26/06   nm/ymc  Fixed issue of tx rate factor not initialized properly
                   in 1x voice call.
07/12/06    hz     Added support for incall mp3 GSM/UMTS, BTSCO and PostProc
                   features.
06/27/06    pl     Added environment & source 3D enable/disable support
                   and reset support.
06/26/06    pl     Added support for doppler and reverb, new
                   ping-pong buffer data writing scheme.
06/16/06    ay     Fixed implicit typecasting error on bt_app_active.
06/15/06   sk/sp   Fixed CR 95512 BT-A2DP worldsoccer issue
06/14/06    sp     Delay sending bitstream/midi commands to the DSP till BT
                   A2DP connection is up.
06/01/06    hz     Added fixes for CR#75189. Before disconnecting BT link,
                   use default codec to enable adie.
05/31/06    sg     Re-featureized synth MIDI functions so that they are not
                   under FEATURE_ADEC.
04/26/06   act     Added support for combo vocoder image.
04/03/06    ak     Added support for sidetone control.
04/02/06    sg     Unfeature-ized FEATURE_QDSP_USES_MODULES and removed support
                   for the old-style QDSP services.
03/24/06    pl     Seperated FIR filter out of FEATURE_QSYNTH_ADRC and
                   FEATURE_QTUNES_ADRC.
03/13/06   act     Added voc_rx_dtmf_override_cmd() to replace change made
                   in voc_rx_mute_cmd().
03/12/06    pl     Modified voc_update_qconcert_config not to write
                   qconcert setting to DSP if voc_data_get_qconcert_config()
                   returns VOC_CMD_FAILED_S.
03/13/06    st     Added support for delayed start of audio decoder channel;
                   added support for 3D positional final source settings
                   query command.
03/09/06    hz     Added support for evrc in-call MP3 support.
02/21/06   act     Added new parameter to voc_rx_mute_cmd() for
                   DTMF mixing support.
02/21/06    hz     Added FEATURE_WEBAUDIO in voc_set_av_sync.
02/18/06    hz     Added fixes for in-call mp3.
02/03/06    sm     Added 4GV VOIP support
01/31/06    lg     Added support for 6550-90nm images
01/31/06    hz     Added support for in-call mp3 feature.
01/30/06    sp     Added support for I2S SBC interface.
01/19/06    sp     Fixed a bug that could cause A2DP not to be disconnected
                   when MIDI playback ends.
01/16/06    sp     Fixed a bug that caused no audio on BT A2DP HS when
                   rapidly switching between two files.
01/16/06    sm     Added FEATURE_QDJ_TIMEWARP
01/14/06    pl     Modified to apply Qconcert configuration only when
                   stereo headset or speaker codec is in use.
01/13/06   act     Fixed compile error/warnings with low-tier featureset.
12/24/05    sp     Fixed a bug that caused A2DP link not to be setup properly
                   when the response from BT task for a START comand is
                   extremely fast.
12/15/05   ymc     Fixed the issue that 4GV rate limit never issued to DSP.
12/07/05    sp     Made changes so that codec config is issued only in idle
                   state when enabling BT.
11/28/05    hz     Added fixes for CR73855.
11/21/05    hz     CR78442: Added 32KHz ADPCM support for UMTS vocoder codecs.
11/16/05    sp     Made changes to support BT CCAP requirements. BT APP can
                   now initialize SCO connection at any time it is required.
                   Audio driver will issue codec config and setup the hardware
                   link only when appropriate image gets loaded onto ADSP.
                   Cleaned up A2DP support code.
10/19/05    pl     Added PCM AVSync support.
09/30/05    sp     Fixed a bug that resulted in MIDI file being played on
                   both A2DP as well as wired headset if a wired headset is
                   inserted while plabyback is on A2DP headset.
09/27/05    sp     Fixed a compilation error that occurs on UI builds.
09/18/05    sp     Added support for DSP based SBC encoder.
09/18/05   pl/st   Modified code to support new game audio ADEC API; added
                   support for QAudioFX: 3D positional commands and query
                   interface.
09/16/05    hz     Added in-call QTV image support.
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
09/12/05    hz     Added AV sync support for windows media.
09/08/05    sp     Fixed a bug in voc_codec_select() which prevented VR playback
                   on BT headset.
08/31/05    sm     VOX support
            sm     Modified voc_enable_codec_func() for new parameters
08/29/05    sp     Fixed bugs that resulted in streaming file not being resumed
                   on BT A2DP HS after fwd/rewind. Centralized A2DP disable in
                   vocfunc.c alone so that PP interface is disabled when
                   A2DP is suspended.
08/15/05    sg     Merged A/V sync changes from avs.00.10.44.01/vocfunc.c#5
07/28/05    sm     Fixes for G723.
07/22/05    lg     Added support for Stereo YADPCM.
            sm     Added Wav A/ULaw support.
07/19/05    pl     Modified voc_adec_move_data() to return VOC_DSP_DATA_ERR_S
                   status when number of ADEC errors goes over the limit.
07/13/05    sp     Added code to honor bitpool values preferred by BT A2DP
                   headset. Added code to issue a suspend command to BT A2DP
                   when the codec changes to something that does not support
                   A2DP.
07/06/05   act     Modified voc_enable_codec_func() and
                   voc_disable_codec_func() to pass codec as a parameter to
                   registered functions.
07/05/05    sm     Added G723.1 support.
07/05/05    aw     Modified qsynth pcm filter for backward compatiblity.
06/15/05    st     Modified code to check for all conditions of a pending DSP
                   reset before issuing MIDI commands.
06/03/05    sp     Changed sbc_data_type to audiosbcenc_data_type consequent
                   to such changes in BT driver.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/24/05    hz     Added support for MultiMedia/AAC record functionality.
                   Fixed a buf fo VOC+VFE+ADPCM in case external sadc is off.
05/18/05    ac     Modified code support AV sync functionality with vocoder
                   images and various audio decoders.
05/16/05    hz     Added support for Vocoder image+ADPCM+VFE.
05/04/05    aw     Replace PCM filter with IIR filter support for Qsynth.
04/28/05   ymc     Added 4GV NB support.
04/22/05    aw     Added support for 48Khz PCM playback.
04/21/05   act     Changed name of voc_codec_type to voc_codec_class_type.
                   Reverted name of voc_codec_id_type to voc_codec_type.
04/12/05    sg     Added mechanism to retry codec config for BT if the DSP
                   command register is busy. Fixed bug where the BT link was not
                   being disconnected when an image does not support BT.
04/01/05   act     Changed use of specific codec IDs to generic codec types
                   where applicable.
03/25/05   ac/sg   Added support for A/V synchronization.
03/02/05    aw     Added support for Qconcert; preset EQ bands; STF;
                   post-proc support for synth.
02/22/05    sm     Added G.711 vocoder support.
02/14/05    hz     Added voc_wma_set_song_config() for WMA support to set
                   the configuration parameters.
02/05/05    st     Modified voc_set_dls_config() to use voc_dls_config_type.
12/29/04    sp     Clean up post processing code. Moved most of the image checks
                   to qdspcmd.c
12/03/04    sm     Added FEATURE_VOC_DTMF_DETECT, removed FEATURE_USB_ISOC_AUDIO
11/19/04    sp     Added support for Arm Post Processing.
            aw     Added equalizer support on QSynth image.
            sg     Added support for QTV and QTV_AAC images.
10/27/04    aw     Added support for DFM SAT on QTunes image.
10/15/04    hz     Added support for QDSP_IMAGE_MP4D_AAC_PLUS.
10/14/04    sg     Modified to retry ADEC flush command for Qtunes images.
                   Modified to reinit the DSP if the AAC song config changes.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
08/11/04    st     Updated voc_midi_cmd() to handle multiple MIDI command
                   buffers.
07/02/04    st     Modified code to support more sampling rates for YADPCM.
06/23/04    aw     Added support for MP4D_AMR_EVRC_V13K image.
06/17/04    aw     Modified code to do critial section for equalizer.
06/08/04    aw     Added support for Spectrum Analyzer and Equalizer.
                   Modified voc_dtmf_cmd to use voc_data_get_dtmf_duration and
                   voc_data_get_sample_rate to get the actual value.
06/01/04    aw     Added voc_restore_comfort_noise function. Added support for
                   H264D_AAC image. Modified code to support AAC+.
05/25/04    aw     Update voc_midi_cmd to check possible state change (CR43857).
05/11/04    vp     Merged ADRC and PCM filter support MP3/AAC from
                   MSMSHARED_AVS.00.09.05.02.00. Added support for H263 images.
04/22/04    lg     Added support for Animated AAC Ringers.
04/16/04    st     Modified code to support more sampling rates for PCM data
                   in YADPCM mode.
03/22/04    st     Added support for PCM playback on QSynth image; modified
                   code to start state machine when QSynth volume mode is
                   updated; modified functions voc_pause_midi_seq() and
                   voc_set_vol_midi_seq() to return status information.
                   Updated voc_master_pan() to return status info; removed
                   voc_seq_vol() function; modified voc_adec_pause() to remove
                   sequence parameter; added voc_pause_midi_seq(),
                   voc_set_vol_midi_seq() functions; updated voc_sample_rate()
                   to not reset sampling rate for QSynth images.
03/20/04    aw     Modified code to use voc_state_config_external_sdac.
03/04/04    aw     Modified code to check interlock before sending codec config
                   in voc_check_bt_ag function; Modified code to return correct
                   status from voc_adec_adpcm_start.
01/30/04    sg     Modified to support only 32k ADPCM for VFE_BAYER. 8k ADPCM
                   and QCP is no longer support by this firmware.
01/26/04    lg     Modified code to support volume adjustment and audioPause as
                   an array for multi_sequencer
01/26/04    sg     Modified voc_sample_rate() to return completion status.
01/21/04    aw     Added support for qsynth volume mapping function.
12/03/03    aw     Added audio support for QGL image.
12/02/03    sg     Added support for linear PCM playback.
11/24/03    aw     Modified code to pass in output sampling rate to qsynth
                   ADRC and PCM filter functions.
11/22/03    sg     Added support for RealAudio codec.
11/07/03  aw/st    Modified code to support YADPCM and Qsynth DLS.
10/30/03    sm     Fixed compiler warnings.
10/10/03  sg/st    Modified to support additional AAC params reqd by the DSP.
                   Added support for MP4D_MP3 image. Removed unnecessary call
                   to shutdown the ADIE if the codec config fails.
            aw     Added MSMAUD_VOC_AMR_COMMON featurization.
08/21/03    st     Added support for VOC_CODEC_BT_INTERCOM codec type.
07/24/03    sg     Added support for STEREO_HEADSET.
05/13/03   ymc     Added GSM half rate support.  Added voc_d2e_loop_cmd.
           ymc     Removed WVS support.
05/06/03    sg     Modified so that the AAC specific song config. is written
                   only after the AAC image is downloaded.
04/18/03    aw     Added audio support for Q3D image.
04/17/03    sm     Removed unused parm in voc_amr_ul().
            sg     Added support for vocoder PCM interfaces.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface; added support for MP4D+AAC image.
03/10/03    st     Updated voc_adec_pause() for FEATURE_ADPCM; modified
                   voc_check_bt_ag() to account for VOC_ACQ_VS key; modified
                   voc_codec_select() to not reconfigure BT codec if VR is
                   active.
            sm     Added half-rate defines & utility functions.
02/28/03    st     Added voc_set_ec_override_mode() and
                   voc_set_ns_override_mode() functions.
02/19/03    st     Added support for ADPCM audio decoder; modified
                   voc_adec_adpcm_start() to validate format of ADPCM files;
                   added support for UNLOCK_GRAPH and UNLOCK_GRAPH_AUDIO.
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
12/03/02    st     Modified code to add support for graph audio states.
11/19/02    st     Added voc_synth_mvol() and voc_adec_mvol() functions.
11/18/02    st     Featurized GSM/WCDMA specific code.
11/06/02    sm     Added QCP AMR support.
10/29/02    st     Added support for SPEAKER_PHONE.
10/17/02   ymc     Added current image checking while setting DTX and initial
                   AMR modes.
09/18/02   ymc     Added FEATURE_MVS.
09/16/02    st     Modified check_interlock() for VOC_UNLOCK_MODEM to ensure
                   we are not in DFM_SAT_DCC state before returning TRUE, we
                   do not want to unmute until we are in DFM state (CR24489);
                   modified code to re-do codec config as audio process
                   override may have changed; added clkregim.h for BT; replaced
                   BT register setting with macro.
09/06/02    st     Modified code to only configure vocoder for BT headset
                   after receiving a BT audio connected event; modified
                   voc_check_bt_ag() to return status of BT AG.
08/23/02    sg     Added support for DSP_IMAGE_VOC_EVRC_DEFINED; modified to
                   use FEATURE_EXTERNAL_SDAC.
08/22/02    sg     Added support for TTY activity status indication.
                   (Merged from MSM_AVS.01.05.18.01.02).
08/13/02  aw/st    Modified code to add support for ADPCM audio decoder.
            st     Modified level of debug messages; fixed bug in voc_dtmf_cmd()
                   when QTUNES is defined and QTUNES_AAC is not.
          sg/st    Modified so that the DTMF path for the DSP is not set when
                   the vocoder is in QSYNTH or QTUNES mode, since it is not
                   supported by those images.
                   Added support for MP4 and AAC.
07/09/02    sm     Merged to perforce tip.
           ymc     Renamed AMR_SPEECH_PRO_DEGRADED as AMR_SPEECH_DEGRADED,
                   AMR_SPARE as AMR_ONSET.
           ymc     Featurized voc_set_clk_ctrl_timer under
                   MSMAUD_CODEC_CLOCK_CTRL.
06/11/02    st     Modified code to support wideband codec for MP3 image.
03/21/02    st     Modified code to support MIDI image; added support for
                   the MSMAUD_CODEC_CLOCK_CTRL feature which delays turning
                   off codec clocks until the proper time.
03/12/02    st     Modified code to use voc_state_adie_wakeup() and
                   voc_state_adie_sleep(); modified code to use voc database
                   for DTMF path setting; removed support for
                   FEATURE_SDEVMAP_SHARE_HFK and SECOND_UART; unfeaturized
                   sample rate code; added support for VOC_UNLOCK_VOC_NOT_RESET
                   in check_interlock(); modified duration calculation for
                   DTMFs for the Synth image; obsoleted FEATURE_NEW_SYNTH;
                   replaced voc_codec_gain() with voc_codec_pad(); modified
                   code to ensure DSP memory is not accessed incorrectly.
01/15/02   ymc     Added GSM and WCDMA support.
           ymc     Updated copyright.
12/11/01    aw     Added MSMAUD_ADIE_CODEC_CONFIG.
            ro     Use MSMAUD_ADIE_CODEC.
            ro     Include and use msmaud.h for ones_detect support.
11/02/01    sg     If the packet interface is in use, 1/8th rate null frames
                   will be sent from qdsp_cmd_tx_frame() instead of erasures
                   from voc_tx_cmd().
                   voc_rx_cmd will no longer drop frames if the packet interface
                   is in use. It will instead be dropped in qdsp_cmd_rx_frame().
10/29/01    sg     Added packet interface support.
10/04/01    sg     Added VCO and HCO modes for TTY.
10/01/01    st     Modified code to support multiple ADEC channels:
                   voc_adec_move_data(), voc_adec_flush(), voc_adec_reset(),
                   voc_adec_chan_pan(), and voc_adec_chan_vol().
10/01/01    sm     Added FEATURE_BT_AG_LOCAL_AUDIO.
09/04/01    aw     Added FEATURE_QSYNTH_ADRC. Set QSynth PCM filter and ADRC
                   configuration in voc_codec_select.
08/09/01   ymc     Not used TG for T_MSM5200 checking.
07/24/01   ymc     Added FEATURE_RUIM_DRIVER dependencies to
                   voc_read_ones_detect and voc_reset_ones_detect.
                   In voc_codec_gain, do qdsp_cmd_codec_gain_ctrl instead
                   of qdsp_cmd_set_pcmpad for VOC_CODEC_ON_BOARD.
                   Added support for loopback tests.
07/16/01    sm     Wakeup ADIE on TTY codec as well.
06/18/01  jkl/ym   Added FEATURE_ACP.
06/05/01    sm     Added voc_tty_cmd().
05/30/01    st     Wakeup/Shutdown ADIE on codec_select().
                   Check voc_adec_cb on voc_adec_move_data().
05/21/01   jkl     Removed Enhanced VR feature.
05/18/01    sm     Switch USB codec to use digital gains.
04/26/01    st     Modified code to add audio decoder type indication in
                   voc_adec_move_data() function.
04/26/01    sm     Added FEATURE_USB_ISOC_AUDIO support.
                   Removed speed_factor from voc_mp3_set_song_config().
04/11/01   jkl     Added voc_retry_codec_gain().
04/04/01    st     Updated featurization to include support for new features;
                   modified code to check if we are in the proper state
                   before issuing certain function calls; added
                   voc_adec_reset().
03/08/01  fas/sm   Corrected featurization for STEREO_DAC in voc_sample_rate().
03/02/01    sm     Added voc_set_bt_app_id() and voc_check_bt_ag(); modified
                   code to not configure to BT AG in voc_codec_select();
                   modified code to issue command for QSynth image in
                   voc_codec_gain().
02/28/01    st     Modified code to check interlocking in voc_adec_flush(),
                   voc_adec_pause() and voc_codec_gain() functions; removed
                   support for 16K codec types; lint updates.
01/26/01    st     Modified code to properly calculate duration of DTMFs
                   for QTunes image; modified code to use
                   voc_data_get_mm_rx_gain(); modified code to set sampling
                   rate in voc_sample_rate() if stereo DAC is active;
                   modified voc_codec_select() to properly handle switching
                   to/from stereo DAC.
12/10/00    st     Added proper featurization throughout file; modified code
                   to properly calculate duration of DTMFs for Synth image.
12/04/00  st/sm    Added voc_adec_chan_pan(), _adec_chan_vol(), _master_pan(),
                   _master_volume(), _mp3_set_song_config().  _adec_move_data()
                   returns voc_status_type.  Check adec error on move_data().
            sm     Added QTUNES, ADEC support.  Moved SDAC support to vocfunc.
10/23/00    st     Modified code to enable DFM commands in QSynth image.
10/18/00    sm     Call qdsp_peek/poke with int as length.
10/18/00    sm     Include bsp.h as part of MSMHW_INTERNAL_STEREO_DAC
10/13/00   ymc     Featurized support of internal SDAC as MSMHW_INTERNAL_STEREO_DAC.
09/22/00    sm     Corrected variable inside FEATURE_STEREO_DAC.
09/21/00    sm     Use voc_data_get_codec() and voc_data_get_codec_config()
                   Added 16k_on_chip_aux path to write_codec_gain()
09/12/00    sm     Featurize BT_AG codec path.
09/08/00    sm     Added Bluetooth/PPG support.
09/01/00   dsb     Added BSP support for TCXO frequency.
            sm     Use OUTHM functions to enable/disable SDAC clocks.
08/28/00    ro     Modified voc_valid_addr, voc_peek_cmd, and voc_poke_cmd to
                   use QDSP Services.
08/24/00    sm     New counter values for SDAC clocks.  Enable clk_ctl7 for
                   DAC on/off.  ** Use clkregim.c functions when available! **
08/16/00   jkl     Added check_interlock to write to DSP when in right state.
07/18/00    st     Added proper featurization.
07/15/00    st     Lint updates.
07/14/00   spf     Added support for TTY headset and HFK devices.
                   Replaced memory.h with string.h.  Lint updates.
06/19/00   spf     Disabled ones detection if FEATURE_RUIM_DRIVER is defined.
06/11/00   spf     Updated references.
06/07/00    st     Added code to enable and disable registered codec amplifier
                   functions; added interlocking to voc_check_codec_config().
06/07/00   spf     Added interlocking to voc_get_pcmpad() and
                   voc_put_pcmpad().
06/06/00    st     Added proper featurization.
05/30/00    st     Modified voc_dtmf_cmd() to enable DTMF generator in QSYNTH
                   image; enable PCM pad volume for QSYNTH image.
05/16/00    st     Added support for retrying voc_codec_select() command
                   upon QDSP failure; added support for stereo DAC codec;
                   added QSynth support.
05/12/00   spf     Add TTY support: external codec padval override.
           jwh     Add UART2/PCM sharing: voc_disable_hfk_pins() and
                   voc_enable_hfk_pins(). (FEATURE_SDEVMAP_SHARE_HFK)
05/02/00    sm     Added voc_aud_process_override( ) to override extra audio
                   features and reset the codec with the new settings.
           spf     Removed call to voc_data_set_dfm_sat_dcc_notify() in
                   voc_dfm_sat_ctrl().
03/28/00    ro     Mute DFM TX when SAT is disabled - CR 13320.
03/08/00   spf     Modified voc_pcm_loop_cmd() to enable audio loopback.
                   Cleaned up voc_compander_ctl().
03/01/00   spf     Fixed voc_compander_ctl() to not cause fatal error when
                   enabling or disabling Compander.
02/02/00   spf     Corrected voc_tx_mute_cmd() to use boolean
                   parameter in call to voc_data_set_tx_mute().
01/25/00    ro     Use voc_state_set_codec.
01/21/00   spf     Cleaned up voc_peek_cmd() and voc_poke_cmd().
12/07/99   spf     Implemented voc_peek_cmd() and voc_poke_cmd().  These
                   replaced voc_dpeek_cmd() and voc_dpoke_cmd().  Also
                   implemented voc_valid_addr().
11/22/99   jkl     Added new parameters for AGC/AVC.
10/26/99   jkl     Updated comments.
10/18/99    st     Added support for audio AGC/AVC.
10/18/99   tc      Check FEATURE_SECOND_UART.
10/14/99   jkl     Modified after code review.
10/12/99   tc      Added voc_read_ones_detect() and voc_reset_ones_detect().
10/04/99   jkl     Modified to convert dbm to hardware values.
                   Fix voc_codec_gain().
09/25/99   ro/st   Allow codec gain setting in IDLE
                   Fix interlocking in voc_rx_mute_cmd
                   Fix interlocking in voc_tx_mute_cmd
09/16/99   spf     Removed FEATURE_QDSP2.
09/10/99   spf     Added interlocking to Vocoder interface functions
                   to prevent QDSP DMA writes while in RESET state.
09/09/99   jkl     Modified voc_codec_gain to write to the right CODEC
07/30/99   st      Modified DTMF vocoder value calculation.
07/15/99   spf     DFM support.
06/15/99    ro     First version.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <string.h>             /* Interface to string library             */
#include "customer.h"           /* Customer Specific Definitions           */
#include "target.h"             /* Target specific definitions             */
#include "task.h"               /* Task definitions and prototypes         */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "assert.h"             /* Definitions for ASSERT                  */
#include "msmaud.h"             /* MSM HW interfaces for audio             */
#include "voci.h"               /* Internal Vocoder Task Definitions       */
#include "voc.h"                /* Vocoder interface definition            */
#include "vocdata.h"            /* Vocoder State and Cal Data Definitions  */
#include "vocsup.h"             /* Vocoder Support Routines                */
#include "vocstate.h"           /* Vocoder State Controller Definitions    */
#include "qdspcmd.h"            /* QDSP2 Driver Command Layer definitions  */
#include "qdsp.h"               /* QDSP Services header                    */
#include "msg.h"                /* Message logging macro                   */
#include "err.h"                /* Error logging macro                     */

#ifdef FEATURE_EXTERNAL_SDAC
#include "sdac.h"               /* External Stereo DAC support             */
#endif /* FEATURE_EXTERNAL_SDAC */
#ifdef FEATURE_BT_AG
#include "bt.h"                 /* Bluetooth prototypes                    */
#include "clkregim.h"           /* For clk_regime_enable/disable.          */
#endif /* FEATURE_BT_AG */

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
#include "audadsp_mixer.h"
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifndef MSMAUD_SCMM
#include "adie_services.h"
#endif /* MSMAUD_SCMM */

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */


/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/* Array to convert voc_rate_type to QDSP register values
*/
const uint8 voc_rate_conv[] = {
  QDSP_RATE_BLANK_V,                /* Blank        */
  QDSP_RATE_EIGHTH_V,               /* 8th rate     */
  QDSP_RATE_QUARTER_V,              /* Quarter rate */
  QDSP_RATE_HALF_V,                 /* Half rate    */
  QDSP_RATE_FULL_V,                 /* Full rate    */
  QDSP_RATE_ERASURE_V               /* Erasure      */
};
/* Array to convert Service Option rate to the Rate Limit Factor
*/
const uint8 vc_rate_factors[] = { 0, 0, 3, 0, 1, 0, 3 };

/* ===========================================================================
**
**                       S T A T E   V A R I A B L E S
**
** =========================================================================*/

/* Encoder control
*/
qdsp_cmd_enc_ctrl_type voc_enc_setting = {
  QDSP_REINIT_BNE_DIS_V,
  QDSP_RATE_EIGHTH_V,
  QDSP_RATE_FULL_V,
  QDSP_RATE_MODULATION_DIS_V,
  QDSP_RATE_REDUCE_144
};

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/* These parameters are used to keep track of the audio decoder callback
** function pointer and client data pointer.
*/
voc_adec_cb_func_type voc_adec_cb = NULL;
const void            *voc_adec_client_data = NULL;
uint8 voc_adec_failed = 0;

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
voc_codec_update_cb_func_type voc_codec_update_cb = NULL;
#endif


#ifdef FEATURE_GRAPH_ADPCM
/* These macros are used to verify that the ADPCM file is able to be played
** by the graphics images. Graphics images currently only support single
** channel, 4 bits per sample, 8kHz sampling and block size of 256 format.
*/
#define VOC_GRAPH_ADPCM_VALID_SAMPLE_RATE(x)     (x == 8000)
#define VOC_GRAPH_ADPCM_VALID_BLOCK_SIZE(x)      (x == 256)

/* These macros are used to verify that the ADPCM file is able to be played
** by the video front end (VFE) images. It only support single channel,
** 4 bits per sample, 32kHz sampling and block size of 512 format.
*/
#if defined(QDSP_IMAGE_VFE_BAYER_DEFINED) || \
    defined(QDSP_IMAGE_VFE_YCBCR_DEFINED)
#define VOC_GRAPH_VFE_32K_ADPCM_VALID_SAMPLE_RATE(x)     (x == 32000)
#define VOC_GRAPH_VFE_32K_ADPCM_VALID_BLOCK_SIZE(x)      (x == 512)
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED || QDSP_IMAGE_VFE_YCBCR_DEFINED */
#endif /* FEATURE_GRAPH_ADPCM */


#ifdef FEATURE_VOC_G711
#define VOC_G711_WAV_VALID_SAMPLE_RATE(x)          ((x) == 8000)
#define VOC_G711_WAV_VALID_BLOCK_SIZE(x)           ((x) == 1)
#define VOC_G711_WAV_VALID_CHANNELS(x)             ((x) == 1)
#define VOC_G711_WAV_VALID_BITS_PER_SAMPLE(x)      ((x) == 8)
#define VOC_G711_WAV_VALID_MODE(x)          ((x) == VOC_ADPCM_WAV_ALAW_MODE || \
                                             (x) == VOC_ADPCM_WAV_MULAW_MODE)
#endif /* FEATURE_VOC_G711 */

#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

#ifdef FEATURE_QAUDIOFX
/* These callback are for 3D positional final source settings query commands.
*/
voc_3d_pos_src_final_cb_type voc_3d_pos_src_final_cb[4];
#endif /* FEATURE_QAUDIOFX */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
/* Store bluetooth app id for use with bt_cmd_ag_audio_connect() and
** bt_cmd_ag_audio_disconnect().
*/
bt_app_id_type voc_bt_app_id     = BT_APP_ID_NULL;

/* State of BT AG/A2DP connection */
voc_bt_state_enum_type voc_bt_state = VOC_BT_STATE_DISABLED;
voc_bt_mode_type       voc_bt_mode  = VOC_BT_MODE_NONE;
boolean voc_sco_connected           = FALSE;
#endif /* defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
audiosbcenc_data_type          voc_sbc_data;
voc_bt_link_enabled_cb_type    voc_bt_a2dp_link_enabled_cb = NULL;
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_SBC_DSP_CODEC
/* This is when the DSP SBC encoder is not yet ready to accept the SBC
   configuration */
#define VOC_DSP_SBC_CONFIG_RETRY_COUNT_MAX  20
uint16 voc_dsp_sbc_config_retry_count = 0;
#endif

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/* This is when the DSP is not yet ready to accept the codec configuration
   for A2DP */
#define VOC_A2DP_ENABLE_RETRY_COUNT_MAX     150
uint16 voc_a2dp_enable_retry_count = 0;

/* This is retry timer to complete codec configuration for A2DP */
#define VOC_A2DP_CODEC_CONFIG_RETRY_COUNT_MAX     20
uint16 voc_a2dp_codec_config_retry_count = 0;

#endif

#ifdef FEATURE_BT_AG
/* This is when the DSP is not yet ready to accept the codec configuration
   for AG */
#define VOC_AG_ENABLE_RETRY_COUNT_MAX       20
uint16 voc_ag_enable_retry_count = 0;

voc_bt_link_enabled_cb_type    voc_bt_ag_link_enabled_cb = NULL;
#endif

#if defined(FEATURE_TTY) && defined(FEATURE_TTY_ACTIVITY_STATUS)
/* Callback to indicate TTY activity
*/
voc_tty_status_fn_type voc_tty_status_func = NULL;

#endif /* FEATURE_TTY && FEATURE_TTY_ACTIVITY_STATUS */

#ifdef FEATURE_MVS

#if defined(MSMAUD_VOC_AMR_WB)
/* AMR_WB Uplink frame processing function definition
*/
voc_amr_wb_ul_fn_type voc_amr_wb_ul_fn = NULL;

/* AMR_WB Downlink frame processing function definition
*/
voc_amr_wb_dl_fn_type voc_amr_wb_dl_fn = NULL;
#endif /* defined(MSMAUD_VOC_AMR_WB) */

#if defined(MSMAUD_VOC_AMR)
/* AMR Uplink frame processing function definition
*/
voc_amr_ul_fn_type voc_amr_ul_fn = NULL;

/* AMR Downlink frame processing function definition
*/
voc_amr_dl_fn_type voc_amr_dl_fn = NULL;
#endif /* defined(MSMAUD_VOC_AMR) */

#if defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) */

#ifdef MSMAUD_VOC_HR
/* HR Uplink frame processing function definition
*/
voc_hr_ul_fn_type voc_hr_ul_fn = NULL;

/* HR Downlink frame processing function definition
*/
voc_hr_dl_fn_type voc_hr_dl_fn = NULL;
#endif  /* MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_G711
/* G.711 Uplink frame processing function definition
*/
voc_g711_ul_fn_type voc_g711_ul_fn = NULL;

/* G.711 Downlink frame processing function definition
*/
voc_g711_dl_fn_type voc_g711_dl_fn = NULL;
#endif  /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
/* G.723 Uplink frame processing function definition
*/
voc_g723_ul_fn_type voc_g723_ul_fn = NULL;

/* G.723 Downlink frame processing function definition
*/
voc_g723_dl_fn_type voc_g723_dl_fn = NULL;
#endif  /* MSMAUD_VOC_G723 */

#endif  /* FEATURE_MVS */

#ifdef FEATURE_VOC_PACKET_INTERFACE

/* Callbacks for packet clients */
/* Reverse Link client */
voc_packet_rl_fn_type voc_packet_rl_func = NULL;

/* Forward Link client */
voc_packet_fl_fn_type voc_packet_fl_func = NULL;

#endif /* FEATURE_VOC_PACKET_INTERFACE */

/*Indicates the total no of DSP Failures due to Bitstream errors*/
#define MAX_DSP_FAILURES      3

/*Indicates the total no of DSP Failures for AAC due to Bitstream errors*/
#define MAX_AAC_DSP_FAILURES  30

#ifdef FEATURE_AUDFMT_EQUALIZER
/* This critial section is used to block the invalid access to the
** Equalizer gain table while other task is calculating the coefficient
*/
LOCAL rex_crit_sect_type voc_equalizer_crit_sect;
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_QTUNES_COMMON
boolean voc_adec_flush_retry_in_progress = FALSE;
#endif

#ifdef FEATURE_MM_REC
/*
 * Recording call back function.
 */
voc_mm_rec_cb_fn_type voc_mm_rec_cb_fn  = NULL;
#endif /* FEATURE_MM_REC */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)

boolean voc_bt_playback_dtmf_started =FALSE; //indicates a voctone dtmf tone

#endif

#ifdef SND_TEST_CODEC_OVERRIDE
/* table for the codec to be overriden */
voc_codec_type voc_codec_override_table[VOC_CODEC_MAX] = {VOC_CODEC_NONE};
/* table for the override flag */
uint8 voc_codec_override_flag_table[VOC_CODEC_MAX] = {FALSE};
#endif // SND_TEST_CODEC_OVERRIDE

/* AuxPGA Line In (on/off) status */
static boolean voc_aux_line_in_flag = FALSE;

static boolean voc_pa_on_status = FALSE;

#ifdef FEATURE_ULC_WARMUP_DELAY
LOCAL boolean voc_headphone_con = FALSE;
#endif /* FEATURE_ULC_WARMUP_DELAY */

/* <EJECT> */
/*===========================================================================

                 I N T E R N A L    F U N C T I O N S

===========================================================================*/
#ifdef SND_TEST_CODEC_OVERRIDE
/*===========================================================================

FUNCTION voc_set_codec_override

DESCRIPTION
  This function sets the codec to be override.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  If override flag is set this function updates the voc_codec_override_table
  table with the codec to be override

===========================================================================*/
void voc_set_codec_override
( voc_codec_type codec_to_be_override,
  voc_codec_type override_codec,
  uint8 overrideFlag
)
{
    if (overrideFlag == TRUE)
	{
		voc_codec_override_table[codec_to_be_override] = override_codec;
		voc_codec_override_flag_table[codec_to_be_override] = TRUE;
	}
	else if (overrideFlag == FALSE)
	{
		voc_codec_override_flag_table[codec_to_be_override] = FALSE;
	}
}
#endif // SND_TEST_CODEC_OVERRIDE

#ifdef MSMAUD_VOC_AMR_WB_COMMON
/*===========================================================================

FUNCTION voc_to_qdsp_amr_wb_ftype

DESCRIPTION
  This function can convert voc_amr_wb_frame_type into
  qdsp_cmd_amr_wb_frame_type.

DEPENDENCIES
  None.

RETURN VALUE
  qdsp_amr_wb_fid_ptr: holding converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_to_qdsp_amr_wb_ftype
(
  voc_amr_wb_frame_type voc_amr_wb_frame_id,
  qdsp_cmd_amr_wb_frame_type *qdsp_amr_wb_fid_ptr
)
{
  if (qdsp_amr_wb_fid_ptr == NULL)
  {
     return VOC_CMD_FAILED_S;
  }


  switch (voc_amr_wb_frame_id)
  {
    case VOC_AMR_WB_SPEECH_GOOD :
         *qdsp_amr_wb_fid_ptr = QDSP_AMR_WB_SPEECH_GOOD;
         break;

    case VOC_AMR_WB_SPEECH_DEGRADED:
         *qdsp_amr_wb_fid_ptr = QDSP_AMR_WB_SPEECH_DEGRADED;
         break;

    case VOC_AMR_WB_ONSET :
         *qdsp_amr_wb_fid_ptr = QDSP_AMR_WB_ONSET;
         break;

    case VOC_AMR_WB_SPEECH_BAD :
         *qdsp_amr_wb_fid_ptr = QDSP_AMR_WB_SPEECH_BAD;
         break;

    case VOC_AMR_WB_SID_FIRST :
         *qdsp_amr_wb_fid_ptr = QDSP_AMR_WB_SID_FIRST;
         break;

    case VOC_AMR_WB_SID_UPDATE :
         *qdsp_amr_wb_fid_ptr = QDSP_AMR_WB_SID_UPDATE;
         break;

    case VOC_AMR_WB_SID_BAD :
         *qdsp_amr_wb_fid_ptr = QDSP_AMR_WB_SID_BAD;
         break;

    case VOC_AMR_WB_NO_DATA :
         *qdsp_amr_wb_fid_ptr = QDSP_AMR_WB_NO_DATA;
         break;

    case VOC_AMR_WB_SPEECH_LOST :
         *qdsp_amr_wb_fid_ptr = QDSP_AMR_WB_SPEECH_LOST;
         break;

    default :
         return VOC_CMD_FAILED_S;
  }


  return VOC_DONE_S;
}


/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_to_voc_amr_wb_ftype

DESCRIPTION
  This function can convert qdsp_cmd_amr_wb_frame_type into
  voc_amr_wb_frame_type.

DEPENDENCIES
  None.

RETURN VALUE
  voc_amr_wb_fid_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type qdsp_to_voc_amr_wb_ftype
(
  qdsp_cmd_amr_wb_frame_type qdsp_amr_wb_frame_id,
  voc_amr_wb_frame_type *voc_amr_wb_fid_ptr
)
{
  if (voc_amr_wb_fid_ptr == NULL)
  {
   return VOC_CMD_FAILED_S;
  }


  switch (qdsp_amr_wb_frame_id)
  {
    case QDSP_AMR_WB_SPEECH_GOOD :
         *voc_amr_wb_fid_ptr = VOC_AMR_WB_SPEECH_GOOD;
         break;

    case QDSP_AMR_WB_SPEECH_DEGRADED :
         *voc_amr_wb_fid_ptr = VOC_AMR_WB_SPEECH_DEGRADED;
         break;

    case QDSP_AMR_WB_ONSET :
         *voc_amr_wb_fid_ptr = VOC_AMR_WB_ONSET;
         break;

    case QDSP_AMR_WB_SPEECH_BAD :
         *voc_amr_wb_fid_ptr = VOC_AMR_WB_SPEECH_BAD;
         break;

    case QDSP_AMR_WB_SID_FIRST :
         *voc_amr_wb_fid_ptr = VOC_AMR_WB_SID_FIRST;
         break;

    case QDSP_AMR_WB_SID_UPDATE :
         *voc_amr_wb_fid_ptr = VOC_AMR_WB_SID_UPDATE;
         break;

    case QDSP_AMR_WB_SID_BAD :
         *voc_amr_wb_fid_ptr = VOC_AMR_WB_SID_BAD;
         break;

    case QDSP_AMR_WB_NO_DATA :
         *voc_amr_wb_fid_ptr = VOC_AMR_WB_NO_DATA;
         break;

    case QDSP_AMR_WB_SPEECH_LOST :
         *voc_amr_wb_fid_ptr = VOC_AMR_WB_SPEECH_LOST;
         break;

    default :
         return VOC_CMD_FAILED_S;
  }


  return VOC_DONE_S;
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_to_qdsp_amr_wb_frate

DESCRIPTION
  This function can convert voc_amr_wb_frame_rate_type into
qdsp_cmd_amr_wb_frame_rate_type.

DEPENDENCIES
  None.

RETURN VALUE
  qdsp_amr_wb_frate_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_to_qdsp_amr_wb_frate
(
  voc_amr_wb_frame_rate_type voc_amr_wb_frame_rate,
  qdsp_cmd_amr_wb_frame_rate_type *qdsp_amr_wb_frate_ptr
)
{
  if (qdsp_amr_wb_frate_ptr == NULL)
  {
   return VOC_CMD_FAILED_S;
  }


  switch (voc_amr_wb_frame_rate)
  {
    case VOC_AMR_WB_RATE_0660 :
         *qdsp_amr_wb_frate_ptr = QDSP_AMR_WB_RATE_0660;
         break;

    case VOC_AMR_WB_RATE_0885 :
         *qdsp_amr_wb_frate_ptr = QDSP_AMR_WB_RATE_0885;
         break;

    case VOC_AMR_WB_RATE_1265 :
         *qdsp_amr_wb_frate_ptr = QDSP_AMR_WB_RATE_1265;
         break;

    case VOC_AMR_WB_RATE_1425 :
         *qdsp_amr_wb_frate_ptr = QDSP_AMR_WB_RATE_1425;
         break;

    case VOC_AMR_WB_RATE_1585 :
         *qdsp_amr_wb_frate_ptr = QDSP_AMR_WB_RATE_1585;
         break;

    case VOC_AMR_WB_RATE_1825 :
         *qdsp_amr_wb_frate_ptr = QDSP_AMR_WB_RATE_1825;
         break;

    case VOC_AMR_WB_RATE_1985 :
         *qdsp_amr_wb_frate_ptr = QDSP_AMR_WB_RATE_1985;
         break;

    case VOC_AMR_WB_RATE_2305 :
         *qdsp_amr_wb_frate_ptr = QDSP_AMR_WB_RATE_2305;
         break;

    case VOC_AMR_WB_RATE_2385 :
         *qdsp_amr_wb_frate_ptr = QDSP_AMR_WB_RATE_2385;
         break;

    default :
         return VOC_CMD_FAILED_S;
  }


  return VOC_DONE_S;
}


/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_to_voc_amr_wb_frate

DESCRIPTION
  This function can convert qdsp_cmd_amr_wb_frame_rate_type into
voc_amr_wb_frame_rate_type.

DEPENDENCIES
  None.

RETURN VALUE
  voc_amr_wb_frate_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type qdsp_to_voc_amr_wb_frate
(
  qdsp_cmd_amr_wb_frame_rate_type qdsp_amr_wb_frame_rate,
  voc_amr_wb_frame_rate_type *voc_amr_wb_frate_ptr
)
{
  if (voc_amr_wb_frate_ptr == NULL)
  {
   return VOC_CMD_FAILED_S;
  }


  switch (qdsp_amr_wb_frame_rate)
  {
    case QDSP_AMR_WB_RATE_0660 :
         *voc_amr_wb_frate_ptr = VOC_AMR_WB_RATE_0660;
         break;

    case QDSP_AMR_WB_RATE_0885 :
         *voc_amr_wb_frate_ptr = VOC_AMR_WB_RATE_0885;
         break;

    case QDSP_AMR_WB_RATE_1265 :
         *voc_amr_wb_frate_ptr = VOC_AMR_WB_RATE_1265;
         break;

    case QDSP_AMR_WB_RATE_1425 :
         *voc_amr_wb_frate_ptr = VOC_AMR_WB_RATE_1425;
         break;

    case QDSP_AMR_WB_RATE_1585 :
         *voc_amr_wb_frate_ptr = VOC_AMR_WB_RATE_1585;
         break;

    case QDSP_AMR_WB_RATE_1825 :
         *voc_amr_wb_frate_ptr = VOC_AMR_WB_RATE_1825;
         break;

    case QDSP_AMR_WB_RATE_1985 :
         *voc_amr_wb_frate_ptr = VOC_AMR_WB_RATE_1985;
         break;

    case QDSP_AMR_WB_RATE_2305 :
         *voc_amr_wb_frate_ptr = VOC_AMR_WB_RATE_2305;
         break;

    case QDSP_AMR_WB_RATE_2385 :
         *voc_amr_wb_frate_ptr = VOC_AMR_WB_RATE_2385;
         break;

    default :
         return VOC_CMD_FAILED_S;
  }


  return VOC_DONE_S;
}
#endif  /* MSMAUD_VOC_AMR_WB_COMMON */


#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR)

/*===========================================================================

FUNCTION voc_to_qdsp_gsm_ftype

DESCRIPTION
  This function can convert voc_gsm_frame_type into qdsp_cmd_gsm_frame_type.

DEPENDENCIES
  None.

RETURN VALUE
  qdsp_gsm_fid_ptr: holding converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_to_qdsp_gsm_ftype
(
  voc_gsm_frame_type voc_gsm_frame_id,
  qdsp_cmd_gsm_frame_type *qdsp_gsm_fid_ptr
)
{
  if (qdsp_gsm_fid_ptr == NULL)
  {
     return VOC_CMD_FAILED_S;
  }


  switch (voc_gsm_frame_id)
  {
    case VOC_GSM_SID :
         *qdsp_gsm_fid_ptr = QDSP_GSM_SID;
         break;

    case VOC_GSM_SPEECH_GOOD :
         *qdsp_gsm_fid_ptr = QDSP_GSM_SPEECH_GOOD;
         break;

    case VOC_GSM_BFI :
         *qdsp_gsm_fid_ptr = QDSP_GSM_BFI;
         break;

    case VOC_GSM_INVALID_SID :
         *qdsp_gsm_fid_ptr = QDSP_GSM_INVALID_SID;
         break;

    case VOC_AMR_SPEECH_GOOD :
         *qdsp_gsm_fid_ptr = QDSP_AMR_SPEECH_GOOD;
         break;

    case VOC_AMR_SPEECH_DEGRADED:
         *qdsp_gsm_fid_ptr = QDSP_AMR_SPEECH_DEGRADED;
         break;

    case VOC_AMR_ONSET :
         *qdsp_gsm_fid_ptr = QDSP_AMR_ONSET;
         break;

    case VOC_AMR_SPEECH_BAD :
         *qdsp_gsm_fid_ptr = QDSP_AMR_SPEECH_BAD;
         break;

    case VOC_AMR_SID_FIRST :
         *qdsp_gsm_fid_ptr = QDSP_AMR_SID_FIRST;
         break;

    case VOC_AMR_SID_UPDATE :
         *qdsp_gsm_fid_ptr = QDSP_AMR_SID_UPDATE;
         break;

    case VOC_AMR_SID_BAD :
         *qdsp_gsm_fid_ptr = QDSP_AMR_SID_BAD;
         break;

    case VOC_AMR_NO_DATA :
         *qdsp_gsm_fid_ptr = QDSP_AMR_NO_DATA;
         break;

    default :
         return VOC_CMD_FAILED_S;
  }


  return VOC_DONE_S;
}


/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_to_voc_gsm_ftype

DESCRIPTION
  This function can convert qdsp_cmd_gsm_frame_type into voc_gsm_frame_type.

DEPENDENCIES
  None.

RETURN VALUE
  voc_gsm_fid_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type qdsp_to_voc_gsm_ftype
(
  qdsp_cmd_gsm_frame_type qdsp_gsm_frame_id,
  voc_gsm_frame_type *voc_gsm_fid_ptr
)
{
  if (voc_gsm_fid_ptr == NULL)
  {
   return VOC_CMD_FAILED_S;
  }


  switch (qdsp_gsm_frame_id)
  {
    case QDSP_GSM_SID :
         *voc_gsm_fid_ptr = VOC_GSM_SID;
         break;

    case QDSP_GSM_SPEECH_GOOD :
         *voc_gsm_fid_ptr = VOC_GSM_SPEECH_GOOD;
         break;

    case QDSP_GSM_BFI :
         *voc_gsm_fid_ptr = VOC_GSM_BFI;
         break;

    case QDSP_GSM_INVALID_SID :
         *voc_gsm_fid_ptr = VOC_GSM_INVALID_SID;
         break;

    case QDSP_AMR_SPEECH_GOOD :
         *voc_gsm_fid_ptr = VOC_AMR_SPEECH_GOOD;
         break;

    case QDSP_AMR_SPEECH_DEGRADED :
         *voc_gsm_fid_ptr = VOC_AMR_SPEECH_DEGRADED;
         break;

    case QDSP_AMR_ONSET :
         *voc_gsm_fid_ptr = VOC_AMR_ONSET;
         break;

    case QDSP_AMR_SPEECH_BAD :
         *voc_gsm_fid_ptr = VOC_AMR_SPEECH_BAD;
         break;

    case QDSP_AMR_SID_FIRST :
         *voc_gsm_fid_ptr = VOC_AMR_SID_FIRST;
         break;

    case QDSP_AMR_SID_UPDATE :
         *voc_gsm_fid_ptr = VOC_AMR_SID_UPDATE;
         break;

    case QDSP_AMR_SID_BAD :
         *voc_gsm_fid_ptr = VOC_AMR_SID_BAD;
         break;

    case QDSP_AMR_NO_DATA :
         *voc_gsm_fid_ptr = VOC_AMR_NO_DATA;
         break;

    default :
         return VOC_CMD_FAILED_S;
  }


  return VOC_DONE_S;
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_to_qdsp_gsm_frate

DESCRIPTION
  This function can convert voc_gsm_frame_rate_type into
qdsp_cmd_gsm_frame_rate_type.

DEPENDENCIES
  None.

RETURN VALUE
  qdsp_gsm_frate_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_to_qdsp_gsm_frate
(
  voc_gsm_frame_rate_type voc_gsm_frame_rate,
  qdsp_cmd_gsm_frame_rate_type *qdsp_gsm_frate_ptr
)
{
  if (qdsp_gsm_frate_ptr == NULL)
  {
   return VOC_CMD_FAILED_S;
  }


  switch (voc_gsm_frame_rate)
  {
    case VOC_AMR_RATE_475 :
         *qdsp_gsm_frate_ptr = QDSP_AMR_RATE_475;
         break;

    case VOC_AMR_RATE_515 :
         *qdsp_gsm_frate_ptr = QDSP_AMR_RATE_515;
         break;

    case VOC_AMR_RATE_590 :
         *qdsp_gsm_frate_ptr = QDSP_AMR_RATE_590;
         break;

    case VOC_AMR_RATE_670 :
         *qdsp_gsm_frate_ptr = QDSP_AMR_RATE_670;
         break;

    case VOC_AMR_RATE_740 :
         *qdsp_gsm_frate_ptr = QDSP_AMR_RATE_740;
         break;

    case VOC_AMR_RATE_795 :
         *qdsp_gsm_frate_ptr = QDSP_AMR_RATE_795;
         break;

    case VOC_AMR_RATE_1020 :
         *qdsp_gsm_frate_ptr = QDSP_AMR_RATE_1020;
         break;

    case VOC_AMR_RATE_1220 :
         *qdsp_gsm_frate_ptr = QDSP_AMR_RATE_1220;
         break;

    case VOC_GSM_FULL_RATE :
         *qdsp_gsm_frate_ptr = QDSP_GSM_FULL_RATE;
         break;

    case VOC_GSM_HALF_RATE :
         *qdsp_gsm_frate_ptr = QDSP_GSM_HALF_RATE;
         break;

    default :
         return VOC_CMD_FAILED_S;
  }


  return VOC_DONE_S;
}


/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_to_voc_gsm_frate

DESCRIPTION
  This function can convert qdsp_cmd_gsm_frame_rate_type into
voc_gsm_frame_rate_type.

DEPENDENCIES
  None.

RETURN VALUE
  voc_gsm_frate_ptr: holding the converted value.
  VOC_CMD_FAILED_S or VOC_DONE_S.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type qdsp_to_voc_gsm_frate
(
  qdsp_cmd_gsm_frame_rate_type qdsp_gsm_frame_rate,
  voc_gsm_frame_rate_type *voc_gsm_frate_ptr
)
{
  if (voc_gsm_frate_ptr == NULL)
  {
   return VOC_CMD_FAILED_S;
  }


  switch (qdsp_gsm_frame_rate)
  {
    case QDSP_AMR_RATE_475 :
         *voc_gsm_frate_ptr = VOC_AMR_RATE_475;
         break;

    case QDSP_AMR_RATE_515 :
         *voc_gsm_frate_ptr = VOC_AMR_RATE_515;
         break;

    case QDSP_AMR_RATE_590 :
         *voc_gsm_frate_ptr = VOC_AMR_RATE_590;
         break;

    case QDSP_AMR_RATE_670 :
         *voc_gsm_frate_ptr = VOC_AMR_RATE_670;
         break;

    case QDSP_AMR_RATE_740 :
         *voc_gsm_frate_ptr = VOC_AMR_RATE_740;
         break;

    case QDSP_AMR_RATE_795 :
         *voc_gsm_frate_ptr = VOC_AMR_RATE_795;
         break;

    case QDSP_AMR_RATE_1020 :
         *voc_gsm_frate_ptr = VOC_AMR_RATE_1020;
         break;

    case QDSP_AMR_RATE_1220 :
         *voc_gsm_frate_ptr = VOC_AMR_RATE_1220;
         break;

    case QDSP_GSM_FULL_RATE :
         *voc_gsm_frate_ptr = VOC_GSM_FULL_RATE;
         break;

    case QDSP_GSM_HALF_RATE :
         *voc_gsm_frate_ptr = VOC_GSM_HALF_RATE;
         break;

    case QDSP_GSM_ENH_FULL_RATE :
         *voc_gsm_frate_ptr = VOC_GSM_ENH_FULL_RATE;
         break;

    default :
         return VOC_CMD_FAILED_S;
  }


  return VOC_DONE_S;
}
#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR || MSMAUD_VOC_HR */


/* <EJECT> */
#ifdef FEATURE_MVS

#if defined(MSMAUD_VOC_AMR_WB)
/*===========================================================================

FUNCTION voc_amr_wb_ul

DESCRIPTION
  AMR_WB uplink (tx) frame processing function.  This function converts
  the AMR_WB frame mode and type to vocoder units and calls the registered
  AMR_WB uplink processing funtion.

DEPENDENCIES
  Must call voc_amr_wb_register to register the AMR_WB uplink processing
  function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_amr_wb_ul(
  uint16                        *vocoder_packet,
  qdsp_cmd_amr_wb_frame_type       frame_type,
  qdsp_cmd_amr_wb_frame_rate_type  amr_wb_mode
)
{
  voc_amr_wb_frame_type      amr_wb_type_voc = VOC_AMR_WB_NO_DATA;
  voc_amr_wb_frame_rate_type amr_wb_mode_voc = VOC_AMR_WB_RATE_1265;

  /* Convert input parameters to vocoder units
  */
  (void) qdsp_to_voc_amr_wb_ftype(frame_type, &amr_wb_type_voc);
  (void) qdsp_to_voc_amr_wb_frate(amr_wb_mode, &amr_wb_mode_voc);

  if (voc_amr_wb_ul_fn != NULL) {
    /* Give the vocoder frame to WCDMA Voice Services
    */
    voc_amr_wb_ul_fn(
      (uint8 *) vocoder_packet,
      amr_wb_type_voc,
      amr_wb_mode_voc
    );
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_amr_wb_dl

DESCRIPTION
  AMR_WB downlink (rx) frame processing function.  This function gets the
  AMR_WB frame, mode, and type from MVS and converts the mode and type to
  QDSP units.

DEPENDENCIES
  Must call voc_amr_wb_register to register the AMR_WB downlink processing function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_amr_wb_dl(
  uint16                        *vocoder_packet,
  qdsp_cmd_amr_wb_frame_type       *frame_type,
  qdsp_cmd_amr_wb_frame_rate_type  *amr_wb_mode
)
{
  voc_amr_wb_frame_type      amr_wb_type_voc = VOC_AMR_WB_NO_DATA;
  voc_amr_wb_frame_rate_type amr_wb_mode_voc = VOC_AMR_WB_RATE_1265;

  if (voc_amr_wb_dl_fn != NULL) {
    /* Get the vocoder frame from MVS Voice Services
    */
    voc_amr_wb_dl_fn(
      (uint8 *) vocoder_packet,
      &amr_wb_type_voc,
      &amr_wb_mode_voc
    );
  }

  /* Convert output parameters to QDSP units
  */
  (void) voc_to_qdsp_amr_wb_ftype(amr_wb_type_voc, frame_type);
  (void) voc_to_qdsp_amr_wb_frate(amr_wb_mode_voc, amr_wb_mode);
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_amr_wb_register

DESCRIPTION
  Register the functions used for uplink and downlink
  frame processing in AMR_WB.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_amr_wb_register(
  voc_amr_wb_ul_fn_type  uplink_func,
  voc_amr_wb_dl_fn_type  downlink_func
)
{
  voc_amr_wb_ul_fn = uplink_func;
  voc_amr_wb_dl_fn = downlink_func;

  if ((uplink_func != NULL) && (downlink_func != NULL)) {
    qdsp_cmd_amr_wb_register(
      voc_amr_wb_ul,
      voc_amr_wb_dl
    );
  } else {
    qdsp_cmd_amr_wb_register(
      NULL,NULL
    );
  }
}
#endif /* defined(MSMAUD_VOC_AMR_WB) */

/* <EJECT> */
#if defined(MSMAUD_VOC_AMR)
/*===========================================================================

FUNCTION voc_amr_ul

DESCRIPTION
  AMR uplink (tx) frame processing function.  This function converts
  the AMR frame mode and type to vocoder units and calls the registered
  AMR uplink processing funtion.

DEPENDENCIES
  Must call voc_amr_register to register the AMR uplink processing function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_amr_ul(
  uint16                        *vocoder_packet,
  qdsp_cmd_gsm_frame_type       frame_type,
  qdsp_cmd_gsm_frame_rate_type  amr_mode
)
{
  voc_gsm_frame_type      amr_type_voc = VOC_AMR_NO_DATA;
  voc_gsm_frame_rate_type amr_mode_voc = VOC_AMR_RATE_1220;

  /* Convert input parameters to vocoder units
  */
  (void) qdsp_to_voc_gsm_ftype(frame_type, &amr_type_voc);
  (void) qdsp_to_voc_gsm_frate(amr_mode, &amr_mode_voc);

  if (voc_amr_ul_fn != NULL) {
    /* Give the vocoder frame to WCDMA Voice Services
    */
    voc_amr_ul_fn(
      (uint8 *) vocoder_packet,
      amr_type_voc,
      amr_mode_voc
    );
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_amr_dl

DESCRIPTION
  AMR downlink (rx) frame processing function.  This function gets the
  AMR frame, mode, and type from MVS and converts the mode and type to
  QDSP units.

DEPENDENCIES
  Must call voc_amr_register to register the AMR downlink processing function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_amr_dl(
  uint16                        *vocoder_packet,
  qdsp_cmd_gsm_frame_type       *frame_type,
  qdsp_cmd_gsm_frame_rate_type  *amr_mode
)
{
  voc_gsm_frame_type      amr_type_voc = VOC_AMR_NO_DATA;
  voc_gsm_frame_rate_type amr_mode_voc = VOC_AMR_RATE_1220;

  if (voc_amr_dl_fn != NULL) {
    /* Get the vocoder frame from MVS Voice Services
    */
    voc_amr_dl_fn(
      (uint8 *) vocoder_packet,
      &amr_type_voc,
      &amr_mode_voc
    );
  }

  /* Convert output parameters to QDSP units
  */
  (void) voc_to_qdsp_gsm_ftype(amr_type_voc, frame_type);
  (void) voc_to_qdsp_gsm_frate(amr_mode_voc, amr_mode);
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_amr_register

DESCRIPTION
  Register the functions used for uplink and downlink
  frame processing in AMR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_amr_register(
  voc_amr_ul_fn_type  uplink_func,
  voc_amr_dl_fn_type  downlink_func
)
{
  voc_amr_ul_fn = uplink_func;
  voc_amr_dl_fn = downlink_func;

  if ((uplink_func != NULL) && (downlink_func != NULL)) {
    qdsp_cmd_amr_register(
      voc_amr_ul,
      voc_amr_dl
    );
  } else {
    qdsp_cmd_amr_register(
      NULL,NULL
    );
  }
}
#endif /* defined(MSMAUD_VOC_AMR) */

/* <EJECT> */
#if defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) */

/* <EJECT> */
#ifdef MSMAUD_VOC_HR
/*===========================================================================

FUNCTION voc_hr_ul

DESCRIPTION
  HR uplink (tx) frame processing function.  This function converts
  the HR frame mode and type to vocoder units and calls the registered
  HR uplink processing funtion.

DEPENDENCIES
  Must call voc_hr_register to register the HR uplink processing function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_hr_ul(
  uint16                        *vocoder_packet,
  qdsp_cmd_gsm_frame_type       frame_type,
  boolean                       enc_err_flag
)
{
  voc_gsm_frame_type      frame_type_voc = VOC_AMR_NO_DATA;

  /* Convert input parameters to vocoder units */
  (void) qdsp_to_voc_gsm_ftype(frame_type, &frame_type_voc);

  if (voc_hr_ul_fn != NULL) {
    /* Give the vocoder frame to registered client */
    voc_hr_ul_fn(
      (uint8 *) vocoder_packet,
      frame_type_voc,
      enc_err_flag
    );
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_hr_dl

DESCRIPTION
  HR downlink (rx) frame processing function.  This function gets the
  HR frame information from MVS and converts the information to
  QDSP units.

DEPENDENCIES
  Must call voc_hr_register to register the HR downlink processing
function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_hr_dl(
  uint16                        *vocoder_packet,
  qdsp_cmd_gsm_frame_type       *frame_type,
  boolean                       *taf,
  boolean                       *bfi,
  boolean                       *ufi
)
{
  voc_gsm_frame_type      frame_type_voc = VOC_AMR_NO_DATA;
  *taf = FALSE;
  *bfi = FALSE;
  *ufi = FALSE;

  if (voc_hr_dl_fn != NULL) {
    /* Get the vocoder frame from registered client
    */
    voc_hr_dl_fn(
      (uint8 *) vocoder_packet,
      &frame_type_voc,
      taf,
      bfi,
      ufi
    );
  }

  /* Convert output parameters to QDSP units
  */
  (void) voc_to_qdsp_gsm_ftype(frame_type_voc, frame_type);
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_hr_register

DESCRIPTION
  Register the functions used for uplink and downlink
  frame processing in HR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_hr_register(
  voc_hr_ul_fn_type  uplink_func,
  voc_hr_dl_fn_type  downlink_func
)
{
  voc_hr_ul_fn = uplink_func;
  voc_hr_dl_fn = downlink_func;

  if ((uplink_func != NULL) && (downlink_func != NULL)) {
    qdsp_cmd_hr_register(
      voc_hr_ul,
      voc_hr_dl
    );
  } else {
    qdsp_cmd_hr_register(
      NULL,NULL
    );
  }
}

#endif  /* MSMAUD_VOC_HR */

/* <EJECT> */
#ifdef MSMAUD_VOC_G711
/*===========================================================================

FUNCTION voc_g711_ul

DESCRIPTION
  G.711 uplink (tx) frame processing function.  This function calls the
  registered G.711 uplink processing funtion.

DEPENDENCIES
  Must call voc_g711_register to register the G.711 uplink processing function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_g711_ul(
  uint16                        *vocoder_packet,
  uint16                        frame_length,
  qdsp_cmd_g711_mode_type       qdsp_g711_mode
)
{
  voc_g711_mode_type      g711_mode;

  /* Convert input parameters to vocoder units */
  if(qdsp_g711_mode == QDSP_G711_MODE_MULAW) {
    g711_mode = VOC_G711_MODE_MULAW;
  } else {
    g711_mode = VOC_G711_MODE_ALAW;
  }

  if (voc_g711_ul_fn != NULL) {
    /* Give the vocoder frame to registered client */
    voc_g711_ul_fn(
      (uint8 *) vocoder_packet,
      frame_length,
      g711_mode
    );
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_g711_dl

DESCRIPTION
  G.711 downlink (rx) frame processing function.  This function gets the
  G.711 frame information from MVS and converts the information to
  QDSP units.

DEPENDENCIES
  Must call voc_g711_register to register the G.711 downlink processing
function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_g711_dl(
  uint16                        *vocoder_packet,
  uint16                        *frame_length,
  qdsp_cmd_g711_mode_type       *qdsp_g711_mode
)
{
  voc_g711_mode_type      g711_mode;

  if (voc_g711_dl_fn != NULL) {
    /* Get the vocoder frame from registered client
    */
    voc_g711_dl_fn(
      (uint8 *) vocoder_packet,
      frame_length,
      &g711_mode
    );
  }

  /* Convert output parameters to QDSP units
  */
  if(g711_mode == VOC_G711_MODE_MULAW) {
    *qdsp_g711_mode = QDSP_G711_MODE_MULAW;
  } else {
    *qdsp_g711_mode = QDSP_G711_MODE_ALAW;
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_g711_register

DESCRIPTION
  Register the functions used for uplink and downlink
  frame processing in G.711 vocoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_g711_register(
  voc_g711_ul_fn_type  uplink_func,
  voc_g711_dl_fn_type  downlink_func
)
{
  voc_g711_ul_fn = uplink_func;
  voc_g711_dl_fn = downlink_func;

  if ((uplink_func != NULL) && (downlink_func != NULL)) {
    qdsp_cmd_g711_register(
      voc_g711_ul,
      voc_g711_dl
    );
  } else {
    qdsp_cmd_g711_register(
      NULL,NULL
    );
  }
}
#endif  /* MSMAUD_VOC_G711 */

/* <EJECT> */
#ifdef MSMAUD_VOC_G723
/*===========================================================================

FUNCTION voc_g723_ul

DESCRIPTION
  G.723 uplink (tx) frame processing function.  This function calls the
  registered G.723 uplink processing funtion.

DEPENDENCIES
  Must call voc_g723_register to register the G.723 uplink processing function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_g723_ul(
  uint16                        *vocoder_packet,
  uint16                        frame_length,
  qdsp_cmd_g723_mode_type       qdsp_g723_mode
)
{
  voc_g723_mode_type      g723_mode;

  /* Convert input parameters to vocoder units */
  switch(qdsp_g723_mode) {
    case QDSP_G723_MODE_HIGH:
      g723_mode = VOC_G723_MODE_HIGH;
      break;
    case QDSP_G723_MODE_LOW:
      g723_mode = VOC_G723_MODE_LOW;
      break;
    case QDSP_G723_MODE_SID:
      g723_mode = VOC_G723_MODE_SID;
      break;
    case QDSP_G723_MODE_BLANK:
      g723_mode = VOC_G723_MODE_BLANK;
      break;
    case QDSP_G723_MODE_ERROR:
    default:
      g723_mode = VOC_G723_MODE_ERROR;
      break;
  }

  if (voc_g723_ul_fn != NULL) {
    /* Give the vocoder frame to registered client */
    voc_g723_ul_fn(
      (uint8 *) vocoder_packet,
      frame_length,
      g723_mode
    );
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_g723_dl

DESCRIPTION
  G.723 downlink (rx) frame processing function.  This function gets the
  G.723 frame information from MVS and converts the information to
  QDSP units.

DEPENDENCIES
  Must call voc_g723_register to register the G.723 downlink processing
function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_g723_dl(
  uint16                        *vocoder_packet,
  uint16                        *frame_length,
  qdsp_cmd_g723_mode_type       *qdsp_g723_mode
)
{
  voc_g723_mode_type      g723_mode;

  if (voc_g723_dl_fn != NULL) {
    /* Get the vocoder frame from registered client
    */
    voc_g723_dl_fn(
      (uint8 *) vocoder_packet,
      frame_length,
      &g723_mode
    );
  }

  /* Convert output parameters to QDSP units
  */
  switch(g723_mode) {
    case VOC_G723_MODE_HIGH:
      *qdsp_g723_mode = QDSP_G723_MODE_HIGH;
      break;
    case VOC_G723_MODE_LOW:
      *qdsp_g723_mode = QDSP_G723_MODE_LOW;
      break;
    case VOC_G723_MODE_SID:
      *qdsp_g723_mode = QDSP_G723_MODE_SID;
      break;
    case VOC_G723_MODE_BLANK:
      *qdsp_g723_mode = QDSP_G723_MODE_BLANK;
      break;
    case VOC_G723_MODE_ERROR:
    default:
      *qdsp_g723_mode = QDSP_G723_MODE_ERROR;
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_g723_register

DESCRIPTION
  Register the functions used for uplink and downlink
  frame processing in G.723 vocoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_g723_register(
  voc_g723_ul_fn_type  uplink_func,
  voc_g723_dl_fn_type  downlink_func
)
{
  voc_g723_ul_fn = uplink_func;
  voc_g723_dl_fn = downlink_func;

  if ((uplink_func != NULL) && (downlink_func != NULL)) {
    qdsp_cmd_g723_register(
      voc_g723_ul,
      voc_g723_dl
    );
  } else {
    qdsp_cmd_g723_register(
      NULL,NULL
    );
  }
}
#endif  /* MSMAUD_VOC_G723 */

#endif  /* FEATURE_MVS */

/* <EJECT> */

#ifdef FEATURE_VOC_PACKET_INTERFACE
/*===========================================================================

FUNCTION VOC_RL_PACKET_PROCESSOR

DESCRIPTION
  Takes the data from the QDSP software and prepares it
  for the client.

DEPENDENCIES
  voc_packet_rl_func must be initialized using voc_register_packet_clients()


RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_rl_packet_processor(
  uint8   *vocoder_packet,    /* Pointer to data                 */
  uint16  qdsp_current_rate,  /* Rate for current vocoder frame  */
  uint16  packet_length       /* Length of current packet        */
)
{
  voc_rate_type voc_current_rate;

  switch (qdsp_current_rate) {
    case QDSP_RATE_BLANK_V:
      voc_current_rate = VOC_0_RATE;
      break;
    case QDSP_RATE_EIGHTH_V:
      voc_current_rate = VOC_8_RATE;
      break;
    case QDSP_RATE_QUARTER_V:
      voc_current_rate = VOC_4_RATE;
      break;
    case QDSP_RATE_HALF_V:
      voc_current_rate = VOC_2_RATE;
      break;
    case QDSP_RATE_FULL_V:
      voc_current_rate = VOC_1_RATE;
      break;
    default:
      MSG_HIGH ("Invalid qdsp rate: %x",qdsp_current_rate,0,0);
      voc_current_rate = VOC_ERR_RATE;
      /* SG: return? */
      break;
  }

  if (voc_packet_rl_func != NULL) {
    voc_packet_rl_func(
      vocoder_packet,
      voc_current_rate,
      packet_length
    );
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_FL_PACKET_PROCESSOR

DESCRIPTION
  Gets the FL packet from the client and prepares it
  for the QDSP software.

DEPENDENCIES
  voc_packet_rl_func must be initialized using voc_register_packet_clients()

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_fl_packet_processor()
{
  voc_rate_type voc_rate = VOC_8_RATE;
  uint8* data_ptr = NULL;
  uint16  qdsp_rate;

  if (voc_packet_fl_func != NULL) {
    /* Get the vocoder frame from CDMA Voice Services
    */
    voc_packet_fl_func(
      &data_ptr,
      &voc_rate
    );
  }

  if(data_ptr == NULL)
  {
    qdsp_cmd_dec_semaphore_flag_set(FALSE);

    MSG_HIGH("ForWard Link Data Pointer is NULL",0,0,0);    

    if(voc_packet_fl_func == NULL)
    {
      MSG_HIGH("ForWard Link Function is NULL",0,0,0);       
    }    
    return;
  }

  /* Check for illegal rates
  */
  if ( voc_rate > VOC_ERASURE ) {
    ERR( "Rx Illegal rate = %x", voc_rate, 0, 0 );
    voc_rate = VOC_8_RATE;
    /* SG: VOC_ERASURE instead? */
  }
  qdsp_rate = voc_rate_conv[voc_rate];

  qdsp_cmd_process_rx_frame(qdsp_rate, data_ptr);

  qdsp_cmd_dec_semaphore_flag_set(TRUE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_REGISTER_PACKET_CLIENTS

DESCRIPTION
  Clients that use packet interface to send/receive data must
  register callbacks using this function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_register_packet_clients (
  voc_packet_rl_fn_type  rl_func,     /* Reverse Link Client function */
  voc_packet_fl_fn_type  fl_func      /* Forward Link Client function */
)
{

  voc_packet_rl_func = rl_func;
  voc_packet_fl_func = fl_func;

  if ((rl_func != NULL) && (fl_func != NULL)) {
    qdsp_cmd_register_packet_clients(
      voc_rl_packet_processor,
      voc_fl_packet_processor
    );
  } else {
    qdsp_cmd_register_packet_clients(
      NULL, NULL
    );
  }
}
#endif /* FEATURE_VOC_PACKET_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_check_pa_on_status

DESCRIPTION
  This function returns the current status of power amplifiers.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - power amplifiers are on.
  FALSE - power amplifiers are off.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_check_pa_on_status (
  void
)
{
  boolean ret_val;
  if(voc_state_aux_pcm_clk_query())
  {
    ret_val = TRUE;
  }
  else
  {
    ret_val = voc_pa_on_status;
  }

  return ret_val;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_pa_state

DESCRIPTION
  This function will set power amplifier on/off status to the incoming state.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void voc_set_pa_state (
  boolean state
)
{
  voc_pa_on_status = state;
}

/* <EJECT> */
/*===========================================================================
FUNCTION check_interlock

DESCRIPTION
  Check whether the DSP is in correct state to accept commands

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - DSP is in the proper state.
  FALSE - DSP is not in the proper state.

SIDE EFFECTS
  None.

===========================================================================*/
boolean check_interlock (voc_unlock_type check)
{
  voc_inform_type  inform_val;
  boolean          result = FALSE;

  inform_val = voc_state_get_inform_val();

  switch (check) {
    case VOC_UNLOCK_NOT_RESET:
      if (inform_val != VOC_INFORM_RESET) {
        result = TRUE;
      }
      break;

    case VOC_UNLOCK_VOC_NOT_RESET:
      if ((inform_val != VOC_INFORM_RESET)
#ifdef FEATURE_QSYNTH_COMMON
          && (inform_val != VOC_INFORM_QSYNTH)
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_QTUNES_COMMON
          && (inform_val != VOC_INFORM_QTUNES)
#endif /* FEATURE_QTUNES_COMMON */
#ifdef FEATURE_GRAPH_AUDIO
          && (inform_val != VOC_INFORM_GRAPH_IDLE)
          && (inform_val != VOC_INFORM_GRAPH_AUDIO)
#endif /* FEATURE_GRAPH_AUDIO */
         ) {
        result = TRUE;
      }
      break;

    case VOC_UNLOCK_MODEM:
      if((inform_val == VOC_INFORM_VOCODER)
#ifdef FEATURE_GRAPH_AUDIO
         || (inform_val == VOC_INFORM_GRAPH_AUDIO)
#endif /* FEATURE_GRAPH_AUDIO */
        ) {
        result = TRUE;
      }
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
      break;

    case VOC_UNLOCK_VOCODER:
      if (inform_val == VOC_INFORM_VOCODER) {
        result = TRUE;
      }
      break;

#ifdef FEATURE_VOC_ADPCM
    case VOC_UNLOCK_VOCODER_ADPCM:
      if (inform_val == VOC_INFORM_VOCODER_ADPCM) {
        result = TRUE;
      }
      break;
#endif /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_AVS_INCALL_ADEC_PB
    case VOC_UNLOCK_VOCODER_ADEC:
      if (inform_val == VOC_INFORM_VOCODER_ADEC) {
        result = TRUE;
      }
      break;
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
    case VOC_UNLOCK_MIXER_ADEC:
      if ((inform_val == VOC_INFORM_MIXER_ACTIVE) ||
          (inform_val == VOC_INFORM_MIXER_IDLE)) {
        result = TRUE;
      }
      break;
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

#ifdef FEATURE_QSYNTH_COMMON
    case VOC_UNLOCK_QSYNTH_STD:
      if (inform_val == VOC_INFORM_QSYNTH) {
        result = TRUE;
      }
      break;

    case VOC_UNLOCK_QSYNTH:
      if (inform_val == VOC_INFORM_QSYNTH
#ifdef FEATURE_AVS_INCALL_MIXER_CMX
          || ((inform_val == VOC_INFORM_MIXER_ACTIVE) &&
              (voc_state_incall_synth_ready() == TRUE))
#endif /* FEATURE_AVS_INCALL_MIXER_CMX */
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
          ||(inform_val == VOC_INFORM_CAM_AUDIO_ACTIVE)
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
         ) {
        return TRUE;
      }
      break;
#endif

#ifdef FEATURE_QTUNES_COMMON
    case VOC_UNLOCK_QTUNES:
      if ((inform_val == VOC_INFORM_QTUNES)
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
      || (inform_val == VOC_INFORM_CAM_AUDIO_ACTIVE)
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY*/
     ){
        return TRUE;
      }
      break;
#endif

#ifdef FEATURE_GRAPH_AUDIO
    case VOC_UNLOCK_GRAPH:
      if((inform_val == VOC_INFORM_GRAPH_IDLE) ||
         (inform_val == VOC_INFORM_GRAPH_AUDIO)) {
        return TRUE;
      }
      break;

    case VOC_UNLOCK_GRAPH_AUDIO:
      if (inform_val == VOC_INFORM_GRAPH_AUDIO) {
        return TRUE;
      }
      break;
#endif /* FEATURE_GRAPH_AUDIO */

    default:
      break;
  }
  if (result == TRUE) {
#ifdef FEATURE_PHONE_VR
    if (vocvr_match_cmd_in_progress()) {
      return FALSE;
    }
#endif
    return TRUE;
  }
  return(FALSE);
} /* check_interlock */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_check_cap_ready

DESCRIPTION
  This function checks whether vocoder service is ready to handle
  request capability

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
boolean voc_check_cap_ready
(
  voc_capability_type cap
) {
  boolean ret_val = FALSE;

  switch(cap) {
#ifdef FEATURE_QSYNTH_COMMON
    case VOC_CAP_QSYNTH:
      ret_val = check_interlock(VOC_UNLOCK_QSYNTH);
      break;
#endif /* FEATURE_QSYNTH_COMMON */
    default:
      break;
  }

  return ret_val;
}
/*===========================================================================

                 V O C O D E R   A P I   F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION voc_read_ones_detect

DESCRIPTION
  This function reads ones detect

DEPENDENCIES
  None.

RETURN VALUE
  Returns the ones detect status.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_read_ones_detect (
  void
)
{
  return( (MSMAUD_READ_ONES_DETECT()) != 0);
} /* voc_read_ones_detect */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_reset_ones_detect

DESCRIPTION
  This function resets ones detect.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_reset_ones_detect (
  void
)
{
  MSMAUD_RESET_ONES_DETECT();
} /* voc_reset_ones_detect */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_register_codec_func

DESCRIPTION
  Set the specified codec enable function.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S - Command completed succesfully

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_register_codec_func
(
  voc_codec_type                pcm_path,
  voc_amp_enable_func_ptr_type  func_ptr
) {
  if(voc_data_register_codec_func(pcm_path, func_ptr) == VOC_DATA_SUCCESS) {
    return(VOC_DONE_S);
  } else {
    return(VOC_CMD_FAILED_S);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_enable_codec_func

DESCRIPTION
  Enables codec with registered codec function of current path.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_enable_codec_func ()
{
  voc_codec_type                pcm_path;
  voc_codec_mode_type           codec_mode;
  voc_amp_enable_func_ptr_type  codec_func;
  voc_state_control_type        *state;

  /* Retrieve codec */
  voc_data_get_codec(&pcm_path);
  state = voc_state_get_state_data();
  voc_data_get_codec_mode(state->current_image, &codec_mode);

  (void)voc_data_get_codec_func(pcm_path, &codec_func);

  if(codec_func != NULL) {
    codec_func(pcm_path, codec_mode, TRUE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_disable_codec_func

DESCRIPTION
  Disables all codecs with registered codec functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_disable_codec_func (
  boolean disable_current_path
    /* TRUE: disables all paths; FALSE: disables all but current path */
) {
  voc_codec_type                pcm_path;
  voc_codec_mode_type           codec_mode;
  voc_amp_enable_func_ptr_type  codec_func;
  uint8                         counter;
  voc_state_control_type        *state;

  /* Retrieve codec */
  voc_data_get_codec(&pcm_path);
  state = voc_state_get_state_data();

  for(counter = VOC_CODEC_DEFAULT; counter < VOC_CODEC_MAX; counter++) {
    (void)voc_data_get_codec_func((voc_codec_type) counter, &codec_func);
    if((disable_current_path) || (counter != pcm_path)) {
      if(codec_func != NULL) {
        voc_data_get_codec_mode(state->current_image, &codec_mode);
        codec_func((voc_codec_type)counter, codec_mode, FALSE);
      }
    }
  }
}
/*===========================================================================

FUNCTION voc_set_qtv_cb_func

DESCRIPTION
  This function is used to inform the audio drivers of the QTV callback fucntion
  to be called by audio drivers to inform QTV about the QDSP_MOD_DISABLE evt
  when the image swap is happening from audio only image to audio/video image.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
==========================================================================*/
void voc_set_qtv_cb_func(voc_notify_qtv_cb_func_ptr_type cb_func)
{
   voc_data_set_qtv_cb_func(cb_func);
}
/* <EJECT> */
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)

#if defined (FEATURE_AVS_BT_SCO_REWORK)
/*===========================================================================

FUNCTION voc_set_bt_cb_func

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
void voc_set_bt_cb_func(voc_bt_cb_func_ptr_type cb_func)
{
  voc_data_set_bt_cb_func(cb_func);
}
/*===========================================================================
FUNCTION voc_get_bt_playback_dtmf_start

DESCRIPTION
  This function is used to get the status of the dtmf playback event
  that is sent to BT driver from voc layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
boolean voc_get_bt_playback_dtmf_start(void)
{
  return voc_bt_playback_dtmf_started;
}
/*===========================================================================
FUNCTION voc_set_bt_playback_dtmf_start

DESCRIPTION
  This function is used to set the status of the dtmf playback event
  that is sent to BT driver from voc layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_set_bt_playback_dtmf_start(boolean bt_dtmf_play_evt_status)
{
  voc_bt_playback_dtmf_started = bt_dtmf_play_evt_status;
}
#endif /*AVS_BT_SCO_REWORK*/

/*===========================================================================

FUNCTION VOC_SET_BT_APP_ID

DESCRIPTION
  This procedure initializes its local bluetooth application ID.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  voc_bt_app_id

===========================================================================*/
void voc_set_bt_app_id( bt_app_id_type bt_app_id ) {
  voc_bt_app_id = bt_app_id;
}

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_GET_BT_APP_ID

DESCRIPTION
  This function returns the BT App id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
bt_app_id_type voc_get_bt_app_id() {
  return voc_bt_app_id;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_check_bt

DESCRIPTION
  Enables/disables Bluetooth A2DP or AG depending on the input.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if BT A2DP is current path and connected.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_check_bt (voc_bt_op_type operation)
{
  voc_codec_type                   pcm_path;
  voc_codec_class_type             codec_class;
  boolean retval = FALSE;
  voc_bt_mode_type bt_mode;

  voc_data_get_codec(&pcm_path);
  voc_data_get_codec_class(&codec_class);
  bt_mode = voc_get_bt_mode();

  switch (operation) {

    case VOC_BT_OP_FORCE_DISABLE:
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    case VOC_BT_OP_A2DP_FORCE_DISABLE:
#endif
#ifdef FEATURE_BT_AG
    case VOC_BT_OP_AG_FORCE_DISABLE:
      if (bt_mode == VOC_BT_MODE_AG)
      {
        retval = voc_check_bt_ag(VOC_BT_OP_AG_FORCE_DISABLE);
      }
#endif /* FEATURE_BT_AG */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
      if (bt_mode == VOC_BT_MODE_A2DP)
      {
        retval = voc_check_bt_a2dp(VOC_BT_OP_A2DP_FORCE_DISABLE);
      }
#endif /* FEATURE_SBC_CODEC */
      break;    /* case VOC_BT_OP_FORCE_DISABLE */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    case VOC_BT_OP_A2DP_NORMAL:

#ifdef FEATURE_BT_AG
      if((codec_class == VOC_CODEC_CLASS_BLUETOOTH)
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
        )
      {
        /* The headset supports only the older profile and we are
        being asked to do a A2DP operation */
        retval = voc_check_bt_ag(VOC_BT_OP_FORCE_DISABLE);
      }
      else
#endif /* FEATURE_BT_AG */
      {
#if defined (FEATURE_BT_AG) && (defined (FEATURE_SBC_CODEC) || \
                                defined (FEATURE_SBC_DSP_CODEC))
        if ((voc_bt_mode == VOC_BT_MODE_AG) &&
                   (codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO))
        {
          /* Disable SCO link first */
          voc_check_bt_ag(VOC_BT_OP_AG_FORCE_DISABLE);
        }
#endif
        /* Call the A2DP function */
        retval = voc_check_bt_a2dp(operation);
      }
      break;  /* VOC_BT_OP_AG_NORMAL */
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_BT_AG
    case VOC_BT_OP_AG_NORMAL:
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
      if ((voc_bt_mode == VOC_BT_MODE_A2DP) &&
               (codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO))
      {
        /* Disable A2DP link first */
        voc_check_bt_a2dp(VOC_BT_OP_A2DP_FORCE_DISABLE);
      }
#endif
      retval = voc_check_bt_ag(operation);
      break;
#endif /* FEATURE_BT_AG*/

    case VOC_BT_OP_NORMAL:
    /* This means that the image could support both A2DP and SCO and we
     have been asked to choose one depending on the current device */
      switch (codec_class) {
#ifdef FEATURE_BT_AG
        case VOC_CODEC_CLASS_BLUETOOTH:
          retval = voc_check_bt_ag(VOC_BT_OP_AG_NORMAL);
          break;
#endif
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
        case VOC_CODEC_CLASS_BLUETOOTH_A2DP:
        case VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO:
           if (voc_bt_mode == VOC_BT_MODE_AG)
           {
             /* We are in AG mode, disconnect SCO link first */
             voc_check_bt_ag(VOC_BT_OP_AG_FORCE_DISABLE);
           }
          retval = voc_check_bt_a2dp(VOC_BT_OP_A2DP_NORMAL);
          break;
#endif
        default:
          switch (voc_bt_mode) {
#ifdef FEATURE_BT_AG
            case VOC_BT_MODE_AG:
              retval = voc_check_bt_ag(VOC_BT_OP_AG_NORMAL);
              break;
#endif
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
            case VOC_BT_MODE_A2DP:
              retval = voc_check_bt_a2dp(VOC_BT_OP_A2DP_NORMAL);
              break;
#endif
		        default:
              /* The current codec is not Bluetooth and Bluetooth is disabled so
			       there is really nothing to be done. */
             break;
          }
          break; /* default for switch (codec_class) */
      }
      break; /* Case VOC_BT_OP_NORMAL */

    default:
      break;  /* default */
  }
  return retval;
}

#endif /* FEATURE_BT_AG || FEATURE_SBC_CODEC */

#ifdef FEATURE_SBC_DSP_CODEC
/* <EJECT> */
/*===========================================================================

FUNCTION voc_sbc_enable

DESCRIPTION
  This function enables the DSP based SBC encoder if the DSP is in proper
  state to accept this command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_sbc_enable()
{
  qdsp_cmd_sbc_codec_config_type config;
  qdsp_cmd_status_type           status  = QDSP_CMD_FAILED;
  boolean                        dsp_ready = FALSE;

  voc_state_control_type           *state;
#ifdef FEATURE_AVS_I2SSBC
  voc_sample_rate_type             sample_rate   = VOC_SAMPLE_RATE_NONE;
  qdsp_cmd_i2ssbc_sample_rate_type v_sample_rate =
                                              QDSP_CMD_I2SSBC_SAMPLE_RATE_48000;
#endif /* FEATURE_AVS_I2SSBC */

#ifdef FEATURE_QSYNTH_COMMON
    dsp_ready |= check_interlock(VOC_UNLOCK_QSYNTH);
#endif

#ifdef FEATURE_QTUNES_COMMON
    dsp_ready |= check_interlock(VOC_UNLOCK_QTUNES);
#endif

/* A2DP support for for NB-AMR */
state = voc_state_get_state_data();
#ifdef FEATURE_GRAPH_AUDIO
#if defined(QDSP_IMAGE_QTV_DEFINED) || defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED)|| \
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED)
if (state->current_image == QDSP_IMAGE_QTV)
{
    dsp_ready |= check_interlock(VOC_UNLOCK_GRAPH);
}
#endif
#endif

#ifdef FEATURE_AVS_I2SSBC

  if (state->current_image == QDSP_IMAGE_I2SSBC) {
#ifdef FEATURE_GRAPH_AUDIO
    dsp_ready |= check_interlock(VOC_UNLOCK_GRAPH);
#endif
    sample_rate = voc_data_get_sample_rate();

    switch(sample_rate) {
      case VOC_SAMPLE_RATE_8000:
        v_sample_rate = QDSP_CMD_I2SSBC_SAMPLE_RATE_8000;
        break;

      case VOC_SAMPLE_RATE_11025:
        v_sample_rate = QDSP_CMD_I2SSBC_SAMPLE_RATE_11025;
        break;

      case VOC_SAMPLE_RATE_12000:
        v_sample_rate = QDSP_CMD_I2SSBC_SAMPLE_RATE_12000;
        break;

      case VOC_SAMPLE_RATE_16000:
        v_sample_rate = QDSP_CMD_I2SSBC_SAMPLE_RATE_16000;
        break;

      case VOC_SAMPLE_RATE_22050:
        v_sample_rate = QDSP_CMD_I2SSBC_SAMPLE_RATE_22050;
        break;

      case VOC_SAMPLE_RATE_24000:
        v_sample_rate = QDSP_CMD_I2SSBC_SAMPLE_RATE_24000;
        break;

      case VOC_SAMPLE_RATE_32000:
        v_sample_rate = QDSP_CMD_I2SSBC_SAMPLE_RATE_32000;
        break;

      case VOC_SAMPLE_RATE_44100:
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
      case VOC_SAMPLE_RATE_44000:
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
        v_sample_rate = QDSP_CMD_I2SSBC_SAMPLE_RATE_44100;
        break;

      case VOC_SAMPLE_RATE_48000:
        v_sample_rate = QDSP_CMD_I2SSBC_SAMPLE_RATE_48000;
        break;

      default:
        break;
    }
    qdsp_cmd_set_i2ssbc_sample_rate(v_sample_rate);
  }
#endif /* FEATURE_AVS_I2SSBC */

  if(dsp_ready) {
    voc_data_get_sbc_config(&config);
    qdsp_cmd_sbc_set_bitrate(voc_data_get_sbc_bitrate());
    status = qdsp_cmd_sbc_config(&config);
  }
  /* status == 0 => SUCCESS */

  return (!status);
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_sbc_reenable

DESCRIPTION
  This function re-enables the DSP based SBC encoder if the DSP is in proper
  state to accept this command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_sbc_reenable()
{
  if (VOC_BT_STATE_A2DP_ENABLED == voc_bt_state)
  {
    if (!voc_sbc_enable())
    {
      /* The DSP is not in a proper state or SBC config failed, in both case
      we need to do a retry */
      MSG_MED("Re-enabling SBC encoder in the DSP", 0,0,0);
      voc_bt_state = VOC_BT_STATE_A2DP_SBC_CONFIG_PENDING;
      (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
    }
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_sbc_disable

DESCRIPTION
  This function disables the DSP based SBC encoder

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void voc_sbc_disable()
{
  qdsp_cmd_status_type           status    = QDSP_CMD_SUCCESS;
  qdsp_cmd_sbc_codec_config_type config;
  boolean                        dsp_ready = FALSE;
#ifdef FEATURE_AVS_I2SSBC
  voc_state_control_type         *state;
#endif /* FEATURE_AVS_I2SSBC */

  config.param_buf = QDSP_CMD_SBC_PARAM_SBC_DIS_V;

#ifdef FEATURE_QSYNTH_COMMON
  dsp_ready |= check_interlock(VOC_UNLOCK_QSYNTH);
#endif

#ifdef FEATURE_QTUNES_COMMON
  dsp_ready |= check_interlock(VOC_UNLOCK_QTUNES);
#endif

#ifdef FEATURE_AVS_I2SSBC
  state = voc_state_get_state_data();
  if (state->current_image == QDSP_IMAGE_I2SSBC) {
#ifdef FEATURE_GRAPH_AUDIO
    dsp_ready |= check_interlock(VOC_UNLOCK_GRAPH);
#endif
  }
#endif /* FEATURE_AVS_I2SSBC */

  if(dsp_ready) {
    status = qdsp_cmd_sbc_config(&config);
    if (status != QDSP_CMD_SUCCESS) {
      MSG_ERROR("Error in disabling SBC encoder \n", 0,0,0);
    }
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_sbc_configure

DESCRIPTION
  This function sets the configuration parameters. SBC will be enabled and
  configured only if a DSP image that supports SBC gets loaded.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_sbc_configure(voc_sbc_config_params_type *params)
{
  voc_data_set_sbc_config(params);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_sbc_set_bitrate

DESCRIPTION
  This function sets the bitrate for the DSP based SBC encoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void voc_sbc_set_bitrate(uint32 bitrate)
{
  boolean                        dsp_ready = FALSE;

  voc_data_set_sbc_bitrate(bitrate);

#ifdef FEATURE_QSYNTH_COMMON
    dsp_ready |= check_interlock(VOC_UNLOCK_QSYNTH);
#endif

#ifdef FEATURE_QTUNES_COMMON
    dsp_ready |= check_interlock(VOC_UNLOCK_QTUNES);
#endif

  if(dsp_ready) {
    qdsp_cmd_sbc_set_bitrate(voc_data_get_sbc_bitrate());
  }
}
#endif /* FEATURE_SBC_DSP_CODEC */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/* <EJECT> */
/*===========================================================================

FUNCTION voc_bt_a2dp_link_not_disabled

DESCRIPTION
  Checks is the A2DP link is not disabled

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if not disabled; FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
voc_bt_state_enum_type voc_bt_a2dp_link_not_disabled( void )
{
  boolean status = FALSE;
  switch (voc_bt_state)
  {
    case VOC_BT_STATE_A2DP_LINK_PENDING:
    case VOC_BT_STATE_A2DP_CODEC_CONFIG_PENDING:
    case VOC_BT_STATE_A2DP_ENABLE_PENDING:
    case VOC_BT_STATE_A2DP_ENABLED:
    case VOC_BT_STATE_A2DP_SBC_CONFIG_PENDING:
    case VOC_BT_STATE_A2DP_DISABLE_PENDING:
      status = TRUE;
      break;

    default:
      break;
  }
  return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_is_bt_a2dp_pending

DESCRIPTION
  This function returns TRUE if we are waiting for A2DP link to be setup.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_is_bt_a2dp_pending(void)
{
  voc_codec_class_type       codec_class;
  boolean                    ret_val = FALSE;

  voc_data_get_codec_class(&codec_class);

  if((codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP)
     ||(codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO)){

    INTLOCK();
    if (VOC_BT_STATE_A2DP_LINK_PENDING == voc_bt_state)
    {
      ret_val = TRUE;
    }
    INTFREE();
  }
  return ret_val;
}
/* <EJECT> */
/*===========================================================================
FUNCTION voc_enable_bt_a2dp

DESCRIPTION
  This function enables the BT A2DP link

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_enable_bt_a2dp(void)
{
  audiosbc_config_type            sbc_config;
  audiosbc_sample_rate_type       sbc_sample_rate;
  audiosbcenc_data_type           *remote_params;
  voc_state_control_type          *state;
  qdsp_cmd_codec_config_type      *codec_config_ptr;

  sbc_sample_rate = voc_data_get_sbc_sample_rate();
  remote_params = voc_data_get_sbc_remote_params();
  state = voc_state_get_state_data();

  if (sbc_sample_rate == AUDIOSBC_SAMPLE_RATE_INVALID)
  {
    MSG_ERROR("Invalid SBC sample rate\n", 0,0,0);
  }
  else
  {
    audiosbc_configure(remote_params, voc_bt_app_id, sbc_sample_rate);
    audiosbc_get_config(&sbc_config);
    voc_sbc_data.octet0.sampling_frequency = sbc_config.sample_rate;
    voc_sbc_data.octet0.channel_mode |= sbc_config.mode;
    voc_sbc_data.octet1.block_length  = sbc_config.block_length;
    voc_sbc_data.octet1.subbands     |= sbc_config.num_subbands;
    voc_sbc_data.octet1.allocation_method  |= sbc_config.alloc;
    voc_sbc_data.min_bitpool = sbc_config.min_bitpool;
    voc_sbc_data.max_bitpool = sbc_config.max_bitpool;


#ifdef FEATURE_SPEAKER_PHONE
    voc_data_get_codec_config(state->current_image, VOC_CODEC_SPEAKER,
                              &codec_config_ptr);
#else
    voc_data_get_codec_config(state->current_image, VOC_CODEC_ON_CHIP_0,
                              &codec_config_ptr);
#endif
    (void) qdsp_cmd_codec_config(codec_config_ptr,\
			voc_data_get_default_frame_rate(state->current_image,\
						voc_data_get_current_codec()));

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

    state->codec_pending = FALSE;

    voc_bt_state = VOC_BT_STATE_A2DP_LINK_PENDING;
#ifdef FEATURE_SBC_DSP_CODEC
    /* Reset the retry counter */
    voc_dsp_sbc_config_retry_count = 0;
#endif
    voc_bt_mode  = VOC_BT_MODE_A2DP;
    MSG_HIGH("Starting A2DP connection \n",0,0,0);
    (void) bt_cmd_pf_a2dp_start(voc_bt_app_id, &voc_sbc_data);
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_bt_a2dp_remote_params

DESCRIPTION
  Configures the BT A2DP parameters received from headset

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_set_bt_a2dp_remote_params (audiosbcenc_data_type sbc_data)
{
  voc_data_set_sbc_remote_params(&sbc_data);
}

/* <EJECT>*/
/*===========================================================================

FUNCTION voc_bt_a2dp_suspend

DESCRIPTION
  Suspend the BT A2DP connection if not already suspended.
  Suspension will happen if a) the current image does not support A2DP,
  b) the codec changed or c) the song ended. In case c) audiosbc module
  will call this function.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_bt_a2dp_suspend(void)
{
  if((voc_bt_state != VOC_BT_STATE_DISABLED) &&
     (voc_bt_state != VOC_BT_STATE_A2DP_DISABLE_PENDING))
  {
    voc_bt_state = VOC_BT_STATE_A2DP_DISABLE_PENDING;
    /* Reinstate the old PP callback, this will disable PP if
       we were not doing PP before enabling SBC codec */
    voc_pp_ctl (voc_data_get_saved_pp_func(VOC_PP_TYPE_GENERIC),
                 VOC_PP_TYPE_GENERIC );
#ifdef FEATURE_SBC_DSP_CODEC
    voc_sbc_disable();
#endif

    voc_bt_state = VOC_BT_STATE_DISABLED;
    voc_bt_mode = VOC_BT_MODE_NONE;
    MSG_HIGH("Initiating BT A2DP Disconnect...\n", 0,0,0);
    bt_cmd_pf_a2dp_suspend(voc_bt_app_id);
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_complete_bt_a2dp

DESCRIPTION
  Completes BT A2DP connection by configuring codec.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_complete_bt_a2dp (void)
{
  qdsp_cmd_codec_config_type *codec_config_ptr;
  voc_state_control_type     *state;
  voc_codec_type             pcm_path;
  voc_codec_class_type       codec_class;

  voc_data_get_codec(&pcm_path);
  voc_data_get_codec_class(&codec_class);
  state = voc_state_get_state_data();

  /* If codec is BT configure codec */
  if((codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP)
     ||(codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO)){
    /* Get the codec config data */
    voc_data_get_codec_config(state->current_image, pcm_path,&codec_config_ptr);
  }else {
    /* PCM path changed */
    voc_check_bt_a2dp(VOC_BT_OP_A2DP_NORMAL);
    return;
  }

  switch (voc_bt_state) {
    case VOC_BT_STATE_DISABLED:
    case VOC_BT_STATE_A2DP_ENABLED:
    default:
      break;

    /* The following state means that we were waiting for the A2DP link to be
    enabled */
    case VOC_BT_STATE_A2DP_LINK_PENDING:
    /* The following state means that we tried to issue a codec config but
       failed.
     */
    case VOC_BT_STATE_A2DP_CODEC_CONFIG_PENDING:
    /* The following state means that we DSP was in RESET or IMAGE state when
       we tried to issue a codec config before. It is essentially same as the
       CODEC CONFIG pending state, but made separate for debugging purposes */
    case VOC_BT_STATE_A2DP_ENABLE_PENDING:

      if (check_interlock(VOC_UNLOCK_NOT_RESET)) {
        if (qdsp_cmd_codec_config(codec_config_ptr,\
         voc_data_get_default_frame_rate(state->current_image,\
					pcm_path)) != QDSP_CMD_SUCCESS) {
          MSG_HIGH("BT codec config failed: retrying...",0,0,0);
          voc_bt_state = VOC_BT_STATE_A2DP_CODEC_CONFIG_PENDING;
          voc_a2dp_codec_config_retry_count++;
          voc_set_bt_config_retry_timer(VOC_BT_RETRY_TIMER_A2DP_ENABLE);
          break;
        }

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

        state->codec_pending = FALSE;

        /* Resetting a2dp codec config retry count on successful codec config */
        voc_a2dp_codec_config_retry_count = 0;
        /* Resetting a2dp enable retry count on successful codec config */
        voc_a2dp_enable_retry_count = 0;
        /* If codec config succeeds we will fall through and issue SBC config */
      }
      else
      {
        MSG_HIGH("DSP Not in proper state for BT A2DP config, retrying...\n", 0,0,0);
        voc_a2dp_enable_retry_count++;
        voc_bt_state = VOC_BT_STATE_A2DP_ENABLE_PENDING;
        voc_set_bt_config_retry_timer(VOC_BT_RETRY_TIMER_A2DP_ENABLE);
        break;
      }
    case VOC_BT_STATE_A2DP_SBC_CONFIG_PENDING:
      /* Save the current PP cb function */
      voc_data_save_pp_func(voc_data_get_pp_func(), VOC_PP_TYPE_GENERIC);
      /* Enable the post processing interface */
#ifdef FEATURE_SBC_DSP_CODEC
      voc_pp_ctl (voc_data_get_saved_pp_func(VOC_PP_TYPE_DSP_SBC),
                  VOC_PP_TYPE_DSP_SBC );
      if (voc_sbc_enable())
      {
        MSG_HIGH("BT A2DP Connected: DSP SBC encoder configured Retries = %d", \
                 voc_dsp_sbc_config_retry_count, 0,0);
        voc_bt_state = VOC_BT_STATE_A2DP_ENABLED;
        voc_dsp_sbc_config_retry_count = 0;
        if (voc_bt_a2dp_link_enabled_cb != NULL)
        {
          voc_bt_a2dp_link_enabled_cb();
        }
      }
      else
      {
        MSG_HIGH("SBC DSP config failed retrying...\n", 0,0,0);
        voc_dsp_sbc_config_retry_count++;
        voc_bt_state = VOC_BT_STATE_A2DP_SBC_CONFIG_PENDING;
        voc_set_bt_config_retry_timer(VOC_BT_RETRY_TIMER_SBC_CONFIG);
      }
#else
      voc_pp_ctl (voc_data_get_saved_pp_func(VOC_PP_TYPE_SBC),VOC_PP_TYPE_SBC );
      MSG_HIGH("BT A2DP enabled \n", 0,0,0);
      voc_bt_state = VOC_BT_STATE_A2DP_ENABLED;
      if (voc_bt_a2dp_link_enabled_cb != NULL)
      {
        voc_bt_a2dp_link_enabled_cb();
      }
#endif
      (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
      break;
    }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_register_a2dp_enabled_cb

DESCRIPTION
  Registers a callback function with Vocoder. This callback function gets
  called when A2DP has been succesfully enabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_register_a2dp_enabled_cb(voc_bt_link_enabled_cb_type cb)
{
  if (cb != NULL)
  {
    voc_bt_a2dp_link_enabled_cb = cb;
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_check_and_enable_bt_a2dp

DESCRIPTION
  Enables the BT A2DP link if the A2DP is supported by FW for the current playback format. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_check_and_enable_bt_a2dp()
{
  voc_state_control_type     *state;
  state = voc_state_get_state_data();
  if((state->config == VOC_CAP_AMR)      || (state->config == VOC_CAP_IS733) 
           || (state->config == VOC_CAP_IS127)     
#ifdef FEATURE_AMR_WB_AUDIO_DEC
           || (state->config == VOC_CAP_AMR_WB)
#endif
           || (state->config == VOC_CAP_4GV_NB)  || (state->config == VOC_CAP_4GV_WB)
           || (state->config == VOC_CAP_GSM_EFR) || (state->config ==VOC_CAP_GSM_FR)
           || (state->config == VOC_CAP_GSM_HR))
        {        
#if defined(QDSP_vocoderPpFirstInitFlag)
#error code not present
#endif
        }
        else{
          voc_enable_bt_a2dp();
        }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_check_bt_a2dp

DESCRIPTION
  Enables/disables Bluetooth A2DP. This is the ONLY place that
  the vocoder is ever configured to the BT SBC codec.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if BT A2DP is current path and connected.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_check_bt_a2dp (
  voc_bt_op_type operation
) {
  voc_codec_class_type       codec_class;
  voc_state_control_type     *state;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  boolean                    bt_app_active;

  /* Retrieve codec */
  voc_data_get_codec_class(&codec_class);
  state = voc_state_get_state_data();

  bt_app_active = ((state->acquire_mask &
                    ((1 << VOC_ACQ_SND) | (1 << VOC_ACQ_VS)
#ifdef FEATURE_AVS_I2SSBC
                     | (1 << VOC_ACQ_I2SSBC)
#endif /* FEATURE_AVS_I2SSBC */
                    )) != 0);

  if((voc_bt_state != VOC_BT_STATE_DISABLED) &&
     (voc_bt_state != VOC_BT_STATE_A2DP_DISABLE_PENDING))
  {
    if((operation == VOC_BT_OP_A2DP_FORCE_DISABLE) ||
       !bt_app_active || ((codec_class != VOC_CODEC_CLASS_BLUETOOTH_A2DP) &&
                          (codec_class != VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO)))
    {
      /* If forced disable or codec not BT_SBC or sound released vocoder
      ** disable BT audio connection.
      */
      if((codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP ) ||
         (codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO)) {
        {
          /* Do not send codec config command if it in RESET state */
          if (check_interlock(VOC_UNLOCK_NOT_RESET))
          {

#ifdef FEATURE_SPEAKER_PHONE
            voc_data_get_codec_config(state->current_image, VOC_CODEC_SPEAKER,
                                      &codec_config_ptr);
#else
            voc_data_get_codec_config(state->current_image, VOC_CODEC_DEFAULT,
                                      &codec_config_ptr);
#endif
            (void) qdsp_cmd_codec_config(codec_config_ptr,\
                   voc_data_get_default_frame_rate(state->current_image,\
                                          voc_data_get_current_codec()));

            state->codec_pending = FALSE;

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

          }
        }
      }
      if ((voc_bt_mode == VOC_BT_MODE_A2DP) &&
          (voc_bt_state != VOC_BT_STATE_DISABLED) &&
		  (voc_bt_state != VOC_BT_STATE_A2DP_ENABLE_PENDING))
      {
        audiosbc_disable_encoder();
        voc_bt_a2dp_suspend();
      }
    }
    else if ((voc_bt_mode == VOC_BT_MODE_AG) &&
             (codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO) &&
             (operation == VOC_BT_OP_A2DP_NORMAL))
    {
      /* We were doing SCO link on A2DP SCO headset but we need to do A2DP */
      voc_check_bt_ag(VOC_BT_OP_AG_FORCE_DISABLE);

      if (voc_bt_state == VOC_BT_STATE_DISABLED) {
      voc_check_and_enable_bt_a2dp();
    }
    }
    else if ((VOC_BT_STATE_A2DP_SBC_CONFIG_PENDING == voc_bt_state)
              || (VOC_BT_STATE_A2DP_CODEC_CONFIG_PENDING == voc_bt_state))
    {
      voc_complete_bt_a2dp();
    }
  }
  else if((voc_bt_state == VOC_BT_STATE_DISABLED) &&
          (operation == VOC_BT_OP_A2DP_NORMAL))
  {
    if(((codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP) ||
        (codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO))&& bt_app_active)
    {
      /* If the state is DISABLED, codec is SBC, BT APP is active and
      if we are being asked to enable the connection lets go ahead and do it */
      voc_check_and_enable_bt_a2dp();
    }
  }

  return !(((codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP)
           || (codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO))
           && bt_app_active
           && (voc_bt_state != VOC_BT_STATE_A2DP_ENABLED));
}
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_AVS_I2SSBC

/*===========================================================================

FUNCTION voc_image_supports_i2ssbc

DESCRIPTION
  This function returns TRUE if an image supporting I2S SBC interface is
  currently loaded

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_image_supports_i2ssbc(void)
{
  voc_state_control_type     *state;
  boolean                    retval = FALSE;

  state = voc_state_get_state_data();

  switch (state->current_image) {
    case QDSP_IMAGE_I2SSBC:
      retval = TRUE;
    break;
    default:
    break;
  }
  return retval;
}

#endif /* FEATURE_AVS_I2SSBC */
#ifdef FEATURE_BT_AG

#if defined (FEATURE_AVS_BT_SCO_REWORK)
/*===========================================================================

FUNCTION voc_is_bt_ag_pending

DESCRIPTION
  This function returns TRUE if we are waiting for Ag link to be setup.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_is_bt_ag_pending(void)
{
  voc_codec_class_type       codec_class;
  boolean                    ret_val = FALSE;

  voc_data_get_codec_class(&codec_class);

  if(codec_class == VOC_CODEC_CLASS_BLUETOOTH){

    INTLOCK();
    if(!voc_sco_connected)
    {
      ret_val = TRUE;
    }
    INTFREE();
  }
  return ret_val;
}
/*=========================================================================
FUNCTION voc_set_sco_connected

DESCRIPTION
  Updates the information about the BT sco connection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void voc_set_sco_connected(void)
{
  MSG_MED("Sco connection evt received",0,0,0);
  voc_sco_connected = TRUE; /*Sco evt connection evt received*/
}
#endif /*AVS_BT_SCO_REWORK*/
/* <EJECT> */
/*===========================================================================

FUNCTION voc_register_ag_enabled_cb

DESCRIPTION
  Registers a callback function with Vocoder. This callback function gets
  called when AG has been succesfully enabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_register_ag_enabled_cb(voc_bt_link_enabled_cb_type cb)
{
  if (cb != NULL)
  {
    voc_bt_ag_link_enabled_cb = cb;
  }
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_sco_disconnected

DESCRIPTION
  Updates the information about the voc_sco connection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void voc_set_sco_disconnected(void)
{
  if(voc_bt_state == VOC_BT_STATE_AG_DISABLE_PENDING)
  {
    voc_bt_state = VOC_BT_STATE_DISABLED;
    voc_bt_mode  = VOC_BT_MODE_NONE;

    (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  } else {
#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if (voc_bt_mode == VOC_BT_MODE_AG) {
      /* Reset the states if it is BT AG connection mode */
      voc_sco_connected = FALSE;
      voc_bt_state = VOC_BT_STATE_DISABLED;
      voc_bt_mode  = VOC_BT_MODE_NONE;
    }
#endif
  }
  MSG_LOW( " voc_sco_connected= %d voc_bt_state= %d"
           " voc_bt_mode= %d \n", voc_sco_connected,
           voc_bt_state, voc_bt_mode);
}

/*===========================================================================

FUNCTION voc_set_a2dp_disconnected

DESCRIPTION
  Updates the information about the voc_a2dp connection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void voc_set_a2dp_disconnected(void)
{
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
// Yangdecai for BT
  if(voc_bt_state == VOC_BT_STATE_A2DP_DISABLE_PENDING)
  {
    voc_bt_state       = VOC_BT_STATE_DISABLED;
    voc_bt_mode        = VOC_BT_MODE_NONE;
    (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  } else {
    MSG_HIGH("Recvd A2DP Disconnect without AVS initiating it", 0,0,0);
  }
  #endif
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_complete_bt_ag

DESCRIPTION
  Completes BT AG connection by configuring codec.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_complete_bt_ag (void)
{
  qdsp_cmd_codec_config_type *codec_config_ptr;
  voc_state_control_type     *state;
  voc_codec_type             pcm_path;
  voc_codec_class_type       codec_class;
  boolean                    ret_val = TRUE;
  qdsp_cmd_status_type       result = QDSP_CMD_FAILED;

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  qdsp_mixer_mode_type          mixer_mode;
  audadsp_mixer_state_entity_state_type entity_state;
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifndef FEATURE_AVS_BT_SCO_REWORK
  voc_sco_connected = TRUE;
#endif

  voc_data_get_codec(&pcm_path);
  voc_data_get_codec_class(&codec_class);
  state = voc_state_get_state_data();

  MSG_HIGH("Rcvd SCO Connected event: codec = %d BT state = %d BT mode = %d",
             pcm_path, voc_bt_state, voc_bt_mode);

  switch (codec_class) {
    case VOC_CODEC_CLASS_BLUETOOTH:
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    case VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO:
#endif
      /* Codec is BT so get the codec config */
      voc_data_get_codec_config(state->current_image, pcm_path,
                                &codec_config_ptr);
      break;
    default:
      /* PCM path has changed, disconnect BT */
      voc_check_bt_ag(VOC_BT_OP_AG_NORMAL);
      return ret_val;
  }

  switch (voc_bt_state) {
    case VOC_BT_STATE_DISABLED:
    case VOC_BT_STATE_AG_DISABLE_PENDING:
    case VOC_BT_STATE_AG_ENABLED:
      break;
    default:
      break;

    case VOC_BT_STATE_AG_SCO_LINK_PENDING:
      if (check_interlock(VOC_UNLOCK_VOCODER))
      {
        /* If we are in vocoder state, we will not issue a codec config right
        away. This is to prevent DSP losing time synchronization (particularly
        if the we are doing a VT call). In voc_state_vocoder we will move
        the vocoder to IDLE state and come back here to issue the
        codec config in when the vocoder is in IDLE state */
        voc_bt_state = VOC_BT_STATE_AG_CODEC_CONFIG_PENDING;
        (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
        break;
      }
      /* Fall thru and issue codec config */
    case VOC_BT_STATE_AG_CODEC_CONFIG_PENDING:
    case VOC_BT_STATE_AG_ENABLE_PENDING:

      if (check_interlock(VOC_UNLOCK_NOT_RESET)) {
        switch(state->current_image) {
#if    (defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) && \
        defined(QDSP_IMAGE_VOC_COMBO_ICA_DEFINED)) || \
       (defined(FEATURE_AVS_INCALL_MIXER_CMX) && \
        defined(QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED))
#if     defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) && \
        defined(QDSP_IMAGE_VOC_COMBO_ICA_DEFINED)
        case QDSP_IMAGE_VOC_COMBO_ICA:
#endif
#if    defined(FEATURE_AVS_INCALL_MIXER_CMX) && \
       defined(QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED)
        case QDSP_IMAGE_VOC_COMBO_SYNTH:
#endif
          /* Mixer ADEC mode, need to bring pcm clock
           * down to 8KHz.
           */
          #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
          #else
          voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
          #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */

          mixer_mode = audadsp_mixer_state_get_mixer_mode(&entity_state);

          if (mixer_mode != QDSP_CMD_MIXER_MODE_NONE) {

            /* Issue mixer codec configuration command */
            result = qdsp_cmd_mixer_codec_config(mixer_mode, codec_config_ptr);
          }

#endif /* (defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) && \
          defined(QDSP_IMAGE_VOC_COMBO_ICA_DEFINED)) || \
          (defined(FEATURE_AVS_INCALL_MIXER_CMX) && \
          defined(QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED)) */

        default:
          result = qdsp_cmd_codec_config(codec_config_ptr, \
                   voc_data_get_default_frame_rate(state->current_image, \
                                                                 pcm_path));
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
        }

        if (result == QDSP_CMD_SUCCESS) {
          voc_bt_state = VOC_BT_STATE_AG_ENABLED;
          MSG_HIGH("BT SCO connected", 0,0,0);

          if (voc_bt_ag_link_enabled_cb != NULL)
          {
            voc_bt_ag_link_enabled_cb();
          }

          (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
        } else {
          MSG_HIGH("BT codec config failed: retrying...",0,0,0);
          voc_bt_state = VOC_BT_STATE_AG_CODEC_CONFIG_PENDING;
          voc_set_codec_retry_timer(TRUE);
          ret_val = FALSE;
        }
      }
      else
      {
        MSG_HIGH("DSP Not in proper state for BT AG config, retrying...\n",
                 0,0,0);
        voc_ag_enable_retry_count++;
        voc_bt_state = VOC_BT_STATE_AG_ENABLE_PENDING;
        voc_set_bt_config_retry_timer(VOC_BT_RETRY_TIMER_AG_ENABLE);
      }
      break;
  }
  return ret_val;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_check_bt_ag

DESCRIPTION
  Enables/disables Bluetooth Audio gateway. This is the ONLY place that
  the vocoder is ever configured to the BT codec.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if BT AG is current path and connected.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_check_bt_ag (
  voc_bt_op_type operation
) {
  voc_codec_class_type       codec_class;
  voc_state_control_type     *state;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  boolean                    bt_app_active;

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  qdsp_mixer_mode_type          mixer_mode;
  audadsp_mixer_state_entity_state_type entity_state;
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#if defined(FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif


  /* Retrieve codec */
  voc_data_get_codec_class(&codec_class);
  state = voc_state_get_state_data();


  bt_app_active = ((state->acquire_mask &
                    ((1 << VOC_ACQ_SND) | (1 << VOC_ACQ_VS)| \
                      (1 << VOC_ACQ_TEST))) != 0);

  if((voc_bt_state != VOC_BT_STATE_DISABLED) &&
     (voc_bt_state != VOC_BT_STATE_AG_DISABLE_PENDING)) {

    if((operation == VOC_BT_OP_AG_FORCE_DISABLE) ||
       (operation == VOC_BT_OP_FORCE_DISABLE)    ||
       !bt_app_active || ((codec_class != VOC_CODEC_CLASS_BLUETOOTH)
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
        && (codec_class != VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO)
#endif /* FEATURE_SBC_CODEC */
       )) {

      /* If forced disable or codec not BT or sound released vocoder
      ** disable BT audio connect.
      */
      if((codec_class == VOC_CODEC_CLASS_BLUETOOTH)
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
         || (codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO)
#endif /* FEATURE_SBC_CODEC */
        ) {
        /* Do not send codec config command if it in RESET state */
        if (check_interlock(VOC_UNLOCK_NOT_RESET))
        {
          voc_data_get_codec_config(state->current_image,
                                    VOC_CODEC_DEFAULT,
                                    &codec_config_ptr);

          /* During BT connection, ADIE is turned off, now it needs to
           * be turned on first */
          voc_state_bt_adie_wakeup(VOC_CODEC_DEFAULT);

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
          if (check_interlock(VOC_UNLOCK_MIXER_ADEC)) {

            #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
            #else
            voc_state_set_icodec_clk(VOC_SAMPLE_RATE_48000);
            #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
            mixer_mode = audadsp_mixer_state_get_mixer_mode(&entity_state);

            if (mixer_mode != QDSP_CMD_MIXER_MODE_NONE) {

              /* Issue mixer codec configuration command */
              (void) qdsp_cmd_mixer_codec_config(mixer_mode, codec_config_ptr);
            }
          } else
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
          {
            (void) qdsp_cmd_codec_config(codec_config_ptr,\
                      voc_data_get_default_frame_rate(state->current_image,\
                                                voc_data_get_current_codec()));

            state->codec_pending = FALSE;

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

          }
        }
      }

      MSMAUD_SEL_PCM_PATH_NORM();
      #ifndef MSMAUD_SCMM
      (void) clk_regime_enable_bt_pcm_clk( FALSE );
      #endif /* MSMAUD_SCMM */
#if defined(FEATURE_AVS_BT_SCO_REWORK)
   if(bt_func_ptr == NULL){
     MSG_HIGH("bt_func_ptr is null...returning",0,0,0);
     voc_bt_state = VOC_BT_STATE_AG_DISABLE_PENDING;
     return FALSE;
   }
   else {
     MSG_HIGH("SCO Rework: BT state changed to Disabled", 0,0,0);
     voc_bt_state = VOC_BT_STATE_DISABLED;
     voc_bt_mode  = VOC_BT_MODE_NONE;
   }
#else
  MSG_ERROR("SCO Rework: PLEASE ENABLE AVS_BT_SCO_REWORK",0,0,0);
  return FALSE;
#endif
     }
  } else if((voc_bt_state == VOC_BT_STATE_DISABLED) &&
            (operation == VOC_BT_OP_AG_NORMAL)) {

    if(((codec_class == VOC_CODEC_CLASS_BLUETOOTH)
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
       || (codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO)
#endif /* FEATURE_SBC_CODEC */
       ) && bt_app_active) {

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
      if ((codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO) &&
          (voc_bt_mode == VOC_BT_MODE_A2DP)) {
        /* We are doing A2DP, we need to bring down the A2DP link */
        MSG_HIGH("Disconnecting BT A2DP...\n", 0,0,0);
        voc_check_bt_a2dp(VOC_BT_OP_A2DP_FORCE_DISABLE);
      }
#endif /* FEATURE_SBC_CODEC */

       MSG_HIGH("Connecting BT SCO...",0,0,0);
#ifdef FEATURE_BT_SOC
       MSMAUD_SEL_PCM_PATH_NORM();
#else
       MSMAUD_SEL_PCM_PATH_MDSP();
#endif
       #ifndef MSMAUD_SCMM
       (void) clk_regime_enable_bt_pcm_clk( TRUE );
       #endif /* MSMAUD_SCMM */

#if defined(FEATURE_AVS_BT_SCO_REWORK)
  if(bt_func_ptr == NULL){
    MSG_HIGH("bt_func_ptr is null...returning",0,0,0);
    voc_bt_state = VOC_BT_STATE_AG_SCO_LINK_PENDING;
    return FALSE;
  }
  else{
    if (voc_sco_connected){
      MSG_HIGH("SCO Rework: BT State changed to CODEC_CFG_PENDING", 0,0,0);
      voc_bt_state = VOC_BT_STATE_AG_CODEC_CONFIG_PENDING;
      voc_complete_bt_ag();
    }
    else{
      MSG_HIGH("SCO Rework: BT State changed to SCO_LINK_PENDING", 0,0,0);
      voc_bt_state = VOC_BT_STATE_AG_SCO_LINK_PENDING;
    }
  }
#else
  MSG_ERROR("SCO Rework: PLEASE ENABLE AVS_BT_SCO_REWORK",0,0,0);
  return FALSE;
#endif
        voc_bt_mode = VOC_BT_MODE_AG;
      }
   }

   return !(((codec_class == VOC_CODEC_CLASS_BLUETOOTH)
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
           || (codec_class == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO)
#endif /* FEATURE_SBC_CODEC */
           ) && bt_app_active && (voc_bt_state != VOC_BT_STATE_AG_ENABLED));
}
#endif /* FEATURE_BT_AG */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_bt_state

DESCRIPTION
  This function get the status of the vocoder bluetooth AG.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_BT_STATE_DISABLED      - BT disabled
  VOC_BT_STATE_AG_PENDING    - BT AG enable pending
  VOC_BT_STATE_AG_ENABLED    - BT AG enabled
  VOC_BT_STATE_A2DP_LINK_PENDING  - BT A2DP enable pending
  VOC_BT_STATE_A2DP_ENABLED  - BT A2DP enabled

SIDE EFFECTS
  None.

===========================================================================*/
voc_bt_state_enum_type voc_get_bt_state( void )
{
  return (voc_bt_state);
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_bt_mode

DESCRIPTION
  This function get the BT mode that we are in.

DEPENDENCIES
  None.

RETURN VALUE

  VOC_BT_MODE_NONE
  VOC_BT_MODE_AG
  VOC_BT_MODE_A2DP

SIDE EFFECTS
  None.

===========================================================================*/
voc_bt_mode_type voc_get_bt_mode( void )
{
  return (voc_bt_mode);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_retry_bt_config

DESCRIPTION
  This function retries the BT configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_retry_bt_config ( void )
{
  switch (voc_bt_state) {
#ifdef FEATURE_SBC_DSP_CODEC
    case VOC_BT_STATE_A2DP_SBC_CONFIG_PENDING:
      if (voc_dsp_sbc_config_retry_count > VOC_DSP_SBC_CONFIG_RETRY_COUNT_MAX)
      {
        MSG_HIGH("BT A2DP Connected: DSP SBC encoder configured Retries = %d", \
                 voc_dsp_sbc_config_retry_count, 0,0);

        voc_dsp_sbc_config_retry_count = 0;
        voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
      }
      else if (voc_sbc_enable())
      {
        voc_bt_state = VOC_BT_STATE_A2DP_ENABLED;
        MSG_HIGH("DSP SBC encoder configured Retries = %d", voc_dsp_sbc_config_retry_count, 0,0);
        voc_dsp_sbc_config_retry_count = 0;
        if (voc_bt_a2dp_link_enabled_cb != NULL)
        {
          voc_bt_a2dp_link_enabled_cb();
        }
      }
      else
      {
        voc_bt_state = VOC_BT_STATE_A2DP_SBC_CONFIG_PENDING;
        voc_dsp_sbc_config_retry_count++;
        voc_set_bt_config_retry_timer(VOC_BT_RETRY_TIMER_SBC_CONFIG);
      }
      break;
#endif
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    case VOC_BT_STATE_A2DP_ENABLE_PENDING:
      if (voc_a2dp_enable_retry_count > VOC_A2DP_ENABLE_RETRY_COUNT_MAX)
      {
        MSG_ERROR("Error in configuring DSP for BT A2DP \n", 0,0,0);
        voc_a2dp_enable_retry_count = 0;
        voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
      }
      else
      {
        voc_complete_bt_a2dp();
      }
      break;
    case VOC_BT_STATE_A2DP_CODEC_CONFIG_PENDING:
      if (voc_a2dp_codec_config_retry_count > VOC_A2DP_CODEC_CONFIG_RETRY_COUNT_MAX)
      {
        MSG_ERROR("Error in configuring codec config for BT A2DP \n", 0,0,0);
        voc_a2dp_codec_config_retry_count = 0;
        voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
      }
      else 
      {
        voc_complete_bt_a2dp();
      }
      break;
#endif
#ifdef FEATURE_BT_AG
    case VOC_BT_STATE_AG_ENABLE_PENDING:
      if (voc_ag_enable_retry_count > VOC_AG_ENABLE_RETRY_COUNT_MAX)
      {
        MSG_ERROR("Error in configuring DSP for BT AG \n", 0,0,0);
        voc_ag_enable_retry_count = 0;
        voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
      }
      else
      {
        voc_complete_bt_ag();
      }
      break;
#endif
    default:
      break;
  }
}
#endif /* defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_dtmf_cmd

DESCRIPTION
  This function plays or stops a DTMF tone

DEPENDENCIES
  QDSP2 must be in IDLE, Vocoder, or DFM state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_dtmf_cmd
(
  uint16 tone1_hz,               /* first tone to generate [Hz]  */
  uint16 tone2_hz,               /* second tone to generate [Hz] */
  sint15 vol,                    /* volume to use                */
  uint16 duration_ms             /* duration of tones [mSec]     */
)
{
  qdsp_cmd_dtmf_type      dtmf;
  qdsp_cmd_status_type    result;
  voc_sample_rate_type    sample_rate = VOC_SAMPLE_RATE_8000;
  voc_state_control_type           *state;
#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON) || \
    defined(FEATURE_GRAPH_AUDIO)
  boolean                 dsp_ready = FALSE;
#endif

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif /*AVS_BT_SCO_REWORK*/


  if(check_interlock(VOC_UNLOCK_NOT_RESET))
  {

    if ((check_interlock(VOC_UNLOCK_VOC_NOT_RESET))
#ifdef QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED
        || (check_interlock(VOC_UNLOCK_GRAPH) &&
            (voc_state_get_state_data()->current_image ==
                                               QDSP_IMAGE_MP4D_AMR_EVRC_V13K))
#endif /* QDSP_IMAGE_MP4D_AMR_EVRC_AMR_DEFINED */
#if defined(QDSP_IMAGE_QTV_DEFINED) || defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED)|| \
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED)
#ifdef FEATURE_GRAPH_AUDIO
        || (check_interlock(VOC_UNLOCK_GRAPH) &&
            (voc_state_get_state_data()->current_image == QDSP_IMAGE_QTV))
#ifdef FEATURE_AVS_QCAM_COMBO_VOC_INTERFACE
#ifdef QDSP_IMAGE_QCAMCORDER_DEFINED
        || (check_interlock(VOC_UNLOCK_GRAPH) &&
            (voc_state_get_state_data()->current_image == QDSP_IMAGE_QCAMCORDER))
#endif /*QDSP_IMAGE_QCAMCORDER_DEFINED*/
#ifdef QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED
#error code not present
#endif /*QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED*/
#endif /* FEATURE_AVS_QCAM_COMBO_VOC_INTERFACE */
#endif /* FEATURE_GRAPH_AUDIO */
#endif /* QDSP_IMAGE_QTV_DEFINED ||QDSP_IMAGE_QTV_VOC_H264_DEFINED ||
          QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED*/
       ) {
      /* Set the DTMF path
      */
      (void) qdsp_cmd_set_dtmf_path(voc_data_get_dtmf_path_setting());
    }
#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)

    if (bt_func_ptr != NULL)
    {
      if (duration_ms){
       if(!voc_bt_playback_dtmf_started)
       {
         MSG_ERROR("Calling BT callback with playback DTMF start", 0,0,0);
         bt_func_ptr (VOC_PLAYBACK_DTMF_START);
         voc_bt_playback_dtmf_started =  TRUE;
       }
      }
    }
#endif /*AVS_BT_SCO_REWORK*/


    MSG_LOW( "voc_dtmf_cmd(%d, %d, vol, %d)",tone1_hz, tone2_hz, duration_ms);

#if defined(FEATURE_QSYNTH_COMMON)      || \
    defined(FEATURE_QTUNES_COMMON)      || \
    defined(FEATURE_GRAPH_AUDIO)        || \
    defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)

#ifdef FEATURE_QSYNTH_COMMON
    dsp_ready |= check_interlock(VOC_UNLOCK_QSYNTH);
#endif

#ifdef FEATURE_QTUNES_COMMON
    dsp_ready |= check_interlock(VOC_UNLOCK_QTUNES);
#endif

#ifdef FEATURE_GRAPH_AUDIO
    dsp_ready |= check_interlock(VOC_UNLOCK_GRAPH);
#endif

#ifdef FEATURE_AVS_INCALL_ADEC_PB
    dsp_ready |= check_interlock(VOC_UNLOCK_VOCODER_ADEC);
#endif

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
    dsp_ready |= check_interlock(VOC_UNLOCK_MIXER_ADEC);
#endif

    if (dsp_ready)
    {
      sample_rate   = voc_data_get_sample_rate();
      dtmf.duration = voc_data_get_dtmf_duration( duration_ms );
    }
    else
#endif /* FEATURE_QSYNTH_COMMON || FEATURE_QTUNES_COMMON)     ||
          FEATURE_GRAPH_AUDIO   || FEATURE_AVS_INCALL_ADEC_PB ||
          FEATURE_AVS_INCALL_MIXER_ADEC_PB	*/
    {
      sample_rate   = VOC_SAMPLE_RATE_8000;
      dtmf.duration = duration_ms / 5;
    }
    state = voc_state_get_state_data();
#if defined(QDSP_IMAGE_4GV_WB_DEFINED)
    if (state->current_image == QDSP_IMAGE_4GV_WB)
    {
      dtmf.dtmf_hi  = tone1_hz ;
      dtmf.dtmf_low = tone2_hz ;
    }
    else
#endif /* defined(QDSP_IMAGE_4GV_WB_DEFINED) */
#if defined(QDSP_IMAGE_AMR_WB_DEFINED)
    if (state->current_image == QDSP_IMAGE_AMR_WB)
    {
      dtmf.dtmf_hi  = tone1_hz ;
      dtmf.dtmf_low = tone2_hz ;
    }
    else
#endif /* defined(QDSP_IMAGE_AMR_WB_DEFINED) */

    switch( state->current_image )
    {
#ifdef QDSP_IMAGE_VOC_COMBO_ICA_DEFINED
      case QDSP_IMAGE_VOC_COMBO_ICA:
#endif /* QDSP_IMAGE_VOC_COMBO_ICA_DEFINED */
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
      case QDSP_IMAGE_VOC_COMBO_SYNTH:
#endif /* QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */
      {
        dtmf.dtmf_hi  = (tone1_hz == 0) ?
                    0 : vocsup_compute_freq(tone1_hz, VOC_SAMPLE_RATE_8000);
        dtmf.dtmf_low = (tone2_hz == 0) ?
                    0 : vocsup_compute_freq(tone2_hz, VOC_SAMPLE_RATE_8000);
        dtmf.duration = duration_ms / 2;
        break;
      }
      default:
      {
        dtmf.dtmf_hi  = (tone1_hz == 0) ?
                               0 : vocsup_compute_freq(tone1_hz, sample_rate);
        dtmf.dtmf_low = (tone2_hz == 0) ?
                               0 : vocsup_compute_freq(tone2_hz, sample_rate);
        break;
      }
    } /* switch */

    dtmf.tx_gain  = voc_data_get_dtmf_tx_gain();
    dtmf.rx_gain  = voc_data_get_dtmf_rx_gain( vol );

    /* Now send the command to the QDSP2 */
    result = qdsp_cmd_dtmf(&dtmf);

    if (result == QDSP_CMD_SUCCESS) {
      return(VOC_DONE_S);
    } else {
      MSG_HIGH("QDSP2 Vocoder DTMF generation error = %d", result, 0, 0);
      return(VOC_DONE_S);
    }
  } else {
    MSG_HIGH("QDSP2 Vocoder DTMF Wrong State!", 0, 0, 0);
    return(VOC_DONE_S);
  }

} /* voc_dtmf_cmd */

/* <EJECT> */
#if defined(FEATURE_QTUNES_COMMON) || defined(FEATURE_MIDI) || \
    defined(FEATURE_MIDI_OUT) || defined(FEATURE_QSYNTH_ADPCM)
static sint15 voc_mastr_vol;

/*===========================================================================

FUNCTION voc_master_vol

DESCRIPTION
  This function sets previously set master volume level for audio format playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_master_vol(void)
{
  uint16 vol = voc_data_get_mm_rx_gain(voc_mastr_vol);
  (void) qdsp_cmd_master_volume(vol);
}
/*===========================================================================

FUNCTION voc_master_volume

DESCRIPTION
  This function selects a new master volume level for audio format playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_master_volume (
  sint15 volume
)
{
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  boolean check_state = FALSE;

  /* save the volume level */
  voc_mastr_vol = volume;

#ifdef FEATURE_QSYNTH_COMMON
  check_state = check_state || check_interlock(VOC_UNLOCK_QSYNTH);
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QTUNES_COMMON
  check_state = check_state || check_interlock(VOC_UNLOCK_QTUNES);
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_AVS_INCALL_ADEC_PB
  check_state = check_state || check_interlock(VOC_UNLOCK_VOCODER_ADEC);
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  check_state = check_state || check_interlock(VOC_UNLOCK_MIXER_ADEC);
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

  if(check_state) {
    uint16 vol = voc_data_get_mm_rx_gain(volume);
    (void) qdsp_cmd_master_volume(vol);
    ret_val = VOC_DONE_S;
  }

  return(ret_val);
}
#endif /* FEATURE_QTUNES_COMMON || FEATURE_MIDI || FEATURE_MIDI_OUT ||
          FEATURE_QSYNTH_ADPCM */

#if defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT)
/*===========================================================================

FUNCTION voc_midi_cmd

DESCRIPTION
  This function plays or stops a midi sequence.

DEPENDENCIES
  QDSP2 must be in Midi state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_midi_cmd (
  voc_midi_cmd_type *cmd,
  uint32            num_cmd,
  sint15            volume
) {
  qdsp_cmd_status_type   result;
  uint16                 vol = 0;
  voc_state_control_type *state = voc_state_get_state_data();

  if((cmd != NULL) && (num_cmd != 0)) {
    if ((check_interlock(VOC_UNLOCK_QSYNTH)) &&
        ((voc_check_pa_on_status() == TRUE)) &&
        (state->hybrid_mode == state->hybrid_mode_pending) &&
        (state->qsynth_vol_mode == state->qsynth_vol_mode_pending) &&
        (state->acquire_mask != 0)
#if defined(FEATURE_QSYNTH_ADPCM) && !(defined(FEATURE_GASYNTH_INTERFACE))
        && (state->adec_mode == state->adec_mode_pending)
#endif /* FEATURE_QSYNTH_ADPCM */
        && (state->module_status != VOC_STATE_MOD_STATUS_DISABLE_PENDING)
       ) {
      /* Now send the command to the QDSP2 */
      result = qdsp_cmd_midi((qdsp_cmd_midi_cmd_type *) cmd, num_cmd, vol);

      if (result == QDSP_CMD_SUCCESS) {
        return(VOC_DONE_S);
      } else {
        MSG_HIGH("QDSP2 Vocoder Midi generation error = %d", result, 0, 0);
        return(VOC_CMD_FAILED_S);
      }
    } else {
      MSG_LOW("Not in Midi State.", 0, 0, 0);
      return(VOC_BADFMT_S);
    }
  }

  return(VOC_DONE_S);
} /* voc_midi_cmd */
#endif /* defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT) */

/* <EJECT> */
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION voc_synth_mvol

DESCRIPTION
  This function sets the synth master volume.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_synth_mvol (
  uint16 vol
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if(check_interlock(VOC_UNLOCK_QSYNTH)) {
    if(qdsp_cmd_synth_mvol(vol) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}
#endif /* FEATURE_MIDI */

/* <EJECT> */
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/*===========================================================================

FUNCTION voc_adec_isr

DESCRIPTION
  This function is called from the QDSP to signal a request for more
  audio decoder data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_adec_isr ( )
{
  if(voc_adec_cb != NULL) {
    /* Tell sound/audfmt to return the next set of data */
    voc_adec_cb((void *)voc_adec_client_data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_init

DESCRIPTION
  This function configures data functions and activates the
  audio decoder playback process.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_adec_init(
  voc_adec_cb_func_type cb_func,
  const void            *client_data,
  voc_codec_update_cb_func_type cb_func2
) {

  /* Register callback function for qdsp originated data request interrupts. */
  voc_adec_cb = cb_func; /* Store it locally so QDSP doesn't need to */
  voc_adec_client_data = client_data;

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
  MSG_LOW("voc_adec_init cb_func2 = %p", cb_func2,0,0);
  voc_codec_update_cb      = cb_func2;

  if ( cb_func2 == NULL ) {
    /* reset qdsp callback and value */
    qdsp_cmd_set_codec_update_int_cb( NULL );
  }
#endif

  return(VOC_DONE_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_move_data

DESCRIPTION
  This function moves blocks of data to the qdsp audio decoder buffers.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_adec_move_data (
  voc_adec_mode_type adec_type,
  voc_adec_cmd_type  *adec_cmd
)
{
  qdsp_cmd_status_type  result;
#ifdef FEATURE_QTUNES_COMMON
  uint16 error = 0;
  uint8 threshold = 0;
#endif /* FEATURE_QTUNES_COMMON */

#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON) || \
    defined(FEATURE_WAVE_PB)
  boolean do_write = FALSE;

  if(voc_adec_cb != NULL) {
    switch (adec_type) {
#ifdef FEATURE_QSYNTH_COMMON
      case VOC_ADEC_MODE_QSYNTH:
        do_write = check_interlock(VOC_UNLOCK_QSYNTH);
        break;
#endif
#ifdef FEATURE_QTUNES_COMMON
      case VOC_ADEC_MODE_QTUNES:
        do_write = check_interlock(VOC_UNLOCK_QTUNES) &&
                   (voc_adec_flush_retry_in_progress == FALSE);
#ifdef FEATURE_AVS_INCALL_ADEC_PB
        do_write = do_write || (check_interlock(VOC_UNLOCK_VOCODER_ADEC) &&
                                (voc_adec_flush_retry_in_progress == FALSE));
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
        do_write = do_write || (check_interlock(VOC_UNLOCK_MIXER_ADEC) &&
                                (voc_adec_flush_retry_in_progress == FALSE));
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
        break;
#endif
#ifdef FEATURE_ADPCM
      case VOC_ADEC_MODE_ADPCM:
#ifdef FEATURE_GRAPH_ADPCM
        do_write = check_interlock(VOC_UNLOCK_GRAPH_AUDIO);
#endif /* FEATURE_GRAPH_ADPCM */
#ifdef FEATURE_QSYNTH_COMMON
        do_write = do_write || check_interlock(VOC_UNLOCK_QSYNTH);
#endif /* FEATURE_QSYNTH_COMMON */
#ifdef FEATURE_VOC_ADPCM
        do_write = do_write || check_interlock(VOC_UNLOCK_VOCODER_ADPCM);
#endif /* FEATURE_VOC_ADPCM */
        break;
#endif /* FEATURE_ADPCM */

#if defined(FEATURE_PCM) && defined(QDSP_IMAGE_SYNTH_DEFINED) && \
    defined(FEATURE_QSYNTH_COMMON)
      case VOC_ADEC_MODE_PCM:
        if (check_interlock(VOC_UNLOCK_QSYNTH) &&
            (voc_state_get_state_data()->current_image == QDSP_IMAGE_SYNTH
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif
		)) {
          do_write = TRUE;
        }
        break;
#endif

#ifdef FEATURE_VOC_G711
      case VOC_ADEC_MODE_WAV_MULAW:
      case VOC_ADEC_MODE_WAV_ALAW:
        if (check_interlock(VOC_UNLOCK_VOCODER) &&
            voc_state_get_state_data()->module_reqd == QDSP_MODULE_G711)
        {
          do_write = TRUE;
        }
        break;
#endif /* FEATURE_VOC_G711 */

      default:
        MSG_ERROR("Unknown ADEC Mode %d detected",adec_type,0,0);
        return(VOC_CMD_FAILED_S);
    }

    if (do_write) {
#ifdef FEATURE_QTUNES_COMMON
      /* Check for qdsp ADEC errors before transfer, clearing errors
      ** every time
      */
      error = qdsp_cmd_adec_error(TRUE);

      /* Check for any error codes */
      if(error == 0) {
        voc_adec_failed = 0;
      } else {
        /* Allow a one-time retry in case there was a simple frame glitch... */
        MSG_ERROR("ADEC Error 0x%x detected",error,0,0);
        voc_adec_failed++;
#ifdef FEATURE_AAC
        if(voc_state_get_state_data()->config == VOC_CAP_QTUNES_AAC){
          threshold = MAX_AAC_DSP_FAILURES;
        } else
#endif /* FEATURE_AAC */
        {
          threshold = MAX_DSP_FAILURES;
        }
        if(voc_adec_failed >= threshold) {
          voc_adec_failed = 0;
          return(VOC_DSP_DATA_ERR_S);
        }
      }
#endif /* FEATURE_QTUNES_COMMON */

      /* Send along the data */
      result = qdsp_cmd_adec((qdsp_cmd_adec_cmd_type *) adec_cmd);
      if(result == QDSP_CMD_FAILED) {
        return(VOC_CMD_FAILED_S);
      }
    }
  }
#endif /* FEATURE_QSYNTH_COMMON || FEATURE_QTUNES_COMMON */
  return(VOC_DONE_S);
}

/* <EJECT> */
#ifdef FEATURE_GASYNTH_INTERFACE
/*==========================================================================

FUNCTION voc_qsynth_adec_cmd_retry

DESCRIPTION
  This function attempts to write adec command to DSP again because the adec
  command was queued up due to DSP in lock state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_qsynth_adec_cmd_retry() {
  qdsp_cmd_qsynth_adec_cmd_retry();
}
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_reset

DESCRIPTION
  This function causes the vocoder to flush all internal buffers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_adec_reset( uint32 reset_mask )
{
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
  if(check_interlock(VOC_UNLOCK_QSYNTH)) {
    qdsp_cmd_adec_reset(reset_mask);
  }
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_pause

DESCRIPTION
  This function causes the vocoder to put the qdsp into a pause state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_adec_pause (
  boolean pause     /* TRUE if output should be paused */
) {
  boolean check_state = FALSE;

#ifdef FEATURE_QSYNTH_COMMON
  check_state = check_state || check_interlock(VOC_UNLOCK_QSYNTH);
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QTUNES_COMMON
  check_state = check_state || check_interlock(VOC_UNLOCK_QTUNES);
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_GRAPH_ADPCM
  check_state = check_state || check_interlock(VOC_UNLOCK_GRAPH_AUDIO);
#endif /* FEATURE_GRAPH_ADPCM */

#ifdef FEATURE_VOC_ADPCM
  check_state = check_state || check_interlock(VOC_UNLOCK_VOCODER_ADPCM);
#endif /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_VOC_G711
  check_state = check_state || check_interlock(VOC_UNLOCK_VOCODER);
#endif /* FEATURE_VOC_G711 */

#ifdef FEATURE_AVS_INCALL_ADEC_PB
  check_state = check_state || check_interlock(VOC_UNLOCK_VOCODER_ADEC);
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  check_state = check_state || check_interlock(VOC_UNLOCK_MIXER_ADEC);
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

  if(check_state) {
    (void) qdsp_cmd_adec_pause(pause);
  }

  return(VOC_DONE_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_int_pending

DESCRIPTION
  This function checks for a pending ADEC interrupt.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an interrupt is pending.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_adec_int_pending()
{
  boolean check_state = FALSE;

#ifdef FEATURE_QSYNTH_COMMON
  check_state = check_state || check_interlock(VOC_UNLOCK_QSYNTH);
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QTUNES_COMMON
  check_state = check_state || check_interlock(VOC_UNLOCK_QTUNES);
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_GRAPH_ADPCM
  check_state = check_state || check_interlock(VOC_UNLOCK_GRAPH_AUDIO);
#endif /* FEATURE_GRAPH_ADPCM */

#ifdef FEATURE_VOC_ADPCM
  check_state = check_state || check_interlock(VOC_UNLOCK_VOCODER_ADPCM);
#endif /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_AVS_INCALL_ADEC_PB
  check_state = check_state || check_interlock(VOC_UNLOCK_VOCODER_ADEC);
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  check_state = check_state || check_interlock(VOC_UNLOCK_MIXER_ADEC);
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifdef FEATURE_VOC_G711
  check_state = check_state || check_interlock(VOC_UNLOCK_VOCODER);
#endif /* FEATURE_VOC_G711 */

  if(check_state) {
    return(qdsp_cmd_adec_int_pending());
  } else {
    return(FALSE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_master_pan

DESCRIPTION
  This function sets the master stereo pan setting for audio format playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_master_pan (
  uint8 pan
)
{
  voc_status_type ret_val = VOC_CMD_FAILED_S;
  boolean         check_state = FALSE;

#ifdef FEATURE_QSYNTH_COMMON
  check_state = check_state || check_interlock(VOC_UNLOCK_QSYNTH);
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QTUNES_COMMON
  check_state = check_state || check_interlock(VOC_UNLOCK_QTUNES);
#endif /* FEATURE_QTUNES_COMMON */

  if(check_state) {
    if(qdsp_cmd_master_pan((sint7) pan - 64) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

/* <EJECT> */
#ifdef FEATURE_WAVE_PB
/*===========================================================================

FUNCTION voc_set_adpcm_mode

DESCRIPTION
  This function sets the ADPCM mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_set_adpcm_mode (
  boolean  mode
) {
  voc_state_control_type     *state;

  state = voc_state_get_state_data();
  state->adpcm_mode_enabled = mode;
#ifdef FEATURE_GRAPH_ADPCM
  state->adpcm_mode_flag    = TRUE;
#endif /* FEATURE_GRAPH_ADPCM */

#ifdef FEATURE_VOC_ADPCM
  if (!mode) {
    state->adpcm_voc_config   = mode;
  }
#endif /* FEATURE_VOC_ADPCM */

  /* Take care of any state changes
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
}

#ifdef FEATURE_VOC_G711
/*===========================================================================

FUNCTION voc_set_wav_mode

DESCRIPTION
  This function sets the WAV mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_set_wav_mode (
  voc_adpcm_mode_enum_type  mode
) {
  voc_state_control_type     *state;

  state = voc_state_get_state_data();
  if(mode != VOC_ADPCM_INVALID_MODE) {
    state->adpcm_mode_enabled = TRUE;
  } else {
    state->adpcm_mode_enabled = FALSE;
#ifdef FEATURE_VOC_ADPCM
    state->adpcm_voc_config   = FALSE;
#endif /* FEATURE_VOC_ADPCM */
  }
#ifdef FEATURE_GRAPH_ADPCM
  state->adpcm_mode_flag    = TRUE;
#endif /* FEATURE_GRAPH_ADPCM */
  state->wav_config = mode;

  /* Take care of any state changes
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
}
#endif /* FEATURE_VOC_G711 */
#endif /* FEATURE_WAVE_PB */

/* <EJECT> */
#ifdef FEATURE_GASYNTH_INTERFACE
/*===========================================================================

FUNCTION voc_adec_voc_start

DESCRIPTION
  This function starts the audio decoder for QCP or AMR playback

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/
voc_status_type voc_adec_voc_start (
  uint32                        channel,
  voc_adec_voc_enum_type        voc_type,
  voc_adec_start_mode_enum_type start_mode
) {
  voc_status_type retval = VOC_CMD_FAILED_S;
  qdsp_cmd_adec_start_mode_enum_type qdsp_cmd_mode;
  qdsp_cmd_adec_voc_enum_type        qdsp_cmd_voc_type = QDSP_CMD_ADEC_VOC_QCP;

  if (check_interlock(VOC_UNLOCK_QSYNTH)) {
    if(start_mode == VOC_ADEC_START_MODE_DELAYED) {
      qdsp_cmd_mode = QDSP_CMD_ADEC_START_MODE_DELAYED;
    } else {
      qdsp_cmd_mode = QDSP_CMD_ADEC_START_MODE_NORMAL;
    }
    if (voc_type == VOC_ADEC_VOC_AMR) {
      qdsp_cmd_voc_type = QDSP_CMD_ADEC_VOC_AMR;
    }
    if (qdsp_cmd_qsynth_voc_adec_start(channel,
				       qdsp_cmd_voc_type,
				       qdsp_cmd_mode) ==
        QDSP_CMD_SUCCESS) {
      retval = VOC_DONE_S;
    }
  } else {
    retval = VOC_CMD_BUSY_S;
  }
  return retval;
}

/*===========================================================================

FUNCTION voc_adec_data_consumed

DESCRIPTION
  This function detects decoder has finished consuming data.

DEPENDENCIES
  This command is only valid when the DSP is in QSYNTH state.

RETURN VALUE
  TRUE - DSP has done consuming data.
  FALSE - DSP has not done consuming data.

SIDE EFFECTS
  NONE

===========================================================================*/
boolean voc_adec_data_consumed(
  uint8 channel
) {
  boolean ret_val = FALSE;
  if (check_interlock(VOC_UNLOCK_QSYNTH)) {
    ret_val = qdsp_cmd_adec_data_consumed(channel);
  }

  return ret_val;
}
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION voc_adec_adpcm_start

DESCRIPTION
  This function will initialize a QSynth adpcm channel.

DEPENDENCIES
  FEATURE_QSYNTH_ADPCM.

RETURN VALUE
  voc_status_type
  - VOC_DONE_S: command was successful.
  - VOC_CMD_FAILED_S: command failed.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_adec_adpcm_start (
  uint32                        channel,
  voc_adpcm_play_enum_type      play_type,
  voc_adpcm_spec_type           *spec,
  voc_adec_start_mode_enum_type start_mode
) {
  voc_status_type        ret_val = VOC_CMD_FAILED_S;
#if defined(FEATURE_QSYNTH_ADPCM) || defined(FEATURE_GRAPH_ADPCM) \
  || defined(FEATURE_VOC_ADPCM) || defined(FEATURE_VOC_G711)
  voc_state_control_type *state;
#endif
#ifdef FEATURE_GRAPH_ADPCM
  boolean                valid_graph;
#endif /* FEATURE_GRAPH_ADPCM */

#ifdef FEATURE_VOC_ADPCM
  boolean                valid_voc_adpcm = FALSE;
#endif /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_VOC_G711
  boolean                valid_g711_wav = FALSE;
#endif /* FEATURE_VOC_G711 */

#ifdef FEATURE_QSYNTH_ADPCM
  qdsp_cmd_qsynth_adpcm_cmd_type     adpcm_cmd;
  qdsp_cmd_adec_start_mode_enum_type qdsp_cmd_mode;
  boolean                            valid_synth = FALSE;

  state = voc_state_get_state_data();

  switch(spec->type) {
    case VOC_ADPCM_ADPCM_MODE:
      valid_synth = (VOC_QSYNTH_ADPCM_VALID_CHANNELS(spec->num_channels) &&
                     VOC_QSYNTH_ADPCM_VALID_SAMPLE_RATE(spec->sampling_rate) &&
                     VOC_QSYNTH_ADPCM_VALID_BITS_PER_SAMPLE(
                                                       spec->bits_per_sample));
      break;

    case VOC_ADPCM_YADPCM_MODE:
      valid_synth = (VOC_QSYNTH_YADPCM_VALID_CHANNELS(spec->num_channels) &&
                     VOC_QSYNTH_YADPCM_VALID_SAMPLE_RATE(spec->sampling_rate) &&
                     VOC_QSYNTH_ADPCM_VALID_BITS_PER_SAMPLE(
                                                       spec->bits_per_sample));
      break;

    case VOC_ADPCM_SIGNED_PCM_MODE:
    case VOC_ADPCM_UNSIGNED_PCM_MODE:
#ifdef FEATURE_GASYNTH_INTERFACE
      valid_synth = (VOC_QSYNTH_PCM_VALID_CHANNELS(spec->num_channels) &&
                     VOC_QSYNTH_PCM_VALID_SAMPLE_RATE(spec->sampling_rate) &&
                     VOC_QSYNTH_PCM_VALID_BITS_PER_SAMPLE(
                                                     spec->bits_per_sample));
#else
      if(state->adec_mode_pending == VOC_QSYNTH_ADEC_MODE_YADPCM) {
        valid_synth = (VOC_QSYNTH_YADPCM_VALID_CHANNELS(spec->num_channels) &&
                       VOC_QSYNTH_YADPCM_VALID_SAMPLE_RATE(
                                                       spec->sampling_rate) &&
                       VOC_QSYNTH_YADPCM_PCM_VALID_BITS_PER_SAMPLE(
                                                       spec->bits_per_sample));
#ifdef FEATURE_QSYNTH_PCM
      } else if(state->adec_mode_pending == VOC_QSYNTH_ADEC_MODE_PCM) {
        valid_synth = (VOC_QSYNTH_PCM_VALID_CHANNELS(spec->num_channels) &&
                       VOC_QSYNTH_PCM_VALID_SAMPLE_RATE(spec->sampling_rate) &&
                       VOC_QSYNTH_PCM_VALID_BITS_PER_SAMPLE(
                                                       spec->bits_per_sample));
#endif /* FEATURE_QSYNTH_PCM */
      }
#endif /* FEATURE_GASYNTH_INTERFACE */
      break;
  }

  if(valid_synth) {
    if(check_interlock(VOC_UNLOCK_QSYNTH)
#ifdef FEATURE_WAVE_PB
       && ((play_type == VOC_ADPCM_PLAY_AUDFADEC) ||
           (state->adpcm_mode_enabled))
#endif /* FEATURE_WAVE_PB */
      ) {
      if(start_mode == VOC_ADEC_START_MODE_DELAYED) {
        qdsp_cmd_mode = QDSP_CMD_ADEC_START_MODE_DELAYED;
      } else {
        qdsp_cmd_mode = QDSP_CMD_ADEC_START_MODE_NORMAL;
      }

      adpcm_cmd.block_length    = spec->block_length;
      adpcm_cmd.sampling_rate   = spec->sampling_rate;
      adpcm_cmd.num_channels    = spec->num_channels;
      adpcm_cmd.bits_per_sample = spec->bits_per_sample;


#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
#ifdef FEATURE_VOC_ADPCM
  valid_voc_adpcm =
      (VOC_GRAPH_ADPCM_VALID_CHANNELS(spec->num_channels)           &&
       VOC_GRAPH_ADPCM_VALID_BITS_PER_SAMPLE(spec->bits_per_sample) &&
       VOC_VOC_32K_ADPCM_VALID_SAMPLE_RATE(spec->sampling_rate)     &&
       VOC_VOC_32K_ADPCM_VALID_BLOCK_SIZE(spec->block_length));
#endif 

      /* Do not send the 0x320 command from the SND task in case of ADPCM
         playback as shutter sound in Audio Camera Concurrency Scenario
         where-in DTMF support has been provided in the Preview mode.There
         is a corner case where the 0x320 command is sent to the DSP before
         the FW command to swap the resident MP3 module and load the ADPCM
         module is sent from QDSP task. */
      if ( voc_get_status() == VOC_INFORM_CAM_AUDIO_ACTIVE 
#ifdef  FEATURE_VOC_ADPCM        
        && valid_voc_adpcm
#endif /*  FEATURE_VOC_ADPCM */       
        )
         return (VOC_DONE_S);
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY*/

      /* The check is required to prevent sending the 0x320 command to the 
         DSP if the vocoder state is in CAM_AUDIO_ACTIVE state */
      if (voc_get_status() == VOC_INFORM_QSYNTH)
      {
        if (qdsp_cmd_qsynth_adpcm_start(channel, &adpcm_cmd,
                                        (qdsp_cmd_adpcm_mode_type) spec->type,
                                        qdsp_cmd_mode) ==
              QDSP_CMD_SUCCESS)
        {
          /*Setting the ADPCM start flag to FALSE so that we will not send
          0x320 in the voc_state_xxx functions */
          state->adpcm_start_pending = FALSE;
          return(VOC_DONE_S);
        }
      }
      else 
      {
           ret_val = VOC_CMD_BUSY_S ;
      }
    } else {
      ret_val = VOC_CMD_BUSY_S;
    }
  }
#endif /* FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_GRAPH_ADPCM
  valid_graph =
  (
#if defined(QDSP_IMAGE_VFE_BAYER_DEFINED) || \
    defined(QDSP_IMAGE_VFE_YCBCR_DEFINED)
    (VOC_GRAPH_ADPCM_VALID_CHANNELS(spec->num_channels) &&
     VOC_GRAPH_ADPCM_VALID_BITS_PER_SAMPLE(spec->bits_per_sample) &&
     VOC_GRAPH_VFE_32K_ADPCM_VALID_SAMPLE_RATE(spec->sampling_rate) &&
     VOC_GRAPH_VFE_32K_ADPCM_VALID_BLOCK_SIZE(spec->block_length))
    ||
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED || QDSP_IMAGE_VFE_YCBCR_DEFINED */
    (VOC_GRAPH_ADPCM_VALID_CHANNELS(spec->num_channels)             &&
     (VOC_GRAPH_ADPCM_VALID_BITS_PER_SAMPLE(spec->bits_per_sample)) &&
     (VOC_GRAPH_ADPCM_VALID_SAMPLE_RATE(spec->sampling_rate))       &&
     (VOC_GRAPH_ADPCM_VALID_BLOCK_SIZE(spec->block_length)))
  );

  if (valid_graph == TRUE) {
    if(check_interlock(VOC_UNLOCK_GRAPH)) {
      state = voc_state_get_state_data();
#if defined(QDSP_IMAGE_VFE_BAYER_DEFINED) || \
    defined(QDSP_IMAGE_VFE_YCBCR_DEFINED)
#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
      if ((state->current_image == QDSP_IMAGE_VFE_BAYER) &&
          !VOC_GRAPH_VFE_32K_ADPCM_VALID_SAMPLE_RATE(spec->sampling_rate)) {
        ret_val = VOC_CMD_FAILED_S;
      } else
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED */
#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
      if ((state->current_image == QDSP_IMAGE_VFE_YCBCR) &&
          !VOC_GRAPH_VFE_32K_ADPCM_VALID_SAMPLE_RATE(spec->sampling_rate)) {
        ret_val = VOC_CMD_FAILED_S;
      } else
#endif /* QDSP_IMAGE_VFE_YCBCR_DEFINED */
      if (
#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
          (state->current_image != QDSP_IMAGE_VFE_BAYER) &&
#endif
#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
          (state->current_image != QDSP_IMAGE_VFE_YCBCR) &&
#endif
          !VOC_GRAPH_ADPCM_VALID_SAMPLE_RATE(spec->sampling_rate)) {
        ret_val = VOC_CMD_FAILED_S;
      }
      else
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED || QDSP_IMAGE_VFE_YCBCR_DEFINED */
      {
        if (state->adpcm_mode_enabled) {
          state->adpcm_block         = spec->block_length;
          state->adpcm_sampling_rate = spec->sampling_rate;
          state->adpcm_num_channels  = spec->num_channels;
          state->adpcm_new_config    = TRUE;
          (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
          ret_val = VOC_DONE_S;
        } else {
          return (VOC_CMD_BUSY_S);
        }
      }
    } else {
      return (VOC_CMD_BUSY_S);
    }
  }
#endif /* FEATURE_GRAPH_ADPCM */

#ifdef FEATURE_VOC_ADPCM
  valid_voc_adpcm =
      (VOC_GRAPH_ADPCM_VALID_CHANNELS(spec->num_channels)           &&
       VOC_GRAPH_ADPCM_VALID_BITS_PER_SAMPLE(spec->bits_per_sample) &&
       VOC_VOC_32K_ADPCM_VALID_SAMPLE_RATE(spec->sampling_rate)     &&
       VOC_VOC_32K_ADPCM_VALID_BLOCK_SIZE(spec->block_length));

  if (valid_voc_adpcm == TRUE) {

    if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {

      state = voc_state_get_state_data();

      if (state->adpcm_mode_enabled) {
        state->adpcm_block         = spec->block_length;
        state->adpcm_sampling_rate = spec->sampling_rate;
        state->adpcm_num_channels  = spec->num_channels;
        state->adpcm_voc_config    = TRUE;

        (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
        ret_val = VOC_DONE_S;
      } else {
        ret_val = VOC_CMD_BUSY_S;
      }
    } else {
      ret_val = VOC_CMD_BUSY_S;
    }
  }
#endif /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_VOC_G711
  valid_g711_wav = (VOC_G711_WAV_VALID_MODE(spec->type) &&
                    VOC_G711_WAV_VALID_SAMPLE_RATE(spec->sampling_rate) &&
                    VOC_G711_WAV_VALID_BLOCK_SIZE(spec->block_length) &&
                    VOC_G711_WAV_VALID_CHANNELS(spec->num_channels) &&
                    VOC_G711_WAV_VALID_BITS_PER_SAMPLE(spec->bits_per_sample));

  if(valid_g711_wav) {
    if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {

      state = voc_state_get_state_data();

      if (state->adpcm_mode_enabled) {
        state->adpcm_block         = spec->block_length;
        state->adpcm_sampling_rate = spec->sampling_rate;
        state->adpcm_num_channels  = spec->num_channels;
        state->wav_config          = spec->type;

        (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

        ret_val = VOC_DONE_S;
      } else {
        ret_val = VOC_CMD_BUSY_S;
      }
    } else {
      ret_val = VOC_CMD_BUSY_S;
    }
  }
#endif /* FEATURE_VOC_G711 */

  return (ret_val);
}
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#ifdef FEATURE_ADEC
/*===========================================================================

FUNCTION voc_adec_flush

DESCRIPTION
  This function causes the vocoder to flush all internal buffers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_adec_flush (
  uint16 flush_mask
) {
#if defined(FEATURE_QTUNES_COMMON)
  if(check_interlock(VOC_UNLOCK_QTUNES)) {
    voc_adec_flush_retry_in_progress = FALSE;
    voc_set_adec_flush_retry_timer(FALSE);
    if (qdsp_cmd_adec_flush(flush_mask) != QDSP_CMD_SUCCESS) {
      voc_adec_flush_retry_in_progress = TRUE;
      voc_set_adec_flush_retry_timer(TRUE);
      return(VOC_CMD_BUSY_S);
    }
  }
  else {
    voc_adec_flush_retry_in_progress = FALSE;
  }
#endif /* FEATURE_QTUNES_COMMON */

#if defined(FEATURE_QSYNTH_COMMON)
  if(check_interlock(VOC_UNLOCK_QSYNTH)) {
    (void) qdsp_cmd_adec_flush(flush_mask);
    return(VOC_DONE_S);
  }
#endif /* FEATURE_QSYNTH_COMMON */

#if defined(FEATURE_AVS_INCALL_ADEC_PB)
  if(check_interlock(VOC_UNLOCK_VOCODER_ADEC)) {
    (void) qdsp_cmd_adec_flush(flush_mask);
    return(VOC_DONE_S);
  }
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  if(check_interlock(VOC_UNLOCK_MIXER_ADEC)) {
    (void) qdsp_cmd_adec_flush(flush_mask);
    return(VOC_DONE_S);
  }
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#if defined(FEATURE_VOC_ADPCM)
  if(check_interlock(VOC_UNLOCK_VOCODER_ADPCM)) {
    (void) qdsp_cmd_adec_flush(flush_mask);
    return(VOC_DONE_S);
  }
#endif  /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_VOC_G711
  if (check_interlock(VOC_UNLOCK_VOCODER) &&
      voc_state_get_state_data()->module_reqd == QDSP_MODULE_G711) {
    (void) qdsp_cmd_adec_flush(flush_mask);
    return(VOC_DONE_S);
  }
#endif /* FEATURE_VOC_G711 */

  return(VOC_DONE_S);
}

/* <EJECT> */
#ifdef FEATURE_QSYNTH_ADPCM
/*===========================================================================

FUNCTION voc_adec_adpcm_stop

DESCRIPTION
  This function will stop a QSynth adpcm channel.

DEPENDENCIES
  FEATURE_QSYNTH_ADPCM.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_adec_adpcm_stop (
  uint32 channel
) {
  if(check_interlock(VOC_UNLOCK_QSYNTH)) {
    qdsp_cmd_qsynth_adpcm_stop(channel);
  }
}
#endif /* FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION voc_adec_mvol

DESCRIPTION
  This function sets the adec master volume.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_adec_mvol (
  uint16 vol
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if(check_interlock(VOC_UNLOCK_QSYNTH)) {
    if(qdsp_cmd_adec_mvol(vol) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION voc_adec_chan_pan

DESCRIPTION
  This function selects a new adec channel stereo pan value for playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_adec_chan_pan (
  uint32 channel,
  sint15 pan
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if(
#ifdef FEATURE_QTUNES_COMMON
	  check_interlock(VOC_UNLOCK_QTUNES) ||
#endif
	  check_interlock(VOC_UNLOCK_QSYNTH)) {
    if(qdsp_cmd_adec_chan_pan(channel, pan) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_chan_start

DESCRIPTION
  This function starts the indicated adec channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_adec_chan_start (
  uint32  channel
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if(check_interlock(VOC_UNLOCK_QSYNTH)) {
    if(qdsp_cmd_adec_chan_start(channel) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_chan_pause

DESCRIPTION
  This function selects a new adec channel stereo pause flag for playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_adec_chan_pause (
  uint32  channel,
  boolean pause_flag
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if(check_interlock(VOC_UNLOCK_QSYNTH)) {
    if(qdsp_cmd_adec_chan_pause(channel, pause_flag) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_chan_vol

DESCRIPTION
  This function selects a new adec channel volume level for playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_adec_chan_vol (
  uint32 channel,
  uint16 volume
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if(
#ifdef FEATURE_QTUNES_COMMON
	  check_interlock(VOC_UNLOCK_QTUNES) ||
#endif
	  check_interlock(VOC_UNLOCK_QSYNTH)) {
    if(qdsp_cmd_adec_chan_vol(channel, volume) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION voc_set_adec_mode

DESCRIPTION
  This function sets the audio decoder mode and signals the voccoder
  task to check the state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_set_adec_mode (
  voc_qsynth_adec_mode_type mode
) {
#ifdef FEATURE_QSYNTH_ADPCM
  voc_state_get_state_data()->adec_mode_pending = mode;

  /* Take care of any Qsynth audio decoder changes
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
#endif /* FEATURE_QSYNTH_ADPCM */
}
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_adec_pcm_stopped

DESCRIPTION
  This function checks if PCM output has stopped.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if PCM output has stopped.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_adec_pcm_stopped()
{
#ifdef FEATURE_QTUNES_COMMON
  if (check_interlock(VOC_UNLOCK_QTUNES)
#ifdef FEATURE_AVS_INCALL_ADEC_PB
      || check_interlock(VOC_UNLOCK_VOCODER_ADEC)
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
      || check_interlock(VOC_UNLOCK_MIXER_ADEC)
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
     ) {

    return(qdsp_cmd_adec_pcm_stopped());
  } else
#endif
  {
    return(FALSE);
  }
}

#if defined(FEATURE_AUDIO_FORMAT) && \
    (defined(FEATURE_AVS_INCALL_ADEC_PB) || \
     defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB))
/*===========================================================================

FUNCTION voc_set_audfmt_mix_mode

DESCRIPTION
  Sets the Audio playback decoder mixing mode with voice call audio

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_set_audfmt_mix_mode(voc_adec_mix_mode_type mix_mode)
{
  voc_state_control_type *state;

  state = voc_state_get_state_data();

  state->audfmt_mix_mode = mix_mode;
}
#endif /* FEATURE_AUDIO_FORMAT &&
          (FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB) */

#ifdef FEATURE_MPEG_AUDIO
/* <EJECT> */
/*===========================================================================

FUNCTION voc_mpeg_set_song_config

DESCRIPTION
  Initializes the ADEC to optimize playback on a specific MP3 or AAC song.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_mpeg_set_song_config(
  const uint8 *mask,
  const uint8 *header,
  const uint8 mask_size,   /* size in bytes */
  const voc_capability_type voc_cap
)
{
  if((check_interlock(VOC_UNLOCK_QTUNES)
#ifdef FEATURE_AVS_INCALL_ADEC_PB
      || (check_interlock(VOC_UNLOCK_VOCODER_ADEC))
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
      || (check_interlock(VOC_UNLOCK_MIXER_ADEC))
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
     ) &&
     voc_state_get_state_data()->config == voc_cap) {
    qdsp_cmd_mpeg_target_header(mask, header, mask_size);
    return TRUE;
  }
  return FALSE;
}
#endif /* FEATURE_MPEG_AUDIO */

#ifdef FEATURE_AAC
/* <EJECT> */
/*===========================================================================

FUNCTION voc_aac_set_song_config

DESCRIPTION
  Initializes the ARM buffer size to optimize playback on a specific AAC song.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the value configuration could be set (AAC image is downloaded).
  FALSE if the AAC image was not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_aac_set_song_config(
  const voc_aac_config_type *new_cfg
)
{
  boolean status = FALSE;
  voc_state_control_type *state = voc_state_get_state_data();
  voc_aac_config_type    *curr_cfg = &state->aac_config;

  /* Only signal the vocoder state machine if the config is different from the
   * current config, to prevent it from transiting to INIT if possible */
  if ((state->tunes_config_pending    == TRUE)                            ||
      (new_cfg->aac_format            != curr_cfg->aac_format)            ||
      (new_cfg->sample_freq_index     != curr_cfg->sample_freq_index)     ||
      (new_cfg->audio_object          != curr_cfg->audio_object)          ||
      (new_cfg->channel_configuration != curr_cfg->channel_configuration) ||
      (new_cfg->ep_config             != curr_cfg->ep_config)             ||
      (new_cfg->aac_section_data_resilience_flag !=
       curr_cfg->aac_section_data_resilience_flag)      ||
      (new_cfg->aac_scalefactor_data_resilience_flag !=
       curr_cfg->aac_scalefactor_data_resilience_flag)  ||
      (new_cfg->aac_spectral_data_resilience_flag !=
       curr_cfg->aac_spectral_data_resilience_flag)) {

    MSG_HIGH("voc_aac_set_song_config:- AAC configuration changed ",0,0,0);
    MSG_LOW("state->tunes_config_pending(T) :%d ",state->tunes_config_pending,0,0);
    state->tunes_config_pending = TRUE;
    state->aac_config = *new_cfg;

    /* Tell the state machine to update the values on the DSP */
    (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  }
  else if((check_interlock(VOC_UNLOCK_QTUNES)
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
       || (check_interlock(VOC_UNLOCK_MIXER_ADEC))
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
     ) &&
     voc_state_get_state_data()->config == VOC_CAP_QTUNES_AAC) {
    status = TRUE;
  }
  return status;
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_aac_reset_song_config

DESCRIPTION
  Initializes the ARM buffer size to optimize playback on a specific AAC song.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the value configuration could be set (AAC image is downloaded).
  FALSE if the AAC image was not available

SIDE EFFECTS
  None.

===========================================================================*/
void  voc_aac_reset_song_config(void)
{
  voc_state_control_type *state = voc_state_get_state_data();

    state->aac_config.buffer_factor      = 50;
    state->aac_config.aac_format         = VOC_AAC_DATA_FORMAT_ADIF;
    state->aac_config.sample_freq_index  = 11;
    state->aac_config.audio_object       = 2;
    state->aac_config.channel_configuration                = 0;
    state->aac_config.ep_config                            = 0;
    state->aac_config.aac_section_data_resilience_flag     = 0;
    state->aac_config.aac_scalefactor_data_resilience_flag = 0;
    state->aac_config.aac_spectral_data_resilience_flag    = 0;
 return;
}/* voc_aac_reset_song_config */


#endif /* FEATURE_AAC */

#ifdef FEATURE_MP3
/*===========================================================================

FUNCTION voc_mp3_set_song_config

DESCRIPTION
  Initializes the ARM buffer size to optimize playback on a specific MP3 song.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the value configuration could be set (MP3 image is downloaded).
  FALSE if the MP3 image was not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_mp3_set_song_config(
  const voc_mp3_config_type *new_cfg
)
{
  boolean status = FALSE;
  voc_state_control_type *state = voc_state_get_state_data();
  voc_mp3_config_type    *curr_cfg = &state->mp3_config;

  /* Only signal the vocoder state machine if the config is different from the
   * current config, to prevent it from transiting to INIT if possible */
  if ((state->tunes_config_pending    == TRUE)||
      (new_cfg->sample_freq != curr_cfg->sample_freq)) {
    MSG_HIGH("voc_mp3_set_song_config:- sample freq changed ",0,0,0);
    MSG_LOW("voc_mp3_set_song_config:- state->tunes_config_pending(T) :%d ",
                                          state->tunes_config_pending,0,0);
    state->tunes_config_pending = TRUE;
    state->mp3_config = *new_cfg;

    /* Tell the state machine to update the values on the DSP */
    (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  }
  else if((check_interlock(VOC_UNLOCK_QTUNES)
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
       || (check_interlock(VOC_UNLOCK_MIXER_ADEC))
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
     ) &&
     voc_state_get_state_data()->config == VOC_CAP_QTUNES) {
    status = TRUE;
  }
  return status;
}
#endif /* FEATURE_MP3 */

#ifdef FEATURE_REAL_AUDIO
/*===========================================================================

FUNCTION voc_ra_set_song_config

DESCRIPTION
  Set configuration params for RealAudio song playback.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the value configuration could be set
  FALSE if the RealAudio image was not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_ra_set_song_config(
  const voc_ra_dsp_config_type *ra_cfg
)
{
  boolean status = FALSE;
  voc_state_control_type *state = voc_state_get_state_data();
  voc_ra_dsp_config_type *curr_cfg = &state->real_audio_config;

  /* Only signal the vocoder state machine if the config is different from the
   * current config, to prevent it from transiting to INIT if possible */
  if (state->tunes_config_pending == TRUE                      ||
      ra_cfg->decoder_mode      != curr_cfg->decoder_mode      ||
      ra_cfg->sample_rate       != curr_cfg->sample_rate       ||
      ra_cfg->num_channels      != curr_cfg->num_channels      ||
      ra_cfg->samples_per_frame != curr_cfg->samples_per_frame ||
      ra_cfg->num_regions       != curr_cfg->num_regions       ||
      ra_cfg->bits_per_frame    != curr_cfg->bits_per_frame    ||
      ra_cfg->coupling_start    != curr_cfg->coupling_start    ||
      ra_cfg->frame_erasure     != curr_cfg->frame_erasure) {

    MSG_HIGH("voc_ra_set_song_config:- RA configuration changed ",0,0,0);
    state->tunes_config_pending = TRUE;
    state->real_audio_config = *ra_cfg;

    /* Tell the state machine to update the values on the DSP */
    (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  }
  else if(check_interlock(VOC_UNLOCK_QTUNES) &&
     voc_state_get_state_data()->config == VOC_CAP_QTUNES_RA) {
    status = TRUE;
  }
  return status;
}

/*===========================================================================

FUNCTION voc_ra_reset_song_config

DESCRIPTION
  Set configuration params for RealAudio song playback.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the value configuration could be set
  FALSE if the RealAudio image was not available

SIDE EFFECTS
  None.

===========================================================================*/
void  voc_ra_reset_song_config(void)
{
  voc_state_control_type *state = voc_state_get_state_data();
  state->real_audio_config.decoder_mode      = 1;
  state->real_audio_config.sample_rate       = 1;
  state->real_audio_config.num_channels      = 1;
  state->real_audio_config.samples_per_frame = 512;
  state->real_audio_config.num_regions       = 24;
  state->real_audio_config.bits_per_frame    = 512;
  state->real_audio_config.coupling_start    = 0;
  state->real_audio_config.coupling_quant    = 0;
  state->real_audio_config.frame_erasure     = 0;
  return;

}
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
/*===========================================================================

FUNCTION voc_wma_set_song_config

DESCRIPTION
  Set configuration params for Windows Media Audio song playback.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the value configuration could be set
  FALSE if the WindowsMedia image was not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_wma_set_song_config(
  const voc_wma_config_type *wma_cfg
)
{
  boolean status = FALSE;
  voc_state_control_type *state;
  voc_wma_config_type *curr_cfg;

  state = voc_state_get_state_data();
  curr_cfg = &state->wma_config;

  /* Only signal the vocoder state machine if the config is different from the
   * current config, to prevent it from transiting to INIT if possible */
  if (state->tunes_config_pending == TRUE                               ||
      wma_cfg->version                     != curr_cfg->version         ||
      wma_cfg->bits_per_sample             != curr_cfg->bits_per_sample ||
      wma_cfg->format_tag                  != curr_cfg->format_tag      ||
      wma_cfg->channel_mask                != curr_cfg->channel_mask    ||
      wma_cfg->advanced_encoder_opt1       != curr_cfg->advanced_encoder_opt1 ||
      wma_cfg->advanced_encoder_opt2       != curr_cfg->advanced_encoder_opt2 ||
      wma_cfg->virtual_pkt_len             != curr_cfg->virtual_pkt_len    ||
      wma_cfg->encoder_options             != curr_cfg->encoder_options    ||
      wma_cfg->sample_rate                 != curr_cfg->sample_rate        ||
      wma_cfg->bytes_per_second            != curr_cfg->bytes_per_second   ||
      wma_cfg->channels                    != curr_cfg->channels ) {

    MSG_HIGH("voc_wma_set_song_config:- WMA configuration changed ",0,0,0);
    state->tunes_config_pending = TRUE;
    state->wma_config = *wma_cfg;

    /* Tell the state machine to update the values on the DSP */
    (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  } else if((check_interlock(VOC_UNLOCK_QTUNES)
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
    || (check_interlock(VOC_UNLOCK_MIXER_ADEC))
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
    )
#ifdef FEATURE_WMA
    &&((voc_state_get_state_data()->config == VOC_CAP_QTUNES_WMA) ||
	   (voc_state_get_state_data()->config == VOC_CAP_QTUNES_WMA_PRO))
	)
#endif
  {
    status = TRUE;
  }
  return status;
}

/*===========================================================================

FUNCTION voc_wma_reset_song_config

DESCRIPTION
  Set configuration params for Windows Media Audio song playback.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the value configuration could be set
  FALSE if the WindowsMedia image was not available

SIDE EFFECTS
  None.

===========================================================================*/
void voc_wma_reset_song_config( void )
{
  voc_state_control_type *state = voc_state_get_state_data();

  state->wma_config.version          = 0x2;
  state->wma_config.channels         = 0x2;
  state->wma_config.bytes_per_second = 0x4E25;
  state->wma_config.sample_rate      = 0x1;
  state->wma_config.encoder_options  = 0xF;

  return;
}

#endif /* FEATURE_WMA */

/* <EJECT> */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
/*===========================================================================

FUNCTION voc_amr_wbp_set_song_config

DESCRIPTION
  Sends a set of AMR-WB+ decoder configurations to the vocoder service.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the new configuration is in effect
  FALSE if the new configuration is pending change

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_amr_wbp_set_song_config (
  const voc_amr_wbp_config_type *new_cfg
)
{
  boolean status = FALSE;
  voc_state_control_type *state = voc_state_get_state_data();
  voc_amr_wbp_config_type *cur_cfg = &state->amr_wbp_config;

  /* Signal the vocoder state machine only if necessary: Try to minimize the
   * costly transition to the INIT state */
  if ((state->tunes_config_pending == TRUE)                 ||
      (new_cfg->sample_rate        != cur_cfg->sample_rate) ||
      (new_cfg->dec_chan_mode      != cur_cfg->dec_chan_mode)) {

    MSG_HIGH(" AMR WBP configuration changed ",0,0,0);
    state->amr_wbp_config       = *new_cfg;
    state->tunes_config_pending = TRUE;

    (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
  }
  else if (check_interlock(VOC_UNLOCK_QTUNES) &&
    voc_state_get_state_data()->config == VOC_CAP_QTUNES_AMR_WBPLUS) {

    status = TRUE;
  }
  return status;
}

/*===========================================================================

FUNCTION voc_amr_wbp_reset_song_config

DESCRIPTION
  Sends a set of AMR-WB+ decoder configurations to the vocoder service.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the new configuration is in effect
  FALSE if the new configuration is pending change

SIDE EFFECTS
  None.

===========================================================================*/
void voc_amr_wbp_reset_song_config (void)
{
  voc_state_control_type *state = voc_state_get_state_data();

  state->amr_wbp_config.sample_rate   = 16000;
  state->amr_wbp_config.dec_chan_mode = 0;

  return;
}

#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

#endif /* FEATURE_ADEC */

/* <EJECT> */
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION voc_set_dls_config

DESCRIPTION
  This function sets the DLS configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_dls_config (
  voc_dls_config_type *dls_config
) {
  /* Update database */
  voc_data_set_dls_config( dls_config );

  if(check_interlock(VOC_UNLOCK_NOT_RESET)) {
    switch(voc_state_get_state_data()->current_image) {
#if defined(QDSP_IMAGE_SYNTH_DEFINED) || \
    defined(QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED)
#if defined(QDSP_IMAGE_SYNTH_DEFINED)
      case QDSP_IMAGE_SYNTH:
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
      case QDSP_IMAGE_VOC_COMBO_SYNTH:
#endif
    qdsp_cmd_set_dls_config(voc_data_get_dls_config());
        break;
#endif
      default:
        break;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_qsynth_vol_mode

DESCRIPTION
  This function sets the qsynth volume level configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_qsynth_vol_mode(
  voc_qsynth_vol_mode_enum_type mode
) {
  voc_state_get_state_data()->qsynth_vol_mode_pending = mode;

  /* Take care of any Qsynth volume mode changes
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_hybrid_mode

DESCRIPTION
  This function sets the hybrid mode configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_hybrid_mode (
  uint8 hybrid_mode
) {
  voc_state_get_state_data()->hybrid_mode_pending = hybrid_mode;

  /* Take care of any Qsynth hybrid mode changes
  */
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_pause_midi_seq

DESCRIPTION
  This function issues a pause command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_pause_midi_seq (
  uint32  sequence,
  boolean pause_flag
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if(check_interlock(VOC_UNLOCK_QSYNTH)) {
    if(qdsp_cmd_pause_midi_seq(sequence, pause_flag) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_vol_midi_seq

DESCRIPTION
  This function issues a set volume command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_set_vol_midi_seq (
  uint32 sequence,
  uint16 vol
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if(check_interlock(VOC_UNLOCK_QSYNTH)) {
    if(qdsp_cmd_set_vol_midi_seq(sequence, vol) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}
#endif /* FEATURE_MIDI */

/* <EJECT> */
#if defined(FEATURE_AUDIO_FORMAT) || \
    defined(FEATURE_MIDI_OUT)     || \
    defined(FEATURE_VOC_ADPCM)
/*===========================================================================

FUNCTION voc_sample_rate

DESCRIPTION
  Select a new sample rate for the codec.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_sample_rate ( voc_sample_rate_type rate )
{
#ifdef FEATURE_EXTERNAL_SDAC
  voc_codec_class_type       codec_type;
#endif /* FEATURE_EXTERNAL_SDAC */
  boolean dsp_ready = FALSE;

#ifdef FEATURE_VOC_ADPCM
  boolean voc_dsp_ready = FALSE;
#endif /* FEATURE_VOC_ADPCM */

#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  boolean incall_adec_dsp_ready = FALSE;
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB*/

  voc_state_control_type     *state;

  voc_data_set_sample_rate(rate);

  /* Setup audio format pro-processing blocks
  */
  voc_set_audfmt_pp_config();

  /* Make any changes based on current settings
  */
  if (check_interlock(VOC_UNLOCK_NOT_RESET)) {

#ifdef FEATURE_EXTERNAL_SDAC
    /* Get the PCM path */
    voc_data_get_codec_class(&codec_type);

    /* Get the current state data
    */
    state = voc_state_get_state_data();

    if ((codec_type == VOC_CODEC_CLASS_EXTERNAL_STEREO)
#ifdef QDSP_IMAGE_VOC_MP3_DEFINED
        && (state->current_image != QDSP_IMAGE_VOC_MP3)
#endif
#ifdef QDSP_IMAGE_VOC_EVRC_MP3_DEFINED
        && (state->current_image != QDSP_IMAGE_VOC_EVRC_MP3)
#endif
#ifdef QDSP_IMAGE_AMR_MP3_DEFINED
        && (state->current_image != QDSP_IMAGE_AMR_MP3)
#endif
#ifdef QDSP_IMAGE_HR_MP3_DEFINED
        && (state->current_image != QDSP_IMAGE_HR_MP3)
#endif
#ifdef QDSP_IMAGE_EFR_MP3_DEFINED
        && (state->current_image != QDSP_IMAGE_EFR_MP3)
#endif
#ifdef QDSP_IMAGE_FR_MP3_DEFINED
        && (state->current_image != QDSP_IMAGE_FR_MP3)
#endif
#ifdef QDSP_IMAGE_VOC_DEFINED
        && (state->current_image != QDSP_IMAGE_VOC)
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_DEFINED
        && (state->current_image != QDSP_IMAGE_VOC_COMBO)
#endif /* QDSP_IMAGE_VOC_COMBO_DEFINED */
#ifdef FEATURE_AMR_FIXED
        && (state->current_image != QDSP_IMAGE_AMR)
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED
        && (state->current_image != QDSP_IMAGE_VOC_COMBO_VOIP)
#endif /* QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED */
#ifdef QDSP_IMAGE_VOC_EVRC_DEFINED
        && (state->current_image != QDSP_IMAGE_VOC_EVRC)
#endif /* QDSP_IMAGE_VOC_EVRC_DEFINED */
#ifdef QDSP_IMAGE_VOC_E90_DEFINED
        && (state->current_image != QDSP_IMAGE_VOC_E90)
#endif /* QDSP_IMAGE_VOC_E90_DEFINED */
#ifdef QDSP_IMAGE_VOC_EVRC_E90_DEFINED
        && (state->current_image != QDSP_IMAGE_VOC_EVRC_E90)
#endif /* QDSP_IMAGE_VOC_EVRC_E90_DEFINED */
#ifdef QDSP_IMAGE_VOC_QTV_DEFINED
        && (state->current_image != QDSP_IMAGE_VOC_QTV)
#endif /* QDSP_IMAGE_VOC_QTV_DEFINED */
#ifdef QDSP_IMAGE_FR_DEFINED
#error code not present
#endif /* QDSP_IMAGE_VOC_FR_DEFINED */
#ifdef QDSP_IMAGE_EFR_DEFINED
#error code not present
#endif /* QDSP_IMAGE_VOC_EFR_DEFINED */
#ifdef QDSP_IMAGE_AMR_DEFINED
        && (state->current_image != QDSP_IMAGE_AMR)
#endif /* QDSP_IMAGE_VOC_AMR_DEFINED */
#ifdef QDSP_IMAGE_AMR_WB_DEFINED
        && (state->current_image != QDSP_IMAGE_AMR_WB)
#endif /* QDSP_IMAGE_VOC_AMR_WB_DEFINED */
#ifdef QDSP_IMAGE_HR_DEFINED
        && (state->current_image != QDSP_IMAGE_HR)
#endif /* QDSP_IMAGE_VOC_HR_DEFINED */
#ifdef QDSP_IMAGE_4GV_DEFINED
        && (state->current_image != QDSP_IMAGE_4GV)
#endif /* QDSP_IMAGE_4GV_DEFINED */
#ifdef QDSP_IMAGE_4GV_WB_DEFINED
        && (state->current_image != QDSP_IMAGE_4GV_WB)
#endif /* QDSP_IMAGE_4GV_WB_DEFINED */
#ifdef QDSP_IMAGE_4GV_VOIP_DEFINED
        && (state->current_image != QDSP_IMAGE_4GV_VOIP)
#endif /* QDSP_IMAGE_4GV_VOIP_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED
        && (state->current_image != QDSP_IMAGE_QCAMCORDER_COMBO)
#endif /*QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED*/
       ) {
      voc_state_config_external_sdac(voc_data_get_sample_rate());
      return TRUE;
    }
#endif /* FEATURE_EXTERNAL_SDAC */

#ifdef MSMAUD_ADIE_CODEC
    state = voc_state_get_state_data();

#ifdef FEATURE_QSYNTH_COMMON
    dsp_ready |= check_interlock(VOC_UNLOCK_QSYNTH);
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QTUNES_COMMON
    dsp_ready |= check_interlock(VOC_UNLOCK_QTUNES);
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_AVS_INCALL_ADEC_PB
    incall_adec_dsp_ready |= check_interlock(VOC_UNLOCK_VOCODER_ADEC);
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
    incall_adec_dsp_ready |= check_interlock(VOC_UNLOCK_MIXER_ADEC);
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifdef FEATURE_VOC_ADPCM
    voc_dsp_ready = FALSE;

    if (dsp_ready == FALSE) {
      switch(state->current_image) {
#ifdef QDSP_IMAGE_VOC_DEFINED
        case QDSP_IMAGE_VOC:
#endif /* QDSP_IMAGE_VOC_DEFINED */
#ifdef QDSP_IMAGE_VOC_COMBO_DEFINED
        case QDSP_IMAGE_VOC_COMBO:
#endif /* QDSP_IMAGE_VOC_COMBO_DEFINED */
#ifdef FEATURE_AMR_FIXED
        case QDSP_IMAGE_AMR:
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED
        case QDSP_IMAGE_VOC_COMBO_VOIP:
#endif /* QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED */
#ifdef QDSP_IMAGE_VOC_EVRC_DEFINED
        case QDSP_IMAGE_VOC_EVRC:
#endif /* QDSP_IMAGE_VOC_EVRC_DEFINED */
#ifdef QDSP_IMAGE_VOC_E90_DEFINED
        case QDSP_IMAGE_VOC_E90:
#endif /* QDSP_IMAGE_VOC_E90_DEFINED */
#ifdef QDSP_IMAGE_VOC_EVRC_E90_DEFINED
        case QDSP_IMAGE_VOC_EVRC_E90:
#endif /* QDSP_IMAGE_VOC_EVRC_E90_DEFINED */
#ifdef QDSP_IMAGE_VOC_QTV_DEFINED
        case QDSP_IMAGE_VOC_QTV:
#endif /* QDSP_IMAGE_VOC_QTV_DEFINED */
#ifdef QDSP_IMAGE_FR_DEFINED
#error code not present
#endif /* QDSP_IMAGE_FR_DEFINED */
#ifdef QDSP_IMAGE_EFR_DEFINED
#error code not present
#endif /* QDSP_IMAGE_EFR_DEFINED */
#ifdef QDSP_IMAGE_AMR_DEFINED
        case QDSP_IMAGE_AMR:
#endif /* QDSP_IMAGE_AMR_DEFINED */
#ifdef QDSP_IMAGE_AMR_WB_DEFINED
        case QDSP_IMAGE_AMR_WB:
#endif /* QDSP_IMAGE_AMR_WB_DEFINED */
#ifdef QDSP_IMAGE_HR_DEFINED
        case QDSP_IMAGE_HR:
#endif /* QDSP_IMAGE_HR_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED
    case QDSP_IMAGE_QCAMCORDER_COMBO:
#endif /*QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED*/
          voc_dsp_ready = TRUE;
          break;

        default:
          break;
      }
    }
#endif /* FEATURE_VOC_ADPCM */

    if (dsp_ready) {
      #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
      #else
      voc_state_set_icodec_clk(voc_data_get_sample_rate());
      #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
      return TRUE;
    }
#ifdef FEATURE_VOC_ADPCM
    else if (voc_dsp_ready) {
      return TRUE;
    }
#endif /* FEATURE_VOC_ADPCM */
#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
    else if (incall_adec_dsp_ready) {
      return TRUE;
    }
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#endif /* MSMAUD_ADIE_CODEC */
  }

  return FALSE;
}
#endif /* FEATURE_AUDIO_FORMAT || FEATURE_MIDI_OUT || FEATURE_VOC_ADPCM */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
/*===========================================================================

FUNCTION voc_3d_pos_enable

DESCRIPTION
  This function issues the appropriate 3D positional enable/disable
  command to the DSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_enable (
  voc_3d_pos_mode_enum_type mode
) {
  qdsp_cmd_3d_pos_output_mode_enum_type output_mode;
  qdsp_cmd_3d_pos_mode_enum_type        enable_mode =
                                           QDSP_CMD_3D_POS_MODE_DISABLE;
  voc_status_type                       ret_val = VOC_CMD_FAILED_S;

  voc_data_set_3d_pos_enable_mode(mode);

  if(check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    output_mode = voc_data_get_3d_pos_output_mode();

    if(qdsp_cmd_3d_pos_set_output_mode(output_mode) != QDSP_CMD_SUCCESS) {
      MSG_ERROR("Failed to set 3D output mode", 0, 0, 0);
    }

    if (mode == VOC_3D_POS_MODE_ENABLE) {
      enable_mode = QDSP_CMD_3D_POS_MODE_ENABLE;
    }

    if(qdsp_cmd_3d_pos_enable(enable_mode) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/*===========================================================================

FUNCTION voc_3d_pos_reset_env

DESCRIPTION
  This function issues 3D environment reset command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_reset_env () {
  voc_status_type    ret_val = VOC_CMD_FAILED_S;
  if (check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if (qdsp_cmd_3d_pos_reset_env() == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/*===========================================================================

FUNCTION voc_3d_pos_set_output_mode

DESCRIPTION
  This function sets QAFX output mode.

DEPENDENCIES
  Only valid when QSYNTH firmware is resident in DSP.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_set_output_mode () {
  qdsp_cmd_3d_pos_output_mode_enum_type output_mode;
  voc_status_type                       ret_val = VOC_CMD_FAILED_S;

  if(check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    output_mode = voc_data_get_3d_pos_output_mode();

    if(qdsp_cmd_3d_pos_set_output_mode(output_mode) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_lsnr_pos

DESCRIPTION
  This function issues a 3D positional listener position command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_lsnr_pos (
  voc_3d_pos_coord_type *coord,
  uint32                duration_ms
) {
  qdsp_cmd_3d_pos_coord_type qdsp_coord;
  voc_status_type            ret_val = VOC_CMD_FAILED_S;

  if((coord != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    qdsp_coord.x_coord = coord->x_pos;
    qdsp_coord.y_coord = coord->y_pos;
    qdsp_coord.z_coord = coord->z_pos;

    if(qdsp_cmd_3d_pos_set_lsnr_pos(&qdsp_coord, duration_ms) ==
       QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_lsnr_orient

DESCRIPTION
  This function issues a 3D positional listener orientation command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_lsnr_orient (
  voc_3d_pos_orient_type *orient,
  uint32                 duration_ms
) {
  qdsp_cmd_3d_pos_orient_type qdsp_orient;
  voc_status_type             ret_val = VOC_CMD_FAILED_S;

  if((orient != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    qdsp_orient.forward.x_coord = orient->forward.x_pos;
    qdsp_orient.forward.y_coord = orient->forward.y_pos;
    qdsp_orient.forward.z_coord = orient->forward.z_pos;
    qdsp_orient.up.x_coord      = orient->up.x_pos;
    qdsp_orient.up.y_coord      = orient->up.y_pos;
    qdsp_orient.up.z_coord      = orient->up.z_pos;

    if(qdsp_cmd_3d_pos_set_lsnr_orient(&qdsp_orient, duration_ms) ==
       QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_lsnr_vel

DESCRIPTION
  This function issues a 3D positional listener velocity command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_lsnr_vel (
  voc_3d_pos_coord_type *coord,
  uint32                duration_ms
) {
  qdsp_cmd_3d_pos_coord_type qdsp_coord;
  voc_status_type            ret_val = VOC_CMD_FAILED_S;

  if((coord != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    qdsp_coord.x_coord = coord->x_pos;
    qdsp_coord.y_coord = coord->y_pos;
    qdsp_coord.z_coord = coord->z_pos;

    if(qdsp_cmd_3d_pos_set_lsnr_vel(&qdsp_coord, duration_ms) ==
       QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_reverb_reset

DESCRIPTION
  This function clears reverb preset and set the reverb enable flag to
  FALSE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function only clears the cached reverb values stored in ARM memory.

===========================================================================*/
void voc_3d_reverb_reset () {

  qdsp_cmd_3d_reverb_reset();

}

/*===========================================================================

FUNCTION voc_3d_reverb_set

DESCRIPTION
  This function issues a 3D Reverb Set command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_reverb_set (
  voc_3d_reverb_enum_type   type,
  voc_3d_reverb_type       *reverb
){
  voc_status_type ret_val = VOC_CMD_FAILED_S;
  qdsp_cmd_3d_reverb_cmd_type    cmd;
  qdsp_cmd_3d_reverb_param_type param;

  if(check_interlock(VOC_UNLOCK_QSYNTH_STD)) {

    switch( type){
      case VOC_3D_POS_REVERB_SET_PRESET:
        cmd = QDSP_CMD_3D_REVERB_PRESET;
        param.env_type      = reverb->env_type;
        param.room_gain     = voc_data_compute_mb(reverb->room_gain) >> 2;
        param.decay_ms      = reverb->decay_ms;
        param.decay_ratio   = voc_data_convert_to_qfactor(reverb->decay_ratio);
        param.ref_delay     = reverb->ref_delay;
        param.reverb_delay  = reverb->reverb_delay;
        param.reverb_gain   = voc_data_compute_mb(reverb->reverb_gain) >> 2;
        break;

      case VOC_3D_POS_REVERB_SET_LEVEL:
        cmd = QDSP_CMD_3D_REVERB_LEVEL;
        param.reverb_gain   = voc_data_compute_mb(reverb->reverb_gain) >> 2;
        break;

      case VOC_3D_POS_REVERB_SET_TIME:
        cmd = QDSP_CMD_3D_REVERB_TIME;
        param.decay_ms      = reverb->decay_ms;
        break;

      case VOC_3D_POS_REVERB_SET_RATIO:
        cmd = QDSP_CMD_3D_REVERB_RATIO;
        param.decay_ratio   = voc_data_convert_to_qfactor(reverb->decay_ratio);
        break;

      default:
        return( ret_val);
    }

    if(qdsp_cmd_3d_reverb_set(cmd, &param) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);

}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_reverb_enable

DESCRIPTION
  This function issues a 3D Reverb Enable/Disable command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_reverb_enable (
  boolean enable
){
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if(qdsp_cmd_3d_reverb_enable( enable) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_src_enable

DESCRIPTION
  This function issues sound source 3D enable/disable command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_src_enable (
  uint32                    channel,
  voc_3d_pos_mode_enum_type mode
) {
  voc_status_type           ret_val = VOC_CMD_FAILED_S;
  qdsp_cmd_3d_pos_mode_enum_type enable_mode = QDSP_CMD_3D_POS_MODE_DISABLE;
  if(check_interlock(VOC_UNLOCK_QSYNTH_STD)) {

    if (mode == VOC_3D_POS_MODE_ENABLE) {
      enable_mode = QDSP_CMD_3D_POS_MODE_ENABLE;
    }

    if(qdsp_cmd_3d_pos_src_enable(channel, enable_mode) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/*===========================================================================

FUNCTION voc_3d_pos_reset_env

DESCRIPTION
  This function issues source 3D reset command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_reset_src (
  uint32 channel
) {
  voc_status_type    ret_val = VOC_CMD_FAILED_S;
  if (check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if (qdsp_cmd_3d_pos_reset_src(channel) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/*===========================================================================
FUNCTION voc_3d_pos_src_vol

DESCRIPTION
  This function issues a 3D positional source volume command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_src_vol (
  uint32 channel,
  sint31 volume,
  uint32 duration_ms
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;
  uint16 qdsp_volume;
  if(check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    qdsp_volume = voc_data_compute_mb(volume);
    if(qdsp_cmd_3d_pos_set_src_vol(channel, qdsp_volume, duration_ms) ==
       QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_src_vol_atten

DESCRIPTION
  This function issues a 3D positional source volume attenuation command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_src_vol_atten (
  uint32                    channel,
  voc_3d_pos_vol_atten_type *vol_atten
) {
  qdsp_cmd_3d_pos_src_vol_atten_type qdsp_vol_atten;
  voc_status_type                    ret_val = VOC_CMD_FAILED_S;

  if((vol_atten != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    qdsp_vol_atten.max_distance = vol_atten->max_distance;
    qdsp_vol_atten.min_distance = vol_atten->min_distance;
    qdsp_vol_atten.rolloff      = vol_atten->scale;
    qdsp_vol_atten.mute         = vol_atten->mute;

    if(qdsp_cmd_3d_pos_set_src_vol_atten(channel, &qdsp_vol_atten) ==
       QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_src_pos

DESCRIPTION
  This function issues a 3D positional source position command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_src_pos (
  uint32                channel,
  voc_3d_pos_coord_type *coord,
  uint32                duration_ms
) {
  qdsp_cmd_3d_pos_coord_type qdsp_coord;
  voc_status_type            ret_val = VOC_CMD_FAILED_S;

  if((coord != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    qdsp_coord.x_coord = coord->x_pos;
    qdsp_coord.y_coord = coord->y_pos;
    qdsp_coord.z_coord = coord->z_pos;

    if(qdsp_cmd_3d_pos_set_src_pos(channel, &qdsp_coord, duration_ms) ==
       QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_src_vel

DESCRIPTION
  This function issues a 3D positional source velocity command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_src_vel (
  uint32                channel,
  voc_3d_pos_coord_type *coord,
  uint32                duration_ms
) {
  qdsp_cmd_3d_pos_coord_type qdsp_coord;
  voc_status_type            ret_val = VOC_CMD_FAILED_S;

  if((coord != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    qdsp_coord.x_coord = coord->x_pos;
    qdsp_coord.y_coord = coord->y_pos;
    qdsp_coord.z_coord = coord->z_pos;

    if(qdsp_cmd_3d_pos_set_src_vel(channel, &qdsp_coord, duration_ms) ==
       QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_reverb_src_vol

DESCRIPTION
  This function issues a 3D reverb source volume command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_reverb_src_vol (
  uint32                channel,
  sint31                volume
) {
  voc_status_type            ret_val = VOC_CMD_FAILED_S;
  uint16                     qdsp_volume;
  if(check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    qdsp_volume = voc_data_compute_mb(volume);
    if(qdsp_cmd_3d_reverb_set_src_vol(channel, qdsp_volume) ==
       QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_query_env_enable

DESCRIPTION
  This function queries 3D environment enable flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/
voc_status_type voc_3d_pos_query_env_enable (
  boolean *flag
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if((flag != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_env_enabled(flag) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  } else{
    MSG_ERROR( " DSP not loaded", 0, 0, 0);
  }

  return(ret_val);
}

/*===========================================================================

FUNCTION voc_3d_pos_query_lsnr_pos

DESCRIPTION
  This function queries the listener position.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_lsnr_pos (
  voc_3d_pos_coord_type *coord
) {
  qdsp_cmd_3d_pos_coord_type qdsp_coord;
  voc_status_type            ret_val = VOC_CMD_FAILED_S;

  if((coord != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_lsnr_pos(&qdsp_coord) == QDSP_CMD_SUCCESS) {
      coord->x_pos = qdsp_coord.x_coord;
      coord->y_pos = qdsp_coord.y_coord;
      coord->z_pos = qdsp_coord.z_coord;

      ret_val = VOC_DONE_S;
    }
  } else{
    MSG_ERROR( " DSP not loaded %ld", coord, 0, 0);
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_query_lsnr_orient

DESCRIPTION
  This function queries the listener orientation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_lsnr_orient (
  voc_3d_pos_orient_type *orient
) {
  qdsp_cmd_3d_pos_orient_type qdsp_orient;
  voc_status_type             ret_val = VOC_CMD_FAILED_S;

  if((orient != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_lsnr_orient(&qdsp_orient) == QDSP_CMD_SUCCESS) {
      orient->forward.x_pos = qdsp_orient.forward.x_coord;
      orient->forward.y_pos = qdsp_orient.forward.y_coord;
      orient->forward.z_pos = qdsp_orient.forward.z_coord;
      orient->up.x_pos      = qdsp_orient.up.x_coord;
      orient->up.y_pos      = qdsp_orient.up.y_coord;
      orient->up.z_pos      = qdsp_orient.up.z_coord;

      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/*===========================================================================

FUNCTION voc_3d_pos_query_lsnr_vel

DESCRIPTION
  This function queries the listener velocity.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_lsnr_vel (
  voc_3d_pos_vector_type *vector
) {
  qdsp_cmd_3d_pos_vector_type qdsp_vector;
  voc_status_type            ret_val = VOC_CMD_FAILED_S;

  if((vector != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_lsnr_vel(&qdsp_vector) == QDSP_CMD_SUCCESS) {
      vector->x_pos = qdsp_vector.x_coord;
      vector->y_pos = qdsp_vector.y_coord;
      vector->z_pos = qdsp_vector.z_coord;

      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/*===========================================================================

FUNCTION voc_3d_pos_query_reverb_enabled

DESCRIPTION
  This function queries reverb enabled flag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_reverb_enabled (
  boolean *flag
) {
  voc_status_type            ret_val = VOC_CMD_FAILED_S;

  if((flag != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_reverb_enabled(flag) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/*===========================================================================

FUNCTION voc_3d_pos_query_reverb_setting

DESCRIPTION
  This function queries reverb setting.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_reverb_setting (
  voc_3d_reverb_type *reverb
) {
  qdsp_cmd_3d_reverb_param_type qdsp_reverb;
  voc_status_type            ret_val = VOC_CMD_FAILED_S;

  if((reverb != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_reverb_setting(&qdsp_reverb) == QDSP_CMD_SUCCESS) {
      reverb->env_type = qdsp_reverb.env_type;
      reverb->decay_ms = qdsp_reverb.decay_ms;
      reverb->decay_ratio = voc_data_convert_from_qfactor(
                                                    qdsp_reverb.decay_ratio);
      reverb->ref_delay = qdsp_reverb.ref_delay;
      reverb->reverb_delay = qdsp_reverb.reverb_delay;
      reverb->reverb_gain = voc_data_compute_mb_from_abs(
                                               qdsp_reverb.reverb_gain << 2);
      reverb->room_gain = voc_data_compute_mb_from_abs(
                                               qdsp_reverb.room_gain << 2);

      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_query_src_final_cb

DESCRIPTION
  This function is the callback for the query of final source settings.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_3d_pos_query_src_final_cb (
  uint32                               channel,
  qdsp_cmd_3d_pos_src_all_setting_type *src_all
) {
  voc_3d_pos_src_all_setting_type voc_setting;

  if((voc_3d_pos_src_final_cb[channel] != NULL) && (src_all != NULL)) {
    voc_setting = *((voc_3d_pos_src_all_setting_type *) src_all);

    voc_setting.volume = voc_data_compute_mb_from_abs(voc_setting.volume);
    voc_setting.reverb_vol = voc_data_compute_mb_from_abs(voc_setting.reverb_vol);
    voc_3d_pos_src_final_cb[channel](channel, &voc_setting);

    voc_3d_pos_src_final_cb[channel] = NULL;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_query_src_final

DESCRIPTION
  This function queries the source settings at end of clip playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_src_final (
  uint32                       channel,
  voc_3d_pos_src_final_cb_type cb_func
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if(check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_src_final(channel,
                                       voc_3d_pos_query_src_final_cb) ==
       QDSP_CMD_SUCCESS) {
      voc_3d_pos_src_final_cb[channel] = cb_func;
      ret_val = VOC_DONE_S;
    }
  }
  return(ret_val);
}

/*===========================================================================

FUNCTION voc_3d_pos_query_src_enable

DESCRIPTION
  This function queries source 3D enable flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/
voc_status_type voc_3d_pos_query_src_enable (
  uint32  channel,
  boolean *flag
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  if((flag != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_src_enabled(channel, flag) == QDSP_CMD_SUCCESS) {
      ret_val = VOC_DONE_S;
    }
  } else{
    MSG_ERROR( " DSP not loaded", 0, 0, 0);
  }

  return(ret_val);
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_query_src_pos

DESCRIPTION
  This function queries the source position.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_src_pos (
  uint32                channel,
  voc_3d_pos_coord_type *coord
) {
  qdsp_cmd_3d_pos_coord_type qdsp_coord;
  voc_status_type            ret_val = VOC_CMD_FAILED_S;

  if((coord != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_src_pos(channel, &qdsp_coord) ==
       QDSP_CMD_SUCCESS) {
      coord->x_pos = qdsp_coord.x_coord;
      coord->y_pos = qdsp_coord.y_coord;
      coord->z_pos = qdsp_coord.z_coord;

      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_query_src_vol

DESCRIPTION
  This function queries the source volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_src_vol (
  uint32 channel,
  sint31 *volume
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;
  uint16          qdsp_vol;

  if((volume != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_src_vol(channel, &qdsp_vol) == QDSP_CMD_SUCCESS) {
      *volume = voc_data_compute_mb_from_abs(qdsp_vol);
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_3d_pos_query_src_vol_atten

DESCRIPTION
  This function queries the source volume attenuation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_src_vol_atten (
  uint32                    channel,
  voc_3d_pos_vol_atten_type *vol_atten
) {
  qdsp_cmd_3d_pos_src_vol_atten_type qdsp_vol_atten;
  voc_status_type                    ret_val = VOC_CMD_FAILED_S;

  if((vol_atten != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_src_vol_atten(channel, &qdsp_vol_atten) ==
       QDSP_CMD_SUCCESS) {
      vol_atten->max_distance = qdsp_vol_atten.max_distance;
      vol_atten->min_distance = qdsp_vol_atten.min_distance;
      vol_atten->scale        = qdsp_vol_atten.rolloff;
      vol_atten->mute         = qdsp_vol_atten.mute;

      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/*===========================================================================

FUNCTION voc_3d_pos_query_src_vel

DESCRIPTION
  This function queries the source velocity.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_src_vel (
  uint32                channel,
  voc_3d_pos_vector_type *vector
) {
  qdsp_cmd_3d_pos_vector_type qdsp_vector;
  voc_status_type            ret_val = VOC_CMD_FAILED_S;

  if((vector != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_src_vel(channel, &qdsp_vector) ==
       QDSP_CMD_SUCCESS) {
      vector->x_pos = qdsp_vector.x_coord;
      vector->y_pos = qdsp_vector.y_coord;
      vector->z_pos = qdsp_vector.z_coord;

      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/*===========================================================================

FUNCTION voc_3d_pos_query_src_revol

DESCRIPTION
  This function queries the source reverb volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_3d_pos_query_src_revol (
  uint32 channel,
  sint31 *volume
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;
  uint16          qdsp_vol;

  if((volume != NULL) && check_interlock(VOC_UNLOCK_QSYNTH_STD)) {
    if(qdsp_cmd_3d_pos_query_src_revol(channel, &qdsp_vol) == QDSP_CMD_SUCCESS) {
      *volume = voc_data_compute_mb_from_abs(qdsp_vol);
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}
#endif /* FEATURE_QAUDIOFX */

/* <EJECT> */
#if defined(FEATURE_EXTERNAL_SDAC) || defined (MSMAUD_CODEC_CLOCK_CTRL)
/*===========================================================================

FUNCTION voc_check_codec_config

DESCRIPTION
  This function verifies if the codec config command has been executed; if
  so, it will disable the indicated clocks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_check_codec_config ( uint32 clk_mask )
{
  if((voc_state_get_inform_val() == VOC_INFORM_RESET) ||
     qdsp_cmd_check_cmd_reg()) {
#ifdef MSMAUD_CODEC_CLOCK_CTRL
    if(clk_mask & VOC_CLK_CTRL_INTERNAL_CODEC) {
      voc_state_disable_codec_clk();
    }
#endif /* MSMAUD_CODEC_CLOCK_CTRL */
#ifdef FEATURE_EXTERNAL_SDAC
    if(clk_mask & VOC_CLK_CTRL_STEREO_DAC) {
      voc_state_config_external_sdac(VOC_SAMPLE_RATE_NONE);
    }
#endif /* FEATURE_EXTERNAL_SDAC */
    return(TRUE);
  }
  return(FALSE);
}
#endif /* FEATURE_EXTERNAL_SDAC || MSMAUD_CODEC_CLOCK_CTRL */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_retry_codec_select

DESCRIPTION
  This function configures to the current codec.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_retry_codec_select ( void )
{
  voc_codec_type              pcm_path;

  voc_data_get_codec(&pcm_path);
  voc_codec_select(pcm_path);

} /* voc_retry_codec_select */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_check_codec_clk_change

DESCRIPTION
  checks if there is a change in the device from external to internal,
  if change is found sets the device_changed variable of vocoder state data.

DEPENDENCIES
None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_check_codec_clk_change
(
  voc_codec_type prev_codec,
  voc_codec_type curr_codec
)
{

  if ( ( voc_aux_pcm_clk_required( prev_codec ) ) && \
      !( voc_aux_pcm_clk_required( curr_codec ) ) )
  {
    voc_data_set_eclock_switched(TRUE);
  }
  else
  {
    voc_data_set_eclock_switched(FALSE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_aux_pcm_clk_required

DESCRIPTION
  This function checks if AUX PCM clock is required by the codec .

DEPENDENCIES
  None

RETURN VALUE
  TRUE: required   FALSE: not required

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_aux_pcm_clk_required(voc_codec_type codec)
{

  switch (codec)
  {
#ifdef FEATURE_BT
#ifdef FEATURE_BT_AG
    case VOC_CODEC_BT_AG:
#endif  /* FEATURE_BT_AG */
#endif  /* FEATURE_BT */
#ifdef FEATURE_TTY
    case VOC_CODEC_TTY_OFF_BOARD:
#endif
    case VOC_CODEC_OFF_BOARD:
    case VOC_CODEC_ON_BOARD:
         return TRUE;

    default:
    break;
  }

  return FALSE;
}




/* <EJECT> */
/*===========================================================================

FUNCTION voc_codec_select

DESCRIPTION
  This function selects which codec the vocoder will use.

DEPENDENCIES
  The command will be sent to the DSP only when the DSP is in IDLE, DFM
  or VOCODER state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_codec_select ( voc_codec_type codec )
{
  voc_codec_type             pcm_path;
  voc_codec_class_type       codec_type;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  qdsp_cmd_status_type       result = QDSP_CMD_FAILED;
  voc_state_control_type     *state;
#ifdef MSMAUD_ADIE_CODEC
  boolean                     adie_needed;
#endif /* MSMAUD_ADIE_CODEC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  boolean                     is_stereo_tx_active_codec;
#endif


#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  qdsp_mixer_mode_type          mixer_mode;
  audadsp_mixer_state_entity_state_type entity_state;
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifdef FEATURE_EXTERNAL_SDAC
  static boolean              sdac_timer_flag = FALSE;
  voc_codec_class_type        old_codec_type;

  voc_data_get_codec_class(&old_codec_type);
#endif

#ifdef SND_TEST_CODEC_OVERRIDE
  /* Check if the override flag is set for this codec or not? */
  if(voc_codec_override_flag_table[codec] == TRUE)
  {
	/* codec override flag is set for this codec so
	   override the codec with the one in override table */
    codec = voc_codec_override_table[codec];
  }
#endif // SND_TEST_CODEC_OVERRIDE

  /* Clear the codec retry counter */
  voc_set_codec_retry_timer(FALSE);

#ifdef MSMAUD_QSC60X5_AUDIO_HW
  #if defined(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)
    if (codec == VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET)
      MSMAUD_ENABLE_EXTERNAL_SADC_CLOCKS(CLK_RGM_MISC_ENA_I2S_CLK_DIV_SEL,CLKRGM_SDAC_CLK_EXT_CLK);
  #endif  /*(FEATURE_AVS_CRYSTAL_SPEECH) && defined(FEATURE_EXTERNAL_SADC)*/
#endif /*MSMAUD_QSC60X5_AUDIO_HW*/

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  is_stereo_tx_active_codec = voc_data_is_stereo_tx_codec();
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

#ifdef FEATURE_BT_AG
  /* FM over BT AG */
  if (voc_aux_line_in_flag == TRUE)
  {
    if (codec == VOC_CODEC_BT_AG)
    {
      MSG_MED("FM is on and Codec in use is BT Headset so "
                      "changing current Coded to FM over BT AG codec",
                                                                0, 0, 0);
      codec = VOC_CODEC_FM_OVER_BT_AG;
    }
  }
#endif /* FEATURE_BT_AG */
  /* Set the configuration for this type of codec
  */
  voc_data_set_codec(codec);
  voc_state_set_codec_cal(codec);

  /* enable eclock if aux pxm interface is being used*/
  if (voc_state_aux_pcm_clk_query()) {
    voc_state_enable_eclock();
  }
#ifdef FEATURE_BT_AG
  /* FM over BT AG */
  if (voc_aux_line_in_flag == TRUE)
  {
    switch(codec)
    {
      case VOC_CODEC_FM_OVER_BT_AG:

        MSG_MED("FM is on and codec settings changed to BT so"
                " acquiring vocoder ", 0, 0, 0);
        /* Acquiring vocoder to bring DSP up */
        voc_process_fm_over_bt(VOC_AUX_LINE_IN_MODE_ON);
        break;

      default:
        MSG_MED("FM is on and codec settings changed so reconfiguring ADIE"
                " And releasing vocoder ",
                0, 0, 0);
        /* Releasing vocoder to bring DSP down */
        voc_process_fm_over_bt(VOC_AUX_LINE_IN_MODE_OFF);
        /* configuring adie to appropriate device */
        voc_process_aux_line_in(VOC_AUX_LINE_IN_MODE_ON);
        break;
    }
  }
#endif /* FEATURE_BT_AG */

  /* Get the current state data
  */
  state = voc_state_get_state_data();


#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  if (check_interlock(VOC_UNLOCK_MODEM)
#ifdef FEATURE_VOC_ADPCM
      || check_interlock(VOC_UNLOCK_VOCODER_ADPCM)
#endif /* FEATURE_VOC_ADPCM */
#ifdef FEATURE_AVS_INCALL_ADEC_PB
      || ((check_interlock(VOC_UNLOCK_MIXER_ADEC))&&(state->mixer_vocoder_active_flag == TRUE))
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
      || (state->state == VOC_STATE_VOCODER_ENTRY)
     )
  {
    /* do nothing and wait for pending request to be served.
       no need to check codec changes.
    */
    if (state->codec_pending == TRUE)
    {
      MSG_HIGH("QDSP2 Codec setting saved = %d", codec, 0, 0);
      return;
    }


    /* this won't cause problem because state transition guarantees
       codec re-configuration.  as a result, this checking will be
       one time only in the beginning of the whole reconfig. sequence.
    */
    if (is_stereo_tx_active_codec != voc_data_is_stereo_tx_codec())
    {
      /* delay codec reconfiguration until idle command is issued */
      state->codec_pending = TRUE;
      (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

      MSG_HIGH("QDSP2 Codec setting saved = %d", codec, 0, 0);
      return;
    }
  }
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

  state->codec_pending = FALSE;


  /* Make any changes based on current settings
  */
  if (check_interlock(VOC_UNLOCK_NOT_RESET)) {

#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
    if (voc_data_get_pp_func() != NULL)
    {
      qdsp_cmd_pp_reset();
    }
#endif/* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

    /* Issue codec configuration command */
    voc_data_get_codec(&pcm_path);
    voc_data_get_codec_class(&codec_type);

    switch (codec_type) {
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
#ifdef FEATURE_BT_AG
      case VOC_CODEC_CLASS_BLUETOOTH:
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
      case VOC_CODEC_CLASS_BLUETOOTH_A2DP:
      case VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO:
#endif /* FEATURE_SBC_CODEC */
#ifdef FEATURE_PHONE_VR
        if (state->vr_mode != VOC_VR_MODE_BC)
#endif
          {
            if (voc_bt_state != VOC_BT_STATE_DISABLED) {
              voc_data_get_codec_config(state->current_image, pcm_path,
                                        &codec_config_ptr);
            } else {
              voc_data_get_codec_config(state->current_image, VOC_CODEC_DEFAULT,
                                        &codec_config_ptr);
            }
        }
#ifdef FEATURE_PHONE_VR
        else if (state->vr_mode == VOC_VR_MODE_BC)  {
          voc_data_get_codec_config(state->current_image, pcm_path,
                                      &codec_config_ptr);
        }
#endif
        break;
#endif /* if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) */
      default:
      voc_data_get_codec_config(state->current_image, pcm_path,
                                &codec_config_ptr);
        break;
    }

    /* Disable any registered codec functions */
    voc_disable_codec_func(FALSE);

#ifdef FEATURE_EXTERNAL_SDAC
    if((codec_type != VOC_CODEC_CLASS_EXTERNAL_STEREO) && (old_codec_type == VOC_CODEC_CLASS_EXTERNAL_STEREO)) {
      (void) sdac_enable(FALSE);
      sdac_timer_flag = TRUE;
    }
#endif /* FEATURE_EXTERNAL_SDAC */

#ifdef MSMAUD_ADIE_CODEC
    adie_needed = voc_state_adie_wakeup();
#endif /* MSMAUD_ADIE_CODEC */

#ifdef FEATURE_QAUDIOFX
    if(voc_data_get_3d_pos_enable_mode() == VOC_3D_POS_MODE_ENABLE) {
      voc_3d_pos_enable(VOC_3D_POS_MODE_ENABLE);
    }
#endif /* FEATURE_QAUDIOFX */

    /* For incall audio cases, the voc_state_adie_wakeup() brings
     * the PCM clock down to 8KHz, here we need to bump it
     * up back in case BT link is not there (adie_needed is
     * FALSE)
     */
#ifdef QDSP_codecCmdProcessed
    if(state->device_changed==TRUE)
    {
      qdsp_write(QDSP_codecCmdProcessed, (uint32)1);
    }
#endif
    switch (state->current_image) {
#ifdef FEATURE_AVS_INCALL_ADEC_PB
#ifdef QDSP_IMAGE_VOC_MP3_DEFINED
      case QDSP_IMAGE_VOC_MP3:
#endif /* QDSP_IMAGE_VOC_MP3_DEFINED */
#ifdef QDSP_IMAGE_VOC_EVRC_MP3_DEFINED
      case QDSP_IMAGE_VOC_EVRC_MP3:
#endif /* QDSP_IMAGE_VOC_EVRC_MP3_DEFINED */
#ifdef QDSP_IMAGE_AMR_MP3_DEFINED
      case QDSP_IMAGE_AMR_MP3:
#endif /* QDSP_IMAGE_AMR_MP3_DEFINED */
#ifdef QDSP_IMAGE_EFR_MP3_DEFINED
      case QDSP_IMAGE_EFR_MP3:
#endif /* QDSP_IMAGE_EFR_MP3_DEFINED */
#ifdef QDSP_IMAGE_FR_MP3_DEFINED
      case QDSP_IMAGE_FR_MP3:
#endif /* QDSP_IMAGE_FR_MP3_DEFINED */
#ifdef QDSP_IMAGE_HR_MP3_DEFINED
      case QDSP_IMAGE_HR_MP3:
#endif /* QDSP_IMAGE_HR_MP3_DEFINED */
#ifdef MSMAUD_ADIE_CODEC
        if (adie_needed) {
          #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
          #else
          voc_state_set_icodec_clk(VOC_SAMPLE_RATE_48000);
          #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
        }
#endif /* MSMAUD_ADIE_CODEC */
        result = qdsp_cmd_codec_config(codec_config_ptr,\
                 voc_data_get_default_frame_rate(state->current_image,codec));
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
        break;
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)

#if defined(QDSP_IMAGE_VOC_COMBO_ICA_DEFINED) || \
    defined(QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED)
#ifdef QDSP_IMAGE_VOC_COMBO_ICA_DEFINED
      case QDSP_IMAGE_VOC_COMBO_ICA:
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
      case QDSP_IMAGE_VOC_COMBO_SYNTH:
#endif
#ifdef MSMAUD_ADIE_CODEC
        if (adie_needed) {
          #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
          #else
          voc_state_set_icodec_clk(VOC_SAMPLE_RATE_48000);
          #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
        }
#endif /* MSMAUD_ADIE_CODEC */

        mixer_mode = audadsp_mixer_state_get_mixer_mode(&entity_state);
        /* We should  issue the codec config command while in the mixer
           exit sequence after checking which entity MIXER/ADEC/VOCODER
           is active.*/
        if (mixer_mode != QDSP_CMD_MIXER_MODE_NONE) {
          if (state->exit_mixer_state== TRUE){
            if (qdsp_cmd_get_state() == QDSP_STATE_INIT){
              mixer_mode &=  (~(1<<QDSP_CMD_MIXER_VOC_INDEX));
              MSG_HIGH("VOCODER in INIT state mixer_mode =0x%x",mixer_mode,0,0);
            }
          if (qdsp_cmd_get_adec_state()== QDSP_STATE_INIT){
            mixer_mode &=(~(1<<QDSP_CMD_MIXER_ADEC0_INDEX));
            MSG_HIGH(" ADEC in INIT state mixer_mode =0x%x",mixer_mode,0,0);
          }
        }
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
          /* Issue mixer codec configuration command */
          result = qdsp_cmd_mixer_codec_config(mixer_mode, codec_config_ptr);
        }
#endif /* QDSP_IMAGE_VOC_COMBO_ICA_DEFINED ||
          QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */
        break;
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB ||
          FEATURE_AVS_INCALL_MIXER_CMX */

      default:
       result = qdsp_cmd_codec_config(codec_config_ptr,\
                 voc_data_get_default_frame_rate(state->current_image,codec));
#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */
        break;
    } /* switch */

    if (result == QDSP_CMD_SUCCESS) {
      MSG_HIGH("QDSP2 Codec Configuration = %d", codec, 0, 0);

      /* Turn analog part of Adie */
      #ifdef MSMAUD_SCMM
#error code not present
      #endif /* MSMAUD_SCMM */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
      switch(voc_bt_state) {
#ifdef FEATURE_BT_AG
        case VOC_BT_STATE_AG_CODEC_CONFIG_PENDING:
          voc_bt_state = VOC_BT_STATE_AG_ENABLED;
          MSG_HIGH("BT SCO connected", 0,0,0);
          if (voc_bt_ag_link_enabled_cb != NULL)
          {
            voc_bt_ag_link_enabled_cb();
          }
          break;
#endif
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
        case VOC_BT_STATE_A2DP_CODEC_CONFIG_PENDING:
          voc_bt_state = VOC_BT_STATE_A2DP_SBC_CONFIG_PENDING;
          /* Lets go ahead and issue SBC config */
          voc_complete_bt_a2dp();
          break;
#endif
       default:
          break;
      }
#endif

#ifdef FEATURE_EXTERNAL_SDAC
      if(sdac_timer_flag) {
        voc_set_clk_ctrl_timer(TRUE, VOC_CLK_CTRL_STEREO_DAC);
        sdac_timer_flag = FALSE;
      }
#endif /* FEATURE_EXTERNAL_SDAC */

#ifdef MSMAUD_CODEC_CLOCK_CTRL
#ifdef MSMAUD_ADIE_CODEC
      voc_set_clk_ctrl_timer(!adie_needed, VOC_CLK_CTRL_INTERNAL_CODEC);
#endif /* MSMAUD_ADIE_CODEC */
#endif /* MSMAUD_CODEC_CLOCK_CTRL */

      /* Setup audio format pro-processing calibration blocks
      */
      voc_set_audfmt_pp_cal();

	  /* write the codec gains here. This function call is addeed to write
      ** the codec gains when device is changed during
      ** playback is active. */
      (void)voc_state_write_codec_gain();

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
#ifdef FEATURE_AUDFMT_QCONCERT
      voc_update_qconcert_config();
#endif /* FEATURE_AUDFMT_QCONCERT */

      /* Enable registered codec function */
      voc_enable_codec_func();

#ifdef FEATURE_EXTERNAL_SDAC
      if ((codec_type == VOC_CODEC_CLASS_EXTERNAL_STEREO)
#ifdef QDSP_IMAGE_VOC_DEFINED
          && (state->current_image != QDSP_IMAGE_VOC)
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_DEFINED
          && (state->current_image != QDSP_IMAGE_VOC_COMBO)
#endif /* QDSP_IMAGE_VOC_COMBO_DEFINED */
#ifdef FEATURE_AMR_FIXED
          && (state->current_image != QDSP_IMAGE_AMR)
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED
          && (state->current_image != QDSP_IMAGE_VOC_COMBO_VOIP)
#endif /* QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED */
#ifdef QDSP_IMAGE_VOC_EVRC_DEFINED
          && (state->current_image != QDSP_IMAGE_VOC_EVRC)
#endif /* QDSP_IMAGE_VOC_EVRC_DEFINED */
#ifdef QDSP_IMAGE_VOC_E90_DEFINED
          && (state->current_image != QDSP_IMAGE_VOC_E90)
#endif /* QDSP_IMAGE_VOC_E90_DEFINED */
#ifdef QDSP_IMAGE_VOC_EVRC_E90_DEFINED
          && (state->current_image != QDSP_IMAGE_VOC_EVRC_E90)
#endif /* QDSP_IMAGE_VOC_EVRC_E90_DEFINED */
#ifdef QDSP_IMAGE_VOC_QTV_DEFINED
          && (state->current_image != QDSP_IMAGE_VOC_QTV)
#endif /* QDSP_IMAGE_VOC_QTV_DEFINED */
#ifdef QDSP_IMAGE_FR_DEFINED
#error code not present
#endif /* QDSP_IMAGE_FR_DEFINED */
#ifdef QDSP_IMAGE_EFR_DEFINED
#error code not present
#endif /* QDSP_IMAGE_EFR_DEFINED */
#ifdef QDSP_IMAGE_AMR_DEFINED
          && (state->current_image != QDSP_IMAGE_AMR)
#endif /* QDSP_IMAGE_AMR_DEFINED */
#ifdef QDSP_IMAGE_AMR_WB_DEFINED
          && (state->current_image != QDSP_IMAGE_AMR_WB)
#endif /* QDSP_IMAGE_AMR_WB_DEFINED */
#ifdef QDSP_IMAGE_HR_DEFINED
          && (state->current_image != QDSP_IMAGE_HR)
#endif /* QDSP_IMAGE_HR_DEFINED */
#ifdef QDSP_IMAGE_4GV_DEFINED
          && (state->current_image != QDSP_IMAGE_4GV)
#endif /* QDSP_IMAGE_4GV_DEFINED */
#ifdef QDSP_IMAGE_4GV_WB_DEFINED
          && (state->current_image != QDSP_IMAGE_4GV_WB)
#endif /* QDSP_IMAGE_4GV_WB_DEFINED */
#ifdef QDSP_IMAGE_4GV_VOIP_DEFINED
          && (state->current_image != QDSP_IMAGE_4GV_VOIP)
#endif /* QDSP_IMAGE_4GV_VOIP_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED
          && (state->current_image != QDSP_IMAGE_QCAMCORDER_COMBO)
#endif /*QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED*/
         ) {
        /* SDAC: set CODEC_CORE_ENABLE */
        voc_set_clk_ctrl_timer(FALSE, VOC_CLK_CTRL_STEREO_DAC);
        voc_state_config_external_sdac(voc_data_get_sample_rate());
        (void) sdac_enable(TRUE);
      }
#endif /* FEATURE_EXTERNAL_SDAC */

      if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
        /* Initialize the audio front end */
        if (voc_state_init_afe() != VOC_STATE_SUCCESS) {
           MSG_ERROR("Failed to initialize afe", 0, 0, 0);
        }
      }
    } else {
      MSG_HIGH("QDSP2 Codec Configuration = %d Error = %d", codec, result, 0);
      voc_set_codec_retry_timer(TRUE);
    }
  } else {
    MSG_HIGH("QDSP2 Codec setting saved = %d", codec, 0, 0);
  }
  (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

} /* voc_codec_select */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_vol_cmd

DESCRIPTION
  This function sets the vocoder RX gain.  The volume will be saved if
  the DSP is not in VOCODER or DFM state.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - Command completed succesfully

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_vol_cmd ( sint15 volume )
{
  qdsp_cmd_status_type  result;

  /* Save the voice volume setting
  */
  voc_data_set_rx_volume(volume);

  if (check_interlock(VOC_UNLOCK_MODEM)
#ifdef FEATURE_VOC_PCM_INTERFACE
      || (voc_state_get_state_data()->pcm_int_mode_pending &&
       check_interlock(VOC_UNLOCK_VOC_NOT_RESET))
#endif
#ifdef FEATURE_VOC_ADPCM
      || (check_interlock(VOC_UNLOCK_VOCODER_ADPCM))
#endif
#ifdef FEATURE_AVS_INCALL_ADEC_PB
      || (check_interlock(VOC_UNLOCK_VOCODER_ADEC))
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
      || (check_interlock(VOC_UNLOCK_MIXER_ADEC))
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
  ) {
    result = qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL));

    if (result == QDSP_CMD_SUCCESS) {
      return(VOC_DONE_S);
    } else {
      MSG_HIGH("Vocoder volume command error = %d", result, 0, 0);
    }
  } else {
    MSG_HIGH("Volume Setting Saved = %d", volume, 0, 0);
  }
  return(VOC_DONE_S);
} /* voc_vol_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_frame_cmd

DESCRIPTION
  This function sets the rate of the next frame for the vocoder.

DEPENDENCIES
  voc_rate_conv[]

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  voc_enc_settings

===========================================================================*/
voc_status_type voc_frame_cmd
(
  voc_rate_type max_rate,      /* max rate for vocoder next frame */
  voc_rate_type min_rate       /* min rate for vocoder next frame */
)
{
  qdsp_cmd_status_type     result;

  /* If min or max rate is "blank", blank the next frame.
  */
  if ( (max_rate == VOC_0_RATE) || (min_rate == VOC_0_RATE) ) {
    max_rate = VOC_0_RATE;
    min_rate = VOC_0_RATE;
  }

  /* Check for illegal rates
  */
  if ( (min_rate > VOC_1_RATE) || (max_rate > VOC_1_RATE) ) {
    ERR( "Frame Illegal rates", 0, 0, 0 );
    min_rate = max_rate = VOC_8_RATE;
  }

  /* indicate signalling */
  if (max_rate != VOC_1_RATE) {
    MSG_MED( "MAX RATE LIMIT %d", max_rate, 0, 0 );
  }

  voc_enc_setting.enc_min_rate = voc_rate_conv[min_rate];
  voc_enc_setting.enc_max_rate = voc_rate_conv[max_rate];

  if (check_interlock(VOC_UNLOCK_VOCODER)) {

    result = qdsp_cmd_enc_ctrl(&voc_enc_setting);

    if (result == QDSP_CMD_SUCCESS) {
      return(VOC_DONE_S);
    } else {
      MSG_HIGH("QDSP2 Vocoder enc_ctrl cmd error = %d", result, 0, 0);
    }
  } else {
    MSG_HIGH("QDSP2 Vocoder frame rates saved", 0, 0, 0);
  }
  return(VOC_DONE_S);
} /* voc_frame_cmd */

/* <EJECT> */
#ifdef FEATURE_VOC_PACKET_INTERFACE
/*===========================================================================

FUNCTION VOC_SET_FRAME_RATE_CMD

DESCRIPTION
  This function sets the rate of the next frame for the vocoder.

DEPENDENCIES
  voc_rate_conv[]

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  voc_enc_settings

===========================================================================*/
voc_status_type voc_set_frame_rate_cmd
(
  voc_rate_type max_rate,      /* max rate for vocoder next frame */
  voc_rate_type min_rate       /* min rate for vocoder next frame */
)
{
  qdsp_cmd_status_type     result;

  /* If min or max rate is "blank", blank the next frame.
  */
  if ( (max_rate == VOC_0_RATE) || (min_rate == VOC_0_RATE) ) {
    max_rate = VOC_0_RATE;
    min_rate = VOC_0_RATE;
  }

  /* Check for illegal rates
  */
  if ( (min_rate > VOC_1_RATE) || (max_rate > VOC_1_RATE) ) {
    ERR( "Frame Illegal rates", 0, 0, 0 );
    min_rate = max_rate = VOC_8_RATE;
  }

  /* indicate signalling */
  if (max_rate != VOC_1_RATE) {
    MSG_MED( "MAX RATE LIMIT %d", max_rate, 0, 0 );
  }

  voc_enc_setting.enc_min_rate = voc_rate_conv[min_rate];
  voc_enc_setting.enc_max_rate = voc_rate_conv[max_rate];

  if (check_interlock(VOC_UNLOCK_VOCODER)) {

    result = qdsp_cmd_frame_rate_ctrl(&voc_enc_setting);

    if (result == QDSP_CMD_SUCCESS) {
      return(VOC_DONE_S);
    } else {
      MSG_HIGH("QDSP2 Vocoder enc_ctrl cmd error = %d", result, 0, 0);
    }
  } else {
    MSG_HIGH("QDSP2 Vocoder frame rates saved", 0, 0, 0);
  }
  return(VOC_DONE_S);
} /* voc_set_frame_rate_cmd */
#endif /* FEATURE_VOC_PACKET_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_tx_cmd

DESCRIPTION
  This function gets the current vocoder frame and sets the rate for the
  next frame.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  Data rate for current vocoder frame.

SIDE EFFECTS
  None

===========================================================================*/
voc_rate_type voc_tx_cmd
(
  voc_rate_type max_rate,             /* Max rate for vocoder next frame */
  voc_rate_type min_rate,             /* Min rate for vocoder next frame */
  uint8 **data_ptr                    /* Pointer to pointer to data      */
)
{
  voc_rate_type  ret_val=VOC_0_RATE;
  uint16         qdsp_min_rate;
  uint16         qdsp_max_rate;
  uint16         qdsp_rate;

  ASSERT( data_ptr != NULL );

  /* Check for illegal rates
  */
  if ( (min_rate > VOC_1_RATE) || (max_rate > VOC_1_RATE) ) {
    ERR( "TX Illegal rates", 0, 0, 0 );
    min_rate = max_rate = VOC_8_RATE;
  }

  qdsp_min_rate = voc_rate_conv[min_rate];
  qdsp_max_rate = voc_rate_conv[max_rate];
  qdsp_rate = qdsp_cmd_tx_frame(qdsp_max_rate, qdsp_min_rate, data_ptr);

  switch (qdsp_rate) {
    case QDSP_RATE_BLANK_V:
      ret_val = VOC_0_RATE;
      break;
    case QDSP_RATE_EIGHTH_V:
      ret_val = VOC_8_RATE;
      break;
    case QDSP_RATE_QUARTER_V:
      ret_val = VOC_4_RATE;
      break;
    case QDSP_RATE_HALF_V:
      ret_val = VOC_2_RATE;
      break;
    case QDSP_RATE_FULL_V:
      ret_val = VOC_1_RATE;
      break;
    default:
      MSG_HIGH ("voc_tx_cmd: Invalid qdsp rate: %x",qdsp_rate,0,0);
      ret_val = VOC_ERR_RATE;
      break;
  }

  return( ret_val );
} /* voc_tx_cmd */

/* <EJECT> */

#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_AMR_WB_COMMON)

/*===========================================================================

FUNCTION VOC_SET_GSM_ENC_DTX

DESCRIPTION
  This function will turn encoder DTX on or off.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_set_gsm_enc_dtx(
  voc_gsm_dtx_mode_type dtx_mode
)
{
 qdsp_cmd_status_type qdsp_status = QDSP_CMD_SUCCESS;


 if ((dtx_mode != VOC_GSM_DTX_OFF) && (dtx_mode != VOC_GSM_DTX_ON))
 {
  return VOC_BADFMT_S;
 }


 /* Save the DTX mode setting */
 voc_data_set_gsm_dtx_mode( dtx_mode );

 /* check current image and state */
 if ((check_interlock(VOC_UNLOCK_NOT_RESET) == TRUE)
     && ((voc_get_curr_config()
          & (VOC_CAP_AMR | VOC_CAP_GSM_EFR | VOC_CAP_GSM_FR | VOC_CAP_GSM_HR
             | VOC_CAP_AMR_WB)
         ) != 0))
 {
   dtx_mode = voc_data_get_gsm_dtx_mode();

   if (dtx_mode == VOC_GSM_DTX_OFF)
   {
    qdsp_status = qdsp_cmd_set_gsm_enc_dtx(QDSP_GSM_DTX_OFF);
   }
   else
       {
        qdsp_status = qdsp_cmd_set_gsm_enc_dtx(QDSP_GSM_DTX_ON);
       }
 }


 if (qdsp_status == QDSP_CMD_SUCCESS)
 {
  return VOC_DONE_S;
 }
 else
     {
      return VOC_CMD_FAILED_S;
     }
}
#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR || MSMAUD_VOC_HR || MSMAUD_VOC_AMR_WB_COMMON */


/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_WB_COMMON
/*===========================================================================

FUNCTION VOC_INIT_AMR_WB_TX_RATE

DESCRIPTION
  This function sets the initial encoder rate for the AMR_WB vocoder.
  Maybe used between frames to set the encoder rate.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_init_amr_wb_tx_rate(
  voc_amr_wb_frame_rate_type amr_wb_tx_rate
)
{
  qdsp_cmd_amr_wb_frame_rate_type qdsp_amr_wb_frame_rate
                                  = QDSP_AMR_WB_RATE_1265;
  qdsp_cmd_status_type qdsp_status = QDSP_CMD_SUCCESS;

  if ((amr_wb_tx_rate < VOC_AMR_WB_RATE_0660)
      || (amr_wb_tx_rate > VOC_AMR_WB_RATE_2385))
  {
    return VOC_BADFMT_S;
  }


  /* Save the initial AMR_WB Tx rate setting */
  voc_data_set_amr_wb_init_rate(amr_wb_tx_rate);

  /* check current image and state */
  if ((check_interlock(VOC_UNLOCK_NOT_RESET) == TRUE) &&
      ((voc_get_curr_config() == VOC_CAP_AMR_WB)
     ))
  {
     amr_wb_tx_rate = voc_data_get_amr_wb_init_rate();

     voc_to_qdsp_amr_wb_frate(amr_wb_tx_rate, &qdsp_amr_wb_frame_rate);
     qdsp_status = qdsp_cmd_set_amr_wb_codec_mode(qdsp_amr_wb_frame_rate);
  }

  if (qdsp_status == QDSP_CMD_SUCCESS)
  {
    return VOC_DONE_S;
  }
  else
  {
    return VOC_CMD_FAILED_S;
  }
}
#endif /* MSMAUD_VOC_AMR_WB_COMMON */

/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_COMMON
/*===========================================================================

FUNCTION VOC_INIT_AMR_TX_RATE

DESCRIPTION
  This function sets the initial encoder rate for the AMR vocoder.
  Maybe used between frames to set the encoder rate.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_init_amr_tx_rate(
  voc_gsm_frame_rate_type amr_tx_rate
)
{
  qdsp_cmd_gsm_frame_rate_type qdsp_amr_frame_rate = QDSP_GSM_FULL_RATE;
  qdsp_cmd_status_type qdsp_status = QDSP_CMD_SUCCESS;

#if (defined(FEATURE_AVS_INCALL_ADEC_PB) && \
     defined(QDSP_MODULE_TUNES_INCALL_AMR_DEFINED)) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  voc_state_control_type        *state;
#endif

  if ((amr_tx_rate < VOC_AMR_RATE_475) || (amr_tx_rate > VOC_AMR_RATE_1220))
  {
    return VOC_BADFMT_S;
  }


  /* Save the initial AMR Tx rate setting */
  voc_data_set_amr_init_rate(amr_tx_rate);

#if (defined(FEATURE_AVS_INCALL_ADEC_PB) && \
     defined(QDSP_MODULE_TUNES_INCALL_AMR_DEFINED)) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  state = voc_state_get_state_data();
#endif

  /* check current image and state */
  if ((check_interlock(VOC_UNLOCK_NOT_RESET) == TRUE) &&
      ((voc_get_curr_config() == VOC_CAP_AMR)
#if defined(FEATURE_AVS_INCALL_ADEC_PB) && \
    defined(QDSP_MODULE_TUNES_INCALL_AMR_DEFINED)
      || ((state->audfmt_mix_mode == VOC_ADEC_MIX_MODE_RX)   &&
          (state->state           == VOC_STATE_ADEC_VOCODER) &&
          (voc_get_desired_config(VOC_ACQ_MVS) == VOC_CAP_AMR))
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
      || ((state->audfmt_mix_mode == VOC_ADEC_MIX_MODE_RX)   &&
          (state->state           == VOC_STATE_MIXER_ACTIVE) &&
          (voc_get_desired_config(VOC_ACQ_MVS) == VOC_CAP_AMR))
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
     ))
  {
     amr_tx_rate = voc_data_get_amr_init_rate();

     voc_to_qdsp_gsm_frate(amr_tx_rate, &qdsp_amr_frame_rate);
     qdsp_status = qdsp_cmd_set_amr_codec_mode(qdsp_amr_frame_rate);
  }


  if (qdsp_status == QDSP_CMD_SUCCESS)
  {
    return VOC_DONE_S;
  }
  else
  {
    return VOC_CMD_FAILED_S;
  }
}
#endif /* MSMAUD_VOC_AMR_COMMON */

/* <EJECT> */
#if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)
/*===========================================================================

FUNCTION VOC_GSM_TX_PACKET

DESCRIPTION
  This function gets the current GSM vocoder reverse-link packet and sets
  the rate for the next encoder packet if it is in AMR mode.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  data_ptr : hold the packet data.
  frame_type : frame type of retrieved encoder packet.
  frame_rate : hold the frame rate of retrieved encoder packet.
  voc_status_type : VOC_DONE_S or VOC_CMD_FAILED_S or VOC_BADFMT_S

SIDE EFFECTS
  Rate of next packet is set if it is in AMR mode.

===========================================================================*/
voc_status_type voc_gsm_tx_packet(
  uint16 *data_ptr,
  voc_gsm_frame_type *frame_type,
  voc_gsm_frame_rate_type *frame_rate,
  voc_gsm_frame_rate_type next_frame_rate                 /* only for AMR */
)
{
 qdsp_cmd_gsm_frame_type qdsp_gsm_frame_id;
 qdsp_cmd_gsm_frame_rate_type qdsp_gsm_frame_rate;
 qdsp_cmd_gsm_frame_rate_type qdsp_amr_next_frame_rate = QDSP_GSM_FULL_RATE;


 if ((data_ptr==NULL) || (frame_type==NULL) || (frame_rate==NULL))
 {
  return VOC_BADFMT_S;
 }


 /* mapping next_frame_rate into QDSP driver level format */
 voc_to_qdsp_gsm_frate(next_frame_rate, &qdsp_amr_next_frame_rate);

 /* next_frame_rate with garbage values will fail here if it is in AMR mode.
 ** garbage values will be ignored in FR and EFR modes.
 */
 if (qdsp_cmd_gsm_tx_packet(data_ptr, &qdsp_gsm_frame_id, &qdsp_gsm_frame_rate,
                            qdsp_amr_next_frame_rate) != QDSP_CMD_SUCCESS)
 {
  return VOC_CMD_FAILED_S;
 }


 /* mapping QDSP returned values into Vocoder driver level format */
 if (qdsp_to_voc_gsm_ftype(qdsp_gsm_frame_id, frame_type)
     != VOC_DONE_S)
 {
   return VOC_CMD_FAILED_S;
 }


 if (qdsp_to_voc_gsm_frate(qdsp_gsm_frame_rate, frame_rate)
     != VOC_DONE_S)
 {
  return VOC_CMD_FAILED_S;
 }


 return VOC_DONE_S;
}


/* <EJECT> */
/*===========================================================================

FUNCTION VOC_GSM_RX_PACKET

DESCRIPTION
  This function transfers one forward-link packet to the GSM vocoder.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_gsm_rx_packet(
  uint16 *data_ptr,
  voc_gsm_frame_type frame_type,
  voc_gsm_frame_rate_type frame_rate,          /* only for AMR */
  boolean taf                /* time alignment flag, only for GSM FR or EFR */
)
{
 qdsp_cmd_gsm_frame_type qdsp_gsm_frame_id;
 qdsp_cmd_gsm_frame_rate_type qdsp_amr_frame_rate = QDSP_GSM_FULL_RATE;


 if (data_ptr==NULL)
 {
  return VOC_BADFMT_S;
 }


 /* mapping parameters into QDSP driver level format */
 if (voc_to_qdsp_gsm_ftype(frame_type, &qdsp_gsm_frame_id)
     != VOC_DONE_S)
 {
  return VOC_BADFMT_S;
 }

 voc_to_qdsp_gsm_frate(frame_rate, &qdsp_amr_frame_rate);


 /* frame_rate with garbage values will fail here if it is in AMR mode.
 ** garbage values will be ignored in FR and EFR modes.
 */
 if (qdsp_cmd_gsm_rx_packet(data_ptr, qdsp_gsm_frame_id, qdsp_amr_frame_rate,
                            taf)
     == QDSP_CMD_SUCCESS)
 {
  return VOC_DONE_S;
 }
 else
     {
       return VOC_CMD_FAILED_S;
     }
}


#endif  /* MSMAUD_VOC_AMR || MSMAUD_VOC_EFR || MSMAUD_VOC_FR */

#ifdef MSMAUD_VOC_G711
/*===========================================================================

FUNCTION VOC_SET_G711_MODE

DESCRIPTION
  This function sets the encoder mode for the G.711 vocoder.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_set_g711_mode(
  voc_g711_mode_type g711_mode
)
{
  qdsp_cmd_g711_mode_type  qdsp_g711_mode = QDSP_G711_MODE_MULAW;
  qdsp_cmd_status_type     qdsp_status    = QDSP_CMD_SUCCESS;


  if ((g711_mode < VOC_G711_MODE_MULAW) || (g711_mode > VOC_G711_MODE_ALAW)) {
    return VOC_BADFMT_S;
  }

  /* Save the initial G.711 mode setting */
  voc_data_set_g711_mode(g711_mode);

  /* check current image and state */
  if ((check_interlock(VOC_UNLOCK_NOT_RESET) == TRUE) &&
     (voc_get_curr_config() == VOC_CAP_G711)) {
    /* Get the current G.711 mode */
    g711_mode = voc_data_get_g711_mode();

    /* Convert the vocoder mode to QDSP mode */
    switch(g711_mode) {
      case VOC_G711_MODE_MULAW:
        qdsp_g711_mode = QDSP_G711_MODE_MULAW;
        break;
      case VOC_G711_MODE_ALAW:
        qdsp_g711_mode = QDSP_G711_MODE_ALAW;
        break;
      default:
        return(VOC_CMD_FAILED_S);
        break;
    }
    qdsp_status = qdsp_cmd_set_g711_mode(qdsp_g711_mode);
  }

  /* Check return code */
  if (qdsp_status == QDSP_CMD_SUCCESS) {
    return VOC_DONE_S;
  } else {
    return VOC_CMD_FAILED_S;
  }
}
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
/*===========================================================================

FUNCTION VOC_SET_G723_MODE

DESCRIPTION
  This function sets the encoder mode for the G.723 vocoder.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_set_g723_mode(
  voc_g723_mode_type g723_mode
)
{
  qdsp_cmd_g723_mode_type  qdsp_g723_mode = QDSP_G723_MODE_HIGH;
  qdsp_cmd_status_type     qdsp_status    = QDSP_CMD_SUCCESS;


  if ((g723_mode < VOC_G723_MODE_HIGH) || (g723_mode > VOC_G723_MODE_LOW)) {
    return VOC_BADFMT_S;
  }

  /* Save the initial G.723 mode setting */
  voc_data_set_g723_mode(g723_mode);

  /* check current image and state */
  if ((check_interlock(VOC_UNLOCK_NOT_RESET) == TRUE) &&
     (voc_get_curr_config() == VOC_CAP_G723)) {
    /* Get the current G.723 mode */
    g723_mode = voc_data_get_g723_mode();

    /* Convert the vocoder mode to QDSP mode */
    switch(g723_mode) {
      case VOC_G723_MODE_HIGH:
        qdsp_g723_mode = QDSP_G723_MODE_HIGH;
        break;
      case VOC_G723_MODE_LOW:
        qdsp_g723_mode = QDSP_G723_MODE_LOW;
        break;
      default:
        return(VOC_CMD_FAILED_S);
        break;
    }
    qdsp_status = qdsp_cmd_set_g723_mode(qdsp_g723_mode);
  }

  /* Check return code */
  if (qdsp_status == QDSP_CMD_SUCCESS) {
    return VOC_DONE_S;
  } else {
    return VOC_CMD_FAILED_S;
  }
}
#endif /* MSMAUD_VOC_G723 */


/* <EJECT> */
/*===========================================================================

FUNCTION voc_pkt_int_loop_cmd

DESCRIPTION
  This function will enable or disable packet internal loopback.  Encoder
packets can be routed back to decoder internally without vocoder driver's
interaction.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_pkt_int_loop_cmd ( boolean loop_on )
{
  if (loop_on == TRUE)
  {
    voc_state_test_mode_config( VOC_TEST_INT_PKT_LOOP );
  }
  else
  {
    if (voc_state_get_state_data()->test_mode == VOC_TEST_INT_PKT_LOOP)
    {
      voc_state_test_mode_config( VOC_TEST_NONE );
    }
  }
} /* voc_pkt_int_loop_cmd */

/* <EJECT> */

#ifdef FEATURE_UP_LOOPBACK_TEST

/*===========================================================================

FUNCTION voc_micro_loop_cmd

DESCRIPTION
  This function will setup the vocoder in software loopback mode and enable
loopback interrupt.  Tx packets from encoder will be routed back to decoder
in Rx path.  This function is used for software packet loopback testing
purpose.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_micro_loop_cmd ( boolean loop_on )
{
  if (loop_on == TRUE)
  {
    voc_state_test_mode_config( VOC_TEST_UP_PKT_LOOP );
    return;
  }

  if (voc_state_get_state_data()->test_mode == VOC_TEST_UP_PKT_LOOP)
  {
    voc_state_test_mode_config( VOC_TEST_NONE );
  }
}

#endif  /* FEATURE_UP_LOOPBACK_TEST */


/* <EJECT> */
/*===========================================================================

FUNCTION voc_dfm_loop_cmd

DESCRIPTION
  This function sets the DFM loopback state of the vocoder
  receive audio path. This causes the DFM RX to loopback to TX.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_dfm_loop_cmd ( boolean loop_on )
{
  if (loop_on == TRUE)
  {
    voc_state_test_mode_config( VOC_TEST_DFM_LOOP );
    return;
  }

  if (voc_state_get_state_data()->test_mode == VOC_TEST_DFM_LOOP)
  {
    voc_state_test_mode_config( VOC_TEST_NONE );
  }
} /* voc_dfm_loop_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_pkt_loop_cmd

DESCRIPTION
  This function is used by diag. task to call either voc_pkt_int_loop_cmd
or voc_micro_loop_cmd as the packet loopback test.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_pkt_loop_cmd ( boolean loop_on )
{
#ifdef FEATURE_UP_LOOPBACK_TEST
  voc_micro_loop_cmd(loop_on);
#else
  voc_pkt_int_loop_cmd(loop_on);
#endif
} /* voc_pkt_loop_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_audio_loop_cmd

DESCRIPTION
  This function sets the Audio loopback state of the vocoder receive
  audio path.  This causes the Tx samples to loop-back as Rx samples.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_audio_loop_cmd ( boolean loop_on )
{
  if (loop_on == TRUE)
  {
    voc_state_test_mode_config( VOC_TEST_AUDIO_LOOP );
    return;
  }

  if (voc_state_get_state_data()->test_mode == VOC_TEST_AUDIO_LOOP)
  {
    voc_state_test_mode_config( VOC_TEST_NONE );
  }
} /* voc_audio_loop_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_pcm_loop_cmd

DESCRIPTION
  PCM loopback is currently not supported.  This function will call
voc_audio_loop_cmd.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_pcm_loop_cmd ( boolean loop_on )
{
  voc_audio_loop_cmd(loop_on);
} /* voc_pcm_loop_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_d2e_loop_cmd

DESCRIPTION
  This function will enable or disable packet internal loopback.  Decoder
packets can be routed back to encoder internally without vocoder driver's
interaction.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_d2e_loop_cmd ( boolean loop_on )
{
  if (loop_on == TRUE)
  {
    voc_state_test_mode_config( VOC_TEST_D2E_LOOP );
    return;
  }

  if (voc_state_get_state_data()->test_mode == VOC_TEST_D2E_LOOP)
  {
    voc_state_test_mode_config( VOC_TEST_NONE );
  }
} /* voc_d2e_loop_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_rx_cmd

DESCRIPTION
  This function gives the current vocoder frame to the vocoder.

DEPENDENCIES
  Must be done after initializing the vocoder.

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_rx_cmd
(
  uint8 rate,                          /* rate of vocoder frame */
  uint8 *data_ptr                      /* pointer to frame data */
)
{
  uint8 qdsp_rate;

  ASSERT( data_ptr != NULL );

  /* Check for illegal rates
  */
  if ( rate > VOC_ERASURE ) {
    ERR( "Rx Illegal rate = %x", rate, 0, 0 );
    rate = VOC_8_RATE;
  }
  qdsp_rate = voc_rate_conv[rate];
  (void) qdsp_cmd_rx_frame(qdsp_rate, data_ptr);

  return( VOC_DONE_S );
} /* voc_rx_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_valid_addr

DESCRIPTION
  Checks if memory block is contained within QDSP memory space.

DEPENDENCIES
  None.

RETURN VALUE
  boolean
  - TRUE: memory block is contained within QDSP memory space.
  - FALSE: memory block is not contained within QDSP memory space.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_valid_addr (
  void* start_addr,
  uint16 length
)
{
   return( qdsp_valid_address( start_addr, length ) );
} /* voc_valid_addr */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_peek_cmd

DESCRIPTION
  Reads 16-bit values from vocoder address space.

DEPENDENCIES
  The address and length parameters should be checked to make sure that
  the memory block is in vocoder address space.  This is done using
  voc_valid_addr().

RETURN VALUE
  voc_status_type
  - VOC_DONE_S: command was successful.
  - VOC_CMD_FAILED_S: command failed.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_peek_cmd (
  void* src_ptr,     /* source pointer */
  void* dest_ptr,    /* destination pointer */
  uint16 length      /* size of data buffer */
)
{
  qdsp_peek (src_ptr, dest_ptr, (int)length);

  return( VOC_DONE_S );
} /* voc_peek_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_poke_cmd

DESCRIPTION
  Writes 16-bit values to vocoder address space.

DEPENDENCIES
  The address and length parameters should be checked to make sure that
  the memory block is in vocoder address space.  This is done using
  voc_valid_addr().

RETURN VALUE
  voc_status_type
  - VOC_DONE_S: command was successful.
  - VOC_CMD_FAILED_S: command failed.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_poke_cmd (
  void* dest_ptr,    /* destination pointer */
  void* src_ptr,     /* source pointer */
  uint16 length      /* size of data buffer */
)
{
  qdsp_poke (src_ptr, dest_ptr, (int)length);

  return( VOC_DONE_S );
} /* voc_poke_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_rx_mute_cmd

DESCRIPTION
  This function mutes/un-mutes the vocoder receive audio path.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - Command completed successfully

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_rx_mute_cmd
(
  boolean mute_on
)
{
  qdsp_cmd_status_type  result;

  /* debug message */
  MSG_MED( "voc_rx_mute_cmd( %d )", mute_on, 0, 0 );

  /* Save the mute setting
  */
  voc_data_set_rx_mute( mute_on, FALSE );

  if (check_interlock(VOC_UNLOCK_MODEM)
#ifdef FEATURE_VOC_PCM_INTERFACE
      || (voc_state_get_state_data()->pcm_int_mode_pending &&
       check_interlock(VOC_UNLOCK_VOC_NOT_RESET))
#endif
  ) {
    result = qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL));

    if (result == QDSP_CMD_SUCCESS) {
      return(VOC_DONE_S);
    } else {
      MSG_HIGH("QDSP2 Vocoder rx mute command error = %d", result, 0, 0);
    }
  } else {
    MSG_MED("RX Mute Setting Saved = %d", mute_on, 0, 0);
  }
  return(VOC_DONE_S);
} /* voc_rx_mute_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_rx_dtmf_override_cmd

DESCRIPTION
  This function mutes/un-mutes the vocoder receive audio path.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - Command completed successfully

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_rx_dtmf_override_cmd
(
  boolean mute_on
)
{
  qdsp_cmd_status_type  result;

  /* debug message */
  MSG_MED( "voc_rx_dtmf_override_cmd( %d )", mute_on, 0, 0 );

  /* Save the mute setting
  */
  voc_data_set_rx_mute( mute_on, TRUE );

  if (check_interlock(VOC_UNLOCK_MODEM)
#ifdef FEATURE_VOC_PCM_INTERFACE
      || (voc_state_get_state_data()->pcm_int_mode_pending &&
       check_interlock(VOC_UNLOCK_VOC_NOT_RESET))
#endif
  ) {
    result = qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL));

    if (result == QDSP_CMD_SUCCESS) {
      return(VOC_DONE_S);
    } else {
      MSG_HIGH("QDSP2 Vocoder rx mute command error = %d", result, 0, 0);
    }
  } else {
    MSG_MED("RX Mute Setting Saved = %d", mute_on, 0, 0);
  }
  return(VOC_DONE_S);
} /* voc_rx_dtmf_override_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_tx_mute_cmd

DESCRIPTION
  This function mutes/un-mutes the vocoder transmit audio path.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_tx_mute_cmd
(
  boolean mute_on                   /* TRUE = mute, FALSE = un-mute */
)
{
  qdsp_cmd_status_type  result;

  /* debug message */
  MSG_MED( "voc_tx_mute_cmd( %d )", mute_on, 0, 0 );

  /* Update database */
  voc_data_set_tx_mute( mute_on );

  if (check_interlock(VOC_UNLOCK_MODEM)
#ifdef FEATURE_VOC_PCM_INTERFACE
      || (voc_state_get_state_data()->pcm_int_mode_pending &&
       check_interlock(VOC_UNLOCK_VOC_NOT_RESET))
#endif
  ) {
    result = qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL));

    if (result == QDSP_CMD_SUCCESS) {
      return(VOC_DONE_S);
    } else {
      MSG_HIGH("QDSP2 Vocoder rx mute command error = %d", result, 0, 0);
    }
  } else {
    MSG_HIGH("TX Mute Setting Saved = %d", mute_on, 0, 0);
  }
  return(VOC_DONE_S);
} /* voc_tx_mute_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_dtmf_tx_mute_cmd

DESCRIPTION
  Control mute/un-mute of the DTMF output to the FM transmit path.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - Command completed successfully

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_dtmf_tx_mute_cmd
(
  boolean mute_on                   /* TRUE = mute, FALSE = un-mute */
)
{
  qdsp_cmd_status_type  result;

  /* debug message */
  MSG_MED( "voc_dtmf_tx_mute_cmd( %d )", mute_on, 0, 0 );

  voc_data_set_dtmf_tx_mute( mute_on );

  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    /* Send the command to the QDSP2 */
    result = qdsp_cmd_set_dtmf_path(voc_data_get_dtmf_path_setting());

    if (result == QDSP_CMD_SUCCESS) {
      /* Send volume command because it may have changed. */
      if (check_interlock(VOC_UNLOCK_MODEM)
#ifdef FEATURE_VOC_PCM_INTERFACE
      || (voc_state_get_state_data()->pcm_int_mode_pending &&
       check_interlock(VOC_UNLOCK_VOC_NOT_RESET))
#endif
      ) {
        (void) qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL));
      }

      return(VOC_DONE_S);
    } else {
      MSG_HIGH("QDSP2 Vocoder DTMF path set error = %d", result, 0, 0);
      return(VOC_DONE_S);
    }
  } else {
    MSG_HIGH("DTMF Tx Mute Setting Saved = %d", mute_on, 0, 0);
    return(VOC_DONE_S);
  }

} /* voc_dtmf_tx_mute_cmd */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_tx_rate_limit

DESCRIPTION
  This function limits the vocoder transmit rate to ( 2s+1 ) / ( 2s+2 ).

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - the command was successful.

SIDE EFFECTS
  voc_enc_settings

===========================================================================*/

voc_status_type voc_tx_rate_limit
(
  word rate_factor                   /* RDR limit from Sysparm msg */
)
{
  voc_state_control_type *state;
  qdsp_cmd_status_type   result;

  MSG_LOW( "voc_tx_rate_limit( 0x%x )", rate_factor, 0, 0 );

  /* Save rate_factor */
  voc_data_set_1x_tx_rate_factor(rate_factor);
  state = voc_state_get_state_data();

  /* TR-45 13kbps Service Option */
  if ((state->config == VOC_CAP_IS733)
      || (state->config == VOC_CAP_4GV_NB)
      || (state->config == VOC_CAP_4GV_NB_VOIP)
      || (state->config == VOC_CAP_4GV_WB)
      || (state->config == VOC_CAP_4GV_WB_VOIP)) {
    /* MCCTC.C shifts the rate reduced mode field up a bit. This works
    ** for IS-96(A). TR-45 (13kbps) needs a value of 0..4 for its' rate
    ** reduced mode, so we compensate for the shift here.
    */
    INTLOCK();
    voc_enc_setting.reduced_rate_level &= ~QDSP_RATE_REDUCE_MODE_M;
    voc_enc_setting.reduced_rate_level |=  rate_factor >> 1;
    INTFREE();

    MSG_HIGH ("Rate Reduced Mode 13kbps rate = %d",rate_factor,0,0);
   } else if ((state->config == VOC_CAP_IS127)
              ||(state->config == VOC_CAP_IS127_VOIP)
              )
   {
     /* IS127 - EVRC Service Option */
    switch (rate_factor) {
      case 8:                          /* rate 4/8 */
      case 0:                          /* 8/8 (1) */
        INTLOCK();
        voc_enc_setting.rate_modulation_cmd &= ~QDSP_RATE_MODULATION_M;
        voc_enc_setting.rate_modulation_cmd |=  QDSP_RATE_MODULATION_DIS_V;
        INTFREE();
        /* debug message */
        MSG_HIGH ("Rate limiting disabled - EVRC",0,0,0);
        break;
      case 2:                          /* S = 3 -> 7/8       */
      case 4:                          /* S = 1 -> 6/8 (3/4) */
        /* enable rate limiting */
        INTLOCK();
        voc_enc_setting.rate_modulation_cmd &=
                     ~(QDSP_RATE_MODULATION_M    |
                       QDSP_RATE_LIMIT_S_OR_1S_M |
                       QDSP_RATE_LIMIT_FACTOR_M);
        voc_enc_setting.rate_modulation_cmd |=
                      (QDSP_RATE_MODULATION_ENA_V |
                       QDSP_RATE_LIMIT_S_V)       |
                      (vc_rate_factors[ rate_factor ] << 2) ;
        INTFREE();
        /* debug message */
        MSG_HIGH ("Rate limiting enabled EVRC - rate = %d",rate_factor,0,0);
        break;
      case 6:                          /* 1/s = 1/3 -> 5/8    */
        /* enable rate limiting - Select factor 1/S */
        INTLOCK();
        voc_enc_setting.rate_modulation_cmd &=
                     ~(QDSP_RATE_MODULATION_M    |
                       QDSP_RATE_LIMIT_S_OR_1S_M |
                       QDSP_RATE_LIMIT_FACTOR_M);
        voc_enc_setting.rate_modulation_cmd |=
                      (QDSP_RATE_MODULATION_ENA_V |
                       QDSP_RATE_LIMIT_1S_V)      |
                      (vc_rate_factors[ rate_factor ] << 2);
        INTFREE();
        /* debug message */
        MSG_HIGH ("Rate limiting enabled EVRC - rate = %d",rate_factor,0,0);
        break;
      default:
        break;
    } /* switch (rate_factor) */
  } else {
    MSG_HIGH("Unmatched vocoder config for tx rate limit %d", \
             state->config, 0, 0);
    return VOC_DONE_S;
  }
  /* Issue the command to the DSP
  */
  if (check_interlock(VOC_UNLOCK_NOT_RESET)) {

    result = qdsp_cmd_enc_ctrl(&voc_enc_setting);

    if (result == QDSP_CMD_SUCCESS) {
      return(VOC_DONE_S);
    } else {
      MSG_HIGH("QDSP2 Vocoder enc_ctrl cmd error = %d", result, 0, 0);
    }
  } else {
    MSG_HIGH("voc_tx_rate_cmd settings saved", 0, 0, 0);
  }
  return( VOC_DONE_S );
} /* voc_tx_rate_limit */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_version

DESCRIPTION
  This function acquires version information from the Vocoder

DEPENDENCIES
  None

RETURN VALUE
  The ROM version and the RAM version.

SIDE EFFECTS
  None

===========================================================================*/
void voc_version
(
  uint8 *hardware,
  uint8 *software
)
{
  qdsp_cmd_diag_type* diag_data;

  diag_data = voc_data_get_diags();
  *hardware = (uint8)(diag_data->rom_version >> 8);
  *software = (uint8)(diag_data->ram_version >> 8);
} /* voc_version */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_codec_pad

DESCRIPTION
  Set the pad values for the Codec.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_codec_pad
(
  uint8  padval,   /* Value for pad bits             */
  uint8  mask      /* Mask of pad bits to control    */
)
{
  voc_codec_class_type           codec_type;
  uint16                         codec_pad;

  voc_data_get_codec_class(&codec_type);
  switch (codec_type) {
    case VOC_CODEC_CLASS_EXTERNAL:
      /* Save the external CODEC volume setting
      */
      codec_pad = voc_data_set_pcmpad_vol( padval, mask, TRUE );

      if(check_interlock(VOC_UNLOCK_NOT_RESET)
#ifdef FEATURE_QTUNES_COMMON
         && !check_interlock(VOC_UNLOCK_QTUNES)
#endif
      ) {
        (void) qdsp_cmd_set_pcmpad(codec_pad);
      } else {
        MSG_HIGH("External CODEC Vol Settings saved", 0, 0, 0);
      }
      break;
    default:
      break;
  }
} /* voc_codec_pad */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_retry_codec_gain

DESCRIPTION
  Set the gain values for the Codec.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_retry_codec_gain(void)
{
  voc_codec_class_type           codec_type;
  qdsp_cmd_codec_gain_ctrl_type* codec_control;
  uint16                         codec_pad;

  voc_data_get_codec_class(&codec_type);
  switch (codec_type) {
    case VOC_CODEC_CLASS_INTERNAL:
#ifdef FEATURE_INTERNAL_SDAC
    case VOC_CODEC_CLASS_INTERNAL_STEREO:
#endif
#ifdef FEATURE_BT
#ifdef FEATURE_BT_AG
    case VOC_CODEC_CLASS_BLUETOOTH:
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    case VOC_CODEC_CLASS_BLUETOOTH_A2DP:
    case VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO:
#endif /* FEATURE_SBC_CODEC */
#endif /* FEATURE_BT */
      /* Save the internal CODEC volume setting
      */
      codec_control = voc_data_get_codec_gain();

      if(check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
        (void) qdsp_cmd_codec_gain_ctrl(
                 codec_control, QDSP_CMD_CODEC_GAIN_OP_NORMAL);
      } else {
        MSG_HIGH("Internal CODEC Gain Settings saved", 0, 0, 0);
      }
      break;
    case VOC_CODEC_CLASS_EXTERNAL:
      /* Save the external CODEC volume setting
      */
      codec_pad = voc_data_set_pcmpad_vol( 0, 0, FALSE );

      if(check_interlock(VOC_UNLOCK_NOT_RESET)
#ifdef FEATURE_QTUNES_COMMON
         && !check_interlock(VOC_UNLOCK_QTUNES)
#endif
      ) {
        (void) qdsp_cmd_set_pcmpad(codec_pad);
      } else {
        MSG_HIGH("External CODEC Vol Settings saved", 0, 0, 0);
      }
      break;
    case VOC_CODEC_CLASS_NONE:
#ifdef FEATURE_EXTERNAL_SDAC
    case VOC_CODEC_CLASS_EXTERNAL_STEREO:
#endif
      MSG_HIGH( "No CODEC to be programmed", 0, 0, 0 );
      break;
    case VOC_CODEC_CLASS_MAX:
    default:
      ERR_FATAL( "Illegal CODEC TYPE", 0, 0, 0 );
  }
} /* voc_retry_codec_gain */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_pcmpad

DESCRIPTION
 Get the value of the incoming (TX) PCM pad byte.

DEPENDENCIES
 None

RETURN VALUE
 PCM pad byte

SIDE EFFECTS
 None

===========================================================================*/

uint8 voc_get_pcmpad( void )
{
  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    return((uint8)(qdsp_cmd_get_pcmpad() >> 8));
  } else {
    return(0);
  }
} /* voc_get_pcmpad */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_put_pcmpad

DESCRIPTION
  Set the value of the outgoing (RX) PCM pad byte.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void voc_put_pcmpad( uint8 pad_byte )
{
  /* Update the database */
  uint16 codec_pad = voc_data_set_pcmpad_ctl(pad_byte);

  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    (void) qdsp_cmd_set_pcmpad(codec_pad);
  }
} /* voc_put_pcmpad */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_AUD_PROCESS_OVERRIDE

DESCRIPTION
  This function sets audio process override mode.  Will override
  ESEC/NS/AGC/HPF settings to OFF if enabled.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - Operation completed successfully.

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_aud_process_override (
  boolean aud_process_override
) {
  if (aud_process_override != voc_data_get_aud_process_override()) {
    /* Set the configuration
    */
    voc_data_set_aud_process_override(aud_process_override);

    /* Only re-initialize audio processing functions if not reset */
    if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
      voc_retry_codec_select();
      if(voc_state_init_aud_process() != VOC_STATE_SUCCESS) {
        return(VOC_CMD_FAILED_S);
      }
    }
  }
  return(VOC_DONE_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_ec_override_mode

DESCRIPTION
  Controls EC (Echo Canceller).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_ec_override_mode (
  voc_ec_ns_mode_type   ec_mode
) {
  voc_data_set_ec_override_mode(ec_mode);

  /* Make any changes based on current settings
  */
  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    voc_state_init_aud_process();
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_ns_override_mode

DESCRIPTION
  Controls NS (Noise Suppressor).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_ns_override_mode (
  voc_ec_ns_mode_type   ns_mode
) {
  voc_data_set_ns_override_mode(ns_mode);

  /* Make any changes based on current settings
  */
  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    voc_state_init_aud_process();
  }
}

/* <EJECT> */

/*===========================================================================
 FUNCTION voc_smartsnd_set_rve_mode

 DESCRIPTION
   Controls RVE

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None

 ==========================================================================*/
void voc_smartsnd_set_rve_mode (
  voc_smartsnd_type rve_mode
){
   voc_data_smartsnd_set_rve(rve_mode);

  /* Make any changes based on current settings
  */
  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    voc_state_init_aud_process();
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_vox_override_mode

DESCRIPTION
  Controls VOX (half/full duplex voice mode).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_vox_override_mode (
  voc_vox_mode_type   vox_mode
) {
  voc_data_set_vox_override_mode(vox_mode);

  /* Make any changes based on current settings
  */
  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    voc_state_init_aud_process();
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_sidetone_override_mode

DESCRIPTION
  Controls sidetone.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_sidetone_override_mode (
  voc_sidetone_mode_type   sidetone_mode
)
{
  voc_data_set_sidetone_override_mode(sidetone_mode);

  /* Make any changes based on current settings
  */
  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    voc_state_init_aud_process();
  }
}

/* <EJECT> */

#ifdef FEATURE_AUDIO_AGC
/*===========================================================================

FUNCTION VOC_AGC_CMD

DESCRIPTION
  This function sets the AGC/AVC mode.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S -

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_agc_cmd ( void )
{
  qdsp_cmd_agc_config_type  agc_config;
  qdsp_cmd_agc_param_type   agc_param;
  qdsp_cmd_status_type      result;
#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

  /* Make any changes based on current settings
  */
  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    /* Override AGC settings if aud_process_override is set */
    if(voc_data_get_aud_process_override() == FALSE) {
      (void) voc_data_get_agc(&agc_config);
    }
#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */
    else {
      agc_config.rx_agc_enable = QDSP_CMD_AGC_DIS;
      agc_config.rx_avc_enable = QDSP_CMD_AGC_DIS;
      agc_config.tx_agc_enable = QDSP_CMD_AGC_DIS;
    }
    /* Issue agc configuration command */
    result = qdsp_cmd_agc_config(&agc_config);

    if (result == QDSP_CMD_SUCCESS) {
      MSG_HIGH("QDSP2 AGC Configuration success", 0, 0, 0);
    } else {
      MSG_HIGH("QDSP2 AGC Configuration Error = %d", result, 0, 0);
      return(VOC_DONE_S);
    }
    voc_data_get_agc_param(&agc_param);
    result = qdsp_cmd_agc_param(&agc_param);
    if (result == QDSP_CMD_SUCCESS) {
      MSG_HIGH("QDSP2 AGC Parameters success", 0, 0, 0);
    } else {
      MSG_HIGH("QDSP2 AGC Parameters Error = %d", result, 0, 0);
    }
  }
  return(VOC_DONE_S);
}


/*===========================================================================
  FUNCTION voc_smartsnd_agc_cmd

  DESCRIPTION
    This function sets the AGC/AVC mode

  DEPENDENCIES
    None

  RETURN VALUE
     VOC_DONE_S

  SIDE EFFECTS
     None

 ===========================================================================*/

voc_status_type voc_smartsnd_agc_cmd( void )
{
  qdsp_cmd_agc_config_type  agc_config;
  qdsp_cmd_agc_param_type   agc_param;
  qdsp_cmd_status_type      result;
#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

  /* Make any changes based on current settings
  */
  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    /* Override AGC settings if aud_process_override is set */
    if(voc_data_get_aud_process_override() == FALSE) {
      (void) voc_data_smartsnd_get_agc(&agc_config);
    }
#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */
    else {
      agc_config.rx_agc_enable = QDSP_CMD_AGC_DIS;
      agc_config.rx_avc_enable = QDSP_CMD_AGC_DIS;
      agc_config.tx_agc_enable = QDSP_CMD_AGC_DIS;
    }
    /* Issue agc configuration command */
    result = qdsp_cmd_agc_config(&agc_config);

    if (result == QDSP_CMD_SUCCESS) {
      MSG_HIGH("QDSP2 AGC Configuration success", 0, 0, 0);
    } else {
      MSG_HIGH("QDSP2 AGC Configuration Error = %d", result, 0, 0);
      return(VOC_DONE_S);
    }
    voc_data_smartsnd_get_agc_param(&agc_param);
    result = qdsp_cmd_agc_param(&agc_param);
    if (result == QDSP_CMD_SUCCESS) {
      MSG_HIGH("QDSP2 AGC Parameters success", 0, 0, 0);
    } else {
	  ERR( "QDSP2 AGC Parameters Error = %d", result, 0, 0);
    }
  }
  return(VOC_DONE_S);

}


/* <EJECT> */
/*===========================================================================

FUNCTION VOC_RX_AGC_CMD

DESCRIPTION
  This function sets the RX AGC/AVC mode.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S -

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_rx_agc_cmd (
  voc_agc_type agc_mode
) {

  /* Set the configuration
  */
  (void) voc_data_set_rx_agc(agc_mode);

  return(voc_agc_cmd());
}



/* <EJECT> */
/*===========================================================================
 FUNCTION voc_smartsnd_rx_agc_cmd

 DESCRIPTION
   This function set the RX AGC/AVC mode.

 DEPENDENCIES
   None

 RETURN VALUE
   VOC_DONE_S

 SIDE_EFFECTS
   None


 ==========================================================================*/

voc_status_type voc_smartsnd_rx_agc_cmd (
  voc_smartsnd_type agc_mode
){
    /*set the configuration*/
    voc_data_smartsnd_set_rx_agc(agc_mode);
    return (voc_smartsnd_agc_cmd());
}

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_TX_AGC_CMD

DESCRIPTION
  This function sets the TX AGC/AVC mode.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S -

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_tx_agc_cmd (
  boolean agc_mode
) {

  /* Set the configuration
  */
  (void) voc_data_set_tx_agc(agc_mode);

  return (voc_smartsnd_agc_cmd());
}
#endif /* FEATURE_AUDIO_AGC */


#ifdef FEATURE_TTY

#ifdef FEATURE_TTY_ACTIVITY_STATUS
/*===========================================================================

FUNCTION voc_register_tty_status_client

DESCRIPTION
  Use this function to register a callback which will receive TTY activity
  status updates. Calling this function with a NULL parameter de-registers
  any previously registered callback. The status is updated whenever the
  encoder/decoder state changes. Internally this state change is checked
  approx. every 50ms, but the callback will not be invoked if no state change
  has occurred between successive 50ms periods.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_register_tty_status_client (
  voc_tty_status_fn_type tty_func  /* Callback for TTY activity status */
)
{
  voc_tty_status_func = tty_func;
  if (tty_func == NULL) {
    qdsp_cmd_register_tty_status_client(NULL);
  } else {
    qdsp_cmd_register_tty_status_client(voc_tty_activity_status);
  }
}
#endif /* FEATURE_TTY_ACTIVITY_STATUS */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_TTY_CMD

DESCRIPTION
  This function sets the TTY mode.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S -

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_tty_cmd ( void )
{
  voc_state_control_type    *state;
  voc_tty_mode_type         tty_mode;
  voc_status_type           result;

  /* Make any changes based on current settings
  */
  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {

    /* Override TTY settings if aud_process_override is set */
    if(voc_data_get_aud_process_override() == FALSE) {
      state = voc_state_get_state_data();
      tty_mode = state->tty_mode;
    } else {
      tty_mode = VOC_TTY_MODE_OFF;
    }

    /* Issue agc configuration command */
    result = voc_state_set_tty(tty_mode);

    if (result != VOC_STATE_SUCCESS) {
      MSG_HIGH("VOC TTY Configuration Error = %d", result, 0, 0);
      return(VOC_DONE_S);
    }

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */
  }
  return(VOC_DONE_S);
}
#endif /* FEATURE_TTY */

/* <EJECT> */
#if defined(FEATURE_PHONE_VR) || defined(FEATURE_VOICE_RECORD)
/*===========================================================================

FUNCTION VOC_RESTORE_COMFORT_NOISE

DESCRIPTION
  This function restore the comfort noise value after voice recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_restore_comfort_noise( void )
{
  if (check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    qdsp_cmd_restore_comfort_noise();
  }
}
#endif /* FEATURE_PHONE_VR || FEATURE_VOICE_RECORD */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_EQUALIZER
/*===========================================================================

FUNCTION voc_equalizer_init_crit_sect

DESCRIPTION
  This function will initialize the critial section for equalizer.
  Must be fore before using the voc_equalizer_crit_sect

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_equalizer_init_crit_sect( void ) {
  rex_init_crit_sect( &voc_equalizer_crit_sect );
}

/*===========================================================================

FUNCTION voc_setup_equalizer

DESCRIPTION
  Issues the DSP command to enable the equalizer and setups up retry timer if
  unsuccessful.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_setup_equalizer(
  const qdsp_cmd_equalizer_type *eq_data,
        boolean                 set_sig
) {
  voc_status_type  status = VOC_DONE_S;

  rex_enter_crit_sect( &voc_equalizer_crit_sect );
  if (!qdsp_cmd_check_equalizer_reset()) {
    if (qdsp_cmd_set_equalizer(eq_data) != QDSP_CMD_SUCCESS) {
      status = VOC_CMD_FAILED_S;
    }
    #ifdef FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL
    else {
      if (set_sig) {
        /* Signal vocstate so that we can reduce the clk rate. */
        (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
      }
    }
    #endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL */
  } else {
    voc_set_equalizer_retry_timer(TRUE);
  }
  rex_leave_crit_sect( &voc_equalizer_crit_sect );
  return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_update_equalizer

DESCRIPTION
  This function will update the QSYNTH/QTUNES equalizer parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_update_equalizer( void ) {

  voc_status_type  status = VOC_DONE_S;
  boolean dsp_ready = FALSE;
  boolean setup_eq = TRUE;
  #ifdef FEATURE_QTUNES_COMMON
  boolean qtunes_ready = FALSE;
  #endif /* FEATURE_QTUNES_COMMON */

  voc_set_equalizer_retry_timer(FALSE);

  #ifdef FEATURE_QSYNTH_COMMON
  dsp_ready |= check_interlock(VOC_UNLOCK_QSYNTH);
  #endif /* FEATURE_QSYNTH_COMMON */

  #ifdef FEATURE_QTUNES_COMMON
  qtunes_ready = check_interlock(VOC_UNLOCK_QTUNES);
  dsp_ready |= qtunes_ready;
  #endif /* FEATURE_QTUNES_COMMON */

  #ifdef FEATURE_AVS_INCALL_ADEC_PB
  dsp_ready |= check_interlock(VOC_INFORM_VOCODER_ADEC);
  #endif /* FEATURE_AVS_INCALL_ADEC_PB  */

  #ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  dsp_ready |= check_interlock(VOC_UNLOCK_MIXER_ADEC);
  #endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB  */

  if (dsp_ready) {
    const qdsp_cmd_equalizer_type *eq_data = voc_data_get_equalizer();

    if (eq_data->enable == FALSE) {
      /* ADSP clock adjustment must be done after disabling */
      status = voc_setup_equalizer(eq_data, TRUE);
    }
    else {
      /* Prior to enabling, the ADSP clock needs to bumped up as necessary. */
      #ifdef FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL
      #if defined(FEATURE_QTUNES_COMMON)
      if (qtunes_ready) {
        /* If audfmt configuration is done then only update equalizer 
        Without this check the equalizer is getting updated with previous
        sample rate and pop sound is coming during start of the playback */
        if (sndhw_get_audfmt_need_config())
        {
          /* Signal the state machine so that the appropriate module and
             ADSP clock setting is loaded by QDSP services. */
          (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

          /* Can't setup the eq now. Rely on the timer to initialize it
             once vocstate is ready. */
          setup_eq = FALSE;
          voc_set_equalizer_retry_timer(TRUE);
        }
        else if (voc_data_current_image_supports_dcs()) {
          qdsp_module_type curr_mod = voc_state_get_state_data()->module_reqd;

          /* Force the equalizer to be setup in the vocoder task context -
             - an ugly way to serialize equalizer setup! Need to do this in
             vocoder task context to ensure that curr_mod does not change on
             us while we are trying to setup the eq in snd task context */
          if (rex_self() != &voc_tcb) {
            /* Signal the state machine so that the appropriate module and
               ADSP clock setting is loaded by QDSP services. */
            (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

            /* Can't setup the eq now. Rely on the timer to initialize it once
               vocstate is ready. */
            setup_eq = FALSE;
            voc_set_equalizer_retry_timer(TRUE);
          }
          else {
            if (curr_mod == QDSP_MODULE_BA_MONO
                #ifdef QDSP_MODULE_BA_STEREO_DEFINED
                || curr_mod == QDSP_MODULE_BA_STEREO
                #endif
                ) {
              /* PB not active */
              setup_eq = FALSE;
            }
            else {
              if (voc_data_get_qtunes_module(
                    voc_state_get_state_data()->config_pending) != curr_mod) {
                /* Signal the state machine so that the appropriate module and
                   ADSP clock setting is loaded by QDSP services. */
                (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

                /* Can't setup the eq now. Rely on the timer to initialize it
                   once vocstate is ready. */
                setup_eq = FALSE;
                voc_set_equalizer_retry_timer(TRUE);
              }
            }
          }
        }
      }
      #endif /* FEATURE_QTUNES_COMMON */
      #endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL */

      if (setup_eq == TRUE) {
        status = voc_setup_equalizer(eq_data, FALSE);
      }
    }
  }

  return (status);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_enable_equalizer

DESCRIPTION
  This function enable/disable the equalizer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_enable_equalizer
(
  boolean                   enable
)
{
  voc_status_type status;

  status = voc_data_enable_equalizer(enable);

  if (status == VOC_DONE_S) {
    voc_update_equalizer();
  }

  return (status);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_equalizer

DESCRIPTION
  This function enable/dsiable the equalizer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_set_equalizer
(
  voc_equalizer_filter_type *filter,
  uint16                    num
)
{
  voc_status_type status;

  rex_enter_crit_sect( &voc_equalizer_crit_sect );
  status = voc_data_set_equalizer(filter, num);
  rex_leave_crit_sect( &voc_equalizer_crit_sect );

  if (status == VOC_DONE_S) {
    voc_update_equalizer();
  }

  return (status);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_equalizer_gain

DESCRIPTION
  This function setup the equalizer gain on a specified band.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_set_equalizer_gain
(
  uint16 band,
  int16  gain
)
{
  voc_status_type status;

  rex_enter_crit_sect( &voc_equalizer_crit_sect );
  status = voc_data_set_equalizer_gain(band, gain);
  rex_leave_crit_sect( &voc_equalizer_crit_sect );

  if (status == VOC_DONE_S) {
    voc_update_equalizer();
  }

  return (status);
}

/*===========================================================================

FUNCTION voc_set_preset_eq_coeff

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_set_preset_eq_coeff
(
  const uint16 **coeff,
  uint16       bands,
  boolean      enable
)
{
  voc_status_type status;
  status = voc_data_set_preset_eq_coeff(coeff, bands, enable);

  if (status == VOC_DONE_S) {
    voc_update_equalizer();
  }

  return (status);
}
#endif /* FEATURE_AUDFMT_EQUALIZER */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/*===========================================================================

FUNCTION voc_set_spectrum_analyzer_interval

DESCRIPTION
  This function setup the spectrum analyzer interval

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_set_spectrum_analyzer_interval( void )
{
  #if defined(FEATURE_QTUNES_COMMON)      || \
      defined(FEATURE_QSYNTH_COMMON)      || \
      defined(FEATURE_AVS_INCALL_ADEC_PB) || \
      defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  boolean dsp_ready = FALSE;
  boolean setup_spa = TRUE;
  #ifdef FEATURE_QTUNES_COMMON
  boolean qtunes_ready = FALSE;
  #endif /* FEATURE_QTUNES_COMMON */

  #ifdef FEATURE_QTUNES_COMMON
  qtunes_ready = check_interlock(VOC_UNLOCK_QTUNES);
  dsp_ready |= qtunes_ready;
  #endif /* FEATURE_QTUNES_COMMON */

  #ifdef FEATURE_QSYNTH_COMMON
  dsp_ready |= check_interlock(VOC_UNLOCK_QSYNTH);
  #endif

  #ifdef FEATURE_AVS_INCALL_ADEC_PB
  dsp_ready |= check_interlock(VOC_UNLOCK_VOCODER_ADEC);
  #endif

  #ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  dsp_ready |= check_interlock(VOC_UNLOCK_MIXER_ADEC);
  #endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB  */

  if(dsp_ready) {
    #ifdef FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL
    #if defined(FEATURE_QTUNES_COMMON)
    if (qtunes_ready) {
      if (voc_data_current_image_supports_dcs()) {
        qdsp_module_type curr_mod = voc_state_get_state_data()->module_reqd;
        if (curr_mod == QDSP_MODULE_BA_MONO
            #ifdef QDSP_MODULE_BA_STEREO_DEFINED
            || curr_mod == QDSP_MODULE_BA_STEREO
            #endif
            ) {
          /* PB not active */
          setup_spa = FALSE;
        }
        else {
          if (voc_data_get_qtunes_module(
                voc_state_get_state_data()->config_pending) != curr_mod) {
            /* Signal the state machine so that the appropriate module and
               ADSP clock setting is loaded by QDSP services. */
            (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

            /* Can't setup the spa now. Rely on vocstate to do it */
            setup_spa = FALSE;
          }
        }
      }
    }
    #endif /* FEATURE_QTUNES_COMMON */
    #endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL */

    if (setup_spa) {
      TASKLOCK();
      qdsp_cmd_spectrum_analyzer_config(
                                     voc_data_get_spectrum_analyzer_interval());
      TASKFREE();
    }
  }
  #endif /* FEATURE_QTUNES_COMMON || FEATURE_QSYNTH_COMMON */
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_spectrum_analyzer_ctl

DESCRIPTION
  Registers the spectrum analyzer callback with the vocoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
extern void voc_spectrum_analyzer_ctl (
  uint16                         update_period,
    /* How often should the data be analyzed (10ms multiples) */

  voc_spectrum_analyzer_cb_type  sp_cb
    /* Callback to receive the data */
)
{

  /* Store the update period
  */
  voc_data_set_spectrum_analyzer_period(update_period);

  /* Register the callback function to sound layer
  */
  qdsp_cmd_set_spectrum_analyzer(sp_cb);

  /* Setup the spectrum analyzer if possible
  */
  voc_set_spectrum_analyzer_interval();
}
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

/* <EJECT> */
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_QCONCERT
/*===========================================================================

FUNCTION voc_update_qconcert_config

DESCRIPTION
  This function update the qconcert parameters value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
void voc_update_qconcert_config( void ) {

  boolean dsp_ready = FALSE;
  boolean setup_qc = TRUE;
  #ifdef FEATURE_QTUNES_COMMON
  boolean qtunes_ready = FALSE;
  #endif /* FEATURE_QTUNES_COMMON */

  voc_set_qconcert_retry_timer(FALSE);

  #ifdef FEATURE_QTUNES_COMMON
  qtunes_ready = check_interlock(VOC_UNLOCK_QTUNES);
  dsp_ready |= qtunes_ready;
  #endif

  #ifdef FEATURE_QSYNTH_COMMON
  dsp_ready |= check_interlock(VOC_UNLOCK_QSYNTH);
  #endif

  #ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
    dsp_ready |= check_interlock(VOC_UNLOCK_MIXER_ADEC);
  #endif

  if (dsp_ready) {
    qdsp_cmd_qconcert_config_type qconcert_config;

    qconcert_config.enable = FALSE;
    (void)voc_data_get_qconcert_config(&qconcert_config);

    if (qconcert_config.enable == FALSE) {
      if (qdsp_cmd_set_qconcert_config(&qconcert_config) != QDSP_CMD_SUCCESS){
        voc_set_qconcert_retry_timer(TRUE);
      }
      else {
        #ifdef FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL
        /* Signal the state machine so that the appropriate module and
           ADSP clock setting is loaded by QDSP services. */
        (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
        #endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL */
      }
    }
    else {
      #ifdef FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL
      #if defined(FEATURE_QTUNES_COMMON)
      if (qtunes_ready) {
        if (voc_data_current_image_supports_dcs()) {
          qdsp_module_type curr_mod = voc_state_get_state_data()->module_reqd;

          /* Force qconcert to be setup in the vocoder task context -
             - an ugly way to serialize qconcert setup! Need to do this in
             vocoder task context to ensure that curr_mod does not change on
             us while we are trying to setup qconcert in snd task context */
          if (rex_self() != &voc_tcb) {
            /* Signal the state machine so that the appropriate module and
               ADSP clock setting is loaded by QDSP services. */
            (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

            /* Can't setup qconcert now. Rely on the timer initialize it once
               vocstate in ready. */
            setup_qc = FALSE;
            voc_set_qconcert_retry_timer(TRUE);
          }
          else {
            if (curr_mod == QDSP_MODULE_BA_MONO
                #ifdef QDSP_MODULE_BA_STEREO_DEFINED
                || curr_mod == QDSP_MODULE_BA_STEREO
                #endif
                ) {
              /* PB not active */
              setup_qc = FALSE;
            }
            else {
              if (voc_data_get_qtunes_module(
                    voc_state_get_state_data()->config_pending) != curr_mod) {
                /* Signal the state machine so that the appropriate module
                   and ADSP clock setting is loaded by QDSP services. */
                (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

                /* Can't setup qconcert now. Rely on the timer initialize it
                   once vocstate in ready. */
                setup_qc = FALSE;
                voc_set_qconcert_retry_timer(TRUE);
              }
            }
          }
        }
      }
      #endif /* FEATURE_QTUNES_COMMON */
      #endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL */

      if (setup_qc) {
        if (qdsp_cmd_set_qconcert_config(&qconcert_config) != QDSP_CMD_SUCCESS){
          voc_set_qconcert_retry_timer(TRUE);
        }
      }
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_enable_qconcert

DESCRIPTION
  This function enable/disable the qconcert

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern voc_status_type voc_enable_qconcert
(
  boolean enable
)
{
  voc_data_enable_qconcert(enable);
  voc_update_qconcert_config();

  return (VOC_DONE_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_qconcert_params

DESCRIPTION
  This function setup the qconcert params

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
extern voc_status_type voc_set_qconcert_params
(
  voc_qconcert_params_type *params
)
{
  voc_status_type status;

  status = voc_data_set_qconcert_params(params);

  if (status == VOC_DONE_S) {
    voc_update_qconcert_config();
  }

  return (status);
}
#endif /* FEATURE_AUDFMT_QCONCERT */


/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_audfmt_pp_config

DESCRIPTION
  This function will setup audio pro-processing blocks configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_set_audfmt_pp_config(void) {

#ifdef FEATURE_AUDFMT_EQUALIZER
  voc_update_equalizer();
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
  voc_set_spectrum_analyzer_interval();
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

#ifdef FEATURE_AUDFMT_QCONCERT
  voc_update_qconcert_config();
#endif /* FEATURE_AUDFMT_QCONCERT */

}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_audfmt_pp_cal

DESCRIPTION
  This function will setup the audio pro-processing calibration blocks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_set_audfmt_pp_cal (void )
{
#if defined(FEATURE_AUDFMT_FIR_FILTER)
  voc_state_control_type     *state;
  const qdsp_cmd_audfmt_pcm_filter_type *audfmt_filter_ptr;
#endif /* FEATURE_AUDFMT_FIR_FILTER */

#ifdef FEATURE_AUDFMT_STF
  if (qdsp_cmd_set_audfmt_sidechain_filter(
    voc_data_get_audfmt_sidechain_filter()) != QDSP_CMD_SUCCESS) {
    MSG_ERROR("Failed to update sidechain fitler",0,0,0);
  }
#endif /* FEATURE_AUDFMT_STF */

#ifdef FEATURE_AUDFMT_IIR_FILTER
  if (qdsp_cmd_set_audfmt_iir_filter(
    voc_data_get_audfmt_iir_filter()) != QDSP_CMD_SUCCESS) {
    MSG_ERROR("Failed to update IIR fitler",0,0,0);
  }
#endif /* FEATURE_AUDFMT_IIR_FILTER */

#ifdef FEATURE_AUDFMT_FIR_FILTER
  /* Note: Can clean up after MP3 and AAC replace the PCM filter with
  ** IIR Filter
  */

  /* Get the current state data
  */
  state = voc_state_get_state_data();

  switch (state->current_image) {
#if defined(QDSP_IMAGE_SYNTH_DEFINED) || defined(QDSP_IMAGE_Q3D_DEFINED) || \
    defined(QDSP_IMAGE_QGL_DEFINED)
#ifdef QDSP_IMAGE_SYNTH_DEFINED
    case QDSP_IMAGE_SYNTH:
#endif
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_Q3D_DEFINED
    case QDSP_IMAGE_Q3D:
#endif
#ifdef QDSP_IMAGE_QGL_DEFINED
    case QDSP_IMAGE_QGL:
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
    case QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
#endif
      /* Get the PCM filter configuration data
      */
      audfmt_filter_ptr = voc_data_get_qsynth_pcm_filter();
      if (qdsp_cmd_set_audfmt_pcm_filter(
                 (qdsp_cmd_audfmt_pcm_filter_type *) audfmt_filter_ptr) !=
            QDSP_CMD_SUCCESS) {
        MSG_ERROR("Graphics QSYNTH PCM Filter control config failed",0,0,0);
      }
      break;
#endif /* QDSP_IMAGE_SYNTH_DEFINED || QDSP_IMAGE_Q3D_DEFINED ||
          QDSP_IMAGE_QGL_DEFINED */

#if defined(QDSP_IMAGE_TUNES_DEFINED)        || \
    defined(QDSP_IMAGE_AAC_DEFINED)          || \
    defined(QDSP_IMAGE_ENH_AAC_PLUS_DEFINED) || \
    defined(QDSP_IMAGE_QTV_AAC_DEFINED)      || \
    defined(QDSP_IMAGE_QTV_AAC_H264_DEFINED) || \
    defined(QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_QTV_CONC_DEFINED)     || \
    defined(QDSP_IMAGE_VOC_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_VOC_EVRC_MP3_DEFINED) || \
    defined(QDSP_IMAGE_AMR_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_EFR_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_FR_MP3_DEFINED)       || \
    defined(QDSP_IMAGE_HR_MP3_DEFINED)       || \
    defined(QDSP_IMAGE_VOC_COMBO_ICA_DEFINED)
#ifdef QDSP_IMAGE_TUNES_DEFINED
    case QDSP_IMAGE_TUNES:
#endif
#ifdef QDSP_IMAGE_AAC_DEFINED
    case QDSP_IMAGE_AAC:
#endif
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
    case QDSP_IMAGE_ENH_AAC_PLUS:
#endif
#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
    case QDSP_IMAGE_QTV_AAC:
#endif
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif 
#ifdef QDSP_IMAGE_QTV_CONC_DEFINED
    case QDSP_IMAGE_QTV_CONC:
#endif /* QDSP_IMAGE_QTV_CONC_DEFINED */
#ifdef QDSP_IMAGE_VOC_MP3_DEFINED
    case QDSP_IMAGE_VOC_MP3:
#endif
#ifdef QDSP_IMAGE_VOC_EVRC_MP3_DEFINED
    case QDSP_IMAGE_VOC_EVRC_MP3:
#endif
#ifdef QDSP_IMAGE_AMR_MP3_DEFINED
    case QDSP_IMAGE_AMR_MP3:
#endif
#ifdef QDSP_IMAGE_EFR_MP3_DEFINED
    case QDSP_IMAGE_EFR_MP3:
#endif
#ifdef QDSP_IMAGE_FR_MP3_DEFINED
    case QDSP_IMAGE_FR_MP3:
#endif
#ifdef QDSP_IMAGE_HR_MP3_DEFINED
    case QDSP_IMAGE_HR_MP3:
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_ICA_DEFINED
    case QDSP_IMAGE_VOC_COMBO_ICA_DEFINED:
#endif

      /* Get the PCM filter configuration data
      */
      audfmt_filter_ptr = voc_data_get_qtunes_pcm_filter();
      if (qdsp_cmd_set_audfmt_pcm_filter (
           (qdsp_cmd_audfmt_pcm_filter_type *) audfmt_filter_ptr) !=
              QDSP_CMD_SUCCESS) {
        MSG_ERROR("QTUNES PCM Filter config failed",0,0,0);
      }
      break;
#endif /*  QDSP_IMAGE_TUNES_DEFINED        || QDSP_IMAGE_AAC_DEFINED     ||
           QDSP_IMAGE_ENH_AAC_PLUS_DEFINED || QDSP_IMAGE_QTV_AAC_DEFINED ||
           QDSP_IMAGE_QTV_CONC_DEFINED     || QDSP_IMAGE_VOC_MP3_DEFINED ||
           QDSP_IMAGE_VOC_EVRC_MP3_DEFINED || QDSP_IMAGE_AMR_MP3_DEFINED ||
           QDSP_IMAGE_EFR_MP3_DEFINED      || QDSP_IMAGE_FR_MP3_DEFINED  ||
           QDSP_IMAGE_HR_MP3_DEFINED                                     */

#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
       case QDSP_IMAGE_CAM_AUDIO_CONC:
        if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_MP3){
           /* Get the PCM filter configuration data
           */
           audfmt_filter_ptr = voc_data_get_qtunes_pcm_filter();
           if (qdsp_cmd_set_audfmt_pcm_filter (
              (qdsp_cmd_audfmt_pcm_filter_type *) audfmt_filter_ptr) !=
                      QDSP_CMD_SUCCESS) {
                MSG_ERROR("QTUNES PCM Filter config failed",0,0,0);
           }
        } else if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_WAVE){
          /* Get the PCM filter configuration data
          */
          audfmt_filter_ptr = voc_data_get_qsynth_pcm_filter();
          if (qdsp_cmd_set_audfmt_pcm_filter(
             (qdsp_cmd_audfmt_pcm_filter_type *) audfmt_filter_ptr) !=
                    QDSP_CMD_SUCCESS) {
                MSG_ERROR("Graphics QSYNTH PCM Filter control config failed",0,0,0);
          }
        }
       break;
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
#endif /*FEATURE_AUDIO_CAMERA_CONCURRENCY */
    default:
      break;
  }
#endif /* FEATURE_AUDFMT_FIR_FILTER */

#if defined(FEATURE_QSYNTH_ADRC) || defined(FEATURE_QTUNES_ADRC)
  /* Get the ADRC control data
  */
  if (qdsp_cmd_set_audfmt_adrc_ctl( (qdsp_cmd_audfmt_adrc_ctl_type *)
                     voc_data_get_audfmt_adrc_ctl() ) != QDSP_CMD_SUCCESS) {
    MSG_ERROR("AUDFMT ADRC control config failed",0,0,0);
  }
#endif /* FEATURE_QSYNTH_ADRC || FEATURE_QTUNES_ADRC */
}

/* <EJECT> */
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
/*===========================================================================

FUNCTION voc_pp_ctl

DESCRIPTION
  Registers the post processing callback with the vocoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occur

==========================================================================*/
void voc_pp_ctl
(
  voc_pp_cb_type  pp_cb,
    /* Callback to receive the data */
  voc_pp_type_type pp_type
)
{
  boolean                          dsp_ready     = FALSE;
#ifdef FEATURE_AVS_I2SSBC
  voc_state_control_type           *state;
#endif /* FEATURE_AVS_I2SSBC */
#if defined(FEATURE_QTUNES_COMMON) || defined(FEATURE_QSYNTH_COMMON)
  qdsp_cmd_status_type status;
  voc_pp_type_type pp_config;

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  if (((pp_type == VOC_PP_TYPE_GENERIC) &&
       (voc_get_bt_state() == VOC_BT_STATE_DISABLED))
#ifdef FEATURE_SBC_CODEC
      || (pp_type == VOC_PP_TYPE_SBC)
#endif
#ifdef FEATURE_SBC_DSP_CODEC
      || (pp_type == VOC_PP_TYPE_DSP_SBC)
#endif
     )
#endif
  {
  voc_data_register_pp_func(pp_cb);
    voc_data_set_pp_type(pp_type);

#ifdef FEATURE_QSYNTH_COMMON
    dsp_ready |= check_interlock(VOC_UNLOCK_QSYNTH);
#endif

#ifdef FEATURE_QTUNES_COMMON
    dsp_ready |= check_interlock(VOC_UNLOCK_QTUNES);
#endif

#ifdef FEATURE_AVS_I2SSBC
  state = voc_state_get_state_data();
  if (state->current_image == QDSP_IMAGE_I2SSBC) {
#ifdef FEATURE_GRAPH_AUDIO
    dsp_ready |= check_interlock(VOC_UNLOCK_GRAPH);
#endif
  }
#endif /* FEATURE_AVS_I2SSBC */

    if(dsp_ready)
  {
      pp_config = voc_data_get_pp_type();
#ifdef FEATURE_SBC_CODEC
      if (pp_config == VOC_PP_TYPE_SBC)
      {
        status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_SBC);
      }
      else
#endif /* FEATURE_SBC_CODEC */
#ifdef FEATURE_SBC_DSP_CODEC
      if (pp_config == VOC_PP_TYPE_DSP_SBC)
      {
        status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_DSP_SBC);
      }
      else
#endif
  {
        status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_GENERIC);
      }
    if ((pp_cb != NULL) && (status == QDSP_CMD_SUCCESS))
    {
      voc_state_set_pp_status(VOC_PP_STATUS_ENABLED);
    }
    else
    {
      /* If we are here it means either the callback was NULL or that the
       * current image doesnot support Post Processing so we mark our
       * state as disabled. If callback is not NULL the next time an
       * image that supports post processing is loaded onto the DSP
       * the interface will be enabled in voc_state.
       */
      voc_state_set_pp_status(VOC_PP_STATUS_DISABLED);
    }
  }
  else
  {
    voc_state_set_pp_status(VOC_PP_STATUS_DISABLED);
  }
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  } else {
    voc_data_save_pp_func(pp_cb, VOC_PP_TYPE_GENERIC);
  }
#endif /* FEATURE_SBC_CODEC */
#endif /* defined(FEATURE_QTUNES_COMMON) || defined(FEATURE_QSYNTH_COMMON) */
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_pp_codec_stereo

DESCRIPTION
  This is the function called by snd to see if the current codec is stereo or
  mono

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
boolean voc_pp_stereo_codec
(
  void
)
{
  return voc_data_stereo_codec();
}
#endif /* FEATURE_ARM_POST_PROCESSING || FEATURE_AVS_WB_PCM_RX_INTERFACE */

#ifdef FEATURE_ARM_POST_PROCESSING
/* <EJECT> */
/*===========================================================================

FUNCTION voc_pp_client_data

DESCRIPTION
  This is the function called by snd to pass the client data to DSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
voc_status_type voc_pp_client_data
(
  uint16    buf_length,
  uint16    *buffer,
  uint16    buf_index
)
{
  qdsp_cmd_status_type    result;

  result = qdsp_cmd_pp_client_data(buf_length, buffer, buf_index);

  if (result == QDSP_CMD_SUCCESS)
  {
    return VOC_DONE_S;
  }
  else
  {
    return VOC_CMD_FAILED_S;
  }
}

#endif  /* FEATURE_ARM_POST_PROCESSING */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/* <EJECT> */
/*===========================================================================

FUNCTION voc_sbc_set_cb

DESCRIPTION
  Sets the BT SBC codec callback with the vocoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_sbc_set_cb
(
  voc_pp_cb_type  pp_cb
    /* Callback to receive the data */
)
{
#ifdef FEATURE_SBC_CODEC
  voc_data_save_pp_func(pp_cb, VOC_PP_TYPE_SBC);
#elif defined(FEATURE_SBC_DSP_CODEC)
  voc_data_save_pp_func(pp_cb, VOC_PP_TYPE_DSP_SBC);
#endif
}
#endif /* FEATURE_SBC_CODEC */

#if defined FEATURE_SBC_CODEC || defined FEATURE_SBC_DSP_CODEC || \
    defined FEATURE_AVS_WB_PCM_RX_INTERFACE
/* <EJECT> */
/*===========================================================================

FUNCTION voc_free_wb_pcm_rx_interface_buffer

DESCRIPTION
  This function calls a qdspcmd function to free the post processing
  buffer sent to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_free_wb_pcm_rx_interface_buffer( uint16 buf_index )
{
  qdsp_cmd_pp_free_read_buffer(buf_index);
}

#endif /* FEATURE_SBC_CODEC || FEATURE_AVS_WB_PCM_RX_INTERFACE */

#ifdef FEATURE_VOC_DTMF_DETECT
/* Last detected value, including NONE */
voc_dtmf_detected_status_type voc_last_rx_dtmf_detected;
voc_dtmf_detected_status_type voc_last_tx_dtmf_detected;
/* Last detected key, NONE means no key is currently held down */
voc_dtmf_detected_status_type voc_last_rx_dtmf_held;
/*===========================================================================

FUNCTION VOC_DTMF_DETECT_CB

DESCRIPTION
  This function sends DTMF detection callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function executes in DSP interrupt context.

===========================================================================*/
void voc_dtmf_detect_cb (
  qdsp_cmd_dtmf_detected_status_type rx_status,
  qdsp_cmd_dtmf_detected_status_type tx_status
) {
  voc_dtmf_detected_status_type status;

  status = (voc_dtmf_detected_status_type) rx_status;
  if((voc_rx_detect_cb != NULL) && (status != voc_last_rx_dtmf_detected)) {
    voc_last_rx_dtmf_detected = status;
    if(status != VOC_DTMF_DETECTED_NONE) {
      /* Compare good dtmf detect with previous detect (checks if the dtmf is
      ** continuously detected -- like a key is held down)
      */
      if(voc_last_rx_dtmf_held != status) {
        if(voc_last_rx_dtmf_held != VOC_DTMF_DETECTED_NONE) {
          /* Key detected is different.  Return NONE to signal key release,
          ** then return new key
          */
          voc_dtmf_detected(TRUE, VOC_DTMF_DETECTED_NONE);
        }
        voc_last_rx_dtmf_held = status;
        voc_dtmf_detected(TRUE, status);
      }
      rex_set_timer(&voc_dtmf_detect_delay_timer, 40);
    }
  }

  /* TX detection doesn't have the repeating interrupt issue that RX
  ** detection has.
  */
  status = (voc_dtmf_detected_status_type) tx_status;
  if((voc_tx_detect_cb != NULL) && (status != voc_last_tx_dtmf_detected)) {
    voc_last_tx_dtmf_detected = status;
    voc_dtmf_detected(FALSE, status);
  }
}

/*===========================================================================

FUNCTION
  VOC_DTMF_DETECT_DELAY_CB

DESCRIPTION
  This function receives a callback after a certain amount of time elapses
    to verify the DTMF detected is finished playing.  It

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function executes in VOC Task context.

===========================================================================*/
void voc_dtmf_detect_delay_cb () {
  /* Reset key */
  INTLOCK();
  voc_last_rx_dtmf_held = VOC_DTMF_DETECTED_NONE;
  INTFREE();
  /* Return completed DTMF detection */
  voc_dtmf_detected(TRUE, VOC_DTMF_DETECTED_NONE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_DTMF_DETECT_ENABLE

DESCRIPTION
  This function sends command to enable/disable DTMF detection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_dtmf_detect_enable ()
{
  qdsp_cmd_dtmf_detect_cmd_type dtmf_cmd;

  if(voc_rx_detect_cb == NULL) {
    if(voc_tx_detect_cb == NULL) {
      dtmf_cmd = QDSP_CMD_DTMF_DETECT_DISABLED;
    } else {
      dtmf_cmd = QDSP_CMD_DTMF_DETECT_TX;
    }
  } else {
    if(voc_tx_detect_cb == NULL) {
      dtmf_cmd = QDSP_CMD_DTMF_DETECT_RX;
    } else {
      dtmf_cmd = QDSP_CMD_DTMF_DETECT_BOTH;
    }
  }

  voc_last_rx_dtmf_detected = VOC_DTMF_DETECTED_NONE;
  voc_last_tx_dtmf_detected = VOC_DTMF_DETECTED_NONE;
  voc_last_rx_dtmf_held = VOC_DTMF_DETECTED_NONE;

  qdsp_cmd_dtmf_detect(dtmf_cmd, voc_dtmf_detect_cb);
}

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_DTMF_DETECT

DESCRIPTION
  This function enables/disables DTMF detection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_dtmf_detect (
  voc_dtmf_detect_cb_type rx_detect,
  voc_dtmf_detect_cb_type tx_detect
) {

  voc_rx_detect_cb = rx_detect;
  voc_tx_detect_cb = tx_detect;

  /* Make any changes based on current settings
  */
  if(check_interlock(VOC_UNLOCK_VOC_NOT_RESET)) {
    voc_dtmf_detect_enable();
  }

}
#endif /* FEATURE_VOC_DTMF_DETECT */

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION voc_set_av_sync

DESCRIPTION
  Registers the av_aync callback

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void voc_set_av_sync
(
  voc_av_sync_dec_enum_type  decoder_type,
  uint32                     av_sync_interval,
  voc_av_sync_interval_type  av_sync_mode,
  /* Callback to receive the data */
  voc_av_sync_cb_type        av_sync_cb
) {
  uint16 sample_rate = voc_data_get_num_samples_per_sec();

  switch (decoder_type) {
#ifdef FEATURE_QTUNES_AAC
    case VOC_AV_SYNC_DEC_AAC:
#if defined(QDSP_IMAGE_AAC_DEFINED)             || \
    defined(QDSP_IMAGE_ENH_AAC_PLUS_DEFINED)    || \
    defined(QDSP_IMAGE_H264D_AAC_DEFINED)       || \
    defined(QDSP_IMAGE_QTV_AAC_H264_DEFINED)    || \
    defined(QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED)   || \
    defined(QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED) || \
    defined(QDSP_IMAGE_QTV_AAC_DEFINED)
      switch(voc_state_get_state_data()->current_image){
#ifdef QDSP_IMAGE_AAC_DEFINED
        case QDSP_IMAGE_AAC:
#endif /* QDSP_IMAGE_AAC_DEFINED */
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
        case QDSP_IMAGE_ENH_AAC_PLUS:
#endif /* QDSP_IMAGE_ENH_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_H264D_AAC_DEFINED
        case QDSP_IMAGE_H264D_AAC:
#endif /* QDSP_IMAGE_H264D_AAC_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif/*QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED*/
#ifdef QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED
        case QDSP_IMAGE_MP4D_AAC_PLUS:
#endif /* QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED
        case QDSP_IMAGE_MP4D_AAC_NO_VPP:
#endif
#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
        case QDSP_IMAGE_QTV_AAC:
#endif /* QDSP_IMAGE_QTV_AAC_DEFINED */

          if (sample_rate < 32) {
            av_sync_interval <<= 1;
          }
          break;
      }
#endif /* QDSP_IMAGE_AAC_DEFINED             ||
          QDSP_IMAGE_ENH_AAC_PLUS_DEFINED    ||
          QDSP_IMAGE_H264D_AAC_DEFINED       ||
          QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED   ||
          QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED ||
          QDSP_IMAGE_QTV_AAC_DEFINED */

      /* Fall thru */
#endif /* FEATURE_QTUNES_AAC */

#ifdef FEATURE_QTUNES_COMMON
#ifdef FEATURE_QTUNES
    case VOC_AV_SYNC_DEC_MP3:
#endif /* FEATURE_QTUNES */
#ifdef FEATURE_REAL_AUDIO
    case VOC_AV_SYNC_DEC_RA:
#endif /* FEATURE_REAL_AUDIO */
#ifdef FEATURE_WMA
    case VOC_AV_SYNC_DEC_WMA:
#endif /* FEATURE_WMA */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
    case VOC_AV_SYNC_DEC_AMR_WBPLUS:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

      if(av_sync_mode == VOC_AV_SYNC_MS) {
        av_sync_interval = sample_rate * av_sync_interval;
      }
      if(check_interlock(VOC_UNLOCK_QTUNES)
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
      || check_interlock(VOC_UNLOCK_MIXER_ADEC)
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
        ) {
        qdsp_cmd_set_av_sync((qdsp_cmd_av_sync_dec_enum_type) decoder_type,
                             (qdsp_cmd_av_sync_cb_type) av_sync_cb,
                             av_sync_interval,
                             sample_rate);
      }
      break;
#endif /* FEATURE_QTUNES_COMMON */

    case VOC_AV_SYNC_DEC_VOCODER:
      if (av_sync_mode == VOC_AV_SYNC_MS) {
        /* converting av sync interval from pcm to ms */
        if((av_sync_interval <= 20) && (av_sync_interval > 0)) {
          av_sync_interval = 1;
        } else {
          av_sync_interval /= 20;
        }
      }
      qdsp_cmd_set_av_sync(QDSP_CMD_AV_SYNC_DEC_VOCODER,
                           (qdsp_cmd_av_sync_cb_type) av_sync_cb,
                           av_sync_interval,
                           sample_rate);
      break;

#ifdef FEATURE_WEBAUDIO
#if defined(FEATURE_ADPCM) || defined(FEATURE_PCM)
    case VOC_AV_SYNC_DEC_WAVE:
      if(av_sync_mode == VOC_AV_SYNC_MS) {
        av_sync_interval = sample_rate * av_sync_interval;
      }
      if(check_interlock(VOC_UNLOCK_QSYNTH)) {
        qdsp_cmd_set_av_sync((qdsp_cmd_av_sync_dec_enum_type) decoder_type,
                             (qdsp_cmd_av_sync_cb_type) av_sync_cb,
                             av_sync_interval,
                             sample_rate);
      }
      break;
#endif /* FEATURE_ADPCM || FEATURE_PCM */
#endif /* FEATURE_WEBAUDIO */

    default:
      qdsp_cmd_set_av_sync(QDSP_CMD_AV_SYNC_DEC_INVALID,
                           (qdsp_cmd_av_sync_cb_type) NULL, 0, 0);
      break;
  }
}
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION voc_mm_parm

DESCRIPTION
  Select a new set of paramters for multimedia recording

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_mm_param (
  voc_sample_rate_type  sample_rate,
  voc_mm_rec_chan_type  chan_index,
  uint16                bit_per_spl,
  voc_mm_rec_cb_fn_type isr_cb_func
)
{

  voc_mm_rec_cb_fn = isr_cb_func;

  voc_data_set_mm_parm (sample_rate,
                        chan_index,
                        bit_per_spl);
}

/*===========================================================================

FUNCTION VOC_MM_REC_UPDATE_READ_COUNT

DESCRIPTION
  Reset the read counter to DSP for MultiMedia recording.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_mm_rec_update_read_count (
  uint32    read_cnt
)
{
  qdsp_cmd_mm_rec_update_read_count(read_cnt);
}

/*===========================================================================

FUNCTION voc_mm_rec_get_data

DESCRIPTION
  Get the encoded data frame from the qdsp multimedia encode buffers.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_mm_rec_get_data(
  voc_mm_record_format_type format,
  voc_mm_record_buf_type  *buf,
  voc_mm_record_data_type *frame_info
)
{
  boolean do_read;

  do_read = FALSE;

  switch (format) {
#ifdef FEATURE_AAC_REC
    case VOC_MM_FORMAT_AAC:
      do_read = check_interlock(VOC_UNLOCK_GRAPH_AUDIO);
      break;
#endif /* FEATURE_AAC_REC */

    default:
      return (FALSE);
  }

  if (do_read) {
    return (qdsp_cmd_mm_rec_get_data((qdsp_cmd_record_buf_type *)buf,
                                     (qdsp_cmd_record_data_type *)frame_info));
  }

  return (FALSE);
}

/*===========================================================================

FUNCTION voc_mm_rec_check_config

DESCRIPTION
  This function checks wether multi media recording configurations have been
  changed.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  TURE  -- Nothing changed, go ahead record
  FALSE -- Something changed

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_mm_rec_check_config (void)
{
  boolean                ret_val;
  voc_state_control_type *state;
  voc_mm_rec_parm_type   *new_cfg;
  voc_aac_config_type    *curr_cfg;

  new_cfg  = voc_data_get_mm_parm();
  state    = voc_state_get_state_data();
  curr_cfg = &state->aac_config;

  ret_val  = FALSE;

  /* Only signal the vocoder state machine if the config is different from the
   * current config, to prevent it from transiting to INIT if possible */
#ifdef FEATURE_AAC_REC
  if (state->config == VOC_CAP_AAC_REC) {
    if ((state->mm_rec_config_pending == TRUE)                           ||
        (new_cfg->sample_rate_index   != curr_cfg->sample_freq_index)     ||
        (new_cfg->rec_chan_index      != curr_cfg->channel_configuration) ||
        (new_cfg->bit_per_spl         != curr_cfg->bit_per_spl)) {

      state->mm_rec_config_pending     = TRUE;
      curr_cfg->sample_freq_index      = new_cfg->sample_rate_index;
      curr_cfg->channel_configuration  = new_cfg->rec_chan_index;
      curr_cfg->bit_per_spl            = new_cfg->bit_per_spl;

      /* Tell the state machine to update the values on the DSP */
      (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );

      ret_val = TRUE;
    } else if (state->mm_rec_config_pending == FALSE &&
               state->config == VOC_CAP_AAC_REC) {
      ret_val = FALSE;
    }
  }
#endif /* FEATURE_AAC_REC */

  return (ret_val);
}

/*===========================================================================

FUNCTION voc_mm_rec_cb_isr

DESCRIPTION
  Multimedia record isr callback function.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_mm_rec_cb_isr(void)
{
  if (voc_mm_rec_cb_fn != NULL) {
    /* Send indication that encoded frame(s) are ready */
    voc_mm_rec_cb_fn();
  }
}
#endif /* #ifdef FEATURE_MM_REC */


#ifdef FEATURE_QDJ_TIMEWARP
/*===========================================================================

FUNCTION voc_set_timewarp

DESCRIPTION
  Sets timewarp factors in the qdsp.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_timewarp(
  boolean enable,
  uint16  factor)
{
  qdsp_cmd_set_timewarp(enable, factor);
}

#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif /* MSMAUD_VOC_4GV_VOIP */

#ifdef MSMAUD_VOC_4GV_WB_VOIP
/*===========================================================================

FUNCTION voc_set_4gv_wb_timewarp

DESCRIPTION
  Sets phase factors for timewarp in the qdsp.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_set_4gv_wb_timewarp(
  boolean  enable,
  uint16   run_length,
  uint16   phase_offset)
{
  qdsp_cmd_set_4gv_wb_timewarp(enable, run_length, phase_offset);
}
#endif /* MSMAUD_VOC_4GV_WB_VOIP */
#endif /* FEATURE_QDJ_TIMEWARP */

#ifdef FEATURE_OEMSBL_AUDIBLE_TONE
/*===========================================================================

FUNCTION voc_set_oemsbl_cb

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
void voc_set_oemsbl_cb(voc_oemsbl_cb_func_ptr_type func_ptr)
{
  voc_state_set_oemsbl_cb(func_ptr);
}
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION voc_ecodec_clock_ctl

DESCRIPTION
 This function will enable/disable (external)auxpcm clock.

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S - Command completed successfully

SIDE EFFECTS


===========================================================================*/
void voc_ecodec_clock_ctl (
  boolean switchon
  /* True = turn ecodec clock ON, False = turn ecodec clock off */
)
{
   if(switchon){
    voc_state_enable_eclock();
   }
   else{
    voc_state_disable_eclock();
   }
}

#ifdef FEATURE_AUDIO_PCR_SUPPORT
/*===========================================================================

FUNCTION voc_audfmt_adjust_sample

DESCRIPTION
  This function add/drop(s) the sample

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
voc_status_type voc_audfmt_adjust_sample
(
boolean                        add_sample
)
{
  if (qdsp_cmd_af_adjust_sample(add_sample)!= QDSP_CMD_SUCCESS) {
    return(VOC_CMD_FAILED_S);
  }
  return(VOC_DONE_S);
}
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION voc_codec_update_isr

DESCRIPTION
  This function is called from the QDSP to update codec info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_codec_update_isr (uint16 dualmono)
{
  if ( voc_codec_update_cb != NULL ) {
    /* Tell sound/audfmt to return the next set of data
    cb will be snd_audfmt_codec_update_int
    voc_codec_update_sndptr must be &snd_array[1] */
    voc_codec_update_cb(dualmono, (void *)voc_adec_client_data);
  }
}

/*===========================================================================

FUNCTION voc_dual_mono_mode

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Audio is active.
  FALSE - Audio is not active.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_dual_mono_mode (
  uint16 dual_mono_mode
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;
  boolean         check_state = FALSE;

  voc_data_set_dual_mono_mode(dual_mono_mode);

#ifdef FEATURE_QTUNES_COMMON
  check_state = check_state || check_interlock(VOC_UNLOCK_QTUNES);
#endif /* FEATURE_QTUNES_COMMON */

  if ( check_state ) {
    if ( qdsp_cmd_dual_mono_mode(dual_mono_mode) == QDSP_CMD_SUCCESS ) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

/*===========================================================================

FUNCTION voc_sbr_mode

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Audio is active.
  FALSE - Audio is not active.

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_sbr_mode (
  uint16 sbr_mode
) {
  voc_status_type ret_val = VOC_CMD_FAILED_S;
  boolean         check_state = FALSE;

  voc_data_set_sbr_mode(sbr_mode);

#ifdef FEATURE_QTUNES_COMMON
  check_state = check_state || check_interlock(VOC_UNLOCK_QTUNES);
#endif /* FEATURE_QTUNES_COMMON */

  if ( check_state ) {
    if ( qdsp_cmd_sbr_mode(sbr_mode) == QDSP_CMD_SUCCESS ) {
      ret_val = VOC_DONE_S;
    }
  }

  return(ret_val);
}

#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

/* FM over BT AG */
/*===========================================================================

FUNCTION voc_process_fm_over_bt

DESCRIPTION
  Acquires/Release the vocoder and initiates/disconnect BT SCO link

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void voc_process_fm_over_bt(
  voc_aux_line_in_mode_type aux_mode
)
{

#if defined (FEATURE_AVS_BT_SCO_REWORK)
     voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
     bt_func_ptr = voc_data_get_bt_cb_func();
#endif/*AVS_BT_SCO_REWORK*/

  if(aux_mode == VOC_AUX_LINE_IN_MODE_ON)
  {
#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if (bt_func_ptr != NULL) {
      MSG_MED("Calling BT callback with PLAYBACK_SCO", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_SCO);
    }
#endif

    /* This means Sound Set device is called with BT */
    /* Acquiring Vocoder */
    MSG_MED("Acquiring Vocoder for FM playback over BT \n", 0, 0, 0);
    voc_acquire(VOC_ACQ_TEST, NULL);
  }
  else
  {
#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if (bt_func_ptr != NULL) {
      MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_STOP);
    }
#endif
    /* This means Sound Set device is called with BT */
    /* Releasing Vocoder */
    MSG_MED("Releasing Vocoder for FM playback stop over BT \n", 0, 0, 0);
    voc_release(VOC_ACQ_TEST);
  }
}
/*===========================================================================

FUNCTION voc_process_aux_line_in

DESCRIPTION
  Process the AuxPGA line in configuration

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void voc_process_aux_line_in(voc_aux_line_in_mode_type aux_mode)
{
#ifndef MSMAUD_SCMM
  voc_codec_type codec;
  uint8 volume;

  /* get the current codec */
  voc_data_get_codec(&codec);

  if(aux_mode == VOC_AUX_LINE_IN_MODE_ON)
  {
    /* Configure aux line in */

    if ( codec == VOC_CODEC_ON_CHIP_0 )
    {
      adie_services_configure_aux_line_in_on(ADIE_AUX_LINE_IN_HANDSET);
      volume = adie_services_get_aux_line_in_vol();
      adie_services_set_aux_line_in_vol(ADIE_AUX_LINE_IN_HANDSET, volume);
    }
    else if (codec == VOC_CODEC_STEREO_HEADSET) {
      adie_services_configure_aux_line_in_on(ADIE_AUX_LINE_IN_HEADSET);
      volume = adie_services_get_aux_line_in_vol();
      adie_services_set_aux_line_in_vol(ADIE_AUX_LINE_IN_HEADSET, volume);
    }
#ifdef FEATURE_SPEAKER_PHONE
    else if (codec == VOC_CODEC_SPEAKER) {
      adie_services_configure_aux_line_in_on(ADIE_AUX_LINE_IN_SPEAKER);
      volume = adie_services_get_aux_line_in_vol();
      adie_services_set_aux_line_in_vol(ADIE_AUX_LINE_IN_SPEAKER, volume);
    }
#endif /* FEATURE_SPEAKER_PHONE */
   else if (codec == VOC_CODEC_ON_CHIP_AUX)
   {
     adie_services_configure_aux_line_in_on(ADIE_AUX_LINE_IN_LINEOUT);
     volume = adie_services_get_aux_line_in_vol();
     adie_services_set_aux_line_in_vol(ADIE_AUX_LINE_IN_LINEOUT, volume);
   }
#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    #ifdef FEATURE_INTERNAL_SDAC
    else if (codec == VOC_CODEC_STEREO_USB) {
      adie_services_configure_aux_line_in_on(ADIE_AUX_LINE_IN_USB);
      volume = adie_services_get_aux_line_in_vol();
      adie_services_set_aux_line_in_vol(ADIE_AUX_LINE_IN_USB, volume);
    }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */
#ifdef FEATURE_BT_AG
    else if ((codec == VOC_CODEC_BT_AG) || (codec == VOC_CODEC_FM_OVER_BT_AG))
    {
      /* FM over BT AG */
      /* The following setting is not required */
      /* adie_services_configure_aux_line_in_on(ADIE_AUX_LINE_IN_TOADSP); */
      MSG_MED("FM on BT is activated\n", 0, 0, 0);
    }
#endif
    else
    {
      /* Print the error massage */
      MSG_ERROR(" AuxLine in Turn On failed, codec not supported :%d\n",
                 codec, 0, 0);
      return;
    }

    /* set the aux line flag to on state */
    voc_aux_line_in_flag = TRUE;

    voc_enable_codec_func();

  }
  else if(aux_mode == VOC_AUX_LINE_IN_MODE_OFF)
  {
    /* Deconfig aux line in adie */
    if( codec == VOC_CODEC_ON_CHIP_0 ) {
      adie_services_configure_aux_line_in_off(ADIE_AUX_LINE_IN_HANDSET);
    }
    else if (codec == VOC_CODEC_STEREO_HEADSET) {
      adie_services_configure_aux_line_in_off(ADIE_AUX_LINE_IN_HEADSET);
    }
#ifdef FEATURE_SPEAKER_PHONE
    else if (codec == VOC_CODEC_SPEAKER) {
      adie_services_configure_aux_line_in_off(ADIE_AUX_LINE_IN_SPEAKER);
    }
#endif /* FEATURE_SPEAKER_PHONE */
    else if( codec == VOC_CODEC_ON_CHIP_AUX ) {
      adie_services_configure_aux_line_in_off(ADIE_AUX_LINE_IN_LINEOUT);
    }
#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    #ifdef FEATURE_INTERNAL_SDAC
    else if (codec == VOC_CODEC_STEREO_USB) {
      adie_services_configure_aux_line_in_off(ADIE_AUX_LINE_IN_USB);
    }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */
#ifdef FEATURE_BT_AG
    else if ((codec == VOC_CODEC_BT_AG) || (codec == VOC_CODEC_FM_OVER_BT_AG))
    {
      /* FM over BT AG */
      /* The following setting is not required */
      /* adie_services_configure_aux_line_in_off(ADIE_AUX_LINE_IN_TOADSP); */
      MSG_MED("FM on BT is deactivated\n", 0, 0, 0);
    }
#endif
    else
    {
      /* Print the error massage */
      MSG_ERROR(" AuxLine in Turn On failed, codec not supported :%d\n",
                 codec, 0, 0);
      return;
    }

    /* set the aux line in flag to off state */
    voc_aux_line_in_flag = FALSE;
  }
#endif /* MSMAUD_SCMM */
}

/*===========================================================================

FUNCTION voc_check_aux_line_in

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
boolean voc_check_aux_line_in(void)
{
  return(voc_aux_line_in_flag);
}
/*===========================================================================

FUNCTION voc_process_aux_line_in_vol

DESCRIPTION
  This function process the aux line in volume set request

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void voc_process_aux_line_in_vol (uint8 volume)
{
#ifndef MSMAUD_SCMM
  voc_codec_type codec;

  /* get the current codec */
  voc_data_get_codec(&codec);

  /* set aux line in volume */
  if( codec == VOC_CODEC_ON_CHIP_0)
  {
    adie_services_set_aux_line_in_vol(ADIE_AUX_LINE_IN_HANDSET, volume);
  }
  else if (codec == VOC_CODEC_STEREO_HEADSET) {
    adie_services_set_aux_line_in_vol(ADIE_AUX_LINE_IN_HEADSET, volume);
  }
#ifdef FEATURE_SPEAKER_PHONE
  else if (codec == VOC_CODEC_SPEAKER) {
    adie_services_set_aux_line_in_vol(ADIE_AUX_LINE_IN_SPEAKER, volume);
  }
#endif /* FEATURE_SPEAKER_PHONE */
  else if (codec == VOC_CODEC_ON_CHIP_AUX) {
    adie_services_set_aux_line_in_vol(ADIE_AUX_LINE_IN_LINEOUT, volume);
  }
#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
  else if (codec == VOC_CODEC_STEREO_USB) {
    adie_services_set_aux_line_in_vol(ADIE_AUX_LINE_IN_USB, volume);
  }
#endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */
  else
  {
    /* Print the error massage */
    MSG_ERROR(" AuxLine in Turn On failed, codec not supported :%d\n",
               codec, 0, 0);
    return;
  }
#endif /* MSMAUD_SCMM */
}

/*===========================================================================

FUNCTION voc_set_idle_metering_enable

DESCRIPTION
This function sets the idlemetering flag in vocdata

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void voc_set_idle_metering_enable (uint16 idlemetering)
{
  voc_data_set_idle_metering(idlemetering);
}

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_CHECK_IN_CALL

DESCRIPTION
  This function verifies if we are currently in a call based on the acquire
  mask.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  True if in a non analog voice call.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_check_in_call (
  uint32 acquire_mask
) {
  uint32 mask;
  extern voc_state_control_type  voc_state_data;
  extern voc_client_config_type voc_client_config[VOC_MAX_ACQUIRE_TYPE];
  mask  = ((word) 1 << VOC_ACQ_CDMA);
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
#ifdef FEATURE_MVS
  mask |= ((word) 1 << VOC_ACQ_MVS);
  if(voc_state_data.acquire_mask & (1 << VOC_ACQ_VS))
  {
    if(voc_client_config[VOC_ACQ_VS].config &
       voc_state_data.config)
    switch(voc_client_config[VOC_ACQ_VS].config)
    {
    case VOC_CAP_4GV_NB_VOIP:
    case VOC_CAP_IS127_VOIP:
    case VOC_CAP_4GV_WB_VOIP:
       mask |= ((word) 1 << VOC_ACQ_VS); 
    }
  }
#endif /* FEATURE_MVS */
  mask |= ((word) 1 << VOC_ACQ_TEST);

  return((acquire_mask & mask) != 0);
}

#ifdef SND_TEST_AUXPCM_INTERFACE_MODES
#error code not present
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION voc_enable_slowtalk

DESCRIPTION
  This function enable/disable the slowtalk

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
voc_status_type voc_enable_slowtalk
(
  boolean enable
)
{
  /* check if slowtalk staus is changed or not? */
  if (enable != voc_data_get_slowtalk_enable_flag())
  {
  /* save the current status of the slowtalk */
  voc_data_set_slowtalk_enable_flag(enable);
  voc_update_slowtalk_config();
  }
  else
  {
    /* no change in the slowtalk status */
    MSG_HIGH("Slowtalk: no change in current status = 0x%x", enable, 0, 0);
  }
  return (VOC_DONE_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_set_slowtalk_params

DESCRIPTION
  This function setup the slowtalk params

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
voc_status_type voc_set_slowtalk_params
(
  voc_slowtalk_params_type *params
)
{
  voc_status_type status = VOC_CMD_FAILED_S;

  status = voc_cal_set_slowtalk_params(params);

  if (status == VOC_DONE_S)
  {
    voc_update_slowtalk_config();
  }

  return (status);
}

/*===========================================================================

FUNCTION voc_update_slowtalk_config

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_update_slowtalk_config( void )
{
  qdsp_cmd_slowtalk_parms_type *pSlowTalkParams = NULL;
  qdsp_cmd_slowtalk_static_params_flag static_params_flag;
  qdsp_cmd_status_type cmd_status = QDSP_CMD_FAILED;
  uint8 cmd_retry_cnt;
  voc_state_control_type *state;

  state = voc_state_get_state_data();

  if ((check_interlock(VOC_UNLOCK_VOCODER)
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
      || check_interlock(VOC_UNLOCK_MIXER_ADEC)
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
     ))
  {
  /* update the slowtalk paramters only if we are in vocoder state */
  if (qdsp_cmd_get_state() != QDSP_STATE_VOCODER)
  {
    MSG_HIGH("slowtalk: can't write slowtalk config to DSP rightnow", 0, 0, 0);
    return;
  }
  }
  else
  {
    /* DSP is not in proper state so we can't write the slowtalk
       paramters right now
    */
    return;
  }

  pSlowTalkParams = voc_cal_get_slowtalk_params();

  /* TODO: needs to take care about the WB or NB vocoder stuffs */

  switch(voc_data_get_slowtalk_static_params_status())
  {
    case VOC_SLOWTALK_STATIC_PARAMS_UPDATED:
      /* static parameters have already been updated so no need to set the
         statis params udpate flag on firmware */
      static_params_flag = QDSP_CMD_SLOWTALK_NO_UPDATE_STATIC_PARAMS;
      break;
    case VOC_SLOWTALK_STATIC_PARAMS_NOT_UPDATED:
      /* static parameters need to be updated so set the
         udpate flag on firmware */
      static_params_flag = QDSP_CMD_SLOWTALK_UPDATE_STATIC_PARAMS;
      break;
    default:
      static_params_flag = QDSP_CMD_SLOWTALK_NO_UPDATE_STATIC_PARAMS;
      break;
  }

  /* try for maximum 5 times. we can't retry infinitely */
  cmd_retry_cnt = 5;
  while (cmd_retry_cnt > 0)
  {
    /* write the new slowtalk parameters to DSP */
    cmd_status = qdsp_cmd_set_slowtalk_config(
                    voc_data_get_slowtalk_enable_flag(),static_params_flag,
                    pSlowTalkParams);

    /* is cmd successful? */
    if (cmd_status == QDSP_CMD_SUCCESS)
    {
      /* check if static parameters are updated or not? */
      if ((voc_data_get_slowtalk_static_params_status() ==
                          VOC_SLOWTALK_STATIC_PARAMS_NOT_UPDATED) &&
           (voc_data_get_slowtalk_enable_flag() == TRUE))
      {
        /* Mark that static parameters are updated */
        voc_data_set_slowtalk_static_params_status(
          VOC_SLOWTALK_STATIC_PARAMS_UPDATED);
      }
      /* we are done so return from this function */
      return;
    }
    else
    {
      /* ohh, cmd failed. let's retry till retry counter is zero */
      cmd_retry_cnt--;
    }
  }
}

#ifdef FEATURE_ULC_WARMUP_DELAY
/*===========================================================================

FUNCTION voc_get_headphone_delay

DESCRIPTION This function returns the delay required for headphone

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
timetick_type voc_get_headphone_delay( void )
{
  timetick_type warmup_ms = 10L;

  /* Set 370ms delay only when headset
  is detected ow set the delay as 10ms
  */
  if(voc_headphone_con == TRUE)
  {
    warmup_ms = 370L;
    voc_headphone_con = FALSE;
  }

  return(warmup_ms);
}

/*===========================================================================

FUNCTION voc_set_headphone_status

DESCRIPTION This function set the headphone connection status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_set_headphone_status (
  voc_codec_type  pcm_path
)
{
  static boolean headphone_on = FALSE;

  /* Set the headphone connection flag when headset is detected
  otherwise set the flag to FALSE */
  if(pcm_path == VOC_CODEC_STEREO_HEADSET && headphone_on == FALSE)
  {
    voc_headphone_con = TRUE;
    headphone_on = TRUE;
  }
  else if(pcm_path != VOC_CODEC_STEREO_HEADSET && headphone_on == TRUE)
  {
    headphone_on = FALSE;
    voc_headphone_con = FALSE;
  }
}
#endif /* FEATURE_ULC_WARMUP_DELAY */

/*===========================================================================

FUNCTION voc_register_pa_on

DESCRIPTION
  Regestering CB function when PAs are turned on, in the VOC context, this 
  registerd callback function is called, which posts a command in SND queue 
  and wakes up the sound task.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S - Command completed succesfully

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_register_pa_on ( voc_pa_on_cb_func_ptr_type  func_ptr) 
{
  voc_state_set_pa_on_cb(func_ptr);
  return VOC_DONE_S;
}

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

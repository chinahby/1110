/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("QDSP2 Device Driver")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Q D S P 2   V O C O D E R   D R I V E R
            C A L I B R A T I O N    A N D   S T A T E   D A T A

GENERAL DESCRIPTION
  Keeps default calibration data for the QDSP2 Vocoder DSP.
  Maintains configuration state.

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS
  voc_data_set_codec
    Set the current Codec PCM path
  voc_data_get_codec
    Get the current Codec path
  voc_data_get_codec_class
    Get the current Codec path's class
  voc_data_get_codec_vr_type
    Get the current Codec path's VR type
  voc_data_get_codec_config
    Get the current Codec Configuration data
  voc_data_get_volume
    Get the current volume settings
  voc_data_set_rx_volume
    Set the RX volume
  voc_data_set_rx_mute
    Set RX muting
  voc_data_set_tx_mute
    Set TX muting
  voc_data_set_codec_gain
    Set the Internal Codec gain values
  voc_data_set_pcmpad_vol
    Set the volume bit for External CODEC Pad
  voc_data_set_pcmpad_ctl
    Set the control bits for External CODEC Pad
  voc_data_dfm_set_deviation
    Set the DFM deviation limit.
  voc_data_dfm_compander_ctl
    Turn the DFM compressor/expander on or off.
  voc_data_get_timing
    Get Frame Timing data
  voc_data_get_ec_mode
    Get the echo canceller mode
  voc_data_get_ec_params
    Get the echo canceller parameters
  voc_data_get_ns_mode
    Get the noise supressor mode
  voc_data_get_dtmf_tx_gain
    Get the DTMF Tx gain
  voc_data_get_rx_filter
    Get the RX PCM filter coefficients
  voc_data_get_tx_filter
    Get the TX PCM filter coefficients
  voc_data_get_agc
    Get the AGC parameters
  voc_data_set_dfm_sat_dcc
    Set the DFM SAT detected color code
  voc_data_get_dfm_sat_dcc
    Get the DFM SAT detected color code
  voc_data_compute_dbm
    This routine provides conversion from dbm values to the correspondent
  voc_data_adjust_dbm
    Adjusts the value returned by voc_data_compute_dbm by specified dbm
  voc_data_get_diags
    Get the Vocoder diagnoostic data
  voc_data_get_aud_process_override
    Get the audio process override setting
  voc_data_set_aud_process_override
    Set the audio process override setting
  voc_data_set_vr_mode
    Save the VR mode
  voc_data_set_vr_cal
    Save the VR calibration data for the current PCM path
  voc_data_get_vr_filter
    Get the Tx PCM filter coefficients to use during VR capture.
  voc_data_get_vr_ec
    Get the echo cancellation mode to use during VR capture.
  voc_data_get_vr_ns
    Get the noise suppression mode to use during VR capture.
  voc_data_get_qsynth_pcm_filter
    Get the QSYNTH PCM filter controls setting.
  voc_data_get_qtunes_pcm_filter
    Get the QTUNES PCM filter controls setting.
  voc_data_get_audfmt_adrc_ctl
    Get the QTUNES ADRC controls setting.
  voc_data_set_mic_gain_adjust
    Sets the mic gain adjustment value for the specified codec path
  voc_data_set_audcal_mode
    Enables or disables special audio calibration table.
  voc_data_set_mm_parm
    Set the current multimedia record parameters.
  voc_data_get_mm_parm
    Get the current multimedia record parameters.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 1999 - 2009 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/vocdata.c#59 $ $DateTime: 2009/10/15 05:43:22 $ $Author: kmodak $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/09   knm     Integrated new marimba codec driver for SCMM
10/07/09    rm     Added condition to handle codec mode for VoIP
09/07/09   vsud    Changes for resampler image imeplementation.
07/27/09    rm     Modified iir filter configuration functionality
07/13/09   rm      Adding support for 4GV_WB_VoIP.
07/07/09   rm      Added changes for QCamCorder Turbo image 
07/02/09   kkm     For the image suppports A2DP added QTV_AAC_H264 image as 
                   it is missed previously. Fixed CR #186393
06/22/09    vm     Added the code to return the flag for adrc filter when the
                   decoder is neither MP3 nor WAVE in the aud cam concurrency.
                   Fixed Cr:186390
06/15/09   kk      Fixed typo mistake in QDSP_IMAGE_QTV_AAC_H264_DEFINED
06/08/09   rm      Removed handling of MSMAUD_QSC6270_AUDIO_CAL
05/27/09    rm     Removed comppilation error due to presence of
                   VOC_CODEC_MODE_CDMA_4GV_WB_VOICE
05/23/09   sud     Changed VOC_DATA_VP3_FLUENCE_DATA_SIZE to 564 to support
                   VP3 for WB Vocoders
05/22/09    nr	   Added WB calibration tables and new SURF devices.
                   Fix for CR#177834, CR#173500
05/21/09    kk     Added support for H264 QCIF playback support
                   for low power mode(TURBO).
05/11/09    rm     QCamCorder FW's Vocoder State Paths has changed
                   to Voc Combo's state path. The SW chnages are done
                   to adapt to those changes.
05/13/09   rm      EVRC-WB and IIR filter table related updates
05/08/09    vm     Added Support for Device switch from external to internal.
05/07/09    kk     Featurised the codec mode selection for BT.
05/07/09    kk     Fixed feature issue in H264 clips playback support.
05/05/09   knm     Added CONSTANT macro for dynamic adie calibration.
05/05/09    kk     Added support for H264 clip playback.
04/17/09   kkm     Codec Mode selection for QTV has been changed
04/09/09   av      Resolved compilation error due to previous check-in.
04/06/09   av      Adding VP3 Feature.
                   Support for TxPCM filter before EC
                   Updated copyright.
04/06/09   skr     The channel information for ADPCM should be take from the
                   spec that is updated by parsing the file. Fixed CR-176952
03/31/09   vm      Added support for bt_ag on 4gv_wb image
                   Fixed Cr 171405
01/19/09   anb     Added voc_data_get/set_qtv_cb_func so that
                   QTV can be informed about mod/image swap event.
                   Fixes Cr 163807(SR-1133583)
03/10/09   sj      Added slowtalk feature support
01/30/09   skr     Changes for DTMF support for Audio camera concurrency image
                   in the camera Preview mode only.
01/19/09   bk      Featurized for setting clock rate of DAC to run at 32K
                   for 32 polyphony std midi playback. Ported this fix from
                   SC2X STD MIDI builds.
01/07/09   skr     Fixed Compilation issues for Mid Tier ULC build
12/26/08    rm     Added RVE API changes
12/23/08    vm     Added test code for AUXPCM testing support
12/22/08   knm     Added post-proc MIPs & modules for acc image.
12/19/08    rm     Fix for CR 164515 - Voice(Qcelp) recording fails after calling(4GV)
12/18/08    sj     Don't disable the Tx and Rx HPF filters for WB voice calls
                   Fixed CR 161301.
16/08/08   skr     use ACC_ADPCM module for files having 1channel,4bits/sample
                   512 block size , and 32Khz sampling Frequency
12/08/08    rm     Modification of wnr and rve parameters
11/26/08   skr     Modified the code to fix THIN UI compilation issues.
11/25/08    rm      CR 153432 - Made proper fix in voc_data_get_codec_mode() to
                   select correct codec mode for combo vocoder image.
11/24/08   skr     Added code for Audio + camera Concurrency
11/18/08    dp     Added support for playing NB-AMR on A2DP
10/17/08   vm      added idlemetring support for qtv image
09/24/08   skr     updated the Default configuration for Qensemble.
28/08/08    rm	   Added support for RVE
08/26/08   anb     Added support for DSP based WMAPRO
06/31/08   vsud    featurized voc cap ac and mp3 properly.
05/14/08    sr     New API voc_data_get_default_frame_rate(image,codec).
                   Fix for CR 134407 - 16khz PCM sampling. Updated copyright.

05/08/08    pl     CR 143215 - Eliminated choosing the adrc paramter based on hybrid
                   mode featurization in voc_data_get_audfmt_adrc_ctl() for
                   qsynth.
05/01/08    pl     Fixed CR - 115224 Modified voc_data_convert_to_qfactor and
                   voc_data_convert_from_qfactor
03/17/08  ymc/anb  Modified codec gain ctrl api to control gain changes.
                   Fix for SC2X codec pop issue through MSMAUD_PCM_DC_OFFSET feature.
03/14/08    az     Fixed the change necessary for the volume.
03/07/08   skr     Fixed CR 139598.changes done in voc_data_get_mm_rx_gain()
02/15/08   anb     Fixed compilation error for SC2X prepaid build
02/07/08    kk     Added support for Qensemble phase 2 interface changes.
01/30/08    az     Include the support for the In call Qtv.
01/15/08    hs     Added support ISDB-T dual mono output control
01/07/08    az     Included the in call midi and the new in call architecture.
11/21/07   anb     Fixed compilation errors for SC2X prepaid build
10/10/07   sk      Added FEATURE_AUDFMT_QCONCERT_PLUS support.
                   Added check in voc_data_set_qconcert_params() for Geometry
                   values for the FF/SF when Qconcert plus is enabled.
11/15/07    az     Featurized the code for In call to avoid compilation for
                   other targets.
11/14/07    hs     Added QEnsemble support
11/02/07    az     Included the support for the in call wma.
10/26/07    az     Included the support for the in call AAC.
09/18/07   ymc     Disabled front end high pass filters for wideband vocoders.
09/07/07   ymc     Updated AMR_WB WCDMA timing parameters.
08/28/07   ymc     Added crystal speech support.
           ymc     Fixed missing AACENC featurization problem.
08/23/07   ymc     Added wideband EC parameters support.
08/20/07   hs      Support AAC stand alone encoding.
06/14/07   anb     Modified voc_data_image_supports_bt_ag to fix SR=929499.
06/13/07    sg     Added support for selection of the AAC module based on MIPs
                   requirements.
06/12/07   pl/sk   Modified qsynth output sample rate to 48K for 65nm or
                   newer target.
06/08/07    sg     Added support for selection of the MP3 module based on MIPs
                   requirements.
04/20/07  sud/anb  Added PMIC MIC-BIAS support for QSC60X5.
03/15/07    kan    Added 4GV-WB Code
02/26/07   kan     Added AMR-WB changes
01/23/07    kk     Fixed the issue related to adding 44k sample rate.
01/09/07   suk     Added QDSP_IMAGE_VOC_COMBO_VOIP support.
                   Modified voc_data_get_codec_mode() to select the correct
                   audio calibration for combo vocoder image. It was defaulting
                   to CDMA voice audio calibration earlier.
                   Updated Copyright text.
11/20/06   suk     Added support for VOIP Combo Vocoder, and Combo vocoder
                   with G711 and 4GV.
10/09/06    kk     fixed,sending wrong sampling freq index for AAC files
09/18/06  anb/ay   Added AMR-WB+ SCO/A2DP support.
09/08/06    pl     Added support for 44K clock.
09/05/06    ay     Added AMR-WB+ support.
08/29/06    hz     Added support for incall MP3 on 65nm platforms.
08/22/06    sp     Added QVP_H263P3_AMR image in image_supports_bt_ag.
07/27/06   anb     Added the FEATURE_AVS_BT_SCO_REWORK which enables the AVS to send
                   messages to the BT driver
07/26/06  nm/ymc   Added FEATURE_AVS_NEXTGEN_EC.
07/26/06  nm/ymc   Added voc_data_[sg]et_1x_tx_rate_factor.
07/20/06   anb     Modified voc_data_image_supports_bt_ag() to support RA,WMA,AAC media
                   play on BT-Sco Head set.
                   Needs FEATURE_AVS_AUDIO_DECODER_SCO to be defined to support this.
07/12/06    hz     Added support for incall mp3 GSM/UMTS, BTSCO and PostProc
                   features.
07/10/06   act     Modified ADIE power-down config to select alternate
                   power-down sequence if HSSD circuit needs to be enabled
                   for the current codec.
06/29/06    sk     Added Bluetooth codec support for combo vocoder image.
06/26/06    pl     Added new functions to convert volume from unit of mb
                   to absolute value and vice versa.
                   Added new functions to convert 16.16 fixed value
                   to absolute value and vice versa.
03/27/06    ak     Added support for getting and setting sidetone mode.
03/24/06    pl     Seperate FIR filter out of FEATURE_QSYNTH_ADRC and
                   FEATURE_QTUNES_ADRC.
03/14/06    sp     Fixed I2S SBC sample rate issues.
03/12/06    pl     Modified qconcert code to apply different geometry value
                   for different sampling rate.
03/09/06    hz     Added support for evrc in-call MP3 support.
02/21/06   act     Modified voc_data_set_rx_mute() to handle muting for
                   tone playback as a special case in order to support
                   DTMF mixing functionality.
02/03/06    sm     Added 4GV VOIP support
01/31/06    lg     Added support for 6550-90nm images
01/31/06    hz     Added support for in-call mp3 feature.
01/30/06    sp     Added support for I2S SBC interface.
01/30/06    pl     Changed delay setting of headphone 3d expander from -1
                   to -28.
01/16/06   act     Added QDSP_IMAGE_VFE_YCBCR and QDSP_IMAGE_VFE_BAYER
                   to switch statement in voc_data_get_codec_mode() to set
                   codec mode to VOC_CODEC_MODE_MULTIMEDIA for those images.
01/14/06    pl     Modified to apply different set of Qconcert parameters
                   depending on codec in use.
01/13/06   act     Fixed compile error with low-tier featureset.
12/09/05    hz     Added support for QVP_H263P3_EVRC image.
11/04/05    sk     Fixed Compiler Error in voc_data_get_sbc_sample_rate()
11/03/05    sk     Added ARM based SBC support for RA.
10/13/05    sp     Changed voc_data_get_codec_mode to list
                   QDSP_IMAGE_QVP_YUV_AMR_H263 and QDSP_IMAGE_QVP_YUV_AMR_MPEG4
                   under GSM so that proper cal values are returned.
09/18/05    sp     Added support for DSP based SBC encoder.
09/18/05   pl/st   Modified code to return sampling rate of new game audio
                   ADEC API; added voc_data_compute_dbm_from_voc(); added
                   QAudioFX functions: voc_data_get_3d_pos_output_mode(),
                   voc_data_set_3d_pos_enable_mode(),
                   voc_data_get_3d_pos_enable_mode().
09/16/05    hz     Added in-call QTV image support.
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
09/14/05    hz     Get rid of a junk character.
09/13/05    sp     Added Post Processing and BT A2DP support for WMA.
09/01/05    sg     Fixed feature-ization in voc_data_get_ec_mode().
08/31/05    sm     VOX support
            sm     Added voc_data_get_codec_mode()
08/10/05    sm     Fixed audio formats featurization.
07/13/05    sp     Added code to honor bitpool values preferred by BT A2DP
                   headset. Added A2DP support for Enhanced AAC+ image.
07/05/05    sm     Added G.723.1 support.
07/05/05    aw     Modified qsynth pcm filter for backward compatiblity.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/24/05    hz     Added support for MultiMedia/AAC recording.
05/18/05    ac     Modified code to support QTV AAC image with
                   voc_data_get_num_samples_per_sec().
05/04/05    aw     Replace PCM filter with IIR filter support for Qsynth.
                   Support stereo ADRC on qsynth.
04/28/05   ymc     Added 4GV NB support.
04/19/05    aw     Modified code to use voc_data_get_sample_rate() to get
                   current sample rate for Post Processing functions.
04/21/05   act     Changed name of voc_codec_type to voc_codec_class_type.
                   Reverted name of voc_codec_id_type to voc_codec_type.
04/12/05   act     Extended voc_data_audcal_mode_enabled to snd_test_block
                   overrides.
04/12/05    sg     Added voc_data_set_codec_cal() to allow the calibration to
                   set independently of voc_data_codec_inuse. Added function to
                   check if an image supports BT.
04/07/05   act     Added QSYNTH ADRC and QTUNES ADRC control to special audio
                   calibration table. Also added runtime support for disabling
                   the special audio calibration table.
04/05/05   act     Modified voc_data_get_codec_type to return codec type rather
                   than codec ID.
04/01/05   act     Changed use of specific codec IDs to generic codec types
                   where applicable. Added access functions to get type.
04/01/05    hz     Added support for Enhanced AAC Plus decoder.
03/25/05   ac/sg   Added support for A/V synchronization.
03/24/05    sg     Added support for QDSP_IMAGE_QVP_H263P3_AMR.
03/01/05    aw     Added support for Qconcert; preset EQ bands; STF.
02/22/05    sm     Added G.711 vocoder support.
           ymc     Added QVP_BAYER_AMR support in voc_data_get_codec.
02/15/05   act     Added AGC and EC params to audio calibration override
                   table. Added access function for EC params.
02/15/05    sm     Added FEATURE_IIR_FILTER
02/14/05    hz     Added support for WMA decode under QDSP_IMAGE_WM_DEFINED.
02/10/05    aw     Added support for QVP_MPEG4_EVRC and QVP_MPEG4_AMR.
02/05/05    st     Modified code to support HQ DLS config parameters.
01/07/05  sm/act   Added support for FEATURE_VOC_MIC_GAIN_ADJUST and
                   FEATURE_VOC_NV_CAL
12/10/04    sg     Added VT and preview images to list images that support BT.
12/03/04    sm     Added FEATURE_AUDIO_CALIBRATION, changed comfort noise vol.
12/01/04    aw     Added support for MP4D_AAC_NO_VPP image.
11/19/04    sp     Added support for Arm Post Processing.
            sg     Added support for QTV_AAC image.
11/04/04    sg     Force audio to handset if the current image does not support
                   BT.
10/15/04    hz     Added support for QDSP_IMAGE_MP4D_AAC_PLUS.
09/14/04    aw     Modified code to return correct voc_timing for QTV+VOC image.
08/19/04    sm     Zero FR silence and comfort noise volumes.
06/23/04    aw     Fixed voc_data_get_sample_rate to return correct sampe rate.
06/08/04    aw     Added support for Spectrum Analyzer and Equalizer.
                   Added voc_data_dtmf_duration to get DTMF duration.
                   Modified voc_data_get_sample_rate to return correct sample
                   rate for AACPlus.
06/01/04    aw     Added support for H264 image.
05/11/04    vp     Merged ADRC and PCM filter support MP3/AAC from
                   MSMSHARED_AVS.00.09.05.02.00. Added support for H263 images.
            sm     Re-featurized gsm afe settings.
05/05/04    sm     Added voc_data_gsm_afe_table, voc_data_get_gsm_afe()
03/03/04    st     Updated voc_data_get_sample_rate() to determine correct
                   sampling rate for QSynth images.
12/03/03    aw     Added audio support for QGL image.
11/24/03    aw     Modified qsynth ADRC and PCM filter function to pass in
                   output sampling rate.
11/22/03    sg     Added support for RealAudio codec.
11/07/03  aw/st    Added support for Qsynth DLS.
10/30/03    sm     Added multimode calibration support.
10/10/03    sg     Added support for MP4D_MP3 image.
            aw     Added MSMAUD_VOC_AMR_COMMON featurization.
05/13/03   ymc     Added GSM half rate support.
           ymc     Added GSM AMR timing support.
           ymc     Updated copyright.
04/18/03    aw     Added audio support for Q3D image.
04/17/03    sm     Added #include <string.h>
03/13/03    st     Added support for MP4D+AAC image.
02/28/03    st     Added code to allow EC/NS to be disabled by client.
02/19/03    st     Exposed voc_data_get_mm_rx_gain() for FEATURE_QSYNTH_ADPCM.
02/12/03    st     Modified so that voc_data_adie_audio_override() returns a
                   different pointer value if called successively. This allows
                   the vocoder driver to recognize a change in the ADIE config.
01/25/03    aw     Fixed featurization in voc_data_get_mm_rx_gain().
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
12/19/02    st     Modified voc_data_adie_audio_override() to copy the
                   num_to_delay parameter.
10/29/02    sg     Modified voc_data_get_adie_codec_config() to use the new
                   adie cal. interface.
09/13/02    st     Modified code to set proper codec values when audio override
                   mode is enabled.
09/06/02    sg     Updated ADIE power down config.
08/13/02  sg/st    Added support for AAC; changed default sampling rate to 32k.
07/09/02    sm     Synced to perforce tip.
           ymc     Moved adie power down configuration parameters to msmaud.h
                   by creating MSMAUD_ADIE_CODEC_PD_CONFIG_SEQ and
                   MSMAUD_ADIE_CODEC_PD_CONFIG_SIZE.
03/12/02    st     Modified code to take into account Tx DTMFs when setting Tx
                   mute; removed MSMAUD_FAST_FRAME_TIMING; modified ADIE
                   codec power-down sequence; replaced voc_data_set_codec_gain
                   with voc_data_get_codec_gain(); added
                   voc_data_get_dtmf_path_setting() and
                   voc_data_set_dtmf_tx_mute().
03/04/02   jkl     Added FEATURE_VR_PUFE.
01/15/02   ymc     Added GSM and WCDMA support.
           ymc     Updated copyright.
12/11/01    aw     Added MSMAUD_ADIE_CODEC_CONFIG.
11/20/01    sm     Recalibrated vocoder timing for 5500/5105/5100 MSMs.
09/04/01    aw     Added FEATURE_QSYNTH_ADRC. Added functions to get QSynth
                   PCM filter and ADRC from PCM path.
09/03/01    st     Added proper featurization around voc_data_get_mm_rx_gain().
07/24/01   ymc     Added support for loopback tests.
06/18/01  jkl/ym   Added FEATURE_ACP.
05/21/01   jkl     Removed Enhanced VR feature.
03/04/01    st     Modified voc_data_get_codec_config() function to accept
                   codec type.
01/26/01    st     Modified code such that voc_data_compute_dbm() now supports
                   calculations from -50dB to 50dB; modified default sampling
                   rate to 8k; added voc_data_get_mm_rx_gain().
12/10/00    st     Modified code to add proper featurization.
12/05/00   jkl     Merged VR4.0 functions. Added FEATURE_ENHANCED_VR,
                   FEATURE_DFM_ANSWER_MACHINE.
12/04/00    sm     Added voc_data_get/set_sample_rate().
09/21/00    sm     voc_data_get_codec() returns the pcm path only.  Added
                   voc_data_get_codec_config() to get the codec configuration.
07/15/00    st     Lint updates.
07/14/00   spf     Removed voc_data_set_tty_mode() and TTY overrides.
                   Lint updates.
06/11/00   spf     Updated references.
06/07/00    st     Added code to register and get amplifier enable/disable
                   functions for each codec path.
05/12/00   spf     Add TTY support: voc_data_tty_cal_ptr, voc_data_tty_mode
                   and voc_data_set_tty_mode().  Add TTY override for
                   tx_gain, codec_tx_gain, codec_st_gain, ec_mode and
                   ns_enable.
05/02/00   spf     Removed voc_data_set_dfm_sat_dcc_notify() and
                   voc_data_get_dfm_sat_dcc_notify().
            sm     Added get/set_aud_process_override().
03/08/00   spf     Moved DFM configuration to voccal.
                   Removed sample slipping support.
01/25/00    ro     Added support for VR. Use voc_ec_type and voc_ns_type.
11/30/99   spf     Changed vocoder timing parameters to {10,100,99}.
11/22/99   jkl     Added new parameters for AGC/AVC.
11/04/99   jkl     Modified SAT Threshold value.
10/18/99    st     Added support for audio AGC/AVC.
10/14/99   jkl     Added function to get Vocoder Diagnostics data.
10/04/99   jkl     Modified to convert dbm values to hardware values.
                   Reviewed PCM pad functions. Restored former vocoder timing.
09/25/98    ro     Updated vocoder timing and decoder interrupt advance to
                   increase decoder packet transaction window.
09/10/99   jkl     Included functions to access internal and external CODECs
08/26/99   spf     Pink Panther Rev2 support.
08/02/99   spf     Code review updates.
07/15/99   spf     DFM support.
06/15/99    ro     First version.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"           /* Customer Specific Definitions           */
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "voc.h"                /* Vocoder interface definition            */
#include "qdspcmd.h"            /* QDSP2 Driver Command Layer definitions  */
#include "vocdata.h"            /* Vocoder State and Cal Data Definitions  */
#include "msg.h"                /* Message logging macro                   */
#include "err.h"                /* Error logging macro                     */
#include <string.h>             /* string/memory manipulation routines     */
#include "assert.h"             /* Assert macro                            */
#include "vocstate.h"           /* Vocoder State Controller Definitions    */
#include "msmaud.h"             /* MSM HW interfaces for audio             */
#ifdef FEATURE_AUDFMT_EQUALIZER
#include "audioeq.h"            /* Audio Equalizer function definitions    */
#endif /* FEATURE_AUDFMT_EQUALIZER */
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
uint16 voc_data_idle_metering_flag; /* idlemetering variable for setting qdsp \
			                        idlemetering value*/
#ifdef SND_TEST_AUXPCM_INTERFACE_MODES
#error code not present
#endif
/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/* ===========================================================================
**
**                       S T A T E   V A R I A B L E S
**
** =========================================================================*/

/* The current call service mode (CDMA/WCDMA) - default is CDMA
*/
LOCAL voc_service_cfg_type  voc_data_call_service = VOC_SERVICE_CDMA;


/* Tx rate factor, RDR limit from Sysparm msg, for 13k/EVRC/4GV vocoders */
word voc_data_1x_tx_rate_factor = VOC_1X_TX_RATE_FACTOR_DEFAULT;
#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_AMR_WB_COMMON)

/* The current GSM encoder DTX mode state
*/
LOCAL voc_gsm_dtx_mode_type voc_data_gsm_dtx_mode = VOC_GSM_DTX_OFF;

#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR || MSMAUD_VOC_HR || MSMAUD_VOC_AMR_WB_COMMON */

#ifdef MSMAUD_VOC_AMR_WB_COMMON
/* The current initial AMR_WB codec mode state
*/
LOCAL voc_amr_wb_frame_rate_type voc_data_amr_wb_init_rate
                                 = VOC_AMR_WB_RATE_0660;

#endif /* MSMAUD_VOC_AMR_WB_COMMON */

#ifdef MSMAUD_VOC_AMR_COMMON
/* The current initial AMR codec mode state
*/
LOCAL voc_gsm_frame_rate_type voc_data_amr_init_rate = VOC_AMR_RATE_475;

#endif /* MSMAUD_VOC_AMR_COMMON */

#ifdef MSMAUD_VOC_G711
/* The current initial G.711 mode state
*/
LOCAL voc_g711_mode_type voc_data_g711_mode = VOC_G711_MODE_MULAW;
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
/* The current initial G.723 mode state
*/
LOCAL voc_g723_mode_type voc_data_g723_mode = VOC_G723_MODE_HIGH;
#endif /* MSMAUD_VOC_G723 */

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

/* The current TX, RX volume, RX mute state, and TX mute (comfort noise) state
*/
LOCAL qdsp_cmd_vol_ctrl_type  voc_data_vol = {
  QDSP_TX_UNITY_GAIN,
  0,
  QDSP_RX_MUTE_ENA_V,
  QDSP_COMFORT_NOISE_ENA_V
};

/* Volume values for Modem Mute
*/
LOCAL const qdsp_cmd_vol_ctrl_type voc_data_modem_mute = {
  QDSP_TX_UNITY_GAIN,
  0,
  QDSP_RX_MUTE_ENA_V,
  QDSP_COMFORT_NOISE_ENA_V
};

/* Volume values when software packet loopback is on
*/
LOCAL const qdsp_cmd_vol_ctrl_type voc_data_lbtest_vol = {
  0x2000,                   /* Tx volume */
  0x2000,                   /* Rx volume */
  QDSP_RX_MUTE_DIS_V,
  QDSP_COMFORT_NOISE_DIS_V
};

/* Internal CODEC gain values
*/
LOCAL qdsp_cmd_codec_gain_ctrl_type  voc_data_codec_gain = {
   0, 0, 0
};

/* CODEC gain values when software packet loopback is on
*/
LOCAL const qdsp_cmd_codec_gain_ctrl_type voc_data_lbtest_codec_gain = {
  0x1500,                  /* Tx Gain */
  0x1500,                  /* Rx Gain */
  0x0000                   /* Side Tone Gain */
};

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */

/* Flags to indicate Tx muting and DTMF Tx muting
*/
LOCAL boolean voc_data_tx_mute_enable      = TRUE;
LOCAL boolean voc_data_dtmf_tx_mute_enable = TRUE;

/* External CODEC pad values
*/
LOCAL uint16  voc_data_codec_pad = 0x1f00;  /* Dedicated Control Bits */

/* Mask for codec volume bits */
LOCAL byte voc_data_codec_mask = 0xff;

/* The current codec id
*/
LOCAL voc_codec_type voc_data_codec_inuse = VOC_CODEC_NONE;

/* The current codec classification
*/
LOCAL voc_codec_class_type voc_data_codec_class_inuse = VOC_CODEC_CLASS_NONE;

#ifdef FEATURE_PHONE_VR
/* The current codec vr type
*/
LOCAL voc_codec_vr_type voc_data_codec_vr_type_inuse = VOC_CODEC_VR_TYPE_NONE;
#endif /* FEATURE_PHONE_VR */

/* The current codec configuration
*/
LOCAL voc_pcm_path_cal_type *voc_data_pcm_cal_ptr = NULL;

#ifdef FEATURE_VOC_MIC_GAIN_ADJUST
/* The current codec mic gain adjustment value.
*/
LOCAL sint15 *voc_data_mic_gain_adjust_ptr = NULL;
#endif /* FEATURE_VOC_MIC_GAIN_ADJUST */

/* Registered codec enable function pointers.
*/
LOCAL voc_amp_enable_func_ptr_type voc_data_codec_enable_func[VOC_CODEC_MAX];

/* Flag to indicate if DFM calibration has been copied to RAM
 */
LOCAL boolean voc_data_dfm_cal_flag = FALSE;

/* Flag and structures used when audio override mode flag is enabled */
LOCAL boolean voc_data_aud_process_override_enabled = FALSE;

/* Audio override mode gains should be set to 0dB with zero sidetone */
LOCAL const qdsp_cmd_codec_gain_ctrl_type voc_data_aud_override_codec_gain = {
  0x4000,                  /* Tx Gain */
  0x4000,                  /* Rx Gain */
  0x0000                   /* Side Tone Gain */
};

/* PCM filters should be disabled for audio override mode */
LOCAL const qdsp_cmd_pcm_filter_type voc_data_aud_override_filter = {
  0, 0, 0, 0, 0, 0, 0
};

#ifdef FEATURE_IIR_FILTER
/* PCM IIR filters should be disabled for audio override mode */
LOCAL const qdsp_cmd_pcm_iir_filter_type voc_data_aud_override_iir_filter = {
  QDSP_CMD_IIR_FILTER_DISABLE,
  3,
  { 0,0,0,0,0 },
  { 0,0,0,0,0 },
  { 0,0,0,0,0 },
  { 0,0,0,0,0 },
  { 0,0,0,0,0 }
};

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/* PCM IIR filters for secondary mic should be disabled for
   audio override mode */
LOCAL const qdsp_cmd_pcm_iir_filter_type
voc_data_aud_override_second_mic_iir_filter = {
  QDSP_CMD_IIR_FILTER_DISABLE,
  5,
  { 0,0,0,0,0 },
  { 0,0,0,0,0 },
  { 0,0,0,0,0 },
  { 0,0,0,0,0 },
  { 0,0,0,0,0 }
};
#endif/*FEATURE_AVS_CRYSTAL_SPEECH*/

#endif /* FEATURE_IIR_FILTER */

/* DFM SAT
*/
LOCAL voc_scc_type voc_data_dfm_sat_dcc = VOC_SCC_INVALID;

#ifdef FEATURE_PHONE_VR
voc_vr_cal_type   *voc_data_vr_cal_ptr = NULL;  /* VR calibration data   */
voc_vr_mode_type  voc_data_vr_mode = VOC_VR_MODE_OFF;   /* VR mode       */
#endif

#ifdef FEATURE_QAUDIOFX
/* 3D positional enable mode */
voc_3d_pos_mode_enum_type voc_data_3d_pos_enable_mode = VOC_3D_POS_MODE_DISABLE;
#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
uint16 voc_data_dual_mono_ch_mode = 0;
uint16 voc_data_sbr_mode          = 0;
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

/* <EJECT> */
/* ===========================================================================
**
**                       D B M  C O N V E R S I O N   T A B L E
**
** =========================================================================*/
/* Maximum and Minimum ddbm that the function can generate.
*/

#define VOC_DBM_MIN   -5000           /* Minimum ddbm value               */
#define VOC_DBM_MAX    5000           /* Maximum ddbm value               */

#define VOC_DBM_STEP   100            /* Interval between successive ddbm */

#define VOC_MIN_GAIN   0x0000         /* Minimum Hardware Register value  */
#define VOC_MAX_GAIN   0xffff         /* Maximum Hardware Register value  */

/* RX volume is calculated based on firmware, so store the current dBm value
** locally.
*/
LOCAL sint15 voc_data_rx_vol_dbm = VOC_VOL_SILENCE;

/* This table contains precomputed hardware register values, based on the
** associated variation of ddbm,  for use with linear interpolation.
** The equation is: (val = exp((ddbm/20) * ln10)
** The index into this table is computed by the following equation:
**         index = floor((dbm-VOC_DBM_MIN) / VOC_DBM_STEP)
** Interpolation then takes the index and index+1 values from this table
** to find the gradient and compute a hardware register value.
*/
LOCAL const uint32 voc_data_dbm_table[] = {

      31 ,  /* -50 dbm */
      35 ,      39 ,      44 ,      50 ,      56 ,
      63 ,      70 ,      79 ,      89 ,      99 ,
     112 ,     125 ,     141 ,     158 ,     177 ,
     199 ,     223 ,     251 ,     281 ,     316 ,
     354 ,     398 ,     446 ,     501 ,     562 ,
     630 ,     707 ,     794 ,     891 ,     999 ,
    1122 ,    1258 ,    1412 ,    1584 ,    1778 ,
    1995 ,    2238 ,    2511 ,    2818 ,    3162 ,
    3548 ,    3981 ,    4466 ,    5011 ,    5623 ,
    6309 ,    7079 ,    7943 ,    8912 ,   10000 ,
   11220 ,   12589 ,   14125 ,   15848 ,   17782 ,
   19952 ,   22387 ,   25118 ,   28183 ,   31622 ,
   35481 ,   39810 ,   44668 ,   50118 ,   56234 ,
   63095 ,   70794 ,   79432 ,   89125 ,  100000 ,
  112201 ,  125892 ,  141253 ,  158489 ,  177827 ,
  199526 ,  223872 ,  251188 ,  281838 ,  316227 ,
  354813 ,  398107 ,  446683 ,  501187 ,  562341 ,
  630957 ,  707945 ,  794328 ,  891250 , 1000000 ,
 1122018 , 1258925 , 1412537 , 1584893 , 1778279 ,
 1995262 , 2238721 , 2511886 , 2818382 , 3162277 ,
 3548133   /*  51 dbm */


};

#ifdef FEATURE_QAUDIOFX
/* ===========================================================================
**
**                       MB  C O N V E R S I O N   T A B L E
**
** =========================================================================*/

LOCAL const uint16 voc_data_mb_table1[] = {
	32767, 24403, 18174,
};

LOCAL const uint16 voc_data_mb_table2[] = {
	32767, 32170, 31583, 31006,
	30440, 29885, 29339, 28804,
	28278, 27762, 27255, 26758,
	26269, 25790, 25319, 24857,
};

LOCAL const uint16 voc_data_mb_table3[] = {
	32767, 32730, 32693, 32655,
	32617, 32580, 32542, 32505,
	32468, 32430, 32393, 32356,
	32318, 32281, 32244, 32207,
};
#endif /* FEATURE_QAUDIOFX */
/* <EJECT> */

/*===========================================================================

     C O N F I G U R A T I O N   A N D  C A L I B R A T I O N   D A T A

===========================================================================*/

/*
** Vocoder Frame Timing
*/

/* Control values for Vocoder Timing Command
*/
LOCAL const qdsp_cmd_voc_timing_type voc_data_is127_timing = {38,144,99};
LOCAL const qdsp_cmd_voc_timing_type voc_data_is733_timing = {38,144,99};

#ifdef MSMAUD_VOC_4GV
LOCAL const qdsp_cmd_voc_timing_type voc_data_4gv_timing   = {38,144,99};
#endif  /* MSMAUD_VOC_4GV */

#ifdef MSMAUD_VOC_4GV_WB
LOCAL const qdsp_cmd_voc_timing_type voc_data_4gv_wb_timing   = {38,144,99};
#endif  /* MSMAUD_VOC_4GV_WB */

LOCAL const qdsp_cmd_voc_timing_type voc_data_gsm_timing   = {35,135,67};

LOCAL const qdsp_cmd_voc_timing_type voc_data_wcdma_timing = {38,144,99};

#ifdef MSMAUD_VOC_AMR_WB_COMMON
LOCAL const qdsp_cmd_voc_timing_type voc_data_amr_wb_wcdma_timing = {38,144,87};
#endif  /* MSMAUD_VOC_AMR_WB_COMMON */

#ifdef FEATURE_PHONE_VR
LOCAL const qdsp_cmd_voc_timing_type voc_data_vrmodebc_timing  = {112,100,99};
#endif /* FEATURE_PHONE_VR */

LOCAL const qdsp_cmd_voc_timing_type voc_data_wlan_timing  = {35,135,67};

/* Override modes for EC, NS, and VOX
*/
voc_ec_ns_mode_type voc_data_ec_override_mode = VOC_EC_NS_NORMAL_OPERATION;
voc_ec_ns_mode_type voc_data_ns_override_mode = VOC_EC_NS_NORMAL_OPERATION;
voc_vox_mode_type   voc_data_vox_override_mode = VOC_VOX_NORMAL_OPERATION;
voc_sidetone_mode_type voc_data_sidetone_override_mode =
                      VOC_SIDETONE_NORMAL_OPERATION;

#ifdef FEATURE_AUDIO_AGC
/* Current AGC/AVC configuration
*/
LOCAL voc_agc_type voc_data_rx_agc_config = VOC_AGC_NONE;
LOCAL boolean      voc_data_tx_agc_config = FALSE;
#endif


LOCAL voc_smartsnd_type voc_data_smartsnd_rx_agc_config = VOC_SMARTSND_NONE;
LOCAL voc_smartsnd_type voc_data_smartsnd_rve_config    = VOC_SMARTSND_RVE_NONE;



/* Storage for current sample rate
*/
voc_sample_rate_type voc_data_sample_rate = VOC_SAMPLE_RATE_32000;

#ifdef FEATURE_MM_REC
/*
 * Storage for current MultiMedia recording session parameters.
 */
voc_mm_rec_parm_type voc_data_mm_rec_parm;

/*
 * Sample rate value mapped to sample rate enum types
 */
const uint16 voc_data_spl_rate_val[] =
{
  0,
  8000,
  11025,
  12000,
  16000,
  22050,
  24000,
  32000,
  44100,
  48000,
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  44000,
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
  0
};
#endif /* FEATURE_MM_REC */

#ifndef MSMAUD_SCMM
#ifdef MSMAUD_ADIE_CODEC
/* Power down sequence for adie codec.
*/
CONSTANT adie_codec_reg_type voc_data_adie_pwr_down_config[] =
  MSMAUD_ADIE_CODEC_PD_CONFIG_SEQ;

CONSTANT voc_adie_codec_config_type voc_data_adie_codec_pwr_down = {
  sizeof(voc_data_adie_pwr_down_config)/sizeof(adie_codec_reg_type),
  0,
  voc_data_adie_pwr_down_config
};

/* Power down sequence for adie codec with HSSD enabled.
*/
CONSTANT adie_codec_reg_type voc_data_adie_pwr_down_with_hssd_config[] =
#ifdef MSMAUD_USES_HSSD_IRQ
  MSMAUD_ADIE_CODEC_PD_WITH_HSSD_CONFIG_SEQ;
#else
  MSMAUD_ADIE_CODEC_PD_CONFIG_SEQ;
#endif /* MSMAUD_USES_HSSD_IRQ */

CONSTANT voc_adie_codec_config_type voc_data_adie_codec_pwr_down_with_hssd = {
  sizeof(voc_data_adie_pwr_down_with_hssd_config)/sizeof(adie_codec_reg_type),
  0,
  voc_data_adie_pwr_down_with_hssd_config
};
#endif /* MSMAUD_ADIE_CODEC */
#endif /* MSMAUD_SCMM */

#ifdef FEATURE_MIDI
qdsp_cmd_dls_config_type voc_data_dls_config = {0, 0, 0, 0, 0, 0, 0, 0};
#endif /* FEATURE_MIDI */

#ifdef FEATURE_AUDFMT_EQUALIZER
/* Macro used to check the boundary. Maximum number of filter band
** Maximum and Minimum of equalizer gain.
*/
#define VOC_DATA_EQUALIZER_MAX_BAND (VOC_EQ_BANDS_NUM)
#define VOC_DATA_EQUALIZER_MAX_GAIN (12)
#define VOC_DATA_EQUALIZER_MIN_GAIN (-12)
#define VOC_DATA_EQUALIZER_COEFF_SIZE (6+1+4)


/* This macro used to calculate the position of the coefficient
*/
#define EQ_NUM_OFFSET(x,y) ((x) * 6)
#define EQ_DEN_OFFSET(x,y) (((x) << 2) + ((y) * 6))
#define EQ_SHIFT_OFFSET(x,y) ((x) + ((y) * (6 + 4)))

/* These structure used to store the equalizer coefficient and
** filter table, will need to recompute coefficient if sample rate change
*/
LOCAL uint16 voc_data_equalizer_coeff
                [VOC_EQ_MAX_BANDS_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
/* This table contained the calculated coefficient value
*/
LOCAL uint16 voc_data_equalizer_coeff_filter_tbl[VOC_EQ_MAX_BANDS_NUM];
/* This is the buffered equalizer table
*/
LOCAL voc_equalizer_filter_type
                          voc_data_equalizer_tbl[VOC_EQ_MAX_BANDS_NUM];

/* This flag store the sample rate used to calculated the coefficient,
** if this sample rate change then we need to recalculate the coefficient
*/
LOCAL voc_sample_rate_type voc_data_equalizer_sample_rate =
                                                        VOC_SAMPLE_RATE_NONE;
#if (VOC_PRESET_EQ_MAX_NUM > 0)
LOCAL uint16 voc_data_preset_eq_coeff_8k
                [VOC_PRESET_EQ_MAX_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
LOCAL uint16 voc_data_preset_eq_coeff_11k
                [VOC_PRESET_EQ_MAX_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
LOCAL uint16 voc_data_preset_eq_coeff_12k
                [VOC_PRESET_EQ_MAX_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
LOCAL uint16 voc_data_preset_eq_coeff_16k
                [VOC_PRESET_EQ_MAX_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
LOCAL uint16 voc_data_preset_eq_coeff_22k
                [VOC_PRESET_EQ_MAX_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
LOCAL uint16 voc_data_preset_eq_coeff_24k
                [VOC_PRESET_EQ_MAX_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
LOCAL uint16 voc_data_preset_eq_coeff_32k
                [VOC_PRESET_EQ_MAX_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
LOCAL uint16 voc_data_preset_eq_coeff_44p1k
                [VOC_PRESET_EQ_MAX_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
LOCAL uint16 voc_data_preset_eq_coeff_48k
                [VOC_PRESET_EQ_MAX_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
LOCAL uint16 voc_data_preset_eq_coeff_44k
                [VOC_PRESET_EQ_MAX_NUM * VOC_DATA_EQUALIZER_COEFF_SIZE];
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */

#else

#define voc_data_preset_eq_coeff_8k  NULL
#define voc_data_preset_eq_coeff_11k NULL
#define voc_data_preset_eq_coeff_12k NULL
#define voc_data_preset_eq_coeff_16k NULL
#define voc_data_preset_eq_coeff_22k NULL
#define voc_data_preset_eq_coeff_24k NULL
#define voc_data_preset_eq_coeff_32k NULL
#define voc_data_preset_eq_coeff_44p1k NULL
#define voc_data_preset_eq_coeff_48k NULL
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
#define voc_data_preset_eq_coeff_44k NULL
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */

#endif

typedef struct {
  uint16  *coeff[VOC_SAMPLE_RATE_MAX];
  uint16  bands;
  boolean enable;
  boolean reset;
} voc_data_preset_eq_type;

voc_data_preset_eq_type voc_data_preset_eq_ctl = {
  {
    NULL,
    voc_data_preset_eq_coeff_8k,
    voc_data_preset_eq_coeff_11k,
    voc_data_preset_eq_coeff_12k,
    voc_data_preset_eq_coeff_16k,
    voc_data_preset_eq_coeff_22k,
    voc_data_preset_eq_coeff_24k,
    voc_data_preset_eq_coeff_32k,
    voc_data_preset_eq_coeff_44p1k,
    voc_data_preset_eq_coeff_48k
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
    ,voc_data_preset_eq_coeff_44k
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
  },
  0
};

/* Equalizer control information, enable, number of filter bands and
** coefficient pointer
*/
typedef struct {
  boolean enable;
  boolean reset;
  uint16  bands;
} voc_data_tbl_eq_ctl_type;

voc_data_tbl_eq_ctl_type voc_data_tbl_eq_ctl = {
  FALSE, /* Enable/ Disable */
  FALSE, /* Need reset      */
  0,     /* Number of bands */
};

/* The structure is used to return DSP
*/
qdsp_cmd_equalizer_type voc_data_equalizer_ctl = {
  FALSE,
  0,
  voc_data_equalizer_coeff
};

#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/* This value used to store the update period of the spectrum analyzer
*/
LOCAL uint16 voc_data_spectrum_analyzer_update_period = 0;
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
voc_pp_cb_type voc_data_sbc_func = NULL;
audiosbcenc_data_type voc_sbc_remote_params;
#endif /* FEATURE_SBC_CODEC */

#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
voc_pp_type_type voc_data_pp_type = VOC_PP_TYPE_GENERIC;
voc_pp_cb_type voc_data_pp_cb = NULL;
voc_pp_cb_type voc_data_pp_saved_func = NULL;
#endif /* FEATURE_ARM_POST_PROCESSING ||(FEATURE_AVS_WB_PCM_RX_INTERFACE) */


#ifdef FEATURE_SBC_DSP_CODEC

/* Mapping between DSP enums and audio driver enums */
uint16 voc_data_sbc_blocks_bits[VOC_SBC_BLOCKS_MAX] =
                                   {
                                     QDSP_CMD_SBC_PARAM_BLOCKS_4_V,
                                     QDSP_CMD_SBC_PARAM_BLOCKS_8_V,
                                     QDSP_CMD_SBC_PARAM_BLOCKS_12_V,
                                     QDSP_CMD_SBC_PARAM_BLOCKS_16_V
                                   };
uint16 voc_data_sbc_subbands_bits[VOC_SBC_SUBBANDS_MAX] =
                                   {
                                     QDSP_CMD_SBC_PARAM_SUBBANDS_8_V
                                   };
uint16 voc_data_sbc_mode_bits[VOC_SBC_MODE_MAX] =
                                   {
                                     QDSP_CMD_SBC_PARAM_MODE_INVALID_V,
                                     QDSP_CMD_SBC_PARAM_MODE_MONO_V,
                                     QDSP_CMD_SBC_PARAM_MODE_DUAL_V,
                                     QDSP_CMD_SBC_PARAM_MODE_STEREO_V,
                                     QDSP_CMD_SBC_PARAM_MODE_JT_STEREO_V
                                   };

uint16 voc_data_sbc_alloc_bits[VOC_SBC_ALLOC_MAX] =
                                   {
                                     QDSP_CMD_SBC_PARAM_BIT_ALLOC_SNR_V,
                                     QDSP_CMD_SBC_PARAM_BIT_ALLOC_LOUD_V
                                   };

/* Default values used for configuring the DSP SBC encoder */
voc_sbc_blocks_type   voc_data_sbc_blocks    = VOC_SBC_BLOCKS_8;
voc_sbc_subbands_type voc_data_sbc_subbands  = VOC_SBC_SUBBANDS_8;
voc_sbc_alloc_type    voc_data_sbc_alloc     = VOC_SBC_ALLOC_SNR;
voc_sbc_mode_type     voc_data_sbc_mode      = VOC_SBC_MODE_JT_STEREO;
uint32                voc_data_sbc_bitrate   = 384000;
#endif /* FEATURE_SBC_DSP_CODEC */

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
voc_bt_cb_func_ptr_type voc_data_bt_cb_func_ptr = NULL;
#endif /*AVS_BT_SCO_REWORK*/


/*callback pointer which holds the Qtv registrd callback for moddisable event*/
voc_notify_qtv_cb_func_ptr_type voc_data_qtv_notify_cb_func_ptr = NULL;


#if defined(FEATURE_QTUNES_COMMON) || defined(FEATURE_AUDFMT_EQUALIZER) || \
    defined(FEATURE_AUDFMT_SPECTRUM_ANALYZER) || defined(FEATURE_AUDIO_FORMAT)
/* The sample rate table converts a value of voc_sample_rate_type to a k number.
** These values are being rounded to the nearest k, which will cause a maximum
** of less than .02% error in the worst case in the duration calculation
** for DTMFs for the QTunes image.
*/
const uint16 voc_data_sr_table[] = {
  0,
  8,     /* 8k      */
  11,    /* 11.025k */
  12,    /* 12k     */
  16,    /* 16k     */
  22,    /* 22.050k */
  24,    /* 24k     */
  32,    /* 32k     */
  44,    /* 44.1k   */
  48,    /* 48k     */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  44,    /* 44k     */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
  0      /* Unknown */
};
#endif /* FEATURE_QTUNES_COMMON || FEATURE_AUDFMT_EQUALIZER ||
          FEATURE_AUDFMT_SPECTRUM_ANALYZER || FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

#ifdef FEATURE_AUDFMT_QCONCERT
/* These defined the supported range of value on QConcert setting
*/
#define VOC_DATA_QCONCERT_MAX_GAIN_V          (0x7FFF)
#define VOC_DATA_QCONCERT_MAX_SPREAD_V        (0x7FFF)
#define VOC_DATA_QCONCERT_MIN_GEOMETRY_V      (-32)
#define VOC_DATA_QCONCERT_MAX_GEOMETRY_V      (32)

#ifdef FEATURE_AUDFMT_QCONCERT_PLUS
#define VOC_DATA_QCONCERT_PLUS_MIN_GEOMETRY_V      (-2)
#define VOC_DATA_QCONCERT_PLUS_MAX_GEOMETRY_V      (-1)
#endif
#define VOC_DATA_QCONCERT_HP_MODE 0
#define VOC_DATA_QCONCERT_SPKFF_MODE 1
#define VOC_DATA_QCONCERT_SPKSF_MODE 2
#define VOC_DATA_QCONCERT_SPKDESK_MODE 3

boolean voc_data_qconcert_enable = FALSE;

const uint16 voc_data_qconcert_mode_table[] = {
  0xFFFF,
  0,
  1,
  2
};

/* This defiend the convertion for sample rate mode.
*/
const uint16 voc_data_qconcert_sr_table[] = {
  0x0000,
  0x0000,    /* 8k      */
  0x0200,    /* 11.025k */
  0x0400,    /* 12k     */
  0x0601,    /* 16k     */
  0x0802,    /* 22.050k */
  0x0a02,    /* 24k     */
  0x0c2b,    /* 32k     */
  0x0ed4,    /* 44.1k   */
  0x117d,    /* 48k     */
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  0x0ed4,    /* 44k     */
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
  0x0000     /* Unknown */
};

typedef int16 voc_data_qconcert_geometry_type;

const voc_data_qconcert_geometry_type
voc_data_qconcert_hp_delay_table[VOC_SAMPLE_RATE_MAX] = {
/* Delay is the same as geometry */
   0,
  -5,
  -7,
  -7,
  -10,
  -14,
  -15,
  -20,
  -28,
  -30
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,-28
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

const voc_data_qconcert_geometry_type
voc_data_qconcert_spkdsk_delay_table[VOC_SAMPLE_RATE_MAX] = {
/* Delay is the same as geometry */
  0,
  0,
  0,
  0,
  1,
  1,
  1,
  4,
  6,
  7
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,6
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};
/* Qconcert and Qconcert plus usees the same Speaker Geometry table for now  */
/* because thier Geometry values happen to be the same.                      */
/* Qconcert plus speaker modes are not sample rate depended.                 */
/* It only takes (-1) narrow , (-2) wide seperation                          */
/* Qconcert can takes value from (-32) to (32)                               */
/* Currently Incall image doesn't support Qconcert plus and if qconcert      */
/* geometry  values for speaker ff/sf happen to be differnent than(-1), then */
/* extra  table should be Created for Qconcert plus                          */
const voc_data_qconcert_geometry_type
voc_data_qconcert_sp_default_delay_table[VOC_SAMPLE_RATE_MAX] = {
/* Delay is the same as geometry */
  0,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  ,-1
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

typedef struct {
  uint16  gain;               /* Gain value                               */
  uint16  spread;             /* Spread value                             */
const voc_data_qconcert_geometry_type *geometry_ptr; /* Geometry value */
} voc_data_qconcert_parameter_type;

/* This defined the default qconcert value
*/

voc_data_qconcert_parameter_type voc_data_headphone_qconcert_config = {
  0x4027, /* gain */
  0x7FFF,   /* spread           */
  voc_data_qconcert_hp_delay_table /* Geometry */
};

voc_data_qconcert_parameter_type voc_data_speaker_qconcert_config = {
  0x4027, /* gain */
  0x7FFF,   /* spread           */
  voc_data_qconcert_sp_default_delay_table, /* Geometry */
};

voc_data_qconcert_parameter_type voc_data_spksf_qconcert_config = {
  0x4027, /* gain */
  0x7FFF,   /* spread           */
  voc_data_qconcert_sp_default_delay_table, /* Geometry */
};

voc_data_qconcert_parameter_type voc_data_spkdsk_qconcert_config = {
  0x4027, /* gain */
  0x7FFF, /* spread */
  voc_data_qconcert_spkdsk_delay_table /* Geometry */
};

voc_data_qconcert_parameter_type *voc_data_qconcert_configs[] = {
  &voc_data_headphone_qconcert_config, /* Headphone            */
  &voc_data_speaker_qconcert_config,   /* Speaker Front Firing */
  &voc_data_spksf_qconcert_config,     /* Speaker Side Firing  */
  &voc_data_spkdsk_qconcert_config     /* Speaker Desktop      */
};
#endif /* FEATURE_AUDFMT_QCONCERT */

#ifdef FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL
typedef enum {
  VOC_DATA_PP_INTERFACE_FIR,      /* FIR filter */
  VOC_DATA_PP_INTERFACE_IIR,      /* IIR filter */
  VOC_DATA_PP_INTERFACE_ADRC,     /* ADRC */
  VOC_DATA_PP_INTERFACE_EQ,       /* Equalizer */
  VOC_DATA_PP_INTERFACE_SPA,      /* Spectrum Analyzer */
  VOC_DATA_PP_INTERFACE_QCONCERT, /* Qconcert */
  VOC_DATA_PP_INTERFACE_STF,      /* Side chain tuning filter */
  VOC_DATA_PP_INTERFACE_SBC,      /* SBC Encoder */
  VOC_DATA_PP_INTERFACE_MAX
} voc_data_pp_interface_enum_type;

typedef struct {
  qdsp_module_type module;
  uint16           mips;   /* MIPS * 10 */
} voc_data_module_to_mips_map_type;

#if defined(FEATURE_QTUNES)
#ifdef QDSP_IMAGE_TUNES_DEFINED
/* Post-proc MIPs for qtunes image */
const byte voc_data_qtunes_mp3_mips_info[] = MSMAUD_QTUNES_MP3_MIPS_USAGE;

/* Available modules for tunes images and their supported mips */
const voc_data_module_to_mips_map_type voc_data_qtunes_mp3_mtm_map[] =
  MSMAUD_QTUNES_MP3_MODULE_TO_MIPS_MAP;
#endif /* QDSP_IMAGE_TUNES_DEFINED */

#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
/* Post-proc MIPs for acc image */
const byte voc_data_qtunes_mp3_mips_info[] = MSMAUD_QTUNES_MP3_MIPS_USAGE;

/* Available modules for acc images and their supported mips */
const voc_data_module_to_mips_map_type voc_data_qtunes_mp3_mtm_map[] =
  MSMAUD_QTUNES_MP3_MODULE_TO_MIPS_MAP;
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
#endif /* FEATURE_QTUNES */

#if defined(FEATURE_QTUNES_AAC)
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
/* Post-proc MIPs for ENH_AAC_PLUS image */
const byte voc_data_enh_aac_plus_mips_info[] = MSMAUD_ENH_AAC_PLUS_MIPS_USAGE;

/* Available modules for ENH_AAC_PLUS images and their supported mips */
const voc_data_module_to_mips_map_type voc_data_enh_aac_plus_mtm_map[] =
   MSMAUD_AAC_PLUS_ENH_MODULE_TO_MIPS_MAP;
#endif /* QDSP_IMAGE_ENH_AAC_PLUS_DEFINED */
#endif /* FEATURE_QTUNES_AAC */

#endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL */


/* VP3 Data definitions
*/

/* Table that maps ECNS mode to FILEID/BUF_ID
*/
vocvp3_buf_id_type voc_data_ecns_mode_to_buf_id[VOC_EC_DEFAULT] =
{
  QARE_FILEID_INVALID,
  QARE_FILEID_ECNS_HANDSET,
  QARE_FILEID_ECNS_HEADSET,
  QARE_FILEID_ECNS_HANDSFREE,
  QARE_FILEID_ECNS_SPEAKER,
  QARE_FILEID_ECNS_BT
};

/* Table that maps Fluence mode to FILEID/BUF_ID
*/
vocvp3_buf_id_type voc_data_fluence_mode_to_buf_id[VOC_EC_DEFAULT] =
{
  QARE_FILEID_INVALID,
  QARE_FILEID_FLUENCE_HANDSET,
  QARE_FILEID_INVALID,
  QARE_FILEID_INVALID,
  QARE_FILEID_INVALID,
  QARE_FILEID_INVALID
};

/* VP3 Data Size definitions
*/
#define VOC_DATA_VP3_ECNS_DATA_SIZE                 864

#define VOC_DATA_VP3_FLUENCE_DATA_SIZE              564

/* Default set of VP3 parameters for ECNS
*/
static uint16 voc_data_vp3_ecns_data_default[VOC_DATA_VP3_ECNS_DATA_SIZE] =
{
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000
};


/* Default set of VP3 parameters for Fluence
*/
static uint16  voc_data_vp3_fluence_data_default[VOC_DATA_VP3_FLUENCE_DATA_SIZE] =
{
  0x0000, 0x2000,
  0x2000, 0x2000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


/* table that maps FILE_ID/BUF_ID to Sizes
*/
unsigned int voc_data_vp3_buf_id_to_buf_size_table[VOCVP3_BUF_IDMAX] =
{
  sizeof(voc_data_vp3_ecns_data_default),
  sizeof(voc_data_vp3_ecns_data_default),
  sizeof(voc_data_vp3_ecns_data_default),
  sizeof(voc_data_vp3_ecns_data_default),
  sizeof(voc_data_vp3_ecns_data_default),
  sizeof(voc_data_vp3_fluence_data_default),
  0
};


/* table that maps FILE_ID/BUF_ID to default data table
*/
uint16* voc_data_vp3_buf_id_to_default_data_table[VOCVP3_BUF_IDMAX] =
{
(uint16*)&voc_data_vp3_ecns_data_default,
(uint16*)&voc_data_vp3_ecns_data_default,
(uint16*)&voc_data_vp3_ecns_data_default,
(uint16*)&voc_data_vp3_ecns_data_default,
(uint16*)&voc_data_vp3_ecns_data_default,
(uint16*)&voc_data_vp3_fluence_data_default,
NULL
};


static boolean voc_data_slowtalk_enable_flag = TRUE;
static voc_slowtalk_static_params_status_type
voc_data_slowtalk_static_params_status = VOC_SLOWTALK_STATIC_PARAMS_NOT_UPDATED;

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_codec

DESCRIPTION
  Set the current codec PCM path.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DATA_SUCCESS - The codec PCM path was set successfully.
  VOC_DATA_FAILED  - The attempt to set the codec PCM path failed.

SIDE EFFECTS
  voc_data_codec_inuse
  voc_data_pcm_cal_ptr

===========================================================================*/
voc_data_return_type voc_data_set_codec
(
  voc_codec_type            pcm_path
)
{
  /* Remember the type of codec
  */
  voc_data_codec_inuse = pcm_path;
  voc_data_codec_class_inuse = voc_cal_get_codec_class(pcm_path);
#ifdef FEATURE_PHONE_VR
  voc_data_codec_vr_type_inuse = voc_cal_get_codec_vr_type(pcm_path);
#endif /* FEATURE_PHONE_VR */

  return( VOC_DATA_SUCCESS );

} /* voc_data_set_codec */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_codec_cal

DESCRIPTION
  Set the calibration for the codec PCM path.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DATA_SUCCESS - The codec PCM path was set successfully.
  VOC_DATA_FAILED  - The attempt to set the codec PCM path failed.

SIDE EFFECTS
  voc_data_codec_inuse
  voc_data_pcm_cal_ptr

===========================================================================*/
voc_data_return_type voc_data_set_codec_cal
(
  voc_codec_type               pcm_path,
  qdsp_image_type              image
)
{
  voc_cal_return_type result = VOC_CAL_FAILURE;

  /* Set the default configuration for this type of codec
  */
  result = voc_cal_get_pcm_cal(
    pcm_path,
    image,
    &voc_data_pcm_cal_ptr
    );

#ifdef FEATURE_VOC_MIC_GAIN_ADJUST
  /* Set the mic gain adjust for this codec
  */
  if ( result == VOC_CAL_SUCCESS ) {
    result = voc_cal_get_mic_gain_adjust(
      pcm_path,
      &voc_data_mic_gain_adjust_ptr
      );
  }
#endif /* FEATURE_VOC_MIC_GAIN_ADJUST */

  if ( result == VOC_CAL_SUCCESS ) {
    return( VOC_DATA_SUCCESS );
  } else {
    return( VOC_DATA_FAILURE );
  }

} /* voc_data_set_codec_cal */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_register_codec_func

DESCRIPTION
  Set the specified codec enable function.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DATA_SUCCESS - The codec function was registered successfully.
  VOC_DATA_FAILED  - The attempt to register the codec function failed.

SIDE EFFECTS
  voc_data_codec_enable_func

===========================================================================*/
voc_data_return_type voc_data_register_codec_func (
  voc_codec_type                pcm_path,
  voc_amp_enable_func_ptr_type  func_ptr
) {
  if(pcm_path < VOC_CODEC_MAX) {
    voc_data_codec_enable_func[pcm_path] = func_ptr;
    return( VOC_DATA_SUCCESS );
  } else {
    return( VOC_DATA_FAILURE );
  }
} /* voc_data_register_codec_func */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_codec_func

DESCRIPTION
  Get the specified codec enable function.

DEPENDENCIES
  voc_data_codec_enable_func

RETURN VALUE
  VOC_DATA_SUCCESS - The codec function was retrieved successfully.
  VOC_DATA_FAILED  - The attempt to retrieve the codec function failed.

SIDE EFFECTS
  None.

===========================================================================*/
voc_data_return_type voc_data_get_codec_func (
  voc_codec_type                pcm_path,
  voc_amp_enable_func_ptr_type  *func_ptr
) {
  if(pcm_path < VOC_CODEC_MAX) {
    *func_ptr = voc_data_codec_enable_func[pcm_path];
    return( VOC_DATA_SUCCESS );
  } else {
    return( VOC_DATA_FAILURE );
  }
} /* voc_data_register_codec_func */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_codec_mode

DESCRIPTION
  Get the current codec mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DATA_SUCCESS - The codec function was retrieved successfully.
  VOC_DATA_FAILED  - The attempt to retrieve the codec function failed.

SIDE EFFECTS
  None.

===========================================================================*/
voc_data_return_type voc_data_get_codec_mode (
  qdsp_image_type               image,
  voc_codec_mode_type           *codec_mode
) {
  voc_data_return_type result;
  voc_capability_type current_config = VOC_CAP_NONE;
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  voc_capability_type    desired_config = VOC_CAP_NONE;
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

  switch(image) {
#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON)
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
#ifdef QDSP_IMAGE_TUNES_DEFINED
    case QDSP_IMAGE_TUNES:
#endif /* QDSP_IMAGE_TUNES_DEFINED */
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
    case QDSP_IMAGE_CAM_AUDIO_CONC:
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
#ifdef QDSP_IMAGE_MP4D_MP3_DEFINED
    case QDSP_IMAGE_MP4D_MP3:
#endif /* QDSP_IMAGE_MP4D_MP3_DEFINED */
#ifdef QDSP_IMAGE_AAC_DEFINED
    case QDSP_IMAGE_AAC:
#endif /* QDSP_IMAGE_AAC_DEFINED */
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
    case QDSP_IMAGE_ENH_AAC_PLUS:
#endif /* QDSP_IMAGE_ENH_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED
    case QDSP_IMAGE_MP4D_AAC_PLUS:
#endif /* QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED
    case QDSP_IMAGE_MP4D_AAC_NO_VPP:
#endif /* QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_DEFINED
    case QDSP_IMAGE_MP4D_AAC:
#endif /* QDSP_IMAGE_MP4D_AAC_DEFINED */
#ifdef QDSP_IMAGE_H263D_MP3_DEFINED
    case QDSP_IMAGE_H263D_MP3:
#endif /* QDSP_IMAGE_H263D_MP3_DEFINED */
#ifdef QDSP_IMAGE_H263D_AAC_DEFINED
    case QDSP_IMAGE_H263D_AAC:
#endif /* QDSP_IMAGE_H263D_AAC_DEFINED */
#ifdef QDSP_IMAGE_H264D_AAC_DEFINED
    case QDSP_IMAGE_H264D_AAC:
#endif /* QDSP_IMAGE_H264D_AAC_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
    case QDSP_IMAGE_QTV_AAC:
#endif /* QDSP_IMAGE_QTV_AAC_DEFINED */
#ifdef QDSP_IMAGE_RA_DEFINED
    case QDSP_IMAGE_RA:
#endif
#ifdef QDSP_IMAGE_WM_DEFINED
    case QDSP_IMAGE_WM:
#endif
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_WM_PRO_DEFINED */
#ifdef QDSP_IMAGE_QTV_CONC_DEFINED
    case QDSP_IMAGE_QTV_CONC:
#endif /* QDSP_IMAGE_QTV_CONC_DEFINED */
#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
    case QDSP_IMAGE_VFE_YCBCR:
#endif /* QDSP_IMAGE_VFE_YCBCR_DEFINED */
#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
    case QDSP_IMAGE_VFE_BAYER:
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED */
      *codec_mode = VOC_CODEC_MODE_MULTIMEDIA;
      result = VOC_CAL_SUCCESS;
      break;

       /* A2DP support for for NB-AMR */
#if defined(QDSP_IMAGE_QTV_DEFINED) || defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED)|| \
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED)
#ifdef QDSP_IMAGE_QTV_DEFINED
  case QDSP_IMAGE_QTV:
#endif
#ifdef QDSP_IMAGE_QTV_VOC_H264_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED
#error code not present
#endif

#ifdef FEATURE_BT_AG
      if (voc_data_codec_inuse == VOC_CODEC_BT_AG) {
        /* In case of VOCODER on SCO selecting different codec mode */
        current_config = ((voc_state_get_state_data())->config_pending);
        switch(current_config)
        {
          case VOC_CAP_IS733:
          case VOC_CAP_IS127:
          case VOC_CAP_4GV_NB:
           *codec_mode = VOC_CODEC_MODE_CDMA_VOICE;
           break;
          case VOC_CAP_4GV_WB:
          case VOC_CAP_4GV_WB_VOIP:
          *codec_mode = VOC_CODEC_MODE_CDMA_WB_VOICE;
          break;
          case VOC_CAP_AMR_WB:
          #if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
#error code not present
          #endif  /* FEATURE_GSM && FEATURE_WCDMA */
          case VOC_CAP_AMR:
          case VOC_CAP_GSM_EFR:
          case VOC_CAP_GSM_FR:
          case VOC_CAP_GSM_HR:

            #if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
            *codec_mode = VOC_CODEC_MODE_GSM_VOICE;
            #else
            *codec_mode = VOC_CODEC_MODE_CDMA_VOICE;
            #endif  /* FEATURE_GSM && FEATURE_WCDMA */
            break;

          default:
            MSG_HIGH("QTV IMAGE with capability = %d requested on BT AG device",
                  current_config, 0, 0);
            /* for other media types combinations selecting multimedia mode */
            *codec_mode = VOC_CODEC_MODE_MULTIMEDIA;
            break;
        }

        result = VOC_CAL_SUCCESS;
        break;
      }
#endif /* FEATURE_BT_AG*/

      /* for other media types combinations selecting multimedia mode */
      *codec_mode = VOC_CODEC_MODE_MULTIMEDIA;
      result = VOC_CAL_SUCCESS;
      break;

#endif /* QDSP_IMAGE_QTV_DEFINED ||QDSP_IMAGE_QTV_VOC_H264_DEFINED ||\
        QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED */

#endif /* FEATURE_QSYNTH_COMMON || FEATURE_QTUNES_COMMON */

#if defined(QDSP_IMAGE_QCAMCORDER_DEFINED)       || \
    defined(QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED) || \
    defined(QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_AACENC_DEFINED)
#ifdef QDSP_IMAGE_QCAMCORDER_DEFINED
    case QDSP_IMAGE_QCAMCORDER:
#endif /* QDSP_IMAGE_QCAMCORDER_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED
#error code not present
#endif /*QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED
    case QDSP_IMAGE_QCAMCORDER_AACENC:
#endif /* QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED */
#ifdef QDSP_IMAGE_AACENC_DEFINED
    case QDSP_IMAGE_AACENC:
#endif /* QDSP_IMAGE_AACENC_DEFINED */

      *codec_mode = VOC_CODEC_MODE_MM_REC;
      result = VOC_CAL_SUCCESS;
      break;
#endif /* QDSP_IMAGE_QCAMCORDER_DEFINED ||
          QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED
          QDSP_IMAGE_AACENC_DEFINED */
#ifdef FEATURE_AVS_I2SSBC
    case QDSP_IMAGE_I2SSBC:
      *codec_mode = VOC_CODEC_MODE_I2SSBC;
      result = VOC_CAL_SUCCESS;
      break;
#endif /* FEATURE_AVS_I2SSBC */

#if defined (QDSP_IMAGE_LOADER_DEFINED)
    case QDSP_IMAGE_LOADER:
      result = VOC_CAL_FAILURE;
      break;
#endif

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)

#if defined(QDSP_IMAGE_VOC_COMBO_ICA_DEFINED) || \
    defined(QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED)

#ifdef QDSP_IMAGE_VOC_COMBO_ICA_DEFINED
    case QDSP_IMAGE_VOC_COMBO_ICA:
#endif /* QDSP_IMAGE_VOC_COMBO_ICA_DEFINED */
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
    case QDSP_IMAGE_VOC_COMBO_SYNTH:
#endif /* QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */
      desired_config = voc_get_desired_config(VOC_ACQ_CDMA);

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */
      if ((desired_config & VOC_CAP_IS733) ||
          (desired_config & VOC_CAP_IS127)) {

        *codec_mode = VOC_CODEC_MODE_CDMA_VOC_ADEC_MIXER;
        result = VOC_CAL_SUCCESS;
      } else {
        result = VOC_CAL_FAILURE;
      }
      break;

#endif /* QDSP_IMAGE_VOC_COMBO_ICA_DEFINED ||
          QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB ||
          FEATURE_AVS_INCALL_MIXER_CMX */

    default:
      current_config = voc_get_curr_config();
      switch (current_config) {
        case VOC_CAP_IS733:
        case VOC_CAP_IS127:
        case VOC_CAP_4GV_NB:
          *codec_mode = VOC_CODEC_MODE_CDMA_VOICE;
          break;
        case VOC_CAP_4GV_WB:
        case VOC_CAP_4GV_WB_VOIP:
          *codec_mode = VOC_CODEC_MODE_CDMA_WB_VOICE;
          break;
        case VOC_CAP_AMR_WB:
          #if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
#error code not present
          #endif  /* FEATURE_GSM && FEATURE_WCDMA */
        case VOC_CAP_AMR:
        case VOC_CAP_GSM_EFR:
        case VOC_CAP_GSM_FR:
        case VOC_CAP_GSM_HR:
        default:
          #if defined(FEATURE_GSM) && defined(FEATURE_WCDMA)
          *codec_mode = VOC_CODEC_MODE_GSM_VOICE;
          #else
          *codec_mode = VOC_CODEC_MODE_CDMA_VOICE;
          #endif  /* FEATURE_GSM && FEATURE_WCDMA */
          break;
       }
      result = VOC_CAL_SUCCESS;
      break;
  }

  return( result );
} /* voc_data_get_codec_mode */

#ifdef FEATURE_BT_AG
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_image_supports_bt_ag

DESCRIPTION
  Checks if the specified firmware image supports the Bluetooth codec.

DEPENDENCIES
  None

RETURN VALUE
  TRUE is supported; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_data_image_supports_bt_ag
(
  qdsp_image_type image
)
{
  boolean supported = FALSE;

  switch (image) {
#if defined(QDSP_IMAGE_VOC_DEFINED)               || \
    defined(QDSP_IMAGE_VOC_COMBO_DEFINED)         || \
    defined(QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED)    || \
    defined(QDSP_IMAGE_VOC_EVRC_DEFINED)          || \
    defined(QDSP_IMAGE_VOC_E90_DEFINED)           || \
    defined(QDSP_IMAGE_VOC_EVRC_E90_DEFINED)      || \
    defined(QDSP_IMAGE_VOC_QTV_DEFINED)           || \
    defined(QDSP_IMAGE_4GV_DEFINED)               || \
    defined(QDSP_IMAGE_4GV_WB_DEFINED)            || \
    defined(QDSP_IMAGE_G711_DEFINED)              || \
    defined(QDSP_IMAGE_AMR_WB_DEFINED)            || \
    defined(QDSP_IMAGE_AMR_DEFINED)               || \
    defined(QDSP_IMAGE_EFR_DEFINED)               || \
    defined(QDSP_IMAGE_FR_DEFINED)                || \
    defined(QDSP_IMAGE_HR_DEFINED)                || \
    defined(QDSP_IMAGE_VRBC_DEFINED)              || \
    defined(QDSP_IMAGE_PUFE_DEFINED)              || \
    defined(QDSP_IMAGE_SYNTH_DEFINED)             || \
    defined(QDSP_IMAGE_AMR_YCBCR2_DEFINED)        || \
    defined(QDSP_IMAGE_EFR_YCBCR2_DEFINED)        || \
    defined(QDSP_IMAGE_FR_YCBCR2_DEFINED)         || \
    defined(QDSP_IMAGE_HR_YCBCR2_DEFINED)         || \
    defined(QDSP_IMAGE_QVP_YUV_AMR_H263_DEFINED)  || \
    defined(QDSP_IMAGE_QVP_YUV_AMR_MPEG4_DEFINED) || \
    defined(QDSP_IMAGE_QVP_BAYER_AMR_DEFINED)     || \
    defined(QDSP_IMAGE_QVP_MPEG4_AMR_DEFINED)     || \
    defined(QDSP_IMAGE_QVP_MPEG4_EVRC_DEFINED)    || \
    defined(QDSP_IMAGE_QVP_H263P3_EVRC_DEFINED)   || \
    defined (QDSP_IMAGE_QVP_MPEG4_4GVWB_DEFINED)  || \
    defined (QDSP_IMAGE_QVP_H263P3_4GVWB_DEFINED) || \
    defined(QDSP_IMAGE_QVP_H263P3_AMR_DEFINED)    || \
    defined(QDSP_IMAGE_VOC_MP3_DEFINED)           || \
    defined(QDSP_IMAGE_VOC_EVRC_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_AMR_MP3_DEFINED)           || \
    defined(QDSP_IMAGE_EFR_MP3_DEFINED)           || \
    defined(QDSP_IMAGE_FR_MP3_DEFINED)            || \
    defined(QDSP_IMAGE_HR_MP3_DEFINED)            || \
    defined(QDSP_IMAGE_VOC_COMBO_ICA_DEFINED)     || \
    defined(QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED)   || \
    defined(QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED)      || \
    (defined(FEATURE_AVS_AUDIO_DECODER_SCO)    && \
     (defined(QDSP_IMAGE_TUNES_DEFINED)        || \
      defined(QDSP_IMAGE_QTV_AAC_DEFINED)      || \
      defined(QDSP_IMAGE_QTV_AAC_H264_DEFINED) || \
      defined(QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED)|| \
      defined(QDSP_IMAGE_ENH_AAC_PLUS_DEFINED) || \
      defined(QDSP_IMAGE_RA_DEFINED)           || \
      defined(QDSP_IMAGE_QTV_CONC_DEFINED)     || \
      defined(QDSP_IMAGE_WM_PRO_DEFINED)       || \
      defined(QDSP_IMAGE_WM_DEFINED)))

#ifdef QDSP_IMAGE_VOC_DEFINED
    case QDSP_IMAGE_VOC:
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_DEFINED
    case QDSP_IMAGE_VOC_COMBO:
#endif
#ifdef FEATURE_AMR_FIXED
    case QDSP_IMAGE_AMR:
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED
    case QDSP_IMAGE_VOC_COMBO_VOIP:
#endif
#ifdef QDSP_IMAGE_VOC_EVRC_DEFINED
    case QDSP_IMAGE_VOC_EVRC:
#endif
#ifdef QDSP_IMAGE_VOC_E90_DEFINED
    case QDSP_IMAGE_VOC_E90:
#endif
#ifdef QDSP_IMAGE_VOC_EVRC_E90_DEFINED
    case QDSP_IMAGE_VOC_EVRC_E90:
#endif
#ifdef QDSP_IMAGE_VOC_QTV_DEFINED
    case QDSP_IMAGE_VOC_QTV:
#endif
#ifdef QDSP_IMAGE_4GV_DEFINED
    case QDSP_IMAGE_4GV:
#endif
#ifdef QDSP_IMAGE_4GV_WB_DEFINED
    case QDSP_IMAGE_4GV_WB:
#endif
#ifdef QDSP_IMAGE_4GV_VOIP_DEFINED
    case QDSP_IMAGE_4GV_VOIP:
#endif
#ifdef QDSP_IMAGE_G711_DEFINED
    case QDSP_IMAGE_G711:
#endif
#ifdef QDSP_IMAGE_AMR_WB_DEFINED
    case QDSP_IMAGE_AMR_WB:
#endif
#ifdef QDSP_IMAGE_AMR_DEFINED
    case QDSP_IMAGE_AMR:
#endif
#ifdef QDSP_IMAGE_EFR_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_FR_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_HR_DEFINED
    case QDSP_IMAGE_HR:
#endif
#ifdef QDSP_IMAGE_VRBC_DEFINED
    case QDSP_IMAGE_VRBC:
#endif
#ifdef QDSP_IMAGE_PUFE_DEFINED
    case QDSP_IMAGE_PUFE:
#endif
#ifdef QDSP_IMAGE_SYNTH_DEFINED
    case QDSP_IMAGE_SYNTH:
#endif
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_AMR_YCBCR2_DEFINED
    case QDSP_IMAGE_AMR_YCBCR2:
#endif
#ifdef QDSP_IMAGE_EFR_YCBCR2_DEFINED
    case QDSP_IMAGE_EFR_YCBCR2:
#endif
#ifdef QDSP_IMAGE_FR_YCBCR2_DEFINED
    case QDSP_IMAGE_FR_YCBCR2:
#endif
#ifdef QDSP_IMAGE_HR_YCBCR2_DEFINED
    case QDSP_IMAGE_HR_YCBCR2:
#endif
#ifdef QDSP_IMAGE_QVP_YUV_AMR_H263_DEFINED
    case QDSP_IMAGE_QVP_YUV_AMR_H263:
#endif
#ifdef QDSP_IMAGE_QVP_YUV_AMR_MPEG4_DEFINED
    case QDSP_IMAGE_QVP_YUV_AMR_MPEG4:
#endif
#ifdef QDSP_IMAGE_QVP_BAYER_AMR_DEFINED
    case QDSP_IMAGE_QVP_BAYER_AMR;
#endif
#ifdef QDSP_IMAGE_QVP_MPEG4_AMR_DEFINED
    case QDSP_IMAGE_QVP_MPEG4_AMR:
#endif
#ifdef QDSP_IMAGE_QVP_MPEG4_EVRC_DEFINED
    case QDSP_IMAGE_QVP_MPEG4_EVRC:
#endif
#ifdef QDSP_IMAGE_QVP_H263P3_EVRC_DEFINED
    case QDSP_IMAGE_QVP_H263P3_EVRC:
#endif
#ifdef QDSP_IMAGE_QVP_MPEG4_4GVWB_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QVP_H263P3_4GVWB_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QVP_H263P3_AMR_DEFINED
    case QDSP_IMAGE_QVP_H263P3_AMR:
#endif
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
#ifdef QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED
    case QDSP_IMAGE_QCAMCORDER_COMBO:
#endif
#ifdef FEATURE_AVS_AUDIO_DECODER_SCO
  #ifdef QDSP_IMAGE_TUNES_DEFINED
    case QDSP_IMAGE_TUNES:
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
  #ifdef QDSP_IMAGE_QTV_DEFINED
    case QDSP_IMAGE_QTV:
  #endif
  #ifdef QDSP_IMAGE_QTV_VOC_H264_DEFINED
#error code not present
  #endif
  #ifdef QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED
#error code not present
  #endif
  #ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
    case QDSP_IMAGE_ENH_AAC_PLUS:
  #endif
  #ifdef QDSP_IMAGE_RA_DEFINED
    case QDSP_IMAGE_RA:
  #endif
  #ifdef QDSP_IMAGE_QTV_CONC_DEFINED
    case QDSP_IMAGE_QTV_CONC:
  #endif
  #ifdef QDSP_IMAGE_WM_DEFINED
    case QDSP_IMAGE_WM:
  #endif
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_WM_PRO_DEFINED */

#endif /* FEATURE_AVS_AUDIO_DECODER_SCO */

#ifdef QDSP_IMAGE_VOC_COMBO_ICA_DEFINED
    case QDSP_IMAGE_VOC_COMBO_ICA:
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
    case QDSP_IMAGE_VOC_COMBO_SYNTH:
#endif
       supported = TRUE;
       break;
#endif /* QDSP_IMAGE_VOC_DEFINED               ||
          QDSP_IMAGE_VOC_EVRC_DEFINED          ||
          QDSP_IMAGE_VOC_E90_DEFINED           ||
          QDSP_IMAGE_VOC_EVRC_E90_DEFINED      ||
          QDSP_IMAGE_VOC_QTV_DEFINED           ||
          QDSP_IMAGE_4GV_DEFINED               ||
          QDSP_IMAGE_G711_DEFINED              ||
          QDSP_IMAGE_AMR_DEFINED               ||
          QDSP_IMAGE_AMR_WB_DEFINED            ||
          QDSP_IMAGE_EFR_DEFINED               ||
          QDSP_IMAGE_FR_DEFINED                ||
          QDSP_IMAGE_HR_DEFINED                ||
          QDSP_IMAGE_VRBC_DEFINED              ||
          QDSP_IMAGE_PUFE_DEFINED              ||
          QDSP_IMAGE_SYNTH_DEFINED             ||
          QDSP_IMAGE_AMR_YCBCR2_DEFINED        ||
          QDSP_IMAGE_EFR_YCBCR2_DEFINED        ||
          QDSP_IMAGE_FR_YCBCR2_DEFINED         ||
          QDSP_IMAGE_HR_YCBCR2_DEFINED         ||
          QDSP_IMAGE_QVP_YUV_AMR_H263_DEFINED  ||
          QDSP_IMAGE_QVP_YUV_AMR_MPEG4_DEFINED ||
          QDSP_IMAGE_QVP_BAYER_AMR_DEFINED     ||
          QDSP_IMAGE_QVP_MPEG4_AMR_DEFINED     ||
          QDSP_IMAGE_QVP_MPEG4_EVRC_DEFINED    ||
          QDSP_IMAGE_QVP_H263P3_EVRC_DEFINED   ||
          QDSP_IMAGE_VOC_MP3_DEFINED           ||
          QDSP_IMAGE_VOC_EVRC_MP3_DEFINED      ||
          QDSP_IMAGE_AMR_MP3_DEFINED           ||
          QDSP_IMAGE_EFR_MP3_DEFINED           ||
          QDSP_IMAGE_FR_MP3_DEFINED            ||
          QDSP_IMAGE_HR_MP3_DEFINED            ||
          QDSP_IMAGE_VOC_COMBO_ICA_DEFINED     ||
          QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED	   ||
          (FEATURE_AVS_AUDIO_DECODER_SCO    &&
           (QDSP_IMAGE_TUNES_DEFINED        ||
            QDSP_IMAGE_QTV_AAC_DEFINED      ||
            QDSP_IMAGE_ENH_AAC_PLUS_DEFINED ||
            QDSP_IMAGE_RA_DEFINED           ||
            QDSP_IMAGE_QTV_CONC_DEFINED     ||
            QDSP_IMAGE_WM_DEFINED))            */
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
    case QDSP_IMAGE_CAM_AUDIO_CONC:
      if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_MP3 ){
         supported = TRUE;
      }else if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_WAVE ){
         supported = FALSE;
      }
        break;
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */

    default:
       break;
  }

  return supported;
} /* voc_data_image_supports_bt_ag */
#endif /* FEATURE_BT_AG */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_image_supports_bt_a2dp

DESCRIPTION
  Checks if the specified firmware image supports the Bluetooth A2DP SBC codec.

DEPENDENCIES
  None

RETURN VALUE
  TRUE is supported; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_data_image_supports_bt_a2dp
(
  qdsp_image_type image
)
{
  boolean supported = FALSE;

  switch (image) {
#if defined(QDSP_IMAGE_AAC_DEFINED)          || \
    defined(QDSP_IMAGE_QTV_AAC_DEFINED)      || \
    defined(QDSP_IMAGE_QTV_AAC_H264_DEFINED)  || \
    defined(QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED)  || \
    defined(QDSP_IMAGE_SYNTH_DEFINED)        || \
    defined(QDSP_IMAGE_TUNES_DEFINED)        || \
    defined(QDSP_IMAGE_WM_DEFINED)           || \
    defined(QDSP_IMAGE_WM_PRO_DEFINED)           || \
    defined(QDSP_IMAGE_ENH_AAC_PLUS_DEFINED) || \
    defined(QDSP_IMAGE_RA_DEFINED)           || \
    defined(QDSP_IMAGE_QTV_CONC_DEFINED)     || \
    defined(QDSP_IMAGE_I2SSBC_DEFINED)       || \
    defined(QDSP_IMAGE_QTV_DEFINED)  || \
    defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED) ||\
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED) /* A2DP support for for NB-AMR */

#ifdef QDSP_IMAGE_AAC_DEFINED
    case QDSP_IMAGE_AAC:
#endif

#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
    case QDSP_IMAGE_QTV_AAC:
#endif
#ifdef QDSP_IMAGE_TUNES_DEFINED
    case QDSP_IMAGE_TUNES:
#endif
#ifdef QDSP_IMAGE_SYNTH_DEFINED
    case QDSP_IMAGE_SYNTH:
#endif
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
    case QDSP_IMAGE_ENH_AAC_PLUS:
#endif
#ifdef QDSP_IMAGE_WM_DEFINED
    case QDSP_IMAGE_WM:
#endif /* QDSP_IMAGE_WM_DEFINED */
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_WM_PRO_DEFINED */
#ifdef QDSP_IMAGE_RA_DEFINED
    case QDSP_IMAGE_RA:
#endif /* QDSP_IMAGE_RA_DEFINED */
#ifdef QDSP_IMAGE_QTV_CONC_DEFINED
    case QDSP_IMAGE_QTV_CONC:
#endif /* QDSP_IMAGE_QTV_CONC_DEFINED */
#ifdef QDSP_IMAGE_I2SSBC_DEFINED
    case QDSP_IMAGE_I2SSBC:
#endif
     /* A2DP support for for NB-AMR */
#ifdef QDSP_IMAGE_QTV_DEFINED
  case QDSP_IMAGE_QTV:
#endif /* QDSP_IMAGE_QTV_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED */
#ifdef QDSP_IMAGE_QTV_VOC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_VOC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED */
      supported = TRUE;
      break;
#endif /* QDSP_IMAGE_AAC_DEFINED          ||
          QDSP_IMAGE_QTV_AAC_DEFINED      ||
          QDSP_IMAGE_SYNTH_DEFINED        ||
          QDSP_IMAGE_TUNES_DEFINED        ||
          QDSP_IMAGE_WM_DEFINED           ||
          QDSP_IMAGE_ENH_AAC_PLUS_DEFINED ||
          QDSP_IMAGE_RA_DEFINED           ||
          QDSP_IMAGE_QTV_CONC_DEFINED     ||
          QDSP_IMAGE_I2SSBC_DEFINED       */
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
  case QDSP_IMAGE_CAM_AUDIO_CONC:
    if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_MP3 ){
      supported = TRUE;
    }else
    if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_WAVE ){
      supported = FALSE;
    }
    break;
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
    default:
      break;
  }
  return supported;
} /* voc_data_image_supports_bt_a2dp */
#endif /* FEATURE_SBC_CODEC */

/*===========================================================================

FUNCTION voc_data_set_qtv_cb_func

DESCRIPTION
  This function is used to set the QTV callback to be called
  by audio drivers to inform QTV about the QDSP_MOD_DISABLE evt
  when the image swap is happening from audio only image to audio/video image.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_data_set_qtv_cb_func(voc_notify_qtv_cb_func_ptr_type cb_func)
{
  voc_data_qtv_notify_cb_func_ptr = cb_func;
}
/*===========================================================================

FUNCTION voc_data_get_qtv_cb_func

DESCRIPTION
  This function is used to get the callback that qtv registerd with
  audio drivers to get informed about the QDSP_MOD_DISABLE evt when
  the image swap is happening from audio only image to audio/video image.

DEPENDENCIES
  None

RETURN VALUE
  voc_notify_qtv_cb_func_ptr_type

SIDE EFFECTS

==========================================================================*/
voc_notify_qtv_cb_func_ptr_type voc_data_get_qtv_cb_func(void)
{
  return voc_data_qtv_notify_cb_func_ptr;
}

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_bt_cb_func

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
void voc_data_set_bt_cb_func(voc_bt_cb_func_ptr_type cb_func)
{
  voc_data_bt_cb_func_ptr = cb_func;
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_bt_cb_func

DESCRIPTION
  This function is used to return the BT callback that got registered
  with the AVS for the various BT events.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
voc_bt_cb_func_ptr_type voc_data_get_bt_cb_func(void)
{
  return voc_data_bt_cb_func_ptr;
}
#endif/*AVS_BT_SCO_REWORK*/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_codec

DESCRIPTION
  Copy the current codec id to the caller's local data.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  None.

SIDE EFFECTS
  pcm_path_ptr

===========================================================================*/
void voc_data_get_codec
(
  voc_codec_type*         pcm_path_ptr
)
{
  voc_state_control_type *state;

  ASSERT( pcm_path_ptr != NULL );
  ASSERT( voc_data_pcm_cal_ptr != NULL );

  state = voc_state_get_state_data();

  switch (voc_data_codec_class_inuse) {
#ifdef FEATURE_BT_AG
    case VOC_CODEC_CLASS_BLUETOOTH:
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
      if (voc_data_image_supports_bt_ag(state->current_image)) {
      *pcm_path_ptr = voc_data_codec_inuse;
      } else {
#ifdef FEATURE_SPEAKER_PHONE
      *pcm_path_ptr = VOC_CODEC_SPEAKER;
#else
      *pcm_path_ptr = VOC_CODEC_DEFAULT;
#endif /* FEATURE_SPEAKER_PHONE */
    }
      break;
#endif /* FEATURE_BT_AG */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    case VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO:
      if (voc_data_image_supports_bt_a2dp(state->current_image)) {
    *pcm_path_ptr = voc_data_codec_inuse;
      } else if (voc_data_image_supports_bt_ag(state->current_image)) {
    *pcm_path_ptr = voc_data_codec_inuse;
      } else {
#ifdef FEATURE_SPEAKER_PHONE
          *pcm_path_ptr = VOC_CODEC_SPEAKER;
#else
          *pcm_path_ptr = VOC_CODEC_ON_CHIP_0;
#endif /* FEATURE_SPEAKER_PHONE */
  }
      break;

    case VOC_CODEC_CLASS_BLUETOOTH_A2DP:
      if (voc_data_image_supports_bt_a2dp(state->current_image)) {
        *pcm_path_ptr = voc_data_codec_inuse;
      } else {
#ifdef FEATURE_SPEAKER_PHONE
          *pcm_path_ptr = VOC_CODEC_SPEAKER;
#else
          *pcm_path_ptr = VOC_CODEC_ON_CHIP_0;
#endif /* FEATURE_SPEAKER_PHONE */
      }
      break;
#endif /* FEATURE_SBC_CODEC */
    default:
      *pcm_path_ptr = voc_data_codec_inuse;
      break;
  }
} /* voc_data_get_codec */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_codec_class

DESCRIPTION
  Copy the current codec class to the caller's local data.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  None.

SIDE EFFECTS
  pcm_path_ptr

===========================================================================*/
void voc_data_get_codec_class
(
  voc_codec_class_type*         codec_type_ptr
)
{
  voc_codec_type         pcm_path;

  voc_data_get_codec(&pcm_path);
  *codec_type_ptr = voc_cal_get_codec_class(pcm_path);
} /* voc_data_get_codec_class */

#ifdef FEATURE_PHONE_VR
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_codec_vr_type

DESCRIPTION
  Copy the current codec VR type to the caller's local data.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  None.

SIDE EFFECTS
  pcm_path_ptr

===========================================================================*/
void voc_data_get_codec_vr_type
(
  voc_codec_vr_type*         codec_type_ptr
)
{
  voc_codec_type         pcm_path;

  voc_data_get_codec(&pcm_path);
  *codec_type_ptr = voc_cal_get_codec_vr_type(pcm_path);
}
#endif /* FEATURE_PHONE_VR */
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_default_frame_rate

DESCRIPTION
  Gets the default PCM sampling rate to be used depending on image and current
  codec.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  TODO: Need to resolve concurrency with pending attributes - Client requests.

===========================================================================*/

extern qdsp_cmd_pcm_sample_rate_type voc_data_get_default_frame_rate
(
  qdsp_image_type        image,
  voc_codec_type         codec
){
  qdsp_cmd_pcm_sample_rate_type default_frame_rate;
  default_frame_rate =  QDSP_CMD_PCM_SAMPLE_RATE_8000;
  switch (codec) {
#ifdef FEATURE_TTY
  case VOC_CODEC_TTY_ON_CHIP_1:        /* TTY On Chip Codec Channel 2      */
  case VOC_CODEC_TTY_OFF_BOARD:        /* TTY Aux. Codec                   */
  case VOC_CODEC_TTY_VCO:              /* Voice Carry-Over TTY             */
  case VOC_CODEC_TTY_HCO:              /* Hearing Carry-Over TTY           */
    break;
#endif/* FEATURE_TTY*/
#ifdef FEATURE_BT
  case VOC_CODEC_BT_INTERCOM:          /* BT Intercom                      */
#ifdef FEATURE_BT_AG
  case VOC_CODEC_BT_AG:                /* BT path                          */
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
    break;
#endif /* FEATURE_BT */
  default:
    if (voc_state_voc_tx_wb_query()) {
      default_frame_rate =  QDSP_CMD_PCM_SAMPLE_RATE_16000;
    } else {
      default_frame_rate =  QDSP_CMD_PCM_SAMPLE_RATE_8000;
    }
  }
  return  default_frame_rate;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_codec_config

DESCRIPTION
  Get the codec configuration for the image and codec indicated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  codec_config_ptr

===========================================================================*/
void voc_data_get_codec_config
(
  qdsp_image_type              image,
  voc_codec_type               codec,
  qdsp_cmd_codec_config_type** codec_config_ptr
)
{
  static qdsp_cmd_codec_config_type codec_config;
  qdsp_cmd_codec_config_type        *temp_codec_config_ptr;
  voc_codec_mode_type 			mode;
  voc_codec_class_type   		codec_type;


  ASSERT( codec_config_ptr != NULL );
  ASSERT( voc_data_pcm_cal_ptr != NULL );

  (void)voc_cal_get_codec_config(codec, image, &temp_codec_config_ptr);

  voc_data_get_codec_mode(image, &mode);
  voc_data_get_codec_class(&codec_type);

  codec_config = *temp_codec_config_ptr;

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    /* disabled front end high pass filter for wideband vocoders for now */
    if (voc_data_aud_process_override_enabled)
    {
      /* Disable Rx/Tx HPF and Tx slope filter for audio override mode */
      codec_config.pcm_format_ctrl &= ~(QDSP_RX_HPF_M | QDSP_TX_HPF_M |
                                        QDSP_TX_SLOPE_FILT_M);

      codec_config.pcm_format_ctrl |= (QDSP_RX_HPF_DIS_V | QDSP_TX_HPF_DIS_V |
                                       QDSP_TX_SLOPE_FILT_DIS_V);
    }
    else if (voc_state_voc_tx_wb_query() == TRUE)
    {
      /* Disable only Tx slope filter for WB calls */
      codec_config.pcm_format_ctrl &= ~(QDSP_TX_SLOPE_FILT_M);
      codec_config.pcm_format_ctrl |= QDSP_TX_SLOPE_FILT_DIS_V;
    }
  }
#ifdef SND_TEST_AUXPCM_INTERFACE_MODES
#error code not present
#endif

  *codec_config_ptr = &codec_config;
} /* voc_data_get_codec_config */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_ec_override_mode

DESCRIPTION
  Controls EC (Echo Canceller).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_set_ec_override_mode (
  voc_ec_ns_mode_type   ec_mode
) {
  voc_data_ec_override_mode = ec_mode;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_ns_override_mode

DESCRIPTION
  Controls NS (Noise Suppressor).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_set_ns_override_mode (
  voc_ec_ns_mode_type   ns_mode
) {
  voc_data_ns_override_mode = ns_mode;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_vox_override_mode

DESCRIPTION
  Controls VOX (half/full duplex voice mode).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_set_vox_override_mode (
  voc_vox_mode_type   vox_mode
) {
  voc_data_vox_override_mode = vox_mode;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_sidetone_override_mode

DESCRIPTION
  Controls sidetone.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_set_sidetone_override_mode (
  voc_sidetone_mode_type   sidetone_mode
)
{
  voc_data_sidetone_override_mode = sidetone_mode;
}


/* <EJECT> */
/*==========================================================================
 FUNCTION voc_data_smartsnd_set_rx_agc

 DESCRIPTION
   Set the current AGC/AVC configuration for the ear piece

 DEPENDENCIES
   None

 RETURN
   VOC_DATA_SUCCESS

 SIDE EFFECTS
  voc_data_smartsnd_rx_agc_config

 ==========================================================================*/

voc_data_return_type voc_data_smartsnd_set_rx_agc
(
  voc_smartsnd_type agc_config
) {
    voc_data_smartsnd_rx_agc_config = agc_config;
    return (VOC_DATA_SUCCESS);
}

/* <EJECT> */
/*==========================================================================
  FUNCTION voc_data_smartsnd_set_rve

  DESCRIPTION
    Set the current RVE configuration

  RETURN
    VOC_DATA_SUCCESS

  SIDE_EFFECTS
    voc_data_smartsnd_rve_config

 ==========================================================================*/

voc_data_return_type voc_data_smartsnd_set_rve
(
 voc_smartsnd_type rve_config
) {
    voc_data_smartsnd_rve_config = rve_config;
    return (VOC_DATA_SUCCESS);
}


/* <EJECT> */
#ifdef FEATURE_AUDIO_AGC
/*===========================================================================

FUNCTION voc_data_set_rx_agc

DESCRIPTION
  Set the current AGC/AVC configuration for the earpiece.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DATA_SUCCESS -

SIDE EFFECTS
  voc_data_rx_agc_config

===========================================================================*/
voc_data_return_type voc_data_set_rx_agc
(
  voc_agc_type agc_config
) {
  voc_data_rx_agc_config = agc_config;
  return( VOC_DATA_SUCCESS );
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_tx_agc

DESCRIPTION
  Set the current tx AGC configuration.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DATA_SUCCESS -

SIDE EFFECTS
  voc_data_tx_agc_config

===========================================================================*/
voc_data_return_type voc_data_set_tx_agc
(
  boolean      agc_config
) {
  voc_data_tx_agc_config = agc_config;
  return( VOC_DATA_SUCCESS );
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_agc

DESCRIPTION
  This function returns the QDSP AGC format command based on the current
  AGC/AVC configuration.

DEPENDENCIES
  voc_data_rx_agc_config
  voc_data_tx_agc_config

RETURN VALUE
  qdsp_cmd_agc_config_type

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_get_agc
(
  qdsp_cmd_agc_config_type *agc_config_ptr
) {

  ASSERT( agc_config_ptr != NULL );

  if(voc_data_tx_agc_config) {
    agc_config_ptr->tx_agc_enable = QDSP_CMD_AGC_ENA;
  } else {
    agc_config_ptr->tx_agc_enable = QDSP_CMD_AGC_DIS;
  }

  switch (voc_data_rx_agc_config) {
    case VOC_AGC_NONE:
      if( qdsp_cmd_rve_is_supported() == QDSP_CMD_SUCCESS )
          agc_config_ptr->rx_agc_enable = QDSP_CMD_AGC_ENA;
      else
      agc_config_ptr->rx_agc_enable = QDSP_CMD_AGC_DIS;
      agc_config_ptr->rx_avc_enable = QDSP_CMD_AGC_DIS;
      break;
    case VOC_AGC_NO_AVC:
      agc_config_ptr->rx_agc_enable = QDSP_CMD_AGC_ENA;
      agc_config_ptr->rx_avc_enable = QDSP_CMD_AGC_DIS;
      break;
    case VOC_AGC_ALL:
      agc_config_ptr->rx_agc_enable = QDSP_CMD_AGC_ENA;
      agc_config_ptr->rx_avc_enable = QDSP_CMD_AGC_ENA;
      break;
    default:
      ERR_FATAL("Illegal AGC mode: %d.", voc_data_rx_agc_config,0,0);
      break;
  }

}


/* <EJECT> */
/*===========================================================================
  FUNCTION voc_data_smartsnd_get_agc
  DESCRIPTION
  This function returns the QDSP AGC format command based on the current
  AGC/AVC configuration.

 DEPENDENCIES
  voc_data_smartsnd_rx_agc_config

 RETURN VALUE
  qdsp_cmd_agc_config_type

 SIDE EFFECTS
  None.

 ===========================================================================*/

void voc_data_smartsnd_get_agc(
qdsp_cmd_agc_config_type  *agc_config_ptr
){

  ASSERT( agc_config_ptr != NULL );

   if(voc_data_tx_agc_config) {
    agc_config_ptr->tx_agc_enable = QDSP_CMD_AGC_ENA;
  } else {
    agc_config_ptr->tx_agc_enable = QDSP_CMD_AGC_DIS;
  }

  switch (voc_data_smartsnd_rx_agc_config) {
    case VOC_SMARTSND_NONE:
      agc_config_ptr->rx_agc_enable = QDSP_CMD_AGC_DIS;
      agc_config_ptr->rx_avc_enable = QDSP_CMD_AGC_DIS;
      break;
    case VOC_SMARTSND_AGC:
    case VOC_SMARTSND_AGC_AIG:
      agc_config_ptr->rx_agc_enable = QDSP_CMD_AGC_ENA;
      agc_config_ptr->rx_avc_enable = QDSP_CMD_AGC_DIS;
      break;
    case VOC_SMARTSND_AGC_AVC_AIG:
    case VOC_SMARTSND_AGC_AVC:
      agc_config_ptr->rx_agc_enable = QDSP_CMD_AGC_ENA;
      agc_config_ptr->rx_avc_enable = QDSP_CMD_AGC_ENA;
      break;
    case VOC_SMARTSND_AVC:
      agc_config_ptr->rx_agc_enable = QDSP_CMD_AGC_DIS;
      agc_config_ptr->rx_avc_enable = QDSP_CMD_AGC_ENA;
      break;
    default:
      ERR_FATAL("Illegal AGC mode: %d.", voc_data_smartsnd_rx_agc_config,0,0);
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_agc_param

DESCRIPTION
  This function returns the QDSP AGC format command based on the current
  PCM Path AGC/AVC configuration.

DEPENDENCIES
  voc_data_rx_agc_config
  voc_data_tx_agc_config
  voc_data_pcm_cal_ptr

RETURN VALUE
  qdsp_cmd_agc_param_type

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_get_agc_param
(
  qdsp_cmd_agc_param_type *agc_param_ptr
)
{

  ASSERT( agc_param_ptr != NULL );
  ASSERT( voc_data_pcm_cal_ptr != NULL );

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    *agc_param_ptr = voc_data_pcm_cal_ptr->agc_param;
  }
}

/* <EJECT> */
/*=============================================================================

FUNCTION voc_data_smartsnd_get_agc_param

DESCRIPTION
  This function returns the QDSP AGC format command based on the current
  PCM Path AGC/AVC configuration.

DEPENDENCIES
  voc_data_smartsnd_rx_agc_config
  voc_data_tx_agc_config
  voc_data_pcm_cal_ptr

RETURN VALUE
  qdsp_cmd_agc_param_type

SIDE EFFECTS
  None.


 =============================================================================*/
void voc_data_smartsnd_get_agc_param
(
  qdsp_cmd_agc_param_type *agc_param_ptr
)
{

  ASSERT( agc_param_ptr != NULL );
  ASSERT( voc_data_pcm_cal_ptr != NULL );

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    *agc_param_ptr = voc_data_pcm_cal_ptr->agc_param;
  }
  /*Check for with/without AIG */
  switch (voc_data_smartsnd_rx_agc_config) {
	case VOC_SMARTSND_AGC:
    case VOC_SMARTSND_AGC_AVC:
    case VOC_SMARTSND_AGC_AVC_AIG:
    case VOC_SMARTSND_AGC_AIG:
    case VOC_SMARTSND_NONE:
    case VOC_SMARTSND_AVC:
	  /* to do. diable AIG he1re */
      break;
    default:
      ERR_FATAL("Illegal AGC mode: %d.", voc_data_smartsnd_rx_agc_config,0,0);
      break;
  }
}
#endif /* FEATURE_AUDIO_AGC */
/* <EJECT> */



/*===========================================================================

FUNCTION voc_data_get_rve

DESCRIPTION
  This function returns the QDSP RVE format command based on the current
  RVE configuration.

DEPENDENCIES
  voc_data_rx_agc_config

RETURN VALUE
  qdsp_cmd_rve_config_type

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_get_rve
(
  qdsp_cmd_rve_config_type *rve_config_ptr
) {

  ASSERT( rve_config_ptr != NULL );

  switch(voc_data_smartsnd_rve_config){
  case VOC_SMARTSND_RVE_NONE:
	  rve_config_ptr->rx_rve_enable = QDSP_CMD_RVE_DIS;
	  break;
  case VOC_SMARTSND_RVE_AVAIL:
	  rve_config_ptr->rx_rve_enable = QDSP_CMD_RVE_ENA;
	  break;
  default:
	  ERR_FATAL("Illegal RVE mode: %d.", voc_data_get_rve,0,0);
  }

}
/* <EJECT> */

/*===========================================================================

FUNCTION voc_data_get_rve_param

DESCRIPTION
  This function returns the QDSP RVE format command based on the current
  PCM Path RVE configuration for Single Mode Mic.

DEPENDENCIES
  voc_data_pcm_cal_ptr

RETURN VALUE
  qdsp_cmd_rve_param_type

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_rve_param_type* voc_data_get_rve_param
(
 voc_ec_type ec_mode
)
{

  ASSERT( voc_data_pcm_cal_ptr != NULL );

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
     return (voc_data_pcm_cal_ptr->rve_param[ec_mode]);
  }
}

/*===========================================================================

FUNCTION voc_data_get_dual_rve_param

DESCRIPTION
  This function returns the QDSP RVE DUAL MIC  format command based on the current
  PCM Path RVE configuration.

DEPENDENCIES
  voc_data_pcm_cal_ptr

RETURN VALUE
  qdsp_cmd_dual_rve_param_type

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_dual_rve_param_type* voc_data_get_dual_rve_param
(
 voc_ec_type ec_mode
)
{

  ASSERT( voc_data_pcm_cal_ptr != NULL );

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
      return (voc_data_pcm_cal_ptr->dual_rve_param[ec_mode]);
  }
}


/* <EJECT> */

#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_AMR_WB_COMMON)

/*===========================================================================

FUNCTION voc_data_get_gsm_dtx_mode

DESCRIPTION
  Get the current GSM DTX mode setting.

DEPENDENCIES
  None

RETURN VALUE
  voc_data_gsm_dtx_mode

SIDE EFFECTS
  None

===========================================================================*/
voc_gsm_dtx_mode_type voc_data_get_gsm_dtx_mode(void)
{
  return voc_data_gsm_dtx_mode;
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_gsm_dtx_mode

DESCRIPTION
  Set GSM DTX mode database value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_set_gsm_dtx_mode(voc_gsm_dtx_mode_type dtx_mode)
{
  voc_data_gsm_dtx_mode = dtx_mode;
}

#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR || MSMAUD_VOC_HR || MSMAUD_VOC_AMR_WB_COMMON */

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_WB_COMMON
/*===========================================================================

FUNCTION voc_data_get_amr_wb_init_rate

DESCRIPTION
  Get the current initial AMR_WB Tx rate setting.

DEPENDENCIES
  None

RETURN VALUE
  voc_data_amr_wb_init_rate

SIDE EFFECTS
  None

===========================================================================*/
voc_amr_wb_frame_rate_type voc_data_get_amr_wb_init_rate(void)
{
  return voc_data_amr_wb_init_rate;
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_amr_wb_init_rate

DESCRIPTION
  Set initial AMR_WB Tx rate database value.
  Invalid argument of VOC_AMR_WB_FULL_RATE will be ignored.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_set_amr_wb_init_rate(voc_amr_wb_frame_rate_type amr_wb_tx_rate)
{
  if ((amr_wb_tx_rate >= VOC_AMR_WB_RATE_0660)
      && (amr_wb_tx_rate <= VOC_AMR_WB_RATE_2385))
  {
     voc_data_amr_wb_init_rate = amr_wb_tx_rate;
  }

}
#endif /* MSMAUD_VOC_AMR_WB_COMMON */

/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_COMMON
/*===========================================================================

FUNCTION voc_data_get_amr_init_rate

DESCRIPTION
  Get the current initial AMR Tx rate setting.

DEPENDENCIES
  None

RETURN VALUE
  voc_data_amr_init_rate

SIDE EFFECTS
  None

===========================================================================*/
voc_gsm_frame_rate_type voc_data_get_amr_init_rate(void)
{
  return voc_data_amr_init_rate;
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_amr_init_rate

DESCRIPTION
  Set initial AMR Tx rate database value.
  Invalid argument of VOC_GSM_FULL_RATE will be ignored.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_set_amr_init_rate(voc_gsm_frame_rate_type amr_tx_rate)
{
  if ((amr_tx_rate >= VOC_AMR_RATE_475) && (amr_tx_rate <= VOC_AMR_RATE_1220))
  {
     voc_data_amr_init_rate = amr_tx_rate;
  }

}
#endif /* MSMAUD_VOC_AMR_COMMON */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_1x_tx_rate_factor

DESCRIPTION
  Set tx rate factor

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_set_1x_tx_rate_factor(word rate_factor)
{
  voc_data_1x_tx_rate_factor = rate_factor;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_1x_tx_rate_factor

DESCRIPTION
  Retrieve tx rate factor

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
word voc_data_get_1x_tx_rate_factor(void)
{
  return voc_data_1x_tx_rate_factor;
}

/* <EJECT> */
#ifdef MSMAUD_VOC_G711
/*===========================================================================

FUNCTION voc_data_get_g711_mode

DESCRIPTION
  Get the current initial G.711 mode setting.

DEPENDENCIES
  None

RETURN VALUE
  voc_data_g711_mode

SIDE EFFECTS
  voc_data_g711_mode

===========================================================================*/
voc_g711_mode_type voc_data_get_g711_mode(void)
{
  return(voc_data_g711_mode);
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_g711_mode

DESCRIPTION
  Set initial G.711 mode database value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  voc_data_g711_mode

===========================================================================*/
void voc_data_set_g711_mode(voc_g711_mode_type g711_mode)
{
  if ((g711_mode >= VOC_G711_MODE_MULAW) && (g711_mode <= VOC_G711_MODE_ALAW)) {
    voc_data_g711_mode = g711_mode;
  }
}
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
/*===========================================================================

FUNCTION voc_data_get_g723_mode

DESCRIPTION
  Get the current initial G.723 mode setting.

DEPENDENCIES
  None

RETURN VALUE
  voc_data_g723_mode

SIDE EFFECTS
  voc_data_g723_mode

===========================================================================*/
voc_g723_mode_type voc_data_get_g723_mode(void)
{
  return(voc_data_g723_mode);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_g723_mode

DESCRIPTION
  Set initial G.723 mode database value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  voc_data_g723_mode

===========================================================================*/
void voc_data_set_g723_mode(voc_g723_mode_type g723_mode)
{
  if ((g723_mode >= VOC_G723_MODE_HIGH) && (g723_mode < VOC_G723_MODE_MAX)) {
    voc_data_g723_mode = g723_mode;
  }
}
#endif /* MSMAUD_VOC_G723 */


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_volume

DESCRIPTION
  Get the current volume settings.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_vol_ctrl_type* - Pointer to the current volume setting.

SIDE EFFECTS
  voc_data_vol.tx_vol

===========================================================================*/
qdsp_cmd_vol_ctrl_type* voc_data_get_volume
(
  voc_data_vol_select_type vol_sel
)
{
  ASSERT( voc_data_pcm_cal_ptr != NULL );

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    if (vol_sel == VOC_DATA_MUTE_VOL) {
      return (qdsp_cmd_vol_ctrl_type *)(&voc_data_modem_mute);
    } else {

      if (voc_state_get_state_data()->test_mode != VOC_TEST_NONE)
      {
        return (qdsp_cmd_vol_ctrl_type*)(&voc_data_lbtest_vol);
      }

      /* Only enable comfort noise if not Txing DTMF and Tx is mute */
      if((voc_data_dtmf_tx_mute_enable) && (voc_data_tx_mute_enable)) {
        voc_data_vol.tx_comfort_noise_ena = QDSP_COMFORT_NOISE_ENA_V;
      } else {
        voc_data_vol.tx_comfort_noise_ena = QDSP_COMFORT_NOISE_DIS_V;
      }

      /* Need to mute Tx Volume only if Txing DTMF and Tx mute is enabled */
      if((!voc_data_dtmf_tx_mute_enable) && (voc_data_tx_mute_enable)) {
        voc_data_vol.tx_vol = 0;
      } else {

#ifdef FEATURE_PHONE_VR
        ASSERT( voc_data_vr_cal_ptr != NULL );

        if ((voc_data_vr_mode == VOC_VR_MODE_BC
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
          ) && (voc_data_vr_cal_ptr->tx_gain != 0)) {

          /* Get the transmit volume from the VR engine calibration data.
          */
          voc_data_vol.tx_vol = voc_data_vr_cal_ptr->tx_gain;
        } else
#endif /* FEATURE_PHONE_VR */
        {
          /* Get the transmit volume from the vocoder calibration data.  The
          ** other volume values are set by the application
          */
          voc_data_vol.tx_vol = voc_data_pcm_cal_ptr->tx_gain;
        }
      }

      /* Calculate rx volume from stored rx dBm + codec offset value */
      voc_data_vol.rx_vol = voc_data_compute_dbm(VOC_CAL_DB0_RX_GAIN,
                       voc_data_rx_vol_dbm + voc_data_pcm_cal_ptr->rx_dbm_offset);

      return (&voc_data_vol);
    }
  }
} /* voc_data_get_volume */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_rx_volume

DESCRIPTION
  Set the RX Volume.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_vol.rx_vol

===========================================================================*/
void voc_data_set_rx_volume
(
  sint15 rx_vol
)
{
  voc_data_rx_vol_dbm = rx_vol;
} /* voc_data_set_rx_volume */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_rx_mute

DESCRIPTION
  Set the RX muting.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_vol.rx_mute

===========================================================================*/
void voc_data_set_rx_mute
(
  boolean mute_flag,
  boolean conditional_on_mixing
)
{
  uint16 qdsp_mute;

  if ( mute_flag == TRUE )
  {
    /* Give preference to full mute. */
    if ( voc_data_vol.rx_mute != QDSP_RX_MUTE_ENA_V
      && conditional_on_mixing == TRUE )
    {
      qdsp_mute = QDSP_RX_MUTE_CONDITIONAL_V;
    }
    else
    {
      qdsp_mute = QDSP_RX_MUTE_ENA_V;
    }
  }
  else
  {
    qdsp_mute = QDSP_RX_MUTE_DIS_V;
  }

  voc_data_vol.rx_mute = qdsp_mute;
} /* voc_data_set_rx_mute */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_tx_mute

DESCRIPTION
  Set TX muting.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_tx_mute_enable

===========================================================================*/
void voc_data_set_tx_mute
(
  boolean mute_flag
)
{
  voc_data_tx_mute_enable = mute_flag;
} /* voc_data_set_tx_mute */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_dtmf_path_setting

DESCRIPTION
  Get DTMF path setting.

DEPENDENCIES
  voc_data_dtmf_tx_mute_enable

RETURN VALUE
  DTMF Path Setting.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 voc_data_get_dtmf_path_setting()
{
  uint16 ret_val;
  if(voc_data_dtmf_tx_mute_enable) {
    ret_val = QDSP_DTMF_RX_ENA_V | QDSP_DTMF_TX_DIS_V;
  } else {
    ret_val = QDSP_DTMF_RX_ENA_V | QDSP_DTMF_TX_ENA_V;
  }
  return (ret_val);
} /* voc_data_get_dtmf_path_setting */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_dtmf_tx_mute

DESCRIPTION
  Set DTMF TX muting.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_dtmf_tx_mute_enable

===========================================================================*/
void voc_data_set_dtmf_tx_mute (
  boolean mute_flag
) {
  voc_data_dtmf_tx_mute_enable = mute_flag;
} /* voc_data_set_dtmf_tx_mute */

/* <EJECT> */
#ifdef MSMAUD_PCM_DC_OFFSET
/*===========================================================================

FUNCTION voc_data_get_codec_offset

DESCRIPTION
  Get CODEC offset values

DEPENDENCIES
  None.

RETURN VALUE
  codec offset values

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_get_codec_offset(
  qdsp_cmd_codec_offset_ctrl_type *codec_offset_ptr
)
{
  ASSERT( codec_offset_ptr != NULL );
  ASSERT( voc_data_pcm_cal_ptr != NULL );

  codec_offset_ptr->codec_rxL_offset = voc_data_pcm_cal_ptr->codec_rxL_offset;
  codec_offset_ptr->codec_rxR_offset = voc_data_pcm_cal_ptr->codec_rxR_offset;
}

#endif /* MSMAUD_PCM_DC_OFFSET */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_codec_gain

DESCRIPTION
  Get CODEC gain values

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the codec gain table

SIDE EFFECTS
  voc_data_codec_gain.codec_rx_gain
  voc_data_codec_gain.codec_st_gain
  voc_data_codec_gain.codec_tx_gain

===========================================================================*/
qdsp_cmd_codec_gain_ctrl_type* voc_data_get_codec_gain()
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    if(voc_data_aud_process_override_enabled) {
      return (qdsp_cmd_codec_gain_ctrl_type*) (&voc_data_aud_override_codec_gain);
    } else if (voc_state_get_state_data()->test_mode != VOC_TEST_NONE) {
      return (qdsp_cmd_codec_gain_ctrl_type*) (&voc_data_lbtest_codec_gain);
    }

#ifdef FEATURE_VOC_MIC_GAIN_ADJUST
    if (*voc_data_mic_gain_adjust_ptr != 0)
    {
      voc_data_codec_gain.codec_tx_gain = voc_data_adjust_dbm(
        VOC_CAL_TX_GAIN_NOM,
        voc_data_pcm_cal_ptr->codec_tx_gain,
        *voc_data_mic_gain_adjust_ptr
        );
    }
    else
#endif /* FEATURE_VOC_MIC_GAIN_ADJUST */
    {
      voc_data_codec_gain.codec_tx_gain = voc_data_pcm_cal_ptr->codec_tx_gain;
    }

    voc_data_codec_gain.codec_rx_gain = voc_data_pcm_cal_ptr->codec_rx_gain;
    voc_data_codec_gain.codec_st_gain = voc_data_pcm_cal_ptr->codec_st_gain;

    return(&voc_data_codec_gain);
  }
} /* voc_data_get_codec_gain */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_pcmpad_vol

DESCRIPTION
  Set External CODEC pcm pad gains bits

DEPENDENCIES
  None.

RETURN VALUE
  The CODEC pcm pad hardware value

SIDE EFFECTS
  voc_data_codec_pad

===========================================================================*/
uint16 voc_data_set_pcmpad_vol
(
  uint8   padval,             /* Value for pad bits          */
  uint8   mask,               /* Mask of pad bits to control */
  boolean update              /* Only update value if TRUE   */
)
{
  if (update) {
    voc_data_codec_mask = mask;
    voc_data_codec_pad  =
           ((((voc_data_codec_pad >> 8) & ~mask) | (padval & mask)) << 8 );
  }
  return(voc_data_codec_pad);
} /* voc_data_set_pcmpad_vol */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_pcmpad_ctl

DESCRIPTION
  Set External CODEC pcm pad control bits

DEPENDENCIES
  None.

RETURN VALUE
  The CODEC pcm pad hardware value

SIDE EFFECTS
  voc_data_codec_pad

===========================================================================*/
uint16 voc_data_set_pcmpad_ctl
(
  uint8   padval             /* Value for pad bits          */
)
{
  /* Combine the pad_byte with the current codec pad.
  ** This routine is only allowed to change bits that are not controlled
  ** by the codec_pad routine.
  */
  voc_data_codec_pad  = ((((voc_data_codec_pad >> 8) & voc_data_codec_mask)|
                          (padval & ~voc_data_codec_mask)) << 8 );
  return(voc_data_codec_pad);
} /* voc_data_set_pcmpad_ctl */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_timing

DESCRIPTION
  Gets Frame Timing Data.

DEPENDENCIES
  None.

RETURN VALUE
  qdsp_cmd_voc_timing_type* - Pointer the current frame timing.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_voc_timing_type* voc_data_get_timing
(
  voc_capability_type so
)
{
  qdsp_cmd_voc_timing_type *ret_val = NULL;

#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
  voc_capability_type    desired_config;
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifdef FEATURE_PHONE_VR
  voc_state_control_type *state;

  state = voc_state_get_state_data ();

  if (state->vr_mode == VOC_VR_MODE_BC
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
   )
  {
    ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_vrmodebc_timing;
    return(ret_val);
  }
#endif /* FEATURE_PHONE_VR */

  switch( so )
  {
    case VOC_CAP_IS733:
      ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_is733_timing;
      break;

    case VOC_CAP_IS127:
#ifdef MSMAUD_VOC_IS127_VOIP
    case VOC_CAP_IS127_VOIP:
#endif /* MSMAUD_VOC_IS127_VOIP */
      ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_is127_timing;
      break;

#ifdef MSMAUD_VOC_4GV
    case VOC_CAP_4GV_NB:
#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif
      ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_4gv_timing;
      break;
#endif  /* MSMAUD_VOC_4GV */

#ifdef MSMAUD_VOC_4GV_WB
    case VOC_CAP_4GV_WB:
#ifdef MSMAUD_VOC_4GV_WB_VOIP
    case VOC_CAP_4GV_WB_VOIP:
#endif
      ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_4gv_wb_timing;
      break;
#endif  /* MSMAUD_VOC_4GV_WB */


#if defined(MSMAUD_VOC_AMR_WB_COMMON)
    case VOC_CAP_AMR_WB:
      ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_amr_wb_wcdma_timing;
      break;
#endif /* defined(MSMAUD_VOC_AMR_WB_COMMON) */

#if defined(MSMAUD_VOC_AMR_COMMON)
    case VOC_CAP_AMR:
#ifdef FEATURE_GSM
#error code not present
#else
      ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_wcdma_timing;
#endif
      break;
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

#if defined(MSMAUD_VOC_EFR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) */

#if defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_FR) */

#if defined(MSMAUD_VOC_HR)
    case VOC_CAP_GSM_HR:
      ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_gsm_timing;
      break;
#endif /* defined(MSMAUD_VOC_HR) */

#ifdef MSMAUD_VOC_G711
    case VOC_CAP_G711:
      ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_wlan_timing;
      break;
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
    case VOC_CAP_G723:
      ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_wlan_timing;
      break;
#endif /* MSMAUD_VOC_G723 */

#if defined(FEATURE_QTUNES)     || \
    defined(FEATURE_QTUNES_AAC) || \
    defined(FEATURE_QSYNTH_COMMON)
#if defined(FEATURE_QTUNES)
  case VOC_CAP_QTUNES:
#endif
#if defined(FEATURE_QTUNES_AAC)
  case VOC_CAP_QTUNES_AAC:
#endif
#ifdef FEATURE_WMA
    case VOC_CAP_QTUNES_WMA:
    case VOC_CAP_QTUNES_WMA_PRO:
#endif
    case VOC_CAP_QSYNTH:
#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
      desired_config = voc_get_desired_config(VOC_ACQ_CDMA)
#ifdef FEATURE_MVS
                       | voc_get_desired_config(VOC_ACQ_MVS)
#endif /* FEATURE_MVS */
      ;

      switch (desired_config) {
        case VOC_CAP_IS733:
          ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_is733_timing;
          break;

        case VOC_CAP_IS127:
          ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_is127_timing;
          break;

#ifdef FEATURE_MVS
        case VOC_CAP_AMR: {
#ifdef FEATURE_GSM
#error code not present
#else
          ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_wcdma_timing;
#endif
        } /* case VOC_CAP_AMR */
          break;

        case VOC_CAP_GSM_EFR:
        case VOC_CAP_GSM_FR:
        case VOC_CAP_GSM_HR: {
          ret_val = (qdsp_cmd_voc_timing_type*)&voc_data_gsm_timing;
        } /* case VOC_CAP_IS127 */
          break;
#endif /* FEATURE_MVS */

        default:
          ret_val = FALSE;
          break;
      } /* switch */
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */
      break;
#endif /* FEATURE_QTUNES || FEATURE_QTUNES_AAC || FEATURE_WMA ||
          FEATURE_QSYNTH_COMMON */

    default:
      ERR_FATAL( "so is invalid: %x", so, 0, 0 );
      break;
  }
  return(ret_val);
} /* voc_data_get_timing */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_ec_mode

DESCRIPTION
  Get the echo-cancellation mode.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  voc_ec_type* - pointer to current echo cancellor mode.

SIDE EFFECTS
  None.

===========================================================================*/
voc_ec_type* voc_data_get_ec_mode
(
  void
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    static voc_ec_type override_mode = VOC_EC_OFF;

    if (voc_data_ec_override_mode == VOC_EC_NS_FORCE_DISABLED) {
      return( &override_mode );
    }

    if (voc_data_vox_override_mode == VOC_VOX_FORCE_ENABLED) {
      switch (voc_state_get_state_data()->current_image) {
#if defined(QDSP_IMAGE_QVP_YUV_AMR_H263_DEFINED) || \
    defined(QDSP_IMAGE_QVP_YUV_AMR_MPEG4_DEFINED)
#ifdef QDSP_IMAGE_QVP_YUV_AMR_H263_DEFINED
        case QDSP_IMAGE_QVP_YUV_AMR_H263:
#endif
#ifdef QDSP_IMAGE_QVP_YUV_AMR_MPEG4_DEFINED
        case QDSP_IMAGE_QVP_YUV_AMR_MPEG4:
#endif
          return( &override_mode );
#endif

        default:
          break;
      }
    }
    return( &voc_data_pcm_cal_ptr->ec_mode );
  }
} /* voc_data_get_ec_mode */

/* <EJECT> */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/*===========================================================================

FUNCTION voc_data_get_crystal_speech_parms

DESCRIPTION
  Get the Crystal Speech parameters.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_crystal_speech_parms_type* - pointer to current parameters

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_crystal_speech_parms_type * voc_data_get_crystal_speech_parms
(
  voc_ec_type ec_mode
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    if (voc_state_voc_tx_wb_query() == TRUE)
    {
      return( voc_data_pcm_cal_ptr->cs_wb_parms[ec_mode] );
    }
    else
    {
      return( voc_data_pcm_cal_ptr->cs_parms[ec_mode] );
    }
  }
} /* voc_data_get_crystal_speech_parms */

#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */


/* <EJECT> */
#ifdef FEATURE_AVS_NEXTGEN_EC
/*===========================================================================

FUNCTION voc_data_get_nextgen_ec_parms

DESCRIPTION
  Get the echo-cancellation parameters.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_nextgen_ec_parms_type* - pointer to current echo cancellor parameters

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_nextgen_ec_parms_type * voc_data_get_nextgen_ec_parms
(
  voc_ec_type ec_mode
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    if (voc_state_voc_tx_wb_query() == TRUE)
    {
      return( voc_data_pcm_cal_ptr->nextgen_ec_wb_parms[ec_mode] );
    }
    else
    {
      return( voc_data_pcm_cal_ptr->nextgen_ec_parms[ec_mode] );
    }
  }
} /* voc_data_get_nextgen_ec_parms */

#endif  /* FEATURE_AVS_NEXTGEN_EC */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_ec_params

DESCRIPTION
  Get the echo-cancellation parameters.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_ec_params_type* - pointer to current echo cancellor parameters.

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_ec_params_type * voc_data_get_ec_params
(
  voc_ec_type ec_mode
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    return( voc_data_pcm_cal_ptr->ec_params[ec_mode] );
  }
} /* voc_data_get_ec_mode */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_ns_mode

DESCRIPTION
  Get the noise suppression mode.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  voc_ns_type - pointer to the current noise suppressor mode.

SIDE EFFECTS
  ns_mode_ptr

===========================================================================*/
voc_ns_type* voc_data_get_ns_mode
(
  void
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    static voc_ns_type override_mode = VOC_NS_OFF;

    if(voc_data_ns_override_mode == VOC_EC_NS_FORCE_DISABLED) {
      return( &override_mode );
    } else {
      return( &voc_data_pcm_cal_ptr->ns_enable );
    }
  }
} /* voc_data_get_ns_mode */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_hssd_mode

DESCRIPTION
  Get the HSSD circuit mode.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  voc_ns_type - pointer to the current noise suppressor mode.

SIDE EFFECTS
  ns_mode_ptr

===========================================================================*/
voc_hssd_type* voc_data_get_hssd_mode
(
  void
)
{
  return( &voc_data_pcm_cal_ptr->hssd );
} /* voc_data_get_hssd_mode */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_vox_mode

DESCRIPTION
  Get the VOX (half/full duplex voice) mode.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  voc_vox_type - pointer to the current vox mode.

SIDE EFFECTS
  None.

===========================================================================*/
voc_vox_type* voc_data_get_vox_mode
(
  void
)
{
  static voc_vox_type vox_mode = VOC_VOX_OFF;

  if(voc_data_vox_override_mode == VOC_VOX_FORCE_ENABLED) {
    vox_mode = VOC_VOX_ON;
    return( &vox_mode );
  } else {
    vox_mode = VOC_VOX_OFF;
    return( &vox_mode );
  }
} /* voc_data_get_vox_mode */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_sidetone_mode

DESCRIPTION
  Get the sidetone mode.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  voc_sidetone_type - pointer to the current vox mode.

SIDE EFFECTS
  None.

===========================================================================*/
voc_sidetone_type* voc_data_get_sidetone_mode
(
  void
)
{
  static voc_sidetone_type sidetone_mode = VOC_SIDETONE_ON;

  if(voc_data_sidetone_override_mode == VOC_SIDETONE_FORCE_DISABLED) {
    sidetone_mode = VOC_SIDETONE_OFF;
    return( &sidetone_mode );
  } else {
    sidetone_mode = VOC_SIDETONE_ON;
    return( &sidetone_mode );
  }
} /* voc_data_get_sidetone_mode */

/* <EJECT> */


/*===========================================================================

FUNCTION voc_data_get_dtmf_tx_gain

DESCRIPTION
  Get the Transmit Gain for FM DTMF.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  uint16 - current DTMF Tx gain.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 voc_data_get_dtmf_tx_gain
(
  void
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    return( voc_data_pcm_cal_ptr->dtmf_tx_gain );
  }
} /* voc_data_get_dtmf_tx_gain */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_dtmf_rx_gain

DESCRIPTION
  Get the Receive Gain for FM DTMF.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  uint16 - Hardware DTMF Rx gain.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 voc_data_get_dtmf_rx_gain
(
  sint15 vol
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    return(voc_data_compute_dbm( VOC_CAL_DB0_DTMF_GAIN , vol ));
  }
} /* voc_data_get_dtmf_rx_gain */

/* <EJECT> */
#if defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT) || \
    defined(FEATURE_QTUNES_COMMON) || defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION voc_data_get_mm_rx_gain

DESCRIPTION
  Get the Receive Gain for multimedia output.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  uint16 - Hardware Rx gain.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 voc_data_get_mm_rx_gain
(
  sint15 vol
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    voc_state_control_type *state;
    uint16                 ret_val = 0;

    state = voc_state_get_state_data ();

    switch (state->current_image) {
#ifdef FEATURE_QSYNTH_COMMON
#ifdef FEATURE_AVS_INCALL_MIXER_CMX
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
      case QDSP_IMAGE_VOC_COMBO_SYNTH:
        if ((state->state == VOC_STATE_MIXER_ACTIVE_ENTRY) ||
            (state->state == VOC_STATE_MIXER_ACTIVE)) {
         ret_val = voc_data_compute_dbm(VOC_CAL_DB0_QSYNTH_GAIN , vol);

        }
        break;
#endif /* QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */
#endif /* FEATURE_AVS_INCALL_MIXER_CMX */
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
        ret_val = voc_data_compute_dbm( VOC_CAL_DB0_QSYNTH_GAIN , vol );
        break;
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QTUNES_COMMON
#ifdef QDSP_IMAGE_TUNES_DEFINED
      case QDSP_IMAGE_TUNES:
#endif /* QDSP_IMAGE_TUNES_DEFINED */
#ifdef QDSP_IMAGE_MP4D_MP3_DEFINED
      case QDSP_IMAGE_MP4D_MP3:
#endif /* QDSP_IMAGE_MP4D_MP3_DEFINED */
#ifdef QDSP_IMAGE_AAC_DEFINED
      case QDSP_IMAGE_AAC:
#endif /* QDSP_IMAGE_AAC_DEFINED */
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
      case QDSP_IMAGE_ENH_AAC_PLUS:
#endif /* QDSP_IMAGE_ENH_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED
      case QDSP_IMAGE_MP4D_AAC_PLUS:
#endif /* QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED
      case QDSP_IMAGE_MP4D_AAC_NO_VPP:
#endif
#ifdef QDSP_IMAGE_MP4D_AAC_DEFINED
      case QDSP_IMAGE_MP4D_AAC:
#endif /* QDSP_IMAGE_MP4D_AAC_DEFINED */
#ifdef QDSP_IMAGE_H263D_MP3_DEFINED
      case QDSP_IMAGE_H263D_MP3:
#endif /* QDSP_IMAGE_H263D_MP3_DEFINED */
#ifdef QDSP_IMAGE_H263D_AAC_DEFINED
      case QDSP_IMAGE_H263D_AAC:
#endif /* QDSP_IMAGE_H263D_AAC_DEFINED */
#ifdef QDSP_IMAGE_H264D_AAC_DEFINED
      case QDSP_IMAGE_H264D_AAC:
#endif /* QDSP_IMAGE_H264D_AAC_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
      case QDSP_IMAGE_QTV_AAC:
#endif /* QDSP_IMAGE_QTV_AAC_DEFINED */
#ifdef QDSP_IMAGE_RA_DEFINED
      case QDSP_IMAGE_RA:
#endif /* QDSP_IMAGE_RA_DEFINED */
#ifdef QDSP_IMAGE_WM_DEFINED
      case QDSP_IMAGE_WM:
#endif /* QDSP_IMAGE_WM_DEFINED */
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_WM_PRO_DEFINED */
#ifdef QDSP_IMAGE_QTV_CONC_DEFINED
      case QDSP_IMAGE_QTV_CONC:
#endif /* QDSP_IMAGE_QTV_CONC_DEFINED */
        ret_val = voc_data_compute_dbm(VOC_CAL_DB0_QTUNES_GAIN , vol);
        break;

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
#ifdef QDSP_IMAGE_HR_MP3_DEFINED
      case QDSP_IMAGE_HR_MP3:
#endif /* QDSP_IMAGE_HR_MP3_DEFINED */
#ifdef QDSP_IMAGE_EFR_MP3_DEFINED
      case QDSP_IMAGE_EFR_MP3:
#endif /* QDSP_IMAGE_EFR_MP3_DEFINED */
#ifdef QDSP_IMAGE_FR_MP3_DEFINED
      case QDSP_IMAGE_FR_MP3:
#endif /* QDSP_IMAGE_FR_MP3_DEFINED */
        if ((state->state == VOC_STATE_ADEC_VOCODER_ENTRY) ||
            (state->state == VOC_STATE_ADEC_VOCODER)) {
         ret_val = voc_data_compute_dbm(VOC_CAL_DB0_QTUNES_GAIN , vol);
        }
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
#ifdef QDSP_IMAGE_VOC_COMBO_ICA_DEFINED
      case QDSP_IMAGE_VOC_COMBO_ICA:
        if ((state->state == VOC_STATE_MIXER_ACTIVE_ENTRY) ||
            (state->state == VOC_STATE_MIXER_ACTIVE)) {
         ret_val = voc_data_compute_dbm(VOC_CAL_DB0_QTUNES_GAIN , vol);
        }
        break;
#endif /* QDSP_IMAGE_VOC_COMBO_ICA_DEFINED */
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#endif /* FEATURE_QTUNES_COMMON */
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
      case  QDSP_IMAGE_CAM_AUDIO_CONC:
        if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_MP3 ){
          ret_val = voc_data_compute_dbm( VOC_CAL_DB0_QTUNES_GAIN , vol );
         } else if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_WAVE){
           ret_val = voc_data_compute_dbm( VOC_CAL_DB0_QSYNTH_GAIN , vol );
         }
        break;
#endif
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
      default:
        break;
    }

    return(ret_val);
  }
} /* voc_data_get_mm_rx_gain */
#endif /* FEATURE_MIDI || FEATURE_MIDI_OUT || FEATURE_QTUNES_COMMON ||
          FEATURE_QSYNTH_ADPCM */

/* <EJECT> */

/*===========================================================================
FUNCTION voc_data_get_pre_ec_tx_pcm_filter

DESCRIPTION
  Gets the Pre EC Tx PCM filter flag.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_filter_type* - pointer to the current Rx PCM filter
                              coefficients.
SIDE EFFECTS
  None.
===========================================================================*/
extern uint16* voc_data_get_pre_ec_tx_pcm_filter( void )
{
  return( &voc_data_pcm_cal_ptr->pre_ec_tx_pcm_filter_flag );
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_rx_filter

DESCRIPTION
  Gets the Rx PCM filter coefficients.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_filter_type* - pointer to the current Rx PCM filter
                              coefficients.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_pcm_filter_type* voc_data_get_rx_filter
(
  void
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    if(voc_data_aud_process_override_enabled) {
      return( (qdsp_cmd_pcm_filter_type*) &voc_data_aud_override_filter );
    } else {
      return( &voc_data_pcm_cal_ptr->rx_filter );
    }
  }
} /* voc_data_get_rx_filter */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_tx_filter

DESCRIPTION
  Gets the Tx PCM filter coefficients

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_filter_type* - pointer to the current Tx PCM filter
                              coefficients.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_pcm_filter_type* voc_data_get_tx_filter
(
  void
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    if(voc_data_aud_process_override_enabled) {
      return( (qdsp_cmd_pcm_filter_type*) &voc_data_aud_override_filter );
    } else {
      return( &voc_data_pcm_cal_ptr->tx_filter );
    }
  }
} /* voc_data_get_tx_filter */

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/*===========================================================================

FUNCTION voc_data_get_tx_second_mic_filter

DESCRIPTION
  Gets the Tx PCM SecondaryMic filter coefficients

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_filter_type* - pointer to the current Tx PCM filter
                              coefficients.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_pcm_filter_type* voc_data_get_tx_second_mic_filter
(
  void
)
{
  if(voc_data_aud_process_override_enabled) {
    return( (qdsp_cmd_pcm_filter_type*) &voc_data_aud_override_filter );
  }else {
    return( &voc_data_pcm_cal_ptr->tx_secondary_mic_filter );
  }
}/*voc_data_get_tx_second_mic_filter*/
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */

#ifdef FEATURE_IIR_FILTER
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_rx_iir_filter

DESCRIPTION
  Gets the Rx PCM IIR filter coefficients.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_iir_filter_type* - pointer to the current Rx PCM filter
                                  coefficients.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_pcm_iir_filter_type* voc_data_get_rx_iir_filter
(
  void
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    if(voc_data_aud_process_override_enabled)
    {
      return( (qdsp_cmd_pcm_iir_filter_type*)
              &voc_data_aud_override_iir_filter );
    }
    else
    {
#ifdef FEATURE_IIR_FILTER_5S
      if((voc_data_pcm_cal_ptr->rx_iir_filter5).flag ==
         QDSP_CMD_IIR_FILTER_ENABLE)
      {
        return( &voc_data_pcm_cal_ptr->rx_iir_filter5 );
      }
#endif
      if((voc_data_pcm_cal_ptr->rx_iir_filter).flag ==
         QDSP_CMD_IIR_FILTER_ENABLE)
        return( &voc_data_pcm_cal_ptr->rx_iir_filter );
      else
        return NULL;
    }
  }
} /* voc_data_get_rx_iir_filter */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_tx_iir_filter

DESCRIPTION
  Gets the Tx PCM IIR filter coefficients.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_iir_filter_type* - pointer to the current Tx PCM filter
                                  coefficients.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_pcm_iir_filter_type* voc_data_get_tx_iir_filter
(
  void
)
{
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    if(voc_data_aud_process_override_enabled)
    {
      return( (qdsp_cmd_pcm_iir_filter_type*)
              &voc_data_aud_override_iir_filter );
    }
    else
    {      
#ifdef FEATURE_IIR_FILTER_5S
      if((voc_data_pcm_cal_ptr->tx_iir_filter5).flag ==
         QDSP_CMD_IIR_FILTER_ENABLE)        
        return( &voc_data_pcm_cal_ptr->tx_iir_filter5 );
#endif
      if((voc_data_pcm_cal_ptr->tx_iir_filter).flag ==
         QDSP_CMD_IIR_FILTER_ENABLE)
        return( &voc_data_pcm_cal_ptr->tx_iir_filter );
      
      else
        return NULL;
    }
  }
} /* voc_data_get_tx_iir_filter */

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_tx_second_mic_iir_filter

DESCRIPTION
  Gets the Tx SecondaryMic PCM IIR filter coefficients.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_iir_filter_type* - pointer to the current Tx PCM filter
                                  coefficients.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_pcm_iir_filter_type* voc_data_get_tx_second_mic_iir_filter
(
  void
)
{
   if(voc_data_aud_process_override_enabled)
   {
     return( (qdsp_cmd_pcm_iir_filter_type*)
             &voc_data_aud_override_second_mic_iir_filter );
   }
   else
   {
#ifdef FEATURE_IIR_FILTER_5S
     if((voc_data_pcm_cal_ptr->tx_secondary_mic_iir_filter5).flag
        == QDSP_CMD_IIR_FILTER_ENABLE)
       return( &voc_data_pcm_cal_ptr->tx_secondary_mic_iir_filter5 );
#endif
     return( NULL );

   }
}/*voc_data_get_tx_second_mic_iir_filter*/
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */

#endif /* FEATURE_IIR_FILTER */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_dfm_sat_dcc

DESCRIPTION
  Sets the SAT DCC value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_sat_dcc

===========================================================================*/
void voc_data_set_dfm_sat_dcc
(
   voc_scc_type sat_dcc
   /* New SAT DCC value */
)
{
   voc_data_dfm_sat_dcc = sat_dcc;
} /* voc_data_set_dfm_sat_dcc */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_dfm_sat_dcc

DESCRIPTION
  Gets the current SAT DCC value.

DEPENDENCIES
  A SAT Message Interrupt must be received, and the voc_sat_dcc_cb()
  callback function called, before valid SAT DCC values are available.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_sat_dcc will be updated.

===========================================================================*/
voc_scc_type voc_data_get_dfm_sat_dcc
(
  void
)
{
   return( voc_data_dfm_sat_dcc );
} /* voc_data_get_dfm_sat_dcc */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_compute_dbm_from_voc

DESCRIPTION
  This function returns the ddbm value from hardware register value for a given
  nominal value (0 db value).

DEPENDENCIES
  Requires voc_data_dbm_table[].

RETURN VALUE
  Returns a word containing the ddbm value.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 voc_data_compute_dbm_from_voc
(
  uint16  nominal_val,
    /* Value of the hardware register for a 0 DBM       */
  uint16  voc_value
) {
  const uint32 *ptable;
  uint32 tlong, diff;
  sint31 calc_factor;
  sint15 counter;

  tlong = (voc_value * 10000) / nominal_val;

  counter = VOC_DBM_MIN;
  ptable  = voc_data_dbm_table;

  while((tlong > *ptable) && (counter < VOC_DBM_MAX)) {
    counter += VOC_DBM_STEP;
    ptable++;
  }

  if((tlong != *ptable) && (counter != VOC_DBM_MIN) &&
     (counter != VOC_DBM_MAX)) {
    diff         = *ptable - *(ptable - 1);
    calc_factor  = *ptable - tlong;
    calc_factor  = (calc_factor * 100) / diff;
    counter     -= calc_factor;
  }

  return(counter);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_compute_dbm

DESCRIPTION
  This function returns the hardware register value for a given ddbm and
  a nominal value (0 db value).

DEPENDENCIES
  Requires voc_data_dbm_table[].

RETURN VALUE
  Returns a word containing the hardware register value.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 voc_data_compute_dbm
(
  uint16  nominal_val,
    /* Value of the hardware register for a 0 DBM       */
  sint15  dbm
    /* Value in dbm for which values are to be computed */
)
{
  int    i;                  /* Temporary counter                     */
  uint32 reg_val = 0;        /* Computed result for correspondent dbm */
  sint15 tlower_dbm;         /* Closest known dbm to given dbm        */
  sint15 tindex;             /* Index into precomputed values table   */
  sint31 xdiff, ydiff;       /* Intermediates needed for computation  */
  static byte last_entry = 0;/* Last used entry from cache            */
  boolean match;             /* Tell if there is a match or not       */
  uint32 tlong;

  static sint15 dbm_cache[4] = {-6000,-6000,-6000,-6000};/* Last dbm  */
  static uint16 nom_cache[4] = {0,0,0,0};       /* Last nominal value */
  static uint16 val_cache[4] = {0,0,0,0};       /* Hw Reg Value cache */

  /* Check if the given dbm is out of range for the vocoder           */
  if ( dbm < VOC_DBM_MIN ) {
    ERR("Dbm: %d -- Min Dbm is %d dbm",dbm,VOC_DBM_MIN,0);
    return (VOC_MIN_GAIN);    /* If dbm is too low return 0 value   */
  } else if ( dbm == VOC_DBM_MIN ) {
    return (VOC_MIN_GAIN);  /* If dbm is the minimum return 0 value */
  } else if ( dbm > VOC_DBM_MAX ) {
    ERR("Dbm: %d -- Max Dbm is %d dbm",dbm,VOC_DBM_MAX,0);
    dbm = VOC_DBM_MAX;       /* If dbm is too high then set to max    */
  }
  match = FALSE;

  for (i=0; i <= 3; i++) {
    /* Check the cache to see if dbm was recently used                */
    if (dbm == dbm_cache[i] && nominal_val == nom_cache[i]) {
      reg_val = val_cache[i];
      match = TRUE;         /* Found previous cache match             */
      break;
    }
  }
  if (!match) {
    /* Value was not cached, therefore, compute from table. */
    /* Find the closest (lower) adjacent dbm from the known values    */
    tindex = (dbm - VOC_DBM_MIN) / VOC_DBM_STEP;
    tlower_dbm =(dbm % (VOC_DBM_STEP));

    if (tlower_dbm < 0) {
      tlower_dbm = dbm - tlower_dbm - VOC_DBM_STEP;
    } else {
      tlower_dbm = dbm - tlower_dbm;
    }
    xdiff   = dbm - tlower_dbm;
    reg_val = voc_data_dbm_table[tindex];

    if (xdiff != 0 ) {
      /* For all other values compute value by interpolation               */
      /*lint -e661 : tindex+1 will never go out of audio_db_table          */
      ydiff   = (sint31)(voc_data_dbm_table[tindex+1] - reg_val);
      /*lint +e661 */
      reg_val = reg_val+(uint32)((ydiff*xdiff)/VOC_DBM_STEP);
    }

    /* Update the cache since a value had to be computed                    */
    dbm_cache[last_entry] = dbm;         /* Update the last computed dbm    */
    nom_cache[last_entry] = nominal_val; /* Update the last computed nom val*/

    tlong = 655350000;
    tlong = tlong/nominal_val;
    if(reg_val < tlong) {
      reg_val = (reg_val * nominal_val) / 10000;
    } else {
      ERR("Overflow dbm= %d, value= %x",dbm,nominal_val,0);
      reg_val = VOC_MAX_GAIN;          /* Set to maximum value            */
    }

    val_cache[last_entry] = (word) reg_val;     /* Update the reg value     */
    last_entry = (last_entry + 1) & 0x3; /* last entry limited from 0 to 3  */
  }
  MSG_LOW("Dbm:%d reg value:%x",dbm,reg_val,0);
  return ((uint16)reg_val);
} /* end of voc_data_compute_dbm */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_adjust_dbm

DESCRIPTION
  This function returns the hardware register value for a given ddbm offset
  applied to a given hardware register value using a nominal value
  (0 db value).

DEPENDENCIES
  Requires voc_data_dbm_table[].

RETURN VALUE
  Returns a word containing the adjusted hardware register value.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 voc_data_adjust_dbm
(
  uint16  nominal_val,
    /* Value of the hardware register for a 0 DBM       */
  uint16  hardware_val,
    /* Value for which offset is to be applied          */
  sint15  dbm_offset
    /* Offset in dbm to be applied to hardware_val      */
)
{
  sint15 tindex  = 0;             /* Index into precomputed values table   */
  sint15 dbm_val = 0;             /* dbm value of supplied hardware_val    */
  sint15 last_tindex;             /* Table search boundary (last searched) */
  sint15 temp_tindex = 0;         /* Temporary table index                 */
  uint32 reg_val;                 /* High-precision log value              */
  uint32 xdiff, ydiff;
  uint16 max_entries;             /* Upper bound of table array            */
  uint16 retval = 0;
  boolean found = FALSE;

  /* Figure out max table size.
  */
  max_entries = (sizeof(voc_data_dbm_table)/sizeof(uint32)) - 1;

  last_tindex = max_entries;

  /* Increase presicion of hardware_val.
  */
  reg_val = 10000 * hardware_val;

  /* Convert reg_val into non-normalized log equivalent (for comparison
  ** with values in the precomputed table).
  */
  reg_val = reg_val / (uint32)nominal_val;

  if (reg_val > voc_data_dbm_table[max_entries])
  {
    ERR("Out of range: val=%x, max=%x",
      reg_val,voc_data_dbm_table[max_entries],0);
    reg_val = voc_data_dbm_table[max_entries];
  }

  if (reg_val < voc_data_dbm_table[0])
  {
    ERR("Out of range: val=%x, min=%x",
      reg_val,voc_data_dbm_table[0],0);
    reg_val = voc_data_dbm_table[0];
  }

  /* Do table search.
  */
  while(!found) {
    if(reg_val >= voc_data_dbm_table[tindex]) {
      if(reg_val < voc_data_dbm_table[tindex+1]) {
        found = TRUE;
      } else {
        if(tindex == last_tindex) {
          tindex++;
        } else {
          temp_tindex = tindex;
          tindex += VOCABS(last_tindex - tindex) / 2;
          last_tindex = temp_tindex;
        }
      }
    } else {
      if(tindex == last_tindex) {
        tindex--;
      } else {
        temp_tindex = tindex;
        tindex -= VOCABS(last_tindex - tindex) / 2;
        last_tindex = temp_tindex;
      }
    }
  }

  /* Convert tindex, which is dbm value now, into snddata equivalent
  ** format (dbm * 100).
  */
  dbm_val = ((sint15)tindex * VOC_DBM_STEP) + (VOC_DBM_MIN);

  /* Perform linear interpolation, if necessary.
  */
  if (voc_data_dbm_table[tindex] != reg_val) {
    /* Increase precision of xdiff for division operation.        */
    /*     |-------|                                              */
    xdiff = 10000 * (reg_val - voc_data_dbm_table[tindex]);
    ydiff = voc_data_dbm_table[tindex+1] - voc_data_dbm_table[tindex];
    dbm_val += (sint15)(((xdiff / ydiff) * VOC_DBM_STEP) / 10000);
    /*                                                  |-------| */
    /* Remove increase in precision.                              */
  }

  /* Apply desired offset.
  */
  dbm_val += dbm_offset;

  /* Convert back to hardware register format.
  */
  retval = voc_data_compute_dbm(nominal_val, dbm_val);

  return (retval);

} /* end of voc_data_adjust_dbm */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_diags

DESCRIPTION
  Get the Vocoder diagnostics data

DEPENDENCIES
  voc_data_diags

RETURN VALUE
  Pointer to qdsp_cmd_diag_type

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_diag_type* voc_data_get_diags
(
  void
)
{
/* Vocoder diagnostic data
*/
  static  qdsp_cmd_diag_type voc_data_diags;

  return (&voc_data_diags);
} /* voc_data_get_diags */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_aud_process_override

DESCRIPTION
  Set the current audio process override value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_aud_process_override_enabled

===========================================================================*/
void voc_data_set_aud_process_override
(
  boolean aud_process_override
) {
  voc_data_aud_process_override_enabled = aud_process_override;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_aud_process_override

DESCRIPTION
  Get the current audio process override value.

DEPENDENCIES
  None.

RETURN VALUE
  voc_data_aud_process_override_enabled

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_data_get_aud_process_override
(
  void
) {
  return(voc_data_aud_process_override_enabled);
}

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

FUNCTION voc_data_set_vr_mode

DESCRIPTION
  Save the VR mode.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  voc_data_vr_mode

===========================================================================*/
void voc_data_set_vr_mode (
  voc_vr_mode_type  vr_mode
)
{
  /* Save the vr mode
  */
  voc_data_vr_mode = vr_mode;

} /* voc_data_set_vr_mode */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_vr_cal

DESCRIPTION
  Save the VR calibration data for the current PCM path.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  voc_data_vr_cal_ptr

===========================================================================*/
void voc_data_set_vr_cal (
  voc_vr_cal_type *vr_cal_ptr
)
{
  ASSERT( vr_cal_ptr != NULL );

  /* Save a pointer to the calibration information
  */
  voc_data_vr_cal_ptr = vr_cal_ptr;

} /* voc_data_set_vr_cal */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_vr_filter

DESCRIPTION
  Gets the Tx PCM filter coefficients to use during VR capture.

DEPENDENCIES
  voc_data_set_vr_cal() must be called prior.

RETURN VALUE
  qdsp_cmd_pcm_filter_type* - pointer to the current Tx PCM filter
                              coefficients. Will be NULL if there
                              is not filter override for capture.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_pcm_filter_type* voc_data_get_vr_filter (
  void
)
{
  static qdsp_cmd_pcm_filter_type pcm_filter;

  ASSERT( voc_data_vr_cal_ptr != NULL );

  if (voc_data_vr_cal_ptr->tx_filter.tap6 != 0) {
    pcm_filter.tap0 = voc_data_vr_cal_ptr->tx_filter.tap0;
    pcm_filter.tap1 = voc_data_vr_cal_ptr->tx_filter.tap1;
    pcm_filter.tap2 = voc_data_vr_cal_ptr->tx_filter.tap2;
    pcm_filter.tap3 = voc_data_vr_cal_ptr->tx_filter.tap3;
    pcm_filter.tap4 = voc_data_vr_cal_ptr->tx_filter.tap4;
    pcm_filter.tap5 = voc_data_vr_cal_ptr->tx_filter.tap5;
    pcm_filter.tap6 = voc_data_vr_cal_ptr->tx_filter.tap6;
    return( &pcm_filter );
  } else {
    return( NULL );
  }
} /* voc_data_get_vr_filter */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_vr_ec

DESCRIPTION
  Get the echo-cancellation mode to use during VR capture.

DEPENDENCIES
  voc_data_set_vr_cal() must be called prior.

RETURN VALUE
  voc_ec_type* - pointer to current echo cancellor mode.
                 NULL if there is no override.

SIDE EFFECTS
  None.

===========================================================================*/
voc_ec_type* voc_data_get_vr_ec (
  void
)
{
  ASSERT( voc_data_vr_cal_ptr != NULL );

  if (voc_data_vr_cal_ptr->ec_mode != VOC_EC_DEFAULT) {
    return( &voc_data_vr_cal_ptr->ec_mode );
  } else {
    return( NULL );
  }
} /* voc_data_get_vr_ec */
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_vr_ns

DESCRIPTION
  Gets the noise suppression mode to be used during VR capture.

DEPENDENCIES
  voc_data_set_vr_cal() must be called prior.

RETURN VALUE
  voc_ns_type - pointer to the current noise suppressor mode.
                 NULL if there is no override.

SIDE EFFECTS
  ns_mode_ptr

===========================================================================*/
voc_ns_type* voc_data_get_vr_ns (
  void
)
{
  ASSERT( voc_data_vr_cal_ptr != NULL );

  if (voc_data_vr_cal_ptr->ns_enable != VOC_NS_DEFAULT) {
    return( &voc_data_pcm_cal_ptr->ns_enable );
  } else {
    return( NULL );
  }
} /* voc_data_get_vr_ns */
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_sample_rate

DESCRIPTION
  Set the current SDAC sample rate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_sample_rate

===========================================================================*/
void voc_data_set_sample_rate
(
  voc_sample_rate_type rate  /* Sample rate for programmable DAC */
)
{
  voc_data_sample_rate = rate;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_sample_rate

DESCRIPTION
  Get the current SDAC sample rate.

DEPENDENCIES
  voc_data_sample_rate

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
voc_sample_rate_type voc_data_get_sample_rate
(
  void
)
{
  voc_sample_rate_type ret_val = voc_data_sample_rate;

  voc_state_control_type *state;

  state = voc_state_get_state_data();

  switch (state->current_image) {
#ifdef FEATURE_QSYNTH_COMMON
#ifdef QDSP_IMAGE_SYNTH_DEFINED
    case QDSP_IMAGE_SYNTH:
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif
		
#ifdef FEATURE_GASYNTH_INTERFACE
#ifdef FEATURE_STD_MIDI
        ret_val = VOC_SAMPLE_RATE_32000;
#else
#ifdef FEATURE_AUDIO_QSYNTH_48K_SUPPORT
      ret_val = VOC_SAMPLE_RATE_48000;
#else
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
      ret_val = VOC_SAMPLE_RATE_44000;
#else
      ret_val = VOC_SAMPLE_RATE_44100;
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
#endif /* FEATURE_AUDIO_QSYNTH_48K_SUPPORT */
#endif /*FEATURE_STD_MIDI*/
#else
#ifdef FEATURE_WEBAUDIO
      if(state->hybrid_mode_pending != 0) {
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
        ret_val = VOC_SAMPLE_RATE_44000;
#else
        ret_val = VOC_SAMPLE_RATE_44100;
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
      } else
#endif /* FEATURE_WEBAUDIO */
      {
        ret_val = VOC_SAMPLE_RATE_32000;
      }
#endif /* FEATURE_GASYNTH_INTERFACE */
      break;
#endif /* QDSP_IMAGE_SYNTH_DEFINED */

#ifdef QDSP_IMAGE_Q3D_DEFINED
    case QDSP_IMAGE_Q3D:
      ret_val = VOC_SAMPLE_RATE_32000;
      break;
#endif /* QDSP_IMAGE_Q3D_DEFINED */

#ifdef QDSP_IMAGE_QGL_DEFINED
    case QDSP_IMAGE_QGL:
      ret_val = VOC_SAMPLE_RATE_32000;
      break;
#endif /* QDSP_IMAGE_QGL_DEFINED */
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QTUNES_COMMON
#if defined(QDSP_IMAGE_AAC_DEFINED)             || \
    defined(QDSP_IMAGE_ENH_AAC_PLUS_DEFINED)    || \
    defined(QDSP_IMAGE_H264D_AAC_DEFINED)       || \
    defined(QDSP_IMAGE_QTV_AAC_H264_DEFINED)    || \
    defined(QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED)|| \
    defined(QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED)   || \
    defined(QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED) || \
    defined(QDSP_IMAGE_QTV_AAC_DEFINED)
#ifdef QDSP_IMAGE_AAC_DEFINED
    case QDSP_IMAGE_AAC:
#endif /* QDSP_IMAGE_AAC_DEFINED */
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
    case QDSP_IMAGE_ENH_AAC_PLUS:
#endif /* QDSP_IMAGE_ENH_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED
    case QDSP_IMAGE_MP4D_AAC_PLUS:
#endif /* QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED
    case QDSP_IMAGE_MP4D_AAC_NO_VPP:
#endif
#ifdef QDSP_IMAGE_H264D_AAC_DEFINED
    case QDSP_IMAGE_H264D_AAC:
#endif /* QDSP_IMAGE_H264D_AAC_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
    case QDSP_IMAGE_QTV_AAC:
#endif /* QDSP_IMAGE_QTV_AAC_DEFINED */
      if (voc_data_sample_rate <= VOC_SAMPLE_RATE_24000) {
        /* Double the sample rate for AAC+ bitstream */
	switch (voc_data_sample_rate) {
          case VOC_SAMPLE_RATE_8000:
            ret_val = VOC_SAMPLE_RATE_16000;
            break;
          case VOC_SAMPLE_RATE_11025:
            ret_val = VOC_SAMPLE_RATE_22050;
            break;
          case VOC_SAMPLE_RATE_12000:
            ret_val = VOC_SAMPLE_RATE_24000;
            break;
          case VOC_SAMPLE_RATE_16000:
            ret_val = VOC_SAMPLE_RATE_32000;
            break;
          case VOC_SAMPLE_RATE_22050:
            ret_val = VOC_SAMPLE_RATE_44100;
            break;
          case VOC_SAMPLE_RATE_24000:
            ret_val = VOC_SAMPLE_RATE_48000;
            break;
        }
      }
#endif /* QDSP_IMAGE_AAC_DEFINED             ||
          QDSP_IMAGE_ENH_AAC_PLUS_DEFINED    ||
          QDSP_IMAGE_H264D_AAC_DEFINED       ||
          QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED   ||
          QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED ||
          QDSP_IMAGE_QTV_AAC_DEFINED */
#ifdef QDSP_IMAGE_TUNES_DEFINED
    case QDSP_IMAGE_TUNES:
#endif /* QDSP_IMAGE_TUNES_DEFINED */
#ifdef QDSP_IMAGE_MP4D_MP3_DEFINED
    case QDSP_IMAGE_MP4D_MP3:
#endif /* QDSP_IMAGE_MP4D_MP3_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_DEFINED
    case QDSP_IMAGE_MP4D_AAC:
#endif /* QDSP_IMAGE_MP4D_AAC_DEFINED */
#ifdef QDSP_IMAGE_H263D_MP3_DEFINED
    case QDSP_IMAGE_H263D_MP3:
#endif /* QDSP_IMAGE_H263D_MP3_DEFINED */
#ifdef QDSP_IMAGE_H263D_AAC_DEFINED
    case QDSP_IMAGE_H263D_AAC:
#endif /* QDSP_IMAGE_H263D_AAC_DEFINED */
#ifdef QDSP_IMAGE_RA_DEFINED
    case QDSP_IMAGE_RA:
#endif /* QDSP_IMAGE_RA_DEFINED */
#ifdef QDSP_IMAGE_WM_DEFINED
    case QDSP_IMAGE_WM:
#endif /* QDSP_IMAGE_WM_DEFINED */
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_WM_PRO_DEFINED */
#if defined(QDSP_IMAGE_QTV_CONC_DEFINED)
    case QDSP_IMAGE_QTV_CONC:
#endif /* QDSP_IMAGE_QTV_CONC_DEFINED */
#ifdef QDSP_IMAGE_VOC_COMBO_ICA_DEFINED
    case QDSP_IMAGE_VOC_COMBO_ICA:
#endif /* QDSP_IMAGE_VOC_COMBO_ICA_DEFINED */
      break;
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_GRAPH_AUDIO
#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
    case QDSP_IMAGE_VFE_BAYER:
      ret_val = VOC_SAMPLE_RATE_32000;
      break;
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED */

#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
    case QDSP_IMAGE_VFE_YCBCR:
      ret_val = VOC_SAMPLE_RATE_32000;
      break;
#endif /* QDSP_IMAGE_VFE_YCBCR_DEFINED */

#ifdef QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED
    case QDSP_IMAGE_QCAMCORDER_AACENC:
      break;
#endif /* QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED */

#ifdef QDSP_IMAGE_AACENC_DEFINED
    case QDSP_IMAGE_AACENC:
      break;
#endif /* QDSP_IMAGE_AACENC_DEFINED */

#endif /* FEATURE_GRAPH_AUDIO */
#ifdef FEATURE_AVS_I2SSBC
    case QDSP_IMAGE_I2SSBC:
      break;
#endif /* FEATURE_AVS_I2SSBC */
#if defined(MSMAUD_VOC_AMR_WB_COMMON)
    case QDSP_IMAGE_AMR_WB:
      ret_val = VOC_SAMPLE_RATE_16000;
      break;
#endif /* defined(MSMAUD_VOC_AMR_WB_COMMON) */
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
    case QDSP_IMAGE_VOC_COMBO_SYNTH:
      ret_val = VOC_SAMPLE_RATE_48000;
      break;
#endif /* QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
    case QDSP_IMAGE_CAM_AUDIO_CONC:
      if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_MP3 ){
        ret_val = voc_data_sample_rate;
      }else if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_WAVE ){
        ret_val = VOC_SAMPLE_RATE_32000;
      }else{
        ret_val = VOC_SAMPLE_RATE_8000;
      }
      break;
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
    default:
      ret_val = VOC_SAMPLE_RATE_8000;
      break;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_dtmf_duration

DESCRIPTION
  Calcuate the DTMF dtmf duration.

DEPENDENCIES
  None.

RETURN VALUE
  DTMF duration

SIDE EFFECTS
  None.

===========================================================================*/
uint16 voc_data_get_dtmf_duration
(
  uint16 duration_ms
)
{
  uint16 ret_val = duration_ms / 5;

#if defined(FEATURE_QSYNTH_COMMON) || defined(FEATURE_QTUNES_COMMON) || \
    defined(FEATURE_GRAPH_AUDIO) || defined(FEATURE_AVS_I2SSBC)
  voc_state_control_type *state;

  state = voc_state_get_state_data();

  switch (state->current_image) {
#ifdef FEATURE_QSYNTH_COMMON
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
    case QDSP_IMAGE_VOC_COMBO_SYNTH:
    #ifdef FEATURE_WEBAUDIO
      ret_val = duration_ms / 10;
#else
      ret_val = duration_ms / 8;
#endif /* FEATURE_WEBAUDIO */
      break;
#endif /* QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_SYNTH_DEFINED
    case QDSP_IMAGE_SYNTH:
#ifdef FEATURE_WEBAUDIO
      ret_val = duration_ms / 10;
#else
      ret_val = duration_ms / 8;
#endif /* FEATURE_WEBAUDIO */
      break;
#endif /* QDSP_IMAGE_SYNTH_DEFINED */

#ifdef QDSP_IMAGE_Q3D_DEFINED
    case QDSP_IMAGE_Q3D:
      ret_val = duration_ms / 8;
      break;
#endif /* QDSP_IMAGE_Q3D_DEFINED */

#ifdef QDSP_IMAGE_QGL_DEFINED
    case QDSP_IMAGE_QGL:
      ret_val = duration_ms / 8;
      break;
#endif /* QDSP_IMAGE_QGL_DEFINED */
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QTUNES_COMMON
#ifdef FEATURE_QTUNES
#ifdef QDSP_IMAGE_TUNES_DEFINED
    case QDSP_IMAGE_TUNES:
#endif /* QDSP_IMAGE_TUNES_DEFINED */
#ifdef QDSP_IMAGE_MP4D_MP3_DEFINED
    case QDSP_IMAGE_MP4D_MP3:
#endif /* QDSP_IMAGE_MP4D_MP3_DEFINED */
#ifdef QDSP_IMAGE_H263D_MP3_DEFINED
    case QDSP_IMAGE_H263D_MP3:
#endif /* QDSP_IMAGE_H263D_MP3_DEFINED */
      ret_val = (uint16) (((duration_ms *
                            voc_data_sr_table[voc_data_sample_rate])/576)+1);
      break;
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
   case QDSP_IMAGE_CAM_AUDIO_CONC:
    if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_MP3 ){
      ret_val = (uint16) (((duration_ms *
                            voc_data_sr_table[voc_data_sample_rate])/576)+1);
      }else{
          ret_val = duration_ms/5;
      }
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY*/

#endif /* FEATURE_QTUNES */

#ifdef FEATURE_QTUNES_AAC
#if defined(QDSP_IMAGE_AAC_DEFINED)           || \
    defined(QDSP_IMAGE_ENH_AAC_PLUS_DEFINED)  || \
    defined(QDSP_IMAGE_H264D_AAC_DEFINED)     || \
    defined(QDSP_IMAGE_QTV_AAC_H264_DEFINED)  || \
    defined(QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED)|| \
    defined(QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED) || \
    defined(QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED) || \
    defined(QDSP_IMAGE_QTV_AAC_DEFINED)
#ifdef QDSP_IMAGE_AAC_DEFINED
    case QDSP_IMAGE_AAC:
#endif /* QDSP_IMAGE_AAC_DEFINED */
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
    case QDSP_IMAGE_ENH_AAC_PLUS:
#endif /* QDSP_IMAGE_ENH_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED
    case QDSP_IMAGE_MP4D_AAC_PLUS:
#endif /* QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED
    case QDSP_IMAGE_MP4D_AAC_NO_VPP:
#endif
#ifdef QDSP_IMAGE_H264D_AAC_DEFINED
    case QDSP_IMAGE_H264D_AAC:
#endif /* QDSP_IMAGE_H264D_AAC_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
    case QDSP_IMAGE_QTV_AAC:
#endif /* QDSP_IMAGE_QTV_AAC_DEFINED */
      if (voc_data_sample_rate <= VOC_SAMPLE_RATE_24000) {
        ret_val = (uint16) (((duration_ms *
                              voc_data_sr_table[voc_data_sample_rate + 3]) /
                             2048) + 1);
        break;
      }
#endif /* QDSP_IMAGE_AAC_DEFINED             ||
          QDSP_IMAGE_ENH_AAC_PLUS_DEFINED    ||
          QDSP_IMAGE_H264D_AAC_DEFINED       ||
          QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED   ||
          QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED ||
          QDSP_IMAGE_QTV_AAC_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_DEFINED
    case QDSP_IMAGE_MP4D_AAC:
#endif /* QDSP_IMAGE_MP4D_AAC_DEFINED */
#ifdef QDSP_IMAGE_H263D_AAC_DEFINED
    case QDSP_IMAGE_H263D_AAC:
#endif /* QDSP_IMAGE_H263D_AAC_DEFINED */
      ret_val = (uint16) (((duration_ms *
                            voc_data_sr_table[voc_data_sample_rate])/1024)+1);
      break;
#endif /* FEATURE_QTUNES_AAC */

#ifdef QDSP_IMAGE_RA_DEFINED
    case QDSP_IMAGE_RA:
      ret_val = (uint16) (((duration_ms *
                            voc_data_sr_table[voc_data_sample_rate]) /
                           state->real_audio_config.samples_per_frame) +1);
      break;
#endif /* QDSP_IMAGE_RA_DEFINED */

#ifdef QDSP_IMAGE_WM_DEFINED
    case QDSP_IMAGE_WM:
#endif /* QDSP_IMAGE_WM_DEFINED */
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_WM_PRO_DEFINED */

      ret_val = (uint16) (((duration_ms *
                            voc_data_sr_table[voc_data_sample_rate]) /1024)+1);
      break;

#ifdef QDSP_IMAGE_QTV_CONC_DEFINED
    case QDSP_IMAGE_QTV_CONC:
      ret_val = (uint16) (((duration_ms *
                            voc_data_sr_table[voc_data_sample_rate])/576)+1);
#endif /* QDSP_IMAGE_QTV_CONC_DEFINED */

#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_AVS_I2SSBC
    case QDSP_IMAGE_I2SSBC:
      ret_val = (uint16) (((duration_ms *
                          voc_data_sr_table[voc_data_sample_rate])/512)+1);
      break;
#endif /* FEATURE_AVS_I2SSBC */

#ifdef FEATURE_GRAPH_AUDIO
#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
    case QDSP_IMAGE_VFE_BAYER:
      ret_val = (duration_ms * 4) / 5;
      break;
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED */

#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
    case QDSP_IMAGE_VFE_YCBCR:
      ret_val = (duration_ms * 4) / 5;
      break;
#endif /* QDSP_IMAGE_VFE_YCBCR_DEFINED */

#ifdef QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED
    case QDSP_IMAGE_QCAMCORDER_AACENC:
      ret_val = (uint16) (duration_ms *
                          voc_data_spl_rate_val[voc_data_sample_rate] / 40);
      break;
#endif /* QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED */

#ifdef QDSP_IMAGE_AACENC_DEFINED
    case QDSP_IMAGE_AACENC:
      ret_val = (uint16) (duration_ms *
                          voc_data_spl_rate_val[voc_data_sample_rate] / 40);
      break;
#endif /* QDSP_IMAGE_AACENC_DEFINED */

#endif /* FEATURE_GRAPH_AUDIO */
  }
#endif /* FEATURE_QSYNTH_COMMON || FEATURE_QTUNES_COMMON) ||
          FEATURE_GRAPH_AUDIO */

  return (ret_val);
}

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

FUNCTION voc_data_get_vr_bark_weight

DESCRIPTION
  Get the VR bark amp weighting values to use during VR capture.

DEPENDENCIES
  voc_data_set_vr_cal() must be called prior.

RETURN VALUE
  Pointer to bark amp weighting data.

SIDE EFFECTS
  None.

===========================================================================*/
const unsigned short* voc_data_get_vr_bark_weight (
  voc_wideband_vr_type band_selection
)
{
  ASSERT( voc_data_vr_cal_ptr != NULL );

  if (band_selection == VOC_WIDEBAND_VR)
  {
    return (voc_data_vr_cal_ptr->barkamp_weight.wideband);
  }
  else
  {
    return (voc_data_vr_cal_ptr->barkamp_weight.narrowband);
  }
} /* end voc_data_get_vr_bark_weight */
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#if defined(FEATURE_AUDFMT_FIR_FILTER) && defined(FEATURE_QTUNES_COMMON)
/*===========================================================================

FUNCTION voc_data_get_qtunes_pcm_filter

DESCRIPTION
  Get the Qtunes PCM filter config for the current path.

DEPENDENCIES
  FEATURE_AUDFMT_FIR_FILTER && FEATURE_QTUNES_COMMON

RETURN VALUE
  Pointer to the Qtunes PCM filter config structure.

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_audfmt_pcm_filter_type *voc_data_get_qtunes_pcm_filter
(
  void
)
{
  ASSERT(voc_data_pcm_cal_ptr != NULL);

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    return (voc_data_pcm_cal_ptr->qtunes_filter[voc_data_get_sample_rate()]);
  }

} /* end voc_data_get_qtunes_pcm_filter */
#endif /* FEATURE_AUDFMT_FIR_FILTER && FEATURE_QTUNES_COMMON */

/* <EJECT> */
#if defined(FEATURE_QTUNES_ADRC) || defined(FEATURE_QSYNTH_ADRC)
/*===========================================================================

FUNCTION voc_data_get_audfmt_adrc_ctl

DESCRIPTION
  Get the Qtunes, Qsynth ADRC config for the current path.

DEPENDENCIES
  FEATURE_QTUNES_ADRC

RETURN VALUE
  Pointer to the Qtunes PCM filter config structure.

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_audfmt_adrc_ctl_type *voc_data_get_audfmt_adrc_ctl
(
  void
)
{
  ASSERT(voc_data_pcm_cal_ptr != NULL);

  switch(voc_state_get_state_data()->current_image) {
#ifdef FEATURE_QSYNTH_ADRC
#ifdef QDSP_IMAGE_SYNTH_DEFINED
    case QDSP_IMAGE_SYNTH:
#endif
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QGL_DEFINED
    case QDSP_IMAGE_QGL:
#endif
#ifdef QDSP_IMAGE_Q3D_DEFINED
    case QDSP_IMAGE_Q3D:
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
    case QDSP_IMAGE_VOC_COMBO_SYNTH:
#endif
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
      {
        return (voc_data_pcm_cal_ptr->qsynth_adrc_ctl[
                            voc_data_get_sample_rate()]);
      }
      break;
#endif /* FEATURE_QSYNTH_ADRC */

#ifdef FEATURE_QTUNES_ADRC
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
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
      {
        return (voc_data_pcm_cal_ptr->qtunes_adrc_ctl[
                                              voc_data_get_sample_rate()]);
      }
      break;
#endif /* FEATURE_QTUNES_ADRC */
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
    case QDSP_IMAGE_CAM_AUDIO_CONC:
      if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_MP3 ){
#ifdef FEATURE_QTUNES_ADRC
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
        {
          return (voc_data_pcm_cal_ptr->qtunes_adrc_ctl[
                            voc_data_get_sample_rate()]);
        }
#endif /*  FEATURE_QTUNES_ADRC */
      }else
      if (qdsp_cmd_get_decoder_in_use() ==  QDSP_CMD_DEC_WAVE){
#ifdef FEATURE_QSYNTH_ADRC
#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
        {
          return (voc_data_pcm_cal_ptr->qsynth_adrc_ctl[
                              voc_data_get_sample_rate()]);
        }
#endif /* FEATURE_QSYNTH_ADRC */
      }
      else
      {
#ifdef FEATURE_QTUNES_ADRC
        return (voc_data_pcm_cal_ptr->qtunes_adrc_ctl[
                            voc_data_get_sample_rate()]); 
#endif
      }
    break;
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */

    default:
      break;
  }

  return (NULL);
} /* end voc_data_get_audfmt_adrc_ctl */
#endif /* FEATURE_QTUNES_ADRC || FEATURE_QSYNTH_ADRC */

/* <EJECT> */
#if defined(FEATURE_AUDFMT_FIR_FILTER) && defined(FEATURE_QSYNTH_COMMON)
/*===========================================================================

FUNCTION voc_data_get_qsynth_pcm_filter

DESCRIPTION
  Get the QSYNTH PCM filter config for the current path.

DEPENDENCIES
  FEATURE_AUDFMT_FIR_FILTER && FEATURE_QSYNTH_COMMON

RETURN VALUE
  Pointer to the QSYNTH PCM filter config structure.

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_audfmt_pcm_filter_type *voc_data_get_qsynth_pcm_filter
(
  void
)
{
  ASSERT( voc_data_pcm_cal_ptr != NULL );

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
#ifdef MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT
       if (
#ifdef FEATURE_AUDIO_QSYNTH_48K_SUPPORT
        voc_data_get_sample_rate() == VOC_SAMPLE_RATE_48000
#else
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
        voc_data_get_sample_rate() == VOC_SAMPLE_RATE_44000
#else
        voc_data_get_sample_rate() == VOC_SAMPLE_RATE_44100
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
#endif /* FEATURE_AUDIO_QSYNTH_48K_SUPPORT */
       ) {
      return (&voc_data_pcm_cal_ptr->qsynth_filter[1]);
    }
#endif /* MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT */
    return (&voc_data_pcm_cal_ptr->qsynth_filter[0]);
  }

} /* end voc_data_get_qsynth_pcm_filter */
#endif /* FEATURE_AUDFMT_FIR_FILTER && FEATURE_QSYNTH_COMMON */

#ifndef MSMAUD_SCMM
/* <EJECT> */
#ifdef MSMAUD_ADIE_CODEC
/*===========================================================================

FUNCTION voc_data_adie_audio_override

DESCRIPTION
  This function modifies the ADIE codec config for audio override mode.

DEPENDENCIES
  MSMAUD_ADIE_CODEC

RETURN VALUE
  Pointer to the ADIE codec config structure.

SIDE EFFECTS
  None.

===========================================================================*/
voc_adie_codec_config_type *voc_data_adie_audio_override (
  voc_adie_codec_config_type *adie_config
) {
  static uint8                      audio_override_index = 0;
  static voc_adie_codec_config_type *last_adie_config    = NULL;
  static voc_adie_codec_config_type audio_override_adie_config[2];
  static adie_codec_reg_type        audio_override_adie_reg[2]
                                             [MSMAUD_ADIE_MAX_CONFIG_REGS];

  voc_adie_codec_config_type *current_adie_config;
  adie_codec_reg_type        *current_adie_reg;
  int                        counter;

  if((adie_config != NULL) && (adie_config->val != NULL) &&
     (adie_config->size != 0) &&
     (adie_config->size <= MSMAUD_ADIE_MAX_CONFIG_REGS)) {
    if(last_adie_config != adie_config) {
      if(audio_override_index == 0) {
        audio_override_index = 1;
      } else {
        audio_override_index = 0;
      }
      current_adie_config = &audio_override_adie_config[audio_override_index];
      current_adie_reg    = &(audio_override_adie_reg[audio_override_index][0]);

      memcpy((uint8 *) current_adie_reg, (uint8 *) adie_config->val,
             adie_config->size * sizeof(adie_codec_reg_type));

      current_adie_config->size         = adie_config->size;
      current_adie_config->num_to_delay = adie_config->num_to_delay;
      current_adie_config->val          = current_adie_reg;

      counter = current_adie_config->size;
      do {
        counter--;
        if(current_adie_reg[counter].reg == MSMAUD_ADIE_MIC_AMP1_GAIN_REG) {
          MSMAUD_ADIE_ADJUST_MIC_AMP1_GAIN (current_adie_reg[counter].val);
          break;
        }
      } while (counter != 0);

      last_adie_config = adie_config;
    } else {
      current_adie_config = &audio_override_adie_config[audio_override_index];
    }

    return(current_adie_config);
  } else {
    return(adie_config);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_adie_codec_config

DESCRIPTION
  Get the ADIE codec config from the current path.

DEPENDENCIES
  MSMAUD_ADIE_CODEC

RETURN VALUE
  Pointer to the ADIE codec config structure.

SIDE EFFECTS
  None.

===========================================================================*/
voc_adie_codec_config_type *voc_data_get_adie_codec_config
(
  qdsp_image_type              image,
  voc_codec_type               codec
)
{
  voc_adie_codec_config_type *adie_config_ptr = NULL;

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
  {
    if(voc_cal_get_adie_config(codec,image,&adie_config_ptr) == VOC_CAL_SUCCESS) {

      if(voc_data_aud_process_override_enabled) {
        return( voc_data_adie_audio_override(adie_config_ptr) );
      }
    }
    return( adie_config_ptr );
  }
} /* end voc_data_get_adie_codec_config */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_adie_codec_pwr_down_config

DESCRIPTION
  Get the ADIE codec power down config.

DEPENDENCIES
  MSMAUD_ADIE_CODEC

RETURN VALUE
  Pointer to the ADIE codec config structure.

SIDE EFFECTS
  None.

===========================================================================*/
voc_adie_codec_config_type *voc_data_get_adie_codec_pwr_down_config
(
  void
)
{
  voc_hssd_type * hssd_mode;

  hssd_mode = voc_data_get_hssd_mode();

  if( *hssd_mode == VOC_HSSD_ENABLED ) {
    return ( (voc_adie_codec_config_type *)
      &voc_data_adie_codec_pwr_down_with_hssd );
  } else {
    return( (voc_adie_codec_config_type *) &voc_data_adie_codec_pwr_down );
  }
} /* end voc_data_get_adie_codec_pwr_down_config */
#endif /* MSMAUD_ADIE_CODEC */
#endif /* MSMAUD_SCMM */

#ifdef MSMAUD_PMIC_CONFIG
/*===========================================================================

FUNCTION voc_data_get_pmic_config

DESCRIPTION
  Get the PMIC codec config from the current path.

DEPENDENCIES
  MSMAUD_PMIC_CONFIG

RETURN VALUE
  Pointer to the PMIC config structure.

SIDE EFFECTS
  None.

===========================================================================*/
voc_pmic_config_type *voc_data_get_pmic_config
(
  qdsp_image_type              image,
  voc_codec_type               codec
)
{
  voc_pmic_config_type *pmic_config_ptr = NULL;

  voc_cal_get_pmic_config(codec,image,&pmic_config_ptr);
  return( pmic_config_ptr );
}

#endif /* MSMAUD_PMIC_CONFIG */

/* <EJECT> */
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION voc_data_set_dls_info

DESCRIPTION
  Set DLS config.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_set_dls_config (
  voc_dls_config_type *dls_config
) {
  voc_data_dls_config.hp_inst_def  = dls_config->hp_inst_def;
  voc_data_dls_config.hp_drum_def  = dls_config->hp_drum_def;
  voc_data_dls_config.hp_wave_info = dls_config->hp_wave_info;
  voc_data_dls_config.hq_inst_def  = dls_config->hq_inst_def;
  voc_data_dls_config.hq_drum_def  = dls_config->hq_drum_def;
  voc_data_dls_config.hq_wave_info = dls_config->hq_wave_info;
  voc_data_dls_config.hq_inst_num  = dls_config->hq_inst_num;
  voc_data_dls_config.hq_drum_num  = dls_config->hq_drum_num;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_dls_info

DESCRIPTION
  Get DLS config.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_dls_config_type *voc_data_get_dls_config (void) {
  return(&voc_data_dls_config);
}
#endif /* FEATURE_MIDI */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_EQUALIZER
/*===========================================================================

FUNCTION voc_data_enable_equalizer

DESCRIPTION
  This function will enable/dsiable the Equalizer control flag and setup
  the equalizer filter talbe for QSYNTH/QTUNES.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_data_enable_equalizer
(
  boolean                   enable
)
{
  voc_data_tbl_eq_ctl.enable = enable;

  return (VOC_DONE_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_equalizer

DESCRIPTION
  This function will enable/dsiable the Equalizer control flag and setup
  the equalizer filter talbe for QSYNTH/QTUNES.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S
  VOC_CMD_FAILED_S

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_data_set_equalizer
(
  voc_equalizer_filter_type *filter,
  uint16                    num
)
{

  if ((filter == NULL) || (num > VOC_DATA_EQUALIZER_MAX_BAND)) {
    return (VOC_CMD_FAILED_S);
  }

  memcpy(voc_data_equalizer_tbl, filter,
         num * sizeof(voc_equalizer_filter_type));

  voc_data_tbl_eq_ctl.bands = num;
  voc_data_tbl_eq_ctl.reset = TRUE;

  return (VOC_DONE_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_equalizer_gain

DESCRIPTION
  Set the new equalizer gain value on a specified band.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_data_set_equalizer_gain(
  uint16 band,
  int16  gain
) {

  if ((band < voc_data_tbl_eq_ctl.bands) &&
      (gain <= VOC_DATA_EQUALIZER_MAX_GAIN) &&
      (gain >= VOC_DATA_EQUALIZER_MIN_GAIN))
  {
    voc_data_equalizer_tbl[band].gain = gain;
    voc_data_tbl_eq_ctl.reset         = TRUE;
    return (VOC_DONE_S);
  }
  return (VOC_CMD_FAILED_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_equalizer

DESCRIPTION
  Get the Equalizer control for QSYNTH/QTUNES for the current path.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern const qdsp_cmd_equalizer_type *voc_data_get_equalizer
(
  void
)
{
  uint16 sample_rate, sample_rate_div_2;
  uint16 i,j; /* j = number of filter need calculate */
  uint16 offset; /* Offset of the coeff buffer */
  uint16 temp_num, temp_den, temp_shift;
  voc_equalizer_filter_type *filter;
  voc_sample_rate_type      voc_sr;
  uint16                    *coeff_ptr;


  if ((voc_data_tbl_eq_ctl.enable) ||
      (voc_data_preset_eq_ctl.enable)) {

    voc_sr = voc_data_get_sample_rate();
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
    if (voc_sr == VOC_SAMPLE_RATE_44000) {
      voc_sr = VOC_SAMPLE_RATE_44100;
    }
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
    voc_data_equalizer_ctl.enable  = TRUE;

    if ((voc_data_tbl_eq_ctl.reset) ||
        (voc_data_preset_eq_ctl.reset) ||
        (voc_data_equalizer_sample_rate != voc_sr)) {
      voc_data_equalizer_ctl.band    = 0;
      offset                         = 0;
      voc_data_tbl_eq_ctl.reset      = FALSE;
      voc_data_preset_eq_ctl.reset   = FALSE;
      voc_data_equalizer_sample_rate = voc_sr;
      sample_rate                    = voc_data_sr_table[voc_sr] * 1000;
      sample_rate_div_2              = sample_rate >> 1;


      /* Get number of bands that is with the freq range from EQ table
      ** If the EQ table is enabled
      */
      j = 0;
      if (voc_data_tbl_eq_ctl.enable) {
        for (i = 0; i < voc_data_tbl_eq_ctl.bands ; i++) {
          if ((voc_data_equalizer_tbl[i].freq < sample_rate_div_2) &&
              (voc_data_equalizer_tbl[i].freq <= 20000)) {
            voc_data_equalizer_coeff_filter_tbl[j] = i;
            j++;
          }
        }
        /* Calculate total number of bands */
        voc_data_equalizer_ctl.band = j;
      }

      if ((voc_data_preset_eq_ctl.enable) && (voc_data_preset_eq_ctl.bands)) {
        /* Calculate total number of bands */
        voc_data_equalizer_ctl.band += voc_data_preset_eq_ctl.bands;

        /* Used the total number of bands to find out the offset,
        ** Always have the preset EQ at the beginning of the buffer
        */
        temp_num   = EQ_NUM_OFFSET(0, voc_data_equalizer_ctl.band);
        temp_den   = EQ_DEN_OFFSET(0, voc_data_equalizer_ctl.band);
        temp_shift = EQ_SHIFT_OFFSET(0, voc_data_equalizer_ctl.band);
        offset     = voc_data_preset_eq_ctl.bands;

        /* Get the coeff value base off the sample rate */
        coeff_ptr  = voc_data_preset_eq_ctl.coeff[voc_sr];

        if (coeff_ptr != NULL) {

          /* Copy the coeff */
          memcpy(&voc_data_equalizer_coeff[temp_num],
                 &coeff_ptr[0],
                 6*2*offset);
          memcpy(&voc_data_equalizer_coeff[temp_den],
                 &coeff_ptr[6*offset],
                 4*2*offset);
          memcpy(&voc_data_equalizer_coeff[temp_shift],
                 &coeff_ptr[10*offset],
                 2*offset);
        }
      }

      /* Calculate the filter from the EQ Table */
      if (voc_data_tbl_eq_ctl.enable) {
        for (i = 0; i<j;i++) {
          /* get the offset pointer of the coeff buffer */
          temp_num   = EQ_NUM_OFFSET(offset+i, voc_data_equalizer_ctl.band);
          temp_den   = EQ_DEN_OFFSET(offset+i, voc_data_equalizer_ctl.band);
          temp_shift = EQ_SHIFT_OFFSET(offset+i, voc_data_equalizer_ctl.band);

          /* Get the active filter */
          filter = &voc_data_equalizer_tbl[
                              voc_data_equalizer_coeff_filter_tbl[i]];

          /* Calculate and store the active filter */
          audioeq_calccoefs(filter->gain,
                            filter->freq,
                            sample_rate,
                            filter->type,
                            filter->qf,
                            (int32 *)&voc_data_equalizer_coeff[temp_num],
                            (int32 *)&voc_data_equalizer_coeff[temp_den],
                            &voc_data_equalizer_coeff[temp_shift]);
        }
      }
    }
  } else {
    voc_data_equalizer_ctl.enable = FALSE;
  }
  return (&voc_data_equalizer_ctl);
}

/*===========================================================================

FUNCTION voc_data_set_preset_eq_coeff

DESCRIPTION
  Get the Equalizer control for QSYNTH/QTUNES for the current path.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern voc_status_type voc_data_set_preset_eq_coeff
(
  const uint16 **coeff,
  uint16       bands,
  boolean      enable
) {
  uint16 i;
  uint16 size;
  if ((coeff != NULL) && (bands < (VOC_PRESET_EQ_MAX_NUM + 1))) {
    if (enable) {
      for (i = VOC_SAMPLE_RATE_8000;i<VOC_SAMPLE_RATE_MAX;i++) {
        size = bands * VOC_DATA_EQUALIZER_COEFF_SIZE * sizeof(uint16);
        memcpy(voc_data_preset_eq_ctl.coeff[i],
               coeff[i],
               size );
      }
    }
    voc_data_preset_eq_ctl.bands  = bands;
    voc_data_preset_eq_ctl.enable = enable;
    voc_data_preset_eq_ctl.reset  = TRUE;
    return (VOC_DONE_S);
  }
  return (VOC_CMD_FAILED_S);
}
#endif /* FEATURE_AUDFMT_EQUALIZER */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/*===========================================================================

FUNCTION voc_data_set_spectrum_analyzer_period

DESCRIPTION
  Set the spectrum analyzer update period in the mulitple of 10ms interval

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_set_spectrum_analyzer_period
(
  uint16 update_period
)
{
  voc_data_spectrum_analyzer_update_period = update_period;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_spectrum_analyzer_interval

DESCRIPTION
  Get the spectrum analyzer update period interval, it is calculated base
  on the sampling rate.

DEPENDENCIES
  None.

RETURN VALUE
  The update interval meansured in number of samples.

SIDE EFFECTS
  None.

===========================================================================*/
const uint16 voc_data_get_spectrum_analyzer_interval ( void )
{
  uint16 sample_rate = voc_data_sr_table[voc_data_get_sample_rate()];

  return (voc_data_spectrum_analyzer_update_period * sample_rate);
}
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

/* <EJECT> */
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_QCONCERT
/*===========================================================================

FUNCTION voc_data_enable_qconcert

DESCRIPTION
  This function enable/disable the global qconcert setting

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_enable_qconcert( boolean enable)
{
  voc_data_qconcert_enable = enable;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_qconcert_params

DESCRIPTION
  This function setup the qconcert setting.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_data_set_qconcert_params(
  voc_qconcert_params_type *params
)
{
  voc_data_qconcert_parameter_type *qconcert_parameters = NULL;

  /* Check for valid configuration */
  if ((params->gain > VOC_DATA_QCONCERT_MAX_GAIN_V) ||
      (params->spread > VOC_DATA_QCONCERT_MAX_SPREAD_V) ||
#ifdef FEATURE_AUDFMT_QCONCERT_PLUS
      (((params->mode ==1) || (params->mode ==2))&&
      (params->geometry > VOC_DATA_QCONCERT_PLUS_MAX_GEOMETRY_V) ||
      (params->geometry < VOC_DATA_QCONCERT_PLUS_MIN_GEOMETRY_V))||
#endif
      (params->geometry > VOC_DATA_QCONCERT_MAX_GEOMETRY_V) ||
      (params->geometry < VOC_DATA_QCONCERT_MIN_GEOMETRY_V)) {
    return (VOC_CMD_FAILED_S);
  }

  qconcert_parameters = voc_data_qconcert_configs[params->mode];
  qconcert_parameters->gain     = params->gain;
  qconcert_parameters->spread   = params->spread;

  return (VOC_DONE_S);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_qconcert_config

DESCRIPTION
  This function get the global qconcert setting.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DONE_S to indicate qconcert_config has valid qconcert setting
  VOC_CMD_FAILED_S to qconcert setting has not been set
SIDE EFFECTS
  None.

===========================================================================*/
voc_status_type voc_data_get_qconcert_config(
  qdsp_cmd_qconcert_config_type *qconcert_config
){
  uint8 output_mode;
  voc_sample_rate_type sample_rate;
  voc_status_type ret_val = VOC_CMD_FAILED_S;

  sample_rate = voc_data_get_sample_rate();

  if ((qconcert_config != NULL) &&
      (sample_rate < VOC_SAMPLE_RATE_MAX)) {

    TASKLOCK();

    switch(voc_data_codec_inuse){

      case VOC_CODEC_STEREO_HEADSET:
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
      case VOC_CODEC_BT_A2DP:
#endif /* FEATURE_SBC_CODEC || FEATURE_SBC_DSP_CODEC */
        output_mode = VOC_DATA_QCONCERT_HP_MODE;
        qconcert_config->enable = voc_data_qconcert_enable;
      break;

#ifdef FEATURE_SPEAKER_PHONE
      case VOC_CODEC_SPEAKER:
        output_mode = VOC_DATA_QCONCERT_SPKFF_MODE;
        qconcert_config->enable = voc_data_qconcert_enable;
      break;
#endif /* FEATURE_SPEAKER_PHONE */

      default:
        output_mode = VOC_DATA_QCONCERT_SPKFF_MODE;
        qconcert_config->enable = FALSE;
      break;
    }

    qconcert_config->gain = voc_data_qconcert_configs[output_mode]->gain;
    qconcert_config->spread = voc_data_qconcert_configs[output_mode]->spread;
    qconcert_config->mode = voc_data_qconcert_mode_table[output_mode];
    qconcert_config->geometry =
          voc_data_qconcert_configs[output_mode]->geometry_ptr[sample_rate];
    qconcert_config->sample_rate =
                       voc_data_qconcert_sr_table[sample_rate];
    ret_val = VOC_DONE_S;
    TASKFREE();
  }

  return ret_val;
}
#endif /* FEATURE_AUDFMT_QCONCERT */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_STF
/*===========================================================================

FUNCTION voc_data_get_audfmt_sidechain_filter

DESCRIPTION
  This function get the audio sidechain tunning filter setting.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_audfmt_sidechain_filter_type*
                                         voc_data_get_audfmt_sidechain_filter
(
  void
)
{
  ASSERT(voc_data_pcm_cal_ptr != NULL);

  return(voc_data_pcm_cal_ptr->audfmt_sidechain_filter[
                                               voc_data_get_sample_rate()]);
}
#endif /* FEATURE_AUDFMT_STF */

#ifdef FEATURE_AUDFMT_IIR_FILTER
/*===========================================================================

FUNCTION voc_data_get_audfmt_iir_filter

DESCRIPTION
  This function get the audio sidechain tunning filter setting.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
const qdsp_cmd_audfmt_iir_filter_type* voc_data_get_audfmt_iir_filter
(
  void
)
{
  ASSERT(voc_data_pcm_cal_ptr != NULL);

  return(voc_data_pcm_cal_ptr->audfmt_iir_filter[voc_data_get_sample_rate()]);
}
#endif /* FEATURE_AUDFMT_IIR_FILTER */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_is_stereo_tx_codec

DESCRIPTION
  Check if the codec inuse supports stereo tx path.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_data_is_stereo_tx_codec(void)
{
  voc_codec_type         pcm_path;

  voc_data_get_codec(&pcm_path);

  switch (pcm_path)
  {
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
    case VOC_CODEC_ON_CHIP_0_DUAL_MIC:
      return TRUE;
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

#if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
    case VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET:
    case VOC_CODEC_IN_STEREO_SADC_OUT_STEREO_HEADSET:
      return TRUE;
#endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */

    default:
      break;
  }


  return FALSE;
}

/* <EJECT> */
#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
/*===========================================================================

FUNCTION voc_data_stereo_codec

DESCRIPTION
  Check if the codec inuse is a Stereo codec or mono codec.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_data_stereo_codec()
{
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  voc_codec_class_type             codec_type;
  voc_data_get_codec_class(&codec_type);
#endif

#ifdef FEATURE_INTERNAL_SDAC
  if (codec_type ==  VOC_CODEC_CLASS_INTERNAL_STEREO)
  {
    return TRUE;
  }
  else
#endif
#ifdef FEATURE_EXTERNAL_SDAC
  if (codec_type ==  VOC_CODEC_CLASS_EXTERNAL_STEREO)
  {
    return TRUE;
  }
  else
#endif
  {
    return FALSE;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_register_pp_func

DESCRIPTION
 Registers the snd post processing callback function

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_register_pp_func (voc_pp_cb_type pp_cb)
{
  voc_data_pp_cb = pp_cb;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_pp_func

DESCRIPTION
 Returns the snd post processing callback function

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
voc_pp_cb_type voc_data_get_pp_func(void)
{
  return voc_data_pp_cb;
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_save_pp_func

DESCRIPTION
 Registers the snd post processing callback function

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_save_pp_func (voc_pp_cb_type pp_cb, voc_pp_type_type pp_type)
{
  switch (pp_type) {
    case VOC_PP_TYPE_GENERIC:
    voc_data_pp_saved_func = pp_cb;
      break;
#if defined (FEATURE_SBC_CODEC)
    case VOC_PP_TYPE_SBC:
    voc_data_sbc_func = pp_cb;
      break;
#elif defined (FEATURE_SBC_DSP_CODEC)
    case VOC_PP_TYPE_DSP_SBC:
    voc_data_sbc_func = pp_cb;
      break;
#endif
}
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_saved_pp_func

DESCRIPTION
 Returns the saved snd pp callback function.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
voc_pp_cb_type voc_data_get_saved_pp_func(voc_pp_type_type pp_type)
{
  voc_pp_cb_type pp_cb = NULL;
  switch (pp_type) {
    case VOC_PP_TYPE_GENERIC:
    pp_cb = voc_data_pp_saved_func ;
      break;
#if defined (FEATURE_SBC_CODEC)
    case VOC_PP_TYPE_SBC:
    pp_cb = voc_data_sbc_func ;
      break;
#elif defined (FEATURE_SBC_DSP_CODEC)
    case VOC_PP_TYPE_DSP_SBC:
    pp_cb = voc_data_sbc_func ;
      break;
#endif
  }
  return pp_cb;
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_pp_type

DESCRIPTION
 Sets the PP type to SBC or GENERIC

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_set_pp_type(voc_pp_type_type pp_type)
{
  voc_data_pp_type = pp_type;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_pp_type

DESCRIPTION
 Gets the PP type

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
voc_pp_type_type voc_data_get_pp_type(void)
{
  return voc_data_pp_type;
}
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */
/* <EJECT> */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
/*===========================================================================

FUNCTION voc_data_set_sbc_remote_params

DESCRIPTION
 Sets the SBC confiuration information received from headset. We will configure
 the SBC encoder with paramters that is a subset of this.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_set_sbc_remote_params(audiosbcenc_data_type *remote_params)
{
  voc_sbc_remote_params = *remote_params; /* Do a bitwise copy (no pointers) */
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_sbc_channel_mode

DESCRIPTION
 Gets the SBC channel_mode

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
audiosbcenc_data_type *voc_data_get_sbc_remote_params(void)
{
  return &voc_sbc_remote_params;
}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_sbc_sample_rate

DESCRIPTION
 Gets the SBC sample rate

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
audiosbc_sample_rate_type voc_data_get_sbc_sample_rate()
{
  audiosbc_sample_rate_type     sbc_sample_rate = AUDIOSBC_SAMPLE_RATE_INVALID;

  switch (voc_state_get_state_data()->current_image) {
#ifdef QDSP_IMAGE_SYNTH_DEFINED
    case QDSP_IMAGE_SYNTH:
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif		
      sbc_sample_rate = AUDIOSBC_SAMPLE_RATE_48000;
      break;
#endif /* QDSP_IMAGE_SYNTH_DEFINED */

#if defined(QDSP_IMAGE_AAC_DEFINED)          || \
    defined(QDSP_IMAGE_QTV_AAC_DEFINED)      || \
    defined(QDSP_IMAGE_QTV_AAC_H264_DEFINED) || \
    defined(QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED)    || \
    defined(QDSP_IMAGE_TUNES_DEFINED)        || \
    defined(QDSP_IMAGE_WM_DEFINED)           || \
    defined(QDSP_IMAGE_WM_PRO_DEFINED)       || \
    defined(QDSP_IMAGE_ENH_AAC_PLUS_DEFINED) || \
    defined(QDSP_IMAGE_RA_DEFINED)           || \
    defined(QDSP_IMAGE_QTV_CONC_DEFINED)     || \
    defined(QDSP_IMAGE_I2SSBC_DEFINED)       || \
    defined(QDSP_IMAGE_QTV_DEFINED)  || \
    defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED) ||\
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED) /* A2DP support for for NB-AMR */

#ifdef QDSP_IMAGE_TUNES_DEFINED
    case QDSP_IMAGE_TUNES:
#endif /* QDSP_IMAGE_TUNES_DEFINED */
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
    case QDSP_IMAGE_CAM_AUDIO_CONC:
       /* Case is only valid for Mp3 . In case of ADPCM shutter sound
        The device will be speaker phone */
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */

#ifdef QDSP_IMAGE_AAC_DEFINED
    case QDSP_IMAGE_AAC:
#endif /* QDSP_IMAGE_AAC_DEFINED */

#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
    case QDSP_IMAGE_QTV_AAC:
#endif /* QDSP_IMAGE_QTV_AAC_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED */
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
    case QDSP_IMAGE_ENH_AAC_PLUS:
#endif /* QDSP_IMAGE_ENH_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_WM_DEFINED
    case QDSP_IMAGE_WM:
#endif /* QDSP_IMAGE_WM_DEFINED */
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_WM_PRO_DEFINED */
#ifdef QDSP_IMAGE_RA_DEFINED
    case QDSP_IMAGE_RA:
#endif /* QDSP_IMAGE_RA_DEFINED */
#ifdef QDSP_IMAGE_QTV_CONC_DEFINED
    case QDSP_IMAGE_QTV_CONC:
#endif /* QDSP_IMAGE_QTV_CONC_DEFINED */
#ifdef QDSP_IMAGE_I2SSBC_DEFINED
    case QDSP_IMAGE_I2SSBC:
#endif /* QDSP_IMAGE_I2SSBC_DEFINED */
    /* A2DP support for for NB-AMR */
#ifdef QDSP_IMAGE_QTV_DEFINED
    case QDSP_IMAGE_QTV:
#endif /* QDSP_IMAGE_QTV_DEFINED */
#ifdef QDSP_IMAGE_QTV_VOC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_VOC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED */
      switch (voc_data_sample_rate) {
        case VOC_SAMPLE_RATE_8000:
        case VOC_SAMPLE_RATE_12000:
        case VOC_SAMPLE_RATE_16000:
        case VOC_SAMPLE_RATE_24000:
        case VOC_SAMPLE_RATE_32000:
        case VOC_SAMPLE_RATE_48000:
          sbc_sample_rate = AUDIOSBC_SAMPLE_RATE_48000;
          break;

        case VOC_SAMPLE_RATE_11025:
        case VOC_SAMPLE_RATE_22050:
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
        case VOC_SAMPLE_RATE_44000:
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
        case VOC_SAMPLE_RATE_44100:
          sbc_sample_rate = AUDIOSBC_SAMPLE_RATE_44100;
          break;

        default:
          sbc_sample_rate = AUDIOSBC_SAMPLE_RATE_INVALID;
          break;
      }
    break;
#endif /* QDSP_IMAGE_AAC_DEFINED          ||
          QDSP_IMAGE_QTV_AAC_DEFINED      ||
          QDSP_IMAGE_TUNES_DEFINED        ||
          QDSP_IMAGE_WM_DEFINED           ||
          QDSP_IMAGE_ENH_AAC_PLUS_DEFINED ||
          QDSP_IMAGE_RA_DEFINED           ||
          QDSP_IMAGE_QTV_CONC_DEFINED     ||
          QDSP_IMAGE_I2SSBC_DEFINED       */

    default:
      sbc_sample_rate = AUDIOSBC_SAMPLE_RATE_INVALID;
      break;
  }

  return sbc_sample_rate;
}
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_VOC_MIC_GAIN_ADJUST
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_mic_gain_adjust

DESCRIPTION
  Sets the mic gain adjustment value for the specified codec.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_DATA_SUCCESS - The codec mic gain adjust was set successfully.
  VOC_DATA_FAILURE - The attempt to set the codec mic gain adjust failed.

SIDE EFFECTS
  None.

===========================================================================*/
voc_data_return_type voc_data_set_mic_gain_adjust
(
  voc_codec_type codec,
  sint15         dbm_val
)
{
  if (VOC_CAL_SUCCESS == voc_cal_set_mic_gain_adjust(codec, dbm_val)) {
    return VOC_DATA_SUCCESS;
  } else {
    return VOC_DATA_FAILURE;
  }
}
#endif /* FEATURE_VOC_MIC_GAIN_ADJUST */

#ifdef FEATURE_AUDIO_FORMAT
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_num_samples_per_sec

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  (number of sample per sec)/1000

SIDE EFFECTS
  None.

===========================================================================*/
const uint16 voc_data_get_num_samples_per_sec ( void )
{
  voc_state_control_type *state;
  uint16 sample_rate = 0;

  state = voc_state_get_state_data();
  switch (state->current_image) {
#if defined(QDSP_IMAGE_AAC_DEFINED)             || \
    defined(QDSP_IMAGE_ENH_AAC_PLUS_DEFINED)    || \
    defined(QDSP_IMAGE_H264D_AAC_DEFINED)       || \
    defined(QDSP_IMAGE_QTV_AAC_H264_DEFINED)    || \
    defined(QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED)    || \
    defined(QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED)   || \
    defined(QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED) || \
    defined(QDSP_IMAGE_QTV_AAC_DEFINED)
#ifdef QDSP_IMAGE_AAC_DEFINED
    case QDSP_IMAGE_AAC:
#endif /* QDSP_IMAGE_AAC_DEFINED */
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
    case QDSP_IMAGE_ENH_AAC_PLUS:
#endif /* QDSP_IMAGE_ENH_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED
    case QDSP_IMAGE_MP4D_AAC_PLUS:
#endif /* QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED
    case QDSP_IMAGE_MP4D_AAC_NO_VPP:
#endif
#ifdef QDSP_IMAGE_H264D_AAC_DEFINED
    case QDSP_IMAGE_H264D_AAC:
#endif /* QDSP_IMAGE_H264D_AAC_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
    case QDSP_IMAGE_QTV_AAC:
#endif /* QDSP_IMAGE_QTV_AAC_DEFINED */
      sample_rate = voc_data_sr_table[voc_data_sample_rate];
      break;
#endif /* QDSP_IMAGE_AAC_DEFINED             ||
          QDSP_IMAGE_ENH_AAC_PLUS_DEFINED    ||
          QDSP_IMAGE_H264D_AAC_DEFINED       ||
          QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED   ||
          QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED ||
          QDSP_IMAGE_QTV_AAC_DEFINED */

    default:
       sample_rate = voc_data_sr_table[voc_data_get_sample_rate()];
  }
  return sample_rate;
}
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION voc_data_set_mm_parm

DESCRIPTION
  Set the current multimedia record parameters.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_mm_rec_parm

===========================================================================*/
void voc_data_set_mm_parm (
  voc_sample_rate_type  sample_rate,
  voc_mm_rec_chan_type  chan_index,
  uint16                bit_per_spl
)
{
  voc_data_set_sample_rate(sample_rate);

  voc_data_mm_rec_parm.bit_per_spl       = bit_per_spl;
  voc_data_mm_rec_parm.rec_chan_index    = chan_index;

  switch (sample_rate) {

  case VOC_SAMPLE_RATE_8000:
    voc_data_mm_rec_parm.sample_rate_index = 0xb;
    break;

  case VOC_SAMPLE_RATE_11025:
    voc_data_mm_rec_parm.sample_rate_index = 0xa;
    break;

  case VOC_SAMPLE_RATE_12000:
    voc_data_mm_rec_parm.sample_rate_index = 0x9;
    break;

  case VOC_SAMPLE_RATE_16000:
    voc_data_mm_rec_parm.sample_rate_index = 0x8;
    break;

  case VOC_SAMPLE_RATE_22050:
    voc_data_mm_rec_parm.sample_rate_index = 0x7;
    break;

  case VOC_SAMPLE_RATE_24000:
    voc_data_mm_rec_parm.sample_rate_index = 0x6;
    break;

  case VOC_SAMPLE_RATE_32000:
    voc_data_mm_rec_parm.sample_rate_index = 0x5;
    break;

#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  case VOC_SAMPLE_RATE_44000:
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
  case VOC_SAMPLE_RATE_44100:
    voc_data_mm_rec_parm.sample_rate_index = 0x4;
    break;

  case VOC_SAMPLE_RATE_48000:
    voc_data_mm_rec_parm.sample_rate_index = 0x3;
    break;

  default:
    voc_data_mm_rec_parm.sample_rate_index = 0x4;
    break;
  } /* switch (sample_rate) */
}

/*===========================================================================

FUNCTION voc_data_get_mm_parm

DESCRIPTION
  Get the current multimedia record parameters.

DEPENDENCIES
  FEATURE_MM_REC is defineds.

RETURN VALUE
  None.

SIDE EFFECTS
  voc_data_mm_rec_parm

===========================================================================*/
voc_mm_rec_parm_type* voc_data_get_mm_parm (void)
{
  voc_mm_rec_parm_type *ret_parm;

  ret_parm = &voc_data_mm_rec_parm;

  return (ret_parm);
}
#endif /* FEATURE_MM_REC */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
/*===========================================================================

FUNCTION voc_data_set_3d_pos_enable_mode

DESCRIPTION
  Set the enable mode for 3D positional.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_set_3d_pos_enable_mode (
  voc_3d_pos_mode_enum_type mode
) {
  voc_data_3d_pos_enable_mode = mode;
} /* voc_data_set_3d_pos_enable_mode */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_3d_pos_enable_mode

DESCRIPTION
  Get the enable mode for 3D positional.

DEPENDENCIES
  None.

RETURN VALUE
  voc_3d_pos_mode_enum_type

SIDE EFFECTS
  None.

===========================================================================*/
voc_3d_pos_mode_enum_type voc_data_get_3d_pos_enable_mode(void)
{
  return(voc_data_3d_pos_enable_mode);
} /* voc_data_set_3d_pos_enable_mode */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_3d_pos_output_mode

DESCRIPTION
  Get the output mode for 3D positional.

DEPENDENCIES
  voc_data_set_codec() must be called prior.

RETURN VALUE
  qdsp_cmd_3d_pos_output_mode_enum_type

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_3d_pos_output_mode_enum_type voc_data_get_3d_pos_output_mode(void)
{
  return( voc_data_pcm_cal_ptr->output_3d_mode );
} /* voc_data_get_3d_pos_output_mode */

/*===========================================================================

FUNCTION voc_data_compute_mb

DESCRIPTION
  This function returns the hardware register value for a given mb

DEPENDENCIES
  Requires voc_data_mb_table1[],
           voc_data_mb_table2[]
           voc_data_mb_table3[]
RETURN VALUE
  Returns a word containing the hardware register value.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 voc_data_compute_mb(
  sint31 mb_value
) {

  sint31 product = 32768;

	if (mb_value>=0) {
    return 0x7FFF;
  } else if (mb_value<=-8000) {
    return 0x0;
  } else {
    mb_value = -mb_value;

    while (mb_value>=2404) {
      product >>= 4;
      mb_value -= 2404;
    }

    while (mb_value>=602) {
      product >>= 1;
      mb_value -= 602;
    }

    if (mb_value>=256) {
      product = (product*voc_data_mb_table1[mb_value>>8]+(1<<14))>>15;
      mb_value &= 255;
    }

    if (mb_value>=16) {
      product = (product*voc_data_mb_table2[mb_value>>4]+(1<<14))>>15;
      mb_value &= 15;
    }
    if (mb_value) {
      product = (product*voc_data_mb_table3[mb_value]+(1<<14))>>15;
    }
  }

  return (uint16) product;
}

/*===========================================================================

FUNCTION voc_data_findmb

DESCRIPTION
  binary search for next set of bits in mb calculation

DEPENDENCIES
  Requires voc_data_mb_table1[],
           voc_data_mb_table2[]
           voc_data_mb_table3[]
RETURN VALUE
  Returns a 16-bit integer containing the mb value.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL sint15 voc_data_findmb(
  sint31        *mb,
  const uint16  *row,
  sint31        n,
  sint15         scale,
  uint16        target
) {
	const uint16 *base = row;
	const uint16 *end = row+n;
	      sint15  update = scale;

  while (n>1) {
		const uint16* mid = base+n/2;
		int16 test = (int16)(((int32)scale**mid+(1<<14))>>15);

		if (test < target) {
			end = mid;
		} else {
			base = mid;
			update = test;
		}
		n = end-base;
	}
	*mb = *mb << 4| (base-row);
	return update;
}

/*===========================================================================

FUNCTION voc_data_compute_mb_from_abs

DESCRIPTION
  This function returns the mb value from hardware register value.

DEPENDENCIES
  Requires voc_data_mb_table1[],
           voc_data_mb_table2[],
           voc_data_mb_table3[]
RETURN VALUE
  Returns a 32-bit integer containing the mb value.

SIDE EFFECTS
  None.
===========================================================================*/
sint31 voc_data_compute_mb_from_abs(
  uint16 abs_value
) {
  sint31 mb = 0, index, mb2;
  sint15 x;

  if (abs_value == 0x0) {
    return -9600;
  } else if (abs_value == 0x7FFF) {
    return 0;
  } else {

    while (abs_value<2048) {
      abs_value <<= 4;
      mb += 2404;
    }
    while (abs_value<16384) {
      abs_value <<= 1;
      mb += 602;
    }

    if (abs_value==16384) {
      return (-mb-602);
    }

    for (index = 0; index<2 && voc_data_mb_table1[index+1]>abs_value; index++) {
    }
    mb2 = index;
    x = voc_data_mb_table1[index];
    x = voc_data_findmb(&mb2, voc_data_mb_table2, 16, x, abs_value);
    x = voc_data_findmb(&mb2, voc_data_mb_table3, 16, x, abs_value);
  }

	return -(mb+mb2);
}


/*===========================================================================

FUNCTION voc_data_convert_to_qfactor

DESCRIPTION
  This functions takes an absolute value and convers to 16.16 fixed value.

DEPENDENCIES
  None

RETURN VALUE
  Returns a usigned 32-bit 16.16 fixed value.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 voc_data_convert_to_qfactor(
  uint32 value
) {
  uint64 temp_value;

  temp_value = ((uint64) value << 16) / 1000 ;

  if (temp_value > 0x7FFFFFFF) {
    return 0x7FFFFFFF;
  } else {
    return ((uint32) temp_value);
  }
}

/*===========================================================================

FUNCTION voc_data_convert_from_qfactor

DESCRIPTION
  This functions takes 16.16 fixed value and converts to absolute value.

DEPENDENCIES
  None

RETURN VALUE
  Returns a unsigned 32-bit aboslute integer.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 voc_data_convert_from_qfactor(
  uint32 qvalue
) {
 uint64 temp_value;

 temp_value = (((uint64) qvalue * 1000) + 0x8000) >> 16;
                                        /* Add 0x8000 to account for loss
                                          in precision from previous
                                        absolute value to qfactor conversion */
 return ((uint32) temp_value);
}
#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_SBC_DSP_CODEC

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_sbc_config

DESCRIPTION
  This function retrives the sbc encoder parameter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_data_get_sbc_config(qdsp_cmd_sbc_codec_config_type *config_ptr)
{
  uint16 param = 0;
  if (config_ptr)
  {
    param = QDSP_CMD_SBC_PARAM_SBC_ENA_V |
             voc_data_sbc_blocks_bits[voc_data_sbc_blocks] |
             voc_data_sbc_subbands_bits[voc_data_sbc_subbands] |
             voc_data_sbc_alloc_bits[voc_data_sbc_alloc]  |
             voc_data_sbc_mode_bits[voc_data_sbc_mode];

    MSG_MED("Configuring SBC encoder with blocks = %d alloc = %d mode = %d", \
              voc_data_sbc_blocks, voc_data_sbc_alloc, voc_data_sbc_mode);
    config_ptr->param_buf = param;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_sbc_config

DESCRIPTION
  This function saves the sbc encoder params

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
voc_status_type voc_data_set_sbc_config(voc_sbc_config_params_type *config_ptr)
{

  if ((config_ptr->alloc  >= VOC_SBC_ALLOC_MAX) ||
      (config_ptr->blocks >= VOC_SBC_BLOCKS_MAX) ||
      (config_ptr->mode   >= VOC_SBC_MODE_MAX))
  {
    return (VOC_CMD_FAILED_S);
  }
  else
  {
    voc_data_sbc_blocks = config_ptr->blocks;
    voc_data_sbc_alloc  = config_ptr->alloc;
    voc_data_sbc_mode   = config_ptr->mode;
    return (VOC_DONE_S);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_sbc_bitrate

DESCRIPTION
  This function retrives the sbc encoder bitrate

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
uint32 voc_data_get_sbc_bitrate(void)
{
  return voc_data_sbc_bitrate;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_sbc_bitrate

DESCRIPTION
  This function sets the sbc encoder bitrate

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_data_set_sbc_bitrate(uint32 bitrate)
{
  voc_data_sbc_bitrate = bitrate;
}
#endif /* FEATURE_SBC_DSP_CODEC */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_dual_mono_mode

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_data_set_dual_mono_mode(uint16 mode)
{
  MSG_LOW("voc_data_set_dual_mono_mode %d",mode,0,0);
  voc_data_dual_mono_ch_mode = mode;
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_dual_mono_mode

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
uint16 voc_data_get_dual_mono_mode(void)
{
  MSG_LOW("voc_data_get_dual_mono_mode %d",voc_data_dual_mono_ch_mode,0,0);
  return voc_data_dual_mono_ch_mode;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_sbr_mode

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_data_set_sbr_mode(uint16 mode)
{
  MSG_LOW("voc_data_set_sbr_mode %d",mode,0,0);
  voc_data_sbr_mode = mode;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_sbr_mode

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
uint16 voc_data_get_sbr_mode(void)
{
  return voc_data_sbr_mode;
}

#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

/* <EJECT> */
#if defined(FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL) && \
    defined(FEATURE_QTUNES_COMMON)
/*===========================================================================

FUNCTION voc_data_get_module_based_on_mips_reqd

DESCRIPTION
  This function returns an appropriate QDSP_MODULE_xxx based on the MIPs
  requirement of the decoder in use and the post-proc features that are
  currently active.

DEPENDENCIES
  None

RETURN VALUE
  Module

SIDE EFFECTS
  None.

==========================================================================*/
qdsp_module_type voc_data_get_module_based_on_mips_reqd(
        qdsp_module_type                 module,
        uint16                           base_mips,
  const byte                             *pp_mips,
  const voc_data_module_to_mips_map_type *module_to_mips_map
) {
  uint16           total_mips = base_mips;
  uint16           mod_index = 0;

  #ifdef FEATURE_AUDFMT_FIR_FILTER
  if (voc_data_get_qtunes_pcm_filter()->flag) {
   total_mips += pp_mips[VOC_DATA_PP_INTERFACE_FIR];
  }
  #endif /* FEATURE_AUDFMT_FIR_FILTER */

  #ifdef FEATURE_AUDFMT_IIR_FILTER
  if (voc_data_get_audfmt_iir_filter()->enable == TRUE) {
   total_mips += pp_mips[VOC_DATA_PP_INTERFACE_IIR];
  }
  #endif /* FEATURE_AUDFMT_IIR_FILTER */

  #ifdef FEATURE_QTUNES_ADRC
  if (voc_data_get_audfmt_adrc_ctl()->flag) {
   total_mips += pp_mips[VOC_DATA_PP_INTERFACE_ADRC];
  }
  #endif /* FEATURE_QTUNES_ADRC */

  #ifdef FEATURE_AUDFMT_EQUALIZER
  if (voc_data_tbl_eq_ctl.enable    ||
     voc_data_preset_eq_ctl.enable ||
     voc_data_equalizer_ctl.enable) {
   total_mips += pp_mips[VOC_DATA_PP_INTERFACE_EQ];
  }
  #endif /* FEATURE_AUDFMT_EQUALIZER */

  #ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
  if (voc_data_spectrum_analyzer_update_period) {
   total_mips += pp_mips[VOC_DATA_PP_INTERFACE_SPA];
  }
  #endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

  #ifdef FEATURE_AUDFMT_QCONCERT
  if (voc_data_qconcert_enable == TRUE) {
   total_mips += pp_mips[VOC_DATA_PP_INTERFACE_QCONCERT];
  }
  #endif /* FEATURE_AUDFMT_QCONCERT */

  #ifdef FEATURE_AUDFMT_STF
  if (voc_data_get_audfmt_sidechain_filter()->enable == TRUE) {
   total_mips += pp_mips[VOC_DATA_PP_INTERFACE_STF];
  }
  #endif /* FEATURE_AUDFMT_STF */

  #if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  if (voc_bt_a2dp_link_not_disabled() == TRUE) {
   total_mips += pp_mips[VOC_DATA_PP_INTERFACE_SBC];
  }
  #endif /* (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC) */

  /* Assumes module_to_mips_map contains ascending order of MIPS and
    terminated by QDSP_MODULE_MAX */
  while (module_to_mips_map[mod_index].module != QDSP_MODULE_MAX) {
    if (module_to_mips_map[mod_index].mips >= total_mips) {
      module = module_to_mips_map[mod_index].module;
      break;
    }
    mod_index++;
  }

  if ((mod_index != 0) &&
      (module_to_mips_map[mod_index].module == QDSP_MODULE_MAX)) {
    /* Cap it to the module with highest clock rate if we've exceed the max */
    module = module_to_mips_map[mod_index-1].module;
  }
  MSG_HIGH("voc_data_get_qtunes_module() selected module=%d", module, 0, 0);

  return module;
}
#endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL && FEATURE_QTUNES_COMMON */

#ifdef FEATURE_QTUNES
/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_mp3_module

DESCRIPTION
  This function returns the appropriate MP3 modules to use, based on MIPs
  required by the decoder and the post-proc blocks combined.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
qdsp_module_type voc_data_get_mp3_module(void) {
  qdsp_module_type module = QDSP_MODULE_TUNES;

  #ifdef FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL
  #ifdef QDSP_IMAGE_TUNES_DEFINED

  if (voc_state_get_state_data()->current_image == QDSP_IMAGE_TUNES) {
    module = voc_data_get_module_based_on_mips_reqd(
               module,
               MSMAUD_QTUNES_MP3_MIPS,
               voc_data_qtunes_mp3_mips_info,
               voc_data_qtunes_mp3_mtm_map);
  }
  #endif /* QDSP_IMAGE_TUNES_DEFINED */

 #ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
  if ( voc_state_get_state_data()->current_image == QDSP_IMAGE_CAM_AUDIO_CONC ) {
    module = voc_data_get_module_based_on_mips_reqd(
               module,
               MSMAUD_QTUNES_MP3_MIPS,
               voc_data_qtunes_mp3_mips_info,
               voc_data_qtunes_mp3_mtm_map);
  }
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */

  #endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL */

  return module;
}
#endif /* FEATURE_QTUNES */

/* <EJECT> */
#ifdef FEATURE_QTUNES_AAC
/*===========================================================================

FUNCTION voc_data_get_aac_module

DESCRIPTION
  This function returns the appropriate AAC modules to use, based on MIPs
  required by the decoder and the post-proc blocks combined.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
qdsp_module_type voc_data_get_aac_module(void) {
  qdsp_module_type module = QDSP_MODULE_AAC;

  #ifdef FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL
  #ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED

  if (voc_state_get_state_data()->current_image == QDSP_IMAGE_ENH_AAC_PLUS) {
    module = voc_data_get_module_based_on_mips_reqd(
               module,
               MSMAUD_ENH_AAC_PLUS_MIPS,
               voc_data_enh_aac_plus_mips_info,
               voc_data_enh_aac_plus_mtm_map);
  }

  #endif /* QDSP_IMAGE_ENH_AAC_PLUS_DEFINED */
  #endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL */
  return module;
}
#endif /* FEATURE_QTUNES_AAC */

/* <EJECT> */
#ifdef FEATURE_QTUNES_COMMON
/*===========================================================================

FUNCTION voc_data_get_qtunes_module

DESCRIPTION
  This function returns the appropriate Qtunes modules to use, based on MIPs
  required by the decoder and the post-proc blocks combined.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
qdsp_module_type voc_data_get_qtunes_module(voc_capability_type voc_cap) {

  qdsp_module_type module = QDSP_MODULE_MAX;

  switch (voc_cap) {
    #ifdef FEATURE_QTUNES
    case VOC_CAP_QTUNES:
      module = voc_data_get_mp3_module();
      break;
    #endif /* FEATURE_QTUNES */
    #ifdef FEATURE_QTUNES_AAC
    case VOC_CAP_QTUNES_AAC:
      module = voc_data_get_aac_module();
      break;
    #endif
    default:
      break;
  }
  return module;
}

/*===========================================================================

FUNCTION voc_data_current_image_supports_dcs

DESCRIPTION
  Checks if the current image support the DCS interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/
boolean voc_data_current_image_supports_dcs(void) {
  boolean status = FALSE;

  #ifdef FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL
  switch (voc_state_get_state_data()->current_image) {
    #if defined(QDSP_IMAGE_TUNES_DEFINED) || \
        defined(QDSP_IMAGE_ENH_AAC_PLUS_DEFINED) ||\
        defined (QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED)
    #ifdef QDSP_IMAGE_TUNES_DEFINED
    case QDSP_IMAGE_TUNES:
      status = TRUE;
      break;
    #endif
    #if defined(QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED)
    case QDSP_IMAGE_CAM_AUDIO_CONC:
      status = TRUE;
      break;
    #endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED*/
    #ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
    case QDSP_IMAGE_ENH_AAC_PLUS:
      status = TRUE;
      break;
    #endif
    #endif

    default:
      break;
  }
  #endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL */
  return status;
}
#endif /* FEATURE_QTUNES_COMMON */


/*===========================================================================

FUNCTION voc_data_get_current_codec

DESCRIPTION
  Returns the current codec_inuse

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/
extern voc_codec_type voc_data_get_current_codec()
{
    return voc_data_codec_inuse;
}


/*===========================================================================

FUNCTION voc_data_set_idle_metering

DESCRIPTION
 sets variable idlemeteringflag....

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/
void voc_data_set_idle_metering(uint16 flag)
{
  voc_data_idle_metering_flag = flag;
}


/*===========================================================================

FUNCTION voc_data_get_idle_metering

DESCRIPTION
returns the voc_idle_metering_flag variable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/
uint16 voc_data_get_idle_metering(void)
{
  return voc_data_idle_metering_flag;
}

/*===========================================================================

FUNCTION voc_data_set_eclock_switched

DESCRIPTION
sets the device_changed variable of vocoder state data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/
void voc_data_set_eclock_switched(boolean change)
{
  voc_state_control_type     *state;
  state = voc_state_get_state_data();
  state->device_changed = change;

}

/*===========================================================================

FUNCTION voc_data_get_eclock_switched

DESCRIPTION
returns the device_changed variable of vocoder state data.

DEPENDENCIES
None

RETURN VALUE
True:device_changed from external to internal  False: device is not changed

SIDE EFFECTS
None.

==========================================================================*/
boolean voc_data_get_eclock_switched(void)
{
  voc_state_control_type     *state;
  state = voc_state_get_state_data();
  return( state->device_changed);
}

#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION voc_set_adpcm_params

DESCRIPTION
  This function saves the ADPCM params in voc_state_data

DEPENDENCIES
  None

RETURN VALUE
  VOC_DONE_S

SIDE EFFECTS
  None.

==========================================================================*/
extern voc_status_type voc_set_adpcm_params(
  uint32                        channel,
  voc_adpcm_play_enum_type      play_type,
  voc_adpcm_spec_type           *spec,
  voc_adec_start_mode_enum_type start_mode
)
{
 voc_state_control_type* state;
 state = voc_state_get_state_data();

 state->adpcm_num_channels = channel;

 /* Update the adpcm_num_channels as contained in spec->numchannels */
 state->adpcm_num_channels = spec->num_channels;
 state->play_type = play_type;
 state->bits_per_sample = spec->bits_per_sample;
 state->adpcm_block = spec->block_length;
 state->adpcm_sampling_rate = spec->sampling_rate;
 state->bits_per_sample =spec->bits_per_sample;
 state->start_mode = start_mode;
 state->adpcm_mode = spec->type;

#ifdef FEATURE_VOC_ADPCM
 if (VOC_GRAPH_ADPCM_VALID_CHANNELS(spec->num_channels)           &&
       VOC_GRAPH_ADPCM_VALID_BITS_PER_SAMPLE(spec->bits_per_sample) &&
       VOC_VOC_32K_ADPCM_VALID_SAMPLE_RATE(spec->sampling_rate)     &&
       VOC_VOC_32K_ADPCM_VALID_BLOCK_SIZE(spec->block_length) ){
	 state->adpcm_voc_config = TRUE;
 }
#endif /* FEATURE_VOC_ADPCM */

 state ->adpcm_start_pending = TRUE;

return VOC_DONE_S;

}
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM  */


/*===========================================================================

FUNCTION voc_data_get_vp3_buf_id_and_offset

DESCRIPTION
  This function gives the BUF_ID and the buffer offset based on the
  current mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void* voc_data_get_vp3_buf_id_and_offset
(
  vocvp3_buf_id_type* buf_id
)
{
  void* buf_ptr = NULL;
  voc_ec_type *ec_mode = voc_data_get_ec_mode();
  qdsp_cmd_ec_gen_id_type ec_gen_id = qdsp_cmd_get_ec_gen_id();

  if (ec_gen_id == QDSP_CMD_EC_GEN_NEXT )
  {
    *buf_id = voc_data_ecns_mode_to_buf_id[*ec_mode];
    buf_ptr = vocvp3_get_buf_address(*buf_id);
  }
  else if (ec_gen_id == QDSP_CMD_EC_GEN_CRYSTAL_SPEECH)
  {
     *buf_id = voc_data_fluence_mode_to_buf_id[*ec_mode];
     buf_ptr = vocvp3_get_buf_address(*buf_id);
  }
  return buf_ptr;
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_buf_id_to_buf_size_table

DESCRIPTION
  This function gives the Size of the buffer given the BUF_ID

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
unsigned int* voc_data_get_buf_id_to_buf_size_table
(
void
)
{
  return voc_data_vp3_buf_id_to_buf_size_table;
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_get_buf_id_to_default_data_table

DESCRIPTION
  This function gives the default buffer table given the BUF_ID

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint16** voc_data_get_buf_id_to_default_data_table
(
  void
)
{
  return voc_data_vp3_buf_id_to_default_data_table;
}


/*===========================================================================

FUNCTION voc_data_get_service_option

DESCRIPTION
  Call this function to get the current service option.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
voc_service_cfg_type voc_data_get_service_option(void) {

  return(voc_data_call_service);
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_data_set_service_option

DESCRIPTION
  Call this function to set the latest service option.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_data_set_service_option(voc_service_cfg_type service) {

  voc_data_call_service = service;

}


#ifdef SND_TEST_AUXPCM_INTERFACE_MODES
#error code not present
#endif

/*===========================================================================

FUNCTION voc_data_set_slowtalk_enable_flag

DESCRIPTION
  This function enable/disable the global slowtalk setting

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_set_slowtalk_enable_flag ( boolean enable)
{
  voc_data_slowtalk_enable_flag = enable;
}

/*===========================================================================

FUNCTION voc_data_get_slowtalk_enable_flag

DESCRIPTION
  This function return the current status of the slowtalk feature.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_data_get_slowtalk_enable_flag ( void )
{
  return (voc_data_slowtalk_enable_flag);
}

/*===========================================================================

FUNCTION voc_data_set_slowtalk_static_params_status

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void voc_data_set_slowtalk_static_params_status (
  voc_slowtalk_static_params_status_type flag
)
{
  /* check if the flag is out of range? */
  if (flag >= VOC_SLOWTALK_STATIC_PARAMS_MAX )
  {
    MSG_ERROR("slowtalk: wrong static_params_status = 0x%x", flag, 0, 0);
    voc_data_slowtalk_static_params_status = VOC_SLOWTALK_STATIC_PARAMS_MAX;
    return;
  }

  voc_data_slowtalk_static_params_status = flag;
}

/*===========================================================================

FUNCTION voc_data_get_slowtalk_static_params_status

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
voc_slowtalk_static_params_status_type
voc_data_get_slowtalk_static_params_status (void)
{
  return (voc_data_slowtalk_static_params_status);
}
/* End of vocdata.c */

/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("QDSP Device Driver")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Q D S P 2   V O C O D E R   D R I V E R
                       S T A T E   C O N T R O L L E R

GENERAL DESCRIPTION
  Controls the state of the QDSP Vocoder DSP.  The QDSP Maintains the
  following internal states:

      RESET ---> IMAGE ---> INIT <----> IDLE <-----> VOCODER
                              ^           |  <-----> DFM
                              |           |
                            SLEEP         |
                              ^           |
                              |-----------|

  This module, via the voc_state_control function, drives the QDSP through
  the required state transitions and monitors the results.

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS
  voc_state_get_state_data
    Returns a pointer to the state control structure.
  voc_state_get_inform_val
    Returns the current state, converted to user value.
  voc_state_control
    The QDSP State controller.
  voc_state_control_init
    Initializes the state controller.
  voc_state_enable_eclock
    Enables vocoder and codec external clocks.
  voc_state_disable_eclock
    Disables vocoder and codec external clocks.
  voc_state_enable_clocks
    Enables all vocoder and codec clocks.
  voc_state_disable_clocks
    Disables all vocoder and codec clocks.
  voc_state_set_codec_cal
    Sets the calibration for the codec specified.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 1999 - 2009 by QUALCOMM, Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/multimedia/audio/avs/src/vocstate.c#5 $ $DateTime: 2010/09/26 23:15:28 $ $Author: asifmd $

when       who     what, where, why
--------   ---    -------------------------------------------------------------
09/27/10   aim     Modified code so that BT callback is done with PLAYBACK_SCO if 
                   A2DP is not supported by FW for the current image.
04/21/10   knm     Fixed pop noise issue with FM recording by disconnecting
                   Rx filter before configuring iclock.
03/31/10   aim     when the image loaded is QDSP_IMAGE_CAM_AUDIO_CONC and we are not 
                   playing any mp3 or ADPCM file the icodec clk is set to 2.048 Mhz 
                   which is required for 8Khz sample rate.
03/23/10   skr     Modified code to send ADPCM decoder start command when ADPCM 
                   playback other than 32Khz, 4bits/sample, 512 blocksize,
                   1channel is initiated when voc_state is in CAM_AUDIO_ACTIVE . 
10/26/09   sud     Removed dependency on feature FEATURE_VOC_AMR_WB for feature
                   .AWB file playback.
                   Updated Copyright.
10/26/09   knm     Fixed compilation error & warnings.  
10/23/09   vsud    Added enable/disable Midi HW clock changes for SCMM.
10/15/09   knm     Integrated new marimba codec driver for SCMM
10/15/09   knm     Removed unfeaturized clk_busy_wait().
10/08/09   knm     Fixed compilation issue in voc_state_adie_set_final()
10/06/09    sj     Added SCMM specific changes
09/16/09    gs     Added support for FM on Handset, Lineout
09/07/09   vsud    Changes for resampler image imeplementation.
07/31/09   rm      snd_expire gets called soon whenever present image does not
                   support its module in dsp. relaxing sound expire by 100ms.
                   [CR 189921 - Sound mute issue (VOC_CMD_BUSY_S)]
07/27/09   rm      Modified iir filter configuration functionality
07/23/09   rm      Fixed CR 184851 - cmx_qcp_record_reverse can't record
07/16/09   kk      Fixed CR 187581 - av clips with h264 and with vocoder audio
                   are freezing.
07/13/09   rm      Adding support for 4GV_WB_VoIP.
07/09/09   kkm     During device switch ecodec clock turning off in isr is removed, as rex dont
                   support critical sections in it.
07/07/09   rk      Notifying SND that PAs are turned on, wakes up sound task.
07/07/09   rm      Added changes for QCamCorder Turbo image
06/05/09   sud     service config is updated in voc_state_idle when module
                   select status is VOC_DONE_S. fix for CR#179727
6/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
06/03/09   kkm     Reverted back "modified to select Qsynth image for ADPCM Media Playback.
                   Support added for ADPCM to play on BT headset" because Camera shutter sound is not stopping
                   and preview is freezing
06/01/09   knm     Added hiss noise fix while changing the icodec clocks
05/27/09    rm     Removed comppilation error due to presence of
                   VOC_CODEC_MODE_CDMA_4GV_WB_VOICE
05/21/09    kk     Added support for H264 QCIF playback support
                   for low power mode(TURBO).
05/13/09    rm     EVRC-WB and IIR filter table related updates
05/07/09    kk     Fixed feature issue in H264 clips playback support.
05/05/09   knm     Added pop fixes for ULC.
05/05/09    kk     Added support for H264 clip playback.
05/05/09   kkm     modified to select Qsynth image for ADPCM Media Playback.
                   Support added for ADPCM to play on BT headset
28/04/09    kd     Fixed side effects caused due to dtmf tones on top of
                   the fix for CR-167490.
04/20/09    rm     CR 168030, handling voc_state_get_high_module
04/08/08    sj     Updating the module_status to
                   VOC_STATE_MOD_STATUS_ENABLE_REQUESTED when we call
                   qdsp_enable(). fixed CR 177577.
01/28/09    av     Adding VP3 Feature.
                   Support for TxPCM filter before EC
                   Updated Copyright
04/08/09   sk      Modified the code to register the cpu client to bump up the
                   clock when Qsynth image is loaded.CR 167823.
                   Added client CLKRGM_CPU_CLIENT_AUDIO_PLAYBACK
04/07/09   knm     Set the delay as 20ms and set the codec rx gain to zero in
                   voc_state_adie_wakeup() & turned off the PAs.
04/07/09   knm     Setting the master volume directly from synth entry to resolve
                   the issue- 1s audio during playback with zero volume.
19/02/09   kd/knm  Fixed CR-167490,sending midi commands before setting
                   the rx and tx gains.
03/23/09   knm     Replaced 3ms delay with 5ms delay to reduce the pop noise
                   level in LCU.
03/23/09   knm     Turned off side tone gain for DTMF playback.
03/23/09   knm     Added fix to reduce the pop noise level.
03/23/09   skr     Fix for Camera + MT call concurrecy issue after DTMF support
                   for Audio + Camera Concurrency.
02/26/09    rm     Synchronized code with changes in clock regim
01/19/09   anb     Modified voc_state_qdsp_event_handler so that QTV can
                   be informed about mod change(image swap) event.
                   Fixes Cr 163807(SR-1133583)
03/12/09   skr     Fixed compilation error for Low tier and mid tier ULC build
03/10/09   sj      Added slowtalk feature support
01/29/09   skr     State machine changes to support DTMF in camera preview mode
                   for Audio Camera Concurrency.
01/19/09    dp     CR 168251 Removed feturization for FEATURE_BT_QSOC
01/07/09   skr     Fixed Compilation issues for Mid Tier ULC build
12/29/08    rm     Added RVE API changes
12/29/08    sj     Added .awb file format support
12/23/08   vm      Added test code for AUXPCM testing support
16/08/08   skr     use ACC_ADPCM module for files having 1 channel,4bits/sample
                   512 block size , and 32Khz sampling Frequency.
12/10/08   knm     Used adie_services_set_adie_sleep_status()to set or reset the
                   Adie sleep status.
12/10/08   skr     Fixed CR 166169 - Audio/CMX: Crash (vocstate.c) observed
                   after playing the PCM file .
12/08/08   knm     Turned off Assert condition in error recovery.
12/02/08   av      Fix for CR 150923.
                   Included support for QCAMCORDER image in voc_state_graph_idle to set amr_tx_rate.
11/14/08   sj      Don't enable ECNS when we don't require it. Fixed CR 160542
11/26/08   skr     Fixed compiler warning
11/25/08   rm      CR 153432 - Modified voc_state_idle() to enable the transition from IDLE
                   to SLEEP state on module changes.
11/25/08   skr     Fixed Issues reported by SU1 regression team related to
                   Audio+Camera Concurrency.
11/24/08   knm     In voc_state_control_init() hclk restriction is set to HCLK_OFF
                   for ULC.
11/24/08   skr     Added code for Audio + camera Concurrency
11/18/08   knm     Added adie_services_init() API call in voc_state_control_init()
                   and FM on USB code.
11/18/08   dp      Added support for playing NB-AMR on A2DP
11/18/08   dp      Featurization related fix for Prepaid build
11/12/08   knm     Added code to collect ADSP Dump in error recovery.
11/11/08   skr/anb Support for BT-SCO for .3g2 and .qcp clips, Fixes CR 158679
11/05/08    dp     Fixed [SR=1063797] Some wav file is not played via Bluetooth SCO
10/31/08   skr     Fixed compilation issues if custwm.h and
                   FEATUE_AVS_INCALL_WMA are undefined.
10/22/08   dp      FM over BT AG changes
10/17/08   vm      added idlemetring support for qtv image
10/15/08   sk      Reverting CR fix 149646.LCU don't need to vote for
                   AUDIO_LCU_PMIC_CDC2LDO
10/14/08   anb     Re-setting codec gain mutex in adie_sleep.
10/04/08   skr     Setting default samplerate for aac,wma,ra to 8KHz
10/14/08    sk     Modifed the code to fix Lint warning for LCU votings on
                   AUDIO_LCU_PMIC_CDC2LDO,AUDIO_LCU_PMIC_RFLDO.
10/13/08    sk     Modfied the code to vote the CD2/LDO for LCU before using
                   the iternal codec CR 149646.The changes are Featurised
                   under AUDIO_LCU_PMIC_CDC2LDO.
09/11/08   dp      featurization related Correction to voc_state_enable_clocks function
                   CR 151562
09/02/08   sud     Fixed target crash while booting.
08/30/08   vsud    fixed compilation errors due to WMA PRO
08/29/08   knm     Fixed problem of taking snapshot continously with shutter sound failed.
08/28/08   rm      Added support for RVE
08/27/08   sk/az   Modified the code to enable/disable CLK_RGM_MIDI_HCLK_M for
                   Qsynth playback.
08/26/08   anb/az  Added support for DSP based WMAPRO
                   Removed the comments added from merge tool in #13
08/25/08    sk     Modfied the code to vote the RF/LDO for LCU before using
                   the iternal codec CR 152541.The changes are Featurised
                   under AUDIO_LCU_PMIC_RFLDO.
08/07/08    sj     Fixed CR 146723 - Disabe PCM interface is not working
                   properly
07/29/08    az     Fixed the compilation issue.
07/30/08    sk     Featurised FEATURE_SPEAKER_PHONE when codec is
                   VOC_CODEC_SPEAKER.
07/30/08    kk     Featurised the support to play Qtv 4gv nb clips.
07/23/08    dp     Fixed CR: 149857
07/17/08    kk     Added support to play Qtv 4gv nb clips.
06/20/08   knm     Added support for AuxPGA Line In path configuration
05/26/08   knm     Fixed CR 148005: Audio is not played because Codec Gain
                   cannot be set. Resetting codec gain mutex whereever required.
05/14/08    sr     Modified qdsp_cmd_codec_cofig() to include default_frame_
                   rate.Fix for CR 134407 - 16khz PCM sampling.
04/01/08   wfd     Increment and Decrement voc_state_eclock_count only for app
                   client requests for eCodec Clock enable/disable.
03/17/08  ymc/anb  Modified codec gain ctrl api to control gain changes.
                   Fix for SC2X codec pop issue through MSMAUD_PCM_DC_OFFSET feature.
03/11/08    kk     Added support for sending mp3 configuration
                   information for DSP.
01/31/08    pl     Fixed CR 127249 - manifest qsynth interrupt when QDSP_MODULE_QSYNTH
                   is enabled while DSP is still in qsynth state.
01/30/08    az     Include the support for the In call Qtv.
01/30/08    hs     Fixed Clockworks warnings
01/23/08    vs     Turn off adsp macro clock to save power when adsp becomes idle.
01/17/08    az     Fixed the CR: 136447 Incall supported decoders (MP3, AAC)
                   cannot be decoded while in a call and when Incall Mixing is
                   disabled.
01/15/08    hs     Added support ISDB-T dual mono output control
01/07/08    az     Included the in call midi and the new in call architecture.
12/18/07    sj     Removed fetaure FEATURE_MULTIMEDIA_CODEC_GAIN from
                   custavs.h
                   so call to voc_state_write_codec_gain() function
                   is no longer featurized now.
11/25/07    hs     Fixed ThinUI build compiling error
11/21/07    hs     Fixed sound lost after switching audio device when play 32Khz ADPCM
11/21/07   anb     Fixed compilation errors for SC2X prepaid build
11/19/07   anb     Moved HCLK restriction api to voc_state_control_init
                   Fixes CR-128607
11/15/07    az     Featurized the code for In call to avoid compilation for
                   other targets.
11/12/07   ymc     Fixed DSP isr table not cleaned up during shutdown.
11/02/07    az     Included the support for the in call wma.
10/26/07    az     Included the support for the in call AAC.
10/15/07    az     Included support for the crystal speech for in call audio.
09/29/07   hs      Fixed featurization problem for aacenc.
09/26/07   ymc     Fixed featurization problem for 4GV_WB and AMR_WB.
09/14/07  az/anb   Fixed the CR: 126185 - AAC+ got high current at 56mA. Reduced
08/31/07   anb     Fixed the Compile issue wrt voc_state_get_pp_status()
08/31/07   anb     Fixed CR 115823 - SCO link is not disconnected after ending clip type AMR
08/30/07   ymc     Added crystal speech support.
           ymc     Fixed missing AACENC featurization problem.
08/30/07   wfd     Correct compile errors: featurize voc_state_get_pp_status()
                   with #ifdef FEATURE_ARM_POST_PROCESSING ||
                   FEATURE_AVS_WB_PCM_RX_INTERFACE.
08/23/07   ymc     Fixed further power up and down codec pops.
08/20/07   hs      Support AAC stand alone encoding
08/20/07   hs      Fixed compiling error, use clk_regime_enable for no dual clk
08/11/07   wfd     Modified logic that determine when eclock is enabled to avoid enabling
                   the eclock while it is already enabled.
08/08/07  anb/wfd  Added aux_pcm_clk to be enabled/disabled from other tasks like BT (CR 122106).
07/28/07   anb     Removed reference to PM_HSED_ENABLE_OFF so that we dont disable micbias
                   during sleep which is also require by HSED
07/27/07   ymc     Fixed codec power up pop.
           ymc     Fixed current configuration for wideband Tx.
07/02/07    pl     Called qdsp_cmd_reset_adec_cmd_buf() in voc_state_qsynth()
                   to clear qsynth adec command buffer when vocoder state machine
                   transitions to sleep state.
06/13/07    sg     Added support for selection of the AAC module based on MIPs
                   requirements.
06/08/07    sg     Added support for selection of the MP3 module based on
                   the post-proc features that are currently enabled.
05/28/07    pl     Modified to use new SLEEP API
05/14/07   sud     EPIC Vreg_NCP support in ground referenced capless mode
                   for QSC60x5 chipsets.
05/08/07    az     Fixed the CR:107845 QCamcorder - The preview becomes blank &
                   then not able to record the clip.
05/07/07    sp     Added code so that SBC encoder is reneabled if the DSP
                   state changes to from QTUNES -> QTUNES_INIT ->...-> QTUNES.
                   Fix for CR 117660.
04/23/07    ay     Modified voc_state_init() to check for module acquired
                   before updating "voc_state_data.config" (SR918985).
04/20/07  sud/anb  Added PMIC MIC-BIAS support for QSC60X5.
04/13/07   sud     Added support for independent control of Tx and Rx codec
                   clock for Saturn codec.
04/10/07    sg     Modified to acknowledge pending MOD_DISABLE events after
                   the error recovery threshold has been reached (CR112743).
03/28/07    az     Included the changes for the CR: 110044 and CR: 103303 BT
                   Headset (SBH100) noise while SCO Disconnecting.
03/15/07    sp     Moved BT SCO/A2DP disable from voc_state_qtunes to
                   voc_state_qtunes_init (when the vocoder goes to RESET or
                   SHUTDOWN. This prevents the SCO/A2DP from toggling when
                   DSP changes state for configuration purposes.
03/15/07    kan    Added 4GV-WB Code
02/27/07   ymc     Renamed QDSP_CMD_EC_GEN_LEGEND as QDSP_CMD_EC_GEN_LEGACY.
02/26/07   kan     Added AMR-WB changes
02/09/07   act     Modified code to enforce no AGC/AVC during UMTS TTY.
                   Added function call to signal PCM interface that a
                   discontinuity has been detected.
01/23/07   suk     Passed active module as parameter to qdsp_cmd_set_image()
                   in addition to current image.
01/10/07    sg     Added support for QTUNES_DISABLE_PENDING state to allow
                   retries on the SLEEP command.
                   Fixed race condition where voc_inform_get_val() may return
                   the previous state even though we've just issued the sleep
                   cmd to the DSP (Partial fix for 106929).
                   Fixed race condition that can occur when vocstate is waiting
                   for VOC_CHECK_STATE_SIG after calling qdsp_enable(), and is
                   depending on QDSP services to be the only entity that sets
                   the signal in this state. The race condition can occur when
                   SND issues any asynchronous command which also sets the same
                   signal (CR108280).
01/09/07   suk     Added QDSP_IMAGE_VOC_COMBO_VOIP support.
                   Updated Copyright text.
12/07/06   ymc     Added VOC_EC_BT_HEADSET support.
                   Added VOC_CODEC_ON_BOARD to voc_state_aux_pcm_clk_query().
           ymc     Fixed AUX PCM clock loss due to DSP image swap.
12/02/06   suk     Added support for VOIP Combo Vocoder, and Combo vocoder
                   with G711 and 4GV.
11/28/06    sp     Fixed a AUDIO over SCO bug and SCO rework bug for QCP files.
11/22/06    az     Added Incall Audio states to voc_state_get_inform_val().
11/20/06    sp     Added support for OEMSBL Audio.
09/11/06    ak     Made changes in voc_state_get_high_module, as part of
                   vocoder modularization. These changes should be picked
                   only with new qdspext.cfg which has modules split in vocoders
                   as DEC, ENC and full(no postfix, name same as prev). These
                   changes are not backwards compatible to the qdspext.cfg
                   with only a full module in a vocoder image description.
09/08/06    pl     Added 44K clock support to voc_state_config_external_sdac()
                   and voc_state_set_icodec_clk().
09/06/06    ak     Changed the arguments for qdsp_cmd_set_pcm_modes()
09/05/06    ay     Added AMR-WB+ support.
09/01/06    hz     Fixed CR#100139 -- No volume control for ADPCM in camera
                   preview mode.
08/31/06    hz     Added support for incall MP3 on 65nm platforms.
08/30/06   ymc     Fixed audio loss due to adie stayed unplugged after woke-up
                   from sleep when there was no adie config change.
08/20/06   ymc     Fixed general codec pop noise issue.
07/31/06    sp     Fixed some merge issues in AV Sync feature and
                   some issues in SCO rework feature
07/27/06   anb     Added the FEATURE_AVS_BT_SCO_REWORK which enables the AVS to send
                   messages to the BT driver
07/26/06  nm/ymc   Added FEATURE_AVS_NEXTGEN_EC.
07/26/06  nm/ymc   Fixed issue of tx rate factor not initialized properly
                   in 1x voice call.
07/20/06   anb	   Added support for FEATURE_AVS_AUDIO_DECODER_SCO
07/12/06    hz     Added support for incall mp3 GSM/UMTS, BTSCO and PostProc
                   features.
06/27/06    pl     Modified voc_state_qsynth_init() to apply 3D output mode and
                   reverb effect by default.
06/15/06    sp     Fixed a linker error
06/15/06   sk/sp   Fixed CR 95512 BT worldsoccer issue in Mainline.
06/02/06    hz     Added fixed for CR#75189. Before disconnecting BT link,
                   use default codec to enable adie, added function
                   voc_state_bt_adie_wakeup and splitted voc_state_adie_wakeup.
06/01/06    sg     Modified so that FEATURE_ADPCM and FEATURE_WAVE_PB can be
                   disabled while allowing G711 vocoder support.
04/26/06   act     Added support for combo vocoder image.
04/14/06   act     Added 4GV case to voc_state_test_mode_config().
04/05/06   act     Modified voc_state_test_mode_config() to cycle through
                   available vocoders.
04/03/06    ak     Added support for sidetone control.
04/02/06    sg     Deleted support for obsolete FEATURE_FAST_QDSP.
                   Unfeature-ized FEATURE_QDSP_USES_MODULES and removed support
                   for the old-style QDSP services.
                   Added support to reset the DSP if we detect a firmware
                   error during state transitions. The DSP will be reset only
                   if no other higher priority client is active.
03/15/06   ymc     Added FEATURE_INTERNAL_SADC support.
03/15/06    ay     Fixed SADC clkregim featurization compile issue.
03/14/06    sp     Fixed I2SSBC noise issue by calling a clkregim function
                   inorder to work properly with CLK Manager.
03/13/06    lg     Fixed race condition between MIDI command timer and
                   DSP image swapping.
03/09/06    hz     Added support for evrc in-call MP3 support.
03/02/06    ay     Modified to inlude SADC headers when FEATURE_EXTERNAL_SADC
                   is enabled--fixes a compile issue for the I2S SBC interface.
02/18/06    hz     Added fixes for in-call mp3.
02/03/06    pl     Changed featurization from T_MSM6550 to FEATURE_AVS_E90.
02/03/06    sm     Added 4GV VOIP support
01/31/06    lg     Added support for 6550-90nm images
01/31/06    hz     Added support for in-call mp3 feature.
01/30/06    sp     Added support for I2S SBC interface.
01/19/06    sp     Fixed a bug that could cause A2DP not to be disconnected
                   when MIDI playback ends.
12/09/05    hz     Added support for QVP_H263P3_EVRC image.
12/07/05    sp     Made changes so that codec config is issued only in idle
                   state when enabling BT.
11/28/05    hz     Added fixes for CR73855.
11/21/05    hz     CR78442: Added 32KHz ADPCM support for UMTS vocoder codecs.
11/14/05    hz     Clarified compilation warning.
11/11/05    pl     Featurized voc_state_data.adec_mode_pending in
                   voc_state_sleep when it is being assigned
                   VOC_QSYNTH_ADEC_MODE_ADPCM under FEEATURE_QSYNTH_ADPCM.
11/06/05    sg     Modified to support dual mode clk rgm apis.
10/21/05    st     Modified code to return INFORM_RESET if in QSYNTH state but
                   module_status is DISABLE_PENDING.
09/23/05    sp     Fixed a compiler error that occurs when FEATURE_SBC_CODEC is
                   not defined.
09/18/05    sp     Added support for DSP based SBC encoder.
09/18/05   pl/st   Modified code to support new audio decoder API; modified
                   code to enable 3D positional interface.
09/16/05    hz     Added in-call QTV image support.
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
08/31/05    sm     VOX support
08/15/05    sp     Made change so that BT AG is enabled when device is set to
                   BT AG headset and unmuted.
08/11/05    hz     Added support for DTX and data requrest time for AMR
                   recording.
08/10/05    sm     Added support for MPEG4 G723 firmware.
07/28/05    hz     Change codec clock setting sequence in voc_adpcm state.
07/22/05    sm     Added Wav A/ULaw support.
07/15/05   act     Added codec gain write to voc_state_qsynth_entry and
                   voc_state_qtunes_entry.
07/13/05    hz     Fixed a bug when transit to VOC_STATE_ADPCM_VOCODER_ENTRY.
07/07/05    aw/sp  Modified voc_state_qsynth_entry and voc_state_qtunes_entry
                   to do pp initialization after voc_inform.
07/05/05    sm     Added G723.1 support.
06/23/05    sm     TTY updates for auto-bypass and VCO/HCO
06/10/05   act     Added call to disable tty processing on
                   transition to SLEEP.
06/03/05    sg     Fixed compilation problem in voc_state_graph_idle.
06/02/05    sg     Fixed compilation problem in graph_init.
05/31/05    aw     Added support for H264D_AMR image.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/27/05    at     Modified to signal the state machine any time the number of
                   high priority clients that are using an image changes
                   (CR64758).
05/24/05    hz     Added support for MultiMedia/AAC record functionality.
05/17/05    sg     Modified to ignore MOD_READY that may show up immediately
                   after qdsp_disable() is called due to a race condition.
                   This occurs when qdsp_enable() and qdsp_disable() are
                   called in rapid succession. Since vocstate does not track
                   the previously called enable(), it mistakenly assumes that
                   the MOD_READY was unexpected (CR56287).
05/16/05    hz     Added support for Vocoder image+ADPCM+VFE.
04/28/05   ymc     Added 4GV NB support.
04/23/05    sg     Modified to set TX pcm filter for AUDDUB images.
04/22/05    sg     Added Audio Front End init for graphic images
04/21/05    hz     Added VirtualPacketLength support for Windows Media
                   playback.
04/21/05   act     Changed name of voc_codec_type to voc_codec_class_type.
                   Reverted name of voc_codec_id_type to voc_codec_type.
04/18/05    sm     Added GSM/WCDMA TTY support.
04/11/05    sg     Modified to use voc_data_set_codec_cal() instead of
                   voc_data_set_codec which would cause the codec set by SND to
                   be overwritten. Modified to use voc_data_image_supports_bt().
04/01/05   act     Changed use of specific codec IDs to generic codec types
                   where applicable.
03/28/05   act     Added checks before applying config_pending so that state
                   machine does not assume incorrect capability.
03/02/05    aw     Added support for Qconcert; preset EQ bands; STF;
                   post-proc support for synth.
02/23/05   act     Corrected a problem with ADIE config writing when
                   FEATURE_AUDIO_CALIBRATION is enabled.
02/22/05    sm     Added G.711 vocoder support.
           ymc     Used correct state transition time for QVP images.
02/15/05   act     Modified voc_state_set_ec() and voc_state_init_aud_process()
                   to pass EC parameters to QDSP layer.
02/15/05    sm     Added FEATURE_IIR_FILTER
02/14/05    hz     Added support for WMA decode under QDSP_IMAGE_WM_DEFINED,
                   new vocoder state introduced as VOC_STATE_QTUNES_CONFIG
                   for WMA configuration setup.
02/10/05    aw     Added support for H264D_EVRC, QVP_MPEG4_AMR, QVP_MPEG4_EVRC
                   images.
12/29/04    sp     Clean up post processing code. Moved most of the image checks
                   to qdspcmd.c
12/03/04    sm     Added FEATURE_VOC_DTMF_DETECT, removed FEATURE_USB_ISOC_AUDIO
12/01/04    aw     Added support for MP4D_AAC_NO_VPP image. Modified code in
                   voc_state_qsynth to avoid unnecessary state transition.
            sg     Added support for Real Audio Module.
11/19/04    sp     Added support for Arm Post Processing.
                   Modified code to support MIDI On BT AG device.
                   Added equalizer support on QSynth image.
                   Added support for setting larger state transition periods
                   for QVP images.
            sg     Added support for QTV and QTV AAC images.
11/05/04    sg     Fixed syntax error.
11/04/04    aw     Added support for DFM SAT on AAC image.
            sg     Modified set audio_mode correctly based on current image.
11/02/04    aw     Modified code to only do vr_codec_config in VRBC/PUFE mode.
10/27/04    aw     Added support for DFM SAT on QTunes image.
10/15/04    hz     Added support for QDSP_IMAGE_MP4D_AAC_PLUS.
10/14/04    sg     Modified to respond to a QDSP_IMAGE_RESET in RESET state
                   also, so that extra state transitions can be avoided if
                   the consequence of allowing reset is going to cause the
                   image to change. Fixed to set correct QDSP state during
                   error recovery. Modified so that the DSP is reinitialized if
                   the AAC song config changes.
10/14/04   ymc     Fixed BT call connection problem.
09/22/04    aw     Added support for in call viewfinder and clean up obsolete
                   code.
            sg     Modified to debounce the state machine from ERROR_REC. state
                   if some other QDSPS client causing an image swap (module
                   based only); Modified to always issue only vr_codec_config
                   for VR images.
09/14/04    aw     Modified code to config correct voc_timing for QTV+VOC image.
08/19/04  sg/sm    Improve performance of state timeline with
                   FEATURE_VOC_FAST_TIMELINE.  Added voc_config_standby
                   functionality.
08/12/04    sg     Modified so that the state machine switches correctly to
                   images that do not satisfy the pending config, so that basic
                   audio can be supported (module-based state machine only).
08/11/04    sg     Modified so that the voc_adec_isr is set correctly if we
                   switch rapidly from one MP3 to another, while the state
                   machine is still in QTUNES state. Modified to allow ADPCM
                   shutter sounds to be played while in a voice call.
06/23/04    aw     Removed the check interlock for VOC_NOT_RESET in
                   voc_dtmf_cmd for MP4D_AMR_EVRC_V13K image.
06/17/04    aw     Modified code to call equalizer and spectrum analyzer in
                   voc_state_qtunes. and Modifide voc_state_get_high_image to
                   use MP4 image for if evrc/13k image is not supported.
06/08/04    aw     Added support for Equalizer and Spectrum analyzer.
                   Removed the check for AAC+ image in voc_state_set_icodec_clk
                   The check is moved to voc_data_get_sample_rate
06/07/04   ymc     Split QVP_YUV_AMR support into QVP_YUV_AMR_H263 and
                   QVP_YUV_AMR_MPEG4.
           ymc     Added HR QDSP module support.
06/01/04    aw     Added support for H264 image.
05/11/04    vp     Merged ADRC and PCM filter support MP3/AAC from
                   MSMSHARED_AVS.00.09.05.02.00. Added support for H263 images.
            sg     Modified to use qdsp_cmd_set_adec_int_cb() so that callback
                   can be updated while in QTUNES state, but still retain the
                   correct buffer pointers.
                   Modified to enable modules correctly.
05/05/04    sm     Added call to write GSM AFE.
04/23/04    aw     Modified code to support VFE_YCBCR camera image.
03/22/04    st     Modified code to support PCM ADEC mode in QSynth.
03/20/04    aw     Modified code to re-configure ADEC in qtunes state.
03/19/04   ymc     Added support for QVP_BAYER_AMR image.
03/04/04    sg     Removed unsed API; modified code to use old adie
                   configuration sequence.
02/13/04    sg     Changes error message levels for state change messages.
02/13/04    sg     Merged QDSP services changes from
                   MSMSHARED_AVS.00.05.17.01.18.01.00
01/30/04    sg     Modified to support only 32k ADPCM for VFE_BAYER. 8k ADPCM
                   and QCP is no longer support by this firmware.
01/26/04    sg     Modified ADIE configuration sequence to minimize clicks
                   associated with powering up/switching codec paths.
                   Modified so that the vocoder is switched back to reset
                   state if the codec clock rate changes.
                   Modified voc_state_get_inform_val() to prevent race
                   conditions where SND is trying to write to the ADEC before
                   the codec_config is complete.
01/21/04    aw     Added support for qsynth volume mapping function.
12/08/03    sg     Modified to support changes in the vocoder PCM interface.
12/03/03    aw     Added audio support for QGL image.
12/02/03    sg     Added support for linear PCM playback.
11/24/03    aw     Modified code to pass in output sampling rate to qsynth
                   ADRC and PCM filter functions.
11/22/03    sg     Added support for RealAudio codec.
11/05/03    aw     Removed T_MSM6200 to use 22KHz for MIDI image.
10/31/03   ymc     Fixed the problem that image cannot be swapped once
                   QVP_YUV_AMR has been loaded.
10/21/03  aw/st    Modified code to support YADPCM and Qsynth Hybrid mode.
                   Fixed featurization problem in voc_state_get_high_image().
10/10/03    sg     Added support for MP4D_MP3 image.
            aw     Added MSMAUD_VOC_AMR_COMMON featurization.
10/03/03   ymc     Added support for QDSP_IMAGE_QVP_YUV_AMR.
08/26/03    sg     Force recording frames rates to be set in GRAPH_IDLE state,
                   to ensure that valid rates are set before going to the
                   vocoder state.
            sg     Modified to support high and low priority vocoder clients
                   (vis-a-vis QDSP Services). The new higher priority is used
                   to force QDSPS to grant DSP access to the vocoder when we
                   want to support AMPS calls, by preempting other QDSPS clients
                   such as QDSP_APP_IGG.
            st     Modified code to verify that vocoder rates are valid in
                   MP4E images before going to IDLE.
            st     Added support for MP4E and MP4E_BAYER images; modified
                   code to support audio mode selection for graphics images;
                   modified code to do voc_inform if config received when
                   already in QTunes state, CR32479.
            st     Added support for VOC_CODEC_BT_INTERCOM codec type;
                   modified code to use EVRC vocoder image as default.
07/24/03    sg     Added support for STEREO_HEADSET.
07/21/03    sg     Added support for VFE_BAYER camera image.
06/20/03    sm     voc_state_get_high_image() prefers vocoders based on mode.
05/13/03   ymc     Added GSM half rate support.  Added D2E loopback support.
            sm     MIDI now uses 32kHz codec clock.
05/06/03   sa/sg   Added support for Audio dubbing images. Set
                   voc_state_data.config value to config_pending if we
                   continue to stay in STATE_QTUNES.
04/18/03    aw     Added audio support for Q3D image.
04/17/03    st     Fixed AMR featurization; fixed macro name.
04/17/03    st     Added support for AMR for both MP4E and MP4D; added support
                   for ADPCM for MP4E images.
            sg     Added support for vocoder PCM interfaces.
03/21/03    st     Added support for 13K/EVRC for MPEG4 encoder.
                   Refeaturized some FR/EFR/AMR code.
03/13/03    st     Removed specific MP4 decoder support, no longer necessary
                   with new MP4D interface; added support for MP4D+AAC and MP4E.
03/10/03    st     Updated M/N:D counter settings for external SDAC.
            sm     Featurized sleep code under FEATURE_SLEEP_TASK.
02/19/03    st     Added code to support ADPCM mode in QSynth and QCamera
                   images; added support for QSYNTH_DISABLE_PENDING state.
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
01/20/03    st     Modified code to support GSM PLT builds.
01/05/03    st     Modified voc_state_get_high_image() to support EFR and FR.
12/03/02    st     Modified code to add support for graph audio states.
            sg     Removed SDAC MN COUNTER values for MSM3100. Removed
                   MSMAUD_ON_CHIP_SDAC_SUPPORT feature-ization. Moved the
                   SDAC clock configuration code to msmaud.h.
11/06/02    sm     Include msmaud.h
10/29/02    sg     Added support for SPEAKER_PHONE. Modified code so that
                   voc_state_get_high_image() chooses the vocoder image if
                   FEATURE_STEREO_HEADSET is used. Updated code to use the new
                   ADIE cal. interface.
09/18/02   ymc     Corrected comments for voc_inform.
          ymc/sg   Added config_pending checking in voc_state_set_tty(CR32868).
09/16/02    st     Modified voc_state_get_inform_val() to return VOC_INFORM_DFM
                   for DFM_SAT_DCC state (CR24489).
09/06/02  sg/st    Modified code to support delayed write to multiple ADIE
                   registers; modified code to stay in IDLE state until BT
                   headset has been fully configured.
08/23/02    sg     Modified code to support FEATURE_EXTERNAL_SDAC and
                   FEATURE_INTERNAL_SDAC; added support for
                   QDSP_IMAGE_VOC_EVRC_DEFINED.
08/22/02    sg     Added support for TTY activity status indication.
                   (Merged from MSM_AVS.01.05.18.01.02).
08/13/02  aw/st    Added support for ADPCM audio decoder in QSynth image.
            sg     Added clock support for 11025Hz and 12000Hz sampling rates
                   for the internal codec.
            st     Added temporary hack for new QSynth image until ADPCM is
                   supported.
           sg/st   Added support for MP4 and AAC; modified code to not reconfig
                   internal codec if config has not changed; modified code to
                   handle QDSP services callbacks properly; modified code to
                   sleep in RESET state; modified code to go back to RESET from
                   IMAGE state if necessary.
07/11/02   jkl     Modified to call qdsp_cmd_vr_codec_config for PUFE.
07/09/02    sm     Merged to perforce tip.
            sm     Renamed clkrgm_codec_clk to clkrgm_codec_clk_type.
                   Use 22kHz sample clock for MIDI image.
           ymc     Featurized clk_regime_sel_icodec_clk under
                   MSMAUD_CODEC_CLOCK_CTRL.
06/25/02   jkl     Turned off Auto Sample Slip if in VR mode.
06/11/02    st     Modified code to support wideband codec for MP3 image;
                   modified code to support FEATURE_HWTC.
06/06/02   jkl     Added retry on qdsp_cmd_voc_frame_reference.
03/21/02    st     Remove codec PLL enable/disable from voc_state_enable_clocks
                   and voc_state_disbale_clocks(); added
                   voc_state_disable_codec_clk() to disbale codec PLL; removed
                   unnecessary INTLOCK in voc_state_adie_set_final(); moved
                   codec clock setting from state machines to
                   voc_state_adie_wakeup(); added code to disable codec clock
                   when sleeping; added code to ignore config_flag if in VR or
                   PUFE images.
03/12/02    st     Removed MSMAUD_TOM_DRIVER support; added code to do a proper
                   bring up/ bring down sequence for the ADIE codec; modified
                   voc_set_sample_rate() to support MSM6050 going forward;
                   modified code to write codec gains in init_afe(); added
                   calls to setup internal codec clocks to the correct rate
                   for each image; removed double and triple codec config in
                   voc_state_init(); added code to handle config_flag; modified
                   code to no longer reset codec in codec_config command.
03/04/02   jkl     Added PureVoice Universal Front End image.
01/23/02    aw     Enable decoder post filter in vocoder config command.
01/15/02   ymc     Added GSM and WCDMA support.
           ymc     Updated copyright.
12/11/01    aw     Added MSMAUD_ADIE_CODEC_CONFIG. Modified voc_state_init()
                   to do VR config if necessary.
            ro     Use msmaud.h to eliminate targetization.
11/27/01    sm     Disable FAST_QDSP and release QDSP when idling in SYNTH or
                   TUNES states.
10/29/01    st     Added proper featurization.
10/05/01    ro     Check for correct image after qdsp_enable in voc_state_init.
            sg     Added VCO and HCO modes for TTY.
10/01/01    sm     Added FEATURE_BT_AG_LOCAL_AUDIO.
09/04/01    aw     Added FEATURE_QSYNTH_ADRC. Set QSynth PCM filter and ADRC
                   configuration in voc_state_qsynth_init.
09/03/01    st     Modified code to perform the voc_inform() for all vocoder
                   clients; removed call to vocsup_rec_init_queue().
08/28/01    st     Modified code to not unmute analog tx and not enable SAT
                   transponding until we are ready to transition to
                   VOC_STATE_DFM; modified code to do VOC_INFORM_DFM in
                   voc_state_dfm_entry() instead of voc_state_dfm_sat_dcc().
08/14/01   ymc     Removed config_flag support.
08/06/01  sm/db    Now call clk_regime_cpll_wakeup() to enable/disable codec pll
07/31/01    sm     Corrected logic error in turning off codec pll when MSM5100.
07/30/01   ymc     Utilized SLEEP_ASSERT_OKTS and SLEEP_NEGATE_OKTS macros.
07/24/01   ymc     In voc_state_write_codec_gain, do qdsp_cmd_codec_gain_ctrl
                   instead of qdsp_cmd_set_pcmpad for VOC_CODEC_ON_BOARD.
                   Added config_flag control.
                   Added support for loopback tests.
07/16/01    sm     Only turn off codec pll when UP_CLOCK_SWITCHING and MSM5100.
06/19/01   jkl     Removed warnings.
06/18/01  jkl/ym   Added FEATURE_ACP. Replaced FEATURE_DFM_ANSWER_MACHINE.
05/30/01    st     Shutdown/wakeup ADIE when in qtunes/qsynth states.
05/28/01    sm     Don't give codec control when in SDAC mode.
05/21/01   jkl     Removed Enhanced VR feature.
05/18/01    sm     Support TTY in 5100.
                   Switch USB codec to use digital gains.
                   Disable fast qdsp when transition to ERROR state.
05/08/01   jkl     Modified voc_adie_wakeup for internal codec only.
04/26/01    sm     Added FEATURE_USB_ISOC_AUDIO support.
                   Call voc_check_bt_ag() when configuring in idle.
                   Call clk_regime_enable() in 5100 for SDAC clocks.
04/04/01    st     Modified code to do the qdsp_cmd_adec_config() in the
                   QSynth and QTunes entry states to ensure that DSP is
                   in the correct state first.
03/02/01    sm     Added calls to voc_check_bt_ag() to enable/disable
                   BT AG in proper states; modified code to not configure
                   to BT codec; modified calls to voc_data_get_codec_config()
                   to use new interface.
02/28/01    st     Removed support for 16K codec types; lint updates.
02/21/01    vl     Added MSM5100 support.
01/26/01    st     Modified code to select proper image when stereo DAC is
                   active; modified voc_state_set_sample_rate() to fix
                   stereo DAC sample rate switching startup screech; modified
                   state entry functions to reduce failure retries; modified
                   state functions to base state/image changes on highest
                   priority image and not the pending configuration.
12/10/00    st     Removed featurization in voc_state_set_sample_rate();
                   modified code to update the config to config_pending
                   in voc_state_idle() even if vocoder is not config'ed.
12/06/00   jkl     Removed set ISR functions. Corrected Featurization.
12/05/00   jkl     Merged VR4.0 functions. Added FEATURE_ENHANCED_VR,
12/04/00  st/sm    Added QTUNES, ADEC, stereo DAC support.
11/06/00    st     Renamed voc_state_retry_timer to voc_state_gp_timer;
                   modified codec reset sequence in voc_state_init() to allow
                   a 20 ms delay before resetting codec; modified code to
                   update the config to config_pending in voc_state_init().
10/27/00    st     Modified code to enable DFM while in QSynth state.
09/21/00    sm     Check qdsp_ret[x] for QDSP_CMD_SUCCESS in voc_state_init_afe
                   Use voc_data_get_codec() and voc_data_get_codec_config()
                   Added 16k_on_chip_aux path to write_codec_gain()
09/12/00    sm     Featurize BT_AG codec support.
09/08/00    sm     Added Bluetooth/PPG support.
08/28/00    ro     Uses QDSP Services. Added voc_state_image.
                   voc_state_get_inform_val returns IDLE when in IDLE_ENTRY.
08/14/00   spf     Updated voc_state_get_high_image() to give DFM highest
                   priority.
07/19/00   spf     Initial GPS and Bluetooth support.
07/18/00    st     Added proper featurization.
07/15/00    st     Lint updates; change MSG_ERROR() to MSG_HIGH() because
                   situation is no longer invalid for QSynth image.
07/14/00   spf     Added support for TTY headset and HFK devices.  Moved
                   calling of qdspdown_set_loader_image() to
                   voc_state_control_init().  Lint updates.
06/30/00   spf     Added NES override support.
06/19/00   spf     Updated to use an enumeration for image selection.
06/11/00   spf     Support for firmare image featurization.
06/07/00    st     Added calls to voc_disable_codec_func() and
                   voc_enable_codec_func() when doing codec config commands.
05/26/00   spf     Moved initialization of voc_state_data.reset_flag to
                   voc_state_reset().
05/16/00    st     Added support for QSynth image state machine and stereo DAC
                   codec; added voc_state_get_high_image() and modified code to
                   use this function to determine proper state changes.
05/12/00   spf     Add TTY support: TTY firmware image selection.
05/02/00   spf     Fixed clock control so that clocks are disabled while in
                   the SLEEP/INIT state. Added voc_inform(VOC_INFORM_NO_SAT)
                   in voc_state_idle() for DFM calls. Added speakerphone echo
                   canceller support.
            sm     Added voc_state_init_aud_process() to separate process
                   selection from filter loading. Added
                   voc_state_set_hpf_override().
03/22/00   spf     Two-stage download support.
03/08/00   spf     Continue support for Rev6/Rev7 image selection.
02/24/00    ro     Continue support for MSM's earlier than Rev6.
02/11/00   spf     Added const qualifier to qdspdown_download_image()
                   image pointer parameter.
01/25/00   ro/spf  Added support for VR and VM. Added voc_state_set_codec.
                   Support for Rev7 only.
01/21/00   spf     Updated voc_state_enable_clocks() and voc_state_disable
                   _clocks() to use semaphores.  Removed codec PLL warmup
                   timer.
01/06/00    ro     Increased time to wait for 1st SAT from .25 to 1 second.
12/17/99   jkl     Wait longer time to reach Init State. Allow sleep at
                   ERROR state. Sample Slipping now in Idle entry state.
12/14/99   jkl     Modified sample slipping to manual at init state.
12/07/99   spf     Added voc_state_enable_clocks() and voc_state_disable_
                   clocks().  Driver will no longer transition to ERROR
                   state if PCM filter loads fail.  Updated codec PLL
                   warmup time for Rev7.
12/01/99   spf     Added support for run-time QDSP download image selection.
                   Image selected is based on the MSM revision (rev6/rev7).
11/22/99   jkl     Added new parameters to AGC/AVC.
11/17/98   jkl     Removed CODEC microprocessor control for REV6 or later.
10/28/99   spf     Merged R1.10.1.0.  Added Codec PLL on/off switching.
10/28/99   spf     Increased IDLE->VOCODER state transition timeout to 1sec.
10/26/99   jkl     Added second voc_config() call if first one fails.
10/18/99    st     Added support for audio AGC/AVC.
10/14/99   tc      Cleared filter load flags before sleep.
10/14/99   jkl     Adjusted Codec Reset Strobe from init to idle.
                   Read Vocoder Diagnostic data after downloading.
10/12/99   tc      Give microprocessor control of internal codec when in
                   SLEEP state.  Give QDSP control of internal codec when
                   in INIT state.
10/07/99   spf     Added new SAT_DCC state to wait for a valid SAT to be
                   detected by the QDSP. Improved state transition retry
                   mechanism by using new voc_state_data.retry_counter and
                   removing voc_state_retry_timer.
10/04/99   jkl     Adjusted PCM pad volume function name.
09/25/99   ro/st   Set state before calling voc_inform.
                   Set volume after voc_inform
09/23/99   spf/tc  Enhanced VOC_STATE_ERROR_RECOVERY state to debounce faults.
                   Implemented 20ms timer-based state transition retry.
                   Updated voc_state_init_afe to only load Tx and Rx PCM filters
                   once in the state machine.  Temporary fix until problem is
                   investigated more thoroughly.
09/22/99   spf     Fixed VOC_STATE_ERROR_RECOVERY state to disable vocoder and
                   codec clock regimes.
09/16/99   spf     Removed FEATURE_QDSP2 and FEATURE_QDSP_INTERNAL_CODEC.
                   Moved voc_state_init_afe() call to voc_state_idle_entry.
09/10/99   spf     Added voc_offline support.  Fixed clock regimes.
                   Improved state transition timing.
09/07/99   jkl     Write CODEC volume when DSP going to idle.
08/26/99   spf     Pink Panther Rev2 support.
08/02/99   jc      Removed support for MSM3100 GPIOs.
08/02/99   spf     Fixed unused variable compiler warnings generated
                   by previous update.
07/15/99   jc/mk   Added support for MSM3100 GPIOs.
07/15/99   spf     DFM support.
06/15/99   ro/sf   First version.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"           /* Customer Specific Definitions           */
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "rex.h"                /* definition of REX data types            */
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif               
#ifdef FEATURE_CMI_MM
#error code not present
#endif                     /* Clock prototypes                        */
#include "task.h"               /* Task definitions and prototypes         */
#include "msg.h"                /* Message logging macro                   */
#include "err.h"                /* Error logging macro                     */
#include "hw.h"                 /* Hardware specific definitions           */

#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#include "sleep.h"              /* Sleep signals                           */
#endif /* T_SLEEP || FEATURE_SLEEP_TASK */

#include "msmaud.h"             /* MSMAudio features                       */
#include "voc.h"                /* Vocoder interface definition            */
#include "vocsup.h"             /* Vocoder Support Functions               */
#include "vocdata.h"            /* Vocoder State and Cal Data Definitions  */
#include "vocstate.h"           /* Vocoder State Controller Definitions    */
#include "qdspcmd.h"            /* QDSP Driver Command Layer definitions  */
#include "qdspdown.h"           /* QDSP download module                   */
#include "qdspext.h"            /* QDSP firmware image definitions         */
#include "clkregim.h"           /* For clk_regime_enable/disable.          */
#ifdef FEATURE_PHONE_VR
#include "vrengine.h"           /* Voice-Recognition Engine                */
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_EXTERNAL_SDAC
#include "sdac.h"               /* External Stereo DAC prototypes          */
#include "bsp.h"                /* Board support package                   */
#endif /* FEATURE_EXTERNAL_SDAC */

#ifdef MSMAUD_ADIE_CODEC
#ifndef MSMAUD_SCMM
#include "adie.h"               /* Analog die definitions                  */
#endif /* MSMAUD_SCMM */
#endif /* MSMAUD_ADIE_CODEC */

#ifdef FEATURE_EXTERNAL_SADC
#include "sadc.h"               /* External PCM codec definitions          */
#include "msmhwio.h"
#endif /* FEATURE_EXTERNAL_SADC */

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
#include "audadsp_mixer.h"
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifdef MSMAUD_PMIC_CONFIG
#include "pm.h"
#include "pmapp.h"
#endif /* MSMAUD_PMIC_CONFIG */

#include "vocvp3.h"  /*VP3 definitions*/

#ifdef FEATURE_QDSP_ERR_LOGS_STORE
#error code not present
#endif

#ifndef MSMAUD_SCMM
#include "adie_services.h"
#endif /* MSMAUD_SCMM */

#ifdef MSMAUD_SCMM
#error code not present
#endif

/* CB Function pointer, when PAs are turned on, initialized at the time of
   sndhw_init()*/
LOCAL voc_pa_on_cb_func_ptr_type voc_state_pa_on_cb = NULL;

#ifdef FEATURE_OEMSBL_AUDIBLE_TONE
voc_oemsbl_cb_func_ptr_type voc_state_oemsbl_func_ptr = NULL;
#endif

#ifdef  AUDIO_LCU_PMIC_RFLDO
#error code not present
#endif   /* AUDIO_LCU_PMIC_RFLDO */

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

extern voc_client_config_type voc_client_config[VOC_MAX_ACQUIRE_TYPE];
/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/* Time to wait between state transitions
*/
#ifdef FEATURE_VOC_FAST_TIMELINE
#define VOC_STATE_TRANS_TIME_DEFAULT    5
#define VOC_STATE_TRANS_TIME_RAPID      0
#else
#define VOC_STATE_TRANS_TIME_DEFAULT    10
#define VOC_STATE_TRANS_TIME_RAPID      VOC_STATE_TRANS_TIME_DEFAULT
#endif

#define VOC_STATE_TRANS_TIME_QVP        10

#define VOC_STATE_VP3_ENABLE_V          0xFFFF
#define VOC_STATE_VP3_DISABLE_V         0
#define VOC_STATE_MPU_PARTITION_NO      0
/*-------------------------------------------------------------------------
** Vocoder driver State Control Types
**-----------------------------------------------------------------------*/
/* Select the HPF override mode
*/
typedef enum {
  VOC_STATE_HPF_OVERRIDE_OFF = 0,
  VOC_STATE_HPF_OVERRIDE_ON
} voc_state_hpf_override_type;

/* Select the NES override mode
*/
typedef enum {
  VOC_STATE_NES_OVERRIDE_OFF = 0,
  VOC_STATE_NES_OVERRIDE_ON
} voc_state_nes_override_type;

/*-------------------------------------------------------------------------
** Vocoder driver State Control Data
**-----------------------------------------------------------------------*/
voc_state_control_type  voc_state_data;

/* Clock control */
/* Counter incremented when Client requests external ECODEC clock to be enabled */
LOCAL uint16 voc_state_eclock_count = 0;

/* Flag to denote when external clocks have been enabled */
LOCAL boolean voc_state_eclocks_on = FALSE;

/* Flag to denote when CODEC clocks have already been enabled directly by Vocoder */
/* This controls calls to voc_state_enable_clocks() to ensure that Clocks are not */
/* enabled more than once for each change in Codec. */
LOCAL boolean voc_state_clocks_on = FALSE;

/* This critial section is used to block the concurrent access to the */
/* functions that enable and disable internal and external clocks */
LOCAL rex_crit_sect_type voc_state_clk_crit_sect;

#ifndef MSMAUD_SCMM
#ifdef MSMAUD_ADIE_CODEC
/* Clock callback structure that implements final setting in ADIE codec.
*/


#ifdef FEATURE_CMI
#error code not present
#else
    	clk_cb_type voc_state_adie_clk_cb;
#endif

/* Pointer to the last ADIE codec configuration structure.
*/
voc_adie_codec_config_type *voc_state_last_adie_config = NULL;
#endif /* MSMAUD_ADIE_CODEC */
#endif /* MSMAUD_SCMM */

/* State change timer */
rex_timer_type voc_state_timer;

/* General purpose timer */
rex_timer_type voc_state_gp_timer;


/*-------------------------------------------------------------------------
** ADSP Error recovery vars
**-----------------------------------------------------------------------*/
/* Firmware error count */
LOCAL uint16 voc_state_fw_err_cnt = 0;
/* Maximum number of consecutive errors */
const LOCAL uint16 voc_state_fw_err_cnt_max = 20;

/* Disable DSP error mechanism by setting to TRUE: Useful for debugging DSP */
boolean voc_state_disable_fw_err_recovery = FALSE;

/*-----------------------------------------------------------------------*/

#ifndef MSMAUD_SCMM
/* Flag to denote when voc_state_adie_set_final function is registered*/
LOCAL boolean voc_state_adie_set_final_reg = FALSE;
#endif /* MSMAUD_SCMM*/

#if ((defined (FEATURE_QSYNTH) || defined(FEATURE_QTUNES)) \
     && defined (FEATURE_ACP))
#error code not present
#endif

#ifdef FEATURE_EXTERNAL_SDAC
typedef struct {
  word mdiv;
  dword not_n_minus_m;
  word duty;
} yamn_sr_ctl_type;

/* Table of values to set the YAMN counter registers in order to get the
** proper clock rate for the specified sampling rate.
*/
const yamn_sr_ctl_type yamn_sr_ctl[] = {

  { 0x0000, 0x00000000, 0x0000 }, /*     0 sps */

#ifdef MSMAUD_NEW_SDAC_DUTY_CYCLE_REG

  { 0x0280, 0x000146FF, 0x4480 }, /*  8000 sps */
  { 0x0372, 0x000147F1, 0x4480 }, /* 11025 sps */
  { 0x03C0, 0x0001483F, 0x4480 }, /* 12000 sps */
  { 0x0500, 0x0001497F, 0x4480 }, /* 16000 sps */
  { 0x06E4, 0x00014B63, 0x4480 }, /* 22050 sps */
  { 0x0780, 0x00014BFF, 0x4480 }, /* 24000 sps */
  { 0x0A00, 0x00014E7F, 0x4480 }, /* 32000 sps */
  { 0x0DC8, 0x00015247, 0x4480 }, /* 44100 sps */
  { 0x0F00, 0x0001537F, 0x4480 }  /* 48000 sps */

#else

  { 0x0280, 0x000146FF, 0x5DC0 }, /*  8000 sps */
  { 0x0372, 0x000147F1, 0x5DC0 }, /* 11025 sps */
  { 0x03C0, 0x0001483F, 0x5DC0 }, /* 12000 sps */
  { 0x0500, 0x0001497F, 0x5DC0 }, /* 16000 sps */
  { 0x06E4, 0x00014B63, 0x5DC0 }, /* 22050 sps */
  { 0x0780, 0x00014BFF, 0x5DC0 }, /* 24000 sps */
  { 0x0A00, 0x00014E7F, 0x5DC0 }, /* 32000 sps */
  { 0x0DC8, 0x00015247, 0x5DC0 }, /* 44100 sps */
  { 0x0F00, 0x0001537F, 0x5DC0 }  /* 48000 sps */

#endif /* MSMAUD_NEW_SDAC_DUTY_CYCLE_REG */

};

#endif /* FEATURE_EXTERNAL_SDAC */

#if defined( FEATURE_ACP ) && defined( FEATURE_VOICE_RECORD )
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)

voc_pp_status_type voc_state_pp_status = VOC_PP_STATUS_DISABLED;

#endif /* FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE)*/

#ifdef FEATURE_MM_REC
voc_mm_record_format_type voc_mm_enc_format;
#endif /* FEATURE_MM_REC */

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
boolean voc_bt_playback_started = FALSE;
#endif /*AVS_BT_SCO_REWORK*/

#ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
#endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */

#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#ifdef FEATURE_NEW_SLEEP_API
  const char *const voc_state_sleep_str = "VOC_SLEEP";
  sleep_okts_handle voc_state_sleep_handle;
#endif /* FEATURE_NEW_SLEEP_API  */
#endif /* T_SLEEP || FEATURE_SLEEP_TASK */

static boolean voc_state_pa_on_status = FALSE;

/* <EJECT> */
/*===========================================================================

                 F U N C T I O N   D E F I N I T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION voc_state_module_commit

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  boolean value indicates synth is ready or not

SIDE EFFECTS
  None

===========================================================================*/
void voc_state_module_commit
(
  qdsp_module_type module
) {
  voc_state_data.module_reqd = module;

#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
  if (voc_state_check_ica_mixer_module(module) == TRUE) {
    voc_state_data.snd_config = voc_get_desired_config(VOC_ACQ_SND);

    if ((voc_state_data.acquire_mask & ((word) 1 << VOC_ACQ_CDMA)) &&
        (voc_state_data.config_mask & ((word) 1 << VOC_ACQ_CDMA))) {
      /* This is for 1X vocoders */
      voc_state_data.voc_config = voc_get_desired_config(VOC_ACQ_CDMA);
    } /* if 1X call */
#ifdef FEATURE_MVS
    else if ((voc_state_data.acquire_mask & ((word) 1 << VOC_ACQ_MVS))  &&
             (voc_state_data.config_mask  & ((word) 1 << VOC_ACQ_MVS))) {
      voc_state_data.voc_config = voc_get_desired_config(VOC_ACQ_MVS);

    } /* if UMTS/GSM call */
#endif /* FEATURE_MVS */
  }
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB ||
          FEATURE_AVS_INCALL_MIXER_CMX */
}
/*===========================================================================

FUNCTION voc_state_voc_tx_wb_query

DESCRIPTION
  This function checks if image supporting voc requires wideband Tx AFE.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: required   FALSE: not required

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean voc_state_voc_tx_wb_query(void)
{
  boolean voc_tx_wb_flag = FALSE;


  switch (voc_state_data.current_image)
  {
#ifdef QDSP_IMAGE_4GV_WB_DEFINED
    case QDSP_IMAGE_4GV_WB:
      voc_tx_wb_flag = TRUE;
      break;
#endif

#ifdef QDSP_IMAGE_AMR_WB_DEFINED
    case QDSP_IMAGE_AMR_WB:
      voc_tx_wb_flag = TRUE;
      break;
#endif

    default:
      voc_tx_wb_flag = FALSE;
      break;
  }


  return voc_tx_wb_flag;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_aux_pcm_clk_query

DESCRIPTION
  This function checks if AUX PCM clock is required with the current codec
configuration.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: required   FALSE: not required

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_state_aux_pcm_clk_query(void)
{
  voc_codec_type codec;


  voc_data_get_codec(&codec);

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
#ifdef FEATURE_BT_AG
    /* FM over BT AG */
    case VOC_CODEC_FM_OVER_BT_AG:
#endif /* FEATURE_BT_AG */
         return TRUE;

    default:
    break;
  }

  return FALSE;
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_test_mode_config

DESCRIPTION
  This function is used to configure the next testing mode.  It can be
used to start or stop a test and transit between two different tests.
It will acquire or release VOC_ACQ_TEST if necessary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  test_mode_pending will be configured.

===========================================================================*/
void voc_state_test_mode_config(
  voc_test_mode_type test        /* test mode to be configured */
)
{
  static voc_capability_type voc_test_mode_cap = VOC_CAP_NONE;

  if (test == voc_state_data.test_mode_pending)
  {
    return;
  }

  voc_state_data.test_mode_pending = test;

  switch (test)
  {
    case VOC_TEST_NONE :
      voc_release(VOC_ACQ_TEST);
      break;


    case VOC_TEST_AUDIO_LOOP :
      voc_acquire(VOC_ACQ_TEST, NULL);
                                   /* prevent QDSP from going to sleep */
      voc_config(VOC_ACQ_TEST, VOC_CAP_NONE);
      break;


    case VOC_TEST_INT_PKT_LOOP :
    case VOC_TEST_D2E_LOOP :
#ifdef FEATURE_UP_LOOPBACK_TEST
    case VOC_TEST_UP_PKT_LOOP :
#endif
      MSG_HIGH("Enabling packet loopback %d.", voc_test_mode_cap,0,0);
      MSG_MED("pkt_lb dis->ena cycles the vocoder used",0,0,0);

      do {
        switch(voc_test_mode_cap)
        {
        case VOC_CAP_NONE:
          /*lint -fallthrough */

#ifdef MSMAUD_VOC_IS733
        case VOC_CAP_IS733:
          if(voc_test_mode_cap != VOC_CAP_IS733) {
            MSG_HIGH("Packet loopback using: V13K",0,0,0);
            voc_test_mode_cap = VOC_CAP_IS733;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_IS733 */

#ifdef MSMAUD_VOC_IS127
        case VOC_CAP_IS127:
          if(voc_test_mode_cap != VOC_CAP_IS127) {
            MSG_HIGH("Packet loopback using: EVRC",0,0,0);
            voc_test_mode_cap = VOC_CAP_IS127;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_IS127 */

#ifdef MSMAUD_VOC_IS127_VOIP
        case VOC_CAP_IS127_VOIP:
          if(voc_test_mode_cap != VOC_CAP_IS127_VOIP) {
            MSG_HIGH("Packet loopback using: EVRC VOIP",0,0,0);
            voc_test_mode_cap = VOC_CAP_IS127_VOIP;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_IS127_VOIP */

#ifdef MSMAUD_VOC_AMR
        case VOC_CAP_AMR:
          if(voc_test_mode_cap != VOC_CAP_AMR) {
            MSG_HIGH("Packet loopback using: AMR",0,0,0);
            voc_set_gsm_enc_dtx(VOC_GSM_DTX_OFF);
            voc_test_mode_cap = VOC_CAP_AMR;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_AMR */

#ifdef MSMAUD_VOC_AMR_WB
        case VOC_CAP_AMR_WB:
          if(voc_test_mode_cap != VOC_CAP_AMR_WB) {
            MSG_HIGH("Packet loopback using: AMR WB",0,0,0);
            voc_set_gsm_enc_dtx(VOC_GSM_DTX_ON);
            voc_test_mode_cap = VOC_CAP_AMR_WB;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_AMR_WB */

#ifdef MSMAUD_VOC_4GV
        case VOC_CAP_4GV_NB:
          if(voc_test_mode_cap != VOC_CAP_4GV_NB) {
            MSG_HIGH("Packet loopback using: 4GV-NB",0,0,0);
            voc_test_mode_cap = VOC_CAP_4GV_NB;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_4GV */

#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif /* MSMAUD_VOC_4GV_VOIP */

#ifdef MSMAUD_VOC_4GV_WB
        case VOC_CAP_4GV_WB:
          if(voc_test_mode_cap != VOC_CAP_4GV_WB) {
            MSG_HIGH("Packet loopback using: 4GV-WB",0,0,0);
            voc_test_mode_cap = VOC_CAP_4GV_WB;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_4GV_WB */

#ifdef MSMAUD_VOC_4GV_WB_VOIP
        case VOC_CAP_4GV_WB_VOIP:
          if(voc_test_mode_cap != VOC_CAP_4GV_WB_VOIP) {
            MSG_HIGH("Packet loopback using: 4GV-WB VOIP",0,0,0);
            voc_test_mode_cap = VOC_CAP_4GV_WB_VOIP;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_4GV_WB_VOIP */

#ifdef MSMAUD_VOC_EFR
#error code not present
#endif /* MSMAUD_VOC_EFR */

#ifdef MSMAUD_VOC_FR
#error code not present
#endif /* MSMAUD_VOC_FR */

#ifdef MSMAUD_VOC_HR
        case VOC_CAP_GSM_HR:
          if(voc_test_mode_cap != VOC_CAP_GSM_HR) {
            MSG_HIGH("Packet loopback using: GSM_HR",0,0,0);
            voc_set_gsm_enc_dtx(VOC_GSM_DTX_OFF);
            voc_test_mode_cap = VOC_CAP_GSM_HR;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_G711
        case VOC_CAP_G711:
          if(voc_test_mode_cap != VOC_CAP_G711) {
            MSG_HIGH("Packet loopback using: G711",0,0,0);
            voc_test_mode_cap = VOC_CAP_G711;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
        case VOC_CAP_G723:
          if(voc_test_mode_cap != VOC_CAP_G723) {
            MSG_HIGH("Packet loopback using: G723",0,0,0);
            voc_test_mode_cap = VOC_CAP_G723;
            break;
          }
          /*lint -fallthrough */
#endif /* MSMAUD_VOC_G723 */

        default:
          if(voc_test_mode_cap != VOC_CAP_NONE) {
            voc_test_mode_cap = VOC_CAP_NONE;
          } else {
            /* The only way to get here is to have no case statements
            ** compiled inside the switch statement.
            */
            MSG_ERROR("No valid packet loopback vocoder.",0,0,0);
            return;
          }
          break;
        }
      } while (voc_test_mode_cap == VOC_CAP_NONE);

      voc_acquire(VOC_ACQ_TEST, NULL);
      voc_config(VOC_ACQ_TEST, voc_test_mode_cap);
      break;

#ifdef FEATURE_ACP
#error code not present
#endif
  }   /* switch (test) */
}  /* voc_state_test_mode_config */


/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_qdsp_event_handler

DESCRIPTION
  This function receives and processes events from the QDSP Services Task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_state_qdsp_event_handler ( qdsp_event_data_type *event_ptr )
{
  voc_notify_qtv_cb_func_ptr_type  qtv_notify_func_ptr = NULL;

  switch (event_ptr->event) {
    case QDSP_EV_COMMAND:
    if (((qdsp_ev_cmd_type*)event_ptr)->cmd_status == QDSP_CMD_FAIL) {
      MSG_ERROR("QDSPS CMD FAILED", 0,0,0);
    }
    break;

    case QDSP_EV_MODULE: {
      qdsp_ev_mod_type *mod_event_ptr = (qdsp_ev_mod_type*)event_ptr;

      switch (mod_event_ptr->mod_status) {
        case QDSP_MOD_READY:
          switch (voc_state_data.module_status) {
            case VOC_STATE_MOD_STATUS_DISABLED:
              MSG_MED("Ignoring MOD_READY rcvd after calling disable",0,0,0);
              break;

            case VOC_STATE_MOD_STATUS_DISABLE_PENDING:
              ERR_FATAL("Unexpected MOD_READY", 0,0,0);
              break;

            case VOC_STATE_MOD_STATUS_ENABLE_REQUESTED:
              voc_state_data.module_status = VOC_STATE_MOD_STATUS_ENABLED;
              /* Fall through */

            case VOC_STATE_MOD_STATUS_ENABLED:
#if defined(FEATURE_QSYNTH_COMMON)
              switch(mod_event_ptr->module) {
                case QDSP_MODULE_QSYNTH:
#ifdef FEATURE_QSYNTH_ADPCM
                case QDSP_MODULE_ADPCM:
#endif /* FEATURE_QSYNTH_ADPCM */
                  if (voc_state_data.state == VOC_STATE_QSYNTH) {
                    MSG_HIGH("emulate synth interrupt", 0, 0, 0);
                    qdsp_cmd_qsynth_emulate_isr();
                  }
                  break;
              }
#endif /* FEATURE_QSYNTH_COMMON */
              voc_state_data.current_image = mod_event_ptr->info.ready.image;
              voc_state_module_commit(mod_event_ptr->module);
              qdsp_cmd_set_image(voc_state_data.current_image,
                                 voc_state_data.module_reqd);
              MSG_HIGH("Recvd Module %d READY state=%d",
                        mod_event_ptr->module,(int)voc_state_data.state,0);
              break;

            default:
              ERR_FATAL("Illegal internal state", 0,0,0);
              break;
          }
          break;

        case QDSP_MOD_DISABLE:
          switch (voc_state_data.module_status) {
            case VOC_STATE_MOD_STATUS_DISABLED:
              MSG_HIGH("qdsp_disable_event_rsp(%d)",
                        voc_state_data.current_qdsps_app,0,0);
              qdsp_cmd_clear_dsp_isr();
              qdsp_disable_event_rsp(voc_state_data.current_qdsps_app);
              break;

            case VOC_STATE_MOD_STATUS_ENABLE_REQUESTED:
            case VOC_STATE_MOD_STATUS_ENABLED:
              MSG_HIGH("Recvd Module %d DISABLE state=%d",
                        mod_event_ptr->module,(int)voc_state_data.state,0);

              if (voc_state_data.state == VOC_STATE_ERROR_RECOVERY) {
                voc_state_data.module_status =
                                         VOC_STATE_MOD_STATUS_ENABLE_REQUESTED;
                MSG_HIGH("qdsp_disable_event_rsp(%d)",
                                         voc_state_data.current_qdsps_app,0,0);
                qdsp_cmd_clear_dsp_isr();
                qdsp_disable_event_rsp(voc_state_data.current_qdsps_app);

                /* Some other DSP user caused the image to change.
                 * Debounce the error condition by reseting DSP */
                voc_state_data.state = VOC_STATE_RESET;
                voc_state_data.retry_counter = 0;
                voc_state_fw_err_cnt = 0;
              }
              else {
                voc_state_data.module_status =
                                          VOC_STATE_MOD_STATUS_DISABLE_PENDING;
              }
              /*Inform QTV about the mod_disable event for audio only image
               This callback is registrd only from qtv's audio thread and
               hence gets triggrd only when audio only image is loaded*/
              qtv_notify_func_ptr = voc_data_get_qtv_cb_func();
              MSG_MED("MOD DISABLE:func_ptr = %d, app_id= %d, Module = %d",
               qtv_notify_func_ptr,mod_event_ptr->app_id,mod_event_ptr->module);
              if ((qtv_notify_func_ptr != NULL)&&
                    (mod_event_ptr->app_id == QDSP_APP_VOC)&&
                      (mod_event_ptr->module != QDSP_MODULE_BA_MONO)){
                MSG_HIGH("Notifying QTV abt mod_disable evt",0,0,0);
                qtv_notify_func_ptr((uint32)QDSP_MOD_DISABLE);
              }
              break;

            case VOC_STATE_MOD_STATUS_DISABLE_PENDING:
              MSG_ERROR("Unexpected MOD_DISABLE", 0,0,0);
              break;

            default:
              ERR_FATAL("Illegal internal state", 0,0,0);
              break;
          }
          break;

        default:
          ERR_FATAL("Illegal module status",0,0,0);
          break;
      }

#ifndef FEATURE_HWTC
      /* Take care of any vocoder state changes caused by this event
      */
      (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
#endif /* FEATURE_HWTC */
    }
    break;

    case QDSP_EV_IMAGE_CHANGE:
      if (voc_state_data.current_qdsps_app == event_ptr->img_chg.app_id) {
        if (voc_state_data.state == VOC_STATE_ERROR_RECOVERY) {
          if (voc_state_data.module_status < VOC_STATE_MOD_STATUS_ENABLED) {
            /* Some other DSP user caused the image to change.
             * Debounce the error condition by reseting DSP */
            voc_state_data.state = VOC_STATE_RESET;
            voc_state_data.retry_counter = 0;
            voc_state_fw_err_cnt = 0;
            (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
          }
        }
        else {
          /* Ignore it if we are currently enabled - we should have recvd a
          ** DISABLE event for this image_change, prior to this event.
          */
          if (voc_state_data.module_status != VOC_STATE_MOD_STATUS_ENABLED) {
            if (voc_state_data.current_image != event_ptr->img_chg.image) {
              voc_state_data.image_changed = TRUE;
            }
          }
          else if (voc_state_data.num_higher_priority_apps !=
                   event_ptr->img_chg.num_higher_priority_apps) {
            voc_state_data.image_changed = TRUE;
          }

          voc_state_data.num_higher_priority_apps =
                                    event_ptr->img_chg.num_higher_priority_apps;
          voc_state_data.current_image = event_ptr->img_chg.image;

          if (voc_state_data.image_changed == TRUE) {
            (void) rex_set_sigs( &voc_tcb, VOC_CHECK_STATE_SIG );
          }
        }
      }
      break;

    default:
      ERR_FATAL("Illegal QDSP Event type",0,0,0);
      break;
  }
} /* voc_state_qdsp_event_handler */


/* <EJECT> */
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_get_module_for_basic_audio

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_state_get_module_for_basic_audio( qdsp_module_type *module )
{
#if defined(FEATURE_EXTERNAL_SDAC)
  voc_codec_class_type    codec_type;
  voc_data_get_codec_class(&codec_type);

  if(codec_type == VOC_CODEC_CLASS_EXTERNAL_STEREO) {
#ifdef QDSP_MODULE_BA_STEREO_DEFINED
    *module = QDSP_MODULE_BA_STEREO;
#else
#error "No QDSP module available for Basic Audio (Stereo)"
#endif
  } else
#endif

  {
#ifdef QDSP_MODULE_BA_MONO_DEFINED
    *module = QDSP_MODULE_BA_MONO;
#else
#error "No QDSP module available for Basic Audio"
#endif
  }
  return VOC_DONE_S;
}

#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
/*===========================================================================

FUNCTION voc_state_check_ica_module

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_state_check_ica_mixer_module
(
  qdsp_module_type module
) {
  boolean ret_val = FALSE;

  switch(module) {
#ifdef QDSP_MODULE_SYNTH_INCALL_MIXER_13K_DEFINED
    case QDSP_MODULE_SYNTH_INCALL_MIXER_13K:
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_13K_DEFINED */
#ifdef QDSP_MODULE_TUNES_INCALL_MIXER_13K_DEFINED
    case QDSP_MODULE_TUNES_INCALL_MIXER_13K:
#endif /* QDSP_MODULE_TUNES_INCALL_MIXER_13K_DEFINED */
#ifdef QDSP_MODULE_TUNES_WMA_INCALL_MIXER_13K_DEFINED
    case QDSP_MODULE_TUNES_WMA_INCALL_MIXER_13K:
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_13K_DEFINED */
#ifdef QDSP_MODULE_TUNES_AAC_INCALL_MIXER_13K_DEFINED
    case QDSP_MODULE_TUNES_AAC_INCALL_MIXER_13K:
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_13K_DEFINED */
#ifdef QDSP_MODULE_SYNTH_INCALL_MIXER_EVRC_DEFINED
    case QDSP_MODULE_SYNTH_INCALL_MIXER_EVRC:
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_EVRC_DEFINED */
#ifdef QDSP_MODULE_TUNES_INCALL_MIXER_EVRC_DEFINED
    case QDSP_MODULE_TUNES_INCALL_MIXER_EVRC:
#endif /* QDSP_MODULE_TUNES_INCALL_MIXER_EVRC_DEFINED */
#ifdef QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EVRC_DEFINED
    case QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EVRC:
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EVRC_DEFINED */
#ifdef QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EVRC_DEFINED
    case QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EVRC:
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EVRC_DEFINED */
#ifdef QDSP_MODULE_SYNTH_INCALL_MIXER_AMR_DEFINED
    case QDSP_MODULE_SYNTH_INCALL_MIXER_AMR:
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_AMR_DEFINED */
#ifdef QDSP_MODULE_TUNES_INCALL_MIXER_AMR_DEFINED
    case QDSP_MODULE_TUNES_INCALL_MIXER_AMR:
#endif /* QDSP_MODULE_TUNES_INCALL_MIXER_AMR_DEFINED */
#ifdef QDSP_MODULE_TUNES_WMA_INCALL_MIXER_AMR_DEFINED
    case QDSP_MODULE_TUNES_WMA_INCALL_MIXER_AMR:
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_AMR_DEFINED */
#ifdef QDSP_MODULE_TUNES_AAC_INCALL_MIXER_AMR_DEFINED
    case QDSP_MODULE_TUNES_AAC_INCALL_MIXER_AMR:
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_AMR_DEFINED */
#ifdef QDSP_MODULE_SYNTH_INCALL_MIXER_EFR_DEFINED
    case QDSP_MODULE_SYNTH_INCALL_MIXER_EFR:
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_EFR_DEFINED */
#ifdef QDSP_MODULE_TUNES_INCALL_MIXER_EFR_DEFINED
    case QDSP_MODULE_TUNES_INCALL_MIXER_EFR:
#endif /* QDSP_MODULE_TUNES_INCALL_MIXER_EFR_DEFINED */
#ifdef QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EFR_DEFINED
    case QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EFR:
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EFR_DEFINED */
#ifdef QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EFR_DEFINED
    case QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EFR:
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EFR_DEFINED */
#ifdef QDSP_MODULE_SYNTH_INCALL_MIXER_FR_DEFINED
    case QDSP_MODULE_SYNTH_INCALL_MIXER_FR:
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_FR_DEFINED */
#ifdef QDSP_MODULE_TUNES_INCALL_MIXER_FR_DEFINED
    case QDSP_MODULE_TUNES_INCALL_MIXER_FR:
#endif /* QDSP_MODULE_TUNES_INCALL_MIXER_FR_DEFINED */
#ifdef QDSP_MODULE_TUNES_WMA_INCALL_MIXER_FR_DEFINED
    case QDSP_MODULE_TUNES_WMA_INCALL_MIXER_FR:
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_FR_DEFINED */
#ifdef QDSP_MODULE_TUNES_AAC_INCALL_MIXER_FR_DEFINED
    case QDSP_MODULE_TUNES_AAC_INCALL_MIXER_FR:
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_FR_DEFINED */
#ifdef QDSP_MODULE_SYNTH_INCALL_MIXER_HR_DEFINED
    case QDSP_MODULE_SYNTH_INCALL_MIXER_HR:
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_HR_DEFINED */
#ifdef QDSP_MODULE_TUNES_INCALL_MIXER_HR_DEFINED
    case QDSP_MODULE_TUNES_INCALL_MIXER_HR:
#endif /* QDSP_MODULE_TUNES_INCALL_MIXER_HR_DEFINED */
#ifdef QDSP_MODULE_TUNES_WMA_INCALL_MIXER_HR_DEFINED
    case QDSP_MODULE_TUNES_WMA_INCALL_MIXER_HR:
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_HR_DEFINED */
#ifdef QDSP_MODULE_TUNES_AAC_INCALL_MIXER_HR_DEFINED
    case QDSP_MODULE_TUNES_AAC_INCALL_MIXER_HR:
#endif  /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_HR_DEFINED */
    ret_val = TRUE;
      break;

    default:
      break;
    }

    return ret_val;
  }

/*===========================================================================

FUNCTION:  voc_state_choose_incall_module

DESCRIPTION
  This function chooses

DEPENDENCIES


RETURN VALUE


SIDE EFFECTS

===========================================================================*/
static qdsp_module_type voc_state_choose_incall_module(
  voc_capability_type snd_cap,
  voc_capability_type voc_cap)
{
  qdsp_module_type module = QDSP_MODULE_MAX;

  switch(voc_cap) {
    case VOC_CAP_IS733:
      switch(snd_cap) {
#if defined(QDSP_MODULE_TUNES_INCALL_13K_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_13K;
          break;
#elif defined(QDSP_MODULE_TUNES_INCALL_MIXER_13K_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_MIXER_13K;
          break;
#endif /* QDSP_MODULE_TUNES_INCALL_13K_DEFINED
          QDSP_MODULE_TUNES_INCALL_MIXER_13K_DEFINED */

#ifdef FEATURE_AVS_IN_CALL_WMA
#if defined(QDSP_MODULE_TUNES_WMA_INCALL_MIXER_13K_DEFINED)
        case VOC_CAP_QTUNES_WMA:
          module = QDSP_MODULE_TUNES_WMA_INCALL_MIXER_13K;
          break;
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_13K_DEFINED */
#endif /* FEATURE_AVS_IN_CALL_WMA */

#ifdef FEATURE_AVS_IN_CALL_AAC
#if defined(QDSP_MODULE_TUNES_AAC_INCALL_MIXER_13K_DEFINED)
        case VOC_CAP_QTUNES_AAC:
          module = QDSP_MODULE_TUNES_AAC_INCALL_MIXER_13K;
          break;
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_13K_DEFINED */
#endif /*FEATURE_AVS_IN_CALL_AAC */

#if defined(QDSP_MODULE_SYNTH_INCALL_MIXER_13K_DEFINED)
        case VOC_CAP_QSYNTH:
          module = QDSP_MODULE_SYNTH_INCALL_MIXER_13K;
          break;
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_13K_DEFINED */
        }
      break;
  case VOC_CAP_IS127:
     switch(snd_cap) {
#if defined(QDSP_MODULE_TUNES_INCALL_EVRC_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_EVRC;
          break;
#elif defined(QDSP_MODULE_TUNES_INCALL_MIXER_EVRC_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_MIXER_EVRC;
          break;
#endif /* QDSP_MODULE_TUNES_INCALL_EVRC_DEFINED
          QDSP_MODULE_TUNES_INCALL_MIXER_EVRC_DEFINED */

#ifdef FEATURE_AVS_IN_CALL_WMA
#if defined(QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EVRC_DEFINED)
        case VOC_CAP_QTUNES_WMA:
          module = QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EVRC;
        break;
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EVRC_DEFINED */
#endif /* FEATURE_AVS_IN_CALL_WMA */

#ifdef FEATURE_AVS_IN_CALL_AAC
#if defined(QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EVRC_DEFINED)
        case VOC_CAP_QTUNES_AAC:
          module = QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EVRC;
          break;
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EVRC_DEFINED */
#endif /*FEATURE_AVS_IN_CALL_AAC */

#if defined(QDSP_MODULE_SYNTH_INCALL_MIXER_EVRC_DEFINED)
        case VOC_CAP_QSYNTH:
          module = QDSP_MODULE_SYNTH_INCALL_MIXER_EVRC;
          break;
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_EVRC_DEFINED */
      }
    break;
  case VOC_CAP_AMR:
     switch(snd_cap) {

#if defined(QDSP_MODULE_TUNES_INCALL_AMR_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_AMR;
          break;
#elif defined(QDSP_MODULE_TUNES_INCALL_MIXER_AMR_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_MIXER_AMR;
          break;
#endif /* QDSP_MODULE_TUNES_INCALL_AMR_DEFINED
          QDSP_MODULE_TUNES_INCALL_MIXER_AMR_DEFINED */
#ifdef FEATURE_AVS_IN_CALL_WMA
#if defined(QDSP_MODULE_TUNES_WMA_INCALL_MIXER_AMR_DEFINED)
        case VOC_CAP_QTUNES_WMA:
          module = QDSP_MODULE_TUNES_WMA_INCALL_MIXER_AMR;
          break;
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_AMR_DEFINED */
#endif /* FEATURE_AVS_IN_CALL_WMA */
#ifdef FEATURE_AVS_IN_CALL_AAC
#if defined(QDSP_MODULE_TUNES_AAC_INCALL_MIXER_AMR_DEFINED)
        case VOC_CAP_QTUNES_AAC:
          module = QDSP_MODULE_TUNES_AAC_INCALL_MIXER_AMR;
          break;
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_AMR_DEFINED*/
#endif /* FEATURE_AVS_IN_CALL_AAC */

#if defined(QDSP_MODULE_SYNTH_INCALL_MIXER_AMR_DEFINED)
        case VOC_CAP_QSYNTH:
          module = QDSP_MODULE_SYNTH_INCALL_MIXER_AMR;
          break;
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_AMR_DEFINED */
      }
     break;
  case VOC_CAP_GSM_EFR:
     switch(snd_cap) {

#if defined(QDSP_MODULE_TUNES_INCALL_EFR_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_EFR;
          break;
#elif defined(QDSP_MODULE_TUNES_INCALL_MIXER_EFR_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_MIXER_EFR;
          break;
#endif /* QDSP_MODULE_TUNES_INCALL_EFR_DEFINED
          QDSP_MODULE_TUNES_INCALL_MIXER_EFR_DEFINED  */

#ifdef FEATURE_AVS_IN_CALL_WMA
#if defined(QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EFR_DEFINED)
        case VOC_CAP_QTUNES_WMA:
          module = QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EFR;
          break;
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_EFR_DEFINED */
#endif /* FEATURE_AVS_IN_CALL_WMA */

#ifdef FEATURE_AVS_IN_CALL_AAC
#if defined(QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EFR_DEFINED)
        case VOC_CAP_QTUNES_AAC:
          module = QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EFR;
          break;
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_EFR_DEFINED*/
#endif /* FEATURE_AVS_IN_CALL_AAC */

#if defined(QDSP_MODULE_SYNTH_INCALL_MIXER_EFR_DEFINED)
        case VOC_CAP_QSYNTH:
          module = QDSP_MODULE_SYNTH_INCALL_MIXER_EFR;
          break;
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_EFR_DEFINED */
        }
        break;
  case VOC_CAP_GSM_FR:
     switch(snd_cap) {

#if defined(QDSP_MODULE_TUNES_INCALL_FR_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_FR;
          break;
#elif defined(QDSP_MODULE_TUNES_INCALL_MIXER_FR_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_MIXER_FR;
          break;
#endif /* QDSP_MODULE_TUNES_INCALL_FR_DEFINED
          QDSP_MODULE_TUNES_INCALL_MIXER_FR_DEFINED */

#ifdef FEATURE_AVS_IN_CALL_WMA
#if defined(QDSP_MODULE_TUNES_WMA_INCALL_MIXER_FR_DEFINED)
        case VOC_CAP_QTUNES_WMA:
          module = QDSP_MODULE_TUNES_WMA_INCALL_MIXER_FR;
          break;
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_FR_DEFINED */
#endif /* FEATURE_AVS_IN_CALL_WMA */

#ifdef FEATURE_AVS_IN_CALL_AAC
#if defined(QDSP_MODULE_TUNES_AAC_INCALL_MIXER_FR_DEFINED)
        case VOC_CAP_QTUNES_AAC:
          module = QDSP_MODULE_TUNES_AAC_INCALL_MIXER_FR;
          break;
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_FR_DEFINED */
#endif /*FEATURE_AVS_IN_CALL_AAC */

#if defined(QDSP_MODULE_SYNTH_INCALL_MIXER_FR_DEFINED)
        case VOC_CAP_QSYNTH:
          module = QDSP_MODULE_SYNTH_INCALL_MIXER_FR;
          break;
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_FR_DEFINED */
        }
        break;
  case VOC_CAP_GSM_HR:
     switch(snd_cap) {

#if defined(QDSP_MODULE_TUNES_INCALL_HR_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_HR;
          break;
#elif defined(QDSP_MODULE_TUNES_INCALL_MIXER_HR_DEFINED)
        case VOC_CAP_QTUNES:
          module = QDSP_MODULE_TUNES_INCALL_MIXER_HR;
          break;
#endif /* QDSP_MODULE_TUNES_INCALL_HR_DEFINED
          QDSP_MODULE_TUNES_INCALL_MIXER_HR_DEFINED */

#ifdef FEATURE_AVS_IN_CALL_WMA
#if defined(QDSP_MODULE_TUNES_WMA_INCALL_MIXER_HR_DEFINED)
        case VOC_CAP_QTUNES_WMA:
          module = QDSP_MODULE_TUNES_WMA_INCALL_MIXER_HR;
          break;
#endif /* QDSP_MODULE_TUNES_WMA_INCALL_MIXER_HR_DEFINED */
#endif /*FEATURE_AVS_IN_CALL_WMA */

#ifdef FEATURE_AVS_IN_CALL_AAC
#if defined(QDSP_MODULE_TUNES_AAC_INCALL_MIXER_HR_DEFINED)
        case VOC_CAP_QTUNES_AAC:
          module = QDSP_MODULE_TUNES_AAC_INCALL_MIXER_HR;
          break;
#endif /* QDSP_MODULE_TUNES_AAC_INCALL_MIXER_HR_DEFINED */
#endif /*FEATURE_AVS_IN_CALL_AAC */

#if defined(QDSP_MODULE_SYNTH_INCALL_MIXER_HR_DEFINED)
        case VOC_CAP_QSYNTH:
          module = QDSP_MODULE_SYNTH_INCALL_MIXER_HR;
          break;
#endif /* QDSP_MODULE_SYNTH_INCALL_MIXER_HR_DEFINED */
        }
        break;
  }

  if (module == QDSP_MODULE_MAX) {
    MSG_HIGH("Un supported Audio option in call audio", 0, 0, 0);
  }

  return module;
}

/*===========================================================================

FUNCTION:  voc_state_check_incall_adec_module

DESCRIPTION
  This function checks whether incall audio module should be selected.

DEPENDENCIES


RETURN VALUE


SIDE EFFECTS

===========================================================================*/
boolean voc_state_check_incall_adec_module(qdsp_module_type *module_select)
{
  boolean ret_val = FALSE;

  /* First IF condition catches the scenario during voice call,
   * an audio (MP3/AAC) clip starts to play
   */
  if ((voc_state_data.config_pending != VOC_CAP_QTUNES)         &&
      (voc_state_data.config_pending != VOC_CAP_QTUNES_AAC)     &&
#ifdef FEATURE_WMA
      (voc_state_data.config_pending != VOC_CAP_QTUNES_WMA)     &&
#endif /* FEATURE_WMA*/
      (voc_state_data.audfmt_mix_mode == VOC_ADEC_MIX_MODE_RX)  &&
      (voc_state_data.num_higher_priority_apps == 0)            &&
      (voc_state_data.acquire_mask & ((word) 1 << VOC_ACQ_SND)) &&
      (voc_state_data.config_mask  & ((word) 1 << VOC_ACQ_SND)) &&
      (voc_get_desired_config(VOC_ACQ_SND) == VOC_CAP_QTUNES)) {

    ret_val = TRUE;

   if ((*module_select = voc_state_choose_incall_module(
        voc_get_desired_config(VOC_ACQ_SND),
        voc_state_data.config_pending)) != QDSP_MODULE_MAX)
      {
        ret_val = TRUE;
      }
    return (ret_val);
  } /* if */

  /* Second IF condition catches the scenario during an audio (MP3/AAC)
   * playback, a voice call is coming.
   */
#if defined(FEATURE_WMA) || defined(FEATURE_QTUNES_AAC) || defined(FEATURE_MP3)
  if (((voc_state_data.config_pending == VOC_CAP_QTUNES) ||
      (voc_state_data.config_pending == VOC_CAP_QTUNES_AAC)
#ifdef FEATURE_WMA
      ||(voc_state_data.config_pending == VOC_CAP_QTUNES_WMA)
#endif
#ifdef FEATURE_AVS_INCALL_MIXER_CMX
      || (voc_state_data.config_pending == VOC_CAP_QSYNTH)
#endif /* FEATURE_AVS_INCALL_MIXER_CMX */
      ) && (voc_state_data.num_higher_priority_apps == 0)
      && (voc_state_data.audfmt_mix_mode == VOC_ADEC_MIX_MODE_RX)) {

    if ((voc_state_data.acquire_mask & ((word) 1 << VOC_ACQ_TEST)) &&
        (voc_state_data.config_mask & ((word) 1 << VOC_ACQ_TEST))) {
      if ((*module_select = voc_state_choose_incall_module(
         voc_state_data.config_pending,
         voc_get_desired_config(VOC_ACQ_TEST))) != QDSP_MODULE_MAX) {
              ret_val = TRUE;
      }
    }

    if ((voc_state_data.acquire_mask & ((word) 1 << VOC_ACQ_CDMA)) &&
        (voc_state_data.config_mask & ((word) 1 << VOC_ACQ_CDMA))) {
      /* This is for 1X vocoders */

      if ((*module_select = voc_state_choose_incall_module(
         voc_state_data.config_pending,
         voc_get_desired_config(VOC_ACQ_CDMA))) != QDSP_MODULE_MAX) {
      ret_val = TRUE;
      }

    } /* if 1X call */
#ifdef FEATURE_MVS
    else if ((voc_state_data.acquire_mask & ((word) 1 << VOC_ACQ_MVS))  &&
             (voc_state_data.config_mask  & ((word) 1 << VOC_ACQ_MVS))) {

      if ((*module_select = voc_state_choose_incall_module(
         voc_state_data.config_pending,
         voc_get_desired_config(VOC_ACQ_MVS))) != QDSP_MODULE_MAX) {
        ret_val = TRUE;
      }
    } /* if UMTS/GSM call */
#endif /* FEATURE_MVS */

    return (ret_val);
  } /* if */
#endif /* defined(FEATURE_WMA) || defined(FEATURE_QTUNES_AAC) || defined(FEATURE_MP3) */

  return (ret_val);
}

#endif /* FEATURE_AVS_INCALL_ADEC_PB ||
          FEATURE_AVS_INCALL_MIXER_CMX ||
          FEATURE_AVS_INCALL_MIXER_ADEC_PB */

/* <EJECT> */
/*===========================================================================

FUNCTION:  voc_state_get_service_cfg_required

DESCRIPTION
  This function returns the service option for the required config_capability.

DEPENDENCIES


RETURN VALUE


SIDE EFFECTS

===========================================================================*/
voc_service_cfg_type voc_state_get_service_cfg_required()
{
  voc_service_cfg_type service;

  switch(voc_state_data.config_pending)
  {
    case VOC_CAP_IS733:
    case VOC_CAP_IS127:
    case VOC_CAP_4GV_NB:
    	  service = VOC_SERVICE_CDMA;
	  break;
    case VOC_CAP_4GV_WB:
	  service = VOC_SERVICE_CDMA_WB;
	  break;
    case VOC_CAP_AMR:
    case VOC_CAP_GSM_EFR:
    case VOC_CAP_GSM_FR:
    case VOC_CAP_GSM_HR:
    case VOC_CAP_G711:
	  service = VOC_SERVICE_UMTS;
          break;
    case VOC_CAP_AMR_WB:
          service = VOC_SERVICE_UMTS_WB;
          break;
    default:
	  service = VOC_SERVICE_MAX;
          break;
  }
  return(service);
}

/*===========================================================================

FUNCTION voc_state_get_high_module

DESCRIPTION
  This function is used to decide what QDSP module to request given the
  current capabilities requested by vocoder clients.

  This function is called not only to retrieve the module to request, but
  also called to retrieve the module enabled to compare against the module
  requested.

DEPENDENCIES
  voc_state_data.acquire_mask
  voc_state_data.adpcm_mode_enabled
  voc_state_data.config_mask
  voc_state_data.config_pending
  voc_state_data.module_reqd
  voc_state_data.num_higher_priority_apps
  voc_state_data.pcm_int_mode_pending
  voc_state_data.vr_mode

RETURN VALUE
  VOC_DONE_S     - Module returned may be successfully acquired at the time
                   of the call to this function.
  VOC_CMD_BUSY_S - Desired module will be unsuccessfully acquired at the
                   time of the call to this function due to another active
                   DSP client having priority. Module returned is
                   "next best".

SIDE EFFECTS
  None

===========================================================================*/
voc_status_type voc_state_get_high_module (
  qdsp_module_type *module_select
)
{
  voc_status_type status = VOC_DONE_S;
  voc_client_cap_dir_type clients_dir;

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

#ifdef FEATURE_AVS_I2SSBC
  if(voc_state_data.acquire_mask & ((word) 1 << VOC_ACQ_I2SSBC))
  {
    *module_select = QDSP_MODULE_I2SSBC;
  } else
#endif /* FEATURE_AVS_I2SSBC */

#ifdef FEATURE_PHONE_VR
  if (voc_state_data.vr_mode == VOC_VR_MODE_BC) {
    *module_select = QDSP_MODULE_VRBC;
  } else
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_ADPCM
  if (voc_state_data.adpcm_mode_enabled) {
#ifdef FEATURE_VOC_G711
    if(voc_state_data.wav_config == VOC_ADPCM_WAV_MULAW_MODE ||
       voc_state_data.wav_config == VOC_ADPCM_WAV_ALAW_MODE) {
      *module_select = QDSP_MODULE_G711;
    } else
#endif /* FEATURE_VOC_G711 */
#ifdef FEATURE_VOC_ADPCM
    if (voc_state_data.adpcm_voc_config) {
#ifdef FEATURE_AVS_E90
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
      *module_select = QDSP_MODULE_ACC_ADPCM;
#else
     *module_select = MSMAUD_GET_MODULE(QDSP_MODULE_VOC_ADPCM);
#endif /*  FEATURE_AUDIO_CAMERA_CONCURRENCY */

#else
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
      *module_select = QDSP_MODULE_ACC_ADPCM;
#else
     *module_select = QDSP_MODULE_VOC_ADPCM;
#endif /*  FEATURE_AUDIO_CAMERA_CONCURRENCY */
#endif
    } else
#endif /* FEATURE_VOC_ADPCM */
    {
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif
      *module_select = QDSP_MODULE_ADPCM;
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif
    }
  } else
#endif

#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  if (voc_state_check_incall_adec_module(module_select) == FALSE)
#endif /* Check incall audio */
  {
    clients_dir =
      voc_clients_requested_cap_dir(voc_state_data.config_pending);

    switch (voc_state_data.config_pending) {

#if defined QDSP_MODULE_4GV_DEFINED     || \
    defined QDSP_MODULE_4GV_DEC_DEFINED || \
    defined QDSP_MODULE_4GV_ENC_DEFINED
      case VOC_CAP_4GV_NB:
#ifdef QDSP_MODULE_4GV_DEFINED
        *module_select = QDSP_MODULE_4GV;
#else
        status = voc_state_get_module_for_basic_audio(module_select);
#endif

        if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_4GV_DEC_DEFINED
          *module_select = QDSP_MODULE_4GV_DEC;
#endif
        }
        else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_4GV_ENC_DEFINED
          *module_select = QDSP_MODULE_4GV_ENC;
#endif
        }
        break;
#endif /*
       **#if defined QDSP_MODULE_4GV_DEFINED     || \
       **    defined QDSP_MODULE_4GV_DEC_DEFINED || \
       **    defined QDSP_MODULE_4GV_ENC_DEFINED
       */

#ifdef QDSP_MODULE_4GV_VOIP_DEFINED
#error code not present
#endif

#if defined QDSP_MODULE_4GV_WB_DEFINED     || \
    defined QDSP_MODULE_4GV_WB_DEC_DEFINED || \
    defined QDSP_MODULE_4GV_WB_ENC_DEFINED
      case VOC_CAP_4GV_WB:
#ifdef QDSP_MODULE_4GV_WB_DEFINED
        *module_select = QDSP_MODULE_4GV_WB;
#else
        status = voc_state_get_module_for_basic_audio(module_select);
#endif

        if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_4GV_DEC_DEFINED
          *module_select = QDSP_MODULE_4GV_WB_DEC;
#endif
        }
        else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_4GV_WB_ENC_DEFINED
          *module_select = QDSP_MODULE_4GV_WB_ENC;
#endif
        }
        break;
#endif /*
       **#if defined QDSP_MODULE_4GV_WB_DEFINED     || \
       **    defined QDSP_MODULE_4GV_WB_DEC_DEFINED || \
       **    defined QDSP_MODULE_4GV_WB_ENC_DEFINED
       */

#ifdef QDSP_MODULE_4GV_WB_VOIP_DEFINED
#error code not present
#endif

#if defined QDSP_MODULE_13K_E90_DEFINED     || \
    defined QDSP_MODULE_13K_ENC_E90_DEFINED || \
    defined QDSP_MODULE_13K_DEC_E90_DEFINED || \
    defined QDSP_MODULE_13K_DEFINED         || \
    defined QDSP_MODULE_13K_ENC_DEFINED     || \
    defined QDSP_MODULE_13K_DEC_DEFINED
      case VOC_CAP_IS733:
#ifdef FEATURE_AVS_E90

#ifdef QDSP_MODULE_13K_E90_DEFINED
      *module_select = MSMAUD_GET_MODULE(QDSP_MODULE_13K);
#else
        status = voc_state_get_module_for_basic_audio(module_select);
#endif

        if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_13K_DEC_E90_DEFINED
          *module_select = MSMAUD_GET_MODULE(QDSP_MODULE_13K_DEC);
#endif
        }
        else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_13K_ENC_E90_DEFINED
          *module_select = MSMAUD_GET_MODULE(QDSP_MODULE_13K_ENC);
#endif
        }

#else /* FEATURE_AVS_E90 */

#ifdef QDSP_MODULE_13K_DEFINED
      *module_select = QDSP_MODULE_13K;
#else
          status = voc_state_get_module_for_basic_audio(module_select);
#endif

          if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_13K_DEC_DEFINED
            *module_select = QDSP_MODULE_13K_DEC;
#endif
          }
          else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_13K_ENC_DEFINED
            *module_select = QDSP_MODULE_13K_ENC;
#endif
          }
#endif /* FEATURE_AVS_E90 */
        break;
#endif  /*
        **#if defined QDSP_MODULE_13K_E90_DEFINED     || \
        **    defined QDSP_MODULE_13K_ENC_E90_DEFINED || \
        **    defined QDSP_MODULE_13K_DEC_E90_DEFINED || \
        **    defined QDSP_MODULE_13K_DEFINED         || \
        **    defined QDSP_MODULE_13K_ENC_DEFINED     || \
        **    defined QDSP_MODULE_13K_DEC_DEFINED
        */

#if defined QDSP_MODULE_EVRC_E90_DEFINED     || \
    defined QDSP_MODULE_EVRC_ENC_E90_DEFINED || \
    defined QDSP_MODULE_EVRC_DEC_E90_DEFINED || \
    defined QDSP_MODULE_EVRC_DEFINED         || \
    defined QDSP_MODULE_EVRC_ENC_DEFINED     || \
    defined QDSP_MODULE_EVRC_DEC_DEFINED
      case VOC_CAP_IS127:
#ifdef FEATURE_AVS_E90

#ifdef QDSP_MODULE_EVRC_E90_DEFINED
      *module_select = MSMAUD_GET_MODULE(QDSP_MODULE_EVRC);
#else
        status = voc_state_get_module_for_basic_audio(module_select);
#endif

        if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_EVRC_DEC_E90_DEFINED
          *module_select = MSMAUD_GET_MODULE(QDSP_MODULE_EVRC_DEC);
#endif
        }
        else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_EVRC_ENC_E90_DEFINED
          *module_select = MSMAUD_GET_MODULE(QDSP_MODULE_EVRC_ENC);
#endif
        }

#else /* FEATURE_AVS_E90 */

#ifdef QDSP_MODULE_EVRC_DEFINED
      *module_select = QDSP_MODULE_EVRC;
#else
          status = voc_state_get_module_for_basic_audio(module_select);
#endif

          if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_EVRC_DEC_DEFINED
            *module_select = QDSP_MODULE_EVRC_DEC;
#endif
          }
          else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_EVRC_ENC_DEFINED
            *module_select = QDSP_MODULE_EVRC_ENC;
#endif
          }

#endif /* FEATURE_AVS_E90 */
        break;
#endif  /*
        **#if defined QDSP_MODULE_EVRC_E90_DEFINED     || \
        **    defined QDSP_MODULE_EVRC_ENC_E90_DEFINED || \
        **    defined QDSP_MODULE_EVRC_DEC_E90_DEFINED || \
        **    defined QDSP_MODULE_EVRC_DEFINED         || \
        **    defined QDSP_MODULE_EVRC_ENC_DEFINED     || \
        **    defined QDSP_MODULE_EVRC_DEC_DEFINED
        */

#ifdef QDSP_MODULE_EVRC_VOIP_DEFINED
      case VOC_CAP_IS127_VOIP:
        *module_select = QDSP_MODULE_EVRC_VOIP;
      break;
#endif


#ifdef FEATURE_ACP
#error code not present
#endif

#if defined QDSP_MODULE_AMR_WB_DEFINED      || \
    defined QDSP_MODULE_AMR_WB_DEC_DEFINED  || \
    defined QDSP_MODULE_AMR_WB_ENC_DEFINED
      case VOC_CAP_AMR_WB:
        if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_AMR_WB_DEC_DEFINED
#error code not present
#endif /* QDSP_MODULE_AMR_WB_DEC_DEFINED */
        }
        else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_AMR_WB_ENC_DEFINED
#error code not present
#endif /* QDSP_MODULE_AMR_WB_ENC_DEFINED */
        }
        else if (clients_dir == VOC_CLIENT_BOTH_DIR) {
#ifdef QDSP_MODULE_AMR_WB_DEFINED
        *module_select = QDSP_MODULE_AMR_WB;
#endif /* QDSP_MODULE_AMR_WB_DEFINED */
        }
        break;
#endif /*
       ** defined QDSP_MODULE_AMR_WB_DEFINED      || \
       ** defined QDSP_MODULE_AMR_WB_DEC_DEFINED  || \
       ** defined QDSP_MODULE_AMR_WB_ENC_DEFINED
       */


#if defined QDSP_MODULE_AMR_DEFINED      || \
    defined QDSP_MODULE_AMR_DEC_DEFINED  || \
    defined QDSP_MODULE_AMR_ENC_DEFINED
      case VOC_CAP_AMR:

#ifdef QDSP_MODULE_AMR_DEFINED
        *module_select = QDSP_MODULE_AMR;
#else
          status = voc_state_get_module_for_basic_audio(module_select);
#endif

          if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_AMR_DEC_DEFINED
            *module_select = QDSP_MODULE_AMR_DEC;
#endif
          }
          else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_AMR_ENC_DEFINED
            *module_select = QDSP_MODULE_AMR_ENC;
#endif
          }

        break;
#endif  /*
        **#if defined QDSP_MODULE_AMR_DEFINED      || \
        **    defined QDSP_MODULE_AMR_DEC_DEFINED  || \
        **    defined QDSP_MODULE_AMR_ENC_DEFINED
        */

#if defined QDSP_MODULE_EFR_DEFINED      || \
    defined QDSP_MODULE_EFR_DEC_DEFINED  || \
    defined QDSP_MODULE_EFR_ENC_DEFINED
      case VOC_CAP_GSM_EFR:

#ifdef QDSP_MODULE_EFR_DEFINED
        *module_select = QDSP_MODULE_EFR;
#else
          status = voc_state_get_module_for_basic_audio(module_select);
#endif
          if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_EFR_DEC_DEFINED
            *module_select = QDSP_MODULE_EFR_DEC;
#endif
          }
          else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_EFR_ENC_DEFINED
            *module_select = QDSP_MODULE_EFR_ENC;
#endif
          }

        break;
#endif  /*
        **#if defined QDSP_MODULE_EFR_DEFINED      || \
        **    defined QDSP_MODULE_EFR_DEC_DEFINED  || \
        **    defined QDSP_MODULE_EFR_ENC_DEFINED
        */

#if defined QDSP_MODULE_FR_DEFINED      || \
    defined QDSP_MODULE_FR_DEC_DEFINED  || \
    defined QDSP_MODULE_FR_ENC_DEFINED
      case VOC_CAP_GSM_FR:
#ifdef QDSP_MODULE_FR_DEFINED
        *module_select = QDSP_MODULE_FR;
#else
          status = voc_state_get_module_for_basic_audio(module_select);
#endif

          if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_FR_DEC_DEFINED
            *module_select = QDSP_MODULE_FR_DEC;
#endif
          }
          else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_FR_ENC_DEFINED
            *module_select = QDSP_MODULE_FR_ENC;
#endif
          }
        break;
#endif  /*
        **#if defined QDSP_MODULE_FR_DEFINED      || \
        **    defined QDSP_MODULE_FR_DEC_DEFINED  || \
        **    defined QDSP_MODULE_FR_ENC_DEFINED
        */

#if defined QDSP_MODULE_HR_DEFINED      || \
    defined QDSP_MODULE_HR_DEC_DEFINED  || \
    defined QDSP_MODULE_HR_ENC_DEFINED
      case VOC_CAP_GSM_HR:
#ifdef QDSP_MODULE_HR_DEFINED
        *module_select = QDSP_MODULE_HR;
#else
          status = voc_state_get_module_for_basic_audio(module_select);
#endif
          if(clients_dir == VOC_CLIENT_RX_ONLY){
#ifdef QDSP_MODULE_HR_DEC_DEFINED
            *module_select = QDSP_MODULE_HR_DEC;
#endif
          }
          else if(clients_dir == VOC_CLIENT_TX_ONLY){
#ifdef QDSP_MODULE_HR_ENC_DEFINED
            *module_select = QDSP_MODULE_HR_ENC;
#endif
          }
        break;
#endif  /*
        **#if defined QDSP_MODULE_HR_DEFINED      || \
        **    defined QDSP_MODULE_HR_DEC_DEFINED  || \
        **    defined QDSP_MODULE_HR_ENC_DEFINED
        */

#ifdef QDSP_MODULE_G711_DEFINED
      case VOC_CAP_G711:
        *module_select = QDSP_MODULE_G711;
        break;
#endif
#ifdef QDSP_MODULE_G723_DEFINED
      case VOC_CAP_G723:
        *module_select = QDSP_MODULE_G723;
        break;
#endif
#ifdef FEATURE_QSYNTH_COMMON
      case VOC_CAP_QSYNTH:
        *module_select = QDSP_MODULE_QSYNTH;
        break;
#endif
#ifdef FEATURE_QTUNES
      case VOC_CAP_QTUNES:
        *module_select = voc_data_get_qtunes_module(VOC_CAP_QTUNES);
        break;
#endif /* FEATURE_QTUNES */
#ifdef FEATURE_QTUNES_AAC
      case VOC_CAP_QTUNES_AAC:
        *module_select = voc_data_get_qtunes_module(VOC_CAP_QTUNES_AAC);
        break;
#endif
#ifdef FEATURE_MM_REC
#ifdef FEATURE_AAC_REC
      case VOC_CAP_AAC_REC:
#ifdef QDSP_MODULE_AACENC_DEFINED
        *module_select = QDSP_MODULE_AACENC;
#endif
        break;
#endif
#endif
#ifdef FEATURE_REAL_AUDIO
      case VOC_CAP_QTUNES_RA:
        *module_select = QDSP_MODULE_RA;
        break;
#endif
#ifdef FEATURE_WMA
      case VOC_CAP_QTUNES_WMA:
        *module_select = QDSP_MODULE_WMA;
        break;
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif

#endif
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case VOC_CAP_QTUNES_AMR_WBPLUS:
        *module_select = QDSP_MODULE_AMRWBP;
        break;
#endif
      case VOC_CAP_NONE:
      default:
#ifdef FEATURE_VOC_PCM_INTERFACE
        if (voc_state_data.pcm_int_mode_pending) {
          *module_select = QDSP_MODULE_PCM;
        } else
#endif /* FEATURE_VOC_PCM_INTERFACE */
        {
          return voc_state_get_module_for_basic_audio(module_select);
        }
        break;
    }
  }

  if (voc_state_data.num_higher_priority_apps > 0) {
    if (!qdsp_module_currently_available(*module_select)) {

      qdsp_module_type module_reqd = *module_select;

      if(qdsp_higher_app_module_enabled(QDSP_APP_VOC) == FALSE)
        return VOC_DONE_S;

      if (voc_state_data.module_status >= VOC_STATE_MOD_STATUS_ENABLED) {
        *module_select = voc_state_data.module_reqd;
      }
      else {
        (void)voc_state_get_module_for_basic_audio(module_select);
      }
      status = VOC_CMD_BUSY_S;
      MSG_HIGH("Required module = %d is not supported on image = %d, "
               "Continuing with module = %d",
               module_reqd, voc_state_data.current_image,*module_select);
    }
  }
  return(status);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_set_codec_cal

DESCRIPTION
  Sets the calibration for the codec specified.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_state_set_codec_cal ( voc_codec_type pcm_path )
{
#ifdef FEATURE_PHONE_VR
  voc_vr_cal_type  *vr_cal_ptr;
#endif

  /* Save the codec selection
  */
  if (voc_data_set_codec_cal(pcm_path, voc_state_data.current_image)
                                                          == VOC_DATA_FAILURE) {
    ERR_FATAL("Illegal codec selection",0,0,0);
  }

#ifdef FEATURE_PHONE_VR
  /* The the VR Engine which codec is in use
  */
  (void) vre_set_path(pcm_path, &vr_cal_ptr);
  voc_data_set_vr_cal(vr_cal_ptr);

#endif  /* FEATURE_PHONE_VR */
} /* voc_state_set_codec_cal */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
#ifdef FEATURE_EXTERNAL_SDAC
/*===========================================================================

FUNCTION voc_state_config_external_sdac

DESCRIPTION
  Reconfigure MSM and codec to new sample rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_state_config_external_sdac (
  voc_sample_rate_type rate
)
{
  byte sdac_val;

  /* Select the SDAC's sample rate */
  switch(rate) {
    case VOC_SAMPLE_RATE_48000:
    case VOC_SAMPLE_RATE_44100:
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
    case VOC_SAMPLE_RATE_44000:
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
      sdac_val = SDAC_SR_SAMP_RATE_48_V;
      break;

    case VOC_SAMPLE_RATE_32000:
      sdac_val = SDAC_SR_SAMP_RATE_32_V;
      break;

    case VOC_SAMPLE_RATE_24000:
    case VOC_SAMPLE_RATE_22050:
      sdac_val = SDAC_SR_SAMP_RATE_24_V;
      break;

    case VOC_SAMPLE_RATE_16000:
      sdac_val = SDAC_SR_SAMP_RATE_16_V;
      break;

    case VOC_SAMPLE_RATE_12000:
    case VOC_SAMPLE_RATE_11025:
      sdac_val = SDAC_SR_SAMP_RATE_12_V;
      break;

    case VOC_SAMPLE_RATE_8000:
      sdac_val = SDAC_SR_SAMP_RATE_08_V;
      break;

    default:
      MSG_ERROR("Unknown sample rate = 0x%02x",rate,0,0);
      /* FALL THROUGH */

    case VOC_SAMPLE_RATE_NONE:
      (void) sdac_enable(FALSE);
      MSMAUD_DISABLE_EXTERNAL_SDAC_CLOCKS();
      return;
  }

  MSMAUD_ENABLE_EXTERNAL_SDAC_CLOCKS(rate);
  (void) sdac_set_sample_rate(sdac_val);
  (void) sdac_enable(TRUE);
}
#endif /* FEATURE_EXTERNAL_SDAC */

#ifdef FEATURE_EXTERNAL_SADC
/*===========================================================================

FUNCTION voc_state_config_external_sadc

DESCRIPTION
  Reconfigure MSM and codec to new sample rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_state_config_external_sadc (
  sadc_mode_type  sadc_mode
)
{
  (void) sadc_enable(sadc_mode);

  if (sadc_mode != SADC_OFF) {

#ifdef FEATURE_CLK_MGR
    clk_regime_sel_sdac_clk(CLKRGM_SDAC_EXT_CLK) ;
#else
 #ifndef MSMAUD_QSC60X5_AUDIO_HW
    HWIO_OUT(SDAC_CLK_NS, 0x80);
 #endif
#endif /* FEATURE_CLK_MGR */
#ifdef FEATURE_DUAL_CLK_RGM
    clk_regime_msm_enable(CLK_RGM_SDAC_M);
#else
    clk_regime_enable(CLK_RGM_SDAC_M);
#endif

  } else if (sadc_mode == SADC_OFF) {

#ifdef FEATURE_DUAL_CLK_RGM
    clk_regime_msm_disable(CLK_RGM_SDAC_M);
#else
    clk_regime_disable(CLK_RGM_SDAC_M);
#endif

  }
} /* voc_state_config_external_sadc */
#endif /* FEATURE_EXTERNAL_SADC */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_set_ec

DESCRIPTION
  This function sets the echo cancellation mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - Echo canceller mode set
  VOC_STATE_FAILURE - Echo canceller mode could not be set

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_set_ec (
  voc_ec_type ec_mode,
  const qdsp_cmd_ec_params_type *ec_params
) {
  qdsp_cmd_status_type     result;
  qdsp_cmd_ec_config_type  ec_setting;

  /* Don't reset echo canceller
  */
  ec_setting.ec_reset_flag = (QDSP_ECHO_RES_EC_DIS_V |
                              QDSP_ECHO_RES_ECBNE_DIS_V);

  switch (ec_mode) {
    case VOC_EC_OFF:
      ec_setting.ec_switch = QDSP_ECHO_CANCEL_DIS_V;
      break;
    case VOC_EC_ESEC:
    case VOC_EC_BT_HEADSET:
      ec_setting.ec_switch = QDSP_ECHO_CANCEL_ENA_V;
      ec_setting.ec_mode   = QDSP_ECHO_CANCEL_ESEC_V;
      break;
    case VOC_EC_HEADSET:
      ec_setting.ec_switch = QDSP_ECHO_CANCEL_ENA_V;
      ec_setting.ec_mode   = QDSP_ECHO_CANCEL_HEADSET_V;
      break;
    case VOC_EC_AEC:
      ec_setting.ec_switch = QDSP_ECHO_CANCEL_ENA_V;
      ec_setting.ec_mode   = QDSP_ECHO_CANCEL_AEC_V;
      break;
    case VOC_EC_SPEAKER:
      ec_setting.ec_switch = QDSP_ECHO_CANCEL_ENA_V;
      ec_setting.ec_mode   = QDSP_ECHO_CANCEL_SPEAKER_V;
      break;
    default:
      ERR_FATAL("Illegal echo canceller setting",0,0,0);
      break;
  }

  result = qdsp_cmd_ec_ctrl(&ec_setting,ec_params);

  if (result == QDSP_CMD_SUCCESS) {
    return(VOC_STATE_SUCCESS);
  } else {
    return(VOC_STATE_FAILURE);
  }
} /* voc_state_set_ec */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
#ifdef FEATURE_AVS_NEXTGEN_EC
/*===========================================================================

FUNCTION voc_state_set_nextgen_ec

DESCRIPTION
  This function sets the nextgen echo cancellation mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - Echo canceller mode set
  VOC_STATE_FAILURE - Echo canceller mode could not be set

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_set_nextgen_ec (
  voc_ec_type ec_mode,
  const qdsp_cmd_nextgen_ec_parms_type *ec_params
) {
  qdsp_cmd_status_type     result;
  uint16                   ec_switch = QDSP_ECHO_CANCEL_DIS_V;

  switch (ec_mode) {
    case VOC_EC_OFF:
      ec_switch = QDSP_ECHO_CANCEL_DIS_V;
      break;
    case VOC_EC_ESEC:
    case VOC_EC_HEADSET:
    case VOC_EC_AEC:
    case VOC_EC_SPEAKER:
    case VOC_EC_BT_HEADSET:
      ec_switch = QDSP_ECHO_CANCEL_ENA_V;
      break;
    default:
      ERR_FATAL("Illegal echo canceller setting",0,0,0);
      break;
  }

  result = qdsp_cmd_nextgen_ec_ctrl(ec_switch,ec_params);

  if (result == QDSP_CMD_SUCCESS) {
    return(VOC_STATE_SUCCESS);
  } else {
    return(VOC_STATE_FAILURE);
  }
} /* voc_state_set_nextgen_ec */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#endif  /* FEATURE_AVS_NEXTGEN_EC */

/* <EJECT> */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/*===========================================================================

FUNCTION voc_state_set_crystal_speech

DESCRIPTION
  This function sets the crystal speech mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - crystal speech mode set
  VOC_STATE_FAILURE - crystal speech mode could not be set

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_set_crystal_speech (
  voc_ec_type ec_mode,
  const qdsp_cmd_crystal_speech_parms_type *cs_params
) {
  qdsp_cmd_status_type     result;
  uint16                   cs_switch = QDSP_ECHO_CANCEL_DIS_V;

  switch (ec_mode) {
    case VOC_EC_OFF:
      cs_switch = QDSP_ECHO_CANCEL_DIS_V;
      break;

    case VOC_EC_HEADSET:
    case VOC_EC_AEC:
    case VOC_EC_SPEAKER:
    case VOC_EC_BT_HEADSET:
      MSG_ERROR("Crystal Speech not supported for mode %d", ec_mode, 0, 0);
      cs_switch = QDSP_ECHO_CANCEL_DIS_V;
      break;

    case VOC_EC_ESEC:
      cs_switch = QDSP_ECHO_CANCEL_ENA_V;
      break;

    default:
      ERR_FATAL("Illegal echo canceller setting %d", ec_mode, 0, 0);
      break;
  }

  result = qdsp_cmd_crystal_speech_ctrl(cs_switch, cs_params);

  if (result == QDSP_CMD_SUCCESS) {
    return(VOC_STATE_SUCCESS);
  } else {
    return(VOC_STATE_FAILURE);
  }
} /* voc_state_set_crystal_speech */

#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */


/* <EJECT> */
#ifdef FEATURE_AUDIO_AGC
/*===========================================================================

FUNCTION voc_state_set_agc

DESCRIPTION
  This function sets the AGC/AVC mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - AGC/AVC mode set
  VOC_STATE_FAILURE - AGC/AVC mode could not be set

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_set_agc (
  qdsp_cmd_agc_config_type *agc_config_ptr
)
{
  qdsp_cmd_status_type     result;
  qdsp_cmd_agc_param_type  agc_param;

  result = qdsp_cmd_agc_config(agc_config_ptr);

  if (result != QDSP_CMD_SUCCESS) {
    return(VOC_STATE_FAILURE);
  }
  voc_data_get_agc_param(&agc_param);
  result = qdsp_cmd_agc_param(&agc_param);
  if (result != QDSP_CMD_SUCCESS) {
    return(VOC_STATE_FAILURE);
  }
  return(VOC_STATE_SUCCESS);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_AUDIO_AGC */

/* <EJECT> */

/*===========================================================================

FUNCTION voc_state_set_rve

DESCRIPTION
  This function sets the RVE mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - RVE Single mode set
  VOC_STATE_FAILURE - RVE Single mode could not be set

SIDE EFFECTSy
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_set_rve (
  voc_ec_type ec_mode,
  qdsp_cmd_rve_config_type *rve_config_ptr
)
{

  qdsp_cmd_status_type      result = QDSP_CMD_SUCCESS;
  const qdsp_cmd_rve_param_type*   rve_param;


  result = qdsp_cmd_rve_config(rve_config_ptr);

  if (result != QDSP_CMD_SUCCESS) {
    return(VOC_STATE_FAILURE);
  }
  rve_param = voc_data_get_rve_param(ec_mode);

  if( rve_param != NULL  )
    result = qdsp_cmd_rve_param(rve_param);


  if (result != QDSP_CMD_SUCCESS) {
    return(VOC_STATE_FAILURE);
  }

  return(VOC_STATE_SUCCESS);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/* <EJECT> */


/*===========================================================================

FUNCTION voc_state_set_dual_rve

DESCRIPTION
  This function sets the RVE Dual Mic mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS -  RVE Dual Mode  mode set
  VOC_STATE_FAILURE -  RVE Dual Mode  could not be set

SIDE EFFECTSy
  None

===========================================================================*/
voc_state_return_type voc_state_set_dual_rve (
  voc_ec_type ec_mode,
  qdsp_cmd_rve_config_type *rve_config_ptr
)
{
  qdsp_cmd_status_type      result = QDSP_CMD_SUCCESS;
  const qdsp_cmd_dual_rve_param_type*   dual_rve_param;


  result = qdsp_cmd_rve_config(rve_config_ptr);

  if (result != QDSP_CMD_SUCCESS) {
    return(VOC_STATE_FAILURE);
  }

  dual_rve_param = voc_data_get_dual_rve_param(ec_mode);

  if( dual_rve_param != NULL )
    result = qdsp_cmd_dual_rve_param(dual_rve_param);


  if (result != QDSP_CMD_SUCCESS) {
    return(VOC_STATE_FAILURE);
  }
  return(VOC_STATE_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_set_ns

DESCRIPTION
  This function sets the noise suppressor mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - Noise suppressor mode set
  VOC_STATE_FAILURE - Noise suppressor mode could not be set

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_set_ns ( voc_ns_type ns_enable )
{
  qdsp_cmd_status_type     result;
  uint16                   ns_ctrl;

  if (ns_enable == VOC_NS_ON) {
    ns_ctrl = QDSP_NOISE_SUP_ENA_V;
  } else {
    ns_ctrl = QDSP_NOISE_SUP_DIS_V;
  }

  result = qdsp_cmd_ns_ctrl(ns_ctrl);

  if (result == QDSP_CMD_SUCCESS) {
    return(VOC_STATE_SUCCESS);
  } else {
    return(VOC_STATE_FAILURE);
  }
} /* voc_state_set_ns */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_set_vox

DESCRIPTION
  This function sets the VOX (half/full duplex voice) mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - VOX mode set
  VOC_STATE_FAILURE - VOX mode could not be set

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_set_vox ( voc_vox_type vox_enable )
{
  qdsp_cmd_status_type     result;
  uint16                   vox_ctrl;

  if (vox_enable == VOC_VOX_ON) {
    vox_ctrl = QDSP_VOX_ENA_V;
  } else {
    vox_ctrl = QDSP_VOX_DIS_V;
  }

  result = qdsp_cmd_vox_ctrl(vox_ctrl);

  if (result == QDSP_CMD_SUCCESS) {
    return(VOC_STATE_SUCCESS);
  } else {
    return(VOC_STATE_FAILURE);
  }
} /* voc_state_set_vox */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */


/*===========================================================================

FUNCTION voc_state_set_vp3_parms

DESCRIPTION
  This function sets the vp3_parms

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - VP3 params set
  VOC_STATE_FAILURE - sVP3 params could not be set

SIDE EFFECTS
  None.

===========================================================================*/
voc_state_return_type voc_state_set_vp3_parms()
{
  qdsp_cmd_vp3_config_type vp3_config;
  void* buf_ptr = NULL;
  uint32 pval = 0;
  qdsp_cmd_status_type     result;
  vocvp3_buf_id_type buf_id;

  buf_id = (vocvp3_buf_id_type)0;

  buf_ptr = voc_data_get_vp3_buf_id_and_offset(&buf_id);

  if(buf_ptr != NULL)
  {
    pval = (uint32)buf_ptr;
    vp3_config.buf_id = (uint16)buf_id;
    vp3_config.memAddrlower = (uint16) pval;
    vp3_config.memAddrupper = (uint16)((pval & 0xffff0000) >> 16);
    vp3_config.enable = VOC_STATE_VP3_ENABLE_V;
    vp3_config.partitionnum = VOC_STATE_MPU_PARTITION_NO;
    result= qdsp_cmd_vp3_config(&vp3_config);

    if (result == QDSP_CMD_SUCCESS) {
      return(VOC_STATE_SUCCESS);
    }else {
      return(VOC_STATE_FAILURE);
    }
  }else
  {
    MSG_MED("VP3 parameters are not set: buf_ptr is NULL",0,0,0);
    return(VOC_STATE_FAILURE);
  }
}


/*===========================================================================

FUNCTION voc_state_set_sidetone

DESCRIPTION
  This function sets the sidetone mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - sidetone mode set
  VOC_STATE_FAILURE - sidetone mode could not be set

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_set_sidetone (
  voc_sidetone_type sidetone_disable
)
{
  qdsp_cmd_status_type                result;
  boolean                              sidetone_allow;
  qdsp_cmd_codec_gain_ctrl_type       *codec_gain = voc_data_get_codec_gain();

  if (sidetone_disable == VOC_SIDETONE_OFF) {
    sidetone_allow = FALSE;
  } else {
    sidetone_allow = TRUE;
  }

  result = qdsp_cmd_sidetone_ctrl(sidetone_allow, codec_gain->codec_st_gain);

  if (result == QDSP_CMD_SUCCESS) {
    return(VOC_STATE_SUCCESS);
  } else {
    return(VOC_STATE_FAILURE);
  }
} /* voc_state_set_sidetone */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */

/*===========================================================================

FUNCTION voc_state_set_hpf_override

DESCRIPTION
  This function sets the hpf override mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - HPF override mode set
  VOC_STATE_FAILURE - HPF override mode could not be set

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_set_hpf_override (
  voc_state_hpf_override_type hpf_override_enable
)
{
  qdsp_cmd_status_type     result;
  uint16                   hpf_ctrl;
  voc_codec_mode_type	   mode;

  voc_data_get_codec_mode(voc_state_data.current_image, &mode);

  #ifdef FEATURE_VOC_4GV_WB
#error code not present
  #endif
  if (hpf_override_enable == VOC_STATE_HPF_OVERRIDE_ON) {
    hpf_ctrl = QDSP_HPF_OVERRIDE_ENA_V;
  } else {
    hpf_ctrl = QDSP_HPF_OVERRIDE_DIS_V;
  }

  result = qdsp_cmd_hpf_ctrl(hpf_ctrl);

  if (result == QDSP_CMD_SUCCESS) {
    return(VOC_STATE_SUCCESS);
  } else {
    return(VOC_STATE_FAILURE);
  }
} /* voc_state_set_hpf_override */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
#ifdef FEATURE_ACP
#error code not present
#endif  /* FEATURE_ACP */

/* <EJECT> */
#ifdef FEATURE_TTY
/*===========================================================================

FUNCTION voc_state_set_tty

DESCRIPTION
  This function sets the TTY mode.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - TTY mode set
  VOC_STATE_FAILURE - TTY mode could not be set

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_set_tty (
  voc_tty_mode_type tty_mode
)
{
  qdsp_cmd_status_type     result;
  uint16                   tty_ctrl;

  if ((tty_mode != VOC_TTY_MODE_OFF) && (tty_mode < VOC_TTY_MODE_MAX)
#ifdef FEATURE_ACP
#error code not present
#endif
  ) {
    tty_ctrl = QDSP_TTY_ENA_V;
  } else {
    tty_ctrl = QDSP_TTY_DIS_V;
  }


  result = qdsp_cmd_tty_ctrl(tty_ctrl, (qdsp_tty_mode_type)tty_mode);

  if (result == QDSP_CMD_SUCCESS) {
    return(VOC_STATE_SUCCESS);
  } else {
    return(VOC_STATE_FAILURE);
  }
} /* voc_state_set_tty */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_TTY */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_write_codec_gain

DESCRIPTION
  Write the gain values for the Codec into the QDSP. This function can only
  be called if the DSP is in the VOCODER or DFM state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_status_type voc_state_write_codec_gain ( void )
{
  voc_codec_class_type                 codec_type;

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
#ifdef FEATURE_INTERNAL_SADC
    case VOC_CODEC_CLASS_INTERNAL_SADC_HANDSET:
    case VOC_CODEC_CLASS_INTERNAL_SADC_HEADSET:
#endif /* FEATURE_INTERNAL_SADC */
#ifdef FEATURE_EXTERNAL_SADC
    case VOC_CODEC_CLASS_EXTERNAL_SADC_HANDSET:
    case VOC_CODEC_CLASS_EXTERNAL_SADC_HEADSET:
#endif /* FEATURE_EXTERNAL_SADC */
      (void) qdsp_cmd_codec_gain_ctrl(voc_data_get_codec_gain(),
                                      QDSP_CMD_CODEC_GAIN_OP_NORMAL);
      break;

    case VOC_CODEC_CLASS_EXTERNAL:
      (void) qdsp_cmd_set_pcmpad(voc_data_set_pcmpad_vol( 0, 0, FALSE ));
      break;

#ifdef FEATURE_EXTERNAL_SDAC
    case VOC_CODEC_CLASS_EXTERNAL_STEREO:
#endif
    case VOC_CODEC_CLASS_NONE:
      MSG_HIGH( "No CODEC to be programmed", 0, 0, 0 );
      break;
    case VOC_CODEC_CLASS_MAX:
    default:
      ERR_FATAL( "Illegal CODEC TYPE", 0, 0, 0 );
  }
  return( VOC_DONE_S );
} /* voc_state_write_codec_gain */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_init_aud_process

DESCRIPTION
  Initialize the audio processing functions.

DEPENDENCIES
  This function should only be called when the DSP is in Idle, Vocoder
  or DFM state. voc_data_set_codec must have been called to set the current
  pcm path.

RETURN VALUE
  VOC_STATE_SUCCESS - Audio processes initialized.
  VOC_STATE_FAILURE - Couldn't complete operation

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_init_aud_process()
{
  #define VOC_STATE_NUM_AUD_PROCESSES 9
  voc_state_return_type                 state_ret[VOC_STATE_NUM_AUD_PROCESSES];
  voc_ec_type*                          ec_mode_ptr = NULL;
  voc_ns_type*                          ns_mode_ptr = NULL;
  voc_vox_type*                         vox_mode_ptr = NULL;
  voc_sidetone_type*                    sidetone_mode_ptr = NULL;
  const qdsp_cmd_ec_params_type*        ec_params_ptr = NULL;
#ifdef FEATURE_AVS_NEXTGEN_EC
  const qdsp_cmd_nextgen_ec_parms_type* nextgen_ec_parms_ptr = NULL;
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  const qdsp_cmd_crystal_speech_parms_type* crystal_speech_parms_ptr = NULL;
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
  qdsp_cmd_ec_gen_id_type               ec_gen_id = QDSP_CMD_EC_GEN_MAX;
  int                                   i;
#ifdef FEATURE_AUDIO_AGC
  qdsp_cmd_agc_config_type              agc_config;
#endif
  qdsp_cmd_rve_config_type              rve_config;

  for (i=0;i<VOC_STATE_NUM_AUD_PROCESSES;i++)
  {
    state_ret[i] = VOC_STATE_SUCCESS;
  }

#ifdef FEATURE_PHONE_VR
  /* If Voice recognition is enabled, then AFE
  ** parameters may be overridden.
  */
  if (voc_state_data.vr_mode == VOC_VR_MODE_BC) {
    if ((ec_mode_ptr = voc_data_get_vr_ec()) == NULL) {
      ec_mode_ptr = voc_data_get_ec_mode();
    }

    if (*ec_mode_ptr >= VOC_EC_DEFAULT) {
      ec_mode_ptr = voc_data_get_ec_mode();
    }

    if ((ns_mode_ptr = voc_data_get_vr_ns()) == NULL) {
      ns_mode_ptr = voc_data_get_ns_mode();
    }
  } else
#endif /* FEATURE_PHONE_VR */
  {
    ec_mode_ptr = voc_data_get_ec_mode();
    ns_mode_ptr = voc_data_get_ns_mode();
    vox_mode_ptr = voc_data_get_vox_mode();
    sidetone_mode_ptr  = voc_data_get_sidetone_mode();
  }

  ec_params_ptr = voc_data_get_ec_params(*ec_mode_ptr);

#ifdef FEATURE_AVS_NEXTGEN_EC
  nextgen_ec_parms_ptr = voc_data_get_nextgen_ec_parms(*ec_mode_ptr);
#endif /* FEATURE_AVS_NEXTGEN_EC */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  crystal_speech_parms_ptr = voc_data_get_crystal_speech_parms(*ec_mode_ptr);
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

  ec_gen_id = qdsp_cmd_get_ec_gen_id();

  /* Override all EC, NS, AGC, HPF and NES if vox_override is set.
  */
  if(voc_data_get_aud_process_override() == TRUE) {
    /* Disable extra vocoder modes */


    rve_config.rx_rve_enable = QDSP_CMD_RVE_DIS;

    switch (ec_gen_id)
    {
      case QDSP_CMD_EC_GEN_LEGACY:
        state_ret[0] = voc_state_set_ec(VOC_EC_OFF, NULL);
        break;

#ifdef FEATURE_AVS_NEXTGEN_EC
      case QDSP_CMD_EC_GEN_NEXT:
        state_ret[8] = voc_state_set_rve(VOC_EC_OFF, &rve_config );
        state_ret[0] = voc_state_set_nextgen_ec(VOC_EC_OFF, NULL);
        break;
#endif  /* FEATURE_AVS_NEXTGEN_EC */

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
      case QDSP_CMD_EC_GEN_CRYSTAL_SPEECH:
        state_ret[8] = voc_state_set_dual_rve(VOC_EC_OFF, &rve_config );
        state_ret[0] = voc_state_set_crystal_speech(VOC_EC_OFF, NULL);
        break;
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

      default:
        state_ret[0] = VOC_STATE_FAILURE;
        break;
    }

    state_ret[1] = voc_state_set_ns(VOC_NS_OFF);
#ifdef FEATURE_AUDIO_AGC
    agc_config.rx_agc_enable = QDSP_CMD_AGC_DIS;
    agc_config.rx_avc_enable = QDSP_CMD_AGC_DIS;
    agc_config.tx_agc_enable = QDSP_CMD_AGC_DIS;
    state_ret[2] = voc_state_set_agc(&agc_config);
#endif
    state_ret[3] = voc_state_set_hpf_override(VOC_STATE_HPF_OVERRIDE_ON);
#ifdef FEATURE_ACP
#error code not present
#endif
#ifdef FEATURE_TTY
    state_ret[5] = voc_state_set_tty(VOC_TTY_MODE_OFF);
#endif /* FEATURE_TTY */
    state_ret[6] = voc_state_set_vox(VOC_VOX_OFF);
    state_ret[7] = voc_state_set_sidetone(VOC_SIDETONE_OFF);

  } else {

    voc_data_get_rve(&rve_config);

    /* Check if the ECNS and RVE is required at this point or not? */
    if (voc_check_in_call(voc_state_data.acquire_mask) == TRUE)
    {
      /* enable ECNS */
    switch (ec_gen_id)
    {
      case QDSP_CMD_EC_GEN_LEGACY:
        state_ret[0] = voc_state_set_ec(*ec_mode_ptr, ec_params_ptr);
        break;

#ifdef FEATURE_AVS_NEXTGEN_EC
      case QDSP_CMD_EC_GEN_NEXT:
        state_ret[0]
        = voc_state_set_nextgen_ec(*ec_mode_ptr, nextgen_ec_parms_ptr);
        state_ret[8] = voc_state_set_rve(*ec_mode_ptr, &rve_config );
        break;
#endif  /* FEATURE_AVS_NEXTGEN_EC */

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
      case QDSP_CMD_EC_GEN_CRYSTAL_SPEECH:
        state_ret[0]
        = voc_state_set_crystal_speech(*ec_mode_ptr, crystal_speech_parms_ptr);
        state_ret[8] = voc_state_set_dual_rve(*ec_mode_ptr, &rve_config );
        break;
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

      default:
        state_ret[0] = VOC_STATE_FAILURE;
        break;
    }

    state_ret[1] = voc_state_set_ns(*ns_mode_ptr);
    state_ret[7] = voc_state_set_sidetone(*sidetone_mode_ptr);
    }
    else
    {
      rve_config.rx_rve_enable = QDSP_CMD_RVE_DIS;
      /* disable ECNS and RVE */
      switch (ec_gen_id)
      {
        case QDSP_CMD_EC_GEN_LEGACY:
          state_ret[0] = voc_state_set_ec(VOC_EC_OFF, ec_params_ptr);
          break;

#ifdef FEATURE_AVS_NEXTGEN_EC
        case QDSP_CMD_EC_GEN_NEXT:
          state_ret[0]
          = voc_state_set_nextgen_ec(VOC_EC_OFF, nextgen_ec_parms_ptr);
          state_ret[8] = voc_state_set_rve(VOC_EC_OFF, &rve_config );
          break;
#endif  /* FEATURE_AVS_NEXTGEN_EC */

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
        case QDSP_CMD_EC_GEN_CRYSTAL_SPEECH:
          state_ret[0]
          = voc_state_set_crystal_speech(VOC_EC_OFF, crystal_speech_parms_ptr);
          state_ret[8] = voc_state_set_dual_rve(VOC_EC_OFF, &rve_config );
          break;
#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

        default:
          state_ret[0] = VOC_STATE_FAILURE;
          break;
      }

      state_ret[1] = voc_state_set_ns(VOC_NS_OFF);
      state_ret[7] = voc_state_set_sidetone(VOC_SIDETONE_OFF);
    }

#ifdef FEATURE_AUDIO_AGC
#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */
    {
      voc_data_smartsnd_get_agc(&agc_config);
    }
    state_ret[2] = voc_state_set_agc(&agc_config);
#endif
    state_ret[3] = voc_state_set_hpf_override(VOC_STATE_HPF_OVERRIDE_OFF);
#ifdef FEATURE_ACP
#error code not present
#endif
#ifdef FEATURE_TTY
    state_ret[5] = voc_state_set_tty(voc_state_data.tty_mode);
#endif /* FEATURE_TTY */
    state_ret[6] = voc_state_set_vox(*vox_mode_ptr);
  }

  /* Check for errors in setting audio processing
  */
  for(i=0;i<VOC_STATE_NUM_AUD_PROCESSES;i++) {
    if(state_ret[i] != VOC_STATE_SUCCESS) {
      return (VOC_STATE_FAILURE);
    }
  }

  return(VOC_STATE_SUCCESS);
} /* voc_state_init_aud_process */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_init_afe

DESCRIPTION
  Initialize the audio front end.

DEPENDENCIES
  This function should only be called when the DSP is in Idle, Vocoder
  or DFM state. voc_data_set_codec must have been called to set the current
  pcm path.

RETURN VALUE
  VOC_STATE_SUCCESS - Audio front end initialized.
  VOC_STATE_FAILURE - Couldn't complete operation

SIDE EFFECTS
  Calls voc_state_init_aud_process().

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_state_return_type voc_state_init_afe ()
{
  qdsp_cmd_status_type           qdsp_ret[4];
  qdsp_cmd_pcm_filter_type*      rx_filter_ptr;
  qdsp_cmd_pcm_filter_type*      tx_filter_ptr;
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  qdsp_cmd_pcm_filter_type*         tx_second_mic_filter_ptr = NULL;
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
  uint16*                        pre_ec_tx_pcm_filter = NULL;
#ifdef FEATURE_IIR_FILTER
  qdsp_cmd_pcm_iir_filter_type*     rx_iir_filter_ptr = NULL;
  qdsp_cmd_pcm_iir_filter_type*     tx_iir_filter_ptr = NULL;
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
  qdsp_cmd_pcm_iir_filter_type*     tx_second_mic_iir_filter_ptr = NULL;
#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
#endif /* FEATURE_IIR_FILTER */
#ifdef FEATURE_PHONE_VR
  voc_wideband_vr_type              band;
  const unsigned short*             ba_weight_ptr;
#endif /* FEATURE_PHONE_VR */

  /* Initialize audio process functions separately from loading filters
  ** to minimize time taken for the QDSP to re-load filters.
  */
#ifdef FEATURE_AVS_VP3
#error code not present
#endif /*FEATURE_AVS_VP3*/

  if(voc_state_init_aud_process() != VOC_STATE_SUCCESS)
  {
    return(VOC_STATE_FAILURE);
  }

  /*rx filter*/
#ifdef FEATURE_PHONE_VR
  /* If Voice recognition is enabled, then AFE
  ** parameters may be overridden.
  */
  if ((voc_state_data.vr_mode == VOC_VR_MODE_BC &&
      voc_state_data.current_image == QDSP_IMAGE_VRBC)
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
      )
  {
#ifdef FEATURE_IIR_FILTER
    rx_iir_filter_ptr = voc_data_get_rx_iir_filter();
    if(rx_iir_filter_ptr != NULL)
    {
      qdsp_ret[0] = qdsp_cmd_set_rx_pcm_iir_filter(rx_iir_filter_ptr);
      if ( qdsp_ret[0] != QDSP_CMD_SUCCESS )
      {
         ERR("Rx PCM IIR filter load command failed",0,0,0);
      }
    }
    else
#endif
    {
      rx_filter_ptr = voc_data_get_rx_filter();
      qdsp_ret[0] = qdsp_cmd_set_rx_pcm_filter(rx_filter_ptr);
      if ( qdsp_ret[0] != QDSP_CMD_SUCCESS )
      {
         ERR("Rx PCM filter load command failed",0,0,0);
      }
    }

    /*pre ec*/    
    pre_ec_tx_pcm_filter = voc_data_get_pre_ec_tx_pcm_filter();
    qdsp_ret[3] = qdsp_cmd_set_pre_ec_tx_pcm_filter(pre_ec_tx_pcm_filter);
    if ( qdsp_ret[3] != QDSP_CMD_SUCCESS )
    {
      ERR("Pre EC PCM filter set command failed",0,0,0);
    }

    /*tx pcm*/
    if ((tx_filter_ptr = voc_data_get_vr_filter()) == NULL)
    {
      tx_filter_ptr = voc_data_get_tx_filter();
    }

    qdsp_ret[1] = qdsp_cmd_set_tx_pcm_filter(tx_filter_ptr);
    if ( qdsp_ret[1] != QDSP_CMD_SUCCESS )
    {
      ERR("Tx PCM filter load command failed",0,0,0);
    }
    /* Set VR Bark Amp weighting values for either narrow or wideband VR.
    */
    band = vre_band_select ();
    ba_weight_ptr = voc_data_get_vr_bark_weight (band);
    if (qdsp_cmd_set_bark_weight (ba_weight_ptr) != QDSP_CMD_SUCCESS)
    {
      ERR("Bark Amp weight load command failed",0,0,0);
    }
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
#ifdef MSMAUD_REINIT_CODEC
    MSMAUD_REINIT_CODEC();
#endif
    return (VOC_STATE_SUCCESS);
  }
#endif

/*rx iir*/
#ifdef FEATURE_IIR_FILTER
    rx_iir_filter_ptr = voc_data_get_rx_iir_filter();
    if(rx_iir_filter_ptr != NULL)
    {
      qdsp_ret[0] = qdsp_cmd_set_rx_pcm_iir_filter(rx_iir_filter_ptr);
      if ( qdsp_ret[0] != QDSP_CMD_SUCCESS )
      {
         ERR("Rx PCM IIR filter load command failed",0,0,0);
      }
    }
    else
#endif /*FEATURE_IIR_FILTER*/
    {
      rx_filter_ptr = voc_data_get_rx_filter();
      qdsp_ret[0] = qdsp_cmd_set_rx_pcm_filter(rx_filter_ptr);
      if ( qdsp_ret[0] != QDSP_CMD_SUCCESS )
      {
         ERR("Rx PCM filter load command failed",0,0,0);
      }
    }

/*tx iir*/
#ifdef FEATURE_IIR_FILTER
    tx_iir_filter_ptr = voc_data_get_tx_iir_filter();
    if(tx_iir_filter_ptr != NULL)
    {
      qdsp_ret[1] = qdsp_cmd_set_tx_pcm_iir_filter(tx_iir_filter_ptr);
      if ( qdsp_ret[1] != QDSP_CMD_SUCCESS )
      {
         ERR("Rx PCM IIR filter load command failed",0,0,0);
      }
    }
    else
#endif /*FEATURE_IIR_FILTER*/
    {
      tx_filter_ptr = voc_data_get_tx_filter();
      qdsp_ret[1] = qdsp_cmd_set_tx_pcm_filter(tx_filter_ptr);
      if ( qdsp_ret[1] != QDSP_CMD_SUCCESS )
      {
         ERR("Tx PCM filter load command failed",0,0,0);
      }
    }

    /*pre ec*/    
    pre_ec_tx_pcm_filter = voc_data_get_pre_ec_tx_pcm_filter();
    qdsp_ret[3] = qdsp_cmd_set_pre_ec_tx_pcm_filter(pre_ec_tx_pcm_filter);
    if ( qdsp_ret[3] != QDSP_CMD_SUCCESS )
    {
      ERR("Pre EC PCM filter set command failed",0,0,0);
    }    

/*secondary mic*/
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
#ifdef FEATURE_IIR_FILTER
    if(qdsp_cmd_get_ec_gen_id()== QDSP_CMD_EC_GEN_CRYSTAL_SPEECH)
    {
      tx_second_mic_iir_filter_ptr = voc_data_get_tx_second_mic_iir_filter();
      if(tx_second_mic_iir_filter_ptr != NULL)
      {
        qdsp_ret[2] = qdsp_cmd_set_tx_pcm_secondmic_iir_filter(
            tx_second_mic_iir_filter_ptr);
        if ( qdsp_ret[2] != QDSP_CMD_SUCCESS )
        {
          ERR("Tx Secondary Mic PCM IIR filter load command failed",0,0,0);
        }
      }
      else
#endif /*FEATURE_IIR_FILTER*/
      {
        tx_second_mic_filter_ptr = voc_data_get_tx_second_mic_filter();
        qdsp_ret[2] = qdsp_cmd_set_tx_pcm_secondmic_filter(
            tx_second_mic_filter_ptr);
        if ( qdsp_ret[2] != QDSP_CMD_SUCCESS )
        {
          ERR("Tx Secondary Mic PCM filter load command failed",0,0,0);
        }
      }
    }
#endif /*FEATURE_AVS_CRYSTAL_SPEECH*/

#ifdef MSMAUD_REINIT_CODEC
  MSMAUD_REINIT_CODEC();
#endif
  
  return(VOC_STATE_SUCCESS);
} /* voc_state_init_afe */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_init_graph_afe

DESCRIPTION
  Initialize the audio front end.

DEPENDENCIES
  This function should only be called when the DSP is in Idle, Vocoder
  or DFM state. voc_data_set_codec must have been called to set the current
  pcm path.

RETURN VALUE
  VOC_STATE_SUCCESS - Audio front end initialized.
  VOC_STATE_FAILURE - Couldn't complete operation

SIDE EFFECTS
  Calls voc_state_init_aud_process().

===========================================================================*/
voc_state_return_type voc_state_init_graph_afe ( void )
{
  qdsp_cmd_pcm_filter_type*  filter_ptr;
  voc_state_return_type      status   = VOC_STATE_SUCCESS;
  boolean                    setup_tx = FALSE;
  boolean                    setup_rx = FALSE;

  switch (voc_state_data.current_image) {
#if defined(QDSP_IMAGE_MP4E_DEFINED)       || \
    defined(QDSP_IMAGE_MP4E_BAYER_DEFINED) || \
    defined(QDSP_IMAGE_MP4E_13K_DEFINED)   || \
    defined(QDSP_IMAGE_MP4E_EVRC_DEFINED)  || \
    defined(QDSP_IMAGE_MP4E_AMR_DEFINED)   || \
    defined(QDSP_IMAGE_QCAMCORDER_DEFINED) || \
    defined(QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED) || \
    defined(QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_AACENC_DEFINED)
#ifdef QDSP_IMAGE_MP4E_DEFINED
    case QDSP_IMAGE_MP4E:
#endif
#ifdef QDSP_IMAGE_MP4E_BAYER_DEFINED
    case QDSP_IMAGE_MP4E_BAYER:
#endif
#ifdef QDSP_IMAGE_MP4E_13K_DEFINED
    case QDSP_IMAGE_MP4E_13K:
#endif
#ifdef QDSP_IMAGE_MP4E_EVRC_DEFINED
    case QDSP_IMAGE_MP4E_EVRC:
#endif
#ifdef QDSP_IMAGE_MP4E_AMR_DEFINED
    case QDSP_IMAGE_MP4E_AMR:
#endif
#ifdef QDSP_IMAGE_AUDDUB_13K_DEFINED
    case QDSP_IMAGE_AUDDUB_13K:
#endif
#ifdef QDSP_IMAGE_AUDDUB_EVRC_DEFINED
    case QDSP_IMAGE_AUDDUB_EVRC:
#endif
#ifdef QDSP_IMAGE_AUDDUB_AMR_DEFINED
    case QDSP_IMAGE_AUDDUB_AMR:
#endif
#ifdef QDSP_IMAGE_QCAMCORDER_DEFINED
    case QDSP_IMAGE_QCAMCORDER:
#endif
#ifdef QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED
    case QDSP_IMAGE_QCAMCORDER_AACENC:
#endif
#ifdef QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED
#error code not present
#endif /*QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED */
#ifdef QDSP_IMAGE_AACENC_DEFINED
    case QDSP_IMAGE_AACENC:
#endif
      setup_tx = TRUE;
      break;
#endif

#if defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED) || \
    defined(QDSP_IMAGE_QTV_DEFINED)                || \
    defined(QDSP_IMAGE_MP4D_DEFINED)               || \
    defined(QDSP_IMAGE_MP4D_AMR_DEFINED)           || \
    defined(QDSP_IMAGE_H263D_DEFINED)              || \
    defined(QDSP_IMAGE_H263D_AMR_DEFINED)          || \
    defined(QDSP_IMAGE_H264D_AMR_DEFINED)          || \
    defined(QDSP_IMAGE_H264D_EVRC_DEFINED)         || \
    defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED)       || \
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_AUDDUB_13K_DEFINED)         || \
    defined(QDSP_IMAGE_AUDDUB_EVRC_DEFINED)        || \
    defined(QDSP_IMAGE_AUDDUB_AMR_DEFINED)
#ifdef QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED
    case QDSP_IMAGE_MP4D_AMR_EVRC_V13K:
#endif
#ifdef QDSP_IMAGE_QTV_DEFINED
    case QDSP_IMAGE_QTV:
#endif
#ifdef QDSP_IMAGE_MP4D_DEFINED
    case QDSP_IMAGE_MP4D:
#endif
#ifdef QDSP_IMAGE_MP4D_AMR_DEFINED
    case QDSP_IMAGE_MP4D_AMR:
#endif
#ifdef QDSP_IMAGE_H263D_DEFINED
    case QDSP_IMAGE_H263D:
#endif
#ifdef QDSP_IMAGE_H263D_AMR_DEFINED
    case QDSP_IMAGE_H263D_AMR:
#endif
#ifdef QDSP_IMAGE_H264D_AMR_DEFINED
    case QDSP_IMAGE_H264D_AMR:
#endif
#ifdef QDSP_IMAGE_H264D_EVRC_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QTV_VOC_H264_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED
#error code not present
#endif
      setup_rx = TRUE;
      break;
#endif

#if defined(QDSP_IMAGE_VFE_BAYER_DEFINED)   || \
    defined(QDSP_IMAGE_VFE_YCBCR_DEFINED)   || \
    defined(QDSP_IMAGE_QCAMERA_DEFINED)
#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
    case QDSP_IMAGE_VFE_BAYER:
#endif
#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
    case QDSP_IMAGE_VFE_YCBCR:
#endif
#ifdef QDSP_IMAGE_QCAMERA_DEFINED
    case QDSP_IMAGE_QCAMERA:
#endif
      /* Only need to set volumes */
      break;
#endif

    default:
      return status;
  }

  if (setup_tx) {
    if ((filter_ptr = voc_data_get_tx_filter()) != NULL) {
      if (qdsp_cmd_set_tx_pcm_filter(filter_ptr) != QDSP_CMD_SUCCESS) {
         ERR("Tx PCM filter load command failed",0,0,0);
         status = VOC_STATE_FAILURE;
      }
    }
  }

  if (setup_rx) {
    if ((filter_ptr = voc_data_get_rx_filter()) != NULL) {
      if (qdsp_cmd_set_rx_pcm_filter(filter_ptr) != QDSP_CMD_SUCCESS) {
         ERR("Rx PCM filter load command failed",0,0,0);
         status = VOC_STATE_FAILURE;
      }
    }
  }
       /* Setting the qdsp idlemetering variable */
       qdsp_cmd_set_idle_metering(voc_data_get_idle_metering());

  return status;
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_go_to_idle

DESCRIPTION
  Request the DSP to go to idle state.

DEPENDENCIES
  QDSP image must be loaded.
  This command is only valid when the DSP is in Vocoder or DFM state.

RETURN VALUE
  VOC_STATE_SUCCESS     - Vocoder on it's way to IDLE state
  VOC_STATE_FAILURE     - Couldn't complete operation

SIDE EFFECTS
  Changes QDSP state.

===========================================================================*/
voc_state_return_type voc_state_go_to_idle (void)
{
  sint15                  dsp_state;
  voc_state_return_type   ret_val;
  qdsp_cmd_status_type    result = QDSP_CMD_SUCCESS;

  dsp_state = qdsp_cmd_get_state();

  if ((dsp_state == QDSP_STATE_VOCODER)     ||
#ifdef FEATURE_PHONE_VR
      (dsp_state == QDSP_STATE_VR_PROMPT)   ||
      (dsp_state == QDSP_STATE_VR_CAPTURE)  ||
#endif /* FEATURE_PHONE_VR */
      (dsp_state == QDSP_STATE_DFM)) {

    /* Mute the modem section before going to IDLE
    */
#ifdef FEATURE_VOC_PCM_INTERFACE
    if (!voc_state_data.pcm_int_mode_pending)
#endif
    {
      result = qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_MUTE_VOL));
    }
    if (result != QDSP_CMD_SUCCESS) {
      ret_val = VOC_STATE_FAILURE;
      MSG_HIGH("QDSP Vol Cmd Failed!", 0, 0, 0);
    } else {
      result = qdsp_cmd_idle();
      if (result != QDSP_CMD_SUCCESS) {
        ret_val = VOC_STATE_FAILURE;
        MSG_HIGH("QDSP Idle Cmd Failed!", 0, 0, 0);
      } else {
        MSG_HIGH("Vocoder going to IDLE",0,0,0);
        ret_val = VOC_STATE_SUCCESS;
      }
    }
  } else {
    ret_val = VOC_STATE_FAILURE;
  }
  return(ret_val);
} /* voc_state_go_to_idle */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_voc_config

DESCRIPTION
  Download the correct vocoder service option and send a vocoder
  configuration command.

DEPENDENCIES
  QDSP must be in IDLE state.

RETURN VALUE
  VOC_STATE_SUCCESS     - QDSP is on it's way to VOCODER state
  VOC_STATE_FAILURE      - One of the vocoder commands failed

SIDE EFFECTS
  Changes QDSP state.

===========================================================================*/
voc_state_return_type voc_state_voc_config (voc_capability_type so)
{
  uint16 config_val  = QDSP_VOC_MODE_NONE_V;
  uint16 enc_val     = QDSP_VOC_ENC_DEC_DIS_V;
  uint16 dec_val     = QDSP_VOC_ENC_DEC_DIS_V;

#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
  voc_capability_type    desired_config;
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */

  switch (so)
  {
    case VOC_CAP_IS733 :
      config_val = QDSP_VOC_MODE_IS733_V;
      /* Enable decoder post filter */
      config_val |= QDSP_DEC_POST_FILT_ENA_V;
      enc_val    = QDSP_VOC_ENC_ENA_V;
      dec_val    = QDSP_VOC_DEC_ENA_V;
      break;

    case VOC_CAP_IS127 :
      config_val = QDSP_VOC_MODE_IS127_V;
      /* Enable decoder post filter */
      config_val |= QDSP_DEC_POST_FILT_ENA_V;
      enc_val    = QDSP_VOC_ENC_ENA_V;
      dec_val    = QDSP_VOC_DEC_ENA_V;
      break;

#ifdef MSMAUD_VOC_IS127_VOIP
    case VOC_CAP_IS127_VOIP:
      config_val = QDSP_VOC_MODE_IS127_VOIP_V;
      /* Enable decoder post filter */
      config_val |= QDSP_DEC_POST_FILT_ENA_V;
      enc_val    = QDSP_VOC_ENC_ENA_V;
      dec_val    = QDSP_VOC_DEC_ENA_V;
      break;
#endif /* MSMAUD_VOC_IS127_VOIP */

#ifdef MSMAUD_VOC_4GV
    case VOC_CAP_4GV_NB :
      config_val = QDSP_VOC_MODE_4GV_V;
      /* Enable decoder post filter */
      config_val |= QDSP_DEC_POST_FILT_ENA_V;
      enc_val    = QDSP_VOC_ENC_ENA_V;
      dec_val    = QDSP_VOC_DEC_ENA_V;
      break;
#endif  /* MSMAUD_VOC_4GV */

#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif  /* MSMAUD_VOC_4GV_VOIP */

#ifdef MSMAUD_VOC_4GV_WB
    case VOC_CAP_4GV_WB :
      config_val = QDSP_VOC_MODE_4GV_WB_V;
      /* Enable decoder post filter */
      config_val |= QDSP_DEC_POST_FILT_ENA_V;
      enc_val    = QDSP_VOC_ENC_ENA_V;
      dec_val    = QDSP_VOC_DEC_ENA_V;
      break;
#endif  /* MSMAUD_VOC_4GV_WB */

#ifdef MSMAUD_VOC_4GV_WB_VOIP
    case VOC_CAP_4GV_WB_VOIP :
      config_val = QDSP_VOC_MODE_4GV_WB_VOIP_V;
      /* Enable decoder post filter */
      config_val |= QDSP_DEC_POST_FILT_ENA_V;
      enc_val    = QDSP_VOC_ENC_ENA_V;
      dec_val    = QDSP_VOC_DEC_ENA_V;
      break;
#endif  /* MSMAUD_VOC_4GV_WB_VOIP */

#if defined(MSMAUD_VOC_AMR_COMMON)
    case VOC_CAP_AMR :
      config_val = QDSP_VOC_MODE_AMR_V;
      /* Enable decoder post filter */
      config_val |= QDSP_DEC_POST_FILT_ENA_V;
      enc_val    = QDSP_VOC_ENC_ENA_V;
      dec_val    = QDSP_VOC_DEC_ENA_V;
      break;
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

#if defined(MSMAUD_VOC_AMR_WB_COMMON)
    case VOC_CAP_AMR_WB :
      config_val = QDSP_VOC_MODE_AMR_WB_V;
      /* Enable decoder post filter */
      config_val |= QDSP_DEC_POST_FILT_ENA_V;
      enc_val    = QDSP_VOC_ENC_ENA_V;
      dec_val    = QDSP_VOC_DEC_ENA_V;
      break;
#endif /* defined(MSMAUD_VOC_AMR_WB_COMMON) */

#if defined(MSMAUD_VOC_EFR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) */

#if defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_FR) */

#ifdef MSMAUD_VOC_HR
    case VOC_CAP_GSM_HR :
      config_val = QDSP_VOC_MODE_GSM_HR_V;
      /* Enable decoder post filter */
      config_val |= QDSP_DEC_POST_FILT_ENA_V;
      enc_val    = QDSP_VOC_ENC_ENA_V;
      dec_val    = QDSP_VOC_DEC_ENA_V;
      break;
#endif  /* MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_G711
    case VOC_CAP_G711:
      config_val = QDSP_VOC_MODE_G711_V;
      enc_val    = QDSP_VOC_ENC_ENA_V;
      dec_val    = QDSP_VOC_DEC_ENA_V;
      break;
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
    case VOC_CAP_G723:
      config_val = QDSP_VOC_MODE_G723_V;
      break;
#endif /* MSMAUD_VOC_G723 */

#if defined(FEATURE_QTUNES) || \
    defined(FEATURE_QTUNES_AAC)
    case VOC_CAP_QTUNES:
#ifdef FEATURE_QTUNES_AAC // Gemsea Add
    case VOC_CAP_QTUNES_AAC:
#endif
#ifdef FEATURE_WMA
    case VOC_CAP_QTUNES_WMA:
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif
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
          config_val = QDSP_VOC_MODE_IS733_V;
          /* Enable decoder post filter */
          config_val |= QDSP_DEC_POST_FILT_ENA_V;
          enc_val    = QDSP_VOC_ENC_ENA_V;
          dec_val    = QDSP_VOC_DEC_ENA_V;
          break;

        case VOC_CAP_IS127:
          config_val = QDSP_VOC_MODE_IS127_V;
          /* Enable decoder post filter */
          config_val |= QDSP_DEC_POST_FILT_ENA_V;
          enc_val    = QDSP_VOC_ENC_ENA_V;
          dec_val    = QDSP_VOC_DEC_ENA_V;
          break;

#ifdef FEATURE_MVS
        case VOC_CAP_AMR:
          config_val = QDSP_VOC_MODE_AMR_V;
          /* Enable decoder post filter */
          config_val |= QDSP_DEC_POST_FILT_ENA_V;
          enc_val    = QDSP_VOC_ENC_ENA_V;
          dec_val    = QDSP_VOC_DEC_ENA_V;
          break;

#ifdef MSMAUD_VOC_EFR
#error code not present
#endif

#ifdef MSMAUD_VOC_FR
#error code not present
#endif

#ifdef MSMAUD_VOC_HR
        case VOC_CAP_GSM_HR:
          config_val = QDSP_VOC_MODE_GSM_HR_V;
          /* Enable decoder post filter */
          config_val |= QDSP_DEC_POST_FILT_ENA_V;
          enc_val    = QDSP_VOC_ENC_ENA_V;
          dec_val    = QDSP_VOC_DEC_ENA_V;
          break;
#endif
#endif /* FEATURE_MVS */

        default:
          break;
      } /* switch (desired_config) */
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */
      break;
#endif /* FEATURE_QTUNES || FEATURE_QTUNES_AAC */

    default :
      ERR("Unsupported Service Option", 0, 0, 0);
      return VOC_STATE_FAILURE;
  }

  if ((so==VOC_CAP_IS733) || (so==VOC_CAP_IS127)
#ifdef MSMAUD_VOC_4GV
      || (so==VOC_CAP_4GV_NB)
#endif
#ifdef MSMAUD_VOC_4GV_WB
      || (so==VOC_CAP_4GV_WB)
#endif
     )
  {
    if (voc_tx_rate_limit( voc_data_get_1x_tx_rate_factor() )
        != VOC_DONE_S)
    {
      return VOC_STATE_FAILURE;
    }
  }
  /* Thers is no need of enabling the encoder during Qtv file playback */
  /* so diabling the encoder flag by setting it to Zero */

  switch(voc_state_data.current_image)
  {
#if defined(QDSP_IMAGE_QTV_DEFINED)   || \
    defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED)   || \
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_QTV_CONC_DEFINED)
#ifdef QDSP_IMAGE_QTV_DEFINED
    case QDSP_IMAGE_QTV:
#endif /* QDSP_IMAGE_QTV_DEFINED */
#ifdef QDSP_IMAGE_QTV_VOC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_VOC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED
#error code not present
#endif /*QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED*/
#ifdef QDSP_IMAGE_QTV_CONC_DEFINED
    case QDSP_IMAGE_QTV_CONC:
#endif /* QDSP_IMAGE_QTV_CONC_DEFINED */
      enc_val = QDSP_VOC_ENC_DEC_DIS_V;
      break;
#endif /* QDSP_IMAGE_QTV_DEFINED || QDSP_IMAGE_QTV_CONC_DEFINED*/
    default:
      break;
  }

  if (qdsp_cmd_voc_config (config_val, enc_val, dec_val) != QDSP_CMD_SUCCESS) {
    return VOC_STATE_FAILURE;
  }

  if ((so==VOC_CAP_IS733) || (so==VOC_CAP_IS127)
#ifdef MSMAUD_VOC_4GV
      || (so==VOC_CAP_4GV_NB)
#endif
#ifdef MSMAUD_VOC_4GV_WB
      || (so==VOC_CAP_4GV_WB)
#endif
     )
  {
    if (voc_tx_rate_limit( voc_data_get_1x_tx_rate_factor() )
        != VOC_DONE_S)
    {
      return VOC_STATE_FAILURE;
    }
  }


#ifdef MSMAUD_VOC_AMR_WB_COMMON
  if (so == VOC_CAP_AMR_WB)
  {
    if (voc_init_amr_wb_tx_rate( voc_data_get_amr_wb_init_rate() )
        != VOC_DONE_S)
    {
      return VOC_STATE_FAILURE;
    }


    if (voc_set_gsm_enc_dtx( voc_data_get_gsm_dtx_mode() )
        != VOC_DONE_S)
    {
      return VOC_STATE_FAILURE;
    }
  }
#endif  /* MSMAUD_VOC_AMR_WB_COMMON */


#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) || \
    defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR)

  switch (so)
  {
#if defined(MSMAUD_VOC_AMR_COMMON)
    case VOC_CAP_AMR :
      if (voc_init_amr_tx_rate( voc_data_get_amr_init_rate() )
          != VOC_DONE_S)
      {
       return VOC_STATE_FAILURE;
      }

      /* drop through */
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

#if defined(MSMAUD_VOC_EFR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) */
#if defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_FR) */
#if defined(MSMAUD_VOC_HR)
    case VOC_CAP_GSM_HR :
#endif /* defined(MSMAUD_VOC_HR) */
      if (voc_set_gsm_enc_dtx( voc_data_get_gsm_dtx_mode() )
          != VOC_DONE_S)
      {
       return VOC_STATE_FAILURE;
      }
      break;
  }
#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR || MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_G711
  if(so == VOC_CAP_G711) {
    if(voc_set_g711_mode( voc_data_get_g711_mode() ) != VOC_DONE_S) {
      return(VOC_STATE_FAILURE);
    }
  }
#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
  if(so == VOC_CAP_G723) {
    if(voc_set_g723_mode( voc_data_get_g723_mode() ) != VOC_DONE_S) {
      return(VOC_STATE_FAILURE);
    }
  }
#endif /* MSMAUD_VOC_G723 */

  if (voc_state_data.test_mode == VOC_TEST_INT_PKT_LOOP)
  {
    qdsp_cmd_loopback_test( QDSP_LB_AUTO_PKT_ENA_V );
  }

  if (voc_state_data.test_mode == VOC_TEST_D2E_LOOP)
  {
    qdsp_cmd_loopback_test( QDSP_LB_PACKET_ENA_V );
  }

#ifdef FEATURE_UP_LOOPBACK_TEST
  if (voc_state_data.test_mode == VOC_TEST_UP_PKT_LOOP)
  {
    qdsp_cmd_up_packet_loopback(TRUE);
  }
#endif


  return VOC_STATE_SUCCESS;

} /* voc_state_voc_config */

/* <EJECT> */
#ifdef FEATURE_QSYNTH_COMMON
/*===========================================================================

FUNCTION voc_state_set_qsynth_adec_mode

DESCRIPTION
  Setup the QSynth audio decoder type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_state_set_qsynth_adec_mode (voc_qsynth_adec_mode_type mode)
{
#ifndef FEATURE_GASYNTH_INTERFACE
  uint16 adec_mode = QDSP_CMD_QSYNTH_ADEC_QCP; /* Default adec mode */

#ifdef FEATURE_QSYNTH_ADPCM
  if (mode == VOC_QSYNTH_ADEC_MODE_ADPCM) {
    adec_mode = QDSP_CMD_QSYNTH_ADEC_ADPCM;
  } else if (mode == VOC_QSYNTH_ADEC_MODE_YADPCM) {
    adec_mode = QDSP_CMD_QSYNTH_ADEC_YADPCM;
#ifdef FEATURE_QSYNTH_PCM
  } else if (mode == VOC_QSYNTH_ADEC_MODE_PCM) {
    adec_mode = QDSP_CMD_QSYNTH_ADEC_PCM;
#endif /* FEATURE_QSYNTH_PCM */
  }
#endif /* FEATURE_QSYNTH_ADPCM */

  qdsp_cmd_qsynth_adec_mode(adec_mode);
#endif /* FEATURE_GASYNTH_INTERFACE */
}
#endif /* FEATURE_QSYNTH_COMMON */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_update_bt_state

DESCRIPTION
  Disables the bluetooth AG when a non bluetooth AG support's DSP image
  is loaded.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_state_update_bt_state(qdsp_image_type image)
{
#ifdef FEATURE_BT_AG
  if (voc_get_bt_mode() == VOC_BT_MODE_AG) {
    if (voc_data_image_supports_bt_ag(image) == FALSE) {
      (void) voc_check_bt_ag(VOC_BT_OP_AG_FORCE_DISABLE);
    }
  }
#endif

#if defined (FEATURE_BT_AG) && (defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC))
  else
#endif /* defined (FEATURE_BT_AG) && defined (FEATURE_SBC_CODEC) */

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  if (voc_get_bt_mode() == VOC_BT_MODE_A2DP) {
    if (voc_data_image_supports_bt_a2dp(image) == FALSE) {
      (void) voc_check_bt_a2dp(VOC_BT_OP_A2DP_FORCE_DISABLE);
    }
  }
#endif
}
#endif /* FEATURE_BT_AG || FEATURE_SBC_CODEC */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_inform

DESCRIPTION
  Inform Vocoder users of changes in the vocoder state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_inform (voc_inform_type inform_val)
{
  int key;

  /* Any user's to inform?
  */
  if (voc_state_data.acquire_mask == 0) {
    return;
  }

  MSG_HIGH("VOC INFORM %x", inform_val, 0, 0);
  
  /*Inform clients if the module required by them cannot be
    fetched on current image*/
  if(inform_val == VOC_INFORM_MODULE_UNFETCHED)
  {
    voc_state_control_type  *state;
    int key_index = 0;

    state = voc_state_get_state_data();
    for(; key_index < VOC_MAX_ACQUIRE_TYPE ; ++key_index)
    {
      if(key_index != VOC_ACQ_SND)
        continue;
      if( ( state->config_mask & ( 1 << key_index ) ) &&
          ( voc_client_config[key_index].config == state->config_pending )
          )
      {
        if (voc_state_data.cb_table[ key_index ] != (voc_state_cb_type) NULL) {
          (voc_state_data.cb_table[ key_index ])( inform_val );
        }      
      }
    }
    return;
  }

  /* update function status */
  for (key = 0; key < VOC_MAX_ACQUIRE_TYPE; key++)
  {
    if (voc_state_data.cb_table[ key ] != (voc_state_cb_type) NULL) {
      (voc_state_data.cb_table[ key ])( inform_val );
    }
  }

} /* voc_inform */

/* <EJECT> */
/*===========================================================================
FUNCTION voc_state_enable_eclock

DESCRIPTION
  Increments a count of the Clients of Vocoder (such as Bluetooth)
  who have requested eCodec clock
  Conditionally enables external codec clock regimes, if not already enabled.
  This is currently only reached via calls to snd_aux_pcm_clk_enable()
  We can assume that AuxPCM is needed immediately
  Updates the voc_state_eclock_count (used only for requests from Clients)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_state_enable_eclock(void)
{
  /* Turn on external Ecodec AuxPCM clocks */
  /* If called externally, then we assume that AuxPCM is needed immediately */

  rex_enter_crit_sect( &voc_state_clk_crit_sect );

  /* This counter is incremented whether ClkRegime function calls are made or not */
  voc_state_eclock_count++;
  MSG_HIGH("voc_state_enable_eclock() inc count to %d",
		   voc_state_eclock_count,0,0);
  if (voc_state_eclock_count == (uint16)(-1)) {
    MSG_ERROR("voc_state_enable_eclock() counter exceeds limit", 0, 0, 0);
	voc_state_eclock_count = 1;
  }
if (
#ifdef SND_TEST_AUXPCM_INTERFACE_MODES
#error code not present
#endif
  (voc_state_eclocks_on == FALSE)) {
    #ifdef SND_TEST_AUXPCM_INTERFACE_MODES
#error code not present
    #endif
	/* Only enable the Eclock is if isn't already enabled */
	/* Avoid potental clicks/pops calling this when already enabled */
#ifdef FEATURE_DUAL_CLK_RGM
    clk_regime_msm_enable(CLK_RGM_ECODEC_M);
#else
    clk_regime_enable(CLK_RGM_ECODEC_M);
#endif
    MSG_HIGH("voc_state_enable_eclock ON",0,0,0);

	voc_state_eclocks_on = TRUE;
  }

  rex_leave_crit_sect( &voc_state_clk_crit_sect );
} /* voc_state_enable_eclock */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_disable_eclock

DESCRIPTION
  Decrements a count of the Clients of Vocoder (such as Bluetooth)
  who have requested eCodec clock
  Conditionally disables all vocoder and external codec clock regimes -
  theoretically if not already disabled, and only if the vocoder is acquired.
  Updates the voc_state_eclock_count (used only for requests from Clients)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Interrupts are locked.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_state_disable_eclock(void)
{
  /* disabling aux pcm clock if enabled by arm */

  rex_enter_crit_sect( &voc_state_clk_crit_sect );

  if (voc_state_eclock_count > 0) {
     voc_state_eclock_count--;
     MSG_HIGH("voc_state_disable_eclock() dec count to %d",
                voc_state_eclock_count,0, 0);
  }
  if ((voc_state_eclock_count == 0) && voc_state_eclocks_on) {
    /* the clocks are not being used by snd or voc tasks, so disable them */
#ifdef FEATURE_DUAL_CLK_RGM
    clk_regime_msm_disable(CLK_RGM_ECODEC_M);
#else
    clk_regime_disable(CLK_RGM_ECODEC_M);
#endif
    MSG_HIGH("voc_state_disable_eclock ON",0,0,0);

    voc_state_eclocks_on = FALSE;
  }

  rex_leave_crit_sect( &voc_state_clk_crit_sect );
} /* voc_state_disable_eclock */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_enable_clocks

DESCRIPTION
  Enables all vocoder and codec clock regimes, if not already enabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_state_enable_clocks()
{
  /* Only enable the ECODEC Clock used for AuxPCM, if current codec uses AuxPCM */

  rex_enter_crit_sect( &voc_state_clk_crit_sect );

#ifdef FEATURE_DUAL_CLK_RGM
  clk_regime_msm_enable(CLK_RGM_ICODEC_M);
#else
  clk_regime_enable(CLK_RGM_ICODEC_M);
#endif

  /* check if any codec requires aux clock and that eclock not already on */
  if (
      (voc_state_aux_pcm_clk_query()) &&
      (voc_state_eclocks_on == FALSE)) {
       voc_state_enable_eclock();
  } /* voc_state_aux_clk_query_check */

  rex_leave_crit_sect( &voc_state_clk_crit_sect );
} /* voc_state_enable_clocks */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_disable_clocks

DESCRIPTION
  Disables all vocoder and codec clock regimes, if not already
  disabled.  However, clocks will not be disabled if the vocoder is acquired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Interrupts are locked.

===========================================================================*/
void voc_state_disable_clocks()
{
  rex_enter_crit_sect( &voc_state_clk_crit_sect );

/* disabling aux pcm clock if enabled by arm */
#ifdef MSMAUD_INIT_AUX_CODEC
  MSMAUD_INIT_CODEC();
#endif

  /* disable internal clocks */
#ifdef FEATURE_DUAL_CLK_RGM
  clk_regime_msm_disable(CLK_RGM_ICODEC_M);
#else
  clk_regime_disable(CLK_RGM_ICODEC_M);
#endif /* FEATURE_DUAL_CLK_RGM */

  /* disabling aux pcm clock if enabled by arm and Not being used by AVS Client */
  if (voc_state_eclocks_on && (voc_state_eclock_count == 0)) {
	/* the clocks are not being used by snd or voc tasks, so disable them */
	#ifdef FEATURE_DUAL_CLK_RGM
	clk_regime_msm_disable(CLK_RGM_ECODEC_M);
	#else
	clk_regime_disable(CLK_RGM_ECODEC_M);
	#endif
	MSG_HIGH("voc_state_disable_clocks: eCodec Clk disabled",0,0,0);
	voc_state_eclocks_on = FALSE;
  }
  rex_leave_crit_sect( &voc_state_clk_crit_sect );

} /* voc_state_disable_clocks */

/* <EJECT> */
#ifdef MSMAUD_CODEC_CLOCK_CTRL
/*===========================================================================

FUNCTION voc_state_disable_codec_clk

DESCRIPTION
  Disables codec PLL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_state_disable_codec_clk()
{
  clk_regime_icodec_clk_disable();
}
#endif /* MSMAUD_CODEC_CLOCK_CTRL */

/* <EJECT> */
#ifdef MSMAUD_ADIE_CODEC
/*===========================================================================

FUNCTION voc_state_get_icodec_clk_freq

DESCRIPTION
  This function returns the internal codec clock frequency given the
  sampling rate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
clkrgm_codec_clk_type voc_state_get_icodec_clk_freq (
  voc_sample_rate_type rate
)
{
  clkrgm_codec_clk_type clk_freq;

  switch(rate)
  {
    case VOC_SAMPLE_RATE_NONE:
      MSG_HIGH("Turn off the internal codec clock (Tx)",0,0,0);
      /* CLKRGM_CODEC_CLK_NONE may be more appropriate here. But we do
         not have one available. So, CLKRGM_CODEC_CLK_MAX is used.
      */
      clk_freq = CLKRGM_CODEC_CLK_MAX;
      break;

    case VOC_SAMPLE_RATE_8000:
      clk_freq = CLKRGM_CODEC_CLK_2P048_MHZ;
      break;

    case VOC_SAMPLE_RATE_11025:
      clk_freq = CLKRGM_CODEC_CLK_2P8224_MHZ;
      break;

    case VOC_SAMPLE_RATE_12000:
      clk_freq = CLKRGM_CODEC_CLK_3P072_MHZ;
      break;

    case VOC_SAMPLE_RATE_16000:
      clk_freq = CLKRGM_CODEC_CLK_4P096_MHZ;
      break;

    case VOC_SAMPLE_RATE_22050:
      clk_freq = CLKRGM_CODEC_CLK_5P6448_MHZ;
      break;

    case VOC_SAMPLE_RATE_24000:
      clk_freq = CLKRGM_CODEC_CLK_6P144_MHZ;
      break;

    case VOC_SAMPLE_RATE_32000:
      clk_freq = CLKRGM_CODEC_CLK_8P192_MHZ;
      break;

#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
    case VOC_SAMPLE_RATE_44000:
      clk_freq = CLKRGM_CODEC_CLK_11P264_MHZ;
      break;
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */

    case VOC_SAMPLE_RATE_44100:
      clk_freq = CLKRGM_CODEC_CLK_11P2896_MHZ;
      break;

    case VOC_SAMPLE_RATE_48000:
      clk_freq = CLKRGM_CODEC_CLK_12P288_MHZ;
      break;

    default:
      MSG_ERROR("Unsupported internal codec clock freq",0,0,0);
      clk_freq = CLKRGM_CODEC_CLK_2P048_MHZ;
      break;
  }
  return (clk_freq);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_set_icodec_clk

DESCRIPTION
  This function set the internal codec clock rate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_state_set_icodec_clk (
  voc_sample_rate_type rate
)
{
#ifdef MSMAUD_SCMM
#error code not present
#else

  clkrgm_codec_clk_type clk_freq;

  clk_freq = voc_state_get_icodec_clk_freq(rate);

  if (clk_freq != CLKRGM_CODEC_CLK_MAX)
  {
    #ifdef FEATURE_DUAL_CLK_RGM
    if (!clk_regime_msm_is_on(CLK_RGM_ICODEC_M))
    {
      clk_regime_msm_enable(CLK_RGM_ICODEC_M);
    }
    #else
    if (!clk_regime_is_on(CLK_RGM_ICODEC_M))
    {
      clk_regime_enable(CLK_RGM_ICODEC_M);
    }
    #endif /* FEATURE_DUAL_CLK_RGM */
  clk_regime_sel_icodec_clk(clk_freq);
  }
  #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
  #endif /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */

  /* Reset Rx codec path */
#ifdef FEATURE_AUDIO_EXTRA_CODEC_CONFIG
#error code not present
#endif /* FEATURE_AUDIO_EXTRA_CODEC_CONFIG */
#endif /* MSMAUD_SCMM */
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
#ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
#endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */

#ifndef MSMAUD_SCMM
/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_get_adie_warmup_delay                            INTERNAL

DESCRIPTION
  Get adie warmup time in ms before powering on PA.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
timetick_type voc_state_get_adie_warmup_delay(void)
{
#ifndef FEATURE_LCU_POP
  timetick_type warmup_ms = 10L;     /* 10ms by default */
#else
#error code not present
#endif /* FEATURE_LCU_POP */

#ifdef FEATURE_ULC_WARMUP_DELAY
  warmup_ms = voc_get_headphone_delay();
#endif /* FEATURE_ULC_WARMUP_DELAY */

#ifndef FEATURE_LCU_POP

#ifdef MSMAUD_PCM_DC_OFFSET
  if (qdsp_cmd_codec_offset_inuse() == TRUE)
  {
    switch (voc_state_data.current_image)
    {
#ifdef QDSP_IMAGE_SYNTH_DEFINED
      case QDSP_IMAGE_SYNTH:
        warmup_ms = 80L;
        break;
#endif

#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif


#ifdef QDSP_IMAGE_TUNES_DEFINED
      case QDSP_IMAGE_TUNES:
        warmup_ms = 80L;
        break;
#endif

#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
      case QDSP_IMAGE_CAM_AUDIO_CONC:
	 warmup_ms = 80L;
	 break;
#endif

#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
      case QDSP_IMAGE_QTV_AAC:
        warmup_ms = 80L;
        break;
#endif

#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif

#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif

#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
      case QDSP_IMAGE_ENH_AAC_PLUS:
        warmup_ms = 80L;
        break;
#endif

#ifdef QDSP_IMAGE_WM_DEFINED
      case QDSP_IMAGE_WM:
#endif
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_WM_PRO_DEFINED */
        warmup_ms = 80L;
        break;


      default:
        warmup_ms = 10L;
        break;
    }
  }
#endif  /* MSMAUD_PCM_DC_OFFSET */

#endif /* FEATURE_LCU_POP */

  return warmup_ms;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#endif /* MSMAUD_SCMM */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_adie_stall

DESCRIPTION
  Stop any pending adie configuration.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_state_adie_stall(void) {
#ifndef MSMAUD_SCMM
  #ifdef FEATURE_CMI
#error code not present
  #else
  clk_dereg( &voc_state_adie_clk_cb );
  #endif

  voc_state_adie_set_final_reg = FALSE;
#endif /* MSMAUD_SCMM */
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_adie_sleep

DESCRIPTION
  Disable the internal codec.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#ifndef MSMAUD_SCMM
void voc_state_adie_sleep() {
  voc_adie_codec_config_type *adie_codec;

  if(voc_check_aux_line_in() != TRUE )
  {
    
    #ifdef FEATURE_CMI
#error code not present
	#else
    clk_dereg( &voc_state_adie_clk_cb );
	#endif

    voc_state_adie_set_final_reg = FALSE;

    /* disabling codec output before power down */
    adie_codec_enable(ADIE_DISABLE);

    /* set the adie sleep status*/
    adie_services_set_adie_sleep_status(TRUE);

    /* Power down ADIE codec */
    adie_codec = voc_data_get_adie_codec_pwr_down_config();
    adie_codec_config(adie_codec->size, (adie_codec_reg_type *) adie_codec->val);

    /* Reset last configuration indicator */
    voc_state_last_adie_config = NULL;

    /* Allow the Analog Die to go to sleep */
    adie_wakeup (ADIE_DISABLE, ADIE_CODEC_BLK);

  #ifdef MSMAUD_PMIC_CONFIG
    #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
    pm_vreg_control(PM_OFF_CMD,PM_VREG_NCP_M);
    #endif /*GRND_REFERENCED_CAPLESS_MODE*/
  #endif
  }
#ifdef MSMAUD_PCM_DC_OFFSET
  qdsp_cmd_codec_gain_ctrl(NULL, QDSP_CMD_CODEC_GAIN_UNLOCK_ALL_NOP);
#endif  /* MSMAUD_PCM_DC_OFFSET */

}
#else
#error code not present
#endif /* MSMAUD_SCMM */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_adie_set_final

DESCRIPTION
  Set final setting in ADIE codec to enable ear amp selection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#ifndef MSMAUD_SCMM
void voc_state_adie_set_final (
  int4 ms_interval
  /*lint -esym(715,ms_interval) Tell Lint we know ms_interval parameter
  ** is not used */
) {
  voc_adie_codec_config_type *adie_codec;
  voc_codec_type codec;


  if ((voc_state_data.state == VOC_STATE_RESET)
      || (voc_state_data.state == VOC_STATE_IMAGE)
      || (voc_state_data.state == VOC_STATE_SLEEP))
  {
    /* abort wake up sequence if DSP is not ready. */
    return;
  }


  /* Enabling codec output before enabling PA.
     DSP should be producing valid output at this stage.
  */
  adie_codec_enable(ADIE_ENABLE);

  adie_codec = voc_state_last_adie_config;

  if(adie_codec != NULL  &&  adie_codec->num_to_delay) {
    adie_codec_config(adie_codec->num_to_delay,
            (adie_codec_reg_type *)&( adie_codec->val[adie_codec->size -
                                      adie_codec->num_to_delay] ) );
  }

  if (voc_check_aux_line_in() == TRUE) {
    /* get the current codec */
    voc_data_get_codec(&codec);

    // Configure the adie for AuxPGA line in
    if( codec == VOC_CODEC_ON_CHIP_0) {
      adie_services_configure_aux_path(ADIE_AUX_LINE_IN_HANDSET);
    }
    else if (codec == VOC_CODEC_STEREO_HEADSET) {
      adie_services_configure_aux_path(ADIE_AUX_LINE_IN_HEADSET);
    }
#ifdef FEATURE_SPEAKER_PHONE
    else if (codec == VOC_CODEC_SPEAKER) {
      // Configure the adie for AuxPGA line in
      adie_services_configure_aux_path(ADIE_AUX_LINE_IN_SPEAKER);
    }
#endif /* FEATURE_SPEAKER_PHONE */
    else if (codec == VOC_CODEC_ON_CHIP_AUX) {
      adie_services_configure_aux_path(ADIE_AUX_LINE_IN_LINEOUT);
    }
#if defined(FEATURE_USB_CARKIT) || defined(FEATURE_HS_USB_ANALOG_AUDIO)
    #ifdef FEATURE_INTERNAL_SDAC
    else if (codec == VOC_CODEC_STEREO_USB) {
      // Configure the adie for AuxPGA line in
      adie_services_configure_aux_path(ADIE_AUX_LINE_IN_USB);
    }
    #endif /* FEATURE_INTERNAL_SDAC */
    #endif /* FEATURE_USB_CARKIT || FEATURE_HS_USB_ANALOG_AUDIO */
#ifdef FEATURE_BT_AG
    /* FM over BT AG */
    else if ((VOC_CODEC_FM_OVER_BT_AG == codec)
            || (VOC_CODEC_BT_AG == codec)) {

      // Configure the adie for AuxPGA line in
      adie_services_configure_aux_path(ADIE_AUX_LINE_IN_TOADSP);

    }
#endif
    else {
      MSG_ERROR("voc_state_adie_set_final: codec not supported for"
                                        "aux line in feature...\n",0,0,0);
    }

  }

  MSG_HIGH("PAs are turned on",0,0,0);

  if(voc_state_pa_on_cb != NULL)
  {
    /* When PA is turned ON, sound is given a callback which
       wakes up SND task*/
    voc_state_pa_on_cb();
  }
  else
  {
    MSG_ERROR (" voc_state_pa_on_cb() is NULL",0,0,0);
  }

#ifdef MSMAUD_PCM_DC_OFFSET
  if (qdsp_cmd_codec_offset_inuse() == TRUE)
  {
    #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
    clk_busy_wait(5000);
    #else
#error code not present
    #endif    /* wait for 5ms for PA saturated with PCM offset */
  }

  qdsp_cmd_codec_gain_ctrl(NULL, QDSP_CMD_CODEC_GAIN_UNLOCK_NOP);

#endif  /* MSMAUD_PCM_DC_OFFSET */

  voc_state_write_codec_gain();

  voc_state_pa_on_status = TRUE;

  voc_state_adie_set_final_reg = FALSE;
/*setting PA status to true*/
  voc_set_pa_state(TRUE);
}
#else
#error code not present
#endif /* MSMAUD_SCMM */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#ifndef  MSMAUD_SCMM
/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_do_adie_wakeup

DESCRIPTION
  Wake up and configure the Analog Die before using the codec.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if ADIE needs to be active for current path.
  FALSE if ADIE does not need to be active for current path.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean voc_state_do_adie_wakeup(voc_adie_codec_config_type *adie_codec)
{
  boolean                    ret_val = TRUE;
  timetick_type              warmup_ms;

#ifdef AUDIO_LCU_PMIC_RFLDO
#error code not present
#endif   /* AUDIO_LCU_PMIC_RFLDO */

#ifdef MSMAUD_SCMM
#error code not present
#endif   /* MSMAUD_SCMM */

#ifdef MSMAUD_PMIC_CONFIG
  voc_pmic_config_type       *pmic_config;
  voc_codec_type             pcm_path;

  voc_data_get_codec(&pcm_path);

#ifdef AUDIO_LCU_PMIC_RFLDO
#error code not present
#endif    /* AUDIO_LCU_PMIC_RFLDO */

#ifdef MSMAUD_SCMM
#error code not present
#endif  /* MSMAUD_SCMM */

  /* Get the PMIC config */

  pmic_config = voc_data_get_pmic_config(voc_state_data.current_image,pcm_path);
#endif

  /* Wake up the Analog Die if the codec in use is the internal codec. */
  if ( adie_codec != NULL && adie_codec->size != 0 ) {
    if(voc_state_adie_set_final_reg == TRUE)
    {
      /** Unlock any previous locks **/
      qdsp_cmd_codec_gain_ctrl(NULL, QDSP_CMD_CODEC_GAIN_UNLOCK_NOP);
      
      #ifdef FEATURE_CMI
#error code not present
	  #else
      clk_dereg( &voc_state_adie_clk_cb );
	  #endif
      voc_state_adie_set_final_reg = FALSE;
    }

    /* reset the adie sleep status*/
    adie_services_set_adie_sleep_status(FALSE);

#ifndef FEATURE_HWTC
    adie_wakeup (ADIE_ENABLE, ADIE_CODEC_BLK);
#endif /* FEATURE_HWTC */

#ifdef MSMAUD_PMIC_CONFIG
    /* Turn on MIC-BIAS in EPIC (only for QSC60X5) */
    if ((pmic_config != NULL) && (pmic_config->mic_bias_on))
    {
      #ifdef MSMAUD_SCMM
#error code not present
      #else
      pm_hsed_enable(PM_HSED_ENABLE_PWM_TCXO);
      #endif /* MSMAUD_SCMM */
    }
#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
    if ((pmic_config != NULL) && (pmic_config->vreg_ncp_on))
    {
      #ifndef MSMAUD_SCMM
      pm_vreg_control(PM_ON_CMD, PM_VREG_NCP_M);
      pm_vreg_set_level(PM_VREG_NCP_ID, 1800);
      #else
#error code not present
      #endif /* MSMAUD_SCMM */
    }
    else
    {
      #ifndef MSMAUD_SCMM
      pm_vreg_control(PM_OFF_CMD,PM_VREG_NCP_M);
      #else
#error code not present
      #endif /* MSMAUD_SCMM */
    }
#endif /*GRND_REFERENCED_CAPLESS_MODE*/
#endif /*MSMAUD_PMIC_CONFIG*/

#ifndef FEATURE_AUDIO_CALIBRATION
    if(voc_state_last_adie_config != adie_codec)
#endif /* FEATURE_AUDIO_CALIBRATION */
    {
      /* disabling codec output before changing configuration */
      adie_codec_enable(ADIE_DISABLE);

      #ifdef MSMAUD_SCMM
#error code not present
      #endif /* MSMAUD_SCMM */

      /* Do all ADIE codec settings except for the last one */
      adie_codec_config(adie_codec->size - adie_codec->num_to_delay,
                        (adie_codec_reg_type *) adie_codec->val);

      if (voc_state_voc_tx_wb_query() == TRUE)
      {
#ifdef MSMAUD_ADIE_WIDEBAND_TX
        MSMAUD_ADIE_WIDEBAND_TX();
#endif
      }

      /* reconfig. done, enabling codec output */
      adie_codec_enable(ADIE_ENABLE);
    }

    /* Set indicator of latest config */
    voc_state_last_adie_config = adie_codec;

    warmup_ms = voc_state_get_adie_warmup_delay();

    MSG_HIGH("Codec Wakeup 1st stage done, Delay:%d",warmup_ms,0,0);
    #ifdef FEATURE_CMI
#error code not present
	#else
    clk_reg(
      &voc_state_adie_clk_cb,    /* Call-back structure to register  */
      voc_state_adie_set_final,  /* Routine called by Clock Services */
      warmup_ms,                 /* Call routine after 10ms          */
      0L,                        /* De-register after first call     */
      FALSE );    /* Do not call on a periodic basis  */
	#endif
	
    voc_state_adie_set_final_reg = TRUE;

#ifdef MSMAUD_CODEC_CLOCK_CTRL
    switch (voc_state_data.current_image)
    {
#ifdef FEATURE_QSYNTH_COMMON
#ifdef QDSP_IMAGE_Q3D_DEFINED
      case QDSP_IMAGE_Q3D:
#endif
#ifdef QDSP_IMAGE_QGL_DEFINED
      case QDSP_IMAGE_QGL:
#endif
#ifdef QDSP_IMAGE_SYNTH_DEFINED
      case QDSP_IMAGE_SYNTH:
#endif /* QDSP_IMAGE_SYNTH_DEFINED */
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif

        #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
        #else
        voc_state_set_icodec_clk(voc_data_get_sample_rate());
        #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
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
        #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
        #else
        voc_state_set_icodec_clk(voc_data_get_sample_rate());
        #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
        break;
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_AVS_I2SSBC
      case QDSP_IMAGE_I2SSBC:
        voc_state_set_icodec_clk(voc_data_get_sample_rate());
        break;
#endif /* FEATURE_AVS_I2SSBC */

#ifdef QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED
      case QDSP_IMAGE_QCAMCORDER_AACENC:
        voc_state_set_icodec_clk(voc_data_get_sample_rate());
        break;
#endif /* QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED */

#ifdef QDSP_IMAGE_AACENC_DEFINED
      case QDSP_IMAGE_AACENC:
        voc_state_set_icodec_clk(voc_data_get_sample_rate());
        break;
#endif /* QDSP_IMAGE_AACENC_DEFINED */

#if defined(QDSP_IMAGE_VFE_BAYER_DEFINED) || \
    defined(QDSP_IMAGE_VFE_YCBCR_DEFINED)
#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
      case QDSP_IMAGE_VFE_BAYER:
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED */
#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
      case QDSP_IMAGE_VFE_YCBCR:
#endif /* QDSP_IMAGE_VFE_YCBCR_DEFINED */
        voc_state_set_icodec_clk(VOC_SAMPLE_RATE_32000);
        break;
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED || QDSP_IMAGE_VFE_YCBCR_DEFINED */
#ifdef QDSP_IMAGE_4GV_WB_DEFINED
      case QDSP_IMAGE_4GV_WB:
        voc_state_set_icodec_clk(VOC_SAMPLE_RATE_16000);
        break;
#endif  /* QDSP_IMAGE_4GV_WB_DEFINED */
#ifdef QDSP_IMAGE_AMR_WB_DEFINED
      case QDSP_IMAGE_AMR_WB:
        voc_state_set_icodec_clk(VOC_SAMPLE_RATE_16000);
        break;
#endif  /* QDSP_IMAGE_AMR_WB_DEFINED */
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
       case QDSP_IMAGE_CAM_AUDIO_CONC:
         if ( qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_MP3){
#ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
#else
           voc_state_set_icodec_clk(voc_data_get_sample_rate());
#endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
        }else
        if (qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_WAVE){
#ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
#else
          voc_state_set_icodec_clk(voc_data_get_sample_rate());
#endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
        }
        else
        {
          voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
        }
       break;
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
      default:
#ifdef FEATURE_VOC_ADPCM
        if((voc_state_data.adpcm_mode_enabled) &&
           (voc_state_data.adpcm_voc_config)) {
          /* set 32Khz to fix sound lost after switching audio device */
          voc_state_set_icodec_clk(VOC_SAMPLE_RATE_32000);
        } else {
          voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
        }
#else
        voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
#endif
    }

#ifdef MSMAUD_RX_FILTER_REG_DWA_SET_RESET
     MSMAUD_RX_FILTER_REG_DWA_SET_RESET();
#endif

 /* Allow the clock to stabilize */
 clk_busy_wait( 200 );

#endif /* MSMAUD_CODEC_CLOCK_CTRL */

  } else {
    /* Indicate that ADIE is not needed for this audio path */
    ret_val = FALSE;
    voc_state_adie_sleep();
  }
  return(ret_val);
} /* end voc_state_do_adie_wakeup */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#else /* MSMAUD_SCMM */
#error code not present
#endif /* MSMAUD_SCMM */

/*===========================================================================

FUNCTION voc_state_bt_adie_wakeup

DESCRIPTION
  Wake up and configure the Analog Die before disabling BT connection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_state_bt_adie_wakeup(voc_codec_type codec)
{
#ifndef MSMAUD_SCMM
  voc_adie_codec_config_type *adie_codec;

  adie_codec = voc_data_get_adie_codec_config(voc_state_data.current_image,
                                              codec);
#endif /* MSMAUD_SCMM */
 /*Reset the codec gain mutext */
  #ifdef MSMAUD_PCM_DC_OFFSET
   qdsp_cmd_codec_gain_ctrl(NULL, QDSP_CMD_CODEC_GAIN_UNLOCK_ALL_NOP);
  #endif  /* MSMAUD_PCM_DC_OFFSET */
#ifndef MSMAUD_SCMM
  voc_state_do_adie_wakeup(adie_codec);
#endif /* MSMAUD_SCMM */
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_adie_wakeup

DESCRIPTION
  Wake up and configure the Analog Die before using the internal codec.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if ADIE needs to be active for current path.
  FALSE if ADIE does not need to be active for current path.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#ifndef MSMAUD_SCMM
boolean voc_state_adie_wakeup( )
{
#ifdef MSMAUD_PCM_DC_OFFSET
  qdsp_cmd_codec_offset_ctrl_type codec_offset;
#endif  /* MSMAUD_PCM_DC_OFFSET */
  qdsp_cmd_codec_gain_ctrl_type codec_gain;
  voc_adie_codec_config_type *adie_codec;
  voc_codec_type             pcm_path;


  voc_data_get_codec(&pcm_path);

#ifdef FEATURE_ULC_WARMUP_DELAY
  voc_set_headphone_status(pcm_path);
#endif /* FEATURE_ULC_WARMUP_DELAY */

  /* Get the ADIE codec config */
  adie_codec = voc_data_get_adie_codec_config(voc_state_data.current_image,
                                              pcm_path);

  if(voc_state_pa_on_status == TRUE)
  {
    /* set codec gain to 0, activate codec offset before turning on adie. */
    codec_gain.codec_tx_gain = 0;
    codec_gain.codec_rx_gain = 0;
    codec_gain.codec_st_gain = 0;
    qdsp_cmd_codec_gain_ctrl(&codec_gain, QDSP_CMD_CODEC_GAIN_OP_NORMAL);

    /* disable codec PAs only to avoid codec pops */
    #ifdef MSMAUD_ADIE_PA_POWER_DOWN
    MSMAUD_ADIE_PA_POWER_DOWN();
    #endif  /* MSMAUD_ADIE_PA_POWER_DOWN */
  }

#ifdef MSMAUD_PCM_DC_OFFSET
  if (adie_codec != NULL && adie_codec->size != 0)
  {
    voc_data_get_codec_offset(&codec_offset);
    qdsp_cmd_codec_offset_ctrl(&codec_offset);
  }
#endif  /* MSMAUD_PCM_DC_OFFSET */

  return (voc_state_do_adie_wakeup(adie_codec));
} /* end voc_state_adie_wakeup */

#else
#error code not present
#endif /* MSMAUD_SCMM */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* MSMAUD_ADIE_CODEC */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_shutdown

DESCRIPTION
  This shuts down clocks and codecs for sleep mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Clocks and codecs will be shut down.

===========================================================================*/
void voc_state_shutdown (void)
{

#ifdef AUDIO_LCU_PMIC_RFLDO
#error code not present
#endif   /*  AUDIO_LCU_PMIC_RFLDO */

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif /*AVS_BT_SCO_REWORK*/

  if(voc_check_aux_line_in() != TRUE )
  {
    /* Disable any registered codec functions */
    voc_disable_codec_func(TRUE);
  }

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
  /* Disable bluetooth connection */
  if (voc_get_bt_state() != VOC_BT_STATE_DISABLED) {
    (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
  }
#if defined(FEATURE_AVS_BT_SCO_REWORK)
    if ((bt_func_ptr != NULL) && voc_bt_playback_started)
    {
      MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_STOP);
      voc_bt_playback_started = FALSE;
    }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* FEATURE_BT_AG || FEATURE_SBC_CODEC */

#ifdef MSMAUD_ADIE_CODEC
  /* Allow the Analog Die to go to sleep */
  voc_state_adie_sleep();
#endif /* MSMAUD_ADIE_CODEC */

#ifdef MSMAUD_CODEC_CLOCK_CTRL
  voc_set_clk_ctrl_timer(FALSE, VOC_CLK_CTRL_INTERNAL_CODEC);
  voc_state_disable_codec_clk();
#endif /* MSMAUD_CODEC_CLOCK_CTRL */
/* CLK_RGM_MIDI_HCLK_M is disabled recently in clkregim_msm.c  */
/*  by default.Due to this there is no audio while playing  qsynth playbacks */

#ifdef FEATURE_LCU_MIDI_HCLK
#error code not present
#endif  /* FEATURE_LCU_MIDI_HCLK */

#ifdef MSMAUD_SCMM
#error code not present
#endif /*MSMAUD_SCMM*/


  /* set that slowtalk STATIC parameters are not updated,
     so next time when voice call starts, it will be updated
  */
  voc_data_set_slowtalk_static_params_status(
                  VOC_SLOWTALK_STATIC_PARAMS_NOT_UPDATED);

  if ( voc_state_clocks_on == TRUE ) {
    /* Disable all vocoder and codec clocks */
    voc_state_disable_clocks();
    voc_state_clocks_on = FALSE;
  }
  /* Release QDSP when going to sleep */
  if (voc_state_data.module_status >= VOC_STATE_MOD_STATUS_ENABLE_REQUESTED) {
    MSG_HIGH("qdsp_disable (%d)",voc_state_data.current_qdsps_app,0,0);
    voc_state_data.module_status = VOC_STATE_MOD_STATUS_DISABLED;
    qdsp_cmd_clear_dsp_isr();
    qdsp_disable(voc_state_data.current_qdsps_app);
  }

  vocvp3_write_to_efs();

#ifdef FEATURE_OEMSBL_AUDIBLE_TONE
  /* Call the OEM SBL function to let them know that the DSP is shutdown
     so that they can start loading AMSS
   */
  if (voc_state_oemsbl_func_ptr != NULL)
  {
    voc_state_oemsbl_func_ptr();
  }
#endif

#ifdef AUDIO_LCU_PMIC_RFLDO
#error code not present
#endif /* AUDIO_LCU_PMIC_RFLDO */

#ifdef MSMAUD_SCMM
#error code not present
#endif  /* MSMAUD_SCMM */


#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#if !defined(FEATURE_HWTC) && !defined(FEATURE_GSM_PLT)
  /* Signal that it is ok to sleep */
#ifdef FEATURE_NEW_SLEEP_API
  sleep_assert_okts(voc_state_sleep_handle);
#else
  SLEEP_ASSERT_OKTS( SLEEP_VOC_OKTS_SIG );
#endif /* FEATURE_NEW_SLEEP_API   */
#endif /* FEATURE_HWTC */
#endif /* T_SLEEP || FEATURE_SLEEP_TASK */
}

/* <EJECT> */
#ifdef FEATURE_VOC_PCM_INTERFACE
/*===========================================================================

FUNCTION voc_state_set_pcm_modes

DESCRIPTION
  Sends commands to DSP to enable/disable PCM interfaces.

DEPENDENCIES
  Clients must register a callback for the PCM interfaces enabled (if any).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_state_set_pcm_modes(void)
{
#ifdef FEATURE_PHONE_VR
  if (voc_state_data.vr_mode == VOC_VR_MODE_OFF)
#endif
  {
    if (voc_state_data.pcm_int_mode_pending != voc_state_data.pcm_int_mode) {
      if (qdsp_cmd_set_pcm_modes(
            voc_state_data.pcm_int_mode_pending, QDSP_CMD_PCM_CLIENT_VOC) ==
          QDSP_CMD_SUCCESS) {
        voc_state_data.pcm_int_mode = voc_state_data.pcm_int_mode_pending;
      }
      else {
        MSG_ERROR("PCM mode %d command failed",
                  voc_state_data.pcm_int_mode_pending, 0, 0);
      }
    }
  }
}
#endif /* FEATURE_VOC_PCM_INTERFACE */

/* <EJECT> */
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
/*===========================================================================

FUNCTION voc_state_set_pp_status

DESCRIPTION
 Sets the PP status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_state_set_pp_status
(
  voc_pp_status_type new_status
)
{
  voc_state_pp_status = new_status;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#endif /* FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

/* <EJECT> */
/*===========================================================================

                                S T A T E S

===========================================================================*/

/*===========================================================================

FUNCTION voc_state_reset

DESCRIPTION
  This state determines which QDSP module is required and commands QDSP Services
  to load it.  It then transitions to IMAGE state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS - next state is triggered by QDSP event.

SIDE EFFECTS
  QDSP will transition to IMAGE state

===========================================================================*/
voc_state_return_type voc_state_reset (void)
{
  qdsp_module_type           module_select;
#ifdef FEATURE_ACP
#error code not present
#endif
  voc_status_type	     module_select_status;

  if ((voc_state_data.acquire_mask) || (voc_state_data.reset_flag)) {
    /* Inform users of transition to RESET state */
    voc_inform(VOC_INFORM_RESET);

    if (voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) {
      voc_state_data.module_status = VOC_STATE_MOD_STATUS_ENABLE_REQUESTED;
      MSG_HIGH("qdsp_disable_event_rsp(%d)",
                                    voc_state_data.current_qdsps_app,0,0);
      qdsp_cmd_clear_dsp_isr();
      qdsp_disable_event_rsp(voc_state_data.current_qdsps_app);
    }

    /* Select the required DSP firmware image   */

    module_select_status = voc_state_get_high_module(&module_select);

    switch(module_select_status)
    {
      case VOC_CMD_FAILED_S:
        ERR( "QDSP Firmware is unavailable", 0, 0, 0 );
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        return(VOC_STATE_CONTINUE);
        break;
      case VOC_CMD_BUSY_S:
        {
          MSG_HIGH("Module not supported on current image = %d, "
                   "Continuing with Module = %d",
                   voc_state_data.current_image,
                   voc_state_data.module_reqd,0);
          voc_inform(VOC_INFORM_MODULE_UNFETCHED);
        }
        break;
      default:
        ;
    }

#ifdef FEATURE_PHONE_VR
    if(module_select == QDSP_MODULE_VRBC
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
       ) {
      /* Give the database the vr mode
      */
      voc_data_set_vr_mode(voc_state_data.vr_mode);
    }
    else {
      voc_data_set_vr_mode(VOC_VR_MODE_OFF);
    }
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

    /* Enable the QDSP and tell QDSP Services which image to load.
    */
    if ((voc_state_data.module_reqd != module_select) ||
        (voc_state_data.module_status < VOC_STATE_MOD_STATUS_ENABLED)) {
      voc_state_data.module_status = VOC_STATE_MOD_STATUS_ENABLE_REQUESTED;
      voc_state_module_commit(module_select);
      MSG_HIGH("qdsp_enable(%d,%d)",
                        voc_state_data.current_qdsps_app, module_select,0);
      qdsp_enable (voc_state_data.current_qdsps_app, module_select);
    }

#ifdef FEATURE_GRAPH_AUDIO
    voc_state_data.audio_mode = VOC_STATE_AUDIO_MODE_DISABLED;
#endif /* FEATURE_GRAPH_AUDIO */

    /* Transition to the IMAGE state
    */
    voc_state_data.state = VOC_STATE_IMAGE;
    MSG_HIGH("Entered IMAGE state",0,0,0);

    /* Continue in IMAGE state
    */
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  } else {

    /* Shut down clocks and codecs */
    voc_state_shutdown();

    return (VOC_STATE_SUCCESS);
  }
} /* voc_state_reset */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_image

DESCRIPTION
  Wait for QDSP Services to download the required image.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  QDSP2 will transition to SLEEP state

===========================================================================*/
voc_state_return_type voc_state_image (void)
{
  qdsp_module_type module_select;
  boolean         pcm_path = TRUE;   /* Setup PCM path to normal operation */
  voc_status_type module_select_status;

  /* If QDSP Services has requested an image reset then allow it.
  */
  if (voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) {
    voc_state_data.module_status = VOC_STATE_MOD_STATUS_ENABLE_REQUESTED;
    MSG_HIGH("qdsp_disable_event_rsp(%d)",
                               voc_state_data.current_qdsps_app,0,0);
    qdsp_disable_event_rsp(voc_state_data.current_qdsps_app);
  }

  /* If the image required by Vocoder Services has been loaded.
  */
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_ENABLED)) {
    voc_state_data.image_changed = FALSE;

    if ( voc_state_clocks_on == FALSE ) {
      /* disconnect Rx Filter in case of FM recording
         before configuring the icodec clock to avoid
         any unwated pop.
      */
      #ifdef MSMAUD_ADIE_RX_FILTER_DISCONNECT
      MSMAUD_ADIE_RX_FILTER_DISCONNECT();
      #endif /* MSMAUD_ADIE_RX_FILTER_DISCONNECT */
      /* Enable all vocoder and codec clocks */
      voc_state_enable_clocks();
      voc_state_clocks_on = TRUE;
    }
/* CLK_RGM_MIDI_HCLK_M is disabled recently in clkregim_msm.c  */
/*  by default.Due to this there is no audio while playing  qsynth playbacks */
#ifdef FEATURE_LCU_MIDI_HCLK
#error code not present
#endif /*FEATURE_LCU_MIDI_HCLK*/

#ifdef MSMAUD_SCMM 
#error code not present
#endif /*MSMAUD_SCMM*/


#ifdef FEATURE_BT_AG
    if ((voc_get_bt_mode() == VOC_BT_MODE_AG)&&
        ((voc_get_bt_state() == VOC_BT_STATE_AG_ENABLED) ||
         (voc_get_bt_state() == VOC_BT_STATE_AG_SCO_LINK_PENDING) ||
         (voc_get_bt_state() == VOC_BT_STATE_AG_CODEC_CONFIG_PENDING))) {
      pcm_path = FALSE;             /* Don't require to setup the PCM path */
    }
#endif /* FEATURE_BT_AG */

    /* Initialize the QDSP command layer */
    if (qdsp_cmd_init(pcm_path) != QDSP_CMD_SUCCESS)
    {
      MSG_ERROR("Couldn't Initialize command layer",0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return (VOC_STATE_CONTINUE);
    }

    /* Transition to the SLEEP state
    */
    voc_state_data.state = VOC_STATE_SLEEP;
    MSG_HIGH("Vocoder going to SLEEP",0,0,0);

    return (VOC_STATE_CONTINUE_IMMEDIATE);
  } else if ((!voc_state_data.acquire_mask) && (!voc_state_data.reset_flag)) {
    voc_state_data.state = VOC_STATE_RESET;
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  } else {

    /* The image may have changed; Re-assess the vocoder's requirements
    */
    module_select = 0;
    module_select_status = voc_state_get_high_module(&module_select);


    switch(module_select_status)
    {
      case VOC_CMD_FAILED_S:
        break;
      case VOC_CMD_BUSY_S:
        {
          MSG_HIGH("Module not supported on current image = %d, "
                   "Continuing with Module = %d",
                   voc_state_data.current_image,voc_state_data.module_reqd,0);
          voc_inform(VOC_INFORM_MODULE_UNFETCHED);
        }
        break;
      default:
        ;
    }


    if(module_select != voc_state_data.module_reqd)
    {
#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
     defined (FEATURE_SBC_DSP_CODEC))
      voc_state_update_bt_state(voc_state_data.current_image);
#endif /* FEATURE_BT_AG */

      voc_state_data.state = VOC_STATE_RESET;
      return (VOC_STATE_CONTINUE_IMMEDIATE);
    } else {
      /* The image did not get loaded.  Wait in this state until the requested
      ** image is ready.
      */
      return (VOC_STATE_SUCCESS);
    }
  }
} /* voc_state_image */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_sleep

DESCRIPTION
  Wait for the QDSP2 to reach INIT state.  When QDSP2 is in INIT,
  transition the driver to INIT state. If the QDSP2 does not reach
  INIT within the allowed time, then perform error recovery.

DEPENDENCIES
  None

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  voc_state_data.state

===========================================================================*/
voc_state_return_type voc_state_sleep (void)
{
  #ifdef MSMAUD_SCMM
#error code not present
  #endif /* MSMAUD_SCMM */
  qdsp_module_type    module_select;
  voc_status_type     module_select_status;
  qdsp_cmd_codec_gain_ctrl_type codec_gain;

  if(voc_check_aux_line_in() != TRUE)
  {
    if(voc_state_pa_on_status == TRUE)
    {
      /* set codec gain to 0, activate codec offset before turning on adie. */
      codec_gain.codec_tx_gain = 0;
      codec_gain.codec_rx_gain = 0;
      codec_gain.codec_st_gain = 0;
      qdsp_cmd_codec_gain_ctrl(&codec_gain, QDSP_CMD_CODEC_GAIN_OP_NORMAL);
  
      MSG_HIGH("Codec Rx Gain set to zero",0,0,0);
  
      #ifndef MSMAUD_SCMM
      /* disable codec PAs only to avoid codec pops */
      #ifdef MSMAUD_ADIE_PA_POWER_DOWN
      MSMAUD_ADIE_PA_POWER_DOWN();
      #endif  /* MSMAUD_ADIE_PA_POWER_DOWN */
  
      #else
#error code not present
      #endif /* MSMAUD_SCMM */
  
      MSG_HIGH("PAs are turned off ",0,0,0);
  
      voc_state_pa_on_status = FALSE;
  
  	#ifndef MSMAUD_SCMM
      /* Reset last configuration indicator */
      voc_state_last_adie_config = NULL;
  	#endif /* MSMAUD_SCMM */
    }
  
  }
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
  qdsp_cmd_set_decoder_in_use(QDSP_CMD_DEC_INVALID);
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
if (voc_state_data.current_image == QDSP_IMAGE_CAM_AUDIO_CONC){
  voc_state_data.state = VOC_STATE_CAM_AUDIO_INIT_ENTRY;
  MSG_HIGH("Vocoder going to CAM AUDIO INIT ENTRY",0,0,0);
  return (VOC_STATE_CONTINUE_IMMEDIATE);
}
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
#endif /*FEATURE_AUDIO_CAMERA_CONCURRENCY  */
  /*setting PA status to FALSE as we swithched off the power amplifiers*/
  voc_set_pa_state(FALSE);

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
#ifdef QDSP_IMAGE_VOC_COMBO_ICA_DEFINED
  if (voc_state_data.current_image == QDSP_IMAGE_VOC_COMBO_ICA) {
    voc_state_data.state = VOC_STATE_MIXER_INIT;
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  }
#endif /* QDSP_IMAGE_VOC_COMBO_ICA_DEFINED */
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifdef FEATURE_AVS_INCALL_MIXER_CMX
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
    if (voc_state_data.current_image == QDSP_IMAGE_VOC_COMBO_SYNTH) {
      voc_state_data.state = VOC_STATE_MIXER_INIT;
      return (VOC_STATE_CONTINUE_IMMEDIATE);
    }
#endif /* QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */
#endif /* FEATURE_AVS_INCALL_MIXER_CMX */

  module_select_status = voc_state_get_high_module(&module_select);
 switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }

  /* If QDSP failed to reach INIT state, attempt retry */
  if ( qdsp_cmd_get_state() != QDSP_STATE_INIT ) {
    if ( voc_state_data.retry_counter < 100 ) {
      MSG_HIGH( "INIT state transition retry", 0,0,0 );
      voc_state_data.retry_counter++;
      return( VOC_STATE_CONTINUE );
    } else {
      /* Transition to ERROR state if retry failed */
      MSG_ERROR( "QDSP failed to reach INIT state", 0,0,0 );
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

  /* Read diagnostic from the QDSP */
  qdsp_cmd_get_diags(voc_data_get_diags());

  voc_state_data.retry_counter = 0;

#ifdef FEATURE_VOC_PCM_INTERFACE
  if(voc_state_data.pcm_int_mode) {
    qdsp_cmd_set_pcm_modes(0, QDSP_CMD_PCM_CLIENT_VOC);
  }
  voc_state_data.pcm_int_mode = 0;
#endif /* FEATURE_VOC_PCM_INTERFACE */

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
     defined (FEATURE_SBC_DSP_CODEC))
  voc_state_update_bt_state(voc_state_data.current_image);
#endif /* FEATURE_BT_AG */

#ifdef FEATURE_GRAPH_AUDIO
  voc_state_data.audio_mode = VOC_STATE_AUDIO_MODE_DISABLED;
#endif /* FEATURE_GRAPH_AUDIO */

  switch (voc_state_data.current_image) {
#ifdef FEATURE_QSYNTH_COMMON
#if defined(QDSP_IMAGE_Q3D_DEFINED) || defined(QDSP_IMAGE_QGL_DEFINED)
#ifdef QDSP_IMAGE_Q3D_DEFINED
    case QDSP_IMAGE_Q3D:
#endif
#ifdef QDSP_IMAGE_QGL_DEFINED
    case QDSP_IMAGE_QGL:
#endif
#ifdef FEATURE_QSYNTH_ADPCM
      voc_state_data.adec_mode_pending = VOC_QSYNTH_ADEC_MODE_ADPCM;
#endif /* FEATURE_QSYNTH_ADPCM */
#endif /* QDSP_IMAGE_Q3D_DEFINED || QDSP_IMAGE_QGL_DEFINED */
#ifdef QDSP_IMAGE_SYNTH_DEFINED
    case QDSP_IMAGE_SYNTH:
#endif

#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif

      if ( module_select_status == VOC_DONE_S ) {
        voc_state_data.config = voc_state_data.config_pending;
      }
      voc_state_data.state  = VOC_STATE_QSYNTH_INIT;
      MSG_HIGH("Vocoder going to QSYNTH_INIT state",0,0,0);
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
#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
    case QDSP_IMAGE_QTV_AAC:
#endif /* QDSP_IMAGE_QTV_AAC_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AAC_DEFINED
    case QDSP_IMAGE_MP4D_AAC:
#endif /* QDSP_IMAGE_MP4D_AAC_DEFINED */
#ifdef QDSP_IMAGE_H263D_AAC_DEFINED
    case QDSP_IMAGE_H263D_AAC:
#endif /* QDSP_IMAGE_H263D_AAC_DEFINED */
#ifdef QDSP_IMAGE_H263D_MP3_DEFINED
    case QDSP_IMAGE_H263D_MP3:
#endif /* QDSP_IMAGE_H263D_MP3_DEFINED */
#ifdef QDSP_IMAGE_H264D_AAC_DEFINED
    case QDSP_IMAGE_H264D_AAC:
#endif /* QDSP_IMAGE_H264D_AAC_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED */
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
      if ( module_select_status == VOC_DONE_S ) {
        voc_state_data.config = voc_state_data.config_pending;
      }
      voc_state_data.state  = VOC_STATE_QTUNES_INIT;
      MSG_HIGH("Vocoder going to QTUNES_INIT state",0,0,0);
      break;
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_GRAPH_AUDIO
#if defined(QDSP_IMAGE_MP4E_DEFINED)               || \
    defined(QDSP_IMAGE_MP4E_BAYER_DEFINED)         || \
    defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED) || \
    defined(QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED)  || \
    defined(QDSP_IMAGE_AACENC_DEFINED)

#ifdef QDSP_IMAGE_MP4E_DEFINED
    case QDSP_IMAGE_MP4E:
#endif /* QDSP_IMAGE_MP4E_DEFINED */
#ifdef QDSP_IMAGE_MP4E_BAYER_DEFINED
    case QDSP_IMAGE_MP4E_BAYER:
#endif /* QDSP_IMAGE_MP4E_BAYER_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED
    case QDSP_IMAGE_MP4D_AMR_EVRC_V13K:
#endif /* QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED
    case QDSP_IMAGE_QCAMCORDER_AACENC:
#endif /* QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED */
#ifdef QDSP_IMAGE_AACENC_DEFINED
    case QDSP_IMAGE_AACENC:
#endif /* QDSP_IMAGE_AACENC_DEFINED */

      voc_state_data.audio_mode = VOC_STATE_AUDIO_MODE_NONE;
#endif

#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
    case QDSP_IMAGE_VFE_BAYER:
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED */
#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
    case QDSP_IMAGE_VFE_YCBCR:
#endif /* QDSP_IMAGE_VFE_YCBCR_DEFINED */
#ifdef QDSP_IMAGE_QCAMERA_DEFINED
    case QDSP_IMAGE_QCAMERA:
#endif /* QDSP_IMAGE_QCAMERA_DEFINED */
#ifdef QDSP_IMAGE_QTV_DEFINED
    case QDSP_IMAGE_QTV:
#endif /* QDSP_IMAGE_QTV_DEFINED */
#ifdef QDSP_IMAGE_QTV_VOC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_VOC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED
#error code not present
#endif /*QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED*/
#ifdef QDSP_IMAGE_MP4D_DEFINED
    case QDSP_IMAGE_MP4D:
#endif /* QDSP_IMAGE_MP4D_DEFINED */
#ifdef QDSP_IMAGE_MP4D_AMR_DEFINED
    case QDSP_IMAGE_MP4D_AMR:
#endif /* QDSP_IMAGE_MP4D_AMR_DEFINED */
#ifdef QDSP_IMAGE_H263D_DEFINED
    case QDSP_IMAGE_H263D:
#endif /* QDSP_IMAGE_H263D_DEFINED */
#ifdef QDSP_IMAGE_H263D_AMR_DEFINED
    case QDSP_IMAGE_H263D_AMR:
#endif /* QDSP_IMAGE_H263D_AMR_DEFINED */
#ifdef QDSP_IMAGE_H264D_AMR_DEFINED
    case QDSP_IMAGE_H264D_AMR:
#endif /* QDSP_IMAGE_H264D_AMR_DEFINED */
#ifdef QDSP_IMAGE_H264D_EVRC_DEFINED
#error code not present
#endif /* QDSP_IMAGE_H264D_EVRC_DEFINED */
#ifdef QDSP_IMAGE_MP4E_13K_DEFINED
    case QDSP_IMAGE_MP4E_13K:
#endif /* QDSP_IMAGE_MP4E_13K_DEFINED */
#ifdef QDSP_IMAGE_MP4E_EVRC_DEFINED
    case QDSP_IMAGE_MP4E_EVRC:
#endif /* QDSP_IMAGE_MP4E_EVRC_DEFINED */
#ifdef QDSP_IMAGE_MP4E_AMR_DEFINED
    case QDSP_IMAGE_MP4E_AMR:
#endif /* QDSP_IMAGE_MP4E_AMR_DEFINED */
#ifdef QDSP_IMAGE_AUDDUB_13K_DEFINED
    case QDSP_IMAGE_AUDDUB_13K:
#endif /* QDSP_IMAGE_AUDDUB_13K_DEFINED */
#ifdef QDSP_IMAGE_AUDDUB_EVRC_DEFINED
    case QDSP_IMAGE_AUDDUB_EVRC:
#endif /* QDSP_IMAGE_AUDDUB_EVRC_DEFINED */
#ifdef QDSP_IMAGE_AUDDUB_AMR_DEFINED
    case QDSP_IMAGE_AUDDUB_AMR:
#endif /* QDSP_IMAGE_AUDDUB_AMR_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_DEFINED
    case QDSP_IMAGE_QCAMCORDER:
#endif /* QDSP_IMAGE_QCAMCORDER_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED */
#ifdef QDSP_IMAGE_I2SSBC_DEFINED
    case QDSP_IMAGE_I2SSBC:
#endif
      voc_state_data.state = VOC_STATE_GRAPH_INIT;
      break;
#endif /* FEATURE_GRAPH_AUDIO */

    default:
      voc_state_data.state = VOC_STATE_INIT;
  }

  return (VOC_STATE_CONTINUE_IMMEDIATE);
} /* voc_state_sleep */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_init

DESCRIPTION
  If the vocoder is acquired, turn it on and send it to IDLE state.  If
  the vocoder is released, turn off the clocks.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS

===========================================================================*/
voc_state_return_type voc_state_init (void)
{
  voc_codec_type             pcm_path;
  voc_codec_class_type       codec_type;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  qdsp_cmd_status_type       result;
  qdsp_module_type           module_select;
  voc_status_type            module_select_status;
#ifdef FEATURE_ACP
#error code not present
#endif

  /* If the image needs to change, or the current image is not correct,
  ** then go to RESET
  */
  module_select = 0;
  module_select_status = voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (voc_state_data.image_changed == TRUE)) {
    MSG_HIGH("Vocoder going to RESET",0,0,0);
    voc_state_data.state  = VOC_STATE_RESET;
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  }

  /* Update config with config_pending */
  if (module_select_status == VOC_DONE_S) {
    voc_state_data.config = voc_state_data.config_pending;
  }

  if ((voc_state_data.acquire_mask) || (voc_state_data.reset_flag)) {
    /* Indicate first time up */
    voc_state_data.reset_flag = FALSE;

    /* Signal that it is no longer ok to sleep */
#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#if !defined(FEATURE_HWTC) && !defined(FEATURE_GSM_PLT)
#ifdef FEATURE_NEW_SLEEP_API
    sleep_negate_okts(voc_state_sleep_handle);
#else
    SLEEP_NEGATE_OKTS( SLEEP_VOC_OKTS_SIG );
#endif /* FEATURE_NEW_SLEEP_API  */
#endif /* FEATURE_HWTC */
#endif /* T_SLEEP || FEATURE_SLEEP_TASK */

#ifdef FEATURE_ACP
#error code not present
#endif

    if (voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLED) {
      voc_state_data.state = VOC_STATE_RESET;
      return (VOC_STATE_CONTINUE_IMMEDIATE);
    }

    if ( voc_state_clocks_on == FALSE ) {
      /* Enable all vocoder and codec clocks */
      voc_state_enable_clocks();
      voc_state_clocks_on = TRUE;
    }

   #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
   clk_busy_wait(200);
   #else
#error code not present
   #endif
    
#ifdef MSMAUD_ADIE_CODEC
    /* Wake up the Analog Die if the codec in use is the internal codec. */
    (void) voc_state_adie_wakeup ();
#endif /* MSMAUD_ADIE_CODEC */

    /* Get the codec configuration data */
    voc_data_get_codec(&pcm_path);
    voc_data_get_codec_class(&codec_type);

    /* Reset the configuration for this type of codec
    */
    voc_state_set_codec_cal(pcm_path);

#ifdef FEATURE_BT_AG
    if(((codec_type == VOC_CODEC_CLASS_BLUETOOTH)
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
     || (codec_type == VOC_CODEC_CLASS_BLUETOOTH_A2DP)
     || (codec_type == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO)
#endif /* FEATURE_SBC_CODEC */
      ) && (voc_get_bt_state() == VOC_BT_STATE_DISABLED)) {
      voc_data_get_codec_config(voc_state_data.current_image,
                                VOC_CODEC_DEFAULT, &codec_config_ptr);
    } else
#endif /* FEATURE_BT_AG */
    {
      voc_data_get_codec_config(voc_state_data.current_image, pcm_path,
                                &codec_config_ptr);
    }

    /* Disable any registered codec functions */
    voc_disable_codec_func(FALSE);

#ifdef FEATURE_PHONE_VR
    if (((voc_state_data.vr_mode == VOC_VR_MODE_BC) &&
         (voc_state_data.current_image== QDSP_IMAGE_VRBC))
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
       )
    {
      /* VR Mode B/C has its own codec config command, which is
      ** basically the same as the codec configuration command plus
      ** an additional field containing the VR software version number.
      */
      result = qdsp_cmd_vr_codec_config
                 (codec_config_ptr, QDSP_VR_SW_VERSION_NUMBER);
      if (result != QDSP_CMD_SUCCESS) {
        MSG_ERROR("VR Codec Config command failed",0,0,0);
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        return(VOC_STATE_CONTINUE);
      }
    }
    else
#endif /* FEATURE_PHONE_VR */
    {
      /* Issue codec configuration command */
     result = qdsp_cmd_codec_config(codec_config_ptr,\
      voc_data_get_default_frame_rate(voc_state_data.current_image, pcm_path));
      if (result != QDSP_CMD_SUCCESS) {
        MSG_ERROR("Codec Config command failed",0,0,0);
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        return(VOC_STATE_CONTINUE);
      }
      // clk_busy_wait(3000);
      #ifdef MSMAUD_SCMM
#error code not present
      #endif /* MSMAUD_SCMM */


      voc_state_data.codec_pending = FALSE;
    }

    /* Enable registered codec function */
    voc_enable_codec_func();

#ifdef FEATURE_VOC_ADPCM
   /*
    * VOCODER image with ADPCM decoder for shutter sound
    */
    if (voc_state_data.adpcm_mode_enabled == FALSE) {
      voc_state_data.adpcm_voc_config      = FALSE;
      voc_state_data.adpcm_num_channels    = 0;
      voc_state_data.adpcm_sampling_rate   = 0;
      voc_state_data.adpcm_block           = 0;
#ifdef FEATURE_VOC_G711
      voc_state_data.wav_config            = VOC_ADPCM_INVALID_MODE;
#endif /* FEATURE_VOC_G711 */
    }
#endif /* FEATURE_VOC_ADPCM */

    /* Transition to the IDLE state */
    MSG_HIGH("Vocoder going to IDLE",0,0,0);
    voc_state_data.state = VOC_STATE_IDLE_ENTRY;
    return(VOC_STATE_CONTINUE);
  } else { /* Vocoder is not acquired */

    /* Shut down clocks and codecs */
    voc_state_shutdown();

#if defined( FEATURE_ACP ) && defined( FEATURE_VOICE_RECORD )
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */
    /* Stay in SLEEP/INIT until vocoder is acquired */
    return(VOC_STATE_SUCCESS);
  }
} /* voc_state_init */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_idle_entry

DESCRIPTION
  Wait for the QDSP2 to reach IDLE state.  When QDSP2 is in IDLE, inform
  users, and transition the driver to IDLE state. If the QDSP2 does not reach
  IDLE within the allowed time, then perform error recovery.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_idle_entry (void)
{
  #if (defined(FEATURE_BT_AG) || defined(FEATURE_SBC_CODEC) || \
       defined(FEATURE_SBC_DSP_CODEC) )&& defined(FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
  #endif /*AVS_BT_SCO_REWORK*/

  if ( qdsp_cmd_get_state() != QDSP_STATE_IDLE ) {
    /* If QDSP failed to reach IDLE state, attempt retry */
    if ( voc_state_data.retry_counter < 100 ) {
      MSG_HIGH( "IDLE state transition retry", 0,0,0 );
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    }
    else {
      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR( "QDSP failed to reach IDLE state", 0,0,0 );
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  voc_state_data.retry_counter = 0;

  /* Issue sample slipping command */
  if ( qdsp_cmd_sample_slip_mode ( QDSP_SAMPLE_SLIP_MANUAL_V )
       != QDSP_CMD_SUCCESS ) {
    MSG_ERROR("Sample Slip command failed",0,0,0);
    voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
    return(VOC_STATE_CONTINUE);
  }

  if(voc_state_data.afe_configured == FALSE){
  /* Initialize the audio front end */
  if (voc_state_init_afe() != VOC_STATE_SUCCESS) {
    MSG_ERROR("Failed to initialize afe", 0, 0, 0);
    voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
    return(VOC_STATE_CONTINUE);
  }

  }

  /* Disable loopback tests */
  if (voc_state_data.test_mode != VOC_TEST_NONE) {
    #ifdef FEATURE_UP_LOOPBACK_TEST
    qdsp_cmd_up_packet_loopback(FALSE);
    #endif
    qdsp_cmd_loopback_test( QDSP_LOOPBACK_INIT );
    voc_state_data.test_mode = VOC_TEST_NONE;
  }

  #if defined (FEATURE_AVS_BT_SCO_REWORK)
  if (((voc_state_data.acquire_mask & (1 << VOC_ACQ_VOCVS)) ||
       (voc_state_data.acquire_mask & (1 << VOC_ACQ_VOCREC))) &&
      (bt_func_ptr != NULL) && !voc_bt_playback_started) {
    MSG_MED("Calling BT callback with PLAYBACK_SCO", 0,0,0);
    bt_func_ptr(VOC_PLAYBACK_SCO);
    voc_bt_playback_started = TRUE;
  }
  else if ((!(voc_state_data.acquire_mask & (1 << VOC_ACQ_VOCVS)) ||
           !(voc_state_data.acquire_mask & (1 << VOC_ACQ_VOCREC))) &&
            (bt_func_ptr != NULL) && voc_bt_playback_started) {
    MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
    bt_func_ptr(VOC_PLAYBACK_STOP);
    voc_bt_playback_started = FALSE;
  }
  #endif

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

  /* Turn analog part of Adie */
  #ifdef MSMAUD_SCMM
#error code not present
  #endif /* MSMAUD_SCMM */

  voc_state_data.state = VOC_STATE_IDLE;

  MSG_HIGH("Entered IDLE state",0,0,0);

  /* Inform users of transition to IDLE state */
  voc_inform(VOC_INFORM_IDLE);

  /* Going to idle state */
  return(VOC_STATE_CONTINUE_IMMEDIATE);
} /* voc_state_idle_entry */

#if defined(FEATURE_WAVE_PB) && defined(FEATURE_VOC_G711)
/*===========================================================================

FUNCTION voc_state_set_voc_wave_pb

DESCRIPTION
  Initialize DSP for wave playback, outside of vocstate's normal vocoder
  initialization and checks.

DEPENDENCIES
  DSP is in idle state.

RETURN VALUE
  False if any configuration failed.

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_state_set_voc_wave_pb()
{
  qdsp_cmd_voc_timing_type *timing_ptr;

  #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
  #else
  voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
  #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */

  qdsp_cmd_wav_config(voc_state_data.wav_config);

  if (qdsp_cmd_sample_slip_mode(QDSP_SAMPLE_SLIP_MANUAL_V) != QDSP_CMD_SUCCESS){
    MSG_ERROR("Sample Slip command failed",0,0,0);
    return (FALSE);
  }

  timing_ptr = voc_data_get_timing(VOC_CAP_G711);
  if (qdsp_cmd_voc_timing(timing_ptr) != QDSP_CMD_SUCCESS) {
    MSG_ERROR("Vocoder Timing command failed",0,0,0);
    return (FALSE);
  }

  if (voc_state_voc_config(VOC_CAP_G711)!= VOC_STATE_SUCCESS) {
    /* Giving a second chance. Wait for 5 ms */
    (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);
    if (voc_state_voc_config( voc_state_data.config )
        != VOC_STATE_SUCCESS ) {
      MSG_ERROR("Vocoder Configuration command failed",0,0,0);
      return (FALSE);
    }
  }

  if (qdsp_cmd_voc_frame_reference() != QDSP_CMD_SUCCESS) {
    /* Giving a second chance. Wait for 5 ms */
    (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);
    if (qdsp_cmd_voc_frame_reference() != QDSP_CMD_SUCCESS) {
      MSG_ERROR("Vocoder Frame Reference command failed",0,0,0);
      return (FALSE);
    }
  }
  return(TRUE);
}
#endif /* defined(FEATURE_WAVE_PB) && defined(FEATURE_VOC_G711) */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_idle

DESCRIPTION
  Stay in IDLE state until the vocoder is configured or released. If the
  vocoder is configured, transition to the correct state. If the vocoder is
  released transition to the SLEEP state.

DEPENDENCIES
  None

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_idle (void)
{
  qdsp_cmd_status_type     result;
  qdsp_cmd_voc_timing_type *timing_ptr;
  qdsp_module_type         module_select;
  voc_status_type          module_select_status;
  boolean                  go_to_sleep = TRUE;
  uint16                   slip_mode_qdsp;
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#ifdef FEATURE_VOC_ADPCM
  qdsp_cmd_audio_config_type config;
#endif /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */

#ifdef FEATURE_BT_AG
  boolean                  bt_status = FALSE;
#endif

#ifdef FEATURE_AVS_INCALL_ADEC_PB
  boolean                  support_image = FALSE;
#endif


#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif /*AVS_BT_SCO_REWORK*/

  if (voc_state_data.codec_pending == TRUE)
  {
    voc_retry_codec_select();
  }

  if (voc_state_data.test_mode_pending != voc_state_data.test_mode)
  {
    voc_state_data.test_mode = voc_state_data.test_mode_pending;

    if (voc_state_data.test_mode == VOC_TEST_AUDIO_LOOP)
    {
      qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL));
      qdsp_cmd_loopback_test( QDSP_LB_AUDIO_AUD_V );
    }
    else
    {
      qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_MUTE_VOL));
      qdsp_cmd_loopback_test( QDSP_LOOPBACK_INIT );
    }
  }

  module_select = 0;
  module_select_status = voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_ENABLED) &&
      (voc_state_data.acquire_mask != 0)) {

    if (voc_state_data.module_reqd == module_select) {
      go_to_sleep = FALSE;
    }
    else {
      MSG_HIGH("qdsp_enable(%d,%d)",
               voc_state_data.current_qdsps_app, module_select,0);
	  voc_state_data.module_status = VOC_STATE_MOD_STATUS_ENABLE_REQUESTED;
	  qdsp_cmd_clear_dsp_isr();
      qdsp_enable (voc_state_data.current_qdsps_app, module_select);
      voc_state_module_commit(module_select);
      if(voc_state_get_service_cfg_required()!=VOC_SERVICE_MAX )
      {
        if(voc_state_get_service_cfg_required() == voc_data_get_service_option()){
          return (VOC_STATE_CONTINUE);
        }
        voc_data_set_service_option(voc_state_get_service_cfg_required());
        MSG_MED("VOC_STATE_IDLE:service config set %d, going to SLEEP",
                                 voc_data_get_service_option(),0,0);
      }else{
         voc_data_set_service_option(VOC_SERVICE_MAX);
        MSG_MED("VOC_STATE_IDLE:service config set MAX, going to SLEEP",0,0,0);
      }
      go_to_sleep = TRUE;
    }
  }

  if (go_to_sleep == FALSE) {
    if ( module_select_status == VOC_DONE_S ) {
      voc_state_data.config = voc_state_data.config_pending;
      voc_data_set_service_option(voc_state_get_service_cfg_required());
      MSG_MED("VOC_STATE_IDLE: service config set %d",
                               voc_data_get_service_option(),0,0);
    }

#ifdef FEATURE_VOC_PCM_INTERFACE
    if (voc_state_data.pcm_int_mode_pending && !voc_state_data.pcm_int_mode) {
      qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL));
    }
    else if (!voc_state_data.pcm_int_mode_pending &&
             voc_state_data.pcm_int_mode &&
             voc_state_data.test_mode != VOC_TEST_AUDIO_LOOP) {
      qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_MUTE_VOL));
    }
    voc_state_set_pcm_modes();
#endif /* FEATURE_VOC_PCM_INTERFACE */

#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

#if defined(FEATURE_WAVE_PB) && defined(FEATURE_VOC_G711)
    if((voc_state_data.module_reqd == QDSP_MODULE_G711) &&
       (voc_state_data.adpcm_mode_enabled) &&
       (voc_state_data.wav_config != VOC_ADPCM_INVALID_MODE))
    {
      if(voc_state_set_voc_wave_pb() == TRUE) {
        voc_state_data.state = VOC_STATE_WAV_VOCODER_ENTRY;
#ifdef FEATURE_BT_AG
        if (voc_get_bt_state() == VOC_BT_STATE_AG_CODEC_CONFIG_PENDING) {
          bt_status = voc_complete_bt_ag();
          if (!bt_status) {
            /* BT state could not be transitioned to AG ENABLED due to
            DSP not accepting a codec config */
            if ( voc_state_data.retry_counter < 100 ) {
              MSG_HIGH( "VOCODER BT state transition retry", 0,0,0 );
              voc_state_data.retry_counter++;
              return(VOC_STATE_CONTINUE);
            } else {
              /* If a retry has already been attempted, go to ERROR state */
              MSG_ERROR( "BT state failed to reach AG ENABLED state", 0,0,0 );
              voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
              return(VOC_STATE_CONTINUE);
            }
          } else {
            voc_state_data.retry_counter = 0;
          }
        } else if(!voc_check_bt(VOC_BT_OP_AG_NORMAL)) {
          return(VOC_STATE_SUCCESS);
        }
#endif /* FEATURE_BT_AG */

        return (VOC_STATE_CONTINUE);
      } else {
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        return (VOC_STATE_CONTINUE);
      }
    }
    else
#endif /* defined(FEATURE_WAVE_PB) && defined(FEATURE_VOC_G711) */
#ifdef FEATURE_VOC_ADPCM
#if defined(QDSP_IMAGE_VOC_DEFINED)            || \
    defined(QDSP_IMAGE_VOC_COMBO_DEFINED)      || \
    defined(QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED) || \
    defined(QDSP_IMAGE_VOC_EVRC_DEFINED)       || \
    defined(QDSP_IMAGE_VOC_QTV_DEFINED)        || \
    defined(QDSP_IMAGE_VOC_E90)                || \
    defined(QDSP_IMAGE_VOC_EVRC_E90)           || \
    defined(QDSP_IMAGE_FR_DEFINED)             || \
    defined(QDSP_IMAGE_EFR_DEFINED)            || \
    defined(QDSP_IMAGE_AMR_DEFINED)            || \
    defined(QDSP_IMAGE_AMR_WB_DEFINED)       || \
    defined(QDSP_IMAGE_HR_DEFINED)
    /* New state for ADPCM VOCODER need to check for IMAGE
     */
    if((voc_state_data.adpcm_mode_enabled) && (voc_state_data.adpcm_voc_config))
      {
        switch( voc_state_data.current_image) {
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

      config.audio_config  = QDSP_AUDIO_MODE_ADPCM;
      config.num_channels  = voc_state_data.adpcm_num_channels;
      config.sampling_rate = voc_state_data.adpcm_sampling_rate;
      config.block_size    = voc_state_data.adpcm_block;

#if defined (FEATURE_BT_AG)     || \
    defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
      /* Before internal codec clock is pumped up to 32KHz, BT
      ** link has to be shutdown.
      */
      if (voc_get_bt_state() != VOC_BT_STATE_DISABLED) {
        MSG_HIGH("BlueTooth link disconnect",0,0,0);
        (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
      }
#if defined (FEATURE_AVS_BT_SCO_REWORK)
        if ((bt_func_ptr != NULL) && voc_bt_playback_started)
        {
          MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
          bt_func_ptr(VOC_PLAYBACK_STOP);
          voc_bt_playback_started = FALSE;
        }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* FEATURE_BT_AG || FEATURE_SBC_CODEC || FEATURE_SBC_DSP_CODEC */

      if (qdsp_cmd_audio_config(&config) != QDSP_CMD_SUCCESS) {
         /* Giving a second chance. Wait for 5 ms */
         (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);
         if (qdsp_cmd_audio_config(&config) != QDSP_CMD_SUCCESS ) {
           MSG_ERROR("Vocoder ADPCM Audio Configuration command failed",0,0,0);
           voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
           return (VOC_STATE_CONTINUE);
         }
      }

      if (qdsp_cmd_voc_frame_reference() != QDSP_CMD_SUCCESS) {
        /* Giving a second chance. Wait for 5 ms */
        (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);
        if (qdsp_cmd_voc_frame_reference() != QDSP_CMD_SUCCESS) {
          MSG_ERROR("Vocoder Frame Reference command failed",0,0,0);
          voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
          return (VOC_STATE_CONTINUE);
        }
      }

      MSG_HIGH("Going to ADPCM VOCODER state",0,0,0);
      voc_state_data.state = VOC_STATE_ADPCM_VOCODER_ENTRY;

#ifdef FEATURE_BT_AG
      if (voc_get_bt_state() == VOC_BT_STATE_AG_CODEC_CONFIG_PENDING) {
        bt_status = voc_complete_bt_ag();
        if (!bt_status) {
          /* BT state could not be transitioned to AG ENABLED due to
          DSP not accepting a codec config */
          if ( voc_state_data.retry_counter < 100 ) {
            MSG_HIGH( "VOCODER BT state transition retry", 0,0,0 );
            voc_state_data.retry_counter++;
            return(VOC_STATE_CONTINUE);
          } else {
            /* If a retry has already been attempted, go to ERROR state */
            MSG_ERROR( "BT state failed to reach AG ENABLED state", 0,0,0 );
            voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
            return(VOC_STATE_CONTINUE);
          }
        } else {
          voc_state_data.retry_counter = 0;
        }
      } else if(!voc_check_bt(VOC_BT_OP_AG_NORMAL)) {
        return(VOC_STATE_SUCCESS);
      }
#endif /* FEATURE_BT_AG */


        return (VOC_STATE_CONTINUE);

          default:

        MSG_ERROR("Vocoder VFE ADPCM image wrong",0,0,0);
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        return (VOC_STATE_CONTINUE);
      } /* switch( voc_state_data.current_image) */
    } else
#endif /* QDSP_IMAGE_VOC_DEFINED          ||
          QDSP_IMAGE_VOC_EVRC_DEFINED     ||
          QDSP_IMAGE_VOC_QTV_DEFINED ||
          QDSP_IMAGE_VOC_E90_DEFINED      ||
          QDSP_IMAGE_VOC_EVRC_E90_DEFINED ||
          QDSP_IMAGE_FR_DEFINED           ||
          QDSP_IMAGE_EFR_DEFINED          ||
          QDSP_IMAGE_AMR_DEFINED          ||
          QDSP_IMAGE_AMR_WB_DEFINED       ||
          QDSP_IMAGE_HR_DEFINED */
#endif /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_BT_AG
      if (voc_get_bt_state() == VOC_BT_STATE_AG_CODEC_CONFIG_PENDING)
      {
        bt_status = voc_complete_bt_ag();
        if (!bt_status)
        {
          /* BT state could not be transitioned to AG ENABLED due to
          DSP not accepting a codec config */
          if ( voc_state_data.retry_counter < 100 ) {
            MSG_HIGH( "VOCODER BT state transition retry", 0,0,0 );
            voc_state_data.retry_counter++;
            return(VOC_STATE_CONTINUE);
          } else {
            /* If a retry has already been attempted, go to ERROR state */
            MSG_ERROR( "BT state failed to reach AG ENABLED state", 0,0,0 );
            voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
            return(VOC_STATE_CONTINUE);
          }
        }
        else
        {
          voc_state_data.retry_counter = 0;
        }
      } else if(!voc_check_bt(VOC_BT_OP_AG_NORMAL)) {
        return(VOC_STATE_SUCCESS);
      }
#endif /* FEATURE_BT_AG */

    if ((voc_state_data.config_mask != 0) &&
        (voc_state_data.config_standby == FALSE)) {

      switch (voc_state_data.config) {
#if defined(FEATURE_AVS_INCALL_ADEC_PB)
#ifdef FEATURE_QTUNES
        case VOC_CAP_QTUNES:
          support_image = FALSE;

          switch (voc_state_data.current_image) {
#if defined(QDSP_IMAGE_VOC_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_VOC_EVRC_MP3_DEFINED) || \
    defined(QDSP_IMAGE_AMR_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_HR_MP3_DEFINED)       || \
    defined(QDSP_IMAGE_EFR_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_FR_MP3_DEFINED)
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
              support_image = TRUE;
              break;
#endif
            default:
              support_image = FALSE;
              break;
          } /* switch (voc_state_data.current_image) */

          if (support_image == FALSE) {
            break;
          }
#endif /* FEATURE_QTUNES */
#endif /* FEATURE_AVS_INCALL_ADEC_PB */
        case VOC_CAP_IS733:
        case VOC_CAP_IS127:
#ifdef MSMAUD_VOC_IS127_VOIP
        case VOC_CAP_IS127_VOIP:
#endif /* MSMAUD_VOC_IS127_VOIP  */
#ifdef MSMAUD_VOC_4GV
        case VOC_CAP_4GV_NB:
#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif /* MSMAUD_VOC_4GV_VOIP */
#endif /* MSMAUD_VOC_4GV */
#ifdef MSMAUD_VOC_4GV_WB
        case VOC_CAP_4GV_WB:
#ifdef MSMAUD_VOC_4GV_WB_VOIP
        case VOC_CAP_4GV_WB_VOIP:
#endif /* MSMAUD_VOC_4GV_WB_VOIP */
#endif /* MSMAUD_VOC_4GV_WB */
#if defined(MSMAUD_VOC_AMR)
        case VOC_CAP_AMR:
#endif /* defined(MSMAUD_VOC_AMR) */
#if defined(MSMAUD_VOC_AMR_WB_COMMON)
        case VOC_CAP_AMR_WB:
#endif /* defined(MSMAUD_VOC_AMR_WB_COMMON) */
#if defined(MSMAUD_VOC_EFR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) */
#if defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_FR) */
#if defined(MSMAUD_VOC_HR)
        case VOC_CAP_GSM_HR:
#endif /* defined(MSMAUD_VOC_HR) */
#ifdef MSMAUD_VOC_G711
        case VOC_CAP_G711:
#endif /* MSMAUD_VOC_G711 */
#ifdef MSMAUD_VOC_G723
        case VOC_CAP_G723:
#endif /* MSMAUD_VOC_G723 */

#ifdef FEATURE_VOICE_PLAYBACK
          /* Decide what to do with current voice service activities */
          voc_vr_check_so_change ();
#endif
          if (voc_state_data.slip_mode_pending == VOC_STATE_SLIP_MAN
#ifdef FEATURE_PHONE_VR
              ||  voc_state_data.vr_mode == VOC_VR_MODE_BC
#endif
              ) {
            slip_mode_qdsp = QDSP_SAMPLE_SLIP_MANUAL_V;
          } else {
            slip_mode_qdsp = QDSP_SAMPLE_SLIP_AUTO_V;
          }
          voc_state_data.slip_mode = voc_state_data.slip_mode_pending;

          if (qdsp_cmd_sample_slip_mode(slip_mode_qdsp) != QDSP_CMD_SUCCESS) {
            MSG_ERROR("Sample Slip command failed",0,0,0);
            voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
            return (VOC_STATE_CONTINUE);
          }

          timing_ptr = voc_data_get_timing(voc_state_data.config);
          if (qdsp_cmd_voc_timing(timing_ptr) != QDSP_CMD_SUCCESS) {
            MSG_ERROR("Vocoder Timing command failed",0,0,0);
            voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
            return (VOC_STATE_CONTINUE);
          }

          if (voc_state_voc_config(voc_state_data.config)!= VOC_STATE_SUCCESS) {
            /* Giving a second chance. Wait for 5 ms */
            (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);
            if (voc_state_voc_config( voc_state_data.config )
                != VOC_STATE_SUCCESS ) {
              MSG_ERROR("Vocoder Configuration command failed",0,0,0);
              voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
              return (VOC_STATE_CONTINUE);
            }
          }

#ifdef FEATURE_AVS_INCALL_ADEC_PB
#ifdef FEATURE_QTUNES
          /* Need to transit to vocoder+ADEC state at this point. */
          switch (voc_state_data.current_image) {
#if defined(QDSP_IMAGE_VOC_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_VOC_EVRC_MP3_DEFINED) || \
    defined(QDSP_IMAGE_AMR_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_HR_MP3_DEFINED)       || \
    defined(QDSP_IMAGE_EFR_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_FR_MP3_DEFINED)
#ifdef QDSP_IMAGE_VOC_MP3_DEFINED
            case QDSP_IMAGE_VOC_MP3:
#endif /* QDSP_IMAGE_VOC_DEFINED */
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

              if (voc_state_data.config == VOC_CAP_QTUNES) {
                config.audio_config  = QDSP_AUDIO_MODE_MP3;
                config.num_channels  = 0;
                config.sampling_rate = 0;
                config.block_size    = 0;

                if (qdsp_cmd_audio_config(&config) != QDSP_CMD_SUCCESS) {
                  /* Giving a second chance. Wait for 5 ms */
                  (void) rex_timed_wait(VOC_STATE_GP_SIG,
                                        &voc_state_gp_timer,
                                        5);

                  if (qdsp_cmd_audio_config(&config) != QDSP_CMD_SUCCESS) {
                    MSG_ERROR("Vocoder ADEC Audio Configuration command failed",
                              0,0,0);
                    voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
                    return (VOC_STATE_CONTINUE);
                  }
                }
              }
              break;
#endif
            default:
              break;
          } /* switch (voc_state_data.current_image) */
#endif /* FEATURE_QTUNES */
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

          /* If the sample slip mode is manual, then issue a manual frame
          ** reference command to cause the QDSP to transition to
          ** VOCODER state.
          */
          if (voc_state_data.slip_mode == VOC_STATE_SLIP_MAN) {
            if (qdsp_cmd_voc_frame_reference() != QDSP_CMD_SUCCESS) {
              /* Giving a second chance. Wait for 5 ms */
              (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);
              if (qdsp_cmd_voc_frame_reference() != QDSP_CMD_SUCCESS) {
                MSG_ERROR("Vocoder Frame Reference command failed",0,0,0);
                voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
                return (VOC_STATE_CONTINUE);
              }
            }
          }

          switch (voc_state_data.current_image) {
#ifdef FEATURE_AVS_INCALL_ADEC_PB
#ifdef FEATURE_QTUNES
#if defined(QDSP_IMAGE_VOC_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_VOC_EVRC_MP3_DEFINED) || \
    defined(QDSP_IMAGE_AMR_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_HR_MP3_DEFINED)       || \
    defined(QDSP_IMAGE_EFR_MP3_DEFINED)      || \
    defined(QDSP_IMAGE_FR_MP3_DEFINED)
#ifdef QDSP_IMAGE_VOC_MP3_DEFINED
            case QDSP_IMAGE_VOC_MP3:
#endif /* QDSP_IMAGE_VOC_DEFINED */
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

              if (voc_state_data.config == VOC_CAP_QTUNES) {

                voc_state_data.state = VOC_STATE_ADEC_VOCODER_ENTRY;
                MSG_HIGH("Going to ADEC_VOCODER_ENTRY state",0,0,0);
              }
              break;
#endif
#endif /* FEATURE_QTUNES */
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

            default:
              voc_state_data.state = VOC_STATE_VOCODER_ENTRY;
              MSG_HIGH("Going to VOCODER state",0,0,0);
              break;
          } /* switch (voc_state_data.current_image) */
          break;

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

        case VOC_CAP_NONE:
        case VOC_CAP_IS96:
        case VOC_CAP_IS96A:
#ifdef FEATURE_QSYNTH_COMMON
        case VOC_CAP_QSYNTH:
#endif
#if !defined(FEATURE_AVS_INCALL_ADEC_PB) && defined(FEATURE_QTUNES)
        case VOC_CAP_QTUNES:
#endif
#ifdef FEATURE_QTUNES_AAC
        case VOC_CAP_QTUNES_AAC:
#endif
#ifdef FEATURE_QTUNES_RA
        case VOC_CAP_QTUNES_RA:
#endif
        case VOC_CAP_MAX:
        default:
          break;
      }
      return (VOC_STATE_CONTINUE);
    } else {
      /* Vocoder is not configured, just stay in IDLE */
      return (VOC_STATE_SUCCESS);
    }
  } else {
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
    /* Disable the bluetooth AG when DSP is going to sleep */
    if ((voc_state_data.acquire_mask == 0) &&
        (voc_get_bt_state() != VOC_BT_STATE_DISABLED)) {
      (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
    }
#if defined (FEATURE_AVS_BT_SCO_REWORK)
      if ((bt_func_ptr != NULL) && voc_bt_playback_started)
      {
        MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
        bt_func_ptr(VOC_PLAYBACK_STOP);
        voc_bt_playback_started = FALSE;
      }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* FEATURE_BT_AG */

/* check for any change in the PCM interface */
#ifdef FEATURE_VOC_PCM_INTERFACE
    voc_state_set_pcm_modes();
#endif /* FEATURE_VOC_PCM_INTERFACE */

voc_state_data.afe_configured =FALSE;


    /* set that slowtalk STATIC parameters are not updated,
       so next time when voice call starts, it will be updated
    */
    voc_data_set_slowtalk_static_params_status(
                    VOC_SLOWTALK_STATIC_PARAMS_NOT_UPDATED);

    /* Go to sleep */
    voc_state_data.state = VOC_STATE_SLEEP;
    qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_MUTE_VOL));
    (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 4);

    voc_state_adie_stall();
    result = qdsp_cmd_sleep(voc_state_aux_pcm_clk_query());
    if (result == QDSP_CMD_SUCCESS) {
      MSG_HIGH("Vocoder going to SLEEP",0,0,0);
    } else {
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      MSG_ERROR("Sleep command failed",0,0,0);
    }
    return (VOC_STATE_CONTINUE);
  }
} /* voc_state_idle */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_vocoder_entry

DESCRIPTION
  When the DSP to arrives in VOCODER state, set the volume, inform users
  of the state change, and transition state machine to VOCODER state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  Volume is set (unmuted).

===========================================================================*/
voc_state_return_type voc_state_vocoder_entry (void)
{
#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
    voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
    bt_func_ptr = voc_data_get_bt_cb_func();
#endif/*AVS_BT_SCO_REWORK*/

  if ( qdsp_cmd_get_state() != QDSP_STATE_VOCODER ) {
    /* If QDSP failed to reach VOCODER state. Attempt retry, max 1s */
    if ( voc_state_data.retry_counter < 100 ) {
      MSG_HIGH( "VOCODER state transition retry", 0,0,0 );
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    } else {
      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR( "QDSP failed to reach VOCODER state", 0,0,0 );
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

#if defined (FEATURE_AVS_BT_SCO_REWORK)
  if (((voc_state_data.acquire_mask & (1 << VOC_ACQ_VOCVS)) ||
       (voc_state_data.acquire_mask & (1 << VOC_ACQ_VOCREC))) &&
       (bt_func_ptr != NULL) && !voc_bt_playback_started)
   {
     MSG_MED("Calling BT callback with PLAYBACK_SCO", 0,0,0);
     bt_func_ptr(VOC_PLAYBACK_SCO);
     voc_bt_playback_started = TRUE;
   }
#endif

  voc_state_data.retry_counter = 0;

  MSG_HIGH("Entered VOCODER state",0,0,0);

  /* Reset config flag */
  voc_state_data.config_flag = FALSE;
  voc_state_data.afe_configured =TRUE;
  voc_state_data.state       = VOC_STATE_VOCODER;

#ifdef FEATURE_VOC_DTMF_DETECT
  /* Enable DTMF detection */
  voc_dtmf_detect_enable();
#endif /* FEATURE_VOC_DTMF_DETECT */

  /* Inform users of transition to VOCODER state */
  voc_inform(VOC_INFORM_VOCODER);

  /* Issue volume control command */
  if (qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL))
       != QDSP_CMD_SUCCESS)
  {
     MSG_ERROR("Failed to set volume", 0, 0, 0);
     voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
     return (VOC_STATE_CONTINUE);
  }
#if defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_GSM) */

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

  /* now we are in VOCODER state so update the slowtalk configuration */
  voc_update_slowtalk_config();

  return (VOC_STATE_CONTINUE_IMMEDIATE);
} /* voc_state_vocoder_entry */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_vocoder

DESCRIPTION
  Monitor the current configuration. If the configuration changes or is
  cleared, command the DSP to transition to IDLE state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None.

===========================================================================*/
voc_state_return_type voc_state_vocoder (void)
{
  qdsp_module_type module_select;
  uint16          slip_mode_qdsp;
  voc_status_type module_select_status;

  /* Clear any previous error condition */
  voc_state_clear_fw_err_cnt();

  /* Update the sample slip mode
  */
  if (voc_state_data.slip_mode_pending != voc_state_data.slip_mode) {
    if (voc_state_data.slip_mode_pending == VOC_STATE_SLIP_MAN) {
      slip_mode_qdsp = QDSP_SAMPLE_SLIP_MANUAL_V;
    } else {
      slip_mode_qdsp = QDSP_SAMPLE_SLIP_AUTO_V;
    }
    voc_state_data.slip_mode = voc_state_data.slip_mode_pending;

    MSG_HIGH("Setting Sample Slip mode = %d",voc_state_data.slip_mode,0,0);
    if (qdsp_cmd_sample_slip_mode(slip_mode_qdsp) != QDSP_CMD_SUCCESS) {
      MSG_ERROR("Sample Slip command failed",0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return (VOC_STATE_CONTINUE);
    }
  }

#ifdef FEATURE_VOC_PCM_INTERFACE
  voc_state_set_pcm_modes();
#endif /* FEATURE_VOC_PCM_INTERFACE */

  /* Determine whether the vocoder should stay in VOCODER state
  ** or return to IDLE.
  */
  module_select = 0;
  module_select_status =  voc_state_get_high_module(&module_select);
 switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (voc_state_data.config_mask == 0) ||
      ((voc_state_data.config_flag || voc_state_data.config_standby)
#ifdef FEATURE_PHONE_VR
        && ((voc_state_data.module_reqd != QDSP_MODULE_VRBC)
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
           )
#endif /* FEATURE_PHONE_VR */
      ) ||
      (voc_state_data.config_pending != voc_state_data.config) ||
      (voc_state_data.test_mode_pending != voc_state_data.test_mode)
#ifdef FEATURE_VOC_ADPCM
      || ((voc_state_data.adpcm_mode_enabled) &&
          (voc_state_data.adpcm_voc_config))
#endif /* FEATURE_VOC_ADPCM */
#ifdef FEATURE_BT_AG
      || (voc_get_bt_state() == VOC_BT_STATE_AG_CODEC_CONFIG_PENDING)
#endif
      || (voc_state_data.codec_pending==TRUE)
     )
  {
#ifdef FEATURE_PHONE_VR
    /* Abort VR if the current image is Mode BC or UFE.
    */
    if (voc_state_data.module_reqd == QDSP_MODULE_VRBC
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
       )
    {
      MSG_HIGH ("Voc driver aborts VR",0,0,0);
      (void) vre_abort_vr (VRE_VOC_ABORT);
    }
#endif /* FEATURE_PHONE_VR */

    if (voc_state_go_to_idle() == VOC_STATE_SUCCESS) {
      voc_state_data.state = VOC_STATE_IDLE_ENTRY;
      if (voc_state_data.codec_pending==TRUE)
      {
        voc_retry_codec_select();
      }
    } else {
      MSG_ERROR("VOCODER->IDLE failed",0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
    }
    return (VOC_STATE_CONTINUE);
  } else {
#ifdef FEATURE_BT_AG
    voc_check_bt(VOC_BT_OP_AG_NORMAL);
#endif /* FEATURE_BT_AG */
    return (VOC_STATE_SUCCESS);
  }
} /* voc_state_vocoder */

#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */

#ifdef FEATURE_VOC_ADPCM
/*===========================================================================

FUNCTION voc_state_adpcm_vocoder_entry

DESCRIPTION
  Wait for the QDSP to reach ADPCM VOCODER state.  When QDSP is in VOCODER
  state, brings up internal codec clock to 32KHz, and transits the driver to
  ADPCM VOCODER state.
  Otherwise if re-tried within the allowed time, then perform error recovery.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_adpcm_vocoder_entry(void)
{
#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
     defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif

  if (qdsp_cmd_get_state() != QDSP_STATE_ADPCM_VOC) {
    /* If QDSP failed to reach DFM+VOCODER state. Attempt retry, max 1s */
    if (voc_state_data.retry_counter < 100) {
      MSG_HIGH("VOCODER+ADPCM state transition retry", 0,0,0);
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    } else {
      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR("QDSP failed to reach VOCODER+ADPCM state", 0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  voc_state_data.state = VOC_STATE_ADPCM_VOCODER;

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
  voc_check_bt(VOC_BT_OP_NORMAL);

#if defined (FEATURE_AVS_BT_SCO_REWORK)
  if (bt_func_ptr != NULL) {
    MSG_MED("Calling BT callback with PLAYBACK SCO", 0,0,0);
    bt_func_ptr( VOC_PLAYBACK_SCO );
    voc_bt_playback_started = TRUE;
  }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* BT_AG | SBC_CODEC */

  #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
  #else
  voc_state_set_icodec_clk(VOC_SAMPLE_RATE_32000);
  #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */

  /* Allow the clock to stabilize */
  #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
  clk_busy_wait(200);
  #else
#error code not present
  #endif


  /* Setup adec callback function
   */
  (void) qdsp_cmd_adec_config(voc_adec_isr);

  voc_inform(VOC_INFORM_VOCODER_ADPCM);

  return (VOC_STATE_CONTINUE);
}

/*===========================================================================

FUNCTION voc_state_adpcm_vocoder

DESCRIPTION
  Monitor the current configuration. If the configuration changes or is
  cleared, command the DSP to transition to IDLE state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None.

===========================================================================*/
voc_state_return_type voc_state_adpcm_vocoder(void)
{
  qdsp_cmd_status_type       result;
  voc_codec_type             pcm_path;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  voc_state_return_type      ret_val = VOC_STATE_SUCCESS;
  qdsp_module_type           module_select;
  voc_status_type 	     module_select_status;
#if (defined (FEATURE_BT_AG) && defined (FEATURE_AVS_BT_SCO_REWORK))
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif

  (void) voc_state_clear_fw_err_cnt();

  module_select_status =  voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (voc_state_data.acquire_mask == 0) ||
      (!voc_state_data.adpcm_mode_enabled) ||
      (!voc_state_data.adpcm_voc_config)   ||
      (voc_state_data.codec_pending==TRUE) ||
#ifdef FEATURE_BT_AG
      (voc_get_bt_state() == VOC_BT_STATE_AG_CODEC_CONFIG_PENDING) ||
#endif
      (!qdsp_cmd_get_adpcm_mode())) {

    /* Reset codec clock and jump back to IDLE state */
    switch (voc_state_data.current_image)
    {
#if defined(QDSP_IMAGE_4GV_WB_DEFINED)
    case QDSP_IMAGE_4GV_WB:
      voc_state_set_icodec_clk(VOC_SAMPLE_RATE_16000);
      break;
#endif /* defined(QDSP_IMAGE_4GV_WB_DEFINED) */
#if defined(QDSP_IMAGE_AMR_WB_DEFINED)
      case QDSP_IMAGE_AMR_WB:
        voc_state_set_icodec_clk(VOC_SAMPLE_RATE_16000);
        break;
#endif /* defined(QDSP_IMAGE_AMR_WB_DEFINED)*/
      default:
        #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
        #else
        voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
        #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
        break;
    }

    /* Allow the clock to stablize */
    #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
    clk_busy_wait(200);
 	#else
#error code not present
	#endif

#if defined (FEATURE_BT_AG)
    /* Disable the bluetooth AG when DSP is going to sleep */
    if ((voc_state_data.acquire_mask == 0) &&
       (voc_get_bt_state() != VOC_BT_STATE_DISABLED)) {
      (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
    }
#endif /* FEATURE_BT_AG */

    if (qdsp_cmd_idle() != QDSP_CMD_SUCCESS) {
      if (voc_state_data.retry_counter < 100) {
        MSG_ERROR("Failed to issue VOCODER IDLE command",0,0,0);
        voc_state_data.retry_counter++;
      } else {
        MSG_ERROR("ADPCM->VOCODER transition to IDLE failed",0,0,0);
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      }
      ret_val = VOC_STATE_CONTINUE;
    } else {
      if (voc_state_data.codec_pending==TRUE)
      {
        voc_state_data.state = VOC_STATE_IDLE_ENTRY;
        voc_retry_codec_select();
        return VOC_STATE_CONTINUE;
      }
      voc_data_get_codec(&pcm_path);

      /* If Blue tooth is still connected, will use the default codec to
       * send the codec config command */
#ifdef FEATURE_BT
      if ((pcm_path == VOC_CODEC_BT_INTERCOM)
#ifdef FEATURE_BT_AG
          || (pcm_path == VOC_CODEC_BT_AG)
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#endif /* FEATURE_BT_AG */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
          || (pcm_path == VOC_CODEC_BT_A2DP)
          || (pcm_path == VOC_CODEC_BT_A2DP_SCO)
#endif /* FEATURE_SBC_CODEC */
         ) {
        pcm_path = VOC_CODEC_DEFAULT;
      }
#endif /* FEATURE_BT */

      voc_state_set_codec_cal(pcm_path);
      voc_data_get_codec_config(voc_state_data.current_image, pcm_path,
                                &codec_config_ptr);

      result = qdsp_cmd_codec_config(codec_config_ptr,\
	    voc_data_get_default_frame_rate(voc_state_data.current_image, \
                                                                  pcm_path));
      if (result != QDSP_CMD_SUCCESS) {
        MSG_ERROR("Codec Config command failed",0,0,0);
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        ret_val = VOC_STATE_CONTINUE;
      }

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

      MSG_HIGH("Going to IDLE state", 0, 0, 0);

#if (defined (FEATURE_BT_AG) && defined (FEATURE_AVS_BT_SCO_REWORK))
      if ((bt_func_ptr != NULL) && voc_bt_playback_started) {
        MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
        bt_func_ptr(VOC_PLAYBACK_STOP);
        voc_bt_playback_started = FALSE;
      }
#endif

      voc_state_data.state = VOC_STATE_IDLE;
      ret_val = VOC_STATE_CONTINUE;
    }
  } else {

#ifdef FEATURE_BT_AG
    voc_check_bt(VOC_BT_OP_AG_NORMAL);
#endif /* FEATURE_BT_AG */
    ret_val = VOC_STATE_SUCCESS;
  }

  return (ret_val);
}
#endif /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_AVS_INCALL_ADEC_PB
#ifdef FEATURE_QTUNES
/*===========================================================================

FUNCTION voc_state_adec_vocoder_entry

DESCRIPTION
  Wait for the QDSP to reach ADEC+VOCODER state.  When QDSP is in VOCODER
  state, brings up internal codec clock to 32KHz, and transits the driver to
  ADPCM VOCODER state.
  Otherwise if re-tried within the allowed time, then perform error recovery.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_adec_vocoder_entry(void)
{
#ifdef FEATURE_BT_AG
  voc_bt_state_enum_type bt_state;
#endif /* FEATURE_BT_AG */

  if (qdsp_cmd_get_state() != QDSP_STATE_ADEC_VOC) {
    /* If QDSP failed to reach ADEC+VOCODER state. Attempt retry, max 1s */
    if (voc_state_data.retry_counter < 100) {
      MSG_HIGH("VOCODER+ADEC qdspState transition retry", 0,0,0);
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    } else {
      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR("Firmware failed to reach VOCODER+ADEC qdspState", 0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  voc_state_data.retry_counter = 0;

  if (qdsp_cmd_get_adec_state() != QDSP_STATE_TUNES) {
    /* If QDSP failed to reach ADEC+VOCODER state. Attempt retry, max 1s */
    if (voc_state_data.retry_counter < 100) {
      MSG_HIGH("VOCODER+ADEC adecState transition retry", 0,0,0);
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    } else {
      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR("Firmware failed to reach VOCODER+ADEC adecState", 0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  voc_state_data.retry_counter = 0;

  /* Configure ADEC interrupts and activate the buffer initialization */
  (void) qdsp_cmd_adec_config(voc_adec_isr);

#ifdef FEATURE_BT_AG
  bt_state = voc_get_bt_state();

  MSG_ERROR("bt_state = %d",bt_state,0,0);

  /* vocoder+ADEC doesn't support BT. Switch back to the internal codec */
  switch (voc_state_data.current_image) {
#ifdef QDSP_IMAGE_VOC_MP3_DEFINED
    case QDSP_IMAGE_VOC_MP3:
#endif
#ifdef QDSP_IMAGE_VOC_EVRC_MP3_DEFINED
    case QDSP_IMAGE_VOC_EVRC_MP3:
#endif
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
      if (voc_state_data.config == VOC_CAP_QTUNES) {
        if ((bt_state >= VOC_BT_STATE_AG_SCO_LINK_PENDING) &&
            (bt_state <= VOC_BT_STATE_AG_ENABLED)) {

          #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
          #else
          voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
          #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
          MSG_HIGH("BlueTooth link existing",0,0,0);

        } else if (bt_state == VOC_BT_STATE_DISABLED) {

          #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
          #else
          voc_state_set_icodec_clk(VOC_SAMPLE_RATE_48000);
          #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
          MSG_HIGH("BlueTooth link is not existing",0,0,0);
        }
      }
      break;

    default:
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return (VOC_STATE_CONTINUE);
  } /* switch (voc_state_data.current_image) */
#endif /* FEATURE_BT_AG */

  MSG_HIGH("Enter VOC_STATE_ADEC_VOCODER",0,0,0);

  if (qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL))
       != QDSP_CMD_SUCCESS)
  {
     MSG_ERROR("Failed to set volume", 0, 0, 0);
     voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
     return (VOC_STATE_CONTINUE);
  }

  /* Allow the clock to stabilize: wait for 20 msec */
  (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 20);

  voc_state_data.state = VOC_STATE_ADEC_VOCODER;

  voc_inform(VOC_INFORM_VOCODER_ADEC);

  /* Setup audio format pro-processing blocks
  */
  voc_set_audfmt_pp_cal();

  voc_set_audfmt_pp_config();

  return (VOC_STATE_CONTINUE);
}

/*===========================================================================

FUNCTION voc_state_adpcm_vocoder

DESCRIPTION
  Monitor the current configuration. If the configuration changes or is
  cleared, command the DSP to transition to IDLE state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None.

===========================================================================*/
voc_state_return_type voc_state_adec_vocoder(void)
{
  qdsp_cmd_status_type       result;
  voc_codec_type             pcm_path;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  voc_state_return_type      ret_val = VOC_STATE_SUCCESS;
  qdsp_module_type           module_select;
  voc_status_type			 module_select_status;

  module_select_status =  voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (voc_state_data.codec_pending==TRUE) ||
      (voc_state_data.acquire_mask == 0)) {

    /* Make sure DSP/AUDIO part has finished processing
     * FLUSH command */

    /* Reset codec clock and jump back to IDLE state */
    switch (module_select)
    {
#if defined(QDSP_IMAGE_AMR_WB_DEFINED)
    case QDSP_IMAGE_AMR_WB:
      voc_state_set_icodec_clk(VOC_SAMPLE_RATE_16000);
      break;
#endif /* defined(QDSP_IMAGE_AMR_WB_DEFINED)*/
#if defined(QDSP_IMAGE_4GV_WB_DEFINED)
    case QDSP_IMAGE_4GV_WB:
        voc_state_set_icodec_clk(VOC_SAMPLE_RATE_16000);
        break;
#endif /* defined(QDSP_IMAGE_4GV_WB_DEFINED) */
      default:
        #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
        #else
        voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
        #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
        break;
    }

    /* Allow the clock to stablize */
    (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 20);

    if (qdsp_cmd_idle() != QDSP_CMD_SUCCESS) {
      if (voc_state_data.retry_counter < 100) {
        MSG_ERROR("Failed to issue VOCODER IDLE command",0,0,0);
        voc_state_data.retry_counter++;
      } else {
        MSG_ERROR("ADPCM+VOCODER transition to IDLE failed",0,0,0);
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      }
      ret_val = VOC_STATE_CONTINUE;
    } else {
      if (voc_state_data.codec_pending == TRUE)
      {
        voc_state_data.state = VOC_STATE_IDLE_ENTRY;
        voc_retry_codec_select();
        return VOC_STATE_CONTINUE;
      }
      voc_data_get_codec(&pcm_path);
      voc_state_set_codec_cal(pcm_path);
      voc_data_get_codec_config(voc_state_data.current_image, pcm_path,
                                &codec_config_ptr);

      result = qdsp_cmd_codec_config(codec_config_ptr,\
	       voc_data_get_default_frame_rate(voc_state_data.current_image,\
	                                                          pcm_path));
      if (result != QDSP_CMD_SUCCESS) {
        MSG_ERROR("Codec Config command failed",0,0,0);
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        ret_val = VOC_STATE_CONTINUE;
      }

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */


      MSG_HIGH("ADEC_VOCODER state Going to IDLE state", 0, 0, 0);

      qdsp_write(QDSP_adecDataHostStickyInt, 0);

      voc_state_data.state = VOC_STATE_IDLE;
      ret_val = VOC_STATE_CONTINUE;
    }
  } else {
    ret_val = VOC_STATE_SUCCESS;
  }

  return (ret_val);
}
#endif /* FEATURE_QTUNES */
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#ifdef FEATURE_VOC_G711
/*===========================================================================

FUNCTION voc_state_wav_vocoder_entry

DESCRIPTION
  Wait for the QDSP to reach WAV VOCODER state.  Transits the driver to
  WAV VOCODER state, if possible.
  Otherwise if re-tried within the allowed time, then perform error recovery.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_wav_vocoder_entry(void)
{
#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
     defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif

  if (qdsp_cmd_get_state() != QDSP_STATE_VOCODER) {
    /* If QDSP failed to reach DFM+VOCODER state. Attempt retry, max 1s */
    if (voc_state_data.retry_counter < 100) {
      MSG_HIGH("VOCODER+WAV state transition retry", 0,0,0);
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    } else {
      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR("QDSP failed to reach VOCODER+WAV state", 0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  voc_state_data.state = VOC_STATE_WAV_VOCODER;

  /* Issue volume control command */
  if (qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL))
       != QDSP_CMD_SUCCESS)
  {
     MSG_ERROR("Failed to set volume", 0, 0, 0);
     voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
     return (VOC_STATE_CONTINUE);
  }
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
  voc_check_bt(VOC_BT_OP_NORMAL);

#if defined (FEATURE_AVS_BT_SCO_REWORK)
  if (bt_func_ptr != NULL) {
    MSG_MED("Calling BT callback with PLAYBACK SCO", 0,0,0);
    bt_func_ptr( VOC_PLAYBACK_SCO );
    voc_bt_playback_started = TRUE;
  }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* BT_AG | SBC_CODEC */

#ifdef FEATURE_WAVE_PB
  /* Setup adec callback function
   */
  (void) qdsp_cmd_adec_config(voc_adec_isr);
#endif /* FEATURE_WAVE_PB */

  return (VOC_STATE_CONTINUE);
}

/*===========================================================================

FUNCTION voc_state_wav_vocoder

DESCRIPTION
  Monitor the current configuration. If the configuration changes or is
  cleared, command the DSP to transition to IDLE state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None.

===========================================================================*/
voc_state_return_type voc_state_wav_vocoder(void)
{
  qdsp_cmd_status_type       result;
  voc_codec_type             pcm_path;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  voc_state_return_type      ret_val = VOC_STATE_SUCCESS;
  qdsp_module_type           module_select;
  voc_status_type			 module_select_status;
#if (defined (FEATURE_BT_AG) && defined (FEATURE_AVS_BT_SCO_REWORK))
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif

  module_select_status =  voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (voc_state_data.acquire_mask == 0)
#ifdef FEATURE_WAVE_PB
      || (!voc_state_data.adpcm_mode_enabled) ||
      (voc_state_data.wav_config == VOC_ADPCM_INVALID_MODE)
#endif /* FEATURE_WAVE_PB */
      || (voc_state_data.codec_pending == TRUE)
#ifdef FEATURE_BT_AG
      || (voc_get_bt_state() == VOC_BT_STATE_AG_CODEC_CONFIG_PENDING)
#endif

      ) {

//    qdsp_cmd_adpcm_gain(FALSE);
#if defined (FEATURE_BT_AG)
     /* Disable the bluetooth AG when DSP is going to sleep */
     if ((voc_state_data.acquire_mask == 0) &&
        (voc_get_bt_state() != VOC_BT_STATE_DISABLED)) {
       (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
     }
#endif /* FEATURE_BT_AG */

    if (qdsp_cmd_idle() != QDSP_CMD_SUCCESS) {
      if (voc_state_data.retry_counter < 100) {
        MSG_ERROR("Failed to issue VOCODER IDLE command",0,0,0);
        voc_state_data.retry_counter++;
      } else {
        MSG_ERROR("ADPCM->VOCODER transition to IDLE failed",0,0,0);
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      }
      ret_val = VOC_STATE_CONTINUE;
    } else {
      if (voc_state_data.codec_pending==TRUE)
      {
        voc_state_data.state = VOC_STATE_IDLE_ENTRY;
        voc_retry_codec_select();
        return VOC_STATE_CONTINUE;
      }

      /* Reset codec clock and jump back to IDLE state
      */
      #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
      #else
      voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
      #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */

      voc_data_get_codec(&pcm_path);
      voc_state_set_codec_cal(pcm_path);
      voc_data_get_codec_config(voc_state_data.current_image, pcm_path,
                                &codec_config_ptr);

       result = qdsp_cmd_codec_config(codec_config_ptr,\
          voc_data_get_default_frame_rate(voc_state_data.current_image,\
                                                                  pcm_path));
      if (result != QDSP_CMD_SUCCESS) {
        MSG_ERROR("Codec Config command failed",0,0,0);
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        ret_val = VOC_STATE_CONTINUE;
      }

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

      MSG_HIGH("Going to IDLE state", 0, 0, 0);

#if (defined (FEATURE_BT_AG) && defined (FEATURE_AVS_BT_SCO_REWORK))
      if ((bt_func_ptr != NULL) && voc_bt_playback_started) {
        MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
        bt_func_ptr(VOC_PLAYBACK_STOP);
        voc_bt_playback_started = FALSE;
      }
#endif
      voc_state_data.state = VOC_STATE_IDLE;
      ret_val = VOC_STATE_CONTINUE;
    }
  } else {

#ifdef FEATURE_BT_AG
    voc_check_bt(VOC_BT_OP_AG_NORMAL);
#endif /* FEATURE_BT_AG */
    ret_val = VOC_STATE_SUCCESS;
  }

  return (ret_val);
}
#endif /* FEATURE_VOC_G711 */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_nop

DESCRIPTION
  Function for unsupported states

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS

===========================================================================*/
voc_state_return_type voc_state_nop (void)
{
  return (VOC_STATE_SUCCESS);
} /* voc_state_nop */

/* <EJECT> */
#ifdef FEATURE_QSYNTH_COMMON
/*===========================================================================

FUNCTION voc_state_qsynth_init

DESCRIPTION
  If the vocoder is acquired, turn it on and send it to QSYNTH_IDLE state.
  If the vocoder is released, turn off the clocks.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS

===========================================================================*/
voc_state_return_type voc_state_qsynth_init (void)
{
  voc_codec_type             pcm_path;
  voc_codec_class_type       codec_type;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  qdsp_cmd_status_type       result;
  qdsp_module_type           module_select;
  voc_status_type            module_select_status;

  module_select_status = voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (voc_state_data.image_changed == TRUE)
#if defined(FEATURE_ACP) && defined(FEATURE_QSYNTH)
#error code not present
#endif /* defined(FEATURE_ACP) && defined(FEATURE_QSYNTH) */
     )
  {
    MSG_HIGH("Vocoder going to RESET",0,0,0);

    if ( module_select_status == VOC_DONE_S ) {
      voc_state_data.config = voc_state_data.config_pending;
    }
    voc_state_data.state  = VOC_STATE_RESET;
#ifdef FEATURE_EXTERNAL_SDAC
    voc_state_config_external_sdac(VOC_SAMPLE_RATE_NONE);
#endif
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  }

  if ((voc_state_data.acquire_mask) || (voc_state_data.reset_flag)) {
    /* Indicate first time up */
    voc_state_data.reset_flag = FALSE;

    /* Signal that it is no longer ok to sleep */
#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#ifndef FEATURE_GSM_PLT
#ifdef FEATURE_NEW_SLEEP_API
    sleep_negate_okts(voc_state_sleep_handle);
#else
    SLEEP_NEGATE_OKTS( SLEEP_VOC_OKTS_SIG );
#endif /* FEATURE_NEW_SLEEP_API  */
#endif
#endif /* T_SLEEP || FEATURE_SLEEP_TASK */

    /* Wake up QDSP if coming out of idle */
    if (voc_state_data.module_status != VOC_STATE_MOD_STATUS_ENABLED) {
      voc_state_data.state = VOC_STATE_RESET;
      return (VOC_STATE_CONTINUE_IMMEDIATE);
    }

    if ( voc_state_clocks_on == FALSE ) {
      /* Enable all vocoder and codec clocks */
      voc_state_enable_clocks();
      voc_state_clocks_on = TRUE;
    }

    /* Allow QDSP time to run */
    #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
    clk_busy_wait(200);
   	#else
#error code not present
	#endif

#ifdef MSMAUD_ADIE_CODEC
    /* Wake up the Analog Die if the codec in use is the internal codec. */
    (void) voc_state_adie_wakeup();
#endif /* MSMAUD_ADIE_CODEC */

    /* Get the codec configuration data */
    voc_data_get_codec(&pcm_path);
    voc_data_get_codec_class(&codec_type);

    /* Reset the configuration for this type of codec
    */
    voc_state_set_codec_cal(pcm_path);

#ifdef FEATURE_BT_AG
    if(((codec_type == VOC_CODEC_CLASS_BLUETOOTH)
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
     || (codec_type == VOC_CODEC_CLASS_BLUETOOTH_A2DP)
     || (codec_type == VOC_CODEC_CLASS_BLUETOOTH_A2DP_SCO)
#endif /* FEATURE_SBC_CODEC */
      ) && (voc_get_bt_state() == VOC_BT_STATE_DISABLED)) {
      voc_data_get_codec_config(voc_state_data.current_image,
                                VOC_CODEC_DEFAULT, &codec_config_ptr);
    } else
#endif /* FEATURE_BT_AG */
    {
      voc_data_get_codec_config(voc_state_data.current_image, pcm_path,
                                &codec_config_ptr);
    }

    /* Disable any registered codec functions */
    voc_disable_codec_func(FALSE);

#ifdef FEATURE_EXTERNAL_SDAC
    if (codec_type == VOC_CODEC_CLASS_EXTERNAL_STEREO) {
      /* NIPPER: set CODEC_CORE_DISABLE */
      voc_set_clk_ctrl_timer(FALSE, VOC_CLK_CTRL_STEREO_DAC);
      voc_state_config_external_sdac(voc_data_get_sample_rate());
    }
#endif

    /* Set ADEC mode */
#ifdef FEATURE_QSYNTH_ADPCM
    voc_state_set_qsynth_adec_mode(voc_state_data.adec_mode_pending);
    voc_state_data.adec_mode = voc_state_data.adec_mode_pending;
#else
    voc_state_set_qsynth_adec_mode(VOC_QSYNTH_ADEC_MODE_QCP);
#endif /* FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_QAUDIOFX
    qdsp_cmd_3d_pos_assign_query_buffs();
    qdsp_cmd_3d_pos_set_output_mode(voc_data_get_3d_pos_output_mode());
    qdsp_cmd_3d_reverb_check();
#endif /* FEATURE_QAUDIOFX */

/*
  set ICODEC CLOCK to 48Khz here 
*/
#ifdef QDSP_IMAGE_QSYNTH_RS_DEFINED
#error code not present
#endif

    /* 
      These functions are already taken care for synth and VOC combo image
      so introducing new image will not write to these address.
     */
       
    /* Set QSynth hybrid mode */
    qdsp_cmd_set_hybrid_mode(voc_state_data.hybrid_mode_pending);
    voc_state_data.hybrid_mode = voc_state_data.hybrid_mode_pending;

    /* Set QSynth volume level mode */
    qdsp_cmd_set_qsynth_vol_mode(
                            (uint16)voc_state_data.qsynth_vol_mode_pending);
    voc_state_data.qsynth_vol_mode = voc_state_data.qsynth_vol_mode_pending;

    /* Set QSynth DLS configuration */
    qdsp_cmd_set_dls_config(voc_data_get_dls_config());

#ifdef FEATURE_SBC_DSP_CODEC
   if (voc_get_bt_state() == VOC_BT_STATE_A2DP_ENABLED)
   {
     /* We need to send the SBC config again to the DSP */
     voc_sbc_reenable();
   }
#endif

    /* Issue codec configuration command */
    result = qdsp_cmd_codec_config(codec_config_ptr,\
             voc_data_get_default_frame_rate(voc_state_data.current_image,\
	                                                          pcm_path));
    if (result != QDSP_CMD_SUCCESS) {
      MSG_ERROR("Codec Config command failed",0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

    voc_state_data.codec_pending = FALSE;

    /* Setup audio format pro-processing calibration blocks
    */
    voc_set_audfmt_pp_cal();

    /* Enable registered codec function */
    voc_enable_codec_func();

#ifdef FEATURE_EXTERNAL_SDAC
    if (codec_type != VOC_CODEC_CLASS_EXTERNAL_STEREO) {
      voc_state_config_external_sdac(VOC_SAMPLE_RATE_NONE);
    }
#endif

    /* Transition to the QSYNTH state */
    MSG_HIGH("Vocoder going to QSYNTH",0,0,0);
    voc_state_data.state = VOC_STATE_QSYNTH_ENTRY;
    return(VOC_STATE_CONTINUE);

  } else { /* Vocoder is not acquired */
    /* Shut down clocks and codecs */
    voc_state_shutdown();

    /* Stay in SLEEP/INIT until vocoder is acquired */
    return(VOC_STATE_SUCCESS);
  }
} /* voc_state_qsynth_init */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_qsynth_entry

DESCRIPTION
  Wait for the QDSP2 to reach QSYNTH state.  When QDSP2 is in QSYNTH, inform
  users, and transition the driver to QSYNTH state. If the QDSP2 does not
  reach QSYNTH within the allowed time, then perform error recovery.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_qsynth_entry (void)
{
#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
  voc_pp_cb_type  pp_cb = NULL;
  voc_pp_type_type pp_config;
  qdsp_cmd_status_type status = QDSP_CMD_SUCCESS;
#endif /* FEATURE_ARM_POST_PROCESSING || FEATURE_AVS_WB_PCM_RX_INTERFACE  */

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();

#endif

  /* Generally volume is set by snd task when voc task ask for it
  by calling voc_inform but if there is a delay in setting the volume
  then there would be audio as default volume is 0x100(Fw Image).
  So,directly setting the previously set master volume  */
  voc_master_vol();

  if ( qdsp_cmd_get_state() != QDSP_STATE_SYNTH ) {
    /* If QDSP failed to reach QSYNTH state, attempt retry */
    if ( voc_state_data.retry_counter < 100 ) {
      MSG_HIGH( "QSYNTH state transition retry", 0,0,0 );
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    } else {
#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
      qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
      voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
#endif  /*(FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR( "QDSP failed to reach QSYNTH state", 0,0,0 );
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  voc_state_data.retry_counter = 0;

#if defined(FEATURE_ACP) && defined(FEATURE_QSYNTH)
#error code not present
#endif

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
#ifdef FEATURE_GASYNTH_INTERFACE
  (void) qdsp_cmd_set_adec_timer_cb(voc_set_adec_cmd_retry_timer);
#endif /* FEATURE_GASYNTH_INTERFACE */
  (void) qdsp_cmd_adec_config(voc_adec_isr);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */


#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
  pp_cb = voc_data_get_pp_func();
  if (pp_cb  != NULL)
  {
    pp_config = voc_data_get_pp_type();
    switch (pp_config) {
      case VOC_PP_TYPE_GENERIC:
      status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_GENERIC);
        break;
#if defined (FEATURE_SBC_CODEC)
      case VOC_PP_TYPE_SBC:
      status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_SBC);
        break;
#elif defined (FEATURE_SBC_DSP_CODEC)
      case VOC_PP_TYPE_DSP_SBC:
        status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_DSP_SBC);
        break;
#endif
    }
    if (status == QDSP_CMD_SUCCESS) {
    voc_state_pp_status = VOC_PP_STATUS_ENABLED;
  } else {
    /* If callback is null or if qdsp_cmd_register_pp_cb failed */
    voc_state_pp_status = VOC_PP_STATUS_DISABLED;
  }
  }
#endif /* FEATURE_ARM_POST_PROCESSING || FEATURE_AVS_WB_PCM_RX_INTERFACE */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
    voc_check_bt(VOC_BT_OP_NORMAL);

#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if (bt_func_ptr != NULL)
    {
      MSG_MED("Calling BT callback with PLAYBACK A2DP | SCO", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_SCO | VOC_PLAYBACK_A2DP);
      voc_bt_playback_started = TRUE;
    }
#endif/*AVS_BT_SCO_REWORK*/
#endif

  /* Turn analog part of Adie */
  #ifdef MSMAUD_SCMM
#error code not present
  #endif /* MSMAUD_SCMM */

  MSG_HIGH("Entered QSYNTH state",0,0,0);

  voc_state_data.state = VOC_STATE_QSYNTH;

  /* Inform users of transition to QSYNTH state */
  voc_inform(VOC_INFORM_QSYNTH);

  /* Setup audio format pro-processing blocks
  */
  voc_set_audfmt_pp_config();

  return(VOC_STATE_CONTINUE);
} /* voc_state_qsynth_entry */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_qsynth

DESCRIPTION
  Holding pattern for Qsynth functions.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_qsynth (void)
{
  qdsp_cmd_status_type     result;
  qdsp_module_type         module_select;
#if defined(FEATURE_ACP) && defined(FEATURE_QSYNTH)
#error code not present
#endif /* FEATURE_ACP && FEATURE_QSYNTH */
  voc_status_type	 module_select_status;

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();

#endif


  /* Clear any previous error condition */
  voc_state_clear_fw_err_cnt();

  module_select_status =  voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_ENABLED) &&
      (voc_state_data.module_reqd != module_select)) {
    MSG_HIGH("qdsp_enable(%d,%d)",
             voc_state_data.current_qdsps_app, module_select, 0);
    qdsp_enable( voc_state_data.current_qdsps_app, module_select );
    return (VOC_STATE_CONTINUE);
  }

  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
#if defined(FEATURE_QSYNTH_ADPCM) && !(defined(FEATURE_GASYNTH_INTERFACE))
     (voc_state_data.adec_mode != voc_state_data.adec_mode_pending) ||
#endif /* FEATURE_QSYNTH_ADPCM */
     (voc_state_data.hybrid_mode != voc_state_data.hybrid_mode_pending) ||
     (voc_state_data.qsynth_vol_mode!=voc_state_data.qsynth_vol_mode_pending) ||
     (voc_state_data.acquire_mask == 0) ||
     (voc_state_data.codec_pending == TRUE) ||
     (voc_state_data.state == VOC_STATE_QSYNTH_DISABLE_PENDING))
  {

#if defined(FEATURE_ACP) && defined(FEATURE_QSYNTH)
#error code not present
#endif /* FEATURE_ACP && FEATURE_QSYNTH */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
    /* Disable the bluetooth AG when DSP is going to sleep */
    if ((voc_state_data.acquire_mask == 0) &&
        (voc_get_bt_state() != VOC_BT_STATE_DISABLED)) {
      (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
    }
#ifdef FEATURE_SBC_DSP_CODEC
    else if ((voc_state_data.acquire_mask != 0) &&
             (voc_get_bt_mode() == VOC_BT_MODE_A2DP) &&
             (voc_get_bt_state() == VOC_BT_STATE_A2DP_ENABLED))
    {
      voc_sbc_disable();
    }
#endif
#endif /* FEATURE_BT_AG */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
#ifdef FEATURE_GASYNTH_INTERFACE
    (void) qdsp_cmd_set_adec_timer_cb(NULL);
    (void) voc_set_adec_cmd_retry_timer(FALSE);
#endif /* FEATURE_GASYNTH_INTERFACE */
    /* Disable audio decoder functions */
    (void) qdsp_cmd_adec_config(NULL);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
    qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
    voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
#endif /* FEATURE_ARM_POST_PROCESSING  || FEATURE_AVS_WB_PCM_RX_INTERFACE */

    qdsp_cmd_qsynth_cmd_dereg();
    voc_state_data.state = VOC_STATE_QSYNTH_DISABLE_PENDING;
    voc_state_adie_stall();
    result = qdsp_cmd_sleep(voc_state_aux_pcm_clk_query());
    if(result == QDSP_CMD_SUCCESS) {
#ifdef FEATURE_GASYNTH_INTERFACE
    (void) qdsp_cmd_reset_adec_cmd_buf();
#endif /* FEATURE_GASYNTH_INTERFACE */
      voc_state_data.state  = VOC_STATE_SLEEP;
      voc_state_data.retry_counter = 0;
    } else {
      MSG_HIGH("Sleep command failed",0,0,0);
      voc_state_data.retry_counter++;

      if (voc_state_data.retry_counter == 100) {
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        MSG_ERROR( "Failed to issue SLEEP command", 0,0,0 );
      }
    }
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)

#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if ((bt_func_ptr != NULL) && voc_bt_playback_started)
    {
      MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_STOP);
      voc_bt_playback_started = FALSE;
    }
#endif /*AVS_BT_SCO_REWORK*/
#endif

    return(VOC_STATE_CONTINUE);

  }
#if defined(FEATURE_ACP) && defined(FEATURE_QSYNTH)
#error code not present
#endif /* FEATURE_ACP && FEATURE_QSYNTH */

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
    voc_check_bt(VOC_BT_OP_NORMAL);
#endif

  return(VOC_STATE_SUCCESS);
} /* voc_state_qsynth */

#endif /* FEATURE_QSYNTH_COMMON */

/* <EJECT> */
#ifdef FEATURE_QTUNES_COMMON
/*===========================================================================

FUNCTION voc_state_qtunes_init

DESCRIPTION
  If the vocoder is acquired, turn it on and send it to QTUNES_IDLE state.
  If the vocoder is released, turn off the clocks.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS

===========================================================================*/
voc_state_return_type voc_state_qtunes_init (void)
{
  voc_codec_type             pcm_path;
  voc_codec_class_type       codec_type;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  qdsp_cmd_status_type       result;
  qdsp_module_type           module_select;
  voc_status_type            module_select_status;

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif/*AVS_BT_SCO_REWORK*/

  module_select_status = voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (voc_state_data.image_changed == TRUE)
#if defined(FEATURE_ACP)
#error code not present
#endif /* FEATURE_ACP) */
  )
  {
    MSG_HIGH("Vocoder going to RESET",0,0,0);
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* Disable the bluetooth A2DP when DSP is going to sleep */
    if (voc_get_bt_state() != VOC_BT_STATE_DISABLED) {
      (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
    }
#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if ((bt_func_ptr != NULL) && voc_bt_playback_started) {
      MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_STOP);
      voc_bt_playback_started = FALSE;
    }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* FEATURE_SBC_CODEC */

    if ( module_select_status == VOC_DONE_S ) {
      voc_state_data.config = voc_state_data.config_pending;
    }
    voc_state_data.state  = VOC_STATE_RESET;
#ifdef FEATURE_EXTERNAL_SDAC
    voc_state_config_external_sdac(VOC_SAMPLE_RATE_NONE);
#endif
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  }

  if ((voc_state_data.acquire_mask) || (voc_state_data.reset_flag)) {
    /* Indicate first time up */
    voc_state_data.reset_flag = FALSE;

    /* Signal that it is no longer ok to sleep */
#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#ifndef FEATURE_GSM_PLT
#ifdef FEATURE_NEW_SLEEP_API
    sleep_negate_okts(voc_state_sleep_handle);
#else
    SLEEP_NEGATE_OKTS( SLEEP_VOC_OKTS_SIG );
#endif /* FEATURE_NEW_SLEEP_API  */
#endif
#endif /* T_SLEEP || FEATURE_SLEEP_TASK */

    /* Wake up QDSP if coming out of idle */
    if (voc_state_data.module_status != VOC_STATE_MOD_STATUS_ENABLED) {
      voc_state_data.state = VOC_STATE_RESET;
      return (VOC_STATE_CONTINUE_IMMEDIATE);
    }

    if ( voc_state_clocks_on == FALSE ) {
      /* Enable all vocoder and codec clocks */
      voc_state_enable_clocks();
      voc_state_clocks_on = TRUE;
    }

    /* Allow QDSP time to run */
    #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
    clk_busy_wait(200);
   	#else
#error code not present
	#endif

#ifdef MSMAUD_ADIE_CODEC
    /* Wake up the Analog Die if the codec in use is the internal codec. */
    (void) voc_state_adie_wakeup();
#endif /* MSMAUD_ADIE_CODEC */

    /* Get the codec configuration data */
    voc_data_get_codec(&pcm_path);
    voc_data_get_codec_class(&codec_type);

    /* Reset the configuration for this type of codec
    */
    voc_state_set_codec_cal(pcm_path);

    voc_data_get_codec_config(voc_state_data.current_image, pcm_path,
                              &codec_config_ptr);

    /* Disable any registered codec functions */
    voc_disable_codec_func(FALSE);

#ifdef FEATURE_EXTERNAL_SDAC
    if (codec_type == VOC_CODEC_CLASS_EXTERNAL_STEREO) {
      voc_set_clk_ctrl_timer(FALSE, VOC_CLK_CTRL_STEREO_DAC);
      voc_state_config_external_sdac(voc_data_get_sample_rate());
    }
#endif

    voc_state_data.tunes_config_pending = FALSE;

#if defined(FEATURE_QTUNES_AAC)           || \
    defined(FEATURE_QTUNES)               || \
    defined(FEATURE_REAL_AUDIO)           || \
    defined(FEATURE_WMA)                  || \
    defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)

    switch (voc_state_data.current_image) {
#ifdef FEATURE_QTUNES_AAC
#ifdef QDSP_IMAGE_AAC_DEFINED
      case QDSP_IMAGE_AAC:
#endif
#ifdef QDSP_IMAGE_ENH_AAC_PLUS_DEFINED
      case QDSP_IMAGE_ENH_AAC_PLUS:
#endif
#ifdef QDSP_IMAGE_MP4D_AAC_PLUS_DEFINED
      case QDSP_IMAGE_MP4D_AAC_PLUS:
#endif
#ifdef QDSP_IMAGE_MP4D_AAC_NO_VPP_DEFINED
      case QDSP_IMAGE_MP4D_AAC_NO_VPP:
#endif
#ifdef QDSP_IMAGE_MP4D_AAC_DEFINED
      case QDSP_IMAGE_MP4D_AAC:
#endif
#ifdef QDSP_IMAGE_H263D_AAC_DEFINED
      case QDSP_IMAGE_H263D_AAC:
#endif
#ifdef QDSP_IMAGE_H264D_AAC_DEFINED
      case QDSP_IMAGE_H264D_AAC:
#endif
#ifdef QDSP_IMAGE_QTV_AAC_H264_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QTV_AAC_H264_TURBO_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QTV_AAC_DEFINED
      case QDSP_IMAGE_QTV_AAC:
#endif
        qdsp_cmd_set_aac_config(
            (qdsp_cmd_aac_config_type*)&voc_state_data.aac_config);
        break;
#endif /* FEATURE_QTUNES_AAC */

#ifdef QDSP_IMAGE_TUNES_DEFINED
      case QDSP_IMAGE_TUNES:
        /* send DSP configuration information */
        qdsp_cmd_set_mp3_config(
            (qdsp_cmd_mp3_config_type*)&voc_state_data.mp3_config);
        break;
#endif /* QDSP_IMAGE_TUNES_DEFINED */

#if defined(FEATURE_REAL_AUDIO) && defined(QDSP_IMAGE_RA_DEFINED)
      case QDSP_IMAGE_RA:
        qdsp_cmd_set_ra_song_config(
          (qdsp_cmd_ra_dsp_config_type*)&voc_state_data.real_audio_config);
        break;
#endif /* FEATURE_REAL_AUDIO && QDSP_IMAGE_RA_DEFINED */
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_WM_PRO_DEFINED */

#ifdef QDSP_IMAGE_QTV_CONC_DEFINED
      case QDSP_IMAGE_QTV_CONC:
        qdsp_cmd_set_amr_wbp_config(
          (qdsp_cmd_amr_wbp_config_type*)&voc_state_data.amr_wbp_config);
        break;
#endif /* QDSP_IMAGE_QTV_CONC_DEFINED */

      default:
        break;
    }
#endif /* FEATURE_QTUNES_AAC ||FEATURE_QTUNES || FEATURE_REAL_AUDIO ||
          FEATURE_AMR_WBPLUS_AUDIO_DEC                       */

#ifdef FEATURE_SBC_DSP_CODEC
   if (voc_get_bt_state() == VOC_BT_STATE_A2DP_ENABLED)
   {
     /* We need to send the SBC config again to the DSP */
     voc_sbc_reenable();
   }
#endif

    /* Issue codec configuration command */
    result = qdsp_cmd_codec_config(codec_config_ptr,\
             voc_data_get_default_frame_rate(voc_state_data.current_image,\
                                                                  pcm_path));
    if (result != QDSP_CMD_SUCCESS) {
    /* If QDSP failed to reach tunes config state, attempt retry */
    if ( voc_state_data.retry_counter < 100 ) {
      MSG_HIGH( "Codec config retry in tunes Init state", 0,0,0 );
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    }
    else {
      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR( "QDSP failed to reach tunes config state", 0,0,0 );
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

    voc_state_data.codec_pending = FALSE;

    /* Setup audio format pro-processing calibration blocks
    */
    voc_set_audfmt_pp_cal();

    /* Enable registered codec function */
    voc_enable_codec_func();

#ifdef FEATURE_EXTERNAL_SDAC
    if (codec_type != VOC_CODEC_CLASS_EXTERNAL_STEREO) {
      voc_state_config_external_sdac(VOC_SAMPLE_RATE_NONE);
    }
#endif

    /* Transition to the QTUNES CONFIG state */
    MSG_HIGH("Vocoder going to QTUNES CONFIG",0,0,0);
    voc_state_data.state = VOC_STATE_QTUNES_CONFIG;

    /* setting the qdsp idlemetering variable */
    qdsp_cmd_set_idle_metering(voc_data_get_idle_metering());

    return(VOC_STATE_CONTINUE);

  } else { /* Vocoder is not acquired */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* Disable the bluetooth A2DP when DSP is going to sleep */
    if (voc_get_bt_state() != VOC_BT_STATE_DISABLED) {
      (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
    }
#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if ((bt_func_ptr != NULL) && voc_bt_playback_started) {
      MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_STOP);
      voc_bt_playback_started = FALSE;
    }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* FEATURE_SBC_CODEC */
    /* Shut down clocks and codecs */
    voc_state_shutdown();

    /* Stay in SLEEP/INIT until vocoder is acquired */
    return(VOC_STATE_SUCCESS);
  }
} /* voc_state_qtunes_init */

/*===========================================================================

FUNCTION voc_state_qtunes_config

DESCRIPTION
  If the vocoder is in config state, send the codec spec information to
  QDSP. Otherwise keep trying till the threshold is reached, then return
  the error report.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS

===========================================================================*/
voc_state_return_type voc_state_qtunes_config (void) {

  voc_state_return_type     status;

#if defined(QDSP_IMAGE_WM_DEFINED) || \
    defined(QDSP_IMAGE_WM_PRO_DEFINED)

  qdsp_cmd_adec_config_type adec_config;

  status = VOC_STATE_CONTINUE;

  if ((voc_state_data.current_image == QDSP_IMAGE_WM)
#ifdef QDSP_IMAGE_WM_PRO_DEFINED
#error code not present
#endif

    ){

    /*enter this block only if its wm image*/
    if (voc_state_data.current_image == QDSP_IMAGE_WM) {
     if ( qdsp_cmd_get_state() != QDSP_STATE_TUNES_CONFIG ) {
      /* If QDSP failed to reach QTUNES CONFIG state, attempt retry */
      if ( voc_state_data.retry_counter < 100 ) {
        MSG_HIGH( "QTUNES CONFIG state transition retry", 0,0,0 );
        voc_state_data.retry_counter++;
        return(VOC_STATE_CONTINUE);
      } else {
        /* If a retry has already been attempted, transition to ERROR state */
        MSG_ERROR( "QDSP failed to reach QTUNES CONFIG state", 0,0,0 );
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        return(VOC_STATE_CONTINUE);
      }
    }
  }/*extra check for WMA image only..*/

    voc_state_data.retry_counter = 0;

    adec_config.adec_type        = 0;
    adec_config.subcommand       = 0;
    adec_config.command_length   = 0;
    adec_config.version          = voc_state_data.wma_config.version;
    adec_config.channels         = voc_state_data.wma_config.channels;
    adec_config.bytes_per_second = voc_state_data.wma_config.bytes_per_second;
    adec_config.sample_rate      = voc_state_data.wma_config.sample_rate;
    adec_config.encoder_options  = voc_state_data.wma_config.encoder_options;
    adec_config.virtual_pkt_len  = voc_state_data.wma_config.virtual_pkt_len;
    adec_config.format_tag       = voc_state_data.wma_config.format_tag;
    adec_config.bits_per_sample  = voc_state_data.wma_config.bits_per_sample;
    adec_config.channel_mask     = voc_state_data.wma_config.channel_mask;
    adec_config.advanced_encoder_opt2 = voc_state_data.wma_config.advanced_encoder_opt2;
    adec_config.advanced_encoder_opt1 = voc_state_data.wma_config.advanced_encoder_opt1;


    if (qdsp_cmd_adec_config_cmd(&adec_config) != QDSP_CMD_SUCCESS) {
      return(VOC_STATE_CONTINUE);
    }
  } else
#endif /* QDSP_IMAGE_WM_DEFINED || QDSP_IMAGE_WM_PRO_DEFINED */
  {
    status = VOC_STATE_CONTINUE_IMMEDIATE;
  }

  voc_state_data.state = VOC_STATE_QTUNES_ENTRY;
  return(status);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_qtunes_entry

DESCRIPTION
  Wait for the QDSP2 to reach QTUNES state.  When QDSP2 is in QTUNES, inform
  users, and transition the driver to QTUNES state. If the QDSP2 does not
  reach QTUNES within the allowed time, then perform error recovery.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_qtunes_entry (void)
{

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
  voc_pp_cb_type  pp_cb = NULL;
  voc_pp_type_type pp_config;
  qdsp_cmd_status_type status = QDSP_CMD_SUCCESS;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif

  if ( qdsp_cmd_get_state() != QDSP_STATE_TUNES ) {
    /* If QDSP failed to reach QTUNES state, attempt retry */
    if ( voc_state_data.retry_counter < 100 ) {
      MSG_HIGH( "QTUNES state transition retry", 0,0,0 );
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    } else {
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
      qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
      voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR( "QDSP failed to reach QTUNES state", 0,0,0 );
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  voc_state_data.retry_counter = 0;

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
  // Issue command here
  MSG_LOW("voc_state_qtunes_entry - issue dual mono cmd here",0,0,0);
  qdsp_cmd_dual_mono_mode(voc_data_get_dual_mono_mode());
  qdsp_cmd_sbr_mode(voc_data_get_sbr_mode());
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

#if defined(FEATURE_ACP) && defined(FEATURE_QTUNES)
#error code not present
#endif

  /* Configure ADEC interrupts and activate the buffer initialization */
  (void) qdsp_cmd_adec_config(voc_adec_isr);


#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
  pp_cb = voc_data_get_pp_func();
  if (pp_cb  != NULL)
  {
    pp_config = voc_data_get_pp_type();
    switch (pp_config) {
      case VOC_PP_TYPE_GENERIC:
      status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_GENERIC);
        break;
#if defined (FEATURE_SBC_CODEC)
      case VOC_PP_TYPE_SBC:
      status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_SBC);
        break;
#elif defined (FEATURE_SBC_DSP_CODEC)
      case VOC_PP_TYPE_DSP_SBC:
        status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_DSP_SBC);
        break;
#endif
    }
    if (status == QDSP_CMD_SUCCESS) {
    voc_state_pp_status = VOC_PP_STATUS_ENABLED;
  } else {
    /* If callback is null or if qdsp_cmd_register_pp_cb failed */
    voc_state_pp_status = VOC_PP_STATUS_DISABLED;
  }
  }
#endif /*(FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE)*/

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
 #if defined (FEATURE_AVS_AUDIO_DECODER_SCO)
   voc_check_bt(VOC_BT_OP_NORMAL);
    #if defined (FEATURE_AVS_BT_SCO_REWORK)
      if (bt_func_ptr != NULL)
      {
        MSG_MED("Calling BT callback with PLAYBACK A2DP | SCO", 0,0,0);
        bt_func_ptr(VOC_PLAYBACK_A2DP|VOC_PLAYBACK_SCO);
        voc_bt_playback_started = TRUE;
      }
    #endif/*AVS_BT_SCO_REWORK*/
 #else
  voc_check_bt(VOC_BT_OP_A2DP_NORMAL);

#if defined (FEATURE_AVS_BT_SCO_REWORK)
  if (bt_func_ptr != NULL)
  {
    MSG_MED("Calling BT callback with PLAYBACK_A2DP", 0,0,0);
    bt_func_ptr(VOC_PLAYBACK_A2DP);
    voc_bt_playback_started = TRUE;
  }
#endif/*AVS_BT_SCO_REWORK*/
 #endif /*FEATURE_AVS_AUDIO_DECODER_SCO */
#endif /* FEATURE_BT_AG */

  /* Turn analog part of Adie */
  #ifdef MSMAUD_SCMM
#error code not present
  #endif /* MSMAUD_SCMM */

  voc_state_data.state = VOC_STATE_QTUNES;

  MSG_HIGH("Entered QTUNES state",0,0,0);

  /* Inform users of transition to QTUNES state */
  voc_inform(VOC_INFORM_QTUNES);

  /* Setup audio format post-processing blocks
  */
  voc_set_audfmt_pp_config();

  return(VOC_STATE_CONTINUE);
} /* voc_state_qtunes_entry */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_qtunes

DESCRIPTION
  Holding pattern for Qtunes functions.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_qtunes (void)
{
  #if defined(FEATURE_ARM_POST_PROCESSING) || \
      defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
  voc_pp_cb_type  pp_cb = NULL;
  voc_pp_type_type pp_config;
  qdsp_cmd_status_type status = QDSP_CMD_SUCCESS;
  #endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

  qdsp_cmd_status_type     result;
  qdsp_module_type         module_select;
  voc_status_type          module_select_status;

  #ifdef FEATURE_ACP
#error code not present
  #endif /* FEATURE_ACP */

  #if defined(FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL) && \
      defined(FEATURE_AUDFMT_SPECTRUM_ANALYZER)
  uint16 spa_interval = 0;
  #endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

  /* Clear any previous error condition */
  voc_state_clear_fw_err_cnt();

  #if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    #if defined (FEATURE_AVS_AUDIO_DECODER_SCO)
    voc_check_bt(VOC_BT_OP_NORMAL);
    #else
    voc_check_bt(VOC_BT_OP_A2DP_NORMAL);
    #endif /*FEATURE_AVS_AUDIO_DECODER_SCO */
  #endif /* FEATURE_SBC_CODEC */

  #if defined(FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL) && \
      defined(FEATURE_AUDFMT_SPECTRUM_ANALYZER)
  /* Make sure that we are not preempted by SND attempting to set SPA state
     while we are trying to compute module_select */
  TASKLOCK();
    spa_interval = voc_data_get_spectrum_analyzer_interval();
    module_select_status = voc_state_get_high_module(&module_select);
    switch(module_select_status)
    {
      case VOC_CMD_FAILED_S:
        break;
      case VOC_CMD_BUSY_S:
        {
          MSG_HIGH("Module not supported on current image = %d, "
                   "Continuing with Module = %d",
                   voc_state_data.current_image,voc_state_data.module_reqd,0);
          voc_inform(VOC_INFORM_MODULE_UNFETCHED);
        }
        break;
      default:
        ;
    }
  TASKFREE();
  #else
  module_select_status = voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  #endif /*FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL&&FEATURE_AUDFMT_SPECTRUM_ANALYZER*/

  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_ENABLED) &&
      (voc_state_data.module_reqd != module_select)) {
    MSG_HIGH("qdsp_enable(%d,%d)",
             voc_state_data.current_qdsps_app, module_select, 0);
    qdsp_enable( voc_state_data.current_qdsps_app, module_select );
    return (VOC_STATE_CONTINUE);
  }

  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.acquire_mask == 0) ||
      (voc_state_data.tunes_config_pending == TRUE) ||
      (voc_state_data.codec_pending == TRUE) ||
      (voc_state_data.state == VOC_STATE_QTUNES_DISABLE_PENDING))
  {

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

    /* Disable audio decoder functions */
    (void) qdsp_cmd_adec_config(NULL);

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
    qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
    voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

    voc_state_data.state = VOC_STATE_QTUNES_DISABLE_PENDING;
    voc_state_adie_stall();
    result = qdsp_cmd_sleep(voc_state_aux_pcm_clk_query());
    if(result == QDSP_CMD_SUCCESS) {
      voc_state_data.state  = VOC_STATE_SLEEP;
      voc_state_data.retry_counter = 0;
    } else {
      MSG_HIGH("Sleep command failed",0,0,0);
      voc_state_data.retry_counter++;

      if ( voc_state_data.retry_counter == 100) {
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        MSG_ERROR( "Failed to issue SLEEP command", 0,0,0 );
      }
    }
    return(VOC_STATE_CONTINUE);
  }
  else {
    /* Configure ADEC interrupts and activate the buffer initialization */
    qdsp_cmd_set_adec_int_cb(voc_adec_isr);
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
    qdsp_cmd_set_codec_update_int_cb(voc_codec_update_isr);
#endif
    if(voc_state_data.config != voc_state_data.config_pending
      && module_select_status == VOC_DONE_S) {
      voc_state_data.config = voc_state_data.config_pending;
      voc_inform(VOC_INFORM_QTUNES);
    }
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
    pp_cb = voc_data_get_pp_func();
    if (pp_cb != NULL && voc_state_pp_status == VOC_PP_STATUS_DISABLED)
    {
      pp_config = voc_data_get_pp_type();
      if (pp_cb  != NULL)
      {
        pp_config = voc_data_get_pp_type();
        switch (pp_config) {
          case VOC_PP_TYPE_GENERIC:
            status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_GENERIC);
            break;
#if defined (FEATURE_SBC_CODEC)
          case VOC_PP_TYPE_SBC:
            status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_SBC);
            break;
#elif defined (FEATURE_SBC_DSP_CODEC)
          case VOC_PP_TYPE_DSP_SBC:
            status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_DSP_SBC);
            break;
#endif
        }
        if (status == QDSP_CMD_SUCCESS) {
          voc_state_pp_status = VOC_PP_STATUS_ENABLED;
        } else {
          /* If callback is null or if qdsp_cmd_register_pp_cb failed */
          voc_state_pp_status = VOC_PP_STATUS_DISABLED;
        }
      }
    }
    else if (pp_cb == NULL && voc_state_pp_status == VOC_PP_STATUS_ENABLED)
    {
      voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
      qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
    }
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

#ifdef QDSP_codecCmdProcessed
    {
      /* The interface variable codeccmdprocessed is defined by the DSP
      and is used to let Voc know that the codec config cmd is processed and
      the unnecessary clock can be disabled.This variable may not be defined
      in all the targets so care should be taken before using the variable */

      uint16 dsp_cmdProcessed = qdsp_read(QDSP_codecCmdProcessed);
      if( ( voc_data_get_eclock_switched() ) && !( dsp_cmdProcessed ) )
      {
        voc_state_disable_eclock();
        voc_data_set_eclock_switched(FALSE);
      }
    }
#endif /*QDSP_codecCmdProcessed */

  }

  #if defined(FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL) && \
      defined(FEATURE_AUDFMT_SPECTRUM_ANALYZER)
  qdsp_cmd_spectrum_analyzer_config(spa_interval);
  #endif /*FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL&&FEATURE_AUDFMT_SPECTRUM_ANALYZER*/

  return(VOC_STATE_SUCCESS);
} /* voc_state_qtunes */

#endif /* FEATURE_QTUNES_COMMON */


/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_cam_audio_init_entry

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_cam_audio_init_entry(void)
{
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
  qdsp_module_type           module_select;
  voc_status_type            module_select_status;

/**TBD Featurization should be removed consult Anilb and azam */
#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif/*AVS_BT_SCO_REWORK*/


  module_select_status = voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (voc_state_data.image_changed == TRUE)
#if defined(FEATURE_ACP)
#error code not present
#endif /* FEATURE_ACP) */
  )
{
    MSG_HIGH("Vocoder going to RESET",0,0,0);
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* Disable the bluetooth A2DP when DSP is going to sleep */
    if (voc_get_bt_state() != VOC_BT_STATE_DISABLED) {
      (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
    }
#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if ((bt_func_ptr != NULL) && voc_bt_playback_started) {
      MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_STOP);
      voc_bt_playback_started = FALSE;
    }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* FEATURE_SBC_CODEC */

    if ( module_select_status == VOC_DONE_S ) {
      voc_state_data.config = voc_state_data.config_pending;
    }
    voc_state_data.state  = VOC_STATE_RESET;
#ifdef FEATURE_EXTERNAL_SDAC
    voc_state_config_external_sdac(VOC_SAMPLE_RATE_NONE);
#endif
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  }

if ( (voc_state_data.adpcm_mode_enabled== TRUE)
#ifdef FEATURE_VOC_ADPCM
     && (voc_state_data.adpcm_voc_config == TRUE)
#endif
     )
{
   MSG_HIGH("Set the Decoder to WAV",0,0,0);
  qdsp_cmd_set_decoder_in_use(QDSP_CMD_DEC_WAVE);
  voc_state_data.state = VOC_STATE_CAM_AUDIO_INIT;
}
else if (voc_state_data.config_pending == VOC_CAP_QTUNES){
    MSG_HIGH("Set the Decoder to QTUNES",0,0,0);
  qdsp_cmd_set_decoder_in_use(QDSP_CMD_DEC_MP3);
  voc_state_data.config = voc_state_data.config_pending;
  voc_state_data.state = VOC_STATE_CAM_AUDIO_INIT;
}
else if ( (voc_state_data.acquire_mask) & (1<< VOC_ACQ_SND) ){
   voc_state_data.state = VOC_STATE_CAM_AUDIO_INIT;
}
else{
    MSG_HIGH("Remaining in INIT ENTRY",0,0,0);
    return (VOC_STATE_SUCCESS);
}

return (VOC_STATE_CONTINUE);

#else
MSG_ERROR(" FEATURE is not supported",0,0,0);
return(VOC_STATE_SUCCESS);
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */


}
/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_cam_audio_init

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_cam_audio_init(void)
{
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
  voc_codec_type             pcm_path;
  voc_codec_class_type       codec_type;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  qdsp_cmd_status_type       result;
  qdsp_module_type           module_select;
  voc_status_type            module_select_status;

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif/*AVS_BT_SCO_REWORK*/


  /* If QDSP failed to reach INIT state, attempt retry */
  if ( qdsp_cmd_get_state() != QDSP_STATE_INIT ) {
    if ( voc_state_data.retry_counter < 100 ) {
      MSG_HIGH( "INIT state transition retry", 0,0,0 );
      voc_state_data.retry_counter++;
      return( VOC_STATE_CONTINUE );
    } else {
      /* Transition to ERROR state if retry failed */
      MSG_ERROR( "QDSP failed to reach INIT state", 0,0,0 );
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  module_select_status = voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (voc_state_data.image_changed == TRUE)
#if defined(FEATURE_ACP)
#error code not present
#endif /* FEATURE_ACP) */
  )
  {
    MSG_HIGH("Vocoder going to RESET",0,0,0);
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* Disable the bluetooth A2DP when DSP is going to sleep */
    if (voc_get_bt_state() != VOC_BT_STATE_DISABLED) {
      (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
    }
#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if ((bt_func_ptr != NULL) && voc_bt_playback_started) {
      MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_STOP);
      voc_bt_playback_started = FALSE;
    }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* FEATURE_SBC_CODEC */

    if ( module_select_status == VOC_DONE_S ) {
      voc_state_data.config = voc_state_data.config_pending;
    }
    voc_state_data.state  = VOC_STATE_RESET;
#ifdef FEATURE_EXTERNAL_SDAC
    voc_state_config_external_sdac(VOC_SAMPLE_RATE_NONE);
#endif
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  }

  if ((voc_state_data.acquire_mask) || (voc_state_data.reset_flag)) {
    /* Indicate first time up */
    voc_state_data.reset_flag = FALSE;

    /* Signal that it is no longer ok to sleep */
#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#ifndef FEATURE_GSM_PLT
#ifdef FEATURE_NEW_SLEEP_API
    sleep_negate_okts(voc_state_sleep_handle);
#else
    SLEEP_NEGATE_OKTS( SLEEP_VOC_OKTS_SIG );
#endif /* FEATURE_NEW_SLEEP_API  */
#endif
#endif /* T_SLEEP || FEATURE_SLEEP_TASK */

    /* Wake up QDSP if coming out of idle */
    if (voc_state_data.module_status != VOC_STATE_MOD_STATUS_ENABLED) {
      voc_state_data.state = VOC_STATE_RESET;
      return (VOC_STATE_CONTINUE_IMMEDIATE);
    }

    if ( voc_state_clocks_on == FALSE ) {
      /* Enable all vocoder and codec clocks */
      voc_state_enable_clocks();
      voc_state_clocks_on = TRUE;
    }

    /* Allow QDSP time to run */
    #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
    clk_busy_wait(200);
    #else
#error code not present
    #endif

#ifdef MSMAUD_ADIE_CODEC
    /* Wake up the Analog Die if the codec in use is the internal codec. */
    (void) voc_state_adie_wakeup();
#endif /* MSMAUD_ADIE_CODEC */

    /* Get the codec configuration data */
    voc_data_get_codec(&pcm_path);
    voc_data_get_codec_class(&codec_type);

    /* Reset the configuration for this type of codec
    */
    voc_state_set_codec_cal(pcm_path);

    voc_data_get_codec_config(voc_state_data.current_image, pcm_path,
                              &codec_config_ptr);

    /* Disable any registered codec functions */
    voc_disable_codec_func(FALSE);

#ifdef FEATURE_EXTERNAL_SDAC
    if (codec_type == VOC_CODEC_CLASS_EXTERNAL_STEREO) {
      voc_set_clk_ctrl_timer(FALSE, VOC_CLK_CTRL_STEREO_DAC);
      voc_state_config_external_sdac(voc_data_get_sample_rate());
    }
#endif

if ( voc_state_data.adpcm_mode_enabled== TRUE)
{
  qdsp_cmd_set_decoder_in_use(QDSP_CMD_DEC_WAVE);
/** ADPCM specific code */
   /* Set ADEC mode */
#ifdef FEATURE_QSYNTH_ADPCM
    voc_state_set_qsynth_adec_mode(voc_state_data.adec_mode_pending);
    voc_state_data.adec_mode = voc_state_data.adec_mode_pending;
#else
    voc_state_set_qsynth_adec_mode(VOC_QSYNTH_ADEC_MODE_QCP);
#endif /* FEATURE_QSYNTH_ADPCM */
}
else if (voc_state_data.config  == VOC_CAP_QTUNES)
{
  qdsp_cmd_set_decoder_in_use(QDSP_CMD_DEC_MP3);
    /** Mp3 Specific Code */
        voc_state_data.tunes_config_pending = FALSE;
#if  defined(FEATURE_QTUNES)
        switch (voc_state_data.current_image) {
#ifdef QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED
          case QDSP_IMAGE_CAM_AUDIO_CONC:
            /* send DSP configuration information */
            qdsp_cmd_set_mp3_config(
                (qdsp_cmd_mp3_config_type*)&voc_state_data.mp3_config);
            break;
#endif /* QDSP_IMAGE_CAM_AUDIO_CONC_DEFINED */
          default:
            break;
        }
#endif /*FEATURE_QTUNES */

}

#ifdef FEATURE_SBC_DSP_CODEC
   if (voc_get_bt_state() == VOC_BT_STATE_A2DP_ENABLED)
   {
     /* We need to send the SBC config again to the DSP */
     voc_sbc_reenable();
   }
#endif

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

    /* Issue codec configuration command */
    result = qdsp_cmd_codec_config(codec_config_ptr,\
             voc_data_get_default_frame_rate(voc_state_data.current_image,\
                                                                  pcm_path));
    if (result != QDSP_CMD_SUCCESS) {
      MSG_ERROR("Codec Config command failed",0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }

    voc_state_data.codec_pending = FALSE;

    /* Setup audio format pro-processing calibration blocks
    */
    voc_set_audfmt_pp_cal();

    /* Enable registered codec function */
    voc_enable_codec_func();

#ifdef FEATURE_EXTERNAL_SDAC
    if (codec_type != VOC_CODEC_CLASS_EXTERNAL_STEREO) {
      voc_state_config_external_sdac(VOC_SAMPLE_RATE_NONE);
    }
#endif

    /* Transition to the CAM_AUDIO_CONFIG  state */
    MSG_HIGH("Vocoder going to CAM AUDIO CONFIG",0,0,0);
    voc_state_data.state = VOC_STATE_CAM_AUDIO_CONFIG;
    return(VOC_STATE_CONTINUE);

  } else { /* Vocoder is not acquired */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* Disable the bluetooth A2DP when DSP is going to sleep */
    if (voc_get_bt_state() != VOC_BT_STATE_DISABLED) {
      (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
    }
#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if ((bt_func_ptr != NULL) && voc_bt_playback_started) {
      MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_STOP);
      voc_bt_playback_started = FALSE;
    }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* FEATURE_SBC_CODEC */
    /* Shut down clocks and codecs */
    voc_state_shutdown();

    /* Stay in SLEEP/INIT until vocoder is acquired */
    return(VOC_STATE_SUCCESS);
  }

#else
MSG_ERROR(" FEATURE is not supported",0,0,0);
return(VOC_STATE_SUCCESS);
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
}


/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_cam_audio_config

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_cam_audio_config(void)
{
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
 voc_state_return_type     status;
#ifdef FEATURE_QSYNTH_ADPCM

  qdsp_cmd_qsynth_adpcm_cmd_type     adpcm_cmd;
  qdsp_cmd_adec_start_mode_enum_type qdsp_cmd_mode;
  boolean                            valid_synth = FALSE;
#endif /* FEATURE_QSYNTH_ADPCM */

 /* There is no need for any audio config for mp3 */
 status = VOC_STATE_CONTINUE_IMMEDIATE;

#ifdef FEATURE_QSYNTH_ADPCM

  if ((qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_WAVE)&&
    (voc_state_data.adpcm_start_pending == TRUE)){

    /* Send the qdsp_cmd_qsynth_adpcm_start */
  if (voc_state_data.adpcm_mode ==  VOC_ADPCM_ADPCM_MODE) {
      valid_synth = (VOC_QSYNTH_ADPCM_VALID_CHANNELS(voc_state_data.adpcm_num_channels) &&
                     VOC_QSYNTH_ADPCM_VALID_SAMPLE_RATE(voc_state_data.adpcm_sampling_rate) &&
                     VOC_QSYNTH_ADPCM_VALID_BITS_PER_SAMPLE(voc_state_data.bits_per_sample));

      if(voc_state_data.start_mode == VOC_ADEC_START_MODE_DELAYED) {
        qdsp_cmd_mode = QDSP_CMD_ADEC_START_MODE_DELAYED;
      } else {
        qdsp_cmd_mode = QDSP_CMD_ADEC_START_MODE_NORMAL;
      }

      adpcm_cmd.block_length    = voc_state_data.adpcm_block;
      adpcm_cmd.sampling_rate   = voc_state_data.adpcm_sampling_rate;
      adpcm_cmd.num_channels    = voc_state_data.adpcm_num_channels;
      adpcm_cmd.bits_per_sample = voc_state_data.bits_per_sample;
     if (valid_synth ){
          if (qdsp_cmd_qsynth_adpcm_start(0, &adpcm_cmd,
                                          (qdsp_cmd_adpcm_mode_type) voc_state_data.adpcm_mode,
                                          qdsp_cmd_mode) ==
              QDSP_CMD_SUCCESS) {
              voc_state_data.adpcm_start_pending = FALSE;
          }
     }
  }

 }
#endif

  voc_state_data.state = VOC_STATE_CAM_AUDIO_ENTRY;
  return(status);

#else
MSG_ERROR(" FEATURE is not supported",0,0,0);
return(VOC_STATE_SUCCESS);
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_cam_audio_entry

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_cam_audio_entry(void)
{
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
  voc_pp_cb_type  pp_cb = NULL;
  voc_pp_type_type pp_config;
  qdsp_cmd_status_type status = QDSP_CMD_SUCCESS;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif

  if ( qdsp_cmd_get_state() != QDSP_STATE_PLAY) {
    /* If QDSP failed to reach QTUNES state, attempt retry */
    if ( voc_state_data.retry_counter < 100 ) {
      MSG_HIGH( "CAM AUDIO ACTIVE state transition retry", 0,0,0 );
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    } else {
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
      qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
      voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR( "QDSP failed to reach CAM AUDIO ACTIVE state", 0,0,0 );
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  voc_state_data.retry_counter = 0;

if (voc_state_data.adpcm_mode_enabled== TRUE)
{
  qdsp_cmd_set_decoder_in_use (QDSP_CMD_DEC_WAVE);
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
        defined(FEATURE_QSYNTH_ADPCM)
#ifdef FEATURE_GASYNTH_INTERFACE
      (void) qdsp_cmd_set_adec_timer_cb(voc_set_adec_cmd_retry_timer);
#endif /* FEATURE_GASYNTH_INTERFACE */
      (void) qdsp_cmd_adec_config(voc_adec_isr);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

}
else if ( voc_state_data.config== VOC_CAP_QTUNES )
{

  qdsp_cmd_set_decoder_in_use (QDSP_CMD_DEC_MP3);
    /** Check if ISDBT is meant for MP3 format */
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
      // Issue command here
      MSG_LOW("voc_state_qtunes_entry - issue dual mono cmd here",0,0,0);
      qdsp_cmd_dual_mono_mode(voc_data_get_dual_mono_mode());
      qdsp_cmd_sbr_mode(voc_data_get_sbr_mode());
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

#if defined(FEATURE_ACP) && defined(FEATURE_QTUNES)
#error code not present
#endif
      /* Configure ADEC interrupts and activate the buffer initialization */
      (void) qdsp_cmd_adec_config(voc_adec_isr);
}

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
  pp_cb = voc_data_get_pp_func();
  if (pp_cb  != NULL)
  {
    pp_config = voc_data_get_pp_type();
    switch (pp_config) {
      case VOC_PP_TYPE_GENERIC:
      status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_GENERIC);
        break;
#if defined (FEATURE_SBC_CODEC)
      case VOC_PP_TYPE_SBC:
      status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_SBC);
        break;
#elif defined (FEATURE_SBC_DSP_CODEC)
      case VOC_PP_TYPE_DSP_SBC:
        status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_DSP_SBC);
        break;
#endif
    }
    if (status == QDSP_CMD_SUCCESS) {
    voc_state_pp_status = VOC_PP_STATUS_ENABLED;
  } else {
    /* If callback is null or if qdsp_cmd_register_pp_cb failed */
    voc_state_pp_status = VOC_PP_STATUS_DISABLED;
  }
  }
#endif /*(FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE)*/

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
 #if defined (FEATURE_AVS_AUDIO_DECODER_SCO)
   voc_check_bt(VOC_BT_OP_NORMAL);
    #if defined (FEATURE_AVS_BT_SCO_REWORK)
      if (bt_func_ptr != NULL)
      {
        MSG_MED("Calling BT callback with PLAYBACK A2DP | SCO", 0,0,0);
        bt_func_ptr(VOC_PLAYBACK_A2DP|VOC_PLAYBACK_SCO);
        voc_bt_playback_started = TRUE;
      }
    #endif/*AVS_BT_SCO_REWORK*/
 #else
  voc_check_bt(VOC_BT_OP_A2DP_NORMAL);

#if defined (FEATURE_AVS_BT_SCO_REWORK)
  if (bt_func_ptr != NULL)
  {
    MSG_MED("Calling BT callback with PLAYBACK_A2DP", 0,0,0);
    bt_func_ptr(VOC_PLAYBACK_A2DP);
    voc_bt_playback_started = TRUE;
  }
#endif/*AVS_BT_SCO_REWORK*/
 #endif /*FEATURE_AVS_AUDIO_DECODER_SCO */
#endif /* FEATURE_BT_AG */

  /* Turn analog part of Adie */
  #ifdef MSMAUD_SCMM
#error code not present
  #endif /* MSMAUD_SCMM */

  voc_state_data.state = VOC_STATE_CAM_AUDIO_ACTIVE;

  MSG_HIGH("Entered CAM AUDIO ACTIVE  state",0,0,0);

  /* Inform users of transition to QTUNES state */
  voc_inform(VOC_INFORM_CAM_AUDIO_ACTIVE);

  /* Setup audio format post-processing blocks
  */
  voc_set_audfmt_pp_config();

  return(VOC_STATE_CONTINUE);

#else
MSG_ERROR(" FEATURE is not supported",0,0,0);
return(VOC_STATE_SUCCESS);
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_cam_audio_active

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_cam_audio_active(void)
{
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY

#if defined(FEATURE_ARM_POST_PROCESSING) || \
      defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
  voc_pp_cb_type  pp_cb = NULL;
  voc_pp_type_type pp_config;
  qdsp_cmd_status_type status = QDSP_CMD_SUCCESS;
  #endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

  qdsp_cmd_status_type     result;
  qdsp_module_type         module_select;
  voc_status_type          module_select_status;

  #if defined(FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL) && \
      defined(FEATURE_AUDFMT_SPECTRUM_ANALYZER)
  uint16 spa_interval = 0;
  #endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif


  /* Clear any previous error condition */
  voc_state_clear_fw_err_cnt();

  #if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    #if defined (FEATURE_AVS_AUDIO_DECODER_SCO)
    voc_check_bt(VOC_BT_OP_NORMAL);
    #else
    voc_check_bt(VOC_BT_OP_A2DP_NORMAL);
    #endif /*FEATURE_AVS_AUDIO_DECODER_SCO */
  #endif /* FEATURE_SBC_CODEC */

  #if defined(FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL) && \
      defined(FEATURE_AUDFMT_SPECTRUM_ANALYZER)
  /* Make sure that we are not preempted by SND attempting to set SPA state
     while we are trying to compute module_select */
  TASKLOCK();
    spa_interval = voc_data_get_spectrum_analyzer_interval();
    module_select_status = voc_state_get_high_module(&module_select);
    switch(module_select_status)
    {
      case VOC_CMD_FAILED_S:
        break;
      case VOC_CMD_BUSY_S:
        {
          MSG_HIGH("Module not supported on current image = %d, "
                   "Continuing with Module = %d",
                   voc_state_data.current_image,voc_state_data.module_reqd,0);
          voc_inform(VOC_INFORM_MODULE_UNFETCHED);
        }
        break;
      default:
        ;
    }
  TASKFREE();
  #else
  module_select_status = voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  #endif /*FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL&&FEATURE_AUDFMT_SPECTRUM_ANALYZER*/

  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_ENABLED) &&
      (voc_state_data.module_reqd != module_select)) {
    MSG_HIGH("qdsp_enable(%d,%d)",
             voc_state_data.current_qdsps_app, module_select, 0);
    qdsp_enable( voc_state_data.current_qdsps_app, module_select );
    return (VOC_STATE_CONTINUE);
  }

if (qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_WAVE)
{
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
#if defined(FEATURE_QSYNTH_ADPCM) && !(defined(FEATURE_GASYNTH_INTERFACE))
     (voc_state_data.adec_mode != voc_state_data.adec_mode_pending) ||
#endif /* FEATURE_QSYNTH_ADPCM */
     (voc_state_data.qsynth_vol_mode!=voc_state_data.qsynth_vol_mode_pending) ||
     (voc_state_data.acquire_mask == 0) ||
     (voc_state_data.codec_pending == TRUE) )
  {

  MSG_HIGH("In ADPCM mode issuing sleep in cam_audio_active",0,0,0);

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
    /* Disable the bluetooth AG when DSP is going to sleep */
    if ((voc_state_data.acquire_mask == 0) &&
        (voc_get_bt_state() != VOC_BT_STATE_DISABLED)) {
      (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
    }
#ifdef FEATURE_SBC_DSP_CODEC
    else if ((voc_state_data.acquire_mask != 0) &&
             (voc_get_bt_mode() == VOC_BT_MODE_A2DP) &&
             (voc_get_bt_state() == VOC_BT_STATE_A2DP_ENABLED))
    {
      voc_sbc_disable();
    }
#endif
#endif /* FEATURE_BT_AG */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
#ifdef FEATURE_GASYNTH_INTERFACE
    (void) qdsp_cmd_set_adec_timer_cb(NULL);
    (void) voc_set_adec_cmd_retry_timer(FALSE);
#endif /* FEATURE_GASYNTH_INTERFACE */
    /* Disable audio decoder functions */
    (void) qdsp_cmd_adec_config(NULL);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
    qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
    voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
#endif /* FEATURE_ARM_POST_PROCESSING  || FEATURE_AVS_WB_PCM_RX_INTERFACE */

    qdsp_cmd_qsynth_cmd_dereg();
    voc_state_adie_stall();
    result = qdsp_cmd_sleep(voc_state_aux_pcm_clk_query());
    if(result == QDSP_CMD_SUCCESS) {
#ifdef FEATURE_GASYNTH_INTERFACE
    (void) qdsp_cmd_reset_adec_cmd_buf();
#endif /* FEATURE_GASYNTH_INTERFACE */
      voc_state_data.state  = VOC_STATE_SLEEP;
      voc_state_data.retry_counter = 0;
    } else {
      MSG_HIGH("Sleep command failed",0,0,0);
      voc_state_data.retry_counter++;

      if (voc_state_data.retry_counter == 100) {
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        MSG_ERROR( "Failed to issue SLEEP command", 0,0,0 );
      }
    }
#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)

#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if ((bt_func_ptr != NULL) && voc_bt_playback_started)
    {
      MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_STOP);
      voc_bt_playback_started = FALSE;
    }
#endif /*AVS_BT_SCO_REWORK*/
#endif

    return(VOC_STATE_CONTINUE);

  }

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
    voc_check_bt(VOC_BT_OP_NORMAL);
#endif

  return(VOC_STATE_SUCCESS);

}
else if(qdsp_cmd_get_decoder_in_use()==QDSP_CMD_DEC_MP3){
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.acquire_mask == 0) ||
      (voc_state_data.tunes_config_pending == TRUE) ||
      (voc_state_data.codec_pending == TRUE) )
  {

    /* Disable audio decoder functions */
    (void) qdsp_cmd_adec_config(NULL);

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
    qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
    voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

    voc_state_adie_stall();
    result = qdsp_cmd_sleep(voc_state_aux_pcm_clk_query());
    if(result == QDSP_CMD_SUCCESS) {
      voc_state_data.state  = VOC_STATE_SLEEP;
      voc_state_data.retry_counter = 0;
    } else {
      MSG_HIGH("Sleep command failed",0,0,0);
      voc_state_data.retry_counter++;

      if ( voc_state_data.retry_counter == 100) {
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        MSG_ERROR( "Failed to issue SLEEP command", 0,0,0 );
      }
    }
    return(VOC_STATE_CONTINUE);
  }
  else {
    /* Configure ADEC interrupts and activate the buffer initialization */
    qdsp_cmd_set_adec_int_cb(voc_adec_isr);
    if(voc_state_data.config != voc_state_data.config_pending
      && module_select_status == VOC_DONE_S) {
      voc_state_data.config = voc_state_data.config_pending;
      voc_inform(VOC_INFORM_CAM_AUDIO_ACTIVE);
    }
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
    pp_cb = voc_data_get_pp_func();
    if (pp_cb != NULL && voc_state_pp_status == VOC_PP_STATUS_DISABLED)
    {
      pp_config = voc_data_get_pp_type();
      if (pp_cb  != NULL)
      {
        pp_config = voc_data_get_pp_type();
        switch (pp_config) {
          case VOC_PP_TYPE_GENERIC:
            status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_GENERIC);
            break;
#if defined (FEATURE_SBC_CODEC)
          case VOC_PP_TYPE_SBC:
            status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_SBC);
            break;
#elif defined (FEATURE_SBC_DSP_CODEC)
          case VOC_PP_TYPE_DSP_SBC:
            status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_DSP_SBC);
            break;
#endif
        }
        if (status == QDSP_CMD_SUCCESS) {
          voc_state_pp_status = VOC_PP_STATUS_ENABLED;
        } else {
          /* If callback is null or if qdsp_cmd_register_pp_cb failed */
          voc_state_pp_status = VOC_PP_STATUS_DISABLED;
        }
      }
    }
    else if (pp_cb == NULL && voc_state_pp_status == VOC_PP_STATUS_ENABLED)
    {
      voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
      qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
    }
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */
  }

  #if defined(FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL) && \
      defined(FEATURE_AUDFMT_SPECTRUM_ANALYZER)
  qdsp_cmd_spectrum_analyzer_config(spa_interval);
  #endif /*FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL&&FEATURE_AUDFMT_SPECTRUM_ANALYZER*/

  return(VOC_STATE_SUCCESS);

}else
if( (voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING )||
    ((voc_state_data.acquire_mask & (1<<VOC_ACQ_SND)) == 0) ||
    (((voc_state_data.config == VOC_CAP_QTUNES)
#if defined ( FEATURE_VOC_ADPCM)  && defined (FEATURE_WAVE_PB)
   ||(voc_state_data.adpcm_mode_enabled ==TRUE )
#endif
     )
     && (qdsp_cmd_get_decoder_in_use() == QDSP_CMD_DEC_INVALID))) {
     /** Issue sleep to go to INIT ENTRY */
    result = qdsp_cmd_sleep(voc_state_aux_pcm_clk_query());
    if(result == QDSP_CMD_SUCCESS) {
      voc_state_data.state  = VOC_STATE_SLEEP;
      voc_state_data.retry_counter = 0;
    } else {
      MSG_HIGH("Sleep command failed",0,0,0);
      voc_state_data.retry_counter++;

      if ( voc_state_data.retry_counter == 100) {
        voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
        MSG_ERROR( "Failed to issue SLEEP command", 0,0,0 );
      }
    }
   return(VOC_STATE_CONTINUE);
}
return(VOC_STATE_SUCCESS);
#else
MSG_ERROR(" FEATURE is not supported",0,0,0);
return(VOC_STATE_SUCCESS);
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
}


/* <EJECT> */
#ifdef FEATURE_GRAPH_AUDIO
/*===========================================================================

FUNCTION voc_state_change_voc_selection

DESCRIPTION
  This function checks state parameters to determine if vocoder mode
  selection has changed.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Vocoder mode selection has changed.
  FALSE - Vocoder mode selection has not changed.

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_state_change_voc_selection() {
  boolean ret_val = FALSE;

  if(voc_state_data.audio_mode != VOC_STATE_AUDIO_MODE_DISABLED) {
    if(voc_state_data.config_mask != 0) {
      if(((voc_state_data.config_pending == VOC_CAP_IS733) &&
          (voc_state_data.audio_mode != VOC_STATE_AUDIO_MODE_13K)) ||
         ((voc_state_data.config_pending == VOC_CAP_IS127) &&
          (voc_state_data.audio_mode != VOC_STATE_AUDIO_MODE_EVRC))
#ifdef MSMAUD_VOC_AMR_COMMON
         || ((voc_state_data.config_pending == VOC_CAP_AMR) &&
             (voc_state_data.audio_mode != VOC_STATE_AUDIO_MODE_AMR))
#endif /* MSMAUD_VOC_AMR_COMMON */
#ifdef FEATURE_MM_REC
         || (voc_state_data.mm_rec_config_pending == TRUE)
#ifdef FEATURE_AAC_REC
         || ((voc_state_data.config_pending == VOC_CAP_AAC_REC) &&
             (voc_state_data.audio_mode != VOC_STATE_AUDIO_MODE_AAC))
#endif /* FEATURE_AAC_REC */
#endif /* FEATURE_MM_REC  */
        ) {
        ret_val = TRUE;
      }
    }
  }
  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_graph_init

DESCRIPTION
  If the vocoder is acquired, turn it on and send it to graph idle state.
  If the vocoder is released, turn off the clocks.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS

===========================================================================*/
voc_state_return_type voc_state_graph_init (void)
{
  voc_codec_type             pcm_path;
  voc_codec_class_type       codec_type;
  qdsp_cmd_codec_config_type *codec_config_ptr;
  qdsp_cmd_status_type       result;
  qdsp_module_type           module_select;
  voc_status_type            module_select_status;
  voc_state_audio_mode_type  audio_mode;
  uint16                     voc_select_cmd=  0;

/* A2DP support for for NB-AMR */
#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif/*AVS_BT_SCO_REWORK*/

  module_select_status = voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (voc_state_data.image_changed == TRUE))
  {
    MSG_HIGH("Vocoder going to RESET",0,0,0);

/* A2DP support for for NB-AMR */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  /* Disable the bluetooth A2DP when DSP is going to sleep */
  if (voc_get_bt_state() != VOC_BT_STATE_DISABLED) {
    (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
  }
#if defined (FEATURE_AVS_BT_SCO_REWORK)
  if ((bt_func_ptr != NULL) && voc_bt_playback_started) {
    MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
    bt_func_ptr(VOC_PLAYBACK_STOP);
    voc_bt_playback_started = FALSE;
  }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* FEATURE_SBC_CODEC */

    if ( module_select_status == VOC_DONE_S ) {
      voc_state_data.config = voc_state_data.config_pending;
    }
    voc_state_data.state  = VOC_STATE_RESET;

#ifdef FEATURE_EXTERNAL_SADC
    voc_state_config_external_sadc(SADC_OFF);
#endif /* FEATURE_EXTERNAL_SADC */

    return (VOC_STATE_CONTINUE_IMMEDIATE);
  }
  /* This check is for assigning the default value for aac_config */
  if(voc_state_data.config == VOC_CAP_NONE ) {
#ifdef FEATURE_AAC_REC
    voc_state_data.aac_config.channel_configuration = 0;
#endif /* FEATURE_AAC_REC */
  }
  if ((voc_state_data.acquire_mask) || (voc_state_data.reset_flag)) {
    /* Indicate first time up */
    voc_state_data.reset_flag = FALSE;

    /* Signal that it is no longer ok to sleep */
#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#ifdef FEATURE_NEW_SLEEP_API
    sleep_negate_okts(voc_state_sleep_handle);
#else
    SLEEP_NEGATE_OKTS( SLEEP_VOC_OKTS_SIG );
#endif /* FEATURE_NEW_SLEEP_API  */
#endif /* T_SLEEP || FEATURE_SLEEP_TASK */

    /* Wake up QDSP if coming out of idle */
    if (voc_state_data.module_status != VOC_STATE_MOD_STATUS_ENABLED) {
      voc_state_data.state = VOC_STATE_RESET;
      return (VOC_STATE_CONTINUE_IMMEDIATE);
    }

    /* Check if audio mode selection needs to be done */
    if(voc_state_data.audio_mode != VOC_STATE_AUDIO_MODE_DISABLED) {
      if(voc_state_data.config_mask != 0) {
#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
        if (voc_state_data.current_image == QDSP_IMAGE_VFE_BAYER) {
          voc_state_data.audio_mode = VOC_STATE_AUDIO_MODE_DISABLED;
          audio_mode                = VOC_STATE_AUDIO_MODE_NONE;
        }
        else
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED */
#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
        if (voc_state_data.current_image == QDSP_IMAGE_VFE_YCBCR) {
          voc_state_data.audio_mode = VOC_STATE_AUDIO_MODE_DISABLED;
          audio_mode                = VOC_STATE_AUDIO_MODE_NONE;
        }
        else
#endif /* QDSP_IMAGE_VFE_YCBCR_DEFINED */
        {
          switch(voc_state_data.config_pending) {
            case VOC_CAP_IS733:
              audio_mode     = VOC_STATE_AUDIO_MODE_13K;
              voc_select_cmd = QDSP_CMD_VOC_SELECT_13K;
              break;

            case VOC_CAP_IS127:
              audio_mode     = VOC_STATE_AUDIO_MODE_EVRC;
              voc_select_cmd = QDSP_CMD_VOC_SELECT_EVRC;
              break;

#ifdef MSMAUD_VOC_AMR_COMMON
            case VOC_CAP_AMR:
              audio_mode     = VOC_STATE_AUDIO_MODE_AMR;
              voc_select_cmd = QDSP_CMD_VOC_SELECT_AMR;
              break;
#endif /* MSMAUD_VOC_AMR_COMMON */

#ifdef FEATURE_AAC_REC
            case VOC_CAP_AAC_REC:
              audio_mode     = VOC_STATE_AUDIO_MODE_AAC;
              voc_select_cmd = QDSP_CMD_VOC_SELECT_AAC;
              break;
#endif /* FEATURE_AAC_REC */

            default:
              audio_mode = VOC_STATE_AUDIO_MODE_NONE;
              break;
          }
        }
      } else {
        audio_mode = VOC_STATE_AUDIO_MODE_NONE;
      }

      /* Check if audio mode has changed; if so, issue command */
      if((audio_mode != VOC_STATE_AUDIO_MODE_NONE) &&
         (voc_state_data.audio_mode != audio_mode)) {
        switch (voc_state_data.current_image) {
#if defined(QDSP_IMAGE_QTV_DEFINED)        || \
    defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED) || \
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_QCAMCORDER_DEFINED) || \
    defined(QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED) || \
    defined(QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_AACENC_DEFINED)
#ifdef QDSP_IMAGE_QTV_DEFINED
          case QDSP_IMAGE_QTV:
#endif /* QDSP_IMAGE_QTV_DEFINED */
#ifdef QDSP_IMAGE_QTV_VOC_H264_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QTV_VOC_H264_DEFINED */
#ifdef QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED
#error code not present
#endif /*QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED*/
#ifdef QDSP_IMAGE_QCAMCORDER_DEFINED
          case QDSP_IMAGE_QCAMCORDER:
#endif /* QDSP_IMAGE_QCAMCORDER_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED
          case QDSP_IMAGE_QCAMCORDER_AACENC:
#endif /* QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED */
#ifdef QDSP_IMAGE_AACENC_DEFINED
          case QDSP_IMAGE_AACENC:
#endif /* QDSP_IMAGE_AACENC_DEFINED */
            voc_state_data.retry_counter = 0;
            voc_state_data.audio_mode    = audio_mode;
            break;
#endif
          default:
            if(qdsp_cmd_select_vocoder(voc_select_cmd) != QDSP_CMD_SUCCESS) {
              if(voc_state_data.retry_counter < 100) {
                voc_state_data.retry_counter++;
              } else {
                MSG_ERROR("DSP failed to issue vocoder select cmd", 0,0,0 );
                voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
              }
              return(VOC_STATE_CONTINUE);
            } else {
              voc_state_data.retry_counter = 0;
              voc_state_data.audio_mode    = audio_mode;
            }

            /* Wait for command to be processed by DSP */
            if(!qdsp_cmd_verify_voc_select_done()) {
              if(voc_state_data.retry_counter < 100) {
                voc_state_data.retry_counter++;
              } else {
                MSG_ERROR("DSP failed to finish vocoder select cmd", 0,0,0 );
                voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
              }
              return(VOC_STATE_CONTINUE);
            }
            voc_state_data.retry_counter = 0;
            break;
        }
      }
    }

    if ( voc_state_clocks_on == FALSE ) {
      /* Enable all vocoder and codec clocks */
      voc_state_enable_clocks();
      voc_state_clocks_on = TRUE;
    }

    /* Allow QDSP time to run */
    #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
    clk_busy_wait(200);
 	#else
#error code not present
	#endif

#ifdef MSMAUD_ADIE_CODEC
    /* Wake up the Analog Die if the codec in use is the internal codec. */
    (void) voc_state_adie_wakeup();
#endif /* MSMAUD_ADIE_CODEC */

#ifdef FEATURE_MM_REC
    voc_state_data.mm_rec_config_pending = FALSE;
#endif /* FEATURE_MM_REC */

    /* Get the codec configuration data */
    voc_data_get_codec(&pcm_path);
    voc_data_get_codec_class(&codec_type);
    voc_data_get_codec_config(voc_state_data.current_image, pcm_path,
                              &codec_config_ptr);

    /* Disable any registered codec functions */
    voc_disable_codec_func(FALSE);

    /* Issue codec configuration command */
    result = qdsp_cmd_codec_config(codec_config_ptr,\
             voc_data_get_default_frame_rate(voc_state_data.current_image,\
                                                                 pcm_path));
    if (result != QDSP_CMD_SUCCESS) {
      MSG_ERROR("MM decode command failed",0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }

#ifdef MSMAUD_SCMM
#error code not present
#endif /* MSMAUD_SCMM */

    voc_state_data.codec_pending = FALSE;

#ifdef FEATURE_AVS_I2SSBC
    if (((pcm_path == VOC_CODEC_BT_A2DP) ||
         (pcm_path == VOC_CODEC_BT_A2DP_SCO)) &&
        (voc_state_data.current_image == QDSP_IMAGE_I2SSBC)) {
#ifdef FEATURE_EXTERNAL_SADC
      voc_state_config_external_sadc(SADC_STEREO_LINE_IN);
#endif
    }
#endif /* FEATURE_AVS_I2SSBC */
   /* Enable registered codec function */
    voc_enable_codec_func();

#ifdef FEATURE_EXTERNAL_SADC
    if ((codec_type == VOC_CODEC_CLASS_EXTERNAL_SADC_HANDSET)
        || (codec_type == VOC_CODEC_CLASS_EXTERNAL_SADC_HEADSET))
    {
      if ((pcm_path == VOC_CODEC_IN_MONO_SADC_OUT_MONO_HANDSET) ||
          (pcm_path == VOC_CODEC_IN_MONO_SADC_OUT_STEREO_HEADSET)) {

        voc_state_config_external_sadc(SADC_MONO_LINE_IN);

      } else if ((pcm_path == VOC_CODEC_IN_STEREO_SADC_OUT_MONO_HANDSET) ||
                 (pcm_path == VOC_CODEC_IN_STEREO_SADC_OUT_STEREO_HEADSET)) {

        voc_state_config_external_sadc(SADC_STEREO_LINE_IN);
      }
    }
#endif /* FEATURE_EXTERNAL_SADC */

#ifdef FEATURE_GRAPH_ADPCM
    /* Reset new config state, should only receive configuration information
    ** after going to GRAPH IDLE state.
    */
    voc_state_data.adpcm_new_config = FALSE;
#endif /* FEATURE_GRAPH_ADPCM */

    /* Transition to the graph Idle state */
    MSG_HIGH("Vocoder going to graph idle",0,0,0);
    voc_state_data.state            = VOC_STATE_GRAPH_IDLE_ENTRY;
    return(VOC_STATE_CONTINUE);

  } else { /* Vocoder is not acquired */

/* A2DP support for for NB-AMR */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    /* Disable the bluetooth A2DP when DSP is going to sleep */
    if (voc_get_bt_state() != VOC_BT_STATE_DISABLED) {
      (void) voc_check_bt(VOC_BT_OP_FORCE_DISABLE);
    }
#if defined (FEATURE_AVS_BT_SCO_REWORK)
    if ((bt_func_ptr != NULL) && voc_bt_playback_started) {
      MSG_MED("Calling BT callback with PLAYBACK_STOP", 0,0,0);
      bt_func_ptr(VOC_PLAYBACK_STOP);
      voc_bt_playback_started = FALSE;
    }
#endif /*AVS_BT_SCO_REWORK*/
#endif /* FEATURE_SBC_CODEC */

    /* Shut down clocks and codecs */
    voc_state_shutdown();

    /* Stay in SLEEP/INIT until vocoder is acquired */
    return(VOC_STATE_SUCCESS);
  }
} /* voc_state_graph_init */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_graph_idle_entry

DESCRIPTION
  Wait for the QDSP2 to reach idle state.  When QDSP2 is in idle, inform
  users, and transition the driver to idle state. If the QDSP2 does not
  reach idle within the allowed time, then perform error recovery.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_graph_idle_entry (void)
{

  if ( qdsp_cmd_get_state() != QDSP_STATE_GRAPH_IDLE ) {
    /* If QDSP failed to reach idle state, attempt retry */
    if ( voc_state_data.retry_counter < 100 ) {
      MSG_HIGH( "Idle state transition retry", 0,0,0 );
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    } else {
      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR( "QDSP failed to reach graph idle state", 0,0,0 );
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  voc_state_data.retry_counter = 0;

  if(voc_state_change_voc_selection()) {
    /* Go to sleep */
    voc_state_data.state = VOC_STATE_SLEEP;
    voc_state_adie_stall();
    if (qdsp_cmd_sleep(voc_state_aux_pcm_clk_query()) == QDSP_CMD_SUCCESS) {
      MSG_HIGH("Vocoder going to SLEEP",0,0,0);
    } else {
      MSG_ERROR("Sleep command failed",0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
    }
  } else {
    if (voc_state_init_graph_afe() != VOC_STATE_SUCCESS) {
      MSG_ERROR("AFE setup failed",0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
    }
    else {
      /* Turn analog part of Adie */
      #ifdef MSMAUD_SCMM
#error code not present
      #endif /* MSMAUD_SCMM */
      voc_state_data.state = VOC_STATE_GRAPH_IDLE;
      MSG_HIGH("Entered GRAPH IDLE state",0,0,0);

      /* Inform users of transition to graph idle state */
      voc_inform(VOC_INFORM_GRAPH_IDLE);
    }
  }

  return(VOC_STATE_CONTINUE);
} /* voc_state_graph_idle_entry */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_graph_idle

DESCRIPTION
  Stay in IDLE state until the vocoder is configured or released. If the
  vocoder is configured, transition to the correct state. If the vocoder is
  released transition to the SLEEP state.

DEPENDENCIES
  None

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type voc_state_graph_idle (void)
{
  qdsp_cmd_status_type       result;
  qdsp_module_type           module_select;
  voc_status_type            module_select_status;
  boolean                    go_to_sleep = TRUE;
  qdsp_cmd_audio_config_type config;
#if defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED) || \
    defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED)       || \
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_QTV_DEFINED)
  qdsp_cmd_voc_timing_type   *timing_ptr;
#endif

#ifdef FEATURE_AAC_REC
  qdsp_cmd_aac_encoder_config_type encoder_config;
#endif /* FEATURE_AAC_REC */

  module_select_status = voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_ENABLED) &&
      (voc_state_data.acquire_mask != 0)) {

    if ((voc_state_data.module_reqd == module_select)
#ifdef FEATURE_MM_REC
        && (voc_state_data.mm_rec_config_pending == FALSE)
#endif /* FEATURE_MM_REC */
        ) {
      go_to_sleep = FALSE;
    }
    else {
      if (!voc_state_change_voc_selection()
#ifdef QDSP_IMAGE_QTV_DEFINED
          && (voc_state_data.current_image != QDSP_IMAGE_QTV)
#endif
#ifdef QDSP_IMAGE_QTV_VOC_H264_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED
#error code not present
#endif
#ifdef FEATURE_MM_REC
          && (voc_state_data.mm_rec_config_pending == FALSE)
#endif /* FEATURE_MM_REC */
          ) {
        MSG_HIGH("qdsp_enable(%d,%d)",
                   voc_state_data.current_qdsps_app, module_select,0);
        qdsp_enable (voc_state_data.current_qdsps_app, module_select);
        return (VOC_STATE_CONTINUE);
      }
    }
  }

  if ((go_to_sleep == FALSE) && !voc_state_change_voc_selection()) {
    if (voc_state_data.codec_pending == TRUE)
    {
      voc_retry_codec_select();
    }

    if ( module_select_status == VOC_DONE_S ) {
      voc_state_data.config = voc_state_data.config_pending;
    }

    if((((voc_state_data.config_mask & (uint16)(0x1 << VOC_ACQ_VOCREC)) != 0)||
       ((voc_state_data.config_mask & (uint16)(0x1 << VOC_ACQ_VOCVS)) != 0))
#ifdef FEATURE_MM_REC
       || ((voc_state_data.config_mask & (uint16)(0x1 << VOC_ACQ_MMREC)) != 0)
#endif /* FEATURE_MM_REC */
      ) {
      switch (voc_state_data.config) {

        case VOC_CAP_IS733:
        case VOC_CAP_IS127:
          qdsp_cmd_set_rec_frame_rates();
#ifdef MSMAUD_VOC_AMR_COMMON
        case VOC_CAP_AMR:
#endif /* MSMAUD_VOC_AMR_COMMON */
#ifdef MSMAUD_VOC_4GV
        case VOC_CAP_4GV_NB:
#endif /* MSMAUD_VOC_4GV */
          config.coder_flag = QDSP_CODER_FLAG_VOC;
          if(voc_state_data.config == VOC_CAP_IS127) {
            config.audio_config = QDSP_AUDIO_MODE_EVRC;
#ifdef MSMAUD_VOC_AMR_COMMON
          } else if(voc_state_data.config == VOC_CAP_AMR) {
            config.audio_config = QDSP_AUDIO_MODE_AMR;
#endif /* MSMAUD_VOC_AMR_COMMON */
#ifdef MSMAUD_VOC_4GV
          }else if (voc_state_data.config == VOC_CAP_4GV_NB ) {
            config.audio_config = QDSP_AUDIO_MODE_4GV_NB;
#endif /* MSMAUD_VOC_4GV */
          } else {
            config.audio_config = QDSP_AUDIO_MODE_13K;
          }

#if defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED) || \
    defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED) || \
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_QTV_DEFINED)
#if defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED) && \
    defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED) && \
    defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) && \
    defined(QDSP_IMAGE_QTV_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED) && \
      defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED) && \
      defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED) && \
      defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED) && \
      defined(QDSP_IMAGE_QTV_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED) && \
      defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) && \
      defined(QDSP_IMAGE_QTV_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED) && \
      defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) && \
      defined(QDSP_IMAGE_QTV_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_QTV_DEFINED)&& \
      defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED)
          if ((voc_state_data.current_image == QDSP_IMAGE_QTV)||\
			  (voc_state_data.current_image == QDSP_IMAGE_MP4D_AMR_EVRC_V13K))
#elif defined(QDSP_IMAGE_QTV_DEFINED)&& \
      defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED) && \
      defined(QDSP_IMAGE_QTV_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED) && \
      defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED) && \
      defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED) && \
      defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED)
          if (voc_state_data.current_image == QDSP_IMAGE_MP4D_AMR_EVRC_V13K)
#elif defined(QDSP_IMAGE_QTV_DEFINED)
          if (voc_state_data.current_image == QDSP_IMAGE_QTV)
#elif defined(QDSP_IMAGE_QTV_VOC_H264_DEFINED)
#error code not present
#elif defined(QDSP_IMAGE_QTV_VOC_H264_TURBO_DEFINED)
#error code not present
#endif
          {
            timing_ptr = voc_data_get_timing(voc_state_data.config);
            if (qdsp_cmd_voc_timing(timing_ptr) != QDSP_CMD_SUCCESS) {
              MSG_ERROR("Vocoder Timing command failed",0,0,0);
              voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
              return (VOC_STATE_CONTINUE);
            }

            if (voc_state_voc_config(voc_state_data.config) !=
                                                        VOC_STATE_SUCCESS) {
              /* Giving a second chance. Wait for 5 ms */
              (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);
              if (voc_state_voc_config( voc_state_data.config )
                  != VOC_STATE_SUCCESS ) {
                MSG_ERROR("Vocoder Configuration command failed",0,0,0);
                voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
                return (VOC_STATE_CONTINUE);
              }
            }

            /* Issue a manual frame reference command to cause the QDSP to
            ** transition to VOCODER state.
            */
            if (qdsp_cmd_voc_frame_reference() != QDSP_CMD_SUCCESS) {
              /* Giving a second chance. Wait for 5 ms */
              (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);
              if (qdsp_cmd_voc_frame_reference() != QDSP_CMD_SUCCESS) {
                MSG_ERROR("Vocoder Frame Reference command failed",0,0,0);
                voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
                return (VOC_STATE_CONTINUE);
              }
            }

          } else
#endif /* QDSP_IMAGE_MP4D_AMR_EVRC_V13K_DEFINED || QDSP_IMAGE_QTV_DEFINED */
          {
            if(qdsp_cmd_audio_config(&config) != QDSP_CMD_SUCCESS)
            {
              /* Giving a second chance. Wait for 5 ms */
              (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);
              if (qdsp_cmd_audio_config( &config )
                  != QDSP_CMD_SUCCESS ) {
                MSG_ERROR("Vocoder Configuration command failed",0,0,0);
                voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
                return (VOC_STATE_CONTINUE);
              }
            }
          }

#ifdef MSMAUD_VOC_AMR_COMMON
          if(voc_state_data.config == VOC_CAP_AMR) {
            switch(voc_state_data.current_image) {
#if defined(QDSP_IMAGE_MP4E_DEFINED)       || \
    defined(QDSP_IMAGE_MP4E_BAYER_DEFINED) || \
    defined(QDSP_IMAGE_MP4E_AMR_DEFINED)   || \
    defined(QDSP_IMAGE_QCAMCORDER_DEFINED) || \
    defined(QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED) || \
    defined(QDSP_IMAGE_AUDDUB_AMR_DEFINED)
#ifdef QDSP_IMAGE_MP4E_DEFINED
              case QDSP_IMAGE_MP4E:
#endif /* QDSP_IMAGE_MP4E_DEFINED */
#ifdef QDSP_IMAGE_MP4E_BAYER_DEFINED
              case QDSP_IMAGE_MP4E_BAYER:
#endif /* QDSP_IMAGE_MP4E_BAYER_DEFINED */
#ifdef QDSP_IMAGE_MP4E_AMR_DEFINED
              case QDSP_IMAGE_MP4E_AMR:
#endif /* QDSP_IMAGE_MP4E_AMR_DEFINED */
#ifdef QDSP_IMAGE_AUDDUB_AMR_DEFINED
              case QDSP_IMAGE_AUDDUB_AMR:
#endif /* QDSP_IMAGE_AUDDUB_AMR_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_DEFINED
              case QDSP_IMAGE_QCAMCORDER:
#endif /* QDSP_IMAGE_QCAMCORDER_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED */

                if((voc_init_amr_tx_rate( voc_data_get_amr_init_rate()) !=
                   VOC_DONE_S) ||
                  (voc_set_gsm_enc_dtx( voc_data_get_gsm_dtx_mode()) !=
                   VOC_DONE_S))
                {
                  return(VOC_STATE_CONTINUE);
                }
                break;
#endif /* QDSP_IMAGE_MP4E_DEFINED     || QDSP_IMAGE_MP4E_BAYER_DEFINED ||
          QDSP_IMAGE_MP4E_AMR_DEFINED || QDSP_IMAGE_AUDDUB_AMR_DEFINED */

              default:
                break;
            }
          }
#endif /* MSMAUD_VOC_AMR_COMMON */

          voc_state_data.state = VOC_STATE_GRAPH_AUDIO_ENTRY;
          MSG_HIGH("Going to GRAPH AUDIO state",0,0,0);
          break;

#ifdef FEATURE_AAC_REC
      case VOC_CAP_AAC_REC: {

          voc_mm_enc_format = VOC_MM_FORMAT_AAC;

          encoder_config.channel_select      =
              voc_state_data.aac_config.channel_configuration;

          encoder_config.sample_rate_index   =
              voc_state_data.aac_config.sample_freq_index;

          encoder_config.bit_per_sample_chan =
              voc_state_data.aac_config.bit_per_spl;

          if (qdsp_cmd_mm_encoder_config(&encoder_config) !=
              QDSP_CMD_SUCCESS) {

            /* Giving a second chance. Wait for 5 ms */
            (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);

            if (qdsp_cmd_mm_encoder_config(&encoder_config) !=
                QDSP_CMD_SUCCESS ) {

              MSG_ERROR("Vocoder Configuration command failed",0,0,0);
              voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
              return (VOC_STATE_CONTINUE);

            }
          } /* if (qdsp_cmd_mm_encoder_config(&encoder_config) */

          if (qdsp_cmd_mm_rec_audio_config() !=
              QDSP_CMD_SUCCESS) {

            /* Giving a second chance. Wait for 5 ms */
            (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);

            if (qdsp_cmd_mm_rec_audio_config() !=
                QDSP_CMD_SUCCESS ) {

              MSG_ERROR("Vocoder Configuration command failed",0,0,0);
              voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
              return (VOC_STATE_CONTINUE);
            }
          } /* if (qdsp_cmd_mm_rec_audio_config() */

          voc_state_data.state = VOC_STATE_GRAPH_AUDIO_ENTRY;

          MSG_HIGH("Going to RECORD ACITIVE state",0,0,0);
      } /* case VOC_CAP_AAC_REC */
#endif /* FEATURE_AAC_REC */

        default:
          break;
      }
      return (VOC_STATE_CONTINUE);
    } else
#ifdef FEATURE_AVS_I2SSBC
   if(((voc_state_data.acquire_mask & (uint16)(0x1 << VOC_ACQ_I2SSBC)) != 0) &&
      (voc_state_data.current_image == QDSP_IMAGE_I2SSBC))
   {
     if (voc_get_bt_state() != VOC_BT_STATE_A2DP_ENABLED) {
       /* Lets enable the BT link */
       voc_check_bt(VOC_BT_OP_A2DP_NORMAL);
       return (VOC_STATE_SUCCESS);
     } else {
       MSG_HIGH("Going to GRAPH AUDIO state",0,0,0);
       voc_state_data.state = VOC_STATE_GRAPH_AUDIO_ENTRY;
       return (VOC_STATE_CONTINUE_IMMEDIATE);
     }
   }else
#endif /* FEATURE_AVS_I2SSBC */
#ifdef FEATURE_GRAPH_ADPCM
    /* Check if in ADPCM mode */
    if(voc_state_data.adpcm_mode_enabled) {
      if(voc_state_data.adpcm_new_config) {
        switch(voc_state_data.current_image) {
#if defined(QDSP_IMAGE_VFE_BAYER_DEFINED) || \
    defined(QDSP_IMAGE_VFE_YCBCR_DEFINED)
#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
          case QDSP_IMAGE_VFE_BAYER:
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED */
#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
          case QDSP_IMAGE_VFE_YCBCR:
#endif /* QDSP_IMAGE_VFE_YCBCR_DEFINED */
            if((voc_state_data.adpcm_block != 0x200) ||
               (voc_state_data.adpcm_sampling_rate != 0x7d00) ||
               (voc_state_data.adpcm_num_channels != 0x1)) {
              voc_state_data.adpcm_new_config = FALSE;
              break;
            }
            /* Fall through */
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED || QDSP_IMAGE_VFE_YCBCR_DEFINED */

#ifdef QDSP_IMAGE_QCAMERA_DEFINED
          case QDSP_IMAGE_QCAMERA:
#endif /* QDSP_IMAGE_QCAMERA_DEFINED */
#ifdef QDSP_IMAGE_MP4E_DEFINED
          case QDSP_IMAGE_MP4E:
#endif /* QDSP_IMAGE_MP4E_DEFINED */
#ifdef QDSP_IMAGE_MP4E_BAYER_DEFINED
          case QDSP_IMAGE_MP4E_BAYER:
#endif /* QDSP_IMAGE_MP4E_BAYER_DEFINED */
#ifdef QDSP_IMAGE_MP4E_13K_DEFINED
          case QDSP_IMAGE_MP4E_13K:
#endif /* QDSP_IMAGE_MP4E_13K_DEFINED */
#ifdef QDSP_IMAGE_MP4E_EVRC_DEFINED
          case QDSP_IMAGE_MP4E_EVRC:
#endif /* QDSP_IMAGE_MP4E_EVRC_DEFINED */
#ifdef QDSP_IMAGE_MP4E_AMR_DEFINED
          case QDSP_IMAGE_MP4E_AMR:
#endif /* QDSP_IMAGE_MP4E_AMR_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_DEFINED
          case QDSP_IMAGE_QCAMCORDER:
#endif /* QDSP_IMAGE_QCAMCORDER_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED
          case QDSP_IMAGE_QCAMCORDER_AACENC:
#endif /* QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED */

#ifdef QDSP_IMAGE_AACENC_DEFINED
          case QDSP_IMAGE_AACENC:
#endif /* QDSP_IMAGE_AACENC_DEFINED */
            if(
#ifdef QDSP_IMAGE_VFE_BAYER_DEFINED
               (voc_state_data.current_image != QDSP_IMAGE_VFE_BAYER) &&
#endif /* QDSP_IMAGE_VFE_BAYER_DEFINED */
#ifdef QDSP_IMAGE_VFE_YCBCR_DEFINED
               (voc_state_data.current_image != QDSP_IMAGE_VFE_YCBCR) &&
#endif /* QDSP_IMAGE_VFE_YCBCR_DEFINED */
               ((voc_state_data.adpcm_block != 0x100) ||
                (voc_state_data.adpcm_sampling_rate != 0x1f40) ||
                (voc_state_data.adpcm_num_channels != 0x1))) {
              voc_state_data.adpcm_new_config = FALSE;
              break;
            }
            config.audio_config  = QDSP_AUDIO_MODE_ADPCM;
            config.num_channels  = voc_state_data.adpcm_num_channels;
            config.sampling_rate = voc_state_data.adpcm_sampling_rate;
            config.block_size    = voc_state_data.adpcm_block;

            /* Set the encoder frames rates even though we may not be
            ** recording 13K/EVRC - these encoders may still be enabled on the
            ** DSP.
            */
            qdsp_cmd_set_rec_frame_rates();

#ifdef QDSP_IMAGE_QCAMCORDER_DEFINED
            if (voc_state_data.current_image == QDSP_IMAGE_QCAMCORDER) {
              config.coder_flag = QDSP_CODER_FLAG_ADPCM;
            }
#endif /* QDSP_IMAGE_QCAMCORDER_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED
#error code not present
#endif /* QDSP_IMAGE_QCAMCORDER_TURBO_DEFINED */

#ifdef QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED
            if (voc_state_data.current_image == QDSP_IMAGE_QCAMCORDER_AACENC) {
              config.coder_flag = QDSP_CODER_FLAG_ADPCM;
            }
#endif /* QDSP_IMAGE_QCAMCORDER_AACENC_DEFINED */
#ifdef QDSP_IMAGE_AACENC_DEFINED
            if (voc_state_data.current_image == QDSP_IMAGE_AACENC) {
              config.coder_flag = QDSP_CODER_FLAG_ADPCM;
            }
#endif /* QDSP_IMAGE_AACENC_DEFINED */

            if(qdsp_cmd_audio_config(&config) != QDSP_CMD_SUCCESS) {
              MSG_ERROR("Vocoder Configuration command failed",0,0,0);
              voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
              return (VOC_STATE_CONTINUE);
            }

            voc_state_data.state = VOC_STATE_GRAPH_AUDIO_ENTRY;
            MSG_HIGH("Going to GRAPH AUDIO state",0,0,0);
            return (VOC_STATE_CONTINUE);
            break;

          default:
            break;
        }
      }
      voc_state_data.adpcm_new_config = FALSE;
      return (VOC_STATE_SUCCESS);
    } else
#endif /* FEATURE_GRAPH_ADPCM */
    {
      /* Vocoder is not configured, just stay in IDLE */
      return (VOC_STATE_SUCCESS);
    }
  } else {
    /* Go to sleep */
    voc_state_data.state = VOC_STATE_SLEEP;
    voc_state_adie_stall();
    result = qdsp_cmd_sleep(voc_state_aux_pcm_clk_query());
    if (result == QDSP_CMD_SUCCESS) {
      MSG_HIGH("Vocoder going to SLEEP",0,0,0);
    } else {
      MSG_ERROR("Sleep command failed",0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
    }
    return (VOC_STATE_CONTINUE);
  }
} /* voc_state_graph_idle */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_graph_audio_entry

DESCRIPTION
  When the DSP to arrives in audio state, set the volume, inform users
  of the state change, and transition state machine to GRAPH AUDIO state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  Volume is set (unmuted).

===========================================================================*/
voc_state_return_type voc_state_graph_audio_entry (void)
{

/* A2DP support for for NB-AMR */
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
  voc_pp_cb_type  pp_cb = NULL;
  voc_pp_type_type pp_config;
  qdsp_cmd_status_type status = QDSP_CMD_SUCCESS;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

/* A2DP support for for NB-AMR */
#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
  voc_bt_cb_func_ptr_type  bt_func_ptr = NULL;
  bt_func_ptr = voc_data_get_bt_cb_func();
#endif

  if ( qdsp_cmd_get_state() != QDSP_STATE_GRAPH_AUDIO ) {
    /* If QDSP failed to reach GRAPH AUDIO state. Attempt retry, max 1s */
    if ( voc_state_data.retry_counter < 100 ) {
      MSG_HIGH( "GRAPH AUDIO state transition retry", 0,0,0 );
      voc_state_data.retry_counter++;
      return(VOC_STATE_CONTINUE);
    } else {

/* A2DP support for for NB-AMR */
#if defined(FEATURE_ARM_POST_PROCESSING) || \
      defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
      qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
      voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR( "QDSP failed to reach GRAPH AUDIO state", 0,0,0 );
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  voc_state_data.retry_counter = 0;

  MSG_HIGH("Entered GRAPH AUDIO state",0,0,0);

  /* Reset config flag */
  voc_state_data.config_flag = FALSE;
  voc_state_data.state       = VOC_STATE_GRAPH_AUDIO;

  /* Issue volume control command */
  if (qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL))
       != QDSP_CMD_SUCCESS)
  {
     MSG_ERROR("Failed to set volume", 0, 0, 0);
     voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
     return (VOC_STATE_CONTINUE);
  }

#ifdef FEATURE_GRAPH_ADPCM
  /* Reset ADPCM new config and mode change flags */
  voc_state_data.adpcm_new_config = FALSE;
  voc_state_data.adpcm_mode_flag  = FALSE;

  /* Initialize ADEC interrupt callback function */
  (void) qdsp_cmd_adec_config(voc_adec_isr);
#endif /* FEATURE_GRAPH_ADPCM */

#ifdef FEATURE_MM_REC
  (void) qdsp_cmd_mm_rec_config(voc_mm_enc_format, voc_mm_rec_cb_isr);
#endif /* FEATURE_MM_REC */

/* A2DP support for for NB-AMR */
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
  pp_cb = voc_data_get_pp_func();
  if (pp_cb  != NULL)
  {
    pp_config = voc_data_get_pp_type();
    switch (pp_config) {
    case VOC_PP_TYPE_GENERIC:
    status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_GENERIC);
      break;
#if defined (FEATURE_SBC_CODEC)
    case VOC_PP_TYPE_SBC:
    status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_SBC);
      break;
#elif defined (FEATURE_SBC_DSP_CODEC)
    case VOC_PP_TYPE_DSP_SBC:
      status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_DSP_SBC);
      break;
#endif
    }
    if (status == QDSP_CMD_SUCCESS) {
    voc_state_pp_status = VOC_PP_STATUS_ENABLED;
  } else {
    /* If callback is null or if qdsp_cmd_register_pp_cb failed */
    voc_state_pp_status = VOC_PP_STATUS_DISABLED;
  }
  }
#endif /*(FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE)*/

/* A2DP support for for NB-AMR */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
 #if defined (FEATURE_AVS_AUDIO_DECODER_SCO)
   voc_check_bt(VOC_BT_OP_NORMAL);
    #if defined (FEATURE_AVS_BT_SCO_REWORK)
    if (bt_func_ptr != NULL)
    {
      /* Calling BT callback with PLAYBACK_A2DP only if FW supports,
         if not callback with PLAYBACK_SCO */
      #if defined(QDSP_vocoderPpFirstInitFlag)
#error code not present
      #endif
      {
        MSG_MED("Calling BT callback with PLAYBACK SCO", 0,0,0);
        bt_func_ptr((voc_playback_enum_type)VOC_PLAYBACK_SCO);
        voc_bt_playback_started = TRUE;
      }
    }
    #endif/*AVS_BT_SCO_REWORK*/
 #else
  voc_check_bt(VOC_BT_OP_A2DP_NORMAL);

#if defined (FEATURE_AVS_BT_SCO_REWORK)
  if (bt_func_ptr != NULL)
  {
    MSG_MED("Calling BT callback with PLAYBACK_A2DP", 0,0,0);
    bt_func_ptr(VOC_PLAYBACK_A2DP);
    voc_bt_playback_started = TRUE;
  }
#endif/*AVS_BT_SCO_REWORK*/
 #endif /*FEATURE_AVS_AUDIO_DECODER_SCO */
#endif /* FEATURE_BT_AG */


  voc_inform(VOC_INFORM_GRAPH_AUDIO);

  return (VOC_STATE_CONTINUE);
} /* voc_state_graph_audio_entry */

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_graph_audio

DESCRIPTION
  Monitor the current configuration. If the configuration changes or is
  cleared, command the DSP to transition to IDLE state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None.

===========================================================================*/
voc_state_return_type voc_state_graph_audio (void)
{
  qdsp_module_type           module_select;
#ifdef FEATURE_AVS_I2SSBC
  boolean goto_idle = FALSE;
#endif /* FEATURE_AVS_I2SSBC */
  voc_status_type			module_select_status;

/* A2DP support for for NB-AMR */
#if defined(FEATURE_ARM_POST_PROCESSING) || \
      defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
  voc_pp_cb_type  pp_cb = NULL;
  voc_pp_type_type pp_config;
  qdsp_cmd_status_type status = QDSP_CMD_SUCCESS;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

  /* Clear any previous error condition */
  voc_state_clear_fw_err_cnt();

/* A2DP support for for NB-AMR */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
  #if defined (FEATURE_AVS_AUDIO_DECODER_SCO)
    voc_check_bt(VOC_BT_OP_NORMAL);
  #else
    voc_check_bt(VOC_BT_OP_A2DP_NORMAL);
  #endif /*FEATURE_AVS_AUDIO_DECODER_SCO */
#endif /* FEATURE_SBC_CODEC */

#ifdef FEATURE_AVS_I2SSBC
  if (voc_state_data.current_image == QDSP_IMAGE_I2SSBC) {
    /* Check if the BT link is still up. If the link is down we need to go to
    sleep state */
    voc_check_bt(VOC_BT_OP_A2DP_NORMAL);
    if (voc_get_bt_state() != VOC_BT_STATE_A2DP_ENABLED) {
      goto_idle = TRUE;
    }
  }
#endif /* FEATURE_AVS_I2SSBC */

  /* Determine whether the vocoder should stay in AUDIO state
  ** or return to IDLE.
  */
  module_select_status =  voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 voc_state_data.current_image,voc_state_data.module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  if ((voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (voc_state_data.module_reqd != module_select) ||
      (((voc_state_data.config_mask == 0) ||
        (voc_state_data.config_flag || voc_state_data.config_standby) ||
        (voc_state_data.config_pending != voc_state_data.config))
#ifdef FEATURE_GRAPH_ADPCM
       && (!voc_state_data.adpcm_mode_enabled)
#endif
#ifdef FEATURE_AVS_I2SSBC
       && (!((module_select == QDSP_MODULE_I2SSBC) &&
             (voc_state_data.acquire_mask & (uint16)(0x1 << VOC_ACQ_I2SSBC)) &&
             (voc_state_data.current_image == QDSP_IMAGE_I2SSBC)))
#endif /* FEATURE_AVS_I2SSBC */
      )
#ifdef FEATURE_GRAPH_ADPCM
      || (voc_state_data.adpcm_new_config)
      || (voc_state_data.adpcm_mode_flag)
#endif
      || voc_state_change_voc_selection()
#ifdef FEATURE_MM_REC
      || (voc_state_data.mm_rec_config_pending == TRUE)
#endif /*  */
#ifdef FEATURE_AVS_I2SSBC
      || (goto_idle)
#endif /* FEATURE_AVS_I2SSBC */
      || (voc_state_data.codec_pending==TRUE)
     )
  {

#ifdef FEATURE_GRAPH_ADPCM
    /* Disable audio decoder */
    (void) qdsp_cmd_adec_config(NULL);
#endif /* FEATURE_GRAPH_ADPCM */

/* A2DP support for for NB-AMR */
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
    qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
    voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

#ifdef FEATURE_AVS_I2SSBC
    if (!goto_idle) {
      /* We are not going to idle because of BT link shutting down, so we
      need to shutdown the BT link */
      voc_check_bt(VOC_BT_OP_A2DP_FORCE_DISABLE);
    }
#endif /* FEATURE_AVS_I2SSBC */

    if (voc_state_go_to_idle() == VOC_STATE_SUCCESS) {
      voc_state_data.state = VOC_STATE_GRAPH_IDLE_ENTRY;

#ifdef FEATURE_EXTERNAL_SADC
      voc_state_config_external_sadc(SADC_OFF);
#endif /* FEATURE_EXTERNAL_SADC */

      if (voc_state_data.codec_pending==TRUE)
      {
        voc_retry_codec_select();
      }
    } else {
      MSG_ERROR("GRAPH AUDIO->IDLE failed",0,0,0);
      voc_state_data.state = VOC_STATE_ERROR_RECOVERY;
    }
    return (VOC_STATE_CONTINUE);
  } else {

/* A2DP support for for NB-AMR */
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined(FEATURE_AVS_WB_PCM_RX_INTERFACE)
    pp_cb = voc_data_get_pp_func();
    if (pp_cb != NULL && voc_state_pp_status == VOC_PP_STATUS_DISABLED)
    {
    pp_config = voc_data_get_pp_type();
    if (pp_cb  != NULL)
    {
      pp_config = voc_data_get_pp_type();
      switch (pp_config) {
      case VOC_PP_TYPE_GENERIC:
        status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_GENERIC);
        break;
#if defined (FEATURE_SBC_CODEC)
      case VOC_PP_TYPE_SBC:
        status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_SBC);
        break;
#elif defined (FEATURE_SBC_DSP_CODEC)
      case VOC_PP_TYPE_DSP_SBC:
        status = qdsp_cmd_register_pp_cb(pp_cb, QDSP_CMD_PP_TYPE_DSP_SBC);
        break;
#endif
      }
      if (status == QDSP_CMD_SUCCESS) {
      voc_state_pp_status = VOC_PP_STATUS_ENABLED;
      } else {
      /* If callback is null or if qdsp_cmd_register_pp_cb failed */
      voc_state_pp_status = VOC_PP_STATUS_DISABLED;
      }
    }
    }
    else if (pp_cb == NULL && voc_state_pp_status == VOC_PP_STATUS_ENABLED)
    {
    voc_state_pp_status = VOC_PP_STATUS_DISABLED ;
    qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
    }
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

    return (VOC_STATE_SUCCESS);
  }
} /* voc_state_graph_audio */
#endif /* FEATURE_GRAPH_AUDIO */

/* <EJECT> */
/*===========================================================================

                    R E G I O N A L   F U N C T I O N S

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
/*===========================================================================

FUNCTION voc_state_get_state_data

DESCRIPTION
  Returns a pointer to the current state control structure.

DEPENDENCIES
  None.

RETURN VALUE
  Returns a pointer to the current state control structure.

SIDE EFFECTS
  None.

===========================================================================*/
voc_state_control_type *voc_state_get_state_data (void)
{
  return(&voc_state_data);
} /* voc_state_get_state_data */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_get_inform_val

DESCRIPTION
  Converts the current internal state to the value needed by vocoder clients.

DEPENDENCIES
  None.

RETURN VALUE
  The inform value for vocoder clients.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_inform_type voc_state_get_inform_val (void)
{
  voc_inform_type        inform_val = VOC_INFORM_RESET;

  switch (voc_state_data.state) {
    case VOC_STATE_RESET:
    case VOC_STATE_IMAGE:
    case VOC_STATE_SLEEP:
    case VOC_STATE_INIT_ENTRY:
    case VOC_STATE_INIT:
#ifdef FEATURE_QSYNTH_COMMON
    case VOC_STATE_QSYNTH_INIT:
    case VOC_STATE_QSYNTH_DISABLE_PENDING:
    case VOC_STATE_QSYNTH_ENTRY:
#endif
#ifdef FEATURE_QTUNES_COMMON
    case VOC_STATE_QTUNES_INIT:
    case VOC_STATE_QTUNES_DISABLE_PENDING:
    case VOC_STATE_QTUNES_ENTRY:
#endif
#ifdef FEATURE_QTUNES_COMMON
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
    case VOC_STATE_CAM_AUDIO_INIT:
    case VOC_STATE_CAM_AUDIO_ENTRY:
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
#endif /* FEATURE_QTUNES_COMMON */
#ifdef FEATURE_GRAPH_AUDIO
    case VOC_STATE_GRAPH_INIT:
#endif
    case VOC_STATE_ERROR_RECOVERY:
      inform_val = VOC_INFORM_RESET;
      break;

    case VOC_STATE_IDLE_ENTRY:
    case VOC_STATE_IDLE:
    case VOC_STATE_DFM_ENTRY:
    case VOC_STATE_VOCODER_ENTRY:
      inform_val = VOC_INFORM_IDLE;
      break;

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

    case VOC_STATE_VOCODER:
    case VOC_STATE_VOC_TWO_DECODERS:
#ifdef FEATURE_VOC_G711
    case VOC_STATE_WAV_VOCODER_ENTRY:
    case VOC_STATE_WAV_VOCODER:
#endif /* FEATURE_VOC_G711 */
      inform_val = VOC_INFORM_VOCODER;
      break;

#ifdef FEATURE_VOC_ADPCM
    case VOC_STATE_ADPCM_VOCODER_ENTRY:
    case VOC_STATE_ADPCM_VOCODER:
     inform_val = VOC_INFORM_VOCODER_ADPCM;
     break;
#endif /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_AVS_INCALL_ADEC_PB
    case VOC_STATE_ADEC_VOCODER:
     inform_val = VOC_INFORM_VOCODER_ADEC;
     break;
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
	case VOC_STATE_MIXER_ACTIVE_ENTRY:
    case VOC_STATE_MIXER_ACTIVE:
     inform_val = VOC_INFORM_MIXER_ACTIVE;
     break;
    case VOC_STATE_MIXER_IDLE_ENTRY:
    case VOC_STATE_MIXER_IDLE:
     inform_val = VOC_INFORM_MIXER_IDLE;
     break;
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifdef FEATURE_QSYNTH_COMMON
    case VOC_STATE_QSYNTH:
      if (
#ifdef FEATURE_QSYNTH_ADPCM
          (voc_state_data.adec_mode   != voc_state_data.adec_mode_pending) ||
#endif /* FEATURE_QSYNTH_ADPCM */
          (voc_state_data.qsynth_vol_mode_pending
                                      != voc_state_data.qsynth_vol_mode) ||
          (voc_state_data.module_status ==
                                       VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
          (voc_state_data.hybrid_mode != voc_state_data.hybrid_mode_pending))
      {
        inform_val = VOC_INFORM_RESET;
      } else {
        inform_val = VOC_INFORM_QSYNTH;
      }
      break;
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
    case VOC_STATE_CAM_AUDIO_ACTIVE:
      inform_val = VOC_INFORM_CAM_AUDIO_ACTIVE;
      break;
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */
#endif
#ifdef FEATURE_QTUNES_COMMON
    case VOC_STATE_QTUNES:
      inform_val = VOC_INFORM_QTUNES;
      break;
#endif

#ifdef FEATURE_GRAPH_AUDIO
    case VOC_STATE_GRAPH_IDLE_ENTRY:
    case VOC_STATE_GRAPH_IDLE:
    case VOC_STATE_GRAPH_AUDIO_ENTRY:
      inform_val = VOC_INFORM_GRAPH_IDLE;
      break;

    case VOC_STATE_GRAPH_AUDIO:
      inform_val = VOC_INFORM_GRAPH_AUDIO;
      break;
#endif /* FEATURE_GRAPH_AUDIO */
  }
  return (inform_val);
} /* voc_state_get_inform_val */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_get_transition_duration

DESCRIPTION
  Returns the time required between transitions for the current DSP image.

DEPENDENCIES
  voc_state_state_data.current_image

RETURN VALUE
  duration (ms)

SIDE EFFECTS
  None.

===========================================================================*/
rex_timer_cnt_type voc_state_get_transition_duration (
  voc_state_return_type state_trans
)
{
  rex_timer_cnt_type state_trans_time = VOC_STATE_TRANS_TIME_DEFAULT;

  switch (voc_state_get_state_data()->current_image) {
#ifdef QDSP_IMAGE_QVP_MPEG4_EVRC_DEFINED
    case QDSP_IMAGE_QVP_MPEG4_EVRC:
      state_trans_time = VOC_STATE_TRANS_TIME_QVP;
      break;
#endif
#ifdef QDSP_IMAGE_QVP_H263P3_EVRC_DEFINED
    case QDSP_IMAGE_QVP_H263P3_EVRC:
      state_trans_time = VOC_STATE_TRANS_TIME_QVP;
      break;
#endif
#ifdef QDSP_IMAGE_QVP_MPEG4_4GVWB_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QVP_H263P3_4GVWB_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QVP_MPEG4_AMR_DEFINED
    case QDSP_IMAGE_QVP_MPEG4_AMR:
      state_trans_time = VOC_STATE_TRANS_TIME_QVP;
      break;
#endif
#ifdef QDSP_IMAGE_QVP_YUV_AMR_H263_DEFINED
    case QDSP_IMAGE_QVP_YUV_AMR_H263:
      state_trans_time = VOC_STATE_TRANS_TIME_QVP;
      break;
#endif
#ifdef QDSP_IMAGE_QVP_YUV_AMR_MPEG4_DEFINED
    case QDSP_IMAGE_QVP_YUV_AMR_MPEG4:
      state_trans_time = VOC_STATE_TRANS_TIME_QVP;
      break;
#endif
#ifdef QDSP_IMAGE_QVP_H263P3_AMR_DEFINED
    case QDSP_IMAGE_QVP_H263P3_AMR:
      state_trans_time = VOC_STATE_TRANS_TIME_QVP;
      break;
#endif
#ifdef QDSP_IMAGE_QVP_H263P3_G723_DEFINED
    case QDSP_IMAGE_QVP_H263P3_G723:
      state_trans_time = VOC_STATE_TRANS_TIME_QVP;
      break;
#endif
#ifdef QDSP_IMAGE_QVP_MPEG4_G723_DEFINED
    case QDSP_IMAGE_QVP_MPEG4_G723:
      state_trans_time = VOC_STATE_TRANS_TIME_QVP;
      break;
#endif
#ifdef QDSP_IMAGE_QVP_BAYER_AMR_DEFINED
    case QDSP_IMAGE_QVP_BAYER_AMR:
      state_trans_time = VOC_STATE_TRANS_TIME_QVP;
      break;
#endif

    default:
      if (state_trans == VOC_STATE_CONTINUE_IMMEDIATE) {
        state_trans_time = VOC_STATE_TRANS_TIME_RAPID;
      }
      break;
  }
  return (state_trans_time);
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_shutdown_due_to_error

DESCRIPTION
  Shuts down the DSP and kicks the state machine back to RESET state.

DEPENDENCIES
  voc_state_data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void voc_state_shutdown_due_to_error (void)
{
  if (voc_state_data.module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) {
    /* The state will be moved to DISABLED in voc_state_shutdown() */
    voc_state_data.module_status = VOC_STATE_MOD_STATUS_ENABLE_REQUESTED;
    MSG_HIGH("qdsp_disable_event_rsp(%d)",
                              voc_state_data.current_qdsps_app,0,0);
    qdsp_cmd_clear_dsp_isr();
    qdsp_disable_event_rsp(voc_state_data.current_qdsps_app);
  }
  voc_state_shutdown();

  /* Debounce the error condition by jumping to RESET state */
  voc_state_data.state = VOC_STATE_RESET;
  voc_state_data.retry_counter = 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_error_recovery

DESCRIPTION
  Attempts to recovery from DSP errors.

DEPENDENCIES
  voc_state_state_data

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None.

===========================================================================*/
voc_state_return_type voc_state_error_recovery (void)
{
  voc_state_return_type ret_val = VOC_STATE_SUCCESS;

  if (voc_state_fw_err_cnt < voc_state_fw_err_cnt_max) {
    voc_state_fw_err_cnt++;

    MSG_ERROR("fw_err_cnt=%d, max=%d", voc_state_fw_err_cnt,
              voc_state_fw_err_cnt_max, 0);

    MSG_ERROR("acq_m=%d, con_m=%d, mod_s=%d",
              voc_state_data.acquire_mask,
              voc_state_data.config_pending,
              voc_state_data.module_status);

    if (voc_state_data.acquire_mask) {
      if (voc_state_disable_fw_err_recovery == TRUE) {
        MSG_HIGH("DSP error recovery disabled", 0,0,0);
        voc_state_shutdown_due_to_error();
      }
      else {
        if (voc_state_data.num_higher_priority_apps == 0) {
          /* Safe to reset since there are no other higher priority clients */

          qdsp_module_type module_select = QDSP_MODULE_MAX;
		  voc_status_type	module_select_status;

          /* Debounce the error condition by resetting the DSP */
          voc_state_data.state = VOC_STATE_RESET;
          voc_state_data.retry_counter = 0;
          voc_state_data.afe_configured = FALSE;

          if (voc_state_data.module_status ==
              VOC_STATE_MOD_STATUS_DISABLE_PENDING) {
            voc_state_data.module_status =
                                        VOC_STATE_MOD_STATUS_ENABLE_REQUESTED;
            MSG_HIGH("qdsp_disable_event_rsp(%d)",
                     voc_state_data.current_qdsps_app,0,0);
            qdsp_cmd_clear_dsp_isr();
            qdsp_disable_event_rsp(voc_state_data.current_qdsps_app);
          }

          if (voc_state_data.module_status < VOC_STATE_MOD_STATUS_ENABLED) {
            voc_state_data.module_status =
                                        VOC_STATE_MOD_STATUS_ENABLE_REQUESTED;
          }

          module_select_status = voc_state_get_high_module(&module_select);
          switch(module_select_status)
          {
            case VOC_CMD_FAILED_S:
              break;
            case VOC_CMD_BUSY_S:
              {
                MSG_HIGH("Module not supported on current image = %d, "
                         "Continuing with Module = %d",
                         voc_state_data.current_image,
                         voc_state_data.module_reqd,0);
                voc_inform(VOC_INFORM_MODULE_UNFETCHED);
              }
              break;
            default:
              ;
          }
          voc_state_module_commit(module_select);

          MSG_HIGH("qdsp_reset_and_enable(%d,%d)",
                   voc_state_data.current_qdsps_app, module_select,0);
          qdsp_reset_and_enable(voc_state_data.current_qdsps_app,
                                module_select);
          ret_val = VOC_STATE_CONTINUE;
        }
        else {
          /* Can't reset the DSP since other higher priority clients are still
           * using the current firmware image.
           * Just shutdown until another config reawakens the state machine.
           */
          MSG_ERROR("Can't reset DSP since num_higher_priority_apps=%d",
                    voc_state_data.num_higher_priority_apps, 0, 0);
          voc_state_shutdown_due_to_error();
        }
      }
    }
    else {
      /* Since there is no active audio client, let's defer the recovery
       * attempt until the next time someone acquires. This has the
       * advantage of avoiding an unnecessary reset if the next config
       * requires a different image anyways.
       */
      MSG_ERROR("Shutting down since acquire_mask=0", 0,0,0);
      voc_state_shutdown_due_to_error();
    }
  }
  else {
    MSG_ERROR("Shutting down: FW error threshold %d reached",
              voc_state_fw_err_cnt,0,0);
    voc_state_shutdown_due_to_error();
  }
  return ret_val;
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_control

DESCRIPTION
  Drives the QDSP2 into the desired state based on command inputs.

DEPENDENCIES
  Call voc_state_control_init once before any calls to this function.

RETURN VALUE
  None

SIDE EFFECTS
  Changes state of the QDSP2

===========================================================================*/
void voc_state_control (void)
{
  voc_state_return_type  ret_val = VOC_STATE_SUCCESS;

#ifdef FEATURE_QDSP_ERR_LOGS_STORE
#error code not present
#endif

  switch (voc_state_data.state) {

    /* Actions to take on power-up reset, or to recover from errors.
    */
    case VOC_STATE_RESET:
      MSG_MED("STATE_RESET",0,0,0);
      ret_val = voc_state_reset();
      break;

    /* Actions to take when waiting for image download
    */
    case VOC_STATE_IMAGE:
      MSG_MED("STATE_IMAGE",0,0,0);
      ret_val = voc_state_image ();
      break;

    /* Actions to take on entry to INIT state
    */
    case VOC_STATE_SLEEP:
    case VOC_STATE_INIT_ENTRY:
      MSG_MED("STATE_SLEEP",0,0,0);
      ret_val = voc_state_sleep ();
      break;

    /* Actions to take when in INIT state
    */
    case VOC_STATE_INIT:
      MSG_MED("STATE_INIT",0,0,0);
      ret_val = voc_state_init();
      break;

    /* Actions to take on entry to IDLE state
    */
    case VOC_STATE_IDLE_ENTRY:
      MSG_MED("STATE_IDLE_ENTRY",0,0,0);
      ret_val = voc_state_idle_entry();
      break;

    case VOC_STATE_IDLE:
      MSG_MED("STATE_IDLE",0,0,0);
      ret_val = voc_state_idle();
      break;

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
    case VOC_STATE_MIXER_INIT:
      MSG_MED("STATE_MIXER_INIT",0,0,0);
      ret_val = audadsp_mixer_state_init();
      break;

    case VOC_STATE_MIXER_IDLE_ENTRY:
      MSG_MED("STATE_MIXER_IDLE_ENTRY",0,0,0);
      ret_val = audadsp_mixer_state_idle_entry();
      break;

    case VOC_STATE_MIXER_IDLE:
      MSG_MED("STATE_MIXER_IDLE",0,0,0);
      ret_val = audadsp_mixer_state_idle();
      break;

    case VOC_STATE_MIXER_ACTIVE_ENTRY:
      MSG_MED("STATE_MIXER_ACTIVE",0,0,0);
      ret_val = audadsp_mixer_state_active_entry();
      break;

    case VOC_STATE_MIXER_ACTIVE:
      MSG_MED("STATE_MIXER_ACTIVE",0,0,0);
      ret_val = audadsp_mixer_state_active();
      break;
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

    /* Actions to take on entry to VOCODER state
    */
    case VOC_STATE_VOCODER_ENTRY:
      MSG_MED("STATE_VOCODER_ENTRY",0,0,0);
      ret_val = voc_state_vocoder_entry();
      break;

    /* Actions to take when in VOCODER state
    */
    case VOC_STATE_VOCODER:
      MSG_MED("STATE_VOCODER",0,0,0);
      ret_val = voc_state_vocoder();
      break;

#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */

#ifdef FEATURE_VOC_ADPCM
    case VOC_STATE_ADPCM_VOCODER_ENTRY:
      MSG_MED("STATE_ADPCM_VOCODER_ENTRY",0,0,0);
      ret_val = voc_state_adpcm_vocoder_entry();
      break;

    case VOC_STATE_ADPCM_VOCODER:
      MSG_MED("STATE_ADPCM_VOCODER",0,0,0);
      ret_val = voc_state_adpcm_vocoder();
      break;
#endif /* FEATURE_VOC_ADPCM */

#ifdef FEATURE_AVS_INCALL_ADEC_PB
    case VOC_STATE_ADEC_VOCODER_ENTRY:
      MSG_MED("STATE_ADEC_VOCODER_ENTRY",0,0,0);
      ret_val = voc_state_adec_vocoder_entry();
      break;

    case VOC_STATE_ADEC_VOCODER:
      MSG_MED("STATE_ADEC_VOCODER",0,0,0);
      ret_val = voc_state_adec_vocoder();
      break;
#endif /* FEATURE_AVS_INCALL_ADEC_PB */

#ifdef FEATURE_VOC_G711
    case VOC_STATE_WAV_VOCODER_ENTRY:
      MSG_MED("STATE_ADPCM_VOCODER_ENTRY",0,0,0);
      ret_val = voc_state_wav_vocoder_entry();
      break;

    case VOC_STATE_WAV_VOCODER:
      MSG_MED("STATE_ADPCM_VOCODER",0,0,0);
      ret_val = voc_state_wav_vocoder();
      break;
#endif /* FEATURE_VOC_G711 */

    case VOC_STATE_VOC_TWO_DECODERS:
      MSG_MED("STATE_NOP",0,0,0);
      ret_val = voc_state_nop();
      break;

#ifdef FEATURE_QSYNTH_COMMON
    case VOC_STATE_QSYNTH_INIT:
      MSG_MED("STATE_QSYNTH_INIT",0,0,0);
      ret_val = voc_state_qsynth_init();
      break;

    case VOC_STATE_QSYNTH_ENTRY:
      MSG_MED("STATE_QSYNTH_ENTRY",0,0,0);
      ret_val = voc_state_qsynth_entry();
      break;

    case VOC_STATE_QSYNTH:
    case VOC_STATE_QSYNTH_DISABLE_PENDING:
      MSG_MED("STATE_QSYNTH",0,0,0);
      ret_val = voc_state_qsynth();
      break;
#endif

#ifdef FEATURE_QTUNES_COMMON
    case VOC_STATE_QTUNES_INIT:
      MSG_MED("STATE_QTUNES_INIT",0,0,0);
      ret_val = voc_state_qtunes_init();
      break;

    case VOC_STATE_QTUNES_CONFIG:
      MSG_MED("STATE_QTUNES_CONFIG",0,0,0);
      ret_val = voc_state_qtunes_config();
      break;

    case VOC_STATE_QTUNES_ENTRY:
      MSG_MED("STATE_QTUNES_ENTRY",0,0,0);
      ret_val = voc_state_qtunes_entry();
      break;

    case VOC_STATE_QTUNES:
    case VOC_STATE_QTUNES_DISABLE_PENDING:
      MSG_MED("STATE_QTUNES",0,0,0);
      ret_val = voc_state_qtunes();
      break;
#endif

#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
    case VOC_STATE_CAM_AUDIO_INIT_ENTRY:
      MSG_MED("VOC_STATE_CAM_AUDIO_INIT_ENTRY",0,0,0);
      ret_val = voc_state_cam_audio_init_entry();
      break;
    case VOC_STATE_CAM_AUDIO_INIT:
      MSG_MED("VOC_STATE_CAM_AUDIO_INIT ",0,0,0);
      ret_val = voc_state_cam_audio_init();
      break;
    case VOC_STATE_CAM_AUDIO_CONFIG:
      MSG_MED("VOC_STATE_CAM_AUDIO_CONFIG ",0,0,0);
      ret_val = voc_state_cam_audio_config();
      break;
    case VOC_STATE_CAM_AUDIO_ENTRY:
      MSG_MED("VOC_STATE_CAM_AUDIO_ENTRY ",0,0,0);
      ret_val = voc_state_cam_audio_entry();
      break;
    case VOC_STATE_CAM_AUDIO_ACTIVE:
      MSG_MED("VOC_STATE_CAM_AUDIO_active ",0,0,0);
      ret_val = voc_state_cam_audio_active();
      break;
#endif /* FEATURE_AUDIO_CAMERA_CONCURRENCY */

#ifdef FEATURE_GRAPH_AUDIO
    case VOC_STATE_GRAPH_INIT:
      MSG_MED("STATE_GRAPH_INIT",0,0,0);
      ret_val = voc_state_graph_init();
      break;

    case VOC_STATE_GRAPH_IDLE_ENTRY:
      MSG_MED("STATE_GRAPH_IDLE_ENTRY",0,0,0);
      ret_val = voc_state_graph_idle_entry();
      break;

    case VOC_STATE_GRAPH_IDLE:
      MSG_MED("STATE_GRAPH_IDLE",0,0,0);
      ret_val = voc_state_graph_idle();
      break;

    case VOC_STATE_GRAPH_AUDIO_ENTRY:
      MSG_MED("STATE_GRAPH_AUDIO_ENTRY",0,0,0);
      ret_val = voc_state_graph_audio_entry();
      break;

    case VOC_STATE_GRAPH_AUDIO:
      MSG_MED("STATE_GRAPH_AUDIO",0,0,0);
      ret_val = voc_state_graph_audio();
      break;
#endif /* FEATURE_GRAPH_AUDIO */

    case VOC_STATE_ERROR_RECOVERY:
      MSG_ERROR("STATE_ERROR_RECOVERY",0,0,0);

#ifdef FEATURE_QDSP_ERR_LOGS_STORE
#error code not present
#endif
      ret_val = voc_state_error_recovery();
      break;
    }

  /* If more state transitions are necessary.
  */
  if ((ret_val == VOC_STATE_CONTINUE) ||
      (ret_val == VOC_STATE_CONTINUE_IMMEDIATE)) {
     (void) rex_set_timer( &voc_state_timer,
                           voc_state_get_transition_duration(ret_val) );
  }

} /* voc_state_control */

#ifdef FEATURE_AVS_INCALL_MIXER_CMX
/*===========================================================================

FUNCTION voc_state_incall_synth_ready

DESCRIPTION
  This function verifies that incall synth is in ready state

DEPENDENCIES


RETURN VALUE
  boolean value indicates synth is ready or not

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_state_incall_synth_ready() {
  if (
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
      (voc_state_data.current_image == QDSP_IMAGE_VOC_COMBO_SYNTH) &&
#endif /* QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */
      (audadsp_mixer_adec_ready() == TRUE)) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}
#endif /* FEATURE_AVS_INCALL_MIXER_CMX */
#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
/*===========================================================================

FUNCTION voc_state_get_clocks_on

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean voc_state_get_clocks_on (void)
{
  return (voc_state_clocks_on);
}

/*===========================================================================

FUNCTION voc_state_set_clocks_on

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void voc_state_set_clocks_on (boolean set)
{
  voc_state_clocks_on = set;
}

#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
#if defined(FEATURE_ARM_POST_PROCESSING) \
    || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
/*===========================================================================

FUNCTION voc_state_get_pp_status

DESCRIPTION
  Gets the PP status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
voc_pp_status_type voc_state_get_pp_status(void)
{
  return voc_state_pp_status;
}
#endif /* FEATURE_ARM_POST_PROCESSING ||FEATURE_AVS_WB_PCM_RX_INTERFACE */

/*===========================================================================

FUNCTION voc_state_clear_fw_err_cnt

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void voc_state_clear_fw_err_cnt (void)
{
  /* Clear any previous error condition */
  if (voc_state_fw_err_cnt < voc_state_fw_err_cnt_max) {
    voc_state_fw_err_cnt = 0;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION voc_state_control_init

DESCRIPTION
  Initialize the state controller

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void voc_state_control_init (void)
{
  /* Initialize timer */
#ifndef FEATURE_HWTC
  rex_def_timer( &voc_state_timer, &voc_tcb, VOC_CHECK_STATE_SIG );
  rex_def_timer( &voc_state_gp_timer, &voc_tcb, VOC_STATE_GP_SIG );
#endif /* FEATURE_HWTC */

#ifndef MSMAUD_ADIE_CODEC
  /* Initialize Clock Services call-back data structures */
  
  #ifdef FEATURE_CMI
#error code not present
  #else
  clk_def( &voc_state_adie_clk_cb );
  #endif

  adie_services_init();
#endif /* MSMAUD_ADIE_CODEC */

#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#ifdef FEATURE_NEW_SLEEP_API
  voc_state_sleep_handle = sleep_register(voc_state_sleep_str,
                                          TRUE);
#ifdef FEATURE_AVS_BUS_CLK_CTRL
#ifndef T_QSC1110
    sleep_set_hclk_restriction(voc_state_sleep_handle,CLKRGM_HCLK_TCXO);
#else
    sleep_set_hclk_restriction(voc_state_sleep_handle,CLKRGM_HCLK_OFF);
#endif /* T_QSC1110 */
#endif /* FEATURE_AVS_BUS_CLK_CTRL */

#endif /* FEATURE_NEW_SLEEP_API  */
#endif /* T_SLEEP || FEATURE_SLEEP_TASK */

#if defined(MSMAUD_ADSP_CLK_GATING)
  /* Turn off adsp macro clock to save power when adsp becomes idle */
  MSMAUD_ADSP_CLK_GATING_ENA();
#endif /* MSMAUD_ADSP_CLK_GATING */

  /* Initial state is RESET */
  voc_state_data.state = VOC_STATE_RESET;

  voc_state_data.current_qdsps_app = QDSP_APP_VOC;

  /* Initialize image setting */
  voc_state_data.current_qdsps_app = QDSP_APP_VOC;
  voc_state_data.module_reqd       = QDSP_MODULE_MAX;
  voc_state_data.module_status     = VOC_STATE_MOD_STATUS_DISABLED;
  voc_state_data.image_changed     = FALSE;
  voc_state_data.current_image     = QDSP_IMAGE_MAX;

  /* Indicate first time up */
  voc_state_data.reset_flag = TRUE;

  /* Get QDSP command to register with QDSP Services
  */
  qdsp_cmd_register (
    QDSP_CMD_QDSPS_CLIENT_LOW,
    QDSP_APP_VOC,
    voc_state_qdsp_event_handler,
    (QDSP_EV_COMMAND | QDSP_EV_MODULE | QDSP_EV_IMAGE_CHANGE)
  );

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

  /* No state transition retries have been attempted yet
  */
  voc_state_data.retry_counter = 0;

#ifdef FEATURE_PHONE_VR
  voc_state_data.vr_mode      = VOC_VR_MODE_OFF;
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_TTY
  voc_state_data.tty_mode     = VOC_TTY_MODE_OFF;
#endif /* FEATURE_TTY */

#ifdef FEATURE_VOICE_RECORD
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#endif /* FEATURE_VOICE_RECORD */

#ifdef FEATURE_QSYNTH_COMMON
  voc_state_data.hybrid_mode_pending = 0;
  voc_state_data.qsynth_vol_mode_pending = 0;
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QSYNTH_ADPCM
  voc_state_data.adec_mode_pending = VOC_QSYNTH_ADEC_MODE_QCP;
#endif /* FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_VOC_PCM_INTERFACE
  voc_state_data.pcm_int_mode         = 0;
  voc_state_data.pcm_int_mode_pending = 0;
#endif /* FEATURE_VOC_PCM_INTERFACE */

#ifdef FEATURE_AAC
  voc_state_data.aac_config.buffer_factor      = 50;
  voc_state_data.aac_config.aac_format         = VOC_AAC_DATA_FORMAT_ADIF;
  voc_state_data.aac_config.sample_freq_index  = 11;  /* 8Khz sample rate*/
  voc_state_data.aac_config.audio_object       = 2;
  voc_state_data.aac_config.channel_configuration                = 0;
  voc_state_data.aac_config.ep_config                            = 0;
  voc_state_data.aac_config.aac_section_data_resilience_flag     = 0;
  voc_state_data.aac_config.aac_scalefactor_data_resilience_flag = 0;
  voc_state_data.aac_config.aac_spectral_data_resilience_flag    = 0;
#ifdef FEATURE_AAC_REC
  voc_state_data.aac_config.bit_per_spl                          = 0;
#endif /* FEATURE_AAC_REC */
#endif /* FEATURE_AAC */

#ifdef FEATURE_REAL_AUDIO
  voc_state_data.real_audio_config.decoder_mode      = 1;
  voc_state_data.real_audio_config.sample_rate       = 1; /* 8Khz sample rate*/
  voc_state_data.real_audio_config.num_channels      = 1;
  voc_state_data.real_audio_config.samples_per_frame = 512;
  voc_state_data.real_audio_config.num_regions       = 24;
  voc_state_data.real_audio_config.bits_per_frame    = 512;
  voc_state_data.real_audio_config.coupling_start    = 0;
  voc_state_data.real_audio_config.coupling_quant    = 0;
  voc_state_data.real_audio_config.frame_erasure     = 0;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
  voc_state_data.wma_config.version          = 0x2;
  voc_state_data.wma_config.channels         = 0x2;
  voc_state_data.wma_config.bytes_per_second = 0x4E25;
  voc_state_data.wma_config.sample_rate      = 0x1;   /* 8Khz sample rate*/
  voc_state_data.wma_config.encoder_options  = 0xF;
  voc_state_data.wma_config.format_tag       = 0x0;
  voc_state_data.wma_config.bits_per_sample  = 0x0;
  voc_state_data.wma_config.channel_mask     = 0x0;
  voc_state_data.wma_config.advanced_encoder_opt2=0x0;
  voc_state_data.wma_config.advanced_encoder_opt1 =0x0;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
  voc_state_data.amr_wbp_config.sample_rate   = 16000;
  voc_state_data.amr_wbp_config.dec_chan_mode = 0;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

#ifdef FEATURE_QTUNES_COMMON
  voc_state_data.tunes_config_pending = FALSE;
#endif /* FEATURE_QTUNES_COMMON */

#ifdef FEATURE_MM_REC
  voc_state_data.mm_rec_config_pending = FALSE;
#endif /* FEATURE_MM_REC */

#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
  voc_state_data.snd_config = VOC_CAP_NONE;
  voc_state_data.voc_config = VOC_CAP_NONE;
#endif
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
  /* Don't think we needt it any more */
  /*voc_state_data.mixer_vocoder_active_flag = FALSE;
  voc_state_data.mixer_audio_active_flag = FALSE; */
#endif

  /* set that slowtalk STATIC parameters are not updated,
     so next time when voice call starts, it will be updated
  */
  voc_data_set_slowtalk_static_params_status(
                  VOC_SLOWTALK_STATIC_PARAMS_NOT_UPDATED);

  voc_state_data.codec_pending = FALSE;
  voc_state_data.afe_configured = FALSE;

  voc_state_data.test_mode_pending = VOC_TEST_NONE;
  voc_state_data.test_mode         = VOC_TEST_NONE;
} /* voc_state_control_init */


#ifdef FEATURE_OEMSBL_AUDIBLE_TONE
/*===========================================================================

FUNCTION voc_state_set_oemsbl_cb

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
void voc_state_set_oemsbl_cb(voc_oemsbl_cb_func_ptr_type func_ptr)
{
  voc_state_oemsbl_func_ptr = func_ptr;
}
#endif

/*===========================================================================

FUNCTION voc_state_set_pa_on_cb

DESCRIPTION
Setting voc_state_pa_on_cb by the function pointer given

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_state_set_pa_on_cb(voc_pa_on_cb_func_ptr_type func_ptr)
{
  /* Setting voc_state_pa_on_cb by the function pointer given*/
  voc_state_pa_on_cb = func_ptr;
}

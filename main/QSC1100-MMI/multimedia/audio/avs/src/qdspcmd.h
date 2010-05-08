#ifndef QDSPCMD_H
#define QDSPCMD_H
/*===========================================================================

                   Q D S P    D E V I C E   D R I V E R
         C O M M A N D   I N T E R F A C E   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions for functions and data used to send
  commands to the QDSP, receive messages from the QDSP, and perform DMA
  reads and writes to QDSP memory. These definitions are internal to the
  QDSP Device Driver.

REFERENCES
  None.

Copyright(c) 1999 - 2009 by QUALCOMM, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/qdspcmd.h#25 $ $DateTime: 2009/05/23 05:35:39 $ $Author: sudhirk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/09   sud	   Removed MSMAUD_QSC6270_FFA_AUDIO_CAL and
                   MSMAUD_QSC6270_SURF_AUDIO_CAL,& add MSMAUD_QSC1110_AUDIO_CAL
                   Updated Copyright.
05/22/09   nr	   Removed MSMAUD_QSC6270_FFA_AUDIO_CAL and
                   MSMAUD_QSC6270_SURF_AUDIO_CAL,& add MSMAUD_QSC6270_AUDIO_CAL
04/06/09   av      Adding VP3 Feature.
03/10/09   sj      Added slowtalk feature support
01/19/09    bk     Featurized the declarations of functions
                   qdsp_cmd_adec_chan_*  (). Fixed the linking errors arised
                   while featurizing for FEATURE_STD_MIDI.
12/30/08    rm     Adding fluence parameters
12/08/08    rm     Modification of wnr and rve parameters
12/01/08    ry     Fixed Thin UI compilation errors.
11/26/08   skr     Modified the code to fix ThinUI build issues.
11/24/08   skr     Added code for Audio + camera Concurrency
10/17/08   vm      added idlemetring support for qtv image
04/09/08   av      Added prototypes for externalized functions  qdsp_cmd_dual_rve_param and  qdsp_cmd_rve_param
29/08/08   av      Assigned dummy values to I2S macro definitions to avoid compilation errors.
28/08/08   rm	   Added support for RVE.
08/28/08    av     Included dummy I2S macro definitions to avoid compilation errors
                   for targets other than QSC6270 and QSC60X5.
08/27/08    ry     Added code to allocate the pp buffer pool dynamically and modified
                   functions to return the status if malloc fails.
08/26/08   anb     Added support for DSP based WMAPRO
08/07/08    av     Added support for CrystalSpeech using EXTERNAL_SADC
07/11/08   skr     Changes to release VOCODER & ADEC separately during InCall.
07/17/08    kk     Added support to play Qtv 4gv nb clips.
05/28/08   knm     Added I2S support for 60x5.
05/14/08    sr     Created qdsp_cmd_pcm_sample_rate_type.Modified qdsp_cmd_
                   codec_config() to include default_frame_rate for PCM
                   sampling. Support for 16khz PCM sampling.Fix for CR 134407.
04/02/08   vs      Added I2S out support for LCU.
03/17/08  ymc/anb  Modified codec gain ctrl api to control gain changes.
                   Fix for SC2X codec pop issue through MSMAUD_PCM_DC_OFFSET feature.
03/11/08    kk     Added support for sending mp3 configuration
                   information for DSP.
02/07/08   mas     Made coeffs of qdsp_cmd_audfmt_iir_filter_type read and
                   writable when AUDIO_CALIBRATION feature enabled
02/07/08    kk     Added support for Qensemble phase 2 interface changes.
01/31/08    pl     Added qdsp_cmd_qsynth_emulate_isr() function prototype.
01/15/08    hs     Added support ISDB-T dual mono output control
01/07/08    az     Included the in call midi and the new in call architecture.
12/14/07    ss     Merged the fix for Bad Quality in MIDI+ADPCM/QCP in PMD
                   files. Fixed CR_122182.
11/14/07    hs     Added QEnsemble support
09/03/07   ymc     Corrected order of vcodec_len0 and vcodec_delay1.
08/27/07   ymc     Added Crystal Speech support.
08/27/07  sg/wfd   Remove obsolete #else code for MSMAUD_MSM6050_CODEC_CONFIG
                   and MSMAUD_AUX_PCM_SDAC_SHARED (for older targets).
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
07/02/07    pl     Added qdsp_cmd_reset_adec_cmd_buf() function prototype.
05/10/07    sp     Added support for AUX PCM primary PCM mode.
04/20/07   sp/anb  Added AUX PCM definitions for the SC2X hardware.
04/07/07    pl     Added qsynth AMR-NB support.
03/15/07    kan    Added 4GV-WB Code
02/27/07   ymc     Renamed QDSP_CMD_EC_GEN_LEGEND as QDSP_CMD_EC_GEN_LEGACY.
02/26/07   kan     Added AMR-WB changes
02/09/07   act     Added qdsp_cmd_tty_signal_discont().
02/08/07    ay     Renamed EC interface variables.
01/23/07    kk     Fixed issues related to adding 44k sample rate.
01/23/07   suk     Added new TTY interface type that facilitates qdspcmd
                   function to know the available TTY interface.
                   Updated copyright text.
12/07/06   ymc     Fixed AUX PCM clock loss due to DSP image swap.
12/02/06   suk     Added new Vocoder type for 4GV VoIP and EVRC VoIP.
11/28/06    sp     Fixed SCO rework issues for QCP files.
09/06/06    ak     Changed the arguments for qdsp_cmd_set_pcm_modes() and added qdsp_cmd_pcm_client_type .
09/05/06    ay     Added AMR-WB+ support.
08/29/06    hz     Added support for incall MP3 on 65nm platforms.
07/31/06   anb     modified the QDSP_playback_enum_type to support BT apps team requirement
                   of variating between a HW/DTMF ringer on FFA
07/27/06   anb     Added the FEATURE_AVS_BT_SCO_REWORK which enables the AVS to send
                   messages to the BT driver
07/26/06  nm/ymc   Added FEATURE_AVS_NEXTGEN_EC.
07/12/06    hz     Added prototype of qdsp_cmd_do_spectrum_analyzer.
06/27/06    pl     Added source & environment enable/disable & reset support.
06/26/06    pl     Added new data structures & function definition for
                   doppler, reverb, and new ping-pong buffer data
                   writting scheme.
05/31/06    sg     Re-featureized synth MIDI functions so that they are not
                   under FEATURE_ADEC.
05/24/06    ay     Fixed I2SSBC featurization issues.
04/26/06   act     Added support for combo vocoder image.
04/03/06    ak     Added support for sidetone control.
03/24/06    pl     Seperated FIR filter from FEATURE_QSYNTH_ADRC and
                   FEATURE_QTUNES_ADRC.
03/15/06   ymc     Fixed compilation issue when FEATURE_MM_REC is enabled.
03/13/06    lg     Added API for dereg MIDI command callback.
03/13/06    st     Added support for delayed start of ADEC channels with synth
                   image; added support for query of 3D positional source
                   parameters at the end of playback.
03/10/06    sp     Fixed a featurization issue with SDAC
02/21/06   act     Addeed QDSP_RX_MUTE_CONDITIONAL_V.
02/17/06    ay     Added image check for voc_adpcm module.
02/03/06    sm     Added 4GV VOIP support
01/31/06    sg     Fixed featurization for incall MP3.
01/31/06    hz     Added support for in-call mp3 feature.
01/30/06    sp     Added support for I2S SBC interface.
01/16/06    sm     Added FEATURE_QDJ_TIMEWARP
10/19/05    pl     Added PCM AVSync support.
09/18/05    sp     Added support for DSP based SBC encoder.
09/18/05   pl/st   Added support for QAudioFX: 3D positional commands and query
                   interface; modified code to support new game audio ADEC
                   API; refeaturized qdsp_cmd_pp_free_read_buffer().
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
09/12/05    hz     Added AV sync support for windows media.
08/31/05    sm     Added vox control support.
08/11/05    hz     Added support for DTX and data requrest time for AMR
                   recording.
08/04/05    sp     Added support for mixing of qcp audio and voice call.
07/28/05    sm     G723 fixes.
07/22/05    sm     Added Wav A/ULaw support.
07/05/05    sm     Added G723.1 support.
06/23/05    sm     TTY updates for auto-bypass and VCO/HCO
06/13/05    sm     Added QDSP_TX_MUTE_GAIN.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/24/05    hz     Added support for MultiMedia/AAC record functionality.
05/18/05    ac     Modified code to support new AV sync API.
05/16/05    hz     Added support for Vocoder image+ADPCM+VFE.
05/04/05    aw     Replace PCM filter with IIR filter support for Qsynth.
04/28/05   ymc     Added 4GV NB support.
04/21/05    hz     Added VirtualPacketLength support for Windows Media
                   playback.
04/01/05    hz     Added support for Enhanced AAC Plus decoder.
03/25/05   ac/sg   Added support for A/V synchronization.
03/02/05    aw     Added support for Qconcert, stf; added pp for Qsynth, mp3.
02/22/05    sm     Added G.711 vocoder support.
02/15/05   act     Added support for EC parameter calibration structure.
02/15/05    sm     Added FEATURE_IIR_FILTER
02/14/05    hz     Added support for WMA decoder.
02/02/05    st     Modified qdsp_cmd_dls_config_type to support HQ DLS.
12/29/04    sp     Added a return type to qdsp_cmd_register_pp_cb() prototype
12/03/04    sm     Added FEATURE_VOC_DTMF_DETECT, removed FEATURE_USB_ISOC_AUDIO
11/19/04    sp     Added support for Arm Post Processing.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
08/11/04    st     Added qdsp_cmd_midi_cmd_type structure definition; updated
                   qdsp_cmd_midi() function prototype.
07/08/04    aw     Added prototype for qdsp_cmd_set_aac_plus_on_flag().
06/08/04    aw     Added support for Equalizer and Spectrum analyzer.
05/11/04    vp     Merged ADRC and PCM filter support MP3/AAC from
                   MSMSHARED_AVS.00.09.05.02.00. Added support for H263 images.
            sg     Added qdsp_cmd_set_adec_int_cb().
05/05/04    sm     Added qdsp_cmd_gsm_afe_type, qdsp_cmd_set_gsm_afe().
03/22/04    st     Added definitions for PCM playback on QSynth image; updated
                   qdsp_cmd_pause_midi_seq() and qdsp_cmd_set_vol_midi_seq()
                   function prototypes.
                   Added qdsp_cmd_pause_midi_seq(), qdsp_cmd_set_vol_midi_seq();
                   removed qdsp_cmd_seq_vol(); updated qdsp_cmd_adec_pause(),
                   qdsp_cmd_adec_chan_pause().
03/04/04    sg     Added prototype for qdsp_cmd_restore_comfort_noise.
01/26/04    lg     Added support for multi-sequencer. audioPause is now an array
01/21/04    aw     Modified code to support qsynth volume mapping function.
12/08/03    sg     Modified to support changes in the vocoder PCM interface.
12/02/03    sg     Added support for linear PCM playback.
11/22/03    sg     Added support for RealAudio codec.
10/16/03    aw     Modified code to support Yamaha ADPCM and SMAF.
10/10/03    sg     Added support for additional AAC data formats.
            aw     Added MSMAUD_VOC_AMR_COMMON featurization.
08/26/03    sg     Replaced qdsp_cmd_check_valid_rates() with
                   qdsp_cmd_set_rec_frame_rates().
            sg     Modified to support high and low priority vocoder clients
                   (vis-a-vis QDSP Services).
            st     Added qdsp_cmd_check_valid_rates() function prototype.
            st     Added support for vocoder select command.
05/13/03   ymc     Added half rate support.
           ymc     Replaced FEATURE_GSM and FEATURE_WCDMA with MSMAUD_VOC_*.
           ymc     Removed WVS support.
04/18/03    aw     Added audio support for Q3D image.
04/17/03    st     Fixed naming of label.
04/17/03    sm     Updated AMR uplink func for new MVS; added
                   QDSP_AUDIO_MODE_AMR.
            sg     Added fm_sat_dc_offset to qdsp_cmd_dfm_config_type.
                   Added support for vocoder PCM interfaces.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface.
03/10/03    st     Refeaturized code to support qdsp_cmd_adec_pause() for ADPCM.
            sm     Added HR enums.
02/19/03    st     Refeaturized code to support ADPCM; added support for
                   ADPCM in graphics images.
01/25/03    st     Fixed DMA path settings for MSM6100.
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
01/05/03    sg     Updated DMA path control values for MSM6100.
12/03/02    st     Modified code to add support for audio for graphics images.
11/19/02    st     Added qdsp_cmd_synth_mvol() and qdsp_cmd_adec_mvol()
                   function prototypes.
11/18/02    st     Featurized GSM/WCDMA specific code.
11/06/02    sm     Added QCP AMR support.
09/09/02   ymc     Added FEATURE_MVS.
09/05/02    sm     Added support for BT AG in 6200.  Added support for
                   MSMAUD_PCM_FORMAT_DAI
08/23/02    sg     Added support for DSP_IMAGE_VOC_EVRC_DEFINED; modified to
                   use FEATURE_EXTERNAL_SDAC.
08/22/02    sg     Added support for TTY activity status indication.
                   (Merged from MSM_AVS.01.05.18.01.02).
08/13/02   aw/st   Added support for ADPCM audio decoder.
           sg/st   Added support for MP4 and AAC.
07/09/02    sm     Synced to perforce tip.
            sm     Rearrange 6200 DMA path control constants.
            sm     Const FIR coeff for ADRC.  Use 6200 DMA PATH CONTROL.
           ymc     Renamed QDSP_AMR_SPEECH_PRO_DEGRADED as
                   QDSP_AMR_SPEECH_DEGRADED, QDSP_AMR_SPARE as QDSP_AMR_ONSET.
03/21/02    st     Added support for MSMAUD_CODEC_CLOCK_CTRL.
03/12/02    st     Obsoleted FEATURE_NEW_SYNTH; added MSM6050 defines.
03/04/02   jkl     Added FEATURE_VR_PUFE.
01/15/02   ymc     Added GSM and WCDMA support.
           ymc     Updated copyright.
12/11/01    aw     Added defines for MSM6050 codec config command.
            ro     Use msmaud.h to eliminate targetization.
10/29/01    sg     Added packet interface support.
10/01/01    st     Modified qdsp_cmd_adec_reset(), qdsp_cmd_adec_flush(),
                   qdsp_cmd_adec_chan_vol(), qdsp_cmd_adec_chan_pan() and
                   qdsp_cmd_adec() function prototypes to support multiple
                   ADEC channels.
10/01/01    sm     Added FEATURE_BT_AG_LOCAL_AUDIO.
09/11/01    st     Modified qdsp_cmd_qsynth_adrc_ctl_type to reflect changes
                   in QSynth ADRC API.
09/04/01    aw     Added FEATURE_QSYNTH_ADRC. Added prototypes for QSynth PCM
                   filter and ADRC configuration.
09/03/01    st     Modified code to return record direction in record callback.
07/24/01   ymc     Externalized function qdsp_cmd_up_packet_loopback.
                   Added support for loopback tests.
06/18/01  jkl/ym   Added FEATURE_ACP. Replaced FEATURE_DFM_ANSWER_MACHINE.
05/21/01   jkl     Removed Enhanced VR feature.
05/18/01    sm     Added TTY support for MSM5100.
04/26/01    sm     Added FEATURE_USB_ISOC_AUDIO support.
                   Removed qdsp_cmd_adec_int_speed(). Updated ADEC_ERROR values.
04/04/01    st     Modified code to support new FEATURE_MIDI_OUT feature; added
                   qdsp_cmd_adec_reset() prototype.
02/21/01    vl     Added MSM5100 support.
02/06/01   jkl     Removed featurization on Mic1 amp. gains.
01/26/01    st     Modified QDSP_VBAP_AMP_SEL_M for PPG to include AUX amp.
12/10/00    st     Removed unnecessary featurization.
12/06/00   jkl     removed set ISR functions.
12/05/00   jkl     Merged VR4.0 functions. Added FEATURE_ENHANCED_VR,
12/04/00    sm     Added protos for master_volume(), master_pan(), adec_error(),
                   adec_int_speed(), mp3_target_header(), adec_chan_pan(),
                   adec_chan_vol().  Added adec error list.
          st/sm    Merged ADEC support, separated QDSP2 ISR routines.
10/13/00   ymc     Added MSM5105 support; T_MSM33 as T_MSM33 && !T_MSM5105;
                   !T_MSM33 as !T_MSM33 || T_MSM5105.
10/04/00   ymc     Added macro definitions to enable output amplifiers for
                   handset and headset.  These macro definitions are featurized
                   under MSMHW_CONCURRENT_VBAP_AMPS.
09/25/00    sm     Fix 8khz mode bit defines.
09/18/00    sm     Added SDAC DMA path, PCM mode for MIDI vocoder image.
08/28/00    ro     Uses QDSP Services.
                   Added qdsp_cmd_register and qdsp_cmd_set_isr.
                   Removed qdsp_cmd_sw_reset and qdsp_cmd_valid_addr.
                   Removed qdsp_cmd_clear_filter_load.
08/24/00   spf     Updated codec configuration command for PPG.
            sm     Added UART2 bits for PPG.
08/16/00    ro     Updated qdsp_cmd_diag_type to remove un-supported variables.
                   Removed qdsp_cmd_reset_mips.
                   Update recording and capture function headers.
06/30/00   spf     Added qdsp_cmd_nes_ctrl().
06/16/00    sm     avcRlinkSensitivityOffset is a 16-bit word.
06/11/00   spf     Updated references.
05/30/00    st     Added definitions for codec config command on QSynth image.
05/05/00    st     Added support for stereo DAC and QSynth image.
05/02/00   spf     Added speakerphone echo canceller support.
            sm     Added qdsp_cmd_hpf_ctrl().
03/28/00    sm     AAGC mainline.
01/26/00    tc     Added VR Mode A support.
12/07/99   spf     Added qdsp_cmd_valid_addr().
12/01/99   spf     Run-time image selection support.
                   Merged with 1.17.1.0: Fixed write to agc sensitivity.
11/22/99   jkl     Added new AGC control table parameters.
10/28/99   spf     Fixed MIC1_GAIN bit definitions.
10/18/99    st     Featurized audio AGC/AVC feature.
10/14/99    tc     Added qdsp_cmd_clear_filter_load()
10/14/99   jkl     Added QDSP_LOOPBACK_INIT and QDSP_CODEC_RES_DIS_V
10/07/99   spf     Added qdsp_cmd_clear_sat_dcc()
09/09/99    tc     Add internal codec gain control structure
09/02/99    tc     Add dsp reset and firmware download value
09/02/99    tc     Add qdsp software reset function
09/02/99    tc     Fix rx mute enable value
08/30/99    tc     Add get dfm sat dcc value function
08/26/99    tc     Pink Panther revison 2 changes
07/30/99    tc     Fix AUX Clock Enable value
07/19/99    tc     Update code review comments
07/15/99    tc     DFM support
06/17/99    tc     Initial Version

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"  /* Customer Specific Definitions        */
#include "comdef.h"    /* Definitions for byte, word, etc.     */
#include "msmaud.h"    /* MSM HW interfaces for audio          */
#include "qdsp.h"      /* QDSP Services                        */
#ifdef FEATURE_AUDIO_FORMAT
#include "qw.h"
#endif
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */

/* To build a version for use in dynamic calibration define this symbol.
** When defined, the volume tables will be placed in RAM where values can
** be modified using the DM.
*/
#ifdef FEATURE_AVS_DYNAMIC_CALIBRATION
#define CAL_MEMORY
#else
#define CAL_MEMORY   const     /* Normally, tables are in ROM  */
#endif
/* <EJECT> */
/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* Return values for QDSP2 Command Layer Functions
*/
typedef enum {
  QDSP_CMD_SUCCESS = 0,
  QDSP_CMD_FAILED,
  QDSP_CMD_TIMEOUT,
  QDSP_CMD_WINDOW_ERR
  /* More status codes as needed */
} qdsp_cmd_status_type;

/* <EJECT> */
/*===========================================================================

             Q D S P   B I O S   S Y S T E M   C O M M A N D S

===========================================================================*/

/* Bit mask values for Loopback Test Command
*/
#define QDSP_LOOPBACK_INIT      0x0000 /* Loop back Initialization            */
#define QDSP_LB_AUTO_PKT_ENA_M  0x0010 /* Auto Packet Loopback control mask   */
#define QDSP_LB_AUTO_PKT_ENA_V  0x0010 /* Auto Packet Loopback Enable         */
#define QDSP_LB_AUTO_PKT_DIS_V  0x0000 /* Auto Packet Loopback Disable        */
#define QDSP_LB_DFM_ENA_M       0x0008 /* Mask to control DFM Loopback        */
#define QDSP_LB_DFM_ENA_V       0x0008 /* Value to enable DFM Loopback        */
#define QDSP_LB_DFM_DIS_V       0x0000 /* Value to disable DFM Loopback       */
#define QDSP_LB_PACKET_ENA_M    0x0004 /* Mask to control Packet Loopback     */
#define QDSP_LB_PACKET_ENA_V    0x0004 /* Value to enable Packet Loopback     */
#define QDSP_LB_PACKET_DIS_V    0x0000 /* Value to disable Packet Loopback    */
#define QDSP_LB_AUDIO_M         0x0003 /* Mask to control Audio Loopback      */
#define QDSP_LB_AUDIO_PCM_V     0x0001 /* Value to enable PCM Loopback        */
#define QDSP_LB_AUDIO_AUD_V     0x0002 /* Value to enable Audio Loopback      */
#define QDSP_LB_AUDIO_DIS_V     0x0000 /* Value to disable Audio Loopback     */

/* QDSP Firmware state values
*/
#define QDSP_STATE_INIT      -1      /* DSP is in INIT state                  */
#define QDSP_STATE_IDLE      1       /* DSP is in IDLE state                  */
#define QDSP_STATE_VOCODER   3       /* DSP is in VOCODER state               */
#define QDSP_STATE_ADPCM_VOC 4       /* DSP is in ADPCM+VOCODER state         */
#define QDSP_STATE_ADEC_VOC  4       /* DSP is in MP3  +VOCODER state         */
#define QDSP_STATE_DFM       5       /* DSP is in DFM state                   */
#define QDSP_STATE_SLEEP     7       /* DSP is in SLEEP state                 */
#define QDSP_STATE_DFM_VOC   9       /* DFM + Vocoder state                   */
#define QDSP_STATE_VOC_DEC   11      /* Vocoder + 2nd Decoder                 */
#ifdef FEATURE_QSYNTH_COMMON
#define QDSP_STATE_SYNTH     0       /* DSP is in QSYNTH state                */
#endif
#define QDSP_STATE_PLAY     0       /* DSP is in Play state */
#ifdef FEATURE_QTUNES_COMMON
#define QDSP_STATE_TUNES_CONFIG 2    /* DSP is in QTUNES config state         */
#define QDSP_STATE_TUNES     0       /* DSP is playing QTUNES or QTUNES_AAC   */
#endif
#ifdef FEATURE_PHONE_VR
#define QDSP_STATE_VR_PROMPT   3     /* DSP is VR Prompt state (Mode B/C)     */
#define QDSP_STATE_VR_CAPTURE  7     /* DSP is VR Capture state (Mode B/C)    */
#define QDSP_STATE_VR_MATCH    11    /* DSP is VR Match state (Mode B/C)      */
#endif /* FEATURE_PHONE_VR */
#ifdef FEATURE_GRAPH_AUDIO
#define QDSP_STATE_GRAPH_IDLE   1    /* DSP is in GRAPH IDLE state            */
#define QDSP_STATE_GRAPH_AUDIO  3    /* DSP is in GRAPH AUDIO state           */
#endif /* FEATURE_GRAPH_AUDIO */
#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
#define QDSP_STATE_MIXER_IDLE   1
#define QDSP_STATE_MIXER_ACTIVE 2
#define QDSP_CMD_MIXER_ADEC0_INDEX 0
#define QDSP_CMD_MIXER_VOC_INDEX 14
#define QDSP_CMD_MIXER_CODEC_INDEX 15

typedef enum {
  QDSP_CMD_MIXER_MODE_VOC_ONLY   = (0x1 << QDSP_CMD_MIXER_VOC_INDEX),
  QDSP_CMD_MIXER_MODE_ADEC_ONLY  = (0x1 << QDSP_CMD_MIXER_ADEC0_INDEX),
  QDSP_CMD_MIXER_MODE_CODEC_ONLY = (0x1 << QDSP_CMD_MIXER_CODEC_INDEX),
  QDSP_CMD_MIXER_MODE_NONE       = 0x0000,
  QDSP_CMD_MIXER_MODE_VOC_ADEC   = (QDSP_CMD_MIXER_MODE_VOC_ONLY |
                                    QDSP_CMD_MIXER_MODE_ADEC_ONLY)
} qdsp_mixer_mode_type;
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB ||
          FEATURE_AVS_INCALL_MIXER_CMX */

/* Data structure to hold diagnostic values
*/
typedef struct {
  sint15 qdsp_state;
  uint16 rom_version;
  uint16 ram_version;
} qdsp_cmd_diag_type;

/* <EJECT> */
#ifdef FEATURE_VOC_PACKET_INTERFACE

/* Callbacks for packet clients */
/* Reverse Link Packet Interface */
typedef void (*qdsp_cmd_rl_packet_fn_type) (
  uint8   *vocoder_packet,      /* Pointer to data                 */
  uint16  frame_rate,           /* Rate for current vocoder frame  */
  uint16  packet_length         /* Length of current packet        */
);

/* Forward Link Packet Interface */
typedef void (*qdsp_cmd_fl_packet_fn_type) ();

#endif /* FEATURE_VOC_PACKET_INTERFACE */

/* <EJECT> */

/*===========================================================================

    Q D S P - V O C O D E R   P A C K E T   E X C H A N G E   F O R M A T

===========================================================================*/

#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR)

#define QDSP_AMR_CODEC_MODE_M  0x00ff
#define QDSP_GSM_CODEC_MODE_M  0xff00

/* GSM Frame Rate Definitions */
typedef enum {
  QDSP_AMR_RATE_475     = 0x0000,       /* AMR 4.75 kbit/s */
  QDSP_AMR_RATE_515     = 0x0001,       /* AMR 5.15 kbit/s */
  QDSP_AMR_RATE_590     = 0x0002,       /* AMR 5.90 kbit/s */
  QDSP_AMR_RATE_670     = 0x0003,       /* AMR 6.70 kbit/s */
  QDSP_AMR_RATE_740     = 0x0004,       /* AMR 7.40 kbit/s */
  QDSP_AMR_RATE_795     = 0x0005,       /* AMR 7.95 kbit/s */
  QDSP_AMR_RATE_1020    = 0x0006,       /* AMR 10.20 kbit/s */
  QDSP_AMR_RATE_1220    = 0x0007,       /* AMR 12.20 kbit/s */
  QDSP_GSM_FULL_RATE    = 0x0100,       /* GSM Full Rate, 13.00 kbit/s */
  QDSP_GSM_HALF_RATE    = 0x0200,       /* GSM Half Rate, 5.60 kbit/s */
  QDSP_GSM_ENH_FULL_RATE= 0x0300        /* GSM Enh. Full Rate, 12.20 kbit/s */
} qdsp_cmd_gsm_frame_rate_type;

/* All AMR TX and RX Frame types */
#define QDSP_AMR_FRAME_TYPE_M           0xff00  /* translated              */

#define QDSP_AMR_TX_SPEECH_V            0x0000  /* as QDSP_AMR_SPEECH_GOOD */
#define QDSP_AMR_TX_SID_FIRST_V         0x0100  /* as QDSP_AMR_SID_FIRST   */
#define QDSP_AMR_TX_SID_UPDATE_V        0x0200  /* as QDSP_AMR_SID_UPDATE  */
#define QDSP_AMR_TX_NO_DATA_V           0x0300  /* as QDSP_AMR_NO_DATA     */

#define QDSP_AMR_RX_SPEECH_GOOD_V       0x0000
#define QDSP_AMR_RX_SPEECH_DEGRADED_V   0x0100
#define QDSP_AMR_RX_ONSET_V             0x0200
#define QDSP_AMR_RX_SPEECH_BAD_V        0x0300
#define QDSP_AMR_RX_SID_FIRST_V         0x0400
#define QDSP_AMR_RX_SID_UPDATE_V        0x0500
#define QDSP_AMR_RX_SID_BAD_V           0x0600
#define QDSP_AMR_RX_NO_DATA_V           0x0700

/* GSM TX and RX Frame Types */
#define QDSP_GSM_TX_SP_M        0x0001
#define QDSP_GSM_HR_TX_SP_M     0x0010
#define QDSP_GSM_HR_TX_SP_V     0x0000
#define QDSP_GSM_HR_TX_ER_M     0x000a

#define QDSP_GSM_HR_RX_SP_M     0x0018
#define QDSP_GSM_HR_RX_SP_V     0x0000
#define QDSP_GSM_HR_RX_ISID_V   0x0008
#define QDSP_GSM_HR_RX_SID_V    0x0010

#define QDSP_GSM_RX_UFI_M       0x0001 /* Unstable frame indicator            */
#define QDSP_GSM_RX_BFI_M       0x0002 /* Bad frame indicator (erase frame)   */
#define QDSP_GSM_RX_TAF_M       0x0004

/* GSM Frame Type Definitions */
typedef enum {
  QDSP_GSM_SID          = 0x0000,   /* FR, EFR, or HR - silence descriptor  */
  QDSP_GSM_SPEECH_GOOD  = 0x0001,   /* FR, EFR, or HR - good frame          */
  QDSP_GSM_BFI          = 0x0002,   /* FR, EFR, or HR - bad frame indicator */
  QDSP_GSM_INVALID_SID  = 0x0003,   /* HR mode, invalid SID                 */

  QDSP_AMR_SPEECH_GOOD          = 0x00ff,  /* AMR mode frame type definitions */
  QDSP_AMR_SPEECH_DEGRADED      = 0x01ff,
  QDSP_AMR_ONSET                = 0x02ff,
  QDSP_AMR_SPEECH_BAD           = 0x03ff,
  QDSP_AMR_SID_FIRST            = 0x04ff,
  QDSP_AMR_SID_UPDATE           = 0x05ff,
  QDSP_AMR_SID_BAD              = 0x06ff,
  QDSP_AMR_NO_DATA              = 0x07ff
} qdsp_cmd_gsm_frame_type;

#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR || MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_AMR_WB_COMMON

#define QDSP_AMR_WB_CODEC_MODE_M  0x00ff

typedef enum {
  QDSP_AMR_WB_RATE_0660      = 0x0000,        /* AMR-WB  6.60 kbps */
  QDSP_AMR_WB_RATE_0885      = 0x0001,        /* AMR-WB  8.85 kbps */
  QDSP_AMR_WB_RATE_1265      = 0x0002,        /* AMR-WB 12.65 kbps */
  QDSP_AMR_WB_RATE_1425      = 0x0003,        /* AMR-WB 14.25 kbps */
  QDSP_AMR_WB_RATE_1585      = 0x0004,        /* AMR-WB 15.85 kbps */
  QDSP_AMR_WB_RATE_1825      = 0x0005,        /* AMR-WB 18.25 kbps */
  QDSP_AMR_WB_RATE_1985      = 0x0006,        /* AMR-WB 19.85 kbps */
  QDSP_AMR_WB_RATE_2305      = 0x0007,        /* AMR-WB 23.05 kbps */
  QDSP_AMR_WB_RATE_2385      = 0x0008         /* AMR-WB 23.85 kbps */
} qdsp_cmd_amr_wb_frame_rate_type;


/* All AMR-WB TX and RX Frame types */
#define QDSP_AMR_WB_FRAME_TYPE_M           0xff00  /* translated              */

#define QDSP_AMR_WB_TX_SPEECH_V            0x0000  /* QDSP_AMR_WB_SPEECH_GOOD */
#define QDSP_AMR_WB_TX_SID_FIRST_V         0x0100  /* QDSP_AMR_WB_SID_FIRST   */
#define QDSP_AMR_WB_TX_SID_UPDATE_V        0x0200  /* QDSP_AMR_WB_SID_UPDATE  */
#define QDSP_AMR_WB_TX_NO_DATA_V           0x0300  /* QDSP_AMR_WB_NO_DATA     */

#define QDSP_AMR_WB_RX_SPEECH_GOOD_V       0x0000
#define QDSP_AMR_WB_RX_SPEECH_DEGRADED_V   0x0100
#define QDSP_AMR_WB_RX_SPEECH_LOST_V       0x0200
#define QDSP_AMR_WB_RX_SPEECH_BAD_V        0x0300
#define QDSP_AMR_WB_RX_SID_FIRST_V         0x0400
#define QDSP_AMR_WB_RX_SID_UPDATE_V        0x0500
#define QDSP_AMR_WB_RX_SID_BAD_V           0x0600
#define QDSP_AMR_WB_RX_NO_DATA_V           0x0700
#define QDSP_AMR_WB_RX_ONSET_V             0x0800

typedef enum {
  QDSP_AMR_WB_SPEECH_GOOD       = 0x0000,
  QDSP_AMR_WB_SPEECH_DEGRADED   = 0x0100,
  QDSP_AMR_WB_SPEECH_LOST       = 0x0200,
  QDSP_AMR_WB_SPEECH_BAD        = 0x0300,
  QDSP_AMR_WB_SID_FIRST         = 0x0400,
  QDSP_AMR_WB_SID_UPDATE        = 0x0500,
  QDSP_AMR_WB_SID_BAD           = 0x0600,
  QDSP_AMR_WB_NO_DATA           = 0x0700,
  QDSP_AMR_WB_ONSET             = 0x0800
} qdsp_cmd_amr_wb_frame_type;

#endif  /* MSMAUD_VOC_AMR_WB_COMMON */

#ifdef MSMAUD_VOC_G711
/* Encoder/decoder mode for G.711 vocoder */
typedef enum {
  QDSP_G711_MODE_MULAW = 0x0000,
  QDSP_G711_MODE_ALAW  = 0x0001
} qdsp_cmd_g711_mode_type;

#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
/* Encoder/decoder mode for G.723 vocoder */
typedef enum {
  QDSP_G723_MODE_HIGH   = 0x0000,
  QDSP_G723_MODE_LOW    = 0x0001,
  QDSP_G723_MODE_SID    = 0x0002,
  QDSP_G723_MODE_BLANK  = 0x0003,
  QDSP_G723_MODE_ERROR  = 0x8003
} qdsp_cmd_g723_mode_type;

#define QDSP_CMD_G723_MODE_MASK 0x0003

#endif /* MSMAUD_VOC_G723 */

#ifdef FEATURE_MVS

#if defined(MSMAUD_VOC_AMR_WB)

/* AMR Uplink frame processing function definition
*/
typedef void (* qdsp_amr_wb_ul_fn_type)(
  uint16                           *vocoder_packet,
  qdsp_cmd_amr_wb_frame_type       frame_type,
  qdsp_cmd_amr_wb_frame_rate_type  amr_mode
);

/* AMR Downlink frame processing function definition
*/
typedef void (* qdsp_amr_wb_dl_fn_type)(
  uint16                           *vocoder_packet,
  qdsp_cmd_amr_wb_frame_type       *frame_type,
  qdsp_cmd_amr_wb_frame_rate_type  *amr_mode
);
#endif /* defined(MSMAUD_VOC_AMR_WB) */

#if defined(MSMAUD_VOC_AMR)

/* AMR Uplink frame processing function definition
*/
typedef void (* qdsp_amr_ul_fn_type)(
  uint16                        *vocoder_packet,
  qdsp_cmd_gsm_frame_type       frame_type,
  qdsp_cmd_gsm_frame_rate_type  amr_mode
);

/* AMR Downlink frame processing function definition
*/
typedef void (* qdsp_amr_dl_fn_type)(
  uint16                        *vocoder_packet,
  qdsp_cmd_gsm_frame_type       *frame_type,
  qdsp_cmd_gsm_frame_rate_type  *amr_mode
);
#endif /* defined(MSMAUD_VOC_AMR) */

#if defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) */

#ifdef MSMAUD_VOC_HR

/* HR Uplink frame processing function definition
*/
typedef void (* qdsp_hr_ul_fn_type)(
  uint16                        *vocoder_packet,
  qdsp_cmd_gsm_frame_type       frame_type,
  boolean                       enc_err_flag
);

/* HR Downlink frame processing function definition
*/
typedef void (* qdsp_hr_dl_fn_type)(
  uint16                        *vocoder_packet,
  qdsp_cmd_gsm_frame_type       *frame_type,
  boolean                       *taf,
  boolean                       *bfi,
  boolean                       *ufi
);

#endif  /* MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_G711

/* G711 Uplink frame processing function definition
*/
typedef void (* qdsp_g711_ul_fn_type)(
  uint16                        *vocoder_packet,
  uint16                        frame_length,
  qdsp_cmd_g711_mode_type       qdsp_g711_mode
);

/* G711 Downlink frame processing function definition
*/
typedef void (* qdsp_g711_dl_fn_type)(
  uint16                        *vocoder_packet,
  uint16                        *frame_length,
  qdsp_cmd_g711_mode_type       *qdsp_g711_mode
);

#endif /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723

/* G723 Uplink frame processing function definition
*/
typedef void (* qdsp_g723_ul_fn_type)(
  uint16                        *vocoder_packet,
  uint16                        frame_length,
  qdsp_cmd_g723_mode_type       qdsp_g723_mode
);

/* G723 Downlink frame processing function definition
*/
typedef void (* qdsp_g723_dl_fn_type)(
  uint16                        *vocoder_packet,
  uint16                        *frame_length,
  qdsp_cmd_g723_mode_type       *qdsp_g723_mode
);

#endif /* MSMAUD_VOC_G723 */

#endif  /* FEATURE_MVS */

#if defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT)
typedef struct {
  uint16 num_words;          /* Number of total words in command buffer  */
  uint16 num_commands;       /* Number of commands in command buffer     */
  uint16 *command_buf;       /* Buffer of commands                       */
} qdsp_cmd_midi_cmd_type;
#endif /* FEATURE_MIDI || FEATURE_MIDI_OUT */

/* <EJECT> */
#ifdef FEATURE_VOC_PCM_INTERFACE
/*===========================================================================

                   Vocoder PCM data interfaces

===========================================================================*/

/* maximum length is calculated based on wide-band dual mic */
#define QDSP_CMD_PCM_DATA_FRAME_LEN (320 * 2)   /* Length of each PCM frame */

#define QDSP_CMD_ENABLE_VOC_PCM_INT_V  0xFFFF
#define QDSP_CMD_DISABLE_VOC_PCM_INT_V 0

/* PCM Interfaces */
/* This should be classified per tapping point in DSP path
   each tapping point could have its own configured attributes
   each tapping point can not support more than 1 attribute simultaneously
   otherwise, that should be considered as 2 tapping points, i.e DSP has
   to support it.
*/
typedef enum {
  QDSP_PCM_INTERFACE_RX_OUTPUT, /* RX output */
  QDSP_PCM_INTERFACE_RX_INPUT,  /* RX input  */
  QDSP_PCM_INTERFACE_TX_OUTPUT, /* TX output */
  QDSP_PCM_INTERFACE_TX_INPUT,  /* TX input  */
  QDSP_PCM_INTERFACE_MAX
} qdsp_cmd_pcm_interface_type;

/* PCM channel attributes:
   all samples are assumed to be 16-bit signed little endian by default
*/
typedef struct {
  qdsp_cmd_pcm_interface_type chan_id;  /* channel identifier */
  uint32 sample_rate;
  uint16 frame_len;                /* size of channel frame in no. of bytes */
} qdsp_cmd_pcm_chan_attrib_type;

typedef struct {
  qdsp_cmd_pcm_interface_type interleave_id;  /* channel ID to interleave */
  uint16 interleave_len;                 /* no. of bytes to interleave */
} qdsp_cmd_pcm_interleave_attrib_type;

/* This table describes how each frame is contributed by each channel. */
/* If the total length accounted in the table is less than the frame length,
   the pattern repeats itself until the end of the frame.
*/
typedef struct {
  uint8 num_interleave;
  qdsp_cmd_pcm_interleave_attrib_type *interleave_tbl;
} qdsp_cmd_pcm_interleave_tbl_type;

typedef struct {
  qdsp_cmd_pcm_chan_attrib_type intf_dsc;  /* descriptor of interface itself */
  qdsp_cmd_pcm_interleave_tbl_type *interleave_dsc;  /* interleave descriptor */
} qdsp_cmd_pcm_interface_attrib_type;

/* PCM Clients */
/* PCM interface does not provide full multiple clients support at present.
   It assumes all active clients will be using same callback functions and
   attributes.
*/
typedef enum {
    QDSP_CMD_PCM_CLIENT_VOC,
    QDSP_CMD_PCM_CLIENT_TTY,
    QDSP_CMD_PCM_CLIENT_LOG,
    QDSP_CMD_PCM_CLIENT_MAX
} qdsp_cmd_pcm_client_type;

/* Data status used by PCM input callbacks to indicate availability */
typedef enum {
  QDSP_PCM_DATA_STATUS_AVAILABLE,   /* Data available for PCM input */
  QDSP_PCM_DATA_STATUS_UNAVAILABLE, /* Data not available           */
  QDSP_PCM_DATA_STATUS_MAX
} qdsp_cmd_pcm_data_status_type;

/* Client callback interface for PCM output */
typedef void (*qdsp_cmd_pcm_client_output_fn_type) (
  const uint16  *pcm_data,
  const qdsp_cmd_pcm_interface_attrib_type *pcm_attrib
);

/* Client callback interface for PCM input */
typedef qdsp_cmd_pcm_data_status_type (*qdsp_cmd_pcm_client_input_fn_type) (
  uint16 *pcm_data,
  const qdsp_cmd_pcm_interface_attrib_type *pcm_attrib
);

#endif /* FEATURE_VOC_PCM_INTERFACE */

typedef enum {
  QDSP_CMD_PCM_SAMPLE_RATE_8000,
  QDSP_CMD_PCM_SAMPLE_RATE_11025,
  QDSP_CMD_PCM_SAMPLE_RATE_12000,
  QDSP_CMD_PCM_SAMPLE_RATE_16000,
  QDSP_CMD_PCM_SAMPLE_RATE_22050,
  QDSP_CMD_PCM_SAMPLE_RATE_24000,
  QDSP_CMD_PCM_SAMPLE_RATE_32000,
  QDSP_CMD_PCM_SAMPLE_RATE_48000,
  QDSP_CMD_PCM_SAMPLE_RATE_MAX
}qdsp_cmd_pcm_sample_rate_type;


/* <EJECT> */
#if defined(FEATURE_ARM_POST_PROCESSING) || defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
/*==============================================================================

             A R M    P O S T  P R O C E S S I N G

==============================================================================*/

typedef void (* qdsp_cmd_arm_pp_cb_type) (
  uint16 buf_size, uint16* buffer, uint16 buf_pool_index);

typedef enum {
  QDSP_CMD_PP_TYPE_NONE,
  QDSP_CMD_PP_TYPE_GENERIC,
#if defined (FEATURE_SBC_CODEC)
  QDSP_CMD_PP_TYPE_SBC,
#endif
#ifdef FEATURE_SBC_DSP_CODEC
  QDSP_CMD_PP_TYPE_DSP_SBC,
#endif /* FEATURE_SBC_DSP_CODEC */
}qdsp_cmd_pp_type_type;
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */
/* <EJECT> */
/*===========================================================================

             Q D S P   M O D E M   M O D U L E   C O M M A N D S

===========================================================================*/

/*
** Control values for Vocoder Configuration Command
*/

/* Bit mask values for Vocoder Configuration
*/
#define QDSP_DEC_POST_FILT_M       0x0040       /* Decoder post filter control mask */
#define QDSP_DEC_POST_FILT_ENA_V   0x0040       /* Decoder post filter enable       */
#define QDSP_DEC_POST_FILT_DIS_V   0x0000       /* Decoder post filter disable      */
#define QDSP_VOC_MODE_M            0x000F       /* Vocoder Mode mask                */
#define QDSP_VOC_MODE_NONE_V       0x0000
#define QDSP_VOC_MODE_IS733_V      0x0001       /* Vocoder QCELP13k mode            */
#define QDSP_VOC_MODE_IS127_V      0x0002       /* Vocoder EVRC mode                */

#define QDSP_VOC_ENC_DEC_DIS_V     0x0000       /* Vocoder EVRC mode                */
#define QDSP_VOC_ENC_ENA_V         0xFFFF       /* Vocoder EVRC mode                */
#define QDSP_VOC_DEC_ENA_V         0xFFFF       /* Vocoder EVRC mode                */

#ifdef FEATURE_GRAPH_AUDIO
/* Commands for vocoder select command for graphics images */
#define QDSP_CMD_VOC_SELECT_13K    0x0001 /* Vocoder QCELP13k mode            */
#define QDSP_CMD_VOC_SELECT_EVRC   0x0002 /* Vocoder EVRC mode                */
#define QDSP_CMD_VOC_SELECT_AMR    0x0003 /* Vocoder AMR mode                 */
#define QDSP_CMD_VOC_SELECT_AAC    0x0004 /* Vocoder AAC LC mode            */

/* Commands for audio configuration for graphics images */
#define QDSP_AUDIO_MODE_13K        0x0001 /* Vocoder QCELP13k mode            */
#define QDSP_AUDIO_MODE_EVRC       0x0000 /* Vocoder EVRC mode                */
#define QDSP_AUDIO_MODE_AMR        0x0003 /* Vocoder AMR mode                 */
#define QDSP_AUDIO_MODE_ADPCM      0x0002 /* ADPCM mode                       */
#define QDSP_AUDIO_MODE_MP3        0x0001 /* MP3   mode                       */
#define QDSP_AUDIO_MODE_4GV_NB     0x0004 /* Vocoder 4GV Narrow band          */

/* Commands for qcamcorder currency image audio configuration */
#define QDSP_CODER_FLAG_VOC        0x0000
#define QDSP_CODER_FLAG_ADPCM      0xFFFF

#ifdef FEATURE_MM_REC
/* This enumerated type is used to indicate the format in mm recording.
*/
typedef enum {
  QDSP_CMD_MM_REC_AAC,
  QDSP_CMD_MM_REC_NONE
} qdsp_cmd_mm_rec_format_type;

#ifdef FEATURE_AAC_REC
/*
 * Maximum Multimedia frame number in output buffer, matches the value
 * VOC_AAC_ENC_MAX_FRAME  defined in voc.h
 */
#define QDSP_CMD_MM_AAC_ENC_MAX_FRAME 8     /* Buffer array's entry number    */

/*
 * Maximum Multimedia encoded data in each frame
 */
#define QDSP_CMD_MM_AAC_MAX_DATA      768

/*
 * Structure of shared buffer with QDSP AAC encoding
 */
typedef struct
{
  uint16 enc_frame_cnt_lsw;                 /* Frame count low word           */
  uint16 enc_frame_cnt_msw;                 /* Frame count high word          */
  uint16 enc_raw_byte_cnt;                  /* Encoded byte number in frame   */
  uint16 sample_erased_cnt;                 /* Erased samples in this frame   */
  uint16 raw_data[QDSP_CMD_MM_AAC_MAX_DATA];/* Encoded data buffer            */
} qdsp_cmd_aac_enc_frame_type;

/* Available Buffer information for AAC data record */
typedef struct {
  uint8  *buf1;                             /* Buffer1 starting address       */
  uint32 buf1_space;                        /* Available space of buffer1     */

  uint8  *buf2;                             /* Buffer2 starting address       */
  uint32 buf2_space;                        /* Available space of buffer2     */

  uint16 hdr_offset;                        /* Format header offset           */
  uint32 sync_samples;                      /* A/V sync required sync samples */
  uint32 sync_frames;                       /* Reported A/V sync frame number */
} qdsp_cmd_aac_buf_type;

/* Structure for each recorded AAC frame information */
typedef struct {
  uint32 frame_cnt;                         /* Frame number encoded           */
  uint32 data_length;                       /* Data length in this frame      */
} qdsp_cmd_aac_frame_type;

/* Data holder to get data from DSP for AAC record */
typedef struct {
  qdsp_cmd_aac_frame_type buf1_frame_info[QDSP_CMD_MM_AAC_ENC_MAX_FRAME];
                                 /* Holder for each frame info in buffer1    */
  uint16 buf1_frame_num;         /* Number of frames recorded in buffer1     */
  uint16 lost_frame1;            /* lost frame number in buffer1 recorded    */
  boolean buf1_flush;            /* Wether need to send buffer to cmx        */
  boolean buf1_avsync;           /* A/V sync report required                 */
  uint32  samples_rpt1;          /* A/V sync report samples for buffer1      */
  uint32  sync_frame1;           /* Frame number need to report              */

  qdsp_cmd_aac_frame_type buf2_frame_info[QDSP_CMD_MM_AAC_ENC_MAX_FRAME];
                                 /* Holder for each frame info in buffer2    */
  uint16 buf2_frame_num;         /* Number of frames recorded in buffer2     */
  uint16 lost_frame2;            /* lost frame number in buffer2 recorded    */
  boolean buf2_flush;            /* Wether need to send buffer to cmx        */
  boolean buf2_avsync;           /* A/V sync report required                 */
  uint32  samples_rpt2;          /* A/V sync report samples for buffer1      */
  uint32  sync_frame2;           /* Frame number need to report              */
} qdsp_cmd_aac_data_type;
#endif /* FEATURE_AAC_REC */

/*
 * Buffer sent to QDSP
 */
typedef union
{
  uint16 dummy_val;
#ifdef FEATURE_AAC_REC
  qdsp_cmd_aac_enc_frame_type aac_frame;    /* AAC encode support only now    */
#endif /* FEATURE_AAC_REC */
} qdsp_cmd_mm_enc_buffer_type;

/*
** Configuration values for mm record
*/
typedef struct {
  uint16 pkt_start_addr_msw;            /* Packet start address high word     */
  uint16 pkt_start_addr_lsw;            /* Packet start address low  word     */
  uint16 buffer_frame_num;              /* External buffer frame number       */
  uint16 sample_rate_index;             /* 0xb      8k
                                           0xa      11.025k
                                           0x9      12k
                                           0x8      16k
                                           0x7      22.05k
                                           0x6      24k
                                           0x5      32k
                                           0x4      44.1k/44k
                                           0x3      48k                       */
  uint16 channel_select;                /* 0        Mono   encode
                                           1        Stereo encode             */
  uint16 bit_per_sample_chan;           /* Average bit persample per channel  */
} qdsp_cmd_aac_encoder_config_type;

/* Buffer  holder to get data from DSP during MultiMedia recording */
typedef union {
  uint8 *dummy_buf;
#ifdef FEATURE_AAC_REC
  qdsp_cmd_aac_buf_type aac_buf; /* Recorded AAC data              */
#endif /* FEATURE_AAC_REC */
} qdsp_cmd_record_buf_type;

/* Data holder to get data from DSP during MultiMedia recording */
typedef union {
  uint32 dummy_data;
#ifdef FEATURE_AAC_REC
  qdsp_cmd_aac_data_type frame_data; /* Recorded AAC data              */
#endif /* FEATURE_AAC_REC */
} qdsp_cmd_record_data_type;
#endif /* FEATURE_MM_REC */

/*
** Configuration values for Graphics audio.
*/
typedef struct {
  uint16 audio_config;          /* Audio decoder type                   */
  uint16 coder_flag;            /* Audio decoder type                   */
#ifdef FEATURE_GRAPH_ADPCM
  uint16 num_channels;          /* Number of channels for ADPCM decoder */
  uint16 sampling_rate;         /* Samplign rate for ADPCM decoder      */
  uint16 block_size;            /* Block size of ADPCM                  */
#endif /* FEATURE_GRAPH_ADPCM */
} qdsp_cmd_audio_config_type;
#endif /* FEATURE_GRAPH_AUDIO */

#if defined(MSMAUD_VOC_AMR_COMMON)
#define QDSP_VOC_MODE_AMR_V        0x0003 /* AMR Vocoder mode                 */
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

#if defined(MSMAUD_VOC_EFR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) */

#if defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined_MSMAUD_VOC_FR) */

#ifdef MSMAUD_VOC_4GV
#define QDSP_VOC_MODE_4GV_V        0x0006 /* CDMA 4GV Vocoder mode            */
#endif  /* MSMAUD_VOC_4GV */

#ifdef MSMAUD_VOC_HR
#define QDSP_VOC_MODE_GSM_HR_V     0x0007 /* GSM Half Rate Vocoder mode       */
#endif  /* MSMAUD_VOC_HR */

#ifdef MSMAUD_VOC_G711
#define QDSP_VOC_MODE_G711_V       0x0008 /* G.711 Vocoder mode               */
#endif  /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
#define QDSP_VOC_MODE_G723_V       0x0009 /* G.723 Vocoder mode               */
#endif  /* MSMAUD_VOC_G723 */

#ifdef MSMAUD_VOC_AMR_WB
#define QDSP_VOC_MODE_AMR_WB_V     0x000B /* AMR-WB Vocoder mode              */
#endif  /* MSMAUD_VOC_AMR_WB */

#ifdef MSMAUD_VOC_4GV_WB
#define QDSP_VOC_MODE_4GV_WB_V        0x000c /* CDMA 4GV_WB Vocoder mode            */
#endif  /* MSMAUD_VOC_4GV_WB */

#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif /* MSMAUD_VOC_4GV_VOIP */

#ifdef MSMAUD_VOC_IS127_VOIP
#define QDSP_VOC_MODE_IS127_VOIP_V  0x000E  /* EVRC VOIP Vocoder mode */
#endif /* MSMAUD_VOC_IS127_VOIP */

/*
** Control values for Vocoder Timing Command
*/
typedef struct {
  uint16 enc_frame_offset;
  uint16 dec_frame_offset;
  uint16 dec_int_advance;
} qdsp_cmd_voc_timing_type;

/*
** Control values for encoder control
*/

/* Bit mask values for reinit_bne (background noise estimate)
*/
#define QDSP_REINIT_BNE_M          0x0001 /* Reinit BNE control mask          */
#define QDSP_REINIT_BNE_ENA_V      0x0001 /* Reinit BNE enable                */
#define QDSP_REINIT_BNE_DIS_V      0x0000 /* Reinit BNE disable               */

/* Bit mask values for enc_min_rate and enc_max_rate
*/
#define QDSP_RATE_LIMIT_M          0x000F /* Rate limit control mask          */
#define QDSP_RATE_BLANK_V          0x0000 /* Blank frame                      */
#define QDSP_RATE_EIGHTH_V         0x0001 /* 1/8 rate                         */
#define QDSP_RATE_QUARTER_V        0x0002 /* 1/4 rate                         */
#define QDSP_RATE_HALF_V           0x0003 /* 1/2 rate                         */
#define QDSP_RATE_FULL_V           0x0004 /* Full rate                        */
#define QDSP_RATE_ERASURE_V        0x000E /* Full rate                        */

/* Bit mask values for rate_modulation_cmd
*/
#define QDSP_RATE_LIMIT_FACTOR_M   0x03FC /* Rate limit factor control mask   */
#define QDSP_RATE_LIMIT_FACTOR_V (s)  (s << 2)
#define QDSP_RATE_LIMIT_S_OR_1S_M  0x0002 /* Select 1 or 1/s control mask     */
#define QDSP_RATE_LIMIT_S_V        0x0002 /* Rate limit factor is S           */
#define QDSP_RATE_LIMIT_1S_V       0x0000 /* Rate limit factor is 1/S         */
#define QDSP_RATE_MODULATION_M     0x0001 /* Rate Modulation control mask     */
#define QDSP_RATE_MODULATION_ENA_V 0x0001 /* Rate Modulation enabled          */
#define QDSP_RATE_MODULATION_DIS_V 0x0000 /* Rate Modulation disabled         */

/* Bit mask values for reduced_rate_level
*/
#define QDSP_RATE_REDUCE_MODE_M    0x0007 /* Rate Reduced mode control mask   */
#define QDSP_RATE_REDUCE_144       0x0000 /* Rate = 14.4 kbps                 */
#define QDSP_RATE_REDUCE_122       0x0001 /* Rate = 12.2 kbps                 */
#define QDSP_RATE_REDUCE_112       0x0002 /* Rate = 11.2 kbps                 */
#define QDSP_RATE_REDUCE_90        0x0003 /* Rate =  9.0 kbps                 */
#define QDSP_RATE_REDUCE_72        0x0004 /* Rate =  7.2 kbps                 */

/* Data structure containing Encoder Control Command values
*/
typedef struct {
  uint16 reinit_bne;
  uint16 enc_min_rate;
  uint16 enc_max_rate;
  uint16 rate_modulation_cmd;
  uint16 reduced_rate_level;
} qdsp_cmd_enc_ctrl_type;

/*
** Control values for DFM Configuration Command
*/

/* Bit mask values for FM_AUDIO_CONFIG
*/
#define QDSP_FM_RX_AUTO_MUTE_M     0x0080 /* WBD auto mute control mask       */
#define QDSP_FM_RX_AUTO_MUTE_ENA_V 0x0080 /* WBD auto mute enable             */
#define QDSP_FM_RX_AUTO_MUTE_DIS_V 0x0000 /* WBD auto mute disable            */
#define QDSP_FM_TX_MUTE_M          0x0040 /* TX audio mute control mask       */
#define QDSP_FM_TX_MUTE_ENA_V      0x0040 /* TX audio mute enable             */
#define QDSP_FM_TX_MUTE_DIS_V      0x0000 /* TX audio mute disable            */
#define QDSP_FM_COMP_M             0x0020 /* Audio compressor control mask    */
#define QDSP_FM_COMP_ENA_V         0x0000 /* Audio compressor enable          */
#define QDSP_FM_COMP_BYP_V         0x0020 /* Audio compressor bypass          */
#define QDSP_FM_PREEMP_M           0x0010 /* Preemphasis control mask         */
#define QDSP_FM_PREEMP_ENA_V       0x0000 /* Preemphasis enable               */
#define QDSP_FM_PREEMP_BYP_V       0x0010 /* Preemphasis bypass               */
#define QDSP_FM_LOOPBACK_M         0x0008 /* FM audio loopback control mask   */
#define QDSP_FM_LOOPBACK_ENA_V     0x0008 /* FM audio loopback enable         */
#define QDSP_FM_LOOPBACK_DIS_V     0x0000 /* FM audio loopback bypass         */
#define QDSP_FM_DEEMP_M            0x0004 /* Deemphasis control mask          */
#define QDSP_FM_DEEMP_ENA_V        0x0000 /* Deemphasis enable                */
#define QDSP_FM_DEEMP_BYP_V        0x0004 /* Deemphasis bypass                */
#define QDSP_FM_EXPAND_M           0x0002 /* Expander control mask            */
#define QDSP_FM_EXPAND_ENA_V       0x0000 /* Expander enable                  */
#define QDSP_FM_EXPAND_BYP_V       0x0002 /* Expander bypass                  */
#define QDSP_FM_SAT_TRANS_M        0x0001 /* SAT transpond control mask       */
#define QDSP_FM_SAT_TRANS_ENA_V    0x0000 /* SAT transpond enable             */
#define QDSP_FM_SAT_TRANS_DIS_V    0x0001 /* SAT transpond disable            */

/* Bit mask values for SAT_CONTROL
*/
#define QDSP_FM_SAT_INT_M          0x0004 /* SAT Interrupt control mask       */
#define QDSP_FM_SAT_INT_ENA_V      0x0000 /* SAT Interrupt enable             */
#define QDSP_FM_SAT_INT_DIS_V      0x0004 /* SAT Interrupt disable            */
#define QDSP_FM_SAT_ECC_M          0x0003 /* SAT ECC-expected color code mask */
#define QDSP_FM_SAT_ECC_5970_V     0x0000 /* SAT ECC = 5970Hz                 */
#define QDSP_FM_SAT_ECC_6000_V     0x0001 /* SAT ECC = 6000Hz                 */
#define QDSP_FM_SAT_ECC_6030_V     0x0002 /* SAT ECC = 6030Hz                 */

/* Data structure containing DFM Configuration Command values.
*/
typedef struct {
  uint16 fm_audio_config;
  uint16 fm_sat_control;
  uint16 fm_sat_agc_level;
  uint16 fm_sat_threshold;
  uint16 fm_deviation_limit;
  uint16 fm_sat_dc_offset;
} qdsp_cmd_dfm_config_type;

/*---------------------------------------------------------------------------
                        SAT Detected Color Code VALUES

qdsp_cmd_dfm_init is passed a pointer to a function to call when there is any
SAT validity change event.  Below are the SAT detected color code value and a
type definition for the function pointer.
---------------------------------------------------------------------------*/

#define QDSP_FM_SAT_DCC_M          0x0003 /* SAT detected color code mask */
#define QDSP_FM_SAT_DCC_5970_V     0x0000 /* SAT DCC = 5970Hz                 */
#define QDSP_FM_SAT_DCC_6000_V     0x0001 /* SAT DCC = 6000Hz                 */
#define QDSP_FM_SAT_DCC_6030_V     0x0002 /* SAT DCC = 6030Hz                 */
#define QDSP_FM_SAT_DCC_RESERVED   0x0003 /* SAT detected color code reserved */

/* A pointer to a function returning void, called with 1 parameter of
** type uint16.
*/

/* SAT Detected Color Code Value is the function parameter when the function
** gets called.
*/

typedef void
  (* qdsp_cmd_sat_dcc_cb_type)( uint16 sat_dcc );

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
typedef void
  (* qdsp_cmd_dualmono_cb_type)( uint16 audio_mode );
#endif

/* <EJECT> */
/*===========================================================================

                     D M A   P A T H   C O N T R O L

===========================================================================*/
#ifdef MSMAUD_6200_UP_DMA_PATH_CONTROL_VALUES
#error code not present
#elif defined(MSMAUD_6100_UP_DMA_PATH_CONTROL_VALUES)

#define QDSP_DMA_CH0_SEL_M         0x0003 /* DMA Channel 0 selection mask     */
#define QDSP_DMA_CH0_SEL_PCM_V     0x0000 /* Channel 0 - PCM                  */
#define QDSP_DMA_CH0_SEL_SDAC_V    0x0001 /* Channel 0 - ExternalDAC I2S slave*/
#define QDSP_DMA_CH0_SEL_CAMERA_V  0x0002 /* Channel 0 - Camera Interface     */
#define QDSP_DMA_CH1_SEL_M         0x0004 /* DMA Channel 1 selection mask     */
#define QDSP_DMA_CH1_SEL_PCM_V     0x0000 /* Channel 1 - PCM                  */
#define QDSP_DMA_CH1_SEL_SDAC_V    0x0004 /* Channel 1 - SDAC                 */
#define QDSP_DMA_CH2_SEL_M         0x0018 /* DMA Channel 2 selection mask     */
#define QDSP_DMA_CH2_SEL_DFM_V     0x0000 /* Channel 2 - DFM                  */
#define QDSP_DMA_CH2_SEL_DAI_V     0x0008 /* Channel 2 - DAI                  */
#define QDSP_DMA_CH2_SEL_USB_V     0x0010 /* Channel 2 - USB                  */
#define QDSP_DMA_CH3_SEL_M         0x0060 /* DMA Channel 3 selection mask     */
#define QDSP_DMA_CH3_SEL_DFM_V     0x0000 /* Channel 3 - DFM                  */
#define QDSP_DMA_CH3_SEL_DAI_V     0x0020 /* Channel 3 - DAI                  */
#define QDSP_DMA_CH3_SEL_USB_V     0x0040 /* Channel 3 - USB                  */
#define QDSP_DMA_CH4_SEL_M         0x0010 /* DMA Channel 4 selection mask     */
#define QDSP_DMA_CH4_SEL_RES_V     0x0000 /* Channel 4 - Reserved             */
#define QDSP_DMA_CH5_SEL_M         0x0020 /* DMA Channel 5 selection mask     */
#define QDSP_DMA_CH5_SEL_RES_V     0x0000 /* Channel 4 - Reserved             */

#define QDSP_DMA_EXT_RPCM_M           0x0080 /* EXT RPCM data source          */
#define QDSP_DMA_EXT_RPCM_VDSP_V      0x0000 /* EXT RPCM data from VDSP       */
#define QDSP_DMA_EXT_RPCM_MDSP_V      0x0080 /* EXT RPCM data from MDSP       */
#define QDSP_DMA_MDSP_RPCM_M          0x0100 /* MDSP RPCM data source         */
#define QDSP_DMA_MDSP_RPCM_VDSP_V     0x0000 /* MDSP RPCM data from VDSP      */
#define QDSP_DMA_MDSP_RPCM_AUX_V      0x0100 /* MDSP RPCM data from AUX_CODEC */
#define QDSP_DMA_VDSP_RPCM_M          0x0200 /* VDSP RPCM data source         */
#define QDSP_DMA_VDSP_RPCM_AUX_V      0x0000 /* VDSP RPCM from AUX_CODEC      */
#define QDSP_DMA_VDSP_RPCM_MDSP_V     0x0200 /* VDSP RPCM from MDSP           */
#define QDSP_DMA_CH0_SYNC_SEL_M       0x0400 /* Sync mode selection mask      */
#define QDSP_DMA_CH0_SYNC_SEL_ASYNC_V 0x0000 /* Async DMA for channel 0       */
#define QDSP_DMA_CH0_SYNC_SEL_SYNC_V  0x0400 /* Sync DMA for channel 0        */

#define QDSP_DMA_PATH_DEFAULT     ( QDSP_DMA_CH0_SEL_PCM_V        | \
                                    QDSP_DMA_CH1_SEL_PCM_V        | \
                                    QDSP_DMA_CH2_SEL_DFM_V        | \
                                    QDSP_DMA_CH3_SEL_DFM_V        | \
                                    QDSP_DMA_EXT_RPCM_VDSP_V      | \
                                    QDSP_DMA_MDSP_RPCM_VDSP_V     | \
                                    QDSP_DMA_VDSP_RPCM_AUX_V      | \
                                    QDSP_DMA_CH0_SYNC_SEL_ASYNC_V )

#ifdef FEATURE_EXTERNAL_SDAC
/* External Stereo DAC DMA path control settings
*/
#define QDSP_DMA_PATH_SDAC        ( QDSP_DMA_CH0_SEL_SDAC_V       | \
                                    QDSP_DMA_CH1_SEL_SDAC_V       | \
                                    QDSP_DMA_CH2_SEL_DFM_V        | \
                                    QDSP_DMA_CH3_SEL_DFM_V        | \
                                    QDSP_DMA_EXT_RPCM_VDSP_V      | \
                                    QDSP_DMA_MDSP_RPCM_VDSP_V     | \
                                    QDSP_DMA_VDSP_RPCM_AUX_V      | \
                                    QDSP_DMA_CH0_SYNC_SEL_ASYNC_V )
#endif /* FEATURE_EXTERNAL_SDAC */

#ifdef FEATURE_BT_AG
/* Bluetooth DMA path control settings
*/
#define QDSP_BT_DMA_DEFAULT_V   0x0000 /* Default DMA path for BT & DFM */
#define QDSP_BT_DMA_EXT_COD_V   0xFFFF /* Internal use only             */

#define QDSP_DMA_BT_CODEC_INT     ( QDSP_DMA_CH0_SEL_PCM_V        | \
                                    QDSP_DMA_CH1_SEL_PCM_V        | \
                                    QDSP_DMA_CH2_SEL_DFM_V        | \
                                    QDSP_DMA_CH3_SEL_DFM_V        | \
                                    QDSP_DMA_EXT_RPCM_VDSP_V      | \
                                    QDSP_DMA_MDSP_RPCM_VDSP_V     | \
                                    QDSP_DMA_VDSP_RPCM_AUX_V      | \
                                    QDSP_DMA_CH0_SYNC_SEL_ASYNC_V )

#define QDSP_DMA_BT_CODEC_EXT     ( QDSP_DMA_CH0_SEL_PCM_V        | \
                                    QDSP_DMA_CH1_SEL_PCM_V        | \
                                    QDSP_DMA_CH2_SEL_DFM_V        | \
                                    QDSP_DMA_CH3_SEL_DFM_V        | \
                                    QDSP_DMA_EXT_RPCM_VDSP_V      | \
                                    QDSP_DMA_MDSP_RPCM_VDSP_V     | \
                                    QDSP_DMA_VDSP_RPCM_AUX_V      | \
                                    QDSP_DMA_CH0_SYNC_SEL_ASYNC_V )
#endif /* FEATURE_BT_AG */

#ifdef FEATURE_EXTERNAL_SADC
#define QDSP_DMA_PATH_SADC        ( QDSP_DMA_CH0_SEL_SDAC_V   | \
                                    QDSP_DMA_CH1_SEL_PCM_V    | \
                                    QDSP_DMA_CH2_SEL_DFM_V    | \
                                    QDSP_DMA_CH3_SEL_DFM_V    | \
                                    QDSP_DMA_CH4_SEL_RES_V    | \
                                    QDSP_DMA_CH5_SEL_RES_V    | \
                                    QDSP_DMA_EXT_RPCM_VDSP_V  | \
                                    QDSP_DMA_MDSP_RPCM_VDSP_V | \
                                    QDSP_DMA_VDSP_RPCM_AUX_V  )
#endif /* FEATURE_EXTERNAL_SADC */

#else
/* 1X Values for DMA Path Control */
#define QDSP_DMA_CH0_SEL_M      0x0180 /* DMA Channel 0 selection mask        */
#define QDSP_DMA_CH0_SEL_PCM_V  0x0000 /* Channel 0 - PCM                     */
#define QDSP_DMA_CH0_SEL_USB_V  0x0080 /* Channel 0 - USB                     */
#define QDSP_DMA_CH0_SEL_GPS_V  0x0100 /* Channel 0 - GPS                     */
#define QDSP_DMA_CH0_SEL_BT_V   0x0180 /* Channel 0 - Bluetooth               */

#define QDSP_DMA_CH1_SEL_M      0x0060 /* DMA Channel 1 selection mask        */
#define QDSP_DMA_CH1_SEL_PCM_V  0x0000 /* Channel 1 - PCM                     */
#define QDSP_DMA_CH1_SEL_USB_V  0x0020 /* Channel 1 - USB                     */
#define QDSP_DMA_CH1_SEL_SDAC_V 0x0040 /* Channel 1 - SDAC                    */
#define QDSP_DMA_CH1_SEL_BT_V   0x0060 /* Channel 1 - Bluetooth               */

#define QDSP_DMA_CH2_SEL_M      0x0018 /* DMA Channel 2 selection mask        */
#define QDSP_DMA_CH2_SEL_RDFM_V 0x0000 /* Channel 2 - RDFM                    */
#define QDSP_DMA_CH2_SEL_BT_V   0x0008 /* Channel 2 - Bluetooth               */
#define QDSP_DMA_CH2_SEL_GPSI_V 0x0010 /* Channel 2 - GPS I                   */
#define QDSP_DMA_CH2_SEL_SRCH_V 0x0018 /* Channel 2 - SRCH                    */

#define QDSP_DMA_CH3_SEL_M      0x0004 /* DMA Channel 3 selection mask        */
#define QDSP_DMA_CH3_SEL_TDFM_V 0x0000 /* Channel 3 - TDFM                    */
#define QDSP_DMA_CH3_SEL_BT_V   0x0004 /* Channel 3 - Bluetooth               */

#define QDSP_DMA_CH4_SEL_M      0x0002 /* DMA Channel 4 selection mask        */
#define QDSP_DMA_CH4_SEL_COD_V  0x0000 /* Channel 4 - Codec                   */
#define QDSP_DMA_CH4_SEL_GPSQ_V 0x0002 /* Channel 4 - GPS Q                   */

#define QDSP_DMA_CH5_SEL_M      0x0001 /* DMA Channel 5 selection mask        */
#define QDSP_DMA_CH5_SEL_COD_V  0x0000 /* Channel 5 - Codec                   */
#define QDSP_DMA_CH5_SEL_RES_V  0x0001 /* Channel 5 - Reserved                */

#define QDSP_DMA_PATH_DEFAULT     ( QDSP_DMA_CH0_SEL_PCM_V  | \
                                    QDSP_DMA_CH1_SEL_PCM_V  | \
                                    QDSP_DMA_CH2_SEL_RDFM_V | \
                                    QDSP_DMA_CH3_SEL_TDFM_V | \
                                    QDSP_DMA_CH4_SEL_COD_V  | \
                                    QDSP_DMA_CH5_SEL_COD_V    )

#ifdef FEATURE_EXTERNAL_SDAC
/* External Stereo DAC DMA path control settings
*/
#define QDSP_DMA_PATH_SDAC        ( QDSP_DMA_CH0_SEL_PCM_V  | \
                                    QDSP_DMA_CH1_SEL_SDAC_V | \
                                    QDSP_DMA_CH2_SEL_RDFM_V | \
                                    QDSP_DMA_CH3_SEL_TDFM_V | \
                                    QDSP_DMA_CH4_SEL_COD_V  | \
                                    QDSP_DMA_CH5_SEL_COD_V    )
#endif /* FEATURE_EXTERNAL_SDAC */

#ifdef FEATURE_BT_AG
/* Bluetooth DMA path control settings
*/
#define QDSP_BT_DMA_DEFAULT_V   0x0000 /* Default DMA path for BT & DFM       */
#define QDSP_BT_DMA_EXT_COD_V   0xFFFF /* Internal use only                   */

#define QDSP_DMA_BT_CODEC_INT     ( QDSP_DMA_CH0_SEL_BT_V   | \
                                    QDSP_DMA_CH1_SEL_BT_V   | \
                                    QDSP_DMA_CH2_SEL_RDFM_V | \
                                    QDSP_DMA_CH3_SEL_TDFM_V | \
                                    QDSP_DMA_CH4_SEL_COD_V  | \
                                    QDSP_DMA_CH5_SEL_COD_V    )

#define QDSP_DMA_BT_CODEC_EXT     ( QDSP_DMA_CH0_SEL_PCM_V  | \
                                    QDSP_DMA_CH1_SEL_PCM_V  | \
                                    QDSP_DMA_CH2_SEL_BT_V   | \
                                    QDSP_DMA_CH3_SEL_BT_V   | \
                                    QDSP_DMA_CH4_SEL_COD_V  | \
                                    QDSP_DMA_CH5_SEL_COD_V    )

#define QDSP_DMA_BT_USB_CODEC_EXT ( QDSP_DMA_CH0_SEL_USB_V  | \
                                    QDSP_DMA_CH1_SEL_USB_V  | \
                                    QDSP_DMA_CH2_SEL_BT_V   | \
                                    QDSP_DMA_CH3_SEL_BT_V   | \
                                    QDSP_DMA_CH4_SEL_COD_V  | \
                                    QDSP_DMA_CH5_SEL_COD_V    )

#endif /* FEATURE_BT_AG */

#endif /* MSMAUD_6200_UP_DMA_PATH_CONTROL */

/* <EJECT> */
/*===========================================================================

        Q D S P   A U D I O   F R O N T   E N D   C O M M A N D S

===========================================================================*/

/*
** Data values for Codec Configuration Command
*/

/* Bit mask values for PCM_FORMAT_CONTROL (pcm_format_ctrl)
*/
#ifdef MSMAUD_PCM_FORMAT_3BIT
#define QDSP_PCM_MODE_M            0x7000 /* PCM Mode mask                    */
#define QDSP_PCM_MODE_INTERNAL_V   0x0000 /* Internal routing mode            */
#define QDSP_PCM_MODE_ENCODER_V    0x1000 /* Encoder routing mode             */
#define QDSP_PCM_MODE_DECODER_V    0x2000 /* Decoder routing mode             */
#define QDSP_PCM_MODE_STANDALONE_V 0x3000 /* Standalone codec mode            */
#ifdef MSMAUD_PCM_FORMAT_DAI
#error code not present
#else /* MSMAUD_PCM_FORMAT_DAI */

#define QDSP_PCM_MODE_BT_AUDIO_V   0x4000 /* Bluetooth audio gateway mode     */
#define QDSP_PCM_MODE_BT_LA_V      0x5000 /* Bluetooth local audio mode       */
#define QDSP_PCM_MODE_BT_EXT_COD_V 0x6000 /* Bluetooth internal use only      */

#endif /* MSMAUD_PCM_FORMAT_DAI */

#else
#define QDSP_PCM_MODE_M            0xC000 /* PCM Mode mask                    */
#define QDSP_PCM_MODE_INTERNAL_V   0x0000 /* Internal routing mode            */
#endif

#ifdef MSMAUD_MSM6050_PCM_CTRL
#if defined (FEATURE_INTERNAL_SDAC) || \
    defined (FEATURE_INTERNAL_SADC) || \
    defined (FEATURE_EXTERNAL_SADC) || \
    defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)

#define QDSP_PCM_MODE_32K_INT_STEREO_V  \
                                   0x6000 /* Wideband internal stereo mode    */
#endif /* FEATURE_INTERNAL_SDAC */
#define QDSP_PCM_MODE_32K_INT_V    0x4000 /* Wideband internal codec mode     */
#define QDSP_PCM_MODE_32K_STEREO_V 0x8000 /* External 32kHz Stereo DAC mode   */
#define QDSP_PCM_MODE_32K_MONO_V   0xA000 /* External 32kHz Mono DAC mode     */
#define QDSP_PCM_MODE_32K_SINGLE_MONO_V \
                                   0xC000 /* External 32kHz single channel    */
                                          /* Mono DAC mode                    */
#else
#define QDSP_PCM_MODE_8K_V         0x0000 /* 8kHz internal/external codec     */
#define QDSP_PCM_MODE_16K_V        0x4000 /* Wideband internal codec mode     */
#define QDSP_PCM_MODE_22K_STEREO_V 0x8000 /* External 22kHz Stereo DAC mode   */
#define QDSP_PCM_MODE_22K_MONO_V   0xC000 /* External 22kHz Mono DAC mode     */
#endif /* MSMAUD_MSM6050_PCM_CTRL */

#define QDSP_WB_COMP_M             0x0080 /* Wideband compensation mask       */
#define QDSP_WB_COMP_ENA_V         0x0080 /* Enable WB comp filter            */
#define QDSP_WB_COMP_DIS_V         0x0000 /* Disable WB comp filter           */

#define QDSP_SDAC_SIGN_SWAP_M      0x0008 /* SDAC sign swap mask              */
#define QDSP_SDAC_SIGN_SWAP_ENA_V  0x0008 /* Enable sign swapping of samples  */
#define QDSP_SDAC_SIGN_SWAP_DIS_V  0x0000 /* Disable sign swapping of samples */

#define QDSP_RX_HPF_M              0x0800 /* RX High Pass Filter control mask */
#define QDSP_RX_HPF_ENA_V          0x0800 /* RX High Pass Filter enable       */
#define QDSP_RX_HPF_DIS_V          0x0000 /* RX High Pass Filter disable      */
#define QDSP_TX_HPF_M              0x0400 /* RX High Pass Filter control mask */
#define QDSP_TX_HPF_ENA_V          0x0400 /* RX High Pass Filter enable       */
#define QDSP_TX_HPF_DIS_V          0x0000 /* RX High Pass Filter disable      */
#define QDSP_TX_SLOPE_FILT_M       0x0200 /* RX High Pass Filter control mask */
#define QDSP_TX_SLOPE_FILT_ENA_V   0x0200 /* RX High Pass Filter enable       */
#define QDSP_TX_SLOPE_FILT_DIS_V   0x0000 /* RX High Pass Filter disable      */

#define QDSP_CODEC_RES_M           0x0100 /* Codec Reset Mask                 */
#define QDSP_CODEC_RES_V           0x0100 /* Reset codec within 1 mSec        */
#define QDSP_CODEC_RES_DIS_V       0x0000 /* Disable Reset codec in 1 msec    */
#define QDSP_DEC_PAD_M             0x0040 /* Decoder padding control mask     */
#define QDSP_DEC_PAD_ENA_V         0x0040 /* Value to enable dec padding      */
#define QDSP_DEC_PAD_DIS_V         0x0000 /* Value to disable dec padding     */
#define QDSP_DEC_PCM_M             0x0030 /* Decoder PCM format mask          */
#define QDSP_DEC_PCM_LIN_V         0x0020 /* Set Decoder PCM format: linear   */
#define QDSP_DEC_PCM_ALAW_V        0x0010 /* Set Decoder PCM format: alaw     */
#define QDSP_DEC_PCM_ULAW_V        0x0000 /* Set Decoder PCM format: ulaw     */
#define QDSP_ENC_PAD_M             0x0004 /* Encoder padding control mask     */
#define QDSP_ENC_PAD_ENA_V         0x0004 /* Value to enable enc padding      */
#define QDSP_ENC_PAD_DIS_V         0x0000 /* Value to disable enc padding     */
#define QDSP_ENC_PCM_M             0x0003 /* Decoder PCM format mask          */
#define QDSP_ENC_PCM_LIN_V         0x0002 /* Set Encoder PCM format: linear   */
#define QDSP_ENC_PCM_ALAW_V        0x0001 /* Set Encoder PCM format: alaw     */
#define QDSP_ENC_PCM_ULAW_V        0x0000 /* Set Encoder PCM format: ulaw     */

/* Bit mask values for PCM_CTRL (pcm_ctrl)
*/
#ifdef MSMAUD_MSM6050_PCM_CTRL
#define QDSP_DEC_SLOT_M            0x3E00 /* Decoder slot control mask        */
#define QDSP_DEC_PCM_SLOT_V(slot)  (slot << 9)
#define QDSP_ENC_SLOT_M            0x01F0 /* Encoder slot control mask        */
#define QDSP_ENC_PCM_SLOT_V(slot)  (slot << 4)
#define QDSP_PCM_LB_M              0x0008 /* PCM Loopback control mask        */
#define QDSP_PCM_LB_DIS_V          0x0000 /* PCM Loopback disable             */
#define QDSP_PCM_LB_ENA_V          0x0008 /* PCM Loopback enable              */
#define QDSP_DEC_PCM_WIDTH_M       0x0002 /* Decoder PCM width mask           */
#define QDSP_DEC_PCM_WIDTH_16_V    0x0002 /* Decoder PCM width 16 bits        */
#define QDSP_DEC_PCM_WIDTH_8_V     0x0000 /* Decoder PCM width 8 bits         */
#define QDSP_ENC_PCM_WIDTH_M       0x0001 /* Decoder PCM width mask           */
#define QDSP_ENC_PCM_WIDTH_16_V    0x0001 /* Decoder PCM width 16 bits        */
#define QDSP_ENC_PCM_WIDTH_8_V     0x0000 /* Decoder PCM width 8 bits         */
#define QDSP_PCM_CLK_SENSE_NORM_V  0x0000 /* Unused value                     */
#else
#define QDSP_PCM_LB_M              0x2000 /* PCM Loopback control mask        */
#define QDSP_PCM_LB_DIS_V          0x0000 /* PCM Loopback disable             */
#define QDSP_PCM_LB_ENA_V          0x2000 /* PCM Loopback enable              */
#define QDSP_PCM_CLK_SENSE_M       0x1000 /* PCM Clock sense control mask     */
#define QDSP_PCM_CLK_SENSE_NORM_V  0x0000 /* PCM Clock sense normal           */
#define QDSP_PCM_CLK_SENSE_INV_V   0x1000 /* PCM Clock sense inverted         */
#define QDSP_DEC_PCM_WIDTH_M       0x0800 /* Decoder PCM width mask           */
#define QDSP_DEC_PCM_WIDTH_16_V    0x0800 /* Decoder PCM width 16 bits        */
#define QDSP_DEC_PCM_WIDTH_8_V     0x0000 /* Decoder PCM width 8 bits         */
#define QDSP_DEC_SLOT_M            0x07C0 /* Decoder slot control mask        */
#define QDSP_DEC_PCM_SLOT_V(slot)  (slot << 6)
#define QDSP_ENC_PCM_WIDTH_M       0x0020 /* Decoder PCM width mask           */
#define QDSP_ENC_PCM_WIDTH_16_V    0x0020 /* Decoder PCM width 16 bits        */
#define QDSP_ENC_PCM_WIDTH_8_V     0x0000 /* Decoder PCM width 8 bits         */
#define QDSP_ENC_SLOT_M            0x001F /* Encoder slot control mask        */
#define QDSP_ENC_PCM_SLOT_V(slot)  (slot)
#endif /* MSMAUD_MSM6050_PCM_CTRL */

/* Bit mask values for Codec Function Control 0 (codec_func_0)
*/

#ifdef MSMAUD_MIC_RESET_CTRL
#define QDSP_MIC_RESET_DIS_M       0x8000 /* MIC reset disable mask           */
#define QDSP_MIC_RESET_DIS_OFF_V   0x0000 /* MIC reset disable control off    */
#define QDSP_MIC_RESET_DIS_ON_V    0x8000 /* MIC reset disable control on     */
#define QDSP_NSOUT_SELECT_OFF_V    0x0000 /* Must be zero                     */
#else
#define QDSP_NSOUT_SELECT_M        0x4000 /* NSOUT_SELECT control mask        */
#define QDSP_NSOUT_SELECT_OFF_V    0x0000 /* NSOUT_SELECT control off         */
#define QDSP_NSOUT_SELECT_ON_V     0x4000 /* NSOUT_SELECT control on          */
#endif

#define QDSP_PDN_BANDGAP_M         0x2000 /* PDN_BANDGAP control mask         */
#define QDSP_PDN_BANDGAP_OFF_V     0x0000 /* PDN_BANDGAP control off          */
#define QDSP_PDN_BANDGAP_ON_V      0x2000 /* PDN_BANDGAP control on           */
#define QDSP_PDN_REFERENCE_M       0x1000 /* PDN_REFERENCE control mask       */
#define QDSP_PDN_REFERENCE_OFF_V   0x0000 /* PDN_REFERENCE control off        */
#define QDSP_PDN_REFERENCE_ON_V    0x1000 /* PDN_REFERENCE control on         */
#define QDSP_PDN_MODULATOR_M       0x0800 /* PDN_MODULATOR control mask       */
#define QDSP_PDN_MODULATOR_OFF_V   0x0000 /* PDN_MODULATOR control off        */
#define QDSP_PDN_MODULATOR_ON_V    0x0800 /* PDN_MODULATOR control on         */
#define QDSP_PDN_RECONST_M         0x0400 /* PDN_RECONSTRUCTION control mask  */
#define QDSP_PDN_RECONST_OFF_V     0x0000 /* PDN_RECONSTRUCTION control off   */
#define QDSP_PDN_RECONST_ON_V      0x0400 /* PDN_RECONSTRUCTION control on    */
#define QDSP_RECON_IN_SEL_M        0x0200 /* RECON_IN_SELECT control mask     */
#define QDSP_RECON_IN_SEL_OFF_V    0x0000 /* RECON_IN_SELECT control off      */
#define QDSP_RECON_IN_SEL_ON_V     0x0200 /* RECON_IN_SELECT control on       */
#define QDSP_COMB_IN_SEL_M         0x0100 /* COMB_IN_SELECT control mask      */
#define QDSP_COMB_IN_SEL_OFF_V     0x0000 /* COMB_IN_SELECT control off       */
#define QDSP_COMB_IN_SEL_ON_V      0x0100 /* COMB_IN_SELECT control on        */

#ifdef MSMAUD_CONCURRENT_VBAP_AMPS_TYPE1

#define QDSP_VBAP_AMP_SEL_M        0x01C0 /* Amp select mask                  */
#define QDSP_VBAP_AMP_SEL_NONE_V   0x0000 /* All amps power down              */
#define QDSP_VBAP_AMP_SEL_EAR_1_V  0x0100 /* Ear Amp 1 selected               */
#define QDSP_VBAP_AMP_SEL_EAR_2_V  0x0080 /* Ear Amp 2 selected               */
#define QDSP_VBAP_AMP_SEL_AUX_V    0x0040 /* Aux Amp selected                 */

#elif defined(MSMAUD_CONCURRENT_VBAP_AMPS_TYPE2)
#define QDSP_VBAP_AMP_SEL_M        0x40C0 /* Amp select mask                  */
#define QDSP_VBAP_AMP_SEL_NONE_V   0x0000 /* All amps power down              */
#define QDSP_VBAP_AMP_SEL_EAR_1_V  0x0040 /* Ear Amp 1 selected               */
#define QDSP_VBAP_AMP_SEL_EAR_2_V  0x0080 /* Ear Amp 2 selected               */
#define QDSP_VBAP_AMP_SEL_AUX_V    0x4000 /* AUX amplifief control on         */

#else
#define QDSP_VBAP_AMP_SEL_M        0x00C0 /* Amp select mask                  */
#define QDSP_VBAP_AMP_SEL_NONE_V   0x0000 /* All amps power down              */
#define QDSP_VBAP_AMP_SEL_EAR_1_V  0x0040 /* Ear Amp 1 selected               */
#define QDSP_VBAP_AMP_SEL_EAR_2_V  0x0080 /* Ear Amp 2 selected               */
#define QDSP_VBAP_AMP_SEL_AUX_V    0x00C0 /* Aux Amp selected                 */
#endif /* MSMAUD_CONCURRENT_VBAP_AMPS_TYPE1 */

#define QDSP_VBAP_MIC2_BYP_M       0x0020 /* Mic Amp 2 bypass control mask    */
#define QDSP_VBAP_MIC2_BYP_V       0x0020 /* Mic Amp 2 bypassed               */
#define QDSP_VBAP_MIC2_ENA_V       0x0000 /* Mic Amp 2 in line                */

#define QDSP_VBAP_MIC1_GAIN_M      0x0018 /* Mic Amp 1 gain select maskB      */
#define QDSP_VBAP_MIC1_GAIN_M2_V   0x0000 /* Mic Amp 1 gain = -2dB            */
#define QDSP_VBAP_MIC1_GAIN_P6_V   0x0008 /* Mic Amp 1 gain = 6dB             */
#define QDSP_VBAP_MIC1_GAIN_P8_V   0x0010 /* Mic Amp 1 gain = 8dB             */
#define QDSP_VBAP_MIC1_GAIN_P16_V  0x0018 /* Mic Amp 1 gain = 16dB            */

#define QDSP_VBAP_MIC_SEL_M        0x0006 /* Mic amp select mask              */
#define QDSP_VBAP_MIC_SEL_NONE_V   0x0000 /* Mic amp, mic bias amp power down */
#define QDSP_VBAP_MIC_SEL_1_V      0x0002 /* Mic 1 selected                   */
#define QDSP_VBAP_MIC_SEL_2_V      0x0004 /* Mic 2 selected                   */
#define QDSP_VBAP_MIC_SEL_AUX_V    0x0006 /* Aux Mic selected                 */

#define QDSP_VBAP_CODEC_PWR_M      0x0001 /* Codec ref. system control mask   */
#define QDSP_VBAP_CODEC_PWR_OFF_V  0x0000 /* Codec ref. system control off    */
#define QDSP_VBAP_CODEC_PWR_ON_V   0x0001 /* Codec ref. system control on     */

#if defined(FEATURE_MM_REC) || defined(FEATURE_AVS_I2SSBC)
#define QDSP_VBAP_CODEC_I2S_MONO        0x0010  /* Incoming PCM I2S mono   */
#define QDSP_VBAP_CODEC_I2S_STEREO      0x0020  /* Incoming PCM I2S stereo */
#define QDSP_VBAP_CODEC_INTERNAL_MONO   0x0040  /* Incoming PCM internal
                                                   codec mono   */
#define QDSP_VBAP_CODEC_INTERNAL_STEREO 0x0080  /* Incoming PCM internal
                                                   codec stereo */
#endif /* FEATURE_MM_REC || FEATURE_AVS_I2SSBC */

#if defined(FEATURE_MM_REC) || defined(FEATURE_AVS_I2SSBC) \
    || defined(FEATURE_AVS_CRYSTAL_SPEECH)
#define QDSP_VBAP_CODEC_MODE_SEL_MONO   0x0008  /* mode select */
#define QDSP_VBAP_CODEC_MODE_SEL_STEREO 0x000C  /* mode select */
#define QDSP_VBAP_CODEC_L_R_N           0x0002  /* sync to left */
#endif /* FEATURE_MM_REC || FEATURE_AVS_I2SSBC || FEATURE_AVS_CRYSTAL_SPEECH */

/* Bit mask values for Codec Interface Control (codec_interface)
*/
#define QDSP_LOOP_SEL_M            0xC000 /* Codec data path loop mask        */
#define QDSP_LOOP_SEL_NONE_V       0x0000 /* Normal codec data path           */
#define QDSP_LOOP_SEL_PLUS_V       0x4000 /* Loop+ codec data path for testing*/
#define QDSP_LOOP_SEL_MINUS_V      0x8000 /* Loop- codec data path for testing*/
#define QDSP_LOOP_SEL_OVS_V        0xC000 /* Over sampled data input          */
#define QDSP_I2S_CTRL_M            0x2000 /* I2S enable control mask          */
#define QDSP_I2S_ENA_V             0x2000 /* Enable                           */
#define QDSP_I2S_DIS_V             0x0000 /* Disable                          */
#define QDSP_SDAC_SIGN_CTRL_M      0x1000 /* SDAC signal control mask         */
#define QDSP_SDAC_SIGN_SAME_V      0x0000 /* Signal is same as before         */
#define QDSP_SDAC_SIGN_REV_V       0x1000 /* Signal is reversed               */

#ifdef MSMAUD_QSC60X5_AUDIO_HW

#define QDSP_I2S_ENABLE            0x4000 /* I2S Int. in ADSP wrapper enabled */
#define QDSP_I2S_DISABLE           0x0000 /* I2S Int. in ADSP wrapper disabled*/

#define QDSP_AUX_PCM_ENABLE        0x2000 /* PCM Int. in ADSP wrapper enabled */
#define QDSP_AUX_PCM_DISABLE       0x0000 /* PCM Int. in ADSP wrapper disabled*/

#define QDSP_ADSP_CODEC_CTL_ENA    0x0800 /* ADSP has the ctrl of AUX register*/

/* Long sync offset by 1/4th cycle              */
#define QDSP_PCM_SYNC_LONG_OFFSET_ENA  0x0400
/* No offset for Long Sync                      */
#define QDSP_PCM_SYNC_LONG_OFFSET_DIS  0x0000

/* Short Sync offset by 1/4th cycle             */
#define QDSP_PCM_SYNC_SHORT_OFFSET_ENA 0x0200
/* No offset for Short Sync                     */
#define QDSP_PCM_SYNC_SHORT_OFFSET_DIS 0x0000

/* The Polarity of the PCM SYNC is inverted     */
#define QDSP_PCM_SYNC_INVERT_ENA       0x0100
/* The Polarity of the PCM SYNC is not inverted */
#define QDSP_PCM_SYNC_INVERT_DIS       0x0000

/* AUX_PCM MODE                                 */
#define QDSP_AUX_PCM_MODE_MASTER_PRIM  0x0000
#define QDSP_AUX_PCM_MODE_MASTER_AUX   0x0004
#define QDSP_AUX_PCM_MODE_SLAVE        0x0008

/* AUX CODEC MODE                               */
#define QDSP_AUX_CODEC_MODE_AUX_PCM    0x0000
#define QDSP_AUX_CODEC_MODE_I2S        0x0002

#if defined(FEATURE_EXTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SADC)
#if defined(MSMAUD_QSC6270_AUDIO_CAL) || \
    defined(MSMAUD_QSC60X5_AUDIO_CAL) || \
    defined(MSMAUD_QSC1110_AUDIO_CAL)
#define QDSP_I2S_CLK_MODE_BMSK         0x0010
#define QDSP_I2S_CLK_MODE_SLAVE        QDSP_I2S_CLK_MODE_BMSK
#define QDSP_I2S_CLK_MODE_MASTER       0x0000
#define QDSP_I2S_RX_MODE_BMSK          0x0020
#define QDSP_I2S_SDAC_MODE             QDSP_I2S_RX_MODE_BMSK
#define QDSP_I2S_RX_MODE               0x0000
#define QDSP_I2S_MCLK_MODE_BMSK        0x0040
#define QDSP_I2S_MCLK_MODE_MASTER      0x0000
#define QDSP_I2S_MCLK_MODE_SLAVE       QDSP_I2S_MCLK_MODE_BMSK
#define QDSP_I2S_MCLK_SRC_BMSK         0x0080
#define QDSP_I2S_MCLK_SRC_SDAC_CLK     0x0000
#define QDSP_I2S_MCLK_SRC_ICODEC_CLK   QDSP_I2S_MCLK_SRC_BMSK

#endif /* defined(MSMAUD_QSC6270_AUDIO_CAL) ||
          defined(MSMAUD_QSC60X5_AUDIO_CAL) ||
          defined(MSMAUD_QSC1110_AUDIO_CAL) */
#endif /* FEATURE_EXTERNAL_SDAC || FEATURE_EXTERNAL_SADC */

#else
/*For other targets following values need to be defined  with proper values
    Macros here are defined to avoid compilation issues for other targets */
#define QDSP_I2S_ENABLE                0x0000
#define QDSP_ADSP_CODEC_CTL_ENA        0x0000
#define QDSP_AUX_CODEC_MODE_I2S        0x0000
#define QDSP_I2S_CLK_MODE_BMSK         0x0000
#define QDSP_I2S_CLK_MODE_SLAVE        QDSP_I2S_CLK_MODE_BMSK
#define QDSP_I2S_CLK_MODE_MASTER       0x0000
#define QDSP_I2S_RX_MODE_BMSK          0x0000
#define QDSP_I2S_SDAC_MODE             QDSP_I2S_RX_MODE_BMSK
#define QDSP_I2S_RX_MODE               0x0000
#define QDSP_I2S_MCLK_MODE_BMSK        0x0000
#define QDSP_I2S_MCLK_MODE_MASTER      0x0000
#define QDSP_I2S_MCLK_MODE_SLAVE       QDSP_I2S_MCLK_MODE_BMSK
#define QDSP_I2S_MCLK_SRC_BMSK         0x0000
#define QDSP_I2S_MCLK_SRC_SDAC_CLK     0x0000
#define QDSP_I2S_MCLK_SRC_ICODEC_CLK   QDSP_I2S_MCLK_SRC_BMSK

#endif /* MSMAUD_QSC60X5_AUDIO_HW */

#define QDSP_AUX_CLK_CONTROL_M     0x0800 /* Aux PCM Clock / Sync control     */
#define QDSP_AUX_CLK_ENA_V         0x0800 /* Enable                           */
#define QDSP_AUX_CLK_DIS_V         0x0000 /* Disable (use AUX_CLK/SYNC_VAL)   */
#define QDSP_AUX_CLK_VAL_M         0x0400 /* Aux PCM Clock value control mask */
#define QDSP_AUX_CLK_VAL_HI_V      0x0400 /* High                             */
#define QDSP_AUX_CLK_VAL_LO_V      0x0000 /* Low                              */
#define QDSP_AUX_SYNC_VAL_M        0x0200 /* Aux PCM Sync value  control mask */
#define QDSP_AUX_SYNC_VAL_HI_V     0x0200 /* High                             */
#define QDSP_AUX_SYNC_VAL_LO_V     0x0000 /* Low                              */
#define QDSP_CLK_SYNC_LOOP_M       0x0100 /* Aux PCM loop control mask        */
#define QDSP_CLK_SYNC_LOOP_ENA_V   0x0100 /* Loop PCM_CLK/SYNC to AUX_CLK/SYNC*/
#define QDSP_CLK_SYNC_LOOP_DIS_V   0x0000 /* Disable loop                     */

#ifdef MSMAUD_DIN_LOOP_BIT
#define QDSP_DIN_LOOP_M            0x0080 /* DIN Loop control mask            */
#define QDSP_DIN_LOOP_ENA_V        0x0080 /* Loop PCM_DIN/DOUT to AUX_DIN/DOUT*/
#define QDSP_DIN_LOOP_DIS_V        0x0000 /* Disable DIN loop                 */
#else
#define QDSP_SDAC_BCLK_M           0x0080 /* SDAC bit clock control mask      */
#define QDSP_SDAC_BCLK_INP_V       0x0080 /* SDAC bit clock is input          */
#define QDSP_SDAC_BCLK_OUTP_V      0x0000 /* SDAC bit clock is output         */
#endif

#define QDSP_AUX_PCM_CLK_SNS_M     0x0040 /* PCM Clock sense control mask     */
#define QDSP_AUX_CLK_SENSE_NORM_V  0x0000 /* PCM Clock sense normal           */
#define QDSP_AUX_CLK_SENSE_INV_V   0x0040 /* PCM Clock sense inverted         */
#define QDSP_SDAC_MCLK_M           0x0020 /* SDAC master clock control mask   */
#define QDSP_SDAC_MCLK_INP_V       0x0020 /* SDAC master clock is input       */
#define QDSP_SDAC_MCLK_OUTP_V      0x0000 /* SDAC master clock is output      */

#define QDSP_AUX_CODEC_SEL_M       0x0010 /* AUX codec select mask            */
#define QDSP_AUX_CODEC_NORM_V      0x0000 /* Normal (Aux DOUT=AUX_DOUT_VAL)   */
#define QDSP_AUX_CODEC_AUX_V       0x0010 /* Auxiliary output                 */
#define QDSP_AUX_POLARITY_M        0x0008 /* AUX codec polarity control mask  */
#define QDSP_AUX_POL_INVERTED_V    0x0008 /* AUX codec inverted               */
#define QDSP_AUX_POL_NORMAL_V      0x0000 /* AUX codec normal                 */

#define QDSP_AUX_PRI_PCM_ENA_V     0x0100 /* Enable the primary pcm mode      */

#define QDSP_PCM_CLK_DIR_M         0x0004 /* PCM Clock source control mask    */
#define QDSP_PCM_CLK_DIR_MOD_V     0x0004 /* Modulator                        */
#define QDSP_PCM_CLK_DIR_EXT_V     0x0000 /* PCM_CLK pin                      */

#define QDSP_SDAC_EN_M             0x0002 /* Stereo DAC enable mask           */
#define QDSP_SDAC_EN_OFF_V         0x0000 /* AUX PCM available                */
#define QDSP_SDAC_EN_ON_V          0x0002 /* AUX PCM used for stereo DAC      */
#define QDSP_RESERVED_0_M          0x0001 /* Bit 0 reserved mask              */

#define QDSP_RESERVED_V            0x0000 /* Reserved value                   */

#ifdef MSMAUD_UP_DMA_PATH_CONTROL
/* Bit mask values for 8KHz Interrupt Mode
*/
#define QDSP_8KHZ_INT_MODE_M       0x000F /* 8KHz interrupt mode mask         */
#define QDSP_8KHZ_INT_MODE_PCM_V   0x0001 /* AUX PCM interrupt                */
#define QDSP_8KHZ_INT_MODE_USB_V   0x0002 /* USB interrupt                    */
#define QDSP_8KHZ_INT_MODE_CODEC_V 0x0004 /* Internal codec interrupt         */
#define QDSP_8KHZ_INT_MODE_BT_V    0x0008 /* Bluetooth interrupt              */
#else
/* Bit mask values for Codec DMA Path Control (dma_path)
*/

#define QDSP_DMA_CH_45_SEL_M       0x0030 /* Select CODEC or GPS(Q)           */
#define QDSP_DMA_CH_45_SEL_CODEC_V 0x0000 /* Use integrated CODEC             */
#define QDSP_DMA_CH_45_SEL_GPS_V   0x0010 /* Use GPS(Q)                       */

#define QDSP_DMA_CH_23_SEL_M       0x000C /* Select DFM, USB or GPS(I)        */
#define QDSP_DMA_CH_23_SEL_DFM_V   0x0000 /* Use DFM                          */
#define QDSP_DMA_CH_23_SEL_USB_V   0x0004 /* Use USB                          */
#define QDSP_DMA_CH_23_SEL_GPS_V   0x0008 /* Use GPS(I)                       */

#define QDSP_DMA_CH_01_SEL_M       0x0003 /* Select PCM or USB                */
#define QDSP_DMA_CH_01_SEL_PCM_V   0x0000 /* Use PCM                          */
#define QDSP_DMA_CH_01_SEL_USB_V   0x0001 /* Use USB                          */
#endif /* MSMAUD_UP_DMA_PATH_CONTROL */

/* Data structure containing Codec Configuration Command values.
*/
typedef struct {
  uint16 pcm_format_ctrl;
  uint16 pcm_ctrl;
  uint16 codec_func_ctrl;
  uint16 codec_interface;
  uint16 dma_path;
#ifdef MSMAUD_UP_DMA_PATH_CONTROL
  uint16 eight_khz_int_mode;
#endif
} qdsp_cmd_codec_config_type;

#ifdef FEATURE_SBC_DSP_CODEC

#define QDSP_CMD_SBC_PARAM_BLOCKS_M           0x000F

#define QDSP_CMD_SBC_PARAM_BLOCKS_4_V         0x0000
#define QDSP_CMD_SBC_PARAM_BLOCKS_8_V         0x0001
#define QDSP_CMD_SBC_PARAM_BLOCKS_12_V        0x0002
#define QDSP_CMD_SBC_PARAM_BLOCKS_16_V        0x0003

#define QDSP_CMD_SBC_PARAM_SUBBANDS_M         0x0030
#define QDSP_CMD_SBC_PARAM_SUBBANDS_8_V       0x0010
/* #define QDSP_CMD_SBC_PARAM_SUBBANDS_4_V */

#define QDSP_CMD_SBC_PARAM_MODE_M             0x00C0
#define QDSP_CMD_SBC_PARAM_MODE_MONO_V        0x0000
#define QDSP_CMD_SBC_PARAM_MODE_DUAL_V        0x0040
#define QDSP_CMD_SBC_PARAM_MODE_STEREO_V      0x0080
#define QDSP_CMD_SBC_PARAM_MODE_JT_STEREO_V   0x00C0
#define QDSP_CMD_SBC_PARAM_MODE_INVALID_V     0x0000

#define QDSP_CMD_SBC_PARAM_BIT_ALLOC_M        0x0300
#define QDSP_CMD_SBC_PARAM_BIT_ALLOC_LOUD_V   0x0000
#define QDSP_CMD_SBC_PARAM_BIT_ALLOC_SNR_V    0x0100

#define QDSP_CMD_SBC_PARAM_SBC_ENA_M          0x8000
#define QDSP_CMD_SBC_PARAM_SBC_ENA_V          0x8000
#define QDSP_CMD_SBC_PARAM_SBC_DIS_V          0x0000

#define QDSP_CMD_SBC_BITRATE_LOW_M            0x0000FFFF
#define QDSP_CMD_SBC_BITRATE_HIGH_M           0x00070000

typedef struct {
  uint16 param_buf;
}qdsp_cmd_sbc_codec_config_type;

#ifdef FEATURE_AVS_I2SSBC
typedef enum {
  QDSP_CMD_I2SSBC_SAMPLE_RATE_8000,
  QDSP_CMD_I2SSBC_SAMPLE_RATE_11025,
  QDSP_CMD_I2SSBC_SAMPLE_RATE_12000,
  QDSP_CMD_I2SSBC_SAMPLE_RATE_16000,
  QDSP_CMD_I2SSBC_SAMPLE_RATE_22050,
  QDSP_CMD_I2SSBC_SAMPLE_RATE_24000,
  QDSP_CMD_I2SSBC_SAMPLE_RATE_32000,
  QDSP_CMD_I2SSBC_SAMPLE_RATE_44100,
  QDSP_CMD_I2SSBC_SAMPLE_RATE_48000,
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  QDSP_CMD_I2SSBC_SAMPLE_RATE_44000,
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
  QDSP_CMD_I2SSBC_SAMPLE_RATE_MAX
}qdsp_cmd_i2ssbc_sample_rate_type;
#endif /* FEATURE_AVS_I2SSBC */
#endif /* FEATURE_SBC_DSP_CODEC */

/*
** Data values for Sample Slip Mode Command
*/
#define QDSP_SAMPLE_SLIP_MODE_M    0x0001 /* Sample Slip mode control mask    */
#define QDSP_SAMPLE_SLIP_AUTO_V    0x0001 /* Automatic Sample Slipping        */
#define QDSP_SAMPLE_SLIP_MANUAL_V  0x0000 /* Manual Sample Slipping           */

/*
** Data values for Sample Slip Control Command
*/
#define QDSP_SAMPLE_SLIP_CTRL_M    0x0003 /* Sample Slip mode control mask    */
#define QDSP_SAMPLE_SLIP_NONE_V    0x0000 /* No action                        */
#define QDSP_SAMPLE_SLIP_SLIP_V    0x0002 /* Slow down                        */
#define QDSP_SAMPLE_SLIP_RETARD_V  0x0003 /* Speed up                         */


/*VP3 cmd structure*/
typedef struct {
  uint16 enable;
  uint16 memAddrlower;
  uint16 memAddrupper;
  uint16 partitionnum;
  uint16 buf_id;
}qdsp_cmd_vp3_config_type;

/*
** Data values for DTMF Tone Request Command
*/

/* Data structure containing DTMF Command values.
** Values are in vocoder units.
*/
typedef struct {
  uint16 dtmf_hi;             /* High tone register value    */
  uint16 dtmf_low;            /* Low tone register value     */
  uint16 duration;            /* Duration in 5ms incremnents */
  uint16 tx_gain;             /* Tx Volume setting           */
  uint16 rx_gain;             /* Rx Volume setting           */
} qdsp_cmd_dtmf_type;

/* Bit mask values for DTMF_PATH (path)
*/
#define QDSP_DTMF_TX_M             0x0002 /* Mask to control TX DTMF          */
#define QDSP_DTMF_TX_ENA_V         0x0002 /* Enable TX DTMF                   */
#define QDSP_DTMF_TX_DIS_V         0x0000 /* Disable TX DTMF                  */
#define QDSP_DTMF_RX_M             0x0001 /* Mask to control RX DTMF          */
#define QDSP_DTMF_RX_ENA_V         0x0001 /* Enable RX DTMF                   */
#define QDSP_DTMF_RX_DIS_V         0x0000 /* Disable RX DTMF                  */

/*
** Data structure containing values for Volume Control Command
*/

#define QDSP_TX_UNITY_GAIN         0x4000 /* Value for TX unity gain          */
#define QDSP_TX_MUTE_GAIN          0x0000 /* Value for TX mute                */

#define QDSP_CODEC_TX_UNITY_GAIN   0x4000 /* Value for CODEC TX unity gain    */
#define QDSP_CODEC_RX_UNITY_GAIN   0x4000 /* Value for CODEC RX unity gain    */
#define QDSP_CODEC_ST_UNITY_GAIN   0x4000 /* Value for Side Tone unity gain   */

#define QDSP_RX_MUTE_DIS_V         0x0000 /* Value to unmute RX audio         */
#define QDSP_RX_MUTE_CONDITIONAL_V 0x0001 /* NOT A DSP INTERFACE VALUE        */
                                          /* ^ Used for hinting to driver that
                                             rx muting should be conditional on
                                             support of DTMF mixing           */
#define QDSP_RX_MUTE_ENA_V         0xFFFF /* Value to mute RX audio           */

/* Bit mask values for tx_comfort_noise_ena
*/
#define QDSP_COMFORT_NOISE_M       0x0001 /* Comfort noise control mask       */
#define QDSP_COMFORT_NOISE_ENA_V   0x0001 /* Comfort noise enable             */
#define QDSP_COMFORT_NOISE_DIS_V   0x0000 /* Comfort noise disable            */

#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_AMR_WB_COMMON)

/*
** Data structure containing values for DTX mode control
*/
typedef enum {
  QDSP_GSM_DTX_OFF = 0x0000,
  QDSP_GSM_DTX_ON  = 0xffff
} qdsp_cmd_gsm_dtx_mode_type;

#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR || MSMAUD_VOC_HR || MSMAUD_VOC_AMR_WB_COMMON */

/*
** Data structure containing values for Volume Control
*/
typedef struct {
  uint16 tx_vol;
  uint16 rx_vol;
  uint16 rx_mute;
  uint16 tx_comfort_noise_ena;
} qdsp_cmd_vol_ctrl_type;

/*
** enum definition to provide mutual exclusive operation to codec gain
** This should not be in codec gain control data structure type because
** it is not part of calibration item list and is intended for internal
** use of control only.
*/
typedef enum {
  QDSP_CMD_CODEC_GAIN_OP_NORMAL,
  QDSP_CMD_CODEC_GAIN_LOCK_WR,
  QDSP_CMD_CODEC_GAIN_LOCK_NOP,
  QDSP_CMD_CODEC_GAIN_UNLOCK_WR,
  QDSP_CMD_CODEC_GAIN_UNLOCK_NOP,
  QDSP_CMD_CODEC_GAIN_FORCE_LOCK_WR,
  QDSP_CMD_CODEC_GAIN_UNLOCK_ALL_WR,
  QDSP_CMD_CODEC_GAIN_UNLOCK_ALL_NOP,
  QDSP_CMD_CODEC_GAIN_OP_MAX
} qdsp_cmd_codec_gain_ctrl_lock_type;

/*
** Data structure containing values for Internal CODEC gain control
*/
typedef struct {
  uint16 codec_tx_gain;
  uint16 codec_rx_gain;
  uint16 codec_st_gain;
} qdsp_cmd_codec_gain_ctrl_type;

/*
** Data structure containing values for Internal CODEC offset control
*/
#ifdef MSMAUD_PCM_DC_OFFSET
typedef struct {
  uint16 codec_rxL_offset;
  uint16 codec_rxR_offset;
} qdsp_cmd_codec_offset_ctrl_type;
#endif  /* MSMAUD_PCM_DC_OFFSET */

#ifdef FEATURE_AUDIO_AGC
/* Values to enable/disable AGC/AVC */
#define QDSP_CMD_AGC_ENA  0xFFFF
#define QDSP_CMD_AGC_DIS  0x0000

/*
** Data structure containing values for AGC Configuration Command
*/
typedef struct {
  uint16 rx_agc_enable;
  uint16 rx_avc_enable;
  uint16 tx_agc_enable;
} qdsp_cmd_agc_config_type;

/*
** Data structure containing values for AGC writable control parameters Command
*/
typedef struct {
  uint16 rx_agc_static_gain;
  uint16 rx_agc_aig;
  uint16 rx_agc_exp_thres;
  uint16 rx_agc_exp_slope;
  uint16 rx_agc_compr_thres;
  uint16 rx_agc_compr_slope;
  uint16 rx_avc_sensitivity;
  uint16 rx_avc_headroom;
  uint16 tx_agc_static_gain;
  uint16 tx_agc_aig;
  uint16 tx_agc_exp_thres;
  uint16 tx_agc_exp_slope;
  uint16 tx_agc_compr_thres;
  uint16 tx_agc_compr_slope;
} qdsp_cmd_agc_param_type;
#endif


/* Values to enable/disable RVE */
#define QDSP_CMD_RVE_ENA  0xFFFF
#define QDSP_CMD_RVE_DIS  0x0000


/*
** Data structure containing values for RVE Configuration Command
*/

typedef struct {
  uint16 rx_rve_enable;
} qdsp_cmd_rve_config_type;


/*
** Data structure containing values for  RVE  writable control parameters Command
*  in single mic condition
*/
typedef struct {

  uint16 rx_rve_sm_alpha_noise;
  uint16 rx_rve_sm_alpha_ref;
  uint16 rx_rve_sm_alpha_rec;
  uint16 rx_rve_sm_alpha_gain_increase;
  uint16 rx_rve_sm_alpha_gain_decrease;
  uint16 rx_rve_sm_alpha_amp;
  uint16 rx_rve_sm_alpha_plattack;
  uint16 rx_rve_sm_alpha_pldecay;
  uint16 rx_rve_low_gain_limit;
  uint16 rx_rve_headroom;
  uint16 rx_rve_dshift;
  uint16 rx_rve_hf_dshift;
  uint16 rx_rve_scale_fact1;
  uint16 rx_rve_scale_fact2;
  uint16 rx_rve_peak_lim;
  uint16 rx_rve_hard_peak_lim;
  uint16 rx_rve_noise_sens_thresh[4];
  uint16 rx_rve_gain_mult_vector[4];
  uint16 rx_rve_upper_out_lim;
  uint16 rx_rve_op_mode;
  uint16 rx_rve_aeqVadThreshold;
  uint16 rx_rve_refVadHangoverMax;
} qdsp_cmd_rve_param_type;


/*
** Data structure containing values for  RVE  writable control parameters Command
*  in dual  mic condition
*/

typedef struct {

  uint16 rx_dual_rve_sm_alpha_noise;
  uint16 rx_dual_rve_sm_alpha_ref;
  uint16 rx_dual_rve_sm_alpha_rec;
  uint16 rx_dual_rve_sm_alpha_gain_increase;
  uint16 rx_dual_rve_sm_alpha_gain_decrease;
  uint16 rx_dual_rve_sm_alpha_amp;
  uint16 rx_dual_rve_sm_alpha_plattack;
  uint16 rx_dual_rve_sm_alpha_pldecay;
  uint16 rx_dual_rve_low_gain_limit;
  uint16 rx_dual_rve_headroom;
  uint16 rx_dual_rve_dshift;
  uint16 rx_dual_rve_hf_dshift;
  uint16 rx_dual_rve_scale_fact1;
  uint16 rx_dual_rve_scale_fact2;
  uint16 rx_dual_rve_peak_lim;
  uint16 rx_dual_rve_hard_peak_lim;
  uint16 rx_dual_rve_noise_sens_thresh[4];
  uint16 rx_dual_rve_gain_mult_vector[4];
  uint16 rx_dual_rve_upper_out_lim;
  uint16 rx_dual_rve_op_mode;
  uint16 rx_rve_aeqVadThreshold;
  uint16 rx_rve_refVadHangoverMax;
} qdsp_cmd_dual_rve_param_type;


/*
** Data values for the Echo Canceller configuration command
*/

/* Bit mask values for ec_switch
*/
#define QDSP_ECHO_CANCEL_M         0xFFFF /* Echo canceller enable mask       */
#define QDSP_ECHO_CANCEL_ENA_V     0xFFFF /* Enable echo canceller            */
#define QDSP_ECHO_CANCEL_DIS_V     0x0000 /* Disable echo canceller           */

/* Bit mask values for ec_mode
*/
#define QDSP_ECHO_CANCEL_MODE_M    0x0002 /* Echo canceller mode control mask */
#define QDSP_ECHO_CANCEL_ESEC_V    0x0000 /* Enable ESEC (ear seal echo)      */
#define QDSP_ECHO_CANCEL_HEADSET_V 0x0001 /* Enable Headset Echo Canceller    */
#define QDSP_ECHO_CANCEL_AEC_V     0x0002 /* Enable Acoustic Echo Canceller   */
#define QDSP_ECHO_CANCEL_SPEAKER_V 0x0003 /* Enable Speakerphone Echo Canceller */

/* Bit mask values for ec_reset_flag
*/
#define QDSP_ECHO_RES_EC_M         0x0002 /* Echo canceller reset mask        */
#define QDSP_ECHO_RES_EC_ENA_V     0x0002 /* Reset ec in next frame           */
#define QDSP_ECHO_RES_EC_DIS_V     0x0000 /* No ec reset                      */
#define QDSP_ECHO_RES_ECBNE_M      0x0001 /* EC BNE reset mask                */
#define QDSP_ECHO_RES_ECBNE_ENA_V  0x0001 /* Reset bne in next frame          */
#define QDSP_ECHO_RES_ECBNE_DIS_V  0x0000 /* No bne reset                     */

/* Data structure containing echo canceller configuration data
*/
typedef struct {
  uint16 farend_hangover_thresh;
  uint16 doubletalk_hangover_thresh;
  uint16 startup_mute_hangover_thresh;
  uint16 startup_mute_mode;
  uint16 startup_erle_thresh;
} qdsp_cmd_ec_params_type;

typedef struct {
  uint16 ec_switch;
  uint16 ec_mode;
  uint16 ec_reset_flag;
} qdsp_cmd_ec_config_type;

/*
** Bit mask values for noise suppressor control command
*/
#define QDSP_NOISE_SUP_M           0xFFFF /* Noise Suppressor enable mask     */
#define QDSP_NOISE_SUP_ENA_V       0xFFFF /* Enable Noise Suppressor          */
#define QDSP_NOISE_SUP_DIS_V       0x0000 /* Disable Noise Suppressor         */

/*
** Bit mask values for VOX control command
*/
#define QDSP_VOX_M                 0xFFFF /* VOX mask                         */
#define QDSP_VOX_ENA_V             0xFFFF /* Enable VOX                       */
#define QDSP_VOX_DIS_V             0x0000 /* Disable VOX                      */

/*
** Value for sidetone control command when it is disabled
*/
#define QDSP_SIDETONE_DIS_V        0x0000 /* Sidetone zero                  */

/*
** Bit mask values for HPF override command
*/
#define QDSP_HPF_OVERRIDE_M        0xFFFF /* HPF override enable mask         */
#define QDSP_HPF_OVERRIDE_ENA_V    0xFFFF /* Enable HPF override              */
#define QDSP_HPF_OVERRIDE_DIS_V    0x0000 /* Disable HPF override             */

/*
** Bit mask values for Network Echo Suppressor (NES) override command
*/
#define QDSP_NES_OVERRIDE_M        0xFFFF /* NES override mask                */
#define QDSP_NES_OVERRIDE_ENA_V    0xFFFF /* Enable NES override              */
#define QDSP_NES_OVERRIDE_DIS_V    0x0000 /* Disable NES override             */

#ifdef FEATURE_AVS_NEXTGEN_EC
/* Constants for MODE parameter of nextgen EC/NS */
#define QDSP_EC_MODE_OFF                   0x00 /* EC is off                  */
#define QDSP_EC_MODE_AF                    0x01 /* AF adaptive filter         */
#define QDSP_EC_MODE_DES                   0x02 /* Dynamic Echo Suppressor    */
#define QDSP_EC_MODE_NS                    0x04 /* Noise Suppressor (NS)      */
#define QDSP_EC_MODE_CNI                   0x08 /* Comfort Noise Injection    */
#define QDSP_EC_MODE_PNLES                 0x10 /* Post-processing non-linear */
                                                /*   echo suppression         */
#define QDSP_EC_MODE_HIGH_BAND_MUTE        0x20 /* High-band mute/active      */
#define QDSP_EC_MODE_HIGH_BAND_VAR_ATTEN   0x40 /* High-band variable atten.  */
                                                /*   off/active               */
#define QDSP_EC_MODE_PCD                   0x80 /* Path Change Detector (PCD) */
/* AF_erl_bg, AF_taps_bg, PCD_threshold, minimum_erl & erl_step are used with
** the PCD feature.
*/
#define QDSP_EC_MODE_FE_HPF               0x100 /* Farend high-pass filter    */
                                                /*   if NLPP is activated     */
#define QDSP_EC_MODE_NE_HPF               0x200 /* Nearend high-pass filter   */
#define QDSP_EC_MODE_FE_PP                0x400 /* Farend preprocessing (NLPP)*/
#define QDSP_EC_MODE_FNE                  0x800 /* Fast Noise Estimation (for */
                                                /*   LifeVibes Voice Clarity) */
#define QDSP_EC_MODE_AF_PP               0x1000 /* AF pre-processing          */
                                                /* (clipping)                 */

#define QDSP_EC_MODE_NORMAL              0x1e9b
#define QDSP_EC_MODE_HANDSET             0x1e1b
#define QDSP_EC_MODE_HANDSFREE           0x1e9b

/* Structure of configurable parameters for the nextgen EC */
typedef struct {
  uint16 NLPP_limit;          /* 0x0800 0x7FFF 0.0625 1.0 Q0.15               */
  uint16 NLPP_gain;           /* 0x0000 0x7FFF -Infinity dB +24 dB Q4.11    (1*/
  uint16 AF_limit;            /* 0x0800 0x7FFF 0.0625 1.0 Q0.15               */
  uint16 mode;                /* see defines above */
  uint16 tuning_mode;         /* */
  uint16 echo_path_delay;     /* in ms                                        */
  uint16 OutputGain;          /* 0x0000 0x7FFF -dB +24dB Q4.11                */
  uint16 InputGain;           /* 0x0000 0x7FFF -dB +12dB Q2.13                */
  uint16 AF_twoalpha;         /* 0x0000 0x7FFF 0 1.0 Q0.15                    */
  uint16 AF_erl;              /* 0x0000 0x7FFF -Infinity dB +54 dB Q9.6     (1*/
  uint16 AF_taps;             /* 12 200 12 200                                */
  uint16 AF_preset_coefs;     /* 0 2 0 2                                      */
  uint16 AF_offset;           /* 0x0000 0x7FFF 0 1.0 Q0.15                    */
  uint16 AF_erl_bg;           /* 0x0000 0x7FFF -Infinity dB +54 dB Q9.6       */
  uint16 AF_taps_bg;          /* 12 200 12 200                                */
  uint16 PCD_threshold;       /* 0x0000 0x7FFF 0 1 Q0.15                      */
  uint16 minimum_erl;         /* 0x0000 0x7FFF -Infinity dB +54 dB Q9.6     (1*/
  uint16 erl_step;            /* 0x0000 0x7FFF 0 2 Q1.14                      */
  uint16 max_noise_floor;     /* */
  uint16 det_threshold;       /* */
  uint16 SPDET_far;           /* 0x0000 0x7FFF 0 1.0 Q0.15                    */
  uint16 SPDET_mic;           /* 0x0000 0x7FFF 0 1.0 Q0.15                    */
  uint16 SPDET_xclip;         /* 0x0000 0x7FFF 0 6.15 dBm0                  (3*/
  uint16 DENS_tail_alpha;     /* 0x0000 0x7FFF 0 1.0 Q0.15                    */
  uint16 DENS_tail_portion;   /* 0x0000 0x7FFF 0 1.0 Q0.15                    */
  uint16 DENS_gamma_e_alpha;  /* 0x0000 0x7FFF 0 1.0 Q0.15                    */
  uint16 DENS_gamma_e_dt;     /* 0x0000 0x7FFF 0 128 Q7.8                     */
  uint16 DENS_gamma_e_low;    /* 0x0000 0x7FFF 0 128 Q7.8                     */
  uint16 DENS_gamma_e_rescue; /* 0x0000 0x7FFF 0 128 Q7.8                     */
  uint16 DENS_gamma_e_high;   /* 0x0000 0x7FFF 0 128 Q7.8                     */
  uint16 DENS_spdet_near;     /* 0x0000 0x7FFF 0 128 Q7.8                     */
  uint16 DENS_spdet_act;      /* 0x0000 0x7FFF 0 128 Q7.8                     */
  uint16 DENS_gamma_n;        /* 0x0000 0x7FFF 0 128 Q7.8                     */
  uint16 DENS_NFE_blocksize;  /* 0x0000 0x7FFF 0 32768                        */
  uint16 DENS_limit_NS;       /* 0x0000 0x7FFF 0 1.0 Q0.15                    */
  uint16 DENS_NL_atten;       /* 0x0000 0x7FFF -Infinity dB +24 dB Q4.11    (1*/
  uint16 DENS_CNI_level;      /* 0x0000 0x7FFF 0 2.0 Q1.14                    */
  uint16 WB_echo_ratio;       /* 0x0000 0x7FFF -Infinity dB +21 dB Q7.8     (2*/
} qdsp_cmd_nextgen_ec_parms_type;

#endif /* FEATURE_AVS_NEXTGEN_EC */

#ifdef FEATURE_AVS_CRYSTAL_SPEECH

#define QDSP_CS_MODE_OFF      0x0000
#define QDSP_CS_MODE_HANDSET  0xfdff

/* Structure of configurable parameters for Crystal Speech */
typedef struct {
  uint16 CSMode;
  uint16 CS_TuningMode;
  uint16 CS_echo_path_delay;
  uint16 AF1_twoalpha;
  uint16 AF1_erl;
  uint16 AF1_taps;
  uint16 AF1_preset_coefs;
  uint16 AF1_offset;
  uint16 AF2_twoalpha;
  uint16 AF2_erl;
  uint16 AF2_taps;
  uint16 AF2_preset_coefs;
  uint16 AF2_offset;
  uint16 PCD_twoalpha;
  uint16 PCD_offset;
  uint16 CSPCD_threshold;
  uint16 WgThreshold;
  uint16 MpThreshold;
  uint16 SF_init_table0[8];
  uint16 SF_init_table1[8];
  uint16 SF_taps;
  uint16 SF_twoalpha;
  uint16 DNNS_EchoAlphaRev;
  uint16 DNNS_EchoYcomp;
  uint16 DNNS_WbThreshold;
  uint16 DNNS_EchoGammaHi;
  uint16 DNNS_EchoGammaLo;
  uint16 DNNS_NoiseGammaS;
  uint16 DNNS_NoiseGammaN;
  uint16 DNNS_NoiseGainMinS;
  uint16 DNNS_NoiseGainMinN;
  uint16 DNNS_NoiseBiasComp;
  uint16 DNNS_AcThreshold;
  uint16 WB_echo_ratio_2mic;
  uint16 WB_Gamma_E;
  uint16 WB_Gamma_NN;
  uint16 WB_Gamma_SN;
  uint16 vcodec_delay0;
  uint16 vcodec_delay1;
  uint16 vcodec_len0;
  uint16 vcodec_len1;
  uint16 vcodec_thr0;
  uint16 vcodec_thr1;
  uint16 FixCalFactorLeft;
  uint16 FixCalFactorRight;
  uint32 vcodec_coeff0[15];
  uint32 vcodec_coeff1[15];
  uint16 CS_OutputGain;
  uint16 Enhance_MeuFilter1;
  uint16 Enhance_MeuFilter2;
  uint16 Fix_OverEstimation;
  uint16 dmNLPPlimit;
  uint16 dmNLPPgain;

  uint16 windNoiseCfgNoiseThresh;
  uint16 windNoiseSensitivity;
  uint16 windNoiseHangOver;
  uint16 windPowerSmAlpha;
  uint16 lpEnergySmAlpha;
  uint16 windNoiseEnergyOffset;
  uint16 windNoiseRatioSmAlpha;
  uint16 numCoefIirTdf2Lpf[6];
  uint16 denCoefIirTdf2Lpf[4];
  uint16 numCoefIirTdf2Hpf[6];
  uint16 denCoefIirTdf2Hpf[4];


} qdsp_cmd_crystal_speech_parms_type;

#endif  /* FEATURE_AVS_CRYSTAL_SPEECH */

#define QDSP_SLOWTALK_MODE_OFF      0x0000
#define QDSP_SLOWTALK_MODE_ON       0xffff

/* Structure of configurable parameters for slowtalk Speech */
typedef struct {

  uint16 ezHearTargetExpRatioIF;
  uint16 ezHearVocoderNBorWBIF;
  uint16 ezHearMaxLocalExpCompDiffIF;
  uint16 ezHearMinSilenceDurationIF;
  uint16 ezHearProcessDelayIF;
  uint16 ezHearMaxGenSamplesExpansionIF;
  uint16 ezHearAvadThreshIF;
  uint16 ezHearAvadThresh2IF;
  uint16 ezHearAvadPwrScaleIF;
  uint16 ezHearAvadHangoverMaxIF;
  uint16 ezHearAvadAlphaSNRIF;
  uint16 ezHearAvadAvarScaleIF;
  uint16 ezHearAvadSnrDiffMaxIF;
  uint16 ezHearAvadSnrDiffMinIF;
  uint16 ezHearAvadInitLengthIF;
  uint16 ezHearAvadMaxValIF;
  uint16 ezHearAvadInitBoundIF;
  uint16 ezHearAvadResetBoundIF;
  uint32 ezHearCirBufSizeIF;
  uint32 ezHearMinimuSimilarityIF;
  uint32 ezHearMinPastSimilarityIF;
  uint32 ezHearMinForceSimilarityIF;

} qdsp_cmd_slowtalk_parms_type;

typedef enum {
    QDSP_CMD_SLOWTALK_UPDATE_STATIC_PARAMS,
    QDSP_CMD_SLOWTALK_NO_UPDATE_STATIC_PARAMS
} qdsp_cmd_slowtalk_static_params_flag;


/* Definitions of EC generation ID */
typedef enum {
  QDSP_CMD_EC_GEN_LEGACY = 0x0000,
  QDSP_CMD_EC_GEN_NEXT   = 0x0001,
  QDSP_CMD_EC_GEN_CRYSTAL_SPEECH = 0x0002,
  QDSP_CMD_EC_GEN_MAX
} qdsp_cmd_ec_gen_id_type;

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */


#ifdef FEATURE_TTY

/* TTY modes for UMTS */
typedef enum {
  QDSP_TTY_MODE_OFF = 0,
  QDSP_TTY_MODE_ON,
  QDSP_TTY_MODE_FULL = QDSP_TTY_MODE_ON,
  QDSP_TTY_MODE_VCO,
  QDSP_TTY_MODE_HCO,
  QDSP_TTY_MODE_MAX
} qdsp_tty_mode_type;

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)

typedef enum {
  QDSP_PLAYBACK_STOP              = 0,
  QDSP_PLAYBACK_OTHER             = (1 << 0),
  QDSP_PLAYBACK_OTHER_STOP        = (1 << 1),
  QDSP_PLAYBACK_SCO               = (1 << 2),
  QDSP_PLAYBACK_A2DP              = (1 << 3),
  QDSP_PLAYBACK_DTMF_START        = (1 << 4),
  QDSP_PLAYBACK_DTMF_STOP         = (1 << 5),
  QDSP_PLAYBACK_DTMF_RINGER_START = (1 << 6),
  QDSP_PLAYBACK_DTMF_RINGER_STOP  = (1 << 7)

} QDSP_playback_enum_type;

typedef void (*qdsp_bt_cb_func_ptr_type)(QDSP_playback_enum_type);

#endif /*FEATURE_AVS_BT_SCO_REWORK */


/*
** Declarations for TTY Activity status
*/
#ifdef FEATURE_TTY_ACTIVITY_STATUS
typedef uint16 qdsp_tty_mode_type;

/* Function type of callback used to indicate TTY activity */
typedef void (*qdsp_tty_status_fn_type) (
  boolean encoder_status, /* TRUE if encoder is active, FALSE otherwise */
  boolean decoder_status  /* TRUE if decoder is active, FALSE otherwise */
);
#endif /* FEATURE_TTY_ACTIVITY_STATUS */

/*
** Bit mask values for TTY command
*/
#define QDSP_TTY_M                 0xFFFF /* TTY Mask                         */
#define QDSP_TTY_ENA_V             0x0001 /* Enable TTY                       */
#define QDSP_TTY_DIS_V             0x0000 /* Disable TTY                      */

/*
** TTY Interface type
*/
typedef enum {
  QDSP_CMD_INTF_TTY_1X = 0,
  QDSP_CMD_INTF_TTY_UMTS,
  QDSP_CMD_INTF_TTY_NONE
} qdsp_cmd_intf_tty_type;

#endif /* FEATURE_TTY */

/*
** Data values for the RX and TX PCM filters
*/

#define QDSP_CMD_TX_PCM_FILTER_BEFORE_EC   0xffff
#define QDSP_CMD_TX_PCM_FILTER_AFTER_EC    0x0


/* Data structure containing coefficients for the 13-tap FIR filters in the
** audio front end.
*/
typedef struct {
  uint16 tap0;     /* Filter Tap, h[0] and h[12], Q14                         */
  uint16 tap1;     /* Filter Tap, h[1] and h[11], Q14                         */
  uint16 tap2;     /* Filter Tap, h[2] and h[10], Q14                         */
  uint16 tap3;     /* Filter Tap, h[3] and h[9], Q14                          */
  uint16 tap4;     /* Filter Tap, h[4] and h[8], Q14                          */
  uint16 tap5;     /* Filter Tap, h[5] and h[7], Q14                          */
  uint16 tap6;     /* Filter Tap, h[6], Q14. The filter is disabled if this   */
                   /* tap is set to zero.                                     */
} qdsp_cmd_pcm_filter_type;

#ifdef FEATURE_IIR_FILTER
/* Data structure containing coefficients for the 3 stage 2nd order IIR filters
** in the audio front end.
**
** They are implemented with the following equation:
**    y(n)    = b0*x(n)+w1(n)
**    w1(n+1) = b1*x(n)-a1*y(n)+w2(n)
**    w2(n+1) = b2*x(n)-a2*y(n)
**    y(n)    = b0*x(n)+b1*x(n-1)+b2*x(n-2)-a1*y(n-1)-a2*y(n-2)
**
** Note: the coeffs are in signed Q30 double precision format
*/
#define QDSP_CMD_IIR_FILTER_ENABLE   0xffff
#define QDSP_CMD_IIR_FILTER_DISABLE  0x0

typedef struct {
  sint31  a1;
  sint31  a2;
  sint31  b0;
  sint31  b1;
  sint31  b2;
} qdsp_cmd_pcm_iir_filter_stage_type;

typedef struct {
  uint16                              flag;
  uint16                              stage_cnt;
  qdsp_cmd_pcm_iir_filter_stage_type  stage1;
  qdsp_cmd_pcm_iir_filter_stage_type  stage2;
  qdsp_cmd_pcm_iir_filter_stage_type  stage3;
  qdsp_cmd_pcm_iir_filter_stage_type  stage4;
  qdsp_cmd_pcm_iir_filter_stage_type  stage5;
} qdsp_cmd_pcm_iir_filter_type;
#endif /* FEATURE_IIR_FILTER */


#ifdef FEATURE_QTUNES_AAC
typedef enum {
  QDSP_CMD_AAC_DATA_FORMAT_ADIF,
  QDSP_CMD_AAC_DATA_FORMAT_ADTS,
  QDSP_CMD_AAC_DATA_FORMAT_RAW,
  QDSP_CMD_AAC_DATA_FORMAT_PSUEDO_RAW
} qdsp_cmd_aac_data_format_enum_type;

typedef struct {
  uint16                              buffer_factor;
  qdsp_cmd_aac_data_format_enum_type  aac_format;

  /* See ISO/IEC 14496-3:2001(E) for details on the parameters below */
  byte                                sample_freq_index;
  byte                                audio_object;
  byte                                channel_configuration;
  byte                                ep_config;
  byte                                aac_section_data_resilience_flag;
  byte                                aac_scalefactor_data_resilience_flag;
  byte                                aac_spectral_data_resilience_flag;
} qdsp_cmd_aac_config_type;
#endif /* FEATURE_QTUNES_AAC */

#ifdef FEATURE_QTUNES

typedef struct {
  uint32              sample_freq;
} qdsp_cmd_mp3_config_type;

#endif /* FEATURE_QTUNES */

#ifdef FEATURE_REAL_AUDIO

/* Song specific DSP configuration for RealAudio */
typedef struct {
  byte                decoder_mode;      /* Decoder (Gecko) mode     */
  byte                sample_rate;       /* Sampling rate            */
  byte                num_channels;      /* Number of channels       */
  uint16              samples_per_frame; /* Samples per frame        */
  uint16              num_regions;       /* Number of regions        */
  uint16              bits_per_frame;    /* Number of bits per frame */
  uint16              coupling_start;    /* Stereo coupling region   */
  uint16              coupling_quant;    /* Coupling quantization    */
  uint16              frame_erasure;     /* Frame erasure flag       */
} qdsp_cmd_ra_dsp_config_type;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
/* Adec configuration command to DSP for Audio decode */
typedef struct {
  uint16 bits_per_sample;      /* Valid bits per sample             */
  uint16 adec_type;          /* adec number. WMA = 0                */
  uint16 subcommand;         /* subcommand = 0, currently unused    */
  uint16 command_length;     /* command length, 0, currently unused */
  uint16 version;            /* version number 1 or 2               */
  uint16 channels;           /* number of channels 1 or 2           */
  uint16 bytes_per_second;   /* bytes per second for playing back   */
  uint16 sample_rate;        /* Vaild sample frequency in Hz        */
  uint16 encoder_options;    /* Encoder option                      */
  uint16 virtual_pkt_len;    /* VirtualPacketLength in ASF          */
  uint16 advanced_encoder_opt1; /* Support for LBR bitstream */
  uint16 channel_mask;         /* For stero or mono streams  */
  uint16 format_tag;           /* Codec ID: 0x162 or 0x166 for WMAPRO 10 */
  uint32 advanced_encoder_opt2; /* Support for LBR bitstream */
} qdsp_cmd_adec_config_type;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
/* AMR-WB+ DSP decoder configuration */
typedef struct {
  uint16 sample_rate;   /* Sampling rate in Hz  */
  uint16 dec_chan_mode; /* Decoder channel mode */
} qdsp_cmd_amr_wbp_config_type;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/* This type is used to receive data to be given to the audio decoder(s).
*/
typedef struct {
  uint8  *buf1;       /* First buffer of data               */
  uint32 length1;     /* Length, in bytes, of first buffer  */
  uint8  *buf2;       /* Second buffer of data              */
  uint32 length2;     /* Length, in bytes, of second buffer */
#ifdef FEATURE_GASYNTH_INTERFACE
  uint8  *buf_usage_mask;
#endif /* FEATURE_GASYNTH_INTERFACE */
} qdsp_cmd_adec_cmd_type;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

#ifdef FEATURE_QSYNTH_COMMON
/* QSynth audio decoder type settings
*/
#define QDSP_CMD_QSYNTH_ADEC_QCP       0xFFFF           /* QCelp 13K decoder */
#define QDSP_CMD_QSYNTH_ADEC_ADPCM     0x0000           /* ADPCM decoder     */
#define QDSP_CMD_QSYNTH_ADEC_YADPCM    0x0001           /* Y-ADPCM decoder   */
#define QDSP_CMD_QSYNTH_ADEC_PCM       0x0002           /* PCM decoder       */
#define QDSP_CMD_QSYNTH_ADEC_AMR       0x0003           /* AMR decoder       */

/* This structure is used to configure DLS structures
*/
typedef struct {
  uint32 hp_inst_def;  /* HP instrument definitions */
  uint32 hp_drum_def;  /* HP drum definitions       */
  uint32 hp_wave_info; /* HP waveform table         */
  uint32 hq_inst_def;  /* HQ instrument definitions */
  uint32 hq_drum_def;  /* HQ drum definitions       */
  uint32 hq_wave_info; /* HQ waveform table         */
  uint32 hq_inst_num;  /* Number of HQ instruments  */
  uint32 hq_drum_num;  /* Number of HQ drums        */
} qdsp_cmd_dls_config_type;
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_QSYNTH_ADPCM
/* These defines are used to set the format of audio decoder data */
#define QDSP_CMD_ADPCM_MODE_ADPCM         0x000A  /* ADPCM decoder           */
#define QDSP_CMD_ADPCM_MODE_YADPCM        0x000D  /* Y-ADPCM decoder         */
#define QDSP_CMD_ADPCM_MODE_SIGNED_PCM    0x000E  /* Signed PCM decoder      */
#define QDSP_CMD_ADPCM_MODE_UNSIGNED_PCM  0x000F  /* Unsigned PCM decoder    */
#define QDSP_CMD_PCM_MODE_SIGNED_8_PCM    0x000E  /* 8-bit signed PCM        */
#define QDSP_CMD_PCM_MODE_UNSIGNED_8_PCM  0x000F  /* 8-bit unsigned PCM      */
#define QDSP_CMD_PCM_MODE_SIGNED_16_PCM   0x000B  /* 16-bit signed PCM       */
#define QDSP_CMD_PCM_MODE_UNSIGNED_16_PCM 0x000C  /* 16-bit unsigned PCM     */

/* This type is used to indicate the type of audio decoder data
*/
typedef enum {
  QDSP_CMD_ADPCM_ADPCM_MODE,        /* ADPCM format          */
  QDSP_CMD_ADPCM_YADPCM_MODE,       /* Y-ADPCM format        */
  QDSP_CMD_ADPCM_SIGNED_PCM_MODE,   /* Signed PCM format     */
  QDSP_CMD_ADPCM_UNSIGNED_PCM_MODE, /* Unsigned PCM format   */
  QDSP_CMD_ADPCM_INVALID_MODE
} qdsp_cmd_adpcm_mode_type;

/* QSynth ADPCM format structure
** Data structure containing ADPCM file specifications.
*/
typedef struct {
  uint16 block_length;      /* Block length              */
  uint16 sampling_rate;     /* Sampling rate             */
  uint16 num_channels;      /* Number of channels        */
  uint16 bits_per_sample;   /* Number of bits per sample */
} qdsp_cmd_qsynth_adpcm_cmd_type;

/* This type is used to indicate the start mode for ADEC playback.
*/
typedef enum {
  QDSP_CMD_ADEC_START_MODE_NORMAL,  /* Start playback immediately         */
  QDSP_CMD_ADEC_START_MODE_DELAYED, /* Delayed start - load file and wait */
  QDSP_CMD_ADEC_START_MODE_INVALID
} qdsp_cmd_adec_start_mode_enum_type;
#endif /* FEATURE_QSYNTH_ADPCM */

#if defined(FEATURE_QSYNTH_ADRC) || defined(FEATURE_QTUNES_ADRC)
/* ADRC controls for Qsynth/Qtunes
** Data structure containing ADRC controls configuration data.
*/
typedef struct {
  uint16 flag;               /* Flag to enable/disable ADRC               */
  uint16 threshold;          /* Compression threshold                     */
  uint16 slope;              /* Compression slope                         */
  uint16 rms_tav;            /* RMS signal energy time-averaging constant */
  uint16 attack[2];          /* Attack constant                           */
  uint16 release[2];         /* Release constant                          */
  uint16 delay;              /* System delay                              */
} qdsp_cmd_audfmt_adrc_ctl_type; /* ADRC control type                     */
#endif /* FEATURE_QSYNTH_ADRC || FEATURE_QTUNES_ADRC */

#ifdef FEATURE_AUDFMT_FIR_FILTER
/* PCM filter controls for Qsynth/Qtunes
** Data structure containing coefficients for the FIR PCM filters in
** the Qsynth/Qtunes.
*/
typedef struct {
  uint16 flag;                /* Flag to enable/disable PCM filter        */
  const uint16 *coeff;        /* FIR filter coefficients                  */
  uint16 length;              /* Number of coefficients                   */
  uint16 factor;              /* Shift factor to compenstate for Q factor */
} qdsp_cmd_audfmt_pcm_filter_type;/* PCM filter control type              */

#endif /* FEATURE_AUDFMT_FIR_FILTER */

#ifdef FEATURE_AUDFMT_EQUALIZER
/* Equalizer control structure
*/
typedef struct {
  boolean      enable;        /* Flag to enable/disable Equalizer         */
  uint16       band;          /* Number of filter                         */
  const uint16 *coeff;        /* Coefficient arrays pointer               */
} qdsp_cmd_equalizer_type;
#endif /* FEATURE_AUDFMT_EQUALIZER */

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

#ifdef FEATURE_AUDFMT_QCONCERT
/* The following structure is used to defined the QConcert control
*/
typedef struct {
  boolean enable;             /* Flag to enable/disable qconcert          */
  uint16  mode;               /* Qconcert output mode                     */
  uint16  gain;               /* Gain value                               */
  uint16  spread;             /* Spread value                             */
  int16   geometry;           /* Geometry value                           */
  uint16  sample_rate;        /* Sample rate mode                         */
} qdsp_cmd_qconcert_config_type;
#endif /* FEATURE_AUDFMT_QCONCERT */

#ifdef FEATURE_AUDFMT_FIR_FILTER
#define QDSP_QTUNES_COEFF_MAX       128

#define QDSP_QSYNTH_COEFF_MAX       27
#endif /* FEATURE_AUDFMT_FIR_FILTER */

#ifdef FEATURE_AUDFMT_STF
/* Maximum supported sidechain tunning filters
*/
#define QDSP_CMD_AUDFMT_STF_MAX 4

/* The size of coefficient value for one filter in WORD
** 6 NCoeff word + 4 MCoeff word + 1 Shift word + 1 channel word
*/
#define QDSP_CMD_AUDFMT_STF_COEFF_SIZE (6 + 4 + 1 + 1)

/* The following structure is used to defined the sidechain tunning filter
*/
typedef struct {
  boolean enable;             /* Flag to enable/disable sidechain filter      */
  uint16  mainChainGain;      /* Gain that will be applied to the main filter */
  uint16  sideChainGain;      /* Gain that will be applied to the side filter */
  uint16  bands;              /* Number of sidechain tunning filter bands     */
  const uint16  *coeff;       /* Filter's coefficient value                   */
} qdsp_cmd_audfmt_sidechain_filter_type;
#endif /* FEATURE_AUDFMT_STF */

#ifdef FEATURE_AUDFMT_IIR_FILTER
/* Maximum supported iir pcm tunning filters
*/
#define QDSP_CMD_AUDFMT_IIR_FILTER_MAX 4

/* The size of coefficient value for one filter in WORD.
** 6 NCoeff word + 4 MCoeff word + 1 Shift word + 1 channel word
*/
#define QDSP_CMD_AUDFMT_IIR_FILTER_COEFF_SIZE (6 + 4 + 1 + 1)

/* The following structure is used to defined the sidechain tunning filter
*/
typedef struct {
  boolean enable;             /* Flag to enable/disable sidechain filter      */
  uint16  bands;              /* Number of sidechain tunning filter bands     */
  CAL_MEMORY uint16  *coeff;       /* Filter's coefficient value                   */
} qdsp_cmd_audfmt_iir_filter_type;
#endif /* FEATURE_AUDFMT_IIR_FILTER */


/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

          Q D S P 2   V R   E N G I N E  :  M O D E   A   C O M M A N D S

===========================================================================*/
/*
** Control values for VR Basic Command
*/

/* Bit mask values for VR Basic
*/
#define QDSP_VR_BASIC_MODE_M                 0x000E /* VR Basic Mode mask     */
#define QDSP_BARK_SCALE_AMPLITUDE_8K_HZ_V    0x0000 /* 8K Hz, narrow band     */
#define QDSP_BARK_SCALE_ENERGY_8K_HZ_V       0x0002 /* 8K Hz, narrow band     */
#define QDSP_BARK_SCALE_AMPLITUDE_16K_HZ_V   0x0004 /* 16K Hz, wide band      */
#define QDSP_BARK_SCALE_ENERGY_16K_HZ_V      0x0006 /* 16K Hz, wide band      */
#define QDSP_VR_MODE_A_SWITCH_M              0x0001 /* VR Mode A mask         */
#define QDSP_VR_MODE_A_OFF_V                 0x0000 /* VR Mode A is off       */
#define QDSP_VR_MODE_A_ON_V                  0x0001 /* VR Mode A is on        */
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_VOICE_PLAYBACK
/* A pointer to a function returning boolean, called with 2 parameters of
** type uint16* and uint16**. These function pointers will be used to
** get a frame of data for voice prompting. The first parameter will be the
** rate information and the second is a pointer to the actual frame data.
** The return parameter will indicate whether a tx frame is to be played to
** both paths, for an rx frame it will be ignored.
*/
typedef boolean (* qdsp_cmd_get_vp_frame_fn_type)( uint16*, uint16** );

/* This enum is used to indicate the path the voice prompt will be played to */
typedef enum {
  QDSP_CMD_VP_RX,
  QDSP_CMD_VP_TX
} qdsp_cmd_vp_path_enum_type;

typedef enum {
  QDSP_CMD_VP_MUX_NONE = 0,
  QDSP_CMD_VP_MUX_RX,
  QDSP_CMD_VP_MUX_TX,
  QDSP_CMD_VP_MUX_BOTH
}qdsp_cmd_vp_mux_type;

#define  QDSP_CMD_SECOND_DEC_RX_MUX_ENA_V   0x0001
#define  QDSP_CMD_SECOND_DEC_RX_MUX_DIS_V   0x0000
#define  QDSP_CMD_SECOND_DEC_TX_MUX_ENA_V   0xffff
#define  QDSP_CMD_SECOND_DEC_TX_MUX_DIS_V   0x0000

typedef enum {
  QDSP_CMD_SECOND_DEC_CONFIG_NONE =  0x0000,
  QDSP_CMD_SECOND_DEC_CONFIG_13K  =  0x8001,
  QDSP_CMD_SECOND_DEC_CONFIG_EVRC =  0x8002,
  QDSP_CMD_SECOND_DEC_CONFIG_AMR  =  0x8003,
  QDSP_CMD_SECOND_DEC_CONFIG_EFR  =  0x8004,
  QDSP_CMD_SECOND_DEC_CONFIG_FR   =  0x8005,
  QDSP_CMD_SECOND_DEC_CONFIG_HR   =  0x8007
}qdsp_cmd_second_dec_config_type;

#endif /* FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_VOICE_RECORD
/* This enumerated type is used to indicate the path in recording.
*/
typedef enum {
  QDSP_CMD_REC_REVERSE,  /* Record reverse link (tx path)         */
  QDSP_CMD_REC_FORWARD,  /* Record forward link (rx path)         */
  QDSP_CMD_REC_BOTH,     /* Record both reverse and forward links */
  QDSP_CMD_REC_NONE
} qdsp_cmd_rec_dir_enum_type;

/* This is the callback function type used to return frames back to
** the vocoder driver layer for recording.
*/
typedef void (* qdsp_cmd_rec_fn_type) ( qdsp_cmd_rec_dir_enum_type,
                                        uint16, uint16, uint16* );

#endif /* FEATURE_VOICE_RECORD */

#ifdef FEATURE_MM_REC
/* This is the callback function type used to return frames back to
** the vocoder driver layer for recording.
*/
typedef void (* qdsp_cmd_mm_rec_fn_type) (void);
#endif /* FEATURE_MM_REC */


#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */

#ifdef FEATURE_PHONE_VR
/* <EJECT> */

/*===========================================================================

                  Q D S P 2   V R   E N G I N E  :
                  M O D E   B / C   D A T A   D E C L A R A T I O N S

===========================================================================*/

/***************************************************************************/
/* Data values for VR Codec Config command                                 */
/***************************************************************************/

/* Software version number
*/
#define QDSP_VR_SW_VERSION_NUMBER        0x0555 /* SW VN expected by DSP   */

/***************************************************************************/
/* Data values for VR Capture Start Command                                */
/***************************************************************************/

/* Bit mask values for VR Capture Control
*/
#define QDSP_VR_WB_SWITCH_M              0x0001 /** VR WB switch mask      */
#define QDSP_VR_WIDEBAND_V               0x0000 /*  Wideband VR            */
#define QDSP_VR_NARROWBAND_V             0x0001 /*  Narrowband VR          */
#define QDSP_VR_REC_SWITCH_M             0x0002 /** VR record switch mask  */
#define QDSP_VR_REC_OFF_V                0x0000 /*  Utt recording off      */
#define QDSP_VR_REC_ON_V                 0x0002 /*  Utt recording on       */
#define QDSP_VR_OPERATION_M              0x001C /** VR operation mask      */
#define QDSP_VR_HANDSET_MODE_V           0x0000 /*  Handset mode           */
#define QDSP_VR_HFK_MODE_V               0x0004 /*  Hands free carkit      */
#define QDSP_VR_HEADSET_MODE_V           0x0008 /*  Headset mode           */
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */

/***************************************************************************/
/* Data values for VR Capture End Command                                  */
/***************************************************************************/

#define QDSP_CMD_MAX_EP_HYP              3 /* Max endpoint hypotheses      */

typedef struct
{
  uint16                     start_frame;
    /* Voice frame number at start of speech                               */
  uint16                     length;
    /* Voice frame number at end of speech                                 */

} qdsp_ep_pair_type;


typedef struct
{
  uint16                     total_num_hyps;
    /* Num of hyps generated by Endpoint Detector                          */
  qdsp_ep_pair_type          ep_pair_buf [QDSP_CMD_MAX_EP_HYP];
    /* Buffer to store start/endpoint pair of all endpoint hypotheses      */

} qdsp_cmd_capture_end_cmd_type;

/***************************************************************************/
/* Data values for VR Match Command                                        */
/***************************************************************************/

/* Bit mask values for VR Match Control
*/
#define QDSP_VR_MATCH_TYPE_M             0x0007 /** Match type mask        */
#define QDSP_VR_MODEB_SD_V               0x0000 /*  ModeB_SD               */
#define QDSP_VR_MODEB_SI_V               0x0001 /*  ModeB_SI               */
#define QDSP_VR_MODEC_SI_V               0x0002 /*  ModeC_SI               */
#define QDSP_VR_MODEBC_SISA              0x0003 /*  ModeBC_SISA            */
#define QDSP_VR_2ND_STAGE_M              0x0008 /** Second stage mask      */
#define QDSP_VR_1ST_STAGE_MATCH_V        0x0000 /*  1st stage match        */
#define QDSP_VR_2ND_STAGE_MATCH_V        0x0008 /*  2nd stage match        */
#define QDSP_VR_DIRECT_PAT_MATCH_M       0x0010 /** Direct pat match mask  */
#define QDSP_VR_DIRECT_PAT_MATCH_ON_V    0x0000 /*  Direct pat match on    */
#define QDSP_VR_DIRECT_PAT_MATCH_OFF_V   0x0010 /*  Direct pat match off   */

/* Structure containing match command data (to DSP.)
*/
typedef struct
{
  uint16 vr_match_control;   /* Match control parameters                   */
  uint16 num_hyps;           /* Number of hyps only for this match cmd     */
  uint16 ep_index_buf [QDSP_CMD_MAX_EP_HYP];  /* Endpoint hyp index buffer */

} qdsp_cmd_match_cmd_type;

/* Structure containing match result message data (from DSP.)
*/
typedef struct
{
  boolean match_res_avail;   /* Indicate match result is available         */
  uint16  *dma_score_addr;   /* DMA address to read VR scores              */
  uint16  num_vr_scores;     /* Number of valid VR scores                  */
  uint16  *dma_tr_pat_addr;  /* DMA address to read training pattern       */
  uint16  *dma_dl_num_addr;  /* DMA address to write number of dl patterns */
  uint16  *dma_dl_data_addr; /* DMA address to download pattern data       */
  uint16  pat_buf_size;      /* Size (in words) of next pattern download   */
  boolean valid_addr;        /* TRUE if DMA addresses are in  range        */

} qdsp_cmd_match_result_type;

/***************************************************************************/
/* General Data Declarations                                               */
/***************************************************************************/

/* This call back function is used to call the end point detection function.
** Note:  if qdsp_cmd_endpt_cb_type needs to be modified, also modify
** voc_endpt_cb_type.
*/
typedef void (*qdsp_cmd_endpt_cb_type)
(
  uint8                      *buf_ptr
  /* Ptr to buffer containing voc frame + vr diagnostic data + frame no    */
);

#endif /* FEATURE_PHONE_VR */

/* Virtual vocoder client identifiers used to map to actual app ids in QDSPS */
typedef enum {
#ifdef FEATURE_ACP
#error code not present
#endif
  QDSP_CMD_QDSPS_CLIENT_LOW,  /* Low priority client for all others */
  QDSP_CMD_QDSPS_CLIENT_MAX
} qdsp_cmd_qdsps_client_type;

#ifdef FEATURE_AUDIO_FORMAT
/* The following type is used to indicate the type of decoder used for
** AV sync function.
*/
typedef enum {
  QDSP_CMD_AV_SYNC_DEC_MP3,        /* MP3 decoder       */
  QDSP_CMD_AV_SYNC_DEC_AAC,        /* AAC decoder       */
  QDSP_CMD_AV_SYNC_DEC_RA,         /* RealAudio decoder */
  QDSP_CMD_AV_SYNC_DEC_VOCODER,    /* Vocoder decoder   */
  QDSP_CMD_AV_SYNC_DEC_WMA,        /* WMA decoder       */
  QDSP_CMD_AV_SYNC_DEC_WAVE,       /* PCM/ADPCM decoder */
  QDSP_CMD_AV_SYNC_DEC_AMR_WB,     /* AMR-WB decoder    */
  QDSP_CMD_AV_SYNC_DEC_AMR_WBPLUS, /* AMR-WB+ decoder   */
  QDSP_CMD_AV_SYNC_DEC_INVALID
} qdsp_cmd_av_sync_dec_enum_type;

typedef enum {
  QDSP_CMD_DEC_MP3,        /* MP3 decoder       */
  QDSP_CMD_DEC_AAC,        /* AAC decoder       */
  QDSP_CMD_DEC_RA,         /* RealAudio decoder */
  QDSP_CMD_DEC_VOCODER,    /* Vocoder decoder   */
  QDSP_CMD_DEC_WMA,        /* WMA decoder       */
  QDSP_CMD_DEC_WAVE,       /* PCM/ADPCM decoder */
  QDSP_CMD_DEC_AMR_WB,     /* AMR-WB decoder    */
  QDSP_CMD_DEC_AMR_WBPLUS, /* AMR-WB+ decoder   */
  QDSP_CMD_DEC_INVALID
} qdsp_cmd_decoder_inuse_type;

typedef void (* qdsp_cmd_av_sync_cb_type) (
  qword timestamp,     /* timestamp for the callback                      */
  qword num_of_samples,/* pcm sample counter. Use only lower 48 bits      */
  qword num_of_bytes   /* frame bytes counter. Use only for windows media */
);
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_QAUDIOFX
typedef enum {
  QDSP_CMD_3D_POS_MODE_ENABLE,
  QDSP_CMD_3D_POS_MODE_DISABLE,
  QDSP_CMD_3D_POS_MODE_INVALID
} qdsp_cmd_3d_pos_mode_enum_type;

/* This enumerated type is used to set the 3D positional output mode */
typedef enum {
  QDSP_CMD_3D_POS_OUTPUT_MODE_HEADPHONE,     /* Headphone output mode       */
  QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_FRONT, /* Speaker front firing mode   */
  QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_SIDE,  /* Speaker side firing mode    */
  QDSP_CMD_3D_POS_OUTPUT_MODE_SPEAKER_DESK,  /* Speaker desk output mode    */
  QDSP_CMD_3D_POS_OUTPUT_MODE_INVALID
} qdsp_cmd_3d_pos_output_mode_enum_type;

/* The following structure is used to represent a position in XYZ coordinates */
typedef struct {
  sint31 x_coord;  /* X-coordinate */
  sint31 y_coord;  /* Y-coordinate */
  sint31 z_coord;  /* Z-coordinate */
} qdsp_cmd_3d_pos_coord_type;

typedef qdsp_cmd_3d_pos_coord_type qdsp_cmd_3d_pos_vector_type;

/* The following structure is used to represent orientation */
typedef struct {
  qdsp_cmd_3d_pos_vector_type forward;  /* Forward vector */
  qdsp_cmd_3d_pos_vector_type up;       /* Up vector      */
} qdsp_cmd_3d_pos_orient_type;

/* The following structure is used to represent the source volume attenuation
** setting.
*/
typedef struct {
  uint32 max_distance; /* Distance at which sounds are played at max volume */
  uint32 min_distance; /* Distance beyond which sounds are muted            */
  uint32 rolloff;      /* Rolloff (0...1000), 1000: 1.0 (normal rolloff)    */
                       /*                        0: 0.0 (no rolloff)        */
  boolean mute;        /* Mute after Max: FALSE-Hold, TRUE-Mute after max   */
} qdsp_cmd_3d_pos_src_vol_atten_type;

/* This enum is used to define the reverb settings */
typedef enum{
  QDSP_CMD_3D_REVERB_PRESET,       /* Set Reverb Preset*/
  QDSP_CMD_3D_REVERB_LEVEL,        /* Set Reverb Reverb Level */
  QDSP_CMD_3D_REVERB_TIME,        /* Set Reverb decay time */
  QDSP_CMD_3D_REVERB_RATIO         /* Set Reverb decay ratio */
} qdsp_cmd_3d_reverb_cmd_type;

/* This structure is used to represent the reverb settings */
typedef struct{
  uint16 env_type;          /* Environment type                             */
  uint16 room_gain;         /* Room Gain                                    */
  uint32 decay_ms;          /* Decay Time in ms for low frequencies         */
  uint32 decay_ratio;       /* Decay time ratio btwn high and low frequencies */
  uint16 ref_delay;         /* Initial reflection delay time in ms          */
  uint16 reverb_gain;       /* Reverb gain relative to room effect          */
  uint16 reverb_delay;      /* Reverb delay relative to initial reflection in ms */
} qdsp_cmd_3d_reverb_param_type;

/* The following structure contains all 3D positional source parameters.
** It is used to return the 3D positional state of a source at the end of
** playback.
*/
typedef struct {
  boolean                            src_enable; /* Source enabled flag      */
  qdsp_cmd_3d_pos_coord_type         position;  /* Source position           */
  qdsp_cmd_3d_pos_src_vol_atten_type vol_atten; /* Source volume attenuation */
  sint31                             volume;    /* Source volume             */
  qdsp_cmd_3d_pos_vector_type        vel_vector; /* Source velocity vector   */
  sint31                             reverb_vol; /* Source reverb volume     */
} qdsp_cmd_3d_pos_src_all_setting_type;

/* This callback type is used to return the final 3D positional source
** settings to the client at the end of playback.
*/
typedef void (* qdsp_cmd_3d_pos_src_final_cb_type) (
  uint32                               channel,   /* ADEC channel queried */
  qdsp_cmd_3d_pos_src_all_setting_type *src_all   /* Source parameters    */
);
#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_GASYNTH_INTERFACE
typedef enum {
  QDSP_CMD_ADEC_VOC_QCP,
  QDSP_CMD_ADEC_VOC_AMR
} qdsp_cmd_adec_voc_enum_type;
#endif /* FEATURE_GASYNTH_INTERFACE */
/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

#ifdef FEATURE_VOC_PACKET_INTERFACE
/*===========================================================================

FUNCTION qdsp_cmd_register_packet_clients

DESCRIPTION
  Clients that use the packet interface to send/receive data must
  register callbacks using this function.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_cmd_register_packet_clients (
  qdsp_cmd_rl_packet_fn_type  rl_func,  /* Reverse Link Client function */
  qdsp_cmd_fl_packet_fn_type  fl_func   /* Forward Link Client function */
);
#endif /* FEATURE_VOC_PACKET_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_init

DESCRIPTION
  One time power-up initialization of the QDSP command layer driver.
  Setup DMA lookup table, interrupt vectors, etc.

DEPENDENCIES
  The pcm path flag used to setup the PCM path to normal operation
  (ADSP PCM data from AUX).

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_init ( boolean pcm_path );

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SET_IMAGE

DESCRIPTION
  Sets the ISR routine that QDSP Services will call based on the image,
  and the current active module in response to event MODULE from
  QDSP services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_set_image (
  qdsp_image_type image_select,
  qdsp_module_type module_select
);
/* <EJECT> */
/*===========================================================================

           Q D S P 2   B I O S   S Y S T E M   C O M M A N D S

===========================================================================*/
/*===========================================================================

FUNCTION qdsp_cmd_sleep

DESCRIPTION
  This function will send a command to the DSP to request the DSP to go to
  low power mode.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_sleep ( boolean aux_pcm_clk_enable );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_idle

DESCRIPTION
  This function will send a command to the DSP to request the DSP to go to
  idle mode.

DEPENDENCIES
  Audio Front End Image must be loaded.
  This command is only valid when the DSP is in Vocoder or DFM state.
  Note: Clear rxVolume to mute the audio.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  If the DSP is in DFM or VOCODER state, it will transition to IDLE state.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_idle ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_loopback_test

DESCRIPTION
  This function will send a Loopback Test Command to the DSP.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_loopback_test ( uint16 lb_ctrl );

/* <EJECT> */
#ifdef FEATURE_UP_LOOPBACK_TEST
/*===========================================================================

FUNCTION QDSP_CMD_UP_PACKET_LOOPBACK (uP packet loopback)

DESCRIPTION
 This function enables the microprocessor packet loopback interrupt
  service routine.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_up_packet_loopback ( boolean enable );

#endif  /* FEATURE_UP_LOOPBACK_TEST */
/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_get_diags

DESCRIPTION
  This function reads the QDSP2 Diagnostic Data

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_get_diags ( qdsp_cmd_diag_type *diag_data );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_get_state

DESCRIPTION
  This function reads the QDSP2 State

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern sint15 qdsp_cmd_get_state ( void );

/* <EJECT> */
/*===========================================================================

          Q D S P 2   M O D E M   M O D U L E   C O M M A N D S

===========================================================================*/
/*===========================================================================

FUNCTION qdsp_cmd_voc_config

DESCRIPTION
  This function will send a command to the DSP to configure the vocoder.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_voc_config ( uint16 config, uint16 enc, uint16 dec );

/* <EJECT> */
#ifdef FEATURE_GRAPH_AUDIO
/*===========================================================================

FUNCTION QDSP_CMD_SELECT_VOCODER (command 0x0300)

DESCRIPTION
  This function will send a command to the DSP to select vocoder mode.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_select_vocoder (
  uint16 voc_select
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_VERIFY_VOC_SELECT_DONE

DESCRIPTION
  This function will check the vocoder select done flag.

DEPENDENCIES
  This command is only valid when the DSP is in INIT state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qdsp_cmd_verify_voc_select_done (void);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SET_REC_FRAME_RATES

DESCRIPTION
  This function will set the previously stored frame rates.

DEPENDENCIES
  qdsp_cmd_rec_max_frame_rate, qdsp_cmd_rec_min_frame_rate

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_set_rec_frame_rates(void);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_AUDIO_CONFIG (command 0x0303)

DESCRIPTION
  This function will send a command to the DSP to configure the audio.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_audio_config (
  qdsp_cmd_audio_config_type *config
);
#endif /* FEATURE_GRAPH_AUDIO */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION QDSP_CMD_MM_REC_CONFIG

DESCRIPTION
  This function will send a command to the DSP to configure the multi media
  recording processing

DEPENDENCIES
  This command is only valid when the DSP is in audio entry state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS

  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_mm_rec_config (
  qdsp_cmd_mm_rec_format_type rec_format,/* Current encoder format            */
  qdsp_cmd_mm_rec_fn_type     cb_func    /* Callback for QDSP to request data */
);

/*===========================================================================

FUNCTION QDSP_CMD_MM_ENCODER_CONFIG (command 0x0304)

DESCRIPTION
  This function will send a command to the DSP to configure the
  audio encoder.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_mm_encoder_config (
  qdsp_cmd_aac_encoder_config_type *config
);

/*===========================================================================

FUNCTION QDSP_CMD_MM_REC_AUDIO_CONFIG (command 0x0303)

DESCRIPTION
  This function will send a command to the DSP to configure the audio.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_mm_rec_audio_config (void);

/*===========================================================================

FUNCTION qdsp_cmd_mm_rec_update_read_count

DESCRIPTION

DEPENDENCIES
  FEATURE_MM_REC

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void qdsp_cmd_mm_rec_update_read_count(uint32 read_cnt);

/*===========================================================================

FUNCTION qdsp_cmd_mm_rec_get_data

DESCRIPTION
  Retrieve encoded multimedia data frame from dsp

DEPENDENCIES
  FEATURE_MM_REC

RETURN VALUE
  TRUE  - success
  FALSE - failed

SIDE EFFECTS

===========================================================================*/
extern boolean qdsp_cmd_mm_rec_get_data (
  qdsp_cmd_record_buf_type *buf,
  qdsp_cmd_record_data_type *frame_info
);
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_VOC_ADPCM
/*===========================================================================

FUNCTION qdsp_cmd_adpcm_gain

DESCRIPTION
  This function will set/clear QDSP_adpcmRxGain variable

DEPENDENCIES
  This command is only valid when the DSP is in IDLE state to set,
  and at the ADPCM_VOCODER state to clear.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_adpcm_gain (
  boolean val
);

/*===========================================================================

FUNCTION qdsp_cmd_get_adpcm_mode

DESCRIPTION
  This function whether current qdsp adpcm status equals to ENABLED.

DEPENDENCIES
  This command is only valid when the DSP is in ADPCM VOCODER state.

RETURN VALUE
  FALSE is adpcm is disabled other TRUE

SIDE EFFECTS
  None

===========================================================================*/
boolean qdsp_cmd_get_adpcm_mode(void);
#endif /* FEATURE_VOC_ADPCM */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_voc_timing

DESCRIPTION
  This function will send a command to the DSP to command the DSP to start
  looking for the Vocoder Frame Reference Strobe and start encoding/decoding
  at the appropiate offsets.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_voc_timing
(
  const qdsp_cmd_voc_timing_type *timing
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_voc_frame_reference

DESCRIPTION
  This function requests the DSP behave as if a vocoder frame reference
  strobe (VFR) is received.

DEPENDENCIES
  This command is only valid ???

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  If the DSP is in IDLE state, it will transition to VOCODER state.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_voc_frame_reference ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_enc_ctrl

DESCRIPTION
  Send encoder rate control parameters to the vocoder.

DEPENDENCIES
  This command is valid???

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_enc_ctrl
(
  const qdsp_cmd_enc_ctrl_type *rate_info
);

/* <EJECT> */
#ifdef FEATURE_VOC_PACKET_INTERFACE
/*===========================================================================

FUNCTION QDSP_CMD_FRAME_RATE_CTRL

DESCRIPTION
  This function sends max and min encoder rate control parameters
  to the QDSP vocoder.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_frame_rate_ctrl
(
  const qdsp_cmd_enc_ctrl_type *rate_info
);
#endif /* FEATURE_VOC_PACKET_INTERFACE */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/* <EJECT> */
/*===========================================================================

          Q D S P 2   P A C K E T   E X C H A N G E

===========================================================================*/
/*===========================================================================

FUNCTION qdsp_cmd_rx_frame

DESCRIPTION
  This function sends a frame of data to the vocoder's decoder.

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was completed.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_rx_frame
(
  uint8  frame_rate,     /* vocoder frame rate                    */
  uint8  *data_ptr       /* pointer to vocoder packet data        */
);


/* <EJECT> */
#ifdef FEATURE_VOC_PACKET_INTERFACE
/*===========================================================================

FUNCTION QDSP_CMD_PROCESS_RX_FRAME

DESCRIPTION
  This function saves a frame of OTA data along with the frame information,
  to the DSP buffers.

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
 None

SIDE EFFECTS
  qdsp_cmd_rx_packet contains the decoder frame.
  Update qdsp_cmd_dec_frame_rate, qdsp_cmd_dec_packet_length.

===========================================================================*/
extern void qdsp_cmd_process_rx_frame (
  uint16 frame_rate,     /* vocoder frame rate                      */
  uint8  *data_ptr       /* pointer to vocoder packet data          */
);
#endif /* FEATURE_VOC_PACKET_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_tx_frame

DESCRIPTION
  This function gets a frame of data from the vocoders encoder.

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
  QDSP_RATE_BLANK_V    - Frame is blank
  QDSP_RATE_EIGHTH_V   - Eight rate frame
  QDSP_RATE_QUARTER_V  - Quarter rate frame
  QDSP_RATE_HALF_V     - Half rate frame
  QDSP_RATE_FULL_V     - Full rate frame

SIDE EFFECTS
  None

===========================================================================*/
extern uint16 qdsp_cmd_tx_frame
(
  uint16   max_rate,                       /* max vocoder rate                */
  uint16   min_rate,                       /* min vocoder rate                */
  uint8    **data_ptr                      /* pointer to packet data          */
);

/* <EJECT> */

#if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)

/*===========================================================================

FUNCTION qdsp_cmd_gsm_tx_packet

DESCRIPTION
  This function gets a frame of data from the GSM vocoders encoder.

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
  qdsp_cmd_gsm_frame_type: return one of the following values -
    QDSP_GSM_SID         : speech flag = 0, FR or EFR mode
    QDSP_GSM_SPEECH_GOOD : speech flag = 1, FR or EFR mode
    QDSP_AMR_SPEECH_GOOD : AMR mode encoder frame type
    QDSP_AMR_SID_FIRST
    QDSP_AMR_SID_UPDATE
    QDSP_AMR_NO_DATA

  qdsp_cmd_gsm_frame_rate_type: GSM encoder rate information

  qdsp_cmd_status_type: indicate whether the command was completed.

SIDE EFFECTS
  Codec mode for the next packet will be configured if it is in AMR mode.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_gsm_tx_packet
(
  uint16 *data_ptr,                               /* pointer to packet data */
  qdsp_cmd_gsm_frame_type *gsm_frame_type,
  qdsp_cmd_gsm_frame_rate_type *gsm_frame_rate,
  qdsp_cmd_gsm_frame_rate_type amr_next_frame_rate         /* AMR only */
);


/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_gsm_rx_packet

DESCRIPTION
  This function sends a frame of data to the GSM vocoder's decoder.

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was completed.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_gsm_rx_packet
(
  uint16 *data_ptr,                       /* pointer to packet data */
  qdsp_cmd_gsm_frame_type gsm_frame_id,   /* packet frame type */
  qdsp_cmd_gsm_frame_rate_type amr_frame_rate,     /* AMR only */
  boolean taf                    /* time alignment flag: not used for AMR */
);


#endif  /* MSMAUD_VOC_AMR || MSMAUD_VOC_EFR || MSMAUD_VOC_FR */


/* <EJECT> */
#ifdef FEATURE_MVS

#ifdef MSMAUD_VOC_AMR_WB
/*===========================================================================

FUNCTION QDSP_CMD_AMR_WB_REGISTER

DESCRIPTION
  Call this function to register the uplink and downlink processing funcitons
  for AMR_WB Voice Services.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_amr_wb_register(
  qdsp_amr_wb_ul_fn_type  uplink_func,
  qdsp_amr_wb_dl_fn_type  downlink_func
);

#endif /* MSMAUD_VOC_AMR_WB */


/* <EJECT> */
#if defined(MSMAUD_VOC_AMR)
/*===========================================================================

FUNCTION qdsp_cmd_amr_register

DESCRIPTION
  This function sets the function pointers for the AMR uplink and downlink
  processing functions.

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void qdsp_cmd_amr_register(
  qdsp_amr_ul_fn_type  uplink_func,
  qdsp_amr_dl_fn_type  downlink_func
);
#endif /* MSMAUD_VOC_AMR */

/* <EJECT> */
#if defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) */

/* <EJECT> */
#ifdef MSMAUD_VOC_HR
/*===========================================================================

FUNCTION qdsp_cmd_hr_register

DESCRIPTION
  This function sets the function pointers for the HR uplink and downlink
  processing functions.

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void qdsp_cmd_hr_register(
  qdsp_hr_ul_fn_type  uplink_func,
  qdsp_hr_dl_fn_type  downlink_func
);
#endif  /* MSMAUD_VOC_HR */

/* <EJECT> */
#ifdef MSMAUD_VOC_G711
/*===========================================================================

FUNCTION qdsp_cmd_g711_register

DESCRIPTION
  This function sets the function pointers for the G711 uplink and downlink
  processing functions.

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void qdsp_cmd_g711_register(
  qdsp_g711_ul_fn_type  uplink_func,
  qdsp_g711_dl_fn_type  downlink_func
);
#endif  /* MSMAUD_VOC_G711 */

#ifdef MSMAUD_VOC_G723
/*===========================================================================

FUNCTION qdsp_cmd_g723_register

DESCRIPTION
  This function sets the function pointers for the G723 uplink and downlink
  processing functions.

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void qdsp_cmd_g723_register(
  qdsp_g723_ul_fn_type  uplink_func,
  qdsp_g723_dl_fn_type  downlink_func
);
#endif  /* MSMAUD_VOC_G723 */

#endif  /* FEATURE_MVS */

/* <EJECT> */
/*===========================================================================

        Q D S P 2   A U D I O   F R O N T   E N D   C O M M A N D S

===========================================================================*/
/*===========================================================================

FUNCTION qdsp_cmd_codec_config

DESCRIPTION
  This function will send a Codec Configuration Command to the DSP along with
  deafult PCM sampling rate, which will be used if there are no active clients.


DEPENDENCIES
  This command is only valid when the DSP is in INIT, IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  If the DSP is in INIT state, it will transition to IDLE state.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_codec_config
(
   const qdsp_cmd_codec_config_type *data,
   qdsp_cmd_pcm_sample_rate_type default_frame_rate
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_sample_slip_mode

DESCRIPTION
  This function enables or disables automatic sample slipping.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_sample_slip_mode ( uint16 slip_mode );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_sample_slip_ctrl

DESCRIPTION
  Send this command to manually advance or retard PCM clock by one cycle.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.  The command will function only if the sample slipping mode is
  set to manual.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_sample_slip_ctrl ( uint16 slip_ctrl );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_dtmf

DESCRIPTION
  This function will send a DTMF Tone Request to the DSP.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_dtmf
(
  const qdsp_cmd_dtmf_type *data
);

/* <EJECT> */
#if defined(FEATURE_EXTERNAL_SDAC) || defined (MSMAUD_CODEC_CLOCK_CTRL)
/*===========================================================================

FUNCTION QDSP_CMD_CHECK_CMD_REG

DESCRIPTION

  This function will check the uP command register to see if the DSP has
  cleared it.


DEPENDENCIES

  None

RETURN VALUE

  True if the semaphore has been cleared
  False if the seaphore has not been cleared

SIDE EFFECTS

  None

 ==========================================================================*/
extern boolean qdsp_cmd_check_cmd_reg(void);
#endif

/* <EJECT> */
#if defined(FEATURE_QTUNES_COMMON) || defined(FEATURE_QSYNTH_COMMON)
/*===========================================================================

FUNCTION QDSP_CMD_MASTER_VOLUME

DESCRIPTION
  This function sets the master volume setting for audio formats.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_master_volume (
  uint16 vol
);
#endif /* FEATURE_QTUNES_COMMON || FEATURE_QSYNTH_COMMON */

#if defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT)
/*===========================================================================

FUNCTION QDSP_CMD_MIDI

DESCRIPTION

  This function will send a command to the DSP to play midi tones.


DEPENDENCIES

  This command is only valid when the DSP is in QSYNTH state.

RETURN VALUE

  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS

  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_midi (
  qdsp_cmd_midi_cmd_type *cmd_buf,
  uint32                 num_cmd,
  uint16                 volume
);
#endif /* defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT) */

/* <EJECT> */
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION QDSP_CMD_SYNTH_MVOL

DESCRIPTION
  This function sets the synth master volume.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_synth_mvol (
  uint16 vol
);
#endif /* FEATURE_MIDI */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_FIR_FILTER
/*===========================================================================

FUNCTION qdsp_cmd_set_audfmt_pcm_filter

DESCRIPTION
  This function configures the QSynth PCM Filter through DMA.

DEPENDENCIES
  FEATURE_AUDFMT_FIR_FILTER

RETURN VALUE
  qdsp_cmd_status_type

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_audfmt_pcm_filter
(
  qdsp_cmd_audfmt_pcm_filter_type *filter
);
#endif /* FEATURE_AUDFMT_FIR_FILTER */
/* <EJECT> */
#if defined(FEATURE_QSYNTH_ADRC) || defined(FEATURE_QTUNES_ADRC)
/*===========================================================================

FUNCTION qdsp_cmd_set_audfmt_adrc_ctl

DESCRIPTION
  This function configures the QSynth ADRC controls through DMA.

DEPENDENCIES
  FEATURE_QSYNTH_ADRC

RETURN VALUE
  qdsp_cmd_status_type

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_audfmt_adrc_ctl
(
  qdsp_cmd_audfmt_adrc_ctl_type *adrc
);
#endif /* FEATURE_QSYNTH_ADRC || FEATURE_QTUNES_ADRC */

#ifdef FEATURE_GASYNTH_INTERFACE
/* Callback function for setting a timer to send the audio decoder command
   again
*/
typedef void (*qdsp_cmd_adec_timer_cb_func_type)(
  boolean
);
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/* Callback function for requesting more data from the vocoder task.  Passed
** to isr config function for storage.
*/
typedef void (*qdsp_cmd_adec_int_cb_func_type) (
  void
);

/* List of supported WAV formats, broken down into specifics */
typedef enum {
  QDSP_CMD_WAV_FORMAT_ADPCM,         /* ADPCM format         */
  QDSP_CMD_WAV_FORMAT_YADPCM,        /* Y-ADPCM format       */
  QDSP_CMD_WAV_FORMAT_SIGNED_PCM,    /* Signed PCM format    */
  QDSP_CMD_WAV_FORMAT_UNSIGNED_PCM,  /* Unsigned PCM format  */
  QDSP_CMD_WAV_FORMAT_ALAW,          /* Wav, A-Law format    */
  QDSP_CMD_WAV_FORMAT_MULAW,         /* Wav, Mu-Law format   */
  QDSP_CMD_WAV_FORMAT_INVALID
} qdsp_cmd_wav_format_type;

/*===========================================================================

FUNCTION QDSP_CMD_WAV_CONFIG

DESCRIPTION
  This function will configure the WAV decoder parameters.

DEPENDENCIES
  This command is valid anytime.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_wav_config(qdsp_cmd_wav_format_type format);

/* <EJECT> */
#ifdef FEATURE_GASYNTH_INTERFACE
/*===========================================================================

FUNCTION qdsp_cmd_reset_adec_cmd_buf

DESCRIPTION
  A function resets the qsynth adec commands buffer as any currently queued
  commands are no longer valid.

DEPENDENCIES
  This function should only be invoked as QSYNTH DSP is transitioning to
  sleep state

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
extern void qdsp_cmd_reset_adec_cmd_buf(void);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_adec_timer_cb

DESCRIPTION
  A function sets the callback function pointer. The callback function is
  used to schedule rex timer to try sending adec command to DSP again.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
extern void qdsp_cmd_set_adec_timer_cb(
  qdsp_cmd_adec_timer_cb_func_type cb_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_qsynth_adec_cmd_retry

DESCRIPTION
  A retry function to issue audio decoder command that are put in the command
  buffer to DSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
extern void qdsp_cmd_qsynth_adec_cmd_retry(void);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_qsynth_change_adec_state

DESCRIPTION
  Changes the State of qdsp_cmd_qsynth_adpcm_data[channel].adec_state and
  maintains Start/Stop states (qdsp_cmd_qsynth_adec_state) based on the
  state of DSP. Issued indicates that the present command has been sent to
  DSP successfully. It follows the below structure:

DEPENDENCIES
  None

RETURN VALUE
   Returns TRUE if the state has been successfully changed.
   If this returns TRUE, the config/action command can be written to
   DSP. Returns FALSE when the present command should not be written
   to DSP.

SIDE EFFECTS
  None

==========================================================================*/
extern boolean qdsp_cmd_qsynth_change_adec_state(
  uint16  *adec_cmd,
  uint32  channel,
  boolean  issued
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_qsynth_voc_adec_start

DESCRIPTION
  This function configures audio decoder to process QCP or AMR-NB
  audio.

DEPENDENCIES
  This command is only valid when the DSP is in QSYNTH state.

RETURN VALUE
  A value of qdsp_cmd_status_type to inidicate whether the
  configuration command was sent to DSP correctly.

SIDE EFFECTS
  NONE

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_qsynth_voc_adec_start(
  uint32                             channel,
  qdsp_cmd_adec_voc_enum_type        voc_type,
  qdsp_cmd_adec_start_mode_enum_type start_mode
);

/*===========================================================================

FUNCTION qdsp_cmd_adec_data_consumed

DESCRIPTION
  This function detects decoder has finished consuming data.

DEPENDENCIES
  This command is only valid when the DSP is in QSYNTH state.

RETURN VALUE
  TRUE - data_is_needed flag is set
  FALSE - data_is_needed flag is not set

SIDE EFFECTS
  NONE

===========================================================================*/
boolean qdsp_cmd_adec_data_consumed(
  uint8 channel
);
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SET_ADEC_INT_CB

DESCRIPTION
  Set the client callback that will be invoked when a ADEC interrupt
  is received.

DEPENDENCIES
  This command is only valid when the DSP is in adec state.

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_adec_int_cb_ptr

===========================================================================*/
extern void qdsp_cmd_set_adec_int_cb (
  qdsp_cmd_adec_int_cb_func_type cb_func /* Callback for QDSP to request data */
);

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION qdsp_cmd_set_codec_update_int_cb

DESCRIPTION
  Set the client callback that will be invoked when a dual mono interrupt
  is received.

DEPENDENCIES
  This command is only valid when the DSP is in adec state.

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_adec_int_cb_ptr

===========================================================================*/
extern void qdsp_cmd_set_codec_update_int_cb(
  qdsp_cmd_dualmono_cb_type cb_func /* Callback for QDSP to request data */
);
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */


/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_ADEC_CONFIG (DMA Type Writable Information)

DESCRIPTION

  This function will send a command to the DSP to configure the ADEC processing.

DEPENDENCIES

  This command is only valid when the DSP is in QTUNES state.
  If the DSP is in IDLE state, this command will start the DFM processing.
  If the DSP is already in DFM state, this command changes the parameters
  as specified in the command buffer.

RETURN VALUE

  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS

  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_adec_config (
  qdsp_cmd_adec_int_cb_func_type cb_func /* Callback for QDSP to request data */
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_ADEC

DESCRIPTION
  This function will fill the DSP buffers with data.


DEPENDENCIES
  This command is only valid when the DSP is in QTUNES state.

RETURN VALUE
  The data lengths are updated to mark what was used to fill the DSP buffers
  completely.
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS

  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_adec (
  qdsp_cmd_adec_cmd_type *adec_cmd
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_adec_int_pending

DESCRIPTION
  This function checks for a pending ADEC interrupt.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an interrupt is pending.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean qdsp_cmd_adec_int_pending(void);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_ADEC_PAUSE

DESCRIPTION
  This function sets/clears the audio decoder pause flag.

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_adec_pause (
  boolean pause  /* TRUE if output should be paused */
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_MASTER_PAN

DESCRIPTION
  This function sets the master pan setting for audio formats.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_master_pan (
  sint7 pan
);
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

/* <EJECT> */
#ifdef FEATURE_ADEC
/*===========================================================================

FUNCTION QDSP_CMD_ADEC_FLUSH

DESCRIPTION
  This function flushes the DSPs ADEC buffers.

DEPENDENCIES
  Must be in QTunes state.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_adec_flush ( uint16 flush_mask );

/* <EJECT> */
#ifdef FEATURE_QSYNTH_ADPCM
/*===========================================================================

FUNCTION QDSP_CMD_QSYNTH_ADPCM_STOP

DESCRIPTION
  This function will stop a QSynth adpcm channel.

DEPENDENCIES
  This command is only valid when the DSP is in QSYNTH state and decoder is
  ADPCM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_qsynth_adpcm_stop ( uint32 channel );

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_QSYNTH_ADPCM_START

DESCRIPTION
  This function will initialize a QSynth adpcm channel.

DEPENDENCIES
  This command is only valid when the DSP is in QSYNTH state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_qsynth_adpcm_start (
  uint32                             channel,
  qdsp_cmd_qsynth_adpcm_cmd_type     *adec_cmd,
  qdsp_cmd_adpcm_mode_type           mode,
  qdsp_cmd_adec_start_mode_enum_type start_mode
);
#endif /* FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION QDSP_CMD_ADEC_MVOL

DESCRIPTION
  This function sets the adec master volume.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_adec_mvol (
  uint16 vol
);
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_ADEC_CHAN_VOL

DESCRIPTION
  This function sets the audio decoder channel volume setting.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_adec_chan_vol (
  uint32 channel,
  uint16 volume
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_ADEC_CHAN_PAN

DESCRIPTION
  This function sets the audio decoder channel stereo pan setting.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_adec_chan_pan (
  uint32 channel,
  sint15 pan
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_ADEC_CHAN_START

DESCRIPTION
  This function starts the audio decoder channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_adec_chan_start (
  uint32  channel
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_ADEC_CHAN_PAUSE

DESCRIPTION
  This function sets the audio decoder channel stereo pause flag.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_adec_chan_pause (
  uint32  channel,
  boolean pause_flag
);

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION QDSP_CMD_ADEC_RESET

DESCRIPTION
  This function resets the DSPs audio decoder.

DEPENDENCIES
  Must be in QSynth state.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_adec_reset ( uint32 reset_mask );
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_adec_pcm_stopped

DESCRIPTION
  This function checks if PCM output has stopped.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if PCM output has stopped.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean qdsp_cmd_adec_pcm_stopped(void);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_adec_error

DESCRIPTION
  This function returns (and clears) any ADEC errors pending.

DEPENDENCIES
  None.

RETURN VALUE
  The value of the error register is returned.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 qdsp_cmd_adec_error(
  boolean clear
);

#ifdef FEATURE_QTUNES_COMMON
/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_mpeg_target_header

DESCRIPTION
  This function updates the value of the target header and mask in QTUNES.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void qdsp_cmd_mpeg_target_header(
  const uint8 *mask,
  const uint8 *header,
  const uint8 mask_size    /* size in bytes */
);
#endif /* FEATURE_QTUNES_COMMON */

/* <EJECT> */
#ifdef FEATURE_QTUNES_AAC
#ifdef FEATURE_AAC_PLUS
/*===========================================================================

FUNCTION qdsp_cmd_set_aac_plus_on_flag

DESCRIPTION
  This function setup AAC Plus on/off flag to enable/disable the AAC Plus SBR
  module.
  Note: The DSP will take in the value in the next call to
        qdsp_cmd_set_aac_config.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qdsp_cmd_set_aac_plus_on_flag
(
  boolean on
);

#ifdef FEATURE_ENHANCED_AAC_PLUS
/*===========================================================================

FUNCTION qdsp_cmd_set_enhanced_aac_plus_on_flag

DESCRIPTION
  This function setup Enhanced AAC Plus on/off flag to enable/disable the AAC
  Plus SBR module.
  Note: The DSP will take in the value in the next call to
        qdsp_cmd_set_aac_config.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qdsp_cmd_set_enhanced_aac_plus_on_flag
(
  boolean on
);
#endif /* FEATURE_ENHANCED_AAC_PLUS */
#endif /* FEATURE_AAC_PLUS */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_aac_config

DESCRIPTION
  Initializes the ARM buffer size to optimize playback on a specific AAC song.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qdsp_cmd_set_aac_config(
  const qdsp_cmd_aac_config_type *aac_config
);
#endif /* FEATURE_QTUNES_AAC */

/* <EJECT> */
#ifdef FEATURE_QTUNES
/*===========================================================================

FUNCTION qdsp_cmd_set_mp3_config

DESCRIPTION
  Initializes the ARM buffer size to optimize playback on a specific MP3 song.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qdsp_cmd_set_mp3_config(
  const qdsp_cmd_mp3_config_type *mp3_config
);
#endif /* FEATURE_QTUNES */

/* <EJECT> */
#ifdef FEATURE_REAL_AUDIO
/*===========================================================================

FUNCTION qdsp_cmd_set_ra_song_config

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
void qdsp_cmd_set_ra_song_config(
  const qdsp_cmd_ra_dsp_config_type *ra_config
);
#endif /* FEATURE_REAL_AUDIO */

/* <EJECT> */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
/*===========================================================================

FUNCTION qdsp_cmd_set_amr_wbp_config

DESCRIPTION
  Sets up AMR-WB+ specific DSP configuration parameters.

DEPENDENCIES
  qdsp_cmd_sample_rate_values

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qdsp_cmd_set_amr_wbp_config(
  const qdsp_cmd_amr_wbp_config_type *config
);
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

#endif /* FEATURE_ADEC */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_enable

DESCRIPTION
  This function enable/disable 3D environment.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_enable (
  qdsp_cmd_3d_pos_mode_enum_type mode
);

/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_assign_query_buffs

DESCRIPTION
  Assign 3D query buffers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qdsp_cmd_3d_pos_assign_query_buffs(void);

/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_reset_env

DESCRIPTION
  Resets 3D environment.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_reset_env (void);

/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_set_output_mode

DESCRIPTION
  Sets the 3D positional output mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_set_output_mode (
  qdsp_cmd_3d_pos_output_mode_enum_type output_mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_set_lsnr_pos

DESCRIPTION
  Sets the 3D positional listener position.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_set_lsnr_pos (
  qdsp_cmd_3d_pos_coord_type *coord,
  uint32                     duration_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_set_lsnr_orient

DESCRIPTION
  Sets the 3D positional listener orientation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_set_lsnr_orient (
  qdsp_cmd_3d_pos_orient_type *orient,
  uint32                      duration_ms
);

/* <EJECT> */
/*===========================================================================
FUNCTION qdsp_cmd_3d_pos_set_lsnr_vel

DESCRIPTION
  Sets the 3D positional listener position.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_set_lsnr_vel (
  qdsp_cmd_3d_pos_coord_type *coord,
  uint32                     duration_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_reverb_reset

DESCRIPTION
  This function clears reverb preset and set the reverb enable flag to
  FALSE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_cmd_3d_reverb_reset (void);

/*===========================================================================

FUNCTION qdsp_cmd_3d_reverb_set

DESCRIPTION
  Sets the 3D reverb parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_reverb_set(
  qdsp_cmd_3d_reverb_cmd_type    cmd,
  qdsp_cmd_3d_reverb_param_type *param
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_reverb_disable

DESCRIPTION
  Enables/Disables the 3D reverb.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_reverb_enable(
  boolean    enable
);

/*===========================================================================

FUNCTION qdsp_cmd_3d_reverb_check

DESCRIPTION
  Enables/Disables the 3D reverb on DSP when loaded.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qdsp_cmd_3d_reverb_check ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_src_enable
DESCRIPTION
  This function enable/disable 3D effect on given sound source.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_src_enable (
  uint32 src_adec_num,
  qdsp_cmd_3d_pos_mode_enum_type mode
);

/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_reset_src

DESCRIPTION
  Resets the 3D positional source.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_reset_src (
  uint32 src_adec_num
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_set_src_vol

DESCRIPTION
  Sets the 3D positional source volume.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_set_src_vol (
  uint32 src_adec_num,
  uint16 volume,
  uint32 duration_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_set_src_vol_atten

DESCRIPTION
  Sets the 3D positional source volume attenuation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_set_src_vol_atten (
  uint32                             src_adec_num,
  qdsp_cmd_3d_pos_src_vol_atten_type *vol_atten
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_set_src_pos

DESCRIPTION
  Sets the 3D positional source position.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_set_src_pos (
  uint32                     src_adec_num,
  qdsp_cmd_3d_pos_coord_type *coord,
  uint32                     duration_ms
);
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_set_src_vel

DESCRIPTION
  Sets the 3D positional source velocity.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_set_src_vel (
  uint32                     src_adec_num,
  qdsp_cmd_3d_pos_coord_type *coord,
  uint32                     duration_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_reverb_set_src_vol

DESCRIPTION
  Sets the 3D positional source velocity.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_reverb_set_src_vol (
  uint32                channel,
  uint16                volume
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_env_enabled

DESCRIPTION
  Queries if 3D environment is currently enabled or not.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_env_enabled(
  boolean *flag
);

/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_lsnr_pos

DESCRIPTION
  Queries the 3D positional listener position.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_lsnr_pos (
  qdsp_cmd_3d_pos_coord_type *coord
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_lsnr_orient

DESCRIPTION
  Queries the 3D positional listener orientation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_lsnr_orient (
  qdsp_cmd_3d_pos_orient_type *orient
);
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_reverb_enabled

DESCRIPTION
  Queries if reverb is currently enabled or not.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_reverb_enabled(
  boolean *flag
);

/*==========================================================================
  FUNCTION qdsp_cmd_3d_pos_query_reverb_setting

DESCRIPTION
  Queries current reverb setting

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
 ==========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_reverb_setting(
  qdsp_cmd_3d_reverb_param_type *reverb
);
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_lsnr_vel

DESCRIPTION
  Queries the 3D positional listener velocity.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_lsnr_vel (
  qdsp_cmd_3d_pos_vector_type *coord
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_src_final

DESCRIPTION
  Queries the 3D positional source settings at end of playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_src_final (
  uint32                            src_adec_num,
  qdsp_cmd_3d_pos_src_final_cb_type cb_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_src_enabled

DESCRIPTION
  Queries if source 3D effect is currently enabled or not.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_src_enabled(
  uint32  src_adec_num,
  boolean *flag
);
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_src_pos

DESCRIPTION
  Queries the 3D positional source position.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_src_pos (
  uint32                     src_adec_num,
  qdsp_cmd_3d_pos_coord_type *coord
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_src_vol

DESCRIPTION
  Queries the 3D positional source volume.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_src_vol (
  uint32 src_adec_num,
  uint16 *volume
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_src_vol_atten

DESCRIPTION
  Queries the 3D positional source volume attenuation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_src_vol_atten (
  uint32                             src_adec_num,
  qdsp_cmd_3d_pos_src_vol_atten_type *vol_atten
);


/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_src_velocity

DESCRIPTION
  Queries the 3D positional source position.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_src_vel (
  uint32                     src_adec_num,
  qdsp_cmd_3d_pos_vector_type *coord
);

/*===========================================================================

FUNCTION qdsp_cmd_3d_pos_query_src_revol

DESCRIPTION
  Queries the 3D positional source reverb volume.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_3d_pos_query_src_revol (
  uint32 src_adec_num,
  uint16 *reverb_volume
);
#endif /* FEATURE_QAUDIOFX */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_dtmf_path

DESCRIPTION
  This function selects the audio path for DTMF generation.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_dtmf_path
(
  uint16 dtmf_path
);

/* <EJECT> */

#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_EFR) \
    || defined(MSMAUD_VOC_FR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_AMR_WB_COMMON)

/*===========================================================================

FUNCTION qdsp_cmd_set_gsm_enc_dtx

DESCRIPTION
  This function switches encoder DTX mode on or off.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_gsm_enc_dtx
(
  qdsp_cmd_gsm_dtx_mode_type dtx_mode
);
#endif /* MSMAUD_VOC_AMR_COMMON || MSMAUD_VOC_EFR ||
          MSMAUD_VOC_FR || MSMAUD_VOC_HR || MSMAUD_VOC_AMR_WB_COMMON */


/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_WB_COMMON
/*===========================================================================

FUNCTION qdsp_cmd_set_amr_wb_codec_mode

DESCRIPTION
  This function will configure the codec mode for the next AMR_WB packet.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_amr_wb_codec_mode
(
  qdsp_cmd_amr_wb_frame_rate_type amr_wb_tx_rate
);

#endif /* MSMAUD_VOC_AMR_WB_COMMON */


/* <EJECT> */
#ifdef MSMAUD_VOC_AMR_COMMON
/*===========================================================================

FUNCTION qdsp_cmd_set_amr_codec_mode

DESCRIPTION
  This function will configure the codec mode for the next AMR packet.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_amr_codec_mode
(
  qdsp_cmd_gsm_frame_rate_type amr_tx_rate
);

#endif /* MSMAUD_VOC_AMR_COMMON */

/* <EJECT> */
#ifdef MSMAUD_VOC_G711
/*===========================================================================

FUNCTION qdsp_cmd_set_g711_mode

DESCRIPTION
  This function will configure the codec mode for the next G711 frame.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_g711_mode
(
  qdsp_cmd_g711_mode_type g711_mode
);
#endif /* MSMAUD_VOC_G711 */

/* <EJECT> */
#ifdef MSMAUD_VOC_G723
/*===========================================================================

FUNCTION qdsp_cmd_set_g723_mode

DESCRIPTION
  This function will configure the codec mode for the next G723 frame.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_g723_mode
(
  qdsp_cmd_g723_mode_type g723_mode
);
#endif /* MSMAUD_VOC_G723 */

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_vol_ctrl

DESCRIPTION
  This function controls the rx and tx volume and muting.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_vol_ctrl
(
  const qdsp_cmd_vol_ctrl_type *vol_set
);

/* <EJECT> */
#ifdef MSMAUD_PCM_DC_OFFSET
/*===========================================================================

FUNCTION QDSP_CMD_CODEC_OFFSET_INUSE

DESCRIPTION
  This function checks if PCM offset compensation is in effect.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qdsp_cmd_codec_offset_inuse(void);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_CODEC_OFFSET_CTRL (DMA Type Writable Information)

DESCRIPTION
  This function controls the QDSP2 internal codec offset.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_codec_offset_ctrl (
  const qdsp_cmd_codec_offset_ctrl_type *codec_offset
);

#endif  /* MSMAUD_PCM_DC_OFFSET */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_codec_gain_ctrl

DESCRIPTION
  This function controls the QDSP2 internal CODEC gain.

DEPENDENCIES
  None.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_codec_gain_ctrl
(
  const qdsp_cmd_codec_gain_ctrl_type *codec_set,
  qdsp_cmd_codec_gain_ctrl_lock_type lock
);

/* <EJECT> */
#ifdef FEATURE_AUDIO_AGC
/*===========================================================================

FUNCTION qdsp_cmd_agc_config

DESCRIPTION
  This function will set the AGC parameters

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_agc_config
(
  const qdsp_cmd_agc_config_type *data
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_AGC_PARAM (DMA Type Writable Information)

DESCRIPTION

  This function sets the QDSP2 AGC parameters.

DEPENDENCIES

  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE

  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS

  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_agc_param
(
  const qdsp_cmd_agc_param_type *data
);
#endif
/* <EJECT> */


/*===========================================================================

  FUNCTION qdsp_cmd_rve_is_supported

  DESCRIPTION

  	This function checks whether RVE support is available or not.

  DEPENDENCIES
  	None

  RETURN VALUE
  	A value of qdsp_cmd_status_type indicating whether the RVE is present
        in firmware or not.
===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_rve_is_supported( void );

/* <EJECT> */

/*===========================================================================

FUNCTION qdsp_cmd_rve_config

DESCRIPTION
  This function will set the RVE parameters

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_rve_config
(
  const qdsp_cmd_rve_config_type *data
);

/* <EJECT> */

/*===========================================================================

FUNCTION QDSP_CMD_RVE_PARAM (DMA Type Writable Information)

DESCRIPTION
  This function sets the QDSP2 RVE SINGLE  parameters.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_rve_param (
  const qdsp_cmd_rve_param_type *data
);

/* <EJECT> */


/*===========================================================================

FUNCTION QDSP_CMD_DUAL_RVE_PARAM (DMA Type Writable Information)

DESCRIPTION
  This function sets the QDSP2 RVE DUAL MIC parameters.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_dual_rve_param (
  const qdsp_cmd_dual_rve_param_type *data
);

/* <EJECT> */

/*===========================================================================

FUNCTION QDSP_CMD_GET_EC_GEN_ID (DMA Type Writable Information)

DESCRIPTION
  This function gets EC generation ID.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  qdsp_cmd_ec_gen_id_type

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_ec_gen_id_type qdsp_cmd_get_ec_gen_id(void);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_ec_ctrl

DESCRIPTION
  This function sets the echo cancellation mode

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_ec_ctrl
(
  const qdsp_cmd_ec_config_type *ec_config,
  const qdsp_cmd_ec_params_type *ec_params
);

/* <EJECT> */
#ifdef FEATURE_AVS_NEXTGEN_EC
/*===========================================================================

FUNCTION QDSP_CMD_NEXTGEN_EC_CTRL (DMA Type Writable Information)

DESCRIPTION
  This function sets the QDSP echo cancellation mode.

DEPENDENCIES
  This command is valid when the DSP is in IDLE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_nextgen_ec_ctrl (
  uint16                                ec_switch,
  const qdsp_cmd_nextgen_ec_parms_type  *ec_parms
);
#endif /* FEATURE_AVS_NEXTGEN_EC */

/* <EJECT> */
#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/*===========================================================================

FUNCTION QDSP_CMD_CRYSTAL_SPEECH_CTRL (DMA Type Writable Information)

DESCRIPTION
  This function sets the QDSP echo cancellation noise suppression mode.

DEPENDENCIES
  This command is valid when the DSP is in IDLE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_crystal_speech_ctrl (
  uint16                                    cs_switch,
  const qdsp_cmd_crystal_speech_parms_type  *cs_parms
);


#endif /* FEATURE_AVS_CRYSTAL_SPEECH */
/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_ns_ctrl

DESCRIPTION
  This function enables or disables the noise suppressor.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_ns_ctrl ( uint16 ns_switch );

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SIDETONE_CTRL (DMA Type Writable Information)

DESCRIPTION
  This function writes the sidetone to the DMA variable.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_sidetone_ctrl (
  boolean sidetone_switch, uint16 st_gain
);
/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_VOX_CTRL (DMA Type Writable Information)

DESCRIPTION
  This function enables or disables the VOX (half-duplex voice).

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_vox_ctrl (
  uint16 vox_switch
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_hpf_ctrl

DESCRIPTION
  This function enables or disables the HPF.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_hpf_ctrl ( uint16 hpf_switch );

/* <EJECT> */
#ifdef FEATURE_ACP
#error code not present
#endif  /* FEATURE_ACP */

/* <EJECT> */
#ifdef FEATURE_TTY

#ifdef FEATURE_TTY_ACTIVITY_STATUS
/*===========================================================================

FUNCTION qdsp_cmd_register_tty_status_client

DESCRIPTION
  Use this function to register a callback which will receive TTY activity
  status updates.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_register_tty_status_client (
  qdsp_tty_status_fn_type tty_func /* Callback to indicate TTY activity */
);
#endif /* FEATURE_TTY_ACTIVITY_STATUS */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_GET_TTY_INTF

DESCRIPTION
  This function gives the available TTY interface

DEPENDENCIES
  This command is valid when the DSP is in Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_intf_tty_type indicating the TTY interface type

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_intf_tty_type qdsp_cmd_get_tty_intf (void);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_TTY_CTRL (DMA Type Writable Information)

DESCRIPTION
  This function enables or disables TTY checking.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_tty_ctrl (
  uint16              tty_switch,
  qdsp_tty_mode_type  tty_mode
);

#endif /* FEATURE_TTY */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_get_pcmpad

DESCRIPTION
  This function reads the PCM pad value

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  The value of the PCM padding.

SIDE EFFECTS
  None

===========================================================================*/
extern uint16 qdsp_cmd_get_pcmpad ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_pcmpad

DESCRIPTION
  This function sets the PCM pad value.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_pcmpad ( uint16 padval );
/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_pre_ec_tx_pcm_filter

DESCRIPTION
  This function will set the pre EC Tx PCM filter flag.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_set_pre_ec_tx_pcm_filter(uint16 *pre_ec_tx_pcm_filter);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_rx_pcm_filter

DESCRIPTION
  This function will load a new set of RX PCM filter coefficients.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE, Vocoder
  or DFM state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_rx_pcm_filter
(
  const qdsp_cmd_pcm_filter_type *coefficients
);

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_tx_pcm_secondmic_filter

DESCRIPTION
  This function will load a new set of TX PCM filter coefficients for secondary mic.

DEPENDENCIES
  This command is valid???

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_tx_pcm_secondmic_filter
(
  const qdsp_cmd_pcm_filter_type *second_mic_coefficients
);
#endif /*FEATURE_AVS_CRYSTAL_SPEECH*/

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_tx_pcm_filter

DESCRIPTION
  This function will load a new set of TX PCM filter coefficients.

DEPENDENCIES
  This command is valid???

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed...

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_tx_pcm_filter
(
  const qdsp_cmd_pcm_filter_type *coefficients
);

/* <EJECT> */
#ifdef FEATURE_IIR_FILTER
/*===========================================================================

FUNCTION QDSP_CMD_SET_RX_PCM_IIR_FILTER (DMA Type Writable Information)

DESCRIPTION
  This function will load a new set of the QDSP PCM filter coefficients.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_rx_pcm_iir_filter (
  const qdsp_cmd_pcm_iir_filter_type *coefficients
);

#ifdef FEATURE_AVS_CRYSTAL_SPEECH
/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SET_TX_PCM_SECONDMIC_IIR_FILTER (DMA Type Writable Information)

DESCRIPTION
  This function will load a new set of the QDSP PCM filter coefficients for secondary mic.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_tx_pcm_secondmic_iir_filter (
  const qdsp_cmd_pcm_iir_filter_type *second_mic_coefficients
);
#endif /*FEATURE_AVS_CRYSTAL_SPEECH*/

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SET_TX_PCM_IIR_FILTER (DMA Type Writable Information)

DESCRIPTION
  This function will load a new set of the QDSP PCM filter coefficients.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_tx_pcm_iir_filter (
  const qdsp_cmd_pcm_iir_filter_type *coefficients
);
#endif /* FEATURE_IIR_FILTER */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_REGISTER

DESCRIPTION
  Registers with QDSP Services.
  Adds the current interrupt service routine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_register (
  /* Vocoder client type */
  qdsp_cmd_qdsps_client_type voc_client,
  /* QDSP Services Application ID */
  qdsp_app_type app_id,
  /* Pointer to the application's QDSP-to-ARM interrupt handler */
  qdsp_event_cb_type cb_ptr,
  /* Mask of events that the application needs to receive */
  uint16 event_mask
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_CLEAR_DSP_ISR

DESCRIPTION
  Clears the ISR registered with QDSP Services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_clear_dsp_isr (void);

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

          Q D S P 2   V R   E N G I N E  :  M O D E   A   C O M M A N D S

===========================================================================*/
/*===========================================================================

FUNCTION qdsp_cmd_vr_basic

DESCRIPTION
  This function will send a command to the DSP to perform VR Basic mode
  operation.

DEPENDENCIES
  This command is only valid when the DSP is in IDLE state.

RETURN VALUE
  QDSP_CMD_SUCCESS - The command was successful
  QDSP_CMD_FAILED  - The command failed

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_vr_basic ( uint16 vr_basic_control );
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION qdsp_cmd_start_vp

DESCRIPTION
  This function will tell the QDSP driver to play a voice prompt.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_vp_rx_playing
  qdsp_cmd_vp_tx_playing
  qdsp_cmd_get_vp_rx_frame
  qdsp_cmd_get_vp_tx_frame

===========================================================================*/
extern void qdsp_cmd_start_vp (
  qdsp_cmd_vp_path_enum_type    path,     /* Path to play VP to           */
  qdsp_cmd_get_vp_frame_fn_type   cb_func,    /* fn pointer to get frame data */
  qdsp_cmd_vp_mux_type            in_call,  /* If this is to be muxed incall  */
  qdsp_cmd_second_dec_config_type sec_dec_config /* Sec dec config if incall  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_end_vp

DESCRIPTION
  This function will tell the QDSP driver to stop playing a voice prompt.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_vp_rx_playing
  qdsp_cmd_vp_tx_playing
  qdsp_cmd_get_vp_rx_frame
  qdsp_cmd_get_vp_tx_frame

===========================================================================*/
extern void qdsp_cmd_end_vp ( qdsp_cmd_vp_path_enum_type path );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_pause_pb

DESCRIPTION
  This function will tell the QDSP driver to pause the voice playback.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void qdsp_cmd_pause_pb ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_resume_pb

DESCRIPTION
  This function will tell the QDSP driver to resume the voice playback.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_vp_rx_playing

===========================================================================*/
extern void qdsp_cmd_resume_pb ( void );

#endif /* FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_PHONE_VR
/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_vr_set_rate

DESCRIPTION
  This function sets the max rate and min rate for the voice recording
  and the utterance capture.

DEPENDENCIES
  FEATURE_PHONE_VR || FEATURE_VOICE_RECORD

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_vr_max_frame_rate
  qdsp_cmd_vr_min_frame_rate

===========================================================================*/
extern void qdsp_cmd_vr_set_rate (
  uint16   max_rate,
  uint16   min_rate
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SND_STATUS

DESCRIPTION
  This function is called by the Vocoder driver everytime Sound starts or
  stops playing a tone.  This function allows the Vocoder driver to handle
  talk over prompt feature in capture mode.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_snd_status (boolean snd_play_stat);


#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_VOICE_RECORD
/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_start_rec

DESCRIPTION
  This function will tell the QDSP driver to start the voice reording.

DEPENDENCIES
  FEATURE_VOICE_RECORD

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_min_frame_rate
  qdsp_cmd_max_frame_rate
  qdsp_cmd_rec_min_frame_rate
  qdsp_cmd_rec_max_frame_rate
  qdsp_cmd_rec_frame_fn
  qdsp_cmd_rec_dir

===========================================================================*/
extern void qdsp_cmd_start_rec (
  qdsp_cmd_rec_dir_enum_type    dir,      /* Path to record voice   */
  qdsp_cmd_rec_fn_type          cb_func,  /* fn pointer to callback */
  uint16                        min_rate,
  uint16                        max_rate
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_end_rec

DESCRIPTION
  This function will tell the QDSP driver to stop the voice reording.

DEPENDENCIES
  FEATURE_VOICE_RECORD

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_rec_frame_fn
  qdsp_cmd_rec_dir

===========================================================================*/
extern void qdsp_cmd_end_rec ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_pause_rec

DESCRIPTION
  This function will tell the QDSP driver to pause the voice reording.

DEPENDENCIES
  FEATURE_VOICE_RECORD

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_rec_dir_pause
  qdsp_cmd_rec_dir

===========================================================================*/
extern void qdsp_cmd_pause_rec ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_resume_rec

DESCRIPTION
  This function will tell the QDSP driver to resume the voice reording.

DEPENDENCIES
  FEATURE_VOICE_RECORD

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_min_frame_rate
  qdsp_cmd_max_frame_rate
  qdsp_cmd_rec_dir
  qdsp_cmd_rec_dir_pause

===========================================================================*/
extern void qdsp_cmd_resume_rec ( void );


#endif /* FEATURE_VOICE_RECORD */

/* <EJECT> */
#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */
#ifdef FEATURE_PHONE_VR
/* <EJECT> */

/*===========================================================================

     Q D S P 2   V R   E N G I N E  :  M O D E   B / C   F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION qdsp_cmd_vr_codec_config

DESCRIPTION
  This function will send a command to the DSP to configure the codec,
  PCM audio path selection and PCM format selection.

  If the VR software version number specified in the command is invalid, the
  DSP will report a VR Initialization error and reject all VR related
  commands.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  This command is only valid when the DSP is in INIT state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  Transition DSP to VR IDLE state if VR software version number is valid.

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_vr_codec_config
(
  const qdsp_cmd_codec_config_type *data,        /* Command data to send   */
  uint16                           vr_sw_version /* Software version       */
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_vr_prompt

DESCRIPTION
  This function will send a command to the DSP to transition the DSP to
  VR PROMPT state.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  This command is only valid when the DSP is in VR IDLE, VR PROMPT,
  VR CAPTURE or VR MATCH state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  Transition DSP to VR PROMPT state.

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_vr_prompt (void);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_capture_start

DESCRIPTION
  This function starts an utterance capture operation by sending a VR capture
  command to the DSP.

DEPENDENCIES
  This function depends on all VR mode B/C vocoder initialization taking place
  first.

  This command is only valid when the DSP is in VR PROMPT or VR CAPTURE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_capture_start
(
  uint16                 max_rate,           /* Max capture rate           */
  uint16                 min_rate,           /* Min capture rate           */
  boolean                talkover_capture,   /* Talk over prompt flag      */
  uint16                 vr_capture_control, /* Command data to send       */
  qdsp_cmd_endpt_cb_type cb_func             /* Function ptr to call back  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_capture_cont

DESCRIPTION
  This function sends a capture continue command to the DSP.

DEPENDENCIES
  This function depends on all VR mode B/C vocoder initialization taking place
  first.

  This command is only valid when the DSP is in VR PROMPT or VR CAPTURE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_capture_cont
(
  uint16                        num_frames_to_discard
   /* Discard this number of frames since they do not contain speech       */
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_capture_stop

DESCRIPTION
  This function stops an utterance capture operation.

DEPENDENCIES
  This function depends on all VR mode B/C vocoder initialization taking place
  first.

  This command is only valid when the DSP is in VR PROMPT or VR CAPTURE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_capture_stop
(
  const qdsp_cmd_capture_end_cmd_type *data      /* Command data to send   */
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_match_start

DESCRIPTION
  This function starts a VR match operation by sending a VR match
  command to the DSP.

  Special Note:

  While a match operation is in progress, the DSP is not configured to
  play voice prompts or capture.

DEPENDENCIES
  This function depends on all VR mode B/C vocoder initialization taking place
  first.

  This command is only valid when the DSP is in VR PROMPT or VR
  MATCH state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_match_start
(
  const qdsp_cmd_match_cmd_type *data        /* Command data to send       */
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_match_result

DESCRIPTION
  If a VR match result message is available from the QDSP2, this function
  converts DSP addresses to ARM addresses and returns the appropriate VR match
  result message data.

DEPENDENCIES
  This function depends on all VR mode B/C vocoder initialization taking
  place first.

RETURN VALUE
  Ptr to match data

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_match_result_type *qdsp_cmd_match_result (void);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_pattern_load

DESCRIPTION
  This function is called to send patterns to the DSP (via DMA) for a
  matching operation.

DEPENDENCIES
  The DMA address and length of pattern buffer are specified by the DSP.
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR MATCH state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_pattern_load
(
  uint16                      num_vr_patterns,
                              /* Number of pattern(s) loaded               */
  uint16                      *pattern_ptr
                              /* Addr of pattern(s) (DSP DMA address)      */
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_narrowb

DESCRIPTION
  This function configures the DSP for narrowband capture before a capture
  command is sent.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  This command is only valid when the DSP is in VR PROMPT or VR CAPTURE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_set_narrowb (void);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_bark_weight

DESCRIPTION
  This function configures the VR Bark Amp weight table in DSP memory.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/

extern qdsp_cmd_status_type qdsp_cmd_set_bark_weight
(
  const unsigned short *barkweight
);

#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */

#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#ifdef FEATURE_VOC_PCM_INTERFACE
/*===========================================================================

FUNCTION qdsp_cmd_register_pcm_input_client

DESCRIPTION
  Use this function to register a callback to feed PCM data on the RX input
  path to the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NONE

===========================================================================*/
void qdsp_cmd_register_pcm_input_client(
  qdsp_cmd_pcm_interface_type        interface,
  qdsp_cmd_pcm_interface_attrib_type *pcm_attrib,
  qdsp_cmd_pcm_client_input_fn_type  client_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_register_pcm_output_client

DESCRIPTION
  Use this function to register a callbacks to get PCM output from the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NONE

===========================================================================*/
void qdsp_cmd_register_pcm_output_client(
  qdsp_cmd_pcm_interface_type         interface,
  qdsp_cmd_pcm_interface_attrib_type  *pcm_attrib,
  qdsp_cmd_pcm_client_output_fn_type  client_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_pcm_modes

DESCRIPTION
  Sends commands to DSP to enable/disable PCM interfaces.

DEPENDENCIES
  Clients must register a callback for the PCM interfaces enabled (if any).

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_pcm_rx_input_enabled
  qdsp_pcm_rx_output_enabled
  qdsp_pcm_tx_output_enabled
  qdsp_pcm_tx_input_enabled

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_set_pcm_modes(
  uint32 pending_mode,
  qdsp_cmd_pcm_client_type pcm_client_id
);
#endif /* FEATURE_VOC_PCM_INTERFACE */

/* <EJECT> */
#if defined(FEATURE_PHONE_VR) || defined(FEATURE_VOICE_RECORD)
/*===========================================================================

FUNCTION QDSP_CMD_RESTORE_COMFORT_NOISE (DMA Type Writable Information)

DESCRIPTION
  This function enables or disables the QDSP comfort noise TX generator.

DEPENDENCIES
  This command is valid when the DSP is in IDLE, Vocoder or DFM state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_cmd_restore_comfort_noise ( void );

#endif /* defined(FEATURE_PHONE_VR) || defined(FEATURE_VOICE_RECORD) */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_EQUALIZER
/*===========================================================================

FUNCTION QDSP_CMD_CHECK_AUDFMT_EQUALIZER_RESET

DESCRIPTION
  This function will check the equalizer reset flag.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - equalizer is in reset state.
  FALSE - equalizer is not in reset state.

SIDE EFFECTS
  None

===========================================================================*/
boolean qdsp_cmd_check_equalizer_reset ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_equalizer

DESCRIPTION
  This function configures the QSynth Equalizer controls through DMA & DME.

DEPENDENCIES
  FEATURE_QSYNTH_ADRC

RETURN VALUE
  qdsp_cmd_status_type

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_equalizer
(
  const qdsp_cmd_equalizer_type *eq
);
#endif /* FEATURE_AUDFMT_EQUALIZER */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_SPECTRUM_ANALYZER
/* Callback for sending data to client */
typedef void (* qdsp_cmd_spectrum_analyzer_cb_type)(
  const uint16 *buffer
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_set_spectrum_analyzer

DESCRIPTION
  Enables/disables the spectrum analyzer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
qdsp_cmd_status_type qdsp_cmd_set_spectrum_analyzer
(
  qdsp_cmd_spectrum_analyzer_cb_type  sp_cb
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_spectrum_analyzer_config

DESCRIPTION
  Configures the spectrum analyzer for the current DSP image

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void qdsp_cmd_spectrum_analyzer_config
(
  uint16 interval
);

/*===========================================================================

FUNCTION qdsp_cmd_do_spectrum_analyzer

DESCRIPTION
  Process the spectrum analyzer interrupt for the current DSP image

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void qdsp_cmd_do_spectrum_analyzer (void);
#endif /* FEATURE_AUDFMT_SPECTRUM_ANALYZER */

/* <EJECT> */
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_QCONCERT
/*===========================================================================

FUNCTION qdsp_cmd_set_qconcert_config

DESCRIPTION
  This function is used to setup the qconcert control DMA values.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS or FAILURE

SIDE EFFECTS
  None

==========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_qconcert_config(
  const qdsp_cmd_qconcert_config_type *qconcert
);
#endif /* FEATURE_AUDFMT_QCONCERT */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_STF
/*===========================================================================

FUNCTION qdsp_cmd_set_audfmt_sidechain_filter

DESCRIPTION
  This function is used to setup the side chain filters

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS or FAILURE

SIDE EFFECTS
  None

==========================================================================*/
qdsp_cmd_status_type qdsp_cmd_set_audfmt_sidechain_filter
(
  const qdsp_cmd_audfmt_sidechain_filter_type *filter
);
#endif /* FEATURE_AUDFMT_STF */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_IIR_FILTER
/*===========================================================================

FUNCTION qdsp_cmd_set_audfmt_iir_filter

DESCRIPTION
  Configures the side chain filter

DEPENDENCIES
  None

RETURN VALUE
  QDSP_CMD_SUCCESS

SIDE EFFECTS
  None

==========================================================================*/
qdsp_cmd_status_type qdsp_cmd_set_audfmt_iir_filter
(
  const qdsp_cmd_audfmt_iir_filter_type *filter
);
#endif /* FEATURE_AUDFMT_IIR_FILTER */

#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_pp_register

DESCRIPTION
  Enables ARM Post Processing by registering/de registering the callback

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
qdsp_cmd_status_type qdsp_cmd_register_pp_cb (
  qdsp_cmd_arm_pp_cb_type pp_cb,
  qdsp_cmd_pp_type_type pp_type
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_pp_reset

DESCRIPTION
  Resets all the input and output buffers used for pp.

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS or FAILURE

SIDE EFFECTS
  None

==========================================================================*/
boolean qdsp_cmd_pp_reset(void);
#endif /* FEATURE_ARM_POST_PROCESSING || FEATURE_AVS_WB_PCM_RX_INTERFACE */

/* <EJECT> */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
/*===========================================================================

FUNCTION QDSP_CMD_PP_FREE_READ_BUFFER

DESCRIPTION
  Frees a buffer to the read pool. This will be called
DEPENDENCIES
  Buffer pool should have been previously initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void  qdsp_cmd_pp_free_read_buffer(uint16 buf_index);
#endif /* FEATURE_SBC_CODEC || FEATURE_SBC_DSP_CODEC ||
          FEATURE_AVS_WB_PCM_RX_INTERFACE */

#if defined(FEATURE_SBC_DSP_CODEC) && defined(FEATURE_AVS_I2SSBC)
/*===========================================================================

FUNCTION QDSP_CMD_SET_I2SSBC_SAMPLE_RATE

DESCRIPTION
  This function sets the sample rate for the i2s sbc interface

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void qdsp_cmd_set_i2ssbc_sample_rate(
  qdsp_cmd_i2ssbc_sample_rate_type sample_rate
);

#endif /* FEATURE_SBC_DSP_CODEC && FEATURE_AVS_I2SSBC */
#ifdef FEATURE_ARM_POST_PROCESSING
/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_pp_client_data

DESCRIPTION
  This is the function called by the client to pass the PP data back to
  the DSP

DEPENDENCIES
  None

RETURN VALUE
  SUCCESS or FAILURE

SIDE EFFECTS
  None

==========================================================================*/
qdsp_cmd_status_type qdsp_cmd_pp_client_data
(
  uint16    buf_length,
  uint16    *buffer,
  uint16    buf_index
);

#endif /* FEATURE_ARM_POST_PROCESSING */

/* <EJECT> */
#ifdef FEATURE_VOC_DTMF_DETECT
/* Enums associated with DTMF detection */
typedef enum {
  QDSP_CMD_DTMF_DETECTED_0,
  QDSP_CMD_DTMF_DETECTED_1,
  QDSP_CMD_DTMF_DETECTED_2,
  QDSP_CMD_DTMF_DETECTED_3,
  QDSP_CMD_DTMF_DETECTED_4,
  QDSP_CMD_DTMF_DETECTED_5,
  QDSP_CMD_DTMF_DETECTED_6,
  QDSP_CMD_DTMF_DETECTED_7,
  QDSP_CMD_DTMF_DETECTED_8,
  QDSP_CMD_DTMF_DETECTED_9,
  QDSP_CMD_DTMF_DETECTED_POUND,
  QDSP_CMD_DTMF_DETECTED_STAR,
  QDSP_CMD_DTMF_DETECTED_A,
  QDSP_CMD_DTMF_DETECTED_B,
  QDSP_CMD_DTMF_DETECTED_C,
  QDSP_CMD_DTMF_DETECTED_D,
  QDSP_CMD_DTMF_DETECTED_NONE
} qdsp_cmd_dtmf_detected_status_type;

typedef void (* qdsp_cmd_dtmf_detect_cb_type) (
  qdsp_cmd_dtmf_detected_status_type rx_status,
  qdsp_cmd_dtmf_detected_status_type tx_status
);

typedef enum {
  QDSP_CMD_DTMF_DETECT_DISABLED,
  QDSP_CMD_DTMF_DETECT_TX,
  QDSP_CMD_DTMF_DETECT_RX,
  QDSP_CMD_DTMF_DETECT_BOTH
} qdsp_cmd_dtmf_detect_cmd_type;

/*===========================================================================

FUNCTION QDSP_CMD_DTMF_DETECT

DESCRIPTION
  This function will enable/disable DTMF detection.

DEPENDENCIES
  This command is only valid when the DSP is in Vocoder or DFM state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_dtmf_detect (
  qdsp_cmd_dtmf_detect_cmd_type cmd,
  qdsp_cmd_dtmf_detect_cb_type  cb_func
);
#endif /* FEATURE_VOC_DTMF_DETECT */

/* <EJECT> */
#ifdef FEATURE_WMA
/*===========================================================================

FUNCTION qdsp_cmd_get_sample_rate_value

DESCRIPTION


DEPENDENCIES
  qdsp_cmd_sample_rate_values

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
uint16 qdsp_cmd_get_sample_rate_value(uint16 sample_rate_index);

/*===========================================================================

FUNCTION qdsp_cmd_adec_config_cmd

DESCRIPTION
  Set adec configuration params in DSP for WindowsMediaAudio song playback.

DEPENDENCIES
  None.

RETURN VALUE
  Success if the value configuration could be set
  Failure if the WindowsMedia Audio image was not available

SIDE EFFECTS
  None.

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_adec_config_cmd(
  const qdsp_cmd_adec_config_type *adec_config
);
#endif /* FEATURE_WMA */

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION qdsp_cmd_set_av_sync

DESCRIPTION
  Set AV Sync parameters

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_av_sync_info is set

==========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_av_sync (
  qdsp_cmd_av_sync_dec_enum_type dec_type,
  qdsp_cmd_av_sync_cb_type       av_sync_cb,
  uint32                         av_sync_interval,
  uint16                         sample_rate
);

/*===========================================================================

FUNCTION qdsp_cmd_set_decoder_in_use

DESCRIPTION
  set the decoder type currently in use in the qdsp

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_set_decoder_in_use is set

==========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_decoder_in_use(qdsp_cmd_decoder_inuse_type dec_type);
/*===========================================================================

FUNCTION qdsp_cmd_get_decoder_in_use

DESCRIPTION
  set the decoder type currently in use in the qdsp

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_set_decoder_in_use is set

==========================================================================*/
extern qdsp_cmd_decoder_inuse_type qdsp_cmd_get_decoder_in_use(void);
#endif /* FEATURE_AUDIO_FORMAT */

#if defined(FEATURE_GRAPH_ADPCM) || defined(FEATURE_VOC_ADPCM)
/*===========================================================================

FUNCTION QDSP_CMD_VOC_ADPCM_IS_SUPPORTED

DESCRIPTION
  This function returns true if vocoder-adpcm is supported under the currently
  loaded image, and false otherwise.

DEPENDENCIES
  This function depends on all initialization taking place first.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qdsp_cmd_voc_adpcm_is_supported (void);

/*===========================================================================

FUNCTION qdsp_cmd_voc_adpcm_send_frame

DESCRIPTION
  This function sends an ADPCM frame to graphics image.

DEPENDENCIES
  This function depends on all initialization taking place first.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qdsp_cmd_voc_adpcm_send_frame (void);

#ifdef FEATURE_GRAPH_ADPCM
/*===========================================================================

FUNCTION QDSP_CMD_GRAPH_ADPCM_SEND_FRAME

DESCRIPTION
  This function sends an ADPCM frame to graphics image.

DEPENDENCIES
  This function depends on all initialization taking place first.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_cmd_graph_adpcm_send_frame(void);
#endif /* FEATURE_GRAPH_ADPCM */
#endif /* FEATURE_GRAPH_ADPCM || FEATURE_VOC_ADPCM */

#ifdef FEATURE_SBC_DSP_CODEC
/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SBC_CONFIG (command 0x0310)

DESCRIPTION
  This function will send a command to the DSP to configure the SBC encoder.

DEPENDENCIES

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_sbc_config (
  const qdsp_cmd_sbc_codec_config_type *data
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_sbc_set_bitrate

DESCRIPTION
  This function will set the bitrate for the DSP based SBC encoder.

DEPENDENCIES

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_cmd_sbc_set_bitrate (uint32 bitrate);

#endif /* FEATURE_SBC_DSP_CODEC */

#ifdef FEATURE_QDJ_TIMEWARP
/* <EJECT> */
/*===========================================================================

FUNCTION
  qdsp_cmd_set_timewarp

DESCRIPTION
  This function will record the timewarp factor for the current frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_set_timewarp(
  boolean  enable,
  uint16   factor
);

#ifdef MSMAUD_VOC_4GV_VOIP
#error code not present
#endif /* MSMAUD_VOC_4GV_VOIP */
#endif /* FEATURE_QDJ_TIMEWARP */

#if defined(FEATURE_AVS_INCALL_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB)
#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
/* Audio+Vocoder mixer mode description.
 */

/*===========================================================================

FUNCTION qdsp_cmd_get_mixer_state

DESCRIPTION
  This function get the corresponding mixer image entities' state.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -- Get state successfully;
  FALSE -- Get state failed;

SIDE EFFECTS
  None
===========================================================================*/
extern boolean qdsp_cmd_get_mixer_state (
  sint15  *mixer_state
);

/*===========================================================================

FUNCTION qdsp_mixer_command

DESCRIPTION
  This function specifically sends command to mixer part.

DEPENDENCIES
  NONE.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_status_type  qdsp_mixer_command (
  uint16 *cmd_ptr
    /* Pointer to command packet to send. The first word of this packet
       must specify the length, in words, of the remainder of the command
       packet. */
);

/*===========================================================================

FUNCTION qdsp_cmd_mixer_adec_config

DESCRIPTION
  This function sends the audio configuration command for MIXER.

DEPENDENCIES
  This command is only valid when the DSP is in MIXER_ACTIVE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_mixer_adec_config
(
  qdsp_cmd_codec_config_type *data
);

/*===========================================================================

FUNCTION qdsp_cmd_mixer_voc_config (command 0x0301)

DESCRIPTION
  This function sends the vocoder configuration command for MIXER.

DEPENDENCIES
  This command is only valid when the DSP is in MIXER_ACTIVE state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_mixer_voc_config
(
  qdsp_cmd_codec_config_type *data
);

/*===========================================================================

FUNCTION qdsp_cmd_mixer_codec_config (command 0x0301)

DESCRIPTION
  Issues the mixer codec command to mixer part.

DEPENDENCIES
  This command is only valid when the DSP is in MIXER_IDLE, MIXER_Vocoder
  state.

RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_mixer_codec_config (
  qdsp_mixer_mode_type       mode,
  qdsp_cmd_codec_config_type *data
);
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

/*===========================================================================

FUNCTION qdsp_adec_command

DESCRIPTION
  Issues the adec command to audio part during incall state.

DEPENDENCIES
  NONE.

RETURN VALUE
   A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_status_type  qdsp_adec_command (
  uint16 *cmd_ptr
    /* Pointer to command packet to send. The first word of this packet
       must specify the length, in words, of the remainder of the command
       packet. */
);

/*===========================================================================

FUNCTION qdsp_cmd_get_adec_state

DESCRIPTION
  This function reads the adec state when the firmware is in VOCODER+ADEC state

DEPENDENCIES
  A firmware image that supports VOCODER+ADEC must be loaded.

RETURN VALUE
  QDSP_adecState

SIDE EFFECTS
  None

===========================================================================*/
extern sint15 qdsp_cmd_get_adec_state (void);
#endif /* FEATURE_AVS_INCALL_ADEC_PB || FEATURE_AVS_INCALL_MIXER_ADEC_PB */

/* <EJECT> */
#ifdef FEATURE_QSYNTH_COMMON
#ifndef FEATURE_GASYNTH_INTERFACE
/*===========================================================================

FUNCTION QDSP_CMD_QSYNTH_ADEC_MODE

DESCRIPTION
  This function will set the QSynth audio decoder type

DEPENDENCIES
  This command is only valid when the DSP is using QSYNTH image and should
  be issued before the first codec config command.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_qsynth_adec_mode ( uint16 type );
#endif /* FEATURE_GASYNTH_INTERFACE */

/*===========================================================================

FUNCTION QDSP_CMD_QSYNTH_EMULATE_ISR

DESCRIPTION
  This function manifests ARM-made qsynth interrupt

DEPENDENCIES
  This function should only called in the situation which QSYNTH is in SYNTH
  state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_qsynth_emulate_isr(void);

/*===========================================================================

FUNCTION QDSP_CMD_SET_DLS_CONFIG

DESCRIPTION
  This function will set the QSynth DLS configuration.

DEPENDENCIES
  This command is only valid when the DSP is using QSYNTH image.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_set_dls_config ( qdsp_cmd_dls_config_type *dls_config );

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SET_QSYNTH_VOL_MODE

DESCRIPTION
  This function sets the qysnth volume mapping configuration.

DEPENDENCIES
  This command is only valid when the DSP is using QSYNTH image.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_set_qsynth_vol_mode( uint16 vol_mode);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SET_HYBRID_MODE

DESCRIPTION
  This function will set the QSynth hybrid mode configuration.

DEPENDENCIES
  This command is only valid when the DSP is using QSYNTH image.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_set_hybrid_mode ( uint8 hybrid_mode );

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_PAUSE_MIDI_SEQ

DESCRIPTION
  This function will set the pause flag.

DEPENDENCIES
  This command is only valid when the DSP is using QSYNTH image.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_pause_midi_seq (
  uint32  sequence,
  boolean pause_flag
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_SET_VOL_MIDI_SEQ

DESCRIPTION
  This function sets the volume for indicated sequence.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_set_vol_midi_seq (
  uint16 sequence,
  uint16 volume
);

/*===========================================================================

FUNCTION qdsp_cmd_qsynth_cmd_dereg

DESCRIPTION
  This function deregisters the MIDI command callback

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void qdsp_cmd_qsynth_cmd_dereg ( void);
#endif /* FEATURE_QSYNTH_COMMON */

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

#ifdef FEATURE_AUDIO_PCR_SUPPORT
/*===========================================================================

FUNCTION qdsp_cmd_af_adjust_sample

DESCRIPTION
  Adjust the Sample
DEPENDENCIES
  None

RETURN VALUE
  QDSP_CMD_SUCCESS - command successfully sent.
  QDSP_CMD_FAILED  - Command failed, need retry.

SIDE EFFECTS
  None

==========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_af_adjust_sample
(
boolean       add_sample
);
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION qdsp_cmd_dual_mono_mode

DESCRIPTION
  Write Dual Mono mode.

DEPENDENCIES
  None

RETURN VALUE
  QDSP_CMD_SUCCESS - command successfully sent.
  QDSP_CMD_FAILED  - Command failed, need retry.

SIDE EFFECTS
  None

==========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_dual_mono_mode (
  uint16 mode
);

/*===========================================================================

FUNCTION qdsp_cmd_sbr_mode

DESCRIPTION
  Write SBR mode.

DEPENDENCIES
  None

RETURN VALUE
  QDSP_CMD_SUCCESS - command successfully sent.
  QDSP_CMD_FAILED  - Command failed, need retry.

SIDE EFFECTS
  None

==========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_sbr_mode (
  uint16 mode
);
#endif /*FEATURE_AUDIO_ISDBT_DUAL_MONO */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_send_voc_wav_data

DESCRIPTION
  Send voc wav data to the DSP

DEPENDENCIES
  None.

RETURN VALUE
  QDSP_CMD_SUCCESS.

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_send_voc_wav_data(
  qdsp_cmd_adec_cmd_type *adec_cmd );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_send_graph_adpcm_data

DESCRIPTION
  Send graph adpcm data to the DSP

DEPENDENCIES
  None.

RETURN VALUE
  QDSP_CMD_SUCCESS.

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_send_graph_adpcm_data(
  qdsp_cmd_adec_cmd_type *adec_cmd );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_send_voc_adpcm_data

DESCRIPTION
  Send voc adpcm data to the DSP

DEPENDENCIES
  None.

RETURN VALUE
  QDSP_CMD_SUCCESS.

SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_send_voc_adpcm_data(
  qdsp_cmd_adec_cmd_type *adec_cmd );

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_send_synth_adpcm_data

DESCRIPTION
  Send synth adpcm data to the DSP

DEPENDENCIES
  None.

RETURN VALUE
  QDSP_CMD_SUCCESS.


SIDE EFFECTS
  None.

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_send_synth_adpcm_data(
  qdsp_cmd_adec_cmd_type *adec_cmd );

#endif /* if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB) */
#ifdef FEATURE_AUDIO_CAMERA_CONCURRENCY
/*===========================================================================

FUNCTION QDSP_CMD_ADPCM_TUNES_QDSP2_ISR

DESCRIPTION
  This function processes the interrupt from the cam_audio_conc image

DEPENDENCIES
  This function depends on all vocoder initialization taking place first.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_cmd_adpcm_tunes_qdsp2_isr(void);
#endif

/*===========================================================================
FUNCTION qdsp_cmd_set_idle_metering

DESCRIPTION
Set qdsp variable idleMeterEnable.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_cmd_set_idle_metering(uint16 flag);


/*===========================================================================

FUNCTION qqdsp_cmd_vp3_config

DESCRIPTION
  Write SBR mode.

DEPENDENCIES
  None.

RETURN VALUE
  QDSP_CMD_SUCCESS - command  successfully sent.
  QDSP_CMD_FAILED  - Command failed, need retry.


SIDE EFFECTS
  None

===========================================================================*/
extern qdsp_cmd_status_type qdsp_cmd_vp3_config(qdsp_cmd_vp3_config_type *data);



/*===========================================================================

FUNCTION qdsp_cmd_set_slowtalk_config (DMA Type Writable Information)

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  A value of qdsp_cmd_status_type indicating whether the command
  was done.

SIDE EFFECTS
  None

===========================================================================*/
qdsp_cmd_status_type qdsp_cmd_set_slowtalk_config (
  boolean                                  slowtalk_enable,
  qdsp_cmd_slowtalk_static_params_flag     slowtalk_static_params_flag,
  const qdsp_cmd_slowtalk_parms_type       *pSlowTalkParams
);

#endif /* QDSPCMD_H */

